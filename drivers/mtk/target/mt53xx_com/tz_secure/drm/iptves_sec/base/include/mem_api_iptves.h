/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef MEM_API_IPTVES_H
#define MEM_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Memory Allocation/De-allocation API
 *
 * require: base
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    MEM_MODE_NORMAL = 0,
    MEM_MODE_ABNORMAL
} mem_mode_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/

/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
/**
 * This function allocates unused space for an array of <i>num</i> elements
 * each of whose size in bytes is <i>size</i>.
 * The space is initialised to all bits 0.
 *
 * @param [in] num              Number of elements
 * @param [in] size             Size of elements type
 * @retval a pointer to the allocated space
 */
extern void *
mem_Calloc(size_t               num,
           size_t               size);

/**
 * This function allocates unused space for an object whose size in bytes is
 * specified by <i>size</i> and whose value is indeterminate.
 *
 * @param [in] size             Size in bytes
 * @retval a pointer to the allocated space
 */
extern void *
mem_Malloc(size_t               size);

/**
 * This function causes the space pointed to by <i>ptr</i> to be deallocated.
 *
 * @param [in] ptr              Pointer of the space to be deallocated
 */
extern void
mem_Free(void                  *ptr);

/**
 * This function changes the size of the memory object pointed to by <i>ptr</i>
 * to the size specified by <i>size</i>.
 * If ptr is a null pointer, mem_Realloc() behaves like mem_Malloc() for the
 * specified size.
 *
 * @param [in] ptr              Pointer of the space to be deallocated
 * @param [in] size             Size in bytes
 * @retval a pointer to the re-allocated space
 */
extern void *
mem_Realloc(void               *ptr,
            size_t              size);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
extern int
mem_SetMode(mem_mode_t          in_mode);

#endif /* !IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* MEM_API_H */
