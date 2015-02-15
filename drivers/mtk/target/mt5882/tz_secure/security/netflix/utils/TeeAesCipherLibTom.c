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
#include "CommonIncludes.h"

#define AES_BLOCK_SIZE_		(16)


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
 #if 0
static int teeAesGetOutSize_libtom(int inSize)
{
    // according to the openssl docs:
    // the amount of data written out may be as large as (inputsize + cipher_block_size - 1)
	int outSize = inSize + AES_BLOCK_SIZE_ - 1;
    // the output buffer must also be a multiple of blocksize
    if ((outSize % AES_BLOCK_SIZE_) != 0)
    	outSize = ((outSize / AES_BLOCK_SIZE_) + 1) * AES_BLOCK_SIZE_;
	return outSize;
}
#endif
nfTeeCryptoResult_t teeAesCipherInit_libtom ( nfAesCipher_t 		cipherType,
										const unsigned char *key,
										uint32_t 			keySize,
										const unsigned char *iv,
										uint32_t 			ivSize,
										nfAesCipherContext_t  	**aesContextPtrPtr)
{
	//if cipherType is GCM then make sure openssl library supports GCM, ASSERT otherwise
	if (cipherType == nfAES_GCM)
	{

	}

	// check for bad args
	if ((key == NULL) || (keySize == 0) || (iv == NULL) || (ivSize == 0))
	{
		AESDBG("%s: required Args are NULL or 0\n", __FUNCTION__ );
		return nfTCR_BAD_PARAMETER;
	}
	if ((keySize != 16) && (keySize != 24) && (keySize != 32))
	{
		//printf("%s: key size is wrong, its %d, should be 16, 24 or 32", __FUNCTION__, keySize);
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
		teeAesCipherTerm_libtom(*aesContextPtrPtr);
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
		teeAesCipherTerm_libtom(*aesContextPtrPtr);
		return nfTCR_OUT_OF_MEMORY;
	}
	teeMemcpy((*aesContextPtrPtr)->iv, iv, ivSize);
	((*aesContextPtrPtr)->iv)[ivSize] = '\0';

	//assign ivsize
	(*aesContextPtrPtr)->ivSize = ivSize;

	// return success
	return nfTGR_SUCCESS;
}

void teeAesCipherTerm_libtom(nfAesCipherContext_t *aesContextPtr)
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


nfTeeCryptoResult_t teeAesGcmDecrypt_libtom(const unsigned char		*encrypted,
									uint32_t				encryptedLength,
									const unsigned char 	*mac,
									uint32_t				macLength,
									const unsigned char 	*aad,
									uint32_t				aadLength,
									const nfAesCipherContext_t	*aesContextPtr,
									unsigned char			**clear)
{
	#if 0
	uint32_t cipher_index = -1;
	uint32_t i4_ret = -1;
	int err;
	gcm_state gcm ;
	unsigned char tag[128];
	uint32_t taglen = 0;
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
	int clearSize = teeAesGetOutSize_libtom(encryptedLength);
	// allocate memory for clear
    *clear = (unsigned char*) (teeMemAlloc(clearSize));
    if (*clear == NULL)
	{
		AESDBG("%s: Out of memory, cannot allocate for clear\n", __FUNCTION__ );
		return nfTCR_OUT_OF_MEMORY;
	}

	i4_ret = register_cipher(&aes_desc);
	if(i4_ret < 0)
	{
		//printf("[trustzone],register cipher fail .\n");
		teeMemFree(clear);
		return nfTCR_CIPHER_ERROR;
	}
	cipher_index = find_cipher("aes");
	if(cipher_index < 0)
	{	
		//printf("[trustzone],find cipher aes fail .\n");
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}	
	if ((err = gcm_init(&gcm, cipher_index, aesContextPtr->key, aesContextPtr->keySize)) != CRYPT_OK) 
	{
		//fprintf(stderr, "gcm_init: %s\n", error_to_string(err)); 
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if ((err = gcm_reset(&gcm)) != CRYPT_OK) {
		//fprintf(stderr, "\nGCM error[%d]... %s\n", __LINE__, error_to_string(err));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if ((err = gcm_add_iv(&gcm, aesContextPtr->iv, aesContextPtr->ivSize)) != CRYPT_OK) {
		//fprintf(stderr, "\nGCM error[%d]... %s\n", __LINE__, error_to_string(err));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if ((err = gcm_add_aad(&gcm, aad, aadLength)) != CRYPT_OK) {
		//fprintf(stderr, "\nGCM error[%d]... %s\n", __LINE__, error_to_string(err));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if ((err = gcm_process(&gcm, *clear, encryptedLength, encrypted, GCM_DECRYPT)) != CRYPT_OK) {
		//fprintf(stderr, "\nGCM error[%d]... %s\n", __LINE__, error_to_string(err));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if(sizeof(macLength) > sizeof(tag))
	{
		//fprintf(stderr, "\nGCM error[%d]... %s,%d.\n", __LINE__, "macLength buffer is large than ",sizeof(tag));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	teeBzero(tag,sizeof(tag));
	taglen = macLength;
	if ((err = gcm_done(&gcm, tag, &taglen)) != CRYPT_OK) {
		//fprintf(stderr, "\nGCM error[%d]... %s\n", __LINE__, error_to_string(err));
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	if((macLength != taglen) && (0 != teeMemcmp(tag,mac,macLength)))
	{
		//fprintf(stderr, "\nGCM error[%d]... %s.\n", __LINE__, "authication fail");
		teeMemFree(clear);
		return nfTCR_INTERNAL_ERROR;
	}
	(*clear)[clearSize] = '\0';
	#endif
	return nfTGR_SUCCESS;
	
}
