/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CERTMNG_API_H
#define CERTMNG_API_H

#include "basic_types_iptves.h"


#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Cert Manager API
 *
 * require: cache, cert, crypto, time, base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define CERTMNG_SRV_SBJ_LEN                     64

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_CERTMNG_CERT_ERR_UNKNOWN            (RET_CERTMNG_BASE|1)
#define RET_CERTMNG_CERT_ERR_FMT                (RET_CERTMNG_BASE|2)
#define RET_CERTMNG_CERT_ERR_TA_FMT             (RET_CERTMNG_BASE|3)
#define RET_CERTMNG_CERT_ERR_TA_VERIFY          (RET_CERTMNG_BASE|4)
#define RET_CERTMNG_CERT_ERR_TA_VALIDITY        (RET_CERTMNG_BASE|5)
#define RET_CERTMNG_CERT_ERR_CA_FMT             (RET_CERTMNG_BASE|6)
#define RET_CERTMNG_CERT_ERR_CA_VERIFY          (RET_CERTMNG_BASE|7)
#define RET_CERTMNG_CERT_ERR_CA_VALIDITY        (RET_CERTMNG_BASE|8)
#define RET_CERTMNG_CERT_ERR_CA_REVOKED         (RET_CERTMNG_BASE|9)
#define RET_CERTMNG_CERT_ERR_SRV_FMT            (RET_CERTMNG_BASE|10)
#define RET_CERTMNG_CERT_ERR_SRV_VERIFY         (RET_CERTMNG_BASE|11)
#define RET_CERTMNG_CERT_ERR_SRV_VALIDITY       (RET_CERTMNG_BASE|12)
#define RET_CERTMNG_CERT_ERR_SRV_REVOKED        (RET_CERTMNG_BASE|13)
#define RET_CERTMNG_CRL_ERR_FMT                 (RET_CERTMNG_BASE|14)
#define RET_CERTMNG_CRL_ERR_VERIFY              (RET_CERTMNG_BASE|15)
#define RET_CERTMNG_MSG_SIG_ERR_FMT             (RET_CERTMNG_BASE|16)
#define RET_CERTMNG_MSG_SIG_ERR_VERIFY          (RET_CERTMNG_BASE|17)
#define RET_CERTMNG_SAVE_ERROR                  (RET_CERTMNG_BASE|18)

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_OK                       Success
 * @retval RET_UNSUPPORTED              There are no cietificate hierarchy in NVS
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CERTMNG_CERT_ERR_FMT     Stored TA or Device Pkipath is corrupted
 * @retval RET_CERTMNG_CRL_ERR_FMT      Stored CRL is corrupted
 */
extern int
certmng_Init(void);

/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_OK                       Success
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
certmng_Fin(void);

/**
 * Generate signature
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_msg               Message
 * @param [in]     in_msg_len           Message length
 * @param [out]    out_sig              Signature
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
certmng_GenSignature(po_thid_t          in_thid,
                     const u_int8_t    *in_msg,
                     u_int32_t          in_msg_len,
                     u_int8_t          *out_sig);

/**
 * Verify signature
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_msg               Message
 * @param [in]     in_msg_len           Message length
 * @param [in]     in_pkipath           Service pkipath
 * @param [in]     in_pkipath_len       Service pkipath length
 * @param [in]     in_sig               Signature
 * @param [out]    out_subjectid        Service subject id
 * @retval RET_OK                               Success
 * @retval RET_ILLEGALARGS                      Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED                      An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR                    An unexpected error has occurred
 * @retval RET_CERTMNG_CERT_ERR_UNKNOWN         Format error
 * @retval RET_CERTMNG_CERT_ERR_FMT             Format error
 * @retval RET_CERTMNG_CERT_ERR_CA_FMT          Format error
 * @retval RET_CERTMNG_CERT_ERR_CA_VERIFY       Fail to verify @p in_pkipath
 * @retval RET_CERTMNG_CERT_ERR_CA_VALIDITY     @p in_pkipath is expired
 * @retval RET_CERTMNG_CERT_ERR_CA_REVOKED      @p in_pkipath is revoked by CRL
 * @retval RET_CERTMNG_CERT_ERR_SRV_FMT         Format error
 * @retval RET_CERTMNG_CERT_ERR_SRV_VERIFY      Fail to verify @p in_pkipath
 * @retval RET_CERTMNG_CERT_ERR_SRV_VALIDITY    @p in_pkipath is expired
 * @retval RET_CERTMNG_CERT_ERR_SRV_REVOKED     @p in_pkipath is revoked by CRL
 * @retval RET_CERTMNG_CRL_ERR_FMT              Fail to verify CRL
 * @retval RET_CERTMNG_MSG_SIG_ERR_FMT          Fail to verify @ in_sig
 * @retval RET_CERTMNG_MSG_SIG_ERR_VERIFY       Fail to verify @ in_sig
 */
extern int
certmng_VerifySignature(po_thid_t       in_thid,
                        const u_int8_t *in_msg,
                        u_int32_t       in_msg_len,
                        const u_int8_t *in_pkipath,
                        u_int32_t       in_pkipath_len,
                        const u_int8_t *in_sig,
                        u_int8_t       *out_subjectid);

/**
 * Get device pkipath
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [out]    out_pkipath          Device pkipath
 * @param [out]    out_pkipath_len      Device pkipath length
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
certmng_GetDevicePkiPath(po_thid_t      in_thid,
                         u_int8_t      *out_pkipath,
                         u_int32_t     *out_pkipath_len);

/**
 * Get device id
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [out]    out_devid            Device id
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 */
extern int
certmng_GetDeviceID(po_thid_t           in_thid,
                    u_int8_t            out_devid[PO_DEVICE_ID_LEN]);

/**
 * Judge CRL should be updated or not
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [out]    out_is_requested     Whether CRL update is requested or not
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
certmng_JudgeCRLUpdate(po_thid_t        in_thid,
                       bool_t          *out_is_requested);

/**
 * Get CRL distribution point
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [out]    out_crl_dp           CRL distribution point (NULL terminated)
 * @retval RET_OK                       Success
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 */
extern int
certmng_GetCRLDstPoint(po_thid_t        in_thid,
                       char             out_crl_dp[PO_MAX_CRL_DP_LEN]);

/**
 * Set CRL
 *
 * @param [in]     in_thid              Trust Hierarchy ID
 * @param [in]     in_crl               CRL
 * @param [in]     in_crl_len           CRL length
 * @param [out]    out_is_updated       Whether CRL is updated or not
 * @retval RET_OK                       Success
 * @retval RET_NOMEM                    Fail to allocate memory
 * @retval RET_ILLEGALARGS              Illegal arguments are passed to this API
 * @retval RET_UNSUPPORTED              An unsupported @p in_thid is passed to thie API
 * @retval RET_INTERNALERROR            An unexpected error has occurred
 * @retval RET_CERTMNG_CRL_ERR_FMT      Fail to parse CRL
 * @retval RET_CERTMNG_CRL_ERR_VERIFY   Fail to verify CRL
 * @retval RET_CERTMNG_SAVE_ERROR       Fail to save read-write NVS
 */
extern int
certmng_SetCRL(po_thid_t                in_thid,
               const u_int8_t          *in_crl,
               u_int32_t                in_crl_len,
               bool_t                  *out_is_updated);

#ifdef __cplusplus
}
#endif

#endif /* CERTMNG_API_H */
