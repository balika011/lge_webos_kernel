/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "tzcrypto.h"
#include "bigpriv.h"


/*
       File modmulch1.c.   Version 1 October, 2002.

            This is the first of several modmulchx files with algorithms for
        modular multiplication, both FROM_LEFT and FROM_RIGHT.
        All procedures have an argument list

            (const digit_t *a, *b,           // Two numbers to multiply
                                        // 0 <= a, b < modulus
             digit_t  *c                // Product.  May overlap a or b.
             const mp_modulus_t *pmodulo,    // Struct with information about modulus
             digit_t  *temps,           // Temporaries (length
                                        //     pmodulo->modmul_algorithm_temps)
             struct bigctx_t *f_pBigCtx)

            FROM_LEFT codes return

                c == (a*b) mod modulus

            FROM_RIGHT codes return the Montgomery product

               c == a*b / RADIX^lng  (mod pmodulo->modulus).

        where lng = pmodulo->length.

            This file contains

                modmul_from_left_default
                modmul_from_right_default

        which work on all architectures and lengths.
*/
/******************************************************************************/
static CRYPTO_BOOL modmul_from_left_default
        (const digit_t *a,              // IN
         const digit_t *b,              // IN
         digit_t  *c,              // OUT
         const mp_modulus_t *pmodulo,   // IN
         digit_t  *temps )          // TEMPORARIES, at least 2*lng         
/*
        This implements ordinary modular multiplication.
    Given inputs a, b with 0 <= a, b < modulus < RADIX^lng,
    and where lng > 0, we form

           c == (a*b) mod modulus.
*/
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD lng = pmodulo->length;
    digit_t *temp1 = temps;     // Length 2*lng

    DRMASSERT (pmodulo->modmul_algorithm_temps == 2*lng);
    OK = OK && multiply(a, lng, b, lng, temp1);     // Double-length product
    OK = OK && divide(temp1, 2*lng, pmodulo->modulus, lng,
                    &pmodulo->left_reciprocal_1, NULL, c);
    return OK;
}  // modmul_from_left_default
/******************************************************************************/


/*
**  General case for mutiplication of arbirtrary long integers 
*/



CRYPTO_BOOL modmul_from_right_default( 
            const digit_t      *a,
            const digit_t      *b,
            digit_t      *c,
            const mp_modulus_t *pmodulo,   
            digit_t      *temps )    // TEMPORARIES, at least 2*lng    
