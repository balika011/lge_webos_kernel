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
 * $RCSfile: drv_if_tuner.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file tuner.c
 *  Driver interface: TUNER part implementation
 */

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_tuner.h"
#include "u_tuner.h"

#include "dmx_if.h"

//#include "tvsys_info.h"
//#include "TDIS.h"


//#include "PI_Tuner.h"
#include "tuner_if.h"
#include "pcmcia_if.h"
#include "dmx_if.h"

#include "TDIS.h"

//#include "nim_if.h"
#include "pi_anana_if.h"
#include "api_srch_if.h"
#include "u_drv_cust.h"
#include "drv_tvd.h"

LINT_EXT_HEADER_END
#include "x_drv_if.h"
#include "drv_if_config.h"
#if !(TUNER_USE_UTDIS)
#include "nim_drvif.h"
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_TUNER_NUM	2

#define TUNER_ADC_WORKAROUND  0    

/// Number of message slots
#define NUM_MESSAGE                     32

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef union
{
    TUNER_CAB_DIG_TUNE_INFO_T t_cab_tuner_info;
    TUNER_TER_DIG_TUNE_INFO_T t_ter_tuner_info;
    TUNER_CAB_ANA_TUNE_INFO_T			t_cana_tuner_info;
    TUNER_TER_ANA_TUNE_INFO_T           t_tana_tuner_info;    
    TUNER_CAB_DIG_OOB_RX_TUNE_INFO_T t_orx_tuner_info;
    TUNER_CAB_DIG_OOB_TX_TUNE_INFO_T t_otx_tuner_info;
} DRV_TUNER_ALL_INFO_T;

typedef enum
{
    DRV_TUNER_MSG_CONNECT,
    DRV_TUNER_MSG_DISCONNECT,    
    DRV_TUNER_MSG_SEARCH,        
    DRV_TUNER_MSG_KILL,     
    DRV_TUNER_MSG_UNKNOWN
} DRV_TUNER_MSG_TYPE_T;

typedef struct 
{
    DRV_TUNER_MSG_TYPE_T rType;
    DRV_TUNER_CONN_TYPE_T rConn;
    void* pvTag;
    DRV_TUNER_ALL_INFO_T rInfo;   
} _TUNER_EVENT_T;

typedef enum
{
    DRV_TUNER_STATE_NOT_INIT,
    DRV_TUNER_STATE_ANA_CONNECTED,
    DRV_TUNER_STATE_DIG_CONNECTED,
    DRV_TUNER_STATE_DISCONNECTED,    
    DRV_TUNER_STATE_UNKNOWN
} DRV_TUNER_STATE_T;

typedef enum
{
    DRV_TUNER_EVENT_ANA_LOCKED,
    DRV_TUNER_EVENT_DIG_LOCKED,
    DRV_TUNER_EVENT_UNKNOWN
} DRV_TUNER_EVENT_T;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static HANDLE_T _hTunerOpMutex;
static HANDLE_T _hTunerMsgq;
static HANDLE_T _hTunerThread;

static DRV_TUNER_PARAM_T _rTunerParam;
static DRV_TUNER_PARAM_T _rTunerInterParam;
static TUNER_CONFIG_TBL_T _rTunerCfgTbl;
static DRV_TUNER_CONN_TYPE_T _rCurrConnType = DRV_TUNER_CT_MAX;
//static DRV_COMP_FCT_TBL_T _rFuncTbl;
static DRV_TUNER_STATE_T _rTunerState = DRV_TUNER_STATE_NOT_INIT;
static BOOL _fgTunerLockStatus = FALSE;
//static BOOL _fgEnableMonitor = TRUE;

/// Internal message structure
typedef struct
{
    BOOL fgInUse;                   ///< Is this message in-use or free
    DRV_TUNER_MSG_T rMsg;             ///< The message body
} _TUNER_MSG_T;

/// Message slots
static _TUNER_MSG_T _arTunerMsg[NUM_MESSAGE];

/// Critical section state
static CRIT_STATE_T _rLock;

/// Tracking if nested critical section occurred
static BOOL _fgTunerLocking = FALSE;

static BOOL _fgTunerBreak = FALSE;

// CI CAM card connectivity
static BOOL _fgCamConnected = FALSE;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define DRV_TUNER_DIGC_MAX_FREQ     ((UINT32) 865000000)
#define DRV_TUNER_DIGC_MIN_FREQ     ((UINT32)  51000000)

#define DRV_TUNER_DIGT_MAX_FREQ     ((UINT32) 865000000)
#define DRV_TUNER_DIGT_MIN_FREQ     ((UINT32)  51000000)

#define DRV_TUNER_MUTEX_LOCK x_sema_lock(_hTunerOpMutex,X_SEMA_OPTION_WAIT);
#define DRV_TUNER_MUTEX_UNLOCK x_sema_unlock(_hTunerOpMutex);
#define DRV_TUNER_MUTEX_INIT x_sema_create(&_hTunerOpMutex,X_SEMA_TYPE_MUTEX,X_SEMA_STATE_UNLOCK);

#define DRV_TUNER_POLLING_DELAY 200

//-----------------------------------------------------------------------------
// Extern functions
//-----------------------------------------------------------------------------
//extern void TunerDisc(UINT16 TunerID, DRV_CONN_TYPE_T e_conn_type);
//extern void TunerAcq(UINT16 TunerID, DRV_CONN_TYPE_T e_conn_type, 
//            void* pv_conn_info, SIZE_T z_conn_info_len, 
//            x_break_fct _BreakFct, void* pvArg);
//extern INT16 TunerGetSync(UINT16 TunerID, DRV_CONN_TYPE_T e_conn_type);
//extern INT32 TunerOpen(void);
//extern INT32 TDIS_RetrieveTunerCfgTbl(TUNER_CONFIG_TBL_T* ptTunerCfgTbl);
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

