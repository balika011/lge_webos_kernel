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
 * $RCSfile: pd_atdeu_glue.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file pd_atdeu_glue.c
 *  Analig Tuner LTDIS Interface
 */

/***********************************************************************/
/*              Includes                                               */
/***********************************************************************/

#include "PI_Def_dbg.h"
#include "x_rm_dev_types.h"
#include "TDIS.h"
#include "queue.h"
#include "TDMgr.h"
#include "util.h"
#include "drvapi_tuner.h"
#include "pd_tuner_type.h"
#include "source_table.h"
#include "UtilAPI.h"
#include "pd_atdeu_glue.h"
//#include "pd_atdeu_if.h"
#include "atd_feature.h"
#include "pi_demod_atd.h"
#include "nptv_drvif.h"
#include "drv_tvd.h"
#include "aud_drvif.h"
#include "pi_demod_atdeu.h"
#include "x_gpio.h"
//#include "ctrl_bus.h"

// Justin TODO, removed in 5363
#if defined (CC_MT5360)
#include "pi_atdeu_tvsys_info.h"
#include "pi_atdeu_tvsys_info.c"
#define fgGetTVDSts(para1_skip,x)  AtdPIEU_fgDrvTunerCheckTVDLock(x, psAtdPdCtx->u1SubSysIdx)
#endif

// ZY, 091222
#include "api_srch_if.h"
#include "api_notify.h"
//d120605_Haibo:for ATD driver version control in driver base project
#include "atd_version.h"
/***********************************************************************/
/*              Conditional Compiling                                  */
/***********************************************************************/

//#if !defined(CC_MTAL) && !defined(CC_SDAL) //linux platform
//#define ANAEU_ENABLE_MONITOR
//#endif

/***********************************************************************/
/*              Defines                                                */
/***********************************************************************/
#define cMONITOR_PERIOD_MS      500
#define fcADD_TVD_SYNC          1           // Pause TVD before TunerAcq complete, or will show "No Signal"
#define fcADD_AV_SYNC           1
#define fcADD_AV_NOTIFY         1

//for new scan step
#define cNEXT_STEP_TVD_UNLOCK           1500000 //Hz, original value
#define cNEXT_STEP_TVD_LOCK_NOISE_HIGH  2000000 //Hz
#define cNEXT_STEP_TVD_LOCK_PAL_BG      3000000 //Hz, 5.5M + 1.5M
#define cNEXT_STEP_TVD_LOCK_PAL_I       3000000 //Hz, 6.0M + 2.0M  
#define cNEXT_STEP_TVD_LOCK_PAL_DK      3000000 //Hz, 6.5M + 1.5M  
#define cNEXT_STEP_TVD_LOCK_SECAM_L     3000000 //Hz, 6.5M + 1.5M  
#define cNEXT_STEP_TVD_LOCK_SECAM_L1    3000000 //Hz, for auto scan is scan-up process, SIF < VIF
#define cNEXT_STEP_TVD_LOCK_NTSC_M      3000000 //Hz, 4.5M + 1.5M
#define cNEXT_STEP_TVD_UNLOCK_OLD       2000000 //Hz, original value
#define cNEXT_STEP_TVD_LOCK_OLD         3000000 //Hz, original value

#define cRISC_TVD_BASE           (IO_VIRT + 0x22000)
//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
ATD_CTX_T*    psAtdeuDemodCtx=NULL;
#if fcTIMING_MEASURE
UINT32  u2TickStart_eu;//jackson: todo, move to a control block
#endif

extern UINT16 g_CPR_delay;
extern UINT16 g_CPR_cnt;
extern UINT16 g_pra_cnt;
extern UINT16 g_CR_BW_AutoConfig_Delay_cnt;
extern BOOL fg_Acq_flag;

UINT8 g_bypass_pilotsearch;


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static TUNER_DEVICE_CTX_T*		pAtdTunerDeviceCtx = NULL;
static BOOL     fgMonitorThreadRunning = FALSE;
static THREAD_T t_monitor_thread;
static MUTEX_T  t_escape_atdeu_mon_suspend;
//static UINT8 u1MonitorLoopCnt = 0;
static UINT8 fgAtdMonitorRet = FALSE;
#if defined(ATD_SNOWSCREEN_MODE)
static UINT8 fgAtdAFTFailCnt = 0;
#endif
#if defined(ATD_NTSC443_DETECT)
static UINT32 u4PreFreq = 0;
static BOOL fgSameFreq = FALSE;
#endif
static ITUNER_CTX_T *pTunerCtx;
static U8    fgNewScanStep = TRUE;
static BOOL   fgTvdLock=FALSE;
//static BOOL   fgAudScan = FALSE;   
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM 
static U8     u1ChChangeInitSys = MOD_PAL_BG; 
#endif
static UINT16 u2DroTarLvlInitial =0;
//static U8    LevelSwitchTogCnt = 0;

STATIC TUNER_ANA_ATTRIBUTE_T sTunerAnaAttribute =
{
    43250000,
    873225000,
    (90 / 2) * 50000
};

STATIC UINT32  au4TvSysGroup[] =
{
    (TV_SYS_MASK_B | TV_SYS_MASK_G),
    (TV_SYS_MASK_D | TV_SYS_MASK_K),
    (TV_SYS_MASK_I),
    (TV_SYS_MASK_L),
    (TV_SYS_MASK_L_PRIME),
    (TV_SYS_MASK_M | TV_SYS_MASK_N)
};

/***********************************************************************/
/*              External declarations                                  */
/***********************************************************************/
#ifdef EU_AUTO_ATP
extern UINT8 gATP_TEST_ITEM_BIT ;
extern UINT8 gATPFailBitMsk ;
#endif

extern UINT8 bGetSignalType(UINT8 bPath);
#ifdef CC_SUPPORT_PIPELINE
extern UINT8 bGetSignalTypeAVD(UINT8 bPath);
#endif
extern int  snprintf(char *s, size_t n, const char * format, ...);

#if defined(__KERNEL__) && (defined(CC_MT5398)  || defined(CC_MT5880) || defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882))
#define AtdPd_TvdNotify(u1Path, u1SigState)  vApiNotifyTunerAnaTvdSigChg(u1Path, u1SigState)     //Tuner analog tvd notify
#else
#define AtdPd_TvdNotify(u1Path, u1SigState)  vApiNotifySigChg(u1Path, u1SigState, 1)
#endif


#if fcADD_AV_SYNC
// Henry Added for AV synchronization
EXTERN VOID AUD_WaitDspFlush(UINT8 u1DecId);
EXTERN VOID AUD_TriggerDspPlay(UINT8 u1DecId);
EXTERN VOID Aud_SetTvSysMask(UINT8 u1DecId, UINT32 u4Mask);
#endif

#if fcADD_AV_NOTIFY
EXTERN VOID AUD_AtunerSigStableNotify(BOOL fgSatble);
#endif

#if fcADD_TVD_SYNC
EXTERN VOID vTvdHoldMode(UINT8 bHoldOnOff);
#endif

/***********************************************************************/
/*              Private (static) Function Prototypes                   */
/***********************************************************************/
VOID Tuner_AtdEU_Register_LTDIS_Fct(PTD_CTX ptTDCtx);
VOID AtdEU_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv);

/***********************************************************************/
/*              Static Functions                                       */
/***********************************************************************/
/************************************************************************
*  AtdPd_CheckTVDLock
*  Check if TV decoder is lock or not at current frequency.
*  @param  psAtdPdCtx : Pointer use to point to struct ATD_PD_CTX_T.
*  @param  u4Freq : Current frequency value in KHz.
*  @param  cMsgInd : Message ID.
*  @retval  TRUE : TV decoder lock.
*  @retval  FALSE : TV decoder unlock.
************************************************************************/
// Justin TODO, why CID implement TVD lock function?
static BOOL AtdPd_CheckTVDLock(ATD_PD_CTX_T *psAtdPdCtx, UINT32 u4Freq, CHAR cMsgInd)
{
    BOOL    fgLockSts = TRUE;
#if fcTIMING_MEASURE
	HAL_TIME_T	rCurTime={0,0};
    HAL_GetTime(&rCurTime);
    mcSHOW_DBG_MSG(("%%-->%c atv-scan ChkTVD[%d  %d]\n", rCurTime.u4Seconds, rCurTime.u4Micros, cMsgInd));
#endif

#ifdef CC_SUPPORT_ATV_DESCRAMBLE
	   if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn)
		 {
			 mcDELAY_MS(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.u2ChkTvdDelay);
			 mcSHOW_DBG_MSG(("current decramble delay is %d!\n",psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.u2ChkTvdDelay));
		 }
#endif

    
#ifdef CC_SUPPORT_PIPELINE
	if ((bGetSignalTypeAVD(SV_VP_MAIN) != (UINT8) SV_ST_TV)&&(bGetSignalTypeAVD(SV_VP_PIP) != (UINT8) SV_ST_TV))//NOT tv source ,aways return TVD lock,liuyuan,091222
#else
	if ((bGetSignalType(SV_VP_MAIN) != (UINT8) SV_ST_TV)&&(bGetSignalType(SV_VP_PIP) != (UINT8) SV_ST_TV))//NOT tv source ,aways return TVD lock,liuyuan,091222
