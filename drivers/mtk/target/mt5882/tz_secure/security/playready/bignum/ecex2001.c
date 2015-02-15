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
#include "ecurve.h"


/*
    File ecex2001.c.    Version 01 October 2001

        Let E be an elliptic curve (over GF(2^m) or GF(p), where p is prime).
        The costs of some group operations (using affine coordinates) are
        (I = inversion, S = squaring, M = multiplication)

     Operation           GF(2^m)        GF(p)
        2P              I +  S + 2M    I + 2S + 2M
       P+Q              I +  S + 2M    I +  S + 2M
     2P+Q = P+(P+Q)    2I + 2S + 3M   2I + 2S + 3M  (ecaffine_PPQ)
      Both P +- Q       I + 2S + 4M    I + 2S + 4M
                                                (ecaffine_addition_subtraction)

 3P+Q = (2P) + (P+Q)   2I + 3S + 9M   2I + 4S + 9M  (after replacing I by 3M)
 3P+Q = P+(P+(P+Q))    3I + 3S + 4M   3I + 2S + 4M  (using same idea as in 2P+Q)
 3P+Q = 2(P+Q) + (P-Q) 3I + 4S + 7M   3I + 4S + 7M


    These costs appply to the generic case, where no input is a
    point at infinity and where we encounter no divisions by zero.

        We are given a point P0 on E and a nonnegative exponent expon.
    Select a width w (2 <= w <= WMAX),
    dependent upon the number of bits in our exponent.
    Set oddmax = 2^(w-1) - 1.  The plan is tabulate j*P0 for all odd j,
    1 <= j <= oddmax, as well as j = 3*2^(w - 3) when w > 2.
    The table is constructed as follows (when w <= 6):

                 if (w >= 3) then
                     Use 2P + Q code to form 2*P0 + P0 = 3*P0.
                 end if
                 if (w >= 4) then
                     Use doubling code to form 6*P0.
                     Use (both P +- Q) code to form 5*P0 and 7*P0    (6 +- 1)
                 end if
                 if (w >= 5) then
                     Use doubling code to form 12*P0
                     Use (both P +- Q) code to form 9*P0, 11*P0, 13*P0, 15*P0
                                                     (12 +- 1, 12 +- 3)
                 end if
                 if (w >= 6) then
                     Use doubling code to form 24*P0
                     Use (both P +- Q) code to form 17*P0, 19*P0, ..., 31*P0
                                         (24 +- 1, 24 +- 3, 24 +- 5, 24 +- 7)
                end if

    This strategy can be improved slightly by (for example)
    computing 6*P0 + 6*P0, and 6*P0 +- P0 together, replacing one of the
    inversions by three multiplications.
    This strategy would do  2^(w-3) - 2 such replacements when w >= 4.
    Assuming such is later implemented, this initialization would use

         1           invocation  of 2*P + Q code
         2^(w-3) - 1 invocations of (both P +- Q) code
         w - 3       invocations of doubling code
         2^(w-3) - 2 replacements of an inversion by 3 multiplications

        After this table is built, the idea is to use windows of width w,
    with odd signed digits from -oddmax to +oddmax.
    A simplified, recursive, description is

              Function multiply_by(e)    // Return e*P0, assuming e > 0
                  if (e is small) then
                      use a table entry, or a sum of a few table entries
                  else if e is even then
                      return 2*multiply_by(e/2)
                  else
                      choose j == e (mod 2^w), -oddmax <= j <= oddmax
                      return 2*multiply_by((e-j)/2) + j*P0
                  end if
              end function multiply_by

    We estimate this cost as log2(e) doublings and log2(e)/(w + 1) additions.
    That is log2(e)*w/(w + 1) invocations of the doubling code and
    log2(e)/(w + 1) invocations of the 2*P + Q code

        Suppose w = 6, so that oddmax = 31 and doublings3pwr = 24.
     Here is how we aim to compute expleft*P0 for various small expleft:

No operation

       0                                       0
       table                                   odd 1 to 31
                                               24

One group operation
       P+Q or 2P                               even 2 to 62 (except 24)
                                               odd 33 to 55

Two group operations
       2P + Q or 3P                            odd 57 to 93
                                               2 mod 4, 66 to 86
                                               72
       2(P + Q)                                2 mod 4, 90 to 110
                                               0 mod 4, 64 to 124 (except 72)

Three group operations
       2(P + Q) + R                            odd 95 to 155
       3(P + Q)                                0 mod 6, 96 to 186
                                                   (except 96, 108, 120)
                                               3 mod 6, 99 to 165

     w             = {2, 3, 4,  5,  6}
     doublings3pwr = (1, 3, 6, 12, 24)
     oddmax =        (1, 3, 7, 15, 31)

     if (expleft is high or more bits left) {
         delay
     else if (expleft == 0) then
         identity
     else if (expleftodd && expleft <= oddmax) then
         Copy from table
     else if (expleft == doublings3pwr) then
         Copy from table
     else if (even(expleft) && expleft <= 2*oddmax) then
         Use MIN(expleft - 1, oddmax) and another odd value
     else if (    odd(expleft + doublings3pwr)
              and expleft <= doublings3pwr + oddmax) then
         Use doublings3pwr   and odd value
     else if (expleftodd && expleft <= 3*oddmax) then
         odd1 =  2*((expleft - oddmax + 2)/4) - 1
         Use 2*odd1 + odd
     else if (expleft - doublings3pwr == 2 mod 4, at most 2*oddmax) then
         Use 2*odd + doublings3pwr
     else if (expleft is even, at most 2*(oddmax + doublings3pwr)) then
         delay
     else if (expleft = 0 mod 4, at most 4*oddmax) then
         delay
     else if (expleft == 0 mod 6, at most 6*oddmax) then
         Use 3 *(odd + odd)
     else if (odd (expleft + doublings3pwr) and
              expleft divisible by 3,
              at most 3*(doublings3pwr + oddmax) then
         Use 3*(doublings3pwr + odd)
     else if (expleftodd, at most 5*oddmax) then
         even1 = 2*((expleft - oddmax + 2)/4)
         Use 2*(oddmax + (even1 - oddmax)) + (oddmax or oddmax - 2)
     else
         delay
     end if
*/


