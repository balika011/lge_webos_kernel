/** @addtogroup cert cert
 *  @ingroup common
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */
#ifndef CERT_TYPES_H
#define CERT_TYPES_H

/*---------------------------------------------------------------------------*
 * Define
 *---------------------------------------------------------------------------*/
/* ALGORITHM OIDS */
#define CERT_L_ALGOOID_SHA1WITHRSAENC   "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x05"
#define CERT_L_ALGOOID_SHA256WITHRSAENC "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x0B"
#define CERT_L_ALGOOID_RSAENC           "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01"

/* EXTENTION OIDS */
#define CERT_L_EXTOID_AUTHORITYKEYIDENTIFIER    "\x55\x1d\x23"
#define CERT_L_EXTOID_SUBJECTKEYIDENTIFIER      "\x55\x1d\x0e"
#define CERT_L_EXTOID_KEYUSAGE                  "\x55\x1d\x0f"
#define CERT_L_EXTOID_BASICCONSTRAINTS          "\x55\x1d\x13"
#define CERT_L_EXTOID_CRLDISTRIBUTIONPOINTS     "\x55\x1d\x1f"
#define CERT_L_EXTOID_CERTIFICATEPOLICIES       "\x55\x1d\x20"
#define CERT_L_EXTOID_CRLNUMBER                 "\x55\x1d\x14"
#define CERT_L_EXTOID_CERTIFICATEISSUER         "\x55\x1d\x1d"
#define CERT_L_EXTOID_ISSUINGDISTRIBUTIONPOINT  "\x55\x1d\x1c"

/* KEYUSAGE VALUES */
#define CERT_L_KEYUSAGEVAL_DIGITALSIGNATURE     "\x03\x02\x07\x80"
#define CERT_L_KEYUSAGEVAL_NONREPUDIATION       "\x03\x02\x06\x40"
#define CERT_L_KEYUSAGEVAL_KEYENCIPHERMENT      "\x03\x02\x05\x20"
#define CERT_L_KEYUSAGEVAL_DATAENCIPHERMENT     "\x03\x02\x04\x10"
#define CERT_L_KEYUSAGEVAL_KEYAGREEMENT         "\x03\x02\x03\x08"
#define CERT_L_KEYUSAGEVAL_KEYCERTSIGN          "\x03\x02\x02\x04"
#define CERT_L_KEYUSAGEVAL_KEYCSIG_DSIG         "\x03\x02\x02\x84"    /* Digital Signature and Cert Signing */
#define CERT_L_KEYUSAGEVAL_DENC_KENC            "\x03\x02\x04\x30"    /* Data and Key encipherment */

/* Current version of Nautilus doesn't support >1 key usage for a key =>
 * the processing of key usages doesn't expect to dinf more than one flag =>
 * we have to provide all possible values for the same key (see also
 * cert_l_ParseKeyUsage() in cert.c) */

#define CERT_L_KEYUSAGEVAL_CRLSIGN_1            "\x03\x02\x01\x02"
#define CERT_L_KEYUSAGEVAL_CRLSIGN_2            "\x03\x02\x01\x80"
#define CERT_L_KEYUSAGEVAL_CRLSIGN_3            "\x03\x02\x01\x82"

#define CERT_L_KEYUSAGEVAL_ENCIPHERONLY         "\x03\x02\x00\x01"
#define CERT_L_KEYUSAGEVAL_DECIPHERONLY         "\x03\x03\x07\x00\x80"

#if defined(NPOPT_USE_ARK)
#define NS_HARDCODED_CRL_DP_VAL "http://crl.marlin-tmo.com/crl/mtmocrls.crl"
#else
#define NS_HARDCODED_CRL_DP_VAL "http://testcrl.marlin-tmo.com/testcrl/mtmocrls.crl"
#endif
/*---------------------------------------------------------------------------*
 * Structure
 *---------------------------------------------------------------------------*/
typedef struct {
    int32_t     type;
    u_int8_t    *val;
    np_size_t   val_len;
} cert_l_type_t;

/*
 * Cert
 */
typedef struct {
    int32_t     num;
    int32_t     max_num;
    binstr_t    **oid;
} cert_l_policies_t;

typedef struct {
    binstr_t            keyid;
} cert_l_authoritykeyid_t;

typedef struct {
    binstr_t            keyid;
} cert_l_subjectkeyid_t;

typedef struct {
    int32_t             ca;             /* TRUE or FALSE */
    int32_t             pathlen;        /* if pathlen == -1, unlimited */
} cert_basicconsts_t;

typedef struct {
    int32_t             exttype;
    binstr_t            extoid;
    int32_t             critical;
    binstr_t            extval;
} cert_l_ext_t;

typedef struct {
    int32_t             num;
    cert_l_ext_t        **ext;
} cert_l_exts_t;

typedef struct {
    int32_t             algotype;
    binstr_t            algoid;
    binstr_t            param;
} cert_l_algo_t;

typedef struct {
    time_date_t         notbefore;
    time_date_t         notafter;
} cert_l_validity_t;

