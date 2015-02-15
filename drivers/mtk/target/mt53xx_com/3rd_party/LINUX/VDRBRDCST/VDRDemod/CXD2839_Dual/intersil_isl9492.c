/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_lnbc.h"
#include "intersil_isl9492.h"

/*----------------------------------------------------------------------------
  Static Functions
----------------------------------------------------------------------------*/
static sonysat_result_t create_1st_byte (sonysat_lnbc_t * pLnbc,
                                      sonysat_lnbc_tone_t tone,
                                      sonysat_lnbc_transmit_mode_t mode,
                                      uint8_t * pData);

static sonysat_result_t create_2nd_byte (sonysat_lnbc_t * pLnbc,
                                      sonysat_lnbc_voltage_t voltage,
                                      uint8_t * pData);

static sonysat_result_t create_3rd_byte (sonysat_lnbc_t * pLnbc,
                                      uint8_t isEnable,
                                      sonysat_lnbc_voltage_t voltage,
                                      uint8_t * pData);

static sonysat_result_t intersil_isl9492_Initialize (sonysat_lnbc_t * pLnbc);

static sonysat_result_t intersil_isl9492_SetConfig (sonysat_lnbc_t * pLnbc,
                                                 sonysat_lnbc_config_id_t configId,
                                                 int32_t value);

static sonysat_result_t intersil_isl9492_SetVoltage (sonysat_lnbc_t * pLnbc,
                                                  sonysat_lnbc_voltage_t voltage);

static sonysat_result_t intersil_isl9492_SetTone (sonysat_lnbc_t * pLnbc,
                                               sonysat_lnbc_tone_t tone);

static sonysat_result_t intersil_isl9492_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                       sonysat_lnbc_transmit_mode_t mode);

static sonysat_result_t intersil_isl9492_Sleep (sonysat_lnbc_t * pLnbc);

static sonysat_result_t intersil_isl9492_WakeUp (sonysat_lnbc_t * pLnbc);

