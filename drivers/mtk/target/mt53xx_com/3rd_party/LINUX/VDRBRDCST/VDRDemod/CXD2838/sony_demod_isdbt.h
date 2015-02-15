/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012-09-01
  File Revision : 1.0.6.0
------------------------------------------------------------------------------*/

#ifndef SONY_DEMOD_ISDBT_H
#define SONY_DEMOD_ISDBT_H

#include "sony_common.h"
#include "sony_i2c.h"
#include "sony_isdbt.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief Calculate the demodulator IF Freq setting ::sony_demod_t::iffreqConfig.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5
*/
#define SONY_DEMOD_MAKE_IFFREQ_CONFIG(iffreq) ((UINT32)(((iffreq)/41.0)*16777216.0 + 0.5))

#define SONY_DEMOD_MAX_CONFIG_MEMORY_COUNT 100 /**< The maximum number of entries in the configuration memory table */

/**
 @brief Freeze all registers in the SLV-T device.  This API is used by the monitor functions to ensure multiple separate 
        register reads are from the same snapshot 

 @note This should not be manually called or additional instances added into the driver unless under specific instruction.
*/
#define SLVT_FreezeReg(pDemod) ((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x01))

/**
 @brief Unfreeze all registers in the SLV-T device 
*/
#define SLVT_UnFreezeReg(pDemod) ((void)((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x00)))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Demodulator crystal frequency.
*/
typedef enum {
    SONY_DEMOD_XTAL_20500KHz,          /**< 20.5 MHz */
    SONY_DEMOD_XTAL_41000KHz           /**< 41 MHz */
} sony_demod_xtal_t;

/**
 @brief The demodulator Chip ID mapping.
*/
typedef enum {
    SONY_DEMOD_CHIP_ID_CXD2838 = 0xB0, /**< CXD2838 ISDB-T */
    SONY_DEMOD_CHIP_ID_CXD2828 = 0x22  /**< CXD2828 ISDB-T */
} sony_demod_chip_id_t;

/**
 @brief Demodulator software state.
*/
typedef enum {
    SONY_DEMOD_STATE_UNKNOWN = 0,   /**< Unknown. */
    SONY_DEMOD_STATE_SHUTDOWN,      /**< Chip is in Shutdown state */
    SONY_DEMOD_STATE_SLEEP,         /**< Chip is in Sleep state */
    SONY_DEMOD_STATE_ACTIVE,        /**< Chip is in Active state */
    SONY_DEMOD_STATE_EWS,           /**< Chip is in EWS state */
    SONY_DEMOD_STATE_INVALID        /**< Invalid, result of an error during a state change. */
} sony_demod_state_t;

/**
 @brief System bandwidth.
*/
typedef enum {
    SONY_DEMOD_BW_UNKNOWN = 0,          /**< Unknown bandwidth */
    SONY_DEMOD_BW_6_MHZ = 6,            /**< 6MHz bandwidth */
    SONY_DEMOD_BW_7_MHZ = 7,            /**< 7MHz bandwidth */
    SONY_DEMOD_BW_8_MHZ = 8             /**< 8MHz bandwidth */
} sony_demod_bandwidth_t;

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
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
    SONY_DEMOD_SPECTRUM_NORMAL = 0,          /**< Spectrum normal sense. */
    SONY_DEMOD_SPECTRUM_INV                  /**< Spectrum inverted. */
} sony_demod_spectrum_sense_t;

/**
 @brief Configuration options for the demodulator.
*/
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
            - 0: Gated
            - 1: Continuous (Default)
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
     @brief IFAGC sense configuration.

            Value:
            - 0: Positive IFAGC.
            - 1: Inverted IFAGC (Default)
    */
    SONY_DEMOD_CONFIG_IFAGCNEG,

    /**
     @brief Configure the full-scale range of the ADC input to the IFAGC.

            Value:
            - 0: 1.4Vpp (Default)
            - 1: 1.0Vpp
            - 2: 0.7Vpp
    */
    SONY_DEMOD_CONFIG_IFAGC_ADC_FS,

    /**
    @brief Spectrum inversion configuration for the terrestrial tuner. 
            
            Value: 
            - 0: Normal (Default).
            - 1: Inverted.
    */
    SONY_DEMOD_CONFIG_SPECTRUM_INV,

    /**
     @brief Set Number of Measured Packets - 15bit
    */
    SONY_DEMOD_CONFIG_BERPER_PERIOD
} sony_demod_config_t;

/**
 @brief Demodulator lock status.
*/
typedef enum {
    SONY_DEMOD_LOCK_RESULT_NOT_DETECT, /**< "Lock" or "Unlock" conditions not met */
    SONY_DEMOD_LOCK_RESULT_LOCK,       /**< "Lock" condition is found. */
    SONY_DEMOD_LOCK_RESULT_UNLOCK      /**< No signal was found or the signal was not the required system. */
} sony_demod_lock_result_t;

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

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief List of register values for IF frequency configuration.  Used for handling 
        tuners that output a different IF depending on the expected channel BW.
        Should be set using ::SONY_DEMOD_MAKE_IFFREQ_CONFIG macro.
