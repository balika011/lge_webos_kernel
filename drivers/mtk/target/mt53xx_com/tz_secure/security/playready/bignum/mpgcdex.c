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

/*
                File mp_gcdex.  Extended GCD algorithm on positive integers.
*/
static CRYPTO_BOOL lehmer_mat22(const CRYPTO_UINT64, const CRYPTO_UINT64,
                                digit_t[4], struct bigctx_t *f_pBigCtx); // This file

inline CRYPTO_DWORD mp_gcdex_ntemps
       (const CRYPTO_DWORD lnga,
        const CRYPTO_DWORD lngb,
        struct bigctx_t *f_pBigCtx)
/* Return temporary count needed by mp_gcdex */
{
    (CRYPTO_VOID)f_pBigCtx;
    return 8*MAX(lnga, lngb) + 6;
} // mp_gcdex_ntemps
/****************************************************************************/
CRYPTO_DWORD mp_invert_ntemps(const CRYPTO_DWORD lng, struct bigctx_t *f_pBigCtx)
/* Return temporary count needed by mp_invert */
{
    return lng + mp_gcdex_ntemps(lng, lng, f_pBigCtx);
} // mp_invert_ntemps
/****************************************************************************/
CRYPTO_BOOL mp_gcdex
        (const digit_t  a[],             // IN  (nonzero)
         const CRYPTO_DWORD lnga,            // IN
         const digit_t  b[],             // IN  (nonzero)
         const CRYPTO_DWORD lngb,            // IN
         digit_t   ainvmodb[],      // OUT
         digit_t   binvmoda[],      // OUT (unless NULL)
         digit_t   gcd[],           // OUT
         digit_t   lcm[],           // OUT (unless NULL)
         CRYPTO_DWORD  *plgcd,          // OUT -- contains length of GCD
         digit_t   *supplied_temps,
                              // TEMPS -- length mp_gcdex_ntemps(lnga, lngb)
         struct bigctx_t *f_pBigCtx)

