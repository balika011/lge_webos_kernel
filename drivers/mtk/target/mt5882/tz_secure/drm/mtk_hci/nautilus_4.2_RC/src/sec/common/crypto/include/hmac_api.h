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

#ifndef HMAC_API_H
#define HMAC_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*========================================================================*/
    /** @file
     *  @brief  HMAC library - API definitions
     *
     *  @verbatim
     *  dependency: base, binstr
     *  @endverbatim
     *
     *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
     *  @date   2 Dec., 2004 modified by y-tanaka
     */
    /*========================================================================*/

    /*------------------------------------------------------------------------*
     * error definition
     *------------------------------------------------------------------------*/

    typedef enum {
        HMAC_ALGO_SHA1 = 1,  /*  hmac with SHA1    */
        HMAC_ALGO_SHA256     /*  hmac with SHA256  */
    } hmac_algo_t;

    /*========================================================================*/
    /**
     * @brief  Calculates HMAC
     *
     * Calculates SHA1 and SHA256 based HMAC
     *
     * @param[in] in_digest_algo  Digest algorithm to be used,
     *                            i.e.., HMAC_ALGO_SHA1, HMAC_ALGO_SHA256
     * @param[in]  in_key         key
     * @param[in]  in_data        message
     * @param[out] out_hmac       hmac
     *
     * @return ::ERR_OK                             <br>Indicating success
     * @return ::ERR_NOMEM                          <br>A resource failure has occurred
     * @return ::ERR_ILLEGALARGS                    <br>The input arguments are incorrect. Arguments cannot be NULL
     * @return ::ERR_INTERNALERROR                  <br>An unrecoverable error has occurred in Nautilus
     * @return ::ERR_CRYPTO_CRYPTO                  <br>If some crypto error has happend
    */
    /*========================================================================*/
    extern retcode_t
    hmac_Calculate(int32_t         in_digest_algo,
                   const binstr_t  *in_key,
                   const binstr_t  *in_data,
                   binstr_t       **out_hmac);

#ifdef __cplusplus
}
#endif

#endif /* HMAC_API_H */
/** @} */
