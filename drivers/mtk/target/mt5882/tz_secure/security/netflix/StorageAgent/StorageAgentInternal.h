/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STORAGEAGENTINTERNAL_H_
#define STORAGEAGENTINTERNAL_H_

// TBD: need local header file for these...
extern nfTeeResult_t saProtectData( nfTeeClientId_t	ClientId,
									uint8_t 		*DataPtr,
									uint32_t 		DataLength,
									uint8_t 		*ProtectedDataPtr,
									uint32_t 		ProtectedBufLength,
									uint32_t		*ProtectedLengthPtr );

extern nfTeeResult_t saUnprotectData( nfTeeClientId_t 	ClientId,
									  uint8_t 			*ProtectedDataPtr,
									  uint32_t 			ProtectedDataLength,
									  uint8_t 			*ClearDataPtr,
									  uint32_t 			ClearBufLength,
									  uint32_t			*ClearLengthPtr );

#endif /* STORAGEAGENTINTERNAL_H_ */
