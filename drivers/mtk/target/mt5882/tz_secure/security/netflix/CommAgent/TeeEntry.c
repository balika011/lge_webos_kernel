/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/*
 *      This file implements the TEE entry point(s). Right now, there is exactly
 *      one functions exposed to the outside world:
 *
 *      - nfTeeProcMsg(): you communicate with the TEE by passing it a message, and waiting
 *        for the response. This is the function you call to do that.
 */

#include "TeeTypes.h"
#include "CommTypes.h"
#include "CryptoTypes.h"
#include "StorageTypes.h"
#include "MgmtTypes.h"
#include "DrmTypes.h"
#include "MgmtAgent.h"
#include "DrmAgent.h"
#include "StorageAgent.h"
#include "CryptoAgent.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "tz.h"
#include "tzst.h"

#ifdef TEE_ULI_CEM_MG
#define TEE_NFLX_KPE_SIZE 	16
#define TEE_NFLX_KPH_SIZE	32
extern uint8_t au1_TEE_NFLX_kpe[];
extern uint8_t au1_TEE_NFLX_kph[];
#endif


uint8_t IDFILE_BUF[nfIDFLIE_MAX_FILE_SIZE];
uint32_t idFile_len = 0;


/*
 * XXX - hard-coding NRD client ID for this library TEE implementation; this is
 * defined in the manufacturing secure store, and in StorageAgentInternals.h
 */
static nfTeeClientId_t NrdClientId =
	{0x15,0xe1,0x25,0xe9,0x11,0x00,0xF1,0x99,0x03,0x33,0xd1,0xfd,0x90,0x87,0xbb,0x40};

/*
 * Comm agent needs reference addresses in order to validate pointers in
 * messages. These values should be set during initialization.
 */
// XXX - for now, we are setting these to dummy values for the ref app, but this must be fixed.
static uint8_t *caSharedMemoryBaseAddr = (uint8_t *)0x00000000;
static uint8_t *caSharedMemoryMaxAddr = (uint8_t *) 0xFFFFFFFF;

/*********************************************************************
 * @function caValidateInboundMsgPtr
 *
 * @brief verify that passed pointer is within allowed range for incoming msg
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgLength - length of incoming message
 *
 * @return nfTGR_SUCCESS if okay, nfTGR_INMSG_INVALID otherwise
 *
 * NB: need to look at optimization for this function. This should
 *     probably be inline-able.
 *
 *********************************************************************/
nfTeeResult_t caValidateInboundMsgPtr( uint8_t 	*InMsgPtr,
									   uint32_t	InMsgLength )
{
	/*
	 * We want to ensure the following:
	 *
	 * 	- message pointer is within allowable range
	 * 		- Not in TEE memory
	 * 		- Not overlapping with InMsg
	 * 		- Not in MMIO (or other inappropriate) memory range
	 *
	 * 	- length does not cause message to exceed allowable range
	 *
	 * 	Because this is an x86 reference implementation, we know that
	 * 	InMsgPtr and OutMsgPtr are on the heap. We could add checks here
	 * 	to ensure that this is the case (to illustrate best practices),
	 * 	but this would be an academic exercise.
	 *
	 * 	What you check here will vary, depending on your implementation.
	 * 	The important thing to keep in mind is that the REE may be
	 * 	compromised, so you cannot trust these pointers or lengths. You
	 * 	need to check EVERY TIME to make sure they are "reasonable".
	 */

	if ( ( InMsgPtr > caSharedMemoryBaseAddr )
	  && ( InMsgPtr < caSharedMemoryMaxAddr )
	  && ( InMsgLength < (uint32_t) (caSharedMemoryMaxAddr - InMsgPtr)))
		return nfTGR_SUCCESS;

	// if we get here, something is wrong.
	COMMDBG("%s: invalid ptr/length (%p/%p, %p/%p, %p/%p)\n", __FUNCTION__,
				InMsgPtr, caSharedMemoryBaseAddr, InMsgPtr, caSharedMemoryMaxAddr,
				InMsgPtr + InMsgLength, caSharedMemoryMaxAddr );

	return nfTGR_INMSG_INVALID;

}/* end caValidateInboundMsgPtr */

/*********************************************************************
 * @function caValidateOutboundMsgPtr
 *
 * @brief verify that passed pointer is within allowed range for Outgoing msg
 *
 * @param[in] OutMsgPtr - pointer to incoming message
 * @param[in] OutMsgMax - max length of outgoing message
 *
 * @return nfTGR_SUCCESS if okay, nfTGR_OUTMSG_INVALID otherwise
 *
 * NB: need to look at optimization for this function. This should
 *     probably be inline-able.
 *
 *********************************************************************/
