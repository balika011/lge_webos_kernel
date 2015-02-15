
#include "tz_dmx_if.h"

#include "mtkcrypto_glue.h"
#include "mtkutil_vector.h"

#include "log.h"


#ifdef USE_OPENSSL

#include "openssl/aes.h"
#include "openssl/bio.h"
#include "openssl/cmac.h"
#include "openssl/err.h"
#include "openssl/evp.h"
#include "openssl/hmac.h"
#include "openssl/rand.h"
#include <openssl/rsa.h>
#include "openssl/sha.h"
#include "openssl/x509.h"
#else

#include "tomcrypt.h"
#endif


#define BER_TAG(x) ((x)&0x1f)
#define BER_INTERGER 0x02
#define BER_OCT_STR  0x04
#define BER_SEQUENCE 0x10



#define PKCS1_RSA_LENGTH 0x800


#define USE_GCPU_DECRYPT
#ifdef USE_GCPU_DECRYPT

#include "gcpu_if.h"
//#include "dmx_drm_drvif.h"

//define the const inside dmx_drm directly due to library naming conflict
#define AES_SVP_CTR_KEY_PTR (76) 
#define AES_SVP_CBC_KEY_PTR   (68)

extern INT32 TZ_SVP_AES_CTR(UINT8 *pu1Src, UINT8 *pu1Dst, UINT32 u4Len, UINT8 *pu1Ctr, UINT8 *pu1RetCtr);
extern void TZ_SWD_FlushDCache(UINT32 u4Addr, UINT32 u4Size);

#define GCPU_PERF_THR (4096)  //Size Larger than this threshold , use GCPU if more efficient than SW

#endif

static const int kPssSaltLength = 20;

#ifdef USE_OPENSSL
static inline void dump_openssl_error() {
    char buffer[120];
    unsigned long err;
    while ((err = ERR_get_error())) {
        LOGE("openssl error -- %lu -- %s",
                err, ERR_error_string(err, buffer));
    }
}
#endif    

#include "wv_cdm_constants.h"

bool RAND_GEN(uint8_t *bytes, size_t size)
{
#ifdef USE_OPENSSL
    return RAND_bytes(bytes, 4);
#else
    return rng_get_bytes(bytes, size, NULL);
#endif
}

bool CMAC_AES( const uint8_t *key, size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out, size_t *outlen)
{
#ifdef USE_OPENSSL
    bool ret= true;
    const EVP_CIPHER* cipher = EVP_aes_128_cbc();
    CMAC_CTX* cmac_ctx = CMAC_CTX_new();
    if (!CMAC_Init(cmac_ctx, key, keylen, cipher, 0)) {
        ret =  false;
        goto final;
    }

    if (!CMAC_Update(cmac_ctx, in, inlen)) {
        ret =  false;
        goto final;
    }

    if (!CMAC_Final(cmac_ctx, out, outlen)) {
        ret =  false;
        goto final;

    }
final:
    CMAC_CTX_free(cmac_ctx);
    return ret;
#else

    unsigned long out_cnt=*outlen;

    if(register_cipher(&aes_desc) ==-1)
    {
        return false;
    }

    if(omac_memory(find_cipher("aes"),key, keylen, in, inlen, out,&out_cnt)==CRYPT_OK){
        *outlen = out_cnt;
        while(out_cnt==0);
        return true;
    }
    LOGE("KEY LEN %d , IN LEN %d idx %d ",keylen , inlen ,find_cipher("rijndael"));
    return false;
#endif
}

bool HMAC_SHA256(const uint8_t *key, size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out, size_t *outlen)
{

#ifdef USE_OPENSSL
    if (HMAC(EVP_sha256(),key, SHA256_DIGEST_LENGTH, in, inlen, out,outlen)) {
        return true;
    }
    return false;
#else
    unsigned long out_cnt=*outlen;
    if(register_hash(&sha256_desc)==-1)
    {
        return false;
    }
    if(hmac_memory(find_hash("sha256"),key, keylen, in, inlen, out,&out_cnt)==CRYPT_OK){
        *outlen = out_cnt;
        return true;
    }
    return false;
#endif
}