/*
        Compute gcd = GCD(a, b) and return its length.
        Also return ainvmodb = gcd/a (mod b)
        and binvmoda = gcd/b (mod a), except that binvmoda
        is not computed if the input argument is NULL.
        The length of ainvmodb will be lngb and
        that of binvmoda will be lnga.

        The gcd array should be dimensioned at least MIN(lnga, lngb).
        The output length will be stored in *plgcd.

        We require that a <> 0 and b <> 0.
        If lcm <> NULL. then return lcm = LCM(a, b)
        (least common multiple); the LCM will have length lnga + lngb
        (possibly with leading zeros).
*/
{
    const CRYPTO_DWORD lnga_sig = significant_digit_count(a, lnga);
    const CRYPTO_DWORD lngb_sig = significant_digit_count(b, lngb);
    const CRYPTO_DWORD max_lngab = MAX(lnga, lngb);
    CRYPTO_DWORD  lgcd = 0, lngab[2], lngmuls;
    digit_t *ab[2] = {NULL, NULL};
                // Each subscripts -1 to max_lngab (max_lngab + 2 elements)
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD iterations;
    digit_tempinfo_t tempinfo;

            // Allocated memory

    digit_t *temps_ab0_padded = NULL;    // max_lngab + 2
    digit_t *temps_ab1_padded = NULL;    // max_lngab + 2
    digit_t *temps_mul[2] = {NULL, NULL};   // each max_lngab
    digit_t *temps_prod = NULL;          // 2*max_lngab
    digit_t *temps_quot = NULL;          // max_lngab + 1

    tempinfo.need_to_free = FALSE;
    tempinfo.nelmt = mp_gcdex_ntemps(lnga, lngb, f_pBigCtx);
    tempinfo.address = supplied_temps;
    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"mp_gcdex");

    lngab[0] = lnga_sig;  lngab[1] = lngb_sig;
    if (!OK) {
    } else if (lnga_sig == 0 || lngb_sig == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_ZERO_OPERAND, "mp_gcdex");
    } else if (    a == NULL || b == NULL
                || gcd == NULL || ainvmodb == NULL) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NULL_POINTER, "mp_gcdex");
    } else if (a == gcd || b == gcd || a == ainvmodb || b == ainvmodb) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS, "mp_gcdex");
    } else {
        digit_t *temps_next = tempinfo.address;

        memset(temps_next,0, ( tempinfo.nelmt)*sizeof( digit_t ));
        temps_ab0_padded = temps_next; temps_next += max_lngab + 2;
        temps_ab1_padded = temps_next; temps_next += max_lngab + 2;
        temps_mul[0]     = temps_next; temps_next += max_lngab;
        temps_mul[1]     = temps_next; temps_next += max_lngab;
        temps_prod       = temps_next; temps_next += 2*max_lngab;
        temps_quot       = temps_next; temps_next += max_lngab + 1;
                                       temps_next += max_lngab + 1;

        if (temps_next != tempinfo.address + tempinfo.nelmt) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "mp_gcdex -- alloc");
        } else {
            ab[0] = temps_ab0_padded + 1; // Subscripts -1 to max_lngab
            ab[1] = temps_ab1_padded + 1;
            //    ab[0][-1] = ab[1][-1] = 0;
            //    ab[0][lnga_sig] = ab[1][lngb_sig] = 0

            memcpy( ab[0],a,( lnga_sig)*sizeof( digit_t ));
            memcpy( ab[1],b,( lngb_sig)*sizeof( digit_t ));

           // ZEROMEM(temps_mul[0],( lngb)*sizeof( digit_t ));
           // ZEROMEM(temps_mul[1],( lngb)*sizeof( digit_t ));
            temps_mul[0][0] = 1;  // mul[0] = 1,  mul[1] = 0
        }
    }
    lngmuls = 1;         // Length of temps_mul[0] and temps_mul[1].
                         // One of these may have leading zeros.

    iterations = 0;
    while (OK && lngab[0] != 0 && lngab[1] != 0) {
        const CRYPTO_DWORD lng0 = lngab[0];
        const CRYPTO_DWORD lng1 = lngab[1];
        digit_t *pab0top = &ab[0][lng0-1];
        digit_t *pab1top = &ab[1][lng1-1];
        digit_t topword0 = *pab0top;
        digit_t topword1 = *pab1top;
        const CRYPTO_DWORD topsigbits0 = significant_bit_count(topword0);
        const CRYPTO_DWORD topsigbits1 = significant_bit_count(topword1);
        const CRYPTO_DWORD sigbits0 = RADIX_BITS*(lng0 - 1) + topsigbits0;
        const CRYPTO_DWORD sigbits1 = RADIX_BITS*(lng1 - 1) + topsigbits1;
        const CRYPTO_DWORD sigbitsmax = MAX(sigbits0, sigbits1);

        // Next was once const CRYPTO_DWORD, but that results in
        // unexpected loop bound on IA64.
        // Problem is subscript labmax - 2 = 0x00000000ffffffff
        // when labmax = 1.  Signed arithmetic gives -1 as desired.
        CRYPTO_LONG labmax = (CRYPTO_LONG)MAX(lng0, lng1);
        const CRYPTO_DWORD ibig = (compare_diff(ab[1], lng1, ab[0], lng0) > 0);
                       // identifies which of ab[0], ab[1] is larger
        const CRYPTO_DWORD ismall = 1 - ibig;
        digit_t mat22[4];

        iterations++;
        if (iterations > RADIX_BITS*(lnga + lngb + 1)) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                            "mp_gcdex");
        }
