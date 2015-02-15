/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sony_demod_dvbc.h"
#include "sony_demod_dvbc_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep T/C to Active for DVB-C
*/
static sony_result_t SLTtoAC(sony_demod_t * pDemod);
/**
@brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_result_t SLTtoAC_BandSetting(sony_demod_t * pDemod);

/**
 @brief Configure the demodulator for use with a 24MHz Xtal
*/
static sony_result_t SLTtoAC_24MHzXtalSetting (sony_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-C to Active to DVB-C.  Used
        for changing channel parameters.
*/
static sony_result_t ACtoAC(sony_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-C to Sleep T/C.
*/
static sony_result_t ACtoSLT(sony_demod_t * pDemod);
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_demod_dvbc_Tune (sony_demod_t * pDemod, sony_dvbc_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_demod_dvbc_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    
    if ((pDemod->state == SONY_DEMOD_STATE_ACTIVE_T_C) && (pDemod->system == SONY_DTV_SYSTEM_DVBC)) {
         pDemod->bandwidth = pTuneParam->bandwidth;
        /* Demodulator Active and set to DVB-C mode */
        result = ACtoAC(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else if((pDemod->state == SONY_DEMOD_STATE_ACTIVE_T_C) && (pDemod->system != SONY_DTV_SYSTEM_DVBC)){
        /* Demodulator Active but not DVB-C mode */
        result = sony_demod_SleepT_C(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        pDemod->system = SONY_DTV_SYSTEM_DVBC;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLTtoAC(pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == SONY_DEMOD_STATE_SLEEP_T_C) {
        /* Demodulator in Sleep T / C mode */
        pDemod->system = SONY_DTV_SYSTEM_DVBC;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLTtoAC (pDemod);
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

sony_result_t sony_demod_dvbc_Sleep (sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_demod_dvbc_Sleep");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP_T_C) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C)) {
        /* This api is accepted in SLEEP_T_C and ACTIVE_T_C states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = ACtoSLT (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_demod_dvbc_CheckDemodLock (sony_demod_t * pDemod, 
                                              sony_demod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t arLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;
    SONY_TRACE_ENTER ("sony_demod_dvbc_CheckDemodLock");

    if ((!pDemod) || (!pLock)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE_T_C state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = sony_demod_dvbc_monitor_SyncStat (pDemod, &arLock, &tsLock, &unlockDetected);

    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pDemod->scanMode) {
        if (unlockDetected == 1) {
           *pLock = SONY_DEMOD_LOCK_RESULT_UNLOCKED;
           SONY_TRACE_RETURN (SONY_RESULT_OK);
        }
    }

    if (arLock == 0) {
        *pLock = SONY_DEMOD_LOCK_RESULT_NOTDETECT;
        SONY_TRACE_RETURN (SONY_RESULT_OK);
    }

    *pLock = tsLock ? SONY_DEMOD_LOCK_RESULT_LOCKED : SONY_DEMOD_LOCK_RESULT_NOTDETECT;

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t SLTtoAC(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAC");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
    result = sony_demod_SetTsClockModeAndFreq (pDemod, SONY_DTV_SYSTEM_DVBC);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x04) != SONY_RESULT_OK) {
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
     * <SLV-T>   10h     D2h    [4:0]     8'h0C      8'h09      OCTL_IFAGC_COARSEGAIN[4:0]
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD2, 0x09, 0x1F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* BBAGC TARGET level setting
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   11h     6Ah    [7:0]     8'h50      8'h48      OREG_ITB_DAGC_TRGT[7:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x48) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if ((pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2D) ||
        (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E) ||
        (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR)||
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

    /* Set SLV-T Bank : 0x40 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Demod setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * --------------------------------------------------------------------------
     * <SLV-T>   40h     C3h    [2]     8'h04      8'h00      OREG_BNDET_EN_64
     */
    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x00, 0x04) != SONY_RESULT_OK) {
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
    
    /* 24MHz Xtal setting */
    if (pDemod->xtalFreq == SONY_DEMOD_XTAL_24000KHz) {
        result = SLTtoAC_24MHzXtalSetting (pDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set tuner and bandwidth specific settings */
    result = SLTtoAC_BandSetting (pDemod);
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

static sony_result_t SLTtoAC_24MHzXtalSetting (sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAC_24MHzXtalSetting");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x40 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   40h     54h    [7:0]     8'h30      8'h29      OREG_BED_THRESHOLD[7:0]
         * <SLV-T>   40h     55h    [7:0]     8'h0A      8'h09      OREG_IF_INC[7:0]
         */
        const uint8_t data[2] = {0x29, 0x09};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x54, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   40h     8Bh    [7:0]     8'h07      8'h08      OREG_AR_AGC_TIMER[7:0]
         * <SLV-T>   40h     8Ch    [7:0]     8'h30      8'h38      OREG_AR_PRE_TIMER[7:0]
         * <SLV-T>   40h     8Dh    [7:0]     8'h70      8'h83      OREG_AR_EQU_TIMER[7:0]
         * <SLV-T>   40h     8Eh    [7:0]     8'h0C      8'h0E      OREG_AR_FEC_TIMER[7:0]
         */
        const uint8_t data[4] = {0x08, 0x38, 0x83, 0x0E};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x8B, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Slave     Bank    Addr   Bit       Default    Value      Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   40h     BFh    [7:0]     8'h52      8'h60      OREG_CKF_FREQ[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBF, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x48 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x48) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   48h     24h    [7:0]     8'h71      8'hDC      OREG_BER_MAXCLKCNT[23:16]
         * <SLV-T>   48h     25h    [7:0]     8'h9C      8'h6C      OREG_BER_MAXCLKCNT[15:8]
         * <SLV-T>   48h     26h    [7:0]     8'h40      8'h00      OREG_BER_MAXCLKCNT[7:0]
         */
        const uint8_t data[3] = {0xDC, 0x6C, 0x00};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x49 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x49) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        /* Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   49h     11h    [7:0]     8'h40      8'h77      OREG_TUNIT[15:8]
         * <SLV-T>   49h     12h    [7:0]     8'h50      8'h00      OREG_TUNIT[7:0]
         */
        const uint8_t data[2] = {0x77, 0x00};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, &data[0], sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    SONY_TRACE_RETURN (result);
}


static sony_result_t SLTtoAC_BandSetting(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("SLTtoAC_BandSetting");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    switch (pDemod->bandwidth) {
    case SONY_DTV_BW_8_MHZ:   
        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E & ASCOT3 tuners
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
                {0x2D,  0xC7,  0x04,  0xF4,  0x07,  0xC5,  0x2A,  0xB8,  0x27,  0x9E,  0x27,  0xA4,  0x29,  0xAB}, /* 20.5MHz XTal */
                {0x29,  0x9A,  0x28,  0x9E,  0x2F,  0xB9,  0x27,  0xA5,  0x28,  0xA9,  0x2B,  0xB0,  0x08,  0xD6}, /* 24MHz XTal   */
                {0x2D,  0xC7,  0x04,  0xF4,  0x07,  0xC5,  0x2A,  0xB8,  0x27,  0x9E,  0x27,  0xA4,  0x29,  0xAB}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x1C,  0xB1,  0x23,  0xB1,  0x09,  0xC7,  0x09,  0xCA,  0x28,  0xCC,  0x1F,  0xAB,  0x27,  0xBC}, /* 20.5MHz XTal */
                {0x2A,  0xBD,  0x1F,  0xAB,  0x26,  0xA8,  0x23,  0xA4,  0x03,  0xDE,  0x01,  0xCE,  0x29,  0xB1}, /* 24MHz XTal   */
                {0x1C,  0xB1,  0x23,  0xB1,  0x09,  0xC7,  0x09,  0xCA,  0x28,  0xCC,  0x1F,  0xAB,  0x27,  0xBC}  /* 41MHz XTal   */
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBC_8 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
  
        /*
         * ==IMR Filter Bandwidth setting
         * slave    Bank    Addr    Bit    default   Value          Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     A3h    [4:0]     8'h00      8'h00      OREG_IMR_FORCE_CHANNEL_WIDTH,1'b0,OREG_IMR_CHANNEL_WIDTH[2:0]
         */
        /* Set SLV-T Bank : 0x11 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA3, 0x00, 0x1F) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        {
            /*  ==The settings of Symbol rate search range 
             *  slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   40h     26h    [3:0]     8'h0B      tsrate1    OREG_TRIAL_SRATE0[11:8]
             *  <SLV-T>   40h     27h    [7:0]     8'h3E      tsrate2    OREG_TRIAL_SRATE0[7:0]
             *  <SLV-T>   40h     28h    [3:0]     8'h01      tsrate3    OREG_TRIAL_SRATE1[11:8]
             *  <SLV-T>   40h     29h    [7:0]     8'h17      tsrate4    OREG_TRIAL_SRATE1[7:0]
             */

            const uint8_t SymRate[3][4] = {
            /*     tsrate1 tsrate2 tsrate3 tsrate4                    */
                {   0x0B,   0x3E,   0x01,   0x17   }, /* 20.5MHz XTal */
                {   0x09,   0x9A,   0x00,   0xEE   }, /* 24MHz XTal   */
                {   0x0B,   0x3E,   0x01,   0x17   }  /* 41MHz XTal   */
            };

            /* Set SLV-T Bank : 0x40 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, &SymRate[pDemod->xtalFreq][0], 4) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        break;

    case SONY_DTV_BW_7_MHZ:
        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E & ASCOT3 tuners
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
                {0x2D,  0xC7,  0x04,  0xF4,  0x07,  0xC5,  0x2A,  0xB8,  0x27,  0x9E,  0x27,  0xA4,  0x29,  0xAB}, /* 20.5MHz XTal */
                {0x29,  0x9A,  0x28,  0x9E,  0x2F,  0xB9,  0x27,  0xA5,  0x28,  0xA9,  0x2B,  0xB0,  0x08,  0xD6}, /* 24MHz XTal   */
                {0x2D,  0xC7,  0x04,  0xF4,  0x07,  0xC5,  0x2A,  0xB8,  0x27,  0x9E,  0x27,  0xA4,  0x29,  0xAB}  /* 41MHz XTal   */
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, &itbCoef[pDemod->xtalFreq][0], 14) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        else if (pDemod->tunerOptimize == SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2XR) {
            const uint8_t itbCoef[3][14] = {
            /*   COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62                 */
                {0x1C,  0xB1,  0x23,  0xB1,  0x09,  0xC7,  0x09,  0xCA,  0x28,  0xCC,  0x1F,  0xAB,  0x27,  0xBC}, /* 20.5MHz XTal */
                {0x2A,  0xBD,  0x1F,  0xAB,  0x26,  0xA8,  0x23,  0xA4,  0x03,  0xDE,  0x01,  0xCE,  0x29,  0xB1}, /* 24MHz XTal   */
                {0x1C,  0xB1,  0x23,  0xB1,  0x09,  0xC7,  0x09,  0xCA,  0x28,  0xCC,  0x1F,  0xAB,  0x27,  0xBC}  /* 41MHz XTal   */
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBC_7 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
  
        /*
         * ==IMR Filter Bandwidth setting
         * slave    Bank    Addr    Bit    default   Value          Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     A3h    [4:0]     8'h00      8'h00      OREG_IMR_FORCE_CHANNEL_WIDTH,1'b0,OREG_IMR_CHANNEL_WIDTH[2:0]
         */
        /* Set SLV-T Bank : 0x11 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA3, 0x00, 0x1F) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        {
            /*  ==The settings of Symbol rate search range 
             *  slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   40h     26h    [3:0]     8'h0B      tsrate1    OREG_TRIAL_SRATE0[11:8]
             *  <SLV-T>   40h     27h    [7:0]     8'h3E      tsrate2    OREG_TRIAL_SRATE0[7:0]
             *  <SLV-T>   40h     28h    [3:0]     8'h01      tsrate3    OREG_TRIAL_SRATE1[11:8]
             *  <SLV-T>   40h     29h    [7:0]     8'h17      tsrate4    OREG_TRIAL_SRATE1[7:0]
             */

            const uint8_t SymRate[3][4] = {
            /*     tsrate1 tsrate2 tsrate3 tsrate4                    */
                {   0x09,   0xD6,   0x01,   0x17   }, /* 20.5MHz XTal */
                {   0x08,   0x66,   0x00,   0xEE   }, /* 24MHz XTal   */
                {   0x09,   0xD6,   0x01,   0x17   }  /* 41MHz XTal   */
            };

            /* Set SLV-T Bank : 0x40 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, &SymRate[pDemod->xtalFreq][0], 4) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
        break;

    case SONY_DTV_BW_6_MHZ:
        /*
         * Group delay equaliser settings for ASCOT2D, ASCOT2E & ASCOT3 tuners
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
            /* T.B.D. */
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBC_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBC_6 & 0xFF);

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /*
         * ==IMR Filter Bandwidth setting
         * slave    Bank    Addr    Bit    default   Value          Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     A3h    [4:0]     8'h00      8'h14      OREG_IMR_FORCE_CHANNEL_WIDTH,1'b0,OREG_IMR_CHANNEL_WIDTH[2:0]
         */
        /* Set SLV-T Bank : 0x11 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA3, 0x14, 0x1F) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        {
            /*  ==The settings of Symbol rate search range 
             *  slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   40h     26h    [3:0]     8'h0B      tsrate1    OREG_TRIAL_SRATE0[11:8]
             *  <SLV-T>   40h     27h    [7:0]     8'h3E      tsrate2    OREG_TRIAL_SRATE0[7:0]
             *  <SLV-T>   40h     28h    [3:0]     8'h01      tsrate3    OREG_TRIAL_SRATE1[11:8]
             *  <SLV-T>   40h     29h    [7:0]     8'h17      tsrate4    OREG_TRIAL_SRATE1[7:0]
             */

            const uint8_t SymRate[3][4] = {
            /*     tsrate1 tsrate2 tsrate3 tsrate4                    */
                {   0x08,   0x6E,   0x01,   0x17   }, /* 20.5MHz XTal */
                {   0x07,   0x33,   0x00,   0xEE   }, /* 24MHz XTal   */
                {   0x08,   0x6E,   0x01,   0x17   }  /* 41MHz XTal   */
            };

            /* Set SLV-T Bank : 0x40 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, &SymRate[pDemod->xtalFreq][0], 4) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        } 
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t ACtoAC(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("ACtoAC");

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
    result = SLTtoAC_BandSetting (pDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }    

    SONY_TRACE_RETURN (result); 
}

static sony_result_t ACtoSLT(sony_demod_t * pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("ACtoSLT");

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

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA3, 0x00, 0x1F) != SONY_RESULT_OK) {
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
