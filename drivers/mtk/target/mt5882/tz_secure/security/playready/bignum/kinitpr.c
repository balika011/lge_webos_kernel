/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*
       File kinitpr.c.   Version 03 October 2002.

       Routines for fields GF(p), p prime.
*/
#include "tzcrypto.h"
#include "bigdefs.h"
#include "fieldpriv.h"
#include "mprand.h"


static CRYPTO_BOOL Kprime_adder
        (const digit_t      *f1,           // IN
         const digit_t      *f2,           // IN
         digit_t       *f3,           // OUT
         const CRYPTO_DWORD      nelmt,        // IN
         const field_desc_t *fdesc,        // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    
    if (OK) {
        CRYPTO_DWORD i;
        const CRYPTO_DWORD elng = fdesc->elng;

        for (i = 0; OK && i != nelmt*elng; i += elng) {
            OK = OK && add_mod(f1 + i, f2 + i, f3 + i,
                               fdesc->modulo->modulus, fdesc->modulo->length);
        }
    }
    return OK;
}  // Kprime_adder


static CRYPTO_BOOL Kprime_exponentiator1
        (const digit_t      *f1,               // IN
         const digit_t      *exponent,         // IN
         const CRYPTO_DWORD      lng_exponent,     // IN
         digit_t       *f2,               // OUT
         const field_desc_t *fdesc,            // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    
    OK = OK && mod_exp(f1, exponent, lng_exponent, f2,
                       fdesc->modulo, f_pBigCtx);
    return OK;
} // Kprime_exponentiator1


static CRYPTO_BOOL Kprime_freer
        (field_desc_t *fdesc,            // IN/OUT
        struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;

    if (    OK
         && fdesc->free_modulus
         && fdesc->modulo != NULL) {
         uncreate_modulus((mp_modulus_t*)fdesc->modulo, f_pBigCtx);
                                             // Remove const attribute
        Free_Temporaries((mp_modulus_t*)fdesc->modulo);
        fdesc->free_modulus = FALSE;
        fdesc->modulo = NULL;
    }
    return OK;
} // Kprime_freer



static CRYPTO_BOOL Kprime_immediater
        (const sdigit_t *scalars,               // IN
         digit_t   *f3,                    // OUT
         const CRYPTO_DWORD  nelmt,                 // IN
         const field_desc_t *fdesc,             // OUT
         struct bigctx_t *f_pBigCtx)
/*
       Convert a signed digit_t value
       (usually a 32-bit signed integer)
       to a field element.

       N.B.  fdesc->one is available to convert a 1.
*/

{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD i;
    const CRYPTO_DWORD elng = fdesc->elng;

    for (i = 0; OK && i != nelmt; i++) {
        const sdigit_t scalar = scalars[i];
        const digit_t abssc = (digit_t)(scalar >= 0 ? scalar : -scalar);
        digit_t* f3addr = f3 + i*elng;

        OK = OK && to_modular(&abssc, 1, f3addr, fdesc->modulo, f_pBigCtx);
        if (scalar < 0) {
            OK = OK && neg_mod(f3addr, f3addr, fdesc->modulo->modulus, fdesc->modulo->length);
        }
    }
    return OK;
} // end Kprime_immediater


static CRYPTO_BOOL Kprime_inverter1
        (const digit_t           *f1,        // IN
         digit_t            *f3,        // OUT
         const field_desc_t      *fdesc,     // IN
         const digit_tempinfo_t  *tempinfo,  // IN
         struct bigctx_t *f_pBigCtx)
//
//     Invert one field element f3 = 1/f1.
//     f1 is guaranteed nonzero.