#endif
    {
        fgLockSts = TRUE;
        mcSHOW_DBG_MSG(("NOT TV SOURCE,NOT TO DO TVD CHECK!"));
        return fgLockSts;
    }
	
    /* do TVD check when auto search find VIF, LC Chien 070302*/
    if (fgGetTVDSts(psAtdeuDemodCtx,CHN_LOCK_CHECK_TIME))
    {
#if TVD_SUPPORT_SNOW_MODE
        ATD_SetSnowMode(psAtdeuDemodCtx,FALSE);//d20110718_Haibo:Disable TVD snow mode after TVD lock
#endif
#if defined(ATD_NTSC443_DETECT)
{
        UINT8 ucBurstFreq, ucBurstFreq2, ucData;

        if(psAtdPdCtx->u1SubSysIdx >= MOD_NTSC_M) //first detect TVD lock
        {
            mcDELAY_MS(50);
            ucBurstFreq = bDrvTvdCheckBurstFreq();

            if((TVD_BLOCK_IS358 == ucBurstFreq ) && fgHwTvd525())//d111107_Haibo:Add line num check
            {
                fgLockSts = TRUE;
                mcSHOW_DBG_MSG(("Burst 358 is detected and line num is 525!\n"));
            }
            else if (fgSameFreq)
            {
                fgLockSts = TRUE;
                mcSHOW_DBG_MSG(("Burst unknow but retry M!\n"));
            }
            else 
            {
                psAtdPdCtx->u1SubSysIdx = MOD_PAL_DK;
                psAtdeuDemodCtx->u1SubSysIdx = MOD_PAL_DK;// add by liuqu for update the sys idx!
                ATD_SetFreq(psAtdeuDemodCtx, u4Freq, TRUE);
                ATD_SetSystem(psAtdeuDemodCtx, MOD_PAL_DK);
                ATD_StartAcq(psAtdeuDemodCtx, TRUE);
                vTvd3dReset(); 
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
				if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn)
				{
					fgLockSts = TRUE;
					mcSHOW_DBG_MSG(("Testlog:Do not check TVD burst freq in descramble case!\n"));
				}
				else
#endif
				{
				    mcDELAY_MS(200);
	                ucBurstFreq2 = bDrvTvdCheckBurstFreq();

					if(TVD_BLOCK_IS443 == ucBurstFreq2)
	                {
	                    fgLockSts = TRUE;
	                    mcSHOW_DBG_MSG(("Burst 443 is detected!\n"));
	                }
	                else if(bTvd3dGetColorSystem()==SV_CS_SECAM)
	                {
	                    fgLockSts = TRUE;
	                    mcSHOW_DBG_MSG(("SECAM is detected!\n"));
	                }    
	                else
	                {
	                    ATD_GetReg(psAtdeuDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
	                    ucData = ucData & 0xF7;
	                    ATD_SetReg(psAtdeuDemodCtx, REG_ATD_AVS_VOL, &ucData, 1);
	                    fgLockSts = TRUE;
	                    mcSHOW_DBG_MSG(("Burst unknown!\n"));
	                }
				}
            }            
        }        
        else
        {
            fgLockSts = TRUE;
        } 
}		
#else
        fgLockSts = TRUE;               //move to Thomson/tuner.c
#endif

#if fcTIMING_MEASURE
        HAL_GetTime(&rCurTime);
        mcSHOW_DBG_MSG(("%%<--%c atv-scan ChkTVD[%d  %d], TVD lock!\n", rCurTime.u4Seconds, rCurTime.u4Micros, cMsgInd));
#endif
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
*  AtdPd_ChangeChannel
*  Flush audio DSP once channel is changed.
*  @param  u4CurFreqency : Current frequency value in Hz.
*  @retval  void.
************************************************************************/
STATIC VOID AtdPd_ChangeChannel(UINT32 u4CurFreqency)
{
#ifndef CC_MT5391_AUD_3_DECODER
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
#endif
    return;
}

/************************************************************************
*  AtdPd_ExitTunerAcq
*  Notify audio DSP once tuner acquisition is finished.
*  @param  fgLocked : Current tuner lock status.
*  @retval  void.
************************************************************************/
STATIC VOID AtdPd_ExitTunerAcq(BOOL fgLocked)
{
#if fcADD_AV_NOTIFY
    AUD_AtunerSigStableNotify(fgLocked);
#endif
#if fcADD_TVD_SYNC
    vTvdHoldMode(SV_OFF);
#endif
    return;
}


/************************************************************************
*  AtdEU_AudSetSystem
*  AUD set demod system to detect system type
*  @param  u1SubSysIdx: system type
*  @param  u1ChMode: 0:scan mode,   1:normal mode
*  @retval none 
************************************************************************/
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
void AtdEU_AudSetSystem(UINT8 u1SubSysIdx, UINT8 u1ChMode)
{	
	if(0 == u1ChMode)  // Scan mode
	{		
		psAtdeuDemodCtx->fgAudScan = TRUE;
		ATD_SetSystemAudio(psAtdeuDemodCtx, u1SubSysIdx);
	    ATD_SetScanMode(psAtdeuDemodCtx);
	}
	else // normal mode
	{
		psAtdeuDemodCtx->fgAudScan = FALSE; 
		if (u1ChChangeInitSys !=  u1SubSysIdx)
		{
            ATD_SetSystem(psAtdeuDemodCtx, u1SubSysIdx);
		}  
		else
		{	
            ATD_SetSystemAudio(psAtdeuDemodCtx, u1SubSysIdx);	
		}  
		ATD_SetNormalMode(psAtdeuDemodCtx);
		u1ChChangeInitSys = u1SubSysIdx;
	}				
	
	if(FALSE == psAtdeuDemodCtx->fgAudScan)
    {   
		mcSHOW_DBG_MSG(("AUD Sys Detect Done, u1SubSysIdx = %\n", u1SubSysIdx));
    }
}
#endif
/************************************************************************
*  GetAtdConnMode
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
    MOD_QAM_896 --> First in Scan; _fgAutoSearch = TRUE
    MOD_QAM_1024 --> Fine-tune (EU mode); _fgAutoSearch = TRUE
    
    u4AutoSearchType:
    0 --> Channel Change;
    1 --> Scan;     
    2 --> Fine-tune (EU mode);     
*/
STATIC UINT8 GetAtdConnMode(UINT8 eMod)
{
UINT8   u1ConnMode = 0, ucData1;
UINT32	u4AutoSearchType = 0;
    
    mcSHOW_DBG_MSG(("eMod = %d\n", eMod));   
    
    u4AutoSearchType = u4GetAutoSearchType();
    mcSHOW_DBG_MSG(("u4AutoSearchType = %d\n", u4AutoSearchType));   
    
    // ZY, 091222, u1ConnMode = u4AutoSearchType, except FM radio mode
    switch (u4AutoSearchType)
    {           	
        case ATV_CONN_MODE_CHAN_CHANGE:      
            u1ConnMode = ATV_CONN_MODE_CHAN_CHANGE;
            ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetLNA, 0, &ucData1); 
            mcSHOW_DBG_MSG(( "Connection_type = channel change!\n"));
            break;
                	
        case ATV_CONN_MODE_CHAN_SCAN:                       
            u1ConnMode = ATV_CONN_MODE_CHAN_SCAN;
            ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetLNA, 0, &ucData1);            
            mcSHOW_DBG_MSG(( "Connection_type = channel scan!\n"));
            break;
        
        case ATV_CONN_MODE_FINE_TUNE:                    
            u1ConnMode = ATV_CONN_MODE_FINE_TUNE;
            ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetLNA, 0, &ucData1);             
            mcSHOW_DBG_MSG(( "Connection_type = fine tune!\n"));
            break; 
        default:
            break;
    }                    
    
    // check eMod for FM radio type
    switch (eMod)
    {         
#ifndef CC_MT5360
        case MOD_FM_RADIO:
            u1ConnMode = ATV_CONN_MODE_FM_RADIO;
            ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetLNA, 0, &ucData1);  
            mcSHOW_DBG_MSG(( "Mod_type = FM radio!\n"));
            break;
#endif        
        default:
            break;
    }
        
    return u1ConnMode;
}

static BOOL SetBreakMonitor(ATD_PD_CTX_T *psAtdPdCtx)
{
    UINT8 u1Cnt=0;
    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        psAtdPdCtx->fgBreakMonitor = TRUE; //set to break monitor
    
        if(psAtdPdCtx->fgPIMoniStatus)  //PI really doing PI Monitor
        {  
            while(psAtdPdCtx->fgPIMoniStatus)
            {
                //wait PI Monitor really finish break
                if(u1Cnt>250)  //in case of deadlock
                {  
                    break;
                }
                u1Cnt++;
                mcSHOW_DBG_MSG(("ATD EU PD wait break PI monitor finish. u1Cnt=%d\r\n",u1Cnt));
                mcDELAY_MS(10);
            }
        }
        mcSHOW_DBG_MSG(("Disable monitor process!!\n"));
    }
    return TRUE;
}

static void ResumeMonitor(ATD_PD_CTX_T *psAtdPdCtx)
{
    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
#if defined(ATD_SNOWSCREEN_MODE)
        fgAtdAFTFailCnt = 0;
        fgAtdMonitorRet = FALSE;
#endif
        psAtdPdCtx->fgBreakMonitor = FALSE;
        mcMUTEX_UNLOCK(t_escape_atdeu_mon_suspend);
    }
}

/************************************************************************
*  i4AtdEUMonitorBreak
*  Break function of monitor thread.
*  @param  *pvArg : A pointer to ATD_PD_CTX_T structure.
*  @retval  Break flag of break function.
************************************************************************/
STATIC INT32 i4AtdEUMonitorBreak(VOID *pvArg)
{
    ATD_PD_CTX_T *psPdCtx = (ATD_PD_CTX_T *) pvArg;

    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (psPdCtx->fgBreakMonitor)
        {
            mcSHOW_DBG_MSG(("ATD_EU_Monitor break function triggered!!\n"));
        }
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
    ATD_PD_CTX_T *psPdCtx = *(ATD_PD_CTX_T **) pvArg;

    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {

        mcSHOW_DBG_MSG(("MonitorProcess start!\n"));

#if defined(ATD_SNOWSCREEN_MODE)
        UINT8 ucData1, ucVpress;
#endif 
        while (fgMonitorThreadRunning)
        {
            if (psPdCtx->fgBreakMonitor)
            {
                // After mcMUTEX_LOCK(t_escape_anaeu_mon_suspend), monitor thread will be suspended until
                // TunerAcq or TunerClose invoked.
                mcSHOW_DBG_MSG(("Entering ATD_EU mon_suspend\n"));
                mcMUTEX_LOCK(t_escape_atdeu_mon_suspend);
                mcSHOW_DBG_MSG(("Escape ATD_EU mon_suspend\n"));
            }
            else
            {
                // mutex lock for I2C access
                mcSEMA_LOCK_HAL(psPdCtx->hHalLock);
                mcSHOW_DBG_MSG4(("Invoke  ATD_EU_Monitor\n"));  //liuqu change debug level

                //add PI monitor API here
                psPdCtx->fgPIMoniStatus = TRUE; //doing PI monitor
#if defined(ATD_SNOWSCREEN_MODE)

				if(1 == fgAtdAFTFailCnt)
                {                                                
                    ucVpress = (IO_READ8(VIDEO_IN0_BASE, 0x81) & 0x10) >> 4;
					if(0x01 == ucVpress)	
                    {
                        ucData1 = 0x08;
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_POA_CFG, &ucData1, 1);
                        ucData1 = 0x04;                    
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_PE_CFOK, &ucData1, 1);
                        ucData1 = 0x21;                    
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_PS_CFG, &ucData1, 1);
                                      
                        psAtdeuDemodCtx->u1PF = 0x01;                    
                        ATD_SetNormalMode(psAtdeuDemodCtx);
                        mcSHOW_DBG_MSG(("VPress lock, restart AFT!!!!!!!!!!!!!!!!!!!!\n"));
                        fgAtdAFTFailCnt = 0;	
                    }
                    else//d20110719_Haibo:reset CR when no signal
                    {
                        ucData1 = 0xFE;
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_CTL_RST2, &ucData1, 1);
                        ucData1 = 0xFF;                    
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_CTL_RST2, &ucData1, 1); 
                    }
                }

                if(0 == fgAtdAFTFailCnt)
                {
                    fgAtdMonitorRet = ATDEU_Monitor(psAtdeuDemodCtx, &psPdCtx->Frequency, i4AtdEUMonitorBreak, psPdCtx);          		
                }

                if (fgAtdMonitorRet) 
                {
                    ATD_Patch(psAtdeuDemodCtx);
#if !ATD_SUPPORT_DRO_SYNC_MODE && !ATD_SUPPORT_H_BLANK_MODE
                    if (fgHwTvdVPresTVD3D())
                    {
                        ATD_DroTargetLevelAdjust(psAtdeuDemodCtx,u2DroTarLvlInitial);
                    }
#else
                    if(IC_VER_5396_AA==BSP_GetIcVersion())//MT5396 before ECO)
                    {
                        if (fgHwTvdVPresTVD3D())
                        {
                            ATD_DroTargetLevelAdjust(psAtdeuDemodCtx,u2DroTarLvlInitial);
                        } 
                    }
#endif				
                }            
                else
                {
					if(0 == fgAtdAFTFailCnt)	
                    {                     
                        mcSHOW_DBG_MSG(("1st time monitor fail!\n"));                                                          
                        ucData1 = 0x00;
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_POA_CFG, &ucData1, 1);
                        ucData1 = 0x09;                    
                        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_ACR_CFG, &ucData1, 1);                   
                        fgAtdAFTFailCnt++;
                    }
                }    
#else                          
                fgAtdMonitorRet = 
                ATDEU_Monitor(psAtdeuDemodCtx, &psPdCtx->Frequency, i4AtdEUMonitorBreak, psPdCtx);
                // do ATD_Patch every 100ms            
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
                if ((1 == fgAtdMonitorRet) && (FALSE == psAtdeuDemodCtx->fgAudScan)) 
#else	
                if (fgAtdMonitorRet) 
