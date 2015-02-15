/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "mem_api_iptves.h"

#include "tzst.h"
//#include "tz_dmx_if.h"
#include "x_os.h"





/* COMPONENT(BASE); */

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
#if !defined(IPTVES_PUBLISH_BUILD)
#define MEM_ALLOC(func)                         \
    do {                                        \
        if (l_mode == MEM_MODE_ABNORMAL)        \
            return NULL;                        \
        else                                    \
            return func;                        \
    } while (0)
#else
#define MEM_ALLOC(func) return func
#endif /* !IPTVES_PUBLISH_BUILD */

/*============================================================================
 * Global Variables
 *==========================================================================*/
#if !defined(IPTVES_PUBLISH_BUILD)
static mem_mode_t l_mode = MEM_MODE_NORMAL;
#endif /* !IPTVES_PUBLISH_BUILD */

/*============================================================================
 * Local Functions
 *==========================================================================*/

/*============================================================================
 * Global Functions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
void *
mem_Calloc(size_t num, size_t size)
{
    return calloc(num, size);

}

void *
mem_Malloc(size_t size)
{
     return malloc(size);

}

void
mem_Free(void *ptr)
{
    free(ptr);
}

void *
mem_Realloc(void *ptr, size_t size)
{
     return realloc(ptr, size);
   
}

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
int
mem_SetMode(mem_mode_t in_mode)
{
    exh_InitOk();

    if (in_mode < MEM_MODE_NORMAL || MEM_MODE_ABNORMAL < in_mode)
        exh_Throw(RET_ILLEGALARGS);

    l_mode = in_mode;

exh_CLEANUP:
    exh_Return();
}
#endif /* !IPTVES_PUBLISH_BUILD */
