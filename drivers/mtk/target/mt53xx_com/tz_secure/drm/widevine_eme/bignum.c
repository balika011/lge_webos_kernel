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

#include "bignum.h"

#define MP_OKAY       0   /* ok result */


/**
 * The current version is just a wrapper for LibTomMath library, so
 * struct mp_int is just typecast to mp_int.
 */

/***
 * bignum_init - Allocate memory for mp_int
 * Returns: Pointer to allocated mp_int or %NULL on failure
 */
mp_int * bignum_init(void)
{
 	mp_int m;
	void *t;
	mp_int *n = NULL;
	
	//DMSG("bignum_init: sizeof(mp_int)=%d \n",sizeof(mp_int));
	t = (void*)&m;

	if (ltc_mp.init(&t)!= CRYPT_OK) {
		DMSG("bignum_init: mp_init fail \n");
		return NULL;
	}

	n = (mp_int *)t;
	dprintf("bignum_init after:n->used=%d,n->alloc=%d,n->sign=%d \n", n->used,n->alloc,n->sign);
	dprintf("bignum_init after:n->dp=0x%x \n",n->dp);
	return n;
}


/***
 * bignum_deinit - Free mp_int
 * @n: Bignum from bignum_init()
 */
void bignum_deinit(mp_int *n)
{
	if(n) {
		ltc_mp.deinit(n);
	}
}


/***
 * bignum_get_unsigned_bin - Get length of mp_int as an unsigned binary buffer
 * @n: Bignum from bignum_init()
 * Returns: Length of n if written to a binary buffer
 */
size_t bignum_get_unsigned_bin_len( mp_int *n)
{
	return mp_unsigned_bin_size(( mp_int *) n);
}


/***
 * bignum_get_unsigned_bin - Set binary buffer to unsigned mp_int
 * @n: Bignum from bignum_init()
 * @buf: Buffer for the binary number
 * @len: Length of the buffer, can be %NULL if buffer is known to be long
 * enough. Set to used buffer length on success if not %NULL.
 * Returns: 0 on success, -1 on failure
 */
int bignum_get_unsigned_bin(const  mp_int *n, u8 *buf, size_t *len)
{
	size_t need = mp_unsigned_bin_size(( mp_int *) n);
	if (len && need > *len) {
		*len = need;
		return -1;
	}
	if (mp_to_unsigned_bin(( mp_int *) n, buf) != MP_OKAY) 
	{
		DMSG("BIGNUM: %s failed", "bignum_get_unsigned_bin\n");
		return -1;
	}
	if (len)
		*len = need;
	return 0;
}


/***
 * bignum_set_unsigned_bin - Set mp_int based on unsigned binary buffer
 * @n: Bignum from bignum_init(); to be set to the given value
 * @buf: Buffer with unsigned binary value
 * @len: Length of buf in octets
 * Returns: 0 on success, -1 on failure
 */
int bignum_set_unsigned_bin(mp_int *n, const u8 *buf, size_t len)
{
	//dprintf("bignum_set_unsigned_bin:n->used=%d,n->alloc=%d,n->sign=%d \n", n->used,n->alloc,n->sign);
	//dprintf("bignum_set_unsigned_bin:n->dp=0x%x \n",n->dp);

	if (ltc_mp.unsigned_read(n, (u8 *)buf, len) != CRYPT_OK) {
		DMSG("mp_read_unsigned_bin: failed \n");
		return -1;
	}
	DMSG("mp_read_unsigned_bin: success \n");
	return 0;
}


/***
 * bignum_cmp - Signed comparison
 * @a: Bignum from bignum_init()
 * @b: Bignum from bignum_init()
 * Returns: 0 on success, -1 on failure
 */
int bignum_cmp(const   mp_int *a, const   mp_int *b)
{
	return mp_cmp(( mp_int *) a, ( mp_int *) b);
}


/***
 * bignum_cmd_d - Compare mp_int to standard integer
 * @a: Bignum from bignum_init()
 * @b: Small integer
 * Returns: 0 on success, -1 on failure
 */
int bignum_cmp_d(const mp_int *a, unsigned long b)
{
	//return mp_cmp_d(a, b);
	return ltc_mp.compare_d(a, b);
}


/***
 * bignum_add - c = a + b
 * @a: Bignum from bignum_init()
 * @b: Bignum from bignum_init()
 * @c: Bignum from bignum_init(); used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 */
int bignum_add(const  mp_int *a, const  mp_int *b,
	        mp_int *c)
{
	if (mp_add(( mp_int *) a, ( mp_int *) b, ( mp_int *) c) != MP_OKAY) {
		DMSG( "BIGNUM: %s failed", "bignum_add");
		return -1;
	}
	return 0;
}


/***
 * bignum_sub - c = a - b
 * @a: Bignum from bignum_init()
 * @b: Bignum from bignum_init()
 * @c: Bignum from bignum_init(); used to store the result of a - b
 * Returns: 0 on success, -1 on failure
 */
int bignum_sub(const  mp_int *a, const  mp_int *b,
	        mp_int *c)
{
	if (mp_sub((mp_int *) a, (mp_int *) b, (mp_int *) c) != MP_OKAY) {
		DMSG( "BIGNUM: %s failed", "bignum_sub");
		return -1;
	}
	return 0;
}


/***
 * bignum_mul - c = a * b
 * @a: Bignum from bignum_init()
 * @b: Bignum from bignum_init()
 * @c: Bignum from bignum_init(); used to store the result of a * b
 * Returns: 0 on success, -1 on failure
 */
int bignum_mul(const  mp_int *a, const  mp_int *b,
	        mp_int *c)
{
	if (mp_mul((mp_int *) a, (mp_int *) b, (mp_int *) c) != MP_OKAY) {
		DMSG( "BIGNUM: %s failed", __FUNCTION__);
		return -1;
	}
	return 0;
}


/***
 * bignum_mulmod - d = a * b (mod c)
 * @a: Bignum from bignum_init()
 * @b: Bignum from bignum_init()
 * @c: Bignum from bignum_init(); modulus
 * @d: Bignum from bignum_init(); used to store the result of a * b (mod c)
 * Returns: 0 on success, -1 on failure
 */
int bignum_mulmod(const  mp_int *a, const  mp_int *b,
		  const  mp_int *c,  mp_int *d)
{
	if (mp_mulmod((mp_int *) a, (mp_int *) b, (mp_int *) c, (mp_int *) d)
	    != MP_OKAY) {
		DMSG( "BIGNUM: %s failed", __FUNCTION__);
		return -1;
	}
	return 0;
}


/***
 * bignum_exptmod - Modular exponentiation: d = a^b (mod c)
 * @a: Bignum from bignum_init(); base
 * @b: Bignum from bignum_init(); exponent
 * @c: Bignum from bignum_init(); modulus
 * @d: Bignum from bignum_init(); used to store the result of a^b (mod c)
 * Returns: 0 on success, -1 on failure
 */
int bignum_exptmod(const  mp_int *a, const  mp_int *b,
		   const  mp_int *c,  mp_int *d)
{
	if (mp_exptmod((mp_int *) a, (mp_int *) b, (mp_int *) c, (mp_int *) d)
	    != MP_OKAY) {
		DMSG( "BIGNUM: %s failed", __FUNCTION__);
		return -1;
	}
	return 0;
}

