/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigdefs.h"
#include "bigpriv.h"
//#include <drmpkcrypto.h>

/*
        File modular.c.  Version 28 November 2002

                create_modulus(a, lnga, reddir, *modulo)
                        Initialize modulo for the modulus a,
                        of length lnga.  reddir can be FROM_LEFT
                        or FROM_RIGHT to specify whether
                        products are reduced from the left or
                        from the right.

                from_modular(a, b, modulo) --
                        The number a (mod modulo) is converted to
                        standard form and stored in b.

                mod_mul(a, b, c, modulo, temps) --
                        Compute c = a * b (mod modulo).

                mod_shift(a, shiftamt, b, modulo) --
                        Compute b = a * 2^shiftamt (mod modulo).
                        The shift count shiftamt can have either
                        sign, but the modulus must be
                        odd if the shift count is negative.
                        Cost is O(shiftamt) (for fixed modulo),
                        so huge values of shiftamt should be avoided.

               to_modular(a, lnga, b, modulo)  --
                        Convert a (of length lnga) to
                        its modular representation b.

               uncreate_modulus(modulo)
                        Release storage allocated by create_modulus
*/


/*
  (December, 1995, note)

       Here is the best way I have found so far to reduce 2048 bits
to 1024 bits.  If the (odd) modulus is N, the method uses two big constants

        Ninv512 = 1/N (mod 2^512)

        N1536 = N * Ninv512 (so N1536 == 1 mod 2^512).

Let the original 2048-bit number be x0.

Step 1)  Reduce to 1536 bits.  Replace x0 by

         x1 = (x0 - N1536 * (lower 512 bits of x0))/2^512.

         (N.B. x1 may be negative).
         Cost: One 512 x 1024-bit multiplication.

Step 2)  Compute q == x1 * Ninv512 (mod 2^512)

         Cost: One 512 x 512-bit multiplication (low half only)

Step 3)  Compute x2 = (x1 - q * N)/2^512.  If negative, add N.

         Cost: One 512 x 1024-bit multiplication,
               where the low 512 bits of the product are known.

In 1) and 3), the 1024-bit operand depends only on N,
         so any additions/subtractions needed to precondition
         the operand for Karatsuba need be done only once.
         The same 512-bit multiplier (bottom of x0, or q)
         is being used for two 512 x 512-bit multiplications,
         and need be preconditioned only once.

In 3), we can do the lower 512 x 512 multiplication with
two 256 x 256 multiplications since the low 512 bits of the product
are known.  Specifically, if we want (a1:a0)*(b1:b0)
where a1, a0, b1, b0 are 256 bits each, then it suffices
to compute a1*b1 and (a1 - a0)*(b1 - b0) if we know the bottom
512 bits of (a1:a0)*(b1:b0).

        The total reduction cost is 6 + 3 + 5 = 14
256 x 256-bit products, whereas the present scheme uses
the equivalent of 16 such products (computed by the classical algorithm).
I estimate the time for a 256 x 256-bit product to be 650 cycles
(on the Pentium), so the total reduction time
(after adding overhead for the additions) will be around 10000 cycles.

        We can do the 512 x 512 low product required by 2)
using 5 full 128 x 128 multiplication and 3 low
128 x 128 multiplications for a slight improvement.
Specifically, we can get the lower half of
(a3:a2:a1:a0) * (b3:b2:b1:b0) if we know
a0*b0, a1*b1, a2*b2, (a1 - a0)*(b1 - b0), (a2 - a0)*(b2 - b0),
and the lower halves of (a2 - a1)*(b2 - b1), a0*b3, a3*b0.

        Or we may skip the first reduction from 2048 to
1536 bits, and compute one 1024-bit q such that
q == x0/N (mod 2^1024) (lower half of 1024 x 1024-bit product).
Then take (x0 - q*N)/2^1024 where the lower half of q*N is known.
The computation of q can be done with eight 256 x 256-bit multiplications,
and q*N with two 512 x 512-bit multiplications, again
for fourteen 256 x 256-bit multiplications.

        If we try to reduce from the left rather than from the right, then
we can do something like 1) (using N1536 = 2^1536 + (number below N)
where N1536 is a multiple of N).  Then we have a 1536-bit remainder,
which we want to further-reduce to 1024 bits.
Define N1280 (slightly above 2^1280) similarly; one 256 x 1024
multiplication will reduce the remainder to 1280 bits.
We can estimate the final quotient at any time, if we
know the reciprocal of the divisor, but this seems to take
more work than working from the right.  It is still desirable to optimize
this case, to avoid the need to convert to/from Montgomery form
when the original exponent is small (e.g., when cubing a number).

        The length 1024 is in the padding table (see kara.c),
as is 512, so nothing is wasted when we pad those operands during
the multiplication.  For an irregular length, such as 608 bits,
the second reduction must be at least 608/2 = 304 bits,
and the first such entry is for 384 bits.  The first reduction
must then be at least 608 - 384 = 224 bits, and we could then
use the entry for 256 bits.  This would require taking
384 x 608 and 256 x 608 products, hence two 384 x 384 products
and three 256 x 256 products.  That is not presently implemented.
Instead, we use 384 bits for both reductions.
When the first padding length is an odd number of words
(namely 1 or 3), the total reduction will exceed the first padding length.

        Question:  Can we do two 2048-bit to 1024-bit
reductions any faster than a single reduction?
If so, we might want to design the exponentiation routine
to allow multiple concurrent exponentiations with same
exponent and modulus.  I am looking for a trick similar to
that used to compute two modular inverses using one
modular inversion and three multiplications:
1/x = y*(1/xy) and 1/y = x*(1/xy).

*/
/******************************************************************************/
/******************************************************************************/
CRYPTO_BOOL create_modulus
        (const digit_t       a[],
         const CRYPTO_DWORD      lnga,
         const reddir_t      reddir,
         mp_modulus_t  *pmodulo,
         struct bigctx_t *f_pBigCtx,
         struct bigctx_t *pbigctxGlobal)
