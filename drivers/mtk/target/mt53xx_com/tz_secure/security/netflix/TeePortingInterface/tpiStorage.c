/*
 * tpiStorage.c
 *
 *  Created on: Jan 31, 2014
 *      Author: scott
 */
/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 * This is our reference implementation of the storage TPI code for the NRD
 * ref app. If you are porting the SDK to another platform (e.g. a game console), then
 * you'll need to replace this file with your own storage TPI implementation. See
 * TEE/include/StorageAgentTPI.h for the interface definitions. At a high level, this
 * file needs to provide functions to populate saManufSSTable[]. We currently assume
 * the following:
 *
 * - an encrypted and integrity-protected "manufacturing secure store" was injected
 *   during manufacturing
 *
 * - we have the keys to authenticate/decrypt it (#defined in tpiStorage.h, but in a real
 *   implementation, you would either store these keys in OTP, derive them from something
 *   in OTP, or store them in NAND, encrypted with a ROM/OTP key)
 *
 * - that we have the ability to create client-unique encryption/authentication
 *   keys based on the clientID of the requester (used for protect/unprotect operations)
 */

#include "DebugTypes.h"
#include "CommTypes.h"
#include "StorageTypes.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include "StorageAgent.h"
#include "StorageAgentTPI.h"
#include "b64.h"
#define nfSS_NEED_KEYS
#include "tpiStorage.h"
#undef nfSS_NEED_KEYS

#define nfSS_MAX_FILE_SIZE	8092

// internal buffer for holding manuf secure store in memory
//static uint8_t	_saMemBuf[nfSS_MAX_FILE_SIZE];



/*
 * These will refer to the in-memory manufacturing secure store data
 */
static uint8_t *saManufSSDataPtr = NULL;
static uint32_t saManufSSDataLength = 0;

static uint8_t saLocalKPWStorage[16];

/*
 * This is the key ladder base for application services offered by storageAgent. We
 * use this, together with our secure store keys, to derive encryption and HMAC keys
 * for each application. Note that we hold this key in memory, but it could also
 * be loaded directly into a crypto processor.
 *
 * In a "real" implementation, this key would be directly loaded from OTP, or
 * derived from a device-unique key that is loaded from OTP. It is very important
 * that this key be device-unique. Otherwise, it would be possible to migrate an
 * application secure store from one device to another.
 *
 * For our reference implementation, we are hard-coding this, but you would never
 * do this in a real implementation.
 */
uint8_t saKeyLadderBase[] = {0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF0,0x0F,
		                     0xAF,0xBF,0xCF,0xDF,0xEF,0xFF,0x00,0x01};

/*
 * Our implementation of storageAgent has its own device unique keys. In a "real"
 * implementation, we would probably re-name these variables to something less obvious, and
 * consider obfuscating the fact that these are crypto keys. Reverse engineers load binaries
 * into tools like IDA Pro, and then look for informative variable names like these. So at
 * minimum, you should strip your binaries.
 */
uint8_t saInternalKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalAesKey[saAES_128_KEY_SIZE_BYTES];
uint8_t saInternalHMACKey[saHMAC_SHA256_KEY_SIZE_BYTES];

/*
 *  We use the client ID to limit access to data items. Since the TEE comm agent
 *  controls the client ID, we can trust it to accurately report the client ID to
 *  us. Even if the REE is compromised, it cannot spoof a client ID.
 *
 *  We have one special client id (all 0x00); for an internal "all-access" pass, use
 *  this client id. This may be removed in a future release, but is currently here for
 *  utility.
 */
static nfTeeClientId_t saMgmtClientId = nfSS_CLIENT_ID_TEE_MGMT;
static nfTeeClientId_t cryptoAgentClientId = nfSS_CLIENT_ID_TEE_NTBA_RO;

/*
 * We can't assume that we have dynamic memory allocation capability (e.g. malloc),
 * so we pre-allocated a work buffer for crypto operations. If your implementation
 * supports malloc, you could use dynamic allocation instead.
 */
static uint8_t WorkBuf[nfTEE_MAX_INTERNAL_SIZE];

/*
 * The following definition relates to how we protect/unprotect data. "Protect" is
 * our name for the act of providing client "secure store" services. For example, the
 * NRD application needs to protect data at rest (e.g. MSL tokens, the UI trust store, etc.)
 *
 * We use a proprietary "wrapper" for this data. Version is prepended to protected data so
 * that we can apply proper algorithms, formatting, etc. when unprotecting. If we don't recognize
 * (or implement) the version, we won't attempt to decrypt the data.
 */
static uint32_t	saStorageVersion = 0x10000001;

/*********************************************************************
 * @function GetClientSection
 *
 * @brief Load client section into runtime secure store
 *
 * @param[in] SectionID - ID of this section (we'll map to ClientID)
 * @param[in] BufPtr - pointer to to buffer, *BufPtr is pointing to padding length
 * @param[in] BufLength - current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int GetClientSection( nfSectionID_t   SectionID,
                             uint8_t            *BufPtr,
                             uint16_t           BufLength )
{
    nfSecureStoreClientKey_t	Key;
    uint16_t					Length;

    /*
     * This section looks like this:
     *
     *       <key><length><data>
     *       <key><length><data>
     *              :
     *       <key><length><data>
     *     ---------------------------------------------------------------------
     */

    STORAGEDBG("%s: called with SectionID %08x, BufLength %d\n", __FUNCTION__, SectionID, BufLength );
