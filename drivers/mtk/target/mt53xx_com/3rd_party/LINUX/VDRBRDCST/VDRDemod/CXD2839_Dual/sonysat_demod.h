/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod.h

          This file provides the common demodulator control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONYSAT_DEMOD_H
#define SONYSAT_DEMOD_H

#define SONYSAT_DRIVER_BUILD_OPTION_CXD2839

/*------------------------------------------------------------------------------
  Device Defines based on pre-compiler BUILD_OPTION
------------------------------------------------------------------------------*/

#if defined SONYSAT_DRIVER_BUILD_OPTION_CXD2837   /* DVB-T/T2/C Demodulator */
#define SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE       /**< Driver supports a Terrestrial or Cable system. */
#elif defined SONYSAT_DRIVER_BUILD_OPTION_CXD2839 /* DVB-S/S2 Demodulator */
#define SONYSAT_DEMOD_SUPPORT_DVBS_S2             /**< Driver supports DVBS and S2. */
#elif defined SONYSAT_DRIVER_BUILD_OPTION_CXD2841 /* DVB-T/T2/C/C2/S/S2 Demodulator */
#define SONYSAT_DEMOD_SUPPORT_DVBC2               /**< Driver supports DVBC2. */
#define SONYSAT_DEMOD_SUPPORT_DVBS_S2             /**< Driver supports DVBS and S2. */
#define SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE       /**< Driver supports a Terrestrial or Cable system. */
#elif defined SONYSAT_DRIVER_BUILD_OPTION_CXD2842 /* DVB-T/T2/C/S/S2 Demodulator */
#define SONYSAT_DEMOD_SUPPORT_DVBS_S2             /**< Driver supports DVBS and S2. */
#define SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE       /**< Driver supports a Terrestrial or Cable system. */
#elif defined SONYSAT_DRIVER_BUILD_OPTION_CXD2843 /* DVB-T/T2/C/C2 Demodulator */
#define SONYSAT_DEMOD_SUPPORT_DVBC2               /**< Driver supports DVBC2. */
#define SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE       /**< Driver supports a Terrestrial or Cable system. */
#else
#error SONYSAT_DRIVER_BUILD_OPTION value not recognised
#endif

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_i2c.h"
#include "sonysat_dtv.h"

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
#include "sonysat_dvbt.h"
#include "sonysat_dvbt2.h"
#include "sonysat_dvbc.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBC2
#include "sonysat_dvbc2.h"
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
#include "sonysat_dvbs.h"
#include "sonysat_dvbs2.h"
#endif


/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Calculate the demodulator IF Freq setting ::sonysat_demod_t::iffreqConfig.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5                
*/
#define SONYSAT_DEMOD_MAKE_IFFREQ_CONFIG(iffreq) ((uint32_t)(((iffreq)/41.0)*16777216.0 + 0.5))
#endif

#define SONYSAT_DEMOD_MAX_CONFIG_MEMORY_COUNT 100 /**< The maximum number of entries in the configuration memory table */

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
    SONYSAT_DEMOD_XTAL_20500KHz,           /**< 20.5 MHz */
    SONYSAT_DEMOD_XTAL_41000KHz            /**< 41 MHz */
} sonysat_demod_xtal_t;

/**
 @brief The demodulator Chip ID mapping.
*/
typedef enum {
    SONYSAT_DEMOD_CHIP_ID_CXD2837 = 0xB1,  /**< CXD2837  DVB-T/T2/C */
    SONYSAT_DEMOD_CHIP_ID_CXD2839 = 0xA5,  /**< CXD2839  DVB-S/S2 */
    SONYSAT_DEMOD_CHIP_ID_CXD2841 = 0xA7,  /**< CXD2841  DVB-T/T2/C/C2/S/S2 */
    SONYSAT_DEMOD_CHIP_ID_CXD2842 = 0xA5,  /**< CXD2842  DVB-T/T2/C/S/S2 */
    SONYSAT_DEMOD_CHIP_ID_CXD2843 = 0xA4,  /**< CXD2843  DVB-T/T2/C/C2 */
} sonysat_demod_chip_id_t;

