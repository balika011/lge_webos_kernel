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
 * $RCSfile: pd_anaeu_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pd_anaeu_glue.c
 *  Analig Tuner LTDIS Interface
 */

/***********************************************************************/
/*              Includes                                               */
/***********************************************************************/

//#include "PI_ATuner_dbg.h"
#include "PI_Def_dbg.h"
#include "pi_anaeu_if.h"
#include "x_rm_dev_types.h"
#include "TDIS.h"
#include "queue.h"
#include "TDMgr.h"
#include "util.h"
//#include "tvsys_info.h"
#include "drvapi_tuner.h"

#ifndef CC_MT5381
#include "source_table.h"
#else
#include "source_select.h"
#endif

#include "UtilAPI.h"

#ifdef fcADD_DIAG_INFO
    #ifdef CC_MT5381
#include "u_drv_cust.h"
    #else
//#include "d_5371_cust.h"
    #endif
#endif
#include "pd_anaeu_glue.h"

#include "drv_tvd.h"   // modified by liuqu,20090511
#ifdef CC_SUPPORT_SIF_BYPASS
#include "pi_demod_atd.h"  //for sif_bypass_to_audio  function
#endif
// Justin TODO, removed in 5363
#if defined (CC_MT5363)
#include "pi_anaeu_tvsys_info.h"
#endif
#include "pi_anaeu_tvsys_info.h"
#include "pd_anaeu_picommonapi.h"
//#include "pi_dvbt_if.h"
#include "nptv_drvif.h"

#ifdef TUNER_ANALOG_NEED_INIT_TUNER
//#include "pd_tuner_commonAPI.h"  //liuqu,20090520
#include "pi_dvbt_if.h"  //here we need tuner context only
#endif


#include "pd_tuner_type.h"
#ifdef ROW_SET_TOP_TOEEPROM
#include "eepdef.h"
#endif

#if defined(CC_INTERNAL_CI)||defined(CC_INTERNAL_POD)||defined(CC_EXTERNAL_CI)||defined(CC_EXTERNAL_POD)
#include "pcmcia_if.h"
#else
#define PCMCIA_SetBypass(x) mcSHOW_USER_MSG(("Not implement PCMCIA function,%d.\n",x))
#endif

/***********************************************************************/
/*              Conditional Compiling                                  */
/***********************************************************************/
#define fcDUMMY_ANA_TUNER       (0 && fcINTERNAL_TEST)
#define fcTEST_I2C_DETACH       ((0 && fcINTERNAL_TEST) || 0)
//#define fcMULTI_REG_RW_DEBUG    1
#if 0  //Jackson remove fcADD_CUST_IF to fix zero used for undefined preprocessing identifier
#define fcADD_CUST_IF           (1 && (fcTUNER_TYPE == cANA_TUNER_ALL) && defined(CC_MT5381))
#endif

#if !defined(CC_MTAL) && !defined(CC_SDAL) //linux platform
#define ANAEU_ENABLE_MONITOR
#else

#endif

/***********************************************************************/
/*              Defines                                                */
/***********************************************************************/
#define fcADD_TVD_SYNC          1           // Pause TVD before TunerAcq complete, or will show "No Signal"

#define fcADD_AV_SYNC           1
//#define fcADD_AUD_SRC_MODE      1           // Pause TVD before TunerAcq complete, or will show "No Signal"
#define fcADD_AUD_SRC_MODE      0

//#ifdef TUNER_SUPPORT_SCART_OUT
#define fcADD_AV_NOTIFY         1
//#else
//#define fcADD_AV_NOTIFY         0
//#endif
#if  fcTUNER_TYPE == cALPS_TDAG4B03A
#define fcSEARCH_NEARBY_TIME_LIMIT      240                          //200ms for pll ,40ms for  AFT AD
#else
#define fcSEARCH_NEARBY_TIME_LIMIT      60
#endif
#define fcSEARCH_NEARBY_INIT_TIME_LIMIT 80

#define cNEXT_STEP_TVD_UNLOCK_OLD       1000000 //Hz, original value
#define cNEXT_STEP_TVD_LOCK_OLD         3000000 //Hz, original value

#define cNEXT_STEP_TVD_UNLOCK           500000 //1MHz, original value,liuqu change to 500k,20090417,for specific pattern channal loss!
#define cNEXT_STEP_TVD_LOCK_NOISE_HIGH  1000000 //Hz
// set back to old value for channel loss issue due to large input power or small P/S ratio
#define cNEXT_STEP_TVD_LOCK_PAL_BG      cNEXT_STEP_TVD_LOCK_OLD//6000000 //Hz, 5.5M + 0.5M
#define cNEXT_STEP_TVD_LOCK_PAL_I       cNEXT_STEP_TVD_LOCK_OLD//6500000 //Hz, 6.0M + 0.5M
#define cNEXT_STEP_TVD_LOCK_PAL_DK      cNEXT_STEP_TVD_LOCK_OLD//7000000 //Hz, 6.5M + 0.5M
#define cNEXT_STEP_TVD_LOCK_SECAM_L     cNEXT_STEP_TVD_LOCK_OLD//7000000 //Hz, 6.5M + 0.5M
#define cNEXT_STEP_TVD_LOCK_SECAM_L1    cNEXT_STEP_TVD_LOCK_OLD//3000000 //Hz, for auto scan is scan-up process, SIF < VIF
#define cNEXT_STEP_TVD_LOCK_PAL_MN      cNEXT_STEP_TVD_LOCK_OLD//5000000 //Hz, 4.5M + 0.5M


//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
UINT8 u1CheckVifLoop = 1;
static INT32 i4FreqOffset = 0;
static U8    fgNewScanStep = 1;
#if fcTIMING_MEASURE
#ifdef CC_SUPPORT_SIF_BYPASS
extern UINT32  u2TickStart_eu;
extern UINT8 fgSifBypassEnabled;
#else
UINT32  u2TickStart_eu;//jackson: todo, move to a control block
#endif
UINT8 ucSearchNearbyLoopCnt;
//UINT32 u4SearchNearbyTime;
static UINT16  u2Delay = 0;
#endif
/*add by liuqu,20080626,for step size test <<<<<<<<<<<<<**/
//static UINT16 u2SmallStep = 250;  //250 as default
//static UINT16 u2MiddleStep = 1500;  // 1500 as default
//liuqu removed ,20081111,get small step and middle step from ctx.


static BOOL fgEnableVIFLCheck = FALSE;
#ifdef CC_SUPPORT_SIF_BYPASS
static BOOL fg_ATD_Initialized = FALSE;
#endif
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL     fgMonitorThreadRunning = FALSE;
static THREAD_T t_monitor_thread;
static MUTEX_T  t_escape_anaeu_mon_suspend;
static UINT8 u1PODSetBypass = 0;
/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
#ifdef EU_AUTO_ATP
extern UINT8 gATP_TEST_ITEM_BIT ;
extern UINT8 gATPFailBitMsk ;
#endif
extern ANAEU_PICOMMONAPI_FCT_TBL_T *pAnaeu_PiCommonAPI_Fct;
#if (fcTUNER_TYPE != cDVBT_TUNER_ALL)
extern void Tuner_Anaeu_PiCommonAPI_Register_Fct(ANAEU_PICOMMONAPI_FCT_TBL_T *pcom_Fct);
#else
extern void Tuner_Anaeu_PiCommonAPI_Register_Fct(ANAEU_PICOMMONAPI_FCT_TBL_T *pcom_Fct, U8 tunerType);
#endif

#if (fcTUNER_TYPE != cITUNER_ALL)
extern UINT8 u1GetTunerType(VOID);
#endif

#ifdef CC_SUPPORT_SIF_BYPASS
static ATD_CTX_T*    psAtdDemodCtx=NULL;
#endif
extern UINT8 bGetSignalType(UINT8 bPath);

//declare the eep api,liuqu,200081015
 INT32 EEPNPTV_Read(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen);
 INT32 EEPNPTV_Write(UINT64 u8Offset, UINT32 u4MemPtr, UINT32 u4MemLen);




#if fcTEST_I2C_DETACH
EXTERN UINT8   fgEnI2CDetach;
#endif
//EXTERN UINT8   _SubSysIdx;

#if defined(CC_DRIVER_DEMO) || defined(CC_LINUX_KERNEL)
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define _Tvd_NotifyTVSig(u1Path, u1SigState)
#else
#define _Tvd_NotifyTVSig
#endif
#else
VOID _Tvd_NotifyTVSig(UINT8 u1Path, UINT8 u1SigState);
#endif

#if fcADD_AV_SYNC
// Henry Added for AV synchronization
EXTERN VOID AUD_WaitDspFlush(UINT8 u1DecId);
EXTERN VOID AUD_TriggerDspPlay(UINT8 u1DecId);
EXTERN VOID Aud_SetTvSysMask(UINT8 u1DecId, UINT32 u4Mask);

//EXTERN VOID AUD_DualDecAtvContinuousPlay(BOOL fgFlag); //jackson remove, I think it is an unsed API.
#endif
#if fcADD_AV_NOTIFY
EXTERN VOID AUD_AtunerSigStableNotify(BOOL fgSatble);
#endif
#if fcADD_TVD_SYNC
EXTERN VOID vTvdHoldMode(UINT8 bHoldOnOff);
#endif
#if fcADD_AUD_SRC_MODE
EXTERN VOID AUD_MW_Set_TV_SrcMode(UINT8 u1SubSysIdx);
#endif

/***********************************************************************/
/*              Imported variables, functions                          */
/***********************************************************************/
#ifdef TUNER_ANALOG_NEED_INIT_TUNER
EXTERN TUNER_COMMONAPI_FCT_TBL_T *pTuner_CommonAPI_Fct;
EXTERN VOID Tuner_CommonAPI_Register_Fct(TUNER_COMMONAPI_FCT_TBL_T *ptuner_Fct);
EXTERN VOID *GetEUTunerCtx(VOID);
GEN_TUNER_CTX_T  *psTunerCtx;
#endif

/***********************************************************************/
/*              Private (static) Function Prototypes                   */
/***********************************************************************/
VOID Tuner_AnalogEU_Register_LTDIS_Fct(PTD_CTX ptTDCtx);
VOID AnalogEU_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv);


/*move this API to correct location*/
static INT32 TunerNullBreak(VOID* pArg)
{
UNUSED(pArg);

    return (FALSE);
}


/***********************************************************************/
/*              Static Functions                                       */
/***********************************************************************/
/************************************************************************
*  AnaPd_CheckTVDLock
*  Check if TV decoder is lock or not at current frequency.
*  @param  psAtvPdCtx : Pointer use to point to struct ATV_PD_CTX_T.
*  @param  u4Freq : Current frequency value in KHz.
*  @param  cMsgInd : Message ID.
*  @retval  TRUE : TV decoder lock.
*  @retval  FALSE : TV decoder unlock.
************************************************************************/
static BOOL AnaPd_CheckTVDLock(ATV_PD_CTX_T *psAtvPdCtx, UINT16 u4Freq, CHAR cMsgInd)
{
BOOL    fgLockSts = TRUE;
    #if fcTIMING_MEASURE
HAL_TIME_T  rCurTime;

    HAL_GetTime(&rCurTime);
    mcSHOW_DBG_MSG(("%%-->%c atv-scan ChkTVD[%d  %d]\n", rCurTime.u4Seconds, rCurTime.u4Micros, cMsgInd));
    #endif

/* do TVD check when auto search find VIF, LC Chien 070302*/
    if (AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME))
    {
    #if fcTIMING_MEASURE
        HAL_GetTime(&rCurTime);
        mcSHOW_DBG_MSG(("%%<--%c atv-scan ChkTVD[%d  %d], TVD lock!\n", rCurTime.u4Seconds, rCurTime.u4Micros, cMsgInd));
    #endif
            fgLockSts = TRUE;               //move to Thomson/tuner.c
    }
    else
    {
    #if fcTIMING_MEASURE
        HAL_GetTime(&rCurTime);
        mcSHOW_DBG_MSG(("%%<--%c atv-scan ChkTVD[%d  %d], TVD unlock!\n", rCurTime.u4Seconds, rCurTime.u4Micros, cMsgInd));
    #endif
        fgLockSts = FALSE;
    }

    return fgLockSts;
}

