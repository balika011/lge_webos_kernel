#ifndef MTKCRYPTO_GLUE_
#define MTKCRYPTO_GLUE_

#include "type_c.h"

//#define USE_OPENSSL

#define SHA256_DIGEST_LENGTH    32
#define SHA1_DIGEST_LENGTH      20
#define AES_BLOCK_SIZE          16
#define SHA_DIGEST_LENGTH       20


#ifndef USE_OPENSSL

#include "tomcrypt.h"
typedef rsa_key RSA;

#else

#include <openssl/rsa.h>

#endif

bool RAND_GEN(uint8_t *bytes, size_t size);

bool CMAC_AES( const uint8_t *key, size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out, size_t *outlen);

bool HMAC_SHA256( const uint8_t *key,size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out,size_t *outlen);

bool HMAC_SHA1(const uint8_t *key, size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out, size_t *outlen);


bool HASH_SHA256(const uint8_t *in,  size_t inlen, uint8_t *out);

bool HASH_SHA1( const uint8_t *in,  size_t inlen, uint8_t *hash);

bool AES_CBC_ENCRYPT(const uint8_t *key, size_t keylen, const uint8_t * iv,
        const uint8_t *in, uint8_t *out, const size_t len);

bool AES_CBC_DECRYPT(const uint8_t *key, size_t keylen, const uint8_t * iv,
        const uint8_t *in, uint8_t *out, const size_t len);

bool AES_CBC_DECRYPT_UPDATE_IV(const uint8_t *key, size_t keylen,
        uint8_t * iv,
        const uint8_t *in, uint8_t *out, const size_t len);


bool AES_CTR_DECRYPT(const uint8_t *key, size_t keylen, const uint8_t *iv,
        const uint8_t *in, uint8_t *out, const size_t len,
        const unsigned long block_offset);


bool RSA_LOAD_PKCS8_KEY (RSA **key,  uint8_t* pkcs8_rsa_key, size_t rsa_keylength);

size_t RSA_GET_SIZE(RSA *key);

void RSA_KEY_TERM(RSA **key);

bool RSA_DECRYPT(RSA *key , const uint8_t * in, const size_t inlen,
        uint8_t *out , size_t *outlen);

//PSS       Padding = 0
//PKCS#1    Padding = 1
bool RSA_SIGN(RSA *key , const uint8_t * message, const size_t message_length,
        uint8_t *signature , size_t *signature_length , int padding);

#endif        
