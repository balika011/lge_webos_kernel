/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: pd_isdbt_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
/** @file pd_isdbt_glue.c
 *  isdbt glue layer
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "tuner_if.h" //DRVAPI_TUNER_OK
#include "i2c_api.h"
#include "tuner_interface_if.h"
#include "x_gpio.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "TDIS_header.h"
#include "drvcust_if.h" //Get Tuner/Demod I2C bus id
#include "x_stl_lib.h"//for StrToInt function

#include "pd_isdbt_glue.h"
#include "pi_isdbt_api.h"
#include "vdr_PD_Def.h"  //add for datatype "THREAD_T"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Import extern variable
EXTERN BOOL _fgAutoSearch;	 

#ifndef DEFAULT_DEMOD_RESET_GPIO
#define DEFAULT_DEMOD_RESET_GPIO (35) //The GPIO Pin must be changed to correct no.
#endif
#if 0
//when is LOW IF add this define
#ifndef CC_LOWIF
#define CC_LOWIF
#endif
#endif
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
					   
//#if !defined(CC_MTAL) && !defined(CC_SDAL) //linux platform
//#define ISDBT_ENABLE_MONITOR
//#endif

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
static SBTVD_DEMOD_CTX_T *psDemodCtx;
//static GEN_TUNER_CTX_T *pTunerCtx;

static TUNER_DEVICE_CTX_T*		pISDBTunerDeviceCtx = NULL;

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
    mcDBG_MSG_INFO(("vResetDemod\n"));
    if (DRVCUST_OptQuery(eDemodResetGpio, &u4RstPinNum) == 0)
    {   
        GPIO_SetOut(u4RstPinNum, 0);
        x_thread_delay(u2DelayTime);
        GPIO_SetOut(u4RstPinNum, 1);
        x_thread_delay(20); // Delay 20 ms for CR[DTV00030459]. Or it will init fail in rel image
        mcDBG_MSG_INFO(("Reset ISDBT Demod GPIO=%d, 0->1, %dms\r\n",u4RstPinNum, u2DelayTime));

		
	}
}

//#ifdef ISDBT_ENABLE_MONITOR
static BOOL SetBreakMonitor(ISDBT_PD_CTX_T *psPdCtx)
{
    UINT8 u1Cnt=0;
    
    psPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psPdCtx->fgPIMoniStatus)
    {  //PI really doing PI Monitor
        while(psPdCtx->fgPIMoniStatus)
        { 
            //wait PI Monitor really finish break 
            if(u1Cnt>250)
            {  //in case of deadlock
                break;   
            }
            u1Cnt++;
            mcDBG_MSG_INFO(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
            //  x_thread_delay(10); 
            x_thread_delay((UINT32)(10));
        }            
    }
    
    return TRUE;
}

static void ResumeMonitor(ISDBT_PD_CTX_T *psPdCtx)
{
    psPdCtx->fgBreakMonitor = FALSE;
    // mcMUTEX_UNLOCK(psPdCtx->t_escape_mon_suspend);
    x_sema_unlock(psPdCtx->t_escape_mon_suspend);
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
        if (psPdCtx->fgBreakMonitor)
        {
            // After mcMUTEX_LOCK(t_escape_mon_suspend), monitor thread will be suspended until
            // TunerAcq or TunerClose invoked.
            mcDBG_MSG_INFO(("Entering ISDBT mon_suspend\n"));
            x_sema_lock(psPdCtx->t_escape_mon_suspend, X_SEMA_OPTION_WAIT);
            //mcMUTEX_LOCK(psPdCtx->t_escape_mon_suspend);
            mcDBG_MSG_INFO(("Escape ISDBT mon_suspend\n"));
        }
        else
        {
            // mutex lock for I2C access
            mcSEMA_LOCK_MTFE(psPdCtx->hHalLock);
            psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
            
            psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
            mcSEMA_UNLOCK_MTFE(psPdCtx->hHalLock);                
            x_thread_delay((UINT32)(500)); 
        }
    }
    
    psPdCtx->t_monitor_thread = (THREAD_T)NULL;
    mcDBG_MSG_INFO(("MonitorProcess exit!\n"));
    // mcTHREAD_DESTROY();
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
    //   if (mcTHREAD_CREATE(&psPdCtx->t_monitor_thread,
    //         "EVBC_MonThread",
    //       cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
    //     MonitorProcess, sizeof(void*), (VOID *) &psPdCtx) != OSR_OK)
    if(x_thread_create(&psPdCtx->t_monitor_thread,
   	          "ISDBT_MonitorThread",
   	          cTHREAD_STACK_SIZE,cTHREAD_PRIORITY,
   	          MonitorProcess,sizeof(void*), (VOID *) &psPdCtx)!= OSR_OK)
    {
        mcDBG_MSG_L1(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
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
//    pPdCtx->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
    while (pPdCtx->t_monitor_thread)
    {
        //   x_thread_delay(10); // in ms
        x_thread_delay((UINT32)(10));
    }
    mcDBG_MSG_INFO(("ISDBT PD_StopMonitorEngine success\n"));
	
    return (DRVAPI_TUNER_OK);
}
//#endif //ISDBT_ENABLE_MONITOR
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
     
    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

//#if !defined(CC_MTAL) && !defined(CC_SDAL)    
//#ifdef ISDBT_ENABLE_MONITOR
    if(pISDBTunerDeviceCtx->u1MWIntf != cMTAL_IF && pISDBTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        pPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
        pPdCtx->fgMonitorThreadRunning = FALSE; // must set to FALSE before unlock sema t_escape_mon_suspend
	 //   mcMUTEX_UNLOCK(pPdCtx->t_escape_mon_suspend);
        x_sema_unlock(pPdCtx->t_escape_mon_suspend);
        PD_StopMonitorEngine();  // stop monitor engine
	 //   mcMUTEX_DESTROY(pPdCtx->t_escape_mon_suspend);  // destory monitor thread
        x_sema_delete(pPdCtx->t_escape_mon_suspend);
    }
//#endif

    //add driver implementation code here
    x_sema_delete(pPdCtx->hHalLock);
    x_mem_free(pPdCtx);
    x_mem_free(psDemodCtx);

    //If you have allocated any resource in TunerOpen(), release them here.

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
    BOOL    fgRetSts = FALSE;
    UINT32  freq;
//    UINT32  symbolRate;
//    BOOL    oobSpecInv;
//    TUNER_MODULATION_T  mode;
    UINT32  ii;
    INT8   LockStatus ;
    PARAM_SETFREQ_T param;//Tuner PLL paramter
    UINT8 u1EnablePreset=FALSE;
    TUNER_TER_DIG_TUNE_INFO_T *pt_tuner_info;
	
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    
//#ifdef ISDBT_ENABLE_MONITOR
    if(pISDBTunerDeviceCtx->u1MWIntf != cMTAL_IF && pISDBTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
    	SetBreakMonitor(pPdCtx);
    }
//#endif

    pt_tuner_info = (TUNER_TER_DIG_TUNE_INFO_T *) pv_conn_info;
    freq = pt_tuner_info->ui4_freq;   // Hz, RF freq from MW
    mcDBG_MSG_INFO(("%s, freq = %ld Hz \n", __FUNCTION__, freq));

/*
    i4State = Util_TranslateConnectInfo(e_conn_type, pv_conn_info, z_conn_info_len,
                         &freq, &symbolRate, &mode, &oobSpecInv);

    if (i4State != RMR_OK)
    {
        mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

        mcDBG_MSG_L1((" Invalid input (%ld) to ISDBT_TunerAcq!\n", i4State));
        return fgRetSts;
    }
*/
    //middleware's definition, if e_mode=MOD_UNLNOWN, middleware is doing channel scan mode
    //otherwise it is doing change change
    /////END

    pPdCtx->isBreak = _BreakFct;
    
    param.Freq=freq/1000;
    param.Modulation=MOD_ISDBT;
    param.fgAutoSearch=_fgAutoSearch;

    psDemodCtx->u4Freq=param.Freq;
	ITuner_OP(ITunerGetCtx(),itSetInCableMode,FALSE,NULL);
	
    if(ISDBT_SetFreq(&param)>0)
    {
        mcDBG_MSG_INFO(("Complete Tuner PLL Config\n"));
    }
    
    if(!ISDBT_Connect())//Wakeup demod
    {
        mcDBG_MSG_INFO(("ISDBT_Connect fail\n"));
    }
	