/************************************************************************
*  AnaPd_ChangeChannel
*  Flush audio DSP once channel is changed.
*  @param  u4CurFreqency : Current frequency value in Hz.
*  @retval  void.
************************************************************************/
#ifndef CC_MT5391_AUD_3_DECODER
STATIC VOID AnaPd_ChangeChannel(UINT32 u4CurFreqency)
{
#if fcADD_AV_SYNC
STATIC UINT32 u4PreFreqency = 0;
BOOL fgSameChannel;

    fgSameChannel = (u4PreFreqency == u4CurFreqency);
    u4PreFreqency = u4CurFreqency;

    //AUD_DualDecAtvContinuousPlay(fgSameChannel);
    if (!fgSameChannel)
        AUD_WaitDspFlush(1);
#endif
}
#endif

/************************************************************************
*  AnaPd_ExitTunerAcq
*  Notify audio DSP once tuner acquisition is finished.
*  @param  fgLocked : Current tuner lock status.
*  @retval  void.
************************************************************************/
STATIC VOID AnaPd_ExitTunerAcq(BOOL fgLocked)
{
#if fcADD_AV_NOTIFY
    AUD_AtunerSigStableNotify(fgLocked);
#endif
#if fcADD_TVD_SYNC
    vTvdHoldMode(SV_OFF);
#endif
}

/************************************************************************
*  GetAnaConnMode
*  Get current analog connection mode.
*  @param  eMod : Connection mode parameter.
*  @retval  ATV_CONN_MODE_CHAN_SCAN : Analog channel scan.
*  @retval  ATV_CONN_MODE_FINE_TUNE : Analog frequency fine-tune.
*  @retval  ATV_CONN_MODE_CHAN_CHANGE : Analog channel change.
************************************************************************/
/***********************************************************************/
/*
    eMod:
    MOD_PSK_8 --> Channel Change; _fgAutoSearch = FALSE
    MOD_UNKNOWN --> Scan; _fgAutoSearch = TRUE
    MOD_QAM_1024 --> Fine-tune (EU mode); _fgAutoSearch = TRUE
*/
STATIC UINT8 GetAnaConnMode(UINT8 eMod)
{
UINT8   u1ConnMode;

    if (eMod != MOD_PSK_8)
    {
        //_fgAutoSearch = TRUE;
        if (eMod == MOD_UNKNOWN)
        {
            u1ConnMode = ATV_CONN_MODE_CHAN_SCAN;
            mcSHOW_DBG_MSG(( "Mod_type = channel scan!\n"));
        }
        else
        {
            u1ConnMode = ATV_CONN_MODE_FINE_TUNE;
            mcSHOW_DBG_MSG(( "Mod_type = fine tune!\n"));
        }
    }
    else
    {
       // _fgAutoSearch = FALSE;
        u1ConnMode = ATV_CONN_MODE_CHAN_CHANGE;
        mcSHOW_DBG_MSG(( "Mod_type = channel change!\n"));
    }
    return u1ConnMode;
}

/***********************************************************************/
#if 0  //Jackson remove fcADD_CUST_IF to fix zero used for undefined preprocessing identifier
#if fcADD_CUST_IF
UINT32  u4TunerCtrlAnaForce = 0;

/************************************************************************
*  Analog_GetCtmrReq
*  Get customized register setting.
*  @param  viod.
*  @retval  Customized register value.
************************************************************************/
STATIC UINT32 Analog_GetCtmrReq(VOID)
{
UINT32  TunerCtrl;

    #ifdef CC_MT5381
    TunerCtrl = DRVCUST_OptGet(eTunerCtrlAna);
    #else
    TunerCtrl = DRVCUST_InitGet(eTunerCtrlAna);
    #endif
//    TunerCtrl = mcBIT(cCUST_REQ_ANA_0);
//    TunerCtrl = mcBIT(cCUST_REQ_ANA_2) | mcBIT(cCUST_REQ_ANA_3);  //JWEI 2007/06/07
    if (u4TunerCtrlAnaForce)
        TunerCtrl = u4TunerCtrlAnaForce;
    mcPRN_DBG_MSG("DRVCUST_OptGet(eTunerCtrlAna) = 0x%02X\n", TunerCtrl);

    return TunerCtrl;
}
#endif
#endif

