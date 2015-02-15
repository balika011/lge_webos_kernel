#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include "Define.h"
#include "Tools.h"

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
#include "sha256.h"
#include "gcpu_if.h"

#define SHA224_DIGEST_LENGTH	28
#define SHA256_DIGEST_LENGTH	32

int get_serial_digest( const unsigned char *serial_no, int serial_len, const unsigned char *enc_key, 
                       int key_len, unsigned char *serial_digest )
{
    unsigned char *serial_and_key;
    unsigned char full_serial_digest[SHA256_DIGEST_LENGTH];
    serial_and_key = (unsigned char*)x_mem_alloc( serial_len + key_len );
    if( serial_and_key == NULL ){
        CSL_ERR_PRINT("cannot malloc\n");
        return -1;
    }

    CSL_PRINT("get_serial_digest: combine serial_no and key.\n");
    combine_data( serial_no, serial_len, enc_key, key_len, serial_and_key, serial_len + key_len );

    CSL_PRINT("get_serial_digest: calculate serial_no and key.\n");
    SHA256( serial_and_key ,serial_len + key_len, full_serial_digest );
    dump( full_serial_digest, SHA256_DIGEST_LENGTH, "full_serial_digest");
    x_mem_free(serial_and_key);

    CSL_PRINT("get_serial_digest is used top 64 bit.\n");
    pickup_top_data(full_serial_digest, DEF_SERIAL_DST_LEN, serial_digest );
    dump( serial_digest, DEF_SERIAL_DST_LEN, "serial_digest");
    return 0;
}

int get_random_digest( const unsigned char *random_number, int random_len, const unsigned char *enc_key,
                       int key_len, unsigned char *random_digest )
{
    unsigned char random_and_key[DEF_RANDOM_LEN+DEF_ENC_KEY_LEN];
    unsigned char full_random_digest[SHA256_DIGEST_LENGTH];

    CSL_PRINT("get_random_digest: combine random_number and key.\n");
    combine_data( random_number, random_len, enc_key, key_len, random_and_key, random_len + key_len );
    SHA256( random_and_key ,random_len + key_len, full_random_digest );
    dump( full_random_digest, SHA256_DIGEST_LENGTH, "full_random_digest");

    CSL_PRINT("get_random_digest: random_digest is top 64bit.\n");
    pickup_top_data(full_random_digest, DEF_RANDOM_DST_LEN, random_digest );
    dump( random_digest, DEF_RANDOM_DST_LEN, "random_digest");
    return 0;
}

int create_challenge( const unsigned char *serial_digest, const unsigned char *random_number, const unsigned char *key,
                      int key_len, unsigned char *iv, int iv_len, unsigned char* challenge )
{
    int ret;
    unsigned char data[DEF_SERIAL_DST_LEN + DEF_RANDOM_LEN];
   // unsigned char src_challenge[DEF_CHALLENGE_LEN];

    CSL_PRINT("create_challenge: combine serial_digest and random_number\n");
    combine_data( serial_digest, DEF_SERIAL_DST_LEN, random_number, DEF_RANDOM_LEN, data, DEF_SERIAL_DST_LEN + DEF_RANDOM_LEN );

    //use AES128
    CSL_PRINT("create_challenge: encrypt challenge by AES128\n");
    ret = aes128_encrypt( data, DEF_SERIAL_DST_LEN + DEF_RANDOM_LEN, key, key_len, iv, iv_len, challenge );
    if( ret != 0 ){
        CSL_ERR_PRINT("aes128_encrypt error\n");
        return -1;
    }

    dump(challenge, DEF_CHALLENGE_LEN, "challenge");
    return 0;
}

int create_response( const unsigned char *random_digest, const unsigned char *key,
                      int key_len, unsigned char *iv, int iv_len, unsigned char* response )
{
    int ret;
    CSL_PRINT("random_digest is encrypted by aes128.\n");
    ret = aes128_encrypt( random_digest, DEF_RANDOM_DST_LEN, key, key_len, iv, iv_len, response );
    if( ret != 0 ){
        CSL_ERR_PRINT("aes128_encrypt error\n");
        return -1;
    }
    dump(response, DEF_RESPONSE_LEN, "response");
    return 0;
}