#ifdef ENABLE_DEMOD_PRESET	
    if((!_fgAutoSearch) && (fgTMCCDataAval(psDemodCtx)))//Channel change and TMCC data available
    {
        u1EnablePreset=TRUE;
        mcDBG_MSG_INFO(("Use Preset TMCC data\n"));
    }
#endif

    if(u1EnablePreset)
    {
#ifdef ENABLE_DEMOD_PRESET
        if(!ISDBT_TuningInit())//Configure  demod as  channel selection mode
        {
            mcDBG_MSG_ERR(("ISDBT_TuningInit fail\n"));
            u1EnablePreset=FALSE;
        }
		
        if(!LoadTMCCData(psDemodCtx))//
        {
            mcDBG_MSG_ERR(("LoadTMCCData fail\n"));
            u1EnablePreset=FALSE;
        }
#endif
    }
    else
    {
        if(!ISDBT_ScanInit())//Configure  demod as scan mode
        {
            mcDBG_MSG_ERR(("ISDBT_ScanInit fail\n"));
        }
    }
    if(!ISDBT_Reset())
    {
        mcDBG_MSG_ERR(("ISDBT_Reset fail\n"));
    }
	
    //Loop check demod lock status	
    for (ii = 0; ii < 100; ii ++)
    {
        if(u1EnablePreset)
        {
#ifdef ENABLE_DEMOD_PRESET
            LockStatus=ISDBT_GetTuningSts();
#endif
        }
        else
        {
            LockStatus=ISDBT_GetLockSts();
        }
		
        if(LockStatus==1)
        {
            break;
        }
	 
        if(LockStatus==-1)
        {
            mcDBG_MSG_L1(("I2C fail\n"));
        }
	 
        LockStatus=ISDBT_TimeOut();
        if(LockStatus==1)
        {
            mcDBG_MSG_ERR(("Time out %d\n",ii));
            ii=100;
            break;
        }
        if(LockStatus==-1)
        {
            mcDBG_MSG_L1(("I2C fail\n"));
        }
    
        x_thread_delay(10);
	 
        if (pPdCtx->isBreak(pvArg))
        {
            break;
        }
    }
	
    if (ii == 100)
    {
	   //if(LockStatus==1)mcDBG_MSG_ERR(("Demod timeout\n"));
        mcDBG_MSG_ERR(("Front-end unlock\n"));
        if(u1EnablePreset)//Clean TMCC data
        {
#ifdef ENABLE_DEMOD_PRESET
            vCleanTMCCData(psDemodCtx);
#endif
        }
    }	
    else
    {
        if(LockStatus==1)
        {
            fgRetSts=TRUE;
#ifdef ENABLE_DEMOD_PRESET	
            if(StoreTMCCData(psDemodCtx))
            {
                mcDBG_MSG_INFO(("Store TMCC data successully\n"))
            }
#endif
            mcDBG_MSG_INFO(("Front-end lock!, ii=%d\n", ii));
        }
    }

