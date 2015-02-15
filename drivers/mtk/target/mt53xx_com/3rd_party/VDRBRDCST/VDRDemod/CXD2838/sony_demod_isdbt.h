/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012-06-07
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/


#ifndef SONY_DEMOD_ISDB_T_H
#define SONY_DEMOD_ISDB_T_H

#include "sony_common.h"
#include "sony_i2c.h"
#include "sony_stdlib.h"
#include "sony_isdbt.h"

/**
 @brief Calculate the demodulator IF Freq setting ::sony_demod_t::iffreqConfig.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5
*/
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG_16000KHz(iffreq) ((UINT32)(((iffreq)/48.0)*16777216.0 + 0.5))
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG_20500KHz(iffreq) ((UINT32)(((iffreq)/41.0)*16777216.0 + 0.5))
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG_24000KHz(iffreq) ((UINT32)(((iffreq)/48.0)*16777216.0 + 0.5))
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG_27000KHz(iffreq) ((UINT32)(((iffreq)/48.9375)*16777216.0 + 0.5))
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG_41000KHz(iffreq) ((UINT32)(((iffreq)/41.0)*16777216.0 + 0.5))

#define SONY_DEMOD_MAX_CONFIG_MEMORY_COUNT 100 /** The maximum number of entries in the configuration memory table */

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Demodulator crystal frequency.
*/
typedef enum {
    SONY_DEMOD_XTAL_16000KHz = 0,      /**< 16 MHz */
    SONY_DEMOD_XTAL_20500KHz,          /**< 20.5 MHz */
    SONY_DEMOD_XTAL_24000KHz,          /**< 24 MHz */
    SONY_DEMOD_XTAL_27000KHz,          /**< 27 MHz */
    SONY_DEMOD_XTAL_41000KHz           /**< 41 MHz */
} sony_demod_xtal_t;


/**
 @brief The demodulator Chip ID mapping.
*/
typedef enum {
    SONY_DEMOD_CHIP_ID_CXD2828 = 0x22,
    SONY_DEMOD_CHIP_ID_CXD2838 = 0xB0
} sony_demod_chip_id_t;


typedef enum {
    SONY_DEMOD_STATE_UNKNOWN = 0,   /**< Unknown. */
    SONY_DEMOD_STATE_SHUTDOWN,      /**< Chip is in Shutdown state */
    SONY_DEMOD_STATE_SLEEP,         /**< Chip is in Sleep state */
    SONY_DEMOD_STATE_ACTIVE_T,      /**< Chip is in Active state */
    SONY_DEMOD_STATE_EWS,           /**< Chip is in EWS state */
    SONY_DEMOD_STATE_INVALID        /**< Invalid, result of an error during a state change. */
} sony_demod_state_t;


