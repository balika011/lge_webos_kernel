/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_isdbt_monitor.h

          This file provides the ISDB-T demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DEMOD_ISDBT_MONITOR_H
#define SONY_DEMOD_ISDBT_MONITOR_H

#include "sony_demod.h"
#include "sony_isdbt.h"

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Target layer for monitoring BER, PER and so on.
*/
typedef enum sony_demod_isdbt_monitor_target_t{
    SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A = 0,    /**< Layer A */
    SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B,        /**< Layer B */
    SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C,        /**< Layer C */
    SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_UNKNOWN   /**< Unkown Layer */
} sony_demod_isdbt_monitor_target_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Monitors the synchronisation state of the demodulator.

 @param pDemod   The demodulator instance.
 @param pDmdLockStat Address of demodulator lock flag
                 - 0: Not lock
                 - 1: Lock
 @param pTSLockStat  Indicates the TS lock condition.
                 - 0: TS not locked.
                 - 1: TS locked.
 @param pUnlockDetected  Early unlock condition
                 - 0: No early unlock.
                 - 1: Early unlock detected.

 @return SONY_RESULT_OK if successful and pSyncStat, pTSLockStat valid.
*/
sony_result_t sony_demod_isdbt_monitor_SyncStat (sony_demod_t * pDemod,
                                                 uint8_t * pDmdLockStat,
                                                 uint8_t * pTSLockStat,
                                                 uint8_t * pUnlockDetected);

/**
 @brief Monitor the IFAGC value.
        Actual dB gain dependent on attached tuner.

 @param pDemod The demodulator instance.
 @param pIFAGCOut The IFAGC output register value. Range 0x000 - 0xFFF. Unitless.

 @return SONY_RESULT_OK if successful and pIFAGCOut valid.
*/
sony_result_t sony_demod_isdbt_monitor_IFAGCOut (sony_demod_t * pDemod,
                                                 uint32_t * pIFAGCOut);

/**
 @brief Monitors the detected carrier offset of the currently tuned channel.

        To get the estimated center frequency of the current channel:
        Freq_Est = Freq_Tune + pOffset;

        This function will compensate for an inverting tuner architecture if the
        demodulator has been configured accordignly using ::SONY_DEMOD_CONFIG_SPECTRUM_INV
        config option for ::sony_demod_SetConfig.

 @param pDemod The demodulator instance.
 @param pOffset Carrier offset value (Hz).

 @return SONY_RESULT_OK if successful and pOffset valid.
*/
sony_result_t sony_demod_isdbt_monitor_CarrierOffset (sony_demod_t * pDemod,
                                                      int32_t * pOffset);

/**
 @brief Monitor the Pre-RS BER.

 @param pDemod The demodulator instance.
 @param target  Target layer of BER estimation.
 @param pBER BER value (Pre reed solomon decoder) x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_demod_isdbt_monitor_PreRSBER (sony_demod_t * pDemod,
                                                 sony_demod_isdbt_monitor_target_t target,
                                                 uint32_t * pBER);

/**
 @brief Monitor the TMCC information.

 @param pDemod The demodulator instance.
 @param pTMCCInfo The TMCC information.

 @return SONY_RESULT_OK if successful and pInfo valid.
*/
sony_result_t sony_demod_isdbt_monitor_TMCCInfo (sony_demod_t * pDemod,
                                                 sony_isdbt_tmcc_info_t * pTMCCInfo);

/**
  @brief Monitor the TMCC preset information.
  
        This information enable demodulator fast acquisition by passing to
        ::sony_demod_isdbt_SetPreset. Tuning time will be shortened.

  @param pDemod The demodulator instance.
  @param pPresetInfo The TMCC Preset information struct instance.

  @return SONY_RESULT_OK if successful and pPresetInfo is valid.
*/
sony_result_t sony_demod_isdbt_monitor_PresetInfo(sony_demod_t * pDemod,
                                                  sony_demod_isdbt_preset_info_t * pPresetInfo);