{
    const CRYPTO_DWORD elng = fdesc->elng;
    digit_t *ftemp = tempinfo->address;   // Length elng
    digit_t *ftemps = ftemp + elng;       // Length MAX(fdesc->ndigtemps_mul,
                                          //            mp_invert_ntemps(elng))
    const digit_t *finp = f1;   // Tentative input location
    const mp_modulus_t *modulo = fdesc->modulo;
    CRYPTO_BOOL OK = TRUE;

    DRMASSERT (   tempinfo->address + tempinfo->nelmt
            == ftemps + MAX(fdesc->ndigtemps_mul,
                            mp_invert_ntemps(elng, f_pBigCtx)));

    if (OK && modulo->reddir == FROM_RIGHT) {
        OK = OK && Kmul(finp, fdesc->inverse_adjustment,
                        ftemp, fdesc, ftemps, f_pBigCtx);
        finp = ftemp;
    }
    OK = OK && mp_invert(finp, modulo->modulus, elng,
                        f3, (CRYPTO_CHAR *)"", ftemps, f_pBigCtx);
    return OK;
} // end Kprime_inverter1



static CRYPTO_BOOL Kprime_mulpower2er
        (const digit_t      *f1,        // IN
         const CRYPTO_INT      ishift,    // IN
         digit_t       *f3,        // OUT
         const CRYPTO_DWORD      nelmt,     // IN
         const field_desc_t *fdesc,     // IN
         struct bigctx_t *f_pBigCtx)
/*
      Multiply field elements in f1 by 2^ishift,
      store results in f3.
*/
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_DWORD i;

    for (i = 0; i != nelmt*elng; i += elng) {
        OK = OK && mod_shift(f1 + i, ishift, f3 + i,
                             fdesc->modulo);
    }
    return OK;
}  // end Kprime_mulpower2er


static CRYPTO_BOOL Kprime_multiplier1
        (const digit_t      *f1,     // IN
         const digit_t      *f2,     // IN
         digit_t       *f3,     // OUT
         const field_desc_t *fdesc,  // IN
         digit_t       *temps,  // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;

    OK = OK && mod_mul(f1, f2, f3, fdesc->modulo, temps, f_pBigCtx);
    return OK;
} // end Kprime_multiplier1


static CRYPTO_BOOL Kprime_negater
        (const digit_t      *f1,        // IN
         digit_t       *f3,        // OUT
         const CRYPTO_DWORD      nelmt,     // IN
         const field_desc_t *fdesc,     // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;

    if (OK) {
        CRYPTO_DWORD i;
        const CRYPTO_DWORD elng = fdesc->elng;

        for (i = 0; OK && i != nelmt*elng; i += elng) {
            OK = OK && neg_mod(f1 + i, f3 + i, fdesc->modulo->modulus, fdesc->modulo->length);            
        }  // for i
    }
    return OK;
}  // Kprime_negater


static CRYPTO_BOOL Kprime_randomizer
        (digit_t       *f3,         // OUT
         const CRYPTO_DWORD      nelmt,      // IN
         const field_desc_t *fdesc,      // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_DWORD i;
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;

    for (i = 0; OK && i != nelmt*elng; i += elng) {
        OK = OK && random_mod(fdesc->modulo->modulus,
                              f3 + i, elng);
    }  // for i
    return OK;
}  // Kprime_randomizer


static CRYPTO_BOOL Kprime_sizer
        (digit_t       *size,    // OUT
         const field_desc_t *fdesc,   // IN
         struct bigctx_t *f_pBigCtx)
/*
     Return field size (length elng+1).
     For prime fields, this is the same as its characteristic.
*/
{
    const CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL OK = TRUE;

    DRMASSERT(elng == fdesc->modulo->length);

    mp_extend(fdesc->modulo->modulus, elng, size, elng + 1);

    return OK;
}  // end Kprime_sizer

CRYPTO_BOOL Kprime_sqrter
        (const digit_t *f1,        // IN
         digit_t       *f3,        // OUT
         const CRYPTO_DWORD      nelmt,     // IN
         const field_desc_t *fdesc,     // IN
         CRYPTO_BOOL      *psquares,  // OUT
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD i;
    CRYPTO_DWORD elng = fdesc->elng;
    CRYPTO_BOOL all_squares = TRUE;

    for (i = 0; OK && i != nelmt*elng; i += elng) {
        CRYPTO_BOOL square_now = FALSE;
        OK = OK && mod_sqrt(f1 + i, f3 + i,
                            fdesc->modulo, &square_now, f_pBigCtx);
        all_squares = all_squares && square_now;
    }
    if (OK) *psquares = all_squares;
    return OK;
}  // Kprime_sqrter