/*
        This routine computes modulus-dependent constants
        for the modulus -a-, and stores them in a
        mp_modulus_t structure.  See the struct declaration
        (in bignum.h) for a description of each field.

        Argument reddir should be FROM_RIGHT if one wants to use
        Montgomery-type reductions (from the right)
        rather than traditional reductions (from the left).
        The FROM_RIGHT option is available only if -a- is odd.
*/
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD lngred2 = (lnga + 1)/2;

    digit_t *dtemps_longterm
        = digit_allocate(3*lnga + lngred2,   (CRYPTO_BYTE *)"create_modulus_longterm");
    digit_t *dtemps_shortterm
        = digit_allocate(2*lnga + 2*lngred2 + 1,  (CRYPTO_BYTE *) "create_modulus_shortterm");

    if (dtemps_shortterm == NULL || dtemps_longterm == NULL) {
        OK =  FALSE;
    } else if (lnga == 0 || a[lnga-1] == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "create_modulus leading zero");
    } else {
        digit_t *mptmp1    = dtemps_shortterm;        // Length lnga + lngred2
        digit_t *quotient  = dtemps_shortterm + lnga + lngred2;
                                                      // Length lngred2 + 1
        digit_t *remainder = dtemps_shortterm + 1 + lnga + 2*lngred2;
                                                      // Length lnga
        digit_t mod0inv;
        CRYPTO_DWORD i;

        pmodulo->modulus           = dtemps_longterm;           // Length lnga
             // N.B. uncreate_modulus assumes modulus comes first
        pmodulo->one               = dtemps_longterm +   lnga;  // Length lnga
        pmodulo->multiplier_first  = dtemps_longterm + 2*lnga;  // Length lnga
        pmodulo->multiplier_second = dtemps_longterm + 3*lnga; // Length lngred2        

        pmodulo->length = lnga;
        pmodulo->lngred2 = lngred2;
        pmodulo->reddir = reddir;
        memcpy( pmodulo->modulus,a,(  lnga)*sizeof( digit_t ));

        OK = OK && divide_precondition_1(a, lnga, &pmodulo->left_reciprocal_1);

        mod0inv = 0;
        if (IS_ODD(a[0])) {
            OK = OK && two_adic_inverse(a[0], &mod0inv);
        }
        pmodulo->right_reciprocal_1 = mod0inv;
                               /* For use by mod_shift if modulus is odd */

        if (reddir == FROM_LEFT) {
            const CRYPTO_DWORD shiftamt = pmodulo->left_reciprocal_1.shiftamt;
            const CRYPTO_DWORD ldividend = lnga + lngred2;

            pmodulo->scaling_power2 = 0;

/*
            Compute constants multiplier_first, multiplier_second so that

               0 <= multiplier_first < modulus
               0 <= multiplier_second < RADIX^lngred2

                  modulus * (multiplier_second + RADIX^lngred2 + 1)
                = RADIX^(length + lngred2) / 2^shiftamt + multiplier_first

            That is, if we divide

                  (RADIX^dividend / 2^shiftamt) - 1

            by modulus, the results will be

                   quotient = multiplier_second + RADIX^lngred2
                   remainder = modulus - 1 - multiplier_first
*/
            for (i = 0; i != ldividend; i++) {
                mptmp1[i] = RADIXM1;
            }
            mptmp1[ldividend-1] = RADIXM1 >> shiftamt;

            OK = OK && divide(mptmp1, ldividend, a, lnga,
                              &pmodulo->left_reciprocal_1,
                              quotient, remainder);

            if (quotient[lngred2] != 1) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                          "create_modulus -- divide error");
                       // quotient should start with 1
            }

            add_immediate(remainder, DIGIT_ONE, remainder, lnga);
            sub_same(pmodulo->modulus, remainder, remainder, lnga);
                           // modulus - 1 - remainder
        } else if (reddir == FROM_RIGHT) {
            pmodulo->scaling_power2 = (CRYPTO_INT)(RADIX_BITS*lnga);

            if (mod0inv == 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                                "create_modulus FROM_RIGHT even");
            } else {
/*
                Compute multiplier_first, multiplier_second, such that

                    0 <= multipler_first < modulus
                    0 <= multiplier_second < RADIX^lngred2

                       multiplier_second * modulus
                 = 1 + multiplier_first * RADIX^lngred2
*/
                pmodulo->multiplier_second[0] = mod0inv;
                mptmp1[lnga] =
                    multiply_immediate(pmodulo->modulus, mod0inv,
                                       mptmp1, lnga);
                        /* 1 mod RADIX,  0 mod modulo->modulus */
                DRMASSERT(mptmp1[0] == 1);

                for (i = 1; i != lngred2; i++) {
                    const digit_t mul = 0 - mod0inv*mptmp1[i];     /* mod RADIX */

                    pmodulo->multiplier_second[i] = mul;
                    mptmp1[i + lnga] = accumulate(pmodulo->modulus, mul,
                                          &mptmp1[i], lnga);
                       /* New mptmp1[i] = 0 */
                       /* Sum is 1 mod RADIX^(i+1),  0 mod modulo->modulus */
                    DRMASSERT (mptmp1[i] == 0);
                }
                memcpy(pmodulo->multiplier_first, mptmp1 + lngred2, lnga* sizeof( digit_t ));
                       /* -1/RADIX^lngred2 mod modulus */
            }
        } else {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                         "create_modulus -- neither FROM_LEFT nor FROM_RIGHT");
        }

        {
            const digit_t d1 = 1;
            CRYPTO_INT modmul_algorithm_index = 0;

            OK = OK && modmul_choices1(pmodulo, &modmul_algorithm_index);
            if (!OK) {
            } else if (modmul_algorithm_index == 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                    "create_modulus -- no modmul routine found");
                         // No qualifying routine found
            } else {
                modmul_algorithm_index = -modmul_algorithm_index;
                OK = OK && modmul_choices1(pmodulo, &modmul_algorithm_index);
                DRMASSERT (modmul_algorithm_index == 0);
            }

