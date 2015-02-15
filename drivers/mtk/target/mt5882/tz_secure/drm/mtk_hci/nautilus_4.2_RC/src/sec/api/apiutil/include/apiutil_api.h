/** @addtogroup apiutil apiutil
 *  @ingroup api
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef APIUTIL_API_H
#define APIUTIL_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /** @file
     * @brief Nautilus Library API Utilities
     *
     * depend: @ref base, @ref time, @ref tmcxt, @ref tt
     *
     * @author Yoshizumi Tanaka <y-tanaka@sm.sony.co.jp>
     * @date   21 May, 2006 created by y-tanaka
     */
    /*===========================================================================*/

    /*===========================================================================*/
    /**
     * @brief License D/B API
     */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*
     * type definition
     *---------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Set time information to the time context.
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    apiutil_SetTime(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Set time information from cache to the time context.
     *
     * @return ::ERR_OK
     * @return ::ERR_INTERNALERROR
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    apiutil_SetTimeFromCache(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Reset time information of the time context.
     *
     * @return ::ERR_OK
     */
    /*---------------------------------------------------------------------------*/
    extern retcode_t
    apiutil_ResetTime(void);

#ifdef __cplusplus
}
#endif

#endif /* APIUTIL_API_H */
/** @} */
