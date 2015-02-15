/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_integ_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"
#include "sonysat_demod_dvbs_s2_blindscan_seq.h"
#include "sonysat_demod_dvbs_s2_tune_srs_seq.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_integ_dvbs_s2_Tune (sonysat_integ_t * pInteg,
                                       sonysat_dvbs_s2_tune_param_t * pTuneParam)
{
    sonysat_stopwatch_t stopwatch;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_dtv_system_t dtvSystem = SONYSAT_DTV_SYSTEM_DVBS2;
    uint32_t elapsedTime = 0;
    uint8_t isContinue = 1;
    uint32_t timeout = 0;
    sonysat_demod_lock_result_t lockStatus = SONYSAT_DEMOD_LOCK_RESULT_NOTDETECT;

    SONYSAT_TRACE_ENTER ("sonysat_integ_dvbs_s2_Tune");

    if ((!pInteg) || (!pInteg->pDemod) || 
        (!pTuneParam) || (pTuneParam->symbolRateKSps == 0)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    sonysat_atomic_set (&(pInteg->cancel), 0);

    result = sonysat_demod_dvbs_s2_Tune (pInteg->pDemod, pTuneParam);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pTuneParam->system == SONYSAT_DTV_SYSTEM_ANY){
        dtvSystem = SONYSAT_DTV_SYSTEM_DVBS;
    } else {
        dtvSystem = pTuneParam->system;
    }

    if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Tune)) {
#ifndef SONYSAT_DISABLE_I2C_REPEATER
        /* Enable the I2C repeater */
        result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
#endif

        result = pInteg->pTunerSat->Tune (pInteg->pTunerSat,
                                          pTuneParam->centerFreqKHz,
                                          dtvSystem,
                                          pTuneParam->symbolRateKSps);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
        /* Disable the I2C repeater */
        result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
#endif
    }

    result = sonysat_demod_TuneEnd(pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = sonysat_stopwatch_start (&stopwatch);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pTuneParam->system == SONYSAT_DTV_SYSTEM_ANY){
        timeout = ((3600000 + (pTuneParam->symbolRateKSps - 1)) / pTuneParam->symbolRateKSps) + 150;
    } else {
        timeout = ((3000000 + (pTuneParam->symbolRateKSps - 1)) / pTuneParam->symbolRateKSps) + 150;
    }

    isContinue = 1;
    while (isContinue) {

        if (sonysat_atomic_read (&(pInteg->cancel))){
            /* Cancel */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_dvbs_s2_CheckTSLock (pInteg->pDemod, &lockStatus);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (lockStatus == SONYSAT_DEMOD_LOCK_RESULT_LOCKED) {
            /* Lock */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
        }

        if (elapsedTime > timeout) {
            isContinue = 0;
        } else {
            result = sonysat_stopwatch_sleep (&stopwatch, SONYSAT_INTEG_DVBS_S2_TUNE_POLLING_INTERVAL);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
    }
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
}

sonysat_result_t sonysat_integ_dvbs_s2_TuneSRS (sonysat_integ_t * pInteg,
                                          sonysat_demod_dvbs_s2_tune_srs_seq_t * pSeq,
                                          sonysat_integ_dvbs_s2_tune_srs_param_t * pParam,
                                          sonysat_dvbs_s2_tune_param_t * pTuneParam)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;
    SONYSAT_TRACE_ENTER ("sonysat_integ_dvbs_s2_TuneSRS");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSeq) || (!pParam) || (!pTuneParam) ||
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->AGCLevel2AGCdB) ||
        (!pInteg->pTunerSat->Tune)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    sonysat_atomic_set (&(pInteg->cancel), 0);

    result = sonysat_demod_dvbs_s2_tune_srs_seq_Initialize (pSeq,
                                                         pInteg->pDemod,
                                                         pParam->centerFreqKHz,
                                                         pParam->freqRange);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    while(pSeq->isContinue){

        if (sonysat_atomic_read (&(pInteg->cancel))){
            /* Cancel */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        if (pSeq->commonParams.waitTime == 0){
            /* Execute one sequence */
            result = sonysat_demod_dvbs_s2_tune_srs_seq_Sequence (pSeq);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Start stopwatch */
            sonysat_stopwatch_start (&stopwatch);

            /* AGC calculate is requested */
            if (pSeq->commonParams.agcInfo.isRequest){

                /* Clear request flag. */
                pSeq->commonParams.agcInfo.isRequest = 0;

                result = pInteg->pTunerSat->AGCLevel2AGCdB (pInteg->pTunerSat, 
                                                            pSeq->commonParams.agcInfo.agcLevel,
                                                            &(pSeq->commonParams.agcInfo.agc_x100dB));
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }

            /* Tune is requested */
            if (pSeq->commonParams.tuneReq.isRequest){

                /* Clear request flag. */
                pSeq->commonParams.tuneReq.isRequest = 0;

#ifndef SONYSAT_DISABLE_I2C_REPEATER
                /* Enable the I2C repeater */
                result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
#endif
                /* RF Tune */
                result = pInteg->pTunerSat->Tune (pInteg->pTunerSat, 
                                                  pSeq->commonParams.tuneReq.frequencyKHz,
                                                  pSeq->commonParams.tuneReq.system,
                                                  pSeq->commonParams.tuneReq.symbolRateKSps);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                /* Set actual frequency to the driver. */
                pSeq->commonParams.tuneReq.frequencyKHz = pInteg->pTunerSat->frequencyKHz;
#ifndef SONYSAT_DISABLE_I2C_REPEATER
                /* Disable the I2C repeater */
                result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
#endif
            }
        } else {
            /* waiting */
            result = sonysat_stopwatch_sleep (&stopwatch, SONYSAT_INTEG_DVBS_S2_TUNE_SRS_POLLING_INTERVAL);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            if(elapsedTime > pSeq->commonParams.waitTime){
                pSeq->commonParams.waitTime = 0;
            }
        }
    }

    if (pSeq->isDetect){
        /* Tune successful. */
        pTuneParam->centerFreqKHz = pSeq->tuneParam.centerFreqKHz;
        pTuneParam->symbolRateKSps = pSeq->tuneParam.symbolRateKSps;
        pTuneParam->system = pSeq->tuneParam.system;

        /* TS output enable */
        result = sonysat_demod_dvbs_s2_blindscan_SetTSOut (pInteg->pDemod, 1);
    } else {
        result = SONYSAT_RESULT_ERROR_UNLOCK;
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_dvbs_s2_BlindScan (sonysat_integ_t * pInteg,
                                            sonysat_demod_dvbs_s2_blindscan_seq_t * pSeq,
                                            sonysat_integ_dvbs_s2_blindscan_param_t * pParam,
                                            sonysat_integ_dvbs_s2_blindscan_callback_t callback)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;
    sonysat_integ_dvbs_s2_blindscan_result_t blindscanResult;

    SONYSAT_TRACE_ENTER("sonysat_integ_dvbs_s2_BlindScan");


    if ((!pInteg) || (!pInteg->pDemod) || (!pSeq) || (!pParam) || (!callback) ||
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->AGCLevel2AGCdB) ||
        (!pInteg->pTunerSat->Tune) || (!pInteg->pTunerSat->Sleep)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    sonysat_atomic_set (&(pInteg->cancel), 0);

    result = sonysat_demod_dvbs_s2_blindscan_seq_Initialize (pSeq, 
                                                          pInteg->pDemod, 
                                                          pParam->minFreqKHz,
                                                          pParam->maxFreqKHz,
                                                          pParam->minSymbolRateKSps,
                                                          pParam->maxSymbolRateKSps);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    while(pSeq->isContinue){

        if (sonysat_atomic_read (&(pInteg->cancel))){
            /* Cancel */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        if (pSeq->commonParams.waitTime == 0){
            /* Execute one sequence */
            result = sonysat_demod_dvbs_s2_blindscan_seq_Sequence (pSeq);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Start stopwatch */
            sonysat_stopwatch_start (&stopwatch);

            if (pSeq->commonParams.agcInfo.isRequest){

                /* Clear request flag. */
                pSeq->commonParams.agcInfo.isRequest = 0;

                result = pInteg->pTunerSat->AGCLevel2AGCdB (pInteg->pTunerSat, 
                                                            pSeq->commonParams.agcInfo.agcLevel,
                                                            &(pSeq->commonParams.agcInfo.agc_x100dB));
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }

            if (pSeq->commonParams.tuneReq.isRequest){

                /* Clear request flag. */
                pSeq->commonParams.tuneReq.isRequest = 0;

#ifndef SONYSAT_DISABLE_I2C_REPEATER
                /* Enable the I2C repeater */
                result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
#endif
                /* RF Tune */
                result = pInteg->pTunerSat->Tune (pInteg->pTunerSat, 
                                                  pSeq->commonParams.tuneReq.frequencyKHz,
                                                  pSeq->commonParams.tuneReq.system,
                                                  pSeq->commonParams.tuneReq.symbolRateKSps);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                pSeq->commonParams.tuneReq.frequencyKHz = pInteg->pTunerSat->frequencyKHz;
#ifndef SONYSAT_DISABLE_I2C_REPEATER
                /* Disable the I2C repeater */
                result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
#endif
            }

            if (pSeq->commonParams.detInfo.isDetect){

                /* Clear detect flag */
                pSeq->commonParams.detInfo.isDetect = 0;

                /* Prepare callback information.(Detected channel) */
                blindscanResult.eventId = SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_DETECT;
                blindscanResult.tuneParam.system = pSeq->commonParams.detInfo.system;
                blindscanResult.tuneParam.centerFreqKHz = pSeq->commonParams.detInfo.centerFreqKHz;
                blindscanResult.tuneParam.symbolRateKSps = pSeq->commonParams.detInfo.symbolRateKSps;

                /* TS output enable */
                result = sonysat_demod_dvbs_s2_blindscan_SetTSOut (pInteg->pDemod, 1);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }

                /* Callback */
                callback(pInteg, &blindscanResult);

                /* TS output disable */
                result = sonysat_demod_dvbs_s2_blindscan_SetTSOut (pInteg->pDemod, 0);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
            
            {
                /* Progress calculation */
                uint8_t progress = 0;
                uint8_t rangeMin = pSeq->commonParams.progressInfo.majorMinProgress;
                uint8_t rangeMax = pSeq->commonParams.progressInfo.majorMaxProgress;
                uint8_t minorProgress = pSeq->commonParams.progressInfo.minorProgress;
                progress = rangeMin + (((rangeMax - rangeMin) * minorProgress) / 100);

                if (pSeq->commonParams.progressInfo.progress < progress){
                    pSeq->commonParams.progressInfo.progress = progress;
                    /* Prepare callback information.(Progress) */
                    blindscanResult.eventId = SONYSAT_INTEG_DVBS_S2_BLINDSCAN_EVENT_PROGRESS;
                    blindscanResult.progress = progress;
                    /* Callback */
                    callback(pInteg, &blindscanResult);
                }
            }

        } else {
            /* waiting */
            result = sonysat_stopwatch_sleep (&stopwatch, SONYSAT_INTEG_DVBS_S2_BLINDSCAN_POLLING_INTERVAL);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            if(elapsedTime > pSeq->commonParams.waitTime){
                pSeq->commonParams.waitTime = 0;
            }
        }
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x01);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    result = pInteg->pTunerSat->Sleep(pInteg->pTunerSat);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

#ifndef SONYSAT_DISABLE_I2C_REPEATER
    result = sonysat_demod_I2cRepeaterEnable (pInteg->pDemod, 0x00);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
#endif

    result = sonysat_demod_dvbs_s2_Sleep (pSeq->commonParams.pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN(result);
}

sonysat_result_t sonysat_integ_dvbs_s2_monitor_RFLevel (sonysat_integ_t * pInteg,
                                                  int32_t * pRFLeveldB)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t ifagc = 0;
    int32_t gain = 0;
    SONYSAT_TRACE_ENTER("sonysat_integ_dvbs_s2_monitor_RFLevel");

    if ((!pInteg) || (!pInteg->pDemod) || (!pInteg->pTunerSat) ||
        (!pInteg->pTunerSat->AGCLevel2AGCdB) || (!pRFLeveldB)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = sonysat_demod_dvbs_s2_monitor_IFAGCOut (pInteg->pDemod, &ifagc);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = pInteg->pTunerSat->AGCLevel2AGCdB (pInteg->pTunerSat, ifagc, &gain);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* RF_Level = Input_Ref_Level - RF_Gain 
     * For this demodulator
     *  - Input_Ref_Level = 0dB
     *  - Gain is in units dB*100
     * Therefore:
     * RF_Level (dB*1000) =  -10 * RF_Gain
     */
    *pRFLeveldB = gain * (-10);

    SONYSAT_TRACE_RETURN(result);
}
