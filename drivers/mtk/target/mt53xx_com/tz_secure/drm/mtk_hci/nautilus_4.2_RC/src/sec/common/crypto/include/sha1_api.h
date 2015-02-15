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

#ifndef SHA1_API_H
#define SHA1_API_H

NP_BEGIN_EXTERN_C

/*========================================================================*/
/** @file
 *  @brief  SHA-1 library - API definitions
 *
 *  @verbatim
 *  dependency: base, mem
 *  @endverbatim
 *
 *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
 *  @date   1 Feb., 2005 modified by y-tanaka
 */
/*========================================================================*/

#define SHA1_DIGESTSIZE    20
#define SHA1_DATASIZE      64

/*------------------------------------------------------------------------*
 * type definition
 *------------------------------------------------------------------------*/
typedef void *sha1_handle_t;

/*------------------------------------------------------------------------*
 * error definition
 *------------------------------------------------------------------------*/

#define ERR_SHA_STATUS               (ERR_SHA_BASE|1)
#define ERR_SHA_DIGEST               (ERR_SHA_BASE|2)

/*========================================================================*/
/**
 * @brief  Initializes SHA context
 *
 * SHA1Init - Initialize new context
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
sha1_Init(sha1_handle_t *out_sha);

/*========================================================================*/
/**
 * @brief  Finalizes SHA
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
sha1_Fin(sha1_handle_t in_sha,
         u_int8_t      out_digest[SHA1_DIGESTSIZE]);

/*========================================================================*/
/**
 * @brief  Updates SHA
 *
 * Run your data through this before calling sha1_MakeDigest or sha1_Fin
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
sha1_Update(sha1_handle_t   io_sha,
            const u_int8_t *in_buf,
            u_int32_t       in_len);

/*========================================================================*/
/**
 * @brief  Generates Digest
 *
 * Returns the SHA-1 message digest.
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
sha1_MakeDigest(const u_int8_t *in_buf,
                u_int32_t       in_len,
                u_int8_t        out_digest[SHA1_DIGESTSIZE]);

NP_END_EXTERN_C

#endif /* SHA1_API_H */
/** @} */