/*
        This implements Montgomery (FROM_RIGHT) multiplication.     
    Let lng = pmodulo->length > 0.
    Given inputs a, b with 0 <= a, b < pmodulo->modulus < RADIX^lng, we form

           c == a*b / RADIX^lng  (mod pmodulo->modulus).

        At the start of the loop on i, there exists templow
    (formed by the discarded values of
    CRYPTO_UI64Low32(prod1) = CRYPTO_UI64Low32(prod2)) such that

               0 <= temp1, temp2 < modulus
               temp1*RADIX^j + templow = b[0:j-1] * a
               temp2*RADIX^j + templow == 0 (mod modulus)

    When j = lng, we exit with c == temp1 - temp2 (mod modulus)
*/
{
    CRYPTO_BOOL OK = TRUE;
    const CRYPTO_DWORD lng = pmodulo->length;
    digit_t *temp1 = temps, *temp2 = temps + lng;   // Both length lng
    const digit_t *modulus = pmodulo->modulus;
    const digit_t minv = pmodulo->right_reciprocal_1;
    const digit_t minva0 = minv*a[0];    // mod RADIX
    CRYPTO_DWORD i, j;
    digit_t carry1, carry2, mul1, mul2;
    CRYPTO_UINT64 carryfull;
    CRYPTO_UINT64 prod1, prod2;

    DRMASSERT (pmodulo->modmul_algorithm_temps == 2*lng);
 // Case j = 0 of main loop, with temp1 = temp2 = 0 beforehand.
    mul1 = b[0];
    mul2 = minva0*mul1;   // Modulo RADIX
    
    carryfull = DPRODUU(mul1, a[0]); 
    carry1 = CRYPTO_UI64High32(carryfull);
    
    carryfull = DPRODUU(mul2, modulus[0]);
    carry2 = CRYPTO_UI64High32( carryfull );
    
    DRMASSERT (mul1*a[0] == mul2*modulus[0]);     // mod RADIX

    for (i = 1; i != lng; i++) {
        prod1 = MULTIPLY_ADD1(mul1,       a[i], carry1);
        prod2 = MULTIPLY_ADD1(mul2, modulus[i], carry2);
        temp1[i-1] = CRYPTO_UI64Low32(prod1);
        temp2[i-1] = CRYPTO_UI64Low32(prod2);
        carry1 = CRYPTO_UI64High32(prod1);
        carry2 = CRYPTO_UI64High32(prod2);
    }
    temp1[lng-1] = carry1;
    temp2[lng-1] = carry2;

    for (j = 1; j != lng; j++) {
        mul1 = b[j];
        mul2 = minva0*mul1 + minv*(temp1[0] - temp2[0]);  // Modulo RADIX
        prod1 = MULTIPLY_ADD1(mul1, a[0], temp1[0]);
        prod2 = MULTIPLY_ADD1(mul2, modulus[0], temp2[0]);

        // Replace temp1 by (temp1 + b[j]*a - CRYPTO_UI64Low32(prod1))/RADIX
        // Replace temp2 by (temp2 + mul2*modulus - CRYPTO_UI64Low32(prod2))/RADIX

        DRMASSERT (CRYPTO_UI64Low32(prod1) == CRYPTO_UI64Low32(prod2));

        carry1 = CRYPTO_UI64High32(prod1);
        carry2 = CRYPTO_UI64High32(prod2);

        for (i = 1; i != lng; i++) {
            prod1 = MULTIPLY_ADD2(mul1,       a[i], temp1[i], carry1);
            prod2 = MULTIPLY_ADD2(mul2, modulus[i], temp2[i], carry2);
            temp1[i-1] = CRYPTO_UI64Low32(prod1);
            temp2[i-1] = CRYPTO_UI64Low32(prod2);
            carry1 = CRYPTO_UI64High32(prod1);
            carry2 = CRYPTO_UI64High32(prod2);
        }
        temp1[lng-1] = carry1;
        temp2[lng-1] = carry2;
    }
    OK = OK && sub_mod(temp1, temp2, c, modulus, lng);
    return OK;
} // modmul_from_right_default;

 
/**********************************************************************
** modmul_from_right_default_modulo5 when the size of big integer is 160 bits 
** Performance improved due to access to local temporary array 
** and loop unrolling.
***********************************************************************/

static CRYPTO_BOOL modmul_from_right_default_modulo5( 
            const digit_t      *a,
            const digit_t      *b,
            digit_t      *c,
            const mp_modulus_t *pmodulo,   
            digit_t      *temps )    // TEMPORARIES, at least 2*lng    
