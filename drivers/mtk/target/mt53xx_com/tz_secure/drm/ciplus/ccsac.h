/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    ccsac.h                                                       */
/*   DATE:    16/01/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Secure Authenticated Channel    */
/*                                                                          */
/****************************************************************************/
#ifndef __CCSAC_H_
#define __CCSAC_H_

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
 * @brief    Initialise SAC
 * @param    sac - pointer to SAC structure
 * @param    ks - key seed (256 bits = 32 bytes)
 * @param    siv - SAC initialisation vector (128 bits = 16 bytes)
 * @param    mem_alloc - dynamic memory allocation function
 * @param    mem_free - dynamic memory freeing function
 ****************************************************************************/
void CC_InitialiseSac(S_CC_SAC *sac, U8BIT *ks, U8BIT *siv,
                      void *(*mem_alloc)(U32BIT), void (*mem_free)(void *));

/*!**************************************************************************
 * @brief    Embed SAC message in an APDU
 * @param    sac - pointer to SAC structure
 * @param    tag - APDU tag
 * @param    payload - payload of message to embed
 * @param    payload_len - length of payload
 * @param    apdu - output APDU
 * @param    apdu_len - length of APDU
 * @note     If the output APDU is not NULL, it must be freed
 ****************************************************************************/
void CC_EmbedSacMessage(S_CC_SAC *sac,U8BIT *payload,
                        U16BIT payload_len, U8BIT **apdu, U32BIT *apdu_len);

/*!**************************************************************************
 * @brief    Extract Sac message from APDU
 * @param    sac - pointer to SAC structure
 * @param    data - input APDU data (not including tag and length)
 * @param    data_len - input APDU data length
 * @param    payload - embedded payload
 * @param    payload_len - length of embedded payload
 * @note     If the output payload is not NULL, it must be freed
 ****************************************************************************/
void CC_ExtractSacMessage(S_CC_SAC *sac, U8BIT *data, U32BIT data_len,
                          U8BIT **payload, U16BIT *payload_len);

/*!**************************************************************************
 * @brief    Return SAK (SAC authentication key) from SAC structure
 * @param    sac - pointer to SAC structure
 * @return   Pointer to SAK
 ****************************************************************************/
U8BIT *CC_GetSacAuthenticationKey(S_CC_SAC *sac);

#endif   /* !__CCSAC_H */
