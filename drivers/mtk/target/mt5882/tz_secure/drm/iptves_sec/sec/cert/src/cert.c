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
#include "cert_parse.h"
#include "cert_verify.h"
#include "crl_inf.h"
#include "crl_parse.h"
#include "crl_verify.h"

COMPONENT(CERT);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_CERT_NAME_TYPE_NEMO_URI      "\x2b\x06\x01\x04\x01\x81\xb9\x2f\x01\x01\x01"
#define L_CERT_NAME_TYPE_NEMO_URI_SZ   (sizeof(L_CERT_NAME_TYPE_NEMO_URI) -1)

#define L_CERT_NAME_TYPE_NEMO_DEVID    "\x2b\x06\x01\x04\x01\x81\xb9\x2f\x03\x01\x01"
#define L_CERT_NAME_TYPE_NEMO_DEVID_SZ (sizeof(L_CERT_NAME_TYPE_NEMO_DEVID) -1)
#if 0
#define L_CERT_NAME_TYPE_PREFIX        "\x55\x1d"
#define L_CERT_NAME_TYPE_PREFIX_SZ     (sizeof(L_CERT_NAME_TYPE_PREFIX) -1)
#endif

#define L_SUBJECT_VALUE_SERVICECA      "urn:marlin:serviceca:"
#define L_SUBJECT_VALUE_SERVICECA_SZ   (sizeof(L_SUBJECT_VALUE_SERVICECA) - 1)

#define L_SUBJECT_VALUE_SERVICE        "urn:marlin:organization:"
#define L_SUBJECT_VALUE_SERVICE_SZ     (sizeof(L_SUBJECT_VALUE_SERVICE) - 1)

