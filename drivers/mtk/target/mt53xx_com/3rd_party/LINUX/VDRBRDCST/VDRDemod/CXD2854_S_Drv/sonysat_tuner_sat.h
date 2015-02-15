/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_tuner_sat.h

          This file provides the tuner control interface specific to satellite.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_TUNER_SAT_H
#define SONYSAT_TUNER_SAT_H

#include "sonysat_common.h"
#include "sonysat_i2c.h"
#include "sonysat_dtv.h"

/**
 @brief Definition of the Satellite tuner driver API.
*/
typedef struct sonysat_tuner_sat_t {
    uint8_t i2cAddress;         /**< I2C address. */
    sonysat_i2c_t * pI2c;          /**< I2C driver instance. */
    uint32_t flags;             /**< Flags that can be used by tuner drivers. */
    uint32_t frequencyKHz;      /**< Current RF frequency(KHz) tuned. */
    sonysat_dtv_system_t system;   /**< Current system tuned. */
    uint32_t symbolRateKSps;    /**< Cuurent symbol rate(KSps) tuned. */

    /**
     @brief Initialize the tuner.

     @param pTuner Instance of the tuner driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*Initialize) (struct sonysat_tuner_sat_t * pTuner);

    /**
     @brief Tune to a given frequency with a given system and symbol rate.
            Tuner driver can block while waiting for PLL locked indication (if required).

     @param pTuner Instance of the tuner driver.
     @param centerFreqKHz RF frequency to tune too (kHz)
     @param system The type of channel to tune too (DVB-S/S2).
     @param symbolRateKSps The symbol rate of the channel in KHz.

     @return SONYSAT_RESULT_OK if tuner successfully tuned.
    */
    sonysat_result_t (*Tune) (struct sonysat_tuner_sat_t * pTuner,
                           uint32_t centerFreqKHz,
                           sonysat_dtv_system_t dtvSystem,
                           uint32_t symbolRateKSps);

    /**
     @brief Sleep the tuner device (if supported).
     
     @param pTuner Instance of the tuner driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*Sleep) (struct sonysat_tuner_sat_t * pTuner);

    /**
     @brief Shutdown the tuner device (if supported).
     
     @param pTuner Instance of the tuner driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*Shutdown) (struct sonysat_tuner_sat_t * pTuner);

    /**
     @brief Calculate AGC (dB * 100) from AGC level from demodulator.

     @param pTuner Instance of the tuner driver.
     @param AGCLevel The value from sonysat_demod_dvbs_monitor_IFAGCOut() API.
     @param pAGCdB Address of AGC (dB * 100) value.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*AGCLevel2AGCdB) (struct sonysat_tuner_sat_t * pTuner,
                                     uint32_t AGCLevel,
                                     int32_t * pAGCdB);

    void * user;                /**< User defined data. */

} sonysat_tuner_sat_t;

#endif /* SONYSAT_TUNER_SAT_H */