#ifdef ANAEU_ENABLE_MONITOR
static BOOL SetBreakMonitor(ATV_PD_CTX_T *psAtvPdCtx)
{
    UINT8 u1Cnt=0;

    psAtvPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psAtvPdCtx->fgPIMoniStatus){  //PI really doing PI Monitor
        while(psAtvPdCtx->fgPIMoniStatus){
            //wait PI Monitor really finish break
            if(u1Cnt>250){  //in case of deadlock
                break;
            }
            u1Cnt++;
            mcSHOW_DBG_MSG(("ANA EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
            mcDELAY_MS(10);
        }
    }

    return TRUE;
}
static void ResumeMonitor(ATV_PD_CTX_T *psAtvPdCtx)
{
    psAtvPdCtx->fgBreakMonitor = FALSE;
    mcMUTEX_UNLOCK(t_escape_anaeu_mon_suspend);
}

/************************************************************************
*  i4AnaEUMonitorBreak
*  Break function of monitor thread.
*  @param  *pvArg : A pointer to ATV_PD_CTX_T structure.
*  @retval  Break flag of break function.
************************************************************************/
STATIC INT32 i4AnaEUMonitorBreak(VOID *pvArg)
{
    ATV_PD_CTX_T *psPdCtx = (ATV_PD_CTX_T *) pvArg;

    if (psPdCtx->fgBreakMonitor)
    {
        mcSHOW_DBG_MSG(("ANA_EU_Monitor break function triggered!!\n"));
    }
    return (psPdCtx->fgBreakMonitor);
}

/************************************************************************
*  MonitorProcess
*  Monitor thread process function.
*  @param  void.
*  @retval  void.
************************************************************************/
STATIC void MonitorProcess(VOID *pvArg)
{

    ATV_PD_CTX_T *psPdCtx = *(ATV_PD_CTX_T **) pvArg;

    mcSHOW_DRVAPI_MSG(("MonitorProcess start!\n"));

    while (fgMonitorThreadRunning)
    {
    	if (psPdCtx->fgBreakMonitor)
    	{
    		// After mcMUTEX_LOCK(t_escape_anaeu_mon_suspend), monitor thread will be suspended until
    		// TunerAcq or TunerClose invoked.
            mcSHOW_DRVAPI_MSG(("Entering ANA_EU mon_suspend\n"));
    		mcMUTEX_LOCK(t_escape_anaeu_mon_suspend);
    		mcSHOW_DRVAPI_MSG(("Escape ANA_EU mon_suspend\n"));
    	}
    	else
    	{
		    // mutex lock for I2C access
            mcSEMA_LOCK_HAL(psPdCtx->hHalLock);
	        mcSHOW_DBG_MSG2(("Invoke  ANA_EU_Monitor\n"));
		//add PI monitor API here
		    psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
            AnalogPIEU_TunerMonitor(cATUNER_PI_CTX(psPdCtx),
                        &psPdCtx->Frequency,
                        i4AnaEUMonitorBreak, psPdCtx);
            psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
            mcSEMA_UNLOCK_HAL(psPdCtx->hHalLock);
	        mcDELAY_MS(500);
        }
    }

    t_monitor_thread = NULL;
    mcSHOW_DRVAPI_MSG(("ANA EU MonitorProcess exit!\n"));
    mcTHREAD_DESTROY();
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static S32 PD_StartMonitorEngine(ATV_PD_CTX_T *psAtvPdCtx)
{
    //ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;

    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not
    // directly use pv_arg. Therefore, if we want thread to share the same
    // context as caller, we need set pv_arg as "pointer of context".
    fgMonitorThreadRunning = TRUE;
    if (mcTHREAD_CREATE(&t_monitor_thread,
            "EU_ANA_MonThread",
            cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
            MonitorProcess, sizeof(void*), (VOID *) &psAtvPdCtx) != OSR_OK)
    {
        mcSHOW_DBG_MSG(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSHOW_DBG_MSG(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  PD_StopMonitorEngine
*  Function which stops monitor thread engine.
*  @param  void.
*  @retval  DRVAPI_TUNER_OK : Stop monitor thread successfully.
************************************************************************/
static S32 PD_StopMonitorEngine(void)
{
//    fgMonitorThreadRunning = FALSE; //must set to FALSE before unlock sema mon_suspend

    while (t_monitor_thread){
        mcDELAY_MS(10); // in ms
    }
    mcSHOW_DRVAPI_MSG(("ANA EU PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}
#endif //ANAEU_ENABLE_MONITOR

/************************************************************************
*  vSetTvSubSysIdx
*  Set specified TV system mask into TV system index of PD context.
*  @param  *psAtvPdCtx : A pointer to ATV_PD_CTX_T structure.
*  @param  u4TvSubSysMask : TV system mask.
*  @retval  TRUE : Current TV system is changed.
*  @retval  FALSE : Current TV system is not changed.
************************************************************************/
STATIC BOOL vSetTvSubSysIdx(ATV_PD_CTX_T *psAtvPdCtx, UINT32  u4TvSubSysMask)
{
UINT8   u1SubSysIdx;

    mcSHOW_DBG_MSG(("%s: u4TvSubSysMask = %04X\n", __FUNCTION__, u4TvSubSysMask));
    if (u4TvSubSysMask != TV_SYS_MASK_NONE)
    {
        u1SubSysIdx = SYS_SUB_NTSC_M;
        if (u4TvSubSysMask & (TV_SYS_MASK_B | TV_SYS_MASK_G))
            u1SubSysIdx = SYS_SUB_PAL_BG;
        else if (u4TvSubSysMask & (TV_SYS_MASK_D | TV_SYS_MASK_K))
            u1SubSysIdx = SYS_SUB_PAL_DK;
        else if (u4TvSubSysMask & (TV_SYS_MASK_I))
            u1SubSysIdx = SYS_SUB_PAL_I;
        else if (u4TvSubSysMask & (TV_SYS_MASK_L))
            u1SubSysIdx = SYS_SUB_SECAM_L;
        else if (u4TvSubSysMask & (TV_SYS_MASK_L_PRIME))
            u1SubSysIdx = SYS_SUB_SECAM_L1;
        else if (u4TvSubSysMask & (TV_SYS_MASK_M | TV_SYS_MASK_N))
            u1SubSysIdx = SYS_SUB_NTSC_M;

        if (psAtvPdCtx->sAtvPiCtx.u4TvSubSysMask != u4TvSubSysMask)
        {
            psAtvPdCtx->sAtvPiCtx.u4TvSubSysMask = u4TvSubSysMask;
        //    if (u1SubSysIdx < SYS_SUB_TOTAL)
            {
                psAtvPdCtx->sAtvPiCtx.u1SubSysIdx = u1SubSysIdx;
                mcSHOW_DBG_MSG(("%s: u1SubSysIdx = %d\n", __FUNCTION__, u1SubSysIdx));

                //AnaPd_SetTvSubSysIdx(u1SubSysIdx);
                #if fcADD_AUD_SRC_MODE
                    AUD_MW_Set_TV_SrcMode(u1SubSysIdx);
                    //andrew wen 07/8/17 Tuner_MW send notify to Aud_MW when ATV source is SECAM
                #endif

                return TRUE;
            }
        }
        else
            return (FALSE);
    }
    return (FALSE);
}

STATIC VOID vSetJumpSmallStep(ITUNER_CTX_T *pDigiTunerCtx, UINT16 u2SetSmallStep)
{
  SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);

	if((u2SetSmallStep>0)&&(u2SetSmallStep<=500))
    {
        pEUCtx->m_Small_Step= u2SetSmallStep;
    }
    mcSHOW_USER_MSG(("set small step as %d.\n",pEUCtx->m_Small_Step));

}

STATIC VOID vSetJumpMiddleStep(ITUNER_CTX_T *pDigiTunerCtx, UINT16 u2SetMiddleStep,UINT8 fgScanDir)
{
  SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);

  if(fgScanDir ==SCAN_DIRECTION_INC )
  {
     if((u2SetMiddleStep>0)&&(u2SetMiddleStep<=3000))
      {
        pEUCtx->m_Middle_Step_Up= u2SetMiddleStep;
      }
      mcSHOW_USER_MSG(("set middle step as %d.\n",pEUCtx->m_Middle_Step_Up));
  }
  else
  {
        if((u2SetMiddleStep>0)&&(u2SetMiddleStep<=3000))
         {
          pEUCtx->m_Middle_Step_Down= u2SetMiddleStep;
         }
          mcSHOW_USER_MSG(("set middle step as %d.\n",pEUCtx->m_Middle_Step_Down));
   }
}

STATIC VOID vSetVIFLBitCheckEnable(UINT8 u1Enable)
{
    if(u1Enable == 0)
        fgEnableVIFLCheck = FALSE;
    else if(u1Enable == 1)
        fgEnableVIFLCheck = TRUE;
    else
    {
        mcSHOW_USER_MSG(("Invalid input parameter,0 for disable,1 for enable."));
    }
}


//external functions need to be called by PI code
INT16 GetJumpSmallStep(ITUNER_CTX_T *pDigiTunerCtx)
{
 SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);

	if(pEUCtx->m_Small_Step <= 500)
    {
        return (pEUCtx->m_Small_Step);
    }
    else
    {
        mcSHOW_USER_MSG(("small step get invalid! u2SmallStep=%d\n",pEUCtx->m_Small_Step));
        return 250;
    }

}

INT16 GetJumpMiddleStep(ITUNER_CTX_T *pDigiTunerCtx, UINT8 fgScanDir)
{
  SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);

   if(fgScanDir == SCAN_DIRECTION_INC)
   {
         if (pEUCtx->m_Middle_Step_Up <= 3000)
        {
         return (pEUCtx->m_Middle_Step_Up);
        }
        else
        {
        mcSHOW_USER_MSG(("middle step get invalid! u2MiddleStep=%d\n",pEUCtx->m_Middle_Step_Up));
        return 500;
        }
   }
   else  //fgScanDir == SCAN_DIRECTION_DEC
   {
         if (pEUCtx->m_Middle_Step_Down<= 3000)
        {
         return (pEUCtx->m_Middle_Step_Down);
        }
        else
        {
        mcSHOW_USER_MSG(("middle back_step get invalid! u2MiddleStepBack=%d\n",pEUCtx->m_Middle_Step_Down));
        return 500;
        }
   }
}



BOOL GetVIFLBitCheckEnable(VOID)
{
    return fgEnableVIFLCheck;
}

/***********************************************************************/
/*              Public Functions                                       */
/***********************************************************************/

/************************************************************************
*  AnalogEU_TunerOpen
*  Analog glue layer initiliazation function.
*  @param  ptTDCtx : A pointer use to point to struct TUNER_DEVICE_CTX_T.
*  @param  ptTunerCfgInfo : A pointer use to point to struct TUNER_CONFIG_T.
*  @param  pptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  DRVAPI_TUNER_OK : Analog glue initialization successfully.
*  @retval  DRVAPI_TUNER_ERROR : Analog glue initialization fail.
************************************************************************/
INT32 AnalogEU_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo,
                     PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ATV_PD_CTX_T* psAtvPdCtx;
    UINT8 u1SubSysIdx;
#if (fcTUNER_TYPE == cITUNER_ALL)
    TUNER_DEVICE_CTX_T* pTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
#endif

    Tuner_AnalogEU_Register_LTDIS_Fct(ptTDCtx);
// Allocate TS_SPECIFIC_CTX for Instance
    psAtvPdCtx = (ATV_PD_CTX_T *) mcALLOC_MEM(sizeof(ATV_PD_CTX_T));
    if (psAtvPdCtx == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    *pptTDSpecificCtx = psAtvPdCtx;
//    mcSHOW_DBG_MSG(("bApiTunerInitInUSA\n"));
//    bApiTunerInitInUSA();
#if 0//jackson remove old tuner all
#if fcTUNER_TYPE == cANA_TUNER_ALL
    psAtvPdCtx->sAtvPiCtx.u1TunerType = Ana_GetConfig(&(psAtvPdCtx->sAtvPiCtx.psTunerFunc));
#endif
#endif

#if (fcTUNER_TYPE != cITUNER_ALL)
 #if (fcTUNER_TYPE != cDVBT_TUNER_ALL)
	Tuner_Anaeu_PiCommonAPI_Register_Fct(pAnaeu_PiCommonAPI_Fct);
 #else
	Tuner_Anaeu_PiCommonAPI_Register_Fct(pAnaeu_PiCommonAPI_Fct, u1GetTunerType());
 #endif
#else
    switch (pTunerDeviceCtx->tunerType)
    {
    case cNXP_UV1356E:
        UV1356E_Anaeu_PiCommonAPI_Register_Fct(pAnaeu_PiCommonAPI_Fct);
        break;
    case cNXP_UV1316E:
        UV1316E_Anaeu_PiCommonAPI_Register_Fct(pAnaeu_PiCommonAPI_Fct);
        break;
    default:
        mcSHOW_USER_MSG(("%s: wrong tuner type\n", __FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
#endif

#ifdef TUNER_ANALOG_NEED_INIT_TUNER
    // Tuner code registration for analog only project.
    // Due to ROW project is analog only, we try to remove out tuner init code from digital demod driver.
	Tuner_CommonAPI_Register_Fct(pTuner_CommonAPI_Fct);
    psTunerCtx = GetEUTunerCtx();
    Tuner_Initialize(psTunerCtx);
#endif
    ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);
    ITuner_Init(ITunerGetCtx());    //todo: modify Tuner I2C Addr
    cATUNER_PI_CTX(psAtvPdCtx)->pDigiTunerCtx=ITunerGetCtx();
//--------------------ITuner Register------------------------------
#if (fcTUNER_TYPE == cLG_TAFTZ716D)
    ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);
    if (ITunerGetCtx() == NULL)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    ITuner_Init(ITunerGetCtx());    //todo: modify Tuner I2C Addr
 #endif
//---------------------------------------------------------------

#if 0   //liuqu,20090427,for abort test!
    pTunerCtx = ITunerGetCtx();
    ITuner_Init(pTunerCtx);
#endif
//    if (!AnalogPI_TunerInit((PATD_SPECIFIC_CTX) psAtvPdCtx,
    if (!AnalogPIEU_TunerInit(cATUNER_PI_CTX(psAtvPdCtx)))//,
                    //ptTunerCfgInfo->ucDemodInbAddr,
                    //ptTunerCfgInfo->ucRFTunerAddr))
        return (DRVAPI_TUNER_ERROR);


#if fcADD_HAL_LOCK
//    if (mcSEMA_CREATE(&psAtvPdCtx->hHalLock, X_SEMA_STATE_UNLOCK) != OSR_OK)
    if (mcSEMA_CREATE(&psAtvPdCtx->hHalLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_USER_MSG(("%s: mcSEMA_CREATE(hHalLock) Fail!\n", __FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
#endif
#if 0  //Jackson remove fcADD_CUST_IF to fix zero used for undefined preprocessing identifier
#if fcADD_CUST_IF
    psAtvPdCtx->u4DrvCustTunerCtrl = Analog_GetCtmrReq();
#endif
#endif
#if fcTUNE_NOT_START
    psAtvPdCtx->u1TunerAcqType = ACQ_ACTION_DEFAULT;
#endif
    u1SubSysIdx = SYS_SUB_NTSC_M;

    psAtvPdCtx->sAtvPiCtx.u4TvSubSysMask = TV_SYS_MASK_M;

#if !fcDUMMY_ANA_TUNER
    DigTunerBypassI2C(TRUE);
    AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), u1SubSysIdx);
    DigTunerBypassI2C(FALSE);
#endif

//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
    if (mcSEMA_CREATE(&t_escape_anaeu_mon_suspend, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("EN_ANA_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }

    psAtvPdCtx->fgBreakMonitor = TRUE;//init
    psAtvPdCtx->fgPIMoniStatus =FALSE; //init
    PD_StartMonitorEngine(psAtvPdCtx);  //start monitor engine
#endif
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  AnalogEU_TunerClose
*  Analog glue layer close function.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  DRVAPI_TUNER_OK : Analog glue close successfully.
*  @retval  DRVAPI_TUNER_ERROR : Analog glue close fail.
************************************************************************/
INT32 AnalogEU_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
#if fcENABLE_MONITOR || fcADD_HAL_LOCK
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
#endif

//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
    //psAtvPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
    SetBreakMonitor(psAtvPdCtx);
    fgMonitorThreadRunning = FALSE; //must set to FALSE before unlock sema mon_suspend
    mcMUTEX_UNLOCK(t_escape_anaeu_mon_suspend);
    PD_StopMonitorEngine();  // stop monitor engine
    mcMUTEX_DESTROY(t_escape_anaeu_mon_suspend);  // destory monitor thread

#endif
//lei131118 for fastboot refine
#if fcADD_HAL_LOCK
		if (mcSEMA_DESTROY(psAtvPdCtx->hHalLock) != OSR_OK)
		{
			mcSHOW_USER_MSG(("%s: mcSEMA_DESTROY(hHalLock) Fail!\n", __FUNCTION__));
			return (DRVAPI_TUNER_ERROR);
		}
#endif

    AnalogPIEU_TunerClose(cATUNER_PI_CTX(psAtvPdCtx));

    mcFREE_MEM(ptTDSpecificCtx);
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  AnalogEU_TunerAcq
*  Analog tuner acquisition function.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @param  pv_conn_info : Connection information.
*  @param  z_conn_info_len : Length of connection information.
*  @param  _BreakFct : Funxtion pointer of break function.
*  @param  pvArg : Break function parameter.
*  @retval  TRUE : Analog tuner acquisition successfully.
*  @retval  FALSE : Analog tuner acquisition fail.
************************************************************************/
BOOL AnalogEU_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    VOID* pv_conn_info,
                    SIZE_T z_conn_info_len,
                    x_break_fct _BreakFct,
                    VOID* pvArg)
{
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
UINT32  u4CurFreq = cTUNER_DEFAULT_FREQ;
UINT32  u4Freq, u4FreqBound;
UINT8   u1Path = SV_VP_MAX;
UINT8   u1ATVStatus = 0;
UINT8   u1ConnMode;
#if !(fcENABLE_MONITOR || 1)
mcLINT_UNUSED(u1ConnMode);
#endif
BOOL    fgLockSts = TRUE;

UINT32 u4FreqIndex, u4FreqTemp1, u4FreqTemp2;

//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
    SetBreakMonitor(psAtvPdCtx);
#endif

    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry
    mcSHOW_DBG_MSG(("Disable monitor process!!\n"));

    if ((e_conn_type == TUNER_CONN_TYPE_TER_ANA) ||
		   (/*COUNTRY_EU == GetTargetCountry() &&*/ (e_conn_type == TUNER_CONN_TYPE_TER_ANA_SCART_OUT)))
    {
        u4Freq = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq;
        u4FreqBound = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq_bound;
#ifndef CC_MT5391_AUD_3_DECODER
        AnaPd_ChangeChannel(u4Freq);
#endif
        u1ConnMode = GetAnaConnMode(((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->e_mod);
        // LC, set TV system into pd context when TV system is changed
        vSetTvSubSysIdx(psAtvPdCtx, ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_tv_sys_mask);
    }
   else
    {
        mcSHOW_USER_MSG(("%s: Wrong e_conn_type (%d)!\n", __FUNCTION__, e_conn_type));
        mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
        return FALSE;
    }

    //AnaPd_EnterTunerAcq();
    #if fcADD_TVD_SYNC
       vTvdHoldMode(SV_ON);
    #endif

   if(u1ConnMode == ATV_CONN_MODE_CHAN_CHANGE)
   {
	vTvd3dChannelChange();
   }
    mcSHOW_DBG_MSG(("[ATuner] %s (Freq=%uHz) (Freq_bound=%uHz)\n", __FUNCTION__, u4Freq, u4FreqBound));

#if fcTIMING_MEASURE
    u2TickStart_eu = mcGET_SYS_TICK();
#endif

    DigTunerBypassI2C(TRUE);
    u4Freq = u4Freq / 1000;  // transfer from Hz to KHz
    u4FreqBound = u4FreqBound / 1000;  // transfer from Hz to KHz

    if (i4FreqOffset)
    {
    	u4Freq += i4FreqOffset;
        mcSHOW_DBG_MSG((" (%d, %d)\n", u4Freq, i4FreqOffset));
    }

    // for Customer (MT5360) PCB, to avoid 503.25MHz spur from 8295 CLK (63M harmonic)
    // bypass CLK to 50M, Ken, 081016
    if (u4Freq > 58000)                                                                     //this part is nessary?
    {
       u4FreqIndex = u4Freq / 63000;
       u4FreqTemp1 = u4FreqIndex * 63000;
       u4FreqTemp2 = u4FreqTemp1 + 63000;
       if ((u4Freq <= (u4FreqTemp1 + 5000)) || (u4Freq >= (u4FreqTemp2 - 5000)))
       {
          if (!u1PODSetBypass)
          {
             mcSHOW_DBG_MSG(("[Atuner] AnalogEU_TunerAcq: PCMCIA_SetBypass(1)\n"));
             PCMCIA_SetBypass(1);
             u1PODSetBypass = 1;
          }
       }
       else
       {
          if (u1PODSetBypass)
          {
             mcSHOW_DBG_MSG(("[Atuner] AnalogEU_TunerAcq: PCMCIA_SetBypass(0)\n"));
             PCMCIA_SetBypass(0);
             u1PODSetBypass = 0;
          }
       }
    }

#if !fcDUMMY_ANA_TUNER
    if (AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4Freq, u1ConnMode)!= 0) /*jackon: why need use SV_TRUE?*/
    {
        mcSHOW_USER_MSG(("Freq %dKHz setting fail!\n", u4Freq));
        DigTunerBypassI2C(FALSE);
        AnaPd_ExitTunerAcq(FALSE);
        mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
        return FALSE;
    }
     // LC Chien, disable TV system setting repetition for freq fine-tune, 080313
    AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), AnalogPIEU_TunerGetSystem(cATUNER_PI_CTX(psAtvPdCtx)));
#endif

    DigTunerBypassI2C(FALSE);

#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG((" (1.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
#endif
/*
#if fcTUNE_NOT_START                  
    if (psAtvPdCtx->u1TunerAcqType == TUNE_FREQ_ONLY)
    {
        mcSHOW_DBG_MSG(("[ATuner] u1TunerAcqType == TUNE_FREQ_ONLY\n"));
        DigTunerBypassI2C(FALSE);
        AnaPd_ExitTunerAcq(TRUE);
        mcSHOW_USER_MSG(("VIF Lock: %d (Freq = %dKHz)\n",AnalogPIEU_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx)), u4Freq));
        mcSHOW_USER_MSG(("TVD Lock: %d\n", AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME)));
        mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
        return TRUE;
    }
#endif
*/
    /* Wait for TV-decoder to connect to tuner */
    if (u1ConnMode != ATV_CONN_MODE_CHAN_CHANGE)
    {
    INT32 i4_i = 0;

        for (i4_i = 0; i4_i < 30; i4_i++)
        {
            if (bGetSignalType(SV_VP_PIP) == (UINT8) SV_ST_TV)
            {
                u1Path = SV_VP_PIP;
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_PIP\n"));
                break;
            }
            else if (bGetSignalType(SV_VP_MAIN) == (UINT8) SV_ST_TV)
            {
                u1Path = SV_VP_MAIN;
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_MAIN\n"));
                break;
            }
            else
            {
                if (_BreakFct(pvArg))
                {
                    mcSHOW_DBG_MSG(("[ATuner] BreakFct\n"));
                    break;
                }
                mcDELAY_MS(50);                //50*30=1500 why need wait so much time???
                continue;
            }
        }
        if ((i4_i >= 30) || (u1Path == SV_VP_MAX))
        {
            mcSHOW_USER_MSG(("[ATuner] ERROR! without TV-decoder!\n"));
            AnaPd_ExitTunerAcq(FALSE);
            mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
            return FALSE;
        }
    }


    DigTunerBypassI2C(TRUE);

    u4CurFreq = u4Freq;

    // LC Chien, check fine-tune flag instead of _fgAutoSearch for EU-mode, 070326
    if ( (u1ConnMode == ATV_CONN_MODE_CHAN_SCAN) && (((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune) )
    {
    UINT32 u2Freq = u4Freq;
    UINT32 u2FreqBD = u4FreqBound;
#if fcTIMING_MEASURE
    UINT32 u4TickStart, u4TickDuration;
#endif

#if fcTIMING_MEASURE
        u4TickStart = mcGET_SYS_TICK();
        ucSearchNearbyLoopCnt = 0;
        //u4SearchNearbyTime = 0;
#endif

        u1ATVStatus = AnalogPIEU_TunerSearchNearbyFreq(cATUNER_PI_CTX(psAtvPdCtx), &u2Freq, _BreakFct, pvArg, u2FreqBD);
        //psAtvPdCtx->u1VIFLock = u1ATVStatus;

#if fcTIMING_MEASURE
        u4TickDuration = (mcGET_SYS_TICK() - u4TickStart) * mcGET_TICK_PERIOD();
        mcSHOW_DBG_MSG(("[Ch_ScanTime.AnalogEU_TunerAcq] SearchNearby=%3u ms\n", u4TickDuration));
        mcSHOW_DBG_MSG(("[Ch_ScanTime.AnalogEU_TunerAcq] SearchNearby Loop=%u\n", ucSearchNearbyLoopCnt));
        if ( u4TickDuration > (fcSEARCH_NEARBY_TIME_LIMIT*ucSearchNearbyLoopCnt+fcSEARCH_NEARBY_INIT_TIME_LIMIT))
        {
        mcSHOW_DBG_MSG(("[Ch_ScanTime.AnalogEU_TunerAcq] Driver ATP Fail : (SearchNearby time - %d) too long > %3ums\n", fcSEARCH_NEARBY_INIT_TIME_LIMIT, fcSEARCH_NEARBY_TIME_LIMIT));
#ifdef EU_AUTO_ATP
               mcSHOW_DRVAPI_MSG(("ATPFailBitMskB =%d\n",gATPFailBitMsk));
		 gATPFailBitMsk |=0X02 ;// Analog ATP Fail !
		 mcSHOW_DRVAPI_MSG(("ATPFailBitMskA =%x\n",gATPFailBitMsk));
#endif
        }
#endif

        if (mcTEST_BIT(u1ATVStatus, cpANA_LOCK_STS_VIF))
        {
            fgLockSts = AnaPd_CheckTVDLock(psAtvPdCtx, u2Freq, 'V');
            psAtvPdCtx->u1VIFLock = 2;
        }
        else
        {
            fgLockSts = FALSE;
            psAtvPdCtx->u1VIFLock = 0;
        }

        if (fgLockSts)
        {
            mcSHOW_DBG_MSG(("[ATuner] A_N_S,TVD notifyTvSig stable\n"));
            _Tvd_NotifyTVSig(u1Path, SV_VDO_STABLE);
        }
        else
        {
            mcSHOW_DBG_MSG(("[ATuner] A_N_N,TVD notifyTvSig nosignal\n"));
           _Tvd_NotifyTVSig(u1Path, SV_VDO_NOSIGNAL);
        }
        u4CurFreq = u2Freq;
    }
    else
    {
    UINT8   ii;

#if !fcDUMMY_ANA_TUNER
    if (u1ConnMode != ATV_CONN_MODE_FINE_TUNE)
    {
    /* Not do fine-tune, just wait VIF lock */
        for (ii = 0; ii < (200 / (cCHECK_VIF_DELAY * 10)); ii ++)
        {
            mcDELAY_MS(10 * cCHECK_VIF_DELAY);
            u1ATVStatus = AnalogPIEU_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx));
             // Check VIF really lock during channel change, LC Chien 070504
            if (u1ATVStatus >= 2)
                break;
        }
        psAtvPdCtx->u1VIFLock = u1ATVStatus;
      }
      else
      {
        // always notify signal stable when fine-tune, LC 071002 CR[DTV00106617]
           _Tvd_NotifyTVSig(u1Path, SV_VDO_STABLE);
            psAtvPdCtx->u1VIFLock = 2;
      }

    // Do TVD check when auto search enable but fine-tune disable, LC Chien 070326
        if (u1ConnMode == ATV_CONN_MODE_CHAN_SCAN)
        {
            if (AnaPd_CheckTVDLock(psAtvPdCtx, u4Freq, 'X'))           //WHAT'S "X"  ????
              _Tvd_NotifyTVSig(u1Path, SV_VDO_STABLE);    //move to Thomson/tuner.c		
			else
              _Tvd_NotifyTVSig(u1Path, SV_VDO_NOSIGNAL);
  
        }
        
#endif
    }
    DigTunerBypassI2C(FALSE);

    mcSHOW_DBG_MSG(("VIFLock: %d (%dKHz)\n",psAtvPdCtx->u1VIFLock,u4CurFreq));

#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG((" (2.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
#endif

    mcSHOW_DBG_MSG(("[ATuner] %s fine-tune: %dKHz\n", __FUNCTION__, u4CurFreq));
    psAtvPdCtx->Frequency = u4CurFreq;

//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
    // do not enable monitor function in fine tune mode
    if (!_BreakFct(pvArg) && (u1ConnMode == ATV_CONN_MODE_CHAN_CHANGE) && (((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune))
    {
        UINT32 u4FreqTemp;
        UINT32 u4FreqLower, u4FreqUpper;
        TUNER_ANA_ATTRIBUTE_T   *psTunerAnaAttribute;

        u4FreqTemp = u4Freq*1000;
        psTunerAnaAttribute = (TUNER_ANA_ATTRIBUTE_T *) (psAtvPdCtx->sAtvPiCtx.psTunerAnaAttr);
        u4FreqLower = psTunerAnaAttribute->ui4_lower_bound_freq;
        u4FreqUpper = psTunerAnaAttribute->ui4_upper_bound_freq;
        if ((u4FreqTemp <= u4FreqUpper) && (u4FreqTemp >= u4FreqLower))
        {
           ResumeMonitor(psAtvPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
           mcSHOW_DBG_MSG(("Enable monitor process!!\n"));
        }
    }
#endif
    if (psAtvPdCtx->u1VIFLock < 2)
    {
        AnaPd_ExitTunerAcq(FALSE);
        mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
        return FALSE;
    }

    AnaPd_ExitTunerAcq(TRUE);
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);

    return TRUE;

}

/************************************************************************
*  AnalogEU_TunerGetSync
*  Get current analog tuner connection status.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  1 : Analog tuner lock.
*  @retval  0 : Analog tuner unlock.
************************************************************************/
INT16 AnalogEU_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                          DRV_CONN_TYPE_T e_conn_type)
{

ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;

INT16   i2RetValue = 1;                     // Always in sync state


    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    #ifdef CC_FOR_SDAL
//    psAtvPdCtx->u1VIFLock = AnalogPI_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx), u4CurFreq);
    #endif

    if (!mcTEST_BIT(psAtvPdCtx->u1VIFLock, cpANA_LOCK_STS_TVD))

    {
        i2RetValue = 0;
    }
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);


    return i2RetValue;
}

/************************************************************************
*  AnalogEU_TunerGetSignal
*  Get current analog signal status.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @param  *_pSignal : Output data content pointer.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    SIGNAL *_pSignal)
{
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
// TUNER_POLARIZATION_T  e_pol;         /* Polarization. */
// TUNER_MODULATION_T    e_mod;         /* Modulation. */
// UINT16                ui2_dir;       /* Direction in 0.1 degrees. Valid value range '0..3599' (inclusive). */
// UINT16                ui2_gain;      /* Gain in 0.1 steps. Valid range '0..1000' (inclusive). */
UINT32 u4CurFreq;
UINT32  u4Freq;

    u4CurFreq = psAtvPdCtx->Frequency;

    u4Freq = u4CurFreq * 1000;

    mcSHOW_DBG_MSG(("[ATuner] %s: %04X (%d)", __FUNCTION__, u4CurFreq, u4Freq));
    if ((e_conn_type == TUNER_CONN_TYPE_CAB_ANA)
//#ifdef TUNER_SUPPORT_SCART_OUT
//        || (e_conn_type == TUNER_CONN_TYPE_CAB_ANA)
//        || (COUNTRY_EU == GetTargetCountry() && (e_conn_type == TUNER_CONN_TYPE_CAB_ANA))
//#endif
    )
    {
        TUNER_CAB_ANA_TUNE_INFO_T * pt_cab_ana = (TUNER_CAB_ANA_TUNE_INFO_T *)_pSignal;
        pt_cab_ana->ui4_freq = u4Freq;
        pt_cab_ana->e_mod = MOD_PSK_8;
        mcSHOW_DBG_MSG((" (%d)\n", pt_cab_ana->ui4_freq));
    }
    else if ((e_conn_type == TUNER_CONN_TYPE_TER_ANA)
//#ifdef TUNER_SUPPORT_SCART_OUT
//        || (e_conn_type == TUNER_CONN_TYPE_TER_ANA)
//        || (COUNTRY_EU == GetTargetCountry() && (e_conn_type == TUNER_CONN_TYPE_TER_ANA))
//#endif
    )
    {
        TUNER_TER_ANA_TUNE_INFO_T * pt_ter_ana = (TUNER_TER_ANA_TUNE_INFO_T *)_pSignal;
        pt_ter_ana->ui4_freq = u4Freq;
        pt_ter_ana->e_mod = MOD_PSK_8;
        mcSHOW_DBG_MSG((" (%d)\n", pt_ter_ana->ui4_freq));
    }
}
//-----------------------------------------------------------------------------
/*
 *  AnalogEU_TunerGetSignalLevel
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   Signal quality, Range is 0~100.
 */
//-----------------------------------------------------------------------------
UINT8  AnalogEU_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                DRV_CONN_TYPE_T e_conn_type)
{
UINT8 u1Value;
    /* Provided by TVD/KY lin
    > 60dB: 99
      55dB: 97
      50dB: 91
      45dB: 84
      40dB: 72
      35dB: 55
      30dB: 43
      25dB: 23
      signal off: 0
    */
    u1Value = bTvdSignalStrengthLevel(0); // bMode = 0, return 0~100
    return (u1Value);
}

//-----------------------------------------------------------------------------
/*
 *  AnalogEU_TunerGetSignalLeveldBm
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   Signal quality (in dB).
 */
//-----------------------------------------------------------------------------
INT16 AnalogEU_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
UINT8 u1Value;

    u1Value = bTvdSignalStrengthLevel(1); // bMode = 1, return in dB
    return (u1Value);

}

/************************************************************************
*  AnalogEU_TunerGetSignalPER
*  Get current analog signal packet error rate.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 0.
************************************************************************/
UINT16  AnalogEU_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);
}

/************************************************************************
*  AnalogEU_TunerGetSignalSNR
*  Get current analog signal SNR.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 0.
************************************************************************/
UINT16 AnalogEU_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);                             // return 0 first
}

/************************************************************************
*  AnalogEU_TunerSetMpgFmt
*  Get current analog signal mpeg format.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  *pt_mpeg_fmt : Output data content pointer.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                            MPEG_FMT_T *pt_mpeg_fmt)
{
    return;
}

/************************************************************************
*  AnalogEU_TunerGetVer
*  Get current analog tuner driver version.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  Current analog tuner driver version.
************************************************************************/
CHAR *AnalogEU_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{//char *pStr;
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;

    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    mcSHOW_USER_MSG(("[ATuner] %s\n", AnalogPIEU_TunerGetVer(cATUNER_PI_CTX(psAtvPdCtx))));

    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
	//ITuner_OP(pTCtx, itGetVer, 0, &pStr);
    //return ((char*)pStr);
    return "Analog_Tuner";
}

/************************************************************************
*  AnalogEU_TunerNimTest
*  Test function of tuner driver.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  Always 0.
************************************************************************/
INT32 AnalogEU_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));

    return 0;
}

