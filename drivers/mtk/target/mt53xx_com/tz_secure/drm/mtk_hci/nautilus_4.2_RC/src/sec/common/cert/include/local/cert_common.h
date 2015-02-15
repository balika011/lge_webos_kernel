/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010 Sony Corporation.
 */


#ifndef CERT_COMMON_H
#define CERT_COMMON_H


/**
 * Parse AlgorithmIdentifier.
 *
 * AlgorithmIdentifier ::= SEQUENCE {
 *      algorithm       OBJECT IDENTIFIER
 *      parameters      ANY DEFINED BY algorithm OPTIONAL
 */
retcode_t
cert_l_ParseAlgorithmIdentifier(asn_t         *in_asn_algo,
                                cert_l_algo_t *out_algo);

/**
 * Parse Time.
 *
 * Time ::= CHOICE {
 *      utcTime        UTCTime,
 *      generalTime    GeneralizedTime }
 *
 */
retcode_t
cert_l_ParseTime(asn_t       *in_asn_time,
                 time_date_t *out_time);

/**
 * Parse Name.
 * @note that a parsed name is not copied from ASN's buffer.
 */
retcode_t
cert_l_ParseName(asn_t    *in_asn_name,
                 binstr_t *out_name);


/**
 * Free Extentions
 */
retcode_t
cert_l_FreeExtentions(cert_l_exts_t *io_exts);


/**
 * Parse Extentions
 *
 *  Extentions ::= SEQUENCE SIZE (1..MAX) OF Extension
 */
retcode_t
cert_l_ParseExtentions(asn_t         *in_asnexts,
                       cert_l_exts_t **out_exts);

/**
 * Parse AuthorityKeyIdentifier
 *  AuthorityKeyIdentifier ::= SEQUENCE {
 *      keyIdentifier [0]               KeyIdentifier OPTIONAL
 *      authorityCertIssuer [1]         GeneralName OPTINAL
 *      authorityCertSerialNumber [2]   CertificateSerialNumber OPTIONAL
 * }
 */
retcode_t
cert_l_ParseAuthorityKeyIdentifier(binstr_t                *in_buf,
                                   cert_l_authoritykeyid_t *out_authorityid);

/**
 * This function compares 2 dates.
 * 
 * @return ::ERR_OK if in_bt < in_at
 * @return ::ERR_CERT_ILLEGALCERT otherwise.
 */
retcode_t
cert_l_CmpTime(time_date_t *in_bt, 
               time_date_t *in_at);


#endif /* CERT_COMMON_H */