typedef enum {
    /**
     @brief Parallel or serial TS output selection.

            Value:
            - 0: Serial output.
            - 1: Parallel output (Default).
    */
    SONY_DEMOD_CONFIG_PARALLEL_SEL,

    /**
     @brief Serial output pin of TS data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
    SONY_DEMOD_CONFIG_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TS data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial):
            - 0: LSB first
            - 1: MSB first (Default).
    */
    SONY_DEMOD_CONFIG_OUTPUT_SEL_MSB,

    /**
     @brief TS valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONY_DEMOD_CONFIG_TSVALID_ACTIVE_HI,

    /**
     @brief TS sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONY_DEMOD_CONFIG_TSSYNC_ACTIVE_HI,

    /**
     @brief TS error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONY_DEMOD_CONFIG_TSERR_ACTIVE_HI,

    /**
     @brief TS clock inversion setting.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
    SONY_DEMOD_CONFIG_LATCH_ON_POSEDGE,

    /**
     @brief TS clock gated on valid TS data or is continuous.

            Value:
            - 0: Continuous (Default)
            - 1: Gated
    */
    SONY_DEMOD_CONFIG_TSCLK_CONT,

    /**
     @brief Disable/Enable TS clock during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 2 : Disable during TS parity
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONY_DEMOD_CONFIG_TSCLK_MASK,

    /**
     @brief Disable/Enable TSVALID during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active 
            - 1 : Disable during TS packet gap (default)
            - 2 : Disable during TS parity (default)
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONY_DEMOD_CONFIG_TSVALID_MASK,

    /**
     @brief Disable/Enable TSERR during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 2 : Disable during TS parity
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONY_DEMOD_CONFIG_TSERR_MASK,

    /**
     @brief Configure the driving curent for the TS Clk pin.

            - 0 : 8mA (Default)
            - 1 : 10mA
    */
    SONY_DEMOD_CONFIG_TSCLK_CURRENT_10mA,

    /**
     @brief Configure the driving curent for the TS Sync / TS Valid 
            / TS Data / TS Error pins.

            - 0 : 8mA (Default)
            - 1 : 10mA
    */
    SONY_DEMOD_CONFIG_TS_CURRENT_10mA,

    /**
     @brief IFAGC sense configuration.

            Value:
            - 0: Positive IFAGC.
            - 1: Inverted IFAGC (Default)
    */
    SONY_DEMOD_CONFIG_IFAGCNEG,

    /**
    @brief Writes a 12-bit value to the PWM output.
        Please note the actual PWM precision.
        - 12-bit.
        0xFFF => DVDD
        0x000 => GND
        
        This configuration is available only while the device
        is in ACTIVE mode.
    */
    SONY_DEMOD_CONFIG_PWM_VALUE,

     /**
     @brief Spectrum inversion configuration for the terrestrial / cable tuner. 
            
            Value: 
            - 0: Normal (Default).
            - 1: Inverted.
    */
    SONY_DEMOD_CONFIG_SPECTRUM_INV,

    /**
     @brief Set Shared IF mode. Shared IF mode is available once
            demodulator is in sleep mode (::SONY_DEMOD_STATE_SLEEP_T_C).
            Please change this setting at driver initilisation.

            Value:
            - 0: Shared IF mode disabled (Default).
            - 1: Shared IF mode enabled.
    */
    SONY_DEMOD_CONFIG_SHARED_IF,

    /**
     @brief Set Number of Measured Packets - 15bit
    */
    SONY_DEMOD_CONFIG_BERPER_PERIOD,
    SONY_DEMOD_CONFIG_UNKNOWN
} sony_demod_config_t;

/**
 @brief Demodulator lock status.
*/
typedef enum {
    SONY_DEMOD_LOCK_RESULT_NOT_DETECT = 0, /**< "Lock" or "Unlock" conditions not met */
    SONY_DEMOD_LOCK_RESULT_LOCK,           /**< "Lock" condition is found. */
    SONY_DEMOD_LOCK_RESULT_UNLOCK          /**< No signal was found or the signal was not the required system. */
} sony_demod_lock_result_t;

/**
 @brief Enumeration of supported sony tuner models used for optimising the 
        demodulator configuration.
*/
typedef enum {
    SONY_DEMOD_TUNER_OPTIMIZE_UNKNOWN = 0,  /**< Non-Sony Tuner. */
    SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D,      /**< Sony ASCOT2D derived tuners. */
    SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E       /**< Sony ASCOT2E derived tuners. */
} sony_demod_tuner_optimize_t;

/**
 @brief Mode select for the multi purpose GPIO pins
*/
typedef enum {
    /** @brief GPIO pin is configured as an output */
    SONY_DEMOD_GPIO_MODE_OUTPUT = 0x00, 

    /** @brief GPIO pin is configured as an input */
    SONY_DEMOD_GPIO_MODE_INPUT = 0x01, 
        
    /** 
     @brief GPIO pin is configured to output an PWM signal which can be configured using the 
            ::sony_demod_SetConfig function with the config ID ::SONY_DEMOD_CONFIG_PWM_VALUE.
    */
    SONY_DEMOD_GPIO_MODE_PWM = 0x03, 

    /** @brief GPIO pin is configured to output TS error */
    SONY_DEMOD_GPIO_MODE_TS_ERROR = 0x04, 

    /** @brief GPIO pin is configured to EWS flag */
    SONY_DEMOD_GPIO_MODE_EWS_FLAG = 0x06
} sony_demod_gpio_mode_t;


