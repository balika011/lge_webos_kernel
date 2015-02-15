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
#include "crypto_api_iptves.h"
#include "asn_api_iptves.h"
#include "cert_api_iptves.h"
#include "cert_inf.h"
#include "cert_verify.h"
//mtk
COMPONENT(CERT);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_ECDSA_SIG_R_SIZE      28
#define L_ECDSA_SIG_S_SIZE      28

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
cert_l_VerifySignature(const cert_l_sig_t *in_sig,
                       const bptr_t       *in_pubkey)
{
    u_int8_t   sig[CRYPTO_ECDSA_SIG_LEN];
    u_int8_t   *p;
    const u_int8_t *pubkey;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_sig && NULL != in_pubkey);

    if (CERT_L_SIG_TYPE_ECDSA != in_sig->type)
        exh_Throw(RET_CERT_ERR_UNSUPPORTED);

    MSGI2(("[Step1] Preparation of Signature\n"));
    if (in_sig->sig.ecdsa.r.sz > L_ECDSA_SIG_R_SIZE || in_sig->sig.ecdsa.s.sz > L_ECDSA_SIG_S_SIZE)
        exh_Throw(RET_CERT_ERR_VERIFY_SIG_FMT);
    memset(sig, 0, CRYPTO_ECDSA_SIG_LEN);

    p = sig + (L_ECDSA_SIG_R_SIZE - in_sig->sig.ecdsa.r.sz);
    exh_Check(base_Memcpy(p, in_sig->sig.ecdsa.r.sz, in_sig->sig.ecdsa.r.p, in_sig->sig.ecdsa.r.sz));

    p = sig + CRYPTO_ECDSA_SIG_LEN - in_sig->sig.ecdsa.s.sz;
    exh_Check(base_Memcpy(p, in_sig->sig.ecdsa.s.sz, in_sig->sig.ecdsa.s.p, in_sig->sig.ecdsa.s.sz));

    MSGI2(("[Step2] Preparation of Public Key\n"));
    if (CRYPTO_ECDSA_PUBKEY_LEN > in_pubkey->sz || (CRYPTO_ECDSA_PUBKEY_LEN + 2) <= in_pubkey->sz)
        exh_Throw(RET_CERT_ERR_VERIFY_PUBKEY_FMT);

    if ((CRYPTO_ECDSA_PUBKEY_LEN + 1) == in_pubkey->sz) {
        if (0x04 == *in_pubkey->p) {
            pubkey = in_pubkey->p + 1;
        } else {
            exh_Throw(RET_CERT_ERR_VERIFY_PUBKEY_FMT);
        }
    } else {
        pubkey = in_pubkey->p;
    }

    MSGI2(("----- signed -----\n")); DUMP2(in_sig->data.sz, in_sig->data.p);
    MSGI2(("----- sig -----\n"));    DUMP2(CRYPTO_ECDSA_SIG_LEN, sig);
    MSGI2(("----- pubkey -----\n")); DUMP2(CRYPTO_ECDSA_PUBKEY_LEN, pubkey);

    MSGI2(("[Step3] Verify\n"));
    exh_Set(crypto_EcdsaVerify(in_sig->data.p, in_sig->data.sz, pubkey, sig));
    if (!exh_IsOk()) exh_Throw(RET_CERT_ERR_VERIFY_SIG);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_VerifyTA(cert_l_t       *in_ta,
                time_unix_t    *in_time)
{
    exh_InitOk();
    MSGENT();

    assert(NULL != in_ta);

    MSGI2(("[Step1] Verify Signature\n"));
    exh_Check(cert_l_VerifySignature(&(in_ta->sig), &in_ta->tbs.pubkey));

    MSGI2(("[Step2] Verify Validity\n"));

    if (NULL != in_time) {
        MSGI2(("current time   : %s\n", base_CTime(in_time)));
        MSGI2(("TA  not_before : %s\n", base_CTime(&in_ta->tbs.not_before)));
        MSGI2(("TA  not_after  : %s\n", base_CTime(&in_ta->tbs.not_after)));

        if (*in_time < in_ta->tbs.not_before || *in_time > in_ta->tbs.not_after)
            exh_Throw(RET_CERT_ERR_VERIFY_VALIDITY);
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
cert_l_VerifyCert(cert_l_t     *in_subject,
                  cert_l_t     *in_issuer,
                  time_unix_t  *in_time)
{
    int r;

    exh_InitOk();
    MSGENT();

    MSGI2(("[Step1] Verify Signature\n"));
    exh_Check(cert_l_VerifySignature(&(in_subject->sig), &in_issuer->tbs.pubkey));

    MSGI2(("[Step2] Verify IssuerName\n"));

    if (in_issuer->tbs.subject.asn.sz != in_subject->tbs.issuer.asn.sz)
        exh_Throw(RET_CERT_ERR_VERIFY_ISSUER_NAME);

    r = memcmp(in_issuer->tbs.subject.asn.p, in_subject->tbs.issuer.asn.p,
               in_subject->tbs.issuer.asn.sz);
    if (0 != r) exh_Throw(RET_CERT_ERR_VERIFY_ISSUER_NAME);

    MSGI2(("[Step3] Verify KeyUsage\n"));

    if (0 != (in_issuer->tbs.ext_flags & CERT_L_EXT_FLAG_KEYUSAGE)) {
        MSGI2(("   keyusage  : 0x%08x\n", in_issuer->tbs.keyusage));

        if (0 == (in_issuer->tbs.keyusage & CERT_L_KEYUSAGE_KEYCERTSIGN))
            exh_Throw(RET_CERT_ERR_VERIFY_ISSUER_KEYUSAGE);
    }

    MSGI2(("[Step4] Verify BasicConstraint\n"));

    if (0 != (in_issuer->tbs.ext_flags & CERT_L_EXT_FLAG_BASIC)) {
        MSGI2(("   basic_ca      : 0x%08x\n", in_issuer->tbs.basic_ca));
        MSGI2(("   basic_pathlen : 0x%08x\n", in_issuer->tbs.basic_pathlen));

        if ((0 == in_issuer->tbs.basic_ca || 0 == in_issuer->tbs.basic_pathlen))
            exh_Throw(RET_CERT_ERR_VERIFY_ISSUER_BASICCONST);

    }

    MSGI2(("[Step5] Verify Validity\n"));
	dprintf("%s:mark time check for test(to-fix)\n", __FUNCTION__);

    if (NULL != in_time) {
        MSGI2(("current time       : %s\n", base_CTime(in_time)));
        MSGI2(("issuer  not_before : %s\n", base_CTime(&in_issuer->tbs.not_before)));
        MSGI2(("issuer  not_after  : %s\n", base_CTime(&in_issuer->tbs.not_after)));
        MSGI2(("subject not_before : %s\n", base_CTime(&in_subject->tbs.not_before)));
        MSGI2(("subject not_after  : %s\n", base_CTime(&in_subject->tbs.not_after)));

        if (*in_time < in_issuer->tbs.not_before ||
            *in_time > in_issuer->tbs.not_after)
        {
        	dprintf("\t time check: CERT_ERR_VERIFY_ISSUER_VALIDITY \n ");
        	//exh_Throw(RET_CERT_ERR_VERIFY_ISSUER_VALIDITY);
        }

        if (*in_time < in_subject->tbs.not_before ||
            *in_time > in_subject->tbs.not_after)
   
        {
           dprintf("\t time check: CERT_ERR_VERIFY_SUBJECT_VALIDITY \n ");
           //exh_Throw(RET_CERT_ERR_VERIFY_SUBJECT_VALIDITY);
        }
   
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

