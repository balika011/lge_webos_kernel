/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : 2014/08/08
  File Revision : 1.9.0.1
------------------------------------------------------------------------------*/
#ifndef SONYRF_APMD_ASCOT3_MODULE_H
#define SONYRF_APMD_ASCOT3_MODULE_H

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

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
  sonyrf_i2c_t      *pI2c         I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Create(sonyrf_ascot3_t *pTuner, uint8_t i2cAddress, sonyrf_i2c_t *pI2c, uint8_t VariationID, uint8_t ProjectID);

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Initialize(sonyrf_ascot3_t *pTuner);

/*--------------------------------------------------------------------
  Tuning

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
  uint32_t        frequencykHz  RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system        Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Tune(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz, sonyrf_ascot3_tv_system_t system);

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Sleep(sonyrf_ascot3_t *pTuner);

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
  uint8_t         sw            Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_AntSw(sonyrf_ascot3_t *pTuner, uint8_t sw);
sonyrf_result_t sonyrf_apmd_ascot3_module_ExtLna(sonyrf_ascot3_t *pTuner, uint8_t ena);
/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_ascot3_t   *pTuner       Tuner struct instance
  uint32_t        frequencykHz  RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Shift(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz);

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  int32_t         *rssi_x100   pointer of RSSI information
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_RSSI(sonyrf_ascot3_t *pTuner, int32_t *rssi_x100);

#endif /* SONYRF_apmd_ascot3_module_H */
