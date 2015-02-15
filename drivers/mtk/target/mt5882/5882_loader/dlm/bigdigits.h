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
/* $Id: //DTV/RELEASE_BR/r_DTV_X_IDTV1401_002306_12_001_37_001/kernel/drivers/mtk/target/mt5882/5882_loader/dlm/bigdigits.h#1 $ */

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

/* Interface to BigDigits "mp" functions */

#ifndef BIGDIGITS_H_
#define BIGDIGITS_H_ 1

#include <stddef.h>

/**** USER CONFIGURABLE SECTION ****/

/* Define type and size of DIGIT */

/* [v2.1] Changed to use C99 exact-width types so it will compile with 64-bit compilers.

The following PP instructions assume that all Linux systems have a C99-conforming 
<stdint.h>; that other Unix systems have the uint32_t definitions in <sys/types.h>;
and that MS et al don't have them at all. Adjust if necessary to suit your system. 
You can override by defining HAVE_C99INCLUDES or HAVE_SYS_TYPES.
CAUTION: change this, change bigd.h to match.
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

typedef uint32_t DIGIT_T;
typedef uint16_t HALF_DIGIT_T;

/* Sizes to match */
#define MAX_DIGIT 0xffffffffUL
#define MAX_HALF_DIGIT 0xffffUL	/* NB 'L' */
#define BITS_PER_DIGIT 32
#define HIBITMASK 0x80000000UL

/* Macros for format specifiers 
-- change to "u", "x" and "X" if necessary */
#ifdef HAVE_C99INCLUDES
	#include <inttypes.h>
#else 
	#define PRIu32 "lu" 
	#define PRIx32 "lx" 
	#define PRIX32 "lX" 
#endif
/* We define our own */
#define PRIuBIGD PRIu32
#define PRIxBIGD PRIx32
#define PRIXBIGD PRIX32

/**** END OF USER CONFIGURABLE SECTION ****/

#define BITS_PER_HALF_DIGIT (BITS_PER_DIGIT / 2)
#define BYTES_PER_DIGIT (BITS_PER_DIGIT / 8)

/* Removed in version 2 ---->
  [Max number of digits expected in a mp array]
//[#define MAX_DIG_LEN 51]
	This [was] required for temp storage only in:
	mpModulo, mpShortMod, mpModMult, mpGcd,
	mpModInv, mpIsPrime
   Changed to use mpAlloc.
<--- */

/* Useful macros */
#define LOHALF(x) ((DIGIT_T)((x) & MAX_HALF_DIGIT))
#define HIHALF(x) ((DIGIT_T)((x) >> BITS_PER_HALF_DIGIT & MAX_HALF_DIGIT))
#define TOHIGH(x) ((DIGIT_T)((x) << BITS_PER_HALF_DIGIT))

#define ISODD(x) ((x) & 0x1)
#define ISEVEN(x) (!ISODD(x))

#define mpISODD(x, n) (x[0] & 0x1)
#define mpISEVEN(x, n) (!(x[0] & 0x1))

#define mpNEXTBITMASK(mask, n) do{if(mask==1){mask=HIBITMASK;n--;}else{mask>>=1;}}while(0)

#ifdef __cplusplus
extern "C" {
#endif

char *copyright_notice(void);
	/* Forces linker to include copyright notice in executable */

/*	
 * Multiple precision calculations	
 * Using known, equal ndigits
 * except where noted
*/

/*************************/
/* ARITHMETIC OPERATIONS */
/*************************/

DIGIT_T mpAdd(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits);
	/* Computes w = u + v, returns carry */

DIGIT_T mpSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits);
	/* Computes w = u - v, returns borrow */

int mpMultiply(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits);
	/* Computes product w = u * v 
	   u, v = ndigits long; w = 2 * ndigits long */

int mpDivide(DIGIT_T q[], DIGIT_T r[], const DIGIT_T u[], 
	size_t udigits, DIGIT_T v[], size_t vdigits);
	/* Computes quotient q = u / v and remainder r = u mod v 
	   q, r, u = udigits long; v = vdigits long
	   Warning: Trashes q and r first */

int mpModulo(DIGIT_T r[], const DIGIT_T u[], size_t udigits, DIGIT_T v[], size_t vdigits);
	/* Computes r = u mod v 
	   u = udigits long; r, v = vdigits long */

