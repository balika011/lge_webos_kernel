/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommTypes.h"
#include "DrmTypes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "StorageAgent.h"

// DRM RO client ID, this ID must match DRM_RO client ID in StorageAgent/StorageAgentInternals.h
nfTeeClientId_t drmStorageClientId =
	{0xb9,0x0e,0x81,0x64,0x91,0x67,0x59,0xaa,0xa8,0x63,0x1d,0x0d,0xc9,0xac,0xb8,0x66};

/*
 * In a "real" implementation, this agent will have an IPC interface
 * to the storage agent, and will use that to request data (e.g. keys).
 * Since we are using a library, and since the storage agent already
 * has a message-based interface, we'll re-use that here. We need to
 * pre-allocate buffers for InMsg and OutMsg.
 */
// size of pre-allocate buffers for storage agent InMsg and OutMsg.
// this is for our TPI/IPC hack.
#define nfSA_REQ_BUF_SIZE 		256 // way bigger than we need
#define nfSA_RSP_BUF_SIZE 		4096 // way bigger than we need
static uint8_t drmStorageRequestBuf[nfSA_REQ_BUF_SIZE];
static uint8_t drmStorageResponseBuf[nfSA_RSP_BUF_SIZE];

/*********************************************************************
 * @function drmLoadProvisionedKeys
 *
 * @brief load pre-provisioned keys
 *
 *
 * @returns nfTGR_SUCCESS if successful,
 *
 *********************************************************************/
