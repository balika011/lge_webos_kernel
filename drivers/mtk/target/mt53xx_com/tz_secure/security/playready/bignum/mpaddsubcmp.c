/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigpriv.h"
//#include "drmsal.h"


/*
        File mpaddsubcmp.c.  Add/subtract/compare routines.
        These are candidates for assembly code when
        porting this package to a new architecture.

        Many of these are low-level routines.
        Many return a carry flag (digit_t) rather than success/failure (CRYPTO_BOOL).


OK = add_diff(a, lnga, b, lngb, c, pcarry)
                                           -- c := a + b, where lnga >= lngb.
                                              Return carry in *pcarry).

CRYPTO_BOOL add_full(a, lng, b, lngb, c, plngc)
                                -- c := a + b, return length of c in *pargc,
                                   without knowing whether a or b is longer.

carry  = add_immediate(a, iadd, lnga, b)
                                    -- b := a + iadd, where iadd is a scalar.

OK = add_mod(a, b, c, modulus, lng);
                                    -- c := a + b (mod modulus), all length lng

carry  = add_same(a, b, c, lng)     -- c := a + b, where all have length lng.

carry  = add_sub_same(a, b, c, d, lng)
                                 -- d := a + b - c, where all have length lng.
                                    N.B.  carry may be negative.

icomp = compare_diff(a, lnga, b, lngb) -- Return sign of a - b, where a and b
                                          are allowed to have different lengths.

icomp = compare_same(a, b, lng)        -- Return sign of a - b, where a and b
                                          both have length lng.
                                          Declared inline in bignum.h.

icomp =  compare_sum_diff(a, lnga, b, lngb, c, lngc)
                    -- Return sign of a + b - c, where lengths are different.

icomp =  compare_sum_same(a, b, c, lng) -- Return sign of a + b - c,
                                          where all have length lng.

OK     = neg_mod(a, b, modulus, lng)   -- b := -a (mod modulus),
                                                             all length lng

OK = sub_diff(a, lnga, b, lngb, c, pborrow )
                                       -- c := a - b, where lnga >= lngb.
                                          Return borrow in *pborrow.

borrow = sub_immediate(a, isub, lnga, b)
                                    -- b := a - isub, where isub is a scalar.

OK = sub_mod(a, b, c, modulus, lng);
                                       -- c := a - b (mod modulus),
                                                      all length lng

borrow = sub_same(a, b, c, lng)        -- c := a - b, where all have length lng.

OK = validate_modular_data(data, modulus, lng, hint) --
                                      Validate 0 <= data < modulus, where
                                      data and modulus have length lng.
                                      If not, issue an error and return FALSE.

        Each routine (except multiply_low) returns a carry/borrow flag.
        Those returning TRYE/FALSE error flags have the struct bigctx_t *f_pBigCtx argument,
        which an error handler might need.

Assembly language note:

        If the architecture has fast add/subtract with carry/borrow
        instructions (e.g., ADC/SBB on Intel x86,
        ADDE/SUBE on PowerPC, addxcc/subxcc on 32-bit SPARCs),
        then that is the most natural way to code these routines.
                The Alpha architecture has CMPULT and CMPULE
        instructions to do unsigned compares (returning 0 or 1).
        It also has a conditional move.  A possible inner loop is

                sum = a[i] + carry;
                c[i] = b[i] + sum;
                if (sum != 0) carry = (c[i] < sum);

        for addition (c := a + b) and

                dif = a[i] - b[i];
                c[i] = dif - borrow;
                if (dif != 0) borrow = (a[i] < b[i]);
                                            [ or   a[i] < dif  ]

        for subtraction (c := a - b).  Each loop body has
        two adds/subtracts, one compare, and one conditional move
        (plus loads, stores, and loop control).

                The MIPS 32-bit R2000/R3000 architecture has SLTU
        for unsigned compares but lacks conditional moves
        (although later versions of the architecture have
        conditional moves on zero/nonzero).
        One can emulate the Alpha code, with explicit branches
        when sum == 0 or dif == 0 (and with the store into c[i]
        occupying the delay slot).  Or one can use SLTU twice, computing

                carry = (sum < carry) + (c[i] < sum);
                borrow = (dif < borrow) + (a[i] < b[i]);

        (three adds or subtracts, two SLTU).

                The Pentium IV architecture has ADC and SBB, but they are
        much slower than a regular add or subtract.  One solution inspects the
        upper bits of a[i], b[i], a[i] + b[i] + carry (mod 2^32)
        to identify whether a carry has occurred.  That is,

                ai = a[i];
                bi = b[i];
                sum = ai + bi + carry;
                carry = MAJORITY(ai, bi, ~sum) >> 31;

        The MAJORITY function acts bitwise, returning 1 wherever
        two (or all three) operands have a 1 bit, zero elsewhere.
        On the Pentium, it could expand to

                (ai | bi) ^ ( sum & (ai ^ bi))

        with (ai | bi) expanding as bi | (ai & bi).  Only three registers are
        needed for ai, bi, carry (with sum occupying same register as carry):

                 load ai and bi;
                 carry += ai;
                 ai ^= bi;           // ai ^ bi
                 carry += bi;        // sum = ai + bi + carry
                 bi |= ai;           // bi | (ai ^ bi) = ai | bi
                 store low part of carry
                 carry &= ai;        // sum & (ai ^ bi)
                 carry ^= bi;        // (ai | bi) ^ (sum & (ai ^ bi))
                 carry >>= 31;

        (two adds, four boolean, one shift).  On Pentium IV, the
        adds and boolean operations take 0.5 cycle each; shift takes
        4 cycles; ADC takes 8 cycles.

        Similarly, the inner loop of a subtraction a - b can have

                sum = ai - bi - borrow;
                borrow = MAJORITY(~ai, bi, sum) >> 31;

        with MAJORITY expanding as

                 ai ^ ( (ai ^ bi) | (ai ^ sum))

        These methods will work in SIMD (= MMX or SSE) mode,
        where no carry bit is available.
        They can be used when there are more than two operands,
        such as A + B - C.
*/
/***************************************************************************/
CRYPTO_BOOL add_diff
        (const digit_t    a[],       // IN
         const CRYPTO_DWORD   lnga,      // IN
         const digit_t    b[],       // IN
         const CRYPTO_DWORD   lngb,      // IN
         digit_t     c[],       // OUT
         digit_t    *pcarry)    // OUT         
