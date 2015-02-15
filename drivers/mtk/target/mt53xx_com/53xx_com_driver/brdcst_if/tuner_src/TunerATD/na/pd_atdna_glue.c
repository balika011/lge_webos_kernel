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
 * $RCSfile: pd_anana_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
//#include "Pd_AnaApi.h"
#include "pd_atd_glue.h"
#include "pd_atdna_glue.h"
//#include "pd_atdeu_if.h"
#include "UtilAPI.h"
#include "TDMgr.h"

//#include "PI_Def_dbg.h"

#define PLUGGABLE_DEMOD_DESIGN //Pluto Kang 090320

#ifdef fcADD_DIAG_INFO
#ifdef CC_MT5381
#include "u_drv_cust.h"
#endif
#endif
#include "nptv_drvif.h"
#include "x_stl_lib.h"
#include "eepdef.h"

#ifdef TUNER_ANALOG_NEED_INIT_TUNER
#include "rf_tuner_api.h"
#include "dtd_if.h"  //here we need tuner context only
#endif
#ifdef PLUGGABLE_DEMOD_DESIGN
#include "tuner_interface_if.h"
#include "pd_tuner_type.h"
//#include "ctrl_bus.h"
#endif
#include "atd_feature.h"
#include "pi_demod_atd.h"
#include "pd_tuner_type.h"
#include "pi_demod_atdna.h"
#include "drvcust_if.h"

// ZY, 091222
#include "api_srch_if.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#if !defined(CC_MTAL) && !defined(CC_SDAL) //linux platform
//Pluto
#define ANAUS_ENABLE_MONITOR
#endif

#define fcADD_TVD_SYNC         // Pause TVD before TunerAcq complete, or will show "No Signal"
#define fcADD_AV_SYNC           1

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define cMONITOR_PERIOD_MS      500

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#ifdef ANAUS_ENABLE_MONITOR
static BOOL             fgMonitorThreadRunning;
static THREAD_T         t_monitor_thread;
static MUTEX_T			t_escape_mon_suspend;
#endif

static UINT8 fgAtdMonitorRet = FALSE;
#if defined(ATD_SNOWSCREEN_MODE)
static UINT8 fgAtdAFTFailCnt = 0;
#endif
//-----------------------------------------------------------------------------
// global variables
#ifdef DTD_INCLUDE
EXTERN UINT8 gfgIFTrap;
#endif
extern BOOL fg_Acq_flag;
BOOL fgEnableATP=FALSE;//Flag to check whether show  Driver ATP Auto Test log
//-----------------------------------------------------------------------------
ATD_CTX_T*    psAtdnaDemodCtx;

#if fcTIMING_MEASURE
UINT32  u2TickStart;//jackson: todo, move to a control block
static UINT16  u2Delay = 0;
#endif
EXTERN UINT8   _bSetChannelCnt;

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------
#ifdef TUNER_ANALOG_NEED_INIT_TUNER
EXTERN TUNER_API_FCT_TBL_T *pTuner_API_Fct;
EXTERN VOID Tuner_Register_Fct(TUNER_API_FCT_TBL_T *ptuner_Fct);
EXTERN VOID *GetUSTunerCtx(VOID);
RF_TUNER_CTX_T  *psTunerCtx;
#endif

#ifdef PLUGGABLE_DEMOD_DESIGN
static ITUNER_CTX_T *pTunerCtx;
#endif
//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
extern UINT8 bGetSignalType(UINT8 bPath);
#ifdef CC_SUPPORT_PIPELINE
extern UINT8 bGetSignalTypeAVD(UINT8 bPath);
#endif
extern UINT32 PDWNC_ReadServoADCChannelValue(UINT32 u4Channel);
extern void AUD_WaitDspFlush(UINT8 u1DecId);
extern int  snprintf(char *s, size_t n, const char * format, ...);
#if (fcTUNER_TYPE >= cTUNER_ALL_START)
#ifdef I2C_TUNER_ALL
extern UINT8 CheckTunerTypeViaI2c(VOID);
#endif
#endif
/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
#if defined(CC_DRIVER_DEMO) || defined(CC_LINUX_KERNEL)
#define _Tvd_NotifyTVSig(u1Path, u1SigState)
#else
VOID _Tvd_NotifyTVSig(UINT8 u1Path, UINT8 u1SigState);
#endif

