/**
 * RSA
 * Copyright (c) 2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef RSAPKCS_H
#define RSAPKCS_H
#include "svc_wv_if.h"


typedef struct crypto_rsa_key {
	int private_key; /** whether private key is set */
	 mp_int *e; /** public exponent */
	/** The following parameters are available only if private_key is set */
	 mp_int *d; /** private exponent */
	mp_int *n; /** modulus (p * q) */
	 mp_int *p; /** prime p (factor of n) */
	 mp_int *q; /** prime q (factor of n) */
	 mp_int *iqmp; /** 1 / q mod p; CRT coefficient */
	 mp_int *dmp1; /** d mod (p - 1); CRT exponent */
	 mp_int *dmq1; /** d mod (q - 1); CRT exponent */
	 
}crypto_rsa_key;


crypto_rsa_key * crypto_rsa_import_public_key(const u8 *buf, size_t len);
crypto_rsa_key * crypto_rsa_import_private_key(const u8 *buf, size_t len);
size_t crypto_rsa_get_modulus_len(crypto_rsa_key *key);
int crypto_rsa_exptmod(const u8 *in, size_t inlen, u8 *out, size_t *outlen,
		        crypto_rsa_key *key, int use_private);
void crypto_rsa_free(crypto_rsa_key *key);

#endif /** RSAPKCS_H */

