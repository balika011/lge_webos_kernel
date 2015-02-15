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

#ifndef RSA_API_H
#define RSA_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*========================================================================*/
    /** @file
     *  @brief  RSA - API definitions
     *
     *  @verbatim
     *  dependency: base, mem, binstr
     *  @endverbatim
     *
     *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
     *  @date   2 Dec., 2004 modified by y-tanaka
     */
    /*========================================================================*/
    /*------------------------------------------------------------------------*
     * type definition
     *------------------------------------------------------------------------*/
    typedef void     *rsa_handle_t;

    typedef struct rsa_key {
        binstr_t    *n;     /** modulus                             */
        binstr_t    *e;     /** publicExponent                      */
        binstr_t    *d;     /** privateExponent                     */
        binstr_t    *p;     /** prime1                              */
        binstr_t    *q;     /** prime2                              */
        binstr_t    *dP;    /** exponent1 d mod (p-1)               */
        binstr_t    *dQ;    /** exponent2 d mod (q-1)               */
        binstr_t    *qInv;  /** coefficient (inverse of q) mod p    */
    } rsa_key_t;

    /*------------------------------------------------------------------------*
     * error definition
     *------------------------------------------------------------------------*/

#define ERR_RSA_OUT_OF_RANGE            (ERR_RSA_BASE|1)
#define ERR_RSA_INVALID                 (ERR_RSA_BASE|2)
#define ERR_RSA_INVALID_SIGNATURE       (ERR_RSA_BASE|3)
#define ERR_RSA_MSG_TOO_LONG            (ERR_RSA_BASE|4)
#define ERR_RSA_MASK_TOO_LONG           (ERR_RSA_BASE|5)
#define ERR_RSA_EMSG_TOO_SHORT          (ERR_RSA_BASE|6)
#define ERR_RSA_INTEGER_TOO_LONG        (ERR_RSA_BASE|7)
#define ERR_RSA_INTEGER_TOO_LARGE       (ERR_RSA_BASE|8)
#define ERR_RSA_MOD_TOO_SHORT           (ERR_RSA_BASE|9)
#define ERR_RSA_DECRYPTION              (ERR_RSA_BASE|10)
#define ERR_RSA_DECODING                (ERR_RSA_BASE|11)
#define ERR_RSA_OTHERS                  (ERR_RSA_BASE|12)
#define ERR_RSA_NOTSUPPORTED            (ERR_RSA_BASE|13)

    /*------------------------------------------------------------------------*
     * macro definition
     *------------------------------------------------------------------------*/

    /**
     * @brief digest algorithm definitions
     */
