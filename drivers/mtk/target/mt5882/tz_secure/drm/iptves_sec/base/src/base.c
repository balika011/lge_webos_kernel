/*
 * Marlin IPTV-ES Client SDK version 3.1
 *
 * Any code and mechanism in this module must not be used
 * in any form without permissions.
 *
 * Copyright 2008,2009 Sony Corporation
 */

#include <time.h>
#include <limits.h>

#include "basic_types_iptves.h"
#include "dbg_api_iptves.h"
#include "exh_api_iptves.h"
#include "base_api_iptves.h"
#include "fio_api_iptves.h"

#include "tzst.h"
#include "x_os.h"




COMPONENT(BASE);

/*============================================================================
 * Macro Definitions
 *==========================================================================*/
#define L_FOUR_YEAR_SEC         (365 * 4 + 1) * 24 * 60 * 60

#if defined(_WIN32)
#define L_MAX_TM_SEC            59
#else
#define L_MAX_TM_SEC            61
#endif

#define L_OUT_OF_RANGE_STRING   "(beyond the year 2038)"



/*============================================================================
 * Type Definitions
 *==========================================================================*/

/*============================================================================
 * Global Variables
 *==========================================================================*/

/*============================================================================
 * Local Functions
 *==========================================================================*/


/*============================================================================
 * Global Functions
 *==========================================================================*/


int
base_Memcpy(void                       *out_dst,
            size_t                      in_dst_len,
            const void                 *in_src,
            size_t                      in_count)
{


    exh_InitOk();


    if (in_dst_len < in_count) {
        MSG1(("base_Memcpy ... [failed]"));
        exh_Throw(RET_NG);
    }
    x_memcpy(out_dst, in_src, in_count);


exh_CLEANUP:
    exh_Return();




return 0;

}

int
base_Strncpy(char                      *out_dst,
             size_t                     in_dst_len,
             const char                *in_src,
             size_t                     in_count)
{

    exh_InitOk();



    if (in_dst_len < in_count) {
        MSG1(("base_Strncpy ... [failed]"));
        exh_Throw(RET_NG);
    }
    strncpy(out_dst, in_src, in_count);


exh_CLEANUP:
    exh_Return();



	return 0;
}


char *
base_Asctime(struct tm                 *in_tm,
             char                      *out_buf)
{
    if ((in_tm->tm_sec  < 0) || (in_tm->tm_sec  > L_MAX_TM_SEC) ||
        (in_tm->tm_min  < 0) || (in_tm->tm_min  > 59)           ||
        (in_tm->tm_hour < 0) || (in_tm->tm_hour > 23)           ||
        (in_tm->tm_mday < 1) || (in_tm->tm_mday > 31)           ||
        (in_tm->tm_mon  < 0) || (in_tm->tm_mon  > 11)) {
        MSGI1(("Invalid Parameter in base_Asctime ... [failed]\n"));
        return NULL;
    }


    //if (asctime_r(in_tm, out_buf) == NULL)
        return NULL;


    //return out_buf;
}

char *
base_CTime(const time_unix_t           *in_ut)
{
    static char buf[BASE_SIZE_OF_TIME_IN_CHAR];

    //return base_Asctime(l_UnixTimeToTm(in_ut), buf);
    return NULL;
}

char *
base_CMintime(const u_int32_t          *in_mt)
{
    static char buf[BASE_SIZE_OF_TIME_IN_CHAR];

    if (*in_mt > INT_MAX / 60) {
        base_Strncpy(buf,
                     BASE_SIZE_OF_TIME_IN_CHAR,
                     L_OUT_OF_RANGE_STRING,
                     BASE_SIZE_OF_TIME_IN_CHAR - 1);
    } else {
        //time_unix_t ut = (time_unix_t)(*in_mt) * 60;

        //base_Asctime(l_UnixTimeToTm(&ut), buf);
    }

    return buf;
}