/**
 @brief Demodulator software state.
*/
typedef enum {
    SONYSAT_DEMOD_STATE_UNKNOWN,           /**< Unknown. */
    SONYSAT_DEMOD_STATE_SHUTDOWN,          /**< Chip is in Shutdown state */
    SONYSAT_DEMOD_STATE_SLEEP_T_C,         /**< Chip is in Sleep state for DVB-T / T2 / C / C2 */
    SONYSAT_DEMOD_STATE_SLEEP_S,           /**< Chip is in Sleep state for DVB-S / S2 */
    SONYSAT_DEMOD_STATE_ACTIVE_T_C,        /**< Chip is in Active state for DVB-T / T2 / C / C2 */
    SONYSAT_DEMOD_STATE_ACTIVE_S,          /**< Chip is in Active state for DVB-S / S2 */
    SONYSAT_DEMOD_STATE_INVALID            /**< Invalid, result of an error during a state change. */
} sonysat_demod_state_t;

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief System bandwidth.
*/
typedef enum {
    SONYSAT_DEMOD_BW_UNKNOWN = 0,          /**< Unknown bandwidth */
    SONYSAT_DEMOD_BW_1_7_MHZ = 1,          /**< 1.7MHz bandwidth (Valid option for DVB-T2 only) */
    SONYSAT_DEMOD_BW_5_MHZ = 5,            /**< 5MHz bandwidth (Valid option for DVB-T / T2) */
    SONYSAT_DEMOD_BW_6_MHZ = 6,            /**< 6MHz bandwidth (Valid option for DVB-T / T2 / C2) */
    SONYSAT_DEMOD_BW_7_MHZ = 7,            /**< 7MHz bandwidth (Valid option for DVB-T / T2) */
    SONYSAT_DEMOD_BW_8_MHZ = 8             /**< 8MHz bandwidth (Valid option for DVB-T / T2 / C / C2) */
} sonysat_demod_bandwidth_t;

/**
 @brief Enumeration of supported sonysat tuner models used for optimising the 
        demodulator configuration.
*/
typedef enum {
    SONYSAT_DEMOD_TUNER_OPTIMIZE_UNKNOWN,  /**< Non-Sony Tuner. */
    SONYSAT_DEMOD_TUNER_OPTIMIZE_ASCOT2D,  /**< Sony ASCOT2D derived tuners. */
    SONYSAT_DEMOD_TUNER_OPTIMIZE_ASCOT2E,  /**< Sony ASCOT2E derived tuners. */
    SONYSAT_DEMOD_TUNER_OPTIMIZE_ASCOT2XR, /**< Sony ASCOT2XR derived tuners. */
    SONYSAT_DEMOD_TUNER_OPTIMIZE_ASCOT3    /**< Sony ASCOT3 derived tuners. */
} sonysat_demod_tuner_optimize_t;

