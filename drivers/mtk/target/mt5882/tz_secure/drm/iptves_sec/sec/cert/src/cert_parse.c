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

COMPONENT(CERT);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_ELM_NUM_EXTENSION_ELM         3
#define L_ELM_NUM_NAME_ELM              2
#define L_ELM_NUM_ALGO                  2
#define L_ELM_NUM_BASIC                 2
#define L_ELM_NUM_VALIDITY              2
#define L_ELM_NUM_PUBKEY                2
#define L_ELM_NUM_TBS                   8
#define L_ELM_NUM_SIG                   2
#define L_ELM_NUM_CERT                  3

#define L_RDNSEQUENCE_MAX_NUM           3

#define L_EXTENSION_ID_PREFIX           "\x55\x1d"
#define L_EXTENSION_ID_PREFIX_SZ        (sizeof(L_EXTENSION_ID_PREFIX) -1)
#define L_EXTENSION_ID_SZ               (L_EXTENSION_ID_PREFIX_SZ + 1 )

#define L_ALGO_SIG_ECDSA                "\x2a\x86\x48\xce\x3d\x04\x03\x02"

#define L_ELM_PUBKEY_ALGO               "\x30\x09\x06\x07\x2a\x86\x48\xce\x3d\x02\x01"
#define L_ELM_PUBKEY_ALGO_SZ            (sizeof(L_ELM_PUBKEY_ALGO) -1)

