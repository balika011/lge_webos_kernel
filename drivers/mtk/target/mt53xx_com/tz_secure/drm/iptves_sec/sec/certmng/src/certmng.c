/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "mutex_api_iptves.h"
#include "bptr_api_iptves.h"
#include "time_api_iptves.h"
#include "crypto_api_iptves.h"
#include "cert_api_iptves.h"
#include "cache_api.h"
#include "certmng_api.h"

COMPONENT(CERTMNG);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_CRL_UPDATE_PROB_PAST_SECOND_UPDATE    100
#define L_CRL_UPDATE_PROB_PAST_FIRST_UPDATE     17
#define L_CRL_UPDATE_PROB_LESS_THREE_MONTH      10
#define L_CRL_UPDATE_PROB_MORE_THREE_MONTH      2

#define L_THREE_MONTH                           129600/* 60*24*90 = 129600 */

#define L_CRLFLAG_NONEXISTENT   0
#define L_CRLFLAG_DEFAULT       1
#define L_CRLFLAG_UPDATED       2

#define L_CRLFLAG_LEN           1

#define L_CRL_DP                "http://iptv-crl.marlin-tmo.com/crl/iptvcrls.crl"
#define L_CRL_DP_RTDB           "http://iptv-crl.marlin-tmo.com/crl/rtdbcrls.crl"

#define ATOI(c) (('0' <= (c) && (c) <= '9') ? (c) - '0' :               \
                 ('a' <= (c) && (c) <= 'f') ? (c) - 'a' + 10 : (c) - 'A' + 10)

#define L_GET_CTX(thid)        &l_ctx_tbl[thid]

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    bool_t      is_supported;
    mutex_t     lock;

    /* CACHE_TAG_PKIPATH */
    u_int8_t    devid[PO_DEVICE_ID_LEN];
    char        crl_dp[PO_MAX_CRL_DP_LEN];

    /* CACHE_TAG_CRLFLAG */
    u_int8_t    crl_flag;

    /* CACHE_TAG_CRL */
    bool_t      is_crl_parsed;
    time_unix_t this_update;
    u_int32_t   next_update;
} certmng_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/
static certmng_ctx_t l_ctx_tbl[PO_THID_MAX];

/*============================================================================
 * Local Functions
 *==========================================================================*/
