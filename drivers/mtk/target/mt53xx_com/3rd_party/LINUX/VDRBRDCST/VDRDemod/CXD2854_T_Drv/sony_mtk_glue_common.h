/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/*
  Commonly used functions for MTK porting.
*/

#ifndef SONY_MTK_GLUE_COMMON_H
#define SONY_MTK_GLUE_COMMON_H

#ifdef SONY_DUMMY_DEBUG
#include "MTKdef.h" // MTK dummy definition for SONY internal debug.
#else
// Please add necessary header files.
#include "x_typedef.h"
#include "u_tuner.h"
#endif

#include "sony_integ.h"


#ifndef tuner_break_fct
#define tuner_break_fct
typedef INT32     (*x_break_fct)(void *pArg);
#endif

/*------------------------------------------------------------------------------
  Sony driver struct to control demod and tuner.
------------------------------------------------------------------------------*/
typedef struct _sony_demod_ctx_t
{
    // Instances to control Sony driver
    sony_demod_t demod;
    sony_tuner_terr_cable_t tuner;
    sony_integ_t integ;
    sony_i2c_t i2c;

    BOOL autoPLP;         // PLP Autodetect ON/OFF
    UINT32 frequency;     // in kHz

#ifdef CC_SONY_DEMOD_SUPPORT_DVBC2
    TUNER_DVBC2_DS_PLP_LIST c2DsPlpList; // C2 Tuning Parameter List (for C2 scan)
    UINT32 c2NextFrequency; // Next frequency to be tried in Hz. (for C2 scan)
    UINT8 c2DsId;         // DVB-C2 DS ID (for DVBT2_TunerGetSignal)
    UINT8 c2PlpId;        // DVB-C2 PLP ID (for DVBT2_TunerGetSignal)
#endif
    BOOL fgEws;           // Previous EWS flag value (For ISDB-T)

    x_break_fct breakFct; // Break function pointer to use in stopwatch function
    void* breakFctArg;    // Argument for break function
} sony_demod_ctx_t;

/*------------------------------------------------------------------------------
  Sony driver struct instance. (global)
------------------------------------------------------------------------------*/
extern sony_demod_ctx_t g_sony_demod_ctx;

/*------------------------------------------------------------------------------
  Sony specific driver and demod/tuner initialization.
  This API should be called from XXXX_TunerOpen API.
------------------------------------------------------------------------------*/
INT32 sony_mtk_glue_common_Initialize (void);

/*------------------------------------------------------------------------------
  Get RSSI from tuner using ITuner_OP(itGetRSSI) and compensate using
  demod internal digital AGC value.
  This API is called from glue APIs and Sony driver APIs.
------------------------------------------------------------------------------*/
sony_result_t sony_integ_monitor_RFLevel_MTK (sony_integ_t * pInteg, int32_t * pRFLeveldB);

/*------------------------------------------------------------------------------
  Utilities
------------------------------------------------------------------------------*/
UINT32 sony_strtoul(const char* str);
const char* sony_FormatResult(sony_result_t result);

#endif /* SONY_MTK_GLUE_COMMON_H */
