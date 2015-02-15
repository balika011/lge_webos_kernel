/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "CommonIncludes.h"

#define teeMAX_TEMP_CRYPTO_BUF_SIZE	(64 * 1024 + 128)
static uint8_t saTempCryptoBuf[teeMAX_TEMP_CRYPTO_BUF_SIZE];

static uint8_t saTempCryptoBuf_1[teeMAX_TEMP_CRYPTO_BUF_SIZE];



#define TEE_PLAINTEXT_PADDING_SIZE 				(16)


#define MAX_ID_FILE_DATA_LEN					2048

static const uint8_t default_iv[] = {  0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,};


/***************************************************************************
 * @function compare_buffer
 *
 * @brief if buf1 equals buf2
 *
 * @param[in] buf1 - pointer to buffer1
 * @param[in] buf2 - pointer to buffer2
 * @param[in] len1 - length of buffer1
 * @param[in] len2 - length of buffer2
 *
 * @return   
 *
 **************************************************************************/

uint32_t compare_buffer(const uint8_t* buf1 , const uint8_t* buf2,uint32_t len1,uint32_t len2)
{
	if(len1 != len2)
		return -1;
	return x_memcmp(buf1,buf2,len1);
}



uint32_t aes_ecb_128_decrypt(const uint8_t* pul_key ,int ul_key_len,const uint8_t* pul_in_data ,uint32_t ul_in_data_len ,uint8_t* pul_out_data)
{
	symmetric_ECB ecb;
	int index = 0;
	int ret = 0;

	ret = register_cipher(&aes_desc);
	if(-1 == ret){
		//printf("register cipher fail. \n");
		return -1;
	}

	index =find_cipher("aes");
	if(-1 == index){
		//printf("find cipher aes fail. \n");
		return -1;
	}
	ret = ecb_start(index,pul_key,ul_key_len,0 ,&ecb);
	if(CRYPT_OK != ret){
		//printf("ecb_start fail. \n");
		return -1;
	}
	ecb_decrypt(pul_in_data,pul_out_data,ul_in_data_len,&ecb);
	ecb_done(&ecb);

	return 0;
}



uint32_t i4_tee_nfx_encrypt(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
									const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t i4Dst_max_len ,/*out*/uint32_t *u4Dst_real_len)
{
	////printf("[nfx_tee_test] i4_tee_nfx_encrypt enter .\n");
	//int i= 0 ;
	
	uint8_t*	pul_plaintext = NULL;		//plain text including padding
	uint32_t	ui4_encryptLen = 0; 		//eccrypt text len which is equal to plain text of including padding
	uint8_t*	pul_encryptData = NULL; //temp buffer of encrypt plain text
	
	uint32_t cipher_index = -1;
	uint32_t i4_ret = -1;
	
	symmetric_CBC t_sym_cbc;

	/*register cipher */
	i4_ret = register_cipher(&aes_desc);
	if(i4_ret < 0)
	{
		////printf("[trustzone],register cipher fail .\n");
		return -1;
	}
	
	/* padding	16bytes , the encode/plain text must be padding 16bytes */ 
	uint32_t	ui4_plaintext_offset = 0;
	uint32_t	ui4_plaintext_trunc = 0;
	uint8_t	aul_padding[TEE_PLAINTEXT_PADDING_SIZE] = {0};
	ui4_plaintext_trunc = u4SrcLen % TEE_PLAINTEXT_PADDING_SIZE ; //truncate len
	ui4_plaintext_offset = u4SrcLen - ui4_plaintext_trunc ;   //(n*TEE_PLAINTEXT_PADDING_SIZE)  len
	x_memset(aul_padding,TEE_PLAINTEXT_PADDING_SIZE-ui4_plaintext_trunc,TEE_PLAINTEXT_PADDING_SIZE*sizeof(uint8_t));
	x_memcpy(aul_padding,pu1SrcBuf+ui4_plaintext_offset,ui4_plaintext_trunc*sizeof(uint8_t));
	
	/*length includes padding*/
	ui4_encryptLen = ui4_plaintext_offset + TEE_PLAINTEXT_PADDING_SIZE ;

	//alloc pul_encryptData buf
	x_memset(saTempCryptoBuf,0,teeMAX_TEMP_CRYPTO_BUF_SIZE);
	pul_encryptData = saTempCryptoBuf;
	
	//pul_encryptData = (uint8_t*)x_alloc_s_dram_mem(ui4_encryptLen);
	if(NULL == pul_encryptData)
	{
		////printf("[trustzone],alloc memory fail .\n");
		return -1;
	}
	// alloc pul_plaintext
	x_memset(saTempCryptoBuf_1,0,teeMAX_TEMP_CRYPTO_BUF_SIZE);
	pul_plaintext = saTempCryptoBuf_1;
	if(NULL == pul_plaintext)
	{
		////printf("[trustzone],alloc memory fail .\n");
		return -1;
	}
	
	x_memcpy(pul_plaintext , pu1SrcBuf ,ui4_plaintext_offset*sizeof(uint8_t));
	x_memcpy(pul_plaintext + ui4_plaintext_offset,aul_padding,TEE_PLAINTEXT_PADDING_SIZE *sizeof(uint8_t));

	/* encrypt */
	cipher_index = find_cipher("aes");
	if(cipher_index < 0)
	{
		//printf("[trustzone],find cipher aes fail .\n");
		return -1 ;
	}		
	i4_ret = cbc_start(cipher_index, pulIV, pulKeyBuf, u4KeyLen, 0, &t_sym_cbc);
	cbc_encrypt(pul_plaintext,pul_encryptData,ui4_encryptLen,&t_sym_cbc);

	cbc_done(&t_sym_cbc);
	
	//copy
	if(ui4_encryptLen > i4Dst_max_len)
	{
		//printf("encode dest buffer size is not enough .\n");
		return -1;
	}
	x_memcpy(pu1DstBuf,pul_encryptData,ui4_encryptLen );
	*u4Dst_real_len = ui4_encryptLen;
	/*unregister cipher*/
	//unregister_cipher(&aes_desc);

	//x_free_s_dram_mem(pul_encryptData);
	//x_free_s_dram_mem(pul_plaintext);
	return 0;

}

