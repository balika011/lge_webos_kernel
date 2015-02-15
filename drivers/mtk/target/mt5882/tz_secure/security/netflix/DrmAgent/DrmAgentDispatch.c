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
#include "DrmAgent.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"


/*********************************************************************
 * @function drmDispatchPRAllocateRegisterMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_AllocateRegister message
 *
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
nfTeeResult_t
drmDispatchPRAllocateRegisterMsg( uint8_t			*InMsgPtr,
  							   	 uint32_t			InMsgBufLength,
  							   	 uint8_t			*OutMsgPtr,
  							   	 uint32_t			OutMsgBufLength,
  							   	 uint32_t			*MsgLengthPtr,
  							   	 nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
	long                            KeyRegisterIndex = 0;
#if 0
	nfTeeDrmPRAllocateRegisterReq_t	*ReqPtr = (nfTeeDrmPRAllocateRegisterReq_t *) InMsgPtr;
	nfTeeDrmPRAllocateRegisterRsp_t	*RspPtr = (nfTeeDrmPRAllocateRegisterRsp_t *) OutMsgPtr;
#endif
	(void) ClientId;

	/*
	 *
	 * nfTO_DRM_OP_PR_ALLOCATE_REGISTER request looks like this:
	 *	typedef struct
	 *	{
	 *		int		KeyType;				// OEM_HAL_KEY_TYPE f_eKeyType
	 *	} nfTeeDrmPRAllocateRegisterReq_t;
	 *
	 * Response looks like this:
	 *
	 *	typedef struct
	 *	{
	 *		long	KeyRegisterIndex;	// OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex
	 *	} nfTeeDrmPRAllocateRegisterRsp_t;
	 *
	 * So, we want to call the AllocateRegister fn, put result in OutMsg if successful
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_ALLOC_REGISTER_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_ALLOC_REGISTER_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/* validate OutMsgPtr */
	if ( OutMsgPtr == NULL )
	{
		DRMDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_PR_ALLOC_REGISTER_RSP_SIZE )
	{
		// this buffer is too small to hold the request
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_PR_ALLOC_REGISTER_RSP_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	// default value in case of error
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRAllocateRegister( ReqPtr->KeyType, ClientId,
							&KeyRegisterIndex);

	// update length if we succeeded
	if ( Result == nfTGR_SUCCESS )
	{
		*MsgLengthPtr = sizeof(nfTeeDrmPRAllocateRegisterRsp_t);
		RspPtr->KeyRegisterIndex = teeHtonl(KeyRegisterIndex);
	}
#else
	(void) KeyRegisterIndex;
#endif

	return Result;

}/* end drmDispatchPRAllocateRegisterMsg */


/*********************************************************************
 * @function drmDispatchPRFreeRegisterMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_FreeRegister message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRFreeRegisterMsg( uint8_t			*InMsgPtr,
  						   	 uint32_t			InMsgBufLength,
  						   	 uint8_t			*OutMsgPtr,
  						   	 uint32_t			OutMsgBufLength,
  						   	 uint32_t			*MsgLengthPtr,
  						   	 nfTeeClientId_t	ClientId )
{
#if 0
	nfTeeDrmPRFreeRegisterReq_t	*ReqPtr = (nfTeeDrmPRFreeRegisterReq_t *)InMsgPtr;
#endif
	(void) OutMsgPtr;
	(void) OutMsgBufLength;
	(void) ClientId;

	/*
	 *
	 * nfTO_DRM_OP_PR_FREE_REGISTER request looks like this:
	 *	typedef struct
	 *	{
	 *		int		KeyType;			// OEM_HAL_KEY_TYPE f_eKeyType
	 *		long	KeyRegisterIndex;	// OEM_HAL_KEY_REGISTER_INDEX f_KeyRegisterIndex
	 *	} nfTeeDrmPRFreeRegisterReq_t;
	 *
	 * Response is empty, so we just return SUCCESS/FAIL
	 *
	 * So, we want to call the FreeRegister fn, return result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_FREE_REGISTER_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_FREE_REGISTER_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	// ignore OutMsgPtr, we are not returning any data
	*MsgLengthPtr = 0;

#if 0
	return (nfTeeResult_t) drmPRFreeRegister( teeNtohl(ReqPtr->KeyType),
											 teeNtohl(ReqPtr->KeyRegisterIndex),
											 ClientId );
#else
	// remove this once PR functions are hooked up...
	return nfTGR_SUCCESS;
#endif

}/* end drmDispatchPRFreeRegisterMsg */

