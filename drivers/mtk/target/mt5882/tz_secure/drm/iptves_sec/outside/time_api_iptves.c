/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2006,2007,2008 Sony Corporation
 */

/*==========================================================================*/
/**
 * @file
 * Trusted Time and System Time Management Library
 *
 * require:
 */
/*==========================================================================*/

//#include <time.h>

#include "basic_types_iptves.h"


#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "mutex_api_iptves.h"



#include "time_api_iptves.h"

COMPONENT(TIME);

#define TIME_2038_OVER_SUPPORT
#define MKTIME_1970_SUPPORT


/*============================================================================
 * Macro Definitions
 *==========================================================================*/


#define L_1Y_SEC   (365*24*60*60)
#define L_1D_SEC   (24*60*60)
#define L_1M_SEC   (60)

#define L_INIT_TIME             L_1D_SEC

#if defined(TIME_2038_OVER_SUPPORT)
#define L_TIME_YEAR_MIN         (1970 - 1900)
#define L_TIME_YEAR_MAX         (2038 - 1 - 1900)
#define L_TIME2038_YEAR_MAX     (2106 - 1900) /* 2106: 1970 + 38 * 2 */
#define L_TIME_MIN              0x00000000
#define L_TIME_MAX              0xffffffff
#endif /* TIME_2038_OVER_SUPPORT */

#if defined(MKTIME_1970_SUPPORT)
#define L_TM_BASE_YEAR          (1970 - 1900)
#define L_TM_BASE_MON           (1 - 1)
#define L_TM_BASE_MDAY          1
#define L_TM_BASE_HOUR          (1 - 1)
#define L_TM_BASE_MIN           (1 - 1)
#define L_TM_BASE_SEC           (1 - 1)
#endif /* MKTIME_1970_SUPPORT */

#define TIME_GET_CTX()  l_ctx_tbl

/*============================================================================
 * Type Definitions
 *==========================================================================*/
typedef enum {
    TIME_STATUS_NOT_ADJUSTED = 0,
    TIME_STATUS_ADJUSTED,
    TIME_STATUS_FIXED,
} time_status_t;

typedef struct {
    time_status_t       tt_status;
    int32_t             tt_offset;
    u_int32_t           tt_fixed;

    time_status_t       st_status;
    int32_t             st_offset;
    time_unix_t         st_fixed;
} time_ctx_t;

/*============================================================================
 * Global Variables
 *==========================================================================*/
static time_ctx_t       l_ctx_tbl[1] = {{TIME_STATUS_NOT_ADJUSTED, 0, 0,
                                         TIME_STATUS_NOT_ADJUSTED, 0, 0}};
static mutex_t          l_lock[1];

/*============================================================================
 * Local Functions
 *==========================================================================*/





extern time_t mtk_time(time_t *t);
extern time_t mtk_mktime(struct tm * const tmp);

//extern double mtk_difftime(const time_t time_1, const time_t time_0);

extern int time_set_start_time(u_int32_t in_start_sec, int gmtoff);

extern int time_clear(void);

extern time_t mtk_gmtoff;



