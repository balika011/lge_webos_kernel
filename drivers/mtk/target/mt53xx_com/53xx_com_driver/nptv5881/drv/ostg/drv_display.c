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
 * $RCSfile: drv_display.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_display.c
 *  Brief of file drv_display.c.
 *  Details of file drv_display.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif
#include "general.h"
#include "hw_vdoin.h"
#include "drv_vdoclk.h"
#include "util.h"
#include "hw_scpos.h"
#include "hw_mlvds.h"
#include "hw_tcon.h"
#include "hw_tg.h"
#include "hw_ycproc.h"
#include "hw_ospe.h"
#include "drv_scpos.h"
#include "drv_display.h"
#include "drv_scaler.h"
#include "drv_ycproc.h"
#include "drv_lvds.h"
#include "drv_di.h"
#include "vdo_misc.h"
#include "drv_hdtv.h"
#include "video_def.h"
#include "drv_video.h"
#include "drv_tcon.h"
#if defined(CC_MT5398)
#include "drv_lcdim_v2.h"
#else
#include "drv_lcdim.h"
#endif
#include "nptv_debug.h"
#include "api_backlt.h"
#include "mute_if.h"
#include "srm_drvif.h"
#include "drv_ldmspi.h"
#ifndef CC_COPLAT_MT82
#include "osd_drvif.h"
#include "pmx_drvif.h"
#endif
#include "x_assert.h"
#include "x_lint.h"
#include "x_pinmux.h"
#include "c_model.h"
#include "x_timer.h"
#include "ostg_if.h"
#include "drv_od.h"	
#ifdef THE_3RD_PARTY_HEADER_FILE_1
#include THE_3RD_PARTY_HEADER_FILE_1
#endif
#ifdef CC_DRV_SUPPORT_SMICRO
#include "SMicro/SMicro_if.h"
#endif
#include "x_gpio.h"
#ifdef CC_MT5398
#include "drv_mjc.h"
#include "drv_mjc_if.h"
#endif
#ifdef CC_SCPOS_3DTV_SUPPORT
#include "drv_tdtv.h"
#endif

#include "drv_pwm.h"
#include "drv_vdoclk.h"
#include "drv_scaler_drvif.h"
#include "drvcust_if.h"  //mtk70763, 2010-7-21
#ifdef DRV_SUPPORT_EXTMJC
#include "extmjc_if.h"
#endif
#include "drv_vbyone.h"
#include "eeprom_if.h"
#include "osd_drvif.h"
#include "drv_scaler_psc.h"
#include "drv_mon_out.h"
//luis@2007/7/5, for vIO32WriteFldXXX
/*lint -save -e666*/

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define CC_BACKPORCH_MIN (99 + 15)
#define CC_FRONTPORCH_MIN (10)
#define CC_BACKPORCH_MAX (2047)
#define CC_FRONTPORCH_MAX (30)
#define OSTG_UPDATE_PIXEL_SHIFT_H 1
#define OSTG_UPDATE_PIXEL_SHIFT_V 2

// force the HTotal to be 4x-aligned in Dispmode + Dual or Four port panel
#define CC_HTOTAL_4X_LIMIT_WA

// when VOUT_H is smaller than HTotal, VSync will be lost.
#define CC_OSTG_WA1

#if CC_OSD_MUTE_WA
UINT8 _bDddsDelayFlag = 0 ;
#endif

UINT32 _u4NSDHTotal=0xff;
UINT32 _u4NSDVTotal=0xff;
UINT32 _u4NSDFrameRate=0xff;

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

#ifdef CC_UBOOT
UINT32 _u4VPllFrequency;
#else
extern UINT32 _u4VPllFrequency;
#endif

//-----------------------------------------------------------------------------
// Public variables
//-----------------------------------------------------------------------------

//UINT32 dwSI_DISPLAY_DCLK_TYPICAL = 0x00;
//UINT32 dwSI_DISPLAY_DCLK_50HZ;
//#ifdef SUPPORT_PANEL_48HZ
//UINT32 dwSI_DISPLAY_DCLK_48HZ;
//#endif

//Give initial value instead of using "CC_DVBT_SUPPORT" define
UINT8 bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
UINT8 bOUTPUT_REFRESH_RATE = 0;
UINT8 bFORCE_FRAMERATE_TYPE  = ~0;

UINT8 bDISPLAY_INTERLACE_ENABLE;
BOOL _fgFRS_ENA;
enum eSyncLockMode _eSyncLockMode = eSYNCLOCK_IGNORED;

#define ERRORLIMIT_INI 0x1F
UINT32 _u4ErrorLimit = ERRORLIMIT_INI;

static UINT32 _u4OstgUpdateEvent = 0;

#ifdef SUPPORT_SW_FRAME_CHECK
UINT32 _u4VDistUpperBound = 0x800 + 2;      // target in-out VSync distance
UINT32 _u4VDistLowerBound = 0x800;      // target in-out VSync distance
UINT32 _u4VLossDistTorance = 15;
UINT32 _u4VDistTorance = 1;
UINT32 _u4VDistTrackState = 0;          // 1: down, 2: up, 3:locked
UINT32 _u4UpTrackRange = 0;
UINT32 _u4DownTrackRange = 0;
UINT32 _u4VDistTrackMaxStep = 3;          
UINT32 _u4VDistTrackEn = 0;  
UINT8 Threadhold = 20;
UINT8 _u4VDistTrackLocked = 0;
#endif

#ifdef SUPPORT_CW_CHANGE_BY_STEP
UINT32 _u4TargetCW = 0;
UINT32 _u4CurrentCW = 0;
UINT32 _u4CWStep = 0x500000;
#endif

#ifdef SUPPORT_DDDS_STEP_TRACKING
UINT32 _u4CenterCW= 0;
UINT16 _u2DDDSStepTrackingEn = 0;
UINT32 _u4DDDSTrackingErrorLimit = 2;        
UINT32 _u4StepInCWTolerance = 0x1000;  
UINT32 _u4CurrentStepCW = 0;

UINT32 _u4StepInCWUpperBound = 0;
UINT32 _u4StepInCWLowerBound = 0;
UINT32 _u4DefaultErrorLimit = 0xf;
#endif

#ifdef CC_MT5398
static UINT16 _u2IntFrameRate = 0;
BOOL bForceMJCMute = SV_FALSE;
#endif

UINT8 _bForceFrameRate = 0;

UINT8 bSupportLocalDimming = SV_FALSE;

UINT16 w3DGlassesDelay = 0;

UINT8 fgApplyScalerSetData=0;
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifndef CC_MTK_LOADER
#ifdef SUPPORT_DDDS_STEP_TRACKING
static void vDrvDDDSTrackingNormProc(void);
#endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
static INT32 i4OrigControlWord = 0x7fffffff;
#define STEP_CW (4)
#define STEP_CW_DELAY (50)
#define STEP_CW_DELAY_EPI (10)
#define STEP_CW_DELAY2 (250)
static UINT8 u1StepControlWord = 0;
static UINT8 u1StepDelay = 0;
static UINT8 u1StepDelay2 = 0;
static UINT8 u1DDDSLockStatus = 0;
static UINT8 u1SignalStatus = SV_VDO_NOSIGNAL;
UINT32 TargetErrlimit = ERRORLIMIT_INI;
#endif
extern UINT32 _u4FgDDDSTrackHalfStep;

#ifdef CC_SCPOS_3DTV_SUPPORT 
static void vDrvDisp3DModeCB(void);
static void vSavePanelAttribute(void);
#ifdef DRV_SUPPORT_MT8283
BOOL fgForceChangeRegion = FALSE;
#endif
#endif

#endif

typedef struct
{
	UINT32 u4HTotal;
	UINT32 u4HActive;
	UINT32 u4HFP;
	UINT32 u4HBP;
	UINT32 u4VTotal;
	UINT32 u4VActive;
	UINT32 u4VFP;
	UINT32 u4VBP;
}LCDTIMING_INFO_T;

LCDTIMING_INFO_T lcdtimingInfo;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
UINT16 wDrvGetOutputHTotal(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4HTotal);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
			return ((UINT16) u4PscGetPanelHTotal());
	    }
		else
		{
			return (UINT16)u4ScpipGetPanelHtotal();
		}
	}
}

void wDrvSetOutputHTotal(UINT16 u2HTotal)
{
	#ifdef CC_MT5398
    UINT16 u2OriHTotal;
    #endif 
    if (fgIsMJCToOSTG())
    {
        #ifdef CC_MT5398
        u2OriHTotal = u4PscGetPanelHTotal();
        if(u2OriHTotal != u2HTotal)
        {
            bForceMJCMute = SV_TRUE;
            #ifndef CC_MTK_LOADER             
                _vDrvVideoSetMute(MUTE_MODULE_MJC, SV_VP_MAIN, 5, SV_TRUE);
		#endif
        }
        #endif  
        vPscSetPanelHTotal(u2HTotal);
    }
    else
    {
        vScpipSetPanelHtotal(u2HTotal);
    }
#ifndef CC_MTK_LOADER    
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

static UINT16 wDrvGetOutputHTotalStatus(void)
{
    if (fgIsMJCToOSTG())
    {
	    return ((UINT16) u4PscGetOutputHTotalStatus());
    }
	else
	{
        return ((UINT16) u4ScpipGetOutputHTotalStatus(0));
	}
}

UINT16 wDrvGetOutputVTotal(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4VTotal);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
		    return ((UINT16) u4PscGetPanelVTotal());
	    }
		else
		{
	        return ((UINT16) u4ScpipGetPanelVtotal());
		}
	}
}

static UINT16 wDrvGetOutputVTotalStatus(void)
{
    if (fgIsMJCToOSTG())
    {
	    return ((UINT16) u4PscGetOutputVTotalStatus());
    }
	else
	{
        return ((UINT16) u4ScpipGetOutputVTotalStatus(0));
	}
}

void wDrvSetOutputVTotal(UINT16 u2VToal)
{

    if (fgIsMJCToOSTG())
    {
	    vPscSetPanelVTotal(u2VToal);
	
    }
	else
	{
	    vScpipSetPanelVtotal(u2VToal);
	}

	if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)||(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
 	{
       vIO32WriteFldAlign(PIXEL_REORDER_15,1,RG_VWIDTH_R);
	   vIO32WriteFldAlign(PIXEL_REORDER_01,1,RG_VS_OUT_POL);
	   vIO32WriteFldAlign(PIXEL_REORDER_16,8,RG_VDE_VSTART_R);
	
       vIO32WriteFldAlign(VINTF_B3, 8, VINTF_PTGEN_V_START);
       vIO32WriteFldAlign(MUTE_PATTERN_05, 8, MUTE_V_START);
	   vIO32WriteFldAlign(EH_13,8,EH_PTGEN_V_START);	  
	}

#ifndef CC_MTK_LOADER    
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

void wDrvSetOutputHActive(UINT16 u2HActive)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelWidth(u2HActive);
    }
    else
    {
        vScpipSetPanelWidth(u2HActive);
    }
    
#ifndef CC_MTK_LOADER    
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

UINT16 wDrvGetOutputHActive(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4HActive);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
		    return ((UINT16) u4PscGetPanelWidth());
	    }
		else
		{
	        return ((UINT16) u4ScpipGetPanelWidth());
		}
	}
}


void wDrvSetOutputVActive(UINT16 u2VActive)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelHeight(u2VActive);
    }
    else
    {
        vScpipSetPanelHeight(u2VActive);
    }

#ifndef CC_MTK_LOADER    
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

UINT16 wDrvGetOutputVActive(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4VActive);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
		    return ((UINT16) u4PscGetPanelHeight());
	    }
		else
		{
	        return ((UINT16) u4ScpipGetPanelHeight());
		}
	}
}

#define ODFPLimit 4
#define REORDERBPLimit 4
void vDrvSetHsyncFp(UINT16 u2Fp) 
{
    if (u2Fp <= ODFPLimit)
    {
        u2Fp = ODFPLimit;
    }

    if(LVDS_OUTPUT_PORT == DUAL_PORT)
	{
	 if( (u2Fp % 2) != 0 )
	 {
	  u2Fp = u2Fp + (2 - (u2Fp % 2));
 	 } 
	}	
	else if(LVDS_OUTPUT_PORT == FOUR_PORT)
	{
	 if (u2Fp % 4 !=  0)
	 {
	  u2Fp = u2Fp + (4 - (u2Fp % 4));
	 }
	} 

    if (fgIsMJCToOSTG())
    {
        vPscSetPanelHSyncFp(u2Fp);
    }
    else
    {
        vScpipSetHsyncFp(u2Fp);
    }
}

void vDrvSetHsyncBp(UINT16 u2Bp) 
{
    if (u2Bp <= REORDERBPLimit)
    {
        u2Bp = REORDERBPLimit;
    }

    if(LVDS_OUTPUT_PORT == DUAL_PORT)
	{
	 if( (u2Bp % 2) != 0 )
	 {
	  u2Bp = u2Bp + (2 - (u2Bp % 2));
 	 } 
	}	
	else if(LVDS_OUTPUT_PORT == FOUR_PORT)
	{
	 if (u2Bp % 4 != 0)
	 {
	  u2Bp = u2Bp + (4 - (u2Bp % 4));
	 }
	} 
    
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelHSyncBp(u2Bp);
    }
    else
    {
        vScpipSetHsyncBp(u2Bp);
    }
}

UINT16 wDrvGetHsyncBp(void) 
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4HBP);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
	        return ((UINT16) u4PscGetPanelHSyncBp());
	    }
	    else
	    {
	        return ((UINT16)u4ScpipGetHsyncBp());
	    }
	}
}
UINT16 wDrvGetHsyncFp(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4HFP);
    }
    else
    {
		if (fgIsMJCToOSTG())
		{
		 return ((UINT16)u4PscGetPanelHSyncFp());
		} 
		else
		{
		 return ((UINT16)u4ScpipGetHsyncFp());
		}
	}
}

void vDrvSetVsyncFp(UINT16 u2VFp)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelVSyncFp(u2VFp);
    } 
    else
    {
        vScpipSetVsyncFp(u2VFp);
    }
}

void vDrvSetVsyncBp(UINT16 u2VBp)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelVSyncBp(u2VBp);
    } 
    else
    {
        vScpipSetVsyncBp(u2VBp);
    }
}


UINT16 wDrvGetVsyncFp(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4VFP);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
	        return ((UINT16)u4PscGetPanelVSyncFp());
	    } 
	    else
	    {
	        return ((UINT16)u4ScpipGetVsyncFp());
	    }
    }
}

UINT16 wDrvGetVsyncBp(void)
{
	if (fgApplyScalerSetData)
    {
		return ((UINT16)lcdtimingInfo.u4VBP);
    }
    else
    {
	    if (fgIsMJCToOSTG())
	    {
	        return ((UINT16)u4PscGetPanelVSyncBp());
	    } 
	    else
	    {
	        return ((UINT16)u4ScpipGetVsyncBp());
	    }
    }
}

#ifndef CC_MTK_LOADER
#ifdef SUPPORT_PANEL_48HZ
/**
 * @brief fgDrvSupport24Hz

 * use variable to control the frame rate control under 24Hz

 * @param  void
 * @retval TRUE/FALSE
 */
UINT8 fgDrvSupport24Hz(void)
{
    if (IS_PANEL_L12R12)
    {
        if( bGetVideoDecType(SV_VP_MAIN) != SV_VD_DVI)			
        return (SV_FALSE);
		else
        return (SV_TRUE);			
    }
	else
	{
        return (SV_TRUE);
	}
}
#endif
#endif

/**
 * @brief bDrvCheckMTKGoodStatus

 * Check MTK Good Clock Lock or Unlock

 * @param  void
 * @retval TRUE/FALSE
 */
UINT8 bDrvCheckMTKGoodStatus(void)
{
    return (IO32ReadFldAlign(STA_DDDS_00, STA_DDDS_00_DDDS_LOCK));
}

UINT32 fgDrvSetMJCToOSTG(UINT32 fgOnOff)
{
    #ifdef CC_MT5398
    if (fgOnOff == IO32ReadFldAlign(OSTG_01, REG_MJC_IN_EN))
    {
        return FALSE;
    }

    vIO32WriteFldAlign(OSTG_01, fgOnOff, REG_MJC_IN_EN);
    vDrvSetLCDTiming();

    return TRUE;
    #else
    return FALSE;
    #endif
}

UINT32 fgIsMJCToOSTG(void)
{
    #ifdef CC_MT5398
    return (IO32ReadFldAlign(OSTG_01, REG_MJC_IN_EN) == 1);
    #else
    return FALSE;
    #endif
}

 #ifndef CC_MTK_LOADER

#ifdef SUPPORT_SW_FRAME_CHECK
UINT16 u4ScposGetInOutVDistance(void)
{
    return IO32ReadFldAlign(SCPIP_PIP1_RO_00, PIP1_RO_00_VCOUNTER_OUT_1);
}
#endif

#define SUPPORT_IN_LOW_FRAME_RATE 36
/**
 * @brief vDrvSetMTKGoodDclk

 * Set MTK Good Dclk Parameters

 * @param  wVERT_LINE: Input Total Lines in the Vertical Direction
 * @retval void
 */ 
void vDrvSetMTKGoodDclk(UINT16 u2InputVTotal, UINT8 bInputFrameRate) // Called by Video
{
    UINT32 DISPLAY_SIZE, HLEN_INT, HLEN_NUM;
    UINT32 u4HTotal, u4VTotal;
    #if ENABLE_DDDS2_FEEDBACK
     UINT8 u1VopllRatio;
    #endif
    #ifdef SUPPORT_SW_FRAME_CHECK
    UINT8 u4VStep;
    UINT32  u4VDistance;
    #endif

    if ((u2InputVTotal == 0) || (bInputFrameRate == 0))
    {
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return;
    }

    u4HTotal = (UINT32)wDrvGetOutputHTotal();
    u4VTotal = (UINT32)wDrvGetOutputVTotal();

    #ifdef SUPPORT_SW_FRAME_CHECK
    if (_u4VDistTrackState == 1) // down tracking
    {   
        u4VStep =  (_u4DownTrackRange > _u4VDistTrackMaxStep)? _u4VDistTrackMaxStep: _u4DownTrackRange;
        u4VTotal = (u4VTotal > u4VStep)? (u4VTotal -u4VStep): 0; 
    }
    else if (_u4VDistTrackState == 2)  // up tracking
    {
        u4VStep =  (_u4UpTrackRange > _u4VDistTrackMaxStep)? _u4VDistTrackMaxStep: _u4UpTrackRange;
        u4VTotal = u4VTotal +u4VStep; 
    }

    if((_u4VDistTrackState == 1) ||(_u4VDistTrackState == 2))
    {
        u4VDistance = u4ScposGetInOutVDistance();
        if ((u4VDistance >= (wDrvGetOutputVTotal()/ 2))
                && (u4VDistance <= wDrvGetOutputVTotal()))
        {
            u4VDistance = wDrvGetOutputVTotal() - u4VDistance;
        }

        if ((u4VStep <= _u4VDistTrackMaxStep) && (u4VStep >= 2) && (u4VDistance <= Threadhold))
        {
            _u4VDistTrackMaxStep = u4VStep / 2;
        }

    }

    #endif     
    {
        DISPLAY_SIZE = u4HTotal * u4VTotal;
    }
    
    //bInputFrameRate = bDrvVideoGetRefreshRate(SV_VP_MAIN);
    #if SUPPORT_DDDS_ON_24P_25P_30P
    if (((bInputFrameRate== 24) || (bInputFrameRate == 25) || (bInputFrameRate== 30)) 
    	&& (bDrvVideoIsSrcInterlace(SV_VP_MAIN) == SV_FALSE))
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "1DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }
    else
    #endif
    if ((bInputFrameRate < SUPPORT_IN_LOW_FRAME_RATE)&& (bDrvVideoIsSrcInterlace(SV_VP_MAIN) == SV_FALSE))
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "0DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }
    else		
    if (((bInputFrameRate== 48) || (bInputFrameRate== 100) || (bInputFrameRate == 120)))
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "2DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }
    else if (PANEL_IsSupport120Hz())
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "3DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }
    else if (PANEL_IsSupport240Hz())
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "4DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }
    else
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "5DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }

    DISPLAY_SIZE = bDrvVideoIsSrcInterlace(SV_VP_MAIN) ? DISPLAY_SIZE * 2 : DISPLAY_SIZE;

    #if ENABLE_DDDS2_FEEDBACK
    if ((LVDS_OUTPUT_PORT == SINGLE_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=1;		
    else  if (LVDS_OUTPUT_PORT == SINGLE_PORT) u1VopllRatio=0;
    else  if ((LVDS_OUTPUT_PORT == DUAL_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=2;		
    else  if ((LVDS_OUTPUT_PORT == DUAL_PORT)&&(IS_PANEL_L12R12)) u1VopllRatio=2;			
    else  if (LVDS_OUTPUT_PORT == DUAL_PORT) u1VopllRatio=1;
    else  if ((LVDS_OUTPUT_PORT == FOUR_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=3;
    else  if ((LVDS_OUTPUT_PORT == FOUR_PORT)&&(IS_PANEL_L12R12)) u1VopllRatio=3;    
    else u1VopllRatio=2;
	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
    {
        if(u1VopllRatio == 1)
            DISPLAY_SIZE = DISPLAY_SIZE / 3;
        else if(u1VopllRatio == 2)
            DISPLAY_SIZE = DISPLAY_SIZE / 6;
            
    }
    else
    {
        DISPLAY_SIZE = DISPLAY_SIZE >> u1VopllRatio;
    }
    #endif

    #ifdef CC_DEBUG
    Printf("u1VopllRatio = %d\n",u1VopllRatio);
	#endif
    
    HLEN_INT = (UINT32) (DISPLAY_SIZE / u2InputVTotal);
    HLEN_NUM = DISPLAY_SIZE - (HLEN_INT * (UINT32) u2InputVTotal);
    LOG(6, "inVTaol: 0x%X, outV: 0x%X, outH: 0x%X, HLEN_INT: 0x%X, HLEN_NUM: 0x%X\n",
    u2InputVTotal, wDrvGetOutputVTotal(), wDrvGetOutputHTotal(), HLEN_INT, HLEN_NUM);
    
    #ifdef SUPPORT_SW_FRAME_CHECK
    #if 0
    HLEN_INT += u4CoarseStepSize;
    
    if(u4CurrentStep!=0 && u4CurrentStep<=200)
    {
        LOG(5,"Chnage HLEN_NUM from (%d) to ",HLEN_NUM);
        if(u4CurrentStep>=FINE_STEP_CENTER)
        {
            HLEN_NUM = HLEN_NUM + (u4FineStepSize * (u4CurrentStep - FINE_STEP_CENTER));
        }
        else
        {
            HLEN_NUM = HLEN_NUM - (u4FineStepSize * (FINE_STEP_CENTER - u4CurrentStep));
        }
        LOG(5,"(%d)\n",HLEN_NUM);        
    }
    #endif    
    #endif    

	#ifdef SUPPORT_DDDS_STEP_TRACKING
    vIO32WriteFldAlign(DDDS_03,_u4ErrorLimit,DDDS_03_DDDS_ERR_LIM);
	#endif
    vIO32WriteFldAlign(DDDS_02,(UINT16)u2InputVTotal,DDDS_02_HLEN_DEN);
    vIO32WriteFldAlign(DDDS_01,(UINT16)HLEN_NUM,DDDS_01_HLEN_NUM);    
    vIO32WriteFldAlign(DDDS_01,HLEN_INT,DDDS_01_HLEN_INT);

   // LOG(3, "HLEN=0x%x . [0x%x / 0x%x] \n",HLEN_INT, HLEN_NUM,u2InputVTotal);    
}
#endif // end of #ifndef CC_MTK_LOADER

UINT8 fgDisplayFreeRun = SV_FALSE;
void vDrvSetDisplayFreeRun(UINT8 bOn_Off)
{
	fgDisplayFreeRun = bOn_Off;
}

void vDrvSetDDDSCloseloop(UINT8 bOn_Off)
{
 static UINT8 fgCloseloop_2nd = 0;

 if ((IS_DISPLAY_FREERUN) || (fgDisplayFreeRun))
 {
	 vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN)|P_Fld(0,DDDS_00_FIX_FS_DDDS_SEL));
 }
 else
 {
 	if (bOn_Off)
 	{
    	if(fgCloseloop_2nd)
    	{
			vIO32WriteFldMulti(DDDS_00,P_Fld(1,DDDS_00_DISP_EN)|P_Fld(1,DDDS_00_FIX_FS_DDDS_SEL));	
    	}
		else
		{
	    	// overcome the clock jump when 1st enter close loop
			vIO32WriteFldAlign(DDDS_00, 1, DDDS_00_DISP_EN);	
			HAL_Delay_us(20);
			vIO32WriteFldAlign(DDDS_00, 1, DDDS_00_FIX_FS_DDDS_SEL);
			fgCloseloop_2nd = 1;
		}
 	}
 	else
 	{
			vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN)|P_Fld(0,DDDS_00_FIX_FS_DDDS_SEL));
 	}
 }
}


/**
 * @brief vDrvSwitchMTKGoodDclk

 * MTK Good Dclk ON/OFF

 * @param  bOn_Off: SV_ON/SV_OFF
 * @retval void
 */
void vDrvSwitchMTKGoodDclk(UINT8 bOn_Off)
{
#ifndef CC_MTK_LOADER
#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
    UINT32 u4TmpCw;
    UINT8 u1Kp0=0;
    UINT8 u1Ki0=0;
#endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
        if(u1DDDSLockStatus != bOn_Off)
        {
            u1DDDSLockStatus = bOn_Off;
            u4TmpCw = IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW);
            u1StepControlWord = 0;
            u1StepDelay = 0;
            u1StepDelay2 = 0;
            if( i4OrigControlWord != _u4TargetCW )
            {
                i4OrigControlWord = _u4TargetCW;
            }
//                        if(TargetErrlimit >= 3)
                            vDrvSetErrorLimit(0);
        }
    }
    else if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
    {
        if(u1DDDSLockStatus != bOn_Off)
        {
            u1DDDSLockStatus = bOn_Off;
            u1StepControlWord = 0;
            u1StepDelay = 0;
            u1StepDelay2 = 0;
            if( i4OrigControlWord != _u4TargetCW )
            {
                i4OrigControlWord = _u4TargetCW;
            }
        }
    }

#endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        if (bOn_Off == SV_ON)
        {
          if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)||
             (PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI))
          {
//            if( bGetVideoDecType(SV_VP_MAIN) != SV_VD_DVI)
//                vIO32WriteFldMulti(DDDS_00,P_Fld(1,DDDS_00_DISP_EN)|P_Fld(1,DDDS_00_FIX_FS_DDDS_SEL));
          }
          else
          {
             vDrvSetDDDSCloseloop(1);
          }
    
        }
