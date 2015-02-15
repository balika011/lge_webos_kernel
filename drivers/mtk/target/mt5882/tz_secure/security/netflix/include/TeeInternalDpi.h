/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEINTERNALDPI_H_
#define TEEINTERNALDPI_H_

#include "CommonIncludes.h"

// function prototypes




/**********************************************/
/*************** From teeUtils.c **************/
/**********************************************/

/***************************************************************************
 * @function teePrintHexDump
 *
 * @brief formatted hex dump to stdout
 *
 * @param[in] BufferPtr - pointer to buffer
 * @param[in] BufferLength - length of buffer
 *
 * @return   void
 *
 **************************************************************************/
void teePrintHexDump( uint8_t *BufferPtr, uint32_t BufferLength );
void teePrintHexDump_tag(char* name ,uint8_t *BufferPtr, uint32_t BufferLength );

/***************************************************************************
 * @function teeNtohl
 *
 * @brief non-libc ntohl
 *
 * @param[in] NetLong - long in network byte order
 *
 * @returns   uint32_t size
 *
 **************************************************************************/
uint32_t teeNtohl(const uint32_t NetLong);

/***************************************************************************
 * @function teeHtonl
 *
 * @brief non-libc htonl
 *
 * @param[in] HostLong - long in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint32_t teeHtonl(const uint32_t HostLong);

/***************************************************************************
 * @function teeNtohs
 *
 * @brief non-libc ntohs
 *
 * @param[in] NetShort - short in network byte order
 *
 * @returns   value in network byte order
 *
 **************************************************************************/
uint16_t teeNtohs(const uint16_t NetShort);

/***************************************************************************
 * @function teeHtons
 *
 * @brief non-libc htons
 *
 * @param[in] HostShort - short in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint16_t teeHtons(const uint32_t HostShort);


/***************************************************************************
 * @function teeMemcmp
 *
 * @brief non-libc memcmp
 *
 * @param[in] Ptr1 - pointer to first location
 * @param[in] Ptr2 - pointer to second location
 * @param[in] Length - length to compare
 *
 * @return   -1 if *Ptr1 < *Ptr2, 0 if equal, 1 if *Ptr1 > *Ptr2
 *
 **************************************************************************/
int teeMemcmp( const void *Ptr1, const void *Ptr2, uint32_t Length );


/***************************************************************************
 * @function teeMemcpy
 *
 * @brief non-libc memcpy
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] SrcPtr - pointer to source
 * @param[in] Length - length to copy
 *
 * @returns   DstPtr
 *
 **************************************************************************/
void *teeMemcpy( void *DstPtr, const void *SrcPtr, uint32_t Length );

/***************************************************************************
 * @function teeBzero
 *
 * @brief non-libc bzero
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Length - length to zeroize
 *
 * @returns  void
 *
 **************************************************************************/
void teeBzero( void *DstPtr, size_t Length );

/***************************************************************************
 * @function teeMemset
 *
 * @brief non-libc memset
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Value - value to set
 * @param[in] Length - length
 *
 * @returns   DstPtr
 *
 **************************************************************************/
void *teeMemset( void *DstPtr, int Value, uint32_t Length );

/***************************************************************************
 * @function teeMemAlloc
 *
 * @brief non-libc malloc
 *
 * @param[in] Size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemAlloc( uint32_t Size );

/***************************************************************************
 * @function teeMemRealloc
 *
 * @brief non-libc realloc
 *
 * @param[in] ptr  - pointer which needs reallocate
 * @param[in] size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemRealloc( uint8_t * ptr, uint32_t Size );

/***************************************************************************
 * @function teeMemFree
 *
 * @brief non-libc free
 *
 * @param[in] MemPtrPtr - pointer to pointer to memory you want freed
 *
 * @returns   void
 *
 **************************************************************************/
void teeMemFree( uint8_t **MemPtrPtr );

