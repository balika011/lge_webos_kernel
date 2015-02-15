#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "x_printf.h"
#include "x_drv_cli.h"
#include "x_assert.h"
#include "x_serial.h"
#include "x_stl_lib.h"
#include "x_timer.h"
#include "x_hal_arm.h"
#include "x_hal_5381.h"
#include "x_bim.h"
#include "x_pdwnc.h"
#include "drvcust_if.h"
#include "x_typedef.h"
#include "x_hal_5381.h"
#include "loader_imghdr.h"
#include "loader_if.h"
#include "eeprom_if.h"
#include "c_model.h"


#include "gcpu_if.h"
#include "sha256.h"


#include "Define.h"
#include "Tools.h"

// Maximum value returned by rand()
#define RAND_MAX  2147483647

//=====================================================================
// Algorithm for random number generation. Default is RAND_KNUTH

typedef enum
{
	RAND_SIMPLEST,
	RAND_SIMPLE,
	RAND_KNUTH
} RAND_ALGORITHM;

static unsigned int s_rand_seed = 0x123456;
static RAND_ALGORITHM s_algo = RAND_KNUTH;

// FUNCTIONS
static int R_rand(void)
{
	HAL_TIME_T t0;
	HAL_GetTime(&t0);
	s_rand_seed = t0.u4Micros;

	return rand_r(&s_rand_seed);
} // rand()

int rand_r(unsigned int *seed)
{
	unsigned int s, t;
	unsigned int uret;
	int retval = 0;

	switch (s_algo)
	{
	case RAND_SIMPLEST:
		// This algorithm sucks in the lower bits
		*seed = (*seed * 1103515245) + 12345; // permutate seed
		retval = (int)( *seed & RAND_MAX );
		break;

	case RAND_SIMPLE:
		// The above algorithm sucks in the lower bits, so we shave them off
		// and repeat a couple of times to make it up

		s = *seed;
		s = (s * 1103515245) + 12345; // permutate seed
		// Only use top 11 bits
		uret = s & 0xffe00000;

		s = (s * 1103515245) + 12345; // permutate seed
		// Only use top 14 bits
		uret += (s & 0xfffc0000) >> 11;

		s = (s * 1103515245) + 12345; // permutate seed
		// Only use top 7 bits
		uret += (s & 0xfe000000) >> (11+14);

		retval = (int)(uret & RAND_MAX);
		*seed = s;
		break;

	case RAND_KNUTH:
		// This is the code supplied in Knuth Vol 2 section 3.6 p.185 bottom

#define MM 2147483647    // a Mersenne prime
#define AA 48271         // this does well in the spectral test
#define QQ 44488         // (long)(MM/AA)
#define RR 3399          // MM % AA; it is important that RR<QQ

		s = AA * (*seed % QQ);
		t = RR * (unsigned int)(*seed / QQ);
		if (s > t)
			*seed = s - t;
		else
			*seed = MM+ s - t;

		retval = (int)(*seed & RAND_MAX);
		break;

	default:
		// No valid algorithm selected
//		ASSERT(0);
		break;
	}

    return retval;
} // rand_r()

#ifdef DEBUG
void dump( const unsigned char *buf, int len, const char *text ){
    int i;
    printf("dump %s",text);
    for( i = 0; i < len; i++ ){
        if( (i) % 16 == 0 ){
            printf("\n");
        }

        printf("%02x ",(unsigned char)*(buf+i)&0xff);
        if( (i != 0) && ((i+1) % 8 == 0) && ((i+1) % 16 != 0) ){
            printf("- ");
        }
    }
    printf("\n");
}
#else /*DEBUG*/
void dump( const unsigned char *buf, int len, const char *text ){}
#endif /*DEBUG*/

int combine_data( const unsigned char* data1, int data1_len, const unsigned char *data2, 
                  int data2_len, unsigned char *out_data, int out_len )
{
    if( out_len < (data1_len + data2_len) ){
        CSL_ERR_PRINT("out_len(%d) > data1_len+data2_len(%d)\n",out_len, data1_len+data2_len);
        return -1;
    }

    int i;
    for( i = 0; i < data1_len; i++ ){
        *(out_data+i) = *(data1+i);
    }
    for( i = 0; i < data2_len; i++ ){
        *(out_data+i+data1_len) = *(data2+i);
    }

    dump(data1, data1_len,"data1");
    dump(data2, data2_len,"data2");
    dump(out_data, data1_len+data2_len,"out_data");

    return 0;
}

