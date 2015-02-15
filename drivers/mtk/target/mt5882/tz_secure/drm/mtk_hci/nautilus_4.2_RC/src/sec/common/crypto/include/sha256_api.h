/** @addtogroup crypto crypto
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 */

#ifndef SHA256_API_H
#define SHA256_API_H

NP_BEGIN_EXTERN_C

/*========================================================================*/
/** @file
 *  @brief  SHA-256 library - API definitions
 *
 *  @verbatim
 *  dependency: base, mem
 *  @endverbatim
 *
 *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
 *  @date   1 Feb., 2005 modified by y-tanaka
 */
/*========================================================================*/

#define SHA256_DIGESTSIZE 32
#define SHA256_DATASIZE   64

/*------------------------------------------------------------------------*
 * type definition
 *------------------------------------------------------------------------*/
typedef void *sha256_handle_t;

/*------------------------------------------------------------------------*
 * error definition
 *------------------------------------------------------------------------*/

#define ERR_SHA_STATUS               (ERR_SHA_BASE|1)
#define ERR_SHA_DIGEST               (ERR_SHA_BASE|2)

/*========================================================================*/
/**
 * @brief  Initializes SHA-256 context
 *
 * SHA-256 - Initializes new context
 *
 * @param[out] out_sha sha handle
 *
 * @return ::ERR_OK                             <br>Indicating success
 * @return ::ERR_NOMEM                          <br>A resource failure has occurred
 * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
 * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
 */
/*========================================================================*/
extern retcode_t
sha256_Init(sha256_handle_t *out_sha);

/*========================================================================*/
/**
 * @brief  Finalizes SHA-256
 *
 * Adds padding and return the message digest
 *
 * @param[in] in_sha  sha handle
 * @param[out] out_digest digest (unless NULL is passed in)
 *
 * @return ::ERR_OK                             <br>Indicating success
 * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
 * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
 */
/*========================================================================*/
extern retcode_t
sha256_Fin(sha256_handle_t in_sha,
           u_int8_t        out_digest[SHA256_DIGESTSIZE]);

/*========================================================================*/
/**
 * @brief  Updates SHA-256
 *
 * Run your data through this before calling sha256_MakeDigest or sha256_Fin
 *
 * @param[in,out] io_sha sha handle
 * @param[in] in_buf message
 * @param[in] in_len message length
 *
 * @return ::ERR_OK                             <br>Indicating success
 * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
 * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
 */
/*========================================================================*/
extern retcode_t
sha256_Update(sha256_handle_t io_sha,
              const u_int8_t *in_buf,
              u_int32_t       in_len);

/*========================================================================*/
/**
 * @brief  Generates SHA-256 Digest
 *
 * Returns the SHA-256 message digest.
 *
 * @param[in] in_buf  message
 * @param[in] in_len  message length
 * @param[out] out_digest digest
 *
 * @return ::ERR_OK                             <br>Indicating success
 * @return ::ERR_NOMEM                          <br>A resource failure has occurred
 * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
 * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
 */
/*========================================================================*/
extern retcode_t
sha256_MakeDigest(const u_int8_t *in_buf,
                  u_int32_t       in_len,
                  u_int8_t        out_digest[SHA256_DIGESTSIZE]);

NP_END_EXTERN_C

#endif /* SHA256_API_H */
/** @} */
