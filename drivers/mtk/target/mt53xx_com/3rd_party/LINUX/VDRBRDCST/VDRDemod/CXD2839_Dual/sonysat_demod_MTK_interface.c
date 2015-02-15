/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_glue_dvbt2.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file sony_demod_MTK_interface.c
 *  MTK platform interface for Sony demod
 *  Please place interface which is used by Sony demod, but depend on MTK platform
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_os.h"
#include "tunerDebug.h"
#include "sonysat_common.h"

//Moved from Pd glue file: For dual tuner optimize, all sony demod global function should not place in pd glue 
// Sony CXD2834 Function trace (For Debug)
void sonysat_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONYSAT_ENTER : %s (%d)\n", funcname, linenum));
}

void sonysat_trace_log_return(sonysat_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONYSAT_RESULT_OK){
        mcDBG_MSG_L4(("SONYSAT_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONYSAT_RETURN : (line %d)\n", linenum));
    }
}

// Function trace for I2C functions (For Debug)
void sonysat_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sonysat_trace_log_enter(funcname, filename, linenum);
}
void sonysat_trace_i2c_log_return(sonysat_result_t result, const char* filename, unsigned int linenum)
{
    sonysat_trace_log_return(result, filename, linenum);
}

//-----------------------------------------------------------------------------
// Stopwatch function implementation necessary for integ code (Sony specific)
//-----------------------------------------------------------------------------
sonysat_result_t sonysat_stopwatch_start (sonysat_stopwatch_t * pStopwatch)
{
    SONYSAT_TRACE_ENTER("sonysat_stopwatch_start");

    if (!pStopwatch) {
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = x_os_get_sys_tick() * x_os_get_sys_tick_period();

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_stopwatch_sleep (sonysat_stopwatch_t * pStopwatch, uint32_t ms)
{
    SONYSAT_TRACE_ENTER("sonysat_stopwatch_sleep");
    if (!pStopwatch) {
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    SONYSAT_ARG_UNUSED(*pStopwatch);
    SONYSAT_SLEEP (ms);

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