int split_data( const unsigned char* data, int data_len, unsigned char *out_data1,
                 int out_len1, unsigned char *out_data2, int out_len2 )
{
    if( data_len < (out_len1 + out_len2) ){
        CSL_ERR_PRINT("data(%d) < out_len1+out_len2(%d)\n",data_len, out_len1+out_len2);
        return -1;
    }
    int i;
    for( i = 0; i < out_len1; i++ ){
        *(out_data1+i) = *(data+i);
    }
    for( i = 0; i < out_len2; i++ ){
        *(out_data2+i) = *(data+i+out_len1);
    }

    dump(data, out_len1+out_len2,"data");
    dump(out_data1, out_len1,"out_data1");
    dump(out_data2, out_len2,"out_data2");
}

void pickup_top_data( const unsigned char *src, int len, unsigned char *dst )
{
    int i;
    for( i = 0; i < len; i++ ){
        *(dst+i) = *(src+i);
    }
}

void uint32touchar(  uint32_t src, unsigned char *dst )
{
    int i;
    char *tmp;
    tmp = (char*)&src;
    for( i = 0; i < sizeof(src); i++ ){
        *(dst+i) = *(tmp+i);
    }
}

void get_random_number( unsigned char *random_number )
{
    //uint32_t random = (uint32_t)0x12345678;//rand();
    
    uint32_t random = (uint32_t)R_rand();
    uint32touchar( random, random_number );
    CSL_PRINT("random=0x%08x\n",random);
    dump( random_number, DEF_RANDOM_LEN, "random_number");
}


int aes128_encrypt( const unsigned char *src, int src_len, const unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *dst )
{
    int ret;
    unsigned char data_for_16byte[16];
	x_memset(data_for_16byte,0x22,16);
	
    if( key_len != DEF_ENC_KEY_LEN ){
        CSL_ERR_PRINT("key length %d is not %d\n",key_len, DEF_ENC_KEY_LEN);
        return -1;
    }

    if( iv_len != DEF_ENC_IV_LEN ){
        CSL_ERR_PRINT("iv length %d is not %d\n",iv_len, DEF_ENC_IV_LEN);
        return -1;
    }
	
	GCPU_LoaderInit(0);
#if 1//defined(CC_MT5880)
	_GCPU_SetAesKey(key, DEF_ENC_KEY_LEN*8, iv);
#endif
	if(src_len != 16)
		{
			x_memcpy(data_for_16byte,src,src_len);
			dump(data_for_16byte,16,"data_for_16byte before aes");
			GCPU_AesEncrypt(data_for_16byte,dst,16);
		}
	else
		{
			
			GCPU_AesEncrypt(src,dst,16);
		}
	
    return 0;
}

int aes128_decrypt( const unsigned char *src, int src_len, const unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *dst )
{
    int ret;
	
    if( key_len != DEF_ENC_KEY_LEN ){
        CSL_ERR_PRINT("key length %d is not %d\n",key_len, DEF_ENC_KEY_LEN);
        return -1;
    }

    if( iv_len != DEF_ENC_IV_LEN ){
        CSL_ERR_PRINT("iv length %d is not %d\n",iv_len, DEF_ENC_IV_LEN);
        return -1;
    }
	GCPU_LoaderInit(0);
	
#if 1//defined(CC_MT5880)
	_GCPU_SetAesKey(key, DEF_ENC_KEY_LEN*8, iv);
#endif
	GCPU_AesDecrypt(src,dst,src_len);
    return 0;
}

int base64_encode( const unsigned char *src, int src_len, unsigned char *dst, int dst_len )
{
    int ret;
    int enc_len;
    enc_len = b64_encode(NULL, src_len, NULL,0);

    if( enc_len > dst_len ){
        CSL_ERR_PRINT("encode_length(%d) > max_len(%d)\n",enc_len, dst_len);
        return -1;
    }
    CSL_PRINT("base64 encode size %d->%d\n",src_len,enc_len);
    ret = b64_encode(src, src_len, dst, enc_len);

    dump(src,src_len,"base64 before");
    dump(dst,enc_len,"base64 after");
    return ret;
}

int base64_decode( const unsigned char *src, int src_len, unsigned char *dst, int max_len )
{
    int ret;
    int dec_len;
    char *data;

    dec_len = b64_decode(NULL, src_len, NULL,0);
    data = x_mem_alloc( dec_len );
    if( data == NULL ){
        CSL_ERR_PRINT("Cannot malloc\n");
        return -1;
    }

    ret = b64_decode(src, src_len, data, dec_len);

    if( ret > max_len ){
        CSL_ERR_PRINT("decode_length(%d) > max_len(%d)\n",ret, max_len);
        x_mem_free(data);
        return -1;
    }

    x_memcpy( dst, data, max_len );

    dump(src,src_len,"base64 before");
    dump(dst,ret,"base64 after");

    x_mem_free(data);
    return ret;
}


