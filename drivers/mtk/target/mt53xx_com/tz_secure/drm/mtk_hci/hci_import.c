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
#include "hci_crypto.h"


#if 1 == HCI_L_API_HCI_ENCRYPTSTREAM_WITH_IV
uint32_t
hci_EncryptStreamWithIV(hci_key_ref_t     in_ref,
                        hci_enc_algo_t    in_enc_algo,
                        hci_enc_mode_t    in_enc_mode,
                        hci_enc_pad_t     in_enc_pad,
                        hci_bool_t        in_is_final,
                        uint32_t          in_iv_len,
                        uint8_t          *io_iv,
                        uint32_t          in_data_len,
                        uint8_t          *in_data,
                        uint32_t         *io_data_len,
                        uint8_t          *out_data)
{
    uint32_t          ret               = ERR_HCI_OK;
    hci_l_key_ref_t  *ref               = in_ref;
    binstr_t         *raw_key           = NULL;
    hci_enc_pad_t     padding           = in_enc_pad;

    MSGENT(("hci_EncryptStreamWithIV"));

    if ( (NULL == ref)                           ||
         (NULL == io_data_len)                   ||
         (NULL == out_data)                      ||
         (in_iv_len     != HCI_L_AES_BLOCKSIZE)  ||
         (in_enc_algo   != HCI_ENC_ALGO_AES)     ||
         (ref->key_type != HCI_L_KEY_TYPE_CONTENT &&
          ref->key_type != HCI_L_KEY_TYPE_GENERATED) ) {
        RET( ERR_HCI_ILLEGALARGS );
    }

    if ( in_enc_mode != HCI_ENC_MODE_CBC ) {
        RET( ERR_HCI_NOTSUPPORTED );
    }

    /* HCI Lock need not be checked in this API */

    if ( NULL == ref->key_data ) {
        RET( ERR_HCI_KEYNOTINREF );
    }

    RET( hci_l_Unscramble(ref->key_data, &raw_key) );

    if ( HCI_FALSE == in_is_final ) {
        padding = HCI_ENC_PAD_NONE;
    }

    RET( hci_l_cryptoEncryptAES128CBC(padding, binstr_Buf(raw_key), io_iv,
                                      in_data_len, in_data, io_data_len, out_data) );

EXIT:
    hci_l_FreeKey(raw_key);
    MSGEXT(("hci_EncryptStreamWithIV"));
    return ret;
}

#endif