typedef struct {
    cert_l_algo_t       algo;
    binstr_t            key;
} cert_l_pubkey_t;

typedef struct {
    binstr_t            body;
    int32_t             version;
    binstr_t            serialnumber;
    cert_l_algo_t       signature;
    binstr_t            issuer;
    cert_l_validity_t   validity;
    binstr_t            subject;
    cert_l_pubkey_t     pubkey;
    cert_l_exts_t       *exts;
} cert_l_tbscert_t;

typedef struct {
    cert_l_tbscert_t    *tbscert;
    cert_l_algo_t       sigalgo;
    binstr_t            signature;
} cert_l_certificate_t;

/*
 * Context
 */
typedef struct {
    binstr_t                    *body;
    cert_l_certificate_t        *cert;
} cert_l_context_t;

/*
 * Work
 */
typedef struct {
    int32_t             validityflag;
    int32_t             check_in_crl;
    time_date_t         curtime;
    int32_t             max_depth;
    cert_l_policies_t   *user_policies;
    int32_t             user_keyusage;
    int32_t             max_path_len;    
    cert_l_policies_t   *exp_policies;
    struct {
        int32_t         cur_depth;
        binstr_t        *issuer_name;
        binstr_t        *issuer_keyn;
        binstr_t        *issuer_keye;
        binstr_t        *issuer_keyid;
    } data; /* current cert's data */
} cert_l_verify_context_t;

/*---------------------------------------------------------------------------*
 * If the cert module separate into the module depending on The X509 spec and
 * the module depending on The Marlin Spec, the following definitions are
 * moved to cert_api.h
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Define
 *---------------------------------------------------------------------------*/
/* KeyUsage Types */
#define CERT_KEYUSAGE_DIGITALSIGNATURE         1
#define CERT_KEYUSAGE_NONREPUDIATION           2
#define CERT_KEYUSAGE_KEYENCIPHERMENT          4
#define CERT_KEYUSAGE_DATAENCIPHERMENT         8
#define CERT_KEYUSAGE_KEYAGREEMENT            16
#define CERT_KEYUSAGE_KEYCERTSIGN             32
#define CERT_KEYUSAGE_CRLSIGN                 64
#define CERT_KEYUSAGE_ENCIPHERONLY           128
#define CERT_KEYUSAGE_DECIPHERONLY           256

/* Algorithm Types */
#define CERT_ALGOTYPE_UNKNOWN                   0
#define CERT_ALGOTYPE_SHA1WITHRSAENC            1
#define CERT_ALGOTYPE_SHA256WITHRSAENC          2
#define CERT_ALGOTYPE_RSAENCRYPTION             3

/* Extention Types */
#define CERT_EXTTYPE_UNKNOWN                    0
#define CERT_EXTTYPE_AUTHORITYKEYIDENTIFIER     1
#define CERT_EXTTYPE_SUBJECTKEYIDENTIFIER       2
#define CERT_EXTTYPE_KEYUSAGE                   3
#define CERT_EXTTYPE_BASICCONSTRAINTS           4
#define CERT_EXTTYPE_CRLDISTRIBUTIONPOINTS      5
#define CERT_EXTTYPE_CERTIFICATEPOLICIES        6
#define CERT_EXTTYPE_CRLNUMBER                  7
#define CERT_EXTTYPE_CERTIFICATEISSUER          8
#define CERT_EXTTYPE_ISSUINGDISTRIBUTIONPOINT   9

/* CERTPOLICY */
#define CERT_CERTPOLICY_ANYPOLICY             "\x55\x1d\x20\x00"

/*---------------------------------------------------------------------------*
 * Structure
 *---------------------------------------------------------------------------*/
/**
 * SetUp Info
 */  
typedef struct {
    int32_t             num;
    binstr_t            *oid;
} cert_policies_t;

typedef struct {
    int32_t             algo;
    union {
        struct {
            binstr_t    n;
            binstr_t    e;
        } rsaenc;
    } key;
} cert_pubkey_t;

typedef struct {
    int32_t             depth;        /* the depth of cert chain */    
    int32_t             validityflag; /* whether the cert validity is checked or not (TRUE or FALSE)*/
    time_date_t         curtime;      /* the current time */
    binstr_t            anchorname;   /* the subject name of the trust anchor */
    cert_pubkey_t       anchorkey;    /* the publickey of trust anchor */
    cert_policies_t     policies;     /* acceptable cert policies list */
    int32_t             keyusage;     /* acceptable keyusage */
    int32_t             check_in_crl; /* TRUE if the cert must be checked against the
                                         CRL, FALSE otherwise. */
} cert_setup_info_t;

/*
 * Verify the cert chain.
 *
 * @param [in] in_info  the information for verification
 * @param [in] in_certs certificates on the path of the cert chain.
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_NOMEM
 * @return ::ERR_ERT_ILLEGALCERT
 */
retcode_t
cert_VerifyChain(cert_setup_info_t      *in_info,
                 cert_chain_t           *in_certs);

#endif /* CERT_TYPES_H */
/** @} */