#define L_SUBJECT_VALUE_CLIENTCA       "urn:marlin:clientca:"
#define L_SUBJECT_VALUE_CLIENTCA_SZ    (sizeof(L_SUBJECT_VALUE_CLIENTCA) - 1)

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
l_GetNameType(cert_l_name_elm_t        *in_name,
              u_int8_t                 *out_type)
{
    exh_InitOk();
    MSGENT();

    if (in_name->type.sz == L_CERT_NAME_TYPE_NEMO_URI_SZ &&
        memcmp(in_name->type.p, L_CERT_NAME_TYPE_NEMO_URI, L_CERT_NAME_TYPE_NEMO_URI_SZ) == 0) {
        *out_type = CERT_INF_NAME_TYPE_NEMO_URI;
    } else if (in_name->type.sz == L_CERT_NAME_TYPE_NEMO_DEVID_SZ &&
               memcmp(in_name->type.p, L_CERT_NAME_TYPE_NEMO_DEVID, L_CERT_NAME_TYPE_NEMO_DEVID_SZ) == 0) {
        *out_type = CERT_INF_NAME_TYPE_NEMO_DEVID;
    } else {
        *out_type = CERT_INF_NAME_TYPE_NULL;
    }

    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GenerateCertInf(cert_l_t             *in_cert,
                  cert_inf_t           *io_inf)
{
    cert_l_name_elm_t *p;
    asn_elm_t elm;
    size_t sz;

    exh_InitOk();
    MSGENT();

    if (1 != in_cert->tbs.subject.num) {
        RUN2(cert_l_DebugPrintName(&in_cert->tbs.subject));
        exh_Throw(RET_CERT_ERR_FMT);
    }

    p = &in_cert->tbs.subject.elms[0];

    MSGI2(("[Step1] Get Subject Name Type\n"));
    exh_Check(l_GetNameType(p, &(io_inf->type)));
    if (io_inf->type == CERT_INF_NAME_TYPE_NULL)
        exh_Throw(RET_CERT_ERR_FMT);
    MSGI2(("  type      : 0x%02x\n", io_inf->type));

    MSGI2(("[Step2] Get Subjecct Name Value\n"));
    exh_Set(asn_elm_Parse(p->val.p, p->val.sz, &elm, NULL, &sz));
    if (!exh_IsOk() || 0 != sz  || ASN_TAG_UTF8STRING != elm.tag) {
        MSGI1(("----- subject name -----\n")); DUMP1(p->val.sz, p->val.p);
        exh_Throw(RET_CERT_ERR_FMT);
    }

    exh_Check(base_Memcpy(io_inf->subject, elm.val.sz, elm.val.p, elm.val.sz));

    io_inf->subject[elm.val.sz] = '\0';
    io_inf->subject_len = elm.val.sz;

    bptr_Copy(&in_cert->tbs.pubkey, &io_inf->pubkey);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_RevokeCheck(cert_l_t                 *in_cert,
              crl_l_t                  *in_crl)
{
    bptr_t   *serial, *issuer, *rserial, *rissuer, rissuer_asn;
    asn_elm_t elms[CERT_L_RLIST_ELM_MAX_NUM];
    int       elm_num = CERT_L_RLIST_ELM_MAX_NUM;
    size_t    sz;
    int       i;

    exh_InitOk();
    MSGENT();

    serial = &(in_cert->tbs.serial);
    issuer = &(in_cert->tbs.issuer.asn);

    MSGI2(("----- cert serial -----\n")); DUMP2(serial->sz, serial->p);
    MSGI2(("----- cert issuer -----\n")); DUMP2(issuer->sz, issuer->p);
    RUN2(cert_l_DebugPrintName(&(in_cert->tbs.issuer)));

    rissuer = &(in_crl->tbs.issuer.asn);

    if (in_crl->tbs.rlist.p) {
        asn_elm_t elm_rlist;

        exh_Set(asn_elm_ParseBptr(&in_crl->tbs.rlist, &elm_rlist, NULL, &sz));
        if (!exh_IsOk() || sz != 0) exh_Throw(RET_CERT_ERR_FMT);

        exh_Set(asn_ParseSeq(&elm_rlist, elms, &elm_num, NULL, &sz));
        if (!exh_IsOk() || sz != 0) exh_Throw(RET_CERT_ERR_FMT);
    } else {
        elm_num = 0;
    }

    MSGI2(("revoked serial num : %d\n", elm_num));

    for (i = 0 ; i < elm_num ; i++) {
        crl_l_rlist_elm_t relm;

        exh_Check(crl_l_ParseRListElm(&elms[i], &relm));

        rserial = &(relm.serial);
        MSGI2(("revoked serial [%d]\n", i));
        MSGI2(("----- revoked serial -----\n")); DUMP2(rserial->sz, rserial->p);

        if (relm.issuer.asn.sz > 0) {
            bptr_Copy(&(relm.issuer.asn), &rissuer_asn);
            rissuer = &rissuer_asn;
            RUN2(cert_l_DebugPrintName(&(relm.issuer)));
        }
        MSGI2(("----- revoked issuer -----\n")); DUMP2(rissuer->sz, rissuer->p);

        if (serial->sz != rserial->sz) continue;
        if (0 != memcmp(serial->p, rserial->p, serial->sz)) continue;
        if (issuer->sz != rissuer->sz) continue;
        if (0 != memcmp(issuer->p, rissuer->p, issuer->sz)) continue;

        exh_Throw(RET_CERT_ERR_REVOKED);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_VerifySubject(cert_l_t               *in_cert,
                u_int8_t                in_type,
                char                   *in_value)
{
    cert_l_name_elm_t *subject = &in_cert->tbs.subject.elms[0];
    u_int8_t type;
    asn_elm_t elm;
    size_t sz;

    exh_InitOk();
    MSGENT();

    exh_Check(l_GetNameType(subject, &type));
    if (type != in_type)
        exh_Throw(RET_CERT_ERR_FMT);

    exh_Set(asn_elm_Parse(subject->val.p, subject->val.sz, &elm, NULL, &sz));
    if (!exh_IsOk() || 0 != sz  || ASN_TAG_UTF8STRING != elm.tag) {
        MSGI2(("----- subject name -----\n")); DUMP2(subject->val.sz, subject->val.p);
        exh_Throw(RET_CERT_ERR_FMT);
    }
    if (memcmp(elm.val.p, in_value, strlen(in_value)))
        exh_Throw(RET_CERT_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_GenerateCRLInf(crl_l_t       *in_crl,
                 crl_inf_t     *out_crl_inf)
{
    exh_InitOk();
    MSGENT();

    out_crl_inf->next_update = in_crl->tbs.next_update;
    out_crl_inf->this_update = in_crl->tbs.this_update;

    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
cert_VerifyCert(u_int32_t               in_type,
                const u_int8_t         *in_certs,
                u_int32_t               in_certs_len,
                const u_int8_t         *in_ta,
                u_int32_t               in_ta_len,
                time_unix_t            *in_time,
                cert_inf_t             *io_inf,
                const u_int8_t         *in_crl,
                u_int32_t               in_crl_len,
                int                    *io_err_inf)
{
    int       certs_num;
    cert_l_t *certs[2];
    cert_l_t  ta;
    cert_l_t  ca;
    cert_l_t  leaf;
    crl_l_t   crl;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] Parse PKIPath \n"));
    *io_err_inf = 0;

    certs[0] = &ca;
    certs[1] = &leaf;
    certs_num = 2;
    exh_Set(cert_l_ParsePKIPath(in_certs, in_certs_len, certs, &certs_num));
    if (2 != certs_num) exh_Throw(RET_CERT_ERR_FMT);
    if (!(exh_IsOk())) {
        switch (certs_num) {
        case 1: /* CA */
            *io_err_inf = 2;
            break;
        case 2: /* Leaf (SRV/DEV) */
            *io_err_inf = 3;
            break;
        case 0: /* PKIPath */
        default:
            break;
        }
        exh_Rethrow();
    }

    RUN3(cert_l_DebugPrint(&ca));
    RUN3(cert_l_DebugPrint(&leaf));

    MSGI2(("[Step2] Parse TA Certificate\n"));
    *io_err_inf = 1; /* TA */
    exh_Check(cert_l_Parse(in_ta, in_ta_len, &ta));

    MSGI2(("[Step3] Verify CA Certificate\n"));
    *io_err_inf = 2; /* CA */
    exh_Check(cert_l_VerifyCert(&ca, &ta, NULL));
    if (in_type == CERT_TYPE_SRVPKIPATH) {
        exh_Check(l_VerifySubject(&ca, CERT_INF_NAME_TYPE_NEMO_URI, L_SUBJECT_VALUE_SERVICECA));
    } else {
        exh_Check(l_VerifySubject(&ca, CERT_INF_NAME_TYPE_NEMO_URI, L_SUBJECT_VALUE_CLIENTCA));
    }

    MSGI2(("[Step4] Verify Leaf Certificate\n"));
    *io_err_inf = 3; /* Service */
    if (in_type == CERT_TYPE_SRVPKIPATH) {
        exh_Check(cert_l_VerifyCert(&leaf, &ca, in_time));
        exh_Check(l_VerifySubject(&leaf, CERT_INF_NAME_TYPE_NEMO_URI, L_SUBJECT_VALUE_SERVICE));
    } else {
        exh_Check(cert_l_VerifyCert(&leaf, &ca, NULL));
    }

    if (io_inf != NULL) {
        MSGI2(("[Step5] Generate Output\n"));
        exh_Check(l_GenerateCertInf(&leaf, io_inf));
    }

    if (in_crl != NULL && in_type == CERT_TYPE_SRVPKIPATH) {
        MSGI2(("[Step6] Parse CRL \n"));
        exh_Check(crl_l_Parse(in_crl, in_crl_len, &crl));

        MSGI2(("[Step7] CA Revoke Check \n"));
        *io_err_inf = 2; /* CA */
        exh_Check(l_RevokeCheck(&ca, &crl));

        MSGI2(("[Step8] Server Revoke Check \n"));
        *io_err_inf = 3; /* Service */
        exh_Check(l_RevokeCheck(&leaf, &crl));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_VerifySignature(const cert_inf_t  *in_inf,
                     const u_int8_t    *in_msg,
                     size_t             in_msg_len,
                     const u_int8_t    *in_sig,
                     size_t             in_sig_len)
{
    cert_l_sig_t sig_inf;

    exh_InitOk();
    MSGENT();

    sig_inf.type = CERT_L_SIG_TYPE_ECDSA;
    MSGI2(("----- sig -----\n")); DUMP2(in_sig_len, in_sig);

    sig_inf.sig.ecdsa.r.p  = in_sig;
    sig_inf.sig.ecdsa.r.sz = 28;
    sig_inf.sig.ecdsa.s.p  = in_sig + 28;
    sig_inf.sig.ecdsa.s.sz = 28;
    sig_inf.data.p = in_msg;
    sig_inf.data.sz = in_msg_len;
    exh_Check(cert_l_VerifySignature(&sig_inf, &in_inf->pubkey));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_VerifyCRL(const u_int8_t          *in_crl,
               u_int32_t                in_crl_len,
               const u_int8_t          *in_ta,
               u_int32_t                in_ta_len,
               u_int32_t               *in_time,
               crl_inf_t               *out_inf)
{
    crl_l_t crl;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] Parse CRL \n"));
    exh_Check(crl_l_Parse(in_crl, in_crl_len, &crl));

    if (in_ta) {
        cert_l_t ta;

        MSGI2(("[Step2] Parse TA Certificate\n"));
        exh_Check(cert_l_Parse(in_ta, in_ta_len, &ta));

        RUN3(crl_l_DebugPrint(&crl));

        MSGI2(("[Step3] Verify CRL\n"));
        exh_Check(crl_l_Verify(&crl, &ta, in_time));
    }

    if (out_inf)
        exh_Check(l_GenerateCRLInf(&crl, out_inf));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
void
cert_PrintCertInf(cert_inf_t           *in_inf)
{
    MSGI1(("===== CertInf START =====\n"));

    switch (in_inf->type) {

    case CERT_INF_NAME_TYPE_NEMO_URI:
        MSGI1(("    NEMO URI   : %s\n", in_inf->subject));
        break;

    case CERT_INF_NAME_TYPE_NEMO_DEVID:
        MSGI1(("    NEMO DEVID : %s\n", in_inf->subject));
        break;

    default:
        return;
    }

    MSGI1(("===== CertInf START =====\n"));

    return;
}

void
cert_PrintCRLInf(crl_inf_t             *in_inf)
{
    MSGI1(("===== CRLInf START =====\n"));
    MSGI1(("    this_update : %s\n", base_CMintime(&in_inf->this_update)));
    MSGI1(("    next_update : %s\n", base_CMintime(&in_inf->next_update)));
    MSGI1(("===== CRLInf START =====\n"));
    return;
}
#endif /* IPTVES_PUBLISH_BUILD */
