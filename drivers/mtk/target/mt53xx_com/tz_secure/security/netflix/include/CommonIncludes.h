/*
 * (c) 1997-2014 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef COMMONINCLUDES_H_
#define COMMONINCLUDES_H_



#include <tomcrypt.h>

#define TEE_LIBTOM_TEST_ENABLE	0
#define TEE_DEBUG_ENABLE		0

typedef  unsigned char 	uint8_t;
typedef  unsigned short	uint16_t;
typedef  unsigned int  	uint32_t;
typedef unsigned long 	uint64_t;
#if 1
#define OPENSSL_LIBTOM_EXCHANGE(func_name)	func_name##_libtom
#else
#define OPENSSL_LIBTOM_EXCHANGE(func_name)	func_name
#endif

#if 0
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#endif


#define x_memset								memset
#define x_memcpy								memcpy
#define x_memcmp								memcmp
#define x_alloc_s_dram_mem			malloc
#define x_free_s_dram_mem				free
#define x_strlen								strlen
#define x_realloc_s_dram_mem		realloc


#endif
