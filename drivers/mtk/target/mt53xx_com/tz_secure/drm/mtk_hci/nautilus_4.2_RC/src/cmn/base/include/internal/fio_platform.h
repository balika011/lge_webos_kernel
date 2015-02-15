/** @addtogroup base base
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 *
 */

#ifndef FIO_PLATFORM_H
#define FIO_PLATFORM_H

#if defined(_WIN32)

#define NPI_FIO_REMOVE_OVERRIDE
#define NPI_FIO_MAKEDIR_OVERRIDE
#define NPI_FIO_GETFILEDELIMITER_OVERRIDE

#elif defined (__linux__)

#define NPI_FIO_REMOVE_OVERRIDE
#define NPI_FIO_MAKEDIR_OVERRIDE
#define NPI_FIO_GETFILEDELIMITER_OVERRIDE

#endif


#endif /* FIO_PLATFORM_H */
/** @} */
