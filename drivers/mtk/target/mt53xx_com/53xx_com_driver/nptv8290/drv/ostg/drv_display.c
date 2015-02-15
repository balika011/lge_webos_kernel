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
#include "panel.h"
#include "hw_vdoin.h"
#include "drv_vdoclk.h"
#include "util.h"
#include "hw_scpos.h"
#include "hw_mlvds.h"
#include "hw_tcon.h"
#include "hw_tg.h"
#include "hw_ycproc.h"
#include "hw_ospe.h"
#include "hw_ckgen.h"
#include "drv_scpos.h"
#include "hw_ckgen.h"
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
#include "vdp_display.h"
#include "source_select.h"
#include "drv_lcdim_v2.h"
#include "hw_sw.h"
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
#ifndef CC_MTK_LOADER
#include "drv_mjc.h"
#include "drv_mjc_if.h"
#include "hw_mjc.h"
#endif

#include "drv_tdtv.h"


#include "drv_pwm.h"
#include "drv_vdoclk.h"
#include "drv_scaler_drvif.h"
#include "frametrack_drvif.h"
#include "drvcust_if.h"  //mtk70763, 2010-7-21
#ifdef DRV_SUPPORT_EXTMJC
#include "extmjc_if.h"
#endif

#include "eeprom_if.h"
#include "osd_drvif.h"
#include "drv_scaler_psc.h"
#ifdef CC_SUPPORT_4K2K
#include "drv_mpeg.h"
#include "drv_dvi.h"
#endif
#ifdef CC_SUPPORT_W2DR2R
#include "drv_w2dr2r.h"
#endif
#include "drv_vbyone.h"
#include "b2r_if.h"
#include "pe_if.h"
#include "tve_if.h"
#include "drvcust_if.h"


#ifdef CC_SUPPORT_UHD
#include "drv_uhd.h"
#endif

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

#ifdef CC_FLIP_MIRROR_SUPPORT
static UINT8 u1Init = 0 ;
#endif


#if !defined(CC_MTK_LOADER) && defined(CC_SUPPORT_UHD)
EXTERN UINT8 fgUhdSetScPipdata;
#endif



UINT32 _u4NSDHTotal=0xff;
UINT32 _u4NSDVTotal=0xff;
UINT32 _u4NSDFrameRate=0xff;

BOOL fgPhotoRasterModeIn4k= SV_FALSE;//4k photo in video path

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
//Give initial value instead of using "CC_DVBT_SUPPORT" define
UINT8 bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
UINT8 bOUTPUT_REFRESH_RATE = 0;
UINT8 bFORCE_FRAMERATE_TYPE  = ~0;

UINT8 bDISPLAY_INTERLACE_ENABLE;
BOOL _fgFRS_ENA;
BOOL fgDDDSEn;
enum eSyncLockMode _eSyncLockMode = eSYNCLOCK_IGNORED;

UINT8 u1CustFRC;

//#ifdef SUPPORT_PANEL_SCAN_PWM
extern UINT32 _u4BrightLevel;
//#endif
#define ERRORLIMIT_INI 0x10
UINT32 _u4ErrorLimit = ERRORLIMIT_INI;

static UINT32 _u4OstgUpdateEvent = 0;

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

#if defined(CC_MT5890)
static UINT16 _u2IntFrameRate = 0;
#endif

UINT8 _bForceFrameRate = 0;
BOOL _fgFrTrkHalfStep = FALSE;
UINT8 _uDDDSDivision = 0;

UINT8 bSupportLocalDimming = SV_FALSE;

UINT16 w3DGlassesDelay = 0;

UINT8 fgPentouchMode = SV_OFF;
UINT8 fgApplyScalerSetData=0;
UINT8 fgBLCustomCtrl=0;  // 1: Backlight can be controled by the customer.

extern BOOL _fgVOPLLPowerOn;

BOOL fgVB1VideoMute = SV_FALSE;
BOOL fgBrowserModeEn = SV_FALSE;
BOOL fgSpeIPTVSEn = SV_FALSE;
#ifdef CC_MAPLE_CUST_DRV
#if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE) 
extern UINT8 u1BePath[3];
#endif
#endif

typedef enum
{
    VDP_PHOTO_2D       = 0,
    VDP_PHOTO_3D       = 1,
    VDP_PHOTO_3D_2_2D  = 2,
    VDP_PHOTO_2D_2_3D  = 3,
    VDP_PHOTO_NA 
}VDP_PHOTO_TYPE;

typedef struct{
    UINT16 u2FrameRate;
    UINT16 u2Width;
    UINT16 u2Height;
    UINT16 u2HTotal;
    UINT16 u2VTotal;
    UINT32 u4PixelClk;
    UINT16 u4VsyncWidth;
    UINT16 u4VsyncBp;
    UINT16 u4HsyncWidth;
    UINT16 u4HsyncFp;
}S_PANEL_CONFIG;

typedef struct{
    UINT32 u4PixleClockMax;
    UINT32 u4PixleClockMin;
    UINT16 u2HTotalMax;
    UINT16 u2HTotalMin;
    UINT16 u2VTotalMax;
    UINT16 u2VTotalMin;
    UINT16 u2VClkMax;
    UINT16 u2VClkMin;
}S_PANEL_CONFIG_COMMON;

static vDisplayTimingCB DiplayTimingCB[E_TIMEGEN_PATH_MAX][E_DISPLAY_TIMING_NUM_OF_CB] =
{
    {NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL},  // E_DISPLAY_TIMING_CB_H_ACTIVE
    {NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL},   // E_DISPLAY_TIMING_CB_H_TOTAL
    {NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL},   // E_DISPLAY_TIMING_CB_H_FP
};

UINT32 _4k2kPanelType = PANEL_TYPE_MAX;
BOOL fg4k2kPNLTypeChg= SV_FALSE;

#ifdef CC_SUPPORT_4K2K
UINT32 _u4I2CDly = 800;     // ms
UINT8 _u14K2KMode = E_4K2K_NORMAL;
UINT8 _u1Force4K2KTiming =0;
S_PANEL_CONFIG rPANEL_FHD120[3];  // [0]: for 48Hz, [1]: for 50Hz, [2]: for 60Hz
S_PANEL_CONFIG rPANEL_FHD60[3];
S_PANEL_CONFIG rPANEL_4K2K30[3];
S_PANEL_CONFIG rPANEL_4K1K60[3];
S_PANEL_CONFIG rREF_4K_TG[3];


S_PANEL_CONFIG_COMMON rPANEL_FHD120_COMMON;
S_PANEL_CONFIG_COMMON rPANEL_FHD60_COMMON;
S_PANEL_CONFIG_COMMON rPANEL_4K2K30_COMMON;
S_PANEL_CONFIG_COMMON rPANEL_4K1K60_COMMON;

HANDLE_T h4k2kUnMuteTimer ;

BOOL _fgBypassToPQ= SV_FALSE;

HANDLE_T _hFrcUnmuteSema=(HANDLE_T)(NULL);
static HANDLE_T _hFrcSetPanelSema=(HANDLE_T)(NULL);

static void vSavePanelAttribute_4K2K(void);
static void vSaveRefAttribute_4K2K(void);
static void vFrcUnmute(void *pvArgs);
#endif  //4k2k

#if defined(CC_MT5890)
#if defined(SUPPORT_PANEL_GRAPH_MODE)
UINT32 u4PreGFXType = 0xff;
#endif
#endif

BOOL _fgESDProtect = FALSE;
BOOL _fgVB1RegisterReMapping = FALSE;

#ifdef CC_PHP_SUPPORT
extern void DRVCUST_PQBAfterSocOutputChanged(UINT32 u4OutputHeigt,UINT32 u4OutputWidth,UINT8 u4OutputFrameRate);
#endif
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

UINT32 _u4TargetCW = 0;
UINT32 _u4CurrentCW = 0;
UINT32 _u4CWStep = 0x500000;
#endif

#ifdef CC_SCPOS_3DTV_SUPPORT
static void vDrvDisp3DModeCB(void);
static void vSavePanelAttribute(void);
static BOOL fgPDPL12R12 = ePanel_PDP_L1R1;
#ifdef DRV_SUPPORT_MT8283
BOOL fgForceChangeRegion = FALSE;
#endif
#endif

#endif

LCDTIMING_INFO_T lcdtimingInfo;
LCDTIMING_INFO_T ReftimingInfo;
extern UINT8 u1FlipMirrorReset;    //flip&mirror is effective after resume

//vbone control bit
#if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)
CONTROL_BIT_VB1_T  gVB1ControlBit;///control bit
E_TIMING_TRIGGER_STATE E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_IDLE;
#endif


#if defined(SUPPORT_PANEL_GRAPH_MODE)
TIMING_INFO_T GFXTimingInfo;
CONTROL_BIT_GFX_T gGFXControlBit;
#endif

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
UINT32 GetPANEL4K2K_TYPE(void)
{
    return _4k2kPanelType;
}

UINT16 wDrvGetOutputHTotal(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
    {
		if (bGetVideoDecType(SV_VP_MAIN) == SV_VD_DVI)
		{
			return ((UINT16)vR2rGetHTotal());
		}
		else  // new B2R
		{
			return (VDP_GetBypassHTotal()+1);
		}
	}
	else
	#endif
#endif
#endif
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
	        
	            return (UINT16)u4ScpipGetPanelHtotal(E_FSC_TG);
	        }
	    }
	}
}

void wDrvSetOutputHTotal(UINT16 u2HTotal)
{
    UINT16 u2OriHTotal;
    if (fgIsMJCToOSTG())
    {
        u2OriHTotal = u4PscGetPanelHTotal();
        if(u2OriHTotal != u2HTotal)
        {
            #ifndef CC_MTK_LOADER
            _vDrvVideoSetMute(MUTE_MODULE_MJC, SV_VP_MAIN, 5, SV_TRUE);
            #endif
        }
        vPscSetPanelHTotal(u2HTotal);
    }
    else
    {
        vScpipSetPanelHtotal(E_FSC_TG, u2HTotal);
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
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
	{
		if (bGetVideoDecType(SV_VP_MAIN) == SV_VD_DVI)
		{
			return ((UINT16)vR2rGetVTotal());
		}
		else  // new B2R
		{
			return (VDP_GetBypassVTotal()+1);
		}
	}
	else
	#endif
#endif
#endif
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
	            return ((UINT16) u4ScpipGetPanelVtotal(E_FSC_TG));
	        }
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
        vScpipSetPanelVtotal(E_FSC_TG,u2VToal);
    }

    if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)||(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
    {
       vIO32WriteFldAlign(PIXEL_REORDER_15,1,RG_VWIDTH_R);
       vIO32WriteFldAlign(PIXEL_REORDER_01,1,RG_VS_OUT_POL);
       vIO32WriteFldAlign(PIXEL_REORDER_16,8,RG_VDE_VSTART_R);

       vIO32WriteFldAlign(VINTF_B3, 8, VINTF_PTGEN_V_START);
	   vIO32WriteFldAlign(MUTE_PATTERN_04,u2VToal,MUTE_V_TOTAL);
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
        vScpipSetPanelWidth(E_FSC_TG,u2HActive);
    }

#ifndef CC_MTK_LOADER
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

UINT16 wDrvGetOutputHActive(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
    {
		if (bGetVideoDecType(SV_VP_MAIN) == SV_VD_DVI)
		{
			return ((UINT16)vR2rGetHActive());
		}
		else  // new B2R
		{
			return (VDP_GetNB2R_HActive()+1);
		}
	}
	else
	#endif
#endif
#endif
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
	        
	            return ((UINT16) u4ScpipGetPanelWidth(E_FSC_TG));
	        }
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
        vScpipSetPanelHeight(E_FSC_TG,u2VActive);
    }

#ifndef CC_MTK_LOADER
    vVRMSetEventFlg(SV_VP_MAIN, VRM_EVENT_BY_PANEL);
    vVRMSetEventFlg(SV_VP_PIP , VRM_EVENT_BY_PANEL);
#endif

}

UINT16 wDrvGetOutputVActive(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
    {
		if (bGetVideoDecType(SV_VP_MAIN) == SV_VD_DVI)
		{
			return ((UINT16)vR2rGetVActive());
		}
		else  // new B2R
		{
			return (VDP_GetNB2R_VActive()+1);
		}
	}
	else
	#endif
#endif
#endif
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
	            return ((UINT16) u4ScpipGetPanelHeight(E_FSC_TG));
	        }
	    }
	}
}

void vDrvSetHsyncFp(UINT16 u2Fp)
{

    if (fgIsMJCToOSTG())
    {
        vPscSetPanelHSyncFp(u2Fp);
    }
    else
    {
        vScpipSetHsyncFp(E_FSC_TG,u2Fp);
    }
}

void vDrvSetHsyncBp(UINT16 u2Bp)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelHSyncBp(u2Bp);
    }
    else
    {
        vScpipSetHsyncBp(E_FSC_TG,u2Bp);
    }
}

UINT16 wDrvGetHsyncBp(void)
{
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
	{
		return ((UINT16)(lcdtimingInfo.u4HBP>>1));
	}
	else
	#endif
#endif
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
	            return ((UINT16)u4ScpipGetHsyncBp(E_FSC_TG));
	        }
	    }
	}
}

UINT16 wDrvGetHsyncFp(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
	{
		return ((UINT16)(lcdtimingInfo.u4HFP>>1));
	}
	else
	#endif
#endif
#endif
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
	         return ((UINT16)u4ScpipGetHsyncFp(E_FSC_TG));
	        }
	    }
	}
}

UINT16 wDrvGetHsyncWidth(void)
{
    UINT16 u2SyncWidth=0;

    u2SyncWidth =  wDrvGetOutputHTotal() - wDrvGetOutputHActive()- wDrvGetHsyncFp()-wDrvGetHsyncBp();
    return ((UINT16)u2SyncWidth);
}

void vDrvSetVsyncFp(UINT16 u2VFp)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetPanelVSyncFp(u2VFp);
    }
    else
    {
        vScpipSetVsyncFp(E_FSC_TG,u2VFp);
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
        vScpipSetVsyncBp(E_FSC_TG,u2VBp);
    }
}

UINT16 wDrvGetVsyncFp(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
	{
		return ((UINT16)lcdtimingInfo.u4VFP);
	}
	else
	#endif
#endif
#endif
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
	            return ((UINT16)u4ScpipGetVsyncFp(E_FSC_TG));
	        }
	    }
	}
}

UINT16 wDrvGetVsyncBp(void)
{
#ifndef CC_MTK_LOADER
#ifdef CC_MT5890
	#if defined(CC_SUPPORT_W2DR2R)
	if((GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60) && fgIsMainDec())
	{
		return ((UINT16)lcdtimingInfo.u4VBP);
	}
	else
	#endif
#endif
#endif
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
	            return ((UINT16)u4ScpipGetVsyncBp(E_FSC_TG));
	        }
	    }
	}
}

UINT16 wDrvGetVsyncWidth(void)
{
    UINT16 u2SyncWidth=0;

    u2SyncWidth =  wDrvGetOutputVTotal() - wDrvGetOutputVActive()- wDrvGetVsyncFp()-wDrvGetVsyncBp();
    return ((UINT16)u2SyncWidth);
}

void vDrvSet2SECVTotal(UINT16 u2VToal)
{
    vIO32WriteFldAlign(VINTF_P29,  (u2VToal-1),  VINTF_P29_2SEC_V_TOTAL);
    LOG(3, "[VB1][2-SEC] 2SEC_V_total=%d\n", IO32ReadFldAlign(VINTF_P29, VINTF_P29_2SEC_V_TOTAL));
}

void vDrvSet2SECVsyncWidth(UINT16 u2VsyncWidth)
{
    vIO32WriteFldAlign(VINTF_P29,  (u2VsyncWidth-1),  VINTF_P29_2SEC_V_WIDTH);
    LOG(3, "[VB1][2-SEC] 2SEC_V_width=%d\n", IO32ReadFldAlign(VINTF_P29, VINTF_P29_2SEC_V_WIDTH));
}

#ifndef CC_MTK_LOADER
/**
 * @brief fgDrvSupport24Hz

 * use variable to control the frame rate control under 24Hz

 * @param  void
 * @retval TRUE/FALSE
 */
UINT8 fgDrvSupport24Hz(void)
{
    if (IS_PANEL_2D_N_3D_L12R12)
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

UINT8 u1DrvCal48HzOutputType(void)
{
	if (SUPPORT_PANEL_48HZ)
		return (SV_DCLK_48HZ);
	else if (IS_DISP_48HZ_TO_50HZ)
		return (SV_DCLK_50HZ);
	else // define DISP_48HZ_TO_60HZ or define none
		return (SV_DCLK_60HZ);
}

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
   #if defined(CC_MT5890)
    if (fgOnOff == IO32ReadFldAlign(OSTG_01, REG_MJC_IN_EN))
    {
        return FALSE;
    }

    vIO32WriteFldAlign(OSTG_01, fgOnOff, REG_MJC_IN_EN);
    vDrvSetPanelTiming();

    return TRUE;
    #else
    return FALSE;
    #endif
}

UINT32 fgIsMJCToOSTG(void)
{
    #if defined(CC_MT5890)
    return (IO32ReadFldAlign(OSTG_01, REG_MJC_IN_EN) == 1);
    #else
    return FALSE;
    #endif
}

 #ifndef CC_MTK_LOADER

#define SUPPORT_IN_LOW_FRAME_RATE 36
/**
 * @brief vDrvSetMTKGoodDclk

 * Set MTK Good Dclk Parameters

 * @param  wVERT_LINE: Input Total Lines in the Vertical Direction
 * @retval void
 */
 #define ORYX_SYSPLL_CLK_SS 648000000
void vDrvSetMTKGoodDclk(UINT16 u2InputVTotal, UINT8 bInputFrameRate) // Called by Video
{
    UINT32 DISPLAY_SIZE, HLEN_INT, HLEN_NUM;
    UINT32 u4HTotal, u4VTotal;
	#if ENABLE_DDDS2_FEEDBACK
    UINT8 u1VopllRatio;
	#endif
	UINT32 u4MUTE_CNT_LIMIT;
	UINT8 bType;

    if ((u2InputVTotal == 0) || (bInputFrameRate == 0))
    {
        LINT_SUPPRESS_NEXT_EXPRESSION(527);
        return;
    }
	//for FP FS, case, bInputFrameRate is half of real input video frame in vidoe path
	bInputFrameRate <<= (u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_FS) ? 1 : 0;


	bType = bGetVideoDecType(SV_VP_MAIN);
	vDrvSetDDDSInput(bType);
	
	if(bDrvVideoIsSrcInterlace(SV_VP_MAIN))
	u4MUTE_CNT_LIMIT = (ORYX_SYSPLL_CLK_SS/(u2InputVTotal*bInputFrameRate));
	else
	u4MUTE_CNT_LIMIT = (ORYX_SYSPLL_CLK_SS/(u2InputVTotal*bInputFrameRate))>>1;
	
	u4MUTE_CNT_LIMIT = (u4MUTE_CNT_LIMIT>0)?(u4MUTE_CNT_LIMIT-1):0;
	vIO32WriteFldAlign(DDDS_12, u4MUTE_CNT_LIMIT, DDDS_12_MUTE_CNT_LIMIT);

    u4HTotal = (UINT32)wDrvGetOutputHTotal();
    u4VTotal = (UINT32)wDrvGetOutputVTotal();
    DISPLAY_SIZE = u4HTotal * u4VTotal;

    DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate;
    LOG(3, "5DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);

    DISPLAY_SIZE = bDrvVideoIsSrcInterlace(SV_VP_MAIN) ? DISPLAY_SIZE * 2 : DISPLAY_SIZE;

    #if ENABLE_DDDS2_FEEDBACK
    if ((LVDS_OUTPUT_PORT == SINGLE_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=1;
    else  if (LVDS_OUTPUT_PORT == SINGLE_PORT) u1VopllRatio=0;
    else  if ((LVDS_OUTPUT_PORT == DUAL_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=2;
    else  if ((LVDS_OUTPUT_PORT == DUAL_PORT)&&(IS_PANEL_L12R12)) u1VopllRatio=2;
    else  if (LVDS_OUTPUT_PORT == DUAL_PORT) u1VopllRatio=1;
    else  if ((LVDS_OUTPUT_PORT == FOUR_PORT)&&(IS_LVDS_HIGHSPEED)) u1VopllRatio=3;
    else  if ((LVDS_OUTPUT_PORT == FOUR_PORT)&&(IS_PANEL_L12R12)) u1VopllRatio=3;
    else  if (LVDS_OUTPUT_PORT == OCTA_PORT)
    {
    	if (PANEL_IsSupport4K2KDD())
		u1VopllRatio=3;
		else
		u1VopllRatio=2;
    }
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

    #ifdef SUPPORT_DDDS_STEP_TRACKING
    vIO32WriteFldAlign(DDDS_03,_u4ErrorLimit,DDDS_03_DDDS_ERR_LIM);
    #endif
    vIO32WriteFldAlign(DDDS_02,(UINT16)u2InputVTotal,DDDS_02_HLEN_DEN);
    vIO32WriteFldAlign(DDDS_01,(UINT16)HLEN_NUM,DDDS_01_HLEN_NUM);
    vIO32WriteFldAlign(DDDS_01,HLEN_INT,DDDS_01_HLEN_INT);

   // LOG(3, "HLEN=0x%x . [0x%x / 0x%x] \n",HLEN_INT, HLEN_NUM,u2InputVTotal);
}

static BOOL vDrvDDDSLockStatusCnt(UINT32 u4Count)
{
 UINT32 u4LoopCnt=u4Count;
 UINT32 u4UnlockCnt=0;

 do
 {
    if (!bDrvCheckMTKGoodStatus())
    {
    	u4UnlockCnt++;
    }
    u4LoopCnt--;
	HAL_Delay_us(100);
 }while(u4LoopCnt);
 
 if(u4UnlockCnt)
 {
	return 0;   
 }
 else
 {
	 return 1;
 }
}
 
void vScanGoodDclkRegion(UINT16 u2StartHsyncFreg, UINT16 u2EndHsyncFreg, UINT16 u2step)
{
	UINT16 u2votal, u2HsyncFreq;
	UINT16 u2Framerate=bDrvVideoGetRefreshRate(SV_VP_MAIN);
	UINT16 u2delayCnt=0;
	BOOL fglockstatus=0;

	if((IO32ReadFldAlign(DDDS_02, DDDS_02_SCPOS_H_SEL)==4)||(IO32ReadFldAlign(DDDS_00, DDDS_00_DISP_EN)==0))
	{
		Printf("Open loop/ mute clk \n");
		return;
	}

	if(u2Framerate ==0)	u2Framerate = 60;

	Printf("u2StartHsyncFreg =%d, u2EndHsyncFreg = %d,u2step =%d u2Framerate = %d \n", u2StartHsyncFreg,u2EndHsyncFreg, u2step, u2Framerate);
	vIO32WriteFldAlign(DDDS_0A, 1, DDDS_0A_SC_DEBUG_MODE);	
	vIO32WriteFldAlign(DDDS_0A, 1, DDDS_0A_MJC_DEBUG_MODE);		
	vIO32WriteFldAlign(DDDS_03, 0x1F, DDDS_03_DDDS_ERR_LIM);
	vIO32WriteFldAlign(DDDS_0C, 0x0, DDDS_0C_VSYNC_TRACK_EN);	
	
	for(u2HsyncFreq=u2StartHsyncFreg; u2HsyncFreq<=u2EndHsyncFreg; u2HsyncFreq+=u2step)
	{
	 vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN)|P_Fld(0,DDDS_00_FIX_FS_DDDS_SEL));
	 for(u2delayCnt=0; u2delayCnt<=1000; u2delayCnt++)	 
	 HAL_Delay_us(1000);  // 1ms	 
	 u2votal = u2HsyncFreq * 100 / u2Framerate;
	 vDrvSetMTKGoodDclk(u2votal, u2Framerate);
	 vIO32WriteFldMulti(DDDS_00,P_Fld(1,DDDS_00_DISP_EN)|P_Fld(1,DDDS_00_FIX_FS_DDDS_SEL));
	 for(u2delayCnt=0; u2delayCnt<=1000; u2delayCnt++)	 
	 HAL_Delay_us(1000);  // 1ms
	 fglockstatus = vDrvDDDSLockStatusCnt(10000);
	 if(fglockstatus)
     Printf("lock = %d, u2HsyncFreq=%3d, u2votal = %d HLEN INT+NUM/DEN %d+%d/%d \n ", fglockstatus, u2HsyncFreq, u2votal, 
	  		 IO32ReadFldAlign(DDDS_01 ,DDDS_01_HLEN_INT) , IO32ReadFldAlign(DDDS_01 ,DDDS_01_HLEN_NUM), IO32ReadFldAlign(DDDS_02 ,DDDS_02_HLEN_DEN));
	 else
	 Printf("lock = %d, u2HsyncFreq=%3d, u2votal = %d\n ", fglockstatus, u2HsyncFreq, u2votal);	 
	}
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
     vIO32WriteFldAlign(DDDS_03, 1, DDDS_03_SPREAD_INIT);
     #ifdef CC_COPLAT_MT82
     vUtDelay2us(50);
     #else
     HAL_Delay_us(100);
     #endif
     vIO32WriteFldAlign(DDDS_03, 0, DDDS_03_SPREAD_INIT);
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
	#if 0
    	vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN)|P_Fld(0,DDDS_00_FIX_FS_DDDS_SEL));
	#else
    	vIO32WriteFldAlign(DDDS_02, 4, DDDS_02_SCPOS_H_SEL);  // use MUTE_HSYNC
	#endif
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
    if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1) && (IS_STEP_CLOCK_CHG))
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
          if(((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1) && (IS_STEP_CLOCK_CHG))||
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
#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1) && (IS_STEP_CLOCK_CHG))
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
        if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1) && (IS_STEP_CLOCK_CHG))
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