/************************************************************************
*  AnalogEU_TunerSetRegSetting
*  Set tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  ucRegSet : Number of setting regsiters.
*  @param  ucRegAddr : Register address.
*  @param  ucRegValue : Register value.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet,
                            UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AnalogEU_TunerShowRegSetting
*  Show tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AnalogEU_TunerTestI2C
*  Test tuner I2C bus.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AnalogEU_TunerRegRead
*  Read tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @param  ucRegAddr : Register address.
*  @param  *pucBuffer : Output data content pointer.
*  @param  ucByteCount : Number of setting regsiters.
*  @retval  Always 0.
************************************************************************/
INT32 AnalogEU_TunerRegRead(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type,
                    UCHAR ucRegAddr, UCHAR *pucBuffer, U16 ucByteCount)
{
    return 0;
}

/************************************************************************
*  AnalogEU_TunerDisc
*  Disconnect analog tuner driver.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  void.
************************************************************************/
VOID  AnalogEU_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                DRV_CONN_TYPE_T e_conn_type)
{
//#if fcENABLE_MONITOR
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
//#endif

    mcSHOW_DBG_MSG(("[Atuner] %s()\n", __FUNCTION__));

//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
    SetBreakMonitor(psAtvPdCtx);
#endif

   // for Customer (MT5360) PCB, to avoid 503.25MHz spur from 8295 CLK (63M harmonic)
   // bypass CLK to 50M, Ken, 081016
   if (u1PODSetBypass)
   {
      mcSHOW_DBG_MSG(("[Atuner] AnalogEU_TunerDisc: PCMCIA_SetBypass(0)\n"));
      PCMCIA_SetBypass(0);
      u1PODSetBypass = 0;
   }

//#ifdef TUNER_SUPPORT_SCART_OUT
    // Please do not do anything for SCART-feature enabled.
    // When input source change between TV and SCART, it required SCART-Out ATV to be not discontinuity.
    return;
//#endif
}