/**
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
    SONYSAT_DEMOD_TERR_CABLE_SPECTRUM_NORMAL = 0,             /**< Spectrum normal sense. */
    SONYSAT_DEMOD_TERR_CABLE_SPECTRUM_INV                     /**< Spectrum inverted. */
} sonysat_demod_terr_cable_spectrum_sense_t;
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
/**
 @brief Enumeration of I/Q inversion monitor values.
*/
typedef enum {
    SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL = 0,   /**< I/Q normal sense. */
    SONYSAT_DEMOD_SAT_IQ_SENSE_INV           /**< I/Q inverted. */
} sonysat_demod_sat_iq_sense_t;
#endif /* SONYSAT_DEMOD_SUPPORT_DVBS_S2 */

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
    SONYSAT_DEMOD_CONFIG_PARALLEL_SEL,

    /**
     @brief Serial output pin of TS data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
    SONYSAT_DEMOD_CONFIG_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TS data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial):
            - 0: LSB first
            - 1: MSB first (Default).
    */
    SONYSAT_DEMOD_CONFIG_OUTPUT_SEL_MSB,

    /**
     @brief TS valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONYSAT_DEMOD_CONFIG_TSVALID_ACTIVE_HI,

    /**
     @brief TS sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONYSAT_DEMOD_CONFIG_TSSYNC_ACTIVE_HI,

    /**
     @brief TS error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    SONYSAT_DEMOD_CONFIG_TSERR_ACTIVE_HI,

    /**
     @brief TS clock inversion setting.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
    SONYSAT_DEMOD_CONFIG_LATCH_ON_POSEDGE,

    /**
     @brief Serial TS clock gated on valid TS data or is continuous.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.  

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
    SONYSAT_DEMOD_CONFIG_TSCLK_CONT,

    /**
     @brief Disable/Enable TS clock during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONYSAT_DEMOD_CONFIG_TSCLK_MASK,

    /**
     @brief Disable/Enable TSVALID during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active 
            - 1 : Disable during TS packet gap (default)
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONYSAT_DEMOD_CONFIG_TSVALID_MASK,
    
    /**
     @brief Disable/Enable TSERR during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
    SONYSAT_DEMOD_CONFIG_TSERR_MASK,

    /**
     @brief Configure the driving current for the TS Clk pin.

            - 0 : 8mA (Default)
            - 1 : 10mA
    */
    SONYSAT_DEMOD_CONFIG_TSCLK_CURRENT_10mA,

    /**
     @brief Configure the driving current for the TS Sync / TS Valid 
            / TS Data / TS Error pins.

            - 0 : 8mA (Default)
            - 1 : 10mA
    */
    SONYSAT_DEMOD_CONFIG_TS_CURRENT_10mA,

    /**
     @brief This configuration can be used to configure the demodulator to output a TS waveform that is
            backwards compatible with previous generation demodulators (CXD2834 / CXD2835 / CXD2836). 
            This option should not be used unless specifically required to overcome a HW configuration issue.

            The demodulator will have the following settings, which will override any prior individual 
            configuration:
            - Disable TS packet gap insertion.
            - Parallel TS maximum bit rate of 82MBps
            - Serial TS clock frequency fixed at 82MHz
            
            Values:
            - 0 : Backwards compatible mode disabled
            - 1 : Backwards compatible mode enabled
    */
    SONYSAT_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE,

    /**
    @brief Writes a 12-bit value to the PWM output.
        Please note the actual PWM precision.
        - 12-bit.
        0xFFF => DVDD
        0x000 => GND
    */
    SONYSAT_DEMOD_CONFIG_PWM_VALUE,

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
     /**
     @brief Set the TS clock rate (data period) manually.
            *NOT recommended for normal use (testing/debug only).*
            If SONYSAT_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS), then this configuration will
            have no effect.
            If SONYSAT_DEMOD_CONFIG_TS_AUTO_RATE_ENABLE = 1, then this configuration will have no
            effect.
            Value:
            - Desired TS clock rate (data period) in kHz in the range 320KHz (2.56Mbps) 
            to 16000KHz (128Mbps)
    */
    SONYSAT_DEMOD_CONFIG_TSIF_SDPR,

    /**
     @brief Enable or disable the auto TS clock rate (data period).
            *NOT recommended for normal use (testing/debug only).*
            If DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS), then this configuration will
            have no effect.
            Value:
            - 0: Disable the TS auto rate.
                 TS clock rate = SONYSAT_DEMOD_CONFIG_TSIF_SDPR.
            - 1: Enable the TS auto rate (Default).
                 TS clock rate is automatic.

            @note Not available in DVB-C mode.
    */
    SONYSAT_DEMOD_CONFIG_TS_AUTO_RATE_ENABLE,

    /**
     @brief Configure the clock frequency for Serial TS in terrestrial and cable active states.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.  
            Only valid when SONYSAT_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : Invalid
            - 1 : 82.00MHz (Default)
            - 2 : 65.60MHz
            - 3 : 54.67MHz
            - 4 : 41.00MHz 
            - 5 : 32.80MHz
    */
    SONYSAT_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ,

    /**
     @brief IFAGC sense configuration.

            Value:
            - 0: Positive IFAGC.
            - 1: Inverted IFAGC (Default)
    */
    SONYSAT_DEMOD_CONFIG_IFAGCNEG,

    /**
     @brief Configure the full-scale range of the ADC input to the IFAGC.

            Value:
            - 0: 1.4Vpp (Default)
            - 1: 1.0Vpp
            - 2: 0.7Vpp
    */
    SONYSAT_DEMOD_CONFIG_IFAGC_ADC_FS,

    /**
     @brief Terrestrial / Cable tuner IF spectrum sense configuration. 
            
            Value: 
            - 0: IF spectrum sense is not same as RF. Used for Normal / Ordinary tuners i.e. ASCOT. (Default)
            - 1: IF spectrum sense is same as RF.
    */
    SONYSAT_DEMOD_CONFIG_SPECTRUM_INV,

    /**
     @brief RFAIN ADC enable/disable.
            Must be asleep for this setting to take effect on next tune/scan.
            Use ::sonysat_demod_terr_cable_monitor_RFAIN to monitor the input.

            Value:
            - 0: Disable RFAIN ADC and monitor (Default).
            - 1: Enable RFAIN ADC and monitor.
    */
    SONYSAT_DEMOD_CONFIG_RFAIN_ENABLE,

    /**
     @brief Configure the order in which systems are attempted in Blind Tune and 
            Scan.  This can be used to optimize scan duration where specifc
            details on system split ratio are known about the spectrum.

            Value:
            - 0: DVB-T followed by DVBT2 (default).
            - 1: DVB-T2 followed by DVBT.
    */
    SONYSAT_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST,

    /**
     @brief Set the measurment period for Pre-RS BER (DVB-T).

            This is a 5 bit value with a default of 11.
    */
    SONYSAT_DEMOD_CONFIG_DVBT_BERN_PERIOD,

    /**
     @brief Set the measurment period for Pre-RS BER (DVB-C).

            This is a 5 bit value with a default of 11.
    */
    SONYSAT_DEMOD_CONFIG_DVBC_BERN_PERIOD,

    /**
     @brief Set the measurment period for Pre-Viterbi BER (DVB-T).

            This is a 3 bit value with a default of 1.
    */
    SONYSAT_DEMOD_CONFIG_DVBT_VBER_PERIOD,

    /**
     @brief Set the measurment period for Pre-BCH BER (DVB-T2/C2) and 
            Post-BCH FER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
    SONYSAT_DEMOD_CONFIG_DVBT2C2_BBER_MES,

    /**
     @brief Set the measurment period for Pre-LDPC BER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
    SONYSAT_DEMOD_CONFIG_DVBT2C2_LBER_MES,

    /**
     @brief Set the measurment period for PER (DVB-T).

            This is a 4 bit value with a default of 10.
    */
    SONYSAT_DEMOD_CONFIG_DVBT_PER_MES,

    /**
     @brief Set the measurment period for PER (DVB-C).

            This is a 4 bit value with a default of 10.
    */
    SONYSAT_DEMOD_CONFIG_DVBC_PER_MES,

    /**
     @brief Set the measurment period for PER (DVB-T2/C2).

            This is a 4 bit value with a default of 10.
    */
    SONYSAT_DEMOD_CONFIG_DVBT2C2_PER_MES,
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2
    /**
     @brief Configure the clock frequency for Serial TS in Satellite active states.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.  
            Only valid when SONYSAT_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : 129.83MHz
            - 1 : 97.38MHz (Default)
            - 2 : 77.90MHz
            - 3 : 64.92MHz
            - 4 : 48.69MHz
            - 5 : 38.95MHz
    */
    SONYSAT_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ,

    /**
     @brief I/Q connection sense inversion between tuner and demod.

            - 0: Normal (Default).
            - 1: Inverted. (I/Q signal input to Q/I pin of demodulator)
    */
    SONYSAT_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV,

    /**
     @brief IFAGC sense configuration for satellite.

            Value:
            - 0: Positive IFAGC (Default)
            - 1: Negative IFAGC.
    */
    SONYSAT_DEMOD_CONFIG_SAT_IFAGCNEG,

    /**
     @brief Use TSDATA6 pin for DiSEqC RXEN and TXEN.
            Only valid when SONYSAT_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            Value:
            - 0: Not used as DiSEqC. (Default)
            - 1: TX_EN
            - 2: RX_EN
    */
    SONYSAT_DEMOD_CONFIG_SAT_TSDATA6_DISEQC,

    /**
     @brief Measurement period for Pre-RS BER(DVB-S), PER(DVB-S) and PER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    SONYSAT_DEMOD_CONFIG_SAT_MEAS_PERIOD1,

    /**
     @brief Measurement period for Pre-BCH BER(DVB-S2) and Post-BCH FER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    SONYSAT_DEMOD_CONFIG_SAT_MEAS_PERIOD2,

    /**
     @brief Measurement period for Pre-Viterbi BER(DVB-S).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    SONYSAT_DEMOD_CONFIG_SAT_MEAS_PERIOD3,

    /**
     @brief Measurement period for Pre-LDPC BER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    SONYSAT_DEMOD_CONFIG_SAT_MEAS_PERIOD4,

#endif /* SONYSAT_DEMOD_SUPPORT_DVBS_S2 */
} sonysat_demod_config_id_t;

