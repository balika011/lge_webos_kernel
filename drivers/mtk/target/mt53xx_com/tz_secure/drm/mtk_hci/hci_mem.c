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
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"


#define L_ALLOC(in_len, out_buf)                           \
    do {                                                   \
        if ( NULL == ((out_buf) = mem_Alloc(in_len)) )     \
            return ERR_HCI_NOMEM;                          \
    } while(0);

/* Internal component API */

uint32_t
hci_l_mem_AllocWithZeros(uint32_t in_len, uint8_t **out_buf)
{
    L_ALLOC(in_len, *out_buf);
    mem_Set(*out_buf, 0x0, in_len);
    return ERR_HCI_OK;
}

uint32_t
hci_l_mem_AllocWithCopy(uint32_t in_len, uint8_t *in_data, uint8_t **out_buf)
{
    L_ALLOC(in_len, *out_buf);
    mem_Copy(*out_buf, in_data, in_len);
    return ERR_HCI_OK;
}

uint32_t
hci_l_mem_Alloc(uint32_t in_len, uint8_t **out_buf)
{
    L_ALLOC(in_len, *out_buf);
    return ERR_HCI_OK;
}

void
hci_l_mem_Free(void *in_ptr)
{
    if ( NULL != in_ptr ) {
        mem_Free(in_ptr);
    }
}

uint32_t
hci_l_mem_BinstrAlloc(uint32_t in_len, binstr_t **out_binstr)
{
    if ( NULL == (*out_binstr = binstr_Alloc(in_len, NULL)) ) {
        return ERR_HCI_NOMEM;
    }
    return ERR_HCI_OK;
}

uint32_t
hci_l_mem_BinstrAllocWithCopy(uint32_t in_len, const uint8_t *in_buf, binstr_t **out_binstr)
{
    if ( NULL == (*out_binstr = binstr_Alloc(in_len, in_buf)) ) {
        return ERR_HCI_NOMEM;
    }
    return ERR_HCI_OK;
}

void
hci_l_mem_binstrFree(binstr_t *in_ptr)
{
    if ( NULL != in_ptr ) {
        binstr_Free(in_ptr);
    }
}

/* Public API */

void
hci_FreeMem(void *in_ptr)
{
    /* Note that Nautilus calls hci_FreeMem(NULL) sometimes without acquiring the HCI lock */

    if ( NULL != in_ptr ) {

        if ( ERR_HCI_OK == hci_l_IsHciLocked() ) {
            /*
             * For the PC implementation we want a memleak to happen to force
             * us to properly implement freeing while being inside the HCI lock.
             * This is not a strick requirement (see the comment in hci_api.h)
             */
            hci_l_mem_Free(in_ptr);
        } else {
        }
    }
}