static u_int32_t
time_TmToTime(const struct tm          *in_tm,
              time_unix_t              *out_ut)
{
   // time_t gmtoff;
    struct tm tm;
    time_t t;
#if defined(TIME_2038_OVER_SUPPORT)
    int year_offset, leap_days, sec_offset;
#endif /* TIME_2038_OVER_SUPPORT */
#if defined(MKTIME_1970_SUPPORT)
    int win_sec_offset;
#endif /* MKTIME_1970_SUPPORT */

    exh_InitOk();
    MSGENT();

    tm = *in_tm;
#if defined(TIME_2038_OVER_SUPPORT)
    if (tm.tm_year < L_TIME_YEAR_MIN) {
        *out_ut = L_TIME_MIN;
        exh_Throw(RET_TIME_OK);
    } else if (tm.tm_year > L_TIME2038_YEAR_MAX) {
        *out_ut = L_TIME_MAX;
        exh_Throw(RET_TIME_OK);
    }

    if (tm.tm_year > L_TIME_YEAR_MAX) {
        year_offset = tm.tm_year - L_TIME_YEAR_MAX;
        if ((tm.tm_year / 4 * 4) == tm.tm_year && tm.tm_mon > 1)
            leap_days = 1;
        else
            leap_days = 0;
        tm.tm_year -= year_offset;

        leap_days += year_offset / 4;
        sec_offset = year_offset * L_1Y_SEC;
        sec_offset += leap_days * L_1D_SEC;
    } else
        sec_offset = 0;
#endif /* TIME_2038_OVER_SUPPORT */

#if defined(MKTIME_1970_SUPPORT)
    /*
     * Fail mktime() at 1970/01/01 00:00:00 (JST) on Windows,
     * so before add 1 day, exec mktime(),
     *    after sub sec of 1 day when 1970/01/01.
     */
    if (tm.tm_year == L_TM_BASE_YEAR
        && tm.tm_mon  == L_TM_BASE_MON
        && tm.tm_mday == L_TM_BASE_MDAY
        /*  && tm.tm_hour == L_TM_BASE_HOUR
            && tm.tm_min  == L_TM_BASE_MIN
            && tm.tm_sec  == L_TM_BASE_SEC */ ) {
        tm.tm_mday += 1;
        win_sec_offset = -L_1D_SEC;
    } else
        win_sec_offset = 0;
#endif /* MKTIME_1970_SUPPORT */

    t = mtk_mktime(&tm);
    if (t == (time_t)-1) {
        exh_Throw(RET_TIME_NG);
    }

	// !! add gmt seconds off
    //exh_Check(l_GetGmtoff(&gmtoff));

    *out_ut = (time_unix_t)(t + mtk_gmtoff);
#if defined(TIME_2038_OVER_SUPPORT)
    *out_ut += sec_offset;
#endif /* TIME_2038_OVER_SUPPORT */
#if defined(MKTIME_1970_SUPPORT)
    *out_ut += win_sec_offset;
#endif /* MKTIME_1970_SUPPORT */

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}


/*============================================================================
 * Global Functions
 *==========================================================================*/
/*----------------------------------------------------------------------------
 * Functions for SDK
 *--------------------------------------------------------------------------*/
