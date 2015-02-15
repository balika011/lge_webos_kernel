/** @addtogroup hci hci
 * @ingroup marlin
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2011 Sony Corporation.
 */

#ifndef HCI_CRYPTO_H
#define HCI_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif


    /**
     * @brief Populates a buffer with random bytes
     *
     * @param[in]     in_num_bytes      length of out_data
     * @param[out]    out_buf           output buffer (allocated by caller)
     *
     * @return   ::ERR_HCI_OK
     * @return   ::ERR_HCI_ILLEGALARGS
     * @return   ::ERR_HCI_NOMEM
     * @return   ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoGenerateRandom(uint32_t  in_num_bytes,
                               uint8_t  *out_buf );


    /**
     * @brief Generates a digest of a message
     *
     * @param[in]      in_digest_algo   either HCI_DIG_ALGO_SHA1 or HCI_DIG_ALGO_SHA256
     * @param[in]      in_len           length of in_buf
     * @param[in]      in_buf           message
     * @param[in,out]  io_digest_size   in: length of allocated out_digest; out: length of returned digest
     * @param[out]     out_digest       digest (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_CRYPTOLIB         (not returned in the sample implementation; maybe returned in the general case)
     */
    extern uint32_t
    hci_l_cryptoMakeDigest(hci_dig_algo_t  in_digest_algo,
                           uint32_t        in_data_len,
                           uint8_t        *in_data,
                           uint32_t       *io_digest_len,
                           uint8_t        *out_digest);

    /**
     * @brief Generates an HMAC of a message
     *
     * @param[in]      in_digest_algo   digest algorithm (SHA1 or SHA256)
     * @param[in]      in_key_len       length of in_key
     * @param[in]      in_key           key to use for the hash
     * @param[in]      in_data_len      length of in_data
     * @param[in]      in_data          data to hash
     * @param[in,out]  io_hmac_len      in: length of allocated out_hmac; out: length of returned hmac
     * @param[out]     out_hmac         hmac value (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoCalculateHmac(hci_dig_algo_t  in_digest_algo,
                              uint32_t        in_key_len,
                              uint8_t        *in_key,
                              uint32_t        in_data_len,
                              uint8_t        *in_data,
                              uint32_t       *io_hmac_len,
                              uint8_t        *out_hmac);

    /**
     * @brief Encrypts data using AES-CBC and 128 bit key
     *
     * @param[in]       in_enc_pad     HCI_ENC_PAD_NONE or HCI_ENC_PAD_RFC2630 or HCI_ENC_PAD_XMLENC
     * @param[in]       in_key         16-byte key
     * @param[in,out]   io_iv          in: 16-byte Initial Vector for encrypting in_plain;
     *                                 out: unchanged or, when HCI_ENC_PAD_NONE, IV for encrypting next block
     * @param[in]       in_plain_len   length of in_plain
     * @param[in]       in_plain       data to encrypt
     * @param[in,out]   io_cipher_len  in: length of allocated out_cipher, out: length of encrypted data
     * @param[out]      out_cipher     encrypted data (without IV); allocated by caller
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoEncryptAES128CBC(hci_enc_pad_t      in_enc_pad,
                                 uint8_t           *in_key,
                                 uint8_t           *io_iv,
                                 uint32_t           in_plain_len,
                                 uint8_t           *in_plain,
                                 uint32_t          *io_cipher_len,
                                 uint8_t           *out_cipher);

    /**
     * @brief Decrypts data using AES-CBC and 128 bit key
     *
     * @param[in]       in_enc_pad     HCI_ENC_PAD_NONE or HCI_ENC_PAD_RFC2630 or HCI_ENC_PAD_XMLENC
     * @param[in]       in_key         16-byte key
     * @param[in,out]   io_iv          in: 16-byte Initial Vector for decrypting in_cipher;
     *                                 out: unchanged or, when HCI_ENC_PAD_NONE, IV for decrypting next block
     * @param[in]       in_cipher_len  length of in_cipher
     * @param[in]       in_cipher      data to decrypt (without IV)
     * @param[in,out]   io_plain_len   in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain      decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoDecryptAES128CBC(hci_enc_pad_t      in_enc_pad,
                                 uint8_t           *in_key,
                                 uint8_t           *io_iv,
                                 uint32_t           in_cipher_len,
                                 uint8_t           *in_cipher,
                                 uint32_t          *io_plain_len,
                                 uint8_t           *out_plain);

    /**
     * @brief Encrypts data using AES-CTR and 128 bit key
     *
     * @param[in]       in_key         16-byte key
     * @param[in]       io_iv          16-byte Initial Vector
     * @param[in]       in_is_final    HCI_TRUE if last block
     * @param[in]       in_plain_len   length of in_plain
     * @param[in]       in_plain       data to encrypt
     * @param[in,out]   io_cipher_len  in: length of allocated out_cipher, out: length of encrypted data
     * @param[out]      out_cipher     encrypted data (without IV); allocated by caller
     * @param[out]      out_counter    in: NULL, else out: next counter value
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoEncryptAES128CTR(uint8_t           *in_key,
                                 uint8_t           *in_iv,
                                 hci_bool_t         in_is_final,
                                 uint32_t           in_plain_len,
                                 uint8_t           *in_plain,
                                 uint32_t          *io_cipher_len,
                                 uint8_t           *out_cipher,
                                 uint8_t           *out_counter);

    /**
     * @brief Decrypts data using AES-CTR and 128 bit key
     *
     * @param[in]       in_key         16-byte key
     * @param[in]       in_offset      The ciphertext's offset from the start of the encrypted data
     * @param[in]       in_ctrsize     Length of the counter (AES block size)
     * @param[in]       in_ctr         Initial counter
     * @param[in]       in_cipher_len  Length of in_cipher
     * @param[in]       in_cipher      Data to decrypt (without IV)
     * @param[in,out]   io_plain_len   in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain      Decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoDecryptAES128CTR(uint8_t          *in_key,
                                 np_size_t         in_offset,
                                 np_size_t         in_ctrsize,
                                 uint8_t          *in_ctr,
                                 np_size_t         in_cipher_len,
                                 uint8_t          *in_cipher,
                                 np_size_t        *io_plain_len,
                                 uint8_t          *out_plain);

    /**
     * @brief Decrypts data using AES-ECB and 128 bit key
     *
     * @param[in]       in_key         16-byte key
     * @param[in]       in_cipher_len  Length of in_cipher
     * @param[in]       in_cipher      16-byte data to decrypt
     * @param[in,out]   io_plain_len   in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain      16-byte decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoDecryptAES128ECB(uint8_t  *in_key,
                                 uint32_t  in_cipher_len,
                                 uint8_t  *in_cipher,
                                 uint32_t *io_plain_len,
                                 uint8_t  *out_plain);

    /**
     * @brief Decrypts data using AES 128 bit key and residual padding
     *
     * @note: this function should be implemented for BBTS service only
     *
     * @param[in]       in_key         16-byte key
     * @param[in]       in_iv          16-byte Initial Vector for decrypting in_cipher;
     * @param[in]       in_cipher_len  length of in_cipher
     * @param[in]       in_cipher      data to decrypt (without IV)
     * @param[in,out]   io_plain_len   in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain      decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoDecryptAES128RTB(uint8_t           *in_key,
                                 uint8_t           *in_iv,
                                 uint32_t           in_cipher_len,
                                 uint8_t           *in_cipher,
                                 uint32_t          *io_plain_len,
                                 uint8_t           *out_plain);

    /**
     * @brief Encrypts data using RSA key
     *
     * @param[in]       in_n_len       length of in_n
     * @param[in]       in_n           value of n
     * @param[in]       in_e_len       length of in_e
     * @param[in]       in_e           value of e
     * @param[in]       in_plain_len   length of in_plain
     * @param[in]       in_plain       data to encrypt
     * @param[in,out]   io_cipher_len  in: length of allocated out_cipher, out: length of encrypted data
     * @param[out]      out_cipher     encrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     */
    extern uint32_t
    hci_l_cryptoEncryptRsaesPkcs1V1_5(uint32_t             in_n_len,
                                      uint8_t             *in_n,
                                      uint32_t             in_e_len,
                                      uint8_t             *in_e,
                                      uint32_t             in_plain_len,
                                      uint8_t             *in_plain,
                                      uint32_t            *io_cipher_len,
                                      uint8_t             *out_cipher);

    /**
     * @brief Decrypts data using RSA key and PKCS#1
     *
     * @param[in]       in_pkcs8_key_len  length of in_pkcs8_key
     * @param[in]       in_pkcs8_key      key in PKCS#8 format
     * @param[in]       in_cipher_len     length of in_cipher
     * @param[in]       in_cipher         data to decrypt
     * @param[in,out]   io_plain_len      in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain         decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_cryptoDecryptRsaesPkcs1V1_5(uint32_t             in_pkcs8_key_len,
                                      uint8_t             *in_pkcs8_key,
                                      uint32_t             in_cipher_len,
                                      uint8_t             *in_cipher,
                                      uint32_t            *io_plain_len,
                                      uint8_t             *out_plain);

    /**
     * @brief Encrypts data using RSA key and OAEP
     *
     * @param[in]       in_n_len          length of in_n
     * @param[in]       in_n              value of n
     * @param[in]       in_e_len          length of in_e
     * @param[in]       in_e              value of e
     * @param[in]       in_plain_len      length of in_plain
     * @param[in]       in_plain          data to encrypt
     * @param[in]       in_oaep_len       0, or length of in_oaep
     * @param[in]       in_oaep           NULL, or oaep padding parameter
     * @param[in,out]   io_cipher_len     in: length of allocated out_cipher, out: length of encrypted data
     * @param[out]      out_cipher        encrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_cryptoEncryptRsaesOaep(uint32_t             in_n_len,
                                 uint8_t             *in_n,
                                 uint32_t             in_e_len,
                                 uint8_t             *in_e,
                                 uint32_t             in_oaep_len,
                                 uint8_t             *in_oaep,
                                 uint32_t             in_plain_len,
                                 uint8_t             *in_plain,
                                 uint32_t            *io_cipher_len,
                                 uint8_t             *out_cipher);

    /**
     * @brief Decrypts data using RSA key and OAEP
     *
     * @param[in]       in_pkcs8_key_len  length of in_pkcs8_key
     * @param[in]       in_pkcs8_key      key in PKCS#8 format
     * @param[in]       in_cipher_len     length of in_cipher
     * @param[in]       in_cipher         data to decrypt
     * @param[in]       in_oaep_len       0, or length of in_oaep
     * @param[in]       in_oaep           NULL, or oaep padding parameter
     * @param[in,out]   io_plain_len      in: length of allocated out_plain, out: length of decrypted data
     * @param[out]      out_plain         decrypted data (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_cryptoDecryptRsaesOaep(uint32_t             in_pkcs8_key_len,
                                 uint8_t             *in_pkcs8_key,
                                 uint32_t             in_cipher_len,
                                 uint8_t             *in_cipher,
                                 uint32_t             in_oaep_len,
                                 uint8_t             *in_oaep,
                                 uint32_t            *io_plain_len,
                                 uint8_t             *out_plain);

    /**
     * @brief Signs data using RSA key
     *
     * @param[in]       in_pkcs8_key_len  length of in_pkcs8_key
     * @param[in]       in_pkcs8_key      key in PKCS#8 format
     * @param[in]       in_digest_algo    digest algorithm (SHA1 or SHA256)
     * @param[in]       in_message_len    length of in_message
     * @param[in]       in_message        message to sign
     * @param[in,out]   io_signature_len  in: length of allocated out_signature, out: length of signature
     * @param[out]      out_signature     signature (allocated by caller)
     *
     * @return ::ERR_HCI_OK
     * @return ::ERR_HCI_ILLEGALARGS
     * @return ::ERR_HCI_NOMEM
     * @return ::ERR_HCI_TOOSMALLBUFFER
     * @return ::ERR_HCI_CRYPTOLIB
     * @return ::ERR_HCI_ILLEGALPERSONALITY
     */
    extern uint32_t
    hci_l_cryptoSignRsassaPkcs1V1_5(uint32_t             in_pkcs8_key_len,
                                    uint8_t             *in_pkcs8_key,
                                    hci_dig_algo_t       in_digest_algo,
                                    uint32_t             in_message_len,
                                    uint8_t             *in_message,
                                    uint32_t            *io_signature_len,
                                    uint8_t             *out_signature);

#ifdef __cplusplus
}
#endif

#endif /* HCI_CRYPTO_H */
/** @} */

