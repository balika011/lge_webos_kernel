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
#else // SONY_DUMMY_DEBUG
#include "MTKdef.h" // MTK dummy definitions
#include "pd_isdbt_glue.h"
#endif // SONY_DUMMY_DEBUG

// for CXD2838 driver
#include "sony_common.h"
#include "sony_i2c_MTK.h"
#include "sony_demod_isdbt.h"
#include "sony_demod_isdbt_monitor.h"
#include "vdr_PD_Def.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define WAIT_TUNER_AGC_STABLE 100  // for CXD2838 tuner
#define WAIT_TS_LOCK_TIMEOUT 1500
#define WAIT_TS_LOCK_INTERVAL 50
//Import extern variable
 
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#define SONY_DEMOD_I2C_ADDRESS  0xD8  // I2C Slave Address in 8bit form
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

typedef struct sony_demod_ctx_t
{
    sony_demod_t demod;
    sony_i2c_t i2c;
    BOOL fgEws;  // Previous EWS flag value
} sony_demod_ctx_t;
static sony_demod_ctx_t g_sony_demod_ctx;

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

static UINT32 sony_strtoul(const char* str)
{
    const char* ptr = str;
    UINT32 value = 0;

    if(!str){
        return 0;
    }
    if((x_strlen(str) > 2) && (str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))){
        // Hex
        for(ptr = &(str[2]); *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value <<= 4; value += (UINT32)(*ptr - '0');
            }else if(('a' <= *ptr) && (*ptr <= 'f')){
                value <<= 4; value += (UINT32)(*ptr - 'a' + 10); 
            }else if(('A' <= *ptr) && (*ptr <= 'F')){
                value <<= 4; value += (UINT32)(*ptr - 'A' + 10);
            }else{
                break;
            }
        }
    }else{
        // Decimal
        for(ptr = str; *ptr; ptr++){
            if(('0' <= *ptr) && (*ptr <= '9')){
                value *= 10; value += (UINT32)(*ptr - '0');
            }else{
                break;
            }
        }
    }
    
    return value;
}