u_int32_t
time_Init(int current_secs, int gmtoff)
{
    exh_InitOk();
    MSGENT();

    exh_Set(mutex_Init(l_lock));
    if (exh_Get() != RET_TIME_OK) exh_Throw(RET_TIME_NG);


	time_set_start_time(current_secs, gmtoff);

	


exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
time_Fin(void)
{
    exh_InitOk();
    MSGENT();

    exh_Set(mutex_Fin(l_lock));
    if (exh_Get() != RET_TIME_OK) exh_Throw(RET_TIME_NG);

	time_clear();


exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();
}

u_int32_t
time_SetTrustedTime(u_int32_t           in_tt)
{

    time_ctx_t *ctx = TIME_GET_CTX();
    u_int32_t   st;
    bool_t      is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    exh_Set(mutex_Lock(l_lock));
    if (exh_Get() != RET_TIME_OK) exh_Throw(RET_TIME_NG);

    is_locked = TRUE;

    st = (u_int32_t)(mtk_time(NULL) / 60);
	/*  TO-DO:
		returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
	*/


	
    ctx->tt_offset = in_tt - st;
    ctx->tt_status = TIME_STATUS_ADJUSTED;

	dprintf("%s tt_status:%d\n",  __FUNCTION__, ctx->tt_status);


exh_CLEANUP:
    if (is_locked) mutex_Unlock(l_lock);

    MSGEXT(exh_Get());
    exh_Return();


}

u_int32_t
time_GetTrustedTime(u_int32_t          *out_tt)
{

    time_ctx_t *ctx = TIME_GET_CTX();
    u_int32_t   tt;
    bool_t      is_locked = FALSE;

    exh_InitOk();
    MSGENT();

    if (!out_tt)
        exh_Throw(RET_TIME_ILLEGAL_ARGS);

    exh_Set(mutex_Lock(l_lock));
    if (exh_Get() != RET_TIME_OK) exh_Throw(RET_TIME_NG);

    is_locked = TRUE;


	dprintf("%s tt_status:%d\n",  __FUNCTION__, ctx->tt_status);

    if (ctx->tt_status == TIME_STATUS_ADJUSTED) 
	{
        tt = (u_int32_t)(mtk_time(NULL) / 60) + ctx->tt_offset;
		/*  TO-DO:
			returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
		*/        
    }
	else
    {
            exh_Throw(RET_TIME_INVALID);
	}

    *out_tt = tt;

exh_CLEANUP:
    if (is_locked) mutex_Unlock(l_lock);

    MSGEXT(exh_Get());
    exh_Return();

	
}

u_int32_t
time_GetSystemTime(time_unix_t         *out_st)
{



    exh_InitOk();
    MSGENT();

    if (!out_st)
        exh_Throw(RET_TIME_ILLEGAL_ARGS);


    {
        time_t t = mtk_time(NULL);
		/*  TO-DO:
			returns the time since the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds.
		*/

		
        if (t == (time_t)-1)
            exh_Throw(RET_TIME_NG);

        *out_st = (time_unix_t)t;
    }

exh_CLEANUP:
    MSGEXT(exh_Get());
    exh_Return();



}

u_int32_t
time_ConvToUnixtime(int                 in_year,
                    int                 in_mon,
                    int                 in_mday,
                    int                 in_hour,
                    int                 in_min,
                    int                 in_sec,
                    time_unix_t        *out_ut)
{

    struct tm tm;
    time_unix_t ut = 0;

    exh_InitOk();
    MSGENT();

    memset(&tm, 0x00, sizeof(tm));
    tm.tm_year = in_year - 1900;
    tm.tm_mon = in_mon - 1;
    tm.tm_mday = in_mday;
    tm.tm_hour = in_hour;
    tm.tm_min = in_min;
    tm.tm_sec = in_sec;
    exh_Check(time_TmToTime(&tm, &ut));

    *out_ut = ut;

exh_CLEANUP:
    MSGEXT(exh_Get());

    exh_Return();



}

u_int32_t
time_ConvToDatetime(int                 in_year,
                    int                 in_mon,
                    int                 in_mday,
                    int                 in_hour,
                    int                 in_min,
                    u_int32_t          *out_mt)
{

    struct tm tm;
    time_unix_t ut = 0;

    exh_InitOk();
    MSGENT();

    memset(&tm, 0x00, sizeof(tm));
    tm.tm_year = in_year - 1900;
    tm.tm_mon = in_mon - 1;
    tm.tm_mday = in_mday;
    tm.tm_hour = in_hour;
    tm.tm_min = in_min;
    tm.tm_sec = 0;
    exh_Check(time_TmToTime(&tm, &ut));

    *out_mt = ut / L_1M_SEC;

exh_CLEANUP:
    MSGEXT(exh_Get());

    exh_Return();



}

/*----------------------------------------------------------------------------
 * Functions for Application
 *--------------------------------------------------------------------------*/
u_int32_t
time_Cleanup(time_type_t                in_type)
{
    time_ctx_t *ctx = TIME_GET_CTX();

    exh_InitOk();
    MSGENT();

    if (in_type == TIME_TYPE_ALL || in_type == TIME_TYPE_TT) {
        ctx->tt_status = TIME_STATUS_NOT_ADJUSTED;
        ctx->tt_offset = 0;
        ctx->tt_fixed  = 0;
    }
    if (in_type == TIME_TYPE_ALL || in_type == TIME_TYPE_ST) {
        ctx->st_status = TIME_STATUS_NOT_ADJUSTED;
        ctx->st_offset = 0;
        ctx->st_fixed  = 0;
    }

    MSGEXT(exh_Get());
    exh_Return();
}


