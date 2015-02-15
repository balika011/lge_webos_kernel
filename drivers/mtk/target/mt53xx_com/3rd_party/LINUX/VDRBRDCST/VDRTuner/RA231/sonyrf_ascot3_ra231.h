/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : 2013/06/26
  File Revision : 1.1.3.1
------------------------------------------------------------------------------*/
#ifndef SONYRF_ASCOT3_RA231_H
#define SONYRF_ASCOT3_RA231_H

#include "sonyrf_ascot3.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
  sonyrf_i2c_t              *pI2c         I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_Create(sonyrf_ascot3_t *pTuner, sonyrf_i2c_t *pI2c);

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_Initialize(sonyrf_ascot3_t *pTuner);

/*--------------------------------------------------------------------
  Tuning

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
  uint32_t                  frequencykHz  RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system        Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_Tune(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz, sonyrf_ascot3_tv_system_t system);

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_Sleep(sonyrf_ascot3_t *pTuner);

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
  uint8_t                   sw            Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_AntSw(sonyrf_ascot3_t *pTuner, uint8_t sw);

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_ascot3_t           *pTuner       Tuner struct instance
  uint32_t                  frequencykHz  RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_Shift(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz);

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  int32_t                   *rssi_x10     pointer of RSSI information
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot3_ra231_RSSI(sonyrf_ascot3_t *pTuner, int32_t *rssi_x100);

#endif /* SONYRF_ASCOT3_RA231_H */
