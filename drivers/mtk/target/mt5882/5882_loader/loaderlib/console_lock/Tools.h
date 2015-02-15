#ifndef __DEF_TOOLS_H__
#define __DEF_TOOLS_H__

#include <stdint.h>

void dump( const unsigned char *buf, int len, const char *text );
int combine_data( const unsigned char* data1, int data1_len, const unsigned char *data2,
                   int data2_len, unsigned char *out_data, int out_len );
int split_data( const unsigned char* data, int data_len, unsigned char *out_data1,
                int out_len1, unsigned char *out_data2, int out_len2 );
void pickup_top_data( const unsigned char *src, int len, unsigned char *dst );
void uint32touchar(  uint32_t src, unsigned char *dst );
void get_random_number( unsigned char *random_number );
int aes128_encrypt( const unsigned char *src, int src_len, const unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *dst );
int aes128_decrypt( const unsigned char *src, int src_len, const unsigned char *key, int key_len,
                    unsigned char *iv, int iv_len, unsigned char *dst );
int base64_encode( const unsigned char *src, int src_len, unsigned char *dst, int dst_len );
int base64_decode( const unsigned char *src, int src_len, unsigned char *dst, int dst_len );


#endif /*__DEF_TOOLS_H__*/