/*
**     This is exactly the same as modmul_from_right_default
**     in the case of pmodulo->length equal to 5
*/
{
    CRYPTO_BOOL OK = TRUE;   
    digit_t temp1[5], temp2[5];   // Both length LNGQ_MODULO_5
    const digit_t *modulus = pmodulo->modulus;
    const digit_t minv = pmodulo->right_reciprocal_1;
    const digit_t minva0 = minv*a[0];    // mod RADIX
    CRYPTO_DWORD j;
    CRYPTO_UINT64 carry1full, carry2full;
    digit_t mul1, mul2;
    CRYPTO_UINT64 prod1, prod2;

    DRMASSERT (pmodulo->modmul_algorithm_temps == 2*5);

 // Case j = 0 of main loop, with temp1 = temp2 = 0 beforehand.
    mul1 = b[0];
    mul2 = minva0*mul1;   // Modulo RADIX
    carry1full = DPRODUU(mul1, a[0]      );
    carry2full = DPRODUU(mul2, modulus[0]);

    DRMASSERT (mul1*a[0] == mul2*modulus[0]);     // mod RADIX
    DRMASSERT ( pmodulo->length == 5 );

    prod1 = MULTIPLY_ADD1(mul1,       a[1], CRYPTO_UI64High32(carry1full));
    prod2 = MULTIPLY_ADD1(mul2, modulus[1], CRYPTO_UI64High32(carry2full));
    temp1[0] = CRYPTO_UI64Low32(prod1);
    temp2[0] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[2], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[2], CRYPTO_UI64High32(prod2));
    temp1[1] = CRYPTO_UI64Low32(prod1);
    temp2[1] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[3], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[3], CRYPTO_UI64High32(prod2));
    temp1[2] = CRYPTO_UI64Low32(prod1);
    temp2[2] = CRYPTO_UI64Low32(prod2);

    prod1 = MULTIPLY_ADD1(mul1,       a[4], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[4], CRYPTO_UI64High32(prod2));
    temp1[3] = CRYPTO_UI64Low32(prod1);
    temp2[3] = CRYPTO_UI64Low32(prod2);
    
    temp1[4] = CRYPTO_UI64High32(prod1);
    temp2[4] = CRYPTO_UI64High32(prod2);

    for (j = 1; j != 5; j++) {
        mul1 = b[j];
        mul2 = minva0*mul1 + minv*(temp1[0] - temp2[0]);  // Modulo RADIX
        prod1 = MULTIPLY_ADD1(mul1, a[0], temp1[0]);
        prod2 = MULTIPLY_ADD1(mul2, modulus[0], temp2[0]);

        // Replace temp1 by (temp1 + b[j]*a - CRYPTO_UI64Low32(prod1))/RADIX
        // Replace temp2 by (temp2 + mul2*modulus - CRYPTO_UI64Low32(prod2))/RADIX

        DRMASSERT (CRYPTO_UI64Low32(prod1) == CRYPTO_UI64Low32(prod2));

        prod1 = MULTIPLY_ADD2(mul1,       a[1], temp1[1], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[1], temp2[1], CRYPTO_UI64High32(prod2));
        temp1[0] = CRYPTO_UI64Low32(prod1);
        temp2[0] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[2], temp1[2], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[2], temp2[2], CRYPTO_UI64High32(prod2));
        temp1[1] = CRYPTO_UI64Low32(prod1);
        temp2[1] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[3], temp1[3], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[3], temp2[3], CRYPTO_UI64High32(prod2));
        temp1[2] = CRYPTO_UI64Low32(prod1);
        temp2[2] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[4], temp1[4], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[4], temp2[4], CRYPTO_UI64High32(prod2));
        temp1[3] = CRYPTO_UI64Low32(prod1);
        temp2[3] = CRYPTO_UI64Low32(prod2);
        
        temp1[4] = CRYPTO_UI64High32(prod1);
        temp2[4] = CRYPTO_UI64High32(prod2);
        
    }
    OK = OK && sub_mod(temp1, temp2, c, modulus, 5);
    return OK;
} // modmul_from_right_default_modulo5;

/**********************************************************************
** modmul_from_right_default_modulo8 when the size of big integer is 256 bits 
** Performance improved due to access to local temporary array 
** and loop unrolling.
***********************************************************************/

CRYPTO_BOOL modmul_from_right_default_modulo8( 
            const digit_t      *a,
            const digit_t      *b,
            digit_t      *c,
            const mp_modulus_t *pmodulo,   
            digit_t      *temps )    // TEMPORARIES, at least 2*lng    
