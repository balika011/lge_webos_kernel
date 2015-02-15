/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CRL_INF_H
#define CRL_INF_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================
  RevocationList
  ============================================================*/

typedef struct crl_l_rlist_elm_tag {
    bptr_t        serial;
    u_int32_t     date;
    cert_l_name_t issuer;
    cert_l_ext_t  ext;
} crl_l_rlist_elm_t;

#define CERT_L_RLIST_ELM_MAX_NUM 100

extern void
crl_l_DebugPrintRevocationList(bptr_t *in_rlist_p);

/*============================================================
  Name
  ============================================================*/

typedef struct crl_l_tbs_tag {
    u_int32_t     ver;
    bptr_t        sig_algo;
    cert_l_name_t issuer;
    time_unix_t   this_update;
    u_int32_t     next_update;
    bptr_t        rlist;
    cert_l_ext_t  ext;
} crl_l_tbs_t;

typedef struct crl_l_tag {
    crl_l_tbs_t  tbs;
    cert_l_sig_t sig;
} crl_l_t;

extern void
crl_l_DebugPrint(crl_l_t *in_crl_p);

#ifdef __cplusplus
}
#endif

#endif /* CRL_INF_H */