#endif
                {  	
#if defined(TVD_SNOWSCREEN_MODE)  
                    ATD_SetSnowMode(psAtdeuDemodCtx,FALSE);//d20110719_Haibo:When AFT ok.Disable snow mode
#endif			 
                    ATD_Patch(psAtdeuDemodCtx);
#if !ATD_SUPPORT_DRO_SYNC_MODE && !ATD_SUPPORT_H_BLANK_MODE
                    if (fgHwTvdVPresTVD3D())
                    {
                        ATD_DroTargetLevelAdjust(psAtdeuDemodCtx,u2DroTarLvlInitial);	
                    }
#else
                    if(IC_VER_5396_AA==BSP_GetIcVersion())//MT5396 before ECO)
                    {
                        if (fgHwTvdVPresTVD3D())
                        {
                            ATD_DroTargetLevelAdjust(psAtdeuDemodCtx,u2DroTarLvlInitial);
                        } 
                    }
#endif
                }    
#endif
                psPdCtx->fgPIMoniStatus = FALSE; //finish doing PI monitor or breaked
                mcSEMA_UNLOCK_HAL(psPdCtx->hHalLock);
                mcDELAY_MS(cMONITOR_PERIOD_MS);
            }
        }

        t_monitor_thread = (THREAD_T)NULL;
        mcSHOW_DBG_MSG(("MonitorProcess exit!\n"));
        mcTHREAD_DESTROY();
    }
}

/************************************************************************
*  PD_StartMonitorEngine
*  Function which starts monitor thread engine.
*  @param  *psAtdPdCtx : A pointer to ATD_PD_CTX_T structure.
*  @retval  DRVAPI_TUNER_OK : Monitor thread create successfully.
*  @retval  DRVAPI_TUNER_ERROR : Monitor thread create fail.
************************************************************************/
static S32 PD_StartMonitorEngine(ATD_PD_CTX_T *psAtdPdCtx)
{
    //ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;

    // x_thread_create(HANDLE_T *ph_th_hdl, CHAR *ps_name, SIZE_T z_stack_size,
    //                 UINT8 ui1_pri, x_os_thread_main_fct pf_main_rtn,
    //                  SIZE_T z_arg_size, VOID *pv_arg);
    // Important: x_thread_create will copy pv_arg to its memory while not
    // directly use pv_arg. Therefore, if we want thread to share the same
    // context as caller, we need set pv_arg as "pointer of context".
   	if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{

        fgMonitorThreadRunning = TRUE;
        if (mcTHREAD_CREATE(&t_monitor_thread,
                "EU_ATD_MonThread",
                cTHREAD_STACK_SIZE, cTHREAD_PRIORITY,
                MonitorProcess, sizeof(void*), (VOID *) &psAtdPdCtx) != OSR_OK)
        {
            mcSHOW_DBG_MSG(("PD_StartMonitorEngine (mcTHREAD_CREATE): DRVAPI_TUNER_ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
        mcSHOW_DBG_MSG(("PD_StartMonitorEngine success\n"));
    }
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
    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        while (t_monitor_thread)
        {
            mcDELAY_MS(10); // in ms
        }
        mcSHOW_DBG_MSG(("ATD EU PD_StopMonitorEngine success\n"));
    }
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  vSetTvSubSysIdx
*  Set specified TV system mask into TV system index of PD context.
*  @param  *psAtdPdCtx : A pointer to ATD_PD_CTX_T structure.
*  @param  u4TvSubSysMask : TV system mask.
*  @retval  TRUE : Current TV system is changed.
*  @retval  FALSE : Current TV system is not changed.
************************************************************************/
STATIC UINT8 vGetTvSubSysIdx(ATD_PD_CTX_T *psAtdPdCtx, UINT32  u4TvSubSysMask)
{
    UINT8   u1SubSysIdx = MOD_NTSC_M;
    mcSHOW_DBG_MSG(("%s: u4TvSubSysMask = %04X\n", __FUNCTION__, u4TvSubSysMask));
    if (u4TvSubSysMask & (TV_SYS_MASK_B | TV_SYS_MASK_G))
    {
        u1SubSysIdx = MOD_PAL_BG;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_D | TV_SYS_MASK_K))
    {
        u1SubSysIdx = MOD_PAL_DK;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_I))
    {
        u1SubSysIdx = MOD_PAL_I;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_L))
    {
        u1SubSysIdx = MOD_SECAM_L;
    }
    else if (u4TvSubSysMask & (TV_SYS_MASK_L_PRIME))
    {
        u1SubSysIdx = MOD_SECAM_L1;
    }

    mcSHOW_DBG_MSG(("%s: u1SubSysIdx = %d\n", __FUNCTION__, u1SubSysIdx - MOD_ANA_TYPE_BEGIN));

    return (u1SubSysIdx);
}

/***********************************************************************/
/*              Public Functions                                       */
/***********************************************************************/
UINT32 AtdEU_GetNoiseLevel(void)
{
    return ATDPI_GetNoiseLevel(psAtdeuDemodCtx);
}
//wenming 2012-09-13:Add for other modules get ATV TV system setting
UINT8 AtdEU_GetTVSystem(void)
{
	return psAtdeuDemodCtx->u1SubSysIdx;
}
UINT16 AtdEU_GetIFAGC(void)
{
    return ATDPI_GetIFAGC(psAtdeuDemodCtx);
}
BOOL AtdEU_SetLNA(BOOL fgLNA)
{
    return ATDPI_SetLNA(psAtdeuDemodCtx, fgLNA);
}

BOOL AtdEU_SetAntIn(UINT8 fgAntIn)
{
    return ATDPI_SetAntIn(psAtdeuDemodCtx, fgAntIn);
}
/*********************************************************
AtdEU_SetDeScrambleBox
This API is for descramble box setting
BOOL fgScramble : TRUE --enable descaramble fuction;FALSE --disable descaramble fuction;
UINT16 DelayForDescramble  When enable descaramble fuction. The delay time(MS) add before check TVD 
*********************************************************/
BOOL AtdEU_SetDeScrambleBox(BOOL fgScramble, UINT16 DelayForDescramble )
{
	#ifdef CC_SUPPORT_ATV_DESCRAMBLE
	if(fgScramble)
	{
		
		psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.u2ChkTvdDelay = 0;
		mcSHOW_DBG_MSG(("[Pd_atdeu_glue.c]->>>descramble on\n"));
	}
	else
	{
		
		psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.u2ChkTvdDelay = 0;
		mcSHOW_DBG_MSG(("[Pd_atdeu_glue.c]->>>descramble off\n"));
	}
	psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn = fgScramble;
	psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgGenSnowChChg = fgScramble;
	vApiSetForDescrambler(fgScramble);
	ATD_SetDelayLine(psAtdeuDemodCtx);
	#endif
	
	return TRUE;
}

