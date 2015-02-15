/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod.h"
#include "sonysat_demod_sat_device_ctrl.h"

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_sat_device_ctrl_DSQOUTSetting (sonysat_demod_t * pDemod,
                                                        sonysat_dsqout_mode_t mode,
                                                        uint8_t toneFreqKHz)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_DSQOUTSetting");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if(toneFreqKHz == 22){
        switch(pDemod->xtalFreq)
        {
        case SONYSAT_DEMOD_XTAL_20500KHz:
        case SONYSAT_DEMOD_XTAL_41000KHz:
            /* Keep default value. */
            break;

        case SONYSAT_DEMOD_XTAL_24000KHz:
            /* ODISEQC_CKDIV[11:8] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x30, 0x04, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* ODISEQC_CKDIV[7:0] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x31, 0x43, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* ODISEQC_CKDIV220K[9:8] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x57, 0x00, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* ODISEQC_CKDIV220K[7:0] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x58, 0xDA, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
    } else if(toneFreqKHz == 44){
        /* Settings for 44KHz tone. */
        switch(pDemod->xtalFreq)
        {
        case SONYSAT_DEMOD_XTAL_20500KHz:
        case SONYSAT_DEMOD_XTAL_41000KHz:

            /*  (In case of 2k12 Generation Demod Chips) */
            if (SONYSAT_DEMOD_CHIP_ID_2k12_GENERATION (pDemod->chipId)) {
                /* ODISEQC_CKDIV[11:8] */
                result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x30, 0x01, 0xFF);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                /* ODISEQC_CKDIV[7:0] */
                result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x31, 0x71, 0xFF);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }

            /*  (In case of 2k14 Generation Demod Chips) */
            if (SONYSAT_DEMOD_CHIP_ID_2k14_GENERATION (pDemod->chipId)) {
                /* ODISEQC_CKDIV[11:8] */
                result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x30, 0x02, 0xFF);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
                /* ODISEQC_CKDIV[7:0] */
                result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x31, 0x29, 0xFF);
                if (result != SONYSAT_RESULT_OK){
                    SONYSAT_TRACE_RETURN (result);
                }
            }
            /*
             * No need to set ODISEQC_CKDIV220K because that register is for DiSEqC2.x.
             * 44KHz tone is not for DiSEqC.
             */
            break;

        case SONYSAT_DEMOD_XTAL_24000KHz:
            /* ODISEQC_CKDIV[11:8] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x30, 0x02, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* ODISEQC_CKDIV[7:0] */
            result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x31, 0x21, 0xFF);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /*
             * No need to set ODISEQC_CKDIV220K because that register is for DiSEqC2.x.
             * 44KHz tone is not for DiSEqC.
             */
            break;
        
        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
        }
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
    }

    switch(mode)
    {
    case SONYSAT_DSQOUT_MODE_PWM:
        /* Default */
        data = 0x00;
        break;

    case SONYSAT_DSQOUT_MODE_ENVELOPE:
        data = 0x01;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* ODISEQC_LVLOUTON[0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x75, data, 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_RXENSetting (sonysat_demod_t * pDemod,
                                                      sonysat_rxen_mode_t mode,
                                                      uint8_t posDelay)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data[2] = {0, 0};
    uint32_t value = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_RXENSetting");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(mode)
    {
    case SONYSAT_RXEN_MODE_NORMAL:
        /*
         * RXEN signal is normal.
         *   High : RX mode.
         *   Low  : TX mode.
         */
        data[0] = 0x00;
        break;

    case SONYSAT_RXEN_MODE_INV:
        /*
         * RXEN signal is inverted.
         *   High : TX mode.
         *   Low  : RX mode.
         */
        data[0] = 0x01;
        break;

    case SONYSAT_RXEN_MODE_FIXED_LOW:
        /*
         * RXEN signal is fixed low.
         */
        data[0] = 0x02;
        break;

    case SONYSAT_RXEN_MODE_FIXED_HIGH:
        /*
         * RXEN signal is fixed high.
         */
        data[0] = 0x03;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* ODISEQC_RXENSEL[1:0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x78, data[0], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Fixed to 22KHz base */
    value = posDelay * 22;

    data[0] = (uint8_t)((value >> 8) & 0xFF);
    data[1] = (uint8_t)( value       & 0xFF);

    /*
     * "!" is the time to finish sending the DiSEqC command.
     *
     *              +-------------+
     *              |             |
     *  ---!--------+             +---------
     *      <------>
     *       posDelay(ms)
     */

    /* ODISEQC_RXPOSDLY[13:8] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x7D, data[0], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
    /* ODISEQC_RXPOSDLY[7:0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x7E, data[1], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_TXENSetting (sonysat_demod_t * pDemod,
                                                      sonysat_txen_mode_t mode,
                                                      uint8_t posDelay,
                                                      uint8_t negDelay)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data[2] = {0, 0};
    uint32_t value = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_TXENSetting");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) && 
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_T_C) && (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    switch(mode)
    {
    case SONYSAT_TXEN_MODE_NORMAL:
        data[0] = 0x00;
        break;

    case SONYSAT_TXEN_MODE_INV:
        data[0] = 0x01;
        break;

    case SONYSAT_TXEN_MODE_FIXED_LOW:
        data[0] = 0x02;
        break;

    case SONYSAT_TXEN_MODE_FIXED_HIGH:
        data[0] = 0x03;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* ODISEQC_TXENSEL[1:0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x77, data[0], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Fixed to 22KHz base */
    value = posDelay * 22;

    data[0] = (uint8_t)((value >> 8) & 0xFF);
    data[1] = (uint8_t)( value       & 0xFF);

    /*
     * "!" is the time to start sequence to transmit single cable command.
     * "#" is the time to finish sequence to transmit single cable command.
     *
     *              +-------------+
     *              |             |
     *  ---!--------+             +--------#-
     *      <------>               <------>
     *       posDelay(ms)           negDelay(ms)
     */

    /* ODISEQC_TXPOSDLY[13:8] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x79, data[0], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
    /* ODISEQC_TXPOSDLY[7:0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x7A, data[1], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    value = negDelay * 22;

    data[0] = (uint8_t)((value >> 8) & 0xFF);
    data[1] = (uint8_t)( value       & 0xFF);

    /* ODISEQC_TXNEGDLY[13:8] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x7B, data[0], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }
    /* ODISEQC_TXNEGDLY[7:0] */
    result = sonysat_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xBB, 0x7C, data[1], 0xFF);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_OutputTone(sonysat_demod_t * pDemod,
                                                    uint8_t isEnable)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_OutputTone");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (isEnable){
        /* Tone ON */
        data = 0x01;
    } else {
        /* Tone OFF */
        data = 0x00;
    }

    /* ODISEQC_CBON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x36, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_SetToneBurst(sonysat_demod_t * pDemod,
                                                      sonysat_toneburst_mode_t toneBurstMode)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_SetToneBurst");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    switch(toneBurstMode)
    {
    case SONYSAT_TONEBURST_MODE_OFF:
        /* ODISEQC_TBON[0] Disable */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x34, 0x00) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);

    case SONYSAT_TONEBURST_MODE_A:
        data = 0x00;
        break;

    case SONYSAT_TONEBURST_MODE_B:
        data = 0x01;
        break;

    default:
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* ODISEQC_TBON[0] Enable */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x34, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* ODISEQC_TBSEL[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x35, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_SetDiseqcCommand(sonysat_demod_t * pDemod,
                                                          uint8_t isEnable,
                                                          sonysat_diseqc_message_t * pCommand1,
                                                          uint8_t count1,
                                                          sonysat_diseqc_message_t * pCommand2,
                                                          uint8_t count2)
{
    int32_t index = 0;
    uint8_t data[SONYSAT_DISEQC_MESSAGE_LENGTH];
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_SetDiseqcCommand");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (isEnable){
        if ((!pCommand1) || (count1 == 0)){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }

        /* ODISEQC_DISEQCON[0] Enable */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* Length check */
        if (pCommand1->length > SONYSAT_DISEQC_MESSAGE_LENGTH){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }

        /* ODISEQC_DCLEN0[3:0] Length of command1 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3D, pCommand1->length) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* Data of command1 */
        for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
            data[index] = 0x00;
        }
        for(index = 0; index < pCommand1->length; index++){
            data[index] = pCommand1->data[index];
        }
        /* ODISEQC_DCDAT0_1[7:0] - ODISEQC_DCDAT0_C[7:0] */
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3E, data, SONYSAT_DISEQC_MESSAGE_LENGTH) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* ODISEQC_DCREP0[3:0] Repeat count of command1 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x37, count1) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        if ((!pCommand2) || (count2 == 0)){
            /* ODISEQC_DCREP1[3:0] Repeat count of command2 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, 0x00) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }
        } else {
            /* Length check */
            if (pCommand2->length > SONYSAT_DISEQC_MESSAGE_LENGTH){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
            }
            /* ODISEQC_DCLEN1[3:0] Length of command2 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4A, pCommand2->length) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* Data of command2 */
            for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
                data[index] = 0x00;
            }
            for(index = 0; index < pCommand2->length; index++){
                data[index] = pCommand2->data[index];
            }
            /* ODISEQC_DCDAT1_1[7:0] - ODISEQC_DCDAT1_C[7:0] */
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, data, SONYSAT_DISEQC_MESSAGE_LENGTH) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }

            /* ODISEQC_DCREP1[3:0] Repeat count of command2 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, count2) != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
            }
        }
    } else {
        /* ODISEQC_DISEQCON[0] Disable */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, 0x00) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_SetIdleTime(sonysat_demod_t * pDemod,
                                                     uint8_t idleTimeMs)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t value = 0;
    uint8_t data[2];
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_SetIdleTime");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Fixed to 22KHz */
    value = idleTimeMs * 22;
    if (value > 0x0FFF){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    data[0] = (uint8_t)((value >> 8) & 0x0F);
    data[1] = (uint8_t)( value       & 0xFF);

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* ODISEQC_TIMIDLETX[11:0] */
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x39, data, 2) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_SetR2RTime (sonysat_demod_t * pDemod,
                                                     uint32_t r2rTime)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t value = 0;
    uint8_t data[2];
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_SetR2RTime");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    value = r2rTime * 22;
    if (value > 0x3FFF){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    data[0] = (uint8_t)((value >> 8) & 0x3F);
    data[1] = (uint8_t)( value       & 0xFF);

    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x71, data, 2) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode(sonysat_demod_t * pDemod,
                                                            uint8_t isEnable,
                                                            uint16_t replyTimeoutMs)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint32_t value = 0;
    uint8_t data[2];
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_SetDiseqcReplyMode");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    if (isEnable){
        /* Set SLV-T Bank : 0xBB */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        value = (uint32_t)replyTimeoutMs * 22;
        if (value > 0x3FFF){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
        }
        data[0] = (uint8_t)((value >> 8) & 0x3F);
        data[1] = (uint8_t)( value       & 0xFF);

        /* ODISEQC_TIMIDLEREPLY[13:0] */
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3B, data, 2) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        /* diseqc_readok[0] Clear reply data register. */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    } else {
        /* Do nothing */
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_StartTransmit(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_StartTransmit");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* diseqc_transmit[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_GetTransmitStatus(sonysat_demod_t * pDemod,
                                                           uint8_t * pStatus)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_GetTransmitStatus");

    if ((!pDemod) || (!pStatus)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* IDISEQC_TXSTATE[1:0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pStatus = data;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_GetReplyMessage(sonysat_demod_t * pDemod,
                                                         sonysat_diseqc_message_t * pReplyMessage)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data = 0;
    int32_t index = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_GetReplyMessage");

    if ((!pDemod) || (!pReplyMessage)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0xBB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* IDISEQC_DRVAL,IDISEQC_DROVF,IDISEQC_DRPERR,IDISEQC_DRDERR (4bit) */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data == 0x08){
        /* Success to receive reply message. */
        /* IDISEQC_DRLEN[4:0] Length of reply message */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &data, 1) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        if (data > SONYSAT_DISEQC_MESSAGE_LENGTH){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }

        pReplyMessage->length = data;
        for(index = 0; index < SONYSAT_DISEQC_MESSAGE_LENGTH; index++){
            pReplyMessage->data[index] = 0;
        }
        /* IDISEQC_DRDAT0[7:0] - IDISEQC_DRDATF[7:0] */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x13, pReplyMessage->data, pReplyMessage->length) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_GetRXENMode (sonysat_demod_t * pDemod,
                                                      uint8_t * pIsEnable)
{
    uint8_t hiz = 0;
    uint8_t data[3] = {0, 0, 0};
    uint8_t tsData6Sel = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_GetRXENMode");

    if ((!pDemod) || (!pIsEnable)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x82, &hiz, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xA3, data, 3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Slave     Bank   Addr   Bit     Default    Name
     * ----------------------------------------------------------
     * <SLV-T>   00h    A8h    [2:0]   3'b000     OREG_TSDATA6SEL
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA8, &tsData6Sel, 1) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if ((((hiz & 0x01) == 0) && (data[0] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_RX_EN)) ||
        (((hiz & 0x02) == 0) && (data[1] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_RX_EN)) ||
        (((hiz & 0x04) == 0) && (data[2] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_RX_EN)) ||
        ((tsData6Sel & 0x07) == 0x03)) {
        /* Set SLV-T Bank : 0xBB */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* ODISEQC_RXENSEL[1:0] */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x78, data, 1) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        switch(data[0])
        {
        case 0x00: /* Normal */
        case 0x01: /* Inverted */
            *pIsEnable = 1;
            break;

        case 0x02: /* Fixed low */
        case 0x03: /* Fixed high */
            *pIsEnable = 0;
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
    } else {
        *pIsEnable = 0;
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_GetTXENMode (sonysat_demod_t * pDemod,
                                                      uint8_t * pIsEnable)
{
    uint8_t hiz = 0;
    uint8_t data[3] = {0, 0, 0};
    uint8_t tsData6Sel = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_GetTXENMode");

    if ((!pDemod) || (!pIsEnable)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S)){
        /* This api is accepted in SLEEP_S and ACTIVE_S states only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x82, &hiz, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xA3, data, 3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Slave     Bank   Addr   Bit     Default    Name
     * ----------------------------------------------------------
     * <SLV-T>   00h    A8h    [2:0]   3'b000     OREG_TSDATA6SEL
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA8, &tsData6Sel, 1) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if ((((hiz & 0x01) == 0) && (data[0] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_TX_EN)) ||
        (((hiz & 0x02) == 0) && (data[1] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_TX_EN)) ||
        (((hiz & 0x04) == 0) && (data[2] == (uint8_t)SONYSAT_DEMOD_GPIO_MODE_DISEQC_TX_EN)) ||
        ((tsData6Sel & 0x07) == 0x02)) {
        /* Set SLV-T Bank : 0xBB */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xBB) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* ODISEQC_TXENSEL[1:0] */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x77, data, 1) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }

        switch(data[0])
        {
        case 0x00: /* Normal */
        case 0x01: /* Inverted */
            *pIsEnable = 1;
            break;

        case 0x02: /* Fixed low */
        case 0x03: /* Fixed high */
            *pIsEnable = 0;
            break;

        default:
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_HW_STATE);
        }
    } else {
        *pIsEnable = 0;
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_sat_device_ctrl_EnableSinglecable (sonysat_demod_t * pDemod,
                                                            uint8_t enable)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_sat_device_ctrl_EnableSinglecable");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    pDemod->isSinglecable = enable;

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