#else
    if (bOn_Off == SV_ON)
    {
		vDrvSetDDDSCloseloop(1);
    }
#endif
    else
    {
        #ifdef SUPPORT_SW_FRAME_CHECK
            _u4UpTrackRange = 0;
            _u4DownTrackRange = 0;
            _u4VDistTrackState = 0;
            _u4VDistTrackLocked = 0;
        #endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
        {
            u4TmpCw = IO32ReadFldAlign(STA_DDDS_00, STA_DDDS_00_DDDS_FREQ_CW);
            u1Ki0 = IO32ReadFldAlign(DDDS_02, DDDS_02_DDDS_KI_0);
            u1Kp0 = IO32ReadFldAlign(DDDS_02, DDDS_02_DDDS_KP_0);
            vIO32WriteFldMulti(DDDS_02, P_Fld(0,DDDS_02_DDDS_KI_0)|P_Fld(0,DDDS_02_DDDS_KP_0));
            vIO32WriteFldAlign(DDDS_00, u4TmpCw, DDDS_00_DDDS_FREQ_CW);
        }
#endif

        vDrvSetDDDSCloseloop(0); 

        vIO32WriteFldAlign(DDDS_03, 1, DDDS_03_SPREAD_INIT);
        #ifdef CC_COPLAT_MT82
        vUtDelay2us(50);
        #else
        HAL_Delay_us(100);
        #endif
        vIO32WriteFldAlign(DDDS_03, 0, DDDS_03_SPREAD_INIT);   

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
        {
            vIO32WriteFldMulti(DDDS_02, P_Fld(u1Ki0,DDDS_02_DDDS_KI_0)|P_Fld(u1Kp0,DDDS_02_DDDS_KP_0));
        }
#endif
    }
    #endif //end of #ifndef CC_MTK_LOADER 
}

UINT32 u4GetPanelVSyncBp(void)
{
 UINT32 u4VPorchB;

 #ifdef SUPPORT_PANEL_CUSTOMER_SPEC 
 u4VPorchB = PANEL_GetVSyncBpByTiming(bSI_DISPLAY_DCLK_TYPE);
 #else 
 u4VPorchB = (V_POS < (0x400 - VS_WIDTH + 6))?((0x400 - V_POS - VS_WIDTH + 6)):(0);
 #endif

 return (u4VPorchB);
} 

// LVDS power------------------------------------------------------------------
void vDrvTurnOnPanel()
{
    do 
    { 
        UINT32 gpio, value; 
        if(DRVCUST_PanelQuery(ePanelPowerGpio, &gpio) == 0 && 
           DRVCUST_PanelQuery(ePanelPowerOnVal, &value) == 0)
        {
            vDrvGpioSet((INT32)gpio, (INT32)value);
        }
    } while (0);
}

void vDrvTurnOffPanel()
{
    do 
    { 
        UINT32 gpio, value; 
        if(DRVCUST_PanelQuery(ePanelPowerGpio, &gpio) == 0 && 
           DRVCUST_PanelQuery(ePanelPowerOnVal, &value) == 0)
        {
            vDrvGpioSet((INT32)gpio, (INT32)!value);
        }
    } while (0);
}
//-----------------------------------------------------------------------------
/** turn on/off panel lvds signal. (used by MTAL)
 *  @param fgEnable: Control mode. It could be TRUE or FALSE
 *  @return
 */
//-----------------------------------------------------------------------------

void vDrvLVDSSignalOnOff(BOOL fgOnOff)
{
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_LVDS)
    {
	if(fgOnOff)
	{
		vDrvLVDSOn();
	}
	else
	{
		vDrvLVDSOff();
	}
	}
}

//-----------------------------------------------------------------------------
/** turn on/off panel Vx1 signal. (used by MTAL)
 *  @param fgEnable: Control mode. It could be TRUE or FALSE
 *  @return
 */
//-----------------------------------------------------------------------------

void vDrvVByOneSignalOnOff(BOOL fgOnOff)
{
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
		if(fgOnOff)
		{
			vDrvVByOneOn();
		}
		else
		{
			vDrvVByOneOff();
		}
	}
}

//-----------------------------------------------------------------------------
/** turn on/off panel lvds powe. (used by MTAL)
 *  @param fgEnable: Control mode. It could be TRUE or FALSE
 *  @return
 */
//-----------------------------------------------------------------------------

void vDrvLVDSPowerOnOff(BOOL fgOnOff)
{
	if(fgOnOff)
	{
		vDrvTurnOnPanel();
	}
	else
	{
		vDrvTurnOffPanel();
	}
}

void vDrvVsyncISRSetPosition(UINT16 u2INTRLine)
{
    vIO32WriteFldAlign(INTR_01, u2INTRLine, REG_INTR_LINE); 
}

void vDrvVsyncISRSetPosition_PDP(UINT16 u2INTRLine)
{
    vIO32WriteFldAlign(INTR_01, 1, REG_INTR_LINE_LATCH_OFF); 
    vIO32WriteFldAlign(INTR_01, u2INTRLine, REG_INTR_LINE); 
    vIO32WriteFldAlign(INTR_01, 0, REG_INTR_LINE_LATCH_OFF); 	
}

static UINT16 vDrvVsyncISRGetPosition(void)
{
    return(IO32ReadFldAlign(INTR_01, REG_INTR_LINE)); 
}

UINT8 u1DrvGetVsyncISRFCNTStatus(void)
{
    return(IO32ReadFldAlign(INTR_00, FCONT)); 
}

static void vDrvVsyncISRInit(UINT16 u2Vfp)
{
    if (IS_PANEL_L12R12)
    {
        vDrvVsyncISRSetPosition((wDrvGetOutputVTotal()/2)-u2Vfp); 
    }
    else
    {
        vDrvVsyncISRSetPosition(wDrvGetOutputVTotal()-u2Vfp); 
    }
}

static void vDrvVsyncISRInit_PDP(void)
{
    if (IS_PANEL_L12R12)
    {
        vDrvVsyncISRSetPosition_PDP(0); 
    }
}

void vDrvSetTCONIrqClear(UINT32 u4INTRSource)
{
    vIO32WriteFldAlign(INTR_02, u4INTRSource, C_INTR_CLEAR);
    vIO32WriteFldAlign(INTR_02, 0, C_INTR_CLEAR);
}

void vDrvGetTCONIrqStatus(UINT32 * u4INTRStatus)
{
    *u4INTRStatus = IO32ReadFldAlign(INTR_00, STATUS_INTR);
}

static void vDrvVsyncISRSetMask(UINT16 u2Mask)
{
    vIO32WriteFldAlign(INTR_02, u2Mask,C_INTR_MASK); 
}

static UINT16 vDrvVsyncISRGetMask(void)
{
    return(IO32ReadFldAlign(INTR_02, C_INTR_MASK)); 
}

void vDrvVsyncISRStatus(void)
{
    UINT16 u2MaskStatus;

	u2MaskStatus = vDrvVsyncISRGetMask();
		
	Printf("====Interrupt status====\n");
    Printf("[Interrupt Line] %d \n",vDrvVsyncISRGetPosition());
    Printf("[Interrupt Mask] 0x%x \n",u2MaskStatus);	
	if((u2MaskStatus >> 0) & 0x01)
	Printf("[Interrupt Mask] disable LR_RISIN \n");	
	if((u2MaskStatus >> 1) & 0x01)
	Printf("[Interrupt Mask] disable LR_FALLING \n");	
	if((u2MaskStatus >> 2) & 0x01)
	Printf("[Interrupt Mask] disable L_RISING \n");	
	if((u2MaskStatus >> 3) & 0x01)
	Printf("[Interrupt Mask] disable L_FALLING \n");	
	if((u2MaskStatus >> 4) & 0x01)
	Printf("[Interrupt Mask] disable R_RISING \n");	
	if((u2MaskStatus >> 5) & 0x01)
	Printf("[Interrupt Mask] disable R_FALLING \n");
	if((u2MaskStatus >> 6) & 0x01)
	Printf("[Interrupt Mask] disable OSTGLINE \n");
	if((u2MaskStatus >> 7) & 0x01)
	Printf("[Interrupt Mask] disable AMBILIT \n");
	
}

void vDrvVsyncISRSetting(UINT16 u2Line, UINT16 u2Mask)
{
    vDrvVsyncISRSetPosition(u2Line);
	vDrvVsyncISRSetMask(u2Mask);

	if ((u2Line>wDrvGetOutputVTotal())&&(!(IS_PANEL_L12R12)))
	{
     Printf("u2INTRLine(%d) over Panel V total (%d)\n",u2Line, wDrvGetOutputVTotal());
	 Printf("Interrupt will not active\n");
	}
}


static UINT8 wDrvGetScalerDISPR_INStatus(void)
{
    if (fgIsMJCToOSTG())
    {
	    return ((UINT8) u1PscGetInDispR());
    }
	else
	{
        return ((UINT8) u1ScpipGetInDispR());
	}
}

static UINT8 wDrvGetScalerDISPR_OUTStatus(void)
{
    if (fgIsMJCToOSTG())
    {
	    return ((UINT8) u1PscGetOutDispR());
    }
	else
	{
        return ((UINT8) u1ScpipGetOutDispR());
	}
}

static UINT8 wDrvGetLVDSDISPR_OUTStatus(void)
{
    return ((UINT8) IO32ReadFldAlign(LVDSB_REG14, ST_LVDS_DISP_R));
}

void vGetPostScalerStatus(void)
{
    UINT8 u1Cnt;

    Printf("===== [Setting] ===== \n");
	Printf("[HSync] Htoal=%d Hactive=%d  Hfp= %d Hbp= %d Hwidth=%d\n", 
		   wDrvGetOutputHTotal(),
		   wDrvGetOutputHActive(), 
		   wDrvGetHsyncFp(), 
		   wDrvGetHsyncBp(),
           wDrvGetOutputHTotal()-wDrvGetOutputHActive()-wDrvGetHsyncFp()-wDrvGetHsyncBp()
	);
	Printf("[VSync] Vtoal=%d Vactive=%d  Vfp= %d Vbp= %d Vwidth=%d\n", 
		   wDrvGetOutputVTotal(),
		   wDrvGetOutputVActive(), 
		   wDrvGetVsyncFp(), 
		   wDrvGetVsyncBp(),
		   wDrvGetOutputVTotal()-wDrvGetOutputVActive()-wDrvGetVsyncFp()-wDrvGetVsyncBp()
	);
	if (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ)
	Printf("[Frame rate] SV_DCLK_48HZ\n");
	else if  (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_50HZ)
	Printf("[Frame rate] SV_DCLK_50HZ\n");
	else if  (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ)
	Printf("[Frame rate] SV_DCLK_60HZ\n");
	else if  (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_OTHERS)
	Printf("[Frame rate] SV_DCLK_OTHERS\n");
	else if  ( bSI_DISPLAY_DCLK_TYPE == SV_DCLK_UNKNOW)
	Printf("[Frame rate] SV_DCLK_UNKNOW\n");

	Printf("_fgPanelDynamicChg =%d\n", _fgPanelDynamicChg);
    Printf("_bForceFrameRatee =%d Hz\n", _bForceFrameRate);
    Printf("fgDisplayFreeRun =%d \n", fgDisplayFreeRun);	

    Printf("Output refresh rate =%d Hz\n", vDrvGetLCDFreq());	

    Printf("===== [Status] ===== \n");
	Printf("HTotal =%d\n", wDrvGetOutputHTotalStatus());
	Printf("VTotal =%d\n", wDrvGetOutputVTotalStatus());
	Printf("DISP_R_IN =%d\n", wDrvGetScalerDISPR_INStatus());
	Printf("DISP_R_OUT=%d\n", wDrvGetScalerDISPR_OUTStatus());	

    Printf("===== [OSTG Status] ===== \n");
	Printf("[Input]DISP_R=%d\n", IO32ReadFldAlign(DEBUG_01, DEBUG_01_PANEL_DISP_R));	
	Printf("[Input]HTotal =%d\n", IO32ReadFldAlign(DEBUG_02, DEBUG_02_INPUT_HTOTAL));
	Printf("[Input]VTotal =%d\n", IO32ReadFldAlign(DEBUG_02, DEBUG_02_INPUT_VTOTAL));

	Printf("[Color]HTotal =%d\n", IO32ReadFldAlign(TIMING_DETECTION_0, COLOR_HTOTAL_CNT));
	Printf("[Color]HActive =%d\n", IO32ReadFldAlign(TIMING_DETECTION_1, COLOR_HDE_CNT)); 
	Printf("[Color]VTotal =%d\n", IO32ReadFldAlign(TIMING_DETECTION_0, COLOR_VTOTAL_CNT));
	Printf("[Color]VActive =%d\n", IO32ReadFldAlign(TIMING_DETECTION_1, COLOR_VDE_CNT)); 
	
	vIO32WriteFldAlign(DEBUG_00, 1, DEBUG_00_R_DEBUG_OSTG_EN);
	HAL_Delay_us(20000);

    for (u1Cnt = 0; u1Cnt<4; u1Cnt++)
    {
	  vIO32WriteFldAlign(DEBUG_01, u1Cnt, DEBUG_01_R_HVM_INPUT_SEL);	
	  HAL_Delay_us(40000);

	  if (IO32ReadFldAlign(DEBUG_01,DEBUG_01_R_HVM_INPUT_SEL) == 0)
	  {
       Printf("The interface between OD to OSTG (default)\n");
	  }
	  else if (IO32ReadFldAlign(DEBUG_01,DEBUG_01_R_HVM_INPUT_SEL) == 1)
	  {
       Printf("The interface between OSTG to OD\n");
	  }	
	  else if (IO32ReadFldAlign(DEBUG_01,DEBUG_01_R_HVM_INPUT_SEL) == 2)
	  {
       Printf("The interface between MJC to OSTG\n");
	  }	
	  else if (IO32ReadFldAlign(DEBUG_01,DEBUG_01_R_HVM_INPUT_SEL) == 3)
	  {
       Printf("The input of OSTG (Note: this option will be fail when project exists MJC)\n");
	  }		  

	 Printf("[Output]HTotal =%d\n", IO32ReadFldAlign(DEBUG_03, DEBUG_03_HVM_HTOTAL_CNT));
	 Printf("[Output]H Active =%d\n", IO32ReadFldAlign(DEBUG_04, DEBUG_04_HVM_HDE_CNT));	 
	 
	 Printf("[Output]VTotal =%d\n", IO32ReadFldAlign(DEBUG_03, DEBUG_03_HVM_VTOTAL_CNT));	
	 Printf("[Output]V Active =%d\n", IO32ReadFldAlign(DEBUG_04, DEBUG_04_HVM_VDE_CNT));
    }

	vIO32WriteFldAlign(DEBUG_00, 0, DEBUG_00_R_DEBUG_OSTG_EN);

}


void vGetDISP_RStatus(void)
{
    Printf("==[DISP_R Setting - Normal Mode] == \n");
    if(IO32ReadFldAlign(LVDSB_REG06,RG_RES_FLD))
	Printf("DISP_R Encode in A Channel\n");		
    if(IO32ReadFldAlign(LVDSB_REG18,RG_RES_FIELD_L))
	Printf("DISP_R Encode in B Channel\n");	
    if(IO32ReadFldAlign(LVDSB_REG19,RG_RES_FIELD_U_4CH))
	Printf("DISP_R Encode in C Channel\n"); 	
    if(IO32ReadFldAlign(LVDSB_REG19,RG_RES_FIELD_L_4CH))
	Printf("DISP_R Encode in D Channel\n"); 
		
    Printf("==[DISP_R Setting - OSD Mode] == \n");
    if(IO32ReadFldAlign(LVDSB_REG08,RG_OSD_3D_IN_HS))
	Printf("DISP_R Encode in HSync\n");	
    if(IO32ReadFldAlign(LVDSB_REG08,RG_OSD_3D_IN_VS))
	Printf("DISP_R Encode in VSync\n");	
	
    Printf("==[DISP_R Status] == \n");
	Printf("[Scaler] DISP_R_IN =%d\n", wDrvGetScalerDISPR_INStatus());
	Printf("[Scaler] DISP_R_OUT=%d\n", wDrvGetScalerDISPR_OUTStatus());	
    Printf("[LVDS] DISP_R_OUT=%d\n", wDrvGetLVDSDISPR_OUTStatus());
}

void u1SetDISP_REncodeEn(UINT8 u1Enable)
{     
    vIO32WriteFldAlign(LVDSB_REG06,(u1Enable>>0 & 0x01),RG_RES_FLD);
    vIO32WriteFldAlign(LVDSB_REG18,(u1Enable>>1 & 0x01),RG_RES_FIELD_L);	
    vIO32WriteFldAlign(LVDSB_REG19,(u1Enable>>2 & 0x01),RG_RES_FIELD_U_4CH);	
    vIO32WriteFldAlign(LVDSB_REG19,(u1Enable>>3 & 0x01),RG_RES_FIELD_L_4CH);
    vIO32WriteFldAlign(LVDSB_REG08,(u1Enable>>4 & 0x01),RG_OSD_3D_IN_HS);
    vIO32WriteFldAlign(LVDSB_REG08,(u1Enable>>5 & 0x01),RG_OSD_3D_IN_VS);	
	if ((u1Enable>>4 & 0x01)==1 || (u1Enable>>5 & 0x01))
	{
	 vIO32WriteFldAlign(LVDSB_REG06,0x01,RG_RES_FLD);
	}
}

void u1SetDISP_REncodeEn_L4(BOOL bValue)
{     
    vIO32WriteFldAlign(LVDSB_REG06,bValue,RG_RES);
}

void u1SetDISP_REncodeEnHW_L4(BOOL bValue)
{     
	vIO32WriteFldAlign(LVDSB_REG19,bValue,RG_RES2_FLD);
	vIO32WriteFldAlign(LVDSB_REG19,bValue,RG_RES2_FLD_L);			
	vIO32WriteFldAlign(LVDSB_REG19,bValue,RG_RES2_FLD_L_4CH);
	vIO32WriteFldAlign(LVDSB_REG19,bValue,RG_RES2_FLD_U_4CH);	
}

