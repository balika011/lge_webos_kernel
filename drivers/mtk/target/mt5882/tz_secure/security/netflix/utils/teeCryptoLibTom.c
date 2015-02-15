/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "CommonIncludes.h"
#include "TeeDebug.h"
#include "TeeInternalDpi.h"
#include "CryptoTypes.h"
/*
 * use libtom
 */


// we are pre-allocating 16K for in-place crypto ops
#define teeMAX_CRYPTO_BUF_SIZE	(64 * 1024 + 128)
static uint8_t saLocalCryptoBuf[teeMAX_CRYPTO_BUF_SIZE];

#define teeTC_SHA1_HASH_SIZE	(20)
#define teeTC_AES128_BLOCK_SIZE	(16)

#define tee_AES128_KEY_SIZE		(16)
#define tee_AES128_IV_SIZE		(16)


#define TEE_SHA256_HMAC_SIZE	(32)
#define TEE_SHA384_HMAC_SIZE	(48)

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
extern uint32_t _teeNflxGetRandomBytes(uint8_t *pu1Buf, uint32_t u4BufLen);
	
int teeCryptoRandomBytes_libtom( uint8_t	 *BufPtr,
										uint32_t	BufLength )
{
	#if 0
	uint8_t tempBuf[512];
	teeBzero(tempBuf,512);
	mp_int a;
	mp_init(&a);
	mp_rand(&a,BufLength);
	UINT32 length = mp_unsigned_bin_size(&a);
	if(length > 512 || length < BufLength)
		return -1;
	mp_to_unsigned_bin(&a,tempBuf);

	teeMemcpy(BufPtr,tempBuf,BufLength);
#endif

	_teeNflxGetRandomBytes(BufPtr,BufLength);

return 1;

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
int teePseudoRandomBytes_libtom( uint8_t     *BufPtr,
         	  	  	  	  uint32_t    BufLength )
{
	return 1;

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
int teeSha384_libtom( uint8_t     *DataPtr,
              uint32_t    DataLength,
              uint8_t     *ResultPtr )
{
	hash_state t_hash_state;
	sha384_init(&t_hash_state);
	sha384_process(&t_hash_state,DataPtr,DataLength);
	sha384_done(&t_hash_state,ResultPtr);
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
int teeSha1_libtom( uint8_t     *DataPtr,
            				uint32_t    DataLength,
            				uint8_t     *ResultPtr )
{
	hash_state t_hash_state;
	sha1_init(&t_hash_state);
	sha1_process(&t_hash_state,DataPtr,DataLength);
	sha1_done(&t_hash_state,ResultPtr);
	return 0;


}/* end teeSha1_libtom */

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
                     				uint8_t     ExpectedResult[] )
{

	uint8_t     LocalResult[teeTC_SHA1_HASH_SIZE];
	int         rc;
	if((rc = teeSha1_libtom(DataPtr, DataLength, LocalResult)) != 0 )
	{
		CRYPTODBG("%s: teeSha1_libtom() failed.\n", __FUNCTION__ );
		return -1;
	}
	if(teeMemcmp( ExpectedResult, LocalResult, teeTC_SHA1_HASH_SIZE ) != 0 )
	{
		CRYPTODBG("%s: Hash does not match\n", __FUNCTION__ );
		return -1;
	}

	CRYPTODBG("%s: hashes match.\n", __FUNCTION__ );

	return 0;

}/* end teeValidateSha1_libtom */

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
int teeComputeClientAes128Key_libtom( uint8_t ClientID[], uint8_t Aes128Key[], uint8_t KeyLadderKey[] )
{

    return 0;

}/* teeComputeClientAes128Key_libtom */

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
                               uint8_t     *AesKeyPtr )
{
    // no mercy
    //assert(CleartextPtr);
    //assert(AesKeyPtr);
    //assert(CleartextLength > 0);

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

	uint8_t IV[16];
	uint32_t Ret = 0;

	
		//INT32 i4_tee_nfx_encrypt(const UINT8* pulKeyBuf, INT32 u4KeyLen ,const UINT8* pulIV ,INT32 u4IvLen ,
		//							const UINT8 *pu1SrcBuf, INT32 u4SrcLen, /*out*/UINT8 *pu1DstBuf, INT32 i4Dst_max_len ,/*out*/INT32 *u4Dst_real_len)
	uint32_t encrypt_rel_len = 16;
	
	(void) teeBzero( saLocalCryptoBuf, sizeof(saLocalCryptoBuf));
	(void) teeBzero(IV, 16);
	Ret = i4_tee_nfx_encrypt(AesKeyPtr,16,IV,16,CleartextPtr,CleartextLength,saLocalCryptoBuf,teeMAX_CRYPTO_BUF_SIZE,&encrypt_rel_len);
	(void) teeMemcpy( CleartextPtr, saLocalCryptoBuf, CleartextLength );
	
    return Ret;

}/* end teeAesCbc128EncryptZeroIV_libtom */

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
                              uint8_t     *AesKeyPtr )
{


    // no mercy
    //assert(CiphertextPtr);
    //assert(AesKeyPtr);
    //assert(CiphertextLength > 0);

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
	
   
	uint32_t Ret = 0;

	//UINT32 i4_tee_nfx_decrypt(const UINT8* pulKeyBuf, UINT32 u4KeyLen ,const UINT8* pulIV ,UINT32 u4IvLen ,
	//								const UINT8 *pu1SrcBuf, UINT32 u4SrcLen, /*out*/UINT8 *pu1DstBuf, UINT32 u4DstLen,/*out*/UINT32 *u4Des_real_len)
	
	uint32_t plaintext_rel_len = 16;
	uint8_t IV[16];

	(void) teeBzero( saLocalCryptoBuf, sizeof(saLocalCryptoBuf));
	(void) teeBzero(IV, sizeof(IV) );	
	Ret = i4_tee_nfx_decrypt_no_del_rear_padding(AesKeyPtr,16,IV,16,CiphertextPtr,CiphertextLength,saLocalCryptoBuf,teeMAX_CRYPTO_BUF_SIZE,&plaintext_rel_len);
	(void) teeMemcpy( CiphertextPtr, saLocalCryptoBuf, plaintext_rel_len );

    return 0;
}/* end teeAesCbc128DecryptZeroIV_libtom */