void vDrvVsyncISRPositionTrigger(UINT16 u2INTRLine)
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

static void vDrvVsyncISRSetLineCounter(UINT16 u2Vfp)
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

static void vDrvVsyncISRInit(void)
{
    vDrvVsyncISRPositionTrigger(0);
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

#if defined(SUPPORT_PANEL_GRAPH_MODE)
UINT32 u4GetGraphFrameRate(void)
{
    return (UINT32)GFXTimingInfo.u1FrameRate;
}

void vGetGraphModeScalerStatus(void)
{
    Printf("===== [Graph Setting] =====\n");
    Printf("Frame-rate = %d Hz\n", GFXTimingInfo.u1FrameRate);
    Printf("[Hori] HTotal=%04d HActive=%4d HFP=%4d HSyncWidth=%3d HBP=%4d\n",
        u4ScpipGetPanelHtotal(E_GFX_TG),
        u4ScpipGetPanelWidth(E_GFX_TG),
        u4ScpipGetHsyncFp(E_GFX_TG),
        (u4ScpipGetPanelHtotal(E_GFX_TG) - u4ScpipGetPanelWidth(E_GFX_TG) - u4ScpipGetHsyncFp(E_GFX_TG) - u4ScpipGetHsyncBp(E_GFX_TG)),
        u4ScpipGetHsyncBp(E_GFX_TG)
        );
    Printf("[Vert] VTotal=%04d VActive=%4d VFP=%4d VSyncWidth=%3d VBP=%4d\n",
        u4ScpipGetPanelVtotal(E_GFX_TG),
        u4ScpipGetPanelHeight(E_GFX_TG),
        u4ScpipGetVsyncFp(E_GFX_TG),
        (u4ScpipGetPanelVtotal(E_GFX_TG) - u4ScpipGetPanelHeight(E_GFX_TG) - u4ScpipGetVsyncFp(E_GFX_TG) - u4ScpipGetVsyncBp(E_GFX_TG)),
        u4ScpipGetVsyncBp(E_GFX_TG)
        );
}
#endif

void vGetPostScalerStatus(void)
{
    UINT8 u1Cnt;

#ifndef CC_MTK_LOADER	
		Printf("_fgESDProtect=%d\n", _fgESDProtect);
#endif

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


static void vSetOSD_IndexEncodeEnable(BOOL bEn)
{
    vIO32WriteFldAlign(LVDSB_REG19,bEn,RG_RES_OSD_INDEX);
    vIO32WriteFldAlign(LVDSB_REG19,bEn,RG_RES_OSD_INDEX_L);
    vIO32WriteFldAlign(LVDSB_REG19,bEn,RG_RES_OSD_INDEX_U_4CH);
    vIO32WriteFldAlign(LVDSB_REG19,bEn,RG_RES_OSD_INDEX_L_4CH);
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

BOOL fgIsVsyncISRInit = SV_FALSE;
extern BOOL fgIsVsyncIsrStart;

extern UINT8 fgApplyScanPWMSetData;
extern UINT8 u1ScanPwmCrossFlg;

vDisplayTimingCB fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH ePath, E_DISPLAY_TIMING_CB eType, vDisplayTimingCB fpDispTimingCB)
{
    vDisplayTimingCB fpPreCB = DiplayTimingCB[ePath][eType];
    DiplayTimingCB[ePath][eType] = fpDispTimingCB;

    return fpPreCB;
}

#if defined(CC_MT5890)
#ifdef CC_SUPPORT_4KBE
void vDrvVB1ControlBitInit(void)
{
    if (PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
        vDrvVByOneCrtlBitCBV(SV_OFF);
        vDrvVByOneCrtlBitTRIG(SV_OFF);// 1:effective immediately 0:effective until next V
        vDrvVByOneCrtlBitEnable(SV_ON);// 1:use different register 4 lane 0:use one lane
        vDrvVByOneCrtlBitDisTRIG(SV_OFF);// 1:do not effective 0:disable
        vDrvVByOneCrtlBitVEDGE(eVB1_ControlBit_FALLING);
    }
}

void vDrvSetVB1ControlBit(void)
{
    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        vDrvVByOneCrtlBit3DST(gVB1ControlBit.u13DStructure);
        vDrvVByOneCrtlBitPICHSize(gVB1ControlBit.u4PicHSize);
        vDrvVByOneCrtlBitPICVSize(gVB1ControlBit.u4PicVSize);
        vDrvVByOneCrtlBitCROPHPos(gVB1ControlBit.u4CropHPosition);
        vDrvVByOneCrtlBitCROPVPos(gVB1ControlBit.u4CropVPosition);
        vDrvVByOneCrtlBitCROPHSize(gVB1ControlBit.u4CropHSize);
        vDrvVByOneCrtlBitCROPVSize(gVB1ControlBit.u4CropVSize);
        vDrvVByOneCrtlBitAT(gVB1ControlBit.u1AspectType);
        vDrvVByOneCrtlBitARLsb(gVB1ControlBit.u1AspectRatio & 0x0F);
        vDrvVByOneCrtlBitARMsb((gVB1ControlBit.u1AspectRatio & 0x10)>>4);
        vDrvVByOneCrtlBitSARHSize(gVB1ControlBit.u4SarHSize);
        vDrvVByOneCrtlBitSARVSize(gVB1ControlBit.u4SarVSize);
        vDrvVByOneCrtlBitCTLsb(gVB1ControlBit.u1CodeType & 0x0F);
        vDrvVByOneCrtlBitCTMsb((gVB1ControlBit.u1CodeType & 0x10)>>4);
        vDrvVByOneCrtlBitRF(gVB1ControlBit.u1DoubleRF);
        vDrvVByOneCrtlBitVP(gVB1ControlBit.u1VideoPath);
        vDrvVByOneCrtlBitCF(gVB1ControlBit.u1ColorFormat);
        vDrvVByOneCrtlBitRA(gVB1ControlBit.u1Range);
        //vDrvVByOneCrtlBitCBV(gVB1ControlBit.u1CtrlBitValid);
    }
}

void vDrvUpdateVB1ControlBit(CONTROL_BIT_VB1_T* ptVB1ControlBit)
{
    if (ptVB1ControlBit == NULL)
    {
       LOG(0, "Error :ptVB1ControlBit == NULL\n");
       return; 
    }

    LOG(2, "B2R Update the Bypass Control Bit Info\n");
    x_memcpy((void*)(&gVB1ControlBit), (void*)ptVB1ControlBit, sizeof(CONTROL_BIT_VB1_T));
    E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_CTRL_BIT;
}

void vDrvdumpControlBitinfo(void)
{
    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        LOG(0, "===========VB1 Control Bit Info============\n");
        LOG(0, "u1VideoPath %d    u1CtrlBitValid %d    u13DStructure %d\n",gVB1ControlBit.u1VideoPath,gVB1ControlBit.u1CtrlBitValid,gVB1ControlBit.u13DStructure);
        LOG(0, "u1Range     %d    u1DoubleRF     %d    u1ColorFormat %d\n",gVB1ControlBit.u1Range,gVB1ControlBit.u1DoubleRF,gVB1ControlBit.u1ColorFormat);
        LOG(0, "u1CodeType  %d    u1AspectType   %d    u1AspectRatio %d\n",gVB1ControlBit.u1CodeType,gVB1ControlBit.u1AspectType,gVB1ControlBit.u1AspectRatio);
        LOG(0, "u4SarHSize  %d    u4SarVSize     %d    u4PicHSize    %d  u4PicVSize   %d\n",gVB1ControlBit.u4SarHSize,gVB1ControlBit.u4SarVSize,gVB1ControlBit.u4PicHSize,gVB1ControlBit.u4PicVSize);
        LOG(0, "u4CropHPos  %d    u4CropVPos     %d    u4CropHSize   %d  u4CropVSize  %d\n",gVB1ControlBit.u4CropHPosition,gVB1ControlBit.u4CropVPosition,gVB1ControlBit.u4CropHSize,gVB1ControlBit.u4CropVSize);

        #if defined(SUPPORT_PANEL_GRAPH_MODE)
        LOG(0, "===========GFX Control Bit Info===============\n");
        LOG(0, "ValidFlag    %d       Graphic Format  %d\n",gGFXControlBit.u4CBVF,gGFXControlBit.u4CBGF);
        LOG(0, "HStart Pos   %d     VStart Pos      %d\n",gGFXControlBit.u4CBHSPos,gGFXControlBit.u4CBVSPos);
        LOG(0, "HTotal       %d    VTotal          %d\n",gGFXControlBit.u4CBHTOTAL,gGFXControlBit.u4CBVTOTAL);
        #endif       
    }
    else
    {
        LOG(0, "Error :PANEL_DISPLAY_TYPE_VB1 = 0\n");
    }
}

void vDrvSetControlBitinfo(UINT8 u1path,UINT8 u1sel2,UINT32 u4value)
{
    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        if(u1path == 0) // vbo control bit
        {
            switch (u1sel2)
            {
                case E_CONTROL_BIT_CB_3DST:
                     gVB1ControlBit.u13DStructure = u4value;
                     break;
                case E_CONTROL_BIT_CB_HSZ:
                     gVB1ControlBit.u4PicHSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_VSZ:
                     gVB1ControlBit.u4PicVSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_CRHP:
                     gVB1ControlBit.u4CropHPosition = u4value;
                     break;
                case E_CONTROL_BIT_CB_CRVP:
                     gVB1ControlBit.u4CropVPosition = u4value; 
                     break;
                case E_CONTROL_BIT_CB_CRHSZ:
                     gVB1ControlBit.u4CropHSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_CRVSZ:
                     gVB1ControlBit.u4CropVSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_AT:
                     gVB1ControlBit.u1AspectType = u4value;
                     break;
                case E_CONTROL_BIT_CB_AR:
                     gVB1ControlBit.u1AspectRatio = u4value;
                     break;
                case E_CONTROL_BIT_CB_SHSZ:
                     gVB1ControlBit.u4SarHSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_SVSZ:
                     gVB1ControlBit.u4SarVSize = u4value;
                     break;
                case E_CONTROL_BIT_CB_CT:
                     gVB1ControlBit.u1CodeType = u4value;
                     break;
                case E_CONTROL_BIT_CB_RF:
                     gVB1ControlBit.u1DoubleRF = u4value;
                     break;
                case E_CONTROL_BIT_CB_VP:
                     gVB1ControlBit.u1VideoPath = u4value;
                     break;
                case E_CONTROL_BIT_CB_CF:
                     gVB1ControlBit.u1ColorFormat = u4value;
                     break;
                case E_CONTROL_BIT_CB_RA:
                     gVB1ControlBit.u1Range = u4value;
                     break;
                case E_CONTROL_BIT_CB_CBV:
                     gVB1ControlBit.u1CtrlBitValid = u4value;
                     break;
                default:
                     break;
            }
            E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_CTRL_BIT;                
        }
        
        #if defined(SUPPORT_PANEL_GRAPH_MODE)
        if (u1path == 1) // gfx control bit
        {
            switch (u1sel2)
            {
                case E_CONTROL_BIT_CB_HTOTAL:
                     gGFXControlBit.u4CBHTOTAL = u4value;
                     break;
                case E_CONTROL_BIT_CB_VTOTAL:
                     gGFXControlBit.u4CBVTOTAL = u4value;
                     break;
                case E_CONTROL_BIT_CB_HSPos:
                     gGFXControlBit.u4CBHSPos = u4value;
                     break;
                case E_CONTROL_BIT_CB_VSPos:
                     gGFXControlBit.u4CBVSPos = u4value;
                     break;
                case E_CONTROL_BIT_CB_GF:
                     gGFXControlBit.u4CBGF = u4value;
                     break;
                case E_CONTROL_BIT_CB_VF:
                     gGFXControlBit.u4CBVF = u4value;
                     break;                    
                default:
                    break;
            } 
            GFXTimingInfo.u1TriggerState = (UINT8)E_TIMING_TRIGGER_STATE_CTRL_BIT;
        }
        #endif         
    }
    else
    {
        LOG(0, "Error :PANEL_DISPLAY_TYPE_VB1 = 0\n");
    }
    
}

void vDrvControlBitinfoTrigger(UINT8 u1sel)
{
	if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
	{
		if (u1sel == 0)
	    {
			vDrvSetVB1ControlBit();
	    }
	    
	    else if(u1sel == 1)
	    {
	    #if defined(SUPPORT_PANEL_GRAPH_MODE)
			if (IS_SUPPORT_VB1_GRAPH_MODE)
			{
	       		vDrvSetGFXControlBit();
			}
		#endif
	    }
	    
	    else if (u1sel == 2)
	    {
	        vDrvSetVB1ControlBit();
	    #if defined(SUPPORT_PANEL_GRAPH_MODE)
			if (IS_SUPPORT_VB1_GRAPH_MODE)
			{
	        	vDrvSetGFXControlBit(); 
			}
	    #endif
	    }
	}
	else
	{
	    LOG(0, "Error :PANEL_DISPLAY_TYPE_VB1 = 0\n");
	}
    
}
#endif
#endif

UINT8 fgIsOSDFromGFX(void)
{
	if(IO32ReadFldAlign(SCPIP_SCSYS_14,SCSYS_14_SC_OSD_SRC_SEL)==2)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

#ifdef CC_MT5890
void vDrvSwitchOSDRoute(void)
{
    if(IS_SUPPORT_VB1_GRAPH_MODE != 0)
	{
	     LOG(0, "vDrvSwitchOSDRoute = 2\n");  
	     vIO32WriteFldAlign(SCPIP_SCSYS_14, 0x2, SCSYS_14_SC_OSD_SRC_SEL);
	  //vIO32WriteFldAlign(GC_MIX_02, 0x0, GC_MIX_02_VIDEO_SEL_MAIN); 
	  //vIO32WriteFldAlign(GC_MIX_02, 0x0, GC_MIX_02_TIMING_SEL_MAIN); // from GFX timing GEN
	}
	else
	{
	     LOG(0, "vDrvSwitchOSDRoute = %d    pnl ctrl word=0x%x\n",fgIsMJCToOSTG(),PANEL_GetControlWord2());
		 vIO32WriteFldAlign(SCPIP_SCSYS_14, fgIsMJCToOSTG()? 0x1 : 0x0, SCSYS_14_SC_OSD_SRC_SEL);
	}
}
#endif

#if defined(SUPPORT_PANEL_GRAPH_MODE)
void vDrvGFXInit(void)
{
    if (PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
        //vDrvVByOneGMCrtlBitVF(SV_OFF);
        vDrvVByOneGMCrtlBitTRIG(SV_OFF); // 1:effective immediately 0:effective until next V
        vDrvVByOneGMCrtlBitEnable(SV_ON);// 1:use different register 4 lane 0:use one lane
        vDrvVByOneGMCrtlBitDisTRIG(SV_OFF);// 1:do not effective 0:disable
        vDrvVByOneGMCrtlBitVEDGE(eVB1_ControlBit_FALLING);
    }
}

void vDrvSetGFXControlBit(void)
{
    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        //set register
        //vDrvVByOneGMCrtlBitVF(gGFXControlBit.u4CBVF);
        vDrvVByOneGMCrtlBitGF(gGFXControlBit.u4CBGF);
        vDrvVByOneGMCrtlBitHTotal(gGFXControlBit.u4CBHTOTAL);
        vDrvVByOneGMCrtlBitVTotal(gGFXControlBit.u4CBVTOTAL);
        vDrvVByOneGMCrtlBitHDEStart(gGFXControlBit.u4CBHSPos);
        vDrvVByOneGMCrtlBitVDEStart(gGFXControlBit.u4CBVSPos);
    }
}

void vDrvUpdateGFXControlBit(void)
{
    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        //update structrue
        gGFXControlBit.u4CBVF = SV_ON;
        gGFXControlBit.u4CBGF = GFXTimingInfo.u4VActive > 1080;
        gGFXControlBit.u4CBHTOTAL = GFXTimingInfo.u4HTotal;
        gGFXControlBit.u4CBVTOTAL = GFXTimingInfo.u4VTotal;
        gGFXControlBit.u4CBHSPos = GFXTimingInfo.u4HSyncWidth + GFXTimingInfo.u4HBP;
        gGFXControlBit.u4CBVSPos = GFXTimingInfo.u4VSyncWidth + GFXTimingInfo.u4VBP;
    }
    
}


void vDrvSwitchGFXTiming(UINT32 u4Type)
{
    if (IS_SUPPORT_VB1_GRAPH_MODE == 0)
    {
        LOG(2, "[vDrvSwitchGFXTiming] Not support Vx1 graph mode %d\n",u4Type);
        return;
    }

    if (u4Type == GraphMode_GetPanelType())
    {
        LOG(2, "[vDrvSwitchGFXTiming] The same graph mode %d\n",u4Type);
        return;
    }

	LOG(2,"[vDrvSwitchGFXTiming] panel_type = %d\n", u4Type);
    GraphMode_SetPanelType(u4Type);

    #if defined(CC_MT5890) && (!defined(CC_MTK_LOADER))
    vSetPlaneEnable(VDP_GFXOUT, SV_ON);
    if (u4Type == PANEL_TYPE_4K1K_60)
    {
        vSetPlaneWidth(VDP_GFXOUT,  1920);
        vSetPlaneHeight(VDP_GFXOUT, 1080);
    }
    else if (u4Type == PANEL_TYPE_4K2K_30)
    {     
        vSetPlaneWidth(VDP_GFXOUT,  3840);
        vSetPlaneHeight(VDP_GFXOUT, 2160);
    }
    else if (u4Type == PANEL_TYPE_2K1K_60)
    {
        vSetPlaneWidth(VDP_GFXOUT,  1920);
        vSetPlaneHeight(VDP_GFXOUT, 1080);
    }	
#ifdef CC_SUPPORT_UHD	
    else if (u4Type == PANEL_TYPE_2K2K_60)
    {
        vSetPlaneWidth(VDP_GFXOUT,  1920);
        vSetPlaneHeight(VDP_GFXOUT, 2160);
    }	
#endif	
	
	if(IS_DISP_4K2K_TYPE3)
	{
		UINT32 srcW=0,srcH=0,dstW=0,dstH=0,Enable=0,u4bpp=0;
		UINT32 u4Width = u4GetPlaneWidth(VDP_GFXOUT);
		UINT32 u4Height = u4GetPlaneHeight(VDP_GFXOUT);
		OSD_SC_GetScalerInfo(OSD_SCALER_3,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_3,Enable,srcW,srcH,u4Width,u4Height);
		OSD_SC_GetScalerInfo(OSD_SCALER_2,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_2,Enable,srcW,srcH,u4Width,u4Height);
		OSD_SC_GetScalerInfo(OSD_SCALER_1,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_1,Enable,srcW,srcH,u4Width,u4Height);
    }	
    #endif
    
    GFXTimingInfo.u1DCLKType = bSI_DISPLAY_DCLK_TYPE;
    vDrvSetGFXTiming(GFXTimingInfo.u1DCLKType);
	#if 0//ndef CC_MTK_LOADER
	#if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)   
	//DRVCUST_BE_Connect(E_CUST_BE_TX_PATH_GFX,E_CUST_BE_SCL_TYPE_GFX,E_CUST_BE_PLANE_ORDER_2,SV_VP_NA);
	#endif
	#endif
}

void vDrvSetGFXTiming(UINT8 u1GfxDclkType)
{
    GFXTimingInfo.u1DCLKType = u1GfxDclkType;

	if (u1GfxDclkType == SV_DCLK_48HZ)
	{
		u1GfxDclkType = u1DrvCal48HzOutputType();
	}

    switch (u1GfxDclkType)
    {
		case SV_DCLK_48HZ:
			GFXTimingInfo.u1FrameRate = 48;
			break;

		case SV_DCLK_50HZ:
			GFXTimingInfo.u1FrameRate = 50;
			break;

		case SV_DCLK_60HZ:
		default:
			GFXTimingInfo.u1FrameRate = 60;
			break;
    }

    if (GraphMode_GetFrameRateMax() <= 30)
    {
        GFXTimingInfo.u1FrameRate >>= 1;
    }

    if (GFXTimingInfo.u4HTotal == GRAPHMODE_GetHtotalByTiming(GFXTimingInfo.u1DCLKType) &&
        GFXTimingInfo.u4VTotal == GRAPHMODE_GetVtotalByTiming(GFXTimingInfo.u1DCLKType))
    {
        return;
    }

    {
        #ifndef CC_MTK_LOADER
        CRIT_STATE_T csState;
        csState = x_crit_start();
        #endif
        GFXTimingInfo.u4HTotal      = GRAPHMODE_GetHtotalByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4HActive     = GRAPHMODE_GetPanelWidth();
        GFXTimingInfo.u4HFP         = GRAPHMODE_GetHSyncFpByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4HSyncWidth  = GRAPHMODE_GetHSyncWidthByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4HBP         = GRAPHMODE_GetHSyncBpByTiming(GFXTimingInfo.u1DCLKType);

        GFXTimingInfo.u4VTotal      = GRAPHMODE_GetVtotalByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4VActive     = GRAPHMODE_GetPanelHeight();
        GFXTimingInfo.u4VFP         = GRAPHMODE_GetVSyncFpByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4VSyncWidth  = GRAPHMODE_GetVSyncWidthByTiming(GFXTimingInfo.u1DCLKType);
        GFXTimingInfo.u4VBP         = GRAPHMODE_GetVSyncBpByTiming(GFXTimingInfo.u1DCLKType);

        vDrvUpdateGFXControlBit();
        #ifndef CC_MTK_LOADER
        x_crit_end(csState);
        #endif
    }

    #ifndef CC_MTK_LOADER
    if ((fgIsVsyncIsrStart) && (_fgVOPLLPowerOn))
    {
        GFXTimingInfo.u1TriggerState = (UINT8)E_TIMING_TRIGGER_STATE_CTRL_BIT;
    }
    else
    #endif
    {
        vDrvSetGFXControlBit();
        vDrvVByOneGMCrtlBitVF(SV_ON);
        vDrvApplyGFXTiming();
    }
}

void vDrvApplyGFXTiming(void)
{
   #ifndef CC_MTK_LOADER
	//CRIT_STATE_T csState;
   #endif

    LOG(2, "vDrvApplyGFXTiming: state = %d\n", GFXTimingInfo.u1TriggerState);
   
    switch (GFXTimingInfo.u1TriggerState)
    {
        case E_TIMING_TRIGGER_STATE_CTRL_BIT:
            vDrvVByOneGMCrtlBitVF(SV_OFF);
            vDrvSetGFXControlBit();
            GFXTimingInfo.u1TriggerState = (UINT8)E_TIMING_TRIGGER_STATE_TIMING;
            break;

        case E_TIMING_TRIGGER_STATE_VALID:
            vDrvVByOneGMCrtlBitVF(SV_ON);
            GFXTimingInfo.u1TriggerState = (UINT8)E_TIMING_TRIGGER_STATE_IDLE;
			#ifndef CC_MTK_LOADER
			#ifndef  CC_SUPPORT_UHD
				vScpipGfxSetData(SV_ON);
				vScpipRefSetData(ON);
			#endif			
			#endif
            break;

        case E_TIMING_TRIGGER_STATE_WAITING:
            break;

        case E_TIMING_TRIGGER_STATE_TIMING:
            if (GraphMode_GetPanelType() == PANEL_TYPE_4K2K_30)
            {
                vDrvVByOneGMCrtlBitVF(SV_ON);
                GFXTimingInfo.u1TriggerState = E_TIMING_TRIGGER_STATE_IDLE;
            }
            else
            {
                GFXTimingInfo.u1TriggerState = (UINT8)E_TIMING_TRIGGER_STATE_VALID;
            }

        case E_TIMING_TRIGGER_STATE_IDLE:
        default:
            OSD_TimingNfy(); // Notify OSD GFX Timing changed	
            
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_H_ACTIVE](GFXTimingInfo.u4HActive);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_H_TOTAL](GFXTimingInfo.u4HTotal);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_H_FP](GFXTimingInfo.u4HFP);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_H_BP](GFXTimingInfo.u4HBP);
            
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_V_ACTIVE](GFXTimingInfo.u4VActive);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_V_TOTAL](GFXTimingInfo.u4VTotal);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_V_FP](GFXTimingInfo.u4VFP);
            DiplayTimingCB[E_TIMEGEN_PATH_GFX][E_DISPLAY_TIMING_CB_V_BP](GFXTimingInfo.u4VBP);

            #ifdef CC_MTK_LOADER
            vScpipGfxSetData(SV_ON);
            #endif
            break;
    }
}