bool HMAC_SHA1(const uint8_t *key, size_t keylen,
        const uint8_t *in,  size_t inlen,
        uint8_t *out, size_t *outlen)
{

#ifdef USE_OPENSSL
    if (HMAC(EVP_sha256(),key, SHA256_DIGEST_LENGTH, in, inlen, out,outlen)) {
        return true;
    }
    return false;
#else
    unsigned long out_cnt=*outlen;
    if(register_hash(&sha1_desc)==-1)
    {
        return false;
    }
    if(hmac_memory(find_hash("sha1"),key, keylen, in, inlen, out,&out_cnt)==CRYPT_OK){
        *outlen = out_cnt;
        return true;
    }
    return false;
#endif
}

bool HASH_SHA256(const uint8_t *in,  size_t inlen,uint8_t *out)
{
#ifdef USE_OPENSSL

#else
    unsigned long out_cnt=SHA256_DIGEST_LENGTH;
    if(register_hash(&sha256_desc)==-1)
    {
        return false;
    }
    if(hash_memory(find_hash("sha256"), in, inlen, out,&out_cnt)==CRYPT_OK){
        return true;
    }
    return false;
#endif
}

bool AES_CBC_ENCRYPT(const uint8_t *key, size_t keylen,
        const uint8_t *iv,
        const uint8_t *in, uint8_t *out, const size_t len)
{
    uint8_t iv_buffer[ KEY_IV_SIZE];
#ifdef USE_OPENSSL
    AES_KEY aes_key;
    if(AES_set_encrypt_key(key, keylen*8, &aes_key)!=0)
    {
        return false;
    }
    memcpy(iv_buffer, iv, KEY_IV_SIZE);
    AES_cbc_encrypt(in, out, len, &aes_key, iv_buffer, AES_ENCRYPT);
#else
    symmetric_CBC cbc;

    if(register_cipher(&aes_desc) ==-1)
    {
        return false;
    }

    memcpy(iv_buffer, iv, KEY_IV_SIZE);
    if (cbc_start( find_cipher("aes"), iv_buffer, key, keylen, 0 , &cbc) != CRYPT_OK) {
        return false;
    }

    if (cbc_encrypt( in, out, len, &cbc) != CRYPT_OK) {
        return false;
    }

    if (cbc_done(&cbc) != CRYPT_OK) {
        return false;
    }
#endif
    return true;
}


bool AES_CBC_DECRYPT_UPDATE_IV(const uint8_t *key, size_t keylen,
        uint8_t * iv,
        const uint8_t *in, uint8_t *out, const size_t len)
{
    symmetric_CBC cbc;
    int cipher_idx = register_cipher(&aes_desc);
    uint8_t iv_buffer[ KEY_IV_SIZE];
    uint8_t buf[AES_BLOCK_SIZE];
    uint8_t tmp[AES_BLOCK_SIZE];
    uint8_t ctsiv[AES_BLOCK_SIZE];
    uint8_t ctsbypes = len % AES_BLOCK_SIZE; // Cipher Text Stealing
    size_t inlen = len;
    size_t decode_len = inlen - ctsbypes;

    if(NULL == iv){
        memcpy(out,in , len);
        return true;
    }

    if(len < AES_BLOCK_SIZE){
        goto fail;
    }

    if(-1 == cipher_idx){
        goto fail;
    }

    memcpy(iv_buffer, iv, KEY_IV_SIZE);

    if(ctsbypes!=0)
    {
        memset(ctsiv , 0 , AES_BLOCK_SIZE);

        if (cbc_start(cipher_idx, ctsiv , key, keylen, 0 , &cbc) != CRYPT_OK){
            goto fail;
        }

        if (cbc_decrypt(in + inlen -AES_BLOCK_SIZE - ctsbypes, tmp, AES_BLOCK_SIZE, &cbc)  != CRYPT_OK){
            goto fail;
        }

        if (cbc_done(&cbc) != CRYPT_OK){
            goto fail;
        }

        memcpy(buf, in + inlen  -AES_BLOCK_SIZE -  ctsbypes , AES_BLOCK_SIZE );
        memcpy(in + inlen - AES_BLOCK_SIZE -ctsbypes, in + inlen -ctsbypes ,  ctsbypes);
        memcpy(in + inlen - AES_BLOCK_SIZE , tmp + ctsbypes , AES_BLOCK_SIZE - ctsbypes);
    }        

#if defined(USE_GCPU_DECRYPT)

    if(decode_len >= GCPU_PERF_THR)
    {
        if (!TZ_GCPU_AES_CBC(key, keylen, iv_buffer, iv_buffer, in, out, decode_len, FALSE))
        {
            return false;
        }

        decode_len = 0;
    }

#endif

    if (cbc_start(cipher_idx, iv_buffer , key, keylen, 0 , &cbc) != CRYPT_OK){
        goto fail;
    }

    if (cbc_decrypt(in  , out,  decode_len, &cbc)  != CRYPT_OK){
        goto fail;
    }

    if(ctsbypes!=0)
    {
        if (cbc_decrypt(buf  , tmp,  AES_BLOCK_SIZE, &cbc)  != CRYPT_OK){
            goto fail;
        }

        memcpy(out + inlen - ctsbypes, tmp , ctsbypes);
    }

    if (cbc_done(&cbc) != CRYPT_OK){
        goto fail;
    }

    memcpy(iv , iv_buffer, KEY_IV_SIZE);
    return true;

fail:    

    return false;

}