static sonysat_result_t intersil_isl9492_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                          uint8_t * pOLF);

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
sonysat_result_t intersil_isl9492_Create (sonysat_lnbc_t * pLnbc,
                                       uint8_t i2cAddress,
                                       sonysat_i2c_t * pI2c)
{
    SONYSAT_TRACE_ENTER ("intersil_isl9492_Create");

    if ((!pLnbc) || (!pI2c)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pLnbc->i2cAddress = i2cAddress;
    pLnbc->pI2c = pI2c;

    pLnbc->Initialize = intersil_isl9492_Initialize;
    pLnbc->SetConfig = intersil_isl9492_SetConfig;
    pLnbc->SetVoltage = intersil_isl9492_SetVoltage;
    pLnbc->SetTone = intersil_isl9492_SetTone;
    pLnbc->SetTransmitMode = intersil_isl9492_SetTransmitMode;
    pLnbc->Sleep = intersil_isl9492_Sleep;
    pLnbc->WakeUp = intersil_isl9492_WakeUp;
    pLnbc->MonitorOverload = intersil_isl9492_MonitorOLF;

    pLnbc->isInternalTone = 0;
    pLnbc->lowVoltage = INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_13_5V; /* Default value */
    pLnbc->highVoltage = INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_18_5V; /* Default value */

    pLnbc->state = SONYSAT_LNBC_STATE_UNKNOWN;
    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_Initialize (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_Initialize");

    pLnbc->isInternalTone = 0;
    pLnbc->lowVoltage = INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_13_5V; /* Default value */
    pLnbc->highVoltage = INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_18_5V; /* Default value */

    pLnbc->voltage = SONYSAT_LNBC_VOLTAGE_LOW;
    pLnbc->tone = SONYSAT_LNBC_TONE_AUTO;
    pLnbc->transmitMode = SONYSAT_LNBC_TRANSMIT_MODE_TX;

    result = create_1st_byte (pLnbc, pLnbc->tone, pLnbc->transmitMode, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = create_2nd_byte (pLnbc, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = create_3rd_byte (pLnbc, 1, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_SetConfig (sonysat_lnbc_t * pLnbc,
                                                 sonysat_lnbc_config_id_t configId,
                                                 int32_t value)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_SetConfig");

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
                result = intersil_isl9492_SetTone (pLnbc, pLnbc->tone);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_LOW_VOLTAGE:
        if ((value != INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_13_5V) &&
            (value != INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_14_2V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->lowVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE){
                result = intersil_isl9492_SetVoltage (pLnbc, pLnbc->voltage);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
        }
        break;

    case SONYSAT_LNBC_CONFIG_ID_HIGH_VOLTAGE:
        if ((value != INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_18_5V) &&
            (value != INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_20_0V)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        } else {
            pLnbc->highVoltage = value;
            if (pLnbc->state == SONYSAT_LNBC_STATE_ACTIVE){
                result = intersil_isl9492_SetVoltage (pLnbc, pLnbc->voltage);
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

static sonysat_result_t intersil_isl9492_SetVoltage (sonysat_lnbc_t * pLnbc,
                                                  sonysat_lnbc_voltage_t voltage)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_SetVoltage");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_AUTO){
        switch(voltage)
        {
        case SONYSAT_LNBC_VOLTAGE_AUTO:
            /* Do nothing */
            break;

        case SONYSAT_LNBC_VOLTAGE_LOW:
        case SONYSAT_LNBC_VOLTAGE_HIGH:
            result = create_2nd_byte (pLnbc, voltage, &data);
            if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
            if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }
    } else if ((pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_LOW) || (pLnbc->voltage == SONYSAT_LNBC_VOLTAGE_HIGH)){
        switch(voltage)
        {
        case SONYSAT_LNBC_VOLTAGE_AUTO:
            /* Low/High -> Auto */
            result = create_2nd_byte (pLnbc, voltage, &data);
            if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
            if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }
            break;

        case SONYSAT_LNBC_VOLTAGE_LOW:
        case SONYSAT_LNBC_VOLTAGE_HIGH:
            /* Do nothing */
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_3rd_byte (pLnbc, 1, voltage, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->voltage = voltage;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_SetTone (sonysat_lnbc_t * pLnbc,
                                               sonysat_lnbc_tone_t tone)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_SetTone");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_1st_byte (pLnbc, tone, pLnbc->transmitMode, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->tone = tone;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_SetTransmitMode (sonysat_lnbc_t * pLnbc,
                                                       sonysat_lnbc_transmit_mode_t mode)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_SetTransmitMode");

    if (!pLnbc){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->state != SONYSAT_LNBC_STATE_ACTIVE){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = create_1st_byte (pLnbc, pLnbc->tone, mode, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->transmitMode = mode;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_Sleep (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_Sleep");

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

    result = create_3rd_byte (pLnbc, 0, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_SLEEP;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_WakeUp (sonysat_lnbc_t * pLnbc)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_WakeUp");

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

    result = create_3rd_byte (pLnbc, 1, pLnbc->voltage, &data);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}
    if (pLnbc->pI2c->Write(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    pLnbc->state = SONYSAT_LNBC_STATE_ACTIVE;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t intersil_isl9492_MonitorOLF (sonysat_lnbc_t * pLnbc,
                                          uint8_t * pOLF)
{
    //sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;

    SONYSAT_TRACE_ENTER ("intersil_isl9492_MonitorOLF");

    if ((!pLnbc) || (!pOLF)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pLnbc->pI2c->Read(pLnbc->pI2c, pLnbc->i2cAddress, &data, 1, SONYSAT_I2C_START_EN|SONYSAT_I2C_STOP_EN) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    
    if(data & 0x02){
        *pOLF = 1;
    } else {
        *pOLF = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t create_1st_byte (sonysat_lnbc_t * pLnbc,
                                      sonysat_lnbc_tone_t tone,
                                      sonysat_lnbc_transmit_mode_t mode,
                                      uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("create_1st_byte");

    if ((!pLnbc) || (!pData)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* SR2
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |SR2H |SR2M |SR2L | ENT |MSEL | TTH |Res* |Res* |
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |  0  |  0  |  1  |  ?  |  ?  |  ?  |  0  |  0  |
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * ENT  = 0 : Tone OFF ( or auto )
     *      = 1 : Tone ON
     *
     * MSEL = 0 : Envelope (internal tone) / I2C control
     *      = 1 : PWM      (external tone)
     *
     * TTH  = 0 : RX mode
     *      = 1 : TX mode
     */
    *pData = 0x20;

    switch(tone)
    {
    case SONYSAT_LNBC_TONE_OFF:
    case SONYSAT_LNBC_TONE_AUTO:
        /* ENT = 0 */
        break;

    case SONYSAT_LNBC_TONE_ON:
        /* ENT = 1 */
        *pData |= 0x10;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(tone)
    {
    case SONYSAT_LNBC_TONE_AUTO:
        if (pLnbc->isInternalTone){
            /* MSEL = 0 */
        } else {
            /* MSEL = 1 */
            *pData |= 0x08;
        }
        break;

    case SONYSAT_LNBC_TONE_OFF:
    case SONYSAT_LNBC_TONE_ON:
        /* MSEL = 0 */
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    switch(mode)
    {
    case SONYSAT_LNBC_TRANSMIT_MODE_TX:
        /* TTH = 1 */
        *pData |= 0x04;
        break;

    case SONYSAT_LNBC_TRANSMIT_MODE_RX:
    case SONYSAT_LNBC_TRANSMIT_MODE_AUTO:
        /* TTH = 0 */
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t create_2nd_byte (sonysat_lnbc_t * pLnbc,
                                      sonysat_lnbc_voltage_t voltage,
                                      uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("create_2nd_byte");

    if ((!pLnbc) || (!pData)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* SR3
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |SR3H |SR3M |SR3L |Res* |VSPEN| DCL |ISELH|ISELL|
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |  0  |  1  |  0  |  0  |  ?  |  0  |  0  |  0  |
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * VSPEN = 0 : TXEN enable
     *       = 1 : TXEN disable
     */
    *pData = 0x40;

    switch(voltage)
    {
    case SONYSAT_LNBC_VOLTAGE_LOW:
    case SONYSAT_LNBC_VOLTAGE_HIGH:
        /* VSPEN = 1 */
        *pData |= 0x08;
        break;

    case SONYSAT_LNBC_VOLTAGE_AUTO:
        /* VSPEN = 0 */
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t create_3rd_byte (sonysat_lnbc_t * pLnbc,
                                      uint8_t isEnable,
                                      sonysat_lnbc_voltage_t voltage,
                                      uint8_t * pData)
{
    SONYSAT_TRACE_ENTER ("create_3rd_byte");

    if ((!pLnbc) || (!pData)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* SR4
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |SR4H |SR4M |SR4L | EN  |DLIN |Res* |VTOP |VBOT |
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * |  0  |  1  |  1  |  ?  |  ?  |  0  |  ?  |  ?  |
     * +-----+-----+-----+-----+-----+-----+-----+-----+
     * EN   = 0 : SLEEP
     *      = 1 : ACTIVE
     *
     * DLIN = 0 : SLEEP
     *      = 1 : ACTIVE
     *
     * VTOP = 0 : Others
     *      = 1 : (Auto & HIGH-20V) / (High)
     *
     * VBOT = 0 : Others
     *      = 1 : (Auto & LOW-14V) / (Low & LOW-14V) / (High & HIGH-20V)
     */
    *pData = 0x60;

    if (isEnable){
        /* EN = 1, DLIN = 1 */
        *pData |= 0x18;
    }

    if ((voltage == SONYSAT_LNBC_VOLTAGE_AUTO) && (pLnbc->highVoltage == INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_20_0V)){
        /* VTOP = 1 */
        *pData |= 0x02;
    } else if (voltage == SONYSAT_LNBC_VOLTAGE_HIGH){
        /* VTOP = 1 */
        *pData |= 0x02;
    } else {
        /* VTOP = 0 */
    }

    if ((voltage == SONYSAT_LNBC_VOLTAGE_AUTO) && (pLnbc->lowVoltage == INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_14_2V)){
        /* VBOT = 1 */
        *pData |= 0x01;
    } else if ((voltage == SONYSAT_LNBC_VOLTAGE_LOW) && (pLnbc->lowVoltage == INTERSIL_ISL9492_CONFIG_VOLTAGE_LOW_14_2V)){
        /* VBOT = 1 */
        *pData |= 0x01;
    } else if ((voltage == SONYSAT_LNBC_VOLTAGE_HIGH) && (pLnbc->highVoltage == INTERSIL_ISL9492_CONFIG_VOLTAGE_HIGH_20_0V)){
        /* VBOT = 1 */
        *pData |= 0x01;
    } else {
        /* VBOT = 0 */
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