/**
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
    SONY_DEMOD_TERR_CABLE_SPECTRUM_NORMAL = 0,             /**< Spectrum normal sense. */
    SONY_DEMOD_TERR_CABLE_SPECTRUM_INV                     /**< Spectrum inverted. */
} sony_demod_terr_cable_spectrum_sense_t;

/*------------------------------------------------------------------------------
  Struct
------------------------------------------------------------------------------*/
/**
 @brief List of register values for IF frequency configuration.  Used for handling 
        tuners that output a different IF depending on the expected channel BW.
        Should be set using ::SONY_DEMOD_MAKE_IFFREQ_CONFIG_XXX macro.
*/
typedef struct sony_demod_iffreq_config_t {
    UINT32 configIsdbt6;   /**M< ISDB-T 6MHz */
    UINT32 configIsdbt7;   /**M< ISDB-T 7MHz */
    UINT32 configIsdbt8;   /**M< ISDB-T 8MHz */
} sony_demod_iffreq_config_t;

/**
 @brief The demodulator configuration memory table entry. Used to store a register or
        bit modification made through either the ::sony_demod_SetConfig or 
        ::sony_demod_SetAndSaveRegisterBits APIs.
*/
typedef struct {
    UINT8 slaveAddress;               /**< Slave address of register */
    UINT8 bank;                       /**< Bank for register */
    UINT8 registerAddress;            /**< Register address */
    UINT8 value;                      /**< Value to write to register */
    UINT8 bitMask;                    /**< Bit mask to apply on the value */
} sony_demod_config_memory_t;

/**
 @brief The demodulator definition which allows control of the demodulator device 
        through the defined set of functions. This portion of the driver is seperate 
        from the tuner portion and so can be operated independently of the tuner.
*/
typedef struct sony_demod_t {
    sony_demod_xtal_t                      xtalFreq;
    sony_demod_state_t                     state;
    sony_demod_iffreq_config_t             iffreqConfig;
    UINT8                                i2cAddressSLVT;
    UINT8                                i2cAddressSLVX;
    sony_i2c_t*                            pI2c;
    UINT8                                bwMHz;
    UINT8                                sharedIf;
    sony_demod_tuner_optimize_t            tunerOptimize;
    sony_demod_config_memory_t             configMemory[SONY_DEMOD_MAX_CONFIG_MEMORY_COUNT];
    UINT8                                configMemoryLastEntry;
    sony_demod_terr_cable_spectrum_sense_t confSense;
} sony_demod_t;

/**
 @brief The tune parameters for a ISDB-T signal
*/
typedef struct sony_isdbt_tune_param_t {
    UINT32 centerFreqKHz;
    UINT8  bwMHz;
} sony_isdbt_tune_param_t;

/**
 @brief The preset information for a ISDB-T signal
*/
typedef struct sony_demod_isdbt_preset_info_t {
    UINT8 data[13];
} sony_demod_isdbt_preset_info_t;


/*------------------------------------------------------------------------------
  Function
------------------------------------------------------------------------------*/

/**
  @brief Set up the demodulator.
         This function Must be called before calling sony_demod_Initialize.
  @param pDemod Reference to memory allocated for the demodulator instance.
  @param xtalFreq The frequency of the demod crystal.
  @param i2cAddress The demod I2C address in 8-bit form.
  @param pDemodI2c The I2C driver that the demod will use as the I2C interface.
 */
sony_result_t sony_demod_Create(sony_demod_t*     pDemod,
                                sony_demod_xtal_t xtalFreq,
                                UINT8           i2cAddress,
                                sony_i2c_t*       pDemodI2c);

/**
 @brief Initialize the demodulator.
        Can also be used to reset the demodulator from any state back to 
        ::SONY_DEMOD_STATE_SLEEP.  Please note this will reset all demodulator registers
        clearing any configuration settings.

        This API also clears the demodulator configuration memory table.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
  */
sony_result_t sony_demod_Initialize(sony_demod_t* pDemod);