nfTeeResult_t caValidateOutboundMsgPtr( uint8_t 	*OutMsgPtr,
									    uint32_t	OutMsgMax )
{
	/*
	 * We want to ensure the following:
	 *
	 * 	- message pointer is within allowable range
	 * 		- Not in TEE memory
	 * 		- Not overlapping with InMsg
	 * 		- Not in MMIO (or other inappropriate) memory range
	 *
	 * 	- length does not cause message to exceed allowable range
	 *
	 * 	Because this is an x86 reference implementation, we know that
	 * 	InMsgPtr and OutMsgPtr are on the heap. We could add checks here
	 * 	to ensure that this is the case (to illustrate best practices),
	 * 	but this would be an academic exercise.
	 *
	 * 	What you check here will vary, depending on your implementation.
	 * 	The important thing to keep in mind is that the REE may be
	 * 	compromised, so you cannot trust these pointers or lengths. You
	 * 	need to check EVERY TIME to make sure they are "reasonable".
	 */

	if ( ( OutMsgPtr > caSharedMemoryBaseAddr )
	  && ( OutMsgPtr < caSharedMemoryMaxAddr )
	  && ( OutMsgMax < (uint32_t) (caSharedMemoryMaxAddr - OutMsgPtr) ))
		return nfTGR_SUCCESS;

	// if we get here, something is wrong.
	COMMDBG("%s: invalid ptr/length (%p/%p, %p/%p, %p/%p)\n", __FUNCTION__,
				OutMsgPtr, caSharedMemoryBaseAddr, OutMsgPtr, caSharedMemoryMaxAddr,
				OutMsgPtr + OutMsgMax, caSharedMemoryMaxAddr );

	return nfTGR_OUTMSG_INVALID;

}/* end caValidateOutboundMsgPtr */