/*
**     This is exactly the same as modmul_from_right_default
**     in the case of pmodulo->length equal to 8
*/
{
    CRYPTO_BOOL OK = TRUE;   
    digit_t temp1[8], temp2[8];   // Both length 8
    const digit_t *modulus = pmodulo->modulus;
    const digit_t minv = pmodulo->right_reciprocal_1;
    const digit_t minva0 = minv*a[0];    // mod RADIX
    CRYPTO_DWORD j;
    CRYPTO_UINT64 carry1full, carry2full;
    digit_t mul1, mul2;
    CRYPTO_UINT64 prod1, prod2;

    DRMASSERT (pmodulo->modmul_algorithm_temps == 2*8);

 // Case j = 0 of main loop, with temp1 = temp2 = 0 beforehand.
    mul1 = b[0];
    mul2 = minva0*mul1;   // Modulo RADIX
    carry1full = DPRODUU(mul1, a[0]      );
    carry2full = DPRODUU(mul2, modulus[0]);

    DRMASSERT (mul1*a[0] == mul2*modulus[0]);     // mod RADIX
    DRMASSERT ( pmodulo->length == 8 );

    prod1 = MULTIPLY_ADD1(mul1,       a[1], CRYPTO_UI64High32(carry1full));
    prod2 = MULTIPLY_ADD1(mul2, modulus[1], CRYPTO_UI64High32(carry2full));
    temp1[0] = CRYPTO_UI64Low32(prod1);
    temp2[0] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[2], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[2], CRYPTO_UI64High32(prod2));
    temp1[1] = CRYPTO_UI64Low32(prod1);
    temp2[1] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[3], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[3], CRYPTO_UI64High32(prod2));
    temp1[2] = CRYPTO_UI64Low32(prod1);
    temp2[2] = CRYPTO_UI64Low32(prod2);

    prod1 = MULTIPLY_ADD1(mul1,       a[4], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[4], CRYPTO_UI64High32(prod2));
    temp1[3] = CRYPTO_UI64Low32(prod1);
    temp2[3] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[5], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[5], CRYPTO_UI64High32(prod2));
    temp1[4] = CRYPTO_UI64Low32(prod1);
    temp2[4] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[6], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[6], CRYPTO_UI64High32(prod2));
    temp1[5] = CRYPTO_UI64Low32(prod1);
    temp2[5] = CRYPTO_UI64Low32(prod2);
    
    prod1 = MULTIPLY_ADD1(mul1,       a[7], CRYPTO_UI64High32(prod1));
    prod2 = MULTIPLY_ADD1(mul2, modulus[7], CRYPTO_UI64High32(prod2));
    temp1[6] = CRYPTO_UI64Low32(prod1);
    temp2[6] = CRYPTO_UI64Low32(prod2);


    temp1[7] = CRYPTO_UI64High32(prod1);
    temp2[7] = CRYPTO_UI64High32(prod2);

    for (j = 1; j != 8; j++) {
        mul1 = b[j];
        mul2 = minva0*mul1 + minv*(temp1[0] - temp2[0]);  // Modulo RADIX
        prod1 = MULTIPLY_ADD1(mul1, a[0], temp1[0]);
        prod2 = MULTIPLY_ADD1(mul2, modulus[0], temp2[0]);

        // Replace temp1 by (temp1 + b[j]*a - CRYPTO_UI64Low32(prod1))/RADIX
        // Replace temp2 by (temp2 + mul2*modulus - CRYPTO_UI64Low32(prod2))/RADIX

        DRMASSERT (CRYPTO_UI64Low32(prod1) == CRYPTO_UI64Low32(prod2));

        prod1 = MULTIPLY_ADD2(mul1,       a[1], temp1[1], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[1], temp2[1], CRYPTO_UI64High32(prod2));
        temp1[0] = CRYPTO_UI64Low32(prod1);
        temp2[0] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[2], temp1[2], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[2], temp2[2], CRYPTO_UI64High32(prod2));
        temp1[1] = CRYPTO_UI64Low32(prod1);
        temp2[1] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[3], temp1[3], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[3], temp2[3], CRYPTO_UI64High32(prod2));
        temp1[2] = CRYPTO_UI64Low32(prod1);
        temp2[2] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[4], temp1[4], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[4], temp2[4], CRYPTO_UI64High32(prod2));
        temp1[3] = CRYPTO_UI64Low32(prod1);
        temp2[3] = CRYPTO_UI64Low32(prod2);
        

        prod1 = MULTIPLY_ADD2(mul1,       a[5], temp1[5], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[5], temp2[5], CRYPTO_UI64High32(prod2));
        temp1[4] = CRYPTO_UI64Low32(prod1);
        temp2[4] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[6], temp1[6], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[6], temp2[6], CRYPTO_UI64High32(prod2));
        temp1[5] = CRYPTO_UI64Low32(prod1);
        temp2[5] = CRYPTO_UI64Low32(prod2);
        
        prod1 = MULTIPLY_ADD2(mul1,       a[7], temp1[7], CRYPTO_UI64High32(prod1));
        prod2 = MULTIPLY_ADD2(mul2, modulus[7], temp2[7], CRYPTO_UI64High32(prod2));
        temp1[6] = CRYPTO_UI64Low32(prod1);
        temp2[6] = CRYPTO_UI64Low32(prod2);
        
        
        temp1[7] = CRYPTO_UI64High32(prod1);
        temp2[7] = CRYPTO_UI64High32(prod2);
        
    }
    OK = OK && sub_mod(temp1, temp2, c, modulus, 8);
    return OK;
} // modmul_from_right_default_modulo8;

