/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigdefs.h"
#include "bignum.h"


/*
        File divide.c  Version 02 November 2002.


        This file has five routines:

        CRYPTO_BOOL div21(db, d, &q, &r) --
                                        Divide CRYPTO_UINT64 db by d,
                                        where db < d*RADIX (so that
                                        0 <= quotient < RADIX).
                                        The quotient is stored in q,
                                        and the remainder in r.

        CRYPTO_BOOL div21_fast(db, d, &recip, &q, &r)--
                                        Similar to div21,
                                        but useful when dividing by
                                        the same denominator
                                        repeatedly.  The recip
                                        argument (of type reciprocal_1_t)
                                        must have been initialized
                                        by divide_precondition_1.
                                        The algorithm uses two
                                        multiplications
                                        (top and bottom halves of each)
                                        and several additions/subtractions.

        CRYPTO_BOOL divide(numer, lnum, denom, lden, &recip,
                    quot, rem, plrem, struct bigctx_t *f_pBigCtx) --
                                        Divide numer (of length lnum)
                                        by denom (of length lden).
                                        Quotient
                                        (of length MAX(lnum - lden + 1 ,0))
                                        goes to quot and
                                        remainder (length lden) to rem.
                                        recip can be a reciprocal_1_t
                                        struct for the denominator,
                                        or NULL.

        CRYPTO_BOOL divide_immediate(numer, den, &recip,
                              quot, lng, prem) --
                                        Divide numer (of length lng)
                                        by den (of length 1);
                                        quotient goes to quot
                                        and remainder to *prem.
                                        recip can be the reciprocal_1_t
                                        struct for the denominator,
                                        or NULL.

        CRYPTO_BOOL divide_precondition_1(denom, lng, &reciprocal)
                                        Initialize a reciprocal_1_t
                                        struct for div21_fast or
                                        divide or divide_immediate.        

        The div21 code is slow but assumes only ANSI C.


                Some architectures (e.g., Intel 386, Power PC,
        Motorola 68020, 32-bit SPARC version 8)
        have a hardware instruction to divide a double-length integer
        by a single-length integer.  On such machines,
        the translation of div21 into assembly code
        is straightforward (although we may need to compute
        the remainder via a multiplication and subtraction).
        Other architectures (e.g., MIPS, 64-bit SPARC version 9)
        have an integer division instruction but allow only
        single-length numerator and denominator.
        Still other architectures (e.g., Alpha, IA-64)
        lack an integer divide instruction.
        On the latter architectures, the div21_fast algorithm is attractive.
        Even when there is a hardware instruction which functions
        like div21, the alternative code is attractive if
        division is much slower than multiplication.

                The div21_fast algorithm resembles that on p. 68 of

                        Torjborn Granlund and Peter L. Montgomery,
                        Division by Invariant Integers using Multiplication,
                        pp. 61-72 of Proceedings of SIGPLAN '94 Conference on
                        Programming Language Design and Implementation (PLDI),
                        ACM Press, 1994.

                Let DBOUND = RADIX be a strict upper bound on the divisor.
        and QBOUND = RADIX be a strict upper bound on the quotient.
        These should be powers of 2, with QBOUND dividing DBOUND.
        (The multiple-precision modular reduction
        analogue of this algorithm uses DBOUND = QBOUND^2.)
        Suppose we want to divide repeatedly by d, where 0 < d < DBOUND.
        Start by computing three constants dependent only on d:

                shiftamt = LOG2(DBOUND) - 1 - FLOOR(LOG2(d))
                dnorm = d * 2^shiftamt      // Normalized divisor
                multiplier = FLOOR((DBOUND*QBOUND - 1)/dnorm) - QBOUND

        Also define k = QBOUND*DBOUND - (multiplier + QBOUND)*dnorm.
        These satisfy

                DBOUND/2 <= dnorm < DBOUND
                0 <= multiplier < QBOUND
                0 < k <= dnorm

                Later, given n with 0 <= n <= d*QBOUND - 1,
        do the following computations:

                qprod, nshifted hold values in [0, QBOUND*DBOUND - 1]
                remest holds values in [1-DBOUND, DBOUND-1]
                qest, nshiftedhi hold values in [0, QBOUND - 1]

                nshifted   = n * 2^shiftamt;
                nshiftedhi = FLOOR(nshifted/DBOUND);
                nshiftedlo = (nshifted mod DBOUND);

                adjust = (top bit of nshiftedlo)*FLOOR(multiplier/2);
                qprod =   nshifted
                        + (multiplier*nshiftedhi + adjust)*DBOUND/QBOUND;
                qest = FLOOR(qprod/DBOUND);
                remest = n - qest*d - d;
                if (remest < 0) then
                    quotient is qest, remainder is remest + d
                else
                    quotient is qest+1, remainder is remest
                end if

        We claim that the correct quotient is either qest or qest + 1.
        We achieve this by proving

(*)            0 <= nshifted/dnorm - qprod/DBOUND < 1.

        Add this to qest <= qprod/DBOUND < qest + 1
        and use nshifted/dnorm = n/d to prove

                qest <= n/d < qest + 2.

        Therefore FLOOR(n/d) is either qest or qest + 1.

                Inequality (a) will be useful later:

  (a)     0 <= multiplier*nshiftedlo/DBOUND - adjust
            <  (multiplier + 1)/2
             = (QBOUND*DBOUND - k - QBOUND*dnorm + dnorm)/(2*dnorm)

        The proof of (a) has two cases:

                Case 1:  0 <= nshiftedlo < DBOUND/2 and adjust = 0;

                Case 2:  DBOUND/2 <= nshiftedlo < DBOUND
                         and adjust = FLOOR(multiplier/2).

        Each case is straightforward.

                Next check that

               qprod*QBOUND
             = nshifted*QBOUND + (multiplier*nshiftedhi + adjust)*DBOUND
             =   nshifted*QBOUND
               + multiplier*(nshifted - nshiftedlo) + adjust*DBOUND
             =   nshifted * (multiplier + QBOUND)
               - (multiplier*nshiftedlo - adjust*DBOUND)
             =   nshifted * (QBOUND * DBOUND - k)/dnorm
               - (multiplier*nshiftedlo - adjust*DBOUND)   .

        Multiply this by dnorm/DBOUND to get

                 QBOUND*(nshifted - qprod*dnorm/DBOUND)
             =   QBOUND*nshifted
               - nshifted*(QBOUND*DBOUND - k)/DBOUND
               + dnorm*(multiplier*nshiftedlo - adjust*DBOUND)/DBOUND
             =   nshifted*k/DBOUND
               + dnorm*(multiplier*nshiftedlo/DBOUND - adjust) .

        By (a), the right side above is nonnegative,
        which proves the left inequality in (*).
        Using nshifted < QBOUND*dnorm and (a),
        we can bound the right side above by

        QBOUND*(nshifted - qprod*dnorm/DBOUND)
      = nshifted*k/DBOUND + dnorm*(multiplier*nshiftedlo/DBOUND - adjust)
     <  QBOUND*dnorm*k/DBOUND + (QBOUND*DBOUND - k - QBOUND*dnorm + dnorm)/2
      = k*(QBOUND*dnorm/DBOUND - 1/2) + dnorm/2 + QBOUND*(DBOUND - dnorm)/2
     <=   dnorm*(QBOUND*dnorm/DBOUND - 1/2)
        + dnorm/2 + QBOUND*(DBOUND - dnorm)/2
      = QBOUND*dnorm - QBOUND*(DBOUND - dnorm)*(dnorm/DBOUND - 1/2)
     <= QBOUND*dnorm.

        (we used QBOUND*dnorm/DBOUND >= QBOUND/2 >= 1/2 above).
        This proves the right inequality in (*).
*/

