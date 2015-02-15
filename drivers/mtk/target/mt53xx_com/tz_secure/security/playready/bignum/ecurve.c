/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "field.h"
#include "ecurve.h"


//           This file (ecurve.c) has routines for
//       elliptic curve arithmetic.  See ecurve.h for
//       background information.

#ifndef MAX_ECTEMPS
    #error -- MAX_ECTEMPS undefined
#endif





//   An elliptic curve over GF(q) is all points (x, y) satisfying
//   y^2 = x^3 + ax + b mod p, plus a point at infinity.
//   The ecurve_t struct has a and b.

//   Points on an elliptic curve may be represented in affine
//   or projective form.  Affine form has simply x and y.
//   If b = 0, the point at infinity has x = 0 and y = 1.
//   If b <> 0, the point at infinity has x = 0 and y = 0.

//   Projective coordinates use a format suggested in
//   the February 6, 1997 IEEE P1363 Working Draft.
//   We represent (x, y) by (X, Y, Z) where x = X/Z^2 and y = Y/Z^2.
//   Then we need Y^2 = X^3 + aX Z^2 + bZ^4.
//   The point at infinity is (lambda^2, lambda^3, 0) where lambda != 0.


//   Over GF(2^m) we use the curves y^2 + x*y = x^3 + a*x^2 + b
//   where b <> 0.  The affine representation of the point at infinity
//   has x = y = 0.  The negative of a point (x, y) is (x, x+y).

CRYPTO_BOOL ecaffine_addition
        (const digit_t *p1,
         const digit_t *p2,
         digit_t  *p3,     // OUT
         const CRYPTO_INT   addsub,
         const ecurve_t   *E,
         digit_t     *supplied_temps,
         struct bigctx_t *f_pBigCtx)

// Compute p3 = p1 + p2 on an elliptic curve (addsub = +1)
//      or p3 = p1 = p2 (addsub = -1)
// Any or all of p1, p2, p3 can overlap.
{
    CRYPTO_BOOL simple_case = FALSE;
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_BOOL char2 = CHARACTERISTIC_2(fdesc);
    CRYPTO_BOOL fFree = FALSE;
                      /* Is field characteristic 2? */
    const CRYPTO_DWORD elng = fdesc->elng;
    const digit_t *x1 = p1, *y1 = p1 + elng;
    const digit_t *x2 = p2, *y2 = p2 + elng;
    digit_t  *x3 = p3, *y3 = p3 + elng;
    digit_t *t1 = NULL;    // Each length elng
    digit_t *t2 = NULL;
    digit_t *t3 = NULL;
    digit_t *ftemps = NULL;

    if (supplied_temps == NULL) 
    {
        /*
        ** Check for integer underflow/overflow 
        */
        if ( 2*elng < elng
          || 3*elng < elng
          || 3*elng+E->ndigtemps < E->ndigtemps 
          || 3*elng+E->ndigtemps < 3*elng
          || 3*elng+E->ndigtemps*sizeof(digit_t) < 3*elng+E->ndigtemps)
        {
         return FALSE;
        }
        
        Allocate_Temporaries_Multiple(3*elng+E->ndigtemps, digit_t, t1);
        if( t1 == NULL )
        {
            return FALSE;
        }
        fFree = TRUE;
        t2 = t1 + elng;
        t3 = t2 + elng;
        ftemps = t3 + elng;
    } 
    else 
    {
        t1 = supplied_temps;
        t2 = t1 + elng;
        t3 = t2 + elng;
        ftemps = t3 + elng;
    }
    
    if (   (fdesc->ftype != FIELD_Q_MP && !char2)
               || (addsub != 1 && addsub != -1) )
    {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "ecaffine_addition");
    }

