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

#ifndef PROF_PLATFORM_H
#define PROF_PLATFORM_H

#if defined(_WIN32)

#define PROF_TIME_OVERRIDE

#elif defined (__linux__)

#define PROF_TIME_OVERRIDE

#endif


#endif /* PROF_PLATFORM_H */
/** @} */
