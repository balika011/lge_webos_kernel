/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeTypes.h"
#include "CommTypes.h"
#include "CryptoAgent.h"
#include "KeyMgmt.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "CommonIncludes.h"


//for aescbc chuck mechansim
uint8_t ChuckIVPtr[nfTEE_AES_IV_MAX];
uint8_t* ChuckAesKeyPtr = NULL;
uint32_t ChuckAesKeyLength = 0;
uint32_t AesInDataTotalLength = 0;
/////////////////////////////////


/*********************************************************************
 * @function caDispatchDeleteKeyMsg
 *
 * @brief Crypto Agent "delete keys" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchDeleteKeyMsg( uint8_t			*InMsgPtr,
									  uint32_t			InMsgBufLength,
									  uint8_t			*OutMsgPtr,
									  uint32_t			OutMsgBufLength,
									  uint32_t			*MsgLengthPtr,
									  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t ResponseCode = nfTGR_SUCCESS;

	(void) OutMsgBufLength;
	(void) OutMsgPtr;

	/*
	 * nfTeeDeleteKeyRequest_t has the following fields:
	 * typedef struct
     * {
	 *		nfCryptoKeyHandle_t	KeyHandle;
	 * } nfTeeDeleteKeyRequest_t;
	 *
	 *  We need to do the following:
	 *  	- validate inmsg/outmsg
	 *  	- call key mgmt function to delete key
	 *  	- get everything into outmsg
	 */

	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( InMsgBufLength <  nfMIN_DELETEKEY_REQ_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_DELETEKEY_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	// default value in case of error
	*MsgLengthPtr = 0;

	ResponseCode = (nfTeeResult_t) caDeleteKey( ClientId,
										teeNtohl(((nfTeeDeleteKeyRequest_t *)InMsgPtr)->KeyHandle) );

	return ResponseCode;

}/* end caDispatchDeleteKeyMsg */

/*********************************************************************
 * @function caDispatchClearKeysMsg
 *
 * @brief Crypto Agent "clear keys" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchClearKeysMsg( uint8_t			*InMsgPtr,
									  uint32_t			InMsgBufLength,
									  uint8_t			*OutMsgPtr,
									  uint32_t			OutMsgBufLength,
									  uint32_t			*MsgLengthPtr,
									  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t ResponseCode = nfTGR_SUCCESS;

	/*
	 * nfTeeClearKeysRequest_t has the following fields:
	 *		(empty message)
	 *
	 *  We need to do the following:
	 *  	- validate inmsg/outmsg
	 *  	- call key mgmt function to clear keys
	 *  	- get everything into outmsg
	 */

	(void) InMsgBufLength;

	/* first, validate message length -- InMsg should be empty */
	if ( InMsgPtr != NULL )
	{
		CRYPTODBG("%s: InMsgPtr is non-NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_CLEARKEYS_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_CLEARKEYS_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default value in case of error
	*MsgLengthPtr = 0;

	/*
	 * We want caClearKeys to return a (variable) list of deleted handles. Best place
	 * to put this list is in OutMsg, but (depending on what the REE allocated for
	 * OutMsg) there may not be enough room for the list. Also, caClearKeys doesn't know
	 * about API byte ordering. So, we'll pass the OutMsg field pointers and max length
	 * to caClearKeys, and assuming there is enough room in OutMsg, we'll byte-swap
	 * everything upon return. A little painful, but it saves us from dynamic memory
	 * allocation.
	 */
	uint32_t MaxHandleListSize = OutMsgBufLength - sizeof(uint32_t);

	ResponseCode = (nfTeeResult_t) caClearKeys( ClientId, MaxHandleListSize,
								&((nfTeeClearKeysResponse_t *)OutMsgPtr)->NumHandles,
								((nfTeeClearKeysResponse_t *)OutMsgPtr)->DeletedHandles );

	if ( ResponseCode == nfTGR_SUCCESS )
	{
		uint32_t NumHandles = ((nfTeeClearKeysResponse_t *)OutMsgPtr)->NumHandles;

		/*
		 *  caClearKeys() doesn't know about byte-ordering issues, so we need
		 *  to byte-swap NumHandles and each handle in DeletedHandles[]
		 */
		if ( NumHandles > 0 )
		{
			int i;

			((nfTeeClearKeysResponse_t *)OutMsgPtr)->NumHandles = teeHtonl(NumHandles);
			for ( i = 0; i < (int) NumHandles; ++i )
			{
				((nfTeeClearKeysResponse_t *)OutMsgPtr)->DeletedHandles[i] =
						teeHtonl(((nfTeeClearKeysResponse_t *)OutMsgPtr)->DeletedHandles[i]);
			}/* end for ( i = 0; i < NumHandles; ++i ) */
		}

		// update message length
		*MsgLengthPtr = sizeof(nfTeeClearKeysResponse_t) + (NumHandles * sizeof(uint32_t));
	}

	return ResponseCode;

}/* end caDispatchClearKeysMsg */

/*********************************************************************
 * @function caDispatchNflxDhDeriveMsg
 *
 * @brief Crypto Agent "Netflix DH derive" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchNflxDhDeriveMsg( uint8_t			*InMsgPtr,
							  	  	  	 uint32_t			InMsgBufLength,
							  	  	  	 uint8_t			*OutMsgPtr,
							  	  	  	 uint32_t			OutMsgBufLength,
							  	  	  	 uint32_t			*MsgLengthPtr,
							  	  	  	 nfTeeClientId_t	ClientId )
{
	nfTeeResult_t		ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t	DHPrivKeyHandle = 0;
	nfCryptoKeyHandle_t	KeyDerivationKeyHandle = 0;
	nfCryptoKeyHandle_t	KceHandle = 0;
	nfCryptoKeyHandle_t	KchHandle = 0;
	nfCryptoKeyHandle_t	KcdHandle = 0;
	uint32_t			PeerPubLength = 0;
	uint8_t				*DHPubValuePtr = NULL;

	/*
	 * nfTeeNflxDhDeriveKeyRequest_t has the following fields:
	 *
	 *	typedef struct
	 *	{
	 *		nfCryptoKeyHandle_t	DHPrivKeyHandle;
	 *		nfCryptoKeyHandle_t	KeyDerivationKeyHandle;
	 *		uint32_t			PeerDHPubValueLength;
	 *		uint8_t				PeerDHPubValue[];
	 *	} nfTeeNflxDhDeriveKeyRequest_t;
	 *
	 *  We need to do the following:
	 *  	- validate inmsg/outmsg
	 *  	- validate private key handle
	 *  	- validate kdk handle
	 *  	- validate peer pub value
	 *  	- call key mgmt function to create keys
	 *  	- get everything into outmsg
	 */

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_NFLX_DHDERIVE_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_GENDH_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_GENDH_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_GENDH_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default value in case of error
	*MsgLengthPtr = 0;

	// get the private key handle
	DHPrivKeyHandle = teeNtohl(((nfTeeNflxDhDeriveKeyRequest_t *)InMsgPtr)->DHPrivKeyHandle);

	// get the key derivation key handle
	KeyDerivationKeyHandle = teeNtohl(((nfTeeNflxDhDeriveKeyRequest_t *)InMsgPtr)->KeyDerivationKeyHandle);

	/*
	 * Validate peer public length. We'll check two things here:
	 * 	- does length make sense given InMsgBufLength?
	 * 	- is length <= nfMAX_DH_MODULUS_LENGTH ?
	 */
	PeerPubLength = teeNtohl(((nfTeeNflxDhDeriveKeyRequest_t *)InMsgPtr)->PeerDHPubValueLength);
	if ( PeerPubLength != (InMsgBufLength - sizeof(nfTeeNflxDhDeriveKeyRequest_t)) )
	{
		CRYPTODBG("%s: PubLength mismatch (%d/%d)\n", __FUNCTION__, PeerPubLength,
				(InMsgBufLength - sizeof(nfTeeNflxDhDeriveKeyRequest_t)));
		return (nfTeeResult_t) nfTCR_MALFORMED_DATA;
	}
	else if ( PeerPubLength > nfMAX_DH_MODULUS_LENGTH )
	{
		CRYPTODBG("%s: ModulusLength invalid (%d)\n", __FUNCTION__, PeerPubLength );
		return (nfTeeResult_t) nfTCR_MALFORMED_DATA;
	}

	// get pointer to peer public value in outmsg
	DHPubValuePtr = ((nfTeeNflxDhDeriveKeyRequest_t *)InMsgPtr)->PeerDHPubValue;

	ResponseCode = (nfTeeResult_t) caDhDeriveKeys( ClientId, DHPrivKeyHandle, KeyDerivationKeyHandle,
									PeerPubLength, DHPubValuePtr, &KceHandle,
									   &KchHandle, &KcdHandle );

	/*
	 *  We want to build this data structure in OutMsg:
	 *
	 *  typedef struct
	 *	{
	 *		nfCryptoKeyHandle_t	KceHandle; // encryption key
	 *		nfCryptoKeyHandle_t	KchHandle; // hmac key
	 *		nfCryptoKeyHandle_t	KcdHandle; // key derivation key
	 *	} nfTeeNflxDhDeriveKeyResponse_t;
	 *
	 */
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		// Copy key handles into outmsg
		((nfTeeNflxDhDeriveKeyResponse_t *)OutMsgPtr)->KceHandle = teeHtonl(KceHandle);
		((nfTeeNflxDhDeriveKeyResponse_t *)OutMsgPtr)->KchHandle = teeHtonl(KchHandle);
		((nfTeeNflxDhDeriveKeyResponse_t *)OutMsgPtr)->KcdHandle = teeHtonl(KcdHandle);

		// don't forget to update message length
		*MsgLengthPtr = sizeof(nfTeeNflxDhDeriveKeyResponse_t);
	}


	return ResponseCode;

}/* end caDispatchNflxDhDeriveMsg */

