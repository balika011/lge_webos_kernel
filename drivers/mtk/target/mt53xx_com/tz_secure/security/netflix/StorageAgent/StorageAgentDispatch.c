/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommonIncludes.h"
#include "CommTypes.h"
#include "StorageTypes.h"
#include "StorageAgent.h"
#include "StorageAgentInternal.h"
#include "StorageAgentTPI.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"

/*********************************************************************
 * @function saDispatchUnprotectMsg
 *
 * @brief Storage Agent message handler for unprotect requests
 *
 * @param[in] InMsgPtr - pointer to incoming unprotect request
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *d
 *********************************************************************/
static
nfTeeResult_t saDispatchUnprotectMsg( uint8_t 			*InMsgPtr,
							 	   	  uint32_t 			InMsgBufLength,
							 	   	  uint8_t 			*OutMsgPtr,
							 	   	  uint32_t 			OutMsgBufLength,
							 	   	  uint32_t			*MsgLengthPtr,
							 	   	  nfTeeClientId_t 	ClientId )
{

	uint32_t 					ResponseCode = (uint32_t)nfTGR_SUCCESS;
	uint32_t					DataLength = 0;
	uint32_t					OutDataLength = 0;
	uint32_t					MaxLength;
	nfTeeStorageUnprotectReq_t	*ReqPtr;

	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_STORAGE_UNPROTECT_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG( "%s: InMsgBufLength is %d, expected size is larger than %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_STORAGE_UNPROTECT_REQ_SIZE);
		return nfTSR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_STORAGE_UNPROTECT_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_STORAGE_UNPROTECT_RSP_SIZE);
		return nfTSR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	/*
	 * Response looks like this:
	 * typedef struct
	 *	{
	 *		uint32_t		DataLength;
	 *		uint8_t			Data[];
	 *	} nfTeeStorageUnprotectRsp_t;
	 *
	 */

	// compute maximum buffer length
	MaxLength = OutMsgBufLength - sizeof(uint32_t);
#if 0
	STORAGEDBG("%s: dumping inbound protect request...\n", __FUNCTION__ );
	teePrintHexDump(InMsgPtr, InMsgBufLength);
#endif
	// get pointer to request data
	ReqPtr = (nfTeeStorageUnprotectReq_t *)InMsgPtr;
	DataLength = teeNtohl(ReqPtr->ProtectedDataLength);

	// the only thing allowed after ReqPtr->DataLength is DataLength bytes
	if ( DataLength != (InMsgBufLength - sizeof(uint32_t)) )
	{
		// NRDAPP-3239 fix
		STORAGEDBG("%s: message header length mismatch (%d/%d)\n",
					__FUNCTION__, DataLength, InMsgBufLength - sizeof(uint32_t) );
		return nfTGR_INMSG_INVALID;
	}
	// compute maximum buffer length
	MaxLength = OutMsgBufLength - sizeof(uint32_t);

	// unprotect the data, put result in OutMsg
//	STORAGEDBG("%s: calling saUnprotect with following data: \n", __FUNCTION__);
//	teePrintHexDump(ReqPtr->ProtectedData, DataLength);

	ResponseCode = saTpiUnprotectData( ClientId,
									ReqPtr->ProtectedData,
									DataLength,
									((nfTeeStorageUnprotectRsp_t *)OutMsgPtr)->Data,
									MaxLength,
									&OutDataLength );


	STORAGEDBG("%s: saUnprotectData returned %08x (DataLength: %d)\n", __FUNCTION__, ResponseCode, DataLength);

	// if successful, fix up read response header, set ResponseLength
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		((nfTeeStorageUnprotectRsp_t *)OutMsgPtr)->DataLength = teeHtonl(OutDataLength);
		*MsgLengthPtr = OutDataLength + sizeof(uint32_t);
	}

	STORAGEDBG("%s: ResponseCode: %08x; ResponseLength: %d\n", __FUNCTION__, ResponseCode,
					*MsgLengthPtr );

#if 0
	// unprotect the data, put result in OutMsg
	STORAGEDBG("%s: dumping OutMsg \n", __FUNCTION__);
	teePrintHexDump(OutMsgPtr, *MsgLengthPtr );
#endif

	// okay, we're good.
	return ResponseCode;

}/* end saDispatchUnprotectMsg */

/*********************************************************************
 * @function saDispatchProtectMsg
 *
 * @brief Storage Agent message handler for protect requests
 *
 * @param[in] InMsgPtr - pointer to incoming protect request
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *d
 *********************************************************************/