#if MAX_ECTEMPS < 3
#error -- Need more EC temporaries
#endif

    if (ecaffine_is_infinite(p2, E, f_pBigCtx)) {
        memcpy( p3,p1,( 2* E->fdesc->elng)*sizeof( digit_t ));
        simple_case = TRUE;
    } else if (ecaffine_is_infinite(p1, E, f_pBigCtx)) {
        if (addsub == +1) {
            memcpy( p3,p2,( 2* E->fdesc->elng)*sizeof( digit_t ));
        } else {
            OK = OK && ecaffine_negate(p2, p3, E, f_pBigCtx);
        }
        simple_case = TRUE;
    } else {                  // t2 = y coordinate of -addsub * (x2, y2)
        if (addsub == -1) {
            memcpy( t2,y2, fdesc->elng*sizeof( digit_t ));  // t2 = y2
        } else if (char2) {
            OK = OK && Kadd(x2, y2, t2, fdesc,
                            f_pBigCtx);  // t2 = -y2 - x2
        } else {
            OK = OK && Knegate(y2, t2, fdesc,
                                f_pBigCtx);   // t2 = -y2
        }                              // Plan to do (x1, y1) - (x2, t2)
        }
    if (simple_case || !OK) {
        // Done
    } else if (Kequal(x1, x2, fdesc, f_pBigCtx)) {
        if (Kequal(y1, t2, fdesc, f_pBigCtx)) {
                                                // Subtracting equal points
            OK = OK && ecaffine_set_infinite(p3, E, f_pBigCtx);
            simple_case = TRUE;
        } else if (char2) {        // Doubling over GF(2^m)
            OK = OK && Kinvert(x1, t1, fdesc, ftemps,
                                f_pBigCtx);  // t1 = 1/x
            OK = OK && Kmul(y1, t1, t1, fdesc, ftemps, f_pBigCtx);
                                                // t1 = y/x
            OK = OK && Kadd(x1, t1, t1, fdesc,
                            f_pBigCtx);   // t1 = m = y/x + x
        } else {                   // Doubling over GF(q)
            OK = OK && Kadd(y1, y1, t2, fdesc, f_pBigCtx);   // t2 = 2*y
            OK = OK && Kmul(x1, x1, t1, fdesc,
                                ftemps, f_pBigCtx);   // t1 = x^2
            OK = OK && Kadd(t1, E->a, t3, fdesc,
                                f_pBigCtx); // t3 = x^2 + a
            OK = OK && Kadd(t1, t3, t3, fdesc,
                                f_pBigCtx);   // t3 = 2x^2 + a
            OK = OK && Kadd(t1, t3, t3, fdesc,
                                f_pBigCtx);   // t3 = 3x^2 + a
            OK = OK && Kinvert(t2, t1, fdesc, ftemps,
                                f_pBigCtx); // t1 = 1/(2*y)
            OK = OK && Kmul(t3, t1, t1, fdesc,
                                ftemps, f_pBigCtx);
                                                // t1 = m = (3x^2 + a)/(2*y)
        } // if x1 == x2
    } else {   // Distinct x coordinates
        OK = OK && Kadd(y1, t2, t3, fdesc,
                        f_pBigCtx);       // t3 = y1 + t2
        if (char2) {
            OK = OK && Kadd(t3, x2, t3, fdesc,
                            f_pBigCtx);
                                        // Use t3 = y1 + t2 + x2 in CRYPTO_CHAR 2
        }
        OK = OK && Ksub(x1, x2, t2, fdesc,
                        f_pBigCtx);       // t2 = x1 - x2
        OK = OK && Kinvert(t2, t1, fdesc, ftemps,
                        f_pBigCtx);   // t1 = 1/t2
        OK = OK && Kmul(t3, t1, t1, fdesc,
                            ftemps, f_pBigCtx); // t1 = m = t3/t2
    }
    if (simple_case || !OK) {
        // Do nothing
    } else if (char2) {
        CRYPTO_DWORD i;
        OK = OK && Kmul(t1, t1, t2, fdesc,
                            ftemps, f_pBigCtx);   // t2 = m^2
        for (i = 0; i != elng; i++) {
            t2[i] = t2[i] ^ t1[i] ^ E->a[i] ^ x2[i];
        }                                       // t2 = m^2 + m - a - x2
        OK = OK && Kmul(t1, t2, t3, fdesc, ftemps, f_pBigCtx);
                                                // t3 = (t2 - 2*x1)*m
        for (i = 0; i != elng; i++) {
            const digit_t x3i = t2[i] ^ x1[i];   // x3 = t2 - x1
            y3[i] = t3[i] ^ y1[i] ^ x3i;    // y3 = -(t2 - 2*x1)*m - y1 - x3
            x3[i] = x3i;
        }
    } else {
        OK = OK && Kmul(t1, t1, t2, fdesc,
                            ftemps, f_pBigCtx); // t2 = m^2
        OK = OK && Ksub(t2, x1, t2, fdesc,
                        f_pBigCtx);     //  m^2 - x1
        OK = OK && Ksub(t2, x2, t2, fdesc,
                        f_pBigCtx);     // t2 = x3 = m^2 - x1 - x2;
        OK = OK && Ksub(x1, t2, t3, fdesc,
                        f_pBigCtx);     // t3 = x1 - x3
        memcpy( x3,t2, fdesc->elng*sizeof( digit_t ));
        OK = OK && Kmul(t1, t3, t2, fdesc,
                        ftemps, f_pBigCtx); // t2 = m*(x1 - x3)
        OK = OK && Ksub(t2, y1, y3, fdesc,
                        f_pBigCtx);     // y3 = m*(x1 - x3) - y1
    }

    if( fFree )
    {
        Free_Temporaries( t1 );
    }
    return OK;
} // end ecaffine_addition