/**
 * @brief u1SetDISP_REncode_L4

 * encode disp_r on Link 4 with register value

 * @param  void
 * @retval TRUE/FALSE
 */

void u1SetDISP_REncode_L4(void)
{     
    #ifdef CC_SCPOS_3DTV_SUPPORT
    static UINT8 u1LRResetFlag = 1;
    static UINT8 u13DCurDispR = 2;
    static UINT8 u13DPreDispR = 2;
    if(IS_DISPR_ENCODEL4)
    {
   	  	if ((BSP_GetIcVersion() == IC_VER_5398_AA)||(BSP_GetIcVersion() == IC_VER_5880_AA)||(BSP_GetIcVersion() == IC_VER_5860_AA))
   	  	{
		  	
        	u13DPreDispR = u13DCurDispR;
        	u13DCurDispR = IO32ReadFldAlign(LVDSB_REG14, ST_LVDS_DISP_R);

        	if (u1LRResetFlag && (u13DPreDispR == 0) && u13DCurDispR)
        	{
            	u1SetDISP_REncodeEn_L4(1);    
             	u1LRResetFlag = 0;
        	}
        	else
        	{
            	u1SetDISP_REncodeEn_L4(0);
            	u1LRResetFlag = 1;	  
        	}
   	  	}
    }
    #endif
}

void vGetLVDS_HVEncodeStatus(void)
{   
    Printf("==[LVDS H/V Encode Value] == \n");
	if (IO32ReadFldAlign(LVDSB_REG06,RG_VS_SEL))
	{
     Printf("[VSYNC Enocde] Vsync encode value=%d\n", IO32ReadFldAlign(LVDSB_REG06,RG_CNTLF));
	}
	if (IO32ReadFldAlign(LVDSB_REG06,RG_HS_SEL))
	{
     Printf("[HSYNC Enocde] Hsync encode value=%d\n", IO32ReadFldAlign(LVDSB_REG06,RG_CNTLE));
	}	
}

void u1SetLVDS_HVEncodeValue(UINT8 u1Enable)
{     
    vIO32WriteFldAlign(LVDSB_REG06,(u1Enable>>0 & 0x01),RG_VS_SEL);
    vIO32WriteFldAlign(LVDSB_REG06,(u1Enable>>1 & 0x01),RG_CNTLF);	
    vIO32WriteFldAlign(LVDSB_REG06,(u1Enable>>2 & 0x01),RG_HS_SEL);	
    vIO32WriteFldAlign(LVDSB_REG06,(u1Enable>>3 & 0x01),RG_CNTLE);	
}

BOOL fgIsVsyncPDPInit = SV_FALSE;
extern BOOL fgIsVsyncIsrStart;
extern UINT32 _u4VSyncCountSD;

extern BOOL _fgVOPLLPowerOn;
/**
 * @brief vDrvSetLCDTiming

 * Set the Display Output Timing for LCD Panel

 * @param  void
 * @retval void
 */
void vDrvSetLCDTiming(void)
{
    UINT32 u4SyncLen;
    UINT32 u4HPorchF, u4HPorchB;
    UINT32 u4VPorchF, u4VPorchB;
    UINT32 u4HTotal,u4VTotal;
    #ifndef CC_MTK_LOADER    
    CRIT_STATE_T csState;
    #endif

    if ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ) || 
        ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_OTHERS) && ((bOUTPUT_REFRESH_RATE >= (55*(PANEL_IsSupport120Hz()? 2: 1))) || bOUTPUT_REFRESH_RATE == 24 || bOUTPUT_REFRESH_RATE == 30) ))
    {
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC 
        u4SyncLen = PANEL_GetHSyncWidthByTiming(SV_DCLK_60HZ);
        #else        
        u4SyncLen = PANEL_GetHSyncWidth();
		#endif		
        u4HPorchF = (H_POS_60HZ > 0x800)?(H_POS_60HZ-0x800):(0);
        u4HPorchB = HLEN_TOTAL_TYPI - wDISPLAY_WIDTH - u4HPorchF - u4SyncLen;

        #ifndef NDEBUG
        Printf("H_POS_60HZ=%d\n",H_POS_60HZ);
        Printf("HLEN_TOTAL_TYPI=%d\n",HLEN_TOTAL_TYPI);
        Printf("HSYNCLEN_TYPI=%d\n",u4SyncLen);
        Printf("u4HPorchF=%d\n",u4HPorchF);
        Printf("wDISPLAY_WIDTH=%d\n",wDISPLAY_WIDTH);        
		#endif
        
		u4HTotal = HLEN_TOTAL_TYPI;

        u4VTotal = VLEN_TOTAL_TYPI;
	    #ifdef SUPPORT_PANEL_CUSTOMER_SPEC  
	     u4VPorchB = PANEL_GetVSyncBpByTiming(SV_DCLK_60HZ);			
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - PANEL_GetVSyncWidthByTiming(SV_DCLK_60HZ) - u4VPorchB;
	    #else 
		 u4VPorchB = u4GetPanelVSyncBp();
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - VS_WIDTH - u4VPorchB;
	    #endif	
    }
    #ifdef SUPPORT_PANEL_48HZ
    else if ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
    {
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC 
	    u4SyncLen = PANEL_GetHSyncWidthByTiming(SV_DCLK_48HZ);
        #else
        u4SyncLen = PANEL_GetHSyncWidth();
	    #endif

        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        u4HPorchF = (PANEL_GetHPositionByTiming(SV_DCLK_48HZ) > 0x800)?(PANEL_GetHPositionByTiming(SV_DCLK_48HZ)-0x800):(0);
        #else
        u4HPorchF = (H_POS_60HZ > 0x800)?(H_POS_60HZ-0x800):(0);
        #endif
	    if (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
        {   
            u4HPorchB = HLEN_TOTAL_48HZ - wDISPLAY_WIDTH - u4HPorchF - u4SyncLen;
        }
		else
        {      
            u4HPorchB = HLEN_TOTAL_TYPI - wDISPLAY_WIDTH - u4HPorchF - u4SyncLen;    
        }
	    if (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
        {   
	        u4HTotal = HLEN_TOTAL_48HZ;	
        }
		else
        {      
		    u4HTotal = HLEN_TOTAL_TYPI;
        }

        u4VTotal = VLEN_TOTAL_48HZ;
	    #ifdef SUPPORT_PANEL_CUSTOMER_SPEC  
	     u4VPorchB = PANEL_GetVSyncBpByTiming(SV_DCLK_48HZ);					
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - PANEL_GetVSyncWidthByTiming(SV_DCLK_48HZ) - u4VPorchB;
	    #else 
		 u4VPorchB = u4GetPanelVSyncBp();
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - VS_WIDTH - u4VPorchB;
	    #endif			
    }
    #endif
    else
    {
		#ifdef SUPPORT_PANEL_CUSTOMER_SPEC 
        u4SyncLen = PANEL_GetHSyncWidthByTiming(SV_DCLK_50HZ);
		#else
        u4SyncLen = PANEL_GetHSyncWidth();
		#endif
		
        if (H_POS_50HZ > 0)
        {
            u4HPorchF = (H_POS_50HZ > 0x800)?(H_POS_50HZ-0x800):(0);
        }
        else
        {
            u4HPorchF = (H_POS_60HZ > 0x800)?(H_POS_60HZ-0x800):(0);
        }
		
		u4HPorchB = HLEN_TOTAL_50HZ - wDISPLAY_WIDTH - u4HPorchF - u4SyncLen;

	    u4HTotal = HLEN_TOTAL_50HZ;

        u4VTotal = VLEN_TOTAL_50HZ;
	    #ifdef SUPPORT_PANEL_CUSTOMER_SPEC  
	     u4VPorchB = PANEL_GetVSyncBpByTiming(SV_DCLK_50HZ);			
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - PANEL_GetVSyncWidthByTiming(SV_DCLK_50HZ) - u4VPorchB;
	    #else 
		 u4VPorchB = u4GetPanelVSyncBp();
	     u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - VS_WIDTH - u4VPorchB;
	    #endif			
    }
    LOG(0, "VPorch (%d %d)\n", u4VPorchF, u4VPorchB);
	
	if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)|| (PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
	{
	 u4VPorchB = 7;
	 u4VPorchF = u4VTotal - wDISPLAY_HEIGHT - 1 - u4VPorchB;	
	}

    // vDrvSetVsyncFp(u4VPorchF);
    // vDrvSetVsyncBp(u4VPorchB);
    #ifndef CC_MTK_LOADER        
    csState = x_crit_start();
    #endif
    lcdtimingInfo.u4HTotal = u4HTotal;
    lcdtimingInfo.u4HActive= wDISPLAY_WIDTH;
    lcdtimingInfo.u4HFP = u4HPorchF;
    lcdtimingInfo.u4HBP = u4HPorchB;		

    lcdtimingInfo.u4VTotal = u4VTotal;
    lcdtimingInfo.u4VActive= wDISPLAY_HEIGHT;
    lcdtimingInfo.u4VFP = u4VPorchF;
    lcdtimingInfo.u4VBP = u4VPorchB;
    #ifndef CC_MTK_LOADER    
    x_crit_end(csState);
    #endif
	
	if((IS_PANEL_L12R12) && (!fgIsVsyncPDPInit))
	{
		vDrvVsyncISRInit_PDP(); 
		fgIsVsyncPDPInit = SV_TRUE;				
	}
	#ifndef CC_MTK_LOADER
	if ((fgIsVsyncIsrStart)&&(_fgVOPLLPowerOn))
	{
		fgApplyScalerSetData = 1;
		LOG(3,"fgApplyScalerSetData = SV_TRUE, _u4VSyncCountSD=%d \n",_u4VSyncCountSD);
	}	
	else
    #endif	    
	{
		vDrvSetLCDTiming_SETDATA();
		vDrvFireImportPortection();
	}   
}
#ifdef SUPPORT_PANEL_SCAN_PWM
extern UINT32 _u4BrightLevel;
#endif
extern void vDrvTCONISRSetLineCounter(UINT32 u4VFp, UINT32 u4VActive, UINT32 u4VBp);
void vDrvSetLCDTiming_SETDATA(void)
{
	LOG(3,"vDrvSetLCDTiming_SETDATA\n");

	wDrvSetOutputHTotal(lcdtimingInfo.u4HTotal);
    wDrvSetOutputHActive(lcdtimingInfo.u4HActive);
	vDrvSetHsyncFp(lcdtimingInfo.u4HFP);
	vDrvSetHsyncBp(lcdtimingInfo.u4HBP);
	
	wDrvSetOutputVTotal(lcdtimingInfo.u4VTotal);	
	wDrvSetOutputVActive(lcdtimingInfo.u4VActive);
    vDrvSetVsyncFp(lcdtimingInfo.u4VFP);
    vDrvSetVsyncBp(lcdtimingInfo.u4VBP);

    // ISR Setting
    vDrvVsyncISRInit(lcdtimingInfo.u4VFP);
    vDrvTCONISRSetLineCounter(lcdtimingInfo.u4VFP, lcdtimingInfo.u4VActive, lcdtimingInfo.u4VBP);

    // For 3D_Glasses Control - software work around
    vIO32WriteFldAlign(VINTF_P8, lcdtimingInfo.u4VTotal,  VINTF_REYE_WIDTH); 
    vIO32WriteFldAlign(VINTF_P9, 1,  VINTF_REYE_RST_MODE);    
    vIO32WriteFldAlign(VINTF_P10, lcdtimingInfo.u4VTotal,  VINTF_LEYE_WIDTH); 
    vIO32WriteFldAlign(VINTF_P11, 1,  VINTF_LEYE_RST_MODE);

    vErrorHandleSetByTiming();
	// osd Hstart setting 
	OSD_Base_SetHStart();

	// update dimming frequency
#ifdef SUPPORT_PANEL_SCAN_PWM
	vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Set data
    LOG(3, "_|-|_ vDrvSetLCDTiming_SETDATA Bright:%d%\n", _u4BrightLevel);
	BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), _u4BrightLevel);
#ifndef CC_MTK_LOADER
    DRVCUST_UpdateBacklightState(BL_STATE_OUTPUT_FR_CHANGE, _u4BrightLevel);
#endif
#endif
#ifndef CC_MTK_LOADER
    vMonOutUpdate();//when output frame rate change, need to update tve parameter
#endif
}


#ifndef CC_MTK_LOADER
/**
 * @brief bDrvDispSupportTiming

 * Check if Dispmdoe supports the input timing

 * @param  wHeight: Source height
 * @param  wVTotal: Total Lines in the Vertical Direction
 * @param wFrameRate: Frame rate
 * @retval SV_SUCCESS/SV_FAIL
 */
UINT8 bDrvDispSupportTiming(UINT16 wHeight, UINT16 wVTotal, UINT16 wFrameRate)
{
    UINT32 u4OutputVTotal;
    UINT32 u4PanelHeight;

    u4PanelHeight = TV_HEIGHT;
    u4OutputVTotal = (u4PanelHeight * (UINT32)wVTotal + (wHeight >> 1))/(UINT32)wHeight;

    return bDrvDispSupport((UINT16)u4OutputVTotal, wFrameRate);
}


/**
 * @brief bDrvDispSupport

 * Check if Dispmdoe supports the output setting

 * @param  wOutputVTotal: Output Total Lines in the Vertical Direction
 * @param wOutputFrameRate: Output frame rate
 * @retval SV_SUCCESS/SV_FAIL
 */
UINT8 bDrvDispSupport(UINT16 wOutputVTotal, UINT16 wOutputFrameRate)
{
    UINT32 u4RefreshRate;
    UINT32 u4HTotal;
    UINT32 u4HTotalMinBySyncMin;
    UINT32 u4HTotalMinByPixelClock;
    UINT32 u4HTotalMaxByPixelClock;

    LOG(1, "DM Panel V(%d %d %d) H(%d %d %d) P(%d %d %d) F(%d %d)\n",
        DVS_TOTAL_MIN, VLEN_TOTAL_TYPI, DVS_TOTAL_MAX,
        DHS_TOTAL_MIN, HLEN_TOTAL_TYPI, DHS_TOTAL_MAX,
        DCLK_MIN, PANEL_GetPixelClk60Hz(), DCLK_MAX,
        DVS_FREQ_MIN, DVS_FREQ_MAX);

    if ((wOutputVTotal < DVS_TOTAL_MIN) ||
        (wOutputVTotal > DVS_TOTAL_MAX))
    {
        LOG(1, "DM FAIL V(%d)\n", wOutputVTotal);

        return SV_FAIL;
    }

    u4RefreshRate = (UINT32)wOutputFrameRate;

    if (u4RefreshRate == 0)
    {
        LOG(1, "DM FAIL Refresh(%d)\n", u4RefreshRate);

        return SV_FAIL;
    }

    u4HTotalMinByPixelClock = (((UINT32) DCLK_MIN) / u4RefreshRate) / (UINT32) wOutputVTotal;
    u4HTotalMinByPixelClock = ((u4HTotalMinByPixelClock + 1) & 0xfffffffe);     //make as even value
    u4HTotalMaxByPixelClock = (((UINT32) DCLK_MAX) / u4RefreshRate) / (UINT32) wOutputVTotal;
    u4HTotalMaxByPixelClock = ((u4HTotalMaxByPixelClock - 1) & 0xfffffffe);     //make as even value
    u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_BACKPORCH_MIN + CC_FRONTPORCH_MIN);
    u4HTotalMinBySyncMin = ((u4HTotalMinBySyncMin + 1) & 0xfffffffe);     //make as even value

#ifdef CC_HTOTAL_4X_LIMIT_WA
    if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//    if (LVDS_OUTPUT_PORT == DUAL_PORT)
    {
        u4HTotalMinByPixelClock = (u4HTotalMinByPixelClock + 3) & (~3);
        u4HTotalMinBySyncMin = (u4HTotalMinBySyncMin + 3) & (~3);
        u4HTotalMaxByPixelClock = u4HTotalMaxByPixelClock & (~3);
    }
#endif

    LOG(1, "DM H MIN(%d %d) H MAX(%d) \n",
        u4HTotalMinByPixelClock,
        u4HTotalMinBySyncMin,
        u4HTotalMaxByPixelClock);

    // Find H-Total such that,
    // 1) Bigger than H-Total MIN calculate by DCLK_MIN
    // 2) Small than H-Total MAX calculate by DCLK_MAX
    // 3) Bigger than DHS_TOTAL_MIN
    // 4) Small than DHS_TOTAL_MAX
    // 5) Bigger than H-Total MIN calculate by CC_BACKPORCH_MIN
    // 6) Close to HLEN_TOTAL_TYPI

#ifdef CC_HTOTAL_4X_LIMIT_WA
    if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//    if (LVDS_OUTPUT_PORT == DUAL_PORT)
    {
        u4HTotal = (UINT32) ((HLEN_TOTAL_TYPI) + 3) & (~3);
    }
    else
#endif
    {
        u4HTotal = (UINT32) (HLEN_TOTAL_TYPI);
    }

    if (u4HTotal > u4HTotalMaxByPixelClock)
    {
        u4HTotal = u4HTotalMaxByPixelClock;
    }

#ifdef CC_HTOTAL_4X_LIMIT_WA
    if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//    if (LVDS_OUTPUT_PORT == DUAL_PORT)
    {
        if (u4HTotal > (UINT32) ((DHS_TOTAL_MAX + 1) & (~3)))
        {
            u4HTotal = (UINT32)((DHS_TOTAL_MAX + 1) & (~3));
        }
    }
    else
#endif
    {
        if (u4HTotal > (UINT32)(DHS_TOTAL_MAX + 1))
        {
            u4HTotal = (UINT32) (DHS_TOTAL_MAX + 1);
        }
    }

    if (u4HTotal < u4HTotalMinByPixelClock)
    {
        u4HTotal = u4HTotalMinByPixelClock;
    }

    if (u4HTotal < u4HTotalMinBySyncMin)
    {
        u4HTotal = u4HTotalMinBySyncMin;
    }

    if (u4HTotal < (UINT32) (DHS_TOTAL_MIN + 1))
    {
        u4HTotal = (UINT32) (DHS_TOTAL_MIN + 1);
    }

    if ((u4HTotal > u4HTotalMaxByPixelClock) ||
        (u4HTotal > (UINT32) DHS_TOTAL_MAX))
    {
        LOG(1, "DM FAIL V(%d) H(%d) F(%d)\n",
            wOutputVTotal,
            u4HTotal,
            u4RefreshRate);

        return SV_FAIL;
    }

    LOG(1, "DM OK V(%d) H(%d) F(%d)\n",
        wOutputVTotal,
        u4HTotal,
        u4RefreshRate);

    return SV_SUCCESS;

}

/**
 * @brief bDrvDispSupportFlag

 * When DISP_MODE_USE is DISP_MODE_ON, Check V_TOTAL and Pixel Clock Rate and Calculate H_Total.

 * @param  DV_TOTAL: Output Total Lines in the Vertical Direction
 * @retval SV_SUCCESS/SV_FAIL
 */
UINT8 bDrvDispSupportFlag(UINT16 DV_TOTAL)
{
    UINT16 u2RefreshRate;

    u2RefreshRate = (UINT16) bDrvVideoGetRefreshRate(SV_VP_MAIN);

    return bDrvDispSupport(DV_TOTAL, u2RefreshRate);
}
#endif //end of #ifndef  CC_MTK_LOADER


/**
 * @brief vDrvOutputStageInit

 * Init the Register Settings of Output Stage Logic

 * @param  void
 * @retval void
 */