int mpSquare(DIGIT_T w[], const DIGIT_T x[], size_t ndigits);
	/* Computes square w = x^2
	   x = ndigits long; w = 2 * ndigits long */

int mpSqrt(DIGIT_T s[], const DIGIT_T x[], size_t ndigits);
	/* Computes integer square root s = floor(sqrt(x)) */

/*************************/
/* COMPARISON OPERATIONS */
/*************************/

int mpEqual(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits);
	/* Returns true if a == b, else false */

int mpCompare(const DIGIT_T a[], const DIGIT_T b[], size_t ndigits);
	/* Returns sign of (a - b) */

int mpIsZero(const DIGIT_T a[], size_t ndigits);
	/* Returns true if a == 0, else false */

/****************************/
/* NUMBER THEORY OPERATIONS */
/****************************/

int mpModMult(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], const DIGIT_T m[], size_t ndigits);
	/* Computes a = (x * y) mod m */

int mpModExp(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T n[], const DIGIT_T d[], size_t ndigits);
	/* Computes y = x^n mod d */

int mpModInv(DIGIT_T inv[], const DIGIT_T u[], const DIGIT_T v[], size_t ndigits);
	/*	Computes inv = u^-1 mod v */

int mpGcd(DIGIT_T g[], const DIGIT_T x[], const DIGIT_T y[], size_t ndigits);
	/* Computes g = gcd(x, y) */

/**********************/
/* BITWISE OPERATIONS */
/**********************/

DIGIT_T mpShiftLeft(DIGIT_T a[], const DIGIT_T b[], size_t x, size_t ndigits);
	/* Computes a = b << x */

DIGIT_T mpShiftRight(DIGIT_T a[], const DIGIT_T b[], size_t x, size_t ndigits);
	/* Computes a = b >> x */

void mpXorBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits);
	/* Computes bitwise a = b XOR c */

void mpOrBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits);
	/* Computes bitwise a = b OR c */

void mpAndBits(DIGIT_T a[], const DIGIT_T b[], const DIGIT_T c[], size_t ndigits);
	/* Computes bitwise a = b AND c */

void mpModPowerOf2(DIGIT_T a[], size_t ndigits, size_t L);
	/* Computes a = a mod 2^L */

/*************************/
/* ASSIGNMENT OPERATIONS */
/*************************/

void mpSetZero(DIGIT_T a[], size_t ndigits);
	/* Sets a = 0 */

void mpSetDigit(DIGIT_T a[], DIGIT_T d, size_t ndigits);
	/* Sets a = d where d is a single digit */

void mpSetEqual(DIGIT_T a[], const DIGIT_T b[], size_t ndigits);
	/* Sets a = b */

/**********************/
/* OTHER MP UTILITIES */
/**********************/

size_t mpSizeof(const DIGIT_T a[], size_t ndigits);
	/* Returns size i.e. number of significant non-zero digits in a */

size_t mpBitLength(const DIGIT_T a[], size_t ndigits);
	/* Returns number of significant bits in a */

int mpIsPrime(const DIGIT_T w[], size_t ndigits, size_t t);
	/* Returns true if w > 2 is a probable prime 
	   t tests using FIPS-186-2/Rabin-Miller */

int mpRabinMiller(const DIGIT_T w[], size_t ndigits, size_t t);
	/* Just the FIPS-186-2/Rabin-Miller test 
	   without trial division by small primes */

/**********************************************/
/* FUNCTIONS THAT OPERATE WITH A SINGLE DIGIT */
/**********************************************/

DIGIT_T mpShortAdd(DIGIT_T w[], const DIGIT_T u[], DIGIT_T d, size_t ndigits);
	/* Computes w = u + d, returns carry */

DIGIT_T mpShortSub(DIGIT_T w[], const DIGIT_T u[], DIGIT_T d, size_t ndigits);
	/* Computes w = u - d, returns borrow */

DIGIT_T mpShortMult(DIGIT_T p[], const DIGIT_T x[], DIGIT_T d, size_t ndigits);
	/* Computes product p = x * d */