/**
 @brief Monitors the number RS (Reed Solomon) errors detected by the
        RS decoder over 1 second. Also known as the code word reject count.

 @param pDemod The demodulator instance.
 @param target  Target layer of PacketErrorNumber estimation.
 @param pPEN The number of RS errors detected over 1 second.

 @return SONY_RESULT_OK if successful and pPEN valid.
*/
sony_result_t sony_demod_isdbt_monitor_PacketErrorNumber (sony_demod_t * pDemod,
                                                          sony_demod_isdbt_monitor_target_t target,
                                                          uint32_t * pPEN);

/**
 @brief Monitors the channel spectrum sense.  To ensure correct polarity detection
        please use the ::SONY_DEMOD_CONFIG_SPECTRUM_INV config option in
        ::sony_demod_SetConfig to select the appropriate spectrum inversion
        for the tuner output.

 @param pDemod The demodulator instance.
 @param pSense Indicates the spectrum sense.

 @return SONY_RESULT_OK if successful and pSense is valid.
*/
sony_result_t sony_demod_isdbt_monitor_SpectrumSense (sony_demod_t * pDemod,
                                                      sony_demod_terr_cable_spectrum_sense_t * pSense);

/**
 @brief Monitors the estimated SNR value.

 @param pDemod The demodulator instance.
 @param pSNR The estimated SNR in dBx1000.

 @return SONY_RESULT_OK if successful and pSNR is valid.
*/
sony_result_t sony_demod_isdbt_monitor_SNR (sony_demod_t * pDemod,
                                            int32_t * pSNR);

/**
 @brief Monitor the detected mode/guard.

 @param pDemod  The demodulator instance.
 @param pMode   Mode estimation result.
 @param pGuard  Guard interval estimation result.

 @return SONY_RESULT_OK if successful and pMode, pGuard are valid.
*/
sony_result_t sony_demod_isdbt_monitor_ModeGuard(sony_demod_t * pDemod,
                                                 sony_isdbt_mode_t * pMode,
                                                 sony_isdbt_guard_t * pGuard);

/**
 @brief Monitor the sampling frequency offset value.

 @param pDemod The demodulator instance.
 @param pPPM The sampling frequency offset in ppm x 100.
             Range: +/- 220ppm.

 @return SONY_RESULT_OK if pPPM is valid.
*/
sony_result_t sony_demod_isdbt_monitor_SamplingOffset (sony_demod_t * pDemod,
                                                       int32_t * pPPM);

/**
 @brief Monitor the PER (Packet Error Rate) parameters.

 @param pDemod The demod instance.
 @param target  Target layer of PER estimation.
 @param pPER The estimated PER value (Post reed solomon decoder) x 1e6.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_PER (sony_demod_t * pDemod,
                                            sony_demod_isdbt_monitor_target_t target,
                                            uint32_t * pPER);

/**
 @brief Calculate the ISDB-T TS (Transport Stream) rate from TMCC and guard interval information.
        
 @param pDemod The demodulator instance.
 @param target  Target layer of TS rate calculation.
 @param pTsRateKbps The calculated TS rate in kbps.

 @return SONY_RESULT_OK if pTsRateKbps is valid.
*/
sony_result_t sony_demod_isdbt_monitor_TsRate (sony_demod_t * pDemod,
                                               sony_demod_isdbt_monitor_target_t target,
                                               uint32_t * pTsRateKbps);

/**
  @brief Monitor the AC EEW (Earthquake Early Warning by AC signal) informations

  @param pDemod     pDemod the demod instance.
  @param pIsExist   ACEEW exist flag.
                    If you get "0", ACEEW information does not exist.
                    If you get "1", ACEEW information exists, please check pACEEWInfo.
  @param pACEEWInfo ACEEW Information struct instance.

  @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_ACEEWInfo(sony_demod_t * pDemod,
                                                 uint8_t * pIsExist,
                                                 sony_isdbt_aceew_info_t * pACEEWInfo);

#endif /* SONY_DEMOD_ISDBT_MONITOR_H */