CRYPTO_BOOL ecaffine_exponentiation
             (const digit_t    *P0,
              const digit_t        exponent[],
              const CRYPTO_DWORD       lng_exponent,
              digit_t     *Presult,        // OUT
              const ecurve_t      *E,
              struct bigctx_t *f_pBigCtx)

// Compute Pesult = exponent*P0 in the elliptic curve group.
// Unlike ecaffine_exponentiation1998, this code allows
// P0 and Pesult to overlap.
{
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    const CRYPTO_DWORD expbits = mp_significant_bit_count(exponent, lng_exponent);

    CRYPTO_BOOL OK = TRUE;
    CRYPTO_BOOL delay;
    digit_t *dtemps = NULL;
    digit_t *etemps = NULL;              // Length E->ndigtemps;
    digit_t *adjustment_locs = NULL;     // Length lng_exponent
    digit_t *Presult1998 = NULL;         // Length 2*elng
    digit_t *p0muls = NULL;              // length 2*elng ((oddmax + 1)/2)
    digit_t doublings3pwr, expon_partial, oddmax;
    sdigit_t expon_excess;
    CRYPTO_DWORD iexpbit, numdoubling, w;

    static const CRYPTO_DWORD wcutoffs[] = {22, 50, 110, 256, 600};
    #define WMAX ((CRYPTO_DWORD)(2 + sizeof(wcutoffs)/sizeof(wcutoffs[0])))

/*
    Use       w = 2 (digits  -1 to  1) if expbits <= 22,
    otherwise w = 3 (digits  -3 to  3) if expbits <= 50,
    otherwise w = 4 (digits  -7 to  7) if expbits <= 110,
    otherwise w = 5 (digits -15 to 15) if expbits <= 256,
    otherwise w = 6 (digits -31 to 31) if expbits <= 600,
    otherwise w = 7 (digits -63 to 63).

    wcutoffs has approximations to the cutoffs suggested by the
    Maple program at the end of this file.
    [Actual cutoffs are about 20% lower for GF(2^n) than
    for GF(p) -- this table has compromise values.]
 */
    w = 2;
    while (w < WMAX && expbits > wcutoffs[w-2]) w++;

         // Check for Koblitz curves (ec_Koblitz2_exponentiation).
         // The Koblitz code allows overlapping arguments.

    if (E->fexpon != NULL) {
        return (*E->fexpon)(P0, exponent, lng_exponent,
                            Presult, E, f_pBigCtx);
    }

    oddmax = ((digit_t)1 << (w-1)) - 1;          // 1, 3, 7, 15, 31, or 63
/*
         Decide how many temporaries we need.  Allocate them.
*/
    if (OK) {
        const CRYPTO_DWORD ndigtemp = E->ndigtemps + ((CRYPTO_DWORD)oddmax + 3)*elng
                                          + lng_exponent;
        if ( ndigtemp < E->ndigtemps
          || ndigtemp < lng_exponent 
          || ((CRYPTO_DWORD)oddmax + 3)*elng < elng
          || ndigtemp < ((CRYPTO_DWORD)oddmax + 3)*elng ) 
        {
            OK = FALSE;
        }
        else
        {
            dtemps = digit_allocate(ndigtemp, (CRYPTO_CHAR *)"ecaffine_exponentiation");
            if (dtemps == NULL) {
                OK = FALSE;
            } else {
                memset(dtemps,0, ( ndigtemp)*sizeof( digit_t ));

                adjustment_locs = dtemps;
                p0muls = adjustment_locs + lng_exponent;
                Presult1998 = p0muls + (oddmax + 1)*elng;
                etemps = Presult1998 + 2*elng;
                DRMASSERT(etemps + E->ndigtemps == dtemps + ndigtemp);
            }
        }
    }

#define p0location(iodd) (DRMASSERT((iodd) & 1), \
                          DRMASSERT((digit_t)(iodd) <= oddmax), \
                           &p0muls[elng*(iodd) - elng])
               // Return address in p0muls array, after validating
               // subscript in debug mode.

    doublings3pwr = 1;
    if (OK) {
        CRYPTO_DWORD iw;
        digit_t j;
/*
             Initialize table of multiples of P0.

             Presult = doublings3pwr * P0
             p0muls = (1, 3, 5, 7, ..., oddmax) * P0
*/
        memcpy( p0location(1),P0,( 2*  E->fdesc->elng)*sizeof( digit_t ));
        if (w == 2) {
            memcpy( Presult,P0,( 2*  E->fdesc->elng)*sizeof( digit_t ));
                                                      // Only P0 stored in table
        } else {
            doublings3pwr = 3;
            OK = OK && ecaffine_PPQ(P0, P0, p0location(3), +1,
                                    E, etemps, f_pBigCtx);
                                                           // 3*P0

            if (w == 3) {
                memcpy( Presult,p0location(3),( 2*  E->fdesc->elng)*sizeof( digit_t ));
                                                           // P0 and 3*P0 saved
            } else {
                OK = OK && ecaffine_addition(p0location(3), p0location(3),
                                             Presult, +1, E,
                                             etemps, f_pBigCtx);
                                   // Presult = 6*P0

                for (iw = 4; iw <= w; iw++) {
                    doublings3pwr *= 2;
                    DRMASSERT(doublings3pwr == (digit_t)3 << (iw - 3));

                          // At this point Presult = doublings3pwr * P0
                          // p0muls has been initialized p0location(2^(w-1) - 1)

// TBD -- Future inplementation should do all group operations below at once,
// combining the inversions
                    for (j = 1; j >> (iw-3) == 0; j += 2) {
                        OK = OK && ecaffine_addition_subtraction(Presult,
                                        p0location(j),
                                        p0location(doublings3pwr + j),
                                        p0location(doublings3pwr - j),
                                        E, etemps, f_pBigCtx);
                    } // for j
                    if (iw != w) {
                        OK = OK && ecaffine_addition(Presult, Presult,
                                                     Presult, +1, E,
                                                     etemps, f_pBigCtx);
                    }
                }  // for iw
            } // if w > 3
        } // if w > 2
        if ( !OK ) {
            TRACE( ( "ecaffine_exponentiation -- OK = FALSE initializing multiples of P0\n" ) );
        } 
    } // if OK

    numdoubling = 0;
    expon_partial = 0;

    for (iexpbit = 0; iexpbit != expbits; iexpbit++) {
        expon_partial += mp_getbit(exponent, iexpbit)
                                << (iexpbit - numdoubling);
        if (iexpbit < numdoubling + w + 1) {
            /* delay */
        } else {
            if (IS_ODD(expon_partial)) {
                const sdigit_t sindex =   ((expon_partial + oddmax) & (2*oddmax))
                                   - oddmax;
                expon_partial -= (digit_t)sindex;
                DRMASSERT((expon_partial & (2*oddmax + 1)) == 0);
                mp_setbit(adjustment_locs, numdoubling, 1);
            }
            expon_partial >>= 1;
            numdoubling++;
        }
    } // for iexpbit

    delay = TRUE;
    while (OK && delay) {
        const CRYPTO_BOOL expon_partial_odd = IS_ODD(expon_partial);
        const digit_t expon_partial3 = expon_partial - doublings3pwr;
                                         /* May be negative */
        const CRYPTO_BOOL expon_partial3_odd = IS_ODD(expon_partial3);
        const digit_t expon_partial_div3 = expon_partial/3;
        const digit_t expon_partial_mod3 = expon_partial - 3*expon_partial_div3;

        delay = FALSE;
        if (expon_partial == 0) {
            OK = OK && ecaffine_set_infinite(Presult, E, f_pBigCtx);
        } else if (expon_partial_odd && expon_partial <= oddmax) {
            memcpy( Presult,p0location(expon_partial),( 2*  E->fdesc->elng)*sizeof( digit_t ));
        } else if (expon_partial == doublings3pwr) {
            /* Presult already has doublings3pwr * P0 */
        } else if (!expon_partial_odd && expon_partial <= 2*oddmax) {
            const digit_t odd1 = MIN(expon_partial - 1, oddmax);
            const digit_t odd2 = expon_partial - odd1;

            OK = OK && ecaffine_addition(p0location(odd1),
                                         p0location(odd2),
                                         Presult, +1, E,
                                         etemps, f_pBigCtx);
        } else if (   expon_partial3_odd
                   && expon_partial <= doublings3pwr + oddmax) {
            OK = OK && ecaffine_addition(Presult,
                                         p0location(expon_partial3),
                                         Presult, +1, E,
                                         etemps, f_pBigCtx);
        } else if (expon_partial_odd && expon_partial <= 3*oddmax) {
            const digit_t odd1 = ((expon_partial - oddmax)/2) | 1;
            const digit_t odd2 = expon_partial - 2*odd1;
            OK = OK && ecaffine_PPQ(p0location(odd1),
                                    p0location(odd2),
                                    Presult, +1, E,
                                    etemps, f_pBigCtx);
        } else if (    (expon_partial3 & 3) == 2
                    && expon_partial3 <= 2*oddmax) {
            OK = OK && ecaffine_PPQ(p0location(expon_partial3 >> 1),
                                    Presult, Presult, +1, E,
                                    etemps, f_pBigCtx);
        } else if (   !expon_partial_odd
                   && expon_partial <= 2*(oddmax + doublings3pwr)) {
            delay = TRUE;
        } else if ( (expon_partial & 3) == 0 && expon_partial <= 4*oddmax) {
            delay = TRUE;
        } else if (    !expon_partial_odd
                    && expon_partial_mod3 == 0
                    && expon_partial_div3 <= 2*oddmax) {
            const digit_t odd1 = MIN(expon_partial_div3 - 1, oddmax);
            const digit_t odd2 = expon_partial_div3 - odd1;
                            // expon_partial = 3*(odd1 + odd2)

            OK = OK && ecaffine_addition(p0location(odd1), p0location(odd2),
                                         Presult, +1, E,
                                         etemps, f_pBigCtx);
            OK = OK && ecaffine_PPQ(Presult, Presult, Presult, +1, E,
                                         etemps, f_pBigCtx);
        } else if (   expon_partial_mod3 == 0
                   && expon_partial3_odd
                   && expon_partial_div3 - doublings3pwr <= oddmax) {
            DRMASSERT (  expon_partial == 3*(doublings3pwr
                    + (expon_partial_div3 - doublings3pwr)));

            OK = OK && ecaffine_addition(
                           p0location(expon_partial_div3 - doublings3pwr),
                           Presult, Presult, +1, E,
                           etemps, f_pBigCtx);
            OK = OK && ecaffine_PPQ(Presult, Presult, Presult, +1, E,
                           etemps, f_pBigCtx);
        } else if (expon_partial_odd && expon_partial <= 5*oddmax) {
                     // Use 2*(oddmax + odd) + odd
            const digit_t even1 = 2*((expon_partial - oddmax + 2)/4);
            DRMASSERT(expon_partial >= 3*oddmax + 2);

            OK = OK && ecaffine_addition(p0location(oddmax),
                                         p0location(even1 - oddmax),
                                         Presult, +1, E,
                                         etemps, f_pBigCtx);
            OK = OK && ecaffine_PPQ(Presult,
                                    p0location(expon_partial - 2*even1),
                                    Presult, +1, E,
                                    etemps, f_pBigCtx);
        } else {
            delay = TRUE;
        }
        if (!OK) {
            TRACE( ( "ecaffine_exponentiation -- OK = FALSE" ) );
            TRACE( ( "multiplying by expon_partial = %ld\n", ( CRYPTO_LONG )expon_partial ) );            
        } else if (delay) {
            if (expon_partial_odd) {
                const sdigit_t sindex =    ((expon_partial + oddmax) & (2*oddmax))
                                    - oddmax;
                expon_partial -= (digit_t)sindex;
                DRMASSERT((expon_partial & (2*oddmax + 1)) == 0);
                mp_setbit( adjustment_locs, numdoubling, 1 );
            }
            numdoubling++;
            expon_partial >>= 1;
        } // if
    } // while

