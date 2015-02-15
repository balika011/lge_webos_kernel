/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2014/05/15
 Revision: 1.1.11.0

------------------------------------------------------------------------------*/
#ifndef SONY_CN_DTV_DEMOD_H
#define SONY_CN_DTV_DEMOD_H

#include "sony_cn_dtv.h"


/*------------------------------------------------------------------------------
  I2C address default (8bit form)
------------------------------------------------------------------------------*/
#define SONY_CN_DTV_DEMOD_ADDRESS           0xD8   /* D8h, DAh, C8h, CAh */

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Software state
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_STATE_UNKNOWN,
    SONY_CN_DTV_DEMOD_STATE_PRE_INIT, /* Hardware state is unknown.    */
    SONY_CN_DTV_DEMOD_STATE_INIT,     /* Hardware state is "Sleep".    */
    SONY_CN_DTV_DEMOD_STATE_NORMAL_Z, /* Hardware state is "Normal-Z". */
    SONY_CN_DTV_DEMOD_STATE_NORMAL_J, /* Hardware state is "Normal-J". */
    SONY_CN_DTV_DEMOD_STATE_INVALID
}sony_cn_dtv_demod_state_t;

/*--------------------------------------------------------------------
  Crystal frequency
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_41000KHZ,   /* 41MHz */
    SONY_CN_DTV_DEMOD_20500KHZ    /* 20.5MHz */
}sony_cn_dtv_demod_xtal_freq_t;

/*--------------------------------------------------------------------
  Enumeration of supported sony tuner models.
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_TUNER_UNKNOWN,   /* Non Sony Tuner */
    SONY_CN_DTV_DEMOD_TUNER_ASCOT2D,   /* Sony ASCOT2D derived tuners */
    SONY_CN_DTV_DEMOD_TUNER_ASCOT2E,   /* Sony ASCOT2E derived tuners */
    SONY_CN_DTV_DEMOD_TUNER_ASCOT3     /* Sony ASCOT3 derived tuners */
}sony_cn_dtv_demod_tuner_model_t;

/*--------------------------------------------------------------------
  Lock status
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK,     /* Unlock */
    SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT, /* Not detect */
    SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK,       /* Lock */
    SONY_CN_DTV_DEMOD_LOCK_STATUS_UNKNOWN     /* Error occurred */
} sony_cn_dtv_demod_lock_status_t;

/*--------------------------------------------------------------------
  Preset mode (for DTMB)
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_PRESET_MODE_NONE,         /* No preset */
    SONY_CN_DTV_DEMOD_PRESET_MODE_CMODE,        /* Carrier mode is only valid and used. */
    SONY_CN_DTV_DEMOD_PRESET_MODE_FULL_USECMODE,/* All preset information is valid but only carrier mode is used to preset. */
    SONY_CN_DTV_DEMOD_PRESET_MODE_FULL          /* All preset information is valid and used. */
} sony_cn_dtv_demod_preset_mode_t;

