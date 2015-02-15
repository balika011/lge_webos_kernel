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
#include "byte_api.h"
#include "str_api.h"
#include "binstr_api.h"
/* sec/marlin */
//#include "ssmem_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#ifdef NPOPT_USE_NPPERS
#include "hci_bootkey.h"
#endif
#include "hci_sackey.h"
#include "hci_internal_api.h"


#define L_NS_SF_ALGO     "http://marlin-drm.com/starfish/algorithmID/1.0"

/* SAC keys */
#define L_SMID_MRLN                   (byte_4CHARCODE('M','R','L','N'))
#define L_SMID_DTCP                   (byte_4CHARCODE('D','T','C','P'))
#define L_SMID_ARIB                   (byte_4CHARCODE('A','R','I','B'))

static const uint8_t l_sac_key_mrln[HCI_L_AES_KEYSIZE] = L_SAC_KEY_MRLN;
static const uint8_t l_sac_key_dtcp[HCI_L_AES_KEYSIZE] = L_SAC_KEY_DTCP;
static const uint8_t l_sac_key_arib[HCI_L_AES_KEYSIZE] = L_SAC_KEY_ARIB;


static uint32_t
l_InitRSAKey(hci_l_key_ref_t   *io_ref,
             hci_key_type_t     in_key_type)
{
    uint32_t          ret      = ERR_HCI_OK;
    binstr_t         *bs       = NULL;
    binstr_t         *keydata  = NULL;
    binstr_t         *sigkey   = NULL;
    binstr_t         *enckey   = NULL;
    hci_l_key_ref_t  *ref      = io_ref;

    MSGENT(("l_InitRSAKey"));

    if ( ref == NULL ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_key_type) {
    case HCI_KEY_TYPE_OPPR_PRV:
        RET(hci_l_GetOctPersonality(L_PERS_TYPE_OPPR_PRV, NULL, &keydata));
#if !defined(CC_ENABLE_NAUTILUS_ATK) && defined(CC_ENABLE_NAUTILUS_NEW_KEYFORMAT)        
        RET(hci_l_Unscramble2(keydata));
#endif        
        break;
    case HCI_KEY_TYPE_NPPR_ENC:
    case HCI_KEY_TYPE_NPPR_SIG:
        RETR(hci_l_PersonalityKeyRetrieve(HCI_PERSONALITY_TYPE_DEVICE_NPPR, &bs), ret );
/*

    if ( NULL == g_octpers ) {
        RETR( ssmem_GetObject(SSMEM_OBJTYPE_OPPR, 0, &g_octpers),
              hci_l_ConvSSMemErr(ret) );
    }
    if ( NULL == g_octpers ) {
        RETR(hci_l_PersonalityKeyRetrieve(HCI_PERSONALITY_TYPE_DEVICE_OPPR, &g_octpers), ret );
    }


        RETR(ssmem_GetObject(SSMEM_OBJTYPE_NPPR, 0, &bs),
             hci_l_ConvSSMemErr(ret) );
*/
        RET(hci_l_ParseNEMOPrivateKeys(bs, &sigkey, &enckey));

        if (in_key_type == HCI_KEY_TYPE_NPPR_ENC) {
            keydata = enckey; enckey = NULL;
        } else {
            keydata = sigkey; sigkey = NULL;
        }
#if !defined(CC_ENABLE_NAUTILUS_ATK) && defined(CC_ENABLE_NAUTILUS_NEW_KEYFORMAT)        
        RET(hci_l_Unscramble2(keydata));
#endif        
        break;
    case HCI_KEY_TYPE_LOPR_SIG:
        RETR(hci_l_PersonalityKeyRetrieve(HCI_PERSONALITY_TYPE_IMPORT_LOPR, &keydata), ret );
/*
        RETR(ssmem_GetObject(SSMEM_OBJTYPE_LOPR, 0, &keydata),
             hci_l_ConvSSMemErr(ret) );
*/
        break;
#ifdef NPOPT_USE_NPPERS
    case HCI_KEY_TYPE_BOOT: {
        static const uint8_t  bootkey_sig_body [] = L_BOOTKEY_PRIV;

        RET( hci_l_mem_BinstrAllocWithCopy(sizeof bootkey_sig_body,
                                           bootkey_sig_body,
                                           &sigkey) );
        keydata = sigkey; sigkey = NULL;
    }
    break;
#endif
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET(hci_l_Scramble(keydata));

    hci_l_FreeKey(ref->key_data);
    ref->key_data = keydata; keydata = NULL;
    ref->format = HCI_L_KEY_FORMAT_RSA;

EXIT:
    hci_l_mem_binstrFree(bs);
    hci_l_FreeKey(keydata);
    hci_l_FreeKey(sigkey);
    hci_l_FreeKey(enckey);
    MSGEXT(("l_InitRSAKey"));
    return ret;
}


/*-----------------------------------------------------------------------*/
/*
 * @brief Check whether buffer in_s2 of length in_s2_len is comprized into
 * buffer in_s1 of length in_s1_len.
 *
 * @param[in] in_s1      buffer to compare against
 * @param[in] in_s1_len  length of in_s1
 * @param[in] in_s2      buffer to compare
 * @param[in] in_s2_len  length of in_s2
 *
 * @return     Return the immediate offset into in_s1 after the matching
 *             buffer, if in_s2 is comprized into in_s1.
 * @return -1  If the two buffers differ.
 */
/*-----------------------------------------------------------------------*/
static int32_t
l_bstrstr(
    const uint8_t *in_s1,
    uint32_t        in_s1_len,
    const uint8_t *in_s2,
    uint32_t        in_s2_len)
{
    const uint8_t *p1 = in_s1;
    const uint8_t *p2 = in_s2;
    uint32_t  off1 = 0;
    uint32_t  off2 = 0;

    MSGENT(("l_bstrstr"));

    do {
        if ( *(p1+off1) == *(p2+off2) ) {
            off1++;
            off2++;
        } else {
            off1 -= off2 - 1;
            off2 = 0;
        }
    } while ( off1 < in_s1_len && off2 < in_s2_len );

    MSGEXT_V(("l_bstrstr"));

    return (off2==in_s2_len) ? (off1-off2) : -1;
}

/* -------------------------------------------------------------------------- */
/**
 * @brief Check whether a given string is found in the given key/data.
 *
 * @param[in]  in_key           The key to look into
 * @param[in]  in_str_to_check  string to find
 * @param[in]  in_str_len       length of string to find
 * @param[out] out_result       Flag holding the check result. TRUE if the
 *                              given string is found, FALSE otherwise.
 *
 * @note   Assume that the key is unscrambled here.
 *
 * @return ::ERR_HCI_OK         Indicating success.
 **/
/* -------------------------------------------------------------------------- */
static uint32_t
l_CheckStringInKey(binstr_t          *in_key,
                   uint8_t           *in_str_to_check,
                   uint32_t           in_str_len,
                   uint32_t          *out_result)
{
    uint32_t                      ret      = ERR_HCI_OK;
    int                           offset   = 0;

    MSGENT(("l_CheckStringInKey"));

    offset = l_bstrstr(binstr_Buf(in_key), binstr_Len(in_key), in_str_to_check, in_str_len);

    if (offset < 0) {
        *out_result = 0;
    } else {
        *out_result = 1;
    }

    MSGEXT(("l_CheckStringInKey"));
    return ret;
}

/* -------------------------------------------------------------------------- */
/**
 * @brief Check whether a given key is Starfishkey.
 *
 * @param[in]  in_key           The key to examine
 * @param[out] out_result       Flag holding the check result. TRUE if the
 *                              given string is found, FALSE otherwise.
 *
 * @note   Assume that the key is not scrambled here.
 *
 * @return ::ERR_HCI_OK
 * @return ::ERR_HCI_ILLEGALARGS
 * @return ::l_CheckStringInKey
 **/
/* -------------------------------------------------------------------------- */
static uint32_t
l_IsStarfishKey(binstr_t           *in_key,
                uint32_t           *out_result)
{
    uint32_t           ret                 = ERR_HCI_OK;
    uint32_t           result              = 0;
    const char         encryptedData_str[] = "EncryptedData";
    const char         cipherData_str[]    = "CipherData";


    MSGENT(("l_IsStarfishKey"));

    if ( (in_key == NULL) || (out_result == NULL)) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET(l_CheckStringInKey(in_key, (uint8_t *)encryptedData_str, str_Len(encryptedData_str), &result));
    if (result == 0) {
        *out_result = result;
        goto EXIT;
    }

    result = 0;
    RET(l_CheckStringInKey(in_key, (uint8_t *)L_NS_SF_ALGO, str_Len(L_NS_SF_ALGO), &result));
    if (result == 0) {
        *out_result = result;
        goto EXIT;
    }

    result = 0;
    RET(l_CheckStringInKey(in_key, (uint8_t *)cipherData_str, str_Len(cipherData_str), &result));
    *out_result = result;

EXIT:
    MSGEXT(("l_IsStarfishKey"));
    return ret;
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Determine the key format based on its length and its content.
 *
 * @Note: assume the key is not scrambled here.
 *
 * @param[in]  in_key         Key to examine
 * @param[out] out_key_format Key format
 *
 * @return ::ERR_HCI_OK
 * @return ::ERR_HCI_ILLEGALARGS
 * @return ::l_IsStarfishKey
 */
/*---------------------------------------------------------------------------*/
uint32_t
hci_l_GetKeyFormat(binstr_t           *in_key,
                   hci_l_key_format_t *out_key_format)
{
    uint32_t           ret        = ERR_HCI_OK;
    hci_l_key_format_t key_format = HCI_L_KEY_FORMAT_UNSPECIFIED;
    uint32_t           result     = 0;

    MSGENT(("hci_l_GetKeyFormat"));

    if ( (in_key == NULL)         ||
         (out_key_format == NULL) ||
         (binstr_Len(in_key) < HCI_L_AES_KEYSIZE) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (binstr_Len(in_key) == HCI_L_AES_KEYSIZE) {
        key_format = HCI_L_KEY_FORMAT_AES;
    } else {
        RET(l_IsStarfishKey(in_key, &result));

        if (result == 1) {
            key_format = HCI_L_KEY_FORMAT_STARFISH;
        } else {
            key_format = HCI_L_KEY_FORMAT_RSA;
        }
    }

    *out_key_format = key_format;

EXIT:
    MSGEXT(("hci_l_GetKeyFormat"));
    return ret;
}

uint32_t
hci_l_InitAESKey(hci_l_key_ref_t *io_ref, hci_key_type_t in_key_type)
{
    uint32_t         ret        = ERR_HCI_OK;
    hci_l_key_ref_t *ref        = io_ref;
    binstr_t        *keydata    = NULL;

    MSGENT(("hci_l_InitAESKey"));

    if ( ref == NULL ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_key_type) {
    case HCI_KEY_TYPE_OPPR_SEC:
        RET(hci_l_GetOctPersonality(L_PERS_TYPE_OPPR_SEC, NULL, &keydata));
#if !defined(CC_ENABLE_NAUTILUS_ATK) && defined(CC_ENABLE_NAUTILUS_NEW_KEYFORMAT)
        RET(hci_l_Unscramble2(keydata));
#endif        
        break;

    default:
        RET(ERR_HCI_ILLEGALARGS);
    }

    RET(hci_l_Scramble(keydata));

    hci_l_FreeKey(ref->key_data);
    ref->key_data = keydata; keydata = NULL;
    ref->format = HCI_L_KEY_FORMAT_AES;

EXIT:
    hci_l_FreeKey(keydata);
    MSGEXT(("hci_l_InitAESKey"));
    return ret;
}

/**
 * @brief Deallocate the given key reference structure.
 *
 * @param[in] in_ref              key reference to deallocate
 */
uint32_t
hci_l_FreeKeyRef( hci_l_key_ref_t *in_ref)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_l_FreeKeyRef"));

    /* Note that Nautilus calls hci_DestroyKeyRef(NULL) sometimes without acquiring the HCI lock */

    if (in_ref != NULL) {

        RET(hci_l_IsHciLocked());

        if ( in_ref->key_data ) {
            hci_l_FreeKey(in_ref->key_data);
            in_ref->key_data = NULL;
        }

        hci_l_mem_Free(in_ref);
        in_ref = NULL;
    }
EXIT:
    MSGEXT(("hci_l_FreeKeyRef"));
    return ret;
}


/**
 * @brief Create a key reference structure with the given key type.
 * The allocated structure can be deallocated with l_KeyRefClean.
 *
 * @param[in]  in_key_type          the type of key the reference will hold
 * @param[out] out_ref              allocated key reference
 */
uint32_t
hci_l_CreateKeyRefStruct( hci_l_key_type_t     in_key_type,
                          hci_l_key_ref_t    **out_ref )
{
    uint32_t           ret = ERR_HCI_OK;
    hci_l_key_ref_t   *ref = NULL;

    MSGENT(("hci_l_CreateKeyRefStruct"));

    RET(hci_l_mem_AllocWithZeros(sizeof(hci_l_key_ref_t), (uint8_t **)&ref));
    ref->key_data    = NULL;
    ref->format      = HCI_L_KEY_FORMAT_UNSPECIFIED;
    ref->key_type    = in_key_type;

    *out_ref = ref; ref = NULL;
EXIT:
    hci_l_FreeKeyRef(ref);
    MSGEXT(("hci_l_CreateKeyRefStruct"));
    return ret;
}


uint32_t
hci_DestroyKeyRef(hci_key_ref_t in_ref)
{
    uint32_t         ret = ERR_HCI_OK;
    hci_l_key_ref_t *ref = in_ref;

    MSGENT(("hci_DestroyKeyRef"));

    RET( hci_l_FreeKeyRef(ref) );

EXIT:
    MSGEXT(("hci_DestroyKeyRef"));
    return ret;
}


uint32_t
hci_CheckKeyRef(hci_key_ref_t  in_ref,
                hci_bool_t    *out_result)
{
    uint32_t         ret = ERR_HCI_OK;
    hci_l_key_ref_t *ref = in_ref;

    MSGENT(("hci_CheckKeyRef"));

    RET( hci_l_IsHciLocked() );

    if ( NULL == out_result ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( (ref != NULL) && (NULL != ref->key_data) ) {
        *out_result = HCI_TRUE;
    } else {
        *out_result = HCI_FALSE;
    }

EXIT:
    MSGEXT(("hci_CheckKeyRef"));
    return ret;
}


uint32_t
hci_CreateKeyRef(hci_key_type_t     in_key_type,
                 hci_key_ref_t     *out_ref)
{
    uint32_t           ret = ERR_HCI_OK;
    hci_l_key_ref_t   *ref = NULL;

    MSGENT(("hci_CreateKeyRef"));

    RET( hci_l_IsHciLocked() );

    if (out_ref == NULL) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    switch (in_key_type) {
    case HCI_KEY_TYPE_OPPR_SEC:
        RET(hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_OPPR_SEC, &ref) );
        RET( hci_l_InitAESKey(ref, in_key_type) );
        break;

    case HCI_KEY_TYPE_OPPR_PRV:
        RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_OPPR_PRV, &ref) );
        RET( l_InitRSAKey(ref, in_key_type) );
        break;

    case HCI_KEY_TYPE_NPPR_ENC:
        RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_NPPR_ENC, &ref) );
        RET( l_InitRSAKey(ref, in_key_type) );
        break;

    case HCI_KEY_TYPE_NPPR_SIG:
        RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_NPPR_SIG, &ref) );
        RET( l_InitRSAKey(ref, in_key_type) );
        break;

    case HCI_KEY_TYPE_LOPR_SIG:
        RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_LOPR_SIG, &ref) );
        RET( l_InitRSAKey(ref, in_key_type) );
        break;

    case HCI_KEY_TYPE_BOOT:
        RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_BOOT, &ref) );
        RET( l_InitRSAKey(ref, in_key_type) );
        break;

    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    *out_ref = (void *)ref; ref = NULL;

