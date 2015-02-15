/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2010,2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "mem_api.h"
#include "binstr_api.h"
#include "byte_api.h"
/* sec/marlin */
//#include "ssmem_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#ifdef NPOPT_USE_NPPERS
#include "hci_bootkey.h"
#endif
#include "hci_internal_api.h"
#include "hci_crypto.h"


#define L_RSA_PUB_E_HEADER_LEN (2)
#define L_RSA_PUB_N_HEADER_LEN (2)

static const hci_bool_t g_move_attr[HCI_L_KEY_MAX] = {HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
#if 1 == HCI_L_SERV_MS3
                                                      HCI_TRUE,
#else
                                                      HCI_FALSE,
#endif
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_TRUE, /* CK move is allowed */
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_TRUE,
                                                      HCI_FALSE,
                                                     }; /* Move of generated key is allowed */

static const hci_bool_t g_export_attr[HCI_L_KEY_MAX] = {HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_TRUE,  /* Export of CK is allowed */
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_FALSE,
                                                        HCI_TRUE,
                                                        HCI_FALSE,
                                                       };

static const hci_bool_t g_sign_attr[HCI_L_KEY_MAX] = {HCI_TRUE,   /* Sign with boot key is allowed */
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_TRUE,   /* Sign with nppr sig is allowed */
                                                      HCI_FALSE,
                                                      HCI_TRUE,   /* Sign with lopr sig is allowed */
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                      HCI_FALSE,
                                                     };

static const hci_bool_t g_key_enc_attr[HCI_L_KEY_MAX] = {HCI_FALSE,
                                                         HCI_TRUE,  /* Key enc with OPPR PRV allowed */
                                                         HCI_TRUE,  /* Key enc with OPPR SEC allowed */
                                                         HCI_FALSE,
                                                         HCI_FALSE,
                                                         HCI_FALSE,
                                                         HCI_FALSE,
                                                         HCI_FALSE,
                                                         HCI_TRUE,  /* Broadcast key */
                                                         HCI_FALSE,
                                                         HCI_TRUE,  /* Key enc with OPPU allowed */
                                                         HCI_TRUE,
                                                         HCI_FALSE
                                                        }; /* Key enc with generated key is allowed */

static binstr_t *g_octpers = NULL;    /**< Cached copy of the oct personality*/


