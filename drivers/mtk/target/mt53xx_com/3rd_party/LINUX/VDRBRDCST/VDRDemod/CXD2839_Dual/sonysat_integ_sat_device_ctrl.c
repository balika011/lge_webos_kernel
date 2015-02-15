/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "sonysat_demod_sat_device_ctrl.h"
#include "sonysat_integ_sat_device_ctrl.h"
#include "sonysat_integ.h"

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
        (!pInteg->pTunerSat) || (!pInteg->pTunerSat->user) || (!pInteg->pTunerSat->Initialize)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pInteg->pDemod->isSinglecable){
        /* Current Demod/Tuner is already configured for Single Cable. */
        pSinglecableTunerData = (sonysat_integ_singlecable_tuner_data_t*)(pInteg->pTunerSat->user);
        pInteg->pTunerSat = pSinglecableTunerData->pTunerReal;
        result = sonysat_demod_sat_device_ctrl_EnableSinglecable (pInteg->pDemod, 0);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
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
                                                                    uint32_t ubSlotFreqKHz)
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

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
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

    sonysat_stopwatch_start(&stopwatch);

    isContinue = 1;
    while (isContinue){
        /* Check cancel */
        if (sonysat_atomic_read(&pInteg->cancel)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
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
                sonysat_stopwatch_sleep(&stopwatch, interval);
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

    sonysat_stopwatch_start (&stopwatch);

    isContinue = 1;
    while (isContinue){
        /* Check cancel */
        if (sonysat_atomic_read (&pInteg->cancel)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
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
        /* Check cancel */
        if (sonysat_atomic_read (&pInteg->cancel)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
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
        /* Check cancel */
        if (sonysat_atomic_read (&pInteg->cancel)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_CANCEL);
        }

        result = pInteg->pfStopwatch_elapsed (&stopwatch, &elapsedTime);
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
    result = sonysat_singlecable_command_ODU_Channel_change (&command,
                                                          pSinglecableTunerData->address,
                                                          pSinglecableTunerData->ubSlot,
                                                          pSinglecableTunerData->ubSlotFreqKHz,
                                                          pSinglecableTunerData->bank,
                                                          centerFreqKHz);
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
