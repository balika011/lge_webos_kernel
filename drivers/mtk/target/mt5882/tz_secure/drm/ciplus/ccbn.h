/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    cccrypt.h                                                     */
/*   DATE:    09/08/10                                                      */
/*   PURPOSE: Interface for cryptographic library                           */
/*                                                                          */
/****************************************************************************/
#ifndef __CCCRYPT_H_
#define __CCCRYPT_H_

#include "techtype.h"
#include "ccdef.h"

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

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
extern void CC_ModExp(U8BIT *base, U16BIT base_len, U8BIT *exp, U16BIT exp_len,
               U8BIT *mod, U16BIT mod_len, U8BIT *result);

#endif   /* !__CCCRYPT_H */
