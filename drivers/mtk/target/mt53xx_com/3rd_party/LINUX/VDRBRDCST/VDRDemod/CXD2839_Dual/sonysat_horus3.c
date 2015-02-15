/*------------------------------------------------------------------------------
  Copyright 2010-2013 Sony Corporation

  Last Updated  : 2013/05/16
  File Revision : 1.2.1.0
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
 Based on HORUS3(CXD2832) application note 2.1.0 with VCO leak countermeasure sequence.
------------------------------------------------------------------------------*/

#include "sonysat_horus3.h"

/*------------------------------------------------------------------------------
 Definitions of static functions
------------------------------------------------------------------------------*/
/**
 @brief Configure the HORUS3 tuner from Power On to Power Save state.
*/
static sonysat_result_t Initialize(sonysat_horus3_t *pTuner);
/**
 @brief Configure the HORUS3 tuner to Active state.
*/
static sonysat_result_t Tune(sonysat_horus3_t *pTuner, uint32_t frequencykHz,
    sonysat_horus3_tv_system_t tvSystem, uint32_t symbolRateksps);
/**
 @brief Configure the HORUS3 tuner to Power Save state.
*/
static sonysat_result_t EnterPowerSave(sonysat_horus3_t *pTuner);
/**
 @brief Configure the HORUS3 tuner to go out from Power Save state.
*/
static sonysat_result_t LeavePowerSave(sonysat_horus3_t *pTuner);

/*------------------------------------------------------------------------------
 Implementation
------------------------------------------------------------------------------*/

sonysat_result_t sonysat_horus3_Create(sonysat_horus3_t *pTuner, uint32_t xtalFreqMHz,
    uint8_t i2cAddress, sonysat_i2c_t *pI2c, uint32_t flags)
{
    SONYSAT_TRACE_ENTER("sonysat_horus3_Create");

    if((!pTuner) || (!pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    if((xtalFreqMHz != 16) && (xtalFreqMHz != 27)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    pTuner->state = SONYSAT_HORUS3_STATE_UNKNOWN; /* Chip is not accessed for now */
    pTuner->xtalFreqMHz = xtalFreqMHz;
    pTuner->pI2c = pI2c;
    pTuner->i2cAddress = i2cAddress;
    pTuner->flags = flags;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONYSAT_HORUS3_TV_SYSTEM_UNKNOWN;
    pTuner->symbolRateksps = 0;
    pTuner->user = NULL;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_horus3_Initialize(sonysat_horus3_t *pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("sonysat_horus3_Initialize");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    /* Wait 4ms after power on */
    SONYSAT_SLEEP(4);

    result = Initialize(pTuner);
    if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_RETURN(result); }

    pTuner->state = SONYSAT_HORUS3_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONYSAT_HORUS3_TV_SYSTEM_UNKNOWN;
    pTuner->symbolRateksps = 0;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_horus3_Tune(sonysat_horus3_t *pTuner, uint32_t frequencykHz,
    sonysat_horus3_tv_system_t tvSystem, uint32_t symbolRateksps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("sonysat_horus3_Tune");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    if((pTuner->state != SONYSAT_HORUS3_STATE_SLEEP) && (pTuner->state != SONYSAT_HORUS3_STATE_ACTIVE)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* Rough frequency range check */
    if((frequencykHz < 500000) || (frequencykHz > 2500000)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_RANGE);
    }

    if(pTuner->state == SONYSAT_HORUS3_STATE_SLEEP){
        result = LeavePowerSave(pTuner);
        if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_RETURN(result); }
    }

    result = Tune(pTuner, frequencykHz, tvSystem, symbolRateksps);
    if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_RETURN(result); }

    pTuner->state = SONYSAT_HORUS3_STATE_ACTIVE;
    /* pTuner->frequencykHz will be updated in Tune function */
    pTuner->tvSystem = tvSystem;
    pTuner->symbolRateksps = symbolRateksps;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_horus3_Sleep(sonysat_horus3_t *pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("sonysat_horus3_Sleep");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    if((pTuner->state != SONYSAT_HORUS3_STATE_SLEEP) && (pTuner->state != SONYSAT_HORUS3_STATE_ACTIVE)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = EnterPowerSave(pTuner);
    if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_RETURN(result); }

    pTuner->state = SONYSAT_HORUS3_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONYSAT_HORUS3_TV_SYSTEM_UNKNOWN;
    pTuner->symbolRateksps = 0;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_horus3_SetGPO(sonysat_horus3_t *pTuner, uint8_t output)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("sonysat_horus3_SetGPO");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    /* Write GPIO[1:0] */
    result = sonysat_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x2D, (uint8_t)(output ? 0x40 : 0x00), 0xC0);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Implementation of static functions
------------------------------------------------------------------------------*/

static sonysat_result_t Initialize(sonysat_horus3_t *pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("Initialize");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    /* VCO calibration disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO M band */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x28, 0x80);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO_RSW = 3 */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x29, 0x31);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* IQ Generator disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x79);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* REF_R = Xtal Frequency */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x06, (uint8_t)(pTuner->xtalFreqMHz));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* FIN = Xtal Frequency */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x07, (uint8_t)(pTuner->xtalFreqMHz));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x08, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    switch(pTuner->flags & SONYSAT_HORUS3_CONFIG_IQOUT_MASK){
    case SONYSAT_HORUS3_CONFIG_IQOUT_DIFFERENTIAL:
        /* IQ Out = Differential */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0A, 0x00);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
        break;
    case SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND:
        /* IQ Out = Single Ended */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0A, 0x40);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
        break;
    case SONYSAT_HORUS3_CONFIG_IQOUT_SINGLEEND_LOWGAIN:
        /* IQ Out = Single Ended (Low Gain) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0A, 0xC0);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
        break;
    default:
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_OTHER);
    }

    /* Release POWER SAVE pin setting by GPO */
    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_POWERSAVE_BY_GPO){
        result = sonysat_horus3_SetGPO(pTuner, 1);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(result); }
    }

    {
        uint8_t data = 0;

        /* XOSC_SEL setting */
        if(pTuner->flags & SONYSAT_HORUS3_CONFIG_EXT_REF){
            data = 0x00;
        }else{
            switch(pTuner->xtalFreqMHz){
            case 27:
                data = 0x1F; break;
            case 16:
                data = 0x0C; break;
            default:
                SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_OTHER);
            }
            data <<= 2; /* XOSC_SEL is Bit[6:2] */
        }

        if(!(pTuner->flags & SONYSAT_HORUS3_CONFIG_REFOUT_OFF)){
            /* REFOUT ON  */
            data |= 0x80;
        }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0E, data);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_LNA_ENABLE){
        /* LNA Enable */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0x26);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }else{
        /* LNA Disable (Through) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0xA6);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    SONYSAT_SLEEP(3); /* Wait 3ms */

    /* Dummy tune setting */
    {
        uint8_t data[] = {0x10, 0xB4, 0x00, 0x00, 0x80};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x00, data, sizeof(data));
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    /* VCO fixed setting. */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1F, 0x40);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO_CAL_EN = 1 */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0x40);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Start Calibration */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x05, 0x80);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Restore VCO fixed setting. */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1F, 0x80);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Loop filter setting */
    {
        uint8_t data[3];

        data[0] = 0x05;

        switch(pTuner->xtalFreqMHz){
        case 16:
            data[1] = 0xF0; break;
        case 27:
            data[1] = 0xCA; break;
        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_OTHER);
        }

        data[2] = 0xF0;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x34, data, sizeof(data));
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    SONYSAT_SLEEP(10); /* Wait 10ms */

    /* Restore VCO_RSW */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x29, 0x71);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Power save setting */
    result = EnterPowerSave(pTuner);
    if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_RETURN(result); }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