CRYPTO_BOOL ecaffine_addition_subtraction
          (const digit_t *p1,
           const digit_t *p2,
           digit_t  *psum,     // OUT
           digit_t  *pdif,     // OUT
           const ecurve_t   *E,
           digit_t     *supplied_temps,
           struct bigctx_t *f_pBigCtx)

// Compute psum = p1 + p2 and pdif = p1 - p2
// on an elliptic curve.
// Do this in a way which uses only one inversion.
// Outputs may overlap inputs.
{
    CRYPTO_BOOL special_case = FALSE;
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_BOOL char2 = CHARACTERISTIC_2(fdesc);
                      /* Is field characteristic 2? */
    const CRYPTO_DWORD elng = fdesc->elng;
    const digit_t *x1 = p1, *y1 = p1 + elng;
    const digit_t *x2 = p2, *y2 = p2 + elng;

    CRYPTO_DWORD i;

    if (fdesc->ftype != FIELD_Q_MP && !char2) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "ecaffine_addition_subtraction");
    } else if (supplied_temps == NULL) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NULL_POINTER,
                        "ecaffine_addition_subtraction");
    }

     if (OK) {
#if !defined(MAX_ECTEMPS) || MAX_ECTEMPS < 5
    #error -- Increase MAX_ECTEMPS
#endif
// WARNING -- ps must come first for special_case code to work.
// This also requires that ecaffine_addition use only three field temps.
        digit_t *ps = supplied_temps;
        digit_t *pd = ps + 2*elng;
        digit_t *t0 = pd + 2*elng;
        digit_t *ftemps = t0 + elng;
        digit_t *special_case_temps = pd;

            // If either input or either output is
            // the point at infinity, use special code.

        if (   ecaffine_is_infinite(p1, E, f_pBigCtx)
            || ecaffine_is_infinite(p2, E, f_pBigCtx)
            || Kequal(x1, x2, fdesc, f_pBigCtx)) {

            special_case = TRUE;
            OK = OK && ecaffine_addition(p1, p2, ps, +1, E,
                                        special_case_temps, f_pBigCtx);
            OK = OK && ecaffine_addition(p1, p2, pdif, -1, E,
                                         special_case_temps, f_pBigCtx);
            memcpy( psum,ps,( 2*  E->fdesc->elng)*sizeof( digit_t ));
        }

        if (OK && !special_case) {
            digit_t *psx = ps, *psy = ps + elng;
            digit_t *pdx = pd, *pdy = pd + elng;

            OK = OK && Ksub(x1, x2, psx, fdesc, f_pBigCtx);
            OK = OK && Kinvert(psx, t0, fdesc,
                               ftemps, f_pBigCtx);  // t0 = 1/(x1 - x2)
            if (char2) {
                for (i = 0; OK && i != elng; i++) {
                    const digit_t deltay = y1[i] ^ y2[i];   // y1 - y2

                    psy[i] = deltay;
                    pdy[i] = deltay ^ x2[i];           // y1 - (-y2 - x2)
                }
            } else {
                OK = OK && Ksub(y1, y2, psy, fdesc,
                                f_pBigCtx);   // y1 - y2
                OK = OK && Kadd(y1, y2, pdy, fdesc,
                                f_pBigCtx);   // y1 - (-y2)
            }
            OK = OK && Kmul(t0, psy, psy, fdesc, ftemps,
                                f_pBigCtx);
                                             // ms = (y1 - y2)/(x1 - x2)
            OK = OK && Kmul(t0, pdy, pdy, fdesc, ftemps,
                                f_pBigCtx);
                                             // md = (y1 + y2)/(x1 - x2)
                                             // or (y1 + y2 + x2)/(x1 - x2)
            OK = OK && Kmul(psy, psy, psx, fdesc, ftemps,
                                f_pBigCtx);  // ms^2
            OK = OK && Kmul(pdy, pdy, pdx, fdesc, ftemps,
                                f_pBigCtx);  // md^2
            if (char2) {
                for (i = 0; OK && i != elng; i++) {
                    const digit_t itemp = E->a[i] ^ x1[i] ^ x2[i];

                    psx[i] ^= psy[i] ^ itemp;  // ms^2 + ms - a - x1 - x2
                    pdx[i] ^= pdy[i] ^ itemp;  // md^2 + md - a - x1 - x2
                }
            } else {
                OK = OK && Kadd(x1, x2, t0, fdesc,
                                f_pBigCtx);    // x1 + x2
                OK = OK && Ksub(psx, t0, psx, fdesc,
                                f_pBigCtx);  // ms^2 - x1 - x2
                OK = OK && Ksub(pdx, t0, pdx, fdesc,
                                f_pBigCtx);  // md^2 - x1 - x2
            }

              // Now psx has x(P1 + P2) and pdx has x(P1 - P2)
              // Compute y(P1 + P2) = ms*(x1 - psx) - y1
              //     (subtract another psx in characteristic 2
              // Likewise y(P1 - P2) = md*(x1 - pdx) - y1

            OK = OK && Ksub(x1, psx, t0, fdesc,
                            f_pBigCtx);
            OK = OK && Kmul(psy, t0, psy, fdesc, ftemps,
                                f_pBigCtx);
            OK = OK && Ksub(x1, pdx, t0, fdesc,
                            f_pBigCtx);
            OK = OK && Kmul(pdy, t0, pdy, fdesc, ftemps,
                                f_pBigCtx);

            if (char2) {
                for (i = 0; OK && i != elng; i++) {
                    const digit_t y1i = y1[i];
                    const digit_t xsumi = psx[i];
                    const digit_t ysumi = psy[i] ^ y1i ^ xsumi;
                    const digit_t xdifi = pdx[i];
                    const digit_t ydifi = pdy[i] ^ y1i ^ xdifi;

                    psum[i]      = xsumi;
                    psum[i+elng] = ysumi;

                    pdif[i]      = xdifi;
                    pdif[i+elng] = ydifi;
                } // for i
            } else {
                OK = OK && Ksub(psy, y1, psy, fdesc, f_pBigCtx);
                OK = OK && Ksub(pdy, y1, pdy, fdesc, f_pBigCtx);

                for (i = 0; OK && i != elng; i++) {
                    const digit_t xsumi = psx[i];
                    const digit_t ysumi = psy[i];
                    const digit_t xdifi = pdx[i];
                    const digit_t ydifi = pdy[i];

                    psum[i]      = xsumi;
                    psum[i+elng] = ysumi;
                    pdif[i]      = xdifi;
                    pdif[i+elng] = ydifi;
                } // for i
            }
        } // if !special_case
    }
    if (!OK) {
        TRACE( ( "ecaffine_addition_subtraction exiting abnormally\n" ) );
    }
    return OK;
} // end ecaffine_addition_subtraction