/*
static INT32 _ANA_SigNotify(UINT8 u1Status, void* pvTag)
{
    if(u1Status)
    {
        printf("[TUNER] Calling tuner lock done callback from ATuner_mw, DETECTED\n");   
        *((DRV_TUNER_SIGNAL_STATUS_T*)pvTag) = DRV_TUNER_SIGNAL_DETECTED;
    }
    else
    {
        printf("[TUNER] Calling tuner lock done callback from ATuner_mw, NOT DETECTED\n");   
        *((DRV_TUNER_SIGNAL_STATUS_T*)pvTag) = DRV_TUNER_SIGNAL_NOT_DETECTED;
    }
}
*/


static UINT8 _Mask2TvSys(UINT32 u4TvSubSysMask)
{
    UINT8 u1SubSysIdx = SYS_SUB_PAL_BG;
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
    }
    return u1SubSysIdx;
}

//-----------------------------------------------------------------------------
/** Enter demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _Lock(void)
{
    _rLock = x_crit_start();

    ASSERT(!_fgTunerLocking);
    _fgTunerLocking = TRUE;

    UNUSED(_fgTunerLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** Leave demux critical section
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void _Unlock(void)
{
    ASSERT(_fgTunerLocking);
    _fgTunerLocking = FALSE;

    x_crit_end(_rLock);

    UNUSED(_fgTunerLocking);          // Make Lint happy
}


//-----------------------------------------------------------------------------
/** Allocate a message slot.
 *
 *  @param[out]     pu1MsgId        The allocated message id.
 *
 *  @retval TRUE            Successful
 *  @retval FALSE           Failed
 */
