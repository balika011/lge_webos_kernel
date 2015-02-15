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
 *		- nfTeeProcMsg(): you communicate with the TEE by passing it a message, and waiting
 *		  for the response. This is the function you call to do that.
 */

#include "CommTypes.h"
#include "MgmtTypes.h"
#include "TeeTypes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "StorageAgent.h"
#include "CryptoAgent.h"
#include "DrmAgent.h"

/*
 * This struct should be static (only visible in TeeMgr), but because
 * we are implementing as a library, it is convenient to make it also
 * visible to the tee message dispatch function. So, it is not defined
 * as static here, but no other function should modify this, and only
 * nfTeeDispatchMsg() should read/rely on this.
 *
 * Allowing it to be readable saves us the overhead of a function
 * call into this module for every message. This may change in the future,
 * though, so don't count on it.
 */
static struct _nfTeeState
{
    nfBoolean_t	Initialized;
} nfTeeState = { .Initialized = nfBT_FALSE };

// this is the one true definition of the debug flags global
nfDebugFlags_t nfDebugFlags = 0;

#define nfMANUF_SS_PATH "./manuf_ss.bin"
//#define nfMANUF_SS_PATH_BACKUP "/tmp/manuf_ss.bin"

/*********************************************************************
 * @function nfTeeIsInitialized()
 *
 * @brief check to see if tee has been initialized
 *
 * @return 0 if not initialized, 1 otherwise
 *
 *********************************************************************/
nfBoolean_t nfTeeIsInitialized(void)
{

    return ( nfTeeState.Initialized );

}/* end nfTeeIsInitialized */

/*********************************************************************
 * @function maTeeShutdown
 *
 * @brief TEE shutdown function
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
static nfTeeMgmtResult_t maTeeShutdown( uint8_t		*InMsgPtr,
                          uint32_t		InMsgBufLength,
                          uint8_t		*OutMsgPtr,
                          uint32_t		OutMsgBufLength,
						  uint32_t		*MsgLengthPtr )
{

    (void)InMsgPtr;
    (void)InMsgBufLength;
    (void)OutMsgPtr;
    (void)OutMsgBufLength;
    (void)MsgLengthPtr;

    // do shutdown and update OutMsg for successful response
    MGMTDBG("%s: shutting down TEE...\n", __FUNCTION__);

    // XXX - need to call module shutdown functions...
    MGMTDBG("%s: XXX - CALL MODULE SHUTDOWN FNs - XXX\n", __FUNCTION__);

    nfTeeState.Initialized = nfBT_FALSE;

    (void) OPENSSL_LIBTOM_EXCHANGE(teeCryptoShutdown)();

    return (nfTeeMgmtResult_t) nfTGR_SUCCESS;

}/* end maTeeShutdown */