/*--------------------------------------------------------------------
  Config ID
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_CONFIG_TS_OUTPUT_PARALLEL_SERIAL, /* TS Output Parallel or Serial */
    SONY_CN_DTV_DEMOD_CONFIG_IS_ASCOT,                  /* Use ASCOT or not (0:Not ASCOT, 1:ASCOT[default]) */
    SONY_CN_DTV_DEMOD_CONFIG_INVERT_TUNER,              /* Invert Tuner (0:Not invert[default], 1:Invert) */
    SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG,             /* Invert the Polarity of the Tuner IFAGC (0:Do not invert, 1:Invert[default]) */
    SONY_CN_DTV_DEMOD_CONFIG_SHARED_IF,                 /* Set Shared IF mode (0:Disable(Default), 1:Enable) */
    SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE,                 /* Scan mode on/off */
    SONY_CN_DTV_DEMOD_CONFIG_TS_AUTO_RATE_ENABLE,       /* Enable or disable the auto TS clock rate(0:Disable, 1:Enable[default]) */
    SONY_CN_DTV_DEMOD_CONFIG_TS_MANUAL_RATE_SETTING,    /* Set the TS clock rate manually(KHz) !!for Debug!! */
    SONY_CN_DTV_DEMOD_CONFIG_GPIO_0_DISABLE_PULLUP,     /* Disable the internal pull-up for GPIO0 */
    SONY_CN_DTV_DEMOD_CONFIG_GPIO_1_DISABLE_PULLUP,     /* Disable the internal pull-up for GPIO1 */
    SONY_CN_DTV_DEMOD_CONFIG_GPIO_2_DISABLE_PULLUP,     /* Disable the internal pull-up for GPIO2 */
    SONY_CN_DTV_DEMOD_CONFIG_TSERR_ENABLE,              /* Enable/Disable TS error output from GPIO2 pin (GPIO2 setting will be overwritten) */
    SONY_CN_DTV_DEMOD_CONFIG_PWM_ENABLE,                /* Enable/Disable PWM output from GPIO1 (RFAGC) pin (GPIO1 setting will be overwritten) */
    SONY_CN_DTV_DEMOD_CONFIG_PWM_VALUE,                 /* Set a 12-bit value to the PWM output setting register */
    SONY_CN_DTV_DEMOD_CONFIG_SETTING_TESTMODE_01        /* Set the test mode 01 */
} sony_cn_dtv_demod_config_t;

/*--------------------------------------------------------------------
  DTMB demodulation sequence state
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_ACQUISITION, /* Acquistion state */
    SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_C1,          /* Tracking state of C1 */
    SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_C3780        /* Tracking state of C3780 */
} sony_cn_dtv_demod_dtmb_seq_state_t;

/*--------------------------------------------------------------------
  TS Output format
--------------------------------------------------------------------*/
typedef enum {
    SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_LSB,        /* PARALLEL output(LSB to TSDATA0) */ 
    SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_MSB,        /* PARALLEL output(LSB to TSDATA7) */ 
    SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_DATAONLY,   /* SERIAL output from TSDATA0 pin (Data Only Mode)*/
    SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_DATAONLY,   /* SERIAL output from TSDATA7 pin (Data Only Mode)*/
    SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_CONTINUOUS, /* SERIAL output from TSDATA0 pin (Continuous Mode)*/
    SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_CONTINUOUS  /* SERIAL output from TSDATA7 pin (Continuous Mode)*/
} sony_cn_dtv_demod_ts_output_t;

/*------------------------------------------------------------------------------
  Macros
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Calculate IF frequency setting register values

  This macro includes floating point values but will be calculated by compiler!
   OREG_DDC_PHASE_INCR = 2^35 / 41[MHz] * IF[MHz]
   OREG_ITB_DWNCONVER_FRQENCY = 2^14 / 41[MHz] * IF Freq[MHz]
--------------------------------------------------------------------*/
#define CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DTMB_U(iffreq)  ((uint32_t)((iffreq) * 8.0 / 41.0) & 0x07)
#define CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DTMB_L(iffreq)  ((uint32_t)(((iffreq) * 8.0 / 41.0 - (uint32_t)((iffreq) * 8.0 / 41.0)) * 4294967296))
#define CN_DTV_DEMOD_MAKE_IFFREQ_CONFIG_DVBC(iffreq)    ((uint16_t)(16384.0 * (iffreq) / 41.0) & 0x3FFF)

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Register values of IF frequency setting
--------------------------------------------------------------------*/
typedef struct {
    uint32_t config_DTMB_U3B;   /* for DTMB upper 3bit */
    uint32_t config_DTMB_L32B;  /* for DTMB lower 32bit */
    uint16_t config_DVBC;       /* for DVB-C */
} sony_cn_dtv_demod_iffreq_config_t;

