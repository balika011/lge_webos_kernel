/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*

       File bignum.c
       Version 20 February 2005

       This file has extern definitions of functions
       which are ordinarily inlined.  In particular, it exists to
       support compilers which don't support inlining.
       It also has simple routines defined nowhere else.
*/
#define BIGNUM_C 1
#include "tzcrypto.h"
#include "bignum.h"

CRYPTO_INT DRMCRT_abs( CRYPTO_INT number )
{
    return (number>=0 ? number : -number);
}


/*
        CRYPTO_DWORD mp_significant_digit_count(a, lng) --  Count the number
                                            of significant bits in a.
                                            This is one more than the
                                            truncated base 2 logarithm of a.

        CRYPTO_DWORD significant_bit_count(d) -- Compute the number of
                                          significant bits in d (d != 0).
                                          This is one more than the
                                          truncated base 2 logarithm of d.

        CRYPTO_BOOL two_adic_inverse(d, &dinv, &ctx) -- Returns dinv so that
                                           d*dinv == 1 (mod RADIX).
                                           d must be odd.

*/
/****************************************************************************/

CRYPTO_BOOL two_adic_inverse( const digit_t d, digit_t *pdinv )
/*
      Find *pdinv so that (*pdinv)*d == 1 (mod RADIX)
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (IS_EVEN(d)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "two_adic_inverse");
    } else {
        digit_t dinv = (3*d)^2;     // 5-bit  2-adic inverse approximation
        digit_t err = 1 - d*dinv;
        CRYPTO_DWORD nbits;   // Bits of accuracy so far

        DRMASSERT((err & 31) == 0);
        for (nbits = 5; nbits < RADIX_BITS/2; nbits *= 2) {
            dinv += dinv*err;
            err = err*err;
            DRMASSERT(err == (digit_t)(1 - d*dinv));
        }
        *pdinv = dinv*err + dinv;
    }
    return OK;
} // two_adic_inverse

/****************************************************************************/


/*
        Here are some simple routines which fit nowhere else.

      carry  = accumulate(a, mult, b, lng)   -- b := b + mult*a, where a, b
                                                           have length lng


borrow = decumulate(a, mult, b, lng)   -- b := b - mult*a, where a, b
                                                           have length lng
carry  = multiply_immediate(a, mult, b, lng) -- b := mult*a, where a, b
                                                            have length lng

        CRYPTO_BOOL mp_shift(a, amt, b, lng)    -- Compute lower lng words
                                            of b = a*2^amt, where
                                            amt can be arbitrarily large.
                                            Overflow/underflow is lost.
                                            Does left shift if amt >= 0,
                                            right shift if amt < 0.

        digit_t mp_shift_lost(a, amt, b, lng, plost)
                                         -- Compute lower lng words
                                            of b = a*2^amt, where
                                            |amt| <= RADIX_BITS.
                                            *plost will receive bits
                                            lost due to overflow/underflow.

        CRYPTO_BOOL multiply(a, lnga, b, lngb, c) - Compute c = a * b
                                             (classical algorithm).

        CRYPTO_BOOL set_immediate(a, ivalue, lngs) -- Set  a = ivalue (a scalar)

         multiply_low(a, b, c, lng)    -- c := a * b (mod RADIX^lng)
         (see file multlow.c)
*/
/***************************************************************************/
digit_t accumulate(
            const digit_t   a[],
            const digit_t   mult,
            digit_t   b[],
            const CRYPTO_DWORD lng)
/*
        Compute b = b + mult*a, where a and b have length lng.
        Function value is carry out of leftmost digit.
*/
{
    digit_t carry = 0;
    CRYPTO_DWORD i;

    for (i = 0; i != lng; i++) {
        const CRYPTO_UINT64 dtemp = MULTIPLY_ADD2(mult, a[i], b[i], carry);
        b[i] = CRYPTO_UI64Low32(dtemp);
        carry = CRYPTO_UI64High32(dtemp);
    }
    return carry;
}  /* accumulate */

