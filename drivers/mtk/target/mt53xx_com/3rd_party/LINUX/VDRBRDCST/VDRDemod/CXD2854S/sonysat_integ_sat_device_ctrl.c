/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_demod_sat_device_ctrl_find1ub_seq.h"
#include "sonysat_demod_sat_device_ctrl_detect_cw_seq.h"
#include "sonysat_integ_sat_device_ctrl.h"
#include "sonysat_integ.h"

/* >>> For MTK */
#ifndef CC_PROJECT_FOR_SONY
extern UINT8 sonysat_DVBS_GetUnicableState (sonysat_integ_t * pInteg);
#endif /* (NOT) CC_PROJECT_FOR_SONY */
/* <<< For MTK */

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t WaitTransmit (sonysat_integ_t * pInteg,
                                   uint32_t interval,
                                   uint32_t timeout);

static sonysat_result_t WaitTransmitSW (sonysat_integ_t * pInteg,
                                     uint32_t timeout1,
                                     uint32_t timeout2,
                                     uint32_t timeout3);

static sonysat_result_t singlecableTunerInitialize (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t singlecableTunerTune (sonysat_tuner_sat_t * pTuner,
                                           uint32_t centerFreqKHz,
                                           sonysat_dtv_system_t dtvSystem,
                                           uint32_t symbolRateKSps);

static sonysat_result_t singlecableTunerSleep (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t singlecableTunerShutdown (sonysat_tuner_sat_t * pTuner);

static sonysat_result_t singlecableTunerAGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                     uint32_t AGCLevel,
                                                     int32_t * pAGCdB);

static sonysat_result_t getCWPower (sonysat_integ_t * pInteg,
                                 sonysat_demod_sat_device_ctrl_detect_cw_seq_t * pSeq,
                                 uint32_t freqKHz,
                                 uint32_t rangeKHz,
                                 int32_t * pPower);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_integ_sat_device_ctrl_SetVoltageTone (sonysat_integ_t * pInteg,
                                                         uint8_t isVoltageHigh,
                                                         uint8_t isContinuousToneOn)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_lnbc_voltage_t voltage;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_SetVoltageTone");

    if ((!pInteg) || (!pInteg->pDemod) ||
        (!pInteg->pLnbc) || (!pInteg->pLnbc->SetVoltage)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pLnbc->tone != SONYSAT_LNBC_TONE_AUTO) ||
        (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Change voltage by LNB controller driver. */
    voltage = isVoltageHigh ? SONYSAT_LNBC_VOLTAGE_HIGH : SONYSAT_LNBC_VOLTAGE_LOW;
    result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, voltage);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Change tone by demodulator */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, isContinuousToneOn);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitToneBurst (sonysat_integ_t * pInteg,
                                                            uint8_t isVoltageHigh,
                                                            sonysat_toneburst_mode_t toneBurstMode,
                                                            uint8_t isContinuousToneOn)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_lnbc_voltage_t voltage;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_TransmitToneBurst");

    if ((!pInteg) || (!pInteg->pDemod) ||
        (!pInteg->pLnbc) || (!pInteg->pLnbc->SetVoltage)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pLnbc->tone != SONYSAT_LNBC_TONE_AUTO) ||
        (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (toneBurstMode == SONYSAT_TONEBURST_MODE_OFF){
        result =  sonysat_integ_sat_device_ctrl_SetVoltageTone (pInteg, isVoltageHigh, isContinuousToneOn);
        SONYSAT_TRACE_RETURN (result);
    }

    /* Stop continuous tone */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Disable DiSEqC command */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcCommand (pInteg->pDemod, 0, NULL, 0, NULL, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set tone burst */
    result = sonysat_demod_sat_device_ctrl_SetToneBurst (pInteg->pDemod, toneBurstMode);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set idle time */
    result = sonysat_demod_sat_device_ctrl_SetIdleTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_TXIDLE_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set repeat to repeat time */
    result = sonysat_demod_sat_device_ctrl_SetR2RTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_R2R_TIME_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Disable DiSEqC reply */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode (pInteg->pDemod, 0, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Voltage control */
    voltage = isVoltageHigh ? SONYSAT_LNBC_VOLTAGE_HIGH : SONYSAT_LNBC_VOLTAGE_LOW;
    result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, voltage);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* === Start transmit === */
    result = sonysat_demod_sat_device_ctrl_StartTransmit (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = WaitTransmit(pInteg, SONYSAT_INTEG_DISEQC_TB_POL, 1000);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Continuous tone */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, isContinuousToneOn);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand (sonysat_integ_t * pInteg,
                                                                uint8_t isVoltageHigh,
                                                                sonysat_toneburst_mode_t toneBurstMode,
                                                                uint8_t isContinuousToneOn,
                                                                sonysat_diseqc_message_t * pCommand1,
                                                                uint8_t repeatCount1,
                                                                sonysat_diseqc_message_t * pCommand2,
                                                                uint8_t repeatCount2)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_lnbc_voltage_t voltage;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_TransmitDiseqcCommand");

    if ((!pInteg) || (!pInteg->pDemod) || (!pInteg->pLnbc) || (!pInteg->pLnbc->SetVoltage) ||
        (!pCommand1) || (repeatCount1 == 0) || (repeatCount1 >= 16) || (repeatCount2 >= 16)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pLnbc->tone != SONYSAT_LNBC_TONE_AUTO) ||
        (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancel flag */
    sonysat_atomic_set (&(pInteg->cancel), 0);

    /* Stop continuous tone */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set DiSEqC command */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcCommand (pInteg->pDemod, 1, pCommand1, repeatCount1, pCommand2, repeatCount2);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set tone burst */
    result = sonysat_demod_sat_device_ctrl_SetToneBurst (pInteg->pDemod, toneBurstMode);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set idle time */
    result = sonysat_demod_sat_device_ctrl_SetIdleTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_TXIDLE_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set repeat to repeat time */
    result = sonysat_demod_sat_device_ctrl_SetR2RTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_R2R_TIME_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Disable DiSEqC reply */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode (pInteg->pDemod, 0, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Voltage control */
    voltage = isVoltageHigh ? SONYSAT_LNBC_VOLTAGE_HIGH : SONYSAT_LNBC_VOLTAGE_LOW;
    result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, voltage);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* === Start transmit === */
    result = sonysat_demod_sat_device_ctrl_StartTransmit (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = WaitTransmit(pInteg,
                          SONYSAT_INTEG_DISEQC_DISEQC_TRANSMIT_POL,
                          (uint32_t)1000 * (repeatCount1 + repeatCount2));
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Continuous tone */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, isContinuousToneOn);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitDiseqcCommandWithReply (sonysat_integ_t * pInteg,
                                                                         uint8_t isVoltageHigh,
                                                                         sonysat_toneburst_mode_t toneBurstMode,
                                                                         uint8_t isContinuousToneOn,
                                                                         sonysat_diseqc_message_t * pCommand,
                                                                         sonysat_diseqc_message_t * pReply)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_lnbc_voltage_t voltage;
    uint8_t enableRXEN = 0;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_TransmitDiseqcCommandWithReply");

    if ((!pInteg) || (!pInteg->pDemod) || (!pInteg->pLnbc) || (!pInteg->pLnbc->SetVoltage) ||
        (!pCommand) || (!pReply)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pLnbc->tone != SONYSAT_LNBC_TONE_AUTO) ||
        (pInteg->pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancel flag */
    sonysat_atomic_set (&(pInteg->cancel), 0);

    /* Stop continuous tone */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set DiSEqC command */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcCommand (pInteg->pDemod, 1, pCommand, 1, NULL, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set tone burst */
    result = sonysat_demod_sat_device_ctrl_SetToneBurst (pInteg->pDemod, toneBurstMode);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set idle time */
    result = sonysat_demod_sat_device_ctrl_SetIdleTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_TXIDLE_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set repeat to repeat time */
    result = sonysat_demod_sat_device_ctrl_SetR2RTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC1_R2R_TIME_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Enable DiSEqC reply */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode (pInteg->pDemod, 1, SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_RTO_TIME_MS);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Voltage control */
    voltage = isVoltageHigh ? SONYSAT_LNBC_VOLTAGE_HIGH : SONYSAT_LNBC_VOLTAGE_LOW;
    result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, voltage);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* === Start transmit === */
    result = sonysat_demod_sat_device_ctrl_StartTransmit (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = sonysat_demod_sat_device_ctrl_GetRXENMode (pInteg->pDemod, &enableRXEN);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (enableRXEN){
        result = WaitTransmit(pInteg, SONYSAT_INTEG_DISEQC_DISEQC_TRANSMIT_POL, 1400);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
    } else {
        result = WaitTransmitSW(pInteg, 500, 600, 1400);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
        /* Wait "Start continuous tone" after "finished to receive reply" without RXEN. */
        SONYSAT_SLEEP (SONYSAT_INTEG_SAT_DEVICE_CTRL_DISEQC2_RXIDLE_MS);
    }

    /* Output continuous tone if required. */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, isContinuousToneOn);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = sonysat_demod_sat_device_ctrl_GetReplyMessage (pInteg->pDemod, pReply);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand (sonysat_integ_t * pInteg,
                                                                     sonysat_diseqc_message_t * pCommand)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_result_t tempResult = SONYSAT_RESULT_OK;
    uint8_t enableTXEN = 0;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand");

    if ((!pInteg) || (!pInteg->pDemod) || (!pInteg->pLnbc) || (!pInteg->pLnbc->SetVoltage) || (!pCommand)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pInteg->pLnbc->tone != SONYSAT_LNBC_TONE_AUTO){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Clear cancel flag */
    sonysat_atomic_set (&(pInteg->cancel), 0);

    /* >>> For MTK */
#ifndef CC_PROJECT_FOR_SONY
    if (pInteg->pDemod->dvbss2ScanMode) {
        while (1)
        {
            /* Check cancel */
            if (sonysat_atomic_read(&pInteg->cancel)){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
            }
            /* Check collision status */
            if (sonysat_DVBS_GetUnicableState (pInteg) == 1) {
                /* Free -> Go to next step. */
                break;
            }
        }
    } else {
        if (sonysat_DVBS_GetUnicableState (pInteg) == 0) {
            /* Busy -> Return. */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
        }
    }
#endif /* (NOT) CC_PROJECT_FOR_SONY */
    /* <<< For MTK */

    result = sonysat_demod_sat_device_ctrl_GetTXENMode (pInteg->pDemod, &enableTXEN);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Stop continuous tone. */
    result = sonysat_demod_sat_device_ctrl_OutputTone (pInteg->pDemod, 0);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Set tone burst (Disable). */
    result = sonysat_demod_sat_device_ctrl_SetToneBurst (pInteg->pDemod, SONYSAT_TONEBURST_MODE_OFF);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Set single cable command. */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcCommand (pInteg->pDemod, 1, pCommand, 1, NULL, 0);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Set idle time */
    result = sonysat_demod_sat_device_ctrl_SetIdleTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_SINGLECABLE_TXIDLE_MS);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Set repeat to repeat time (Fixed to 0ms) */
    result = sonysat_demod_sat_device_ctrl_SetR2RTime (pInteg->pDemod, SONYSAT_INTEG_SAT_DEVICE_CTRL_SINGLECABLE_R2R_TIME_MS);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Set reply mode (Disable). */
    result = sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode (pInteg->pDemod, 0, 0);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    if (enableTXEN == 0){
        /* Change voltage to "High" */
        result = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, SONYSAT_LNBC_VOLTAGE_HIGH);
        if (result != SONYSAT_RESULT_OK){
            goto End;
        }
    }

    /* === Start transmit === */
    result = sonysat_demod_sat_device_ctrl_StartTransmit (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

    /* Wait for finish transmit */
    result = WaitTransmit (pInteg, SONYSAT_INTEG_DISEQC_SINGLECABLE_TRANSMIT_POL, 1000);
    if (result != SONYSAT_RESULT_OK){
        goto End;
    }

End:
    if (enableTXEN == 0){
        /* Change voltage to "Low" */
        tempResult = pInteg->pLnbc->SetVoltage (pInteg->pLnbc, SONYSAT_LNBC_VOLTAGE_LOW);
        if (tempResult != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (tempResult);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_EnableSinglecable (sonysat_integ_t * pInteg,
                                                            sonysat_integ_singlecable_data_t * pSinglecableData,
                                                            sonysat_result_t (*pTransmitByOtherDemod)(sonysat_diseqc_message_t * pMessage))
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_EnableSinglecable");

    if ((!pInteg) || (!pInteg->pDemod) ||
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->Initialize) ||
        (!pSinglecableData)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->isSinglecable){
        /* Current Demod/Tuner is already configured for Single Cable. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
    } else {
        pSinglecableData->tunerSinglecableData.pInteg = pInteg;
        pSinglecableData->tunerSinglecableData.pTunerReal = pInteg->pTunerSat;
        pSinglecableData->tunerSinglecableData.address = SONYSAT_SINGLECABLE_ADDRESS_ALL_DEVICES;
        pSinglecableData->tunerSinglecableData.bank = SONYSAT_SINGLECABLE_BANK_0;
        pSinglecableData->tunerSinglecableData.ubSlot = SONYSAT_SINGLECABLE_UB_1;
        pSinglecableData->tunerSinglecableData.ubSlotFreqKHz = 1068000;
        pSinglecableData->tunerSinglecableData.enableMDUMode = 0;
        pSinglecableData->tunerSinglecableData.PINCode = 0;
        pSinglecableData->tunerSinglecable.Initialize = singlecableTunerInitialize;
        pSinglecableData->tunerSinglecable.Tune = singlecableTunerTune;
        pSinglecableData->tunerSinglecable.Sleep = singlecableTunerSleep;
        pSinglecableData->tunerSinglecable.Shutdown = singlecableTunerShutdown;
        pSinglecableData->tunerSinglecable.AGCLevel2AGCdB = singlecableTunerAGCLevel2AGCdB;
        pSinglecableData->tunerSinglecable.user = &pSinglecableData->tunerSinglecableData;
        pSinglecableData->tunerSinglecableData.pTransmitByOtherDemod = pTransmitByOtherDemod;
        pInteg->pTunerSat = &pSinglecableData->tunerSinglecable;
        result = sonysat_demod_sat_device_ctrl_EnableSinglecable (pInteg->pDemod, 1);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_DisableSinglecable (sonysat_integ_t * pInteg)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_DisableSinglecable");

    if ((!pInteg) || (!pInteg->pDemod) ||
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->Initialize)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->isSinglecable){
        if (pInteg->pTunerSat->user) {
            /* Current Demod/Tuner is already configured for Single Cable. */
            pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)(pInteg->pTunerSat->user);
            pInteg->pTunerSat = pSinglecableTunerData->pTunerReal;
            result = sonysat_demod_sat_device_ctrl_EnableSinglecable (pInteg->pDemod, 0);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
    } else {
        /* Not single cable mode. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams (sonysat_integ_t * pInteg,
                                                                    sonysat_singlecable_address_t address,
                                                                    sonysat_singlecable_bank_t bank,
                                                                    sonysat_singlecable_ub_slot_t ubSlot,
                                                                    uint32_t ubSlotFreqKHz,
                                                                    uint8_t enableMDUMode, 
                                                                    uint8_t PINCode)
{
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_SetSinglecableTunerParams");

    if ((!pInteg) || (!pInteg->pTunerSat) || (!pInteg->pTunerSat->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (!pInteg->pDemod->isSinglecable){
        /* Current Demod/Tuner is not configured for Single Cable. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pInteg->pTunerSat->user;
    pSinglecableTunerData->address = address;
    pSinglecableTunerData->bank = bank;
    pSinglecableTunerData->ubSlot = ubSlot;
    pSinglecableTunerData->ubSlotFreqKHz = ubSlotFreqKHz;
    pSinglecableTunerData->enableMDUMode = enableMDUMode;
    pSinglecableTunerData->PINCode = PINCode;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_Find1UB (sonysat_integ_t * pInteg,
                                                  sonysat_demod_sat_device_ctrl_find1ub_seq_t * pSeq,
                                                  uint32_t minFreqKHz,
                                                  uint32_t maxFreqKHz,
                                                  sonysat_singlecable_ub_slot_t targetUB,
                                                  uint32_t * pFreqKHz)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;
    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_Find1UB");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSeq) || (!pFreqKHz) || 
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->AGCLevel2AGCdB) ||
        (!pInteg->pTunerSat->Tune)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (pInteg->pDemod->isSinglecable){
        /* Current Demod/Tuner is configured for Single Cable but 
           this API is accepted in non-Single Cable (Normal) mode only. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    sonysat_atomic_set (&(pInteg->cancel), 0);

    result = sonysat_demod_sat_device_ctrl_find1ub_seq_Initialize (pSeq, pInteg->pDemod, minFreqKHz, maxFreqKHz, targetUB);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    while(pSeq->isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        if (pSeq->commonParams.waitTime == 0){
            /* Execute one sequence */
            result = sonysat_demod_sat_device_ctrl_find1ub_seq_Sequence (pSeq);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Start stopwatch */
            result = sonysat_stopwatch_start (&stopwatch);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }

            /* Transmitting single cable command is requested */
            if (pSeq->commandReq){
                pSeq->commandReq = 0;

                result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand(pInteg, &pSeq->message);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }

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
                uint32_t symbolRateKSps = pSeq->commonParams.tuneReq.symbolRateKSps;

                /* Clear request flag. */
                pSeq->commonParams.tuneReq.isRequest = 0;

                /* Symbol rate */
                if (symbolRateKSps == 0) {
                    /* Symbol rate setting for power spectrum */
                    symbolRateKSps = pInteg->pTunerSat->symbolRateKSpsForSpectrum;
                }

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
                                                  symbolRateKSps);
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
            result = sonysat_stopwatch_sleep (&stopwatch, 10);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            if(elapsedTime > pSeq->commonParams.waitTime){
                pSeq->commonParams.waitTime = 0;
            }
        }
    }

    *pFreqKHz = pSeq->ubFreqKHz;

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

    result = sonysat_demod_dvbs_s2_Sleep (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_integ_sat_device_ctrl_DetectYesNo (sonysat_integ_t * pInteg,
                                                      sonysat_demod_sat_device_ctrl_detect_cw_seq_t * pSeq,
                                                      uint32_t ubFreqKHz,
                                                      uint32_t noFreqOffsetKHz,
                                                      uint32_t yesMarginFreqKHz,
                                                      uint32_t noMarginFreqKHz,
                                                      sonysat_integ_sat_device_ctrl_detect_yes_no_result_t * pResult)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    int32_t yesPower = 0;
    int32_t noPower = 0;

    SONYSAT_TRACE_ENTER ("sonysat_integ_sat_device_ctrl_DetectYesNo");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSeq) || (!pResult)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->isSinglecable){
        /* Current Demod/Tuner is configured for Single Cable but 
           this API is accepted in non-Single Cable (Normal) mode only. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    sonysat_atomic_set (&(pInteg->cancel), 0);

    result = getCWPower (pInteg, pSeq, ubFreqKHz, yesMarginFreqKHz, &yesPower);
    if(result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    result = getCWPower (pInteg, pSeq, ubFreqKHz + noFreqOffsetKHz, noMarginFreqKHz, &noPower);
    if(result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if(yesPower >= noPower + 1000){
        /* Return YES. */
        *pResult = SONYSAT_INTEG_SAT_DEVICE_CTRL_DETECT_YES_NO_RESULT_YES;
    } else if(noPower >= yesPower + 1000) {
        /* Return NO. */
        *pResult = SONYSAT_INTEG_SAT_DEVICE_CTRL_DETECT_YES_NO_RESULT_NO;
    } else {
        /* Return NOT Detected. */
        *pResult = SONYSAT_INTEG_SAT_DEVICE_CTRL_DETECT_YES_NO_RESULT_NOTDETECT;
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

    result = sonysat_demod_dvbs_s2_Sleep (pInteg->pDemod);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t getCWPower (sonysat_integ_t * pInteg,
                                 sonysat_demod_sat_device_ctrl_detect_cw_seq_t * pSeq,
                                 uint32_t freqKHz,
                                 uint32_t rangeKHz,
                                 int32_t * pPower)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;
    SONYSAT_TRACE_ENTER ("getCWPower");

    if ((!pInteg) || (!pInteg->pDemod) || (!pSeq) || (!pPower) ||
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->AGCLevel2AGCdB)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pInteg->pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pInteg->pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = sonysat_demod_sat_device_ctrl_detect_cw_seq_Initialize (pSeq, pInteg->pDemod, freqKHz, rangeKHz);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    while(pSeq->isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        if (pSeq->commonParams.waitTime == 0){
            /* Execute one sequence */
            result = sonysat_demod_sat_device_ctrl_detect_cw_seq_Sequence (pSeq);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            /* Start stopwatch */
            result = sonysat_stopwatch_start (&stopwatch);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }

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
                uint32_t symbolRateKSps = pSeq->commonParams.tuneReq.symbolRateKSps;

                /* Clear request flag. */
                pSeq->commonParams.tuneReq.isRequest = 0;

                /* Symbol rate */
                if (symbolRateKSps == 0) {
                    /* Symbol rate setting for power spectrum */
                    symbolRateKSps = pInteg->pTunerSat->symbolRateKSpsForSpectrum;
                }

                if ((pInteg->pTunerSat) && (pInteg->pTunerSat->Tune)) {
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
                                                      symbolRateKSps);
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
            }
        } else {
            /* waiting */
            result = sonysat_stopwatch_sleep (&stopwatch, 10);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }

            if(elapsedTime > pSeq->commonParams.waitTime){
                pSeq->commonParams.waitTime = 0;
            }
        }
    }

    *pPower = pSeq->power;

    SONYSAT_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t WaitTransmit (sonysat_integ_t * pInteg,
                                   uint32_t interval,
                                   uint32_t timeout)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t isContinue = 0;
    uint8_t transmitStatus = 0;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;

    SONYSAT_TRACE_ENTER ("WaitTransmit");
    
    if (!pInteg) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_stopwatch_start(&stopwatch);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    isContinue = 1;
    while (isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_sat_device_ctrl_GetTransmitStatus (pInteg->pDemod, &transmitStatus);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (transmitStatus == 0x00){
            /* Success */
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
        } else {
            if(elapsedTime > timeout){
                /* Timeout error */
                isContinue = 0;
            } else {
                result = sonysat_stopwatch_sleep(&stopwatch, interval);
                if (result != SONYSAT_RESULT_OK) {
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
    }
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
}

static sonysat_result_t WaitTransmitSW (sonysat_integ_t * pInteg,
                                     uint32_t timeout1,
                                     uint32_t timeout2,
                                     uint32_t timeout3)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t isContinue = 0;
    uint8_t transmitStatus = 0;
    uint32_t elapsedTime = 0;
    sonysat_stopwatch_t stopwatch;

    SONYSAT_TRACE_ENTER ("WaitTransmitSW");

    if (!pInteg) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_stopwatch_start (&stopwatch);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    isContinue = 1;
    while (isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_sat_device_ctrl_GetTransmitStatus (pInteg->pDemod, &transmitStatus);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (transmitStatus == 0x02){
            /* Go to next step */
            isContinue = 0;
        } else {
            if (elapsedTime > timeout1){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
            }
        }
    }

    isContinue = 1;
    while (isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_sat_device_ctrl_GetTransmitStatus (pInteg->pDemod, &transmitStatus);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (transmitStatus != 0x02){
            /* Go to next step */
            isContinue = 0;
        } else {
            if (elapsedTime > timeout2){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
            }
        }
    }

    result = pInteg->pLnbc->SetTransmitMode (pInteg->pLnbc, SONYSAT_LNBC_TRANSMIT_MODE_RX);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    isContinue = 1;
    while (isContinue){
        /* Check cancellation. */
        result = sonysat_integ_CheckCancellation (pInteg);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_stopwatch_elapsed (&stopwatch, &elapsedTime);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        result = sonysat_demod_sat_device_ctrl_GetTransmitStatus (pInteg->pDemod, &transmitStatus);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }

        if (transmitStatus == 0x00){
            /* Go to next step */
            isContinue = 0;
        } else {
            if (elapsedTime > timeout3){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_TIMEOUT);
            }
        }
    }

    result = pInteg->pLnbc->SetTransmitMode (pInteg->pLnbc, SONYSAT_LNBC_TRANSMIT_MODE_TX);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

