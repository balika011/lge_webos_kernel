/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "allegro_a8304.h"

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t allegro_a8304_Initialize (sonysat_lnbc_t * pLnbc);

static sonysat_result_t allegro_a8304_SetConfig (sonysat_lnbc_t * pLnbc,
                                              sonysat_lnbc_config_id_t configId,
                                              int32_t value);

static sonysat_result_t allegro_a8304_SetVoltage (sonysat_lnbc_t * pLnbc,
                                               sonysat_lnbc_voltage_t voltage);

static sonysat_result_t allegro_a8304_SetTone (sonysat_lnbc_t * pLnbc,
                                            sonysat_lnbc_tone_t tone);

static sonysat_result_t allegro_a8304_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                    sonysat_lnbc_transmit_mode_t mode);

static sonysat_result_t allegro_a8304_Sleep (sonysat_lnbc_t * pLnbc);

static sonysat_result_t allegro_a8304_WakeUp (sonysat_lnbc_t * pLnbc);

static sonysat_result_t create_data (sonysat_lnbc_t * pLnbc,
                                  uint8_t isEnable,
                                  sonysat_lnbc_voltage_t voltage,
                                  uint8_t * pData);
/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t allegro_a8304_Create (sonysat_lnbc_t * pLnbc,
                                    uint8_t i2cAddress,
                                    sonysat_i2c_t * pI2c)
{
    SONYSAT_TRACE_ENTER ("allegro_a8304_Create");

    if ((!pI2c) || (!pLnbc)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->i2cAddress = i2cAddress;
    pLnbc->pI2c = pI2c;

    pLnbc->Initialize = allegro_a8304_Initialize;
    pLnbc->SetConfig = allegro_a8304_SetConfig;
    pLnbc->SetVoltage = allegro_a8304_SetVoltage;
    pLnbc->SetTone = allegro_a8304_SetTone;
    pLnbc->SetTransmitMode = allegro_a8304_SetTransmitMode;
    pLnbc->Sleep = allegro_a8304_Sleep;
    pLnbc->WakeUp = allegro_a8304_WakeUp;

    pLnbc->isInternalTone = 1; /* Fixed to 1 */
    pLnbc->lowVoltage = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V; /* Default value */
    pLnbc->highVoltage = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V; /* Default value */

    pLnbc->state = SONYSAT_LNBC_STATE_UNKNOWN;
    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t allegro_a8304_Initialize (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("allegro_a8304_Initialize");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->isInternalTone = 1; /* Fixed to 1 */
    pLnbc->lowVoltage = ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V; /* Default value */
    pLnbc->highVoltage = ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V; /* Default value */

    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    data = 0x00;
    if (pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pLnbc->pI2c->Read (pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    data = 0x00;
    if (pLnbc->pI2c->Write (pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pLnbc->pI2c->Read (pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = create_data (pLnbc, 1, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x00, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t allegro_a8304_SetConfig (sonysat_lnbc_t * pLnbc,
                                              sonysat_lnbc_config_id_t configId,
                                              int32_t value)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("allegro_a8304_SetConfig");

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
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE){
                result = allegro_a8304_SetTone (pLnbc, pLnbc->tone);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE:
        if ((value != ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V) &&
            (value != ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_14_333V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->lowVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE){
                result = allegro_a8304_SetVoltage (pLnbc, pLnbc->voltage);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE:
        if ((value != ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V) &&
            (value != ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_19_667V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->highVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE){
                result = allegro_a8304_SetVoltage (pLnbc, pLnbc->voltage);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t allegro_a8304_SetVoltage (sonysat_lnbc_t * pLnbc,
                                               sonysat_lnbc_voltage_t voltage)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("allegro_a8304_SetVoltage");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_data (pLnbc, 1, voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x00, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->voltage = voltage;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t allegro_a8304_SetTone (sonysat_lnbc_t * pLnbc,
                                            sonysat_lnbc_tone_t tone)
{
    SONYSAT_TRACE_ENTER ("allegro_a8304_SetTone");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(tone)
    {
    case SONYSAT_LNBC_TONE_ON:
    case SONYSAT_LNBC_TONE_OFF:
        /* A8304 doesn't support this function. */
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

static sonysat_result_t allegro_a8304_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                    sonysat_lnbc_transmit_mode_t mode)
{
    SONYSAT_TRACE_ENTER ("allegro_a8304_SetTransmitMode");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
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

static sonysat_result_t allegro_a8304_Sleep (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("allegro_a8304_Sleep");

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

    result = create_data (pLnbc, 0, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x00, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t allegro_a8304_WakeUp (sonysat_lnbc_t * pLnbc)
{
    uint8_t data = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("allegro_a8304_WakeUp");

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

    result = create_data (pLnbc, 1, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if (pLnbc->pI2c->WriteOneRegister (pLnbc->pI2c, pLnbc->i2cAddress, 0x00, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t create_data (sonysat_lnbc_t * pLnbc,
                                  uint8_t isEnable,
                                  sonysat_lnbc_voltage_t voltage,
                                  uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("create_data");

    *pData = 0;

    if (isEnable){
        *pData |= 0x10;
    }

    if (!pLnbc->isInternalTone){
        /* TMODE = 1 */
        *pData |= 0x20;
    }

    switch(voltage)
    {
    case SONYSAT_LNBC_VOLTAGE_LOW:
        switch(pLnbc->lowVoltage)
        {
        case ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_13_333V:
            /* 13.333V */
            *pData |= 0x02;
            break;

        case ALLEGRO_A8304_CONFIG_VOLTAGE_LOW_14_333V:
            /* 14.333V */
            *pData |= 0x05;
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
        break;

    case SONYSAT_LNBC_VOLTAGE_HIGH:
        switch(pLnbc->highVoltage)
        {
        case ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_18_667V:
            /* 18.667V */
            *pData |= 0x0B;
            break;

        case ALLEGRO_A8304_CONFIG_VOLTAGE_HIGH_19_667V:
            /* 19.667V */
            *pData |= 0x0E;
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
