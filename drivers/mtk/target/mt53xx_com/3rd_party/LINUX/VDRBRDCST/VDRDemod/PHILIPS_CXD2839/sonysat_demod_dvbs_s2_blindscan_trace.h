/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_trace.h

          This file provides trace functions definication.
          If you would like to use this trace functions for TuneSRS/BlindScan,
          please enable "SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_ENABLE" definition,
          and implement following functions.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_H

#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"

#ifdef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_ENABLE
/**
 @brief Start trace function.

 This function is called at the beginning of TuneSRS/BlindScan.
 If you have any process to do at this timing, please implement it in this function.
 (i.e. Open output file, etc...)
*/
void sonysat_demod_dvbs_s2_blindscan_trace_init ();
/**
 @brief Output power list.

 @param name The event name.
 @param pList Power data list.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_powerlist (char * name, sonysat_demod_dvbs_s2_blindscan_power_t * pList);
/**
 @brief Output candidate list.

 @param name The event name.
 @param pList Candidate data list.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_candlist (char * name, sonysat_demod_dvbs_s2_blindscan_data_t * pList);
/**
 @brief Start BT sub sequence.

 @param freqKHz Frequency in KHz.
 @param candSR Candidate symbol rate in KSps.
 @param minCandSR Min candidate symbol rate in KSps.
 @param maxCandSR Max candidate symbol rate in KSps.
 @param minSR Min search range symbol rate in KSps.
 @param maxSR Max search range symbol rate in KSps.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_bt_start (uint32_t freqKHz, uint32_t candSR, uint32_t minCandSR, uint32_t maxCandSR, uint32_t minSR, uint32_t maxSR);
/**
 @brief Finish BT sub sequence.

 @param eventName The result of that BT sub sequence.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_bt_result (char * eventName);
/**
 @brief RF tune for CS sub sequence.

 @param freqKHz Frequency in KHz.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_cs_tune (uint32_t freqKHz);
/**
 @brief Start CS sub sequence.

 @param offsetFreqKHz Frequency offset in KHz.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_cs_start (int32_t offsetFreqKHz);
/**
 @brief Finish CS sub sequence.

 @param coarseSR Detected symbol rate in KSps.
        If symbol rate was not detected coarseSR is 0 or 1.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_cs_result (uint32_t coarseSR);
/**
 @brief PM sub sequence.

 @param offsetFreqKHz Frequency offset in KHz.
 @param power Power value.
*/
void sonysat_demod_dvbs_s2_blindscan_trace_pm_power (int32_t offsetFreqKHz, int32_t power);
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_INIT() sonysat_demod_dvbs_s2_blindscan_trace_init()
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_POWERLIST(A, B) sonysat_demod_dvbs_s2_blindscan_trace_powerlist(A, B)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CANDLIST(A, B) sonysat_demod_dvbs_s2_blindscan_trace_candlist(A, B)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_START(A, B, C, D, E, F) sonysat_demod_dvbs_s2_blindscan_trace_bt_start(A, B, C, D, E, F)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT(A) sonysat_demod_dvbs_s2_blindscan_trace_bt_result(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_TUNE(A) sonysat_demod_dvbs_s2_blindscan_trace_cs_tune(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_START(A) sonysat_demod_dvbs_s2_blindscan_trace_cs_start(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_RESULT(A) sonysat_demod_dvbs_s2_blindscan_trace_cs_result(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_PM_POWER(A, B) sonysat_demod_dvbs_s2_blindscan_trace_pm_power(A, B)
#else  /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_ENABLE */
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_INIT()
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_POWERLIST(A, B)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CANDLIST(A, B)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_START(A, B, C, D, E, F)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_TUNE(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_START(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_RESULT(A)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_PM_POWER(A, B)
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_FIN()
#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_ENABLE */

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_H */