/************************************************************************
*  AtdEU_TunerOpen
*  Analog glue layer initiliazation function.
*  @param  ptTDCtx : A pointer use to point to struct TUNER_DEVICE_CTX_T.
*  @param  ptTunerCfgInfo : A pointer use to point to struct TUNER_CONFIG_T.
*  @param  pptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  DRVAPI_TUNER_OK : Analog glue initialization successfully.
*  @retval  DRVAPI_TUNER_ERROR : Analog glue initialization fail.
************************************************************************/
INT32 AtdEU_TunerOpen(PTD_CTX ptTDCtx, TUNER_CONFIG_T * ptTunerCfgInfo,
                     PTD_SPECIFIC_CTX* pptTDSpecificCtx)
{
#if (FALSE == ATD_NOT_USE)
    ATD_PD_CTX_T   *psAtdPdCtx;

    Tuner_AtdEU_Register_LTDIS_Fct(ptTDCtx);
    /*create ATD PD CTX*/
    pAtdTunerDeviceCtx = (TUNER_DEVICE_CTX_T*)ptTDCtx;
    psAtdPdCtx = (ATD_PD_CTX_T *) mcALLOC_MEM(sizeof(ATD_PD_CTX_T));
	if (NULL == psAtdPdCtx)	
    {
        return (DRVAPI_TUNER_ERROR);
    }    
    /* Init Disconnect Status */
    psAtdPdCtx->fgDisStatus = FALSE;
    *pptTDSpecificCtx = psAtdPdCtx;

    psAtdPdCtx->ucTvSysGroupNum = sizeof(au4TvSysGroup) / sizeof(UINT32);
    psAtdPdCtx->pu4TvSysGroup = au4TvSysGroup;
    psAtdPdCtx->psTunerAnaAttr = &sTunerAnaAttribute;

    psAtdeuDemodCtx = ATD_DemodCtxCreate();
	if (NULL == psAtdeuDemodCtx)		
    {
        return (DRVAPI_TUNER_ERROR);
    }   
    /*link demod ctx to pd ctx*/
    psAtdPdCtx->sAtdPiCtx = psAtdeuDemodCtx;

    psAtdeuDemodCtx->u1TunerType =((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType;
    ITunerRegistration(((TUNER_DEVICE_CTX_T*)ptTDCtx)->tunerType);

    pTunerCtx = ITunerGetCtx();
	if (NULL == pTunerCtx)		
    {
        return (DRVAPI_TUNER_ERROR);
    }   
    ITuner_Init(pTunerCtx);

    psAtdeuDemodCtx->pTCtx = pTunerCtx; 
#if defined(CC_REGION_CHINA_ATD)||defined(CC_REGION_PAA_ATD)
		psAtdeuDemodCtx->pTCtx->u2RetryTVDTm=100;//100ms delay and then re-check TVD lock
#endif
#ifdef CC_SUPPORT_ATV_DESCRAMBLE	
	ATD_InitDescramblePara(psAtdeuDemodCtx);
#endif

    // Initialize ATD context & download FW
    if (ATD_Initialize(psAtdeuDemodCtx, 0x92, TRUE))
    {
        mcSHOW_USER_MSG(("ATD_Initialize failed\n"));
        return (DRVAPI_TUNER_ERROR);
    }
    else 
    {
    	mcSHOW_USER_MSG(("ATD_Initialize successful !\n"));
    }    
	ATD_DisConnect(psAtdeuDemodCtx);//20110503_Haibo:Discconnect ATD after tuner open
	
#if fcADD_HAL_LOCK
    if (mcSEMA_CREATE(&psAtdPdCtx->hHalLock, X_SEMA_STATE_LOCK) != OSR_OK)
    {
        mcSHOW_DRVERR_MSG(("%s: mcSEMA_CREATE(hHalLock) Fail!\n", __FUNCTION__));
        return (DRVAPI_TUNER_ERROR);
    }
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
#endif

#if fcTUNE_NOT_START
    psAtdPdCtx->u1TunerAcqType = ACQ_ACTION_DEFAULT;
#endif
    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        if (mcSEMA_CREATE(&t_escape_atdeu_mon_suspend, X_SEMA_STATE_LOCK) != OSR_OK)
        {
            mcSHOW_DBG_MSG(("EU_ATD_TunerOpen (mcSEMA_CREATE): ERROR!\n"));
            return (DRVAPI_TUNER_ERROR);
        }
        psAtdPdCtx->fgBreakMonitor = TRUE;//init
        psAtdPdCtx->fgPIMoniStatus =FALSE; //init
        PD_StartMonitorEngine(psAtdPdCtx);  //start monitor engine
    }
#endif
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  AtdEU_TunerClose
*  Analog glue layer close function.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  DRVAPI_TUNER_OK : Analog glue close successfully.
*  @retval  DRVAPI_TUNER_ERROR : Analog glue close fail.
************************************************************************/
INT32 AtdEU_TunerClose(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
#if (FALSE == ATD_NOT_USE)
#if fcADD_HAL_LOCK
    ATD_PD_CTX_T *psAtdPdCtx;
#endif
#endif
#ifdef CC_SUPPORT_STR
    ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetTunerInit, (UINT8)FALSE, NULL);
#endif

#if (FALSE == ATD_NOT_USE)
#if fcADD_HAL_LOCK
    psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif

    if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
    {
        psAtdPdCtx->fgBreakMonitor = TRUE; //disable and breadk monitor thread
        fgMonitorThreadRunning = FALSE; //need set to false before mcMUTEX_UNLOCK(t_escape_mon_suspend);
        mcMUTEX_UNLOCK(t_escape_atdeu_mon_suspend);
        PD_StopMonitorEngine();  // stop monitor engine
        mcMUTEX_DESTROY(t_escape_atdeu_mon_suspend);  // destory monitor thread
    }
//lei131118 for fastboot refine	
#if fcADD_HAL_LOCK
	if (mcSEMA_DESTROY(psAtdPdCtx->hHalLock) != OSR_OK)
	{
		mcSHOW_DRVERR_MSG(("%s: mcSEMA_DESTROY(hHalLock) Fail!\n", __FUNCTION__));
		return (DRVAPI_TUNER_ERROR);
	}
#endif

    ATD_DemodCtxDestroy(psAtdeuDemodCtx);
    mcFREE_MEM(ptTDSpecificCtx);
#endif
    return (DRVAPI_TUNER_OK);
}

/************************************************************************
*  AtdEU_TunerAcq
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
//get the time cost tag of ATD_ACQ
#define SUPPORT_OUTPUT_CHANNELSCAN_TIME      0   
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
     extern UINT32 os_get_sys_tick(void);
#endif
UINT8 u1FMRadioDelay=20;
BOOL AtdEU_TunerAcq(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    VOID* pv_conn_info,
                    SIZE_T z_conn_info_len,
                    x_break_fct _BreakFct,
                    VOID* pvArg)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    UINT32  i4CurFreq = cTUNER_DEFAULT_FREQ;
    UINT32  u4Freq, u4FreqBound;
    INT32   s4FreqOffset;
    INT16   s2FreqOffset;
    UINT8   u1Path = SV_VP_MAX;
    UINT8   u1ATVStatus = 0;
    UINT8   ucData1;
    UINT8   u1ConnMode;
    TUNER_MODULATION_T emod;
    BOOL    fgLockSts = TRUE;
    UINT8  u1AirOrCable=0;	
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
    UINT16          u2AcqTime_Test;
    UINT32          u4TickCnt_Start;
    UINT32          u4TickCnt_Test; 
    u4TickCnt_Start = os_get_sys_tick();
#endif
    SetBreakMonitor(psAtdPdCtx);
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    DigTunerBypassI2C (TRUE);
    fg_Acq_flag = TRUE;

	if ((TUNER_CONN_TYPE_TER_ANA == e_conn_type)		
        || ((TUNER_CONN_TYPE_TER_ANA_SCART_OUT == e_conn_type)))
    {
        u4Freq = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq;
        u4FreqBound = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq_bound;
        psAtdPdCtx->u4TvSubSysMask = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_tv_sys_mask;
        psAtdeuDemodCtx->fgAFT = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune;
        s2FreqOffset = ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->i2_fine_tune_offset;
        AtdPd_ChangeChannel(u4Freq);
        
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
	    u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
	    mcSHOW_DBG_MSG(("ATD_Acq Connection time,after AtdPd_ChangeChannel = %d ms\n", u2AcqTime_Test));
#endif		
        emod=((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->e_mod;	
        u1ConnMode = GetAtdConnMode(emod);
        psAtdPdCtx->u1SubSysIdx = vGetTvSubSysIdx(psAtdPdCtx, psAtdPdCtx->u4TvSubSysMask);        
        u1AirOrCable=0;        
    }
    else if (TUNER_CONN_TYPE_CAB_ANA == e_conn_type) 		
    {
        u4Freq = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq;
        u4FreqBound = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_freq_bound;
        psAtdPdCtx->u4TvSubSysMask = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_tv_sys_mask;
        psAtdeuDemodCtx->fgAFT = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->b_fine_tune;
        s2FreqOffset = ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->i2_fine_tune_offset;
        AtdPd_ChangeChannel(u4Freq);

#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
	    u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
	    mcSHOW_DBG_MSG(("ATD_Acq Connection time,after AtdPd_ChangeChannel = %d ms\n", u2AcqTime_Test));
#endif
        emod=((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->e_mod;	
        u1ConnMode = GetAtdConnMode(emod);
        psAtdPdCtx->u1SubSysIdx = vGetTvSubSysIdx(psAtdPdCtx, psAtdPdCtx->u4TvSubSysMask); 
		u1AirOrCable=1;
    }
    else
    {
        mcSHOW_DRVERR_MSG(("%s: Wrong e_conn_type (%d)!\n", __FUNCTION__, e_conn_type));
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }
    ITuner_OP(psAtdeuDemodCtx->pTCtx,itSetAntIn,u1AirOrCable,NULL); ;//Switch tuner input  0: Air 1:Cable
    // update psAtdeuDemodCtx->u1SubSysIdx for SetFreq needs TV system too
    psAtdeuDemodCtx->u1SubSysIdx = psAtdPdCtx->u1SubSysIdx;

    mcSHOW_DBG_MSG((" b_fine_tune=%d\n", psAtdeuDemodCtx->fgAFT));
#if fcADD_TVD_SYNC
    if (u1ConnMode != ATV_CONN_MODE_CHAN_SCAN)     // liuqu,20100315, do not do this job in channel scan mode.
    {
        vTvdHoldMode(SV_ON);
    }
#endif 

    if (ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode) 		
    {
        vTvd3dChannelChange();
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM
        psAtdeuDemodCtx->fgAudScan = FALSE;
#endif 

#ifdef CC_SUPPORT_ATV_DESCRAMBLE //d120312_Haibo:Enable ATD snow mode
    if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgGenSnowChChg)
	{
		
		if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn)
			{
	  			ATD_SetATDSnowmode(psAtdeuDemodCtx,TRUE);
			}
	}
#endif	  

    }
    
    mcSHOW_DBG_MSG(("%s (Freq=%uHz) (Freq_bound=%uHz)\n", __FUNCTION__, u4Freq, u4FreqBound));
#if fcTIMING_MEASURE
    u2TickStart_eu = mcGET_SYS_TICK();
#endif

    u4Freq = u4Freq / 1000;  // transfer from Hz to KHz
    u4FreqBound = u4FreqBound / 1000;  // transfer from Hz to KHz

#if defined(ATD_NTSC443_DETECT)
    fgSameFreq = (u4PreFreq == u4Freq)?TRUE:FALSE;
    u4PreFreq = u4Freq;
#endif
        
	if (ATV_CONN_MODE_FM_RADIO == u1ConnMode)
    {
        //EC_code_d100825: For MT5365/95 FM Radio flow modification: 
        psAtdPdCtx->u1SubSysIdx = MOD_PAL_BG;
         psAtdeuDemodCtx->u1SubSysIdx = MOD_PAL_BG;// add by liuqu for update the sys idx!
         
        ATD_SetFreq(psAtdeuDemodCtx, u4Freq, _fgAutoSearch);
        mcSHOW_DBG_MSG(("Set ATDEU freq. of FM Radio = %dKHz\n",u4Freq));
        ATD_ChipInit(psAtdeuDemodCtx);
        ATD_SetSystem(psAtdeuDemodCtx, psAtdPdCtx->u1SubSysIdx);        
        mcSHOW_DBG_MSG(("Set ATDEU system of FM Radio = %d\n",psAtdPdCtx->u1SubSysIdx));  
		ATD_SetAudioSnowMode(psAtdeuDemodCtx,FALSE);//zhilin_20130206:set audio snow mode off in FM mode
		mcSHOW_DBG_MSG(("FM Mode, Disable audio PRBS noise\n"));
        SetFMRadio(psAtdeuDemodCtx, u4Freq);
        ATD_StartAcq(psAtdeuDemodCtx, FALSE);
		mcDELAY_MS(u1FMRadioDelay);//zhilin_20130207:delay 20ms for channel loss
		mcSHOW_DBG_MSG(("FM Mode, delay %d ms to make signal stable"));
        AtdPd_ExitTunerAcq(TRUE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        psAtdPdCtx->u1VIFLock = 2;
        /* Init Disconnect Status */
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return TRUE;	
    }	    
    
    if ((u1ConnMode != ATV_CONN_MODE_CHAN_SCAN)&&(FALSE == psAtdeuDemodCtx->fgAFT) && (s2FreqOffset > -80) && (s2FreqOffset < 80))
    {        
        ucData1 = 0xF9;
        ATD_SetReg(psAtdeuDemodCtx, REG_ATD_ACR_CFG, &ucData1, 1); //EC_code_d9A28: Enable CR auto-reacq for Fine tune mode
#if defined(ATD_FINETUNE_STEPSIZE)  
        s4FreqOffset = s2FreqOffset * ATD_FINETUNE_STEPSIZE; //EC_code_dA111: For SY project, ATD_FINETUNE_STEPSIZE should be define as "31250" equal to 31.25KHz for each step
#else
        s4FreqOffset = s2FreqOffset * 0;
#endif
        mcSHOW_DBG_MSG(("Original freq = %dKHz, Offset freq = %dKHz\n",u4Freq, s4FreqOffset/1000));
        u4Freq = u4Freq + s4FreqOffset/1000;
        mcSHOW_DBG_MSG(("New freq = %dKHz\n",u4Freq));
    }
    
    if (ATD_SetFreq(psAtdeuDemodCtx, u4Freq, _fgAutoSearch) != TRUE) /*jackon: why need use SV_TRUE?*/
    {
        mcSHOW_USER_MSG(("Freq %dKHz setting fail!.Return directly\n", u4Freq));
        AtdPd_ExitTunerAcq(FALSE);
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
		return FALSE;//d121015_Haibo:return false when tuner set fail
    }
        
    if (u1ConnMode != ATV_CONN_MODE_FINE_TUNE)
    {
        ATD_ChipInit(psAtdeuDemodCtx);		
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
	    u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
	    mcSHOW_DBG_MSG(("ATD_Acq Connection time,after ATD_ChipInit = %d ms\n", u2AcqTime_Test));
#endif
        ATD_SetSystem(psAtdeuDemodCtx, psAtdPdCtx->u1SubSysIdx);
        if ((MOD_SECAM_L == psAtdPdCtx->u1SubSysIdx) ||(MOD_SECAM_L1 == psAtdPdCtx->u1SubSysIdx))// to get the initial DRO target level
        {
            ATD_GetReg(psAtdeuDemodCtx, REG_ATD_DRO_LP_CFG03, &ucData1, 1); 
            u2DroTarLvlInitial = 0x0000;
		    u2DroTarLvlInitial |= (UINT16)ucData1;
    		u2DroTarLvlInitial = (u2DroTarLvlInitial << 8);
	    	ATD_GetReg(psAtdeuDemodCtx, REG_ATD_DRO_LP_CFG04, &ucData1, 1);
		    u2DroTarLvlInitial |= (UINT16)ucData1;
			u2DroTarLvlInitial &= 0x7FFF;
    		mcSHOW_DBG_MSG1(("The SECAML/L1 DRO target level(initial value) = 0x%04X \n", u2DroTarLvlInitial));
        }
			
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
		u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
		mcSHOW_DBG_MSG(("ATD_Acq Connection time,after ATD_SetSystem = %d ms\n", u2AcqTime_Test));
#endif
    }

	if (ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode) 	
    {
        //Reset CR module before SetFreq when channel change ,for CR DTV00144511
		ucData1 = 0xFE; 
	    ATD_SetReg(psAtdeuDemodCtx, REG_ATD_CTL_RST2, &ucData1, 1);
		ucData1 = 0xFF; 
	    ATD_SetReg(psAtdeuDemodCtx, REG_ATD_CTL_RST2, &ucData1, 1);
		
		// Release PRA for TDTWS810D Analog channel change LNA 
		if (cLG_TDTWS810D == psAtdeuDemodCtx->u1TunerType)	
		{
		   ucData1 = 0xFF; 
	       ATD_SetReg(psAtdeuDemodCtx, REG_ATD_CTL_RST1, &ucData1, 1); //realese data path register,for TDTWS810D tuner LNA
		}
    }
    	
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
	u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
	u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
	mcSHOW_DBG_MSG(("ATD_Acq Connection time,before ATD_StartAcq = %d ms\n", u2AcqTime_Test));
