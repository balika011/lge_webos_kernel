#ifndef __DEF_DEFINE_H__
#define __DEF_DEFINE_H__

//#ifdef CRYPT_AES128
#define DEF_ENC_KEY_LEN 16
#define DEF_ENC_IV_LEN 16
#define DEF_CHALLENGE_LEN DEF_ENC_KEY_LEN
#define DEF_RESPONSE_LEN DEF_ENC_KEY_LEN
#define DEF_B64_CHALLENGE_LEN (DEF_ENC_KEY_LEN*2)
#define DEF_B64_RESPONSE_LEN (DEF_ENC_KEY_LEN*2)
//#endif /*CRYPT_AES128*/
//#define DEBUG

#define DEF_SERIAL_DST_LEN 8
#define DEF_RANDOM_LEN 4
#define DEF_RANDOM_DST_LEN 16

#define DEF_MAX_SERIAL_NO 128
#define DEF_MAX_INPUTL_LEN 1024

#define CSL_ERR_PRINT(fmt, args...) printf("%s(%d)" fmt, __FUNCTION__, __LINE__, ## args)
#ifdef DEBUG
#define CSL_PRINT(fmt, args...) printf(fmt, ## args)
#else /*DEBUG*/
#define CSL_PRINT(fmt, args...)
#endif



#endif /*__DEF_DEFINE_H__*/

