/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include "CommonIncludes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"

/*
 * Our TEE simulator uses OpenSSL, but in general, this would be too large
 * for implementation in the TEE. The internal APIs are designed to isolate
 * this, so that if you have crypto hardware, and/or a different library,
 * you could replace OpenSSL here.
 */
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/dh.h>

// we are pre-allocating 16K for in-place crypto ops
#define teeMAX_CRYPTO_BUF_SIZE	(16 * 1024)
static uint8_t saLocalCryptoBuf[teeMAX_CRYPTO_BUF_SIZE];

#define teeTC_SHA1_HASH_SIZE	20
#define teeTC_AES128_BLOCK_SIZE	16

#define tee_EV_CLEANUP
//#undef tee_EV_CLEANUP

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
              	  	  	  uint32_t    BufLength )
{
	assert(BufPtr);
	assert(BufLength);
	CRYPTODBG("%s: requesting %d random bytes...\n", __FUNCTION__, BufLength );

	return RAND_bytes( (unsigned char *)BufPtr, (int) BufLength );

}/* end teeCryptoRandomBytes */

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
         	  	  	  	  uint32_t    BufLength )
{
	assert(BufPtr);
	assert(BufLength);
	CRYPTODBG("%s: requesting %d pseudorandom bytes...\n", __FUNCTION__, BufLength );

	return RAND_pseudo_bytes( (unsigned char *)BufPtr, (int) BufLength );

}/* end teePseudoRandomBytes */

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
              uint8_t     *ResultPtr )
{

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(ResultPtr);

    // one-shot hash function
    SHA384(DataPtr, DataLength, ResultPtr);

    return 0;

}/* end teeSha384 */

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
            uint8_t     *ResultPtr )
{
    SHA_CTX    Sha1Context;

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(ResultPtr);

    //initialize hash function
    SHA1_Init(&Sha1Context);

    // add the input buffer for hash creation
    SHA1_Update(&Sha1Context, DataPtr, DataLength );

    // Finalize and save the hash data
    SHA1_Final(ResultPtr, &Sha1Context);

    return 0;

}/* end teeSha1 */

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
                     uint8_t     ExpectedResult[] )
{
    uint8_t     LocalResult[teeTC_SHA1_HASH_SIZE];
    int         rc;

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(ExpectedResult);

	CRYPTODBG("%s: called to validate %d-byte buffer at %p\n", __FUNCTION__, DataLength, DataPtr );

    if ( (rc = teeSha1(DataPtr, DataLength, LocalResult)) != 0 )
    {
        CRYPTODBG("%s: Sha1() failed.\n", __FUNCTION__ );
        return -1;
    }

    if ( teeMemcmp( ExpectedResult, LocalResult, teeTC_SHA1_HASH_SIZE ) != 0 )
    {
        CRYPTODBG("%s: Hash does not match\n", __FUNCTION__ );
        return -1;
    }

    CRYPTODBG("%s: hashes match.\n", __FUNCTION__ );

    return 0;

}/* end ValidateSha1 */

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
int teeComputeClientAes128Key( uint8_t ClientID[], uint8_t Aes128Key[], uint8_t KeyLadderKey[] )
{
    AES_KEY LocalKey;

    /*
     * To generate the client aes-128 key, we aes-128-ecb encrypt the
     * client ID with our secure store key.
     */
    (void) teeBzero(Aes128Key, 16);

//    CRYPTODBG("%s: here is the client ID:\n", __FUNCTION__ );
//    teeHexDump(ClientID, 16);

    (void) teeBzero(&LocalKey, sizeof(AES_KEY) );
    if ( AES_set_encrypt_key(KeyLadderKey, 128, &LocalKey) < 0 )
    {
        printf("%s; AES_set_encrypt_key() failed\n", __FUNCTION__);
        return -1;
    }

    AES_ecb_encrypt( ClientID, Aes128Key, &LocalKey, AES_ENCRYPT);

//    CRYPTODBG("%s: here is the client Key:\n", __FUNCTION__ );
//    teeHexDump(Aes128Key, 16);

#ifdef CHECK_CRYPTO
    {
        AES_KEY stKey;
        uint8_t buf[16];
        int len = 0;
        teeBzero(buf, 16);
        teeBzero(&stKey, sizeof(AES_KEY));

        if(AES_set_decrypt_key(KeyLadderKey, 128, &stKey)<0)
        {
            printf("%s: AES_set_decrypt_key() failed.\n", __FUNCTION__ );
            return -1;
        }
        AES_ecb_encrypt(Aes128Key, buf, &stKey, AES_DECRYPT);
        CRYPTODBG("%s: here is the decrypted key:\n", __FUNCTION__);
        teeHexDump(buf, 16);
    }
#endif

    return 0;

}/* teeComputeClientAes128Key */

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
                               uint8_t     *AesKeyPtr )
{
    AES_KEY LocalKey;
    uint8_t IV[16];

    // no mercy
    assert(CleartextPtr);
    assert(AesKeyPtr);
    assert(CleartextLength > 0);

    if ( CleartextLength % 16 != 0 )
    {
        CRYPTODBG( "%s: %d-byte buffer needs %d bytes of padding.\n",
                 __FUNCTION__, CleartextLength, CleartextLength % 16 );
        return -1;
    }

    if ( CleartextLength > sizeof(saLocalCryptoBuf) )
    {
    	CRYPTODBG("%s: not enough local mem for in-place crypto (%d/%d)",
    				__FUNCTION__, CleartextLength, sizeof(saLocalCryptoBuf));
    	return -1;
    }

    (void) teeBzero( saLocalCryptoBuf, sizeof(saLocalCryptoBuf));

    (void) teeBzero(&LocalKey, sizeof(AES_KEY) );
    if ( AES_set_encrypt_key(AesKeyPtr, 128, &LocalKey) < 0 )
    {
        CRYPTODBG("%s; AES_set_encrypt_key() failed\n", __FUNCTION__);
        return -1;
    }

    (void) teeBzero(IV, sizeof(IV) );
    AES_cbc_encrypt( CleartextPtr, saLocalCryptoBuf, CleartextLength, &LocalKey, IV, AES_ENCRYPT);

//    SECSTOREDBG("%s: dumping clear data...\n", __FUNCTION__ );
//    teeHexDump( CleartextPtr, CleartextLength );

    // now, copy encrypted data over top of cleartext
    (void) teeMemcpy( CleartextPtr, saLocalCryptoBuf, CleartextLength );

#ifdef tee_EV_CLEANUP
  //  CRYPTO_cleanup_all_ex_data();
#endif

    return 0;

}/* end teeAesCbc128EncryptZeroIV */

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
                              uint8_t     *AesKeyPtr )
{
    AES_KEY LocalKey;
    uint8_t IV[16];

    // no mercy
    assert(CiphertextPtr);
    assert(AesKeyPtr);
    assert(CiphertextLength > 0);

    if ( CiphertextLength % 16 != 0 )
    {
        CRYPTODBG( "%s: %d-byte buffer needs %d bytes of padding.\n",
                 __FUNCTION__, CiphertextLength, CiphertextLength % 16 );
        return -1;
    }

    if ( CiphertextLength > sizeof(saLocalCryptoBuf) )
    {
    	CRYPTODBG("%s: not enough local mem for in-place crypto (%d/%d)",
    				__FUNCTION__, CiphertextLength, sizeof(saLocalCryptoBuf));
    	return -1;
    }

    (void) teeBzero(&LocalKey, sizeof(AES_KEY) );
    if ( AES_set_decrypt_key(AesKeyPtr, 128, &LocalKey) < 0 )
    {
        CRYPTODBG("%s; AES_set_decrypt_key() failed\n", __FUNCTION__);
        return -1;
    }

    (void) teeBzero( saLocalCryptoBuf, sizeof(saLocalCryptoBuf));
    (void) teeBzero(IV, sizeof(IV) );

    AES_cbc_encrypt( CiphertextPtr, saLocalCryptoBuf, CiphertextLength, &LocalKey, IV, AES_DECRYPT);

    // now, copy encrypted data over top of cleartext
    (void) teeMemcpy( CiphertextPtr, saLocalCryptoBuf, CiphertextLength );

#ifdef tee_EV_CLEANUP
  //  CRYPTO_cleanup_all_ex_data();
#endif

    return 0;

}/* end teeAesCbc128DecryptZeroIV */