//-----------------------------------------------------------------------------
static BOOL _AllocateMessage(UINT8* pu1MsgId)
{
    BOOL fgRet;
    UINT32 i;

    ASSERT(pu1MsgId != NULL);

    fgRet = FALSE;

    _Lock();

    for (i = 0; i < NUM_MESSAGE; i++)
    {
        if (!_arTunerMsg[i].fgInUse)
        {
            _arTunerMsg[i].fgInUse = TRUE;
            *pu1MsgId = (UINT8)i;
            fgRet = TRUE;
            break;
        }
    }

    _Unlock();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** Release a message slot
 *
 *  @param  u1MsgId         The message id
 *
 *  @retval TRUE            Successful
 *  @retval FALSE           Failed
 */
//-----------------------------------------------------------------------------
static BOOL _ReleaseMessage(UINT8 u1MsgId)
{
    if (u1MsgId >= NUM_MESSAGE)
    {
        return FALSE;
    }

    if (!_arTunerMsg[u1MsgId].fgInUse)
    {
        return FALSE;
    }

    _Lock();

    _arTunerMsg[u1MsgId].fgInUse = FALSE;

    _Unlock();

    return TRUE;
}

static INT32 _TunerBreaker(void* pArg)
{
    if(_fgTunerBreak)
    {
        _fgTunerBreak = FALSE;
        return (INT32)TRUE;    
    }
    else
    {
        return (INT32)FALSE;
    }
}

static BOOL _GetLockStatus(_TUNER_EVENT_T* prMsg, UINT32 u4Iter)
{
    UINT16 u2Locked = 0;
    UINT32 i;
    UINT16 u2TunerId;

    UNUSED(_Mask2TvSys);
    
    if(prMsg->rConn == DRV_TUNER_CT_DIGC ||prMsg->rConn == DRV_TUNER_CT_DIGT)
    {
        u2TunerId = _rTunerCfgTbl.tTunerCfg[0].TunerID;
    }
    else
    {
        u2TunerId = _rTunerCfgTbl.tTunerCfg[1].TunerID;
    }
    u2Locked = TunerGetSync(u2TunerId, prMsg->rConn);                             
    for(i = 0; i < u4Iter && u2Locked == 0; i++)
    {                                 
        x_thread_delay(100);
        u2Locked = TunerGetSync(u2TunerId, prMsg->rConn);
    }
    if(u2Locked == 0)
    {
        return FALSE;
    }
    return TRUE;
}

static void _RequestHandler(_TUNER_EVENT_T* prMsg)
{
    switch (prMsg->rType)
    {
        case DRV_TUNER_MSG_DISCONNECT:                	              
            if(prMsg->rConn == DRV_TUNER_CT_DIGC || prMsg->rConn == DRV_TUNER_CT_DIGT)
            {
                TunerDisc(_rTunerCfgTbl.tTunerCfg[0].TunerID, prMsg->rConn);
            } 
            else if(prMsg->rConn == DRV_TUNER_CT_ANAC || prMsg->rConn == DRV_TUNER_CT_ANAT)
            {            
                TunerDisc(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn);
            }                     
            _rTunerState = DRV_TUNER_STATE_DISCONNECTED;                    
            break;                
        case DRV_TUNER_MSG_CONNECT:
        	if(prMsg->rConn == DRV_TUNER_CT_DIGC || prMsg->rConn == DRV_TUNER_CT_DIGT)
        	{
        	
                printf("[TUNER] Set Freq to %d\n", prMsg->rInfo.t_ter_tuner_info.ui4_freq);
#if 0          
                //bApiVideoSetVideoSrc(0, SV_VS_DT1);           
#endif
                
                TunerAcq(_rTunerCfgTbl.tTunerCfg[0].TunerID, prMsg->rConn,
                              (void*)&(prMsg->rInfo),
                              sizeof(prMsg->rInfo),
                              _TunerBreaker, NULL);                

                _fgTunerLockStatus = _GetLockStatus(prMsg, 10);                         

#ifdef CC_MT5391_SLT
                if(_fgTunerLockStatus)
                {
                    UINT32 u4Lvl = 0;
                    UINT32 i;
                    for(i = 0; i < 10; i++)
                    {                        
                        u4Lvl += TunerGetSignalSNR(_rTunerCfgTbl.tTunerCfg[0].TunerID, prMsg->rConn);
                    }
                    u4Lvl /= 10;
                    if(u4Lvl < 30)
                    {
                        PMX_ShowPattern(TRUE);
                    }
                }
                else
                {
                    PMX_ShowPattern(TRUE);
                }
#endif

                if(_rTunerParam.rDigParam.pfnTuningDone != NULL)
                { 
                    UINT8 u1MsgId;
                    SIGNAL rSignal;

                    TunerGetSignal(_rTunerCfgTbl.tTunerCfg[0].TunerID, prMsg->rConn, &rSignal);

                    // Allocate message
                    if (!_AllocateMessage(&u1MsgId))
                    {
                        return;
                    }

                    _arTunerMsg[u1MsgId].rMsg.eType = TUNER_MSG_TYPE_SETFREQ;
                    _arTunerMsg[u1MsgId].rMsg.u1Id = 0;
                    _arTunerMsg[u1MsgId].rMsg.pvNotifyTag = prMsg->pvTag;
                    _arTunerMsg[u1MsgId].rMsg.rData.u4Freq = prMsg->rInfo.t_ter_tuner_info.ui4_freq / 1000;
                    _arTunerMsg[u1MsgId].rMsg.rData.fgLock = _fgTunerLockStatus;
                    _arTunerMsg[u1MsgId].rMsg.rData.QAMSize = rSignal.QAMSize;
                    _arTunerMsg[u1MsgId].rMsg.rData.SymbolRate = rSignal.SymbolRate;
                
                    if (_fgTunerLockStatus)
                    {                    
                        printf("[TUNER] Lock at freq %d\n", prMsg->rInfo.t_ter_tuner_info.ui4_freq);
                        _rTunerParam.rDigParam.pfnTuningDone(u1MsgId);
                    }                         
                    else
                    {
                        printf("[TUNER] Not lock at freq %d\n", prMsg->rInfo.t_ter_tuner_info.ui4_freq);
                        _rTunerParam.rDigParam.pfnTuningDone(u1MsgId);
                    }
        	    }

#if !defined(CC_TS_OUT_SUPPORT) && !defined(CC_ISDBT_SUPPORT)                
    #ifdef CC_DVBT_SUPPORT
                  PCMCIA_AutoSetTsRoutingPath();
    #else
                  DMX_SetFrontEnd(DMX_FE_ATSC_DEMOD);
    #endif
#else
#if defined(CC_MT5365) || defined(CC_MT5395)
                  DMX_SetFrontEnd(DMX_FE_EXT_SERIAL0);
#else
                  DMX_SetFrontEnd(DMX_FE_EXT_SERIAL);
#endif
#endif

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
                DMX_SetFrontEnd(DMX_FE_INTERNAL);
#elif defined(CC_MT5389)
    #ifdef CC_EXTERNAL_DEMOD
        DMX_SetFrontEndEx(0, DMX_FE_EXT_2BIT);
    #else
        DMX_SetFrontEnd(DMX_FE_INTERNAL);
    #endif
#endif
                _rTunerState = DRV_TUNER_STATE_DIG_CONNECTED;    
        	}
        	else if(prMsg->rConn == DRV_TUNER_CT_ANAC || prMsg->rConn == DRV_TUNER_CT_ANAT)
        	{
                 BOOL fgTunerLock;
                 UINT8 u1TunerStatus;
                 UINT32 u4Freq;                 
#ifndef CC_DVBT_SUPPORT
#if TUNER_USE_UTDIS // Use UTDIS
                SIGNAL rSignal;

                UNUSED(u1TunerStatus);
                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 
                fgTunerLock = TunerAcq(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn,
                (void*)&(prMsg->rInfo),
                sizeof(prMsg->rInfo),
                _TunerBreaker, NULL);   

                TunerGetSignal(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn, &rSignal);
                u4Freq = rSignal.Frequency /1000;
#else
                 ATV_PI_CTX_T* prAtvCtx;

                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 

                prAtvCtx = TunerGetCtx(_rTunerCfgTbl.tTunerCfg[1].TunerID);

                u4Freq = prMsg->rInfo.t_tana_tuner_info.ui4_freq / 1000;
                
                 DigTunerDetachI2C(TRUE);

                 AnalogPINA_TunerSetSystem(prAtvCtx, SYS_SUB_NTSC_M); 
                 
                 AnalogPINA_TunerSetFreq(
                 	prAtvCtx,
                 	u4Freq,
                  MOD_ANA_CABLE);

                 u1TunerStatus = AnalogPINA_TunerSearchNearbyFreq(
                 	    prAtvCtx,
                 	    &u4Freq,
                 	    _TunerBreaker,
                 	    NULL);
                 
                 if (u1TunerStatus == 3) // VIF/TVD both lock
                 {
                     fgTunerLock = TRUE;
                 }
                 else
                 {
                     fgTunerLock = FALSE;
                 }	
                 
                 #if 0
                 fgTunerLock = AnalogPINA_TunerCheckVIFLock(prAtvCtx, u4Freq);
                 #endif
 
                 DigTunerDetachI2C(FALSE);        
#endif /* Use UTDIS */ 
                
#else
#if TUNER_USE_UTDIS // Use UTDIS
                SIGNAL rSignal;

                UNUSED(u1TunerStatus);
                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 
                fgTunerLock = TunerAcq(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn,
                (void*)&(prMsg->rInfo),
                sizeof(prMsg->rInfo),
                _TunerBreaker, NULL);   

                TunerGetSignal(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn, &rSignal);
                u4Freq = rSignal.Frequency /1000;
#else
                 ATV_PI_CTX_T* prAtvCtx;

                UNUSED(u1TunerStatus);
                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 

                prAtvCtx = TunerGetCtx(_rTunerCfgTbl.tTunerCfg[1].TunerID);

                u4Freq = prMsg->rInfo.t_tana_tuner_info.ui4_freq / 1000;

                if(prMsg->rInfo.t_tana_tuner_info.e_mod == MOD_UNKNOWN)
                {
                     DigTunerDetachI2C(TRUE);
                     // DVBT_TunerDisc();
                     TunerDisc(_rTunerCfgTbl.tTunerCfg[0].TunerID, TUNER_CONN_TYPE_TER_DIG);

                     AnalogPIEU_TunerSetSystem(prAtvCtx, _Mask2TvSys(prMsg->rInfo.t_cana_tuner_info.ui4_tv_sys_mask));

                     AnalogPIEU_TunerSetFreq(
                     	prAtvCtx,
                     	u4Freq,
                        0);
                     
                     AnalogPIEU_TunerSearchNearbyFreq(
                     	prAtvCtx,
                     	&u4Freq,
                     	_TunerBreaker,
                     	NULL,
                     	u4Freq + 250);
     
                     fgTunerLock = AnalogPIEU_TunerCheckVIFLock(prAtvCtx);
     
                     DigTunerDetachI2C(FALSE);
                }
                else
                {
                     DigTunerDetachI2C(TRUE);
                     
                     AnalogPIEU_TunerSetFreq(
                     	prAtvCtx,
                     	u4Freq,
                            0);
     
                     fgTunerLock = AnalogPIEU_TunerCheckVIFLock(prAtvCtx);
     
                     DigTunerDetachI2C(FALSE);                
                }
#endif // TUNER_USE_UTDIS                
#endif                
                
                _rTunerState = DRV_TUNER_STATE_ANA_CONNECTED; 
                
                if(_rTunerParam.rAnaParam.pfnTuningDone != NULL)
                {         
                    BOOL fgLockStatus;
                    //fgLockStatus = _GetLockStatus(prMsg, 10);

                    UINT8 u1MsgId;
                    // Allocate message
                    if (!_AllocateMessage(&u1MsgId))
                    {
                        return;
                    }


                    _arTunerMsg[u1MsgId].rMsg.eType = TUNER_MSG_TYPE_SETFREQ;
                    _arTunerMsg[u1MsgId].rMsg.u1Id = 0;
                    _arTunerMsg[u1MsgId].rMsg.pvNotifyTag = prMsg->pvTag;
                    _arTunerMsg[u1MsgId].rMsg.rData.u4Freq = u4Freq;
                    
                    if(fgTunerLock)
                    {
                        fgLockStatus = fgDrvTvdCheckTVDLock(20);
                    }
                    else
                    {
                        fgLockStatus = FALSE;
                    }

                    _arTunerMsg[u1MsgId].rMsg.rData.fgLock = fgLockStatus;
                    
                    if(fgLockStatus)
                    {                                          
                        _rTunerParam.rAnaParam.pfnTuningDone(u1MsgId);
                    }
                    else
                    {
                        _rTunerParam.rAnaParam.pfnTuningDone(u1MsgId);
                    }
                }                                              

        	}
        	else
        	{
        	    
        	}                	
             break;
        case DRV_TUNER_MSG_SEARCH:
            {
                 BOOL fgTunerLock;
                 UINT8 u1TunerStatus;
                 UINT32 u4Freq;      
                 UINT32 u4FreqBound;
#ifndef CC_DVBT_SUPPORT

#if TUNER_USE_UTDIS // Use UTDIS
                SIGNAL rSignal;

                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 
                fgTunerLock = TunerAcq(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn,
                (void*)&(prMsg->rInfo),
                sizeof(prMsg->rInfo),
                _TunerBreaker, NULL);    

                TunerGetSignal(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn, &rSignal);
                u4Freq = rSignal.Frequency /1000;
                u4FreqBound = rSignal.Frequency /1000;
#else
                ATV_PI_CTX_T* prAtvCtx;                

                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 
                prAtvCtx = TunerGetCtx(_rTunerCfgTbl.tTunerCfg[1].TunerID);

                u4Freq = prMsg->rInfo.t_tana_tuner_info.ui4_freq / 1000;
                u4FreqBound = prMsg->rInfo.t_tana_tuner_info.ui4_freq_bound / 1000;

                 DigTunerDetachI2C(TRUE);

                 AnalogPINA_TunerSetSystem(prAtvCtx, SYS_SUB_NTSC_M); 

                 AnalogPINA_TunerSetFreq(
                 	prAtvCtx,
                 	u4Freq,
                    MOD_ANA_CABLE);
                 
                 u1TunerStatus = AnalogPINA_TunerSearchNearbyFreq(
                 	    prAtvCtx,
                 	    &u4Freq,
                 	    _TunerBreaker,
                 	    NULL);
                 
                 if (u1TunerStatus == 3) // VIF/TVD both lock
                 {
                     fgTunerLock = TRUE;
                 }
                 else
                 {
                     fgTunerLock = FALSE;
                 }	
                 
                 #if 0
                 fgTunerLock = AnalogPINA_TunerCheckVIFLock(prAtvCtx, u4Freq);
                 #endif
 
                 DigTunerDetachI2C(FALSE);
#endif /* Use UTDIS */ 
                 
#else
#if TUNER_USE_UTDIS // Use UTDIS
                SIGNAL rSignal;
                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 
                fgTunerLock = TunerAcq(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn,
                (void*)&(prMsg->rInfo),
                sizeof(prMsg->rInfo),
                _TunerBreaker, NULL);    

                TunerGetSignal(_rTunerCfgTbl.tTunerCfg[1].TunerID, prMsg->rConn, &rSignal);
                u4Freq = rSignal.Frequency /1000;
                u4FreqBound = rSignal.Frequency /1000;
#else
                 ATV_PI_CTX_T* prAtvCtx;                

                //bApiVideoSetVideoSrc(0, SV_VS_TUNER1); 

                prAtvCtx = TunerGetCtx(_rTunerCfgTbl.tTunerCfg[1].TunerID);

                u4Freq = prMsg->rInfo.t_tana_tuner_info.ui4_freq / 1000;
                u4FreqBound = prMsg->rInfo.t_tana_tuner_info.ui4_freq_bound / 1000;

                 DigTunerDetachI2C(TRUE);

                 AnalogPIEU_TunerSetSystem(prAtvCtx, _Mask2TvSys(prMsg->rInfo.t_tana_tuner_info.ui4_tv_sys_mask)); 

                 AnalogPIEU_TunerSetFreq(
                 	prAtvCtx,
                 	u4Freq,
                        0);
                 
                 AnalogPIEU_TunerSearchNearbyFreq(
                 	prAtvCtx,
                 	&u4Freq,
                 	_TunerBreaker,
                 	NULL,
                 	u4FreqBound);
 
                 fgTunerLock = AnalogPIEU_TunerCheckVIFLock(prAtvCtx);
 
                 DigTunerDetachI2C(FALSE);
#endif // TUNER_USE_UTDIS
#endif // CC_DVBT_SUPPORT               
                UNUSED(u1TunerStatus);
                UNUSED(u4FreqBound);

                _rTunerState = DRV_TUNER_STATE_ANA_CONNECTED; 
                
                if(_rTunerParam.rAnaParam.pfnTuningDone != NULL)
                {         
                    BOOL fgLockStatus;
                    UINT8 u1Format;
                    
                    //fgLockStatus = _GetLockStatus(prMsg, 10);

                    UINT8 u1MsgId;
                    // Allocate message
                    if (!_AllocateMessage(&u1MsgId))
                    {
                        return;
                    }


                    _arTunerMsg[u1MsgId].rMsg.eType = TUNER_MSG_TYPE_SEARCH;
                    _arTunerMsg[u1MsgId].rMsg.u1Id = 0;
                    _arTunerMsg[u1MsgId].rMsg.pvNotifyTag = prMsg->pvTag;
                    _arTunerMsg[u1MsgId].rMsg.rData.u4Freq = u4Freq;
                        
                    if(fgTunerLock)
                    {
                        fgLockStatus = fgDrvTvdCheckTVDLock(20);
                        u1Format = bTvd3dGetColorSystem();
                    }
                    else
                    {
                        fgLockStatus = FALSE;
                    }

                    _arTunerMsg[u1MsgId].rMsg.rData.fgLock = fgLockStatus;
                    _arTunerMsg[u1MsgId].rMsg.rData.u1Format = u1Format;
                        
                    if(fgLockStatus)
                    {                                          
                        _rTunerParam.rAnaParam.pfnTuningDone(u1MsgId);
                    }
                    else
                    {
                        _rTunerParam.rAnaParam.pfnTuningDone(u1MsgId);
                    }
                }                                              
        	}           	
 
             break;   
    	     
        case DRV_TUNER_MSG_KILL:
             _rTunerState = DRV_TUNER_STATE_NOT_INIT;
             break;
        default:
        	VERIFY(0);
    }
}

static void _TunerThread(VOID *pvArg)
{
    UINT16 u2MsgIdx;
    SIZE_T u4MsgSize;
    _TUNER_EVENT_T rMsg;
    INT32 i4Ret;

    UNUSED(i4Ret);
    
    while (_rTunerState != DRV_TUNER_STATE_NOT_INIT)
    {

        u4MsgSize = sizeof(rMsg);

        /* If there's no message in _hTunerMsgq, polling the sync status of tuner if tuner is in connected state */        
        if(_rTunerState == DRV_TUNER_STATE_DIG_CONNECTED && _rTunerParam.rDigParam.pfnLockStatusChange != NULL)
        {
            if( (i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&rMsg, &u4MsgSize, &_hTunerMsgq, 1, X_MSGQ_OPTION_NOWAIT)) == OSR_NO_MSG)
            {
    
                BOOL fgLockStatus = _GetLockStatus(&rMsg, 5);
                
                /* Not provided for the simplicity */           
                rMsg.rConn = _rCurrConnType;
                if(fgLockStatus != _fgTunerLockStatus)
                {
                    UINT8 u1MsgId;

                    // Allocate message
                    if (!_AllocateMessage(&u1MsgId))
                    {
                        return;
                    }
                    
                    _arTunerMsg[u1MsgId].rMsg.eType = TUNER_MSG_TYPE_SETFREQ;
                    _arTunerMsg[u1MsgId].rMsg.u1Id = 0;
                    _arTunerMsg[u1MsgId].rMsg.pvNotifyTag = rMsg.pvTag;
                    _arTunerMsg[u1MsgId].rMsg.rData.u4Freq = rMsg.rInfo.t_ter_tuner_info.ui4_freq / 1000;
                    _arTunerMsg[u1MsgId].rMsg.rData.fgLock = fgLockStatus;
                
                    _fgTunerLockStatus = fgLockStatus;
                    printf("[TUNER] Monitored lock status change to %d\n", fgLockStatus); 
                    if(_rTunerParam.rDigParam.pfnLockStatusChange != NULL)
                    {
                        _rTunerParam.rDigParam.pfnLockStatusChange(u1MsgId);                      
                    }      
                }                                          	
                else if(_rTunerCfgTbl.TunerNum >= 2 && _rTunerState == DRV_TUNER_STATE_ANA_CONNECTED)
                {
                    /* Not provided due to F/W limitation */           
                }
                else
                {
                    /* Nothing */
                }
                x_thread_delay(DRV_TUNER_POLLING_DELAY);
            }
            else
            {
                _RequestHandler(&rMsg);
            }
        }    
        else if((i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&rMsg, &u4MsgSize, &_hTunerMsgq, 1, X_MSGQ_OPTION_WAIT)) == OSR_OK)
        {                     
            _RequestHandler(&rMsg);   
        }
    } 

    x_thread_exit();
}

