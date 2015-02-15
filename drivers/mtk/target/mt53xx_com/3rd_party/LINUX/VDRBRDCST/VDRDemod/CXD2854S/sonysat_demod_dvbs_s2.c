/*------------------------------------------------------------------------------
  Copyright 2013 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_dtv.h"
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
static sonysat_result_t SLStoASX (sonysat_demod_t * pDemod,
                               sonysat_dtv_system_t dtvSystem,
                               uint32_t symbolRateKSps);

static sonysat_result_t ASXtoSLS (sonysat_demod_t * pDemod);

static sonysat_result_t ASXtoASX (sonysat_demod_t * pDemod,
                               uint32_t symbolRateKSps);

static sonysat_result_t TSRStoSLS(sonysat_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sonysat_result_t sonysat_demod_dvbs_s2_Tune (sonysat_demod_t * pDemod,
                                       sonysat_dvbs_s2_tune_param_t * pTuneParam)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pTuneParam->system != SONYSAT_DTV_SYSTEM_DVBS) &&
        (pTuneParam->system != SONYSAT_DTV_SYSTEM_DVBS2) &&
        (pTuneParam->system != SONYSAT_DTV_SYSTEM_ANY)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_S){
        /* Active */
        if (pDemod->dvbss2ScanMode > 0){
            /* Scan -> Sleep-S */
            result = TSRStoSLS (pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* Sleep-S -> Active-S(X) */
            result = SLStoASX (pDemod, pTuneParam->system, pTuneParam->symbolRateKSps);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else if (pDemod->system == pTuneParam->system){
            /* Active-S(X) -> Active-S(X) */
            result = ASXtoASX (pDemod, pTuneParam->symbolRateKSps);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            /* Active-S(X) -> Sleep-S */
            result = ASXtoSLS (pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
            /* Sleep-S -> Active-S(X) */
            result = SLStoASX (pDemod, pTuneParam->system, pTuneParam->symbolRateKSps);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
    } else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S){
        /* Sleep-S -> Active-S(X) */
        result = SLStoASX (pDemod, pTuneParam->system, pTuneParam->symbolRateKSps);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (result);
        }
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pDemod->state = SONYSAT_DEMOD_STATE_ACTIVE_S;
    pDemod->system = pTuneParam->system;
    pDemod->dvbss2ScanMode = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_Sleep (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_Sleep");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state == SONYSAT_DEMOD_STATE_ACTIVE_S){
        if (pDemod->dvbss2ScanMode > 0){
            result = TSRStoSLS (pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        } else {
            /* Active-S(X) -> Sleep-S */
            result = ASXtoSLS (pDemod);
            if (result != SONYSAT_RESULT_OK){
                SONYSAT_TRACE_RETURN (result);
            }
        }
        pDemod->state = SONYSAT_DEMOD_STATE_SLEEP_S;
    } else if (pDemod->state == SONYSAT_DEMOD_STATE_SLEEP_S){
        /* Do nothing. */
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    pDemod->dvbss2ScanMode = 0;

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_CheckTSLock (sonysat_demod_t * pDemod,
                                              sonysat_demod_lock_result_t * pLock)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t isLock = 0;

    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_CheckTSLock");

    if ((!pDemod) || (!pLock)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) {
        /* This api is accepted in ACTIVE_S state only */
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = sonysat_demod_dvbs_s2_monitor_SyncStat(pDemod, &isLock);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    if(isLock){
        *pLock = SONYSAT_DEMOD_LOCK_RESULT_LOCKED;
    } else {
        *pLock = SONYSAT_DEMOD_LOCK_RESULT_NOTDETECT;
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_CheckIQInvert (sonysat_demod_t * pDemod,
                                                uint8_t * pIsInvert)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_CheckIQInvert");

    if ((!pDemod) || (!pIsInvert)) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    *pIsInvert = 0;
    if (pDemod->isSinglecable){
        if (pDemod->isSinglecableIqInv){
            if (pDemod->satTunerIqSense == SONYSAT_DEMOD_SAT_IQ_SENSE_NORMAL){
                *pIsInvert = 1;
            }
        } else {
            if (pDemod->satTunerIqSense == SONYSAT_DEMOD_SAT_IQ_SENSE_INV){
                *pIsInvert = 1;
            }
        }
    } else {
        if (pDemod->satTunerIqSense == SONYSAT_DEMOD_SAT_IQ_SENSE_INV){
            *pIsInvert = 1;
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_SetSymbolRate (sonysat_demod_t * pDemod,
                                                uint32_t symbolRateKSps)
{
    uint32_t regValue = 0;
    uint8_t data[3] = {0, 0, 0};
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_SetSymbolRate");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONYSAT_DEMOD_STATE_ACTIVE_S) &&
        (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /*
     * regValue = (symbolRateKSps * 2^14 / 1000) + 0.5
     *          = ((symbolRateKSps * 2^14) + 500) / 1000
     *          = ((symbolRateKSps * 16384) + 500) / 1000
     */
    regValue = ((symbolRateKSps * 16384) + 500) / 1000;
    if ((regValue == 0) || (regValue > 0x0FFFFF)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
    }
    data[0] = (uint8_t)((regValue >> 16) & 0x0F);
    data[1] = (uint8_t)((regValue >>  8) & 0xFF);
    data[2] = (uint8_t)( regValue        & 0xFF);

    /*  slave     Bank    Addr    Bit     Default    Setting       Signal name
     * ----------------------------------------------------------------------------------
     * <SLV-T>    AEh     20h     [3:0]    8'h07      8'h0x        OSYMBOLRATE[19:16]
     * <SLV-T>    AEh     21h     [7:0]    8'h37      8'hxx        OSYMBOLRATE[15:8]
     * <SLV-T>    AEh     22h     [7:0]    8'h0A      8'hxx        OSYMBOLRATE[7:0]
     */
    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x20, data, 3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sonysat_result_t SLStoASX (sonysat_demod_t * pDemod,
                               sonysat_dtv_system_t dtvSystem,
                               uint32_t symbolRateKSps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t data[4] = {0, 0, 0, 0};

    SONYSAT_TRACE_ENTER ("SLStoASX");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency. */
    /* NOTE: It is used for both DVB-S and S2 systems though only DVB-S is passed as a parameter. */
    result = sonysat_demod_SetTsClockModeAndFreq (pDemod, SONYSAT_DTV_SYSTEM_DVBS);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set demod mode */
    if(dtvSystem == SONYSAT_DTV_SYSTEM_DVBS){
        data[0] = 0x0A;
    } else if(dtvSystem == SONYSAT_DTV_SYSTEM_DVBS2){
        data[0] = 0x0B;
    } else if(dtvSystem == SONYSAT_DTV_SYSTEM_ANY){
        data[0] = 0x0B;
    } else {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }
    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, data[0]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if ((dtvSystem == SONYSAT_DTV_SYSTEM_DVBS) || (dtvSystem == SONYSAT_DTV_SYSTEM_DVBS2)){
        /* DVB-S/S2 */
        data[0] = 0x00;
    } else {
        /* ANY */
        data[0] = 0x01;
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable S/S2 auto detection 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2D, data[0]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable S/S2 auto detection 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, data[0]) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable ADC clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x31, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable ADC 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable ADC 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x3F) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Enable ADC 3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit    Default     Value         Signal name
     * -------------------------------------------------------------------------------
     * <SLV-T>    A3h     ACh     [0]     1'b1        1'b0         OTRL_CKFERRSEL
     * <SLV-T>    ABh     98h     [3:0]   8'h01       8'h07        OCFRL_FMAX_HS[11:8]
     * <SLV-T>    ABh     99h     [7:0]   8'hF9       8'h3B        OCFRL_FMAX_HS[7:0]
     * <SLV-T>    ABh     9Ah     [3:0]   8'h0E       8'h08        OCFRL_FMIN_HS[11:8]
     * <SLV-T>    ABh     9Bh     [7:0]   8'h07       8'hC5        OCFRL_FMIN_HS[7:0]
     * <SLV-T>    ABh     A8h     [3:0]   8'h02       8'h05        OCFRL_FMAX[11:8]
     * <SLV-T>    ABh     A9h     [7:0]   8'hF5       8'h80        OCFRL_FMAX[7:0]
     * <SLV-T>    ABh     AAh     [3:0]   8'h0D       8'h0A        OCFRL_FMIN[11:8]
     * <SLV-T>    ABh     ABh     [7:0]   8'h0B       8'h80        OCFRL_FMIN[7:0]
     * <SLV-T>    ABh     C3h     [7:0]   6'h08       8'h0C        OCFRL_FSTEP_HARD_ARS1[13:8]
     * <SLV-T>    ABh     C4h     [7:0]   8'h3D       8'hCC        OCFRL_FSTEP_HARD_ARS1[7:0]
     */

    /* Set SLV-T Bank : 0xA3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAC, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    data[0] = 0x07;
    data[1] = 0x3B;
    data[2] = 0x08;
    data[3] = 0xC5;
    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x98, data, 4) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    data[0] = 0x05;
    data[1] = 0x80;
    data[2] = 0x0A;
    data[3] = 0x80;
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA8, data, 4) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    data[0] = 0x0C;
    data[1] = 0xCC;
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, data, 2) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /*   slave     Bank    Addr    Bit    Default     Setting       Signal name
     *  --------------------------------------------------------------------------------
     *   <SLV-T  >  A0h     D7h     [0]      8'h00      8'h00      OTUIF_POLAR
     *  
     */
    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OTUIF_POLAR[0] */
    if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set demod parameter */
    result = sonysat_demod_dvbs_s2_SetSymbolRate (pDemod, symbolRateKSps);
    if (result != SONYSAT_RESULT_OK){SONYSAT_TRACE_RETURN (result);}

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable Hi-Z setting 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x10) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable Hi-Z setting 2 */
    result = sonysat_demod_DisableTsDataPinHiZ (pDemod);
    if (result != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t ASXtoSLS (sonysat_demod_t * pDemod)
{
    SONYSAT_TRACE_ENTER ("ASXtoSLS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* enable Hi-Z setting 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* enable Hi-Z setting 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, 0xFF) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x31, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* SADC Bias ON */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable S/S2 auto detection1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable S/S2 auto detection2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2D, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t ASXtoASX (sonysat_demod_t * pDemod, uint32_t symbolRateKSps)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("ASXtoASX");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* set demod parameter */
    result = sonysat_demod_dvbs_s2_SetSymbolRate(pDemod, symbolRateKSps);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

static sonysat_result_t TSRStoSLS(sonysat_demod_t * pDemod)
{
    SONYSAT_TRACE_ENTER ("TSRStoSLS");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OTSDTDISABLE[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_ARC_HSMODE[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_SRSON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_UNLOCKSEL_ARS[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2E, 0x06) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_AUTOMODECHGON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_ARC_CKACHGAUTO[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2B, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* enable Hi-Z setting 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* enable Hi-Z setting 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, 0xFF) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 1 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x31, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 2 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x63, 0x16) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* disable ADC 3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x65, 0x27) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_VINCM_SEL_STB[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x06) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_CK_EN[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_ARC_AUTOMODECHGON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2D, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OMODE[1:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x03) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSBUFOFF[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF9, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0xA3 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBD, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OTUIF_POLAR[0] */
    if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}
