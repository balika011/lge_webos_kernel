/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEAESCIPHER_H_
#define TEEAESCIPHER_H_

#include "CryptoTypes.h"


/*********************************************************************
 * @function nfAesCipherInit
 *
 * @brief initializes nfAesCipherContext_t based on input args
 *
 * @param[in] cipherType 			- type of AES cipher (CBC or GCM)
 * @param[in] key 					- key
 * @param[in] KeySize 				- size of key
 * @param[in] iv 					- initialization vector
 * @param[in] ivSize				- size of initialization vector
 * @param[out] aesContextPtrPtr  	- pointer to pointer of nfAesGcmContext_t
 * 									  that gets created
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeAesCipherInit ( nfAesCipher_t 		cipherType,
										const unsigned char *key,
										uint32_t 			keySize,
										const unsigned char *iv,
										uint32_t 			ivSize,
										nfAesCipherContext_t  	**aesContextPtrPtr);


/*********************************************************************
 * @function nfAesCipherTerm
 *
 * @brief terminates the nfAesCipherContext_t instance and frees all memory
 *
 * @param[in] aesContextPtr - pointer to nfAesCipherContext_t
 *
 * @returns void
 *
 *********************************************************************/
void teeAesCipherTerm(nfAesCipherContext_t  *aesContextPtr);


/*********************************************************************
 * @function nfAesCbcEncrypt
 *
 * @brief performs AES-CBC encryption on given clear text
 *
 * @param[in] clear 			- clear string/text to encrypt
 * @param[in] clearLength		- length of clear string/text
 * @param[in] aesContextPtr 	- pointer to nfAesCipherContext_t
 * @param[out] encrypted 		- pointer to encrypted text
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeAesCbcEncrypt (const unsigned char 	*clear,
									uint32_t				clearLength,
									const nfAesCipherContext_t 	*aesContextPtr,
									unsigned char	 		**encrypted);


/*********************************************************************
 * @function nfAesCbcDecrypt
 *
 * @brief performs AES-CBC decryption on given encrypted text
 *
 * @param[in] encrypted 		- encrypted string/text to decrypt
 * @param[in] encryptedLength	- length of encrypted string/text
 * @param[in] aesContextPtr 	- pointer to nfAesCipherContext_t
 * @param[out] clear 			- pointer to clear text
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeAesCbcDecrypt(const unsigned char	*encrypted,
									uint32_t			encryptedLength,
									const nfAesCipherContext_t	*aesContextPtr,
									unsigned char			**clear);

/*********************************************************************
 * @function nfAesGcmEncrypt
 *
 * @brief performs AES-GCM encryption on given clear text
 *
 * @param[in] clear 			- clear string/text to encrypt
 * @param[in] clearLength		- length of clear string/text
 * @param[in] aad 				- additional authentication data
 * @param[in] aadLength			- length of clear additional authentication data
 * @param[in] aesContextPtr 	- pointer to nfAesCipherContext_t
 * @param[out] encrypted 		- pointer to encrypted text
 * @param[out] mac 				- integrity value of the encrypted string
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeAesGcmEncrypt(const unsigned char 	*clear,
									uint32_t				clearLength,
									const unsigned char 	*aad,
									uint32_t				aadLength,
									const nfAesCipherContext_t 	*aesContextPtr,
									unsigned char	 		**encrypted,
									unsigned char 			**mac);


/*********************************************************************
 * @function nfAesGcmDecrypt
 *
 * @brief performs AES-GCM decryption on given encrypted text
 *
 * @param[in] encrypted 		- encrypted string/text to decrypt
 * @param[in] encryptedLength	- length of encrypted string/text
 * @param[in] mac 				- integrity value string
 * @param[in] macLength			- length of integrity value string
 * @param[in] aad 				- additional authentication data
 * @param[in] aadLength			- length of additional authentication data
 * @param[in] aesContextPtr 	- pointer to nfAesCipherContext_t
 * @param[out] clear 			- pointer to clear text
 *
 * @returns nfTCR_OKAY, nfTCR_BAD_PARAMETER, nfTCR_OUT_OF_MEMORY
 *
 *********************************************************************/
nfTeeCryptoResult_t teeAesGcmDecrypt(const unsigned char		*encrypted,
									uint32_t				encryptedLength,
									const unsigned char 	*mac,
									uint32_t				macLength,
									const unsigned char 	*aad,
									uint32_t				aadLength,
									const nfAesCipherContext_t	*aesContextPtr,
									unsigned char			**clear);

#endif // TEEAESCIPHER_H_
