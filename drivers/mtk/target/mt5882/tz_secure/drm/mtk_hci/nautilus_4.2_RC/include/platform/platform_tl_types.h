/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010,2011 Sony Corporation.
 *
 */
#ifndef PLATFORM_TRANSPORTLAYER_TYPES_H
#define PLATFORM_TRANSPORTLAYER_TYPES_H

NP_BEGIN_EXTERN_C

/*------- Transport Layer -----------*/
#if defined( NPOPT_ENABLE_TCPIP_TL )
# include "tl_tcpip_api.h"
# include "tl_tcpip_types.h"
extern tcpip_context_t  tinfo_conn;
# define TINFO          &tinfo_conn
# define SENDRECV_CB    tl_tcpip_SendRecv
# define MSGFREE_CB     tl_tcpip_MsgFree
#elif defined( NPOPT_ENABLE_USB_TL )
# include "tl_usb_api.h"
# define TINFO          NULL
# define SENDRECV_CB    tl_usb_SendRecv
# define MSGFREE_CB     tl_usb_MsgFree
#else
# define TINFO          NULL
# define SENDRECV_CB    NULL
# define MSGFREE_CB     NULL
#endif

extern np_transporthandle_t tl_local;
extern np_transporthandle_t tl_remote;

#define platform_tl_getLocalTransport()  &tl_local
#define platform_tl_getRemoteTransport() &tl_remote

NP_END_EXTERN_C

#endif /* PLATFORM_TRANSPORTLAYER_TYPES_H */



