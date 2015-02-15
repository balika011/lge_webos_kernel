/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */
#ifndef TIME_API_IPTVES_H
#define TIME_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Trusted Time and System Time Management Library
 *
 * require:
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    TIME_TYPE_ALL = 0,
    TIME_TYPE_TT,
    TIME_TYPE_ST
} time_type_t;

/*============================================================================
 * Error Codes
 *==========================================================================*/
#define RET_TIME_OK                     0x00000000
#define RET_TIME_NG                     0x00000001
#define RET_TIME_ILLEGAL_ARGS           0x00000002
#define RET_TIME_INVALID                0x00000003

/*============================================================================
 * APIs
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
/**
 * Initialize component (not MT-safe)
 *
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_Init(int current_secs, int gmtoff);


/**
 * Finalize component (not MT-safe)
 *
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_Fin(void);

/**
 * Set trusted time
 *
 * @param [in]     in_tt                Trusted time (min time)
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_SetTrustedTime(u_int32_t           in_tt);

/**
 * Get trusted time
 *
 * @param [out]    out_tt               Trusted time (min time)
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 * @retval RET_TIME_ILLEGAL_ARGS        Illegal arguments are passed to this API
 * @retval RET_TIME_INVALID             Trusted time is not set
 */
extern u_int32_t
time_GetTrustedTime(u_int32_t          *out_tt);

/**
 * Get system time
 *
 * @param [out]    out_st               System time (unix time)
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 * @retval RET_TIME_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
time_GetSystemTime(time_unix_t         *out_st);

/**
 * Convert to unix time
 *
 * @param [in]     in_year              Year
 * @param [in]     in_mon               Month
 * @param [in]     in_mday              Day
 * @param [in]     in_hour              Hour
 * @param [in]     in_min               Minute
 * @param [in]     in_sec               Second
 * @param [out]    out_ut               Unix time
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 * @retval RET_TIME_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
time_ConvToUnixtime(int                 in_year,
                    int                 in_mon,
                    int                 in_mday,
                    int                 in_hour,
                    int                 in_min,
                    int                 in_sec,
                    time_unix_t        *out_ut);

/**
 * Convert to datetime
 *
 * @param [in]     in_year              Year
 * @param [in]     in_mon               Month
 * @param [in]     in_mday              Day
 * @param [in]     in_hour              Hour
 * @param [in]     in_min               Minute
 * @param [out]    out_mt               Min time
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 * @retval RET_TIME_ILLEGAL_ARGS        Illegal arguments are passed to this API
 */
extern u_int32_t
time_ConvToDatetime(int                 in_year,
                    int                 in_mon,
                    int                 in_mday,
                    int                 in_hour,
                    int                 in_min,
                    u_int32_t          *out_mt);

/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
/**
 * Clean up context
 *
 * @param [in]     in_type              Time Type
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_Cleanup(time_type_t                in_type);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Set fixed trusted time
 *
 * @param [in]     in_tt                Fixex trusted time (min time)
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_SetFixedTrustedTime(u_int32_t      in_tt);

/**
 * Set fixed system time
 *
 * @param [in]     in_st                Fixex system time (unix time)
 * @retval RET_TIME_OK                  Success
 * @retval RET_TIME_NG                  Fail
 */
extern u_int32_t
time_SetFixedSystemTime(const time_unix_t in_st);

#endif /* IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* TIME_API_H */