/**
 @brief Demodulator lock status.
*/
typedef enum {
    SONYSAT_DEMOD_LOCK_RESULT_NOTDETECT, /**< Neither "Lock" or "Unlock" conditions are met, lock status cannot be determined */
    SONYSAT_DEMOD_LOCK_RESULT_LOCKED,    /**< "Lock" condition is found. */
    SONYSAT_DEMOD_LOCK_RESULT_UNLOCKED   /**< No signal was found or the signal was not the required system. */
} sonysat_demod_lock_result_t;

/**
 @brief Mode select for the multi purpose GPIO pins
*/
typedef enum {
    /** @brief GPIO pin is configured as an output */
    SONYSAT_DEMOD_GPIO_MODE_OUTPUT = 0x00, 

    /** @brief GPIO pin is configured as an input */
    SONYSAT_DEMOD_GPIO_MODE_INPUT = 0x01, 
        
    /** 
     @brief GPIO pin is configured to output an PWM signal which can be configured using the 
            ::sonysat_demod_SetConfig function with the config ID ::SONYSAT_DEMOD_CONFIG_PWM_VALUE.
    */
    SONYSAT_DEMOD_GPIO_MODE_PWM = 0x03, 

    /** @brief GPIO pin is configured to output TS error */
    SONYSAT_DEMOD_GPIO_MODE_TS_ERROR = 0x04, 

    /** @brief GPIO pin is configured to output the FEF timing indicator (DVB-T2 Only) */
    SONYSAT_DEMOD_GPIO_MODE_FEF_PART = 0x05, 

    /** @brief GPIO pin is configured to output DiSEqC Transmit Enable */
    SONYSAT_DEMOD_GPIO_MODE_DISEQC_TX_EN = 0x08, 

    /** @brief GPIO pin is configured to output DiSEqC Receive Enable */
    SONYSAT_DEMOD_GPIO_MODE_DISEQC_RX_EN = 0x09
} sonysat_demod_gpio_mode_t;