static CRYPTO_BOOL Kprime_subtracter
        (const digit_t      *f1,     // IN
         const digit_t      *f2,     // IN
         digit_t       *f3,     // OUT
         const CRYPTO_DWORD      nelmt,  // IN
         const field_desc_t *fdesc,  // IN
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD i;
    const CRYPTO_DWORD elng = fdesc->elng;

    for (i = 0; OK && i != nelmt*elng; i += elng) {
        OK = OK && sub_mod(f1 + i, f2 + i, f3 + i, fdesc->modulo->modulus, fdesc->modulo->length);
    }
    return OK;
}  // Kprime_subtracter


#pragma prefast(suppress:22112) /* security mitigation warning */
extern const field_arithmetic_t prime_arithmetic = 
{
    Kprime_adder,
    Kequaler_default,
    Kprime_freer,
    Kprime_immediater,
    Kprime_inverter1,
    Kiszeroer_default,
    Kprime_mulpower2er,
    Kprime_multiplier1,
    Kprime_negater,
    Kprime_sizer,    
    Kprime_subtracter,
    Kzeroizer_default
};



CRYPTO_BOOL Kinitialize_prime
        (const mp_modulus_t *modulo,     // IN
         field_desc_t  *fdesc,      // OUT
         struct bigctx_t      *pbigctxTemp,
         struct bigctx_t *f_pBigCtx)
/*
        Initialize a field descriptor for a prime field.

        CAUTION -- The modulo struct must remain accessible
        (i.e., in scope) as long as the field is being accessed.
        That is, don't use an automatic variable in a function
        for *modulo if the field will be accessed after exiting the function.

        CAUTION -- modulo->modulus should be prime, but we don't check that.
*/

{
    const CRYPTO_DWORD elng = modulo->length;
    const CRYPTO_DWORD ninv_temps = mp_invert_ntemps(elng, pbigctxTemp);
    CRYPTO_BOOL OK = TRUE;

    fdesc->deallocate = NULL;
    OK = OK && Kfdesc_initialize(fdesc, 0, pbigctxTemp);

    fdesc->arithmetic = &prime_arithmetic;
    fdesc->degree = 1;
    fdesc->elng = elng;
    fdesc->ftype = FIELD_Q_MP;
    fdesc->modulo = modulo;
    fdesc->ndigtemps_mul = modulo->modmul_algorithm_temps;
    fdesc->ndigtemps_invert1 = elng + MAX(fdesc->ndigtemps_mul, ninv_temps);
    fdesc->ndigtemps_arith = fdesc->ndigtemps_invert1 + elng;
    fdesc->one = (digit_t*)modulo->one;  // Remove const attribute
          // Set up multiplier for Kinvert.
          // This is 1 for FROM_LEFT arithmetic.
          // For FROM_RIGHT arithmetic, we twice unscale the
          // constant fdesc->one.
    if (!OK) {
    } else if (modulo->reddir == FROM_LEFT) {
        fdesc->inverse_adjustment = fdesc->one;
    } else {
        digit_t *invadj = digit_allocate(elng, (CRYPTO_CHAR *)"Kinitialize_prime");
        if (invadj == NULL) {
            OK = FALSE;
        }
        fdesc->deallocate = invadj;
        fdesc->inverse_adjustment = invadj;
        OK = OK && mod_shift(fdesc->one, -2*modulo->scaling_power2,
                             invadj, modulo);
        if (!OK && invadj != NULL) {
            Free_Temporaries(invadj);
        }
    }
    return OK;
} /* Kinitialize_prime */


