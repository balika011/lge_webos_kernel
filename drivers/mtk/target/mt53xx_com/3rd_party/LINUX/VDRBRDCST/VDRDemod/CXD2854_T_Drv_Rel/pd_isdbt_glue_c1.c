/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_isdbt_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_isdbt_glue.c
 *  ISDBT glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifndef SONY_DUMMY_DEBUG
#include "u_tuner.h"
#include "x_os.h"
#include "x_stl_lib.h"//StrToInt function
#include "tuner_if.h"
#include "TDIS_header.h"
#include "pd_isdbt_glue.h"
#include "tunerDebug.h"
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "drvcust_if.h"//Get Tuner/Demod I2C bus id
#include "x_gpio.h"
#include "u_drv_cust.h"
#include "vdr_PD_Def.h"
#else // SONY_DUMMY_DEBUG
#include "MTKdef.h" // MTK dummy definitions
#include "pd_isdbt_glue.h"
#endif // SONY_DUMMY_DEBUG

// for CXD2854 driver
#include "sony_i2c_MTK.h"
#include "sony_demod.h"
#include "sony_demod_isdbt.h"
#include "sony_demod_isdbt_monitor.h"
#include "sony_integ_isdbt.h"
#include "sony_math.h"

#include "sony_mtk_glue_common.h"

#define SONY_CXD2854_ISDBT_DRIVER_VERSION_STR "Sony CXD2854 (ISDB-T) v0.02 2014-03-07"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//Import extern variable
 
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
//#define SONY_DEMOD_IIC_ADDRESS 0xD8 /* 8bit form */
//-----------------------------------------------------------------------------
// Export definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define mcSEMA_LOCK_MTFE(sema)                                       \
{                                                                    \
    mcDBG_MSG_L4((" [ _Lock_%s-%d ]\r\n", __FUNCTION__, __LINE__)); \
    x_sema_lock(sema, X_SEMA_OPTION_WAIT);                           \
    mcDBG_MSG_L4((" [ _Excape_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
}
#define mcSEMA_UNLOCK_MTFE(sema)                                     \
{                                                                    \
    mcDBG_MSG_L4((" [ _Unlock_%s-%d ]\r\n", __FUNCTION__, __LINE__));     \
    x_sema_unlock(sema);                                             \
}

//-----------------------------------------------------------------------------
// static variables
//-----------------------------------------------------------------------------
static ISDBT_PD_CTX_T *pPdCtx;
static TUNER_DEVICE_CTX_T*  pIsdbtTunerDeviceCtx = NULL;

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------
static VOID vResetDemod(UINT16 u2DelayTime)
{
    UINT32 u4RstPinNum;

    // Config GPIO to reset demod
    mcDBG_MSG_L2(("vResetDemod\n"));
    if (DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum) == 0)
    {
        GPIO_SetOut(u4RstPinNum, 0);
        x_thread_delay((UINT32)u2DelayTime);
        GPIO_SetOut(u4RstPinNum, 1);
        x_thread_delay((UINT32)20); // Delay 20 ms for CR[DTV00030459]. Or it will init fail in rel image
        mcDBG_MSG_L2(("Reset ISDBT Demod GPIO=0x%x, 0->1, %dms\r\n",u4RstPinNum, u2DelayTime));
    }
}

static BOOL SetBreakMonitor(ISDBT_PD_CTX_T *psPdCtx)
{
    UINT8 u1Cnt=0;
    
    psPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psPdCtx->fgPIMoniStatus){  //PI really doing PI Monitor
        while(psPdCtx->fgPIMoniStatus){ 
            //wait PI Monitor really finish break 
            if(u1Cnt>250){  //in case of deadlock
                break;   
            }
            u1Cnt++;
            mcDBG_MSG_L2(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
            //  mcDELAY_MS(10); 
            x_thread_delay((UINT32)(10));
        }             
    }
    
    return TRUE;
}

static void ResumeMonitor(ISDBT_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgBreakMonitor = FALSE;
    mcSEMA_UNLOCK_MTFE(psPdCtx->t_escape_mon_suspend);
}

//-----------------------------------------------------------------------------
// Sony specific
//-----------------------------------------------------------------------------
static BOOL sony_GetTsFmt(sony_demod_t *pDemod, TS_FMT_T *pTsfmt)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    UINT8 data = 0;

    // Set Bank 0
    sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00);
    if(sony_result != SONY_RESULT_OK){ return 0; }

    // OSERIALEN (Bank:00h, Addr:C4h, Bit[7])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->SerOrPar = (data & 0x80) ? SP_SERIAL : SP_PARALLEL;

    // OSERCKMODE[1:0] (Bank:00h, Addr:C4h, Bit[1:0]
    pTsfmt->fgGatedClock = ((data & 0x03) == 0x03) ? BOOL_TRUE : BOOL_FALSE;

    // OWFMT_CKINV (Bank:00h, Addr:C5h, Bit[0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC5, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->ClockPolarity = (data & 0x01) ? POL_HIGH_ACTIVE : POL_LOW_ACTIVE;

    // OTSDTDISABLE (Bank:00h, Addr:C3h, Bit[0])
    sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, &data, 1);
    if(sony_result != SONY_RESULT_OK){ return 0; }
    pTsfmt->fgTristate = (data & 0x01) ? BOOL_FALSE : BOOL_TRUE ;

    pTsfmt->u4TsClockInKHz = 82000;

    pTsfmt->fgExternalDemod = BOOL_TRUE;

    return 1;
}

//-----------------------------------------------------------------------------
// ISDB-T SQI (NOT FORMAL)
//-----------------------------------------------------------------------------
/**
 @brief The following Quality monitor is provided as an example only as it is not based
        on a formal specification. It is therefore advised that this is only used for
        guidance, rather than an exact representation. 
 
        For improved accuracy the C/N ref table values can be characterised on your own
        implementation.  ARIB specification values are currently used, but may not give
        an accurate representation of your specific hardware capabilities.

 @param pDemod The demodulator instance.
 @param pQualityA The quality of Layer A as a percentage (0-100).
 @param pQualityB The quality of Layer B as a percentage (0-100). If this layer is not exist, 0 is set.
 @param pQualityC The quality of Layer C as a percentage (0-100). If this layer is not exist, 0 is set.

 @return SONY_RESULT_OK if successful and pQuality valid.
*/

