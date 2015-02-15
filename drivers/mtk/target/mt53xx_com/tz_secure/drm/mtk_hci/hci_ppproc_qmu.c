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
#include "ns_config.h"
#include "dbg_api.h"
#include "mem_api.h"
#include "binstr_api.h"

/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"
#include "hci_internal_api.h"

#if 1 == HCI_L_API_HCI_IMPORTPPPROCDATA
#if (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_QMU)

#define L_PPPROC_DIVERSITY_KEY  "QJM3EXVinH+P598FZntxsQ=="
#define L_PPPROC_DIVERSITY_IV   "\x51\x99\x97\x32\x30\x79\x39\x1e\x8e\x87\x28\x07\x5a\x5d\x9a\x57"

uint32_t
hci_ImportPpprocData(uint32_t             in_ppproc_data_len,
                     uint8_t             *in_ppproc_data,
                     hci_ppproc_format_t  in_format,
                     uint32_t            *out_public_ppproc_len,
                     uint8_t            **out_public_ppproc)
{
    uint32_t   ret                     = ERR_HCI_OK;
    uint8_t   *ppproc_data             = in_ppproc_data;
    uint32_t   ppproc_data_len         = in_ppproc_data_len;
    uint8_t   *dec_ppproc_data         = NULL;
    uint32_t   dec_ppproc_data_len     = 0;
    uint8_t    iv[]                    = L_PPPROC_DIVERSITY_IV;
    MSGENT(("hci_ImportPpprocData"));

    RET( hci_l_IsHciLocked() );
    if ( (NULL == in_ppproc_data)                                                               ||
         (0    == in_ppproc_data_len)                                                           ||
         ((HCI_PLAIN_PPPROC_FORMAT != in_format) && (HCI_ENCRYPTED_PPPROC_FORMAT != in_format)) ||
         (NULL == out_public_ppproc_len)                                                        ||
         (NULL == out_public_ppproc) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( (HCI_ENCRYPTED_PPPROC_FORMAT == in_format)) {

        RET(hci_l_DecryptPpprocData(ppproc_data_len,
                                    ppproc_data,
                                    sizeof(L_PPPROC_DIVERSITY_KEY)-1,
                                    (uint8_t *)L_PPPROC_DIVERSITY_KEY,
                                    iv,
                                    HCI_ENC_PAD_NONE,
                                    &dec_ppproc_data_len,
                                    &dec_ppproc_data));

        if ( NULL == dec_ppproc_data || 0 == dec_ppproc_data_len ) {
            RET(ERR_HCI_ILLEGALPPPROC);
        }
        ppproc_data     = dec_ppproc_data;
        ppproc_data_len = dec_ppproc_data_len;
    }

    RET(hci_l_ParseMoaPpprocData(ppproc_data_len,
                                 ppproc_data,
                                 out_public_ppproc_len,
                                 out_public_ppproc));

EXIT:
    hci_l_DataScrub(dec_ppproc_data, dec_ppproc_data_len);
    hci_l_mem_Free(dec_ppproc_data);
    MSGEXT(("hci_ImportPpprocData"));
    return ret;
}

#endif /* #if (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_MOA_QMU) */
#endif /* #if 1 == HCI_L_API_HCI_IMPORTPPPROCDATA */
