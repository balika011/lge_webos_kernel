/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "st_lnbh29.h"

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t st_lnbh29_Initialize (sonysat_lnbc_t * pLnbc);

static sonysat_result_t st_lnbh29_SetConfig (sonysat_lnbc_t * pLnbc,
                                          sonysat_lnbc_config_id_t configId,
                                          int32_t value);

static sonysat_result_t st_lnbh29_SetVoltage (sonysat_lnbc_t * pLnbc,
                                           sonysat_lnbc_voltage_t voltage);

static sonysat_result_t st_lnbh29_SetTone (sonysat_lnbc_t * pLnbc,
                                        sonysat_lnbc_tone_t tone);

static sonysat_result_t st_lnbh29_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                sonysat_lnbc_transmit_mode_t mode);

static sonysat_result_t st_lnbh29_Sleep (sonysat_lnbc_t * pLnbc);

static sonysat_result_t st_lnbh29_WakeUp (sonysat_lnbc_t * pLnbc);

static sonysat_result_t st_lnbh29_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                           uint8_t * pOLF);

static sonysat_result_t create_data (sonysat_lnbc_t * pLnbc,
                                  sonysat_lnbc_voltage_t voltage,
                                  uint8_t * pData);
/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t st_lnbh29_Create (sonysat_lnbc_t * pLnbc,
                                uint8_t i2cAddress,
                                sonysat_i2c_t * pI2c)
{
    SONYSAT_TRACE_ENTER ("st_lnbh29_Create");

    if ((!pI2c) || (!pLnbc)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->i2cAddress = i2cAddress;
    pLnbc->pI2c = pI2c;

    pLnbc->Initialize = st_lnbh29_Initialize;
    pLnbc->SetConfig = st_lnbh29_SetConfig;
    pLnbc->SetVoltage = st_lnbh29_SetVoltage;
    pLnbc->SetTone = st_lnbh29_SetTone;
    pLnbc->SetTransmitMode = st_lnbh29_SetTransmitMode;
    pLnbc->Sleep = st_lnbh29_Sleep;
    pLnbc->WakeUp = st_lnbh29_WakeUp;
    pLnbc->MonitorOverload = st_lnbh29_MonitorOLF;

    pLnbc->isInternalTone = 1; /* Fixed to 1 */
    pLnbc->lowVoltage = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V; /* Default value */
    pLnbc->highVoltage = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V; /* Default value */

    pLnbc->state = SONYSAT_LNBC_STATE_UNKNOWN;
    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_Initialize (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("st_lnbh29_Initialize");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->isInternalTone = 1; /* Fixed to 1 */
    pLnbc->lowVoltage = ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V; /* Default value */
    pLnbc->highVoltage = ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V; /* Default value */

    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x01, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_SetConfig (sonysat_lnbc_t * pLnbc,
                                          sonysat_lnbc_config_id_t configId,
                                          int32_t value)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("st_lnbh29_SetConfig");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) &&
        (pLnbc->state != SONYSAT_LNBC_STATE_SLEEP)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(configId)
    {
    case SONYSAT_LNBC_CONFIG_ID_TONE_INTERNAL:
        if ((value != 0) && (value != 1)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->isInternalTone = value;
            result = st_lnbh29_SetTone (pLnbc, pLnbc->tone);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE:
        if ((value != ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V) &&
            (value != ST_LNBH29_CONFIG_VOLTAGE_LOW_13_667V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->lowVoltage = value;
            result = st_lnbh29_SetVoltage (pLnbc, pLnbc->voltage);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE:
        if ((value != ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V) &&
            (value != ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_817V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->highVoltage = value;
            result = st_lnbh29_SetVoltage (pLnbc, pLnbc->voltage);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_SetVoltage (sonysat_lnbc_t * pLnbc,
                                           sonysat_lnbc_voltage_t voltage)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("st_lnbh29_SetVoltage");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) &&
        (pLnbc->state != SONYSAT_LNBC_STATE_SLEEP)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_data (pLnbc, voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE) {
        if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x01, data) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    pLnbc->voltage = voltage;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_SetTone (sonysat_lnbc_t * pLnbc,
                                        sonysat_lnbc_tone_t tone)
{
    SONYSAT_TRACE_ENTER ("st_lnbh29_SetTone");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) &&
        (pLnbc->state != SONYSAT_LNBC_STATE_SLEEP)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(tone)
    {
    case SONYSAT_LNBC_TONE_ON:
    case SONYSAT_LNBC_TONE_OFF:
        /* ST LNBH29 doesn't support this function. */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_NOSUPPORT);
        break;

    case SONYSAT_LNBC_TONE_AUTO:
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->tone = tone;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                sonysat_lnbc_transmit_mode_t mode)
{
    SONYSAT_TRACE_ENTER ("st_lnbh29_SetTransmitMode");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE) &&
        (pLnbc->state != SONYSAT_LNBC_STATE_SLEEP)) {
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

static sonysat_result_t st_lnbh29_Sleep (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("st_lnbh29_Sleep");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(pLnbc->state)
    {
    case SONYSAT_LNBC_STATE_ACTIVE:
        /* Continue */
        break;

    case SONYSAT_LNBC_STATE_SLEEP:
        /* Do nothing */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);

    case SONYSAT_LNBC_STATE_UNKNOWN:
    default:
        /* Error */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x01, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_WakeUp (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("st_lnbh29_WakeUp");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(pLnbc->state)
    {
    case SONYSAT_LNBC_STATE_ACTIVE:
        /* Do nothing */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);

    case SONYSAT_LNBC_STATE_SLEEP:
        /* Continue */
        break;

    case SONYSAT_LNBC_STATE_UNKNOWN:
    default:
        /* Error */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_data (pLnbc, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x01, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t st_lnbh29_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                           uint8_t * pOLF)
{
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("allegro_a8304_MonitorOLF");

    if ((!pLnbc) || (!pOLF)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->pI2c->ReadRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x00, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if(data & 0x10){
        *pOLF = 1;
    } else {
        *pOLF = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t create_data (sonysat_lnbc_t * pLnbc,
                                  sonysat_lnbc_voltage_t voltage,
                                  uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("create_data");
    
    if ((!pLnbc) || (!pData)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    *pData = 0;

    switch(voltage)
    {
    case SONYSAT_LNBC_VOLTAGE_LOW:
        switch(pLnbc->lowVoltage)
        {
        case ST_LNBH29_CONFIG_VOLTAGE_LOW_13_0V:
            /* 13.0V */
            *pData |= 0x01;
            break;

        case ST_LNBH29_CONFIG_VOLTAGE_LOW_13_667V:
            /* 13.667V */
            *pData |= 0x03;
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
        break;

    case SONYSAT_LNBC_VOLTAGE_HIGH:
        switch(pLnbc->highVoltage)
        {
        case ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_150V:
            /* 18.150V */
            *pData |= 0x05;
            break;

        case ST_LNBH29_CONFIG_VOLTAGE_HIGH_18_817V:
            /* 18.817V */
            *pData |= 0x07;
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