static uint8_t calc_Quality (sony_isdbt_modulation_t modulation, sony_isdbt_coding_rate_t codeRate, int32_t snr, uint32_t ber)
{
    int32_t snrRel = 0;
    uint32_t berSQI = 0;
    uint8_t quality = 0;

    static const int32_t snrNordigP1dB1000[4][5] = {
    /*   1/2,   2/3,    3/4,    5/6,    7/8  */
        {6200,  7700,   8700,   9600,   10400 }, /* DQPSK */
        {4900,  6600,   7500,   8500,   9100  }, /* QPSK  */
        {11500, 13500,  14600,  15600,  16200 }, /* 16-QAM */
        {16500, 18700,  20100,  21300,  22000 }, /* 64-QAM */
    };
    /* Ensure correct TMCC info. */
    if ((codeRate > SONY_ISDBT_CODING_RATE_7_8) || (modulation > SONY_ISDBT_MODULATION_64QAM)) {
        return 0;
    }

    /* BER_SQI Calculated from:
     * if (Pre-RS BER > 10^-4)            BER_SQI = 0
     * if (10^-4 >= Pre-RS BER >= 10^-7)  BER_SQI = 100/15 = 6.667
     * if (Pre-RS BER < 10^-7)            BER_SQI = 100/6  = 16.667
     *
     * Note : Pre-RS BER is scaled by 10^7
     */
    if (ber > 1000) {
        berSQI = 0;
    } 
    else if (ber >= 1) {
        berSQI = 6667;
    } 
    else {
        berSQI = 16667;
    }

    /* C/Nrel = C/Nrec - C/Nnordigp1 */
    snrRel = snr - snrNordigP1dB1000[modulation][codeRate];

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -3dB)         SQI = 0
     * if (-3dB <= CNrel <= 3dB)  SQI = (C/Nrel + 3) * BER_SQI 
     * if (CNrel > 3dB)           SQI = 100
     */
    if (snrRel < -3000) {
        quality = 0;
    } 
    else if (snrRel <= 3000) {
        /* snrRel and berSQI scaled by 10^3 so divide by 10^6 */
        uint32_t tempSQI = (((snrRel + 3000) * berSQI) + 500000) / 1000000;
        /* Clip value to 100% */
        quality = (tempSQI > 100)? 100 : (uint8_t) tempSQI;
    } 
    else {
        quality = 100;
    }

    return quality;
}
static sony_result_t sony_demod_isdbt_monitor_Quality (sony_demod_t * pDemod, uint8_t * pQualityA, uint8_t * pQualityB, uint8_t * pQualityC)
{
    sony_result_t result = SONY_RESULT_OK;
    int32_t snr = 0;
    uint32_t ber = 0;
    sony_isdbt_tmcc_info_t tmccInfo;

    SONY_TRACE_ENTER ("sony_demod_isdbt_monitor_Quality");

    if ((!pDemod) || (!pQualityA) || (!pQualityB) || (!pQualityC)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    *pQualityA = 0;
    *pQualityB = 0;
    *pQualityC = 0;

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get TMCC Info */
    result = sony_demod_isdbt_monitor_TMCCInfo (pDemod, &tmccInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR */
    result = sony_demod_isdbt_monitor_SNR (pDemod, &snr);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get Layer A Quality */
    if ((tmccInfo.currentInfo.layerA.modulation <= SONY_ISDBT_MODULATION_64QAM) &&
        (tmccInfo.currentInfo.layerA.codingRate <= SONY_ISDBT_CODING_RATE_7_8)) {

        result = sony_demod_isdbt_monitor_PreRSBER (pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
        if (result == SONY_RESULT_OK) {
            *pQualityA = calc_Quality (tmccInfo.currentInfo.layerA.modulation, tmccInfo.currentInfo.layerA.codingRate, snr, ber);
        } else {
            *pQualityA = 0;
        }
    }

    /* Get Layer B Quality */
    if ((tmccInfo.currentInfo.layerB.modulation <= SONY_ISDBT_MODULATION_64QAM) &&
        (tmccInfo.currentInfo.layerB.codingRate <= SONY_ISDBT_CODING_RATE_7_8)) {

        result = sony_demod_isdbt_monitor_PreRSBER (pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
        if (result == SONY_RESULT_OK) {
            *pQualityB = calc_Quality (tmccInfo.currentInfo.layerB.modulation, tmccInfo.currentInfo.layerB.codingRate, snr, ber);
        } else {
            *pQualityB = 0;
        }
    }

    /* Get Layer C Quality */
    if ((tmccInfo.currentInfo.layerC.modulation <= SONY_ISDBT_MODULATION_64QAM) &&
        (tmccInfo.currentInfo.layerC.codingRate <= SONY_ISDBT_CODING_RATE_7_8)) {

        result = sony_demod_isdbt_monitor_PreRSBER (pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
        if (result == SONY_RESULT_OK) {
            *pQualityC = calc_Quality (tmccInfo.currentInfo.layerC.modulation, tmccInfo.currentInfo.layerC.codingRate, snr, ber);
        } else {
            *pQualityC = 0;
        }
    }

    SONY_TRACE_RETURN (result);
}

//-----------------------------------------------------------------------------
// ISDB-T Layer Information
//-----------------------------------------------------------------------------
typedef enum {
    SONY_DEMOD_ISDBT_LAYER_STATE_OK,       /* OK */
    SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR, /* Reed Solomon Decode Error */
    SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED,   /* Layer is unused */
    SONY_DEMOD_ISDBT_LAYER_STATE_INVALID   /* Invalid (not demod locked etc) */
} sony_demod_isdbt_layer_state_t;
static sony_result_t sony_demod_isdbt_monitor_LayerState (sony_demod_t *pDemod,
                                                          sony_demod_isdbt_layer_state_t *pLayerA,
                                                          sony_demod_isdbt_layer_state_t *pLayerB,
                                                          sony_demod_isdbt_layer_state_t *pLayerC)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_isdbt_tmcc_info_t tmccInfo;
    uint8_t data = 0;

    SONY_TRACE_ENTER ("sony_demod_isdbt_monitor_LayerState");

    if ((!pDemod) || (!pLayerA) || (!pLayerB) || (!pLayerC)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    *pLayerA = SONY_DEMOD_ISDBT_LAYER_STATE_INVALID;
    *pLayerB = SONY_DEMOD_ISDBT_LAYER_STATE_INVALID;
    *pLayerC = SONY_DEMOD_ISDBT_LAYER_STATE_INVALID;

    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE_T_C) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = sony_demod_isdbt_monitor_TMCCInfo (pDemod, &tmccInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* 
     * slave    Bank    Addr    Bit              Name                meaning
     * --------------------------------------------------------------------------------
     * <SLV-T>   60h     11h    [4:0]           IFEC_RSERR        [3]:A, [2]:B, [1]:C
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, &data, 1);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Layer A Check */
    if (tmccInfo.currentInfo.layerA.modulation == SONY_ISDBT_MODULATION_UNUSED_7) {
        *pLayerA = SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED;
    } else {
        if (data & 0x08) {
            *pLayerA = SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR;
        } else {
            *pLayerA = SONY_DEMOD_ISDBT_LAYER_STATE_OK;
        }
    }

    /* Layer B Check */
    if (tmccInfo.currentInfo.layerB.modulation == SONY_ISDBT_MODULATION_UNUSED_7) {
        *pLayerB = SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED;
    } else {
        if (data & 0x04) {
            *pLayerB = SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR;
        } else {
            *pLayerB = SONY_DEMOD_ISDBT_LAYER_STATE_OK;
        }
    }

    /* Layer C Check */
    if (tmccInfo.currentInfo.layerC.modulation == SONY_ISDBT_MODULATION_UNUSED_7) {
        *pLayerC = SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED;
    } else {
        if (data & 0x02) {
            *pLayerC = SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR;
        } else {
            *pLayerC = SONY_DEMOD_ISDBT_LAYER_STATE_OK;
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

#ifdef CC_PROJECT_FOR_SONY
/*
*This function convert modulation type.
*Remove Lock&Unlock to avoid Hung up compared with ISDBT_TunerGetSignal
*/
static void ISDBT_TunerGetDemodData(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_isdbt_tmcc_info_t tmccInfo;
    sony_isdbt_modulation_t mod;
    sony_isdbt_mode_t mode;
    sony_isdbt_guard_t guard;
    sony_demod_isdbt_layer_state_t layerAState;
    sony_demod_isdbt_layer_state_t layerBState;
    sony_demod_isdbt_layer_state_t layerCState;
    UINT8 codenum;
    uint32_t ber = 0;
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));

    // mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    
    //add driver implementation code here
    _pSignal->Frequency = g_sony_demod_ctx.frequency;
    switch(pDemod->bandwidth){
    case SONY_DTV_BW_6_MHZ:
        _pSignal->e_bandwidth = BW_6_MHz; break;
    case SONY_DTV_BW_7_MHZ:
        _pSignal->e_bandwidth = BW_7_MHz; break;
    case SONY_DTV_BW_8_MHZ:
        _pSignal->e_bandwidth = BW_8_MHz; break;
    default:
        _pSignal->e_bandwidth = BW_UNKNOWN; break;
    }

    //TODO GET Mode and Guard   

    sony_result = sony_demod_isdbt_monitor_ModeGuard(pDemod, &mode, &guard);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_ModeGuard (%s)\n", sony_FormatResult(sony_result)));
    }

    switch(mode){
    case SONY_ISDBT_MODE_1:       /**< ISDB-T Mode 1 (Number of carriers = 1405) */
        _pSignal->u1Mode_Num=0;
        break;
    case SONY_ISDBT_MODE_2:       /**< ISDB-T Mode 2 (Number of carriers = 2809) */
        _pSignal->u1Mode_Num=1;
        break;
    case SONY_ISDBT_MODE_3:       /**< ISDB-T Mode 3 (Number of carriers = 5617) */
        _pSignal->u1Mode_Num=2;
        break;
    case SONY_ISDBT_MODE_UNKNOWN:
        _pSignal->u1Mode_Num=3;
        break;
    default:
        _pSignal->u1Mode_Num=0;
        break;
    }

    switch(guard){
    case SONY_ISDBT_GUARD_1_32:
        _pSignal->u1Guard_Num=0;
        break;
    case SONY_ISDBT_GUARD_1_16:
        _pSignal->u1Guard_Num=1;
        break;
    case SONY_ISDBT_GUARD_1_8:
        _pSignal->u1Guard_Num=2;
        break;
    case SONY_ISDBT_GUARD_1_4:
        _pSignal->u1Guard_Num=3;
        break;
    case SONY_ISDBT_GUARD_UNKNOWN:
        _pSignal->u1Guard_Num=4;
        break;          
    default:
        _pSignal->u1Guard_Num=0;
        break;  
    }

    sony_result = sony_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo); //TODO Get QAMSize from demod driver
    if(sony_result != SONY_RESULT_OK){        
        mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_TMCCInfo (%s)\n", sony_FormatResult(sony_result)));
    }

    sony_result =sony_demod_isdbt_monitor_LayerState(pDemod,&layerAState,&layerBState,&layerCState);
    if(sony_result != SONY_RESULT_OK){
         mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_LayerState (%s)\n", sony_FormatResult(sony_result)));
    }
        
    switch(_pSignal->u1LayerNum){
    case ISDBT_LAYER_A:
        mcDBG_MSG_L4(("ISDBT_LAYER_A\n"));
        mod = tmccInfo.currentInfo.layerA.modulation;
        codenum=tmccInfo.currentInfo.layerA.codingRate;
        _pSignal->u1ErrFlg=layerAState;
        sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
        }
        break;
    case ISDBT_LAYER_B:
        mcDBG_MSG_L4(("ISDBT_LAYER_B\n"));
        mod = tmccInfo.currentInfo.layerB.modulation;
        codenum=tmccInfo.currentInfo.layerB.codingRate;
        _pSignal->u1ErrFlg=layerBState;
        sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
        }
        break;
    case ISDBT_LAYER_C:
        mcDBG_MSG_L4(("ISDBT_LAYER_C\n"));
        mod = tmccInfo.currentInfo.layerC.modulation;
        codenum=tmccInfo.currentInfo.layerC.codingRate;
        _pSignal->u1ErrFlg=layerCState;
        sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
        }
        break;
    default:
        mcDBG_MSG_L2(("DEFAULT ISDBT_LAYER_A\n"));
        /* Error */
        mod = tmccInfo.currentInfo.layerA.modulation;
        codenum=tmccInfo.currentInfo.layerA.codingRate;
        _pSignal->u1ErrFlg=layerAState;
        sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
        }
        break;
    }
          
    switch(mod){
    case SONY_ISDBT_MODULATION_DQPSK:
        _pSignal->QAMSize = 4;
        break;
    case SONY_ISDBT_MODULATION_QPSK:
        _pSignal->QAMSize = 4;
        break;
    case SONY_ISDBT_MODULATION_16QAM:
        _pSignal->QAMSize = 16;
        break;
    case SONY_ISDBT_MODULATION_64QAM:
        _pSignal->QAMSize = 64;
        break;
    default:
        _pSignal->QAMSize = 999;
        break;
    }
        
    // not used in ISDBT
    _pSignal->SymbolRate = 0;
    _pSignal->e_hierarchy_priority = 0;
    //
    _pSignal->u4BER=ber;
    _pSignal->u1CodeRate_Num=codenum;      
    mcDBG_MSG_L4(("_pSignal->u1Mode_Num=%d,_pSignal->u1Guard_Num=%d,_pSignal->u1CodeRate_Num=%d\n",\
    _pSignal->u1Mode_Num,_pSignal->u1Guard_Num,_pSignal->u1CodeRate_Num));
    
    // mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