//    teePrintHexDump( BufPtr, BufLength );

    do
    {
        int DataSize;

        /*
         * We have a key-length-data record, with variable-length data. We need
         * read the length, and allocate a variable-sized section data structure
         * to hold this record
         */

        // each element holds at minimum 1 byte of data, so sanity-check
        if ( BufLength < sizeof(nfSecureStoreClientKey_t) + sizeof(uint16_t) + 1 )
        {
            STORAGEDBG("%s: not enough data (%d bytes) to construct section list (?!)\n",
            __FUNCTION__, BufLength );
            return -1;
        }

        // get DataSize from record
        DataSize = (*(BufPtr + 4) << 8) + *(BufPtr + 5);

  //     STORAGEDBG("%s: DataSize: %d; BufLength: %d\n", __FUNCTION__, DataSize, BufLength );
  //      teePrintHexDump( BufPtr, DataSize ? DataSize : 16);
			//assert(DataSize > 0);

        // sanity check
        if ( ( DataSize > (int) (BufLength - sizeof(nfSecureStoreClientKey_t)) )
          || ( DataSize < 0 ) )
        {
            // PUKE. We don't have this much data in this buffer....
            STORAGEDBG("%s: inconsistent datasize (%d/%d)\n",
                        __FUNCTION__, DataSize, BufLength );
            return -1;
        }

        // get the SectionID
        nfGETWORD32(BufPtr, Key);
        BufPtr += sizeof(nfSecureStoreClientKey_t);

        STORAGEDBG("%s: ClientKey: %08x; BufPtr[0-1]: %x %x\n", __FUNCTION__, Key, *BufPtr, *(BufPtr+1));

        // get the length
        nfGETWORD16(BufPtr, Length);
        BufPtr += sizeof(uint16_t);

	   /*
		* Remember, each table row looks like this:
		*
		* 	typedef struct
		* 	{
		*		nfSecureStoreClientKey_t	ClientKey;
		*		nfTeeClientId_t				*ClientIdPtr;
		*		uint32_t					DataLength;
		*		uint8_t						*DataPtr;
		*	} nfSecureStoreTableRow_t;
		*
		* Here, we are updating DataLength and DataPtr for individual elements.
		* Look at the key to see which element to update.
		*/

        switch ( Key )
        {
        	// save length and pointer to data
        	case ntbaAGENT_KEY_ESN:
 //       		assert(SectionID == teeSID_NTBA_RO );
        		saManufSSTable[nfSSRI_ESN].DataLength = (uint32_t) Length;
        		saManufSSTable[nfSSRI_ESN].DataPtr = BufPtr;
        		STORAGEDBG("%s: got pointer to %d bytes of ESN at %p\n", __FUNCTION__, (int) Length, BufPtr);
        		break;
        	case ntbaAGENT_KEY_KPE:
//        		assert(SectionID == teeSID_NTBA_RO );
        		saManufSSTable[nfSSRI_KPE].DataLength = (uint32_t) Length;
        		saManufSSTable[nfSSRI_KPE].DataPtr = BufPtr;
        		STORAGEDBG("%s: got pointer to %d bytes of KPE at %p\n", __FUNCTION__, (int) Length, BufPtr);
        		break;
        	case ntbaAGENT_KEY_KPH:
//        	    assert(SectionID == teeSID_NTBA_RO );
			    saManufSSTable[nfSSRI_KPH].DataLength = (uint32_t) Length;
			    saManufSSTable[nfSSRI_KPH].DataPtr = BufPtr;
			    STORAGEDBG("%s: got pointer to %d bytes of KPH at %p\n", __FUNCTION__, (int) Length, BufPtr);
			    break;
        	case drmAGENT_KEY_MODEL_PRIV:
//        	    assert(SectionID == teeSID_DRM_RO);
			    saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataLength = (uint32_t) Length;
			    saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataPtr = BufPtr;
			    STORAGEDBG("%s: got pointer to %d bytes of GROUP_PRIV at %p\n", __FUNCTION__, (int) Length, BufPtr);
			    break;
        	case drmAGENT_KEY_MODEL_CERT:
 //       	    assert(SectionID == teeSID_DRM_RO);
			    saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataLength = (uint32_t) Length;
			    saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataPtr = BufPtr;
			    STORAGEDBG("%s: got pointer to %d bytes of GROUP_CERT at %p\n", __FUNCTION__, (int) Length, BufPtr);
			    break;
        	case drmAGENT_KEY_DEVCERT_TEMPLATE:
//        	    assert(SectionID == teeSID_DRM_RO);
			    saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataLength = (uint32_t) Length;
			    saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataPtr = BufPtr;
			    STORAGEDBG("%s: got pointer to %d bytes of CERT_TEMP at %p\n", __FUNCTION__, (int) Length, BufPtr);
			    break;
        	default:
        		break;
        }/* end switch( Key ) */


        // adjust SectionLength
        BufLength -= (sizeof(nfSectionID_t) + sizeof(uint16_t) + Length);

        // adjust BufPtr
        BufPtr += Length;

    } while ( BufLength > 0 );

    // we're done...
    return 0;

}/* end GetClientSection */

/*********************************************************************
 * @function GetMgmtSection
 *
 * @brief Load mgmt section into runtime secure store
 *
 * @param[in] BufPtr - pointer to to buffer, *BufPtr is pointing to padding length
 * @param[in] BufLength - current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int GetMgmtSection( uint8_t *BufPtr, uint16_t BufLength )
{
    nfSectionID_t SectionID;

    /*
     * This record looks like this:
     *
     * TEE MGMT Section ID                 4 bytes
     * section length                      2 bytes
     * NTBA RO section ID                  4 bytes
     * NTBA RO client ID                  16 bytes
     * NTBA RW section ID                  4 bytes
     * NTBA RW client ID                  16 bytes
     * DRM RO section ID                  4 bytes
     * DRM RO client ID                  16 bytes
     * DRM RW section ID                  4 bytes
     * DRM RW client ID                  16 bytes
     * NRD section ID                     4 bytes
     * NRD client ID                     16 bytes
     *
     * When this function is called, the TEE MGMT section ID and section
     * length have already been extracted, and *BufPtr is pointing at
     * first section ID
     *
     * We want to save pointers to the client IDs in saManufSSTable[]
     */

    STORAGEDBG("%s: called with %d-byte buffer\n", __FUNCTION__, BufLength );
 //   teePrintHexDump( BufPtr, BufLength);

    /*
     * We've already stripped the section ID, section length, padding length,
     * padding, and MAC, so what we have are a series of section/client
     * ID pairs. We will take these and construct the list of section
     * headers. See secstoreinternals.h for the secStoreSectionType
     * definition -- that is the struct we use to create the list.
     *
     */
    do
    {
        // get the SectionID, advance pointer
       nfGETWORD32(BufPtr, SectionID);
       BufPtr += sizeof(nfSectionID_t);

       STORAGEDBG("%s: SectionID is %08x, BufPtr is %p\n", __FUNCTION__, SectionID, BufPtr );

       // we're looking for specific sections
       switch ( SectionID )
       {
       	   case teeSID_NTBA_RO:
       		   /*
       		    * ESN, KPE, KPH, KPW belong to this client ID; set pointers in table, each table
       		    * row looks like this:
       		    *
       		    * 	typedef struct
       		    * 	{
				*		nfSecureStoreClientKey_t	ClientKey;
				*		nfTeeClientId_t				*ClientIdPtr;
				*		uint32_t					DataLength;
				*		uint8_t						*DataPtr;
				*	} nfSecureStoreTableRow_t;
       		    *
       		    */
       		   saManufSSTable[nfSSRI_ESN].ClientKey = ntbaAGENT_KEY_ESN;
       		   saManufSSTable[nfSSRI_ESN].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   saManufSSTable[nfSSRI_KPE].ClientKey = ntbaAGENT_KEY_KPE;
       		   saManufSSTable[nfSSRI_KPE].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   saManufSSTable[nfSSRI_KPH].ClientKey = ntbaAGENT_KEY_KPH;
       		   saManufSSTable[nfSSRI_KPH].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   STORAGEDBG("%s: got NTBA section, keys/clientid's init'd\n", __FUNCTION__);
               break;
       	   case teeSID_DRM_RO:
       		   /*
       		    * DRM device cert, model cert, and model priv key belong to this client ID
       		    */
       		   saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].ClientKey = drmAGENT_KEY_DEVCERT_TEMPLATE;
       		   saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   saManufSSTable[nfSSRI_DRM_GROUP_CERT].ClientKey = drmAGENT_KEY_MODEL_CERT;
       		   saManufSSTable[nfSSRI_DRM_GROUP_CERT].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   saManufSSTable[nfSSRI_DRM_GROUP_PRIV].ClientKey = drmAGENT_KEY_MODEL_PRIV;
       		   saManufSSTable[nfSSRI_DRM_GROUP_PRIV].ClientIdPtr = (nfTeeClientId_t *)BufPtr;
       		   STORAGEDBG("%s: got DRM section, keys/clientid's init'd\n", __FUNCTION__);
       		   break;
       	   case teeSID_NRD_APP1:
       		   /*
       		    * This one is a special case; no data, we just want the NRD client ID
       		    */
       		   saManufSSTable[nfSSRI_NRD_APP_ID].ClientKey = teeAPPID_KEY_NRD;
       		   saManufSSTable[nfSSRI_NRD_APP_ID].ClientIdPtr = (nfTeeClientId_t *)saMgmtClientId;
       		   saManufSSTable[nfSSRI_NRD_APP_ID].DataPtr = (uint8_t *)BufPtr;
       		   saManufSSTable[nfSSRI_NRD_APP_ID].DataLength = sizeof(nfTeeClientId_t);
       		   STORAGEDBG("%s: got NRD section, key/clientid init'd\n", __FUNCTION__);
       		   STORAGEDBG("\t%02x %02x %02x %02x %02x %02x %02x %02x ", *BufPtr, *(BufPtr + 1),
       				*(BufPtr + 2), *(BufPtr + 3),*(BufPtr + 4),*(BufPtr + 5),
       				*(BufPtr + 6),*(BufPtr + 7 ));
       		   break;
       	   default:
       		   STORAGEDBG("%s: skipping unused section id (%08x)\n", __FUNCTION__, SectionID );
       		   break;
       }/* end switch ( SectionID ) */

        // adjust SectionLength
       BufLength -= (sizeof(nfSectionID_t) + sizeof(nfTeeClientId_t));
       STORAGEDBG("%s: deducted %d from BufLength, remaining length is %d\n", __FUNCTION__,
    		   sizeof(nfSectionID_t) + sizeof(nfTeeClientId_t), BufLength );

       // adjust BufPtr
       BufPtr += sizeof(nfTeeClientId_t);

    } while ( BufLength > 0 );

    STORAGEDBG("%s: finished decoding mgmt section...\n", __FUNCTION__ );
    STORAGEDBG("%s: ------------------------------------\n", __FUNCTION__ );

    // we're done...
    return 0;

}/* end GetMgmtSection */

