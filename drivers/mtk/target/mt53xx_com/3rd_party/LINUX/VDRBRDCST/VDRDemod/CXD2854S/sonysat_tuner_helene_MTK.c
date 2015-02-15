/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_tuner_sat.h"
#include "sonysat_i2c.h"
#include "sonysat_tuner_helene_MTK.h"

#if defined(CC_PROJECT_FOR_SONY) || defined(SONYSAT_TUNERMODULE_HELENE)
extern INT16 HELENE_Satellite_Tune(UINT8 tunerID, UINT32 freq, UINT8 tvSystem, UINT32 symbolRate);
#endif

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t sonysat_tuner_helene_MTK_Initialize (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_helene_MTK_Tune (sonysat_tuner_sat_t * pTuner,
                                                       uint32_t centerFreqKHz,
                                                       sonysat_dtv_system_t dtvSystem,
                                                       uint32_t symbolRateKSps);

static sonysat_result_t sonysat_tuner_helene_MTK_Sleep (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_helene_MTK_Shutdown (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t sonysat_tuner_helene_MTK_sat_AGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                                     uint32_t AGCLevel,
                                                                     int32_t * pAGCdB);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_tuner_helene_MTK_Create (sonysat_tuner_sat_t * pTuner,
                                                  uint8_t tunerID,
                                                  sonysat_helene_mtk_t * pHelene)
{
    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Create");

    if ((!pTuner) || (!pHelene)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pHelene->tunerID = tunerID;

    pTuner->i2cAddress = 0;
    pTuner->pI2c = NULL;
    pTuner->flags = 0;
    pTuner->frequencyKHz = 0;
    pTuner->system = SONYSAT_DTV_SYSTEM_UNKNOWN;
    pTuner->symbolRateKSps = 0;
    pTuner->symbolRateKSpsForSpectrum = 29000;
    pTuner->Initialize = sonysat_tuner_helene_MTK_Initialize;
    pTuner->Tune = sonysat_tuner_helene_MTK_Tune;
    pTuner->Sleep = sonysat_tuner_helene_MTK_Sleep;
    pTuner->Shutdown = sonysat_tuner_helene_MTK_Shutdown;
    pTuner->AGCLevel2AGCdB = sonysat_tuner_helene_MTK_sat_AGCLevel2AGCdB;
    pTuner->user = pHelene;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t sonysat_tuner_helene_MTK_Initialize (sonysat_tuner_sat_t * pTuner)
{
    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Initialize");
    /* Do nothing */
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t sonysat_tuner_helene_MTK_Tune (sonysat_tuner_sat_t * pTuner,
                                                       uint32_t centerFreqKHz,
                                                       sonysat_dtv_system_t dtvSystem,
                                                       uint32_t symbolRateKSps)
{
#ifdef SONYSAT_TUNERMODULE_HELENE
    sonysat_result_t result = SONYSAT_RESULT_OK;
    INT16 tuneResult = 0;
    INT8 satSystem = 0;
    UINT8 tunerID = 0;
    sonysat_helene_mtk_t * pHelene = (sonysat_helene_mtk_t*)(pTuner->user);

    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Tune");

    if ((!pTuner) || (!pHelene)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(dtvSystem)
    {
    case SONYSAT_DTV_SYSTEM_DVBS:
        satSystem = 0;
        break;

    case SONYSAT_DTV_SYSTEM_DVBS2:
        satSystem = 1;
        break;

    case SONYSAT_DTV_SYSTEM_ISDBS:
        satSystem = 2;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    tunerID = pHelene->tunerID;

    tuneResult = HELENE_Satellite_Tune (tunerID, centerFreqKHz, satSystem, symbolRateKSps);

    pTuner->frequencyKHz = ((centerFreqKHz + 125) / 250) * 250;
    pTuner->system = dtvSystem;
    pTuner->symbolRateKSps = symbolRateKSps;

    /* Tuner stabillization time */
    SONYSAT_SLEEP (50);

    if (tuneResult == 0) {
        result = SONYSAT_RESULT_OK;
    } else {
        result = SONYSAT_RESULT_ERROR_UNLOCK;
    }
    SONYSAT_TRACE_RETURN (result);
#else
    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Tune");
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_OTHER);
#endif
}

static sonysat_result_t sonysat_tuner_helene_MTK_Sleep (sonysat_tuner_sat_t * pTuner)
{
    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Sleep");
    /* Do nothing */
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t sonysat_tuner_helene_MTK_Shutdown (sonysat_tuner_sat_t * pTuner)
{
    SONYSAT_TRACE_ENTER ("sonysat_tuner_helene_MTK_Shutdown");
    /* Do nothing */
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t sonysat_tuner_helene_MTK_sat_AGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                                     uint32_t AGCLevel,
                                                                     int32_t * pAGCdB)
{
    int32_t tempA = 0;
    uint8_t isNegative = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    SONYSAT_TRACE_ENTER("sonysat_tuner_helene_MTK_sat_AGCLevel2AGCdB");

    if ((!pTuner) || (!pAGCdB)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pTuner->flags & SONYSAT_HELENE_MTK_CONFIG_SAT_LNA_OFF) {
        /*------------------------------------------------------------
           LNA Off
           Gain_db      = (AGCLevel - 8192) * (-38/1024) - 214
           Gain_db_x100 = (AGCLevel - 8192) * (-3800/1024) - 21400
                        = (AGCLevel - 8192) * (-475/128) - 21400
         -----------------------------------------------------------*/
        tempA = ((int32_t)AGCLevel - 8192) * (-475);
        tempDiv = 128;
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
        *pAGCdB -= 21400;
    } else {
        /*------------------------------------------------------------
         LNA On
         Gain_db      = (AGCLevel - 8192) * (-29/1024) - 125
         Gain_db_x100 = (AGCLevel - 8192) * (-2900/1024) - 12500
                      = (AGCLevel - 8192) * (-2900/1024) - 12500
                      = (AGCLevel - 8192) * (-725/256) - 12500
         -----------------------------------------------------------*/
        tempA = ((int32_t)AGCLevel - 8192) * (-725);
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
        *pAGCdB -= 12500;
    }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}
