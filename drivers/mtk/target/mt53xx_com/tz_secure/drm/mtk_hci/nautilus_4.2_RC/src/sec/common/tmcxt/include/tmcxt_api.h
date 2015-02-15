/** @addtogroup tmcxt tmcxt
 *  @ingroup common
 * @{
 */

/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011 Sony Corporation.
 *
 */

#ifndef TMCXT_API_H
#define TMCXT_API_H

/**
 * @file
 * Time Context API
 *
 * @require none
 * @author Yoshizumi Tanaka <y-tanaka@sm.sony.co.jp>
 */

NP_BEGIN_EXTERN_C

#define ERR_TMCXT_TTNOTAVAILABLE    (ERR_TMCXT_BASE|1)

/*---------------------------------------------------------------------------*
 * API
 *---------------------------------------------------------------------------*/

#define TMCXT_TT_STATUS_DISABLED    0x00
#define TMCXT_TT_STATUS_ACCURATE    0x01
#define TMCXT_TT_STATUS_ESTIMATED   0x02

/**
 * Set times in the tmcxt.
 *
 * @param[in] in_tt      Trusted Time
 * @param[in] in_status  Status
 * @param[in] in_st      System Time
 * @param[in] in_offset  Local Time Offset
 * @return ::ERR_OK
 */
extern retcode_t
tmcxt_SetTime(time_unix_t in_tt,
              u_int32_t   in_status,
              time_unix_t in_st,
              int32_t     in_offset);

/**
 * Get trusted time
 *
 * @param[out] out_tt     Trusted Time
 * @param[out] out_status Context status (one of ::TMCXT_TT_STATUS_ESTIMATED,
 *                        ::TMCXT_TT_STATUS_ACCURATE or ::TMCXT_TT_STATUS_DISABLED )
 * @return ::ERR_OK
 * @return ::ERR_TMCXT_TTNOTAVAILABLE
 */
extern retcode_t
tmcxt_GetTrustedTime(time_unix_t *out_tt,
                     u_int32_t   *out_status);

/**
 * Get system time
 *
 * @param[out] out_st System Time (UTC)
 * @return ::ERR_OK
 */
extern retcode_t
tmcxt_GetSystemTime(time_unix_t *out_st);

/**
 * Get local time offset.
 * This value is difference from system time.
 *
 * @param[out] out_offset Local Time Offset
 * @return ::ERR_OK
 */
extern retcode_t
tmcxt_GetLocalTimeOffset(int32_t *out_offset);

/**
 * Get time. If trusted time is set, it is trusted time,
 * else, it is system time.
 *
 * @param[out] out_time Trusted Time or System Time
 * @return ::ERR_OK
 */
extern retcode_t
tmcxt_GetTimeForValidation(time_unix_t *out_time);

NP_END_EXTERN_C

#endif /* TMCXT_API_H */
/** @} */