static uint32_t
l_CalculateHMAC(hci_key_ref_t   in_ref,
                hci_dig_algo_t  in_dig_algo,
                binstr_t       *in_data,
                binstr_t      **out_hmac)
{
    uint32_t          ret       = ERR_HCI_OK;
    binstr_t         *hmac      = NULL;
    hci_l_key_ref_t  *ref       = in_ref;
    binstr_t         *tmp_key   = NULL;

    MSGENT(("l_CalculateHMAC"));

    if ( (in_data       == NULL)   ||
         (ref           == NULL)   ||
         (out_hmac      == NULL)   ||
         (HCI_L_KEY_TYPE_CONTENT   != ref->key_type &&
          HCI_L_KEY_TYPE_GENERATED != ref->key_type &&
          HCI_L_KEY_TYPE_SAC != ref->key_type ) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    /* Unscramble key_data here */
    RET(hci_l_Unscramble(ref->key_data, &tmp_key));

    /* forsee largest size possible*/
    RET( hci_l_mem_BinstrAlloc(HCI_L_SHA256_DIGESTSIZE, &hmac) );

    RET(hci_l_cryptoCalculateHmac(in_dig_algo,
                                  binstr_Len(tmp_key),
                                  binstr_Buf(tmp_key),
                                  binstr_Len(in_data),
                                  binstr_Buf(in_data),
                                  &binstr_Len(hmac),
                                  binstr_Buf(hmac)));

    *out_hmac = hmac;
    hmac = NULL;
EXIT:
    hci_l_mem_binstrFree(hmac);
    hci_l_FreeKey(tmp_key);
    MSGEXT(("l_CalculateHMAC"));
    return ret;
}

#if 1 == HCI_L_API_HCI_LOADKEY
static uint32_t
l_GetKeyType(hci_load_key_type_t  in_type,
             hci_l_key_type_t    *out_type)
{
    uint32_t          ret = ERR_HCI_OK;
    hci_l_key_type_t  key_type;

    MSGENT(("l_GetKeyType"));

    switch (in_type) {
    case HCI_LOAD_KEY_TYPE_OCTPUB:
        key_type = HCI_L_KEY_TYPE_OPPU;
        break;
    case HCI_LOAD_KEY_TYPE_BROADCASTKEY:
        key_type = HCI_L_KEY_TYPE_BROADCAST;
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_type     = key_type;

EXIT:
    MSGEXT(("l_GetKeyType"));
    return ret;
}
#endif

#if 1 == HCI_L_API_HCI_LOADKEY
/* Key_data format for rsa public key is:
 *    length of e (2 Bytes) + e + length of n (2 Bytes) + n
 */
static uint32_t
l_SerializeKeyDataRsaEN(uint32_t      in_pub_e_len,
                        uint8_t      *in_pub_e,
                        uint32_t      in_pub_n_len,
                        uint8_t      *in_pub_n,
                        binstr_t    **out_data)
{
    uint32_t    ret   = ERR_HCI_OK;
    binstr_t    pub_e = { in_pub_e_len, in_pub_e };
    binstr_t    pub_n = { in_pub_n_len, in_pub_n };
    u_int16_t   len   = 0;
    uint8_t    *cur   = NULL;
    binstr_t   *data  = NULL;

    MSGENT(("l_SerializeKeyDataRsaEN"));

    if ((0 == binstr_Check(&pub_e)) ||
        (0 == binstr_Check(&pub_n))) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    len = L_RSA_PUB_E_HEADER_LEN +
          binstr_Len(&pub_e)   +
          L_RSA_PUB_N_HEADER_LEN +
          binstr_Len(&pub_n);

    RET(hci_l_mem_BinstrAlloc(len, &data));
    cur = binstr_Buf(data);

    byte_BE2W(cur, binstr_Len(&pub_e));
    cur = cur + L_RSA_PUB_E_HEADER_LEN;
    mem_Copy(cur, binstr_Buf(&pub_e), binstr_Len(&pub_e));
    cur = cur + binstr_Len(&pub_e);
    byte_BE2W(cur, binstr_Len(&pub_n));
    cur = cur + L_RSA_PUB_N_HEADER_LEN;
    mem_Copy(cur, binstr_Buf(&pub_n), binstr_Len(&pub_n));

    *out_data = data; data = NULL;
EXIT:
    hci_l_mem_binstrFree(data);
    MSGEXT(("l_SerializeKeyDataRsaEN"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_LOADKEY
static uint32_t
l_GetKeyDataAndFormat(hci_l_key_type_t    in_type,
                      hci_key_data_t     *in_key_data,
                      binstr_t          **out_key_data,
                      hci_l_key_format_t *out_format)
{
    uint32_t            ret      = ERR_HCI_OK;
    binstr_t           *key_data = NULL;
    hci_l_key_format_t  format   = HCI_L_KEY_FORMAT_UNSPECIFIED;

    MSGENT(("l_GetKeyDataAndFormat"));

    switch ( in_type ) {
    case HCI_L_KEY_TYPE_BROADCAST: {
        binstr_t key = { in_key_data->raw_key.len,
                         in_key_data->raw_key.buf
                       };

        if ( (HCI_L_AES_KEYSIZE != in_key_data->raw_key.len)                       ||
             (0 != in_key_data->raw_key.len && NULL == in_key_data->raw_key.buf) ||
             (0 == in_key_data->raw_key.len && NULL != in_key_data->raw_key.buf) ) {
            RET( ERR_HCI_ILLEGALARGS );
        }

        RET( hci_l_Unscramble( &key, &key_data ) );

        format = HCI_L_KEY_FORMAT_AES;
    }
    break;

    case HCI_L_KEY_TYPE_OPPU:
        RET( l_SerializeKeyDataRsaEN(in_key_data->pub_key.pub_e_len,
                                     in_key_data->pub_key.pub_e,
                                     in_key_data->pub_key.pub_n_len,
                                     in_key_data->pub_key.pub_n,
                                     &key_data) );
        format = HCI_L_KEY_FORMAT_RSA;
        break;

    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_format   = format;
    *out_key_data = key_data; key_data = NULL;
EXIT:
    hci_l_mem_binstrFree(key_data);
    MSGEXT(("l_GetKeyDataAndFormat"));
    return ret;
}
#endif

#if 1 == HCI_L_API_HCI_GENERATECONTENTKEY
static uint32_t
l_GenerateKey(hci_l_key_algo_type_t   in_key_algo,
              uint32_t                in_key_sz,
              binstr_t               *in_pv,
              binstr_t              **out_key_data)
{
    uint32_t    ret     = ERR_HCI_OK;
    binstr_t   *keydata = NULL;

    MSGENT(("l_GenerateKey"));

    switch ( in_key_algo ) {
    case HCI_L_KEY_ALGO_TYPE_AES: {
        if ( NULL == in_pv ) {
            /* if in_pv == NULL, then key size should be 16 bytes*/
            if ( HCI_L_AES_KEYSIZE != in_key_sz ) {
                RET( ERR_HCI_ILLEGALARGS );
            }

            RET(hci_l_mem_BinstrAlloc(HCI_L_AES_KEYSIZE, &keydata));
            RET(hci_l_cryptoGenerateRandom(HCI_L_AES_KEYSIZE, binstr_Buf(keydata)));
        } else {  /* Generate key with application provided data */
            if ( (0 == binstr_Check(in_pv)) || (HCI_L_AES_KEYSIZE != binstr_Len(in_pv)) ) {
                RET( ERR_HCI_ILLEGALARGS );
            }

            RET(hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_pv), binstr_Buf(in_pv), &keydata));
        }
    }
    break;
    case HCI_L_KEY_ALGO_TYPE_RSA:
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_key_data = keydata;
    keydata       = NULL;

EXIT:
    hci_l_FreeKey(keydata);
    MSGEXT(("l_GenerateKey"));
    return ret;
}
#endif

uint32_t
hci_l_CheckKeyAttr(hci_l_key_type_t  in_key_type,
                   hci_l_attr_t      in_attr,
                   hci_bool_t       *out_usable)
{
    uint32_t    ret     = ERR_HCI_OK;
    hci_bool_t  usable  = HCI_FALSE;

    MSGENT(("hci_l_CheckKeyAttr"));

    if ( in_key_type >= HCI_L_KEY_MAX ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_attr) {

    case HCI_L_ATTR_MOVE:
        usable = g_move_attr[in_key_type];
        break;

    case HCI_L_ATTR_EXPORT:
        usable = g_export_attr[in_key_type];
        break;

    case HCI_L_ATTR_SIGN:
        usable = g_sign_attr[in_key_type];
        break;

    case HCI_L_ATTR_ENC_KEY:
        usable = g_key_enc_attr[in_key_type];
        break;

    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_usable = usable;

EXIT:
    MSGEXT(("hci_l_CheckKeyAttr"));
    return ret;
}

void
hci_l_ClearCachedOctPersonality()
{
    hci_l_mem_binstrFree(g_octpers);
    g_octpers = NULL;
}

uint32_t
hci_l_ConvSSMemErr(retcode_t in_ret)
{
    uint32_t hci_ret;

    switch ( in_ret ) {
    case ERR_OK:
        hci_ret = ERR_HCI_OK;
        break;
    case ERR_CORRUPTED:
        hci_ret = ERR_HCI_ILLEGALPERSONALITY;
        break;
    case ERR_NOMEM:
        hci_ret = ERR_HCI_NOMEM;
        break;
    case ERR_NG:
        hci_ret = ERR_HCI_NOPERSONALITYREGISTERED;
        break;
    case ERR_ILLEGALARGS:
    case ERR_INTERNALERROR:
    default:
        hci_ret = ERR_HCI_INTERNALERROR;
        break;
    }

    return hci_ret;
}


uint32_t
hci_l_GetOctPersonality(hci_l_pers_type_t   in_type,
                        char              **out_keyid,
                        binstr_t          **out_keydata)
{
    uint32_t   ret      = ERR_HCI_OK;
    char      *keyid    = NULL;
    binstr_t  *keydata  = NULL;

    MSGENT(("hci_l_GetOctPersonality"));
/*
    if ( NULL == g_octpers ) {
        RETR( ssmem_GetObject(SSMEM_OBJTYPE_OPPR, 0, &g_octpers),
              hci_l_ConvSSMemErr(ret) );
    }
*/
    if ( NULL == g_octpers ) {
        RETR(hci_l_PersonalityKeyRetrieve(HCI_PERSONALITY_TYPE_DEVICE_OPPR, &g_octpers), ret );
    }

    RET( hci_l_ParseOctopusPrivatePersonality(g_octpers, in_type, &keyid, &keydata) );

    if (out_keyid) {
        *out_keyid = keyid; keyid = NULL;
    }
    if (out_keydata) {
        *out_keydata = keydata; keydata = NULL;
    }

EXIT:
    hci_l_mem_Free(keyid);
    hci_l_FreeKey(keydata);
    MSGEXT(("hci_l_GetOctPersonality"));
    return ret;
}


/*
 * Common functionality for hash generation and checking, used by hci_CheckHash
 *                                                            and hci_GenerateHash
 */
uint32_t
hci_l_CalculateHash(hci_key_ref_t   in_ref,
                    hci_dig_algo_t  in_dig_algo,
                    binstr_t       *in_data,
                    binstr_t      **out_hash)
{
    uint32_t         ret                       = ERR_HCI_OK;
    hci_l_key_ref_t *ref                       = in_ref;
    uint8_t         *buf                       = NULL;
    uint8_t          digest[HCI_L_SHA256_DIGESTSIZE];
    binstr_t        *tmp_key                   = NULL;
    uint32_t         buflen                    = 0;
    uint32_t         diglen                    = HCI_L_SHA256_DIGESTSIZE;
    binstr_t        *hash                      = NULL;
    
    MSGENT(("hci_l_CalculateHash"));

    if ( (ref              == NULL)  ||
         (out_hash         == NULL)  ||
         ((HCI_L_KEY_TYPE_CONTENT   != ref->key_type) &&
          (HCI_L_KEY_TYPE_GENERATED != ref->key_type) &&
          (HCI_L_KEY_TYPE_MESSAGE   != ref->key_type)) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    /* Unscramble key_data here */
    RET( hci_l_Unscramble(ref->key_data, &tmp_key));

    if (in_data != NULL) {
        RET(hci_l_mem_Alloc((binstr_Len(in_data) + binstr_Len(tmp_key)), &buf));
        mem_Copy(buf, binstr_Buf(in_data), binstr_Len(in_data));
        mem_Copy(buf + binstr_Len(in_data), binstr_Buf(tmp_key), binstr_Len(tmp_key));
        buflen = binstr_Len(in_data) + binstr_Len(tmp_key);
    } else {
        buf = binstr_Buf(tmp_key);
        buflen = binstr_Len(tmp_key);
    }

    RET(hci_l_cryptoMakeDigest(in_dig_algo, buflen, buf, &diglen, digest));

    RET(hci_l_mem_BinstrAllocWithCopy(diglen, digest, &hash));
    *out_hash = hash;
    hash      = NULL;

EXIT:
    if (in_data != NULL && buf != NULL) {
        hci_l_DataScrub(buf + binstr_Len(in_data), binstr_Len(tmp_key));
        hci_l_mem_Free(buf);
    }
    hci_l_FreeKey(tmp_key);
    hci_l_mem_binstrFree(hash);
    MSGEXT(("hci_l_CalculateHash"));
    return ret;
}


/* -------------------------------------------------------------------------- */
/* PUBLIC INTERFACE */
/* -------------------------------------------------------------------------- */

uint32_t
hci_SignData(hci_key_ref_t       in_ref,
             hci_sig_algo_t      in_sig_algo,
             uint32_t            in_data_len,
             uint8_t            *in_data,
             uint32_t           *out_sig_data_len,
             uint8_t           **out_sig_data)
{
    uint32_t          ret      = ERR_HCI_OK;
    hci_l_key_ref_t  *ref      = in_ref;
    uint8_t          *sig_buf  = NULL;
    uint32_t          sig_len  = HCI_L_RSA_SIGSIZEMAX;
    binstr_t         *tmp_key  = NULL;
    hci_bool_t        usable   = HCI_FALSE;

    MSGENT(("hci_SignData"));

    RET( hci_l_IsHciLocked() );

    if ( (in_ref           == NULL)   ||
         ((in_data == NULL) && (in_data_len != 0))  ||
         ((in_data != NULL) && (in_data_len == 0))  ||
         (out_sig_data_len == NULL)   ||
         (out_sig_data     == NULL) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CheckKeyAttr(ref->key_type, HCI_L_ATTR_SIGN, &usable) );
    if ( HCI_TRUE != usable ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_Unscramble(ref->key_data, &tmp_key) );

    RET( hci_l_mem_Alloc(sig_len, &sig_buf) );

    if ( in_sig_algo == HCI_SIG_ALGO_RSA_PKCS1_V1_5_SHA256 ) {

        ret = hci_l_cryptoSignRsassaPkcs1V1_5(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                              HCI_DIG_ALGO_SHA256,
                                              in_data_len, in_data,
                                              &sig_len, sig_buf);

    } else if ( in_sig_algo == HCI_SIG_ALGO_RSA_PKCS1_V1_5_SHA1 ) {

        ret = hci_l_cryptoSignRsassaPkcs1V1_5(binstr_Len(tmp_key), binstr_Buf(tmp_key),
                                              HCI_DIG_ALGO_SHA1,
                                              in_data_len, in_data,
                                              &sig_len, sig_buf);

    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (ERR_HCI_TOOSMALLBUFFER == ret) {
        RET(ERR_HCI_INTERNALERROR);
    }
    RET(ret);

    *out_sig_data = sig_buf; sig_buf = NULL;
    *out_sig_data_len = sig_len;

EXIT:
    hci_l_mem_Free(sig_buf);
    hci_l_FreeKey(tmp_key);
    MSGEXT(("hci_SignData"));
    return ret;
}


uint32_t
hci_CheckHMAC(hci_key_ref_t   in_ref,
              hci_dig_algo_t  in_dig_algo,
              uint32_t        in_data_len,
              uint8_t        *in_data,
              uint32_t        in_expected_hmac_len,
              uint8_t        *in_expected_hmac,
              hci_bool_t     *out_result)
{
    uint32_t    ret           = ERR_HCI_OK;
    binstr_t    data          = { in_data_len, in_data };
    binstr_t    expected_hmac = { in_expected_hmac_len, in_expected_hmac };
    binstr_t   *hmac          = NULL;

    MSGENT(("hci_CheckHMAC"));

    RET( hci_l_IsHciLocked() );

    if ( (0 == binstr_Check(&expected_hmac))   ||
         (NULL == out_result)                  ||
         (in_data == NULL && in_data_len != 0) ||
         (in_data != NULL && in_data_len == 0) ||
         (in_ref  == NULL)                     ||
         (NULL == out_result) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_CalculateHMAC(in_ref, in_dig_algo, &data, &hmac) );

    *out_result = (0 == binstr_Cmp(&expected_hmac, hmac)) ? HCI_TRUE : HCI_FALSE;

EXIT:
    hci_l_mem_binstrFree(hmac);
    MSGEXT(("hci_CheckHMAC"));
    return ret;
}


#if 1 == HCI_L_API_HCI_CHECKHASH
uint32_t
hci_CheckHash(hci_key_ref_t   in_ref,
              hci_dig_algo_t  in_dig_algo,
              uint32_t        in_data_len,
              uint8_t        *in_data,
              uint32_t        in_expected_hmac_len,
              uint8_t        *in_expected_hmac,
              hci_bool_t     *out_result)
{
    uint32_t    ret           = ERR_HCI_OK;
    binstr_t    data          = { in_data_len, in_data};
    binstr_t    expected_hash = { in_expected_hmac_len, in_expected_hmac};
    binstr_t    *hash         = NULL;
    int i;
    
    MSGENT(("hci_CheckHash"));

    RET( hci_l_IsHciLocked() );

    if ( (0 == binstr_Check(&expected_hash))   ||
         (in_data == NULL && in_data_len != 0) ||
         (in_data != NULL && in_data_len == 0) ||
         (NULL == out_result) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CalculateHash(in_ref, in_dig_algo, &data, &hash) );

    *out_result = (0 == binstr_Cmp(&expected_hash, hash)) ? HCI_TRUE : HCI_FALSE;

EXIT:
    hci_l_mem_binstrFree(hash);
    MSGEXT(("hci_CheckHash"));
    return ret;
}
#endif


uint32_t
hci_GetDevID(hci_key_type_t   in_key_type,
             char           **out_id)
{
    uint32_t  ret = ERR_HCI_OK;
    char     *id  = NULL;

    MSGENT(("hci_GetDevID"));

    RET( hci_l_IsHciLocked() );

    if (out_id == NULL) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( in_key_type == HCI_KEY_TYPE_OPPR_PRV ) {
        RET( hci_l_GetOctPersonality(L_PERS_TYPE_OPPR_PRV, &id, NULL) );
    } else if ( in_key_type == HCI_KEY_TYPE_OPPR_SEC ) {
        RET( hci_l_GetOctPersonality(L_PERS_TYPE_OPPR_SEC, &id, NULL) );
    } else {
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_id = id; id = NULL;

EXIT:
    hci_l_mem_Free(id);
    MSGEXT(("hci_GetDevID"));
    return ret;
}

#if 1 == HCI_L_API_HCI_GENERATECONTENTKEY
uint32_t
hci_GenerateContentKey(uint32_t        in_key_sz,
                       uint32_t        in_private_data_len,
                       uint8_t        *in_private_data,
                       hci_key_ref_t  *out_ref)
{
    uint32_t          ret              = ERR_HCI_OK;
    hci_l_key_ref_t  *key_ref          = NULL;
    binstr_t         *keydata          = NULL;
    binstr_t          private_data     = { in_private_data_len, in_private_data };
    binstr_t         *private_data_ptr = &private_data;

    MSGENT(("hci_GenerateContentKey"));

    RET( hci_l_IsHciLocked() );

    if ( out_ref == NULL ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( 0 == in_private_data_len && NULL == in_private_data ) {
        private_data_ptr = NULL;
    } else if ( 0 == binstr_Check(private_data_ptr) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }
    RET( l_GenerateKey(HCI_L_KEY_ALGO_TYPE_AES,
                       in_key_sz,
                       private_data_ptr,
                       &keydata) );

    RET( hci_l_Scramble(keydata) );

    RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_GENERATED,
                                  &key_ref) );

    key_ref->format   = HCI_L_KEY_FORMAT_AES;
    key_ref->key_data = keydata;
    keydata           = NULL;

    *out_ref  = key_ref;
    key_ref   = NULL;

EXIT:
    hci_l_FreeKeyRef(key_ref);
    hci_l_FreeKey(keydata);
    MSGEXT(("hci_GenerateContentKey"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_LOADKEY
uint32_t
hci_LoadKey(hci_load_key_type_t  in_type,
            hci_key_data_t      *in_key_data,
            hci_key_ref_t       *out_ref)
{
    uint32_t            ret      = ERR_HCI_OK;
    hci_l_key_ref_t    *ref      = NULL;
    binstr_t           *keydata  = NULL;
    hci_l_key_format_t  format   = HCI_L_KEY_FORMAT_UNSPECIFIED;
    hci_l_key_type_t    key_type;

    MSGENT(("hci_LoadKey"));

    RET( hci_l_IsHciLocked() );

    if ( (NULL == out_ref) || (NULL == in_key_data) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_GetKeyType(in_type, &key_type) );

    RET( l_GetKeyDataAndFormat(key_type, in_key_data, &keydata, &format) );

    RET( hci_l_Scramble(keydata) );

    RET( hci_l_CreateKeyRefStruct(key_type, &ref) );

    ref->format      = format;
    ref->key_data    = keydata; keydata = NULL;

    *out_ref = (void *) ref; ref = NULL;
EXIT:
    hci_l_FreeKeyRef(ref);
    hci_l_FreeKey(keydata);
    MSGEXT(("hci_LoadKey"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_GENERATEHMAC
uint32_t
hci_GenerateHMAC(hci_key_ref_t   in_ref,
                 hci_dig_algo_t  in_dig_algo,
                 uint32_t        in_data_len,
                 uint8_t        *in_data,
                 uint32_t       *out_hmac_len,
                 uint8_t       **out_hmac)
{
    uint32_t   ret      = ERR_HCI_OK;
    binstr_t   data     = { in_data_len, in_data};
    uint8_t   *hmac_buf = NULL;
    binstr_t  *hmac     = NULL;

    MSGENT(("hci_GenerateHMAC"));

    RET( hci_l_IsHciLocked() );

    if ( (in_data == NULL && in_data_len != 0)   ||
         (in_data != NULL && in_data_len == 0)   ||
         (in_ref  == NULL)                       ||
         (out_hmac_len == NULL)                  ||
         (out_hmac == NULL ) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( l_CalculateHMAC(in_ref, in_dig_algo, &data, &hmac) );
    RET( hci_l_mem_AllocWithCopy(binstr_Len(hmac), binstr_Buf(hmac), &hmac_buf) );
    *out_hmac_len = binstr_Len(hmac);
    *out_hmac = hmac_buf; hmac_buf = NULL;

EXIT:
    hci_l_mem_Free(hmac_buf);
    hci_l_mem_binstrFree(hmac);
    MSGEXT(("hci_GenerateHMAC"));
    return ret;
}
#endif