#endif

    // Justin TODO, fgScanMode & psAtdeuDemodCtx->u1PF necessary??
	if (ATV_CONN_MODE_CHAN_SCAN == u1ConnMode)		
    {
		if(TRUE == psAtdeuDemodCtx->fgAFT) 			
        {
    	    if(g_bypass_pilotsearch)
            {
                psAtdeuDemodCtx->u1PF = 0x00;  //disable pilot searcher when tvd unlock
                g_bypass_pilotsearch=0x00;
                mcSHOW_DBG_MSG(("Last status is VIF full lock,TVD unlock,need bypass pilot search \n"));
            }
            else
            {
        	    psAtdeuDemodCtx->u1PF = 0x01;  //enable pilot filter when initial scan
            }
    	}
    	else 
        {
    	    psAtdeuDemodCtx->u1PF = 0x00;  //disable pilot filter when retry during scan
    	}  
		#if  ATD_SUPPORT_VSI
		  if(psAtdPdCtx->u1SubSysIdx >= MOD_NTSC_M)//d120209_Haibo:If it is CN,then set VSI scan mode
		  {
		    ATD_SetVSIScanMode(psAtdeuDemodCtx);
		  }
		#endif
    	ATD_StartAcq(psAtdeuDemodCtx, TRUE);
    }
	else if(ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode)		
    {
        psAtdeuDemodCtx->u1PF = 0x00;  //enable bypass pilot searcher when channel change
        ATD_StartAcq(psAtdeuDemodCtx, FALSE);
#ifdef CC_SUPPORT_AUTO_DETECT_AUD_SYSTEM         
        u1ChChangeInitSys = psAtdPdCtx->u1SubSysIdx; //100412, for sys detect at change change
#endif        
    }
#if (fcTUNER_TYPE ==cLG_TDTKG931D || fcTUNER_TYPE ==cTCL_DT91WT)
	if(ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode)		
    {
        mcSHOW_DBG_MSG(("set LNA for analog TV \n"));
        ITuner_OP(psAtdeuDemodCtx->pTCtx, itSetLNABYADC, 0, 0);
    }
#endif	

#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
    u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
    u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
    mcSHOW_DBG_MSG(("ATD_Acq Connection time,after ATD_StartAcq = %d ms\n", u2AcqTime_Test));
#endif

#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG((" (1.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
#endif
#if fcTUNE_NOT_START
	if (TUNE_FREQ_ONLY == psAtdPdCtx->u1TunerAcqType)	
    {
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
        /* Init Disconnect Status */
        psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return TRUE;
    }
#endif
    /* Wait for TV-decoder to connect to tuner */
    if (u1ConnMode != ATV_CONN_MODE_CHAN_CHANGE)
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
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_PIP\n"));
                break;
            }
#ifdef CC_SUPPORT_PIPELINE
			else if (bGetSignalTypeAVD(SV_VP_MAIN) == (UINT8) SV_ST_TV)
#else
			else if (bGetSignalType(SV_VP_MAIN) == (UINT8) SV_ST_TV)
#endif

            {
                u1Path = SV_VP_MAIN;
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_MAIN\n"));
                break;
            }
#ifdef CC_SUPPORT_PIPELINE
			else if (bGetSignalTypeAVD(SV_VP_PIP) == (UINT8) SV_ST_AV)
#else
			else if (bGetSignalType(SV_VP_PIP) == (UINT8) SV_ST_AV)
#endif

            {
                u1Path = SV_VP_PIP;
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_PIP, AV mode\n"));
                break;
            }
#ifdef CC_SUPPORT_PIPELINE
			else if (bGetSignalTypeAVD(SV_VP_MAIN) == (UINT8) SV_ST_AV)
#else
			else if (bGetSignalType(SV_VP_MAIN) == (UINT8) SV_ST_AV)
#endif

            {
                u1Path = SV_VP_MAIN;
                mcSHOW_DBG_MSG(("[ATuner] u1Path = SV_VP_MAIN, AV mode\n"));
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
            mcSHOW_DRVERR_MSG(("[ATuner] ERROR! without TV-decoder!\n"));
            AtdPd_ExitTunerAcq(FALSE);          
            mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            fg_Acq_flag = FALSE;
            DigTunerBypassI2C (FALSE);
            return FALSE;
        }
    }
    i4CurFreq = u4Freq;

    // LC Chien, check fine-tune flag instead of _fgAutoSearch for EU-mode, 070326
	if ((ATV_CONN_MODE_CHAN_SCAN == u1ConnMode) && (psAtdeuDemodCtx->fgAFT))		
    {
        UINT32 u2FFF = u4Freq;
        UINT32 u2FFFBD = u4FreqBound;
        if(u2FFF== u2FFFBD)
        {
        	u2FFFBD += 100;
        }        
        psAtdeuDemodCtx->fgCheckCR = TRUE;
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
		u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
		mcSHOW_DBG_MSG(("ATD_Acq Connection time,before SearchNearbyFreq = %d ms\n", u2AcqTime_Test));
#endif
#if TVD_SUPPORT_SNOW_MODE
        if(fgTvdLock)
        {
            mcSHOW_DBG_MSG(("Reset TVD status\n"));
            vTvd3dReset();//d20110719_Haibo:Reset TVD staye if previous VPress status is ON 
        }
#endif		 
        ATD_SetSnowMode(psAtdeuDemodCtx, TRUE);//d20110719_Haibo:Enable snow mode before searchNearBy
        u1ATVStatus = ATDEU_SearchNearbyFreq(psAtdeuDemodCtx, &u2FFF, _BreakFct, pvArg, u2FFFBD);
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
		u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
		u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
		mcSHOW_DBG_MSG(("ATD_Acq Connection time,after SearchNearbyFreq = %d ms\n", u2AcqTime_Test));
#endif       
        if (mcTEST_BIT(u1ATVStatus, cpANA_LOCK_STS_VIF))
        {
            fgLockSts = AtdPd_CheckTVDLock(psAtdPdCtx, u2FFF, 'V');
            psAtdPdCtx->u1VIFLock = 2;
        }
        else
        {
            fgLockSts = FALSE;
            psAtdPdCtx->u1VIFLock = 0;
        }

        if (fgLockSts)
        {
            fgTvdLock = TRUE;
            mcSHOW_DBG_MSG(("[ATuner] A_N_S\n"));
            AtdPd_TvdNotify(u1Path, SV_VDO_STABLE);
        }
        else
        {
            fgTvdLock = FALSE;
            mcSHOW_DBG_MSG(("[ATuner] A_N_N\n"));
            AtdPd_TvdNotify(u1Path, SV_VDO_NOSIGNAL);
        }
        i4CurFreq = u2FFF;
    } //end of SearchNearBy
    else
    {    	
        UINT8   ii;
        if(u1ConnMode!=ATV_CONN_MODE_CHAN_SCAN)       
        {
            mcSHOW_DBG_MSG3(("Snow Mode Off: When not do search\n"));
            ATD_SetSnowMode(psAtdeuDemodCtx, FALSE);
        }
        //check both VIF status & CFO when 1st in scan, LC 090813
        if(MOD_QAM_896 == emod)   
        {
            psAtdeuDemodCtx->fgCheckCR = TRUE;
        }
        else
        {
            psAtdeuDemodCtx->fgCheckCR = FALSE;
        }
        switch(u1ConnMode)
        {
            case ATV_CONN_MODE_CHAN_CHANGE:
                /* Not do fine-tune, just wait VIF lock */
                for (ii = 0; ii < (200 / (cCHECK_VIF_DELAY * 10)); ii ++)
                {
                    mcDELAY_MS(10 * cCHECK_VIF_DELAY);
                    u1ATVStatus = ATD_GetVIFLock(psAtdeuDemodCtx, TRUE);
                    // Check VIF really lock during channel change, LC Chien 070504
                    if (u1ATVStatus >= 1)
                        break;
                }
                psAtdPdCtx->u1VIFLock = u1ATVStatus;
			#ifdef CC_SUPPORT_ATV_DESCRAMBLE //d120312_Haibo:Disable ATD snow mode after channel change
                if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgGenSnowChChg)
	            {
	            	if(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn)
					{
			  			ATD_SetATDSnowmode(psAtdeuDemodCtx,FALSE);
					}
	            }
            #endif   
                break;

            case ATV_CONN_MODE_FINE_TUNE:
                //*** ZY 091222, ATD not notify at fine tune mode. TVD will notify
                // always notify signal stable when fine-tune, LC 071002 CR[DTV00106617]
                //_Tvd_NotifyTVSig(u1Path, SV_VDO_STABLE);
                psAtdPdCtx->u1VIFLock = 3;
                break;

            case ATV_CONN_MODE_CHAN_SCAN:
                mcDELAY_MS(20);
                u1ATVStatus = ATD_GetVIFLock(psAtdeuDemodCtx, TRUE);
                //psAtdPdCtx->u1VIFLock = u1ATVStatus;
                // Do TVD check when auto search enable but fine-tune disable, LC Chien 070326
                // When VIFLock=1 at 1st scan, do not do TVD check, LC 090813
                // When VIFLock=1 at retry, do TVD check, LC 090813
                //if ( (u1ATVStatus == 2) || ((u1ATVStatus == 1) && (emod == MOD_UNKNOWN)) )
                if ((u1ATVStatus == 2) ||((u1ATVStatus == 1) && (MOD_NTSC_M!=psAtdPdCtx->u1SubSysIdx)))//d111107_Haibo:Only check CR lock when do system retry.For channel loss caused by FN(CFO variation)
                {
              #ifdef CC_SUPPORT_ATV_DESCRAMBLE //d120312_Haibo:do not check tvd lock status when retry sub system
               		if((psAtdeuDemodCtx->pTCtx->sTunerDescramblePara.fgDescrambleBoxOn) && (psAtdeuDemodCtx->u1SubSysIdx != MOD_NTSC_M))
	           		{
						 mcSHOW_DBG_MSG(("Do not check TVD lock when descramble ON\n"));
						 if(fgTvdLock)//d120312_Haibo:depend on previous tvd lock status 
						 {
						    mcSHOW_DBG_MSG(("[ATuner] A_N_S\n"));    
		                    AtdPd_TvdNotify(u1Path, SV_VDO_STABLE);
						 }
						 else
						 {
						    mcSHOW_DBG_MSG(("[ATuner] A_N_N\n"));
		                    AtdPd_TvdNotify(u1Path, SV_VDO_NOSIGNAL);
						 }
			             psAtdPdCtx->u1VIFLock = 2; 
			         }
					else
				#endif
					{
	                    if (AtdPd_CheckTVDLock(psAtdPdCtx, u4Freq, 'X'))
	                    {
	                        fgTvdLock = TRUE;
	                        mcSHOW_DBG_MSG(("[ATuner] A_N_S\n"));    
	                        AtdPd_TvdNotify(u1Path, SV_VDO_STABLE);    //move to Thomson/tuner.c
	                    }
	                    else
	                    {
	                        fgTvdLock = FALSE;
	                        mcSHOW_DBG_MSG(("[ATuner] A_N_N\n"));
	                        AtdPd_TvdNotify(u1Path, SV_VDO_NOSIGNAL);
	                    }
	                    psAtdPdCtx->u1VIFLock = 2;
	                }
                }
                else
                {
                    fgTvdLock = FALSE;
                    psAtdPdCtx->u1VIFLock = 0;
                    mcSHOW_DBG_MSG(("[ATuner] A_N_N\n"));
                    AtdPd_TvdNotify(u1Path, SV_VDO_NOSIGNAL);
                }
                break;

            default:
                break;
        }
    } //end of non-SearchNearBy

    mcSHOW_DBG_MSG(("VIFLock: %d (%dKHz)\n",
                        psAtdPdCtx->u1VIFLock,
                        i4CurFreq));
