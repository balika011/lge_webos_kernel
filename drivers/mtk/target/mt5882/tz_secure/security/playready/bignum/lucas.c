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
//     mod_LucasUV(C1, C0, exponent, lngexpon, Uout, Vout, modulo) --
//              Compute
//
//                  Vout = alpha^exponent + beta^exponent
//                  Uout = (alpha^exponent - beta^exponent)/(alpha - beta)
//
//              where alpha, beta satisfy X^2 - C1*X + C0 = 0 (mod modulo).
//              Modulus must be odd.
//
//                  Vout[0] = 2     Vout[1] = C1
//                  Uout[0] = 1     Uout[1] = 1
//                  Vout[n+2] = C1*Vout[n+1] - C0*Vout[n]
//                  Uout[n+2] = C1*Uout[n+1] - C0*Uout[n]

*/
/***************************************************************************/
CRYPTO_BOOL mod_LucasUV
        (const digit_t C1[],
         const digit_t C0[],
         const digit_t exponent[],
         const CRYPTO_DWORD   lngexpon,
         digit_t  Uout[],
         digit_t  Vout[],
         const mp_modulus_t *modulo,
         struct bigctx_t *f_pBigCtx)
{
/*
       Vout = alpha^exponent + beta^exponent
       Uout = (alpha^exponent - beta^exponent)/(alpha - beta)

    where alpha, beta satisfy X^2 - C1*X + C0 = 0 (mod modulo).
    Modulus must be odd.
*/

    CRYPTO_BOOL OK = TRUE;
    CRYPTO_DWORD bits_left = mp_significant_bit_count(exponent, lngexpon);
    const CRYPTO_DWORD lng = modulo->length;

    /* Check for integer underflow/overflow */
    if (2*lng + modulo->modmul_algorithm_temps < 2*lng)
    {
        OK = FALSE;
    }
    else
    {
        digit_t *dtemps = digit_allocate(2*lng + modulo->modmul_algorithm_temps,
                                         (CRYPTO_CHAR *)"mod_LucasUV");
        if (dtemps == NULL) {
            OK = FALSE;
        } else if (bits_left == 0) {       /* If exponent is identically zero  */
            OK = OK && add_mod(modulo->one, modulo->one, Vout,
                               modulo->modulus, modulo->length);  // V = 2
            memset(Uout, 0, ( lng)*sizeof( digit_t ));                           // U = 0
        } else {
            digit_t *DELTA = dtemps;        // Length lng
            digit_t *temp1 = dtemps + lng;  // Length lng
            digit_t *modmultemps = dtemps + 2*lng;
                                    // Length modulo->modmul_algorithm_temps

            memcpy( Vout,C1,( lng)*sizeof( digit_t ));                         // V = C1
            memcpy( Uout,modulo->one,( lng)*sizeof( digit_t ));                // U = 1
            OK = OK && mod_shift(C0, +2, temp1, modulo);    // 4*C0
            OK = OK && mod_mul(C1, C1, DELTA, modulo,
                              modmultemps, f_pBigCtx);                 // C1*C1
            OK = OK && sub_mod(DELTA, temp1, DELTA, modulo->modulus, modulo->length);            
                                                             // DELTA = C1*C1 - 4*C0

            bits_left--;
            while (OK && bits_left != 0) {
                bits_left--;
                    // First replace n by 2*n
                OK = OK && mod_mul(Uout, Uout, temp1, modulo,
                                   modmultemps, f_pBigCtx);
                OK = OK && mod_mul(temp1, DELTA, temp1, modulo,
                                   modmultemps, f_pBigCtx);
                                                        // DELTA * Uout^2
                OK = OK && mod_mul(Uout, Vout, Uout, modulo,
                                   modmultemps, f_pBigCtx);
                                                        // Uout * Vout
                OK = OK && mod_mul(Vout, Vout, Vout, modulo,
                                   modmultemps, f_pBigCtx);
                                                        // Vout^2
                OK = OK && add_mod(Vout, temp1, Vout, modulo->modulus, modulo->length );
                                                        // Vout^2 + DELTA*Uout^2
                OK = OK && mod_shift(Vout, -1, Vout, modulo);
                                                        // (Vout^2 + DELTA*Uout^2)/2

                if (mp_getbit(exponent, bits_left) != 0) {

                    // Replace 2*n by 2*n + 1
                    // (U, V) := (C1*U + V, C1*V + DELTA*U)/2

                    OK = OK && mod_mul(DELTA, Uout, temp1, modulo,
                                       modmultemps, f_pBigCtx);
                    OK = OK && mod_mul(C1, Uout, Uout, modulo,
                                       modmultemps, f_pBigCtx);
                    OK = OK && add_mod(Uout, Vout, Uout, modulo->modulus, modulo->length);
                    OK = OK && mod_shift(Uout, -1, Uout, modulo);

                    OK = OK && mod_mul(C1, Vout, Vout, modulo,
                                       modmultemps, f_pBigCtx);
                    OK = OK && add_mod(Vout, temp1, Vout, modulo->modulus, modulo->length);
                    OK = OK && mod_shift(Vout, -1, Vout, modulo);
                }
            } /* while */
        }
        if (dtemps != NULL) {
            Free_Temporaries(dtemps);
        }
    }
    return OK;
} /* mod_LucasUV */