static UINT32 _TvSys2Mask(UINT32  u4TvSys)
{
    UINT32   u4Mask;

    if (u4TvSys != DRV_TV_SYS_UNKNOWN)
    {
        u4Mask = TV_SYS_MASK_B | TV_SYS_MASK_G;
        if (u4TvSys == (DRV_TV_SYS_B_G))
        {
            u4Mask = TV_SYS_MASK_B | TV_SYS_MASK_G;
        }
        else if (u4TvSys == (DRV_TV_SYS_D_K))
        {
            u4Mask = TV_SYS_MASK_D | TV_SYS_MASK_K;
        }
        else if (u4TvSys == (DRV_TV_SYS_I))
        {
            u4Mask = TV_SYS_MASK_I;
        }
        else if (u4TvSys == (DRV_TV_SYS_L))
        {
            u4Mask = TV_SYS_MASK_L;
        }
        else if (u4TvSys == (DRV_TV_SYS_L1))
        {
            u4Mask = TV_SYS_MASK_L_PRIME;
        }
        return u4Mask;
    }
    return TV_SYS_MASK_NONE;
}

static DRV_ERR_CODE_T _SetFreq(UINT32* pu4Freq, DRV_TUNER_CONN_TYPE_T rType, DRV_TV_SYS_T rTvSys, BOOL fgAutoFineTune, void* pvTag)
{
    //UINT32 u4InfoSize;
    //DRV_COMP_ID_T rCompId;
    DRV_TUNER_ALL_INFO_T rInfo;

    _TUNER_EVENT_T rMsg;

    if(pu4Freq == NULL)
    {
        return DRV_E_INV_ARG;
    }

    /* Disconnect currently enabled connection first */ 
    rMsg.rType = DRV_TUNER_MSG_DISCONNECT;
    rMsg.rConn = _rCurrConnType;            
    x_msg_q_send(_hTunerMsgq, (void*)&rMsg, sizeof(rMsg), 253);

    if(rType == DRV_TUNER_CT_DIGC)    
    {                    
        TUNER_CAB_DIG_TUNE_INFO_T *prInfo = (TUNER_CAB_DIG_TUNE_INFO_T *)&rInfo;

        if(*pu4Freq > DRV_TUNER_DIGC_MAX_FREQ || *pu4Freq < DRV_TUNER_DIGC_MIN_FREQ)
        {
            return DRV_E_INV_ARG;
        }
        
        prInfo->e_mod = (TUNER_MODULATION_T)_rTunerParam.rDigParam.rMod;
        prInfo->ui4_freq = *pu4Freq;
        prInfo->b_no_disc_nfy = FALSE;

        prInfo->ui4_sym_rate = _rTunerParam.rDigParam.u4SymbolRate;        
        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[0].TunerID;        
        //rCompId.e_type = DRVT_TUNER_CAB_DIG;        

        
        //u4InfoSize = sizeof(TUNER_CAB_DIG_TUNE_INFO_T);
    }
    else if(rType == DRV_TUNER_CT_DIGT)
    {
        TUNER_TER_DIG_TUNE_INFO_T *prInfo = (TUNER_TER_DIG_TUNE_INFO_T *)&rInfo;
        
        if(*pu4Freq > DRV_TUNER_DIGT_MAX_FREQ || *pu4Freq < DRV_TUNER_DIGT_MIN_FREQ)
        {
            return DRV_E_INV_ARG;
        }        
        
        prInfo->e_mod = (TUNER_MODULATION_T)_rTunerParam.rDigParam.rMod;
        prInfo->e_bandwidth = (TUNER_BANDWIDTH_T)_rTunerParam.rDigParam.rBandwidth;
        prInfo->ui4_freq = *pu4Freq;
        prInfo->b_no_disc_nfy = FALSE;        

        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[0].TunerID;
        //rCompId.e_type = DRVT_TUNER_TER_DIG;
        
        //u4InfoSize = sizeof(TUNER_TER_DIG_TUNE_INFO_T);        
    }
    else if(rType == DRV_TUNER_CT_ANAC && _rTunerCfgTbl.TunerNum >= 2)
    {
        TUNER_CAB_ANA_TUNE_INFO_T *prInfo = (TUNER_CAB_ANA_TUNE_INFO_T *)&rInfo;
        prInfo->ui4_freq = *pu4Freq;
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)  
        prInfo->ui4_central_freq = *pu4Freq;
#endif
        prInfo->ui4_freq_bound = *pu4Freq + 100000;  
		prInfo->i2_fine_tune_offset=0;
        prInfo->ui4_tv_sys_mask = _TvSys2Mask(rTvSys);
		prInfo->b_fine_tune=TRUE;
        prInfo->b_no_disc_nfy = FALSE;
        if(fgAutoFineTune)
        {
            prInfo->e_mod = MOD_QAM_1024;
        }
        else
        {
            prInfo->e_mod = MOD_PSK_8;
        }        
        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[1].TunerID;        
        //rCompId.e_type = DRVT_TUNER_CAB_ANA;
        
        //u4InfoSize = sizeof(TUNER_CAB_ANA_TUNE_INFO_T);
    }
    else if(rType == DRV_TUNER_CT_ANAT && _rTunerCfgTbl.TunerNum >= 2)
    {
        TUNER_TER_ANA_TUNE_INFO_T *prInfo = (TUNER_TER_ANA_TUNE_INFO_T *)&rInfo;
        prInfo->ui4_freq = *pu4Freq;
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)   
        prInfo->ui4_central_freq = *pu4Freq;
