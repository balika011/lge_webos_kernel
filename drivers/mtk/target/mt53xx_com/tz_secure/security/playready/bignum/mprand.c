/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
        File mprand.c.  This file has some routines which
        generate random numbers with special distributions.

        CRYPTO_BOOL random_digit_interval(dlow, dhigh, pdout)
              -- Generate pseudorandom value *pdout, dlow <= *pdout <= dhigh.

        CRYPTO_BOOL random_mod(n, array, lng)
                 -- Generate random multiple-precision
                    value -array-, 0 <= array < n.

        CRYPTO_BOOL random_mod_nonzero(n, array, lng)
                 -- Generate random multiple-precision
                    value -array-, 1 <= array < n.

  Alse see file randminv.c
*/
#define MPRAND_C 1   // Tested in mprand.h

#include "tzcrypto.h"
#include "bigdefs.h"
#include "bignum.h"
#include "mprand.h"


CRYPTO_BOOL random_digit_interval
        (const digit_t  dlow,
         const digit_t  dhigh,
         digit_t  *pdout)

{                       /* Return random integer in [dlow, dhigh] */
    CRYPTO_BOOL OK = TRUE;

    if (dhigh < dlow) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "random_digit_interval");
    } else {
        const digit_t spread = dhigh - dlow;
        const CRYPTO_DWORD shift_count = RADIX_BITS - significant_bit_count(spread | 1);
        digit_t result = 0;
        do {
            OK = OK && random_digits(&result, 1);
            result >>= shift_count;
        } while (OK && result > spread);
        *pdout = dlow + result;
    }
    return OK;
} /* random_digit_interval */

/****************************************************************************/
CRYPTO_BOOL random_mod
        (const digit_t   n[],
         digit_t    arr[],
         const CRYPTO_DWORD  lng)
/*
        Generate pseudorandom value in [0, n - 1].
        n has length lng and must be nonzero.
*/
{
    CRYPTO_DWORD lngsig = lng;
    CRYPTO_BOOL OK = TRUE;

    while (lngsig > 0 && n[lngsig-1] == 0) {
        arr[lngsig-1] = 0;
        lngsig--;
    }

    if (!OK) {
    } else if (n == arr) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_OVERLAPPING_ARGS,
                        "random_mod");
    } else if (lngsig == 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_ZERO_OPERAND,
                        "random_mod");
    } else {
        const digit_t nlead = n[lngsig-1];
        CRYPTO_INT ntry = 0;
        do {
            ntry++;
            if (ntry > 100) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                                "random_mod");
            }
            OK = OK && random_digits(arr, lngsig-1);
            OK = OK && random_digit_interval(0, nlead,
                                        &arr[lngsig-1]);
        } while (OK && compare_same(arr, n, lngsig) >= 0);
    }
    return OK;
} /* random_mod */

/****************************************************************************/
CRYPTO_BOOL random_mod_nonzero
        (const digit_t   n[],
         digit_t    arr[],
         const CRYPTO_DWORD  lng)
/*
        Generate pseudorandom value in [1, n-1].  Require n > 1.
*/
{
    CRYPTO_BOOL OK = TRUE;

    if (compare_immediate(n, DIGIT_ONE, lng) <= 0) {
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "random_mod_nonzero");
    } else {
        CRYPTO_INT ntry = 0;
        do {
            ntry++;
            if (ntry > 100) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_TOO_MANY_ITERATIONS,
                                "random_mod_nonzero");
            } else {
                OK = random_mod(n, arr, lng);
            }
        } while (OK && significant_digit_count(arr,  lng) == 0);
    }
    return OK;
} /* random_mod_nonzero */

