/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*
       File kmuladd.c. Version  10 April 2002
*/

#include "tzcrypto.h"
#include "fieldpriv.h"

CRYPTO_BOOL Kmuladd
            (const digit_t *f1,     // Set f4 = f1 * f2 + f3
            const digit_t *f2,
            const digit_t *f3,
            digit_t  *f4,
            const field_desc_t *fdesc,
            digit_t  *supplied_temps,
            struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD elng = fdesc->elng;
    digit_tempinfo_t tempinfo;

    tempinfo.address = supplied_temps;
    tempinfo.nelmt = elng + fdesc->ndigtemps_mul;
    tempinfo.need_to_free = FALSE;

    OK = OK && possible_digit_allocate(&tempinfo, (CRYPTO_CHAR *)"Kmuladd");

    if (OK) {
        digit_t *prod = tempinfo.address;
        digit_t *ftemps = prod + elng;

        OK = OK && Kmul(f1, f2, prod, fdesc, ftemps, f_pBigCtx);
        OK = OK && Kadd(prod, f3, f4, fdesc, f_pBigCtx);
    }
    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address);
    }
    return OK;
} /* Kmuladd */