uint32_t i4_tee_nfx_decrypt(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
								const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t u4DstLen,/*out*/uint32_t *u4Des_real_len)
{
	int i = 0;

	symmetric_CBC t_sym_cbc;
	uint8_t ul_padding = 0;
	uint32_t l_plainSize = 0;

	l_plainSize = u4SrcLen;

	/* decrypt */
	x_memset(pu1DstBuf, 0, u4SrcLen);
	register_cipher(&aes_desc);
	cbc_start(find_cipher("aes"), pulIV, pulKeyBuf, u4KeyLen, 0, &t_sym_cbc);
	cbc_decrypt(pu1SrcBuf, pu1DstBuf, l_plainSize, &t_sym_cbc);
	cbc_done(&t_sym_cbc);
	/*delete padding in the rear*/
	ul_padding = pu1DstBuf[l_plainSize - 1] ;

	for(i = 0 ; i < l_plainSize ; i ++)
	{
		if(ul_padding != pu1DstBuf[l_plainSize - i - 1])
		break;
	}
	if(ul_padding != i)
	{
		//printf("[nfx_tee_test] padding error . \n");
		return -1;
	}
	pu1DstBuf[l_plainSize - ul_padding] = '\0' ;
	*u4Des_real_len = l_plainSize - ul_padding;
	return 0;
}


uint32_t i4_tee_nfx_decrypt_no_del_rear_padding(const uint8_t* pulKeyBuf, uint32_t u4KeyLen ,const uint8_t* pulIV ,uint32_t u4IvLen ,
								const uint8_t *pu1SrcBuf, uint32_t u4SrcLen, /*out*/uint8_t *pu1DstBuf, uint32_t u4DstLen,/*out*/uint32_t *u4Des_real_len)
{
	//int i = 0;

	symmetric_CBC t_sym_cbc;
	//uint8_t ul_padding = 0;
	uint32_t l_plainSize = 0;

	l_plainSize = u4SrcLen;

	/* decrypt */
	x_memset(pu1DstBuf, 0, u4SrcLen);
	register_cipher(&aes_desc);
	cbc_start(find_cipher("aes"), pulIV, pulKeyBuf, u4KeyLen, 0, &t_sym_cbc);
	cbc_decrypt(pu1SrcBuf, pu1DstBuf, l_plainSize, &t_sym_cbc);
	cbc_done(&t_sym_cbc);
	
	*u4Des_real_len = l_plainSize;
	return 0;
}


uint32_t aes_unwrap_key(const uint8_t *key, const uint8_t *iv,
		uint8_t *out,
		const uint8_t *in, unsigned int inlen)
{
	uint8_t *A, B[16], *R;
	uint8_t T[16];
	unsigned int  ii, j, t;
	inlen -= 8;
	if (inlen & 0x7)
		return -1;
	if (inlen < 8)
		return -1;
	A = B;
	t =  6 * (inlen >> 3);
	x_memcpy(A, in, 8);
	x_memcpy(out, in + 8, inlen);
	for (j = 0; j < 6; j++)
	{
		R = out + inlen - 8;
		for (ii = 0; ii < inlen; ii += 8, t--, R -= 8)
			{
			A[7] ^= (uint8_t)(t & 0xff);
			if (t > 0xff)	
			{
				A[6] ^= (uint8_t)((t >> 8) & 0xff);
				A[5] ^= (uint8_t)((t >> 16) & 0xff);
				A[4] ^= (uint8_t)((t >> 24) & 0xff);
			}
			x_memcpy(B + 8, R, 8);
			aes_ecb_128_decrypt(key,16,B,16,T);
			x_memcpy(B,T,16);
			x_memcpy(R, B + 8, 8);
			}
	}
	if (!iv)
		iv = default_iv;
	if (x_memcmp(A, iv, 8))
		return 0;
	return inlen;
}



