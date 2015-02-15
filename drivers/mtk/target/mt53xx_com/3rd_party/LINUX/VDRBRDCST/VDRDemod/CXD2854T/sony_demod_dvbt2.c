/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#include "sony_demod_dvbt2.h"
#include "sony_demod_dvbt2_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep T/C to Active for DVB-T2
*/
static sony_result_t SLTtoAT2(sony_demod_t * pDemod);

/**
 @brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_result_t SLTtoAT2_BandSetting(sony_demod_t * pDemod);

/**
 @brief Configure the demodulator for use with a 24MHz Xtal
*/
static sony_result_t SLTtoAT2_24MHzXtalSetting (sony_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T2 to Active to DVB-T2.  Used
        for changing channel parameters.
*/
static sony_result_t AT2toAT2(sony_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T2 to Sleep T/C.
*/
static sony_result_t AT2toSLT(sony_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_demod_dvbt2_Tune (sony_demod_t * pDemod,
                                    sony_dvbt2_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_demod_dvbt2_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state == SONY_DEMOD_STATE_ACTIVE_T_C) && (pDemod->system == SONY_DTV_SYSTEM_DVBT2)) {
        /* Demodulator Active and set to DVB-T2 mode */
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = AT2toAT2(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else if((pDemod->state == SONY_DEMOD_STATE_ACTIVE_T_C) && (pDemod->system != SONY_DTV_SYSTEM_DVBT2)){
        /* Demodulator Active but not DVB-T2 mode */
        result = sony_demod_SleepT_C(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        pDemod->system = SONY_DTV_SYSTEM_DVBT2;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLTtoAT2(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONY_DEMOD_STATE_SLEEP_T_C) {
        /* Demodulator in Sleep T / C mode */
        pDemod->system = SONY_DTV_SYSTEM_DVBT2;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLTtoAT2 (pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
    pDemod->state = SONY_DEMOD_STATE_ACTIVE_T_C;

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_Sleep (sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_demod_dvbt2_Sleep");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C)) {
        /* This api is accepted in SLEEP_T_C and ACTIVE_T_C states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = AT2toSLT (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_CheckDemodLock (sony_demod_t * pDemod, 
                                              sony_demod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_demod_dvbt2_CheckDemodLock");

    if (!pDemod || !pLock) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE_T_C state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    *pLock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_demod_dvbt2_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);

    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (unlockDetected) {
        *pLock = SONY_DEMOD_LOCK_RESULT_UNLOCKED;
        SONY_TRACE_RETURN (SONY_RESULT_OK);
    }

    if (sync >= 6) {
        *pLock = SONY_DEMOD_LOCK_RESULT_LOCKED;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_CheckTSLock (sony_demod_t * pDemod, 
                                            sony_demod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_demod_dvbt2_CheckTSLock");

    if (!pDemod || !pLock) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE_T_C state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    *pLock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_demod_dvbt2_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (unlockDetected) {
        *pLock = SONY_DEMOD_LOCK_RESULT_UNLOCKED;
        SONY_TRACE_RETURN (SONY_RESULT_OK);
    }

    if ((sync >= 6) && tslock) {
        *pLock = SONY_DEMOD_LOCK_RESULT_LOCKED;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_SetPLPConfig (sony_demod_t * pDemod, 
                                             uint8_t autoPLP, 
                                             uint8_t plpId)
{
 sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_demod_dvbt2_SetPLPConfig");
    
    if ((!pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C)) {
        /* This api is accepted in SLEEP_T_C and ACTIVE_T_C states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C && pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) {
        /* This api is accepted in ACTIVE or SLEEP states for terrestrial / cable only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }
    
    /* Set SLV-T Bank : 0x23 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x23) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (!autoPLP) {
        /* Manual PLP selection mode. Set the data PLP Id. */
        /* Slave    Bank    Addr    Bit     Default   Value      Name
         * --------------------------------------------------------------------------
         * <SLV-T>   23h     AFh    [7:0]   8'h00     8'h**      OREGD_FP_PLP_ID[7:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAF, plpId) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Auto PLP select (Scanning mode = 0x00). Data PLP select = 0x01. */
    /* Slave    Bank    Addr    Bit     Default   Value      Name
     * --------------------------------------------------------------------------------
     * <SLV-T>   23h     ADh    [1:0]   8'h00     8'h01      OREGD_FP_PLP_AUTO_SEL[1:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAD, autoPLP? 0x00 : 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_SetProfile (sony_demod_t * pDemod, 
                                           sony_dvbt2_profile_t profile)
{
    uint8_t t2Mode_tuneMode = 0;
    uint8_t seqNot2Dtime = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_demod_dvbt2_SetProfile");
    
    if ((!pDemod)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
        
    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C && pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) {
        /* This api is accepted in ACTIVE or SLEEP states for terrestrial / cable only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }
    
    switch (profile) {
    case SONY_DVBT2_PROFILE_BASE:
        t2Mode_tuneMode = 0x01; /* Set profile to DVB-T2 base without recovery */
        seqNot2Dtime = (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz)? 0x0E : 0x0C; /* Set early unlock time */
        break;

    case SONY_DVBT2_PROFILE_LITE:
        t2Mode_tuneMode = 0x05; /* Set profile to DVB-T2 lite without recovery */
        seqNot2Dtime = (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz)? 0x2E : 0x28; /* Set early unlock time */
        break;

    case SONY_DVBT2_PROFILE_ANY:
        t2Mode_tuneMode = 0x00; /* Set profile to auto detection */
        seqNot2Dtime = (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz)? 0x2E : 0x28; /* Set early unlock time */
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x2E */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set profile and tune mode
     * slave    Bank    Addr    Bit       default   Value       Name
     * ---------------------------------------------------------------------------------------
     * <SLV-T>  2Eh     10h     [2:0]     3'b001    3'b001      OREG_T2MODE,OREG_TUNEMODE[1:0]
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, t2Mode_tuneMode, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set early unlock detection time
     * slave    Bank    Addr    Bit      default   Value     Name
     * -------------------------------------------------------------------------------
     * <SLV-T>  2Bh     9Dh     [7:0]    8'd12     8'd12     OREG_SEQ_NOT2_DTIME[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, seqNot2Dtime) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbt2_CheckL1PostValid (sony_demod_t * pDemod, 
                                                 uint8_t * pL1PostValid)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t data;

    SONY_TRACE_ENTER ("sony_demod_dvbt2_CheckL1PostValid");

    if (!pDemod || !pL1PostValid) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C)) {
        /* This api is accepted in SLEEP_T_C and ACTIVE_T_C states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x22 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Slave     Bank    Addr    Bit     Name             Meaning
     * ---------------------------------------------------------------------
     * <SLV-T>   22h     12h     [0]     IL1POST_OK       0:Invalid, 1:Valid
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }    

    *pL1PostValid = data & 0x01;

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t SLTtoAT2(sony_demod_t * pDemod)
{
sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAT2");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
    result = sony_demod_SetTsClockModeAndFreq (pDemod, SONY_DTV_SYSTEM_DVBT2);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x02) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Enable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->enableRfain) {
        /* Enable RFAIN monitor (RF level monitor) clock */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2F, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /* Enable RFAIN monitor (RF level monitor) ADC */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }
    else {
        /* Disable RFAIN monitor (RF level monitor) ADC */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /* Disable RFAIN monitor (RF level monitor) clock */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2F, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Enable ADC clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Enable ADC 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        uint8_t data[2] = {0x00, 0x00};

        /* Enable ADC 2 & 3 */
        if ((pDemod->xtalFreq == SONY_DEMOD_XTAL_20500KHz) || (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz)) {
            data[0] = 0x09;
            data[1] = 0x54;
        }
        else if (pDemod->xtalFreq == SONY_DEMOD_XTAL_41000KHz) {
            data[0] = 0x0A;
            data[1] = 0xD4;
        }
        else {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, &data[0], 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Enable ADC 4 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* IFAGC gain setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   10h     D2h    [4:0]   8'h0C      8'h0C      OCTL_IFAGC_COARSEGAIN[4:0]
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD2, 0x0C, 0x1F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* BBAGC TARGET level setting
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   11h     6Ah    [7:0]     8'h50      8'h50      OREG_ITB_DAGC_TRGT[7:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x50) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) || 
        (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) ||
        (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) || 
        (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
        /* ASCOT setting ON
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }
    else {
        /* ASCOT setting OFF
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Acquisition optimization setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   20h     8Bh    [7:0]   8'h28      8'h3C      OREG_SEQ_SNR_GOODCNT[7:0] (For 2k12 Generation Demod Chips)
     * <SLV-T>   20h     8Bh    [7:0]   8'h3C      8'h3C      OREG_SEQ_SNR_GOODCNT[7:0] (For 2k14 Generation Demod Chips)
     * <SLV-T>   2Bh     76h    [6:4]   8'h00      8'h20      OREG_NOISE_GAIN_ACQ[2:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x8B, 0x3C) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x76, 0x20, 0x70) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x23 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x23) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* L1 Control setting
     * Slave     Bank    Addr   Bit      Default    Value      Name
     * --------------------------------------------------------------------------------------
     * <SLV-T>   23h     E6h    [1:0]    8'h01      8'h00      OREG_FP_CALC_L1SELST_MODE[1:0]
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE6, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* TSIF setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * (For 2k12 Generation Demod Chips)
     * <SLV-T>   00h     CEh     [0]      8'h00      8'h01      ONOPARITY
     * <SLV-T>   00h     CFh     [0]      8'h00      8'h01      ONOPARITY_MANUAL_ON
     * (For 2k14 Generation Demod Chips)
     * <SLV-T>   00h     CEh     [0]      8'h01      8'h01      ONOPARITY
     * <SLV-T>   00h     CFh     [0]      8'h01      8'h01      ONOPARITY_MANUAL_ON
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCE, 0x01, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCF, 0x01, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x13 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* DVB-T2 initial setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   13h     83h    [7:0]     8'h40      8'h10      OREG_ISIC_POSPROTECT[7:0]
     * <SLV-T>   13h     86h    [7:0]     8'h21      8'h34      OREG_ISIC_CFNRMOFFSET
     * <SLV-T>   13h     9Eh    [3:0]     8'h09      8'h09      OREG_CFUPONTHRESHOLDMAX[11:8]
     * <SLV-T>   13h     9Fh    [7:0]     8'hFB      8'hD8      OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>   2Ah     38h    [3:0]     8'h00      8'h04      OREG_CAS_CCIFLT_EN_CW[1:0],OREG_CAS_CCIFLT_EN_CW2[1:0]
     * <SLV-T>   2Bh     11h    [5:0]     8'h00      8'h20      OREG_CAS_CWFLT_ACTCOND[2:0],OREG_CAS_CW2FLT_ACTCOND[2:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, 0x34) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9E, 0x09, 0x0F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, 0xD8) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2A */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, 0x04, 0x0F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, 0x20, 0x3F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* 24MHz Xtal setting */
    if (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz) {
        result = SLTtoAT2_24MHzXtalSetting (pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set tuner and bandwidth specific settings */
    result = SLTtoAT2_BandSetting (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x28) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 2 */
    result = sony_demod_DisableTsDataPinHiZ (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t SLTtoAT2_24MHzXtalSetting (sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAT2_24MHzXtalSetting");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     33h    [7:0]     8'hC8      8'hEB      OCTL_IFAGC_INITWAIT[7:0]
         * <SLV-T>   11h     34h    [7:0]     8'h02      8'h03      OCTL_IFAGC_MINWAIT[7:0]
         * <SLV-T>   11h     35h    [7:0]     8'h32      8'h3B      OCTL_IFAGC_MAXWAIT[7:0]
         */
        const uint8_t data[3] = {0xEB, 0x03, 0x3B};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   20h     95h    [7:0]     8'h50      8'h5E      OREG_SEQ_L1PRE_WTIME[15:8]
         * <SLV-T>   20h     96h    [7:0]     8'h50      8'h5E      OREG_SEQ_L1POST_WTIME[15:8]
         * <SLV-T>   20h     97h    [7:0]     8'h3C      8'h47      OREG_SEQ_DMDOK_WTIME[15:8]
         */
        const uint8_t data[3] = {0x5E, 0x5E, 0x47};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x95, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   20h     99h    [7:0]     8'h14      8'h18      OREG_SEQ_RESET_WTIME[7:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x99, 0x18) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   20h     D9h    [5:0]     8'h08      8'h3F      OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>   20h     DAh    [7:0]     8'h00      8'hFF      OREG_CDRB_GTDOFST[7:0]
         */
        const uint8_t data[2] = {0x3F, 0xFF};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x24 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   24h     34h    [5:0]     8'h09      8'h0B      OREG_PBER_MESB[13:8]
         * <SLV-T>   24h     35h    [7:0]     8'hC7      8'h72      OREG_PBER_MESB[7:0]
         */
        const uint8_t data[2] = {0x0B, 0x72};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x34, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   24h     D2h    [7:0]     8'h7E      8'h93      OREG_SP_TSLK_TIMNOPST[23:16]
         * <SLV-T>   24h     D3h    [7:0]     8'h5F      8'hF3      OREG_SP_TSLK_TIMNOPST[15:8]
         * <SLV-T>   24h     D4h    [7:0]     8'h90      8'h00      OREG_SP_TSLK_TIMNOPST[7:0]
         */
        const uint8_t data[3] = {0x93, 0xF3, 0x00};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD2, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   24h     DDh    [3:0]     8'h04      8'h05      OREG_SP_BERT_MAXCLKCNT[27:24]
         * <SLV-T>   24h     DEh    [7:0]     8'hE3      8'hB8      OREG_SP_BERT_MAXCLKCNT[23:16]
         * <SLV-T>   24h     DFh    [7:0]     8'h38      8'hD8      OREG_SP_BERT_MAXCLKCNT[15:8]
         */
        const uint8_t data[3] = {0x05, 0xB8, 0xD8};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDD, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   24h     E0h    [7:0]     8'h80      8'h00      OREG_SP_BERT_MAXCLKCNT[7:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE0, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x25 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x25) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   25h     EDh    [7:0]     8'h52      8'h60      OREG_CKF_FREQ[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xED, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }
    
    /* Set SLV-T Bank : 0x27 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   27h     FAh    [6:0]     8'h2C      8'h34      OREG_T2FS_TIMEOUT_WTIME[14:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFA, 0x34) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   2Bh     4Bh    [7:0]     8'h28      8'h2F      OREG_SEQ_P1DET_WTIME[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x2F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   2Bh     9Eh    [7:0]     8'h0C      8'h0E      OREG_SEQ_NOT2_DINTERVAL[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9E, 0x0E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   2Dh     24h    [7:0]     8'h75      8'h89      OREG_LDPCD_PLP_MIN_OUTTIME_N[7:0]
         * <SLV-T>   2Dh     25h    [7:0]     8'h75      8'h89      OREG_LDPCD_PLP_MIN_OUTTIME_S[7:0]
         */
        const uint8_t data[2] = {0x89, 0x89};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x5E */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x5E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {    
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   5Eh     8Ch    [7:0]     8'h1F      8'h24      OREG_FP_RATE_CTRL_MARGIN[15:8]
         * <SLV-T>   5Eh     8Dh    [7:0]     8'h40      8'h95      OREG_FP_RATE_CTRL_MARGIN[7:0]
         */
        const uint8_t data[2] = {0x24, 0x95};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x8C, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t SLTtoAT2_BandSetting(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAT2_BandSetting");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    switch (pDemod->bandwidth) {
    case SONY_DTV_BW_8_MHZ:
        {
            const uint8_t nominalRate[3][5] = {
                /* TRCG Nominal Rate [37:0] */
                {0x11, 0xF0, 0x00, 0x00, 0x00}, /* 20.5MHz XTal */
                {0x15, 0x00, 0x00, 0x00, 0x00}, /* 24MHz XTal */
                {0x11, 0xF0, 0x00, 0x00, 0x00}  /* 41MHz XTal */
            };
            /* <Timing Recovery setting>   
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h11     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'hF0     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0     
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, &nominalRate[pDemod->xtalFreq][0], 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00, 0x0F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E and ASCOT3 tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) || (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) || 
            (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x26,  0xAF,  0x06,  0xCD,  0x13,  0xBB,  0x28,  0xBA,  0x23,  0xA9,  0x1F,  0xA8,  0x2C,  0xC8}, /* 20.5MHz XTal */
                {0x2F,  0xBA,  0x28,  0x9B,  0x28,  0x9D,  0x28,  0xA1,  0x29,  0xA5,  0x2A,  0xAC,  0x29,  0xB5}, /* 24MHz XTal   */
                {0x26,  0xAF,  0x06,  0xCD,  0x13,  0xBB,  0x28,  0xBA,  0x23,  0xA9,  0x1F,  0xA8,  0x2C,  0xC8}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x1E,  0x1D,  0x29,  0xC9,  0x2A,  0xBA,  0x29,  0xAD,  0x29,  0xA4,  0x29,  0x9A,  0x28,  0x9E}, /* 20.5MHz XTal */
                {0x28,  0x9B,  0x2E,  0x98,  0x2C,  0x97,  0x30,  0x9C,  0x30,  0xA4,  0x32,  0xAC,  0x33,  0xB8}, /* 24MHz XTal   */
                {0x1E,  0x1D,  0x29,  0xC9,  0x2A,  0xBA,  0x29,  0xAD,  0x29,  0xA4,  0x29,  0x9A,  0x28,  0x9E}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_8 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h00      OREG_CHANNEL_WIDTH[2:0]
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00, 0x07) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
        break;

    case SONY_DTV_BW_7_MHZ:
        {
            const uint8_t nominalRate[3][5] = {
                /* TRCG Nominal Rate [37:0] */
                {0x14, 0x80, 0x00, 0x00, 0x00}, /* 20.5MHz XTal */
                {0x18, 0x00, 0x00, 0x00, 0x00}, /* 24MHz XTal */
                {0x14, 0x80, 0x00, 0x00, 0x00}  /* 41MHz XTal */
            };
            /* <Timing Recovery setting>   
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h11     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'hF0     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0     
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, &nominalRate[pDemod->xtalFreq][0], 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00, 0x0F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E and ASCOT3 tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) || (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) || 
            (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x2C,  0xBD,  0x02,  0xCF,  0x04,  0xF8,  0x23,  0xA6,  0x29,  0xB0,  0x26,  0xA9,  0x21,  0xA5}, /* 20.5MHz XTal */
                {0x30,  0xB1,  0x29,  0x9A,  0x28,  0x9C,  0x28,  0xA0,  0x29,  0xA2,  0x2B,  0xA6,  0x2B,  0xAD}, /* 24MHz XTal   */
                {0x2C,  0xBD,  0x02,  0xCF,  0x04,  0xF8,  0x23,  0xA6,  0x29,  0xB0,  0x26,  0xA9,  0x21,  0xA5}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x1B,  0x22,  0x2B,  0xC1,  0x2C,  0xB3,  0x2B,  0xA9,  0x2B,  0xA0,  0x2B,  0x97,  0x2B,  0x9B}, /* 20.5MHz XTal */
                {0x20,  0xA6,  0x2F,  0x9A,  0x28,  0x9B,  0x30,  0x9F,  0x2D,  0x97,  0x32,  0xA6,  0x33,  0xB0}, /* 24MHz XTal   */
                {0x1B,  0x22,  0x2B,  0xC1,  0x2C,  0xB3,  0x2B,  0xA9,  0x2B,  0xA0,  0x2B,  0x97,  0x2B,  0x9B}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_7 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h02      OREG_CHANNEL_WIDTH[2:0]
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x02, 0x07) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
        break;

    case SONY_DTV_BW_6_MHZ:
        {
            const uint8_t nominalRate[3][5] = {
                /* TRCG Nominal Rate [37:0] */
                {0x17, 0xEA, 0xAA, 0xAA, 0xAA}, /* 20.5MHz XTal */
                {0x1C, 0x00, 0x00, 0x00, 0x00}, /* 24MHz XTal */
                {0x17, 0xEA, 0xAA, 0xAA, 0xAA}  /* 41MHz XTal */
            };
            /* <Timing Recovery setting>   
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h11     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'hF0     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0     
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, &nominalRate[pDemod->xtalFreq][0], 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00, 0x0F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E and ASCOT3 tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) || (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) || 
            (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x27,  0xA7,  0x28,  0xB3,  0x02,  0xF0,  0x01,  0xE8,  0x00,  0xCF,  0x00,  0xE6,  0x23,  0xA4}, /* 20.5MHz XTal */
                {0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8}, /* 24MHz XTal   */
                {0x27,  0xA7,  0x28,  0xB3,  0x02,  0xF0,  0x01,  0xE8,  0x00,  0xCF,  0x00,  0xE6,  0x23,  0xA4}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x19,  0x24,  0x2B,  0xB7,  0x2C,  0xAC,  0x29,  0xA6,  0x2A,  0x9F,  0x2A,  0x99,  0x2A,  0x9B}, /* 20.5MHz XTal */
                {0x2C,  0x99,  0x30,  0x97,  0x31,  0x9C,  0x2A,  0x99,  0x2E,  0x94,  0x33,  0xA0,  0x34,  0xA8}, /* 24MHz XTal   */
                {0x19,  0x24,  0x2B,  0xB7,  0x2C,  0xAC,  0x29,  0xA6,  0x2A,  0x9F,  0x2A,  0x99,  0x2A,  0x9B}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_6 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]     8'h00      8'h04      OREG_CHANNEL_WIDTH[2:0]
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x04, 0x07) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
        break;

    case SONY_DTV_BW_5_MHZ:
        {
            const uint8_t nominalRate[3][5] = {
                /* TRCG Nominal Rate [37:0] */
                {0x1C, 0xB3, 0x33, 0x33, 0x33}, /* 20.5MHz XTal */
                {0x21, 0x99, 0x99, 0x99, 0x99}, /* 24MHz XTal */
                {0x1C, 0xB3, 0x33, 0x33, 0x33}  /* 41MHz XTal */
            };
            /* <Timing Recovery setting>   
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h11     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'hF0     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0     
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, &nominalRate[pDemod->xtalFreq][0], 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00, 0x0F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E and ASCOT3 tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) || (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) || 
            (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x27,  0xA7,  0x28,  0xB3,  0x02,  0xF0,  0x01,  0xE8,  0x00,  0xCF,  0x00,  0xE6,  0x23,  0xA4}, /* 20.5MHz XTal */
                {0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8}, /* 24MHz XTal   */
                {0x27,  0xA7,  0x28,  0xB3,  0x02,  0xF0,  0x01,  0xE8,  0x00,  0xCF,  0x00,  0xE6,  0x23,  0xA4}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x19,  0x24,  0x2B,  0xB7,  0x2C,  0xAC,  0x29,  0xA6,  0x2A,  0x9F,  0x2A,  0x99,  0x2A,  0x9B}, /* 20.5MHz XTal */
                {0x2C,  0x99,  0x30,  0x97,  0x31,  0x9C,  0x2A,  0x99,  0x2E,  0x94,  0x33,  0xA0,  0x34,  0xA8}, /* 24MHz XTal   */
                {0x19,  0x24,  0x2B,  0xB7,  0x2C,  0xAC,  0x29,  0xA6,  0x2A,  0x9F,  0x2A,  0x99,  0x2A,  0x9B}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_5 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_5 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_5 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h06      OREG_CHANNEL_WIDTH[2:0]
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x06, 0x07) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        break;

    case SONY_DTV_BW_1_7_MHZ:
        {
            const uint8_t nominalRate[3][5] = {
                /* TRCG Nominal Rate [37:0] */
                {0x58, 0xE2, 0xAF, 0xE0, 0xBC}, /* 20.5MHz XTal */
                {0x68, 0x0F, 0xA2, 0x32, 0xD0}, /* 24MHz XTal */
                {0x58, 0xE2, 0xAF, 0xE0, 0xBC}  /* 41MHz XTal */
            };
            /* <Timing Recovery setting>   
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h11     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'hF0     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h03   OREG_TRCG_LMTVAL0     
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, &nominalRate[pDemod->xtalFreq][0], 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x03, 0x0F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E and ASCOT3 tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) || (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT3)) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x25,  0xA0,  0x36,  0x8D,  0x2E,  0x94,  0x28,  0x9B,  0x32,  0x90,  0x2C,  0x9D,  0x29,  0x99}, /* 20.5MHz XTal */
                {0x33,  0x8E,  0x2B,  0x97,  0x2D,  0x95,  0x37,  0x8B,  0x30,  0x97,  0x2D,  0x9A,  0x21,  0xA4}, /* 24MHz XTal   */
                {0x25,  0xA0,  0x36,  0x8D,  0x2E,  0x94,  0x28,  0x9B,  0x32,  0x90,  0x2C,  0x9D,  0x29,  0x99}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_1_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_1_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_1_7 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h03      OREG_CHANNEL_WIDTH[2:0]
         */
        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x03, 0x07) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }
    SONY_TRACE_RETURN (result);
}


static sony_result_t AT2toAT2(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("AT2toAT2");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLTtoAT2_BandSetting (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }    

    SONY_TRACE_RETURN (result); 
}

static sony_result_t AT2toSLT(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("AT2toSLT");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x3F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, 0xFF) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x13 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Cancel DVB-T2 setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   13h     83h    [7:0]     8'h40      8'h40      OREG_ISIC_POSPROTECT[7:0]
     * <SLV-T>   13h     86h    [7:0]     8'h21      8'h21      OREG_ISIC_CFNRMOFFSET[7:0]
     * <SLV-T>   13h     9Eh    [3:0]     8'h09      8'h09      OREG_CFUPONTHRESHOLDMAX[11:8]
     * <SLV-T>   13h     9Fh    [7:0]     8'hFB      8'hFB      OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>   2Ah     38h    [3:0]     8'h00      8'h00      OREG_CAS_CCIFLT_EN_CW[1:0],OREG_CAS_CCIFLT_EN_CW2[1:0]
     * <SLV-T>   2Bh     11h    [5:0]     8'h00      8'h00      OREG_CAS_CWFLT_ACTCOND[2:0],OREG_CAS_CW2FLT_ACTCOND[2:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, 0x40) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, 0x21) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9E, 0x09, 0x0F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, 0xFB) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2A */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, 0x00, 0x0F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, 0x00, 0x3F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable ADC 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable ADC 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x43, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable ADC 3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x41, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable ADC clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (pDemod->enableRfain) {
        /* Disable RFAIN monitor (RF level monitor) ADC */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /* Disable RFAIN monitor (RF level monitor) clock */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2F, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Disable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (result);
}
