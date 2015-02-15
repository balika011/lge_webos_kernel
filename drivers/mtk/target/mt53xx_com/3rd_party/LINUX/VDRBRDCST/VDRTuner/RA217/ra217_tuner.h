/*------------------------------------------------------------------------------

 Copyright 2010 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2010/10/04
 Revision: 1.0.2.0

------------------------------------------------------------------------------*/

#ifndef RA217_TUNER_H
#define RA217_TUNER_H

#include "ra217_atv.h"
#include "ra217_dtv.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/* I2C slave address of the tuner */
#define RA217_TUNER_ADDRESS     0xC0

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/
/* Tuner state */
typedef enum {
	RA217_TUNER_STATE_UNKNOWN,
	RA217_TUNER_STATE_SLEEP,
	RA217_TUNER_STATE_ACTIVE
} ra217_tuner_state_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
typedef struct ra217_tuner_t {
	ra217_tuner_state_t   state;
	UINT32             xtalFreq;
	UINT8              i2cAddress;
	ra217_tuner_i2c_t*          pI2c;
	UINT32             flags;

	/* For saving current setting */
	UINT32             frequency;
	ra217_atv_video_t     atvVideoID;
	ra217_atv_audio_t     atvAudioID;
	ra217_dtv_system_t    dtvSystem;

	/* Special value for Korea (System-M) */
	UINT8              specialValueForKorea;
	void*                user;
} ra217_tuner_t;

/* Config flag definitions (Apply as mask to "flags" member fo ra217_tuner_t struct) */
#define RA217_TUNER_CONFIG_EXT_REF         0x80000000  /* Use external XTAL (Ref input). */
#define RA217_TUNER_CONFIG_SYSTEML_REGION  0x40000000  /* System is used in System-L region. */

#define RA217_TUNER_CONFIG_IFAGCSEL_ALL1   0x00000000  /* IFOUT/AGCIN 1 is used (default) */
#define RA217_TUNER_CONFIG_IFAGCSEL_ALL2   0x10000000  /* IFOUT/AGCIN 2 is used */
#define RA217_TUNER_CONFIG_IFAGCSEL_A1D2   0x20000000  /* IFOUT/AGCIN 1 is for Analog, 2 is for Digital */
#define RA217_TUNER_CONFIG_IFAGCSEL_D1A2   0x30000000  /* IFOUT/AGCIN 1 is for Digital, 2 is for Analog */
#define RA217_TUNER_CONFIG_IFAGCSEL_MASK   0x30000000

/* ONLY for ASCOT2D */
#define RA217_TUNER_CONFIG_NVMSEL_0_1      0x00000000  /* NVM Bank 0/1 (default) */
#define RA217_TUNER_CONFIG_NVMSEL_0_3      0x01000000  /* NVM Bank 0/3 */
#define RA217_TUNER_CONFIG_NVMSEL_AUTO     0x02000000  /* NVM Bank Auto Select */
#define RA217_TUNER_CONFIG_NVMSEL_MASK     0x03000000

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Tuner instance creation

  ra217_tuner_t     *pTuner      Tuner struct instance
  UINT32         xtalFreq     Xtal frequency for silicon tuner
  UINT8          i2cAddress   I2c slave address of the tuner
  ra217_tuner_i2c_t       *pI2c        I2c struct instance
  UINT32         flags        Configuration flags
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_Create(ra217_tuner_t *pTuner, UINT32 xtalFreq,
								UINT8 i2cAddress, ra217_tuner_i2c_t *pI2c, UINT32 flags);

/*--------------------------------------------------------------------
  Tuner initialization

  ra217_tuner_t     *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_Initialize(ra217_tuner_t *pTuner);

/*--------------------------------------------------------------------
  Tuning to analog channel

  ra217_tuner_t     *pTuner      Tuner struct instance
  UINT32         frequency    RF FP frequency (kHz)
  ra217_atv_video_t videoID      Video system ID (defined in ra217_atv.h)
  ra217_atv_audio_t audioID      Audio system ID (defined in ra217_atv.h)
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_AnalogTune(ra217_tuner_t *pTuner, UINT32 frequency,
									ra217_atv_video_t videoID, ra217_atv_audio_t audioID);

/*--------------------------------------------------------------------
  Tuning to analog channel
  NOTE: ra217_tuner_AnalogTune -> wait 100ms -> ra217_tuner_AnalogAfterTune

  ra217_tuner_t     *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_AnalogAfterTune(ra217_tuner_t *pTuner);

/*--------------------------------------------------------------------
  Tuning to digital channel

  ra217_tuner_t      *pTuner      Tuner struct instance
  UINT32          frequency    RF center frequency (kHz)
  ra217_dtv_system_t system       Broadcast system ID (defined in ra217_dtv.h)
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_DigitalTune(ra217_tuner_t *pTuner, UINT32 frequency, ra217_dtv_system_t system);

/*--------------------------------------------------------------------
  Tuner sleep

  ra217_tuner_t     *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_Sleep(ra217_tuner_t *pTuner);

/*--------------------------------------------------------------------
  Tuner GPO ctrl

  ra217_tuner_t     *pTuner      Tuner struct instance
  UINT8          id           GPIO id (0 / 1)
  UINT8          val          Lo(0) or Hi(1)
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_GPOCtrl(ra217_tuner_t *pTuner, UINT8 id, UINT8 val);

/*--------------------------------------------------------------------
  Prepare special value for analog tuning in Korea

  This function should be called if the system support analog tuning in Korea.

  ra217_tuner_t     *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_PrepareForKorea(ra217_tuner_t *pTuner);

/*--------------------------------------------------------------------
  (ONLY for ASCOT2D)
  RF filter compensation setting for VHF-L band.
  (Please see RFVGA Description of datasheet.)
  New setting will become effective after next tuning.

  mult = VL_TRCKOUT_COEFF(8bit unsigned) / 128
  ofs  = VL_TRCKOUT_OFS(8bit 2s complement)
  (compensated value) = (original value) * mult + ofs

  ra217_tuner_t     *pTuner      Tuner struct instance
  UINT8          coeff        VL_TRCKOUT_COEFF (multiplier)
  UINT8          offset       VL_TRCKOUT_OFS (additional term)
--------------------------------------------------------------------*/
ra217_result_t ra217_tuner_RFFilterConfig(ra217_tuner_t *pTuner, UINT8 coeff, UINT8 offset);

#endif /* RA217_TUNER_H */
