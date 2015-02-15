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

#ifndef BASE_PLATFORM_H
#define BASE_PLATFORM_H

#if defined(_WIN32)

#define NPI_BASE_GETENV_OVERRIDE

#elif defined (__linux__)

#define NPI_BASE_GETENV_OVERRIDE

#endif

#endif /* BASE_PLATFORM_H */
/** @} */