/****************************************************************************/
digit_t  estimated_quotient_1
        (const digit_t         n2,
         const digit_t         n1,
         const digit_t         n0,
         const reciprocal_1_t *recip)
/*
        This routine estimates a quotient digit given
        the three most significant digits of the unshifted
        numerator and given the multiplier from
        divide_precondition_1.

        The quotient returned will be in the interval
        [0, RADIX - 1] and will be correct or one too low.
*/
{
    CRYPTO_UINT64 qprod;
    const digit_t nshiftedhi = DOUBLE_SHIFT_LEFT(n2, n1, recip->shiftamt);
    const digit_t nshiftedlo = DOUBLE_SHIFT_LEFT(n1, n0, recip->shiftamt);
    
    qprod = CRYPTO_UI64Add(CRYPTO_UI64HL(nshiftedhi, nshiftedlo),
                       DPRODUU(nshiftedhi, recip->multiplier));

    if (nshiftedlo & RADIX_HALF) {
        qprod = CRYPTO_UI64Add(qprod, CRYPTO_UI64(recip->multiplier >> 1));
    }
    return CRYPTO_UI64High32(qprod);
} /* estimated_quotient_1 */
/****************************************************************************/
CRYPTO_BOOL  div21(
        const CRYPTO_UINT64    db,        /* Dividend */
        const digit_t       d,          /* Divisor */
        digit_t            *quot,       /* Quotient (output) */
        digit_t            *rem )       /* Remainder (output) */