static const char* sony_FormatResult(sony_result_t result)
{
    switch(result){
    case SONY_RESULT_OK:
        return "OK";
    case SONY_RESULT_ERROR_ARG:
        return "ERROR_ARG";
    case SONY_RESULT_ERROR_I2C:
        return "ERROR_I2C";
    case SONY_RESULT_ERROR_SW_STATE:
        return "ERROR_SW_STATE";
    case SONY_RESULT_ERROR_HW_STATE:
        return "ERROR_HW_STATE";
    case SONY_RESULT_ERROR_TIMEOUT:
        return "ERROR_TIMEOUT";
    case SONY_RESULT_ERROR_UNLOCK:
        return "ERROR_UNLOCK";
    case SONY_RESULT_ERROR_RANGE:
        return "ERROR_RANGE";
    case SONY_RESULT_ERROR_NOSUPPORT:
        return "ERROR_NOSUPPORT";
    case SONY_RESULT_ERROR_CANCEL:
        return "ERROR_CANCEL";
    case SONY_RESULT_ERROR_OTHER:
        return "ERROR_OTHER";
    case SONY_RESULT_ERROR_OVERFLOW:
        return "ERROR_OVERFLOW";
    case SONY_RESULT_OK_CONFIRM:
        return "OK_CONFIRM";
    default:
        return "Unknown";
    }
}

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
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
     
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

    pPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    pPdCtx->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
     mcSEMA_UNLOCK_MTFE(pPdCtx->t_escape_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    x_sema_delete(pPdCtx->t_escape_mon_suspend); 

    //add driver implementation code here
    x_sema_delete(pPdCtx->hHalLock);
    x_mem_free(pPdCtx);
    //x_mem_free(psDemodCtx);

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
    UINT32 u4Freq_KHz;
    BOOL fgChannelScan = FALSE;
    BOOL fgLock = FALSE;
    UINT32 cnt = 0;
  //  UINT8 inv_seq = 0;

    // CXD2838 Specific
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_demod_lock_result_t lockResult;

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Pause monitor thread
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        SetBreakMonitor(pPdCtx);
    }
    //Parse parameter from upper layer
    
    pPdCtx->isBreak = _BreakFct;
    
    if(pt_tuner_info->e_mod == MOD_UNKNOWN)
    { 
        fgChannelScan = TRUE;
    }
    u4Freq_KHz = pt_tuner_info->ui4_freq/1000;
    pPdCtx->Frequency = u4Freq_KHz;
    
    mcDBG_MSG_L1(("Acq freq=%dKHz,fgChannelScan=%d\n",u4Freq_KHz,fgChannelScan));
    

    // Demod (CXD2838) Tune
    {
        sony_isdbt_tune_param_t tuneParam;
        tuneParam.bwMHz = 6; // For South America
        tuneParam.centerFreqKHz = u4Freq_KHz;

        sony_result = sony_demod_isdbt_Tune(pDemod, &tuneParam); 
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in sony_demod_isdbt_Tune (%s)\n", sony_FormatResult(sony_result)));
            goto ErrorExit;
        }
    }
    
    // I2C repeater enable
    sony_result = sony_demod_I2cRepeaterEnable(pDemod, 0x01);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_I2cRepeaterEnable (%s)\n", sony_FormatResult(sony_result)));
        goto ErrorExit;
    }

    // Tuner (RB22X series) tuning
    {
        PARAM_SETFREQ_T param;
        param.Freq = u4Freq_KHz;
        param.Modulation = MOD_ISDBT;
        //Set Tuner PLL
        if(ITuner_SetFreq(ITunerGetCtx(), &param))
        {
            mcDBG_MSG_ERR(("ITuner_SetFreq failed.\n"));
            goto ErrorExit;
        }
    }

    // I2C repeater disable
    sony_result = sony_demod_I2cRepeaterEnable(pDemod, 0x00);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_I2cRepeaterEnable (%s)\n", sony_FormatResult(sony_result)));
        goto ErrorExit;
    }

    // Demod Tune End
    sony_result = sony_demod_TuneEnd(pDemod); 
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_TuneEnd (%s)\n", sony_FormatResult(sony_result)));
        goto ErrorExit;
    }

    // Wait for Demod Lock
    for(cnt = 0; cnt < WAIT_TS_LOCK_TIMEOUT / WAIT_TS_LOCK_INTERVAL; cnt++)
    {
        if(_BreakFct(pvArg)){
            goto ErrorExit;
        }

        sony_result = sony_demod_isdbt_CheckTSLock(pDemod, &lockResult);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in sony_demod_isdbt_CheckTSLock (%s)\n", sony_FormatResult(sony_result)));
            goto ErrorExit;
        }

        switch(lockResult){
        case SONY_DEMOD_LOCK_RESULT_LOCK:
            mcDBG_MSG_L1(("[ISDBT_TunerAcq] Demod is Locked.\n"));
            fgLock = 1; // Lock
            goto SuccessExit;
        case SONY_DEMOD_LOCK_RESULT_UNLOCK:
            mcDBG_MSG_L1(("[ISDBT_TunerAcq] Demod is Unlocked.\n"));
            fgLock = 0; // Unlock
            goto SuccessExit;
        case SONY_DEMOD_LOCK_RESULT_NOT_DETECT:
            // Lock status is not detected. Continue waiting.
            break;
        default:
            mcDBG_MSG_L1(("Invalid Lock Status.\n"));
            fgLock = 0;
            goto ErrorExit;
        }

        x_thread_delay((UINT32)WAIT_TS_LOCK_INTERVAL); /* wait 50ms */
    }

    mcDBG_MSG_L1(("[ISDBT_TunerAcq] Waiting Demod Lock but Timeout.\n"));