/*
        Add c := a + b.
        Arrays a and c have length lnga and array b has length lngb,
        where lnga >= lngb.

        *pcarry is set to 1 if there is a carry out of the left of c.
        Otherwise *pcarry is set to 0.
        The store is suppressed if pcarry = NULL;
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (lnga < lngb || ((lnga | lngb) & DWORD_LEFT_BIT)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "add_diff length conflict");
    } else {
        digit_t carry = add_same(a, b, c, lngb);
        carry = add_immediate(a + lngb, carry, c + lngb, lnga - lngb);
        if (pcarry != NULL) {
            *pcarry = carry;
        } else if (carry != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_OVERFLOW,
                            "add_diff overflow");
        }
    }
    return OK;
}  /* add_diff */
/***************************************************************************/
CRYPTO_BOOL add_full
        (const digit_t  a[],       // IN
         const CRYPTO_DWORD lnga,      // IN
         const digit_t  b[],       // IN
         const CRYPTO_DWORD lngb,      // IN
         digit_t   c[],       // OUT
         CRYPTO_DWORD *plngc)     // OUT         
/*
        Compute c = a + b, where either a or b may be longer.
        Set *plngc to MAX(lnga, lngb, length of c).

        If lnga and lngb are minimal (i.e., no leading zeros in a or b),
        then *plngc will be minimal too.
*/
{
    digit_t carry = 0;
    CRYPTO_DWORD lngc;
    CRYPTO_BOOL OK = TRUE;

    if (lnga < lngb) {
        OK = OK && add_diff(b, lngb, a, lnga, c, &carry);
        lngc = lngb;
    } else {
        OK = OK && add_diff(a, lnga, b, lngb, c, &carry);
        lngc = lnga;
    }
    if (OK) {
        if (carry != 0) c[lngc++] = carry;
        *plngc = lngc;
    }
    return OK;
} /* add_full */
/***************************************************************************/
digit_t add_immediate
        (const digit_t   a[],        // IN
         const digit_t   iadd,       // IN
         digit_t    b[],        // IN
         const CRYPTO_DWORD  lng)        // IN
