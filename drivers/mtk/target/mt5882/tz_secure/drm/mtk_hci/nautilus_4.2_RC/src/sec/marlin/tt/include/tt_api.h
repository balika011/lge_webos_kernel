/** @addgroup tt
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

#ifndef TT_API_H
#define TT_API_H

/**
 * @file
 * Trusted Time API
 *
 * @require none
 * @author Yoshizumi Tanaka <y-tanaka@sm.sony.co.jp>
 */

NP_BEGIN_EXTERN_C


/**
 * Error
 */
#define ERR_TT_NOTAVAILABLE        (ERR_TT_BASE|1)

/*---------------------------------------------------------------------------*
 * API
 *---------------------------------------------------------------------------*/

/**
 * get the latest trusted UNIX time
 *
 * @param[out] out_time                The unix time
 * @param[out] out_flag                The flag(s)
 *
 * @return ::ERR_OK
 * @return ::ERR_TT_NOTAVAILABLE
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
extern retcode_t
tt_GetTrustedTime(
    time_unix_t        *out_time,
    u_int32_t          *out_flag);

#define TT_FLAG_NONE        0x00
#define TT_FLAG_ESTIMATE    0x01

/**
 * set the trusted Unix time
 *
 * @param [in] in_time                  The unix time
 *
 * @return ::ERR_OK
 * @return ::ERR_ILLEGALARGS
 * @return ::ERR_INTERNALERROR
 * @return ::ERR_NOMEM
 */
extern retcode_t
tt_SetTrustedTime(
    time_unix_t                 in_time);

NP_END_EXTERN_C

#endif /* TT_API_H */
/** @} */