extern TUNER_MODULATION_T TunerMod(S32 qam_size);

static void ISDBT_TunerGetTMCC(void* pzAttribute)
{   
    SIGNAL t_signal;
    TUNER_TER_DIG_TMCC_INFO_T *pt_tuner_info = (TUNER_TER_DIG_TMCC_INFO_T *) pzAttribute;   
    UINT8 ii;

    for(ii=0; ii<3; ii++){
        t_signal.u1LayerNum=ii;

        switch(t_signal.u1LayerNum){
        case LAYERA:
            ISDBT_TunerGetDemodData(NULL, TUNER_CONN_TYPE_TER_DIG,&t_signal);
            pt_tuner_info->e_ISDBT_LAYERA.CodeRate=(TUNER_ISDBT_CODING_RATE_T)t_signal.u1CodeRate_Num;
            pt_tuner_info->e_ISDBT_LAYERA.ErrFlg=(TUNER_ISDBT_LAYER_STATE_T)t_signal.u1ErrFlg;
            pt_tuner_info->e_ISDBT_LAYERA.Modulation=TunerMod(t_signal.QAMSize);
            pt_tuner_info->e_ISDBT_LAYERA.BER=t_signal.u4BER;
            mcDBG_MSG_L2(("[MWInfo]ISDBT_LAYERA:codeRate=%d,ErrFlg=%d,Modulation=%d,BER=%d\n",pt_tuner_info->e_ISDBT_LAYERA.CodeRate, \
                pt_tuner_info->e_ISDBT_LAYERA.ErrFlg,pt_tuner_info->e_ISDBT_LAYERA.Modulation,pt_tuner_info->e_ISDBT_LAYERA.BER));
            break;                      
        case LAYERB:
            ISDBT_TunerGetDemodData(NULL, TUNER_CONN_TYPE_TER_DIG,&t_signal);
            pt_tuner_info->e_ISDBT_LAYERB.CodeRate=t_signal.u1CodeRate_Num;
            pt_tuner_info->e_ISDBT_LAYERB.ErrFlg=(TUNER_ISDBT_LAYER_STATE_T)t_signal.u1ErrFlg;
            pt_tuner_info->e_ISDBT_LAYERB.Modulation=TunerMod(t_signal.QAMSize);
            pt_tuner_info->e_ISDBT_LAYERB.BER=t_signal.u4BER;
            mcDBG_MSG_L2(("[MWInfo]ISDBT_LAYERB:codeRate=%d,ErrFlg=%d,Modulation=%d,BER=%d\n",pt_tuner_info->e_ISDBT_LAYERB.CodeRate, \
                pt_tuner_info->e_ISDBT_LAYERB.ErrFlg,pt_tuner_info->e_ISDBT_LAYERB.Modulation,pt_tuner_info->e_ISDBT_LAYERB.BER));
            break;
        case LAYERC:
            ISDBT_TunerGetDemodData(NULL, TUNER_CONN_TYPE_TER_DIG,&t_signal);
            pt_tuner_info->e_ISDBT_LAYERC.CodeRate=t_signal.u1CodeRate_Num;
            pt_tuner_info->e_ISDBT_LAYERC.ErrFlg=(TUNER_ISDBT_LAYER_STATE_T)t_signal.u1ErrFlg;
            pt_tuner_info->e_ISDBT_LAYERC.Modulation=TunerMod(t_signal.QAMSize);
            pt_tuner_info->e_ISDBT_LAYERC.BER=t_signal.u4BER;
            mcDBG_MSG_L2(("[MWInfo]ISDBT_LAYERC:codeRate=%d,ErrFlg=%d,Modulation=%d,BER=%d\n",pt_tuner_info->e_ISDBT_LAYERC.CodeRate, \
                pt_tuner_info->e_ISDBT_LAYERC.ErrFlg,pt_tuner_info->e_ISDBT_LAYERC.Modulation,pt_tuner_info->e_ISDBT_LAYERC.BER));
            break;
        default:
            break;
        }
    }

    pt_tuner_info->e_isdbt_mode=(TUNER_ISDBT_MODE)t_signal.u1Mode_Num;
    pt_tuner_info->e_isdbt_guard=(TUNER_ISDBT_GUARD_T)t_signal.u1Guard_Num;
    mcDBG_MSG_L2(("[MWInfo] pt_tuner_info->c_ISDBT_Mode=%d \n",pt_tuner_info->e_isdbt_mode));
    mcDBG_MSG_L2(("[MWInfo] pt_tuner_info->c_ISDBT_GI=%d \n",pt_tuner_info->e_isdbt_guard));
}
#endif // CC_PROJECT_FOR_SONY