DIGIT_T mpShortDiv(DIGIT_T q[], const DIGIT_T u[], DIGIT_T d, size_t ndigits);
	/* Computes q = u / d, returns remainder */

DIGIT_T mpShortMod(const DIGIT_T a[], DIGIT_T d, size_t ndigits);
	/* Returns r = a mod d */

int mpShortCmp(const DIGIT_T a[], DIGIT_T d, size_t ndigits);
	/* Returns sign of (a - d) where d is a single digit */

/**************************************/
/* CORE SINGLE PRECISION CALCULATIONS */
/* (double where necessary)      */
/**************************************/

/* NOTE spMultiply and spDivide are used by almost all mp functions. 
   Using the Intel MASM alternatives gives significant speed improvements
   -- to use, define USE_SPASM as a preprocessor directive and compile with
   spASM.c instead of spBigdigits.c.
*/
#ifdef USE_SPASM
	#define spMultiply spasmMultiply
	#define spDivide spasmDivide
	#pragma message("Using MASM")
	#pragma comment(exestr, "MASM Version")
#endif

int spMultiply(DIGIT_T p[2], DIGIT_T x, DIGIT_T y);
	/* Computes p = x * y */

DIGIT_T spDivide(DIGIT_T *q, DIGIT_T *r, const DIGIT_T u[2], DIGIT_T v);
	/* Computes quotient q = u / v, remainder r = u mod v */

/****************************/
/* RANDOM NUMBER FUNCTIONS  */
/* CAUTION: NOT thread-safe */
/****************************/

DIGIT_T spSimpleRand(DIGIT_T lower, DIGIT_T upper);
	/* Returns a simple pseudo-random digit between lower and upper */

/* [Version 2.1: spBetterRand moved to spRandom.h] */

/*******************/
/* PRINT UTILITIES */
/*******************/

void mpPrint(const DIGIT_T *p, size_t len);
	/* Print all digits incl leading zero digits */
void mpPrintNL(const DIGIT_T *p, size_t len);
	/* Print all digits with newlines */
void mpPrintTrim(const DIGIT_T *p, size_t len);
	/* Print but trim leading zero digits */
void mpPrintTrimNL(const DIGIT_T *p, size_t len);
	/* Print, trim leading zeroes, add newlines */

/************************/
/* CONVERSION UTILITIES */
/************************/

size_t mpConvFromOctets(DIGIT_T a[], size_t ndigits, const unsigned char *c, size_t nbytes);
	/* Converts nbytes octets into big digit a of max size ndigits
	   Returns actual number of digits set */
size_t mpConvToOctets(const DIGIT_T a[], size_t ndigits, unsigned char *c, size_t nbytes);
	/* Convert big digit a into string of octets, in big-endian order,
	   padding to nbytes or truncating if necessary.
	   Return number of non-zero octets required. */
size_t mpConvFromDecimal(DIGIT_T a[], size_t ndigits, const char *s);
	/* Convert a string in decimal format to a big digit.
	   Return actual number of (possibly zero) digits set. */
size_t mpConvToDecimal(const DIGIT_T a[], size_t ndigits, char *s, size_t smax);
	/* Convert big digit a into a string in decimal format, 
	   where s has size smax including the terminating zero.
	   Return number of chars required excluding leading zeroes. */
size_t mpConvFromHex(DIGIT_T a[], size_t ndigits, const char *s);
	/* Convert a string in hexadecimal format to a big digit.
	   Return actual number of (possibly zero) digits set. */
size_t mpConvToHex(const DIGIT_T a[], size_t ndigits, char *s, size_t smax);
	/* Convert big digit a into a string in hexadecimal format, 
	   where s has size smax including the terminating zero.
	   Return number of chars required excluding leading zeroes. */

/****************/
/* VERSION INFO */
/****************/
/* [added in ver 2.0.2] */
int mpVersion(void);
	/* Returns version number = major*1000+minor*100+release*10+uses_asm(0|1) */

/*************************************************/
/* MEMORY ALLOCATION FUNCTIONS - USED INTERNALLY */
/*************************************************/

DIGIT_T *mpAlloc(size_t ndigits);
void mpFree(DIGIT_T **p);
void mpFail(char *msg);

#ifdef __cplusplus
}
#endif

#endif	/* BIGDIGITS_H_ */
