/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t incrementMemCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage);
static sonysat_result_t decrementMemCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage);
static sonysat_result_t incrementPowCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage);
static sonysat_result_t decrementPowCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_StorageInit (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                        sonysat_demod_dvbs_s2_blindscan_power_t * pPowerArrayTop,
                                                        int32_t powerArrayLength,
                                                        sonysat_demod_dvbs_s2_blindscan_data_t * pDataArrayTop,
                                                        int32_t dataArrayLength)
{
    int32_t i = 0;
    sonysat_demod_dvbs_s2_blindscan_power_t * pPowerCurrent = NULL;
    sonysat_demod_dvbs_s2_blindscan_data_t * pDataCurrent = NULL;

    if ((!pStorage) || (!pPowerArrayTop) || (powerArrayLength <= 0) || (!pDataArrayTop) || (dataArrayLength <= 0)){
        return SONYSAT_RESULT_ERROR_ARG;
    }

    pStorage->pPowerArrayTop = pPowerArrayTop;
    pStorage->powerArrayLength = powerArrayLength;
    pStorage->pDataArrayTop = pDataArrayTop;
    pStorage->dataArrayLength = dataArrayLength;

    pPowerCurrent = &(pStorage->availablePowerList);
    for(i = 0; i < powerArrayLength; i++){
        pPowerCurrent->pNext = (pPowerArrayTop + i);
        pPowerCurrent = pPowerCurrent->pNext;
    }
    pPowerCurrent->pNext = NULL;
    pStorage->currentUsedPowerCount = 0;
    pStorage->maxUsedPowerCount = 0;

    pDataCurrent = &(pStorage->availableDataList);
    for(i = 0; i < dataArrayLength; i++){
        pDataCurrent->pNext = (pDataArrayTop + i);
        pDataCurrent = pDataCurrent->pNext;
    }
    pDataCurrent->pNext = NULL;
    pStorage->currentUsedCount = 0;
    pStorage->maxUsedCount = 0;

    return SONYSAT_RESULT_OK;
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_AllocPower (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                       sonysat_demod_dvbs_s2_blindscan_power_t ** ppPower)
{
    if(pStorage->availablePowerList.pNext != NULL){
        *ppPower = pStorage->availablePowerList.pNext;
        if((*ppPower)->pNext != NULL){
            pStorage->availablePowerList.pNext = (*ppPower)->pNext;
        } else {
            /* Alloc the last data. */
            pStorage->availablePowerList.pNext = NULL;
        }
    } else {
        /* Overflow. */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    }
    (*ppPower)->pNext = NULL;
    return incrementPowCount (pStorage);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_FreePower (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                      sonysat_demod_dvbs_s2_blindscan_power_t * pPower)
{
    sonysat_demod_dvbs_s2_blindscan_power_t * pTemp = NULL;
    if(pStorage->availablePowerList.pNext != NULL){
        pTemp = pStorage->availablePowerList.pNext;
        pStorage->availablePowerList.pNext = pPower;
        pPower->pNext = pTemp;
    } else {
        /* Data is empty. */
        pStorage->availablePowerList.pNext = pPower;
        pPower->pNext = NULL;
    }
    return decrementPowCount (pStorage);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_ClearPowerList (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                           sonysat_demod_dvbs_s2_blindscan_power_t * pListTop)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_demod_dvbs_s2_blindscan_power_t * pCurrent = NULL;
    sonysat_demod_dvbs_s2_blindscan_power_t * pTemp = NULL;

    if ((!pStorage) || (!pListTop)){
        return SONYSAT_RESULT_ERROR_ARG;
    }

    pCurrent = pListTop->pNext;

    while (pCurrent){
        pTemp = pCurrent->pNext;
        result = sonysat_demod_dvbs_s2_blindscan_FreePower (pStorage, pCurrent);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pCurrent = pTemp;
    }

    pListTop->pNext = NULL;

    return SONYSAT_RESULT_OK;
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_AllocData (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                      sonysat_demod_dvbs_s2_blindscan_data_t ** ppData)
{
    if (!pStorage){
        return SONYSAT_RESULT_ERROR_ARG;
    }

    if(pStorage->availableDataList.pNext){
        *ppData = pStorage->availableDataList.pNext;
        if((*ppData)->pNext){
            pStorage->availableDataList.pNext = (*ppData)->pNext;
        } else {
            /* Alloc the last data. */
            pStorage->availableDataList.pNext = NULL;
        }
    } else {
        /* Overflow. */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    }
    (*ppData)->pNext = NULL;
    return incrementMemCount(pStorage);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_FreeData (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                     sonysat_demod_dvbs_s2_blindscan_data_t * pData)
{
    sonysat_demod_dvbs_s2_blindscan_data_t * pTemp = NULL;

    if ((!pStorage) || (!pData)){
        return SONYSAT_RESULT_ERROR_OTHER;
    }

    if(pStorage->availableDataList.pNext){
        pTemp = pStorage->availableDataList.pNext;
        pStorage->availableDataList.pNext = pData;
        pData->pNext = pTemp;
    } else {
        /* Data is empty. */
        pStorage->availableDataList.pNext = pData;
        pData->pNext = NULL;
    }
    return decrementMemCount(pStorage);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_ClearDataList (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage,
                                                          sonysat_demod_dvbs_s2_blindscan_data_t * pListTop)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_demod_dvbs_s2_blindscan_data_t * pCurrent = NULL;
    sonysat_demod_dvbs_s2_blindscan_data_t * pTemp = NULL;

    if ((!pStorage) || (!pListTop)){
        return SONYSAT_RESULT_ERROR_ARG;
    }

    pCurrent = pListTop->pNext;

    while (pCurrent){
        pTemp = pCurrent->pNext;
        result = sonysat_demod_dvbs_s2_blindscan_FreeData (pStorage, pCurrent);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pCurrent = pTemp;
    }

    pListTop->pNext = NULL;

    return SONYSAT_RESULT_OK;
}

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t incrementMemCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage)
{
    pStorage->currentUsedCount++;
    if(pStorage->currentUsedCount <= 0){
        /* Error(Overflow) */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    }
    if(pStorage->maxUsedCount < pStorage->currentUsedCount){
        pStorage->maxUsedCount = pStorage->currentUsedCount;
    }
    return SONYSAT_RESULT_OK;
}

static sonysat_result_t decrementMemCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage)
{
    if(pStorage->currentUsedCount == 0){
        /* Error(Overflow) */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    } else {
        pStorage->currentUsedCount--;
    }
    return SONYSAT_RESULT_OK;
}

static sonysat_result_t incrementPowCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage)
{
    pStorage->currentUsedPowerCount++;
    if(pStorage->currentUsedPowerCount <= 0){
        /* Error(Overflow) */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    }
    if(pStorage->maxUsedPowerCount < pStorage->currentUsedPowerCount){
        pStorage->maxUsedPowerCount = pStorage->currentUsedPowerCount;
    }
    return SONYSAT_RESULT_OK;
}

static sonysat_result_t decrementPowCount (sonysat_demod_dvbs_s2_blindscan_data_storage_t * pStorage)
{
    if(pStorage->currentUsedPowerCount == 0){
        /* Error(Overflow) */
        return SONYSAT_RESULT_ERROR_OVERFLOW;
    } else {
        pStorage->currentUsedPowerCount--;
    }
    return SONYSAT_RESULT_OK;
}
