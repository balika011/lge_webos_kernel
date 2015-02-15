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
#include "sony_common.h"
#include "sony_demod.h"
#include "tuner_interface_if.h"
#include "sony_integ.h"

// Sony CXD2837 Function trace (For Debug)
//Moved from Pd glue file: For dual tuner optimize, all sony demod global function should not place in pd glue 
void sony_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONY_ENTER : %s (%d)\n", funcname, linenum));
}

void sony_trace_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONY_RESULT_OK){
        mcDBG_MSG_L4(("SONY_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONY_RETURN : (line %d)\n", linenum));
    }
}

// Function trace for I2C functions (For Debug)
void sony_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sony_trace_log_enter(funcname, filename, linenum);
}
void sony_trace_i2c_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    sony_trace_log_return(result, filename, linenum);
}

//-----------------------------------------------------------------------------
// Stopwatch function implementation necessary for integ code (Sony specific)
//-----------------------------------------------------------------------------
sony_result_t sony_stopwatch_start (sony_stopwatch_t * pStopwatch)
{
    SONY_TRACE_ENTER("sony_stopwatch_start");

    if (!pStopwatch) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    pStopwatch->startTime = x_os_get_sys_tick() * x_os_get_sys_tick_period();

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

sony_result_t sony_stopwatch_sleep (sony_stopwatch_t * pStopwatch, uint32_t ms)
{
    SONY_TRACE_ENTER("sony_stopwatch_sleep");
    if (!pStopwatch) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    SONY_ARG_UNUSED(*pStopwatch);
    SONY_SLEEP (ms);

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}

// This function should return dB * 1000 value.
sony_result_t sony_integ_monitor_RFLevel_MTK (sony_integ_t * pInteg, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;
    INT32 rssi = 0;

    SONY_TRACE_ENTER ("sony_integ_monitor_RFLevel_MTK");

    if (!pRFLeveldB) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_demod_I2cRepeaterEnable(pInteg->pDemod, 1);
    if(result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }
    // Call ITuner_OP to get RSSI
    // NOTE: The RSSI value is dB * 100 integer.
    if(ITuner_OP(ITunerGetCtx_Multi(pInteg->u1Tuner_ID), itGetRSSI, 0, &rssi) == ITUNER_OK){
        *pRFLeveldB = 10 * rssi;

        // Compensation using BBAGC value
        {
            /*
             *  slave    Bank    Addr    Bit              Name
             * ----------------------------------------------------------
             * <SLV-T>   11h     6Dh     [5:0]        ITDA_DAGC_GAIN[13:8]
             * <SLV-T>   11h     6Eh     [7:0]        ITDA_DAGC_GAIN[7:0]
            */
            uint8_t data[2];
            int32_t bbagcreg = 0;
            int32_t bbagcgain = 0;

            result = pInteg->pDemod->pI2c->WriteOneRegister(pInteg->pDemod->pI2c, pInteg->pDemod->i2cAddressSLVT, 0x00, 0x11);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            result = pInteg->pDemod->pI2c->ReadRegister(pInteg->pDemod->pI2c, pInteg->pDemod->i2cAddressSLVT, 0x6D, data, sizeof(data));
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            bbagcreg = ((data[0] & 0x3F) << 8) | data[1];

            switch (pInteg->pDemod->system) {
            case SONY_DTV_SYSTEM_DVBT:
            case SONY_DTV_SYSTEM_DVBT2:
            case SONY_DTV_SYSTEM_DVBC2:
                /* BBAGCGain = -0.0184 * (BBAGCReg - 528) */
                bbagcgain = -184 * (bbagcreg - 528);
                break;
            case SONY_DTV_SYSTEM_DVBC:
                /* BBAGCGain = -0.0184 * (BBAGCReg - 506) */
                bbagcgain = -184 * (bbagcreg - 506);
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            /* Get dB x 1000 value */
            if (bbagcgain >= 0) {
                bbagcgain = (bbagcgain + 5) / 10;
            } else {
                bbagcgain = (bbagcgain - 5) / 10;
            }

            mcDBG_MSG_L2(("bbagcreg = 0x%04X\n", bbagcreg));
            mcDBG_MSG_L2(("bbagcgain (dB x 1000) = %d\n", bbagcgain));

            *pRFLeveldB += bbagcgain;
        }
    }else{
        *pRFLeveldB = -100 * 1000;
        result = SONY_RESULT_ERROR_OTHER;
    }

    mcDBG_MSG_L2(("RFLevel (dB x 1000) = %d\n", *pRFLeveldB));

    result = sony_demod_I2cRepeaterEnable(pInteg->pDemod, 0);
    if(result != SONY_RESULT_OK){
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

