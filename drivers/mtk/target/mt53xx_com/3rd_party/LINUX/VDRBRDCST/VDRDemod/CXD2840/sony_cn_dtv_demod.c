/*------------------------------------------------------------------------------

 Copyright 2011-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2014/05/15
 Revision: 1.1.17.0

------------------------------------------------------------------------------*/

#include "sony_cn_dtv_demod.h"


/*------------------------------------------------------------------------------
 Prototypes of static functions
------------------------------------------------------------------------------*/
static sony_result_t SD2SL(sony_cn_dtv_demod_t* pDemod);
static sony_result_t SL2NOZ(sony_cn_dtv_demod_t* pDemod);
static sony_result_t NOZ2SL(sony_cn_dtv_demod_t* pDemod);
static sony_result_t SL2NOJ(sony_cn_dtv_demod_t* pDemod);
static sony_result_t NOJ2SL(sony_cn_dtv_demod_t* pDemod);
static sony_result_t setEqParam(sony_cn_dtv_demod_t* pDemod, uint8_t isCondition1);

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  Create instance

  Initialize pDemod.

  pDemod        : Address of Demodulator instance.
  pI2C          : Address of I2C instance for pDemod.
  i2cAddress    : I2C slave address (default:0xD8)
  xtalFreq      : Crystal frequency
  iffreq_config : IF frequency settings.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Create(sony_cn_dtv_demod_t* pDemod,
                                       sony_i2c_t* pI2C,
                                       uint8_t i2cAddress,
                                       sony_cn_dtv_demod_xtal_freq_t xtalFreq,
                                       sony_cn_dtv_demod_iffreq_config_t* iffreq_config)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_Create");

    /* Null check */
    if(!pDemod || !pI2C || !pI2C->Read || !pI2C->ReadRegister || !pI2C->Write || 
        !pI2C->WriteRegister || !pI2C->WriteOneRegister || !iffreq_config){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Initialize sony_cn_dtv_demod_t */
    pDemod->state = SONY_CN_DTV_DEMOD_STATE_PRE_INIT;
    pDemod->pI2c = pI2C;
    pDemod->i2cAddress = i2cAddress;
    pDemod->xtalFreq = xtalFreq;
    pDemod->isAscot = SONY_CN_DTV_DEMOD_TUNER_ASCOT2E; /* Default */
    pDemod->iffreq_config = *iffreq_config;
    pDemod->isInvertTuner = 0; /* Not invert tuner */
    pDemod->tsOutput = SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_LSB; /* Default */
    pDemod->sharedIf = 0;   /* Not shared(default) */

    pDemod->system = SONY_CN_DTV_SYSTEM_UNKNOWN;
    pDemod->isScanMode = 0;    /* OFF is default */
    pDemod->testMode01 = 0;    /* OFF is default */

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Initialize Demodulator

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Initialize(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_Initialize");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_PRE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set only for 20.5MHz xtal */
    if(pDemod->xtalFreq == SONY_CN_DTV_DEMOD_20500KHZ){
        result = SD2SL(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Initialize Register ---*/
    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* OREGCCLR: RW and RWP Registers */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x02, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* SW reset; Clear Read Registers */
    result = sony_cn_dtv_demod_SoftReset(pDemod);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    pDemod->system = SONY_CN_DTV_SYSTEM_UNKNOWN;
    pDemod->isScanMode = 0;    /* OFF is default */
    pDemod->testMode01 = 0;    /* OFF is default */

    /* SW state transit */
    pDemod->state = SONY_CN_DTV_DEMOD_STATE_INIT;

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuning

  Set tuning parameter.
  If using the fast synchronized Tune, before calling this function, 
  please call the below function.
   1. sony_cn_dtv_demod_SetPreset
      Set preset information and set enable/disable.

  pDemod : Address of demodulator instance.
  system : Broadcast sysytem.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Tune(sony_cn_dtv_demod_t* pDemod, sony_cn_dtv_system_t system)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_Tune");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    switch(pDemod->state){
    case SONY_CN_DTV_DEMOD_STATE_UNKNOWN:
    case SONY_CN_DTV_DEMOD_STATE_PRE_INIT:
    case SONY_CN_DTV_DEMOD_STATE_INVALID:
    default:
        result = SONY_RESULT_ERROR_SW_STATE;
        break;
    case SONY_CN_DTV_DEMOD_STATE_INIT:
        if(system == SONY_CN_DTV_SYSTEM_DTMB){
            pDemod->system = system;
            result = SL2NOZ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else if(system == SONY_CN_DTV_SYSTEM_DVBC){
            pDemod->system = system;
            result = SL2NOJ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else{ /* Unknown system */
            result = SONY_RESULT_ERROR_ARG;
        }
        break;
    case SONY_CN_DTV_DEMOD_STATE_NORMAL_Z:
        if(system == SONY_CN_DTV_SYSTEM_DTMB){
            pDemod->system = system;
            /* Normal-Z -> Normal-Z */
            result = SL2NOZ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else if(system == SONY_CN_DTV_SYSTEM_DVBC){
            pDemod->system = system;
            /* Normal-Z -> Sleep */
            result = NOZ2SL(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            /* Sleep -> Normal-J */
            result = SL2NOJ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else{ /* Unknown system */
            result = SONY_RESULT_ERROR_ARG;
        }
        break;
    case SONY_CN_DTV_DEMOD_STATE_NORMAL_J:
        if(system == SONY_CN_DTV_SYSTEM_DTMB){
            pDemod->system = system;
            /* Normal-J -> Sleep */
            result = NOJ2SL(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            /* Sleep -> Normal-Z */
            result = SL2NOZ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else if(system == SONY_CN_DTV_SYSTEM_DVBC){
            pDemod->system = system;
            /* Normal-J -> Normal-J */
            result = SL2NOJ(pDemod);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }else{ /* Unknown system */
            result = SONY_RESULT_ERROR_ARG;
        }
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Post process of Tuning

  Software reset and excute the following processes.

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_PostTune(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_PostTune");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){
        /* Softreset ---*/
        result = sony_cn_dtv_demod_SoftReset(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        pDemod->state = SONY_CN_DTV_DEMOD_STATE_NORMAL_Z;
    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){
        /* Softreset ---*/
        result = sony_cn_dtv_demod_SoftReset(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        pDemod->state = SONY_CN_DTV_DEMOD_STATE_NORMAL_J;
    }else{ /* Unknown system */
        result = SONY_RESULT_ERROR_HW_STATE;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Sleep

  Change hardware state to "Sleep".

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Sleep(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_Sleep");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    switch(pDemod->state){
    case SONY_CN_DTV_DEMOD_STATE_UNKNOWN:
    case SONY_CN_DTV_DEMOD_STATE_PRE_INIT:
    case SONY_CN_DTV_DEMOD_STATE_INVALID:
    default:
        result = SONY_RESULT_ERROR_SW_STATE;
        break;
    case SONY_CN_DTV_DEMOD_STATE_INIT:
        /* Do nothing */
        break;
    case SONY_CN_DTV_DEMOD_STATE_NORMAL_Z:
        result = NOZ2SL(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        pDemod->state = SONY_CN_DTV_DEMOD_STATE_INIT;
        break;
    case SONY_CN_DTV_DEMOD_STATE_NORMAL_J:
        result = NOJ2SL(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        pDemod->state = SONY_CN_DTV_DEMOD_STATE_INIT;
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Set preset information

  Set preset information to shorten tuning time.
  It must call this function before tuning sequence.

  pDemod         : Address of demodulator instance.
  presetInfo     : Address of preset information.
  presetMode     : Preset mode. (See sony_cn_dtv_demod_preset_mode_t definition.)
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SetPreset(sony_cn_dtv_demod_t* pDemod,
                                          sony_cn_dtv_demod_preset_info_t* pPresetInfo,
                                          sony_cn_dtv_demod_preset_mode_t presetMode)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t autoCmode = 0;
    uint8_t autoDetect = 0;
    uint8_t isCondition1 = 0;
    SONY_TRACE_ENTER("sony_cn_dtv_demod_SetPreset");

    /* Null check */
    if(!pDemod || ((presetMode != SONY_CN_DTV_DEMOD_PRESET_MODE_NONE) && !pPresetInfo)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_PRE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    switch(presetMode){
    case SONY_CN_DTV_DEMOD_PRESET_MODE_NONE:
        /* Set C3780 carrier mode to try first. */
        /* OREG_CMODE */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x80, 0x01, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        autoCmode = 1;
        autoDetect = 1;
        break;
    case SONY_CN_DTV_DEMOD_PRESET_MODE_CMODE:
    case SONY_CN_DTV_DEMOD_PRESET_MODE_FULL_USECMODE:
        /* Set carrier mode only. */
        /* OREG_CMODE */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x80, (uint8_t)pPresetInfo->carrierMode, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        autoCmode = 0;
        autoDetect = 1;
        break;
    case SONY_CN_DTV_DEMOD_PRESET_MODE_FULL:
        /* Set all preset parameters. */
        /* OREG_CMODE */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x80, (uint8_t)pPresetInfo->carrierMode, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_CNST[2:0]/OREG_LDPC_RATE[1:0]/OREG_FEC_ILMD(force_siinfo) */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x81, 
                 (uint8_t)(((pPresetInfo->systemInfo.cnst) << 3) | ((pPresetInfo->systemInfo.rate) << 1) | (pPresetInfo->systemInfo.il)), 0x3F);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_PN[2:0] */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x82, (uint8_t)pPresetInfo->pnInfo, 0x07);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        autoCmode = 0;
        autoDetect = 0;
        break;
    default:
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* OREG_AUTO_CMODE_EN */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xAD, autoCmode, 0x01);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    /* OREG_AUTO_DETECT_EN */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x83, (autoDetect << 1), 0x02);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if((presetMode == SONY_CN_DTV_DEMOD_PRESET_MODE_FULL_USECMODE) || (presetMode == SONY_CN_DTV_DEMOD_PRESET_MODE_FULL)){
        result = sony_cn_dtv_demod_CheckCondition1(pPresetInfo, &isCondition1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    } else {
        isCondition1 = 0;
    }

    result = setEqParam(pDemod, isCondition1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  TS Output enable/disable

  pDemod         : Address of demodulator instance.
  isTsOutEnable  : 0: disable, 1: enable
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_TsOutputEnable(sony_cn_dtv_demod_t* pDemod,
                                               uint8_t isTsOutEnable)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_TsOutputEnable");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(isTsOutEnable){
        /* TS output enable ---*/
        /* OTSDTDISABLE */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x2A, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }
    else{
        /* TS output disable ---*/
        /* OTSDTDISABLE */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x2A, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Soft Reset

  Software reset to DTMB/DVB-C demodulator.

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SoftReset(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_SoftReset");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW reset ---*/
    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* OSOFTRST */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xFE, 0x01);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Check Demod lock status

  pDemod     : Address of demodulator instance.
  pLock      : Lock status.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckDemodLock(sony_cn_dtv_demod_t* pDemod,
                                               sony_cn_dtv_demod_lock_status_t* pLock)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t demodLock = 0;
    uint8_t unlock = 0;
    uint8_t arTimeout = 0;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_CheckDemodLock");

    /* Null check */
    if(!pDemod || !pLock){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(((pDemod->state) != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) && 
       ((pDemod->state) != SONY_CN_DTV_DEMOD_STATE_NORMAL_J)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){ /* DTMB */
        /* Set Bank 0x30 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IREG_DMDLOCK */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x40, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        demodLock = (data & 0x01) ? 1 : 0;

        /* IREG_AR_SCANDONE_SHOLD */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x1F, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        unlock = (data & 0x01) ? 1 : 0;

        if(unlock){
            /* UNLOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK;
        }else if(demodLock){
            /* LOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK;
        }else{
            /* LOCK NOT DETECTED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT;
        }

    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){ /* DVB-C */
        /* Set Bank 0x40 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IREG_AR_TIMEOUT/IREG_AR_LOCK(ar_status1) */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x88, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        demodLock = (data & 0x01);
        arTimeout = ((data >> 1) & 0x01);

        if(demodLock){
            /* LOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK;
        }else if(arTimeout){
            /* UNLOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK;
        }else{
            /* LOCK NOT DETECTED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT;
        }

    }else{ /* Unknown system */
        result = SONY_RESULT_ERROR_HW_STATE;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Check TS lock status

  pDemod     : Address of demodulator instance.
  pLock      : Lock status.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckTSLock(sony_cn_dtv_demod_t* pDemod,
                                            sony_cn_dtv_demod_lock_status_t* pLock)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t demodLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlock = 0;
    uint8_t arTimeout = 0;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_CheckTSLock");

    /* Null check */
    if(!pDemod || !pLock){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if(((pDemod->state) != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) && 
       ((pDemod->state) != SONY_CN_DTV_DEMOD_STATE_NORMAL_J)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if(pDemod->system == SONY_CN_DTV_SYSTEM_DTMB){ /* DTMB */
        /* Set Bank 0x30 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IREG_TSIF_TS_LOCK */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x5C, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        tsLock = (data & 0x01) ? 1 : 0;

        /* IREG_AR_SCANDONE_SHOLD */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x1F, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        unlock = (data & 0x01) ? 1 : 0;

        if(unlock){
            /* UNLOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK;
        }else if(tsLock){
            /* LOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK;
        }else{
            /* LOCK NOT DETECTED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT;
        }
    }else if(pDemod->system == SONY_CN_DTV_SYSTEM_DVBC){ /* DVB-C */
        /* Set Bank 0x40 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IREG_AR_TIMEOUT/IREG_AR_LOCK(ar_status1) */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x88, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        demodLock = (data & 0x01);
        arTimeout = ((data >> 1) & 0x01);

        /* IREG_TSIF_TS_LOCK */
        result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB0, &data, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        tsLock = (data & 0x01) ? 1 : 0;

        if(demodLock && tsLock){
            /* LOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK;
        }else if(arTimeout){
            /* UNLOCKED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK;
        }else{
            /* LOCK NOT DETECTED */
            *pLock = SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT;
        }
    }else{ /* Unknown system */
        result = SONY_RESULT_ERROR_HW_STATE;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Set various parameters 

  pDemod     : Address of demodulator instance.
  configId   : Config parmeters type.
  value      : Setting value.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_SetConfig(sony_cn_dtv_demod_t* pDemod,
                                          sony_cn_dtv_demod_config_t configId,
                                          uint32_t value)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_SetConfig");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    switch(configId)
    {
    case SONY_CN_DTV_DEMOD_CONFIG_TS_OUTPUT_PARALLEL_SERIAL: /* TS Output Parallel or Serial */
        {
            uint8_t data = 0;

            /* SW state check */
            if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_INIT){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

            switch(value)
            {
            /* zout_params
               bit[7]: OSERIALEN (0: Parallel, 1: Serial)
               bit[4]: OWFMT_LSB1STON (TSDATA bit order inversion for Parallel)
               bit[3]: OSEREXCHGB7 (From TSDATA0 or From TSDATA7 for Serial 0: TSDATA0 1: TSDATA7)
               bit[1]: SERCKMODE (0: Continuous, 1: Data only) */

            case SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_LSB:
                /* TS Parallel Output, LSB is output to TSDATA0(default) */
                data = 0x29;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            case SONY_CN_DTV_DEMOD_TS_OUTPUT_PARALLEL_MSB:
                /* TS Parallel Output, LSB is output to TSDATA7 */
                data = 0x39;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            case SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_DATAONLY:
                /* SERIAL output from TSDATA0 pin (Data Only Mode) */
                data = 0xA3;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            case SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_DATAONLY:
                /* SERIAL output from TSDATA7 pin (Data Only Mode) */
                data = 0xAB;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            case SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_CONTINUOUS:
                /* SERIAL output from TSDATA0 pin (Continuous Mode) */
                data = 0xA1;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            case SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_CONTINUOUS:
                /* SERIAL output from TSDATA7 pin (Continuous Mode) */
                data = 0xA9;
                pDemod->tsOutput = (sony_cn_dtv_demod_ts_output_t)value;
                break;
            default:
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
            }

            /* Set Bank 0x00 */
            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC1, data);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_IS_ASCOT:  /* Use ASCOT or Not */
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_PRE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        switch(value)
        {
        case SONY_CN_DTV_DEMOD_TUNER_UNKNOWN:
            /* Do not Use ASCOT */
            pDemod->isAscot = SONY_CN_DTV_DEMOD_TUNER_UNKNOWN;
            break;
        case SONY_CN_DTV_DEMOD_TUNER_ASCOT2D:
            /* Use ASCOT */
            pDemod->isAscot = SONY_CN_DTV_DEMOD_TUNER_ASCOT2D;
            break;
        case SONY_CN_DTV_DEMOD_TUNER_ASCOT2E:
            /* Use ASCOT */
            pDemod->isAscot = SONY_CN_DTV_DEMOD_TUNER_ASCOT2E;
            break;
        case SONY_CN_DTV_DEMOD_TUNER_ASCOT3:
            /* Use ASCOT */
            pDemod->isAscot = SONY_CN_DTV_DEMOD_TUNER_ASCOT3;
            break;
        default:
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
        }

        break;

    case SONY_CN_DTV_DEMOD_CONFIG_INVERT_TUNER:  /* Invert Tuner */
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        pDemod->isInvertTuner = value ? 1 : 0;
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG:  /* Invert the Polarity of the Tuner IFAGC */
        {
            uint8_t data = 0;

            /* SW state check */
            if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
            }

            switch(value)
            {
            case 0:
                /* Do not invert */
                data = 0x08;
                break;
            case 1:
                /* Invert */
                data = 0x48;
                break;
            default:
                SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
            }

            /* Set Bank 0x00 */
            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            /* IFAGC polarity setting ---*/
            /* OCTL_IFAGCNEG */
            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4E, data);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_SHARED_IF:  /* Set Shared IF mode. */
        /* Shared IF mode is available once demodulator is in sleep mode. */
        /* SW state check */
        if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_INIT){ SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE); }

        pDemod->sharedIf = value ? 1 : 0;
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_SCAN_MODE:  /* Scan mode on/off */
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        pDemod->isScanMode = value ? 1 : 0;
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_TS_AUTO_RATE_ENABLE:   /* Enable or disable the auto TS clock rate(0:Disable, 1:Enable[default]) */
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_TSIF_SCR_DP_AUTO */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x91, (value ? 0x01 : 0x00));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_TS_MANUAL_RATE_SETTING:   /* Set the TS clock rate manually(for Debug) */
        /* If SONY_CN_DTV_DEMOD_TS_AUTO_RATE_ENABLE = 1, then this configuration will have no effect.
            Value (DTMB/DVB-C):
            - Desired TS clock rate in kHz. */

        /* SW state check */
        if(pDemod->state != SONY_CN_DTV_DEMOD_STATE_INIT){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Argument check : Maximum allowed manual TS clock rate setting */
        if((value <= 0) || (value > 10250)) { SONY_TRACE_RETURN(SONY_RESULT_ERROR_RANGE); }

        {
            uint32_t val = 0x00;
            uint8_t data[3];

            /* Set Bank 0x00 */
            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            /* TS clock rate[MHz] = 82[MHz] / ( OREG_TSIF_SDPR[19:0] / 2^12 )
               OREG_TSIF_SDPR = (82/TSCLK) * 2^12 ;
               Notes: 82000 * (2^12) = 335872000 */
            val = (335872000u + value / 2) / value;
            data[0] = (uint8_t) ((val & 0x0F0000) >> 16);
            data[1] = (uint8_t) ((val & 0xFF00) >> 8);
            data[2] = (uint8_t) (val & 0xFF);

            result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x92, data, sizeof(data));
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_GPIO_0_DISABLE_PULLUP:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*  Slave    Bank    Addr    Bit    Default         Signal name
         * -----------------------------------------------------------
         * <SLV-Z>   00h     E0h     [0]       1'b0         OGPIO0_PUDS
         */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xE0, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_GPIO_1_DISABLE_PULLUP:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*  Slave    Bank    Addr    Bit    Default         Signal name
         * -----------------------------------------------------------
         * <SLV-Z>   00h     E0h     [1]       1'b0         OGPIO1_PUDS
         */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xE0, (uint8_t)(value ? 0x02 : 0x00), 0x02);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_GPIO_2_DISABLE_PULLUP:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*  Slave    Bank    Addr    Bit    Default         Signal name
         * -----------------------------------------------------------
         * <SLV-Z>   00h     E0h     [2]       1'b0         OGPIO2_PUDS
         */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xE0, (uint8_t)(value ? 0x04 : 0x00), 0x04);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_TSERR_ENABLE:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*
         *  Slave   Bank    Addr    Bit    Default     Setting       Signal name
         * -------------------------------------------------------------------------------
         * <SLV-Z>  00h     28h     [6]       1'b1        1'b0       OGPIO2_HIZ
         * <SLV-Z>  00h     D8h     [2]       1'b0        1'b0       OGOUTEN[2]
         */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, (uint8_t)(value ? 0x00 : 0x40), 0x40);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD8, 0x00, 0x04);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_PWM_ENABLE:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        /* Set Bank 0x00 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*
         *  Slave   Bank    Addr    Bit    Default     Setting       Signal name
         * -------------------------------------------------------------------------------
         * <SLV-Z>  00h     28h     [5]       1'b1        1'b0       OGPIO1_HIZ
         * <SLV-Z>  00h     D8h     [1]       1'b0        1'b0       OGOUTEN[1]
         * <SLV-Z>  06h     9Ch     [0]       1'b0        1'b1       OCTL_RFAGCMANUAL
         */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, (uint8_t)(value ? 0x00 : 0x20), 0x20);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD8, 0x00, 0x02);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* Set Bank 0x06 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x06);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x9C, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_PWM_VALUE:
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        if((value < 0) || (value > 0xFFF)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Set Bank 0x06 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x06);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>  06h     9Dh     [3:0]    8'h00      8'h0x     OCTL_RFAGCMANUAL_VAL[11:8]
         * <SLV-T>  06h     9Eh     [7:0]    8'h00      8'hxx     OCTL_RFAGCMANUAL_VAL[7:0]
         */
        {
            uint8_t data[2];
            data[0] = (uint8_t) (((uint16_t)value >> 8) & 0x0F);
            data[1] = (uint8_t) ((uint16_t)value & 0xFF);

            result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x9D, data, sizeof(data));
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;

    case SONY_CN_DTV_DEMOD_CONFIG_SETTING_TESTMODE_01:  /* Test mode 01 on/off */
        /* SW state check */
        if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state > SONY_CN_DTV_DEMOD_STATE_NORMAL_Z)){
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        pDemod->testMode01 = value ? 1 : 0;

        if(pDemod->state == SONY_CN_DTV_DEMOD_STATE_NORMAL_Z){
            /* Set Bank 0x35 */
            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x35);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

            result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC1, (pDemod->testMode01 ? 0x01 : 0x00));
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;

    default:
        /* Illegal Config ID */
        result = SONY_RESULT_ERROR_ARG;
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Monitor Internal Digital AGC output level

  pDemod              : Address of demodulator instance.
  pInternalDigitalAgc : Address of Internal Digital AGC ouput level.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_Monitor_InternalDigitalAgcout(sony_cn_dtv_demod_t* pDemod,
                                                              uint32_t* pInternalDigitalAgc)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t rdata[2];

    SONY_TRACE_ENTER("sony_cn_dtv_demod_Monitor_InternalDigitalAgcout");

    /* Null check */
    if(!pDemod || !pInternalDigitalAgc){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if ((pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_Z) &&
        (pDemod->state != SONY_CN_DTV_DEMOD_STATE_NORMAL_J)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x04 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x04);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /*
     *  Slave   Bank    Addr    Bit       Signal name
     * ---------------------------------------------------------------------------------
     * <SLV-Z>  04h     12h     [5:0]     ITDA_DAGC_GAIN[13:8]
     * <SLV-Z>  04h     13h     [7:0]     ITDA_DAGC_GAIN[7:0]
     */

    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x12, rdata, 2);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    *pInternalDigitalAgc = (rdata[0] & 0x3F);
    *pInternalDigitalAgc = (*pInternalDigitalAgc << 8) + rdata[1];

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  GPIO setting

  Set each GPIO to "Enable/Disable" and "Read/Write".
  Read  : Read the data input from GPIO pin.
  Write : Output the data that set by register from GPIO pin.

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  isEnable   : 0: disable, 1: enable
  isWrite    : 0: read, 1: write
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIOSetting(sony_cn_dtv_demod_t* pDemod,
                                            uint8_t id,
                                            uint8_t isEnable,
                                            uint8_t isWrite)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_GPIOSetting");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /*
                  +--------------+--------------+--------------+
                  |    GPIO 0    |    GPIO 1    |    GPIO 2    |
                  +--------------+--------------+--------------+
        Bank      |0x00|0x00|0x00|0x00|0x00|0x00|0x00|0x00|0x00|
        Addr      |0x28|0xD8|0xD7|0x28|0xD8|0xD7|0x28|0xD8|0xD7|
        Bit       | [4]| [0]| [0]| [5]| [1]| [1]| [6]| [2]| [2]|
        ----------+----+----+----+----+----+----+----+----+----+
     Disable GPIO |  1 |  0 |  - |  1 |  0 |  - |  1 |  0 |  - |
        GPI       |  1 |  1 |  1 |  1 |  1 |  1 |  1 |  1 |  1 |
        GPO       |  1 |  1 |  0 |  1 |  1 |  0 |  1 |  1 |  0 |
        (RFAGC)   |  - |  - |  - |  0 |  0 |  - |  - |  - |  - |
        TSERR     |  - |  - |  - |  - |  - |  - |  0 |  0 |  - |
     Disable TSERR|  - |  - |  - |  - |  - |  - |  1 |  0 |  - |

    */

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    switch(id)
    {
    case 0: /* GPIO0 */
        /* OGPIO0_HIZ = 1 */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x10, 0x10);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OGOUTEN[0] enable/disable */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD8, ((isEnable) ? 0x01 : 0x00), 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        if(isEnable){
            /* OGOUT_HIZ[0] */
            result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((isWrite) ? 0x00 : 0x01), 0x01);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;
    case 1: /* GPIO1 */
        /* OGPIO1_HIZ = 1 */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x20, 0x20);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OGOUTEN[1] enable/disable */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD8, ((isEnable) ? 0x02 : 0x00), 0x02);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        if(isEnable){
            /* OGOUT_HIZ[1] */
            result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((isWrite) ? 0x00 : 0x02), 0x02);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;
    case 2: /* GPIO2 */
        /* OGPIO2_HIZ = 1 */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x40, 0x40);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OGOUTEN[2] enable/disable */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD8, ((isEnable) ? 0x04 : 0x00), 0x04);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        if(isEnable){
            /* OGOUT_HIZ[2] */
            result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((isWrite) ? 0x00 : 0x04), 0x04);
            if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        }
        break;
    default: /* Illegal ID */
        result = SONY_RESULT_ERROR_ARG;
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Read GPIO value

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  pValue     : Address of the read result.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIORead(sony_cn_dtv_demod_t* pDemod,
                                         uint8_t id,
                                         uint8_t* pValue)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_GPIORead");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* sel_gin(IGPIOx) */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC0, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    switch(id)
    {
    case 0: /* GPIO 0 */
        *pValue = (data & 0x01);
        break;
    case 1: /* GPIO 1 */
        *pValue = ((data & 0x02) >> 1);
        break;
    case 2: /* GPIO 2 */
        *pValue = ((data & 0x04) >> 2);
        break;
    default: /* Illegal ID */
        result = SONY_RESULT_ERROR_ARG;
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Write GPIO value

  pDemod     : Address of demodulator instance.
  id         : GPIO ID (0/1/2)
  pValue     : The value of data output by GPIO pin.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_GPIOWrite(sony_cn_dtv_demod_t* pDemod,
                                          uint8_t id,
                                          uint8_t value)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_GPIOWrite");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    switch(id)
    {
    case 0: /* GPIO 0 */
        /* OGOUT[0] */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((value & 0x01) << 4), 0x10);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;
    case 1: /* GPIO 1 */
        /* OGOUT[1] */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((value & 0x01) << 5), 0x20);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;
    case 2: /* GPIO 2 */
        /* OGOUT[2] */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0xD7, ((value & 0x01) << 6), 0x40);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        break;
    default: /* Illegal ID */
        result = SONY_RESULT_ERROR_ARG;
        break;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
 Configure the demodulator to forward I2C messages to the
 output port for tuner control.

 pDemod      : Address of demodulator instance.
 enable      : Enable(enable != 0) / Disable I2C repeater(enable == 0)

--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_I2cRepeaterEnable(sony_cn_dtv_demod_t* pDemod,
                                                  uint8_t              enable)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data = 0x00;
    SONY_TRACE_ENTER("sony_demod_I2cRepeaterEnable");
    
    if (enable != 0) {
        data = 0x02;
    }

    /*
     *  slave    Bank    Addr    Bit    default    Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-Z>   08h     10h     [1]      8'h00     8'h02       OTHREPEN
     */

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x08);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x10, data);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Check condition1

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_CheckCondition1(sony_cn_dtv_demod_preset_info_t* pPresetInfo,
                                                uint8_t* pIsCondition1)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_CheckCondition1");
    
    /* Null check */
    if(!pIsCondition1) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Check condition1 setting */
    
    if( (pPresetInfo->carrierMode == SONY_CN_DTV_DTMB_CARRIER_MODE_C3780) && 
        (pPresetInfo->pnInfo == SONY_CN_DTV_DTMB_PN_945_VARIOUS) && 
        (pPresetInfo->systemInfo.cnst == SONY_CN_DTV_DTMB_CNST_64QAM) && 
        (pPresetInfo->systemInfo.rate == SONY_CN_DTV_DTMB_LDPC_RATE_0_6) && 
        (pPresetInfo->systemInfo.il == SONY_CN_DTV_DTMB_FEC_IL_M_720)){
        /* Condition1 case */
        *pIsCondition1 = 1;
    } else {
        *pIsCondition1 = 0;
    }
    
    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Optimizing process (eq_param_init3)

  pDemod : Address of demodulator instance.
--------------------------------------------------------------------*/
sony_result_t sony_cn_dtv_demod_OptimizeSet(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data;
    uint8_t wdata1=0;
    uint8_t wdata2=0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_OptimizeSet");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) || (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_SEQ_CMODE */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x11, &data, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(data == 0){
        /* CarrierMode C1 */
        wdata1 = 0x07;
        wdata2 = 0x0B;
    }else{
        /* CarrierMode C3780 */
        wdata1 = 0x0C;
        wdata2 = 0x11;
    }

    /* Set Bank 0x38 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x38);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /*
     *   Slave   Bank    Addr    Bit    Default     Setting       Signal name
     *  ---------------------------------------------------------------------------------
     *  <SLV-Z>  38h     83h     [3:0]    4'h0C       8'h**       OREG_EQ_PARAMETER_38_83
     */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x83, wdata1, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x3C */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x3C);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /*
     *   Slave   Bank    Addr    Bit    Default     Setting       Signal name
     *  ---------------------------------------------------------------------------------
     *  <SLV-Z>  3Ch     5Ch     [4:0]    5'h11       8'h**       OREG_EQ_PARAMETER_3C_5C
     */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x5C, wdata2, 0x1F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  check possibility of c1 early detection

  pDemod : Address of demodulator instance.
  pPossibility : signal not detect or early detection(lock) or check again(unlock)
--------------------------------------------------------------------*/

sony_result_t sony_cn_dtv_demod_CheckC1EarlyDetectionPossibility(sony_cn_dtv_demod_t* pDemod, uint8_t *pPossibility)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t unlockDetection = 0;
    uint8_t seqState = 0;
    SONY_TRACE_ENTER("sony_cn_dtv_demod_CheckC1EarlyDetectionPossibility");

    /* NULL check */
    if (!pDemod) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if ((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) ||
        (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* read IREG_AR_SCANDONE_SHOLD */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x1F, &unlockDetection, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((unlockDetection & 0x01)) {
        *pPossibility = SONY_CN_DTV_DEMOD_LOCK_STATUS_UNLOCK;
        SONY_TRACE_RETURN(SONY_RESULT_OK);
    }

    /* IREG_SEQ_STATE[5:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x10, &seqState, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if ((seqState & 0x3F) >= 9) {
        *pPossibility = SONY_CN_DTV_DEMOD_LOCK_STATUS_LOCK;
    } else {
        *pPossibility = SONY_CN_DTV_DEMOD_LOCK_STATUS_NOT_DETECT;
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Early Detect C1

  pDemod : Address of demodulator instance.
  pIsAssumed : c1 detect or not detect
--------------------------------------------------------------------*/

sony_result_t sony_cn_dtv_demod_C1EarlyDetect(sony_cn_dtv_demod_t* pDemod, uint8_t *pIsAssumed)
{

    sony_result_t result = SONY_RESULT_OK;
    uint8_t pn = 0;

    SONY_TRACE_ENTER("sony_cn_dtv_demod_C1EarlyDetect");

    /* NULL check */
    if ((!pDemod) ||
        (!pIsAssumed)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* SW state check */
    if ((pDemod->state < SONY_CN_DTV_DEMOD_STATE_INIT) ||
        (pDemod->state == SONY_CN_DTV_DEMOD_STATE_INVALID)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* IREG_PN[2:0] */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddress, 0x13, &pn, 1);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    if ((pn & 0x07) == 0x01) { /* In the case PN595, it is assumed as C1. */
        /* OREG_CMODE = 0 */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x80, 0x00, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = sony_cn_dtv_demod_SoftReset(pDemod);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        *pIsAssumed =1;
    } else {
        *pIsAssumed =0;
    }

    SONY_TRACE_RETURN(SONY_RESULT_OK);
}


/*------------------------------------------------------------------------------
  static functions
------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  setEqParam

  pDemod       : Address of demodulator instance.
  isCondition1 : 0: non Condition1, 1: Condition1
--------------------------------------------------------------------*/
static sony_result_t setEqParam(sony_cn_dtv_demod_t* pDemod, uint8_t isCondition1)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data_b34_dc;
    uint8_t data_b39_3d[17];
    uint8_t data_b39_50[3];
    uint8_t data_b3a_4e;
    uint8_t data_b3a_50[4];
    uint8_t data_b3a_56;
    uint8_t data_b3a_58[4];

    SONY_TRACE_ENTER("setEqParam");

    /* Null check */
    if(!pDemod){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if( isCondition1 ){
        /* eq_param_init1 */
        data_b34_dc = 0x01;
        data_b39_3d[0]=0x01; data_b39_3d[1]=0x00; data_b39_3d[2]=0x0C; data_b39_3d[3]=0x00; data_b39_3d[4]=0x0E; data_b39_3d[5]=0x00;
        data_b39_3d[6]=0x20; data_b39_3d[7]=0x00; data_b39_3d[8]=0x80; data_b39_3d[9]=0x00; data_b39_3d[10]=0x90; data_b39_3d[11]=0x00;
        data_b39_3d[12]=0xA0; data_b39_3d[13]=0x9A; data_b39_3d[14]=0x88; data_b39_3d[15]=0x77; data_b39_3d[16]=0x77;
        data_b39_50[0]=0xFF; data_b39_50[1]=0xFF; data_b39_50[2] = 0x08;
        data_b3a_4e = 0x07;
        data_b3a_50[0]=0x0A; data_b3a_50[1]=0x0A; data_b3a_50[2]=0x0A; data_b3a_50[3]=0x0A;
        data_b3a_56 = 0x07;
        data_b3a_58[0]=0x0A; data_b3a_58[1]=0x0A; data_b3a_58[2]=0x0A; data_b3a_58[3]=0x0A;
    }else{
        /* eq_param_init2 */
        data_b34_dc = 0x04;
        data_b39_3d[0]=0x00; data_b39_3d[1]=0x01; data_b39_3d[2]=0x00; data_b39_3d[3]=0x03; data_b39_3d[4]=0x00; data_b39_3d[5]=0x08;
        data_b39_3d[6]=0x00; data_b39_3d[7]=0x0A; data_b39_3d[8]=0x00; data_b39_3d[9]=0x0C; data_b39_3d[10]=0x00; data_b39_3d[11]=0x18;
        data_b39_3d[12]=0x00; data_b39_3d[13]=0xAA; data_b39_3d[14]=0x89; data_b39_3d[15]=0x67; data_b39_3d[16]=0x56;
        data_b39_50[0]=0xEF; data_b39_50[1]=0xCD; data_b39_50[2] = 0x0E;
        data_b3a_4e = 0x05;
        data_b3a_50[0]=0x0D; data_b3a_50[1]=0x0F; data_b3a_50[2]=0x0F; data_b3a_50[3]=0x10;
        data_b3a_56 = 0x05;
        data_b3a_58[0]=0x0D; data_b3a_58[1]=0x0F; data_b3a_58[2]=0x0F; data_b3a_58[3]=0x10;
    }

    /* Set Bank 0x34 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x34);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xDC, data_b34_dc);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x39 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x39);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x3D, data_b39_3d, sizeof(data_b39_3d));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x50, data_b39_50, sizeof(data_b39_50));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x3A */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x3A);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x4E, data_b3a_4e);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x50, data_b3a_50, sizeof(data_b3a_50));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x56, data_b3a_56);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x58, data_b3a_58, sizeof(data_b3a_58));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  SD2SL (Shutdown -> Sleep)
  I2C state transition sequence. 

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t SD2SL(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("SD2SL");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Slave address of xtal part */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress+4, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x0F */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress+4, 0x00, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Assert SW reset to Xtal part */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress+4, 0x14, 0x01);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Select 20.5MHz Xtal */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress+4, 0x11, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Negate SW reset to Xtal part */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress+4, 0x14, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Wait for PLL stabilization */
    SONY_SLEEP(1);

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  SL2NOZ (Sleep -> Normal-Z)
  I2C state transition sequence. 
  But except for the process after "Softreset".

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t SL2NOZ(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data[5];

    SONY_TRACE_ENTER("SL2NOZ");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DTMB demod setting ---*/
    /* OREG_DVBCMODE(dvbcmode): 0=DTMB demodulation mode */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x24, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DTMB DemodClock enable ---*/
    /* clkdisable */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x25, 0x02);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ADC enable ---*/
    /* if_adc_ctrl0 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x26, 0x07);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Memory enable ---*/
    /* OREG_DRAM_ENABLE(dram_enable) */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x27, 0x01);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /*--- DTMB demod parameter setting ---*/
    /* Setting for IF Frequency ---*/
    data[0] = (uint8_t)(pDemod->iffreq_config.config_DTMB_U3B & 0xFF);
    data[1] = (uint8_t)((pDemod->iffreq_config.config_DTMB_L32B >> 24) & 0xFF);
    data[2] = (uint8_t)((pDemod->iffreq_config.config_DTMB_L32B >> 16) & 0xFF);
    data[3] = (uint8_t)((pDemod->iffreq_config.config_DTMB_L32B >> 8) & 0xFF);
    data[4] = (uint8_t)(pDemod->iffreq_config.config_DTMB_L32B & 0xFF);

    /* OREG_DDC_PHASE_INCR[34:0](ddc_phase_icr_X) */
    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x88, data, sizeof(data));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* OREG_DDC_PHASE_INCR_PRE[34:0](ddc_phase_icr_pre_X) */
    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC5, data, sizeof(data));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ASCOT setting ---*/
    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(pDemod->isAscot >= SONY_CN_DTV_DEMOD_TUNER_ASCOT2D ){
        /* Default setting for these registers is the most suitable. */
        /* ASCOT setting ON ---*/
        /* OREG_ITB_GDEQ_EN */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x6A, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IFAGC polarity setting ---*/
        /* OCTL_IFAGCNEG */
        /* set by SetConfig(ID=SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG) */

        /* ADC setting ---*/
        /* OADCGAIN[1:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x21, 0xD6);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }else{
        /* If RF is NOT ASCOT, need to set to these registers. Refer to the application note. */
        /* ASCOT setting OFF ---*/
        /* OREG_ITB_GDEQ_EN */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x6A, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* IFAGC polarity setting ---*/
        /* OCTL_IFAGCNEG */
        /* set by SetConfig(ID=SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG) */

        /* ADC setting ---*/
        /* OADCGAIN[1:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x21, 0xB6);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* The filter settings for ASCOT2x ---*/
    if(pDemod->isAscot >= SONY_CN_DTV_DEMOD_TUNER_ASCOT2D){
        /* Addr 6Bh-78h */
        const uint8_t data_00_6b[]={0x0E, 0xC5, 0x15, 0xB4, 0x22, 0xA6, 0x26, 0xB9, 0x1E, 0xB2, 0x2C, 0xC5, 0x22, 0xB2};

        /* OREG_ITB_COEFxx[7:0] */
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x6B, data_00_6b, sizeof(data_00_6b));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    }else{ /* RF is NOT ASCOT */
        /* Do nothing */
    }

    /* Set Bank 0x30 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x30);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Setting for Timing Recovery ---*/
    {
        /* Bank 0x30, SubAddress 0xB4-0xB7 OREG_INTERP_NOMINAL[27:0](interp_nominal_X) */
        const uint8_t data_30_b4[]={0x00, 0x45, 0x14, 0x51};
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB4, data_30_b4, sizeof(data_30_b4));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Setting for Equalizer Optimization ---*/
    {
        /* Set Bank 0x32 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x32);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_REPSEQ_S110C_TIMER */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x89, 0x82);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_REPSEQAR_RISIC_GIVEUP_TH[10:8] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x9A, 0x03);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* OREG_REPSEQAR_RISIC_GIVEUP_TH[7:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x9B, 0xFF);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* Set Bank 0x38 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x38);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_EQ_MU_PN_MC_PN_ACQ_595[3:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x83, 0x07);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* OREG_EQ_MUA_PN_MC_PN_TRK_595[3:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x86, 0xB0);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* Set Bank 0x3C */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x3C);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /* OREG_EQ_LEAKA_PN_ACQ_MC_595[4:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x54, 0x09);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* OREG_EQ_LEAKA_PN_TRK_MC_595[4:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x55, 0x0D);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* OREG_EQ_LEAKNA_PN_ACQ_MC_595[4:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x5C, 0x0B);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* OREG_EQ_LEAKNA_PN_TRK_MC_595[4:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x5D, 0x12);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Setting for Equalizer Optimization2 */
    {
        uint8_t data_3a_64[8] = {0x10, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
        uint8_t data_3a_74[5] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F};
        /* Set Bank 0x3A */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x3A);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x64, data_3a_64, sizeof(data_3a_64));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x74, data_3a_74, sizeof(data_3a_74));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Setting for Equalizer Optimization3 */
    {
        /* Set Bank 0x34 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x34);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC2, 0x1A);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xD2, 0x82);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        /* Set Bank 0x38 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x38);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB5, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB6, 0xC0);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB9, 0x02);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xBA, 0x71);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }
    
    /* Setting for Start-up X'tal Fluctuation */
    {
        /* Set Bank 0x31 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x31);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xCC, 0x0C);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Setting for Short-echo Detection */
    {
        /* Set Bank 0x38 */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x38);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xB1, 0x14);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Setting for Adjustment of Noise Estimation */
    {
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x9C, 0x09);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xA4, 0xC8);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* Set Bank 0x35 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x35);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0xC1, (pDemod->testMode01 ? 0x01 : 0x00));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* TS output disable */
    result = sony_cn_dtv_demod_TsOutputEnable(pDemod, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Hi-Z setting disable ---*/
    /* tsoutctrl0 (set OIFAGC_HIZ/OTSVALID_HIZ/OTSSYNC_HIZ/OTSCLK_HIZ to 0) */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x00, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* TSDATA7-0 Hi-Z setting disable ---*/
    /* OTSDT_HIZ[7:0](tsoutctrl1) */
    {
        uint8_t dataT = 0x00;
        if((pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_DATAONLY) ||
           (pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_CONTINUOUS)){
            dataT = 0xFE;
        }else if((pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_DATAONLY) ||
                 (pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_CONTINUOUS)){
            dataT = 0x7F;
        }else{  /* Parallel */
            dataT = 0x00;
        }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x29, dataT);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  NOZ2SL (Normal-Z -> Sleep)
  I2C state transition sequence. 

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t NOZ2SL(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("NOZ2SL");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* TS output disable ---*/
    /* OTSDTDISABLE */
    result = sony_cn_dtv_demod_TsOutputEnable(pDemod, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* TSDATA7-0 Hi-Z setting ---*/
    /* OTSDT_HIZ[7:0](tsoutctrl1) */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x29, 0xFF);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Hi-Z setting ---*/
    /* tsoutctrl0 (set OIFAGC_HIZ/OTSVALID_HIZ/OTSSYNC_HIZ/OTSCLK_HIZ to 1) */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x0F, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ADC disable ---*/
    /* if_adc_ctrl0 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x26, (pDemod->sharedIf ? 0x05 : 0x00));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Memory disable ---*/
    /* OREG_DRAM_ENABLE(dram_enable) */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x27, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DTMB DemodClock disable ---*/
    /* clkdisable */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x25, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  SL2NOJ (Sleep -> Normal-J)
  I2C state transition sequence. 
  But except for the process after "Softreset".

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t SL2NOJ(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data[2];

    SONY_TRACE_ENTER("SL2NOJ");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DVB-C demod setting ---*/
    /* OREG_DVBCMODE(dvbcmode), 1=DVB-C demodulation mode */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x24, 0x01);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DVB-C Demod clock enable ---*/
    /* clkdisable */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x25, 0x0D);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ADC enable ---*/
    /* if_adc_ctrl0 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x26, 0x07);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /*--- DVB-C demod parameter setting ---*/
    /* Setting for IF Frequency */
    data[0] = (uint8_t)((pDemod->iffreq_config.config_DVBC >> 8) & 0x3F);
    data[1] = (uint8_t)(pDemod->iffreq_config.config_DVBC & 0xFF);

    /* OREG_ITB_DWNCONVER_FRQENCY[13:0](itb_freq_hi, itb_freq_lo) */
    result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x8D, data, sizeof(data));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    {
        
        uint8_t data_00_68[2] = {0x09, 0x00};
        /*
         *   Slave   Bank    Addr    Bit    Default     Setting       Signal name
         *  -------------------------------------------------------------------------------
         *  <SLV-Z>  00h     68h     [7:0]  8'h08       8'h09          OREG_ITB_DAGC_MGAIN_DVBC[15:8]
         *  <SLV-Z>  00h     69h     [7:0]  8'hd0       8'h00          OREG_ITB_DAGC_MGAIN_DVBC[7:0]
         */
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x68, data_00_68, sizeof(data_00_68));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

        /*
         *   Slave   Bank    Addr    Bit    Default     Setting       Signal name
         *  -------------------------------------------------------------------------------
         *  <SLV-Z>  00h     32h     [7:0]  8'h48       8'h50          OCTL_IFAGC_TARGET_DVBC[7:0]
         */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x32, 0x50);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }
    /* ASCOT setting */
    if(pDemod->isAscot >= SONY_CN_DTV_DEMOD_TUNER_ASCOT2D){
        /* Default setting for these registers is the most suitable. */
        /* ASCOT setting ON ---*/
        /* OREG_ITB_GDEQ_EN_DVBC */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x79, 0x01);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        
        /* IFAGC polarity setting ---*/
        /* OCTL_IFAGCNEG */
        /* set by SetConfig(ID=SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG) */
        
        /* ADC setting ---*/
        /* OADCGAIN[1:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x21, 0xD6);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }else{
        /* If RF is NOT Ascot, need to set to these registers. Refer to the application note. */
        /* ASCOT setting OFF ---*/
        /* OREG_ITB_GDEQ_EN_DVBC */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x79, 0x00);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
        
        /* IFAGC polarity setting ---*/
        /* OCTL_IFAGCNEG */
        /* set by SetConfig(ID=SONY_CN_DTV_DEMOD_CONFIG_OCTL_IFAGCNEG) */
        
        /* ADC setting ---*/
        /* OADCGAIN[1:0] */
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x21, 0xB6);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* The filter settings for ASCOT2x ---*/
    if(pDemod->isAscot >= SONY_CN_DTV_DEMOD_TUNER_ASCOT2D){
        /* Addr 7Ah-87h */
        const uint8_t data_00_7a[]={0x2D, 0xC7, 0x04, 0xF4, 0x07, 0xC5, 0x2A, 0xB8, 0x27, 0x9E, 0x27, 0xA4, 0x29, 0xAB};

        /* OREG_ITB_COEFxx_DVBC[7:0] */
        result = pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddress, 0x7A, data_00_7a, sizeof(data_00_7a));
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    }else{ /* RF is NOT ASCOT */
        /* Do nothing */
    }

    /* Set Bank 0x40 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x40);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    if(!pDemod->isScanMode){
        /* Normal Tuning */
        /* OREG_AR_SCANNING */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x86, 0x00, 0x20);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }else{
        /* Scan Mode */
        /* OREG_AR_SCANNING */
        result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x86, 0x20, 0x20);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    /* TS output disable*/
    result = sony_cn_dtv_demod_TsOutputEnable(pDemod, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    
    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Hi-Z setting disable ---*/
    /* tsoutctrl0 (set OIFAGC_HIZ/OTSVALID_HIZ/OTSSYNC_HIZ/OTSCLK_HIZ to 0) */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x00, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* TSDATA7-0 Hi-Z setting disable ---*/
    /* OTSDT_HIZ[7:0](tsoutctrl1) */
    {
        uint8_t dataT = 0x00;
        if((pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_DATAONLY) ||
           (pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_0_CONTINUOUS)){
            dataT = 0xFE;
        }else if((pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_DATAONLY) ||
                 (pDemod->tsOutput == SONY_CN_DTV_DEMOD_TS_OUTPUT_SERIAL_7_CONTINUOUS)){
            dataT = 0x7F;
        }else{  /* Parallel */
            dataT = 0x00;
        }
        result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x29, dataT);
        if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }
    }

    SONY_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  NOJ2SL (Normal-J -> Sleep)
  I2C state transition sequence. 

  pDemod     : Address of demodulator instance.
--------------------------------------------------------------------*/
static sony_result_t NOJ2SL(sony_cn_dtv_demod_t* pDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER("NOJ2SL");

    /* Null check */
    if(!pDemod ){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* TS output disable ---*/
    result = sony_cn_dtv_demod_TsOutputEnable(pDemod, 0);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Set Bank 0x00 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x00, 0x00);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* TSDATA7-0 Hi-Z setting ---*/
    /* OTSDT_HIZ[7:0](tsoutctrl1) */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x29, 0xFF);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* Hi-Z setting ---*/
    /* tsoutctrl0 (set OIFAGC_HIZ/OTSVALID_HIZ/OTSSYNC_HIZ/OTSCLK_HIZ to 1) */
    result = sony_i2c_SetRegisterBits_DTMB(pDemod->pI2c, pDemod->i2cAddress, 0x28, 0x0F, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* ADC disable ---*/
    /* if_adc_ctrl0 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x26, (pDemod->sharedIf ? 0x05 : 0x00));
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    /* DVB-C Demod clock disable ---*/
    /* clkdisable */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddress, 0x25, 0x0F);
    if(result != SONY_RESULT_OK){ SONY_TRACE_RETURN(result); }

    SONY_TRACE_RETURN(result);
}

