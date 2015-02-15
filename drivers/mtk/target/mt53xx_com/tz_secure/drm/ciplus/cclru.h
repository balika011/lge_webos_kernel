/****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED             */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.   */
/****************************************************************************/
/*                                                                          */
/*   FILE:    cclru.h                                                       */
/*   DATE:    02/02/09                                                      */
/*   PURPOSE: CI+ Content Control Resource: Least-Recently-Used management  */
/*                                                                          */
/****************************************************************************/
#ifndef __CCLRU_H_h
#define __CCLRU_H_h

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

struct cc_lru;
typedef struct cc_lru S_CC_LRU;
struct cc_lru
{
   void *data;
   U8BIT context_count;
   void (*set_context)(void *data, U8BIT index, void *context);
   U8BIT (*get_timestamp)(void *data, U8BIT index);
   void (*set_timestamp)(void *data, U8BIT index, U8BIT timestamp);
   BOOL (*equals)(void *data, void *context, U8BIT index);
};
             
/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Update contexts using new context
 * @param    lru - least-recently-used contexts information
 * @param    context - pointer to context
 * @note     lru must be initialised. At the very least, all function pointers
 *           and context_count must be set.
 ****************************************************************************/
void CC_UpdateLruContexts(S_CC_LRU *lru, void *context);

#endif   /* !__CCLRU_H */