/*------------------------------------------------------------------------------
  Static Functions for dummy tuner
------------------------------------------------------------------------------*/
static sonysat_result_t singlecableTunerInitialize (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("singlecableTunerInitialize");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pTuner->user;

    result = pSinglecableTunerData->pTunerReal->Initialize (pSinglecableTunerData->pTunerReal);

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t singlecableTunerTune (sonysat_tuner_sat_t * pTuner,
                                     uint32_t centerFreqKHz,
                                     sonysat_dtv_system_t dtvSystem,
                                     uint32_t symbolRateKSps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;
    sonysat_diseqc_message_t command;
    uint32_t scFreqKHz = 0;
    uint32_t tuFreqKHz = 0;

    SONYSAT_TRACE_ENTER ("singlecableTunerTune");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pTuner->user;

    /* Create command */

    if (pSinglecableTunerData->enableMDUMode) {
        result = sonysat_singlecable_command_ODU_Channel_change_MDU (&command,
                                                                  pSinglecableTunerData->address,
                                                                  pSinglecableTunerData->ubSlot,
                                                                  pSinglecableTunerData->ubSlotFreqKHz,
                                                                  pSinglecableTunerData->bank,
                                                                  centerFreqKHz, 
                                                                  pSinglecableTunerData->PINCode);
    } else {
        result = sonysat_singlecable_command_ODU_Channel_change (&command,
                                                              pSinglecableTunerData->address,
                                                              pSinglecableTunerData->ubSlot,
                                                              pSinglecableTunerData->ubSlotFreqKHz,
                                                              pSinglecableTunerData->bank,
                                                              centerFreqKHz);
    }
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    scFreqKHz = ((uint32_t)(command.data[3] & 0x03) << 8) | (uint32_t)(command.data[4] & 0xFF);
    scFreqKHz = ((scFreqKHz + 350) * 4000) - pSinglecableTunerData->ubSlotFreqKHz;

    if(pSinglecableTunerData->pTransmitByOtherDemod){
        /* Transmit command by user's function */
        result = pSinglecableTunerData->pTransmitByOtherDemod(&command);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
    } else {
        /* Transmit command */
        result = sonysat_integ_sat_device_ctrl_TransmitSinglecableCommand (pSinglecableTunerData->pInteg, &command);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
    }

    /* Calculate tuner tune frequency */
    if (pSinglecableTunerData->pInteg->pDemod->isSinglecableIqInv == 1){
        /* Normal -> Invert(in case of single cable) */
        tuFreqKHz = pSinglecableTunerData->ubSlotFreqKHz - (centerFreqKHz - scFreqKHz);
    } else {
        /* Invert -> Normal(in case of single cable) */
        tuFreqKHz = pSinglecableTunerData->ubSlotFreqKHz + (centerFreqKHz - scFreqKHz);
    }

    /* Tune to UB slot */
    result = pSinglecableTunerData->pTunerReal->Tune (pSinglecableTunerData->pTunerReal, tuFreqKHz, dtvSystem, symbolRateKSps);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Calculate tuner tune frequency */
    if (pSinglecableTunerData->pInteg->pDemod->isSinglecableIqInv == 1){
        /* Normal -> Invert(in case of single cable) */
        pTuner->frequencyKHz = scFreqKHz - (pSinglecableTunerData->pTunerReal->frequencyKHz - pSinglecableTunerData->ubSlotFreqKHz);
    } else {
        /* Invert -> Normal(in case of single cable) */
        pTuner->frequencyKHz = scFreqKHz + (pSinglecableTunerData->pTunerReal->frequencyKHz - pSinglecableTunerData->ubSlotFreqKHz);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t singlecableTunerSleep (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("singlecableTunerSleep");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pTuner->user;

    result = pSinglecableTunerData->pTunerReal->Sleep (pSinglecableTunerData->pTunerReal);

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t singlecableTunerShutdown (sonysat_tuner_sat_t * pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("singlecableTunerShutdown");

    if ((!pTuner) || (!pTuner->user)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pTuner->user;

    result = pSinglecableTunerData->pTunerReal->Shutdown (pSinglecableTunerData->pTunerReal);

    SONYSAT_TRACE_RETURN (result);
}
static sonysat_result_t singlecableTunerAGCLevel2AGCdB (sonysat_tuner_sat_t * pTuner,
                                                     uint32_t AGCLevel,
                                                     int32_t * pAGCdB)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_integ_singlecable_tuner_data_t * pSinglecableTunerData = NULL;

    SONYSAT_TRACE_ENTER ("singlecableTunerAGCLevel2AGCdB");

    if ((!pTuner) || (!pTuner->user) || (!pAGCdB)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)pTuner->user;

    result = pSinglecableTunerData->pTunerReal->AGCLevel2AGCdB (pSinglecableTunerData->pTunerReal, AGCLevel, pAGCdB);

    SONYSAT_TRACE_RETURN (result);
}
