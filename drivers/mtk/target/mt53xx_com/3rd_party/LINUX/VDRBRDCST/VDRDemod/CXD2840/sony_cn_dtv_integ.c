/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2013/05/30
 Revision: 1.1.6.0

------------------------------------------------------------------------------*/

#include "sony_cn_dtv_integ.h"

#include "sony_cn_dtv_demod_monitorDtmb.h"
#include "sony_cn_dtv_demod_monitorDvbc.h"


/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_CN_DTV_INTEG_TUNE_DTMB_TIMEOUT            10000  /* 10sec */
#define SONY_CN_DTV_INTEG_TUNE_DVBC_TIMEOUT            1000   /* 1sec */
#define SONY_CN_DTV_INTEG_TUNE_LOCK_CHECK_PERIOD       10     /* 10msec */


/*------------------------------------------------------------------------------
 Prototypes of static functions
------------------------------------------------------------------------------*/
static sony_result_t Tune(sony_cn_dtv_integ_t* pInteg);
static sony_result_t PostSeqDTMB(sony_cn_dtv_integ_t* pInteg, uint8_t* pIsCondition1);

/* For MTK */
extern sony_result_t sony_integ_MTK_CheckBreak (void);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Initialize the demodulator and tuner.

  pInteg   : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Initialize(sony_cn_dtv_integ_t* pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER("sony_cn_dtv_integ_Initialize");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Initialize Demod */
    result = sony_cn_dtv_demod_Initialize(pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->Initialize)) {
#ifdef SONY_ENABLE_I2C_REPEATER
        /* Enable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 1);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#endif
        /*Initialize Tuner*/
        result = pInteg->pTuner->Initialize(pInteg->pTuner);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#ifdef SONY_ENABLE_I2C_REPEATER
        /* Disable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 0);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#endif
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Put the demodulator and tuner devices into a low power state.

  pInteg   : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Sleep(sony_cn_dtv_integ_t* pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER("sony_cn_dtv_integ_Sleep");

    if ((!pInteg) || (!pInteg->pDemod)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }
    
    if ((pInteg->pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J) &&
        (pInteg->pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) && 
        (pInteg->pDemod->state != SONY_CN_DTV_DEMOD_STATE_INIT)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pTuner) && (pInteg->pTuner->Sleep)) {
#ifdef SONY_ENABLE_I2C_REPEATER
        /* Enable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 1);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#endif
        /*Sleep Tuner*/
        result = pInteg->pTuner->Sleep(pInteg->pTuner);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#ifdef SONY_ENABLE_I2C_REPEATER
        /* Disable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 0);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN(result);
        }
#endif
    }

    result = sony_cn_dtv_demod_Sleep(pInteg->pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tune sequence

  pInteg   : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Tune(sony_cn_dtv_integ_t* pInteg)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_integ_Tune");

    /* Null check */
    if(!pInteg || !pInteg->pDemod || !pInteg->pTuner){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Clear cancel flag */
    pInteg->isCancel = 0;

#if 0 /* For MTK */
    /* Off scan mode */
    result = sony_cn_dtv_demod_SetConfig(pInteg->pDemod, SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
#endif

    result = Tune(pInteg);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Scan sequence

  pInteg       : Address of integration layer instance.
  freqEnd      : End frequency of scan sequence.
  freqInterval : Frequency interval.
  pCallback    : Callback function pointer.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Scan(sony_cn_dtv_integ_t* pInteg,
                                     uint32_t freqEnd,
                                     uint32_t freqInterval,
                                     void (*pCallback)(sony_cn_dtv_integ_t*))
{
    sony_result_t result = SONY_RESULT_OK;
    sony_result_t scan_result = SONY_RESULT_OK;
    uint32_t freqCurrent = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_integ_Scan");

    /* Null check */
    if(!pInteg || !pInteg->pDemod || !pInteg->pTuner || !pCallback){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Check frequency values */
    if((pInteg->frequency > freqEnd) || (freqInterval == 0)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Clear cancel flag */
    pInteg->isCancel = 0;

    /* Force preset off (for DTMB) */
    pInteg->presetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_NONE;

    /* Set scan mode */
    result = sony_cn_dtv_demod_SetConfig(pInteg->pDemod, SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set first frequency */
    freqCurrent = pInteg->frequency;

    /* Scan loop ---*/
    while(freqCurrent <= freqEnd){
        /* Check cancel flag */
        if(pInteg->isCancel){
            scan_result = SONY_RESULT_ERROR_CANCEL;
            goto EXIT;
        }

        /* Set tune frequency */
        pInteg->frequency = freqCurrent;

        result = Tune(pInteg);

        switch(result)
        {
        case SONY_RESULT_OK: /* LOCK */
            pInteg->lockInfo = SONY_CN_DTV_INTEG_LOCK_ID_LOCK;
            (*pCallback)(pInteg);
            break;
        case SONY_RESULT_ERROR_TIMEOUT: /* UNLOCK(Timeout) */
        case SONY_RESULT_ERROR_UNLOCK:  /* UNLOCK */
            pInteg->lockInfo = SONY_CN_DTV_INTEG_LOCK_ID_UNLOCK;
            (*pCallback)(pInteg);
            break;
        default:
            /* error case */
            scan_result = result;
            goto EXIT;
        }

        /* Next frequency */
        freqCurrent += freqInterval;
    }

    /* Scan process succeeded */
    scan_result = SONY_RESULT_OK;

EXIT:
    /* Scan terminal process */
    /* Off scan mode */
    result = sony_cn_dtv_demod_SetConfig(pInteg->pDemod, SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(scan_result);
}

/*--------------------------------------------------------------------
  Cancel Sequence

  pInteg : Address of integration layer instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_integ_Cancel(sony_cn_dtv_integ_t* pInteg)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_integ_Cancel");

    /* Null check */
    if(!pInteg){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Set cancel flag */
    pInteg->isCancel = 1;

    SONY_TRACE_RETURN(result);
}


/*------------------------------------------------------------------------------
  static functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Tuning Sequence

  pInteg : Address of integration layer instance.
--------------------------------------------------------------------*/
static sony_result_t Tune(sony_cn_dtv_integ_t* pInteg)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t index = 0;
    uint32_t timeout = 0;
    uint8_t possibility = 0;
    uint8_t isC1Assumed = 0;
    uint8_t c1EarlyDetectDone = 0;
    uint8_t isCondition1 = 0;
    sony_cn_dtv_demod_preset_mode_t currentPresetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_NONE;
    sony_cn_dtv_demod_lock_status_t lockStatus = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNKNOWN;

    SONY_TRACE_ENTER("Tune");

    /* Null check */
    if(!pInteg){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Preset (for DTMB) */
    if(pInteg->system == SONY_CN_DTV_SYSTEM_DTMB){
        result = sony_cn_dtv_demod_SetPreset(pInteg->pDemod, &pInteg->presetInfo, pInteg->presetMode);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    currentPresetMode = pInteg->presetMode;

    /* Tune demodulator */
    result = sony_cn_dtv_demod_Tune(pInteg->pDemod, pInteg->system);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((pInteg->pTuner) && (pInteg->pTuner->Tune)) {
#ifdef SONY_ENABLE_I2C_REPEATER
        /* Enable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 1);
        if (result != SONY_RESULT_OK) { SONY_TRACE_RETURN(result); }
#endif

        /* Tune RF */
        result = pInteg->pTuner->Tune(pInteg->pTuner, pInteg->frequency, pInteg->system, pInteg->autoscan);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

#ifdef SONY_ENABLE_I2C_REPEATER
        /* Disable the I2C repeater */
        result = sony_cn_dtv_demod_I2cRepeaterEnable(pInteg->pDemod, 0);
        if (result != SONY_RESULT_OK) { SONY_TRACE_RETURN(result); }
#endif
    }
    /* PostTune demodulator */
    result = sony_cn_dtv_demod_PostTune(pInteg->pDemod);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Lock check sequence ---*/
    if (pInteg->pDemod->system == SONY_CN_DTV_SYSTEM_DTMB) {
        timeout = SONY_CN_DTV_INTEG_TUNE_DTMB_TIMEOUT/SONY_CN_DTV_INTEG_TUNE_LOCK_CHECK_PERIOD;
    } else {
        timeout = SONY_CN_DTV_INTEG_TUNE_DVBC_TIMEOUT/SONY_CN_DTV_INTEG_TUNE_LOCK_CHECK_PERIOD;
    }

    for(index = 0; index < timeout; index++){
        /* Check cancel flag */
        if(pInteg->isCancel){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_CANCEL);
        }

        /* For MTK */
        result = sony_integ_MTK_CheckBreak();
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        if(pInteg->pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
            if((currentPresetMode == SONY_CN_DTV_DEMOD_PRESET_MODE_NONE) && (!c1EarlyDetectDone)){
                /* 
                   Carrier mode early detection.
                   In SONY_CN_DTV_DEMOD_PRESET_MODE_NONE case, the driver tries C3780 first.
                   If C1 is detected, change the setting to try C1.
                */
                result = sony_cn_dtv_demod_CheckC1EarlyDetectionPossibility(pInteg->pDemod, &possibility);
                if (result != SONY_RESULT_OK) { SONY_TRACE_RETURN(result);}

                switch (possibility) {
                case SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK:
                    SONY_TRACE_RETURN(SONY_RESULT_ERROR_UNLOCK);
                case SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK:
                    result = sony_cn_dtv_demod_C1EarlyDetect(pInteg->pDemod, &isC1Assumed);
                    if (result != SONY_RESULT_OK) {SONY_TRACE_RETURN(result);}
                    if (isC1Assumed) {
                        /* C1 has been detected. Restart wait demod lock sequence. */
                        index = 0;
                    }
                    c1EarlyDetectDone = 1;
                    break;
                case SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT:
                default:
                    SONY_SLEEP(SONY_CN_DTV_INTEG_TUNE_LOCK_CHECK_PERIOD);
                    break;
                }

                if (!c1EarlyDetectDone) {
                    continue;
                }
            }

            result = sony_cn_dtv_demod_OptimizeSet(pInteg->pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }

        /* Get lock status */
        result = sony_cn_dtv_demod_CheckTSLock(pInteg->pDemod, &lockStatus);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* Check lock status */
        switch(lockStatus)
        {
        case SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK:
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_UNLOCK);
        case SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK:
            if((pInteg->pDemod->system == SONY_CN_DTV_SYSTEM_DTMB)
                && (currentPresetMode != SONY_CN_DTV_DEMOD_PRESET_MODE_FULL)) {

                result = PostSeqDTMB(pInteg, &isCondition1);
                if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

                if(currentPresetMode == SONY_CN_DTV_DEMOD_PRESET_MODE_FULL_USECMODE){
                    /* Check that preset information from user was Condition1 case or not. */
                    uint8_t isPresetInfoCondition1 = 0;
                    currentPresetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_FULL;

                    result = sony_cn_dtv_demod_CheckCondition1(&pInteg->presetInfo, &isPresetInfoCondition1);
                    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

                    if(isCondition1 != isPresetInfoCondition1){
                        /* This means that preset information by user was not correct and need to go to wait lock sequence again. */
                        result = sony_cn_dtv_demod_SoftReset(pInteg->pDemod);
                        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

                        index = 0;
                        continue;
                    }
                }else{
                    /* SONY_CN_DTV_DEMOD_PRESET_MODE_NONE or SONY_CN_DTV_DEMOD_PRESET_MODE_CMODE */
                    currentPresetMode = SONY_CN_DTV_DEMOD_PRESET_MODE_FULL;

                    if(isCondition1){
                        /* In the case of Condition1, go to wait lock sequence once again. */
                        result = sony_cn_dtv_demod_SoftReset(pInteg->pDemod);
                        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

                        index = 0;
                        continue;
                    }
                }
            }

            /* TS output enable */
            result = sony_cn_dtv_demod_TsOutputEnable(pInteg->pDemod, 1);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            SONY_TRACE_RETURN(SONY_RESULT_OK);

        case SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT:
        default:
            /* Continue waiting... */
            SONY_SLEEP(SONY_CN_DTV_INTEG_TUNE_LOCK_CHECK_PERIOD);
            break;
        }
    }

    SONY_TRACE_RETURN(SONY_RESULT_ERROR_TIMEOUT);
}

/*--------------------------------------------------------------------
  Sequence for DTMB after TS Lock

  pInteg       : Address of integration layer instance.
--------------------------------------------------------------------*/
static sony_result_t PostSeqDTMB(sony_cn_dtv_integ_t* pInteg, uint8_t* pIsCondition1)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_cn_dtv_demod_preset_info_t presetInfo;
    SONY_TRACE_ENTER("PostSeqDTMB");

    /* Null check */
    if(!pInteg){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    result = sony_cn_dtv_demod_MonitorDtmb_Preset(pInteg->pDemod, &presetInfo);
    if (result != SONY_RESULT_OK) { SONY_TRACE_RETURN(result); }
    
    result = sony_cn_dtv_demod_SetPreset(pInteg->pDemod, &presetInfo, SONY_CN_DTV_DEMOD_PRESET_MODE_FULL);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = sony_cn_dtv_demod_CheckCondition1(&presetInfo, pIsCondition1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

