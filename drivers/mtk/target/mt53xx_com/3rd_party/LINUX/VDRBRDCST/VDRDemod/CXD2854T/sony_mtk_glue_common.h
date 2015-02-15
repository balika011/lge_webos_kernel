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
#include "cust_common_interface.h"


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
    sony_i2c_t i2cTuner;  // Used for tuner register dump only.

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

    // Device context object pointer
    VOID* pPdCtxDvb;
    VOID* pTunerDeviceCtxDvb; // Used in DVB-T side.
    VOID* pPdCtxIsdb;
    VOID* pTunerDeviceCtxIsdb; // Used in ISDB-T side.

    // Global variables used in DVB-T2 side.
    BOOL fgHWReset; /*wenming 12-3-24: HW reset flag*/
    BOOL fgChanScanforGetAtt;
    BOOL fgDemodInited;
    UINT32 u4Uec;
    UINT8  u1UecAccDelay;
} sony_demod_ctx_t;

/*------------------------------------------------------------------------------
  Sony driver struct instance. (global)
  Index 0 : Main, 1 : Sub
------------------------------------------------------------------------------*/
#define SONY_DEMOD_CTX_NUM 2
extern sony_demod_ctx_t g_sony_demod_ctx[SONY_DEMOD_CTX_NUM];

/*------------------------------------------------------------------------------
  Sony product information (global)
------------------------------------------------------------------------------*/
#ifdef CC_PROJECT_FOR_SONY
extern SONYPRODUCT_TYPEINFO g_sonyproduct_typeinfo;
#endif

/*------------------------------------------------------------------------------
  Sony specific driver and demod/tuner initialization.
  This API should be called from XXXX_TunerOpen API.
------------------------------------------------------------------------------*/
INT32 sony_mtk_glue_common_Initialize (UINT8 tunerID);

/*------------------------------------------------------------------------------
  Utilities
------------------------------------------------------------------------------*/
UINT32 sony_strtoul(const char* str);
const char* sony_FormatResult(sony_result_t result);

#endif /* SONY_MTK_GLUE_COMMON_H */