static nfTeeDrmResult_t drmLoadProvisionedKeys()
{
    nfTeeStorageSimpleReadRsp_t *RspPtr;
    uint32_t					Result;
    nfTeeClientId_t				ClientId;
    uint32_t					MsgLength = 0;
    nfTeeStorageSimpleReadReq_t *ReqPtr = (nfTeeStorageSimpleReadReq_t *) drmStorageRequestBuf;

    /*
      * Request ESN from storage agent. Use existing message interface, but partners will
      * have to replace this with their internal TEE IPC mechanism.
      *
      * We need to do the following:
      *
      * 	- construct read request in drmStorageRequestBuf
      * 	- call storageAgentDispatchMsg
      * 	- if successful
      * 		- extract model private key from drmStorageResponseBuf
      * 		- store it (where?)
      *  - else
      *  	- bail out, this is fatal
      */

     INITDBG("%s: loading Model private key\n", __FUNCTION__ );
     // construct read request
     ReqPtr->RequestedKey = teeHtonl(drmAGENT_KEY_MODEL_PRIV);

     // init client ID
     (void) teeMemcpy( ClientId, drmStorageClientId, sizeof(ClientId) );

     // call storageAgent
     Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
     								  (uint8_t *) ReqPtr, nfMIN_SIMPLE_READ_REQ_SIZE ,
     								  drmStorageResponseBuf, nfSA_RSP_BUF_SIZE,
     								  &MsgLength, ClientId );
     if ( Result != nfTGR_SUCCESS )
     {
     	INITDBG("%s: failed to retrieve model priv key (error: %08x)\n", __FUNCTION__, Result);
     	return Result;
     }

     /*
      * Because we hacked this to use the standard message interface
      * to storageAgent, the response has this structure:
      *
      * typedef struct
   	  *	{
 	  *		nfTeeClientId_t	ClientId; // client id associated with this data
 	  *		uint32_t		DataLength;
 	  *		uint8_t			Data[];
 	  *	} nfTeeStorageSimpleReadRsp_t;
      *
      *
      */
     RspPtr = (nfTeeStorageSimpleReadRsp_t *)drmStorageResponseBuf;

     // now, copy key
     #if TEE_DEBUG_ENABLE
     uint32_t PrivKeyLength = teeNtohl(RspPtr->DataLength);
     INITDBG("%s: Model private key Length is %d\n", __FUNCTION__, PrivKeyLength);
	 #endif
     // XXX - need to store the key (where? we'll add this once we integrate the
     //       playready hal code...

    /*
     * Request model cert from storage agent. Use existing message interface, but partners will
     * have to replace this with their internal TEE IPC mechanism.
     *
     * We need to do the following:
     *
     * 	- construct read request in drmStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract model cert from drmStorageResponseBuf
     * 		- save it
     *  - else
     *  	- bail out, this is fatal
     */

    INITDBG("%s: loading playready model cert\n", __FUNCTION__ );
    // construct read request
    ReqPtr->RequestedKey = teeHtonl(drmAGENT_KEY_MODEL_CERT);

    // init client ID
    (void) teeMemcpy( ClientId, drmStorageClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  drmStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  drmStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != nfTGR_SUCCESS )
    {
    	INITDBG("%s: failed to retrieve model cert (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    RspPtr = (nfTeeStorageSimpleReadRsp_t *)drmStorageResponseBuf;
	#if TEE_DEBUG_ENABLE
    uint32_t ModelCertLength = teeNtohl(RspPtr->DataLength);

    // now, copy cert
	INITDBG("%s: Model cert Length is %d\n", __FUNCTION__, ModelCertLength);
	#endif
	// XXX - need to store the cert (where? we'll add this once we integrate the
	//       playready hal code...

    /*
     * Request dev cert template from storage agent. Use existing message interface, but
     * partners will have to replace this with their internal TEE IPC mechanism.
     *
     * We need to do the following:
     *
     * 	- construct read request in drmStorageRequestBuf
     * 	- call storageAgentDispatchMsg
     * 	- if successful
     * 		- extract device cert template from drmStorageResponseBuf
     * 		- save it
     *  - else
     *  	- bail out, this is fatal
     */

    INITDBG("%s: loading playready device cert template\n", __FUNCTION__ );
    // construct read request
    ReqPtr->RequestedKey = teeHtonl(drmAGENT_KEY_DEVCERT_TEMPLATE);

    // init client ID
    (void) teeMemcpy( ClientId, drmStorageClientId, sizeof(ClientId) );

    // call storageAgent
    Result = storageAgentDispatchMsg( nfTO_STORAGE_OP_SIMPLE_READ,
    								  drmStorageRequestBuf, nfMIN_SIMPLE_READ_REQ_SIZE ,
    								  drmStorageResponseBuf, nfSA_RSP_BUF_SIZE,
    								  &MsgLength, ClientId );
    if ( Result != (nfTeeStorageResult_t)nfTGR_SUCCESS )
    {
    	INITDBG("%s: failed to retrieve device cert template (error: %08x)\n", __FUNCTION__, Result);
    	return Result;
    }

    RspPtr = (nfTeeStorageSimpleReadRsp_t *)drmStorageResponseBuf;
	#if TEE_DEBUG_ENABLE
   	uint32_t CertTemplateLength = teeNtohl(RspPtr->DataLength);

    // now, copy cert
	INITDBG("%s: Cert template length is %d\n", __FUNCTION__, CertTemplateLength);
	#endif
	// XXX - need to store the template (where? we'll add this once we integrate the
	//       playready hal code...

	return (nfTeeDrmResult_t) nfTGR_SUCCESS;

}/* end drmLoadProvisionedKeys */

/*********************************************************************
 * @function drmAgentInit
 *
 * @brief DRM Agent initialization
 *
 * @param[in] argc - argument count
 * @param[in] argv - pointer to argument vector
 *
 * @returns nfTSR_SUCCESS,
 *
 *********************************************************************/
nfTeeDrmResult_t drmAgentInit( int argc, char **argv )
{
	nfTeeDrmResult_t	Result;

	//TODO: these lines are added just to avoid compiler warnings, deal with the args properly
	if ((argc > 0) && (argv) && (*argv))
	{
		DRMDBG("%s: we have some arguments, do something with them!\n", __FUNCTION__);
	}

	INITDBG("%s: calling drmLoadProvisionedKeys()\n", __FUNCTION__);

	Result = drmLoadProvisionedKeys();

	INITDBG("%s: done with drmLoadProvisionedKeys()\n", __FUNCTION__);

	return Result;

}/* end drmAgentInit */