/**
 @brief TS serial clock frequency options
*/
typedef enum {
    SONYSAT_DEMOD_SERIAL_TS_CLK_HIGH_FULL,   /** High frequency, full rate */
    SONYSAT_DEMOD_SERIAL_TS_CLK_MID_FULL,    /** Mid frequency, full rate */
    SONYSAT_DEMOD_SERIAL_TS_CLK_LOW_FULL,    /** Low frequency, full rate */
    SONYSAT_DEMOD_SERIAL_TS_CLK_HIGH_HALF,   /** High frequency, half rate */
    SONYSAT_DEMOD_SERIAL_TS_CLK_MID_HALF,    /** Mid frequency, half rate */
    SONYSAT_DEMOD_SERIAL_TS_CLK_LOW_HALF     /** Low frequency, half rate */
} sonysat_demod_serial_ts_clk_t ;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief List of register values for IF frequency configuration.  Used for handling 
        tuners that output a different IF depending on the expected channel BW.
        Should be set using ::SONYSAT_DEMOD_MAKE_IFFREQ_CONFIG macro.
*/
typedef struct {
    uint32_t configDVBT_5;              /**< DVB-T 5MHz */
    uint32_t configDVBT_6;              /**< DVB-T 6MHz */
    uint32_t configDVBT_7;              /**< DVB-T 7MHz */
    uint32_t configDVBT_8;              /**< DVB-T 8MHz */
    uint32_t configDVBT2_1_7;           /**< DVB-T2 1.7MHz */
    uint32_t configDVBT2_5;             /**< DVB-T2 5MHz */
    uint32_t configDVBT2_6;             /**< DVB-T2 6MHz */
    uint32_t configDVBT2_7;             /**< DVB-T2 7MHz */
    uint32_t configDVBT2_8;             /**< DVB-T2 8MHz */
    uint32_t configDVBC2_6;             /**< DVB-C2 6MHz */
    uint32_t configDVBC2_8;             /**< DVB-C2 8MHz */
    uint32_t configDVBC_6;              /**< DVB-C  6MHZ */
    uint32_t configDVBC_7;              /**< DVB-C  7MHZ */
    uint32_t configDVBC_8;              /**< DVB-C  8MHZ */
} sonysat_demod_iffreq_config_t;
#endif
/**
 @brief The demodulator configuration memory table entry. Used to store a register or
        bit modification made through either the ::sonysat_demod_SetConfig or 
        ::sonysat_demod_SetAndSaveRegisterBits APIs.
*/
typedef struct {
    uint8_t slaveAddress;               /**< Slave address of register */
    uint8_t bank;                       /**< Bank for register */
    uint8_t registerAddress;            /**< Register address */
    uint8_t value;                      /**< Value to write to register */
    uint8_t bitMask;                    /**< Bit mask to apply on the value */
} sonysat_demod_config_memory_t;

