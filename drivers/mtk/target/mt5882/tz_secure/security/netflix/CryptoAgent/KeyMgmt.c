/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * This is the key management module for the cryptoAgent. We currently manage
 * two types of (cryptographic) keys:
 *
 * 		- "named" keys (pre-provisioned)
 *
 * 		- dynamic keys (loaded at runtime)
 *
 * Our internal key objects are closely related to the W3C webcrypto working
 * group's "Key" object (and we try to influence them when we can).
 *
 * Key objects have a number of important attributes:
 *
 * 	- type: secret (i.e. symmetric crypto), public, or private
 *
 * 	- key usage: encrypt, decrypt, sign, unwrap, etc.
 *
 * 	- Algorithm: the cryptographic Algorithm this key is intended for. Note that
 *          	  a key may only have one Algorithm.
 *
 *  - access policy: who can access the key? Currently, our access policy is quite
 *                   simple-minded, but this could change. For now, each key has a
 *                   128-bit client ID associated with it. We assume that the TEE
 *                   communications agent authenticates applications, and we trust
 *                   the comm agent to tell us the correct client ID with each
 *                   operation request. If the passed client ID is incorrect, the
 *                   operation is not allowed.
 *
 *  Note that "extractable" (a webcrypto key attribute) is also related to policy,
 *  but rather than maintaining a separate field for this, use one of the keyusage
 *  bits.
 *
 *  You can refer to keys in 2 ways: by name, and by "handle". Only pre-provisioned
 *  keys have names (e.g. "kpe", "kph", "kpw"), so only they can be referred to by
 *  name. The only operation you can do with a key name is get its handle. For all
 *  other operations, you must know the key handle.
 */

#include "TeeDebug.h"
#include "TeeTypes.h"
#include "KeyMgmt.h"
#include "CryptoAgent.h"
#include "StorageAgent.h"
#include "TeeInternalDpi.h"
#include "TeeAesWrapperLibTom.h"
#include "TeeAesCipherLibTom.h"
#include "b64.h"


//*********************  Constants definitions *****************************
// maximum length of ESN
#define caMAX_ESN_LEN			42

// we currently only support Netflix application keys (Kpe, Kph, Kpw, APKP) as named keys
#define caMAX_NAMED_KEYS		4

// Maximum keys (including named keys) in the key store.
// TODO: Estimate maximum number of keys allowed in the key store.
#define caMAX_KEYS 				64

// Maximum supported key name length
// XXX - this should be defined in the TEE API (CryptoTypes.h?)
#define caMAX_KEY_NAME_LEN 		16

// maximum supported key bits length
//TBD: 256 is enough for kpe/kph keys, but need to use this huge length for app boot key!!
#define caMAX_KEY_BIT_LEN		4096
#define caMAX_KEY_BYTE_LEN		(caMAX_KEY_BIT_LEN/8)

// size of pre-allocate buffers for storage agent InMsg and OutMsg.
// this is for our TPI/IPC hack.
#define nfSA_REQ_BUF_SIZE 		256 // way bigger than we need
#define nfSA_RSP_BUF_SIZE 		512 // way bigger than we need

// cryptoAgent has its own device unique keys, define their sizes
#define caAES_128_KEY_SIZE_BYTES		16
#define caAES_128_KEY_SIZE_BITS			128
#define caHMAC_SHA256_KEY_SIZE_BYTES	32
#define caHMAC_SHA256_KEY_SIZE_BITS		256

	// TBD - need high quality value here = f(MAX_KEY_SIZE_BYTES)
#define caCRYPT_WORKBUF_SIZE 512


//*********************  Struct and Enum definitions *****************************

// object to store the device ESN
uint8_t		caESN[caMAX_ESN_LEN];
uint32_t	caEsnLength;

/*
 * We currently only support one TEE client (the NRDP reference app), but once
 * we move this to a real TEE implementation, we can support more than one. Keys
 * should be associated with a specific client ID. We do this by creating an internal
 * list of known client IDs, and then including the index into this list with each
 * key object.
 */
typedef enum
{
	caCID_INDEX_NRDP_APP,

	// leave this last!!!
	caCID_INDEX_INVALID
} caClientIdIndex_t;

static nfTeeClientId_t	caClientIdList[caCID_INDEX_INVALID];

/*
 *  There are currently 3 tables associated with keys:
 *
 *   - the client ID list (caClientIdList)
 *   - the named key store (caNamedKeyStore)
 *   - the key store (caKeyStore)
 *
 *  These tables implement a simple relational database. The named key store and
 *  client ID list are populated during initialization. The named key store entries
 *  each contain an index into the client id list (always the same value for now, since
 *  we only have one client, but this will change in systems with multiple clients).
 *
 *  When a named key is actually loaded into the key store, a handle is assigned, and
 *  this handle is present in both the named key store key object and in the key store
 *  object. This allows us to quickly locate named keys that have already been loaded.
 *
 *  The key store contains "live" key objects, and these may be added in one of 3
 *  ways:
 *
 *  	- via a loadNamedKey call
 *  	- as a result of key derivation (unwrap or DH)
 *  	- via an importKey call
 *
 *  Note that we have a limited number of slots in the key store, and we have to deal
 *  with the case where N + 1 keys are loaded. In this case, we expect the client to
 *  evict a key, replacing it with the newly loaded key. If the client attempts to add
 *  a key when the key store is full, the operation will fail.
 *
 *  The REE will always refer to keys by handle, so every time there is an operation
 *  request, we need to look up the key using the handle. The brain-dead solution to
 *  maximize performance is to simply use the array index as the handle. This may change
 *  at some point in the future (so do not assume this is the case!), but for now, this
 *  is what we do.
 *
 */
// Named Key object that will be stored in named keys store
typedef struct
{
	uint32_t				KeyNameLength;
	uint8_t					KeyName[caMAX_KEY_NAME_LEN]; // NB: not NULL-terminated
	uint32_t				KeyLength;
	uint8_t					KeyBits[caMAX_KEY_BYTE_LEN];
	nfKeyFormat_t			KeyFormat;
	nfKeyUsageFlagsWord_t	KeyUsageFlags;
	nfCryptoAlgorithm_t		Algorithm;
	nfKeyType_t				KeyType;
	nfCryptoKeyHandle_t		KeyHandle; // we'll assign a key handle when asked
} caNamedKey_t;

// index into caNamedKeyStore for known keys...
enum
{
	caNK_INDEX_KPE = 0,
	caNK_INDEX_KPH,
	caNK_INDEX_KPW,

	// leave this last!
	caNK_INDEX_INVALID
} caNamedKeyIndex_t;


// key object that will be stored in the key store.
typedef struct
{
	int						InUse;
    nfKeyType_t				KeyType;
    nfKeyContext_t			KeyContext;
    nfCryptoAlgorithm_t 	Algorithm;
    nfKeyUsageFlagsWord_t 	KeyUsageFlags;
    caClientIdIndex_t		ClientIdIndex;
	caIncrementalCryptoContext_t // defined in KeyMgmt.h
    						CryptoContext;
} caKeyObject_t;

/*
 * This is the format of a "sealed" key. We use this to "persist" keys,
 * and then to reload them later. Note that it is a variable length
 * structure that ends in HMAC_SHA256; We encrypt/hmac this with our
 * (device-unique) sealing keys, and then b64-encode it. We use the
 * following keys for these operations:
 *
 * AES-128-CBC: caInternalAesKey
 * HMAC-SHA256: caInternalHmacKey
 *
 * So, sealed key construction looks like this:
 *
 * {Version} {16-byte random IV}
 * ------------------------------------------------------+
 *  { nfSealedKeyHeader_t }                              | This portion is AES-128-CBC encrypted
 *  { padding if sizeof(nfSealedKeyHeader) % 16 != 0 }   | using the pre-pended random IV.
 * ------------------------------------------------------+
 * {HMAC-SHA256 of IV + encrypted data}
 *
 * The whole thing (IV + encrypted data + HMAC) is b64-encoded. So, when the TEE is asked
 * to load a sealed key, it does the following:
 *
 *	- sanity-check size
 *		- reject if less than MIN_IV_SIZE + min_size(nfSealedKeyHeader_t) + sizeof(HMAC-SHA256)
 * 	- b64 decode the data
 * 		- reject if not well-formed b64
 * 	- locate the HMAC value, confirm HMAC over IV + encrypted data
 * 		- reject if this fails
 * 	- using IV, decrypt data, remove padding
 * 		- reject if padding is invalid
 * 	- verify Magic{Hi,Lo}
 * 		- reject if unrecognized magic
 * 	- verify Version
 * 		- reject if unsupported version
 * 	- sanity-check keytype, alg, key usage, bit count
 * 		- reject if anything is funky
 * 	- (attempt to) load key, assign handle
 *  	- reject if no slots available (user needs to remove key first)
 * 	- return key handle to REE
 */
static uint32_t caSealedKeyVersion = 0x00000001;

typedef struct
{
#define nfSEALEDKEY_MAGIC_HI	0xF001F001
#define nfSEALEDKEY_MAGIC_LO	0xBADDF00D
	uint32_t				MagicHi; // F001F001
	uint32_t				MagicLo; // BADDF00D
	nfKeyType_t				KeyType;
	nfCryptoAlgorithm_t		Algorithm;
    nfKeyUsageFlagsWord_t 	KeyUsageFlags;
	uint32_t				KeyBitCount; // how many bits in this key?
//-------------------------------------------------- 28 bytes at this point
	uint8_t					KeyBits[/* KeyBitCount/8 + (KeyBitCount % 8 ? 1:0) */];
} nfSealedKeyHeader_t;

#define nfSEALED_KEY_MIN_LENGTH (sizeof(uint32_t) + caAES_128_KEY_SIZE_BYTES + sizeof(nfSealedKeyHeader_t) + caAES_128_KEY_SIZE_BYTES + caHMAC_SHA256_KEY_SIZE_BYTES)
#define nfUNSEALED_KEY_MIN_LENGTH (sizeof(nfSealedKeyHeader_t) + caAES_128_KEY_SIZE_BYTES)

//********************* Internal variables *********************************

// named key store, loaded at init time
caNamedKey_t caNamedKeyStore[caMAX_NAMED_KEYS] = {  { 0, {0}, 0, {0}, 0, 0, 0, 0, 0 } };

// Key store, loaded via caImportKey
caKeyObject_t caKeyStore[caMAX_KEYS] = {  { 0, 0, {0,.ctxt.rawContext={0,{0}}}, 0, 0, 0, .CryptoContext={0,caICCS_UNINIT,0,NULL,NULL} } };

// starting/ending index of caKeyStore
static const int caKeyStartIndex = 0;
static const int caKeyEndIndex = caMAX_KEYS - 1;
static const nfCryptoKeyHandle_t caKeyStartHandle = 1;
static const nfCryptoKeyHandle_t caKeyEndHandle = caMAX_KEYS;

// invalid key handle and index
static const nfCryptoKeyHandle_t caKeyInvalidHandle = (caMAX_KEYS + 1);
static const int caKeyInvalidIndex = (caMAX_KEYS + 1);

/*
 * In a "real" implementation, this agent will have an IPC interface
 * to the storage agent, and will use that to request data (e.g. keys).
 * Since we are using a library, and since the storage agent already
 * has a message-based interface, we'll re-use that here. We need to
 * pre-allocate buffers for InMsg and OutMsg.
 */
static uint8_t caStorageRequestBuf[nfSA_REQ_BUF_SIZE];
static uint8_t caStorageResponseBuf[nfSA_RSP_BUF_SIZE];

// NTBA RO client ID, this ID must match NTBA_RO client ID in StorageAgent/StorageAgentInternals.h
nfTeeClientId_t caMyClientId =
 {0x9d,0x4a,0x88,0xc4,0x32,0x62,0x78,0x7c,0x8f,0x27,0xba,0xed,0xa8,0x71,0x61,0x85};

// NRD client ID, this will be read from storage agent at init time
nfTeeClientId_t caNrdClientId;

/*
 * cryptoAgent has its own device unique keys. In the final implementation, we
 * should re-name these variables to something less obvious, and we may want to
 * consider obfuscating the fact that these are crypto keys. But for now,
 * let's assume the TEE is secure :-)
 */
static uint8_t caInternalKey[caAES_128_KEY_SIZE_BYTES];
static uint8_t caInternalAesKey[caAES_128_KEY_SIZE_BYTES];
static uint8_t caInternalHMACKey[caHMAC_SHA256_KEY_SIZE_BYTES];


//*********************  Private function declarations *****************************

/*********************************************************************
 * @function caClientIdMatchesKeyHandle
 *
 * @brief validate that key at this handle belongs to this ClientId
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyTableIndex - index in key table
 *
 * @returns 1 if correct client ID, 0 otherwise
 *
 *********************************************************************/
static nfBoolean_t caClientIdMatchesKeyHandle( nfTeeClientId_t	ClientId,
										 	 	int				KeyTableIndex )
{
	unsigned int ClientIdMatches;

	// don't mess with me.
	//assert( caKeyStore[KeyTableIndex].ClientIdIndex < caCID_INDEX_INVALID );

	ClientIdMatches = ( teeMemcmp( ClientId, caClientIdList[caKeyStore[KeyTableIndex].ClientIdIndex],
									sizeof(nfTeeClientId_t) ) == 0 );

	return ( ClientIdMatches ? nfBT_TRUE : nfBT_FALSE );

}/* end caClientIdMatchesKeyHandle */

/*********************************************************************
 * @function caGetClientIdIndex
 *
 * @brief return client id index in caClientIdList (if valid)
 *
 * @param[in] ClientId - the TEE client ID
 *
 * @returns index if correct client ID, -1 otherwise
 *
 *********************************************************************/
static int caGetClientIdIndex( nfTeeClientId_t	ClientId )
{
	int i;

	// don't mess with me.
	//assert( ClientId );

	for ( i = 0; i < caCID_INDEX_INVALID; ++ i )
	{
		if ( teeMemcmp( ClientId, caClientIdList[i], sizeof(nfTeeClientId_t) ) == 0 )
			return i;
	}

	// if we get here, we didn't find it.
	CRYPTODBG("%s: ClientId not found...\n", __FUNCTION__ );

	return -1;

}/* end caClientIdMatchesKeyHandle */

/*********************************************************************
 * @function caAllocateKeyHandle
 *
 * @brief allocate the next available key Handle in caKeyStore
 * 		  NOTE: this function never returns 0
 *
 *
 * @returns KeyHandle
 *
 *********************************************************************/
static nfCryptoKeyHandle_t caAllocateKeyHandle()
{
    int nextIndex;

    for ( nextIndex = caKeyStartIndex; nextIndex <= caKeyEndIndex; nextIndex++ )
    {
        if (caKeyStore[nextIndex].InUse == 0)
        {
        	caKeyStore[nextIndex].InUse = 1;
            return (nfCryptoKeyHandle_t) (nextIndex + caKeyStartHandle);
        }
    }

    // if we get here, no handles are available...
    return (nfCryptoKeyHandle_t) caKeyInvalidHandle;

}/* end caAllocateKeyHandle */

/*********************************************************************
 * @function caGetHandleFromIndex
 *
 * @brief gets the handle of key in caKeyStore for the given index
 *
 * @param[in] Index - Index of the key
 *
 * @returns handle of the key, -1 on error
 *
 *********************************************************************/
static nfCryptoKeyHandle_t caGetHandleFromIndex(int Index)
{
	if ( ( Index < 0 ) || ( Index >= (int) (caKeyEndHandle - caKeyStartHandle) ))
		return -1;

	return (nfCryptoKeyHandle_t) caKeyStartHandle + Index;

}/* end caGetHandleFromIndex */

/*********************************************************************
 * @function caGetIndexFromHandle
 *
 * @brief gets the index of key in caKeyStore for the given KeyHandle
 *
 * @param[in] KeyHandle - handle of the key
 *
 * @returns index of the key, -1 on error
 *
 *********************************************************************/
static int caGetIndexFromHandle(nfCryptoKeyHandle_t KeyHandle)
{
	if ((KeyHandle < caKeyStartHandle) || (KeyHandle > caKeyEndHandle))
	{
		return (int) caKeyInvalidIndex;
	}
	return (int) (KeyHandle - caKeyStartHandle);
}

/*********************************************************************
 * @function caDeallocateKeyHandle
 *
 * @brief deallocate this key Handle in caKeyStore
 *
 * @param[in] Handle - key handle to deallocate
 *
 * @returns 0 if everything okay, -1 if key handle not allocated
 *
 *********************************************************************/
static int caDeallocateKeyHandle(nfCryptoKeyHandle_t Handle)
{
    int Index;

    if ( ( Index = caGetIndexFromHandle(Handle) ) != caKeyInvalidIndex )
    {
        if (caKeyStore[Index].InUse == 1)
        {
        	caKeyStore[Index].InUse = 0;
        	return 0;
        }
    }

    // if we get here, handle is invalid or not in use...
    return -1;

}/* end caDeallocateKeyHandle */

/*********************************************************************
 * @function caReconcileAlgVsUsage
 *
 * @brief make sure usage and Algorithm are consistent
 *
 * @param[in] Algorithm - the crypto Algorithm
 * @param[in] KeyUsage - the key usage flags word
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static int caReconcileAlgVsUsage(const nfCryptoAlgorithm_t Algorithm, const nfKeyUsageFlagsWord_t KeyUsage)
{
	nfKeyUsageFlagsWord_t AllowedUsages;

    // empty usage means default usage, so always ok
    if (!KeyUsage)
        return 1;

    switch (Algorithm)
    {
		case nfCA_HMAC:
		case nfCA_RSASSA_PKCS1_V1_5:
			AllowedUsages = (nfKeyUsageFlagsWord_t)(nfKUF_SIGN | nfKUF_VERIFY | nfKUF_EXTRACTABLE);
			break;
		case nfCA_AES_CBC:
			AllowedUsages = (nfKeyUsageFlagsWord_t)
								(nfKUF_ENCRYPT | nfKUF_DECRYPT | nfKUF_UNWRAP | nfKUF_EXTRACTABLE);
			break;
		case nfCA_RSAES_PKCS1_V1_5:
		case nfCA_AES_GCM:
		case nfCA_AES_CTR:
			AllowedUsages = (nfKeyUsageFlagsWord_t) (nfKUF_ENCRYPT | nfKUF_DECRYPT | nfKUF_EXTRACTABLE);
			break;
		case nfCA_RSA_OAEP:
			AllowedUsages = (nfKeyUsageFlagsWord_t)
				(nfKUF_ENCRYPT | nfKUF_DECRYPT | nfKUF_WRAP | nfKUF_UNWRAP | nfKUF_EXTRACTABLE);
			break;
		case nfCA_AES_KW:
			AllowedUsages = (nfKeyUsageFlagsWord_t)(nfKUF_WRAP | nfKUF_UNWRAP | nfKUF_EXTRACTABLE);
			break;
		default:
			return 0;
    }/* end switch(Algorithm) */

    return !(KeyUsage & ~(AllowedUsages));
} /* end caReconcileAlgVsUsage */