/***********************************************************************/
/*              Private (static) Function Prototypes                   */
/***********************************************************************/
VOID Tuner_AtdNA_Register_LTDIS_Fct(PTD_CTX ptTDCtx);
VOID AtdNA_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv);
#ifndef CC_MT5880_CODE_SIZE_CTRL
BOOL AnaPi_TunerGetVIFLock(VOID)
{
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  AnaPd_EnterTunerAcq
 *  TvdHoldMode : On
 *  @param  void.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
VOID AnaPd_EnterTunerAcq(VOID)
{
#ifdef fcADD_TVD_SYNC //fix zero used for undefined preprocessing identifier
    vTvdHoldMode(SV_ON);
    mcSHOW_DBG_MSG(("vTvdHoldMode(SV_ON) when enter TunerAcq\n"));
#endif
}

//-----------------------------------------------------------------------------
/*
 *  AnaPd_ExitTunerAcq
 *  TvdHoldMode : Off
 *  @param  void.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
VOID AnaPd_ExitTunerAcq(BOOL fgLocked)
{
#ifdef fcADD_AV_NOTIFY //fix zero used for undefined preprocessing identifier
    AUD_AtunerSigStableNotify(fgLocked);
#endif
#ifdef fcADD_TVD_SYNC  //fix zero used for undefined preprocessing identifier
    vTvdHoldMode(SV_OFF);
    mcSHOW_DBG_MSG(("vTvdHoldMode(SV_OFF) when exit TunerAcq\n"));
#endif
}

//-----------------------------------------------------------------------------
/*
 *  AnaPd_ChangeChannel
 *  Do Channel Change
 *  @param  u4CurFreqency
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
#ifndef CC_MT5391_AUD_3_DECODER
VOID AnaPd_ChangeChannel(UINT32 u4CurFreqency)
{
#if fcADD_AV_SYNC
    STATIC UINT32 u4PreFreqency = 0;
    BOOL fgSameChannel;

    fgSameChannel = (u4CurFreqency == u4PreFreqency);
    u4PreFreqency = u4CurFreqency;

    if (!fgSameChannel)
    {
        AUD_WaitDspFlush(1);
    }
#endif
    return;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  vTunerBusyWait
 *  Do Channel Change
 *  @param  u2WaitTimeus 	waiting time
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
#ifdef ANAUS_ENABLE_MONITOR
static BOOL SetBreakMonitor(ATD_PD_CTX_T *psAtdPdCtx)
{
    UINT8 u1Cnt=0;
    psAtdPdCtx->fgBreakMonitor = TRUE; //set to break monitor

    if(psAtdPdCtx->fgPIMoniStatus) //PI really doing PI Monitor
    {  
        while(psAtdPdCtx->fgPIMoniStatus)
        {
            //wait PI Monitor really finish break
            if(u1Cnt>250)  //in case of deadlock
            {  
                break;
            }
            u1Cnt++;
            mcSHOW_DBG_MSG(("ANA US PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
            mcDELAY_MS(10);
        }
    }
    return TRUE;
}

static void ResumeMonitor(ATD_PD_CTX_T *psAtdPdCtx)
{
#if defined(ATD_SNOWSCREEN_MODE)
    fgAtdAFTFailCnt = 0;
    fgAtdMonitorRet = FALSE;
#endif
    psAtdPdCtx->fgBreakMonitor = FALSE;
    mcMUTEX_UNLOCK(t_escape_mon_suspend);
}
//-----------------------------------------------------------------------------
/*
 *  i4CheckNoticeBreak
 *  Break function in the monitor thread and searchnearby function
 *  @param  *pvArg	function argument.
 *  @retval   TRUE 	have a break notification.
 */
//-----------------------------------------------------------------------------
STATIC INT32 i4CheckNoticeBreak(VOID *pvArg)
{    
    ATD_PD_CTX_T *psPdCtx = (ATD_PD_CTX_T *) pvArg;
    if(psPdCtx->fgBreakMonitor)
    {
        mcPRN_MSG_MONI(" [%s-%s = 1]\n", __FILE__, __FUNCTION__);
    }
    return (psPdCtx->fgBreakMonitor);
}

//-----------------------------------------------------------------------------
/*
 *  MonitorProcess
 *  Main Monitor thread ==> ISP
 *  @param  *pvArg	function argument.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
STATIC void MonitorProcess(VOID *pvArg)
{
    ATD_PD_CTX_T *psPdCtx = *(ATD_PD_CTX_T **) pvArg;
#if defined(ATD_SNOWSCREEN_MODE)
    UINT8 ucData1, ucVpress;
#endif

    mcSHOW_DBG_MSG(("MonitorProcess start\n"));
    while (fgMonitorThreadRunning)
    {
    	if (psPdCtx->fgBreakMonitor)
    	{    		
            mcSHOW_DBG_MSG(("Entering 5391 mon_suspend\n"));
            mcMUTEX_LOCK(t_escape_mon_suspend);
 		    mcSHOW_DBG_MSG(("Escape 5391 mon_suspend\n"));
    	}
    	else
    	{
            mcSEMA_LOCK_HAL(psPdCtx->hHalLock);
		    //Add Monitor ISP here
            psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
#if defined(ATD_SNOWSCREEN_MODE)
            if(1 == fgAtdAFTFailCnt)
            {                                                
                ucVpress = (IO_READ8(VIDEO_IN0_BASE, 0x81) & 0x10) >> 4;
                if(0x01 == ucVpress)
                {
                    ucData1 = 0x08;
                    ATD_SetReg(psAtdnaDemodCtx, REG_ATD_POA_CFG, &ucData1, 1);
                    ucData1 = 0x04;                    
                    ATD_SetReg(psAtdnaDemodCtx, REG_ATD_PE_CFOK, &ucData1, 1);
                    ucData1 = 0x21;                    
                    ATD_SetReg(psAtdnaDemodCtx, REG_ATD_PS_CFG, &ucData1, 1); 
                    psAtdnaDemodCtx->u1PF = 0x01;
                    ATD_SetNormalMode(psAtdnaDemodCtx);                    
                    mcSHOW_DBG_MSG(("VPress lock, restart AFT!!!!!!!!!!!!!!!!!!!!\n"));
                    fgAtdAFTFailCnt = 0;	
                }
            }

            if(0 == fgAtdAFTFailCnt)
            {
                fgAtdMonitorRet = ATDNA_Monitor(psAtdnaDemodCtx, &psPdCtx->Frequency, i4CheckNoticeBreak, psPdCtx);          
            }

            if (fgAtdMonitorRet) 
            {
                ATD_Patch(psAtdnaDemodCtx);
                fgAtdAFTFailCnt = 0;
            }            
            else
            {
                if(0 == fgAtdAFTFailCnt)
                {                     
                    ucData1 = 0x00;
                    ATD_SetReg(psAtdnaDemodCtx, REG_ATD_POA_CFG, &ucData1, 1);
                    ucData1 = 0x09;
                    ATD_SetReg(psAtdnaDemodCtx, REG_ATD_ACR_CFG, &ucData1, 1);       
                    mcSHOW_DBG_MSG(("1st time monitor fail!\n"));                                                                                
                    fgAtdAFTFailCnt++;
                }
            } 
#else
            fgAtdMonitorRet =
                ATDNA_Monitor(psAtdnaDemodCtx, &psPdCtx->Frequency,
        		                    i4CheckNoticeBreak, psPdCtx);
            // do ATD_Patch every 100ms
            if (fgAtdMonitorRet) 
            {
                ATD_Patch(psAtdnaDemodCtx);
            }
#endif 
            psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
            mcSHOW_DBG_MSG4(("===ANA_NA Monitor Thread ===\n"));
            mcSEMA_UNLOCK_HAL(psPdCtx->hHalLock);

            mcDELAY_MS(cMONITOR_PERIOD_MS);
        }
    }
    t_monitor_thread = (THREAD_T)NULL;
    mcSHOW_DBG_MSG(("MonitorProcess exit!\n"));
    mcTHREAD_DESTROY();
}

//-----------------------------------------------------------------------------
/*
 *  PD_StartMonitorEngine
 *  To create and start the monitor thread
 *  @param  *psAtvPdCtx	Pd handler.
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
STATIC S32 PD_StartMonitorEngine(ATD_PD_CTX_T *psAtdPdCtx)
{
    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not
    // directly use pv_arg. Therefore, if we want thread to share the same
    // context as caller, we need set pv_arg as "pointer of context".
    fgMonitorThreadRunning = TRUE;
    if (mcTHREAD_CREATE(&t_monitor_thread,
            "NA_ANA_MonThread",
             cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
            MonitorProcess, sizeof(void*), (VOID*)&psAtdPdCtx) != OSR_OK)
    {
        mcSHOW_DBG_MSG(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSHOW_DRVAPI_MSG(("PD_StartMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}

//-----------------------------------------------------------------------------
/*
 *  PD_StopMonitorEngine
 *  To create and start the monitor thread
 *  @param  v0od.
 *  @retval   0 ==> Close successful.
 */
//-----------------------------------------------------------------------------
STATIC S32 PD_StopMonitorEngine(void)
{
    while (t_monitor_thread)
    {
        mcDELAY_MS(10); // in ms
    }
    mcSHOW_DRVAPI_MSG(("ANA NA PD_StopMonitorEngine success\n"));
    return (DRVAPI_TUNER_OK);
}
#endif //ANAUS_ENABLE_MONITOR

/***********************************************************************/
/*
 *  GetAnaConnMode
 *  Get Connection Mode
 *  @param      eMod:
			    MOD_PSK_8 --> Channel Change
			    MOD_UNKNOWN --> Scan
			    MOD_QAM_1024 --> Fine-tune (EU mode)
 *  @retval   connectmode
 */
 //-----------------------------------------------------------------------------
 /*
    u4AutoSearchType:
    0 --> Channel Change;
    1 --> Scan;     
    2 --> Fine-tune (EU mode);  
 */    
STATIC UINT8 GetAnaConnMode(UINT8 eMod)
{
    UINT8   u1ConnMode=0, ucData1;
    UINT32	u4AutoSearchType = 0;
    
    mcSHOW_DBG_MSG(("eMod = %d\n", eMod));   
    
    u4AutoSearchType = u4GetAutoSearchType();
    mcSHOW_DBG_MSG(("u4AutoSearchType = %d\n", u4AutoSearchType));   
    
    // ZY, 091222, u1ConnMode = u4AutoSearchType, except FM radio mode
    switch (u4AutoSearchType)
    {           	
        case ATV_CONN_MODE_CHAN_CHANGE:      
            u1ConnMode = ATV_CONN_MODE_CHAN_CHANGE;
            ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetLNA, 0, &ucData1); 
            mcSHOW_DBG_MSG(( "Connection_type = channel change!\n"));
            break;
                	
        case ATV_CONN_MODE_CHAN_SCAN:                       
            u1ConnMode = ATV_CONN_MODE_CHAN_SCAN;
            ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetLNA, 0, &ucData1);            
            mcSHOW_DBG_MSG(( "Connection_type = channel scan!\n"));
            break;
        
        case ATV_CONN_MODE_FINE_TUNE:                    
            u1ConnMode = ATV_CONN_MODE_FINE_TUNE;
            ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetLNA, 0, &ucData1);             
            mcSHOW_DBG_MSG(( "Connection_type = fine tune!\n"));
            break; 
        default:
            break;
    }                    

    switch (eMod)
    { 
#ifndef CC_MT5360
        case MOD_FM_RADIO:
            u1ConnMode = ATV_CONN_MODE_FM_RADIO;
            ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetLNA, 0, &ucData1);
            mcSHOW_DBG_MSG(( "Mod_type = FM radio!\n"));
            break;