/*
                This routine divides a double-length
        dividend (db = nhigh * RADIX + nlow) by a single-length divisor (d).
        All arguments are unsigned.
        We require db < d*RADIX, to ensure the quotient will be <= RADIX - 1.
        The quotient *quot and the remainder *rem satisfy the usual rules:

                0 <= *rem < d
                db = d * (*quot) + (*rem)
*/
{
    CRYPTO_BOOL OK = TRUE;
    const digit_t nhigh = CRYPTO_UI64High32(db), nlow = CRYPTO_UI64Low32(db);
    if (!OK) {
    } else if (nhigh >= d) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA, "div21");
    } else {   /* nhigh < d */

#if DBLINT_BUILTIN
/*
                Use the 2-digit / 1-digit division built into the language.
*/
        const digit_t q = (digit_t)(db/CRYPTO_UI64(d));
        *quot = q;
        *rem = nlow - d*q;
#else /* DBLINT_BUILTIN */
/*
                Construct quotient one bit at a time.
*/
        digit_t rhigh = nhigh, rlow = nlow, q = 0, ipow2;

        for (ipow2 = RADIX_HALF; ipow2 != 0; ipow2 >>= 1) {
/*
                At this point,

                nhigh * RADIX + nlow
              = q*d + rhigh*(2*ipow2) + rlow*(2*ipow2/RADIX)

                0 <= rhigh < d

                If 2*rhigh + 2*rlow/RADIX >= d, increase q by ipow2.
                Avoid integer overflow during the test.
*/
            digit_t rlow_hibit = rlow >> (RADIX_BITS - 1);

            if (rhigh + rlow_hibit >= d - rhigh) {
                q += ipow2;
                rlow_hibit -= d;
            }
            rhigh = 2*rhigh + rlow_hibit;
            rlow <<= 1;
        } /* for ipow2 */
/*
        Store quotient and remainder.  rlow is zero now.
*/
        *quot = q;
        *rem = rhigh;
#endif /* DBLINT_BUILTIN */
    } /* if nhigh */
    return OK;
} /* div21 */
/****************************************************************************/
inline CRYPTO_BOOL  div21_fast(
         const CRYPTO_UINT64       db,
         const digit_t          d,
         const reciprocal_1_t  *recip,
         digit_t               *quot,
         digit_t               *rem )
{
/*
        See start of file for explanation of algorithm.
*/
    const digit_t n1 = CRYPTO_UI64High32(db);
    const digit_t n0 = CRYPTO_UI64Low32(db);
    const digit_t qestcomp
        = RADIXM1 - estimated_quotient_1(n1, n0, 0, recip);
                                             /* RADIX - 1 - qest */

    const CRYPTO_UINT64 remest = CRYPTO_UI64Add(CRYPTO_UI64HL(n1 - d, n0),
                                  DPRODUU(qestcomp, d) );

/*
        remest = (n1 - d)*RADIX + n0 + (RADIX - 1 - qest)*d
               = n1*RADIX + n0 - (qest + 1)*d
        is the remainder using qest+1 as estimated quotient.
        Be careful since qest+1 may equal RADIX.
        remest is in [-d, d - 1]

        If CRYPTO_UI64High32(remest) = 0, then remest is nonnegative;
        set the quotient to qest + 1 = RADIX - qestcomp
        and the remainder to CRYPTO_UI64Low32(remest).
        Otherwise CRYPTO_UI64High32(remest) = RADIXM1;
        set the quotient to qest = RADIXM1 - qestcomp
        and the remainder to CRYPTO_UI64Low32(remest) + d - RADIX.
*/
    *quot = CRYPTO_UI64High32(remest) - qestcomp;
    *rem  = CRYPTO_UI64Low32(remest) + (d & CRYPTO_UI64High32(remest));
    return TRUE;
} /* div21_fast */
/****************************************************************************/
CRYPTO_BOOL  divide(
         const digit_t          numer[],
         const CRYPTO_DWORD        lnum,
         const digit_t          denom[],
         const CRYPTO_DWORD        lden,
         const reciprocal_1_t  *supplied_reciprocal,
                                   /* reciprocal_t struct for this denominator,
                                      or NULL
                                      if not previously precomputed */
         digit_t   quot[],     /* Quotient -- length MAX(lnum - lden + 1, 0) */
         digit_t   rem[])      /* Remainder -- length lden  */         