/*********************************************************************
 * @function caInsertKey
 *
 * @brief ubsert keys into key table
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in[ KeyHandle - the key handle
 * @param[in[ KeyContextPtr - pointer to (preallocated) key context
 * @param[in[ KeyType - the key type
 * @param[in[ Algorithm - the Algorithm
 * @param[in[ KeyUsageFlags - key usage flags
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static nfTeeCryptoResult_t caInsertKey( nfTeeClientId_t			ClientId,
 	 									int 					KeyHandle,
 									    nfKeyContext_t 			*KeyContextPtr,
 									    nfKeyType_t 			KeyType,
 									    nfCryptoAlgorithm_t 	Algorithm,
 									    nfKeyUsageFlagsWord_t	KeyUsageFlags )
{
	// XXX - error checking?

	int	ClientIdIndex = -1;

	int index = KeyHandle - caKeyStartHandle;

	// make sure this client Id is one we know about
	if ( ( ClientIdIndex = caGetClientIdIndex( ClientId ) ) < 0 )
	{
		CRYPTODBG("%s: invalid client id.\n", __FUNCTION__);
		return nfTCR_UNKOWN_CLIENTID;
	}

	caKeyStore[index].Algorithm 		= Algorithm;
	caKeyStore[index].KeyType 			= KeyType;
	caKeyStore[index].KeyUsageFlags		= KeyUsageFlags;
	caKeyStore[index].KeyContext  		= *KeyContextPtr;
	caKeyStore[index].InUse 			= 1;
	caKeyStore[index].ClientIdIndex		= ClientIdIndex;

    return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caInsertKey */


/*********************************************************************
 * @function caImportNamedKeys
 *
 * @brief load pre-provisioned (named) keys
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static nfTeeCryptoResult_t caImportNamedKeys()
{
    nfTeeStorageSimpleReadRsp_t *RspPtr;
    nfTeeStorageSimpleReadReq_t *ReqPtr;
    uint32_t					Result;
    nfTeeClientId_t				ClientId;
    uint32_t					MsgLength = 0;

    /*
      * Request ESN from storage agent. Use existing message interface, but partners will
      * have to replace this with their internal TEE IPC mechanism.
      *
      * We need to do the following:
      *
      * 	- construct read request in caStorageRequestBuf
      * 	- call storageAgentDispatchMsg
      * 	- if successful
      * 		- extract ESN from caStorageResponseBuf
      * 		- store it in ESN object
      *  - else
      *  	- bail out, this is fatal
      */

     INITDBG("%s: loading ESN\n", __FUNCTION__ );
     // construct read request
     ReqPtr = (nfTeeStorageSimpleReadReq_t *) caStorageRequestBuf;
     ReqPtr->RequestedKey = teeHtonl(ntbaAGENT_KEY_ESN);

     // For this one, we need to use our own client ID
     (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

     // call storageAgent
     Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
     								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
     								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
     								  &MsgLength, ClientId );
     if ( Result != nfTGR_SUCCESS )
     {
     	INITDBG("%s: failed to retrieve ESN (error: %08x)\n", __FUNCTION__, Result);
     	return Result;
     }

     /*
      * Because we use the standard message interface
      * to storageAgent, the response has this structure:
      *
      * typedef struct
 	 *	{
 	 *		nfTeeClientId_t	ClientId; // client id associated with this data
 	 *		uint32_t		DataLength;
 	 *		uint8_t			Data[];
 	 *	} nfTeeStorageSimpleReadRsp_t;
      *
      * Ignore the nfTeeApiRspHdr_t at the beginning and go for the gold.
      *
      */
     RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;

     // now, copy key
     caEsnLength = teeNtohl(RspPtr->DataLength);
     INITDBG("%s: ESN Length is %d\n", __FUNCTION__, caEsnLength);
     (void) teeMemcpy(caESN, RspPtr->Data, caEsnLength);

    /*
     * Request Kpe from storage agent. Use existing message interface, but partners will
     * have to replace this with their internal TEE IPC mechanism.
     *
     * We need to do the following:
     *
     * 	- construct read request in caStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract Kpe from caStorageResponseBuf
     * 		- build key object
     *  - else
     *  	- bail out, this is fatal
     */

    INITDBG("%s: loading Kpe\n", __FUNCTION__ );
    // construct read request
    ReqPtr->RequestedKey = teeHtonl(ntbaAGENT_KEY_KPE);

    // For this one, we need to use our own client ID
    (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	INITDBG("%s: failed to retrieve Kpe (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    // set key name and name length (don't assume string functions)
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPE].KeyName, nfTEE_PSK_KPE_NAME, nfTEE_PSK_KPE_NAME_LEN );
    caNamedKeyStore[caNK_INDEX_KPE].KeyNameLength = nfTEE_PSK_KPE_NAME_LEN;

    /*
     * Because we use the standard message interface
     * to storageAgent, the response has this structure:
     *
     * typedef struct
	 *	{
	 *		nfTeeClientId_t	ClientId; // client id associated with this data
	 *		uint32_t		DataLength;
	 *		uint8_t			Data[];
	 *	} nfTeeStorageSimpleReadRsp_t;
     *
     */
    RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;

    // now, copy key
    INITDBG("%s: KeyLength is %d\n", __FUNCTION__, teeNtohl(RspPtr->DataLength) );

    caNamedKeyStore[caNK_INDEX_KPE].KeyLength = teeNtohl(RspPtr->DataLength);
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPE].KeyBits,
    				RspPtr->Data,
    				caNamedKeyStore[caNK_INDEX_KPE].KeyLength );
#if 0
    INITDBG("%s: here's KPE: \n", __FUNCTION__);
    teePrintHexDump(RspPtr->Data, 16);
#endif
    // set key usage flags
    caNamedKeyStore[caNK_INDEX_KPE].KeyFormat = nfKF_RAW;
    caNamedKeyStore[caNK_INDEX_KPE].KeyUsageFlags = nfKUF_ENCRYPT | nfKUF_DECRYPT;
    caNamedKeyStore[caNK_INDEX_KPE].Algorithm = nfCA_AES_CBC;
    caNamedKeyStore[caNK_INDEX_KPE].KeyType = nfKT_SECRET;
    caNamedKeyStore[caNK_INDEX_KPE].KeyHandle = caKeyInvalidHandle;

    /*
     * Request Kph from storage agent. Use existing message interface, but partners will
     * have to replace this. We need to do the following:
     * 	- construct read request in caStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract Kph from caStorageResponseBuf
     * 		- build key object
     *  - else
     *  	- bail out, this is fatal
     */
    INITDBG("%s: loading Kph\n", __FUNCTION__ );

    // construct read request
    ReqPtr->RequestedKey = teeHtonl(ntbaAGENT_KEY_KPH);

    // For this one, we need to use our own client ID
    (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	CRYPTODBG("%s: failed to retrieve Kph (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    // set key name and name length (don't assume string functions)
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPH].KeyName, nfTEE_PSK_KPH_NAME, nfTEE_PSK_KPH_NAME_LEN );
    caNamedKeyStore[caNK_INDEX_KPH].KeyNameLength = nfTEE_PSK_KPH_NAME_LEN;

    // now, copy key
    RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;
    INITDBG("%s: KeyLength is %d\n", __FUNCTION__, teeNtohl(RspPtr->DataLength) );
    caNamedKeyStore[caNK_INDEX_KPH].KeyLength = teeNtohl(RspPtr->DataLength);
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPH].KeyBits,
    				RspPtr->Data,
    				caNamedKeyStore[caNK_INDEX_KPH].KeyLength );
#if 0
    INITDBG("%s: here's KPH: \n", __FUNCTION__);
    teePrintHexDump(RspPtr->Data, 16);
#endif
    // set key usage flags
    caNamedKeyStore[caNK_INDEX_KPH].KeyFormat = nfKF_RAW;
    caNamedKeyStore[caNK_INDEX_KPH].KeyUsageFlags = nfKUF_SIGN | nfKUF_VERIFY;
    caNamedKeyStore[caNK_INDEX_KPH].Algorithm = nfCA_HMAC;
    caNamedKeyStore[caNK_INDEX_KPH].KeyType = nfKT_SECRET;
    caNamedKeyStore[caNK_INDEX_KPH].KeyHandle = caKeyInvalidHandle;

    /*
     * Request Kpw from storage agent. Use existing message interface, but partners will
     * have to replace this. We need to do the following:
     * 	- construct read request in caStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract Kpw from caStorageResponseBuf
     * 		- build key object
     *  - else
     *  	- bail out, this is fatal
     */
    INITDBG("%s: loading Kpw\n", __FUNCTION__ );
    // construct read request
    ReqPtr->RequestedKey = teeHtonl(ntbaAGENT_KEY_KPW);

    // For this one, we need to use our own client ID
    (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	CRYPTODBG("%s: failed to retrieve Kpe (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    // set key name and name length (don't assume string functions)
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPW].KeyName, nfTEE_PSK_KPW_NAME, nfTEE_PSK_KPW_NAME_LEN );
    caNamedKeyStore[caNK_INDEX_KPW].KeyNameLength = nfTEE_PSK_KPW_NAME_LEN;

    // now, copy key
    RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;
    caNamedKeyStore[caNK_INDEX_KPW].KeyLength = teeNtohl(RspPtr->DataLength);
    (void) teeMemcpy(caNamedKeyStore[caNK_INDEX_KPW].KeyBits,
    				RspPtr->Data,
    				caNamedKeyStore[caNK_INDEX_KPW].KeyLength );
    INITDBG("%s: KeyLength is %d\n", __FUNCTION__, caNamedKeyStore[caNK_INDEX_KPW].KeyLength );
#if 0
    INITDBG("%s: here's KPW: \n", __FUNCTION__);
    teePrintHexDump(RspPtr->Data, 16);
