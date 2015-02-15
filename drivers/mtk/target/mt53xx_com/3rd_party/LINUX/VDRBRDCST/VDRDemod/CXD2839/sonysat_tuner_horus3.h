/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_tuner_horus3.h

          This file provides the tuner control function for HORUS3(CXD2832).
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_TUNER_HORUS3_H
#define SONYSAT_TUNER_HORUS3_H

#include "sonysat_common.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_horus3.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create a instance for HORUS3 driver.

 @param pTuner The tuner driver instance to create.
        Memory must have been allocated for this instance before creation.
 @param xtalFreqMHz The crystal frequency of the tuner (MHz). 
        Supports 16MHz or 24MHz or 27MHz.
 @param i2cAddress The I2C address of the HORUS3 device.
        Typically 0xC0.
 @param pI2c The I2C driver that the tuner driver will use for 
        communication.
 @param configFlags See ::SONYSAT_HORUS3_CONFIG_REFOUT_OFF,
                        ::SONYSAT_HORUS3_CONFIG_POWERSAVE_ENABLEXTAL,
                        ::SONYSAT_HORUS3_CONFIG_EXT_REF,
                        ::SONYSAT_HORUS3_CONFIG_LNA_ENABLE,
                        ::SONYSAT_HORUS3_CONFIG_POWERSAVE_STOPLNA,
                        ::SONYSAT_HORUS3_CONFIG_IQOUT_DIFFERENTIAL,
                        ::SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND,
                        ::SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND_LOWGAIN defined in
                        \link sonysat_horus3.h \endlink
 @param pHorus3 The HORUS3 tuner driver pointer to use. 
        Memory must have been allocated for the HORUS3 driver structure.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_tuner_horus3_Create (sonysat_tuner_sat_t * pTuner,
                                        uint32_t xtalFreqMHz,
                                        uint8_t i2cAddress,
                                        sonysat_i2c_t * pI2c,
                                        uint32_t configFlags,
                                        sonysat_horus3_t * pHorus3);

#endif /* SONYSAT_TUNER_HORUS3_H */