/*
        Compute b = a + iadd, where iadd has length 1.
        Both a and b have length lng.
        Function value is carry out of leftmost digit in b.
*/
{
    digit_t carry = iadd;
    CRYPTO_DWORD i;

    for (i = 0; i != lng; i++) {
        const digit_t bi = a[i] + carry;
        b[i] = bi;

        if (bi >= carry) { // No carry propagation
            if (a != b) {
                memcpy( b + i + 1,a + i + 1,( lng - i - 1)*sizeof( digit_t ));
            }
            return 0;
        }
        carry = 1;
    } // for i
    return carry;
}  // end add_immediate
/****************************************************************************/
CRYPTO_BOOL add_mod
        (const digit_t a[],         // IN
         const digit_t b[],         // IN
         digit_t  c[],         // OUT
         const digit_t modulus[],   // IN
         const CRYPTO_DWORD lng)        // IN         
/*
    c := a + b (mod modulus),where 0 <= a, b < modulus
*/
{
    CRYPTO_BOOL OK = TRUE;
    sdigit_t carry;

    if (lng == 0) {
        OK = FALSE;
    } else {
        const digit_t alead = a[lng-1], blead = b[lng-1], mlead = modulus[lng-1];
        CRYPTO_INT itest;     // Sign of a + b - modulus

        if (alead >= mlead) {
            OK = OK && validate_modular_data(a, modulus, lng,
                                            (CRYPTO_CHAR *) "add_mod arg 1");
        }
        if (blead >= mlead) {
            OK = OK && validate_modular_data(b, modulus, lng,
                                             (CRYPTO_CHAR *) "add_mod arg 2");
        }

        if (OK) {

               // Try to determine whether a + b >= modulus
               // by looking only at the leading digits.

            if (blead > mlead - alead) {
                itest = +1;      // implies a + b > modulus
            } else if (mlead - alead - blead > 1) {
                itest = -1;      // implies a + b < modulus
            } else {
                itest = compare_sum_same(a, b, modulus, lng);
                                 // Compare a + b to modulus
            }

            if (itest >= 0) {
                carry = add_sub_same(a, b, modulus, c, lng);
                                                 // c = a + b - modulus
            } else {
                carry = (sdigit_t)add_same(a, b, c, lng);     // c = a + b
            }
            if (carry != 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "add_mod");
            }
#if BIGNUM_EXPENSIVE_DEBUGGING
            OK = OK && validate_modular_data(c, modulus, lng,
                                              "add_mod result" );
#endif
        }
    }
    return OK;
} // end add_mod
/****************************************************************************/
digit_t add_same(
                const digit_t  a[],     // IN
                const digit_t  b[],     // IN
                digit_t   c[],     // OUT          c := a + b
                const CRYPTO_DWORD lng)     // IN
/*
        c := a + b, where all operands have length lng.

        Function value is 1 if there is a carry out of the
        left of c.  Otherwise function value is zero.
*/
{
    digit_t carry = 0;
    CRYPTO_DWORD i;

    for (i = 0; i < lng; i++) {
        const digit_t ai = a[i];
        const digit_t bi = b[i];

        carry += ai + bi;
        c[i] = carry;
        carry = (ai | bi) ^ ((ai ^ bi) & carry);  // MAJORITY(ai, bi, ~carry)
        carry >>= (RADIX_BITS - 1);
    } // for i
    return carry;
}  /* add_same */
/****************************************************************************/
sdigit_t add_sub_same(
                const digit_t    a[],     // IN
                const digit_t    b[],     // IN
                const digit_t    c[],     // IN
                digit_t     d[],     // OUT
                const CRYPTO_DWORD   lng)     // IN
