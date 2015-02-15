/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CERT_INF_H
#define CERT_INF_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================
  Extensions
  ============================================================*/

enum cert_l_ext_type_enum {
    CERT_L_EXT_TYPE_SKEYID     = 0x0e,
    CERT_L_EXT_TYPE_KEYUSAGE   = 0x0f,
    CERT_L_EXT_TYPE_BASIC      = 0x13,
    CERT_L_EXT_TYPE_CRLNUMBER  = 0x14,
    CERT_L_EXT_TYPE_DISTPOINT  = 0x1c,
    CERT_L_EXT_TYPE_CERTISSUER = 0x1d,
    CERT_L_EXT_TYPE_CRLDIST    = 0x1f,
    CERT_L_EXT_TYPE_AKEYID     = 0x23,
};

typedef struct cert_l_ext_elm_tag {
    u_int8_t id;
    bptr_t   val;
    bool_t   critical;
} cert_l_ext_elm_t;

#define CERT_L_EXT_ELM_MAX_NUM 5

typedef struct cert_l_ext_tag {
    cert_l_ext_elm_t elms[CERT_L_EXT_ELM_MAX_NUM];
    int              num;
    bptr_t           asn;
} cert_l_ext_t;

/*============================================================
  Name
  ============================================================*/

typedef struct cert_l_name_elm_tag {
    bptr_t type;
    bptr_t val;
} cert_l_name_elm_t;

#define CERT_L_NAME_ELM_MAX_NUM 1

typedef struct cert_l_name_tag {
    cert_l_name_elm_t elms[CERT_L_NAME_ELM_MAX_NUM];
    int               num;
    bptr_t            asn;
} cert_l_name_t;

/*============================================================
  Signature
  ============================================================*/

typedef struct cert_l_sig_algo_tag {
    bptr_t id;
    bptr_t param;
} cert_l_algo_t;

enum cert_l_sig_type_enum {
    CERT_L_SIG_TYPE_ECDSA = 1,
};

typedef struct cert_l_sig_tag {
    u_int8_t      type;
    bptr_t        data;
    cert_l_algo_t algo;
    union {
        struct {
            bptr_t r;
            bptr_t s;
        } ecdsa;
    } sig;
} cert_l_sig_t;

/*============================================================
  Certificate
  ============================================================*/
enum cert_l_ext_keyusage_enum {
    CERT_L_KEYUSAGE_DIGITALSIGNATURE = 0x8000,
    CERT_L_KEYUSAGE_NONREPUDATION    = 0x4000,
    CERT_L_KEYUSAGE_KEYENCIPHERMENT  = 0x2000,
    CERT_L_KEYUSAGE_DATAENCIPHERMENT = 0x1000,
    CERT_L_KEYUSAGE_KEYAGREEMENT     = 0x0800,
    CERT_L_KEYUSAGE_KEYCERTSIGN      = 0x0400,
    CERT_L_KEYUSAGE_CRLSIGN          = 0x0200,
    CERT_L_KEYUSAGE_ENCIPHERMENTONLY = 0x0100,
    CERT_L_KEYUSAGE_DECIPEHRONLY     = 0x80,
};

#define CERT_L_EXT_FLAG_KEYUSAGE  0x1
#define CERT_L_EXT_FLAG_BASIC     0x2

typedef struct cert_l_tbs_tag {
    u_int32_t     ver;
    bptr_t        serial;
    cert_l_name_t issuer;
    bptr_t        sig_algo;
    time_unix_t   not_before;
    time_unix_t   not_after;
    cert_l_name_t subject;
    bptr_t        pubkey_algo;
    bptr_t        pubkey;
    cert_l_ext_t  ext;
    u_int32_t     ext_flags;
    u_int32_t     keyusage;
    bool_t        basic_ca;
    u_int32_t     basic_pathlen;
} cert_l_tbs_t;

typedef struct cert_l_tag {
    cert_l_tbs_t tbs;
    cert_l_sig_t sig;
} cert_l_t;

extern void
cert_l_DumpBptr(const bptr_t *in_p, char *in_s_p);

extern void
cert_l_DebugPrintExt(const cert_l_ext_t *in_ext_p);

extern void
cert_l_DebugPrintName(const cert_l_name_t *in_name_p);

extern void
cert_l_DebugPrintSig(const cert_l_sig_t *in_sig_p);

extern void
cert_l_DebugPrint(const cert_l_t *in_cert_p);

#ifdef __cplusplus
}
#endif

#endif /* CERT_INF_H */