void vDrvSetGFXTimingTriggerState(E_TIMING_TRIGGER_STATE eTriggerState)
{
    GFXTimingInfo.u1TriggerState = (UINT8)eTriggerState;
}

void vDrvSetGFXHActive(UINT16 u2Active)
{
    vScpipSetPanelWidth(E_GFX_TG, u2Active);
}

void vDrvSetGFXHTotal(UINT16 u2Total)
{
    vScpipSetPanelHtotal(E_GFX_TG, u2Total);
}

void vDrvSetGFXHFP(UINT16 u2FP)
{
    vScpipSetHsyncFp(E_GFX_TG, u2FP);
}

void vDrvSetGFXHBP(UINT16 u2BP)
{
    vScpipSetHsyncBp(E_GFX_TG, u2BP);
}

void vDrvSetGFXVActive(UINT16 u2Active)
{
    vScpipSetPanelHeight(E_GFX_TG, u2Active);
}

void vDrvSetGFXVTotal(UINT16 u2Total)
{
    vScpipSetPanelVtotal(E_GFX_TG, u2Total);
}

void vDrvSetGFXVFP(UINT16 u2FP)
{
    vScpipSetVsyncFp(E_GFX_TG, u2FP);
}

void vDrvSetGFXVBP(UINT16 u2BP)
{
    vScpipSetVsyncBp(E_GFX_TG, u2BP);
}
#endif

#ifndef CC_MTK_LOADER
void vDrvVB1VideoMuteTiming(void)
{
	static UINT8 u1VideoMuteCnt = 0;
	UINT8 MuteTh = IO32ReadFldAlign(BETEST_00,VB1_STABLE_TH);
	MuteTh = MAX(MuteTh,4);
	
		if(fgVB1VideoMute)
		{
			u1VideoMuteCnt++;
		}
		
		if (u1VideoMuteCnt == MuteTh)
		{
			u1VideoMuteCnt = 0;
			fgVB1VideoMute = SV_FALSE;
			LOG(2, "UnMute now\n");
			LOG(2, "fgVB1VideoMute = %d\n", fgVB1VideoMute);
			DRVCUST_SendEvent(E_CUST_OUT_MUTE_CHANGE, 0);
}
	
}

#else
void vDrvVB1VideoMuteTiming(void)
{
    
}
#endif

void vDrvRegisterDisplayCB(void)
{
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_H_ACTIVE, wDrvSetOutputHActive);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_H_TOTAL, wDrvSetOutputHTotal);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_H_FP, vDrvSetHsyncFp);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_H_BP, vDrvSetHsyncBp);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_V_ACTIVE, wDrvSetOutputVActive);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_V_TOTAL, wDrvSetOutputVTotal);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_V_FP, vDrvSetVsyncFp);
    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_PQ, E_DISPLAY_TIMING_CB_V_BP, vDrvSetVsyncBp);

    #if defined(SUPPORT_PANEL_GRAPH_MODE)
	if (IS_SUPPORT_VB1_GRAPH_MODE)
	{
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_H_ACTIVE, vDrvSetGFXHActive);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_H_TOTAL, vDrvSetGFXHTotal);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_H_FP, vDrvSetGFXHFP);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_H_BP, vDrvSetGFXHBP);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_V_ACTIVE, vDrvSetGFXVActive);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_V_TOTAL, vDrvSetGFXVTotal);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_V_FP, vDrvSetGFXVFP);
	    fpDrvRegisterDisplayTimingUpdateCB(E_TIMEGEN_PATH_GFX, E_DISPLAY_TIMING_CB_V_BP, vDrvSetGFXVBP);
	}
    #endif
}

void vDrvSetPanelTiming(void)
{
    UINT32 u4HSyncWidth, u4VSyncWidth, u4HPosition;
    UINT32 u4HFrontPorch, u4VFrontPorch;
    UINT32 u4HBackPorch, u4VBackPorch;
    UINT32 u4HTotal, u4VTotal;

	if (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ)
	{
		bSI_DISPLAY_DCLK_TYPE = u1DrvCal48HzOutputType();
	}
	
    switch (bSI_DISPLAY_DCLK_TYPE)
    {
        case SV_DCLK_48HZ:
	        u4HPosition = H_POS_48HZ;
	        u4HTotal = HLEN_TOTAL_48HZ;
	        u4VTotal = VLEN_TOTAL_48HZ;
			break;

        case SV_DCLK_50HZ:
            u4HPosition = H_POS_50HZ;
            u4HTotal = HLEN_TOTAL_50HZ;
            u4VTotal = VLEN_TOTAL_50HZ;
            break;

        case SV_DCLK_OTHERS:
        case SV_DCLK_60HZ:
        default:
            u4HPosition = H_POS_60HZ;
            u4HTotal = HLEN_TOTAL_TYPI;
            u4VTotal = VLEN_TOTAL_TYPI;
            break;
    }

    #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
    u4HSyncWidth = PANEL_GetHSyncWidthByTiming(bSI_DISPLAY_DCLK_TYPE);
    u4VSyncWidth = PANEL_GetVSyncWidthByTiming(bSI_DISPLAY_DCLK_TYPE);
    u4VBackPorch = PANEL_GetVSyncBpByTiming(bSI_DISPLAY_DCLK_TYPE);
    u4HPosition = PANEL_GetHPositionByTiming(bSI_DISPLAY_DCLK_TYPE);
    #else
    u4HSyncWidth = PANEL_GetHSyncWidth();
    u4VSyncWidth = PANEL_GetVSyncWidth();
    u4VBackPorch = u4GetPanelVSyncBp();
    #endif

    u4HFrontPorch = (u4HPosition > 0x800) ? (u4HPosition - 0x800) : 0;
    u4HBackPorch = u4HTotal - wDISPLAY_WIDTH - u4HFrontPorch - u4HSyncWidth;

    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_EPI || PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_MLVDS)
    {
        u4VBackPorch = 7;
        u4VSyncWidth = 1;
    }

    u4VFrontPorch = u4VTotal - wDISPLAY_HEIGHT - u4VSyncWidth - u4VBackPorch;

    {
        #define ODFPLimit 4
        #define REORDERBPLimit 4
        u4HFrontPorch = (u4HFrontPorch < ODFPLimit) ? ODFPLimit : u4HFrontPorch;
        u4HBackPorch = (u4HBackPorch < REORDERBPLimit) ? REORDERBPLimit : u4HBackPorch;
    }

    if ((lcdtimingInfo.u4HActive != wDISPLAY_WIDTH) || (lcdtimingInfo.u4VActive != wDISPLAY_HEIGHT) ||
        (lcdtimingInfo.u4HTotal != u4HTotal) || (lcdtimingInfo.u4VTotal != u4VTotal) ||
        (lcdtimingInfo.u4HFP != u4HFrontPorch) || (lcdtimingInfo.u4HBP != u4HBackPorch) ||
        (lcdtimingInfo.u4HSyncWidth != u4HSyncWidth) || (lcdtimingInfo.u4VSyncWidth != u4VSyncWidth)||
        (lcdtimingInfo.u4VFP != u4VFrontPorch) || (lcdtimingInfo.u4VBP != u4VBackPorch))
    {
        #ifndef CC_MTK_LOADER
        CRIT_STATE_T csState;
        csState = x_crit_start();
        #endif
        lcdtimingInfo.u4HTotal = u4HTotal;
        lcdtimingInfo.u4HActive = wDISPLAY_WIDTH;
        lcdtimingInfo.u4HFP = u4HFrontPorch;
        lcdtimingInfo.u4HBP = u4HBackPorch;
		lcdtimingInfo.u4HSyncWidth = u4HSyncWidth;

        lcdtimingInfo.u4VTotal = u4VTotal;
        lcdtimingInfo.u4VActive = wDISPLAY_HEIGHT;
        lcdtimingInfo.u4VFP = u4VFrontPorch;
        lcdtimingInfo.u4VBP = u4VBackPorch;
		lcdtimingInfo.u4VSyncWidth = u4VSyncWidth;
        #ifndef CC_MTK_LOADER
        x_crit_end(csState);
        #endif
    }

    #ifdef CC_PHP_SUPPORT
#ifndef CC_MTK_LOADER
    DRVCUST_PQBAfterSocOutputChanged(lcdtimingInfo.u4VActive, lcdtimingInfo.u4HActive, bOUTPUT_REFRESH_RATE);
#endif
    #endif
#ifdef CC_SUPPORT_4K2K
    if ((IS_PANEL_L12R12 || IS_SUPPORT_4K2K_DISPLAY) && (!fgIsVsyncISRInit))
#else
	if (IS_PANEL_L12R12 && (!fgIsVsyncISRInit))
#endif
    {
        vDrvVsyncISRInit();
        fgIsVsyncISRInit = SV_TRUE;
    }

    // When MJC on/off update PSC Global to  FSC Global
    #ifndef CC_MTK_LOADER
    #if defined(CC_MT5890)
    if (!fgIsMJCToOSTG())
    {
       vDrvUpdateFSCTimeGenPrm();
    }
    #endif
    #endif

    #ifndef CC_MTK_LOADER
    if ((fgIsVsyncIsrStart) && (_fgVOPLLPowerOn))
    {
        #if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)   
        E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_CTRL_BIT;
        LOG(2, "VB1ControlBit = TRIGGER_STATE_CTRL_BIT\n");
        #endif
        fgApplyScalerSetData = 1;
        fgApplyScanPWMSetData = 1;
        u1ScanPwmCrossFlg = 2;
        LOG(2, "fgApplyScalerSetData = SV_TRUE\n");
    }
    else
    #endif
    {
		printf("[LVDS]vDrvApplyPanelTiming.....\n");
        vDrvApplyPanelTiming();
        vDrvFireImportPortection();
       // VDP_SetBypassUFOModeEnable(1,0);
    }

	#ifndef CC_MTK_LOADER    
	#ifdef CC_SUPPORT_UHD
		vDrvApplyUHDTiming(&lcdtimingInfo);
	#endif
    #endif
	
}

void vVdoOutISRCheckApplyPanelTiming(void)
{
    #if !defined(CC_MTK_LOADER) && defined(CC_SUPPORT_4KBE)
    switch (E_VB1ControlBit_Trig_Sta)
    {
        case E_TIMING_TRIGGER_STATE_CTRL_BIT:
            vDrvVByOneCrtlBitCBV(SV_OFF);
            vDrvSetVB1ControlBit();
            E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_TIMING;
            break;

        case E_TIMING_TRIGGER_STATE_VALID:
            E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_IDLE;
            break;

        case E_TIMING_TRIGGER_STATE_WAITING:
            break;

        case E_TIMING_TRIGGER_STATE_TIMING:
            if(GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60)
            {
              vDrvVByOneCrtlBitCBV(SV_ON);  
            }
            E_VB1ControlBit_Trig_Sta = E_TIMING_TRIGGER_STATE_IDLE;

        case E_TIMING_TRIGGER_STATE_IDLE:
        default:
            vVdoOutISRCheckSetDataTrigger();
            break;
    }
    #else
    #if !defined(CC_MTK_LOADER)
    vVdoOutISRCheckSetDataTrigger();
    #endif
    #endif
}

void vDrvApplyPanelTiming(void)
{
    E_TIMEGEN_PATH ePath;
    
    LOG(3, "vDrvApplyPanelTiming\n");
	fgVB1VideoMute = SV_TRUE;
	#ifndef CC_MTK_LOADER
	LOG(2, "Mute now\n");
	LOG(2, "fgVB1VideoMute = %d\n", fgVB1VideoMute);
	DRVCUST_SendEvent(E_CUST_OUT_MUTE_CHANGE, 0);
	#endif
  //  ePath = vDrvGetTconTopVdoSel() ? E_TIMEGEN_PATH_BYPASS : E_TIMEGEN_PATH_PQ;
    ePath = E_TIMEGEN_PATH_PQ;
    #if defined(CC_SUPPORT_4K2K) && defined(CC_MT5890)
    if( GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_60)		
    {
        if(bGetVideoDecType(SV_VP_MAIN) == SV_VD_DVI)
    	{
    	    ePath = E_TIMEGEN_PATH_BYPASS_HDMI;			
    	}
		else
		{
            ePath = E_TIMEGEN_PATH_BYPASS_MM;
		}
    }
	#endif
    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_H_ACTIVE](lcdtimingInfo.u4HActive);
    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_H_TOTAL](lcdtimingInfo.u4HTotal);
    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_H_FP](lcdtimingInfo.u4HFP);
    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_H_BP](lcdtimingInfo.u4HBP);

    #ifndef CC_MTK_LOADER
	vDrvMJCUpdateOutputTiming(lcdtimingInfo.u4HTotal,lcdtimingInfo.u4VTotal,lcdtimingInfo.u4HActive,lcdtimingInfo.u4VActive);
    #endif

    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_TOTAL](lcdtimingInfo.u4VTotal);
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
    {
        DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_ACTIVE](lcdtimingInfo.u4VActive + 2);
        DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_FP](lcdtimingInfo.u4VFP - 2);
    }
    else
    {
        DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_ACTIVE](lcdtimingInfo.u4VActive);
        DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_FP](lcdtimingInfo.u4VFP);
    }
    #ifndef CC_MTK_LOADER
    //mask for DTV00602998 show garbage
    //vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_ON, MJC_OUTPUT_RST);
    //vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, SV_OFF, MJC_OUTPUT_RST);
    #endif
    DiplayTimingCB[ePath][E_DISPLAY_TIMING_CB_V_BP](lcdtimingInfo.u4VBP);

    if (IS_VB1_2SEC_ENABLE && IS_IC_5861())
    {
        vDrvSet2SECVsyncWidth(lcdtimingInfo.u4VSyncWidth);
        vDrvSet2SECVTotal(lcdtimingInfo.u4VTotal);
    }
	

    // ISR Setting
    vDrvVsyncISRSetLineCounter(lcdtimingInfo.u4VFP);
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
    //#ifdef SUPPORT_PANEL_SCAN_PWM
	if (PANEL_IsUseScanPwm())
	{
    vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Set data
    LOG(3, "_|-|_ vDrvApplyPanelTiming Bright:%d%\n", _u4BrightLevel);
    BACKLT_SetDimming(BACKLT_GetDimmingFrequency(), _u4BrightLevel);
    #ifndef CC_MTK_LOADER
    DRVCUST_UpdateBacklightState(BL_STATE_OUTPUT_FR_CHANGE, _u4BrightLevel);
    #endif
	}
    //#endif
    #ifndef CC_MTK_LOADER
    if(IS_PANEL_L12R12)
    {
        Panel_3DGlassesCTL(e3DGlasses_P_3DBL,e3DGlasses_S_BL,e3DGlasses_POl_Bypass,e3DGlasses_POS_1stR,0,w3DGlassesDelay);
        Panel_3DGlassesDutyCTL(e3DGlasses_S_BL,50);
    }
	DRVCUST_SendEvent(E_CUST_OUT_FMT_CHANGE, 0);
	DRVCUST_SendEvent(E_CUST_OUT_POSI_CHANGE, 0);
	DRVCUST_SendEvent(E_CUST_SRC_AREA_CHANGE, 0);
    #endif


}

#if defined(CC_MT5890)
#ifdef CC_SUPPORT_4K2K
/*
//reference timing generator initial steps:
//1.	Disable FSC/PSC/GFX time-gen
//2.	Set HTOT/VTOT for reference time-gen
//3.	Set VSYNC gen and N/M mask settings
//4.	Set FSC/PSC/GFX time-gen as slave mode <-- setting initial state 
//5.	Enable FSC/PSC/GFX time-gen
*/
static UINT8 bSI_DISPLAY_DCLK_TYPE_bak=0xff;
void vDrvSetRefTiming( void )
{
    UINT32 u4HSyncWidth,u4VSyncWidth;
    UINT32 u4HPosition;

    //todo:
    //check if not 4k model ( ref tg will not enable) then return;
    //? get the panel control for chk ??


    if ( bSI_DISPLAY_DCLK_TYPE_bak == bSI_DISPLAY_DCLK_TYPE )
        return;    //if output frame has no change, just keep the ref_tg setting.
    
    bSI_DISPLAY_DCLK_TYPE_bak=bSI_DISPLAY_DCLK_TYPE;

    LOG(0,"-vDrvSetRefTiming----- %d\n",bSI_DISPLAY_DCLK_TYPE_bak);
    
    u4HSyncWidth = PANEL_GetHSyncWidth();
    u4VSyncWidth = PANEL_GetVSyncWidth();

    u4HPosition=PANEL_GetHPositionByTiming(bSI_DISPLAY_DCLK_TYPE);



    //Printf("HSyncWidth60Hz=%d, ", PANEL_GetHSyncWidthByTiming(SV_DCLK_60HZ));
    //Printf("HSyncWidth50Hz=%d, ", PANEL_GetHSyncWidthByTiming(SV_DCLK_50HZ));
    //Printf("VSyncWidth60Hz=%d, ", PANEL_GetVSyncWidthByTiming(SV_DCLK_60HZ));
    //Printf("VSyncWidth50Hz=%d\n", PANEL_GetVSyncWidthByTiming(SV_DCLK_50HZ));
    
    ReftimingInfo.u4HTotal  = rREF_4K_TG[bSI_DISPLAY_DCLK_TYPE].u2HTotal;
    ReftimingInfo.u4VTotal  = rREF_4K_TG[bSI_DISPLAY_DCLK_TYPE].u2VTotal;
    ReftimingInfo.u4HActive = rREF_4K_TG[bSI_DISPLAY_DCLK_TYPE].u2Width;
    ReftimingInfo.u4VActive = rREF_4K_TG[bSI_DISPLAY_DCLK_TYPE].u2Height;
    ReftimingInfo.u4HFP     =  (u4HPosition > 0x800) ? (u4HPosition - 0x800) : 0;

#ifdef CC_SUPPORT_UHD
	ReftimingInfo.u4HFP=ReftimingInfo.u4HFP>>1;
	u4HSyncWidth=u4HSyncWidth>>1;
#endif
    //Printf("hhh ref ReftimingInfo.u4HTotal=%d\n", ReftimingInfo.u4HTotal);
    //Printf("hhh ref u4HSyncWidth=%d\n", u4HSyncWidth);
    //Printf("hhh ref ReftimingInfo.u4HActive=%d\n", ReftimingInfo.u4HActive);
    //Printf("hhh ref ReftimingInfo.u4HFP=%d\n", ReftimingInfo.u4HFP);

	
    ReftimingInfo.u4HBP     = ReftimingInfo.u4HTotal - u4HSyncWidth - ReftimingInfo.u4HActive - ReftimingInfo.u4HFP;
    ReftimingInfo.u4VBP     = rREF_4K_TG[bSI_DISPLAY_DCLK_TYPE].u4VsyncBp;
    ReftimingInfo.u4VFP     = ReftimingInfo.u4VTotal - u4VSyncWidth - ReftimingInfo.u4VBP - ReftimingInfo.u4VActive;
    //todo...
    //set the VSYNC gen and N/M mask settings
    //N = rREF_4K2K30[bSI_DISPLAY_DCLK_TYPE].u2FrameRate;  // ref frame rate 
    //M = b2r, r2r, FSC, GFX framerate

    vDrvSetRefTiming_SETDATA();
    //set flag first and set import with GFX TG at the same time!!
    ReftimingInfo.u1SetData = 1;
}
#endif
#endif




void vDrvSetVB1ControlBit_Valid(BOOL fgValid)
{
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
    
    }
}

#if defined(CC_MT5890)

void vDrvSetRefTiming_SETDATA(void)
{
    vScpipSetPanelHtotal( E_REF_TG , ReftimingInfo.u4HTotal );
    vScpipSetPanelVtotal( E_REF_TG , ReftimingInfo.u4VTotal );
    vScpipSetPanelWidth ( E_REF_TG , ReftimingInfo.u4HActive );
    vScpipSetPanelHeight( E_REF_TG , ReftimingInfo.u4VActive );
   
    vScpipSetHsyncFp( E_REF_TG , ReftimingInfo.u4HFP );
    vScpipSetHsyncBp( E_REF_TG , ReftimingInfo.u4HBP );
    vScpipSetVsyncFp( E_REF_TG , ReftimingInfo.u4VFP );
    vScpipSetVsyncBp( E_REF_TG , ReftimingInfo.u4VBP );

	vScpipRefSetTgImportOnOff(ON);
    //vScpipRefSetData(ON);
}
#endif

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
#ifndef CC_MTK_LOADER
        if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else
        if(0)
