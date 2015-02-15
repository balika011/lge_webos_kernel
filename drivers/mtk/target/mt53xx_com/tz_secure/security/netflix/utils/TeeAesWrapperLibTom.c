/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeAesWrapperLibTom.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"

nfTeeCryptoResult_t teeAesUnwrap_libtom( const unsigned char 	*WrappedKeyPtr,
								  uint32_t				WrappedKeyLength,
								  uint8_t			 	*AesWrapKeyPtr,
								  uint32_t				AesWrapKeyLength,
								  uint8_t				*InitValPtr,
								  uint32_t				*UnwrappedKeyLengthPtr,
								  unsigned char			 *UnwrappedKeyPtr)
{

	aes_unwrap_key(AesWrapKeyPtr,InitValPtr,UnwrappedKeyPtr,WrappedKeyPtr,WrappedKeyLength);
	*UnwrappedKeyLengthPtr = WrappedKeyLength - 8;
    return nfTGR_SUCCESS;
	
}/* end teeAesUnwrap */