bool AES_CBC_DECRYPT(const uint8_t *key, size_t keylen,
        const uint8_t * iv,
        const uint8_t *in, uint8_t *out, const size_t len)
{
    uint8_t iv_clone[AES_BLOCK_SIZE];
    memcpy(iv_clone , iv , AES_BLOCK_SIZE);
    return AES_CBC_DECRYPT_UPDATE_IV(key, keylen,iv_clone, in, out, len);
}

// Increment counter for AES-CTR

//CENC Standard only increase the lower 64 bit
static bool ctr128_inc64(uint8_t* counter) {
    uint32_t n = 16;
     do {
         if (++counter[--n] != 0) 
            return false;
     } while (n>8);
     return true;
}

static bool ctr128_will_overflow(uint8_t* counter, size_t decode_len) {
    uint8_t cnt[16];
    uint32_t n;
    uint32_t i;
    decode_len = (decode_len+15)/ 16 ; //Number of Cnt to be increased

    memcpy(cnt, counter , 16);

    for(i=0; i<decode_len; ++i)
    {
        if(ctr128_inc64(cnt)) 
            return true;
    }
    return false;
}

bool AES_CTR_DECRYPT(const uint8_t *key, size_t keylen, const uint8_t *iv,
        const uint8_t *in, uint8_t *out, const size_t len,
        const unsigned long block_offset)
{
#if defined(USE_OPENSSL)
    AES_KEY aes_key;
    uint8_t aes_iv[AES_BLOCK_SIZE ];
    uint8_t ecount_buf[AES_BLOCK_SIZE];
    unsigned int block_offset_cur = block_offset;

    if(AES_set_encrypt_key(key, keylen*8, &aes_key)!=0)
        return false;

    memcpy(aes_iv, iv, AES_BLOCK_SIZE);

    // Encrypt the IV.
    if (block_offset != 0) {
        // The context is needed only when not starting a new block.
        AES_encrypt(aes_iv, ecount_buf, &aes_key);
        ctr128_inc64(aes_iv);
    }

    AES_ctr128_encrypt( in, (uint8_t*)out, len, &aes_key, aes_iv, ecount_buf, &block_offset_cur);

    if (block_offset_cur != ((block_offset + len) % AES_BLOCK_SIZE)) {
        LOGE("[DecryptCTR(): FAILURE: byte offset wrong.]");
        return false;
    }

    return true;

#else
    symmetric_key skey;
    symmetric_CTR ctr;
    uint8_t iv_buffer[KEY_IV_SIZE];
    size_t decode_len = len;
    uint32_t l=0;
    uint32_t n=block_offset;
    const uint8_t *src_addr = in;
    uint8_t *out_addr = out;
    bool isWrapped = false;
    
    if(register_cipher(&aes_desc) ==-1)
    {
        return false;
    }

    memcpy(iv_buffer, iv, KEY_IV_SIZE);

    if(block_offset !=0)
    {
        if (aes_setup(key, keylen, 0, &skey) != CRYPT_OK) {
            return false;
        }
        aes_ecb_encrypt(iv, iv_buffer, &skey);
        while (l<len) {
            out[l] = in[l] ^ iv_buffer[n];
            ++l;
            ++n ;
            if(n == 16)
                break;
        }
        TZ_SWD_FlushDCache(out_addr, l );
        
        memcpy(iv_buffer, iv, KEY_IV_SIZE);
        ctr128_inc64(iv_buffer);
        decode_len -= l;
        src_addr += l;
        out_addr += l;
    }

    isWrapped = ctr128_will_overflow(iv_buffer , decode_len);

#if defined(USE_GCPU_DECRYPT)
    //GCPU Can Not support CENC 64 bit standard
    if((decode_len >= GCPU_PERF_THR) && !isWrapped)
    {
        if (S_GCPU_OK != GCPU_SetSecureSlot(AES_SVP_CTR_KEY_PTR, key, keylen))
        {
            return false;
        }

        if( S_GCPU_OK != TZ_SVP_AES_CTR(src_addr, out_addr , decode_len , iv_buffer, iv_buffer))
        {
            return false;
        }

    }else 
#endif    
    if(decode_len > 0 )
    {
        if(ctr_start( find_cipher("aes"), iv_buffer, key, keylen, 0
                    , CTR_COUNTER_BIG_ENDIAN | 8, &ctr) != CRYPT_OK) {
            return false;
        }
        if(ctr_decrypt( src_addr, out_addr, decode_len, &ctr)!= CRYPT_OK) {
            return false;
        }

        if(ctr_done(&ctr)!= CRYPT_OK) {
            return false;
        }
    }
#endif
    return true;
}

