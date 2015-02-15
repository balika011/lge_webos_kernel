/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */


/*==========================================================================*/
/**
 * @file
 * Crypto Library
 *
 * require:
 */
/*==========================================================================*/
#include "basic_types_iptves.h"

#include "crypto_api_iptves.h"
#include "crypto_ecc_iptves.h"


void dump_hex_name_i(unsigned char *name, unsigned char *buf, int len, int ii)
{
    int idx = 0;


	
    dprintf("char %s%d[] = {\n", name, ii);
    while (idx < len)
    {
        if (idx%28 == 0)
        {
            if (idx != 0)
                dprintf("\n");
            //dprintf("0x%04x| ", idx);
        }

		if(idx==len-1)
	        dprintf("0x%02x", buf[idx]);
		else
	        dprintf("0x%02x, ", buf[idx]);			
        idx++;
    }
    dprintf("};\n\n");
}


#undef IPTVES_DUMP


static int g_aes_index = -1 ; 
static symmetric_CBC g_cbcAES = {0};



/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/

/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 */
u_int32_t 
crypto_iptves_Init(void)
{
	int aes_index = -1;

	dprintf(">>%s\n",  __FUNCTION__);


	aes_index = register_cipher(&aes_desc);
	if(-1 == aes_index)
	{
		dprintf("register_chiper(aes_desc) failed\n");
		return RET_CRYPTO_NG;
	}
	else
	{
		dprintf("%s:register_chiper(aes_desc) ok[%d]\n", __FUNCTION__, aes_index);

	}
	
	g_aes_index = find_cipher("aes");
	
	if(g_aes_index == -1)
	{
		dprintf("find_cipher(aes) failed\n");
		return RET_CRYPTO_NG;
	}


#if 0
		ltc_mp = tfm_desc;
	
		ltc_mp = ltm_desc;
#endif

	crypto_GlobalInit();

	
	return RET_CRYPTO_OK ;
}

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 */
u_int32_t 
crypto_iptves_Fin(void)
{
	int error =  0;  //return value

	//dprintf("\n%s\n", __FUNCTION__);
	
	error = unregister_cipher(&aes_desc);
	
	if(error != CRYPT_OK)
	{
		dprintf("\n%s:unregister_cipher error : %s\n" , __FUNCTION__, error_to_string(error));
		return RET_CRYPTO_NG;
	}	


	error = crypto_GlobalFin();
	
	return RET_CRYPTO_OK ;
}

