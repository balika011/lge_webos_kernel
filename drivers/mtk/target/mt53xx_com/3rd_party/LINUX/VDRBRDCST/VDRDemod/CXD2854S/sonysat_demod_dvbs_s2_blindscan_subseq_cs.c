/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_pm.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_cs.h"
#include "sonysat_demod_dvbs_s2_blindscan_trace.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t get_step_cs (int32_t stepCount, int32_t * pValue);
static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq);
static sonysat_result_t finish_ng (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_cs_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeqPM,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_cs_Initialize");

    if ((!pSeq) || (!pSeqPM) || (!pCommonParams)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 0;
    pSeq->state = CS_STATE_START;
    pSeq->pSeqPM = pSeqPM;
    pSeq->pCommonParams = pCommonParams;
    pSeq->freqOffsetKHz = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_cs_Start (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq,
                                                            int32_t freqOffsetKHz)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_cs_Start");

    if (!pSeq){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_START (freqOffsetKHz);

    pSeq->isEnable = 1;
    pSeq->state = CS_STATE_START;
    pSeq->freqOffsetKHz = freqOffsetKHz;
    pSeq->isExist = 0;
    pSeq->coarseSymbolRateKSps = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_cs_Sequence(sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    int32_t power = 0;
    int32_t stepFreqKHz = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_cs_Sequence");

    if(!pSeq) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(pSeq->state)
    {
    case CS_STATE_START:
        result = sonysat_demod_dvbs_s2_blindscan_CS_INIT (pSeq->pCommonParams->pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->pCommonParams->waitTime = 0;
        pSeq->state = CS_STATE_PEAK_SEARCH_START;
        /* Set init value for index */
        pSeq->index = -5;
        break;

    case CS_STATE_PEAK_SEARCH_START:
        result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->freqOffsetKHz + ((int32_t)(pSeq->index) * 100));
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->state = CS_STATE_PEAK_SEARCHING;
        break;

    case CS_STATE_PEAK_SEARCHING:
        /* Get result */
        power = (int32_t)(pSeq->pSeqPM->power);
        if ((pSeq->index == -5) || (pSeq->peakPower < power)){
            pSeq->peakPower = power;
            pSeq->peakFreqOfsKHz = (pSeq->index) * 100;
        }
        pSeq->index++;
        if(pSeq->index <= 5){
            /* Continue peak search */
            result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->freqOffsetKHz + (pSeq->index * 100));
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            /* Next stage */
            pSeq->state = CS_STATE_LOWER_SEARCHING;
            pSeq->index = 1;
            pSeq->isFin = 0;
            result = get_step_cs (pSeq->index, &stepFreqKHz);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            pSeq->lowerFreqKHz = pSeq->freqOffsetKHz + (pSeq->peakFreqOfsKHz - stepFreqKHz);
            result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->lowerFreqKHz);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case CS_STATE_LOWER_SEARCHING:
        power = (int32_t)(pSeq->pSeqPM->power);
        if ((pSeq->peakPower * 100) > (power * 137)){
            /* Next stage */
            pSeq->state = CS_STATE_UPPER_SEARCHING;
            pSeq->index = 1;
            result = get_step_cs (pSeq->index, &stepFreqKHz);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            pSeq->upperFreqKHz = pSeq->freqOffsetKHz + (pSeq->peakFreqOfsKHz + stepFreqKHz);
            result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->upperFreqKHz);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else if ((power * 100) > (pSeq->peakPower * 150)){
            /* NG */
            pSeq->lowerFreqKHz = -1000; /* Invalid */
            result = finish_ng (pSeq);
        } else {
            pSeq->index++;
            if (pSeq->index < 27){
                /* Continue */
                result = get_step_cs (pSeq->index, &stepFreqKHz);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                pSeq->lowerFreqKHz = pSeq->freqOffsetKHz + (pSeq->peakFreqOfsKHz - stepFreqKHz);
                result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->lowerFreqKHz);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            } else {
                /* NG */
                pSeq->lowerFreqKHz = -1000; /* Invalid */
                result = finish_ng (pSeq);
            }
        }
        break;

    case CS_STATE_UPPER_SEARCHING:
        power = (int32_t)(pSeq->pSeqPM->power);
        if ((pSeq->peakPower * 100) > (power * 137)){
            /* OK */
            result = finish_ok (pSeq);
        } else if ((power * 100) > (pSeq->peakPower * 150)){
            /* NG */
            pSeq->upperFreqKHz = -1000; /* Invalid */
            result = finish_ng (pSeq);
        } else {
            pSeq->index++;
            if (pSeq->index < 27){
                /* Continue */
                result = get_step_cs (pSeq->index, &stepFreqKHz);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                pSeq->upperFreqKHz = pSeq->freqOffsetKHz + (pSeq->peakFreqOfsKHz + stepFreqKHz);
                result = sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (pSeq->pSeqPM, pSeq->upperFreqKHz);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            } else {
                /* NG */
                pSeq->upperFreqKHz = -1000; /* Invalid */
                result = finish_ng (pSeq);
            }
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t get_step_cs (int32_t stepCount, int32_t * pValue)
{
    static const int32_t step_table[] = {
        0,     /* 100(KHz) * 0         */
        100,   /* 100(KHz) * 1         */
        200,   /* 100(KHz) * 2         */
        300,   /* 100(KHz) * 3         */
        400,   /* 400(KHz) * (1.1^0)   */
        440,   /* 400(KHz) * (1.1^1)   */
        484,   /* 400(KHz) * (1.1^2)   */
        532,   /* 400(KHz) * (1.1^3)   */
        586,   /* 400(KHz) * (1.1^4)   */
        644,   /* 400(KHz) * (1.1^5)   */
        709,   /* 400(KHz) * (1.1^6)   */
        779,   /* 400(KHz) * (1.1^7)   */
        857,   /* 400(KHz) * (1.1^8)   */
        943,   /* 400(KHz) * (1.1^9)   */
        1037,  /* 400(KHz) * (1.1^10)  */
        1141,  /* 400(KHz) * (1.1^11)  */
        1255,  /* 400(KHz) * (1.1^12)  */
        1381,  /* 400(KHz) * (1.1^13)  */
        1519,  /* 400(KHz) * (1.1^14)  */
        1671,  /* 400(KHz) * (1.1^15)  */
        1838,  /* 400(KHz) * (1.1^16)  */
        2022,  /* 400(KHz) * (1.1^17)  */
        2224,  /* 400(KHz) * (1.1^18)  */
        2446,  /* 400(KHz) * (1.1^19)  */
        2691,  /* 400(KHz) * (1.1^20)  */
        2960,  /* 400(KHz) * (1.1^21)  */
        3256,  /* 400(KHz) * (1.1^22)  */
        3582   /* 400(KHz) * (1.1^23)  */
    };

    SONYSAT_TRACE_ENTER ("get_step_cs");

    if ((!pValue) || (stepCount < 0) || (stepCount > 27)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    *pValue = step_table[stepCount];

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("finish_ok");
    if(!pSeq) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    if (pSeq->upperFreqKHz > pSeq->lowerFreqKHz){
        pSeq->coarseSymbolRateKSps = (uint32_t)(pSeq->upperFreqKHz - pSeq->lowerFreqKHz);
        pSeq->isExist = 1;
        SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_RESULT(pSeq->coarseSymbolRateKSps);
    } else {
        pSeq->coarseSymbolRateKSps = 0;
        pSeq->isExist = 0;
        SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_RESULT(1);
    }
    pSeq->isEnable = 0;
    result = sonysat_demod_dvbs_s2_blindscan_CS_FIN (pSeq->pCommonParams->pDemod);
    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t finish_ng (sonysat_demod_dvbs_s2_blindscan_subseq_cs_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("finish_ng");
    if(!pSeq) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    pSeq->coarseSymbolRateKSps = 0;
    pSeq->isExist = 0;
    pSeq->isEnable = 0;
    SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_CS_RESULT(0);
    result = sonysat_demod_dvbs_s2_blindscan_CS_FIN (pSeq->pCommonParams->pDemod);
    SONYSAT_TRACE_RETURN (result);
}