/*
                        Loop invariants:

                            0 <= ab[ismall] <= ab[ibig]

                            mul[0] and mul[1] have leading zeros to length lngb.

                             {ibig, ismall} = {0, 1}

                            Except possibly on first iteration,
                            mul[ibig] <= mul[ismall].

                            There exist c0 >= 0, c1 > 0 such that

                               ( ab[0] )     ( mul[0]   -c0 )   ( a )
                               (       )  =  (              )   (   ) .
                               ( ab[1] )     (-mul[1]    c1 )   ( b )

                            The 2 x 2 matrix has determinant +1,
                            so GCD(a, b) = GCD(ab[0], ab[1]).

                            Invert the matrix to get

                                a = c1*ab[0] + c0*ab[1]
                                b = mul[1]*ab[0] + mul[0]*ab[1]

                            with everything nonnegative.
                            When, say, ibig = 1 and ab[0] = 0 then
                            a = c0*ab[1] and b = mul[0]*ab[1],
                            The latter implies 0 <= mul[0] <= b.
                            Then mul[ibig] <= mul[ismall] implies
                            0 <= mul[1] <= b
                            (ab[0] cannot vanish on first iteration).

---- Distribution of partial quotients

                Fix a positive integer q0.  Let q be a partial quotient from the
                Euclidean algorithm.  Then (Knuth, Seminumerical Algorithms,
                3rd Edition, Section 4.5.3, Theorem E, p. 368,

                      Prob(q < q0) = log2(2q0/(q0+1))
                      Prob(q = q0) = log2((q0^2 + 2q0 + 1)/(q0^2 + 2q0))
                      Prob(q > q0) = log2((q0+2)/(q0+1))

                where log2 denotes the base-2 logarithm.
                According to this model, small quotients are frequent:

                        q = 1   probability 0.415
                        q = 2   probability 0.170
                        q = 3   probability 0.093
                        q = 4   probability 0.059
                        q = 5   probability 0.041
                        q = 6   probability 0.030
                        q = 7   probability 0.022
                        q >= 8  probability 0.170

                As of 1999, integer division is slow on many processors
                (e.g., Intel Pentium) and non-existent on others
                (e.g., DEC/Compaq Alpha, Intel IA-64).
                We check explicitly for small quotients.
*/
        if (!OK) {
        } else if (labmax == 1) { // Implies lng0 == lng1 == 1
                                  // Use single-precision hereafter
            digit_t ab0, ab1, m00, m01, m10, m11, carrys[2];

            // To ensure the arguments to mp_mul22u are in range,
            // we check whether topword0 + topword1 < RADIX.
            // Observe that (new topword0) + (new topword1)
            //  = MAX(old topword0, old topword1) < RADIX.

            if (topword0 > RADIXM1 - topword1) {
                digit_t carry;
                if (ibig == 0) {
                    topword0 -= topword1;
                } else {
                    topword1 -= topword0;
                }
                carry = add_same(temps_mul[0], temps_mul[1],
                                 temps_mul[ibig], lngmuls);
                if (!OK) {
                } else if (carry == 0) {
                } else if (lngmuls >= lngb) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "mp_gcdex -- lngmuls overflow 1");
                } else {
                    temps_mul[ibig][lngmuls] = carry;
                    lngmuls++;
                }
            }  // if topword0 + topword1 >= RADIX

            ab0 = topword0;
            ab1 = topword1;
            m00 = 1; m01 = 0; m10 = 0; m11 = 1;

            while (OK && ab1 != 0) {   // Also exit by break if ab0 == 0
                if (ab0 >= ab1) {
                    if ((ab0 >> 2) >= ab1) {     // Quotient >= 4
                        digit_t q = ab0 / ab1;   // truncated
                        ab0 -= q*ab1;
                        m00 += q*m10;
                        m01 += q*m11;
                    } else {
                        do {
                            ab0 -= ab1;
                            m00 += m10;
                            m01 += m11;
                        } while (ab0 >= ab1);
                    }
                } // if ab0 >= ab1

                DRMASSERT (ab1 > ab0);
                if (ab0 == 0) break;

                if ((ab1 >> 2) >= ab0) {      // Quotient >= 4
                    digit_t q = ab1 / ab0;    // truncated
                    ab1 -= q*ab0;
                    m10 += q*m00;
                    m11 += q*m01;
                } else {
                    do {
                        ab1 -= ab0;
                        m10 += m00;
                        m11 += m01;
                    } while (ab1 >= ab0);
                }
                DRMASSERT (ab0 > ab1);
            } // while


                   // Now    (ab0)     (  m00  -m01 )   ( topword0 )
                   //               =                 *
                   //        (ab1)     ( -m10   m11 )   ( topword1 )

                   // Invert to get

                   //         ( topword0 )   ( m11  m01 )   ( ab0 )
                   //                      =              *
                   //         ( topword1 )   ( m10  m00 )   ( ab1 )


                   // We want to update

                   //        (new mul[0] }   (  m00  m01 )   (old mul[0] )
                   //                      *               *
                   //        (new mul[1] )   (  m10  m11 )   (old mul[1] )

                   // One of ab0, ab1 is zero.  If, say, ab0 = 0 and
                   // ab1 = GCD(topword0, topword1), then

                   //        topword1 = ab1 * m00
                   //        topword0 = ab1 * m01
                   //        ab1 = m11*topword1 - m10*topword0
                   //        0 <= m10 < m00
                   //        1 <= m11 < m01

                   // Hence m10 + m11 < m00 + m11
                   //                 <= topword1 + topword0 < RADIX
                   // A similar inequality holds when ab1 = 0.

            ab[0][0] = ab0;
            ab[1][0] = ab1;
            lngab[0] = (CRYPTO_DWORD)(ab0 != 0);
            lngab[1] = (CRYPTO_DWORD)(ab1 != 0);
            mat22[0] = m00;
            mat22[1] = m01;
            mat22[2] = m10;
            mat22[3] = m11;

            OK = OK && mp_mul22u(mat22, temps_mul[0], temps_mul[1],
                                 lngmuls, carrys);
            if (!OK) {
            } else if (carrys[0] == 0 && carrys[1] == 0) {
            } else if (lngmuls >= lngb) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "mp_gcdex -- lngmuls overflow 2");
            } else {
                temps_mul[0][lngmuls] = carrys[0];
                temps_mul[1][lngmuls] = carrys[1];
                lngmuls++;
            }
        } else if (   sigbits0 > sigbits1 + RADIX_BITS/2
                   || sigbits1 > sigbits0 + RADIX_BITS/2) {
                       // Big difference in bit lengths.
                       // Use multi-precision division.
            CRYPTO_DWORD  lquot = lngab[ibig] - lngab[ismall] + 1;
            const CRYPTO_DWORD lngmul_small
                   = significant_digit_count(temps_mul[ismall], lngmuls);

            OK = OK && divide(ab[ibig], lngab[ibig], ab[ismall], lngab[ismall],
                   NULL, temps_quot, temps_prod);

            if (OK) {
                lquot = significant_digit_count(temps_quot, lquot);
                lngab[ibig] = significant_digit_count(temps_prod,
                                                      lngab[ismall]);

                memcpy( ab[ibig],temps_prod,( lngab[ibig])*sizeof( digit_t ));

                OK = OK && multiply(temps_quot, lquot, temps_mul[ismall],
                     lngmul_small, temps_prod);
                if (OK) {
                    CRYPTO_DWORD ltemp = significant_digit_count(temps_prod,
                                                         lquot + lngmul_small);
                    OK = OK && add_full(temps_prod, ltemp,
                        temps_mul[ibig], lngmuls, temps_mul[ibig],
                        &lngmuls);
                }
            }
        } else {  // Not far apart in bit lengths (and at most one word apart)
            const CRYPTO_DWORD normalize = RADIX_BITS*labmax - sigbitsmax;
            CRYPTO_UINT64 lead0, lead1;

            pab0top[1] = pab1top[1] = 0;      /* Insert leading zeros */

            DRMASSERT (labmax >= 2);    // Implies subscripts are -1 or higher

            lead0 = CRYPTO_UI64HL(DOUBLE_SHIFT_LEFT(ab[0][labmax-1],
                                                  ab[0][labmax-2], normalize),
                                DOUBLE_SHIFT_LEFT(ab[0][labmax-2],
                                                  ab[0][labmax-3], normalize));
            lead1 = CRYPTO_UI64HL(DOUBLE_SHIFT_LEFT(ab[1][labmax-1],
                                                  ab[1][labmax-2], normalize),
                                DOUBLE_SHIFT_LEFT(ab[1][labmax-2],
                                                  ab[1][labmax-3], normalize));

                   // Find 2 x 2 matrix M such that M*[ab0  ab1]^T is "small"

            OK = OK && lehmer_mat22(lead0, lead1, mat22, f_pBigCtx);

            if (OK && (mat22[1] | mat22[2]) == 0) {
                DRMASSERT(mat22[0] == 1 && mat22[3] == 1);  // Identity matrix
                mat22[ibig + 1] = 1;        // Set m01 = 1 if ibig = 0,
                                            //     m10 = 1 if ibig = 1
            }
                 //  Do 2 x 2 multiplies, where mat22 holds [m00, m01, m10, m11]

                 //     (  m00  -m01 ) ( ab[0] )
                 //     ( -m10   m11 ) ( ab[1] )

                 //     (  m00   m01 ) ( mul[0] )
                 //     (  m10   m11 ) ( mul[1] )

                 // to update the multi-precision [ab[0], ab[1]]
                 // and [mul[0], mul[1]] vectors.

            if (OK) {
                const CRYPTO_DWORD lab = MAX(lng0, lng1);
                digit_t carrys[2];
                sdigit_t scarrys[2];
                OK = OK && mp_mul22s(mat22, ab[0], ab[1],  lab,
                                     scarrys);
                if (!OK) {
                } else if (scarrys[0] != 0 || scarrys[1] != 0) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                    "mp_gcdex -- scarrys nonzero\n");
                } else {
                    CRYPTO_LONG lng0new = (CRYPTO_LONG)lab, lng1new = (CRYPTO_LONG)lab;
                    // petmon, September, 2000.
                    //         Lower bounds changed from 1 to 0.
                    //         lng0new and lng1new made signed for IA-64.

                    while (lng0new != 0 && ab[0][lng0new-1] == 0) lng0new--;
                    while (lng1new != 0 && ab[1][lng1new-1] == 0) lng1new--;
                    lngab[0] = (CRYPTO_DWORD)lng0new;
                    lngab[1] = (CRYPTO_DWORD)lng1new;
                }

                OK = OK && mp_mul22u(mat22, temps_mul[0], temps_mul[1],
                                     lngmuls, carrys);
                if (!OK) {
                } else if (carrys[0] == 0 && carrys[1] == 0) {
                } else if (lngmuls >= lngb) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                               "mp_gcdex -- lngmuls overflow 3");
                } else {
                    temps_mul[0][lngmuls] = carrys[0];
                    temps_mul[1][lngmuls] = carrys[1];
                    lngmuls++;
                }
            } // local block to multiply by 2 x 2 matrix
        } // case where we generate 2 x 2 Lehmer matrix
    } /* while */

    if (OK) {

          // One argument has been reduced to zero (or an error has occurred)

        const CRYPTO_DWORD igcd = (lngab[0] == 0 ? 1 : 0);
        lgcd = lngab[igcd];
        memcpy( gcd,ab[igcd],( lgcd)*sizeof( digit_t ));

                  // Verify mul[igcd] <= mul[1-igcd] <= b

        if (   compare_same(b, temps_mul[1-igcd], lngb) < 0
            || compare_same(temps_mul[1-igcd], temps_mul[igcd], lngb) < 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                       "mp_gcdex -- multiplier out of range");
        }
        if (!OK) {
        } else if (igcd == 0) {
            memcpy( ainvmodb,temps_mul[0],( lngb)*sizeof( digit_t ));
        } else {
            sub_same(temps_mul[0], temps_mul[1], ainvmodb, lngb);
        }

        if (OK && lcm != NULL) {
/*
                Compute LCM(a, b) = a*b/gcd.
                We know mul[1-igcd] = b/ab[igcd] = b/gcd,
                since b = ab[0]*mul[1] + ab[1]*mul[0] and ab[1-igcd] = 0.
                Therefore LCM(a, b) = a*mul[1-igcd].
*/
            OK = OK && multiply(a, lnga, temps_mul[1-igcd],
                                lngb, lcm);
        }

        if (OK && binvmoda != NULL) {
    /*
                Compute binvmoda so that

                   a*ainvmodb + b*binvmoda = lcm + gcd.
                                           = a*mul[1-igcd] + gcd

                We want [a*(mul[1-igcd] - ainvmodb) + gcd]/b.
    */
            sub_same(temps_mul[1-igcd],
                     ainvmodb, temps_mul[1-igcd], lngb);
            OK = OK && multiply(a, lnga, temps_mul[1-igcd], lngb,
                                temps_prod);
            OK = OK && add_diff(temps_prod, lnga + lngb, gcd, lgcd,
                                temps_prod, NULL);
                 /* Cannot overflow -- at most a*b + MAX(a, b) */
            OK = OK && divide(temps_prod, lnga + lngb_sig, b, lngb_sig,
                          NULL,
                          temps_quot, temps_mul[1-igcd]);
            if (!OK) {
            } else if (significant_digit_count(temps_mul[1-igcd],  lngb_sig) != 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "mp_gcdex -- nonzero remainder\n");
            } else {
                memcpy( binvmoda,temps_quot,( lnga)*sizeof( digit_t ));
                          /* truncate from lnga + 1 to lnga */
            }
        }  /* if binvmoda */
    }
    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    if (OK) *plgcd = lgcd;
    return OK;
} /* mp_gcdex */
/****************************************************************************/
#if 1