int decode_challenge( const unsigned char *challenge, const unsigned char *key, int key_len,
                      unsigned char *iv, int iv_len, unsigned char* serial_digest, unsigned char *random_number )
{
   // int ret;
    unsigned char data[DEF_ENC_KEY_LEN];

    CSL_PRINT("decode_challenge: decrypt challenge by aes128\n");
    aes128_decrypt( challenge, DEF_CHALLENGE_LEN, key, key_len, iv, iv_len, data );

    dump( data, DEF_ENC_KEY_LEN, "decoded challenge");
    CSL_PRINT("decode_challenge: split challenge between serial_digest and random_number\n");
    split_data( data, DEF_SERIAL_DST_LEN+DEF_RANDOM_LEN, serial_digest, DEF_SERIAL_DST_LEN, random_number, DEF_RANDOM_LEN );
	
    return 0;
}

int decode_response( const unsigned char *response, const unsigned char *key, int key_len,
                     unsigned char *iv, int iv_len, unsigned char* random_digest )
{
   // int ret;
    unsigned char data[DEF_ENC_KEY_LEN];
    CSL_PRINT("decode_response: random_digest is decoded by aes128.\n");
    aes128_decrypt( response, DEF_RESPONSE_LEN, key, key_len, iv, iv_len, data );

    dump( data, DEF_ENC_KEY_LEN, "decoded response");
    pickup_top_data(data, DEF_RANDOM_DST_LEN, random_digest );
	
    return 0;
}

int CreateChallenge( const unsigned char *serial_no, const unsigned char *random_number, unsigned char *challenge,
                     const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len )
{
    int ret;
    unsigned char serial_digest[DEF_SERIAL_DST_LEN];

    ret = get_serial_digest( serial_no, x_strlen( (const char *)serial_no ), enc_key, key_len, serial_digest );
    if( ret != 0 ){
        CSL_ERR_PRINT("get_serial_digest error\n");
        return -1;
    }

    ret = create_challenge( serial_digest, random_number, enc_key, key_len, enc_iv, iv_len, challenge );
    if(  ret != 0 ){
        CSL_ERR_PRINT("create_challenge error\n");
        return -1;
    }

    return 0;
}

int VerifyChallenge( const unsigned char *serial_no, const unsigned char *challenge, unsigned char *random_number,
                     const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len )
{
    int ret;
    unsigned char serial_digest[DEF_SERIAL_DST_LEN];
    unsigned char c_serial_digest[DEF_SERIAL_DST_LEN];

    ret = get_serial_digest( serial_no, x_strlen((const char *) serial_no ), enc_key, key_len, serial_digest );
    if( ret != 0 ){
        CSL_ERR_PRINT("get_serial_digest error\n");
        return -1;
    }

    ret = decode_challenge( challenge, enc_key, key_len, enc_iv, iv_len, c_serial_digest, random_number);

    ret = x_memcmp( serial_digest, c_serial_digest, DEF_SERIAL_DST_LEN );
    if( ret != 0 ){
        CSL_ERR_PRINT("serial digest is not same.\n");
        dump(serial_digest, DEF_SERIAL_DST_LEN, "input serial:" );
        dump(c_serial_digest, DEF_SERIAL_DST_LEN, "serial in Challenge:" );
        return -1;
    }

    CSL_PRINT("serial_digest is same!\n");
    return 0;
}

int CreateResponse( const unsigned char *random_number, unsigned char *response,
                    const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len )
{
    int ret;
    unsigned char random_digest[DEF_RANDOM_DST_LEN];
    ret = get_random_digest( random_number, DEF_RANDOM_LEN, enc_key, key_len, random_digest );

    ret = create_response( random_digest, enc_key, key_len, enc_iv, iv_len, response );
    if(  ret != 0 ){
        CSL_ERR_PRINT("create_response error\n");
        return -1;
    }

    return 0;
}

int VerifyResponse( const unsigned char *random_number, unsigned char *response,
                    const unsigned char *enc_key, int key_len, unsigned char *enc_iv, int iv_len )
{
    int ret;
    unsigned char random_digest[DEF_RANDOM_DST_LEN];
    unsigned char r_random_digest[DEF_RANDOM_DST_LEN];
    ret = get_random_digest( random_number, DEF_RANDOM_LEN, enc_key, key_len, random_digest );

    ret = decode_response( response, enc_key, key_len, enc_iv, iv_len, r_random_digest );

    ret = x_memcmp( random_digest, r_random_digest, DEF_RANDOM_DST_LEN );
    if( ret != 0 ){
        CSL_ERR_PRINT("random digest is not same.\n");
        dump(random_digest, DEF_RANDOM_DST_LEN, "create response:" );
        dump(r_random_digest, DEF_RANDOM_DST_LEN, "random in Response:" );
        return -1;
    }

    CSL_PRINT("random_digest is same!\n");

    return 0;
}