/*********************************************************************
 * @function caDispatchDhGenKeysMsg
 *
 * @brief Crypto Agent "generate DH keys" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchDhGenKeysMsg( uint8_t			*InMsgPtr,
							  	  	  uint32_t			InMsgBufLength,
							  	  	  uint8_t			*OutMsgPtr,
							  	  	  uint32_t			OutMsgBufLength,
							  	  	  uint32_t			*MsgLengthPtr,
							  	  	  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t		ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t	DHPrivKeyHandle = 0;
	uint32_t			Generator;
	uint32_t			ModulusLength;
	uint8_t	*			RandomKey;

	/*
	 * nfTeeGenerateDHKeyRequest_t has the following fields:
	 *
	 *  - Generator
	 *	- ModulusLength
	 *	- Modulus
	 *
	 *  We need to do the following:
	 *  	- validate inmsg/outmsg
	 *  	- validate generator
	 *  	- validate modulus
	 *  	- call key mgmt function to create keys
	 *  	- get everything into outmsg
	 */

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_GENDH_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_GENDH_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_GENDH_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_GENDH_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default value
	*MsgLengthPtr = 0;

	/*
	 * Validate generator. Note that for Netflix, we know this will typically be very
	 * small (2, 3, 5, etc.). So, we'll make sure it is less than our predefined
	 * upper bound.
	 */
#if 0
	if ( ( Generator = teeNtohl(((nfTeeGenerateDHKeyRequest_t *)InMsgPtr)->Generator)) > nfMAX_DH_GENERATOR )
	{
		CRYPTODBG("%s: invalid generator value (%d).\n", __FUNCTION__, Generator);
		return (nfTeeResult_t) nfTCR_INVALID_DH_GENERATOR;
	}
#else
	Generator = teeNtohl(((nfTeeGenerateDHKeyRequest_t *)InMsgPtr)->Generator);
#endif

	CRYPTODBG("%s: generator111111 is %08x\n", __FUNCTION__, Generator);

	#if 0
	RandomKey = ((nfTeeGenerateDHKeyRequest_t *)InMsgPtr)->RandomKey;
	teePrintHexDump_tag("RandomKey",RandomKey,nfMAX_DH_PRIVKEY_LENGTH);
	#endif
	/*
	 * Validate modulus length. We'll check two things here:
	 * 	- does modulus length make sense given InMsgBufLength?
	 * 	- is nfMIN_DH_MODULUS_LENGTH <= length <= nfMAX_DH_MODULUS_LENGTH ?
	 */
	ModulusLength = teeNtohl(((nfTeeGenerateDHKeyRequest_t *)InMsgPtr)->ModulusLength);
	if ( ModulusLength != (InMsgBufLength - sizeof(nfTeeGenerateDHKeyRequest_t)) )
	{
		CRYPTODBG("%s: ModulusLength mismatch (%d/%d)\n", __FUNCTION__, ModulusLength,
				(InMsgBufLength - sizeof(nfTeeGenerateDHKeyRequest_t)));
		return (nfTeeResult_t) nfTCR_MALFORMED_DATA;
	}
	else if ( ( ModulusLength < nfMIN_DH_MODULUS_LENGTH )
		   || ( ModulusLength > nfMAX_DH_MODULUS_LENGTH ) )
	{
		CRYPTODBG("%s: ModulusLength invalid (%d)\n", __FUNCTION__, ModulusLength );
		return (nfTeeResult_t) nfTCR_MALFORMED_DATA;
	}

	/*
	 *  We want to build these data structures in OutMsg:
	 *
	 *	typedef struct
	 *	{
	 * 		nfCryptoKeyHandle_t	DHPrivKeyHandle;
	 * 		nfCryptoKeyHandle_t	DHPubKeyHandle;
	 *	} nfTeeGenerateDHKeyResponse_t;
	 *
	 */

	// do it
	ResponseCode = caDhGenKeys( ClientId, Generator,/*RandomKey,nfMAX_DH_PRIVKEY_LENGTH,*/
								((nfTeeGenerateDHKeyRequest_t *)InMsgPtr)->Modulus,
								ModulusLength, &DHPrivKeyHandle  );

	if ( ResponseCode == nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: generated DH keys, updating OutMsg...\n", __FUNCTION__ );
		((nfTeeGenerateDHKeyResponse_t *)OutMsgPtr)->DHPrivKeyHandle = teeHtonl(DHPrivKeyHandle);
		((nfTeeGenerateDHKeyResponse_t *)OutMsgPtr)->DHPubKeyHandle = teeHtonl(DHPrivKeyHandle);
		*MsgLengthPtr = sizeof(nfTeeGenerateDHKeyResponse_t);
	}

	return ResponseCode;

}/* end caDispatchDhGenKeysMsg */

/*********************************************************************
 * @function caDispatchImportSealedKeyMsg
 *
 * @brief Crypto Agent "import sealed key" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ESN value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchImportSealedKeyMsg( uint8_t			*InMsgPtr,
							  	  	   	    uint32_t		InMsgBufLength,
							  	  	   	    uint8_t			*OutMsgPtr,
							  	  	   	    uint32_t		OutMsgBufLength,
							  	  	   	    uint32_t		*MsgLengthPtr,
							  	  	   	    nfTeeClientId_t	ClientId )
{

	nfTeeResult_t					ResponseCode = nfTGR_SUCCESS;
	nfTeeImportSealedKeyResponse_t	*RspPtr;
	nfTeeImportSealedKeyRequest_t	*ReqPtr;
	nfCryptoKeyHandle_t				KeyHandle;
	nfKeyType_t						KeyType;
	nfCryptoAlgorithm_t				Algorithm;
	nfKeyUsageFlagsWord_t 			KeyUsageFlags;
	uint32_t						KeyBitCount; // how many bits in this key?

	/*
	 * nfTeeHmacImportSealedKeyRequest_t has the following fields:
	 *
	 *  - DataLength
	 *	- KeyData[]
	 *
	 *  We need to do the following:
	 *  	- call appropriate key management function to import key
	 *  	- fix up message response header
	 *  	- bda bda bda... that's all, folks!
	 */

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_IMPORT_SEALED_KEY_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_IMPORT_SEALED_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_IMPORT_SEALED_KEY_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_IMPORT_SEALED_KEY_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/*
	 *  We want to build these data structures in OutMsg:
	 *
	 *	typedef struct
	 *	{
	 *		nfCryptoKeyHandle_t		KeyHandle;
	 *		nfKeyType_t				KeyType;
	 *		nfCryptoAlgorithm_t		Algorithm;
	 *   	nfKeyUsageFlagsWord_t 	KeyUsageFlags;
	 *		uint32_t				KeyBitCount; // how many bits in this key?
	 *	} nfTeeImportSealedKeyResponse_t;
	 *
	 */

	ReqPtr = (nfTeeImportSealedKeyRequest_t *)(InMsgPtr);
	RspPtr = (nfTeeImportSealedKeyResponse_t *)(OutMsgPtr);
	// the only thing allowed after ReqPtr->DataLength is DataLength bytes
	if ( teeHtonl(ReqPtr->DataLength) != (InMsgBufLength - sizeof(uint32_t)) )
	{
		CRYPTODBG("%s: DataLength mismatch (%d/%d)\n", __FUNCTION__, htonl(ReqPtr->DataLength),
				(InMsgBufLength - sizeof(uint32_t)));
		return (nfTeeResult_t) nfTCR_MALFORMED_DATA;
	}

	// get pointers to DataLength and SealedKeyData for caExportSealedKey...
	ResponseCode = (nfTeeResult_t) caImportSealedKey( ClientId, &KeyType, &Algorithm,
									  &KeyUsageFlags, &KeyBitCount, &KeyHandle,
									  teeHtonl(ReqPtr->DataLength), ReqPtr->KeyData );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caImportSealedKey failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
	}
	else
	{
		*MsgLengthPtr = nfMIN_IMPORT_SEALED_KEY_RSP_SIZE;
		RspPtr->Algorithm = teeHtonl(Algorithm);
		RspPtr->KeyUsageFlags = teeHtonl(KeyUsageFlags);
		RspPtr->KeyBitCount = teeHtonl(KeyBitCount);
		RspPtr->KeyHandle = teeHtonl(KeyHandle);
		RspPtr->KeyType = teeHtonl(KeyType);
	}

	return ResponseCode;

}/* end caDispatchImportSealedKeyMsg */

