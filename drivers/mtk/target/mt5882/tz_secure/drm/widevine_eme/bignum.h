/**
 * Big number math
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

#ifndef BIGNUM_H
#define BIGNUM_H
#include "svc_wv_if.h"

//#include <tommath.h>

/* the infamous mp_int structure */
//typedef struct  {
//    int used, alloc, sign;
//    unsigned short *dp;
//}mp_int;

mp_int * bignum_init(void);
void bignum_deinit( mp_int *n);
size_t bignum_get_unsigned_bin_len( mp_int *n);
int bignum_get_unsigned_bin(const  mp_int *n, u8 *buf, size_t *len);
int bignum_set_unsigned_bin( mp_int *n, const u8 *buf, size_t len);
int bignum_cmp(const  mp_int *a, const  mp_int *b);
int bignum_cmp_d(const  mp_int *a, unsigned long b);
int bignum_add(const  mp_int *a, const  mp_int *b,
	        mp_int *c);
int bignum_sub(const  mp_int *a, const  mp_int *b,
	        mp_int *c);
int bignum_mul(const  mp_int *a, const  mp_int *b,
	        mp_int *c);
int bignum_mulmod(const  mp_int *a, const  mp_int *b,
		  const  mp_int *c,  mp_int *d);
int bignum_exptmod(const  mp_int *a, const  mp_int *b,
		   const  mp_int *c,  mp_int *d);

#endif /** BIGNUM_H */

