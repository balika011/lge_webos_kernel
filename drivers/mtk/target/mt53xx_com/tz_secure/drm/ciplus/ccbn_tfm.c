/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     cccrypt_tfm.c                                              */
/*                                                                           */
/* Description:   Interface for cryptographic library (TomsFastMath)         */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 09/08/2010                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */

/* third party header files */
//#include "tfm.h"

/* Ocean Blue header files */
#include "ccdef.h"
#include "stbcihw.h"
#include "techtype.h"
//#include "tomcrypt.h"
#include "tommath.h"
/* --- constant definitions for this file --- */

#ifdef DEBUG_CCCRYPT
#define CC_DEBUG_PRINT(x) CC_PRINT(x)
#define CC_DEBUG_BUFFER(x,y) CC_PRINT_BUFFER(x,y)
#else
#define CC_DEBUG_PRINT(x)
#define CC_DEBUG_BUFFER(x,y)
#endif

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Perform modular exponentiation
 * @param    base - the base of the exponent
 * @param    base_len - length of base in bytes
 * @param    exp - the exponent
 * @param    exp_len - length of exponent in bytes
 * @param    mod - the modulus
 * @param    mod_len - length of modulus in bytes
 * @param    result - the result base ^ exp % mod
 * @note     The result is the same length as the modulus (mod_len)
 ****************************************************************************/
void CC_ModExp(U8BIT *base, U16BIT base_len, U8BIT *exp, U16BIT exp_len,
               U8BIT *mod, U16BIT mod_len, U8BIT *result)
{
   mp_int *vars;
   mp_int *i_base, *i_exp, *i_mod, *i_result;
   int bits;

   vars = (mp_int *)malloc(4 * sizeof(mp_int));
   if (vars != NULL)
   {
      i_base = &vars[0];
      i_exp = &vars[1];
      i_mod = &vars[2];
      i_result = &vars[3];
      mp_init(i_base);
      mp_init(i_exp);
      mp_init(i_mod);
      mp_init(i_result);
      mp_read_unsigned_bin(i_base, base, base_len);
      mp_read_unsigned_bin(i_exp, exp, exp_len);
      mp_read_unsigned_bin(i_mod, mod, mod_len);

      CC_DEBUG_PRINT(("Calling fp_exptmod\n"));
      CC_DEBUG_PRINT(("=======  base (%d bytes):\n", base_len));
      CC_DEBUG_BUFFER(base, base_len);
      CC_DEBUG_PRINT(("=======  exp (%d bytes):\n", exp_len));
      CC_DEBUG_BUFFER(exp, exp_len);
      CC_DEBUG_PRINT(("=======  mod (%d bytes):\n", mod_len));
      CC_DEBUG_BUFFER(mod, mod_len);
      mp_exptmod(i_base, i_exp, i_mod, i_result);
      CC_DEBUG_PRINT(("Back from fp_exptmod\n"));

      memset(result, 0, mod_len);
      bits = mp_count_bits(i_result);
      CC_DEBUG_PRINT(("bits = %d, offset = %d\n",
                bits, (mod_len * 8 - bits) / 8));
      mp_to_unsigned_bin(i_result, result + (mod_len * 8 - bits) / 8);

      CC_DEBUG_PRINT(("=======  result (%d bytes):\n", mod_len));
      CC_DEBUG_BUFFER(result, mod_len);
      
      /* Clear variables */
      mp_clear(i_base);
      mp_clear(i_exp);
      mp_clear(i_mod);
      mp_clear(i_result);
      memset(vars, 0xbd, 4 * sizeof(mp_int));

      free((U8BIT *)vars);
   }
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
