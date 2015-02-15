/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 */

#ifndef NET_API_H
#define NET_API_H

#ifdef __cplusplus
extern "C" {
#endif

    /*---------------------------------------------------------------------------*
     * macro definitions
     *---------------------------------------------------------------------------*/

    /**
     * @brief Error code
     */
#define ERR_NET_BASE 0x00001f00
#define ERR_NET_OK   0
#define ERR_NET_INIT (ERR_NET_BASE|1)

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Initialize Network
     *
     * @param
     * @return ::ERR_NET_OK
     * @return ::ERR_NET_INIT
     */
    /*---------------------------------------------------------------------------*/
    extern int32_t
    net_Init(void);

    /*---------------------------------------------------------------------------*/
    /**
     * @brief Finalize Network
     *
     * @param
     * @return ::ERR_NET_OK
     */
    /*---------------------------------------------------------------------------*/
    extern int32_t
    net_Fin(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_API_H */
