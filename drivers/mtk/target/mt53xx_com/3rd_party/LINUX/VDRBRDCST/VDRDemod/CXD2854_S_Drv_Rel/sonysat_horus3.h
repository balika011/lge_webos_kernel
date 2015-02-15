/*------------------------------------------------------------------------------
  Copyright 2010-2013 Sony Corporation

  Last Updated  : 2013/05/15
  File Revision : 1.2.1.0
------------------------------------------------------------------------------*/
/**
 @file    sonysat_horus3.h

          This file provides the HORUS3 tuner control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONYSAT_HORUS3_H
#define SONYSAT_HORUS3_H

#include "sonysat_common.h"
#include "sonysat_i2c.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief Version of this driver.
*/
#define SONYSAT_HORUS3_VERSION     "1.2.1.0"

/**
 @brief Default I2C slave address of the HORUS3 tuner.
*/
#define SONYSAT_HORUS3_ADDRESS     0xC0

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/
/**
 @brief HORUS3 tuner state.
*/
typedef enum {
    SONYSAT_HORUS3_STATE_UNKNOWN,
    SONYSAT_HORUS3_STATE_SLEEP,
    SONYSAT_HORUS3_STATE_ACTIVE
} sonysat_horus3_state_t;

/**
 @brief Satellite broadcasting system definitions supported by HORUS3.
*/
typedef enum {
    SONYSAT_HORUS3_TV_SYSTEM_UNKNOWN,
    SONYSAT_HORUS3_STV_ISDBS,     /**< ISDB-S */
    SONYSAT_HORUS3_STV_DVBS,      /**< DVB-S */
    SONYSAT_HORUS3_STV_DVBS2      /**< DVB-S2 */
} sonysat_horus3_tv_system_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief The HORUS3 tuner definition which allows control of the HORUS3 tuner device 
        through the defined set of functions.
*/
typedef struct sonysat_horus3_t {
    uint32_t                 xtalFreqMHz;    /**< Xtal frequency for HORUS3 in MHz. */
    uint8_t                  i2cAddress;     /**< I2C slave address of the HORUS3 tuner (8-bit form - 8'bxxxxxxx0) */
    sonysat_i2c_t*              pI2c;           /**< I2C API instance. */
    uint32_t                 flags;          /**< ORed value of SONYSAT_HORUS3_CONFIG_XXXX */

    /* For saving current setting */
    sonysat_horus3_state_t      state;          /**< The driver operating state. */
    uint32_t                 frequencykHz;   /**< Currently RF frequency(kHz) tuned. */
    sonysat_horus3_tv_system_t  tvSystem;       /**< Current broadcasting system tuned. */
    uint32_t                 symbolRateksps; /**< Current symbol rate(ksym/s) tuned. */

    void*                    user;           /**< User defined data. */
} sonysat_horus3_t;

/*
 Config flag definitions. (ORed value should be set to flags argument of Create API.)
*/

/**
 @brief REFOUT output Off
*/
#define SONYSAT_HORUS3_CONFIG_REFOUT_OFF               0x80000000

/**
 @brief Keep Xtal enable in power save state.
        Should be used for Xtal shared system.
        Not only for the master HORUS3 which has Xtal, but also slave HORUS3 which receive the clock signal.
*/
#define SONYSAT_HORUS3_CONFIG_POWERSAVE_ENABLEXTAL     0x40000000

/**
 @brief Use external Xtal
        Should be used for slave HORUS3 in Xtal shared system.
*/
#define SONYSAT_HORUS3_CONFIG_EXT_REF                  0x20000000

/**
 @brief Internal LNA enable
*/
#define SONYSAT_HORUS3_CONFIG_LNA_ENABLE               0x10000000

/**
 @brief Stop internal LNA in power save state.
        This flag is ignored if SONYSAT_HORUS3_CONFIG_LNA_ENABLE is not used.
*/
#define SONYSAT_HORUS3_CONFIG_POWERSAVE_STOPLNA        0x08000000

/**
 @brief POWER SAVE pin is controlled by GPO pin.
        Should be used if controlling POWER SAVE pin from GPO pin in initialization.
*/
#define SONYSAT_HORUS3_CONFIG_POWERSAVE_BY_GPO         0x04000000

/**
 @name IQ output setting.
       One of following value should be used.
*/
/**@{*/
#define SONYSAT_HORUS3_CONFIG_IQOUT_DIFFERENTIAL       0x00000000  /**< IQ Output is Differential */
#define SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND          0x01000000  /**< IQ Output is Single-Ended (+4.4dB) */
#define SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND_LOWGAIN  0x02000000  /**< IQ Output is Single-Ended (-2.4dB) */
#define SONYSAT_HORUS3_CONFIG_IQOUT_MASK               0x03000000  /**< DON'T set this value. (Internal use only) */
/**@}*/

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/**
 @brief Set up the HORUS3 tuner driver.

        This MUST be called before calling sonysat_horus3_Initialize.

 @param pTuner      Reference to memory allocated for the HORUS3 instance.
                    The create function will setup this HORUS3 instance.
 @param xtalFreqMHz The frequency of the HORUS3 crystal.
 @param i2cAddress  The HORUS3 tuner I2C slave address in 8-bit form.
 @param pI2c        The I2C APIs that the HORUS3 driver will use as the I2C interface.
 @param flags       Configuration flags. It should be ORed value of SONYSAT_HORUS3_CONFIG_XXXX.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_horus3_Create(sonysat_horus3_t *pTuner, uint32_t xtalFreqMHz,
    uint8_t i2cAddress, sonysat_i2c_t *pI2c, uint32_t flags);

/**
 @brief Initialize the HORUS3 tuner.
        
        This MUST be called before calling sonysat_horus3_Tune.

 @param pTuner       The HORUS3 tuner instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_horus3_Initialize(sonysat_horus3_t *pTuner);

/**
 @brief Tune to a given RF frequency with broadcasting system.

 @param pTuner         The HORUS3 tuner instance.
 @param frequencykHz   RF frequency to tune. (kHz)
 @param tvSystem       The type of broadcasting system to tune.
 @param symbolRateksps Symbol rate to tune. (ksps)

 @return SONYSAT_RESULT_OK if tuner successfully tuned.
*/
sonysat_result_t sonysat_horus3_Tune(sonysat_horus3_t *pTuner, uint32_t frequencykHz,
    sonysat_horus3_tv_system_t tvSystem, uint32_t symbolRateksps);

/**
 @brief Put the HORUS3 tuner into Sleep state.
 
        From this state the tuner can be directly tuned by calling sonysat_horus3_Tune.

 @param pTuner       The HORUS3 tuner instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_horus3_Sleep(sonysat_horus3_t *pTuner);

/**
 @brief Write to GPO

 @param pTuner       The HORUS3 tuner instance.
 @param output       Output logic level, 0 = Low, 1 = High

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_horus3_SetGPO(sonysat_horus3_t *pTuner, uint8_t output);

#endif /* SONYSAT_HORUS3_H */
