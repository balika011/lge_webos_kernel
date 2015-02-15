/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    ccfunc.h                                                      */
/*   DATE:    02/07/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Licencee functions              */
/*                                                                          */
/****************************************************************************/
#ifndef __CCFUNC_H_
#define __CCFUNC_H_

#include "techtype.h"

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
 * @brief    Calculate SEK and SAK from Ks (f-SAC)
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    sak - SAC authentication key (128 bits = 16 bytes)
 * @param    sek - SAC encryption key (128 bits = 16 bytes);
 ****************************************************************************/
void CC_CalculateSacKeys(U8BIT *ks, U8BIT *sak, U8BIT *sek);

/*!**************************************************************************
 * @brief    Calculate CCK and SIV from Kp (f-CC)
 * @param    kp - key precursor (256 bits = 32 bytes)
 * @param    cipher - the cipher to use (STB_CI_CIPHER_DES/STB_CI_CIPHER_AES)
 * @param    cck - content control key (length depends on cipher)
 * @param    civ - key initialisation vector (only for AES-128)
 ****************************************************************************/
void CC_CalculateCcKeys(U8BIT *kp, U8BIT cipher, U8BIT *cck, U8BIT *civ);

#endif   /* !__CCFUNC_H */