#endif
    // set key usage flags
    caNamedKeyStore[caNK_INDEX_KPW].KeyFormat = nfKF_RAW;
    caNamedKeyStore[caNK_INDEX_KPW].KeyUsageFlags = nfKUF_UNWRAP;
    caNamedKeyStore[caNK_INDEX_KPW].Algorithm = nfCA_AES_KW;
    caNamedKeyStore[caNK_INDEX_KPW].KeyType = nfKT_SECRET;
    caNamedKeyStore[caNK_INDEX_KPW].KeyHandle = caKeyInvalidHandle;

    /*
     * Request NRD client Id from storage agent. Use existing message interface, but partners will
     * have to replace this. We need to do the following:
     * 	- construct read request in caStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract client Id from caStorageResponseBuf
     * 		- store client Id in caClientIdList
     *  - else
     *  	- bail out, this is fatal
     */
    INITDBG("%s: loading NRD client ID\n", __FUNCTION__ );

    // construct read request
    ReqPtr->RequestedKey = teeHtonl(teeAPPID_KEY_NRD);

    // For this one, we need to use our own client ID
    (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	CRYPTODBG("%s: failed to retrieve NRD client ID (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    // copy client id
    RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;
    INITDBG("%s: ClientId DataLength is %d (we expect 16)\n", __FUNCTION__, teeNtohl(RspPtr->DataLength) );
    (void) teeMemcpy( caClientIdList[caCID_INDEX_NRDP_APP], RspPtr->Data, teeNtohl(RspPtr->DataLength) );
#if 0
    INITDBG("%s: dumping NRD cient ID...\n", __FUNCTION__);
    (void) teePrintHexDump( (uint8_t *) caClientIdList[caCID_INDEX_NRDP_APP], sizeof(nfTeeClientId_t));
#endif
    /*
     * Finally, load cryptoAgent's internal key, create AES/HMAC-SHA256 keys.
     */

    INITDBG("%s: loading caInternalKey\n", __FUNCTION__ );
    // construct read request
    ReqPtr->RequestedKey = teeHtonl(ntbaAGENT_KEY_INTERNAL);

    // For this one, we need to use our own client ID
    (void) teeMemcpy( ClientId, caMyClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  caStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  caStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	CRYPTODBG("%s: failed to retrieve caInternalKey (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    // no nonsense, an error here is fatal.
//    assert( teeNtohl(RspPtr->DataLength) == caAES_128_KEY_SIZE_BYTES );

    // now, copy key
    RspPtr = (nfTeeStorageSimpleReadRsp_t *)caStorageResponseBuf;
    (void) teeMemcpy(caInternalKey, RspPtr->Data, caAES_128_KEY_SIZE_BYTES );
#if 0
    CRYPTODBG("%s: dumping ladder base...\n", __FUNCTION__);
    teePrintHexDump( caInternalKey, caAES_128_KEY_SIZE_BYTES );
#endif
    /*
     * caInternalKey is our key ladder key. We will use it to derive our internal
     * AES and HMAC-SHA256 keys.
     */
    {/* begin local scope limiter */
    	uint8_t TempBuf[caAES_128_KEY_SIZE_BYTES + caHMAC_SHA256_KEY_SIZE_BYTES];
    	int rc;

    	if ( ( rc = OPENSSL_LIBTOM_EXCHANGE(teeSha384)(caInternalKey, caAES_128_KEY_SIZE_BYTES, TempBuf) ) != 0 )
    	{
    		CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
    		// fatal
    		//assert(0);
    	}

#if TEE_LIBTOM_TEST_ENABLE
		teePrintHexDump_tag("[teeSha384] Buf :", TempBuf, sizeof(TempBuf) );
		uint8_t 	TempBuf__[1024];
		teeBzero(TempBuf__,1024);
		int rc__ = teeSha384_libtom(caInternalKey, caAES_128_KEY_SIZE_BYTES, TempBuf__ );
		teePrintHexDump_tag("[teeSha384_libtom] temBuf :", TempBuf__, sizeof(TempBuf) );
		printf("[sahala compare_buffer] TempBuf__\n");
		int Ret = compare_buffer(TempBuf,TempBuf__,sizeof(TempBuf),sizeof(TempBuf));
		if(Ret != 0)
			exit(1);
#endif
		
    	// first 128 bits is AES key, next 256 is HMAC key
    	(void)teeMemcpy( caInternalAesKey, TempBuf, caAES_128_KEY_SIZE_BYTES );
    	(void)teeMemcpy( caInternalHMACKey, TempBuf + caAES_128_KEY_SIZE_BYTES, caHMAC_SHA256_KEY_SIZE_BYTES );
#if 0
        CRYPTODBG("%s: dumping ladder rungs...\n", __FUNCTION__);
        teePrintHexDump( TempBuf, 48 );
#endif
    	// clear other keys from memory
    	(void) teeBzero(caInternalKey, caAES_128_KEY_SIZE_BYTES);
    	(void) teeBzero(TempBuf, caAES_128_KEY_SIZE_BYTES + caHMAC_SHA256_KEY_SIZE_BYTES);
    	CRYPTODBG("%s: created internal keys.\n", __FUNCTION__);

    }/* end local scope limiter */

    return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caImportNamedKeys */
#if 0
/*
 * SCOTT: I've disabled this for now. Need to check on why this is required
 * for key unwrap, and decide if this stays or goes.
 */
/*********************************************************************
 * @function extractIntFromString
 *
 * @brief gets the integer bits in a string and returns them as a number
 *
 * @param[in] str - string
 *
 * @returns uint32_t number
 *
 *********************************************************************/
static uint32_t extractIntFromString(uint8_t* str)
{
	//make sure string is not null
	if (str == NULL)
	{
		return 0;
	}
	uint32_t size = teeStrlen((const char*) str);
	//make sure string is not empty
	if (size == 0)
	{
		return 0;
	}

	uint8_t* digitBegin;
    uint32_t digitSize = 0;
    uint32_t i;
    uint32_t j;
    for (i=0; i < size; i++)
    {
        //iterate the string to find the first "number" character
        //if found create another loop to extract it
        //and then break the current one
        //thus extracting the FIRST encountered numeric block
        if ((str[i] >= '0') && (str[i] <= '9'))
        {
        	digitBegin = str+i;
            for (j=i; j<size; j++)
            {
            	digitSize++;
            	if ((str[i] < '0') || (str[i] > '9'))
            	{
            		break;
            	}
            }
            break;
        }
    }

    uint8_t number[digitSize+1];
    teeMemcpy(number, digitBegin, digitSize);
    number[digitSize] = '\0';
    return ((uint32_t) atoi((const char*) number));

}/* end extractIntFromString */
#endif


//********************* non-static Function implementations *****************************
/*********************************************************************
 * @function caHmacSha256Init
 *
 * @brief initialize crypto context for HMAC SHA256 operations
 *
 * @param[in]  KeyHandle - handle of the key you want to use
 * @param[in]  KeyPtr - pointer to the AES key you want to use
 * @param[out] CryptoContextHandlePtr - buffer where you want the crypto context stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: This function assumes you've already validate the ClientId for
 *     the associated key handle, and that the key is the right size,
 *     type, etc.
 *
 *********************************************************************/
nfTeeCryptoResult_t caHmacSha256Init( nfCryptoKeyHandle_t			KeyHandle,
									  uint8_t						*KeyPtr,
									  nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr )
{
	int 		KeyIndex;

	/*
	 * Crypto context for incremental hmac is stored in the
	 * key table, along with the associated key. This will be used to
	 * call a crypto TPI function (which has its own context). Here's what
	 * our context structure looks like:
	 *
	 *	typedef struct
	 *	{
	 *		uint32_t							KeyHandle;
	 *		caIncrementalCryptoContextState_t	State;
	 *		uint32_t							Validator;
	 *		void								*CryptoContextPtr;
	 *	} caIncrementalCryptoContext_t;
	 *
	 * We need to do the following:
	 * 		- sanity check crypto context for this key
	 * 			- if state is not UNINIT, this is an error
	 * 			- if Validator is not zero, this is an error
	 * 			- if CryptoContextPtr is not NULL, this is an error
	 * 		- if tpi aes encrypt-init function is successful
	 * 			- store context ptr at CryptoContextPtr
	 * 			- get random value for Validator
	 * 			- set state to UPDATE
	 * 			- put Validator and KeyHandle into CryptoContextHandle
	 * 		- else
	 * 			- return error
	 *
	 */

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( (caKeyStore[KeyIndex].CryptoContext.KeyHandle != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_UNINIT )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d, %d, %p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State, caKeyStore[KeyIndex].CryptoContext.Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	// call TPI function to init crypto context

	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = teeHmacSha256Init( KeyPtr );
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr = teeHmacSha256Init_libtom(KeyPtr);

	#if 0
	CRYPTODBG("%s: caHmacSha256Init return pointer : %p \n", __FUNCTION__,caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr);
	CRYPTODBG("%s: caHmacSha256Init_libtom return pointer : %p \n", __FUNCTION__,caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr);
	#endif

	
	if ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL )
	{
		#if 0
		int rc;
		rc = teeCryptoRandomBytes( (uint8_t *) &caKeyStore[KeyIndex].CryptoContext.Validator,
		              	  	  	   sizeof(uint32_t) );
		if ( rc == 0 )
		{
			// failed to get random bytes, clean up and return.
			CRYPTODBG("%s: failed to get random bytes for context validator (?!)\n", __FUNCTION__);
			(void) teeHmacSha256Cleanup( caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr );
			caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = NULL;
			return nfTCR_INTERNAL_ERROR;
		}
		#endif
		// update state
		caKeyStore[KeyIndex].CryptoContext.State = caICCS_HMAC_UPDATING;
		caKeyStore[KeyIndex].CryptoContext.KeyHandle = KeyHandle;

		// we are good, copy context into passed buffer and return.
		(void) teeMemcpy( (uint8_t *)CryptoContextHandlePtr,
						  &caKeyStore[KeyIndex].CryptoContext,
						  sizeof(nfTeeCryptoContextHandle_t) );
		
		CRYPTODBG("%s: dumping crypto context: \n", __FUNCTION__);
//		teePrintHexDump((uint8_t *) CryptoContextHandlePtr, sizeof(nfTeeCryptoContextHandle_t) );

		#if 0
		teePrintHexDump_tag("CryptoContextHandlePtr",(uint8_t *) CryptoContextHandlePtr, sizeof(nfTeeCryptoContextHandle_t) );
		caIncrementalCryptoContext_t* pContext = (caIncrementalCryptoContext_t*)CryptoContextHandlePtr;
		teePrintHexDump_tag("ContextPtr->cryptoItemPtr",pContext->cryptoItemPtr,sizeof(caAes128CryptoContextItem_t));
		#endif
		
	}

	return (nfTeeCryptoResult_t)nfTGR_SUCCESS;

}/* end caHmacSha256Init */

/*********************************************************************
 * @function caHmacSha256Update
 *
 * @brief HMAC SHA256 update operation (incremental hmac)
 *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[in] DataPtr - data to hmac
 * @param[in] DataLength - length of data
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 *********************************************************************/
nfTeeCryptoResult_t caHmacSha256Update( caIncrementalCryptoContext_t	*ContextPtr,
										uint8_t							*DataPtr,
										uint32_t						DataLength )
{
	int 				KeyIndex;
	int					rc;
	nfCryptoKeyHandle_t	KeyHandle;

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_HMAC_UPDATING
	  * 	- call hmac-update function
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;
	 teePrintHexDump_tag("[caHmacSha256Update]ContextPtr",(uint8_t *) ContextPtr, sizeof(caIncrementalCryptoContext_t) );
	CRYPTODBG("CryptoContextPtr %p\n",ContextPtr->cryptoItemPtr);
	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_HMAC_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );

		return nfTCR_OPERATION_NOT_ALLOWED;
	}



	CRYPTODBG("%s: calling teeHmacSha256Update for %d bytes.\n", __FUNCTION__, DataLength );

	#if 0
	rc = teeHmacSha256Update( ContextPtr->CryptoContextPtr, DataPtr, DataLength );
	#endif
	rc = teeHmacSha256Update_libtom(ContextPtr->cryptoItemPtr,DataPtr, DataLength );

	if ( rc < 0 )
	{
		CRYPTODBG("%s: teeHmacSha256Update failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caHmacSha256Update */

/*********************************************************************
 * @function caHmacSha256Final
 *
 * @brief incremental HMAC-SHA256 final
 * *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[out] HmacDataPtr - pointer to where you want the hmac
 * @param[in] HmacMaxLength - maximum length of hmac
 * @param[out] HmacLengthPtr - pointer to where you want hmac length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 *********************************************************************/
nfTeeCryptoResult_t caHmacSha256Final( caIncrementalCryptoContext_t *ContextPtr,
									   uint8_t						*HmacDataPtr,
									   uint32_t						HmacMaxLength,
									   uint32_t						*HmacLengthPtr )
{
	 nfCryptoKeyHandle_t	KeyHandle;
	 int 					KeyIndex;
	 int					rc;

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_ENCRYPT_UPDATING
	  * 	- call encrypt-final function
	  * 	- update context in key store (clear it)
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_HMAC_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	CRYPTODBG("%s: calling teeHmacSha256Final.\n", __FUNCTION__ );

	#if 0
	rc = teeHmacSha256Final( ContextPtr->CryptoContextPtr,
									HmacDataPtr, HmacMaxLength );

	teePrintHexDump_tag("HmacDataPtr",HmacDataPtr,HmacMaxLength);


	uint8_t HmacDataPtr_temp[1024];
	#endif
	
	rc = teeHmacSha256Final_libtom( ContextPtr->cryptoItemPtr,
									HmacDataPtr, HmacMaxLength ); 
	
	if ( rc < 0 )
	{
		CRYPTODBG("%s: teeHmacSha256Final failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}

	// clear context in key table, we're finished.
	caKeyStore[KeyIndex].CryptoContext.KeyHandle		= 0;
	caKeyStore[KeyIndex].CryptoContext.State			= 0;
	caKeyStore[KeyIndex].CryptoContext.Validator		= 0;
	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr	= NULL;
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr	= NULL;

	// update length and return
	*HmacLengthPtr = (uint32_t) rc;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caHmacSha256Final */

/*********************************************************************
 * @function caAes128CbcDecryptInit
 *
 * @brief initialize crypto context for AES128 CBC decryption operations
 *
 * @param[in]  KeyHandle - handle of the key you want to use
 * @param[in]  KeyPtr - pointer to the AES key you want to use
 * @param[in]  IV - the AES initialization vector (16 bytes for AES 128)
 * @param[out] CryptoContextHandlePtr - buffer where you want the crypto context stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: This function assumes you've already validate the ClientId for
 *     the associated key handle, and that the key is the right size,
 *     type, etc.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcDecryptInit( nfCryptoKeyHandle_t			KeyHandle,
											uint8_t						*KeyPtr,
									 	 	uint8_t						IV[],
									 	 	nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr )
{
	int 		KeyIndex;

	/*
	 * Crypto context for incremental encrypt/decrypt is stored in the
	 * key table, along with the associated key. This will be used to
	 * call a crypto TPI function (which has its own context). Here's what
	 * our context structure looks like:
	 *
	 *	typedef struct
	 *	{
	 *		caIncrementalCryptoContextState_t	State;
	 *		uint32_t							Validator;
	 *		void								*CryptoContextPtr;
	 *	} caIncrementalCryptoContext_t;
	 *
	 * We need to do the following:
	 * 		- sanity check crypto context for this key
	 * 			- if state is not UNINIT, this is an error
	 * 			- if Validator is not zero, this is an error
	 * 			- if CryptoContextPtr is not NULL, this is an error
	 * 		- if tpi aes encrypt-init function is successful
	 * 			- store context ptr at CryptoContextPtr
	 * 			- get random value for Validator
	 * 			- set state to UPDATE
	 * 			- put Validator and KeyHandle into CryptoContextHandle
	 * 		- else
	 * 			- return error
	 *
	 */

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( (caKeyStore[KeyIndex].CryptoContext.KeyHandle != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_UNINIT )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d, %d, %p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State, caKeyStore[KeyIndex].CryptoContext.Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	// call TPI function to init crypto context

	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = teeAes128CbcDecryptInit( IV, KeyPtr );
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr = teeAes128CbcDecryptInit_libtom(IV, KeyPtr );
	
	if ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL )
	{
		#if 0
		int rc;
		rc = teeCryptoRandomBytes( (uint8_t *) &caKeyStore[KeyIndex].CryptoContext.Validator,
		              	  	  	   sizeof(uint32_t) );
		if ( rc == 0 )
		{
			// failed to get random bytes, clean up and return.
			CRYPTODBG("%s: failed to get random bytes for context validator (?!)\n", __FUNCTION__);
			(void) teeAes128CbcEncryptCleanup( caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr );
			caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = NULL;
			return nfTCR_INTERNAL_ERROR;
		}
		#endif

		caKeyStore[KeyIndex].CryptoContext.Validator = 1 ;

		// update state
		caKeyStore[KeyIndex].CryptoContext.State = caICCS_DECRYPT_UPDATING;
		caKeyStore[KeyIndex].CryptoContext.KeyHandle = KeyHandle;

		// we are good, copy context into passed buffer and return.
		(void) teeMemcpy( (uint8_t *)CryptoContextHandlePtr,
						  &caKeyStore[KeyIndex].CryptoContext,
						  sizeof(nfTeeCryptoContextHandle_t) );

		CRYPTODBG("%s: dumping crypto context: \n", __FUNCTION__);
	//	teePrintHexDump((uint8_t *) CryptoContextHandlePtr, sizeof(nfTeeCryptoContextHandle_t) );
	}

	return (nfTeeCryptoResult_t)nfTGR_SUCCESS;

}/* end caAes128CbcDecryptInit */

/*********************************************************************
 * @function caAes128CbcDecryptUpdate
 *
 * @brief incremental AES-128-CBC decryption update
 * *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[in] EncryptedDataPtr - pointer to data you want decrypted
 * @param[in] EncryptedDataLength - length of the data you want decrypted
 * @param[out] DataPtr - pointer to where you want decrypted data
 * @param[in] DataMaxLength - maximum length of decrypted data
 * @param[out] DataLengthPtr - pointer to where you want actual length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: this is an internal function, we assume valid pointers/lengths.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcDecryptUpdate( caIncrementalCryptoContext_t 	*ContextPtr,
											  uint8_t						*EncryptedDataPtr,
											  uint32_t						EncryptedDataLength,
											  uint8_t						*DataPtr,
											  uint32_t						DataMaxLength,
											  uint32_t						*DataLengthPtr )
{
	 nfCryptoKeyHandle_t	KeyHandle;
	 int 					KeyIndex;
	 int					ActualLength = 0;

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_DECRYPT_UPDATING
	  * 	- call encrypt-update function
	  * 	- store length in *DataLengthPtr
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_DECRYPT_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	CRYPTODBG("%s: calling teeAes128CbcDecryptUpdate for %d bytes.\n", __FUNCTION__, EncryptedDataLength );

	ActualLength = teeAes128CbcDecryptUpdate_libtom( ContextPtr->cryptoItemPtr,
										EncryptedDataPtr, EncryptedDataLength,
										DataPtr, DataMaxLength );
	
	//printf("============================================\n");
	//teePrintHexDump_tag("DataPtr_temp",DataPtr,ActualLength);
	//printf("============================================\n");
	if ( ActualLength < 0 )
	{
		CRYPTODBG("%s: teeAes128CbcDecryptUpdate failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}

	// update length and return
	*DataLengthPtr = ActualLength;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caAes128CbcDecryptUpdate */

/*********************************************************************
 * @function caAes128CbcDecryptFinal
 *
 * @brief incremental AES-128-CBC Decryption final
 * *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[out] DataPtr - pointer to where you want any remaining decrypted data
 * @param[in] DataMaxLength - maximum length of decrypted data
 * @param[out] DataLengthPtr - pointer to where you want actual length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: this is an internal function, we assume valid pointers/lengths.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcDecryptFinal( caIncrementalCryptoContext_t 	*ContextPtr,
											 uint8_t						*DataPtr,
											 uint32_t						DataMaxLength,
											 uint32_t						*DataLengthPtr )
{
	 nfCryptoKeyHandle_t	KeyHandle;
	 int 					KeyIndex;
	 int					ActualLength = 0;

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_DECRYPT_UPDATING
	  * 	- call decrypt-final function
	  * 	- update context in key store (clear it)
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_DECRYPT_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	CRYPTODBG("%s: calling teeAes128CbcDecryptFinal.\n", __FUNCTION__ );

	ActualLength = teeAes128CbcDecryptFinal_libtom( ContextPtr->cryptoItemPtr,
										DataPtr, DataMaxLength );

	if ( ActualLength < 0 )
	{
		CRYPTODBG("%s: teeAes128CbcDecryptFinal failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}

	// clear context in key table, we're finished.
	caKeyStore[KeyIndex].CryptoContext.KeyHandle		= 0;
	caKeyStore[KeyIndex].CryptoContext.State			= 0;
	caKeyStore[KeyIndex].CryptoContext.Validator		= 0;
	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr	= NULL;
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr	= NULL;

	// update length and return
	*DataLengthPtr = ActualLength;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caAes128CbcDecryptFinal */

/*********************************************************************
 * @function caAes128CbcEncryptInit
 *
 * @brief initialize crypto context for AES128 CBC encryption operations
 *
 * @param[in]  KeyHandle - handle of the key you want to use
 * @param[in]  KeyPtr - pointer to the AES key you want to use
 * @param[in]  IV - the AES initialization vector (16 bytes for AES 128)
 * @param[out] CryptoContextHandlePtr - buffer where you want the crypto context stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: This function assumes you've already validate the ClientId for
 *     the associated key handle, and that the key is the right size,
 *     type, etc.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcEncryptInit( nfCryptoKeyHandle_t			KeyHandle,
											uint8_t						*KeyPtr,
									 	 	uint8_t						IV[],
									 	 	nfTeeCryptoContextHandle_t	*CryptoContextHandlePtr )
{
	int 		KeyIndex;

	/*
	 * Crypto context for incremental encrypt/decrypt is stored in the
	 * key table, along with the associated key. This will be used to
	 * call a crypto TPI function (which has its own context). Here's what
	 * our context structure looks like:
	 *
	 *	typedef struct
	 *	{
	 *		caIncrementalCryptoContextState_t	State;
	 *		uint32_t							Validator;
	 *		void								*CryptoContextPtr;
	 *	} caIncrementalCryptoContext_t;
	 *
	 * We need to do the following:
	 * 		- sanity check crypto context for this key
	 * 			- if state is not UNINIT, this is an error
	 * 			- if Validator is not zero, this is an error
	 * 			- if CryptoContextPtr is not NULL, this is an error
	 * 		- if tpi aes encrypt-init function is successful
	 * 			- store context ptr at CryptoContextPtr
	 * 			- get random value for Validator
	 * 			- set state to UPDATE
	 * 			- put Validator and KeyHandle into CryptoContextHandle
	 * 		- else
	 * 			- return error
	 *
	 */

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( (caKeyStore[KeyIndex].CryptoContext.KeyHandle != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_UNINIT )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != 0 )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d, %d, %p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State, caKeyStore[KeyIndex].CryptoContext.Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	// call TPI function to init crypto context

	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = teeAes128CbcEncryptInit( IV, KeyPtr );
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr = teeAes128CbcEncryptInit_libtom(IV, KeyPtr );

	CRYPTODBG("%s: teeAes128CbcEncryptInit_libtom return pointer : %p \n", __FUNCTION__,caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr);
	
	if ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != NULL )
	{
		#if 0
		int rc;
		rc = teeCryptoRandomBytes( (uint8_t *) &caKeyStore[KeyIndex].CryptoContext.Validator,
		              	  	  	   sizeof(uint32_t) );
		if ( rc == 0 )
		{
			// failed to get random bytes, clean up and return.
			CRYPTODBG("%s: failed to get random bytes for context validator (?!)\n", __FUNCTION__);
			//(void) teeAes128CbcEncryptCleanup( caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr );
			(void)teeAes128CbcEncryptCleanup_libtom(caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr);
			caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr = NULL;
			caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr = NULL;
			return nfTCR_INTERNAL_ERROR;
		}
		#endif
		caKeyStore[KeyIndex].CryptoContext.Validator = 1;
		// update state
		caKeyStore[KeyIndex].CryptoContext.State = caICCS_ENCRYPT_UPDATING;
		caKeyStore[KeyIndex].CryptoContext.KeyHandle = KeyHandle;

		// we are good, copy context into passed buffer and return.
		(void) teeMemcpy( (uint8_t *)CryptoContextHandlePtr,
						  &caKeyStore[KeyIndex].CryptoContext,
						  sizeof(nfTeeCryptoContextHandle_t) );

		CRYPTODBG("%s: dumping crypto context: \n", __FUNCTION__);
		teePrintHexDump_tag("CryptoContextHandlePtr",(uint8_t *) CryptoContextHandlePtr, sizeof(nfTeeCryptoContextHandle_t) );
	}

	return (nfTeeCryptoResult_t)nfTGR_SUCCESS;

}/* end caAes128CbcEncryptInit */

/*********************************************************************
 * @function caAes128CbcEncryptUpdate
 *
 * @brief incremental AES-128-CBC encryption update
 * *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[in] DataPtr - pointer to data you want encrypted
 * @param[in] DataLength - length of the data you want encrypted
 * @param[out] EncryptedDataPtr - pointer to where you want encrypted data
 * @param[in] EncryptedDataMaxLength - maximum length of encrypted data
 * @param[out] EncryptedDataLengthPtr - pointer to where you want actual length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: this is an internal function, we assume valid pointers/lengths.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcEncryptUpdate( caIncrementalCryptoContext_t 	*ContextPtr,
											  uint8_t						*DataPtr,
											  uint32_t						DataLength,
											  uint8_t						*EncryptedDataPtr,
											  uint32_t						EncryptedDataMaxLength,
											  uint32_t						*EncryptedDataLengthPtr )
{
	 nfCryptoKeyHandle_t	KeyHandle;
	 int 					KeyIndex;
	 int					ActualLength = 0;

	teePrintHexDump_tag("[caAes128CbcEncryptUpdate] CryptoContextHandlePtr ",ContextPtr, sizeof(caIncrementalCryptoContext_t) );

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_ENCRYPT_UPDATING
	  * 	- call encrypt-update function
	  * 	- store length in *EncryptedDataLengthPtr
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_ENCRYPT_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	CRYPTODBG("%s: calling teeAes128CbcEncryptUpdate for %d bytes.\n", __FUNCTION__, DataLength );

	#if 0
	ActualLength = teeAes128CbcEncryptUpdate( ContextPtr->CryptoContextPtr, DataPtr, DataLength,
									EncryptedDataPtr, EncryptedDataMaxLength );
	printf("============================================\n");
	teePrintHexDump_tag("DataPtr",DataPtr,DataLength);
	teePrintHexDump_tag("EncryptedDataPtr",EncryptedDataPtr,ActualLength);
	printf("============================================\n");

	#endif
	
	ActualLength = teeAes128CbcEncryptUpdate_libtom( ContextPtr->cryptoItemPtr, DataPtr, DataLength,
									EncryptedDataPtr, EncryptedDataMaxLength );

	#if 0
	printf("============================================\n");
	teePrintHexDump_tag("DataPtr",DataPtr,DataLength);
	teePrintHexDump_tag("IV",ContextPtr->cryptoItemPtr->IV,16);
	teePrintHexDump_tag("EncryptedDataPtr_temp",EncryptedDataPtr_temp,ActualLength1);
	printf("============================================\n");
	#endif
	
	CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
	
	if ( ActualLength < 0 )
	{
		CRYPTODBG("%s: teeAes128CbcEncryptUpdate failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}

	// update length and return
	*EncryptedDataLengthPtr = ActualLength-16;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caAes128CbcEncryptUpdate */

/*********************************************************************
 * @function caAes128CbcEncryptFinal
 *
 * @brief incremental AES-128-CBC encryption final
 * *
 * @param[in] ContextPtr - pointer to crypto context
 * @param[out] EncryptedDataPtr - pointer to where you want any remaining encrypted data
 * @param[in] EncryptedDataMaxLength - maximum length of encrypted data
 * @param[out] EncryptedDataLengthPtr - pointer to where you want actual length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 * NB: this is an internal function, we assume valid pointers/lengths.
 *
 *********************************************************************/
nfTeeCryptoResult_t caAes128CbcEncryptFinal( caIncrementalCryptoContext_t 	*ContextPtr,
											 uint8_t						*EncryptedDataPtr,
											 uint32_t						EncryptedDataMaxLength,
											 uint32_t						*EncryptedDataLengthPtr )
{
	 nfCryptoKeyHandle_t	KeyHandle;
	 int 					KeyIndex;
	 int					ActualLength = 0;

	 /*
	  * We need to do the following:
	  * 	- get KeyHandle out of context
	  * 	- validate context
	  * 		- this context matches context for this key handle
	  * 		- state is caICCS_ENCRYPT_UPDATING
	  * 	- call encrypt-final function
	  * 	- update context in key store (clear it)
	  */

	 // get key handle out of context
	KeyHandle = ContextPtr->KeyHandle;

	// sanity check crypto context
	if ( ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	  || ( caKeyStore[KeyIndex].InUse == 0 ) )
	{
		CRYPTODBG("%s: index invalid (%d) or slot not in use.\n", __FUNCTION__, KeyIndex );
		return nfTCR_BAD_PARAMETER;
	}
	else if ( ( caKeyStore[KeyIndex].CryptoContext.State != ContextPtr->State )
		   || ( caKeyStore[KeyIndex].CryptoContext.State != caICCS_ENCRYPT_UPDATING )
		   || ( caKeyStore[KeyIndex].CryptoContext.Validator != ContextPtr->Validator )
		   || ( caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr != ContextPtr->cryptoItemPtr ) )
	{
		CRYPTODBG("%s: invalid crypto context (%d/%d, %d/%d, %p/%p)\n", __FUNCTION__,
				caKeyStore[KeyIndex].CryptoContext.State,  ContextPtr->State,
				caKeyStore[KeyIndex].CryptoContext.Validator, ContextPtr->Validator,
				caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr,
				ContextPtr->cryptoItemPtr );
		return nfTCR_OPERATION_NOT_ALLOWED;
	}

	CRYPTODBG("%s: calling teeAes128CbcEncryptFinal.\n", __FUNCTION__ );

	#if 0
	ActualLength = teeAes128CbcEncryptFinal( ContextPtr->CryptoContextPtr,
									EncryptedDataPtr, EncryptedDataMaxLength );

	printf("teeAes128CbcEncryptFinal return length : %d \n" , ActualLength);
	teePrintHexDump_tag("teeAes128CbcEncryptFinal",EncryptedDataPtr,ActualLength);
	#endif

	ActualLength = teeAes128CbcEncryptFinal_libtom( ContextPtr->cryptoItemPtr,
									EncryptedDataPtr, EncryptedDataMaxLength );

	#if 0
	teePrintHexDump_tag("teeAes128CbcEncryptFinal_libtom",EncryptedDataPtr,ActualLength);
	#endif
	
	if ( ActualLength < 0 )
	{
		CRYPTODBG("%s: teeAes128CbcEncryptFinal failed.\n", __FUNCTION__ );
		return (nfTeeCryptoResult_t) nfTCR_INTERNAL_ERROR;
	}
	// clear context in key table, we're finished.
	caKeyStore[KeyIndex].CryptoContext.KeyHandle		= 0;
	caKeyStore[KeyIndex].CryptoContext.State			= 0;
	caKeyStore[KeyIndex].CryptoContext.Validator		= 0;
	//caKeyStore[KeyIndex].CryptoContext.CryptoContextPtr	= NULL;
	caKeyStore[KeyIndex].CryptoContext.cryptoItemPtr	= NULL;
	// update length and return
	*EncryptedDataLengthPtr = ActualLength;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caAes128CbcEncryptFinal */


/*********************************************************************
 * @function caGetEsn
 *
 * @brief get ESN (if client ID is valid/correct)
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] EsnMaxLength - maximum ESN length to return
 * @param[out] EsnPtr - pointer to where you want the ESN stored
 * @param[out] EsnLengthPtr - pointer to where you want length stored
 *
 * @returns nfTGR_SUCCESS if okay, nfTCR_OPERATION_NOT_ALLOWED otherwise
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetEsn( nfTeeClientId_t 	ClientId,
							  uint32_t			EsnMaxLength,
						   	  uint8_t 			*EsnPtr,
							  uint32_t 			*EsnLengthPtr )
{
	// no nonsense
	//assert(ClientId);
	//assert(EsnPtr);
	//assert(EsnLengthPtr);

	// validate client ID, only netflix gets this value...
	if ( teeMemcmp(ClientId, caClientIdList[caCID_INDEX_NRDP_APP], sizeof(nfTeeClientId_t)) == 0 )
	{
		CRYPTODBG("%s: ClientId matches\n", __FUNCTION__);

		if ( caEsnLength <= EsnMaxLength )
		{
			(void) teeMemcpy(EsnPtr, caESN, caEsnLength);
			//printf("%s: returning %d for ESN length.\n", __FUNCTION__, caEsnLength);
			*EsnLengthPtr = caEsnLength;
			return (nfTeeCryptoResult_t) nfTGR_SUCCESS;
		}
		else
		{
			CRYPTODBG("%s: not enough room for ESN (%d/%d)\n",
						__FUNCTION__, caEsnLength, EsnMaxLength );
			return nfTCR_OUT_OF_MEMORY;
		}
	}

	// else invalid client id
	return nfTCR_OPERATION_NOT_ALLOWED;

}/* end caGetEsn */

/*********************************************************************
 * @function keyMgmtInit
 *
 * @brief crypto agent key mgmt module initialization
 *
 * @returns nfTGR_SUCCESS if successful
 *********************************************************************/
nfTeeCryptoResult_t keyMgmtInit()
{
    nfTeeCryptoResult_t Result;
    
    //clear the key store 
    teeBzero(caKeyStore,caMAX_KEYS*sizeof(caKeyObject_t));
		teeBzero(caNamedKeyStore,caMAX_NAMED_KEYS*sizeof(caNamedKey_t));
		teeBzero(caInternalKey,caAES_128_KEY_SIZE_BYTES*sizeof(uint8_t));
		teeBzero(caInternalAesKey,caAES_128_KEY_SIZE_BYTES*sizeof(uint8_t));
		teeBzero(caInternalHMACKey,caHMAC_SHA256_KEY_SIZE_BYTES*sizeof(uint8_t));


    // import named keys (from manufacturing secure store)
    if ( (Result = (nfTeeCryptoResult_t) caImportNamedKeys())
    					!= (nfTeeCryptoResult_t) nfTGR_SUCCESS )
    {
        return Result;
    }

    // additional init code here...

    return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end keyMgmtInit */

/*********************************************************************
 * @function caGetNamedKeyHandle
 *
 * @brief get handle for named key
 *
 * @param[in] ClientId - the TEE client ID for this key
 * @param[in] KeyName - the name of the key you are looking for
 * @param[in] KeyNameLength - length of the key name
 * @param[out] KeyHandlePtr - pointer to where you want key handle stored
 *
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_KEY_NOT_FOUND, nfTCR_BAD_PARAMETER
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetNamedKeyHandle( nfTeeClientId_t		ClientId,
						 	 	 	 	 uint8_t 				KeyName[],
						 	 	 	 	 uint32_t 				KeyNameLength,
						 	 	 	 	 nfCryptoKeyHandle_t 	*KeyHandlePtr )
{
	int 				i;
	nfCryptoKeyHandle_t	KeyHandle = 0;
	nfKeyContext_t		KeyContext;
	nfTeeCryptoResult_t	Result;

	// validate arguments
    //assert(ClientId);
    //assert(KeyName);
	//assert(KeyHandlePtr != NULL);

	if ( ( KeyNameLength > nfTEE_CRYPTO_MAX_KEYNAME_SIZE ) || ( KeyNameLength == 0 ) )
	{
		CRYPTODBG("%s: returning BAD_PARM for KeyNameLength %d\n", __FUNCTION__, KeyNameLength );
		return nfTCR_BAD_PARAMETER;
	}

	// see if we recognize this key
	for ( i = 0; i < caNK_INDEX_INVALID; ++i )
	{
		// first, check length for match
		if ( caNamedKeyStore[i].KeyNameLength == KeyNameLength )
		{
			// length is right, check name
			if ( teeMemcmp( caNamedKeyStore[i].KeyName, KeyName, KeyNameLength ) == 0 )
			{
				// bingo. Does this one have a handle>
				if ( caNamedKeyStore[i].KeyHandle == caKeyInvalidHandle )
				{
					//create the key context
					if (caNamedKeyStore[i].KeyFormat == nfKF_RAW)
					{
						KeyContext.ContextType = caKC_RAW;
						KeyContext.ctxt.rawContext.KeyDataLength = caNamedKeyStore[i].KeyLength;
						teeMemcpy(KeyContext.ctxt.rawContext.KeyData, caNamedKeyStore[i].KeyBits, caNamedKeyStore[i].KeyLength);
					}
					else
					{
	                    CRYPTODBG("%s: returning UNSUPPORTED_KEY_ENCODING, cannot handle non-RAW keys\n", __FUNCTION__);
	                    return nfTCR_UNSUPPORTED_KEY_ENCODING;
					}
					// no handle, insert this into key table and get handle
					if (  ( KeyHandle = caAllocateKeyHandle() ) == caKeyInvalidHandle )
				    {
				    	CRYPTODBG("%s: returning KEY_STORE_FULL...\n", __FUNCTION__);
				        return nfTCR_KEY_STORE_FULL;
				    }

				    CRYPTODBG("%s: calling caInsertKey with  keytype %d, Algorithm %d, KeyUsageFlags %08x\n",
				    			__FUNCTION__,  caNamedKeyStore[i].KeyType, caNamedKeyStore[i].Algorithm,
			    				  caNamedKeyStore[i].KeyUsageFlags );

				    Result = caInsertKey( ClientId, KeyHandle, &KeyContext,
				    				  caNamedKeyStore[i].KeyType, caNamedKeyStore[i].Algorithm,
				    				  caNamedKeyStore[i].KeyUsageFlags );

				    if ( Result == (nfTeeCryptoResult_t) nfTGR_SUCCESS )
				    {
				    	// save key handle in named key list
				    	caNamedKeyStore[i].KeyHandle = KeyHandle;
				    }
				    else
				    {
				    	return Result;
				    }
				}/* end if ( caNamedKeyStore[i].KeyHandle == 0 ) */

				/*
				 * Either the handle is in the named key list (already inserted into
				 * key table), or we just inserted it. Either way, need to assign
				 * *KeyHandlePtr here
			     */
				*KeyHandlePtr = caNamedKeyStore[i].KeyHandle;
				// and return
				return (nfTeeCryptoResult_t) nfTGR_SUCCESS;
			}/* end if ( teeMemcmp( caNamedKeyStore[i].KeyName, KeyName, KeyNameLength ) == 0 ) */

		}/* end if ( caNamedKeyStore[i].KeyNameLength == KeyNameLength ) */

	}/* end for ( i = 0; i < caNK_INDEX_INVALID; ++i ) */

	// if we get here, we didn't find it...
	return nfTCR_KEY_NOT_FOUND;

}/* end caGetNamedKeyHandle */

/*********************************************************************
 * @function caImportKey
 *
 * @brief load keys into key table
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyFormat - key format
 * @param[in] KeyData - the key (interpretation depends on KeyFormat)
 * @param[in] DataLength - length of KeyData
 * @param[in] Algorithm - the Algorithm
 * @param[in] hash - ??? not sure why this is here
 * @param[in] KeyUsageFlags - key usage flags
 * @param[out] KeyHandlePtr - pointer to where you want key handle
 * @param[out] KeyTypePtr - pointer to where you want key type
 *
 * @returns nfTGR_SUCCESS if successful,
 *********************************************************************/
nfTeeCryptoResult_t caImportKey( nfTeeClientId_t 		ClientId,
							 	 nfKeyFormat_t 			KeyFormat,
							 	 uint8_t 				KeyData[],
							 	 uint32_t 				DataLength,
							 	 nfCryptoAlgorithm_t 	Algorithm,
							 	 nfKeyUsageFlagsWord_t 	KeyUsageFlags,
							 	 nfCryptoKeyHandle_t 	*KeyHandlePtr,
							 	 nfKeyType_t			*KeyTypePtr )
{
	(void)ClientId;	//TODO: this is just to remove compiler warning. Use it or remove from func signature.
	//nfTeeCryptoResult_t 	Result;

    // no nonsense allowed
	//assert(KeyData != NULL);
	//assert(DataLength != 0);
	//assert(KeyHandlePtr != NULL);
    //assert(KeyTypePtr != NULL);

	nfKeyContext_t 		KeyContext;

    if (nfKF_RAW != KeyFormat)
    {
        CRYPTODBG("%s: returning BAD_PARAMETER, Only RAW Key Import is allowed...\n", __FUNCTION__ );
        return nfTCR_BAD_PARAMETER;
    }
    if (0 == caReconcileAlgVsUsage(Algorithm, KeyUsageFlags))
    {
        CRYPTODBG("%s: returning OP_NOT_ALLOWED...\n", __FUNCTION__ );
        return nfTCR_OPERATION_NOT_ALLOWED;
    }

    switch (Algorithm)
    {
        case nfCA_AES_CBC:
        case nfCA_AES_GCM:
        case nfCA_AES_CTR:
        case nfCA_AES_KW:
        case nfCA_HMAC:
            // We assume RAW key input is always a symmetric secret key
            *KeyTypePtr = nfKT_SECRET;

            //create the raw key context
            KeyContext.ContextType = caKC_RAW;
            KeyContext.ctxt.rawContext.KeyDataLength = DataLength;
            teeMemcpy (KeyContext.ctxt.rawContext.KeyData, KeyData, DataLength);
            CRYPTODBG("%s: Raw context with key data length %d...\n", __FUNCTION__ ,
                    KeyContext.ctxt.rawContext.KeyDataLength);
            break;
        // all the following Algorithms will be dealt in JWK format, so throw error for RAW format
        case nfCA_RSASSA_PKCS1_V1_5:
        case nfCA_RSA_OAEP:
        case nfCA_RSAES_PKCS1_V1_5:
        default:
            CRYPTODBG("%s: returning BAD_PARM...\n", __FUNCTION__ );
            return nfTCR_BAD_PARAMETER;
    } /* end switch (Algorithm) */

    // Finally, make a new Key object containing the extracted key and add it to
    // the key store, indexed by (output) KeyHandle.
    // Note that extractable and KeyUsage are copied through without processing.
    *KeyHandlePtr = caAllocateKeyHandle();
    if (*KeyHandlePtr == caKeyInvalidHandle)
    {
    	CRYPTODBG("%s: returning KEY_STORE_FULL...\n", __FUNCTION__);
    	return nfTCR_KEY_STORE_FULL;
    }

    CRYPTODBG("%s: calling caInsertKey with handle %d, keytype %d, Algorithm %d, KeyUsageFlags %08x\n",
    			__FUNCTION__, *KeyHandlePtr, *KeyTypePtr, Algorithm, KeyUsageFlags );

    return caInsertKey(ClientId, *KeyHandlePtr, &KeyContext,  *KeyTypePtr, Algorithm, KeyUsageFlags );

}/* end caImportKey */

/*********************************************************************
 * @function caExportKey
 *
 * @brief export key from table
 *
 * @param[in]  ClientId  - the TEE client ID
 * @param[in]  KeyHandle - handle of the key you want
 * @param[in]  KeyFormat - key format
 * @param[out] KeyDataPtr - pointer to where you want the key
 * @param[in] KeyDataMax - maximum length of *KeyDataPtr
 * @param[out] DataLengthPtr - pointer to length of KeyData
 *
 * @returns nfTGR_SUCCESS if successful,
 * 		nfTCR_OPERATION_NOT_ALLOWED, nfTCR_BAD_PARAMETER, nfTCR_MALFORMED_DATA otherwise
 *********************************************************************/
nfTeeCryptoResult_t caExportKey( nfTeeClientId_t 		ClientId,
							     nfCryptoKeyHandle_t 	KeyHandle,
							     nfKeyFormat_t 			KeyFormat,
							     uint8_t				*KeyDataPtr,
							     uint32_t				KeyDataLength,
							     uint32_t				*DataLengthPtr )
{

	// keys in caNamedKeyStore are not exportable, so only look into caKeyStore
	caKeyObject_t  	*KeyPtr;

    if (nfKF_RAW != KeyFormat)
    {
        CRYPTODBG("%s: returning BAD_PARAMETER, Only RAW Key Export is allowed...\n", __FUNCTION__ );
        return nfTCR_BAD_PARAMETER;
    }

    int index = caGetIndexFromHandle(KeyHandle);
	if ((index == caKeyInvalidIndex) || (caKeyStore[index].InUse == 0))
	{
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, index ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE
		uint8_t *BytePtr = ClientId;
		
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	KeyPtr = &(caKeyStore[index]);
	if (!(KeyPtr->KeyUsageFlags & nfKUF_EXTRACTABLE))
	{
		// if this is a DH pub key, this is an exception
		if ( KeyPtr->KeyContext.ContextType  != caKC_DH )
		{
			CRYPTODBG("%s: key is not extractable, returning OP_NOT_ALLOWED...\n", __FUNCTION__ );
			return nfTCR_OPERATION_NOT_ALLOWED;
		}
		// else this is a DH key -- export pub key
		CRYPTODBG("%s: exporting DH public key...\n", __FUNCTION__);
		*DataLengthPtr = KeyPtr->KeyContext.ctxt.dhContext.PubKeyLength;
		teeMemcpy(KeyDataPtr, KeyPtr->KeyContext.ctxt.dhContext.PubKey, *DataLengthPtr);
		return (nfTeeCryptoResult_t) nfTGR_SUCCESS;
    }/* end if ... */

	switch (KeyPtr->Algorithm)
    {
        case nfCA_HMAC:
        case nfCA_DH_ALGO:
        case nfCA_AES_CBC:
        case nfCA_AES_GCM:
        case nfCA_AES_CTR:
        case nfCA_AES_KW:
            {
                if ( KeyPtr->KeyContext.ctxt.rawContext.KeyDataLength <= KeyDataLength )
                {
                    *DataLengthPtr = KeyPtr->KeyContext.ctxt.rawContext.KeyDataLength;
                    teeMemcpy(KeyDataPtr, KeyPtr->KeyContext.ctxt.rawContext.KeyData, *DataLengthPtr);
                }
                else
                {
                    // not enough space in outgoing buffer
                    CRYPTODBG("%s: key is too long for passed buffer (%d/%d)\n", __FUNCTION__,
                             KeyPtr->KeyContext.ctxt.rawContext.KeyDataLength, KeyDataLength );
                    return nfTCR_BUFFER_TOO_SMALL;
                }
            }
            break;
        default:
            {
                CRYPTODBG("%s: Invalid or unknown Algorithm to export RAW key, returning MALFORMED_DATA...\n", __FUNCTION__ );
                return nfTCR_MALFORMED_DATA;
            }
    }/* end switch */

    return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caExportKey */

/*********************************************************************
 * @function caGetKeyInfoFromHandle
 *
 * @brief find key by handle, return key info
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - pointer to where you want key handle
 * @param[out] KeyTypePtr - pointer to where you want key type
 * @param[out] AlgorithmPtr - pointer to where you want key type
 * @param[out] KeyUsageFlagsPtr - pointer to where you want key type
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetKeyInfoFromHandle( nfTeeClientId_t 		ClientId,
											nfCryptoKeyHandle_t		KeyHandle,
											nfKeyType_t 			*KeyTypePtr,
											nfCryptoAlgorithm_t 	*AlgorithmPtr,
											nfKeyUsageFlagsWord_t 	*KeyUsageFlagsPtr )
{
	int	KeyTableIndex;
	(void) ClientId;

	// no nonsense
	//assert(KeyTypePtr);
	//assert(AlgorithmPtr);
	//assert(KeyUsageFlagsPtr);

	// get key table index
	CRYPTODBG("%s: XXX - need keytable LOCK (TOCTTOU issue)...\n", __FUNCTION__);

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Get the info.
	*KeyTypePtr 		= caKeyStore[KeyTableIndex].KeyType;
	*AlgorithmPtr 		= caKeyStore[KeyTableIndex].Algorithm;
	*KeyUsageFlagsPtr 	= caKeyStore[KeyTableIndex].KeyUsageFlags;

	CRYPTODBG("%s: returning %d/%d/%08x\n", __FUNCTION__,
					*KeyTypePtr, *AlgorithmPtr, *KeyUsageFlagsPtr );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end  caGetKeyInfoFromHandle */

/*********************************************************************
 * @function caGetAesKeyFromHandle
 *
 * @brief find AES key by handle, return key
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] KeyPtrPtr - pointer to where you want pointer to key bits
 * @param[out] KeyLengthPtr - pointer to key length
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetAesKeyFromHandle( nfTeeClientId_t 		ClientId,
										   nfCryptoKeyHandle_t	KeyHandle,
										   uint8_t				**KeyPtrPtr,
										   uint32_t				*KeyLengthPtr )
{
	int	KeyTableIndex;

	// no nonsense
	//assert(ClientId);
	//assert(KeyPtrPtr);
	//assert(KeyLengthPtr);

	// get key table index
	CRYPTODBG("%s: XXX - need keytable LOCK (TOCTTOU issue)...\n", __FUNCTION__);

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE	
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Make sure it is an AES key.
	if ( ( caKeyStore[KeyTableIndex].KeyType != nfKT_SECRET )
	  || ( ( ! nfIS_AES_ALG(caKeyStore[KeyTableIndex].Algorithm) )
	    && ( ! nfIS_HMAC_ALG(caKeyStore[KeyTableIndex].Algorithm) ) ) )
	{
		CRYPTODBG("%s: key type is invalid (%08x/%0x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyType, caKeyStore[KeyTableIndex].Algorithm );
		return nfTCR_KEY_TYPE_ALG_MISMATCH;
	}

	// check key context type
	if ( caKeyStore[KeyTableIndex].KeyContext.ContextType != caKC_RAW )
	{
		CRYPTODBG("%s: key context type is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyContext.ContextType );
		return nfTCR_INTERNAL_ERROR;
	}

	// if we get here it's all good, so return pointer to key bits
	*KeyPtrPtr 		= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyData;
	*KeyLengthPtr 	= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyDataLength;

	CRYPTODBG("%s: returning %p/%d for handle %08x\n", __FUNCTION__,
					*KeyPtrPtr, *KeyLengthPtr, KeyHandle );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end  caGetAesKeyFromHandle */

/*********************************************************************
 * @function caGetHmacKeyFromHandle
 *
 * @brief find hmac key by handle, return key
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] KeyPtrPtr - pointer to where you want pointer to key bits
 * @param[out] KeyLengthPtr - pointer to key length
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetHmacKeyFromHandle( nfTeeClientId_t 	ClientId,
							 			    nfCryptoKeyHandle_t	KeyHandle,
										    uint8_t				**KeyPtrPtr,
										    uint32_t			*KeyLengthPtr )
{
	int	KeyTableIndex;

	// no nonsense
	//assert(ClientId);
	//assert(KeyPtrPtr);
	//assert(KeyLengthPtr);

	// get key table index
	CRYPTODBG("%s: XXX - need keytable LOCK (TOCTTOU issue)...\n", __FUNCTION__);

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Make sure it is an AES key.
	if ( ( caKeyStore[KeyTableIndex].KeyType != nfKT_SECRET )
	  || ( ! nfIS_HMAC_ALG(caKeyStore[KeyTableIndex].Algorithm) ) )
	{
		CRYPTODBG("%s: key type is invalid (%08x/%0x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyType, caKeyStore[KeyTableIndex].Algorithm );
		return nfTCR_KEY_TYPE_ALG_MISMATCH;
	}

	// check key context type
	if ( caKeyStore[KeyTableIndex].KeyContext.ContextType != caKC_RAW )
	{
		CRYPTODBG("%s: key context type is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyContext.ContextType );
		return nfTCR_INTERNAL_ERROR;
	}

	// if we get here it's all good, so return pointer to key bits
	*KeyPtrPtr 		= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyData;
	*KeyLengthPtr 	= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyDataLength;

	CRYPTODBG("%s: returning %p/%d for handle %08x\n", __FUNCTION__,
					*KeyPtrPtr, *KeyLengthPtr, KeyHandle );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end  caGetHmacKeyFromHandle */

/*********************************************************************
 * @function caGetKeyDerivationKeyFromHandle
 *
 * @brief find AES key by handle, return key
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] KeyPtrPtr - pointer to where you want pointer to key bits
 * @param[out] KeyLengthPtr - pointer to key length
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static nfTeeCryptoResult_t
caGetKeyDerivationKeyFromHandle( nfTeeClientId_t 		ClientId,
					   	   	   	 nfCryptoKeyHandle_t	KeyHandle,
					   	   	   	 uint8_t				**KeyPtrPtr,
					   	   	   	 uint32_t				*KeyLengthPtr )
{
	int	KeyTableIndex;

	// since function is static, assume arguments are sane, don't check them.

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Make sure it is a Key derivation key.
	if ( ( caKeyStore[KeyTableIndex].KeyType != nfKT_SECRET )
#if 1
	  || ( caKeyStore[KeyTableIndex].Algorithm != nfCA_AES_KW ) )
#else
	  || ( caKeyStore[KeyTableIndex].Algorithm != nfCA_NFLX_DH )
	  || ( ! ( caKeyStore[KeyTableIndex].KeyUsageFlags & nfKUF_DERIVE ) ) )
#endif
	{
		CRYPTODBG("%s: key type/alg/usage is invalid (%08x/%0x/%0x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyType,
				caKeyStore[KeyTableIndex].Algorithm, caKeyStore[KeyTableIndex].KeyUsageFlags);
		return nfTCR_KEY_TYPE_ALG_MISMATCH;
	}

	// check key context type
	if ( caKeyStore[KeyTableIndex].KeyContext.ContextType != caKC_RAW )
	{
		CRYPTODBG("%s: key context type is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyContext.ContextType );
		return nfTCR_INTERNAL_ERROR;
	}

	// if we get here it's all good, so return pointer to key bits
	*KeyPtrPtr 		= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyData;
	*KeyLengthPtr 	= caKeyStore[KeyTableIndex].KeyContext.ctxt.rawContext.KeyDataLength;

	CRYPTODBG("%s: returning %p/%d for handle %08x\n", __FUNCTION__,
					*KeyPtrPtr, *KeyLengthPtr, KeyHandle );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end  caGetKeyDerivationKeyFromHandle */

/*********************************************************************
 * @function caGetRsaContextFromHandle
 *
 * @brief find rsa context by handle, return context
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] rsaCtxtPtrPtr - pointer to where you want pointer to rsa context
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
 #if 0
nfTeeCryptoResult_t caGetRsaContextFromHandle( nfTeeClientId_t 	ClientId,
											nfCryptoKeyHandle_t	KeyHandle,
											nfRsaContext_t		**rsaCtxtPtrPtr)
{
	int	KeyTableIndex;

	// no nonsense
	//assert(ClientId);
	//assert(rsaCtxtPtrPtr);

	// get key table index
	CRYPTODBG("%s: XXX - need keytable LOCK (TOCTTOU issue)...\n", __FUNCTION__);

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Make sure it is an RSA key.
	if ( ! nfIS_RSA_ALG(caKeyStore[KeyTableIndex].Algorithm) )
	{
		CRYPTODBG("%s: key Algorithm is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].Algorithm );
		return nfTCR_KEY_TYPE_ALG_MISMATCH;
	}

	// check key context type
	if ( caKeyStore[KeyTableIndex].KeyContext.ContextType != caKC_RSA )
	{
		CRYPTODBG("%s: key context type is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyContext.ContextType );
		return nfTCR_INTERNAL_ERROR;
	}

	// if we get here it's all good, so return pointer to key bits
	*rsaCtxtPtrPtr = &(caKeyStore[KeyTableIndex].KeyContext.ctxt.rsaContext);

	CRYPTODBG("%s: returning %p for handle %08x\n", __FUNCTION__,
					*rsaCtxtPtrPtr, KeyHandle );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caGetRsaContextFromHandle */
#endif

/*********************************************************************
 * @function caGetDhContextFromHandle
 *
 * @brief find DiffieHellman context by handle, return context
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] DhCtxtPtrPtr - pointer to where you want pointer to DH context
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caGetDhContextFromHandle( nfTeeClientId_t 		ClientId,
											  nfCryptoKeyHandle_t	KeyHandle,
											  nfDHContext_t			**DhCtxtPtrPtr)
{
	int	KeyTableIndex;

	// no nonsense
	//assert(ClientId);
	//assert(DhCtxtPtrPtr);

	// get key table index
	CRYPTODBG("%s: XXX - need keytable LOCK (TOCTTOU issue)...\n", __FUNCTION__);

	KeyTableIndex = caGetIndexFromHandle(KeyHandle);
	if ((KeyTableIndex == (int)caKeyInvalidHandle) || (caKeyStore[KeyTableIndex].InUse == 0))
	{
		CRYPTODBG("%s: key handle %08x is invalid.\n", __FUNCTION__, KeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// okay, it's a valid key handle; is this the right client?
	if ( caClientIdMatchesKeyHandle( ClientId, KeyTableIndex ) == nfBT_FALSE )
	{
		#if TEE_DEBUG_ENABLE
		uint8_t *BytePtr = ClientId;
		CRYPTODBG("%s: key handle %08x is invalid for client id %08x %08x %08x %08x.\n", __FUNCTION__,
				KeyHandle, *((uint32_t *) BytePtr), *((uint32_t *) BytePtr + 4),
				*((uint32_t *) BytePtr + 8), *((uint32_t *) BytePtr + 12) );
		#endif
		return nfTCR_KEY_NOT_FOUND;
	}

	// okay, it's a valid key handle. Make sure it is a DH key.
	if ( ( caKeyStore[KeyTableIndex].Algorithm != nfCA_NFLX_DH )
	  && ( caKeyStore[KeyTableIndex].Algorithm != nfCA_DH_ALGO ) )
	{
		CRYPTODBG("%s: key Algorithm is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].Algorithm );
		return nfTCR_KEY_TYPE_ALG_MISMATCH;
	}

	// check key context type
	if ( caKeyStore[KeyTableIndex].KeyContext.ContextType != caKC_DH )
	{
		CRYPTODBG("%s: key context type is invalid (%08x/%0x).\n", __FUNCTION__,
				KeyHandle, caKeyStore[KeyTableIndex].KeyContext.ContextType );
		return nfTCR_INTERNAL_ERROR;
	}

	// if we get here it's all good, so return pointer to key bits
	*DhCtxtPtrPtr = &(caKeyStore[KeyTableIndex].KeyContext.ctxt.dhContext);

	CRYPTODBG("%s: returning %p for handle %08x\n", __FUNCTION__,
					*DhCtxtPtrPtr, KeyHandle );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caGetDhContextFromHandle */

/*********************************************************************
 * @function caExportSealedKey
 *
 * @brief export sealed key
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - key handle
 * @param[out] SealedKeyPtr - pointer to where you want sealed key
 * @param[out] SealedKeyLengthPtr - pointer to where you want sealed key length
 * @param[in] SealedKeyMaxLength - maximum length of buffer at SealedKeyPtr
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_KEY_NOT_FOUND, nfTCR_BAD_PARAMETER
 *
 *********************************************************************/
nfTeeCryptoResult_t caExportSealedKey( nfTeeClientId_t 		ClientId,
                       		    	   nfCryptoKeyHandle_t	KeyHandle,
                                       uint8_t				*SealedKeyPtr,
                                       uint32_t				*SealedKeyLengthPtr,
                                       uint32_t				SealedKeyMaxLength )
{
	uint8_t				WorkBuf[caCRYPT_WORKBUF_SIZE];
	uint8_t				*BytePtr;
	uint8_t				*IVPtr;
	nfTeeCryptoResult_t	Result;
	uint32_t			PlaintextLength;
	int					CiphertextLength;
	uint32_t			HmacDataLength;
	int					HmacLength;
	uint32_t			B64Length;
	uint8_t				*KeyPtr;

	/*
	 * We need to do the following:
	 *  - sanity-check output buffer size
	 *		- reject if less than MIN_IV_SIZE + min_size(nfSealedKeyHeader_t) + sizeof(HMAC-SHA256)
	 *  - find key based on ClientId/KeyHandle
	 *  	- verify that key is non-extractable
	 *  - put version, IV in buffer
	 *  - create nfSealedKeyHeader_t in buffer following IV
	 *  - encrypt (add padding if needed)
	 *  - compute hmac
	 * 	- b64 encode the data at SealedKeyPtr
	 * 	- update *SealedKeyLengthPtr
	 *
	 * 	We need at least 2 buffers to work with as we do this. First, we need to
	 * 	assemble the plaintext in b1, we need to call encrypt with b1 and b2 (b2 is
	 * 	for the ciphertext), and then we need to b64-encode b2 into b1.
	 *
	 * 	If your implementation can do all of this in place, that's awesome! But mine
	 * 	can't :-( so I have to try to be clever. Since SealedKeyPtr should be large
	 * 	enough to hold the final result, I will use this as scratch memory (it is b1).
	 * 	WorkBuf (defined above) is b2. So simple!
	 */

	/*
	 * Do initial  check. Note: This doesn't guarantee that there is enough space, but
	 * it ensures that we fail early if there definitely isn't enough.
	 */
	if ( SealedKeyMaxLength < nfSEALED_KEY_MIN_LENGTH )
	{
		CRYPTODBG("%s: not enough room for sealed key (%d/%d)\n", __FUNCTION__,
					SealedKeyMaxLength, nfSEALED_KEY_MIN_LENGTH);
		return nfTCR_OPERAND_TOO_SHORT;
	}

	// start building key blob in scratch memory -- version first
	BytePtr = WorkBuf;
	*((uint32_t *) BytePtr) = caSealedKeyVersion;
	BytePtr += sizeof(uint32_t);

	// get IV into WorkBuf
	if ( ! OPENSSL_LIBTOM_EXCHANGE(teeCryptoRandomBytes)( BytePtr, caAES_128_KEY_SIZE_BYTES ) )
	{
		// this is fatal
		CRYPTODBG("%s: failed to allocate random bytes for key seal IV\n", __FUNCTION__ );
		return nfTCR_CRYPT_RANDOM_FAILURE;
	}
#if 0
	CRYPTODBG("%s: dumping IV...\n", __FUNCTION__);
	teePrintHexDump(BytePtr, caAES_128_KEY_SIZE_BYTES);
#endif
	// save pointer to IV
	IVPtr = BytePtr;

	// now, re-aim BytePtr into SealedKeyPtr (b1)
	BytePtr = SealedKeyPtr;

	// build nfSealedKeyHeader_t at BytePtr
	((nfSealedKeyHeader_t *)BytePtr)->MagicHi = nfSEALEDKEY_MAGIC_HI;
	((nfSealedKeyHeader_t *)BytePtr)->MagicLo = nfSEALEDKEY_MAGIC_LO;

	// get key info
	Result = caGetKeyInfoFromHandle( ClientId, KeyHandle,
							&((nfSealedKeyHeader_t *)BytePtr)->KeyType,
							&((nfSealedKeyHeader_t *)BytePtr)->Algorithm,
							&((nfSealedKeyHeader_t *)BytePtr)->KeyUsageFlags );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: failed to get key info from handle (%08x), returning %08x\n",
					__FUNCTION__, KeyHandle, Result );
		return Result;
	}

	// check key usage flags
	if ( ((nfSealedKeyHeader_t *)BytePtr)->KeyUsageFlags & nfKUF_EXTRACTABLE )
	{
		// not allowed
		CRYPTODBG("%s: trying to seal extractable key, not allowed.\n", __FUNCTION__);
		return nfTCR_OPERATION_NOT_ALLOWED;
	}
	CRYPTODBG("%s: checked key usage flags, okay...\n", __FUNCTION__);

	// Based on key type, get key bits
	switch ( ((nfSealedKeyHeader_t *)BytePtr)->Algorithm )
	{
		// AES key
		case nfCA_AES_CBC:
		case nfCA_AES_GCM:
		case nfCA_AES_CTR:
		case nfCA_AES_KW:
			Result = caGetAesKeyFromHandle( ClientId, KeyHandle,
								&KeyPtr, &((nfSealedKeyHeader_t *)BytePtr)->KeyBitCount );
			break;

		// hmac key
		case nfCA_HMAC:
			Result = caGetHmacKeyFromHandle( ClientId, KeyHandle,
								&KeyPtr, &((nfSealedKeyHeader_t *)BytePtr)->KeyBitCount );
			break;

		default:
			CRYPTODBG("%s: cannot seal this key (alg: %d)\n", __FUNCTION__, ((nfSealedKeyHeader_t *)BytePtr)->Algorithm);
			return nfTCR_OPERATION_NOT_ALLOWED;
	}/* end switch ( ((nfSealedKeyHeader_t *)BytePtr)->KeyType ) */

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: failed to get key bits from handle (%08x)\n", __FUNCTION__, Result );
		return Result;
	}
	CRYPTODBG("%s: got key from handle...\n", __FUNCTION__);

	// else copy key bits
	(void) teeMemcpy(((nfSealedKeyHeader_t *)BytePtr)->KeyBits, KeyPtr, ((nfSealedKeyHeader_t *)BytePtr)->KeyBitCount );

	// how big is nfSealedKeyHeader_t + key bits?
	PlaintextLength = sizeof(nfSealedKeyHeader_t) + ((nfSealedKeyHeader_t *)BytePtr)->KeyBitCount;
	CRYPTODBG("%s: PlaintextLength is %d\n", __FUNCTION__, PlaintextLength);

	// now, re-aim BytePtr back into workbuf; this is where ciphertext will go
	BytePtr = IVPtr + caAES_128_KEY_SIZE_BYTES;

	teePrintHexDump_tag("export sealed key",SealedKeyPtr,PlaintextLength);
	
	// now, encrypt b1 (SealedKeyPtr) into b2 (WorkBuf, just after IV)
	CiphertextLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcEncrypt)( SealedKeyPtr, PlaintextLength, caInternalAesKey, IVPtr,
											BytePtr, caCRYPT_WORKBUF_SIZE - sizeof(uint32_t) - caAES_128_KEY_SIZE_BYTES );

	CRYPTODBG("%s: ciphertextlength is %d\n", __FUNCTION__, CiphertextLength );

	if ( CiphertextLength <= 0 )
	{
		// WTF?! Some sort of internal crypto error?
		CRYPTODBG("%s: teeAes128CbcEncrypt returned %d, bailing.\n", __FUNCTION__, CiphertextLength );
		return nfTCR_CIPHER_ERROR;
	}

	// now, compute HMAC-SHA256 of this mess and append it
	HmacDataLength = sizeof(uint32_t) + caAES_128_KEY_SIZE_BYTES + CiphertextLength;
	HmacLength = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( WorkBuf, HmacDataLength, caInternalHMACKey, caHMAC_SHA256_KEY_SIZE_BYTES, &WorkBuf[HmacDataLength] );

	CRYPTODBG("%s: back from teeHmacSha256, HmacLength is %d\n", __FUNCTION__, HmacLength );

	if ( HmacLength != caHMAC_SHA256_KEY_SIZE_BYTES )
	{
		CRYPTODBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
		return nfTCR_CIPHER_ERROR;
	}

	// almost there: make sure there is room for b64-encoded data
	if ( ( B64Length = Base64encode_len(HmacDataLength + HmacLength)) > SealedKeyMaxLength )
	{
		CRYPTODBG("%s: not enough room at SealedKeyPtr (%d/%d)\n", __FUNCTION__, SealedKeyMaxLength,
				Base64encode_len(HmacDataLength + HmacLength) );
		return nfTCR_OPERAND_TOO_SHORT;
	}

	CRYPTODBG("%s: B64Length is %d, SealedKeyMaxLength is %d\n", __FUNCTION__, B64Length, SealedKeyMaxLength );

	// okay, do the encoding
	B64Length = Base64encode( (char *) SealedKeyPtr,  (const char *) WorkBuf, HmacDataLength + HmacLength );

	//assert(B64Length > 0);

	*SealedKeyLengthPtr = (uint32_t) B64Length;
#if 0
	CRYPTODBG("%s: here's a hexdump of WorkBuf:\n", __FUNCTION__);
	teePrintHexDump(WorkBuf, HmacDataLength + HmacLength);
#endif
	CRYPTODBG("%s: returning %d b64-encoded bytes...\n", __FUNCTION__, B64Length );

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caExportSealedKey */

/*********************************************************************
 * @function caImportSealedKey
 *
 * @brief import sealed key
 *
 * @param[in] ClientId - the TEE client ID
 * @param[out] KeyTypePtr - pointer to where you want key type
 * @param[out] AlgorithmPtr - pointer to where you want key algorithm
 * @param[out] KeyUsageFlagsPtr - pointer to where you want key usage
 * @param[out] KeyBitCountPtr - pointer to where you want key length (in bits)
 * @param[out] KeyHandlePtr - pointer to where you want key handle
 * @param[in] SealedKeyLength - length of sealed key (in bytes)
 * @param[in] SealedKey - array containing key bits
 *
 * @returns nfTGR_SUCCESS if successful, nfTCR_MALFORMED_DATA, nfTCR_INTERNAL_ERROR,
 *         nfTCR_SEALED_KEY_DATA_TOO_LARGE, nfTCR_SEALED_KEY_VERSION_UNSUPPORTED,
 *         nfTCR_CIPHER_ERROR otherwise
 *
 *********************************************************************/
nfTeeCryptoResult_t caImportSealedKey( nfTeeClientId_t 			ClientId,
                       		    	   nfKeyType_t				*KeyTypePtr,
                       		    	   nfCryptoAlgorithm_t		*AlgorithmPtr,
                       		    	   nfKeyUsageFlagsWord_t	*KeyUsageFlagsPtr,
                       		    	   uint32_t					*KeyBitCountPtr,
                       		    	   nfCryptoKeyHandle_t		*KeyHandlePtr,
                       		    	   uint32_t					KeyDataLength,
                       		    	   uint8_t					KeyData[] )
{
	uint8_t				WorkBuf[caCRYPT_WORKBUF_SIZE];
	/*
	 * The following buffer will hold the decrypted key and metadata.
	 * Because we only seal Netflix keys (Kce/Kch), we know the maximum size
	 * key we support (256 bits as of today). And because we only use AES128 to encrypt
	 * sealed keys, we know the maximum padding size (15). So we know the maximum size
	 * of the decrypted data (sizeof(nfSealedKeyHeader_t) + 256/8 + 15).
	 */
	uint8_t				KeyBuf[sizeof(nfSealedKeyHeader_t) + caHMAC_SHA256_KEY_SIZE_BYTES + (caAES_128_KEY_SIZE_BYTES - 1)];
	uint8_t				*BytePtr;
	uint8_t				*HmacPtr;
	uint8_t				*IVPtr;
	int					SealedKeyLength;
	uint32_t			HmacLength;
	uint8_t				HmacValue[caHMAC_SHA256_KEY_SIZE_BYTES];
	uint32_t			EncryptedDataLength;
	uint32_t			DecryptedDataLength;
	nfTeeCryptoResult_t Result;

	/*
	 * Sealed key construction looks like this:
	 *
	 * {Version} {16-byte random IV}
	 * ------------------------------------------------------+
	 *  { nfSealedKeyHeader_t }                              | This portion is AES-128-CBC encrypted
	 *  { padding if sizeof(nfSealedKeyHeader) % 16 != 0 }   | using the pre-pended random IV.
	 * ------------------------------------------------------+
	 * {HMAC-SHA256 of IV + encrypted data}
	 *
	 * The whole thing (IV + encrypted data + HMAC) is b64-encoded. So, we need to
	 * do the following:
	 *
	 *	- sanity-check size
	 *		- reject if less than MIN_IV_SIZE + min_size(nfSealedKeyHeader_t) + sizeof(HMAC-SHA256)
	 * 	- b64 decode the data
	 * 		- reject if not well-formed b64
	 * 	- verify Version
	 * 		- reject if unsupported version
	 * 	- locate the HMAC value, confirm HMAC over IV + encrypted data
	 * 		- reject if this fails
	 * 	- using IV, decrypt data, remove padding
	 * 		- reject if padding is invalid
	 * 	- verify Magic{Hi,Lo}
	 * 		- reject if unrecognized magic
	 * 	- sanity-check keytype, alg, key usage, bit count
	 * 		- reject if anything is funky
	 * 	- (attempt to) load key, assign handle
	 *  	- reject if no slots available (user needs to remove key first)
	 * 	- return key handle to REE
	 */

	// sanity-check size
	if ( KeyDataLength < nfMIN_ENCODED_SEALED_KEYDATA_SIZE )
	{
		CRYPTODBG("%s: sealed key data is too small (%d/%d)\n", __FUNCTION__,
					KeyDataLength, nfMIN_ENCODED_SEALED_KEYDATA_SIZE );
		return nfTCR_OPERAND_TOO_SHORT;
	}

	// make sure decoded data will fit in workbuf
	if ( ( SealedKeyLength = Base64decode_len((const char *)KeyData) ) > (int) sizeof(WorkBuf) )
	{
		CRYPTODBG("%s: can't fit decoded key into work buffer (%d/%d)\n", __FUNCTION__,
				SealedKeyLength, sizeof(WorkBuf) );
		return nfTCR_SEALED_KEY_DATA_TOO_LARGE;
	}
#if 0
	CRYPTODBG("%s: hex-dumping b64 key data; SealedKeyLength: %d\n", __FUNCTION__, SealedKeyLength );
	teePrintHexDump(KeyData, KeyDataLength );
#endif
	// b64 decode the data
	if ( ( SealedKeyLength = Base64decode((char *)WorkBuf, (const char *)KeyData) ) <= 0 )
	{
		CRYPTODBG("%s: Base64decode returned %d\n", __FUNCTION__, SealedKeyLength );
		return nfTCR_INTERNAL_ERROR;
	}
#if 0
	CRYPTODBG("%s: dumping b64-decoded buffer...\n", __FUNCTION__ );
	teePrintHexDump( WorkBuf, SealedKeyLength );
#endif
	// verify version (at beginning of buffer)
	uint32_t *VersionPtr = (uint32_t *) WorkBuf;
	if ( *VersionPtr != caSealedKeyVersion )
	{
		CRYPTODBG("%s: unrecognized version (%08x)\n", __FUNCTION__, *VersionPtr );
		return nfTCR_SEALED_KEY_VERSION_UNSUPPORTED;
	}

	// get pointer to HMAC value
	HmacPtr = WorkBuf + SealedKeyLength - caHMAC_SHA256_KEY_SIZE_BYTES;

	HmacLength = HmacPtr - WorkBuf;

	// compute HMAC over version + IV + encrypted data
	if ( OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( WorkBuf, HmacLength, caInternalHMACKey,
			caHMAC_SHA256_KEY_SIZE_BYTES, HmacValue ) != caHMAC_SHA256_KEY_SIZE_BYTES )
	{
		CRYPTODBG("%s: internal error computing HMAC\n", __FUNCTION__);
		return nfTCR_INTERNAL_ERROR;
	}

	//		
#if TEE_LIBTOM_TEST_ENABLE
	teePrintHexDump_tag("[teeHmacSha256] HmacValue :", HmacValue, sizeof(HmacValue) );
	uint8_t 	HmacValue__[caHMAC_SHA256_KEY_SIZE_BYTES];
	teeBzero(HmacValue__,caHMAC_SHA256_KEY_SIZE_BYTES);
	int rc__ = teeHmacSha256_libtom(WorkBuf, HmacLength, caInternalHMACKey,
				caHMAC_SHA256_KEY_SIZE_BYTES, HmacValue__ );
	teePrintHexDump_tag("[teeHmacSha256_libtom] HmacValue__ :", HmacValue__, sizeof(HmacValue__) );
	printf("[sahala compare_buffer]HmacValue vs. HmacValue__\n");
	int Ret = compare_buffer(HmacValue,HmacValue__,sizeof(HmacValue),sizeof(HmacValue__));
	if(Ret != 0)
		exit(1);

#endif
	

	// compare HMAC values
	if ( teeMemcmp( HmacPtr, HmacValue, caHMAC_SHA256_KEY_SIZE_BYTES ) != 0 )
	{
		CRYPTODBG("%s: HMAC mismatch, rejecting.\n", __FUNCTION__ );
		return nfTCR_SEALED_KEY_VALIDATION_ERROR;
	}

	// get pointer to IV
	IVPtr = WorkBuf + sizeof(uint32_t);

	// get pointer to encrypted data
	BytePtr = IVPtr + caAES_128_KEY_SIZE_BYTES;

	// Make sure we have enough room for the decrypted blob.
	EncryptedDataLength = (uint32_t)((unsigned)HmacPtr - (unsigned)BytePtr);
	if ( EncryptedDataLength > sizeof(KeyBuf) )
	{
		CRYPTODBG("%s: sealed data is too long (%d)\n", __FUNCTION__, EncryptedDataLength );
		return nfTCR_INTERNAL_ERROR;
	}

	// decrypt the key and metadata
	DecryptedDataLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcDecrypt)( BytePtr, EncryptedDataLength,
									caInternalAesKey, IVPtr, KeyBuf, sizeof(KeyBuf) );


#if TEE_LIBTOM_TEST_ENABLE
	teePrintHexDump_tag("[teeAes128CbcDecrypt] KeyBuf :", KeyBuf, DecryptedDataLength );
	uint8_t 	KeyBuf__[sizeof(nfSealedKeyHeader_t) + caHMAC_SHA256_KEY_SIZE_BYTES + (caAES_128_KEY_SIZE_BYTES - 1)];
	teeBzero(KeyBuf__,sizeof(nfSealedKeyHeader_t) + caHMAC_SHA256_KEY_SIZE_BYTES + (caAES_128_KEY_SIZE_BYTES - 1));
	rc__ = teeAes128CbcDecrypt_libtom( BytePtr, EncryptedDataLength,
									caInternalAesKey, IVPtr, KeyBuf__, sizeof(KeyBuf__));
	teePrintHexDump_tag("[teeAes128CbcDecrypt_libtom] KeyBuf__ :", KeyBuf__, rc__);
	printf("[sahala compare_buffer]KeyBuf vs. KeyBuf__\n");
	Ret = compare_buffer(KeyBuf,KeyBuf__,DecryptedDataLength,rc__);
	if(Ret != 0)
		exit(1);
	teePrintHexDump_tag("import seal key",KeyBuf,DecryptedDataLength);
#endif


	// make sure decryption succeeded
	if ( DecryptedDataLength < nfUNSEALED_KEY_MIN_LENGTH )
	{
		CRYPTODBG("%s: decryption failed (length: %d)\n", __FUNCTION__, DecryptedDataLength );
		return nfTCR_CIPHER_ERROR;
	}

	/*
	 * KeyBuf now contains nfSealedKeyHeader_t. Make sure the contents make sense.
	 */
	// verify magic
	nfSealedKeyHeader_t *HdrPtr = (nfSealedKeyHeader_t *)KeyBuf;
	if ( ( HdrPtr->MagicHi != nfSEALEDKEY_MAGIC_HI )
	  || ( HdrPtr->MagicLo != nfSEALEDKEY_MAGIC_LO ) )
	{
		CRYPTODBG("%s: key magic mismatch (lo: %08x; hi: %08x)\n", __FUNCTION__,
					HdrPtr->MagicHi,
					HdrPtr->MagicLo );
		return nfTCR_MALFORMED_DATA;
	}

	// verify key type
	if ( HdrPtr->KeyType != nfKT_SECRET )
	{
		CRYPTODBG("%s: invalid keytype (%d)\n", __FUNCTION__, HdrPtr->KeyType );
		return nfTCR_MALFORMED_DATA;
	}

	// verify algorithm
	if ( ( HdrPtr->Algorithm != nfCA_AES_CBC )
	  && ( HdrPtr->Algorithm != nfCA_AES_KW )
	  && ( HdrPtr->Algorithm != nfCA_HMAC ) )
	{
		CRYPTODBG("%s: invalid algorithm (%d)\n", __FUNCTION__, HdrPtr->Algorithm );
		return nfTCR_MALFORMED_DATA;
	}

	// verify key usage flags and key bit count
	if ( HdrPtr->Algorithm == nfCA_AES_CBC )
	{
		// Kce
		if ( HdrPtr->KeyUsageFlags & ~(nfKUF_ENCRYPT | nfKUF_DECRYPT) )
		{
			CRYPTODBG("%s: invalid KeyUsage for Kce (%08x)\n", __FUNCTION__,
						HdrPtr->KeyUsageFlags );
			return nfTCR_MALFORMED_DATA;
		}
		// verify key bit count
//		if ( HdrPtr->KeyBitCount != (caAES_128_KEY_SIZE_BYTES * 8))
		if ( HdrPtr->KeyBitCount != (caAES_128_KEY_SIZE_BYTES))
		{
			CRYPTODBG("%s: invalid KeyBitCount for Kce (%0d)\n", __FUNCTION__,
						HdrPtr->KeyBitCount );
			return nfTCR_MALFORMED_DATA;

		}
	}
	else if ( HdrPtr->Algorithm == nfCA_AES_KW )
	{
		// Kcw
		if ( HdrPtr->KeyUsageFlags & ~(nfKUF_WRAP | nfKUF_UNWRAP) )
		{
			CRYPTODBG("%s: invalid KeyUsage for Kcw (%08x)\n", __FUNCTION__,
						HdrPtr->KeyUsageFlags );
			return nfTCR_MALFORMED_DATA;
		}
		// verify key bit count
//		if ( HdrPtr->KeyBitCount != (caAES_128_KEY_SIZE_BYTES * 8))
		if ( HdrPtr->KeyBitCount != (caAES_128_KEY_SIZE_BYTES))
		{
			CRYPTODBG("%s: invalid KeyBitCount for Kcw (%0d)\n", __FUNCTION__,
						HdrPtr->KeyBitCount );
			return nfTCR_MALFORMED_DATA;

		}
	}
	else
	{
		// Kch
		if ( HdrPtr->KeyUsageFlags & ~(nfKUF_SIGN | nfKUF_VERIFY) )
		{
			CRYPTODBG("%s: invalid KeyUsage for Kch (%08x)\n", __FUNCTION__,
						HdrPtr->KeyUsageFlags );
			return nfTCR_MALFORMED_DATA;
		}
		// verify key bit count
//		if ( HdrPtr->KeyBitCount != (caHMAC_SHA256_KEY_SIZE_BYTES * 8))
		if ( HdrPtr->KeyBitCount != (caHMAC_SHA256_KEY_SIZE_BYTES))
		{
			CRYPTODBG("%s: invalid KeyBitCount for Kch (%0d)\n", __FUNCTION__,
						HdrPtr->KeyBitCount );
			return nfTCR_MALFORMED_DATA;

		}

	}/* end else ( HdrPtr->Algorithm != nfCA_AES_CBC ) */

	// finally, add key and return result...
	Result = caImportKey( ClientId, nfKF_RAW, HdrPtr->KeyBits,
				HdrPtr->KeyBitCount,
				HdrPtr->Algorithm,
				HdrPtr->KeyUsageFlags,
				KeyHandlePtr, KeyTypePtr );

	// don't modify out parms unless we succeeded...
	if ( Result == (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		*AlgorithmPtr = HdrPtr->Algorithm;
		*KeyBitCountPtr = HdrPtr->KeyBitCount;
		*KeyUsageFlagsPtr = HdrPtr->KeyUsageFlags;
		*KeyTypePtr = HdrPtr->KeyType;
	}

	return Result;

}/* end caImportSealedKey */

/*********************************************************************
 * @function caDeriveNflxKcd
 *
 * @brief derive kcd from kce/kch (using Netflix HKDF variant)
 *
 * @param[in] Kce - 128-bit AES key
 * @param[in] Kch - 256-bit hmac key
 * @param[out] Kcd - where you want Kcd added
 * @param[in] KcdMaxLength - maximum length of Kcd
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static int caDeriveNflxKcd( uint8_t Kce[], uint8_t Kch[], uint8_t Kcd[], uint32_t KcdMaxLength )
{
	int		rc;

#define caKPW_LEN 16
	uint8_t Salt[caKPW_LEN] = {0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53,
							   0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69};
	uint8_t Info[caKPW_LEN] = {0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d,
							   0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91 };
#define caSHA256_LEN 32
	uint8_t FirstResult[caSHA256_LEN]; // HMAC-SHA256(salt, Kde||Kdh)
	uint8_t SecondResult[caSHA256_LEN]; // HMAC-SHA256(FirstResult, info))

#define caKPE_LEN 16
#define caKPH_LEN 32
#define caCONCAT_KEY_LEN (caKPE_LEN + caKPH_LEN) // concatentate Kpe|Kph
	uint8_t	ConcatKeys[caCONCAT_KEY_LEN];

	/* Kcw = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, Kce||Kch), info)) */

	// concatenate Kce|Kch
	(void) teeMemcpy(ConcatKeys, Kce, caKPE_LEN);
	(void) teeMemcpy(ConcatKeys + caKPE_LEN, Kch, caKPH_LEN);

	// compute FirstResult: key is Salt[], data is concatenated keys
	rc = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( ConcatKeys, caCONCAT_KEY_LEN, Salt, caKPW_LEN, FirstResult );
	if ( rc != caSHA256_LEN )
	{
		CRYPTODBG("%s: teeHmacSha256(FirstResult) failed, returned %d\n", __FUNCTION__, rc );
		return nfTCR_INTERNAL_ERROR;
	}

	// compute SecondResult: key is FirstResult, data is Info[]
	rc = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( Info, caKPW_LEN, FirstResult, caSHA256_LEN, SecondResult );
	if ( rc != caSHA256_LEN )
	{
		CRYPTODBG("%s: teeHmacSha256(SecondResult) failed, returned %d\n", __FUNCTION__, rc );
		return nfTCR_INTERNAL_ERROR;
	}

	if ( KcdMaxLength < caKPE_LEN )
	{
		CRYPTODBG("%s: not enough room for Kcd (%d/%d)\n", __FUNCTION__,
					KcdMaxLength, caKPE_LEN );
		return nfTCR_INTERNAL_ERROR;
	}

	// now, copy first 128 bits into Kpw
	(void) teeMemcpy(Kcd, SecondResult, caKPE_LEN );

	CRYPTODBG("%s: returning success.\n", __FUNCTION__);

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caDeriveNflxKcd */

/*********************************************************************
 * @function caDeriveNflxDhKeys
 *
 * @brief using DH shared secret and key derivation key, generate keys
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] SharedSecretPtr - pointer to shared secret
 * @param[in] SharedSecretLength - length of shared secret
 * @param[in] KeyDerivationKeyHandle - handle for key derivation key
 * @param[in] KceHandle - Handle for insertion of Kce into key table
 * @param[in] KchHandle - Handle for insertion of Kch into key table
 * @param[in] KcdHandle - Handle for insertion of Kcd into key table
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caDeriveNflxDhKeys( nfTeeClientId_t 	ClientId,
										uint8_t 			*SharedSecretPtr,
							 	 	 	uint32_t 			SharedSecretLength,
							 	 	 	nfCryptoKeyHandle_t	KeyDerivationKeyHandle,
							 	 	 	nfCryptoKeyHandle_t	KceHandle,
							 	 	 	nfCryptoKeyHandle_t	KchHandle,
							 	 	 	nfCryptoKeyHandle_t	KcdHandle )
{
	int					KeyIndex;
	uint8_t				*KcePtr;
	uint8_t				*KchPtr;
	uint8_t				*KeyDerivationKeyPtr = NULL;
	uint32_t			KeyDerivationKeyLength = 0;
	uint8_t				Kcd[caKPE_LEN];
	nfTeeCryptoResult_t	Result;
	nfKeyContext_t		KeyContext;

	// XXX HACK ALERT -- need to move derivation somewhere...
#define caSHA384_LEN 48
	uint8_t HmacKey[caSHA384_LEN]; // contains SHA384(KeyDerivationKey)
	uint8_t HmacResult[caSHA384_LEN]; // HMAC-SHA384(HmacKey, SharedSecret)


	/*
	 * We need to do the following:
	 * 	- make sure this ClientId has access to this KeyDerivationKey
	 * 		- get key index from key handle
	 * 		- verify client ID and key
	 * 	- do the key derivation
	 * 		- first, derive Kce/Kch using HMAC-SHA384
	 * 			- Kce|Kch = HMAC-SHA384(SHA384(KeyDerivationKey), SharedSecret)
	 * 		- then, derive Kcd from Kce/Kch using HKDF
	 * 			- Kcd = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, Kce||Kch), info))
	 * 	- add keys to key table
	 * 		- add Kce using KceHandle
	 * 		- add Kch using KchHandle
	 * 		- add Kcd using KdeHandle
	 */

	// get key index from handle
	if ( (KeyIndex = caGetIndexFromHandle(KeyDerivationKeyHandle)) == caKeyInvalidIndex )
	{
		CRYPTODBG("%s: KeyDerivationKeyHandle invalid.\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	CRYPTODBG("%s: successfully retrieved KDK index (%d).\n", __FUNCTION__, KeyIndex );

	/*
	 * First step: get KeyDerivationKey, compute HmacKey
	 */
	Result = caGetKeyDerivationKeyFromHandle( ClientId, KeyDerivationKeyHandle,
  	   	   	 	 	 	 	 	 &KeyDerivationKeyPtr,&KeyDerivationKeyLength );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyDerivationKeyFromHandle failed.\n", __FUNCTION__ );
		return Result;
	}

	// compute HmacKey
	if ( OPENSSL_LIBTOM_EXCHANGE(teeSha384)( KeyDerivationKeyPtr, KeyDerivationKeyLength, HmacKey) != 0 )
	{
		CRYPTODBG("%s: processing KDK failed (%p/%d)\n", __FUNCTION__,
				KeyDerivationKeyPtr, KeyDerivationKeyLength );
		return nfTCR_INTERNAL_ERROR;
	}

#if TEE_LIBTOM_TEST_ENABLE

	teePrintHexDump_tag("HmacKey",HmacKey,sizeof(HmacKey)); 
	int	rc__;

	uint8_t HmacKey__[caSHA384_LEN];
	teeBzero(HmacKey__,caSHA384_LEN);
	teeSha384_libtom(KeyDerivationKeyPtr,KeyDerivationKeyLength,HmacKey__);

	teePrintHexDump_tag("HmacKey__",HmacKey__,sizeof(HmacKey__)); 
	int Ret = compare_buffer(HmacKey,HmacKey__,caSHA384_LEN,caSHA384_LEN);
	if(Ret != 0)
		exit(1);

#endif

	CRYPTODBG("%s: successfully computed HmacKey\n", __FUNCTION__);


	// now, compute Kce|Kch
	if ( OPENSSL_LIBTOM_EXCHANGE(teeHmacSha384)( SharedSecretPtr, SharedSecretLength,
					HmacKey, sizeof(HmacKey), HmacResult ) != sizeof(HmacResult) )
	{
		CRYPTODBG("%s: teeHmacSha384 failed.\n", __FUNCTION__);
		return nfTCR_INTERNAL_ERROR;
	}

#if TEE_LIBTOM_TEST_ENABLE

	teePrintHexDump_tag("HmacResult",HmacResult,sizeof(HmacResult)); 

	uint8_t HmacResult__[caSHA384_LEN];
	teeBzero(HmacResult__,caSHA384_LEN);
	teeHmacSha384_libtom(SharedSecretPtr, SharedSecretLength,
					HmacKey, sizeof(HmacKey), HmacResult__);

	teePrintHexDump_tag("HmacResult__",HmacResult__,sizeof(HmacResult__)); 
	Ret = compare_buffer(HmacResult,HmacResult__,caSHA384_LEN,caSHA384_LEN);
	if(Ret != 0)
		exit(1);

#endif	

	// set up KcePtr and KchPtr
	KcePtr = HmacResult;
	KchPtr = HmacResult + caKPE_LEN;

	CRYPTODBG("%s: computed Kce/Kch; deriving Kcd...\n", __FUNCTION__ );

	// now, compute Kcd
	Result = caDeriveNflxKcd( KcePtr, KchPtr, Kcd, sizeof(Kcd) );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caDeriveNflxKcd failed.\n", __FUNCTION__);
		return Result;
	}

	/*
	 * Add Kce to key table
	 * 	- set up KeyContext
	 * 	- call caInsertKey
	 * 	- check result (bail on failure)
	 */
	(void) teeBzero( &KeyContext, sizeof(KeyContext) );
	KeyContext.ContextType = caKC_RAW;
	(void) teeMemcpy( KeyContext.ctxt.rawContext.KeyData, KcePtr, caKPE_LEN );
	KeyContext.ctxt.rawContext.KeyDataLength = caKPE_LEN;

	CRYPTODBG("%s: inserting Kce...\n", __FUNCTION__ );

	// finally, insert key
	Result = caInsertKey( ClientId, KceHandle, &KeyContext, nfKF_RAW, nfCA_AES_CBC,
					(nfKeyUsageFlagsWord_t)(nfKUF_ENCRYPT | nfKUF_DECRYPT) );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caInsertKey(Kce) failed.\n", __FUNCTION__);
		return Result;
	}

	/*
	 * Add Kch to key table
	 * 	- set up KeyContext
	 * 	- call caInsertKey
	 * 	- check result (bail on failure)
	 */
//	(void) teeBzero( &KeyContext, sizeof(KeyContext) );
//	KeyContext.ContextType = caKC_RAW;
	(void) teeMemcpy( KeyContext.ctxt.rawContext.KeyData, KchPtr, caKPH_LEN );
	KeyContext.ctxt.rawContext.KeyDataLength = caKPH_LEN;

	// finally, insert key
	CRYPTODBG("%s: inserting Kch...\n", __FUNCTION__ );
	Result = caInsertKey( ClientId, KchHandle, &KeyContext, nfKF_RAW, nfCA_HMAC,
							(nfKeyUsageFlagsWord_t)(nfKUF_SIGN | nfKUF_VERIFY) );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caInsertKey(Kch) failed.\n", __FUNCTION__);
		return Result;
	}

	/*
	 * Add Kcd to key table
	 * 	- set up KeyContext
	 * 	- call caInsertKey
	 * 	- check result (bail on failure)
	 */
