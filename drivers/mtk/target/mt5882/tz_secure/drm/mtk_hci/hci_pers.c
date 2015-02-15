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
#include "byte_api.h"
#include "binstr_api.h"
/* sec/marlin */
//#include "ssmem_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"

#include "tzst.h" 
/*
 * Platform specific functions for loading and parsing device personality
 * keys. This could be replace with a platform specific implementation. For
 * example if the keys were loaded into the hw at factory time
 */

#define HCI_PERSONALITY_VERSION 1

static binstr_t *gNPPR = NULL;
static binstr_t *gOPPR = NULL;
static binstr_t *gLOPR = NULL;
static binstr_t *gINPR = NULL;
static binstr_t *gROPR = NULL;

typedef struct pers_box1 {
    u_int16_t   offset;
    u_int16_t   len[1];
} pers_box1_t;

typedef struct pers_box2 {
    u_int16_t   offset;
    u_int16_t   len[2];
    uint8_t     reserved[2];
} pers_box2_t;

typedef struct pers_box3 {
    u_int16_t   offset;
    u_int16_t   len[3];
} pers_box3_t;

typedef struct pers_box4 {
    u_int16_t   offset;
    u_int16_t   len[4];
    uint8_t     reserved[2];
} pers_box4_t;

typedef struct nemo_priv {
    uint8_t     version;
    uint8_t     reserved[3];
    pers_box2_t  pkcs8;      /* 0:pkcs#8 for sig, 1:pkcs#8 for enc */
} nemo_priv_t;

typedef struct oct_priv {
    uint8_t     version;
    uint8_t     reserved[3];

    pers_box2_t  scuba;      /* 0:uid, 1:subject */
    pers_box2_t  seckey;     /* 0:uid, 1:keydata */
    pers_box3_t  privkey;    /* 0:pairid, 1:uid, 2:keydata */
    pers_box2_t  torpedo;    /* 0:uid, 1:subject */
    pers_box4_t  starfish;   /* 0:uid, 1:source, 2: algorithm 3: keydata*/
} oct_priv_t;