/*--------------------------------------------------------------------
  Preset data for fast synchronizaion (for DTMB)
--------------------------------------------------------------------*/
typedef struct {
    sony_cn_dtv_dtmb_carrier_mode_t     carrierMode;  /* Carrier Mode */
    sony_cn_dtv_dtmb_system_info_t      systemInfo;   /* System Information */
    sony_cn_dtv_dtmb_pseudo_noise_t     pnInfo;       /* Pseudo Noise */
} sony_cn_dtv_demod_preset_info_t;

/*--------------------------------------------------------------------
  demod part control data
--------------------------------------------------------------------*/
typedef struct {
    sony_cn_dtv_demod_state_t           state;         /* status of this software */
    sony_i2c_t*                         pI2c;          /* I2C instance */
    uint8_t                             i2cAddress;    /* I2C slave address */
    sony_cn_dtv_demod_xtal_freq_t       xtalFreq;      /* Crystal frequency */
    sony_cn_dtv_demod_tuner_model_t     isAscot;       /* Enable/Disable ASCOT special settings */
    sony_cn_dtv_demod_iffreq_config_t   iffreq_config; /* IF frequency settings */
    uint8_t                             isInvertTuner; /* Tuner is inverting tuner or not */
    sony_cn_dtv_demod_ts_output_t       tsOutput;      /* TS Output Format */
    uint8_t                             sharedIf;      /* Shared IF enable/disable flag */
    sony_cn_dtv_system_t                system;        /* Current broadcast system */
    uint8_t                             isScanMode;    /* scanmode flag */
    uint8_t                             testMode01;    /* Test mode 01 flag */
    void*                               user;          /* User defined void* pointer */
} sony_cn_dtv_demod_t;


/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Create instance

  Initialize pDemod.

  pDemod        : Address of Demodulator instance.
  pI2C          : Address of I2C instance for pDemod.
  i2cAddress    : I2C slave address (default:0xD8)
  xtalFreq      : Crystal frequency
  iffreq_config : IF frequency settings.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Create(sony_cn_dtv_demod_t* pDemod,
                                       sony_i2c_t* pI2C,
                                       uint8_t i2cAddress,
                                       sony_cn_dtv_demod_xtal_freq_t xtalFreq,
                                       sony_cn_dtv_demod_iffreq_config_t* iffreq_config);

