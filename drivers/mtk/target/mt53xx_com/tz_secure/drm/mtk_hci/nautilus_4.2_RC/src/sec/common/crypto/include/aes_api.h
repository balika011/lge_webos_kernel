/** @addtogroup crypto crypto
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef AES_API_H
#define AES_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*========================================================================*/
    /** @file
     *  @brief  AES - API definitions
     *
     *  @verbatim
     *  dependency: base, mem
     *  @endverbatim
     *
     *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
     *  @date   2 Dec., 2004 modified by y-tanaka
     */
    /*========================================================================*/

#define AES_ENCRYPT        1  /*  Are we encrpyting?  */
#define AES_DECRYPT        2  /*  Are we decrpyting?  */

#define AES_BLOCKSIZE        16

    /*------------------------------------------------------------------------*
     * type definition
     *------------------------------------------------------------------------*/
    typedef void     *aes_handle_t;

    /*------------------------------------------------------------------------*
     * type definition
     *------------------------------------------------------------------------*/

#define ERR_AES_KEY_DIR         (ERR_AES_BASE|1)
#define ERR_AES_KEY_MAT         (ERR_AES_BASE|2)
#define ERR_AES_KEY_INSTANCE    (ERR_AES_BASE|3)
#define ERR_AES_CIPHER_STATE    (ERR_AES_BASE|4)


    /*========================================================================*/
    /**
     * @brief  Initializes AES context
     *
     * Sets key (initialize key schedule array)
     *
     * @param[out] out_aes aes handle
     * @param[in] in_dir direction (AES_ENCRYPT or AES_DECRYPT)
     * @param[in] in_keyLen key length (128 or 192 or 256)
     * @param[in] in_keyBuf key data
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_ILLEGALHANDLE                  <br>If an Invalid handle is provided as input to function
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_AES_KEY_DIR                    <br>If invalid direction is supplied
     * @return ::ERR_AES_KEY_MAT                    <br>If invalid key length is supplied
     * @return ::ERR_CRYPTO_INIT                    <br>Error in crypto initialization
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     */
    /*========================================================================*/
    retcode_t
    aes_Init(aes_handle_t   *out_aes,
             u_int32_t       in_dir,
             u_int32_t       in_keyLen,
             const u_int8_t *in_keyBuf);

    /*========================================================================*/
    /**
     * @brief  Finalizes AES context
     *
     * Destroys context
     *
     * @param[in] in_aes aes handle
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_ILLEGALHANDLE                  <br>If an Invalid handle is provided as input to function
     */
    /*========================================================================*/
    retcode_t
    aes_Fin(aes_handle_t in_aes);

    /*========================================================================*/
    /**
     * @brief  Encrypts a block of data
     *
     * Encrypts input plain text block of size 128 bits
     *
     * @param[in,out] io_aes  aes handle
     * @param[in] in_plain  plain text
     * @param[out] out_cipher cipher text
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_AES_CIPHER_STATE               <br>If cipher state is invalid (io_aes->dir is decrypt)
     */
    /*========================================================================*/
    retcode_t
    aes_BlockEncrypt(aes_handle_t    io_aes,
                     const u_int8_t *in_plain,
                     u_int8_t       *out_cipher);

    /*========================================================================*/
    /**
     * @brief  Decrypts a block of data
     *
     * Decrypts input cipher text block of size 128 bits
     *
     * @param[in,out] io_aes  aes handle
     * @param[in] in_cipher  cipher text
     * @param[out] out_plain plain text
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_AES_CIPHER_STATE               <br>If cipher state is invalid (io_aes->dir is encrypt)
     */
    /*========================================================================*/
    retcode_t
    aes_BlockDecrypt(aes_handle_t    io_aes,
                     const u_int8_t *in_cipher,
                     u_int8_t       *out_plain);


#ifdef __cplusplus
}
#endif

#endif /* AES_API_H */
/** @} */