/************************************************************************
*  AnalogEU_TunerCommand
*  Analog tuner nim command.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  i4Argc : Nim command parameter number.
*  @param  aszArgv : Nim command parameter.
*  @retval  void.
************************************************************************/
void AnalogEU_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                      INT32 i4Argc, const CHAR** aszArgv)
{
#if fcADD_HAL_LOCK
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
#endif
UCHAR   ucCmdId = 0;

    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    if (i4Argc < 1)
    {
        mcSHOW_USER_MSG(("Cmd: \n"));
        mcSHOW_USER_MSG(("\t o               FreqOffset\n"));
        mcSHOW_USER_MSG(("\t n               new scan step\n"));
        mcSHOW_USER_MSG(("\t js [step (kHz)] Set jump small step\n"));
        mcSHOW_USER_MSG(("\t jm [step (kHz)] Set jump middle step\n"));
        mcSHOW_USER_MSG(("\t jb [step (kHz)] Set jump middle back step\n"));
        mcSHOW_USER_MSG(("\t e [1/0]         enable VIFL bit check\n"));
        mcSHOW_USER_MSG(("\t l  [times]      Set check-VIF-lock time\n"));

    //PK
    #if fcTEST_I2C_DETACH
        mcSHOW_USER_MSG(("\t i               fgEnI2CDetach\n"));
    #endif
    #if fcTIMING_MEASURE
        mcSHOW_USER_MSG(("\t d               u2Delay\n"));
    #endif
    //    mcSHOW_USER_MSG(("\t u Light bar reduction\n"));
#if 0  //Jackson remove fcADD_CUST_IF to fix zero used for undefined preprocessing identifier
    #if fcADD_CUST_IF && fcINTERNAL_TEST
        mcSHOW_USER_MSG(("\t qf u4TunerCtrlAnaForce: Force DRVCUST_OptGet(eTunerCtrlAna)\n"));
    #endif
#endif
    }

    if (i4Argc > 0)
    {
        ucCmdId = *((CHAR *) aszArgv[0]);

        switch (ucCmdId)
        {
            case 'l':
                if (i4Argc > 1)
                {
                    u1CheckVifLoop = StrToInt(aszArgv[1]);
                }
                mcSHOW_USER_MSG(("u1CheckVifLoop = %d\n", u1CheckVifLoop));
                break;

            case 'e':
            {
                UINT8 ii;
                if(i4Argc >1)
                {
                    ii = StrToInt(aszArgv[1]);
                    vSetVIFLBitCheckEnable(ii);
                    mcSHOW_USER_MSG(("BIT VIFL Check is set as %d.",ii));
                }
                else
                {
                    if(GetVIFLBitCheckEnable())
                    {
                        mcSHOW_USER_MSG(("BIT VIFL Check is enabled."));
                    }
                    else
                    {
                        mcSHOW_USER_MSG(("BIT VIFL Check is disabled."));
                    }
                }
                break;
            }
            case 'j':
            {
                UINT8   ucCmdExt;

		//add by liuqu,20081111
		 ITUNER_CTX_T *pDigiTunerCtx = ITunerGetCtx();

                ucCmdExt = ((CHAR *) aszArgv[0])[1];

                if (ucCmdExt == 's')
                {
                    if (i4Argc > 1) {
                        vSetJumpSmallStep(pDigiTunerCtx,StrToInt(aszArgv[1]));
                    }
                    else{
                        mcSHOW_USER_MSG(("jump small step = %d.", GetJumpSmallStep(pDigiTunerCtx)));
                    }
                }
                else if (ucCmdExt == 'm')
                {
                    if (i4Argc > 1) {
                        vSetJumpMiddleStep(pDigiTunerCtx,StrToInt(aszArgv[1]),SCAN_DIRECTION_INC);
                    }
                    else{
                        mcSHOW_USER_MSG(("jump middle step = %d.", GetJumpMiddleStep(pDigiTunerCtx,SCAN_DIRECTION_INC)));
                    }
                }
		 else if(ucCmdExt == 'b')
		 {
                    if (i4Argc > 1) {
                        vSetJumpMiddleStep(pDigiTunerCtx,StrToInt(aszArgv[1]),SCAN_DIRECTION_DEC);
                    }
                    else{
                        mcSHOW_USER_MSG(("jump middle back step = %d.", GetJumpMiddleStep(pDigiTunerCtx,SCAN_DIRECTION_DEC)));
                    }
                }


            }
            break;

            case 'o':
                if (i4Argc > 1)
                {
                //    i4FreqOffset = axtoi((CHAR *) aszArgv[1]);
                    i4FreqOffset = StrToInt(aszArgv[1]);
                    if (i4Argc > 2)
                        i4FreqOffset = -i4FreqOffset;
                }
                mcSHOW_USER_MSG(("i4FreqOffset = %d\n", i4FreqOffset));
                break;

           case 'n':
                if (i4Argc > 1)
                {
                    fgNewScanStep = StrToInt(aszArgv[1]);
                }
                mcSHOW_USER_MSG(("fgNewScanStep = %d\n", fgNewScanStep));
                break;

        #if fcTEST_I2C_DETACH
            case 'i':
                if (i4Argc > 1)
                {
                    fgEnI2CDetach = StrToInt(aszArgv[1]);
                }
                mcSHOW_USER_MSG(("fgEnI2CDetach = %d\n", fgEnI2CDetach));
                break;
        #endif

        #if fcTIMING_MEASURE
            case 'd':
                if (i4Argc > 1)
                {
                    u2Delay = StrToInt(aszArgv[1]);
                }
                mcSHOW_USER_MSG(("u2Delay = %d\n", u2Delay));
                break;
        #endif

            case 'u':
        #if 0
                if (i4Argc > 1)
                {
                    vSetTunerTuning(StrToInt(aszArgv[1]));
                    DigTunerBypassI2C(TRUE);
                    AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), psAtvPdCtx->Frequency, ATV_CONN_MODE_CHAN_CHANGE);
                    DigTunerBypassI2C(FALSE);
                }
        #endif
                break;
#if 0  //Jackson remove fcADD_CUST_IF to fix zero used for undefined preprocessing identifier
        #if fcADD_CUST_IF && fcINTERNAL_TEST    // [
            case 'q':
                {
                UINT8   ucCmdExt;

                    ucCmdExt = ((CHAR *) aszArgv[0])[1];

                    if (ucCmdExt == 'f')
                    {
                        if (i4Argc > 1)
                            u4TunerCtrlAnaForce =  axtoi((CHAR *) aszArgv[1]);
                        mcSHOW_USER_MSG(("u4TunerCtrlAnaForce=0x%02X", u4TunerCtrlAnaForce));
                    }
                }
                break;
        #endif  // ]
#endif
            default:
            //    mcSHOW_USER_MSG(("Invalid parameter!\n"));
                break;
        }
    }
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
#ifndef CC_MT5381
  //  AnalogEU_TunerAtdCommand(ptTDSpecificCtx, i4Argc, aszArgv);//liuqu,20081112, clear  ("Invalid parameter!\n")