/*********************************************************************
 * @function maTeeInit
 *
 * @brief TEE initialization function
 *
 * @param[in] InMsgPtr - pointer to incoming message
 * @param[in] InMsgBufLength - actual size of input buffer
 * @param[out] OutMsgPtr - pointer to buffer for response
 * @param[in] OutMsgBufLength - maximum size of response
 *
 * @return nfTGR_SUCCESS if everything okay
 *
 *********************************************************************/
 static nfTeeMgmtResult_t maTeeInit( uint8_t			*InMsgPtr,
                       uint32_t			InMsgBufLength,
                       uint8_t			*OutMsgPtr,
                       uint32_t			OutMsgBufLength,
                       uint32_t			*MsgLengthPtr )
{
     nfTeeMgmtResult_t Result;

     // these template args are not used by this function
     (void)OutMsgBufLength;
     (void)OutMsgPtr;
     (void)OutMsgBufLength;

     // default setting: no response
     *MsgLengthPtr = 0;

     // validate inmsg
     if ( InMsgBufLength < nfMIN_INIT_REQ_SIZE )
     {
 		// this buffer is too small to hold a valid message
 		INITDBG("%s: InMsgBufLength is %d, expected size is larger than %d\n",
 				__FUNCTION__, InMsgBufLength, nfMIN_INIT_REQ_SIZE);

 		return (nfTeeMgmtResult_t) nfTMR_PARAM_TOO_SMALL;
     }

     if ( InMsgPtr == NULL )
     {
    	 //printf("%s: InMsgPtr is NULL?!\n", __FUNCTION__);
    	 //exit(1);
    	 return (nfTeeMgmtResult_t) nfTMR_NOT_INITIALIZED;
     }

     // get debug flags
     nfDebugFlags = teeNtohl(((nfTeeInitRequest_t *)InMsgPtr)->DebugFlags);
     //nfDebugFlags = 0xFFFFFFFF;
     INITDBG("%s: set DebugFlags to %08x\n", __FUNCTION__, nfDebugFlags );

     // first, storageAgent

     INITDBG("%s: calling storageAgentInit_data() with >%s<\n", __FUNCTION__, nfMANUF_SS_PATH  );
     Result = (nfTeeMgmtResult_t) storageAgentInit_data(NULL);

     if ( Result != (nfTeeMgmtResult_t) nfTGR_SUCCESS )
     {
        INITDBG("%s: can't initialize secure store, giving up.\n", __FUNCTION__);
        return Result;
     }/* end if secure store not found in cwd */

     // next, CryptoAgent
     INITDBG("%s: calling cryptoAgentInit()\n", __FUNCTION__ );
     Result = (nfTeeCryptoResult_t)cryptoAgentInit(0, NULL);

     if ( Result != (nfTeeMgmtResult_t) nfTGR_SUCCESS )
     {
        INITDBG("%s: failed to init crypto agent; bailing.\n", __FUNCTION__);
        return Result;
     }

     // next, DrmAgent
     INITDBG("%s: calling drmAgentInit()\n", __FUNCTION__ );
     Result = (nfTeeDrmResult_t)drmAgentInit(0, NULL);

     if ( Result != (nfTeeMgmtResult_t) nfTGR_SUCCESS )
     {
        INITDBG("%s: failed to init drm agent; bailing.\n", __FUNCTION__);
        return Result;
     }

     nfTeeState.Initialized = nfBT_TRUE;

     INITDBG("%s: TEE is initialized, returning nfTGR_SUCCESS.\n", __FUNCTION__ );

     return (nfTeeMgmtResult_t) nfTGR_SUCCESS;

}/* end maTeeInit */

 /*********************************************************************
  * @function mgmtAgentDispatchMsg
  *
  * @brief Crypto Agent message handler/entry point
  *
  * @param[in] Operation - operation requested by client
  * @param[in] InMsgPtr - pointer to incoming message
  * @param[in] InMsgBufLength - actual size of input buffer
  * @param[out] OutMsgPtr - pointer to buffer for response
  * @param[in] OutMsgBufLength - maximum size of response
  * @param[in] ClientId - ID for REE client who made request
  *
  * @return nfTGR_SUCCESS if everything is okay
  *
  *********************************************************************/
 nfTeeMgmtResult_t mgmtAgentDispatchMsg( nfTeeOperation_t	Operation,
                          uint8_t			*InMsgPtr,
                          uint32_t			InMsgBufLength,
                          uint8_t			*OutMsgPtr,
                          uint32_t			OutMsgBufLength,
                          uint32_t			*MsgLengthPtr,
                          nfTeeClientId_t	ClientId )
 {
	 nfTeeMgmtResult_t	result = (nfTeeMgmtResult_t)nfTGR_SUCCESS;

    (void)ClientId;

    /*
     * This is the message dispatcher for the mgmt agent. Usually, dispatch
     * functions are only called if the system is initialized, but since
     * this agent is the one responsible for initialization, it can be
     * called before the system is init'd. In that case, the only valid
     * operation is INIT.
     *
     * The tee dispatcher function validated all args, so we can assume
     * they are good. If this agent were implemented standalone (e.g. in
     * a VM, or as a separate process), then argument validation would be
     * important.
     */

		/*
    assert(InMsgPtr);
    assert(OutMsgPtr);
    assert(MsgLengthPtr);
    assert(ClientId);
		*/
    // in case of error, message is always empty
    *MsgLengthPtr = 0;

    switch ( Operation )
    {
        case nfTO_MGMT_OP_INIT_TEE:
            if ( 1 )//nfTeeState.Initialized == nfBT_FALSE )
            {
                result = maTeeInit( InMsgPtr, InMsgBufLength,
                                   OutMsgPtr, OutMsgBufLength, MsgLengthPtr );
            }
            else
            {
                INITDBG("%s: called to INIT, but already INIT'd.\n", __FUNCTION__ );

                // already initialized
                return nfTMR_ALREADY_INITIALIZED;

            }
            break;
        case nfTO_MGMT_OP_SHUTDOWN:
            if ( nfTeeState.Initialized == nfBT_TRUE )
            {
                result = maTeeShutdown( InMsgPtr, InMsgBufLength,
                                       OutMsgPtr, OutMsgBufLength, MsgLengthPtr );
            }
            else
            {
                MGMTDBG("%s: received shutdown message, but not initialized?!\n", __FUNCTION__);

                // this is an error, how the heck did we get here?
                return (nfTeeMgmtResult_t) nfTGR_INTERNAL_ERROR;
            }
            break;
        case nfTO_MGMT_OP_DEBUG:
            if ( nfTeeState.Initialized == nfBT_TRUE )
            {
            	if ( InMsgBufLength >= nfMIN_DEBUG_REQ_SIZE)
            		nfDebugFlags = teeNtohl(((nfTeeDebugRequest_t *)InMsgPtr)->DebugFlags);
            	else
            	{
            		MGMTDBG("%s: invalid message length (%d)\n", __FUNCTION__, InMsgBufLength);
            		return nfTMR_PARAM_TOO_SMALL;
            	}
            }
            else
            {
                MGMTDBG("%s: received shutdown message, but not initialized?!\n", __FUNCTION__);

                // this is an error, how the heck did we get here?
                return (nfTeeMgmtResult_t) nfTGR_INTERNAL_ERROR;
            }
        	break;
        default:
            // XXX - replace with debug macro...
            MGMTDBG( "%s: unimplemented operation (%08x)\n",
                     __FUNCTION__, Operation );
            return (nfTeeMgmtResult_t) nfTGR_INVALID_OPERATION;
            break;
    }/* end switch ( Operation ) */


    // else
    MGMTDBG("%s: returning nfTGR_SUCCESS\n", __FUNCTION__ );
    return result;

 }/* end mgmtAgentDispatchMsg */

// vim: expandtab tabstop=4 shiftwidth=4