#endif        
        prInfo->ui4_freq_bound = *pu4Freq + 100000;
		prInfo->i2_fine_tune_offset=0;
        prInfo->ui4_tv_sys_mask = _TvSys2Mask(rTvSys);
        //prInfo->b_fine_tune = fgAutoFineTune;
        prInfo->b_fine_tune = TRUE; // EU
        prInfo->b_no_disc_nfy = FALSE;
        if(fgAutoFineTune)
        {
            prInfo->e_mod = MOD_QAM_1024;
        }
        else
        {
            prInfo->e_mod = MOD_PSK_8; 
        }
        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[1].TunerID;
        //rCompId.e_type = DRVT_TUNER_TER_ANA;
        
        //u4InfoSize = sizeof(TUNER_TER_ANA_TUNE_INFO_T);        
    }
    else
    {
        return DRV_E_FAIL;
    }
    rMsg.rType = DRV_TUNER_MSG_CONNECT;
    rMsg.rInfo = rInfo;
    rMsg.rConn = rType;
    rMsg.pvTag = pvTag;
    _rCurrConnType = rType;     
    vSetAutoSearchType(AUTOSEARCH_CHANGE_CHANNEL);

    x_msg_q_send(_hTunerMsgq, (void*)&rMsg, sizeof(rMsg), 253);

    return DRV_E_OK;    
}