#if SUPPORT_OUTPUT_CHANNELSCAN_TIME
	u4TickCnt_Test = os_get_sys_tick()-u4TickCnt_Start;
	u2AcqTime_Test = u4TickCnt_Test * x_os_drv_get_tick_period();
	mcSHOW_DBG_MSG(("ATD_Acq Connection time,after check VIF and TVD = %d ms\n", u2AcqTime_Test));
#endif 

#if fcTIMING_MEASURE
    mcSHOW_DBG_MSG((" (2.%3u ms)\n", (mcGET_SYS_TICK() - u2TickStart_eu) * mcGET_TICK_PERIOD()));
#endif

    mcSHOW_DBG_MSG(("[ATuner] %s fine-tune: %dKHz\n", __FUNCTION__, i4CurFreq));
    psAtdPdCtx->Frequency = i4CurFreq;
    
	if(!_BreakFct(pvArg) && (ATV_CONN_MODE_CHAN_CHANGE == u1ConnMode))		
    {    	
    	ResumeMonitor(psAtdPdCtx);        
    }               
    
    if (psAtdPdCtx->u1VIFLock < 1)
    {
        AtdPd_ExitTunerAcq(FALSE);       
        mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
		psAtdPdCtx->fgDisStatus = FALSE;
        fg_Acq_flag = FALSE;
        DigTunerBypassI2C (FALSE);
        return FALSE;
    }
    
    AtdPd_ExitTunerAcq(TRUE);
#ifndef CC_MT5391_AUD_3_DECODER    
    if(u1ConnMode != ATV_CONN_MODE_CHAN_SCAN) //LC Chien, 20090813
    {
		if(TUNER_CONN_TYPE_CAB_ANA == e_conn_type)			
        {
            Aud_SetTvSysMask(0, ((TUNER_CAB_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_tv_sys_mask);
        }
        else
        {
            Aud_SetTvSysMask(0, ((TUNER_TER_ANA_TUNE_INFO_T *) pv_conn_info)->ui4_tv_sys_mask);
        }
        AUD_TriggerDspPlay(1); /*to start audio*/
    }
#endif
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    /* Init Disconnect Status */
    psAtdPdCtx->fgDisStatus = FALSE;
    fg_Acq_flag = FALSE;
    DigTunerBypassI2C (FALSE);
    return TRUE;
}

/************************************************************************
*  AtdEU_TunerGetSync
*  Get current analog tuner connection status.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  1 : Analog tuner lock.
*  @retval  0 : Analog tuner unlock.
************************************************************************/
INT16 AtdEU_TunerGetSync(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                          DRV_CONN_TYPE_T e_conn_type)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    INT16   i2RetValue = 1;  // Always in sync state
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
	if(0 == psAtdPdCtx->u1VIFLock)	
    {
        i2RetValue = 0;
    }
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    return i2RetValue;
}

/************************************************************************
*  AtdEU_TunerGetSignal
*  Get current analog signal status.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @param  *_pSignal : Output data content pointer.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerGetSignal(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                    DRV_CONN_TYPE_T e_conn_type,
                    SIGNAL *_pSignal)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    UINT32 i4CurFreq;
    UINT32  u4Freq;
    i4CurFreq = psAtdPdCtx->Frequency;
    u4Freq = i4CurFreq * 1000;
    mcSHOW_DBG_MSG(("[ATuner] %s: %04X (%d)", __FUNCTION__, i4CurFreq, u4Freq));
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

/************************************************************************
*  AtdEU_TunerGetSignalLevel
*  Get current analog signal level.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 100.
************************************************************************/
UINT8  AtdEU_TunerGetSignalLevel(PTD_SPECIFIC_CTX ptTDSpecificCtx,
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
    u1Value = bTvdSignalStrengthLevel(2); // bMode = 0, return 0~100
    return (u1Value);
}
#ifndef CC_MT5880_CODE_SIZE_CTRL
void ATD_PowerDown(UINT8 fg)
{
    if(fg)
    {
    	ATD_DisConnect(psAtdeuDemodCtx);
    }
    else
    {
    	ATD_ChipInit(psAtdeuDemodCtx);
    }
    return;
}
#endif

/************************************************************************
*  AtdEU_TunerGetSignalLeveldBm
*  Get current analog signal level in dBm.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 0.
************************************************************************/
INT16 AtdEU_TunerGetSignalLeveldBm(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    UINT8 u1Value;
    u1Value = bTvdSignalStrengthLevel(1); // bMode = 1, return in dB
    return (u1Value);
}

/************************************************************************
*  AtdEU_TunerGetSignalPER
*  Get current analog signal packet error rate.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 0.
************************************************************************/
UINT16  AtdEU_TunerGetSignalPER(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);
}

/************************************************************************
*  AtdEU_TunerGetSignalSNR
*  Get current analog signal SNR.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  Always 0.
************************************************************************/
UINT16 AtdEU_TunerGetSignalSNR(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                    DRV_CONN_TYPE_T e_conn_type)
{
    return (0);                             // return 0 first
}

/************************************************************************
*  AtdEU_TunerSetMpgFmt
*  Get current analog signal mpeg format.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  *pt_mpeg_fmt : Output data content pointer.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerSetMpgFmt(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                            MPEG_FMT_T *pt_mpeg_fmt)
{
    return;
}

/************************************************************************
*  AtdEU_TunerGetVer
*  Get current analog tuner driver version.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  Current analog tuner driver version.
************************************************************************/
CHAR *AtdEU_TunerGetVer(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    char *pVer;
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);	// Avoid re-entry
    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
	if(cCTMR_TUNER!=psAtdeuDemodCtx->u1TunerType)
	{
    ITuner_OP(psAtdeuDemodCtx->pTCtx, itGetVer, 0, &pVer);
	}
	else//d120605_Haibo:driver base case.Return ATD driver version since ATD driver is not open to customer
	{
	   pVer=ATD_DRV_VERSION;
	}
    return ((char*)pVer);
}

/************************************************************************
*  AtdEU_TunerNimTest
*  Test function of tuner driver.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  Always 0.
************************************************************************/
INT32 AtdEU_TunerNimTest(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
    return 0;
}

/************************************************************************
*  AtdEU_TunerSetRegSetting
*  Set tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  ucRegSet : Number of setting regsiters.
*  @param  ucRegAddr : Register address.
*  @param  ucRegValue : Register value.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerSetRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx, UCHAR ucRegSet,
                            UCHAR ucRegAddr, UCHAR ucRegValue)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AtdEU_TunerShowRegSetting
*  Show tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerShowRegSetting(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AtdEU_TunerTestI2C
*  Test tuner I2C bus.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerTestI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx)
{
    mcSHOW_DBG_MSG(("%s\n", __FUNCTION__));
}

/************************************************************************
*  AtdEU_TunerRegRead
*  Read tuner register.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @param  ucRegAddr : Register address.
*  @param  *pucBuffer : Output data content pointer.
*  @param  ucByteCount : Number of setting regsiters.
*  @retval  Always 0.
************************************************************************/
INT32 AtdEU_TunerRegRead(PTD_SPECIFIC_CTX ptTDSpecificCtx, DRV_CONN_TYPE_T e_conn_type,
                    UCHAR ucRegAddr, UCHAR *pucBuffer, U16 ucByteCount)
{
    return 0;
}

/************************************************************************
*  AtdEU_TunerDisc
*  Disconnect analog tuner driver.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_conn_type : Connection type.
*  @retval  void.
************************************************************************/
VOID  AtdEU_TunerDisc(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                                DRV_CONN_TYPE_T e_conn_type)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;    
	if (TRUE == psAtdPdCtx->fgDisStatus)		
    {
        /* Has Disconnected, return */
        mcSHOW_DBG_MSG(("ATD has Disconnect!\n"));
        return; 
    }
    mcSHOW_DBG_MSG4(("ATD_TunerDisc pPdCtx->hHalLock\n")); 
    mcMUTEX_LOCK(psAtdPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("escape ATD_TunerDisc pPdCtx->hHalLock\n"));

    mcSHOW_DBG_MSG(("[Atuner] %s()\n", __FUNCTION__));

    SetBreakMonitor(psAtdPdCtx);
    ATD_DisConnect(psAtdeuDemodCtx);
    
    mcMUTEX_UNLOCK(psAtdPdCtx->hHalLock);
    mcSHOW_DBG_MSG4(("ATD_TunerDisc unlock pPdCtx->hHalLock\n"));
	/* set disconnect status */
	psAtdPdCtx->fgDisStatus = TRUE;	    
    return;
}

/************************************************************************
*  AtdEU_TunerCommand
*  Analog tuner nim command.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  i4Argc : Nim command parameter number.
*  @param  aszArgv : Nim command parameter.
*  @retval  void.
************************************************************************/
void AtdEU_TunerCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                      INT32 i4Argc, const CHAR** aszArgv)
{
#if fcADD_HAL_LOCK
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
#endif
    UCHAR   ucCmdId = 0;
    UINT16 u2Data;
    UINT8 u1fgMask=1;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    if (i4Argc < 1)
    {
        mcSHOW_USER_MSG(("eq:read out current EQ coefficient \n"));//d110327
    	mcSHOW_USER_MSG(("d:Dump ATD register \n"));//d110330_Haibo:Add command to dump ATD register
    }
    if (i4Argc > 0)
    {
        ucCmdId = *((CHAR *) aszArgv[0]);
        switch (ucCmdId)
        {
            case 'e':
                if('q'==((CHAR *) aszArgv[0])[1])				
                {
                   ATD_GetEQ(psAtdeuDemodCtx);
                }
                break;
                
            case 'd':
                u2Data=0;
                if (i4Argc > 2)
                {
                    u1fgMask=(UINT8)StrToInt(aszArgv[2]);
                }
                else if (i4Argc > 1)
                {
                    u2Data=(UINT16)StrToInt(aszArgv[1]);
                }
                if(u1fgMask)
                {
                    u2Data|=0x8000;
                }
                ATD_DumpRegister(psAtdeuDemodCtx,u2Data); 
                break;
            
            default:
                mcSHOW_USER_MSG(("Invalid parameter!\n"));
                break;
        }
    }

    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    return;
}

/************************************************************************
*  AtdEU_TunerAtdCommand
*  Analog tuner ATD command.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  i4Argc : ATD command parameter number.
*  @param  aszArgv : ATD command parameter.
*  @retval  void.
************************************************************************/
VOID AtdEU_TunerAtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        INT32 i4Argc, const CHAR** aszArgv)
{
	ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;	
	INT32 i4AtdEUArgc;
	const CHAR** aszAtdEUArgv;
	UCHAR	ucAtdCmdId = 0;
	BOOL fgMonitorEnable = 0;

	UINT8	ii;
	UINT32	i4CurFreq;
	UINT32	u4Freq = 0;
#ifndef CC_MT5360
	UINT32	u4Temp;
	BOOL	fgFMstate;
	UINT8	ucFMdir;
	U16 	u2FMstep;
#endif
	
	
	i4AtdEUArgc = i4Argc;
	aszAtdEUArgv = aszArgv;
		
	mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);	// Avoid re-entry