static CRYPTO_BOOL lehmer_mat22
        (const CRYPTO_UINT64   lead0orig,         // IN
         const CRYPTO_UINT64   lead1orig,         // IN
         digit_t     mat22[4],          // OUT
         struct bigctx_t *f_pBigCtx)
/*
            This invokes the Lehmer variant of the extended GCD.
        We are given the 2*RADIX_BITS most significant bits
        lead0orig and lead1orig of ab0 and ab1
        (up to a power-of-2 scale factor).  That is,

                sc*ab0 = lead0orig + eps0
                sc*ab1 = lead1orig + eps1

        where 0 <= eps0, eps1 < 1.  The larger of lead0orig, lead1orig
        has the full 2*RADIX_BITS significant bits.

            Use the continued fraction expansion of lead0orig/lead1orig
        to get the next several partial quotients and a single-precision
        2 x 2 matrix (m00, m01, m10, m11 stored in mat22)

                             ( m00    -m01)
                        M =
                             (-m10     m11)

        of determinant +1 such that  M * [ab0   ab1]^T is `small'.
        The caller uses M to update [ab0 ab1] and [mul0 mul1].
        We promise 0 <= m00, m01, m10, m11 < RADIX/2.

            Invariants:

                    1 <= m00, m11 < RADIX/2
                    0 <= m01, m10 < RADIX/2

                    m00*m11 - m01*m10 = 1

               If lead0 = CRYPTO_UI64HL(lead0h, lead0l) and
                  lead1 = CRYPTO_UI64HL(lead1h, lead1l), then

                    ( lead0 )   ( m00   -m01) ( sc*ab0 - eps0)
                              =
                    ( lead1 )   (-m10    m11) ( sc*ab1 - eps1)
        so
                    ( lead0 + m00*eps0 - m01*eps1 )   ( m00  -m01 ) ( sc*ab0 )
                                                    =
                    ( lead1 + m11*eps1 - m10*eps0 )   (-m10   m11 ) ( sc*ab1 )

        The right side of this identity represents the new scaled ab0
        and ab1 if we had perfect precision in intermediate calculations.
        Looking at the left side (with the updated lead0, lead1),
        and knowing 0 <= m00, m01, m10, m11 < RADIX/2,
        along with 0 <= eps0, eps1 < 1, we can get estimates and error
        bounds for the scaled, full-precision, sc0*ab0 and sc1*ab1
        There are the intervals RADIX*(lead0h - 1/2, lead0h + 3/2)
        and RADIX*(lead1h - 1/2, lead1h + 3/2).
        The old sc0*ab0 and ac1*ab1 were around RADIX^2.
        We try to make lead0h and lead1h small, which
        should alos help ab0 and ab1.
        (CAUTION -- lead0h + 1 and lead1h + 1 may overflow.)

            Conceptually lead0, lead1 are double-length (2*RADIX_BITS bits),
        initialized to lead0orig and lead1orig.  However, we manipulate
        the high and low pieces of each separately.

            WARNING.  This may return the identity matrix
        when lead0 and lead1 are very close (or equal).
*/
{
    digit_t lead0h = CRYPTO_UI64High32(lead0orig), lead0l = CRYPTO_UI64Low32(lead0orig);
    digit_t lead1h = CRYPTO_UI64High32(lead1orig), lead1l = CRYPTO_UI64Low32(lead1orig);
    CRYPTO_BOOL OK = TRUE, progress = TRUE;
    digit_t m00 = 1, m01 = 0, m10 = 0, m11 = 1;

    if (    lead0h == 0
         || lead1h == 0                      // Matrix entries will be too big
         || ((lead0h | lead1h) & RADIX_HALF) == 0) {  // Neither normalized
             OK = FALSE;
             SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "lehmer_mat22 -- Bad lead0h or lead1h");
    }
    while (progress && OK) {
        progress = FALSE;

        if (lead0h - 1 > lead1h && lead0h != 0) {
                //  The (updated) quotient ab0 / ab1 is at least
                //  (lead0h - 1/2)/(lead1h + 3/2) >= lead0h/(lead1h + 2) >= 1
                //  The denominator lead1h + 2 cannot overflow.

            if ((lead0h >> 2) >= lead1h + 2) {   // Quotient >= 4
                const digit_t q = lead0h / (lead1h + 2);
                const CRYPTO_UINT64 prod10_11 = DPRODUU(q, m10 + m11);
                const CRYPTO_UINT64 prod1l = DPRODUU(q, lead1l);

                if (q <= 3) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                    "lehmer_mat22 -- small quotient 1");
                }
                        // Update m00 = m00 + q*m10;
                        //        m01 = m01 + q*m11;
                        //        lead0 -= q*lead1
                        // but only if (new m00) and (mew m01) are < RADIX/2.
                        // Rather than make two tests, we
                        // check whether (new m00) + (new m01) < RADIX/2

                if (   CRYPTO_UI64High32(prod10_11) == 0
                    && CRYPTO_UI64Low32(prod10_11) <= RADIXM1/2 - m00 - m01) {

                    const digit_t prod10 = q*m10;

                    progress = TRUE;
                    lead0h -= q*lead1h + CRYPTO_UI64High32(prod1l)
                                       + (CRYPTO_UI64Low32(prod1l) > lead0l);
                    lead0l -= CRYPTO_UI64Low32(prod1l);

                    m00 += prod10;
                    m01 += (CRYPTO_UI64Low32(prod10_11) - prod10);

                    if ((m00 | m01) >= RADIX_HALF) {
                        OK = FALSE;
                        SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                        "lehmer_mat22 -- m00 or m01 overflow.");
                    }
                } // if CRYPTO_UI64High32
            } else {  // Estimated quotient <= 3 (actual may be slightly larger)
                digit_t overflow_test;
                do {
                    m00 += m10;
                    m01 += m11;
                    overflow_test = (m00 | m01) & RADIX_HALF;
                    lead0h -= lead1h + (lead1l > lead0l);
                    lead0l -= lead1l;
                } while (overflow_test == 0 && lead0h >= lead1h + 2);

                progress = TRUE;
                if (overflow_test != 0) {
                    progress = FALSE;
                    m00 -= m10;
                    m01 -= m11;
                }
            } // large or small quotient?
        }  // lead0h >= lead1h + 2

        if (lead1h - 1 > lead0h && lead1h != 0) {
            if ((lead1h >> 2) >= lead0h + 2) {  // Quotient >= 4
                const digit_t q = lead1h / (lead0h + 2);
                const CRYPTO_UINT64 prod00_01 = DPRODUU(q, m00 + m01);
                const CRYPTO_UINT64 prod0l = DPRODUU(q, lead0l);

                if (q <= 3) {
                    OK = FALSE;
                    SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                    "lehmer_mat22 -- tiny quotient 2");
                }
                            // Update m10 = m10 + q*m00;
                            //        m11 = m11 + q*m01;
                            //        lead1 -= q*lead0
                            // but only if (new m10) and (mew m11)
                            // are both < RADIX/2.
                            // Rather than make two tests, we
                            // check whether (new m10) + (new m11) < RADIX/2

                if (   CRYPTO_UI64High32(prod00_01) == 0
                    && CRYPTO_UI64Low32(prod00_01) <= RADIXM1/2 - m10 - m11) {

                    const digit_t prod00 = q*m00;

                    progress = TRUE;
                    lead1h -= q*lead0h + CRYPTO_UI64High32(prod0l)
                                       + (CRYPTO_UI64Low32(prod0l) > lead1l);
                    lead1l -= CRYPTO_UI64Low32(prod0l);

                    m10 += prod00;
                    m11 += (CRYPTO_UI64Low32(prod00_01) - prod00);

                    if ((m10 | m11) >= RADIX_HALF) {
                        OK = FALSE;
                        SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                        "lehmer_mat22 -- m10 or m11 overflow.");
                    }
                } // if prod_00_01
            } else {     // Estimated quotient <= 3 (actual may be more)
                digit_t overflow_test;
                do {
                    m10 += m00;
                    m11 += m01;
                    overflow_test = (m10 | m11) & RADIX_HALF;
                    lead1h -= lead0h + (lead0l > lead1l);
                    lead1l -= lead0l;
                } while (overflow_test == 0 && lead1h >= lead0h + 2);

                progress = TRUE;
                if (overflow_test != 0) {
                    progress = FALSE;
                    m10 -= m00;
                    m11 -= m01;
                }
            } // large or small quotient?
        }  // lead1h >= lead0h + 2
    }  // while (progress && OK)

    if (!OK) {
    } else if ((m00 | m01 | m10 | m11) & RADIX_HALF) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                        "lehmer_mat22 -- matrix element too high");
    } else {
        mat22[0] = m00;
        mat22[1] = m01;
        mat22[2] = m10;
        mat22[3] = m11;
    }
    return OK;
} // lehmer_mat22

