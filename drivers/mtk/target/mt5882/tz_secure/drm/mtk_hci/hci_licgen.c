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
#include "str_api.h"
#include "binstr_api.h"
#include "byte_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"
#include "hci_crypto.h"


#define L_RSA_PUB_E_HEADER_LEN     2
#define L_RSA_PUB_N_HEADER_LEN     2


/*For BKB parsing*/
#define L_BKB          "BroadcastKeyBlock>"
#define L_BROADCASTKEY "broadcastKey"
#define L_QUOTE_STRING "\""
#define L_BK_LEN       (24)

#if 1 == HCI_L_API_HCI_ENCRYPTKEYDATA
/* Key_data format for rsa public key is length of e (2 Bytes) + e + length of n (2 Bytes) + n */
static uint32_t
l_DecryptKeyDataRsaEN(uint32_t     in_data_len,
                      uint8_t     *in_data,
                      binstr_t   **out_pub_e,
                      binstr_t   **out_pub_n)
{
    uint32_t   ret       = ERR_HCI_OK;
    u_int16_t  len       = 0;
    uint8_t   *cur       = NULL;
    int32_t    remaining = 0;
    binstr_t  *pub_e     = NULL;
    binstr_t  *pub_n     = NULL;

    MSGENT(("l_DecryptKeyDataRsaEN"));

    /* Data length MUST be at least the size of two length fields and
     * one byte of data for each of N and E
     */
    if ( in_data_len < (2 * sizeof(len) + 2) || NULL == in_data ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    cur = in_data;
    remaining = in_data_len;

    len = byte_BE2R(cur);
    cur = cur + L_RSA_PUB_E_HEADER_LEN;

    remaining -= len + sizeof(len);
    if ( remaining <= 0 ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET(hci_l_mem_BinstrAllocWithCopy(len, cur, &pub_e));
    cur = cur + len;

    len = byte_BE2R(cur);
    cur = cur + L_RSA_PUB_N_HEADER_LEN;

    remaining -= len + sizeof(len);
    if ( remaining != 0 ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET(hci_l_mem_BinstrAllocWithCopy(len, cur, &pub_n));
    *out_pub_e = pub_e; pub_e = NULL;
    *out_pub_n = pub_n; pub_n = NULL;
EXIT:
    hci_l_mem_binstrFree(pub_e);
    hci_l_mem_binstrFree(pub_n);
    MSGEXT(("l_DecryptKeyDataRsaEN"));
    return ret;
}
#endif

#if 1 == HCI_L_API_HCI_IMPORTBKBCONTAINER
static uint32_t
l_SearchBKStart(char    *in_bkb_container,
                char   **out_bk_start)
{
    uint32_t ret      = ERR_HCI_OK;
    char    *read_cur = NULL;
    char    *bk_start = NULL;
    char    *ptr      = NULL;

    MSGENT(("l_SearchBKStart"));

    read_cur = (char *) str_Str(in_bkb_container, L_BROADCASTKEY);
    if ( NULL == read_cur ) {
        RET(ERR_HCI_ILLEGALBKB);
    }

    read_cur += sizeof(L_BROADCASTKEY) - 1;

    /* Only one BK is allowed in a container */
    ptr = (char *) str_Str(read_cur, L_BROADCASTKEY);
    if ( NULL != ptr ) {
        RET(ERR_HCI_ILLEGALBKB);
    }

    read_cur = (char *) str_Str(read_cur, L_QUOTE_STRING);
    if ( NULL == read_cur ) {
        RET(ERR_HCI_ILLEGALBKB);
    }

    read_cur += sizeof(L_QUOTE_STRING) - 1;

    bk_start = read_cur;

    read_cur += L_BK_LEN;
    if ( '"' != read_cur[0] ) {
        RET(ERR_HCI_ILLEGALBKB);
    }

    *out_bk_start = bk_start;

EXIT:
    MSGEXT(("l_SearchBKStart"));
    return ret;
}

static uint32_t
l_SearchBK(char    *in_bkb_container,
           char   **out_bk_start)
{
    uint32_t ret      = ERR_HCI_OK;
    char    *read_cur = NULL;
    char    *bk_start = NULL;

    MSGENT(("l_SearchBK"));

    read_cur = (char *) str_Str(in_bkb_container, L_BROADCASTKEY);
    /* BKB is non-empty */
    if ( NULL != read_cur ) {
        RET( l_SearchBKStart(in_bkb_container, &read_cur) );

        bk_start = read_cur;

        /* Check if BKB exists */
        read_cur = (char *) str_Str(read_cur, L_BKB);
        if ( NULL == read_cur ) {
            RET(ERR_HCI_ILLEGALBKB);
        }
    } else {
        /* Check if illegal (BKB exists but there is no BK) */
        read_cur = (char *) str_Str(in_bkb_container, L_BKB);
        if ( NULL != read_cur ) {
            RET(ERR_HCI_ILLEGALBKB);
        }
    }

    *out_bk_start = bk_start;

EXIT:
    MSGEXT(("l_SearchBK"));
    return ret;
}

static uint32_t
l_ExtractBK(binstr_t   *in_bkb_container,
            binstr_t  **out_bk,
            binstr_t  **out_container)
{
    uint32_t  ret       = ERR_HCI_OK;
    uint32_t  len       = binstr_Len(in_bkb_container) + 1;
    char     *bk_start  = NULL;
    binstr_t  plain_bk  = { 0, NULL };
    binstr_t *bk        = NULL;
    binstr_t *container = NULL;

    MSGENT(("l_ExtractBK"));

    /* Make a null-terminated binstr_t out of a binstr_t */
    RET( hci_l_mem_BinstrAlloc( len, &container ) );
    mem_Copy(binstr_Buf(container), binstr_Buf(in_bkb_container), len - 1);
    binstr_Buf(container)[len - 1] = 0;

    RET( l_SearchBK((char *)binstr_Buf(container), &bk_start) );

    if ( NULL != bk_start ) {
        binstr_Len(&plain_bk) = L_BK_LEN;
        binstr_Buf(&plain_bk) = (uint8_t *) bk_start;
        RETR( hci_l_b64Decode(&plain_bk, &bk),
              ret == ERR_HCI_NOMEM ? ret : ERR_HCI_ILLEGALBKB );
        RET( hci_l_Scramble(bk) );

        mem_Set( bk_start, '0', L_BK_LEN); /* scrub out BK in container*/
        binstr_Len(container) -= 1;        /*null termination shall not be included in out_container*/

        *out_container = container;
        container = NULL;
    } else {
        *out_container = NULL;
    }

    *out_bk = bk;
    bk = NULL;

EXIT:
    hci_l_mem_binstrFree(bk);
    hci_l_mem_binstrFree(container);

    MSGEXT(("l_ExtractBK"));
    return ret;
}
#endif /* #if 1 == HCI_L_API_HCI_IMPORTBKBCONTAINER*/


#if 1 == HCI_L_API_HCI_ENCRYPTDATA || 1 == HCI_L_API_HCI_ENCRYPTKEYDATA
static uint32_t
l_GenerateIV( uint32_t *out_iv_len, uint8_t **out_iv )
{
    uint32_t    ret    = ERR_HCI_OK;
    uint32_t    iv_len = HCI_L_AES_BLOCKSIZE;
    uint8_t    *iv     = NULL;

    MSGENT(("l_GenerateIV"));

    RET(hci_l_mem_Alloc(iv_len, &iv));
    RET(hci_l_cryptoGenerateRandom(iv_len, iv));

    *out_iv_len = iv_len;
    *out_iv = iv;
    iv = NULL;

EXIT:
    hci_l_mem_Free(iv);
    MSGEXT(("l_GenerateIV"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_ENCRYPTKEYDATA
static uint32_t
l_EncryptKeyDataRsa(binstr_t      *in_raw_key,
                    hci_enc_mode_t in_enc_mode,
                    hci_enc_pad_t  in_enc_pad,
                    binstr_t      *in_oaep_param,
                    binstr_t      *in_raw_data,
                    uint32_t      *out_key_data_len,
                    uint8_t      **out_key_data)
{
    uint32_t          ret                   = ERR_HCI_OK;
    uint32_t          data_len              = 0;
    uint8_t          *data                  = NULL;
    binstr_t         *pub_e                 = NULL;
    binstr_t         *pub_n                 = NULL;

    MSGENT(("l_EncryptKeyDataRsa"));

    if ( HCI_ENC_MODE_NONE      != in_enc_mode ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_DecryptKeyDataRsaEN(binstr_Len(in_raw_key), binstr_Buf(in_raw_key), &pub_e, &pub_n) );

    data_len = binstr_Len(pub_n);
    RET( hci_l_mem_Alloc(data_len, &data) );

    switch (in_enc_pad) {
    case HCI_ENC_PAD_PKCS1_V1_5:
        RETR( hci_l_cryptoEncryptRsaesPkcs1V1_5(binstr_Len(pub_n), binstr_Buf(pub_n), binstr_Len(pub_e), binstr_Buf(pub_e),
                                                binstr_Len(in_raw_data), binstr_Buf(in_raw_data), &data_len, data),
              ERR_HCI_TOOSMALLBUFFER == ret ? ERR_HCI_INTERNALERROR : ret);
        break;
    case HCI_ENC_PAD_OAEP:
        if (NULL == in_oaep_param) {
            RET(ERR_HCI_ILLEGALARGS);
        }
        RETR( hci_l_cryptoEncryptRsaesOaep(binstr_Len(pub_n), binstr_Buf(pub_n), binstr_Len(pub_e), binstr_Buf(pub_e),
                                           binstr_Len(in_oaep_param), binstr_Buf(in_oaep_param),
                                           binstr_Len(in_raw_data), binstr_Buf(in_raw_data), &data_len, data),
              ERR_HCI_TOOSMALLBUFFER == ret ? ERR_HCI_INTERNALERROR : ret);
        break;
    default:
        RET(ERR_HCI_ILLEGALARGS);
    }

    *out_key_data_len = data_len;
    *out_key_data     = data; data = NULL;

EXIT:
    hci_l_mem_Free(data);
    hci_l_mem_binstrFree(pub_e);
    hci_l_mem_binstrFree(pub_n);
    MSGEXT(("l_EncryptKeyDataRsa"));
    return ret;
}

static uint32_t
l_EncryptKeyDataAes(binstr_t      *in_raw_key,
                    hci_enc_mode_t in_enc_mode,
                    hci_enc_pad_t  in_enc_pad,
                    binstr_t      *in_raw_data,
                    uint32_t      *out_iv_len,
                    uint8_t      **out_iv,
                    uint32_t      *out_key_data_len,
                    uint8_t      **out_key_data)
{
    uint32_t          ret                   = ERR_HCI_OK;
    uint32_t          data_len              = 0;
    uint8_t          *data                  = NULL;
    uint32_t          iv_len                = 0;
    uint8_t          *iv                    = NULL;

    MSGENT(("l_EncryptKeyDataAes"));

    if ( NULL == out_iv_len                 ||
         NULL == out_iv                     ||
         HCI_ENC_MODE_CBC != in_enc_mode ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET(l_GenerateIV(&iv_len, &iv));

    data_len = HCI_L_AES_KEYSIZE + HCI_L_AES_BLOCKSIZE;   /* key data + worst-case padding length */
    RET( hci_l_mem_Alloc(data_len, &data) );

    RETR( hci_l_cryptoEncryptAES128CBC(in_enc_pad, binstr_Buf(in_raw_key), iv,
                                       binstr_Len(in_raw_data), binstr_Buf(in_raw_data), &data_len, data),
          ERR_HCI_TOOSMALLBUFFER == ret ? ERR_HCI_INTERNALERROR : ret);

    *out_iv_len = iv_len;
    *out_iv     = iv;
    iv = NULL;

    *out_key_data_len = data_len;
    *out_key_data     = data; data = NULL;

EXIT:
    hci_l_mem_Free(data);
    hci_l_mem_Free(iv);
    MSGEXT(("l_EncryptKeyDataAes"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_GENERATEHASH
uint32_t
hci_GenerateHash(hci_key_ref_t   in_ref,
                 hci_dig_algo_t  in_dig_algo,
                 uint32_t        in_data_len,
                 uint8_t        *in_data,
                 uint32_t       *out_hash_len,
                 uint8_t       **out_hash)
{
    uint32_t      ret       = ERR_HCI_OK;
    binstr_t      data      = { in_data_len, in_data};
    uint8_t      *hash_buf  = NULL;
    binstr_t     *hash      = NULL;

    MSGENT(("hci_GenerateHash"));

    RET( hci_l_IsHciLocked() );

    if ( (in_data == NULL && in_data_len != 0) ||
         (in_data != NULL && in_data_len == 0) ||
         (NULL == out_hash_len)                 ||
         (NULL == out_hash) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CalculateHash(in_ref, in_dig_algo, &data, &hash) );
    RET(hci_l_mem_AllocWithCopy(binstr_Len(hash), binstr_Buf(hash), &hash_buf));

    *out_hash_len = binstr_Len(hash);
    *out_hash = hash_buf; hash_buf = NULL;

EXIT:
    hci_l_mem_Free(hash_buf);
    hci_l_mem_binstrFree(hash);
    MSGEXT(("hci_GenerateHash"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_ENCRYPTDATA
extern uint32_t
hci_EncryptData(hci_key_ref_t     in_ref,
                hci_enc_algo_t    in_enc_algo,
                hci_enc_mode_t    in_enc_mode,
                hci_enc_pad_t     in_enc_pad,
                uint32_t          in_oaep_param_len,
                uint8_t          *in_oaep_param,
                uint32_t          in_data_len,
                uint8_t          *in_data,
                uint32_t         *out_iv_len,
                uint8_t         **out_iv,
                uint32_t         *out_data_len,
                uint8_t         **out_data)
{
    uint32_t          ret               = ERR_HCI_OK;
    hci_l_key_ref_t  *key_ref           = in_ref;
    uint32_t          iv_len            = 0;
    uint8_t          *iv                = NULL;
    binstr_t         *raw_key           = NULL;
    uint32_t          enc_len;
    uint8_t          *enc_data          = NULL;
    hci_bool_t        usable            = HCI_FALSE;

    MSGENT(("hci_EncryptData"));

    RET( hci_l_IsHciLocked() );

    if ( (NULL == in_ref)                  ||
         ((0 == in_oaep_param_len) && (NULL != in_oaep_param)) ||
         ((0 != in_oaep_param_len) && (NULL == in_oaep_param)) ||
         (NULL == out_iv_len)              ||
         (NULL == out_iv)                  ||
         (NULL == out_data_len)            ||
         (NULL == out_data)                ||
         (in_enc_algo != HCI_ENC_ALGO_AES) ||
         (in_enc_mode != HCI_ENC_MODE_CBC) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CheckKeyAttr(key_ref->key_type, HCI_L_ATTR_ENC_KEY, &usable) );
    if ( HCI_TRUE != usable ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (NULL == key_ref->key_data) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET( hci_l_Unscramble(key_ref->key_data, &raw_key) );

    enc_len = in_data_len + HCI_L_AES_BLOCKSIZE;
    RET( hci_l_mem_Alloc(enc_len, &enc_data) );
    RET( l_GenerateIV(&iv_len, &iv) );
    RET( hci_l_cryptoEncryptAES128CBC(in_enc_pad, binstr_Buf(raw_key), iv,
                                      in_data_len, in_data, &enc_len, enc_data) );

    *out_iv_len = iv_len;
    *out_iv     = iv; iv = NULL;
    *out_data_len = enc_len;
    *out_data = enc_data; enc_data = NULL;

EXIT:
    hci_l_mem_Free(iv);
    hci_l_mem_Free(enc_data);
    hci_l_FreeKey(raw_key);
    MSGEXT(("hci_EncryptData"));
    return ret;
}
#endif

#if 1 == HCI_L_API_HCI_ENCRYPTKEYDATA
uint32_t
hci_EncryptKeyData(hci_key_ref_t  in_ref,
                   hci_enc_algo_t in_enc_algo,
                   hci_enc_mode_t in_enc_mode,
                   hci_enc_pad_t  in_enc_pad,
                   uint32_t       in_oaep_param_len,
                   uint8_t       *in_oaep_param,
                   hci_key_ref_t  in_key,
                   uint32_t      *out_iv_len,
                   uint8_t      **out_iv,
                   uint32_t      *out_key_data_len,
                   uint8_t      **out_key_data)
{
    uint32_t          ret                   = ERR_HCI_OK;
    hci_l_key_ref_t  *key_ref               = in_ref;
    hci_l_key_ref_t  *data_ref              = in_key;
    binstr_t         *raw_key               = NULL;
    binstr_t         *raw_data              = NULL;
    hci_bool_t        usable                = HCI_FALSE;
    binstr_t          oaep_param            = { in_oaep_param_len, in_oaep_param };

    MSGENT(("hci_EncryptKeyData"));

    RET( hci_l_IsHciLocked() );

    if ( NULL == in_ref           ||
         NULL == in_key           ||
         NULL == out_iv_len       ||
         NULL == out_iv           ||
         NULL == out_key_data_len ||
         NULL == out_key_data ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CheckKeyAttr(key_ref->key_type, HCI_L_ATTR_ENC_KEY, &usable) );
    if ( (HCI_TRUE != usable) ||
         (HCI_L_KEY_TYPE_GENERATED != data_ref->key_type &&
          HCI_L_KEY_TYPE_CONTENT   != data_ref->key_type  ) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( (NULL == data_ref->key_data) || (NULL == key_ref->key_data) ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET(hci_l_Unscramble(data_ref->key_data, &raw_data));
    RET(hci_l_Unscramble(key_ref->key_data, &raw_key));

    switch (in_enc_algo) {
    case HCI_ENC_ALGO_RSA:
        RET(l_EncryptKeyDataRsa(raw_key, in_enc_mode, in_enc_pad, &oaep_param,
                                raw_data, out_key_data_len, out_key_data));
        break;
    case HCI_ENC_ALGO_AES:
        RET(l_EncryptKeyDataAes(raw_key, in_enc_mode, in_enc_pad,
                                raw_data, out_iv_len, out_iv, out_key_data_len, out_key_data));
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

EXIT:
    hci_l_FreeKey(raw_key);
    hci_l_FreeKey(raw_data);
    MSGEXT(("hci_EncryptKeyData"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_IMPORTBKBCONTAINER
uint32_t
hci_ImportBKBContainer(uint32_t    in_container_len,
                       uint8_t    *in_container,
                       hci_bool_t  in_from_ppproc,
                       uint32_t    in_private_data_len,
                       uint8_t    *in_private_data,
                       uint32_t   *out_container_len,
                       uint8_t   **out_container,
                       uint32_t   *out_bk_len,
                       uint8_t   **out_bk)
{
    uint32_t   ret                = ERR_HCI_OK;
    binstr_t   input_container_bs = { in_container_len, in_container };
    binstr_t  *bk                 = NULL;
    uint32_t   bk_len             = 0;
    uint8_t   *bk_buf             = NULL;
    binstr_t  *container          = NULL;
    uint32_t   container_len      = 0;
    uint8_t   *container_buf      = NULL;
    binstr_t  *container_clear    = NULL;

    MSGENT(("hci_ImportBKBContainer"));

    RET( hci_l_IsHciLocked() );

    if ( (0 == binstr_Check(&input_container_bs))               ||
         (NULL == out_container_len || NULL == out_container)   ||
         (NULL == out_bk_len        || NULL == out_bk) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( in_from_ppproc == TRUE ) {
        /* Unscramble the BKB container.( It should have been protected when returned by hci_ImportPpprocData(.) ) */
        RET( hci_l_Unscramble( &input_container_bs, &container_clear ) );
    } else {
        if ( (in_private_data == NULL && in_private_data_len != 0) ||
             (in_private_data != NULL && in_private_data_len == 0) ) {
            RET( ERR_HCI_ILLEGALARGS );
        }
        container_clear = &input_container_bs;
    }

    RET( l_ExtractBK(container_clear, &bk, &container) );

    if ( NULL != bk ) {
        bk_len = binstr_Len(bk);
        RET(hci_l_mem_AllocWithCopy(bk_len, binstr_Buf(bk), &bk_buf));
    }

    if ( NULL != container ) {
        container_len = binstr_Len(container);
        RET(hci_l_mem_AllocWithCopy(container_len, binstr_Buf(container), &container_buf));
    }

    *out_container_len = container_len;
    *out_container     = container_buf;
    container_buf      = NULL;
    *out_bk_len        = bk_len;
    *out_bk            = bk_buf;
    bk_buf             = NULL;

EXIT:
    hci_l_mem_binstrFree(bk);
    if ( (in_from_ppproc == TRUE) && (container_clear != NULL) ) {
        hci_l_DataScrub(binstr_Buf(container_clear), binstr_Len(container_clear));
        hci_l_mem_binstrFree(container_clear);
    }
    hci_l_mem_Free(bk_buf);
    hci_l_mem_binstrFree(container);
    hci_l_mem_Free(container_buf);

    MSGEXT(("hci_ImportBKBContainer"));
    return ret;
}
#endif /* HCI_L_API_HCI_IMPORTBKBCONTAINER */