static int
l_ConvErr(int                         in_err,
          int                           in_err_inf)
{
    exh_InitOk();

    switch (in_err) {
    case RET_OK:
        exh_Throw(RET_OK);

    case RET_CRL_ERR_FMT:
    case RET_CRL_ERR_UNSUPPORTED:
        exh_Throw(RET_CERTMNG_CRL_ERR_FMT);

    case RET_CRL_ERR_VERIFY_SIG:
        exh_Throw(RET_CERTMNG_CRL_ERR_VERIFY);

    default:
        break;
    }

    switch (in_err_inf) {
    case 0: /* pkipath */
        switch (in_err) {
        case RET_CERT_ERR_UNSUPPORTED:
        case RET_CERT_ERR_FMT:
            exh_Throw(RET_CERTMNG_CERT_ERR_FMT);

        default:
            exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        }

        break;

    case 1: /* TA*/
        switch (in_err) {
        case RET_CERT_ERR_UNSUPPORTED:
        case RET_CERT_ERR_FMT:
        case RET_CERT_ERR_VERIFY_PUBKEY_FMT:
        case RET_CERT_ERR_VERIFY_SIG_FMT:
            exh_Throw(RET_CERTMNG_CERT_ERR_TA_FMT);

        case RET_CERT_ERR_VERIFY_ISSUER_NAME:
        case RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE:
        case RET_CERT_ERR_VERIFY_ISSUER_BASICCONST:
        case RET_CERT_ERR_VERIFY_SIG:
            exh_Throw(RET_CERTMNG_CERT_ERR_TA_VERIFY);

        case RET_CERT_ERR_VERIFY_VALIDITY:
        case RET_CERT_ERR_VERIFY_ISSUER_VALIDITY:
        case RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY:
            exh_Throw(RET_CERTMNG_CERT_ERR_TA_VALIDITY);

        default:
            exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        }

        break;

    case 2: /* CA */
        switch (in_err) {
        case RET_CERT_ERR_UNSUPPORTED:
        case RET_CERT_ERR_FMT:
        case RET_CERT_ERR_VERIFY_SIG_FMT:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_FMT);

        case RET_CERT_ERR_VERIFY_PUBKEY_FMT:
        case RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE:
        case RET_CERT_ERR_VERIFY_ISSUER_BASICCONST:
            exh_Throw(RET_CERTMNG_CERT_ERR_TA_FMT);

        case RET_CERT_ERR_VERIFY_ISSUER_VALIDITY:
            exh_Throw(RET_CERTMNG_CERT_ERR_TA_VALIDITY);

        case RET_CERT_ERR_VERIFY_ISSUER_NAME:
        case RET_CERT_ERR_VERIFY_SIG:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_VERIFY);

        case RET_CERT_ERR_VERIFY_VALIDITY:
        case RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_VALIDITY);

        case RET_CERT_ERR_REVOKED:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_REVOKED);

        default:
            exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        }

        break;

    case 3: /* SRV */
        switch (in_err) {
        case RET_CERT_ERR_UNSUPPORTED:
        case RET_CERT_ERR_FMT:
            exh_Throw(RET_CERTMNG_CERT_ERR_SRV_FMT);

        case RET_CERT_ERR_VERIFY_PUBKEY_FMT:
        case RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE:
        case RET_CERT_ERR_VERIFY_ISSUER_BASICCONST:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_FMT);

        case RET_CERT_ERR_VERIFY_ISSUER_VALIDITY:
            exh_Throw(RET_CERTMNG_CERT_ERR_CA_VALIDITY);

        case RET_CERT_ERR_VERIFY_SIG_FMT:
        case RET_CERT_ERR_VERIFY_ISSUER_NAME:
        case RET_CERT_ERR_VERIFY_SIG:
            exh_Throw(RET_CERTMNG_CERT_ERR_SRV_VERIFY);

        case RET_CERT_ERR_VERIFY_VALIDITY:
        case RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY:
            exh_Throw(RET_CERTMNG_CERT_ERR_SRV_VALIDITY);

        case RET_CERT_ERR_REVOKED:
            exh_Throw(RET_CERTMNG_CERT_ERR_SRV_REVOKED);

        default:
            exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        }

        break;

    case 4: /* MSG */
        switch (in_err) {
        case RET_CERT_ERR_UNSUPPORTED:
        case RET_CERT_ERR_VERIFY_PUBKEY_FMT:
            exh_Throw(RET_CERTMNG_CERT_ERR_SRV_FMT);

        case RET_CERT_ERR_VERIFY_SIG_FMT:
            exh_Throw(RET_CERTMNG_MSG_SIG_ERR_FMT);

        case RET_CERT_ERR_VERIFY_SIG:
            exh_Throw(RET_CERTMNG_MSG_SIG_ERR_VERIFY);

        default:
            exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        }

        break;

    default:
        exh_Throw(RET_CERTMNG_CERT_ERR_UNKNOWN);
        break;
    }

exh_CLEANUP:
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Initialization/Finalization
 *--------------------------------------------------------------------------*/
