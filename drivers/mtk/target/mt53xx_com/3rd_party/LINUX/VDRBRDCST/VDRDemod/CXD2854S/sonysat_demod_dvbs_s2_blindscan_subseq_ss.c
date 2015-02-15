/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_math.h"
#include "sonysat_demod.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_data.h"
#include "sonysat_demod_dvbs_s2_blindscan_utility.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Initialize (sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq,
                                                                 sonysat_demod_dvbs_s2_blindscan_seq_common_t * pCommonParams)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_ss_Initialize");

    if ((!pSeq) || (!pCommonParams)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 0;
    pSeq->state = SS_STATE_INIT;

    pSeq->maxFreqKHz = 0;
    pSeq->minFreqKHz = 0;
    pSeq->stepFreqKHz = 0;
    pSeq->tunerStepFreqKHz = 0;

    pSeq->pCommonParams = pCommonParams;

    result = sonysat_demod_dvbs_s2_blindscan_AllocPower (&(pSeq->pCommonParams->storage), &(pSeq->pPowerList));
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
    
    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Start (sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq,
                                                            uint32_t minFreqKHz,
                                                            uint32_t maxFreqKHz,
                                                            uint32_t stepFreqKHz,
                                                            uint32_t tunerStepFreqKHz,
                                                            uint8_t averageMode,
                                                            uint8_t averageCount)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_ss_Start");

    if (!pSeq){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSeq->isEnable = 1;
    pSeq->state = SS_STATE_START;

    pSeq->minFreqKHz = minFreqKHz;
    pSeq->maxFreqKHz = maxFreqKHz;
    pSeq->stepFreqKHz = stepFreqKHz;
    pSeq->tunerStepFreqKHz = tunerStepFreqKHz;

    pSeq->pCommonParams->waitTime = 0;

    pSeq->averageMode = averageMode;
    pSeq->averageCount = averageCount;
    pSeq->averageCurrent = 0;

    result = sonysat_demod_dvbs_s2_blindscan_ClearPowerList (&(pSeq->pCommonParams->storage), pSeq->pPowerList);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
    pSeq->pPowerListLast = pSeq->pPowerList;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_subseq_ss_Sequence(sonysat_demod_dvbs_s2_blindscan_subseq_ss_t * pSeq)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t ocfr_csk = 0;
    uint8_t csfin = 0;
    uint8_t csrdend = 0;
    int32_t csfreq = 0;
    uint32_t cspow = 0;
    uint32_t elapsedTime = 0;
    int32_t offsetFreq = 0;
    uint32_t ckaFreqKHz = 0;
    sonysat_demod_dvbs_s2_blindscan_power_t * pTempPower = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_subseq_ss_Sequence");

    if(!pSeq) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(pSeq->state)
    {
    case SS_STATE_START:
        if (pSeq->averageMode == 1) {
            switch (pSeq->averageCount) {
            case 0:
                ocfr_csk = 0x00;
                break;
            case 1:
                /* default setting */
                ocfr_csk = 0x01;
                break;
            case 2:
                ocfr_csk = 0x02;
                break;
            case 4:
                /* 1st recommended setting. */
                ocfr_csk = 0x03;
                break;
            case 8:
                /* 2nd recommended setting. */
                ocfr_csk = 0x04;
                break;
            case 16:
                ocfr_csk = 0x05;
                break;
            case 32:
                ocfr_csk = 0x06;
                break;
            case 64:
                ocfr_csk = 0x07;
                break;
            default:
                /* Error */
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
            }
        } else {
            ocfr_csk = 0x01;
        }
        result = sonysat_demod_dvbs_s2_blindscan_PS_INIT (pSeq->pCommonParams->pDemod, ocfr_csk);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        offsetFreq = (int32_t)pSeq->tunerStepFreqKHz / 2;
        ckaFreqKHz = pSeq->pCommonParams->ckahFreqKHz;

        result = sonysat_demod_dvbs_s2_blindscan_PS_SET (pSeq->pCommonParams->pDemod,
                                                      -offsetFreq,
                                                      offsetFreq,
                                                      pSeq->stepFreqKHz,
                                                      ckaFreqKHz);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->currentFreqKHz = (uint32_t)((int32_t)pSeq->minFreqKHz + offsetFreq);
        pSeq->state = SS_STATE_RF_TUNING;
        pSeq->pCommonParams->waitTime = 0;
        break;

    case SS_STATE_RF_TUNING:
        pSeq->pCommonParams->tuneReq.isRequest = 1;
        pSeq->pCommonParams->tuneReq.frequencyKHz = pSeq->currentFreqKHz;
        pSeq->pCommonParams->tuneReq.system = SONYSAT_DTV_SYSTEM_DVBS;
        pSeq->pCommonParams->tuneReq.symbolRateKSps = 0;
        pSeq->pCommonParams->waitTime = 0;
        pSeq->state = SS_STATE_RF_TUNED;
        break;

    case SS_STATE_RF_TUNED:
        result = sonysat_demod_dvbs_s2_monitor_IFAGCOut (pSeq->pCommonParams->pDemod, &(pSeq->agcLevel));
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->pCommonParams->agcInfo.isRequest = 1;
        pSeq->pCommonParams->agcInfo.agcLevel = pSeq->agcLevel;
        pSeq->pCommonParams->waitTime = 0;
        pSeq->state = SS_STATE_AGC_CALCULATED;
        break;

    case SS_STATE_AGC_CALCULATED:
        pSeq->agc_x100dB = pSeq->pCommonParams->agcInfo.agc_x100dB;
        result = sonysat_demod_dvbs_s2_blindscan_PS_START (pSeq->pCommonParams->pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        result = sonysat_stopwatch_start (&pSeq->stopwatch);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->state = SS_STATE_WAIT_CSFIN;
        pSeq->pCommonParams->waitTime = 10;
        break;


    case SS_STATE_WAIT_CSFIN:
        result = sonysat_stopwatch_elapsed (&pSeq->stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        result = sonysat_demod_dvbs_s2_blindscan_GetCSFIN (pSeq->pCommonParams->pDemod, &csfin);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if (csfin){
            pSeq->state = SS_STATE_READ_CS;
            pSeq->pCommonParams->waitTime = 0;
        } else {
            if (elapsedTime > 10000){
                /* Timeout error */
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
            }
        }
        break;

    case SS_STATE_READ_CS:
        result = sonysat_demod_dvbs_s2_blindscan_GetCSRDEND(pSeq->pCommonParams->pDemod, &csrdend);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        if (csrdend){
            if ((pSeq->averageMode == 0) || (pSeq->averageMode == 1)) {
                if ((pSeq->currentFreqKHz + (pSeq->tunerStepFreqKHz / 2)) < pSeq->maxFreqKHz){
                    pSeq->currentFreqKHz += pSeq->tunerStepFreqKHz;
                    pSeq->state = SS_STATE_RF_TUNING;
                    pSeq->pCommonParams->waitTime = 0;
                    /* Go to next loop */
                } else {
                    pSeq->state = SS_STATE_END;
                    pSeq->pCommonParams->waitTime = 0;
                }
            } else {
                /* Error */
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
            }
        } else {
            ckaFreqKHz = pSeq->pCommonParams->ckahFreqKHz;

            result = sonysat_demod_dvbs_s2_blindscan_GetCSINFO (pSeq->pCommonParams->pDemod, &csfreq, &cspow);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Add to power list */
            result = sonysat_demod_dvbs_s2_blindscan_AllocPower (&(pSeq->pCommonParams->storage), &pTempPower);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Power data (frequency) */
            if (csfreq >= 0){
                pTempPower->freqKHz = (uint32_t)((int32_t)(pSeq->pCommonParams->tuneReq.frequencyKHz) + (((csfreq * (int32_t)ckaFreqKHz) + (int32_t)1024) / (int32_t)2048));
            } else {
                pTempPower->freqKHz = (uint32_t)((int32_t)(pSeq->pCommonParams->tuneReq.frequencyKHz) + (((csfreq * (int32_t)ckaFreqKHz) - (int32_t)1024) / (int32_t)2048));
            }

            if (pTempPower->freqKHz > pSeq->maxFreqKHz){
                result = sonysat_demod_dvbs_s2_blindscan_FreePower (&(pSeq->pCommonParams->storage), pTempPower);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            } else {
                /*
                 * Power[dB]     = 10 * log10 (CSPOW/2^14) - gain_db
                 * Power[x100dB] = (1000 * log10 (CSPOW/2^14)) - gain_db_x100
                 *               = (1000 * (log10(CSPOW) - log10(2^14))) - gain_db_x100
                 *               = (10 * (log10(CSPOW) - 421)) - gain_db_x100
                 *
                 * log10() in this driver returns "result x 100".
                 */
                pTempPower->power = (int32_t)((sonysat_math_log10 (cspow) - 421) * 10) - pSeq->pCommonParams->agcInfo.agc_x100dB;

                if (( pSeq->averageMode == 0) ||
                    ( pSeq->averageMode == 1)){

                    /* Add data to list */
                    pSeq->pPowerListLast->pNext = pTempPower;
                    pSeq->pPowerListLast = pSeq->pPowerListLast->pNext;

                } else {
                    /* Error */
                    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
                }
            }

            result = sonysat_demod_dvbs_s2_blindscan_PS_RACK (pSeq->pCommonParams->pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case SS_STATE_END:
        pSeq->isEnable = 0;
        result = sonysat_demod_dvbs_s2_blindscan_PS_FIN (pSeq->pCommonParams->pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        pSeq->pCommonParams->waitTime = 0;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}