/*********************************************************************
 * @function GetNextManufSection
 *
 * @brief TEE secure store initialization
 *
 * @param[in] BufPtrPtr - pointer to pointer to buffer, *BufPtrPtr is pointing to next section
 * @param[in] BufLengthPtr - pointer to current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
 static int GetNextManufSection( uint8_t **BufPtrPtr, uint16_t *BufLengthPtr )
 {
    uint32_t                UniqueID = 0;
    int                     LengthRemaining;
    uint16_t                SectionLength = (uint16_t) 0;
    uint8_t                 *BytePtr;
    int                     rc;

    // no mercy
    //assert(BufPtrPtr);
    //assert(*BufPtrPtr);
    //assert(BufLengthPtr);
    //assert(*BufLengthPtr > 0);

    /*
     * (already decrypted) manufacturing secure store sections look like this:
     *
     * <32-bit unique ID><16-bit section length><data>
     *
     * The unique ID can be used to find the client ID, which can
     * in turn be used to decrypt the data. Each section results in
     * a linked list of section data entries, and this list is in
     * turn linked into the list of sections in the SecStoreContext
     * structure. See secstoreInternals.h for pretty pictures.
     */

    // first, get the unique id

    BytePtr = *BufPtrPtr;
    LengthRemaining = *BufLengthPtr;
    if ( ( LengthRemaining < 0)
      || ( (unsigned) LengthRemaining < sizeof(nfSectionID_t) + sizeof(uint16_t) ) )
    {
        STORAGEDBG("%s: premature EOF (section ID)\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

    UniqueID = (*BytePtr << 24) + (*(BytePtr + 1) << 16) + (*(BytePtr + 2) << 8) + *(BytePtr + 3);
    BytePtr += sizeof(UniqueID);
    LengthRemaining -= sizeof(UniqueID);

    STORAGEDBG("%s: UniqueID: %08x\n", __FUNCTION__, UniqueID );

    // now, get section length
    SectionLength = (*BytePtr << 8) + *(BytePtr + 1);
    BytePtr += sizeof(SectionLength);
    LengthRemaining -= sizeof(SectionLength);

    STORAGEDBG("%s: SectionLength is %d, LengthRemaining is %d\n", __FUNCTION__,
                SectionLength, LengthRemaining );

    // sanity check
    if ( LengthRemaining < SectionLength)
    {
        STORAGEDBG("%s: insufficient DataLength (%d/%d)\n", __FUNCTION__, LengthRemaining, SectionLength );
        goto FILE_ERROR;
    }

    /*
     * Okay: now we have a section ID + length. We need to find the client
     * ID (based on section ID), and then derive the client crypto key from
     * that.
     */

    if ( UniqueID == teeSID_TEE_MGMT )
    {

        /*
         * This one is a special case: it contains all sectionID to clientID
         * mappings. From this, we'll build our internal map, and this will be
         * used to look up the client ID for other sections. This is encrypted
         * with the first key in the internal TEE key ladder.
         */
        if ( ( rc = GetMgmtSection(BytePtr, SectionLength) ) != 0 )
        {
            STORAGEDBG("%s: GetMgmtSection failed, bailing.\n", __FUNCTION__ );
            return -1;
        }

        LengthRemaining -= SectionLength;
        BytePtr += SectionLength;

    }
    else
    {
        // this is a non-mgmt section
        STORAGEDBG("%s: calling GetClientSection for ID %08x\n", __FUNCTION__, UniqueID );
        if ( ( rc = GetClientSection(UniqueID, BytePtr, SectionLength) ) != 0 )
        {
            STORAGEDBG("%s: getCkuebtSection failed, bailing.\n", __FUNCTION__ );
            return -1;
        }

        LengthRemaining -= SectionLength;
        BytePtr += SectionLength;
    }


    /*
     * If we get here, we succeeded in loading a section; adjust
     * TotalLength and return.
     */
    *BufLengthPtr -= (BytePtr - *BufPtrPtr);
    *BufPtrPtr = BytePtr;

    return 0;

/* NOTREACHED */
FILE_ERROR:
    return -1;

}/* end GetNextManufSection */

 /*********************************************************************
 * @function saDecryptManufStore
 *
 * @brief Decrypt/validate manufacturing secure store
 *
 * @param[in] BufPtrPtr - pointer to pointer to buffer
 * @param[in] BufLengthPtr - pointer to current buffer length
 *
 * @returns 0 upon success, -1 otherwise
 *
 *********************************************************************/
static int saDecryptManufStore( uint8_t *BufPtr, uint16_t BufLength )
{
	int  rc;

	 // no mercy
	 //assert(BufPtr);
	 //assert(BufLength > 0);

	STORAGEDBG("%s: calling decrypt...\n", __FUNCTION__ );

	 if ( ( rc = OPENSSL_LIBTOM_EXCHANGE(teeAesCbc128DecryptZeroIV)( BufPtr,
							 BufLength, _SecStoreAesKey ) ) != 0 )
	 {
		 // encryption failed, need to bail...
		 /*fprintf(stderr, "%s: EncryptWithClientKey returned %d, exiting.\n",
			 __FUNCTION__, rc );*/
		 return -1;
	 }

	/*
	* Okay, so we decrypted in place. Now, we need to check the hash. The
	* buffer looks like this:
	*
	* <padlen><data><pad><hash>
	*
	* The hash is 20 bytes, and there may or may not be padding. First thing
	* we want to do is verify the hash
	*/
	if ( BufLength < saSHA1_HASH_SIZE )
	 {
		 STORAGEDBG("%s: BufLength too small (%d).\n", __FUNCTION__, BufLength );
		 return -1;
	 }

	 if ( OPENSSL_LIBTOM_EXCHANGE(teeValidateSha1)( BufPtr, (unsigned)(BufLength - saSHA1_HASH_SIZE),
				 BufPtr + (BufLength - saSHA1_HASH_SIZE) ) != 0 )
	 {
		 STORAGEDBG("%s: SHA1 validation failed on manuf-secstore.\n", __FUNCTION__ );
		 return -1;
	 }

	// if we get here, everything is dandy
	return 0;

 }/* end saDecryptManufStore */

/*************d********************************************************
 * @function saValidateManufSecStore
 *
 * @brief validate manufacturing secure store
 *
 * @param[in] BufPtr - pointer to in-memory copy of manuf secure store
 * @param[in] BufSize - size of manuf secure store
 *
 * @return 0 if secure store is valid, -1 otherwise
 *
 *********************************************************************/
 #if 0
static int saValidateManufSecStore( uint8_t *BufPtr, uint32_t BufSize )
{
	uint8_t 	*BytePtr;
    uint8_t     HmacData[nfSS_HEADER_HMAC_SIZE];
    int			rc;

    // get file header into context structure
    if ( BufSize < nfSS_FILE_HEADER_SIZE )
    {
        STORAGEDBG("%s: premature EOF (file header)\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

#if 0
    printf("%s: here's the file header I read: \n", __FUNCTION__ );
    teePrintHexDump(BufPtr, nfSS_FILE_HEADER_SIZE );
#endif

    // compute HMAC on header data
    rc = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( BufPtr,
                        nfSS_FILE_HEADER_SIZE,
                        _SecStoreHmacKey,
                        sizeof(_SecStoreHmacKey),
                        HmacData);
    if ( rc <= 0 )
    {
        STORAGEDBG("%s: Hmac error?!\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

#if 0
    printf("%s: Here's the computed HMAC: \n", __FUNCTION__ );
    teePrintHexDump(HmacData, nfSS_HEADER_HMAC_SIZE);
#endif

    // verify computed HMAC against value in file
    BytePtr = BufPtr + nfSS_FILE_HEADER_SIZE;
    if ( teeMemcmp( HmacData, BytePtr, nfSS_HEADER_HMAC_SIZE ) != 0 )
    {
        STORAGEDBG("%s: Hmac mismatch, exiting\n", __FUNCTION__ );
        goto FILE_ERROR;
    }

    /*
     * okay, header looks okay, check magic/version
     */
    // magic high word
    if ( ( BufPtr[0] != ((nfSS_FILE_MAGIC1 >> 24) & 0xFF))
      || ( BufPtr[1] != ((nfSS_FILE_MAGIC1 >> 16) & 0xFF))
      || ( BufPtr[2] != ((nfSS_FILE_MAGIC1 >> 8) & 0xFF))
      || ( BufPtr[3] !=  (nfSS_FILE_MAGIC1 & 0xFF) ) )
    {
        STORAGEDBG("%s: invalid magic1 (%08x)\n", __FUNCTION__,
                        *((uint32_t *)BufPtr)  );
        goto FILE_ERROR;
    }
    // magic low word
    if ( ( BufPtr[4] != ((nfSS_FILE_MAGIC2 >> 24) & 0xFF))
      || ( BufPtr[5] != ((nfSS_FILE_MAGIC2 >> 16) & 0xFF))
      || ( BufPtr[6] != ((nfSS_FILE_MAGIC2 >> 8) & 0xFF))
      || ( BufPtr[7] !=  (nfSS_FILE_MAGIC2 & 0xFF) ) )
    {
        STORAGEDBG("%s: invalid magic2 (%08x)\n", __FUNCTION__,
                        *((uint32_t *)(BufPtr + 4))  );
        goto FILE_ERROR;
    }

    STORAGEDBG("%s: file magic is fine.\n", __FUNCTION__ );

    // verify file version, we only recognize one for now
    if ( ( BufPtr[8] != ((nfSS_FILE_VERSION >> 8) & 0xFF))
      || ( BufPtr[9] != ((nfSS_FILE_VERSION & 0xFF))) )
    {
        STORAGEDBG("%s: invalid version (%04x)\n", __FUNCTION__,
          *((uint16_t *)(BufPtr + nfSS_FILE_MAGIC_SIZE)));
        goto FILE_ERROR;
    }

    // okay, file header is good...
    return 0;

// NOTREACHED
FILE_ERROR:
    return -1;

}/* end saValidateManufSecStore */
#endif
/*********************************************************************
 * @function saDeriveKpw
 *
 * @brief RFC5869 Kpw derivation function using SHA256
 *
 * @param[in] Kpe - preshared netflix encryption key
 * @param[in] Kph - preshared netflix hmac key
 * @param[out] Kpw - buffer for Kpw
 * @param[in] KpwMaxLength - max length of Kpw
 *
 *
 * @returns length of stored Kpw on success, -1 on failure
 *
 *********************************************************************/
static int saDeriveKpw( uint8_t Kpe[], uint8_t Kph[], uint8_t Kpw[], uint32_t KpwMaxLength )
{
	int		rc;

#define saKPW_LEN 16
	uint8_t Salt[saKPW_LEN] = {0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53,
							   0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69};
	uint8_t Info[saKPW_LEN] = {0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d,
							   0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91 };
#define saSHA256_LEN 32
	uint8_t FirstResult[saSHA256_LEN]; // HMAC-SHA256(salt, Kde||Kdh)
	uint8_t SecondResult[saSHA256_LEN]; // HMAC-SHA256(FirstResult, info))

#define saKPE_LEN 16
#define saKPH_LEN 32
#define saCONCAT_KEY_LEN (saKPE_LEN + saKPH_LEN) // concatentate Kpe|Kph
	uint8_t	ConcatKeys[saCONCAT_KEY_LEN];

	/* Kdw = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, Kde||Kdh), info)) */

	// no nonsense.
	//assert( KpwMaxLength >= saKPW_LEN );
	//assert(Kpe != NULL);
	//assert(Kph != NULL);
	//assert(Kpw != NULL);

	// concatenate Kpe|Kph
	(void) teeMemcpy(ConcatKeys, Kpe, saKPE_LEN);
	(void) teeMemcpy(ConcatKeys + saKPE_LEN, Kph, saKPH_LEN);
#if 0
	// here's our HMAC prototype. NB: data first, key second.
	int teeHmacSha256( uint8_t     *DataPtr,
	                   uint32_t    DataLength,
	                   uint8_t     *KeyPtr,
	                   uint32_t    KeyLength,
	                   uint8_t     *ResultPtr )
#endif
	// compute FirstResult: key is Salt[], data is concatenated keys
	rc = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( ConcatKeys, saCONCAT_KEY_LEN, Salt, saKPW_LEN, FirstResult );
	if ( rc != saSHA256_LEN )
	{
		STORAGEDBG("%s: teeHmacSha256(FirstResult) failed, returned %d\n", __FUNCTION__, rc );
		return -1;
	}

#if TEE_LIBTOM_TEST_ENABLE
		int		rc__;
		uint8_t FirstResult__[saSHA256_LEN];
		rc__ = teeHmacSha256_libtom(ConcatKeys, saCONCAT_KEY_LEN, Salt, saKPW_LEN, FirstResult__ );
		printf("[sahala compare_buffer] FirstResult__\n");
		int Ret = compare_buffer(FirstResult,FirstResult__,rc,rc__);
		
		assert(Ret == 0);	
		
#endif

	// compute SecondResult: key is FirstResult, data is Info[]
	rc = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( Info, saKPW_LEN, FirstResult, saSHA256_LEN, SecondResult );
	if ( rc != saSHA256_LEN )
	{
		STORAGEDBG("%s: teeHmacSha256(SecondResult) failed, returned %d\n", __FUNCTION__, rc );
		return -1;
	}

	#if TEE_LIBTOM_TEST_ENABLE
		uint8_t SecondResult__[saSHA256_LEN];
		rc__ = teeHmacSha256_libtom(Info, saKPW_LEN, FirstResult, saSHA256_LEN, SecondResult__ );
		printf("[sahala compare_buffer] SecondResult__\n");
		Ret = compare_buffer(SecondResult,SecondResult__,rc,rc__);
		
		assert(Ret == 0);	
		
	#endif

	// copy first 128 bits into Kpw if there is room
	if ( KpwMaxLength <= saKPW_LEN )
		(void) teeMemcpy(Kpw, SecondResult, saKPW_LEN );

	// also, save copy
	(void) teeMemcpy(saLocalKPWStorage, SecondResult, saKPW_LEN );

	STORAGEDBG("%s: dumping derived Kpw:\n", __FUNCTION__ );
	//teePrintHexDump( saLocalKPWStorage, saKPW_LEN );

	return saKPW_LEN;

}/* end saDeriveKpw */

/***************************************************************************
 *
 * @function   teeGetSecStoreData
 *
 * @brief   Load secure store data into memory
 *
 * @param[in] FilenamePtr - pointer to filename for secure store
 * @param[in] BufPtrPtr - pointer to pointer to buffer
 * @param[in] BufLengthPtr - pointer to where you want length stored
 *
 * @returns  0 if everything okay, -1 otherwise
 *
 **************************************************************************/
 #if 0
int teeGetSecStoreData( uint8_t *FilenamePtr, uint8_t **BufPtrPtr, uint32_t *BufSizePtr )
{
    FILE        *fp;
    int         Size;
    struct stat StatBuf;
    uint8_t     *BufPtr;
    char		*PathPtr;

#define nfMANUF_SS_PATH "./manuf_ss.bin"
#define nfMANUF_SS_PATH_BACKUP "/tmp/manuf_ss.bin"

    if ((PathPtr = (char *) FilenamePtr) == NULL )
    	PathPtr = nfMANUF_SS_PATH;

    // get file size; try cwd, and if that fails, try backup path (/tmp)
    if ( stat( PathPtr, &StatBuf ) != 0 )
    {
    	PathPtr = nfMANUF_SS_PATH_BACKUP;
        if ( stat( PathPtr, &StatBuf ) != 0 )
        {
			STORAGEDBG("%s: failed to stat >%s<\n", __FUNCTION__, PathPtr );
			return -1;
        }
    }

    // allocate buffer large enough to hold file
    STORAGEDBG("%s: allocating %d-byte buffer to hold file...\n", __FUNCTION__, (int) StatBuf.st_size );
    if ( StatBuf.st_size  > nfSS_MAX_FILE_SIZE )
    {
        STORAGEDBG("%s: SS is too large (%d/%d), bailing.\n", __FUNCTION__,
                   (int)StatBuf.st_size, nfSS_MAX_FILE_SIZE );
        return -1;
    }

    // open file
    if ( ( fp = fopen( PathPtr, "r") ) == NULL )
    {
        STORAGEDBG("%s: failed to open >%s<\n", __FUNCTION__, PathPtr );
        return -1;
    }

    /*
     * Use BufPtr to point to local static buffer. If system supports dynamic
     * mem allocation, can alloc instead
     */
    BufPtr = _saMemBuf;

    // XXX - note we will fail if buffer if not read in one pass
    // read file into buffer
    if ( (Size = fread(BufPtr, sizeof(char), StatBuf.st_size, fp)) != StatBuf.st_size )
    {
        STORAGEDBG("%s: fread returned %d, bailing.\n", __FUNCTION__, Size);
        (void) fclose(fp);
        return -1;
    }

    // close file, update pointers, get outta here...
    (void) fclose(fp);
    *BufPtrPtr = BufPtr;
    *BufSizePtr = Size;

    return 0;

}/* end teeGetSecStoreData */
#endif
/*********************************************************************
 * @function saGetAppKeys
 *
 * @brief gets AES/Hmac keys for a given client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] AesKeyBuf - buffer for AES key
 * @param[out] HmacKeyBuf - buffer for HmacSha256 key
 *
 * NB: Assumes 128-bit AES key and 256-bit HMAC key; make sure your buffers
 *     are big enough...
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
static nfTeeStorageResult_t saGetAppKeys( nfTeeClientId_t 	ClientId,
										  uint8_t 			AesKey[],
										  uint8_t 			HmacKey[] )
{
	uint8_t	TempBuf[saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES];
	int		rc;

	/*
	 * TBD: these keys should be computed once and cached...
	 *
	 * Compute application-specific keys
	 */

	// first, encrypt saKeyLadderBase; this becomes saInternalKey
	(void) teeMemcpy( AesKey, ClientId, saAES_128_KEY_SIZE_BYTES );
	rc = OPENSSL_LIBTOM_EXCHANGE(teeAesCbc128EncryptZeroIV)( AesKey,
			saAES_128_KEY_SIZE_BYTES, saInternalAesKey );
	if ( rc != 0 )
	{
		CRYPTODBG("%s: failed to create app key ladder base!?\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	if ( ( rc = OPENSSL_LIBTOM_EXCHANGE(teeSha384)(AesKey, saAES_128_KEY_SIZE_BYTES, TempBuf) ) != 0 )
	{
		CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	// first 128 bits is AES key, next 256 is HMAC key
	(void)teeMemcpy( AesKey, TempBuf, saAES_128_KEY_SIZE_BYTES );
	(void)teeMemcpy( HmacKey, TempBuf + saAES_128_KEY_SIZE_BYTES, saHMAC_SHA256_KEY_SIZE_BYTES );

	// don't leave key material laying around...
	(void) teeBzero(TempBuf, sizeof(TempBuf));

	return nfTGR_SUCCESS;

}/* end saGetAppKeys */

/*********************************************************************
 * @function saTpiGetTAKeyLadderBase
 *
 * @brief creates device-unique key ladder base for a given (trusted) client ID
 *
 * @param[in] ClientId - ID for client who made request
 * @param[out] Buf - buffer for key ladder
 * @param[in] BufSize - size of buffer
  * @param[out] ActualSizePtr - pointer to where you output size
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiGetTAKeyLadderBase( nfTeeClientId_t 	ClientId,
										  	  uint8_t 			Buf[],
										  	  uint32_t 			BufSize,
										  	  uint32_t			*ActualSizePtr )
{
	int rc;

	/*
	 * NB: we currently only support this for cryptoAgent, so make
	 * sure it's the right clientId
	 */
	if ( teeMemcmp(cryptoAgentClientId, ClientId, sizeof(nfTeeClientId_t)) )
	{
		// they don't match, this is a security violation.
		STORAGEDBG("%s: wrong client ID.\n", __FUNCTION__);
		return nfTSR_NOT_FOUND;
	}

	// okay, generate the key. Remember, encrypts in place...

	if ( BufSize < sizeof(nfTeeClientId_t) )
	{
		STORAGEDBG("%s: passed buffer is too small (%d/%d)", __FUNCTION__,
					BufSize, sizeof(nfTeeClientId_t) );
		return nfTSR_BUFFER_TOO_SHORT;
	}

	teeMemcpy( Buf, ClientId, sizeof(nfTeeClientId_t) );
	rc = OPENSSL_LIBTOM_EXCHANGE(teeAesCbc128EncryptZeroIV)( Buf, sizeof(nfTeeClientId_t), saKeyLadderBase);
	if ( rc != 0 )
	{
		STORAGEDBG("%s: failed to create internal key for cryptoAgent\n", __FUNCTION__ );
		// fatal
		return nfTSR_INTERNAL_ERROR;
	}
	
	#if TEE_LIBTOM_TEST_ENABLE
	teePrintHexDump_tag("[teeAesCbc128EncryptZeroIV] Buf :", Buf, sizeof(nfTeeClientId_t) );
	uint8_t 	temBuf[1024];
	teeBzero(temBuf,1024);
	(void) teeMemcpy( temBuf, ClientId, sizeof(nfTeeClientId_t) );
	int rc__ = teeAesCbc128EncryptZeroIV_libtom(temBuf, sizeof(nfTeeClientId_t), saKeyLadderBase );
	teePrintHexDump_tag("[teeAesCbc128EncryptZeroIV_libtom] temBuf :", temBuf, sizeof(nfTeeClientId_t) );
	printf("[sahala compare_buffer] temBuf\n");
	int Ret = compare_buffer(Buf,temBuf,sizeof(nfTeeClientId_t),sizeof(nfTeeClientId_t));
	if(Ret != 0)
		exit(1);
	
	#endif


	STORAGEDBG("%s: successfully created cryptoAgent ladder key.\n", __FUNCTION__);
	*ActualSizePtr = sizeof(saKeyLadderBase);

	return nfTGR_SUCCESS;


}/* end saTpiGetKeyLadderBase */

/**************************************************
 * @function saTpiLoadNflxIdFile
 *
 * @brief storage agent TPI function to load Netflix idfile
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to idfile data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxIdFile( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
	(void) ClientId;
	(void) DataPtr;
	(void) DataLength;

	STORAGEDBG("%s: this function is unimplemented\n",__FUNCTION__ );
	//assert(0);
	return (nfTeeStorageResult_t) nfTGR_INTERNAL_ERROR;
}/* end saTpiLoadNflxIdFile */

/**************************************************
 * @function saTpiLoadDrmCredentials
 *
 * @brief   storage agent TPI function to load DRM credentials
 *
 * @param[in] ClientId - ID of client who can access these
 * @param[in] DataPtr - pointer to credential data
 * @param[in] DataLength - length of data
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiLoadNflxDrmCredentials( nfTeeClientId_t ClientId, uint8_t *DataPtr, uint32_t DataLength )
{
	(void) ClientId;
	(void) DataPtr;
	(void) DataLength;

	STORAGEDBG("%s: this function is unimplemented\n", __FUNCTION__ );
	//assert(0);
	return (nfTeeStorageResult_t) nfTGR_INTERNAL_ERROR;
}

/**************************************************
 * @function saTpiProtectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] DataPtr - pointer to data that you want protected
 * @param[in] DataLength - length of unprotected data
 * @param[out] ProtectedDataPtr - pointer to preallocated buffer for protected data
 * @param[in] MaxProtectedDataLength - size of buffer at ProtectedDataPtr
 * @param[out] ProtectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiProtectData( nfTeeClientId_t	ClientId,
					  	  	  	  	   uint8_t 			*DataPtr,
					  	  	  	  	   uint32_t 		DataLength,
									  uint8_t 			*ProtectedDataPtr,
									  uint32_t 			MaxProtectedDataLength,
									  uint32_t 			*ProtectedDataLengthPtr )
{
	uint32_t 					ResponseCode;
	uint32_t					TotalLength;
	uint8_t						AppAesKey[saAES_128_KEY_SIZE_BYTES];
	uint8_t						AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t						*BytePtr;
	uint8_t						*IVPtr;
	int							CiphertextLength;
	uint32_t					HmacDataLength;
	int							HmacLength;
	int							B64Length;

	/*
	 * This is not as easy as it looks. We need to do the following:
	 *
	 *  - compute final length, puke if it won't fit
	 * 	- get the app keys for this client
	 * 	- generate (random) IV
	 * 	- encrypt (padding if need be) the data
	 * 	- prepend version and IV
	 * 	- compute HMAC over version, IV, and encrypted data
	 * 	- append HMAC
	 * 	- b64-encode result into ProtectedDataPtr
	 * 	- update *ProtectedLengthPtr
	 */

	TotalLength = sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES + DataLength;
	if ( DataLength % 16 != 0 )
		TotalLength += saAES_128_KEY_SIZE_BYTES - (DataLength % 16);

	if ( TotalLength > (uint32_t) nfTEE_MAX_PROTECT_SIZE )
	{
		STORAGEDBG("%s: data too large (%d)\n", __FUNCTION__, TotalLength );
		return nfTSR_CHUNK_SIZE_EXCEEDED;
	}

	if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
	{
		STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
		return ResponseCode;
	}

	// start assembling the data
	BytePtr = WorkBuf;
	*((uint32_t *)BytePtr) = saStorageVersion;
	BytePtr += sizeof(uint32_t);

	// get IV into WorkBuf
	if ( ! OPENSSL_LIBTOM_EXCHANGE(teeCryptoRandomBytes)( BytePtr, saAES_128_KEY_SIZE_BYTES ) )
	{
		// this is fatal
		STORAGEDBG("%s: failed to allocate random bytes for key seal IV\n", __FUNCTION__ );
		return nfTSR_CRYPT_RANDOM_FAILURE;
	}

	// save pointer to IV, advance BytePtr beyond IV
	IVPtr = BytePtr;
	BytePtr += saAES_128_KEY_SIZE_BYTES;

	/*
	 *  Now, put encrypted data at BytePtr. Note that we can fudge the max size parm
	 *  for the crypto call because we already know we have more than enough space in
	 *  Workbuf (yeah, yeah, I'm being lazy...)
	 */
	CiphertextLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcEncrypt)( DataPtr, DataLength, AppAesKey, IVPtr, BytePtr,
									DataLength + saAES_128_KEY_SIZE_BYTES );

	if ( CiphertextLength <= 0 )
	{
		STORAGEDBG("%s: encrypt failure, bailing.", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	BytePtr += CiphertextLength;

	// how much data to HMAC?
	HmacDataLength = (uint32_t) (BytePtr - WorkBuf);

	// compute HMAC, store at BytePtr
	HmacLength = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( WorkBuf, HmacDataLength, AppHmacKey,
								saHMAC_SHA256_KEY_SIZE_BYTES, BytePtr );

	CRYPTODBG("%s: back from teeHmacSha256, HmacLength is %d\n", __FUNCTION__, HmacLength );

	if ( HmacLength != saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
		return nfTSR_CIPHER_ERROR;
	}

	//  make sure there is room for b64-encoded data
	if ( ( B64Length = Base64encode_len(HmacDataLength + HmacLength)) > (int) MaxProtectedDataLength )
	{
		CRYPTODBG("%s: not enough room at protected data (%d/%d)\n", __FUNCTION__, MaxProtectedDataLength,
				Base64encode_len(HmacDataLength + HmacLength) );
		return nfTSR_OPERAND_TOO_SHORT;
	}

	STORAGEDBG("%s: B64Length is %d, MaxProtectedDataLength is %d\n", __FUNCTION__,
			 	 B64Length, MaxProtectedDataLength );

	// okay, do the encoding
	B64Length = Base64encode( (char *) ProtectedDataPtr,
			(const char *) WorkBuf, HmacDataLength + HmacLength );

	if ( B64Length <= 0 )
		return nfTSR_INTERNAL_ERROR;

	*ProtectedDataLengthPtr = (uint32_t) B64Length;
#if 0
	STORAGEDBG("%s: here's a hexdump of WorkBuf:\n", __FUNCTION__);
	teePrintHexDump(WorkBuf, HmacDataLength + HmacLength);

	STORAGEDBG("%s: here's a hexdump of ProtectedData:\n", __FUNCTION__);
	teePrintHexDump(ProtectedDataPtr, *ProtectedLengthPtr);
#endif
	// okay, we're good.
	return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiProtectData */


/**************************************************
 * @function saTpiUnrotectData
 *
 * @brief   storage agent TPI function to protect application data
 *
 * @param[in] ClientId - ID of client requesting operation
 * @param[in] ProtectedDataPtr - pointer to protected data
 * @param[in] ProtectedDataLength - length of protected data
 * @param[out] DataPtr - pointer to preallocated buffer for unprotected data
 * @param[in] MaxDataLength - size of buffer at DataPtr
 * @param[out] UnprotectedDataLengthPtr - actual output length will be stored here.
 *
 * @return 0 if successful, -1 otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiUnprotectData( nfTeeClientId_t 	ClientId,
					    				 uint8_t 			*ProtectedDataPtr,
					    				 uint32_t 			ProtectedDataLength,
					    				 uint8_t 			*DataPtr,
										 uint32_t 			MaxDataLength,
										 uint32_t 			*UnprotectedDataLengthPtr )
{
	uint32_t 		ResponseCode;
	uint32_t		TotalLength;
	uint8_t			AppAesKey[saAES_128_KEY_SIZE_BYTES];
	uint8_t			AppHmacKey[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t			HmacValue[saHMAC_SHA256_KEY_SIZE_BYTES];
	uint8_t			*BytePtr;
	uint8_t			*IVPtr;
	int				CiphertextLength;
	int				CleartextLength;
	uint32_t		HmacDataLength;
	int				HmacLength;

	/*
	 * We need to do the following:
	 *
	 *  - b64-decode the protected data
	 *  - sanity-check length, version
	 *  - compute worst-case final length, puke if it won't fit
	 * 	- get the app keys for this client
	 *  - validate HMAC
	 * 	- decrypt the data
	 */

	// XXX - make sure b64 data is NULL-terminated?

	// make sure decoded data will fit in workbuf
	if ( ( TotalLength = Base64decode_len((const char *)ProtectedDataPtr) ) > (int) sizeof(WorkBuf) )
	{
		STORAGEDBG("%s: can't fit decoded key into work buffer (%d/%d)\n", __FUNCTION__,
				TotalLength, sizeof(WorkBuf) );
		return nfTSR_CHUNK_SIZE_EXCEEDED;
	}

	// sanity-check total length
	if ( ProtectedDataLength <= TotalLength )
	{
		STORAGEDBG("%s: ProtectedDataLength is too short?! (%d/%d)\n", __FUNCTION__,
					ProtectedDataLength, TotalLength );
		return nfTSR_INTERNAL_ERROR;
	}

	// b64-decode protected data into workbuf
	if ( ( TotalLength = Base64decode((char *)WorkBuf, (const char *)ProtectedDataPtr) ) <= 0 )
	{
		STORAGEDBG("%s: Base64decode returned %d\n", __FUNCTION__, TotalLength );
		return nfTSR_INTERNAL_ERROR;
	}

	/*
	 * Sanity-check length; we know it should be version + IV + encr data + HMAC value, and because
	 * we use AES-128-CBC, encrypted data is at least the size of the key (the block size).
	 */
	if ( TotalLength < sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES
						+ saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		// this can't be valid data.
		STORAGEDBG("%s: Base64decode returned %d, this is too small.\n", __FUNCTION__, TotalLength );
		return nfTSR_OPERAND_TOO_SHORT;
	}

	// check the version
	BytePtr = WorkBuf;
	if ( *((uint32_t *)BytePtr) != saStorageVersion )
	{
		// format error
		STORAGEDBG("%s: unrecognized version (%08x); TotalLength: %d.\n", __FUNCTION__,
					*((uint32_t *)BytePtr), TotalLength );
		//teePrintHexDump( WorkBuf, TotalLength );
		return nfTSR_UNRECOGNIZED_VERSION;
	}

	STORAGEDBG("%s: version is okay, getting app keys...\n", __FUNCTION__);

	BytePtr += sizeof(uint32_t);

	// get the app keys for this client
	if ( ( ResponseCode = saGetAppKeys( ClientId, AppAesKey, AppHmacKey ) ) != nfTGR_SUCCESS )
	{
		STORAGEDBG("%s: failed to get app keys, bailing.\n", __FUNCTION__);
		return ResponseCode;
	}

	STORAGEDBG("%s: got app keys...\n", __FUNCTION__);

	// save pointer to IV, advance BytePtr beyond IV
	IVPtr = BytePtr;
	BytePtr += saAES_128_KEY_SIZE_BYTES;

	// Verify HMAC
	HmacDataLength = (uint32_t) (TotalLength - saHMAC_SHA256_KEY_SIZE_BYTES);

	STORAGEDBG("%s: HmacDataLength: %d.\n", __FUNCTION__, HmacDataLength);
	// compute HMAC, store at BytePtr
	HmacLength = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( WorkBuf, HmacDataLength, AppHmacKey,
								saHMAC_SHA256_KEY_SIZE_BYTES, HmacValue );

	STORAGEDBG("%s: back from teeHmacSha256, HmacLength is %d\n", __FUNCTION__, HmacLength );

	if ( HmacLength != saHMAC_SHA256_KEY_SIZE_BYTES )
	{
		STORAGEDBG("%s: HMAC failed (length: %d)\n", __FUNCTION__, HmacLength );
		return nfTSR_CIPHER_ERROR;
	}

	// compare calculated value to actual value
	if ( teeMemcmp( HmacValue, WorkBuf + HmacDataLength, saHMAC_SHA256_KEY_SIZE_BYTES ) != 0 )
	{
		STORAGEDBG("%s: HMAC failure on protected data\n", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	/*
	 * Okay, data is valid, so decrypt into passed buffer. Remember, IVPtr
	 * is pointing to IV, BytePtr is pointing to encrypted data. We need
	 * ciphertext length. It is TotalLength - (version + IV + HMAC).
	 */

	CiphertextLength = TotalLength - (sizeof(saStorageVersion) + saAES_128_KEY_SIZE_BYTES
			                           + saHMAC_SHA256_KEY_SIZE_BYTES );

	CleartextLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcDecrypt)( BytePtr, CiphertextLength, AppAesKey,
										   IVPtr, DataPtr, MaxDataLength );

	if ( CleartextLength <= 0 )
	{
		STORAGEDBG("%s: decrypt failure, bailing.", __FUNCTION__);
		return nfTSR_CIPHER_ERROR;
	}

	*UnprotectedDataLengthPtr = (uint32_t) CleartextLength;
#if 0
	STORAGEDBG("%s: here's a hexdump of ClearDataPtr:\n", __FUNCTION__);
	teePrintHexDump(ClearDataPtr, CleartextLength);
#endif
	// okay, we're good.
	return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiUnprotectData */

/**************************************************
 * @function saTpiInit
 *
 * @brief   storage agent TPI initialization
 *
 * @param[in] InitDataPtr - pointer to initialization data
 * @param[in] InitDataLength - length of initialization data
 * @param[out] StatePtr - pointer to where you want storage agent state
 *
 * @return nfTGR_SUCCESS if initialized, error otherwise
 *
 *********************************************************************/
nfTeeStorageResult_t saTpiInit( uint8_t 				*InitDataPtr,
								uint32_t 				InitDataLength,
								nfStorageAgentState_t	*StatePtr )
{
	int 		rc;
	//uint8_t 	*SSFileNamePtr;
	uint8_t		*BytePtr;
	uint16_t	TotalLength;

	(void) InitDataLength;

	/*
	 * We need to do the following:
	 *
	 * 	- construct pathname for secure store
	 * 	- load encrypted/hmac'd secure store into memory
	 * 	- validate (hmac)
	 * 	- decrypt
	 * 	- initialize saManufSSTable
	 * 	- initialize app service
	 */

	saManufSSDataPtr = InitDataPtr;
	saManufSSDataLength = InitDataLength;

	// step over secure store header, extract TotalLength

	BytePtr = (saManufSSDataPtr + nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE);
    TotalLength = ((*BytePtr) << 8) + *(BytePtr + 1);
    BytePtr += sizeof(uint16_t);

    // error check
    if (TotalLength > (saManufSSDataLength - nfSS_FILE_MAGIC_SIZE - nfSS_FILE_VERSION_SIZE - nfSS_HEADER_HMAC_SIZE))
    {
    	STORAGEDBG("%s: TotalLength too large (%d)\n", __FUNCTION__, TotalLength );
    	return nfTSR_MANUF_SSTORE_INVALID;
    }

    STORAGEDBG("%s: header verified, total length: %d\n", __FUNCTION__, TotalLength );

    // total length includes HMAC, so deduct
    //TotalLength -= nfSS_HEADER_HMAC_SIZE;
    // step over HMAC
    BytePtr += nfSS_HEADER_HMAC_SIZE;
	

	// now, we are pointing at encrypted data: decrypt it
	if ( ( rc = saDecryptManufStore( BytePtr, TotalLength ) ) != 0 )
	{
        STORAGEDBG("%s: decryption/validation of manuf-ss failed, aborting\n", __FUNCTION__ );
        return nfTSR_MANUF_SSTORE_INVALID;
	}

	
/*
 * The encrypted data looks like this:
 *===================================================================
 *--------------------- BEGIN ENCRYPTED DATA ------------------------
 *===================================================================
 * PADDING LENGTH                      2 bytes (for AES)
 *------------------------------------------------ padding total:2 bytes (fixed)
 * TEE MGMT Section ID                 4 bytes
 * section length                      2 bytes
 * NTBA RO section ID                  4 bytes
 * NTBA RO client ID                  16 bytes
 * NTBA RW section ID                  4 bytes
 * NTBA RW client ID                  16 bytes
 * DRM RO section ID                  4 bytes
 * DRM RO client ID                  16 bytes
 * DRM RW section ID                  4 bytes
 * DRM RW client ID                  16 bytes
 * NRD section ID                     4 bytes
 * NRD client ID                     16 bytes
 *------------------------------------------------ Mgmt section total:106 bytes (fixed)
 * NTBA RO Section ID                  4 bytes
 * section length                      2 bytes
 * ESN key                             4 bytes
 * length                              2 bytes
 * data (ESN)                         42 bytes (variable, 11-42 bytes)
 * KPE key                             4 bytes
 * length                              2 bytes
 * data (128 aes key)                 16 bytes
 * KPH key                             4 bytes
 * length                              2 bytes
 * data (SHA-256 key)                 32 bytes
 *----------------------------------------------- NTBA RO section total: 126 bytes (maximum)
 * DRM RO Section ID                   4 bytes
 * section length                      2 bytes
 * MODEL_PRIV Key                      4 bytes
 * length                              2 bytes
 * data (private key)                 32 bytes (256 bit ECC key)
 * MODEL_CERT Key                      4 bytes
 * length                              2 bytes
 * data (model cert)                1596 bytes
 * DEVCERT_TEMPLATE Key                4 bytes
 * length                              2 bytes
 * data (device cert template)      3974 bytes
 *----------------------------------------------- DRM RO section total: 5628 bytes (fixed)
 * padding (for AES)                  15 bytes (max, may be 0)
 * MAC value                          20 bytes (SHA1 of all sections, including pad)
 *----------------------------------------------- TRAILER total: 35 bytes (maximum)
 *===================================================================
 *---------------------- END ENCRYPTED DATA -------------------------
 *===================================================================
 *
 * We've successfully decrypted the data, BytePtr points to padding length. We
 * want to decode each section, storing associated pointers in saManufSSTable.
 * Boring grunt work, but somebody's gotta do it.
 */
	// strip padding, store inner record length
    STORAGEDBG("%s: %d bytes of encryption padding\n", __FUNCTION__, (*BytePtr << 8) + *(BytePtr + 1) );
    TotalLength = TotalLength - ((*BytePtr << 8) + *(BytePtr + 1) + sizeof(uint16_t));
    BytePtr += sizeof(uint16_t);

    // deduct hash length
    TotalLength -= nfSS_RECORD_ICV_SIZE;

    // okay -- time to locate elements of interest in secure store
   do
   {
       /*
        * process each manuf-secstore file section. Remember, each section looks like this:
        *
        * <unique ID><16-bit section length><data>
        *
        * and there are an arbitrary number of sections (actually, only 3 today).
        */
        if ( ( rc = GetNextManufSection( &BytePtr, &TotalLength ) ) != 0 )
        {
           STORAGEDBG("%s: GetNextSection returned %d; bailing.\n", __FUNCTION__, rc );
           return nfTSR_MANUF_SSTORE_INVALID;
        }

   } while (TotalLength > 0);

   /*
    * Special case for KPW: need to derive from Kpe/Kph; do that now.
    */
   // saLocalKPWStorage[] should hold KPW
   saManufSSTable[nfSSRI_KPW].DataLength = saDeriveKpw( saManufSSTable[nfSSRI_KPE].DataPtr,
		   	   	   	   	   	   	   	   	   	   	   	    saManufSSTable[nfSSRI_KPH].DataPtr,
		   	   	   	   	   	   	   	   	   	   	   	    saLocalKPWStorage, sizeof(saLocalKPWStorage) );
   if (saManufSSTable[nfSSRI_KPW].DataLength > 0)
   {
	   saManufSSTable[nfSSRI_KPW].DataPtr = saLocalKPWStorage;
	   saManufSSTable[nfSSRI_KPW].ClientKey = ntbaAGENT_KEY_KPW;
	   saManufSSTable[nfSSRI_KPW].ClientIdPtr = saManufSSTable[nfSSRI_KPE].ClientIdPtr;
   }
   else
   {
	   STORAGEDBG("%s: failed to derive Kpw, bailing.\n", __FUNCTION__);
	   return nfTSR_KEY_DERIV_ERROR;
   }

   /*
    * Derive key ladder for app storage services
    */
   /*
    * caInternalKey is our key ladder key. We will use it to derive our internal
    * AES and HMAC-SHA256 keys.
    */
   {/* begin local scope limiter */
	   uint8_t TempBuf[saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES];
	   int rc;

		// first, encrypt saKeyLadderBase; this becomes saInternalKey
		(void) teeMemcpy( saInternalKey, saKeyLadderBase, sizeof(saInternalKey) );
		rc = OPENSSL_LIBTOM_EXCHANGE(teeAesCbc128EncryptZeroIV)( saInternalKey,
				sizeof(saInternalKey), _SecStoreAesKey );
		if ( rc != 0 )
		{
			CRYPTODBG("%s: failed to create key ladder base!?\n", __FUNCTION__);
			// fatal
			return nfTSR_INTERNAL_ERROR;
		}
		
		if ( ( rc = OPENSSL_LIBTOM_EXCHANGE(teeSha384)(saInternalKey, saAES_128_KEY_SIZE_BYTES, TempBuf) ) != 0 )
		{
			CRYPTODBG("%s: teeSha384() failed!?\n", __FUNCTION__);
			// fatal
			return nfTSR_INTERNAL_ERROR;
		}
		 
		
		// first 128 bits is AES key, next 256 is HMAC key
		(void)teeMemcpy( saInternalAesKey, TempBuf, saAES_128_KEY_SIZE_BYTES );
		(void)teeMemcpy( saInternalHMACKey, TempBuf + saAES_128_KEY_SIZE_BYTES, saHMAC_SHA256_KEY_SIZE_BYTES );

		CRYPTODBG("%s: dumping ladder rungs...\n", __FUNCTION__);
		//teePrintHexDump( TempBuf, 48 );

		// clear other keys from memory
		(void) teeBzero(saInternalKey, saAES_128_KEY_SIZE_BYTES);
		(void) teeBzero(TempBuf, saAES_128_KEY_SIZE_BYTES + saHMAC_SHA256_KEY_SIZE_BYTES);
		CRYPTODBG("%s: created internal keys.\n", __FUNCTION__);

   }/* end local scope limiter */

   // update state
   *StatePtr = nfSAS_RUN;

	#if 0
   printf("here is dump all keys : \n");
	
   teePrintHexDump_tag("ESN:",saManufSSTable[nfSSRI_ESN].DataPtr,saManufSSTable[nfSSRI_ESN].DataLength);
   teePrintHexDump_tag("KPE:",saManufSSTable[nfSSRI_KPE].DataPtr,saManufSSTable[nfSSRI_KPE].DataLength);
   teePrintHexDump_tag("KPH:",saManufSSTable[nfSSRI_KPH].DataPtr,saManufSSTable[nfSSRI_KPH].DataLength);
   teePrintHexDump_tag("KPW:",saManufSSTable[nfSSRI_KPW].DataPtr,saManufSSTable[nfSSRI_KPW].DataLength);
   teePrintHexDump_tag("saInternalAesKey:",saInternalAesKey,saAES_128_KEY_SIZE_BYTES);		
   teePrintHexDump_tag("saInternalHMACKey:",saInternalHMACKey,saHMAC_SHA256_KEY_SIZE_BYTES);
   teePrintHexDump_tag("nfSSRI_DRM_GROUP_PRIV:",saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataPtr,saManufSSTable[nfSSRI_DRM_GROUP_PRIV].DataLength);
   teePrintHexDump_tag("nfSSRI_DRM_GROUP_CERT:",saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataPtr,saManufSSTable[nfSSRI_DRM_GROUP_CERT].DataLength);
   teePrintHexDump_tag("nfSSRI_DRM_CERTTEMPLATE:",saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataPtr,saManufSSTable[nfSSRI_DRM_CERTTEMPLATE].DataLength);
	#endif
   return (nfTeeStorageResult_t) nfTGR_SUCCESS;

}/* end saTpiInit */