/*
        Divide numer (of length lnum) by denom (of length lden).
        Fifth argument can be the reciprocal_1_t struct
        from divide_precondition_1 if same denominator is used
        repeatedly, or can be NULL if
        this denominator is new.

        Quotient (of length MAX(lnum - lden + 1, 0)) is put in quot.
        However, if quot == NULL, no quotient is returned.

        Remainder (of length lden) is put in rem.
        There is no option to suppress the remainder.

        Function value is the number of significant digits
        in the remainder.  The function value is zero precisely
        when the remainder is zero.

        Quotient and remainder should not overlap other arguments.
        The leading digit of the denominator should be nonzero.
*/
{
    CRYPTO_BOOL OK = TRUE;
    digit_t dlead;

    if (lden <= 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_DIVIDE_ZERO, "divide");
    } else if (numer == NULL || denom == NULL || rem == NULL){
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NULL_POINTER, "divide");
    } else if (numer == quot || numer == rem || denom == quot || denom == rem) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS, "divide");
    }
    if (OK) {
        dlead = denom[lden - 1];
        if (dlead == 0) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                            "divide -- leading zero");
        } else if (lnum < lden) {
            mp_extend(numer, lnum, rem, lden);
                             // Quotient length zero
        } else if (lden == 1) {
            OK = OK && divide_immediate(numer, dlead, supplied_reciprocal,
                                        quot, lnum, &rem[0]);
        } else {
            CRYPTO_DWORD iq, i;
            reciprocal_1_t computed_reciprocal;
            const reciprocal_1_t *used_reciprocal;

            used_reciprocal = supplied_reciprocal;
            if (used_reciprocal == NULL) {
                divide_precondition_1(denom, lden,
                                      &computed_reciprocal);
                used_reciprocal = &computed_reciprocal;
            }
/*
                Copy top lden-1 words of numerator to remainder.
                Zero most significant word of remainder.
*/
            rem[lden-1] = 0;
            memcpy( rem,&numer[lnum-lden+1],( lden-1)*sizeof( digit_t ));

            for (iq = lnum-lden+1; OK && (iq--) != 0; /*null*/) {
                const digit_t remtop = rem[lden-1];
                digit_t qest;
/*
                    Multiply old remainder by RADIX.  Add numer[iq].
*/
                for (i = lden-1; i != 0; i--) {
                    rem[i] = rem[i-1];
                }
                rem[0] = numer[iq];
/*
                    Check for zero digit in quotient.
                    This is especially likely to happen on the
                    first iteration of the iq loop.
*/
                if (remtop == 0 && compare_same(rem, denom, lden) < 0) {
                    qest = 0;
                } else {
                    digit_t borrow;
                    qest = estimated_quotient_1(remtop, rem[lden-1],
                                                rem[lden-2],
                                                used_reciprocal);
                       /* qest is correct or one too low */
                    qest += (qest < RADIXM1);
                       /* Now qest is correct or one too high */
                    borrow = decumulate(denom, qest, rem, lden);
                       /* Subtract qest*denom from rem */
                    if (borrow > remtop) { // If estimated quotient is too high
                        qest--;
                        borrow -= add_same(rem, denom, rem, lden);
                    }

                    if (borrow != remtop) {
                        OK = FALSE;
                        SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                        "divide - Quotient estimation error.\n");
                    }
                } /* qest == 0 */

                if (quot != NULL) quot[iq] = qest;
            }  /* for iq */
        }
    }
    return OK;
}  /* divide */
/****************************************************************************/
CRYPTO_BOOL  divide_immediate(
         const digit_t          numer[],
         const digit_t          den,
         const reciprocal_1_t  *supplied_reciprocal,
         digit_t                quot[],
         const CRYPTO_DWORD        lng,
         digit_t               *prem )      // OUT (remainder)         