/*********************************************************************
 * @function drmDispatchPRRegisterCountMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_RegisterCount message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRRegisterCountMsg( uint8_t			*InMsgPtr,
  						      uint32_t			InMsgBufLength,
  							  uint8_t			*OutMsgPtr,
  							  uint32_t			OutMsgBufLength,
  							  uint32_t			*MsgLengthPtr,
  							  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
//	nfTeeDrmPRRegisterCountReq_t	*ReqPtr = (nfTeeDrmPRRegisterCountReq_t *) InMsgPtr;
	uint32_t						TotalRegisters = 0;
	uint32_t						AllocatedRegisters = 0;

	(void) ClientId;
	(void) TotalRegisters;

	/*
	 *
	 * nfTO_DRM_OP_PR_REGISTER_COUNT request looks like this:
	 *	typedef struct
	 *	{
	 *		int		KeyType;				// OEM_HAL_KEY_TYPE f_eKeyType
	 *	} nfTeeDrmPRRegisterCountReq_t;
	 *
	 * Response looks like this:
	 *
	 *	typedef struct
	 *	{
	 *		uint32_t	TotalRegisters;		// DRM_DWORD *f_pdwTotalRegisters
	 *		uint32_t	AllocatedRegisters;	// DRM_DWORD *f_pdwAllocatedRegisters
	 *	} nfTeeDrmPRRegisterCountRsp_t;
	 *
	 * So, we want to call the RegisterCount fn, put results in OutMsg if successful
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_REGISTER_COUNT_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		CRYPTODBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_REGISTER_COUNT_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/* validate OutMsgPtr */
	if ( OutMsgPtr == NULL )
	{
		DRMDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_PR_REGISTER_COUNT_RSP_SIZE )
	{
		// this buffer is too small to hold the request
		CRYPTODBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_PR_REGISTER_COUNT_RSP_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}


	DRMDBG("%s: counting registers for keytype %d\n", __FUNCTION__, teeNtohl(ReqPtr->KeyType) );

#if 0
	Result = (nfTeeResult_t) drmPRRegisterCount( teeNtohl(ReqPtr->KeyType), ClientId,
							&TotalRegisters, &AllocatedRegisters );

	// update length if we succeeded
	if ( Result == nfTGR_SUCCESS )
	{
		DRMDBG("%s: returning %d/%d for total/allocated registers.\n", __FUNCTION__,
					TotalRegisters, AllocatedRegisters );
		*MsgLengthPtr = sizeof(nfTeeDrmPRRegisterCountRsp_t);
		RspPtr->TotalRegisters = teeHtonl(TotalRegisters);
		RspPtr->AllocatedRegisters = teeHtonl(AllocatedRegisters);
	}
	else
		*MsgLengthPtr = 0;
#else
	TotalRegisters = AllocatedRegisters = 0;
	*MsgLengthPtr = 0;
#endif

	return Result;

}/* end drmDispatchPRRegisterCountMsg */

