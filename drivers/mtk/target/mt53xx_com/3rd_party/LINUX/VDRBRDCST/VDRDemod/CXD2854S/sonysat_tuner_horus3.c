/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_i2c.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_tuner_horus3.h"
#include "sonysat_horus3.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t sonysat_tuner_horus3_Initialize (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_horus3_Tune (sonysat_tuner_sat_t * pTuner,
                                             uint32_t centerFreqKHz,
                                             sonysat_dtv_system_t dtvSystem,
                                             uint32_t symbolRateKSps);

static sonysat_result_t sonysat_tuner_horus3_Sleep (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_horus3_Shutdown (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_horus3_AGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                       uint32_t AGCLevel,
                                                       int32_t * pAGCdB);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_tuner_horus3_Create (sonysat_tuner_sat_t * pTuner,
                                        uint32_t xtalFreqMHz,
                                        uint8_t i2cAddress,
                                        sonysat_i2c_t * pI2c,
                                        uint32_t configFlags,
                                        sonysat_horus3_t * pHorus3)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_tuner_horus3_Create");

    if ((!pI2c) || (!pHorus3) || (!pTuner)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_horus3_Create (pHorus3, xtalFreqMHz, i2cAddress, pI2c, configFlags);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    pTuner->i2cAddress = i2cAddress;
    pTuner->pI2c = pI2c;
    pTuner->flags = configFlags;
    pTuner->frequencyKHz = 0;
    pTuner->system = SONYSAT_DTV_SYSTEM_UNKNOWN;
    pTuner->symbolRateKSps = 0;
    pTuner->symbolRateKSpsForSpectrum = 45000;
    pTuner->Initialize = sonysat_tuner_horus3_Initialize;
    pTuner->Tune = sonysat_tuner_horus3_Tune;
    pTuner->Sleep = sonysat_tuner_horus3_Sleep;
    pTuner->Shutdown = sonysat_tuner_horus3_Shutdown;
    pTuner->AGCLevel2AGCdB = sonysat_tuner_horus3_AGCLevel2AGCdB;
    pTuner->user = pHorus3;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t sonysat_tuner_horus3_Initialize (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_horus3_t * pHorus3 = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_tuner_horus3_Initialize");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pHorus3 = (sonysat_horus3_t *)(pTuner->user);

    result = sonysat_horus3_Initialize (pHorus3);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    pTuner->frequencyKHz = 0;
    pTuner->system = SONYSAT_DTV_SYSTEM_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t sonysat_tuner_horus3_Tune (sonysat_tuner_sat_t * pTuner,
                                             uint32_t centerFreqKHz,
                                             sonysat_dtv_system_t dtvSystem,
                                             uint32_t symbolRateKSps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_horus3_t * pHorus3 = NULL;
    sonysat_horus3_tv_system_t satSystem = SONYSAT_HORUS3_STV_DVBS2;

    SONYSAT_TRACE_ENTER ("sonysat_tuner_horus3a_Tune");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pHorus3 = (sonysat_horus3_t *)(pTuner->user);

    switch(dtvSystem)
    {
    case SONYSAT_DTV_SYSTEM_DVBS:
        satSystem = SONYSAT_HORUS3_STV_DVBS;
        break;

    case SONYSAT_DTV_SYSTEM_DVBS2:
        satSystem = SONYSAT_HORUS3_STV_DVBS2;
        break;

    case SONYSAT_DTV_SYSTEM_ISDBS:
        satSystem = SONYSAT_HORUS3_STV_ISDBS;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_horus3_Tune (pHorus3, centerFreqKHz, satSystem, symbolRateKSps);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    pTuner->frequencyKHz = pHorus3->frequencykHz;
    pTuner->system = dtvSystem;
    pTuner->symbolRateKSps = symbolRateKSps;

    /* Tuner stabillization time */
    SONYSAT_SLEEP (50);

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t sonysat_tuner_horus3_Sleep (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_horus3_t * pHorus3 = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_tuner_horus3_Sleep");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pHorus3 = (sonysat_horus3_t *)(pTuner->user);

    result = sonysat_horus3_Sleep (pHorus3);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    pTuner->frequencyKHz = 0;
    pTuner->system = SONYSAT_DTV_SYSTEM_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t sonysat_tuner_horus3_Shutdown (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_horus3_t * pHorus3 = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_tuner_horus3_Shutdown");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pHorus3 = (sonysat_horus3_t *)(pTuner->user);

    result = sonysat_horus3_Sleep (pHorus3);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    pTuner->frequencyKHz = 0;
    pTuner->system = SONYSAT_DTV_SYSTEM_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t sonysat_tuner_horus3_AGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                       uint32_t AGCLevel,
                                                       int32_t * pAGCdB)
{
    int32_t tempA = 0;
    uint8_t isNegative = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    SONYSAT_TRACE_ENTER("sonysat_tuner_horus3_AGCLevel2AGCdB");

    if ((!pTuner) || (!pAGCdB)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /*------------------------------------------------
      Gain_db      = (AGCLevel - 8192) * (-  33/1024) -   142
      Gain_db_x100 = (AGCLevel - 8192) * (-3300/1024) - 14200
                   = (AGCLevel - 8192) * (- 825/ 256) - 14200
    ------------------------------------------------*/
    tempA = (int32_t)AGCLevel - 8192;
    tempA *= -825;

    tempDiv = 256;
    if (tempA > 0){
        isNegative = 0;
        tempQ = (uint32_t)tempA / tempDiv;
        tempR = (uint32_t)tempA % tempDiv;
    } else {
        isNegative = 1;
        tempQ = (uint32_t)(tempA * (-1)) / tempDiv;
        tempR = (uint32_t)(tempA * (-1)) % tempDiv;
    }

    if (isNegative){
        if (tempR >= (tempDiv/2)){
            *pAGCdB = (int32_t)(tempQ + 1) * (int32_t)(-1);
        } else {
            *pAGCdB = (int32_t)tempQ * (int32_t)(-1);
        }
    } else {
        if (tempR >= (tempDiv/2)){
            *pAGCdB = (int32_t)(tempQ + 1);
        } else {
            *pAGCdB = (int32_t)tempQ;
        }
    }
    *pAGCdB -= 14200;
    
    if(*pAGCdB < 0){
        *pAGCdB = 0;
    }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}