static sonysat_result_t Tune(sonysat_horus3_t *pTuner, uint32_t frequencykHz,
    sonysat_horus3_tv_system_t tvSystem, uint32_t symbolRateksps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t mixdiv = 0;
    uint8_t mdiv = 0;
    uint32_t ms = 0;
    uint8_t f_ctl = 0;
    uint8_t g_ctl = 0;
    uint8_t fc_lpf = 0;

    SONYSAT_TRACE_ENTER("Tune");

    /* IQ Generator disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x79);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* frequency should be X MHz (X : integer) */
    frequencykHz = ((frequencykHz + 500) / 1000) * 1000;

    switch(tvSystem){
    case SONYSAT_HORUS3_STV_ISDBS:
        if(frequencykHz <= 1100000){
            mixdiv = 4;
            mdiv = 1;
        }else{
            mixdiv = 2;
            mdiv = 0;
        }
        break;
    case SONYSAT_HORUS3_STV_DVBS:
    case SONYSAT_HORUS3_STV_DVBS2:
        if(frequencykHz <= 1155000){
            mixdiv = 4;
            mdiv = 1;
        }else{
            mixdiv = 2;
            mdiv = 0;
        }
        break;
    default:
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG); /* Invalid system */
    }

    /* Assumed that fREF == 1MHz (1000kHz) */
    ms = ((frequencykHz * mixdiv)/2 + 1000/2) / 1000; /* Round */
    if(ms > 0x7FFF){ /* 15 bit */
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG); /* Invalid frequency */
    }

    /* Setup RF tracking filter */
    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_LNA_ENABLE){
        if(frequencykHz < 975000){
            /* F_CTL=11100 G_CTL=001 */
            f_ctl = 0x1C; g_ctl = 0x01;
        }else if(frequencykHz < 1050000){
            /* F_CTL=11000 G_CTL=010 */
            f_ctl = 0x18; g_ctl = 0x02;
        }else if(frequencykHz < 1150000){
            /* F_CTL=10100 G_CTL=010 */
            f_ctl = 0x14; g_ctl = 0x02;
        }else if(frequencykHz < 1250000){
            /* F_CTL=10000 G_CTL=011 */
            f_ctl = 0x10; g_ctl = 0x03;
        }else if(frequencykHz < 1350000){
            /* F_CTL=01100 G_CTL=100 */
            f_ctl = 0x0C; g_ctl = 0x04;
        }else if(frequencykHz < 1450000){
            /* F_CTL=01010 G_CTL=100 */
            f_ctl = 0x0A; g_ctl = 0x04;
        }else if(frequencykHz < 1600000){
            /* F_CTL=00111 G_CTL=101 */
            f_ctl = 0x07; g_ctl = 0x05;
        }else if(frequencykHz < 1800000){
            /* F_CTL=00100 G_CTL=110 */
            f_ctl = 0x04; g_ctl = 0x06;
        }else if(frequencykHz < 2000000){
            /* F_CTL=00010 G_CTL=110 */
            f_ctl = 0x02; g_ctl = 0x06;
        }else{
            /* F_CTL=00000 G_CTL=111 */
            f_ctl = 0x00; g_ctl = 0x07;
        }
    }else{
        if(frequencykHz < 975000){
            /* F_CTL=11100 G_CTL=001 */
            f_ctl = 0x1C; g_ctl = 0x01;
        }else if(frequencykHz < 1050000){
            /* F_CTL=11000 G_CTL=010 */
            f_ctl = 0x18; g_ctl = 0x02;
        }else if(frequencykHz < 1150000){
            /* F_CTL=10100 G_CTL=010 */
            f_ctl = 0x14; g_ctl = 0x02;
        }else if(frequencykHz < 1250000){
            /* F_CTL=10000 G_CTL=011 */
            f_ctl = 0x10; g_ctl = 0x03;
        }else if(frequencykHz < 1350000){
            /* F_CTL=01100 G_CTL=100 */
            f_ctl = 0x0C; g_ctl = 0x04;
        }else if(frequencykHz < 1450000){
            /* F_CTL=01010 G_CTL=100 */
            f_ctl = 0x0A; g_ctl = 0x04;
        }else if(frequencykHz < 1600000){
            /* F_CTL=00111 G_CTL=101 */
            f_ctl = 0x07; g_ctl = 0x05;
        }else if(frequencykHz < 1800000){
            /* F_CTL=00100 G_CTL=010 */
            f_ctl = 0x04; g_ctl = 0x02;
        }else if(frequencykHz < 2000000){
            /* F_CTL=00010 G_CTL=001 */
            f_ctl = 0x02; g_ctl = 0x01;
        }else{
            /* F_CTL=00000 G_CTL=000 */
            f_ctl = 0x00; g_ctl = 0x00;
        }
    }

    /* LPF cutoff frequency setting */
    switch(tvSystem){
    case SONYSAT_HORUS3_STV_ISDBS:
        fc_lpf = 22; /* 22MHz */
        break;
    case SONYSAT_HORUS3_STV_DVBS:
        /*
            rolloff = 0.35
            
            SR <= 4.3
              fc_lpf = 5
            4.3 < SR <= 10
              fc_lpf = SR * (1 + rolloff) / 2 + SR / 2 = SR * 1.175 = SR * (47/40)
            10 < SR
              fc_lpf = SR * (1 + rolloff) / 2 + 5 = SR * 0.675 + 5 = SR * (27/40) + 5
            NOTE: The result should be round up.
        */
        if(symbolRateksps <= 4300){
            fc_lpf = 5;
        }else if(symbolRateksps <= 10000){
            fc_lpf = (uint8_t)((symbolRateksps * 47 + (40000-1)) / 40000);
        }else{
            fc_lpf = (uint8_t)((symbolRateksps * 27 + (40000-1)) / 40000 + 5);
        }

        if(fc_lpf > 36){
            fc_lpf = 36; /* 5 <= fc_lpf <= 36 is valid */
        }
        break;
    case SONYSAT_HORUS3_STV_DVBS2:
        /*
            rolloff = 0.2
            
            SR <= 4.5
              fc_lpf = 5
            4.5 < SR <= 10
              fc_lpf = SR * (1 + rolloff) / 2 + SR / 2 = SR * 1.1 = SR * (11/10)
            10 < SR
              fc_lpf = SR * (1 + rolloff) / 2 + 5 = SR * 0.6 + 5 = SR * (3/5) + 5
            NOTE: The result should be round up.
        */
        if(symbolRateksps <= 4500){
            fc_lpf = 5;
        }else if(symbolRateksps <= 10000){
            fc_lpf = (uint8_t)((symbolRateksps * 11 + (10000-1)) / 10000);
        }else{
            fc_lpf = (uint8_t)((symbolRateksps * 3 + (5000-1)) / 5000 + 5);
        }

        if(fc_lpf > 36){
            fc_lpf = 36; /* 5 <= fc_lpf <= 36 is valid */
        }
        break;
    default:
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG); /* Invalid system */
    }

    {
        uint8_t data[5];    /* 0x00 - 0x04 */

        data[0] = (uint8_t)((ms >> 7) & 0xFF);
        data[1] = (uint8_t)((ms & 0x7F) << 1);
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = (uint8_t)(mdiv << 7);
        
        /* Write 0x00 - 0x04 */
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x00, data, sizeof(data));
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    /* Write G_CTL, F_CTL */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x09, (uint8_t)((g_ctl << 5) | f_ctl));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO calibration disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Write LPF cutoff frequency */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x37, (uint8_t)(0x80 | (fc_lpf << 1)));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Start Calibration */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x05, 0x80);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO VT setting */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x24, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x27, 0xE0);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    SONYSAT_SLEEP(1);

    /* Search speedup setting */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x20, 0x0C);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    {
        uint8_t data = 0;

        switch(pTuner->xtalFreqMHz){
        case 16:
            data = 0x01; break;
        case 27:
            data = 0x02; break;
        default:
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_SW_STATE);
        }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x07, data);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x20, 0x1C);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* 27MHz xtal special */
    if(pTuner->xtalFreqMHz == 27){
        uint32_t fvco_reg = 0;
        uint8_t data[2];

        /* Stop VCO target frequency automatic calculation */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x3C, 0x80);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

        /* fVCO(MHz) / 13.5 (round up) = fREF(kHz) * mixdiv / 13500 */
        fvco_reg = ((frequencykHz * mixdiv) + (13500-1)) / 13500;

        if(fvco_reg > 0x1FFF){
            SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG); /* Invalid frequency */
        }

        data[0] = (uint8_t)((fvco_reg >> 5) & 0xFF);
        data[1] = (uint8_t)((fvco_reg << 3) & 0xF8);

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x32, data, sizeof(data));
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    /* Write LPF cutoff frequency (LPF calibration disable) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x37, (uint8_t)(fc_lpf << 1));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO calibration enable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0xC0);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Start Calibration */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x05, 0x80);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Restore VCO VT setting */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x24, 0x40);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x27, 0xA0);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    SONYSAT_SLEEP(10);

    /* IQ Generator enable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x7B);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Restore search speedup setting */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x20, 0x0C);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x07, (uint8_t)(pTuner->xtalFreqMHz));
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x20, 0x1C);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* Store tuned frequency to the struct */
    pTuner->frequencykHz = ms * 2 * 1000 / mixdiv;

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