/*********************************************************************
 * @function teeHmacSha256
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
int teeHmacSha256( uint8_t     *DataPtr,
                   uint32_t    DataLength,
                   uint8_t     *KeyPtr,
                   uint32_t    KeyLength,
                   uint8_t     *ResultPtr )
{
    uint32_t    Length = 0;

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(KeyPtr);
    assert(KeyLength > 0);
    assert(ResultPtr);

    CRYPTODBG("%s: %d bytes of data\n", __FUNCTION__, DataLength);

    ResultPtr = HMAC ( EVP_sha256(), KeyPtr, KeyLength, DataPtr, DataLength,
              ResultPtr, &Length);

#ifdef tee_EV_CLEANUP
 //   CRYPTO_cleanup_all_ex_data();
//    EVP_cleanup();
#endif
    return Length > 0 ? (int) Length : -1;

}/* end teeHmacSha256 */

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
                  uint8_t     *ResultPtr )
{
    HMAC_CTX    Context;
    uint32_t    Length = 0;

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(KeyPtr);
    assert(KeyLength > 0);
    assert(ResultPtr);

    CRYPTODBG("%s: called with %d bytes of data, %d-byte key.\n", __FUNCTION__,
            DataLength, KeyLength );

    HMAC_Init(&Context, KeyPtr, KeyLength, EVP_sha384());
    HMAC_Update(&Context, DataPtr, DataLength);
    HMAC_Final(&Context, ResultPtr, &Length);

    CRYPTODBG("%s: HMAC length is %d\n", __FUNCTION__, Length );

    return Length > 0 ? (int) Length : -1;

}/* end teeHmacSha384 */

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
                  uint8_t     *ResultPtr )
{
    HMAC_CTX    Context;
    uint32_t    Length = 0;

    // no mercy
    assert(DataPtr);
    assert(DataLength > 0);
    assert(KeyPtr);
    assert(KeyLength > 0);
    assert(ResultPtr);

    CRYPTODBG("%s: called with %d bytes of data, %d-byte key.\n", __FUNCTION__,
            DataLength, KeyLength );

    HMAC_Init(&Context, KeyPtr, KeyLength, EVP_sha512());
    HMAC_Update(&Context, DataPtr, DataLength);
    HMAC_Final(&Context, ResultPtr, &Length);

    CRYPTODBG("%s: HMAC length is %d\n", __FUNCTION__, Length );

    return Length > 0 ? (int) Length : -1;

}/* end teeHmacSha512 */


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
						 uint32_t	CiphertextMax )
{
	EVP_CIPHER_CTX	Ctxt;
	uint32_t		CiphertextLen;
	int				GeneratedLen = 0;
	int				FinalLen = 0;

	CRYPTODBG("%s: called with %p (%d), %p, %p, %p (%d)\n",  __FUNCTION__, PlaintextPtr,
		PlaintextLen, KeyPtr, IVPtr, CiphertextPtr, CiphertextMax );

	// no nonsense
	assert(PlaintextPtr);
	assert(IVPtr);
	assert(CiphertextPtr);

	CiphertextLen = PlaintextLen + (teeTC_AES128_BLOCK_SIZE - (PlaintextLen % teeTC_AES128_BLOCK_SIZE));

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n",  __FUNCTION__, CiphertextMax, CiphertextLen );

	if ( CiphertextMax < CiphertextLen )
	{
		CRYPTODBG("%s: not enough space for ciphertext (%d/%d).\n",
				 __FUNCTION__, CiphertextMax, CiphertextLen );
		return -1;
	}

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n", __FUNCTION__, CiphertextMax, CiphertextLen );
	EVP_CIPHER_CTX_init(&Ctxt);

  	EVP_EncryptInit_ex(&Ctxt, EVP_aes_128_cbc(), NULL, KeyPtr, IVPtr);

	EVP_EncryptUpdate(&Ctxt, CiphertextPtr, &GeneratedLen, PlaintextPtr, PlaintextLen);

	/* update ciphertext with the final remaining bytes */
	EVP_EncryptFinal_ex(&Ctxt, CiphertextPtr + GeneratedLen, &FinalLen);

	if ( FinalLen + GeneratedLen != (int) CiphertextLen )
	{
		CRYPTODBG("%s: length mismatch?! (%d/%d)\n",  __FUNCTION__,
			GeneratedLen + FinalLen, CiphertextLen );
		return -1;
	}

	CRYPTODBG("%s: successfully encrypted %d bytes, (dumped below) final length is %d\n",
			__FUNCTION__, PlaintextLen, CiphertextLen );
//	teePrintHexDump( CiphertextPtr, CiphertextLen );
#ifdef tee_EV_CLEANUP
	CRYPTO_cleanup_all_ex_data();
 //   EVP_cleanup();
#endif

    return CiphertextLen;

}/* end teeAes128CbcEncrypt */


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
						 uint32_t	PlaintextMax )
{
	EVP_CIPHER_CTX	Ctxt;
	int				GeneratedLen = 0;
	int				FinalLen = 0;

	CRYPTODBG("%s: called with %p (%d), %p, %p, %p (%d)\n", __FUNCTION__, CiphertextPtr,
		CiphertextLen, KeyPtr, IVPtr, PlaintextPtr, PlaintextMax );

	// no nonsense
	assert(PlaintextPtr);
	assert(IVPtr);
	assert(CiphertextPtr);


	if ( PlaintextMax < CiphertextLen - teeTC_AES128_BLOCK_SIZE )
	{
		CRYPTODBG("%s: not enough space for plaintext (%d/%d).\n",
				 __FUNCTION__, PlaintextMax, CiphertextLen - teeTC_AES128_BLOCK_SIZE );
		return -1;
	}

	EVP_CIPHER_CTX_init(&Ctxt);

  	EVP_DecryptInit_ex(&Ctxt, EVP_aes_128_cbc(), NULL, KeyPtr, IVPtr);

	EVP_DecryptUpdate(&Ctxt, PlaintextPtr, &GeneratedLen, CiphertextPtr, CiphertextLen);

	/* update plaintext with the final remaining bytes */
	EVP_DecryptFinal_ex(&Ctxt, PlaintextPtr + GeneratedLen, &FinalLen);

	CRYPTODBG("%s: successfully decrypted %d bytes, (dumped below) final length is %d\n",
			__FUNCTION__, FinalLen + GeneratedLen, FinalLen );

	//teePrintHexDump( PlaintextPtr, FinalLen + GeneratedLen );

#ifdef tee_EV_CLEANUP
    CRYPTO_cleanup_all_ex_data();
 //   EVP_cleanup();
#endif

    return FinalLen + GeneratedLen;

}/* end teeAes128CbcDecrypt */

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
		   	   	  uint8_t		*PubKeyPtr )
{
	DH		*DHPtr = NULL;
	int		rc;

	/*
	 * NB: This is an internal TPI function, so we assume you have pre-validated
	 *     your arguments. Just sayin.
	 */

	// allocate DH struct
	if ( ( DHPtr = DH_new() ) == NULL )
	{
		CRYPTODBG("%s: DH_new() returned NULL, bailing.\n",  __FUNCTION__ );
		return -1;
	}

	// okay, get p out of buffer
	if ( ( DHPtr->p = BN_bin2bn( ModulusPtr, ModulusLength, NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to extract DH_p\n", __FUNCTION__ );
		goto ERROR;
	}

	// encode generator; remember, BN_bin2bn expects BIGENDIAN
	Generator = teeHtonl(Generator);
	if ( ( DHPtr->g = BN_bin2bn( (unsigned char *)&Generator, sizeof(uint32_t), NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to extract DH_g\n", __FUNCTION__ );
		goto ERROR;
	}


	// now, generate public/private keys
	if ( ! DH_generate_key(DHPtr) )
	{
		// XXX - error, figure out what to do
		CRYPTODBG("%s: DH_generate_key() failed\n", __FUNCTION__ );
		goto ERROR;
	}


	/*
	 * Now, we need to extract the binary priv/pub key values and return them.
	 * DH struct:
	 *
	 * struct
	 * {
	 * 	  	BIGNUM *p;
	 *		BIGNUM *g;
	 *		BIGNUM *priv_key;
 	 *		BIGNUM *pub_key;
	 * } DH;
	 *
	 */

	// need to makes sure pub key is not too big for buffer; how?
	rc = BN_num_bytes(DHPtr->pub_key);
	if ( ( rc <= 0 ) || ((uint32_t) rc > PubKeyMax ) )
	{
		CRYPTODBG("%s: public key too big (rc: %d)\n", __FUNCTION__, rc);
		goto ERROR;
	}

	// encode public key
	if ( ( rc = BN_bn2bin( DHPtr->pub_key, PubKeyPtr ) ) <= 0 )
	{
		CRYPTODBG("%s: failed to convert pubkey to binary (%d)\n",  __FUNCTION__, rc );
		goto ERROR;
	}

	*PubKeyLengthPtr = (uint32_t) rc;

	//  need to makes sure priv key is not too big for buffer
	rc = BN_num_bytes(DHPtr->priv_key);
	if ( ( rc <= 0 ) || ((uint32_t) rc > PrivKeyMax ) )
	{
		CRYPTODBG("%s: private key too big (rc: %d; PrivKeyMax: %d)\n",
					__FUNCTION__, rc, PrivKeyMax );
		goto ERROR;
	}

	// encode private key
	if ( ( rc = BN_bn2bin( DHPtr->priv_key, PrivKeyPtr ) ) <= 0 )
	{
		CRYPTODBG("%s: failed to convert privkey to binary (%d)\n",  __FUNCTION__, rc );
		goto ERROR;
	}

	*PrivKeyLengthPtr = (uint32_t) rc;

#if 0
	// XXX debug
	CRYPTODBG("%s: dumping generator:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->g);
	CRYPTODBG("\n\n%s: dumping modulus:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->p);
	CRYPTODBG("\n\n%s: dumping TEE public value:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->pub_key);
	CRYPTODBG("\n\n%s: dumping TEE private value:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->priv_key);
	printf("\n\n");
#endif

	/*
	 * Don't be forgetful...
	 */
	(void) DH_free( DHPtr );

	CRYPTODBG("%s: returning successfully...\n", __FUNCTION__);

	return 0;

// NOTREACHED
ERROR:
if ( DHPtr != NULL )
	(void) DH_free( DHPtr );
	return -1;

}/* end teeGenDHKeys */

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
							  uint8_t		*SharedSecretPtr )
{
	DH			*DHPtr = NULL;
	int			rc;
	BIGNUM		*ServerDhPubPtr = NULL;
	uint32_t	SharedSecretSize = 0;

	/*
	 * NB: This is an internal TPI function, so we assume you have pre-validated
	 *     your arguments. Just sayin.
	 */

	CRYPTODBG("%s: called with Generator %08x\n", __FUNCTION__, Generator);
	// allocate DH struct
	if ( ( DHPtr = DH_new() ) == NULL )
	{
		CRYPTODBG("%s: DH_new() returned NULL, bailing.\n",  __FUNCTION__ );
		return -1;
	}

	// okay, get p out of passed buffer
	if ( ( DHPtr->p = BN_bin2bn( ModulusPtr, ModulusLength, NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to extract DH_p\n", __FUNCTION__ );
		goto ERROR;
	}

	// encode generator; remember, BN_bin2bn expects BIGENDIAN
	Generator = teeHtonl(Generator);

	// okay, get g out of passed buffer
	if ( ( DHPtr->g = BN_bin2bn( (unsigned char *)&Generator, sizeof(uint32_t), NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to extract DH_g\n", __FUNCTION__ );
		goto ERROR;
	}

	// okay, get priv out of passed buffer
	if ( ( DHPtr->priv_key = BN_bin2bn( (unsigned char *)PrivKeyPtr, PrivKeyLength, NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to extract DH_priv\n", __FUNCTION__ );
		goto ERROR;
	}

	// make sure we have enough space for shared secret
	if ( ( SharedSecretSize = DH_size(DHPtr) ) > MaxSharedSecretLength )
	{
		CRYPTODBG("%s: shared secret buffer is too small (%d)\n", __FUNCTION__, MaxSharedSecretLength );
		goto ERROR;
	}

	// okay, get pub out of passed buffer
	if ( ( ServerDhPubPtr = BN_bin2bn( (unsigned char *)PubKeyPtr, PubKeyLength, NULL ) ) == NULL )
	{
		CRYPTODBG("%s: failed to convert dh pub to BN\n", __FUNCTION__ );
		goto ERROR;
	}

#if 0
	CRYPTODBG("\n\n%s: dumping TEE generator:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->g);
	printf("\n\n");
	CRYPTODBG("\n\n%s: dumping TEE modulus:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->p);
	printf("\n\n");
	CRYPTODBG("\n\n%s: dumping TEE private value:\n", __FUNCTION__);
	BN_print_fp(stdout, DHPtr->priv_key);
	CRYPTODBG("\n\n%s: dumping REE public value:\n", __FUNCTION__);
	BN_print_fp(stdout, ServerDhPubPtr);
	CRYPTODBG("\n%s: dumping REE PASSED public value:\n", __FUNCTION__);
	teePrintHexDump(PubKeyPtr, PubKeyLength );
	printf("\n\n");
#endif

	/* now, need to generate shared secret */
	if ( ( rc = DH_compute_key( SharedSecretPtr, ServerDhPubPtr, DHPtr ) ) <= 0 )
	{
		CRYPTODBG("%s: DH_compute_key returned %d\n",  __FUNCTION__, rc );
		goto ERROR;
	}

	CRYPTODBG("%s: computed shared secret (%d bytes), returning...\n", __FUNCTION__, SharedSecretSize );

	*SharedSecretLengthPtr = rc;

	// don't forget to free DHPtr
	(void) DH_free( DHPtr );

	return 0;

// NOTREACHED
ERROR:
if ( DHPtr != NULL )
	(void) DH_free( DHPtr );
if ( ServerDhPubPtr != NULL )
	(void) BN_free( ServerDhPubPtr );
	return -1;

}/* end teeComputeDhSharedSecret */

/*********************************************************************
 * @function teeCryptoShutdown
 *
 * @brief   TPI crypto cleanup, call upon system shutdown
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeCryptoShutdown()
{
   CRYPTO_cleanup_all_ex_data();
   EVP_cleanup();

   return 0;

}/* end teeCryptoShutdown */

// vim: expandtab tabstop=4 shiftwidth=4