static DRV_ERR_CODE_T _Search(UINT32* pu4Freq, DRV_TUNER_CONN_TYPE_T rType, DRV_TV_SYS_T rTvSys, UINT32 u4EndFreq, void* pvTag)
{
    //UINT32 u4InfoSize;
    //DRV_COMP_ID_T rCompId;
    DRV_TUNER_ALL_INFO_T rInfo;

    _TUNER_EVENT_T rMsg;

    if(pu4Freq == NULL)
    {
        return DRV_E_INV_ARG;
    }

    /* Disconnect currently enabled connection first */ 
    rMsg.rType = DRV_TUNER_MSG_DISCONNECT;
    rMsg.rConn = _rCurrConnType;            
    x_msg_q_send(_hTunerMsgq, (void*)&rMsg, sizeof(rMsg), 253);

    if(rType == DRV_TUNER_CT_ANAC && _rTunerCfgTbl.TunerNum >= 2)
    {
        TUNER_CAB_ANA_TUNE_INFO_T *prInfo = (TUNER_CAB_ANA_TUNE_INFO_T *)&rInfo;
        prInfo->ui4_freq = *pu4Freq;
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)      
        prInfo->ui4_central_freq = *pu4Freq;
#endif
        prInfo->ui4_freq_bound = u4EndFreq;        
        prInfo->ui4_tv_sys_mask = _TvSys2Mask(rTvSys);
        prInfo->b_no_disc_nfy = FALSE;

        prInfo->e_mod = MOD_UNKNOWN;
 
        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[1].TunerID;        
        //rCompId.e_type = DRVT_TUNER_CAB_ANA;
        
        //u4InfoSize = sizeof(TUNER_CAB_ANA_TUNE_INFO_T);
    }
    else if(rType == DRV_TUNER_CT_ANAT && _rTunerCfgTbl.TunerNum >= 2)
    {
        TUNER_TER_ANA_TUNE_INFO_T *prInfo = (TUNER_TER_ANA_TUNE_INFO_T *)&rInfo;
        prInfo->ui4_freq = *pu4Freq;
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)     
        prInfo->ui4_central_freq = *pu4Freq; 
