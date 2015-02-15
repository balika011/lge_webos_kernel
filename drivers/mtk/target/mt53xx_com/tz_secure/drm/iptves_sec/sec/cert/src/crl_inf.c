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
#include "bptr_api_iptves.h"
#include "time_api_iptves.h"
#include "asn_api_iptves.h"
#include "cert_api_iptves.h"
#include "cert_inf.h"
#include "crl_inf.h"
#include "crl_parse.h"

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
 * RevocationList
 *--------------------------------------------------------------------------*/
static void
crl_l_DebugPrintRListElm(crl_l_rlist_elm_t *in_elm)
{

    if (in_elm->serial.sz < 0x16) {
        size_t i;

        MSGI3(("    serial : "));

        for (i = 0; i < in_elm->serial.sz; i++) {
            MSG3(("%02x", *(in_elm->serial.p + i)));

            if (3 == i % 4) {
                MSG3((" ", *(in_elm->serial.p + i)));
            }
        }

        MSG3(("\n"));
    } else {
        MSGI3(("----- serial -----\n")); DUMP3(in_elm->serial.sz, in_elm->serial.p);
    }

    /* need not to parse revocationDate */
    /*    MSGI3(("    revoke date : %s\n", base_CMintime(&in_elm->date))); */

    if (in_elm->ext.num != 0) {
        MSGI3(("     ===== issuer name start =====\n"));
        cert_l_DebugPrintName(&(in_elm->issuer));
        MSGI3(("     ===== issuer name end =====\n"));
        cert_l_DebugPrintExt(&(in_elm->ext));
    }
    return;
}

void
crl_l_DebugPrintRevocationList(bptr_t *in_rlist)
{
    asn_elm_t elms[CERT_L_RLIST_ELM_MAX_NUM];
    int       elm_num = CERT_L_RLIST_ELM_MAX_NUM;
    size_t    sz;
    int       i;

    if (in_rlist->p) {
        asn_elm_t elm_rlist;

        asn_elm_ParseBptr(in_rlist, &elm_rlist, NULL, &sz);
        asn_ParseSeq(&elm_rlist, elms, &elm_num, NULL, &sz);
    } else {
        elm_num = 0;
    }

    if (0 == elm_num) {
        MSGI3(("     RevocationList : NULL\n"));
        return;
    }

    MSGI3(("     ===== RevocationList start =====\n"));

    for (i = 0; i < elm_num ; i++) {
        crl_l_rlist_elm_t relm;

        crl_l_ParseRListElm(&elms[i], &relm);
        crl_l_DebugPrintRListElm(&relm);
    }

    MSGI3(("----- asn revokelist -----\n")); DUMP3(in_rlist->sz, in_rlist->p);

    MSGI3(("     ===== RevocationList end =====\n"));
    return;
}

/*----------------------------------------------------------------------------
 * CRL
 *--------------------------------------------------------------------------*/
static void
crl_l_DebugPrintTBS(crl_l_tbs_t *in_tbs)
{
    size_t i;

    MSGI3(("     sig_algo   :"));
    for (i = 0; i < in_tbs->sig_algo.sz; i++) {
        MSG3(("%02x", *(in_tbs->sig_algo.p + i)));

        if (3 == i % 4) {
            MSG3((" ", *(in_tbs->sig_algo.p + i)));
        }
    }
    MSGI3(("\n"));
    MSGI3(("===== issuer start =====\n"));
    cert_l_DebugPrintName(&in_tbs->issuer);
    MSGI3(("     thisUpdate : %s\n", base_CMintime(&in_tbs->this_update)));
    MSGI3(("     nextUpdate : %s\n", base_CMintime(&in_tbs->next_update)));
    crl_l_DebugPrintRevocationList(&(in_tbs->rlist));
    cert_l_DebugPrintExt(&(in_tbs->ext));

    return;
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
void
crl_l_DebugPrint(crl_l_t *in_crl)
{
    MSGI3(("===== CRL PRINT START ===========================\n"));
    crl_l_DebugPrintTBS(&(in_crl->tbs));
    cert_l_DebugPrintSig(&(in_crl->sig));
    MSGI3(("===== CRL PRINT END =============================\n"));
    return;
}


