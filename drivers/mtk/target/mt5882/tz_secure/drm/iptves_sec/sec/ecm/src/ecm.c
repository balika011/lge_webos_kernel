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
#include "byte_api_iptves.h"
#include "time_api_iptves.h"
#include "crypto_api_iptves.h"
#include "rend_api_iptves.h"
#include "keymng_api.h"
#include "ecm_api.h"

COMPONENT(ECM);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_PROTOCOL_VER  0x0100
#define L_ENCDATA_LEN   (PO_ECM_LEN - (sizeof(u_int16_t) + PO_CKEY_ID_LEN + 10))

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef struct {
    u_int16_t protocol_ver;                 /**< ProtocolVersion */
    u_int8_t  wkey_id[PO_CKEY_ID_LEN];      /**< WorkKeyID */
    u_int32_t datetime1;                    /**< Datetime */
    u_int8_t  tierbits1[PO_TIERBITS_LEN];   /**< ChannelTierBits */
    u_int16_t ro1;                          /**< RenderingObligation */
    u_int8_t  skey_odd[PO_CKEY_LEN];        /**< ScrambleKey (odd) */
    u_int32_t datetime2;                    /**< Datetime */
    u_int8_t  tierbits2[PO_TIERBITS_LEN];   /**< ChannelTierBits */
    u_int16_t ro2;                          /**< RenderingObligation */
    u_int8_t  skey_even[PO_CKEY_LEN];       /**< ScrambleKey (even) */
} ecm_inf_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/
#if defined(ENABLE_DEBUG_FUNCTION)
static void
l_PrintECMInf(ecm_inf_t        *in_ecm_inf)
{
    MSGI2(("===== ECM Information (ecm_inf_t) ========================\n"));

    MSGI2((" ProtocolVersion    : 0x%04x\n", in_ecm_inf->protocol_ver));
    MSGI2((" WorkKeyID          : ")); BDUMP2(PO_CKEY_ID_LEN, in_ecm_inf->wkey_id); MSG2(("\n"));
    MSGI2((" Datetime1          : 0x%08x, %s\n",
           in_ecm_inf->datetime1, base_CMintime(&in_ecm_inf->datetime1)));
    MSGI2((" ChannelTierBits    : ")); BDUMP2(PO_TIERBITS_LEN, in_ecm_inf->tierbits1); MSG2(("\n"));
    MSGI2((" RenderingObligation: 0x%02x\n", in_ecm_inf->ro1));
    MSGI2((" ScrambleKey(odd)   : ")); BDUMP2(PO_CKEY_LEN, in_ecm_inf->skey_odd); MSG2(("\n"));
    MSGI2((" Datetime2          : 0x%08x, %s\n",
           in_ecm_inf->datetime2, base_CMintime(&in_ecm_inf->datetime2)));
    MSGI2((" ChannelTierBits    : ")); BDUMP2(PO_TIERBITS_LEN, in_ecm_inf->tierbits2); MSG2(("\n"));
    MSGI2((" RenderingObligation: 0x%02x\n", in_ecm_inf->ro2));
    MSGI2((" ScrambleKey(even)  : ")); BDUMP2(PO_CKEY_LEN, in_ecm_inf->skey_even); MSG2(("\n"));

    MSGI2(("===== ecm_inf_t END ==============================\n"));

}
#endif /* ENABLE_DEBUG_FUNCTION */