static
nfTeeResult_t saDispatchProtectMsg( uint8_t 			*InMsgPtr,
							 	 	uint32_t 			InMsgBufLength,
							 	 	uint8_t 			*OutMsgPtr,
							 	 	uint32_t 			OutMsgBufLength,
							 	 	uint32_t			*MsgLengthPtr,
							 	 	nfTeeClientId_t 	ClientId )
{
	nfTeeResult_t		ResponseCode = (uint32_t)nfTGR_SUCCESS;
	uint32_t			DataLength = 0;
	uint32_t			MaxLength;

	/*
	 *	nfTeeStorageUnprotectReq_t has the following fields:
	 *
	 * 	uint32_t		ProtectedDataLength;
	 * 	uint8_t			ProtectedData[];
	 *
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key lookup function. Then, we need to call the lookup function
	 *  and construct the response message in *OutMsgPtr
	 */

	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength < nfMIN_STORAGE_PROTECT_REQ_SIZE )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG( "%s: InMsgBufLength is %d, expected size is larger than %d\n",
				__FUNCTION__, InMsgBufLength, nfMIN_STORAGE_PROTECT_REQ_SIZE);
		return nfTSR_OPERAND_TOO_SHORT;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_STORAGE_PROTECT_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_STORAGE_PROTECT_RSP_SIZE);
		return nfTSR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	// the only thing allowed after ReqPtr->DataLength is DataLength bytes
	DataLength = teeNtohl(((nfTeeStorageProtectReq_t *)InMsgPtr)->DataLength);
	if ( DataLength != (InMsgBufLength - sizeof(uint32_t)) )
	{
		// NRDAPP-3239 fix
		STORAGEDBG("%s: message header length mismatch (%d/%d)\n",
					__FUNCTION__, DataLength, InMsgBufLength - sizeof(uint32_t) );
		return nfTGR_INMSG_INVALID;
	}
	/*
	 * Response looks like this:
	 * typedef struct
	 *	{
	 *		uint32_t		ProtectedDataLength;
	 *		uint8_t			ProtectedData[];
	 *	} nfTeeStorageProtectRsp_t;
	 *
	 * We need to give the lookup function a pointer to RspPtr->Data. We'll fill
	 * in DataLength.
	 */

	// compute maximum buffer length
	MaxLength = OutMsgBufLength - sizeof(uint32_t);
#if 0
	STORAGEDBG("%s: dumping inbound protect request...\n", __FUNCTION__ );
	teePrintHexDump(InMsgPtr, InMsgBufLength);
#endif
	ResponseCode = saTpiProtectData( ClientId, ((nfTeeStorageProtectReq_t *)InMsgPtr)->Data,
								 teeNtohl(((nfTeeStorageProtectReq_t *)InMsgPtr)->DataLength),
								 ((nfTeeStorageProtectRsp_t *)OutMsgPtr)->ProtectedData,
								 MaxLength, &DataLength );

	STORAGEDBG("%s: saProtectData returned %08x\n", __FUNCTION__, ResponseCode);

	// if successful, fix up read response header, set ResponseLength
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		*MsgLengthPtr = DataLength + sizeof(uint32_t);
		((nfTeeStorageProtectRsp_t *)OutMsgPtr)->ProtectedDataLength = teeHtonl(DataLength);
	}

	STORAGEDBG("%s: ResponseCode: %08x; ResponseLength: %d\n", __FUNCTION__, ResponseCode,
					DataLength );

	// okay, we're good.
	return ResponseCode;

}/* end saDispatchProtectMsg */

/*********************************************************************
 * @function saDispatchSimpleReadMsg
 *
 * @brief Stdorage Agent message handler for read requests
 *
 * @param[in] InMsgPtr - pointer to incoming read request
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return nfTGR_SUCCESS if everything okay, appropriate error otherwise
 *d
 *********************************************************************/