void RSA_KEY_TERM(RSA **key)
{
#ifndef USE_OPENSSL
    if(*key !=NULL)
    {
        rsa_free(*key);
        free(*key);
    }
    *key = NULL;
#endif    
}

static inline uint32_t BER_LENTH(const uint8_t *byte,uint32_t *unit)
{
    uint32_t len,i;

    *unit =1;
    len = *byte&0x7F;

    if( (*byte++ & 0x80) != 0)
    {
        *unit += len;
        if(len > 4)
        {
            return false;
        }
        for(i=0,len=0;i< *unit-1;++i)
        {
            len=len<<8;
            len|=*byte++;
        }
    }
    return len;
}

static inline bool PKCS8_TO_PKCS1(const uint8_t * pkcs8_key , const size_t pkcs8_len ,uint8_t *pkcs1_key ,size_t *pkcs1_len)
{
    const uint8_t *cur = pkcs8_key;
    uint32_t len, unit;

    //SEQ TOKEN
    if( BER_TAG(*cur++) != BER_SEQUENCE)
    {
        LOGE("PKCS8 INIT TOKEN FAIL");
        return false;
    }

    len = BER_LENTH(cur,&unit);
    cur +=unit;

    if( cur+len != pkcs8_key + pkcs8_len)
    {
        LOGE("PKCS8 SIZE Incorrect %x ,%x, %x",cur+len,len, pkcs8_key + pkcs8_len);
        return false;
    }

    //INT TOKEN
    if(BER_TAG(*cur++) != BER_INTERGER)
    {
        LOGE("PKCS8 VERSION TOKEN FAIL");
        return false;
    }

    len = BER_LENTH(cur,&unit);
    cur +=unit+len;

    //Algorithm Tag Section
    if(BER_TAG(*cur++) != BER_SEQUENCE)
    {
        LOGE("PKCS8 ALGO TOKEN FAIL");
        return false;
    }

    len = BER_LENTH(cur,&unit);
    cur +=unit+len;
    
    //PRIVATE KEY
    if(BER_TAG(*cur++) != BER_OCT_STR)
    {
        LOGE("PKCS8 PRIVATE_KEY TOKEN FAIL");
        return false;
    }
    len = BER_LENTH(cur,&unit);
    cur +=unit;

    if( len < *pkcs1_len)
    {
        memcpy(pkcs1_key , cur , len);
        *pkcs1_len = len;
        LOGE("PKCS8 PARSE O.K , have length %x",len);
        return true;
    }
    else
    {
        *pkcs1_len = 0;
        LOGE("PKCS8 N.G , key length %x too large",len);
        return false;
    }
}