void vDrvVideoDitherInit(void)
{
    // DITHER
    UINT32 u4PanelDither = PANEL_GetDither();

    UINT32 u4Round, u4RDither, u4EDither, u4LFSR, u4Subpxl, u4LsbOff;   

    // 1. OSTG Dither
    // 1.1 Set Dither Depth
    if (DISP_BIT == DISP_30BIT)
    {
        return;
    }
    else if (DISP_BIT == DISP_24BIT)
    {
#ifndef CC_MTK_LOADER                
        if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else        
        if(0)
#endif
        {
            // Front Dither (10 to 10)
            vIO32WriteFldAlign(OS_DITHER_02, 0, DRMOD_R);          // No Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0                                                                        
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)   

            // Post Dither (10 to 8)
            vIO32WriteFldAlign(OS_DITHER_05, 1, REG_DRMOD_R_POST);     // 12 to  8 Dither Mode
            //vIO32WriteFldAlign(OS_DITHER_03, 1, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0                                                                        
            //vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)                                                                
                                                                    
        }
        else
        {
            // Front Dither (10 to 8) 
            vIO32WriteFldAlign(OS_DITHER_02, 2, DRMOD_R);          // 12 to  8 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0                                                                        
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)   
            // Post Dither (Off When OD Disabled)
            vIO32WriteFldAlign(OS_DITHER_05, 0, REG_DRMOD_R_POST);     // 12 to  8 Dither Mode
            //vIO32WriteFldAlign(OS_DITHER_03, 0, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0                                                                        
            //vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)                                                                
                                                             

        }
    }
    else
    {
        Printf("DISP_BIT=%d error\n", DISP_BIT);
    }

    // 1.2 Set Front Dither Type
    if (u4PanelDither != 0xffffffff)
    {
        u4LsbOff = (u4PanelDither & OSTG_LSBOFF_FUNCTION)? 1: 0; 
        u4Round = (u4PanelDither & OSTG_ROUND_FUNCTION)? 1: 0;
        u4RDither = (u4PanelDither & OSTG_RDITHER_FUNCTION)? 1 : 0;
        u4EDither = (u4PanelDither & OSTG_ERROR_DITHER_FUNCTION)? 1 : 0;
        u4LFSR = (u4PanelDither & OSTG_LFSR_FUNCTION)? 1 : 0;
        u4Subpxl = (u4PanelDither & OSTG_SUB_PIXEL_FUNCTION)? 1 : 0;
    }
    else    // No Define
    {
        u4LsbOff = 1;
        u4Round = 0;
        u4RDither = 1;
        u4EDither = 0;
        u4LFSR = 0;        
        u4Subpxl = 1;        
    }

    vIO32WriteFldAlign(OS_DITHER_02, u4LsbOff, DITHER_LSB_OFF);
    vIO32WriteFldAlign(OS_DITHER_00, u4Round, DITHER_ROUND_EN);
    vIO32WriteFldAlign(OS_DITHER_00, u4RDither, RDITHER_EN);        
    vIO32WriteFldAlign(OS_DITHER_00, u4EDither, EDITHER_EN);
    vIO32WriteFldAlign(OS_DITHER_00, u4LFSR, DITHER_LFSR_EN);      
    vIO32WriteFldAlign(OS_DITHER_00, u4Subpxl, REG_SUBPIX_EN);    

    // 1.3 Common Dither Setting
    vIO32WriteFldAlign(OS_DITHER_00, 0, REG_SUB_R);
    vIO32WriteFldAlign(OS_DITHER_00, 1, REG_SUB_G);        
    vIO32WriteFldAlign(OS_DITHER_00, 2, REG_SUB_B);
    
    vIO32WriteFldAlign(OS_DITHER_00, SV_TRUE, REG_FPHASE_EN);    
    vIO32WriteFldAlign(OS_DITHER_00, 0, REG_FPHASE_R);           // phase select for running order.
                                                                //  0: internal control register from reg_fphase[5:0]
                                                                //  1: LFSR

    vIO32WriteFldAlign(OS_DITHER_00, 3, DITHER_FPHASE);             // Running order dithering frame phase increment.
                                                                // 1. If ROUND_EN = 1, the output will only rounding only 
                                                                // (no matter what RDITHER_EN and EDITHER_EN setting)
                                                                // 2. RDITHER_EN and EDITHER_EN can work together.
    vIO32WriteFldAlign(OS_DITHER_00, 1, REG_FPHASE_SEL);         // fphase XOR function bit. Only bit2,bit4,bit6 is available.                                                                
    vIO32WriteFldAlign(OS_DITHER_00, 1, REG_FPHASE_BIT);         // fphase_bit XOR bus number. 
                                                                // Note: this function will check reg_fphase_sel.
                                                                // Only 0,1,2,4,6 is available."                     

    // 2. OD Dither
    // 2.1 Set Dither Type
    if (u4PanelDither != 0xffffffff)
    {
#ifndef CC_MTK_LOADER                
        if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else        
        if(0)
#endif
        {        
            u4LsbOff = (u4PanelDither & TCON_LSBOFF_FUNCTION)? 1: 0; 
            u4Round = (u4PanelDither & TCON_ROUND_FUNCTION)? 1: 0;
            u4RDither = (u4PanelDither & TCON_RDITHER_FUNCTION)? 1 : 0;
            u4EDither = (u4PanelDither & TCON_ERROR_DITHER_FUNCTION)? 1 : 0;
            u4LFSR = (u4PanelDither & TCON_LFSR_FUNCTION)? 1 : 0;
            u4Subpxl = (u4PanelDither & TCON_SUB_PIXEL_FUNCTION)? 1 : 0;
        }
        else
        {
            u4LsbOff = 0;
            u4Round = 0;
            u4RDither = 0;
            u4EDither = 0;
            u4LFSR = 0;        
            u4Subpxl = 0;  
        }
    }
    else    // No Define
    {
#ifndef CC_MTK_LOADER                
        if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else        
        if(0)
#endif
        {        
            u4LsbOff = 1;
            u4Round = 0;
            u4RDither = 1;
            u4EDither = 0;
            u4LFSR = 0;        
            u4Subpxl = 1;        
        }
        else
        {
            u4LsbOff = 0;
            u4Round = 0;
            u4RDither = 0;
            u4EDither = 0;
            u4LFSR = 0;        
            u4Subpxl = 0;  
        }
    }

    //vIO32WriteFldAlign(OS_DITHER_05, u4LsbOff, REG_LSB_OFF_POST);
    //vIO32WriteFldAlign(OS_DITHER_03, u4Round, REG_ROUND_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4RDither, REG_RDITHER_EN_POST);        
    //vIO32WriteFldAlign(OS_DITHER_03, u4EDither, REG_EDITHER_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4LFSR, REG_LFSR_EN_POST);      
    //vIO32WriteFldAlign(OS_DITHER_03, u4Subpxl, REG_SUBPIX_EN_POST);    

    // 2.2 Common Dither Setting
    //vIO32WriteFldAlign(OS_DITHER_03, 0, REG_SUB_R_POST);
    //vIO32WriteFldAlign(OS_DITHER_03, 1, REG_SUB_G_POST);        
    //vIO32WriteFldAlign(OS_DITHER_03, 2, REG_SUB_B_POST);
    
    vIO32WriteFldAlign(OS_DITHER_03, SV_TRUE, REG_FPHASE_EN_POST);    
    vIO32WriteFldAlign(OS_DITHER_03, 0, REG_FPHASE_R_POST);      // phase select for running order.
                                                                //  0: internal control register from reg_fphase[5:0]
                                                                //  1: LFSR

    vIO32WriteFldAlign(OS_DITHER_03, 3, REG_FPHASE_POST);        // Running order dithering frame phase increment.
                                                                // 1. If ROUND_EN = 1, the output will only rounding only 
                                                                // (no matter what RDITHER_EN and EDITHER_EN setting)
                                                                // 2. RDITHER_EN and EDITHER_EN can work together.
    vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_SEL_POST);    // fphase XOR function bit. Only bit2,bit4,bit6 is available.                                                                
    vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_BIT_POST);    // fphase_bit XOR bus number. 
                                                                // Note: this function will check reg_fphase_sel.
                                                                // Only 0,1,2,4,6 is available."    


#ifdef __MODEL_slt__    
    vIO32WriteFldAlign(OS_DITHER_02, 0, DRMOD_R);
#endif
}


void vDrvOutputStageInit(void)
{

    #ifdef CC_SCPOS_3DTV_SUPPORT
	UINT32 u4EnPair;
    #endif

    #if (SRGB_FUNCTION_EN == SRGB_EN_ON)
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 1, FLD_DEN_EN);
    }
    #else
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 0, FLD_DEN_EN);
    }
    #endif

    #if (DEN_TYPE == DEN_ACIVE)
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 1, FLD_DENTPY);
    }
    #else
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 0, FLD_DENTPY);
    }
    #endif

    #if (DEN_POLARITY == DEN_LOW)
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 1, FLD_DENP);
    }
    #else
    {
        vIO32WriteFldAlign(OSTG_PTGEN_00, 0, FLD_DENP);
    }
    #endif

    vIO32WriteFldAlign(OSTG_PTGEN_00, 0, FLD_HSYNCP);
    vIO32WriteFldAlign(OSTG_PTGEN_00, 0, FLD_VSYNCP);

//    vDrvVideoDitherInit();

    // Config HV alignment
    //vIO32WriteFldAlign(OUTSTG_OS_08, 3, OS_08_REG_VSYNC_OUT_SEL);

    if ((DISP_BIT == DISP_30BIT)
        #ifdef DRV_SUPPORT_EXTMJC
        || (IS_COMPANION_CHIP_ON() && (vDrvGetCurLvdsMode() !=  eLvdsVidNormal))
        #endif
        )
    {
        vIO32WriteFldAlign(LVDSB_REG06, 1, RG_10B_EN);                
    }
    else
    {
        vIO32WriteFldAlign(LVDSB_REG06, 0, RG_10B_EN);                
    }

#ifdef CC_SCPOS_3DTV_SUPPORT
		if (DRVCUST_PanelQuery(eDISP_RLVDSEncodeLink, &u4EnPair) == 0)
		{
		  u1SetDISP_REncodeEn((UINT8)u4EnPair);
		}	 
		else
		{
		 vIO32WriteFldAlign(LVDSB_REG06, 0x1, RG_RES_FLD);
		} 
		vIO32WriteFldAlign(OSTG_01, 0x1, REG_3DTV_OUT_SEL);
		if(IS_DISPR_ENCODEL4)
		u1SetDISP_REncodeEnHW_L4(1);
#endif


    // scaler timing initial
    /*  cobra don't have SET_DATA_POS_SEL
	if (fgIsMJCToOSTG())
    vIO32WriteFldAlign(PSPIP_OA_07, 1, PSPIP_OA_07_SCALER_SET_DATA_POS_SEL);
    else
    vIO32WriteFldAlign(SCPIP_PIP_OA_07, 1 ,PIP_OA_07_SCALER_SET_DATA_POS_SEL);			
    */
    // local dimming initialization
    #ifdef CC_MTK_LOADER
    // init local dimming panel with default setting.
    #else
    
    if (fgIsSWRegInit()!=SV_FALSE)
    {
        if(PANEL_IsSupportLocalDimming())
        {
            if(bDrvLcdimSpiInit() == SV_FAIL)
            {
                bSupportLocalDimming = SV_FALSE;
            }        
            else    // PQ setting is only initialized when SPI is ready
            {
                if(bDrvLcDimPanelInit() == SV_SUCCESS)
                {
                    vDrvLcDimQualityInit();
                    bSupportLocalDimming = SV_TRUE;
                }
                else
                {
                    bSupportLocalDimming = SV_FALSE;
                }
            }
        }    
        else
        {
            bSupportLocalDimming = SV_FALSE;
        }
    }
   
    #endif

    switch(PANEL_GetDisplayType())
    {
        default:
        case PANEL_DISPLAY_TYPE_MAX:
            // error display type, defaultly do nothing
            break;
        case PANEL_DISPLAY_TYPE_LVDS:
            vIO32WriteFldAlign(TMGR5, 3, TMGR5_RG_FIFO_CTRL);
            vIO32WriteFldAlign(MLVDS_CTRL_09, 0, RG_FIFO_SEL); 
//mt5396            vIO32WriteFldAlign(MLVDS_CTRL_09, 0, RG_FIFO_SEL);			
            break;
        case PANEL_DISPLAY_TYPE_MLVDS:
            vIO32WriteFldAlign(TMGR5, 4, TMGR5_RG_FIFO_CTRL);			
            vIO32WriteFldAlign(MLVDS_CTRL_09, 1, RG_FIFO_SEL); 
//mt5396            vIO32WriteFldAlign(MLVDS_CTRL_09, 1, RG_FIFO_SEL);						
            break;
        #ifdef CC_MT5398
            case PANEL_DISPLAY_TYPE_VB1:
                vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);			
//                vIO32WriteFldAlign(FIFO_CTRL_01, 3, TCLK_FIFO_ACTIVE_PORT); 
                vIO32WriteFldAlign(TMGR2, 2, TMGR2_RG_FIFO_CK_SEL); 
                vIO32WriteFldAlign(MLVDS_CTRL_09, 2, RG_FIFO_SEL); 
//                vIO32WriteFldAlign(FIFO_CTRL_01, 1, TCLK_FIFO_START); 
                break;
        #endif
            case PANEL_DISPLAY_TYPE_EPI:
                vIO32WriteFldAlign(TMGR5, 2, TMGR5_RG_FIFO_CTRL);						
                vIO32WriteFldAlign(FIFO_CTRL_01, 5, TCLK_FIFO_ACTIVE_PORT); 
                vIO32WriteFldAlign(TMGR2, 4, TMGR2_RG_FIFO_CK_SEL); 
                vIO32WriteFldAlign(MLVDS_CTRL_09, 4, RG_FIFO_SEL); 
                vIO32WriteFldAlign(FIFO_CTRL_01, 1, TCLK_FIFO_START); 
                break;
    }
    vIO32WriteFldAlign(TMGR5, 1, TMGR5_RG_FIFO_EN);

#ifndef CC_MTK_LOADER
#ifdef CC_SCPOS_3DTV_SUPPORT 
    vSavePanelAttribute();
    fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_OSTG, (vTDTVModeChangeCB) vDrvDisp3DModeCB);
#endif
#endif

    vIO32WriteFldAlign(OSTG_01, 0, REG_MAIN_TOP_EN); // set sub plane mixer order above main
}


/**

 * @brief vDrvDisplayInit

 * Init the Display Settings

 * @param  void
 * @retval void
 */
void vDrvDisplayInit(void)
{
#if defined(SCPIP_SUPPORT_POST_SCALER)
    vPscPreInit();
#endif

    
// vpll init
    #ifndef CC_MTK_LOADER
    #ifdef NEW_COUNTRY_TYPE
    if(GetTargetCountryMask(COUNTRY_ATV_MASK)==COUNTRY_ATV_EU)
    #else
    #ifdef COUNTRY_FROM_EEPROM
    u1TargetCountryInfo = (GetTargetCountry() & COUNTRY_SCAN_ATV_MASK) >> SCAN_ATV_POS;;
    if (u1TargetCountryInfo == COUNTRY_EU)
    #else
    if(GetTargetCountry()==COUNTRY_EU)
    #endif
    #endif
    #else
    if (0)
    #endif
    {
        #ifdef CC_UBOOT
        vDDDSInit_uboot();
        #else
        vDDDSInit();    
        #endif	
        if (PANEL_IsSupport120Hz())
        {
            vDrvLCDFreqSet(SV_DCLK_50HZ, 100);
        }
        else
        {
            vDrvLCDFreqSet(SV_DCLK_50HZ, 50);
        }
    }
    else
    {
        // if panel already on, don't set again.
        if( !_fgVOPLLPowerOn)  //if(!bIsPanelOn())
        {
        #ifdef CC_UBOOT
         vDDDSInit_uboot();
        #else
         vDDDSInit();
        #endif  
        if (PANEL_IsSupport240Hz())
        {
            vDrvLCDFreqSet(SV_DCLK_60HZ, 240);
        }
        else if (PANEL_IsSupport120Hz())
            {
                vDrvLCDFreqSet(SV_DCLK_60HZ, 120);
            }
            else
            {
                vDrvLCDFreqSet(SV_DCLK_60HZ, 60);
            }
        }
        else
        {
            bSI_DISPLAY_DCLK_TYPE = SV_DCLK_60HZ;
        if (PANEL_IsSupport240Hz())
        {
            bOUTPUT_REFRESH_RATE = 240;
        }
        else if (PANEL_IsSupport120Hz())
            {
                bOUTPUT_REFRESH_RATE = 120;
            }
            else
            {
                bOUTPUT_REFRESH_RATE = 60;
            }
            _u4VPllFrequency = PANEL_GetPixelClk60Hz();//for dynamic MDR
            #ifndef CC_MTK_LOADER
            vDrvSetSSMDRAsInputHsync();//for dynamic MDR
            #ifdef CC_MT5398
            bDrvUpdateInternalFrameRate();
            #endif
            #endif
            vDrvSwitchImportProtection(0);
            #ifndef CC_MTK_LOADER
            #ifdef SUPPORT_DDDS_STEP_TRACKING
            vDrvUpdateDisplayCenterCW(IO32ReadFldAlign(DDDS_00,DDDS_00_DDDS_FREQ_CW));
            #endif
            #endif
    
        }
    }
    #ifndef CC_MTK_LOADER
    u1DrvSetForceOutputFrameRateByUser(0);
    #endif
    vDrvOutputStageInit();
    vDrvInitSCANPWM(DRVCUST_PanelGet(ePanelBacklightPwmPort));

    //SET main/pip mute works immediately
    vIO32WriteFldAlign(MUTE_00, 1 ,R_MUTE_POST_SEL);
    vIO32WriteFldAlign(MUTE_00, 1 ,R_P_MUTE_FRONT_SEL);
    vIO32WriteFldAlign(MUTE_00, 1 ,R_M_MUTE_FRONT_SEL);       
    // disable video plane
    //vIO32WriteFldAlign(SCPIP_PIP1_02, 0, PIP1_02_RVL_1);
    //vIO32WriteFldAlign(SCPIP_PIP2_02, 0, PIP2_02_RVL_2);
}

BOOL _fgPanelDynamicChg = 0;