*/
typedef struct sony_demod_iffreq_config_t {
    UINT32 configIsdbt6;   /**< ISDB-T 6MHz */
    UINT32 configIsdbt7;   /**< ISDB-T 7MHz */
    UINT32 configIsdbt8;   /**< ISDB-T 8MHz */
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
    /**
    @brief The demodulator crystal frequency.
    */
    sony_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0).
    */
    UINT8 i2cAddressSLVT;

    /**
     @brief SLVX I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT + 4.
    */
    UINT8 i2cAddressSLVX;

    /**
     @brief I2C API instance.
    */
    sony_i2c_t * pI2c;

    /**
    @brief The driver operating state.
    */
    sony_demod_state_t state;

    /**
     @brief Auto detected chip ID at initialisation.
    */
    sony_demod_chip_id_t chipId;

    /**
     @brief The current bandwidth.
    */
    sony_demod_bandwidth_t bandwidth;

    /**
     @brief IF frequency configuration. Configure prior to initialization.
            Use the ::SONY_DEMOD_MAKE_IFFREQ_CONFIG macro for configuration.
    */
    sony_demod_iffreq_config_t iffreqConfig;

    /**
     @brief Stores the tuner model for demodulator specific optimisations.
    */
    sony_demod_tuner_optimize_t tunerOptimize;

    /**
     @brief The sense configured on the demodulator with
            ::sony_demod_SetConfig.
    */
    sony_demod_spectrum_sense_t confSense;

    /**
     @brief A table of the demodulator configuration changes stored from the 
            ::sony_demod_SetConfig and ::sony_demod_SetAndSaveRegisterBits functions.
    */
    sony_demod_config_memory_t configMemory[SONY_DEMOD_MAX_CONFIG_MEMORY_COUNT];

    /**
     @brief The index of the last valid entry in the configMemory table
    */
    UINT8 configMemoryLastEntry;

    /**
     @brief User defined data.
    */
    void * user;
} sony_demod_t;

/**
 @brief The preset information for a ISDB-T signal
*/
typedef struct sony_demod_isdbt_preset_info_t {
    UINT8 data[13];                   /**< TMCC information defined by ISDB-T */
} sony_demod_isdbt_preset_info_t;

/**
 @brief The tune parameters for a ISDB-T signal
*/
typedef struct sony_isdbt_tune_param_t {
    UINT32 centerFreqKHz;             /**< Center frequency(kHz) of the ISDB-T channel */
    sony_demod_bandwidth_t bandwidth;   /**< Bandwidth of the ISDB-T channel */
} sony_isdbt_tune_param_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/**
  @brief Set up the demodulator.
         This function Must be called before calling sony_demod_Initialize.
  @param pDemod Reference to memory allocated for the demodulator instance.
  @param xtalFreq The frequency of the demod crystal.
  @param i2cAddress The demod I2C address in 8-bit form.
  @param pDemodI2c The I2C driver that the demod will use as the I2C interface.

  @return SONY_RESULT_OK if successful.
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


/**
 @brief Set a specific value with bit mask to any demod register.  
        NOTE : This API should only be used under instruction from Sony 
        support. Manually modifying any demodulator register could have a negative 
        effect for performance or basic functionality.
         
 @param pDemod The demodulator instance.
 @param slaveAddress Slave address of configuration setting
 @param bank Demodulator bank of configuration setting
 @param registerAddress Register address of configuration setting
 @param value The value being written to this register
 @param bitMask The bit mask used on the register
 
 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_SetAndSaveRegisterBits (sony_demod_t * pDemod, 
                                                 UINT8 slaveAddress, 
                                                 UINT8 bank, 
                                                 UINT8 registerAddress,
                                                 UINT8 value,
                                                 UINT8 bitMask);

/*------------------------------------------------------------------------------
  Functions for ISDB-T
------------------------------------------------------------------------------*/
/**
 @brief Set Preset Infomation

 @param pDemod      The demodulator instance.
 @param pPresetInfo Preset information

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_SetPreset(sony_demod_t*                   pDemod,
                                         sony_demod_isdbt_preset_info_t* pPresetInfo);

/**
 @brief Enable acquisition on the demodulator for ISDB-T channels.  Called from
        the integration layer ::sony_integ_isdbt_Tune API.

 @param pDemod     The demodulator instance
 @param pTuneParam Tune parameters structure.

 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_demod_isdbt_Tune(sony_demod_t*            pDemod,
                                    sony_isdbt_tune_param_t* pTuneParam);
/**
 @brief Enable acquisition on the demodulator for ISDB-T channels in EWS mode.
        Called from the integration layer ::sony_integ_isdbt_EWSTune API.

 @param pDemod     The demodulator instance
 @param pTuneParam Tune parameters structure.
  
 @return SONY_RESULT_OK if successful.
 */
sony_result_t sony_demod_isdbt_EWSTune(sony_demod_t*            pDemod,
                                       sony_isdbt_tune_param_t* pTuneParam);


/**
@brief Check ISDB-T demodulator lock status.

 @param pDemod  The demodulator instance
 @param pLock   Demod lock state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_CheckDemodLock(sony_demod_t*             pDemod,
                                              sony_demod_lock_result_t* pLock);

/**
 @brief Check TS lock status.

 @param pDemod  The demodulator instance
 @param pLock   TS lock state

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_demod_isdbt_CheckTSLock(sony_demod_t*             pDemod,
                                           sony_demod_lock_result_t* pLock);

#endif /* SONY_DEMOD_ISDBT_H */
