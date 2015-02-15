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
#include "binstr_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"


#define L_MS3_CONTENT_ID_SIZE     20
#define L_MS3_SAS_MINIMUM_SIZE    21


#if 1 == HCI_L_API_HCI_IMPORTSAS || 1 == HCI_L_API_HCI_FREESASKEYS
/**
 * @brief Deallocate the given SAS keys structure.
 *
 * @param[in,out] io_sas_keys              keys structure to deallocate
 */
static uint32_t
l_FreeSASKeys(hci_sas_keys_t    *io_sas_keys)
{
    uint32_t ret = ERR_HCI_OK;

    uint32_t  idx = 0;

    MSGENT(("l_FreeSASKeys"));

    if ( io_sas_keys != NULL ) {
        if ( io_sas_keys->contentids != NULL ) {
            for ( idx = 0; idx < io_sas_keys->key_count; idx++ ) {
                hci_l_mem_Free(io_sas_keys->contentids[idx]);
            }
            hci_l_mem_Free(io_sas_keys->contentids);
        }
        if ( io_sas_keys->key_refs != NULL ) {
            for ( idx = 0; idx < io_sas_keys->key_count; idx++ ) {
                hci_l_FreeKeyRef((hci_l_key_ref_t *)io_sas_keys->key_refs[idx]);
            }
            hci_l_mem_Free(io_sas_keys->key_refs);
        }
        hci_l_mem_Free(io_sas_keys);
        io_sas_keys = NULL;
    }

    MSGEXT(("l_FreeSASKeys"));
    return ret;
}
#endif


#if 1 == HCI_L_API_HCI_IMPORTSAS
static uint32_t
l_ImportSAS(uint32_t         in_sas_len,
            uint8_t         *in_sas,
            uint32_t         in_private_data_len,
            uint8_t         *in_private_data,
            uint32_t        *out_public_sas_len,
            uint8_t        **out_public_sas,
            hci_sas_keys_t **out_sas_keys)
{
    uint32_t           ret            = ERR_HCI_OK;
    uint32_t           key_count      = 0;
    uint32_t           idx            = 0;
    uint32_t           sz             = 0;
    hci_sas_keys_t    *sas_keys       = NULL;
    hci_l_key_ref_t  **key_refs       = NULL;
    uint8_t           *cur            = NULL;
    uint8_t          **contentids     = NULL;
    uint32_t           public_sas_len;
    uint8_t           *public_sas_buf = NULL;
    binstr_t          *keydata        = NULL;

    MSGENT(("l_ImportSAS"));

    cur = in_sas;
    sz  = in_sas_len;

    /*read key count*/
    key_count = byte_BE4R(cur);

    cur += sizeof key_count;
    sz -= sizeof key_count;

    if ( key_count > 0 ) {
        RET(hci_l_mem_AllocWithZeros((sizeof(uint8_t *) * key_count), (uint8_t **)&contentids));
        RET(hci_l_mem_AllocWithZeros((sizeof(hci_l_key_ref_t *) * key_count), (uint8_t **)&key_refs));

        for ( ; idx < key_count ; idx++ ) {
            /*read content id*/
            if ( sz < L_MS3_CONTENT_ID_SIZE ) {
                RET( ERR_HCI_ILLEGALSAS );
            }

            RET(hci_l_mem_AllocWithCopy(L_MS3_CONTENT_ID_SIZE, cur, (uint8_t **)&contentids[idx]));
            cur += L_MS3_CONTENT_ID_SIZE;
            sz  -= L_MS3_CONTENT_ID_SIZE;

            /*read key data*/
            if ( sz < HCI_L_AES_KEYSIZE ) {
                RET( ERR_HCI_ILLEGALSAS );
            }

            RET(hci_l_CreateKeyRefStruct(HCI_L_KEY_TYPE_CONTENT,
                                         &key_refs[idx]));

            RET(hci_l_mem_BinstrAllocWithCopy(HCI_L_AES_KEYSIZE, cur, &keydata));
            RET(hci_l_GetKeyFormat(keydata, &(key_refs[idx]->format)));
            RET(hci_l_Scramble(keydata));

            key_refs[idx]->key_data = keydata;
            keydata = NULL;

            cur += HCI_L_AES_KEYSIZE;
            sz -= HCI_L_AES_KEYSIZE;
        }
    }

    if ( sz == 0 ) {
        RET( ERR_HCI_ILLEGALSAS );
    }

    /*send back the rest of sas */
    public_sas_len = sz;

    RET(hci_l_mem_AllocWithCopy(public_sas_len, cur, &public_sas_buf));
    RET(hci_l_mem_Alloc(sizeof(hci_sas_keys_t), (uint8_t **)(&sas_keys)));

    sas_keys->key_count  = key_count;
    sas_keys->contentids = contentids;
    sas_keys->key_refs   = (hci_key_ref_t **)key_refs;

    contentids = NULL;
    key_refs = NULL;

    *out_sas_keys = sas_keys;
    sas_keys = NULL;

    *out_public_sas_len = public_sas_len;
    *out_public_sas     = public_sas_buf; public_sas_buf = NULL;

EXIT:
    hci_l_mem_Free(public_sas_buf);
    hci_l_FreeKey(keydata);
    if ( contentids != NULL ) {
        for (idx = 0; idx < key_count; idx++) {
            hci_l_mem_Free(contentids[idx]);
        }
        hci_l_mem_Free(contentids);
    }
    if ( key_refs != NULL ) {
        for ( idx = 0; idx < key_count; idx++ ) {
            hci_l_FreeKeyRef(key_refs[idx]);
        }
        hci_l_mem_Free(key_refs);
    }
    /*Free sas keys*/
    l_FreeSASKeys(sas_keys);
    MSGEXT(("l_ImportSAS"));
    return ret;
}

