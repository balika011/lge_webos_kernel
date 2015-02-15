/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "bptr_api_iptves.h"
#include "time_api_iptves.h"
#include "asn_api_iptves.h"
#include "cert_api_iptves.h"
#include "cert_inf.h"

COMPONENT(CERT);

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
/*----------------------------------------------------------------------------
 * Extensions
 *--------------------------------------------------------------------------*/
static void
cert_l_DebugPrintExtElm(const cert_l_ext_elm_t *in_elm_p)
{

    const char *extid_str[0x30] = {"",
                                   "", "", "", "",
                                   "", "", "", "",
                                   "", "", "", "",
                                   "", "skeyid", "keyusage", "",
                                   "", "", "basic", "crlnumber",
                                   "", "", "", "",
                                   "", "", "", "distpoint",
                                   "certissuer", "", "crldist", "",
                                   "", "", "akeyid", "",
                                   "", "", "", "",
                                   "", "", "", "",
                                   "", "", ""};
    const char unknown[] = "????";
    const char *s;

    if (0 == in_elm_p->id || 0x30 <= in_elm_p->id)
        s = unknown;
    else
        s = extid_str[in_elm_p->id];

    if (0x20 >= in_elm_p->val.sz) {
        MSGI2(("[ext %d] (c %d) %s : \n", in_elm_p->id, in_elm_p->critical, s));
        cert_l_DumpBptr(&in_elm_p->val, (char*)s);
    } else {
        MSGI2(("[ext %d] (c %d) %s : \n", in_elm_p->id, in_elm_p->critical, s));
        MSGI2(("----- %s -----\n", (char*)s)); DUMP2(in_elm_p->val.sz, in_elm_p->val.p);
    }

    return;
}

/*----------------------------------------------------------------------------
 * Name
 *--------------------------------------------------------------------------*/
static void
cert_l_DebugPrintNameElm(const cert_l_name_elm_t *in_elm_p)
{
    asn_elm_t elm;

    exh_InitOk();

    cert_l_DumpBptr(&in_elm_p->type, "type");
    cert_l_DumpBptr(&in_elm_p->val,  "val");

    exh_Set(asn_elm_ParseBptr(&in_elm_p->val, &elm, NULL, NULL));
    if (RET_OK == exh_Get() && ASN_TAG_UTF8STRING == elm.tag) {
        MSGI2(("val str : ")); PRINT2(elm.val.sz, elm.val.p); MSGI2(("\n"));
    }

    return;
}

/*----------------------------------------------------------------------------
 * Signature
 *--------------------------------------------------------------------------*/
static void
cert_l_DebugPrintAlgo(const cert_l_algo_t *in_algo_p)
{
    cert_l_DumpBptr(&in_algo_p->id, "algorithm");

    if (0 == in_algo_p->param.sz) {
        MSGI2(("params      : NULL\n"));
    } else {
        cert_l_DumpBptr(&in_algo_p->param, "param");
    }

    return;
}

/*----------------------------------------------------------------------------
 * Certificate
 *--------------------------------------------------------------------------*/
static void
cert_l_DebugPrintTBS(const cert_l_tbs_t *in_tbs_p)
{
    cert_l_DumpBptr(&in_tbs_p->serial, "serial");
    MSGI2(("===== issuer start =====\n"));
    cert_l_DebugPrintName(&in_tbs_p->issuer);
    MSGI2(("===== issuer end   =====\n"));
    cert_l_DumpBptr(&in_tbs_p->sig_algo, "sig_algo");
    MSGI2(("===== subject start =====\n"));
    cert_l_DebugPrintName(&in_tbs_p->subject);
    MSGI2(("===== subject end   =====\n"));
    MSGI2(("not_before : %s\n", base_CTime(&in_tbs_p->not_before)));
    MSGI2(("not_after  : %s\n", base_CTime(&in_tbs_p->not_after)));
    cert_l_DumpBptr(&in_tbs_p->pubkey_algo, "pubkey_algo");
    cert_l_DumpBptr(&in_tbs_p->pubkey, "pubkey");

    if (0 != (in_tbs_p->ext_flags & CERT_L_EXT_FLAG_KEYUSAGE)) {
        MSGI2(("keyusage  : 0x%08x\n", in_tbs_p->keyusage));
    }

    if (0 != (in_tbs_p->ext_flags & CERT_L_EXT_FLAG_BASIC)) {
        if (in_tbs_p->basic_ca) {
            MSGI2(("basic_ca  : TRUE\n"));
        } else {
            MSGI2(("basic_ca  : FALSE\n"));
        }

        MSGI2(("basic_path_len : 0x%08x\n", in_tbs_p->basic_pathlen));
    }

    cert_l_DebugPrintExt(&(in_tbs_p->ext));

    return;
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
void
cert_l_DumpBptr(const bptr_t *in_p, char *in_s_p)
{
    if (in_p->sz <= 0x14) {
        size_t i;

        MSGI3(("----- %s -----\n", in_s_p));

        for (i = 0; i < in_p->sz ;i++) {
            MSG3(("%02x", *(in_p->p + i)));

            if (3 == i % 4) {
                MSG3((" ", *(in_p->p + i)));
            }
        }

        MSG3(("\n"));
    } else {
        MSGI3(("----- %s -----\n", in_s_p)); DUMP3(in_p->sz, in_p->p);
    }
}

void
cert_l_DebugPrintExt(const cert_l_ext_t *in_ext_p)
{
    int i;
    MSGI2(("===== Extensions num=%d start =====\n", in_ext_p-> num));

    for (i = 0; i < in_ext_p->num ; i++) {
        cert_l_DebugPrintExtElm(&in_ext_p->elms[i]);
    }

    MSGI3(("----- asn extensions -----\n")); DUMP3(in_ext_p->asn.sz, in_ext_p->asn.p);

    MSGI2(("===== Extensions end =====\n"));
    return;
}

void
cert_l_DebugPrintName(const cert_l_name_t *in_name_p)
{
    int i;

    for (i = 0; i < in_name_p->num ; i++) {
        cert_l_DebugPrintNameElm(&in_name_p->elms[i]);
    }

    return;
}

void
cert_l_DebugPrintSig(const cert_l_sig_t *in_sig_p)
{
    const cert_l_sig_t *p;

    p = in_sig_p;

    MSGI2(("====== sig START =====\n"));

    switch (p->type) {
    case CERT_L_SIG_TYPE_ECDSA:
        MSGI2(("type : ECDSA (%d)\n", p->type));
        break;

    default:
        MSGI2(("type : UNKNOWN (%d)\n", p->type));
        break;
    }

    cert_l_DebugPrintAlgo(&p->algo);

    switch (p->type) {
    case CERT_L_SIG_TYPE_ECDSA:
        cert_l_DumpBptr(&p->sig.ecdsa.r, "r");
        cert_l_DumpBptr(&p->sig.ecdsa.s, "s");
        break;

    default:
        break;
    }

    MSGI2(("====== sig END =====\n"));

    return;
}

void
cert_l_DebugPrint(const cert_l_t *in_cert_p)
{
    MSGI2(("===== CERT PRINT START ==========================\n"));
    cert_l_DebugPrintTBS(&(in_cert_p->tbs));
    cert_l_DebugPrintSig(&(in_cert_p->sig));
    MSGI2(("===== CERT PRINT END ============================\n"));
    return;
}