void PanelSupportDynamicChg(BOOL fgEnable)
{
  	#ifdef SUPPORT_PANEL_DYNAMIC_CHANGE
 _fgPanelDynamicChg = fgEnable;
  	#endif
}
void vDrvLCDSetFrame(UINT16 wFrameRate)
{
#ifdef SUPPORT_PANEL_DYNAMIC_CHANGE
	if(_fgPanelDynamicChg)
	{
     bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
	} 
#endif

    if (wFrameRate==50)
    {
        vDrvLCDFreqSet(SV_DCLK_50HZ, 50);
    }
    else if (wFrameRate==60)
    {
        vDrvLCDFreqSet(SV_DCLK_60HZ, 60);
    }
    else if (wFrameRate==100)
    {
        vDrvLCDFreqSet(SV_DCLK_50HZ, 100);
    }
    else if (wFrameRate==120)
    {
        vDrvLCDFreqSet(SV_DCLK_60HZ, 120);
    }
    #ifdef SUPPORT_PANEL_48HZ
    else if ((wFrameRate == 48) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
    {
        vDrvLCDFreqSet(SV_DCLK_48HZ, wFrameRate);
    }
    #endif
    else
    {
        vDrvLCDFreqSet(SV_DCLK_OTHERS, (UINT8)wFrameRate);
    }
	#ifdef SUPPORT_PANEL_DYNAMIC_CHANGE
    if (_fgPanelDynamicChg)
	{
        vDrvMJCSetModeChangeTrig();
        vDrvSetErrorLimit(0);
    } 
	#endif
}


#ifndef CC_MTK_LOADER
/**
 * @brief vDrvUpdateInternalFrameRate

 * update internal frame rate between up scalar and MJC

 * @param  void
 * @retval update or not
 */
UINT8 bDrvUpdateInternalFrameRate(void)
{
    #ifdef CC_MT5398
        UINT32 u4PCLK_Sel;

        if (fgIsMJCToOSTG())
        {
            u4PCLK_Sel = 2; // poclk_dpll
        }
        else
        {
            u4PCLK_Sel = 0; // oclk
        }
        vIO32WriteFldAlign(CKGEN_DISP_CKCFG, u4PCLK_Sel, FLD_POCLK_SEL);                                   
    #endif

    return SV_TRUE;
}
#endif // CC_MTK_LOADER

//get internal frame rate between Front scaler and MJC
UINT16 u2DrvGetInternalFrameRate(void)
{
    #ifdef CC_MT5398
    return _u2IntFrameRate;
    #else
    return (UINT16)bOUTPUT_REFRESH_RATE;
    #endif
}

//set internal frame rate between Front scaler and MJC
void vDrvSetInternalFrameRate(UINT16 frameRate)
{
    #if defined(CC_MT5398)
    _u2IntFrameRate = frameRate;
    #else
    //no-nothing
    #endif
}

/**
 * @brief vDrvLCDFreqSet

 * wDCLK Setting, MTK Good Dclk ON/OFF and the Display Output Timing for LCD panel

 * @param  bDCLKType: SV_DCLK_50HZ/SV_DCLK_60HZ
 * @param  bFrameRate: frame rate
 * @retval void
 */
void vDrvLCDFreqSet(UINT8 bDCLKType, UINT8 bFrameRate)    // called by Video
{
#ifndef SUPPORT_PANEL_SCAN_PWM
    static BOOL fgInit = FALSE;
#endif
    UINT32 dwDCLK = PANEL_GetPixelClk60Hz();
    UINT32 u4Ratio;

    LOG(3, "[New]vDrvLCDFreqSet(bDCLKType=%d,bFrameRate=%d)\n",bDCLKType,bFrameRate);
    LOG(3, "[Old]vDrvLCDFreqSet(bSI_DISPLAY_DCLK_TYPE=%d,bOUTPUT_REFRESH_RATE=%d)\n",bSI_DISPLAY_DCLK_TYPE,bOUTPUT_REFRESH_RATE);

    if ((bDCLKType == bSI_DISPLAY_DCLK_TYPE)
        #ifndef CC_MTK_LOADER		
        && ((bOUTPUT_REFRESH_RATE == bFrameRate)) 
        // for 3D case, there might be FHD60 <-> 720p120 switching with same DCLKType
        //              or 24/48/60 with DCLK_60HZ type. 
        // || (bDCLKType != SV_DCLK_OTHERS))
        #endif        
        )
    {
        #ifndef CC_MTK_LOADER		
        bDrvUpdateInternalFrameRate();
        vDrvSetSSMDRAsInputHsync();//for dynamic MDR
        #endif        
        return;
    }

    // Update Current DCLK Type
    bSI_DISPLAY_DCLK_TYPE = bDCLKType;
    if (PANEL_IsSupport240Hz())
    {
        u4Ratio = 4;
    }
    else if (PANEL_IsSupport120Hz())
    {
        u4Ratio = 2;
    }
    else
    {
        u4Ratio = 1;
    }

    //  wDCLK  Setting
    switch(bDCLKType)
    {
    case SV_DCLK_50HZ:
		if ((bFrameRate == 25) && (DRVCUST_PanelGet(ePanel242530pOutput) == 1))
		{
		 dwDCLK = (PANEL_GetPixelClk50Hz() /(50*u4Ratio)) * bFrameRate;
		 bOUTPUT_REFRESH_RATE = (UINT8)(bFrameRate * u4Ratio);
		}
		else
		{
         dwDCLK = PANEL_GetPixelClk50Hz();
         bOUTPUT_REFRESH_RATE = (UINT8)(50 * u4Ratio);
		}
        bSI_DISPLAY_DCLK_TYPE = SV_DCLK_50HZ;
        #ifndef CC_MTK_LOADER
        #ifdef CC_DRV_SUPPORT_SMICRO
        vSMicroPreSetFreq(FREQ_50HZ);
        #endif
        #endif        
        break;

    case SV_DCLK_60HZ:
		if ((bFrameRate == 30) && (DRVCUST_PanelGet(ePanel242530pOutput) == 1))
		{
		 dwDCLK = (PANEL_GetPixelClk60Hz() /(60*u4Ratio)) * bFrameRate;
		 bOUTPUT_REFRESH_RATE = (UINT8)(bFrameRate * u4Ratio);
		}
		else
		{
         dwDCLK = PANEL_GetPixelClk60Hz();
         bOUTPUT_REFRESH_RATE = (UINT8)(60 * u4Ratio);
		}
        bSI_DISPLAY_DCLK_TYPE= SV_DCLK_60HZ;
        #ifndef CC_MTK_LOADER
        #ifdef CC_DRV_SUPPORT_SMICRO
        vSMicroPreSetFreq(FREQ_60HZ);
        #endif
        #endif        
        break;
#ifndef CC_MTK_LOADER	
    #ifdef SUPPORT_PANEL_48HZ
    case SV_DCLK_48HZ:  
	    if (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
        {   
            if(PANEL_IsSupportExtAttribute()!=0)
            {		
    		if ((bFrameRate == 24) && (DRVCUST_PanelGet(ePanel242530pOutput) == 1))
    		{
    		 dwDCLK = (PANEL_GetPixelClk48Hz() /(48*u4Ratio)) * bFrameRate;
    		 bOUTPUT_REFRESH_RATE = (UINT8)(bFrameRate * u4Ratio);
    		}
    		else
    		{
             dwDCLK = PANEL_GetPixelClk48Hz();
             bOUTPUT_REFRESH_RATE = (UINT8)(48 * u4Ratio);
    		}
            }
            else
            {
                /* panel table lacks of the 48hz setting, keep 60hz refresh rate */
                dwDCLK = PANEL_GetPixelClk60Hz();
                bOUTPUT_REFRESH_RATE = (UINT8)(60 * u4Ratio);
            }
            bSI_DISPLAY_DCLK_TYPE= SV_DCLK_48HZ;		
            #ifdef CC_DRV_SUPPORT_SMICRO
            vSMicroPreSetFreq(FREQ_48HZ);
            #endif
            break;
        }
    #endif
    case SV_DCLK_OTHERS:
		// SUPPORT Non-Standard signal
		if ((bFrameRate<SUPPORT_IN_LOW_FRAME_RATE) && (DRVCUST_PanelGet(ePanel242530pOutput) == 0)) 
		bOUTPUT_REFRESH_RATE = bFrameRate *2;
		else 	
        bOUTPUT_REFRESH_RATE = bFrameRate;
        #ifdef SUPPORT_PANEL_48HZ
        if (((bOUTPUT_REFRESH_RATE < (UINT8)(24 * u4Ratio))&& (fgDrvSupport24Hz())) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
        {   
            if(DRVCUST_PanelGet(ePanel242530pOutput) == 1)
            dwDCLK = (PANEL_GetPixelClk48Hz() / 2 /(24*u4Ratio)) * bOUTPUT_REFRESH_RATE;			
			else
			dwDCLK = (PANEL_GetPixelClk48Hz() /(24*u4Ratio)) * bOUTPUT_REFRESH_RATE;
			
		#ifndef CC_MTK_LOADER
		#ifdef CC_DRV_SUPPORT_SMICRO
            vSMicroPreSetFreq(FREQ_48HZ);
		#endif
		#endif		  
        }
		else
        #endif
        if (bOUTPUT_REFRESH_RATE < (UINT8)(SUPPORT_IN_LOW_FRAME_RATE * u4Ratio))
        {
            if(DRVCUST_PanelGet(ePanel242530pOutput) == 1)
			dwDCLK = (PANEL_GetPixelClk60Hz() /2 /(30*u4Ratio)) * bOUTPUT_REFRESH_RATE;
			else
            dwDCLK = (PANEL_GetPixelClk60Hz() /(30*u4Ratio)) * bOUTPUT_REFRESH_RATE;
			
			#ifndef CC_MTK_LOADER
			#ifdef CC_DRV_SUPPORT_SMICRO
            vSMicroPreSetFreq(FREQ_60HZ);
			#endif
			#endif		  
        }
		else							
        if (bOUTPUT_REFRESH_RATE < (UINT8)(55 * u4Ratio))
        {
            dwDCLK = (PANEL_GetPixelClk50Hz() /(50*u4Ratio)) * bOUTPUT_REFRESH_RATE;
            #ifndef CC_MTK_LOADER
            #ifdef CC_DRV_SUPPORT_SMICRO
            vSMicroPreSetFreq(FREQ_50HZ);
            #endif
            #endif        
        }
        else
        {
            dwDCLK = (PANEL_GetPixelClk60Hz() /(60*u4Ratio)) * bOUTPUT_REFRESH_RATE;
            #ifndef CC_MTK_LOADER
            #ifdef CC_DRV_SUPPORT_SMICRO
            vSMicroPreSetFreq(FREQ_60HZ);
            #endif
            #endif        
        }
        bSI_DISPLAY_DCLK_TYPE= SV_DCLK_OTHERS;
        break;
#endif // end of #ifndef CC_MTK_LOADER		
    default:
        break;
    }

    LOG(3, "bSI_DISPLAY_DCLK_TYPEe=%d\n",bSI_DISPLAY_DCLK_TYPE);
    LOG(3, "bOUTPUT_REFRESH_RATE=%d\n",bOUTPUT_REFRESH_RATE);    
#ifndef SUPPORT_PANEL_SCAN_PWM
    if (fgInit)
    {
        BeforeOutputFrameRateChange(bOUTPUT_REFRESH_RATE);
    }
#endif

    vDrvSwitchMTKGoodDclk(SV_OFF);  //turn off DDDS 8202 (Dragon)

    vDrvSetLCDTiming();

#ifndef CC_MTK_LOADER
#ifdef CC_SCPOS_3DTV_SUPPORT 
    if( u4DrvGetDisp3DModeDclk() != 0)
    {
        dwDCLK = u4DrvGetDisp3DModeDclk();
        bOUTPUT_REFRESH_RATE = u1DrvGetDisp3DModeRefreshRate();
    }
#endif
#endif

#ifdef CC_MTK_LOADER
    vDrvVOPLLFreqSet(dwDCLK);
#else
    vDrvVOFreqSet(dwDCLK);
    vDrvSetSSMDRAsInputHsync();//for dynamic MDR
#endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
#if !defined(SUPPORT_CW_CHANGE_BY_STEP) && !defined(CC_MTK_LOADER)
    // store the original control word
    i4OrigControlWord = IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW);
#endif
#endif

    if (_fgFRS_ENA == SV_ON)
    {
        vDrvSwitchMTKGoodDclk(SV_ON);
    }
#ifndef SUPPORT_PANEL_SCAN_PWM
    if (fgInit)
    {
        AfterOutputFrameRateChange();
    }
#endif
    #ifndef CC_MTK_LOADER
    #ifdef CC_DRV_SUPPORT_SMICRO
    {
        SMICRO_VERTICAL_FREQ eFreq;
        SMICRO_MSG rMsg;       
        #ifdef SUPPORT_PANEL_48HZ
        if ((bOUTPUT_REFRESH_RATE == (UINT8)(48 * u4Ratio)) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
        {
            eFreq = FREQ_48HZ;
        }
        else 
        #endif
        if (bOUTPUT_REFRESH_RATE < (UINT8)(55 * u4Ratio))
        {
            eFreq = FREQ_50HZ;
        }
        else
        {
            eFreq = FREQ_60HZ;
        }
        rMsg.u4ModuleId = MODULEID_PANEL_PARAM;
        rMsg.u4CommandId = CMD17_SET_VERTICAL_FREQ;
        rMsg.aPrm[0] = (UINT8)eFreq;
        rMsg.pCallbackFcn = NULL;
        vSMicroSendEvent(rMsg);  
    }
    #endif
    #endif        
#ifndef SUPPORT_PANEL_SCAN_PWM
    if (!fgInit)
    {
        fgInit = TRUE;
    }
#endif
}

/**
 * @brief wDrvCalOutputVTotal
 *
 * When DISP_MODE_USE is DISP_MODE_ON, Get the recommended V_TOTAL with additional VTotal limitation.
 *
 * @param  u1OutFrameRate: Output frame rate
 * @param  u2OutVTotalMin: Minimum Output Total Lines in the Vertical Direction
 * @param  u2OutVTotalMax: Maximum Output Total Lines in the Vertical Direction
 * @retval recommended V_TOTAL
 */
UINT16 wDrvCalOutputVTotal(UINT8 u1OutFrameRate, UINT16 u2OutVTotalMin, UINT16 u2OutVTotalMax)
{
    UINT32 u4VTotalMinByPixelClock;
    UINT32 u4VTotalMaxByPixelClock;
    UINT32 u4FrameRate;
    UINT32 u4HTotalMinBySyncMin;
    UINT32 u4HTotalMaxBySyncMax;

    if ((u2OutVTotalMin < DVS_TOTAL_MIN) && (u2OutVTotalMin != SCALE_VTOTAL_DO_NOT_CARE))
    {
        u2OutVTotalMin = DVS_TOTAL_MIN;
    }

    if ((u2OutVTotalMax > DVS_TOTAL_MAX) && (u2OutVTotalMax != SCALE_VTOTAL_DO_NOT_CARE))
    {
        u2OutVTotalMax = DVS_TOTAL_MAX;
    }

    u4FrameRate = (UINT32)u1OutFrameRate;

    u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_BACKPORCH_MIN + CC_FRONTPORCH_MIN);
    u4HTotalMinBySyncMin = ((u4HTotalMinBySyncMin + 1) & 0xfffffffe);     //make as even value
    u4HTotalMaxBySyncMax = (UINT32) (wDISPLAY_WIDTH + CC_BACKPORCH_MAX + CC_FRONTPORCH_MAX);
    u4HTotalMaxBySyncMax = ((u4HTotalMaxBySyncMax + 1) & 0xfffffffe);           //make as even value       

    if (u4HTotalMinBySyncMin < DHS_TOTAL_MIN)
    {
        u4HTotalMinBySyncMin = DHS_TOTAL_MIN;
    }
    if (u4HTotalMaxBySyncMax > DHS_TOTAL_MAX)
    {
        u4HTotalMaxBySyncMax = DHS_TOTAL_MAX;
    }    

    if (u4FrameRate ==0)
    {
        return  (0);
    }
    
    u4VTotalMinByPixelClock = ((UINT32) DCLK_MIN / u4FrameRate) / u4HTotalMaxBySyncMax;
    u4VTotalMaxByPixelClock = ((UINT32) DCLK_MAX / u4FrameRate) / u4HTotalMinBySyncMin;

    if ((u2OutVTotalMin < u4VTotalMinByPixelClock) && (u2OutVTotalMin != SCALE_VTOTAL_DO_NOT_CARE))
    {
        u2OutVTotalMin = u4VTotalMinByPixelClock;
    }

    if ((u2OutVTotalMax > u4VTotalMaxByPixelClock) && (u2OutVTotalMax != SCALE_VTOTAL_DO_NOT_CARE))
    {
        u2OutVTotalMax = u4VTotalMaxByPixelClock;
    }

    if (u2OutVTotalMin == SCALE_VTOTAL_DO_NOT_CARE)
    {
        u2OutVTotalMin = DVS_TOTAL_MIN;
    }   
    if (u2OutVTotalMax == SCALE_VTOTAL_DO_NOT_CARE)
    {
        u2OutVTotalMax = DVS_TOTAL_MAX;
    }

    if (u2OutVTotalMin < u2OutVTotalMax)
    {
        u2OutVTotalMin = ((u2OutVTotalMax+u2OutVTotalMin)>>1);
    }
    else
    {
        u2OutVTotalMin = 0;
    }

    return u2OutVTotalMin;
}


UINT8 vDrvGetLCDFreq(void)   
{
	return bOUTPUT_REFRESH_RATE;
}

/**
 * @brief vDrvSetDynamicDisplay

 * When DISP_MODE_USE is DISP_MODE_ON:
 * According to V_TOTAL That the Scaler Driver Calculates and HLEN_TOTAL_TYPI, Calculate Output Clock.
   If BiMode is SV_SUCCESS, Set Video Output Clock and the New Display Output Timing.

 * @param  BiMode: SV_SUCCESS/SV_FAIL
 * @retval void
 */
 #ifndef CC_MTK_LOADER
void  vDrvSetDynamicDisplay(UINT8 BiMode)
{
        static BOOL fgPreviousMode = FALSE;


    if (fgPreviousMode == BiMode)
    {
        if (BiMode)
        {
        }
        else
        {
            return;
        }
    }    

    if (BiMode)
    {
        UINT32 u4PixelClock;
        UINT32 u4HSyncLen;
        UINT32 u4HTotalFinal;
        UINT32 u4VTotalFinal;

        UINT32 u4RefreshRate;
        UINT32 u4VTotal;
        UINT32 u4HTotal;

        UINT32 u4HTotalMinBySyncMin;
        UINT32 u4HTotalMinByPixelClock;
        UINT32 u4HTotalMaxByPixelClock;
        UINT32 wOldHTotal, wOldVTotal;

        u4RefreshRate = (UINT16) bDrvVideoGetRefreshRate(SV_VP_MAIN);
        u4VTotal = (UINT32) u2ScpipCalculateOutputVTotal();

        if ((u4VTotal < DVS_TOTAL_MIN) ||
            (u4VTotal > DVS_TOTAL_MAX) ||
            (u4RefreshRate == 0))
        {

            if (WFB_PANEL_RESET == WFB_PANEL_RESET_ON)
            {
                LVDS_SwitchDataEnable(TRUE);
            }

            return;
        }
        
        u4HTotalMinByPixelClock = (((UINT32) DCLK_MIN) / u4RefreshRate) / u4VTotal;
        u4HTotalMinByPixelClock = ((u4HTotalMinByPixelClock + 1) & 0xfffffffe);     //make as even value
        u4HTotalMaxByPixelClock = (((UINT32) DCLK_MAX) / u4RefreshRate) / u4VTotal;
        u4HTotalMaxByPixelClock = ((u4HTotalMaxByPixelClock - 1) & 0xfffffffe);     //make as even value
        u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_BACKPORCH_MIN + CC_FRONTPORCH_MIN);
        u4HTotalMinBySyncMin = ((u4HTotalMinBySyncMin + 1) & 0xfffffffe);           //make as even value

#ifdef CC_HTOTAL_4X_LIMIT_WA
        if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
        {
            u4HTotalMinByPixelClock = (u4HTotalMinByPixelClock + 3) & (~3);
            u4HTotalMinBySyncMin = (u4HTotalMinBySyncMin + 3) & (~3);
            u4HTotalMaxByPixelClock = u4HTotalMaxByPixelClock & (~3);
        }
#endif

        // Find H-Total such that,
        // 1) Bigger than H-Total MIN calculate by DCLK_MIN
        // 2) Small than H-Total MAX calculate by DCLK_MAX
        // 3) Bigger than DHS_TOTAL_MIN
        // 4) Small than DHS_TOTAL_MAX
        // 5) Bigger than H-Total MIN calculate by CC_BACKPORCH_MIN
        // 6) Close to HLEN_TOTAL_TYPI

#ifdef CC_HTOTAL_4X_LIMIT_WA
        if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//        if (LVDS_OUTPUT_PORT == DUAL_PORT)
        {
            u4HTotal = (UINT32) ((HLEN_TOTAL_TYPI) + 3) & (~3);
        }
        else
#endif
        {
            u4HTotal = (UINT32) (HLEN_TOTAL_TYPI);
        }

        if (u4HTotal > u4HTotalMaxByPixelClock)
        {
            u4HTotal = u4HTotalMaxByPixelClock;
        }

#ifdef CC_HTOTAL_4X_LIMIT_WA
        if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//        if (LVDS_OUTPUT_PORT == DUAL_PORT)
        {
            if (u4HTotal > (UINT32) ((DHS_TOTAL_MAX ) & (~3)))
            {
                u4HTotal = (UINT32)((DHS_TOTAL_MAX ) & (~3));
            }
        }
        else
#endif
        {
            if (u4HTotal > (UINT32)(DHS_TOTAL_MAX ))
            {
                u4HTotal = (UINT32) (DHS_TOTAL_MAX );
            }
        }

        if (u4HTotal < u4HTotalMinByPixelClock)
        {
            u4HTotal = u4HTotalMinByPixelClock;
        }

        if (u4HTotal < u4HTotalMinBySyncMin)
        {
            u4HTotal = u4HTotalMinBySyncMin;
        }

        if (u4HTotal < (UINT32) (DHS_TOTAL_MIN ))
        {
            u4HTotal = (UINT32) (DHS_TOTAL_MIN );
        }

        // Pixel Clock
        u4PixelClock = (u4RefreshRate * u4VTotal) * u4HTotal;

        // [DTV00111681] Update related register and Do OSD_PLA_Mute only when u4VTotal or u4HTotal changed.
        // Peggy 20080109
        wOldHTotal = wDrvGetOutputHTotal();
        wOldVTotal = wDrvGetOutputVTotal(); 
        	
        if ((wOldHTotal != u4HTotal) || (wOldVTotal != u4VTotal))
        {        	
            if (WFB_PANEL_RESET == WFB_PANEL_RESET_ON)
            {
                LVDS_SwitchDataEnable(FALSE);
            }
    
            vDrvVOFreqSet(u4PixelClock);
           #ifndef CC_MTK_LOADER		
           vDrvSetSSMDRAsInputHsync();//for dynamic MDR
	    #endif	
    
            // HSync Length
            u4HSyncLen = (u4HTotal - wDISPLAY_WIDTH) ;
            if (u4HSyncLen >= CC_BACKPORCH_MAX)
            {
                u4HSyncLen = CC_BACKPORCH_MAX ;
            }
    
            // H Total
            u4HTotalFinal = u4HTotal ;
    
            // V Total
            u4VTotalFinal = u4VTotal ;

            wDrvSetOutputVTotal(u4VTotalFinal);
            #ifndef CC_COPLAT_MT82
			wDrvSetOutputHTotal(u4HTotalFinal);
            #else
			wDrvSetOutputHTotal(u4HTotalFinal);
            #endif
			wDrvSetOutputHActive(wDISPLAY_WIDTH);
    
            LOG(1, "DM OK HS(%d)\n", u4HSyncLen);
    
        }
        vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
        vDrvSwitchMTKGoodDclk(SV_ON);    
        bSI_DISPLAY_DCLK_TYPE = SV_DCLK_OTHERS;
    }
    else
    {
        // FIXME
        // Call vDDDSLCDSetting() instead
        if (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 50)
        {
            vDrvLCDFreqSet(SV_DCLK_50HZ, (PANEL_IsSupport120Hz()?100 : 50));
        }
        else if (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 25)
        {
            if (DRVCUST_PanelGet(ePanel242530pOutput))
            {
                vDrvLCDFreqSet(SV_DCLK_OTHERS, 25);
            }
            else
            {                
                vDrvLCDFreqSet(SV_DCLK_50HZ, (PANEL_IsSupport120Hz()?100 : 50));
            }
        }
        else if (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 60)
        {
            vDrvLCDFreqSet(SV_DCLK_60HZ, (PANEL_IsSupport120Hz()?120 : 60));
        }
        else if (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 30)
        {
            if (DRVCUST_PanelGet(ePanel242530pOutput))
            {
                vDrvLCDFreqSet(SV_DCLK_OTHERS, 30);
            }
            else
            {                
                vDrvLCDFreqSet(SV_DCLK_60HZ, (PANEL_IsSupport120Hz()?120 : 60));
            }
        }
        else if (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 24)
	 {
            if (DRVCUST_PanelGet(ePanel242530pOutput))
            {
                vDrvLCDFreqSet(SV_DCLK_OTHERS, 24);
            }
            else
			{
			    #ifdef SUPPORT_PANEL_48HZ
			    if(u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
                {         
    				if(fgDrvSupport24Hz())
    				{
    					vDrvLCDFreqSet(SV_DCLK_48HZ, (PANEL_IsSupport120Hz()?96 : 48));
    				}
    				else
    				{
    			    #ifndef CC_MTK_LOADER				
    				#ifdef NEW_COUNTRY_TYPE
    				    if(GetTargetCountryMask(COUNTRY_ATV_MASK)==COUNTRY_ATV_EU)
    				#else
    				#ifdef COUNTRY_FROM_EEPROM
    				    u1TargetCountryInfo = (GetTargetCountry() & COUNTRY_SCAN_ATV_MASK) >> SCAN_ATV_POS;;
    				    if (u1TargetCountryInfo == COUNTRY_EU)
    				#else
    				    if(GetTargetCountry()==COUNTRY_EU)
    				#endif
    				#endif
    			    #else
    				    if(0)
    			    #endif	
    				    {
    					    vDrvLCDFreqSet(SV_DCLK_50HZ, (PANEL_IsSupport120Hz()?100 : 50));
    				    }
    				    else
    				    {				
    					    vDrvLCDFreqSet(SV_DCLK_60HZ, (PANEL_IsSupport120Hz()?120 : 60));
    				    }
    			    }
                }
			    else
                #endif
                {         
				    vDrvLCDFreqSet(SV_DCLK_60HZ, (PANEL_IsSupport120Hz()?120 : 60));
                }
	 	}
	}
            vDrvSwitchMTKGoodDclk(SV_OFF);
    }
	
    if (WFB_PANEL_RESET == WFB_PANEL_RESET_ON)
    {
        LVDS_SwitchDataEnable(TRUE);
    }

    fgPreviousMode = BiMode;

}
#endif // end of #ifndef CC_MTK_LOADER


void OSTG_SetPixelShift(BOOL fgEnable, INT32 i4HPixelShift, INT32 i4VPixelShift)
{
    BOOL fgOriPixelShiftEnable;
    
    if ((ABS(i4HPixelShift) > PIXEL_SHIFT_H_MAX) || (ABS(i4VPixelShift) > PIXEL_SHIFT_V_MAX))
    {
        //LOG(1, "pixel shift out of range, i4HPixelShift=%d, i4VPixelShift=%d\n",  i4HPixelShift, i4VPixelShift);
        return;
    }
    //LOG(5, "PMX_SetPixelShift, i4HPixelShift=%d, i4VPixelShift=%d\n",  i4HPixelShift, i4VPixelShift);

    fgOriPixelShiftEnable =  PANEL_IsPixelShift();
    PANEL_SetPixelShift(fgEnable, i4HPixelShift, i4VPixelShift);
     
    // turn on pixel shift
    if (fgEnable)
    {
//        _i4HPixelShift = i4HPixelShift;
        if (!fgOriPixelShiftEnable)
        {
        #ifndef CC_COPLAT_MT82
            IGNORE_RET(OSD_UpdateTiming());
        #endif
            //LOG(5, "turn on pixel shift, width=%d, height=%d\n", PANEL_GetPanelWidth(), PANEL_GetPanelHeight());
        }
        _u4OstgUpdateEvent |= OSTG_UPDATE_PIXEL_SHIFT_H;
        _u4OstgUpdateEvent |= OSTG_UPDATE_PIXEL_SHIFT_V;
    }
    else if (!fgEnable && fgOriPixelShiftEnable)     // previous is enable, restore timing
    {
        vDrvSetVsyncBp(u4GetPanelVSyncBp());
        #ifndef CC_COPLAT_MT82
        IGNORE_RET(OSD_UpdateTiming());
        #endif

      //  LOG(5, "turn off pixel shift, width=%d, height=%d\n", PANEL_GetPanelWidth(), PANEL_GetPanelHeight());
    }
}

void Panel_DISPR_FIELD_Delay(UINT8 u1Tpye, UINT8 u1AlignType,UINT16 u1DelayLine)	
{
 if (u1Tpye == eDispRFieldAlign_S_Field)
 {

 }
 else
 {
	#if 0
   //#if defined(CC_MT5368) || defined(CC_MT5389)
   if ((BSP_GetIcVersion() >= IC_VER_5368_AB)||(BSP_GetIcVersion() >= IC_VER_5389_AB))
   {
    vIO32WriteFldAlign(VINTF_P19, u1AlignType, VINTF_DISP_R_ALLIGN_TYPE);
   } 
   else
   {
    vIO32WriteFldAlign(VINTF_P19, u1AlignType, VINTF_DISP_R_ALLIGN_TYPE_NOECO);
   }
   #else
   vIO32WriteFldAlign(VINTF_P19, u1AlignType, VINTF_DISP_R_ALLIGN_TYPE);
   #endif
   
   // DISP_R delay limitaion: "0~1122" OK, "1123" need bypass 6 line buffer, 1124 can't reach
   if(u1DelayLine < (wDrvGetOutputVTotal()-2))
   {
    vIO32WriteFldAlign(VINTF_P19, u1DelayLine, VINTF_DISP_R_H_DLY);	
   }
   else
   {
    vIO32WriteFldAlign(VINTF_P19, 0, VINTF_DISP_R_H_DLY); 
   }
 }
}

void Panel_3DGlassesCTL(UINT8 u1Pin, UINT8 u1Tpye, BOOL fgPol,BOOL fgPos, UINT8 u1DelayFrame, UINT16 u2DelayLine)
{
  #if defined(CC_MT5398)
  if (u1Pin == e3DGlasses_P_3DR)
  {
   UNUSED(BSP_PinSet(PIN_GPIO52, PINMUX_FUNCTION1)); 
  }
  else if (u1Pin == e3DGlasses_P_3DL)
  {
   UNUSED(BSP_PinSet(PIN_GPIO53, PINMUX_FUNCTION1)); 
  }
  else if (u1Pin == e3DGlasses_P_3DBL)
  {
   UNUSED(BSP_PinSet(PIN_GPIO54, PINMUX_FUNCTION1)); 
  }
  // #elif defined(CC_MT5880)
  #else
  if (u1Pin == e3DGlasses_P_3DR)
  {
   UNUSED(BSP_PinSet(PIN_GPIO6, PINMUX_FUNCTION2)); 
  }
  else if (u1Pin == e3DGlasses_P_3DL)
  {
   UNUSED(BSP_PinSet(PIN_GPIO7, PINMUX_FUNCTION2)); 
  }
  else if (u1Pin == e3DGlasses_P_3DBL)
  {
   //UNUSED(BSP_PinSet(PIN_GPIO8, PINMUX_FUNCTION2)); 
  }  
  #endif

  vIO32WriteFldAlign(VINTF_P9 + (u1Pin *8), u1Tpye, VINTF_REYE_SEL);
  
  vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), fgPos, VINTF_2EYE_POL);    
  vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), fgPos, VINTF_REYE_V_POS); // 2EYE with V_POS  
  
   if (u1Tpye == e3DGlasses_S_BL)
   {
    vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), 1, VINTF_3DBL_LRLR);  
   }	
   else
   {
   vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), 1, VINTF_2EYE_LRLR);  
   }	
  
  vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), u1DelayFrame, VINTF_2EYE_V_DLY);
  vIO32WriteFldAlign(VINTF_P6 + (u1Tpye *8), u2DelayLine, VINTF_2EYE_H_DLY);
  
  if (u1Tpye == e3DGlasses_S_2EYE)
  {
   vIO32WriteFldAlign(VINTF_P7, u1Tpye, VINTF_2EYE_SEL);
   vIO32WriteFldAlign(VINTF_P7, fgPol, VINTF_2EYE_POL);
   vIO32WriteFldAlign(VINTF_P7, u1DelayFrame, VINTF_2EYE_V_DLY);
   vIO32WriteFldAlign(VINTF_P6, u2DelayLine, VINTF_2EYE_H_DLY);
  }

  if (u1Tpye == e3DGlasses_S_BL2)
  {
   vIO32WriteFldAlign(VINTF_P26, u1Tpye, VINTF_3DBL_2_SEL);
   vIO32WriteFldAlign(VINTF_P26, fgPol, VINTF_3DBL_2_POL);
   vIO32WriteFldAlign(VINTF_P26, 1, VINTF_3DBL_2_LRLR);   
   vIO32WriteFldAlign(VINTF_P26, u1DelayFrame, VINTF_3DBL_2_V_DLY);
   vIO32WriteFldAlign(VINTF_P25, u2DelayLine, VINTF_3DBL_2_H_DLY);
  }  
}