static int
l_Check(ecm_inf_t              *in_ecm_inf)
{
    exh_InitOk();
    MSGENT();

    /* In future, version check should exist here. The followins are
       specification check of operaion rules for version 0100h */

    /*
     * [MIPTV] 6.1.3: Processing Rules for Datetime
     */
    MSGI3(("[Step1] check Datetime\n"));
    if (in_ecm_inf->datetime1 != in_ecm_inf->datetime2) {
        MSG1(("*** ERROR: Datetimes are mismatched...\n"));
        MSG1(("      datetime1 = 0x%08x, %s\n",
              in_ecm_inf->datetime1, base_CMintime(&in_ecm_inf->datetime1)));
        MSG1(("      datetime2 = 0x%08x, %s\n",
              in_ecm_inf->datetime2, base_CMintime(&in_ecm_inf->datetime2)));
        exh_Throw(RET_ECM_CHECK_ERR_FMT_DATETIME);
    }

    /*
     * [MIPTV] 6.1.3: Processing Rules for ChannelTierBits
     */
    MSGI3(("[Step2] check ChannelTierbit\n"));
    if (memcmp(in_ecm_inf->tierbits1, in_ecm_inf->tierbits2, PO_TIERBITS_LEN)) {
        MSG1(("*** ERROR: ChannelTierBits are mismatched...\n"));
        MSG1(("      tierbits1 = ")); BDUMP1(PO_TIERBITS_LEN, in_ecm_inf->tierbits1); MSG1(("\n"));
        MSG1(("      tierbits2 = ")); BDUMP1(PO_TIERBITS_LEN, in_ecm_inf->tierbits2); MSG1(("\n"));
        exh_Throw(RET_ECM_CHECK_ERR_FMT_TIERBITS);
    }

    /*
     * [MIPTV] 6.1.3: Processing Rules for RenderingObligation
     */
    MSGI3(("[Step3] check RenderingObligation\n"));
    if (in_ecm_inf->ro1 != in_ecm_inf->ro2) {
        MSG1(("*** ERROR: RenderingObligations are mismatched...\n"));
        MSG1(("      ro1 = 0x%04x", in_ecm_inf->ro1));
        MSG1(("      ro2 = 0x%04x", in_ecm_inf->ro2));
        exh_Throw(RET_ECM_CHECK_ERR_FMT_RO);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Verify(ecm_inf_t             *in_ecm_inf,
         po_key_info_t         *in_key_info)
{
    int i;

    exh_InitOk();
    MSGENT();

    /*
     * [MIPTV] 6.1.3: Processing Rules for NotBefore/NotAfter
     */
    MSGI3(("[Step1] Check Datetime Validity\n"));
    if (in_key_info->not_before != 0xFFFFFFFF || in_key_info->not_after != 0xFFFFFFFF) {
        u_int32_t tt;

        exh_Set(time_GetTrustedTime(&tt));
        if (!exh_IsOk()) exh_Throw(RET_ECM_TT_ERROR);

        if (in_key_info->not_before != 0xFFFFFFFF && tt < in_key_info->not_before) {
            MSG1(("*** ERROR: Key validity error... (not_before <= tt)\n"));
            MSG1(("      not_before = 0x%08x, %s\n",
                  in_key_info->not_before, base_CMintime(&in_key_info->not_before)));
            MSG1(("      tt         = 0x%08x, %s\n",
                  tt, base_CMintime(&tt)));
            exh_Throw(RET_ECM_WKEY_ERR_NOTBEFORE);
        }

        if (in_key_info->not_after  != 0xFFFFFFFF && tt > in_key_info->not_after) {
            MSG1(("*** ERROR: Key validity error... (tt <= not_after)\n"));
            MSG1(("      not_after  = 0x%08x, %s\n",
                  in_key_info->not_after, base_CMintime(&in_key_info->not_after)));
            MSG1(("      tt         = 0x%08x, %s\n",
                  tt, base_CMintime(&tt)));
            exh_Throw(RET_ECM_WKEY_ERR_NOTAFTER);
        }
    }

    /*
     * [MIPTV] 6.1.3: Processing Rules for ChannelTierBits/SubscriptionTierBits
     */
    MSGI3(("[Step2] Check Tierbits\n"));
    for (i = 0; i < PO_TIERBITS_LEN; i++)
        if ((in_key_info->tierbits[i] & in_ecm_inf->tierbits1[i]) != 0)
            break;
    if (i == PO_TIERBITS_LEN) {
        MSG1(("*** ERROR: Tierbits are mismatched...\n"));
        MSG1(("      tierbits (wkey) = ")); BDUMP1(PO_TIERBITS_LEN, in_key_info->tierbits); MSG1(("\n"));
        MSG1(("      tierbits (ecm)  = ")); BDUMP1(PO_TIERBITS_LEN, in_ecm_inf->tierbits1); MSG1(("\n"));
        exh_Throw(RET_ECM_WKEY_ERR_TIERBITS);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_Parse(po_thid_t               in_thid,
        const u_int8_t          in_ecm[PO_ECM_LEN],
        ecm_inf_t              *out_ecm_inf,
        po_key_info_t          *out_key_info)
{
    const u_int8_t     *p = in_ecm;
    u_int8_t            data[L_ENCDATA_LEN];
    u_int8_t            wkey[PO_CKEY_LEN];
    u_int8_t            iv[CRYPTO_AES_BLOCK_LEN];
    const u_int8_t     *priv_data1;
    const u_int8_t     *priv_data2;
    const u_int8_t     *priv_data3;
    const u_int8_t     *priv_data4;

    exh_InitOk();
    MSGENT();

    MSGI3(("----- ECM -----\n")); DUMP3(PO_ECM_LEN, p);

    /*
     * [MIPTV] 6.1.3: Processing Rules for ProtocolVersion
     */
    MSGI3(("[STEP1] check ProtocolVersion == 0x0100\n"));
    byte_BE2RI(p, out_ecm_inf->protocol_ver);
    if (out_ecm_inf->protocol_ver != L_PROTOCOL_VER) {
        MSG1(("*** ERROR: Protocol version is wrong...\n"
              "      expected = 0x%04x, actual = 0x%04x\n",
              L_PROTOCOL_VER, out_ecm_inf->protocol_ver));
        exh_Throw(RET_ECM_PARSE_ERR_PROTOCOL_VER);
    }

    MSGI3(("[STEP2] Parse for ver1\n"));
    byte_CPYRI(p, out_ecm_inf->wkey_id,      PO_CKEY_ID_LEN);
    byte_REFRI(p, priv_data1,                10);

    MSGI3(("[Step3] Get WorkKey from keymng\n"));
    exh_Set(keymng_GetContentKeyById(in_thid, out_ecm_inf->wkey_id, wkey, out_key_info));
    if (!exh_IsOk()) {
        if (exh_Get() == RET_KEYMNG_KEY_NOT_FOUND)
            exh_Throw(RET_ECM_WKEY_NOT_FOUND);
        else
            exh_Rethrow();
    }

    MSGI3(("[STEP4] Decrypt\n"));
    memset(iv, 0, CRYPTO_AES_BLOCK_LEN);
    exh_Set(crypto_AesCbcofbDecrypt(p, L_ENCDATA_LEN, wkey, iv, data));
    if (!exh_IsOk()) exh_Throw(RET_INTERNALERROR);

    p = data;
    byte_BE4RI(p, out_ecm_inf->datetime1);
    byte_CPYRI(p, out_ecm_inf->tierbits1,    PO_TIERBITS_LEN);
    byte_BE2RI(p, out_ecm_inf->ro1);
    byte_REFRI(p, priv_data2,                2);
    byte_CPYRI(p, out_ecm_inf->skey_odd,     PO_CKEY_LEN);
    byte_BE4RI(p, out_ecm_inf->datetime2);
    byte_CPYRI(p, out_ecm_inf->tierbits2,    PO_TIERBITS_LEN);
    byte_BE2RI(p, out_ecm_inf->ro2);
    byte_REFRI(p, priv_data3,                2);
    byte_CPYRI(p, out_ecm_inf->skey_even,    PO_CKEY_LEN);
    byte_REFRI(p, priv_data4,                16);

    MSGI3(("[Step5] Check ECM Operational Rule\n"));
    exh_Check(l_Check(out_ecm_inf));

    RUN2(l_PrintECMInf(out_ecm_inf));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
ecm_ExtractScrambleKey(po_thid_t        in_thid,
                       u_int32_t        in_rend_id,
                       const u_int8_t   in_ecm[PO_ECM_LEN])
{
    ecm_inf_t           ecm_inf;
    po_key_info_t       key_info;
    u_int8_t            output_ctrl[PO_EXPORT_INFO_MAX_LEN];
    u_int16_t           output_ctrl_len;

    exh_InitOk();
    MSGENT();

    if (!in_ecm)
        exh_Check(RET_ILLEGALARGS);

 
	dprintf("%s thid:%d rend_id:%d ecm_len:%d\n", __FUNCTION__, 
		 in_thid, 
		 in_rend_id);


    MSGI2(("[Step1] Parse ECM\n"));
    exh_Check(l_Parse(in_thid, in_ecm, &ecm_inf, &key_info));

    MSGI2(("[Step2] Verify Work Key\n"));
    exh_Check(l_Verify(&ecm_inf, &key_info));

    MSGI2(("[Step3] Extract ScrambleKeys to Renderer\n"));

    byte_BE2W(output_ctrl, ecm_inf.ro1);
    output_ctrl_len = 2;
    exh_Set(rend_SetContentKey(in_rend_id,
                               in_thid,
                               NULL,
                               ecm_inf.skey_odd,
                               ecm_inf.skey_even,
                               output_ctrl,
                               output_ctrl_len,
                               0,
                               NULL));

    if (!exh_IsOk()) exh_Throw(RET_ECM_REND_ERROR);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

//#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
int
ecm_GetScrambleKey(po_thid_t            in_thid,
                   const u_int8_t      *in_ecm,
                   u_int8_t            *out_skey_even,
                   u_int8_t            *out_skey_odd)
{
    ecm_inf_t           ecm_inf;
    po_key_info_t       key_info;

    exh_InitOk();
    MSGENT();

    if (!in_ecm || !out_skey_even || !out_skey_odd)
        exh_Check(RET_ILLEGALARGS);

    MSGI2(("[Step1] Parse ECM\n"));
    exh_Check(l_Parse(in_thid, in_ecm, &ecm_inf, &key_info));

    MSGI2(("[Step2] Verify Work Key\n"));
    exh_Check(l_Verify(&ecm_inf, &key_info));

    MSGI2(("[Step3] Get ScrambleKeys\n"));
    exh_Check(base_Memcpy(out_skey_odd,  PO_CKEY_LEN,  ecm_inf.skey_odd,  PO_CKEY_LEN));
    exh_Check(base_Memcpy(out_skey_even, PO_CKEY_LEN, ecm_inf.skey_even, PO_CKEY_LEN));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
//#endif /* !IPTVES_PUBLISH_BUILD */