/**
 * AES Encryption (CBC mode)
 *
 * @param [in]     in_plain             Plain text
 * @param [in]     in_plain_len         Plain text length
 * @param [in]     in_key               Key
 * @param [in]     in_iv                IV
 * @param [out]    out_cipher           Cipher text
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t 
crypto_AesCbcofbEncrypt(const unsigned char *in_plain,
                        int32_t         in_plain_len,
                        const unsigned char  in_key[CRYPTO_AES_KEY_LEN],
                        const unsigned char  in_iv[CRYPTO_AES_BLOCK_LEN],
                        unsigned char       *out_cipher)
{
	int32_t block_count = in_plain_len/16 ;
	int32_t block_last_len = in_plain_len%16 ;
	const unsigned char * in_plain_tmp  = in_plain;
	unsigned char * out_cipher_tmp = out_cipher;

	//for non -16 block data ^
	unsigned char   chiper_block_data[CRYPTO_AES_BLOCK_LEN] = {0};
	int i = 0;
	int error =  0;  //return value


	//DumpHex("in_plain", in_plain, in_plain_len);
	//DumpHex("in_key", in_key, CRYPTO_AES_KEY_LEN);	
	//
	//DumpHex("in_iv", in_iv, CRYPTO_AES_BLOCK_LEN);



	if( (in_plain == NULL) ||  ( out_cipher ==NULL ) ||  (in_plain_len <1 ) )
	{
		return RET_CRYPTO_ILLEGAL_ARGS;
	}
	

	error = cbc_start(g_aes_index,in_iv,in_key,CRYPTO_AES_KEY_LEN,0,&g_cbcAES);
	if(error != CRYPT_OK)
	{
		dprintf("cbc_start error : %s" , error_to_string(error));
		return RET_CRYPTO_NG;
	}

	//printf("block_count:%d   rtbp:%d\n",  block_count, block_last_len);

	//DumpHex("pt", in_plain_tmp,   in_plain_len);
	//DumpHex("ct",  out_cipher_tmp, in_plain_len);


	
	if(block_count == 0  )
	{
		//printf("cbc_encrypt non-16-block data & block_count == 0 :" );
		
		//error = cbc_encrypt(in_iv,chiper_block_data,16,&g_cbcAES);
		error = cbc_encrypt(in_plain, chiper_block_data, CRYPTO_AES_KEY_LEN, &g_cbcAES);
		if(error != CRYPT_OK)
		{
			dprintf("cbc_encrypt error : %s" , error_to_string(error));
			return RET_CRYPTO_NG;
		}	

		
		while( i < block_last_len )
		{
			out_cipher_tmp[i] = chiper_block_data[i+1]^in_plain[i];
			++i;
		};
		
	}
	else
	{

		for(i=0; i<block_count; i++)
		{
			//printf("encrypt block[%d]\n", i);
			error = cbc_encrypt(in_plain_tmp, out_cipher_tmp + (CRYPTO_AES_KEY_LEN*i), CRYPTO_AES_KEY_LEN, &g_cbcAES);	
			if(error != CRYPT_OK)
				{
				dprintf("cbc_encrypt error : %s" , error_to_string(error));
				return RET_CRYPTO_NG;
			}	
			
			in_plain_tmp   += CRYPTO_AES_KEY_LEN;
		}

		//DumpHex("cihper block", out_cipher_tmp, in_plain_len);
		
		if(block_last_len != 0)//last non block data decrypt by ^
		{
			unsigned char tmp[CRYPTO_AES_KEY_LEN] = {0};

			error = cbc_start(g_aes_index,in_iv,in_key,CRYPTO_AES_KEY_LEN,0,&g_cbcAES);
			if(error != CRYPT_OK)
			{
				dprintf("cbc_start error : %s" , error_to_string(error));
				return RET_CRYPTO_NG;
			}


			
			// re-encrypte previous cipher block
			//printf("\nre-encrypte previous cipher block:\n" );
			//DumpHex("last cihper block", out_cipher_tmp+(CRYPTO_AES_KEY_LEN*(block_count-1)), CRYPTO_AES_KEY_LEN);
			error = cbc_encrypt(out_cipher_tmp+(CRYPTO_AES_KEY_LEN*(block_count-1)), tmp, CRYPTO_AES_KEY_LEN, &g_cbcAES);	
			//DumpHex("cihper block", tmp, CRYPTO_AES_KEY_LEN);

			//printf("\nXOR: block_last_len[%d] [%x] [%x]\n", block_last_len, in_plain_tmp[0], in_plain_tmp[1]);
			
			i = 0;
			while( i < block_last_len )
			{
				out_cipher_tmp[(16*block_count)+i] = tmp[i]^in_plain_tmp[i];
				///printf("%x XOR %x -->  %x \n", tmp[i], in_plain_tmp[i], out_cipher_tmp[(16*block_count)+i]);
				++i;
			};

		}
	}

	cbc_done(&g_cbcAES);
	
	return RET_CRYPTO_OK ;
}

/**
 * AES Decryption (CBC mode)
 *
 * @param [in]     in_cipher            Cipher text
 * @param [in]     in_cipher_len        Cipher text length
 * @param [in]     in_key               Key
 * @param [in]     in_iv                IV
 * @param [out]    out_plain            Plain text
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_AesCbcofbDecrypt(const unsigned char *in_cipher,
                        int32_t         in_cipher_len,
                        const unsigned char  in_key[CRYPTO_AES_KEY_LEN],
                        const unsigned char  in_iv[CRYPTO_AES_BLOCK_LEN],
                        unsigned char       *out_plain)
{
		int32_t block_count = in_cipher_len/16 ;
		int32_t block_last_len = in_cipher_len%16 ;
		const unsigned char * in_cipher_tmp  = in_cipher;
		unsigned char * out_plain_tmp = NULL;
	
		//for non -16 block data ^
		unsigned char	chiper_block_data[CRYPTO_AES_BLOCK_LEN] = {0};
		int i = 0;
		int error =  0;  //return value
	
	
		out_plain_tmp = malloc(in_cipher_len);
		if(NULL == out_plain_tmp)
		{
			dprintf("out plain malloc error");
			return -1;
		}

		//in_cipher_len = sizeof(in_cipher);
		block_last_len= in_cipher_len%16 ;
		
		//DumpHex("in_cipher", in_cipher, in_cipher_len);
		DumpHex("in_key", in_key, CRYPTO_AES_KEY_LEN);	
		DumpHex("in_iv", in_iv, CRYPTO_AES_BLOCK_LEN);
	
	
	
		if( (in_cipher == NULL) ||  ( out_plain ==NULL ) ||  (in_cipher_len <1 ) )
		{
			return RET_CRYPTO_ILLEGAL_ARGS;
		}
		
	
		error = cbc_start(g_aes_index,in_iv,in_key,CRYPTO_AES_KEY_LEN,0,&g_cbcAES);
		if(error != CRYPT_OK)
		{
			dprintf("cbc_start error : %s" , error_to_string(error));
			return RET_CRYPTO_NG;
		}

		
		dprintf("\n\n%s:\n", __FUNCTION__);
		dprintf("block_count:%d	 rtbp:%d\n",  block_count, block_last_len);
	
		DumpHex("in data", in_cipher_tmp,   in_cipher_len);
		DumpHex("ct",  out_plain_tmp, in_cipher_len);
	
	
		
		if(block_count == 0  )
		{
			// fix
			//printf("cbc_decrypt non-16-block data & block_count == 0 :" );
			
			//error = cbc_encrypt(in_iv,chiper_block_data,16,&g_cbcAES);
			error = cbc_decrypt(in_cipher, out_plain_tmp, CRYPTO_AES_KEY_LEN, &g_cbcAES);
			if(error != CRYPT_OK)
			{
				dprintf("cbc_encrypt error : %s" , error_to_string(error));
				return RET_CRYPTO_NG;
			}	
	
			
			while( i < block_last_len )
			{
				out_plain_tmp[i] = chiper_block_data[i+1]^in_cipher[i];
				++i;
			};
			
		}
		else
		{
	
			for(i=0; i<block_count; i++)
			{
				//printf("decrypt block[%d]\n", i);
				error = cbc_decrypt(in_cipher_tmp, out_plain_tmp + (CRYPTO_AES_KEY_LEN*i), CRYPTO_AES_KEY_LEN, &g_cbcAES);	
				//DumpHex(" ", out_plain_tmp + (CRYPTO_AES_KEY_LEN*i), CRYPTO_AES_KEY_LEN);
				if(error != CRYPT_OK)
					{
					dprintf("cbc_encrypt error : %s" , error_to_string(error));
					return RET_CRYPTO_NG;
				}	
				
				in_cipher_tmp	+= CRYPTO_AES_KEY_LEN;
			}
	
			//DumpHex("cihper block", out_plain_tmp, in_cipher_len);
			
			if(block_last_len != 0)//last non block data decrypt by ^
			{
				unsigned char tmp[CRYPTO_AES_KEY_LEN] = {0};
	#if 1
				error = cbc_start(g_aes_index,in_iv,in_key,CRYPTO_AES_KEY_LEN,0,&g_cbcAES);
				if(error != CRYPT_OK)
				{
					dprintf("cbc_start error : %s" , error_to_string(error));
					return RET_CRYPTO_NG;
				}
	
	
				DumpHex("in data", in_cipher_tmp,   in_cipher_len);
				// re-encryptprevious cipher block
				dprintf("\nre-encrypt previous cipher block:\n" );
				DumpHex("last cihper block", in_cipher_tmp-CRYPTO_AES_KEY_LEN, CRYPTO_AES_KEY_LEN);
				//DumpHex("last cihper block", in_cipher_tmp+(CRYPTO_AES_KEY_LEN*(block_count-1)), CRYPTO_AES_KEY_LEN);

			error = cbc_encrypt(in_cipher_tmp-CRYPTO_AES_KEY_LEN, tmp, CRYPTO_AES_KEY_LEN, &g_cbcAES);	
				DumpHex("cihper block", tmp, CRYPTO_AES_KEY_LEN);
	#endif
				dprintf("\nXOR: block_last_len[%d] [%x] [%x]\n", block_last_len, in_cipher_tmp[0], in_cipher_tmp[1]);
				
				i = 0;
				while( i < block_last_len )
				{
					out_plain_tmp[(16*block_count)+i] = tmp[i]^in_cipher_tmp[i];
					dprintf("%02x XOR %02x -->  %02x \n", tmp[i], in_cipher_tmp[i], out_plain_tmp[(16*block_count)+i]);
					++i;
				};
	
			}
		}
	
		cbc_done(&g_cbcAES);

#if 1
		memcpy(out_plain, out_plain_tmp, in_cipher_len);

		DumpHex("out_plain", out_plain_tmp, in_cipher_len);
		if(NULL != out_plain_tmp)
		{
			free(out_plain_tmp);
			out_plain_tmp = NULL;
		}

#endif	
	return RET_CRYPTO_OK ;
}
int test_aescbcofb(void)
{

	//key:
	unsigned char key[] = {0x75, 0x30, 0xa4, 0x40, 0xdd, 0xee, 0x6f, 0xec, 0xe5, 0x0c, 0x72, 0xc0, 0x2f, 0xd7, 0xda, 0x15};
	//iv:
	unsigned char iv[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	// [Message to be encrypted]
	unsigned char plain[] = {
							0x00, 0x00, 0x04, 0x01, 0x00, 0x01, 0x00, 0x00, 0xB2, 0x8C, 0xE8, 0xA4, 0xE3, 0x56, 0xCD, 0x74,
							0x8D, 0xC8, 0x72, 0x4A, 0x8B, 0x24, 0x31, 0xA0, 0x59, 0xB8, 0xBA, 0x42, 0x88, 0xEE, 0x73, 0x2A,
							0x01, 0x2B, 0xC0, 0x88, 0x4C, 0x78, 0x42, 0xED};
					
	unsigned char encrypted[40] = {0};
	unsigned char out_plain[40] = {0};

	crypto_AesCbcofbEncrypt(plain,
						    sizeof(plain),
						    key,
						    iv,
						    encrypted);

	DumpHex("ciphsv:", encrypted, sizeof(encrypted));
	
	dprintf("%s done1\n", __FUNCTION__);

	crypto_AesCbcofbDecrypt(encrypted,
		sizeof(encrypted),
		key,
		iv,
		out_plain);

	DumpHex("out_plain:", out_plain, sizeof(out_plain));
	dprintf("%s done2\n", __FUNCTION__);


	
	while(1);
	return 0;
}

/**
 * Get Random Number
 *
 * @param [in]     in_rnd_len           Random number length
 * @param [out]    out_rnd              Random numbers
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_GetRandomNumber(u_int32_t        in_rnd_len,
                       			unsigned char        *out_rnd)
 {
 	if( (out_rnd == NULL) || ( in_rnd_len <1 ))
		return RET_CRYPTO_ILLEGAL_ARGS;
	int error = 0;
	prng_state prng ;

	#define SYSENTROPY_LEN 10		
	unsigned char sysentropy[SYSENTROPY_LEN]={};
	sprng_read(sysentropy, SYSENTROPY_LEN, NULL);
	 
	if((error = fortuna_start(&prng)!= CRYPT_OK))
		{
		dprintf("fortuna_start error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
		}
	if((error = fortuna_add_entropy(sysentropy,10,&prng)!= CRYPT_OK))
		{
		dprintf("fortuna_start error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
		}
	if((error = fortuna_ready(&prng)!= CRYPT_OK))
		{
		dprintf("fortuna_ready error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
		}

	dprintf("read %lu bytes from fortuna \n" , fortuna_read(out_rnd,in_rnd_len,&prng));

	if((error = fortuna_done(&prng)!= CRYPT_OK))
		{
		dprintf("fortuna_done error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
		}
	
 	return RET_CRYPTO_OK ;
 }

/**
 * EC-DSA Signature Generation
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [in]     in_privkey           Private Key
 * @param [out]    out_sig              Signature
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_EcdsaGenerate(const unsigned char    *in_data,
                     int32_t            in_data_len,
                     const unsigned char     in_privkey[CRYPTO_ECDSA_PRIVKEY_LEN],
                     unsigned char           out_sig[CRYPTO_ECDSA_SIG_LEN])
{
	int err = -1;
	if( (in_data == NULL) || ( in_data_len < 1 ))
		return RET_CRYPTO_ILLEGAL_ARGS;

	unsigned char out_pubkey[2*CRYPTO_ECDSA_PRIVKEY_LEN] = {0x0};



	
	err = ecc_sign_hash_emsa1_sha256(in_privkey,
								//CRYPTO_ECDSA_PRIVKEY_LEN,
								in_data,
								in_data_len,
								out_sig,
								CRYPTO_ECDSA_SIG_LEN,
								&sony_ecc_para,
								out_pubkey);

	return err ;

}




/**
 * EC-DSA Signature Verifycation
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [in]     in_pubkey            Public Key
 * @param [in]     in_sig               Signature
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_EcdsaVerify(const unsigned char      *in_data,
                   int32_t              in_data_len,
                   const unsigned char       in_pubkey[CRYPTO_ECDSA_PUBKEY_LEN],
                   const unsigned char       in_sig[CRYPTO_ECDSA_SIG_LEN]) 
{
//	int  out_sign_len = CRYPTO_ECDSA_SIG_LEN;
	int ret = -1;




	DumpHex("in_pubkey", in_pubkey, CRYPTO_ECDSA_PUBKEY_LEN);
	DumpHex("in_data", in_data, in_data_len);


	ret = ecc_verify_hash_emsa1_sha256( in_pubkey, 
										//CRYPTO_ECDSA_PRIVKEY_LEN,
										in_data, 
										in_data_len, 
										in_sig, 
										//out_sign_len, 
										&sony_ecc_para);
	if(0 != ret)
	{
		dprintf("\tERROR:%s [%d]\n", __FUNCTION__, ret);
	}


	return ret ;

}
/**
 * EC-DH
 *
 * @param [in]     in_svr_p1v           Server Phase 1 Value
 * @param [out]    out_client_p1v       Client Phase 1 Value
 * @param [out]    out_pubkey           Public Key
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_Ecdh(const unsigned char              in_svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_client_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            unsigned char                    out_pubkey[CRYPTO_ECDH_PUBKEY_LEN])
{
	if( (in_svr_p1v == NULL) || (out_client_p1v == NULL ) || (out_pubkey == NULL ))
		return RET_CRYPTO_ILLEGAL_ARGS;
	
	// generate the client p1v:

	//Q = a*b*G


	//printf("do ecc_dh:\n");





	ecc_dh(in_svr_p1v, out_client_p1v, out_pubkey);
	
	//DumpHex("out_client_p1v:", out_client_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN);
	//DumpHex("out_pubkey:", out_pubkey, CRYPTO_ECDH_PUBKEY_LEN);


	//DumpHex("server p1v:", in_svr_p1v, CRYPTO_ECDH_PHASE1VALUE_LEN);


	
	return RET_CRYPTO_OK ;
}


/**
 * SHA-256
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [out] out_hash                Hash value
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
u_int32_t
crypto_Sha256Hash(const unsigned char       *in_data,
                  int32_t               in_data_len,
                  unsigned char              out_hash[CRYPTO_SHA256_DIGEST_LEN])

{
	if( (in_data == NULL) || ( in_data_len < 1 ) )
			return RET_CRYPTO_ILLEGAL_ARGS;
	
	hash_state md ;

	memset(out_hash, 0, CRYPTO_SHA256_DIGEST_LEN);
	//dprintf("sha256 indata len:%d\n", in_data_len);
	//DumpHex("Sha256Hash in data:", in_data, in_data_len);

	tom_sha256_init(&md);
	
	sha256_process(&md, in_data, in_data_len);
	
	sha256_done(&md,out_hash);


	//DumpHex("sha256:", out_hash, CRYPTO_SHA256_DIGEST_LEN);

	
	return RET_CRYPTO_OK ;
}


/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
/**
 * Global init
 *
 * @retval RET_CRYPTO_OK                Success
 */
