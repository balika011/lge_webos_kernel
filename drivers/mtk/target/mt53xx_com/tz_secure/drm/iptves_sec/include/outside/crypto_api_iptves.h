/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CRYPTO_IPTVES_H
#define CRYPTO_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Crypto Library
 *
 * require:
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define CRYPTO_AES_BLOCK_LEN            16
#define CRYPTO_AES_KEY_LEN              16

#define CRYPTO_ECDSA_PRIVKEY_LEN        28
#define CRYPTO_ECDSA_PUBKEY_LEN         56
#define CRYPTO_ECDSA_SIG_LEN            56

#define CRYPTO_ECDH_PRIVKEY_LEN         28
#define CRYPTO_ECDH_PUBKEY_LEN          56
#define CRYPTO_ECDH_PHASE1VALUE_LEN     56

#define CRYPTO_RANDOM_NUMBER_LEN        16

#define CRYPTO_SHA256_DIGEST_LEN        32

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    CRYPTO_MODE_SINK = 0,
    CRYPTO_MODE_SOURCE,
} crypto_mode_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_CRYPTO_OK                   0x00000000
#define RET_CRYPTO_NG                   0x00000001
#define RET_CRYPTO_ILLEGAL_ARGS         0x00000002


/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/

/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 */
extern u_int32_t
crypto_iptves_Init(void);

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 */
extern u_int32_t
crypto_iptves_Fin(void);

/**
 * AES Encryption (CBC mode)
 *
 * @param [in]     in_plain             Plain text
 * @param [in]     in_plain_len         Plain text length
 * @param [in]     in_key               Key
 * @param [in]     in_iv                IV
 * @param [out]    out_cipher           Cipher text
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_AesCbcofbEncrypt(const u_int8_t *in_plain,
                        int32_t         in_plain_len,
                        const u_int8_t  in_key[CRYPTO_AES_KEY_LEN],
                        const u_int8_t  in_iv[CRYPTO_AES_BLOCK_LEN],
                        u_int8_t       *out_cipher);

/**
 * AES Decryption (CBC mode)
 *
 * @param [in]     in_cipher            Cipher text
 * @param [in]     in_cipher_len        Cipher text length
 * @param [in]     in_key               Key
 * @param [in]     in_iv                IV
 * @param [out]    out_plain            Plain text
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_AesCbcofbDecrypt(const u_int8_t *in_cipher,
                        int32_t         in_cipher_len,
                        const u_int8_t  in_key[CRYPTO_AES_KEY_LEN],
                        const u_int8_t  in_iv[CRYPTO_AES_BLOCK_LEN],
                        u_int8_t       *out_plain);

/**
 * Get Random Number
 *
 * @param [in]     in_rnd_len           Random number length
 * @param [out]    out_rnd              Random numbers
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_GetRandomNumber(u_int32_t        in_rnd_len,
                       u_int8_t        *out_rnd);

/**
 * EC-DSA Signature Generation
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [in]     in_privkey           Private Key
 * @param [out]    out_sig              Signature
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_EcdsaGenerate(const u_int8_t    *in_data,
                     int32_t            in_data_len,
                     const u_int8_t     in_privkey[CRYPTO_ECDSA_PRIVKEY_LEN],
                     u_int8_t           out_sig[CRYPTO_ECDSA_SIG_LEN]);

/**
 * EC-DSA Signature Verifycation
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [in]     in_pubkey            Public Key
 * @param [in]     in_sig               Signature
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_EcdsaVerify(const u_int8_t      *in_data,
                   int32_t              in_data_len,
                   const u_int8_t       in_pubkey[CRYPTO_ECDSA_PUBKEY_LEN],
                   const u_int8_t       in_sig[CRYPTO_ECDSA_SIG_LEN]);

/**
 * EC-DH
 *
 * @param [in]     in_svr_p1v           Server Phase 1 Value
 * @param [out]    out_client_p1v       Client Phase 1 Value
 * @param [out]    out_pubkey           Public Key
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_Ecdh(const u_int8_t              in_svr_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            u_int8_t                    out_client_p1v[CRYPTO_ECDH_PHASE1VALUE_LEN],
            u_int8_t                    out_pubkey[CRYPTO_ECDH_PUBKEY_LEN]);

/**
 * SHA-256
 *
 * @param [in]     in_data              Data
 * @param [in]     in_data_len          Data length
 * @param [out] out_hash                Hash value
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_Sha256Hash(const u_int8_t       *in_data,
                  int32_t               in_data_len,
                  u_int8_t              out_hash[CRYPTO_SHA256_DIGEST_LEN]);

/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
/**
 * Global init
 *
 * @retval RET_CRYPTO_OK                Success
 */
extern u_int32_t
crypto_GlobalInit(void);

/**
 * Global fin
 *
 * @retval RET_CRYPTO_OK                Success
 */
extern u_int32_t
crypto_GlobalFin(void);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Clean up context
 *
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Error
 */
extern u_int32_t
crypto_Cleanup(void);

/**
 * Set fixed random number mode
 *
 * @param [in]     in_mode              Mode
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_SetMode(crypto_mode_t            in_mode);

/**
 * Get fixed random number mode
 *
 * @param [out]    out_mode             Mode
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_GetMode(crypto_mode_t           *out_mode);

/**
 * Set fixed random number
 *
 * @param [in]     in_fpath             File path in which fixed random nubmers are written
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_SetFixedRandomNumber(const char *in_fpath);

/**
 * Set fixed random number
 *
 * @param [out]    out_random_number    Fixed random number
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_GetFixedRandomNumber(u_int8_t    out_random_number[CRYPTO_RANDOM_NUMBER_LEN]);

/**
 * Set fixed cast
 *
 * @param [in]     in_cast              Fixed cast (Spots of dice)
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_SetFixedCast(u_int8_t            in_cast);

/**
 * EC-DH Phase 1
 *
 * @param [out]    out_privkey          Private Key
 * @param [out]    out_w                Phase 1 Value
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_Ecdh1(u_int8_t                   in_privkey[CRYPTO_ECDH_PRIVKEY_LEN],
             u_int8_t                   out_w[CRYPTO_ECDH_PHASE1VALUE_LEN]);

/**
 * EC-DH Phase 2
 *
 * @param [in]     in_w                 Phase 1 Value
 * @param [in]     in_privkey           Private Key
 * @param [out]    out_pubkey           Public Key
 * @retval RET_CRYPTO_OK                Success
 * @retval RET_CRYPTO_NG                Fail
 * @retval RET_CRYPTO_ILLEGAL_ARGS      Illegal arguments are passed to this API
 */
extern u_int32_t
crypto_Ecdh2(const u_int8_t             in_w[CRYPTO_ECDH_PHASE1VALUE_LEN],
             const u_int8_t             in_privkey[CRYPTO_ECDH_PRIVKEY_LEN],
             u_int8_t                   out_pubkey[CRYPTO_ECDH_PUBKEY_LEN]);

#endif /* IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_H */