static int
l_Init(po_thid_t                        in_thid)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);

    exh_InitOk();
    MSGENT();

    memset(ctx, 0, sizeof(certmng_ctx_t));

    exh_Set(mutex_Init(&ctx->lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Fin(po_thid_t                         in_thid)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);

    exh_InitOk();
    MSGENT();

    exh_Set(mutex_Fin(&ctx->lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    memset(ctx, 0, sizeof(certmng_ctx_t));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * Cache Wrapper
 *--------------------------------------------------------------------------*/
static int
l_GetTA(po_thid_t                       in_thid,
        u_int8_t                      **out_ta,
        u_int32_t                      *out_ta_len)
{
    exh_InitOk();
    MSGENT();

    *out_ta     = NULL;
    *out_ta_len = 0;

    exh_Set(cache_GetObject(in_thid, NULL,
                            CACHE_TAG_TA, CACHE_ACT_REFER,
                            NULL, NULL, out_ta, out_ta_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);
    if (!(*out_ta))
        exh_Throw(RET_UNSUPPORTED);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetDevPkipath(po_thid_t               in_thid,
                u_int8_t              **out_pkipath,
                u_int32_t              *out_pkipath_len)
{
    exh_InitOk();
    MSGENT();

    *out_pkipath     = NULL;
    *out_pkipath_len = 0;

    exh_Set(cache_GetObject(in_thid, NULL,
                            CACHE_TAG_PKIPATH, CACHE_ACT_REFER,
                            NULL, NULL, out_pkipath, out_pkipath_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);
    if (!(*out_pkipath))
        exh_Throw(RET_UNSUPPORTED);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetDevPrivKey(po_thid_t               in_thid,
                u_int8_t                out_privkey[CRYPTO_ECDSA_PRIVKEY_LEN])
{
    u_int8_t *privkey     = out_privkey;
    u_int32_t privkey_len = CRYPTO_ECDSA_PRIVKEY_LEN;

    exh_InitOk();
    MSGENT();

    exh_Set(cache_GetObject(in_thid, NULL,
                            CACHE_TAG_PRIVKEY, CACHE_ACT_COPY,
                            NULL, NULL, &privkey, &privkey_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);
    if (!privkey)
        exh_Throw(RET_UNSUPPORTED);
    if (privkey_len != CRYPTO_ECDSA_PRIVKEY_LEN)
        exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetCRLFlag(po_thid_t                  in_thid)
{
    certmng_ctx_t *ctx          = L_GET_CTX(in_thid);
    u_int8_t       buf[L_CRLFLAG_LEN];
    u_int8_t      *crl_flag     = buf;
    u_int32_t      crl_flag_len = L_CRLFLAG_LEN;

    exh_InitOk();
    MSGENT();

    exh_Set(cache_GetObject(in_thid, NULL,
                            CACHE_TAG_CRLFLAG, CACHE_ACT_COPY,
                            NULL, NULL, &crl_flag, &crl_flag_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);

    ctx->crl_flag = crl_flag ? *crl_flag : L_CRLFLAG_NONEXISTENT;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_SetCRLFlag(po_thid_t                  in_thid,
             cache_tkt_t               *io_tkt)
{
    certmng_ctx_t *ctx      = L_GET_CTX(in_thid);
    u_int8_t       crl_flag = ctx->crl_flag = L_CRLFLAG_UPDATED;

    exh_InitOk();
    MSGENT();

    exh_Set(cache_SetObject(in_thid, io_tkt,
                            CACHE_TAG_CRLFLAG, CACHE_ATTR_READWRITE,
                            NULL, NULL, &crl_flag, sizeof(u_int8_t)));
    if (!exh_IsOk()) {
        if (exh_Get() == RET_NOMEM)
            exh_Rethrow();
        else
            exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetCRL(po_thid_t                      in_thid,
         u_int8_t                     **out_crl,
         u_int32_t                     *out_crl_len)
{
    exh_InitOk();
    MSGENT();

    exh_Set(cache_GetObject(in_thid, NULL,
                            CACHE_TAG_CRL, CACHE_ACT_COPY,
                            NULL, NULL, out_crl, out_crl_len));
    if (!exh_IsOk())
        exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_SetCRL(po_thid_t                      in_thid,
         cache_tkt_t                   *io_tkt,
         const u_int8_t                *in_crl,
         u_int32_t                      in_crl_len)
{
    exh_InitOk();
    MSGENT();

    exh_Set(cache_SetObject(in_thid, io_tkt,
                            CACHE_TAG_CRL, CACHE_ATTR_READWRITE,
                            NULL, NULL, in_crl, in_crl_len));
    if (!exh_IsOk()) {
        if (exh_Get() == RET_NOMEM)
            exh_Rethrow();
        else
            exh_Throw(RET_INTERNALERROR);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * X.509 Parse/Verification
 *--------------------------------------------------------------------------*/
static int
l_VerifyPkipath(po_thid_t               in_thid,
                u_int32_t               in_type,
                const u_int8_t         *in_pkipath,
                u_int32_t               in_pkipath_len,
                cert_inf_t             *out_inf)
{
    u_int8_t      *ta;
    u_int32_t      ta_len;
    u_int8_t       buf[PO_MAX_CRL_LEN];
    u_int8_t      *crl     = buf;
    u_int32_t      crl_len = PO_MAX_CRL_LEN;
    time_unix_t    cur_time;
    int            err_inf;

    exh_InitOk();
    MSGENT();

    exh_Set(time_GetSystemTime(&cur_time));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
    exh_Check(l_GetTA(in_thid, &ta, &ta_len));
    exh_Check(l_GetCRL(in_thid, &crl, &crl_len));

    exh_Set(cert_VerifyCert(in_type, in_pkipath, in_pkipath_len, ta, ta_len,
                            &cur_time, out_inf, crl, crl_len, &err_inf));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get(), err_inf));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifySignature(const cert_inf_t     *in_inf,
                  const u_int8_t       *in_msg,
                  u_int32_t             in_msg_len,
                  const u_int8_t       *in_sig,
                  u_int32_t             in_sig_len)
{
    exh_InitOk();
    MSGENT();

    exh_Set(cert_VerifySignature(in_inf, in_msg, in_msg_len, in_sig, in_sig_len));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get(), 4));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifyCRL(po_thid_t                   in_thid,
            const u_int8_t             *in_crl,
            u_int32_t                   in_crl_len,
            crl_inf_t                  *io_inf)
{
    u_int8_t  *ta;
    u_int32_t  ta_len;
    u_int32_t  cur_time;

    exh_InitOk();
    MSGENT();

    exh_Set(time_GetTrustedTime(&cur_time));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

	dprintf("%s: in_thid:%d\n", __FUNCTION__, in_thid);

	
    exh_Check(l_GetTA(in_thid, &ta, &ta_len));
    exh_Set(cert_VerifyCRL(in_crl, in_crl_len, ta, ta_len, &cur_time, io_inf));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get(), 1));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseCRL(po_thid_t                    in_thid)
{
    certmng_ctx_t *ctx     = L_GET_CTX(in_thid);
    u_int8_t       buf[PO_MAX_CRL_LEN];
    u_int8_t      *crl     = buf;
    u_int32_t      crl_len = PO_MAX_CRL_LEN;
    crl_inf_t      crl_inf;

    exh_InitOk();
    MSGENT();

    if (!ctx->is_crl_parsed) {
        exh_Check(l_GetCRLFlag(in_thid));
        exh_Check(l_GetCRL(in_thid, &crl, &crl_len));

        if (crl) {
            if (ctx->crl_flag == L_CRLFLAG_NONEXISTENT)
                ctx->crl_flag = L_CRLFLAG_DEFAULT;

            exh_Set(cert_VerifyCRL(crl, crl_len, NULL, 0, NULL, &crl_inf));
            if (!exh_IsOk()) exh_Throw(RET_CERTMNG_CRL_ERR_FMT);

            ctx->this_update = crl_inf.this_update;
            ctx->next_update = crl_inf.next_update;
        }

        ctx->is_crl_parsed = TRUE;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseDevPkipath(po_thid_t             in_thid)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    u_int8_t      *pkipath;
    u_int32_t      pkipath_len;
    u_int8_t      *devid;
    cert_inf_t     cert_inf;
    int            i;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetDevPkipath(in_thid, &pkipath, &pkipath_len));
    exh_Set(l_VerifyPkipath(in_thid, CERT_TYPE_DEVPKIPATH,
                            pkipath, pkipath_len, &cert_inf));
    if (!exh_IsOk()) exh_Throw(RET_CERTMNG_CERT_ERR_FMT);

    if (cert_inf.type != CERT_INF_NAME_TYPE_NEMO_DEVID ||
        cert_inf.subject_len != (PO_DEVICE_ID_LEN << 1))
        exh_Throw(RET_CERTMNG_CERT_ERR_FMT);

    devid = cert_inf.subject;

    /* cache device id */
    for (i = 0; i < PO_DEVICE_ID_LEN; i++) {
        ctx->devid[i] = (u_int8_t)((ATOI(devid[0])<<4)|ATOI(devid[1]));
        devid += 2;
    }

    /* cache crl distribution point */
    if (in_thid == PO_THID_COMMON)
        exh_Check(base_Strncpy(ctx->crl_dp, PO_MAX_CRL_DP_LEN,
                               L_CRL_DP, PO_MAX_CRL_DP_LEN - 1));
    else if (in_thid == PO_THID_RTDB)
        exh_Check(base_Strncpy(ctx->crl_dp, PO_MAX_CRL_DP_LEN,
                               L_CRL_DP_RTDB, PO_MAX_CRL_DP_LEN - 1));
    else
        exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*----------------------------------------------------------------------------
 * CRL Update
 *--------------------------------------------------------------------------*/
static int
l_GetCRLUpdateProb(po_thid_t            in_thid,
                   u_int32_t           *out_prob)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    u_int32_t      next_update;
    u_int32_t      cur_time;
    u_int32_t      prob;

    exh_InitOk();
    MSGENT();

    if (ctx->crl_flag == L_CRLFLAG_NONEXISTENT) {
        prob = L_CRL_UPDATE_PROB_PAST_FIRST_UPDATE;
    } else {
        exh_Set(time_GetTrustedTime(&cur_time));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

        next_update = ctx->next_update;

        if (next_update < cur_time) {
            if (ctx->crl_flag == L_CRLFLAG_DEFAULT) {
                prob = L_CRL_UPDATE_PROB_PAST_FIRST_UPDATE;
            } else {
                prob = L_CRL_UPDATE_PROB_PAST_SECOND_UPDATE;
            }
        } else if (next_update < cur_time + L_THREE_MONTH) {
            prob = L_CRL_UPDATE_PROB_LESS_THREE_MONTH;
        } else {
            prob = L_CRL_UPDATE_PROB_MORE_THREE_MONTH;
        }
    }

    *out_prob = prob;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GetRandomNumber(u_int32_t            *out_random)
{
    u_int8_t random[1];

    exh_InitOk();
    MSGENT();

    do {
        exh_Set(crypto_GetRandomNumber(1, random));
        if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
    } while (random[0] >= 100);

    *out_random = random[0];

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
certmng_Init(void)
{
    po_thid_t thid;
    bool_t    is_initialized = FALSE;

    exh_InitOk();
    MSGENT();

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        certmng_ctx_t *ctx = L_GET_CTX(thid);
        u_int8_t      *ta;
        u_int32_t      ta_len;

        exh_Check(l_Init(thid));

        exh_Set(l_GetTA(thid, &ta, &ta_len));
        if (exh_IsOk()) {
            exh_Check(l_ParseDevPkipath(thid));
            exh_Check(l_ParseCRL(thid));

            ctx->is_supported = TRUE;
            is_initialized    = TRUE;
        } else if (exh_Get() == RET_UNSUPPORTED) {
            CLEARLOG();
            exh_SetOk();

            ctx->is_supported = FALSE;
        } else {
            exh_Rethrow();
        }
    }
    if (!is_initialized)
        exh_Throw(RET_UNSUPPORTED);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_Fin(void)
{
    po_thid_t thid;

    exh_InitOk();
    MSGENT();

    for (thid = PO_THID_COMMON; thid < PO_THID_MAX; thid++) {
        exh_Check(l_Fin(thid));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_GenSignature(po_thid_t          in_thid,
                     const u_int8_t    *in_msg,
                     u_int32_t          in_msg_len,
                     u_int8_t          *out_sig)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    u_int8_t       privkey[CRYPTO_ECDSA_PRIVKEY_LEN];

    exh_InitOk();
    MSGENT();

    if (!in_msg || in_msg_len == 0 || !out_sig)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Check(l_GetDevPrivKey(in_thid, privkey));

    exh_Set(crypto_EcdsaGenerate(in_msg, in_msg_len, privkey, out_sig));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_VerifySignature(po_thid_t       in_thid,
                        const u_int8_t *in_msg,
                        u_int32_t       in_msg_len,
                        const u_int8_t *in_pkipath,
                        u_int32_t       in_pkipath_len,
                        const u_int8_t *in_sig,
                        u_int8_t       *out_subjectid)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    cert_inf_t     cert_inf;

    exh_InitOk();
    MSGENT();

    if (!in_msg || in_msg_len == 0 || !in_pkipath || in_pkipath_len == 0 || !in_sig)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Check(l_VerifyPkipath(in_thid, CERT_TYPE_SRVPKIPATH,
                              in_pkipath, in_pkipath_len, &cert_inf));
    exh_Check(l_VerifySignature(&cert_inf, in_msg, in_msg_len, in_sig, 56));

    if (cert_inf.subject_len > CERTMNG_SRV_SBJ_LEN)
        exh_Check(RET_CERTMNG_CERT_ERR_FMT);
    if (out_subjectid) {
        exh_Check(base_Memcpy(out_subjectid, cert_inf.subject_len,
                              cert_inf.subject, cert_inf.subject_len));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_GetDevicePkiPath(po_thid_t      in_thid,
                         u_int8_t      *out_pkipath,
                         u_int32_t     *out_pkipath_len)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    u_int8_t      *pkipath;
    u_int32_t      pkipath_len;

    exh_InitOk();
    MSGENT();

    if (!out_pkipath || !out_pkipath_len)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Check(l_GetDevPkipath(in_thid, &pkipath, &pkipath_len));

    exh_Check(base_Memcpy(out_pkipath, pkipath_len, pkipath, pkipath_len));
    *out_pkipath_len = pkipath_len;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_GetDeviceID(po_thid_t           in_thid,
                    u_int8_t            out_devid[PO_DEVICE_ID_LEN])
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);

    exh_InitOk();
    MSGENT();

    if (!out_devid)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Check(base_Memcpy(out_devid, PO_DEVICE_ID_LEN,
                          ctx->devid, PO_DEVICE_ID_LEN));

    MSGI3(("Device ID: ")); BDUMP3(PO_DEVICE_ID_LEN, out_devid); MSG3(("\n"));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_JudgeCRLUpdate(po_thid_t        in_thid,
                       bool_t          *out_is_requested)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    u_int32_t      prob;
    u_int32_t      random;
    bool_t         is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (!out_is_requested)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Set(mutex_Lock(&ctx->lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    is_locked = TRUE;

    exh_Check(l_GetCRLUpdateProb(in_thid, &prob));
    exh_Check(l_GetRandomNumber(&random));

    *out_is_requested = random < prob;

    MSGI2(("Probability  : [%3d]\n", prob));
    MSGI2(("Random       : [%3d]\n", random));

exh_CLEANUP:
    if (is_locked) mutex_Unlock(&ctx->lock);

    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_GetCRLDstPoint(po_thid_t        in_thid,
                       char             out_crl_dp[PO_MAX_CRL_DP_LEN])
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);

    exh_InitOk();
    MSGENT();

    if (!out_crl_dp)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

    exh_Check(base_Strncpy(out_crl_dp, PO_MAX_CRL_DP_LEN,
                           ctx->crl_dp, PO_MAX_CRL_DP_LEN - 1));

    MSGI3(("CRL Disrtibution Point: %s\n", out_crl_dp));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
certmng_SetCRL(po_thid_t                in_thid,
               const u_int8_t          *in_crl,
               u_int32_t                in_crl_len,
               bool_t                  *out_is_updated)
{
    certmng_ctx_t *ctx = L_GET_CTX(in_thid);
    cache_tkt_t    ticket;
    crl_inf_t      crl_inf;
    bool_t         is_updated;
    bool_t         is_locked = FALSE;
    bool_t         is_in_transaction = FALSE;

    exh_InitOk();
    MSGENT();

    if (!in_crl || in_crl_len == 0 || !out_is_updated)
        exh_Check(RET_ILLEGALARGS);
    if (!ctx->is_supported)
        exh_Check(RET_UNSUPPORTED);

	dprintf("%s: in_thid:%d\n", __FUNCTION__, in_thid);

    exh_Check(l_VerifyCRL(in_thid, in_crl, in_crl_len, &crl_inf));

    exh_Set(mutex_Lock(&ctx->lock));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    is_locked = TRUE;

    exh_Check(cache_Begin(&ticket)); is_in_transaction = TRUE;

    if ((ctx->crl_flag == L_CRLFLAG_NONEXISTENT) ||
        (ctx->crl_flag == L_CRLFLAG_DEFAULT &&
         ctx->this_update < crl_inf.this_update) ||
        (ctx->crl_flag == L_CRLFLAG_UPDATED &&
         ctx->this_update < crl_inf.this_update)) {
        ctx->this_update = crl_inf.this_update;
        ctx->next_update = crl_inf.next_update;

        exh_Check(l_SetCRL(in_thid, &ticket, in_crl, in_crl_len));
        exh_Check(l_SetCRLFlag(in_thid, &ticket));

        is_updated = TRUE;

        MSGI2(("CRL is updated!\n"));
    } else {
        if (ctx->crl_flag == L_CRLFLAG_DEFAULT &&
            ctx->this_update == crl_inf.this_update) {
            exh_Check(l_SetCRLFlag(in_thid, &ticket));

            MSGI2(("CRL Flag is changed (DEFAULT -> UPDATED).\n"));
        }

        is_updated = FALSE;

        MSGI2(("CRL is not update...\n"));
    }

    *out_is_updated = is_updated;

exh_CLEANUP:
    if (is_in_transaction) {
        int ret = exh_Get();

        exh_Set(cache_Commit(&ticket, exh_IsOk()));
        if (ret == RET_OK) {
            switch (exh_Get()) {
            case RET_OK:
            case RET_NOMEM:
                break;
            case RET_CACHE_SAVE_ERROR:
                exh_Set(RET_CERTMNG_SAVE_ERROR);
                break;
            default:
                exh_Set(RET_INTERNALERROR);
                break;
            }
        } else {
            /* in this case, cache_Commit should not fail */
            exh_Set(ret);
        }
    }

    if (is_locked) mutex_Unlock(&ctx->lock);

    MSGEXT(exh_Get());
    exh_Return();
}
