/*****************************************************************************/
/* (c) Copyright Ocean Blue Software Ltd. - ALL RIGHTS RESERVED              */
/* This document is the CONFIDENTIAL property of Ocean Blue Software Ltd.    */
/*****************************************************************************/
/*                                                                           */
/* File Name:     cclru.c                                                    */
/*                                                                           */
/* Description:   CI+ Content Control Resource: LRU management               */
/*                                                                           */
/* Project:       CI / CI+ Library                                           */
/*                                                                           */
/* Creation Date: 02/02/2009                                                 */
/*                                                                           */
/*****************************************************************************/

/* --- includes for this file --- */
/* Ocean Blue header files */
#include "techtype.h"
#include "cclru.h"
#include "ccdef.h"

#define FUNCTION_START(x) do { CC_PRINT((">> %s\n",#x)); } while(0)
#define FUNCTION_FINISH(x) do { CC_PRINT(("<< %s\n",#x)); } while(0)

/* --- constant definitions for this file --- */

/* --- local typedef structs for this file --- */

/* --- local (static) variable declarations for this file --- */
/* (internal variables declared static to make them local) */

/* --- local function prototypes for this file --- */
/* (internal functions declared static to make them local) */


static void FindExtrema(S_CC_LRU *lru, U8BIT *imin, U8BIT *imax);
static U8BIT FindContext(S_CC_LRU *lru, void *context);
static void UpdateTimestamps(S_CC_LRU *lru, U8BIT old_timestamp);

/*---------------------------------------------------------------------------*/
/* global function definitions                                               */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Update contexts using new context
 * @param    lru - least-recently-used contexts information
 * @param    context - pointer to context
 * @note     lru must be initialised. At the very least, all function pointers
 *           and context_count must be set.
 ****************************************************************************/
void CC_UpdateLruContexts(S_CC_LRU *lru, void *context)
{
   U8BIT imax, imin;
   U8BIT ivalue;
   U8BIT old_timestamp;
   U8BIT max_timestamp;

   FUNCTION_START(CC_UpdateLruContexts);

   if (lru->context_count > 1)
   {
      /* Find minimum and maximum indices */
      FindExtrema(lru, &imin, &imax);

      /* Search the context for the value */
      ivalue = FindContext(lru, context);

      if (ivalue == lru->context_count)
      {
         ivalue = imin;
      }

      old_timestamp = lru->get_timestamp(lru->data, ivalue);
      max_timestamp = lru->get_timestamp(lru->data, imax);

      lru->set_context(lru->data, ivalue, context);
      lru->set_timestamp(lru->data, ivalue, max_timestamp + 1);

      UpdateTimestamps(lru, old_timestamp);
   }
   else if (lru->context_count > 0)
   {
      lru->set_context(lru->data, 0, context);
   }

   FUNCTION_FINISH(CC_UpdateLruContexts);
}

/*---------------------------------------------------------------------------*/
/* local function definitions                                                */
/*---------------------------------------------------------------------------*/

/*!**************************************************************************
 * @brief    Find minimum and maximum timestamps
 * @param    lru - least-recently-used contexts information
 * @param    [out] imin - index of minimal timestamp
 * @param    [out] imax - index of maximal timestamp
 ****************************************************************************/
static void FindExtrema(S_CC_LRU *lru, U8BIT *imin, U8BIT *imax)
{
   U16BIT min, max;
   U16BIT i;
   U8BIT timestamp;

   FUNCTION_START(FindExtrema);

   *imax = 0;
   max = lru->get_timestamp(lru->data, *imax);
   *imin = 0;
   min = lru->get_timestamp(lru->data, *imin);
   
   for (i = 1; i < lru->context_count; ++i)
   {
      timestamp = lru->get_timestamp(lru->data, i);
      if (timestamp >  max)
      {
         max = timestamp;
         *imax = i;
      }
      if (timestamp < min)
      {
         min = timestamp;
         *imin = i;
      }
   }

   FUNCTION_FINISH(FindExtrema);
}

/*!**************************************************************************
 * @brief    Find context in context information
 * @param    lru - least-recently-used contexts information
 * @return   index of context if found, otherwise number of contexts
 ****************************************************************************/
static U8BIT FindContext(S_CC_LRU *lru, void *context)
{
   U8BIT i;

   FUNCTION_START(FindContext);

   for (i = 0; i < lru->context_count; ++i)
   {
      if (lru->equals(lru->data, context, i))
      {
         break;
      }
   }

   FUNCTION_FINISH(FindContext);

   return i;
}

/*!**************************************************************************
 * @brief    Update timestamps to preserve timestamp invariant
 * @param    lru - least-recently-used contexts information
 * @parma    old_timestamp - timestamp that was overwritten by new context
 ****************************************************************************/
static void UpdateTimestamps(S_CC_LRU *lru, U8BIT old_timestamp)
{
   U8BIT i;
   U8BIT timestamp;

   FUNCTION_START(UpdateTimestamps);

   if (old_timestamp > 0)
   {
      for (i = 0; i < lru->context_count; ++i)
      {
         timestamp = lru->get_timestamp(lru->data, i);
         if (timestamp > old_timestamp)
         {
            lru->set_timestamp(lru->data, i, timestamp - 1);
         }
      }
   }

   FUNCTION_FINISH(UpdateTimestamps);
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