/*********************************************************************
 * @function drmDispatchPRGetPreloadedIndexMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_GetPreloadedIndex message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRGetPreloadedIndexMsg( uint8_t			*InMsgPtr,
  						      	  uint32_t			InMsgBufLength,
  						      	  uint8_t			*OutMsgPtr,
  						      	  uint32_t			OutMsgBufLength,
  						      	  uint32_t			*MsgLengthPtr,
  						      	  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
	nfTeeDrmPRGetPreloadedIndexReq_t *ReqPtr = (nfTeeDrmPRGetPreloadedIndexReq_t *) InMsgPtr;
#if 0
	nfTeeDrmPRGetPreloadedIndexRsp_t *RspPtr = (nfTeeDrmPRGetPreloadedIndexRsp_t *) OutMsgPtr;
#endif
	int								IndexKey = 0;

	(void) ClientId;

	/*
	 *
	 * nfTO_DRM_OP_PR_REGISTER_COUNT request looks like this:
	 *	typedef struct
	 *	{
	 *		void		*OEMContextPtr;	// *f_pOEMContext
	 *		int			KeyType;	 	// f_eKeyType
	 *		void		*KeyIdPtr;	 	// *f_pbKeyId
	 *		uint32_t	KeyIdLength; 	// f_cbKeyId
	 *	} nfTeeDrmPRGetPreloadedIndexReq_t;
	 *
	 * Response looks like this:
	 *
	 *	typedef struct
	 *	{
	 *		long	IndexKey;
	 *	} nfTeeDrmPRGetPreloadedIndexRsp_t;
	 *
	 * So, we want to call the GetPreloadedIndex fn, put results in OutMsg if successful
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_GET_PRELOADED_INDEX_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_GET_PRELOADED_INDEX_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/* validate OutMsgPtr */
	if ( OutMsgPtr == NULL )
	{
		DRMDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure outmsg has enough room for response */
	if ( OutMsgBufLength <  nfMIN_PR_GET_PRELOADED_INDEX_RSP_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_PR_GET_PRELOADED_INDEX_RSP_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	// we don't allow context ptr
	if ( ReqPtr->OEMContextPtr != NULL )
	{
		DRMDBG("%s: OemContextPtr is non-null (%p), failing.\n",
					__FUNCTION__, ReqPtr->OEMContextPtr );
		return nfTDR_INVALID_OPERAND;
	}

	// validate KeyIdPtr
	if ( ReqPtr->KeyIdLength != 0 )
	{
		if ( teeNtohl(ReqPtr->KeyIdLength) > nfDRM_PR_MAX_KEY_ID_LENGTH )
		{
			DRMDBG("%s: KeyIdLength invalid (%d), failing.\n",
						__FUNCTION__, teeNtohl(ReqPtr->KeyIdLength) );
			return nfTDR_INVALID_OPERAND;
		}
		else if ( ReqPtr->KeyIdPtr == NULL )
		{
			DRMDBG("%s: KeyIdLength non-zero (%d) but pointer is NULL, failing.\n",
						__FUNCTION__, teeNtohl(ReqPtr->KeyIdLength) );
			return nfTDR_INVALID_OPERAND;
		}
		else // validate KeyIdPtr before using it!!!
		{
			/*
			 * Not sure how to do this yet, have to wait until we implement
			 * to see... make some noise so we don't forget.
			 */
			DRMDBG("\n\n\n*************************************************\n");
			DRMDBG("*************************************************\n");
			DRMDBG("*************************************************\n");
			DRMDBG("%s: XXXXXXXXXXX - VALIDATE KeyIdPtr!!!!  XXXXXXXXX\n", __FUNCTION__ );
			DRMDBG("*************************************************\n");
			DRMDBG("*************************************************\n");
			DRMDBG("*************************************************\n\n\n\n\n");
		}

	}/* end if ( ReqPtr->KeyIdLength != 0 ) */

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: getting preloaded index for keytype %d\n", __FUNCTION__, teeNtohl(ReqPtr->KeyType) );

#if 0
	Result = (nfTeeResult_t) drmPRGetPreloadedIndex( teeNtohl(ReqPtr->KeyType), ClientId,
										ReqPtr->KeyIdPtr,
										teeNtohl(ReqPtr->KeyIdLength),
										ReqPtr->OEMContextPtr,
										&IndexKey );

	// update length if we succeeded
	if ( Result == nfTGR_SUCCESS )
	{
		DRMDBG("%s: returning %d for IndexKey.\n", __FUNCTION__, IndexKey );
		*MsgLengthPtr = sizeof(nfTeeDrmPRGetPreloadedIndexRsp_t);
		RspPtr->IndexKey = teeHtonl(IndexKey);
	}
	else
		*MsgLengthPtr = 0;
#else
	(void) IndexKey;
	*MsgLengthPtr = 0;
#endif

	return Result;

}/* end drmDispatchPRGetPreloadedIndexMsg */

/*********************************************************************
 * @function drmDispatchPRVerifyMsgSignatureMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_VerifyMessageSignature message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRVerifyMsgSignatureMsg( uint8_t			*InMsgPtr,
  						       	   uint32_t			InMsgBufLength,
  						       	   uint8_t			*OutMsgPtr,
  						       	   uint32_t			OutMsgBufLength,
  						       	   uint32_t			*MsgLengthPtr,
  						       	   nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRVerifyMsgSignatureReq_t
								*ReqPtr = (nfTeeDrmPRVerifyMsgSignatureReq_t *) InMsgPtr;
#endif

	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE request looks like this:
	 *	typedef struct
	 *	{
	 *		uint8_t		*MsgPtr;
	 *		uint32_t	MsgLength; 			// NB: DRM_DWORD is unsigned long
	 *		uint8_t		*SignaturePtr;
	 *		uint32_t	SignatureLength; 	// NB: DRM_DWORD is unsigned long
	 *		int			HashType; 			// OEM_HAL_HASH_TYPE is enum
	 *		int			SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	 *		uint32_t	IndexIntegrityKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *	} nfTeeDrmPRGetPreloadedIndexReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only)
	 *
	 * So, we want to call the VerifyMessageSignature fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_VERIFY_MSG_SIG_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_VERIFY_MSG_SIG_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}


	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRVerifyMsgSignature( ClientId,
										ReqPtr->MsgPtr,
										teeNtohl(ReqPtr->MsgLength),
										ReqPtr->SignaturePtr,
										teeNtohl(ReqPtr->SignatureLength),
										teeNtohl(ReqPtr->HashType),
										teeNtohl(ReqPtr->SignatureScheme),
										teeNtohl(ReqPtr->IndexIntegrityKey) );

#endif

	return Result;

}/* end drmDispatchPRVerifyMsgSignatureMsg */

/*********************************************************************
 * @function drmDispatchPRSignWithPrivKeyMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_SignWithPrivateKey message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRSignWithPrivKeyMsg( uint8_t			*InMsgPtr,
  						   	   	uint32_t		InMsgBufLength,
  						       	uint8_t			*OutMsgPtr,
  						       	uint32_t		OutMsgBufLength,
  						       	uint32_t		*MsgLengthPtr,
  						       	nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRSignWithPrivKeyReq_t 	*ReqPtr = (nfTeeDrmPRSignWithPrivKeyReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY request looks like this:
	 *	typedef struct
	 *	{
	 *		uint8_t		*OemContextPtr;		// DRM_VOID *
	 *		uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	 *		uint8_t		*MsgPtr;			// DRM_BYTE *
	 *		uint32_t	MsgLength; 			// NB: DRM_DWORD is unsigned long
	 *		uint8_t		*SignaturePtr;		// DRM_BYTE *
	 *		uint32_t	SignatureLength; 	// NB: DRM_DWORD is unsigned long
	 *		int			HashType; 			// OEM_HAL_HASH_TYPE is enum
	 *		int			SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexIntegrityKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *	} nfTeeDrmPRSignWithPrivKeyReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only)
	 *
	 * So, we want to call the SignWithPrivKey fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_SIGN_WPRIV_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_SIGN_WPRIV_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}


	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");
	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0

	Result = (nfTeeResult_t) drmPRSignWithPrivKey( ClientId,
										ReqPtr->OemContextPtr,
										ReqPtr->CryptoContextPtr,
										ReqPtr->MsgPtr,
										teeNtohl(ReqPtr->MsgLength),
										ReqPtr->SignaturePtr,
										teeNtohl(ReqPtr->SignatureLength),
										teeNtohl(ReqPtr->HashType),
										teeNtohl(ReqPtr->SignatureScheme),
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexIntegrityKey) );

#endif

	return Result;

}/* end drmDispatchPRSignWithPrivKeyMsg */

/*********************************************************************
 * @function drmDispatchPRVerifyOmac1SignatureMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_VerifyOMAC1Signature message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRVerifyOmac1SignatureMsg( uint8_t			*InMsgPtr,
  						       	     uint32_t			InMsgBufLength,
  						       	     uint8_t			*OutMsgPtr,
  						       	     uint32_t			OutMsgBufLength,
  						       	     uint32_t			*MsgLengthPtr,
  						       	     nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRVerifyOmac1SignatureReq_t
								*ReqPtr = (nfTeeDrmPRVerifyOmac1SignatureReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE request looks like this:
	 *	typedef struct
	 *	{
	 *		uint8_t			*DataPtr;			// DRM_BYTE *
	 *		uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		uint8_t			*SignaturePtr;
	 *		uint32_t		SignatureLength; 	// NB: DRM_DWORD is unsigned long
	 *		int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	 *		int				KeyType;	 		// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t		IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *	} nfTeeDrmPRVerifyOmac1SignatureReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only)
	 *
	 * So, we want to call the VerifyOmac1Signature fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_VERIFY_OMAC1_SIG_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_VERIFY_OMAC1_SIG_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before verifying!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRVerifyOmac1Signature( ClientId,
										ReqPtr->DataPtr,
										teeNtohl(ReqPtr->DataLength),
										ReqPtr->SignaturePtr,
										teeNtohl(ReqPtr->SignatureLength),
										teeNtohl(ReqPtr->SignatureScheme),
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey) );

#endif

	return Result;

}/* end drmDispatchPRVerifyOmac1SignatureMsg */

/*********************************************************************
 * @function drmDispatchPRCreateOmac1SignatureMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_CreateOMAC1Signature message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRCreateOmac1SignatureMsg( uint8_t			*InMsgPtr,
  						       	     uint32_t			InMsgBufLength,
  						       	     uint8_t			*OutMsgPtr,
  						       	     uint32_t			OutMsgBufLength,
  						       	     uint32_t			*MsgLengthPtr,
  						       	     nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRCreateOmac1SignatureReq_t
								*ReqPtr = (nfTeeDrmPRCreateOmac1SignatureReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE request looks like this:
	 *	typedef struct
	 *	{
	 *		uint8_t			*DataPtr;			// DRM_BYTE *
	 *		uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		uint8_t			*SignaturePtr;
	 *		uint32_t		*SignatureLengthPtr; 	// NB: DRM_DWORD is unsigned long
	 *		int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	 *		uint32_t		IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *	} nfTeeDrmPRCreateOmac1SignatureReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, sig is stored at passed pointer)
	 *
	 * So, we want to call the CreateOmac1Signature fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_VERIFY_OMAC1_SIG_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_VERIFY_OMAC1_SIG_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRCreateOmac1Signature( ClientId,
										ReqPtr->DataPtr,
										teeNtohl(ReqPtr->DataLength),
										ReqPtr->SignaturePtr,
										teeNtohl(ReqPtr->SignatureLength),
										teeNtohl(ReqPtr->SignatureScheme),
										teeNtohl(ReqPtr->IndexKey) );

#endif

	return Result;

}/* end drmDispatchPRCreateOmac1SignatureMsg */

/*********************************************************************
 * @function drmDispatchPREncryptDataBlockMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_EncryptDataBlock message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPREncryptDataBlockMsg( uint8_t			*InMsgPtr,
  						         uint32_t			InMsgBufLength,
  						         uint8_t			*OutMsgPtr,
  						         uint32_t			OutMsgBufLength,
  						         uint32_t			*MsgLengthPtr,
  						         nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPREncryptDataBlockReq_t
								*ReqPtr = (nfTeeDrmPREncryptDataBlockReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t			*DataPtr;			// DRM_BYTE *
	 *		uint32_t		DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		uint32_t		*CiphertextLengthPtr; 	// NB: DRM_DWORD is unsigned long
	 *		uint8_t			*CiphertextPtr;
	 *		int				SignatureScheme; 	// OEM_HAL_SIGNATURE_TYPE is enum
	 *		int				KeyType;	 		// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t		IndexMsgEncryptKey; // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *	} nfTeeDrmPREncryptDataBlockReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, sig is stored at passed pointer)
	 *
	 * So, we want to call EncryptDataBlock fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_ENCR_DATA_BLOCK_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_ENCR_DATA_BLOCK_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPREncryptDataBlock( ClientId,
										ReqPtr->DataPtr,
										teeNtohl(ReqPtr->DataLength),
										ReqPtr->CiphertextPtr,
										ReqPtr->CiphertextLengthPtr,
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexMsgEncryptKey) );
#endif

	return Result;

}/* end drmDispatchPREncryptDataBlockMsg */

/*********************************************************************
 * @function drmDispatchPRUnwrapKeyMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_UnwrapKey message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRUnwrapKeyMsg( uint8_t			*InMsgPtr,
  						   uint32_t			InMsgBufLength,
  						   uint8_t			*OutMsgPtr,
  						   uint32_t			OutMsgBufLength,
  						   uint32_t			*MsgLengthPtr,
  						   nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRUnwrapKeyReq_t	*ReqPtr = (nfTeeDrmPRUnwrapKeyReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_UNWRAP_KEY request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		int			WrapKeyType;  		// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexWrapKey;  		// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint8_t		*WrappedKeyDataPtr;	// DRM_BYTE *
	 *		uint32_t	WrappedKeyDataLength;// NB: DRM_DWORD is unsigned long
	 *		uint8_t		*ParamaterDataPtr;	// DRM_BYTE *
	 *		uint32_t	ParamaterDataLength;// NB: DRM_DWORD is unsigned long
	 *	} nfTeeDrmPRUnwrapKeyReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at crypto context pointer)
	 *
	 * So, we want to call unwrapKey fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_UNWRAP_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_UNWRAP_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRUnwrapKey( ClientId,
										ReqPtr->CryptoContextPtr,
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										teeNtohl(ReqPtr->WrapKeyType),
										teeNtohl(ReqPtr->IndexWrapKey),
										ReqPtr->WrappedKeyDataPtr,
										teeNtohl(ReqPtr->WrappedKeyDataLength),
										ReqPtr->ParamaterDataPtr,
										ReqPtr->ParameterDataLength );
#endif

	return Result;

}/* end drmDispatchUnwrapKeyMsg */

/*********************************************************************
 * @function drmDispatchPRWrapKeyMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_WrapKey message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRWrapKeyMsg( uint8_t			*InMsgPtr,
  					   	 uint32_t			InMsgBufLength,
  						 uint8_t			*OutMsgPtr,
  						 uint32_t			OutMsgBufLength,
  						 uint32_t			*MsgLengthPtr,
  						 nfTeeClientId_t	ClientId )
{
	nfTeeResult_t			Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRWrapKeyReq_t	*ReqPtr = (nfTeeDrmPRWrapKeyReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_WRAP_KEY request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*OemContextPtr;     // DRM_VOID *
	 *		uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		int			WrapKeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexWrapKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint8_t		*WrappedKeyDataPtr;			// DRM_BYTE *
	 *		uint32_t	*WrappedKeyDataLengthPtr; 			// NB: DRM_DWORD is unsigned long
	 *	} nfTeeDrmPRWrapKeyReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at WrappedKeyDataPtr )
	 *
	 * So, we want to call WrapKey fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_WRAP_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_WRAP_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRWrapKey( ClientId,
										ReqPtr->OemContextPtr,
										ReqPtr->CryptoContextPtr,
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										teeNtohl(ReqPtr->WrapKeyType),
										teeNtohl(ReqPtr->IndexWrapKey),
										ReqPtr->WrappedKeyDataPtr,
										ReqPtr->WrappedKeyDataLengthPtr );
#endif

	return Result;

}/* end drmDispatchWrapKeyMsg */

/*********************************************************************
 * @function drmDispatchPRGenerateKeyMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_GenerateKey message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRGenerateKeyMsg( uint8_t			*InMsgPtr,
  					   	 	 uint32_t			InMsgBufLength,
  					   	 	 uint8_t			*OutMsgPtr,
  					   	 	 uint32_t			OutMsgBufLength,
  					   	 	 uint32_t			*MsgLengthPtr,
  					   	 	 nfTeeClientId_t	ClientId )
{
	nfTeeResult_t				Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRGenerateKeyReq_t	*ReqPtr = (nfTeeDrmPRGenerateKeyReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_GENERATE_KEY request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*OemContextPtr;     // DRM_VOID *
	 *		uint8_t		*CryptoContextPtr;	// DRM_CRYPTO_CONTEXT *
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexKey;  			// OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint32_t	RequestedSecurityLevel; // DRM_DWORD
	 *	} nfTeeDrmPRGenerateKeyReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at WrappedKeyDataPtr )
	 *
	 * So, we want to call WrapKey fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_GENERATE_KEY_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_GENERATE_KEY_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: validating message signature\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRWrapKey( ClientId,
										ReqPtr->OemContextPtr,
										ReqPtr->CryptoContextPtr,
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										teeNtohl(ReqPtr->RequestedSecurityLevel) );
#endif

	return Result;

}/* end drmDispatchGenerateKeyMsg */

/*********************************************************************
 * @function drmDispatchPRDecryptContentMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_DecryptContent message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRDecryptContentMsg( uint8_t			*InMsgPtr,
  					   	 	 	uint32_t		InMsgBufLength,
								uint8_t			*OutMsgPtr,
								uint32_t		OutMsgBufLength,
								uint32_t		*MsgLengthPtr,
								nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRDecryptContentReq_t	*ReqPtr = (nfTeeDrmPRDecryptContentReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_DECRYPT_CONTENT request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*DataPtr;			// DRM_BYTE *
	 *		uint32_t	DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexContentKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint64_t	InitializationVector;
	 *		uint64_t	SampleByteOffset;
	 *	} nfTeeDrmPRDecryptContentReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at WrappedKeyDataPtr )
	 *
	 * So, we want to call DecryptContent fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_DECRYPT_CONTENT_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_DECRYPT_CONTENT_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: decrypting content\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRDecryptContent( ClientId,
										ReqPtr->DataPtr,
										teeNtohl(ReqPtr->DataLength),
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										ReqPtr->InitializationVector,
										ReqPtr->SampleByteOffset );
#endif

	return Result;

}/* end drmDispatchDecryptContentMsg */

/*********************************************************************
 * @function drmDispatchPRDecryptContentOpaqueMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_DecryptContentOpaque message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchPRDecryptContentOpaqueMsg( uint8_t			*InMsgPtr,
									  uint32_t		InMsgBufLength,
									  uint8_t			*OutMsgPtr,
									  uint32_t		OutMsgBufLength,
									  uint32_t		*MsgLengthPtr,
									  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRDecryptContentOpaqueReq_t	*ReqPtr = (nfTeeDrmPRDecryptContentOpaqueReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_DECRYPT_CONTENT request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*InBufPtr;			// DRM_VOID *
	 *		uint8_t		*OutBufPtr;			// DRM_VOID *
	 *		uint32_t	DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexContentKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint64_t	InitializationVector;
	 *		uint64_t	SampleByteOffset;
	 *	} nfTeeDrmPRDecryptContentReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at OutBufPtr )
	 *
	 * So, we want to call DecryptContentOpaque fn, return Result
	 */

	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_DECRYPT_CONTENT_OPAQUE_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_DECRYPT_CONTENT_OPAQUE_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: decrypting content\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRDecryptContentOpaque( ClientId,
										ReqPtr->InBufPtr,
										ReqPtr->OutBufPtr,
										teeNtohl(ReqPtr->DataLength),
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										ReqPtr->InitializationVector,
										ReqPtr->SampleByteOffset );
#endif

	return Result;

}/* end drmDispatchDecryptContentOpaqueMsg */