//	(void) teeBzero( &KeyContext, sizeof(KeyContext) );
//	KeyContext.ContextType = caKC_RAW;
	(void) teeMemcpy( KeyContext.ctxt.rawContext.KeyData, Kcd, caKPE_LEN );
	KeyContext.ctxt.rawContext.KeyDataLength = caKPE_LEN;

	// finally, insert key
	CRYPTODBG("%s: inserting Kcd...\n", __FUNCTION__ );
	Result = caInsertKey( ClientId, KcdHandle, &KeyContext, nfKF_RAW,
							nfCA_AES_KW, nfKUF_DERIVE );

	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caInsertKey(Kch) failed.\n", __FUNCTION__);
		return Result;
	}

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caDeriveNflxDhKeys */

/*********************************************************************
 * @function caGenDhKeys
 *
 * @brief using existing DH context and peer public value, generate keys
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] DHPrivKeyHandle - the DH private key handle
 * @param[in] KeyDerivationKeyHandle - handle for key derivation key
 * @param[in] PeerPubLength - length of peer DH public value
 * @param[in] DHPubValuePtr - pointer to peer DH public value
 * @param[out] KceHandlePtr - pointer to where you want Kce handle stored
 * @param[out] KchHandlePtr - pointer to where you want Kch handle stored
 * @param[out] KcdHandlePtr - pointer to where you want Kcd handle stored
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
nfTeeCryptoResult_t caDhDeriveKeys( nfTeeClientId_t 	ClientId,
									nfCryptoKeyHandle_t DHPrivKeyHandle,
									nfCryptoKeyHandle_t	KeyDerivationKeyHandle,
									uint32_t			PeerPubLength,
									uint8_t				*DHPubValuePtr,
									nfCryptoKeyHandle_t *KceHandlePtr,
									nfCryptoKeyHandle_t	*KchHandlePtr,
									nfCryptoKeyHandle_t	*KcdHandlePtr )
{
	nfTeeCryptoResult_t 	Result;
	nfCryptoKeyHandle_t		KceHandle = caKeyInvalidHandle;
	nfCryptoKeyHandle_t		KchHandle = caKeyInvalidHandle;
	nfCryptoKeyHandle_t		KcdHandle = caKeyInvalidHandle;
	int						Index;
	nfKeyType_t 			KeyType = nfKT_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = nfKUF_UNDEFINED;
	nfDHContext_t			*DhContextPtr = NULL;
	int						rc;

	/*
	 * We need to do the following:
	 * 	- validate DHPrivKeyHandle
	 * 		- it's a valid handle for this client
	 * 		- it's a DH private key
	 * 	- validate KeyDerivationKeyHandle
	 * 		- it's a valid handle for this client
	 * 		- it's a valid KDK
	 * 	- make sure we have enough space in key table
	 * 	- do diffie hellman shared secret derivation
	 * 	- derive keys from shared secret
	 * 	- add keys to key table
	 * 	- save key handles for caller
	 */

	// validate DHPrivKeyHandle
	if ( ( Index = caGetIndexFromHandle( DHPrivKeyHandle )) != caKeyInvalidIndex )
	{
		if ( caClientIdMatchesKeyHandle( ClientId, DHPrivKeyHandle ) == nfBT_FALSE )
		{
			// DHPrivKeyHandle does not belong to this client
			CRYPTODBG("%s: clientid doesn't match dh priv key handle.\n", __FUNCTION__);
			return nfTCR_BAD_PARAMETER;
		}
		else
		{
			Result = caGetKeyInfoFromHandle( ClientId, DHPrivKeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );
			if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
			{
				CRYPTODBG("%s: couldn't get key info for (valid?) DHPrivKeyHandle.\n", __FUNCTION__);
				return nfTCR_INTERNAL_ERROR;
			}
			// make sure this is a DH priv key
			if ( ( ( Algorithm != nfCA_NFLX_DH ) && ( Algorithm != nfCA_DH_ALGO) )
			  || ( KeyType != nfKT_PRIVATE )
			  || ( ( KeyUsageFlags & nfKUF_DERIVE ) == 0 ) )
			{
				CRYPTODBG("%s: inconsistent key policy (%d/%d/%d) for DHPriv\n", __FUNCTION__,
						Algorithm, KeyType, KeyUsageFlags );
				return nfTCR_OPERATION_NOT_ALLOWED;

			}
		}
	}
	else
	{
		// DHPrivKeyHandle is not a valid handle
		CRYPTODBG("%s: DH priv key handle invalid.\n", __FUNCTION__);
		return nfTCR_BAD_PARAMETER;
	}

	// validate KeyDerivationKeyHandle
	if ( ( Index = caGetIndexFromHandle( KeyDerivationKeyHandle )) != caKeyInvalidIndex )
	{
		if ( caClientIdMatchesKeyHandle( ClientId, KeyDerivationKeyHandle ) == nfBT_FALSE )
		{
			// KeyDerivationKeyHandle does not belong to this client
			CRYPTODBG("%s: clientid doesn't match kdk handle.\n", __FUNCTION__);
			return nfTCR_BAD_PARAMETER;
		}
		else
		{
			Result = caGetKeyInfoFromHandle( ClientId, KeyDerivationKeyHandle, &KeyType,
											  &Algorithm, &KeyUsageFlags );
			if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
			{
				CRYPTODBG("%s: couldn't get key info for (valid?) KeyDerivationKeyHandle.\n", __FUNCTION__);
				return nfTCR_INTERNAL_ERROR;
			}
			// make sure this is a DH key derivation key
			if ( ( ( Algorithm != nfCA_NFLX_DH )
				&& ( Algorithm != nfCA_AES_KW ) )
			  || ( KeyType != nfKT_SECRET ) )
			{
				CRYPTODBG("%s: inconsistent key policy (%d/%d/%d) for kdk\n", __FUNCTION__,
						Algorithm, KeyType, KeyUsageFlags );
				return nfTCR_OPERATION_NOT_ALLOWED;

			}
		}
	}
	else
	{
		// KeyDerivationKeyHandle is not a valid handle
		CRYPTODBG("%s: key deriv key handle (%d) invalid.\n", __FUNCTION__ ,KeyDerivationKeyHandle);
		return nfTCR_BAD_PARAMETER;
	}

	// make sure we have enough space in key table
	if ( ( ( KceHandle = caAllocateKeyHandle() ) == caKeyInvalidHandle )
	  || ( ( KchHandle = caAllocateKeyHandle() ) == caKeyInvalidHandle )
	  || ( ( KcdHandle = caAllocateKeyHandle() ) == caKeyInvalidHandle ) )
	{
		CRYPTODBG("%s: failed to allocate key handle (%d/%d/%d)\n", __FUNCTION__,
				  KceHandle, KchHandle, KcdHandle );
		Result = nfTCR_KEY_STORE_FULL;
		goto ERROR;
	}

	/*
	 *  do diffie hellman shared secret derivation
	 *  	- get dh context
	 *  	- derive shared secret
	 */
	Result = caGetDhContextFromHandle( ClientId, DHPrivKeyHandle, &DhContextPtr );
	if ( Result != (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: failed to get DH context.\n", __FUNCTION__ );
		goto ERROR;
	}

	CRYPTODBG("%s: DH generator: %08x\n", __FUNCTION__, DhContextPtr->Generator);
#if 0
	CRYPTODBG("%s: DH modulus:\n", __FUNCTION__);
	teePrintHexDump(DhContextPtr->Modulus, DhContextPtr->ModulusLength );
#endif

	rc = OPENSSL_LIBTOM_EXCHANGE(teeComputeDhSharedSecret)( DhContextPtr->Generator,
														   DhContextPtr->ModulusLength,
														   DhContextPtr->Modulus,
														   DhContextPtr->PrivKeyLength,
														   DhContextPtr->PrivKey,
														   PeerPubLength,
														   DHPubValuePtr,
														   nfMAX_DH_MODULUS_LENGTH - 1,
														   &DhContextPtr->SharedSecretLength,
														   DhContextPtr->SharedSecret);

	if ( rc != 0 )
	{
		CRYPTODBG("%s: teeComputeDhSharedSecret() failed.\n", __FUNCTION__);
		goto ERROR;
	}

#if TEE_LIBTOM_TEST_ENABLE

	teePrintHexDump_tag("DhContextPtr->SharedSecret",DhContextPtr->SharedSecret,DhContextPtr->SharedSecretLength); 
	int	rc__;
	uint8_t temp_sharedSecureKey[nfMAX_DH_MODULUS_LENGTH];
	teeBzero(temp_sharedSecureKey,nfMAX_DH_MODULUS_LENGTH);
	uint32_t temp_sharedSecureKey_len = 0;
	
	rc__ = teeComputeDhSharedSecret_libtom( DhContextPtr->Generator,
										   DhContextPtr->ModulusLength,
										   DhContextPtr->Modulus,
										   DhContextPtr->PrivKeyLength,
										   DhContextPtr->PrivKey,
										   PeerPubLength,
										   DHPubValuePtr,
										   nfMAX_DH_MODULUS_LENGTH - 1,
										   &temp_sharedSecureKey_len,
										   temp_sharedSecureKey);
	teePrintHexDump_tag("temp_sharedSecureKey",temp_sharedSecureKey,temp_sharedSecureKey_len); 
	int Ret = compare_buffer(DhContextPtr->SharedSecret,temp_sharedSecureKey,DhContextPtr->SharedSecretLength,temp_sharedSecureKey_len);
	if(Ret != 0)
		exit(1);

#endif

   /*
	Per MSL spec:

	Since the computed shared secret is a numeric value (typically a BigInteger) it
	must be converted into a byte array when computing the HMAC-SHA384. The byte array
	will be the minimum number of bytes required for the two's complement representation
	in big-endian byte-order (the most significant byte is first) including at least one
	sign bit, with exactly one zero byte in the zeroth element. As a result, a shared
	secret value of zero will be represented by an array of length one containing a single
	byte with a value of zero. This representation is compatible with the Java
	BigInteger.toByteArray() function and BigInteger(byte[]) constructor.

	So, essentially, if the first byte of the shared secret is not zero, we prepend
	a zero byte.
	*/

	if ( DhContextPtr->SharedSecret[0] != '\0' )
	{
		int i;
		CRYPTODBG("%s: pre-pending 0 byte to DH shared secret...\n", __FUNCTION__);
		// PITA: we have to fix this up. Shift it over by one byte
		for ( i = nfMAX_DH_MODULUS_LENGTH - 1; i > 0; --i )
			DhContextPtr->SharedSecret[i] = DhContextPtr->SharedSecret[i - 1];

		// prepend 0 byte...
		DhContextPtr->SharedSecret[0] = '\0';

		// update length
		++DhContextPtr->SharedSecretLength;
	}

#if 0
	CRYPTODBG("%s: dumping DH shared secret:\n", __FUNCTION__);
	teePrintHexDump(DhContextPtr->SharedSecret, DhContextPtr->SharedSecretLength );
#endif

	// derive keys from shared secret (this adds them to key table)
	Result = caDeriveNflxDhKeys( ClientId, DhContextPtr->SharedSecret,
								 DhContextPtr->SharedSecretLength,
								 KeyDerivationKeyHandle,
								 KceHandle, KchHandle, KcdHandle );

	if ( Result == (nfTeeCryptoResult_t) nfTGR_SUCCESS )
	{
		// save key handles for caller
		*KceHandlePtr = KceHandle;
		*KchHandlePtr = KchHandle;
		*KcdHandlePtr = KcdHandle;
	}

	return Result;

// NOTREACHED
ERROR:
	if ( KceHandle != caKeyInvalidHandle )
		(void) caDeallocateKeyHandle( KceHandle );
	if ( KchHandle != caKeyInvalidHandle )
		(void) caDeallocateKeyHandle( KchHandle );
	if ( KcdHandle != caKeyInvalidHandle )
		(void) caDeallocateKeyHandle( KcdHandle );
	return Result;

}/* end caDhDeriveKeys */