CRYPTO_BOOL ecaffine_is_infinite (
    const digit_t *p1,
    const ecurve_t   *E,
    struct bigctx_t *f_pBigCtx )
    
// Check for point at infinity,
// If b = 0, check whether x = 0 and y = 1.
// If b <> 0, check whether x = y = 0

//    If an error occurs, this routine returns FALSE.
{
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE, infinite;

    if (Kiszero(p1, fdesc, f_pBigCtx)) {    // If x = 0
        if (E->biszero) {
            infinite = Kequal(p1 + elng, fdesc->one, fdesc, f_pBigCtx);
        } else {
            infinite = Kiszero(p1 + elng, fdesc, f_pBigCtx);
        }
    } else {
        infinite = FALSE;
    }
    return OK && infinite;
} // end ecaffine_is_infinite


CRYPTO_BOOL ecaffine_multiply_pm1(
    const digit_t *p1,
    digit_t  *p2,
    const CRYPTO_INT    negate_flag,
    const ecurve_t   *E,
    struct bigctx_t *f_pBigCtx)

//      Set p2 =  p1 if negate_flag = +1
//      Set p2 = -p1 if negate_flag = -1

{
    CRYPTO_BOOL OK = TRUE;
    if (negate_flag == +1) {
        memcpy( p2,p1,( 2*  E->fdesc->elng)*sizeof( digit_t ));
    } else if (negate_flag == -1) {
        OK = OK && ecaffine_negate(p1, p2, E, f_pBigCtx);
    } else {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "ecaffine_multiply_pm1");
    }
    return OK;
} // end ecaffine_multiply_pm1