static
nfTeeResult_t saDispatchSimpleReadMsg( uint8_t 			*InMsgPtr,
							 	 	uint32_t 			InMsgBufLength,
							 	 	uint8_t 			*OutMsgPtr,
							 	 	uint32_t 			OutMsgBufLength,
							 	 	uint32_t			*MsgLengthPtr,
							 	 	nfTeeClientId_t 	ClientId )
{

	uint32_t 					ResponseCode = (uint32_t)nfTGR_SUCCESS;
	uint8_t						*DataPtr;
	uint32_t					DataLength = 0;
	uint32_t					MaxLength;

	/*
	 *	nfTeeStorageSimpleReadReq_t has the following fields:
	 *
	 *  - Key
	 *
	 *  We need to do first-level sanity checks on the data before we call
	 *  the key lookup function. Then, we need to call the lookup function
	 *  and construct the response message in *OutMsgPtr
	 */

	*MsgLengthPtr = 0;

	/* first, validate message length */
	if ( InMsgBufLength != nfMIN_SIMPLE_READ_REQ_SIZE )
	{
		if ( InMsgBufLength < nfMIN_SIMPLE_READ_REQ_SIZE )
		{
				// this buffer is too small to hold a valid message
			STORAGEDBG( "%s: InMsgBufLength is %d, expected size is larger than %d\n",
					__FUNCTION__, InMsgBufLength, nfMIN_SIMPLE_READ_REQ_SIZE);
			return nfTSR_OPERAND_TOO_SHORT;
		}
		// else it's not the right size
		return nfTSR_OPERAND_TOO_LONG;
	}

	if ( InMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	if ( OutMsgBufLength < nfMIN_SIMPLE_READ_RSP_SIZE )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgBufLength is %d, expected size is at least %d\n",
				__FUNCTION__, OutMsgBufLength, nfMIN_SIMPLE_READ_RSP_SIZE);
		return nfTSR_OPERAND_TOO_SHORT;
	}

	if ( OutMsgPtr == NULL )
	{
		// this buffer is too small to hold a valid message
		STORAGEDBG("%s: OutMsgPtr is NULL?!\n", __FUNCTION__);
		return nfTSR_INTERNAL_ERROR;
	}

	/*
	 * Simple read request looks like this:
	 *	typedef struct
	 *	{
	 *		nfSecureStoreClientKey_t	RequestedKey;
	 *	} nfTeeStorageSimpleReadReq_t;
	 *
	 * Get the RequestedKey from the message
	 */
	/*
	 * Response looks like this:
	 * typedef struct
	 *	{
	 *		nfClientId_t	ClientId;
     *		uint32_t	DataLength;
	 *		uint8_t		Data[];
	 *	} nfTeeStorageSimpleReadRsp_t;
	 *
	 * We need to give the lookup function a pointer to RspPtr->Data. We'll fill
	 * in DataLength.
	 */

	// Get pointer beyond response header in OutMsg
	DataPtr = OutMsgPtr + sizeof(uint32_t) + sizeof(nfTeeClientId_t);

	// compute maximum buffer length
	MaxLength = OutMsgBufLength -  sizeof(uint32_t) - sizeof(nfTeeClientId_t);;
#if 0
	STORAGEDBG("%s: dumping inbound read request...\n", __FUNCTION__ );
	teePrintHexDump(InMsgPtr, 64);
#endif
	ResponseCode = saLookupKey( ClientId,
			teeNtohl(((nfTeeStorageSimpleReadReq_t *)InMsgPtr)->RequestedKey),
								DataPtr, &DataLength, MaxLength );

	// no matter what, put response in OutMsg
	((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(ResponseCode);

	// if successful, fix up read response header, set ResponseLength
	if ( ResponseCode == nfTGR_SUCCESS )
	{
		nfTeeStorageSimpleReadRsp_t *RspPtr
				= (nfTeeStorageSimpleReadRsp_t *)OutMsgPtr;

		// copy clientId into message
		(void) teeMemcpy( RspPtr->ClientId, ClientId, sizeof(nfTeeClientId_t) );

		RspPtr->DataLength = teeHtonl(DataLength);
		*MsgLengthPtr = DataLength + sizeof(nfTeeStorageSimpleReadRsp_t);
	}

	// okay, we're good.
	return ResponseCode;

}/* end saDispatchSimpleReadMsg */

/*********************************************************************
 * @function storageAgentDispatchMsg
 *
 * @brief Storage Agent message handler/entry point
 *
 * @param[in] Operation - operation requested by client
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 * @param[in] ClientId - ID for REE client who made request
 *
 * @return void
 *
 *********************************************************************/
nfTeeResult_t storageAgentDispatchMsg( nfTeeOperation_t	Operation,
							 	 	   uint8_t 			*InMsgPtr,
							 	 	   uint32_t 		InMsgBufLength,
							 	 	   uint8_t 			*OutMsgPtr,
							 	 	   uint32_t 		OutMsgBufLength,
							 	 	   uint32_t			*MsgLengthPtr,
							 	 	   nfTeeClientId_t 	ClientId )
{
	nfTeeResult_t	Result = 0;

	switch ( Operation )
	{
		case nfTO_STORAGE_OP_SIMPLE_READ:
			Result = saDispatchSimpleReadMsg( InMsgPtr, InMsgBufLength,
											 OutMsgPtr, OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_STORAGE_OP_PROTECT:
			Result = saDispatchProtectMsg( InMsgPtr, InMsgBufLength,
											 OutMsgPtr, OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_STORAGE_OP_UNPROTECT:
			Result = saDispatchUnprotectMsg( InMsgPtr, InMsgBufLength,
											 OutMsgPtr, OutMsgBufLength, MsgLengthPtr, ClientId );
			break;
		case nfTO_STORAGE_OP_COMPOUND_READ:
		default:
			STORAGEDBG( "%s: unimplemented operation (%08x)\n",
					 __FUNCTION__, Operation );
			Result = nfTGR_INTERNAL_ERROR;
			break;
	}/* end switch ( Operation ) */

	STORAGEDBG("%s: returning %08x\n", __FUNCTION__, Result);

	return Result;

}/* end storageAgentDispatchMsg */

// vim: expandtab tabstop=4 shiftwidth=4
