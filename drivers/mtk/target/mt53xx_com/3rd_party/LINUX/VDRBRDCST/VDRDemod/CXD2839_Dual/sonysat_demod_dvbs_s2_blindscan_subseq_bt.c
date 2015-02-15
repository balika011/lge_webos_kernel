/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_subseq_bt.h"
#include "sonysat_demod_dvbs_s2_blindscan_trace.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq);
static sonysat_result_t finish_ng (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_bt_Initialize");

    if ((!pSeq) || (!pCommonParams)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 0;
    pSeq->state = BT_STATE_INIT;
    pSeq->pCommonParams = pCommonParams;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Start (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq,
                                                            uint32_t centerFreqKHz,
                                                            uint32_t candSymbolRateKSps,
                                                            uint32_t minCandSymbolRateKSps,
                                                            uint32_t maxCandSymbolRateKSps,
                                                            uint32_t minSymbolRateKSps,
                                                            uint32_t maxSymbolRateKSps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_bt_Start");

    if (!pSeq){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 1;
    pSeq->state = BT_STATE_START;
    pSeq->centerFreqKHz = centerFreqKHz;
    pSeq->candSymbolRateKSps = candSymbolRateKSps;
    pSeq->minCandSymbolRateKSps = minCandSymbolRateKSps;
    pSeq->maxCandSymbolRateKSps = maxCandSymbolRateKSps;
    pSeq->minSymbolRateKSps = minSymbolRateKSps;
    pSeq->maxSymbolRateKSps = maxSymbolRateKSps;
    pSeq->isLocked = 0;

    pSeq->pCommonParams->waitTime = 0;

    SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_START (centerFreqKHz, candSymbolRateKSps, minCandSymbolRateKSps, maxCandSymbolRateKSps, minSymbolRateKSps, maxSymbolRateKSps);

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_bt_Sequence (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t srsfin = 0;
    uint8_t trllock = 0;
    uint32_t elapsedTime = 0;
    uint32_t detSymbolRateSps = 0;
    uint32_t detSymbolRateKSps = 0;
    uint32_t ratioMax = 0;
    uint32_t ratioMin = 0;
    sonysat_demod_lock_result_t lockState;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_bt_Sequence");

    switch(pSeq->state)
    {
    case BT_STATE_START:

        if (pSeq->maxCandSymbolRateKSps < pSeq->minSymbolRateKSps){
            result = finish_ng (pSeq);
            SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(range)");
            SONYSAT_TRACE_RETURN (result);
        }

        if (pSeq->maxCandSymbolRateKSps >= 20000){
            /* HS mode */
            result = sonysat_demod_dvbs_s2_blindscan_SetSampleMode (pSeq->pCommonParams->pDemod, 1);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            /* LS mode */
            result = sonysat_demod_dvbs_s2_blindscan_SetSampleMode (pSeq->pCommonParams->pDemod, 0);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }

        /* Clip 1 */
        if (pSeq->candSymbolRateKSps < pSeq->minSymbolRateKSps) {
            pSeq->candSymbolRateKSps = pSeq->minSymbolRateKSps;
        }
        if (pSeq->candSymbolRateKSps > pSeq->maxSymbolRateKSps){
            pSeq->candSymbolRateKSps = pSeq->maxSymbolRateKSps;
        }
        if (pSeq->minCandSymbolRateKSps < pSeq->minSymbolRateKSps) {
            pSeq->minCandSymbolRateKSps = pSeq->minSymbolRateKSps;
        }
        if (pSeq->minCandSymbolRateKSps > pSeq->maxSymbolRateKSps){
            pSeq->minCandSymbolRateKSps = pSeq->maxSymbolRateKSps;
        }
        if (pSeq->maxCandSymbolRateKSps < pSeq->minSymbolRateKSps) {
            pSeq->maxCandSymbolRateKSps = pSeq->minSymbolRateKSps;
        }
        if (pSeq->maxCandSymbolRateKSps > pSeq->maxSymbolRateKSps){
            pSeq->maxCandSymbolRateKSps = pSeq->maxSymbolRateKSps;
        }

        /* Clip 2 */
        if (pSeq->maxCandSymbolRateKSps < pSeq->candSymbolRateKSps){
            pSeq->maxCandSymbolRateKSps = pSeq->candSymbolRateKSps;
        }
        if (pSeq->maxCandSymbolRateKSps > (pSeq->candSymbolRateKSps * 2)){
            pSeq->maxCandSymbolRateKSps = (pSeq->candSymbolRateKSps * 2);
        }
        if (pSeq->minCandSymbolRateKSps > pSeq->candSymbolRateKSps){
            pSeq->minCandSymbolRateKSps = pSeq->candSymbolRateKSps;
        }

        /* Set target symbol rate */
        result = sonysat_demod_dvbs_s2_SetSymbolRate (pSeq->pCommonParams->pDemod, pSeq->candSymbolRateKSps);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if(pSeq->candSymbolRateKSps == 0){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }

        ratioMax = ((pSeq->maxCandSymbolRateKSps * 1024) / (pSeq->candSymbolRateKSps)) - 1024;
        ratioMin = ((pSeq->minCandSymbolRateKSps * 1024) / (pSeq->candSymbolRateKSps));

        if (ratioMin >= 1024){
            ratioMin = 1023;
        }

        result = sonysat_demod_dvbs_s2_blindscan_SetSymbolRateRatio (pSeq->pCommonParams->pDemod, ratioMin, ratioMax);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        /* Tune */
        pSeq->pCommonParams->tuneReq.isRequest = 1;
        pSeq->pCommonParams->tuneReq.frequencyKHz = pSeq->centerFreqKHz;
        pSeq->pCommonParams->tuneReq.system = SONYSAT_DTV_SYSTEM_DVBS;
        pSeq->pCommonParams->tuneReq.symbolRateKSps = pSeq->maxCandSymbolRateKSps;
        pSeq->pCommonParams->waitTime = 0;
        pSeq->state = BT_STATE_RF_TUNED;
        break;

    case BT_STATE_RF_TUNED:
        result = sonysat_demod_SoftReset (pSeq->pCommonParams->pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        result = sonysat_stopwatch_start (&(pSeq->stopwatch));
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->pCommonParams->waitTime = 10;
        pSeq->state = BT_STATE_WAIT_SRSFIN;
        break;

    case BT_STATE_WAIT_SRSFIN:
        result = pSeq->pfStopwatch_elapsed (&(pSeq->stopwatch), &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        result = sonysat_demod_dvbs_s2_blindscan_GetSRSFIN (pSeq->pCommonParams->pDemod, &srsfin);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if (srsfin){
            /* Check TRL lock */
            result = sonysat_demod_dvbs_s2_blindscan_CheckTRLLock (pSeq->pCommonParams->pDemod, &trllock);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            if (trllock){
                /* Symbol rate information */
                result = sonysat_demod_dvbs_s2_monitor_SymbolRate (pSeq->pCommonParams->pDemod, &detSymbolRateSps);
                switch(result)
                {
                case SONYSAT_RESULT_OK:
                    /* Set symbol rate */
                    pSeq->detSymbolRateSps = detSymbolRateSps;
                    break;

                case SONYSAT_RESULT_ERROR_HW_STATE:
                    /* Not detect */
                    SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(SR)");
                    result = finish_ng (pSeq);
                    SONYSAT_TRACE_RETURN (result);

                default:
                    SONYSAT_TRACE_RETURN (result);
                }
                detSymbolRateKSps = (detSymbolRateSps + 500) / 1000;
                pSeq->timeout = ((3600000 + (detSymbolRateKSps - 1)) / detSymbolRateKSps) + 150;
                result = sonysat_stopwatch_start (&(pSeq->stopwatch));
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                pSeq->state = BT_STATE_WAIT_TSLOCK;
                pSeq->pCommonParams->waitTime = 10;
            } else {
                /* TRL unlock */
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(TRL unlock)");
                result = finish_ng (pSeq);
            }
        } else {
            if (elapsedTime > 10000){
                /* Timeout */
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(SRS timeout)");
                result = finish_ng (pSeq);
            } else {
                /* Continue */
                pSeq->pCommonParams->waitTime = 10;
                pSeq->state = BT_STATE_WAIT_SRSFIN;
            }
        }
        break;

    case BT_STATE_WAIT_TSLOCK:
        result = pSeq->pfStopwatch_elapsed (&(pSeq->stopwatch), &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        result = sonysat_demod_dvbs_s2_CheckTSLock (pSeq->pCommonParams->pDemod, &lockState);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if (lockState == SONYSAT_DEMOD_LOCK_RESULT_LOCKED){
            /* Carrier offset frequency */
            result = sonysat_demod_dvbs_s2_monitor_CarrierOffset (pSeq->pCommonParams->pDemod, &(pSeq->detCarrierOffsetKHz));
            switch(result)
            {
            case SONYSAT_RESULT_OK:
                break;

            case SONYSAT_RESULT_ERROR_HW_STATE:
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(CarrierOffset)");
                result = finish_ng (pSeq);
                SONYSAT_TRACE_RETURN (result);

            default:
                SONYSAT_TRACE_RETURN (result);
            }

            /* System information */
            result = sonysat_demod_dvbs_s2_monitor_System (pSeq->pCommonParams->pDemod, &(pSeq->detSystem));
            switch(result)
            {
            case SONYSAT_RESULT_OK:
                pSeq->pCommonParams->pDemod->system = pSeq->detSystem;
                break;

            case SONYSAT_RESULT_ERROR_HW_STATE:
                /* Not detect */
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(GetSystem)");
                result = finish_ng (pSeq);
                SONYSAT_TRACE_RETURN (result);

            default:
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(GetSystem-default)");
                SONYSAT_TRACE_RETURN (result);
            }
            /* Success */
            SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("OK");
            result = finish_ok (pSeq);
            SONYSAT_TRACE_RETURN (result);
        } else {
            if (elapsedTime > pSeq->timeout){
                /* Not detected channel */
                SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG( TS lock timeout)");
                result = finish_ng (pSeq);
                SONYSAT_TRACE_RETURN (result);
            } else {
                /* Continue to wait */
            }
        }
        break;

    default:
        SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_TRACE_BT_RESULT("NG(SW state)");
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t finish_ok (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq)
{
    pSeq->isLocked = 1;
    pSeq->isEnable = 0;
    return SONYSAT_RESULT_OK;
}

static sonysat_result_t finish_ng (sonysat_demod_dvbs_s2_blindscan_subseq_bt_t * pSeq)
{
    pSeq->isLocked = 0;
    pSeq->isEnable = 0;
    return SONYSAT_RESULT_OK;
}
