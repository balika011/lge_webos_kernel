/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef CERT_VERIFY_H
#define CERT_VERIFY_H

#ifdef __cplusplus
extern "C" {
#endif

extern int
cert_l_VerifySignature(const cert_l_sig_t *in_sig,
                       const bptr_t       *in_pubkey);

extern int
cert_l_VerifyTA(cert_l_t               *in_ta,
                time_unix_t            *in_time);

extern int
cert_l_VerifyCert(cert_l_t             *in_subject,
                  cert_l_t             *in_issuer,
                  time_unix_t          *in_time);

#ifdef __cplusplus
}
#endif

#endif /* CERT_VERIFY_H */