/*********************************************************************
 * @function teeHmacSha256_libtom
 *
 * @brief  Do HmacSha256 on passed buffer
 *
 * @param[in]  DataPtr - pointer to cleartext
 *          	  [in]  DataLength - length of cleartext
 *		  [in]  KeyPtr - pointer to key
 *		  [in]  KeyLength - length of key
 *             [out] ResultPtr - pointer to buffer for result
 *
 * @returns Length of HMAC (256/8) if everything okay
 *          -1 otherwise
 *
 *********************************************************************/

int teeHmacSha256_libtom( uint8_t     *DataPtr,
                   				uint32_t    DataLength,
                   				uint8_t     *KeyPtr,
                   				uint32_t    KeyLength,
                   				uint8_t     *ResultPtr )
{
	int idx, err;	
	hmac_state hmac;  
	uint32_t ul4_ret = 0;

	if(register_hash(&sha256_desc) == -1) 
	{	  
		//printf("[teeHmacSha256_libtom] Error registering SHA1\n");	  
		return -1;	
	}	
	idx = find_hash("sha256");  
	if(-1 == idx)
	{
		//printf("[teeHmacSha256_libtom] find cipher sha256 fail. \n");
		return -1;
	}
	/* start the HMAC */
	if((err = hmac_init(&hmac, idx, KeyPtr, KeyLength)) != CRYPT_OK) 
	{	  
		//printf("[teeHmacSha256_libtom] Error setting up hmac: %s\n", error_to_string(err)); 
		return -1;  
	}  
	/* process  */ 
	if((err = hmac_process(&hmac, DataPtr, DataLength) != CRYPT_OK))
	{ 
		//printf("[teeHmacSha256_libtom] Error processing hmac: %s\n", error_to_string(err));	
		return -1;	
	}	
	/* get result  */	
	ul4_ret = TEE_SHA256_HMAC_SIZE;
	if ((err = hmac_done(&hmac, ResultPtr, (long unsigned int *)(&ul4_ret))) != CRYPT_OK) 
	{	
		//printf("[teeHmacSha256_libtom] Error finishing hmac: %s\n", error_to_string(err)); 
		return -1; 
	}  
	//unregister_hash(&sha256_desc); 
    return ul4_ret;


}/* end teeHmacSha256_libtom */

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
                  uint8_t     *ResultPtr )
{
	int idx, err;	
	hmac_state hmac;  
	unsigned long ul4_ret = 0;

	if(register_hash(&sha384_desc) == -1) 
	{	  
		//printf("Error registering SHA1\n");	  
		return -1;	
	}	
	idx = find_hash("sha384");  
	if(-1 == idx)
	{
		//printf("find cipher sha384 fail. \n");
		return -1;
	}
	/* start the HMAC */
	if((err = hmac_init(&hmac, idx, KeyPtr, KeyLength)) != CRYPT_OK) 
	{	  
		//printf("Error setting up hmac: %s\n", error_to_string(err)); 
		return -1;  
	}  
	/* process  */ 
	if((err = hmac_process(&hmac, DataPtr, DataLength) != CRYPT_OK))
	{ 
		//printf("Error processing hmac: %s\n", error_to_string(err));	
		return -1;	
	}	
	/* get result  */	
	ul4_ret = TEE_SHA384_HMAC_SIZE;
	if ((err = hmac_done(&hmac, ResultPtr, &ul4_ret)) != CRYPT_OK) 
	{	
		//printf("Error finishing hmac: %s\n", error_to_string(err)); 
		return -1; 
	}  
	//unregister_hash(&sha256_desc); 
    return ul4_ret;
   return -1;
}/* end teeHmacSha384_libtom */

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
                  				uint8_t     *ResultPtr )
{
    
    uint32_t    Length = 0;

    

    return Length > 0 ? (int) Length : -1;

}/* end teeHmacSha512_libtom */


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
						 uint32_t	CiphertextMax )
{

	uint32_t		CiphertextLen;
	//int				GeneratedLen = 0;
	//int				FinalLen = 0;
	CRYPTODBG("%s: called with %p (%d), %p, %p, %p (%d)\n",  __FUNCTION__, PlaintextPtr,
		PlaintextLen, KeyPtr, IVPtr, CiphertextPtr, CiphertextMax );

	// no nonsense
	//assert(PlaintextPtr);
	//assert(IVPtr);
	//assert(CiphertextPtr);

	CiphertextLen = PlaintextLen + (teeTC_AES128_BLOCK_SIZE - (PlaintextLen % teeTC_AES128_BLOCK_SIZE));

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n",  __FUNCTION__, CiphertextMax, CiphertextLen );

	if ( CiphertextMax < CiphertextLen )
	{
		CRYPTODBG("%s: not enough space for ciphertext (%d/%d).\n",
				 __FUNCTION__, CiphertextMax, CiphertextLen );
		return -1;
	}

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n", __FUNCTION__, CiphertextMax, CiphertextLen );


	//UINT32 i4_tee_nfx_encrypt(const UINT8* pulKeyBuf, UINT32 u4KeyLen ,const UINT8* pulIV ,UINT32 u4IvLen ,
	//									const UINT8 *pu1SrcBuf, UINT32 u4SrcLen, /*out*/UINT8 *pu1DstBuf, UINT32 i4Dst_max_len ,/*out*/UINT32 *u4Dst_real_len)

	uint32_t Real_cipher_len = 0;
	int Ret = -1;
	Ret = i4_tee_nfx_encrypt(KeyPtr,tee_AES128_KEY_SIZE,IVPtr,tee_AES128_IV_SIZE,PlaintextPtr,PlaintextLen,CiphertextPtr,CiphertextMax,&Real_cipher_len);
	if(Ret != 0)
		return -1;
	
	if(Real_cipher_len != CiphertextLen)
		return -1;
	
    return CiphertextLen;

}/* end teeAes128CbcEncrypt_libtom */