#endif
        prInfo->ui4_freq_bound = u4EndFreq;
        prInfo->ui4_tv_sys_mask = _TvSys2Mask(rTvSys);
        prInfo->b_fine_tune = TRUE; 
        prInfo->b_no_disc_nfy = FALSE;

        prInfo->e_mod = MOD_UNKNOWN;

        //rCompId.ui2_id = _rTunerCfgTbl.tTunerCfg[1].TunerID;
        //rCompId.e_type = DRVT_TUNER_TER_ANA;
        
        //u4InfoSize = sizeof(TUNER_TER_ANA_TUNE_INFO_T);        
    }
    else
    {
        return DRV_E_FAIL;
    }
    rMsg.rType = DRV_TUNER_MSG_SEARCH;
    rMsg.rInfo = rInfo;
    rMsg.rConn = rType;
    rMsg.pvTag = pvTag;
    _rCurrConnType = rType;     
    vSetAutoSearchType(AUTOSEARCH_SCAN);

    x_msg_q_send(_hTunerMsgq, (void*)&rMsg, sizeof(rMsg), 253);

    return DRV_E_OK;    
}

static void _SetBreak()
{
    _fgTunerBreak = TRUE;
}

EXTERN INT32 NAV_DrawInfoOSD(void); // FIXME: temporary for CI callback test
EXTERN BOOL fgNavAvInit;

// Callback function for CI driver
static void _CamConnected(BOOL fgCamConnected) 
{
    // set status variable
    _fgCamConnected = fgCamConnected;
    if(fgNavAvInit)
    {
        // refresh UI
        NAV_DrawInfoOSD(); // FIXME: temporary for CI callback test
    }
    return;
}


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Exported functionss
//-----------------------------------------------------------------------------