/*
                Generate the constant one.
                Its numerical value is 2^scaling_power2 mod modulus.
*/
            OK = OK && to_modular(&d1, 1, pmodulo->one, pmodulo, f_pBigCtx);
        } // local block
    } // if


    if (dtemps_shortterm != NULL) {
        Free_Temporaries(dtemps_shortterm);
    }
    if (!OK) {
        pmodulo->modulus = NULL;

        if (dtemps_longterm != NULL) {
            Free_Temporaries(dtemps_longterm);
        }
    }
    return OK;
}   /* create_modulus */
/******************************************************************************/
CRYPTO_BOOL from_modular
        (const digit_t        a[],
         digit_t         b[],
         const mp_modulus_t  *pmodulo)
{
    CRYPTO_BOOL OK = TRUE;

    OK = OK && validate_modular_data(a, pmodulo->modulus,
                               pmodulo->length, (CRYPTO_CHAR *)"from_modular");
    OK = OK && mod_shift(a, -pmodulo->scaling_power2, b, pmodulo);
    return OK;
} /* from_modular */
/******************************************************************************/
CRYPTO_BOOL mod_mul
        (const digit_t       a[],
         const digit_t       b[],
         digit_t        c[],
         const mp_modulus_t *pmodulo,
         digit_t       *supplied_temps,   // May be NULL
         struct bigctx_t *f_pBigCtx)
