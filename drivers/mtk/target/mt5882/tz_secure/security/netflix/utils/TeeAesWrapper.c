/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeAesWrapper.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include <openssl/aes.h>

nfTeeCryptoResult_t teeAesUnwrap( const unsigned char 	*WrappedKeyPtr,
								  uint32_t				WrappedKeyLength,
								  uint8_t			 	*AesWrapKeyPtr,
								  uint32_t				AesWrapKeyLength,
								  uint8_t				*InitValPtr,
								  uint32_t				*UnwrappedKeyLengthPtr,
								  unsigned char			 *UnwrappedKeyPtr)
{
    AES_KEY 	AesKey;
    int ret = AES_set_decrypt_key(AesWrapKeyPtr, AesWrapKeyLength * 8, &AesKey);
    if (ret != 0)
	{
    	AESDBG("%s: AES_set_decrypt_key failed\n", __FUNCTION__ );
    	return nfTCR_INTERNAL_ERROR;
	}

    if ( *UnwrappedKeyLengthPtr < (WrappedKeyLength - 8) )
    {
    	AESDBG("%s: not enough space for unwrapped key (%d/%d)\n", __FUNCTION__,
    			*UnwrappedKeyLengthPtr, (WrappedKeyLength - 8) );
    	return nfTCR_BUFFER_TOO_SMALL;

    }

    *UnwrappedKeyLengthPtr = WrappedKeyLength - 8;

    ret = AES_unwrap_key(&AesKey, InitValPtr, UnwrappedKeyPtr, WrappedKeyPtr, WrappedKeyLength);
    if (ret < 0 || ret != (int)(*UnwrappedKeyLengthPtr))
    {
    	AESDBG("%s: AES_unwrap_key failed, returned %d\n", __FUNCTION__, ret);
    	return nfTCR_INTERNAL_ERROR;
    }

    return nfTGR_SUCCESS;

}/* end teeAesUnwrap */