/****************************************************************************/
digit_t decumulate(
                const digit_t   a[],
                const digit_t   mult,
                digit_t   b[],
                const CRYPTO_DWORD lng )
/*
        Compute b = b - mult*a, where a and b have length lng.
        Function value is borrow out of leftmost digit.
*/
{
    digit_t borrow = 0;
    CRYPTO_DWORD i;

    for (i = 0; i != lng; i++) {
        const CRYPTO_UINT64 dtemp = CRYPTO_UI64Sub(CRYPTO_UI64Sub(CRYPTO_UI64(b[i]),
                                                CRYPTO_UI64(borrow)),
                                     DPRODUU(mult, a[i]));
        b[i] = CRYPTO_UI64Low32(dtemp);
        borrow = 0 - CRYPTO_UI64High32(dtemp);
    }
    return borrow;
}  /* decumulate */
/***************************************************************************/
CRYPTO_BOOL mp_shift(
                const digit_t   a[],
                const CRYPTO_INT   ishift,
                digit_t   b[],
                const CRYPTO_DWORD lng)
/*
//              Compute  b = a << ishift     (if ishift >= 0)
//                   or  b = a >> (-ishift)  (if ishift < 0).
//
//              Both input and output are length lng.
//              Unlike mp_shift, the shift count may
//              exceed RADIX_BITS bits (either direction).
//              It may even exceed lng*RADIX_BITS.
//              Bits shifted past either end are lost.
*/
{
    const CRYPTO_INT itranslate = (ishift >= 0 ?   (CRYPTO_INT) (CRYPTO_DWORD)ishift/RADIX_BITS
                                   : - (CRYPTO_INT)((CRYPTO_DWORD)(-ishift)/RADIX_BITS));
                   /* Round quotient ishift/RADIX_BITS towards zero */
    CRYPTO_DWORD i;
    CRYPTO_BOOL OK = TRUE;
    digit_t lost = 0;

    OK = OK && mp_shift_lost(a, ishift - RADIX_BITS*itranslate, b,
                             lng, &lost);

    if (!OK) {
    } else if (itranslate < 0) {               /* Right shift, multiple words */
        const CRYPTO_DWORD dtranslate = (CRYPTO_DWORD)(-itranslate);
        for (i = 0; i < lng; i++) {
            CRYPTO_DWORD j = i + dtranslate;
            b[i] = 0;
            
            // Skip the following assignment if integer overflow occurrs.
            if ( j >= i && j >= dtranslate && j < lng && j >= 0 )
            {
                b[i] = b[j];
            }
        }
    } else if (itranslate > 0) {        /* Left shift, multiple words */
        const CRYPTO_DWORD dtranslate = (CRYPTO_DWORD)itranslate;
        for (i = lng; (i--) > 0; /*null*/) {
            b[i] = (i >= dtranslate ? b[i - dtranslate] : 0);
        }
    }
    return OK;
} /* mp_shift */
/****************************************************************************/
CRYPTO_BOOL mp_shift_lost(
                const digit_t    a[],
                const CRYPTO_INT    shift_amt,
                digit_t    b[],
                const CRYPTO_DWORD  lng,
                      digit_t   *plost)
