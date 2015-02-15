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
#include "bptr_api_iptves.h"
#include "time_api_iptves.h"
#include "asn_api_iptves.h"
#include "cert_api_iptves.h"
#include "cert_inf.h"
#include "cert_parse.h"
#include "crl_inf.h"
#include "crl_parse.h"

COMPONENT(CERT);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_TBS_VAL_VER           "\x02\x01\x01"
#define L_TBS_VAL_VER_SZ        (sizeof(L_TBS_VAL_VER) - 1)

#define L_ELM_NUM_RLIST_ELM     3
#define L_ELM_NUM_TBS           7
#define L_ELM_NUM_CRL           3

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
l_ParseRListExtensions(crl_l_rlist_elm_t *io_elm)
{
    asn_elm_t elm_gname;
    int       elm_num;
    asn_elm_t elm;
    size_t    sz;
    cert_l_ext_elm_t *p;
    int       i;

    exh_InitOk();
    MSGENT();

    for (i = 0; i < io_elm->ext.num ;i++) {
        p = &(io_elm->ext.elms[i]);

        switch (p->id) {
        case CERT_L_EXT_TYPE_CERTISSUER:
            exh_Set(asn_elm_Parse(p->val.p, p->val.sz, &elm_gname, NULL, &sz));
            if (!exh_IsOk() || sz != 0) exh_Throw(RET_CRL_ERR_FMT);

            elm_num = 1;

            exh_Set(asn_ParseSeq(&elm_gname, &elm, &elm_num, NULL, &sz));
            if (!exh_IsOk() || 0 != sz || 1 != elm_num)
                exh_Throw(RET_CRL_ERR_UNSUPPORTED);

            if (0xa4 != elm.tag) exh_Throw(RET_CRL_ERR_UNSUPPORTED);

            exh_Set(asn_elm_ParseBptr(&elm.val, &elm, NULL, &sz));
            if (!exh_IsOk() || 0 != sz)
                exh_Throw(RET_CRL_ERR_FMT);

            exh_Check(cert_l_ParseName(&elm, &(io_elm->issuer)));

            break;

        default:
            MSGI3((" ext id 0x%08x Not Supported\n", p->id));

            if (p->critical) exh_Throw(RET_CERT_ERR_FMT);

            break;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseExtensions(crl_l_tbs_t          *io_tbs)
{
    cert_l_ext_elm_t *p;
    int i;

    exh_InitOk();
    MSGENT();

    for (i = 0; i < io_tbs->ext.num ;i++) {
        p = &(io_tbs->ext.elms[i]);

        switch (p->id) {
        case CERT_L_EXT_TYPE_AKEYID:
        case CERT_L_EXT_TYPE_CRLNUMBER:
        case CERT_L_EXT_TYPE_DISTPOINT:
        case CERT_L_EXT_TYPE_CRLDIST:
            MSGI3((" ext id 0x%08x Not Implemented\n", p->id));
            break;

        default:
            MSGI3((" ext id 0x%08x Not Supported\n", p->id));
            if (p->critical) exh_Throw(RET_CERT_ERR_FMT);
            break;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseTBS(asn_elm_t                   *in_elm,
           crl_l_tbs_t                 *io_tbs)
{
    asn_elm_t elms[L_ELM_NUM_TBS];
    int       elm_num = L_ELM_NUM_TBS;
    size_t    sz;
    int       r;
    int       idx;
    asn_time_t at;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] ASN Parse TBSCertList\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_TBS - 1 > elm_num) {
        MSGI1(("[ERROR] in asn_ParseSeq aret=0x%08x, sz=0x%08x, elm_num=0x%08x\n",
               exh_Get(), sz , elm_num));
        MSGI1(("----- TBSCertList -----\n")); DUMP1(in_elm->asn.sz, in_elm->asn.p);
        exh_Throw(RET_CRL_ERR_FMT);
    }

    MSGI2(("[Step2] Check Version\n"));
    r = memcmp(elms[0].asn.p, L_TBS_VAL_VER, L_TBS_VAL_VER_SZ);
    if (0 != r) exh_Throw(RET_CRL_ERR_FMT);

    io_tbs->ver = 2;

    MSGI2(("[Step3] Parse sig algo\n"));
    if (ASN_TAG_SEQUENCE != elms[1].tag) exh_Throw(RET_CRL_ERR_FMT);
    bptr_Copy(&elms[1].val, &io_tbs->sig_algo);

    MSGI2(("[Step4] Parse issuer\n"));
    exh_Check(cert_l_ParseName(&elms[2], &io_tbs->issuer));

    MSGI2(("[Step5] Parse thisUpdate\n"));
    exh_Set(asn_ParseTime(&elms[3], &at));
    if (!exh_IsOk()) exh_Throw(RET_CRL_ERR_FMT);
    exh_Set(time_ConvToUnixtime(at.year, at.mon, at.mday,
                                at.hour, at.min, at.sec, &io_tbs->this_update));
    if (!exh_IsOk()) exh_Throw(RET_CRL_ERR_FMT);

    MSGI2(("[Step6] Parse nextUpdate\n"));
    exh_Set(asn_ParseTime(&elms[4], &at));
    if (!exh_IsOk()) exh_Throw(RET_CRL_ERR_FMT);
    exh_Set(time_ConvToDatetime(at.year, at.mon, at.mday,
                                at.hour, at.min, &io_tbs->next_update));
    if (!exh_IsOk()) exh_Throw(RET_CRL_ERR_FMT);

    MSGI2(("[Step7] Parse revokedCertificates\n"));
    if (ASN_TAG_SEQUENCE == elms[5].tag) {
        asn_elm_t rlist_elms[CERT_L_RLIST_ELM_MAX_NUM];
        int       rlist_elm_num = CERT_L_RLIST_ELM_MAX_NUM;
        size_t    rlist_sz;
        int       i;

        exh_Set(asn_ParseSeq(&elms[5], rlist_elms, &rlist_elm_num,
                             NULL, &rlist_sz));
        if (!exh_IsOk() || rlist_sz != 0) exh_Throw(RET_CERT_ERR_FMT);

        for (i = 0 ; i < rlist_elm_num ; i++) {
            crl_l_rlist_elm_t relm;

            exh_Check(crl_l_ParseRListElm(&rlist_elms[i], &relm));
        }

        io_tbs->rlist.sz = elms[5].asn.sz;
        io_tbs->rlist.p  = elms[5].asn.p;
        idx = 6;
    } else {
        io_tbs->rlist.sz = 0;
        io_tbs->rlist.p  = NULL;
        MSGI3((" revokedCertificates are NULL\n"));
        idx = 5;
    }

    MSGI2(("[Step8] Parse Extension\n"));
    if (0xa0 != elms[idx].tag) exh_Throw(RET_CRL_ERR_FMT);
    exh_Check(cert_l_ParseExtensions(&elms[idx].val, &(io_tbs->ext)));

    MSGI2(("[Step8-2] Parse Extension Value\n"));
    exh_Check(l_ParseExtensions(io_tbs));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
crl_l_Parse(const u_int8_t             *in_crl,
            size_t                      in_crl_sz,
            crl_l_t                    *io_inf)
{
    asn_elm_t elms[L_ELM_NUM_CRL];
    int       elm_num = L_ELM_NUM_CRL;
    size_t    sz;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_crl && NULL != io_inf);

    MSGI2(("[Step1] ASN Parse Certificate Element\n"));
    exh_Set(asn_ParseSeqStr(in_crl, in_crl_sz, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_CRL != elm_num)
        exh_Throw(RET_CRL_ERR_FMT);

    MSGI3(("----- TBSCertificateList -----\n")); DUMP3(elms[0].asn.sz, elms[0].asn.p);
    MSGI3(("----- Sig Algo -----\n"));           DUMP3(elms[1].asn.sz, elms[1].asn.p);
    MSGI3(("----- Sig -----\n"));                DUMP3(elms[2].asn.sz, elms[2].asn.p);

    MSGI2(("[Step2] Parse TBSCertificateList\n"));
    exh_Set(l_ParseTBS(&elms[0], &(io_inf->tbs)));
    if (!(exh_IsOk())) exh_Throw(RET_CRL_ERR_FMT);

    bptr_Copy(&elms[0].asn, &io_inf->sig.data);

    MSGI2(("[Step3] Parse sig algo\n"));
    exh_Set(cert_l_ParseAlgo(&elms[1], &(io_inf->sig.algo)));
    if (!(exh_IsOk())) exh_Throw(RET_CRL_ERR_FMT);

    MSGI2(("[Step4] Parse sig\n"));
    exh_Set(cert_l_ParseSig(&elms[2], &(io_inf->sig)));
    if (!(exh_IsOk())) exh_Throw(RET_CRL_ERR_FMT);

    /* crl_l_DebugPrint(io_inf);*/

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
crl_l_ParseRListElm(const asn_elm_t    *in_elm,
                    crl_l_rlist_elm_t  *io_elm)
{
    asn_elm_t  elms[L_ELM_NUM_RLIST_ELM];
    int        elm_num = L_ELM_NUM_RLIST_ELM;
    asn_int_t  aint;
    size_t     sz;

    exh_InitOk();
    MSGENT();

    io_elm->issuer.asn.p  = NULL;
    io_elm->issuer.asn.sz = 0;

    MSGI2(("[Step1] ASN Parse Attribute\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_RLIST_ELM - 1 > elm_num)
        exh_Throw(RET_CRL_ERR_FMT);

    MSGI2(("[Step2] Parse Serial\n"));
    /* In RFC3280 Serial must be positive */
    exh_Set(asn_ParseInteger(&elms[0], &aint));
    if (!exh_IsOk() || FALSE == aint.sign)
        exh_Throw(RET_CRL_ERR_FMT);

    bptr_Copy(&aint.val, &io_elm->serial);

    /* need not to parse revocationDate */
    MSGI2(("[Step2] Parse Date (skip)\n"));
    if (elms[1].tag != ASN_TAG_UTCTIME && elms[1].tag != ASN_TAG_GTIME)
        exh_Throw(RET_CRL_ERR_FMT);

    MSGI2(("[Step3] Parse Extension\n"));
    if (L_ELM_NUM_RLIST_ELM == elm_num) {
        exh_Check(cert_l_ParseExtensions(&elms[2].asn, &(io_elm->ext)));
        exh_Check(l_ParseRListExtensions(io_elm));
    } else {
        io_elm->ext.num    = 0;
        io_elm->ext.asn.p  = NULL;
        io_elm->ext.asn.sz = 0;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