bool RSA_LOAD_PKCS8_KEY(RSA **key,  uint8_t* pkcs8_rsa_key, size_t rsa_keylength) {
#ifdef USE_OPENSSL
    BIO *bio;
    bool success = false;
    EVP_PKEY *evp = NULL;
    PKCS8_PRIV_KEY_INFO *pkcs8_pki ;


    if (*key) {
        RSA_free(*key);
        *key = NULL;
    }

    bio = BIO_new_mem_buf(pkcs8_rsa_key, rsa_keylength);
    if( bio == NULL ) {
        LOGE("[LoadRSAKey(): Could not allocate bio buffer]");
        return false;
    }

    success = true;
    pkcs8_pki = d2i_PKCS8_PRIV_KEY_INFO_bio(bio, NULL);

    if (pkcs8_pki == NULL) {
        LOGE("d2i_PKCS8_PRIV_KEY_INFO_bio returned NULL.");
        success = false;
    }
    if (success) {
        evp = EVP_PKCS82PKEY(pkcs8_pki);
        if (evp == NULL) {
            LOGE("EVP_PKCS82PKEY returned NULL.");
            success = false;
        }
    }
    if (success) {
        *key = EVP_PKEY_get1_RSA(evp);
        if (*key == NULL) {
            LOGE("PrivateKeyInfo did not contain an RSA key.");
            success = false;
        }
    }
    if (evp != NULL) {
        EVP_PKEY_free(evp);
    }
    if (pkcs8_pki != NULL) {
        PKCS8_PRIV_KEY_INFO_free(pkcs8_pki);
    }
    BIO_free(bio);
    if (!success) {
        return false;
    }
    switch (RSA_check_key(*key)) {
        case 1: // valid.
            return true;
        case 0:  // not valid.
            LOGE("[LoadRSAKey(): rsa key not valid]");
            return false;
        default:  // -1 == check failed.
            LOGE("[LoadRSAKey(): error checking rsa key]");
            return false;
    }
#else

    size_t keylen=PKCS1_RSA_LENGTH;
    //uint8_t pkcs1_rsa_key[PKCS1_RSA_LENGTH];
    bool ret=false;
    uint8_t *pkcs1_rsa_key;
    pkcs1_rsa_key = malloc(PKCS1_RSA_LENGTH);
    if(pkcs1_rsa_key ==NULL){
        goto final;
    }

    if(!PKCS8_TO_PKCS1(pkcs8_rsa_key , rsa_keylength , pkcs1_rsa_key ,&keylen))
    {
        goto final;
    }

    if(*key !=NULL)
    {
        rsa_free(*key);
        free(*key);
    }
    *key = malloc(sizeof(rsa_key));
    memset(*key,0,sizeof(rsa_key));

    if(rsa_import(pkcs1_rsa_key, keylen ,*key)!=CRYPT_OK)
    {
        goto final;        
    }

    ret = true;
final:
    free(pkcs1_rsa_key);
    
    return ret;
#endif
}

size_t RSA_GET_SIZE(RSA *key)
{
#ifdef USE_OPENSSL
    //LOGE("===========================================");
    //LOGE("RSA KEY SIZE 0x%x", RSA_size(key));
    return RSA_size(key);
#else
    //TODO 
    //Should be number of bit of key->N/8
    //In widevine , spec is fix for 2048/8
    return 2048/8;
    //return mp_unsigned_bin_size((key->N));
#endif
}

bool RSA_DECRYPT(RSA *key , const uint8_t * in, const size_t inlen,
        uint8_t *out , size_t *outlen){

#ifdef USE_OPENSSL
    int decrypted_size;

    decrypted_size = RSA_private_decrypt(inlen, in, out, key, RSA_PKCS1_OAEP_PADDING);

    if (-1 == decrypted_size) {
        LOGE("[RSADeriveKeys(): error decrypting session key.]");
        dump_openssl_error();
        *outlen = 0;
        return false;
    }

    *outlen = (size_t)decrypted_size;
    return true;
#else
    int res;
    int ret;
    unsigned long out_size =*outlen;
    int sha1idx =register_hash(&sha1_desc);

    if(sha1idx== -1) {
        LOGD("[RSA_DECRYPT] sha1 hash register fail");
        return false;
    }

    if ((ret=rsa_decrypt_key(in, inlen, out, &out_size, NULL, 0, sha1idx, &res,key))!=CRYPT_OK)
    {
        LOGD("[RSA_DECRYPT] FAIL :%d %d %d",ret, out_size , *outlen);
        return false;
    }
    
    *outlen = out_size; 
    LOGD("[RSA_DECRYPT] PASS");
    return true;

#endif
}

