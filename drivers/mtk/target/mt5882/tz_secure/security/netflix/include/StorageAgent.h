/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STORAGEAGENT_H_
#define STORAGEAGENT_H_

#include "StorageTypes.h"


enum ntba_RO_AgentClientKeys
{
    // these elements are write-protected
    ntbaAGENT_KEY_ESN = 0x80000001,
    ntbaAGENT_KEY_KPE = 0x80000002,
    ntbaAGENT_KEY_KPH = 0x80000003,
    ntbaAGENT_KEY_KPW = 0x80000004,
    ntbaAGENT_KEY_INTERNAL = 0x80000005, // crypto agent "personal" key
};

enum drm_RO_AgentClientKeys
{
    // these elements are write-protected
    drmAGENT_KEY_MODEL_PRIV = 0x80010001,
    drmAGENT_KEY_MODEL_CERT = 0x80010002,
    drmAGENT_KEY_DEVCERT_TEMPLATE = 0x80010003,
};

enum tee_ClientIdKeys
{
	teeAPPID_KEY_NRD = 0xF00D0001,
};

nfTeeResult_t saLookupKey( nfTeeClientId_t 			ClientId,
				 nfSecureStoreClientKey_t	Key,
				 uint8_t					*DataPtr,
				 uint32_t					*DataLengthPtr,
				 uint32_t					MaxLength );

nfTeeResult_t storageAgentInit();

nfTeeResult_t storageAgentInit_data( const char*  SecureStoreFileData);
nfTeeResult_t storageAgentDispatchMsg( nfTeeOperation_t	Operation,
							 uint8_t 			*InMsgPtr,
							 uint32_t 			InMsgBufLength,
							 uint8_t 			*OutMsgPtr,
							 uint32_t 			OutMsgBufLength,
							 uint32_t			*MsgLengthPtr,
							 nfTeeClientId_t 	ClientId );

/*
 * Constants used for crypto operations
 */
#define saSHA1_HASH_SIZE				20

#endif /* STORAGEAGENT_H_ */
