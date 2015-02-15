/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 *
 */

#ifndef BASIC_TYPES__H
#define BASIC_TYPES__H

/*===========================================================================*/
/** @file
 * Basic type and macro definition (NP internal, platform depended)
 *
 * @reqire none
 * @author Norifumi Goto <n-goto@sm.sony.co.jp>
 */
/*===========================================================================*/

NP_BEGIN_EXTERN_C

#include <stdlib.h>

#include <memory.h>
#include <process.h>
#include <windows.h>

/*
 * def: NAUTILUS_MODULE
 */

#define NAUTILUS_MODULE(modulename) \
    char *l_modulename = #modulename;

/*
 * def: INLINE
 */
#define INLINE                __inline


/*
 * type: bool_t
 *
 */
typedef int32_t            bool_t;

/*
 * type: uintptr_t
 */
typedef u_int32_t            uintptr_t;

/*
 * type: fio_size_t, fio_offset_t
 */

typedef np_size_t             fio_size_t;
typedef int32_t               fio_offset_t;

/*
 * type: time_unix_t
 */
#if MSC_VER < 1400
typedef u_int32_t        time_unix_t;
#else
typedef time_t            time_unix_t;
#endif

/*
 * type: mutex_t
 */
typedef struct {
    CRITICAL_SECTION mutex;
    int32_t          init;
}                           mutex_t;

NP_END_EXTERN_C

#endif
