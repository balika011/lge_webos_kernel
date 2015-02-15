/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2012/08/07
 Revision: 1.0.3.0

------------------------------------------------------------------------------*/

#include "sony_cn_dtv_demod_monitorDvbc.h"
#include "sony_math.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
/* Freeze/UnFreeze Register */
#define j_FreezeReg(pDemod) ((pDemod)->pI2c->WriteOneRegister((pDemod)->pI2c, (pDemod)->i2cAddress, 0x01, 0x01))
#define j_UnFreezeReg(pDemod) ((void)((pDemod)->pI2c->WriteOneRegister((pDemod)->pI2c, (pDemod)->i2cAddress, 0x01, 0x00)))

/*------------------------------------------------------------------------------
 Prototypes of static functions
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Monitor Sync Status

  pDemod    : Address of demodulator instance.
  pArLock   : Address of demodulator lock flag.(0:UNLOCK, 1:LOCK ; in Scan 0:searching, 1:DVB-C signal detected)
  pTsLock   : Address of TS lock flag(new).(0:UNLOCK, 1:LOCK)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SyncStat(sony_cn_dtv_demod_t* pDemod,
                                                     uint8_t* pArLock,
                                                     uint8_t* pTsLock)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_SyncStat");

    /* Null check */
    if(!pDemod || !pArLock || !pTsLock){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_AR_TIMEOUT/IREG_AR_LOCK(ar_status1) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x88, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pArLock = data & 0x03;

    /* IREG_TSIF_TS_LOCK */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB0, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pTsLock = (data & 0x01) ? 1 : 0;

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor ifagc output level

  pDemod : Address of demodulator instance.
  pIfAgc : Address of AGC output level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_IfAgcout(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pIfAgc)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_IfAgcout");

    /* Null check */
    if(!pDemod || !pIfAgc){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IIFAGC_OUT[11:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xA5, rdata, 2);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pIfAgc = ((rdata[0] & 0x0F) << 8) | (rdata[1]);


    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor Constellation

  pDemod : Address of demodulator instance.
  pQam   : Address of constellation.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_QAM(sony_cn_dtv_demod_t* pDemod,
                                                sony_cn_dtv_dvbc_cnst_t* pQam)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t arLock = 0;
    uint8_t tsLock = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_QAM");

    /* Null check */
    if(!pDemod || !pQam){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!(arLock&0x01)){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_QAM_LEVEL[2:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x19, &data, 1);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    *pQam = (sony_cn_dtv_dvbc_cnst_t)(data & 0x07);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor symbol rate

  pDemod : Address of demodulator instance.
  pSymbolRate : Address of symbol rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SymbolRate(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pSymbolRate)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t rdata1[2];
    uint8_t rdata2[5];
    uint32_t trialsrate;
    int32_t  tlfIntegral;
    uint32_t tsmDrate;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_SymbolRate");

    /* Null check */
    if(!pDemod || !pSymbolRate){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_TRIALSRATE[11:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x1A, rdata1, 2);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    trialsrate = ((rdata1[0] & 0x0F) << 8) | rdata1[1];

    /* IREG_TLFINTEGRAL[31:0] & IREG_TSMDRATE[1:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x9B, rdata2, 5);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    tlfIntegral = (int32_t)((rdata2[0] << 24) | (rdata2[1] << 16) | (rdata2[2] << 8) | rdata2[3]);
    tsmDrate = rdata2[4] & 0x03;

    /* IREG_AR_STATE[4:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x8F, &data, 1);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    /* Validity check */    
    if((11 <= (data & 0x1F)) && ((data & 0x1F) <= 15)){
        /* Synchronization is ready */
        /* Symbol Rate[kSymbol/sec] = ( IREG_TRIALSRATE + ( IREG_TLFINTEGRAL / ( 2^( 24 + IREG_TSMDRATE ) ) ) ) * 41 * 1000 / 16384 
                                    = ( IREG_TRIALSRATE + ( IREG_TLFINTEGRAL / ( 2^( 24 + IREG_TSMDRATE ) ) ) ) * 2^2 * 41 * 1000 / 2^16 */
        *pSymbolRate = ((trialsrate * 4 + (tlfIntegral / (1 << (22 + tsmDrate)))) * 41 * 1000 / 65536);
    }else{
        /* Synchronization is not ready */
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor carrier offset

  pDemod : Address of demodulator instance.
  pOffset : Address of carrier offset.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_CarrierOffset(sony_cn_dtv_demod_t* pDemod,
                                                          int32_t* pOffset)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];
    uint8_t arLock = 0;
    uint8_t tsLock = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_CarrierOffset");

    /* Null check */
    if(!pDemod || !pOffset){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!(arLock&0x01)){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_FREQ_OFFSET[13:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x15, rdata, 2);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    /* 14bit signed value to 32bit signed value */
    *pOffset = sony_Convert2SComplement_DTMB((((rdata[0] & 0x3F) << 8) | rdata[1]), 14);
    /* Carrier Offset[KHz] = IREG_FREQ_OFFSET * 2.50244 */
    *pOffset = *pOffset * 41000 / 16384;

    /* RF inversion */
    if(pDemod->isInvertTuner){
        *pOffset *= -1;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor SNR

  pDemod : Address of demodulator instance.
  pSnr   : Address of SNR * 1e3.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SNR(sony_cn_dtv_demod_t* pDemod,
                                                int32_t* pSnr)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t snrReg = 0;
    sony_cn_dtv_dvbc_cnst_t qam;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_SNR");

    /* Null check */
    if(!pDemod || !pSnr){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDvbc_SNRReg(pDemod, &snrReg, &qam);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(snrReg == 0){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    switch(qam){
    case SONY_CN_DTV_DVBC_CNST_16QAM:
    case SONY_CN_DTV_DVBC_CNST_64QAM:
    case SONY_CN_DTV_DVBC_CNST_256QAM:
        /*
         * SNR x 1e3 = -9500 * ln(snrReg / (760 * 32))
         *           = -9500 * (ln(snrReg) - ln(24320))
         *           = -95 * sonylog(snrReg) + 95941
         *
         * NOTE: sonylog(n) returns 100 * ln(n)
         */
        *pSnr = -95 * (int32_t) sony_math_log_DTMB (snrReg) + 95941;
        break;
    case SONY_CN_DTV_DVBC_CNST_32QAM:
    case SONY_CN_DTV_DVBC_CNST_128QAM:
        /*
         * SNR x 1e3 = -8750 * ln(snrReg / (650 * 32))
         *           = -8750 * (ln(snrReg) - ln(20800))
         *           = -875 * sonylog(snrReg) / 10 + 86999
         */
        *pSnr = -875 * (int32_t) sony_math_log_DTMB (snrReg);
        /* Rounding */
        if (*pSnr >= 0){
            *pSnr = (*pSnr + 5) / 10;
        } else {
            *pSnr = (*pSnr - 5) / 10;
        }
        *pSnr += 86999;
        break;
    default:
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor SNR register

  pDemod : Address of demodulator instance.
  pSnrReg : Address of SNR register value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SNRReg(sony_cn_dtv_demod_t* pDemod,
                                                   uint32_t* pSnrReg,
                                                   sony_cn_dtv_dvbc_cnst_t* pQam)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];
    uint8_t arLock = 0;
    uint8_t tsLock = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_SNRReg");

    /* Null check */
    if(!pDemod || !pSnrReg || !pQam){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!(arLock&0x01)){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_SNR_ESTIMATE[12:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4C, rdata, 2);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    *pSnrReg = ((rdata[0] & 0x1F) << 8) | rdata[1];

    /* Constellation */
    result = sony_cn_dtv_demod_MonitorDvbc_QAM(pDemod, pQam);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor BER before Reed-Solomon

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e7.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PreRSBER(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pBer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t biteCnt = 0;
    uint8_t period = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_PreRSBER");

    /* Null check */
    if(!pDemod || !pBer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDvbc_PreRSBitError(pDemod, &biteCnt, &period);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((period <= 11) && (biteCnt > ((1U << period) * 204 * 8))) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      BER = biteCnt * 10000000 / (2^period * 204 * 8)
          = biteCnt * 312500 / (2^period * 51)
          = biteCnt * 250 * 1250 / (2^period * 51)
          (Divide in 2 steps to prevent overflow.)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        if (period <= 8) {
            div = (1U << period) * 51;
        } 
        else {
            div = (1U << 8) * 51;
        }

        Q = (biteCnt * 250) / div;
        R = (biteCnt * 250) % div;

        R *= 1250;
        Q = Q * 1250 + R / div;
        R = R % div;

        if (period > 8) {
            /* rounding */
            *pBer = (Q + (1 << (period - 9))) >> (period - 8);
        } 
        else {
            /* rounding */
            if (R >= (div/2)) {
                *pBer = Q + 1;
            } else {
                *pBer = Q;
            }
        }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of BER before Reed-Solomon

  pDemod   : Address of demodulator instance.
  pBiteCnt : Address of IREG_CWBER_BITECNT registers value.
  pPeriod  : Address of BER measurement period.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PreRSBitError(sony_cn_dtv_demod_t* pDemod,
                                                          uint32_t* pBiteCnt,
                                                          uint8_t* pPeriod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_PreRSBitError");

    /* Null check */
    if(!pDemod || !pBiteCnt || !pPeriod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* OREG_CWBER_PERIOD[4:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xA8, &data, 1);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pPeriod = data & 0x1F;

    /* Period must be less than or equal to 16 */
    if(*pPeriod > 16){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* IREG_CWBER_BITECNT[21:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x36, rdata, 3);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Check Validity */
    if(((rdata[0] >> 6) & 0x01) == 0){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    *pBiteCnt = ((rdata[0] & 0x3F) << 16) | (rdata[1] << 8) | rdata[2];

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor Reed-Solomon error count

  pDemod : Address of demodulator instance.
  pErrorCnt : Address of RS errors per second.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_RSErrorNum(sony_cn_dtv_demod_t* pDemod,
                                                          uint32_t* pErrorCnt)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_RSErrorNum");

    /* Null check */
    if(!pDemod || !pErrorCnt){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_BER_CWRJCT_VALID & IREG_BER_CWRJCTCNT[15:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xE9, rdata, 3);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Check Validity */
    if((rdata[0] & 0x01) == 0){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pErrorCnt = (rdata[1] << 8) | rdata[2];

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor spectrum sense

  pDemod    : Address of demodulator instance.
  pIsInvert : Address of spectrum sense. (0:Normal, 1:Invert)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_SpectrumSense(sony_cn_dtv_demod_t* pDemod,
                                                          uint8_t* pIsInvert)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t arLock = 0;
    uint8_t tsLock = 0;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDvbc_SpectrumSense");

    /* Null check */
    if(!pDemod || !pIsInvert){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(j_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Validity check (AR LOCK) */
    result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!(arLock&0x01)){
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* IREG_DET_SPECTRUM_INV(detectedqam) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x19, &data, 1);
    if(result != SONY_RESULT_OK){ 
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Unfreeze registers */
    j_UnFreezeReg(pDemod);

    if (pDemod->isInvertTuner) {
        *pIsInvert = ((data >> 7) & 0x01) ? 0 : 1;
    } else {
        *pIsInvert = ((data >> 7) & 0x01) ? 1 : 0;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor PER

  pDemod : Address of demodulator instance.
  pBer   : Address of PER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDvbc_PER(sony_cn_dtv_demod_t* pDemod,
                                                uint32_t* pPer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];
    uint32_t rsError = 0;
    uint32_t symRateksps = 0;
    uint32_t N = 0;

    SONY_TRACE_ENTER ("sony_cn_dtv_demod_MonitorDvbc_PER");

    if (!pDemod || !pPer){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (j_FreezeReg(pDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    {
        uint8_t arLock = 0;
        uint8_t tsLock = 0;

        /* Validity check (AR LOCK) */
        result = sony_cn_dtv_demod_MonitorDvbc_SyncStat(pDemod, &arLock, &tsLock);
        if(result != SONY_RESULT_OK){ 
            j_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN(result); 
        }

        if(!(arLock&0x01)){
            j_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set bank 40 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40) != SONY_RESULT_OK) {
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* IREG_BER_CWRJCTCNT */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddress, 0xEA, rdata, 2) != SONY_RESULT_OK) {
        j_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }
    rsError = (rdata[0] << 8) | rdata[1];

    {
        const uint8_t N_table[] = {4, 5, 6, 7, 8}; /* 16QAM, 32QAM, 64QAM, 128QAM, 256QAM */
        /* IREG_QAM_LEVEL */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddress, 0x19, &rdata[0], 1) != SONY_RESULT_OK) {
            j_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if ((rdata[0] & 0x07) >= 5) {
            /* Invalid value */
            j_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
        }

        N = N_table[rdata[0] & 0x07];
    }

    j_UnFreezeReg(pDemod);

    /* Get Symbol Rate (ksps) */
    result = sony_cn_dtv_demod_MonitorDvbc_SymbolRate(pDemod, &symRateksps);
    if(result != SONY_RESULT_OK){ 
        SONY_TRACE_RETURN(result); 
    }

    if ((symRateksps == 0) || (N == 0)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      PER = IREG_BER_CWRJCTCNT / (SymbolRate[MSym/s] * 10^6 * N / (204 * 8))
          = IREG_BER_CWRJCTCNT * (204 * 8) / (SymbolRate[MSym/s] * 10^6 * N)
      PER * 10^6 = IREG_BER_CWRJCTCNT * (204 * 8) / (SymbolRate[MSym/s] * N)
                 = IREG_BER_CWRJCTCNT * (204 * 8) * 1000 / (SymbolRate[KSym/s] * N)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        div = symRateksps * N;

        Q = (rsError * 204 * 8) / div;
        R = (rsError * 204 * 8) % div;

        R *= 1000;
        Q = Q * 1000 + R / div;
        R = R % div;

        /* rounding */
        if ((div != 1) && (R >= (div/2))) {
            *pPer = Q + 1;
        } 
        else {
            *pPer = Q;
        }
    }

    if (*pPer > 1000000) {
        *pPer = 1000000; /* Clip */
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/*------------------------------------------------------------------------------
  static functions
------------------------------------------------------------------------------*/



