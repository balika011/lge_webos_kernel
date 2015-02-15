/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TeeAesCipher.h"
#include "TeeInternalDpi.h"
#include "TeeDebug.h"
#include <openssl/aes.h>
#include <openssl/evp.h>
#if 0
/*********************************************************************
 * @function teeAesCbcCipher
 *
 * @brief creates EVP_CIPHER for CBC based on key Size
 *
 * @param[in] size 					- size of AES-CBC key
 * 									  that gets created
 *
 * @returns EVP_CIPHER object
 *
 *********************************************************************/
static const EVP_CIPHER* teeAesCbcCipher(uint32_t size)
{
	const EVP_CIPHER* cipher = NULL;
	switch (size)
	{
		case 16:
			cipher = EVP_aes_128_cbc();
			break;
		case 24:
			cipher = EVP_aes_192_cbc();
			break;
		case 32:
			cipher = EVP_aes_256_cbc();
			break;
	}
	return cipher;
}
#endif

/*********************************************************************
 * @function teeAesGcmCipher
 *
 * @brief creates EVP_CIPHER for GCM based on key Size
 *
 * @param[in] size 					- size of AES-GCM key
 * 									  that gets created
 *
 * @returns EVP_CIPHER object
 *
 *********************************************************************/
static const EVP_CIPHER* teeAesGcmCipher(uint32_t size)
{
	const EVP_CIPHER* cipher = NULL;
	#ifdef HAVE_GCM_OPENSSL
	switch (size)
	{
		case 16:
			cipher = EVP_aes_128_gcm();
			break;
		case 24:
			cipher = EVP_aes_192_gcm();
			break;
		case 32:
			cipher = EVP_aes_256_gcm();
			break;
	}
	#endif
	return cipher;
}

/*********************************************************************
 * @function teeAesGetOutSize
 *
 * @brief computes the size of encryption/decryption output block
 *
 * @param[in] insize - size of the input block
 *
 * @returns int
 *
 *********************************************************************/