void Panel_3DGlassesDelay(UINT8 u1Pin, UINT16 u2DelayLine)
{
    if (u1Pin == e3DGlasses_P_3DR)
    {
        vIO32WriteFldAlign(VINTF_P8, u2DelayLine, VINTF_REYE_H_DLY);
    }
    else if (u1Pin == e3DGlasses_P_3DL)
    {
        vIO32WriteFldAlign(VINTF_P10, u2DelayLine, VINTF_LEYE_H_DLY);
    }
    else if (u1Pin == e3DGlasses_P_3DBL)
    {
        vIO32WriteFldAlign(VINTF_P12, u2DelayLine, VINTF_3DBL_H_DLY);
    }

    w3DGlassesDelay = u2DelayLine;
}

void Panel_3DGlassesRSTCTL(UINT8 u1Tpye, BOOL fgOnOff, UINT16 u2RSTLine)
{  
  vIO32WriteFldAlign(VINTF_P7 + (u1Tpye *8), fgOnOff, VINTF_2EYE_RST_MODE);    
  vIO32WriteFldAlign(VINTF_P6 + (u1Tpye *8), u2RSTLine, VINTF_2EYE_WIDTH);
}

void Panel_3DGlassesDutyCTL(UINT8 u1Tpye, UINT8 u1DutyH)
{ 
    UINT32 u4Vtotal,u4VtotalH;
	u4Vtotal = wDrvGetOutputVTotal();
	
	if (IS_PANEL_L12R12)
	{
     u4Vtotal = u4Vtotal >>1;
	}	
	
	 u4VtotalH = u4Vtotal *  u1DutyH / 100 ;


    if (u4VtotalH >= 1)
    {
      u4VtotalH = u4VtotalH - 1;
    }

    Panel_3DGlassesRSTCTL(u1Tpye, 1, u4VtotalH);
}

// fgonoff = 0 [off ] fgonoff = 1 [on ]
void Panel_3DGlassesCTLEn(UINT8 u1Pin, BOOL fgOnfff)
{
    vIO32WriteFldAlign(VINTF_P9 + (u1Pin * 8), !fgOnfff, VINTF_REYE_OP_EN);  
	LOG(0, "Panel_3DGlassesCTLEn: %d, %d\n", u1Pin, fgOnfff);
    if (e3DGlasses_P_3DBL == u1Pin)
    {
        LOG(0, "Panel_3DGlassesCTLEn: %d\n", fgOnfff);
        vIO32WriteFldAlign(VINTF_P9 + (u1Pin * 8), (fgOnfff) ? 3 : 7, VINTF_REYE_SEL);  
    }
}

void Panel_2ndChannelOnOff(BOOL fgOnfff)
{
    if (IS_PANEL_L12R12)
    {
        if (LVDS_OUTPUT_PORT == FOUR_PORT)
        {
			LOG(0, "Panel_2ndChannelOnOff: %d, %d\n", fgOnfff);
            vIO32WriteFldAlign(LVDSB_REG07, (fgOnfff)? 0x0 : 0xCFFC00, RG_PD);
        }
        else if (LVDS_OUTPUT_PORT == DUAL_PORT)
        {
			LOG(0, "Panel_2ndChannelOnOff: %d, %d\n", fgOnfff);
            vIO32WriteFldAlign(LVDSB_REG07, (fgOnfff)? 0x0 : 0xEFFFE0, RG_PD);
        }
    }
}

void vDrvVideoSetMuteColor(UINT8 bPath, UINT32 u4Bg) 
{
    RDispInfo *prDispInfo;
    prDispInfo = getDispInfo(bPath);
    
    if (prDispInfo->u4MuteColor != u4Bg)
    {
        prDispInfo->u4MuteColor = u4Bg;
        if (bPath == SV_VP_MAIN)
        {
            vSetScposFlg(MAIN_MUTECOLOR_CHG);
        }
        else
        {
            vSetScposFlg(PIP_MUTECOLOR_CHG);
        }
    }
}

/**
* @brief Output stage normproc
*/
void vOstgNormProc(void)
{
#ifndef CC_MTK_LOADER
#if 0//def SUPPORT_CW_CHANGE_BY_STEP
    static UINT32 u4Val = 0;
    static UINT32 u4LocalTargetCW = 0;
    static BOOL isIncrease;
    static BOOL isFirstStep = TRUE;

    if( (u4LocalTargetCW != _u4TargetCW) && isFirstStep )
    {
        u4Val = _u4CurrentCW; //inital the first CW
        isIncrease = (_u4TargetCW > _u4CurrentCW ) ? TRUE : FALSE;
        isFirstStep = FALSE;

        //give-up first entry, delay the pll setting to next vsync (osd muted)
        return;
    }

    if( (_u4CurrentCW == _u4TargetCW) || (u4Val == _u4TargetCW) )
    {
        u4LocalTargetCW = _u4TargetCW;
        isFirstStep = TRUE;

        return;
    }



    if( 0xfffffff == _u4CWStep )
    {
        u4Val = _u4TargetCW;
    }
    else
    {
        if (isIncrease)
        {
            u4Val = ( (u4Val+_u4CWStep) >= _u4TargetCW ) ? _u4TargetCW : (u4Val+_u4CWStep);
        }
        else
        {
            u4Val = ( (u4Val-_u4CWStep) <= _u4TargetCW ) ? _u4TargetCW : (u4Val-_u4CWStep);
        }
    }

    if( u4Val == _u4TargetCW )
    {
        u4LocalTargetCW = u4Val;
        isFirstStep = TRUE;
    }
    
    vIO32WriteFldAlign(DDDS_00,u4Val,DDDS_00_DDDS_FREQ_CW);

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
    // store the original control word
    i4OrigControlWord = u4Val;
#endif
    
    vDrvVoDclkSpreadInit(1);
    HAL_Delay_us(100);
    vDrvVoDclkSpreadInit(0);   
#endif

    
    #ifdef SUPPORT_DDDS_STEP_TRACKING
    vDrvDDDSTrackingNormProc();
    #endif
    
#ifdef SUPPORT_FREERUN_DCLK_BY_STEP 
        static INT32 i4DeltaControlWord = 0;
        if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
        {
    
        if (u1DDDSLockStatus == 1)    
        {
            if(u1SignalStatus != bDrvVideoSignalStatus(SV_VP_MAIN))
            {
                 u1SignalStatus = bDrvVideoSignalStatus(SV_VP_MAIN);
             }
            if(u1SignalStatus == SV_VDO_STABLE)
            {
               if(u1StepDelay2 < STEP_CW_DELAY2)
               {
                   u1StepDelay2++;
               }
               else
               {
                   if(_u4ErrorLimit < TargetErrlimit)
                   {
                       vDrvSetErrorLimit(_u4ErrorLimit+1);
                       LOG(2,"_u4ErrorLimit %d\n", _u4ErrorLimit);
                   }
				   vDrvSetDDDSCloseloop(1);
               }
            }
        }
    //    if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 1)    
        {
            if(u1StepDelay < STEP_CW_DELAY)
            {
                u1StepDelay++;
           }
            else
        {
            if(u1StepControlWord < STEP_CW)
            {
                u1StepControlWord++;       
    
                if(u1StepControlWord < STEP_CW && i4OrigControlWord != 0x7fffffff)
                {
                    if(u1StepControlWord == 1)
                    {
                        i4DeltaControlWord = ((INT32)IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW) - i4OrigControlWord)/STEP_CW;
                    }
                    
                    vIO32WriteFldAlign(DDDS_00, i4OrigControlWord + i4DeltaControlWord*(STEP_CW - u1StepControlWord), DDDS_00_DDDS_FREQ_CW);
                    
                }
                else if(u1StepControlWord == STEP_CW && i4OrigControlWord != 0x7fffffff)
                {
                    vIO32WriteFldAlign(DDDS_00, i4OrigControlWord, DDDS_00_DDDS_FREQ_CW);
                    vIO32WriteFldMulti(DDDS_00,P_Fld(1,DDDS_00_DISP_EN));
                    HAL_Delay_us(20);
                    vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN));
                }
    
                vIO32WriteFldAlign(DDDS_03, 1, DDDS_03_SPREAD_INIT);
                HAL_Delay_us(20);
                LOG(2,"step %d, freerun-CW: %x\n", u1StepControlWord, IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW));
                vIO32WriteFldAlign(DDDS_03, 0, DDDS_03_SPREAD_INIT);
            }
        }
        }
        }
        else if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
        {
    
    //    if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 1)    
        {
            if(u1StepDelay < STEP_CW_DELAY_EPI)
            {
                u1StepDelay++;
           }
            else
        {
            if(u1StepControlWord < STEP_CW)
            {
                u1StepControlWord++;       
    
                if(u1StepControlWord < STEP_CW && i4OrigControlWord != 0x7fffffff)
                {
                    if(u1StepControlWord == 1)
                    {
                        i4DeltaControlWord = ((INT32)IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW) - i4OrigControlWord)/STEP_CW;
                    }
                    
                    vIO32WriteFldAlign(DDDS_00, i4OrigControlWord + i4DeltaControlWord*(STEP_CW - u1StepControlWord), DDDS_00_DDDS_FREQ_CW);
                    
                }
                else if(u1StepControlWord == STEP_CW && i4OrigControlWord != 0x7fffffff)
                {
                    vIO32WriteFldAlign(DDDS_00, i4OrigControlWord, DDDS_00_DDDS_FREQ_CW);
                }
    
                vIO32WriteFldAlign(DDDS_03, 1, DDDS_03_SPREAD_INIT);
                HAL_Delay_us(20);
                LOG(2,"step %d, freerun-CW: %x\n", u1StepControlWord, IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW));
                vIO32WriteFldAlign(DDDS_03, 0, DDDS_03_SPREAD_INIT);
                if(u1StepControlWord == STEP_CW)
                {
                    if (u1DDDSLockStatus == 1)    
                        vDrvSetDDDSCloseloop(1);
                }
            }
        }
        }
        }
#endif

#endif // CC_MTK_LOADER
    return;
}

#ifndef CC_MTK_LOADER
	extern BOOL _fgVideoReady;
#endif
// OSTG Event on Output VSync
// Note: Keep this function simple since this function is called by ISR
void OSTG_OnOutputVSync(void)
{
#ifndef CC_MTK_LOADER
		static UINT8 u1ClockSkewCnt = 0;
		BOOL fgclockskew = 0;
		static BOOL fgPreClkSkew = 0;
#endif
    
#ifndef CC_MTK_PRELOADER
    if (_u4OstgUpdateEvent & OSTG_UPDATE_PIXEL_SHIFT_H)
    {
        _u4OstgUpdateEvent &= ~OSTG_UPDATE_PIXEL_SHIFT_H;
    }
    if (_u4OstgUpdateEvent & OSTG_UPDATE_PIXEL_SHIFT_V)
    {
        vDrvSetVsyncBp(u4GetPanelVSyncBp());
        _u4OstgUpdateEvent &= ~OSTG_UPDATE_PIXEL_SHIFT_V;
    }    
#endif

#ifndef CC_MTK_LOADER
    if (fgIsScposFlgSet(MAIN_MUTECOLOR_CHG))
    {
        vDrvVideoMuteColor(SV_VP_MAIN, _rMDispInfo.u4MuteColor);
        vClrScposFlg(MAIN_MUTECOLOR_CHG);
    }
    if (fgIsScposFlgSet(PIP_MUTECOLOR_CHG))
    {
        vDrvVideoMuteColor(SV_VP_PIP, _rPDispInfo.u4MuteColor);
        vClrScposFlg(PIP_MUTECOLOR_CHG);
    }

#endif 

#ifndef CC_MTK_LOADER
	if (_fgVideoReady)

	{
		fgclockskew = u1ChkLVDSClockSkewStatus(); 
		if (fgPreClkSkew!=fgclockskew)
		{
			Printf("[ESD] fgclockskew = %d \n",fgclockskew);
			fgPreClkSkew = fgclockskew;
		}
		if (fgclockskew)
		{
			//Printf("[ESD] clock skew \n");
			u1ClockSkewCnt ++;
			if (u1ClockSkewCnt > 12)
			{
				u1LVDSClockSkew_Recover();
				u1ClockSkewCnt = 0 ;
			}
				u1LVDSClockSkew_Reset();				
		}	
		else
		{
			u1ClockSkewCnt = 0;
		}
	}
#endif 
}


void vDrvSwitchImportProtection(UINT8 bOn_Off)
{
}


void vDrvFireImportPortection(void)
{
	if (fgIsMJCToOSTG())
	{
		vPscSetData(1);
		LOG(3,"PSC Set data _u4VSyncCountSD = %d \n",_u4VSyncCountSD);		
	} 
	else
	{
		vScpipSetData(1);
		LOG(3,"FSC Set data _u4VSyncCountSD = %d \n",_u4VSyncCountSD);		
	} 
}

void vDrvSetErrorLimit(UINT32 u4Error)
{
    if (u4Error != _u4ErrorLimit)
    {
        _u4ErrorLimit = u4Error;
        vIO32WriteFldAlign(DDDS_03, _u4ErrorLimit, DDDS_03_DDDS_ERR_LIM);
    }
}

#ifndef CC_MTK_LOADER
#ifdef SUPPORT_SW_FRAME_CHECK
void vSetSWFrameTrackOnOff(UINT32 u4OnOff)
{
    if (_u4VDistTrackEn != u4OnOff)
    {
        LOG(3, "SW Frame Track (%d)\n", u4OnOff);
        _u4VDistTrackEn = u4OnOff;

	 // Speed up DDDS lock when SW frame track is enable.
         Set_DDDS_KPI(u4OnOff);
    }
}

void vSetSWFrameTrackStep(UINT32 u4Step)
{
    if (_u4VDistTrackMaxStep != u4Step)
    {
        LOG(3, "SW Frame Track Step (%d)\n", u4Step);
        _u4VDistTrackMaxStep = u4Step;
    }
}

void vSetSWFrameTrackLockRange(UINT32 u4Upper, UINT32 u4Lower)
{
    if (_u4VDistUpperBound != u4Upper)
    {
        LOG(3, "SW Frame Track Upper Bound (%d --> %d)\n", _u4VDistUpperBound, u4Upper);
        _u4VDistUpperBound = u4Upper;
    }
    if (_u4VDistLowerBound != u4Lower)
    {
        LOG(3, "SW Frame Track Lower Bound (%d --> %d)\n", _u4VDistLowerBound, u4Lower);
        _u4VDistLowerBound = u4Lower;
    }
}

void vDrvVDistanceCheck(void)
{
    UINT32 u4VDistance, bmode;
    UINT32 u4LowerBound, u4UpperBound;
    UINT32 u4VTotal;
    UINT32 u4UpTrackRange, u4DownTrackRange;
    UINT8 bScaleMode =  u1ScpipGetDispMode(SV_VP_MAIN) ;
    if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 0)    
    {
        _u4UpTrackRange = 0;
        _u4DownTrackRange = 0;
        _u4VDistTrackState = 0;
        _u4VDistTrackLocked = 0;
	 LOG(6, "vDrvVDistanceCheck() : DISP_EN=0, return.\n");

        return;
    }

    if(IO32ReadFldAlign(SCPIP_PIP1_09,  PIP1_09_F_SYNC_1)==1)
    {
       LOG(6, "vDrvVDistanceCheck() : PIP1_0B_F_SYNC_1=1, return.\n");
    	return;
    }

    bmode =u1ScpipGetDispMode(SV_VP_MAIN);
    if(bmode == VDP_SCPOS_PSCAN_DISPMODE)
    {
        u4VDistance = u2DrvDIGetVcnt(VDP_1);
    }
    else
    {
        u4VDistance = u4ScpipGetInOutVDistance();
    }

    #if 1
    u4VTotal = (UINT32)wDrvGetOutputVTotal();
    
    u4LowerBound = (_u4VDistLowerBound > 0x800)? (_u4VDistLowerBound-0x800) : (u4VTotal + _u4VDistLowerBound - 0x800);
    u4UpperBound = (_u4VDistUpperBound > 0x800)? (_u4VDistUpperBound-0x800) : (u4VTotal + _u4VDistLowerBound - 0x800);

    // get tracking distance
    u4UpTrackRange = 0;
    u4DownTrackRange = 0;
    if (u4UpperBound >= u4LowerBound)
    {
        if (u4VDistance > u4UpperBound)
        {
            u4DownTrackRange = u4VDistance - u4UpperBound;
            u4UpTrackRange = u4VTotal + u4LowerBound - u4VDistance;
        }
        else if (u4VDistance < u4LowerBound)
        {
            u4DownTrackRange = u4VDistance + u4VTotal - u4UpperBound;
            u4UpTrackRange = u4LowerBound - u4VDistance;        
        }
    }
    else  // u4UpperBound < u4LowerBound
    {
        if ((u4VDistance > u4UpperBound) && (u4VDistance < u4LowerBound))
        {
            u4DownTrackRange = u4VDistance - u4UpperBound;
            u4UpTrackRange = u4LowerBound - u4VDistance;        
        }
    }

    if (_u4VDistTrackEn)
    {
        if (_u4VDistTrackLocked < 2)
        {
            if ((u4UpTrackRange > _u4VDistTorance) && (u4DownTrackRange > _u4VDistTorance))
            {
                // start tracking
                _u4VDistTrackLocked = 0;
                _u4UpTrackRange = u4UpTrackRange;
                _u4DownTrackRange = u4DownTrackRange;
                if (u4UpTrackRange > u4DownTrackRange)
                {  // down tracking
                    _u4VDistTrackState = 1;
                }
                else
                {  // up tracking
                    _u4VDistTrackState = 2;
                }
                vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN),bDrvVideoGetRefreshRate(SV_VP_MAIN));
                LOG(5, "SW Frame Track Bound(%d ~ %d), Dir(%d),Current: %d, Dist(%d, %d)\n", u4LowerBound, u4UpperBound, _u4VDistTrackState, u4VDistance, u4DownTrackRange, u4UpTrackRange);
            }
            else
            {
                _u4VDistTrackLocked ++;
                if (_u4VDistTrackLocked == 2)
                {
                    _u4VDistTrackState = 3;// SW frame track locked.
                    vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN),bDrvVideoGetRefreshRate(SV_VP_MAIN));
                    LOG(5, "SW frame track locked.\n");
                }
            }
        }
        else  if ((u4UpTrackRange > _u4VLossDistTorance) && (u4DownTrackRange > _u4VLossDistTorance))
        {
            _u4VDistTrackLocked = 0;
	     _u4VDistTrackState = 0;
	       LOG(5, "SW frame track : Loss lock.  (Range: %d, %d)\n",u4UpTrackRange, u4DownTrackRange);
        }

    }
    else
    {
        if (_u4VDistTrackState)
        {
            // recover
            _u4UpTrackRange = 0;
            _u4DownTrackRange = 0;
            _u4VDistTrackState = 0;
            _u4VDistTrackLocked = 0;
            vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN),bDrvVideoGetRefreshRate(SV_VP_MAIN));
            LOG(5, "SW frame track : Off.\n");
        }
    }

    
    #else
    
    if(u4VDistance >= V_DISTANCE_RANGE)
    {
        LOG(5,"u4VDistance(%d)\n",u4VDistance);
        u4CoarseStepSize = COARSE_STEP_SIZE;
        u4FineStepSize = FINE_STEP_SIZE;
        vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN));
    }
    else
    {   
        u4CoarseStepSize = 0;
        
        if(u4InOutVDistance != u4VDistance)
        {
            LOG(5,"IN OUT V DISTANCE CHANGED FROM (%d) TO (%d)\n",u4InOutVDistance,u4VDistance);

            if(u4VDistance < u4InOutVDistance)
            {
                if(u4CurrentStep<FINE_STEP_MAX)
                {
                    u4CurrentStep++;
                }
                else
                {
                    u4CurrentStep = FINE_STEP_MAX;
                }            
            }
            else
            {
                if(u4CurrentStep>FINE_STEP_MIN)
                {
                    u4CurrentStep--;
                }
                else
                {
                    u4CurrentStep = FINE_STEP_MIN;
                }        
            }
            
            LOG(5,"u4CurrentStep(%d)\n",u4CurrentStep);        
            
            //update current distance
            u4InOutVDistance = u4VDistance;
            
            vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN));
        }
        else
        {                
            u4NoChangeCounter++;
            if(u4NoChangeCounter == V_STABLE_COUNT)
            {
                LOG(5,"V DISTANCE IS STABLE(%d)\n",u4VDistance);
                u4NoChangeCounter = 0;
                u4CurrentStep = FINE_STEP_CENTER;
            }
        }
    }
    #endif
}

