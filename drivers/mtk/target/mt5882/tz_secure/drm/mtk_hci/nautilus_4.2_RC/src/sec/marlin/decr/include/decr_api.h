/** @addtogroup decr decr
 * @ingroup marlin
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */

#ifndef DECR_API_H
#define DECR_API_H

NP_BEGIN_EXTERN_C

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

#define ERR_DECR_UNSUPPORTED       (ERR_DECR_BASE|1)
#define ERR_DECR_CRYPTOERROR       (ERR_DECR_BASE|2)
#define ERR_DECR_TOOSMALLBUFFER    (ERR_DECR_BASE|3)
#define ERR_DECR_KEYNOTFOUND       (ERR_DECR_BASE|4)

#define DECR_TYPE_NOT_SPECIFIED    0
#define DECR_TYPE_CLEAR            1
#define DECR_TYPE_AES128CTR        2
#define DECR_TYPE_AES128CTR64      3
#define DECR_TYPE_KFCBC            4 /* KFormat CBC mode */
#define DECR_TYPE_BBTS_SKL         5 /* BBTS crypto for Single Key Layer*/
#define DECR_TYPE_BBTS_AES128CBC   6 /* TS packet decryption mode */
#define DECR_TYPE_UNDEFINED        7 /* Use for unknown crypto key move */

/* encryption algorithm for KFormat CBC mode */
#define DECR_KFCBC_ALGO_AES        0 /* AES128 */
#define DECR_KFCBC_ALGO_DES        1 /* DES */

/* padding algorithm for KFormat CBC mode */
#define DECR_KFCBC_PAD_NONE        0
#define DECR_KFCBC_PAD_RFC2630     1
#define DECR_KFCBC_PAD_LENGTH      2

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

typedef void *decr_handle_t;

/**
 * Key info struct
 *
 * Keys are either 128 bit AES in Counter mode, or KFormat in CBC mode.
 * The ::decr_info_t type handles either key type in an opaque manner.
 */
typedef union {
    struct {
        u_int8_t selenc;                  /**< Flag that denotes encryption is on  */
        u_int8_t algo;                    /**< Either ::DECR_KFCBC_ALGO_DES or ::DECR_KFCBC_ALGO_AES  */
        u_int8_t padding;                 /**< Block padding type */
    } kfcbc;                              /**< KFormat CBC key info */
    struct {
        u_int8_t keyindex;                /**< Key index for BBTS */
    } bbtsaes128cbc;                      /**< BBTS data format */
} decr_info_t;

/**
 * Initialize Decryptor Context
 *
 * @param[in,out] io_decr decryptor handle
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 */
extern retcode_t
decr_Init(decr_handle_t *io_decr);


/**
 * Destroy Decryptor Context
 *
 * @param[in,out] io_decr decryptor handle
 * @return ::ERR_OK
 */
extern retcode_t
decr_Fin(decr_handle_t io_decr);


/**
 * Set SINF information to internal structure
 *
 * @param[in,out] io_decr  decryptor context handle
 * @param[in]     in_type  crypto type, one of ::DECR_TYPE_KFCBC, ::DECR_TYPE_ISMACRYP, ::DECR_TYPE_CLEAR
 *                                             ::DECR_TYPE_UNDEFINED, ::DECR_TYPE_BBTS_AES128CBC
 * @param[in]     in_info  crypto infomation
 * @param[in]     in_cid   content id
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_DECR_UNSUPPORTED
 */
extern retcode_t
decr_SetInfo(decr_handle_t io_decr, int32_t in_type, decr_info_t *in_info, binstr_t *in_cid);


/**
 * @brief Using the content ID contained in the decryptor, load the key from the
 *        keyring into the internal structure
 *
 * @param[in,out] io_decr            decryptor handle
 * @param[in]     in_keymng_context  key ring
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor or keyring handle
 * @return ::ERR_DECR_KEYNOTFOUND    if key is not found
 */
extern retcode_t
decr_SetKey(decr_handle_t io_decr, keymng_handle_t in_keymng_context);


/**
 * @brief Move a content key out of the HCI. This is done in a platform specific way
 *
 * @param[in]  in_decr    Decryptor handle associated with the key to move
 * @param[in]  in_pv      Application private data
 * @param[out] out_pv     Application private data
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS         if encrypted data length is too short(broken?)
 * @return ::ERR_DECR_KEYNOTFOUND    if key is not found
 */
extern retcode_t
decr_MoveKey(decr_handle_t in_decr, binstr_t *in_pv, binstr_t **out_pv);


/**
 * @brief Get a content ID out of decr context.
 *
 * @param[in]  in_decr     Decryptor context
 * @param[out] out_cid     Content id associated with the decryptor context
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS     if decrypt context is NULL
 * @return ::ERR_NOMEM           if there is no mem to allocate
 */
extern retcode_t
decr_GetCid(decr_handle_t  in_decr, binstr_t **out_cid);

/**
 * @brief Export a content key out of the HCI. This is done in a platform specific way.
 *
 * @note it can only export the key if an ExportPerform has been successful
 *
 * @param[in]  in_decr    Decryptor handle associated with the key to move
 * @param[in]  in_pv      Private Application data used to protect the
 *                        key being exported
 * @param[out] out_ck     Content key protected in an application specific way
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS         if encrypted data length is too short(broken?)
 * @return ::ERR_DECR_KEYNOTFOUND    if key is not found
 */