uint32_t
hci_ImportSAS(uint32_t         in_sas_len,
              uint8_t         *in_sas,
              uint32_t         in_private_data_len,
              uint8_t         *in_private_data,
              uint32_t        *out_public_sas_len,
              uint8_t        **out_public_sas,
              hci_sas_keys_t **out_sas_keys)
{
    uint32_t  ret = ERR_HCI_OK;
    binstr_t  sas = { in_sas_len, in_sas };

    MSGENT(("hci_ImportSAS"));

    RET( hci_l_IsHciLocked() );

    if ( (in_private_data == NULL && in_private_data_len != 0) ||
         (in_private_data != NULL && in_private_data_len == 0) ||
         (0 == binstr_Check(&sas))                             ||
         (NULL == out_public_sas_len)                          ||
         (NULL == out_public_sas)                              ||
         (NULL == out_sas_keys) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( in_sas_len < L_MS3_SAS_MINIMUM_SIZE ) {
        RET( ERR_HCI_ILLEGALSAS );
    }

    RET( l_ImportSAS(in_sas_len,
                     in_sas,
                     in_private_data_len,
                     in_private_data,
                     out_public_sas_len,
                     out_public_sas,
                     out_sas_keys) );

EXIT:
    MSGEXT(("hci_ImportSAS"));
    return ret;
}
#endif /* #if 1 == HCI_L_API_HCI_IMPORTSAS*/


#if 1 == HCI_L_API_HCI_FREESASKEYS
uint32_t
hci_FreeSASKeys(hci_sas_keys_t *io_sas_keys)
{
    uint32_t        ret      = ERR_HCI_OK;
    hci_sas_keys_t *sas_keys = io_sas_keys;

    MSGENT(("hci_FreeSASKeys"));

    RET( hci_l_IsHciLocked() );

    l_FreeSASKeys(sas_keys);

EXIT:
    MSGEXT(("hci_FreeSASKeys"));
    return ret;
}
#endif