/**
 @brief The demodulator definition which allows control of the demodulator device 
        through the defined set of functions. This portion of the driver is seperate 
        from the tuner portion and so can be operated independently of the tuner.
*/
typedef struct sonysat_demod_t {
    /**
    @brief The demodulator crystal frequency.
    */
    sonysat_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0).
    */
    uint8_t i2cAddressSLVT;

    /**
     @brief SLVX I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT + 4.
    */
    uint8_t i2cAddressSLVX;

    /**
     @brief I2C API instance.
    */
    sonysat_i2c_t * pI2c;

    /**
    @brief The driver operating state.
    */
    sonysat_demod_state_t state;

    /**
    @brief The current system.
    */
    sonysat_dtv_system_t system;

    /**
     @brief Auto detected chip ID at initialisation.
    */
    sonysat_demod_chip_id_t chipId;

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
    /**
     @brief The current bandwidth, terrestrial and cable systems only.
    */
    sonysat_demod_bandwidth_t bandwidth;

    /**
     @brief IF frequency configuration for terrestrial and cable systems. Configure 
            prior to initialisation. Use the ::SONYSAT_DEMOD_MAKE_IFFREQ_CONFIG macro for 
            configuration.
    */
    sonysat_demod_iffreq_config_t iffreqConfig;

    /**
     @brief Stores the terrestrial / cable tuner model for demodulator specific optimisations.
    */
    sonysat_demod_tuner_optimize_t tunerOptimize;

    /**
     @brief RFAIN ADC enable/disable. Must be configured prior to initialisation.
            Only change this indicator during the ::SONYSAT_DEMOD_STATE_SLEEP_T_C state. 
            Use ::sonysat_demod_SetConfig to configure this flag.
    */
    uint8_t enableRfain;

    /**
     @brief Scan mode enable/disable. Only change this indicator during the
            SONYSAT_DEMOD_STATE_SLEEP_T_C.
    */
    uint8_t scanMode;

    /**
     @brief The terrestrial / cable tuner IF spectrum sense configured on 
            the demodulator with ::sonysat_demod_SetConfig.
    */
    sonysat_demod_terr_cable_spectrum_sense_t confSense;

    /**
     @brief The serial TS clock frequency option for terrestrial and cable active states. 
            This is configured using ::sonysat_demod_SetConfig with the 
            SONYSAT_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ option.
    */
    sonysat_demod_serial_ts_clk_t serialTsClkFreqTerrCable;

    /**
     @brief The order in which Blind Tune attempts acquisition.  This value can
            be configured using ::sonysat_demod_SetConfig with the 
            SONYSAT_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST option.
    */
    uint8_t blindTuneDvbt2First;
#endif

#ifdef SONYSAT_DEMOD_SUPPORT_DVBS_S2

    /**
     @brief The scan mode for DVB-S/S2.
    */
    uint8_t dvbss2ScanMode;

    /**
     @brief The I/Q sense configured on the demodulator with ::sonysat_demod_SetConfig.
    */
    sonysat_demod_sat_iq_sense_t satTunerIqSense;

    /**
     @brief The flag whether current mode is "Single cable" mode or not.
            with sonysat_integ_sat_device_ctrl_EnterSinglecable and
            sonysat_integ_sat_device_ctrl_ExitSinglecable.
    */
    uint8_t isSinglecable;

    /**
     @brief The flag whether the IQ polarity of single cable switch is invert.
            If it is necessary to change this value, please change it directly after initialize.
    */
    uint8_t isSinglecableIqInv;

    /**
     @brief The serial TS clock frequency option for Satellite active states.  This is 
            configured using ::sonysat_demod_SetConfig with the 
            SONYSAT_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ option.
    */
    sonysat_demod_serial_ts_clk_t serialTsClkFreqSat;
