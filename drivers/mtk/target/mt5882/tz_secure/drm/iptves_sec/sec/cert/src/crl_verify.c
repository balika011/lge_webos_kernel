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
#include "cert_verify.h"
#include "crl_inf.h"
#include "crl_verify.h"

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

/*============================================================================
 * Global Functions
 *==========================================================================*/
int
crl_l_Verify(crl_l_t           *in_crl,
             cert_l_t          *in_issuer,
             u_int32_t         *in_t)
{
    int r;

    exh_InitOk();
    MSGENT();

    assert(NULL != in_crl && NULL != in_issuer);

    MSGI3(("[Step1] Verify Signature\n"));
    exh_Set(cert_l_VerifySignature(&in_crl->sig, &in_issuer->tbs.pubkey));
    if (!(exh_IsOk())) exh_Throw(RET_CRL_ERR_VERIFY_SIG);

    MSGI3(("[Step2] Verify IssuerName\n"));
    if (in_issuer->tbs.subject.asn.sz != in_crl->tbs.issuer.asn.sz)
        exh_Throw(RET_CRL_ERR_VERIFY_SIG);

    r = memcmp(in_issuer->tbs.subject.asn.p, in_crl->tbs.issuer.asn.p,
               in_crl->tbs.issuer.asn.sz);
    if (0 != r) exh_Throw(RET_CRL_ERR_VERIFY_SIG);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