u_int32_t
crypto_GlobalInit(void)
{

	// to-do be called somewhere
	if(register_prng(&fortuna_desc) == -1)
	{
		dprintf("register_prng error fortuna_desc fail\n" );
		return RET_CRYPTO_NG;
	}	

	if(register_prng(&yarrow_desc) == -1)
	{
		dprintf("register_prng error yarrow_desc fail\n" );
		return RET_CRYPTO_NG;
	}	
	if(register_hash(&sha256_desc) == -1)
	{
		dprintf("register_hash error sha256_desc fail \n" );
		return RET_CRYPTO_NG;
	}	

	return RET_CRYPTO_OK ;
}

/**
 * Global fin
 *
 * @retval RET_CRYPTO_OK                Success
 */
u_int32_t
crypto_GlobalFin(void)
{
	int error =  0;  //return value
	
	error = unregister_prng(&fortuna_desc);
	
	if(error != CRYPT_OK)
	{
		dprintf("unregister_prng fortuna_desc error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
	}	

	error = unregister_prng(&yarrow_desc);

	if(error != CRYPT_OK)
	{
		dprintf("unregister_prng  yarrow_desc error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
	}	

	error = unregister_hash(&sha256_desc);
	if(error != CRYPT_OK)
	{
		dprintf("unregister_hash  sha256_desc error : %s\n" , error_to_string(error));
		return RET_CRYPTO_NG;
	}	
	
	return RET_CRYPTO_OK ;
}





