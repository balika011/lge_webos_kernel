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
#include "time_api_iptves.h"
#include "crypto_api_iptves.h"
#include "rend_api_iptves.h"
#include "cache_api.h"
#include "certmng_api.h"
#include "sdi_api.h"

COMPONENT(SDI);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/
static int
l_ConvErr(int                         in_err)
{
    exh_InitOk();
    exh_Set(in_err);

    switch (exh_Get()) {
    case RET_OK:
    case RET_NOMEM:
    case RET_UNSUPPORTED:
        exh_Rethrow();

    /* cache error */
    case RET_CACHE_RODATA_CORRUPTED:
        exh_Throw(RET_SDI_RODATA_CORRUPTED);

    case RET_CACHE_LOAD_ERROR:
    case RET_CACHE_PARSE_ERROR:
        exh_Throw(RET_SDI_LOAD_ERROR);

    case RET_CACHE_SAVE_ERROR:
        exh_Throw(RET_SDI_SAVE_ERROR);

    case RET_CACHE_REMOVE_ERROR:
        exh_Throw(RET_SDI_REMOVE_ERROR);

    /* certmng error */
    case RET_CERTMNG_CERT_ERR_UNKNOWN:
    case RET_CERTMNG_CERT_ERR_FMT:
    case RET_CERTMNG_CERT_ERR_TA_FMT:
    case RET_CERTMNG_CERT_ERR_TA_VERIFY:
    case RET_CERTMNG_CERT_ERR_CA_FMT:
    case RET_CERTMNG_CERT_ERR_CA_VERIFY:
    case RET_CERTMNG_CERT_ERR_SRV_FMT:
    case RET_CERTMNG_CERT_ERR_SRV_VERIFY:
        exh_Throw(RET_SDI_CERT_VERIFY_FAILED);

    case RET_CERTMNG_CERT_ERR_TA_VALIDITY:
    case RET_CERTMNG_CERT_ERR_CA_VALIDITY:
    case RET_CERTMNG_CERT_ERR_SRV_VALIDITY:
        exh_Throw(RET_SDI_CERT_VALIDITY_ERROR);

    case RET_CERTMNG_CERT_ERR_CA_REVOKED:
    case RET_CERTMNG_CERT_ERR_SRV_REVOKED:
        exh_Throw(RET_SDI_CERT_REVOKED);

    case RET_CERTMNG_CRL_ERR_FMT:
    case RET_CERTMNG_CRL_ERR_VERIFY:
        exh_Throw(RET_SDI_CRL_VERIFY_FAILED);

    case RET_CERTMNG_MSG_SIG_ERR_FMT:
    case RET_CERTMNG_MSG_SIG_ERR_VERIFY:
        exh_Throw(RET_SDI_SIG_VERIFY_FAILED);

    case RET_CERTMNG_SAVE_ERROR:
        exh_Throw(RET_SDI_SAVE_ERROR);

    default:
        exh_Throw(RET_INTERNALERROR);
        break;
    }

exh_CLEANUP:
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
sdi_Init(int current_secs, int gmtoff)
{
    exh_InitOk();
    MSGENT();


    exh_Set(crypto_iptves_Init());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
	//dprintf("crypto_Init init ok\n");

    exh_Set(time_Init(current_secs, gmtoff));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
	//dprintf("time_Init init ok\n");


    exh_Set(rend_Init());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);
	//dprintf("rend_Init init ok\n");



    exh_Set(cache_Init());
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));
	//dprintf("cache_Init init ok\n");


    exh_Set(certmng_Init());
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));
	//dprintf("certmng_Init init ok\n");

	dbg_Init();
	dprintf("dbg_Init init ok\n");


exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_Fin(void)
{
    exh_InitOk();
    MSGENT();

	dprintf(">>%s\n", __FUNCTION__);

    exh_Set(certmng_Fin());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);


    exh_Set(cache_Fin());
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));


    exh_Set(rend_Fin());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);


    exh_Set(time_Fin());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);


    exh_Set(crypto_iptves_Fin());
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);


	dbg_Fin();
	dprintf("dbg_Fin init ok\n");

	
exh_CLEANUP:
    MSGEXT(exh_Get());

	
    exh_Return();
}

int
sdi_CleanupNVS(void)
{
    exh_InitOk();
    MSGENT();

    exh_Set(cache_CleanupNVS());
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_GetDeviceID(po_thid_t               in_thid,
                u_int8_t                *out_devid)
{
    exh_InitOk();
    MSGENT();

    if (!out_devid)
        exh_Throw(RET_ILLEGALARGS);
	
	


    exh_Set(certmng_GetDeviceID(in_thid, out_devid));

	dprintf("\n>>%s\n", __FUNCTION__);
	DumpHex("device_id:", out_devid, PO_DEVICE_ID_LEN);
	
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));
	

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_VerifyServiceMetaData(po_thid_t       in_thid,
                          const u_int8_t *in_msg,
                          u_int32_t       in_msg_len,
                          const u_int8_t *in_pkipath,
                          u_int32_t       in_pkipath_len,
                          const u_int8_t *in_sig)
{
    exh_InitOk();
    MSGENT();


    if (!in_msg || in_msg_len == 0 || !in_pkipath || in_pkipath_len == 0 || !in_sig)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(certmng_VerifySignature(in_thid,
                                    in_msg,
                                    in_msg_len,
                                    in_pkipath,
                                    in_pkipath_len,
                                    in_sig,
                                    NULL));
    if (!exh_IsOk()) exh_Check(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_JudgeCRLUpdate(po_thid_t            in_thid,
                   bool_t              *out_is_requested)
{
    exh_InitOk();
    MSGENT();

    if (!out_is_requested)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(certmng_JudgeCRLUpdate(in_thid, out_is_requested));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_GetCRLDstPoint(po_thid_t            in_thid,
                   char                 out_crl_dp[PO_MAX_CRL_DP_LEN])
{
    exh_InitOk();
    MSGENT();

    if (!out_crl_dp)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(certmng_GetCRLDstPoint(in_thid, out_crl_dp));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_SetCRL(po_thid_t                    in_thid,
           const u_int8_t              *in_crl,
           u_int32_t                    in_crl_len,
           bool_t                      *out_is_updated)
{
    exh_InitOk();
    MSGENT();

    if (!in_crl || in_crl_len == 0 || !out_is_updated)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(certmng_SetCRL(in_thid, in_crl, in_crl_len, out_is_updated));
    if (!exh_IsOk()) exh_Throw(l_ConvErr(exh_Get()));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
sdi_CheckTrustedTime(bool_t            *out_is_valid)
{
    u_int32_t tt;
    bool_t    is_valid;

    exh_InitOk();
    MSGENT();

    if (!out_is_valid)
        exh_Throw(RET_ILLEGALARGS);

    exh_Set(time_GetTrustedTime(&tt));
    if (exh_IsOk()) {
        is_valid = TRUE;
    } else if (exh_Get() == RET_TIME_INVALID) {
        CLEARLOG();
        exh_SetOk();
        is_valid = FALSE;
    } else {
        exh_Throw(RET_INTERNALERROR);
    }

    *out_is_valid = is_valid;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