CRYPTO_BOOL ecaffine_negate
        (const digit_t *p1,
         digit_t  *p2,
         const ecurve_t   *E,
         struct bigctx_t *f_pBigCtx)

// Compute p2 = -p1 with elliptic curve arithmetic.
{
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;

    if (ecaffine_is_infinite(p1, E, f_pBigCtx)) {
        memcpy( p2 + elng,p1 + elng, fdesc->elng*sizeof( digit_t ));    // y2 = y1
    } else if (CHARACTERISTIC_2(fdesc)) {
        OK = Kadd(p1, p1 + elng, p2 + elng, fdesc,
                  f_pBigCtx); // y2 = x1 + y1
    } else {
        OK = Knegate(p1 + elng, p2 + elng, fdesc,
                  f_pBigCtx);  // y2 = -y1
    }
    memcpy( p2,p1, fdesc->elng*sizeof( digit_t ));                // x2 = x1
    return OK;
} // end ecaffine_negate



CRYPTO_BOOL ecaffine_on_curve
            (const digit_t  *p1,
             const ecurve_t    *E,
             const CRYPTO_CHAR   *pdebug_info,
             digit_t      *supplied_temps,
             struct bigctx_t *f_pBigCtx)

// Test whether p1 = (x1, y1) is on the curve.
// In GF(q) case, check whether y1^2 = x1 * (x1^2 + a) + b.
// In GF(2^m) case, check whether y1*(x1 + y1) = x1^2*(x1 + a) + b.

// When the point is not on the curve, we call SetMpErrno_clue,
// using *pdebug_info as a hint
{
    CRYPTO_BOOL OK = TRUE;
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    const digit_t *x1 = p1, *y1 = p1 + elng;
    digit_t *t1 = NULL, *t2 = NULL, *ftemps = NULL;
    digit_tempinfo_t tempinfo;

    if (ecaffine_is_infinite(p1, E, f_pBigCtx)) return TRUE;

    tempinfo.address = supplied_temps;
    tempinfo.need_to_free = FALSE;
    tempinfo.nelmt = E->ndigtemps;
    OK = OK && possible_digit_allocate(&tempinfo,(CRYPTO_CHAR *)"");

    if (OK) {
        t1 = tempinfo.address;
        t2 = t1 + elng;
        ftemps = t2 + elng;
#if MAX_ECTEMPS < 2
    #error -- "Increase MAX_ECTEMPS"
#endif
    }

    OK = OK && Kmul(x1, x1, t1, fdesc, ftemps, f_pBigCtx);      // x^2

    if (CHARACTERISTIC_2(fdesc)) {       // Characteristic 2
        OK = OK && Kadd(x1, E->a, t2, fdesc,
                        f_pBigCtx);            // t2 = x + a
        OK = OK && Kmul(t1, t2, t1, fdesc, ftemps,
                        f_pBigCtx);  // t1 = x^2*(x + a)
        OK = OK && Kadd(t1, E->b, t1, fdesc,
                        f_pBigCtx);            // t1 = x^2*(x + a) + b
        OK = OK && Kadd(y1, x1, t2, fdesc,
                        f_pBigCtx);              // t2 = y1 + x1;
        OK = OK && Kmul(y1, t2, t2, fdesc, ftemps,
                        f_pBigCtx);  // t2 = y1*(y1 + x1)

    } else {                            // Odd characteristic
        OK = OK && Kadd(t1, E->a, t1, fdesc,
                        f_pBigCtx);            // t1 = x^2 + a
        OK = OK && Kmul(x1, t1, t1, fdesc, ftemps,
                        f_pBigCtx);  // t1 = x*(x^2 + a)
        OK = OK && Kadd(t1, E->b, t1, fdesc,
                        f_pBigCtx);            // t1 = x*(x^2 + a) + b
        OK = OK && Kmul(y1, y1, t2, fdesc, ftemps,
                        f_pBigCtx);  // t2 = y^2
    }
    if (OK && !Kequal(t1, t2, fdesc, f_pBigCtx)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_NOT_ON_CURVE, pdebug_info);
    }
    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} // end ecaffine_on_curve