/*
        Compute c = a * b / 2^scaling_power2 (mod modulo->modulus).
        Operands should satisfy 0 <= a, b < modulo->modulus.

        supplied_temps should have pmodulo->modmul_algorithm_temps
        temporaries of type digit_t, or be NULL.
*/
{
    CRYPTO_BOOL OK = TRUE;
    digit_tempinfo_t tempinfo;

    tempinfo.address = supplied_temps;
    tempinfo.nelmt = pmodulo->modmul_algorithm_temps;
    tempinfo.need_to_free = FALSE;

    OK = OK && validate_modular_data(a, pmodulo->modulus,
                                     pmodulo->length, (CRYPTO_CHAR *)"mod_mul 1");
    if (a != b) {
        OK = OK && validate_modular_data(b, pmodulo->modulus,
                                         pmodulo->length, (CRYPTO_CHAR *)"mod_mul 2");
    }
    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"mod_mul");

    OK = OK && (*pmodulo->modmul_algorithm)(a, b, c, pmodulo,
                                            tempinfo.address);

    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} /* mod_mul */
/***************************************************************************/
CRYPTO_BOOL mod_shift
        (const digit_t       a[],
         const CRYPTO_INT           shiftamt,
         digit_t        b[],
         const mp_modulus_t *modulo)
