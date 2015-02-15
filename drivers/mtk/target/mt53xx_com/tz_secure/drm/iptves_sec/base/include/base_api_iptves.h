/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2008,2009 Sony Corporation
 */
#ifndef BASE_API_IPTVES_H
#define BASE_API_IPTVES_H

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================*/
/**
 * @file
 * Basic functions for SDK
 *
 * require:
 */
/*==========================================================================*/

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define BASE_SIZE_OF_TIME_IN_CHAR       26

/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Error Codes
 *==========================================================================*/

/*============================================================================
 * APIs
 *==========================================================================*/
/**
 * Convert unix time to struct tm (generalized time)
 *
 * @param [out]    out_tm               Struct tm
 * @param [in]     in_time              Unix time
 * @retval RET_OK                       Success
 * @retval RET_NG                       Fail
 */
extern int
base_Gmtime(struct tm                  *out_tm,
            time_t                     *in_time);

/**
 * Convert unix time to struct tm (local time)
 *
 * @param [out]    out_tm               Struct tm
 * @param [in]     in_time              Unix time
 * @retval RET_OK                       Success
 * @retval RET_NG                       Fail
 */
extern int
base_Localtime(struct tm               *out_tm,
               time_t                  *in_time);

/**
 * Meomry copy
 *
 * @param [out]    out_dst              Destination
 * @param [in]     in_dst_len           Destination buffer length
 * @param [in]     in_src               Source
 * @param [in]     in_count             Data number
 * @retval RET_OK                       Success
 * @retval RET_NG                       Fail
 */
extern int
base_Memcpy(void                       *out_dst,
            size_t                      in_dst_len,
            const void                 *in_src,
            size_t                      in_count);

/**
 * String copy
 *
 * @param [out]    out_dst              Destination
 * @param [in]     in_dst_len           Destination buffer length
 * @param [in]     in_src               Source
 * @param [in]     in_count             Data number
 * @retval RET_OK                       Success
 * @retval RET_NG                       Fail
 */
extern int
base_Strncpy(char                      *out_dst,
             size_t                     in_dst_len,
             const char                *in_src,
             size_t                     in_count);

#if !defined(IPTVES_PUBLISH_BUILD)
/*----------------------------------------------------------------------------
 * Functions for Test/Tools
 *--------------------------------------------------------------------------*/
/**
 * Convert unix time to formatted string
 *
 * @param [in]     in_ut                Unix time
 * @retval !NULL                        Success (struct tm)
 * @retval NULL                         Fail
 */
struct tm *
base_UnixTimeToTm(const time_unix_t    *in_ut);

/**
 * Convert unix time to formatted string
 *
 * @param [in]     in_ut                Unix time
 * @retval !NULL                        Success (formatted string)
 * @retval NULL                         Fail
 */
extern char *
base_CTime(const time_unix_t           *in_ut);

/**
 * Convert min time to formatted string
 *
 * @param [in]     in_mt                Min time
 * @retval !NULL                        Success (formatted string)
 * @retval NULL                         Fail
 */
extern char *
base_CMintime(const u_int32_t          *in_mt);

/**
 * Convert struct tm to formatted string
 *
 * @param [in]     in_tm                Struct tm
 * @param [out]    out_buf              Formatted string
 * @retval !NULL                        Success (formatted string)
 * @retval NULL                         Fail
 */
extern char *
base_Asctime(struct tm                 *in_tm,
             char                      *out_buf);

#endif /* !IPTVES_PUBLISH_BUILD */

#ifdef __cplusplus
}
#endif

#endif /* BASE_API_H */
