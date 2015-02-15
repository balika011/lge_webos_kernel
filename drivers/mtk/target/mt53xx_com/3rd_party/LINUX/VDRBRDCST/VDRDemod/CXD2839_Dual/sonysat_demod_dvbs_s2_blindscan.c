/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/
#include "sonysat_common.h"
#include "sonysat_demod_dvbs_s2.h"
#include "sonysat_demod_dvbs_s2_blindscan.h"

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_Initialize(sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    sonysat_dvbs_s2_tune_param_t tuneParam;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_Initialize");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    /* State check */
    if (pDemod->state != SONYSAT_DEMOD_STATE_SLEEP_S){
        result = sonysat_demod_dvbs_s2_Sleep (pDemod);
        if (result != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN(result);
        }
    }

    tuneParam.centerFreqKHz = 1000000;
    tuneParam.symbolRateKSps = 20000;
    tuneParam.system = SONYSAT_DTV_SYSTEM_ANY;

    result = sonysat_demod_dvbs_s2_Tune (pDemod, &tuneParam);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN(result);
    }

    pDemod->dvbss2ScanMode = 0x01;

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_ARC_CKACHGAUTO[0]
     * LS/HS auto selection : OFF
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2B, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_SRSON[0]
     * SR search : ON
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_UNLOCKSEL_ARS[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2E, 0x05) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OMODE[1:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_PS_INIT (sonysat_demod_t * pDemod)
{
    uint8_t iqInv = 0;
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_PS_INIT");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_SW_STATE);
    }

    result = sonysat_demod_dvbs_s2_blindscan_SetSampleMode (pDemod, 1);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSK[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x57, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSBUFOFF[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF9, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = sonysat_demod_dvbs_s2_CheckIQInvert (pDemod, &iqInv);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN(result);
    }

    if (iqInv){
        /* Set SLV-T Bank : 0xA0 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* OTUIF_POLAR[0] */
        if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x01, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_PS_SET (sonysat_demod_t * pDemod,
                                                   int32_t lowerOffsetKHz,
                                                   int32_t upperOffsetKHz,
                                                   uint32_t stepKHz,
                                                   uint32_t ckaFreqKHz)
{
    uint8_t data[6];
    int32_t startFreqKHz = 0;
    int32_t endFreqKHz = 0;
    uint32_t stepFreqKHz = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_PS_SET");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (ckaFreqKHz == 0){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    startFreqKHz =  (lowerOffsetKHz * 2048)                              / (int32_t)ckaFreqKHz; /* Floor */
    endFreqKHz   = ((upperOffsetKHz * 2048) + ((int32_t)ckaFreqKHz - 1)) / (int32_t)ckaFreqKHz; /* Ceil  */
    stepFreqKHz  = ((stepKHz        * 2048) + (         ckaFreqKHz - 1)) /          ckaFreqKHz; /* Ceil  */

    data[0] = (uint8_t)(((uint32_t)startFreqKHz >> 8) & 0x07);
    data[1] = (uint8_t)(((uint32_t)startFreqKHz     ) & 0xFF);
    data[2] = (uint8_t)(((uint32_t)endFreqKHz   >> 8) & 0x07);
    data[3] = (uint8_t)(((uint32_t)endFreqKHz       ) & 0xFF);
    data[4] = (uint8_t)((          stepFreqKHz  >> 8) & 0x03);
    data[5] = (uint8_t)((          stepFreqKHz      ) & 0xFF);

    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_FSTART_CS[10:0]
     * OCFRL_FEND_CS[10:0]
     * OCFRL_FSTEP_CS[9:0]
     */
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF1, data, 6) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_PS_START (sonysat_demod_t * pDemod)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_PS_START");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANST[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x54, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_PS_RACK (sonysat_demod_t * pDemod)
{
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_PS_RACK");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANRACK[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x55, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_PS_FIN (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t iqInv = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_PS_FIN");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    result = sonysat_demod_dvbs_s2_blindscan_SetSampleMode (pDemod, 0);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSK[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x57, 0x02) != SONYSAT_RESULT_OK){
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

    result = sonysat_demod_dvbs_s2_CheckIQInvert (pDemod, &iqInv);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN(result);
    }

    if (iqInv){
        /* Set SLV-T Bank : 0xA0 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* OTUIF_POLAR[0] */
        if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_CS_INIT (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t iqInv = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_CS_INIT");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSK[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x57, 0x01) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = sonysat_demod_dvbs_s2_CheckIQInvert (pDemod, &iqInv);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN(result);
    }

    if (iqInv){
        /* Set SLV-T Bank : 0xA0 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* OTUIF_POLAR[0] */
        if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x01, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_CS_FIN (sonysat_demod_t * pDemod)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t iqInv = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_CS_FIN");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CHSCANON[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CSK[2:0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x57, 0x02) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    result = sonysat_demod_dvbs_s2_CheckIQInvert (pDemod, &iqInv);
    if (result != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN(result);
    }

    if (iqInv){
        /* Set SLV-T Bank : 0xA0 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
        /* OTUIF_POLAR[0] */
        if (sonysat_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00, 0x01) != SONYSAT_RESULT_OK){
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_GetCSFIN (sonysat_demod_t * pDemod,
                                                     uint8_t * pCSFIN)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_GetCSFIN");

    if ((!pDemod) || (!pCSFIN)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* ICFRL_CSFIN[0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data & 0x01){
        *pCSFIN = 1;
    } else {
        *pCSFIN = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_GetCSRDEND (sonysat_demod_t * pDemod,
                                                       uint8_t * pCSRDEND)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_GetCSRDEND");

    if ((!pDemod) || (!pCSRDEND)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* ICFRL_CSRDEND[0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x44, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data & 0x01){
        *pCSRDEND = 1;
    } else {
        *pCSRDEND = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_GetCSINFO (sonysat_demod_t * pDemod,
                                                      int32_t * pCSFREQ,
                                                      uint32_t * pCSPOW)
{
    uint8_t data[4];
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_GetCSINFO");

    if ((!pDemod) || (!pCSFREQ) || (!pCSPOW)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xAB */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAB) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* ICFRL_CSFREQ[10:0]
     * ICFRL_CSPOW[11:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, data, 4) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pCSFREQ = sonysat_Convert2SComplement(((uint32_t)(data[0] & 0x07) << 8) | (uint32_t)(data[1] & 0xFF), 11);
    *pCSPOW = ((uint32_t)(data[2] & 0x0F) << 8) | (uint32_t)(data[3] & 0xFF);

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_GetSRSFIN (sonysat_demod_t * pDemod,
                                                      uint8_t * pSRSFIN)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_GetSRSFIN");

    if ((!pDemod) || (!pSRSFIN)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* IFSM_SRSFIN_ARS[0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data & 0x01){
        *pSRSFIN = 1;
    } else {
        *pSRSFIN = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_CheckTRLLock (sonysat_demod_t * pDemod,
                                                         uint8_t * pIsTRLLock)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_CheckTRLLock");

    if ((!pDemod) || (!pIsTRLLock)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* ITRL_LOCK[0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    if (data & 0x01){
        *pIsTRLLock = 1;
    } else {
        *pIsTRLLock = 0;
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_GetPSPow (sonysat_demod_t * pDemod,
                                                     uint16_t * pPower)
{
    uint8_t data[2] = {0, 0};
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_GetPSPow");

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* ICFRL_PSPOW[11:0] */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, data, 2) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    *pPower = (uint16_t)(((uint32_t)(data[0] & 0x0F) << 8) | (uint32_t)(data[1] & 0xFF));

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_SetCFFine (sonysat_demod_t * pDemod,
                                                      int32_t freqOffsetKHz)
{
    uint8_t data[3];
    uint32_t regvalue = 0;
    uint8_t isNegative = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    uint32_t tempDiv = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_SetCFFine");

    if (!pDemod) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    freqOffsetKHz *= (-1);

    if (freqOffsetKHz < 0){
        isNegative = 1;
        regvalue = (uint32_t)(freqOffsetKHz * (-1));
    } else {
        isNegative = 0;
        regvalue = (uint32_t)freqOffsetKHz;
    }

    /*
     * regvalue = regvalue(21bit) * 2^20 / SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_CKAL
     *          = ((regvalue(21bit) * 2^10) * 2^10) / SONYSAT_DEMOD_DVBS_S2_BLINDSCAN_CKAL
     */

    /* CKAL */
    tempDiv = 64917;
    
    tempQ = (regvalue * 1024) / tempDiv;
    tempR = (regvalue * 1024) % tempDiv;

    tempR *= 1024;
    tempQ = (tempQ * 1024) + (tempR / tempDiv);
    tempR = tempR % tempDiv;

    if (tempR >= (tempDiv/2)){
        tempQ = tempQ + 1;
    } else {
        tempQ = tempQ;
    }

    if (isNegative){
        if (tempQ > 0x100000) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        regvalue = (uint32_t)((int32_t)tempQ * (-1));
    } else {
        if (tempQ > 0xFFFFF) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_RANGE);
        }
        regvalue = tempQ;
    }

    data[0] = (uint8_t)((regvalue >> 16) & 0x1F);
    data[1] = (uint8_t)((regvalue >>  8) & 0xFF);
    data[2] = (uint8_t)( regvalue        & 0xFF);

    /* Set SLV-T Bank : 0xA0 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xA0) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OCFRL_CTRLVALINIT[20:0] */
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCA, data, 3) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_SetSampleMode (sonysat_demod_t * pDemod,
                                                          uint8_t isHSMode)
{
    uint8_t data = 0;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_SetSampleMode");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if (isHSMode){
        data = 0x01;
    } else {
        data = 0x00;
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OREG_ARC_HSMODE[0] */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, data) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_SetSymbolRateRatio (sonysat_demod_t * pDemod,
                                                               uint32_t ratioMin,
                                                               uint32_t ratioMax)
{
    uint8_t data[4];
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_SetSymbolRateRatio");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    if ((ratioMax > 0x03FF) || (ratioMin > 0x3FF)){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    data[0] = (uint8_t)((ratioMax >> 8) & 0x03);
    data[1] = (uint8_t)( ratioMax       & 0xFF);
    data[2] = (uint8_t)((ratioMin >> 8) & 0x03);
    data[3] = (uint8_t)( ratioMin       & 0xFF);

    /* Set SLV-T Bank : 0xAE */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xAE) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    /* OFSM_RATIOMAX_ARS[9:0]
     * OFSM_RATIOMIN_ARS[9:0]
     */
    if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x28, data, 4) != SONYSAT_RESULT_OK){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }

    SONYSAT_TRACE_RETURN (SONYSAT_RESULT_OK);
}

sonysat_result_t sonysat_demod_dvbs_s2_blindscan_SetTSOut (sonysat_demod_t * pDemod,
                                                     uint8_t enable)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    SONYSAT_TRACE_ENTER ("sonysat_demod_dvbs_s2_blindscan_SetTSOut");

    if (!pDemod){
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONYSAT_RESULT_OK) {
        SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
    }
    if (enable){
        /* Enable TS output */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x00) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    } else {
        /* Disable TS output */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != SONYSAT_RESULT_OK) {
            SONYSAT_TRACE_RETURN (SONYSAT_RESULT_ERROR_I2C);
        }
    }

    SONYSAT_TRACE_RETURN (result);
}