/*
        Divide numer (length lng) by den (length 1).
        Quotient (length lng) is written to quot (or can be suppressed
        if quot = NULL).
        Remainder is returned as function value.

        supplied_reciprocal is the output of
        divide_precondition_1 for this denominator,
        or can be NULL if reciprocal was not
        previously computed.
*/
{
    CRYPTO_BOOL OK = TRUE;
    digit_t carry = 0;
    CRYPTO_DWORD i, lngleft = lng;

    if (lngleft > 0 && numer[lngleft-1] < den) {
        lngleft--;
        carry = numer[lngleft];
        if (quot != NULL) quot[lngleft] = 0;
    }

    if (supplied_reciprocal == NULL && lngleft < 2) {
        for (i = lngleft; (i--) != 0; /*null*/) {
            digit_t qest = 0;
            OK = OK && div21(CRYPTO_UI64HL(carry, numer[i]),
                             den, &qest, &carry);
            if (quot != NULL) quot[i] = qest;
        }
    } else {
        reciprocal_1_t computed_reciprocal;
        const reciprocal_1_t *used_reciprocal = supplied_reciprocal;

        if (used_reciprocal == NULL) {
            OK = OK && divide_precondition_1(&den, 1,
                              &computed_reciprocal);
            used_reciprocal = &computed_reciprocal;
        }

        for (i = lngleft; OK && (i--) != 0; /*null*/) {
            digit_t qest = 0;
            OK = OK && div21_fast(CRYPTO_UI64HL(carry, numer[i]),
                                   den,used_reciprocal, &qest,
                                   &carry);
            if (quot != NULL) quot[i] = qest;
        }
    }
    if (OK) *prem = carry;
    return OK;
} /* divide_immediate */
/****************************************************************************/
CRYPTO_BOOL  divide_precondition_1(
         const digit_t         denom[],
         const CRYPTO_DWORD       lden,
         reciprocal_1_t       *recip )
