/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef CERT_API_H
#define CERT_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * X509 Cert library
 *
 * require: asn, crypto, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define CERT_INF_ID_MAX_SZ      256

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int8_t    type;
    u_int8_t    subject[CERT_INF_ID_MAX_SZ];
    bptr_t      pubkey;
    size_t      subject_len;
} cert_inf_t;

typedef struct {
    time_unix_t this_update;
    u_int32_t   next_update;
} crl_inf_t;

enum cert_name_type {
    CERT_INF_NAME_TYPE_NULL = 0,
    CERT_INF_NAME_TYPE_NEMO_URI,
    CERT_INF_NAME_TYPE_NEMO_DEVID,
};

enum cert_type {
    CERT_TYPE_SRVPKIPATH        = 0,    /* service pkipath */
    CERT_TYPE_DEVPKIPATH        = 1,    /* device pkipath */
};

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_CERT_ERR_UNSUPPORTED                (RET_CERT_BASE|1)
#define RET_CERT_ERR_FMT                        (RET_CERT_BASE|2)
#define RET_CERT_ERR_VERIFY_SIG                 (RET_CERT_BASE|3)
#define RET_CERT_ERR_VERIFY_SIG_FMT             (RET_CERT_BASE|4)
#define RET_CERT_ERR_VERIFY_PUBKEY_FMT          (RET_CERT_BASE|5)
#define RET_CERT_ERR_VERIFY_VALIDITY            (RET_CERT_BASE|6)
#define RET_CERT_ERR_VERIFY_ISSUER_NAME         (RET_CERT_BASE|7)
#define RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE     (RET_CERT_BASE|8)
#define RET_CERT_ERR_VERIFY_ISSUER_BASICCONST   (RET_CERT_BASE|9)
#define RET_CERT_ERR_VERIFY_ISSUER_VALIDITY     (RET_CERT_BASE|10)
#define RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY    (RET_CERT_BASE|11)
#define RET_CERT_ERR_REVOKED                    (RET_CERT_BASE|12)
#define RET_CRL_ERR_UNSUPPORTED                 (RET_CERT_BASE|13)
#define RET_CRL_ERR_FMT                         (RET_CERT_BASE|14)
#define RET_CRL_ERR_VERIFY_SIG                  (RET_CERT_BASE|15)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Verify cert (pkipath)
 *
 * @param [in]     in_type              Pkipath type (service/device)
 * @param [in]     in_certs             Pkipath
 * @param [in]     in_certs_len         Pkipath length
 * @param [in]     in_ta                Trust anchor
 * @param [in]     in_ta_len            Trust anchor length
 * @param [in]     in_time              Current time
 * @param [in,out] io_inf               Cert information
 * @param [in]     in_crl               CRL
 * @param [in]     in_crl_len           CRL length
 * @param [in,out] io_err_inf           Error information
 * @retval RET_OK                       Success
 * @retval RET_CERT_ERR_UNSUPPORTED
 * @retval RET_CERT_ERR_FMT
 * @retval RET_CERT_ERR_VERIFY_ISSUER_NAME
 * @retval RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE
 * @retval RET_CERT_ERR_VERIFY_ISSUER_BASICCONST
 * @retval RET_CERT_ERR_VERIFY_ISSUER_VALIDITY
 * @retval RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY
 * @retval RET_CERT_ERR_REVOKED
 * @retval RET_CRL_ERR_UNSUPPORTED
 * @retval RET_CRL_ERR_FMT
 */
extern int
cert_VerifyCert(u_int32_t               in_type,
                const u_int8_t         *in_certs,
                u_int32_t               in_certs_len,
                const u_int8_t         *in_ta,
                u_int32_t               in_ta_len,
                time_unix_t            *in_time,
                cert_inf_t             *io_inf,
                const u_int8_t         *in_crl,
                u_int32_t               in_crl_len,
                int                    *io_err_inf);

/**
 * Verify signature
 *
 * @param [in]     in_inf               Cert information
 * @param [in]     in_msg               Message
 * @param [in]     in_msg_len           Message length
 * @param [in]     in_sig               Signature
 * @param [in]     in_sig_len           Signature length
 * @retval RET_OK                       Success
 * @retval RET_CERT_ERR_UNSUPPORTED
 * @retval RET_CERT_ERR_VERIFY_SIG
 * @retval RET_CERT_ERR_VERIFY_SIG_FMT
 * @retval RET_CERT_ERR_VERIFY_PUBKEY_FMT
 */
extern int
cert_VerifySignature(const cert_inf_t  *in_inf,
                     const u_int8_t    *in_msg,
                     size_t             in_msg_len,
                     const u_int8_t    *in_sig,
                     size_t             in_sig_len);

/**
 * Verify CRL
 *
 * @param [in]     in_crl               CRL
 * @param [in]     in_crl_len           CRL length
 * @param [in]     in_ta                Trust anchor
 * @param [in]     in_ta_len            Trust anchor length
 * @param [in]     in_time              Current time
 * @param [out]    out_inf              CRL information
 * @retval RET_OK                       Success
 * @retval RET_CERT_ERR_UNSUPPORTED
 * @retval RET_CERT_ERR_FMT
 * @retval RET_CRL_ERR_UNSUPPORTED
 * @retval RET_CRL_ERR_FMT
 * @retval RET_CRL_ERR_VERIFY_SIG
 */
extern int
cert_VerifyCRL(const u_int8_t          *in_crl,
               u_int32_t                in_crl_len,
               const u_int8_t          *in_ta,
               u_int32_t                in_ta_len,
               u_int32_t               *in_time,
               crl_inf_t               *out_inf);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
extern void
cert_PrintCertInf(cert_inf_t           *in_inf);

extern void
cert_PrintCRLInf(crl_inf_t             *in_inf);

#endif /* !IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* CERT_API_H */
