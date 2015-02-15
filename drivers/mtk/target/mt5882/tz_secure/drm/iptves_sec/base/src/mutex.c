/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2005,2006,2007,2008 Sony Corporation
 */
#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "mutex_api_iptves.h"

COMPONENT(BASE);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#if defined(__linux__)

//#define MUTEX_INIT(x)           pthread_mutex_init((&(x->mutex)), NULL)
//#define MUTEX_FIN(x)            pthread_mutex_destroy(&(x->mutex))
//#define MUTEX_ENTER(x)          pthread_mutex_lock(&(x->mutex))
//#define MUTEX_LEAVE(x)          pthread_mutex_unlock(&(x->mutex))

#define MUTEX_INIT(x) 
#define MUTEX_FIN(x)            
#define MUTEX_ENTER(x)         
#define MUTEX_LEAVE(x)         

#elif defined(_WIN32)

#define MUTEX_INIT(x)           InitializeCriticalSection(&(x->mutex))
#define MUTEX_FIN(x)            DeleteCriticalSection(&(x->mutex))
#define MUTEX_ENTER(x)          EnterCriticalSection(&(x->mutex))
#define MUTEX_LEAVE(x)          LeaveCriticalSection(&(x->mutex))

#else
#error "unknown platform"
#endif

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/

/*============================================================================
 * Global Functions
 *==========================================================================*/



int
mutex_Init(mutex_t             *out_mutex)
{
    exh_InitOk();
    MSGENT();

    if (out_mutex == NULL)
        exh_Throw(RET_ILLEGALARGS);

    MUTEX_INIT(out_mutex);
    out_mutex->init = 1;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
mutex_Fin(mutex_t              *io_mutex)
{
    exh_InitOk();
    MSGENT();

    if (io_mutex == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (io_mutex->init == 1)
        MUTEX_FIN(io_mutex);
    io_mutex->init = 0;

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
mutex_Lock(mutex_t             *io_mutex)
{
    exh_InitOk();
    MSGENT();

    if (io_mutex == NULL)
        exh_Throw(RET_ILLEGALARGS);
    if (io_mutex->init == 0)
        exh_Throw(RET_INTERNALERROR);

    MUTEX_ENTER(io_mutex);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

int
mutex_Unlock(mutex_t           *io_mutex)
{
    exh_InitOk();
    MSGENT();

    if (io_mutex == NULL)
        exh_Throw(RET_ILLEGALARGS);

    if (io_mutex->init == 1)
        MUTEX_LEAVE(io_mutex);

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}