static uint32_t
l_CheckPersonalityKey(hci_pers_key_type_t in_type)
{
    uint32_t ret = ERR_HCI_OK;
    int32_t  type;
    binstr_t  *p_object = NULL;

    MSGENT(("l_CheckPersonalityKey"));

    switch (in_type) {
    case HCI_PERSONALITY_TYPE_DEVICE_NPPR:
        p_object = gNPPR;
        break;
    case HCI_PERSONALITY_TYPE_DEVICE_OPPR:
        p_object = gOPPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_LOPR:
        p_object = gLOPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_INPR:
        p_object = gINPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_ROPR:
        p_object = gROPR;
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

//    RETR( ssmem_GetObject(type, 0, NULL), hci_l_ConvSSMemErr(ret) );
    if (p_object) {
    }
    else {
        RET(ERR_HCI_NOPERSONALITYREGISTERED);
    }
    
EXIT:
    MSGEXT(("l_CheckPersonalityKey"));
    return ret;
}

uint32_t
hci_l_ParseNEMOPrivateKeys(
    binstr_t    *in_bs,
    binstr_t   **out_sigkey,
    binstr_t   **out_enckey)
{
    uint32_t        ret       = ERR_HCI_OK;
    nemo_priv_t     privh;
    uint32_t        privh_len = 12;
    uint8_t        *body      = NULL;
    uint8_t        *ptr       = NULL;
    binstr_t       *sigkey    = NULL;
    binstr_t       *enckey    = NULL;

    MSGENT(("hci_l_ParseNEMOPrivateKeys"));

    if ( !in_bs || !out_sigkey || !out_enckey ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* NEMO Private keys */
    if ( binstr_Len(in_bs) < privh_len ) {
        Printf("11111111111\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }

    ptr = binstr_Buf(in_bs);

    privh.version = byte_BE1R(ptr);
    ptr += 1;
    ptr += 3; /* Unread */

    privh.pkcs8.offset = byte_BE2R(ptr);
    ptr += 2;

    privh.pkcs8.len[0] = byte_BE2R(ptr);
    ptr += 2;

    privh.pkcs8.len[1] = byte_BE2R(ptr);
    ptr += 2;
    ptr += 2; /* Unread */

    body = ptr;

    if ( privh_len + privh.pkcs8.offset + privh.pkcs8.len[0] + privh.pkcs8.len[1] != binstr_Len(in_bs) ) {
        Printf("222222222\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }

    /* Signature */
    ptr = body + privh.pkcs8.offset;
    RET(hci_l_mem_BinstrAllocWithCopy(privh.pkcs8.len[0], ptr, &sigkey));

    /* Encryption */
    ptr += privh.pkcs8.len[0];
    RET(hci_l_mem_BinstrAllocWithCopy(privh.pkcs8.len[1], ptr, &enckey));

    *out_sigkey = sigkey; sigkey = NULL;
    *out_enckey = enckey; enckey = NULL;

EXIT:
    hci_l_FreeKey(enckey);
    hci_l_FreeKey(sigkey);
    MSGEXT(("hci_l_ParseNEMOPrivateKeys"));
    return ret;
}

uint32_t
hci_l_ParseOctopusPrivatePersonality(
    binstr_t           *in_bs,
    hci_l_pers_type_t   in_type,
    char              **out_keyid,
    binstr_t          **out_keydata)
{
    uint32_t            ret        = ERR_HCI_OK;
    oct_priv_t          privh;
    uint32_t            privh_len  = 48;
    uint8_t            *body       = NULL;
    uint8_t            *ptr        = NULL;
    pers_box2_t        *seckey     = NULL;
    pers_box3_t        *privkey    = NULL;
    pers_box4_t        *starfish   = NULL;
    binstr_t           *keydata    = NULL;
    char               *keyid      = NULL;
    uint32_t            uid_offset = 0;
    uint32_t            uid_length = 0;
    uint32_t            key_offset = 0;
    uint32_t            key_length = 0;


    MSGENT(("hci_l_ParseOctopusPrivatePersonality"));

    if ( !in_bs || ! binstr_Buf(in_bs) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( binstr_Len(in_bs) < privh_len ) {
        Printf("33333333\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }

    ptr = binstr_Buf(in_bs);

    privh.version         = byte_BE1R(ptr);    ptr += 1;    ptr += 3;
    privh.scuba.offset    = byte_BE2R(ptr);    ptr += 2;
    privh.scuba.len[0]    = byte_BE2R(ptr);    ptr += 2;
    privh.scuba.len[1]    = byte_BE2R(ptr);    ptr += 2;    ptr += 2;
    privh.seckey.offset   = byte_BE2R(ptr);    ptr += 2;
    privh.seckey.len[0]   = byte_BE2R(ptr);    ptr += 2;
    privh.seckey.len[1]   = byte_BE2R(ptr);    ptr += 2;    ptr += 2;
    privh.privkey.offset  = byte_BE2R(ptr);    ptr += 2;
    privh.privkey.len[0]  = byte_BE2R(ptr);    ptr += 2;
    privh.privkey.len[1]  = byte_BE2R(ptr);    ptr += 2;
    privh.privkey.len[2]  = byte_BE2R(ptr);    ptr += 2;
    privh.torpedo.offset  = byte_BE2R(ptr);    ptr += 2;
    privh.torpedo.len[0]  = byte_BE2R(ptr);    ptr += 2;
    privh.torpedo.len[1]  = byte_BE2R(ptr);    ptr += 2;    ptr += 2;
    privh.starfish.offset = byte_BE2R(ptr);    ptr += 2;
    privh.starfish.len[0] = byte_BE2R(ptr);    ptr += 2;
    privh.starfish.len[1] = byte_BE2R(ptr);    ptr += 2;
    privh.starfish.len[2] = byte_BE2R(ptr);    ptr += 2;
    privh.starfish.len[3] = byte_BE2R(ptr);    ptr += 2;    ptr += 2;

    body = ptr;

    if ( privh.version != HCI_PERSONALITY_VERSION ) {
        Printf("44444444\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }

    switch (in_type) {
    case L_PERS_TYPE_OPPR_SEC:
        seckey = &(privh.seckey);
        uid_offset = seckey->offset;
        uid_length = seckey->len[0];
        key_offset = uid_offset + uid_length;
        key_length = seckey->len[1];
        break;
    case L_PERS_TYPE_OPPR_PRV:
        privkey = &(privh.privkey);
        uid_offset = privkey->offset;
        uid_length = privkey->len[0];
        key_offset = uid_offset + uid_length + privkey->len[1];
        key_length = privkey->len[2];
        break;
    case L_PERS_TYPE_STARFISH:
        starfish = &(privh.starfish);
        uid_offset = starfish->offset;
        uid_length = starfish->len[0];
        key_offset = uid_offset + uid_length + starfish->len[1] + starfish->len[2];
        key_length = starfish->len[3];
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* Key id */
    if ( privh_len + uid_offset + uid_length > binstr_Len(in_bs) ) {
        Printf("5555555\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }

    RET(hci_l_mem_AllocWithCopy(uid_length+1, body + uid_offset, (uint8_t **)&keyid));
    keyid[uid_length] = 0;

    /* Key data */
    if ( privh_len + key_offset + key_length > binstr_Len(in_bs) ) {
        Printf("66666666\n");
        RET( ERR_HCI_ILLEGALPERSONALITY );
    }
    RET(hci_l_mem_BinstrAllocWithCopy(key_length, body + key_offset, &keydata));

    *out_keyid = keyid; keyid = NULL;
    *out_keydata = keydata; keydata = NULL;

EXIT:
    hci_l_mem_Free(keyid);
    hci_l_FreeKey(keydata);
    MSGEXT(("hci_l_ParseOctopusPrivatePersonality"));
    return ret;
}

uint32_t
hci_l_LoadPersonalityKey(hci_pers_key_type_t   in_type,
                         uint32_t              in_len,
                         uint8_t              *in_buf)
{
    uint32_t  ret = ERR_HCI_OK;
    int32_t   type;
    binstr_t    bs;
    binstr_t  **pp_object = NULL;

    MSGENT(("hci_l_LoadPersonalityKey"));

    switch (in_type) {
    case HCI_PERSONALITY_TYPE_DEVICE_NPPR:
        pp_object = &gNPPR;
        break;
    case HCI_PERSONALITY_TYPE_DEVICE_OPPR:
        pp_object = &gOPPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_LOPR:
        pp_object = &gLOPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_INPR:
        pp_object = &gINPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_ROPR:
        pp_object = &gROPR;
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
/*
    case HCI_PERSONALITY_KEY_TYPE_NEMOPRIV:
        pp_object = &g_NEMOPRIV;
        break;
    case HCI_PERSONALITY_KEY_TYPE_OCTPRIV:
        pp_object = &g_OCTPRIV;
        break;
    default:
        RET(ERR_ILLEGALARGS);
*/
    }

    /* If in_buf = NULL then clear the personality */
    if (NULL == in_buf) {
        binstr_Free (*pp_object);
        *pp_object = NULL;
    } else {
        binstr_Free (*pp_object);
        *pp_object = NULL;
        
        *pp_object = binstr_Alloc( in_len, in_buf );
        if (NULL == (*pp_object))
        {
            RET(ERR_HCI_NOMEM);
        }
    }
    
    /* If in_buf = NULL then clear the personality */
/*
    if ( NULL == in_buf ) {
        RETR( ssmem_SetObject(type, 0, NULL), hci_l_ConvSSMemErr(ret) );
    } else {
        binstr_t bs = { in_len, in_buf};
        if ( 0 == binstr_Check(&bs) ) {
            RET( ERR_HCI_ILLEGALARGS );
        }
        RETR( ssmem_SetObject(type, 0, &bs), hci_l_ConvSSMemErr(ret) );
    }
*/

EXIT:
    MSGEXT(("hci_l_LoadPersonalityKey"));
    return ret;
}

retcode_t 
hci_l_PersonalityKeyRetrieve(hci_pers_key_type_t in_type,
                             binstr_t          **out_object)
{
    retcode_t  ret = ERR_HCI_OK;
    binstr_t  *p_object = NULL;

    MSGENT(("hci_l_PersonalityKeyRetrieve"));

    switch (in_type) {
    case HCI_PERSONALITY_TYPE_DEVICE_NPPR:
        p_object = gNPPR;
        break;
    case HCI_PERSONALITY_TYPE_DEVICE_OPPR:
        p_object = gOPPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_LOPR:
        p_object = gLOPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_INPR:
        p_object = gINPR;
        break;
    case HCI_PERSONALITY_TYPE_IMPORT_ROPR:
        p_object = gROPR;
        break;
    default:
        RET( ERR_HCI_ILLEGALARGS );
    }

    if (NULL == p_object)
    {
        RET(ERR_HCI_NOPERSONALITYREGISTERED);
    }

    if (out_object)
    {
        if (p_object)
        {
            *out_object = binstr_Alloc(binstr_Len(p_object),
                                       binstr_Buf(p_object));
        }
    }

    if (NULL == *out_object)
    {
        RET(ERR_HCI_NOMEM);
    }

EXIT:
    MSGEXT(("hci_l_PersonalityKeyRetrieve"));
    return ret;
}

uint32_t
hci_LoadPersonalityKey(hci_pers_key_type_t   in_type,
                       uint32_t              in_len,
                       uint8_t              *in_buf)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_LoadPersonalityKey"));

    if ( (in_len == 0 && NULL != in_buf) ||
         (in_len != 0 && NULL == in_buf) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    RET( hci_l_IsHciLocked() );

    RET( hci_l_LoadPersonalityKey(in_type, in_len, in_buf) );

EXIT:
    MSGEXT(("hci_LoadPersonalityKey"));
    return ret;
}

uint32_t
hci_CheckPersonalityKey(hci_pers_key_type_t in_type)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_CheckPersonalityKey"));

    RET( hci_l_IsHciLocked() );

    RET( l_CheckPersonalityKey(in_type) );

EXIT:
    MSGEXT(("hci_CheckPersonalityKey"));
    return ret;
}