#define RSA_DIGESTALGO_SHA1   0
#define RSA_DIGESTALGO_SHA256 1

    /*========================================================================*/
    /**
     * @brief  Initializes RSA context
     *
     * Intializes rsa handle with various parameters such as modulus, publicExponent, privateExponent,
     * prime1, prime2, exponent1, exponent2, coefficient (inverse of q) mod p
     *
     * @param[out] out_rsa   rsa handle
     * @param[in] in_n       modulus
     * @param[in] in_e       publicExponent
     * @param[in] in_d       privateExponent
     * @param[in] in_p       prime1
     * @param[in] in_q       prime2
     * @param[in] in_dP      exponent1 d mod (p-1)
     * @param[in] in_dQ      exponent2 d mod (q-1)
     * @param[in] in_qInv    coefficient (inverse of q) mod p
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NG                             <br>If the wrap type of input parameter in_n is not supported
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     */
    /*========================================================================*/
    extern retcode_t
    rsa_Init(rsa_handle_t   *out_rsa,
             const binstr_t *in_n,
             const binstr_t *in_e,
             const binstr_t *in_d,
             const binstr_t *in_p,
             const binstr_t *in_q,
             const binstr_t *in_dP,
             const binstr_t *in_dQ,
             const binstr_t *in_qInv);

    /*========================================================================*/
    /**
     * @brief  Finalizes RSA context
     *
     * Destroys the rsa handle
     *
     * @param[in] in_rsa rsa handle
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     */
    /*========================================================================*/
    extern retcode_t
    rsa_Fin(rsa_handle_t in_rsa);

    /*========================================================================*/
    /**
     * @brief Creates and set the structure of an RSA public key object
     *
     * Loads the structure with public key (e,n)
     *
     * @param[out]      out_key     Populated key
     * @param[in]       in_n        Modulus
     * @param[in]       in_e        Public Exponent
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     */
    /*========================================================================*/
    extern retcode_t
    rsa_CreatePubKey(
        rsa_key_t   **out_key,
        binstr_t     *in_n,
        binstr_t     *in_e);

    /*========================================================================*/
    /**
     * @brief Creates and set the structure of an RSA private key object
     *
     * Loads the structure with private key (d,n)
     *
     * @param[out]      out_key     Populated key
     * @param[in]       in_n        Modulus
     * @param[in]       in_e        Public Exponent
     * @param[in]       in_d        Private Exponent
     * @param[in]       in_p        Prime 1
     * @param[in]       in_q        Prime 2
     * @param[in]       in_dP       Exponent1 d mod (p-1)
     * @param[in]       in_dQ       Exponent2 d mod (q-1)
     * @param[in]       in_qInv     Coefficient (inverse of q) mod p
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     */
    /*========================================================================*/
    extern retcode_t
    rsa_CreatePrivKey(
        rsa_key_t   **out_key,
        binstr_t     *in_n,
        binstr_t     *in_e,
        binstr_t     *in_d,
        binstr_t     *in_p,
        binstr_t     *in_q,
        binstr_t     *in_dP,
        binstr_t     *in_dQ,
        binstr_t     *in_qInv);

    /*========================================================================*/
    /**
     * @brief  Frees the RSA Key
     *
     * Frees the resources of an allocated key object
     *
     * @param[in] in_key Pointer to key object
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     */
    /*========================================================================*/
    extern retcode_t
    rsa_FreeKey(rsa_key_t *in_key);

    /*========================================================================*/
    /**
     * @brief  Encrypts using RSA OAEP
     *
     * Performs RSA encryption using OAEP padding scheme
     *
     * @param[in,out] io_rsa  rsa handle
     * @param[in] in_M  message
     * @param[in] in_P  encoding parameter
     * @param[out] out_C cipher text
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     * @return ::ERR_RSA_OUT_OF_RANGE               <br>If the integer message representative m is not between 0 and n-1
     * @return ::ERR_RSA_INVALID                    <br>If the RSA encryption operation fails
     * @return ::ERR_RSA_MSG_TOO_LONG               <br>If the input message is long
     * @return ::ERR_RSA_INTEGER_TOO_LARGE          <br>If the Integer to string pointer conversion fails
     * @return ::ERR_RSA_OTHERS                     <br>If there is some RSA operation error
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsaesOaepEncrypt(rsa_handle_t     io_rsa,
                         const binstr_t  *in_M,
                         const binstr_t  *in_P,
                         binstr_t       **out_C);

    /*========================================================================*/
    /**
     * @brief  Decrypts using RSA OAEP
     *
     * Performs RSA decryption using OAEP padding scheme
     *
     * @param[in,out] io_rsa  rsa handle
     * @param[in] in_C  cipher text
     * @param[in] in_P  encoding parameter
     * @param[out] out_M message
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     * @return ::ERR_RSA_INVALID                    <br>If the encryption operation fails
     * @return ::ERR_RSA_DECRYPTION                 <br>If the decryption fails
     * @return ::ERR_RSA_OTHERS                     <br>If there is some RSA operation error
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsaesOaepDecrypt(rsa_handle_t     io_rsa,
                         const binstr_t  *in_C,
                         const binstr_t  *in_P,
                         binstr_t       **out_M);

    /*========================================================================*/
    /**
     * @brief  Encrypts using RSAES PKCS1 V1.5
     *
     * Performs RSA PKCS1 V1.5 standard encryption
     *
     * @param[in,out] io_rsa  rsa handle
     * @param[in] in_M  message
     * @param[out] out_C cipher text
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_ILLEGALHANDLE                  <br>If an Invalid handle is provided as input to function
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     * @return ::ERR_RSA_OUT_OF_RANGE               <br>If the integer message representative is not between 0 and n-1
     * @return ::ERR_RSA_INVALID                    <br>If the encryption operation fails
     * @return ::ERR_RSA_MSG_TOO_LONG               <br>If the input message is long
     * @return ::ERR_RSA_INTEGER_TOO_LARGE          <br>If the integer to string pointer conversion fails
     * @return ::ERR_RSA_OTHERS                     <br>If there is some RSA operation error
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsaesPkcs1V1_5Encrypt(rsa_handle_t     io_rsa,
                              const binstr_t  *in_M,
                              binstr_t       **out_C);

    /*========================================================================*/
    /**
     * @brief  Decrypts using RSAES PKCS1 V1.5
     *
     * Performs RSA PKCS1 V1.5 standard decryption
     *
     * @param[in,out] io_rsa  rsa handle
     * @param[in] in_C  cipher text
     * @param[out] out_M message
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     * @return ::ERR_RSA_INVALID                    <br>If the encryption operation fails
     * @return ::ERR_RSA_OUT_OF_RANGE               <br>If the integer message representative is not between 0 and n-1
     * @return ::ERR_RSA_DECRYPTION                 <br>Error in RSA decryption
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsaesPkcs1V1_5Decrypt(rsa_handle_t     io_rsa,
                              const binstr_t  *in_C,
                              binstr_t       **out_M);

    /*========================================================================*/
    /**
     * @brief  Generates signature using RSAES PKCS1 V1.5
     *
     * Performs RSA PKCS1 V1.5 standard signature generation
     *
     * @param[in,out] io_rsa  rsa handle
     * @param[in] in_dalgo  digest algorithm
     * @param[in] in_M  message
     * @param[out] out_S signature
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_RSA_OUT_OF_RANGE               <br>If the integer message representative is not between 0 and n-1
     * @return ::ERR_RSA_INVALID                    <br>If the signature generation operation fails
     * @return ::ERR_RSA_INVALID_SIGNATURE          <br>If the signature verification fails
     * @return ::ERR_RSA_MSG_TOO_LONG               <br>If the input message is long
     * @return ::ERR_RSA_INTEGER_TOO_LARGE          <br>If the Integer to string pointer conversion fails
     * @return ::ERR_RSA_MOD_TOO_SHORT              <br>If the modulus used in RSA signature generation is short
     * @return ::ERR_RSA_NOTSUPPORTED               <br>If the requested digest algorithm is not supported
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsassaPkcs1V1_5Sign(rsa_handle_t     io_rsa,
                            int32_t          in_dalgo,
                            const binstr_t  *in_M,
                            binstr_t       **out_S);

    /*========================================================================*/
    /**
     * @brief  Verifies signature using RSAES PKCS1 V1.5
     *
     * Performs RSA PKCS1 V1.5 standard signature verification
     *
     * @param[in,out] io_rsa rsa handle
     * @param[in] in_dalgo digest algorithm
     * @param[in] in_M message
     * @param[in] in_S signature
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
     * @return ::ERR_RSA_MSG_TOO_LONG               <br>If the input message is long
     * @return ::ERR_RSA_MOD_TOO_SHORT              <br>If the modulus used in RSA signature generation is short
     * @return ::ERR_RSA_NOTSUPPORTED               <br>If the requested digest algorithm is not supported
     * @return ::ERR_RSA_INVALID_SIGNATURE          <br>If the signature verification fails
     */
    /*========================================================================*/
    extern retcode_t
    rsa_RsassaPkcs1V1_5Verify(rsa_handle_t    io_rsa,
                              int32_t         in_dalgo,
                              const binstr_t *in_M,
                              const binstr_t *in_S);

#ifdef __cplusplus
}
#endif

#endif /* RSA_API_H */
/** @} */