/*********************************************************************
 * @function caGenDhKeys
 *
 * @brief create DH context and load into key table, returning DH pub value
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] Generator - the DH generator value
 * @param[in] ModulusPtr - pointer to DH modulus
 * @param[in] ModulusLength - length of DH modulus
 * @param[out] PrivKeyHandlePtr - pointer to where you want priv key handle
 *
 * @returns nfTGR_SUCCESS if successful,
 *********************************************************************/
nfTeeCryptoResult_t caDhGenKeys( nfTeeClientId_t 		ClientId,
							     uint32_t 				Generator,
							     //uint8_t*				RandomKey,
   							     //uint32_t				RandomKeyLen,
							     uint8_t 				*ModulusPtr,
							     uint32_t 				ModulusLength,
							     nfCryptoKeyHandle_t 	*PrivKeyHandlePtr )
{
	nfCryptoKeyHandle_t		PrivKeyHandle;
	nfKeyContext_t 			KeyContext;
	int						rc;
	nfCryptoAlgorithm_t		Algorithm = nfCA_DH_ALGO;
	nfKeyUsageFlagsWord_t	KeyUsageFlags = nfKUF_DERIVE;
	nfKeyType_t				KeyType = nfKT_PRIVATE;
	nfTeeCryptoResult_t 	Result;

    CRYPTODBG("%s: calling caInsertKey with handle %d, keytype %d, Algorithm %d, KeyUsageFlags %08x\n",
    			__FUNCTION__,-1, KeyType, Algorithm, KeyUsageFlags );

	/*
	 * We need to do the following:
	 * 	- generate DH key pair
	 * 		- pass key context fields to DH generation fn
	 * 	- if successful
	 * 		- copy generator, modulus, modulus length into KeyContext
	 *
	 */

	// clear key context struct
	(void) teeBzero(&KeyContext, sizeof(KeyContext));
	#if 0
	if (RandomKey == NULL || ( RandomKeyLen != nfMAX_DH_PRIVKEY_LENGTH))
	{
		CRYPTODBG("%s: teeGenDhKeys returned  failing.\n", __FUNCTION__ );
		return nfTCR_INTERNAL_ERROR;
	}
	#endif
	//re-generate random key
	uint8_t randBuf[nfMAX_DH_PRIVKEY_LENGTH];
	teeMemset(randBuf,0,nfMAX_DH_PRIVKEY_LENGTH);
	OPENSSL_LIBTOM_EXCHANGE(teeCryptoRandomBytes)(randBuf,nfMAX_DH_PRIVKEY_LENGTH);

	teePrintHexDump_tag("[random buffer]",randBuf,nfMAX_DH_PRIVKEY_LENGTH);
	
	teeMemcpy(KeyContext.ctxt.dhContext.PrivKey,randBuf,nfMAX_DH_PRIVKEY_LENGTH);
	KeyContext.ctxt.dhContext.PrivKeyLength = nfMAX_DH_PRIVKEY_LENGTH;
	
	/* generate keys */
	rc = OPENSSL_LIBTOM_EXCHANGE(teeGenDhKeys)( Generator,
											   ModulusLength, ModulusPtr,
											   &KeyContext.ctxt.dhContext.PrivKeyLength,
											   nfMAX_DH_PRIVKEY_LENGTH, KeyContext.ctxt.dhContext.PrivKey,
											   &KeyContext.ctxt.dhContext.PubKeyLength,
											   nfMAX_DH_MODULUS_LENGTH,
											   KeyContext.ctxt.dhContext.PubKey );
	if ( rc != 0 )
	{
		CRYPTODBG("%s: teeGenDhKeys returned %d, failing.\n", __FUNCTION__, rc );
		// XXX - need more granular errors
		return nfTCR_INTERNAL_ERROR;
	}
	
    CRYPTODBG("%s: calling caInsertKey with handle %d, keytype %d, Algorithm %d, KeyUsageFlags %08x\n",
    			__FUNCTION__,-1, KeyType, Algorithm, KeyUsageFlags );
	//
#if 0

	teePrintHexDump_tag("KeyContext PubKey",KeyContext.ctxt.dhContext.PubKey,KeyContext.ctxt.dhContext.PubKeyLength); 
	int	rc__;
	//using above private key to test dh 
	nfKeyContext_t	KeyContext__;
	teeGenDhKeys_libtom_test( Generator,
				   ModulusLength, ModulusPtr,
				   &(KeyContext.ctxt.dhContext.PrivKeyLength),
				   nfMAX_DH_PRIVKEY_LENGTH, KeyContext.ctxt.dhContext.PrivKey,
				   &KeyContext__.ctxt.dhContext.PubKeyLength,
				   nfMAX_DH_MODULUS_LENGTH,
				   KeyContext__.ctxt.dhContext.PubKey );
		
	teePrintHexDump_tag("KeyContext__ PubKey",KeyContext__.ctxt.dhContext.PubKey,KeyContext__.ctxt.dhContext.PubKeyLength);	

	int Ret = compare_buffer(KeyContext.ctxt.dhContext.PubKey,KeyContext__.ctxt.dhContext.PubKey,
								KeyContext.ctxt.dhContext.PubKeyLength,KeyContext__.ctxt.dhContext.PubKeyLength);
	if(Ret != 0)
		exit(1);

#endif

	// make sure we can get a handle before we do any more work
    PrivKeyHandle = caAllocateKeyHandle();
    if (PrivKeyHandle == caKeyInvalidHandle)
    {
    	CRYPTODBG("%s: returning KEY_STORE_FULL...\n", __FUNCTION__);
    	return nfTCR_KEY_STORE_FULL;
    }

    // copy DH values into KeyContext
    KeyContext.ContextType = caKC_DH;
    KeyContext.ctxt.dhContext.Generator = Generator;
    KeyContext.ctxt.dhContext.ModulusLength = ModulusLength;
    (void) teeMemcpy( KeyContext.ctxt.dhContext.Modulus, ModulusPtr, ModulusLength );

    CRYPTODBG("%s: calling caInsertKey with handle %d, keytype %d, Algorithm %d, KeyUsageFlags %08x\n",
    			__FUNCTION__, PrivKeyHandle, KeyType, Algorithm, KeyUsageFlags );

    Result = caInsertKey(ClientId, PrivKeyHandle, &KeyContext,  KeyType, Algorithm, KeyUsageFlags );

    if ( Result == (nfTeeCryptoResult_t) nfTGR_SUCCESS )
    	*PrivKeyHandlePtr = PrivKeyHandle;

    return Result;

}/* end caGenDhKeys */

