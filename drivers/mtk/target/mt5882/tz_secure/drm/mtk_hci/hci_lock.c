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
#include "binstr_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"


#define L_HCI_LOCK    (1)
#define L_HCI_UNLOCK  (0)

/* HCI lock. */
static int l_lock = L_HCI_UNLOCK;

uint32_t
hci_l_IsHciLocked(void)
{
    uint32_t ret = ERR_HCI_OK;

    if ( L_HCI_LOCK != l_lock ) {
        /*printf("######################################\n");
          printf("########### HCI NOT LOCKED ###########\n");
          printf("######################################\n");*/
        RET( ERR_HCI_NOTLOCKED );
    }

EXIT:
    return ret;
}

uint32_t
hci_Lock(hci_lock_t *out_lock)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_Lock"));

    if (out_lock == NULL) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    /* Check if hci is already locked */
    if ( L_HCI_LOCK == l_lock ) {
        RET( ERR_HCI_ALREADYLOCKED);
    }

    l_lock = L_HCI_LOCK;

    *out_lock = (hci_lock_t)1;

EXIT:
    MSGEXT(("hci_Lock"));
    return ret;
}


uint32_t
hci_Unlock(hci_lock_t io_lock)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_Unlock"));

    if ( io_lock != NULL ) {

        if ( L_HCI_UNLOCK == l_lock ) {
            RET( ERR_HCI_NOTLOCKED );
        }

        hci_l_ClearCachedOctPersonality();

        /* Reset the lock */
        l_lock = L_HCI_UNLOCK;
    }

EXIT:
    MSGEXT(("hci_Unlock"));
    return ret;
}