#endif
#endif

#ifdef CC_ENABLE_MTK_MODEL_INDEX
UINT8 u1UseSuperFlipMirror=0;
UINT8 u1SuperFlipMirrorCfg=0;

void vSetSuperFlipMirrorConfig(UINT8 u1SuperCfgOnOff, UINT8 u1SuperFlip, UINT8 u1SuperMirror)
{
	u1UseSuperFlipMirror =u1SuperCfgOnOff;

	if(u1SuperFlip)
	{
		u1SuperFlipMirrorCfg |= SYS_FLIP_CONFIG_ON;
	}
	else
	{
		u1SuperFlipMirrorCfg &= (~SYS_FLIP_CONFIG_ON);
	}

	if(u1SuperMirror)
	{
		u1SuperFlipMirrorCfg  |= SYS_MIRROR_CONFIG_ON;

	}
	else
	{
		u1SuperFlipMirrorCfg &= (~SYS_MIRROR_CONFIG_ON);
	}

}
#endif

#ifdef CC_FLIP_MIRROR_FROM_DDI
#ifdef CC_FLIP_MIRROR_SUPPORT
static BOOL _fgIsMirrorOn = FALSE; 
static BOOL _fgIsFlipOn = FALSE; 

UINT8 u1SetFlipMirrorConfig(BOOL fgMirrorEn, BOOL fgFlipEn)
{
    _fgIsMirrorOn = fgMirrorEn;
    _fgIsFlipOn = fgFlipEn;

    return SV_SUCCESS;
}

UINT8 u1GetFlipMirrorConfig(void)
{
    static UINT8 u1FlipMirrorEnable= 0 ;
    if (_fgIsMirrorOn)
    {
        u1FlipMirrorEnable |= SYS_MIRROR_CONFIG_ON ;
    }
    if(_fgIsFlipOn)
    {
        u1FlipMirrorEnable |= SYS_FLIP_CONFIG_ON ;
    }
    
    return u1FlipMirrorEnable;
}
#endif

#else
UINT8 u1SetFlipMirrorConfig(BOOL fgMirrorEn, BOOL fgFlipEn)
{
#ifdef CC_FLIP_MIRROR_FROM_EEPROM
    DTVCFG_T rDtvCfg;
    
    if(EEPDTV_GetCfg(&rDtvCfg))
    {
        Printf("Read Flip-Mirror Config Fail\n");
        return SV_FAIL;
    }

    if (fgMirrorEn)
    {
        rDtvCfg.u1Flags2 |= (DTVCFG_FLAG2_MIRROR_ON);
    }
    else
    {
        rDtvCfg.u1Flags2 &= ~(DTVCFG_FLAG2_MIRROR_ON);
    }
    if(EEPDTV_SetCfg(&rDtvCfg))
    {
        Printf("Set Mirror Config Fail\n");
        return SV_FAIL;
    }

    if (fgFlipEn)
    {
        rDtvCfg.u1Flags2 |= (DTVCFG_FLAG2_FLIP_ON);
    }
    else
    {
        rDtvCfg.u1Flags2 &= ~(DTVCFG_FLAG2_FLIP_ON);
    }
    if(EEPDTV_SetCfg(&rDtvCfg))
    {
        Printf("Set Flip Config Fail\n");
        return SV_FAIL;
    }
    Printf("fgMirrorEn: %d\tfgFlipEn: %d\trDtvCfg.u1Flags2: %d\n", fgMirrorEn, fgFlipEn, rDtvCfg.u1Flags2);
    return SV_SUCCESS;
    
#else
    Printf("CC_FLIP_MIRROR_SUPPORT is not defined!\n");
    return SV_FAIL;
#endif
}

#ifdef CC_FLIP_MIRROR_SUPPORT
#include "eeprom_if.h"
UINT8 u1GetFlipMirrorConfig(void)
{
    static UINT8 u1Init = 0 ;
    static UINT8 u1FlipMirrorEnable= 0 ;
#ifdef CC_FLIP_MIRROR_FROM_EEPROM
	DTVCFG_T rDtvCfg;
#endif

	if(u1Init == 0 )
	{
#ifdef CC_FLIP_MIRROR_FROM_EEPROM
		if(EEPDTV_GetCfg(&rDtvCfg))  
		{
			Printf(" Get DTV config fail\n");
		}
		else
		{
			if(rDtvCfg.u1Flags2 & DTVCFG_FLAG2_FLIP_ON)	
			{
				u1FlipMirrorEnable |= SYS_FLIP_CONFIG_ON;
			}
			if(rDtvCfg.u1Flags2 & DTVCFG_FLAG2_MIRROR_ON)
			{
				u1FlipMirrorEnable |= SYS_MIRROR_CONFIG_ON;
			}
		}
#else
		#ifdef CC_FLIP_ENABLE
		u1FlipMirrorEnable |= SYS_FLIP_CONFIG_ON ;
		#endif
		#ifdef CC_MIRROR_ENABLE
		u1FlipMirrorEnable |= SYS_MIRROR_CONFIG_ON ;
		#endif
#endif
		u1Init = 1 ;
	}      

#ifdef CC_ENABLE_MTK_MODEL_INDEX
	if(u1UseSuperFlipMirror)
	{
	    LOG(9, "======u1SuperFlipMirrorCfg %d\n\n", u1SuperFlipMirrorCfg);
	    return u1SuperFlipMirrorCfg;
	}
	else
#endif
	{
    	LOG(9, "======u1FlipMirrorEnable %d\n\n", u1FlipMirrorEnable);
	    return u1FlipMirrorEnable;
	}
}
#endif
#endif

#ifdef CC_FLIP_MIRROR_SUPPORT
UINT32 u4GetFlipMirrorModule(UINT32 u4VdpId)
{
#ifndef CC_MTK_LOADER
	UINT32 u4Module =FLIP_NONE;
	if(u1GetFlipMirrorConfig())
	{
		u4Module= u4QueryFlipModule(u4VdpId);
	}
	
       LOG(9, "======u4GetFlipMirrorModule(%d) %d\n\n", u4VdpId, u4Module);

	return u4Module;
#else
	return 0;
#endif
}
#endif  //CC_FLIP_MIRROR_SUPPORT

#ifndef CC_MTK_LOADER
#ifdef SUPPORT_DDDS_STEP_TRACKING
void vDrvDDDSStepTrackingOnOff(UINT16 u2OnOff)
{
    if (_u2DDDSStepTrackingEn == u2OnOff)
    {
        return;
    }
    LOG(3, "DDDSStepTrackingOnOff: %d\n", u2OnOff);
    _u2DDDSStepTrackingEn = u2OnOff;

    // update error limit
    if (u2OnOff)
    {
        _u4DefaultErrorLimit = IO32ReadFldAlign(DDDS_03,DDDS_03_DDDS_ERR_LIM);
        vDrvSetErrorLimit(_u4DDDSTrackingErrorLimit);
    }
    else
    {
        vDrvSetErrorLimit(_u4DefaultErrorLimit);
    }
    
}

void vDrvUpdateDisplayCenterCW(UINT32 u4CW)
{

    if (_u4CenterCW == u4CW)
    {
        return;
    }    

    // update center frequence
    _u4CenterCW = u4CW;
    _u4CurrentStepCW = u4CW;

    // calculate CW upper/lower bound
    _u4StepInCWUpperBound = _u4CenterCW + ((_u4DefaultErrorLimit - _u4DDDSTrackingErrorLimit) * 32768);    
    _u4StepInCWLowerBound = _u4CenterCW - ((_u4DefaultErrorLimit - _u4DDDSTrackingErrorLimit) * 32768);   

    // if target freq delta is over  _u4CenterFreqStep, FW will update display freq.  
//    _u4StepInCW = _u4CenterFreq * (_u2DDDSTrackingSpeed - _u4CenterFreqStepInTolerance)/1000000;   
}

static void vDrvDDDSTrackingNormProc(void)
{
    if (_u2DDDSStepTrackingEn)
    {
        UINT32 u4CW;
        UINT32 u4CWUpperBound, u4CWLowerBound;
        
        // only step tracking with DDDS close loop
        if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN) == 0)
        {
            return;
        }
        if ((_u4CenterCW == 0) || (_u4CurrentStepCW == 0))
        {
            LOG(1, "Invalid DDDS center CW\n");
            return;
        }

        u4CW = IO32ReadFldAlign(STA_DDDS_00, STA_DDDS_00_DDDS_FREQ_CW);
        u4CWUpperBound = _u4CurrentStepCW + (_u4DDDSTrackingErrorLimit * 32768) - _u4StepInCWTolerance;
        u4CWLowerBound = _u4CurrentStepCW - (_u4DDDSTrackingErrorLimit * 32768) + _u4StepInCWTolerance;
        if (u4CWUpperBound <= u4CWLowerBound)
        {
            LOG(2, "Invalid DDDS Step Tracking Bound (%d, %d)\n", u4CWLowerBound, u4CWUpperBound);
            return;
        }
        
        if ((u4CW > u4CWUpperBound) || (u4CW < u4CWLowerBound))
        {
            UINT32 u4NewCW;

            if (u4CW > u4CWUpperBound)
            {
                u4NewCW = _u4CurrentStepCW + (_u4DDDSTrackingErrorLimit * 32768); 
            }
            else //             (u4CW < u4CWLowerBound))
            {
                u4NewCW = _u4CurrentStepCW - (_u4DDDSTrackingErrorLimit * 32768); 
            }

            if (u4NewCW > _u4StepInCWUpperBound)
            {               
                u4NewCW = _u4StepInCWUpperBound;               
            }
            else if (u4NewCW < _u4StepInCWLowerBound)
            {
                u4NewCW = _u4StepInCWLowerBound;               
            }

            if (u4NewCW != _u4CurrentStepCW)
            {
                LOG(3, "DDDS Step Tracking: CW (0x%X) --> (0x%X)\n", _u4CurrentStepCW, u4NewCW);
                _u4CurrentStepCW = u4NewCW;
                vIO32WriteFldAlign(DDDS_00,u4NewCW,DDDS_00_DDDS_FREQ_CW);                
            }
            
            
        }

    }
}
#endif

void vDrvSetSyncLockMode(enum eSyncLockMode eMode)
{
    _eSyncLockMode = eMode;

    if(_eSyncLockMode == eSYNCLOCK_OFF)
    {
        vDrvSwitchMTKGoodDclk(SV_OFF);
    }
    else
    if(_eSyncLockMode == eSYNCLOCK_ON)
    {
        vDrvSetMTKGoodDclk(wDrvVideoGetVTotal(SV_VP_MAIN),bDrvVideoGetRefreshRate(SV_VP_MAIN));
        vDrvSwitchMTKGoodDclk(SV_ON);
    }
    else
    {
        /* do nothing. */
    }
    return;
}

/**
 * @brief vDrvSetDDDSFrameTrackTarget

 * Set DDDS Frame track Parameters

 * @param  u1SlowFast: slow/fast parameter
 * @param  u4TargetVTotal: target output Vtotal
 * @retval void
 */ 
void vDrvSetDDDSFrameTrackTarget(UINT8 u1SlowFast, UINT32 u4TargetVTotal) 
{
	UINT32 DISPLAY_SIZE, HLEN_INT, HLEN_NUM, InputVTotal;
	UINT32 u4HTotal, u4VTotal;
	UINT32 u4HalfSetp;
//	Printf("slowfast(%d), vtotal(%d)\n",u1SlowFast,u4TargetVTotal);

	InputVTotal = IO32ReadFldAlign(DDDS_02, DDDS_02_HLEN_DEN);
	HLEN_NUM = IO32ReadFldAlign(DDDS_01, DDDS_01_HLEN_NUM);
	HLEN_INT = IO32ReadFldAlign(DDDS_01, DDDS_01_HLEN_INT);
	DISPLAY_SIZE = (HLEN_INT * InputVTotal) + HLEN_NUM;

    u4HTotal = (UINT32)wDrvGetOutputHTotal();
    u4VTotal = (UINT32)wDrvGetOutputVTotal();
    
    if(_u4FgDDDSTrackHalfStep)
    {
        u4HalfSetp = 1;
    }
    else
    {
        u4HalfSetp = 0;
    }

	if(u4TargetVTotal > u4VTotal)
		DISPLAY_SIZE = DISPLAY_SIZE +(((u4TargetVTotal - u4VTotal)*u4HTotal)>>u4HalfSetp);
	else
		DISPLAY_SIZE = DISPLAY_SIZE -(((u4VTotal - u4TargetVTotal)*u4HTotal)>>u4HalfSetp);

	if (InputVTotal ==0) InputVTotal = 1;
	
    HLEN_INT = (DISPLAY_SIZE / InputVTotal);
    HLEN_NUM = DISPLAY_SIZE - (HLEN_INT * InputVTotal);
	
    switch(u1SlowFast)
    {
    case SV_DDDS_FRAMETRACK_FAST1:
		vIO32WriteFldAlign(DDDS_0C,(UINT16)InputVTotal,DDDS_0C_HLEN_DEN_E1);
		vIO32WriteFldAlign(DDDS_0D,(UINT16)HLEN_NUM,DDDS_0D_HLEN_NUM_E1);	  
		vIO32WriteFldAlign(DDDS_0C,HLEN_INT,DDDS_0C_HLEN_INT_E1);
        break;

    case SV_DDDS_FRAMETRACK_FAST2:
		vIO32WriteFldAlign(DDDS_0E,(UINT16)InputVTotal,DDDS_0E_HLEN_DEN_E2);
		vIO32WriteFldAlign(DDDS_0E,(UINT16)HLEN_NUM,DDDS_0E_HLEN_NUM_E2);	  
		vIO32WriteFldAlign(DDDS_0D,HLEN_INT,DDDS_0D_HLEN_INT_E2);
        break;

    case SV_DDDS_FRAMETRACK_SLOW1:
		vIO32WriteFldAlign(DDDS_0F,(UINT16)InputVTotal,DDDS_0F_HLEN_DEN_L1);
		vIO32WriteFldAlign(DDDS_10,(UINT16)HLEN_NUM,DDDS_10_HLEN_NUM_L1);	  
		vIO32WriteFldAlign(DDDS_0F,HLEN_INT,DDDS_0F_HLEN_INT_L1);
        break;

	case SV_DDDS_FRAMETRACK_SLOW2:
		vIO32WriteFldAlign(DDDS_11,(UINT16)InputVTotal,DDDS_11_HLEN_DEN_L2);
		vIO32WriteFldAlign(DDDS_11,(UINT16)HLEN_NUM,DDDS_11_HLEN_NUM_L2);	  
		vIO32WriteFldAlign(DDDS_10,HLEN_INT,DDDS_10_HLEN_INT_L2);
		break;

    default:
        break;
    }
	
}

/**
 * @brief vDrvSetDDDSFrameTrackMode

 * Set DDDS Frame track mode

 * @param  u1Mode: 0 => off, 1 => form scaler, 2 => form MJC
 * @retval void
 */ 
void vDrvSetDDDSFrameTrackMode(UINT8 u1Mode) 
{
    switch(u1Mode)
    {
	case 0: //off
		vIO32WriteFldAlign(DDDS_0C,SV_OFF,DDDS_0C_VSYNC_TRACK_EN);
		break;

	case 1: //from scaler
		vIO32WriteFldAlign(DDDS_0C,SV_ON,DDDS_0C_VSYNC_TRACK_EN);
		vIO32WriteFldAlign(DDDS_03,1,DDDS_03_SC_MJC_TRACK_SEL);
		break;

	case 2: //from MJC
		vIO32WriteFldAlign(DDDS_0C,SV_ON,DDDS_0C_VSYNC_TRACK_EN);
		vIO32WriteFldAlign(DDDS_03,0,DDDS_03_SC_MJC_TRACK_SEL);
		break;

	default:
		break;
    }
}

