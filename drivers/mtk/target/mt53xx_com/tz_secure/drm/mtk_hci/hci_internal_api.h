/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011,2012 Sony Corporation.
 */

#ifndef HCI_INTERNAL_H_
#define HCI_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

    extern uint32_t
    hci_l_IsHciLocked(void);

    extern uint32_t
    hci_l_CheckKeyAttr(hci_l_key_type_t  in_key_type,
                       hci_l_attr_t      in_attr,
                       hci_bool_t       *out_usable);

    extern uint32_t
    hci_l_CreateKeyRefStruct(hci_l_key_type_t     in_key_type,
                             hci_l_key_ref_t    **out_ref );

    extern uint32_t
    hci_l_GetKeyFormat(binstr_t                  *in_key,
                       hci_l_key_format_t        *out_key_format);

    extern uint32_t
    hci_l_FreeKeyRef( hci_l_key_ref_t            *in_ref);


    extern uint32_t
    hci_l_InitAESKey(hci_l_key_ref_t             *io_ref,
                     hci_key_type_t               in_key_type);


    extern void
    hci_l_ClearCachedOctPersonality(void);


    extern uint32_t
    hci_l_CalculateHash(hci_key_ref_t             in_ref,
                        hci_dig_algo_t            in_dig_algo,
                        binstr_t                 *in_data,
                        binstr_t                **out_hash);


    extern uint32_t
    hci_l_ConvSSMemErr(retcode_t                  in_ret);


    extern uint32_t
    hci_l_mem_AllocWithZeros(uint32_t             in_len,
                             uint8_t            **out_buf);

    extern uint32_t
    hci_l_mem_AllocWithCopy(uint32_t              in_len,
                            uint8_t              *in_data,
                            uint8_t             **out_buf);

    extern uint32_t
    hci_l_mem_Alloc(uint32_t                      in_len,
                    uint8_t                     **out_buf);

    extern uint32_t
    hci_l_mem_BinstrAlloc(uint32_t                in_len,
                          binstr_t              **out_binstr);

    extern uint32_t
    hci_l_mem_BinstrAllocWithCopy(uint32_t        in_len,
                                  const uint8_t  *in_buf,
                                  binstr_t      **out_binstr);

    extern void
    hci_l_mem_Free(void *in_ptr);

    extern void
    hci_l_mem_binstrFree(binstr_t                *in_ptr);


    /**
     * @brief Parse the nemo personality and return the private keys.
     *
     * @param[in]  in_bs       Binary string containing the nemo personality.
     * @param[out] out_sigkey  Nemo signing key in PKCS#8 format.
     * @param[out] out_enckey  Nemo encoding key in PKCS#8 format.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_ParseNEMOPrivateKeys(binstr_t    *in_bs,
                               binstr_t   **out_sigkey,
                               binstr_t   **out_enckey);

    /**
     * @brief Parse the octopus personality and return the private keys.
     *
     * @param[in]  in_bs           Binary string containing the octopus personality.
     * @param[in]  in_type         Type of private key to return.
     * @param[out] out_keyid       Octopus key id.
     * @param[out] out_keydata     Octopus key data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_ParseOctopusPrivatePersonality(binstr_t            *in_bs,
                                         hci_l_pers_type_t    in_type,
                                         char               **out_keyid,
                                         binstr_t           **out_keydata);

    /**
     * @brief Load the Nemo/Octopus private personality into the HCI.
     *
     * @param[in] in_type     Personality type
     * @param[in] in_len      Length of the buffer.
     * @param[in] in_buf      Buffer which contains the personality data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ssmem_SetObject
     */
    extern uint32_t
    hci_l_LoadPersonalityKey(hci_pers_key_type_t  in_type,
                             uint32_t             in_len,
                             uint8_t             *in_buf);

    extern uint32_t
    hci_l_GetOctPersonality(hci_l_pers_type_t     in_type,
                            char                **out_keyid,
                            binstr_t            **out_keydata);

    /* Example function for scrambling the supplied key data */
    extern uint32_t
    hci_l_Scramble(binstr_t                      *io_key);

    /* Example function for unscrambling the supplied key data */
    extern uint32_t
    hci_l_Unscramble(binstr_t                    *in_key,
                     binstr_t                   **out_key);

    /* Scrubbing the sensitive contents before de-allocating*/
    extern uint32_t
    hci_l_FreeKey(binstr_t                       *io_key);

    /* Randomise (scrub) the sensitive data in the when no longer used*/
    extern void
    hci_l_DataScrub(uint8_t                      *io_buf,
                    uint32_t                      in_buf_len);

    extern uint32_t
    hci_l_b64Decode(const binstr_t               *in_str,
                    binstr_t                    **out_str);

    /**
     * @brief Decrypts the encrypted ppproc data
     *
     * @param[in]  in_len       Length of the buffer.
     * @param[in]  in_buf       Buffer which contains the encrypted ppproc data.
     * @param[in]  in_key_len   Length of the key.
     * @param[in]  in_key       Key to decrypt ppproc data. Key is scrambled(base64 encoded)
     * @param[in]  in_iv        IV.
     * @param[in]  in_pad_type  Padding type.
     * @param[out] out_buf_len  Length of the buffer.
     * @param[out] out_buf      Buffer which contains the plain ppproc data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_INTERNALERROR
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::crypto_Init
     * @return ::crypto_Decrypt
     */
    extern uint32_t
    hci_l_DecryptPpprocData( uint32_t      in_len,
                             uint8_t      *in_buf,
                             uint32_t      in_key_len,
                             uint8_t      *in_key,
                             uint8_t      *in_iv,
                             hci_enc_pad_t in_pad_type,
                             uint32_t     *out_buf_len,
                             uint8_t     **out_buf );

    /**
     * @brief Parses the plain ppproc data and returns the public part of the ppproc data
     *
     * @param[in]  in_len       Length of the buffer.
     * @param[in]  in_buf       Buffer which contains the plain ppproc data.
     * @param[out] out_buf_len  Length of the buffer.
     * @param[out] out_buf      Buffer which contains the public part of ppproc data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ILLEGALPPPROC
     * @return ::ssmem_SetObject
     */
    extern uint32_t
    hci_l_ParsePpprocData( uint32_t             in_len,
                           uint8_t             *in_buf,
                           uint32_t            *out_buf_len,
                           uint8_t            **out_buf );

    /**
     * @brief Parses the plain moa ppproc data and returns the public part of the ppproc data
     *
     * @param[in]  in_moa_ppproc_data_len  Length of the plain moa ppproc data.
     * @param[in]  in_moa_ppproc_data      plain moa ppproc data.
     * @param[out] out_public_ppproc_len   Length of the public part of ppproc data.
     * @param[out] out_public_ppproc       public part of ppproc data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_ILLEGALPPPROC
     * @return ::ssmem_SetObject
     */

    extern uint32_t
    hci_l_ParseMoaPpprocData( uint32_t             in_moa_ppproc_data_len,
                              uint8_t             *in_moa_ppproc_data,
                              uint32_t            *out_public_ppproc_len,
                              uint8_t            **out_public_ppproc );

    /**
     * @brief Local function to add a key to an internal key cache
     *
     * @param[in] in_key_type     The key type. Currently on domain keys are added to the cache.
     * @param[in] in_enc_algo     The encryption algorithm the key is used in.
     * @param[in] in_enc_mode     The mode used by the supplied encryption algorithm
     * @param[in] in_enc_pad      The padding algorithm used by the supplied encryption algorithm.
     * @param[in] in_oaep_param   RSA OAEP parameters associated with the encrypted key. Can be NULL.
     * @param[in] in_iv           An initialisation vector associated with the key. Can be NULL.
     * @param[in] in_key_data     The key to add to the cache, protected with an encryption key.
     * @param[in] in_enc_data     The encryption key data protecting the input key data.
     * @param[in] in_dec_data     The plaintext key corresponding to the protected input key data.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     */
    extern uint32_t
    hci_l_AddKeyToCache(hci_imp_key_type_t     in_key_type,
                        hci_enc_algo_t         in_enc_algo,
                        hci_enc_mode_t         in_enc_mode,
                        hci_enc_pad_t          in_enc_pad,
                        binstr_t              *in_oaep_param,
                        binstr_t              *in_iv,
                        binstr_t              *in_key_data,
                        binstr_t              *in_enc_data,
                        binstr_t              *in_dec_data);

    /**
     * @brief Local function to access a key that matches with the input information
     *
     * @param[in]  in_key_type     The key type. Currently on domain keys are added to the cache.
     * @param[in]  in_enc_algo     The encryption algorithm the key is used in.
     * @param[in]  in_enc_mode     The mode used by the supplied encryption algorithm
     * @param[in]  in_enc_pad      The padding algorithm used by the supplied encryption algorithm
     * @param[in]  in_oaep_param   RSA OAEP parameters associated with the encrypted key. Can be NULL.
     * @param[in]  in_iv           An initialisation vector associated with the key. Can be NULL.
     * @param[in]  in_key_data     The original key data.
     * @param[in]  in_enc_data     The encrypted data.
     * @param[out] out_dec_data    If there is a key in the internal cache that matches the input parameters,
     *                             the key data is returned in this pointer. If no key is found ERR_HCI_OK is returned
     *                             and the output is set to NULL.
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     */
    extern uint32_t
    hci_l_GetKeyFromCache(hci_imp_key_type_t    in_key_type,
                          hci_enc_algo_t        in_enc_algo,
                          hci_enc_mode_t        in_enc_mode,
                          hci_enc_pad_t         in_enc_pad,
                          binstr_t             *in_oaep_param,
                          binstr_t             *in_iv,
                          binstr_t             *in_key_data,
                          binstr_t             *in_enc_data,
                          binstr_t            **out_dec_data);

    /**
     * @brief Local function to destroy the internal key cache
     *
     * @return ::ERR_HCI_OK
     */
    extern uint32_t
    hci_l_DestroyCache(void);

#ifdef __cplusplus
}
#endif

#endif /* HCI_INTERNAL_H_ */