extern retcode_t
decr_ExportKey(decr_handle_t in_decr, binstr_t *in_pv, binstr_t **out_ck);


/**
 * @brief Check the hmac using the content key associated with the decr over the supplied
 *        data against the supplied digest
 *
 * @param[in]     in_decr            decryptor handle
 * @param[in]     in_digest_algo     digest algorithm to use with the HMAC operation
 * @param[in]     in_data            data to generate the digest over
 * @param[in]     in_digest          expected digest value to check against
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE        illegal decryptor handle
 * @return ::ERR_ILLEGALARGS          if encrypted data length is too short(broken?)
 * @return ::ERR_DECR_KEYNOTFOUND     if key is not found
 * @return ::ERR_DECR_CRYPTOERROR     if the hci crypto fails or if the check of the digests fails
 */
extern retcode_t
decr_CheckHMAC(decr_handle_t    in_decr,
               int32_t          in_digest_algo,
               binstr_t        *in_data,
               binstr_t        *in_digest);

/**
 * @brief Decrypting routine
 *
 * @param[in]     in_decr       decryptor handle
 * @param[in]     in_is_final   The Decrypt is the last one of a multipart decrypt and padding is to be applied
 * @param[in]     in_encData    Encrypted Data
 * @param[in]     in_encLen     Encrypted Data Length
 * @param[in]     in_iv         Initialization vector data
 * @param[in]     in_ivLen      Initialization vector data length
 * @param[out]    out_decData   Decrypted Data
 * @param[in,out] io_decLen     Decrypted Data Length, initially set to the size of the incoming decrypted buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS         if encrypted data length is too short(broken?)
 * @return ::ERR_DECR_TOOSMALLBUFFER if io_decLen is too small to store the decrypted data
 * @return ::ERR_DECR_UNSUPPORTED      if input padding or encryption mode is not supported
 * @return ::ERR_DECR_CRYPTOERROR    if the hci crypto fails
 */
extern retcode_t
decr_DecryptBlock(decr_handle_t     in_decr,
                  bool_t            in_is_final,
                  u_int8_t         *in_encData,
                  u_int32_t         in_encLen,
                  u_int8_t         *in_iv,
                  u_int32_t         in_ivLen,
                  u_int8_t         *out_decData,
                  u_int32_t        *io_decLen);

/**
 * @brief Decryption for streams
 *
 * @param[in]     in_decr     Decryptor handle
 * @param[in]     in_offset   Offset from the first byte in the cipher text
 * @param[in]     in_ctrsize  Counter size
 * @param[in]     in_ctr      Counter
 * @param[in]     in_ibsize   Input ciphertext length
 * @param[in]     in_buffer   Input ciphertext
 * @param[in,out] io_obsize   Decrypted data length
 * @param[out]    out_buffer  Decrypted data
 */
extern retcode_t
decr_DecryptStreamAtOffset(decr_handle_t     in_decr,
                           np_size_t         in_offset,
                           np_size_t         in_ctrsize,
                           u_int8_t         *in_ctr,
                           np_size_t         in_ibsize,
                           u_int8_t         *in_buffer,
                           np_size_t        *io_obsize,
                           u_int8_t         *out_buffer);

/**
 * @brief Encrypts Data
 *
 * @param[in]     in_decr       decryptor handle
 * @param[in]     in_is_final   The Encrypt is the last one of a multipart encrypt and padding is to be applied
 * @param[in]     in_plain_len  Plain Data Length
 * @param[in]     in_plain_buf  Plain Data Buffer
 * @param[in]     in_iv_len     IV length
 * @param[in,out] io_iv         IV buffer
 * @param[in,out] io_enc_len    Encrypted Data Length
 * @param[in,out] io_enc_buf    Encrypted Data Buffer
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS         if plain data length is too short(broken?)
 * @return ::ERR_DECR_TOOSMALLBUFFER if io_enc_len is too small to store the encrypted data
 * @return ::ERR_DECR_UNSUPPORTED      if input padding or encryption mode is not supported
 * @return ::ERR_DECR_CRYPTOERROR    if the hci crypto fails
 */
extern retcode_t
decr_EncryptBlock(decr_handle_t   in_decr,
                  bool_t          in_is_final,
                  u_int32_t       in_plain_len,
                  u_int8_t       *in_plain_buf,
                  u_int32_t       in_iv_len,
                  u_int8_t       *io_iv,
                  u_int32_t      *io_enc_len,
                  u_int8_t       *io_enc_buf);

/**
 * @brief Generating Hmac routine
 *
 * @param[in]     in_decr           decryptor handle
 * @param[in]     in_digest_algo    digest algo
 * @param[in]     in_data           data, Hmac to be calculated on
 * @param[out]    out_hmac          generated Hmac
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALHANDLE       illegal decryptor handle
 * @return ::ERR_ILLEGALARGS         if arguments are invalid
 * @return ::ERR_DECR_UNSUPPORTED    if input digest algo is not supported
 * @return ::ERR_DECR_CRYPTOERROR    if the hci crypto fails
 */
extern retcode_t
decr_GenerateHMAC(decr_handle_t       in_decr,
                  int32_t             in_digest_algo,
                  binstr_t           *in_data,
                  binstr_t          **out_hmac);

NP_END_EXTERN_C

#endif /* DECR_API_H */
/** @{ */