/*********************************************************************
 * @function teeAes128CbcDecrypt_no_remove_padding_libtom
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

int teeAes128CbcDecrypt_no_remove_padding_libtom( uint8_t 	*CiphertextPtr,
					     uint32_t 	CiphertextLen,
						 uint8_t  	*KeyPtr,
						 uint8_t	*IVPtr,
						 uint8_t 	*PlaintextPtr,
						 uint32_t	PlaintextMax )
{

	int i = 0;	
	symmetric_CBC t_sym_cbc;
	uint8_t ul_padding = 0;
	uint32_t l_plainSize = 0;
	
	l_plainSize = CiphertextLen;
	
	/* decrypt */
	teeMemset(PlaintextPtr, 0, CiphertextLen);
	register_cipher(&aes_desc);
	cbc_start(find_cipher("aes"), IVPtr, KeyPtr, 16, 0, &t_sym_cbc);
	cbc_decrypt(CiphertextPtr, PlaintextPtr, l_plainSize, &t_sym_cbc);
	cbc_done(&t_sym_cbc);

	#if 0
	/*delete padding in the rear*/
	ul_padding = PlaintextPtr[l_plainSize - 1] ;
	
	for(i = 0 ; i < l_plainSize ; i ++)
	{
		if(ul_padding != PlaintextPtr[l_plainSize - i - 1])
			break;
	}
	if(ul_padding != i)
	{
		printf("[nfx_tee_test] padding error . \n");
		return -1;
	}
	PlaintextPtr[l_plainSize - ul_padding] = '\0' ;

	return l_plainSize - ul_padding;
