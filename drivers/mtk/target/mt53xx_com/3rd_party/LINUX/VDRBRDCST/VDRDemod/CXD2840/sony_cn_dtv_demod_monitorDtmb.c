/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2013/08/07
 Revision: 1.0.3.0

------------------------------------------------------------------------------*/

#include "sony_math.h"
#include "sony_cn_dtv_demod_monitorDtmb.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
/* Freeze/UnFreeze Register */
#define z_FreezeReg(pDemod) ((pDemod)->pI2c->WriteOneRegister((pDemod)->pI2c, (pDemod)->i2cAddress, 0x01, 0x01))
#define z_UnFreezeReg(pDemod) ((void)((pDemod)->pI2c->WriteOneRegister((pDemod)->pI2c, (pDemod)->i2cAddress, 0x01, 0x00)))

/* MER Table */
#define SONY_CN_DTV_DEMOD_PN_MAX_ENTRY 3

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  MER calculation params
--------------------------------------------------------------------*/
typedef struct {
    uint32_t  n; /* n * 10^6 */
    uint8_t   a;
    uint32_t  m;
} sony_cn_dtv_demod_mer_param_t;

/*--------------------------------------------------------------------
  Definition of MER calculation params
--------------------------------------------------------------------*/
/* C1 4QAM-NR */
static const sony_cn_dtv_demod_mer_param_t C1_4QAM_NR[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {120000,  2, 8},   /* PN420 */
    {120000,  2, 8},   /* PN595 */
    {120000,  2, 8},   /* PN945 */
};

/* C1 4QAM */
static const sony_cn_dtv_demod_mer_param_t C1_4QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {120000,  2, 8},   /* PN420 */
    {120000,  2, 8},   /* PN595 */
    {120000,  2, 8},   /* PN945 */
};

/* C1 16QAM */
static const sony_cn_dtv_demod_mer_param_t C1_16QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {3000,    3, 18},   /* PN420 */
    {3000,    3, 18},   /* PN595 */
    {3000,    3, 18},   /* PN945 */
};

/* C1 32QAM */
static const sony_cn_dtv_demod_mer_param_t C1_32QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {600,     3, 40},   /* PN420 */
    {600,     3, 40},   /* PN595 */
    {600,     3, 40},   /* PN945 */
};

/* C1 64QAM */
static const sony_cn_dtv_demod_mer_param_t C1_64QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {150,     3, 80},   /* PN420 */
    {150,     3, 80},   /* PN595 */
    {150,     3, 80},   /* PN945 */
};

/* C3780 4QAM-NR */
static const sony_cn_dtv_demod_mer_param_t C3780_4QAM_NR[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    { 300000, 2, 3},    /* PN420 */
    { 300000, 2, 3},    /* PN595 */
    {1200000, 1, 500},  /* PN945 */
};

/* C3780 4QAM */
static const sony_cn_dtv_demod_mer_param_t C3780_4QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {1100000, 1, 200},  /* PN420 */
    { 900000, 1, 4},    /* PN595 */
    {1200000, 1, 400},  /* PN945 */
};

/* C3780 16QAM */
static const sony_cn_dtv_demod_mer_param_t C3780_16QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {60000,   2, 16},   /* PN420 */
    {50000,   2, 18},   /* PN595 */
    {60000,   2, 18},   /* PN945 */
};

/* C3780 32QAM */
static const sony_cn_dtv_demod_mer_param_t C3780_32QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {800,     3, 40},   /* PN420 */
    {700,     3, 36},   /* PN595 */
    {800,     3, 36},   /* PN945 */
};

/* C3780 64QAM */
static const sony_cn_dtv_demod_mer_param_t C3780_64QAM[SONY_CN_DTV_DEMOD_PN_MAX_ENTRY] =
{
  /* n*10^6,  a, m */
    {150,     3, 80},   /* PN420 */
    {150,     3, 80},   /* PN595 */
    {200,     3, 70},   /* PN945 */
};