//	  ATD_TunerAtdCommand(psAtdeuDemodCtx, i4AtdEUArgc, aszAtdEUArgv, ATDEU_SearchNearbyFreq);	
	if(pAtdTunerDeviceCtx->u1MWIntf != cMTAL_IF && pAtdTunerDeviceCtx->u1MWIntf != cSDAL_IF)
	{
		fgMonitorEnable = 1;
	}

	if (i4AtdEUArgc < 1)
	{
		mcSHOW_USER_MSG(("Cmd_PD: \n"));		
		mcSHOW_USER_MSG(("\t a [Freq (kHz)]: FM radio test \n"));
		mcSHOW_USER_MSG(("\t d [DeScramble box]: 1/0 : On/OFF\n"));//Test ok
		mcSHOW_USER_MSG(("\t j [Start Freq (kHz)][0/1]: FM radio scan up/down\n")); 	
		mcSHOW_USER_MSG(("\t n [0/1]: Disable/enable new scan step\n"));
		ATD_TunerAtdCommandList(i4AtdEUArgc, fgMonitorEnable);
	}	
	
	if (i4AtdEUArgc > 0)
	{
		ucAtdCmdId = *((CHAR *) aszAtdEUArgv[0]);

		switch (ucAtdCmdId) 		
		{	
			
			case 'm':
				{
					UINT8	ucCmdExt;

					ucCmdExt = ((CHAR *) aszArgv[0])[1];

					if ('p' == ucCmdExt)						
					{
						SetBreakMonitor(psAtdPdCtx);
					}
					else if ('r' == ucCmdExt)						
					{
						ResumeMonitor(psAtdPdCtx);
					}
				}
				break;
			case 'n':
				if (i4Argc > 1)
				{
					fgNewScanStep = StrToInt(aszArgv[1]);
				}
				mcSHOW_USER_MSG(("fgNewScanStep = %d\n", fgNewScanStep));
				break;	 

#ifndef CC_MT5360
			case 'a':
				if (i4Argc > 1)
				{
					i4CurFreq = StrToInt(aszArgv[1]);
				}
				else
				{
					i4CurFreq = psAtdPdCtx->Frequency;
				} 
				u4Temp = 0xffffffff;
				IO_WRITE32(cRISC_TVD_BASE, 0x40C, u4Temp);				  
//	  #ifdef ANAEU_ENABLE_MONITOR									
				SetBreakMonitor(psAtdPdCtx);
//	  #endif				
				SetFMRadio(psAtdeuDemodCtx, u4Freq);
				ATD_SetFreq(psAtdeuDemodCtx, i4CurFreq, FALSE);
				ATD_StartAcq(psAtdeuDemodCtx, FALSE);
				fgFMstate = _AudFMRadioDetection();
				mcSHOW_USER_MSG(("Set Frequency = %d KHz, FM status = %d\n", i4CurFreq, fgFMstate));											
				if (fgFMstate)
				{
					_AudPlayFMRadio(); 
					mcSHOW_USER_MSG(("Play FM radio channel by tuner driver...\n"));   
				}																		 
			break;
			
			case 'j':
				if (2 == i4Argc)					
				{
					i4CurFreq = StrToInt(aszArgv[1]);
					ucFMdir = 0;
				}
				else if(3 == i4Argc)					
				{
					i4CurFreq = StrToInt(aszArgv[1]);
					ucFMdir = StrToInt(aszArgv[2]); 				   
				}
				else
				{
					break;
				}	  
				
				if(ucFMdir)
				{
					u2FMstep = (i4CurFreq - 88000)/50;
					i4CurFreq = i4CurFreq - 100;
				}
				else
				{
					u2FMstep = (108000 - i4CurFreq)/50;
					i4CurFreq = i4CurFreq + 100;
				}
				
				u4Temp = 0xffffffff;
				IO_WRITE32(cRISC_TVD_BASE, 0x40C, u4Temp);	 
//			  #ifdef ANAEU_ENABLE_MONITOR															  
				SetBreakMonitor(psAtdPdCtx);
//			  #endif
				SetFMRadio(psAtdeuDemodCtx, u4Freq);
				
				for(ii=0; ii<=u2FMstep; ii++)
				{
					ATD_SetFreq(psAtdeuDemodCtx, i4CurFreq, FALSE);
					ATD_StartAcq(psAtdeuDemodCtx, FALSE);
					
					fgFMstate = _AudFMRadioDetection();
					
					mcSHOW_USER_MSG(("Set Frequency = %d KHz, FM status = %d\n", i4CurFreq, fgFMstate)); 
					if(fgFMstate)
					{
						if((i4CurFreq - (i4CurFreq/100)*100) != 0 )  //found FM radio at xxx.xMHz
						{							 
							if(ucFMdir)
							{
								ATD_SetFreq(psAtdeuDemodCtx, i4CurFreq-50, FALSE);
							}
							else
							{
								ATD_SetFreq(psAtdeuDemodCtx, i4CurFreq+50, FALSE);
							}								 
							ATD_StartAcq(psAtdeuDemodCtx, FALSE);
							
							if(_AudFMRadioDetection())
							{
								if(ucFMdir)
								{
									i4CurFreq = i4CurFreq - 50;
									mcSHOW_USER_MSG(("Jump -50 KHz!!\n"));
								}
								else
								{
									i4CurFreq = i4CurFreq + 50;
									mcSHOW_USER_MSG(("Jump +50 KHz!!\n"));	  
								}
							}												 
						}		 
						mcSHOW_USER_MSG(("Found FM radio at %d KHz!!\n", i4CurFreq));
						_AudPlayFMRadio(); 
						mcSHOW_USER_MSG(("Play FM radio channel by tuner driver...\n"));  
						break;
					}
					else
					{
						if(ucFMdir)
						{
							i4CurFreq = i4CurFreq - 50;
							if(i4CurFreq < 88000)
							{
								mcSHOW_USER_MSG(("Can not found FM radio!!\n"));
							break;
							}
						}
						else
						{
							i4CurFreq = i4CurFreq + 50;
							if(i4CurFreq > 108000)
							{
								mcSHOW_USER_MSG(("Can not found FM radio!!\n"));
							break;
							}
						}
					}
				}											   
			break;
#endif 
			default:
				ATD_TunerAtdCommand(psAtdeuDemodCtx, i4AtdEUArgc, aszAtdEUArgv, ATDEU_SearchNearbyFreq);
				break;
		}
	}
	mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
}