/*
        At this point, Presult = expon_partial * P0.

        We intend to double Presult numdoubling times,
        occasionally adding or subtracting
        (odd value in [1, oddmax] * P0).
*/

    expon_excess = (sdigit_t)expon_partial;
    for (iexpbit = expbits; iexpbit-- && OK; ) {
        const digit_t adjustment_bit = mp_getbit(adjustment_locs, iexpbit);
        const digit_t exponent_bit = mp_getbit(exponent, iexpbit);

/*
           The final result is

              (Presult - expon_excess*P0) * 2^numdoubling
              + (bits 0 to iexpbit-1 of exponent)*P0
*/
        while (OK && numdoubling > iexpbit + adjustment_bit) {
            OK = OK && ecaffine_addition(Presult, Presult,
                                         Presult, +1, E,
                                         etemps, f_pBigCtx);
            numdoubling--;
            expon_excess *= 2;
        }

        if (!OK) {
        } else if (adjustment_bit == 0) {
            DRMASSERT(iexpbit >= numdoubling);
            expon_excess -= exponent_bit << (iexpbit - numdoubling);
        } else {
            CRYPTO_INT pm1;
            digit_t index;
            sdigit_t sindex;
            const CRYPTO_DWORD pre_doublings = numdoubling - iexpbit;

            expon_excess = (expon_excess << pre_doublings) - exponent_bit;
            pm1 = (expon_excess > 0 ? -1 : +1);

            sindex = 1 | (-expon_excess);
            index = (digit_t)(pm1*sindex);
            expon_excess += sindex;

            if (pre_doublings > 1) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "ecaffine_exponentiation pre_doublings");
                TRACE( ( "ecaffine_exponentiation -- pre_doublings = %ld out of range\n",
                       ( CRYPTO_LONG )pre_doublings ) );
            } else if (index > oddmax) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "ecaffine_exponentiation index > oddmax");
                TRACE( ( "ecaffine_exponentiation -- index = %ld exceeds oddmax = %ld\n",
                       ( CRYPTO_LONG )index, ( CRYPTO_LONG )oddmax ) );
            } else {
                numdoubling = iexpbit;
                DRMASSERT(expon_excess == 0 || expon_excess == 1);
            }

            if (!OK) {
            } else if (pre_doublings == 0) {
                OK = OK && ecaffine_addition(Presult,
                               p0location(index), Presult, pm1, E,
                               etemps, f_pBigCtx);
            } else if (pre_doublings == 1) {
                OK = OK && ecaffine_PPQ(Presult, p0location(index),
                                        Presult, pm1, E,
                                        etemps, f_pBigCtx);
            } else {
                DRMASSERT(FALSE);
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "ecaffine_exponentiation - 3");
            }
        } // if (OK && adjustment_bit)
    } // for iexpbit

    if (OK) {
        DRMASSERT(numdoubling == 0);
        DRMASSERT(expon_excess == 0);
    }
    if (dtemps != NULL) {
        Free_Temporaries(dtemps);
    }
    if (!OK) {
        TRACE( ( "ecaffine_exponentiation exiting unsuccessfully\n" ) );
    }
    return OK;
#undef p0location
#undef WMAX

/*

#      Maple code for estimating change-overs.
#      Paste this code into a maple input buffer.


# I = inversion, S = squaring, M = multiplication

for char2 in [false, true] do

    if (char2) then     # Relative operation costs over GF(2^m)
                        # with polynomial basis.   October, 2001.
        cS := cM/3;
        cI := 6*cM;
        cost2P := costPQ;
    else                # Relative operation costs over GF(p)
        cS := 9*cM/10;
        cI := 11*cM/2;
        cost2P := costPQ + cS;
    fi;
    costPQ := cI + cS + 2*cM;


    costPPQ := 2*costPQ - cM;
    costPpmQ := 2*costPQ - cI;

# Next approximation assumes w >= 2.

    costtot := costPPQ + (2^(w-3) - 1)*costPpmQ
           + (w - 3)*cost2P
           + (2^(w-3) - 2) * (3*cM - cI)
           + (costPPQ + w*cost2P)*log2e/(w + 1);

    for ww from 3 to 8 do
        savings := subs(w = ww, costtot) - subs(w = ww-1, costtot);
        print(char2, ww, evalf(solve(savings = 0, log2e)));
    od:
od;  # char2

# End of maple code

*/
} // ecaffine_exponentiation