/***************************************************************************
 * @function teeStrlen
 *
 * @brief non-libc strlen
 *
 * @param[in] str - pointer to string
 *
 * @returns   int size
 *
 **************************************************************************/
int teeStrlen (const char* str);


/***********************************************/
/*************** From teeCrypto.c **************/
/***********************************************/

/*********************************************************************
 * @function teeSha384
 *
 * @brief  Do Sha384 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ResultPtr - pointer to buffer for result
 *
 * @returns 0 always (asserts otherwise)
 *
 *********************************************************************/
int teeSha384( uint8_t     *DataPtr,
              uint32_t    DataLength,
              uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeSha1
 *
 * @brief  Do Sha1 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ResultPtr - pointer to buffer for result
 *
 * @returns 0 always (asserts otherwise)
 *
 *********************************************************************/
int teeSha1( uint8_t     *DataPtr,
            uint32_t    DataLength,
            uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeValidateSha1
 *
 * @brief  Validate Sha1 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ExpectedResult - buffer containing result
 *
 * @returns 0 if SHA1 is valid
 *          -1 otherwise
 *
 *********************************************************************/
int teeValidateSha1( uint8_t     *DataPtr,
                    uint32_t    DataLength,
                    uint8_t     ExpectedResult[] );


/*********************************************************************
 * @function teeComputeClientAes128Key
 *
 * @brief Generate client secure store AES-128 key from client ID
 *
 * @param[in] ClientID - array containing client ID
 * @param[in] Aes128Key - array where you want AES key placed
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeComputeClientAes128Key( uint8_t ClientID[],
							  uint8_t Aes128Key[],
							  uint8_t KeyLadderKey[] );


/*********************************************************************
 * @function teeAesCbc128InPlaceEncryptZeroIV
 *
 * @brief  Do AES 128 CBC in-place encryption on passed buffer
 *           that has already been padded (if necessary)
 *
 * @param[in] CleartextPtr - pointer to cleartext
 * @param[in] CleartextLength - length of cleartext
 * @param[in] AesKeyPtr - pointer to 128-bit AES key
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAesCbc128EncryptZeroIV( uint8_t     *CleartextPtr,
                              uint32_t    CleartextLength,
                              uint8_t     *AesKeyPtr );

/*********************************************************************
 * @function teeAesCbc128InPlaceDecryptZeroIV
 *
 * @brief  Do AES 128 CBC in-place decryption on passed buffer
 *
 * @param[in] CiphertextPtr - pointer to ciphertext
 * @param[in] CiphertextLength - length of ciphertext
 * @param[in] AesKeyPtr - pointer to 128-bit AES key
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAesCbc128DecryptZeroIV( uint8_t     *CiphertextPtr,
                              uint32_t    CiphertextLength,
                              uint8_t     *AesKeyPtr );

/*********************************************************************
 * @function teeHmacSha256
 *
 * @brief  Do HmacSha256 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * Outputs: Writes HMAC value at *ResltPtr
 *
 *
 * @returns Length of HMAC (256/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha256( uint8_t     *DataPtr,
                  uint32_t    DataLength,
                  uint8_t     *KeyPtr,
                  uint32_t    KeyLength,
                  uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeHmacSha384
 *
 * @brief  Do HmacSha384 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * Outputs: Writes HMAC value at *ResltPtr
 *
 *
 * @returns Length of HMAC (384/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha384( uint8_t     *DataPtr,
                  uint32_t    DataLength,
                  uint8_t     *KeyPtr,
                  uint32_t    KeyLength,
                  uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeHmacSha512
 *
 * @brief  Do HmacSha512 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * Outputs: Writes HMAC value at *ResltPtr
 *
 *
 * @returns Length of HMAC (512/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha512( uint8_t     *DataPtr,
                  uint32_t    DataLength,
                  uint8_t     *KeyPtr,
                  uint32_t    KeyLength,
                  uint8_t     *ResultPtr );


/*********************************************************************
 * @function teeAes128CbcEncrypt
 *
 * @brief   handle a AES encryption request from REE
 *
 * @param[in] PlaintextPtr - pointer to plaintext
 * @param[in] PlaintextLen - length of message
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] IVPtr - pointer to AES Key
 * @param[out] CiphertextPtr - pointer to where ciphertext should go
 * @param[in] CiphertextMax - maximum available buffer space in *CiphertextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcEncrypt( uint8_t 	*PlaintextPtr,
					   	 uint32_t 	PlaintextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t	*CiphertextPtr,
						 uint32_t	CiphertextMax );

/*********************************************************************
 * @function teeAes128CbcDecrypt
 *
 * @brief   handle a AES Decryption request from REE
 *
 * @param[in] CiphertextPtr - pointer to ciphertext
 * @param[in] CiphertextLen - length of message
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] IVPtr - pointer to AES Key
 * @param[out] PlaintextPtr - pointer to where ciphertext should go
 * @param[in] PlaintextMax - maximum available buffer space in *PlaintextPtr
 *
 * @return Decrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcDecrypt( uint8_t 	*CiphertextPtr,
					     uint32_t 	CiphertextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t 	*PlaintextPtr,
						 uint32_t	PlaintextMax );

/*********************************************************************
 * @function teeCryptoRandomBytes
 *
 * @brief  get cryptographically random bytes from RNG
 *
 * @param[in] BufPtr - pointer to where you want data
 * @param[in] BufLength - how many bytes you want
 *
 * @returns 1 on success, 0 otherwise
 *
 *********************************************************************/
int teeCryptoRandomBytes( uint8_t     *BufPtr,
              	  	  	  uint32_t    BufLength );

/*********************************************************************
 * @function teePseudoRandomBytes
 *
 * @brief  get pseudorandom bytes from PRNG
 *
 * @param[in] BufPtr - pointer to where you want data
 * @param[in] BufLength - how many bytes you want
 *
 * @returns 1 on success, 0 otherwise
 *
 *********************************************************************/
int teePseudoRandomBytes( uint8_t     *BufPtr,
         	  	  	  	  uint32_t    BufLength );

/*********************************************************************
 * @function teeGenDHKeys
 *
 * @brief   handle a DH genkeys request from REE
 *
 * @param[in] Generator - the DH generator value
 * @param[in] ModulusLength - length of DH modulus
 * @param[in] ModulusPtr - pointer to DH modulus
 * @param[out] PrivKeyLengthPtr - pointer to where you want length of DH priv value
 * @param[in] PrivKeymax - maximum length of DH private value
 * @param[out] PrivKeyPtr - pointer to where you want DH priv key
 * @param[out] PubKeyLengthPtr - pointer to where you want length of DH pub value
 * @param[out] PubKeyPtr - pointer to where you want DH pub key
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeGenDhKeys( uint32_t		Generator,
		   	   	  uint32_t		ModulusLength,
		   	   	  uint8_t		*ModulusPtr,
		   	   	  uint32_t		*PrivKeyLengthPtr,
		   	   	  uint32_t		PrivKeyMax,
		   	   	  uint8_t		*PrivKeyPtr,
		   	   	  uint32_t		*PubKeyLengthPtr,
		   	   	  uint32_t		PubKeyMax,
		   	   	  uint8_t		*PubKeyPtr );

/*********************************************************************
 * @function teeComputeDhSharedSecret
 *
 * @brief   handle a DH genkeys request from REE
 *
 * @param[in] Generator - the DH generator value
 * @param[in] ModulusLength - length of DH modulus
 * @param[in] ModulusPtr - pointer to DH modulus
 * @param[in] PrivKeyLength - length of DH priv value
 * @param[in] PrivKeyPtr - pointer to where you want DH priv key
 * @param[in] PubKeyLength - peer DH pub value length
 * @param[in] PubKeyPtr - peer DH pub key
 * @param[in] MaxSharedSecretLength - max space at SharedSecretPtr
 * @param[out] SharedSecretLengthPtr - pointer to where you want ss length
 * @param[out] SharedSecretPtr - pointer to where you want shared secret
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeComputeDhSharedSecret( uint32_t 		Generator,
							  uint32_t		ModulusLength,
							  uint8_t		*ModulusPtr,
							  uint32_t		PrivKeyLength,
							  uint8_t		*PrivKeyPtr,
							  uint32_t		PubKeyLength,
							  uint8_t		*PubKeyPtr,
							  uint32_t		MaxSharedSecretLength,
							  uint32_t		*SharedSecretLengthPtr,
							  uint8_t		*SharedSecretPtr );

/*********************************************************************
 * @function teeCryptoShutdown
 *
 * @brief   TPI crypto cleanup, call upon system shutdown
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeCryptoShutdown();



/*****************************************************/
/*************** From teeCryptoLibTom.c **************/
/*****************************************************/

/*********************************************************************
 * @function teeCryptoRandomBytes
 *
 * @brief  get cryptographically random bytes from RNG
 *
 * @param[in] BufPtr - pointer to where you want data
 * @param[in] BufLength - how many bytes you want
 *
 * @returns 1 on success, 0 otherwise
 *
 *********************************************************************/
int teeCryptoRandomBytes_libtom( uint8_t     *BufPtr,
              	  	  	  			uint32_t    BufLength );

/*********************************************************************
 * @function teePseudoRandomBytes
 *
 * @brief  get pseudorandom bytes from PRNG
 *
 * @param[in] BufPtr - pointer to where you want data
 * @param[in] BufLength - how many bytes you want
 *
 * @returns 1 on success, 0 otherwise
 *
 *********************************************************************/
int teePseudoRandomBytes_libtom( uint8_t     *BufPtr,
         	  	  	  	  uint32_t    BufLength );

/*********************************************************************
 * @function teeSha384
 *
 * @brief  Do Sha384 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ResultPtr - pointer to buffer for result
 *
 * @returns 0 always (asserts otherwise)
 *
 *********************************************************************/
int teeSha384_libtom( uint8_t     *DataPtr,
              uint32_t    DataLength,
              uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeSha1
 *
 * @brief  Do Sha1 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ResultPtr - pointer to buffer for result
 *
 * @returns 0 always (asserts otherwise)
 *
 *********************************************************************/
int teeSha1_libtom( uint8_t     *DataPtr,
            				uint32_t    DataLength,
            				uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeValidateSha1
 *
 * @brief  Validate Sha1 on passed buffer
 *
 * @param[in] DataPtr - pointer to cleartext
 * @param[in] DataLength - length of cleartext
 * @param[in] ExpectedResult - buffer containing result
 *
 * @returns 0 if SHA1 is valid
 *          -1 otherwise
 *
 *********************************************************************/
int teeValidateSha1_libtom( uint8_t     *DataPtr,
                     				uint32_t    DataLength,
                     				uint8_t     ExpectedResult[] );

/*********************************************************************
 * @function teeComputeClientAes128Key_libtom
 *
 * @brief Generate client secure store AES-128 key from client ID
 *
 * @param[in] ClientID - array containing client ID
 * @param[in] Aes128Key - array where you want AES key placed
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeComputeClientAes128Key_libtom( uint8_t ClientID[], uint8_t Aes128Key[], uint8_t KeyLadderKey[] );

/*********************************************************************
 * @function teeAesCbc128EncryptZeroIV_libtom
 *
 * @brief  Do AES 128 CBC in-place encryption on passed buffer
 *           that has already been padded (if necessary)
 *
 * @param[in] CleartextPtr - pointer to cleartext
 * @param[in] CleartextLength - length of cleartext
 * @param[in] AesKeyPtr - pointer to 128-bit AES key
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAesCbc128EncryptZeroIV_libtom( uint8_t     *CleartextPtr,
                               uint32_t    CleartextLength,
                               uint8_t     *AesKeyPtr );

/*********************************************************************
 * @function teeAesCbc128DecryptZeroIV_libtom
 *
 * @brief  Do AES 128 CBC in-place decryption on passed buffer
 *
 * @param[in] CiphertextPtr - pointer to ciphertext
 * @param[in] CiphertextLength - length of ciphertext
 * @param[in] AesKeyPtr - pointer to 128-bit AES key
 *
 * @returns 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAesCbc128DecryptZeroIV_libtom( uint8_t     *CiphertextPtr,
                              uint32_t    CiphertextLength,
                              uint8_t     *AesKeyPtr );

/*********************************************************************
 * @function teeHmacSha256_libtom
 *
 * @brief  Do HmacSha256 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * @returns Length of HMAC (256/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha256_libtom( uint8_t     *DataPtr,
                   				uint32_t    DataLength,
                   				uint8_t     *KeyPtr,
                   				uint32_t    KeyLength,
                   				uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeHmacSha384_libtom
 *
 * @brief  Do HmacSha384 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * Outputs: Writes HMAC value at *ResltPtr
 *
 *
 * @returns Length of HMAC (384/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha384_libtom( uint8_t     *DataPtr,
                  uint32_t    DataLength,
                  uint8_t     *KeyPtr,
                  uint32_t    KeyLength,
                  uint8_t     *ResultPtr );

/*********************************************************************
 * @function teeHmacSha512
 *
 * @brief  Do HmacSha512 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          DataLength - length of cleartext
 *          ResultPtr - pointer to buffer for result
 *
 * Outputs: Writes HMAC value at *ResltPtr
 *
 *
 * @returns Length of HMAC (512/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/
int teeHmacSha512_libtom( uint8_t     *DataPtr,
                				  uint32_t    DataLength,
                  				uint8_t     *KeyPtr,
                 					uint32_t    KeyLength,
                  				uint8_t     *ResultPtr );


/*********************************************************************
 * @function teeAes128CbcEncrypt_libtom
 *
 * @brief   handle a AES encryption request from REE
 *
 * @param[in] PlaintextPtr - pointer to plaintext
 * @param[in] PlaintextLen - length of message
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] IVPtr - pointer to AES Key
 * @param[out] CiphertextPtr - pointer to where ciphertext should go
 * @param[in] CiphertextMax - maximum available buffer space in *CiphertextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcEncrypt_libtom( uint8_t 	*PlaintextPtr,
					   	 uint32_t 	PlaintextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t	*CiphertextPtr,
						 uint32_t	CiphertextMax );


/*********************************************************************
 * @function teeAes128CbcDecrypt
 *
 * @brief   handle a AES Decryption request from REE
 *
 * @param[in] CiphertextPtr - pointer to ciphertext
 * @param[in] CiphertextLen - length of message
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] IVPtr - pointer to AES Key
 * @param[out] PlaintextPtr - pointer to where ciphertext should go
 * @param[in] PlaintextMax - maximum available buffer space in *PlaintextPtr
 *
 * @return Decrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcDecrypt_libtom( uint8_t 	*CiphertextPtr,
					     uint32_t 	CiphertextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t 	*PlaintextPtr,
						 uint32_t	PlaintextMax );

int teeAes128CbcDecrypt_no_remove_padding_libtom( uint8_t 	*CiphertextPtr,
					     uint32_t 	CiphertextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t 	*PlaintextPtr,
						 uint32_t	PlaintextMax );


int teeGenDhKeys_libtom( uint32_t		Generator,
		   	   	  uint32_t		ModulusLength,
		   	   	  uint8_t		*ModulusPtr,
		   	   	  uint32_t		*PrivKeyLengthPtr,
		   	   	  uint32_t		PrivKeyMax,
		   	   	  uint8_t		*PrivKeyPtr,
		   	   	  uint32_t		*PubKeyLengthPtr,
		   	   	  uint32_t		PubKeyMax,
		   	   	  uint8_t		*PubKeyPtr );



int teeGenDhKeys_libtom_test( uint32_t		Generator,
		   	   	  uint32_t		ModulusLength,
		   	   	  uint8_t		*ModulusPtr,
		   	   	  uint32_t		*PrivKeyLengthPtr,
		   	   	  uint32_t		PrivKeyMax,
		   	   	  uint8_t		*PrivKeyPtr,
		   	   	  uint32_t		*PubKeyLengthPtr,
		   	   	  uint32_t		PubKeyMax,
		   	   	  uint8_t		*PubKeyPtr );
		   	   	  


/*********************************************************************
 * @function teeComputeDhSharedSecret_libtom
 *
 * @brief   handle a DH genkeys request from REE
 *
 * @param[in] Generator - the DH generator value
 * @param[in] ModulusLength - length of DH modulus
 * @param[in] ModulusPtr - pointer to DH modulus
 * @param[in] PrivKeyLength - length of DH priv value
 * @param[in] PrivKeyPtr - pointer to where you want DH priv key
 * @param[in] PubKeyLength - peer DH pub value length
 * @param[in] PubKeyPtr - peer DH pub key
 * @param[in] MaxSharedSecretLength - max space at SharedSecretPtr
 * @param[out] SharedSecretLengthPtr - pointer to where you want ss length
 * @param[out] SharedSecretPtr - pointer to where you want shared secret
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeComputeDhSharedSecret_libtom( uint32_t 		Generator,
							  uint32_t		ModulusLength,
							  uint8_t		*ModulusPtr,
							  uint32_t		PrivKeyLength,
							  uint8_t		*PrivKeyPtr,
							  uint32_t		PubKeyLength,
							  uint8_t		*PubKeyPtr,
							  uint32_t		MaxSharedSecretLength,
							  uint32_t		*SharedSecretLengthPtr,
							  uint8_t		*SharedSecretPtr );


int teeCryptoShutdown_libtom();

/*********************************************************************
 * @function teeHmacSha256Init_libtom
 *
 * @brief   handle a hmac init request from REE
 *
 * @param[in] KeyPtr - pointer to HMAC Key
 *
 * @return pointer to crypto context if everything okay, -1 otherwise
 *
 *********************************************************************/
void *teeHmacSha256Init_libtom( uint8_t *KeyPtr );

/*********************************************************************
 * @function teeHmacSha256Update_libtom
 *
 * @brief   handle a hmac update request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[in] DataPtr - pointer to data you want hmac'd
 * @param[in] DataLen - length of message
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeHmacSha256Update_libtom( void		*CtxtPtr,
					   	 uint8_t 	*DataPtr,
					   	 uint32_t	DataLen );

/*********************************************************************
 * @function teeHmacSha256Final_libtom
 *
 * @brief   handle a hmac final request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[in] HmacPtr - pointer to Where you want hmac stored
 * @param[in] DataLen - maximum length of hmac
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeHmacSha256Final_libtom( void		*CtxtPtr,
					   	 uint8_t 	*HmacPtr,
					   	 uint32_t	HmacLen );

/*********************************************************************
 * @function teeHmacSha256Cleanup_libtom
 *
 * @brief   hmac context cleanup
 *
 * @param[in] CtxtPtr - pointer to hmac context
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeHmacSha256Cleanup_libtom( void  *CtxtPtr );


/*********************************************************************
 * @function teeAes128CbcDecryptInit_libtom
 *
 * @brief   handle a AES decrypt init request from REE
 *
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] KeyPtr - pointer to AES Key
 *
 * @return pointer to crypto context if everything okay, -1 otherwise
 *
 *********************************************************************/
void *teeAes128CbcDecryptInit_libtom( uint8_t  *IVPtr,
						 	   uint8_t	*KeyPtr );

/*********************************************************************
 * @function teeAes128CbcDecryptUpdate_libtom
 *
 * @brief   handle a AES decrypt update request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[in] CiphertextPtr - pointer to ciphertext
 * @param[in] CiphertextLen - length of message
 * @param[out] PlaintextPtr - pointer to where plaintext should go
 * @param[in] PlaintextMax - maximum available buffer space in *PlaintextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcDecryptUpdate_libtom( void		*CtxtPtr,
							   uint8_t	*CiphertextPtr,
							   uint32_t	CiphertextLen,
							   uint8_t 	*PlaintextPtr,
							   uint32_t	PlaintextMax );

/*********************************************************************
 * @function teeAes128CbcDecryptFinal_libtom
 *
 * @brief   handle a AES decrypt-final request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[out] PlaintextPtr - pointer to where plaintext should go
 * @param[in] PlaintextMax - maximum available buffer space in *PlaintextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeAes128CbcDecryptFinal_libtom( void		*CtxtPtr,
							  uint8_t	*PlaintextPtr,
							  uint32_t	PlaintextMax );



/*********************************************************************
 * @function teeAes128CbcEncryptInit_libtom
 *
 * @brief   handle a AES encrypt init request from REE
 *
 * @param[in] IVPtr - pointer to AES IV
 * @param[in] KeyPtr - pointer to AES Key
 *
 * @return pointer to crypto context if everything okay, -1 otherwise
 *
 *********************************************************************/
void *teeAes128CbcEncryptInit_libtom( uint8_t  *IVPtr,
						 	   uint8_t	*KeyPtr );

/*********************************************************************
 * @function teeAes128CbcEncryptUpdate_libtom
 *
 * @brief   handle a AES encrypt update request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[in] PlaintextPtr - pointer to plaintext
 * @param[in] PlaintextLen - length of message
 * @param[out] CiphertextPtr - pointer to where ciphertext should go
 * @param[in] CiphertextMax - maximum available buffer space in *CiphertextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
uint32_t teeAes128CbcEncryptUpdate_libtom( void		*CtxtPtr,
							   uint8_t 	*PlaintextPtr,
							   uint32_t	PlaintextLen,
							   uint8_t	*CiphertextPtr,
							   uint32_t	CiphertextMax );
/*********************************************************************
 * @function teeAes128CbcEncryptFinal
 *
 * @brief   handle a AES encrypt-final request from REE
 *
 * @param[in] CtxtPtr - pointer to previously init'd cipher context
 * @param[out] CiphertextPtr - pointer to where ciphertext should go
 * @param[in] CiphertextMax - maximum available buffer space in *CiphertextPtr
 *
 * @return encrypted data length if everything okay, -1 otherwise
 *
 *********************************************************************/
uint32_t teeAes128CbcEncryptFinal_libtom( void		*CtxtPtr,
							  uint8_t	*CiphertextPtr,
							  uint32_t	CiphertextMax );

/*********************************************************************
 * @function teeAes128CbcEncryptCleanup_libtom
 *
 * @brief   encryption context cleanup
 *
 * @param[in] CtxtPtr - pointer to EVP context
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
uint32_t teeAes128CbcEncryptCleanup_libtom( void  *CtxtPtr );




uint32_t aes_ecb_128_decrypt(const uint8_t* pul_key ,int ul_key_len,const uint8_t* pul_in_data ,uint32_t ul_in_data_len ,uint8_t* pul_out_data);


uint32_t i4_tee_nfx_encrypt(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
									const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t i4Dst_max_len ,/*out*/uint32_t *u4Dst_real_len);


uint32_t i4_tee_nfx_decrypt(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
								const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t u4DstLen,/*out*/uint32_t *u4Des_real_len);


uint32_t i4_tee_nfx_decrypt_no_del_rear_padding(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
								const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t u4DstLen,/*out*/uint32_t *u4Des_real_len);


uint32_t aes_unwrap_key(const uint8_t *key, const uint8_t *iv,uint8_t *out,const uint8_t *in, unsigned int inlen);

// vim: expandtab tabstop=4 shiftwidth=4



#endif /* TEEINTERNALDPI_H_ */