#endif
        default:
            break;
    }

    return u1ConnMode;
}

//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------
UINT32 AtdNA_GetNoiseLevel(void)
{
    return ATDPI_GetNoiseLevel(psAtdnaDemodCtx);
}

//wenming 2012-09-13:Add for other modules get ATV TV system setting
UINT8 AtdNA_GetTVSystem(void)
{
	return psAtdnaDemodCtx->u1SubSysIdx;
}

UINT16 AtdNA_GetIFAGC(void)
{
    return ATDPI_GetIFAGC(psAtdnaDemodCtx);
}

BOOL AtdNA_SetLNA(BOOL fgLNA)
{
    return ATDPI_SetLNA(psAtdnaDemodCtx, fgLNA);
}

#ifndef CC_MT5880_CODE_SIZE_CTRL
void ATD_PowerDownNA(UINT8 fg)
{
    if(fg)
    {
    	ATD_DisConnect(psAtdnaDemodCtx);
    }
    else
    {
    	ATD_ChipInit(psAtdnaDemodCtx);
    }
    return;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerOpen
 *  Open analog tuner driver
 *  @param  ptTDCtx  a void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  ptTunerCfgInfo    tuner config information
 *  @param  pptTDSpecificCtx
 *  @retval   DRVAPI_TUNER_OK : Success.
 */
//-----------------------------------------------------------------------------
INT32 AtdNA_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo,
                     PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
    ATD_PD_CTX_T   *psAtdPdCtx;
    mcSHOW_USER_MSG(("ATD_Initialize successful\n"));

    Tuner_AtdNA_Register_LTDIS_Fct(ptTDCtx);

    /*create ATD PD CTX*/  
    psAtdPdCtx = (ATD_PD_CTX_T *) mcALLOC_MEM(sizeof(ATD_PD_CTX_T));
    if (NULL == psAtdPdCtx)
    {
        return (DRVAPI_TUNER_ERROR);
    }   
    psAtdPdCtx->fgDisStatus = FALSE;
    *pptTDSpecificCtx = psAtdPdCtx;

    psAtdnaDemodCtx = ATD_DemodCtxCreate();
    if (NULL == psAtdnaDemodCtx)
    {
        return (DRVAPI_TUNER_ERROR);
    }

    /*link demod ctx to pd ctx*/
    psAtdPdCtx->sAtdPiCtx = psAtdnaDemodCtx;
    
	#if (fcTUNER_TYPE >= cTUNER_ALL_START)
    #ifdef I2C_TUNER_ALL
	((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType=CheckTunerTypeViaI2c();
    #endif
	#endif
	
    //090720, LC, move ITuner init b4 ATD init
    ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);
    
    pTunerCtx = ITunerGetCtx();
    if (NULL == pTunerCtx)
    {
        return (DRVAPI_TUNER_ERROR);
    }
    ITuner_Init(pTunerCtx); //todo: modify tuneri2caddr

    psAtdnaDemodCtx->pTCtx = pTunerCtx;
    psAtdnaDemodCtx->u1TunerType =((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType;

    // Initialize ATD context & download FW (Pluto Kang don't load FW for MT5387)
    if (ATD_Initialize(psAtdnaDemodCtx, 0x92, FALSE))
    {
    	mcSHOW_USER_MSG(("ATD_Initialize failed\n"));
    	return (DRVAPI_TUNER_ERROR);
    }
    else
    {
    	mcSHOW_USER_MSG(("ATD_Initialize successful\n"));
    }      
	ATD_DisConnect(psAtdnaDemodCtx);//20110503_Haibo:Discconnect ATD after tuner open
	
#if fcADD_HAL_LOCK
    if (mcSEMA_CREATE(&psAtdPdCtx->hHalLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_HW_MSG(("%s: mcSEMA_CREATE(hHalLock) Fail!\n", __FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
#endif

#if fcTUNE_NOT_START
    psAtdPdCtx->u1TunerAcqType = ACQ_ACTION_DEFAULT;
#endif

#ifdef ANAUS_ENABLE_MONITOR
    if (mcSEMA_CREATE(&t_escape_mon_suspend, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DBG_MSG(("MT5391_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    psAtdPdCtx->fgBreakMonitor = TRUE;//init
    psAtdPdCtx->fgPIMoniStatus = FALSE; //init
    // Start Monitor Engine
    PD_StartMonitorEngine(psAtdPdCtx);
#endif
    mcSHOW_USER_MSG(("AtdNA_TunerOpen END\n"));

    return (DRVAPI_TUNER_OK);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerClose
 *  To close analog tuner driver
 *  @param  ptTDCtx  a void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   DRVAPI_TUNER_OK : Success.
 */
//-----------------------------------------------------------------------------
INT32 AtdNA_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
#if fcENABLE_MONITOR || fcADD_HAL_LOCK
    ATD_PD_CTX_T *psAtdPdCtx;
#endif
#ifdef CC_SUPPORT_STR
    ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetTunerInit, (UINT8)FALSE, NULL);
#endif

#if fcENABLE_MONITOR || fcADD_HAL_LOCK
    psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif

#ifdef ANAUS_ENABLE_MONITOR
    psAtdPdCtx->fgBreakMonitor = TRUE;
    fgMonitorThreadRunning = FALSE; //need set to false before mcMUTEX_UNLOCK(t_escape_mon_suspend);
    mcMUTEX_UNLOCK(t_escape_mon_suspend);
    PD_StopMonitorEngine();
    mcMUTEX_DESTROY(t_escape_mon_suspend);
#endif

#if fcADD_HAL_LOCK
    if (mcSEMA_DESTROY(psAtdPdCtx->hHalLock) != OSR_OK)
    {
        mcSHOW_HW_MSG(("%s: mcSEMA_DESTROY(hHalLock) Fail!\n", __FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
#endif
    ATD_DemodCtxDestroy(psAtdnaDemodCtx);
    mcFREE_MEM(ptTDSpecificCtx);
    return (DRVAPI_TUNER_OK);
}

BOOL AtdNA_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    VOID* pv_conn_info,
                    SIZE_T z_conn_info_len,
                    x_break_fct _BreakFct,
                    VOID* pvArg)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    INT32   i4CurFreq = cTUNER_DEFAULT_FREQ;
    UINT32  u4Freq; 
    UINT8   u1Path = SV_VP_MAX;
    UINT8   u1ATVStatus = 0;
    UINT8   ucData1; 
    UINT8   u1ConnMode;
    BOOL    fgLockSts = TRUE;
    INT32   s4FreqOffset;
    INT16   s2FreqOffset;
    STATIC  UINT8 ScanedNo=0;//counter for scaned channel number
    mcSHOW_DBG_MSG(("START AtdNA_TunerAcq\n"));
#ifdef ANAUS_ENABLE_MONITOR
    SetBreakMonitor(psAtdPdCtx);
#endif
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    _bSetChannelCnt = 0x40;
    DigTunerBypassI2C (TRUE);
    fg_Acq_flag = TRUE;
    if ((e_conn_type != TUNER_CONN_TYPE_CAB_ANA) &&
        (e_conn_type != TUNER_CONN_TYPE_TER_ANA))
    {
        mcSHOW_HW_MSG(("%s: Wrong e_conn_type (%d)!\n", __FUNCTION__, e_conn_type));
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);       
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }

    if ((TUNER_CONN_TYPE_CAB_ANA == e_conn_type))
    {
#if defined(CC_ATDNA_CENTRAL_FREQ)
        u4Freq = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_central_freq; //new method 
#else
        u4Freq = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq; //original method
#endif        
        psAtdnaDemodCtx->fgAFT = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune;
        s2FreqOffset = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->i2_fine_tune_offset;
        //ZY, 091222. use u4AutoSearchType, e_mod only for FM_radio ***//
        u1ConnMode = GetAnaConnMode(((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->e_mod);    	
        // Brazil,  switch tuner to input signal in cable mode
        ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetInCableMode, TRUE, NULL); 
    }
    else if ((TUNER_CONN_TYPE_TER_ANA == e_conn_type))
    {
#if defined(CC_ATDNA_CENTRAL_FREQ)
        u4Freq = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_central_freq;//new method 
#else
        u4Freq = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq;//original method
#endif
        psAtdnaDemodCtx->fgAFT = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune;
        s2FreqOffset = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->i2_fine_tune_offset;
        //ZY, 091222. use u4AutoSearchType, e_mod only for FM_radio ***//
        u1ConnMode = GetAnaConnMode(((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->e_mod);

        // Brazil,	switch tuner to input signal in air mode
        ITuner_OP(psAtdnaDemodCtx->pTCtx, itSetInCableMode, FALSE, NULL); 
    }
    else
    {
        mcSHOW_HW_MSG(("%s: Wrong e_conn_type (%d)!\n", __FUNCTION__, e_conn_type));
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }
#ifndef CC_MT5391_AUD_3_DECODER
    AnaPd_ChangeChannel(u4Freq);
#endif

    AnaPd_EnterTunerAcq();  
    mcSHOW_DBG_MSG(("[DriverSearchChannelStart-ATuner] %s (Freq=%u)\n", __FUNCTION__, u4Freq));
#if fcTIMING_MEASURE
    u2TickStart = mcGET_SYS_TICK();
#endif

    u4Freq = u4Freq / 1000;

    if (ATV_CONN_MODE_FM_RADIO == u1ConnMode)
    {
        //EC_code_d100825: For MT5365/95 FM Radio flow modification: 
        psAtdPdCtx->u1SubSysIdx = MOD_PAL_BG;
        psAtdnaDemodCtx->u1SubSysIdx = MOD_PAL_BG;
        ATD_SetFreq(psAtdnaDemodCtx, u4Freq, _fgAutoSearch);
        mcSHOW_DBG_MSG(("Set ATDNA freq. of FM Radio = %dKHz\n",u4Freq));
        ATD_ChipInit(psAtdnaDemodCtx);
        ATD_SetSystem(psAtdnaDemodCtx, psAtdPdCtx->u1SubSysIdx);        
        mcSHOW_DBG_MSG(("Set ATDNA system of FM Radio = %d\n",psAtdPdCtx->u1SubSysIdx));  
        SetFMRadio(psAtdnaDemodCtx, u4Freq);
        ATD_StartAcq(psAtdnaDemodCtx, FALSE);
        AnaPd_ExitTunerAcq(TRUE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        psAtdPdCtx->u1VIFLock = 2;      
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return TRUE;
    }
    if ((u1ConnMode != ATV_CONN_MODE_CHAN_SCAN)&&(FALSE == psAtdnaDemodCtx->fgAFT)  && (s2FreqOffset > -80) && (s2FreqOffset < 80))
    {        
        ucData1 = 0xF9;
        ATD_SetReg(psAtdnaDemodCtx, REG_ATD_ACR_CFG, &ucData1, 1); //EC_code_d9A28: Enable CR auto-reacq for Fine tune mode
#if defined(ATD_FINETUNE_STEPSIZE)  
        s4FreqOffset = s2FreqOffset * ATD_FINETUNE_STEPSIZE; //EC_code_dA111: For SONY project, ATD_FINETUNE_STEPSIZE should be define as "31250" equal to 31.25KHz for each step
#else
        s4FreqOffset = s2FreqOffset * 0;
#endif
        mcSHOW_DBG_MSG(("Original freq = %dKHz, Offset freq = %dKHz\n",u4Freq, s4FreqOffset/1000));
        u4Freq = u4Freq + s4FreqOffset/1000;
        mcSHOW_DBG_MSG(("New freq = %dKHz\n",u4Freq));
    }
    psAtdPdCtx->u4AcqFrequency = u4Freq;     
    psAtdPdCtx->u1SubSysIdx = MOD_NTSC_M;    
    psAtdnaDemodCtx->u1SubSysIdx = MOD_NTSC_M; //EC_code_d100830: Add for Si-tuner application 

    if (ATD_SetFreq(psAtdnaDemodCtx, u4Freq, _fgAutoSearch) != TRUE)
    {
        mcSHOW_HW_MSG(("Freq %dKHz setting fail!\n", u4Freq));
        AnaPd_ExitTunerAcq(FALSE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);       
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }

    if (u1ConnMode != ATV_CONN_MODE_FINE_TUNE)
    {
        ATD_ChipInit(psAtdnaDemodCtx);
        ATD_SetSystem(psAtdnaDemodCtx, psAtdPdCtx->u1SubSysIdx);
    }
    if(ATV_CONN_MODE_CHAN_SCAN == u1ConnMode)//d110921_Haibo:Move here to fix channel scan time expand issue [DTV00357794]
    {
        ATD_AVSStrongAfterWeakM(psAtdnaDemodCtx);
    }
#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG4((" (1.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart) * mcGET_TICK_PERIOD()));
#endif
#if fcTUNE_NOT_START
    if (TUNE_FREQ_ONLY == psAtdPdCtx->u1TunerAcqType)
    {
        ATD_StartAcq(psAtdnaDemodCtx, TRUE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        mcSHOW_HW_MSG(("Running ATD StartAcq\n"));       
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return TRUE;
    }
#endif
    #ifndef DTD_INCLUDE
	if(u1ConnMode != ATV_CONN_MODE_CHAN_CHANGE) //d20121031_wenming:do not check TV source when channel change if not NA model
	#endif  
    {
        INT32 i4_i = 0;
        for (i4_i = 0; i4_i < 30; i4_i++)
        {
#ifdef CC_SUPPORT_PIPELINE
            if (bGetSignalTypeAVD(SV_VP_PIP) == (UINT8) SV_ST_TV)
#else
            if (bGetSignalType(SV_VP_PIP) == (UINT8) SV_ST_TV)
#endif

            {
                u1Path = SV_VP_PIP;
                mcSHOW_DBG_MSG(("[ATuner] Now is PIP path\n"));
                break;
            }
#ifdef CC_SUPPORT_PIPELINE
            else if (bGetSignalTypeAVD(SV_VP_MAIN) == (UINT8) SV_ST_TV)
#else
            else if (bGetSignalType(SV_VP_MAIN) == (UINT8) SV_ST_TV)
#endif

            {
                u1Path = SV_VP_MAIN;
                mcSHOW_DBG_MSG(("[ATuner] Now is MAIN path\n"));
                break;
            }
            else
            {
                if (_BreakFct(pvArg))
                {
                    mcSHOW_DBG_MSG(("[ATuner] BreakFct\n"));
                    break;
                }
                mcDELAY_MS(50);
                continue;
            }
        }
        if ((i4_i >= 30) || (SV_VP_MAX == u1Path))
        {
            mcSHOW_HW_MSG(("[ATuner] ERROR! without TV-decoder!\n"));
            AnaPd_ExitTunerAcq(FALSE);
            mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);            
            psAtdPdCtx->fgDisStatus = FALSE;
            fg_Acq_flag = FALSE;
            DigTunerBypassI2C (FALSE);
            return FALSE;
        }
    }
    i4CurFreq = u4Freq;

    if(ATV_CONN_MODE_CHAN_SCAN == u1ConnMode)
    {
        UINT32 u2FFF = u4Freq;
        psAtdnaDemodCtx->u1PF = 0x01;
        ATD_StartAcq(psAtdnaDemodCtx, TRUE);
        psAtdnaDemodCtx->fgCheckCR = TRUE;
        u1ATVStatus = ATDNA_SearchNearbyFreq(psAtdnaDemodCtx, &u2FFF, _BreakFct, pvArg, u2FFF);
        psAtdPdCtx->u1VIFLock = u1ATVStatus;
        ATD_SetSystemAudio(psAtdnaDemodCtx,MOD_NTSC_M);
        if (mcTEST_BIT(u1ATVStatus, cpANA_LOCK_STS_TVD))
        {
        	  if(2 == fgEnableATP)ScanedNo++;//for Driver ATP auto test(scaned channel number)
            fgLockSts = TRUE;
        }
        else
        {
            fgLockSts = FALSE;
        }
        if (fgLockSts)
        {
           mcSHOW_DBG_MSG(("[ATuner] Notify signal stable now\n"));
            _Tvd_NotifyTVSig(u1Path, SV_VDO_STABLE);
        }
        else
        {
            mcSHOW_DBG_MSG(("[ATuner] Notify no signal now \n"));
            _Tvd_NotifyTVSig(u1Path, SV_VDO_NOSIGNAL);
        }
        i4CurFreq = u2FFF;
    }
    else if(ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode)
    {
        psAtdnaDemodCtx->u1PF = 0x01;
        ATD_StartAcq(psAtdnaDemodCtx, TRUE);
        if(TRUE == psAtdnaDemodCtx->fgAFT)   //do searchnearby when AFT is enabled
        {
            psAtdnaDemodCtx->fgCheckCR = TRUE;
            u1ATVStatus = ATDNA_SearchNearbyFreq(psAtdnaDemodCtx, &u4Freq, _BreakFct, pvArg, u4Freq);
            psAtdPdCtx->u1VIFLock = u1ATVStatus;
            if (mcTEST_BIT(u1ATVStatus, cpANA_LOCK_STS_TVD))
            {
                i4CurFreq = u4Freq;
            }
            else
            {
                ATD_SetFreq(psAtdnaDemodCtx, i4CurFreq, _fgAutoSearch);
            }
        }
        else  //no searchnearby when AFT is disabled
        {
            psAtdPdCtx->u1VIFLock = 3;
        }

        if(2 == fgEnableATP) //show scaned channel when first time channel change after channel scan
        {  
        mcSHOW_DBG_MSG(("##Start\n##Scan : %u\n##End\n",ScanedNo));
        ScanedNo=0;
        }
    }
    else  //fine_tune mode
    {            
        psAtdPdCtx->u1VIFLock = 3;
    }

    mcSHOW_DBG_MSG(("DriverSearchChannelEnd-Status: %d (%dKHz)\
    (3-VIF and TVD both lock,2-TVD lock,1-VIF lock)\n",
                  psAtdPdCtx->u1VIFLock,
                  i4CurFreq));
#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG4((" (2.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart) * mcGET_TICK_PERIOD()));
#endif
    psAtdPdCtx->Frequency = i4CurFreq;
#if 0	
    if(mcCHECK_DBG_LVL(4))
    {    
        mcDELAY_MS(100);
        mcSHOW_USER_MSG(("Test Mod = %d\n", (IO_READ8(0x20022080, 0x03)>>4)&0x07));
    }
#endif	
#ifdef CC_DISABLE_AFT
    psAtdnaDemodCtx->fgAFT=FALSE;
    mcSHOW_DBG_MSG(("Disable AFT\n"));
#endif

#ifdef ANAUS_ENABLE_MONITOR
    if (!_BreakFct(pvArg) && (ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode))
    {
        ResumeMonitor(psAtdPdCtx);   // Enable monitor thread and Disable break function flag,  Unlock monitor thread
        mcSHOW_DBG_MSG(("Enable monitor process!!\n"));
    }
#endif
    if (psAtdPdCtx->u1VIFLock < 2)
    {
        AnaPd_ExitTunerAcq(FALSE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);        
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }
    AnaPd_ExitTunerAcq(TRUE);
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    mcSHOW_USER_MSG(("END AtdNA_TunerAcq\n"));
    psAtdPdCtx->fgDisStatus = FALSE;
    fg_Acq_flag = FALSE;
    DigTunerBypassI2C (FALSE);
    return TRUE;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetSync
 *  Not Used now
 *  @param  ptTDCtx  a void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type connection type (Air / Cable)
 *  @retval   none.
 */
//-----------------------------------------------------------------------------
INT16 AtdNA_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                          DRV_CONN_TYPE_T e_conn_type)
{
    INT16   i2RetValue = 1;                     // Always in sync state
    return i2RetValue;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetSignal
 *  To get current signal characteristic
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @param  *_pSignal Singal parameters
 *  @retval   void.
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    SIGNAL *_pSignal)
{
    INT32 i4CurFreq;
    UINT32  u4Freq;
    i4CurFreq=psAtdnaDemodCtx->pTCtx->u4RF_Freq;
    u4Freq = i4CurFreq * 1000;  //Return frequency in Hz
    mcSHOW_DBG_MSG(("[ATuner] %s:  %dKHz (%dHz)", __FUNCTION__, i4CurFreq,u4Freq));
    if ((TUNER_CONN_TYPE_CAB_ANA == e_conn_type))
    {
        TUNER_CAB_ANA_TUNE_INFO_T * pt_cab_ana = (TUNER_CAB_ANA_TUNE_INFO_T *)_pSignal;
        pt_cab_ana->ui4_freq = u4Freq;
        pt_cab_ana->e_mod = MOD_PSK_8;
        mcSHOW_DBG_MSG((" (%d)\n", pt_cab_ana->ui4_freq));
    }
    else if ((TUNER_CONN_TYPE_TER_ANA == e_conn_type))
    {
        TUNER_TER_ANA_TUNE_INFO_T * pt_ter_ana = (TUNER_TER_ANA_TUNE_INFO_T *)_pSignal;
        pt_ter_ana->ui4_freq = u4Freq;
        pt_ter_ana->e_mod = MOD_PSK_8;
        mcSHOW_DBG_MSG((" (%d)\n", pt_ter_ana->ui4_freq));
    }
    return;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetSignalLevel
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   Signal quality, Range is 0~100.
 */
//-----------------------------------------------------------------------------
UINT8  AtdNA_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx,
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
 *  AtdNA_TunerGetSignalLeveldBm
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   Signal quality (in dB).
 */
//-----------------------------------------------------------------------------
// Signal Level
INT16 AtdNA_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    UINT8 u1Value;
    u1Value = bTvdSignalStrengthLevel(1); // bMode = 1, return in dB
    return (u1Value);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetSignalPER
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   None.
 */
//-----------------------------------------------------------------------------
// packet error rate
UINT16  AtdNA_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetSignalSNR
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   None.
 */
//-----------------------------------------------------------------------------
UINT16 AtdNA_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);                             // return 0 first
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerSetMpgFmt
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type Connection type (Air / Cable)
 *  @retval   None.
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                            MPEG_FMT_T *pt_mpeg_fmt)
{
    return;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetVer
 *  To get tuner version
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   "Analog_Tuner".
 */
//-----------------------------------------------------------------------------
CHAR *AtdNA_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{ 
    char *pVer;
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
	
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
	
	ITuner_OP(psAtdnaDemodCtx->pTCtx,itGetVer,0,&pVer);
	return ((char*)pVer);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerNimTest
 *  No Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
INT32 AtdNA_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
    return 0;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerSetRegSetting
 *  No Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet,
                            UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
    return;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerShowRegSetting
 *  No Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
    return;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerTestI2C
 *  No Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
    return;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerRegRead
 *  No Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
INT32 AtdNA_TunerRegRead(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type,
                    UCHAR ucRegAddr, UCHAR *pucBuffer, U16 ucByteCount)
{
    return 0;
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerDisc
 *  Tuner driver disconection
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  e_conn_type connection type (Air / Cable)
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID  AtdNA_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                DRV_CONN_TYPE_T e_conn_type)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    if (TRUE == psAtdPdCtx->fgDisStatus)
    {
        /* Has Disconnected, return */
        mcSHOW_DBG_MSG(("ATD has Disconnect!\n"));
        return; 
    }
	#ifdef ANAUS_ENABLE_MONITOR
    SetBreakMonitor(psAtdPdCtx);
    #endif
    mcSHOW_DBG_MSG4(("ATD_TunerDisc pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(psAtdPdCtx->hHalLock);
    
    mcSHOW_DBG_MSG(("[Atuner] %s()\n", __FUNCTION__));

    ATD_DisConnect(psAtdPdCtx->sAtdPiCtx);    
    mcMUTEX_UNLOCK(psAtdPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("ATD_TunerDisc unlock pPdCtx->hHalLock\n"));
    
    /* set disconnect status */
    psAtdPdCtx->fgDisStatus = TRUE;
    return;    
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerCommand
 *  NA analog Tuner command
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  i4Argc Number of argument
 *  @param  aszArgve argument
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                      INT32 i4Argc, const CHAR** aszArgv)
{
#if fcADD_HAL_LOCK
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif
    UCHAR   ucCmdId = 0;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    if (i4Argc < 1)
    {
	    mcSHOW_USER_MSG(("eq:read out current EQ coefficient \n"));//d111202_Haibo:Add command to read current eq
#if fcTIMING_MEASURE
        mcSHOW_USER_MSG(("\t d u2Delay\n"));
#endif
    }

    if (i4Argc > 0)
    {
        ucCmdId = *((CHAR *) aszArgv[0]);

        switch (ucCmdId)
        {           
            case 'e':
        		if('q'==((CHAR *) aszArgv[0])[1])
        		{
        		   ATD_GetEQ(psAtdnaDemodCtx);
        		}
        		break;
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
                break;
         
            default:
                break;
        }
    }
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerAtdCommand
 *  Some NA analog ATD Tuner command (ex : set frequency...)
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @param  i4Argc Number of argument
 *  @param  aszArgve argument
 *  @retval   none
 */
//-----------------------------------------------------------------------------
VOID AtdNA_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv)
{
	ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#ifdef DTD_INCLUDE
	UINT8 ii;
#endif	
	INT32 i4AtdNAArgc;
	const CHAR** aszAtdNAArgv;
	UCHAR	ucAtdCmdId = 0;
	BOOL fgMonitorEnable = 0;

	i4AtdNAArgc = i4Argc;
	aszAtdNAArgv = aszArgv; 

#ifdef ANAUS_ENABLE_MONITOR
	fgMonitorEnable = 1;
#endif

	mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);	// Avoid re-entry

	if (i4AtdNAArgc < 1)
	{
		mcSHOW_USER_MSG(("Cmd_PD: \n"));		
#ifdef DTD_INCLUDE
		if (cALPS_TDAU4D01A == psAtdnaDemodCtx->u1TunerType)
		{
			mcSHOW_USER_MSG(("\t u: Analogue tuner adjacent trap control. 1--ON,0--OFF\n"));
		}
#endif			
		ATD_TunerAtdCommandList(i4AtdNAArgc, fgMonitorEnable);
	}	

	if (i4AtdNAArgc > 0)
	{
		ucAtdCmdId = *((CHAR *) aszAtdNAArgv[0]);

		switch (ucAtdCmdId)
		{		 
#ifdef DTD_INCLUDE
			case 'u':
				if (cALPS_TDAU4D01A == psAtdnaDemodCtx->u1TunerType)
				{
					if (i4AtdNAArgc > 1)
					{
						ii = StrToInt(aszAtdNAArgv[1]);
						if(1 == ii)
						{
							gfgIFTrap=0x03;//IF Trap ON
						}
						else
						{
							gfgIFTrap=0x02;//IF Trap OFF
						}
					}
				}
				else
				{
					mcSHOW_USER_MSG(("Not support!\n"));
				}
			 break;
#endif
		   
			
			case 'm':
			{
				if (1 == fgMonitorEnable)	
				{		
					UINT8	ucCmdExt;

					ucCmdExt = ((CHAR *) aszAtdNAArgv[0])[1];

					if ('p' == ucCmdExt)
					{
						SetBreakMonitor(psAtdPdCtx);
					}
					else if ('r' == ucCmdExt)
					{
						ResumeMonitor(psAtdPdCtx);
					}
				}				 
			}
				break;
				
			default:
				ATD_TunerAtdCommand(psAtdnaDemodCtx, i4AtdNAArgc, aszAtdNAArgv, ATDNA_SearchNearbyFreq);
				break;
		}
	}
	mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerGetAttribute
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
INT16 AtdNA_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_GET_TYPE_T  e_get_type,
                           VOID *pvAnaAttribute,
                           SIZE_T *pzAnaAttributeLen)
{
#if fcADD_HAL_LOCK
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif
    UINT16 u2Value;

    switch (e_get_type)
    {
        case TUNER_GET_TYPE_RFAGC:
        {
			mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
            u2Value = ATD_GetRFAGC(psAtdnaDemodCtx);
            *(UINT16*)pvAnaAttribute = u2Value;
            *pzAnaAttributeLen = sizeof(UINT16);
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            break;
        }
		case TUNER_GET_TYPE_NL:
        {
            *(UINT32*)pvAnaAttribute = AtdNA_GetNoiseLevel();
            *pzAnaAttributeLen = sizeof(UINT32);
        }
		break;
	case TUNER_GET_TYPE_KHZ_CFO:
	{
		 INT32 i4Data = 0;
		//ATD_CTX_T *psDemodCtx =NULL;
        // ATD_CTX_T*    psAtdnaDemodCtx=NULL;
		 i4Data = ATD_GetCFOKHz(psAtdnaDemodCtx, FALSE);
		*(INT32*)pvAnaAttribute = i4Data;
		*pzAnaAttributeLen = sizeof(INT32);
		 mcSHOW_USER_MSG(("CFO = %d KHz\n", i4Data));
	}
	break;
	case TUNER_GET_TYPE_TUNER_CHIP_REVISION:
		{
			UINT8 u1RFVer;
    		DigTunerBypassI2C (TRUE);
			ITuner_OP(ITunerGetCtx(),itGetChipRev,0,&u1RFVer);
    		DigTunerBypassI2C (FALSE);
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_TUNER_CHIP_REVISION\r\n"));
            mcSHOW_USER_MSG(("TUNER_CHIP_REVISION = 0x%x\n", u1RFVer));
			*pzAnaAttributeLen = sizeof(UINT8);
			x_memcpy(pvAnaAttribute, &u1RFVer, *pzAnaAttributeLen);
		}
	break;
	
	case TUNER_GET_TYPE_DEMOD_CHIP_REVISION:
		{
            UINT8 demodRev= 0x00;
            
            #if defined (CC_MT5880)|| defined(CC_MT5890)
               ATD_GetReg(psAtdnaDemodCtx, REG_ATD_CTL_CID, &demodRev, 1);
            #endif
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_DEMOD_CHIP_REVISION\r\n"));
            mcSHOW_USER_MSG(("DEMOD_CHIP_REVISION = 0x%x\n", demodRev));
			*pzAnaAttributeLen = sizeof(UINT8);
			x_memcpy(pvAnaAttribute, &demodRev, *pzAnaAttributeLen);
		}
	break;

	case TUNER_GET_TYPE_SSI:
		{
			UINT8 ssi=0;
			ssi = bTvdSignalStrengthLevel(1); // bMode = 1, return in dB
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_SSI\r\n"));
			*pzAnaAttributeLen = sizeof(UINT8);
			x_memcpy(pvAnaAttribute, &ssi, *pzAnaAttributeLen);
		}
	break;

	case TUNER_GET_TYPE_AGC:
		{
			UINT8 u1Value;			
			u1Value =(UINT8)AtdNA_GetIFAGC();
			*(UINT8*)pvAnaAttribute = u1Value;
			*pzAnaAttributeLen = sizeof(UINT8);
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_AGC\r\n"));
		}
	break;
	
	case TUNER_GET_TYPE_IFAGC:
		{
			UINT8 u1Value;			
			u1Value =(UINT8)AtdNA_GetIFAGC();
			*(UINT8*)pvAnaAttribute = u1Value;
			*pzAnaAttributeLen = sizeof(UINT8);
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_IFAGC\r\n"));
		}
	break;
	case TUNER_GET_TYPE_TUNER_STATUS:
		{
			BOOL u1RFStatus;
			DigTunerBypassI2C (TRUE);
			ITuner_OP(ITunerGetCtx(),itGetStatus,0,&u1RFStatus);			
  		          DigTunerBypassI2C (FALSE);
			mcSHOW_DBG_MSG(("HELLO,e_get_type =TUNER_GET_TYPE_TUNER_STATUS\r\n"));
			*pzAnaAttributeLen = sizeof(BOOL);
			x_memcpy(pvAnaAttribute, &u1RFStatus, *pzAnaAttributeLen);
		}
	break;
        default:
        {
            mcSHOW_DBG_MSG4(("%s: RMR_DRV_INV_GET_INFO!\n", __FUNCTION__));
            return (RMR_DRV_INV_GET_INFO);
    }
    }
    return (RMR_OK);
}

//-----------------------------------------------------------------------------
/*
 *  AtdNA_TunerSetAttribute
 *  Not Used now
 *  @param  ptTDCtx  A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
 *  @retval   none
 */
//-----------------------------------------------------------------------------
INT32 AtdNA_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_SET_TYPE_T  e_set_type,
                           const VOID *pvAnaAttribute,
                           SIZE_T zAnaAttributeLen)
{
#if fcTUNE_NOT_START
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif
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
                psAtdPdCtx->u1TunerAcqType = *peActionType;
            }
            break;
#endif
        default:
        {
            mcSHOW_DBG_MSG4(("%s: RMR_DRV_INV_GET_INFO!\n", __FUNCTION__));
            return (RMR_DRV_INV_GET_INFO);
        }
    }
#if fcTUNE_NOT_START
    return (RMR_OK);
#endif
}

/***********************************************************************/
#ifdef fcADD_DIAG_INFO
#define DRV_CUSTOM_TUNER_DIAG_ITEMS         3
INT32 AtdNA_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                              DRV_CUSTOM_TUNER_GET_TYPE_T   e_tuner_get_type,
                              VOID                           *pv_get_info,
                              SIZE_T                        *pz_size)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    TUNER_TYPE_DIAG_CONTENT_T *pDiagContent = (TUNER_TYPE_DIAG_CONTENT_T *) pv_get_info;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    switch (e_tuner_get_type)
    {
        case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTROL:
            if ((*pz_size) < sizeof(TUNER_TYPE_DIAG_CONTROL_T))
            {
                *pz_size = sizeof(TUNER_TYPE_DIAG_CONTROL_T);
                mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            ((TUNER_TYPE_DIAG_CONTROL_T *) pv_get_info)->u2PollPeriodMs = 2000;
            ((TUNER_TYPE_DIAG_CONTROL_T *) pv_get_info)->ucCntOfDiagStruc = DRV_CUSTOM_TUNER_DIAG_ITEMS;
            break;

        case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTENT:
            if ((*pz_size) < DRV_CUSTOM_TUNER_DIAG_ITEMS * sizeof(TUNER_TYPE_DIAG_CONTENT_T))
            {
                *pz_size = DRV_CUSTOM_TUNER_DIAG_ITEMS * sizeof(TUNER_TYPE_DIAG_CONTENT_T);
                mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }

        // To add new items, please remember to modify DRV_CUSTOM_TUNER_DIAG_ITEMS
        // Note that both cDisplayName and cDisplayValue can NOT exceed 15 bytes

        // Record 1
            snprintf(pDiagContent->cDisplayName,15,"%s", "Version:");
            snprintf(pDiagContent->cDisplayValue,15, "%s", "MTK_ATD");
            pDiagContent++;

        // Record 2
            snprintf(pDiagContent->cDisplayName,15, "%s", "RF Freq (KHz):");
            snprintf(pDiagContent->cDisplayValue,15, "%6d", psAtdnaDemodCtx->pTCtx->u4RF_Freq);
            pDiagContent++;

        // Record 3
            snprintf(pDiagContent->cDisplayName,15, "%s", "Lock State:");
            snprintf(pDiagContent->cDisplayValue,15, "%d", psAtdPdCtx->u1VIFLock);
            pDiagContent++;

            break;                          // End of case DRV_CUSTOM_TUNER_GET_TYPE_DIAG_CONTENT

        default:
            mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            return RMR_DRV_NO_GET_INFO;
    }
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    return RMR_OK;
}
#endif

/***********************************************************************/
/*                  Acquisition APIs                                   */
/***********************************************************************/
/***********************************************************************/
/*                  Public Functions (not static)                      */
/***********************************************************************/
VOID Tuner_AtdNA_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = ptTDCtx;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose = AtdNA_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq = AtdNA_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync = AtdNA_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal = AtdNA_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = AtdNA_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = AtdNA_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = AtdNA_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = AtdNA_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt = AtdNA_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer = AtdNA_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest = AtdNA_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand = AtdNA_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAtdCommand = AtdNA_TunerAtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = AtdNA_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = AtdNA_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C = AtdNA_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerRegRead = AtdNA_TunerRegRead;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc = AtdNA_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute = AtdNA_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute = AtdNA_TunerGetAttribute;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = AtdNA_TunerGetDiagInfo;
#endif
    return;
}