ErrorExit:
SuccessExit:
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg) && (!fgChannelScan))
        {
            ResumeMonitor(pPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
            mcDBG_MSG_L1(("Enable monitor process!!\n"));
        }
    }

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    // TODO return correct lock status
    return (fgLock);
    //if lock return 1, unlock return 0
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
    //sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;

    mcDBG_MSG_L2((" %s \n",__FUNCTION__));  
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Should force demod enter standby mode(just need i2c active),otherwise,ATV will be interfered

    sony_demod_Sleep(pDemod);

    //END
    SetBreakMonitor(pPdCtx);
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
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
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_isdbt_tmcc_info_t tmccInfo;
    sony_isdbt_tmcc_modulation_t mod;

    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    //add driver implementation code here
    _pSignal->Frequency  =  pPdCtx->Frequency;
    _pSignal->e_bandwidth = BW_6_MHz;
    
    //TODO Get QAMSize from demod driver
    sony_result = sony_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo);
    if(sony_result != SONY_RESULT_OK){
        /* Error */
        _pSignal->QAMSize = MOD_UNKNOWN;
    }

    switch(*((UINT8*)(ptTDSpecificCtx))){
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
    case SONY_ISDBT_TMCC_MODULATION_DQPSK:
        _pSignal->QAMSize = MOD_QPSK;
        break;
    case SONY_ISDBT_TMCC_MODULATION_QPSK:
        _pSignal->QAMSize = MOD_QPSK;
        break;
    case SONY_ISDBT_TMCC_MODULATION_16QAM:
        _pSignal->QAMSize = MOD_QAM_16;
        break;
    case SONY_ISDBT_TMCC_MODULATION_64QAM:
        _pSignal->QAMSize = MOD_QAM_64;
        break;
    default:
        _pSignal->QAMSize = MOD_UNKNOWN;
        break;
    }
    //END
    
    // not used in ISDBT
    _pSignal->SymbolRate = 0;
    _pSignal->e_hierarchy_priority = 0;

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
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
    UINT8 u1Ret=0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    UINT32 ifagcOut = 0;
    UINT8  u1signallevel = 0;
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //TODO Read If_AGC control word and mapping it to signal level(0~100)

    sony_result = sony_demod_isdbt_monitor_IFAGCOut(pDemod, &ifagcOut);
    if(sony_result == SONY_RESULT_OK){
        // ifagcOut range 0 - 4095(0x0FFF)
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
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //Not used now

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return i2Ret;
}

static UINT32 ISDBT_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret = 100000;//unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    UINT32 per = 0; // unit is 1e-6
    UINT32 perTotal = 0; // unit is 1e-6
    UINT32 validLayer = 0;

    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-5

    /* Monitor for Layer A */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_A, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Monitor for Layer B */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_B, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Monitor for Layer C */
    sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_C, &per);
    if(sony_result == SONY_RESULT_OK){
        perTotal += per;
        validLayer++;
    }

    /* Get Average & 1e-6 -> 1e-5 */
    if(validLayer > 0){
        u4_ret = (perTotal + validLayer * 5) / (validLayer * 10); // rounding
    }else{
        u4_ret = 100000;
    }

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u4_ret;//modify return value
}

static UINT32 ISDBT_TunerGetSignalBER(PTD_SPECIFIC_CTX ptTDSpecificCtx,  DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret = 100000; //unit is 1e-5
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    UINT32 ber = 0; // unit is 1e-7
    UINT32 berTotal = 0; // unit is 1e-7
    UINT32 validLayer = 0;

     mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    //mcSEMA_LOCK_MTFE(pPdCtx->hHalLock); // DO NOT LOCK MUTEX HERE!! (This function is called from GetAttribute)
	//To Do Get PER(packet error ratio) from demod driver.note the unit is 1e-5

    /* Monitor for Layer A */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Monitor for Layer B */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Monitor for Layer C */
    sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
    if(sony_result == SONY_RESULT_OK){
        berTotal += ber;
        validLayer++;
    }

    /* Get Average & 1e-7 -> 1e-5 */
    if(validLayer > 0){
        u4_ret = (berTotal + validLayer * 50) / (validLayer * 100); // rounding
    }else{
        u4_ret = 100000;
    }

    //END
    //mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u4_ret;//modify return value
}