uint32_t _teeNflxStoreIdfile(uint8_t *pu1SrcBuf, uint32_t u4SrcLen)
{
#ifndef NFLX_ULI_SUPPORT	
	Printf("[sahala Trustzone][_teeNflxStoreIdfile]: Enter non ULI branch.\n");
    uint8_t au1_plaintext[nfIDFLIE_MAX_FILE_SIZE];
    uint8_t au1_decodeBuf[nfIDFLIE_MAX_FILE_SIZE];
    long l_plainSize = 0;
		uint32_t pu4DstLen = 0;
  	uint32_t i = 0;

    Printf("[Trustzone][_teeNflxStoreIdfile]: Enter.\n");

    /* Check the idfile size */
    if(u4SrcLen < 0 || u4SrcLen > nfIDFLIE_MAX_FILE_SIZE) {
        pu4DstLen = 0;
        Printf("idfile size exceed %d bytes.\n", nfIDFLIE_MAX_FILE_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Initial au1_plaintext array*/
    memset(au1_plaintext, 0, nfIDFLIE_MAX_FILE_SIZE);
    l_plainSize = u4SrcLen;
    
    /* Check header and decrypt */
    if(pu1SrcBuf[0] == 0xa2 && pu1SrcBuf[1] == 0x5e && pu1SrcBuf[2] == 0xc5 && pu1SrcBuf[3] == 0x70) {
        Printf("Header match A2 pre-encrypted format. Decrypt it..\n");
        _teeDDIPreEncA2Dec(pu1SrcBuf, u4SrcLen, au1_plaintext, (uint32_t *)&l_plainSize);
        if(l_plainSize == 0) {
            Printf("Decrypt error.\n");
            pu4DstLen = 0;
            return TZ_ERROR_SECURITY;
        }
    } else {
        Printf("Header doesn't match pre-encrypted format. Try _teeSecureStorageSecretDec..\n");
        /* Decrypt by secure storage API */
        l_plainSize = u4SrcLen;
        if(_teeSecureStorageSecretDec(pu1SrcBuf, u4SrcLen, au1_plaintext, &l_plainSize) != TZ_SUCCESS) {
            Printf("Decrypt error.\n");
            pu4DstLen = 0;
            return TZ_ERROR_SECURITY;
        }
    }
	memset(IDFILE_BUF,0,nfIDFLIE_MAX_FILE_SIZE);
	idFile_len = 0;

	//
	memcpy_s2s_chk((VOID*)IDFILE_BUF,(VOID*)au1_plaintext,l_plainSize,NULL);
	idFile_len = l_plainSize;

	teePrintHexDump_tag("au1_plaintext",au1_plaintext,l_plainSize);

	teePrintHexDump_tag("IDFILE_BUF",IDFILE_BUF,l_plainSize);
	
	Printf("l_plainSize : %d \n",l_plainSize);
#else
	
	Printf("[sahala Trustzone][_teeNflxStoreIdfile]: Enter ULI branch.\n");  
    if(NULL == pu1SrcBuf || u4SrcLen < 0 || u4SrcLen > nfIDFLIE_MAX_FILE_SIZE) {
        Printf("idfile size exceed %d bytes.\n", nfIDFLIE_MAX_FILE_SIZE);
        return TZ_ERROR_ILLEGAL_ARGUMENT;
    }

	memset(IDFILE_BUF,0,nfIDFLIE_MAX_FILE_SIZE);
	idFile_len = 0;
	//copy esn
	uint8_t *pdata = IDFILE_BUF;
	memcpy_n2s_chk((VOID*)pdata,(VOID*)pu1SrcBuf,u4SrcLen,NULL);
	pdata += u4SrcLen;

	teePrintHexDump_tag("[sahala Trustzone]esn",pu1SrcBuf,u4SrcLen);

	*pdata = '\n';
	pdata ++ ;

	memcpy_s2s_chk((VOID*)pdata,(VOID*)au1_TEE_NFLX_kpe,TEE_NFLX_KPE_SIZE,NULL);
	pdata += TEE_NFLX_KPE_SIZE;

	*pdata = '\n';
	pdata ++ ;

	teePrintHexDump_tag("[sahala Trustzone]KPE",au1_TEE_NFLX_kpe,TEE_NFLX_KPE_SIZE);

	memcpy_s2s_chk((VOID*)pdata,(VOID*)au1_TEE_NFLX_kph,TEE_NFLX_KPH_SIZE,NULL);
	pdata += TEE_NFLX_KPH_SIZE;

	*pdata = '\0' ;

	teePrintHexDump_tag("[sahala Trustzone]KPH",au1_TEE_NFLX_kph,TEE_NFLX_KPH_SIZE);

	idFile_len = pdata - IDFILE_BUF;
	
	teePrintHexDump_tag("sahala IDFILE_BUF",IDFILE_BUF,idFile_len);	
#endif
	
    return TZ_SUCCESS;

}



int _teeNflxMSLEntry(uint8_t 	*InMsgPtr,uint32_t InMsgBufLength,uint8_t 	*OutMsgPtr,uint32_t OutMsgBufLength)
{
	#if TEE_DEBUG_ENABLE
	Printf("[trust zone] _teeNflxMSLEntry enter .\n");
	#endif

	nfTeeDispatchMsg(InMsgPtr,InMsgBufLength,OutMsgPtr,OutMsgBufLength);
	
	//the return value should be in outMsgPtr, so we can return 1 directly.
	return 1;
}


/*********************************************************************
 * @function nfTeeDispatchMsg
 *
 * @brief TEE message dispatcher/entry point
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 *
 * @return void
 *
 *********************************************************************/
void nfTeeDispatchMsg( uint8_t 	*InMsgPtr,
					   uint32_t InMsgBufLength,
					   uint8_t 	*OutMsgPtr,
					   uint32_t OutMsgBufLength )
{
	nfTeeResult_t 		Result = 0;
    nfTeeOperation_t 	Operation = nfTOB_UNDEFINED;
    uint32_t			OutMsgLength = 0;
    uint8_t 			*ReqPtr = NULL;
    uint32_t 			ReqBufLength = 0;
    uint8_t 			*RspPtr = NULL;
    uint32_t 			RspBufLength = 0;

    /*
     *  This is the TEE library entry point function. We use messages for
     *  communications between the REE and the TEE. The current communications
     *  model is very simple:
     *
     *	- The TEE is passive, and only responds to requests. It never
     *	  initiates action.
     *
     *	- This function requires two buffers, one containing the inbound
     *	  request (the "operation" and "operands"), and one which will
     *	  contain the response. The REE must allocate these buffers.
     *
     *	- This function is also the first line of defense against a
     *	  potentially hostile REE. So, we should do as much argument
     *	  validation as we can in this function, before invoking the
     *	  target trusted application (TA).
     *
     */

    // if OutMsg is invalid, we are finished.
    if ( ( OutMsgPtr == NULL )
      || ( OutMsgBufLength < sizeof(nfTeeApiRspHdr_t) )
      || (( Result = caValidateOutboundMsgPtr( OutMsgPtr, OutMsgBufLength ) ) != nfTGR_SUCCESS ) )
    {
    	// can't send a response; puke
    	/*printf("%s: must have valid outmsg (poinger: %p/length: %d)",
    			__FUNCTION__, OutMsgPtr, OutMsgBufLength );*/
    	//exit(1);
    	return;
    }

    // validate InMsgPtr/length
    if (InMsgPtr == NULL )
    {
        if (InMsgBufLength < sizeof(nfTeeApiReqHdr_t))
        {
            Result = nfTGR_INMSG_TOO_SHORT;
        }
        else
            Result = nfTGR_INMSG_INVALID;

        // Store result and length in outmsg
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(Result);
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;

        // if we get here, OutMsg contains response, just return
        return;

    }
    else if (InMsgBufLength < sizeof(nfTeeApiReqHdr_t))
    {
        // Store result and length in outmsg
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(nfTGR_INMSG_TOO_SHORT);
        ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;

        // if we get here, OutMsg contains response, just return
        return;
    }

    // Verify that InMsg is in valid shared memory range
    if ( ( ( Result = caValidateInboundMsgPtr( InMsgPtr, InMsgBufLength ) ) != nfTGR_SUCCESS ) )
    {
    	COMMDBG("%s: message buffer(s) failed validation.\n", __FUNCTION__);

		// Store result and length in outmsg
		((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(Result);
		((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = 0;
		return;
    }

    // okay, got past first line of obstacles. Now, check operation
    Operation = teeNtohl(((nfTeeApiReqHdr_t *) InMsgPtr)->Operation);

    /*
     * Because we are implementing the TEE as a library (instead of a VM, or
     * TZ, or a HW TEE), we have to deal with library artifacts (like function
     * call behavior, return values, etc.).
     *
     * In typical (non-libary) TEE implementation, we would expect that
     * communications with modules would be asynchronous (we send and receive
     * messages, all message ops are non-blocking), but that's not the case here.
     * So, we have to deal with this.
     *
     * We adopt the following conventions:
     *
     *  - this dispatcher GUARANTEES that InMsgPtr, InMsgBufLength, OutMsgPtr,
     *    OutMsgBuflength, and Operation are all valid and/or reasonable. The
     *    module dispatch function need not repeat basic argument validation.
     *
     *	- dispatch functions MUST return message length and a
     *	  valid/informative ResponseCode.
     *
     */

    // fix up lengths and buffer pointers for agent dispatch functions
    if ( ( ReqBufLength = InMsgBufLength - sizeof(nfTeeApiReqHdr_t) ) > 0 )
    	ReqPtr = InMsgPtr + sizeof(nfTeeApiReqHdr_t);

    if ( ( RspBufLength = OutMsgBufLength - sizeof(nfTeeApiRspHdr_t) ) > 0 )
    	RspPtr = OutMsgPtr + sizeof(nfTeeApiRspHdr_t);
	
    /*
     * We haven't implemented session management yet, so we use the operation
     * macros to determine which agent to route requests to. When we implement
     * session management, this will change.
     */
    if ( nfTeeIsInitialized() == nfBT_TRUE )
    {
        if ( nfTee_IS_MGMT_OP(Operation) )
        {
            COMMDBG("%s: calling mgmtAgentDispatchMsg()\n", __FUNCTION__ );
            Result = mgmtAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
            							   RspPtr, RspBufLength,
                                           &OutMsgLength, NrdClientId );

        }
        else if ( nfTEE_IS_CRYPTO_OP(Operation) )
        {
            // call crypto message dispatch function
            Result = cryptoAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
            							   	 RspPtr, RspBufLength,
            							   	 &OutMsgLength, NrdClientId );
        }
        else if ( nfTEE_IS_STORAGE_OP(Operation) )
        {
            // call storage message dispatch function
        	Result = storageAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
        									  RspPtr, RspBufLength,
        									  &OutMsgLength, NrdClientId );

        }
        else if ( nfTEE_IS_DRM_OP(Operation) )
        {
            // call drm message dispatch function
        	Result = drmAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
        									  RspPtr, RspBufLength,
        									  &OutMsgLength, NrdClientId );

        }
        else
        {
            // we don't recognize the operations
            Result = nfTGR_INVALID_OPERATION;
        }

    }
    else
    {
    	// if we are not initialized, the only message allowed is an INIT message
		if ( nfTee_IS_MGMT_OP(Operation) )
		{
			COMMDBG("%s: calling mgmtAgentDispatchMsg()\n", __FUNCTION__ );
			Result = mgmtAgentDispatchMsg( Operation, ReqPtr, ReqBufLength,
				   	 	 	 	 	 	   RspPtr, RspBufLength,
				   	 	 	 	 	 	   &OutMsgLength, NrdClientId );

		}
		else
		{
			// not initialized and not a mgmt message --> this is an error
			Result = nfTMR_NOT_INITIALIZED;
		}
    }

    // Store result and length in outmsg
    ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseCode = teeHtonl(Result);
    ((nfTeeApiRspHdr_t *)OutMsgPtr)->ResponseLength = teeHtonl(OutMsgLength);

    // if we get here, OutMsg contains response, just return
    return;

}/* end nfTeeDispatchMsg */

// vim: expandtab tabstop=4 shiftwidth=4