#define L_TBS_VAL_VER                   "\xa0\x03\x02\x01\x02"
#define L_TBS_VAL_VER_SZ                (sizeof(L_TBS_VAL_VER) - 1)

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
l_GetExtensionType(const asn_elm_t     *in_elm,
                   u_int8_t            *io_type)
{
    int r;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_type);

    if (ASN_TAG_OID != in_elm->tag) exh_Throw(RET_CERT_ERR_FMT);
    if (L_EXTENSION_ID_SZ != in_elm->val.sz) exh_Throw(RET_CERT_ERR_FMT);

    r = memcmp(in_elm->val.p, L_EXTENSION_ID_PREFIX, L_EXTENSION_ID_PREFIX_SZ);
    if (0 != r) exh_Throw(RET_CERT_ERR_FMT);

    *io_type = *(in_elm->val.p + 2);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseExtensionElm(const asn_elm_t    *in_elm,
                    cert_l_ext_elm_t   *io_ext)
{
    asn_elm_t  elms[L_ELM_NUM_EXTENSION_ELM];
    int        elm_num = L_ELM_NUM_EXTENSION_ELM;
    size_t     sz;
    asn_elm_t *v;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_ext);

    MSGI2(("[Step1] ASN Parse Extensions\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || sz != 0 || L_ELM_NUM_EXTENSION_ELM - 1 > elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse ExtensionID\n"));
    exh_Check(l_GetExtensionType(&elms[0], &(io_ext->id)));
    MSGI2(("  id      : 0x%02x\n", io_ext->id));

    MSGI2(("[Step3] Parse Critical\n"));
    /* critical flag can be omitted */
    if (L_ELM_NUM_EXTENSION_ELM == elm_num) {
        exh_Set(asn_ParseBoolean(&elms[1], &(io_ext->critical)));
        if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);

        v = &elms[2];
    } else {
        io_ext->critical = FALSE;

        v = &elms[1];
    }
    MSGI2(("  critical: 0x%02x\n", io_ext->critical));

    MSGI2(("[Step4] Parse Value\n"));
    if (ASN_TAG_OSTRING != v->tag) exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&v->val, &io_ext->val);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseNameElm(const asn_elm_t         *in_elm,
               cert_l_name_elm_t       *io_name)
{
    asn_elm_t elms[L_ELM_NUM_NAME_ELM];
    int       elm_num = L_ELM_NUM_NAME_ELM;
    size_t    sz;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_name);

    MSGI2(("[Step1] ASN Parse Attribute\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_NAME_ELM != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse AttributeType\n"));
    if (ASN_TAG_OID != elms[0].tag) exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&elms[0].val, &io_name->type);

    MSGI2(("[Step2] Parse AttributeValue\n"));
    bptr_Copy(&elms[1].asn, &io_name->val);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseRDName(const asn_elm_t          *in_elm,
              cert_l_name_t            *io_name)
{
    asn_elm_t elms[CERT_L_NAME_ELM_MAX_NUM];
    int       elm_num = CERT_L_NAME_ELM_MAX_NUM;
    size_t    sz;
    int       i;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_name);

    MSGI2(("[Step1] Parse ASN.1 Set\n"));
    exh_Set(asn_ParseSet(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);
    if (0 != sz) exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    MSGI2(("[Step2] Parse Each Attribute\n"));
    for (i = 0; i < elm_num; i++) {
        if (CERT_L_NAME_ELM_MAX_NUM == io_name->num)
            exh_Throw(RET_CERT_ERR_UNSUPPORTED);

        exh_Check(l_ParseNameElm(&elms[i], &io_name->elms[io_name->num++]));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseKeyUsage(const asn_elm_t        *in_elm,
                u_int32_t              *io_keyusage)
{
    asn_bit_str_t bstr;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_keyusage);

    RUN2(cert_l_DumpBptr(&in_elm->val, "keyusage elm"));
    if (ASN_TAG_BITSTRING != in_elm->tag) exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step1] ASN Parse KeyUsage\n"));
    exh_Set(asn_ParseBitString(in_elm, &bstr));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);

    *io_keyusage = ((*bstr.val.p) << 8);

    if (bstr.val.sz != 1)
        *io_keyusage += *(bstr.val.p + 1);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseBasic(const asn_elm_t           *in_elm,
             cert_l_tbs_t              *io_tbs)
{
    asn_elm_t elms[L_ELM_NUM_BASIC];
    int       elm_num = L_ELM_NUM_BASIC;
    size_t    sz;
    int       i;

    exh_InitOk();
    MSGENT();

    MSGI2(("----- basic elm -----\n")); DUMP2(in_elm->val.sz, in_elm->val.p);

    MSGI2(("[Step1] ASN Parse Basic\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || sz != 0 || 0 == elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    if (ASN_TAG_BOOLEAN != elms[0].tag) {
        io_tbs->basic_ca = 0;
        i = 0;
    } else {
        exh_Set(asn_ParseBoolean(&elms[0], &(io_tbs->basic_ca)));
        if (!exh_IsOk() || sz != 0) exh_Throw(RET_CERT_ERR_FMT);

        i = 1;
    }

    if (i + 1 <= elm_num) {
        if (i + 1 != elm_num || ASN_TAG_INTEGER != elms[i].tag)
            exh_Throw(RET_CERT_ERR_FMT);

        if (1 == elms[i].val.sz) {
            io_tbs->basic_pathlen = (u_int32_t) * (elms[i].val.p);
        } else {
            if (0x00 != *elms[i].val.p || 2 != elms[i].val.sz)
                exh_Throw(RET_CERT_ERR_FMT);

            io_tbs->basic_pathlen = (u_int32_t) * (elms[i].val.p + 1);
        }
    } else {
        io_tbs->basic_pathlen = 0xFFFFFFFF;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseValidity(const asn_elm_t        *in_elm,
                cert_l_tbs_t           *io_tbs)
{
    asn_elm_t  elms[L_ELM_NUM_VALIDITY];
    int        elm_num = L_ELM_NUM_VALIDITY;
    size_t     sz;
    asn_time_t ct;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] ASN Parse Validity\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || sz != 0 || L_ELM_NUM_VALIDITY != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse notBefore\n"));
    exh_Set(asn_ParseTime(&elms[0], &ct));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);
    exh_Set(time_ConvToUnixtime(ct.year, ct.mon, ct.mday,
                                ct.hour, ct.min, ct.sec,
                                &io_tbs->not_before));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step3] Parse notAfter\n"));
    exh_Set(asn_ParseTime(&elms[1], &ct));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);
    exh_Set(time_ConvToUnixtime(ct.year, ct.mon, ct.mday,
                                ct.hour, ct.min, ct.sec,
                                &io_tbs->not_after));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParsePubkey(const asn_elm_t          *in_elm,
              cert_l_tbs_t             *io_tbs)
{
    asn_elm_t     elms[L_ELM_NUM_PUBKEY];
    int           elm_num = L_ELM_NUM_PUBKEY;
    size_t        sz;
    asn_bit_str_t asn_key;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] ASN Parse Pubkey\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || sz != 0 || L_ELM_NUM_PUBKEY != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse algo\n"));
    if (ASN_TAG_SEQUENCE != elms[0].tag)
        exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&elms[0].asn, &io_tbs->pubkey_algo);

    MSGI2(("[Step3] Parse key\n"));
    exh_Set(asn_ParseBitString(&elms[1], &asn_key));
    if (!exh_IsOk() || asn_key.unused != 0)
        exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&asn_key.val, &io_tbs->pubkey);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseExtensions(cert_l_tbs_t         *io_tbs)
{
    asn_elm_t         elm;
    size_t            sz;
    cert_l_ext_elm_t *p;
    int               i;

    exh_InitOk();
    MSGENT();

    io_tbs->ext_flags = 0;

    for (i = 0; i < io_tbs->ext.num; i++) {
        p = &(io_tbs->ext.elms[i]);

        switch (p->id) {
        case CERT_L_EXT_TYPE_KEYUSAGE:
            if (0 != (io_tbs->ext_flags & CERT_L_EXT_FLAG_KEYUSAGE))
                exh_Throw(RET_CERT_ERR_FMT);

            io_tbs->ext_flags |= CERT_L_EXT_FLAG_KEYUSAGE;

            exh_Set(asn_elm_Parse(p->val.p, p->val.sz, &elm, NULL, &sz));
            if (!exh_IsOk() || sz != 0) exh_Throw(RET_CERT_ERR_FMT);

            exh_Check(l_ParseKeyUsage(&elm, &(io_tbs->keyusage)));
            continue;

        case CERT_L_EXT_TYPE_BASIC:
            if (0 != (io_tbs->ext_flags & CERT_L_EXT_FLAG_BASIC))
                exh_Throw(RET_CERT_ERR_FMT);

            io_tbs->ext_flags |= CERT_L_EXT_FLAG_BASIC;

            exh_Set(asn_elm_Parse(p->val.p, p->val.sz, &elm, NULL, &sz));
            if (!exh_IsOk() || sz != 0)
                exh_Throw(RET_CERT_ERR_FMT);

            exh_Check(l_ParseBasic(&elm, io_tbs));
            continue;

        case CERT_L_EXT_TYPE_SKEYID:
        case CERT_L_EXT_TYPE_AKEYID:
        case CERT_L_EXT_TYPE_CRLNUMBER:
        case CERT_L_EXT_TYPE_DISTPOINT:
        case CERT_L_EXT_TYPE_CRLDIST:
            MSGI2((" ext id 0x%08x Not Implemented\n", p->id));
            break;

        default:
            MSGI2((" ext id 0x%08x Not Supported\n", p->id));

            if (p->critical)
                exh_Throw(RET_CERT_ERR_FMT);
            break;
        }
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

static int
l_ParseTBS(asn_elm_t                   *in_elm,
           cert_l_tbs_t                *io_tbs)
{
    asn_elm_t elms[L_ELM_NUM_TBS];
    int       elm_num = L_ELM_NUM_TBS;
    asn_int_t aint;
    size_t    sz;
    int       r;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] ASN Parse TBSCertificate\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_TBS != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Check Version\n"));
    r = memcmp(elms[0].asn.p, L_TBS_VAL_VER, L_TBS_VAL_VER_SZ);
    if (0 != r) exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    io_tbs->ver = 2;

    MSGI2(("[Step3] Parse serialNumber\n"));
    /* In RFC3280 Serial must be positive */
    exh_Set(asn_ParseInteger(&elms[1], &aint));
    if (!exh_IsOk() || FALSE == aint.sign)
        exh_Throw(RET_CERT_ERR_FMT);
    bptr_Copy(&aint.val, &io_tbs->serial);

    MSGI2(("[Step4] Parse sig algo\n"));
    if (ASN_TAG_SEQUENCE != elms[2].tag) exh_Throw(RET_CERT_ERR_FMT);
    bptr_Copy(&elms[2].val, &io_tbs->sig_algo);

    MSGI2(("[Step5] Parse issuer\n"));
    exh_Check(cert_l_ParseName(&elms[3], &io_tbs->issuer));

    MSGI2(("[Step6] Parse Validity\n"));
    exh_Check(l_ParseValidity(&elms[4], io_tbs));

    MSGI2(("[Step7] Parse Subject\n"));
    exh_Check(cert_l_ParseName(&elms[5], &io_tbs->subject));

    MSGI2(("[Step8] Parse SubjectPublicKeyInfo\n"));
    exh_Check(l_ParsePubkey(&elms[6], io_tbs));

    MSGI2(("[Step9] Parse Extension\n"));
    MSGI3(("----- Extensions -----\n")); DUMP3(elms[7].asn.sz, elms[7].asn.p);
    if (0xa3 != elms[7].tag) exh_Throw(RET_CERT_ERR_FMT);
    exh_Check(cert_l_ParseExtensions(&elms[7].val, &(io_tbs->ext)));

    MSGI2(("[Step9-2] Parse Extension Value\n"));
    exh_Check(l_ParseExtensions(io_tbs));

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
cert_l_ParseExtensions(const bptr_t    *in_elm,
                       cert_l_ext_t    *io_ext)
{
    asn_elm_t elms[CERT_L_EXT_ELM_MAX_NUM];
    int       elm_num = CERT_L_EXT_ELM_MAX_NUM;
    size_t    sz;
    int       i;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_ext);

    io_ext->num = 0;

    bptr_Copy(in_elm, &io_ext->asn);

    MSGI2(("[Step1] Parse ASN.1 Sequence\n"));
    exh_Set(asn_ParseSeqBptr(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);
    if (0 != sz) exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    MSGI2(("[Step2] Parse Each Extension\n"));
    for (i = 0; i < elm_num; i++) {
        exh_Check(l_ParseExtensionElm(&elms[i], &io_ext->elms[i]));
        io_ext->num++;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_ParseName(const asn_elm_t       *in_elm,
                 cert_l_name_t         *io_name)
{
    asn_elm_t elms[L_RDNSEQUENCE_MAX_NUM];
    int       elm_num = L_RDNSEQUENCE_MAX_NUM;
    size_t    sz;
    int       i;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_name);

    io_name->num = 0;

    bptr_Copy(&in_elm->asn, &io_name->asn);

    MSGI2(("[Step1] Parse ASN.1 Sequence\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);
    if (0 != sz) exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    MSGI2(("[Step2] Parse Each RDN\n"));
    for (i = 0; i < elm_num; i++) {
        exh_Check(l_ParseRDName(&elms[i], io_name));
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_ParseAlgo(const asn_elm_t       *in_elm,
                 cert_l_algo_t         *io_algo)
{
    asn_elm_t elms[L_ELM_NUM_ALGO];
    int       elm_num = L_ELM_NUM_ALGO;
    size_t    sz;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_algo);

    MSGI2(("[Step1] ASN Parse AlgorithmIdentifier\n"));
    exh_Set(asn_ParseSeq(in_elm, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_ALGO - 1 > elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse algorithm\n"));
    if (ASN_TAG_OID != elms[0].tag) exh_Throw(RET_CERT_ERR_FMT);
    bptr_Copy(&elms[0].val, &io_algo->id);

    MSGI2(("[Step3] Parse parameter\n"));
    if (L_ELM_NUM_ALGO == elm_num) {
        bptr_Copy(&elms[1].asn, &io_algo->param);
    } else {
        bptr_t bptr_null = BPTR_NULL;

        bptr_Copy(&bptr_null, &io_algo->param);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_ParseSig(const asn_elm_t        *in_elm,
                cert_l_sig_t           *io_sig)
{
    asn_elm_t     elms[L_ELM_NUM_SIG];
    int           elm_num = L_ELM_NUM_SIG;
    asn_bit_str_t asn_sig;
    asn_int_t     asn_int;
    size_t        sz;
    int           r;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_elm && NULL != io_sig);

    r = memcmp(io_sig->algo.id.p, L_ALGO_SIG_ECDSA,
               sizeof(L_ALGO_SIG_ECDSA) - 1);
    if (0 != r) exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    io_sig->type = CERT_L_SIG_TYPE_ECDSA;

    MSGI2(("[Step1] Parse ASN.1 BitString \n"));
    exh_Set(asn_ParseBitString(in_elm, &asn_sig));
    if (!exh_IsOk() || 0 != asn_sig.unused)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step2] Parse ASN.1 in BitString \n"));
    exh_Set(asn_ParseSeqBptr(&asn_sig.val, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_SIG != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI2(("[Step3] Parse r\n"));
    exh_Set(asn_ParseInteger(&elms[0], &asn_int));
    if (!exh_IsOk() || FALSE == asn_int.sign)
        exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&asn_int.val, &io_sig->sig.ecdsa.r);

    MSGI2(("[Step4] Parse s\n"));
    exh_Set(asn_ParseInteger(&elms[1], &asn_int));
    if (!exh_IsOk() || FALSE == asn_int.sign)
        exh_Throw(RET_CERT_ERR_FMT);

    bptr_Copy(&asn_int.val, &io_sig->sig.ecdsa.s);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_Parse(const u_int8_t            *in_cert,
             size_t                     in_cert_len,
             cert_l_t                  *io_inf)
{
    asn_elm_t elms[L_ELM_NUM_CERT];
    int       elm_num = L_ELM_NUM_CERT;
    size_t    sz;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_cert && NULL != io_inf);

    MSGI3(("----- X509 Certificate ----\n")); DUMP3(in_cert_len, in_cert);

    MSGI2(("[Step1] ASN Parse Certificate Element\n"));
    exh_Set(asn_ParseSeqStr(in_cert, in_cert_len, elms, &elm_num, NULL, &sz));
    if (!exh_IsOk() || 0 != sz || L_ELM_NUM_CERT != elm_num)
        exh_Throw(RET_CERT_ERR_FMT);

    MSGI3(("----- TBSCertificate -----\n")); DUMP3(elms[0].asn.sz, elms[0].asn.p);
    MSGI3(("----- Sig Algo -----\n")); DUMP3(elms[1].asn.sz, elms[1].asn.p);
    MSGI3(("----- Sig -----\n")); DUMP3(elms[2].asn.sz, elms[2].asn.p);

    MSGI2(("[Step2] Parse TBSCertificate\n"));
    exh_Check(l_ParseTBS(&elms[0], &(io_inf->tbs)));
    bptr_Copy(&elms[0].asn, &io_inf->sig.data);

    MSGI2(("[Step3] Parse sig algo\n"));
    exh_Check(cert_l_ParseAlgo(&elms[1], &(io_inf->sig.algo)));

    MSGI2(("[Step4] Parse sig\n"));
    exh_Check(cert_l_ParseSig(&elms[2], &(io_inf->sig)));

    /* cert_l_DebugPrint(io_inf); */

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_ParsePKIPath(const u_int8_t     *in_pkipath,
                    size_t              in_pkipath_len,
                    cert_l_t          **io_inf,
                    int                *io_num)
{
    asn_elm_t elm;
    size_t sz;
    const u_int8_t *p;
    int i;
    int certbufnum;

    exh_InitOk();
    MSGENT();

    certbufnum = *io_num;

    *io_num = 0;

    MSGI2(("[Step1] Parse ASN \n"));
    exh_Set(asn_elm_Parse(in_pkipath, in_pkipath_len, &elm, NULL, &sz));
    if (!exh_IsOk() || sz != 0) exh_Throw(RET_CERT_ERR_FMT);
    if (ASN_TAG_SEQUENCE != elm.tag) exh_Throw(RET_CERT_ERR_FMT);

    p  = elm.val.p;
    sz = elm.val.sz;

    for (i = 0; i  < certbufnum ; i++) {
        MSGI2(("[Step%d] Parse Certificate[%d]\n", i + 2, i));

        *io_num += 1;

        exh_Set(asn_elm_Parse(p, sz, &elm, &p, &sz));
        if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_FMT);

        exh_Check(cert_l_Parse(elm.asn.p, elm.asn.sz, *io_inf++));
    }

    if (0 != sz) exh_Throw(RET_CERT_ERR_FMT);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