DRV_ERR_CODE_T DRV_TUNER_Init(void)
{
    DRV_TUNER_MUTEX_INIT

    DRV_TUNER_MUTEX_LOCK


    x_memset(&_rTunerParam, 0, sizeof(DRV_TUNER_PARAM_T));
    x_memset(&_rTunerInterParam, 0, sizeof(DRV_TUNER_PARAM_T));

    _rTunerState = DRV_TUNER_STATE_DISCONNECTED;
    VERIFY(x_msg_q_create(&_hTunerMsgq, "M_TUNER", sizeof(_TUNER_EVENT_T), 64) == OSR_OK);
    VERIFY(x_thread_create(&_hTunerThread, "T_TUNER", 2048, 253, _TunerThread, 0, NULL) == OSR_OK); 

    //Tuner_SetAnaNfy(_ANA_SigNotify);

    VERIFY(DMX_Init());
/*
#ifndef CC_TS_OUT_SUPPORT
    #ifdef CC_DVBT_SUPPORT
                  DMX_SetFrontEnd(DMX_FE_DVB_DEMOD);
    #else
                  DMX_SetFrontEnd(DMX_FE_ATSC_DEMOD);
    #endif
#else
#if defined(CC_MT5365) || defined(CC_MT5395) 
                  DMX_SetFrontEnd(DMX_FE_EXT_SERIAL0);
#else
                  DMX_SetFrontEnd(DMX_FE_EXT_SERIAL);
#endif
#endif
*/
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
                DMX_SetFrontEnd(DMX_FE_INTERNAL);
#elif defined(CC_MT5389)
    #ifdef CC_EXTERNAL_DEMOD
        DMX_SetFrontEndEx(0, DMX_FE_EXT_2BIT);
    #else
        DMX_SetFrontEnd(DMX_FE_INTERNAL);
    #endif

#endif

    TunerOpen();
    TDIS_RetrieveTunerCfgTbl(&_rTunerCfgTbl);

#ifdef CC_TS_OUT_SUPPORT                
    CLI_Parser("nim.hc t 1");
#endif

    PCMCIA_Demo((pcmcia_cam_connectivity_nfy)_CamConnected);

    DRV_TUNER_MUTEX_UNLOCK

    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TUNER_SetParameter(DRV_TUNER_PARAM_T* prParam)
{
    DRV_TUNER_MUTEX_LOCK    

    x_memcpy(&_rTunerInterParam, prParam, sizeof(DRV_TUNER_PARAM_T));

    /*
    _rTunerParam.rAnaParam.fgAfcEnabled = prParam->rAnaParam.fgAfcEnabled;
    _rTunerParam.rAnaParam.pfnTuningDone = prParam->rAnaParam.pfnTuningDone;
    _rTunerParam.rAnaParam.rTvSys = prParam->rAnaParam.rTvSys;

    _rTunerParam.rDigParam.fgSpectrumInversion = prParam->rDigParam.fgSpectrumInversion;
    _rTunerParam.rDigParam.pfnTuningDone = prParam->rDigParam.pfnTuningDone;
    _rTunerParam.rDigParam.pfnLockStatusChange = prParam->rDigParam.pfnLockStatusChange;
    _rTunerParam.rDigParam.rBandwidth = prParam->rDigParam.rBandwidth;
    _rTunerParam.rDigParam.rMod = prParam->rDigParam.rMod;
    _rTunerParam.rDigParam.u4QamSize = prParam->rDigParam.u4QamSize;
    _rTunerParam.rDigParam.u4SymbolRate = prParam->rDigParam.u4SymbolRate;    
    */
    
    DRV_TUNER_MUTEX_UNLOCK        

    return DRV_E_OK;    
}

DRV_ERR_CODE_T DRV_TUNER_SetFreq(UINT32* pu4Freq, DRV_TUNER_CONN_TYPE_T rType, DRV_TV_SYS_T rTvSys, BOOL fgAutoFineTune, void* pvTag)
{
    DRV_ERR_CODE_T rRetCode = DRV_E_FAIL;    
    
    DRV_TUNER_MUTEX_LOCK    

    x_memcpy(&_rTunerParam, &_rTunerInterParam, sizeof(DRV_TUNER_PARAM_T));    
    
    rRetCode = _SetFreq(pu4Freq, rType, rTvSys, fgAutoFineTune, pvTag);
    
    DRV_TUNER_MUTEX_UNLOCK
    
    return rRetCode;
}

DRV_ERR_CODE_T DRV_TUNER_Search(UINT32* pu4Freq, DRV_TUNER_CONN_TYPE_T rType, DRV_TV_SYS_T rTvSys, UINT32 u4EndFreq, void* pvTag)
{
    DRV_ERR_CODE_T rRetCode = DRV_E_FAIL;    
    
    DRV_TUNER_MUTEX_LOCK    

    x_memcpy(&_rTunerParam, &_rTunerInterParam, sizeof(DRV_TUNER_PARAM_T));    
    
    rRetCode = _Search(pu4Freq, rType, rTvSys, u4EndFreq, pvTag);
    
    DRV_TUNER_MUTEX_UNLOCK
    
    return rRetCode;
}

DRV_ERR_CODE_T DRV_TUNER_GetFreq(UINT32* pu4Freq, INT32 i4Tag)
{
    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TUNER_GetInfo(DRV_TUNER_INFO_T* prInfo)
{
    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TUNER_GetMessage(UINT8 u1MsgId, DRV_TUNER_MSG_T* prMsg)
{
    ASSERT(prMsg != NULL);

    // Check if message is in-use
    if (u1MsgId >= NUM_MESSAGE)
    {
        return DRV_E_INV_ARG;
    }
    if (!_arTunerMsg[u1MsgId].fgInUse)
    {
        return DRV_E_NOT_EXIST;
    }

    // Copy message
    *prMsg = _arTunerMsg[u1MsgId].rMsg;

    // Release message
    if (_ReleaseMessage(u1MsgId))
    {
        return DRV_E_FAIL;
    }

    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TUNER_SetBreak()
{
    _SetBreak();
    return DRV_E_OK;    
}

BOOL DRV_TUNER_IsCamConnected()
{    
    return _fgCamConnected;    
}

