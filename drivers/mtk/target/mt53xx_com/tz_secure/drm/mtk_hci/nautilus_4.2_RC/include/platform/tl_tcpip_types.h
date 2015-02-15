/** @addtogroup tl
 * @ingroup platform
 * @{ */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 */

#ifndef TL_TCPIP_TYPES_H
#define TL_TCPIP_TYPES_H

NP_BEGIN_EXTERN_C

#if defined( NPOPT_ENABLE_TCPIP_TL )
/**
 * @brief This structure contains the details for a TCP/IP connection
 */
typedef struct {
    u_int16_t        port;     /**< Port to connect to */
    char            *addr;     /**< TCP/IP address     */
    int32_t          sock;     /**< Socket that is opened by a connect call */
} tcpip_context_t;
#endif

NP_END_EXTERN_C

#endif