/******************************************************************************/
CRYPTO_BOOL modmul_choices1
        (mp_modulus_t *pmodulo,        // IN/OUT
         CRYPTO_INT          *pindex )         // IN/OUT         
/*
            This procedure (and any other modmul_choicesX ones it calls)
        looks for a suitable modmul_algorithm.  The selection algorithm
        can look at:

              1)  The target architecture (if there is a known target with assembly available)
              2)  Instruction-availability flags
              3)  Fields in the pmodulo struct, such as the length and the
                  FROM_LEFT/FROM_RIGHT flag.  A Karatsuba routine might
                  accept only moduli beyond a specific length.
                  Another routine might accept only Mersenne prime moduli.

            Each modmul_algorithm should be able to do both squarings and
        general multiplications.  It might branch to separate squaring code,
        but that check will be made when the routine is called, not now.

            If *pindex = 5 (say) on entry, then the fifth qualifying routine
        will be used.  During testing, the test routine calls this once
        with *pindex = 0, ending with *pindex = -k where k is the number
        of routines accepting this modulus.  Then it makes one call each with
        *pindex = 1, 2, ..., k, checking that all k procedures
        give consistent results (and perhaps comparing their timings).
        During production, the second call uses *pindex = k, and the
        final qualifying one is chosen.  For this reason, faster codes
        (typically assembly language versions) should appear last.

            Each routine accepting the modulus should save its address,
        its name, and the number of digit_t temporaries it will need.
*/
{
    CRYPTO_BOOL OK = TRUE;
    CRYPTO_INT index = *pindex;
    const CRYPTO_DWORD lng = pmodulo->length;

    if (pmodulo->reddir == FROM_LEFT) {
        index--;
        if (index >= 0) {
            pmodulo->modmul_algorithm = modmul_from_left_default;            
            pmodulo->modmul_algorithm_temps = 2*lng;
        }
    } else if (pmodulo->reddir == FROM_RIGHT) {
        index--;
        if (index >= 0) 
        { /*  If the lenght of the modulo is 5, then we have special 
          **  function that optimized for 5.
          **  It is used in ECC 160 algorithms.
          **  RSA uses lenght of 16, 17 and 32. 
          **  Generic implementation for these cases is modmul_from_right_default
          */
            
            if ( pmodulo->length == 5 )
            {
                pmodulo->modmul_algorithm = modmul_from_right_default_modulo5;
            }
            else if ( pmodulo->length == 8 )
            {   
                pmodulo->modmul_algorithm = modmul_from_right_default_modulo8;
            }
            else 
            {   
                pmodulo->modmul_algorithm = modmul_from_right_default;
            }

            pmodulo->modmul_algorithm_temps = 2*lng;
        }
    } 
    *pindex = index;
    return OK;
} // end modmul_choices1