/*********************************************************************
 * @function drmDispatchCreateSampleEncryptCtxtMsg
 *
 * @brief dispatch function for PlayReady Oem_Hal_CreateSampleEncryptionContext message
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[out] MsgLengthPtr - pointer to where you want msg length stored
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if okay,
 * 		   nfTDR_INTERNAL_ERROR || nfTDR_OPERAND_TOO_SHORT otherwise
 *
 *********************************************************************/
nfTeeResult_t
drmDispatchCreateSampleEncryptCtxtMsg( uint8_t			*InMsgPtr,
									  uint32_t		InMsgBufLength,
									  uint8_t			*OutMsgPtr,
									  uint32_t		OutMsgBufLength,
									  uint32_t		*MsgLengthPtr,
									  nfTeeClientId_t	ClientId )
{
	nfTeeResult_t					Result = nfTGR_SUCCESS;
#if 0
	nfTeeDrmPRDecryptContentOpaqueReq_t	*ReqPtr = (nfTeeDrmPRDecryptContentOpaqueReq_t *) InMsgPtr;
#endif
	(void) ClientId;
	(void) OutMsgPtr;
	(void) OutMsgBufLength;

	/*
	 *
	 * nfTO_DRM_OP_PR_DECRYPT_CONTENT request looks like this:
	 *	typedef struct`
	 *	{
	 *		uint8_t		*InBufPtr;			// DRM_VOID *
	 *		uint8_t		*OutBufPtr;			// DRM_VOID *
	 *		uint32_t	DataLength; 		// NB: DRM_DWORD is unsigned long
	 *		int			KeyType;  			// OEM_HAL_KEY_TYPE is enum
	 *		uint32_t	IndexContentKey;  // OEM_HAL_KEY_REGISTER_INDEX is DRM_LONG
	 *		uint64_t	InitializationVector;
	 *		uint64_t	SampleByteOffset;
	 *	} nfTeeDrmPRDecryptContentReq_t;
	 *
	 * Response looks like this:
	 *
	 * (ResponseCode only, key is stored at OutBufPtr )
	 *
	 * So, we want to call DecryptContentOpaque fn, return Result
	 */
	/*
	 * First, validate message pointers, lengths
	 */
	/* first, validate InMsgPtr */
	if ( InMsgPtr == NULL )
	{
		DRMDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return (nfTeeResult_t) nfTDR_INTERNAL_ERROR;
	}

	/* make sure inmsg has enough room for data we expect */
	if ( InMsgBufLength <  nfMIN_PR_DECRYPT_CONTENT_OPAQUE_REQ_SIZE )
	{
		// this buffer is too small to hold the request
		DRMDBG( "%s: InMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_PR_DECRYPT_CONTENT_OPAQUE_REQ_SIZE );
		return (nfTeeResult_t) nfTDR_OPERAND_TOO_SHORT;
	}

	/*
	 * XXX
	 * Next, pointer validation. We have to assume the REE is hostile and
	 * that these pointers may be bogus.
	 *
	 * Until we get further into implementation, I don't know how to check
	 * these. For now, make a bunch of noise so we don't forget.
	 */
	DRMDBG("\n\n\n*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("%s: XXXXXXXXXXX - VALIDATE pointers before signing!!!!  XXXXXXXXX\n", __FUNCTION__ );
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n");
	DRMDBG("*************************************************\n\n\n\n\n");

	/*
	 * Okay, message looks valid, do the deed.
	 */
	DRMDBG("%s: decrypting content\n", __FUNCTION__ );

	// no matter what, message length is 0
	*MsgLengthPtr = 0;

#if 0
	Result = (nfTeeResult_t) drmPRDecryptContentOpaque( ClientId,
										ReqPtr->InBufPtr,
										ReqPtr->OutBufPtr,
										teeNtohl(ReqPtr->DataLength),
										teeNtohl(ReqPtr->KeyType),
										teeNtohl(ReqPtr->IndexKey),
										ReqPtr->InitializationVector,
										ReqPtr->SampleByteOffset );
#endif

	return Result;

}/* end drmDispatchCreateSampleEncryptCtxtMsg */

/*********************************************************************
 * @function drmAgentDispatchMsg
 *
 * @brief drm Agent message handler/entry point
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
nfTeeResult_t drmAgentDispatchMsg( nfTeeOperation_t	Operation,
								   uint8_t			*InMsgPtr,
								   uint32_t			InMsgBufLength,
								   uint8_t			*OutMsgPtr,
								   uint32_t			OutMsgBufLength,
								   uint32_t			*MsgLengthPtr,
								   nfTeeClientId_t	ClientId )
{
	nfTeeResult_t Result = 0;

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
		case nfTO_DRM_OP_PR_ALLOCATE_REGISTER:
			Result = drmDispatchPRAllocateRegisterMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_FREE_REGISTER:
			Result = drmDispatchPRFreeRegisterMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_REGISTER_COUNT:
			Result = drmDispatchPRRegisterCountMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_GET_PRELOADED_INDEX:
			Result = drmDispatchPRGetPreloadedIndexMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_INITIALIZE:
		case nfTO_DRM_OP_PR_DEINITIALIZE:
			// these are no-ops for now...
			DRMDBG("%s: short-circuiting init/de-init messages.\n", __FUNCTION__ );
			*MsgLengthPtr = 0;
			Result = nfTGR_SUCCESS;
			break;
		case nfTO_DRM_OP_PR_VERIFY_MSG_SIGNATURE:
			Result = drmDispatchPRVerifyMsgSignatureMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_SIGN_WITH_PRIVATE_KEY:
			Result = drmDispatchPRSignWithPrivKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_VERIFY_OMAC1_SIGNATURE:
			Result = drmDispatchPRVerifyOmac1SignatureMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_CREATE_OMAC1_SIGNATURE:
			Result = drmDispatchPRCreateOmac1SignatureMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_ENCRYPT_DATA_BLOCK:
			Result = drmDispatchPREncryptDataBlockMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_UNWRAP_KEY:
			Result = drmDispatchPRUnwrapKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_WRAP_KEY:
			Result = drmDispatchPRWrapKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_GENERATE_KEY:
			Result = drmDispatchPRGenerateKeyMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_DECRYPT_CONTENT:
			Result = drmDispatchPRDecryptContentMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_DECRYPT_CONTENT_OPAQUE:
			Result = drmDispatchPRDecryptContentOpaqueMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_CREATE_SAMPLE_ENCRYPTION_CONTEXT:
			Result = drmDispatchCreateSampleEncryptCtxtMsg(InMsgPtr, InMsgBufLength, OutMsgPtr,
					OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_DRM_OP_PR_ENCRYPT_SAMPLE_DATA:
		case nfTO_DRM_OP_PR_QUERY_SAMPLE_METADATA:
		case nfTO_DRM_OP_PR_DESTROY_SAMPLE_ENCRYPTION_CONTEXT:
		case nfTO_DRM_OP_PR_LOAD_PLAYREADY_REVOCATION_INFO:
		case nfTO_DRM_OP_PR_LOAD_PLAYREADY_CRL:
		default:
			/*fprintf( stderr, "%s: unimplemented operation (%08x)\n",
					 __FUNCTION__, Operation );*/
			Result = nfTGR_INTERNAL_ERROR;
			break;
	}/* end switch ( Operation ) */

	return Result;

}/* end drmAgentDispatchMsg */

// vim: expandtab tabstop=4 shiftwidth=4