#endif /* SONYSAT_DEMOD_SUPPORT_DVBS_S2 */

    /**
     @brief The serial TS clock mode for all active states.  This is configured using
            ::sonysat_demod_SetConfig with the SONYSAT_DEMOD_CONFIG_TSCLK_CONT option.
    */
    uint8_t serialTsClockModeContinuous;

    /**
     @brief A table of the demodulator configuration changes stored from the 
            ::sonysat_demod_SetConfig and ::sonysat_demod_SetAndSaveRegisterBits functions.
    */
    sonysat_demod_config_memory_t configMemory[SONYSAT_DEMOD_MAX_CONFIG_MEMORY_COUNT];

    /**
     @brief The index of the last valid entry in the configMemory table
    */
    uint8_t configMemoryLastEntry;

    /**
     @brief User defined data.
    */
    void * user;
} sonysat_demod_t;

/**
 @brief Register definition structure for TS clock configurations.    
 */
typedef struct {
    uint8_t serialClkMode;      /**< Serial clock mode (gated or continuous) */
    uint8_t serialDutyMode;     /**< Serial clock duty mode (full rate or half rate) */
    uint8_t tsClkPeriod;        /**< TS clock period */
    uint8_t clkSelTsIf;         /**< TS clock frequency (low, mid or high) */
} sonysat_demod_ts_clk_configuration_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Set up the demodulator.

        This MUST be called before calling ::sonysat_demod_InitializeS or ::sonysat_demod_InitializeT_C.

 @param pDemod Reference to memory allocated for the demodulator instance. The create 
        function will setup this demodulator instance.
 @param xtalFreq The frequency of the demod crystal.
 @param i2cAddress The demod I2C address in 8-bit form.
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_Create (sonysat_demod_t * pDemod,
                                 sonysat_demod_xtal_t xtalFreq,
                                 uint8_t i2cAddress,
                                 sonysat_i2c_t * pDemodI2c);

/**
 @brief Initialize the demodulator, into Terrestrial / Cable mode from a power on state.
        For Satellite systems please use ::sonysat_demod_InitializeS.
        
        Can also be used to reset the demodulator from any state back to 
        ::SONYSAT_DEMOD_STATE_SLEEP_T_C.  Please note this will reset all demodulator registers
        clearing any configuration settings.

        This API also clears the demodulator configuration memory table.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_InitializeT_C (sonysat_demod_t * pDemod);

/**
 @brief Initialize the demodulator, into Satellite mode from a power on state.
        For Terrestrial / Cable systems please use ::sonysat_demod_InitializeT_C.
        
        Can also be used to reset the demodulator from any state back to 
        ::SONYSAT_DEMOD_STATE_SLEEP_S.  Please note this will reset all demodulator registers
        clearing any configuration settings.

        This API also clears the demodulator configuration memory table.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_InitializeS (sonysat_demod_t * pDemod);

#if defined(CC_SONYDEMOD_CXD2842) || defined(CC_SONYDEMOD_CXD2841) /* For MTK */
sonysat_result_t sonysat_demod_ReinitializeS (sonysat_demod_t * pDemod);
#endif

