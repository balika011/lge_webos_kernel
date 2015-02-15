/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef CRL_VERIFY_H
#define CRL_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

extern int
crl_l_Verify(crl_l_t           *in_crl,
             cert_l_t          *in_issuer,
             u_int32_t         *in_t);

#ifdef __cplusplus
}
#endif

#endif /* CRL_VERIFY_H */