/*
//      Compute b = a*2^shift_amt.
//      Require -RADIX_BITS <= shift_amt <= RADIX_BITS.
//      Function value reflects bits shifted off the
//      right or off the left.

//      The arrays a and b should be identical, or not overlap.
*/
{
    CRYPTO_DWORD i;
    CRYPTO_BOOL OK = TRUE;
    digit_t bits_lost = 0;

    if (DRMCRT_abs(shift_amt) > RADIX_BITS) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "mp_shift_lost -- bad shift count");
    } else if (lng == 0) {
    } else if (shift_amt > 0) {           /* left shift */
        if (shift_amt == RADIX_BITS) {
            bits_lost = a[lng-1];
            for (i = lng-1; i != 0; i--) b[i] = a[i-1];
            b[0] = 0;
        } else {
            for (i = 0; i != lng; i++) {
                const digit_t bnew = (a[i] << shift_amt) | bits_lost;

                bits_lost = a[i] >> (RADIX_BITS - shift_amt);
                b[i] = bnew;
            }
        }
    } else if (shift_amt < 0) {
        if (shift_amt == -RADIX_BITS) {
            bits_lost = a[0];
            for (i = 1; i != lng; i++) b[i-1] = a[i];
            b[lng-1] = 0;
        } else {

            for (i = lng; (i--) > 0; /*null*/) {
                const digit_t bnew = (a[i] >> (-shift_amt) ) | bits_lost;
                bits_lost = a[i] << (RADIX_BITS + shift_amt);
                b[i] = bnew;
            }
            bits_lost >>= (RADIX_BITS + shift_amt); /* Move to bottom of word */
        }
    } else {
        memcpy( b,a,( lng)*sizeof( digit_t ));
    }
    if (OK) *plost = bits_lost;
    return OK;
} /* mp_shift_lost */

/****************************************************************************/
digit_t multiply_immediate(
                const digit_t     a[],
                const digit_t     mult,
                digit_t             b[],
                const CRYPTO_DWORD   lng )
/*
        Compute b = mult*a, where a and b have length lng.
        Function value is carry out of leftmost digit.
*/
{
    digit_t carry = 0;
    CRYPTO_DWORD i;
    
    for (i = 0; i < lng; i++) {
        const CRYPTO_UINT64 dtemp = MULTIPLY_ADD1(mult, a[i], carry);
        b[i] = CRYPTO_UI64Low32(dtemp);
        carry = CRYPTO_UI64High32(dtemp);
    }
    return carry;
} /* multiply_immediate */
/****************************************************************************/
CRYPTO_BOOL set_immediate(
    digit_t             a[],          // OUT
    const digit_t       ivalue,       // IN
    const CRYPTO_DWORD     lnga,         // IN
    struct bigctx_t    *f_pBigCtx )
{
    CRYPTO_BOOL OK = TRUE;
    if (lnga == 0) {
        if (ivalue != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_OVERFLOW,
                            "set_immediate");
        }
    } else {
        a[0] = ivalue;
        memset(a + 1, 0, ( lnga - 1)*sizeof( digit_t ));
    }
    return OK;
} // end set_immediate
/****************************************************************************/


/*
** This is pretty much a no-op function that is needed for prebuilt
** stublibs for testing.
*/
CRYPTO_BOOL mp_initialization( struct bigctx_t *f_pBigCtx )
{
    return TRUE;
}

#if !CRYPTO_INLINING_SUPPORTED
/****************************************************************************/
CRYPTO_BOOL multiply(
    const digit_t   a[],
    const CRYPTO_DWORD lnga,
    const digit_t   b[],
    const CRYPTO_DWORD lngb,
          digit_t   c[] )
/*
//        Multiply a (length lnga) times b (length lngb),
//        getting a product c (length lnga + lngb).

//        The output should not overlap the inputs.
*/
{
    CRYPTO_BOOL OK = TRUE;
    const digit_t *p1, *p2;
    CRYPTO_DWORD i, lng1, lng2;

    if (lnga > lngb) {           /* Put longer operand in p1 */
        p1 = a; p2 = b; lng1 = lnga; lng2 = lngb;
    } else {
        p2 = a; p1 = b; lng2 = lnga; lng1 = lngb;
    }

    if (!OK) {
    } else if (a == c || b == c) {
        OK = FALSE;        
    } else if (lng2 == 0) {       /* If an operand has length zero */
        ZEROMEM(c,( lng1)*SIZEOF( digit_t ));
    } else {
        c[lng1] = multiply_immediate(p1, p2[0], c, lng1);
        for (i = 1; i != lng2; i++) {
            c[i + lng1] = accumulate(p1, p2[i], &c[i], lng1);
        }
    }
    return OK;
} /* multiply */
#endif


