/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008,2009 Sony Corporation
 */
#ifndef PO_BASICTYPES__H
#define PO_BASICTYPES__H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Basic type and macro definition (platform depended)
 *
 * require: none
 */
/*==========================================================================*/
#ifndef __linux__
#define __linux__
#endif



#if defined(__linux__)
/*
 * type: fixed size integer
 */
#include <sys/types.h>

#else


/*
 * type: fixed size integer
 */
typedef char            int8_t;
typedef short           int16_t;
typedef int             int32_t;
typedef unsigned char   u_int8_t;
typedef unsigned short  u_int16_t;
typedef unsigned int    u_int32_t;


#endif

/*
 * type: bool_t
 */
typedef int             bool_t;

/*
 * type: po_size_t
 */
typedef u_int32_t       po_size_t;

/*
 * type: time_unix_t
 */
typedef unsigned int    time_unix_t;

#ifdef __cplusplus
}
#endif

#endif /* PO_BASICTYPES__H */
