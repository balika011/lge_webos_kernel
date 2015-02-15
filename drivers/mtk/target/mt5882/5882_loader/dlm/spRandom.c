/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/* $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/5882_loader/dlm/spRandom.c#1 $ */

/******************** SHORT COPYRIGHT NOTICE**************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 2.1 originally written by David Ireland,
copyright (c) 2001-6 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with this
library or can be obtained from <www.di-mgt.com.au/bigdigits.html>.
This notice must always be retained in any copy.
******************* END OF COPYRIGHT NOTICE***************************/
/*
	Last updated:
	$Date: 2015/02/15 $
	$Revision: #1 $
	$Author: p4admin $
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bigdigits.h"
#include "spRandom.h"

static uint32_t btrrand(void);

/*********************/
/* EXPORTED FUNCTION */
/*********************/

DIGIT_T spBetterRand(void)
{	/*	Returns a "better" pseudo-random digit. */
	return (DIGIT_T)btrrand();
}

/******************************************************************************
Generates a pseudo-random DIGIT value by using
the ANSI X9.31 algorithm but with the `Tiny Encryption Algorithm' 
replacing the `Triple DES' algorithm (much less code to copy, and faster).

CAUTION: not thread-safe as it uses a static variable.

Not quite cryptographically secure but much better than 
just using the plain-old-rand() function. 
Output should always pass the FIPS 140-2 statistical test.
Users can make their own call as to the security of this approach.
It's certainly sufficient for generating random digits for tests.

[v2.1] Changed to `new variant' TEAX of encipher algorithm
(this is unlikely to make any change in the security of this RNG).
******************************************************************************/

/******************************************************************************
ANSI X9.31 ALGORITHM:
Given

    * D, a 64-bit representation of the current date-time
    * S, a secret 64-bit seed that will be updated by this process
    * K, a secret [Triple DES] key

Step 1. Compute the 64-bit block X = G(S, K, D) as follows:

   1. I = E(D, K)
   2. X = E(I XOR S, K)
   3. S' = E(X XOR I, K)

where E(p, K) is the [Triple DES] encryption of the 64-bit block p using key K.

Step 2. Return X and set S = S' for the next cycle. 
******************************************************************************/

#define KEY_WORDS 4
static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k);

/* CAUTION: We use a static structure to store our values in. */
static struct {
	int seeded;
	uint32_t seed[2];
	uint32_t key[KEY_WORDS];
} m_generator;

/* Cross-platform ways to get a 64-bit time value */
#if defined(unix) || defined(__unix__)
static void get_time64(uint32_t t[2])
{
	#include <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv, NULL);
	memcpy(t, &tv, 2*sizeof(uint32_t));
}
#elif defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void get_time64(uint32_t t[2])
{
	FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	t[0] = ft.dwHighDateTime;
	t[1] = ft.dwLowDateTime;
}
#else
static void get_time64(uint32_t t[2])
{
	/* Best we can do with strict ANSI */
	t[0] = time(NULL);
	t[1] = 0;
}
#endif

static void btrseed(uint32_t seed)
{
	int i;
	uint32_t t[2];

	/* Use plain old rand function to generate our internal seed and key */
	srand(seed);
	for (i = 0; i < 2; i++)
		m_generator.seed[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);
	for (i = 0; i < KEY_WORDS; i++)
		m_generator.key[i] = (rand() & 0xFFFF) << 16 | (rand() & 0xFFFF);

	/* Set flag so we only do it once */
	m_generator.seeded = 1;

	/* Set key = key XOR time */
	get_time64(t);
	m_generator.key[0] ^= t[0];
	m_generator.key[1] ^= t[1];

}

static uint32_t btrrand(void)
/* Returns one 32-bit word */
{
	uint32_t inter[2], x[2];

	/* Set seed if not already seeded */
	if (!m_generator.seeded)
	{
		btrseed((uint32_t)time(NULL));
	}

	/* I = E(D, K) */
	get_time64(inter);
	encipher(inter, inter, m_generator.key);

	/* X = E(I XOR S, K) */
	x[0] = inter[0] ^ m_generator.seed[0];
	x[1] = inter[1] ^ m_generator.seed[1];
	encipher(x, x, m_generator.key);

	/* S' = E(X XOR I, K) */
	inter[0] ^= x[0];
	inter[1] ^= x[1];
	encipher(inter, m_generator.seed, m_generator.key);

	return x[0];
}

/************************************************

The Tiny Encryption Algorithm (TEA) by 
David Wheeler and Roger Needham of the
Cambridge Computer Laboratory.

Placed in the Public Domain by
David Wheeler and Roger Needham.

**** ANSI C VERSION (New Variant) ****

Notes:

TEA is a Feistel cipher with XOR and
and addition as the non-linear mixing
functions. 

Takes 64 bits of data in v[0] and v[1].
Returns 64 bits of data in w[0] and w[1].
Takes 128 bits of key in k[0] - k[3].

TEA can be operated in any of the modes
of DES. Cipher Block Chaining is, for example,
simple to implement.

n is the number of iterations. 32 is ample,
16 is sufficient, as few as eight may be OK.
The algorithm achieves good dispersion after
six iterations. The iteration count can be
made variable if required.

Note this is optimised for 32-bit CPUs with
fast shift capabilities. It can very easily
be ported to assembly language on most CPUs.

delta is chosen to be the real part of (the
golden ratio Sqrt(5/4) - 1/2 ~ 0.618034
multiplied by 2^32).

This version has been amended to foil two
weaknesses identified by David A. Wagner
(daw@cs.berkeley.edu): 1) effective key
length of old-variant TEA was 126 not 128
bits 2) a related key attack was possible
although impractical.

************************************************/

static void encipher(uint32_t *const v,uint32_t *const w, const uint32_t *const k)
{
	register uint32_t y=v[0],z=v[1],sum=0,delta=0x9E3779B9,n=32;

	while(n-->0)
	{
		y+= (((z<<4) ^ (z>>5)) + z) ^ (sum + k[sum & 3]);
		sum += delta;
		z+= (((y<<4) ^ (y>>5)) + y) ^ (sum + k[sum>>11 & 3]);
	}

	w[0]=y; w[1]=z;
}