#endif

	return l_plainSize;


}/* end teeAes128CbcDecrypt_no_remove_padding_libtom */



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
						 uint32_t	PlaintextMax )
{

	int i = 0;	
	symmetric_CBC t_sym_cbc;
	uint8_t ul_padding = 0;
	uint32_t l_plainSize = 0;
	
	l_plainSize = CiphertextLen;
	
	/* decrypt */
	teeMemset(PlaintextPtr, 0, CiphertextLen);
	register_cipher(&aes_desc);
	cbc_start(find_cipher("aes"), IVPtr, KeyPtr, tee_AES128_KEY_SIZE, 0, &t_sym_cbc);
	cbc_decrypt(CiphertextPtr, PlaintextPtr, l_plainSize, &t_sym_cbc);
	cbc_done(&t_sym_cbc);
	
	/*delete padding in the rear*/
	ul_padding = PlaintextPtr[l_plainSize - 1] ;
	
	for(i = 0 ; i < l_plainSize ; i ++)
	{
		if(ul_padding != PlaintextPtr[l_plainSize - i - 1])
			break;
	}
	if(ul_padding != i)
	{
		//printf("[nfx_tee_test] padding error . \n");
		return -1;
	}
	PlaintextPtr[l_plainSize - ul_padding] = '\0' ;

	return l_plainSize - ul_padding;


}/* end teeAes128CbcDecrypt */

/*********************************************************************
 * @function teeGenDhKeys_libtom
 *
 * @brief   handle a DH genkeys request from REE , generate private key randomly
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
int teeGenDhKeys_libtom( uint32_t		Generator,
		   	   	  uint32_t		ModulusLength,
		   	   	  uint8_t		*ModulusPtr,
		   	   	  uint32_t		*PrivKeyLengthPtr,
		   	   	  uint32_t		PrivKeyMax,
		   	   	  uint8_t		*PrivKeyPtr,
		   	   	  uint32_t		*PubKeyLengthPtr,
		   	   	  uint32_t		PubKeyMax,
		   	   	  uint8_t		*PubKeyPtr )
{

	int  err;
	void *p,*g,*a,*A;
	Generator = teeHtonl(Generator);
	teePrintHexDump_tag("[teeGenDhKeys_libtom] PrivKeyPtr :",PrivKeyPtr,*PrivKeyLengthPtr);

	/* Init p, g, a */
	if ((err = mp_init_multi(&p,&g,&a,&A, NULL)) != CRYPT_OK)
	{ 
		return err; 
	}
	if((err = mp_read_unsigned_bin(p,ModulusPtr,ModulusLength)) != CRYPT_OK)
		goto error;
	if((err = mp_read_unsigned_bin(g,(unsigned char *)&Generator, sizeof(uint32_t))) != CRYPT_OK)
		goto error;
	if((err = mp_read_unsigned_bin(a,PrivKeyPtr,*PrivKeyLengthPtr)) != CRYPT_OK)
		goto error;
		
	/*A = g ^a mode p*/
	if((err = mp_exptmod(g,a,p,A)) != CRYPT_OK)
		goto error;

	*PubKeyLengthPtr = mp_unsigned_bin_size(A);

	if(PubKeyMax < *PubKeyLengthPtr)
	{
		//printf("[teeGenDhKeys_libtom_test] public key size is not enough .\n");
		err= -1;
		goto error;
	}
	if((err = mp_to_unsigned_bin(A,PubKeyPtr)) != CRYPT_OK)
		goto error;
	
	teePrintHexDump_tag("[teeGenDhKeys_libtom] PubKeyPtr :",PubKeyPtr,*PubKeyLengthPtr);
	err = 0;
	error:
	   mp_clear_multi(p, g, a, A, NULL);
	   return err;

	
}/* end teeGenDhKeys_libtom */