/*
        d := a + b - c, where all operands have length lng.

        Function value (0, 1, or -1) reflects any carry or
        borrow out of the left of d.
*/
{
    digit_t carry1 = 0, carry2 = 0;
    CRYPTO_DWORD i;

    for (i = 0; i < lng; i++) {
        const digit_t ai = a[i];
        const digit_t bi = b[i];
        const digit_t ci = c[i];
        const digit_t sum1 = ai + bi + carry1;
        const digit_t sum2 = sum1 - ci - carry2;

        d[i] = sum2;
        carry1 = sum1 ^ ((sum1 ^ ai) | (sum1 ^ bi));  // MAJORITY(ai, bi, ~sum1)
        carry2 = sum1 ^ ((sum1 ^ ci) | (sum1 ^ sum2));
                                                    // MAJORITY(~sum1, ci, sum2)
        carry1 >>= (RADIX_BITS - 1);
        carry2 >>= (RADIX_BITS - 1);
    } // for i
    return (sdigit_t)carry1 - (sdigit_t)carry2;
}  /* add_sub_same */
/****************************************************************************/
CRYPTO_INT compare_diff
        (const digit_t    a[],   // IN
         const CRYPTO_DWORD   lnga,  // IN
         const digit_t    b[],   // IN
         const CRYPTO_DWORD   lngb)  // IN

//        Compare two multiple precision numbers a (of length lnga)
//        and b (of length lngb).  Return the sign of a - b, namely

//                +1  if  a > b
//                 0  if  a = b  (after stripping leading zeros)
//                -1  if  a < b .

{
    CRYPTO_DWORD la = lnga, lb = lngb;

    while (la > lb) {
        if (a[la-1] != 0) return +1;
        la--;
    }

    while (lb > la) {
        if (b[lb-1] != 0) return -1;
        lb--;
    }
    DRMASSERT(la == lb);

    while (la != 0) {
        if (a[la-1] != b[la-1]) return (a[la-1] > b[la-1] ? +1 : -1);
        la--;
    }
    return 0;
} // compare_diff
/****************************************************************************/
CRYPTO_INT compare_same
        (const digit_t  a[],       // IN
         const digit_t  b[],       // IN
         const CRYPTO_DWORD lng)       // IN
/*
        Compare two multiple precision numbers a and b each of length lng.
        Function value is the sign of a - b, namely

                          +1 if a > b
                           0 if a = b
                          -1 if a < b
*/
{
    CRYPTO_DWORD i;
    for (i = lng; (i--) != 0; /*null*/) {
        if (a[i] != b[i]) return (a[i] > b[i] ? +1 : -1);
    }
    return 0;
}  /* compare_same */
/****************************************************************************/
CRYPTO_INT compare_sum_diff
        (const digit_t     a[],
         const CRYPTO_DWORD    lnga,
         const digit_t     b[],
         const CRYPTO_DWORD    lngb,
         const digit_t     c[],
         const CRYPTO_DWORD    lngc)
/*
//        Compare multiple-precision integers a, b, c, of lengths
//        lnga, lngb, lngc, respectively.  Return the sign of a + b - c, namely

//                +1  if  a + b > c
//                 0  if  a + b = c
//                -1  if  a + b < c
*/
{
    const CRYPTO_DWORD lmax = MAX(MAX(lnga, lngb), lngc);
    CRYPTO_DWORD i;
    CRYPTO_INT sum_prev = 0;

    for (i = lmax; (i--) != 0; /*null*/) {
        const digit_t aval = (i >= lnga ? 0 : a[i]);
        const digit_t bval = (i >= lngb ? 0 : b[i]);
        const digit_t cval = (i >= lngc ? 0 : c[i]);
        digit_t  sum_now = aval + bval;

        DRMASSERT(sum_prev == 0 || sum_prev == -1);
        sum_prev += (sum_now < aval) - (sum_now < cval);  // -2, -1, 0, or +1
        sum_now -= cval;
              // Exit unless sum_now = sum_prev = 0 or -1
        if (   (digit_t)sum_prev != sum_now
            || ((sum_prev + 3) & 2) == 0) {   // If -2 or 1
            return ((sum_prev + 2) & 2) - 1;  // 1 if sum_prev > 0, else -1
        }
    }  // for i
    return sum_prev;     // 0 or -1
} // compare_sum_diff
/****************************************************************************/
CRYPTO_INT compare_sum_same
       (const digit_t  a[],      // IN
        const digit_t  b[],      // IN
        const digit_t  c[],      // IN
        const CRYPTO_DWORD lng)      // IN
