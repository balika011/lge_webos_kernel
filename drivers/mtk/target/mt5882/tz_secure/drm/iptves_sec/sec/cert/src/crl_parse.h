/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CRL_PARSE_H
#define CRL_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

extern int
crl_l_Parse(const u_int8_t     *in_crl,
            size_t              in_crl_len,
            crl_l_t            *io_inf);

extern int
crl_l_ParseRListElm(const asn_elm_t    *in_elm,
                    crl_l_rlist_elm_t  *io_elm);

#ifdef __cplusplus
}
#endif

#endif /* CRL_PARSE_H */