/************************************************************************
*  AtdEU_TunerGetAttribute
*  Get analog tuner driver attribute.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_get_type : Attribute type.
*  @param  *pvAnaAttribute : Output data content pointer.
*  @param  *pzAnaAttributeLen : Output data length pointer.
*  @retval  RMR_OK : Get analog tuner driver attribute Successfully.
*  @retval  RMR_DRV_INV_GET_INFO : Get analog tuner driver attribute fail.
************************************************************************/
INT16 AtdEU_TunerGetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                           DRV_GET_TYPE_T  e_get_type,
                           VOID *pvAnaAttribute,
                           SIZE_T *pzAnaAttributeLen)
{
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    UINT32  u4TvSubSysMask;
    UINT16 u2Value;


    switch (e_get_type)
    {
        case TUNER_GET_TYPE_RFAGC:
       {
			mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
            u2Value = ATD_GetRFAGC(psAtdeuDemodCtx);
            *(UINT16*)pvAnaAttribute = u2Value;
            *pzAnaAttributeLen = sizeof(UINT16);
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            break;
        }
        case TUNER_GET_TYPE_IFAGC:
        {
			
			mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
            u2Value = ATDPI_GetIFAGC(psAtdeuDemodCtx);
            *(UINT16*)pvAnaAttribute = u2Value;
            *pzAnaAttributeLen = sizeof(UINT16);		
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            break;
        }
        case TUNER_GET_TYPE_AGC:
        {
			mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
            u2Value = ATDPI_GetIFAGC(psAtdeuDemodCtx);
            *(UINT16*)pvAnaAttribute = u2Value;
            *pzAnaAttributeLen = sizeof(UINT16);		
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
            break;
        }
        case TUNER_GET_TYPE_KHZ_CFO:
        {
            INT32 i4Data = 0;
            i4Data = ATD_GetCFOKHz(psAtdeuDemodCtx, FALSE);
            mcSHOW_USER_MSG(("Freq offset = %d KHz\n", i4Data));
            *(INT32*)pvAnaAttribute = i4Data;
            *pzAnaAttributeLen = sizeof(INT32);
            break;
        }
        case TUNER_GET_TYPE_TUNER_STATUS:
        {
            BOOL pllLock = FALSE;
            UINT8 temp = 0;
            DigTunerBypassI2C (TRUE);
            ITuner_OP(psAtdeuDemodCtx->pTCtx, itGetStatus, 0, &pllLock);
            DigTunerBypassI2C (FALSE);
            mcSHOW_DBG_MSG3(("e_get_type = TUNER_GET_TYPE_TUNER_STATUS\n"));
            mcSHOW_USER_MSG(("pllLock = %d\n", pllLock));
            temp = (UINT8)pllLock;
            *pzAnaAttributeLen = sizeof(UINT8);
            *(UINT8*)pvAnaAttribute = temp;
            break;
        }
        case TUNER_GET_TYPE_TV_SYS_MASK:
            u4TvSubSysMask = psAtdPdCtx->u4TvSubSysMask;
            mcSHOW_DBG_MSG(("\n%s: u1SubSysIdx = %d, u4TvSubSysMask = %04X\n", __FUNCTION__,
            psAtdPdCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN, u4TvSubSysMask));
            *((UINT32 *) pvAnaAttribute) = u4TvSubSysMask;
            break;

        case TUNER_GET_TYPE_ANA_ATTRIBUTE:
            {
                TUNER_ANA_ATTRIBUTE_T   *psTunerAnaAttribute;
                psTunerAnaAttribute = (TUNER_ANA_ATTRIBUTE_T *) pvAnaAttribute;
                if(cCUST_TUNER == psAtdeuDemodCtx->u1TunerType)
                {
                psTunerAnaAttribute->ui4_lower_bound_freq=psAtdeuDemodCtx->pTCtx->specific_member.eu_ctx.m_s4AnaFreqBoundLower;
                psTunerAnaAttribute->ui4_upper_bound_freq=psAtdeuDemodCtx->pTCtx->specific_member.eu_ctx.m_s4AnaFreqBoundUpper;
                }
                else
                {
                ITuner_OP(pTunerCtx, itGetAnaFreqBound, 0, psAtdPdCtx->psTunerAnaAttr); // update freq bound freom tuner
                *psTunerAnaAttribute = *((TUNER_ANA_ATTRIBUTE_T *) (psAtdPdCtx->psTunerAnaAttr));
                }
                break;
            }

        case TUNER_GET_TYPE_TV_SYS_GROUP:
            {
                UINT8   u1TvSysGroupIdx;
                TUNER_TV_SYS_GROUP_INFO_T   *psTvSysGroup;
                psTvSysGroup = (TUNER_TV_SYS_GROUP_INFO_T *) pvAnaAttribute;
                u1TvSysGroupIdx = psTvSysGroup->ui1_tv_sys_group_idx;
                psTvSysGroup->ui4_tv_sys_mask = TV_SYS_MASK_NONE;
                if (u1TvSysGroupIdx < psAtdPdCtx->ucTvSysGroupNum)
                {
                    psTvSysGroup->ui4_tv_sys_mask =
                    psAtdPdCtx->pu4TvSysGroup[u1TvSysGroupIdx];
                }
                break;
            }

        case TUNER_GET_TYPE_NEXT_STEP_SIZE:
            {
                TUNER_STEP_SIZE_T *psTunerStepSize;
                UINT8  u1TvdState;
            mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
                psTunerStepSize = (TUNER_STEP_SIZE_T *)pvAnaAttribute;            
                u1TvdState = bTvdNAState4Step();
                // bTvdNAState4Step(): Get TVD noise level
                // 0: reserved
                // 1: Low noise
                // 2: High noise
                if (fgTvdLock) 
                {
                    
                    if (u1TvdState > 1) // if TVD noise is high
                    { 
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_NOISE_HIGH;
                    }
                    else 
                    {
                        if (MOD_PAL_BG == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_BG;            
                        else if (MOD_PAL_DK == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_DK;            
                        else if (MOD_PAL_I == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_PAL_I;            
                        else if (MOD_SECAM_L == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_SECAM_L;            
                        else if (MOD_SECAM_L1 == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_SECAM_L1;            
                        else if (MOD_NTSC_M == psAtdPdCtx->u1SubSysIdx)
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_NTSC_M; 
                        else
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_NOISE_HIGH;                
                    }
                }
                else 
                {
                    psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_UNLOCK;
                    g_bypass_pilotsearch=0x01;
                    mcSHOW_DBG_MSG(("VIF Full lock,TVD Unlock,next step size is 1.5M.try to bypass pilot searcher\n"));
                }

                mcSHOW_DBG_MSG(("TVD noise = %d, TV spec = %d\n", u1TvdState, psAtdPdCtx->u1SubSysIdx - MOD_ANA_TYPE_BEGIN));
                mcSHOW_DBG_MSG(("Next step = %dHz\n", psTunerStepSize->i4_step_size));

                if (!fgNewScanStep)
                {
                    if (fgTvdLock)
                    {
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_LOCK_OLD;
                    }
                    else 
                    {
                        psTunerStepSize->i4_step_size = cNEXT_STEP_TVD_UNLOCK_OLD;
                    }
                }
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
                break;  
            }
	    case TUNER_GET_TYPE_NL:
        {
            *(UINT32*)pvAnaAttribute = AtdEU_GetNoiseLevel();
            *pzAnaAttributeLen = sizeof(UINT32);
        }
		break;
		case TUNER_GET_TYPE_SIGNAL_STATUS://d120320_Haibo:For nim.rs cli command
		{
		     UINT16 u2SigSts; 
			 mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
		     u2SigSts=fgTvdLock;//LSB is TVD lock Sts
			 u2SigSts|=(UINT16)(psAtdPdCtx->u1SubSysIdx)<<8;//MSB is sub system
			*(UINT16*)pvAnaAttribute=u2SigSts;
            *pzAnaAttributeLen = sizeof(UINT16);
			mcSHOW_DBG_MSG(("u2SigSts is 0x%04X\n",u2SigSts));
			mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
		}
		break;
#ifdef CC_SUPPORT_ATV_DESCRAMBLE		
        case TUNER_GET_TYPE_ATD_DESCRAMBLE_PROPERTY:
		{		
		   TUNER_DESCRAMBLE_ATTRIBUTE_T *pDescramblePro=(TUNER_DESCRAMBLE_ATTRIBUTE_T *) pvAnaAttribute;
		   TUNER_DESCRAMBLE_ATTRIBUTE_T * pDesAtt=&(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara);
		   mcSHOW_DBG_MSG2(("descramble Get,,fgDescrambleBoxOn=%d\n",pDesAtt->fgDescrambleBoxOn));
           ATD_GetDelayLine(psAtdeuDemodCtx);//d120312_Haibo:update scaling gain
		   x_memcpy(pDescramblePro,pDesAtt, sizeof(TUNER_DESCRAMBLE_ATTRIBUTE_T));
		   *pzAnaAttributeLen = sizeof(TUNER_DESCRAMBLE_ATTRIBUTE_T);
	   	}
		break;
#endif		
        default:
        {
            mcSHOW_DBG_MSG4(("%s: RMR_DRV_INV_GET_INFO!\n", __FUNCTION__));
            return (RMR_DRV_INV_GET_INFO);
    }
    }

    return (RMR_OK);

}

/************************************************************************
*  AtdEU_TunerSetAttribute
*  Set analog tuner driver attribute.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_set_type : Attribute type.
*  @param  *pvAnaAttribute : Input data content pointer.
*  @param  zAnaAttributeLen : Input data length.
*  @retval  RMR_OK : Set analog tuner driver attribute Successfully.
*  @retval  RMR_DRV_INV_GET_INFO : Set analog tuner driver attribute fail.
************************************************************************/
INT32 AtdEU_TunerSetAttribute(PTD_SPECIFIC_CTX ptTDSpecificCtx,
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

        case TUNER_SET_TYPE_TUNER_LOCAL_DEFINE_RESUME_PI_MONITOR:
            if(psAtdPdCtx->fgBreakMonitor)
            {
                ResumeMonitor(psAtdPdCtx);
            }
            break;
        
        case TUNER_SET_TYPE_FREQ_AUTO_NFY:
            {
                ANALOG_BRDCST_CTRL_FREQ_AUTO_NFY_T* pAutoNFY = (ANALOG_BRDCST_CTRL_FREQ_AUTO_NFY_T *)pvAnaAttribute;
                if (pAutoNFY->b_enable_freq_auto_nfy)
                {
                    ATDEU_SetFreqAutoNFY(TRUE);
                    mcSHOW_DBG_MSG(("TUNER_SET_TYPE_FREQ_AUTO_NFY = TRUE"));        
                }
                else
                {
                    ATDEU_SetFreqAutoNFY(FALSE);
                    mcSHOW_DBG_MSG(("TUNER_SET_TYPE_FREQ_AUTO_NFY = FALSE"));           
                }
                break;
            }
#ifdef CC_SUPPORT_ATV_DESCRAMBLE
		 case TUNER_SET_TYPE_ATD_DESCRAMBLE_PROPERTY:
		 {
			   TUNER_DESCRAMBLE_ATTRIBUTE_T *pDescramblePro=(TUNER_DESCRAMBLE_ATTRIBUTE_T *) pvAnaAttribute;
			   TUNER_DESCRAMBLE_ATTRIBUTE_T * pDesAtt=&(psAtdeuDemodCtx->pTCtx->sTunerDescramblePara);
	           x_memcpy(pDesAtt,pDescramblePro, sizeof(TUNER_DESCRAMBLE_ATTRIBUTE_T));
			   mcSHOW_DBG_MSG2(("descramble set,,fgDescrambleBoxOn=%d\n",pDesAtt->fgDescrambleBoxOn));
			   ATD_SetDelayLine(psAtdeuDemodCtx);
	           vApiSetForDescrambler(pDesAtt->fgDescrambleBoxOn);//d120312_Haibo:update descaramble ON/OFF info to TVD and TVE		   
		}
		 break;
#endif	 
        default:
            mcSHOW_DBG_MSG4(("%s: RMR_DRV_INV_GET_INFO!\n", __FUNCTION__));
            return (RMR_DRV_INV_GET_INFO);
    }

#if fcTUNE_NOT_START
    return (RMR_OK);
#endif
}

/***********************************************************************/
#ifdef fcADD_DIAG_INFO

#define DRV_CUSTOM_TUNER_DIAG_ITEMS         3

/************************************************************************
*  AtdEU_TunerSetDiagCmd
*  Set analog tuner diagnostic information.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_tuner_set_type : Diagnostic information type.
*  @param  *pv_set_info : Input data content pointer.
*  @param  z_size : Input data length.
*  @retval  RMR_OK : Set analog tuner diagnostic information successfully.
*  @retval  RMR_DRV_SET_FAILED : Set analog tuner diagnostic information fail.
*  @retval  RMR_DRV_INV_SET_INFO : Invalid analog tuner diagnostic information.
************************************************************************/
INT32 AtdEU_TunerSetDiagCmd(PTD_SPECIFIC_CTX ptTDSpecificCtx,
                        DRV_CUSTOM_TUNER_SET_TYPE_T     e_tuner_set_type,
                        VOID*                           pv_set_info,
                        SIZE_T                          z_size)
{
	INT32	retSts = RMR_OK;
    ATD_PD_CTX_T *psAtdPdCtx = (ATD_PD_CTX_T *) ptTDSpecificCtx;
    mcSEMA_LOCK_HAL(psAtdPdCtx->hHalLock);  // Avoid re-entry
    switch(e_tuner_set_type)
    {
        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_OPEN:
            // Do initialization here
            break;

        case DRV_CUSTOM_TUNER_SET_TYPE_DIAG_CLOSE:
            // Do cleanup here
            break;
        default:
            retSts = RMR_DRV_INV_SET_INFO;
            break;
    }

    mcSEMA_UNLOCK_HAL(psAtdPdCtx->hHalLock);
    return retSts;
}



/************************************************************************
*  AtdEU_TunerGetDiagInfo
*  Get analog tuner diagnostic information.
*  @param  ptTDSpecificCtx : A pointer use to point to struct PTD_SPECIFIC_CTX.
*  @param  e_tuner_get_type : Diagnostic information type.
*  @param  *pv_get_info : Output data content pointer.
*  @param  *pz_size : Output data length.
*  @retval  RMR_OK : Get analog tuner diagnostic information successfully.
*  @retval  RMR_DRV_NO_GET_INFO : Get analog tuner diagnostic information fail.
*  @retval  RMR_DRV_NOT_ENOUGH_SPACE : Get analog tuner diagnostic information fail.
************************************************************************/
INT32 AtdEU_TunerGetDiagInfo(PTD_SPECIFIC_CTX ptTDSpecificCtx,
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
            snprintf(pDiagContent->cDisplayName,15, "%s", "Version:");
            snprintf(pDiagContent->cDisplayValue, 15, "%s", "MTK_ATD");
            pDiagContent++;

        // Record 2
            snprintf(pDiagContent->cDisplayName,15, "%s", "RF Freq (KHz):");
            snprintf(pDiagContent->cDisplayValue, 15, "%6d", psAtdPdCtx->Frequency);
            pDiagContent++;

        // Record 3
            snprintf(pDiagContent->cDisplayName,15, "%s", "Lock State:");
            snprintf(pDiagContent->cDisplayValue, 15, "%d", psAtdPdCtx->u1VIFLock);
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
static void AtdEU_NonImplement_TunerDtdCommand(PTD_SPECIFIC_CTX ptTDSpecificCtx, INT32 i4Argc, const CHAR** aszArgv)
{
    mcSHOW_USER_MSG(("AtdEU_NonImplement_TunerDtdCommand\r\n"));
}

static void AtdEU_NonImplement_TunerDetachI2C(PTD_SPECIFIC_CTX ptTDSpecificCtx, BOOL bDetach)
{
    mcSHOW_DBG_MSG2(("AtdEU_NonImplement_TunerDetachI2C\r\n"));
}
/***********************************************************************/
/*                  Acquisition APIs                                   */
/***********************************************************************/
/***********************************************************************/
/*                  Public Functions (not static)                      */
/***********************************************************************
*  Tuner_AtdEU_Register_LTDIS_Fct
*  Analog ltdis api mapping.
*  @param  ptTDCtx : A void type pointer use to point to struct TUNER_DEVICE_CTX_T.
*  @retval  void.
***********************************************************************/
VOID Tuner_AtdEU_Register_LTDIS_Fct(PTD_CTX ptTDCtx)
{
    TUNER_DEVICE_CTX_T*     pTunerDeviceCtx = ptTDCtx;

    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerClose = AtdEU_TunerClose;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAcq = AtdEU_TunerAcq;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGerSync = AtdEU_TunerGetSync;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignal = AtdEU_TunerGetSignal;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLevel = AtdEU_TunerGetSignalLevel;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalLeveldBm = AtdEU_TunerGetSignalLeveldBm;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalPER = AtdEU_TunerGetSignalPER;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetSignalSNR = AtdEU_TunerGetSignalSNR;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetMpgFmt = AtdEU_TunerSetMpgFmt;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetVer = AtdEU_TunerGetVer;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerNimTest = AtdEU_TunerNimTest;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerCommand = AtdEU_TunerCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDtdCommand  = AtdEU_NonImplement_TunerDtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerAtdCommand = AtdEU_TunerAtdCommand;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetRegSetting = AtdEU_TunerSetRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerShowRegSetting = AtdEU_TunerShowRegSetting;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerTestI2C = AtdEU_TunerTestI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerRegRead = AtdEU_TunerRegRead;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDisc = AtdEU_TunerDisc;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerDetachI2C   = AtdEU_NonImplement_TunerDetachI2C;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetAttribute = AtdEU_TunerSetAttribute;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetAttribute = AtdEU_TunerGetAttribute;
#ifdef fcADD_DIAG_INFO
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerGetDiagInfo = AtdEU_TunerGetDiagInfo;
    pTunerDeviceCtx->tLTDIS_Fct_Tbl.pf_TunerSetDiagCmd  = AtdEU_TunerSetDiagCmd;
#endif
    return;
}