#endif
}

/************************************************************************
*  AnalogEU_TunerAtdCommand
*  Analog tuner ATD command.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  i4Argc : ATD command parameter number.
*  @param  aszArgv : ATD command parameter.
*  @retval  void.
************************************************************************/
VOID AnalogEU_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv)
{
    ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
    UCHAR   ucCmdId = 0;
    UINT32  u4CurFreq,StartFreq,EndFreq;
    UINT32  u4Freq;    
    UINT8   u1SubSysIdx;
    UINT8   ii, u1Count,u1IFFreq;
#ifdef CC_SUPPORT_SIF_BYPASS            
    UCHAR   ucPar, ucData[256];
    U16 	u2RegAddr;
#endif


    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    if (i4Argc < 1)
    {
        mcSHOW_USER_MSG(("Cmd: \n"));
        mcSHOW_USER_MSG(("\t c:              Show Current Freq (kHz)\n"));    //CR 00012779, for Hisense's request, LC Chien 070514
        mcSHOW_USER_MSG(("\t k [TopValue]:   Set ATV TOP \n"));
        mcSHOW_USER_MSG(("\t f [Freq (kHz)]: AnaTunerSetFreq\n"));
        mcSHOW_USER_MSG(("\t s [SubSysIdx]:  Color / Sound sub-system\n"));
        mcSHOW_USER_MSG(("\t v [Freq (kHz)]: Check Lock Status\n"));
        mcSHOW_USER_MSG(("\t h [StartFreq Count EndFreq]: TunerSearchNearbyFreq\n"));
#ifdef CC_SUPPORT_SIF_BYPASS
        mcSHOW_USER_MSG(("\t w [RegAddr] [Value]: write ATD register\n"));  //Test OK   
        mcSHOW_USER_MSG(("\t r [RegAddr] [Num]:   read  ATD register\n")); //Test OK
        mcSHOW_USER_MSG(("\t o:             Turn on the CVBS out\n"));
#endif
        mcSHOW_USER_MSG(("\t t [StartFreq(kHz) EndFreq(kHz) SubSysIdx]:   test S-curve --StartFFreq to EndFreq\n"));
	#ifdef ROW_SET_TOP_TOEEPROM
        mcSHOW_USER_MSG(("\t q [TopValue]: Set TOP to EEPROM\n"));
        mcSHOW_USER_MSG(("\t n read EEPROM TOP value\n"));
	#endif
//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
        mcSHOW_USER_MSG(("\t mp:             Pause monitor thread\n"));
        mcSHOW_USER_MSG(("\t mr:             Resume monitor thread\n"));
#endif
        mcSHOW_USER_MSG(("\t z:               Query/Set tuner IF frequency 0-38000KHz 1-38900KHz\n"));
    }

    if (i4Argc > 0)
    {
        ucCmdId = *((CHAR *) aszArgv[0]);

        switch (ucCmdId)
        {                            
		   case 'z':
                if (i4Argc > 1)
                {
                    u1IFFreq = (UINT8)StrToInt(aszArgv[1]);
                   if(u1IFFreq==0)
				   {
				     ITunerGetCtx()->u2IF_Freq_A=38000;
				   }
				   
				   if(u1IFFreq==1)
				   {
				     ITunerGetCtx()->u2IF_Freq_A=38900; 
				   }
                }
				 mcSHOW_USER_MSG(("Current tuner IF frequency %d KHz\n", ITunerGetCtx()->u2IF_Freq_A));
				 break;
            case 'c':
                u4CurFreq = psAtvPdCtx->Frequency;
                mcSHOW_USER_MSG(("Current Freq = %d kHz\n", u4CurFreq));
                break;

            case 'k':     //change to k to keep allience with ROW LATIM
             	{
		        ITUNER_CTX_T *pDigiTunerCtx = ITunerGetCtx();
		        SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);
                if (i4Argc < 2)
                {
                    mcSHOW_USER_MSG(("DTV TOP = 0x%02X\n", pEUCtx->m_Ana_Top));
                }
                else
                {
                    pEUCtx->m_Ana_Top = (U8)StrToInt(aszArgv[1]);

                    AnalogPIEU_TunerSetFreq(  cATUNER_PI_CTX(psAtvPdCtx), psAtvPdCtx->Frequency, ATV_CONN_MODE_CHAN_CHANGE);
                    AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), psAtvPdCtx->sAtvPiCtx.u1SubSysIdx);

                    mcSHOW_USER_MSG(("set ATV TOP = 0x%02X\n", pEUCtx->m_Ana_Top));
                }

                break;
            	}
            case 'f':
                u4CurFreq = psAtvPdCtx->Frequency;
                if (i4Argc > 1)
                {
                    u4CurFreq = StrToInt(aszArgv[1]);
                    psAtvPdCtx->Frequency = u4CurFreq;
                }
                AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4CurFreq, ATV_CONN_MODE_CHAN_CHANGE);
                mcSHOW_USER_MSG(("Set Frequency = %d KHz\n", u4CurFreq));
                break;

            case 't':
                u4CurFreq = psAtvPdCtx->Frequency;
                u1SubSysIdx = psAtvPdCtx->sAtvPiCtx.u1SubSysIdx;
                if (i4Argc > 1)
                {
                 StartFreq = StrToInt(aszArgv[1]);
                 EndFreq = StrToInt(aszArgv[2]);
                 u1SubSysIdx = StrToInt(aszArgv[3]);
                 psAtvPdCtx->sAtvPiCtx.u1SubSysIdx = u1SubSysIdx;
                 u4CurFreq = StartFreq;
                 mcSHOW_USER_MSG(("StartFreq =%d,EndFreq=%d,u1SubSysIdx=%d\n", StartFreq,EndFreq,u1SubSysIdx));
                 while (u4CurFreq <= EndFreq)
                   {
                   psAtvPdCtx->Frequency = u4CurFreq;
                   AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4CurFreq, u1SubSysIdx);
                   AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), u1SubSysIdx);   // for S-curve log
                   u4CurFreq +=50;
                   }
                }
                break;
                
#ifdef CC_SUPPORT_SIF_BYPASS                  
            case 'r':	
				if (i4Argc < 2)
				 	break;
				else if (i4Argc == 2)
                   ucPar = 1;
				else
                	ucPar = (U8)StrToInt(aszArgv[2]);
				
				u2RegAddr = (U16)StrToInt(aszArgv[1]);
        
                if(FALSE == fg_ATD_Initialized)
                {
                    fgSifBypassEnabled = TRUE;
                    psAtdDemodCtx = ATD_DemodCtxCreate();
                    if (NULL == psAtdDemodCtx)        
                    {
                        return (DRVAPI_TUNER_ERROR);
                    } 

                    psAtdDemodCtx->pTCtx = ITunerGetCtx();
                    if (NULL == psAtdDemodCtx->pTCtx)		
                    {
                        return (DRVAPI_TUNER_ERROR);
                    } 
                    ITuner_Init(psAtdDemodCtx->pTCtx);                                                                          

                    // Initialize ATD context & download FW
                    if (ATD_Initialize(psAtdDemodCtx, 0x92, TRUE))
                    {
                        mcSHOW_USER_MSG(("ATD_Initialize failed\n"));
                        return (DRVAPI_TUNER_ERROR);
                    }
                    else 
                    {
                        mcSHOW_USER_MSG(("ATD_Initialize successful !\n"));
                    }   
                    fg_ATD_Initialized = TRUE;
                }   

                if (ATD_GetReg(psAtdDemodCtx, u2RegAddr, ucData, ucPar))
                {
                    mcSHOW_USER_MSG(("MTATD Get Register 0x%03X Fail!\n", u2RegAddr));
                    break;
                }
                for (ii=0; ii<ucPar; ii++)
                {
                    mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X\n", u2RegAddr+ii, ucData[ii]));
                }
                break;
                
            case 'w':
        		if (i4Argc < 3)
        			break;
        	
        		u2RegAddr = (U16)StrToInt(aszArgv[1]);
        		ucPar = (U8)StrToInt(aszArgv[2]);
                if(FALSE == fg_ATD_Initialized)
                {
                    psAtdDemodCtx = ATD_DemodCtxCreate();
                    if (NULL == psAtdDemodCtx)        
                    {
                        return (DRVAPI_TUNER_ERROR);
                    } 

                    psAtdDemodCtx->pTCtx = ITunerGetCtx();
                    if (NULL == psAtdDemodCtx->pTCtx)		
                    {
                        return (DRVAPI_TUNER_ERROR);
                    } 
                    ITuner_Init(psAtdDemodCtx->pTCtx);                                                                          

                    // Initialize ATD context & download FW
                    if (ATD_Initialize(psAtdDemodCtx, 0x92, TRUE))
                    {
                        mcSHOW_USER_MSG(("ATD_Initialize failed\n"));
                        return (DRVAPI_TUNER_ERROR);
                    }
                    else 
                    {
                        mcSHOW_USER_MSG(("ATD_Initialize successful !\n"));
                    }   
                    fg_ATD_Initialized = TRUE;
                } 
                
        		if (ATD_SetReg(psAtdDemodCtx, u2RegAddr, &ucPar, 1))
        		{
        		    mcSHOW_USER_MSG(("MTATD Set Register 0x%03X Fail!\n", u2RegAddr));
        			break;
        		}
        		mcSHOW_USER_MSG(("Register 0x%03X = 0x%02X, OK!\n", u2RegAddr, ucPar));
        		break;    
            case 'o':
                mcSHOW_USER_MSG(("Turn on the CVBS Out function\n"));
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)                 
                IO_WRITE32(0xF0061000,0x300, 0x6ca00000);
                IO_WRITE32(0xF002d000,0x604, 0x00000000);
                IO_WRITE32(0xF002d000,0x634, 0x000000ff);                
                IO_WRITE32(0xF000d000,0x10c, 0x00605000);
                IO_WRITE32(0xF002d000,0x678, 0x000d0000);
#endif                
				break;
#endif                

            case 's':
                u1SubSysIdx = psAtvPdCtx->sAtvPiCtx.u1SubSysIdx;
                if (i4Argc > 1)
                {
                //    _SubSysIdx = StrToInt(aszArgv[1]);
                    u1SubSysIdx = StrToInt(aszArgv[1]);
                    if (u1SubSysIdx < SYS_SUB_TOTAL)
                    {
                        psAtvPdCtx->sAtvPiCtx.u1SubSysIdx = u1SubSysIdx;                        
                        DigTunerBypassI2C(TRUE);
                        AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), u1SubSysIdx);
                        DigTunerBypassI2C(FALSE);
						mcSHOW_USER_MSG(("current SubSysIdx = %d\n", u1SubSysIdx));
