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

uint32_t
hci_Init(void)
{
    uint32_t ret = ERR_HCI_OK;

    MSGENT(("hci_Init"));

    /* @note HCI Locking is not required for this API */

    /* Add platform specific initialisations here if necessary */

    MSGEXT(("hci_Init"));
    return ret;
}


void
hci_Fin(void)
{
    MSGENT(("hci_Fin"));

    /* @note HCI Locking is not required for this API */
    hci_l_DestroyCache();

    /* Add platform specific finalisations here if necessary */

    MSGEXT_V(("hci_Fin"));
}