/*********************************************************************
 * @function caClearKeys
 *
 * @brief clear all non-named keys from key table, return list of deleted handles
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] MaxHandleListSize - maximum space for handles in DeletedHandlesList
 * @param[out] NumHandlesPtr - pointer to count for DeletedHandlesList
 * @param[out] DeletedHandlesList - pointer to where you want list of handles
 *
 * @returns nfTGR_SUCCESS if successful,
 *********************************************************************/
nfTeeCryptoResult_t caClearKeys( nfTeeClientId_t 	ClientId,
	     	 	 	 	 	 	 uint32_t 			MaxHandleListSize,
	     	 	 	 	 	 	 uint32_t			*NumHandlesPtr,
	     	 	 	 	 	 	 uint32_t			DeletedHandlesList[] )
{
	uint32_t 	NamedKeyCount = 0;
	uint32_t 	TotalKeyCount = 0;
	uint32_t	NumHandles = 0;
	int			i,j;

	/*
	 * For now, we need to do the following:
	 *	- count number of named keys in key table
	 *  - count total number of entries in key table
	 *  - if not enough space in DeletedHandlesList, return error
	 * 	- for each key table entry
	 * 		- if this is not a named key
	 * 			- delete key
	 * 			- add handle to DeletedHandlesList
	 * 			- increment NumHandles
	 * 	- store NumHandles at *NumHandlesPtr
	 * 	- simple as that.
	 */

	// Count number of named keys in key table
	for ( i = 0; i < caNK_INDEX_INVALID; ++i )
	{
    	if ( caNamedKeyStore[i].KeyHandle != caKeyInvalidHandle )
    	{
    		++NamedKeyCount;
    	}
	}

	CRYPTODBG("%s: there are %d named keys in key table...\n", __FUNCTION__, NamedKeyCount );

	// count total number of entries in key table (for this ClientId)
	for ( i = caKeyStartIndex; i <= caKeyEndIndex; i++ )
	{
		if (caKeyStore[i].InUse == 1)
		{
			if ( caClientIdMatchesKeyHandle( ClientId, i ) == nfBT_TRUE )
			{
				CRYPTODBG("%s: incrementing TotalKeyCount for index %d\n", __FUNCTION__, i);
				++TotalKeyCount;
			}

		}/* end if (caKeyStore[i].InUse == 1) */

	}/* end for ( i = caKeyStartIndex; i < caKeyEndIndex; i++ ) */

	CRYPTODBG("%s: there are %d keys (total) in the key table.\n", __FUNCTION__, TotalKeyCount );

	// if not enough space in DeletedHandlesList, return error
	if ( ((TotalKeyCount - NamedKeyCount) * sizeof(uint32_t)) > MaxHandleListSize )
	{
		// not enough room in DeletedHandlesList
		CRYPTODBG("%s: not enough room in OutMsg for %d handles (%d bytes)\n", __FUNCTION__,
				TotalKeyCount - NamedKeyCount, MaxHandleListSize );
		return nfTCR_OUT_OF_MEMORY;
	}

	CRYPTODBG("%s: I should be deleting a maximum of %d keys from key table.\n",
			__FUNCTION__, TotalKeyCount - NamedKeyCount );

	/*
	 * okay, there's enough room -- go through list, deleting non-named keys
	 * owned by this ClientId.
	 */
	for ( i = caKeyStartIndex; i <= caKeyEndIndex; i++ )
	{
		nfBoolean_t isNamed;
		nfCryptoKeyHandle_t Handle;


		if (caKeyStore[i].InUse == 1)
		{
			CRYPTODBG("%s: looking at key[%d]\n", __FUNCTION__, i);

			// does this client own this key?
			if ( caClientIdMatchesKeyHandle( ClientId, i ) == nfBT_TRUE )
			{
				Handle = caGetHandleFromIndex(i);

				// need to prime this each time
				isNamed = nfBT_FALSE;

				CRYPTODBG("%s: Handle is %d, checking to see if it is a named key.\n",
						   __FUNCTION__, Handle);

				// is this a named key?
				for (j = 0; j < caNK_INDEX_INVALID; ++j )
				{
			    	if ( caNamedKeyStore[j].KeyHandle == Handle )
			    	{
			    		CRYPTODBG("%s: Handle %d is for named key.\n", __FUNCTION__, Handle );
			    		isNamed = nfBT_TRUE;
			    		break;
			    	}
				}/* end for ( j = 0; j < caNK_INDEX_INVALID; ++j ) */

				// if not a named key, delete it, add to deleted handles list, increment count
				if ( isNamed == nfBT_FALSE )
				{
					int rc;

					CRYPTODBG("%s: deallocating handle %d\n", __FUNCTION__, Handle );
					if ( ( rc = caDeallocateKeyHandle(Handle) ) != 0 )
					{
						// internal error ?!
						CRYPTODBG("%s: failed to deallocate handle %d\n", __FUNCTION__, Handle);
						return nfTCR_INTERNAL_ERROR;
					}

					// put handle into DeletedHandlesList, increment count
					DeletedHandlesList[NumHandles++] = Handle;
				}

			}/* end if ( caClientIdMatchesKeyHandle( ClientId, i ) ) */


		}/* end if (caKeyStore[i].InUse == 1) */

	}/* end for ( i = caKeyStartIndex; i < caKeyEndIndex; i++ ) */

	CRYPTODBG("%s: deleted %d keys...\n", __FUNCTION__, NumHandles );

	*NumHandlesPtr = NumHandles;

	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caClearKeys */

/*********************************************************************
 * @function caDeleteKey
 *
 * @brief delete key from key table
 *
 * @param[in] ClientId - the TEE client ID
 * @param[in] KeyHandle - handle of key to delete
 *
 * @returns nfTGR_SUCCESS if successful,
 *********************************************************************/
nfTeeCryptoResult_t caDeleteKey( nfTeeClientId_t 		ClientId,
							  	 nfCryptoKeyHandle_t	KeyHandle )
{
	int		KeyIndex;
	int		j;

	/*
	 * We need to do the following:
	 * 	- get the index that matches this key handle
	 * 	- if this key is in use and clientid matches
	 * 		- if named key
	 * 			- invalidate namedkey.keyhandle
	 * 		- deallocate key
	 */

	/*
	 *  This function returns success even if the key is not deleted.
	 *  Currently, we use one handle for both keys,
	 *  so when the refapp tries to delete them, the first one succeeds,
	 *  and the second one fails.
	 */
	if ( ( KeyIndex = caGetIndexFromHandle(KeyHandle) ) == caKeyInvalidIndex )
	{
		CRYPTODBG("%s: handle %d is invalid.\n", __FUNCTION__, KeyHandle );
		return nfTCR_INVALID_KEY_HANDLE;
	}

	if ( ( caKeyStore[KeyIndex].InUse == 1 )
	  && ( caClientIdMatchesKeyHandle( ClientId, KeyIndex ) == nfBT_TRUE ) )
		{
			CRYPTODBG("%s: Handle is %d, checking to see if it is a named key.\n",
					   __FUNCTION__, KeyHandle);

			// is this a named key? If so, clear named key handle
			for ( j = 0; j < caNK_INDEX_INVALID; ++j )
			{
				if ( caNamedKeyStore[j].KeyHandle == KeyHandle )
				{
					CRYPTODBG("%s: Handle %d is for named key (clearing).\n",
							__FUNCTION__, KeyHandle );
					caNamedKeyStore[j].KeyHandle = caKeyInvalidHandle;
				}
			}/* end for ( j = 0; j < caNK_INDEX_INVALID; ++j ) */

			// now, deallocate form key table
			CRYPTODBG("%s: deallocating handle %d\n", __FUNCTION__, KeyHandle );
			if ( caDeallocateKeyHandle(KeyHandle) != 0 )
			{
				// internal error ?!
				CRYPTODBG("%s: failed to deallocate handle %d\n", __FUNCTION__, KeyHandle);
				return nfTCR_INTERNAL_ERROR;
			}

		}/* end if (caKeyStore[i].InUse == 1) && ... */



	return (nfTeeCryptoResult_t) nfTGR_SUCCESS;

}/* end caDeleteKey */

// vim: expandtab tabstop=4 shiftwidth=4
