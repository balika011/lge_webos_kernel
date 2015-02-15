/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "mem_api.h"
#include "binstr_api.h"
/* sec/common */
#include "crypto_api.h"
#include "rsa_api.h"
#include "hmac_api.h"
#include "sha1_api.h"
#include "sha256_api.h"
#include "pkcs8_api.h"
#include "rand_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_crypto.h"
#include "hci_types.h"
#include "hci_internal_api.h"


/* utilities ***************************************************************/

static uint32_t
l_InitAes(uint32_t           in_crypto_type,
          hci_enc_pad_t      in_enc_pad,
          uint8_t           *in_key,
          uint8_t           *in_modedata,
          crypto_handle_t   *out_handle)
{
    uint32_t          ret             = ERR_HCI_OK;
    uint32_t          crypto_type     = 0;

    MSGENT(("l_InitAes"));

    crypto_type = in_crypto_type;
    if (in_enc_pad == HCI_ENC_PAD_RFC2630 || in_enc_pad == HCI_ENC_PAD_XMLENC) {
        crypto_type |= CRYPTO_PAD_PKCS5;
    } else if (in_enc_pad == HCI_ENC_PAD_NONE) {
        crypto_type |= CRYPTO_PAD_NONE;
    } else {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RETR(crypto_Init(out_handle,
                     crypto_type,
                     in_modedata,
                     HCI_L_AES_BLOCKSIZE,
                     in_key,
                     HCI_L_AES_KEYSIZE),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

EXIT:
    MSGEXT(("l_InitAes"));
    return ret;
}

static void
l_FinAes(crypto_handle_t   in_handle)
{
    MSGENT(("l_FinAes"));
    crypto_Fin(in_handle);
    MSGEXT_V(("l_FinAes"));
}

#if 1 == HCI_L_CRYPTO_DECRYPTAES128RTB
static uint32_t
l_cryptoEncryptAES128ECB(uint8_t *in_key,
                         uint8_t *in_plain,
                         uint8_t *out_cipher)
{
    uint32_t ret;
    crypto_handle_t handle = NULL;
    uint32_t  len = HCI_L_AES_BLOCKSIZE;

    MSGENT(("l_cryptoEncryptAES128ECB"));

    if ( NULL == in_key        ||
         NULL == in_plain      ||
         NULL == out_cipher)   {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RETR( crypto_Init(&handle,
                      CRYPTO_TYPE_AES|CRYPTO_DIR_ENC|CRYPTO_MODE_ECB|CRYPTO_PAD_NONE,
                      NULL, 0, in_key, HCI_L_AES_BLOCKSIZE),
          ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);
    RETR( crypto_Encrypt(handle, TRUE, in_plain, HCI_L_AES_BLOCKSIZE, out_cipher, &len),
          ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);
EXIT:
    crypto_Fin(handle);
    MSGEXT(("l_cryptoEncryptAES128ECB"));
    return ret;
}
#endif

static uint32_t
l_InitRsa(binstr_t            *in_pkcs8,
          rsa_handle_t        *out_rsa)
{
    uint32_t            ret         = ERR_HCI_OK;
    pkcs8_key_t        *pkcs8_key   = NULL;

    MSGENT(("l_InitRsa"));

    RETR(pkcs8_GetKey(in_pkcs8, &pkcs8_key),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_ILLEGALPERSONALITY);

    RETR(rsa_Init(out_rsa, &pkcs8_key->n, &pkcs8_key->e, &pkcs8_key->d, &pkcs8_key->p,
                  &pkcs8_key->q, &pkcs8_key->dp, &pkcs8_key->dq, &pkcs8_key->qinv),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

EXIT:
    if ( pkcs8_key ) {
        pkcs8_FreeKey(pkcs8_key);
    }
    MSGEXT(("l_InitRsa"));
    return ret;
}

static void
l_FinRsa(rsa_handle_t        in_rsa)
{
    MSGENT(("l_FinRsa"));
    rsa_Fin(in_rsa);
    MSGEXT_V(("l_FinRsa"));
}

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET
static void
l_CalculateCounterStartValue( np_size_t  in_offset,
                              uint8_t   *in_original_counter,
                              uint8_t    out_counter[HCI_L_AES_BLOCKSIZE] )
{
    uint32_t higher     = 0;
    uint32_t lower      = 0;
    uint32_t lowest     = 0;
    uint32_t i          = 0;
    uint32_t counterlen = HCI_L_AES_BLOCKSIZE;
    uint32_t increment  = in_offset / HCI_L_AES_BLOCKSIZE;

    MSGENT(("l_CalculateCounterStartValue"));

    mem_Copy( out_counter, in_original_counter, HCI_L_AES_BLOCKSIZE );

    if ( increment > 0 ) {
        lowest = (out_counter[counterlen - 4] << 24) |
                 (out_counter[counterlen - 3] << 16) |
                 (out_counter[counterlen - 2] << 8)  |
                 (out_counter[counterlen - 1] );

        lower = lowest;
        for ( i = 0; i < (HCI_L_AES_BLOCKSIZE / sizeof(uint32_t)) - 1; ++i ) {
            if ( lower + increment > lower ) {
                break;
            }

            /* If higher = 0xFFFFFFFF then carry is needed ... */
            higher = (out_counter[counterlen - 8 - (i * sizeof(uint32_t))] << 24) |
                     (out_counter[counterlen - 7 - (i * sizeof(uint32_t))] << 16) |
                     (out_counter[counterlen - 6 - (i * sizeof(uint32_t))] << 8)  |
                     (out_counter[counterlen - 5 - (i * sizeof(uint32_t))] );

            lower = higher;
            increment = 1;
            higher += increment;

            out_counter[counterlen - 8 - (i * sizeof(uint32_t))] = (higher >> 24) & 0xFF;
            out_counter[counterlen - 7 - (i * sizeof(uint32_t))] = (higher >> 16) & 0xFF;
            out_counter[counterlen - 6 - (i * sizeof(uint32_t))] = (higher >> 8 ) & 0xFF;
            out_counter[counterlen - 5 - (i * sizeof(uint32_t))] = (higher      ) & 0xFF;
        }

        lowest += in_offset / HCI_L_AES_BLOCKSIZE;
        out_counter[counterlen - 4] = (lowest >> 24) & 0xFF;
        out_counter[counterlen - 3] = (lowest >> 16) & 0xFF;
        out_counter[counterlen - 2] = (lowest >> 8 ) & 0xFF;
        out_counter[counterlen - 1] = (lowest      ) & 0xFF;
    }

    MSGEXT_V(("l_CalculateCounterStartValue"));
}

static uint32_t
l_CounterModeDecrypt( uint8_t       *in_key,
                      uint32_t       in_offset,
                      uint32_t       in_cipher_len,
                      uint8_t       *in_cipher,
                      uint8_t       *in_ctr,
                      uint8_t       *out_plain )
{
    uint32_t        ret         = ERR_HCI_OK;
    crypto_handle_t handle      = NULL;
    uint32_t        len         = HCI_L_AES_BLOCKSIZE;
    uint32_t        byte_offset = in_offset % HCI_L_AES_BLOCKSIZE;
    uint8_t         counter[HCI_L_AES_BLOCKSIZE];
    uint8_t         firstblock[HCI_L_AES_BLOCKSIZE];

    MSGENT(("l_CounterModeDecrypt"));

    l_CalculateCounterStartValue(in_offset, in_ctr, counter);

    RET( l_InitAes(CRYPTO_TYPE_AES|CRYPTO_DIR_CTR|CRYPTO_MODE_CTR,
                   HCI_ENC_PAD_NONE, in_key, counter, &handle) );

    /* Decrypt the first block separately to handle byte offsets */
    mem_Set( firstblock, 0x00, sizeof firstblock );
    mem_Copy( firstblock + byte_offset, in_cipher, HCI_L_AES_BLOCKSIZE - byte_offset );
    RETR( crypto_Decrypt(handle,
                         TRUE,
                         firstblock,
                         sizeof firstblock,
                         firstblock,
                         &len),
          (ret == ERR_NOMEM ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB) );

    if ( sizeof firstblock != len ) {
        RET( ERR_HCI_CRYPTOLIB );
    }

    /* Decrypt the remainder as it is aligned correctly along the blocksize */
    len = ( HCI_L_AES_BLOCKSIZE - byte_offset < in_cipher_len ? HCI_L_AES_BLOCKSIZE - byte_offset : in_cipher_len);
    mem_Copy( out_plain, firstblock + byte_offset, len );

    if ( in_cipher_len > HCI_L_AES_BLOCKSIZE - byte_offset ) {
        len = in_cipher_len - (HCI_L_AES_BLOCKSIZE - byte_offset);
        RETR( crypto_Decrypt(handle,
                             TRUE,
                             in_cipher + (HCI_L_AES_BLOCKSIZE - byte_offset),
                             len,
                             out_plain + (HCI_L_AES_BLOCKSIZE - byte_offset),
                             &len),
              (ret == ERR_NOMEM ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB) );
    }

EXIT:
    crypto_Fin(handle);
    MSGEXT(("l_CounterModeDecrypt"));
    return ret;
}
#endif

/* public functions ******************************************************/

uint32_t
hci_l_cryptoGenerateRandom(uint32_t  in_num_bytes,
                           uint8_t  *out_buf )
{
    uint32_t    ret     = ERR_HCI_OK;
    rand_ctx_t  rng     = RAND_NULL;

    MSGENT(("hci_l_cryptoGenerateRandom"));

    if ( (in_num_bytes     == 0)     ||
         (out_buf          == NULL)) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RETR(rand_Init(&rng),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);
    RETR(rand_GetBytes(rng, in_num_bytes, out_buf),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

EXIT:
    rand_Fin(rng);
    MSGEXT(("hci_l_cryptoGenerateRandom"));
    return ret;
}

uint32_t
hci_l_cryptoMakeDigest(hci_dig_algo_t  in_digest_algo,
                       uint32_t        in_data_len,
                       uint8_t        *in_data,
                       uint32_t       *io_digest_len,
                       uint8_t        *out_digest)
{
    uint32_t          ret       = ERR_HCI_OK;
    int i;
    
    MSGENT(("hci_l_cryptoMakeDigest"));

    if ( (in_data_len       == 0)     ||
         (in_data           == NULL)  ||
         (io_digest_len     == NULL)  ||
         (out_digest        == NULL)) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_digest_algo) {
    case HCI_DIG_ALGO_SHA1:
        if (*io_digest_len < HCI_L_SHA1_DIGESTSIZE) {
            RET(ERR_HCI_ILLEGALARGS);
        }
        sha1_MakeDigest(in_data, in_data_len, out_digest);
        *io_digest_len = HCI_L_SHA1_DIGESTSIZE;
        break;
    case HCI_DIG_ALGO_SHA256:
        if (*io_digest_len < HCI_L_SHA256_DIGESTSIZE) {
            RET(ERR_HCI_ILLEGALARGS);
        }
        sha256_MakeDigest(in_data, in_data_len, out_digest);
        *io_digest_len = HCI_L_SHA256_DIGESTSIZE;
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
        break;
    }

EXIT:
    MSGEXT(("hci_l_cryptoMakeDigest"));
    return ret;
}

uint32_t
hci_l_cryptoCalculateHmac(hci_dig_algo_t  in_digest_algo,
                          uint32_t        in_key_len,
                          uint8_t        *in_key,
                          uint32_t        in_data_len,
                          uint8_t        *in_data,
                          uint32_t       *io_hmac_len,
                          uint8_t        *out_hmac)
{
    uint32_t          ret       = ERR_HCI_OK;
    binstr_t          key       = { in_key_len, in_key };
    binstr_t          data      = { in_data_len, in_data };
    binstr_t         *hmac      = NULL;
    int32_t           hmac_algo = 0;

    MSGENT(("hci_l_cryptoCalculateHmac"));

    if ( (0 == binstr_Check(&key))                 ||
         ((0 == in_data_len) && (NULL != in_data)) ||
         ((0 != in_data_len) && (NULL == in_data)) ||
         (NULL == io_hmac_len)                     ||
         (NULL == out_hmac) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_digest_algo) {
    case HCI_DIG_ALGO_SHA1:
        hmac_algo = HMAC_ALGO_SHA1;
        if (*io_hmac_len < HCI_L_SHA1_DIGESTSIZE) {
            RET(ERR_HCI_ILLEGALARGS);
        }
        break;
    case HCI_DIG_ALGO_SHA256:
        hmac_algo = HMAC_ALGO_SHA256;
        if (*io_hmac_len < HCI_L_SHA256_DIGESTSIZE) {
            RET(ERR_HCI_ILLEGALARGS);
        }
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
        break;
    }

    RETR(hmac_Calculate(hmac_algo, &key, &data, &hmac),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    mem_Copy(out_hmac, binstr_Buf(hmac), binstr_Len(hmac));
    *io_hmac_len = binstr_Len(hmac);

EXIT:
    hci_l_mem_binstrFree(hmac);
    MSGEXT(("hci_l_cryptoCalculateHmac"));
    return ret;

}

/* The higher than normal complexity comes from the parameter checking, not the complexity of the body */
uint32_t
hci_l_cryptoEncryptAES128CBC(hci_enc_pad_t      in_enc_pad,
                             uint8_t           *in_key,
                             uint8_t           *io_iv,
                             uint32_t           in_plain_len,
                             uint8_t           *in_plain,
                             uint32_t          *io_cipher_len,
                             uint8_t           *out_cipher)
{
    uint32_t          ret             = ERR_HCI_OK;
    crypto_handle_t   handle          = NULL;
    hci_bool_t        is_final        = HCI_FALSE;

    MSGENT(("hci_l_cryptoEncryptAES128CBC"));

    if ( ((HCI_ENC_PAD_NONE == in_enc_pad) &&
          (0 != in_plain_len % HCI_L_AES_BLOCKSIZE) ) ||
         /* Other values of in_enc_pad are tested in l_InitAes() */
         NULL == in_key        ||
         NULL == io_iv         ||
         0    == in_plain_len  ||
         NULL == in_plain      ||
         NULL == io_cipher_len ||
         NULL == out_cipher) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( ((HCI_ENC_PAD_NONE == in_enc_pad) && (*io_cipher_len < in_plain_len)) ||
         ((HCI_ENC_PAD_NONE != in_enc_pad) && (*io_cipher_len < in_plain_len + HCI_L_AES_BLOCKSIZE)) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    RET(l_InitAes(CRYPTO_TYPE_AES|CRYPTO_DIR_ENC|CRYPTO_MODE_CBC,
                  in_enc_pad, in_key, io_iv, &handle));

    if (in_enc_pad == HCI_ENC_PAD_NONE) {
        is_final = HCI_FALSE;
    } else {
        is_final = HCI_TRUE;
    }

    RETR(crypto_Encrypt(handle, is_final, in_plain, in_plain_len, out_cipher, io_cipher_len),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if (HCI_ENC_PAD_NONE == in_enc_pad) {
        /* copy last encrypted block to io_iv */
        mem_Copy( io_iv, &out_cipher[*io_cipher_len - HCI_L_AES_BLOCKSIZE], HCI_L_AES_BLOCKSIZE );
    }

EXIT:
    l_FinAes(handle);
    MSGEXT(("hci_l_cryptoEncryptAES128CBC"));
    return ret;
}

/* The higher than normal compexity comes from the paramter checking, not the complexity of the body */
uint32_t
hci_l_cryptoDecryptAES128CBC(hci_enc_pad_t      in_enc_pad,
                             uint8_t           *in_key,
                             uint8_t           *io_iv,
                             uint32_t           in_cipher_len,
                             uint8_t           *in_cipher,
                             uint32_t          *io_plain_len,
                             uint8_t           *out_plain)
{
    uint32_t          ret             = ERR_HCI_OK;
    crypto_handle_t   handle          = NULL;
    hci_bool_t        is_final        = HCI_FALSE;
    uint8_t           iv[HCI_L_AES_BLOCKSIZE];

    MSGENT(("hci_l_cryptoDecryptAES128CBC"));

    if (    /* values of in_enc_pad are tested in l_InitAes() */
        NULL == in_key        ||
        NULL == io_iv         ||
        0 == in_cipher_len    ||
        0 != in_cipher_len % HCI_L_AES_BLOCKSIZE ||
        NULL == in_cipher     ||
        NULL == io_plain_len  ||
        NULL == out_plain)   {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( ((HCI_ENC_PAD_NONE == in_enc_pad) && (*io_plain_len < in_cipher_len)) ||
         ((HCI_ENC_PAD_NONE != in_enc_pad) && (*io_plain_len < in_cipher_len - HCI_L_AES_BLOCKSIZE)) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    RET(l_InitAes(CRYPTO_TYPE_AES|CRYPTO_DIR_DEC|CRYPTO_MODE_CBC,
                  in_enc_pad, in_key, io_iv, &handle));

    if (in_enc_pad == HCI_ENC_PAD_NONE) {
        is_final = HCI_FALSE;
    } else {
        is_final = HCI_TRUE;
    }

    /* back-up last encrypted block in case it gets overwritten (during in-place decryption) */
    mem_Copy( iv, &in_cipher[in_cipher_len - HCI_L_AES_BLOCKSIZE], HCI_L_AES_BLOCKSIZE );

    RETR(crypto_Decrypt(handle, is_final, in_cipher, in_cipher_len, out_plain, io_plain_len),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if (HCI_ENC_PAD_NONE == in_enc_pad) {
        /* copy last encrypted block to io_iv */
        mem_Copy( io_iv, iv, HCI_L_AES_BLOCKSIZE );
    }

EXIT:
    l_FinAes(handle);
    MSGEXT(("hci_l_cryptoDecryptAES128CBC"));
    return ret;
}

uint32_t
hci_l_cryptoEncryptAES128CTR(uint8_t           *in_key,
                             uint8_t           *in_iv,
                             hci_bool_t         in_is_final,
                             uint32_t           in_plain_len,
                             uint8_t           *in_plain,
                             uint32_t          *io_cipher_len,
                             uint8_t           *out_cipher,
                             uint8_t           *out_counter)
{
    return ERR_HCI_INTERNALERROR;
/*
    uint32_t          ret             = ERR_HCI_OK;
    crypto_handle_t   handle          = NULL;

    MSGENT(("hci_l_cryptoEncryptAES128CTR"));

    if ( NULL == in_key        ||
         NULL == in_iv         ||
         0    == in_plain_len  ||
         NULL == in_plain      ||
         NULL == io_cipher_len ||
         NULL == out_cipher) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( *io_cipher_len < in_plain_len ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    RET(l_InitAes(CRYPTO_TYPE_AES|CRYPTO_DIR_ENC|CRYPTO_MODE_CTR,
                  HCI_ENC_PAD_NONE, in_key, in_iv, &handle));

    RETR(crypto_Encrypt(handle, in_is_final, in_plain, in_plain_len, out_cipher, io_cipher_len),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if ((HCI_FALSE == in_is_final) && (NULL != out_counter)) {
        RET( crypto_GetAes128Counter( handle, HCI_L_AES_BLOCKSIZE, out_counter ) );
    }

EXIT:
    l_FinAes(handle);
    MSGEXT(("hci_l_cryptoEncryptAES128CTR"));
    return ret;
*/
}

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET
uint32_t
hci_l_cryptoDecryptAES128CTR(uint8_t          *in_key,
                             uint32_t          in_offset,
                             uint32_t          in_ctrsize,
                             uint8_t          *in_ctr,
                             uint32_t          in_cipher_len,
                             uint8_t          *in_cipher,
                             uint32_t         *io_plain_len,
                             uint8_t          *out_plain)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_l_cryptoDecryptAES128CTR"));

    if ( NULL == in_key          ||
         NULL == in_ctr          ||
         NULL == in_cipher       ||
         NULL == io_plain_len    ||
         NULL == out_plain       ||
         0    == in_cipher_len   ||
         HCI_L_AES_BLOCKSIZE != in_ctrsize  ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( *io_plain_len < in_cipher_len ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    RET( l_CounterModeDecrypt(in_key,
                              in_offset,
                              in_cipher_len,
                              in_cipher,
                              in_ctr,
                              out_plain) );

    *io_plain_len = in_cipher_len;

EXIT:
    MSGEXT(("hci_l_cryptoDecryptAES128CTR"));
    return ret;
}
#endif

uint32_t
hci_l_cryptoDecryptAES128ECB(uint8_t  *in_key,
                             uint32_t  in_cipher_len,
                             uint8_t  *in_cipher,
                             uint32_t *io_plain_len,
                             uint8_t  *out_plain)
{
    uint32_t ret;
    crypto_handle_t handle = NULL;
    uint32_t  len = HCI_L_AES_BLOCKSIZE;

    MSGENT(("hci_l_cryptoDecryptAES128ECB"));

    if ( NULL == in_key                       ||
         NULL == in_cipher                    ||
         HCI_L_AES_BLOCKSIZE != in_cipher_len ||
         NULL == out_plain)   {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RETR( crypto_Init(&handle,
                      CRYPTO_TYPE_AES|CRYPTO_DIR_DEC|CRYPTO_MODE_ECB|CRYPTO_PAD_NONE,
                      NULL, 0, in_key, HCI_L_AES_BLOCKSIZE),
          ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);
    RETR( crypto_Decrypt(handle, TRUE, in_cipher, in_cipher_len, out_plain, &len),
          ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    *io_plain_len = in_cipher_len;
EXIT:
    crypto_Fin(handle);
    MSGEXT(("hci_l_cryptoDecryptAES128ECB"));
    return ret;
}

/* For BBTS only */
uint32_t
hci_l_cryptoDecryptAES128RTB(uint8_t           *in_key,
                             uint8_t           *in_iv,
                             uint32_t           in_cipher_len,
                             uint8_t           *in_cipher,
                             uint32_t          *io_plain_len,
                             uint8_t           *out_plain)
{
    uint32_t            ret         = ERR_HCI_OK;
#if 1 == HCI_L_CRYPTO_DECRYPTAES128RTB
    uint32_t            remainder   = in_cipher_len % HCI_L_AES_BLOCKSIZE;
    uint32_t            blocks_len  = in_cipher_len - remainder;
    uint8_t             residual_block[HCI_L_AES_BLOCKSIZE];
    uint8_t             temp_iv[HCI_L_AES_BLOCKSIZE];
#endif

    MSGENT(("hci_l_cryptoDecryptAES128RTB"));

    if ( NULL == in_key        ||
         NULL == in_iv         ||
         0    == in_cipher_len ||
         NULL == in_cipher     ||
         NULL == io_plain_len  ||
         NULL == out_plain)   {
        RET( ERR_HCI_ILLEGALARGS );
    }

#if 0 == HCI_L_CRYPTO_DECRYPTAES128RTB
    RET(ERR_HCI_NOTSUPPORTED);
#else
    if ( 0 == blocks_len ) {
        mem_Copy(residual_block, in_iv, sizeof residual_block);
    } else {
        /* residual block is last full encrypted block */
        mem_Copy(residual_block, in_cipher + blocks_len - HCI_L_AES_BLOCKSIZE, sizeof residual_block);

        mem_Copy(temp_iv, in_iv, sizeof temp_iv);
        RET(hci_l_cryptoDecryptAES128CBC(HCI_ENC_PAD_NONE, in_key, temp_iv,
                                         blocks_len, in_cipher, io_plain_len, out_plain));
    }

    if ( 0 != remainder ) {
        /*
         * Decrypt the remainder cipher text by xor-ing this remainder with the encrypted:
         *  - last full block, if any
         *  - iv otherwise (solitary termination block).
         */
        uint8_t   double_cipher[HCI_L_AES_BLOCKSIZE];
        uint32_t  i;

        RET(l_cryptoEncryptAES128ECB(in_key, residual_block, double_cipher));

        /* XOR remainder with encrypted block*/
        for ( i=0; i<remainder; i++ ) {
            out_plain[blocks_len+i] = double_cipher[i] ^ in_cipher[blocks_len+i];
        }
    }
    *io_plain_len = in_cipher_len;
#endif

EXIT:
    MSGENT(("hci_l_cryptoDecryptAES128RTB"));
    return ret;
}

uint32_t
hci_l_cryptoEncryptRsaesPkcs1V1_5(uint32_t             in_n_len,
                                  uint8_t             *in_n,
                                  uint32_t             in_e_len,
                                  uint8_t             *in_e,
                                  uint32_t             in_plain_len,
                                  uint8_t             *in_plain,
                                  uint32_t            *io_cipher_len,
                                  uint8_t             *out_cipher)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t            n           = { in_n_len, in_n };
    binstr_t            e           = { in_e_len, in_e };
    binstr_t            plain       = { in_plain_len, in_plain };
    rsa_handle_t        hrsa        = NULL;
    binstr_t           *cipher      = NULL;

    MSGENT(("hci_l_cryptoEncryptRsaesPkcs1V1_5"));

    if ( 0    == binstr_Check(&n)       ||
         0    == binstr_Check(&e)       ||
         0    == binstr_Check(&plain)   ||
         NULL == io_cipher_len         ||
         0    == *io_cipher_len        ||
         NULL == out_cipher) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RETR(rsa_Init(&hrsa, &n, &e, NULL, NULL, NULL, NULL, NULL, NULL),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    RETR(rsa_RsaesPkcs1V1_5Encrypt(hrsa, &plain, &cipher),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if ( *io_cipher_len < binstr_Len(cipher) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    mem_Copy( out_cipher, binstr_Buf(cipher), binstr_Len(cipher) );
    *io_cipher_len = binstr_Len(cipher);

EXIT:
    hci_l_mem_binstrFree(cipher);
    rsa_Fin(hrsa);
    MSGEXT(("hci_l_cryptoEncryptRsaesPkcs1V1_5"));
    return ret;
}


uint32_t
hci_l_cryptoDecryptRsaesPkcs1V1_5(uint32_t             in_pkcs8_key_len,
                                  uint8_t             *in_pkcs8_key,
                                  uint32_t             in_cipher_len,
                                  uint8_t             *in_cipher,
                                  uint32_t            *io_plain_len,
                                  uint8_t             *out_plain)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t            pkcs8       = { in_pkcs8_key_len, in_pkcs8_key };
    binstr_t            cipher      = { in_cipher_len, in_cipher };
    rsa_handle_t        hrsa        = NULL;
    binstr_t           *plain       = NULL;

    MSGENT(("hci_l_cryptoDecryptRsaesPkcs1V1_5"));

    if ( 0    == binstr_Check(&pkcs8)   ||
         0    == binstr_Check(&cipher)  ||
         NULL == io_plain_len         ||
         0    == *io_plain_len        ||
         NULL == out_plain) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RET(l_InitRsa(&pkcs8, &hrsa));

    RETR(rsa_RsaesPkcs1V1_5Decrypt(hrsa, &cipher, &plain),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if ( *io_plain_len < binstr_Len(plain) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    mem_Copy( out_plain, binstr_Buf(plain), binstr_Len(plain) );
    *io_plain_len = binstr_Len(plain);

EXIT:
    hci_l_mem_binstrFree(plain);
    l_FinRsa(hrsa);
    MSGEXT(("hci_l_cryptoDecryptRsaesPkcs1V1_5"));
    return ret;
}

uint32_t
hci_l_cryptoEncryptRsaesOaep(uint32_t             in_n_len,
                             uint8_t             *in_n,
                             uint32_t             in_e_len,
                             uint8_t             *in_e,
                             uint32_t             in_oaep_len,
                             uint8_t             *in_oaep,
                             uint32_t             in_plain_len,
                             uint8_t             *in_plain,
                             uint32_t            *io_cipher_len,
                             uint8_t             *out_cipher)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t            n           = { in_n_len, in_n };
    binstr_t            e           = { in_e_len, in_e };
    binstr_t            pad         = { in_oaep_len, in_oaep };
    binstr_t            plain       = { in_plain_len, in_plain };
    rsa_handle_t        hrsa        = NULL;
    binstr_t           *cipher      = NULL;

    MSGENT(("hci_l_cryptoEncryptRsaesOaep"));

    if ( 0    == binstr_Check(&n)       ||
         0    == binstr_Check(&e)       ||
         0    == binstr_Check(&plain)   ||
         NULL == io_cipher_len         ||
         0    == *io_cipher_len        ||
         NULL == out_cipher) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RETR(rsa_Init(&hrsa, &n, &e, NULL, NULL, NULL, NULL, NULL, NULL),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    RETR(rsa_RsaesOaepEncrypt(hrsa, &plain, &pad, &cipher),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if ( *io_cipher_len < binstr_Len(cipher) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    mem_Copy( out_cipher, binstr_Buf(cipher), binstr_Len(cipher) );
    *io_cipher_len = binstr_Len(cipher);

EXIT:
    hci_l_mem_binstrFree(cipher);
    rsa_Fin(hrsa);
    MSGEXT(("hci_l_cryptoEncryptRsaesOaep"));
    return ret;

}

uint32_t
hci_l_cryptoDecryptRsaesOaep(uint32_t             in_pkcs8_key_len,
                             uint8_t             *in_pkcs8_key,
                             uint32_t             in_cipher_len,
                             uint8_t             *in_cipher,
                             uint32_t             in_oaep_len,
                             uint8_t             *in_oaep,
                             uint32_t            *io_plain_len,
                             uint8_t             *out_plain)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t            pkcs8       = { in_pkcs8_key_len, in_pkcs8_key };
    binstr_t            cipher      = { in_cipher_len, in_cipher };
    binstr_t            pad         = { in_oaep_len, in_oaep };
    rsa_handle_t        hrsa        = NULL;
    binstr_t           *plain       = NULL;

    MSGENT(("hci_l_cryptoDecryptRsaesOaep"));

    if ( 0    == binstr_Check(&pkcs8)   ||
         0    == binstr_Check(&cipher)  ||
         NULL == io_plain_len         ||
         0    == *io_plain_len        ||
         NULL == out_plain) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RET(l_InitRsa(&pkcs8, &hrsa));

    RETR(rsa_RsaesOaepDecrypt(hrsa, &cipher, &pad, &plain),
         ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB);

    if ( *io_plain_len < binstr_Len(plain) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    mem_Copy( out_plain, binstr_Buf(plain), binstr_Len(plain) );
    *io_plain_len = binstr_Len(plain);

EXIT:
    hci_l_mem_binstrFree(plain);
    l_FinRsa(hrsa);
    MSGEXT(("hci_l_cryptoDecryptRsaesOaep"));
    return ret;
}

uint32_t
hci_l_cryptoSignRsassaPkcs1V1_5(uint32_t             in_pkcs8_key_len,
                                uint8_t             *in_pkcs8_key,
                                hci_dig_algo_t       in_digest_algo,
                                uint32_t             in_message_len,
                                uint8_t             *in_message,
                                uint32_t            *io_signature_len,
                                uint8_t             *out_signature)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t            pkcs8       = { in_pkcs8_key_len, in_pkcs8_key };
    binstr_t            message     = { in_message_len, in_message };
    rsa_handle_t        hrsa        = NULL;
    binstr_t           *signature   = NULL;

    MSGENT(("hci_l_cryptoSignRsaesPkcs1V1_5"));

    if ( 0    == binstr_Check(&pkcs8)   ||
         0    == binstr_Check(&message) ||
         NULL == io_signature_len       ||
         0    == *io_signature_len      ||
         NULL == out_signature) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    RET(l_InitRsa(&pkcs8, &hrsa));

    if ( in_digest_algo == HCI_DIG_ALGO_SHA256 ) {
        RETR( rsa_RsassaPkcs1V1_5Sign(hrsa, RSA_DIGESTALGO_SHA256, &message, &signature),
              ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB );
    } else if ( in_digest_algo == HCI_DIG_ALGO_SHA1 ) {
        RETR( rsa_RsassaPkcs1V1_5Sign(hrsa, RSA_DIGESTALGO_SHA1, &message, &signature),
              ERR_NOMEM == ret ? ERR_HCI_NOMEM : ERR_HCI_CRYPTOLIB );
    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( *io_signature_len < binstr_Len(signature) ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    mem_Copy( out_signature, binstr_Buf(signature), binstr_Len(signature) );
    *io_signature_len = binstr_Len(signature);

EXIT:
    hci_l_mem_binstrFree(signature);
    l_FinRsa(hrsa);
    MSGEXT(("hci_l_cryptoSignRsaesPkcs1V1_5"));
    return ret;
}

