/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2.h

          This file provides the demodulator control interface specific to DVB-S/S2.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_H
#define SONYSAT_DEMOD_DVBS_S2_H

#include "sonysat_common.h"
#include "sonysat_demod.h"

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The Tune parameters for a DVB-S/S2 signal.
*/
typedef struct {
    sonysat_dtv_system_t system;       /**< System of the channel. */
    uint32_t centerFreqKHz;         /**< Center frequency in kHz of the DVB-S/S2 channel. */
    uint32_t symbolRateKSps;        /**< Symbol rate in kHz of the DVB-S/S2 channel. */
} sonysat_dvbs_s2_tune_param_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/**
 @brief Enable acquisition on the demodulator for DVB-S/S2 channels.
        Called from the integration layer ::sonysat_integ_dvbs_s2_Tune API.

 @param pDemod The demodulator instance
 @param pTuneParam The tune parameters.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_Tune (sonysat_demod_t * pDemod,
                                       sonysat_dvbs_s2_tune_param_t * pTuneParam);

/**
 @brief Put the demodulator into ::SONYSAT_DEMOD_STATE_SLEEP_S state.
        Can be called from Active, Shutdown or Sleep states.
        Called from the integration layer ::sonysat_integ_SleepS API.

 @param pDemod  The demodulator instance

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_Sleep (sonysat_demod_t * pDemod);

/**
 @brief Check whether TS signal is locked or not locked.

 @param pDemod  The demodulator instance
 @param pLock   Address of lock status.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_CheckTSLock (sonysat_demod_t * pDemod,
                                              sonysat_demod_lock_result_t * pLock);

/**
 @brief Get the I/Q polarity of device.

        This API is called by driver internal.
        So user doesn't need to use it.

 @param pDemod    The demodulator instance
 @param pIsInvert Polarity of I/Q sense. (0: Not inverted. 1: Inverted)

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_CheckIQInvert (sonysat_demod_t * pDemod,
                                                uint8_t * pIsInvert);

/**
 @brief Configure demodulator setting to demodulate symbol rate.

 @param pDemod         The demodulator instance
 @param symbolRateKSps Symbol rate in KSps.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_SetSymbolRate (sonysat_demod_t * pDemod,
                                                uint32_t symbolRateKSps);

#endif /* SONYSAT_DEMOD_DVBS_S2_H */