/*
//        Compare multiple-precision integers a, b, c, all of length lng.
//        Return the sign of a + b - c, namely

//                +1  if  a + b > c
//                 0  if  a + b = c
//                -1  if  a + b < c
*/
{
    CRYPTO_DWORD i;
    CRYPTO_INT sum_prev = 0;

    for (i = lng; (i--) != 0; /*null*/) {
        const digit_t aval = a[i];
        const digit_t bval = b[i];
        const digit_t cval = c[i];
        digit_t  sum_now = aval + bval;

        DRMASSERT(sum_prev == 0 || sum_prev == -1);

        sum_prev += (sum_now < aval) - (sum_now < cval);  // -2, -1, 0, or +1
        sum_now -= cval;
              // Exit unless sum_now = sum_prev = 0 or -1
        if (   (digit_t)sum_prev != sum_now
            || ((sum_prev + 3) & 2) == 0) {   // If -2 or 1
            return ((sum_prev + 2) & 2) - 1;  // 1 if sum_prev > 0, else -1
        }
    }
    return sum_prev;     // 0 or -1
} // compare_sum_same
/****************************************************************************/
CRYPTO_BOOL neg_mod(
                const digit_t  a[],         // IN
                digit_t   b[],              // OUT
                const digit_t  modulus[],       // IN
                const CRYPTO_DWORD lng )         // IN                           
/* b := -a (mod modulus), where 0 <= a < modulus */
{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD i;
    digit_t all_zero = 0;

    for (i = 0; i < lng; i++) {
        all_zero |= a[i];
        b[i] = a[i];
    } // First copy a to b

    if (all_zero != 0) {
        digit_t borrow = sub_same(modulus, b, b, lng);
        if (borrow != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "neg_mod input");
        }
    }     // If not identically zero, subtract from modulo->modulus

#if BIGNUM_EXPENSIVE_DEBUGGING
    OK = OK && validate_modular_data(b, modulus, lng,
                                     "neg_mod result" );
#endif
    return OK;
} // end neg_mod
/****************************************************************************/
CRYPTO_BOOL sub_diff
        (const digit_t    a[],        // IN
         const CRYPTO_DWORD   lnga,       // IN
         const digit_t    b[],        // IN
         const CRYPTO_DWORD   lngb,       // IN
         digit_t     c[],        // OUT
         digit_t     *pborrow )  // OUT
/*
        Subtract c := a - b.
        Arrays a and c have length lnga and array b has length lngb,
        where lnga >= lngb.

        *pborrow is set to 1 if there is a borrow out of the left of a.
        Otherwise *pborrow is set to 0.
        This store is suppressed if pborrow = NULL.
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (lnga < lngb || ((lnga | lngb) & DWORD_LEFT_BIT)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "sub_diff -- length conflict");
    } else {
        digit_t borrow = sub_same(a, b, c, lngb);
        borrow = sub_immediate(a + lngb, borrow, c + lngb, lnga - lngb);
        if (pborrow != NULL) {
            *pborrow = borrow;
        } else if (borrow != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_OVERFLOW,
                            "sub_diff underflow");
        }
    }
    return OK;
}  /* sub_diff */
/****************************************************************************/
digit_t sub_immediate
        (const digit_t  a[],   // IN
         const digit_t  isub,  // IN
         digit_t   b[],   // OUT
         const CRYPTO_DWORD lng)   // IN