/*********************************************************************
 * @function teeGenDhKeys_libtom_test
 *
 * @brief   handle a DH genkeys request from REE, 
 *
 * @param[in] Generator - the DH generator value
 * @param[in] ModulusLength - length of DH modulus
 * @param[in] ModulusPtr - pointer to DH modulus
 * @param[in] PrivKeyLengthPtr - pointer to where you want length of DH priv value
 * @param[in] PrivKeymax - maximum length of DH private value
 * @param[in] PrivKeyPtr - pointer to where you want DH priv key
 * @param[out] PubKeyLengthPtr - pointer to where you want length of DH pub value
 * @param[out] PubKeyPtr - pointer to where you want DH pub key
 *
 * @return 0 if everything okay, -1 otherwise
 *
  *********************************************************************/

int teeGenDhKeys_libtom_test( uint32_t		Generator,
		   	   	  uint32_t		ModulusLength,
		   	   	  uint8_t		*ModulusPtr,
		   	   	  uint32_t		*PrivKeyLengthPtr,
		   	   	  uint32_t		PrivKeyMax,
		   	   	  uint8_t		*PrivKeyPtr,
		   	   	  uint32_t		*PubKeyLengthPtr,
		   	   	  uint32_t		PubKeyMax,
		   	   	  uint8_t		*PubKeyPtr )
{

	
	return 1;

}/* end teeGenDhKeys_libtom_test */


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
							  uint8_t		*SharedSecretPtr )
{	

	/* Init p, g, a */
	int err;
	void *p,*a,*B,*K;
	if((err = mp_init_multi(&p, &a, &B, &K, NULL))!= CRYPT_OK)
		return err;

	if((err = mp_read_unsigned_bin(p,ModulusPtr,ModulusLength))!= CRYPT_OK)
		goto error;
	if((err = mp_read_unsigned_bin(a,PrivKeyPtr,PrivKeyLength))!= CRYPT_OK)
		goto error;
	if((err = mp_read_unsigned_bin(B,PubKeyPtr,PubKeyLength))!= CRYPT_OK)
		goto error;	
	/* K = B ^a mod p*/
	if((err = mp_exptmod(B,a,p,K))!= CRYPT_OK)
		goto error;	

	*SharedSecretLengthPtr = mp_unsigned_bin_size(K);

	if(MaxSharedSecretLength < *SharedSecretLengthPtr)
	{
		//printf("[teeComputeDhSharedSecret_libtom] shared secret key size is not enough .\n");
		err = -1;
		goto error;
	}
	if((err = mp_to_unsigned_bin(K,SharedSecretPtr))!= CRYPT_OK)
		goto error;

	error:
	   mp_clear_multi(p, a, B, K, NULL);
	   return err;



}/* end teeComputeDhSharedSecret_libtom */

/*********************************************************************
 * @function teeCryptoShutdown_libtom
 *
 * @brief   TPI crypto cleanup, call upon system shutdown
 *
 * @return 0 if everything okay, -1 otherwise
 *
 *********************************************************************/
int teeCryptoShutdown_libtom()
{
   return 0;

}/* end teeCryptoShutdown_libtom */

uint32_t teeAes128CbcEncryptCleanup_libtom(void* pIVKEY)
{
	if(pIVKEY != NULL)
	{
		teeMemFree(&pIVKEY);
		pIVKEY = NULL;
	}
	return 0;
}