/**
 @brief Put the demodulator into Sleep state.  From
        this state the demodulator can be directly tuned.

        If currently in ::SONY_DEMOD_STATE_SHUTDOWN the configuration memory will be loaded
        back into the demodulator.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_Sleep(sony_demod_t* pDemod);

/**
 @brief Shutdown the demodulator.

        The device is placed in "Shutdown" state.
        sony_demod_Sleep must be called to re-initialise the
        device and driver for future acquisitions.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_demod_Shutdown(sony_demod_t* pDemod);

/**
 @brief Soft reset the demodulator.
        The soft reset will begin the devices acquisition process.

 @param pDemod The demod instance.

 @return SONY_RESULT_OK if successfully reset.
*/
sony_result_t sony_demod_SoftReset(sony_demod_t * pDemod);

/**
 @brief Completes the demodulator acquisition setup.
        Must be called after system specific demod and RF tunes.

 @param pDemod The demodulator instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_TuneEnd(sony_demod_t * pDemod);

/**
 @brief Set configuration options on the demodulator.

 @param pDemod The demodulator instance.
 @param config The configuration ID to set. See ::sony_demod_config_id_t.
 @param value The associated value. Depends on the configId.

 @return SONY_RESULT_OK if successfully set the configuration option.
*/
sony_result_t sony_demod_SetConfig(sony_demod_t*       pDemod,
                                   sony_demod_config_t config,
                                   UINT32            value);


/**
 @brief Setup the GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_GPIOSetConfig(sony_demod_t*          pDemod,
                                       UINT8                id, 
                                       UINT8                enable,
                                       sony_demod_gpio_mode_t mode);

/**
 @brief Read the GPIO value.
        The GPIO should have been configured as an input (Read) GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param pValue The current value of the GPIO.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_GPIORead(sony_demod_t* pDemod,
                                  UINT8       id,
                                  UINT8*      pValue);

/**
 @brief Write the GPIO value.
        The GPIO should have been configured as an output (Write) GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param value The value to set as output.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_GPIOWrite(sony_demod_t* pDemod,
                                   UINT8       id,
                                   UINT8       value);

/**
 @brief Get the Chip ID of the connected demodulator.
 
 @param pDemod The demodulator instance.
 @param pChipId Pointer to receive the IP ID into.

 @return SONY_RESULT_OK if pChipId is valid.
*/
sony_result_t sony_demod_ChipID(sony_demod_t*         pDemod,
                                sony_demod_chip_id_t* pChipId);

/**
 @brief Configure the demodulator to forward I2C messages to the
        output port for tuner control.

 @param pDemod The demodulator instance.
 @param enable Enable(enable != 0) / Disable I2C repeater(enable == 0)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_I2cRepeaterEnable(sony_demod_t* pDemod,
                                           UINT8       enable);

/* For ISDB-T*/

/**
 @brief Set Preset Infomation

 @param pDemod The demodulator instance.
 @param preset infomation

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_SetPreset(sony_demod_t*                   pDemod,
                                         sony_demod_isdbt_preset_info_t* pPresetInfo);

/**
 @brief 

 @param pDemod The demodulator instance.
 @param preset infomation

 @return SONY_RESULT_OK if successful.
*/

/**
 @brief Enable acquisition on the demodulator for ISDB-T channels.  Called from
        the integration layer ::sony_integ_dvbc_Tune API.

 @param pDemod  The demodulator instance
 @param pTunerParam Tune parameters structure.

 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_demod_isdbt_Tune(sony_demod_t*      pDemod,
                                    sony_isdbt_tune_param_t* pTuneParam);
/**
 @brief   Set EWS tuning parameter to ISDB-T demod part.

 @param pDemod  The demodulator instance
 @param pTunerParam Tune parameters structure.
  
 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_demod_isdbt_EWSTune(sony_demod_t*            pDemod,
                                       sony_isdbt_tune_param_t* pTuneParam);


/**
@brief Check ISDB-T demodulator lock status.

 @param pDemod The demodulator instance
 @param pLock Demod lock state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_CheckDemodLock(sony_demod_t*             pDemod,
                                              sony_demod_lock_result_t* pLock);

/**
 @brief Check ISDB-T demodulator lock status.

 @param pDemod The demodulator instance
 @param pLock Demod lock state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_CheckTSLock(sony_demod_t*             pDemod,
                                           sony_demod_lock_result_t* pLock);

#endif  /*SONY_DEMOD_ISDBT_H*/
