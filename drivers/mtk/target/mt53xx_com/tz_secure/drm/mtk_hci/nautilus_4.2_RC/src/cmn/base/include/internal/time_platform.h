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

#ifndef TIME_PLATFORM_H
#define TIME_PLATFORM_H

#if defined(_WIN32)

#define NPI_TIME_GETLOCALTIMEOFFSET_OVERRIDE
#define NPI_TIME_GETSEEDVALUE_OVERRIDE
#define NPI_TIME_DATE2UNIX_OVERRIDE
#define NPI_TIME_UNIX2DATE_OVERRIDE

#elif defined (__linux__)

#define NPI_TIME_GETSEEDVALUE_OVERRIDE
#define NPI_TIME_DATE2UNIX_OVERRIDE
#define NPI_TIME_UNIX2DATE_OVERRIDE

#if defined(ANDROID)

#define NPI_TIME_GETLOCALTIMEOFFSET_OVERRIDE

#endif

#endif

#endif /* TIME_PLATFORM_H */
/** @} */
