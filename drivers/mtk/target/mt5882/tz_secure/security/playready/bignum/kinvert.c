/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*
     File kinvert.c.    Version 08 March 2002
*/
#include "tzcrypto.h"
#include "fieldpriv.h"


CRYPTO_BOOL Kinvert
        (const digit_t *f1,
         digit_t  *f3,     // OUT
         const field_desc_t *fdesc,
         digit_t  *supplied_temps,
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    digit_tempinfo_t tempinfo;

    tempinfo.address = supplied_temps;
    tempinfo.nelmt = fdesc->ndigtemps_invert1;
    tempinfo.need_to_free = FALSE;

    if (OK && Kiszero(f1, fdesc, f_pBigCtx)) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_DIVIDE_ZERO, "Kinvert");
    }
    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"Kinvert");

    OK = OK && (fdesc->arithmetic->inverter1)(f1, f3, fdesc, &tempinfo,
                                              f_pBigCtx);

    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} // end Kinvert


CRYPTO_BOOL Kinvert_many
        (const digit_t *f1,
         digit_t  *f3,     // OUT
         const CRYPTO_DWORD   nelmt,
         const field_desc_t *fdesc,
         digit_t  *supplied_temps,
         struct bigctx_t *f_pBigCtx)
/*
        Compute f3[i] = 1/f1[i] for 0 <= i < nelmt.
        When nelmt > 1, the f1 and f3 arrays are not allowed to overlap.

        We use the identities 1/x = y*(1/(x*y)) and 1/y = x*(1/(x*y))
        to exchange all but one inversion for three multiplications.

        If the temporaries array is supplied, it must have length
        at least fdesc->ndigtemps_arith .
*/
 {
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;
    digit_tempinfo_t tempinfo, tempinfo2;

    if (nelmt == 0) return OK;
    if (nelmt > 1 && f1 == f3) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS,
                        "Kinvert_many -- Arrays overlap, nelmt > 1");
    }

    tempinfo.address = supplied_temps;
    tempinfo.nelmt = fdesc->ndigtemps_arith;
                    // Field element ftemp
                    // Temporaries for Kinvert and Kmul
    tempinfo.need_to_free = FALSE;

    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"Kinvert_many");

    if (OK) {
/*
                 Suppose the f1 has nelmt = 4 values a0, a1, a2, a3.
                 Successively store the following in f3 and ftemp

             f3[0]       f3[1]       f3[2]      f3[3]         ftemp

               a0
                         a0*a1
                                     a0*a1*a2
                                               a0*a1*a2*a3
                                                             (a0*a1*a2*a3)^(-1)
                                                  a3^(-1)    (a0*a1*a2)^(-1)
                                      a2^(-1)                (a0*a1)^(-1)
                          a1^(-1)                            a0^(-1)
              a0^(-1)

                  We use the identities 1/x = y*(1/(x*y)) and 1/y = x*(1/(x*y))
                  to exchange all but one inversion for three multiplications.

                  When nelmt = 1, we allow f1 == f3 (i.e., same array).
                  The copy of a0 from f1 to f3 becomes a no-op.
*/
        CRYPTO_DWORD i;
        digit_t *ftemp = tempinfo.address;  // Length elng
        digit_t *ftemps = ftemp + elng;  // Length MAX(fdesc->ndigtemps_mul,
                                         //            fdesc->ndigtemps_invert1)

        memcpy( f3,f1, fdesc->elng*sizeof( digit_t ));

        for (i = elng; OK && i != nelmt*elng; i += elng) {
                                           // CAUTION -- index variable used
                                           // after loop exit

            OK = OK && Kmul(f1 + i, f3 + i - elng, f3 + i,
                                fdesc, ftemps, f_pBigCtx);
                     // Store partial products in f3
        }

        i -= elng;     // (nelmt - 1)*elng

        if (!OK) {
        } else if (Kiszero(f3 + i, fdesc, f_pBigCtx)) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_NOT_INVERTIBLE,
                            "Kinvert_many");
        } else {
            tempinfo2.need_to_free = FALSE;
            tempinfo2.address = ftemps;
            tempinfo2.nelmt = fdesc->ndigtemps_invert1;

            OK = OK && (fdesc->arithmetic->inverter1)(f3 + i, ftemp,
                                              fdesc, &tempinfo2, f_pBigCtx);
                           // Invert product of everything

            while (i != 0) {
                OK = OK && Kmul(ftemp, f3 + i - elng,
                                    f3 + i, fdesc, ftemps, f_pBigCtx);
                OK = OK && Kmul(ftemp, f1 + i, ftemp, fdesc,
                                    ftemps, f_pBigCtx);
                i -= elng;
           } // while

           memcpy( f3,ftemp, fdesc->elng*sizeof( digit_t ));
        }
    } // if OK

    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} // end Kinvert_many

