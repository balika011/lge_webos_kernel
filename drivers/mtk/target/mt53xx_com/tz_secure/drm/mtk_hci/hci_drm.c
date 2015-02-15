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
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_crypto.h"
#include "hci_internal_api.h"



#define L_SF_MAXLEVEL        16
#define L_DKS_SIZE           4096     /**< Device Key Set size*/
#define L_BK_HASH_LEN        16

/* Public decrypt using the exported public key */
static uint32_t
l_DataDecryptRSA(hci_l_key_ref_t  *in_keyref,
                 hci_enc_pad_t     in_enc_pad,
                 uint32_t          in_oaep_param_len,
                 uint8_t          *in_oaep_param,
                 binstr_t         *in_data,
                 uint32_t         *io_data_len,
                 uint8_t          *out_data)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t           *tmp_key     = NULL;

    MSGENT(("l_DataDecryptRSA"));

    /* Unscramble key to local storage here */
    RET( hci_l_Unscramble(in_keyref->key_data, &tmp_key));

    switch (in_enc_pad) {
    case HCI_ENC_PAD_PKCS1_V1_5:
        RET( hci_l_cryptoDecryptRsaesPkcs1V1_5(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                               binstr_Len(in_data), binstr_Buf(in_data),
                                               io_data_len, out_data) );
        break;
    case HCI_ENC_PAD_OAEP:
        RET( hci_l_cryptoDecryptRsaesOaep(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                          binstr_Len(in_data), binstr_Buf(in_data),
                                          in_oaep_param_len, in_oaep_param,
                                          io_data_len, out_data) );
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

EXIT:
    hci_l_FreeKey(tmp_key);
    MSGEXT(("l_DataDecryptRSA"));
    return ret;
}