#endif

/****************************************************************************/
CRYPTO_BOOL mp_invert
        (const digit_t    denom[],        // IN
         const digit_t    modulus[],      // IN
         const CRYPTO_DWORD   lng,            // IN
         digit_t     result[],       // OUT
         const CRYPTO_CHAR *caller,         // IN
         digit_t    *supplied_temps, // TEMPS -- length mp_invert_ntemps(lng)
         struct bigctx_t *f_pBigCtx)
/*
     Return result = 1/denom (mod modulus).
     denom, modulus, result all have length lng.
     supplied_temps may be NULL or have length mp_invet_ntemps(lng)
*/
{
    CRYPTO_BOOL OK = TRUE;
    digit_tempinfo_t tempinfo;

    tempinfo.address = supplied_temps;
    tempinfo.need_to_free = FALSE;
    tempinfo.nelmt = mp_invert_ntemps(lng, f_pBigCtx);

    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"mp_invert");
    OK = OK && validate_modular_data(denom, modulus, lng, (CRYPTO_CHAR *) "mp_invert");

    if (!OK) {
    } else if (significant_digit_count(denom,  lng) == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_DIVIDE_ZERO, caller);
    } else {
        CRYPTO_DWORD lgcd = 0;
        digit_t *gcd         = tempinfo.address;
        digit_t *gcdex_temps = gcd + lng;

        DRMASSERT(   gcdex_temps + mp_gcdex_ntemps(lng, lng, f_pBigCtx)
               == tempinfo.address + tempinfo.nelmt);

        OK = OK && mp_gcdex(denom, lng, modulus, lng, result,
                            NULL, gcd, NULL,
                            &lgcd, gcdex_temps, f_pBigCtx);
        if (!OK) {
        } else if (lgcd != 1 || gcd[0] != 1) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_NOT_INVERTIBLE, caller);
            
            TRACE( ( "Using data from %s, mp_invert got nontrivial GCD\n", caller ) ); 
        }
    }
    if (tempinfo.need_to_free == TRUE) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} // mp_invert