void *teeAes128CbcEncryptInit_libtom( uint8_t  *IVPtr,
						 	   uint8_t	*KeyPtr )
{
	caAes128CryptoContextItem_t* pIVKey = (caAes128CryptoContextItem_t*)teeMemAlloc(sizeof(caAes128CryptoContextItem_t));
	if(NULL == pIVKey)
		return NULL;
	teeMemcpy(pIVKey->IV,IVPtr,16);
	teeMemcpy(pIVKey->Key,KeyPtr,16);
	teeBzero(pIVKey->cipher,16);
	pIVKey->hmac = NULL;
	return pIVKey;
}

uint32_t teeAes128CbcEncryptUpdate_libtom(void		*IVKEYPtr,
							   uint8_t 	*PlaintextPtr,
							   uint32_t	PlaintextLen,
							   uint8_t	*CiphertextPtr,
							   uint32_t	CiphertextMax)
{
	
	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)IVKEYPtr;
	uint32_t		CiphertextLen;
	CRYPTODBG("%s: called with %p (%d), %p, %p, %p (%d)\n",  __FUNCTION__, PlaintextPtr,
		PlaintextLen, pIVKEY->Key, pIVKEY->IV, CiphertextPtr, CiphertextMax );


	CiphertextLen = PlaintextLen + (teeTC_AES128_BLOCK_SIZE - (PlaintextLen % teeTC_AES128_BLOCK_SIZE));

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n",  __FUNCTION__, CiphertextMax, CiphertextLen );

	if ( CiphertextMax < CiphertextLen )
	{
		CRYPTODBG("%s: not enough space for ciphertext (%d/%d).\n",
				 __FUNCTION__, CiphertextMax, CiphertextLen );
		return -1;
	}

	CRYPTODBG("%s: CipherTextMax: %d; CiphertextLen: %d\n", __FUNCTION__, CiphertextMax, CiphertextLen );

	uint32_t Real_cipher_len = 0;
	int Ret = -1;
	
	Ret = i4_tee_nfx_encrypt(pIVKEY->Key,tee_AES128_KEY_SIZE,pIVKEY->IV,tee_AES128_IV_SIZE,PlaintextPtr,PlaintextLen,CiphertextPtr,CiphertextMax,&Real_cipher_len);
	if(Ret != 0)
		return -1;
	
	if(Real_cipher_len != CiphertextLen)
		return -1;

	//update the IVKEYPtr.
	teeMemcpy(pIVKEY->IV,CiphertextPtr+PlaintextLen-16,16);

	//store the last 16 bytes cipher
	teeMemcpy(pIVKEY->cipher,CiphertextPtr+Real_cipher_len-16,16);
	
    return CiphertextLen;

	
}




uint32_t teeAes128CbcEncryptFinal_libtom( void		*CtxtPtr,
							  uint8_t	*CiphertextPtr,
							  uint32_t	CiphertextMax )
{

	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)CtxtPtr;

	if(pIVKEY == NULL)
		return -1;
	if(CiphertextMax < 16)
		return -1 ;
	
	teeMemcpy(CiphertextPtr,pIVKEY->cipher,16);
	
	teeAes128CbcEncryptCleanup_libtom(CtxtPtr);
	return 16;
}


void *teeAes128CbcDecryptInit_libtom( uint8_t  *IVPtr,
						 	   uint8_t	*KeyPtr )
{
	caAes128CryptoContextItem_t* pIVKey = (caAes128CryptoContextItem_t*)teeMemAlloc(sizeof(caAes128CryptoContextItem_t));
	if(NULL == pIVKey)
		return NULL;

	teeMemcpy(pIVKey->IV,IVPtr,16);
	teeMemcpy(pIVKey->Key,KeyPtr,16);
	teeBzero(pIVKey->cipher,16);
	pIVKey->hmac = NULL;
	return pIVKey;

}/* end teeAes128CbcDecryptInit */


int teeAes128CbcDecryptUpdate_libtom( void		*IVKEYPtr,
							   uint8_t	*CiphertextPtr,
							   uint32_t	CiphertextLen,
							   uint8_t 	*PlaintextPtr,
							   uint32_t	PlaintextMax )

{
	
	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)IVKEYPtr;
	
	int plainSize = teeAes128CbcDecrypt_no_remove_padding_libtom(CiphertextPtr,CiphertextLen,pIVKEY->Key,pIVKEY->IV,PlaintextPtr,PlaintextMax);
	
	//copy IV
	teeMemcpy(pIVKEY->IV,CiphertextPtr+CiphertextLen-16,16);
	teeMemcpy(pIVKEY->cipher,PlaintextPtr+plainSize-16,16);
	
	return plainSize;	


}