/************************************************************************
*  MonitorProcess
*  Monitor thread process function.
*  @param  void.
*  @retval  void.
************************************************************************/
static void MonitorProcess(VOID *pvArg)
{
    
    ISDBT_PD_CTX_T *psPdCtx = *(ISDBT_PD_CTX_T **) pvArg;
    
    mcDBG_MSG_INFO(("MonitorProcess start!\n"));
        
    while (psPdCtx->fgMonitorThreadRunning)
    {

      //  Snr = 0;
        if (psPdCtx->fgBreakMonitor)
        {
            // After mcMUTEX_LOCK(t_escape_mon_suspend), monitor thread will be suspended until
            // TunerAcq or TunerClose invoked.
            mcDBG_MSG_L2(("Entering ISDBT mon_suspend\n"));
            mcSEMA_LOCK_MTFE(psPdCtx->t_escape_mon_suspend);
            //mcMUTEX_LOCK(psPdCtx->t_escape_mon_suspend);
            mcDBG_MSG_L2(("Escape ISDBT mon_suspend\n"));
        }
        else
        {
            // mutex lock for I2C access
            mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
            psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
        
            //! [TODO:PI] put your monitor process code here
   
            psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
            mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock);                
            x_thread_delay((UINT32)(500)); 
        }
    }
    
    psPdCtx->t_monitor_thread = (THREAD_T)NULL;
    mcDBG_MSG_INFO(("MonitorProcess exit!\n"));
    x_thread_exit();
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static INT32 PD_StartMonitorEngine(ISDBT_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgMonitorThreadRunning = TRUE;
    if(x_thread_create(&psPdCtx->t_monitor_thread,
        "ISDBT_MonitorThread",
        cTHREAD_STACK_SIZE,cTHREAD_PRIORITY,
        MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
    {
        mcDBG_MSG_ERR(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcDBG_MSG_INFO(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  PD_StopMonitorEngine
*  Function which stops monitor thread engine.
*  @param  void.
*  @retval  DRVAPI_TUNER_OK : Stop monitor thread successfully.
************************************************************************/
static INT32 PD_StopMonitorEngine(void)
{
    while (pPdCtx->t_monitor_thread){
        x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_L1(("ISDBT PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}
//-------------------------------------------------------------------------
/**
 *  This API implement close tuner. It should performs
 *  1. free all allocated resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
static INT32 ISDBT_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
     
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtx->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
    mcSEMA_UNLOCK_MTFE(pPdCtx->t_escape_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    x_sema_delete(pPdCtx->t_escape_mon_suspend); 

    //add driver implementation code here
    sony_integ_SleepT_C(pInteg);

    x_sema_delete(pPdCtx->hHalLock);
    x_mem_free(pPdCtx);

    return (DRVAPI_TUNER_OK);
}
//-------------------------------------------------------------------------
/**
 *  This API do channel acuqusition.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @param   pv_conn_info       middleware tuner info structure, refer u_tuner.h
 *  @param   z_conn_info_len    length of pv_conn_info
 *  @param   _BreakFct          TunerHAL callback API for Vendor_TunerAcq to check whether need to break TunerAcq
 *  @param   *pvArg             pass this parameter to _BreakFct when calling _BreakFct
 *  @retval  1                  TPS LOCK
 *  @retval  0                  TPS UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL ISDBT_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, void* pv_conn_info,
                            SIZE_T z_conn_info_len, x_break_fct _BreakFct, void* pvArg)
{
    TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info = 
                    (TUNER_TER_DIG_TUNE_INFO_T *) pv_conn_info;
    BOOL fgChannelScan = FALSE;
    BOOL fgLock = FALSE;

    // CXD2854 driver specific
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    mcDBG_MSG_L2((SONY_CXD2854_ISDBT_DRIVER_VERSION_STR "\n"));
    mcDBG_MSG_L2(("ISDBT_TunerAcq--------->>>>>>>>>>>>\n"));

    //MUTEX lock
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    //Pause monitor thread
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        SetBreakMonitor(pPdCtx);
    }
    pPdCtx->isBreak = _BreakFct;

    // Save break function pointer/argument here to use in stopwatch function.
    // Set break function pointer to the member to call it from integ APIs.
    g_sony_demod_ctx.breakFct = _BreakFct;
    g_sony_demod_ctx.breakFctArg = pvArg;
    // Clear cancel flag
    sony_atomic_set (&(pInteg->cancel), 0);

    if(pPdCtx->fgDisStatus == TRUE){
        /* This means that terrestrial driver was disconnected now. */
        /* Check register to check that re-initialization is necessary or not. */
        sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
        uint8_t data = 0;

        sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in WriteOneRegister\n", sony_FormatResult(sony_result)));
        }

        sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x15, &data, 1);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in ReadRegister\n", sony_FormatResult(sony_result)));
        }

        if(data == 0x01){
            /* Demod HW is now in Shutdown state. */
            mcDBG_MSG_L1(("Return back from Shutdown.\n"));
            sony_result = sony_demod_ReinitializeT_C(pDemod);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_ReinitializeT_C (%s)\n", sony_FormatResult(sony_result)));
                fgLock = 0;
                goto ErrorExit;
            }
        }else{
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, &data, 1);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_ERR(("Error in ReadRegister\n", sony_FormatResult(sony_result)));
            }

            if((data != 0x01) && (data != 0x02) && (data != 0x04) && (data != 0x05) && (data != 0x06)){
                /* Demod HW is now in not DVB-T/T2/C/C2 and ISDB-T state. ReInitialize demodulator. */
                /* This case occurs in Terrestrial/Cable and Satellite combined demodulator. */
                mcDBG_MSG_L1(("Switch to Terrestrial/Cable.\n"));
                sony_result = sony_demod_ReinitializeT_C(pDemod);
                if(sony_result != SONY_RESULT_OK){
                    mcDBG_MSG_L1(("Error in sony_demod_ReinitializeT_C (%s)\n", sony_FormatResult(sony_result)));
                    fgLock = 0;
                    goto ErrorExit;
                }
            }
        }
    }

    if(pt_tuner_info->e_mod == MOD_UNKNOWN)
    { 
        fgChannelScan = TRUE;
    }
    g_sony_demod_ctx.frequency = pt_tuner_info->ui4_freq/1000;

    {
        sony_isdbt_tune_param_t tune_param; // tune param struct for ISDB-T

        // Normal Tune : Wait TS Lock.
        // Scan        : Wait Demod Lock.
        sony_result = sony_demod_terr_cable_SetScanMode(pInteg->pDemod, SONY_DTV_SYSTEM_ISDBT, fgChannelScan ? 0x01 : 0x00);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_L1(("Error in sony_demod_terr_cable_SetScanMode (%s)\n", sony_FormatResult(sony_result)));
            fgLock = 0;
            goto ErrorExit;
        }

        tune_param.centerFreqKHz = pt_tuner_info->ui4_freq/1000;
        tune_param.bandwidth = SONY_DTV_BW_6_MHZ;
        mcDBG_MSG_L2(("Freq = %d\n", tune_param.centerFreqKHz));
        mcDBG_MSG_L2(("System = ISDB-T\n"));
        mcDBG_MSG_L2(("Bandwidth = %d\n", tune_param.bandwidth));
        if(fgChannelScan){
            mcDBG_MSG_L2(("Channel Scan\n"));
        }else{
            mcDBG_MSG_L2(("Normal Tune\n"));
        }

        sony_result = sony_integ_isdbt_Tune(pInteg, &tune_param);
        switch(sony_result){
        case SONY_RESULT_OK:
            mcDBG_MSG_L2(("ISDB-T Locked.\n"));
            fgLock = 1;
            break;
        case SONY_RESULT_ERROR_UNLOCK:
            mcDBG_MSG_L2(("ISDB-T Unlocked.\n"));
            fgLock = 0;
            break;
        case SONY_RESULT_ERROR_TIMEOUT:
            mcDBG_MSG_L2(("ISDB-T Wait Lock but Timeout.\n"));
            fgLock = 0;
            break;
        default:
            mcDBG_MSG_L2(("Error in sony_integ_isdbt_Tune (%s)\n", sony_FormatResult(sony_result)));
            fgLock = 0;
            break;
        }
    }

ErrorExit:
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg) && (!fgChannelScan))
        {
            ResumeMonitor(pPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
            mcDBG_MSG_L1(("Enable monitor process!!\n"));
        }
    }

    // Clear break function pointer
    g_sony_demod_ctx.breakFct = NULL;
    g_sony_demod_ctx.breakFctArg = NULL;

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    /* Init Disconnect Status */
    pPdCtx->fgDisStatus = FALSE;

    return (fgLock); //if lock return 1, unlock return 0
}
//-------------------------------------------------------------------------
/**
 *  This API check lock status
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  1                  LOCK
 *  @retval  0                UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL ISDBT_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    BOOL fgLock=FALSE;

    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Get demod lock status from demod driver and update it as variable fgLock

    {
        UINT8 dmdLock =0;   
        UINT8 tsLock =0;
        UINT8 unlock =0;

        sony_result = sony_demod_isdbt_monitor_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
        
        if(sony_result == SONY_RESULT_OK){
            /* Judge Lock or Unlock */
            if(tsLock == 0){
                /*Unlock */
                fgLock=0;
            }else{
                /* Lock */
                fgLock=1;
            }
        }else{
            fgLock = 0;
        }
    }
    //END

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return fgLock;
}


//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval    void
 */
//-------------------------------------------------------------------------
static VOID  ISDBT_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    if(pPdCtx->fgDisStatus == TRUE)
    {
        /* Has Disconnected, return */
        mcDBG_MSG_L2(("ISDBT Has Disconnect!\n"));
        return;
    }
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));  

    sony_integ_SleepT_C(pInteg);

    //END
    SetBreakMonitor(pPdCtx);
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    /* set disconnect status */
    pPdCtx->fgDisStatus = TRUE;
}

//-------------------------------------------------------------------------
/**
 *  This API get modulation information.
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  Outputs:
 *  @param   *_pSignal          Modulation info. structure to be filled.
 *  @retval    void
 */
//-------------------------------------------------------------------------
static void ISDBT_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

#ifdef CC_PROJECT_FOR_SONY

    ISDBT_TunerGetDemodData(ptTDSpecificCtx, e_conn_type, _pSignal);

#else // CC_PROJECT_FOR_SONY
    {
        sony_result_t sony_result = SONY_RESULT_OK;
        sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
        sony_isdbt_tmcc_info_t tmccInfo;
        sony_isdbt_modulation_t mod;

        //add driver implementation code here
        _pSignal->Frequency = g_sony_demod_ctx.frequency;
        switch(pDemod->bandwidth){
        case SONY_DTV_BW_6_MHZ:
            _pSignal->e_bandwidth = BW_6_MHz; break;
        case SONY_DTV_BW_7_MHZ:
            _pSignal->e_bandwidth = BW_7_MHz; break;
        case SONY_DTV_BW_8_MHZ:
            _pSignal->e_bandwidth = BW_8_MHz; break;
        default:
            _pSignal->e_bandwidth = BW_UNKNOWN; break;
        }

        //TODO Get QAMSize from demod driver
        sony_result = sony_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo);
        if(sony_result != SONY_RESULT_OK){
            /* Error */
            _pSignal->QAMSize = MOD_UNKNOWN;
        }

        switch((TUNER_ISDBT_LAYER_T)ptTDSpecificCtx){
        case ISDBT_LAYER_A:
            mod = tmccInfo.currentInfo.layerA.modulation;
            break;
        case ISDBT_LAYER_B:
            mod = tmccInfo.currentInfo.layerB.modulation;
            break;
        case ISDBT_LAYER_C:
            mod = tmccInfo.currentInfo.layerC.modulation;
            break;
        default:
            /* Error */
            mod = tmccInfo.currentInfo.layerA.modulation;
            break;
        }
          
        switch(mod){
        case SONY_ISDBT_MODULATION_DQPSK:
            _pSignal->QAMSize = 4;
            break;
        case SONY_ISDBT_MODULATION_QPSK:
            _pSignal->QAMSize = 4;
            break;
        case SONY_ISDBT_MODULATION_16QAM:
            _pSignal->QAMSize = 16;
            break;
        case SONY_ISDBT_MODULATION_64QAM:
            _pSignal->QAMSize = 64;
            break;
        default:
            _pSignal->QAMSize = 999;
            break;
        }
        
        // not used in ISDBT
        _pSignal->SymbolRate = 0;
        _pSignal->e_hierarchy_priority = 0;
    }