/*********************************************************************
 * @function caDispatchExportSealedKeyMsg
 *
 * @brief Crypto Agent "export sealed key" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ESN value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchExportSealedKeyMsg( uint8_t			*InMsgPtr,
							  	  	   	    uint32_t		InMsgBufLength,
							  	  	   	    uint8_t			*OutMsgPtr,
							  	  	   	    uint32_t		OutMsgBufLength,
							  	  	   	    uint32_t 		*MsgLengthPtr,
							  	  	   	    nfTeeClientId_t	ClientId )
{

	nfTeeResult_t					ResponseCode = nfTGR_SUCCESS;
	nfTeeExportSealedKeyResponse_t	*RspPtr;
	nfCryptoKeyHandle_t 			KeyHandle;
	uint32_t						DataLength = 0;

	/*
	 * nfTeeHmacExportSealedKeyRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *
	 *  We need to do the following:
	 *  	- call appropriate key management function to seal key
	 *  	- fix up message response header
	 *  	- bda bda bda... that's all, folks!
	 */

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_EXPORT_SEALED_KEY_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_EXPORT_SEALED_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_EXPORT_SEALED_KEY_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_EXPORT_SEALED_KEY_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get key handle from inmsg
	KeyHandle = (nfCryptoKeyHandle_t) teeNtohl((( nfTeeExportSealedKeyRequest_t *)InMsgPtr)->KeyHandle);

	CRYPTODBG("%s: called to seal key for handle %08x\n", __FUNCTION__, KeyHandle );

	/*
	 *  We want to build these data structures in OutMsg:
	 *
	 *
	 *	typedef struct
	 *	{
	 *		uint32_t				DataLength;
	 *		uint8_t	    			SealedKeyData[];
	 *	} nfTeeExportSealedKeyResponse_t;
	 *
	 */
	RspPtr = (nfTeeExportSealedKeyResponse_t *)OutMsgPtr;

	// get pointers to DataLength and SealedKeyData for caExportSealedKey...
	ResponseCode = (nfTeeResult_t) caExportSealedKey( ClientId, KeyHandle,
									  RspPtr->SealedKeyData,
									  &DataLength,
									  OutMsgBufLength - sizeof(nfTeeApiRspHdr_t) - sizeof(uint32_t) );


	RspPtr->DataLength = teeHtonl(DataLength);
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caExportSealedKey failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
	}
	else
		*MsgLengthPtr = sizeof(uint32_t) + DataLength;


	CRYPTODBG("%s: returning successfully...\n", __FUNCTION__);

	return ResponseCode;

}/* end caDispatchExportSealedKeyMsg */

/*********************************************************************
 * @function caDispatchGetEsnMsg
 *
 * @brief Crypto Agent "Get ESN" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ESN value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchGetEsnMsg( uint8_t			*InMsgPtr,
							  	   uint32_t			InMsgBufLength,
							  	   uint8_t			*OutMsgPtr,
							  	   uint32_t			OutMsgBufLength,
							  	   uint32_t			*MsgLengthPtr,
							  	   nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfTeeGetEsnResponse_t 	*RspPtr;
	uint32_t				RspLength;
	uint32_t				EsnLength;

	/*
	 * nfTeeGetEsnRequest_t is currently empty (we rely on operation to tell us
	 * what to do)
	 *
	 *  We need to do the following:
	 *  	- make sure there's enough room in outmsg
	 *  	- get the ESN
	 *  	- fix up message response header
	 */

	if ( ( InMsgBufLength != 0 ) || ( InMsgPtr != NULL ) )
	{
		CRYPTODBG( "%s: InMsgBufLength is %d, InMsgPtr is %p (expect 0/NULL)\n",
				__FUNCTION__, InMsgBufLength, InMsgPtr );
		return (nfTeeResult_t) nfTCR_BAD_PARAMETER;
	}

	/* first, validate message length */
	if ( OutMsgBufLength <  nfMAX_ESN_RESPONSE_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMAX_ESN_RESPONSE_SIZE );
		return (nfTeeResult_t)nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t)nfTCR_INTERNAL_ERROR;
	}

	RspPtr = (nfTeeGetEsnResponse_t *)OutMsgPtr;
	RspLength = OutMsgBufLength - sizeof(uint32_t);

	// get the ESN handle
	ResponseCode = caGetEsn( ClientId, RspLength, RspPtr->Esn, &EsnLength );
	RspPtr->EsnLength = teeHtonl(EsnLength);

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
	}
	else
		*MsgLengthPtr = EsnLength + sizeof(uint32_t);

	return ResponseCode;

}/* end caDispatchGetEsnMsg */

/*********************************************************************
 * @function caDispatchHmacSha256Msg
 *
 * @brief Crypto Agent "HMAC SHA256" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256Msg( uint8_t			*InMsgPtr,
									   uint32_t			InMsgBufLength,
									   uint8_t			*OutMsgPtr,
									   uint32_t			OutMsgBufLength,
									   uint32_t			*MsgLengthPtr,
									   nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	int 					DataLength = 0;
	int						OutLength = 0;

	/*
	 * nfTeeHmacSha256Request_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- DataLength
	 *	- Data
	 *
	 *  We need to do the following:
	 *  	- make sure there's some data to hmac
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength <  nfHMAC_SHA256_OUTPUT_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfHMAC_SHA256_OUTPUT_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get data length
	DataLength = teeNtohl(((nfTeeHmacSha256Request_t *)InMsgPtr)->DataLength);

	// validate DataLength
	if ( DataLength != (int)(InMsgBufLength - sizeof(nfTeeHmacSha256Request_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
				(InMsgBufLength - sizeof(nfTeeHmacSha256Request_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}
	// get the key handle
	KeyHandle = (nfCryptoKeyHandle_t) teeNtohl(((nfTeeHmacSha256Request_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_SIGN ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		*MsgLengthPtr = 0;
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
		return ResponseCode;
	}

	// okay, call the HMAC operation, putting result into OutMsg
	OutLength = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( ((nfTeeHmacSha256Request_t *)InMsgPtr)->Data, DataLength,
							 	HmacKeyPtr, HmacKeyLength,
								((nfTeeHmacSha256Response_t *)OutMsgPtr)->HmacValue );



	if ( OutLength == nfHMAC_SHA256_OUTPUT_SIZE )
	{
		CRYPTODBG("%s: teeHmacSha256 returned, output is %d bytes...\n.",
					__FUNCTION__, OutLength);
		// set length/ResponseCode in outmsg
		ResponseCode = nfTGR_SUCCESS;
		*MsgLengthPtr = sizeof(nfTeeHmacSha256Response_t);
	}
	else
	{
		CRYPTODBG("%s: teeHmacSha256 failed (OutLength %d)...\n.",
					__FUNCTION__, OutLength);
		ResponseCode = (nfTeeResult_t) nfTCR_CIPHER_ERROR;
		*MsgLengthPtr = 0;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256Msg */

/*********************************************************************
 * @function caDispatchHmacSha256InitMsg
 *
 * @brief Crypto Agent "HMAC SHA256 INIT" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256InitMsg( uint8_t			*InMsgPtr,
									   	   uint32_t			InMsgBufLength,
									   	   uint8_t			*OutMsgPtr,
									   	   uint32_t			OutMsgBufLength,
									   	   uint32_t			*MsgLengthPtr,
									   	   nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;

	/*
	 * nfTeeHmacSha256InitRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *
	 *  We need to do the following:
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_INIT_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_INIT_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength <  nfMIN_HMACSHA256_INIT_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_HMACSHA256_INIT_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get the key handle
	KeyHandle = (nfCryptoKeyHandle_t) teeNtohl(((nfTeeHmacSha256InitRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_SIGN ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call hmac-init function, which
		 * returns context handle.
		 */
		nfTeeHmacSha256InitResponse_t *RspMsgPtr = (nfTeeHmacSha256InitResponse_t *) OutMsgPtr;

		ResponseCode = caHmacSha256Init( KeyHandle, HmacKeyPtr, &RspMsgPtr->CryptoContextHandle );

		CRYPTODBG("%s: caHmacSha256Init() returned %08x\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
			*MsgLengthPtr = sizeof(nfTeeHmacSha256InitResponse_t);
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256InitMsg */

/*********************************************************************
 * @function caDispatchHmacSha256UpdateMsg
 *
 * @brief Crypto Agent "HMAC SHA256 UPDATE" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256UpdateMsg( uint8_t			*InMsgPtr,
									   	   	 uint32_t			InMsgBufLength,
									   	   	 uint8_t			*OutMsgPtr,
									   	   	 uint32_t			OutMsgBufLength,
									   	   	 uint32_t			*MsgLengthPtr,
									   	   	 nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;

	/*
	 * nfTeeHmacSha256UpdateRequest_t has the following fields:
	 *
	 *	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *	uint32_t					DataLength;
	 *	uint8_t	    				Data[];
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	// NB: we don't check OutMsg because we don't currently use it.
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_UPDATE_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_UPDATE_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}


	// get the key info from handle
	HandlePtr = &((nfTeeHmacSha256UpdateRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_SIGN ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		uint32_t DataLength = teeNtohl(((nfTeeHmacSha256UpdateRequest_t *)InMsgPtr)->DataLength);

		/*
		 * This is one we know how to do. Call hmac-update function.
		 */
		// validate DataLength
		if ( DataLength != (InMsgBufLength - sizeof(nfTeeHmacSha256UpdateRequest_t)) )
		{
			// this buffer is too small to hold a valid message
			CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
					(InMsgBufLength - sizeof(nfTeeHmacSha256Request_t)));
			return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
		}

		ResponseCode = caHmacSha256Update( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeHmacSha256UpdateRequest_t *)InMsgPtr)->Data, DataLength );

		CRYPTODBG("%s: caHmacSha256Update() returned %d\n", __FUNCTION__, ResponseCode );

		// we don't need to update *MsgLengthPtr, this one is empty...
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256UpdateMsg */