int teeAes128CbcDecryptFinal_libtom( void		*IVKEYPtr,
							  uint8_t	*PlaintextPtr,
							  uint32_t	PlaintextMax )
{
	(void)PlaintextPtr;
	(void)PlaintextMax;
	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)IVKEYPtr;
	uint8_t ul_padding = 0;
	uint8_t * ChipertextPtr = pIVKEY->cipher;

	/*compute padding in the rear*/
	ul_padding = ChipertextPtr[15] ;
	int i = 0;
	for(i = 0 ; i < 16 ; i ++)
	{
		if(ul_padding != ChipertextPtr[16 - i - 1])
			break;
	}
	if(ul_padding != i)
	{
		//printf("[nfx_tee_test] padding error . \n");
		return -1;
	}
	
	if(pIVKEY != NULL)
	{
		teeMemFree(&pIVKEY);
		pIVKEY = NULL;
	}
	return 16-i;

}


void *teeHmacSha256Init_libtom( uint8_t *KeyPtr )
{
	caAes128CryptoContextItem_t* pIVKey = (caAes128CryptoContextItem_t*)teeMemAlloc(sizeof(caAes128CryptoContextItem_t));
	if(NULL == pIVKey)
		return NULL;
	teeMemcpy(pIVKey->Key,KeyPtr,32);
	teeBzero(pIVKey->cipher,16);
	teeBzero(pIVKey->IV,16);

	hmac_state* phmac_state = (hmac_state*)teeMemAlloc(sizeof(hmac_state));
	CRYPTODBG(" *****************teeHmacSha256Init_libtom success .hmac_state %p. \n",phmac_state);

	pIVKey->hmac = (void*)phmac_state;

	int idx, err;	

	if(register_hash(&sha256_desc) == -1) 
	{	  
		return NULL;	
	}	
	idx = find_hash("sha256");  
	if(-1 == idx)
	{
		return NULL;
	}
	/* start the HMAC */
	if((err = hmac_init(phmac_state, idx, KeyPtr, 32)) != CRYPT_OK) 
	{	  
		return NULL;  
	}  
	return pIVKey;
}

int teeHmacSha256Update_libtom( void		*IVKEYPtr,
					   	 uint8_t 	*DataPtr,
					   	 uint32_t	DataLen )
{
	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)IVKEYPtr;

	hmac_state* pHmac_state = (hmac_state*)(pIVKEY->hmac);
	CRYPTODBG("teeHmacSha256Update_libtom .hmac_state %p. \n",pHmac_state);
	int err;			
	/* process	*/ 
	if((err = hmac_process(pHmac_state, DataPtr, DataLen) != CRYPT_OK))
	{
		CRYPTODBG("%s: teeHmacSha256Update_libtom fail.\n");
		return -1;	
	}	
	return 0;
}

int teeHmacSha256Final_libtom( void		*IVKEYPtr,
					   	 uint8_t 	*HmacPtr,
					   	 uint32_t	HmacLen )
{
	caAes128CryptoContextItem_t* pIVKEY = (caAes128CryptoContextItem_t*)IVKEYPtr;
	if(pIVKEY == NULL)
		return -1;
	hmac_state* pHmac_state = (hmac_state*)(pIVKEY->hmac);
	CRYPTODBG(" teeHmacSha256Final_libtom .hmac_state %p. \n",pHmac_state);
	int err;			

	/* get result  */	
	uint32_t ul4_ret = HmacLen;
	if ((err = hmac_done(pHmac_state, HmacPtr, (long unsigned int *)(&ul4_ret))) != CRYPT_OK) 
	{	
		CRYPTODBG("%s: teeHmacSha256Final_libtom fail.\n");
		return -1; 
	}  

	if(pIVKEY != NULL)
	{	
		if(pIVKEY->hmac != NULL)
			teeMemFree(&(pIVKEY->hmac));
		teeMemFree(&pIVKEY);
		pIVKEY = NULL;
	}
	return HmacLen;

}


// vim: expandtab tabstop=4 shiftwidth=4