#ifdef CC_SUPPORT_SIF_BYPASS                        
                        mcSHOW_USER_MSG(("Bypass sif to audio function\n"));
                        if(FALSE == fg_ATD_Initialized)
                        {
                            psAtdDemodCtx = ATD_DemodCtxCreate();
                            if (NULL == psAtdDemodCtx)        
                            {
                                return (DRVAPI_TUNER_ERROR);
                            } 

                            psAtdDemodCtx->pTCtx = ITunerGetCtx();
                            if (NULL == psAtdDemodCtx->pTCtx)		
                            {
                                return (DRVAPI_TUNER_ERROR);
                            } 
                            ITuner_Init(psAtdDemodCtx->pTCtx);                                                                          

                            // Initialize ATD context & download FW
                            if (ATD_Initialize(psAtdDemodCtx, 0x92, TRUE))
                            {
                                mcSHOW_USER_MSG(("ATD_Initialize failed\n"));
                                return (DRVAPI_TUNER_ERROR);
                            }
                            else 
                            {
                                mcSHOW_USER_MSG(("ATD_Initialize successful !\n"));
                            }   
                            fg_ATD_Initialized = TRUE;
                        }

                        psAtdDemodCtx->u1SubSysIdx = u1SubSysIdx + MOD_ANA_TYPE_BEGIN;
                        ATD_ChipInit(psAtdDemodCtx);
                        ATD_SetSystem(psAtdDemodCtx, psAtdDemodCtx->u1SubSysIdx);
                        ATD_StartAcq(psAtdDemodCtx, FALSE);
                        ATD_BypassSIF2Aud(psAtdDemodCtx,psAtdDemodCtx->u1SubSysIdx);
#endif                        
                    }
                }
                else
                {
                    mcSHOW_USER_MSG(("\t %d: PAL_BG  \n", SYS_SUB_PAL_BG));
                    mcSHOW_USER_MSG(("\t %d: PAL_DK  \n", SYS_SUB_PAL_DK));
                    mcSHOW_USER_MSG(("\t %d: PAL_I   \n", SYS_SUB_PAL_I));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L \n", SYS_SUB_SECAM_L));
                    mcSHOW_USER_MSG(("\t %d: SECAM_L1\n", SYS_SUB_SECAM_L1));
                    mcSHOW_USER_MSG(("\t %d: NTSC_M  \n", SYS_SUB_NTSC_M));
                }
                mcSHOW_USER_MSG(("_SubSysIdx = %d\n", u1SubSysIdx));
#ifdef  EU_AUTO_ATP


             if( gATP_TEST_ITEM_BIT &0x08)
             	{
             	UINT8 bVIFLock,bTVDLock;
		u4CurFreq = psAtvPdCtx->Frequency;

               bVIFLock = AnalogPIEU_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx));
		 bTVDLock =  AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME);

              mcSHOW_DBG_MSG(("##Start\n##CurFreq : %d\n##VIFLOCK : %d\n##TVDLOCK : %d\n##SubSysIdx : %d\n",u4CurFreq,bVIFLock,bTVDLock,u1SubSysIdx));

		     if (gATP_TEST_ITEM_BIT & 0x10)
     	           {
                     if (!gATPFailBitMsk)
         	       {
                      mcSHOW_DBG_MSG(("##ATPFAIL : 0\n##End\n"));
         	       }
	             else
	  	      {
                      mcSHOW_DRVAPI_MSG(("##ATPFAIL : 1 , Msk =%x\n##End\n",gATPFailBitMsk));
		         //reset to 0
		        gATPFailBitMsk = 0;
	  	      }
     	           }
		    else
		    	{
                     mcSHOW_DRVAPI_MSG(("##End\n"));
		    	}
             	}

		 else if(gATP_TEST_ITEM_BIT & 0x01)
		{
                mcSHOW_DBG_MSG(("##Start\n##SubSysIdx : %d\n##End\n",u1SubSysIdx));
		}

#endif
                break;

            case 'v':
		{
                u4CurFreq = psAtvPdCtx->Frequency;
                if (i4Argc > 1)
                {
                    u4CurFreq = StrToInt(aszArgv[1]);
                    psAtvPdCtx->Frequency = u4CurFreq;
                    DigTunerBypassI2C(TRUE);
                    AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4CurFreq, ATV_CONN_MODE_CHAN_CHANGE);
                    DigTunerBypassI2C(FALSE);                    
                }
                DigTunerBypassI2C(TRUE);
                mcSHOW_USER_MSG(("VIF Lock: %d (Freq = %dKHz)\n",
                                 AnalogPIEU_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx)),
                                 u4CurFreq));
                DigTunerBypassI2C(FALSE);
                mcSHOW_USER_MSG(("TVD Lock: %d\n", AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME)));
				#ifdef  EU_AUTO_ATP
               UINT8 bVIFLock,bTVDLock;
               bVIFLock = AnalogPIEU_TunerCheckVIFLock(cATUNER_PI_CTX(psAtvPdCtx));
		 bTVDLock =  AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME);

		  if (gATP_TEST_ITEM_BIT & 0x04)
		 {   //only available in nim.atd p 4(bit 2 =1)
		    if ((bVIFLock ==2) && (bTVDLock ==1))
		   {
                   mcSHOW_DBG_MSG(("##Start\n##Video : ON\n##End\n"));
		    }
		    else
		   {
                  mcSHOW_DBG_MSG(("##Start\n##Video : OFF\n##End\n"));
		    }
		}
		  else if (gATP_TEST_ITEM_BIT & 0x01)
		  {
		      mcSHOW_DBG_MSG(("##Start\n##VIFLOCK : %d\n##TVDLOCK : %d\n##End\n", bVIFLock,  bTVDLock));
		  }
#endif
            	}
                break;
     // for ROW project
     #ifdef ROW_SET_TOP_TOEEPROM
            case 'q':
            {
                UINT8  u1Val = 18;
                if (i4Argc > 1)
                {
                   u1Val = StrToInt(aszArgv[1]);
                //    pDigiTunerCtx->m_Ana_Top =(U8) u1Val;
                }

                //    AnalogPIEU_TunerSetFreq(  cATUNER_PI_CTX(psAtvPdCtx), psAtvPdCtx->Frequency, ATV_CONN_MODE_CHAN_CHANGE);
                //    AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), psAtvPdCtx->sAtvPiCtx.u1SubSysIdx);

                //    mcSHOW_USER_MSG(("set ATV TOP = 0x%02X\n", pDigiTunerCtx->m_Ana_Top));
                if(EEPNPTV_Write((UINT64)EEP_eTunerAnaDemoTop, (UINT32)(void *)&u1Val, 1))
                {
                    mcSHOW_USER_MSG(("Write EEPROM fail!!\n"));
                }
                else
                {
                    mcSHOW_USER_MSG(("Write EEPROM AnaDemoTop = %d\n", u1Val));
                }
            }
            break;

            case 'n':
            {
                UINT8  u1Val = 0;
                if(EEPNPTV_Read((UINT64)EEP_eTunerAnaDemoTop, (UINT32)(void *)&u1Val, 1))
                {
                    mcSHOW_USER_MSG(("Read EEPROM fail!!\n"));
                }
                else
                {
                    mcSHOW_USER_MSG(("Read EEPROM AnaDemoTop = %d\n", u1Val));
                }
            }
	    break;
	#endif
            case 'h':
            {
                // initialize paramenters
                UINT32  u4CurFreqTmp;
                u4CurFreq = psAtvPdCtx->Frequency;
                u4Freq = u4CurFreq;
                u1Count = 1;
                // update parameters if necessary
                switch (i4Argc) {
                case 4:
                    u4Freq = StrToInt(aszArgv[3]);
                case 3:
                    u1Count = StrToInt(aszArgv[2]);
                case 2:
                    u4CurFreq = StrToInt(aszArgv[1]);
                    if (i4Argc <= 3) {
                        u4Freq = u4CurFreq;
                    }
                    // force search nearby range <= 8M
                    else if ((S32)u4CurFreq - (S32)u4Freq > 50000) {
                        u4Freq = u4CurFreq - 50000;
                    }
                    else if ((S32)u4Freq - (S32)u4CurFreq > 50000) {
                        u4Freq = u4CurFreq + 50000;
                    }
                default:
                    break;
                }
                // do search-near-by
                DigTunerBypassI2C(TRUE);
                AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4CurFreq, ATV_CONN_MODE_CHAN_CHANGE);

                mcSHOW_USER_MSG(("Search NearBy from %d to %d KHz\n", u4CurFreq, u4Freq));
                u4CurFreqTmp = u4CurFreq;
                for(ii=0; ii<u1Count; ii++)
                {
                    mcSHOW_USER_MSG(("the %d th time SearchNearFreq.\n",ii));
                    u4CurFreq = u4CurFreqTmp;	 //reset initial freq
                    AnalogPIEU_TunerSearchNearbyFreq(cATUNER_PI_CTX(psAtvPdCtx), &u4CurFreq, TunerNullBreak, NULL, u4Freq);

                    //need to tune back to a bad freq,or tuner check VIF will lock and never move the 2nd time
                    if(ii <(u1Count -1))
                    {
                        AnalogPIEU_TunerSetFreq(cATUNER_PI_CTX(psAtvPdCtx), u4Freq, ATV_CONN_MODE_CHAN_CHANGE);
                    }
                }
                psAtvPdCtx->Frequency = u4CurFreq;
                DigTunerBypassI2C(FALSE);

                break;
            }
//#if !defined(CC_MTAL) && !defined(CC_SDAL)
#ifdef ANAEU_ENABLE_MONITOR
            case 'm':
                {
                    UINT8   ucCmdExt;

                    ucCmdExt = ((CHAR *) aszArgv[0])[1];

                    if (ucCmdExt == 'p')
                    {
                        //psAtvPdCtx->fgBreakMonitor = TRUE;
                        SetBreakMonitor(psAtvPdCtx);
                    }
                    else if (ucCmdExt == 'r')
                    {
                        //psAtvPdCtx->fgBreakMonitor = FALSE;
                        //mcMUTEX_UNLOCK(t_escape_anaeu_mon_suspend);
                        ResumeMonitor(psAtvPdCtx);
                    }
                }
                break;
#endif
  #ifdef EU_AUTO_ATP
            case 'z':

                if (i4Argc > 1)
                {
                    gATP_TEST_ITEM_BIT = StrToInt(aszArgv[1]);
                }
		break;
#endif
            default:
                mcSHOW_USER_MSG(("Invalid parameter!\n"));
                break;
        }
    }
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
}