/*--------------------------------------------------------------------
  Initialize Demodulator

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Initialize(sony_cn_dtv_demod_t* pDemod);

/*--------------------------------------------------------------------
  Tuning

  Set tuning parameter.
  If using the fast synchronized Tune, before calling this function, 
  please call the below function.
   1. sony_cn_dtv_demod_SetPreset
      Set preset information and set enable/disable.

  pDemod : Address of demodulator instance.
  system : Broadcast sysytem.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Tune(sony_cn_dtv_demod_t* pDemod, 
                                     sony_cn_dtv_system_t system);

/*--------------------------------------------------------------------
  Post process of Tuning

  Software reset and excute the following processes.

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_PostTune(sony_cn_dtv_demod_t* pDemod);

/*--------------------------------------------------------------------
  Sleep

  Change hardware state to "Sleep".

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Sleep(sony_cn_dtv_demod_t* pDemod);

/*--------------------------------------------------------------------
  Set preset information

  Set preset information to shorten tuning time.
  It must call this function before tuning sequence.

  pDemod         : Address of demodulator instance.
  presetInfo     : Address of preset information.
  presetMode     : Preset mode. (See sony_cn_dtv_demod_preset_mode_t definition.)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SetPreset(sony_cn_dtv_demod_t* pDemod,
                                          sony_cn_dtv_demod_preset_info_t* pPresetInfo,
                                          sony_cn_dtv_demod_preset_mode_t presetMode);

/*--------------------------------------------------------------------
  TS Output enable/disable

  pDemod         : Address of demodulator instance.
  isTsOutEnable  : 0: disable, 1: enable
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_TsOutputEnable(sony_cn_dtv_demod_t* pDemod,
                                               uint8_t isTsOutEnable);

/*--------------------------------------------------------------------
  Soft Reset

  Software reset to DTMB/DVB-C demodulator.

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SoftReset(sony_cn_dtv_demod_t* pDemod);

/*--------------------------------------------------------------------
  Check Demod lock status

  pDemod     : Address of demodulator instance.
  pLock      : Lock status.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckDemodLock(sony_cn_dtv_demod_t* pDemod,
                                               sony_cn_dtv_demod_lock_status_t* pLock);

/*--------------------------------------------------------------------
  Check TS lock status

  pDemod     : Address of demodulator instance.
  pLock      : Lock status.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckTSLock(sony_cn_dtv_demod_t* pDemod,
                                            sony_cn_dtv_demod_lock_status_t* pLock);

/*--------------------------------------------------------------------
  Set various parameters 

  pDemod     : Address of demodulator instance.
  configId   : Config parmeters type.
  value      : Setting value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SetConfig(sony_cn_dtv_demod_t* pDemod,
                                          sony_cn_dtv_demod_config_t configId,
                                          uint32_t value);

/*--------------------------------------------------------------------
  Monitor Internal Digital AGC output level

  pDemod              : Address of demodulator instance.
  pInternalDigitalAgc : Address of Internal Digital AGC ouput level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Monitor_InternalDigitalAgcout(sony_cn_dtv_demod_t* pDemod,
                                                              uint32_t* pInternalDigitalAgc);

/*--------------------------------------------------------------------
  GPIO setting

  Set each GPIO to "Enable/Disable" and "Read/Write".
  Read  : Read the data input from GPIO pin.
  Write : Output the data that set by register from GPIO pin.

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  isEnable   : 0: disable, 1: enable
  isWrite    : 0: read, 1: write
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIOSetting(sony_cn_dtv_demod_t* pDemod,
                                            uint8_t id,
                                            uint8_t isEnable,
                                            uint8_t isWrite);

/*--------------------------------------------------------------------
  Read GPIO value

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  pValue     : Address of the read result.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIORead(sony_cn_dtv_demod_t* pDemod,
                                         uint8_t id,
                                         uint8_t* pValue);


/*--------------------------------------------------------------------
  Write GPIO value

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  pValue     : The value of data output by GPIO pin.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIOWrite(sony_cn_dtv_demod_t* pDemod,
                                          uint8_t id,
                                          uint8_t value);

/*--------------------------------------------------------------------
 Configure the demodulator to forward I2C messages to the
 output port for tuner control.

 pDemod      : Address of demodulator instance.
 enable      : Enable(enable != 0) / Disable I2C repeater(enable == 0)

--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_I2cRepeaterEnable(sony_cn_dtv_demod_t* pDemod,
                                                  uint8_t              enable);

/*--------------------------------------------------------------------
  Check condition1

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckCondition1(sony_cn_dtv_demod_preset_info_t* pPresetInfo,
                                                uint8_t* pIsCondition1);

/*--------------------------------------------------------------------
  Optimizing process (eq_param_init3)

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_OptimizeSet(sony_cn_dtv_demod_t* pDemod);

/*--------------------------------------------------------------------
  check possibility of c1 early detection

  pDemod : Address of demodulator instance.
  pPossibility : signal not detect or early detection or chec again
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckC1EarlyDetectionPossibility(sony_cn_dtv_demod_t* pDemod, uint8_t *pPossibility);

/*--------------------------------------------------------------------
  Early Detect C1

  pDemod : Address of demodulator instance.
  pPossibility : signal not detect or early detection or chec again
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_C1EarlyDetect(sony_cn_dtv_demod_t* pDemod, uint8_t *pIsAssumed);


#endif /* SONY_CN_DTV_DEMOD_H */

