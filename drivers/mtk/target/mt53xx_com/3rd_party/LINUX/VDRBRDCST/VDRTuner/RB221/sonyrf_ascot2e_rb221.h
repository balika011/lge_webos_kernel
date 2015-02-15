/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012/04/06
  File Revision : 0.1.0.0
------------------------------------------------------------------------------*/
#ifndef SONYRF_ASCOT2E_RB221_H
#define SONYRF_ASCOT2E_RB221_H

#include "sonyrf_ascot2e.h"

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  sonyrf_i2c_t       *pI2c        I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_Create(sonyrf_ascot2e_t *pTuner, sonyrf_i2c_t *pI2c);

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_Initialize(sonyrf_ascot2e_t *pTuner);

/*--------------------------------------------------------------------
  Tuning

  sonyrf_ascot2e_t          *pTuner      Tuner struct instance
  uint32_t                frequencykHz RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system       Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_Tune(sonyrf_ascot2e_t *pTuner, uint32_t frequencykHz, sonyrf_ascot2e_tv_system_t system);

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_Sleep(sonyrf_ascot2e_t *pTuner);

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  uint8_t          sw           Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_AntSw(sonyrf_ascot2e_t *pTuner, uint8_t sw);

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  uint32_t         frequencykHz RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_Shift(sonyrf_ascot2e_t *pTuner, uint32_t frequencykHz);

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  int16_t          *rssi_x100   pointer of RSSI information
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_rb221_RSSI(sonyrf_ascot2e_t *pTuner, int16_t *rssi_x100);

#endif /* SONYRF_ASCOT2E_RB221_H */