/************************************************************************
*  AnalogEU_TunerGetAttribute
*  Get analog tuner driver attribute.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_get_type : Attribute type.
*  @param  *pvAnaAttribute : Output data content pointer.
*  @param  *pzAnaAttributeLen : Output data length pointer.
*  @retval  RMR_OK : Get analog tuner driver attribute Successfully.
*  @retval  RMR_DRV_INV_GET_INFO : Get analog tuner driver attribute fail.
************************************************************************/
INT16 AnalogEU_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_GET_TYPE_T  e_get_type,
                           VOID *pvAnaAttribute,
                           SIZE_T *pzAnaAttributeLen)
{

ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;


UINT32  u4TvSubSysMask;


    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    switch (e_get_type)
    {

        case TUNER_GET_TYPE_TV_SYS_MASK:
        {
            u4TvSubSysMask = psAtvPdCtx->sAtvPiCtx.u4TvSubSysMask;
            mcSHOW_DBG_MSG(("\n%s: u1SubSysIdx = %d, u4TvSubSysMask = %04X\n", __FUNCTION__,
                            psAtvPdCtx->sAtvPiCtx.u1SubSysIdx, u4TvSubSysMask));
            *((UINT32 *) pvAnaAttribute) = u4TvSubSysMask;
            break;
        }

        case TUNER_GET_TYPE_ANA_ATTRIBUTE:
        {
        TUNER_ANA_ATTRIBUTE_T   *psTunerAnaAttribute;

            psTunerAnaAttribute = (TUNER_ANA_ATTRIBUTE_T *) pvAnaAttribute;
            *psTunerAnaAttribute = *((TUNER_ANA_ATTRIBUTE_T *) (psAtvPdCtx->sAtvPiCtx.psTunerAnaAttr));
            break;
        }

        case TUNER_GET_TYPE_TV_SYS_GROUP:
        {
        UINT8   u1TvSysGroupIdx;
        TUNER_TV_SYS_GROUP_INFO_T   *psTvSysGroup;

            psTvSysGroup = (TUNER_TV_SYS_GROUP_INFO_T *) pvAnaAttribute;
            u1TvSysGroupIdx = psTvSysGroup->ui1_tv_sys_group_idx;
            psTvSysGroup->ui4_tv_sys_mask = TV_SYS_MASK_NONE;
            if (u1TvSysGroupIdx < psAtvPdCtx->sAtvPiCtx.ucTvSysGroupNum)
            {
                psTvSysGroup->ui4_tv_sys_mask =
                        psAtvPdCtx->sAtvPiCtx.pu4TvSysGroup[u1TvSysGroupIdx];
            }
            break;
        }

        case TUNER_GET_TYPE_NEXT_STEP_SIZE:
        {
            TUNER_STEP_SIZE_T *psTunerStepSize;
            BOOL   fgTvdLock;
            UINT8  u1TvdState;

            psTunerStepSize = (TUNER_STEP_SIZE_T *)pvAnaAttribute;

            // AnalogPIEU_fgDrvTunerCheckTVDLock() will average noise level
            fgTvdLock = AnalogPIEU_fgDrvTunerCheckTVDLock(CHN_LOCK_CHECK_TIME);
            u1TvdState = bTvdNAState4Step();

            if (fgTvdLock) {
                // bTvdNAState4Step(): Get TVD noise level
                // 0: reserved
                // 1: Low noise
                // 2: High noise
                if (u1TvdState > 1) { // if TVD noise is high
                    psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_NOISE_HIGH;
                }
                else {
                    if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_PAL_BG)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_BG;
                    else if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_PAL_DK)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_DK;
                    else if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_PAL_I)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_I;
                    else if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_SECAM_L)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_SECAM_L;
                    else if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_SECAM_L1)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_SECAM_L1;
                    else if (psAtvPdCtx->sAtvPiCtx.u1SubSysIdx == SYS_SUB_NTSC_M)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_MN;
                    else
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_NOISE_HIGH;
                }
            }
            else {
                psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_UNLOCK;
            }

            if (!fgNewScanStep){
                if (fgTvdLock){
                    psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_OLD;
                }
                else {
                    psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_UNLOCK_OLD;
                }
            }
            mcSHOW_DBG_MSG(("TVD noise = %d, TV spec = %d, next step = %d\n", u1TvdState, psAtvPdCtx->sAtvPiCtx.u1SubSysIdx, psTunerStepSize->i4_step_size));

            break;
        }

        default:
        {
            mcSHOW_DBG_MSG4(("%s: RMR_DRV_INV_GET_INFO,e_get_type=%d!\n", __FUNCTION__,(UINT32)e_get_type));

            mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
            return (RMR_DRV_INV_GET_INFO);
        //    break;
        }
    }

    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
    return (RMR_OK);

}

/************************************************************************
*  AnalogEU_TunerSetAttribute
*  Set analog tuner driver attribute.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_set_type : Attribute type.
*  @param  *pvAnaAttribute : Input data content pointer.
*  @param  zAnaAttributeLen : Input data length.
*  @retval  RMR_OK : Set analog tuner driver attribute Successfully.
*  @retval  RMR_DRV_INV_GET_INFO : Set analog tuner driver attribute fail.
************************************************************************/
INT32 AnalogEU_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_SET_TYPE_T  e_set_type,
                           const VOID *pvAnaAttribute,
                           SIZE_T zAnaAttributeLen)
{
#if fcTUNE_NOT_START
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
#endif

//    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    switch (e_set_type)
    {


#if fcTUNE_NOT_START
        case TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS:
            if (zAnaAttributeLen)
            {
            TUNER_ACQ_ACTION_T *peActionType = (TUNER_ACQ_ACTION_T *) pvAnaAttribute;

                mcSHOW_DBG_MSG(("%s: (%d, %X) eActionType=%d!\n", __FUNCTION__,
                                TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS,
                                TUNER_SET_SPECIAL_TYPE_ACQ_ACTION_CLASS, *peActionType));
                psAtvPdCtx->u1TunerAcqType = *peActionType;
            }
            break;
#endif

        case TUNER_SET_TYPE_TUNER_LOCAL_DEFINE_RESUME_PI_MONITOR:
            psAtvPdCtx->Frequency = *( (UINT32 *) pvAnaAttribute)/1000;//fix CR [DTV00123197]
#ifdef ANAEU_ENABLE_MONITOR
            if(psAtvPdCtx->fgBreakMonitor){
                ResumeMonitor(psAtvPdCtx);
            }
#endif
        break;
        default:
        {
            mcSHOW_DRVERR_MSG(("%s: RMR_DRV_INV_GET_INFO!\n", __FUNCTION__));
//            mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
            return (RMR_DRV_INV_GET_INFO);
        //    break;
        }
    }
//    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);

#if fcTUNE_NOT_START
    return (RMR_OK);
#endif
}

/***********************************************************************/
#ifdef fcADD_DIAG_INFO

#define DRV_CUSTOM_TUNER_DIAG_ITEMS         3

/************************************************************************
*  AnalogEU_TunerSetDiagCmd
*  Set analog tuner diagnostic information.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_tuner_set_type : Diagnostic information type.
*  @param  *pv_set_info : Input data content pointer.
*  @param  z_size : Input data length.
*  @retval  RMR_OK : Set analog tuner diagnostic information successfully.
*  @retval  RMR_DRV_SET_FAILED : Set analog tuner diagnostic information fail.
*  @retval  RMR_DRV_INV_SET_INFO : Invalid analog tuner diagnostic information.
************************************************************************/
INT32 AnalogEU_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        DRV_CUSTOM_TUNER_SET_TYPE_T     e_tuner_set_type,
                        VOID*                           pv_set_info,
                        SIZE_T                          z_size)
{
	INT32	retSts = RMR_OK;
#ifdef TUNER_DVBT_ENABLE_AGC_CTRL /*fix build warning*/
    ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;


    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry
#endif /*TUNER_DVBT_ENABLE_AGC_CTRL*/

    switch(e_tuner_set_type)
    {
        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_OPEN:
            // Do initialization here
            break;

        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_CLOSE:
            // Do cleanup here
            break;
 #ifdef TUNER_DVBT_ENABLE_AGC_CTRL
        case DRV_CUSTOM_TUNER_SET_TYPE_AGC:
        {

            ITUNER_CTX_T * pDigiTunerCtx = ITunerGetCtx();
	      SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);
	      pEUCtx->m_Ana_Top = ((TUNER_TYPE_DIAG_AGC_T *) pv_set_info)->ucAgcValue;

            if ( !AnalogPIEU_TunerSetSystem(cATUNER_PI_CTX(psAtvPdCtx), AnalogPIEU_TunerGetSystem(cATUNER_PI_CTX(psAtvPdCtx))))
            {

                //PDWNC_WriteErrorCode(ERR_I2C_DEMOD_ANALOG);
                mcSHOW_USER_MSG(("TunerSetDiagCmd: set ATV TOP fail!!\n"));
            }
          mcSHOW_DBG_MSG(("TunerSetDiagCmd: set ATV TOP = 0x%02X\n", pEUCtx->m_Ana_Top));
        }
          break;
#endif /*TUNER_DVBT_ENABLE_AGC_CTRL*/
        default:
            retSts = RMR_DRV_INV_SET_INFO;
            break;
    }

#ifdef TUNER_DVBT_ENABLE_AGC_CTRL
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
#endif /*TUNER_DVBT_ENABLE_AGC_CTRL*/
    return retSts;
}



/************************************************************************
*  AnalogEU_TunerGetDiagInfo
*  Get analog tuner diagnostic information.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_tuner_get_type : Diagnostic information type.
*  @param  *pv_get_info : Output data content pointer.
*  @param  *pz_size : Output data length.
*  @retval  RMR_OK : Get analog tuner diagnostic information successfully.
*  @retval  RMR_DRV_NO_GET_INFO : Get analog tuner diagnostic information fail.
*  @retval  RMR_DRV_NOT_ENOUGH_SPACE : Get analog tuner diagnostic information fail.
************************************************************************/
INT32 AnalogEU_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                              DRV_CUSTOM_TUNER_GET_TYPE_T   e_tuner_get_type,
                              VOID                           *pv_get_info,
                              SIZE_T                        *pz_size)
{
ATV_PD_CTX_T *psAtvPdCtx = (ATV_PD_CTX_T *) ptTDSpecificCtx;
TUNER_TYPE_DIAG_CONTENT_T *pDiagContent = (TUNER_TYPE_DIAG_CONTENT_T *) pv_get_info;

    mcSEMA_LOCK_HAL(psAtvPdCtx->hHalLock);  // Avoid re-entry

    switch (e_tuner_get_type)
    {
    	  /*
        case DRV_CUSTOM_TUNER_GET_TYPE_AGC:
        {
	     ITUNER_CTX_T *  pDigiTunerCtx = ITunerGetCtx();
	      SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pDigiTunerCtx->specific_member.eu_ctx);

            if ((*pz_size) < sizeof(TUNER_TYPE_DIAG_AGC_T))
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            ((TUNER_TYPE_DIAG_AGC_T *) pv_get_info)->ucAgcValue = pEUCtx->m_Ana_Top;
            mcSHOW_DBG_MSG(("TunerGetDiagInfo: Get ATV TOP = 0x%02X\n", pEUCtx->m_Ana_Top));
        }
            break;
         */
        case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTROL:
            if ((*pz_size) < sizeof(TUNER_TYPE_DIAG_CONTROL_T))
            {
                *pz_size = sizeof(TUNER_TYPE_DIAG_CONTROL_T);
                mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            ((TUNER_TYPE_DIAG_CONTROL_T *) pv_get_info)->u2PollPeriodMs = 2000;
            ((TUNER_TYPE_DIAG_CONTROL_T *) pv_get_info)->ucCntOfDiagStruc = DRV_CUSTOM_TUNER_DIAG_ITEMS;
            break;

        case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTENT:
            if ((*pz_size) < DRV_CUSTOM_TUNER_DIAG_ITEMS * sizeof(TUNER_TYPE_DIAG_CONTENT_T))
            {
                *pz_size = DRV_CUSTOM_TUNER_DIAG_ITEMS * sizeof(TUNER_TYPE_DIAG_CONTENT_T);
                mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }

        // To add new items, please remember to modify DRV_CUSTOM_TUNER_DIAG_ITEMS
        // Note that both cDisplayName and cDisplayValue can NOT exceed 15 bytes

        // Record 1
            sprintf(pDiagContent->cDisplayName, "%s", "Version:");
            sprintf(pDiagContent->cDisplayValue, "%s", AnalogPIEU_TunerGetVer(cATUNER_PI_CTX(psAtvPdCtx)));
            pDiagContent++;

        // Record 2
            sprintf(pDiagContent->cDisplayName, "%s", "RF Freq (KHz):");
            sprintf(pDiagContent->cDisplayValue, "%6d", psAtvPdCtx->Frequency);
            pDiagContent++;

        // Record 3
            sprintf(pDiagContent->cDisplayName, "%s", "Lock State:");
            sprintf(pDiagContent->cDisplayValue, "%d", psAtvPdCtx->u1VIFLock);
            pDiagContent++;

            break;                          // End of case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTENT

        default:
            mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
            return RMR_DRV_NO_GET_INFO;
    }
    mcSEMA_UNLOCK_HAL(psAtvPdCtx->hHalLock);
    return RMR_OK;
}
#endif

/***********************************************************************/
/*                  Acquisition APIs                                   */
/***********************************************************************/
/***********************************************************************/
/*                  Public Functions (not static)                      */
/***********************************************************************
*  Tuner_AnalogEU_Register_LTDIS_Fct
*  Analog ltdis api mapping.
*  @param  ptTDCtx : A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
*  @retval  void.
***********************************************************************/
VOID Tuner_AnalogEU_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
  TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose = AnalogEU_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq = AnalogEU_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync = AnalogEU_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal = AnalogEU_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = AnalogEU_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = AnalogEU_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = AnalogEU_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = AnalogEU_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt = AnalogEU_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer = AnalogEU_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest = AnalogEU_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand = AnalogEU_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAtdCommand = AnalogEU_TunerAtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = AnalogEU_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = AnalogEU_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C = AnalogEU_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerRegRead = AnalogEU_TunerRegRead;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc = AnalogEU_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute = AnalogEU_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute = AnalogEU_TunerGetAttribute;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = AnalogEU_TunerGetDiagInfo;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = AnalogEU_TunerSetDiagCmd;
#endif
}

