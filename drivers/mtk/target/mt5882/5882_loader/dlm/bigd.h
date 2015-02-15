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
/* $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/5882_loader/dlm/bigd.h#1 $ */

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

/* 
Interface to the BIGD library (BigDigits "bd" functions)
Multiple-precision arithmetic for non-negative numbers
using memory management and opaque pointers. 
*/

#ifndef BIGD_H_
#define BIGD_H_ 1

#include <stddef.h>

/**** USER CONFIGURABLE SECTION ****/

/* [v2.1] Changed to use exact width integer types.
   CAUTION: change this, change bigdigits.h to match.
*/
#ifndef HAVE_C99INCLUDES
	#if (__STDC_VERSION >= 199901L) || defined(linux) || defined(__linux__)
	#define HAVE_C99INCLUDES
	#endif
#endif
#ifndef HAVE_SYS_TYPES
	#if defined(unix) || defined(__unix__)
	#define HAVE_SYS_TYPES
	#endif
#endif
#ifdef HAVE_C99INCLUDES
	#include <stdint.h>
#elif defined(HAVE_SYS_TYPES)
	#include <sys/types.h>
#else 
	#define uint32_t unsigned long 
	#define uint16_t unsigned short 
#endif

/* DIGIT_T type is not exposed by this library so we expose 
   a synonym `bdigit_t' for a single digit */
typedef uint32_t bdigit_t;

/* Macros for format specifiers 
-- change these to "u", "x" and "X" if necessary */
#ifdef HAVE_C99INCLUDES
	#include <inttypes.h>
#else 
	#define PRIu32 "lu" 
	#define PRIx32 "lx" 
	#define PRIX32 "lX" 
#endif
/* We define our own based on the C99 standard ones */
#define PRIuBIGD PRIu32
#define PRIxBIGD PRIx32
#define PRIXBIGD PRIX32

/**** END OF USER CONFIGURABLE SECTION ****/

#define OCTETS_PER_DIGIT (sizeof(bdigit_t))
/* Options for bdPrint */
#define BD_PRINT_NL   0x1	/* append a newline after printing */
#define BD_PRINT_TRIM 0x2	/* trim leading zero digits */

/*
This interface uses opaque pointers of type BIGD using
the conventions in "C Interfaces and Implementions" by
David R. Hanson, Addison-Wesley, 1996, pp21-4.
Thanks to Ian Tree for the C++ fudge.
*/
#define T BIGD
#ifdef __cplusplus
	typedef struct T2 *T;
#else
	typedef struct T *T;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* PROGRAMMER'S NOTES
