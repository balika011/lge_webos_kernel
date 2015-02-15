/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_lnbc.h

          This file provides the LNB controller control interface.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_LNBC_H
#define SONYSAT_LNBC_H

#include "sonysat_common.h"
#include "sonysat_i2c.h"

/**
 @brief Definition of the state of the driver.
*/
typedef enum {
    SONYSAT_LNBC_STATE_UNKNOWN,    /**< Unknown */
    SONYSAT_LNBC_STATE_SLEEP,      /**< Sleep */
    SONYSAT_LNBC_STATE_ACTIVE      /**< Active */
} sonysat_lnbc_state_t;

/**
 @brief Definition of the voltage.
*/
typedef enum {
    SONYSAT_LNBC_VOLTAGE_LOW,      /**< Low voltage. */
    SONYSAT_LNBC_VOLTAGE_HIGH,     /**< High voltage. */
    SONYSAT_LNBC_VOLTAGE_AUTO,     /**< Control voltage by demodulator. (For single cable) */
} sonysat_lnbc_voltage_t;

/**
 @brief Definition of the tone state.
*/
typedef enum {
    SONYSAT_LNBC_TONE_OFF,         /**< Not output tone signal. */
    SONYSAT_LNBC_TONE_ON,          /**< Output tone signal. */
    SONYSAT_LNBC_TONE_AUTO,        /**< Control tone by demodulator. (Default) */
} sonysat_lnbc_tone_t;

/**
 @brief Transmit mode.
*/
typedef enum {
    SONYSAT_LNBC_TRANSMIT_MODE_TX,     /**< TX mode. */
    SONYSAT_LNBC_TRANSMIT_MODE_RX,     /**< RX mode. */
    SONYSAT_LNBC_TRANSMIT_MODE_AUTO,   /**< Control transmit mode by demodulator. (For receiving DiSEqC2.x reply) */
} sonysat_lnbc_transmit_mode_t;

/**
 @brief Config ID for SetConfig API.
*/
typedef enum {
    SONYSAT_LNBC_CONFIG_ID_TONE_INTERNAL,  /**< Tone mode (0: External tone, 1: Internal tone). */
    SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE,    /**< Definition of voltage for "Low voltage". */
    SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE    /**< Definition of voltage for "High voltage". */
} sonysat_lnbc_config_id_t;

/**
 @brief Definition of the LNB controller driver API.
*/
typedef struct sonysat_lnbc_t {
    uint8_t i2cAddress;                     /**< I2C address. */
    sonysat_i2c_t * pI2c;                      /**< I2C driver instance. */
    sonysat_lnbc_state_t state;                /**< The state of this driver */
    sonysat_lnbc_voltage_t voltage;            /**< Current voltage. */
    sonysat_lnbc_tone_t tone;                  /**< Current tone status. */
    sonysat_lnbc_transmit_mode_t transmitMode; /**< Current transmit mode. */

    int32_t isInternalTone;                 /**< Tone mode flag.(0: External tone, 1: Internal tone) */
    int32_t lowVoltage;                     /**< Definition of low voltage. */
    int32_t highVoltage;                    /**< Definition of high voltage. */

    /**
     @brief Initialize the LNB controller.

     When user call this API, the LNB controller's state is following.
       - Voltage      : Low
       - Tone         : Auto (Control tone by demodulator)
       - TransmitMode : TX mode

     @param pLnbc Instance of the LNB controller driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*Initialize) (struct sonysat_lnbc_t * pLnbc);

    /**
     @brief Configure to LNB controller

     The parameters set by this API are initialized to default value by Initialize API.

     @param pLnbc Instance of the LNB controller driver.
     @param configId Configure parameter ID.
     @param value The parameter value.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*SetConfig) (struct sonysat_lnbc_t * pLnbc, sonysat_lnbc_config_id_t configId, int32_t value);

    /**
     @brief Set voltage.

     @param pLnbc Instance of the LNB controller driver.
     @param voltage Voltage to set.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*SetVoltage) (struct sonysat_lnbc_t * pLnbc, sonysat_lnbc_voltage_t voltage);

    /**
     @brief Set tone mode.

     @param pLnbc Instance of the LNB controller driver.
     @param tone Tone mode.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*SetTone) (struct sonysat_lnbc_t * pLnbc, sonysat_lnbc_tone_t tone);

    /**
     @brief Set transmit mode.

     @param pLnbc Instance of the LNB controller driver.
     @param mode Transmit mode.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*SetTransmitMode) (struct sonysat_lnbc_t * pLnbc, sonysat_lnbc_transmit_mode_t mode);

    /**
     @brief Sleep the LNB controller.

     @param pLnbc Instance of the LNB controller driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*Sleep) (struct sonysat_lnbc_t * pLnbc);

    /**
     @brief Return from sleep the LNB controller.

     When user call this API, the LNB controller's state is kept before sleep.

     @param pLnbc Instance of the LNB controller driver.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*WakeUp) (struct sonysat_lnbc_t * pLnbc);

    /**
     @brief Monitor Overload status.

     @param pLnbc Instance of the LNB controller driver.
     @param pIsDetect Address of the status.
                        - 0: Not detect.
                        - 1: Detect.

     @return SONYSAT_RESULT_OK if successful.
    */
    sonysat_result_t (*MonitorOverload) (struct sonysat_lnbc_t * pLnbc, uint8_t * pIsDetect);

    /**
     @brief User defined data.
    */
    void * user;

} sonysat_lnbc_t;

#endif /* SONYSAT_LNBC_H */