#endif
        {
            // Front Dither
            vIO32WriteFldAlign(OS_DITHER_02, 1, DRMOD_R);          // 12 to 10 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)
            // Post Dither
            vIO32WriteFldAlign(OS_DITHER_05, 1, REG_DRMOD_R_POST);     // 12 to 10 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_03, 2, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_03, 2, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)
        }
        else
        {
            // Front Dither
            vIO32WriteFldAlign(OS_DITHER_02, 1, DRMOD_R);          // 12 to 10 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)

            // Post Dither (Off When OD Disabled)
            vIO32WriteFldAlign(OS_DITHER_05, 0, REG_DRMOD_R_POST);     // 12 to 10 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_03, 0, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_03, 2, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)
        }
    }
    else if (DISP_BIT == DISP_24BIT)
    {
#ifndef CC_MTK_LOADER
        if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else
        if(0)
#endif
        {
            // Front Dither (12 to 10)
            vIO32WriteFldAlign(OS_DITHER_02, 1, DRMOD_R);          // 12 to  8 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)

            // Post Dither (10 to 8)
            vIO32WriteFldAlign(OS_DITHER_05, 2, REG_DRMOD_R_POST);     // 12 to  8 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_03, 1, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)

        }
        else
        {
            // Front Dither (12 to 8)
            vIO32WriteFldAlign(OS_DITHER_02, 2, DRMOD_R);          // 12 to  8 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_00, 1, REG_TABLE_EN);       // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_00, 1, REG_FPHASE_CTRL);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)
            // Post Dither (Off When OD Disabled)
            vIO32WriteFldAlign(OS_DITHER_05, 0, REG_DRMOD_R_POST);     // 12 to  8 Dither Mode
            vIO32WriteFldAlign(OS_DITHER_03, 0, REG_TABLE_EN_POST);  // active bit of magic table for running order.
                                                                    //  01: phase active in bit3 ~ bit0 (12 to 8)
                                                                    //  10: phase active in bit1 ~ bit0 (12 to 10)
                                                                    //  else phase active in bit5 ~ bit0
            vIO32WriteFldAlign(OS_DITHER_03, 1, REG_FPHASE_CTRL_POST);    // phase range select.
                                                                    //  00 : 0~63. (12 to 6)
                                                                    //  01 : 0~16. (12 to 8)
                                                                    //  10 : 0~3.  (12 to 10)


        }
    }
    else if (DISP_BIT == DISP_18BIT)
	{
#ifndef CC_MTK_LOADER				 
		if (_ODParam.u1ODCurrentONOFF == SV_TRUE)
#else		 
		if(0)
#endif
		{
			// Front Dither (12 to 10)
			vIO32WriteFldAlign(OS_DITHER_02, 1, DRMOD_R);		   // 12 to  10 Dither Mode
			vIO32WriteFldAlign(OS_DITHER_00, 2, REG_TABLE_EN);	   // active bit of magic table for running order.
																   //  01: phase active in bit3 ~ bit0 (12 to 8)
																   //  10: phase active in bit1 ~ bit0 (12 to 10)
																   //  else phase active in bit5 ~ bit0 																	   
			vIO32WriteFldAlign(OS_DITHER_00, 2, REG_FPHASE_CTRL);  // phase range select.
																   //  00 : 0~63. (12 to 6)
																   //  01 : 0~16. (12 to 8)
																   //  10 : 0~3.  (12 to 10)
			vIO32WriteFldAlign(OS_DITHER_00, 4, DITHER_FPHASE);    // Running order dithering frame phase increment.
																   // 1. If ROUND_EN = 1, the output will only rounding only 
																   // (no matter what RDITHER_EN and EDITHER_EN setting)
																   // 2. RDITHER_EN and EDITHER_EN can work together.	
	
			// Post Dither (10 to 6)
			vIO32WriteFldAlign(OS_DITHER_05, 3, REG_DRMOD_R_POST);	   // 12 to  6 Dither Mode
			vIO32WriteFldAlign(OS_DITHER_03, 0, REG_TABLE_EN_POST);    // active bit of magic table for running order.
																	   //  01: phase active in bit3 ~ bit0 (12 to 8)
																	   //  10: phase active in bit1 ~ bit0 (12 to 10)
																	   //  else phase active in bit5 ~ bit0 																	   
			vIO32WriteFldAlign(OS_DITHER_03, 0, REG_FPHASE_CTRL_POST); // phase range select.
																	   //  00 : 0~63. (12 to 6)
																	   //  01 : 0~16. (12 to 8)
																	   //  10 : 0~3.  (12 to 10) 
			vIO32WriteFldAlign(OS_DITHER_03, 3, REG_FPHASE_POST);	   // Running order dithering frame phase increment.
																	   // 1. If ROUND_EN = 1, the output will only rounding only 
																	   // (no matter what RDITHER_EN and EDITHER_EN setting)
																	   // 2. RDITHER_EN and EDITHER_EN can work together.  
		}
		else
		{
			// Front Dither (12 to 6) 
			vIO32WriteFldAlign(OS_DITHER_02, 3, DRMOD_R);		   // 12 to  8 Dither Mode
			vIO32WriteFldAlign(OS_DITHER_00, 0, REG_TABLE_EN);	   // active bit of magic table for running order.
																   //  01: phase active in bit3 ~ bit0 (12 to 8)
																   //  10: phase active in bit1 ~ bit0 (12 to 10)
																   //  else phase active in bit5 ~ bit0 																	   
			vIO32WriteFldAlign(OS_DITHER_00, 0, REG_FPHASE_CTRL);  // phase range select.
																   //  00 : 0~63. (12 to 6)
																   //  01 : 0~16. (12 to 8)
																   //  10 : 0~3.  (12 to 10) 
			vIO32WriteFldAlign(OS_DITHER_00, 4, DITHER_FPHASE);    // Running order dithering frame phase increment.
																   // 1. If ROUND_EN = 1, the output will only rounding only 
																   // (no matter what RDITHER_EN and EDITHER_EN setting)
																   // 2. RDITHER_EN and EDITHER_EN can work together.
			// Post Dither (Off When OD Disabled)
			vIO32WriteFldAlign(OS_DITHER_05, 0, REG_DRMOD_R_POST);	   // 12 to  8 Dither Mode
			vIO32WriteFldAlign(OS_DITHER_03, 0, REG_TABLE_EN_POST);    // active bit of magic table for running order.
																	   //  01: phase active in bit3 ~ bit0 (12 to 8)
																	   //  10: phase active in bit1 ~ bit0 (12 to 10)
																	   //  else phase active in bit5 ~ bit0 																	   
			vIO32WriteFldAlign(OS_DITHER_03, 0, REG_FPHASE_CTRL_POST); // phase range select.
																	   //  00 : 0~63. (12 to 6)
																	   //  01 : 0~16. (12 to 8)
																	   //  10 : 0~3.  (12 to 10) 
			vIO32WriteFldAlign(OS_DITHER_03, 3, REG_FPHASE_POST);	   // Running order dithering frame phase increment.
																	   // 1. If ROUND_EN = 1, the output will only rounding only 
																	   // (no matter what RDITHER_EN and EDITHER_EN setting)
																	   // 2. RDITHER_EN and EDITHER_EN can work together.  
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

    vIO32WriteFldAlign(OS_DITHER_05, u4LsbOff, REG_LSB_OFF_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4Round, REG_ROUND_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4RDither, REG_RDITHER_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4EDither, REG_EDITHER_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4LFSR, REG_LFSR_EN_POST);
    vIO32WriteFldAlign(OS_DITHER_03, u4Subpxl, REG_SUBPIX_EN_POST);

    // 2.2 Common Dither Setting
    vIO32WriteFldAlign(OS_DITHER_03, 0, REG_SUB_R_POST);
    vIO32WriteFldAlign(OS_DITHER_03, 1, REG_SUB_G_POST);
    vIO32WriteFldAlign(OS_DITHER_03, 2, REG_SUB_B_POST);

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
        else
        u1SetDISP_REncodeEnHW_L4(0);
#endif

        if(IS_SUPPORT_OSD_INDEX)
        vSetOSD_IndexEncodeEnable(1);
        else
        vSetOSD_IndexEncodeEnable(0);


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
            break;
			
        case PANEL_DISPLAY_TYPE_MLVDS:
            vIO32WriteFldAlign(TMGR5, 4, TMGR5_RG_FIFO_CTRL);
            vIO32WriteFldAlign(MLVDS_CTRL_09, 1, RG_FIFO_SEL);
            break; 
       #if defined(CC_MT5890)
            case PANEL_DISPLAY_TYPE_VB1:
                vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);
                vIO32WriteFldAlign(TMGR2, 2, TMGR2_RG_FIFO_CK_SEL);
                vIO32WriteFldAlign(MLVDS_CTRL_09, 2, RG_FIFO_SEL);
                break;
        #endif
            case PANEL_DISPLAY_TYPE_EPI:
                vIO32WriteFldAlign(TMGR5, 2, TMGR5_RG_FIFO_CTRL);
                vIO32WriteFldAlign(TMGR2, 4, TMGR2_RG_FIFO_CK_SEL);
                vIO32WriteFldAlign(MLVDS_CTRL_09, 4, RG_FIFO_SEL);
                break;
    }
    vIO32WriteFldAlign(TMGR5, 1, TMGR5_RG_FIFO_EN);

#ifndef CC_MTK_LOADER
#ifdef CC_SCPOS_3DTV_SUPPORT
    vSavePanelAttribute();
    fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_OSTG, (vTDTVModeChangeCB) vDrvDisp3DModeCB);
#endif

#ifdef CC_SUPPORT_4K2K
    vSavePanelAttribute_4K2K();
    if( IS_IC_5861())
       vScpipSetSlaveMode(E_ALL_TG, OFF);  //gazelle no need slaved mode
	else
	{
       vSaveRefAttribute_4K2K();
       vScpipSetSlaveMode(E_ALL_TG, ON);    //oryx 
     }
#else
    vScpipSetSlaveMode(E_ALL_TG, OFF);
#endif

#if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)
    vDrvVB1ControlBitInit();
#endif

#if defined(SUPPORT_PANEL_GRAPH_MODE)
    if (IS_SUPPORT_VB1_GRAPH_MODE)
    {
		vDrvGFXInit();
    }
#endif
#endif

    vIO32WriteFldAlign(OSTG_01, 0, REG_MAIN_TOP_EN); // set sub plane mixer order above main

#if defined(CC_MT5890)
    #ifdef CC_BYPASS_MJC
	vIO32WriteFldAlign(OSTG_01, 0, REG_MJC_IN_EN);//Selet OSTG input data path from MJC
	#else

	LOG(0,"[4K2KDD]GetCurrentPanelIndex()=%d\n",GetCurrentPanelIndex());
	LOG(0,"[4K2KDD]PANEL_GetPixelClkMax()=%d\n",PANEL_GetPixelClkMax());	
	LOG(2," 4kdd mjc chk ---- \n");
	
    if (PANEL_IsSupport4K2KDD() || (TV_HEIGHT >= 2160)) //support 4k2k  or 2k2k output , 
    {
        vIO32WriteFldAlign(OSTG_01, 0, REG_MJC_IN_EN);//Selet OSTG input data path from MJC	   	
    }	
	else
	{
        vIO32WriteFldAlign(OSTG_01, 1, REG_MJC_IN_EN);//Selet OSTG input data path from MJC
        LOG(2," 4kdd mjc on ---- \n");
	}
    #endif
#endif
}


void vDrvSetFixedFrameRate(UINT8 u1FrameRate)
{
    _bForceFrameRate = u1FrameRate;
}

/**

 * @brief vDrvDisplayInit

 * Init the Display Settings

 * @param  void
 * @retval void
 */