#endif // CC_PROJECT_FOR_SONY

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in t base on Sony demod AGC voltage
 *   No "Lock & unlock'' to aviod Hung up.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------

static UINT8 sony_demod_isdbt_monitor_IFAGCOut_MTK(void)
{
    UINT8 u1Ret=0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    uint32_t ifagcOut = 0;
    uint32_t iTune = 300;//fine tune Signal level for different tuner
    UINT8  u1signallevel = 0;
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
    //TODO Read If_AGC control word and mapping it to signal level(0~100)

    sony_result = sony_demod_isdbt_monitor_IFAGCOut(pDemod, (uint32_t *)(&ifagcOut));
    if(sony_result == SONY_RESULT_OK){
        // ifagcOut range 0 - 4095(0x0FFF)
        ifagcOut = ifagcOut + iTune;
        u1Ret = (UINT8)(ifagcOut * 100 / 4095);
    }else{
        u1Ret = 0;
    }

    // NOTE: Following calculation is defined by MTK. (depend on customer requirement)
    if(u1Ret > 33){
        u1signallevel = 100;
    }else if(u1Ret < 12){
        u1signallevel = 0;
    }else{
        u1signallevel = (UINT8)((u1Ret - 12) * 33 / 7);
    }

    //example of return
    return (u1signallevel);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in t
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 ISDBT_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT8  u1signallevel = 0;

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    u1signallevel = sony_demod_isdbt_monitor_IFAGCOut_MTK();

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    //example of return
    return (u1signallevel);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in multiples of (0.1dBm).
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0 ~ -900           signal level
 */
//-------------------------------------------------------------------------
static INT16 ISDBT_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT16 i2Ret=0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;
    int32_t rfLevel = 0;

    mcDBG_MSG_L2(("%s \n", __FUNCTION__));

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    sony_result = sony_integ_monitor_RFLevel_MTK(pInteg, &rfLevel);
    if(sony_result == SONY_RESULT_OK){
        if(rfLevel >= 0){
            i2Ret = (INT16)((rfLevel + 50) / 100); // In 0.1dBm unit.
        }else{
            i2Ret = (INT16)((rfLevel - 50) / 100); // In 0.1dBm unit.
        }
    }else{
        i2Ret = 0;
    }

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return i2Ret;
}

static UINT32 sony_GetBER(void)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    uint32_t ber = 0; // unit is 1e-7
    uint32_t berTotal = 0; // unit is 1e-7
    uint32_t validLayer = 0;

    /* Monitor for Layer A */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Monitor for Layer B */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Monitor for Layer C */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Get Average & 1e-7 -> 1e-5 */
    if(validLayer > 0){
        return (berTotal + validLayer * 50) / (validLayer * 100); // rounding
    }else{
        return 100000;
    }
}

static UINT32 sony_GetPER(void)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    uint32_t per = 0;
    uint32_t perTotal = 0; // unit is 1e-6
    uint32_t validLayer = 0;

    /* Monitor for Layer A */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Monitor for Layer B */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Monitor for Layer C */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Get Average & 1e-6 -> 1e-5 */
    if(validLayer > 0){
        return (perTotal + validLayer * 5) / (validLayer * 10); // rounding
    }else{
        return 100000;
    }
}

static UINT32 ISDBT_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    uint32_t u4_ret = 100000;//unit is 1e-5

    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-5

    u4_ret = sony_GetPER();

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u4_ret;//modify return value
}

static UINT16 ISDBT_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret=0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    int32_t snr = 0;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Get SNR(signal-noise ratio) from demod driver

    sony_result = sony_demod_isdbt_monitor_SNR(pDemod, &snr);
    if(sony_result != SONY_RESULT_OK){
        snr = 0;
    }

    /* Value (dB x 1000) -> dB */
    u2_ret = (UINT16)((snr + 500)/1000);

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u2_ret;//modify return value
}

static void ISDBT_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static char *ISDBT_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    static char * version = SONY_CXD2854_ISDBT_DRIVER_VERSION_STR;

    return version;
}

