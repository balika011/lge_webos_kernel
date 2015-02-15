/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DRMAGENT_H_
#define DRMAGENT_H_

#include "DrmTypes.h"

nfTeeResult_t drmAgentInit( int argc, char **argv );

nfTeeResult_t drmAgentDispatchMsg( nfTeeOperation_t	Operation,
							 	   uint8_t 			*InMsgPtr,
  								   uint32_t 			InMsgBufLength,
  								   uint8_t 			*OutMsgPtr,
  								   uint32_t 			OutMsgBufLength,
  								   uint32_t			*MsgLengthPtr,
  								   nfTeeClientId_t 	ClientId );



#endif /* DRMAGENT_H_ */