static UINT16 ISDBT_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret=0;
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    UINT32 snr = 0;

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
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//Not used

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return NULL;
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

   //TODO you can add command for debug use here
   
   //END
    UCHAR   ucCmdId = 0;
    if (i4Argc < 1)
    {
        mcDBG_MSG_L1(("\t s                          Show signal, TS, ISDB-T system status\n"));
        mcDBG_MSG_L1(("\t d [t|x] [Bank]             Demod register page dump\n")); 
        mcDBG_MSG_L1(("\t r [t|x] [RegAddr] [Num]    Read demod register\n"));
        mcDBG_MSG_L1(("\t w [t|x] [RegAddr] [Value]  Write demod register\n"));
        mcDBG_MSG_L1(("\t ad                         Tuner register page dump\n")); 
        mcDBG_MSG_L1(("\t ar [RegAddr] [Num]         Read tuner register\n"));
        mcDBG_MSG_L1(("\t aw [RegAddr] [Value]       Write tuner register\n"));
        mcDBG_MSG_L1(("\t f [Freq]                   Set freq (kHz)\n"));
        mcDBG_MSG_L1(("\t t                          Tuner status\n")); 
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
            UINT32 ifAgc = 0;
            sony_result = sony_demod_isdbt_monitor_IFAGCOut(pDemod, &ifAgc);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Signal Level (From IFAGC monitor) = %d\n", ifAgc * 100 / 4095));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_IFAGCOut (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* Carrier Offset */
        {
            INT32 offset;
            sony_result = sony_demod_isdbt_monitor_CarrierOffset(pDemod, &offset);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Carrier Offset = %d(Hz)\n", offset));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_CarrierOffset (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* BER */
        {
            UINT32 ber = 0;
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_A, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer A, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_B, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer B, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PreRSBER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_C, &ber);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("BER (Layer C, unit: 1e-7) = %d\n", ber));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PreRSBER (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* PER */
        {
            UINT32 per = 0;
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_A, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer A, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_B, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer B, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PER(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_C, &per);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PER (Layer C, unit: 1e-6) = %d\n", per));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PER (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* Packet Error Number */
        {
            UINT32 packetErrorNum = 0;
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_A, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer A, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_B, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer B, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
            sony_result = sony_demod_isdbt_monitor_PacketErrorNumber(pDemod, SONY_ISDBT_MONITOR_TARGET_LAYER_C, &packetErrorNum);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("PacketErrorNumber (Layer C, 1sec) = %d\n", packetErrorNum));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_PacketErrorNumber (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* SNR */
        {
            UINT32 snr = 0;
            sony_result = sony_demod_isdbt_monitor_SNR(pDemod, &snr);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("SNR = %d.%03d\n", snr/1000U, snr%1000U));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_SNR (%s)\n", sony_FormatResult(sony_result)));
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

        mcDBG_MSG_L1(("===== ISDB-T System Information =====\n"));
        /* Mode, GI */
        {
            const char* mode_str[] = {
                "1", "2", "3", "Unknown(3)"
            };
            const char* guard_str[] = {
                "1/32", "1/16", "1/8", "1/4"
            };
            sony_isdbt_mode_t mode;
            sony_isdbt_guard_t guard;
            sony_result = sony_demod_isdbt_monitor_ModeGuard(pDemod, &mode, &guard);
            if(sony_result == SONY_RESULT_OK){
                mcDBG_MSG_L1(("Mode           : %s\n", mode_str[mode]));
                mcDBG_MSG_L1(("GI             : %s\n", guard_str[mode]));
            }else{
                mcDBG_MSG_L1(("Error in sony_demod_isdbt_monitor_ModeGuard (%s)\n", sony_FormatResult(sony_result)));
            }
        }
        /* TMCC */
        {
            const char* system_str[] = {
                "ISDB-T", "ISDB-Tsb", "Reserved(2)", "Reserved(3)"
            };
            const char* modulation_str[] = {
                "DQPSK", "QPSK", "16QAM", "64QAM", "Reserved(4)", "Reserved(5)", "Reserved(6)", "Unused"
            };
            const char* coding_rate_str[] = {
                "1/2", "2/3", "3/4", "5/6", "7/8", "Reserved(5)", "Reserved(6)", "Unused"
            };
			/*
            const char* il_length_str[] = {
                "0(Mode1),0(Mode2),0(Mode3)",
                "4(Mode1),2(Mode2),1(Mode3)",
                "8(Mode1),4(Mode2),2(Mode3)",
                "16(Mode1),8(Mode2),4(Mode3)",
                "Reserved(5)",
                "Reserved(6)",
                "Unused"
            };
		*/
            sony_isdbt_tmcc_info_t tmccInfo;
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
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.currentInfo.layerA.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerA.segmentsNum));
                mcDBG_MSG_L1((" Layer B\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.currentInfo.layerB.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.currentInfo.layerB.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.currentInfo.layerB.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerB.segmentsNum));
                mcDBG_MSG_L1((" Layer C\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.currentInfo.layerC.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.currentInfo.layerC.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.currentInfo.layerC.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.currentInfo.layerC.segmentsNum));
                mcDBG_MSG_L1(("Next\n"));
                mcDBG_MSG_L1((" isPartial     : %d\n", tmccInfo.nextInfo.isPartial));
                mcDBG_MSG_L1((" Layer A\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerA.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerA.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.nextInfo.layerA.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.nextInfo.layerA.segmentsNum));
                mcDBG_MSG_L1((" Layer B\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerB.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerB.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.nextInfo.layerB.ilLength]));
                mcDBG_MSG_L1(("  SegmentsNum  : %d\n", tmccInfo.nextInfo.layerB.segmentsNum));
                mcDBG_MSG_L1((" Layer C\n"));
                mcDBG_MSG_L1(("  Modulation   : %s\n", modulation_str[tmccInfo.nextInfo.layerC.modulation]));
                mcDBG_MSG_L1(("  CodingRate   : %s\n", coding_rate_str[tmccInfo.nextInfo.layerC.codingRate]));
                mcDBG_MSG_L1(("  Interleaving : %s\n", coding_rate_str[tmccInfo.nextInfo.layerC.ilLength]));
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

            //sony_i2c_MTK_CreateI2c(&i2cTuner);
            /* If tuner is accessed by I2C Gateway */
            /* Tentative Gateway Modification */
            sony_i2c_MTK_CreateI2cGw(&i2cTuner, pDemod->i2cAddressSLVT, 0x09);

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
            UINT8 subAddress = 0;
            UINT32 length = 0;
            UINT32 i = 0;
            if(i4Argc < 4){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and subaddress, length are necessary.\n"));
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
            subAddress = (UINT8)sony_strtoul(aszArgv[2]);
            length = (UINT32)sony_strtoul(aszArgv[3]);
            if(length > 256){
                length = 256;
            }
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->ReadRegister(pDemod->pI2c, i2cAddress, subAddress, data, length);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            for(i=0; i<length; i++){
                mcDBG_MSG_L1(("Read Slave:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, i + subAddress, data[i]));
            }
        }
        break;
    case 'w':
        {
            UINT8 i2cAddress = 0;
            UINT8 subAddress = 0;
            UINT8 value = 0;
            if(i4Argc < 4){
                mcDBG_MSG_L1(("Error: Slave ('t' or 'x') and subaddress, value are necessary.\n"));
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
            subAddress = (UINT8)sony_strtoul(aszArgv[2]);
            value = (UINT8)sony_strtoul(aszArgv[3]);
            mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
            sony_result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, i2cAddress, subAddress, value);
            mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
            if(sony_result != SONY_RESULT_OK){
                mcDBG_MSG_L1(("I2C Error.\n"));
                return;
            }
            mcDBG_MSG_L1(("Wrote Slave:0x%02X, Sub:0x%02X = 0x%02X\n", i2cAddress, subAddress, value));
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

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    mcDBG_MSG_L4((" %s \n",__FUNCTION__));

    mcDBG_MSG_L4(("GetType : %d.\n", e_get_type));

    switch (e_get_type)
    {
    case TUNER_GET_TYPE_BER: 
        *(UINT32*)pzAttribute = ISDBT_TunerGetSignalBER(ptTDSpecificCtx, 0 /* dummy */);
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

    //END
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
void PD_ISDBT_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*		pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
	
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose 	= ISDBT_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq 	= ISDBT_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync 	= ISDBT_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal 	= ISDBT_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = ISDBT_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = ISDBT_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = ISDBT_TunerGetSignalPER;
    //pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalBER = ISDBT_TunerGetSignalBER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = ISDBT_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt 	= ISDBT_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer 	= ISDBT_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest 	= ISDBT_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = ISDBT_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = ISDBT_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C 	= ISDBT_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand 	= ISDBT_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand 	= ISDBT_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C 	= ISDBT_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc 	= ISDBT_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute= ISDBT_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute= ISDBT_TunerGetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerBypassI2C= ISDBT_TunerBypassI2C;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd	= ISDBT_TunerSetDiagCmd;
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
    sony_result_t sony_result = SONY_RESULT_OK;
    sony_demod_t *pDemod = &g_sony_demod_ctx.demod;
    sony_i2c_t *pI2c = &g_sony_demod_ctx.i2c;
    sony_demod_xtal_t demod_xtal;
    BOOL is_parallel = FALSE;

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

    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtx->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_ERR(("%s:  x_sema_create(pPdCtx->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    //register LTDIS APIs to HAL
    PD_ISDBT_Register_LTDIS_Fct(ptTDCtx);
    vResetDemod(100); // 100 ms
    //Regisiter tuner driver
    ITunerRegistration(pIsdbtTunerDeviceCtx->tunerType);

    // Sony demod configuration ----------------------------------------------

    // NOTE: Please set correct demod xtal frequency here!!
    // SONY_DEMOD_XTAL_16000KHz : 16MHz
    // SONY_DEMOD_XTAL_20500KHz : 20.5MHz
    // SONY_DEMOD_XTAL_24000KHz : 24MHz
    // SONY_DEMOD_XTAL_27000KHz : 27MHz
    // SONY_DEMOD_XTAL_41000KHz : 41MHz
    demod_xtal = SONY_DEMOD_XTAL_20500KHz;

    // NOTE: Please set TS serial or parallel here!!
    is_parallel = FALSE;

    // Sony specific initialization -------------------------------------------

    // clear EWS flag.
    g_sony_demod_ctx.fgEws = FALSE;

    // sony_i2c_t instance creation
    sony_result = sony_i2c_MTK_CreateI2c(pI2c);
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    // sony_demod_t instance creation
    // Argument of Create API should be changed depend on xtal frequency, I2C address.
    // I2C slave (SLVT) address: 0xD8
    sony_result = sony_demod_Create(pDemod, demod_xtal, 0xD8, pI2c); // 16MHz
    if(sony_result != SONY_RESULT_OK){ return DRVAPI_TUNER_ERROR; }

    mcDBG_MSG_L2(("Starting CXD2838 Demod Initialization.\n"));
    
    // Sony demod initialization
    sony_result = sony_demod_Initialize(pDemod);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_Initialize (%s)\n", sony_result));
        return DRVAPI_TUNER_ERROR;
    }

    // IF frequency setting
    // NOTE: This setting should be changed not only by the RF tuner but also by demod xtal frequency.
    //       (Calculation formula is changed.)
    //       SONY_DEMOD_MAKE_IFFREQ_CONFIG_XXXX macros to calculate IF frequency setting value
    //       are defined in sony_demod_isdbt.h for each demod xtal frequency.
    switch(demod_xtal){
    case SONY_DEMOD_XTAL_16000KHz:
        pDemod->iffreqConfig.configIsdbt6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_16000KHz(3.55);
        pDemod->iffreqConfig.configIsdbt7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_16000KHz(4.15); // not used
        pDemod->iffreqConfig.configIsdbt8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_16000KHz(4.75); // not used
        break;
    case SONY_DEMOD_XTAL_20500KHz:
        pDemod->iffreqConfig.configIsdbt6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_20500KHz(3.55);
        pDemod->iffreqConfig.configIsdbt7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_20500KHz(4.15); // not used
        pDemod->iffreqConfig.configIsdbt8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_20500KHz(4.75); // not used
        break;
    case SONY_DEMOD_XTAL_24000KHz:
        pDemod->iffreqConfig.configIsdbt6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_24000KHz(3.55);
        pDemod->iffreqConfig.configIsdbt7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_24000KHz(4.15); // not used
        pDemod->iffreqConfig.configIsdbt8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_24000KHz(4.75); // not used
        break;
    case SONY_DEMOD_XTAL_27000KHz:
        pDemod->iffreqConfig.configIsdbt6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_27000KHz(3.55);
        pDemod->iffreqConfig.configIsdbt7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_27000KHz(4.15); // not used
        pDemod->iffreqConfig.configIsdbt8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_27000KHz(4.75); // not used
        break;
    case SONY_DEMOD_XTAL_41000KHz:
        pDemod->iffreqConfig.configIsdbt6 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_41000KHz(3.55);
        pDemod->iffreqConfig.configIsdbt7 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_41000KHz(4.15); // not used
        pDemod->iffreqConfig.configIsdbt8 = SONY_DEMOD_MAKE_IFFREQ_CONFIG_41000KHz(4.75); // not used
        break;
    default:
        return DRVAPI_TUNER_ERROR;
    }

    // Tuner optimization setting (For Sony ASCOT2E silicon tuner)
    pDemod->tunerOptimize = SONY_DEMOD_TUNER_OPTIMIZE_ASCOT2E;
    // pDemod->tunerOptimize = SONY_DEMOD_TUNER_OPTIMIZE_UNKNOWN; // For not Sony tuner

    // Configuration by sony_demod_SetConfig function

    // Enable Shared IF
    sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_SHARED_IF, 1);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_SetConfig (%s)\n", sony_result));
        return DRVAPI_TUNER_ERROR;
    }

    if(is_parallel){
        // It's hardware default value so additional setting is unnecessary.
    }else{
        // TS output format (Serial)
        sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_PARALLEL_SEL, 0);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in sony_demod_SetConfig (%s)\n", sony_result));
            return DRVAPI_TUNER_ERROR;
        }

        // TS output from TSDATA[0] (For Serial)
        sony_result = sony_demod_SetConfig(pDemod, SONY_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
        if(sony_result != SONY_RESULT_OK){
            mcDBG_MSG_ERR(("Error in sony_demod_SetConfig (%s)\n", sony_result));
            return DRVAPI_TUNER_ERROR;
        }
    }
    // Sony specific initialization (end) -------------------------------------

    // Tuner initialization (After demod initialization)
    sony_result = sony_demod_I2cRepeaterEnable(pDemod, 0x01);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_I2cRepeaterEnable (%s)\n", sony_result));
        return DRVAPI_TUNER_ERROR;
    }

    ITuner_Init(ITunerGetCtx());

    sony_result = sony_demod_I2cRepeaterEnable(pDemod, 0x00);
    if(sony_result != SONY_RESULT_OK){
        mcDBG_MSG_ERR(("Error in sony_demod_I2cRepeaterEnable (%s)\n", sony_result));
        return DRVAPI_TUNER_ERROR;
    }

//#ifdef ISDBT_ENABLE_MONITOR
    if(pIsdbtTunerDeviceCtx->u1MWIntf != cMTAL_IF && pIsdbtTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtx->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {    
            mcDBG_MSG_ERR(("EN_Vendor_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        pPdCtx->fgBreakMonitor = TRUE;      //init
        pPdCtx->fgPIMoniStatus = FALSE;     //init
        PD_StartMonitorEngine(pPdCtx);   //start monitor engine
    }
//#endif

    return (DRVAPI_TUNER_OK);
}


// For Debugging
void sony_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("SONY_ENTER : %s (%d)\n", funcname, linenum));
}
void sony_trace_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONY_RESULT_OK){
        mcDBG_MSG_L4(("SONY_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("SONY_RETURN : (line %d))\n", linenum));
    }
}

void sony_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sony_trace_log_enter(funcname, filename, linenum);
}
void sony_trace_i2c_log_return(sony_result_t result, const char* filename, unsigned int linenum)
{
    sony_trace_log_return(result, filename, linenum);
}
