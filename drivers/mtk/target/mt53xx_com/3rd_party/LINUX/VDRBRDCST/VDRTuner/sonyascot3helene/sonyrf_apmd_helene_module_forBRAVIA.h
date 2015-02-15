/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : 2014/08/08
  File Revision : 1.9.0.1
------------------------------------------------------------------------------*/
#ifndef SONYRF_APMD_HELENE_MODULE_H
#define SONYRF_APMD_HELENE_MODULE_H

#include "sonyrf_helene.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_apmd_t   *pTuner       Tuner struct instance
  sonyrf_i2c_t      *pI2c         I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Create(sonyrf_helene_t *pTuner, uint8_t i2cAddress, sonyrf_i2c_t *pI2c, uint8_t VariationID, uint8_t ProjectID);

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_apmd_t   *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Initialize(sonyrf_helene_t *pTuner);

/*--------------------------------------------------------------------
  Tuning

  sonyrf_apmd_t   *pTuner       Tuner struct instance
  uint32_t        frequencykHz  RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system        Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_terr_Tune(sonyrf_helene_t *pTuner, uint32_t frequencykHz, sonyrf_helene_tv_system_t system);
sonyrf_result_t sonyrf_apmd_helene_module_sat_Tune (sonyrf_helene_t *pTuner, uint32_t frequencykHz, sonyrf_helene_tv_system_t system, uint32_t symbolRateksps);
/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_apmd_t   *pTuner       Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Sleep(sonyrf_helene_t *pTuner);

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_apmd_t   *pTuner       Tuner struct instance
  uint8_t         sw            Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_terrAntSw(sonyrf_helene_t *pTuner, uint8_t sw);
sonyrf_result_t sonyrf_apmd_helene_module_satAntSw(sonyrf_helene_t *pTuner, uint8_t sw);
/*--------------------------------------------------------------------
  Control External LNA

  sonyrf_apmd_t   *pTuner      Tuner struct instance
  uint8_t         status       LNA Status (0:OFF 1:ON) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_TerrExtLna(sonyrf_helene_t *pTuner, uint8_t status);

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_apmd_t   *pTuner       Tuner struct instance
  uint32_t        frequencykHz  RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Shift(sonyrf_helene_t *pTuner, uint32_t frequencykHz);

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_apmd_t   *pTuner      Tuner struct instance
  int32_t         *rssi_x100   pointer of RSSI information
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_RSSI(sonyrf_helene_t *pTuner, int32_t *rssi_x100);

#endif /* SONYRF_APMD_HELENE_MODULE_H */