/*
        Compute b = a - isub, where isub has length 1.
        Both a and b have length lng.
        Function value is borrow out of leftmost digit in b.
*/
{
    digit_t borrow = isub;
    CRYPTO_DWORD i;

    for (i = 0; i != lng; i++) {
        const digit_t ai = a[i];

        b[i] = ai - borrow;
        if (ai >= borrow) { // No carry propagation
            if (a != b) {
                memcpy( b + i + 1,a + i + 1,( lng - i - 1)*sizeof( digit_t ));
            }
            return 0;
        }
        borrow = 1;
    }  // for i
    return borrow;
} // end sub_immediate
/****************************************************************************/
CRYPTO_BOOL sub_mod
        (const digit_t a[],       // IN
         const digit_t b[],       // IN
         digit_t  c[],       // OUT
         const digit_t modulus[], // IN
         const CRYPTO_DWORD lng )      // IN         
// c := a - b (mod modulus), where 0 <= a, b < modulus
{
    CRYPTO_BOOL OK = TRUE;
    sdigit_t carry;

    if (lng == 0) {
        OK = FALSE;
    } else {
        const digit_t alead = a[lng-1], blead = b[lng-1], mlead = modulus[lng-1];
        CRYPTO_INT itest;

        if (alead == blead) {
            itest = compare_same(a, b, lng-1);    // Sign of a - b
        } else {
            itest = (alead < blead ? -1 : +1);
        }

        if (itest < 0) {   // a < b, set c = a - b + modulus
            if (blead >= mlead && OK) {
                OK = validate_modular_data(b, modulus, lng,
                                           (CRYPTO_CHAR *)"sub_mod arg 2");
            }
            carry = add_sub_same(a, modulus, b, c, lng);
        } else {          // b <= a, set c = a - b
            if (alead >= mlead && OK) {
                OK = validate_modular_data(a, modulus, lng,
                                           (CRYPTO_CHAR *)"sub_mod arg 1");
            }
            carry = -(sdigit_t)sub_same(a, b, c, lng);     // c = a - b
        }
        if (!OK) {
        } else if (carry != 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR, "sub_mod");
        } else {
#if BIGNUM_EXPENSIVE_DEBUGGING
            OK = OK && validate_modular_data(c, modulus, lng,
                                             "sub_mod output" );
#endif
        }
    }
    return OK;
} // end sub_mod
/****************************************************************************/
digit_t sub_same(
                const digit_t    a[],       // IN
                const digit_t    b[],       // IN
                digit_t     c[],       // OUT
                const CRYPTO_DWORD   lng)       // IN
/*
        c := a - b, where all operands have length lng.

        Function value is 1 if there is a borrow out of the
        left of a.  Otherwise function value is zero.
*/
{
    digit_t borrow = 0;
    CRYPTO_DWORD i;

    for (i = 0; i < lng; i++) {
        const digit_t ai = a[i];
        const digit_t bi = b[i];
        const digit_t sum = ai - bi - borrow;

        c[i] = sum;
        borrow = ai ^ ((ai ^ bi) | (ai ^ sum));  // MAJORITY(~ai, bi, sum)
        borrow >>= (RADIX_BITS - 1);
    } // for i
    return borrow;
}  /* sub_same */
/****************************************************************************/
CRYPTO_BOOL validate_modular_data
        (const digit_t   data[],          // IN
         const digit_t   modulus[],       // IN
         const CRYPTO_DWORD  lng,             // IN
         const CRYPTO_CHAR *hint )           // IN
{
    CRYPTO_BOOL OK = TRUE;
    if (compare_same(data, modulus, lng) >= 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_MODULAR_TOO_BIG, hint);

#if BIGNUM_EXPENSIVE_DEBUGGING
        {
            CRYPTO_DWORD j;
            for (j = 0; j != lng; j++) {
                TRACE( ( "data[%ld] = %x, modulus[%ld] = %x\n",
                        ( CRYPTO_LONG )j, data[j], ( CRYPTO_LONG )j, modulus[j] ) );
            } // for j
        }
#endif
    }
    return OK;
} // validate_modular_data

