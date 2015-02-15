/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_monitor.h

          This file provides the DVB-S/S2 demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_MONITOR_H
#define SONYSAT_DEMOD_DVBS_S2_MONITOR_H

#include "sonysat_common.h"
#include "sonysat_demod.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/**
 @brief Monitors the synchronisation state of the DVB-S/S2 demodulator.
 
 @param pDemod The demodulator instance.
 @param pTSLockStat Indicates the TS lock condition.
        - 0: TS not locked.
        - 1: TS locked.

 @return SONYSAT_RESULT_OK if pTSLockStat is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_SyncStat (sonysat_demod_t * pDemod,
                                                   uint8_t * pTSLockStat);

/**
 @brief Monitor the carrier offset of the currently tuned channel.
        To get the estimated center frequency of the current channel:
        Fest = Ftune (KHz) + pOffset (KHz) ;

 @param pDemod The demodulator instance.
 @param pOffset The detected carrier offset in KHz.

 @return SONYSAT_RESULT_OK if pOffset is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_CarrierOffset (sonysat_demod_t * pDemod,
                                                        int32_t * pOffset);

/**
 @brief Monitor the IFAGC value of the DVB-S/S2 demodulator.
        Actual dB gain dependent on attached tuner.
 
 @param pDemod The demodulator instance.
 @param pIFAGC The IFAGC output register value. Range 0 - 0xFFF. Unitless.

 @return SONYSAT_RESULT_OK if pIFAGC is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_IFAGCOut (sonysat_demod_t * pDemod,
                                                   uint32_t * pIFAGC);

/**
 @brief Monitor the System which current receiving.
 
 @param pDemod The demodulator instance.
 @param pSystem The system of current receiving signal.

 @return SONYSAT_RESULT_OK if pSystem is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_System (sonysat_demod_t * pDemod,
                                                 sonysat_dtv_system_t * pSystem);

/**
 @brief Monitor the symbol rate of the currently tuned channel in Sps (Symbols per Second).
 
 @param pDemod The demodulator instance.
 @param pSymbolRateSps The symbol rate in Sps (Symbols per Second).

 @return SONYSAT_RESULT_OK if pSymbolRateSps is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_SymbolRate (sonysat_demod_t * pDemod,
                                                     uint32_t * pSymbolRateSps);

/**
 @brief Monitors the I/Q sense used by the DVB-S/S2 demodulator.

 @param pDemod The demodulator instance.
 @param pSense The detected I/Q sense.

 @return SONYSAT_RESULT_OK if successful and pSense is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_IQSense (sonysat_demod_t * pDemod,
                                                  sonysat_demod_sat_iq_sense_t * pSense);

/**
 @brief Monitor the CNR estimation made by the demodulator.
 
 @param pDemod The demodulator instance.
 @param pCNR The returned CNR in dB x 1000.

 @return SONYSAT_RESULT_OK if pCNR is valid.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_CNR (sonysat_demod_t * pDemod,
                                              int32_t * pCNR);

/**
 @brief Monitor PER.

 @param pDemod The demodulator instance.
 @param pPER The returned PER in 1e6.

 @return SONYSAT_RESULT_OK if pPER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs_s2_monitor_PER (sonysat_demod_t * pDemod,
                                              uint32_t * pPER);

/**
 @brief Monitor the DVB-S Code Rate.

 @param pDemod The demodulator instance.
 @param pCodeRate Code rate.

 @return SONYSAT_RESULT_OK if pCodeRate is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs_monitor_CodeRate (sonysat_demod_t * pDemod,
                                                sonysat_dvbs_coderate_t * pCodeRate);

/**
 @brief Monitor pre viterbi BER.

 @param pDemod The demodulator instance.
 @param pBER The returned BER in 1e7.

 @return SONYSAT_RESULT_OK if pBER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs_monitor_PreViterbiBER (sonysat_demod_t * pDemod,
                                                     uint32_t * pBER);

/**
 @brief Monitor pre RS BER.

 @param pDemod The demodulator instance.
 @param pBER The returned BER in 1e7.

 @return SONYSAT_RESULT_OK if pBER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs_monitor_PreRSBER (sonysat_demod_t * pDemod,
                                                uint32_t * pBER);

/**
 @brief Monitor the DVB-S2 PLS code.

 @param pDemod The demodulator instance.
 @param pPLSCode The returned PLS code. The IC does not support short frames, 
                 so sonysat_dvbs2_plscode_t::isShortFrame is always set to 0:normal.

 @return SONYSAT_RESULT_OK if pPLSCode is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_PLSCode (sonysat_demod_t * pDemod,
                                                sonysat_dvbs2_plscode_t * pPLSCode);

/**
 @brief Monitor pre LDPC BER.

 @param pDemod The demodulator instance.
 @param pBER The returned BER in 1e7.

 @return SONYSAT_RESULT_OK if pBER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_PreLDPCBER (sonysat_demod_t * pDemod,
                                                   uint32_t * pBER);

/**
 @brief Monitor pre BCH BER.

 @param pDemod The demodulator instance.
 @param pBER The returned BER in 1e9.

 @return SONYSAT_RESULT_OK if pBER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_PreBCHBER (sonysat_demod_t * pDemod,
                                                  uint32_t * pBER);

/**
 @brief Monitor post BCH FER.

 @param pDemod The demodulator instance.
 @param pFER The returned FER in 1e6.

 @return SONYSAT_RESULT_OK if pFER is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_PostBCHFER (sonysat_demod_t * pDemod,
                                                   uint32_t * pFER);

/**
 @brief Monitor DVB-S2 base-band Header (BBHEADER).

 @param pDemod The demodulator instance.
 @param pBBHeader The returned base-band Header (BBHEADER) information.

 @return SONYSAT_RESULT_OK if pBBHeader is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_BBHeader (sonysat_demod_t * pDemod, 
                                                 sonysat_dvbs2_bbheader_t * pBBHeader);

/**
 @brief Monitor Rolloff parameter.

 @param pDemod The demodulator instance.
 @param pFER The returned pRolloff.

 @return SONYSAT_RESULT_OK if pRolloff is valid, otherwise an error is returned.
*/
sonysat_result_t sonysat_demod_dvbs2_monitor_Rolloff(sonysat_demod_t * pDemod,
                                                   uint8_t * pRolloff);                              

#endif /* SONYSAT_DEMOD_DVBS2_MONITOR_H */