//#ifdef ISDBT_ENABLE_MONITOR
    if(pISDBTunerDeviceCtx->u1MWIntf != cMTAL_IF && pISDBTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (!_BreakFct(pvArg))// && (u1ConnMode == ATV_CONN_MODE_CHAN_CHANGE))
        {
            ResumeMonitor(pPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
            mcDBG_MSG_INFO(("Enable monitor process!!\n"));
        }
    }
//#endif

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);


    return (fgRetSts);    //if lock return 1, unlock return 0
}
//-------------------------------------------------------------------------
/**
 *  This API check lock status
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  1                  LOCK
 *  @retval  0                  UNLOCK
 */
//-------------------------------------------------------------------------
static BOOL ISDBT_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{  
   // INT8 u2Ret=0;
	BOOL u2Ret=FALSE;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    // mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

    u2Ret=ISDBT_GetLockSts();
  
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
	
   // return (INT16)(u2Ret==1);
   return u2Ret;
}

//-------------------------------------------------------------------------
/**
 *  This API do disconnect tuner
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  void
 */
//-------------------------------------------------------------------------
static VOID  ISDBT_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{	  
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));
    //add driver implementation code here

    if(!ISDBT_Sleep())
    {
        mcDBG_MSG_L1(("Disconnect demod fail\n"));
    }

