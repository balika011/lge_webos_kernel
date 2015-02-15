/** @addtogroup drm drm
 * @ingroup marlin
 *  @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef LIL_API_H
#define LIL_API_H

NP_BEGIN_EXTERN_C

/*===========================================================================*/
/** @file
 * @brief Marlin LIL Implementation
 *
 * dependency:
 *
 */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * error code definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * type definitions
 *---------------------------------------------------------------------------*/

typedef struct {
    int32_t      off;
    int32_t      isupdated;
    u_int32_t    not_after;
    binstr_t    *lid;
    binstr_t    *cur;
} lil_t;

/*---------------------------------------------------------------------------*
 * macro
 *---------------------------------------------------------------------------*/

#define lil_GetOffset(c)     ((c) ? (c)->off : 0)
#define lil_IsUpdated(c)     ((c) ? (c)->isupdated : 0)
#define lil_GetNotAfter(c)   ((c) ? (c)->not_after : 0)
#define lil_GetLID(c)        ((c) ? (c)->lid : NULL)
#define lil_GetLIL(c)        ((c) ? (c)->cur : NULL)
#define lil_GetLILLen(c)     ((c) ? (((c)->cur) ? binstr_Len((c)->cur) : 0) : 0)
#define lil_GetLILBuf(c)     ((c) ? (((c)->cur) ? binstr_Buf((c)->cur) : NULL) : NULL)

/*---------------------------------------------------------------------------*/
/**
 * Load license id list to lil context.
 *
 * Loading license id list and initializing context.
 *
 * @param[in] in_lil buffer storing license id list.
 * @param[in] in_wocopy flag indicating whether to copy lil to context or not.
 * @param[out] out_context pointer to the buffer where lil context is stored.
 * @return ::ERR_OK
 * @return ::ERR_NOMEM
 */
extern retcode_t
lil_Load(
    binstr_t  *in_lil,
    int32_t    in_wocopy,
    lil_t    **out_context);

/*---------------------------------------------------------------------------*/
/**
 * Unload license id list from lil context.
 *
 * Unloading license id list from lil context.
 *
 * @param[in] in_context lil context
 * @return ::ERR_OK
 */
extern retcode_t
lil_Unload(
    lil_t    *in_context);

/*---------------------------------------------------------------------------*/
/**
 * Find a entry from lil context.
 *
 * Find the entry linked by inputed license id from lil context.
 *
 * @param[in] in_context lil context
 * @param[in] in_id license id
 * @return ::ERR_OK
 */
extern retcode_t
lil_Find(
    lil_t       *in_context,
    binstr_t    *in_id);

/*---------------------------------------------------------------------------*/
/**
 * Append new entry to lil context
 *
 * Appending new entry to lil context and storing NotAfter and NotLongerThan.
 *
 * @param[in] in_context lil context
 * @param[in] in_id license id
 * @return ::ERR_OK
 */
extern retcode_t
lil_Append(
    lil_t      *in_context,
    binstr_t   *in_id,
    u_int32_t   in_notafter);


/*---------------------------------------------------------------------------*/
/**
 * Update the NotAfter
 *
 * Updating the NotAfter value in the entry found by lil_Find
 * lil_Find should be called in advance
 *
 * @param[in] in_context lil context
 * @param[in] in_notafter NotAfter date
 * @return ::ERR_OK
 * @return ::ERR_INTERNALERROR
 */
extern retcode_t
lil_Update(
    lil_t      *in_context,
    u_int32_t   in_notafter);

/*---------------------------------------------------------------------------*/
/**
 * Garbage collection
 *
 * Tyring garbage collection of license id list.
 *
 * @param[in] in_context lil context
 * @return ::ERR_OK
 */
extern retcode_t
lil_GarbageCollection(
    lil_t    *in_context);

NP_END_EXTERN_C

#endif /* LIL_API_H */
/** @} */