static int teeAesGetOutSize(int inSize)
{
    // according to the openssl docs:
    // the amount of data written out may be as large as (inputsize + cipher_block_size - 1)
	int outSize = inSize + AES_BLOCK_SIZE - 1;
    // the output buffer must also be a multiple of blocksize
    if ((outSize % AES_BLOCK_SIZE) != 0)
    	outSize = ((outSize / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
	return outSize;
}

nfTeeCryptoResult_t teeAesCipherInit ( nfAesCipher_t 		cipherType,
										const unsigned char *key,
										uint32_t 			keySize,
										const unsigned char *iv,
										uint32_t 			ivSize,
										nfAesCipherContext_t  	**aesContextPtrPtr)
{
	//if cipherType is GCM then make sure openssl library supports GCM, ASSERT otherwise
	if (cipherType == nfAES_GCM)
	{
		#ifndef EVP_CTRL_GCM_SET_IVLEN
			AESDBG("%s: ALERT: open ssl library does not support AES-GCM, asserting\n", __FUNCTION__ );
			assert(0);
		#endif
	}

	// check for bad args
	if ((key == NULL) || (keySize == 0) || (iv == NULL) || (ivSize == 0))
	{
		AESDBG("%s: required Args are NULL or 0\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if ((keySize != 16) && (keySize != 24) && (keySize != 32))
	{
		printf("%s: key size is wrong, its %d, should be 16, 24 or 32", __FUNCTION__, keySize);
		return nfTCR_BAD_PARAMETER;
	}

	// allocate memory for aesContextPtr
	*aesContextPtrPtr = (nfAesCipherContext_t*)(teeMemAlloc(sizeof(nfAesCipherContext_t)));
	if (*aesContextPtrPtr == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for aes cipher context\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}

	// assign ciphertype
	(*aesContextPtrPtr)->cipherType = cipherType;

	// allocate memory for key and assign it
	(*aesContextPtrPtr)->key = (unsigned char*)(teeMemAlloc(keySize+1));
	if ((*aesContextPtrPtr)->key == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for key inside aes cipher context\n", __FUNCTION__ );
		teeAesCipherTerm(*aesContextPtrPtr);
		return nfTCR_OUT_OF_MEMORY;
	}
	teeMemcpy((*aesContextPtrPtr)->key, key, keySize);
	((*aesContextPtrPtr)->key)[keySize] = '\0';

	// assign keysize
	(*aesContextPtrPtr)->keySize = keySize;

	// allocate memory for key and assign IV
	(*aesContextPtrPtr)->iv = (unsigned char*)(teeMemAlloc(ivSize+1));
	if ((*aesContextPtrPtr)->iv == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for IV inside aes cipher context\n", __FUNCTION__ );
		teeAesCipherTerm(*aesContextPtrPtr);
		return nfTCR_OUT_OF_MEMORY;
	}
	teeMemcpy((*aesContextPtrPtr)->iv, iv, ivSize);
	((*aesContextPtrPtr)->iv)[ivSize] = '\0';

	//assign ivsize
	(*aesContextPtrPtr)->ivSize = ivSize;

	// return success
	return nfTGR_SUCCESS;
}

void teeAesCipherTerm(nfAesCipherContext_t *aesContextPtr)
{
    if (aesContextPtr != NULL) {
        if(aesContextPtr->key) {
        	teeMemFree((uint8_t**) &(aesContextPtr->key));
        }
        if(aesContextPtr->iv) {
        	teeMemFree((uint8_t**) &(aesContextPtr->iv));
		}
        teeMemFree((uint8_t**) &aesContextPtr);
    }
}
#if 0
nfTeeCryptoResult_t teeAesCbcEncrypt (const unsigned char 	*clear,
									uint32_t				clearLength,
									const nfAesCipherContext_t 	*aesContextPtr,
									unsigned char	 		**encrypted)
{
	//validate arguments
	if ((aesContextPtr == NULL) || (clear == NULL) || (teeStrlen((const char*) clear) == 0))
	{
		AESDBG("%s: required Args are NULL or 0\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if (aesContextPtr->cipherType != nfAES_CBC)
	{
		AESDBG("%s: aesContextPtr was not initialized for CBC cipher\n", __FUNCTION__ );
		return nfTCR_MALFORMED_DATA;
	}

	// get clear and encrypted sizes
    int encLength = teeAesGetOutSize (clearLength);

	// allocate memory for encrypted
    *encrypted = (unsigned char*) (teeMemAlloc(encLength));
    if (*encrypted == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for encrypted\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}

    // init encryption
    EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* pCipher = teeAesCbcCipher(aesContextPtr->keySize);
    if (pCipher == NULL)
    {
    	teeMemFree(encrypted);
    	EVP_CIPHER_CTX_free(pCtx);
    	return nfTCR_INTERNAL_ERROR;
    }

    int ret = EVP_EncryptInit(pCtx, pCipher, aesContextPtr->key, aesContextPtr->iv);
    if (!ret)
	{
		AESDBG("%s: EVP_EncryptInit failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
	}

    // do the encrypt; must keep track of the number of bytes produced
    int nBytes = 0;
    ret = EVP_EncryptUpdate(pCtx, *encrypted, &nBytes, clear, clearLength);
    if (!ret)
	{
		AESDBG("%s: EVP_EncryptUpdate failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
	}

    int nTotalBytes = nBytes;
    ret = EVP_EncryptFinal(pCtx, &((*encrypted)[nBytes]), &nBytes);
    if (!ret)
	{
		AESDBG("%s: EVP_EncryptFinal failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
	}
    nTotalBytes += nBytes;

    // done with Ctx, free it
    EVP_CIPHER_CTX_free(pCtx);

    // the actual output size is in nTotalBytes
    if (nTotalBytes == 0)
    {
    	AESDBG("%s: total 0 bytes encrypted\n", __FUNCTION__ );
    	teeMemFree(encrypted);
		return nfTCR_INTERNAL_ERROR;
    }

    // resize encrypted
    *encrypted = (unsigned char*) teeMemRealloc(*encrypted, nTotalBytes+1);
    if (*encrypted == NULL)
    {
    	AESDBG("%s: Out of memory, cannot reallocate for encrypted\n", __FUNCTION__ );
    	teeMemFree(encrypted);
    	return nfTCR_OUT_OF_MEMORY;
    }
    (*encrypted)[nTotalBytes] = '\0';

    return nfTGR_SUCCESS;
}

nfTeeCryptoResult_t teeAesCbcDecrypt(const unsigned char	*encrypted,
									uint32_t			encryptedLength,
									const nfAesCipherContext_t	*aesContextPtr,
									unsigned char			**clear)
{
	//validate arguments
	if ((aesContextPtr == NULL) || (encrypted == NULL) || (teeStrlen((const char*) encrypted) == 0))
	{
		AESDBG("%s: required Args are NULL or 0\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if (aesContextPtr->cipherType != nfAES_CBC)
	{
		AESDBG("%s: aesContextPtr was not initialized for CBC cipher\n", __FUNCTION__ );
		return nfTCR_MALFORMED_DATA;
	}

	// get clear and encrypted sizes
	int clearLength = teeAesGetOutSize (encryptedLength);

	// allocate memory for clear
	*clear = (unsigned char*) (teeMemAlloc(clearLength));
	if (*clear == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for clear\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}

    // init decryption
    EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* pCipher = teeAesCbcCipher(aesContextPtr->keySize);
    if (pCipher == NULL)
    {
    	teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    int ret = EVP_DecryptInit(pCtx, pCipher, aesContextPtr->key, aesContextPtr->iv);
    if (!ret)
	{
		AESDBG("%s: EVP_DecryptInit failed\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
	}

    // do the decrypt; must keep track of the number of bytes produced
    int nBytes = 0;
    ret = EVP_DecryptUpdate(pCtx, *clear, &nBytes, encrypted, encryptedLength);
    if (!ret)
	{
    	AESDBG("%s: EVP_DecryptUpdate failed\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
	}

    int nTotalBytes = nBytes;
    ret = EVP_DecryptFinal(pCtx, &((*clear)[nBytes]), &nBytes);
    if (!ret)
    {
		AESDBG("%s: padding incorrect in decryptfinal\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }
    nTotalBytes += nBytes;

    // done with Ctx, free it
    EVP_CIPHER_CTX_free(pCtx);

    // the actual output size is in nTotalBytes
	if (nTotalBytes == 0)
	{
		AESDBG("%s: total 0 bytes decrypted\n", __FUNCTION__ );
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}

	// resize decrypted
    *clear = (unsigned char*) teeMemRealloc(*clear, nTotalBytes+1);
    if (*clear == NULL)
    {
    	AESDBG("%s: Out of memory, cannot reallocate for clear\n", __FUNCTION__ );
    	teeMemFree(clear);
    	return nfTCR_OUT_OF_MEMORY;
    }
    (*clear)[nTotalBytes] = '\0';

    return nfTGR_SUCCESS;
}
#endif

nfTeeCryptoResult_t teeAesGcmEncrypt(const unsigned char 	*clear,
									uint32_t				clearLength,
									const unsigned char 	*aad,
									uint32_t				aadLength,
									const nfAesCipherContext_t 	*aesContextPtr,
									unsigned char	 		**encrypted,
									unsigned char 			**mac)
{
#ifndef EVP_CTRL_GCM_SET_IVLEN
	(void) clear;
	(void) aad;
	(void) aesContextPtr;
	(void) encrypted;
	(void) mac;
	AESDBG("%s: ALERT: open ssl library does not support AES-GCM, returning error\n", __FUNCTION__ );
	return nfTCR_INTERNAL_ERROR;
#else
	//validate arguments
	if ((aesContextPtr == NULL) || (aad == NULL) || (clear == NULL) || (encrypted == NULL) || (mac == NULL))
	{
		AESDBG("%s: required Args are NULL\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if (aesContextPtr->cipherType != nfAES_GCM)
	{
		AESDBG("%s: aesContextPtr was not initialized for GCM cipher\n", __FUNCTION__ );
		return nfTCR_MALFORMED_DATA;
	}
	if ((aadLength == 0) || (clearLength == 0))
	{
		AESDBG("%s: required Args are empty (size 0)\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}

	// get encrypted size
	int encSize = teeAesGetOutSize (clearLength);
	// allocate memory for encrypted
    *encrypted = (unsigned char*) (teeMemAlloc(encSize));
    if (*encrypted == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for encrypted\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}


	// initialize encryption
    EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* pCipher = teeAesGcmCipher(aesContextPtr->keySize);
    if (pCipher == NULL)
    {
    	teeMemFree(encrypted);
    	EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set cipher
    int ret = EVP_EncryptInit(pCtx, pCipher, NULL, NULL);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptInit failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set iv length
    ret = EVP_CIPHER_CTX_ctrl(pCtx, EVP_CTRL_GCM_SET_IVLEN, aesContextPtr->ivSize, NULL);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptInit failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set key and iv
    ret = EVP_EncryptInit(pCtx, NULL, aesContextPtr->key, aesContextPtr->iv);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptInit failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // add Additional Authenticated Data (aad)
    int nBytes = 0;
    ret = EVP_EncryptUpdate(pCtx, NULL, &nBytes, aad, aadLength);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptUpdate failed when setting aad\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }


    // do the encrypt
    nBytes = 0;
    ret = EVP_EncryptUpdate(pCtx, *encrypted, &nBytes, clear, clearLength);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptUpdate failed during encrypt\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // finalization
    int nTotalBytes = nBytes;
    ret = EVP_EncryptFinal(pCtx, *encrypted, &nBytes);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptFinal failed\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }
    nTotalBytes += nBytes;

    // the actual output size is in nTotalBytes
    if (nTotalBytes == 0)
    {
    	AESDBG("%s: total 0 bytes encrypted\n", __FUNCTION__ );
    	teeMemFree(encrypted);
    	EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // resize encrypted
    *encrypted = (unsigned char*) teeMemRealloc(*encrypted, nTotalBytes+1);
    if (*encrypted == NULL)
    {
    	AESDBG("%s: Out of memory, cannot reallocate for encrypted\n", __FUNCTION__ );
    	teeMemFree(encrypted);
    	EVP_CIPHER_CTX_free(pCtx);
    	return nfTCR_OUT_OF_MEMORY;
    }
    (*encrypted)[nTotalBytes] = '\0';

    // get the tag (MAC)
    int macSize = 16;	// always want a 128-bit tag
    *mac = (unsigned char*) (teeMemAlloc(macSize+1));
	if (*mac == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for mac\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_OUT_OF_MEMORY;
	}
    ret = EVP_CIPHER_CTX_ctrl(pCtx, EVP_CTRL_GCM_GET_TAG, macSize, *mac);
    if (!ret)
    {
		AESDBG("%s: EVP_CIPHER_CTX_ctrl fail while getting mac\n", __FUNCTION__ );
		teeMemFree(encrypted);
		EVP_CIPHER_CTX_free(pCtx);
		teeMemFree(mac);
		return nfTCR_OUT_OF_MEMORY;
    }

    // done with Ctx, free it
    EVP_CIPHER_CTX_free(pCtx);

    return nfTGR_SUCCESS;
#endif
}

nfTeeCryptoResult_t teeAesGcmDecrypt(const unsigned char		*encrypted,
									uint32_t				encryptedLength,
									const unsigned char 	*mac,
									uint32_t				macLength,
									const unsigned char 	*aad,
									uint32_t				aadLength,
									const nfAesCipherContext_t	*aesContextPtr,
									unsigned char			**clear)
{
#ifndef EVP_CTRL_GCM_SET_IVLEN
	(void) clear;
	(void) aad;
	(void) aesContextPtr;
	(void) encrypted;
	(void) mac;
	AESDBG("%s: ALERT: open ssl library does not support AES-GCM, returning error\n", __FUNCTION__ );
	return nfTCR_INTERNAL_ERROR;
#else
	//validate arguments
	if ((aesContextPtr == NULL) || (aad == NULL) || (mac == NULL) || (encrypted == NULL) || (clear == NULL))
	{
		AESDBG("%s: required Args are NULL\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if (aesContextPtr->cipherType != nfAES_GCM)
	{
		AESDBG("%s: aesContextPtr was not initialized for GCM cipher\n", __FUNCTION__ );
		return nfTCR_MALFORMED_DATA;
	}
	if ((encryptedLength == 0) || (macLength == 0) || (aadLength == 0))
	{
		AESDBG("%s: required Args are empty (size 0)\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}

	// get clear size
	int clearSize = teeAesGetOutSize (encryptedLength);
	// allocate memory for clear
    *clear = (unsigned char*) (teeMemAlloc(clearSize));
    if (*clear == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for clear\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}

	// initialize decryption
    EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* pCipher = teeAesGcmCipher(aesContextPtr->keySize);
    if (pCipher == NULL)
    {
    	teeMemFree(clear);
    	EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set cipher
    int ret = EVP_DecryptInit(pCtx, pCipher, NULL, NULL);
    if (!ret)
    {
		AESDBG("%s: EVP_DecryptInit failed\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }
    AESDBG("%s: block size of cipher is %d\n", __FUNCTION__, EVP_CIPHER_block_size(pCipher));

    // set iv length
    ret = EVP_CIPHER_CTX_ctrl(pCtx, EVP_CTRL_GCM_SET_IVLEN, aesContextPtr->ivSize, NULL);
    AESDBG("%s: EVP_CIPHER_CTX_ctrl set iv len to %d\n", __FUNCTION__, aesContextPtr->ivSize);
    if (!ret)
    {
		AESDBG("%s: EVP_DecryptInit failed\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set key and iv
    ret = EVP_DecryptInit(pCtx, NULL, aesContextPtr->key, aesContextPtr->iv);
    if (!ret)
    {
		AESDBG("%s: EVP_DecryptInit failed\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // set authentication tag (MAC)
    ret = EVP_CIPHER_CTX_ctrl(pCtx, EVP_CTRL_GCM_SET_TAG, macLength, (void*) mac);
    AESDBG("%s: EVP_CIPHER_CTX_ctrl set mac len to %d\n", __FUNCTION__, macLength);
    if (!ret)
    {
		AESDBG("%s: EVP_CIPHER_CTX_ctrl EVP_CTRL_GCM_SET_TAG fail\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // add Additional Authenticated Data (aad)
    int nBytes = 0;
    ret = EVP_DecryptUpdate(pCtx, NULL, &nBytes, aad, aadLength);
    AESDBG("%s: EVP_DecryptUpdate for AAD returned %d, nBytes is %d\n", __FUNCTION__, ret, nBytes);
    if (!ret)
    {
		AESDBG("%s: EVP_EncryptUpdate failed when setting aad\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // do the decrypt
    nBytes = 0;
    ret = EVP_DecryptUpdate(pCtx, *clear, &nBytes, encrypted, encryptedLength);
    AESDBG("%s: EVP_DecryptUpdate for data/decrypt returned %d, nBytes is %d\n", __FUNCTION__, ret, nBytes);
    //(void) fwrite(*clear , 1 , nBytes , stderr);

    if (!ret)
    {
		AESDBG("%s: EVP_DecryptUpdate failed during decrypt\n", __FUNCTION__ );
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }

    // finalize and authenticate
    int nTotalBytes = nBytes;
    ret = EVP_DecryptFinal(pCtx, *clear, &nBytes);
    AESDBG("%s: EVP_DecryptFinal returned %d, nBytes is %d\n", __FUNCTION__, ret, nBytes);
    if (!ret)
    {
		AESDBG("%s: EVP_DecryptFinal failed, returned %d\n", __FUNCTION__, ret);
		teeMemFree(clear);
		EVP_CIPHER_CTX_free(pCtx);
		return nfTCR_INTERNAL_ERROR;
    }
    nTotalBytes += nBytes;

    // done with Ctx, free it
    EVP_CIPHER_CTX_free(pCtx);

    // the actual output size is in nTotalBytes
	if (nTotalBytes == 0)
	{
		AESDBG("%s: total 0 bytes decrypted\n", __FUNCTION__ );
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}

	// resize decrypted
    *clear = (unsigned char*) teeMemRealloc(*clear, nTotalBytes+1);
    if (*clear == NULL)
    {
    	AESDBG("%s: Out of memory, cannot reallocate for clear\n", __FUNCTION__ );
    	teeMemFree(clear);
    	return nfTCR_OUT_OF_MEMORY;
    }
    (*clear)[nTotalBytes] = '\0';

    return nfTGR_SUCCESS;
#endif
}