void vDrvDisplayInit(void)
{
	UINT32 u4Mask = 0;
	//UINT32 u4Ext3DLRPin, u4Ext3DLRPinFunc;

#if defined(CC_MT5890)||defined(CC_MT5861)
    #ifdef CC_BYPASS_MJC
		vIO32WriteFldAlign(OSTG_01, 0, REG_MJC_IN_EN);
	#else
		//Selet OSTG input data path from MJC
		vIO32WriteFldAlign(OSTG_01, 1, REG_MJC_IN_EN);
		//Set OSD Timing from Post Scaler
	#endif
#endif

#if defined(SCPIP_SUPPORT_POST_SCALER)
    vPscPreInit();
#endif
    vDrvRegisterDisplayCB();

	if(IS_ENGINEER_DISPLAY_MODE)
	{
		if (PANEL_IsSupport240Hz())
        {
            vDrvSetFixedFrameRate(240);
        }
        else if (PANEL_IsSupport120Hz())
        {
            vDrvSetFixedFrameRate(120);
        }
        else
        {
            vDrvSetFixedFrameRate(60);
        }		
	}


	vDrvOutputStageInit();


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
		#ifdef CC_SUPPORT_4K2K
		if (IS_DISP_4K2K_TYPE3)  // for Sony NO_SIGNAL: the video path frame rate must be 60Hz, as the same as the GFX path
		{
			if (PANEL_IsSupport240Hz())
	        {
	            vDrvCalPanelTiming(SV_DCLK_60HZ, 240);
	        }
	        else if (PANEL_IsSupport120Hz())
	        {
	            vDrvCalPanelTiming(SV_DCLK_60HZ, 120);
	        }
	        else
	        {
	            vDrvCalPanelTiming(SV_DCLK_60HZ, 60);
	        }
		}
		else
		#endif
		{
			if(IS_ENGINEER_DISPLAY_MODE)
			{
		        if (PANEL_IsSupport240Hz())
		        {
		            vDrvCalPanelTiming(SV_DCLK_60HZ, 240);
		        }
		        else if (PANEL_IsSupport120Hz())
		        {
		            vDrvCalPanelTiming(SV_DCLK_60HZ, 120);
		        }
		        else
		        {
		            vDrvCalPanelTiming(SV_DCLK_60HZ, 60);
		        }			
			}
			else
			{
		        if (PANEL_IsSupport240Hz())
		        {
		            vDrvCalPanelTiming(SV_DCLK_50HZ, 200);
		        }
		        else if (PANEL_IsSupport120Hz())
		        {
		            vDrvCalPanelTiming(SV_DCLK_50HZ, 100);
		        }
		        else
		        {
		            vDrvCalPanelTiming(SV_DCLK_50HZ, 50);
		        }
			}
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
            vDrvCalPanelTiming(SV_DCLK_60HZ, 240);
        }
        else if (PANEL_IsSupport120Hz())
            {
                vDrvCalPanelTiming(SV_DCLK_60HZ, 120);
            }
            else
            {
                vDrvCalPanelTiming(SV_DCLK_60HZ, 60);
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
            #if defined(CC_MT5890)
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
#if !defined(CC_MTK_LOADER) 
#ifdef SUPPORT_DISP_50TO120
	if(SUPPORT_DISP_50TO120 & SUPPORT_50TO60_2D)
	{	   
	   u4Mask |= SUPPORT_50TO60_2D;
	}
	if(SUPPORT_DISP_50TO120 & SUPPORT_50TO60_3D)
	{
	   u4Mask |= SUPPORT_50TO60_3D;
	}
#endif
#ifdef SUPPORT_DISP_60TO100
	if(SUPPORT_DISP_60TO100 & SUPPORT_60TO50_2D)
	{
	   u4Mask |= SUPPORT_60TO50_2D;
	}
	if(SUPPORT_DISP_60TO100 & SUPPORT_60TO50_3D)
	{
	   u4Mask |= SUPPORT_60TO50_3D;
	}
#endif

#ifdef SUPPORT_DISP_FILM22_48TO120
	if(SUPPORT_DISP_FILM22_48TO120 & SUPPORT_FILM22_48TO120_2D)
	{
	   u4Mask |= SUPPORT_FILM22_48TO120_2D;
	}
#endif  

#ifdef FORCE_3D_60HZ_OUTPUT_ENABLE
	u4Mask |= (SUPPORT_50TO60_3D|SUPPORT_3D_OUTPUT_60);
#endif
#endif

    if (PANEL_IsUseScanPwm())
    {
		_u4BrightLevel = 50;   // 50 means 50%; 0%:dark --> 100%:bright
    }
    else
    {
		_u4BrightLevel = 0x80; // 0:dark --> 255:bright
    }

    //vDrvOutputStageInit();
    //vDrvRegisterDisplayCB();
    vDrvInitSCANPWM(DRVCUST_PanelGet(ePanelBacklightPwmPort));
	fgBLCustomCtrl = 1;

    //SET main/pip mute works immediately
    vIO32WriteFldAlign(MUTE_00, 1 ,R_MUTE_POST_SEL);
    vIO32WriteFldAlign(MUTE_00, 1 ,R_P_MUTE_FRONT_SEL);
    vIO32WriteFldAlign(MUTE_00, 1 ,R_M_MUTE_FRONT_SEL);
    // disable video plane
    //vIO32WriteFldAlign(SCPIP_PIP1_02, 0, PIP1_02_RVL_1);
    //vIO32WriteFldAlign(SCPIP_PIP2_02, 0, PIP2_02_RVL_2);

#ifndef CC_MTK_LOADER
    #ifdef CC_SUPPORT_4K2K
    //if ((GetCurrentPanelIndex() == PANEL_CMI_4K2K_FHD120))
    {
        HANDLE_T  hThread;
        if(!_hFrcUnmuteSema)
        {
            VERIFY(x_sema_create(&_hFrcUnmuteSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
            VERIFY(x_sema_create(&_hFrcSetPanelSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
            VERIFY(x_thread_create(&hThread, "FRC Unmute", 4096, 60, vFrcUnmute, 0, NULL) == OSR_OK);
            Printf("----[4k2k][I2C] @vDrvDisplayInit 4k2k  ----\n");
            VERIFY(x_timer_create(&h4k2kUnMuteTimer) == OSR_OK);
            vExt4k2kFrcInit();
        }
	#if 0
		if ((DRVCUST_OptQuery(e3DLRPin,&u4Ext3DLRPin)==0)&&(DRVCUST_OptQuery(e3DLRPinFunc,&u4Ext3DLRPinFunc)==0))
		{
	        BSP_PinSet(u4Ext3DLRPin, u4Ext3DLRPinFunc);
		}
	#endif
        if(IS_DISPR_INVERSE)
        Panel_3DGlassesCTL(e3DGlasses_P_3DL,e3DGlasses_S_LEYE,e3DGlasses_POl_Bypass,e3DGlasses_POS_1stR,0,0);
        else
        Panel_3DGlassesCTL(e3DGlasses_P_3DL,e3DGlasses_S_DISPR,e3DGlasses_POl_Reverse,e3DGlasses_POS_1stR,0,0);
        Panel_DISPR_FIELD_Delay(eDispRFieldAlign_S_DispR,eDispRFieldAlign_M_Bypass,0);
        Panel_3DGlassesCTLEn(e3DGlasses_P_3DL,1);

    }

    #endif
    u4DrvSetCustFRC(u4Mask);
#endif

#if 0 //defined(CC_MT5890)
	if(IS_SUPPORT_VB1_GRAPH_MODE != 0)
	{
  		vIO32WriteFldAlign(SCPIP_SCSYS_14, 0x2, SCSYS_14_SC_OSD_SRC_SEL);
	}
	else
	{
  		vIO32WriteFldAlign(SCPIP_SCSYS_14, fgIsMJCToOSTG()? 0x1 : 0x0, SCSYS_14_SC_OSD_SRC_SEL);
	}
#endif
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
        vDrvCalPanelTiming(SV_DCLK_50HZ, 50);
    }
    else if (wFrameRate==60)
    {
        vDrvCalPanelTiming(SV_DCLK_60HZ, 60);
    }
    else if (wFrameRate==100)
    {
        vDrvCalPanelTiming(SV_DCLK_50HZ, 100);
    }
    else if (wFrameRate==120)
    {
        vDrvCalPanelTiming(SV_DCLK_60HZ, 120);
    }
    else if (SUPPORT_PANEL_48HZ && (wFrameRate == 48))
    {
        vDrvCalPanelTiming(SV_DCLK_48HZ, wFrameRate);
    }
    else
    {
        vDrvCalPanelTiming(SV_DCLK_OTHERS, (UINT8)wFrameRate);
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
    #if defined(CC_MT5890)
        UINT32 u4PCLK_Sel;
		UINT8 tveEnable;
	    TVE_GetEnable(TVE_1, &tveEnable);    

        if (fgIsMJCToOSTG())
        {
            u4PCLK_Sel = 2; // poclk_dpll
        }
        else
        {
            u4PCLK_Sel = 0; // oclk
        }
        vIO32WriteFldAlign(CKGEN_DISP_CKCFG, u4PCLK_Sel, FLD_POCLK_SEL);
		if(tveEnable==SV_FALSE)
	    {
	        //set OCLK_TVE (for sub path)
	        vScpipSetOclkTveSelReg(u4PCLK_Sel);
	    }
    #endif

    return SV_TRUE;
}
#endif // CC_MTK_LOADER

//get internal frame rate between Front scaler and MJC
UINT16 u2DrvGetInternalFrameRate(void)
{
    if (fgIsMJCToOSTG())
    {
        return _u2IntFrameRate;
    }
    else
    {
        return (UINT16)bOUTPUT_REFRESH_RATE;
    }
}

//set internal frame rate between Front scaler and MJC
void vDrvSetInternalFrameRate(UINT16 frameRate)
{
    #if defined(CC_MT5890)
    _u2IntFrameRate = frameRate;
    #else
    //no-nothing
    #endif
}

EXTERN BOOL fgIs4k2kNativeTimingOut(UINT8 bPath);


void vDrvCalPanelTiming(UINT8 u1DCLKType, UINT32 u4FrameRate)
{
    UINT32 u4PanelClock;
    UINT32 u4PanelRatio;
    //#ifndef SUPPORT_PANEL_SCAN_PWM
    static BOOL fgInit = FALSE;
    //#endif

    LOG(2, "vDrvCalPanelTiming: (DCLKType, FrameRate) = (%d, %d) -> (%d, %d)\n",
        bSI_DISPLAY_DCLK_TYPE, bOUTPUT_REFRESH_RATE,
        u1DCLKType, u4FrameRate);

    if ((bSI_DISPLAY_DCLK_TYPE == u1DCLKType)
        #ifndef CC_MTK_LOADER
        && (bOUTPUT_REFRESH_RATE == u4FrameRate)
        #endif
        #ifdef CC_SUPPORT_4K2K
        && (fg4k2kPNLTypeChg == SV_FALSE)
        #endif
		#if !defined(CC_MTK_LOADER) && defined(CC_SUPPORT_UHD)
		&& (fgIs4k2kNativeTimingOut(SV_VP_MAIN)==SV_FALSE)
		#endif
        )
    {
        #ifndef CC_MTK_LOADER
        bDrvUpdateInternalFrameRate();
        vDrvSetSSMDRAsInputHsync();
        #endif
		return;
    }

	u4PanelRatio = u4DrvCalPanelRatio();
    bOUTPUT_REFRESH_RATE = u4FrameRate;

	if (u1DCLKType == SV_DCLK_48HZ)
	{
		u1DCLKType = u1DrvCal48HzOutputType();
	}

    switch (u1DCLKType)
    {
        case SV_DCLK_48HZ:
            u4PanelClock = PANEL_GetPixelClk48Hz();
            bSI_DISPLAY_DCLK_TYPE = SV_DCLK_48HZ;
            break;
		case SV_DCLK_50HZ:
            u4PanelClock = PANEL_GetPixelClk50Hz();
            bSI_DISPLAY_DCLK_TYPE = SV_DCLK_50HZ;
            break;
        case SV_DCLK_60HZ:
            u4PanelClock = PANEL_GetPixelClk60Hz();
            bSI_DISPLAY_DCLK_TYPE = SV_DCLK_60HZ;
            break;
        case SV_DCLK_OTHERS:
        default:
            if (u4FrameRate < (50 << u4PanelRatio))
            {
                u4PanelClock = (PANEL_GetPixelClk50Hz() / (50 << u4PanelRatio)) * bOUTPUT_REFRESH_RATE;
                bSI_DISPLAY_DCLK_TYPE = SV_DCLK_50HZ;
            }
            else
            {
                u4PanelClock = (PANEL_GetPixelClk60Hz() / (60 << u4PanelRatio)) * bOUTPUT_REFRESH_RATE;
                bSI_DISPLAY_DCLK_TYPE = SV_DCLK_60HZ;
            }
            break;
    }

    #if !defined(CC_MTK_LOADER) && defined(CC_DRV_SUPPORT_SMICRO)
    {
        SMICRO_VERTICAL_FREQ eFreq;
        eFreq = (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ)? FREQ_48HZ :
                (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_50HZ)? FREQ_50HZ : FREQ_60HZ;
        vSMicroPreSetFreq(eFreq);
    }
    #endif

    //#ifndef SUPPORT_PANEL_SCAN_PWM
    if ((!PANEL_IsUseScanPwm())&&fgInit)
    {
        BeforeOutputFrameRateChange(bOUTPUT_REFRESH_RATE);
    }
    //#endif

    vDrvSwitchMTKGoodDclk(SV_OFF);
    vDrvSetPanelTiming();
    //#ifdef CC_SUPPORT_UHD
	//	vDrvSetRefTiming();
    #if defined(CC_MT5890) && defined(SUPPORT_PANEL_GRAPH_MODE) 
	if (IS_SUPPORT_VB1_GRAPH_MODE)
	{
	#if defined(CC_SUPPORT_4K2K) && defined(CC_SUPPORT_4KBE)
		vDrvSetRefTiming();
	    //sony backend IC can not support Video output FHD120 && GFX output 4k2k
	    //when video output FHD@120 Force GFX output 4k1k@60
	    if(u2DrvGetInternalFrameRate() == 96 || u2DrvGetInternalFrameRate() == 100 || u2DrvGetInternalFrameRate() == 120)
	    {
	        u4PreGFXType = GraphMode_GetPanelType();
	        vDrvSwitchGFXTiming(PANEL_TYPE_4K1K_60);//force gfx output 4k1k
	    } 
		else
	#endif
		{
	#ifdef   CC_SUPPORT_UHD
			vDrvSetRefTiming();
	#endif
	    	vDrvSetGFXTiming(bSI_DISPLAY_DCLK_TYPE);
		}
	}
    #endif 
	
    
    #if defined(CC_SCPOS_3DTV_SUPPORT) && !defined(CC_MTK_LOADER)
    if (u4DrvGetDisp3DModeDclk() != 0)
    {
        u4PanelClock = u4DrvGetDisp3DModeDclk();
        bOUTPUT_REFRESH_RATE = u1DrvGetDisp3DModeRefreshRate();
    }
    #endif

    vDrvVOFreqSet(u4PanelClock);
    
    #ifndef CC_MTK_LOADER
    vDrvSetSSMDRAsInputHsync(); // for dynamic MDR
    #endif

    #if defined(SUPPORT_FREERUN_DCLK_BY_STEP) && !defined(CC_MTK_LOADER)
    // store the original control word
    i4OrigControlWord = IO32ReadFldAlign(DDDS_00, DDDS_00_DDDS_FREQ_CW);
    #endif

    if (fgDDDSEn == SV_ON && _eSyncLockMode != eSYNCLOCK_OFF)
    {
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

    //#ifndef SUPPORT_PANEL_SCAN_PWM
    if ((!PANEL_IsUseScanPwm())&&fgInit)
    {
        AfterOutputFrameRateChange();
    }
   //#endif

    #if !defined(CC_MTK_LOADER) && defined(CC_DRV_SUPPORT_SMICRO)
    {
        SMICRO_VERTICAL_FREQ eFreq;
        SMICRO_MSG rMsg;
        if (SUPPORT_PANEL_48HZ && (bOUTPUT_REFRESH_RATE == (UINT8)(48 * u4Ratio)))
        {
            eFreq = FREQ_48HZ;
        }
        else if (bOUTPUT_REFRESH_RATE < (UINT8)(55 * u4Ratio))
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

    //#ifndef SUPPORT_PANEL_SCAN_PWM
    if ((!PANEL_IsUseScanPwm())&&(!fgInit))
    {
        fgInit = TRUE;
    }
    //#endif
}


UINT8 vDrvGetLCDFreq(void)
{
    return bOUTPUT_REFRESH_RATE;
}

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

void Panel_SetDisplayShift(BOOL fgEnable, UINT8 u1ShiftMode,UINT8 u1L1DisplayShift, UINT8 u1R2DisplayShift)
{
  vIO32WriteFldAlign(VINTF_P17, fgEnable, VINTF_3D_DISP_SHIFT_EN);
  vIO32WriteFldAlign(VINTF_P17, u1ShiftMode, VINTF_3D_DISP_SHIFT_FRAME);
  vIO32WriteFldAlign(VINTF_P17, 1, VINTF_3D_DISP_SHIFT_CTL);
  vIO32WriteFldAlign(VINTF_P17, u1L1DisplayShift, VINTF_3D_DISP_SHIFT_VALUE_L);
  vIO32WriteFldAlign(VINTF_P17, u1R2DisplayShift, VINTF_3D_DISP_SHIFT_VALUE_R);
}

void Panel_SetDisplayShiftColor(UINT8 u1ShiftColorB,UINT8 u1ShiftColorG, UINT8 u1ShiftColorR)
{
  vIO32WriteFldAlign(VINTF_P18, u1ShiftColorB, VINTF_3D_DISP_SHIFT_B);
  vIO32WriteFldAlign(VINTF_P18, u1ShiftColorG, VINTF_3D_DISP_SHIFT_G);
  vIO32WriteFldAlign(VINTF_P18, u1ShiftColorR, VINTF_3D_DISP_SHIFT_R);
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
	UINT32 u4Ext3DLRPin, u4Ext3DLRPinFunc;

#ifndef CC_MTK_LOADER
	if ((DRVCUST_OptQuery(e3DLRPin,&u4Ext3DLRPin)==0)&&(DRVCUST_OptQuery(e3DLRPinFunc,&u4Ext3DLRPinFunc)==0))
	{
		LOG(3, "[3DGlassesCTL] 0x%x set to FUNCTION%d\n", u4Ext3DLRPin, u4Ext3DLRPinFunc);
        UNUSED(BSP_PinSet(u4Ext3DLRPin, u4Ext3DLRPinFunc));
	}
	else
#endif
	{
	#if !defined(CC_MAPLE_CUST_DRV)
	#if defined(CC_MT5890) || defined( CC_MT5861)
		if (u1Pin == e3DGlasses_P_3DR)
		{
			u4Ext3DLRPin = PIN_GPIO52;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION1;
			LOG(3, "[3DGlassesCTL] 3DR:0x%x set to FUNCTION%d\n", PIN_GPIO52, PINMUX_FUNCTION1);
		}
		else if (u1Pin == e3DGlasses_P_3DL)
		{
			u4Ext3DLRPin = PIN_GPIO53;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION1;
			LOG(3, "[3DGlassesCTL] 3DL:0x%x set to FUNCTION%d\n", PIN_GPIO53, PINMUX_FUNCTION1);
		}
		else if (u1Pin == e3DGlasses_P_3DBL)
		{
			u4Ext3DLRPin = PIN_GPIO54;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION1;
			LOG(3, "[3DGlassesCTL] 3DBL:0x%x set to FUNCTION%d\n", PIN_GPIO54, PINMUX_FUNCTION1);
		}
	// #elif defined(CC_MT5880)
	#else
		if (u1Pin == e3DGlasses_P_3DR)
		{
			u4Ext3DLRPin = PIN_GPIO6;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION2;
			LOG(3, "[3DGlassesCTL] 3DR:0x%x set to FUNCTION%d\n", PIN_GPIO6, PINMUX_FUNCTION2);
		}
		else if (u1Pin == e3DGlasses_P_3DL)
		{
			u4Ext3DLRPin = PIN_GPIO7;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION2;
			LOG(3, "[3DGlassesCTL] 3DL:0x%x set to FUNCTION%d\n", PIN_GPIO7, PINMUX_FUNCTION2);
		}
		else if (u1Pin == e3DGlasses_P_3DBL)
		{
			u4Ext3DLRPin = PIN_GPIO8;
			u4Ext3DLRPinFunc = PINMUX_FUNCTION2;
			LOG(3, "[3DGlassesCTL] 3DBL:0x%x set to FUNCTION%d\n", PIN_GPIO8, PINMUX_FUNCTION2);
		}
	#endif
	
		UNUSED(BSP_PinSet(u4Ext3DLRPin, u4Ext3DLRPinFunc));
	#endif
	}
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

void vDrvDisplaySetPentouchMode(UINT32 fgOnOff)
{
    if (fgPentouchMode != fgOnOff)
    {
        fgPentouchMode = (UINT8)fgOnOff;
        vSetMainFlg(MAIN_FLG_MODE_CHG);
        vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
    }
}

UINT32 fgDrvDisplayGetPentouchMode(void)
{
    return fgPentouchMode;
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

    #ifdef SUPPORT_DDDS_STEP_TRACKING
    vDrvDDDSTrackingNormProc();
    #endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        static INT32 i4DeltaControlWord = 0;
        if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1) && (IS_STEP_CLOCK_CHG))
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
                   if((_u4ErrorLimit&0xff) < TargetErrlimit)
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


// OSTG Event on Output VSync
// Note: Keep this function simple since this function is called by ISR
void OSTG_OnOutputVSync(void)
{

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
#ifdef LVDS_ESD_PROTECT
	if (_fgESDProtect)
	{
		if (fgIsLVDSFIFOSkew())
		{
			vLVDSFIFOSkew_Recover();
		}
	}
#endif
#endif 

#ifndef CC_MTK_LOADER
		if (_fgVB1RegisterReMapping && IS_IC_5890_ES3())
		{
			VByOne_Register_Mapping_Write_E3();
		}
#endif 

}

void vDrvVB1ES3_DEBUG(UINT8 bOn_Off)
{
	_fgVB1RegisterReMapping = bOn_Off;
}

void vDrvSwitchImportProtection(UINT8 bOn_Off)
{
}

void vDrvFireImportPortection(void)
{
    if (fgIsMJCToOSTG())
    {
        vPscSetData(1);
        LOG(3,"PSC Set data\n");
    }
    else
    {
		#if !defined(CC_MTK_LOADER) && defined(CC_SUPPORT_UHD)
		    fgUhdSetScPipdata=ON;
	    #else
	        vScpipSetData(1);
		#endif
        LOG(3,"FSC Set data\n");
    }
}

void vDrvSetErrorLimit(UINT32 u4Error)
{
#ifdef SUPPORT_PANEL_ERRORLIMIT_TABLE
 	UINT16 u2ErrLim=0;
	UINT16 u2ErrLimLow=0;
#endif

    if (u4Error != _u4ErrorLimit)
    {
        _u4ErrorLimit = u4Error;
	#ifdef SUPPORT_PANEL_ERRORLIMIT_TABLE
		u2ErrLim = (UINT16)(_u4ErrorLimit & 0xff);
		u2ErrLimLow = (UINT16)(_u4ErrorLimit >> 16);
		vIO32WriteFldAlign(DDDS_03, u2ErrLim, DDDS_03_DDDS_ERR_LIM);
		vIO32WriteFldAlign(DDDS_1C, u2ErrLimLow, DDDS_1C_NOR_DDDS_LIM_LOW);
	#else
        vIO32WriteFldAlign(DDDS_03, _u4ErrorLimit, DDDS_03_DDDS_ERR_LIM);
	#endif
    }
}

#ifdef CC_ENABLE_MTK_MODEL_INDEX
UINT8 u1UseSuperFlipMirror=0;
UINT8 u1SuperFlipMirrorCfg=0;
UINT8 u1SetFlipMirrorConfig(BOOL fgMirrorEn, BOOL fgFlipEn);

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

    UNUSED(u1SetFlipMirrorConfig(u1SuperMirror, u1SuperFlip));
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
    static UINT8 u1FlipMirrorEnable= 0 ;
#ifdef CC_FLIP_MIRROR_FROM_EEPROM
    DTVCFG_T rDtvCfg;
#endif

    if((u1Init == 0) || (u1FlipMirrorReset == 1))
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
		else
		{
			u1FlipMirrorEnable &= ~(SYS_FLIP_CONFIG_ON);
		}
		
            if(rDtvCfg.u1Flags2 & DTVCFG_FLAG2_MIRROR_ON)
            {
                u1FlipMirrorEnable |= SYS_MIRROR_CONFIG_ON;
            }
		else
		{
			u1FlipMirrorEnable &= ~(SYS_MIRROR_CONFIG_ON);
		}
	     Printf("====u1FlipMirrorEnable %d\n", u1FlipMirrorEnable);
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
	u1FlipMirrorReset = 0;
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

void vReinitFlipMirrorConfig(void)
{
    u1Init=0;
}


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

void vDrvSetAllMute(UINT8 bOnOff)
{
    vRegWriteFldAlign(MUTE_00, bOnOff,  R_MUTE_POST_EN);
}

UINT32 u4DrvCalPanelRatio(void)
{
    if (PANEL_IsSupport240Hz())
    {
        return 2;
    }
    else if (PANEL_IsSupport120Hz())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

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
        vFrameTrackDDDSConfig(0, 0);
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
//  Printf("slowfast(%d), vtotal(%d)\n",u1SlowFast,u4TargetVTotal);

    InputVTotal = IO32ReadFldAlign(DDDS_02, DDDS_02_HLEN_DEN);
    HLEN_NUM = IO32ReadFldAlign(DDDS_01, DDDS_01_HLEN_NUM);
    HLEN_INT = IO32ReadFldAlign(DDDS_01, DDDS_01_HLEN_INT);
    DISPLAY_SIZE = (HLEN_INT * InputVTotal) + HLEN_NUM;

    u4HTotal = (UINT32)wDrvGetOutputHTotal();
    u4VTotal = (UINT32)wDrvGetOutputVTotal();

    if (_fgFrTrkHalfStep)
    {
        _uDDDSDivision = 1;
    }
    else
    {
        _uDDDSDivision = 0;
    }
    if(u4TargetVTotal > u4VTotal)
        DISPLAY_SIZE = DISPLAY_SIZE +(((u4TargetVTotal - u4VTotal)*u4HTotal)>>_uDDDSDivision);
    else
        DISPLAY_SIZE = DISPLAY_SIZE -(((u4VTotal - u4TargetVTotal)*u4HTotal)>>_uDDDSDivision);

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

    static UINT8 u1PrevMode = 0xFF;

    if (u1PrevMode == u1Mode)
    {
        return;
    }

    u1PrevMode = u1Mode;

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

UINT32 fgDrvIsSupport48(void)
{
	if(SUPPORT_PANEL_48HZ == 0)
	{
		return FALSE;
	}
    
    if (IS_PANEL_2D_N_3D_L12R12)
    {
        return fgDrvSupport24Hz() ? TRUE : FALSE;
    }
    #ifdef CC_SUPPORT_4K2K
    else if (_u14K2KMode == E_4K2K_NATIVE)
    {
        return TRUE;
    }
    #endif
    else
    {
    #if defined(SUPPORT_CUSTOMER_FRAME_RATE_SPEC) && defined(CC_MT5890)
		return TRUE;
    #elif defined(SUPPORT_48HZ_NOT_96HZ)  // for LG spec.
        return (u1DrvVideoGetIs44PullDownInfo() && PANEL_IsSupport120Hz() == FALSE) ? TRUE : FALSE;
	#else
		if (SUPPORT_PANEL_48HZ)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
    #endif
    }
}

UINT32 fgDrvIsSupport60To50(void)
{
    #ifdef SUPPORT_60TO50
    if (u1DrvTDTVGetCur3DMode() == E_TDTV_UI_3D_MODE_OFF)
    {
        return u1CustFRC & SUPPORT_60TO50_2D;
    }
    else
    {
        return u1CustFRC & SUPPORT_60TO50_3D;
    }
    #else
    return FALSE;
    #endif
}

UINT32 fgDrvIsSupport50To60(void)
{
    #ifdef SUPPORT_50TO60
    if (u1DrvTDTVGetCur3DMode() == E_TDTV_UI_3D_MODE_OFF)
    {
        return u1CustFRC & SUPPORT_50TO60_2D;
    }
    else
    {
        return u1CustFRC & SUPPORT_50TO60_3D;
    }
    #else
    return FALSE;
    #endif
}
UINT32 fgDrvIsSupport3D60Output(void)
{
    #ifdef FORCE_3D_60HZ_OUTPUT_ENABLE
    if (u1DrvTDTVGetCur3DMode() != E_TDTV_UI_3D_MODE_OFF)
    {
        return u1CustFRC & SUPPORT_3D_OUTPUT_60;
    }
    else
    {
        return FALSE;
    }
    #else
    return FALSE;
    #endif
}

void VDP_SetBrowserInput(UINT8 uEnable)
{
    if(uEnable)
	{
	    fgBrowserModeEn = SV_TRUE;
	}
	else
	{
	    fgBrowserModeEn = SV_FALSE;
	}
		
}

void VDP_SetSpeIPTVS(UINT8 uEnable)
{
    if(uEnable)
	{
	    fgSpeIPTVSEn = SV_TRUE;
	}
	else
	{
	    fgSpeIPTVSEn = SV_FALSE;
	}
		
}


BOOL fgIsBrowseInput(void)
{
   return fgBrowserModeEn;
}

BOOL fgIsSpeIPTVS(void)
{
    return fgSpeIPTVSEn;
}
UINT32 fgIsSupportFixMode(void)
{
   #if defined(SUPPORT_CUSTOMER_FRAME_RATE_SPEC) && defined(CC_MT5890)
    UINT32 u4VdoFrameRate;
    u4VdoFrameRate = bDrvVideoGetRefreshRate(SV_VP_MAIN);
    u4VdoFrameRate >>= (u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_FS) ? 1 : 0;
    if(fgIsBrowseInput() || fgIsSpeIPTVS())
	{   
    #ifndef CC_SUPPORT_4K2K
		return (PANEL_IsSupport120Hz() && ((u4VdoFrameRate == 8) || (u4VdoFrameRate == 24) || (u4VdoFrameRate == 48))) ? FALSE : TRUE;
    #else
		return TRUE;
    #endif  
	}
	else
	{
        return FALSE;
	}
	#else
	return FALSE;
	#endif
}
UINT32 fgIsSupportFlickerLessMode(void)
{
    #if defined(SUPPORT_CUSTOMER_FRAME_RATE_SPEC) && defined(CC_MT5890)
	    UINT32 u4VdoFrameRate;
	    u4VdoFrameRate = bDrvVideoGetRefreshRate(SV_VP_MAIN);
	    u4VdoFrameRate >>= (u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_FS) ? 1 : 0;
		
	    if((u4DrvTDTV3DModeQuery() != E_TDTV_UI_3D_MODE_OFF)
			&&(fgIsMJCToOSTG())
		#ifndef CC_SUPPORT_4K2K//fhd120hz model or fhd60hz model
			&& (MJC_GetEffectLevel() != 0) && (PANEL_IsSupport120Hz())//only for 120hz model
		#endif
		)
		{
		    #ifndef CC_SUPPORT_4K2K
			return ((u4VdoFrameRate == 8) || (u4VdoFrameRate == 24) || (u4VdoFrameRate == 48)) ? FALSE : TRUE;//for fhd120hz model, 48hz input have no flickless mode
			#else
			return TRUE;
			#endif
		}
		else
		{
	        return FALSE;
		}
	#else
	return FALSE;
	#endif
}

/*
* @SUPPORT_CUSTOMER_FRAME_RATE_SPEC
* @VDP_PHOTO_2D:         1920*1080 60hz
* @VDP_PHOTO_3D_2_2D: 1920*1080 60hz
* @VDP_PHOTO_2D_2_3D: 1920*1080 60hz
* @VDP_PHOTO_3D:         1920*540 120hz
*/
extern TDTV_3D_CROP_INTO_T eCropInfo;
UINT32 u4VDPGetPhotoType(void)
{
    UINT32  u4PhotoType = VDP_PHOTO_NA;
    //#ifdef SUPPORT_CUSTOMER_FRAME_RATE_SPEC
    if((SV_TRUE==eCropInfo.fgIsFixSrcAspectRatio)
     &&(bGetSignalType(SV_VP_MAIN)==SV_ST_MPEG))
    {
    if(E_TDTV_UI_3D_MODE_OFF!=u4DrvTDTV3DModeQuery())
    {
        if(TD_TTT_OUT(u4DrvTDTVTTTModeQuery(SV_VP_MAIN))==E_TD_OUT_3D_2_2D)
        {
            u4PhotoType = VDP_PHOTO_3D_2_2D;
        }
        else if(E_TDTV_UI_3D_MODE_TTD==u4DrvTDTV3DModeQuery())
        {
            u4PhotoType = VDP_PHOTO_2D_2_3D;
        }
        else
        {
            u4PhotoType = VDP_PHOTO_3D;
        }
    }
    else
    {
        u4PhotoType = VDP_PHOTO_2D;
    }
    }
    else
    {
        u4PhotoType = VDP_PHOTO_NA;
    }
    //#endif
    return u4PhotoType;
}
/*
* For non-4K ext FRC panel(PANEL_FHD_BACKEND)
* Get panel type according to different input frame rate/format
*/
UINT32 vDrvCal2KPanelType(void)
{
    UINT32 u4OutType = PANEL_TYPE_2K1K_60;
	UINT32 u4PhotoType = VDP_PHOTO_NA;

	if (IS_DISP_2K_TYPE1)  // 720p120/FHD60/1920x540p
	{
		u4PhotoType = u4VDPGetPhotoType();

		if(VDP_PHOTO_3D == u4PhotoType)
	    {
	        u4OutType = PANEL_TYPE_1920_540P;
	        return u4OutType;
	    }
	    else if((VDP_PHOTO_3D_2_2D == u4PhotoType) ||
	            (VDP_PHOTO_2D == u4PhotoType) || 
	            (VDP_PHOTO_2D_2_3D == u4PhotoType))
	    {
	        u4OutType = PANEL_TYPE_2K1K_60;
	        return u4OutType;
	    }
	    else
	    {
	    }
	    
	    if((E_TDTV_UI_3D_MODE_OFF == u4DrvTDTV3DModeQuery())||
	        (E_TDTV_UI_3D_MODE_TTD == u4DrvTDTV3DModeQuery()))
	    {
	        if((bDrvVideoGetRefreshRate(SV_VP_MAIN) == 100) ||
	            (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 120))
	        {
	            u4OutType = PANEL_TYPE_720P_120;
	        }
	        else
	        {
	            u4OutType = PANEL_TYPE_2K1K_60;
	        }
	    }
	    else
	    {
	        u4OutType = PANEL_TYPE_1920_540P;
	    }
	}
	
    return u4OutType;
}

UINT32 u4DrvSetCustFRC(UINT32 u4Mask)
{
    if (u1CustFRC != u4Mask)
    {
        u1CustFRC = u4Mask;
        vDrvCalPanelFrameRate(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
        vVrmOnVdoModeChangeDone(SV_VP_MAIN);
    }
    return u1CustFRC;
}

UINT32 u4DrvGetCustFRC(void)
{
    return u1CustFRC;
}
void  vDrvCheck3DOuputFrameRate(void)
{
	static UINT8 u1PreIs3DFmt=0 ;
	UINT32 u4Type = PANEL_TYPE_2K1K_60;
	
	if(fgDrvIsSupport50To60() ||fgDrvIsSupport60To50())
	{
		//just for UI control 3d mode after signal stable case 
		if(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
		{
			vDrvCalPanelFrameRate(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
	        //vVrmOnVdoModeChangeDone(SV_VP_MAIN);
		}		
	}

	if (u1PreIs3DFmt !=u1DrvTDTVCurIs3DMode())
	{
		u1PreIs3DFmt = u1DrvTDTVCurIs3DMode();
		DRVCUST_SendEvent(E_CUST_TV_OUT_FMT_CHANGE, 0);
		if(u1PreIs3DFmt  == 0) //2D
		{
		#ifdef CC_SUPPORT_4K2K
			if(IS_SUPPORT_4K2K_DISPLAY)
			{
			    u4Type = vDrvCal4K2KPanelType();
				Printf("[4K3D]-->2D panel_type:%d\n", u4Type);
				if(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
				{
					vDrvCalPanelFrameRate(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
					vVrmOnVdoModeChangeDone(SV_VP_MAIN);
				}		
			}
			else
		#endif
			{
			#ifdef PANEL_FHD_BACKEND
				if (IS_DISP_2K_TYPE1)
				{
			        if((bDrvVideoGetRefreshRate(SV_VP_MAIN) == 100) ||
			            (bDrvVideoGetRefreshRate(SV_VP_MAIN) == 120))
			        {
			            u4Type = PANEL_TYPE_720P_120;
			        }
			        else
			        {
			            u4Type = PANEL_TYPE_2K1K_60;
			        }
					Printf("[FHD_BE]-->2D panel_type:%d\n", u4Type);
					vDrvSwitchVideoType(u4Type);
				}
			#endif
			}
		}
		
		else //3D
		{
		#ifdef CC_SUPPORT_4K2K
			if(IS_SUPPORT_4K2K_DISPLAY)
			{
				u4Type = PANEL_TYPE_2K1K_120;
				Printf("[4K3D]-->3D panel_type:%d\n", u4Type);
				if(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
				{
					vDrvCalPanelFrameRate(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
					vVrmOnVdoModeChangeDone(SV_VP_MAIN);
				}		
			}
			else
		#endif
			{
			#ifdef PANEL_FHD_BACKEND
				if (IS_DISP_2K_TYPE1)
				{
					u4Type = PANEL_TYPE_1920_540P;
					Printf("[FHD_BE]-->3D panel_type:%d\n", u4Type);
					vDrvSwitchVideoType(u4Type);
				}
			#endif
			}	
		}
	}	
}

void vDrvCalPanelFrameRate(UINT16 u2VdoVTotal, UINT32 u4VdoFrameRate)
{
    UINT32 u4DCLKType;
    UINT32 u4PanelFrameRate;
    UINT32 u4PanelRatio;

    #ifndef CC_MTK_LOADER
	UINT32 u4Type;
	LOG(2,"vDrvCalPanelFrameRate u2VdoVTotal = %d u4VdoFrameRate = %d\n",u2VdoVTotal,u4VdoFrameRate);

	if(u2VdoVTotal == 0 || u4VdoFrameRate == 0)
	{
		return;
	}
	
    #ifdef CC_SUPPORT_4K2K
	if(IS_SUPPORT_4K2K_DISPLAY)
	{
		u4Type = vDrvCal4K2KPanelType();
	    vDrvSwitchVideoType(u4Type);
	}
	else
	#endif
	{
	#ifdef PANEL_FHD_BACKEND
		if (IS_SUPPORT_2K_BE)// FHD_BACKEND
		{
			u4Type = vDrvCal2KPanelType();
			vDrvSwitchVideoType(u4Type);
		}
	#endif
	}
    #endif

    u4PanelRatio = u4DrvCalPanelRatio();
    u4VdoFrameRate >>= (u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_FS) ? 1 : 0;
    if (_bForceFrameRate != 0)
    {
        u4VdoFrameRate = _bForceFrameRate;
    }

    switch (u4VdoFrameRate)
    {
        case 25:
        case 50:
        case 100:
            u4DCLKType = (fgDrvIsSupport50To60() || fgIsSupportFixMode() || fgIsSupportFlickerLessMode()) ? SV_DCLK_60HZ : SV_DCLK_50HZ;
            break;
        case 30:
        case 60:
        case 120:
            u4DCLKType = fgDrvIsSupport60To50() ? SV_DCLK_50HZ : SV_DCLK_60HZ;
            break;
        case 24:
			u4DCLKType = ( fgDrvIsSupport48() && !(fgIsSupportFixMode() || fgIsSupportFlickerLessMode()) && !fgDrvIsSupport3D60Output() ) ? SV_DCLK_48HZ : SV_DCLK_60HZ;
            break;
        case 48:
            if (SUPPORT_PANEL_48HZ)
            {
	            if(fgIsSupportFixMode() || fgIsSupportFlickerLessMode())
				{
				    u4DCLKType = SV_DCLK_60HZ;
				}
				else
				{
				    u4DCLKType = SV_DCLK_48HZ;
	    	    }
            }
            else if (IS_DISP_48HZ_TO_50HZ)
            {
				u4DCLKType = SV_DCLK_50HZ;
            }
			else
			{
				u4DCLKType = SV_DCLK_60HZ;
			}
				
			if(fgDrvIsSupport3D60Output())
			{
				u4DCLKType = SV_DCLK_60HZ;
			}
            break;
        case 20:
            if (fgIsMJCToOSTG())
            {
                #ifndef SUPPORT_20TO60
                u4DCLKType = SV_DCLK_50HZ;
                break;
                #endif
            }
        case 12:
        case 15:
            u4DCLKType = SV_DCLK_60HZ;
            break;
#ifdef SUPPORT_CUSTOMER_FRAME_RATE_SPEC
		case 8:
		u4DCLKType = (fgDrvIsSupport48() && !(fgIsSupportFixMode() || fgIsSupportFlickerLessMode())) ? SV_DCLK_48HZ : SV_DCLK_60HZ;
		break;
#endif
        default:
        #ifdef SUPPORT_CUSTOMER_FRAME_RATE_SPEC
			u4DCLKType = SV_DCLK_60HZ;
		#else
            if ((u4VdoFrameRate < 25) ||
                (u4VdoFrameRate > 30 && u4VdoFrameRate < 50) ||
                (u4VdoFrameRate > 60 && u4VdoFrameRate < 100))
            {
                u4DCLKType = SV_DCLK_50HZ;
            }
            else
            {
                u4DCLKType = SV_DCLK_60HZ;
            }
		#endif
            break;
    }
#ifdef SUPPORT_CUSTOMER_FRAME_RATE_SPEC
    if(bIsScalerInput444(SV_VP_MAIN)) //444 case
	{
	    u4DCLKType = SV_DCLK_60HZ;
	}
#endif

    u4PanelFrameRate = (u4DCLKType == SV_DCLK_48HZ)? 48 :
                       (u4DCLKType == SV_DCLK_50HZ)? 50 : 60;
    u4PanelFrameRate <<= u4PanelRatio;
#ifdef CC_SUPPORT_4K2K
	if(GetPANEL4K2K_TYPE() == PANEL_TYPE_4K2K_30)
	{
	    u4PanelFrameRate = u4VdoFrameRate;
	}
#endif

    if  (u4VdoFrameRate == 24 || u4VdoFrameRate == 25 || u4VdoFrameRate == 30)
    {
        #if !defined(CC_MT5890)
		#ifdef CC_SUPPORT_4K2K
        if (_u14K2KMode == E_4K2K_NATIVE)
        {
            u4PanelFrameRate = u4VdoFrameRate;
        }
	    #endif
        #endif
        if (DRVCUST_PanelGet(ePanel242530pOutput) == 1)
        {
            u4DCLKType = SV_DCLK_OTHERS;
            u4PanelFrameRate = u4VdoFrameRate;
        }
    }

    if (_bForceFrameRate != 0)
    {
        u4PanelFrameRate = _bForceFrameRate;

        // Customer requests freerun at picture wizard mode to prevent VSYNC flicker for PDP
        fgDDDSEn = (IS_PANEL_2D_N_3D_L12R12|IS_ENGINEER_DISPLAY_MODE) ? SV_OFF : SV_ON;
    }
    else if (fgDrvDisplayGetPentouchMode())
    {
        u4PanelFrameRate = 60;
        fgDDDSEn = SV_OFF;
    }
    else
    {
        fgDDDSEn = SV_ON;
    }

    vDrvCalPanelTiming(u4DCLKType, u4PanelFrameRate);
    vDrvSetMTKGoodDclk(u2VdoVTotal, u4VdoFrameRate);
    vFrameTrackDDDSConfig(0, 0);
}

void vDddsSettingCheck(void)
{

#if CC_OSD_MUTE_WA
    //channel change will reset out region
    if (_bDddsDelayFlag && (!(_fgAutoSearch || SRM_IsEpgMode())) && SRM_IsMainVideoStrictStable())
    {
        vDrvCalPanelFrameRate(wDrvVideoGetVTotal(SV_VP_MAIN), bDrvVideoGetRefreshRate(SV_VP_MAIN));
        // reset non-standard signal status
        _u4NSDHTotal=0xff;
        _u4NSDVTotal=0xff;
        _u4NSDFrameRate=0xff;
        _bDddsDelayFlag = 0 ;
        #ifdef DRV_SUPPORT_EXTMJC
        vDrvExtmjcRedoTwoChipConnection( SV_VP_MAIN, SV_TRUE);
        #endif
        vDrvMJCSetModeChangeTrig();
    }
#endif

    if ((_fgFRS_ENA == SV_ON) && fgIsMainTvd3d()
        && (_eSyncLockMode == eSYNCLOCK_IGNORED))   // Switch MTK Good Clock
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
    else if(SUPPORT_PANEL_48HZ && (u2FrameRate == 48 || u2FrameRate == 96))
    {
        PANEL_SetHTotal48Hz(u2HTotal);
        PANEL_SetVTotal48Hz(u2VTotal);
    }
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

static void updatePanelAttribute_PDP(UINT16 u2HTotal,
                                 UINT16 u2VTotal,
                                 UINT32 u2PixelClk,
                                 UINT16 u2Width,
                                 UINT16 u2Height,
                                 UINT16 u2VSyncWidth,
                                 UINT16 u2VSyncFP,
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
        PANEL_SetPixelClk50Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_50HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_50HZ, u2VSyncFP);
        #endif
    }
    else if(SUPPORT_PANEL_48HZ && (u2FrameRate == 48 || u2FrameRate == 96))
    {
        PANEL_SetHTotal48Hz(u2HTotal);
        PANEL_SetVTotal48Hz(u2VTotal);
        PANEL_SetPixelClk48Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_48HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_48HZ, u2VSyncFP);
        #endif
    }
    else
    {
        PANEL_SetHTotal60Hz(u2HTotal);
        PANEL_SetVTotal60Hz(u2VTotal);
        PANEL_SetPixelClk60Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_60HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_60HZ, u2VSyncFP);
        #endif
    }

    return;
}

static UINT16 au2OrigHTotal[3];
static UINT16 au2OrigVTotal[3];
static UINT16 u2OrigWidth, u2OrigHeight;
static UINT32 au4OrigPixelMinMax[2];
static UINT8 u1OrigVClkMax;

#if 0  // need to review later
typedef struct{
    UINT16 u2FrameRate;
    UINT16 u2Width;
    UINT16 u2Height;
    UINT16 u2HTotal;
    UINT16 u2VTotal;
    UINT32 u4PixelClk;
    UINT16 u4VsyncWidth;
    UINT16 u4VsyncBp;
}S_PANEL_CONFIG;

typedef struct{
    UINT32 u4PixleClockMax;
    UINT32 u4PixleClockMin;
    UINT16 u2VTotalMax;
    UINT16 u2VTotalMin;
    UINT16 u2VClkMax;   ;
}S_PANEL_CONFIG_COMMON;
#endif

S_PANEL_CONFIG rPANEL_2D[3];
S_PANEL_CONFIG rPANEL_3D[3];

S_PANEL_CONFIG_COMMON rPANEL_2D_COMMON;
S_PANEL_CONFIG_COMMON rPANEL_3D_COMMON;
static UINT32 u43DModeDclk = 0;
static UINT8 u13DModeRefreshRate = 0;
#define DBG_3D_PANEL_ATTRIBUTE 0
void vPanelAttribute_Status(void)
{
    #if DBG_3D_PANEL_ATTRIBUTE
    UINT8 u1Cnt=0;

	if(SUPPORT_PANEL_48HZ)
	{
		u1Cnt=SV_DCLK_48HZ;
	}
	else
	{
		u1Cnt=SV_DCLK_50HZ;
	}

	for( ;u1Cnt<SV_DCLK_OTHERS;u1Cnt++)
    {
    Printf("2D_u2FrameRate[%d] = %d\n",u1Cnt, rPANEL_2D[u1Cnt].u2FrameRate);
    Printf("2D_u2Width[%d] = %d\n",u1Cnt, rPANEL_2D[u1Cnt].u2Width);
    Printf("2D_u2Height[%d] = %d\n", u1Cnt,rPANEL_2D[u1Cnt].u2Height );
    Printf("2D_u2HTotal[%d] = %d\n", u1Cnt,rPANEL_2D[u1Cnt].u2HTotal);
    Printf("2D_u2VTotal[%d] = %d\n", u1Cnt,rPANEL_2D[u1Cnt].u2VTotal);
    Printf("2D_u4PixelClk[%d] = %d\n", u1Cnt,rPANEL_2D[u1Cnt].u4PixelClk);
    Printf("2D_u4VsyncWidth[%d] = %d\n", u1Cnt, rPANEL_2D[u1Cnt].u4VsyncWidth);
    Printf("2D_u4VsyncBp[%d] = %d\n", u1Cnt, rPANEL_2D[u1Cnt].u4VsyncBp);

    Printf("3D_u2FrameRate[%d] = %d\n",u1Cnt, rPANEL_3D[u1Cnt].u2FrameRate);
    Printf("3D_u2Width[%d] = %d\n",u1Cnt, rPANEL_3D[u1Cnt].u2Width);
    Printf("3D_u2Height[%d] = %d\n", u1Cnt,rPANEL_3D[u1Cnt].u2Height );
    Printf("3D_u2HTotal[%d] = %d\n", u1Cnt,rPANEL_3D[u1Cnt].u2HTotal);
    Printf("3D_u2VTotal[%d] = %d\n", u1Cnt,rPANEL_3D[u1Cnt].u2VTotal);
    Printf("3D_u4PixelClk[%d] = %d\n", u1Cnt,rPANEL_3D[u1Cnt].u4PixelClk);
    Printf("3D_u4VsyncWidth[%d] = %d\n", u1Cnt, rPANEL_3D[u1Cnt].u4VsyncWidth);
    Printf("3D_u4VsyncBp[%d] = %d\n", u1Cnt, rPANEL_3D[u1Cnt].u4VsyncBp);
    }

    Printf("rPANEL_2D_COMMON.u4PixleClockMax = %d \n ", rPANEL_2D_COMMON.u4PixleClockMax);
    Printf("rPANEL_2D_COMMON.u4PixleClockMin = %d \n ", rPANEL_2D_COMMON.u4PixleClockMin);
    Printf("rPANEL_2D_COMMON.u2VTotalMax = %d \n ", rPANEL_2D_COMMON.u2VTotalMax);
    Printf("rPANEL_2D_COMMON.u2VTotalMin = %d \n ", rPANEL_2D_COMMON.u2VTotalMin);
    Printf("rPANEL_2D_COMMON.u2VClkMax = %d \n ", rPANEL_2D_COMMON.u2VClkMax);

    Printf("rPANEL_3D_COMMON.u4PixleClockMax = %d \n ", rPANEL_3D_COMMON.u4PixleClockMax);
    Printf("rPANEL_3D_COMMON.u4PixleClockMin = %d \n ", rPANEL_3D_COMMON.u4PixleClockMin);
    Printf("rPANEL_3D_COMMON.u2VTotalMax = %d \n ", rPANEL_3D_COMMON.u2VTotalMax);
    Printf("rPANEL_3D_COMMON.u2VTotalMin = %d \n ", rPANEL_3D_COMMON.u2VTotalMin);
    Printf("rPANEL_3D_COMMON.u2VClkMax = %d \n ", rPANEL_3D_COMMON.u2VClkMax);
    #endif

}

void vSavePanelAttribute_PDP(void)
{
        UINT8 u1Cnt=0;

        if(!IS_PANEL_2D_N_3D_L12R12)
        {
            return;
        }

        if(LVDS_OUTPUT_PORT == DUAL_PORT)
            fgPDPL12R12 = ePanel_PDP_L12R12; // L12R12
        else
            fgPDPL12R12 = ePanel_PDP_L1R1; // L1R1

        rPANEL_2D[SV_DCLK_60HZ].u2FrameRate = 60;
        rPANEL_2D[SV_DCLK_60HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_2D[SV_DCLK_60HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_2D[SV_DCLK_60HZ].u2HTotal = PANEL_GetHTotal60Hz();
        rPANEL_2D[SV_DCLK_60HZ].u2VTotal = PANEL_GetVTotal60Hz();
        rPANEL_2D[SV_DCLK_60HZ].u4PixelClk = PANEL_GetPixelClk60Hz();
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        rPANEL_2D[SV_DCLK_60HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_60HZ);
        rPANEL_2D[SV_DCLK_60HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_60HZ);
        #else
        rPANEL_2D[SV_DCLK_60HZ].u4VsyncWidth = PANEL_GetVSyncWidth();
        rPANEL_2D[SV_DCLK_60HZ].u4VsyncBp = u4GetPanelVSyncBp();
        #endif

        rPANEL_2D[SV_DCLK_50HZ].u2FrameRate = 50;
        rPANEL_2D[SV_DCLK_50HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_2D[SV_DCLK_50HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_2D[SV_DCLK_50HZ].u2HTotal = PANEL_GetHTotal50Hz();
        rPANEL_2D[SV_DCLK_50HZ].u2VTotal = PANEL_GetVTotal50Hz();
        rPANEL_2D[SV_DCLK_50HZ].u4PixelClk = PANEL_GetPixelClk50Hz();
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        rPANEL_2D[SV_DCLK_50HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_50HZ);
        rPANEL_2D[SV_DCLK_50HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_50HZ);
        #else
        rPANEL_2D[SV_DCLK_50HZ].u4VsyncWidth = PANEL_GetVSyncWidth();
        rPANEL_2D[SV_DCLK_50HZ].u4VsyncBp = u4GetPanelVSyncBp();
        #endif

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_2D[SV_DCLK_48HZ].u2FrameRate = 48;
	        rPANEL_2D[SV_DCLK_48HZ].u2Width = PANEL_GetPanelWidth();
	        rPANEL_2D[SV_DCLK_48HZ].u2Height = PANEL_GetPanelHeight();
	        rPANEL_2D[SV_DCLK_48HZ].u2HTotal = PANEL_GetHTotal48Hz();
	        rPANEL_2D[SV_DCLK_48HZ].u2VTotal = PANEL_GetVTotal48Hz();
	        rPANEL_2D[SV_DCLK_48HZ].u4PixelClk = PANEL_GetPixelClk48Hz();
	        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
	        rPANEL_2D[SV_DCLK_48HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_48HZ);
	        rPANEL_2D[SV_DCLK_48HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_48HZ);
	        #else
	        rPANEL_2D[SV_DCLK_48HZ].u4VsyncWidth = PANEL_GetVSyncWidth();
	        rPANEL_2D[SV_DCLK_48HZ].u4VsyncBp = u4GetPanelVSyncBp();
			#endif
		}

        rPANEL_2D_COMMON.u4PixleClockMax = PANEL_GetPixelClkMax();
        rPANEL_2D_COMMON.u4PixleClockMin = PANEL_GetPixelClkMin();
        rPANEL_2D_COMMON.u2VTotalMax = PANEL_GetVTotalMax();
        rPANEL_2D_COMMON.u2VTotalMin = PANEL_GetVTotalMin();
        rPANEL_2D_COMMON.u2VClkMax = PANEL_GetVClkMax();

		
		if(SUPPORT_PANEL_48HZ)
		{
			u1Cnt=SV_DCLK_48HZ;
		}
		else
		{
			u1Cnt=SV_DCLK_50HZ;
		}

        for( ;u1Cnt<SV_DCLK_OTHERS;u1Cnt++)
        {
            rPANEL_3D[u1Cnt].u2FrameRate = rPANEL_2D[u1Cnt].u2FrameRate;
            rPANEL_3D[u1Cnt].u2Width = rPANEL_2D[u1Cnt].u2Width;
            rPANEL_3D[u1Cnt].u2Height = rPANEL_2D[u1Cnt].u2Height << 1;
            rPANEL_3D[u1Cnt].u2HTotal = rPANEL_2D[u1Cnt].u2HTotal;
            rPANEL_3D[u1Cnt].u2VTotal = rPANEL_2D[u1Cnt].u2VTotal << 1;
            rPANEL_3D[u1Cnt].u4PixelClk = rPANEL_2D[u1Cnt].u4PixelClk << 1;
            rPANEL_3D[u1Cnt].u4VsyncWidth = rPANEL_2D[u1Cnt].u4VsyncWidth << 1;
            rPANEL_3D[u1Cnt].u4VsyncBp = rPANEL_2D[u1Cnt].u4VsyncBp << 1;
        }

        rPANEL_3D_COMMON.u4PixleClockMax = rPANEL_2D_COMMON.u4PixleClockMax << 1;
        rPANEL_3D_COMMON.u4PixleClockMin = rPANEL_2D_COMMON.u4PixleClockMin << 1;
        rPANEL_3D_COMMON.u2VTotalMax = rPANEL_2D_COMMON.u2VTotalMax << 1;
        rPANEL_3D_COMMON.u2VTotalMin = rPANEL_2D_COMMON.u2VTotalMin << 1;
        rPANEL_3D_COMMON.u2VClkMax = rPANEL_2D_COMMON.u2VClkMax << 1;

        vPanelAttribute_Status();

}

static void vSavePanelAttribute(void)
{
    u2OrigWidth = PANEL_GetPanelWidth();
    u2OrigHeight = PANEL_GetPanelHeight();

    au2OrigHTotal[0] = PANEL_GetHTotal60Hz();
    au2OrigVTotal[0] = PANEL_GetVTotal60Hz();

    au2OrigHTotal[1] = PANEL_GetHTotal50Hz();
    au2OrigVTotal[1] = PANEL_GetVTotal50Hz();

	if (SUPPORT_PANEL_48HZ)
	{
	    au2OrigHTotal[2] = PANEL_GetHTotal48Hz();
	    au2OrigVTotal[2] = PANEL_GetVTotal48Hz();
	}

    au4OrigPixelMinMax[0] = PANEL_GetPixelClkMin();
    au4OrigPixelMinMax[1] = PANEL_GetPixelClkMax();
    u1OrigVClkMax = PANEL_GetVClkMax();

}

void vDrvDisp3DModeChg(UINT8 u13DMode)
{
    static UINT8 u1fg3DMode_Backup=0;
    UINT8 u1fg3DMode=0;
    UINT8 u1fg3DMode_ModChg=0;
    UINT32 u4CtlWord;
    UINT8 u1Cnt=0;
    UINT32 dwDCLK = PANEL_GetPixelClk60Hz();


    #if DBG_3D_PANEL_ATTRIBUTE
    Printf("[PDP_NEW] vDrvDisp3DModeChg u13DMode= %d\n", u13DMode);
    #endif

    if(!IS_PANEL_2D_N_3D_L12R12)
    {
        return;
    }

    if(u13DMode)
    {
        u1fg3DMode = 1;
    }
    else
    {
        u1fg3DMode = 0;
    }

    if (u1fg3DMode != u1fg3DMode_Backup)
    {
        u1fg3DMode_ModChg = 1;
        u1fg3DMode_Backup = u1fg3DMode;
    }
    else
    {
        u1fg3DMode_ModChg =0;
    }

    #if DBG_3D_PANEL_ATTRIBUTE
    vPanelAttribute_Status();
    #endif

    #if DBG_3D_PANEL_ATTRIBUTE
    Printf("[PDP_NEW] vDrvDisp3DModeChg u1fg3DMode_ModChg= %d\n", u1fg3DMode_ModChg);
    #endif

    if(u1fg3DMode_ModChg)
    {

    vDrvSetAllMute(SV_ON);

    vDrvLVDSOff();

    if(u1fg3DMode)
    {
        if(fgPDPL12R12)
            PANEL_SetLvdsPixel(2); // DUAL -> FHD
        else
            PANEL_SetLvdsPixel(1); // SINGLE -> DUAL
    }
    else
    {
        if(fgPDPL12R12)
           PANEL_SetLvdsPixel(1); // DUAL
        else
           PANEL_SetLvdsPixel(0); // SINGLE
    }

    u4CtlWord = PANEL_GetControlWord() & 0xFBFFFFFF;
	
	if(SUPPORT_PANEL_48HZ)
	{
		u1Cnt=SV_DCLK_48HZ;
	}
	else
	{
		u1Cnt=SV_DCLK_50HZ;
	}

    if(u1fg3DMode)
    {
        u4CtlWord = u4CtlWord | PANEL_L12R12;
        fgIsVsyncISRInit = SV_FALSE;
		
		for( ;u1Cnt<SV_DCLK_OTHERS;u1Cnt++)
        {
            updatePanelAttribute_PDP(rPANEL_3D[u1Cnt].u2HTotal,
                                rPANEL_3D[u1Cnt].u2VTotal,
                                rPANEL_3D[u1Cnt].u4PixelClk,
                                rPANEL_3D[u1Cnt].u2Width,
                                rPANEL_3D[u1Cnt].u2Height,
                                rPANEL_3D[u1Cnt].u4VsyncWidth,
                                rPANEL_3D[u1Cnt].u4VsyncBp,
                                rPANEL_3D[u1Cnt].u2FrameRate);
        }

        PANEL_SetPixelClkMax(rPANEL_3D_COMMON.u4PixleClockMax);
        PANEL_SetPixelClkMin(rPANEL_3D_COMMON.u4PixleClockMin);
        PANEL_SetVTotalMax(rPANEL_3D_COMMON.u2VTotalMax);
        PANEL_SetVTotalMin(rPANEL_3D_COMMON.u2VTotalMin);
        PANEL_SetVClkMax(rPANEL_3D_COMMON.u2VClkMax);
     }
     else
     {
		 for( ;u1Cnt<SV_DCLK_OTHERS;u1Cnt++)
         {
             updatePanelAttribute_PDP(rPANEL_2D[u1Cnt].u2HTotal,
                                 rPANEL_2D[u1Cnt].u2VTotal,
                                 rPANEL_2D[u1Cnt].u4PixelClk,
                                 rPANEL_2D[u1Cnt].u2Width,
                                 rPANEL_2D[u1Cnt].u2Height,
                                 rPANEL_2D[u1Cnt].u4VsyncWidth,
                                 rPANEL_2D[u1Cnt].u4VsyncBp,
                                 rPANEL_2D[u1Cnt].u2FrameRate);
         }

        PANEL_SetPixelClkMax(rPANEL_2D_COMMON.u4PixleClockMax);
        PANEL_SetPixelClkMin(rPANEL_2D_COMMON.u4PixleClockMin);
        PANEL_SetVTotalMax(rPANEL_2D_COMMON.u2VTotalMax);
        PANEL_SetVTotalMin(rPANEL_2D_COMMON.u2VTotalMin);
        PANEL_SetVClkMax(rPANEL_2D_COMMON.u2VClkMax);
        }

    PANEL_SetControlWord(u4CtlWord);
    vDrvLVDSConfig();
    vDrvLVDSInit();
    vErrorHandleSetByPanel();
    if ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ) || (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_OTHERS))
    {
        dwDCLK = PANEL_GetPixelClk60Hz();
    }
    else if (SUPPORT_PANEL_48HZ && (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ))
    {
        dwDCLK = PANEL_GetPixelClk48Hz();
    }
    else // (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_50HZ)
    {
        dwDCLK = PANEL_GetPixelClk50Hz();
    }

    #if DBG_3D_PANEL_ATTRIBUTE
    Printf("[PDP_NEW] dwDCLK %d\n", dwDCLK);
    #endif

    #ifdef CC_MTK_LOADER
    vDrvVOPLLFreqSet(dwDCLK);
    #else
    vDrvVOFreqSet(dwDCLK);
    #endif
    fgIsVsyncIsrStart = 0;
    vDrvSetPanelTiming();

    OSD_BASE_SwitchResolution();

    vDrvMJCSetModeChangeTrig();
    if(bDrvVideoSignalStatus(SV_VP_MAIN) == (UINT8)SV_VDO_STABLE)
    {
        vSetMainFlg(MAIN_FLG_MODE_CHG);
        vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
    }
    vDrvLVDSOn();
    vDrvSetAllMute(SV_OFF);
    }

    return;
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
    vDrvSetPanelTiming();
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
  printf("[LVDS]FLD_TCLK_SEL=%d\n",IO32ReadFldAlign(CKGEN_TCLK_TEST,FLD_TCLK_SEL));
  u1FlipMirrorReset = 1;
  vIO32WriteFldAlign(CKGEN_TCLK_TEST, 0, FLD_TCLK_SEL);	// xtal to tclk for tco isr.
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

#define FRC_UNMUTE_DELAY 500 // 2 sec

#ifndef CC_MTK_LOADER
static void updatePanelAttribute_4k2k(UINT16 u2HTotal,
                                 UINT16 u2VTotal,
                                 UINT32 u2PixelClk,
                                 UINT16 u2Width,
                                 UINT16 u2Height,
                                 UINT16 u2VSyncWidth,
                                 UINT16 u2VSyncBP,
                                 UINT16 u2HSyncWidth,
                                 UINT16 u2HsyncFP,
                                 UINT16 u2FrameRate)
{
    PANEL_SetPanelWidth(u2Width);
    PANEL_SetPanelHeight(u2Height);

    if(u2FrameRate == 25 || u2FrameRate == 50 || u2FrameRate == 100)
    {
        Printf("[4k2k][LVDS]%dHz input --> 50Hz output\n", u2FrameRate);
        PANEL_SetHTotal50Hz(u2HTotal);
        PANEL_SetVTotal50Hz(u2VTotal);
        PANEL_SetPixelClk50Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_50HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_50HZ, u2VSyncBP);
		PANEL_SetHSyncWidthByTiming(SV_DCLK_50HZ,u2HSyncWidth);
		PANEL_SetHSyncFpByTiming(SV_DCLK_50HZ,u2HsyncFP);
        #endif
    }
    else if(SUPPORT_PANEL_48HZ && (u2FrameRate == 24 || u2FrameRate == 48 || u2FrameRate == 96))
    {
        Printf("[4k2k][LVDS]%dHz input --> 48Hz output\n", u2FrameRate);
        PANEL_SetHTotal48Hz(u2HTotal);
        PANEL_SetVTotal48Hz(u2VTotal);
        PANEL_SetPixelClk48Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_48HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_48HZ, u2VSyncBP);
		PANEL_SetHSyncWidthByTiming(SV_DCLK_48HZ,u2HSyncWidth);
		PANEL_SetHSyncFpByTiming(SV_DCLK_48HZ,u2HsyncFP);
        #endif
    }
    else
    {
        Printf("[4k2k][LVDS]%dHz input --> 60Hz output\n", u2FrameRate);
        PANEL_SetHTotal60Hz(u2HTotal);
        PANEL_SetVTotal60Hz(u2VTotal);
        PANEL_SetPixelClk60Hz(u2PixelClk);
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        PANEL_SetVSyncWidthByTiming(SV_DCLK_60HZ,u2VSyncWidth);
        PANEL_SetVSyncBpByTiming(SV_DCLK_60HZ, u2VSyncBP);
		PANEL_SetHSyncWidthByTiming(SV_DCLK_60HZ,u2HSyncWidth);
		PANEL_SetHSyncFpByTiming(SV_DCLK_60HZ,u2HsyncFP);
        #endif
    }
}
#ifdef CC_SUPPORT_4K2K
extern r4k2kHandler *psCur4k2kHandler;
#endif

UINT32 vDrvCal4K2KPanelType(void)
{  


    UINT32 u4Type = PANEL_TYPE_2K1K_60;
#ifndef CC_MTK_LOADER
#ifdef CC_SUPPORT_4K2K
    VDP_SEAMLESS_INFO_T b2rVrmInfo;
    
	if(IS_SUPPORT_4K2K_DISPLAY)
	{
	
		if(IS_DISP_4K2K_TYPE2)
		{
			u4Type = fgIs4k2kNativeTimingOut(SV_VP_MAIN) ? PANEL_TYPE_4K2K_30 :
			 u1DrvTDTVCurIs3DMode() == SV_TRUE ? PANEL_TYPE_2K1K_120 : PANEL_TYPE_2K1K_60;
		}
		else if(IS_DISP_4K2K_TYPE4)
		{
			if(fgIs4k2kNativeTimingOut(SV_VP_MAIN))
			{
                         if ( fgPhotoRasterModeIn4k || 
                              VDP_GetSeamlessInfo(VDP_1, &b2rVrmInfo) != VDP_SET_ERROR)
                         { //photo  seamless  use PQ path 4k2k@30
                            u4Type = PANEL_TYPE_4K2K_30;
                         }
                         else
                         {
                            u4Type = PANEL_TYPE_4K2K_60;
                         }
		 	}
			else
			{
			    u4Type = (u1DrvTDTVCurIs3DMode() == SV_TRUE) ? PANEL_TYPE_2K1K_120 : PANEL_TYPE_2K1K_60;
			}
		}
#ifdef   CC_SUPPORT_UHD		
		else if(IS_DISP_4K2K_TYPE5)   //for oryx 4KDD module
		{
			if(fgIs4k2kNativeTimingOut(SV_VP_MAIN))
			{
				if ( fgPhotoRasterModeIn4k || 
					 VDP_GetSeamlessInfo(VDP_1, &b2rVrmInfo) != VDP_SET_ERROR)
				{ //photo  seamless  use PQ path 4k2k@30
			   		u4Type = PANEL_TYPE_2K2K_60;
				}
				else
				{
			   		u4Type = PANEL_TYPE_4K2K_60;
				}
			}
			else
			{
			    u4Type = (u1DrvTDTVCurIs3DMode() == SV_TRUE) ? PANEL_TYPE_2K1K_120 : PANEL_TYPE_2K2K_60;			
			}
		}
#endif		
		else
		{
			if(fgIs4k2kNativeTimingOut(SV_VP_MAIN))//4
			{
                         if ( fgPhotoRasterModeIn4k || 
                              VDP_GetSeamlessInfo(VDP_1, &b2rVrmInfo) != VDP_SET_ERROR)
                         { //photo  seamless  use PQ path 4k2k@30
                            u4Type = PANEL_TYPE_4K2K_30;
                         }
                         else
                         {
                            u4Type = PANEL_TYPE_4K2K_60;
                         }
		 	}
			else
			{
			    u4Type = ((u1DrvTDTVCurIs3DMode() == SV_TRUE) || (bDrvVideoGetRefreshRate(SV_VP_MAIN) >= 96)) ? PANEL_TYPE_2K1K_120 : PANEL_TYPE_2K1K_60;
			}
		}
	}


	
#endif
#endif
return u4Type;

}

void vDrvSwitchVideoType(UINT32 u4Type)
{
    UINT32 u4CtlWord = 0;
	UINT32 u4CtlWord2 = 0;
	UINT8 u1PnlMode = 0;
	UINT32 i4panelType = E_4K2K_NATIVE;
	UINT32 u4OSDMode=OSD_4K2K_MODE_NONE;
    if (_4k2kPanelType == u4Type)
    {
        Printf("[4KBE] Skip switch panel mode(%d).\n", _4k2kPanelType);
		fg4k2kPNLTypeChg = SV_FALSE;
        return;
    }
	Printf("[4KBE] old panel mode(%d)  new(%d)\n", _4k2kPanelType, u4Type);
	#ifdef CC_SUPPORT_4K2K
	if((_4k2kPanelType == PANEL_TYPE_4K2K_60)&&(u4Type != PANEL_TYPE_4K2K_60))
	{
	    _fgBypassToPQ = SV_TRUE;
		fgIsVsyncISRInit = SV_FALSE;
	}
	#endif
	fg4k2kPNLTypeChg = SV_TRUE;
    _4k2kPanelType = u4Type;
	bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;  // Force vDrvCalPanelTiming(...) to run, if _4k2kPanelType changes.
//	#ifdef CC_MT5890
//	vDrvSwitchOSDRoute();
//	#endif
	if(IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
	{
	    #ifdef CC_SUPPORT_4K2K
	    VERIFY (x_sema_lock(_hFrcSetPanelSema, X_SEMA_OPTION_WAIT) == OSR_OK);
		#endif
	    u4CtlWord = PANEL_GetControlWord() & 0xFFFFFFE7;
        u4CtlWord2 = PANEL_GetControlWord2() & 0xFFFFFDFF;  // clean linebuffer bit first
	}
    switch (u4Type)
    {
        case PANEL_TYPE_4K2K_60:
            vSetPlaneEnable(VDP_BYPASS, SV_ON);
            vSetPlaneEnable(VDP_PQPATH, SV_OFF);
            vSetPlaneWidth(VDP_BYPASS, 3840);
            vSetPlaneHeight(VDP_BYPASS, 2160);
            if (IS_IC_5861())
            {
            	vSetOutputPlane(VDP_1, VDP_PQPATH);
            	vSetOutputPlane(VDP_2, VDP_PQPATH);
            	vScpipTGInOutSel(E_FSC_TG);
            }
			else
			{
            	vSetOutputPlane(VDP_1, VDP_BYPASS);
            	vSetOutputPlane(VDP_2, VDP_GFXOUT);
	        	vScpipTGInOutSel(E_GFX_TG);
			}
			if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
			{
				u4CtlWord = u4CtlWord | OCTA_PORT;
				u4CtlWord2 = u4CtlWord2 | DISABLE_LVDS_LINEBUFFER;
				i4panelType = PANEL_4K2K_60;
			}
            break;
        case PANEL_TYPE_4K1K_60:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1920);
            vSetPlaneHeight(VDP_PQPATH, 1080);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
			if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
			{
				u4CtlWord = u4CtlWord | FOUR_PORT;	
				u4CtlWord2 = u4CtlWord2 | DISABLE_LVDS_LINEBUFFER;
			}
            break;
		case PANEL_TYPE_4K2K_30:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 3840);
            vSetPlaneHeight(VDP_PQPATH, 2160);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
			if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
			{
				u4CtlWord = u4CtlWord | FOUR_PORT;
				u4CtlWord2 = u4CtlWord2 | DISABLE_LVDS_LINEBUFFER;
				i4panelType = PANEL_4K2K_30;
				u1PnlMode = E_4K2K_NATIVE;
				u4OSDMode = OSD_4K2K_MODE_HDMI_4K2K;
			}
            break;
		case PANEL_TYPE_2K1K_60:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1920);
            vSetPlaneHeight(VDP_PQPATH, 1080);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
			if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
			{
			    
				u4CtlWord = u4CtlWord | DUAL_PORT;	
				i4panelType = PANEL_4K2K_FHD60;
				u1PnlMode = E_4K2K_NORMAL;
				u4OSDMode = OSD_4K2K_MODE_NONE;
			}
            break;
		case PANEL_TYPE_720P_120:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1280);
            vSetPlaneHeight(VDP_PQPATH, 720);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
            break;
		case PANEL_TYPE_1920_540P:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1920);
            vSetPlaneHeight(VDP_PQPATH, 540);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
            break;
#ifdef CC_SUPPORT_UHD	
		case PANEL_TYPE_2K2K_60:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1920);
            vSetPlaneHeight(VDP_PQPATH, 2160);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
			break;
#endif			
        default:
        case PANEL_TYPE_2K1K_120:
            vSetPlaneEnable(VDP_BYPASS, SV_OFF);
            vSetPlaneEnable(VDP_PQPATH, SV_ON);
            vSetPlaneWidth(VDP_PQPATH, 1920);
            vSetPlaneHeight(VDP_PQPATH, 1080);
            vSetOutputPlane(VDP_1, VDP_PQPATH);
            vSetOutputPlane(VDP_2, VDP_PQPATH);
            vScpipTGInOutSel(E_FSC_TG);
			if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
			{
		        u4CtlWord = u4CtlWord | FOUR_PORT;
				i4panelType = PANEL_4K2K_FHD120;
				u1PnlMode = E_4K2K_NORMAL_3D;
				u4OSDMode = OSD_4K2K_MODE_NONE;
			}
            break;
    }
	if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
	{
	    #ifdef CC_SUPPORT_4K2K
	    vExt4k2kFrcMute(SV_ON);
		#endif
		vDrvSetAllMute(SV_ON);
	    vDrvVByOneOff();	
	    PANEL_SetControlWord(u4CtlWord);
		PANEL_SetControlWord2(u4CtlWord2);
    	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
	    {		
			vDrvVB14K2KPanelInterfaceConfig();	    
    	}
    }

    {
        GRAPHMODE_ATTRIBUTE_T * prVideoModeAttr;
        prVideoModeAttr = VideoMode_GetAttributeByType(u4Type);
		if(prVideoModeAttr==NULL)
		{
			printf("[4K2K]VideoModeAttribute[]for panel_#%d is NULL!!\n", GetCurrentPanelIndex());
		}
		else
		{
	        if (SUPPORT_PANEL_48HZ)
	        {
		        updatePanelAttribute_4k2k(
		            prVideoModeAttr->au2HTotal[SV_DCLK_48HZ],
		            prVideoModeAttr->au2VTotal[SV_DCLK_48HZ],
		            prVideoModeAttr->au4PixelClk[SV_DCLK_48HZ],
		            prVideoModeAttr->u2GraphModeWidth,
		            prVideoModeAttr->u2GraphModeHeight,
		            prVideoModeAttr->au1VSyncWidth[SV_DCLK_48HZ],
		            prVideoModeAttr->au2VBackPorch[SV_DCLK_48HZ],
		            prVideoModeAttr->au1HSyncWidth[SV_DCLK_48HZ],
		            prVideoModeAttr->au2HFrontPorch[SV_DCLK_48HZ],
		            48);
	        }
	        updatePanelAttribute_4k2k(
	            prVideoModeAttr->au2HTotal[SV_DCLK_50HZ],
	            prVideoModeAttr->au2VTotal[SV_DCLK_50HZ],
	            prVideoModeAttr->au4PixelClk[SV_DCLK_50HZ],
	            prVideoModeAttr->u2GraphModeWidth,
	            prVideoModeAttr->u2GraphModeHeight,
	            prVideoModeAttr->au1VSyncWidth[SV_DCLK_50HZ],
	            prVideoModeAttr->au2VBackPorch[SV_DCLK_50HZ],
	            prVideoModeAttr->au1HSyncWidth[SV_DCLK_50HZ],
	            prVideoModeAttr->au2HFrontPorch[SV_DCLK_50HZ],
	            50);
	        updatePanelAttribute_4k2k(
	            prVideoModeAttr->au2HTotal[SV_DCLK_60HZ],
	            prVideoModeAttr->au2VTotal[SV_DCLK_60HZ],
	            prVideoModeAttr->au4PixelClk[SV_DCLK_60HZ],
	            prVideoModeAttr->u2GraphModeWidth,
	            prVideoModeAttr->u2GraphModeHeight,
	            prVideoModeAttr->au1VSyncWidth[SV_DCLK_60HZ],
	            prVideoModeAttr->au2VBackPorch[SV_DCLK_60HZ],
	            prVideoModeAttr->au1HSyncWidth[SV_DCLK_60HZ],
	            prVideoModeAttr->au2HFrontPorch[SV_DCLK_60HZ],
	            60);

	        PANEL_SetVClkMax(prVideoModeAttr->u1FrameRateMax);
	        PANEL_SetVClkMin(prVideoModeAttr->u1FrameRateMin);
		}
    }
	if (IS_DISP_4K2K_TYPE2 || IS_DISP_4K2K_TYPE4)
	{
		vErrorHandleSetByPanel();
		#ifdef CC_MTK_LOADER
		vDrvVOPLLFreqSet(
					(vDrvGetLCDFreq()==25 || vDrvGetLCDFreq()==50 || vDrvGetLCDFreq()==100)?
						(PANEL_GetPixelClk50Hz()):
						(SUPPORT_PANEL_48HZ && (vDrvGetLCDFreq()==24 || vDrvGetLCDFreq()==48))?
						(PANEL_GetPixelClk48Hz()):
						(PANEL_GetPixelClk60Hz()));			
        #else

	    vDrvVOFreqSet(
	        (vDrvGetLCDFreq()==25 || vDrvGetLCDFreq()==50 || vDrvGetLCDFreq()==100)?
	            (PANEL_GetPixelClk50Hz()):
            (SUPPORT_PANEL_48HZ && (vDrvGetLCDFreq()==24 || vDrvGetLCDFreq()==48))?
            (PANEL_GetPixelClk48Hz()):
            (PANEL_GetPixelClk60Hz()));
       #endif 

        vDrvSetPanelTiming();
		IGNORE_RET(OSD_UpdateTiming());
		IGNORE_RET(OSD_Base_SetHStart());

    	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    	{
			vDrvVByOneOn();
    	}

		//Switch OSD mode
		OSD_PLA_SetLinuxUIMode(u4OSDMode,0,0,0,0);
	#ifdef CC_SUPPORT_4K2K
        vSend4k2kFrcModeCmd(u1PnlMode);
       	x_thread_delay(50);
		vSend4k2kFrcPanelTypeCmd(i4panelType);  // move to here
	
	if (psCur4k2kHandler && (psCur4k2kHandler->frc_snd_mute==NULL))
    {
        Printf("[4k2k]No FRC mute --> set all mute!\n");
        vDrvSetAllMute(SV_OFF);
    }
#ifndef NDEBUG
    Printf("[4k2k]unmute timer start %d\n", u2Ext4k2kGetMuteTime());
#endif
    vExt4k2kFrcUmMuteTimer(u2Ext4k2kGetMuteTime());
	x_sema_unlock(_hFrcSetPanelSema);
	#endif
		
	}
	#ifndef CC_MTK_LOADER
#ifdef PANEL_FHD_BACKEND
	if (IS_DISP_2K_TYPE1)
	{
		UINT32 srcW=0,srcH=0,dstW=0,dstH=0,Enable=0,u4bpp=0;
		UINT32 u4Width = u4GetPlaneWidth(VDP_PQPATH);
		UINT32 u4Height = u4GetPlaneHeight(VDP_PQPATH);
		OSD_SC_GetScalerInfo(OSD_SCALER_3,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_3,Enable,srcW,srcH,u4Width,u4Height);
		OSD_SC_GetScalerInfo(OSD_SCALER_2,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_2,Enable,srcW,srcH,u4Width,u4Height);
		OSD_SC_GetScalerInfo(OSD_SCALER_1,&Enable,&srcW,&srcH,&dstW,&dstH,&u4bpp);
		OSD_SC_Scale(OSD_SCALER_1,Enable,srcW,srcH,u4Width,u4Height);
	}
#endif
#endif
	#ifndef CC_MTK_LOADER
	#ifdef CC_MAPLE_CUST_DRV
	#if defined(CC_MT5890) && defined(CC_SUPPORT_4KBE)   
		DRVCUST_SendEvent(E_CUST_SCALING_REQ_CHANGE, u1BePath[0]);
        DRVCUST_SendEvent(E_CUST_SRC_AREA_CHANGE, u1BePath[0]);
	
	    DRVCUST_SendEvent(E_CUST_SCALING_REQ_CHANGE, u1BePath[1]);
		DRVCUST_SendEvent(E_CUST_SRC_AREA_CHANGE, u1BePath[1]);
	#endif
	#endif
	#endif
}

#ifdef CC_SUPPORT_4K2K
#define DBG_CMI_4K2K_PANEL_ATTRIBUTE 0
void vPanelAttribute_Status_4k2k(void)
{
#if DBG_CMI_4K2K_PANEL_ATTRIBUTE
    UINT8 u1Cnt=0;

	if(SUPPORT_PANEL_48HZ)
	{
		u1Cnt=SV_DCLK_48HZ;
	}
	else
	{
		u1Cnt=SV_DCLK_50HZ;
	}

	for( ;u1Cnt<SV_DCLK_OTHERS;u1Cnt++)	
    {
        Printf("FHD120_u2FrameRate[%d] = %d\n",u1Cnt, rPANEL_FHD120[u1Cnt].u2FrameRate);
        Printf("FHD120_u2Width[%d] = %d\n",u1Cnt, rPANEL_FHD120[u1Cnt].u2Width);
        Printf("FHD120_u2Height[%d] = %d\n", u1Cnt,rPANEL_FHD120[u1Cnt].u2Height );
        Printf("FHD120_u2HTotal[%d] = %d\n", u1Cnt,rPANEL_FHD120[u1Cnt].u2HTotal);
        Printf("FHD120_u2VTotal[%d] = %d\n", u1Cnt,rPANEL_FHD120[u1Cnt].u2VTotal);
        Printf("FHD120_u4PixelClk[%d] = %d\n", u1Cnt,rPANEL_FHD120[u1Cnt].u4PixelClk);
        Printf("FHD120_u4VsyncWidth[%d] = %d\n", u1Cnt, rPANEL_FHD120[u1Cnt].u4VsyncWidth);
        Printf("FHD120_u4VsyncBp[%d] = %d\n", u1Cnt, rPANEL_FHD120[u1Cnt].u4VsyncBp);

        Printf("FHD60_u2FrameRate[%d] = %d\n",u1Cnt, rPANEL_FHD60[u1Cnt].u2FrameRate);
        Printf("FHD60_u2Width[%d] = %d\n",u1Cnt, rPANEL_FHD60[u1Cnt].u2Width);
        Printf("FHD60_u2Height[%d] = %d\n", u1Cnt,rPANEL_FHD60[u1Cnt].u2Height );
        Printf("FHD60_u2HTotal[%d] = %d\n", u1Cnt,rPANEL_FHD60[u1Cnt].u2HTotal);
        Printf("FHD60_u2VTotal[%d] = %d\n", u1Cnt,rPANEL_FHD60[u1Cnt].u2VTotal);
        Printf("FHD60_u4PixelClk[%d] = %d\n", u1Cnt,rPANEL_FHD60[u1Cnt].u4PixelClk);
        Printf("FHD60_u4VsyncWidth[%d] = %d\n", u1Cnt, rPANEL_FHD60[u1Cnt].u4VsyncWidth);
        Printf("FHD60_u4VsyncBp[%d] = %d\n", u1Cnt, rPANEL_FHD60[u1Cnt].u4VsyncBp);

        Printf("4K2K30_u2FrameRate[%d] = %d\n",u1Cnt, rPANEL_4K2K30[u1Cnt].u2FrameRate);
        Printf("4K2K30_u2Width[%d] = %d\n",u1Cnt, rPANEL_4K2K30[u1Cnt].u2Width);
        Printf("4K2K30_u2Height[%d] = %d\n", u1Cnt,rPANEL_4K2K30[u1Cnt].u2Height );
        Printf("4K2K30_u2HTotal[%d] = %d\n", u1Cnt,rPANEL_4K2K30[u1Cnt].u2HTotal);
        Printf("4K2K30_u2VTotal[%d] = %d\n", u1Cnt,rPANEL_4K2K30[u1Cnt].u2VTotal);
        Printf("4K2K30_u4PixelClk[%d] = %d\n", u1Cnt,rPANEL_4K2K30[u1Cnt].u4PixelClk);
        Printf("4K2K30_u4VsyncWidth[%d] = %d\n", u1Cnt, rPANEL_4K2K30[u1Cnt].u4VsyncWidth);
        Printf("4K2K30_u4VsyncBp[%d] = %d\n", u1Cnt, rPANEL_4K2K30[u1Cnt].u4VsyncBp);
    }

    Printf("FHD120_COMMON.u4PixleClockMax = %d \n ", rPANEL_FHD120_COMMON.u4PixleClockMax);
    Printf("FHD120_COMMON.u4PixleClockMin = %d \n ", rPANEL_FHD120_COMMON.u4PixleClockMin);
    Printf("FHD120_COMMON.u2HTotalMax = %d \n ", rPANEL_FHD120_COMMON.u2HTotalMax);
    Printf("FHD120_COMMON.u2HTotalMin = %d \n ", rPANEL_FHD120_COMMON.u2HTotalMin);
    Printf("FHD120_COMMON.u2VTotalMax = %d \n ", rPANEL_FHD120_COMMON.u2VTotalMax);
    Printf("FHD120_COMMON.u2VTotalMin = %d \n ", rPANEL_FHD120_COMMON.u2VTotalMin);
    Printf("FHD120_COMMON.u2VClkMax = %d \n ", rPANEL_FHD120_COMMON.u2VClkMax);
    Printf("FHD120_COMMON.u2VClkMin = %d \n ", rPANEL_FHD120_COMMON.u2VClkMin);

    Printf("FHD60_COMMON.u4PixleClockMax = %d \n ", rPANEL_FHD60_COMMON.u4PixleClockMax);
    Printf("FHD60_COMMON.u4PixleClockMin = %d \n ", rPANEL_FHD60_COMMON.u4PixleClockMin);
    Printf("FHD60_COMMON.u2HTotalMax = %d \n ", rPANEL_FHD60_COMMON.u2HTotalMax);
    Printf("FHD60_COMMON.u2HTotalMin = %d \n ", rPANEL_FHD60_COMMON.u2HTotalMin);
    Printf("FHD60_COMMON.u2VTotalMax = %d \n ", rPANEL_FHD60_COMMON.u2VTotalMax);
    Printf("FHD60_COMMON.u2VTotalMin = %d \n ", rPANEL_FHD60_COMMON.u2VTotalMin);
    Printf("FHD60_COMMON.u2VClkMax = %d \n ", rPANEL_FHD60_COMMON.u2VClkMax);
    Printf("FHD60_COMMON.u2VClkMin = %d \n ", rPANEL_FHD60_COMMON.u2VClkMin);

    Printf("4K2K30_COMMON.u4PixleClockMax = %d \n ", rPANEL_4K2K30_COMMON.u4PixleClockMax);
    Printf("4K2K30_COMMON.u4PixleClockMin = %d \n ", rPANEL_4K2K30_COMMON.u4PixleClockMin);
    Printf("4K2K30_COMMON.u2HTotalMax = %d \n ", rPANEL_4K2K30_COMMON.u2HTotalMax);
    Printf("4K2K30_COMMON.u2HTotalMin = %d \n ", rPANEL_4K2K30_COMMON.u2HTotalMin);
    Printf("4K2K30_COMMON.u2VTotalMax = %d \n ", rPANEL_4K2K30_COMMON.u2VTotalMax);
    Printf("4K2K30_COMMON.u2VTotalMin = %d \n ", rPANEL_4K2K30_COMMON.u2VTotalMin);
    Printf("4K2K30_COMMON.u2VClkMax = %d \n ", rPANEL_4K2K30_COMMON.u2VClkMax);
    Printf("4K2K30_COMMON.u2VClkMin = %d \n ", rPANEL_4K2K30_COMMON.u2VClkMin);
#endif
}

static void vSavePanelAttribute_4K2K(void)
{
    if (IS_SUPPORT_4K2K_DISPLAY)
    {
#ifdef CC_SUPPORT_UHD    
    if (IS_DISP_4K2K_TYPE1 || IS_DISP_4K2K_TYPE5 )
#else
	if (IS_DISP_4K2K_TYPE1 )
#endif
    {
        _4k2kPanelType = PANEL_4K2K_FHD120;
		vSetPlaneEnable(VDP_BYPASS, SV_OFF);
        vSetPlaneEnable(VDP_PQPATH, SV_ON);
        vSetPlaneWidth(VDP_PQPATH, 1920);
        vSetPlaneHeight(VDP_PQPATH, 1080);
        vSetOutputPlane(VDP_1, VDP_PQPATH);
        vSetOutputPlane(VDP_2, VDP_PQPATH);
        vScpipTGInOutSel(E_FSC_TG);
        Printf("[SA8] vSavePanelAttribute_4K2K --> IS_DISP_4K2K_TYPE1\n");

    // CMO_4K2K_FHD120
        rPANEL_FHD120[SV_DCLK_60HZ].u2FrameRate = 120;
        rPANEL_FHD120[SV_DCLK_60HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_FHD120[SV_DCLK_60HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_FHD120[SV_DCLK_60HZ].u2HTotal = PANEL_GetHTotal60Hz();
        rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal = PANEL_GetVTotal60Hz();
        rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk = PANEL_GetPixelClk60Hz();
        rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_60HZ);
        rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_60HZ);

        rPANEL_FHD120[SV_DCLK_50HZ].u2FrameRate = 100;
        rPANEL_FHD120[SV_DCLK_50HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_FHD120[SV_DCLK_50HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_FHD120[SV_DCLK_50HZ].u2HTotal = PANEL_GetHTotal50Hz();
        rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal = PANEL_GetVTotal50Hz();
        rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk = PANEL_GetPixelClk50Hz();
        rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_50HZ);
        rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_50HZ);

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_FHD120[SV_DCLK_48HZ].u2FrameRate = 96;
	        rPANEL_FHD120[SV_DCLK_48HZ].u2Width = PANEL_GetPanelWidth();
	        rPANEL_FHD120[SV_DCLK_48HZ].u2Height = PANEL_GetPanelHeight();
	        rPANEL_FHD120[SV_DCLK_48HZ].u2HTotal = PANEL_GetHTotal48Hz();
	        rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal = PANEL_GetVTotal48Hz();
	        rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk = PANEL_GetPixelClk48Hz();
	        rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_48HZ);
	        rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_48HZ);
        }

        rPANEL_FHD120_COMMON.u4PixleClockMax = PANEL_GetPixelClkMax();
        rPANEL_FHD120_COMMON.u4PixleClockMin = PANEL_GetPixelClkMin();
        rPANEL_FHD120_COMMON.u2HTotalMax = PANEL_GetHTotalMax();
        rPANEL_FHD120_COMMON.u2HTotalMin = PANEL_GetHTotalMin();
        rPANEL_FHD120_COMMON.u2VTotalMax = PANEL_GetVTotalMax();
        rPANEL_FHD120_COMMON.u2VTotalMin = PANEL_GetVTotalMin();
        rPANEL_FHD120_COMMON.u2VClkMax = PANEL_GetVClkMax();
        rPANEL_FHD120_COMMON.u2VClkMin= PANEL_GetVClkMin();

    // CMO_4K2K_FHD60
        rPANEL_FHD60[SV_DCLK_60HZ].u2FrameRate = 60;
        rPANEL_FHD60[SV_DCLK_60HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width;
        rPANEL_FHD60[SV_DCLK_60HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height;
        rPANEL_FHD60[SV_DCLK_60HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2HTotal;
        rPANEL_FHD60[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal;
        rPANEL_FHD60[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk >> 1;
        rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
        rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;

        rPANEL_FHD60[SV_DCLK_50HZ].u2FrameRate = 50;
        rPANEL_FHD60[SV_DCLK_50HZ].u2Width = rPANEL_FHD120[SV_DCLK_50HZ].u2Width;
        rPANEL_FHD60[SV_DCLK_50HZ].u2Height = rPANEL_FHD120[SV_DCLK_50HZ].u2Height;
        rPANEL_FHD60[SV_DCLK_50HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2HTotal;
        rPANEL_FHD60[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal;
        rPANEL_FHD60[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk >> 1;
        rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth;
        rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp;

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_FHD60[SV_DCLK_48HZ].u2FrameRate = 48;
	        rPANEL_FHD60[SV_DCLK_48HZ].u2Width = rPANEL_FHD120[SV_DCLK_48HZ].u2Width;
	        rPANEL_FHD60[SV_DCLK_48HZ].u2Height = rPANEL_FHD120[SV_DCLK_48HZ].u2Height;
	        rPANEL_FHD60[SV_DCLK_48HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2HTotal;
	        rPANEL_FHD60[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal;
	        rPANEL_FHD60[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk >> 1;
	        rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth;
	        rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp;
        }

        rPANEL_FHD60_COMMON.u4PixleClockMax = rPANEL_FHD120_COMMON.u4PixleClockMax >> 1;
        rPANEL_FHD60_COMMON.u4PixleClockMin = rPANEL_FHD120_COMMON.u4PixleClockMin >> 1;
        rPANEL_FHD60_COMMON.u2HTotalMax = rPANEL_FHD120_COMMON.u2HTotalMax;
        rPANEL_FHD60_COMMON.u2HTotalMin = rPANEL_FHD120_COMMON.u2HTotalMin;
        rPANEL_FHD60_COMMON.u2VTotalMax = rPANEL_FHD120_COMMON.u2VTotalMax;
        rPANEL_FHD60_COMMON.u2VTotalMin = rPANEL_FHD120_COMMON.u2VTotalMin;
        rPANEL_FHD60_COMMON.u2VClkMax = 60;
        rPANEL_FHD60_COMMON.u2VClkMin= 47;

    // CMO_4K2K30
        rPANEL_4K2K30[SV_DCLK_60HZ].u2FrameRate = 30;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2HTotal = 4400;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;

        rPANEL_4K2K30[SV_DCLK_50HZ].u2FrameRate = 25;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2Width = rPANEL_FHD120[SV_DCLK_50HZ].u2Width << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2Height = rPANEL_FHD120[SV_DCLK_50HZ].u2Height << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2HTotal = 5280;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp;

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2FrameRate = 24;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2HTotal = 5500;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;
        }

        rPANEL_4K2K30_COMMON.u4PixleClockMax = rPANEL_FHD120_COMMON.u4PixleClockMax;
        rPANEL_4K2K30_COMMON.u4PixleClockMin = rPANEL_FHD120_COMMON.u4PixleClockMin;
        rPANEL_4K2K30_COMMON.u2HTotalMax = 5760;
        rPANEL_4K2K30_COMMON.u2HTotalMin = 3960;
        rPANEL_4K2K30_COMMON.u2VTotalMax = 2450;
        rPANEL_4K2K30_COMMON.u2VTotalMin = 2200;
        rPANEL_4K2K30_COMMON.u2VClkMax = 31;
        rPANEL_4K2K30_COMMON.u2VClkMin= 23;
    }
    else if (IS_DISP_4K2K_TYPE2)
    {

        _4k2kPanelType = PANEL_4K2K_FHD60;
		vSetPlaneEnable(VDP_BYPASS, SV_OFF);
        vSetPlaneEnable(VDP_PQPATH, SV_ON);
        vSetPlaneWidth(VDP_PQPATH, 1920);
        vSetPlaneHeight(VDP_PQPATH, 1080);
        vSetOutputPlane(VDP_1, VDP_PQPATH);
        vSetOutputPlane(VDP_2, VDP_PQPATH);
        vScpipTGInOutSel(E_FSC_TG);
        Printf("[SA8] vSavePanelAttribute_4K2K --> IS_DISP_4K2K_TYPE2\n");
    // AUO_4K2K_FHD60
        rPANEL_FHD60[SV_DCLK_60HZ].u2FrameRate = 60;
        rPANEL_FHD60[SV_DCLK_60HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_FHD60[SV_DCLK_60HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_FHD60[SV_DCLK_60HZ].u2HTotal = PANEL_GetHTotal60Hz();
        rPANEL_FHD60[SV_DCLK_60HZ].u2VTotal = PANEL_GetVTotal60Hz();
        rPANEL_FHD60[SV_DCLK_60HZ].u4PixelClk = PANEL_GetPixelClk60Hz();
        rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_60HZ);
        rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_60HZ);

        rPANEL_FHD60[SV_DCLK_50HZ].u2FrameRate = 50;
        rPANEL_FHD60[SV_DCLK_50HZ].u2Width = PANEL_GetPanelWidth();
        rPANEL_FHD60[SV_DCLK_50HZ].u2Height = PANEL_GetPanelHeight();
        rPANEL_FHD60[SV_DCLK_50HZ].u2HTotal = PANEL_GetHTotal50Hz();
        rPANEL_FHD60[SV_DCLK_50HZ].u2VTotal = PANEL_GetVTotal50Hz();
        rPANEL_FHD60[SV_DCLK_50HZ].u4PixelClk = PANEL_GetPixelClk50Hz();
        rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_50HZ);
        rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_50HZ);

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_FHD60[SV_DCLK_48HZ].u2FrameRate = 48;
	        rPANEL_FHD60[SV_DCLK_48HZ].u2Width = PANEL_GetPanelWidth();
	        rPANEL_FHD60[SV_DCLK_48HZ].u2Height = PANEL_GetPanelHeight();
	        rPANEL_FHD60[SV_DCLK_48HZ].u2HTotal = PANEL_GetHTotal48Hz();
	        rPANEL_FHD60[SV_DCLK_48HZ].u2VTotal = PANEL_GetVTotal48Hz();
	        rPANEL_FHD60[SV_DCLK_48HZ].u4PixelClk = PANEL_GetPixelClk48Hz();
	        rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncWidth = PANEL_GetVSyncWidthByTiming(SV_DCLK_48HZ);
	        rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncBp = PANEL_GetVSyncBpByTiming(SV_DCLK_48HZ);
        }

        rPANEL_FHD60_COMMON.u4PixleClockMax = PANEL_GetPixelClkMax();
        rPANEL_FHD60_COMMON.u4PixleClockMin = PANEL_GetPixelClkMin();
        rPANEL_FHD60_COMMON.u2HTotalMax = PANEL_GetHTotalMax();
        rPANEL_FHD60_COMMON.u2HTotalMin = PANEL_GetHTotalMin();
        rPANEL_FHD60_COMMON.u2VTotalMax = PANEL_GetVTotalMax();
        rPANEL_FHD60_COMMON.u2VTotalMin = PANEL_GetVTotalMin();
        rPANEL_FHD60_COMMON.u2VClkMax = PANEL_GetVClkMax();
        rPANEL_FHD60_COMMON.u2VClkMin= PANEL_GetVClkMin();

    // AUO_4K2K_FHD120
        rPANEL_FHD120[SV_DCLK_60HZ].u2FrameRate = 120;
        rPANEL_FHD120[SV_DCLK_60HZ].u2Width = rPANEL_FHD60[SV_DCLK_60HZ].u2Width;
        rPANEL_FHD120[SV_DCLK_60HZ].u2Height = rPANEL_FHD60[SV_DCLK_60HZ].u2Height;
        rPANEL_FHD120[SV_DCLK_60HZ].u2HTotal = rPANEL_FHD60[SV_DCLK_60HZ].u2HTotal;
        rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD60[SV_DCLK_60HZ].u2VTotal;
        rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD60[SV_DCLK_60HZ].u4PixelClk << 1;
        rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncWidth;
        rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD60[SV_DCLK_60HZ].u4VsyncBp;

        rPANEL_FHD120[SV_DCLK_50HZ].u2FrameRate = 100;
        rPANEL_FHD120[SV_DCLK_50HZ].u2Width = rPANEL_FHD60[SV_DCLK_50HZ].u2Width;
        rPANEL_FHD120[SV_DCLK_50HZ].u2Height = rPANEL_FHD60[SV_DCLK_50HZ].u2Height;
        rPANEL_FHD120[SV_DCLK_50HZ].u2HTotal = rPANEL_FHD60[SV_DCLK_50HZ].u2HTotal;
        rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD60[SV_DCLK_50HZ].u2VTotal;
        rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD60[SV_DCLK_50HZ].u4PixelClk << 1;
        rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncWidth;
        rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD60[SV_DCLK_50HZ].u4VsyncBp;

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_FHD120[SV_DCLK_48HZ].u2FrameRate = 96;
	        rPANEL_FHD120[SV_DCLK_48HZ].u2Width = rPANEL_FHD60[SV_DCLK_48HZ].u2Width;
	        rPANEL_FHD120[SV_DCLK_48HZ].u2Height = rPANEL_FHD60[SV_DCLK_48HZ].u2Height;
	        rPANEL_FHD120[SV_DCLK_48HZ].u2HTotal = rPANEL_FHD60[SV_DCLK_48HZ].u2HTotal;
	        rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD60[SV_DCLK_48HZ].u2VTotal;
	        rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD60[SV_DCLK_48HZ].u4PixelClk << 1;
	        rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncWidth;
	        rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD60[SV_DCLK_48HZ].u4VsyncBp;
        }

        rPANEL_FHD120_COMMON.u4PixleClockMax = rPANEL_FHD60_COMMON.u4PixleClockMax << 1;
        rPANEL_FHD120_COMMON.u4PixleClockMin = rPANEL_FHD60_COMMON.u4PixleClockMin << 1;
        rPANEL_FHD120_COMMON.u2HTotalMax = rPANEL_FHD60_COMMON.u2HTotalMax;
        rPANEL_FHD120_COMMON.u2HTotalMin = rPANEL_FHD60_COMMON.u2HTotalMin;
        rPANEL_FHD120_COMMON.u2VTotalMax = rPANEL_FHD60_COMMON.u2VTotalMax;
        rPANEL_FHD120_COMMON.u2VTotalMin = rPANEL_FHD60_COMMON.u2VTotalMin;
        rPANEL_FHD120_COMMON.u2VClkMax = 120;
        rPANEL_FHD120_COMMON.u2VClkMin= 60;

    // AUO_4K2K30
        rPANEL_4K2K30[SV_DCLK_60HZ].u2FrameRate = 30;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2HTotal = 4400;
        rPANEL_4K2K30[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal << 1;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
        rPANEL_4K2K30[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;

        rPANEL_4K2K30[SV_DCLK_50HZ].u2FrameRate = 25;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2Width = rPANEL_FHD120[SV_DCLK_50HZ].u2Width << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2Height = rPANEL_FHD120[SV_DCLK_50HZ].u2Height << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2HTotal = 5280;
        rPANEL_4K2K30[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal << 1;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth;
        rPANEL_4K2K30[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp;

        if (SUPPORT_PANEL_48HZ)
        {
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2FrameRate = 24;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2Width = rPANEL_FHD120[SV_DCLK_48HZ].u2Width << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2Height = rPANEL_FHD120[SV_DCLK_48HZ].u2Height << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2HTotal = 5500;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal << 1;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth;
	        rPANEL_4K2K30[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp;
        }

        rPANEL_4K2K30_COMMON.u4PixleClockMax = rPANEL_FHD120_COMMON.u4PixleClockMax;
        rPANEL_4K2K30_COMMON.u4PixleClockMin = rPANEL_FHD120_COMMON.u4PixleClockMin;
        rPANEL_4K2K30_COMMON.u2HTotalMax = 5760;
        rPANEL_4K2K30_COMMON.u2HTotalMin = 3960;
        rPANEL_4K2K30_COMMON.u2VTotalMax = 2450;
        rPANEL_4K2K30_COMMON.u2VTotalMin = 2200;
        rPANEL_4K2K30_COMMON.u2VClkMax = 31;
        rPANEL_4K2K30_COMMON.u2VClkMin= 23;
    }

    vPanelAttribute_Status_4k2k();
    }
}



static void vSaveRefAttribute_4K2K(void)
{
    if (IS_SUPPORT_4K2K_DISPLAY)
    {    
	LOG(0, " vSaveRefAttribute_4K2K\n");
#ifdef CC_SUPPORT_UHD
     if ( IS_DISP_4K2K_TYPE5 )
     {
		 // AUO_4K2K30
		   rREF_4K_TG[SV_DCLK_60HZ].u2FrameRate = 30;
		   rREF_4K_TG[SV_DCLK_60HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width >> 1;
		   rREF_4K_TG[SV_DCLK_60HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height;
		   rREF_4K_TG[SV_DCLK_60HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2HTotal >>1; 
		   rREF_4K_TG[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal;
		   rREF_4K_TG[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk;
		   rREF_4K_TG[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
		   rREF_4K_TG[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;
		 
		   rREF_4K_TG[SV_DCLK_50HZ].u2FrameRate = 25;
		   rREF_4K_TG[SV_DCLK_50HZ].u2Width = rPANEL_FHD120[SV_DCLK_50HZ].u2Width >>1;
		   rREF_4K_TG[SV_DCLK_50HZ].u2Height = rPANEL_FHD120[SV_DCLK_50HZ].u2Height;
		   rREF_4K_TG[SV_DCLK_50HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2HTotal>>1;
		   rREF_4K_TG[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal;
		   rREF_4K_TG[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk;
		   rREF_4K_TG[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth;
		   rREF_4K_TG[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp;
		 
		   if (SUPPORT_PANEL_48HZ)
		   {
			   rREF_4K_TG[SV_DCLK_48HZ].u2FrameRate = 24;
			   rREF_4K_TG[SV_DCLK_48HZ].u2Width = rPANEL_FHD120[SV_DCLK_48HZ].u2Width >> 1;
			   rREF_4K_TG[SV_DCLK_48HZ].u2Height = rPANEL_FHD120[SV_DCLK_48HZ].u2Height;
			   rREF_4K_TG[SV_DCLK_48HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2HTotal >>1; 
			   rREF_4K_TG[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal;
			   rREF_4K_TG[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk;
			   rREF_4K_TG[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth;
			   rREF_4K_TG[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp;
		   }
     
     }
     else
#endif
    {
      // AUO_4K2K30
        rREF_4K_TG[SV_DCLK_60HZ].u2FrameRate = 30;
        rREF_4K_TG[SV_DCLK_60HZ].u2Width = rPANEL_FHD120[SV_DCLK_60HZ].u2Width << 1;
        rREF_4K_TG[SV_DCLK_60HZ].u2Height = rPANEL_FHD120[SV_DCLK_60HZ].u2Height << 1;
        rREF_4K_TG[SV_DCLK_60HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2HTotal << 1;
        rREF_4K_TG[SV_DCLK_60HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_60HZ].u2VTotal << 1;
        rREF_4K_TG[SV_DCLK_60HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_60HZ].u4PixelClk;
        rREF_4K_TG[SV_DCLK_60HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncWidth;
        rREF_4K_TG[SV_DCLK_60HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_60HZ].u4VsyncBp;

        rREF_4K_TG[SV_DCLK_50HZ].u2FrameRate = 25;
        rREF_4K_TG[SV_DCLK_50HZ].u2Width = rPANEL_FHD120[SV_DCLK_50HZ].u2Width << 1;
        rREF_4K_TG[SV_DCLK_50HZ].u2Height = rPANEL_FHD120[SV_DCLK_50HZ].u2Height << 1;
        rREF_4K_TG[SV_DCLK_50HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2HTotal << 1;
        rREF_4K_TG[SV_DCLK_50HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_50HZ].u2VTotal << 1;
        rREF_4K_TG[SV_DCLK_50HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_50HZ].u4PixelClk;
        rREF_4K_TG[SV_DCLK_50HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncWidth;
        rREF_4K_TG[SV_DCLK_50HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_50HZ].u4VsyncBp;

        if (SUPPORT_PANEL_48HZ)
        {
	        rREF_4K_TG[SV_DCLK_48HZ].u2FrameRate = 24;
	        rREF_4K_TG[SV_DCLK_48HZ].u2Width = rPANEL_FHD120[SV_DCLK_48HZ].u2Width << 1;
	        rREF_4K_TG[SV_DCLK_48HZ].u2Height = rPANEL_FHD120[SV_DCLK_48HZ].u2Height << 1;
	        rREF_4K_TG[SV_DCLK_48HZ].u2HTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2HTotal << 1;
	        rREF_4K_TG[SV_DCLK_48HZ].u2VTotal = rPANEL_FHD120[SV_DCLK_48HZ].u2VTotal << 1;
	        rREF_4K_TG[SV_DCLK_48HZ].u4PixelClk = rPANEL_FHD120[SV_DCLK_48HZ].u4PixelClk;
	        rREF_4K_TG[SV_DCLK_48HZ].u4VsyncWidth = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncWidth;
	        rREF_4K_TG[SV_DCLK_48HZ].u4VsyncBp = rPANEL_FHD120[SV_DCLK_48HZ].u4VsyncBp;
        }
     }

    }
    //vPanelAttribute_Status_4k2k();
}

static void vFrcUnmute(void *pvArgs)
{
    while(TRUE)
    {
        VERIFY (x_sema_lock(_hFrcUnmuteSema, X_SEMA_OPTION_WAIT) == OSR_OK);
        vExt4k2kFrcMute(SV_OFF);
        Printf("\n[4k2k].....FRC Unmute\n");
        vDrvSetAllMute(SV_OFF);
    }
}

void Panel4K2KStatus(void)
{
    Printf("[4k2k] Panel4K2K Mode: %d\n", _u14K2KMode);
    Printf("[4k2k] Panel4K2K Type: %d\n", _4k2kPanelType);
}


void SetPanel_4k2k(UINT8 u1Mode)
{
    //UINT32 u4LastPNLType;
    //UINT8 u1Cnt=0;
    UINT32 u4CtlWord = 0;
    UINT32 u4CtlWord2 = 0;
    UINT32 i4panelType = E_4K2K_NORMAL;
    UINT32 u4OSDMode=OSD_4K2K_MODE_NONE;

    #ifndef NDEBUG
    HAL_TIME_T hStartMeasureTime;
    HAL_TIME_T hEndMeasureTime;
    HAL_TIME_T rDeltaTime;
    #endif

    if (_u14K2KMode == u1Mode)
    {
        Printf("[4k2k] skip Set Panel Mode %d\n", _u14K2KMode);
        return ;
    }

    VERIFY (x_sema_lock(_hFrcSetPanelSema, X_SEMA_OPTION_WAIT) == OSR_OK);

    #ifndef NDEBUG
    HAL_GetTime(&hStartMeasureTime);
    #endif

    switch (u1Mode)
    {
        case E_4K2K_SPLIT_PHOTO :
            u4OSDMode = OSD_4K2K_MODE_JPEG;
            i4panelType = PANEL_TYPE_2K1K_60;
            Printf("[4k2k] Photo mode\n");
        break;
        case E_4K2K_SPLIT_VDO :
            u4OSDMode = OSD_4K2K_MODE_MVC;
            i4panelType = IS_DISP_4K2K_TYPE1 ? PANEL_TYPE_2K1K_120 : PANEL_TYPE_2K1K_60;
            Printf("[4k2k] Video mode\n");
        break;
        case E_4K2K_NATIVE:
            u4OSDMode = OSD_4K2K_MODE_HDMI_4K2K;
            i4panelType = PANEL_TYPE_4K2K_30;
            Printf("[4k2k] Native mode\n");
            break;
        case E_4K2K_NORMAL_3D:
            u4OSDMode = OSD_4K2K_MODE_NONE;
            i4panelType = PANEL_TYPE_2K1K_120;
            Printf("[4k2k] 3D mode\n");
            break;
        case E_4K2K_NORMAL:
        default:
            u4OSDMode = OSD_4K2K_MODE_NONE;
            i4panelType = IS_DISP_4K2K_TYPE1 ? PANEL_TYPE_2K1K_120: PANEL_TYPE_2K1K_60;
            Printf("[4k2k] Normal mode\n");
        break;
    }

    vExt4k2kFrcMute(SV_ON);
    vDrvSetAllMute(SV_ON);
    vDrvLVDSOff();

    if (IS_DISP_4K2K_TYPE2)
    {
        u4CtlWord = PANEL_GetControlWord() & 0xFFFFFFE7;
    }
    u4CtlWord2 = PANEL_GetControlWord2() & 0xFFFFFDFF;  // clean linebuffer bit first

    vDrvSwitchVideoType(i4panelType);

    if (IS_DISP_4K2K_TYPE2)
    {
        PANEL_SetControlWord(u4CtlWord);
    }
    PANEL_SetControlWord2(u4CtlWord2);

    if (PANEL_GetDisplayType() == PANEL_DISPLAY_TYPE_VB1)
    {
        vDrvVB14K2KPanelInterfaceConfig();
    }
    else
    {
        vDrvLVDSConfig();
        vDrvLVDSInit();
    }

    vErrorHandleSetByPanel();

    vDrvSetPanelTiming();

    IGNORE_RET(OSD_UpdateTiming());
    IGNORE_RET(OSD_Base_SetHStart());

    if (PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_VB1)
    {
        vDrvVByOneOn();
    }
    else
    {
        vDrvLVDSOn();
    }

    Printf("---[4k2k][LVDS]panelType change to: %d\n", i4panelType);

    vSend4k2kFrcModeCmd(u1Mode);

    //Switch OSD mode
    OSD_PLA_SetLinuxUIMode(u4OSDMode,0,0,0,0);

    vSend4k2kFrcPanelTypeCmd(i4panelType);  // move to here

    if (psCur4k2kHandler && (psCur4k2kHandler->frc_snd_mute==NULL))
    {
        Printf("[4k2k]No FRC mute --> set all mute!\n");
        vDrvSetAllMute(SV_OFF);
    }

    #ifndef NDEBUG
    Printf("[4k2k]unmute timer start %d\n", u2Ext4k2kGetMuteTime());
    #endif
    vExt4k2kFrcUmMuteTimer(u2Ext4k2kGetMuteTime());

    //update current 4k2k mode
    _u14K2KMode = u1Mode;
    #ifndef NDEBUG
    HAL_GetTime(&hEndMeasureTime);
    HAL_GetDeltaTime(&rDeltaTime, &hStartMeasureTime, &hEndMeasureTime);
    Printf("[4K2K] set panel diff %d.%03d s\n", rDeltaTime.u4Seconds,  (rDeltaTime.u4Micros / 1000));
    #endif
    x_sema_unlock(_hFrcSetPanelSema);
}
#endif
#endif // CC_MTK_LOADER
LINT_RESTORE