//#ifdef ISDBT_ENABLE_MONITOR
    if(pISDBTunerDeviceCtx->u1MWIntf != cMTAL_IF && pISDBTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        SetBreakMonitor(pPdCtx);
    }
//#endif

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
 *  @retval  void
 */
//-------------------------------------------------------------------------
static void ISDBT_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type, SIGNAL *_pSignal)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here

     mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

    _pSignal->Frequency  = psDemodCtx->u4Freq;
    _pSignal->e_bandwidth = BW_6_MHz;

    // not used in isdbt
    _pSignal->SymbolRate = 0;
    _pSignal->QAMSize = 8;
    _pSignal->e_hierarchy_priority = 0;
    
	mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

//-------------------------------------------------------------------------
/**
 *  This API get signal level in terms of percentage.
 *  @param   ptTDSpecificCtx    Pointer to tuner driver context.
 *  @param   e_conn_type        tuner connection type, refer x_tuner.h
 *  @retval  0~100              signal level
 */
//-------------------------------------------------------------------------
static UINT8 ISDBT_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    INT8 u1Ret=0;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    mcDBG_MSG_L1((" %s \n",__FUNCTION__));
    if((u1Ret=ISDBT_GetSignalLevel())<0)
    {
        mcDBG_MSG_L1(("Get Signal Level fail\n"));
        u1Ret=0;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    //example of return
    return (UINT8)(u1Ret);
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
    //add driver implementation code here
    //i2Ret = Vendor PI get signal level in dBm, return vlaue should be range from 0 ~ -900

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return i2Ret;
}

static UINT32 ISDBT_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT32 u4_ret=100000;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	
    //add your implementation here	  
    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

    if(ISDBT_GetBer(psDemodCtx,FALSE))
    {
        u4_ret=psDemodCtx->u4Per;
    }
    
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return u4_ret;//modify return value
}
/*
static UINT32 ISDBT_TunerGetSignalBER(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
   mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return 0;//u4_ret;//modify return value
}
*/
static UINT16 ISDBT_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type)
{
    UINT16 u2_ret=0;
    UINT32 u4CNR;

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add your implementation here	 
    mcDBG_MSG_L1((" %s \n",__FUNCTION__));

    u4CNR = ISDBT_GetSigCnr();   // Units: 0.01
    if(u4CNR > 3000)
    {
        u2_ret = 100;   
    }
    else if(u4CNR < 500)
    {
        u2_ret = 0;
    }
    else
    {
        u2_ret = (UINT16)((u4CNR-500)/25);
    }
	
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return u2_ret; //modify return value
}

static void ISDBT_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx, MPEG_FMT_T  *pt_mpeg_fmt)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static char *ISDBT_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return NULL;
}

