/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef CERT_PARSE_H
#define CERT_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

extern int
cert_l_ParseSig(const asn_elm_t        *in_elm,
                cert_l_sig_t           *io_sig);

extern int
cert_l_ParseAlgo(const asn_elm_t       *in_elm,
                 cert_l_algo_t         *io_algo);

extern int
cert_l_ParseName(const asn_elm_t       *in_asn,
                 cert_l_name_t         *io_name);

extern int
cert_l_ParseExtensions(const bptr_t    *in_asn,
                       cert_l_ext_t    *io_ext);

extern int
cert_l_Parse(const u_int8_t            *in_cert,
             size_t                     in_cert_len,
             cert_l_t                  *io_inf);

extern int
cert_l_ParsePKIPath(const u_int8_t     *in_pkipath,
                    size_t              in_pkipath_len,
                    cert_l_t          **io_inf,
                    int                *io_num);

#ifdef __cplusplus
}
#endif

#endif /* CERT_PARSE_H */
