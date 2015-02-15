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
#if (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_DEFAULT)

#define L_PPPROC_DIVERSITY_KEY  "4/ek4G6CPVAufeY3aj3ykw=="

/* Mcabe complexity of this funtion is 11. However the funtion is simple and easily understandable.
   The complexity is high because of input argument validation.*/
uint32_t
hci_ImportPpprocData(uint32_t             in_ppproc_data_len,
                     uint8_t             *in_ppproc_data,
                     hci_ppproc_format_t  in_format,
                     uint32_t            *out_public_ppproc_len,
                     uint8_t            **out_public_ppproc)
{
    uint32_t          ret                  = ERR_HCI_OK;
    uint8_t          *ppproc_data          = in_ppproc_data;
    uint32_t          ppproc_data_len      = in_ppproc_data_len;
    uint8_t          *dec_ppproc_data      = NULL;
    uint32_t          dec_ppproc_data_len  = 0;

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
        /* check whether the ppproc data has IV + 1 Block at least */
        if ( in_ppproc_data_len < (2 * HCI_L_AES_BLOCKSIZE) ) {
            RET( ERR_HCI_ILLEGALPPPROC);
        }

        RET(hci_l_DecryptPpprocData(in_ppproc_data_len - HCI_L_AES_BLOCKSIZE,
                                    in_ppproc_data + HCI_L_AES_BLOCKSIZE,
                                    sizeof(L_PPPROC_DIVERSITY_KEY)-1,
                                    (uint8_t *)L_PPPROC_DIVERSITY_KEY,
                                    in_ppproc_data,
                                    HCI_ENC_PAD_RFC2630,
                                    &dec_ppproc_data_len,
                                    &dec_ppproc_data));
        if ( NULL == dec_ppproc_data || 0 == dec_ppproc_data_len ) {
            RET(ERR_HCI_ILLEGALPPPROC);
        }
        ppproc_data     = dec_ppproc_data;
        ppproc_data_len = dec_ppproc_data_len;
    }

    RET(hci_l_ParsePpprocData(ppproc_data_len,
                              ppproc_data,
                              out_public_ppproc_len,
                              out_public_ppproc));

EXIT:
    hci_l_DataScrub(dec_ppproc_data, dec_ppproc_data_len);
    hci_l_mem_Free(dec_ppproc_data);
    MSGEXT(("hci_ImportPpprocData"));
    return ret;
}

#endif /* #if (PPPROC_FORMAT_VAL == HCI_L_PPPROC_FORMAT_DEFAULT) */
#endif /* #if 1 == HCI_L_API_HCI_IMPORTPPPROCDATA*/
