/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 *
 */

#ifndef MUTEX_PLATFORM_H
#define MUTEX_PLATFORM_H

#if defined(_WIN32)

#define NPI_MUTEX_OVERRIDE
#define NPI_MUTEX_PROC_OVERRIDE

#elif defined (__linux__)

#define NPI_MUTEX_OVERRIDE
#define NPI_MUTEX_PROC_OVERRIDE

#endif

#endif /* MUTEX_PLATFORM_H */
/** @} */
