/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
/**
 @file    sonysat_demod_dvbs_s2_blindscan_utility.h

          This file provides the functions for alloc/free data.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_UTILITY_H
#define SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_UTILITY_H

#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"

/**
 @brief Initialize storage instance.

 @param pPowerArrayTop Pointer of array for power data.
 @param powerArrayLength Length of array for power data "pPowerArrayTop".
 @param pDataArrayTop Pointer of array for data.
 @param dataArrayLength Length of array for data "pDataArrayTop".

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_StorageInit (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                        sonysat_demod_dvbs_s2_blindscan_power_t * pPowerArrayTop,
                                                        int32_t powerArrayLength,
                                                        sonysat_demod_dvbs_s2_blindscan_data_t * pDataArrayTop,
                                                        int32_t dataArrayLength);

/**
 @brief Allocate power data.

 @param pStorage Storage instance.
 @param ppPower Address of address for power data.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_AllocPower (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                       sonysat_demod_dvbs_s2_blindscan_power_t ** ppPower);

/**
 @brief Free power data.

 @param pStorage Storage instance.
 @param pPower Address for power data.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_FreePower (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                      sonysat_demod_dvbs_s2_blindscan_power_t * pPower);

/**
 @brief Clear power list.

 @param pStorage Storage instance.
 @param pListTop Address for power list.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_ClearPowerList (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                           sonysat_demod_dvbs_s2_blindscan_power_t * pListTop);

/**
 @brief Allocate data.

 @param pStorage Storage instance.
 @param ppPower Address of address for data.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_AllocData (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                      sonysat_demod_dvbs_s2_blindscan_data_t ** ppData);

/**
 @brief Free data.

 @param pStorage Storage instance.
 @param pPower Address for data.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_FreeData (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                     sonysat_demod_dvbs_s2_blindscan_data_t * pData);

/**
 @brief Clear data list.

 @param pStorage Storage instance.
 @param pListTop Address for data list.

 @return SONYSAT_RESULT_OK if successful.
*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_ClearDataList (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                          sonysat_demod_dvbs_s2_blindscan_data_t * pListTop);

#endif /* SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_UTILITY_H */
