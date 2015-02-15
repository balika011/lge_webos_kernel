/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2004,2005,2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef CRYPTO_TEST_H
#define CRYPTO_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

    /*========================================================================*/
    /** @file
     *  @brief  TEST library - API definitions
     *
     *  @verbatim
     *  dependency: base
     *  @endverbatim
     *
     *  @author Yoshizumi Tanaka (y-tanaka@sm.sony.co.jp)
     *  @date   8 Dec., 2004 modified by y-tanaka
     */
    /*========================================================================*/
#define MAX_DUMP_SIZE    64

#define CRYPTO_DIR_ENC_DEC 0x00000400

    /*------------------------------------------------------------------------*
     * error definition
     *------------------------------------------------------------------------*/

    /*========================================================================*/
    /**
     * @brief  DES (Data Encryption Standard) test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    crypto_test_Init(u_int32_t        in_type,
                     finf_t          *in_iv,
                     finf_t          *in_key,
                     crypto_handle_t *out_handle);

    /*========================================================================*/
    /**
     * @brief  DES (Data Encryption Standard) test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    crypto_test_Encrypt(crypto_handle_t  in_crypto,
                        int32_t          in_loop,
                        finf_t          *in_plain,
                        finf_t          *io_cipher);

    /*========================================================================*/
    /**
     * @brief  DES (Data Encryption Standard) test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    crypto_test_Decrypt(crypto_handle_t  in_crypto,
                        int32_t          in_loop,
                        finf_t          *in_cipher,
                        finf_t          *io_result);

    /*========================================================================*/
    /**
     * @brief  DES (Data Encryption Standard) test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    crypto_test_Check(finf_t *in_plain,
                      finf_t *in_result);

    /*========================================================================*/
    /**
     * @brief  RSA test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    rsa_test(int32_t loop, int32_t keytype, int32_t algo);

    /*========================================================================*/
    /**
     * @brief  SHA test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    sha_test(finf_t *ifinf);

    /*========================================================================*/
    /**
     * @brief  RNG (Rnadom Number Generater) test
     *
     * @return ::ERR_OK
     */
    /*========================================================================*/
    extern retcode_t
    rng_test(void);

    /*========================================================================*/
    /**
     * @brief  HMAC test
     *
     * @return ::ERR_OK
     * @return ::ERR_NOMEM
     */
    /*========================================================================*/
    extern retcode_t
    hmac_test(finf_t *ifinf,
              finf_t *kfinf);


    extern retcode_t
    rand_test(int32_t in_count);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_TEST_H */
