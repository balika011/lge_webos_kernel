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
        File modexp.c.  Modular_exponentiation routines.    

        CRYPTO_BOOL mod_exp(base, exponent, lngexpon,
                        answer, modulo)
                     -- Compute answer = base^exponent mod modulo->modulus.                        
                        The 2000 version of the code stores fewer
                        Karatsuba images, to reduce memory requirements
                        and hopefully improve cache utilization.
        
            Function mod_exp (modular exponentiation)
        computes b^e (modulo something).
        Here b and e are multiple precision, typically
        from 512 to 2048 bits.  We strive hard to keep
        the number of multiplications low.

            For a treatise on methods of exponentiation,
        see Section 4.6.3 of Donald E. Knuth, The Art of
        Computer Programming, Volume 2, Seminumerical Algorithms,
        2nd Edition, 1981.

            The classical left-to-right binary method of exponentiation
        (Knuth, p. 441) looks at one bit of the exponent at a
        time, starting from the most significant bit.
        It requires  log2(e) + (number of 1's in e) - 1
        multiplications (or squarings), which averages about
        (3/2)*log2(e) multiplications for a random exponent
        with log2(e)+1 bits.

            When e is large, this cost can be reduced by using
        radix 2^k for suitable k (Knuth, p. 446).
        The cost is 2^k - 2 multiplications to build a table with
        b^0 = 1,  b^1 = b,  b^2,  b^3,  ... ,  b^(2^k - 1).
        After this table is built, there will be about log2(e)/k steps
        consisting of k squarings and a multiplication by a table entry.
        The final multiply can be skipped when the table entry is
        b^0 = 1, which occurs with probability about 2^(-k).
        The approximate cost of the radix 2^k method (Knuth, p. 451) is

                              log2(e)           log2(e)
           2^k + (k+1-2^(-k)) -------  =  2^k + -------(1 - 2^(-k)) + log2(e)
                                 k                 k

            mod_exp1995 (no longer here) usea a modified method with radix
        2^k and cost about

                         log2(e)
(*)            2^(k-1) + -------  + log2(e).
                           k+1

        It resembled that in ``Fast square-and-multiply
        exponentiation for RSA'' by L. C. K. Hui and K.-Y. Lam,
        Electronics Letters, 18 August 1994, Volume 30, No, 17,
        pp. 1396-1397, Electronics Letters Online No. 19940949.

            The upcoming presentation assumes k = 6.
        A recursive formulation of the mod_exp1995 algorithm resembles

                if e is small then
                    Use table look-up for b^e
                else if e is even
                    Compute  b^e  as  (b^(e/2))^2
                else
                    Compute  b^e  as  (b^j)^64 * b^i,
                    where e = 64 * j + i and 0 <= i <= 63
                end if

        Except possibly when initializing the recursion
        (i.e., to compute b^e where e is `small'), the algorithm needs
        only the odd powers  b^1,  b^3,  b^5,  ...,  b^63.
        It costs 32 = 2^(k-1) multiplications to compute
        b^2 and this table.

            Half of the time this value of i
        (i.e., where e = 64*j + i) will be in [0, 31].
        In those cases, we can delay the multiplication by b^i
        for one step, merely replacing x by x^2 at this time
        and awaiting another bit of the exponent.
        This explains the denominator (k+1) in (*).

            The optimal k (i.e., the one requiring the fewest
        multiplications) depends upon the exponent e.
        According to (*), the value k is better than k-1 if

                      log2(e)                           log2(e)
            2^(k-1) + -------  + log2(e)  <   2^(k-2) + ------- + log2(e) .
                        k+1                                k

                                           log2(e)
        This is equivalent to  2^(k-2)  <  -------  and to
                                           k (k+1)

        log2(e) > k (k+1) 2^(k-2)


            k         Approximate log2(e)
                      where k first beats k-1

            2            6
            3           24
            4           80
            5          240
            6          672
            7         1680
            8         4608

        For a 1024-bit exponent, this analysis suggests using k = 6
        since 672 <= 1024 <= 1680.  The estimated costs
        for a 1024-bit exponent are

            k         2^(k-1) + 1024/(k + 1) + 1024

            1         1537
            2         1367.3
            3         1284
            4         1236.8
            5         1210.7
            6         1202.3
            7         1216
            8         1265.8

        The estimated costs for k = 5, 6, 7 vary by less than 1.2%.
        The precise costs will depend upon the bit pattern in
        the exponent.  The code tries multiple values of k,
        determining the cost associated with each radix,
        and selects the one with the smallest cost.

        mod_exp uses a window of width 2^k on the exponent.
        There are bucket_max = 2^k - 1 buckets bucket[1]
        to bucket[bucket_max], all initialized to 1.
        The final result is intended to be

           answer = bucket[1]^1 * bucket[2]^2 * bucket[3]^3 * ...
                                * bucket[bucket_max]^bucket_max

        Except during final processing, all even-numbered buckets
        are empty (i.e., contain 1)


*/

#if defined (_WIN32_WCE)
         // Opt for less memory under Windows CE
#define MAX_BUCKET_WIDTH 5
#else
#define MAX_BUCKET_WIDTH 6
#endif


typedef struct {         // Temps and interface for mod_exp and helpers                
        const mp_modulus_t   *modulo;
        CRYPTO_BOOL             bucket_occupied[1L << MAX_BUCKET_WIDTH];
        digit_t         *bucket_location[1L << MAX_BUCKET_WIDTH];
        digit_t         *modmultemps;  // Length modmul_algorithm_temps
    } temps_t;


/***************************************************************************/

/*
      basepower_squaring and bucket_multiply are helpers to mod_exp.

      bucket_multiply multiplies the contents
      of bucket[ibucket] by multiply_by.

      basepower_squaring replaces basepower by basepower^2.

      The temps struct is used to share miscellaneous
      local and malloc-ed data with mod_exp.
*/

static CRYPTO_BOOL basepower_squaring(
        digit_t      *basepower,               // IN/OUT
        temps_t  *temps,                   // IN/OUT
        struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    const mp_modulus_t *modulo = temps->modulo;        
    
    OK = OK && mod_mul(basepower, basepower, basepower,
                           modulo, temps->modmultemps, f_pBigCtx);    
    return OK;
} // end basepower_squaring



static CRYPTO_BOOL bucket_multiply
        (const CRYPTO_DWORD    ibucket,                  // IN
         const digit_t     multiply_by[],            // IN
         temps_t *temps,                    // IN/OUT
         struct bigctx_t *f_pBigCtx)
{
    CRYPTO_BOOL OK = TRUE;
    digit_t       *bloc       = temps->bucket_location[ibucket];
    const mp_modulus_t *modulo     = temps->modulo;
    const CRYPTO_DWORD      elng       = modulo->length;
    
    if (temps->bucket_occupied[ibucket]) {
        
        mod_mul(bloc, multiply_by, bloc, modulo,
                    temps->modmultemps, f_pBigCtx);        
    } else {
        temps->bucket_occupied[ibucket] = TRUE;
    
        memcpy( bloc,multiply_by,( elng)*sizeof( digit_t ));    
    }
    return OK;
} // end bucket_multiply




CRYPTO_BOOL mod_exp
        (const digit_t         base[],
         const digit_t         exponent[],
         const CRYPTO_DWORD        lngexpon,
         digit_t          answer[],
         const mp_modulus_t   *modulo,         
         struct bigctx_t *f_pBigCtx)

{
    CRYPTO_BOOL OK = TRUE;    
    const CRYPTO_DWORD elng = modulo->length;
    const CRYPTO_DWORD exponent_bits_used
             = mp_significant_bit_count(exponent, lngexpon);
    const unsigned short width_cutoffs[] = {6, 24, 80, 240, 672};
                                           // (k + 2)*(k + 3) * 2^k

    temps_t *temps = NULL;       // malloc-ed
    digit_t *basepower = answer;           // Same array used for both
    digit_t *bucket_data = NULL;     // malloc-ed

    CRYPTO_DWORD bucket_mask, bucket_width, carried;
    CRYPTO_DWORD ibit, ibucket, max_bucket, ndoubling;
    CRYPTO_BOOL base2;

    bucket_width = 1;
    while (    bucket_width < MAX_BUCKET_WIDTH
            && (CRYPTO_DWORD)width_cutoffs[bucket_width-1] < exponent_bits_used) {
        bucket_width++;
    }

    OK = OK && validate_modular_data(base, modulo->modulus, elng,
                                    (CRYPTO_CHAR *) "mod_exp");

    bucket_mask = (DIGIT_ONE << bucket_width) - 1;
    max_bucket = bucket_mask;   // Another name for the variable

    Allocate_Temporaries(temps_t, temps);

    if ((elng == 0) ||
        (elng*max_bucket + modulo->modmul_algorithm_temps < elng*max_bucket)) /* check for integer underflow/overlfow */
    {
        bucket_data = NULL;
    }
    else
    {
        bucket_data = digit_allocate(elng*max_bucket + modulo->modmul_algorithm_temps,
                                     (CRYPTO_CHAR *)"mod_exp");
    }
    
    OK = OK && (temps != NULL) && (bucket_data != NULL);

    base2 = FALSE;
    if (OK) {
        memset(temps, 0, sizeof(temps_t));
        temps->modulo = modulo;
        temps->bucket_location[0] = NULL;
        temps->modmultemps = bucket_data + elng*max_bucket;

        //  Check for base = 2.  When base = 2, use alternate algorithm.
        //  (squarings and doublings).

        OK = OK && add_mod(modulo->one, modulo->one,
                           bucket_data, modulo->modulus, modulo->length);
                                                        /* bucket_data = 2 */
        base2 = OK && compare_same(base, bucket_data, elng) == 0;
    }
    if (!OK) {
    } else if (base2 && exponent_bits_used != 0) {
        const CRYPTO_DWORD shift_max = MIN(RADIX_BITS*elng, 1024);
        CRYPTO_DWORD ibit;
        CRYPTO_DWORD high_expon_bits = 0;
        CRYPTO_BOOL high_bits_processed = FALSE;
        digit_t *dtemp = bucket_data; /* Some convenient temporary */

        /*
             Start at the most significant end of the exponent.
             Accumulate the first few bits in high_expon_bits.
             As long as 2^high_expon_bits is about the
             same size as the modulus, or smaller,
             continue this accumulation.

             While processing the lower bits of the exponent,
             do repeated squaring, plus a doubling
             where the exponent bit is 1.

        */
        for (ibit = exponent_bits_used; OK && (ibit--) != 0; /*null*/) {
            const CRYPTO_DWORD iexpbit = (CRYPTO_DWORD)mp_getbit(exponent,ibit);

            if (high_bits_processed) {
            
                OK = OK && mod_mul(dtemp, dtemp, dtemp,
                                   modulo, temps->modmultemps,
                                   f_pBigCtx);
                if (iexpbit != 0) {
                    OK = OK && add_mod(dtemp, dtemp, dtemp,
                                       modulo->modulus, modulo->length);
                }                
            } else {  /* high_bits_processed */
                high_expon_bits = 2*high_expon_bits + iexpbit;
                if (   ibit == 0
                    || 2*high_expon_bits >= shift_max) {

                    high_bits_processed = TRUE;
                    OK = OK && mod_shift(modulo->one,
                              (CRYPTO_INT)high_expon_bits,
                              dtemp, modulo);

                }
            } /* high_bits_processed */
        } /* for ibit */
        temps->bucket_location[1] = dtemp;  /* For copy into answer */

        if (!high_bits_processed) {
            OK = FALSE;
            SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                            "mod_exp -- high_bits_processed = FALSE");
        }
    } else {  /* not base2 */

         // Partition bucket_data array into length-elng
         // pieces for individual buckets.
         // Allocate pieces for odd-numbered buckets
         // contiguously, to lessen cache conflicts.

        for (ibucket = 1; OK && ibucket <= max_bucket; ibucket++) {
            digit_t *bloc = bucket_data
                + elng*(ibucket - 1 + (IS_EVEN(ibucket) ? max_bucket : 0))/2;

            temps->bucket_location[ibucket] = bloc;
            temps->bucket_occupied[ibucket] = FALSE;
            
            memcpy( bloc,modulo->one,( elng)*sizeof( digit_t ));
                                                      // Set bucket contents = 1        
        }  // for ibucket
        
        if (OK) {
            memcpy( basepower,base,( elng)*sizeof( digit_t ));    // basepower = base
        }
        carried = 0;
        ndoubling = 0;

        for (ibit = 0; OK && ibit != exponent_bits_used; ibit++) {
            const digit_t bit_now = mp_getbit(exponent, ibit);

            // Want bucket[1]^1 * ... * bucket[max_bucket]^max_bucket
            //  * basepower^ (2^ndoublings * remaining exponent bits + carried)
            // 0 <= carried <= 2^(bucket_width + 2) - 1

            if (carried >> (bucket_width + 2) != 0) {
                OK = FALSE;
                SetMpErrno_clue(MP_ERRNO_INTERNAL_ERROR,
                                "mod_exp -- carried overflow");
            }

            if (bit_now != 0) {
                while (OK && ndoubling >= bucket_width + 1) {
                    if (IS_ODD(carried)) {
                        ibucket = carried & bucket_mask;
                        carried -= ibucket;
                        OK = OK && bucket_multiply(ibucket, basepower,
                                                   temps, f_pBigCtx);
                    }
                    OK = OK && basepower_squaring(basepower,
                                                  temps, f_pBigCtx);
                    carried /= 2;
                    ndoubling--;
                }
                carried |= (CRYPTO_DWORD)1 << ndoubling;
            }
            ndoubling++;
        } // for ibit

        while (OK && carried != 0) {
            CRYPTO_BOOL square_now = FALSE;

            if (carried <= max_bucket) {
                ibucket = carried;
            } else if (IS_EVEN(carried)) {
                square_now = TRUE;
            } else if (carried <= 3*max_bucket) {
                ibucket = max_bucket;
            } else {
                ibucket = carried & bucket_mask;
            }
            if (square_now) {
                carried /= 2;
                OK = OK && basepower_squaring(basepower,
                                              temps, f_pBigCtx);
            } else {
                carried -= ibucket;

                OK = OK && bucket_multiply(ibucket, basepower,
                                           temps, f_pBigCtx);
            }
        } // while carried

        for (ibucket = max_bucket; OK && ibucket >= 2; ibucket--) {
            if (temps->bucket_occupied[ibucket]) {
                CRYPTO_BOOL found = FALSE;
                CRYPTO_DWORD jbucket, jbucketlargest, kbucket;
                digit_t *bloci;

                // Try to find jbucket, kbucket, such that
                // jbucket+kbucket = ibucket, both occupied.

                if (IS_EVEN(ibucket)) {
                         // Defaults in case no jbucket found below
                    jbucketlargest = ibucket/2;
                } else {
                    jbucketlargest = 1;
                }
                for (jbucket = ibucket >> 1;
                    jbucket != ibucket && !found;
                    jbucket++) {

                    if (temps->bucket_occupied[jbucket]) {
                        jbucketlargest = jbucket;
                        found = temps->bucket_occupied[ibucket - jbucket];
                    }
                }
                jbucket = jbucketlargest;
                kbucket = ibucket - jbucket;

                   // Deposit ibucket contents at jbucket and kbucket.

                bloci = temps->bucket_location[ibucket];

                OK = OK && bucket_multiply(jbucket, bloci, temps, f_pBigCtx);

                OK = OK && bucket_multiply(kbucket, bloci, temps, f_pBigCtx);

            } // if ibucket occupied
        }  // for ibucket
    } // if base 2
    
    if (OK) {
        memcpy( answer,temps->bucket_location[1],( elng)*sizeof( digit_t ));
    }
    if (bucket_data != NULL) {
        Free_Temporaries(bucket_data);
    }
    if (temps != NULL) {
        Free_Temporaries(temps);
    } 
    return OK;
} // end mod_exp

#undef MAX_BUCKET_WIDTH