/*------------------------------------------------------------------------------
 Prototypes of static functions
------------------------------------------------------------------------------*/
static sony_result_t GetMerCalcParams(sony_cn_dtv_demod_t* pDemod, sony_cn_dtv_demod_mer_param_t* pMerParam);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Monitor chip info

  pDemod  : Address of demodulator instance.
  pChipId : Address of chip id. (8'h29=CXD2840x, 8'h12=CXD2830x)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_ChipID(sony_cn_dtv_demod_t* pDemod,
                                                   uint8_t* pChipId)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_ChipID");

    /* Null check */
    if(!pDemod || !pChipId){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_PRE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ICHIP_ID(chip_id) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xFD, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pChipId = data;

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor sync status

  pDemod   : Address of demodulator instance.
  pDmdLock : Address of demodulator lock flag. (0:UNLOCK, 1:LOCK)
  pTsLock  : Address of TS lock flag. (0:UNLOCK, 1:LOCK)
  pUnlock  : Address of Unlock flag. (0:Searching, 1:DTMB signal is not detected)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SyncStat(sony_cn_dtv_demod_t* pDemod,
                                                     uint8_t* pDmdLock,
                                                     uint8_t* pTsLock,
                                                     uint8_t* pUnlock)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_SyncStat");

    /* Null check */
    if(!pDemod || !pDmdLock || !pTsLock || !pUnlock){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_DMDLOCK */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x40, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pDmdLock = (data & 0x01) ? 1 : 0;

    /* IREG_TSIF_TS_LOCK */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x5C, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pTsLock = (data & 0x01) ? 1 : 0;

    /* IREG_AR_SCANDONE_SHOLD */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x1F, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pUnlock = (data & 0x01) ? 1 : 0;

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor ifagc output level

  pDemod : Address of demodulator instance.
  pIfAgc : Address of AGC ouput level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_IfAgcout(sony_cn_dtv_demod_t* pDemod,
                                                     uint32_t* pIfAgc)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_Ifagcout");

    /* Null check */
    if(!pDemod || !pIfAgc){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IIFAGC_OUT[11:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x24, rdata, 2);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pIfAgc = ((rdata[0] & 0x0F) << 8) | rdata[1];

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor carrier mode

  pDemod       : Address of demodulator instance.
  pCarrierMode : Address of carrier mode.(C1/C3780)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_CarrierMode(sony_cn_dtv_demod_t* pDemod,
                                                        sony_cn_dtv_dtmb_carrier_mode_t* pCarrierMode)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_CarrierMode");

    /* Null check */
    if(!pDemod || !pCarrierMode){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_SEQ_CMODE */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x11, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pCarrierMode = (sony_cn_dtv_dtmb_carrier_mode_t)(data & 0x01);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor pseudo noise information

  pDemod  : Address of demodulator instance.
  pPnInfo : Address of pseudo noise information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PNInfo(sony_cn_dtv_demod_t* pDemod,
                                                   sony_cn_dtv_dtmb_pseudo_noise_t* pPnInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t dmdLock = 0;
    uint8_t tsLock  = 0;
    uint8_t unlock  = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PNInfo");

    /* Null check */
    if(!pDemod || !pPnInfo){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!dmdLock){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_PN[2:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x13, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    *pPnInfo = (sony_cn_dtv_dtmb_pseudo_noise_t)(data & 0x07);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor system information

  pDemod      : Address of demodulator instance.
  pSystemInfo : Address of system information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SystemInfo(sony_cn_dtv_demod_t* pDemod,
                                                       sony_cn_dtv_dtmb_system_info_t* pSystemInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t dmdLock = 0;
    uint8_t tsLock  = 0;
    uint8_t unlock  = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_SystemInfo");

    /* Null check */
    if(!pDemod || !pSystemInfo){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    if(!dmdLock){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IREG_CNST[2:0]/IREG_LDPC_RATE[1:0]/IREG_FEC_ILMD(si_mon_0) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    pSystemInfo->cnst = (sony_cn_dtv_dtmb_cnst_t)((data >> 3) & 0x07);
    pSystemInfo->rate = (sony_cn_dtv_dtmb_ldpc_rate_t)((data >> 1) & 0x03);
    pSystemInfo->il   = (sony_cn_dtv_dtmb_fec_il_t)(data & 0x01);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor demodulation sequence state

  pDemod    : Address of demodulator instance.
  pSeqState : Address of demodulator sequence state.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SeqState(sony_cn_dtv_demod_t* pDemod,
                                                     sony_cn_dtv_demod_dtmb_seq_state_t* pSeqState)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_SeqState");

    /* Null check */
    if(!pDemod || !pSeqState){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_SEQ_STATE[5:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x10, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if((data & 0x3F) == 63){
        *pSeqState = SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_C3780;
    }else if(((data & 0x3F) == 62) || ((data & 0x3F) == 61)){
        *pSeqState = SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_C1;
    }else{ /* Others(under 60) */
        *pSeqState = SONY_CN_DTV_DEMOD_DTMB_SEQ_STATE_ACQUISITION;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor spectrum sense

  pDemod    : Address of demodulator instance.
  pIsInvert : Address of spectrum sense. (0:Normal, 1:Invert)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_SpectrumSense(sony_cn_dtv_demod_t* pDemod,
                                                          uint8_t* pIsInvert)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_cn_dtv_dtmb_carrier_mode_t mode;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_SpectrumSense");

    /* Null check */
    if(!pDemod || !pIsInvert){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Get carrier mode ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &mode);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(mode == SONY_CN_DTV_DTMB_CARRIER_MODE_C1){ /* Carrier mode C1 */
        /* IREG_LSD_IQINV/IREG_LSD_STATELOCK */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x76, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    }else{ /* Carrier mode C3780 */
        /* IREG_SIIF_SPINV/IREG_SIIF_SPINV_ACT */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x77, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    }

    /* Check validity */
    if((data&0x01) == 0x00){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    if (pDemod->isInvertTuner) {
        *pIsInvert = ((data >> 1) & 0x01) ? 0 : 1;
    } else {
        *pIsInvert = ((data >> 1) & 0x01) ? 1 : 0;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor carrier offset

  pDemod  : Address of demodulator instance.
  pOffset : Address of carrier offset(KHz).
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_CarrierOffset(sony_cn_dtv_demod_t* pDemod,
                                                          int32_t* pOffset)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_cn_dtv_dtmb_carrier_mode_t mode;
    uint8_t dmdLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlock = 0;
    uint8_t rdata[4];
    int32_t prepddcIntAgc = 0;
    int32_t ddclIntAcc = 0;
    int32_t crcgIntAcc = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_CarrierOffset");

    /* Null check */
    if(!pDemod || !pOffset){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    if(!dmdLock){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* Get carrier mode ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &mode);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Calculate carrier offset ---*/
    if(mode == SONY_CN_DTV_DTMB_CARRIER_MODE_C1){ /* Carrier mode C1 */
        /* IREG_PREPDDC_INTAGC_OUT[31:0] */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x26, rdata, 4);
        if(result != SONY_RESULT_OK){ 
            z_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN(result);
        }

        prepddcIntAgc = (int32_t)((rdata[0] << 24) | (rdata[1] << 16) | (rdata[2] << 8) | rdata[3]);

        /* IREG_DDCL_INT_ACC_OUT[31:0] */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x2A, rdata, 4);
        if(result != SONY_RESULT_OK){ 
            z_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN(result); 
        }

        /* Unfreeze registers */
        z_UnFreezeReg(pDemod);

        ddclIntAcc = (int32_t)((rdata[0] << 24) | (rdata[1] << 16) | (rdata[2] << 8) | rdata[3]);

        /* Carrier Offset[kHz] = ( IREG_PREPDDC_INTAGC_OUT + IREG_DDCL_INT_ACC_OUT ) * 7560 / ( 2^30 ) */
        /* 7560/(2^30) = 7.040799e-6 -> 1/(7560/(2^30)) = 142029.33 = 142029 */
        *pOffset = (prepddcIntAgc + ddclIntAcc) / 142029;

    }else{ /* Carrier mode C3780 */
        /* IREG_CRCG_INT_ACC[27:0] */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x2E, rdata, 4);
        if(result != SONY_RESULT_OK){ 
            z_UnFreezeReg(pDemod);
            SONY_TRACE_RETURN(result); 
        }

        /* Unfreeze registers */
        z_UnFreezeReg(pDemod);

        /* 28bit signed value to 32bit signed value */
        crcgIntAcc = sony_Convert2SComplement_DTMB((((rdata[0] & 0x0F) << 24) | (rdata[1] << 16) | (rdata[2] << 8) | rdata[3]), 28);

        /* Carrier Offset[kHz] = IREG_CRCG_INT_ACC * 15120 / ( 2^31 ) = IREG_CRCG_INT_ACC * 7560 / ( 2^30 ) */
        /* 7560/(2^30) = 7.040799e-6 -> 1/(7560/(2^30)) = 142029.33 = 142029 */
        *pOffset = crcgIntAcc / 142029;
    }

    /* Sign of carrier offset value changes according to RF spectrum sense */
    if(pDemod->isInvertTuner){
        *pOffset *= -1;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor MER(Modulation Error Ratio)

  pDemod : Address of demodulator instance.
  pMer   : Address of MER * 1e3
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_MER(sony_cn_dtv_demod_t* pDemod,
                                                int32_t* pMer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t merDt=0;
    sony_cn_dtv_demod_mer_param_t merParams = {0, 0, 0};

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_MER");

    /* Null check */
    if(!pDemod || !pMer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_MERDT(pDemod, &merDt);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(merDt == 0){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    result = GetMerCalcParams(pDemod, &merParams);

    if((merDt/64) <= merParams.m){
        /*
         * MER[dB]         = 10 * log(n * (merDt/64)^a)
         * MER * 10^3 [dB] = 10000 * log(n * (merDt/64)^a)
         *                 = 10000 * (log(n) + log((merDt/64)^a))
         *                 = 10000 * (log(n) + a * log(merDt) - a * log(64))
         *                 = 10000 * (log(n_table) - log(10^6) + a * log(merDt) - a * log(64))
         *                 = 10000 * (log(n_table) + a * log(merDt) - a * log(64) - 6)
         *                 = 100 * (sonylog10(n_table) + a * sonylog10(merDt)) - a * 18062 - 60000
         *
         * NOTE: n_table = n * 10^6, sonylog10(n) returns 100 * log(n)
         */
        *pMer = 100 * ((int32_t)sony_math_log10_DTMB(merParams.n) + ((int32_t)(merParams.a) * (int32_t)sony_math_log10_DTMB(merDt)))
                - ((int32_t)(merParams.a) * 18062) - 60000;
    }else{
        /*
         * MER[dB]         = 10 * log(merDt/64)
         * MER * 10^3 [dB] = 10000 * log(merDt/64)
         *                 = 100 * sonylog10(merDt) - 18062
         */
        *pMer = 100 * (int32_t)sony_math_log10_DTMB(merDt) - 18062;
    }


    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor MERDT

  pDemod : Address of demodulator instance.
  pMerDt : Address of MER_DT registers value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_MERDT(sony_cn_dtv_demod_t* pDemod,
                                                  uint32_t* pMerDt)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_MERDT");

    /* Null check */
    if(!pDemod || !pMerDt){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_MER_VALID/IREG_MER_DT[17:0](mer_dt_X) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x37, rdata, 3);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Check Validity */
    if(((rdata[0] >> 2) & 0x01) == 0){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pMerDt = (((rdata[0] & 0x03) << 16) | (rdata[1] << 8) | rdata[2]);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor BER before LDPC

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e7
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pBer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t bitErr = 0;
    uint8_t lberMes = 0;
    uint8_t lberInfo = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    int32_t i = 0;
    sony_cn_dtv_dtmb_ldpc_rate_t rate = SONY_CN_DTV_DTMB_LDPC_RATE_RESERVED;


    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PreLdpcBER");

    /* Null check */
    if(!pDemod || !pBer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_PreLdpcBitError(pDemod, &bitErr, &lberMes, &lberInfo, &rate);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if (lberMes > 15) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /*
     * BER = IBER1_BITERR / ( 2^OREG_LBER_MES * N_ldpc )
     *
     * IBER1_BITERR  : 0 - 0x0FFFFFFF
     * OREG_LBER_MES : 0 - 15
     */
    if(!lberInfo){
        /* N_ldpc = 7488
         *
         * BER * 1e7 = IBER1_BITERR * 10000000 / ( 2^OREG_LBER_MES * 7488 )
         *           = IBER1_BITERR * 156250 / ( 2^OREG_LBER_MES * 117 )
         *           = IBER1_BITERR * 10 * 125^2 / ( 2^OREG_LBER_MES * 117 )
         */
        if (bitErr > ((1U << lberMes) * 7488)) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }

        tempDiv = (uint32_t)((1U << lberMes) * 117);

        tempQ = (bitErr * 10) / tempDiv;
        tempR = (bitErr * 10) % tempDiv;

        for (i = 0; i < 2; i++) {
            tempR *= 125;
            tempQ = (tempQ * 125) + (tempR / tempDiv);
            tempR = tempR % tempDiv;
        }

        if (tempR >= (tempDiv/2)) {
            *pBer = tempQ + 1;
        } else {
            *pBer = tempQ;
        }
    }else{
        if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_4){
            /* N_ldpc = 3048
             *
             * BER * 1e7 = IBER1_BITERR * 10000000 / ( 2^OREG_LBER_MES * 3048 )
             *           = IBER1_BITERR * 1250000 / ( 2^OREG_LBER_MES * 381 )
             *           = IBER1_BITERR * 10 * 50^3 / ( 2^OREG_LBER_MES * 381 )
             */
            if (bitErr > ((1U << lberMes) * 3048)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)((1U << lberMes) * 381);

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 3; i++) {
                tempR *= 50;
                tempQ = (tempQ * 50) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_6){
            /* N_ldpc = 4572
             *
             * BER * 1e7 = IBER1_BITERR * 10000000 / ( 2^OREG_LBER_MES * 4572 )
             *           = IBER1_BITERR * 2500000 / ( 2^OREG_LBER_MES * 1143 )
             *           = IBER1_BITERR * 10 * 100 * 100 * 25 / ( 2^OREG_LBER_MES * 1143 )
             */
            if (bitErr > ((1U << lberMes) * 4572)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)((1U << lberMes) * 1143);

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 2; i++) {
                tempR *= 100;
                tempQ = (tempQ * 100) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            tempR *= 25;
            tempQ = (tempQ * 25) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_8){
            /* N_ldpc = 6096
             *
             * BER * 1e7 = IBER1_BITERR * 10000000 / ( 2^OREG_LBER_MES * 6096 )
             *           = IBER1_BITERR * 625000 / ( 2^OREG_LBER_MES * 381 )
             *           = IBER1_BITERR * 10 * 250^2 / ( 2^OREG_LBER_MES * 381 )
             */
            if (bitErr > ((1U << lberMes) * 6096)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)((1U << lberMes) * 381);

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 2; i++) {
                tempR *= 250;
                tempQ = (tempQ * 250) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else{
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of BER before LDPC

  pDemod           : Address of demodulator instance.
  pBitError        : Address of IBER1_BITERR registers value.
  pLberMes         : Address of OREG_LBER_MES register value.
  pLberInfoOnlyReg : Address of OREG_LBER_INFO_ONLY register value.
  pRate            : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreLdpcBitError(sony_cn_dtv_demod_t* pDemod,
                                                            uint32_t* pBitError,
                                                            uint8_t* pLberMes,
                                                            uint8_t* pLberInfoOnlyReg,
                                                            sony_cn_dtv_dtmb_ldpc_rate_t* pRate)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[4];
    uint8_t data=0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PreLdpcBitError");

    /* Null check */
    if(!pDemod || !pBitError || !pLberMes || !pLberInfoOnlyReg || !pRate){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* ILBER_VALID/IBER1_BITERR[27:0](lber_valid & ber1_biterr_X) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x47, rdata, 4);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    /* Check Validity */
    if(((rdata[0] >> 4) & 0x01) == 0){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pBitError = ((rdata[0] & 0x0F) << 24) | (rdata[1] << 16) | (rdata[2] << 8) | rdata[3];

    /* OREG_LBER_INFO_ONLY/OREG_LBER_MES[3:0](lber_mes) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBE, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result);
    }

    *pLberMes = data & 0x0F;
    *pLberInfoOnlyReg = (data >> 5) & 0x01;

    /* IREG_LDPC_RATE[1:0](si_mon_0) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    *pRate = (sony_cn_dtv_dtmb_ldpc_rate_t)((data >> 1) & 0x03);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor FER after LDPC

  pDemod : Address of demodulator instance.
  pFer   : Address of FER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostLdpcFER(sony_cn_dtv_demod_t* pDemod,
                                                        uint32_t* pFer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t fbErr = 0;
    uint8_t lberMes = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostLdpcFER");

    /* Null check */
    if(!pDemod || !pFer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_PostLdpcFbError(pDemod, &fbErr, &lberMes);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if (lberMes > 15) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /*
     * FER       = fbErr / (2^lberMes)
     * FER * 1e6 = fbErr * 1000000 / (2^lberMes)
     *           = fbErr * 100000 * 10 / (2^lberMes)
     *
     * 0 <= fbErr <= 0x7FFF
     */
    if (fbErr > (1U << lberMes)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    tempDiv = (uint32_t)(1 << lberMes);

    tempQ = (fbErr * 100000) / tempDiv;
    tempR = (fbErr * 100000) % tempDiv;

    tempR *= 10;
    tempQ = (tempQ * 10) + (tempR / tempDiv);
    tempR = tempR % tempDiv;

    if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
        *pFer = tempQ + 1;
    } else {
        *pFer = tempQ;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of FER after LDPC

  pDemod : Address of demodulator instance.
  pFbError : Address of IBER0_FBERR registers value.
  pLberMes : Address of OREG_LBER_MES register value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostLdpcFbError(sony_cn_dtv_demod_t* pDemod,
                                                            uint32_t* pFbError,
                                                            uint8_t* pLberMes)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t rdata[2];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostLdpcFbError");

    /* Null check */
    if(!pDemod || !pFbError || !pLberMes){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* ILBER_VALID(lber_valid) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x47, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Check Validity */
    if(((data >> 4) & 0x01) == 0){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* IBER0_FBERR[14:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4B, rdata, 2);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pFbError = (((rdata[0]) & 0x7F) << 8) | (rdata[1]);

    /* OREG_LBER_MES[3:0](lber_mes) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBE, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    *pLberMes = data & 0x0F;

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor BER before BCH

  pDemod : Address of demodulator instance.
  pBer   : Address of BER * 1e9.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreBchBER(sony_cn_dtv_demod_t* pDemod,
                                                      uint32_t* pBer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t bitErr = 0;
    uint8_t bberMes = 0;
    uint8_t lberMes = 0;
    uint8_t bberSep = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    int32_t i = 0;
    sony_cn_dtv_dtmb_ldpc_rate_t rate = SONY_CN_DTV_DTMB_LDPC_RATE_RESERVED;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PreBchBER");

    /* Null check */
    if(!pDemod || !pBer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_PreBchBitError(pDemod, &bitErr, &bberMes, &lberMes, &bberSep, &rate);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((bberMes > 18) && (lberMes > 15)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* BER = IBER0_BITERR / ( N_BBER_MES * 762 ) */
    if(!bberSep){
        if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_4){
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 4
             *
             * BER * 1e9 = IBER0_BITERR * 1000000000 / ((2^OREG_LBER_MES) * 4 * 762)
             *           = IBER0_BITERR *  125000000 / ((2^OREG_LBER_MES) * 381)
             *           = IBER0_BITERR * 10 * 250 * 250 * 200 / ((2^OREG_LBER_MES) * 381)
             *
             *
             */
            if (bitErr > ((1U << lberMes) * 4 * 762)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1U << lberMes) * 381;

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 2; i++) {
                tempR *= 250;
                tempQ = (tempQ * 250) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            tempR *= 200;
            tempQ = (tempQ * 200) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_6){
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 6
             *
             * BER * 1e9 = IBER0_BITERR * 1000000000 / ((2^OREG_LBER_MES) * 6 * 762)
             *           = IBER0_BITERR *  250000000 / ((2^OREG_LBER_MES) * 1143)
             *           = IBER0_BITERR * 10 * 100 * 100 * 100 * 25 / ((2^OREG_LBER_MES) * 1143)
             */
            if (bitErr > ((1U << lberMes) * 6 * 762)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1U << lberMes) * 1143;

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 3; i++) {
                tempR *= 100;
                tempQ = (tempQ * 100) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            tempR *= 25;
            tempQ = (tempQ * 25) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_8){
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 8
             *
             * BER * 1e9 = IBER0_BITERR * 1000000000 / ((2^OREG_LBER_MES) * 8 * 762)
             *           = IBER0_BITERR *   62500000 / ((2^OREG_LBER_MES) * 381)
             *           = IBER0_BITERR * 10 * 250 * 250 * 100 / ((2^OREG_LBER_MES) * 381)
             */
            if (bitErr > ((1U << lberMes) * 8 * 762)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1U << lberMes) * 381;

            tempQ = (bitErr * 10) / tempDiv;
            tempR = (bitErr * 10) % tempDiv;

            for (i = 0; i < 2; i++) {
                tempR *= 250;
                tempQ = (tempQ * 250) + (tempR / tempDiv);
                tempR = tempR % tempDiv;
            }

            tempR *= 100;
            tempQ = (tempQ * 100) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        }else{
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }else{
        /* N_BBER_MES = 2^OREG_BBER_MES
         *
         * BER * 1e9 = IBER0_BITERR * 1000000000 / ((2^OREG_BBER_MES) * 762)
         *           = IBER0_BITERR *  500000000 / ((2^OREG_BBER_MES) * 381)
         *           = (IBER0_BITERR * 2^8 * 5^9)  / ((2^OREG_BBER_MES) * 381)
         *
         * (OREG_BBER_MES <  8)
         *           = (IBER0_BITERR * 2^8 * 5^9)  / ((2^OREG_BBER_MES) * 381)
         *           = (IBER0_BITERR * 20000 * 25000)  / ((2^OREG_BBER_MES) * 381)
         *
         * (OREG_BBER_MES >= 8)
         *           = (IBER0_BITERR * 5^9)  / ((2^(OREG_BBER_MES - 8)) * 381)
         *           = (IBER0_BITERR * 3125 * 625)  / ((2^(OREG_BBER_MES - 8)) * 381)
         *
         * 0 <= IBER0_BITERR  <= 0x0FFFFFFF
         * 0 <= OREG_BBER_MES <= 18
         */
        if (bitErr > ((1U << bberMes) * 762)) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }

        if (bberMes < 8) {
            tempDiv = (uint32_t)(1U << bberMes) * 381;

            tempQ = (bitErr * 20000) / tempDiv;
            tempR = (bitErr * 20000) % tempDiv;

            tempR *= 25000;
            tempQ = (tempQ * 25000) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }
        } else {
            tempDiv = (uint32_t)(1U << (bberMes - 8)) * 381;

            tempQ = bitErr / tempDiv;
            tempR = bitErr % tempDiv;

            tempR *= 3125;
            tempQ = (tempQ * 3125) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            tempR *= 625;
            tempQ = (tempQ * 625) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)){
                *pBer = tempQ + 1;
            } else {
                *pBer = tempQ;
            }            
        }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of BER before BCH

  pDemod : Address of demodulator instance.
  pBitError : Address of IBER0_BITERR registers value.
  pBberMes  : Address of OREG_BBER_MES register value.
  pLberMes  : Address of OREG_LBER_MES register value.
  pBberSep  : Address of OREG_BBER_SEP register value.
  pRate     : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PreBchBitError(sony_cn_dtv_demod_t* pDemod,
                                                           uint32_t* pBitError,
                                                           uint8_t* pBberMes,
                                                           uint8_t* pLberMes,
                                                           uint8_t* pBberSep,
                                                           sony_cn_dtv_dtmb_ldpc_rate_t* pRate)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[4];
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PreBchBitError");

    /* Null check */
    if(!pDemod || !pBitError || !pBberMes || !pLberMes || !pBberSep || !pRate){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IBBER_VALID/IBER0_BITERR[27:0](bber_valid & ber0_biterr_X) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4D, rdata, 4);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Check Validity */
    if(((rdata[0] >> 4) & 0x01) == 0){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pBitError = ((rdata[0] & 0x0F) << 24) | (rdata[1] << 16) | (rdata[2] << 8) | (rdata[3]);

    /* OREG_BBER_MES[4:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBF, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pBberMes = data & 0x1F;

    /* OREG_BBER_SEP/OREG_LBER_MES[3:0](lber_mes) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBE, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pLberMes = data & 0x0F;
    *pBberSep = (data >> 4) & 0x01;

    /* IREG_LDPC_RATE[1:0](si_mon_0) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pRate = (sony_cn_dtv_dtmb_ldpc_rate_t)((data >> 1) & 0x03);

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor FER after BCH

  pDemod : Address of demodulator instance.
  pFer   : Address of FER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchFER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pFer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t fbErr = 0;
    uint8_t bberMes = 0;
    uint8_t lberMes = 0;
    uint8_t bberSep = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    sony_cn_dtv_dtmb_ldpc_rate_t rate = SONY_CN_DTV_DTMB_LDPC_RATE_RESERVED;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostBchFER");

    /* Null check */
    if(!pDemod || !pFer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_PostBchFbError(pDemod, &fbErr, &bberMes, &lberMes, &bberSep, &rate);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((bberMes > 18) && (lberMes > 15)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* FER = IBER2_FBERR / ( N_BBER_MES ) */
    if(!bberSep){
        if (rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_4) {
            /* 
             * N_BBER_MES = (2^OREG_LBER_MES) * 4 
             *
             * FER * 1e6 = IBER2_FBERR * 1000000 / ((2^OREG_LBER_MES) * 4)
             *           = IBER2_FBERR *  250000 / (2^OREG_LBER_MES)
             *           = IBER2_FBERR * 10000 * 25 / (2^OREG_LBER_MES)
             *
             * 0 <= IBER2_FBERR   <= 0x03FFFF
             * 0 <= OREG_LBER_MES <= 0x0F
             */
            if (fbErr > ((1U << lberMes) * 4)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1U << lberMes);

            tempQ = (fbErr * 10000) / tempDiv;
            tempR = (fbErr * 10000) % tempDiv;

            tempR *= 25;
            tempQ = (tempQ * 25) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
                *pFer = tempQ + 1;
            } else {
                *pFer = tempQ;
            }
        } else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_6) {
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 6
             *
             * FER * 1e6 = IBER2_FBERR * 1000000 / ((2^OREG_LBER_MES) * 6)
             *           = IBER2_FBERR *  500000 / ((2^OREG_LBER_MES) * 3)
             *           = IBER2_FBERR * 10000 * 50 / ((2^OREG_LBER_MES) * 3)
             *
             */
            if (fbErr > ((1U << lberMes) * 6)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)((1U << lberMes) * 3);

            tempQ = (fbErr * 10000) / tempDiv;
            tempR = (fbErr * 10000) % tempDiv;

            tempR *= 50;
            tempQ = (tempQ * 50) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pFer = tempQ + 1;
            } else {
                *pFer = tempQ;
            }
        } else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_8) {
            /* 
             * N_BBER_MES = (2^OREG_LBER_MES) * 8
             *
             * FER * 1e6 = IBER2_FBERR * 1000000 / ((2^OREG_LBER_MES) * 8)
             *           = IBER2_FBERR *  125000 / (2^OREG_LBER_MES)
             *           = IBER2_FBERR * 1000 * 125 / (2^OREG_LBER_MES)
             *
             * 0 <= IBER2_FBERR   <= 0x03FFFF
             * 0 <= OREG_LBER_MES <= 0x0F
             */
            if (fbErr > ((1U << lberMes) * 8)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1U << lberMes);

            tempQ = (fbErr * 1000) / tempDiv;
            tempR = (fbErr * 1000) % tempDiv;

            tempR *= 125;
            tempQ = (tempQ * 125) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
                *pFer = tempQ + 1;
            } else {
                *pFer = tempQ;
            }
        } else {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    } else {
        /* 
         * N_BBER_MES = (2^OREG_BBER_MES)
         *
         * FER * 1e6 = IBER2_FBERR * 1000000 / (2^OREG_BBER_MES)
         *           = IBER2_FBERR * 10000 * 100 / (2^OREG_BBER_MES)
         *
         * 0 <= IBER2_FBERR   <= 0x03FFFF
         * 0 <= OREG_BBER_MES <= 18
         */
        if (fbErr > (1U << bberMes)) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }

        tempDiv = (uint32_t)(1U << bberMes);

        tempQ = (fbErr * 10000) / tempDiv;
        tempR = (fbErr * 10000) % tempDiv;

        tempR *= 100;
        tempQ = (tempQ * 100) + (tempR / tempDiv);
        tempR = tempR % tempDiv;

        if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
            *pFer = tempQ + 1;
        } else {
            *pFer = tempQ;
        }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of FER after BCH

  pDemod    : Address of demodulator instance.
  pFbError  : Address of IBER2_FBERR registers value.
  pBberMes  : Address of OREG_BBER_MES register value.
  pLberMes  : Address of OREG_LBER_MES register value.
  pBberSep  : Address of OREG_BBER_SEP register value.
  pRate     : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchFbError(sony_cn_dtv_demod_t* pDemod,
                                                           uint32_t* pFbError,
                                                           uint8_t* pBberMes,
                                                           uint8_t* pLberMes,
                                                           uint8_t* pBberSep,
                                                           sony_cn_dtv_dtmb_ldpc_rate_t* pRate)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostBchFbError");

    /* Null check */
    if(!pDemod || !pFbError || !pBberMes || !pLberMes || !pBberSep || !pRate){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IBBER_VALID(bber_valid) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4D, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Check Validity */
    if(((data >> 4) & 0x01) == 0 ){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* IBER2_FBERR[17:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x51, rdata, 3);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pFbError = ((rdata[0] & 0x03) << 16) | (rdata[1] << 8) | (rdata[2]);

    /* OREG_BBER_MES[4:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBF, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pBberMes = data & 0x1F;

    /* OREG_BBER_SEP/OREG_LBER_MES[3:0](lber_mes) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBE, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pLberMes = data & 0x0F;
    *pBberSep = (data >> 4) & 0x01;

    /* IREG_LDPC_RATE[1:0](si_mon_0) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pRate = (sony_cn_dtv_dtmb_ldpc_rate_t)((data >> 1) & 0x03);

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor PER after BCH

  pDemod : Address of demodulator instance.
  pPer   : Address of PER * 1e6.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPER(sony_cn_dtv_demod_t* pDemod,
                                                       uint32_t* pPer)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t pktErr = 0;
    uint8_t bberMes = 0;
    uint8_t lberMes = 0;
    uint8_t bberSep = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    sony_cn_dtv_dtmb_ldpc_rate_t rate;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostBchPER");

    /* Null check */
    if(!pDemod || !pPer){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    result = sony_cn_dtv_demod_MonitorDtmb_PostBchPacketError(pDemod, &pktErr, &bberMes, &lberMes, &bberSep, &rate);

    if ((bberMes > 18) && (lberMes > 15)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* PER = IBER2_PKTERR / ( N_BBER_MES / 2 ) */
    if(!bberSep){
        if (rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_4) {
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 4
             *
             * PER * 1e6 = IBER2_PKTERR * 1000000 / (((2^OREG_LBER_MES) * 4) / 2)
             *           = IBER2_PKTERR *  500000 / (2^OREG_LBER_MES)
             *           = IBER2_PKTERR * 10000 * 50 / (2^OREG_LBER_MES)
             *
             * 0 <= IBER2_PKTERR  <= 0x1FFFF
             * 0 <= OREG_LBER_MES <= 0x0F
             */
            if (pktErr > ((1U << lberMes) * 4 / 2)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1 << lberMes);

            tempQ = (pktErr * 10000) / tempDiv;
            tempR = (pktErr * 10000) % tempDiv;

            tempR *= 50;
            tempQ = (tempQ * 50) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
                *pPer = tempQ + 1;
            } else {
                *pPer = tempQ;
            }
        } else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_6) {
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 6
             *
             * PER * 1e6 = IBER2_PKTERR * 1000000 / (((2^OREG_LBER_MES) * 6) / 2)
             *           = IBER2_PKTERR * 1000000 / ((2^OREG_LBER_MES) * 3)
             *           = IBER2_PKTERR * 10000 * 100 / ((2^OREG_LBER_MES) * 3)
             *
             * 0 <= IBER2_PKTERR  <= 0x1FFFF
             * 0 <= OREG_LBER_MES <= 0x0F
             */
            if (pktErr > ((1U << lberMes) * 6 / 2)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1 << lberMes) * 3;

            tempQ = (pktErr * 10000) / tempDiv;
            tempR = (pktErr * 10000) % tempDiv;

            tempR *= 100;
            tempQ = (tempQ * 100) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if (tempR >= (tempDiv/2)) {
                *pPer = tempQ + 1;
            } else {
                *pPer = tempQ;
            }
        }else if(rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_8){
            /*
             * N_BBER_MES = (2^OREG_LBER_MES) * 8
             *
             * PER * 1e6 = IBER2_PKTERR * 1000000 / (((2^OREG_LBER_MES) * 8) / 2)
             *           = IBER2_PKTERR *  250000 / (2^OREG_LBER_MES)
             *           = IBER2_PKTERR * 10000 * 25 / (2^OREG_LBER_MES)
             *
             * 0 <= IBER2_PKTERR  <= 0x1FFFF
             * 0 <= OREG_LBER_MES <= 0x0F
             */
            if (pktErr > ((1U << lberMes) * 8 / 2)) {
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
            }

            tempDiv = (uint32_t)(1 << lberMes);

            tempQ = (pktErr * 10000) / tempDiv;
            tempR = (pktErr * 10000) % tempDiv;

            tempR *= 25;
            tempQ = (tempQ * 25) + (tempR / tempDiv);
            tempR = tempR % tempDiv;

            if ((tempDiv != 1) && (tempR >= (tempDiv/2))){
                *pPer = tempQ + 1;
            } else {
                *pPer = tempQ;
            }
        }else{
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }else{
        /*
         * N_BBER_MES = (2^OREG_BBER_MES)
         *
         * PER * 1e6 = IBER2_PKTERR * 1000000 / ((2^OREG_BBER_MES) / 2)
         *           = IBER2_PKTERR * 1000000 / (2^(OREG_BBER_MES - 1))
         *           = IBER2_FBERR * 10000 * 100 / (2^(OREG_BBER_MES - 1))
         *
         * 0 <= IBER2_PKTERR  <= 0x1FFFF
         * 0 <= OREG_BBER_MES <= 18
         */
        if (bberMes == 0) {
            *pPer = (pktErr > 0) ? 1000000 : 0;
            SONY_TRACE_RETURN(SONY_RESULT_OK);
        }

        if (pktErr > ((1U << bberMes) / 2)) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }

        tempDiv = (uint32_t)(1U << (bberMes - 1));

        tempQ = (pktErr * 10000) / tempDiv;
        tempR = (pktErr * 10000) % tempDiv;

        tempR *= 100;
        tempQ = (tempQ * 100) + (tempR / tempDiv);
        tempR = tempR % tempDiv;

        if ((tempDiv != 1) && (tempR >= (tempDiv/2))) {
            *pPer = tempQ + 1;
        } else {
            *pPer = tempQ;
        }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor the registers of PER after BCH

  pDemod       : Address of demodulator instance.
  pPacketError : Address of IBER2_PKTERR registers value.
  pBberMes     : Address of OREG_BBER_MES register value.
  pLberMes     : Address of OREG_LBER_MES register value.
  pBberSep     : Address of OREG_BBER_SEP register value.
  pRate        : Address of LDPC code rate.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPacketError(sony_cn_dtv_demod_t* pDemod,
                                                               uint32_t* pPacketError,
                                                               uint8_t* pBberMes,
                                                               uint8_t* pLberMes,
                                                               uint8_t* pBberSep,
                                                               sony_cn_dtv_dtmb_ldpc_rate_t* pRate)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostBchPacketError");

    /* Null check */
    if(!pDemod || !pPacketError || !pBberMes || !pLberMes || !pBberSep || !pRate){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Freeze registers */
    if(z_FreezeReg(pDemod) != SONY_RESULT_OK){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_I2C); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* IBBER_VALID(bber_valid) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4D, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    /* Check Validity */
    if(((data >> 4) & 0x01) == 0){
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /* IBER2_PKTERR[16:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x54, rdata, 3);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pPacketError = ((rdata[0] & 0x01) << 16) | (rdata[1] << 8) | (rdata[2]);

    /* OREG_BBER_MESS[4:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBF, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pBberMes = data & 0x1F;

    /* OREG_BBER_SEP/OREG_LBER_MES[3:0](lber_mes) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBE, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pLberMes = data & 0x0F;
    *pBberSep = (data >> 4) & 0x01;

    /* IREG_LDPC_RATE[1:0](si_mon_0) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, &data, 1);
    if(result != SONY_RESULT_OK){ 
        z_UnFreezeReg(pDemod);
        SONY_TRACE_RETURN(result); 
    }

    *pRate = (sony_cn_dtv_dtmb_ldpc_rate_t)((data >> 1) & 0x03);

    /* Unfreeze registers */
    z_UnFreezeReg(pDemod);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor packet error numbers after BCH

  pDemod : Address of demodulator instance.
  pPacketErrorNum : Address of BCH errors per second.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum(sony_cn_dtv_demod_t* pDemod,
                                                               uint32_t* pPacketErrorNum)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[3];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_PostBchPktErrorNum");

    /* Null check */
    if(!pDemod || !pPacketErrorNum){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IBBER_VALID_CK(bber_valid_ck) & IBER2_PKTERR_CK[15:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x57, rdata, 3);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Check Validity */
    if((rdata[0] & 0x01) == 0){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pPacketErrorNum = (rdata[1] << 8) | (rdata[2]);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor preset information

  pDemod      : Address of demodulator instance.
  pPresetInfo : Address of preset information.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_MonitorDtmb_Preset(sony_cn_dtv_demod_t* pDemod,
                                                   sony_cn_dtv_demod_preset_info_t* pPresetInfo)
{
    uint8_t dmdLock = 0;
    uint8_t tsLock  = 0;
    uint8_t unlock  = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_MonitorDtmb_Preset");

    /* Null check */
    if(!pDemod || !pPresetInfo){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

    /* Check validity ---*/
    result = sony_cn_dtv_demod_MonitorDtmb_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(!tsLock){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &pPresetInfo->carrierMode);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = sony_cn_dtv_demod_MonitorDtmb_PNInfo(pDemod, &pPresetInfo->pnInfo);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo(pDemod, &pPresetInfo->systemInfo);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}


/*------------------------------------------------------------------------------
  static functions
------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------
  GetMerCalcParams

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t GetMerCalcParams(sony_cn_dtv_demod_t* pDemod, sony_cn_dtv_demod_mer_param_t* pMerParam)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_cn_dtv_dtmb_carrier_mode_t carrierMode;
    sony_cn_dtv_dtmb_system_info_t  systemInfo;
    sony_cn_dtv_dtmb_pseudo_noise_t pnInfo;

    SONY_TRACE_ENTER("GetMerCalcParams");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Get carrier mode */
    result = sony_cn_dtv_demod_MonitorDtmb_CarrierMode(pDemod, &carrierMode);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Get Constellation */
    result = sony_cn_dtv_demod_MonitorDtmb_SystemInfo(pDemod, &systemInfo);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Get PN */
    result = sony_cn_dtv_demod_MonitorDtmb_PNInfo(pDemod, &pnInfo);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Range check */
    if(pnInfo >= SONY_CN_DTV_DTMB_PN_RESERVED){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_RANGE); }

    /* Permute value for MER table */
    if(pnInfo == SONY_CN_DTV_DTMB_PN_420_CONST){ pnInfo = SONY_CN_DTV_DTMB_PN_420_VARIOUS; }
    if(pnInfo == SONY_CN_DTV_DTMB_PN_945_CONST){ pnInfo = SONY_CN_DTV_DTMB_PN_945_VARIOUS; }

    if(carrierMode == SONY_CN_DTV_DTMB_CARRIER_MODE_C1){
        switch(systemInfo.cnst){
        case SONY_CN_DTV_DTMB_CNST_4QAM:
            *pMerParam = C1_4QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_4QAM_NR:
            *pMerParam = C1_4QAM_NR[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_16QAM:
            *pMerParam = C1_16QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_32QAM:
            *pMerParam = C1_32QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_64QAM:
            *pMerParam = C1_64QAM[pnInfo];
            break;
        default:
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }
    }else{
        switch(systemInfo.cnst){
        case SONY_CN_DTV_DTMB_CNST_4QAM:
            *pMerParam = C3780_4QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_4QAM_NR:
            *pMerParam = C3780_4QAM_NR[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_16QAM:
            *pMerParam = C3780_16QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_32QAM:
            *pMerParam = C3780_32QAM[pnInfo];
            break;
        case SONY_CN_DTV_DTMB_CNST_64QAM:
            *pMerParam = C3780_64QAM[pnInfo];
            break;
        default:
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }
    }

    SONY_TRACE_RETURN(result);
}
