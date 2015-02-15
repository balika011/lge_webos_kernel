/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2008,2009,2010 Sony Corporation.
 *
 */

#ifndef NP_BASICTYPES__H
#define NP_BASICTYPES__H

/*===========================================================================*/
/** @file
 * @brief Basic type and macro definition (platform depended)
 *
 * @require none
 * @author  Norifumi Goto <n-goto@sm.sony.co.jp>
 */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * type: fixed size integer
 */
typedef char             int8_t;
typedef short            int16_t;
typedef int              int32_t;
typedef unsigned char    u_int8_t;
typedef unsigned short   u_int16_t;
typedef unsigned int     u_int32_t;

/*
 * type: np_size_t
 */
typedef u_int32_t        np_size_t;

#ifdef __cplusplus
}
#endif

#endif /* NP_BASICTYPES__H */