#ifdef FORCE_3D_60HZ_OUTPUT_ENABLE
void vForce60HZOutputDetect(void)
{
	static UINT8 u1TTDStaPre=0; 
	static UINT8 u1TTDStaCur=0;
	UINT8 u1InFR;

	if ((bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE))
	{
		u1TTDStaCur = u4DrvTDTV3DModeQuery();
		u1InFR = bDrvVideoGetRefreshRate(SV_VP_MAIN);
		if ((u1TTDStaCur>=E_TDTV_UI_3D_MODE_TTD)&&(u1TTDStaCur<E_TDTV_UI_3D_MODE_END))  // 2D force 3D
		{
			if (u1TTDStaPre!=u1TTDStaCur)
			{
				LOG(3, "====#1 TDTV 3DMode: %d-->%d\n", u1TTDStaPre, u1TTDStaCur);
				if (u1TTDStaPre==E_TDTV_UI_3D_MODE_OFF)
				{
					if ((u1InFR==25)||(u1InFR==50)||(u1InFR==100)||((u1InFR>=SUPPORT_IN_LOW_FRAME_RATE)&&(u1InFR<55)))
					{
						_bForceFrameRate=60;
						//vSetMainFlg(MAIN_FLG_MODE_CHG);
						//vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
						if(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
            {
            	vDDDSLCDSetting(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
            }
						Printf("====Force 60Hz\n");
					}
				}
				u1TTDStaPre = u1TTDStaCur;
			}
		}
		else if (u1TTDStaCur==E_TDTV_UI_3D_MODE_OFF)  // cancel force-3D
		{
			if (u1TTDStaPre!=u1TTDStaCur)
			{
				LOG(3, "====#2 TDTV 3DMode: %d-->%d\n", u1TTDStaPre, u1TTDStaCur);
				if ((u1TTDStaPre>=E_TDTV_UI_3D_MODE_TTD)&&(u1TTDStaPre<E_TDTV_UI_3D_MODE_END))
				{
					if ((u1InFR==25)||(u1InFR==50)||(u1InFR==100)||((u1InFR>=SUPPORT_IN_LOW_FRAME_RATE)&&(u1InFR<55)))
					{
						_bForceFrameRate=0;
						//vSetMainFlg(MAIN_FLG_MODE_CHG);
						//vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
						if(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
            {
            	vDDDSLCDSetting(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
            }
						LOG(3, "====Cancel force-60Hz\n");
					}
				}
				u1TTDStaPre = u1TTDStaCur;
			}
		}
	}
}
#endif

void u1DrvSetForceOutputFrameRateByUser(UINT8 u1ForFraRateCtl)
{
    #ifdef SUPPORT_FORCE_OUTPUT_FRAMERATE_BY_USER
    LOG(2, "u1DrvSetForceOutputFrameRateByUser,u1ForFraRateCtl : %d \n", u1ForFraRateCtl);
    if ((u1ForFraRateCtl & FORCE_DCLK_FORCE_48HZ) != (bFORCE_FRAMERATE_TYPE & FORCE_DCLK_FORCE_48HZ))
    {
        bFORCE_FRAMERATE_TYPE = u1ForFraRateCtl;
        vSetMainFlg(MAIN_FLG_MODE_CHG);
        vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
    }
    else
    {
        bFORCE_FRAMERATE_TYPE = u1ForFraRateCtl;
        #ifdef CC_MT5398
        vDrvMJCSetModeChangeTrig();
        #endif
    }
    #else
    bFORCE_FRAMERATE_TYPE = ~0;
    #endif
}

UINT8 u1DrvGetForceOutputFrameRate(void)
{
    UINT8 u1ForFraRateCtl = 0;
    
    #ifdef SUPPORT_PANEL_48HZ
    u1ForFraRateCtl |= FORCE_DCLK_FORCE_48HZ;
    #endif
    #ifdef SUPPORT_DISP_50TO120
    u1ForFraRateCtl |= IS_SUPPORT_DISP_50TO120_2D ? FORCE_DCLK_FORCE_2D_60HZ : FORCE_DCLK_NORMAL;
    u1ForFraRateCtl |= IS_SUPPORT_DISP_50TO120_3D ? FORCE_DCLK_FORCE_3D_60HZ : FORCE_DCLK_NORMAL;
    #endif
    #ifdef SUPPORT_DISP_60TO100
    u1ForFraRateCtl |= IS_SUPPORT_DISP_60TO100_2D ? FORCE_DCLK_FORCE_2D_50HZ : FORCE_DCLK_NORMAL;
    u1ForFraRateCtl |= IS_SUPPORT_DISP_60TO100_3D ? FORCE_DCLK_FORCE_3D_50HZ : FORCE_DCLK_NORMAL;
    #endif
    u1ForFraRateCtl &= bFORCE_FRAMERATE_TYPE;

    LOG(2 , "u1DrvGetForceOutputFrameRateu1,ForFraRateCtl: %d  bFORCE_FRAMERATE_TYPE:%d \n" , u1ForFraRateCtl , bFORCE_FRAMERATE_TYPE);
    return u1ForFraRateCtl;
}

void vDDDSLCDSetting(UINT16 u2InVTotal, UINT8 bRefreshRate)
{
//    UINT8 bRefreshRate;
    UINT8 bLCDFreqType, bDDDSOn;
    UINT8 bOutFrameRate, bInFrameRateTemp;
    bInFrameRateTemp=0;

    // --------LCD Freq Settings-----------
    //bRefreshRate = bDrvVideoGetRefreshRate(SV_VP_MAIN);

    LOG(3, "vDDDSLCDSetting(u2InVTotal=%d,bRefreshRate=%d)\n",u2InVTotal,bRefreshRate);

    if ( _bForceFrameRate != 0 )
    {
	    bInFrameRateTemp = bRefreshRate;
        bRefreshRate = _bForceFrameRate;
	    bDDDSOn = (IS_PANEL_L12R12)? SV_FALSE : SV_TRUE;
    }
    else
	{
		bDDDSOn = SV_TRUE;
	}

	if (IS_PANEL_L12R12)
	bRefreshRate =(bRefreshRate ==0)? bDrvVideoGetRefreshRate(SV_VP_MAIN):bRefreshRate;
    bLCDFreqType = SV_DCLK_60HZ;
    bOutFrameRate = bRefreshRate;

    if (((bRefreshRate == 25) && (DRVCUST_PanelGet(ePanel242530pOutput) == 0)) || (bRefreshRate == 50) || (bRefreshRate == 100))
    {
        #ifdef SUPPORT_DISP_50TO120
        if (((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_2D_60HZ)) ||
            ((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_3D_60HZ)))
        {
            bLCDFreqType = SV_DCLK_60HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
            LOG(2, "vDDDSLCDSetting: enable 25/50/100-to-120 display.");
        }
        else
        #endif
        {
            bLCDFreqType = SV_DCLK_50HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 100: 50;
        }
    }
    else if (((bRefreshRate == 30) && (DRVCUST_PanelGet(ePanel242530pOutput) == 0)) || (bRefreshRate == 60)|| (bRefreshRate == 120))
    {
        #ifdef SUPPORT_DISP_60TO100
        if (((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_2D_50HZ)) ||
            ((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_3D_50HZ)))
        {
            bLCDFreqType = SV_DCLK_50HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 100 : 50;
            LOG(2, "vDDDSLCDSetting: enable 30/60/120-to-100 display.");
        }
        else
        #endif
        {
            bLCDFreqType = SV_DCLK_60HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
        }
    }
    else if ((bRefreshRate == 24) && (DRVCUST_PanelGet(ePanel242530pOutput) == 0))
    {
        bLCDFreqType  = SV_DCLK_60HZ;
        bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
        #ifdef SUPPORT_PANEL_48HZ
        if (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
        {
            if (IS_PANEL_L12R12)
            {
                if (fgDrvSupport24Hz())
                {
                 	bLCDFreqType  = SV_DCLK_48HZ;
                 	bOutFrameRate = PANEL_IsSupport120Hz()? 96 : 48;
                }
    			else
    			{
    				 bLCDFreqType  = SV_DCLK_60HZ;
    				 bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
    			}
            }
            else
            {
                if (PANEL_IsSupport120Hz())
                {
                    if ((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_SBS) && (u1DrvVideoGetIs44PullDownInfo() == SV_TRUE))
                    {
                        bLCDFreqType  = SV_DCLK_48HZ;
                        bOutFrameRate = 96;
                    }
                    else
                    {
    	                if (u1DrvVideoGetIs44PullDownInfo() == SV_TRUE)
    	                {
                            bLCDFreqType  = SV_DCLK_48HZ;
                            bOutFrameRate = 48;
            	        }
    	                else
        	            {                
                            bLCDFreqType  = SV_DCLK_60HZ;
                            bOutFrameRate = 120;
    	                }
                    }	
                }
                else
                {
                    if (u1DrvVideoGetIs44PullDownInfo() == SV_TRUE)
                    {
                        bLCDFreqType  = SV_DCLK_48HZ;
                        bOutFrameRate = 48;
                    }
                    else
                    {
                        bLCDFreqType  = SV_DCLK_60HZ;
                        bOutFrameRate = 60;
                    }
                }
            }
        }
        #endif
        
        #ifdef  SUPPORT_DISP_60TO100
        else if (((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_2D_50HZ)) ||
                ((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_3D_50HZ)))
        {
            bLCDFreqType = SV_DCLK_50HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 100 : 50;
            LOG(2, "vDDDSLCDSetting: enable 24-to-100 display.");
        }	
        #endif
    }
    else if (bRefreshRate == 48)
    {
        bLCDFreqType  = SV_DCLK_60HZ;
        bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
        #ifdef SUPPORT_PANEL_48HZ
        if (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ)
        {
    		if(IS_PANEL_L12R12)
    		{
    			bLCDFreqType  = SV_DCLK_48HZ;
    			bOutFrameRate = PANEL_IsSupport120Hz()? 96 : 48;
    		}
    		else
    		{
    
    			if ((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_FS) && (u1DrvVideoGetIs44PullDownInfo() == SV_FALSE))
    			{
    				bLCDFreqType  = SV_DCLK_60HZ;
    				bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
    			}
    			else
    			{
    				bLCDFreqType  = SV_DCLK_48HZ;
    				bOutFrameRate = PANEL_IsSupport120Hz()? 96: 48;
    			}
    		}
        }
        #endif
        
        #ifdef  SUPPORT_DISP_60TO100
        else if (((u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_2D_50HZ)) ||
            ((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_3D_50HZ)))
        {
            bLCDFreqType = SV_DCLK_50HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 100 : 50;
            LOG(2, "vDDDSLCDSetting: enable 48-to-100 display.");
        }
        #endif
    }
	#ifdef SUPPORT_DISP_WEB_VIDEO_SMOOTHER
	else if ((bRefreshRate == 12) || (bRefreshRate == 15) || (bRefreshRate == 20))
	{
        bLCDFreqType = SV_DCLK_60HZ;
        bOutFrameRate = PANEL_IsSupport120Hz()? 120: 60;
        LOG(2, "vDDDSLCDSetting: enable 12/15/20  to 120 display.");
		if ((bRefreshRate == 20) && (!IS_SUPPORT_DISP_6X3X_WEB))
		{
            bLCDFreqType = SV_DCLK_50HZ;
            bOutFrameRate = PANEL_IsSupport120Hz()? 100: 50;
            LOG(2, "vDDDSLCDSetting: enable 20  to 100 display.");
		}
	}
	#endif
    else
    {
        UINT32 u4LockOthers;
        UINT32 u4Clk;
        if (DRVCUST_PanelQuery(eDDDSLockOnOthers, &u4LockOthers) != 0)
        {
            u4LockOthers = 1;
        }
        if (PANEL_IsSupport120Hz())
        {
            if (bRefreshRate >= 110)     // > 110; try 1:1 output
            {
                u4Clk = PANEL_GetPixelClk60Hz()/120 * bRefreshRate;
                bLCDFreqType = SV_DCLK_60HZ;
                bOutFrameRate = 120;
            }
            else if((bRefreshRate >= 77)&&(!IS_PANEL_L12R12))  // 77 ~ 110; try 1:1 output             
			{
				u4Clk = PANEL_GetPixelClk50Hz()/100 * bRefreshRate;
				if ((u4Clk < DCLK_MAX) && (u4Clk > DCLK_MIN) && (u2InVTotal > 0) && (u4LockOthers))   // 1:1 output
				{
					bLCDFreqType = SV_DCLK_50HZ;
					bOutFrameRate = 100;
				}
				else    // 1:2 output
				{
					bLCDFreqType = SV_DCLK_60HZ;
					bOutFrameRate = 120;
				}
			}
			else if ((bRefreshRate >= 90)&&(IS_PANEL_L12R12))  // 90 ~ 110; try 1:1 output 
			{
                u4Clk = PANEL_GetPixelClk50Hz()/100 * bRefreshRate;
                if ((u4Clk < DCLK_MAX) && (u4Clk > DCLK_MIN) && (u2InVTotal > 0) && (u4LockOthers))   // 1:1 output
                {
                    bLCDFreqType = SV_DCLK_50HZ;
                    bOutFrameRate = 100;
                }
                else    // 1:2 output
                {
                    bLCDFreqType = SV_DCLK_60HZ;
                    bOutFrameRate = 120;
                }
            }
            else if (bRefreshRate < 55)   // < 55; try 1:2 output
            {
                u4Clk = PANEL_GetPixelClk50Hz()/100 * bRefreshRate * 2;
                bLCDFreqType = SV_DCLK_50HZ;
                bOutFrameRate = 100;
            }
            else        // 55 ~ 77; try 1:2 output
            {
                u4Clk = PANEL_GetPixelClk60Hz()/120 * bRefreshRate * 2;
                bLCDFreqType = SV_DCLK_60HZ;
                bOutFrameRate = 120;
            }
        }
        else
        {
            #ifdef SUPPORT_PANEL_48HZ
            if((bRefreshRate < 24)&&(fgDrvSupport24Hz()) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
            { 
                if(DRVCUST_PanelGet(ePanel242530pOutput) == 0)
				bRefreshRate = bRefreshRate *2;	               
				
                u4Clk = PANEL_GetPixelClk48Hz()/48 * (bRefreshRate*2);
                bLCDFreqType = SV_DCLK_48HZ;
                bOutFrameRate = 48;				
            }
			else 
            #endif
			if(bRefreshRate < SUPPORT_IN_LOW_FRAME_RATE)
            { 
                if(DRVCUST_PanelGet(ePanel242530pOutput) == 0)
				bRefreshRate = bRefreshRate *2;
			   
                u4Clk = PANEL_GetPixelClk60Hz()/60 * (bRefreshRate*2);
                bLCDFreqType = SV_DCLK_60HZ;
                bOutFrameRate = 60;				
            }
			else        
            if (bRefreshRate < 55) 
            {
                u4Clk = PANEL_GetPixelClk50Hz()/50 * bRefreshRate;
                bLCDFreqType = SV_DCLK_50HZ;
                bOutFrameRate = 50;
            }
            else
            {
                u4Clk = PANEL_GetPixelClk60Hz()/60 * bRefreshRate;
                bLCDFreqType = SV_DCLK_60HZ;
                bOutFrameRate = 60;
            }
        }        

			LOG(3, "u4Clk=%d,bLCDFreqType=%d,bOutFrameRate=%d)\n",u4Clk,bLCDFreqType,bOutFrameRate);


        #ifdef DRV_SUPPORT_EXTMJC
        if ((IS_COMPANION_CHIP_ON()) && (u4Clk < DCLK_MAX) && (u2InVTotal > 0) && (u4LockOthers))
        {
            // ignore DLCK_MIN checking when connecting with companion chip
            LOG(3, "DDDSLCDSetting: ignore DLCK_MIN/DCLK_MAX checking when connecting with companion chip\n");
            bLCDFreqType = SV_DCLK_OTHERS;
            bOutFrameRate = bRefreshRate;
        }
        else
        #endif        
        if ((u4Clk < DCLK_MAX) && (u4Clk > DCLK_MIN) && (u2InVTotal > 0) && (u4LockOthers))
        {  
            bLCDFreqType = SV_DCLK_OTHERS;
            if ((bRefreshRate < 77) && PANEL_IsSupport120Hz())
            {
                bOutFrameRate = bRefreshRate * 2;
            }
            else
            {
                bOutFrameRate = bRefreshRate;
            }
        }
        else
        {
            LOG(3, "vDDDSLCDSetting - clock protect u4Clk=%d DCLK_MAX = %d, DCLK_MIN =%d \n",u4Clk,DCLK_MAX,DCLK_MIN);
            // Cannot turn on DDDS
            bDDDSOn = SV_FALSE;
        }

        if(DRVCUST_PanelGet(ePanel242530pOutput) != 0)
        {
            if(bRefreshRate == 24 || bRefreshRate == 25 || bRefreshRate == 30)
            {
                bOutFrameRate = bRefreshRate;
				LOG(3, "support ePanel_242530pOutput mode\n");
            }
        }		
    }

    if ((bRefreshRate == 24) || (bRefreshRate == 25) || (bRefreshRate == 30))
    {
        #if SUPPORT_DDDS_ON_24P_25P_30P
        if (bDrvVideoIsSrcInterlace(SV_VP_MAIN))
        #endif
        {
            bDDDSOn = SV_FALSE;
        }
    }

	  if ( _bForceFrameRate != 0 )
	  {
		    bRefreshRate = bInFrameRateTemp;
	  }

    vDrvLCDFreqSet(bLCDFreqType, bOutFrameRate);
    // ---------DDDS Settings------------
    if (bDDDSOn ==SV_TRUE && _eSyncLockMode != eSYNCLOCK_OFF)
    {
        vDrvSetMTKGoodDclk(u2InVTotal, bRefreshRate);
        if (fgIsMainTvd3d())
        {
            _fgFRS_ENA = SV_ON;    //Delay for VCR mode: Only for TVD
        }
        else
        {
            vDrvSwitchMTKGoodDclk(SV_ON);
        }
    }
    else
    {
        _fgFRS_ENA = SV_OFF;
        vDrvSwitchMTKGoodDclk(SV_OFF);
    }

    LOG(3, "DDDSLCDSetting: bRefreshRate(%d), bLCDFreqType(%d), bDDDSOn(%d)\n", bRefreshRate, bLCDFreqType, bDDDSOn);
}

void vDddsSettingCheck(void)
{

#if CC_OSD_MUTE_WA
    //channel change will reset out region
    if (_bDddsDelayFlag && (!(_fgAutoSearch || SRM_IsEpgMode())) && SRM_IsMainVideoStrictStable())
    {
        vDDDSLCDSetting(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
        // reset non-standard signal status
        _u4NSDHTotal=0xff;
        _u4NSDVTotal=0xff;
        _u4NSDFrameRate=0xff;
        _bDddsDelayFlag = 0 ;
        #ifdef DRV_SUPPORT_EXTMJC
        vDrvExtmjcRedoTwoChipConnection( SV_VP_MAIN, SV_TRUE);
        #endif
		#if defined(CC_MT5398) 
		vDrvMJCSetModeChangeTrig();
		#endif
    }
#endif

    if ((_fgFRS_ENA == SV_ON) && fgIsMainTvd3d() 
        && (_eSyncLockMode == eSYNCLOCK_IGNORED))  	// Switch MTK Good Clock
    {
        if (bhwVCRSetting||(_na_state == 0x03)) //||!_fgHLOCK) //When VCR mode or Signal unsatble will OFF
        {
            vDrvSwitchMTKGoodDclk(SV_OFF);
        }
        else
        {
            vDrvSwitchMTKGoodDclk(SV_ON);
        }
    }
}

void vDrvDDDSLockStatus(UINT32 u4Count)
{
 UINT32 u4LoopCnt=u4Count;
 UINT32 u4UnlockCnt=0;
 static UINT8 u1CheckGoing=0;

 if (u1CheckGoing)
 {
	return;
 }
 do
 {
  u1CheckGoing = 1;
  if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 1) 
  {
  	if (!bDrvCheckMTKGoodStatus())
  	{
   	u4UnlockCnt++;
  	}
  	u4LoopCnt--;
  }	
  else
  {
	Printf("Open loop\n");
	u4LoopCnt = 0;
  }
 }while(u4LoopCnt);
 if(u4UnlockCnt)
 {
 	Printf("[UNLOCK] vDrvDDDSLockStatus check Finished unlock/check = %d / %d\n",u4UnlockCnt,u4Count);
 }	
 else
 {
 	if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 1)
 	{
 		Printf("[LOCK] vDrvDDDSLockStatus check Finished OK\n");
 	}	
 }
 u1CheckGoing = 0;
}

void vDrvSetAllMute(UINT8 bOnOff)
{
    vRegWriteFldAlign(MUTE_00, bOnOff,  R_MUTE_POST_EN);
}

#ifdef CC_SCPOS_3DTV_SUPPORT 
 
static void updatePanelAttribute(UINT16 u2HTotal,
                                 UINT16 u2VTotal,
                                 UINT16 u2Width,
                                 UINT16 u2Height,
                                 UINT16 u2FrameRate)
{
    PANEL_SetPanelWidth(u2Width);
    PANEL_SetPanelHeight(u2Height);

    if(u2FrameRate >= 96)
    {
        PANEL_SetVClkMax(122);
    }
    else
    {
        PANEL_SetVClkMax(61);
    }

    if(u2FrameRate == 25 || u2FrameRate == 50 || u2FrameRate == 100)
    {
        PANEL_SetHTotal50Hz(u2HTotal);
        PANEL_SetVTotal50Hz(u2VTotal);
    }
    #ifdef SUPPORT_PANEL_48HZ
    else if((u2FrameRate == 48 || u2FrameRate == 96) && (u1DrvGetForceOutputFrameRate() & FORCE_DCLK_FORCE_48HZ))
    {
        PANEL_SetHTotal48Hz(u2HTotal);
        PANEL_SetVTotal48Hz(u2VTotal);
    }
    #endif
    else 
    {
        PANEL_SetHTotal60Hz(u2HTotal);
        PANEL_SetVTotal60Hz(u2VTotal);
    }
     // update OSD scaler parameters
    {
    UINT32 srcW=0,srcH=0,dstW=0,dstH=0,Enable=0,u4bpp=0;
    OSD_SC_GetScalerInfo(OSD_SCALER_3,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
    OSD_SC_Scale(OSD_SCALER_3,Enable,srcW,srcH,u2Width,u2Height);
    OSD_SC_GetScalerInfo(OSD_SCALER_2,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
    OSD_SC_Scale(OSD_SCALER_2,Enable,srcW,srcH,u2Width,u2Height);
    }
    return;
}

static UINT16 au2OrigHTotal[3];
static UINT16 au2OrigVTotal[3];
static UINT16 u2OrigWidth, u2OrigHeight;
static UINT32 au4OrigPixelMinMax[2];
static UINT8 u1OrigVClkMax; 

static UINT32 u43DModeDclk = 0;
static UINT8 u13DModeRefreshRate = 0;
static void vSavePanelAttribute(void)
{
    u2OrigWidth = PANEL_GetPanelWidth();
    u2OrigHeight = PANEL_GetPanelHeight();

    au2OrigHTotal[0] = PANEL_GetHTotal60Hz();
    au2OrigVTotal[0] = PANEL_GetVTotal60Hz();

    au2OrigHTotal[1] = PANEL_GetHTotal50Hz();
    au2OrigVTotal[1] = PANEL_GetVTotal50Hz();

#ifdef SUPPORT_PANEL_48HZ
    au2OrigHTotal[2] = PANEL_GetHTotal48Hz();
    au2OrigVTotal[2] = PANEL_GetVTotal48Hz();
#endif

    au4OrigPixelMinMax[0] = PANEL_GetPixelClkMin();
    au4OrigPixelMinMax[1] = PANEL_GetPixelClkMax();
    u1OrigVClkMax = PANEL_GetVClkMax();
    
}

static void vDrvDisp3DModeCB(void)
{
    S_TDTV_OSTG_CONFIG rOstgCnf;
    //UINT16 u2FrameRate; 

    rOstgCnf = eDrvTDTVOSTGModeQuery();
    if(rOstgCnf.eOSTGCfg == E_TDTV_OSTG_CFG_BY_PANEL_TBL)
    {
        //resotred the panel attributes
#ifdef PANEL_SUPPORT_48HZ
        updatePanelAttribute(au2OrigHTotal[2],
                             au2OrigVTotal[2],
                             u2OrigWidth,
                             u2OrigHeight,
                             48);
#endif
        updatePanelAttribute(au2OrigHTotal[1],
                             au2OrigVTotal[1],
                             u2OrigWidth,
                             u2OrigHeight,
                             50);
        updatePanelAttribute(au2OrigHTotal[0],
                             au2OrigVTotal[0],
                             u2OrigWidth,
                             u2OrigHeight,
                             60);

        u43DModeDclk = 0;
        PANEL_SetPixelClkMin(au4OrigPixelMinMax[0]);
        PANEL_SetPixelClkMax(au4OrigPixelMinMax[1]);
        PANEL_SetVClkMax(u1OrigVClkMax);
    }
    else
    {
        updatePanelAttribute(rOstgCnf.u4HTotal,
                             rOstgCnf.u4VTotal,
                             rOstgCnf.u4Width,
                             rOstgCnf.u4Height,
                             rOstgCnf.u4FrameRate);

        u43DModeDclk = rOstgCnf.u4Clk;
        u13DModeRefreshRate = rOstgCnf.u4FrameRate;

        // clock range to cover 24/25/30Hz output clock
        PANEL_SetPixelClkMin(50000000);
    }
    vDrvOutputStageInit();
    vDrvLVDSConfig();	
    vErrorHandleSetByPanel();
    vDrvSetLCDTiming();
    #ifdef DRV_SUPPORT_EXTMJC
    vDrvExtmjcRedoTwoChipConnection( SV_VP_MAIN, SV_TRUE);
    #endif
#ifdef DRV_SUPPORT_MT8283
    fgForceChangeRegion = TRUE;
#endif
    return;
}

UINT32 u4DrvGetDisp3DModeDclk(void)
{
    return u43DModeDclk;
}

UINT8 u1DrvGetDisp3DModeRefreshRate(void)
{
    return u13DModeRefreshRate;
}

#endif

#endif

#if defined(CC_FAST_INIT) || defined(CC_SUPPORT_STR)
extern void VOPLL_pm_resume(void);
void vDrvDisplayInit_pm_resume(void)
{
  bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
  VOPLL_pm_resume();
  #if SUPPORT_DISPLAY
  printf("[SA7]vApiHwInit\n");	
  vDrvLVDSInit();
  vErrorHandleInit();
  vDrvDisplayInit();
  LOG(0, "Display Init");
  #endif
}
#endif

LINT_RESTORE