static INT32 ISDBT_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return 0;
}
static void ISDBT_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet, UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}
static void ISDBT_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,INT32 i4Argc, const CHAR** aszArgv)
{
    UCHAR   ucCmdId = 0;
    UINT32 u4Freq;
    PARAM_SETFREQ_T param;
    UINT8 u1SubAddr,ii,jj,ucPar,u2RegAddr;
    UINT8 ucData[256];  
    BOOL bTunerSts;
    INT16 u2OPRet;    // ITuner_OP return value
    TS_FMT_T  tTsfmt;
	
    u1SubAddr=0x30;
                  
    if (i4Argc < 1)
    {
        mcDBG_MSG_INFO(("\t s:                   show signal/ts/tps status\n"));
        mcDBG_MSG_INFO(("\t f [Freq]:            set freq (kHz)\n"));
        mcDBG_MSG_INFO(("\t d [DevAddr] :    Register Page Dump Result\n")); 
        mcDBG_MSG_INFO(("\t r  [RegAddr] [Num]:  read  Demod register\n"));
        mcDBG_MSG_INFO(("\t w [RegAddr] [Value]: write Demod register\n"));
        mcDBG_MSG_INFO(("\t t  [Top]: Set/Get Tuner Top, (Top range: 0~7)\n"));   
        mcDBG_MSG_INFO(("\t p:          Tuner PLL status\n"));           
    }	

    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);

    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));

    if (i4Argc > 0)
        ucCmdId = *((CHAR *) aszArgv[0]);
        
    switch (ucCmdId)
    {
    case 's':
        mcDBG_MSG_INFO(("=================================\n"));
        mcDBG_MSG_INFO(("Lock Status=%d\n",ISDBT_GetLockSts()));
        mcDBG_MSG_INFO(("Signal level=%d%%\n",ISDBT_GetSignalLevel()));
        if(ISDBT_GetBer(psDemodCtx,TRUE))
        {
            mcDBG_MSG_INFO(("Ber=%d ^10 -5\n",psDemodCtx->u4Ber));
        }
        if(ISDBT_GetBer(psDemodCtx,FALSE))
        {
            mcDBG_MSG_INFO(("Per=%d ^ 10 -5\n",psDemodCtx->u4Per));
        }
        if(!ISDBT_GetTSFMT(&tTsfmt))
        {		
            mcDBG_MSG_L1(("ISDBT_GetEWSInfo fail\n"));;
        }
        else
        {
            mcDBG_MSG_INFO(("Out_Ctrl=%d,Ser_Par=%d,CP=%d\n",tTsfmt.fgTristate,tTsfmt.SerOrPar,tTsfmt.ClockPolarity));
        }
#ifdef ENABLE_DEMOD_PRESET	
        vShowTMCCData(psDemodCtx);
#endif
        break;

    case 'f':
        if (i4Argc >=2)
        {		
            u4Freq = (UINT32)StrToInt(aszArgv[1]); // freq
            
            param.Freq=u4Freq;
            param.Modulation=MOD_ISDBT;
            param.fgAutoSearch=_fgAutoSearch;
            if(ISDBT_SetFreq(&param)<1)
            {
                mcDBG_MSG_L1(("Failed on Config tuner (%d KHz)\n", u4Freq));
            }
            else
            {
                mcDBG_MSG_INFO(("Config tuner(%d KHz) OK!\n", u4Freq));
            }
        }
        break;

    case 'd':        
        mcDBG_MSG_INFO(("  Register Page Dump Result:\n"));
        mcDBG_MSG_INFO(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
        if(i4Argc >1)
        {
            u1SubAddr=(UINT8)StrToInt(aszArgv[1]);
            mcDBG_MSG_INFO(("Addr: 0x%02x \n", u1SubAddr));
            for (ii=0; ii<16; ii++)
            {
                mcDBG_MSG_INFO(("0x%02X ", ii));
                for (jj=0; jj<16; jj++)
                {
                    ICtrlBus_I2cDemodRead(270,u1SubAddr, ii*16, ucData, 16);
                    mcDBG_MSG_INFO(("%02x ", ucData[jj]));
                }
                mcDBG_MSG_INFO(("\n"));
            }
        }
        break;
		
    case 'r':
        if (i4Argc < 3)
            break;
     
        u2RegAddr = (UINT16)StrToInt(aszArgv[1]);
        ucPar = (UINT8)StrToInt(aszArgv[2]);
        if (ICtrlBus_I2cDemodRead(270,u1SubAddr, u2RegAddr, ucData, ucPar))
        {        
            mcDBG_MSG_INFO(("ISDB-T Get Register 0x%02X Fail!\n", u2RegAddr));
            break;
        }          
        for (ii=0; ii<ucPar; ii++)
        {
            mcDBG_MSG_INFO(("ISDB-T Register 0x%02X = 0x%02X\n", u2RegAddr+ii, ucData[ii]));
        }
        break;

    case 'w':
        if (i4Argc < 3)
            break;
		
        u2RegAddr = (UINT16)StrToInt(aszArgv[1]);
        ucPar = (UINT8)StrToInt(aszArgv[2]);
        if (ICtrlBus_I2cDemodWrite(270,u1SubAddr, u2RegAddr, &ucPar, 1))
        {        
            mcDBG_MSG_INFO(("ISDB-T Set Register 0x%02X Fail!\n", u2RegAddr));
            break;
        }          
        mcDBG_MSG_INFO(("ISDB-T Register 0x%02X = 0x%02X, OK!\n", u2RegAddr, ucPar));
        break;

    case 't':
        if (i4Argc > 1)
        {
            ucPar = (UINT8)StrToInt(aszArgv[1]); //TOP
            if((ITuner_OP(ITunerGetCtx(), itSetTop, ((ucPar&0x07)|0x08), NULL))==ITUNER_OK)
            {
                param.Freq = psDemodCtx->u4Freq;
                param.Modulation = MOD_ISDBT;
                param.fgAutoSearch = _fgAutoSearch;
                ISDBT_SetFreq(&param);
                mcDBG_MSG_INFO(("Set TOP %d ok\n",ucPar)); 
            }
            else
            {
                mcDBG_MSG_INFO(("Not support!\n")); 
            }
        }
        else
        {
            if((ITuner_OP(ITunerGetCtx(), itGetTop, 1, &ucPar))==ITUNER_OK)
            {
                mcDBG_MSG_INFO(("Current tuner TOP=%d\n",ucPar)); 
            }  
        }
        break;
		
     case 'p':
        u2OPRet = ITuner_OP(ITunerGetCtx(), itGetStatus, 0, &bTunerSts);
        if(u2OPRet)
        {
            mcDBG_MSG_L1(("ITuner_OP (eOperation = %d) Fail! Return Value = %d\n", itGetStatus, u2OPRet));
        }
        else
        {
            mcDBG_MSG_INFO(("Tuner PLL Status = %d (0:Unlock, 1:Lock)\n", bTunerSts));
        }
        break;
		  
    default:
        break;
    }    

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
}