/*********************************************************************
 * @function caDispatchHmacSha256FinalMsg
 *
 * @brief Crypto Agent "HMAC SHA256 FINAL" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256FinalMsg( uint8_t			*InMsgPtr,
									   	   	uint32_t			InMsgBufLength,
									   	   	uint8_t			*OutMsgPtr,
									   	   	uint32_t			OutMsgBufLength,
									   	   	uint32_t			*MsgLengthPtr,
									   	   	nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;

	/*
	 * nfTeeHmacSha256FinalRequest_t has the following fields:
	 *
	 *	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_FINAL_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_FINAL_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength <  nfMIN_HMACSHA256_FINAL_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_HMACSHA256_FINAL_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get the key info from handle
	HandlePtr = &((nfTeeHmacSha256FinalRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_SIGN ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call hmac-update function.
		 */

		ResponseCode = caHmacSha256Final( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeHmacSha256FinalResponse_t *)OutMsgPtr)->HmacValue,
				nfHMAC_SHA256_OUTPUT_SIZE, &OutLength );

		CRYPTODBG("%s: caHmacSha256Update() returned %08x\n", __FUNCTION__, ResponseCode );

		if ( ResponseCode == nfTGR_SUCCESS )
			*MsgLengthPtr = OutLength;
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256FinalMsg */


