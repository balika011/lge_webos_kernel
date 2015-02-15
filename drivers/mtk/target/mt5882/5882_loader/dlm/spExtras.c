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
/* spExtras.c */

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


#include <assert.h>
#include "bigdigits.h"
#include "spExtras.h"

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m)
{	/*	Computes a = (x * y) mod m */
	
	/* Calc p[2] = x * y */
	DIGIT_T p[2];
	spMultiply(p, x, y);

	/* Then modulo */
	*a = mpShortMod(p, m, 2);
	return 0;
}

DIGIT_T spGcd(DIGIT_T x, DIGIT_T y)
{	/*	Returns gcd(x, y) */

	/* Ref: Schneier 2nd ed, p245 */
	
	DIGIT_T g;

	if (x + y == 0)
		return 0;	/* Error */

	g = y;
	while (x > 0)
	{
		g = x;
		x = y % x;
		y = g;
	}
	return g;
}

/* spIsPrime */

static DIGIT_T SMALL_PRIMES[] = { 2, 3, 5, 7, 11, 13, 17, 19 };
#define N_SMALL_PRIMES sizeof(SMALL_PRIMES)/sizeof(DIGIT_T)

int spIsPrime(DIGIT_T w, size_t t)
{	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) 
	*/
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379
		and ANSI 9.42-2003 Annex B.1.1.
	*/

	unsigned int i, j;
	DIGIT_T m, a, b, z;
	int failed;

	/*	First check for small primes */
	for (i = 0; i < N_SMALL_PRIMES; i++)
	{
		if (w % SMALL_PRIMES[i] == 0)
			return 0;	/* Failed */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */

	m = w - 1;
	for (a = 0; ISEVEN(m); a++)
		m >>= 1;	/* Divide by 2 until m is odd */

	/*
	assert((1 << a) * m + 1 == w);
	*/

	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate a random integer 1 < b < w */
		/* [v2.1] changed to 1 < b < w-1 */
		b = spSimpleRand(2, w - 2);

		/*
		assert(1 < b && b < w-1);
		*/

		/* Step 4. Set j = 0 and z = b^m mod w */
		j = 0;
		spModExp(&z, b, m, w);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			if ((j == 0 && z == 1) || (z == w - 1))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (j > 0 && z == 1)
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			j++;
			if (j < a)
				spModMult(&z, z, z, w);
			/* Loop: if j < a go to Step 5 */
		} while (j < a);

		if (failed)
		{	/* Step 8. Not a prime - stop */
			return 0;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* If got here, probably prime => success */
	return 1;
}

/* spModExp */

/* Two alternative functions for spModExp */

int spModExpK(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{	/*	Computes exp = x^n mod d */
	/*	Ref: Knuth Vol 2 Ch 4.6.3 p 462 Algorithm A
	*/
	DIGIT_T y = 1;		/* Step A1. Initialise */

	while (n > 0)
	{							/* Step A2. Halve N */
		if (n & 0x1)			/* If odd */
			spModMult(&y, y, x, d);	/*   Step A3. Multiply Y by Z */	
		
		n >>= 1;					/* Halve N */
		if (n > 0)				/* Step A4. N = 0? Y is answer */
			spModMult(&x, x, x, d);	/* Step A5. Square Z */
	}

	*exp = y;
	return 0;
}

int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T e, DIGIT_T m)
{	/*	Computes exp = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	DIGIT_T y;	/* Temp variable */

	/* Find most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e & mask)
			break;
	}

	y = x;
	/* For j = k-2 downto 0 step -1 */
	for (mask >>= 1; mask > 0; mask >>= 1)
	{
		spModMult(&y, y, y, m);		/* Square */
		if (e & mask)
			spModMult(&y, y, x, m);	/* Multiply */
	}

	*exp = y;
	return 0;
}

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342 
		ignoring u2, v2, t2
		and avoiding negative numbers
	*/
	DIGIT_T u1, u3, v1, v3, t1, t3, q, w;
	int bIterations = 1;
	int result;
	
	/* Step X1. Initialise */
	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	while (v3 != 0)	/* Step X2. */
	{	/* Step X3. */
		q = u3 / v3;	/* Divide and */
		t3 = u3 % v3;
		w = q * v1;		/* "Subtract" */
		t1 = u1 + w;
		/* Swap */
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		bIterations = -bIterations;
	}

	if (bIterations < 0)
		*inv = v - u1;
	else
		*inv = u1;

	/* Make sure u3 = gcd(u,v) == 1 */
	if (u3 != 1)
	{
		result = 1;
		*inv = 0;
	}
	else
		result = 0;

	return result;
}

