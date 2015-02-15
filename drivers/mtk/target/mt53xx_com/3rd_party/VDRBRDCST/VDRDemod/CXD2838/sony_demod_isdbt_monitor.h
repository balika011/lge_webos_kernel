/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012-05-22
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONY_DEMOD_ISDBT_MONITOR_H
#define SONY_DEMOD_ISDBT_MONITOR_H

#include "sony_common.h"
#include "sony_demod_isdbt.h"

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/

typedef enum {
    SONY_ISDBT_MONITOR_TARGET_LAYER_A = 0,    /* A layer */
    SONY_ISDBT_MONITOR_TARGET_LAYER_B,        /* B layer */
    SONY_ISDBT_MONITOR_TARGET_LAYER_C,        /* C layer */
    SONY_ISDBT_MONITOR_TARGET_LAYER_UNKNOWN
} sony_isdbt_monitor_target_t;

/*------------------------------------------------------------------------------
  Function
------------------------------------------------------------------------------*/

/**
 @brief Monitor the IFAGC value of the demodulator.
        Actual dB gain dependent on attached tuner.

 @param pDemod The demodulator instance.
 @param pIFAGC The IFAGC output register value. Range 0x000 - 0xFFF. Unitless.

 @return SONY_RESULT_OK if pIFAGC is valid.
*/
sony_result_t sony_demod_isdbt_monitor_IFAGCOut(sony_demod_t* pDemod,
                                                UINT32*     pIFAGC);
/**
 @brief Monitors the synchronisation state of the demodulator.

 @param pDemod The demodulator instance.
 @param pDmdLock Address of demodulator lock flag
                 - 0: Not lock
                 - 1: Lock
 @param pTSLock  Indicates the TS lock condition.
                 - 0: TS not locked.
                 - 1: TS locked.
 @param pUnlock an early unlock condition
                 - 0: No early unlock.
                 - 1: Early unlock detected.

 @return SONY_RESULT_OK if pSyncStat, pTSLockStat is valid, otherwise an error is returned.
*/
sony_result_t sony_demod_isdbt_monitor_SyncStat(sony_demod_t* pDemod,
                                                UINT8*      pDmdLock,
                                                UINT8*      pTsLock,
                                                UINT8*      pUnlock);
/**
 @brief Monitors the spectrum sense used by the demodulator.

 @param pDemod The demodulator instance.
 @param pSense The detected spectrum sense.

 @return SONY_RESULT_OK if successful and pSense is valid.
*/
sony_result_t sony_demod_isdbt_monitor_SpectrumSense(sony_demod_t* pDemod,
                                                     UINT8*      pIsInvert);

/**
 @brief Monitor the carrier offset of the currently tuned channel.
        This uses the continual pilot (CP) carrier offset estimation
        from the device.
        To get the estimated center frequency of the current channel:
        Fest = Ftune (KHz) + pOffset (KHz) ;

 @param pDemod The demodulator instance.
 @param pOffset The detected carrier offset in KHz.

 @return SONY_RESULT_OK if pOffset is valid.
*/
sony_result_t sony_demod_isdbt_monitor_CarrierOffset(sony_demod_t* pDemod,
                                                     INT32*      pOffset);

/**
 @brief Monitor the SNR of the ISDB-T demodulator.

 @param pDemod The demodulator instance.
 @param pSNR The returned SNR in dB x 1000.

 @return SONY_RESULT_OK if pSNR is valid.
*/
sony_result_t sony_demod_isdbt_monitor_SNR(sony_demod_t* pDemod,
                                           UINT32*     pSNR);
/**
 @brief Monitor the detected mode/guard.

 @param pDemod The demodulator instance.
 @param pMode Mode estimation result.
 @param pGuard Guard interval estimation result.

 @return SONY_RESULT_OK if successful and pMode, pGuard valid.
*/
sony_result_t sony_demod_isdbt_monitor_ModeGuard(sony_demod_t*        pDemod,
                                                 sony_isdbt_mode_t*  pMode,
                                                 sony_isdbt_guard_t* pGuard);
/**
 @brief Monitor the Pre-RS BER.

 @param pDemod The demodulator instance.
 @param pBER BER value (Pre reed solomon decoder) x 1e7.

 @return SONY_RESULT_OK if successful and pBER valid.
*/
sony_result_t sony_demod_isdbt_monitor_PreRSBER(sony_demod_t*                pDemod,
                                                sony_isdbt_monitor_target_t target,
                                                UINT32*                    pBER);

/**
 @brief Monitor the ISDB-T PER (Packet Error Rate) parameters.

 @param pDemod The demod instance.
 @param pPER The estimated PER x 1e6.
 
 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_PER(sony_demod_t*                pDemod,
                                           sony_isdbt_monitor_target_t target,
                                           UINT32*                    pPER);

/**
  @brief Monitor the TMCC informations

  @param pDemod    : pDemod the demod instance.
  @param pTMCCInfo : pTMCCInfo  TMCC information.

  @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_TMCCInfo(sony_demod_t*            pDemod,
                                                sony_isdbt_tmcc_info_t* pTMCCInfo);

/**
  @brief Monitor the preset informations

  @param pDemod    : pDemod the demod instance.
  @param pTMCCInfo : pPresetInfo  preset information.

  @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_PresetInfo(sony_demod_t* pDemod,
                                                  sony_demod_isdbt_preset_info_t* pPresetInfo);

/**
 @brief Monitors the number RS (Reed Solomon) errors detected by the 
        RS decoder over 1 second. Also known as the code word reject count.

 @param pDemod The demodulator instance.
 @param pPEN The number of RS errors detected over 1 second.

 @return SONY_RESULT_OK if successful and pPEN valid.
*/
sony_result_t sony_demod_isdbt_monitor_PacketErrorNumber(sony_demod_t*               pDemod,
                                                         sony_isdbt_monitor_target_t target,
                                                         UINT32*                   pPacketErrorNum);

/**
  @brief Monitor the SCEEW informations

  @param pDemod  pDemod the demod instance.
  @param pIsExist ACEEW exist flag.
                  If you get "0", ACEEW information is not exist.
                  If you get "1", ACEEW information is exist, please check pACEEWInfo.
  @param pACEEWInfo ACEEW Informations

  @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_monitor_ACEEWInfo(sony_demod_t*            pDemod,
                                                 UINT8*                 pIsExist,
                                                 sony_isdbt_aceew_info_t* pACEEWInfo);

#endif  /*SONY_DEMOD_ISDBT_MONITOR_H*/