static INT32 ISDBT_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return 0;
}
static void ISDBT_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
static void ISDBT_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_integ_t *pInteg = &g_sony_demod_ctx.integ;

    //TODO you can add command for debug use here
   
    UCHAR   ucCmdId = 0;
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t s                          Show signal, TS, ISDB-T system status\n"));
        mcDBG_MSG_L1(("\t d [t|x] [Bank]             Demod register page dump\n")); 
        mcDBG_MSG_L1(("\t r [t|x] [Bank] [RegAddr] [Num]    Read demod register\n"));
        mcDBG_MSG_L1(("\t w [t|x] [Bank] [RegAddr] [Value]  Write demod register\n"));
        mcDBG_MSG_L1(("\t ad                         Tuner register page dump\n")); 
        mcDBG_MSG_L1(("\t ar [RegAddr] [Num]         Read tuner register\n"));
        mcDBG_MSG_L1(("\t aw [RegAddr] [Value]       Write tuner register\n"));
        mcDBG_MSG_L1(("\t f [Freq]                   Set freq (kHz)\n"));
        mcDBG_MSG_L1(("\t t                          Tuner status\n")); 
        mcDBG_MSG_L1(("\t p [d|a] [freqKHz]                 Change trap threshold\n"));
    }
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);
        
    switch (ucCmdId)
    {
    case 's':
        mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
        /* Demod Lock Status */
        {
            UINT8 dmdLock = 0;
            UINT8 tsLock = 0;
            UINT8 unlock = 0;
            sony_result = sony_demod_isdbt_monitor_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Demod Lock = %d\n", dmdLock));
                mcDBG_MSG_L1(("TS Lock    = %d\n", tsLock));
                mcDBG_MSG_L1(("Unlock Det = %d\n", unlock));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_SyncStat (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* Signal level */
        {
            uint32_t ifAgc = 0;
            sony_result = sony_demod_isdbt_monitor_IFAGCOut(pDemod, &ifAgc);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Signal Level (From IFAGC monitor) = %d\n", ifAgc * 100 / 4095));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_IFAGCOut (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* Carrier Offset */
        {
            int32_t offset;
            sony_result = sony_demod_isdbt_monitor_CarrierOffset(pDemod, &offset);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Carrier Offset = %d(Hz)\n", offset));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_CarrierOffset (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* BER */
        {
            uint32_t ber = 0;
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer A, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer B, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer C, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* PER */
        {
            uint32_t per = 0;
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer A, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer B, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer C, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* Packet Error Number */
        {
            uint32_t packetErrorNum = 0;
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_A, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer A, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_B, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer B, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_DEMOD_ISDBT_MONITOR_TARGET_LAYER_C, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer C, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* SNR */
        {
            int32_t snr = 0;
            sony_result = sony_demod_isdbt_monitor_SNR(pDemod, &snr);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SNR = %d.%03d\n", snr/1000U, snr%1000U));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* RF level */
        {
            int32_t rfLevel = 0;
            sony_result = sony_integ_monitor_RFLevel_MTK(pInteg, &rfLevel);
            if(sony_result == SONY_RESULT_OK){
                if(rfLevel < 0){
                    mcDBG_MSG_L1(("RF Level = -%d.%03d\n", (-rfLevel)/1000, (-rfLevel)%1000));
                }else{
                    mcDBG_MSG_L1(("RF Level = %d.%03d\n", rfLevel/1000, rfLevel%1000));
                }
            }else{
                mcDBG_MSG_L1(("Error in sony_integ_monitor_RFLevel_MTK (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* SQI (NOT FORMAL) */
        {
            uint8_t sqiA = 0;
            uint8_t sqiB = 0;
            uint8_t sqiC = 0;
            sony_result = sony_demod_isdbt_monitor_Quality(pDemod, &sqiA, &sqiB, &sqiC);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SQI (LayerA, %%) : %d\n", sqiA));
                mcDBG_MSG_L1(("SQI (LayerB, %%) : %d\n", sqiB));
                mcDBG_MSG_L1(("SQI (LayerC, %%) : %d\n", sqiC));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_Quality (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* TS Format */
        {
            /*
            TS_FMT_T tsfmt;
            if(sony_GetTsFmt(pDemod, &tsfmt)){
                mcDBG_MSG_L1(("SerOrPar : %d\n", tsfmt.SerOrPar));
                mcDBG_MSG_L1(("ClockPolarity : %d\n", tsfmt.ClockPolarity));
                mcDBG_MSG_L1(("Tristate : %d\n", tsfmt.fgTristate));
                mcDBG_MSG_L1(("TsClock : %d\n", tsfmt.u4TsClockInKHz));
                mcDBG_MSG_L1(("GatedClock : %d\n", tsfmt.fgGatedClock));
                mcDBG_MSG_L1(("ExternalDemod : %d\n", tsfmt.fgExternalDemod));
            }else{
                mcDBG_MSG_L1(("Error in GetTsFmt\n"));
            }
            */
        }
        /* Layer State */
        {
            sony_demod_isdbt_layer_state_t layerAState;
            sony_demod_isdbt_layer_state_t layerBState;
            sony_demod_isdbt_layer_state_t layerCState;

            sony_result = sony_demod_isdbt_monitor_LayerState(pDemod, &layerAState, &layerBState, &layerCState);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_LayerState (%s)\n", sony_FormatResult(sony_result)));
            }

            switch(layerAState){
            case SONY_DEMOD_ISDBT_LAYER_STATE_OK:
                mcDBG_MSG_L1(("Layer A = OK\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR:
                mcDBG_MSG_L1(("Layer A = RS Error\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED:
                mcDBG_MSG_L1(("Layer A = Unused\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_INVALID:
            default:
                mcDBG_MSG_L1(("Layer A = Invalid\n")); break;
            }

            switch(layerBState){
            case SONY_DEMOD_ISDBT_LAYER_STATE_OK:
                mcDBG_MSG_L1(("Layer B = OK\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR:
                mcDBG_MSG_L1(("Layer B = RS Error\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED:
                mcDBG_MSG_L1(("Layer B = Unused\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_INVALID:
            default:
                mcDBG_MSG_L1(("Layer B = Invalid\n")); break;
            }

            switch(layerCState){
            case SONY_DEMOD_ISDBT_LAYER_STATE_OK:
                mcDBG_MSG_L1(("Layer C = OK\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_RS_ERROR:
                mcDBG_MSG_L1(("Layer C = RS Error\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_UNUSED:
                mcDBG_MSG_L1(("Layer C = Unused\n")); break;
            case SONY_DEMOD_ISDBT_LAYER_STATE_INVALID:
            default:
                mcDBG_MSG_L1(("Layer C = Invalid\n")); break;
            }
        }

        mcDBG_MSG_L1(("===== ISDB-T System Information =====\n"));
        /* Mode, GI, TMCC */
        {
            const char* mode_str[] = {
                "1", "2", "3", "Unknown(3)"
            };
            const char* guard_str[] = {
                "1/32", "1/16", "1/8", "1/4"
            };
            const char* system_str[] = {
                "ISDB-T", "ISDB-Tsb", "Reserved(2)", "Reserved(3)"
            };
            const char* modulation_str[] = {
                "DQPSK", "QPSK", "16QAM", "64QAM", "Reserved(4)", "Reserved(5)", "Reserved(6)", "Unused"
            };
            const char* coding_rate_str[] = {
                "1/2", "2/3", "3/4", "5/6", "7/8", "Reserved(5)", "Reserved(6)", "Unused"
            };
            const char* il_length_str[4][8] = {
                {"0", "4", "8", "16", "Reserved(4)", "Reserved(5)", "Reserved(6)", "Unused"}, /* mode 1 */
                {"0", "2", "4", "8",  "Reserved(4)", "Reserved(5)", "Reserved(6)", "Unused"}, /* mode 2 */
                {"0", "1", "2", "4",  "Reserved(4)", "Reserved(5)", "Reserved(6)", "Unused"}, /* mode 3 */
                {"Invalid", "Invalid", "Invalid", "Invalid", "Invalid", "Invalid", "Invalid", "Invalid"}
            };

            sony_isdbt_mode_t mode;
            sony_isdbt_guard_t guard;
            sony_isdbt_tmcc_info_t tmccInfo;

            sony_result = sony_demod_isdbt_monitor_ModeGuard(pDemod, &mode, &guard);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Mode           : %s\n", mode_str[mode]));
                mcDBG_MSG_L1(("GI             : %s\n", guard_str[guard]));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_ModeGuard (%s)\n", sony_FormatResult(sony_result)));
            }

            sony_result = sony_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("System         : %s\n", system_str[tmccInfo.systemId]));
                mcDBG_MSG_L1(("CountDownIndex : %d\n", tmccInfo.countDownIndex));
                mcDBG_MSG_L1(("EWS            : %d\n", tmccInfo.ewsFlag));
                mcDBG_MSG_L1(("Current\n"));
                mcDBG_MSG_L1((" isPartial     : %d\n", tmccInfo.currentInfo.isPartial));
                mcDBG_MSG_L1((" Layer A\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.currentInfo.layerA.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.currentInfo.layerA.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.currentInfo.layerA.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerA.segmentsNum));
                mcDBG_MSG_L1((" Layer B\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.currentInfo.layerB.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.currentInfo.layerB.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.currentInfo.layerB.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerB.segmentsNum));
                mcDBG_MSG_L1((" Layer C\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.currentInfo.layerC.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.currentInfo.layerC.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.currentInfo.layerC.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerC.segmentsNum));
                mcDBG_MSG_L1(("Next\n"));
                mcDBG_MSG_L1((" isPartial     : %d\n", tmccInfo.nextInfo.isPartial));
                mcDBG_MSG_L1((" Layer A\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerA.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerA.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.nextInfo.layerA.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.nextInfo.layerA.segmentsNum));
                mcDBG_MSG_L1((" Layer B\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerB.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerB.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.nextInfo.layerB.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.nextInfo.layerB.segmentsNum));
                mcDBG_MSG_L1((" Layer C\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerC.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerC.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", il_length_str[mode][tmccInfo.nextInfo.layerC.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.nextInfo.layerC.segmentsNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_TMCCInfo (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
        break;

    /* Tuner Debug Command ----------------------------------------------------- */
    case 'a':
        {
            /* Tuner Dump/Read/Write */
            const UINT8 tunerI2cAddress = 0xC0; /* Please fix it if tuner address is changed. */
            sony_i2c_t i2cTuner;

            /* If tuner is accessed by I2C Gateway */
            /* Tentative Gateway Modification */
              #ifndef CC_SONY_DISABLE_I2C_REPEATER
              		sony_i2c_MTK_CreateI2c(&i2cTuner);
              #else 
              		sony_i2c_MTK_CreateI2cGw(&i2cTuner, pDemod->i2cAddressSLVT, 0x09);
              #endif
			  
            switch(aszArgv[0][1]){
            case 'd':
            default:
                {
                    UINT8 data[256];
                    
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); // Enter Critical Section
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, 0x00, data, 256);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock); // Leave Critical Section
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Register Dump Result. (Tuner)\n"));
                    mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
                    {
                        int i = 0;
                        int j = 0;
                        char dumpline[53];
                        char* ptr = dumpline;
                        const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                        dumpline[52] = '\0';
                        for(i=0; i<16; i++){
                            ptr = dumpline;
                            for(j=0; j<16; j++){
                                if(j == 0){
                                    *ptr++ = hexchar[(i*16) >> 4];
                                    *ptr++ = hexchar[(i*16) & 0x0F];
                                    *ptr++ = ' ';
                                    *ptr++ = '|';
                                }
                                *ptr++ = ' ';
                                *ptr++ = hexchar[data[(i*16)+j] >> 4];
                                *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                            }
                            mcDBG_MSG_L1(("%s\n", dumpline));
                        }
                    }
                    mcDBG_MSG_L1(("\n"));
                }
                break;
            case 'r':
                {
                    UINT8 data[256];
                    UINT8 subAddress = 0;
                    UINT32 length = 0;
                    UINT32 i = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No read register and length.\n"));
                        return;
                    }
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    length = (UINT32)sony_strtoul(aszArgv[2]);
                    if(length > 256){
                        length = 256;
                    }
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.ReadRegister(&i2cTuner, tunerI2cAddress, subAddress, data, length);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    for(i=0; i<length; i++){
                        mcDBG_MSG_L1(("Tuner Register 0x%02X = 0x%02X\n", i + subAddress, data[i]));
                    }
                }
                break;
            case 'w':
                {
                    UINT8 subAddress = 0;
                    UINT8 value = 0;
                    if(i4Argc < 3){
                        mcDBG_MSG_L1(("Error: No write register and data.\n"));
                        return;
                    }
                    subAddress = (UINT8)sony_strtoul(aszArgv[1]);
                    value = (UINT8)sony_strtoul(aszArgv[2]);
                    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
                    sony_result = i2cTuner.WriteOneRegister(&i2cTuner, tunerI2cAddress, subAddress, value);
                    sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
                    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                    if(sony_result != SONY_RESULT_OK){
                        mcDBG_MSG_L1(("I2C Error.\n"));
                        return;
                    }
                    mcDBG_MSG_L1(("Wrote Tuner Register 0x%02X = 0x%02X\n", subAddress, value));
                }
                break;
            }
            break;
        }  
    case 'd':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            
            if(i4Argc < 3){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and bank are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); // Enter Critical Section
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
                return;
            }
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, 0x00, data, 256);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock); // Leave Critical Section
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Register Dump Result. (Slave:0x%02X, Bank:0x%02X)\n", i2cAddress, bank));
            mcDBG_MSG_L1(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
            {
                int i = 0;
                int j = 0;
                char dumpline[53];
                char* ptr = dumpline;
                const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

                dumpline[52] = '\0';
                for(i=0; i<16; i++){
                    ptr = dumpline;
                    for(j=0; j<16; j++){
                        if(j == 0){
                            *ptr++ = hexchar[(i*16) >> 4];
                            *ptr++ = hexchar[(i*16) & 0x0F];
                            *ptr++ = ' ';
                            *ptr++ = '|';
                        }
                        *ptr++ = ' ';
                        *ptr++ = hexchar[data[(i*16)+j] >> 4];
                        *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                    }
                    mcDBG_MSG_L1(("%s\n", dumpline));
                }
            }
            mcDBG_MSG_L1(("\n"));
        }
        break;
    case 'r':
        {
            UINT8 data[256];
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT32 length = 0;
            UINT32 i = 0;
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, length are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            length = (UINT32)sony_strtoul(aszArgv[4]);
            if(length > 256){
                length = 256;
            }
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, subAddress, data, length);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            for(i=0; i<length; i++){
                mcDBG_MSG_L1(("Read Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, i + subAddress, data[i]));
            }
        }
        break;
    case 'w':
        {
            UINT8 i2cAddress = 0;
            UINT8 bank = 0;
            UINT8 subAddress = 0;
            UINT8 value = 0;
            if(i4Argc < 5){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x'), bank, subaddress, value are necessary.\n"));
                return;
            }

            switch(aszArgv[1][0]){
            case 'x':
            case 'X':
                i2cAddress = pDemod->i2cAddressSLVX;
                break;
            case 't':
            case 'T':
                i2cAddress = pDemod->i2cAddressSLVT;
                break;
            default:
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') is necessary.\n"));
                return;
            }
            bank = (UINT8)sony_strtoul(aszArgv[2]);
            subAddress = (UINT8)sony_strtoul(aszArgv[3]);
            value = (UINT8)sony_strtoul(aszArgv[4]);
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, 0x00, bank);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, subAddress, value);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Wrote Slave:0x%02X, Bank:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, bank, subAddress, value));
        }
        break;
    case 'f':
        {
            UINT32 u4Freq = 0;
            PARAM_SETFREQ_T param;

            if(i4Argc < 2){
                mcDBG_MSG_L1(("Error: No Frequency.\n"));
                return;
            }
            u4Freq = (UINT32)sony_strtoul(aszArgv[1]); // freq

            param.Freq=u4Freq;
            param.Modulation=MOD_ISDBT;
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

            sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable

            if(ITuner_SetFreq(ITunerGetCtx(), &param))
            {
                mcDBG_MSG_L1(("Failed on Config tuner (%d KHz)\n", u4Freq));
            }
            else
            {
                mcDBG_MSG_L1(("Config tuner(%d KHz) OK!\n", u4Freq));
            }

            sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable

            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
        }
        break;
    case 't':
        {
            INT16 u2OPRet = 0;
            BOOL bTunerSts = FALSE;
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_demod_I2cRepeaterEnable(pDemod, 0x01); // I2C repeater enable
            u2OPRet = ITuner_OP(ITunerGetCtx(), itGetStatus, 0, &bTunerSts);
            sony_demod_I2cRepeaterEnable(pDemod, 0x00); // I2C repeater disable
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(u2OPRet)
            {
                mcDBG_MSG_L1(("ITuner_OP (eOperation = %d) Fail! Return Value = %d\n", itGetStatus, u2OPRet));
            }
            else
            {
                mcDBG_MSG_L1(("Tuner PLL Status = %d (0:Unlock, 1:Lock)\n", bTunerSts));
            }
        }
        break;
    case 'p':
        {
            UINT32 frequencyKHz = 0;
            UINT8 isAnalog = 0;

            switch(aszArgv[1][0]){
            case 'd':
            case 'D':
                isAnalog = 0;
                break;
            case 'a':
            case 'A':
                isAnalog = 1;
                break;
            default:
                mcDBG_MSG_L1(("Error: Digital/Analog ('d' or 'a') is necessary.\n"));
                return;
            }
            frequencyKHz = (UINT32)sony_strtoul(aszArgv[2]);

            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            if(ITuner_OP(ITunerGetCtx(), itSetTrapThresold, isAnalog, &frequencyKHz) == ITUNER_OK){
                if(isAnalog){
                    mcDBG_MSG_L1(("Analog trap threshold : %d kHz\n", frequencyKHz));
                }else{
                    mcDBG_MSG_L1(("Digital trap threshold : %d kHz\n", frequencyKHz));
                }
            }else{
                mcDBG_MSG_L1(("ITuner_OP failed.\n"));
            }
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
        }
        break;
    default:
        break;
    }
}

static void ISDBT_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get attribute from tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_get_type         tuner get type, refer x_tuner.h TUNER_GET_TYPE_ISDBT
 *  Outputs:
 *  @param   *pzAttribute       Attribute structure to be filled. Refer x_tuner.h structure definition TUNER_ISDBT_ATTRIBUTE_T
 *  @param   *pzAttributeLen    length to be filled.
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT16 ISDBT_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_GET_TYPE_T  e_get_type,
                                VOID *pzAttribute, SIZE_T* pzAttributeLen)
{
    INT16 retval = DRVAPI_TUNER_ERROR;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    INT32 i4Temp=0;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));

    mcDBG_MSG_L4(("GetType : %d.\n", e_get_type));

    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER: 
        *(UINT32*)pzAttribute = sony_GetBER();
        break;
    case TUNER_GET_TYPE_TS_FMT:
        {
            TS_FMT_T *pTsfmt=(TS_FMT_T *)pzAttribute;
            if(!sony_GetTsFmt(pDemod, pTsfmt)){
                goto ErrorExit;
            }
        }
        *pzAttributeLen = sizeof(TS_FMT_T); 
        break;
    case TUNER_GET_SPECIAL_TYPE_EWS_STATUS:
        {
            EWS_INFO *pEwsInfo = (EWS_INFO*)pzAttribute;
            sony_isdbt_tmcc_info_t tmccInfo;
            BOOL ewsFlag = FALSE;

            sony_result = sony_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo);
            if(sony_result == SONY_RESULT_OK){
                ewsFlag = tmccInfo.ewsFlag ? TRUE : FALSE;
            }else{
                ewsFlag = g_sony_demod_ctx.fgEws; // Previous EWS flag
            }

            // Set current value.
            pEwsInfo->val = ewsFlag ? 1 : 0;

            // Check that EWS flag was changed.
            if(g_sony_demod_ctx.fgEws != ewsFlag){
                pEwsInfo->isNotify = TRUE;
            }else{
                pEwsInfo->isNotify = FALSE;
            }

            // Save current value
            g_sony_demod_ctx.fgEws = ewsFlag;
            *pzAttributeLen = sizeof(EWS_INFO);
        }
        break;
    case TUNER_GET_TYPE_DBM_SNR:
        {
            int32_t snr = 0;
            sony_result = sony_demod_isdbt_monitor_SNR(pDemod, &snr);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("SNR = %d.%03d\n", snr/1000U, snr%1000U));
            }else{
                snr = 0;
                mcDBG_MSG_L2(("Error in sony_demod_isdbt_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
            }
            snr=snr/100;
            snr=(int32_t)snr;
            x_memcpy(pzAttribute, &snr, *pzAttributeLen);

        }
        break;
    case TUNER_GET_TYPE_AGC:
        {
            uint32_t ifAgc = 0;
            sony_result = sony_demod_isdbt_monitor_IFAGCOut(pDemod, &ifAgc);            
            ifAgc = 255 - (ifAgc >> 4);
            //ifAgc=ifAgc * 100 / 4095;
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L2(("Signal Level (From IFAGC monitor) = %d\n", ifAgc));
            }else{
            	ifAgc = 0;
                mcDBG_MSG_L2(("Error in sony_demod_isdbt_monitor_IFAGCOut (%s)\n", sony_FormatResult(sony_result)));
            }
            i4Temp=(INT32)ifAgc;
            x_memcpy(pzAttribute, &i4Temp, *pzAttributeLen);
       }       
        break;
    case TUNER_GET_TYPE_SSI:
        {
            UINT8 ssi=0;
            *pzAttributeLen = sizeof(UINT8);
            ssi = sony_demod_isdbt_monitor_IFAGCOut_MTK();        
            x_memcpy(pzAttribute, &ssi, *pzAttributeLen);
        }
        break;

#ifdef CC_PROJECT_FOR_SONY
    case TUNER_GET_TYPE_BER_UPPER:
        {
            UINT16 berUpper;            
            UINT32 ber=sony_GetBER();
            berUpper = (UINT16)((ber>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berUpper, *pzAttributeLen);     
        }
        break;
    case TUNER_GET_TYPE_BER_LOWER:
        {
            UINT16 berLower;        
            UINT32 ber=sony_GetBER();
            berLower = (UINT16)(ber&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &berLower, *pzAttributeLen);     
        }
        break;
    case TUNER_GET_TYPE_PER_UPPER:
        {
            UINT16 perUpper;
            UINT32 per=sony_GetPER();
            perUpper = (UINT16)((per>>16)&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perUpper, *pzAttributeLen);     
        }
        break;
    case TUNER_GET_TYPE_PER_LOWER:
        {   
            UINT16 perLower;    
            UINT32 per=sony_GetPER();
            perLower = (UINT16)(per&0xffff);
            *pzAttributeLen = sizeof(UINT16);
            x_memcpy(pzAttribute, &perLower, *pzAttributeLen);     
        }
        break;
    case TUNER_GET_TYPE_TMCC_INFO:
        mcDBG_MSG_L2(("e_get_type = TUNER_GET_TYPE_TMCC_INFO\r\n"));
        {
            TUNER_TER_DIG_TMCC_INFO_T TunerTmcc;
            *pzAttributeLen = sizeof(TUNER_TER_DIG_TMCC_INFO_T);
            ISDBT_TunerGetTMCC(&TunerTmcc);
            x_memcpy(pzAttribute, &TunerTmcc, *pzAttributeLen);
        }
        break;
#endif // CC_PROJECT_FOR_SONY

    default:
        //mcDBG_MSG_ERR(("unknown: get Attribute %d\r \n",e_get_type));
        break;
    }

    retval = DRVAPI_TUNER_OK;
ErrorExit:
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return retval;
}
//-------------------------------------------------------------------------
/**
 *  This API set attribute to tuner driver
 *  Inputs:
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_set_type         tuner set type, check x_tuner.h set operation TUNER_SET_TYPE_ISDBTX
 *  @param   *pvAttribute       set value.
 *  @param   zAttributeLen      length of *pvAttribute.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  DRVAPI_TUNER_OK    success.
 *  @retval  DRVAPI_TUNER_ERROR fail.
 */
//-------------------------------------------------------------------------
static INT32 ISDBT_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_SET_TYPE_T  e_set_type,
                                        const VOID *pvAttribute, SIZE_T zAttributeLen)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return DRVAPI_TUNER_OK;
}
#ifdef fcADD_DIAG_INFO
static INT32 ISDBT_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    INT32   retSts = RMR_OK;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return retSts;
}
static INT32 ISDBT_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return 0;
}
#endif
//-------------------------------------------------------------------------
/**
*  This API  control  the i2c bypass to tuner. When zap from digital to ananlog,we need this api to bypass i2c to tuner. 
* Inputs:
* @param   ptTDSpecificCtx  Pointer to tuner driver context, there is no use for third party tuner.  
* @param    bSwitchOn        TRUE(bypass i2c to tuner on).
                             FALSE(bypass i2c to tuner off) 
* @Retval                    void
*/
//-------------------------------------------------------------------------
static void ISDBT_TunerBypassI2C (PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bSwitchOn)

{
    //sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO  Bypass i2c to tuner connected(bSwitchOn is TRUE) or disconnect(bSwitchOn is FALSE)

    // I2C repeater enable/disable
    sony_demod_I2cRepeaterEnable(pDemod, bSwitchOn ? 0x01 : 0x00);
    mcDBG_MSG_L2(("[CXD2854_ISDBT]Testlog:Now is set I2C bypass: %d!!\n", bSwitchOn));

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
static void PD_ISDBT_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T* pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose       = ISDBT_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq         = ISDBT_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync     = ISDBT_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal   = ISDBT_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = ISDBT_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = ISDBT_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = ISDBT_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = ISDBT_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt   = ISDBT_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer      = ISDBT_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest     = ISDBT_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = ISDBT_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = ISDBT_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C     = ISDBT_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand     = ISDBT_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = ISDBT_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = ISDBT_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc        = ISDBT_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= ISDBT_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= ISDBT_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= ISDBT_TunerBypassI2C;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = ISDBT_TunerSetDiagCmd;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = ISDBT_TunerGetDiagInfo;
#endif
}
//-------------------------------------------------------------------------
/**
 *  This API is driver entry point. It performs
 *  1. allocate and initialize resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDCtx            TunerHAL Context use for LTDIS API mapping
 *  @param   ptTunerCfgInfo     Tuner configuration information.
 *  @param   pptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
INT32 Vendor_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //create pdctx used in this file, you also can add any declaration you want in pdctx
    INT32 retCode = 0;
    ITUNER_CTX_T *psTunerCtx ;
    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=0xD8;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtx = (ISDBT_PD_CTX_T *) x_mem_calloc(1, sizeof(ISDBT_PD_CTX_T));
    if (pPdCtx == NULL)
    {
        mcDBG_MSG_ERR(("ISDBT_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtx;
    pIsdbtTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    mcDBG_MSG_L1(("pIsdbtTunerDeviceCtx->tunerType=0x%02X\n", pIsdbtTunerDeviceCtx->tunerType));

    /* Init Disconnect Status */
    pPdCtx->fgDisStatus = TRUE;

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtx->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtx->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    //register LTDIS APIs to HAL
    PD_ISDBT_Register_LTDIS_Fct(ptTDCtx);
    //Regisiter tuner driver
    ITunerRegistration(pIsdbtTunerDeviceCtx->tunerType);

    // demod instance initialization (to do only once)
    // NOTE: If Initialization is done from DVB-T/T2/C/C2 side, this is unnecessary.

    vResetDemod(20); // 20 ms
    retCode = sony_mtk_glue_common_Initialize();
    if (retCode != DRVAPI_TUNER_OK) {
        return retCode;
    }

//#ifdef ISDBT_ENABLE_MONITOR
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtx->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {    
            mcDBG_MSG_ERR(("EN_Vendor_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        pPdCtx->fgBreakMonitor = TRUE;   //init
        pPdCtx->fgPIMoniStatus = FALSE;  //init
        PD_StartMonitorEngine(pPdCtx);   //start monitor engine
    }
//#endif

    return (DRVAPI_TUNER_OK);
}

//-------------------------------------------------------------------------
/**
 *  This API is driver entry point. It performs
 *  1. allocate and initialize resource.
 *  2. do demod/tuner initialization.
 *  3. do LTDIS API mapping
 *  @param   ptTDCtx            TunerHAL Context use for LTDIS API mapping
 *  @param   ptTunerCfgInfo     Tuner configuration information.
 *  @param   pptTDSpecificCtx   Pointer to tuner driver context.
 *  @retval  DRVAPI_TUNER_OK    Success
 *  @retval  DRVAPI_TUNER_ERROR Fail
 */
//-------------------------------------------------------------------------
INT32 Vendor_TunerOpen_c1(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo, PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    //create pdctx used in this file, you also can add any declaration you want in pdctx
    INT32 retCode = 0;
    ITUNER_CTX_T *psTunerCtx ;
    psTunerCtx=ITunerGetCtx();
   psTunerCtx->u1DemodI2cAddress_Drv=0xD8;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtx = (ISDBT_PD_CTX_T *) x_mem_calloc(1, sizeof(ISDBT_PD_CTX_T));
    if (pPdCtx == NULL)
    {
        mcDBG_MSG_ERR(("ISDBT_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtx;
    pIsdbtTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    mcDBG_MSG_L1(("pIsdbtTunerDeviceCtx->tunerType=0x%02X\n", pIsdbtTunerDeviceCtx->tunerType));

    /* Init Disconnect Status */
    pPdCtx->fgDisStatus = TRUE;

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtx->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtx->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    //register LTDIS APIs to HAL
    PD_ISDBT_Register_LTDIS_Fct(ptTDCtx);
    //Regisiter tuner driver
    ITunerRegistration(pIsdbtTunerDeviceCtx->tunerType);

    // demod instance initialization (to do only once)
    // NOTE: If Initialization is done from DVB-T/T2/C/C2 side, this is unnecessary.

    vResetDemod(20); // 20 ms
    retCode = sony_mtk_glue_common_Initialize();
    if (retCode != DRVAPI_TUNER_OK) {
        return retCode;
    }

//#ifdef ISDBT_ENABLE_MONITOR
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtx->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {    
            mcDBG_MSG_ERR(("EN_Vendor_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        pPdCtx->fgBreakMonitor = TRUE;   //init
        pPdCtx->fgPIMoniStatus = FALSE;  //init
        PD_StartMonitorEngine(pPdCtx);   //start monitor engine
    }
//#endif

    return (DRVAPI_TUNER_OK);
}