static void ISDBT_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

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
    TS_FMT_T * pTsfmt=NULL;
    static BOOL prevEwsVal = 0;
    EWS_INFO * pEwsInfo=NULL;
	UINT16 u2Quality=0;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    //mcDBG_MSG_INFO((" %s  %d\n",__FUNCTION__,e_get_type));

    switch (e_get_type)
    {
    case TUNER_GET_SPECIAL_TYPE_EWS_STATUS:
        pEwsInfo = (EWS_INFO*)pzAttribute;

        if(!ISDBT_GetEWSInfo(pEwsInfo))
        {
            pEwsInfo->val=prevEwsVal;
        }

        if (prevEwsVal != pEwsInfo->val)
        {
            pEwsInfo->isNotify = TRUE;
        }
        else
        {
            pEwsInfo->isNotify = FALSE;
        }       
        prevEwsVal = pEwsInfo->val;
        *pzAttributeLen = sizeof(EWS_INFO);
        break;
		
    case TUNER_GET_TYPE_BER: //get attribute depend on what attribute middleware need to know. Here just give an example.
        if(ISDBT_GetBer(psDemodCtx,TRUE))
        {
            *(UINT32*)pzAttribute=psDemodCtx->u4Ber;
        }
        else
        {
            *(UINT32*)pzAttribute=100000;
        }
        *pzAttributeLen = sizeof(UINT32);
        break;
		
    case TUNER_GET_TYPE_SIGNAL_STATUS://Get Demod lock status
        if (ISDBT_GetLockSts()==1)
        {
            *(BOOL*)pzAttribute=TRUE;
        }
        else
        {
            *(BOOL*)pzAttribute =FALSE;
        }
        *pzAttributeLen = sizeof(BOOL);
        break;
   
    case TUNER_GET_TYPE_TS_FMT://GEt TS format
        pTsfmt=(TS_FMT_T *)pzAttribute;
        if(!ISDBT_GetTSFMT(pTsfmt))
        {
            mcDBG_MSG_L1(("ISDBT_GetTSFMT fail!\n"));
        }
        *pzAttributeLen = sizeof(TS_FMT_T); 
        break;
    case TUNER_GET_TYPE_SQI:		
		*pzAttributeLen = sizeof(UINT16);		
		u2Quality=ISDBT_GetSQI(psDemodCtx);        
		x_memcpy(pzAttribute, &u2Quality, *pzAttributeLen);
		
		break;
     
    default:
        mcDBG_MSG_L4(("unknown: get Attribute %d\r \n",e_get_type));
        break;
    }
    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
	
    return DRVAPI_TUNER_OK;
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
    //add driver implementation code here