EXIT:
    hci_l_FreeKeyRef(ref);
    MSGEXT(("hci_CreateKeyRef"));
    return ret;
}

uint32_t
hci_CreateSACKeyRef(uint32_t           in_sac_smid,
                    hci_key_ref_t     *out_ref)
{
    uint32_t            ret         = ERR_HCI_OK;
    hci_l_key_ref_t    *ref         = NULL;
    binstr_t           *keydata     = NULL;

    MSGENT(("hci_CreateSACKeyRef"));

    RET( hci_l_IsHciLocked() );

    if (out_ref == NULL) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_SAC, &ref) );

    switch (in_sac_smid) {
    case L_SMID_MRLN:
        keydata = binstr_Alloc(HCI_L_AES_KEYSIZE, l_sac_key_mrln);
        break;

    case L_SMID_DTCP:
        keydata = binstr_Alloc(HCI_L_AES_KEYSIZE, l_sac_key_dtcp);
        break;

    case L_SMID_ARIB:
        keydata = binstr_Alloc(HCI_L_AES_KEYSIZE, l_sac_key_arib);
        break;

    default:
        RET(ERR_HCI_ILLEGALARGS);
        break;
    }

    if (NULL == keydata) {
        RET(ERR_HCI_NOMEM);
    }

    RET(hci_l_Scramble(keydata));

    ref->key_data = keydata; keydata = NULL;
    ref->format = HCI_L_KEY_FORMAT_AES;

    *out_ref = (void *)ref; ref = NULL;

EXIT:
    hci_l_FreeKey(keydata);
    hci_l_FreeKeyRef(ref);
    MSGEXT(("hci_CreateSACKeyRef"));
    return ret;
}