/*
        This routine computes the reciprocal_1_t structure
        for the denominator denom, of length lden.
        The leading digit denom[lden-1] must be nonzero.
        It computes a multiplier accurate enough for estimated_quotient_1
        to predict one digit of a quotient (with an error at most 1).
        This is the case QBOUND = RADIX, DBOUND = RADIX^lden
        of the theory atop this file.
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (denom == NULL || recip == NULL) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NULL_POINTER,
                        "divide_precondition_1");
    } else if (lden == 0 || denom[lden-1] == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "divide_precondition_1");
    } else {
        CRYPTO_DWORD iden, recip_shift;
        digit_t recip_multiplier = 0, dlead[3], dshiftedhi, dshiftedlo, rem = 0;

        recip_shift = RADIX_BITS - significant_bit_count(denom[lden-1]);
        dlead[2] = denom[lden-1];
        dlead[1] = (lden >= 2 ? denom[lden-2] : 0);
        dlead[0] = (lden >= 3 ? denom[lden-3] : 0);

        dshiftedhi = DOUBLE_SHIFT_LEFT(dlead[2], dlead[1], recip_shift);
        dshiftedlo = DOUBLE_SHIFT_LEFT(dlead[1], dlead[0], recip_shift);
/*
             We want our RADIX + multiplier to be the integer part of

                               RADIX^lden - 1
             ---------------------------------------------------------
             dshiftedhi*RADIX^(lden-2) + dshiftedlo*RADIX^(lden-3) + ...


        The leading digit of this quotient is 1*RADIX^1
        since dshiftedhi >= RADIX/2.
        After subtracting RADIX*denominator from the numerator,
        we get the next digit of the quotient by approximating
        the denominator by dshiftedhi*RADIX^(lden-1). Standard theory
        (see, e.g., Knuth, Seminumerical Algorithms, 1981,
        Theorem B, p. 257) says the so-estimated quotient
        differs from the real quotient by at most 2,
        and the so-estimated quotient is never smaller than the real
        quotient.  When we use the two leading digits from the
        divisor, the error in the estimate can be at most one.
        We allow one correction while looking at
        the dshiftedlo term and one more later on.
*/

        OK = OK && div21(CRYPTO_UI64HL(RADIXM1 - dshiftedhi,
                                     RADIXM1 - dshiftedlo),
                         dshiftedhi, &recip_multiplier, &rem);

/*
                Quick adjustment.  Check the sign of

//        RADIX^3 - 1 - (RADIX + multiplier)*(dshiftedhi*RADIX + dshiftedlo)
//       =    RADIX*(RADIX^2 - 1 - dshiftedhi*RADIX - dshliftedlo)
//         - multiplier*(dshiftedhi*RADIX + dshiftedlo)
//         + RADIX - 1
//      = RADIX*(rem + multiplier*dshiftedhi) + RADIX - 1
//         - multiplier*(dshiftedhi*RADIX + dshiftedlo)
//      = rem*RADIX + RADIX - 1 - multiplier*dshiftedlo
*/
        if ( OK ) 
        {   
            CRYPTO_UINT64 tempRem = DPRODUU( recip_multiplier, dshiftedlo );
            
            if ( CRYPTO_UI64High32(tempRem) > rem ) 
            {
                recip_multiplier--;
            }

/*
                Fine adjustment.  Check the sign of

                      RADIX^(lden+1)/2^shiftamt - 1
                    - (RADIX+multiplier)*denom[lden-1:0].

                If this is negative, then the multiplier is too large.
                If this is nonnegative, then the multiplier is correct.
*/
            rem = (RADIXM1 >> recip_shift) - denom[lden-1];
                             /* RADIX/2^shiftamt - 1 - den[lden-1] */
/*
                Repeatedly replace rem by

            rem*RADIX + RADIX - 1 - denom[iden-1] - multiplier*denom[iden]

        until it is known whether rem >= multiplier or rem < 0.
        Once one of these happens, the sign of rem won't change.
*/
            for (iden = lden; (iden--) != 0 && rem < recip_multiplier;/*null*/){
                        /* CAUTION -- loop may exit early */

                CRYPTO_UINT64  test1 = CRYPTO_UI64HL(rem,
                                      RADIXM1 - (iden > 0 ? denom[iden-1] : 0));
                const CRYPTO_UINT64 test2 = DPRODUU(recip_multiplier, denom[iden]);
                if (!CRYPTO_UI64Les(test2, test1)) {
                    recip_multiplier--;
                    break;
                }
                test1 = CRYPTO_UI64Sub(test1, test2);
                rem = CRYPTO_UI64Low32(test1);
                if (CRYPTO_UI64High32(test1) != 0) break;
            } // for iden
            recip->shiftamt   = recip_shift;
            recip->multiplier = recip_multiplier;
        } // if OK
    } // else
    return OK;
} /* divide_precondition_1 */
/****************************************************************************/

