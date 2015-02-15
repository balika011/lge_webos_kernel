/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* sec/include */
#include "cmn_api_sec.h"
/* cmn */
#include "dbg_api.h"
#include "base_api.h"
#include "mem_api.h"
#include "binstr_api.h"
/* platform/sec */
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"

typedef struct l_key_cache_s {
    struct l_key_cache_s    *next;
    hci_imp_key_type_t       key_type;
    hci_enc_algo_t           enc_algo;
    hci_enc_mode_t           enc_mode;
    hci_enc_pad_t            enc_pad;
    binstr_t                *oaep_param;    /* can be NULL */
    binstr_t                *iv;            /* can be NULL */
    binstr_t                *key_data;      /* this should be protected */
    binstr_t                *enc_data;
    binstr_t                *dec_data;      /* this should be protected */
} l_key_cache_t;

/* Global cache */
static l_key_cache_t *l_key_cache = NULL;

static void
l_FreeCacheEntry(l_key_cache_t *io_cache)
{
    l_key_cache_t *cache = io_cache;

    MSGENT(("l_FreeCacheEntry"));

    while ( NULL != cache ) {
        l_key_cache_t *next = cache->next;

        hci_l_mem_binstrFree( cache->oaep_param );
        hci_l_mem_binstrFree( cache->iv );
        hci_l_mem_binstrFree( cache->key_data );
        hci_l_mem_binstrFree( cache->enc_data );
        hci_l_FreeKey( cache->dec_data );
        hci_l_mem_Free( cache );

        cache = next;
    }

    MSGEXT_V(("l_FreeCacheEntry"));
}

static bool_t
l_KeyMatchesCacheEntry(hci_imp_key_type_t   in_key_type,
                       hci_enc_algo_t       in_enc_algo,
                       hci_enc_mode_t       in_enc_mode,
                       hci_enc_pad_t        in_enc_pad,
                       binstr_t            *in_oaep_param,
                       binstr_t            *in_iv,
                       binstr_t            *in_key_data,
                       binstr_t            *in_enc_data,
                       l_key_cache_t       *in_cache)
{
    bool_t match;
    MSGENT(("l_KeyMatchesCacheEntry"));

    match = ((in_cache->key_type == in_key_type) &&
             (in_cache->enc_algo == in_enc_algo) &&
             (in_cache->enc_mode == in_enc_mode) &&
             (in_cache->enc_pad  == in_enc_pad ) &&
             (0 == binstr_Cmp(in_cache->oaep_param, in_oaep_param)) &&
             (0 == binstr_Cmp(in_cache->iv,         in_iv))         &&
             (0 == binstr_Cmp(in_cache->key_data,   in_key_data))   &&
             (0 == binstr_Cmp(in_cache->enc_data,   in_enc_data))) ? TRUE : FALSE;

    MSGEXT_V(("l_KeyMatchesCacheEntry"));
    return match;
}

static uint32_t
l_AddCacheEntry(hci_imp_key_type_t in_key_type,
                hci_enc_algo_t     in_enc_algo,
                hci_enc_mode_t     in_enc_mode,
                hci_enc_pad_t      in_enc_pad,
                binstr_t          *in_oaep_param,
                binstr_t          *in_iv,
                binstr_t          *in_key_data,
                binstr_t          *in_enc_data,
                binstr_t          *in_dec_data)
{
    uint32_t       ret   = ERR_HCI_OK;
    l_key_cache_t *cache = NULL;

    MSGENT(("l_AddCacheEntry"));

    RET( hci_l_mem_AllocWithZeros(sizeof *cache, (uint8_t **)&cache) );

    cache->key_type = in_key_type;
    cache->enc_algo = in_enc_algo;
    cache->enc_mode = in_enc_mode;
    cache->enc_pad  = in_enc_pad;

    if ( NULL != in_oaep_param ) {
        RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_oaep_param),
                                           binstr_Buf(in_oaep_param),
                                           &cache->oaep_param) );
    }

    if ( NULL != in_iv ) {
        RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_iv),
                                           binstr_Buf(in_iv),
                                           &cache->iv) );
    }

    RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_key_data),
                                       binstr_Buf(in_key_data),
                                       &cache->key_data) );

    RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_enc_data),
                                       binstr_Buf(in_enc_data),
                                       &cache->enc_data) );

    RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(in_dec_data),
                                       binstr_Buf(in_dec_data),
                                       &cache->dec_data) );

    cache->next = l_key_cache;
    l_key_cache = cache;

    cache = NULL;