Where the function computes a new BIGD value,
the result is returned in the first argument.
Some functions do not allow variables to overlap (noted by w#v).
Functions of type int generally return 0 to denote success 
but some return True/False (1/0) or borrow (+1) or error (-1).
Functions of type size_t (an unsigned int) return a length.

Memory is allocated if the function might need it. 
It is only released when freed with bdFree(), e.g.:

	BIGD b;
	b = bdNew();
	...
	bdFree(&b);
*/

/******************************/
/* CONSTRUCTOR AND DESTRUCTOR */
/******************************/

BIGD bdNew(void);
	/* Return handle to new BIGD object */

void bdFree(BIGD *bd);	
	/* Zeroise and free BIGD memory
	   - NB pass a pointer to the handle */

int bd_resize(BIGD b, size_t newsize);

/*************************/
/* ARITHMETIC OPERATIONS */
/*************************/

int bdAdd(BIGD w, BIGD u, BIGD v);
	/* Compute w = u + v, w#v */

int bdSubtract(BIGD w, BIGD u, BIGD v);
	/* Compute w = u - v, return borrow, w#v */

int bdMultiply(BIGD w, BIGD u, BIGD v);
	/* Compute w = u * v, w#u */

int bdDivide(BIGD q, BIGD r, BIGD u, BIGD v);
	/* Computes quotient q = u / v and remainder r = u mod v 
	   Trashes q and r first. q#(u,v), r#(u,v) */

int bdModulo(BIGD r, BIGD u, BIGD v);
	/* Computes r = u mod v, r#u */

int bdSquare(BIGD w, BIGD x);
	/* Computes w = x^2, w#x */

int bdIncrement(BIGD a);
	/* Sets a = a + 1, returns carry */

int bdDecrement(BIGD a);
	/* Sets a = a - 1, returns borrow */

/* 'Safe' versions with no restrictions on overlap */
int bdAddEx(BIGD w, BIGD u, BIGD v);
int bdSubtractEx(BIGD w, BIGD u, BIGD v);
int bdMultiplyEx(BIGD w, BIGD u, BIGD v);
int bdDivideEx(BIGD q, BIGD r, BIGD u, BIGD v);
int bdModuloEx(BIGD r, BIGD u, BIGD v);
int bdSquareEx(BIGD w, BIGD x);

/* [v2.1.0] new function */
int bdSqrt(BIGD s, BIGD x);
	/* Computes integer square root s = floor(sqrt(x)) */

/*************************/
/* COMPARISON OPERATIONS */
/*************************/

int bdIsEqual(BIGD a, BIGD b);
	/* Returns true if a == b, else false */

int bdCompare(BIGD a, BIGD b);
	/* Returns sign of (a-b) */

int bdIsZero(BIGD a);
	/* Returns true if a == 0, else false */

int bdIsEven(BIGD a);
int bdIsOdd(BIGD a);
	/* Return TRUE or FALSE */

/*************************/
/* ASSIGNMENT OPERATIONS */
/*************************/

int bdSetEqual(BIGD a, BIGD b);
	/* Sets a = b */

int bdSetZero(BIGD a);
	/* Sets a = 0 */

/****************************/
/* NUMBER THEORY OPERATIONS */
/****************************/

int bdModExp(BIGD y, BIGD x, BIGD e, BIGD m);
	/* Compute y = x^e mod m */

int bdModMult(BIGD a, BIGD x, BIGD y, BIGD m);
	/* Compute a = (x * y) mod m */

int bdModInv(BIGD x, BIGD a, BIGD m);
	/* Compute x = a^-1 mod m */

int bdGcd(BIGD g, BIGD x, BIGD y);
	/* Compute g = gcd(x, y) */

int bdIsPrime(BIGD b, size_t ntests);
	/* Returns true if passes ntests x Miller-Rabin tests with trial division */

int bdRabinMiller(BIGD b, size_t ntests);
	/* Returns true if passes ntests x Miller-Rabin tests without trial division, b > 2 */

/**********************************************/
/* FUNCTIONS THAT OPERATE WITH A SINGLE DIGIT */
/**********************************************/

int bdSetShort(BIGD b, bdigit_t d);
	/* Converts single digit d into a BIGD b */

int bdShortAdd(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u + d */

int bdShortSub(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u - d, return borrow */

int bdShortMult(BIGD w, BIGD u, bdigit_t d);
	/* Compute w = u * d */

int bdShortDiv(BIGD q, BIGD r, BIGD u, bdigit_t d);
	/* Computes quotient q = u / d and remainder r = u mod d */

bdigit_t bdShortMod(BIGD r, BIGD u, bdigit_t d);
	/* Computes r = u mod d. Returns r. */

int bdShortCmp(BIGD a, bdigit_t d);
	/* Returns sign of (a-d) */

void InFromBuffer(unsigned char* buffer, size_t n_size, BIGD p);
void OutToBuffer(unsigned char* buffer, size_t n_size, BIGD p);


/***********************/
/* BIT-WISE OPERATIONS */
/***********************/

size_t bdBitLength(BIGD bd);
	/* Returns number of significant bits in bd */

int bdSetBit(BIGD a, size_t n, int value);
	/* Set bit n (0..nbits-1) with value 1 or 0 */

int bdGetBit(BIGD a, size_t ibit);
	/* Returns value 1 or 0 of bit n (0..nbits-1) */

/* [v2.1.0] Removed restriction on shift size */
void bdShiftLeft(BIGD a, BIGD b, size_t n);
	/* Computes a = b << n */

void bdShiftRight(BIGD a, BIGD b, size_t n);
	/* Computes a = b >> n */

/* [v2.1.0] Added ModPowerOf2, Xor, Or and And functions */
void bdModPowerOf2(BIGD a, size_t L);
	/* Computes a = a mod 2^L */

void bdXorBits(BIGD a, BIGD b, BIGD c);
	/* Computes bitwise operation a = b XOR c */

void bdOrBits(BIGD a, BIGD b, BIGD c);
	/* Computes bitwise operation a = b OR c */

void bdAndBits(BIGD a, BIGD b, BIGD c);
	/* Computes bitwise operation a = b AND c */

/*******************/
/* MISC OPERATIONS */
/*******************/

size_t bdSizeof(BIGD b);
	/* Returns number of significant digits in b */

void bdPrint(BIGD bd, size_t flags);
	/* Print bd to stdout (see flags above) */

/***************/
/* CONVERSIONS */
/***************/
/* [v2.1] Tightened up specs here */

/* All bdConv functions return 0 on error */

/* `Octet' = an 8-bit byte */

size_t bdConvFromOctets(BIGD b, const unsigned char *octets, size_t nbytes);
	/* Converts nbytes octets into big digit b, resizing b if necessary.
	   Returns actual number of digits set, which may be larger than mpSizeof(b). */

size_t bdConvToOctets(BIGD b, unsigned char *octets, size_t nbytes);
	/* Convert big digit b into array of octets, in big-endian order,
	   padding on the left with zeroes to nbytes, or truncating as necessary.
       It returns the exact number of significant bytes <nsig> in the result.
	   If octets is NULL or nbytes == 0 then just return required <nsig>;
       if nbytes== nsig, octets <-    nnnnnn;
       if nbytes > nsig, octets <- 000nnnnnn;
       if nbytes < nsig, octets <-       nnn. */

/* For Hex and Decimal conversions:
   The parameter `smax' in the bdConvFrom functions
   is the size INCLUDING the terminating zero (as in sizeof(s))
   but the bdConvTo functions return the number of characters required
   EXCLUDING the terminating zero.

   The bdConvToHex and bdConvToDecimal functions return the 
   total length of the string they tried to create even if 
   the output string was truncated at a shorter length.
*/

size_t bdConvFromHex(BIGD b, const char *s);
	/* Convert string s of hex chars into big digit b. 
	   Returns number of significant digits actually set, which could be 0. */

size_t bdConvToHex(BIGD b, char *s, size_t smax);
	/* Convert big digit b into string s of hex characters
	   where s has size smax including the terminating zero.
	   Returns number of chars required for s excluding leading zeroes. 
	   If s is NULL or smax == 0 then just return required size. */

size_t bdConvFromDecimal(BIGD b, const char *s);
	/* Convert string s of decimal chars into big digit b. 
	   Returns number of significant digits actually set, which could be 0. */

size_t bdConvToDecimal(BIGD b, char *s, size_t smax);
	/* Convert big digit b into string s of decimal characters
	   where s has size smax including the terminating zero.
	   Returns number of chars required for s excluding leading zeroes. 
	   If s is NULL or smax == 0 then just return required size. */


/****************************/
/* RANDOM NUMBER OPERATIONS */
/****************************/

/* [Version 2.1: bdRandDigit and bdRandomBits moved to bigdRand.h] */

size_t bdSetRandTest(BIGD a, size_t ndigits);
	/* Fill a with [1,ndigits] pseudo-random digits. Returns # digits set.
	   Randomly clears a random number of bits in the most significant digit.
	   For testing only. Not thread safe. */

/* TYPEDEF for user-defined random byte generator function */
typedef int (* BD_RANDFUNC)(unsigned char *buf, size_t nbytes, const unsigned char *seed, size_t seedlen);

int bdRandomSeeded(BIGD a, size_t nbits, const unsigned char *seed, 
	size_t seedlen, BD_RANDFUNC RandFunc);
	/* Generate a random number nbits long using RandFunc */

int bdGeneratePrime(BIGD b, size_t nbits, size_t ntests, const unsigned char *seed, 
	size_t seedlen, BD_RANDFUNC RandFunc);
	/* Generate a prime number nbits long; carry out ntests x primality tests */


/****************/
/* VERSION INFO */
/****************/
/* [v2.0.2] added bdVersion */

int bdVersion(void);
	/* Returns version number = major*1000+minor*100+release*10+uses_asm(0|1) */


#undef T /* (for opaque BIGD pointer) */

#ifdef __cplusplus
}
#endif

#endif /* BIGD_H_ */