static sonysat_result_t EnterPowerSave(sonysat_horus3_t *pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("EnterPowerSave");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    /* IQ Generator disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x79);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* MDIV_EN = 0 */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x29, 0x70);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO disable preparation */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x28, 0x3E);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO buffer disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x19);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO calibration disable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_POWERSAVE_ENABLEXTAL){
        /* Power save setting (xtal is not stopped) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x11, 0xC0);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }else{
        /* Power save setting (xtal is stopped) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x11, 0x80);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_LNA_ENABLE){
        if(pTuner->flags & SONYSAT_HORUS3_CONFIG_POWERSAVE_STOPLNA){
            /* LNA is stopped */
            result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0x23);
            if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
        }else{
            /* LNA is not stopped */
            result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0x27);
            if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
        }
    }else{
        /* LNA is Disabled */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0xA7);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}

static sonysat_result_t LeavePowerSave(sonysat_horus3_t *pTuner)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER("LeavePowerSave");

    if((!pTuner) || (!pTuner->pI2c)){
        SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    /* Disable power save */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x11, 0x00);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    if(pTuner->flags & SONYSAT_HORUS3_CONFIG_LNA_ENABLE){
        /* LNA is Enabled */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0x27);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }else{
        /* LNA is Disabled */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x12, 0xA7);
        if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }
    }

    /* VCO buffer enable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x79);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* VCO calibration enable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1C, 0xC0);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* MDIV_EN = 1 */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x29, 0x71);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    /* IQ Generator enable */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x2A, 0x7B);
    if(result != SONYSAT_RESULT_OK) { SONYSAT_TRACE_RETURN(SONYSAT_RESULT_ERROR_I2C); }

    if(!(pTuner->flags & SONYSAT_HORUS3_CONFIG_POWERSAVE_ENABLEXTAL)){
        /* Wait Xtal stable */
        SONYSAT_SLEEP(5);
    }

    SONYSAT_TRACE_RETURN(SONYSAT_RESULT_OK);
}