EXIT:
    l_FreeCacheEntry(cache);
    MSGEXT(("l_AddCacheEntry"));
    return ret;
}

/* -------------------------------------------------------------------------- */
/* PUBLIC INTERFACE */
uint32_t
hci_l_AddKeyToCache(hci_imp_key_type_t in_key_type,
                    hci_enc_algo_t     in_enc_algo,
                    hci_enc_mode_t     in_enc_mode,
                    hci_enc_pad_t      in_enc_pad,
                    binstr_t          *in_oaep_param,
                    binstr_t          *in_iv,
                    binstr_t          *in_key_data,
                    binstr_t          *in_enc_data,
                    binstr_t          *in_dec_data)
{
    uint32_t   ret      = ERR_HCI_OK;
    binstr_t  *dec_data = NULL;

    MSGENT(("hci_l_AddKeyToCache"));

    if ( 0 == binstr_Check(in_key_data) &&
         0 == binstr_Check(in_enc_data) &&
         0 == binstr_Check(in_dec_data)  ) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    if ( HCI_IMP_KEY_TYPE_DOMAIN == in_key_type ) {
        RET( hci_l_GetKeyFromCache(in_key_type,
                                   in_enc_algo,
                                   in_enc_mode,
                                   in_enc_pad,
                                   in_oaep_param,
                                   in_iv,
                                   in_key_data,
                                   in_enc_data,
                                   &dec_data) );

        if ( NULL == dec_data ) {
            RET( l_AddCacheEntry(in_key_type,
                                 in_enc_algo,
                                 in_enc_mode,
                                 in_enc_pad,
                                 in_oaep_param,
                                 in_iv,
                                 in_key_data,
                                 in_enc_data,
                                 in_dec_data) );
        }
    }

EXIT:
    hci_l_FreeKey(dec_data);
    MSGEXT(("hci_l_AddKeyToCache"));
    return ret;
}

uint32_t
hci_l_GetKeyFromCache(hci_imp_key_type_t   in_key_type,
                      hci_enc_algo_t       in_enc_algo,
                      hci_enc_mode_t       in_enc_mode,
                      hci_enc_pad_t        in_enc_pad,
                      binstr_t            *in_oaep_param,
                      binstr_t            *in_iv,
                      binstr_t            *in_key_data,
                      binstr_t            *in_enc_data,
                      binstr_t           **out_dec_data)
{
    uint32_t       ret      = ERR_OK;
    binstr_t      *dec_data = NULL;
    l_key_cache_t *cache    = NULL;

    MSGENT(("hci_l_GetKeyFromCache"));

    if ( 0 == binstr_Check(in_key_data) &&
         0 == binstr_Check(in_enc_data) &&
         NULL == out_dec_data ) {
        RET(ERR_HCI_ILLEGALARGS);
    }

    if ( HCI_IMP_KEY_TYPE_DOMAIN == in_key_type ) {
        cache = l_key_cache;

        while (NULL != cache) {
            if ( TRUE == l_KeyMatchesCacheEntry(in_key_type,
                                                in_enc_algo,
                                                in_enc_mode,
                                                in_enc_pad,
                                                in_oaep_param,
                                                in_iv,
                                                in_key_data,
                                                in_enc_data,
                                                cache) ) {
                break;
            }
            cache = cache->next;
        }
    }

    if ( NULL != cache ) {
        RET( hci_l_mem_BinstrAllocWithCopy(binstr_Len(cache->dec_data),
                                           binstr_Buf(cache->dec_data),
                                           &dec_data) );
    }

    *out_dec_data = dec_data;

EXIT:
    MSGEXT(("hci_l_GetKeyFromCache"));
    return ret;
}

uint32_t
hci_l_DestroyCache(void)
{
    MSGENT(("hci_l_DestroyCache"));

    l_FreeCacheEntry( l_key_cache );
    l_key_cache = NULL;

    MSGEXT_V(("hci_l_DestroyCache"));
    return ERR_HCI_OK;
}