bool RSA_SIGN(RSA *key , const uint8_t * message, const size_t message_length,
        uint8_t *signature , size_t *signature_length ,int padding){
#ifdef USE_OPENSSL
    bool ret = true;
    int status;
    uint8_t hash[SHA_DIGEST_LENGTH];

    Vector padded_digest;
    VecInit(&padded_digest);

    if (!SHA1(message, message_length, hash)) {
        LOGE("[GeneratRSASignature(): error creating signature hash.]");
        dump_openssl_error();
        ret = false;
        goto final;
    }

    VecResize(&padded_digest, *signature_length);

    if (RSA_padding_add_PKCS1_PSS(key, padded_digest.buf, hash,
            EVP_sha1(), kPssSaltLength)== -1) {
        LOGE("[GeneratRSASignature(): error padding hash.]");
        dump_openssl_error();
        ret = false;
        goto final;
    }

    if (RSA_private_encrypt(*signature_length, padded_digest.buf, signature,
            key, RSA_NO_PADDING)== -1) {
        LOGE("[GeneratRSASignature(): error in private encrypt.]");
        dump_openssl_error();
        ret = false;
        goto final;
    }

final :

    VecTerm(&padded_digest);
    LOGD("[RSA_SIGN]RSA Sign O.K 0x%x", *signature_length);
    return ret;

#else
    int prngidx =register_prng(&sprng_desc) ;
    int sha1idx =register_hash(&sha1_desc);
    uint8_t hashed[SHA1_DIGEST_LENGTH];
    unsigned long hlen=SHA1_DIGEST_LENGTH;
    unsigned long sig_len =*signature_length;
    int state;

    uint8_t PKCA1_MSG[2048/8];
    unsigned long modulus_len;
    int i;

    if(padding==0)
    {
        if(prngidx== -1) {
            LOGD("[RSA_SIGN]prng spring register fail");
            return false;
        }

        if(sha1idx== -1) {
            LOGD("[RSA_SIGN]hash sha1register fail");
            return false;
        }

        if(hash_memory(sha1idx, message, message_length ,hashed, &hlen)!=CRYPT_OK)
        {
            LOGD("[RSA_SIGN]HASH Fail");
            return false;
        }

        //if(rsa_sign_hash(message, message_length, signature, &sig_len ,
        if(rsa_sign_hash(hashed, hlen, signature, &sig_len ,
                    NULL, prngidx, sha1idx, kPssSaltLength, key)!=CRYPT_OK)
        {
            return false;
        }
        
        *signature_length = sig_len;
        return true;
    }
    else
    {
        modulus_len =   (mp_count_bits(key->N) >> 3) + (mp_count_bits(key->N) & 7 ? 1 : 0);
    
        PKCA1_MSG[0]=0;
        PKCA1_MSG[1]=1;
        for(i=0;i< modulus_len - 3 -message_length ; ++i)
        {
            PKCA1_MSG[i+2]=0xFF;
        }
        PKCA1_MSG[i+2]=0;
        memcpy(PKCA1_MSG + i+3, message, message_length);
            
        //if(rsa_sign_hash_ex(message, message_length, signature, &sig_len, LTC_PKCS_1_V1_5, NULL,0,0,0,key)!=CRYPT_OK)
        //if(rsa_exptmod(message , message_length , signature, &sig_len ,PK_PRIVATE ,key)!=CRYPT_OK)                
        if(rsa_exptmod(PKCA1_MSG , modulus_len, signature, &sig_len ,PK_PRIVATE ,key)!=CRYPT_OK)                            
        {
            return false;
        }
        *signature_length = sig_len;
        return true;       
    }
#endif

}