CRYPTO_BOOL ecaffine_set_infinite
        (digit_t *p1,           // OUT
         const ecurve_t  *E,
         struct bigctx_t *f_pBigCtx)
// Set p1 to identity (point at infinity).
// If b = 0, set x = 0 and y = 1.
// If b <> 0. set x = y = 0.
{
    const field_desc_t *fdesc = E->fdesc;
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;

    OK = OK && Kclear_many(p1, 2, fdesc, f_pBigCtx);
    if (E->biszero) {
        memcpy( p1 + elng,fdesc->one, fdesc->elng*sizeof( digit_t ));
    }
    return OK;
} // end ecaffine_set_infinite


CRYPTO_BOOL ec_free(ecurve_t *E, struct bigctx_t *f_pBigCtx)
// Free the temporaries associated with an elliptic curve.
{
    CRYPTO_BOOL OK = TRUE;
    if (E->free_field && E->fdesc != NULL) {
        OK = OK && Kfree((field_desc_t*)E->fdesc, f_pBigCtx);
                                   // Remove const attribute

        Free_Temporaries((field_desc_t*)E->fdesc);
    }
    if (E->deallocate != NULL) {
        Free_Temporaries(E->deallocate);
    }
    E->deallocate = NULL;
    E->generator = NULL;
    E->a = NULL;
    E->b = NULL;
    E->gorder = NULL;
    E->fdesc = NULL;
    return OK;
} // end ec_free


CRYPTO_BOOL ec_initialize
        (const digit_t      *a,
         const digit_t      *b,
         const field_desc_t *fdesc,
         ecurve_t      *E,         
         struct bigctx_t *f_pBigCtx,
         struct bigctx_t      *pbigctxGlobal)
// Initialize struct for elliptic curve with parameters a and b
{
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;
    digit_t *fields = digit_allocate(5*elng + 1, (CRYPTO_CHAR *)"ec_initialize");
    E->deallocate = fields;
    E->free_field = FALSE;
    E->ndigtemps = fdesc->ndigtemps_arith + MAX_ECTEMPS*elng;
    E->biszero = Kiszero(b, fdesc, f_pBigCtx);

    if (fields == NULL) {
        OK = FALSE;
    } else {
        E->a = fields;  fields += elng;
        E->b = fields;  fields += elng;
        E->generator = fields;  fields += 2*elng;
        E->gorder = fields; fields += (elng+1);
        E->fexpon = NULL;
    }

    if (CHARACTERISTIC_2(fdesc)) {
        OK = OK && !E->biszero;        // GF(2^m) requires b <> 0
    } else {
        digit_t *ftemps = NULL;
        OK = OK && Kimmediate(27, E->b, fdesc, f_pBigCtx)
                && Kmul(E->b, b, E->b, fdesc, ftemps,
                            f_pBigCtx)        // 27*b
                && Kmul(E->b, b, E->b, fdesc, ftemps,
                            f_pBigCtx)        // 27*b^2
                && Kadd(a, a, E->a, fdesc,
                            f_pBigCtx)                       // 2*a
                && Kmul(E->a, E->a, E->a, fdesc, ftemps,
                            f_pBigCtx)     // 4*a^2
                && Kmuladd(a, E->a, E->b, E->a, fdesc, ftemps,
                            f_pBigCtx)   // 4*a^3 + 27*b^2
                && !Kiszero(E->a, fdesc,
                            f_pBigCtx);   // Ensure discriminant nonzero
    }
    if (OK) {
        E->fdesc = fdesc;
        memcpy( E->a,a, fdesc->elng*sizeof( digit_t ));
        memcpy( E->b,b, fdesc->elng*sizeof( digit_t ));

          // Initialize generator to point at infinity
          // and order to 1.  Application can change these.

        OK = OK && ecaffine_set_infinite(E->generator, E, f_pBigCtx)
                && set_immediate(E->gorder, 1, elng+1, f_pBigCtx);
    }
    if (!OK) {
        (CRYPTO_VOID)ec_free(E, f_pBigCtx);
    }
    return OK;
} // end ec_initialize_curve

