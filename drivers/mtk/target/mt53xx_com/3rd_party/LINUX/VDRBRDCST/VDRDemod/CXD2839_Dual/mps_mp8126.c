/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "mps_mp8126.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t mps_mp8126_Initialize (sonysat_lnbc_t * pLnbc);

static sonysat_result_t mps_mp8126_SetConfig (sonysat_lnbc_t * pLnbc,
                                           sonysat_lnbc_config_id_t configId,
                                           int32_t value);

static sonysat_result_t mps_mp8126_SetVoltage (sonysat_lnbc_t * pLnbc,
                                            sonysat_lnbc_voltage_t voltage);

static sonysat_result_t mps_mp8126_SetTone (sonysat_lnbc_t * pLnbc,
                                         sonysat_lnbc_tone_t tone);

static sonysat_result_t mps_mp8126_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                 sonysat_lnbc_transmit_mode_t mode);

static sonysat_result_t mps_mp8126_Sleep (sonysat_lnbc_t * pLnbc);

static sonysat_result_t mps_mp8126_WakeUp (sonysat_lnbc_t * pLnbc);

static sonysat_result_t mps_mp8116_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                               uint8_t * pOLF);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t mps_mp8126_Create (sonysat_lnbc_t * pLnbc,
                                 sonysat_demod_t * pDemod,
                                 uint8_t voltagePinId,
                                 uint8_t linedropPinId,
                                 uint8_t enablePinId,
                                 mps_mp8126_t * pMP8126)
{
    SONYSAT_TRACE_ENTER ("mps_mp8126_Create");

    if ((!pLnbc) || (!pDemod) || (!pMP8126)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->i2cAddress = 0;
    pLnbc->pI2c = NULL;

    pLnbc->Initialize = mps_mp8126_Initialize;
    pLnbc->SetConfig = mps_mp8126_SetConfig;
    pLnbc->SetVoltage = mps_mp8126_SetVoltage;
    pLnbc->SetTone = mps_mp8126_SetTone;
    pLnbc->SetTransmitMode = mps_mp8126_SetTransmitMode;
    pLnbc->Sleep = mps_mp8126_Sleep;
    pLnbc->WakeUp = mps_mp8126_WakeUp;
    pLnbc->MonitorOverload = mps_mp8116_MonitorOLF;
    pLnbc->user = (void *)pMP8126;

    pLnbc->isInternalTone = 0;
    pLnbc->lowVoltage = MPS_MP8126_CONFIG_VOLTAGE_LOW_13V; /* Default value */
    pLnbc->highVoltage = MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V; /* Default value */

    pLnbc->state = SONYSAT_LNBC_STATE_UNKNOWN;
    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    pMP8126->pDemod = pDemod;
    pMP8126->voltagePinId = voltagePinId;
    pMP8126->linedropPinId = linedropPinId;
    pMP8126->enablePinId = enablePinId;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t mps_mp8126_Initialize (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    mps_mp8126_t * pMP8126;

    SONYSAT_TRACE_ENTER ("mps_mp8126_Initialize");

    if ((!pLnbc) || (!pLnbc->user)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pMP8126 = (mps_mp8126_t *)(pLnbc->user);

    /* Configure all three GPIO Pins as output pins. */
    result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->enablePinId, 1, SONYSAT_DEMOD_GPIO_MODE_OUTPUT);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
    result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->linedropPinId, 1, SONYSAT_DEMOD_GPIO_MODE_OUTPUT);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }
    result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->voltagePinId, 1, SONYSAT_DEMOD_GPIO_MODE_OUTPUT);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set LineDrop Pin to low state. */
    result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->linedropPinId, 0);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set Voltage Pin to low state. */
    result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->voltagePinId, 0);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    pLnbc->isInternalTone = 0;
    pLnbc->lowVoltage = MPS_MP8126_CONFIG_VOLTAGE_LOW_13V; /* Default value */
    pLnbc->highVoltage = MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V; /* Default value */

    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    /* Activate/Enable the LNBC. */
    result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->enablePinId, 1);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t mps_mp8126_SetConfig (sonysat_lnbc_t * pLnbc,
                                           sonysat_lnbc_config_id_t configId,
                                           int32_t value)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    
    SONYSAT_TRACE_ENTER ("mps_mp8126_SetConfig");

    if ((!pLnbc) || (!pLnbc->user)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) &&
        (pLnbc->state != SONYSAT_LNBC_STATE_SLEEP)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(configId)
    {
    case SONYSAT_LNBC_CONFIG_ID_TONE_INTERNAL:
        if (value == 0) {
            pLnbc->isInternalTone = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE) {
                result = mps_mp8126_SetTone (pLnbc, pLnbc->tone);
                if (result != SONYSAT_RESULT_OK) {
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        } else if (value == 1) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        } else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE:
        if ((value != MPS_MP8126_CONFIG_VOLTAGE_LOW_13V) &&
            (value != MPS_MP8126_CONFIG_VOLTAGE_LOW_14V)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->lowVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE) {
                result = mps_mp8126_SetVoltage (pLnbc, pLnbc->voltage);
                if (result != SONYSAT_RESULT_OK) {
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE:
        if ((value != MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V) && 
            (value != MPS_MP8126_CONFIG_VOLTAGE_HIGH_19V)) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->highVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE) {
                result = mps_mp8126_SetVoltage (pLnbc, pLnbc->voltage);
                if (result != SONYSAT_RESULT_OK) {
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t mps_mp8126_SetVoltage (sonysat_lnbc_t * pLnbc,
                                            sonysat_lnbc_voltage_t voltage)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    mps_mp8126_t * pMP8126;

    SONYSAT_TRACE_ENTER ("mps_mp8126_SetVoltage");

    if ((!pLnbc) || (!pLnbc->user)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pMP8126 = (mps_mp8126_t *)(pLnbc->user);

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(voltage)
    {
    case SONYSAT_LNBC_VOLTAGE_LOW:
        if (pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO) {
            /* Configure voltage GPIO pin as an output. */
            result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->voltagePinId, 1, SONYSAT_DEMOD_GPIO_MODE_OUTPUT);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }

        if (pLnbc->lowVoltage == MPS_MP8126_CONFIG_VOLTAGE_LOW_13V) {
            result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->linedropPinId, 0);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        } else if (pLnbc->lowVoltage == MPS_MP8126_CONFIG_VOLTAGE_LOW_14V) {
            result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->linedropPinId, 1);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }

        result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->voltagePinId, 0);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_LNBC_VOLTAGE_HIGH:
        if (pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO) {
            /* Configure voltage GPIO pin as an output. */
            result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->voltagePinId, 1, SONYSAT_DEMOD_GPIO_MODE_OUTPUT);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        }

        if (pLnbc->highVoltage == MPS_MP8126_CONFIG_VOLTAGE_HIGH_18V) {
            result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->linedropPinId, 0);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        } else if (pLnbc->highVoltage == MPS_MP8126_CONFIG_VOLTAGE_HIGH_19V) {
            result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->linedropPinId, 1);
            if (result != SONYSAT_RESULT_OK) {
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }

        result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->voltagePinId, 1);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_LNBC_VOLTAGE_AUTO:
        /* Configure voltage GPIO pin as DISEQC TX. */
        result = sonysat_demod_GPIOSetConfig (pMP8126->pDemod, pMP8126->voltagePinId, 1, SONYSAT_DEMOD_GPIO_MODE_DISEQC_TX_EN);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->voltage = voltage;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t mps_mp8126_SetTone (sonysat_lnbc_t * pLnbc,
                                         sonysat_lnbc_tone_t tone)
{
    SONYSAT_TRACE_ENTER ("mps_mp8126_SetTone");

    if (!pLnbc) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(tone)
    {
    case SONYSAT_LNBC_TONE_OFF:
    case SONYSAT_LNBC_TONE_ON:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        break;

    case SONYSAT_LNBC_TONE_AUTO:
        /* Do nothing */
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->tone = tone;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t mps_mp8126_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                 sonysat_lnbc_transmit_mode_t mode)
{
    SONYSAT_TRACE_ENTER ("mps_mp8126_SetTransmitMode");

    if (!pLnbc) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(mode)
    {
    case SONYSAT_LNBC_TRANSMIT_MODE_TX:
        /* Do nothing */
        break;

    case SONYSAT_LNBC_TRANSMIT_MODE_RX:
    case SONYSAT_LNBC_TRANSMIT_MODE_AUTO:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->transmitMode = mode;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t mps_mp8126_Sleep (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    mps_mp8126_t * pMP8126;

    SONYSAT_TRACE_ENTER ("mps_mp8126_Sleep");

    if ((!pLnbc) || (!pLnbc->user)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pMP8126 = (mps_mp8126_t *)(pLnbc->user);

    switch(pLnbc->state)
    {
    case SONYSAT_LNBC_STATE_ACTIVE:
        result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->enablePinId, 0);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_LNBC_STATE_SLEEP:
        /* Do nothing */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);

    case SONYSAT_LNBC_STATE_UNKNOWN:
    default:
        /* Error */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t mps_mp8126_WakeUp (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    mps_mp8126_t * pMP8126;

    SONYSAT_TRACE_ENTER ("mps_mp8126_WakeUp");

    if ((!pLnbc) || (!pLnbc->user)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pMP8126 = (mps_mp8126_t *)(pLnbc->user);

    switch(pLnbc->state)
    {
    case SONYSAT_LNBC_STATE_ACTIVE:
        /* Do nothing */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);

    case SONYSAT_LNBC_STATE_SLEEP:
        result = sonysat_demod_GPIOWrite (pMP8126->pDemod, pMP8126->enablePinId, 1);
        if (result != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (result);
        }
        break;

    case SONYSAT_LNBC_STATE_UNKNOWN:
    default:
        /* Error */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (result);
}

static sonysat_result_t mps_mp8116_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                               uint8_t * pOLF)
{
    SONYSAT_TRACE_ENTER ("mps_mp8116_MonitorOLF");
    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
}
