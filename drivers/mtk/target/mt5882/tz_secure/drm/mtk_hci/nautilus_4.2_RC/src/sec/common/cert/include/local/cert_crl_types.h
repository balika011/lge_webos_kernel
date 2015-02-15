/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */


#ifndef CERT_CRL_TYPES_H
#define CERT_CRL_TYPES_H


#include <limits.h>


typedef struct {
    binstr_t certissuer;
} cert_l_crlentryexts_t;


typedef struct {
    binstr_t              serialnumber; 
    time_date_t           revocationdate;
    cert_l_crlentryexts_t entryexts;
} cert_l_revokedcert_t;


typedef struct {
    int32_t                num;
    cert_l_revokedcert_t** certs;
} cert_l_revokedcerts_t;


/* Max. number of bits in ReasonFlags - see rfc3280. */
#define L_MAX_BITS_IN_REASONFLAGS (9)
#define L_SIZE_REASONFLAGS_BUF ((L_MAX_BITS_IN_REASONFLAGS + CHAR_BIT - 1) / CHAR_BIT)

typedef struct {
    unsigned char buf[L_SIZE_REASONFLAGS_BUF];
    np_size_t     numbits;
} cert_l_reasonflags_t;


typedef struct {
    bool_t               onlycontainsusercerts;
    bool_t               onlycontainscacerts;
    cert_l_reasonflags_t reasonflags;
    bool_t               indirectcrl;
    bool_t               onlycontainsattrcerts;
} cert_l_issuingdp_t;

typedef struct {
    cert_l_authoritykeyid_t  authkeyid;
    crlnumber_t      crlnumber;
    cert_l_issuingdp_t       idp;
} cert_l_crlexts_t;


typedef struct {
    binstr_t               body;
    int32_t                version;
    cert_l_algo_t          signature;
    binstr_t               issuer;
    time_date_t            thisupdate;
    time_date_t            nextupdate;
    cert_l_revokedcerts_t* revokedcerts;
    cert_l_crlexts_t       crlexts;
} cert_l_tbscertlist_t;


typedef struct {
    cert_l_tbscertlist_t* tbscertlist;
    cert_l_algo_t         sigalgo;
    binstr_t              signature;
} cert_l_crl_t;


typedef struct {
    binstr_t*     body;
    cert_l_crl_t* crl;
} cert_l_crlcontext_t;


/**
 * This function parses a given CRL.
 *
 * @param [in]  in_crl_buf  A pointer to a buffer to be parsed.
 * @param [out] out_crl_ctx A pointer to a buffer which will hold a pointer
 *                          to the parsed cert upon return.
 *
 * @return ::ERR_OK A given CRL has been successfully parsed.
 * @return ::ERR_NOMEM If it is not enough memory to complete the operation.
 * @return ::ERR_CERT_ILLEGALCRL If a given CERT doesn't follow the specs.
 * @return ::ERR_INTERNALERROR In all other cases.
 */
retcode_t
cert_l_ParseCRL(binstr_t*             in_crl_buf,
                cert_l_crlcontext_t** out_crl_ctx);


/**
 * This function destroys a given CRL object.
 * 
 * @param [in] in_crl_ctx A pointer to a CRL context returned by
 *                        the cert_l_ParseCRL().
 *
 * @return ::ERR_OK
 */
retcode_t
cert_l_FreeCRL(cert_l_crlcontext_t* in_crl_ctx);


/**
 * This function checks if a given CRL is valid.
 *
 * @note if a given CRL is NULL, it is considered as invlalid.
 *
 * @param [in]     in_crl_ctx  A pointer to CR which status is being checked.
 * @param [in,out] io_is_valid A pointer to a buffer which will contain TRUE
 *                             if the CRL is valid and FALSE otherwise. This
 *                             value is trusted only when function returns
 *                             ::ERR_OK.
 *
 * @return ::ERR_OK
 * @return ::ERR_TIME_OUTOFRANGE If the time is out of range. 
 * @return ::ERR_CERT_ILLEGALCERT Illegal Certificate.
 * @return ::ERR_ILLEGALARGS The input arguments are incorrect. Arguments cannot be NULL
 * @return ::ERR_INTERNALERROR An unrecoverable error has occurred in Nautilus.
 */
retcode_t
cert_l_IsCRLValid(cert_l_crlcontext_t* in_crl_ctx,
                  bool_t*              io_is_valid);


#endif /* CERT_CRL_TYPES_H */