static uint32_t
l_DataDecryptAES(hci_l_key_ref_t  *in_keyref,
                 hci_enc_mode_t    in_enc_mode,
                 hci_enc_pad_t     in_enc_pad,
                 bool_t            in_is_final,
                 binstr_t         *in_data,
                 binstr_t         *io_iv,
                 uint32_t         *io_data_len,
                 uint8_t          *out_data)
{
    uint32_t            ret         = ERR_HCI_OK;
    binstr_t           *tmp_key     = NULL;
    uint8_t            *ct          = binstr_Buf(in_data);
    uint32_t            ct_len      = binstr_Len(in_data);
    uint8_t            *iv          = binstr_Buf(io_iv);

    MSGENT(("l_DataDecryptAES"));

    if ( (in_data == NULL)    ||
         (out_data == NULL)   ||
         ((HCI_L_KEY_TYPE_CONTENT != in_keyref->key_type) &&
          (HCI_L_KEY_TYPE_MESSAGE != in_keyref->key_type)) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( *io_data_len < ct_len ) {
        RET( ERR_HCI_TOOSMALLBUFFER );
    }

    /* Unscramble the key to local storage here */
    RET( hci_l_Unscramble(in_keyref->key_data, &tmp_key));

    if ( in_enc_mode == HCI_ENC_MODE_CBC ) {
        hci_enc_pad_t  padding = in_enc_pad;

        if (HCI_FALSE == in_is_final) {
            padding = HCI_ENC_PAD_NONE;
        }

        RET(hci_l_cryptoDecryptAES128CBC(padding, binstr_Buf(tmp_key), iv,
                                         ct_len, ct, &ct_len, out_data));

    } else if ( in_enc_mode == HCI_ENC_MODE_RTB ) {

        RET(hci_l_cryptoDecryptAES128RTB(binstr_Buf(tmp_key), iv,
                                         ct_len, ct, &ct_len, out_data));

    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    *io_data_len = ct_len;

EXIT:
    hci_l_FreeKey(tmp_key);
    MSGEXT(("l_DataDecryptAES"));
    return ret;
}


static uint32_t
l_ImportAESEncryptedKey(hci_l_key_ref_t       *in_ref,
                        hci_enc_algo_t         in_enc_algo,
                        uint8_t               *in_pEncryptedKeyIV,
                        const int32_t          in_lenEncryptedKeyIV,
                        uint8_t               *in_pEncryptedKeyData,
                        const int32_t          in_lenEncryptedKeyData,
                        hci_l_key_ref_t       *out_ref)
{
    uint32_t        ret          = ERR_HCI_OK;
    uint8_t        *plain_key    = NULL;
    uint32_t        plain_len    = 0;
    int32_t         cipher_len   = 0;
    binstr_t       *tmp_key      = NULL;
    binstr_t       *keydata      = NULL;
    uint8_t        *aes_key      = NULL;

    MSGENT(("l_ImportAESEncryptedKey"));

    if ( in_ref == NULL               ||
         in_pEncryptedKeyIV == NULL   ||
         in_pEncryptedKeyData == NULL ||
         in_lenEncryptedKeyIV != HCI_L_AES_BLOCKSIZE ||
         in_lenEncryptedKeyData < 2 * HCI_L_AES_BLOCKSIZE ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    plain_len = cipher_len = in_lenEncryptedKeyData;

    RET( hci_l_mem_AllocWithZeros(cipher_len, &plain_key) );
    RET( hci_l_Unscramble(in_ref->key_data, &tmp_key) );
    aes_key = binstr_Buf(tmp_key);

    ret = hci_l_cryptoDecryptAES128CBC(HCI_ENC_PAD_XMLENC, aes_key, in_pEncryptedKeyIV,
                                       cipher_len, in_pEncryptedKeyData, &plain_len, plain_key);

    if (ERR_HCI_TOOSMALLBUFFER == ret) {
        RET(ERR_HCI_INTERNALERROR);
    }
    RET(ret);

    RET(hci_l_mem_BinstrAllocWithCopy(plain_len, plain_key, &keydata));
    RET(hci_l_GetKeyFormat(keydata, &(out_ref->format)));
    RET(hci_l_Scramble(keydata));

    out_ref->key_data = keydata; keydata = NULL;

EXIT:
    hci_l_FreeKey(keydata);
    if (plain_key) {
        hci_l_DataScrub (plain_key, plain_len);
        hci_l_mem_Free(plain_key);
    }
    hci_l_FreeKey(tmp_key);

    MSGEXT(("l_ImportAESEncryptedKey"));
    return ret;
}


static uint32_t
l_ImportRSAEncryptedKey( hci_l_key_ref_t     *in_ref,
                         hci_enc_pad_t        in_enc_pad,
                         const binstr_t      *in_oaepData,
                         const binstr_t      *in_pEncryptedKeyData,
                         hci_l_key_ref_t     *out_ref)
{
    uint32_t           ret            = ERR_HCI_OK;
    hci_l_key_ref_t   *ref            = in_ref;
    binstr_t          *plain_text     = NULL;
    binstr_t          *tmp_key        = NULL;

    MSGENT(("l_ImportRSAEncryptedKey"));

    if ( ref == NULL                  ||
         in_pEncryptedKeyData == NULL ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_Unscramble(in_ref->key_data, &tmp_key));

    RET( hci_l_mem_BinstrAlloc(binstr_Len(in_pEncryptedKeyData), &plain_text) );

    if ( HCI_ENC_PAD_PKCS1_V1_5 == in_enc_pad ) {

        ret = hci_l_cryptoDecryptRsaesPkcs1V1_5(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                                binstr_Len(in_pEncryptedKeyData), binstr_Buf(in_pEncryptedKeyData),
                                                &binstr_Len(plain_text), binstr_Buf(plain_text));

    } else if ( HCI_ENC_PAD_OAEP == in_enc_pad ) {

        ret = hci_l_cryptoDecryptRsaesOaep(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                           binstr_Len(in_pEncryptedKeyData), binstr_Buf(in_pEncryptedKeyData),
                                           binstr_Len(in_oaepData), binstr_Buf(in_oaepData),
                                           &binstr_Len(plain_text), binstr_Buf(plain_text));
    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (ERR_HCI_TOOSMALLBUFFER == ret) {
        RET(ERR_HCI_INTERNALERROR);
    }
    RET(ret);

    RET(hci_l_GetKeyFormat(plain_text, &(out_ref->format)));

    RET(hci_l_Scramble(plain_text));
    out_ref->key_data = plain_text; plain_text = NULL;

EXIT:
    hci_l_FreeKey(plain_text);
    hci_l_FreeKey(tmp_key);
    MSGEXT(("l_ImportRSAEncryptedKey"));
    return ret;
}


/*
 * Common functionality for decrypt, used by hci_DecryptStreamWithIV (no locking)
 *                                       and hci_DecryptData (locking)
 */
static uint32_t
l_Decrypt(hci_key_ref_t     in_ref,
          hci_enc_algo_t    in_enc_algo,
          hci_enc_mode_t    in_enc_mode,
          hci_enc_pad_t     in_enc_pad,
          hci_bool_t        in_is_final,
          uint32_t          in_oaep_param_len,
          uint8_t          *in_oaep_param,
          uint32_t          in_data_len,
          uint8_t          *in_data,
          uint32_t          in_iv_len,
          uint8_t          *io_iv,
          uint32_t         *io_data_len,
          uint8_t          *out_data)
{
    uint32_t          ret    = ERR_HCI_OK;
    hci_l_key_ref_t  *ref    = in_ref;
    binstr_t          data   = { in_data_len,  in_data };
    binstr_t          iv     = { in_iv_len,    io_iv };
    binstr_t         *iv_ptr = &iv;

    MSGENT(("l_Decrypt"));

    if ( (in_ref          == NULL)     ||
         (0 == binstr_Check(&data))    ||
         (0 == binstr_Check(&iv))      ||
         (io_data_len     == NULL)     ||
         (out_data        == NULL) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    switch ( in_enc_algo ) {
    case HCI_ENC_ALGO_RSA:
        RET( l_DataDecryptRSA(ref, in_enc_pad, in_oaep_param_len, in_oaep_param, &data, io_data_len, out_data) );
        break;
    case HCI_ENC_ALGO_AES:
        if (( 0 == binstr_Check(iv_ptr)) || (binstr_Len(iv_ptr) != HCI_L_AES_BLOCKSIZE)) {
            RET( ERR_HCI_ILLEGALARGS );
        }
        RET( l_DataDecryptAES(ref,
                              in_enc_mode,
                              in_enc_pad,
                              in_is_final,
                              &data,
                              iv_ptr,
                              io_data_len,
                              out_data) );
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

EXIT:
    MSGEXT(("l_Decrypt"));
    return ret;
}


static uint32_t
l_ImportStarfishKey(uint32_t          in_num_iterations,
                    uint32_t          in_dks_key_index,
                    uint32_t          in_enc_bk_len,
                    uint8_t          *in_enc_bk,
                    uint32_t          in_bk_hash_len,
                    uint8_t          *in_bk_hash,
                    uint32_t          in_iv_len,
                    uint8_t          *in_iv,
                    uint32_t          in_enc_ck_len,
                    uint8_t          *in_enc_ck,
                    hci_l_key_ref_t **out_ref)
{
    uint32_t         ret             = ERR_HCI_OK;
    hci_l_key_ref_t *sf_ref          = NULL;
    uint8_t         *dks             = NULL;
    uint8_t         *node_key        = NULL;
    uint8_t         *content_key     = NULL;
    uint8_t         *bcast_key       = NULL;
    binstr_t        *starfishkey     = NULL;
    binstr_t        *keydata         = NULL;
    uint32_t         content_key_len = 0;
    uint8_t          key_buf[HCI_L_AES_KEYSIZE];   /* SF_KEY_LENGTH   */
    uint8_t          out_buf[HCI_L_SHA1_DIGESTSIZE];
    uint32_t         buf_len         = HCI_L_SHA1_DIGESTSIZE;
    int32_t          i;

    MSGENT(("l_ImportStarfishKey"));

    RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_CONTENT, &sf_ref) );

    RET( hci_l_GetOctPersonality(L_PERS_TYPE_STARFISH, NULL, &starfishkey) );

#if !defined(CC_ENABLE_NAUTILUS_ATK) && defined(CC_ENABLE_NAUTILUS_NEW_KEYFORMAT)
    RET(hci_l_Unscramble2(starfishkey));
#endif    
    dks = binstr_Buf(starfishkey);

    /* Get the node key, make a tmp copy to avoid destroying the dks */
    node_key = dks + (in_dks_key_index * in_bk_hash_len);
    mem_Copy(key_buf, node_key, in_bk_hash_len);

    /* Hash it iteration times */
    for ( i=0; i < in_num_iterations; i++ ) {
        RET(hci_l_cryptoMakeDigest(HCI_DIG_ALGO_SHA1, in_bk_hash_len, key_buf, &buf_len, out_buf));
        mem_Copy(key_buf, out_buf, in_bk_hash_len);
    }

    RET(hci_l_mem_AllocWithZeros(in_enc_ck_len, &content_key));
    content_key_len = in_enc_ck_len;
    RET(hci_l_mem_AllocWithZeros(in_bk_hash_len, &bcast_key));

    if ( hci_l_cryptoDecryptAES128ECB(key_buf, in_enc_bk_len, in_enc_bk, &in_enc_bk_len, bcast_key) ) {
        RET( ERR_HCI_CRYPTOLIB );
    }

    /* Check the hash of the BK matches the supplied hash taken from the
     * BKB */
    RET(hci_l_cryptoMakeDigest(HCI_DIG_ALGO_SHA1, in_bk_hash_len, bcast_key, &buf_len, out_buf));

    if ( 0 != mem_Cmp(out_buf, in_bk_hash, in_bk_hash_len) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* Decrypt the supplied cipher data with the BK and return it to the
     * caller */
    if ( hci_l_cryptoDecryptAES128CBC(HCI_ENC_PAD_RFC2630, bcast_key, in_iv,
                                      in_enc_ck_len, in_enc_ck, &content_key_len, content_key) ) {
        RET( ERR_HCI_CRYPTOLIB );
    }

    RET(hci_l_mem_BinstrAllocWithCopy(content_key_len, content_key, &keydata));
    RET(hci_l_GetKeyFormat(keydata, &(sf_ref->format)));
    RET(hci_l_Scramble(keydata));

    sf_ref->key_data = keydata; keydata = NULL;

    *out_ref = sf_ref;
    sf_ref = NULL;

EXIT:
    hci_l_FreeKey(keydata);
    hci_l_FreeKey(starfishkey);
    if (content_key) {
        hci_l_DataScrub(content_key, content_key_len);
        hci_l_mem_Free(content_key);
    }
    if (bcast_key) {
        hci_l_DataScrub(bcast_key, in_bk_hash_len);
        hci_l_mem_Free(bcast_key);
    }
    hci_l_FreeKeyRef(sf_ref);
    MSGEXT(("l_ImportStarfishKey"));
    return ret;
}


static uint32_t
l_TranslateImportKeyType(hci_imp_key_type_t   in_imp_type,
                         hci_l_key_type_t    *out_key_type)
{
    uint32_t         ret      = ERR_HCI_OK;
    hci_l_key_type_t key_type = HCI_L_KEY_MAX;

    MSGENT(("l_TranslateImportKeyType"));

    switch (in_imp_type) {
    case HCI_IMP_KEY_TYPE_CONTENT:
        key_type = HCI_L_KEY_TYPE_CONTENT;
        break;

    case HCI_IMP_KEY_TYPE_BROADCAST:
        key_type = HCI_L_KEY_TYPE_BROADCAST;
        break;

    case HCI_IMP_KEY_TYPE_MESSAGE:
        key_type = HCI_L_KEY_TYPE_MESSAGE;
        break;

    case HCI_IMP_KEY_TYPE_DOMAIN:
        key_type = HCI_L_KEY_TYPE_DOMAIN;
        break;

    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_key_type = key_type;

EXIT:
    MSGEXT(("l_TranslateImportKeyType"));
    return ret;
}


uint32_t
hci_ImportKey(hci_key_ref_t      in_ref,
              hci_imp_key_type_t in_imp_type,
              hci_enc_algo_t     in_enc_algo,
              hci_enc_mode_t     in_enc_mode,
              hci_enc_pad_t      in_enc_pad,
              uint32_t           in_oaep_param_len,
              uint8_t           *in_oaep_param,
              uint32_t           in_iv_len,
              uint8_t           *in_iv,
              uint32_t           in_key_data_len,
              uint8_t           *in_key_data,
              hci_key_ref_t     *out_ref)
{
    uint32_t          ret        = ERR_HCI_OK;
    binstr_t         *cached_key = NULL;
    hci_l_key_ref_t  *ref        = in_ref;
    hci_l_key_ref_t  *new_keyref = NULL;
    hci_l_key_type_t  key_type   = HCI_L_KEY_MAX;

    MSGENT(("hci_ImportKey"));

    RET( hci_l_IsHciLocked() );

    /* oaep_param might be NULL so is not checked */
    if ( in_ref == NULL                              ||
         0 == in_key_data_len || NULL == in_key_data ||
         NULL == out_ref ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_TranslateImportKeyType(in_imp_type, &key_type) );

    RET( hci_l_CreateKeyRefStruct(key_type, &new_keyref) );

    if (in_enc_algo == HCI_ENC_ALGO_RSA) {
        binstr_t  oaep_param      = { in_oaep_param_len, in_oaep_param };
        binstr_t *oaep_param_ptr  = &oaep_param;
        binstr_t  key_data        = { in_key_data_len, in_key_data };

        RET( hci_l_GetKeyFromCache(in_imp_type,
                                   in_enc_algo,
                                   in_enc_mode,
                                   in_enc_pad,
                                   &oaep_param,
                                   NULL,
                                   &key_data,
                                   ref->key_data,
                                   &cached_key) );

        if ( NULL == cached_key ) {
            RET( l_ImportRSAEncryptedKey(ref,
                                         in_enc_pad,
                                         oaep_param_ptr,
                                         &key_data,
                                         new_keyref) );

            RET( hci_l_AddKeyToCache(in_imp_type,
                                     in_enc_algo,
                                     in_enc_mode,
                                     in_enc_pad,
                                     &oaep_param,
                                     NULL,
                                     &key_data,
                                     ref->key_data,
                                     new_keyref->key_data) );
        } else {
            new_keyref->key_data = cached_key;
            cached_key = NULL;

            RET(hci_l_GetKeyFormat(new_keyref->key_data, &new_keyref->format));
        }

    } else if (in_enc_algo == HCI_ENC_ALGO_AES) {
        if ( (in_enc_pad  != HCI_ENC_PAD_XMLENC) ||
             (in_enc_mode != HCI_ENC_MODE_CBC)   ||
             (in_iv == NULL)                     ||
             (in_iv_len != HCI_L_AES_BLOCKSIZE) ) {
            RET( ERR_HCI_ILLEGALARGS );
        }

        RET( l_ImportAESEncryptedKey(ref,
                                     in_enc_algo,
                                     in_iv,
                                     in_iv_len,
                                     in_key_data,
                                     in_key_data_len,
                                     new_keyref) );
    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_ref = (void *)new_keyref; new_keyref = NULL;

EXIT:
    hci_l_FreeKey(cached_key);
    hci_l_FreeKeyRef(new_keyref);
    MSGEXT(("hci_ImportKey"));
    return ret;
}

#if 1 == HCI_L_API_HCI_EXPORTKEY
uint32_t
hci_ExportKey(hci_key_ref_t    in_ref,
              uint32_t         in_private_data_len,
              uint8_t         *in_private_data,
              uint32_t        *out_private_data_len,
              uint8_t        **out_private_data)
{
    uint32_t         ret     = ERR_HCI_OK;
    binstr_t        *key     = NULL;
    uint8_t         *key_buf = NULL;
    hci_l_key_ref_t *ref     = in_ref;
    hci_bool_t       usable  = HCI_FALSE;

    MSGENT(("hci_ExportKey"));

    RET( hci_l_IsHciLocked() );

    if ( (NULL == in_ref)                                      ||
         (NULL == out_private_data_len)                        ||
         (NULL == out_private_data)                            ||
         (0 == in_private_data_len && NULL != in_private_data) ||
         (0 != in_private_data_len && NULL == in_private_data) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CheckKeyAttr(ref->key_type, HCI_L_ATTR_EXPORT, &usable) );
    if ( HCI_TRUE != usable ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET( hci_l_Unscramble(ref->key_data, &key) );
    RET( hci_l_mem_AllocWithCopy(binstr_Len(key), binstr_Buf(key), &key_buf) );

    *out_private_data_len = binstr_Len(key);
    *out_private_data     = key_buf; key_buf = NULL;

EXIT:
    if ( NULL != key_buf ) {
        hci_l_DataScrub(key_buf, binstr_Len(key));
        hci_FreeMem(key_buf);
    }
    hci_l_FreeKey(key);
    MSGEXT(("hci_ExportKey"));
    return ret;
}
#endif /* HCI_L_API_HCI_EXPORTKEY */


uint32_t
hci_ImportStarfishKey(uint32_t        in_num_iterations,
                      uint32_t        in_dks_key_index,
                      uint32_t        in_enc_bk_len,
                      uint8_t        *in_enc_bk,
                      uint32_t        in_bk_hash_len,
                      uint8_t        *in_bk_hash,
                      uint32_t        in_iv_len,
                      uint8_t        *in_iv,
                      uint32_t        in_enc_ck_len,
                      uint8_t        *in_enc_ck,
                      hci_key_ref_t  *out_ref)
{
    uint32_t         ret    = ERR_HCI_OK;
    hci_l_key_ref_t *sf_ref = NULL;

    MSGENT(("hci_ImportStarfishKey"));

    RET( hci_l_IsHciLocked() );

    if ((in_num_iterations     > L_SF_MAXLEVEL) ||
        (in_dks_key_index      > L_DKS_SIZE) ||
        (in_enc_bk             == NULL) ||
        (in_enc_bk_len         != HCI_L_AES_BLOCKSIZE) ||
        (in_bk_hash_len        != L_BK_HASH_LEN) ||
        (in_bk_hash            == NULL) ||
        (in_iv                 == NULL) ||
        (in_iv_len             != HCI_L_AES_BLOCKSIZE) ||
        (in_enc_ck             == NULL) ||
        (in_enc_ck_len         != 2 * HCI_L_AES_BLOCKSIZE ) ||
        (out_ref               == NULL)) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_ImportStarfishKey(in_num_iterations,
                             in_dks_key_index,
                             in_enc_bk_len,
                             in_enc_bk,
                             in_bk_hash_len,
                             in_bk_hash,
                             in_iv_len,
                             in_iv,
                             in_enc_ck_len,
                             in_enc_ck,
                             &sf_ref) );

    *out_ref = (void *)sf_ref;
    sf_ref   = NULL;

EXIT:
    MSGEXT(("hci_ImportStarfishKey"));
    return ret;
}


uint32_t
hci_GetKeyDataIfStarfish(hci_key_ref_t      in_ref,
                         uint32_t          *out_key_data_len,
                         uint8_t          **out_key_data)
{
    uint32_t         ret      = ERR_HCI_OK;
    hci_l_key_ref_t *ref      = in_ref;
    binstr_t        *tmp_key  = NULL;
    uint8_t         *key_data = NULL;

    MSGENT(("hci_GetKeyDataIfStarfish"));

    RET( hci_l_IsHciLocked() );

    if ( (in_ref           == NULL)   ||
         (out_key_data_len == NULL)   ||
         (out_key_data     == NULL) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    if (ref->format == HCI_L_KEY_FORMAT_STARFISH) {
        RET( hci_l_Unscramble(ref->key_data, &tmp_key) );
        RET( hci_l_mem_AllocWithCopy(binstr_Len(tmp_key), binstr_Buf(tmp_key), &key_data) );
        *out_key_data_len = binstr_Len(tmp_key);
        *out_key_data = key_data; key_data = NULL;
    } else {
        *out_key_data_len = 0;
        *out_key_data = NULL;
    }

EXIT:
    hci_l_mem_Free(key_data);
    hci_l_FreeKey(tmp_key);
    MSGEXT(("hci_GetKeyDataIfStarfish"));
    return ret;
}


uint32_t
hci_GetStarfishId(char  **out_id)
{
    uint32_t     ret = ERR_HCI_OK;
    char        *uid = NULL;

    MSGENT(("hci_GetStarfishId"));

    RET( hci_l_IsHciLocked() );

    if ( NULL == out_id ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_GetOctPersonality(L_PERS_TYPE_STARFISH, &uid, NULL) );

    *out_id = uid; uid = NULL;

EXIT:
    hci_l_mem_Free(uid);
    MSGEXT(("hci_GetStarfishId"));
    return ret;
}


uint32_t
hci_DecryptData(hci_key_ref_t     in_ref,
                hci_enc_algo_t    in_enc_algo,
                hci_enc_mode_t    in_enc_mode,
                hci_enc_pad_t     in_enc_pad,
                uint32_t          in_oaep_param_len,
                uint8_t          *in_oaep_param,
                uint32_t          in_iv_len,
                uint8_t          *in_iv,
                uint32_t          in_data_len,
                uint8_t          *in_data,
                uint32_t         *out_data_len,
                uint8_t         **out_data)
{
    uint32_t         ret      = ERR_HCI_OK;
    uint32_t         data_len = in_data_len;
    uint8_t         *data     = NULL;
    hci_l_key_ref_t *ref      = in_ref;

    MSGENT(("hci_DecryptData"));

    /* other argument checks are done in l_Decrypt function */
    if ( (ref                    == NULL)        ||
         (in_data_len            == 0)           ||
         (out_data_len           == NULL)        ||
         (out_data               == NULL )       ||
         (HCI_ENC_ALGO_AES       != in_enc_algo) ||
         (HCI_ENC_MODE_CBC       != in_enc_mode) ||
         (HCI_ENC_PAD_XMLENC     != in_enc_pad)  ||
         (HCI_L_KEY_TYPE_MESSAGE != ref->key_type) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_IsHciLocked() );

    RET(hci_l_mem_Alloc(in_data_len, &data));
    RETR( l_Decrypt(in_ref,
                    in_enc_algo,
                    in_enc_mode,
                    in_enc_pad,
                    TRUE,
                    in_oaep_param_len,
                    in_oaep_param,
                    in_data_len,
                    in_data,
                    in_iv_len,
                    in_iv,
                    &data_len,
                    data),
          ERR_HCI_TOOSMALLBUFFER == ret ? ERR_HCI_INTERNALERROR : ret);

    *out_data_len = data_len;
    *out_data     = data;      data = NULL;

EXIT:
    MSGEXT(("hci_DecryptData"));
    hci_l_mem_Free(data);
    return ret;
}


#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_IV
uint32_t
hci_DecryptStreamWithIV(hci_key_ref_t     in_ref,
                        hci_enc_algo_t    in_enc_algo,
                        hci_enc_mode_t    in_enc_mode,
                        hci_enc_pad_t     in_enc_pad,
                        hci_bool_t        in_is_final,
                        uint32_t          in_iv_len,
                        uint8_t          *io_iv,
                        uint32_t          in_data_len,
                        uint8_t          *in_data,
                        uint32_t         *io_data_len,
                        uint8_t          *out_data)
{
    uint32_t          ret = ERR_HCI_OK;
    hci_l_key_ref_t  *ref = in_ref;

    MSGENT(("hci_DecryptStreamWithIV"));

    /* other argument checks are done in l_Decrypt function */
    if ( in_ref == NULL ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* If this is not a content key then we do not have a key available for decryption */
    if ( ref->key_type == HCI_L_KEY_TYPE_GENERATED ) {
        RET( ERR_HCI_KEYNOTINREF );
    } else if ( ref->key_type != HCI_L_KEY_TYPE_CONTENT ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* @note HCI Lock is not checked in this API */

    RET( l_Decrypt(in_ref,
                   in_enc_algo,
                   in_enc_mode,
                   in_enc_pad,
                   in_is_final,
                   0, /* oaep param*/
                   NULL,
                   in_data_len,
                   in_data,
                   in_iv_len,
                   io_iv,
                   io_data_len,
                   out_data) );

EXIT:
    MSGEXT(("hci_DecryptStreamWithIV"));
    return ret;
}
#endif

#if 1 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET
uint32_t
hci_DecryptStreamWithOffset(hci_key_ref_t     in_ref,
                            hci_enc_algo_t    in_enc_algo,
                            hci_enc_mode_t    in_enc_mode,
                            hci_enc_pad_t     in_enc_pad,
                            np_size_t         in_offset,
                            np_size_t         in_ctrsize,
                            uint8_t          *in_ctr,
                            np_size_t         in_data_len,
                            uint8_t          *in_data,
                            np_size_t        *io_data_len,
                            uint8_t          *out_data)
{
    uint32_t          ret = ERR_HCI_OK;
    hci_l_key_ref_t  *ref = in_ref;
    binstr_t         *key = NULL;

    MSGENT(("hci_DecryptStreamWithOffset"));

    /* @note HCI Lock is not checked in this API */
    if ( (in_ref == NULL) ||
         HCI_ENC_ALGO_AES != in_enc_algo ||
         HCI_ENC_MODE_CTR != in_enc_mode ||
         HCI_ENC_PAD_NONE != in_enc_pad   ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( (NULL == ref->key_data) ||
         (ref->key_type == HCI_L_KEY_TYPE_GENERATED) ) {
        RET( ERR_HCI_KEYNOTINREF );
    } else if ( ref->key_type != HCI_L_KEY_TYPE_CONTENT ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_Unscramble(ref->key_data, &key));
    RET( hci_l_cryptoDecryptAES128CTR(binstr_Buf(key),
                                      in_offset,
                                      in_ctrsize,
                                      in_ctr,
                                      in_data_len,
                                      in_data,
                                      io_data_len,
                                      out_data) );
EXIT:
    hci_l_FreeKey(key);
    MSGEXT(("hci_DecryptStreamWithOffset"));
    return ret;
}
#endif
