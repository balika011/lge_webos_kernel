/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"
#include "sonysat_demod_dvbs_s2_blindscan_trace.h"

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_pm_Initialize");

    if ((!pSeq) || (!pCommonParams)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 0;
    pSeq->state = PM_STATE_START;
    pSeq->pCommonParams = pCommonParams;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq,
                                                            int32_t freqOffsetKHz)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_pm_Start");

    pSeq->isEnable = 1;
    pSeq->state = PM_STATE_START;
    pSeq->freqOffsetKHz = freqOffsetKHz;
    pSeq->power = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_pm_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_pm_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t elapsedTime = 0;
    uint8_t csfin = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_pm_Sequence");

    switch(pSeq->state)
    {
    case PM_STATE_START:
        result = sonysat_demod_dvbs_s2_blindscan_SetCFFine (pSeq->pCommonParams->pDemod, pSeq->freqOffsetKHz);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        /* Scan start */
        result = sonysat_demod_dvbs_s2_blindscan_PS_START (pSeq->pCommonParams->pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        pSeq->state = PM_STATE_WAITING_CSFIN;
        pSeq->pCommonParams->waitTime = 0;
        result = sonysat_stopwatch_start (&pSeq->stopwatch);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case PM_STATE_WAITING_CSFIN:
        result = pSeq->pfStopwatch_elapsed (&pSeq->stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_dvbs_s2_blindscan_GetCSFIN (pSeq->pCommonParams->pDemod, &csfin);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (csfin){
            result = sonysat_demod_dvbs_s2_blindscan_GetPSPow (pSeq->pCommonParams->pDemod, &pSeq->power);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Scan ack */
            result = sonysat_demod_dvbs_s2_blindscan_PS_RACK (pSeq->pCommonParams->pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* End */
            pSeq->isEnable = 0;
            SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_PM_POWER (pSeq->freqOffsetKHz, pSeq->power);
        } else {
            if (elapsedTime >= 1000){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
            } else {
                /* Continue to wait */
            }
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}