/*
        Compute b = a * 2^shiftamt (mod modulo).
        The shift count, shiftamt, may be positive, negative, or zero.
        If shiftamt < 0, then the modulus must be odd.
        The lengths of a and b should match those of the modulus.
*/
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD lng = modulo->length;
    CRYPTO_INT shift_remaining = shiftamt;

    if (a != b) memcpy( b,a,( lng)*sizeof( digit_t ));

    OK = OK && validate_modular_data(a, modulo->modulus, lng,
                                    (CRYPTO_CHAR *) "mod_shift");

    if (OK && shift_remaining < 0 && IS_EVEN(modulo->modulus[0])) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "mod_shift -- negative count, even modulus");
    }

    while (OK && shift_remaining > 0) {          /* Do left shift, if any */
        const CRYPTO_INT shift_now = MIN(shift_remaining, RADIX_BITS);
                                /* 1 <= shift_now <= RADIX_BITS */
/*
                Multiply b by 2^shift_now and reduce modulo modulo->modulus.
*/
        digit_t carry_out = 0;

        OK = OK && mp_shift_lost(b, shift_now, b, lng,
                                 &carry_out);
        if (OK) {
            const digit_t qest = estimated_quotient_1(carry_out, b[lng-1],
                                                 (lng >= 2 ? b[lng-2] : 0),
                                                 &modulo->left_reciprocal_1);
            carry_out -= decumulate(modulo->modulus, qest, b,lng);

            if (carry_out != 0 || compare_same(b, modulo->modulus, lng) >= 0) {
                carry_out -= sub_same(b, modulo->modulus, b, lng);
            }
            if (carry_out != 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                    "mod_shift left carry nonzero");
            }
        }
        shift_remaining -= shift_now;
    } /* while shift_remaining > 0 */

    while (OK && shift_remaining < 0) {
        const CRYPTO_INT shift_now = MIN(-shift_remaining, RADIX_BITS);
                                /* 1 <= shift_now <= RADIX_BITS */
        const digit_t mul =   (0-modulo->right_reciprocal_1 * b[0])
                       & (RADIXM1 >> (RADIX_BITS - shift_now) );
        digit_t carry, low_bits_lost = 0;
/*
                mul has been chosen so that  b + mul * modulo->modulus
                is a multiple of 2^shift_now.
                Replace b by (b + mul * mudulo->modulus)/2^shift_now.
*/
        carry = accumulate(modulo->modulus, mul, b, lng);
        OK = OK && mp_shift_lost(b, -shift_now, b, lng,
                                 &low_bits_lost);
        b[lng-1] |= carry << (RADIX_BITS - shift_now);

        if (OK && low_bits_lost != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "mod_shift -- low bits lost");
        }
        shift_remaining += shift_now;
    } /* while shift_remaining < 0 */
    return OK;
} /* mod_shift */
/****************************************************************************/
CRYPTO_BOOL to_modular
        (const digit_t      a[],
         const CRYPTO_DWORD     lnga,
         digit_t       b[],
         const mp_modulus_t *pmodulo,
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    const digit_t *arem;
    CRYPTO_DWORD lngarem;
    const CRYPTO_DWORD lngmod = pmodulo->length;
    digit_t *remainder = NULL;   // Length lngmod

    if (compare_diff(a, lnga, pmodulo->modulus, lngmod) >= 0) {
        remainder = digit_allocate(lngmod, (CRYPTO_CHAR *)"to_modular");
        if (remainder == NULL) OK = FALSE;

        OK = OK && divide(a, lnga, pmodulo->modulus, lngmod,
                         &pmodulo->left_reciprocal_1,
                         NULL, remainder);
        arem = remainder;
        lngarem = lngmod;
    } else {
        arem = a;
        lngarem = lnga;
    }

    if ( lngarem > lnga )
    {
        OK = FALSE;
    }

    if (OK) {
        lngarem = significant_digit_count(a, lngarem);
        mp_extend(arem, lngarem, b, lngmod);
                /* Copy remainder to b */
        OK = OK && mod_shift(b, pmodulo->scaling_power2, b,
                             pmodulo);
                                    // Scale by power of 2
    }
    if (remainder != NULL) {
        Free_Temporaries(remainder);
    }
    return OK;
} /* to_modular */
/******************************************************************************/
CRYPTO_VOID uncreate_modulus
        (mp_modulus_t *pmodulo,
         struct bigctx_t *f_pBigCtx)
/*
        Release storage allocated by create_modulus
*/
{
    if (pmodulo->modulus != NULL) {
        Free_Temporaries(pmodulo->modulus);
        pmodulo->modulus = NULL;
    }
} // uncreate_modulus

