/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */
/* cmn/include */
#include "basic_types.h"
/* cmn */
#include "ns_config.h"
#include "binstr_api.h"
/* platform/sec */
#include "hci_config.h"
#include "hci_api.h"
#include "hci_types.h"


/*
 * Implements all not-supported HCI APIs (disabled through hci_config.h)
 */


#if 0 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_IV
uint32_t
hci_DecryptStreamWithIV(hci_key_ref_t     in_ref,
                        hci_enc_algo_t    in_enc_algo,
                        hci_enc_mode_t    in_enc_mode,
                        hci_enc_pad_t     in_enc_pad,
                        hci_bool_t        in_is_final,
                        uint32_t          in_iv_len,
                        uint8_t          *in_iv,
                        uint32_t          in_data_len,
                        uint8_t          *in_data,
                        uint32_t         *io_data_len,
                        uint8_t          *out_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif

#if 0 == HCI_L_API_HCI_DECRYPTSTREAM_WITH_OFFSET
uint32_t
hci_DecryptStreamWithOffset(hci_key_ref_t     in_ref,
                            hci_enc_algo_t    in_enc_algo,
                            hci_enc_mode_t    in_enc_mode,
                            hci_enc_pad_t     in_enc_pad,
                            np_size_t         in_offset,
                            np_size_t         in_ctrsize,
                            u_int8_t         *in_ctr,
                            np_size_t         in_data_len,
                            u_int8_t         *in_data,
                            np_size_t        *io_data_len,
                            u_int8_t         *out_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_EXPORTKEY
uint32_t
hci_ExportKey(hci_key_ref_t    in_ref,
              uint32_t         in_private_data_len,
              uint8_t         *in_private_data,
              uint32_t        *out_private_data_len,
              uint8_t        **out_private_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_CHECKHASH
uint32_t
hci_CheckHash(hci_key_ref_t   in_ref,
              hci_dig_algo_t  in_dig_algo,
              uint32_t        in_data_len,
              uint8_t        *in_data,
              uint32_t        in_expected_hmac_len,
              uint8_t        *in_expected_hmac,
              hci_bool_t     *out_result)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_GENERATEHASH
uint32_t
hci_GenerateHash(hci_key_ref_t   in_ref,
                 hci_dig_algo_t  in_dig_algo,
                 uint32_t        in_data_len,
                 uint8_t        *in_data,
                 uint32_t       *out_hash_len,
                 uint8_t       **out_hash)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_MOVEKEY
uint32_t
hci_MoveKey(hci_key_ref_t   io_ref,
            uint32_t        in_private_data_len,
            uint8_t        *in_private_data,
            uint32_t       *out_private_data_len,
            uint8_t       **out_private_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_ENCRYPTKEYDATA
uint32_t
hci_EncryptKeyData(hci_key_ref_t  in_key_ref,
                   hci_enc_algo_t in_enc_algo,
                   hci_enc_mode_t in_enc_mode,
                   hci_enc_pad_t  in_enc_pad,
                   uint32_t       in_oaep_param_len,
                   uint8_t       *in_oaep_param,
                   hci_key_ref_t  in_key,
                   uint32_t      *out_iv_len,
                   uint8_t      **out_iv,
                   uint32_t      *out_key_data_len,
                   uint8_t      **out_key_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_IMPORTSAS
uint32_t
hci_ImportSAS(uint32_t         in_sas_len,
              uint8_t         *in_sas,
              uint32_t         in_private_data_len,
              uint8_t         *in_private_data,
              uint32_t        *out_public_sas_len,
              uint8_t        **out_public_sas,
              hci_sas_keys_t **out_sas_keys)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_FREESASKEYS
uint32_t
hci_FreeSASKeys(hci_sas_keys_t *io_sas_keys)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_GENERATECONTENTKEY
uint32_t
hci_GenerateContentKey(uint32_t        in_key_sz,
                       uint32_t        in_private_data_len,
                       uint8_t        *in_private_data,
                       hci_key_ref_t  *out_ref)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_LOADKEY
uint32_t
hci_LoadKey(hci_load_key_type_t  in_type,
            hci_key_data_t      *in_key_data,
            hci_key_ref_t       *out_ref)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_GENERATEHMAC
uint32_t
hci_GenerateHMAC(hci_key_ref_t   in_ref,
                 hci_dig_algo_t  in_dig_algo,
                 uint32_t        in_data_len,
                 uint8_t        *in_data,
                 uint32_t       *out_hmac_len,
                 uint8_t       **out_hmac)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_IMPORTBKBCONTAINER
uint32_t
hci_ImportBKBContainer(uint32_t    in_container_len,
                       uint8_t    *in_container,
                       hci_bool_t  in_from_ppproc,
                       uint32_t    in_private_data_len,
                       uint8_t    *in_private_data,
                       uint32_t   *out_container_len,
                       uint8_t   **out_container,
                       uint32_t   *out_bkb_len,
                       uint8_t   **out_bkb)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_ENCRYPTDATA
extern uint32_t
hci_EncryptData(hci_key_ref_t     in_ref,
                hci_enc_algo_t    in_enc_algo,
                hci_enc_mode_t    in_enc_mode,
                hci_enc_pad_t     in_enc_pad,
                uint32_t          in_oaep_param_len,
                uint8_t          *in_oaep_param,
                uint32_t          in_data_len,
                uint8_t          *in_data,
                uint32_t         *out_iv_len,
                uint8_t         **out_iv,
                uint32_t         *out_data_len,
                uint8_t         **out_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif


#if 0 == HCI_L_API_HCI_ENCRYPTSTREAM_WITH_IV
uint32_t
hci_EncryptStreamWithIV(hci_key_ref_t     in_ref,
                        hci_enc_algo_t    in_enc_algo,
                        hci_enc_mode_t    in_enc_mode,
                        hci_enc_pad_t     in_enc_pad,
                        hci_bool_t        in_is_final,
                        uint32_t          in_iv_len,
                        uint8_t          *in_iv,
                        uint32_t          in_data_len,
                        uint8_t          *in_data,
                        uint32_t         *io_data_len,
                        uint8_t          *out_data)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif

#if 0 == HCI_L_API_HCI_IMPORTPPPROCDATA
uint32_t
hci_ImportPpprocData(uint32_t             in_ppproc_data_len,
                     uint8_t             *in_ppproc_data,
                     hci_ppproc_format_t  in_format,
                     uint32_t            *out_public_ppproc_len,
                     uint8_t            **out_public_ppproc)
{
    return ERR_HCI_NOTSUPPORTED;
}
#endif