/**
 @brief Put the demodulator into Sleep state for Terrestrial and Cable mode.  From
        this state the demodulator can be directly tuned to any T / T2 / C / C2 signal,
        or have the mode changed to Satellite by calling sonysat_demod_Sleep_S.

        If currently in ::SONYSAT_DEMOD_STATE_SHUTDOWN the configuration memory will be loaded
        back into the demodulator.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_SleepT_C (sonysat_demod_t * pDemod);

/**
 @brief Put the demodulator into Sleep state for Terrestrial and Cable mode.  From
        this state the demodulator can be directly tuned to any S / S2 signal, or have 
        the mode changed to Terrestrial / Cable by calling sonysat_demod_Sleep_T_C.

        If currently in ::SONYSAT_DEMOD_STATE_SHUTDOWN the configuration memory will be loaded
        back into the demodulator.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_SleepS (sonysat_demod_t * pDemod);

/**
 @brief Shutdown the demodulator.

        The device is placed in "Shutdown" state.
        ::sonysat_demod_SleepT_C or ::sonysat_demod_SleepS must be called to re-initialise the
        device and driver for future acquisitions.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_Shutdown (sonysat_demod_t * pDemod);

/**
 @brief Completes the demodulator acquisition setup.
        Must be called after system specific demod and RF tunes.

 @param pDemod The demodulator instance.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_TuneEnd (sonysat_demod_t * pDemod);

/**
 @brief Soft reset the demodulator.
        The soft reset will begin the devices acquisition process.

 @param pDemod The demod instance.

 @return SONYSAT_RESULT_OK if successfully reset.
*/
sonysat_result_t sonysat_demod_SoftReset (sonysat_demod_t * pDemod);

/**
 @brief Set configuration options on the demodulator.

 @param pDemod The demodulator instance.
 @param configId The configuration ID to set. See ::sonysat_demod_config_id_t.
 @param value The associated value. Depends on the configId.

 @return SONYSAT_RESULT_OK if successfully set the configuration option.
*/
sonysat_result_t sonysat_demod_SetConfig (sonysat_demod_t * pDemod, 
                                    sonysat_demod_config_id_t configId, 
                                    int32_t value);

/**
 @brief Configure the demodulator to forward I2C messages to the
        output port for tuner control.

 @param pDemod The demodulator instance.
 @param enable Enable / Disable I2C repeater

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_I2cRepeaterEnable (sonysat_demod_t * pDemod, 
                                            uint8_t enable);

/**
 @brief Setup the GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_GPIOSetConfig (sonysat_demod_t * pDemod, 
                                        uint8_t id, 
                                        uint8_t enable, 
                                        sonysat_demod_gpio_mode_t mode);

/**
 @brief Read the GPIO value.
        The GPIO should have been configured as an input (Read) GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param pValue The current value of the GPIO.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_GPIORead (sonysat_demod_t * pDemod, 
                                   uint8_t id, 
                                   uint8_t * pValue);

/**
 @brief Write the GPIO value.
        The GPIO should have been configured as an output (Write) GPIO.

 @param pDemod The demodulator instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param value The value to set as output.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_GPIOWrite (sonysat_demod_t * pDemod, 
                                    uint8_t id, 
                                    uint8_t value);

/**
 @brief Get the Chip ID of the connected demodulator.
 
 @param pDemod The demodulator instance.
 @param pChipId Pointer to receive the IP ID into.

 @return SONYSAT_RESULT_OK if pChipId is valid.
*/
sonysat_result_t sonysat_demod_ChipID (sonysat_demod_t * pDemod, 
                                 sonysat_demod_chip_id_t * pChipId);

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Monitor the value of the 10bit ADC on the RFAIN demodulator pin
 
 @param pDemod The demodulator instance.
 @param pRFAIN The value of the 10bit ADC RFAIN

 @return SONYSAT_RESULT_OK if pRFAIN is valid.
*/
sonysat_result_t sonysat_demod_terr_cable_monitor_RFAIN (sonysat_demod_t * pDemod, 
                                                   uint16_t * pRFAIN);
#endif

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
 
 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_SetAndSaveRegisterBits (sonysat_demod_t * pDemod, 
                                                 uint8_t slaveAddress, 
                                                 uint8_t bank, 
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask);

#ifdef SONYSAT_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Enable / disable scan mode for acquisition in the demodulator.

 @param pDemod The demodulator instance
 @param system The system used for scanning
 @param scanModeEnabled State of scan mode to set

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_terr_cable_SetScanMode (sonysat_demod_t * pDemod, 
                                      sonysat_dtv_system_t system,
                                      uint8_t scanModeEnabled);
#endif


/**
 @brief Set the TS clock mode and frequency based on the demod struct
        members.  Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_SetTsClockModeAndFreq (sonysat_demod_t * pDemod, sonysat_dtv_system_t system);

#endif /* SONYSAT_DEMOD_H */