#if 0
    switch (e_set_type)
    {
//set attribute depend on what attribute middleware need to set to driver. Here just give an example
    case TUNER_SET_TYPE_CI_CARD_TYPE:
        break;

    default:
        mcDBG_MSG_INFO(("unknown: set Attribute\r\n"));
        break;
    }
#endif

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);
    return DRVAPI_TUNER_OK;
}

#ifdef fcADD_DIAG_INFO
static INT32 ISDBT_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_SET_TYPE_T e_tuner_set_type, VOID* pv_set_info, SIZE_T z_size)
{
    INT32   retSts = RMR_OK;
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

    mcSEMA_UNLOCK_MTFE(pPdCtx->hHalLock);

    return retSts;
}

static INT32 ISDBT_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CUSTOM_TUNER_GET_TYPE_T e_tuner_get_type, VOID* pv_get_info, SIZE_T* pz_size)
{
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
	//add your implementation here

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
    mcSEMA_LOCK_MTFE(pPdCtx->hHalLock);
    //add driver implementation code here
    /*if(!Vendor_BypassI2C(psDemodCtx,bSwitchOn))
    	{
    		 mcDBG_MSG_L1(("Vendor_BypassI2C %s fai!! \n",(bSwitchOn?"ON":"OFF")));
    	}
       */
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
    INT32 retsts;		 
   
    mcDBG_MSG_INFO((" %s \n",__FUNCTION__));	 
	
    pISDBTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    pPdCtx = (ISDBT_PD_CTX_T *) x_mem_calloc(1, sizeof(ISDBT_PD_CTX_T));
    if (pPdCtx == NULL)
    {
        mcDBG_MSG_L1(("ISDBT_PD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = pPdCtx;

    //create demod ctx
    psDemodCtx = (SBTVD_DEMOD_CTX_T *) x_mem_calloc(1, sizeof(SBTVD_DEMOD_CTX_T));
    
    if (psDemodCtx == NULL)
    {
        mcDBG_MSG_L1(("SBTVD_DEMOD_CTX_T malloc fail\n"));
        return (DRVAPI_TUNER_ERROR);
    }
  
    pPdCtx->pDemodCtx=psDemodCtx;
	
    //Create a semaphore for low level demod PI access protection
    if (x_sema_create(&pPdCtx->hHalLock, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcDBG_MSG_L1(("%s:  x_sema_create(pPdCtx->hHalLock) Fail!\n",__FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    x_sema_unlock(pPdCtx->hHalLock);

    //register LTDIS APIs to HAL
    PD_ISDBT_Register_LTDIS_Fct(ptTDCtx);

    // Reset Demod
    vResetDemod(100); // 100 ms
    
    //Tuner  api registeration 
    retsts=ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);
    ITuner_Init(ITunerGetCtx());	
         
    if(DRVAPI_TUNER_ERROR==retsts)
    {
        return retsts;
    }
#if 0
    psDemodCtx->fgLowIF = FALSE;	// Default, IF=44MHz
#ifdef CC_LOWIF
    psDemodCtx->fgLowIF = TRUE;   // Low IF, IF=6MHz
#endif
#endif
    if (!ISDBT_Init(psDemodCtx))
    {
        mcDBG_MSG_L1(("Failed on init demod!! \n"));
        return DRVAPI_TUNER_ERROR;
    }
	
//#ifdef ISDBT_ENABLE_MONITOR
    if(pISDBTunerDeviceCtx->u1MWIntf != cMTAL_IF && pISDBTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (x_sema_create(&pPdCtx->t_escape_mon_suspend, X_SEMA_TYPE_BINARY,X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcDBG_MSG_L1(("EN_ISDBT_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }

        pPdCtx->fgBreakMonitor = TRUE;      //init
        pPdCtx->fgPIMoniStatus = FALSE;     //init
        PD_StartMonitorEngine(pPdCtx);   //start monitor engine
    }
//#endif

    ISDBT_Sleep();  // Disconnect the demod in order to avoid the interference to the ATV signal

    return (DRVAPI_TUNER_OK);
}