/*********************************************************************
 * @function caDispatchHmacSha256VerifyMsg
 *
 * @brief Crypto Agent "HMAC SHA256 VERIFY" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256VerifyMsg( uint8_t			*InMsgPtr,
									   	     uint32_t			InMsgBufLength,
									   	     uint8_t			*OutMsgPtr,
									   	     uint32_t			OutMsgBufLength,
									   	     uint32_t			*MsgLengthPtr,
									   	     nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	uint8_t					ComputedHmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
	int 					DataLength = 0;
	int						OutLength = 0;

	/*
	 * nfTeeHmacSha256VerifyRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- HmacValue
	 *	- DataLength
	 *	- Data
	 *
	 *  We need to do the following:
	 *  	- make sure there's some data to hmac
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *		- compare HMAC values
	 *		- return SUCCESS/FAIL
	 */

	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_VERIFY_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_VERIFY_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// XXX - ignore OutMsg, we don't use it.

	// default setting
	*MsgLengthPtr = 0;

	// get data length
	DataLength = teeNtohl(((nfTeeHmacSha256VerifyRequest_t *)InMsgPtr)->DataLength);

	// validate DataLength
	if ( DataLength != (int)(InMsgBufLength - sizeof(nfTeeHmacSha256VerifyRequest_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
				(InMsgBufLength - sizeof(nfTeeHmacSha256VerifyRequest_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}
	// get the key handle
	KeyHandle = (nfCryptoKeyHandle_t) teeNtohl(((nfTeeHmacSha256VerifyRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = (nfTeeResult_t) caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & (nfKUF_VERIFY) ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// okay, call the HMAC operation, putting result into OutMsg
	OutLength = OPENSSL_LIBTOM_EXCHANGE(teeHmacSha256)( ((nfTeeHmacSha256VerifyRequest_t *)InMsgPtr)->Data, DataLength,
							 		HmacKeyPtr, HmacKeyLength,
									ComputedHmacValue );


	if ( OutLength == nfHMAC_SHA256_OUTPUT_SIZE )
	{
		CRYPTODBG("%s: teeHmacSha256 returned, output is %d bytes...\n.",
					__FUNCTION__, OutLength);
		// compare values
		if ( teeMemcmp( ComputedHmacValue,
						((nfTeeHmacSha256VerifyRequest_t *)InMsgPtr)->HmacValue, OutLength ) == 0 )
		{
			// okay, they match
			ResponseCode = nfTGR_SUCCESS;
		}
		else
		{
			CRYPTODBG("%s: HMAC verification FAILED\n.", __FUNCTION__ );
			ResponseCode = (nfTeeResult_t) nfTCR_HMAC_VERIFY_FAILED;
		}
	}
	else
	{
		CRYPTODBG("%s: teeHmacSha256 failed (OutLength %d)...\n.",
					__FUNCTION__, OutLength);
		ResponseCode = (nfTeeResult_t) nfTCR_CIPHER_ERROR;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256VerifyMsg */

/*********************************************************************
 * @function caDispatchHmacSha256VerifyInitMsg
 *
 * @brief Crypto Agent "HMAC SHA256 VERIFY INIT" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256VerifyInitMsg( uint8_t			*InMsgPtr,
									   	   	     uint32_t			InMsgBufLength,
									   	   	     uint8_t			*OutMsgPtr,
									   	   	     uint32_t			OutMsgBufLength,
									   	   	     uint32_t			*MsgLengthPtr,
									   	   	     nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;

	/*
	 * nfTeeHmacSha256VerifyInitRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *
	 *  We need to do the following:
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_VERIFY_INIT_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_VERIFY_INIT_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength <  nfMIN_HMACSHA256_VERIFY_INIT_RSP_SIZE )
	{
		// this buffer is too small to hold the response
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_HMACSHA256_VERIFY_INIT_RSP_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get the key handle
	KeyHandle = (nfCryptoKeyHandle_t) teeNtohl(((nfTeeHmacSha256VerifyInitRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_VERIFY ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call hmac-init function, which
		 * returns context handle.
		 */
		nfTeeHmacSha256VerifyInitResponse_t *RspMsgPtr = (nfTeeHmacSha256VerifyInitResponse_t *) OutMsgPtr;

		ResponseCode = caHmacSha256Init( KeyHandle, HmacKeyPtr, &RspMsgPtr->CryptoContextHandle );

		CRYPTODBG("%s: caHmacSha256Init() returned %08x\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
			*MsgLengthPtr = sizeof(nfTeeHmacSha256VerifyInitResponse_t);
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256VerifyInitMsg */

/*********************************************************************
 * @function caDispatchHmacSha256VerifyUpdateMsg
 *
 * @brief Crypto Agent "HMAC SHA256 VERIFY UPDATE" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256VerifyUpdateMsg( uint8_t			*InMsgPtr,
									   	   	 	   uint32_t			InMsgBufLength,
									   	   	 	   uint8_t			*OutMsgPtr,
									   	   	 	   uint32_t			OutMsgBufLength,
									   	   	 	   uint32_t			*MsgLengthPtr,
									   	   	 	   nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;

	/*
	 * nfTeeHmacSha256VerifyUpdateRequest_t has the following fields:
	 *
	 *	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *	uint32_t					DataLength;
	 *	uint8_t	    				Data[];
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- fix up message response header
	 */

	// NB: we don't check OutMsg because we don't currently use it.
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_VERIFY_UPDATE_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_VERIFY_UPDATE_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}


	// get the key info from handle
	HandlePtr = &((nfTeeHmacSha256VerifyUpdateRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// XXX - need so support all HMAC algs

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_VERIFY ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		uint32_t DataLength = teeNtohl(((nfTeeHmacSha256VerifyUpdateRequest_t *)InMsgPtr)->DataLength);

		/*
		 * This is one we know how to do. Call hmac-update function.
		 */

		// validate DataLength first
		if ( DataLength != (InMsgBufLength - sizeof(nfTeeHmacSha256VerifyUpdateRequest_t)) )
		{
			// this buffer is too small to hold a valid message
			CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
					(InMsgBufLength - sizeof(nfTeeHmacSha256VerifyUpdateRequest_t)));
			return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
		}
		ResponseCode = caHmacSha256Update( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeHmacSha256VerifyUpdateRequest_t *)InMsgPtr)->Data, DataLength );

		CRYPTODBG("%s: caHmacSha256Update() returned %d\n", __FUNCTION__, ResponseCode );

		// we don't need to update *MsgLengthPtr, this one is empty...
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256VerifyUpdateMsg */

/*********************************************************************
 * @function caDispatchHmacSha256VerifyFinalMsg
 *
 * @brief Crypto Agent "HMAC SHA256 FINAL" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (hmac value)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchHmacSha256VerifyFinalMsg( uint8_t			*InMsgPtr,
									   	   		  uint32_t			InMsgBufLength,
									   	   		  uint8_t			*OutMsgPtr,
									   	   		  uint32_t			OutMsgBufLength,
									   	   		  uint32_t			*MsgLengthPtr,
									   	   		  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t 	KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	KeyAlgorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*HmacKeyPtr = NULL;
	uint32_t				HmacKeyLength = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;

	/*
	 * nfTeeHmacSha256VerifyFinalRequest_t has the following fields:
	 *
	 *	nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *	uint8_t	    				HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate hmac algorithm based on key size
	 *  	- check hmac value against passed value
	 *  	- return success/fail
	 */

	// NB: we don't use OutMsg, so we don't check these values
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	// default value
	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength <  nfMIN_HMACSHA256_VERIFY_FINAL_REQ_SIZE  )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength,  nfMIN_HMACSHA256_VERIFY_FINAL_REQ_SIZE );
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}


	// get the key info from handle
	HandlePtr = &((nfTeeHmacSha256VerifyFinalRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
									 	 &KeyType, &KeyAlgorithm, &KeyUsageFlags );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( KeyAlgorithm != nfCA_HMAC )
	  || ( ( KeyUsageFlags & nfKUF_SIGN ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, KeyAlgorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetHmacKeyFromHandle( ClientId, KeyHandle, &HmacKeyPtr, &HmacKeyLength );
	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetHmacKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( HmacKeyLength == nfTEE_HMAC_SHA256_KEY_SIZE_BYTES )
	{
		uint8_t HmacValue[nfHMAC_SHA256_OUTPUT_SIZE];

		/*
		 * This is one we know how to do. Call hmac-final function.
		 */

		ResponseCode = caHmacSha256Final( (caIncrementalCryptoContext_t *)HandlePtr,
								HmacValue,nfHMAC_SHA256_OUTPUT_SIZE, &OutLength );

		CRYPTODBG("%s: caHmacSha256Update() returned %08x\n", __FUNCTION__, ResponseCode );

		if ( ResponseCode == nfTGR_SUCCESS )
		{
			// verify value passed by client
			if ( teeMemcmp( HmacValue,
					((nfTeeHmacSha256VerifyFinalRequest_t *)InMsgPtr)->HmacValue,
					nfHMAC_SHA256_OUTPUT_SIZE ) != 0 )
			{
				ResponseCode = nfTCR_HMAC_VERIFY_FAILED;
			}

		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, HmacKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchHmacSha256VerifyFinalMsg */

/*********************************************************************
 * @function caDispatchAesCbcEncryptMsg
 *
 * @brief Crypto Agent "aes-cbc decrypt" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with cleartext
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ciphertext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcEncryptMsg( uint8_t			*InMsgPtr,
								 	 	  uint32_t			InMsgBufLength,
								 	 	  uint8_t			*OutMsgPtr,
								 	 	  uint32_t			OutMsgBufLength,
								 	 	  uint32_t			*MsgLengthPtr,
								 	 	  nfTeeClientId_t	ClientId )
{

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;
	int						OutLength;
	uint8_t					*RspMsgPtr = OutMsgPtr;
	int						DataLength;

	/*
	 * nfTeeAesCbcEncryptRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- IV
	 *	- DataLength
	 *	- Data
	 *
	 *  We need to do the following:
	 *  	- make sure there's some data to encrypt
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate crypto algorithm based on key size
	 *  	- fix up message response header
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_ENCR_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_ENCR_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_ENCR_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_ENCR_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	DataLength = teeNtohl(((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->DataLength);

	// validate DataLength
	if ( DataLength != (int)(InMsgBufLength - sizeof(nfTeeAesCbcEncryptRequest_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
				(InMsgBufLength - sizeof(nfTeeAesCbcEncryptRequest_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}
	// get the key info from handle
	KeyHandle = teeNtohl(((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
		return ResponseCode;
	}

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_ENCRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		*MsgLengthPtr = 0;
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		*MsgLengthPtr = 0;
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		// okay, call the AES encrypt operation, putting ciphertext into OutMsg
		OutLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcEncrypt)( ((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->Data,
										 teeNtohl(((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->DataLength),
										 AesKeyPtr,
										 ((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->IV,
										 ((nfTeeAesCbcEncryptResponse_t *)RspMsgPtr)->EncryptedData,
										 OutMsgBufLength - sizeof(nfTeeAesCbcEncryptResponse_t) );
#if TEE_LIBTOM_TEST_ENABLE
	//teePrintHexDump_tag("[teeAes128CbcEncrypt] EncryptedData :", ((nfTeeAesCbcEncryptResponse_t *)RspMsgPtr)->EncryptedData,OutLength );

	uint8_t temp_cipher_buf[4096*8];
	teeBzero(temp_cipher_buf,4096*8);
	
	int rc__ = teeAes128CbcEncrypt_libtom(((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->Data,
										 teeNtohl(((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->DataLength),
										 AesKeyPtr,
										 ((nfTeeAesCbcEncryptRequest_t *)InMsgPtr)->IV,
										 temp_cipher_buf,
										 4096*2);
	
	//teePrintHexDump_tag("[teeAes128CbcDecrypt_libtom] temp_cipher_buf :", temp_cipher_buf, rc__);
	printf("[sahala compare_buffer] EncryptedData vs. temp_cipher_buf\n");
	int Ret = compare_buffer(((nfTeeAesCbcEncryptResponse_t *)RspMsgPtr)->EncryptedData,temp_cipher_buf,OutLength,rc__);
	if(Ret != 0)
		exit(1);

#endif
	
		// XXX - should OutLength be >= DataLength ?
		if ( OutLength > 0 )
		{
			CRYPTODBG("%s: teeAes128CbcEncrypt successfully encrypted, output is %d bytes...\n.",
						__FUNCTION__, OutLength);
			// set length/ResponseCode in outmsg
			((nfTeeAesCbcEncryptResponse_t *)RspMsgPtr)->DataLength = teeHtonl(OutLength);
			ResponseCode = nfTGR_SUCCESS;
			*MsgLengthPtr = OutLength + sizeof(nfTeeAesCbcEncryptResponse_t);
		}
		else
		{
			CRYPTODBG("%s: teeAes128CbcEncrypt failed (OutLength %d)...\n.",
						__FUNCTION__, OutLength);
			ResponseCode = (nfTeeResult_t) nfTCR_CIPHER_ERROR;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchUnwrapKeyMsg */

/*********************************************************************
 * @function caDispatchAesCbcEncryptInitMsg
 *
 * @brief Crypto Agent "aes-cbc encrypt init" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with key handle and IV
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (cipher context)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcEncryptInitMsg( uint8_t			*InMsgPtr,
								 	 	  	  uint32_t			InMsgBufLength,
								 	 	  	  uint8_t			*OutMsgPtr,
								 	 	  	  uint32_t			OutMsgBufLength,
								 	 	  	  uint32_t			*MsgLengthPtr,
								 	 	  	  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;

	/*
	 * nfTeeAesCbcEncryptInitRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- IV
	 *
	 *  We need to do the following:
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate crypto init algorithm based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_ENCRYPT_INIT_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_ENCRYPT_INIT_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_ENCRYPT_INIT_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_ENCRYPT_INIT_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	KeyHandle = teeNtohl(((nfTeeAesCbcEncryptInitRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_ENCRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-init function, which
		 * returns context handle.
		 */
		nfTeeAesCbcEncryptInitResponse_t *RspMsgPtr = (nfTeeAesCbcEncryptInitResponse_t *) OutMsgPtr;

		ResponseCode = caAes128CbcEncryptInit( KeyHandle, AesKeyPtr,
				 ((nfTeeAesCbcEncryptInitRequest_t *)InMsgPtr)->IV,
				 &RspMsgPtr->CryptoContextHandle );

		CRYPTODBG("%s: caAes128CbcEncryptInit() returned %08x\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
			*MsgLengthPtr = sizeof(nfTeeAesCbcEncryptInitResponse_t);
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcEncryptInitMsg */

/*********************************************************************
 * @function caDispatchAesCbcEncryptUpdateMsg
 *
 * @brief Crypto Agent "aes-cbc encrypt update" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with crypto context and data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ciphertext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcEncryptUpdateMsg( uint8_t			*InMsgPtr,
								 	 	  	  	uint32_t		InMsgBufLength,
								 	 	  	  	uint8_t			*OutMsgPtr,
								 	 	  	  	uint32_t		OutMsgBufLength,
								 	 	  	  	uint32_t		*MsgLengthPtr,
								 	 	  	  	nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;

	/*
	 * nfTeeAesCbcEncryptUpdateRequest_t has the following fields:
	 *
	 * nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 * uint32_t					DataLength;
	 * uint8_t	    				Data[];
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate crypto update function based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_ENCRYPT_UPDATE_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_ENCRYPT_UPDATE_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_ENCRYPT_UPDATE_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_ENCRYPT_UPDATE_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	HandlePtr = &((nfTeeAesCbcEncryptUpdateRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_ENCRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-update function, which
		 * returns context handle.
		 */

		ResponseCode = caAes128CbcEncryptUpdate( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeAesCbcEncryptUpdateRequest_t *)InMsgPtr)->Data,
				teeNtohl(((nfTeeAesCbcEncryptUpdateRequest_t *)InMsgPtr)->DataLength),
				((nfTeeAesCbcEncryptUpdateResponse_t *)OutMsgPtr)->EncryptedData,
				OutMsgBufLength, &OutLength );

		CRYPTODBG("%s: caAes128CbcEncryptUpdate() returned %d\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
		{
			CRYPTODBG("%s: stored %d encrypted bytes in OutMsg...\n", __FUNCTION__, OutLength );
			((nfTeeAesCbcEncryptUpdateResponse_t *)OutMsgPtr)->EncryptedDataLength = teeHtonl(OutLength);
			*MsgLengthPtr = sizeof(nfTeeAesCbcEncryptUpdateResponse_t) + OutLength;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcEncryptUpdateMsg */

/*********************************************************************
 * @function caDispatchAesCbcEncryptFinalMsg
 *
 * @brief Crypto Agent "aes-cbc encrypt final" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with crypto context
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ciphertext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcEncryptFinalMsg( uint8_t			*InMsgPtr,
								 	 	   	   uint32_t			InMsgBufLength,
								 	 	   	   uint8_t			*OutMsgPtr,
								 	 	   	   uint32_t			OutMsgBufLength,
								 	 	   	   uint32_t			*MsgLengthPtr,
								 	 	   	   nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;

	/*
	 * nfTeeAesCbcEncryptFinalRequest_t has the following fields:
	 *
	 * nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate crypto final algorithm based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_ENCRYPT_FINAL_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_ENCRYPT_FINAL_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_ENCRYPT_FINAL_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_ENCRYPT_FINAL_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	HandlePtr = &((nfTeeAesCbcEncryptFinalRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_ENCRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-update function, which
		 * returns context handle.
		 */

		ResponseCode = caAes128CbcEncryptFinal( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeAesCbcEncryptFinalResponse_t *)OutMsgPtr)->EncryptedData,
				OutMsgBufLength, &OutLength );

		CRYPTODBG("%s: caAes128CbcEncryptUpdate() returned %d\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
		{
			CRYPTODBG("%s: stored %d encrypted bytes in OutMsg...\n", __FUNCTION__, OutLength );
			((nfTeeAesCbcEncryptFinalResponse_t *)OutMsgPtr)->EncryptedDataLength = teeHtonl(OutLength);
			*MsgLengthPtr = sizeof(nfTeeAesCbcEncryptFinalResponse_t) + OutLength;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcEncryptFinalMsg */





/*********************************************************************
 * @function caDispatchAesCbcDecryptMsg
 *
 * @brief Crypto Agent "aes-cbc decrypt" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with ciphertext
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (cleartext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcDecryptMsg( uint8_t			*InMsgPtr,
								 	 	  uint32_t			InMsgBufLength,
								 	 	  uint8_t			*OutMsgPtr,
								 	 	  uint32_t			OutMsgBufLength,
								 	 	  uint32_t			*MsgLengthPtr,
								 	 	  nfTeeClientId_t	ClientId )
{
	(void) ClientId; //TODO: this is just to remove compiler warning. Use it or remove from func signature.

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;
	int						OutLength;

	/*
	 * nfTeeAesCbcDecryptRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- IV
	 *	- DataLength
	 *	- EncryptedData
	 *
	 *  We need to do the following:
	 *  	- make sure there's some data to decrypt
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate crypto algorithm based on key size
	 *  	- fix up message response header
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_DECRYPT_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_DECRYPT_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_DECRYPT_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_DECRYPT_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// get the key info from handle
	KeyHandle = teeNtohl(((nfTeeAesCbcDecryptRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check values
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_DECRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// okay, we are good to do crypto, so get the key and do the deed...
	ResponseCode = (nfTeeResult_t) caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		// okay, call the AES encrypt operation, putting ciphertext into OutMsg
		OutLength = OPENSSL_LIBTOM_EXCHANGE(teeAes128CbcDecrypt)( ((nfTeeAesCbcDecryptRequest_t *)InMsgPtr)->EncryptedData,
										 teeNtohl(((nfTeeAesCbcDecryptRequest_t *)InMsgPtr)->DataLength),
										 AesKeyPtr,
										 ((nfTeeAesCbcDecryptRequest_t *)InMsgPtr)->IV,
										 ((nfTeeAesCbcDecryptResponse_t *)OutMsgPtr)->DecryptedData,
										 OutMsgBufLength - sizeof(nfTeeAesCbcDecryptResponse_t) );


		// XXX - should OutLength be <= DataLength ?
		if ( OutLength >= 0 )
		{
			CRYPTODBG("%s: teeAes128CbcDecrypt successfully decrypted, output is %d bytes...\n.",
						__FUNCTION__, OutLength);
			// set length/ResponseCode in outmsg
			((nfTeeAesCbcDecryptResponse_t *)OutMsgPtr)->DataLength = teeHtonl(OutLength);
			ResponseCode = nfTGR_SUCCESS;
			*MsgLengthPtr = OutLength + sizeof(nfTeeAesCbcDecryptResponse_t);
		}
		else
		{
			CRYPTODBG("%s: teeAes128CbcDecrypt failed (OutLength %d)...\n.",
						__FUNCTION__, OutLength);
			ResponseCode = (nfTeeResult_t) nfTCR_CIPHER_ERROR;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcDecryptMsg */


/*********************************************************************
 * @function caDispatchAesCbcDecryptInitMsg
 *
 * @brief Crypto Agent "aes-cbc decrypt init" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with key handle and IV
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (cipher context)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcDecryptInitMsg( uint8_t			*InMsgPtr,
								 	 	  	  uint32_t			InMsgBufLength,
								 	 	  	  uint8_t			*OutMsgPtr,
								 	 	  	  uint32_t			OutMsgBufLength,
								 	 	  	  uint32_t			*MsgLengthPtr,
								 	 	  	  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;

	/*
	 * nfTeeAesCbcDecryptInitRequest_t has the following fields:
	 *
	 *	- KeyHandle
	 *	- IV
	 *
	 *  We need to do the following:
	 *  	- get the key handle, make sure it's valid
	 *  	- determine key size
	 *  	- call appropriate crypto init algorithm based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_DECRYPT_INIT_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_DECRYPT_INIT_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_DECRYPT_INIT_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_DECRYPT_INIT_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	KeyHandle = teeNtohl(((nfTeeAesCbcDecryptInitRequest_t *)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_DECRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-init function, which
		 * returns context handle.
		 */
		nfTeeAesCbcDecryptInitResponse_t *RspMsgPtr = (nfTeeAesCbcDecryptInitResponse_t *) OutMsgPtr;

		ResponseCode = caAes128CbcDecryptInit( KeyHandle, AesKeyPtr,
				 ((nfTeeAesCbcDecryptInitRequest_t *)InMsgPtr)->IV,
				 &RspMsgPtr->CryptoContextHandle );

		CRYPTODBG("%s: caAes128CbcDecryptInit() returned %08x\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
			*MsgLengthPtr = sizeof(nfTeeAesCbcDecryptInitResponse_t);
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcDecryptInitMsg */

/*********************************************************************
 * @function caDispatchAesCbcDecryptUpdateMsg
 *
 * @brief Crypto Agent "aes-cbc decrypt update" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with crypto context and data
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ciphertext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcDecryptUpdateMsg( uint8_t			*InMsgPtr,
								 	 	  	  	uint32_t		InMsgBufLength,
								 	 	  	  	uint8_t			*OutMsgPtr,
								 	 	  	  	uint32_t		OutMsgBufLength,
								 	 	  	  	uint32_t		*MsgLengthPtr,
								 	 	  	  	nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;
	int						DataLength;

	/*
	 * nfTeeAesCbcDecryptUpdateRequest_t has the following fields:
	 *
	 * nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 * uint32_t					DataLength;
	 * uint8_t	    				Data[];
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate crypto update function based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_DECRYPT_UPDATE_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_DECRYPT_UPDATE_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_DECRYPT_UPDATE_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_DECRYPT_UPDATE_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	DataLength = teeNtohl(((nfTeeAesCbcDecryptUpdateRequest_t *)InMsgPtr)->EncryptedDataLength);

	// validate DataLength
	if ( DataLength != (int)(InMsgBufLength - sizeof(nfTeeAesCbcDecryptUpdateRequest_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
				(InMsgBufLength - sizeof(nfTeeAesCbcDecryptUpdateRequest_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}

	// get the key info from handle
	HandlePtr = &((nfTeeAesCbcDecryptUpdateRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_ENCRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-update function, which
		 * returns context handle.
		 */

		ResponseCode = caAes128CbcDecryptUpdate( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeAesCbcDecryptUpdateRequest_t *)InMsgPtr)->EncryptedData,
				teeNtohl(((nfTeeAesCbcDecryptUpdateRequest_t *)InMsgPtr)->EncryptedDataLength),
				((nfTeeAesCbcDecryptUpdateResponse_t *)OutMsgPtr)->Data,
				OutMsgBufLength, &OutLength );

		CRYPTODBG("%s: caAes128CbcDecryptUpdate() returned %d\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
		{
			CRYPTODBG("%s: stored %d encrypted bytes in OutMsg...\n", __FUNCTION__, OutLength );
			((nfTeeAesCbcDecryptUpdateResponse_t *)OutMsgPtr)->DataLength = teeHtonl(OutLength);
			*MsgLengthPtr = sizeof(nfTeeAesCbcDecryptUpdateResponse_t) + OutLength;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcDecryptUpdateMsg */

/*********************************************************************
 * @function caDispatchAesCbcDecryptFinalMsg
 *
 * @brief Crypto Agent "aes-cbc decrypt final" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message with crypto context
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response (ciphertext)
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchAesCbcDecryptFinalMsg( uint8_t			*InMsgPtr,
								 	 	   	   uint32_t			InMsgBufLength,
								 	 	   	   uint8_t			*OutMsgPtr,
								 	 	   	   uint32_t			OutMsgBufLength,
								 	 	   	   uint32_t			*MsgLengthPtr,
								 	 	   	   nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t 			KeyType = nfKF_UNDEFINED;
	nfCryptoAlgorithm_t 	Algorithm = nfCA_UNDEFINED;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags = 0;
	nfTeeCryptoContextHandle_t	*HandlePtr;
	uint32_t				OutLength = 0;
	uint8_t					*AesKeyPtr = NULL;
	uint32_t				AesKeyLength = 0;

	/*
	 * nfTeeAesCbcDecryptFinalRequest_t has the following fields:
	 *
	 * nfTeeCryptoContextHandle_t	CryptoContextHandle;
	 *
	 *  We need to do the following:
	 *		- get key handle out of crypto context, validate
	 *  	- determine key size
	 *  	- call appropriate crypto final algorithm based on key size
	 *  	- fix up message length if appropriate
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_AES_CBC_DECRYPT_FINAL_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_AES_CBC_DECRYPT_FINAL_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer must hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_AES_CBC_DECRYPT_FINAL_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_AES_CBC_DECRYPT_FINAL_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer must be able to hold a response message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	HandlePtr = &((nfTeeAesCbcDecryptFinalRequest_t *)InMsgPtr)->CryptoContextHandle;
	KeyHandle = ((caIncrementalCryptoContext_t *)HandlePtr)->KeyHandle;
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle, &KeyType, &Algorithm, &KeyUsageFlags );


	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetKeyInfoFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}

	// sanity check key
	if ( ( KeyType != nfKT_SECRET )
	  || ( Algorithm != nfCA_AES_CBC )
	  || ( ( KeyUsageFlags & nfKUF_DECRYPT ) == 0 ) )
	{
		CRYPTODBG("%s: operation not allowed (KeyType/alg/flags: %d/%d/%08x\n.",
					__FUNCTION__, KeyType, Algorithm, KeyUsageFlags );
		return (nfTeeResult_t) nfTCR_OPERATION_NOT_ALLOWED;
	}

	// also need to check for supported key length
	ResponseCode = caGetAesKeyFromHandle( ClientId, KeyHandle, &AesKeyPtr, &AesKeyLength );

	if ( ResponseCode != nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: caGetAesKeyFromHandle failed (ResponseCode: %08x)...\n.",
					__FUNCTION__, ResponseCode);
		return ResponseCode;
	}


	if ( AesKeyLength == nfTEE_AES_128_KEY_SIZE_BYTES )
	{
		/*
		 * This is one we know how to do. Call encrypt-update function, which
		 * returns context handle.
		 */

		ResponseCode = caAes128CbcDecryptFinal( (caIncrementalCryptoContext_t *)HandlePtr,
				((nfTeeAesCbcDecryptFinalResponse_t *)OutMsgPtr)->Data,
				OutMsgBufLength, &OutLength );

		CRYPTODBG("%s: caAes128CbcDecryptFinal() returned %d\n", __FUNCTION__, ResponseCode );

		// update msg length if we have crypto context in response
		if ( ResponseCode == nfTGR_SUCCESS )
		{
			CRYPTODBG("%s: stored %d encrypted bytes in OutMsg...\n", __FUNCTION__, OutLength );
			((nfTeeAesCbcDecryptFinalResponse_t *)OutMsgPtr)->DataLength = teeHtonl(OutLength);
			*MsgLengthPtr = sizeof(nfTeeAesCbcDecryptFinalResponse_t) + OutLength;
		}
	}
	else
	{
		CRYPTODBG("%s: unsupported key size (%d)\n", __FUNCTION__, AesKeyLength );
		ResponseCode = (nfTeeResult_t) nfTCR_KEY_LENGTH_UNSUPPORTED;
	}

	return ResponseCode;

}/* end caDispatchAesCbcDecryptFinalMsg */





/*********************************************************************
 * @function caDispatchImportKeyMsg
 *
 * @brief Crypto Agent "import key" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchImportKeyMsg( uint8_t			*InMsgPtr,
							 	 	  uint32_t			InMsgBufLength,
							 	 	  uint8_t			*OutMsgPtr,
							 	 	  uint32_t			OutMsgBufLength,
							 	 	  uint32_t			*MsgLengthPtr,
							 	 	  nfTeeClientId_t	ClientId )
{
	(void) OutMsgBufLength;	//todo: avoids compiler warning, either use it or remove from function signature

	nfTeeResult_t		ResponseCode = nfTGR_SUCCESS;
	nfCryptoAlgorithm_t	Algorithm;
	nfKeyType_t 		KeyType;
	nfCryptoKeyHandle_t	KeyHandle;
	nfKeyFormat_t		KeyFormat;
	nfKeyUsageFlagsWord_t KeyUsageFlags;
	int					DataLength;

	/*
	 * nfTeeImportKeyRequest_t has the following fields:
	 *
	 *  - KeyType
	 *  - Algorithm
	 *  - KeyUsageFlags
	 *  - KeyFormat
	 *  - DataLength
	 *  - KeyData
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key import function. Then, we need to call the import function
	 *  and construct the response message in *OutMsgPtr
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_IMPORT_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_IMPORT_KEY_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_IMPORT_KEY_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_IMPORT_KEY_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// validate DataLength
	DataLength = teeNtohl(((nfTeeImportKeyRequest_t *)InMsgPtr)->DataLength);

	if ( DataLength != (int)(InMsgBufLength - sizeof(nfTeeImportKeyRequest_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, DataLength,
				(InMsgBufLength - sizeof(nfTeeImportKeyRequest_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}
	// get Algorithm and KeyType from message
	Algorithm = teeNtohl((nfCryptoAlgorithm_t)((nfTeeImportKeyRequest_t *)InMsgPtr)->Algorithm);
	KeyFormat = teeNtohl((nfKeyFormat_t) ((nfTeeImportKeyRequest_t *)InMsgPtr)->KeyFormat);

	/*
	 * validate the Algorithm and KeyUsage values.
	 */
	switch ( Algorithm )
	{
		case nfCA_HMAC:
		case nfCA_AES_CBC:
		case nfCA_AES_GCM:
		case nfCA_AES_CTR:
		case nfCA_AES_KW:
		case nfCA_RSAES_PKCS1_V1_5:
		case nfCA_RSA_OAEP:
			// make sure key usage includes encrypt or decrypt?
		case nfCA_RSASSA_PKCS1_V1_5:
			// make sure key usage includes sign?
			break;
		default:
			// no other algorithms make sense
			return (nfTeeResult_t) nfTCR_KEY_TYPE_ALG_MISMATCH;
	}/* end switch Algorithm */
    switch ( KeyFormat )
    {
        case nfKF_RAW:
            break;
        default:
            // no other keyformat make sense
            return (nfTeeResult_t) nfTCR_KEY_TYPE_FORMAT_MISMATCH;
    }/* end switch keyFormat */


	/*
	 * Okay, we've done basic sanity checks and as far as we can tell it's okay,
	 * so call the import function
	 */

	KeyUsageFlags = teeNtohl(((nfTeeImportKeyRequest_t *)InMsgPtr)->KeyUsageFlags);
	ResponseCode = caImportKey( ClientId,
							    KeyFormat,
							    ((nfTeeImportKeyRequest_t *)InMsgPtr)->KeyData,
							    teeNtohl(((nfTeeImportKeyRequest_t *)InMsgPtr)->DataLength),
							    Algorithm,
							    KeyUsageFlags,
							    &KeyHandle,
							    &KeyType );



	if ( ResponseCode == nfTGR_SUCCESS )
	{

		CRYPTODBG("%s: response is okay, constructing outmsg...\n", __FUNCTION__);
		((nfTeeImportKeyResponse_t *)OutMsgPtr)->KeyHandle = teeHtonl(KeyHandle);
		((nfTeeImportKeyResponse_t *)OutMsgPtr)->KeyType = teeHtonl(KeyType);
		*MsgLengthPtr = sizeof( nfTeeImportKeyResponse_t);
	}
	else
	{
		CRYPTODBG("%s: caImportKey failed (ResponseCode: %08x)...\n.", __FUNCTION__, ResponseCode);
	}

	return ResponseCode;

}/* end caDispatchImportKeyMsg */

/*********************************************************************
 * @function caDispatchExportKeyMsg
 *
 * @brief Crypto Agent "export key" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchExportKeyMsg( uint8_t			*InMsgPtr,
							 	 	  uint32_t			InMsgBufLength,
							 	 	  uint8_t			*OutMsgPtr,
							 	 	  uint32_t			OutMsgBufLength,
							 	 	  uint32_t			*MsgLengthPtr,
							 	 	  nfTeeClientId_t	ClientId )
{
	(void) OutMsgBufLength;	//todo: avoids compiler warning

	nfTeeResult_t		ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t KeyHandle;
	nfKeyFormat_t 		KeyFormat;

	uint32_t 			DataLength;
	uint8_t				*KeyDataPtr;
	uint32_t			KeyDataMaxLength;

	/*
	 * nfTeeExportKeyRequest_t has the following fields:
	 *
	 *  - KeyHandle
	 *  - KeyFormat
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key export function. Then, we need to call the export function
	 *  and construct the response message in *OutMsgPtr
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_EXPORT_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_EXPORT_KEY_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_EXPORT_KEY_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_EXPORT_KEY_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	*MsgLengthPtr = 0;


	// get KeyHandle and KeyFormat from message
	KeyHandle = teeNtohl((nfCryptoKeyHandle_t)((nfTeeExportKeyRequest_t *)InMsgPtr)->KeyHandle);
	KeyFormat = teeNtohl((nfKeyFormat_t) ((nfTeeExportKeyRequest_t *)InMsgPtr)->KeyFormat);

	// get pointer to KeyData in outmsg
	KeyDataPtr = (uint8_t *) (OutMsgPtr + sizeof(uint32_t));
	KeyDataMaxLength = OutMsgBufLength - sizeof(uint32_t);

	/*
	 * validate the keyFormat values.
	 */
	switch ( KeyFormat )
	{
		case nfKF_RAW:
			break;
		default:
			// no other keyformat make sense
			return (nfTeeResult_t) nfTCR_KEY_TYPE_FORMAT_MISMATCH;
	}/* end switch keyFormat */

	/*
	 * Okay, we've done basic sanity checks and as far as we can tell it's okay,
	 * so call the import function
	 */

	ResponseCode = caExportKey( ClientId, KeyHandle, KeyFormat,
							    KeyDataPtr, KeyDataMaxLength, &DataLength);

	((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(ResponseCode);


	if ( ResponseCode == nfTGR_SUCCESS )
	{
		CRYPTODBG( "%s: response is okay, constructing outmsg...\n", __FUNCTION__);
		((nfTeeExportKeyResponse_t *)OutMsgPtr)->DataLength = teeHtonl(DataLength);
		// keyData is already in the buffer, since we passed pointer to that location when calling caExportKey
		*MsgLengthPtr = sizeof(nfTeeImportKeyResponse_t) + DataLength;
	}
	else
	{
		CRYPTODBG( "%s: caExportKey failed (ResponseCode: %08x)...\n.", __FUNCTION__, ResponseCode);
	}

	return ResponseCode;

}/* end caDispatchExportKeyMsg */

/*********************************************************************
 * @function caDispatchGetNamedKeyMsg
 *
 * @brief Crypto Agent "get named key (handle)" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchGetNamedKeyMsg( uint8_t			*InMsgPtr,
							 	   	    uint32_t		InMsgBufLength,
							 	   	    uint8_t			*OutMsgPtr,
							 	   	    uint32_t		OutMsgBufLength,
							 	   	    uint32_t		*MsgLengthPtr,
							 	   	    nfTeeClientId_t	ClientId )
{
//	(void) ClientId;	//TODO: avoids compiler warning, either use it or remove from function signature
	(void) OutMsgBufLength;	//TODO: avoids compiler warning, either use it or remove from function signature

	nfTeeResult_t		ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t	KeyHandle;
	int					NameLength;

	/*
	 * nfTeeGetNamedKeyHandleRequest_t has the following fields:
     *
	 *  - NameLength
	 *  - KeyName
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key lookup function, then upon return we need to construct
	 *  the response message in *OutMsgPtr
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_GET_NAMEDKEY_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_GET_NAMEDKEY_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_GET_NAMEDKEY_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_GET_NAMEDKEY_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	*MsgLengthPtr = 0;
	NameLength = teeHtonl(((nfTeeGetNamedKeyHandleRequest_t *)InMsgPtr)->KeyNameLength);
	if ( NameLength != (int)(InMsgBufLength - sizeof(nfTeeGetNamedKeyHandleRequest_t)) )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsg size mismatch (%d/%d)!\n", __FUNCTION__, NameLength,
				(InMsgBufLength - sizeof(nfTeeGetNamedKeyHandleRequest_t)));
		return (nfTeeResult_t)nfTCR_MALFORMED_DATA;
	}

	// okay, get the handle
	ResponseCode = caGetNamedKeyHandle( ClientId, ((nfTeeGetNamedKeyHandleRequest_t *)InMsgPtr)->KeyName,
										teeHtonl(((nfTeeGetNamedKeyHandleRequest_t *)InMsgPtr)->KeyNameLength),
										&KeyHandle );


	// if successful, put handle in outmsg
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: response is okay, constructing outmsg...\n", __FUNCTION__);
		((nfTeeGetNamedKeyHandleResponse_t *)OutMsgPtr)->KeyHandle = teeHtonl(KeyHandle);
		*MsgLengthPtr = sizeof(nfTeeGetNamedKeyHandleResponse_t);
	}
	else
	{
		CRYPTODBG("%s: caGetNamedKeyHandle failed (ResponseCode: %08x)...\n.", __FUNCTION__, ResponseCode);
	}

	return ResponseCode;

}/* end caDispatchGetNamedKeyMsg */

/*********************************************************************
 * @function caDispatchGetKeyInfoMsg
 *
 * @brief Crypto Agent "get key info (handle)" message handler
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static
nfTeeResult_t caDispatchGetKeyInfoMsg( uint8_t			*InMsgPtr,
							 	  	   uint32_t			InMsgBufLength,
							 	  	   uint8_t			*OutMsgPtr,
							 	  	   uint32_t			OutMsgBufLength,
							 	  	   uint32_t			*MsgLengthPtr,
							 	  	   nfTeeClientId_t	ClientId )
{
	(void) OutMsgBufLength;	//TODO: avoids compiler warning, either use it or remove from function signature

	nfTeeResult_t			ResponseCode = nfTGR_SUCCESS;
	nfCryptoKeyHandle_t		KeyHandle;
	nfKeyType_t				KeyType;
	nfCryptoAlgorithm_t 	keyAlgo;
	nfKeyUsageFlagsWord_t 	KeyUsageFlags;


	/*
	 * nfTeeGetKeyInfoRequest_t has the following fields:
     *
	 *  - KeyHandle
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key lookup function, then upon return we need to construct
	 *  the response message in *OutMsgPtr
	 */

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_GETKEYINFO_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgBufLength is %d, expected size is %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_GETKEYINFO_REQ_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_GETKEYINFO_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_GETKEYINFO_RSP_SIZE);
		return (nfTeeResult_t) nfTCR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		CRYPTODBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTCR_INTERNAL_ERROR;
	}

	// default setting
	*MsgLengthPtr = 0;

	// get the key info from handle
	KeyHandle = teeNtohl(((nfTeeGetKeyInfoRequest_t*)InMsgPtr)->KeyHandle);
	ResponseCode = caGetKeyInfoFromHandle( ClientId, KeyHandle,
		 	 	 	 	 	 	 	 &KeyType, &keyAlgo, &KeyUsageFlags );



	// if successful, put info in outmsg
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		CRYPTODBG("%s: response is okay, constructing outmsg...\n", __FUNCTION__);
		((nfTeeGetKeyInfoResponse_t *)OutMsgPtr)->KeyType = teeHtonl(KeyType);
		((nfTeeGetKeyInfoResponse_t *)OutMsgPtr)->Algorithm = teeHtonl(keyAlgo);
		((nfTeeGetKeyInfoResponse_t *)OutMsgPtr)->KeyUsageFlags = teeHtonl(KeyUsageFlags);
		*MsgLengthPtr = sizeof(nfTeeGetKeyInfoResponse_t);
	}
	else
	{
		CRYPTODBG("%s: caGetNamedKeyHandle failed (ResponseCode: %08x)...\n.", __FUNCTION__, ResponseCode);
	}

	return ResponseCode;

}/* end caDispatchGetKeyInfoMsg */

/*********************************************************************
 * @function cryptoAgentDispatchMsg
 *
 * @brief Crypto Agent message handler/entry point
 *
 * @param[in] Operation - operation requested by client
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return void
 *
 *********************************************************************/
nfTeeResult_t cryptoAgentDispatchMsg( nfTeeOperation_t	Operation,
									  uint8_t			*InMsgPtr,
									  uint32_t			InMsgBufLength,
									  uint8_t			*OutMsgPtr,
									  uint32_t			OutMsgBufLength,
				  	   				  uint32_t			*MsgLengthPtr,
									  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t	Result;

	/*
	 * Each agent dispatch function is the second line of defense in the TEE (commAgent
	 * is the first). We should not trust any input; we should always assume that we are
	 * under attack.
	 *
	 * We need to be careful to strike a balance between scaling/efficiency and
	 * modularity. Since this function is strictly a dispatcher (with no knowledge
	 * of specific operation semantics), we'll defer further argument checks to the
	 * operation-specific dispatchers
	 */
	switch ( Operation )
	{
		case nfTO_CRYPTO_OP_GET_NAMED_KEY_HANDLE:
			Result = caDispatchGetNamedKeyMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_ENCRYPT:
			Result = caDispatchAesCbcEncryptMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_INIT:
			Result = caDispatchAesCbcEncryptInitMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_UPDATE:
			Result = caDispatchAesCbcEncryptUpdateMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_ENCRYPT_FINAL:
			Result = caDispatchAesCbcEncryptFinalMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_DECRYPT:
			Result = caDispatchAesCbcDecryptMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_DECRYPT_INIT:
			Result = caDispatchAesCbcDecryptInitMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_DECRYPT_UPDATE:
			Result = caDispatchAesCbcDecryptUpdateMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_AES_CBC_DECRYPT_FINAL:
			Result = caDispatchAesCbcDecryptFinalMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256:
			Result =  caDispatchHmacSha256Msg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_INIT:
			Result =  caDispatchHmacSha256InitMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_UPDATE:
			Result =  caDispatchHmacSha256UpdateMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_FINAL:
			Result =  caDispatchHmacSha256FinalMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY:
			Result =  caDispatchHmacSha256VerifyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_INIT:
			Result =  caDispatchHmacSha256VerifyInitMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_UPDATE:
			Result =  caDispatchHmacSha256VerifyUpdateMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_HMAC_SHA256_VERIFY_FINAL:
			Result =  caDispatchHmacSha256VerifyFinalMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_GET_KEY_INFO:
			Result = caDispatchGetKeyInfoMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_GET_ESN:
			Result = caDispatchGetEsnMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_EXPORT_SEALED_KEY:
			Result = caDispatchExportSealedKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_IMPORT_SEALED_KEY:
			Result = caDispatchImportSealedKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
						OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_EXPORT_KEY:
			Result = caDispatchExportKeyMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_IMPORT_KEY:
			Result = caDispatchImportKeyMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_DH_GEN_KEYS:
			Result = caDispatchDhGenKeysMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_CRYPTO_OP_NFLX_DH_DERIVE:
			Result = caDispatchNflxDhDeriveMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;

		case nfTO_CRYPTO_OP_CLEARKEYS:
			Result = caDispatchClearKeysMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;

		case nfTO_CRYPTO_OP_DELETE_KEY:
			Result = caDispatchDeleteKeyMsg( InMsgPtr, InMsgBufLength, OutMsgPtr,
								OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		default:
		/*	fprintf( stderr, "%s: unimplemented operation (%08x)\n",
					 __FUNCTION__, Operation );*/
			Result = nfTGR_INTERNAL_ERROR;
			break;
	}/* end switch ( Operation ) *//* end switch ( Operation ) */

	return Result;

}/* end cryptoAgentDispatchMsg */

// vim: expandtab tabstop=4 shiftwidth=4
