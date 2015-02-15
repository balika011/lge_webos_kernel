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
#include "drv_lcdim.h"
#include "drv_tcon.h"
#include "nptv_debug.h"
#include "api_backlt.h"
#ifndef CC_COPLAT_MT82
#include "osd_drvif.h"
#include "pmx_drvif.h"
#endif
#include "x_assert.h"
#include "x_lint.h"
#include "c_model.h"
#include "x_timer.h"
#include "ostg_if.h"
#ifdef THE_3RD_PARTY_HEADER_FILE_1
#include THE_3RD_PARTY_HEADER_FILE_1
#endif
#ifdef CC_DRV_SUPPORT_SMICRO
#include "SMicro/SMicro_if.h"
#endif
#include "x_gpio.h"
#ifdef CC_MT5395
#include "drv_mjc.h"
#include "drv_mjc_if.h"
#ifdef CC_SCPOS_3DTV_SUPPORT
#include "drv_tdtv_drvif.h"
#include "drv_mjc.h"
#endif
#endif
#include "drv_vdoclk.h"
#include "drv_scaler_drvif.h"
//#include "drvcust_if.h"  //mtk70763, 2010-7-21
#include "ldm_if.h"
#include "panel.h"
#ifdef DRV_SUPPORT_EXTMJC
#include "extmjc_if.h"
#endif

//luis@2007/7/5, for vRegWriteFldXXX
/*lint -save -e666*/

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#ifndef CC_MOSYNCLEN_MIN
#define CC_MOSYNCLEN_MIN (99 + 15)
#endif
#define CC_RIGHTMASK_MIN (10)
#define CC_MOSYNCLEN_MAX (2047)
#define CC_RIGHTMASK_MAX (30)


#define OSTG_UPDATE_PIXEL_SHIFT_H 1
#define OSTG_UPDATE_PIXEL_SHIFT_V 2

// force the HTotal to be 4x-aligned in Dispmode + Dual or Four port panel
#define CC_HTOTAL_4X_LIMIT_WA

// when VOUT_H is smaller than HTotal, VSync will be lost.
#define CC_OSTG_WA1

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

extern UINT32 _u4VPllFrequency;//for dynamic MDR
extern UINT8 bLdmPanelType;    //for SPI

//-----------------------------------------------------------------------------
// Public variables
//-----------------------------------------------------------------------------

//UINT32 dwSI_DISPLAY_DCLK_TYPICAL = 0x00;
//UINT32 dwSI_DISPLAY_DCLK_50HZ;
//#ifdef SUPPORT_PANEL_48HZ
//UINT32 dwSI_DISPLAY_DCLK_48HZ;
//#endif
UINT32 _u4OSTGDelta = 0;

//Give initial value instead of using "CC_DVBT_SUPPORT" define
UINT8 bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
UINT8 bOUTPUT_REFRESH_RATE = 0;

UINT8 bDISPLAY_INTERLACE_ENABLE;
BOOL _fgFRS_ENA;
enum eSyncLockMode _eSyncLockMode = eSYNCLOCK_IGNORED;

#define IMPORT_PROTECTION 1
#define IMPORT_PROTECTION_RECOVER 3
UINT8 _bIsImportProtection = IMPORT_PROTECTION;

UINT32 _u4ErrorLimit = 0x3f;

#if CC_SCPOS_VSYNC_WA1
//static UINT8 _u1TopMask = 0;
static UINT8 _u1BottomMask = 0;
static UINT8 _u1LeftMask = 0;
static UINT8 _u1RightMask = 0;
static UINT32 _u4VTotal = 0;
static UINT32 _u4OutHVChanged = 0;

static UINT32 _u4HTotal = 0;	//OSD falsh when HVToatl change
static UINT32 _u4HSyncLen = 0;//OSD falsh when HVToatl change
#endif

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


#if defined(CC_HVSYNC_ALIGN_WA) && defined(LOAD_PANEL_FROM_FLASH_CUSTOM)
extern UINT32 _u4HVAlignmentOffset;
#endif

#ifdef CC_MT5395
static UINT16 _u2IntFrameRate = 0;
static UINT16 _u2HtotalRatio = HTOTAL_RATIO_BASE;
static UINT16 _u2PClkRatio = 1;
extern UINT8 _bForceFrameRate;
#endif

static UINT8 _u1ScalerOutVTotalDiv = 1;

UINT8 bSupportLocalDimming = SV_FALSE;

UINT8 u1PoclkRatio=0;

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
#endif

#ifdef CC_SCPOS_3DTV_SUPPORT 
static void vDrvDisp3DModeCB(void);
static void vSavePanelAttribute(void);
#ifdef DRV_SUPPORT_MT8283
extern BOOL fgForceChangeRegion = FALSE;
#endif
#endif

#endif

// Output stage settings during output V-sync. 
static void vDrvOstgParaUpdate(void)
{
#if CC_SCPOS_VSYNC_WA1
    if (_u4OutHVChanged)
    {
        UINT32 u4OriginalV1;
        UINT32 u4DEPowerDown;

        u4OriginalV1 = RegReadFldAlign(SCPIP_PIP_OA_00, PIP_OA_00_MOVLEN);
        u4DEPowerDown = RegReadFldAlign(OUTSTG_OS_15, OS_15_DEN_EN);

        if (u4DEPowerDown == 0)
        {
            LVDS_SwitchDataEnable(FALSE);
        }
            
        if (u4OriginalV1 <= _u4VTotal)
        {
            vRegWriteFldAlign(OUTSTG_OS_27, _u1BottomMask, OS_27_BOTTOM_MASK);
            vRegWriteFldAlign(SCPIP_PIP_OA_00, _u4VTotal, PIP_OA_00_MOVLEN); 
        }
        else
        {
            vRegWriteFldAlign(SCPIP_PIP_OA_00, _u4VTotal, PIP_OA_00_MOVLEN); 
            vRegWriteFldAlign(OUTSTG_OS_27, _u1BottomMask, OS_27_BOTTOM_MASK);
        }
            /* update htotal & hsync len in blanking interval */
            vRegWriteFldAlign(SCPIP_PIP_OA_00, _u4HTotal, PIP_OA_00_MOHLEN);
			        		
			#if defined(CC_SCPOS_3DTV_SUPPORT) &&  !defined(CC_MTK_LOADER) && !defined(CC_MT5365)
			if (u1DrvMJCIsOnOff())
			{
				u2HSyncLen = _u4HSyncLen;
			}
			else
			{
            	vRegWriteFldAlign(SCPIP_PIP_OA_01, _u4HSyncLen, PIP_OA_01_MOSYNCLEN);
			}
			#else
            vRegWriteFldAlign(SCPIP_PIP_OA_01, _u4HSyncLen, PIP_OA_01_MOSYNCLEN);
			#endif

            /* update left/right mask */
            vRegWriteFldAlign(OUTSTG_OS_28, _u1LeftMask, OS_28_LEFT_MASK);
            vRegWriteFldAlign(OUTSTG_OS_28, _u1RightMask, OS_28_RIGHT_MASK); 
            _u4OutHVChanged = 0;
            
        if (u4DEPowerDown == 0)
        {
            LVDS_SwitchDataEnable(TRUE);
        }
    }
#endif
}


static void vDrvSetOSTGHPos(UINT32 u4Pos)
{
    UINT32 u4Ofst;
    UINT32 u4HTotal, u4HSyncWidth;
    UINT32 u4HBlanking;
    
    u4HTotal = wDrvGetOutputHTotal();
    u4HSyncWidth = RegReadFldAlign(OUTSTG_OS_17, OS_17_HS_WIDTH) + 1;
    u4HBlanking = u4HTotal - PANEL_GetPanelWidth();
        
    if (u4Pos < 0x800)
    {
        LOG(0, "Invalid H_POS: 0x%X ", u4Pos);
        u4Pos = 0x800;
        LOG(0, "--> 0x%X\n", u4Pos);
    }
    else if ((u4Pos+u4HSyncWidth) > (0x800+u4HBlanking))
    {
        LOG(0, "Invalid H_POS: 0x%X ", u4Pos);
        u4Pos = (0x800+u4HBlanking) -u4HSyncWidth;
        if (u4Pos < 0x800)
        {
            u4Pos = 0x800;
        }
        LOG(0, "--> 0x%X\n", u4Pos);
    }

    if (u4Pos == 0x800)
    {
        #ifdef CC_OSTG_WA1
        u4Pos = 0x802;
        u4Ofst = u4Pos - 0x800-1;
        #else	
        u4Ofst = u4HTotal - 1;
        #endif
    }
    else
    {
        u4Ofst = u4Pos - 0x800-1;
    }
    vRegWriteFldAlign(OUTSTG_OS_16, u4Pos, OS_16_H_POS);
    vRegWriteFldAlign(OUTSTG_OS_08, u4Ofst, OS_08_REG_VOUT_H);
    
}

static void vDrvSetOSTGVPos(UINT32 u4Pos)
{
    vRegWriteFldAlign(OUTSTG_OS_18, u4Pos, OS_18_V_POS);
}


/**
 * @brief vDrvSetDENAMask

 * Set the Top/Bottom/Left/Right Masking

 * @param  TOP: the value of TOP_MASK
 *         BOTTOM: the value of BOTTOM_MASK
 *         LEFT: the value of LEFT_MASK
 *         RIGHT: the value of RIGHT_MASK
 * @retval void
 */
#if (!CC_SCPOS_VSYNC_WA1) 
static void vDrvSetDENAMask(UINT16 TOP, UINT16 BOTTOM, UINT16 LEFT, UINT16 RIGHT)
{
    vRegWriteFldAlign(OUTSTG_OS_27, TOP, OS_27_TOP_MASK);
    vRegWriteFldAlign(OUTSTG_OS_27, BOTTOM, OS_27_BOTTOM_MASK);
    vRegWriteFldAlign(OUTSTG_OS_28, LEFT+_u4OSTGDelta, OS_28_LEFT_MASK);
    vRegWriteFldAlign(OUTSTG_OS_28, RIGHT, OS_28_RIGHT_MASK);
}
#endif


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

/**
 * @brief wDrvGetOutputHTotal

 * Get Output H_Total

 * @param  void
 * @retval Output H_Total
 */
UINT16 wDrvGetOutputHTotal(void)
{
    UINT16 wTemp;

    #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
    if (u1DrvMJCIsOnOff())  // MJC is on
    {
        wTemp = u2DrvMJCGetOutputHTotal();
    }
    else
    #endif
    #if CC_SCPOS_VSYNC_WA1
    if (_u4OutHVChanged)
    {
        wTemp = (UINT16)_u4HTotal + 1;
    }
    else
    #endif// CC_SCPOS_VSYNC_WA1
    {
        wTemp= RegReadFldAlign(SCPIP_PIP_OA_00, PIP_OA_00_MOHLEN) + 1;
    }
    return (wTemp);
}


/**
 * @brief wDrvGetOutputVTotal

 * Get Output V_Total

 * @param  void
 * @retval Output V_Total
 */
UINT16 wDrvGetOutputVTotal(void)
{
    UINT16 wTemp;

    #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
    if (u1DrvMJCIsOnOff())  // MJC is on
    {
        wTemp = u2DrvMJCGetOutputVTotal();
    }
    else
    #endif
    #if CC_SCPOS_VSYNC_WA1
    if (_u4OutHVChanged)
    {
        wTemp = (UINT16)_u4VTotal+1;
    }
    else
    #endif// CC_SCPOS_VSYNC_WA1
    {
        wTemp= RegReadFldAlign(SCPIP_PIP_OA_00, PIP_OA_00_MOVLEN)+1;
    }
    return (wTemp);
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


 #ifndef CC_MTK_LOADER

#ifdef SUPPORT_SW_FRAME_CHECK
UINT16 u4ScposGetInOutVDistance(void)
{
    return RegReadFldAlign(SCPIP_PIP1_RO_02, PIP1_RO_02_VCOUNTER_OUT_1);
}
#endif

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
        if ((u4VDistance >= (RegReadFldAlign(SCPIP_PIP_OA_00,  PIP_OA_00_MOVLEN) / 2))
                && (u4VDistance <= RegReadFldAlign(SCPIP_PIP_OA_00,  PIP_OA_00_MOVLEN)))
        {
            u4VDistance = RegReadFldAlign(SCPIP_PIP_OA_00,  PIP_OA_00_MOVLEN) - u4VDistance;
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
    else
    {
        DISPLAY_SIZE = DISPLAY_SIZE * bOUTPUT_REFRESH_RATE / bInputFrameRate; 
        LOG(3, "4DDDS %dHz --> %dHz\n", bInputFrameRate, bOUTPUT_REFRESH_RATE);
    }

    DISPLAY_SIZE = bDrvVideoIsSrcInterlace(SV_VP_MAIN) ? DISPLAY_SIZE * 2 : DISPLAY_SIZE;

    #if ENABLE_DDDS2_FEEDBACK
    if (LVDS_OUTPUT_PORT == SINGLE_PORT) u1VopllRatio=0;
    else  if (LVDS_OUTPUT_PORT == DUAL_PORT) u1VopllRatio=1;
    else u1VopllRatio=2;
    DISPLAY_SIZE = DISPLAY_SIZE >> u1VopllRatio;
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
    
    vIO32WriteFldAlign(DDDS_03,_u4ErrorLimit,DDDS_03_DDDS_ERR_LIM);
    vIO32WriteFldAlign(DDDS_02,(UINT16)u2InputVTotal,DDDS_02_HLEN_DEN);
    vIO32WriteFldAlign(DDDS_01,(UINT16)HLEN_NUM,DDDS_01_HLEN_NUM);    
    vIO32WriteFldAlign(DDDS_01,HLEN_INT,DDDS_01_HLEN_INT);

   // LOG(3, "HLEN=0x%x . [0x%x / 0x%x] \n",HLEN_INT, HLEN_NUM,u2InputVTotal);    
}
#endif // end of #ifndef CC_MTK_LOADER


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
    UINT8 u1Kp0, u1Ki0;
#endif

    if (bOn_Off == SV_ON)
    {
        vIO32WriteFldMulti(DDDS_00,P_Fld(1,DDDS_00_DISP_EN)|P_Fld(1,DDDS_00_FIX_FS_DDDS_SEL));
    }
    else
    {
        #ifdef SUPPORT_SW_FRAME_CHECK
            _u4UpTrackRange = 0;
            _u4DownTrackRange = 0;
            _u4VDistTrackState = 0;
            _u4VDistTrackLocked = 0;
        #endif

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        u4TmpCw = IO32ReadFldAlign(STA_DDDS_00, STA_DDDS_00_DDDS_FREQ_CW);
        u1Ki0 = IO32ReadFldAlign(DDDS_02, DDDS_02_DDDS_KI_0);
        u1Kp0 = IO32ReadFldAlign(DDDS_02, DDDS_02_DDDS_KP_0);
        vIO32WriteFldMulti(DDDS_02, P_Fld(0,DDDS_02_DDDS_KI_0)|P_Fld(0,DDDS_02_DDDS_KP_0));
        vIO32WriteFldAlign(DDDS_00, u4TmpCw, DDDS_00_DDDS_FREQ_CW);
#endif

        vIO32WriteFldMulti(DDDS_00,P_Fld(0,DDDS_00_DISP_EN)|P_Fld(0,DDDS_00_FIX_FS_DDDS_SEL));

        vIO32WriteFldAlign(DDDS_03, 1, DDDS_03_SPREAD_INIT);
        #ifdef CC_COPLAT_MT82
        vUtDelay2us(50);
        #else
        HAL_Delay_us(100);
        #endif
        vIO32WriteFldAlign(DDDS_03, 0, DDDS_03_SPREAD_INIT);   

#ifdef SUPPORT_FREERUN_DCLK_BY_STEP
        vIO32WriteFldMulti(DDDS_02, P_Fld(u1Ki0,DDDS_02_DDDS_KI_0)|P_Fld(u1Kp0,DDDS_02_DDDS_KP_0));
#endif
    }
    #endif //end of #ifndef CC_MTK_LOADER 
}


void vDrvUpdateHPosition(void)
{
    if ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ) || 
        ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_OTHERS) && (bOUTPUT_REFRESH_RATE >= (55 * (PANEL_IsSupport120Hz()? 2: 1)))))
    {
    #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(TRUE);
        vDrvSetOSTGHPos(H_POS_60HZ);
        _OSD_AlwaysUpdateReg(FALSE);
    #else
        vDrvSetOSTGHPos(H_POS_60HZ);
    #endif
    }
    #ifdef SUPPORT_PANEL_48HZ
    else if(bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ)
    {
    	#ifdef SUPPORT_PANEL_CUSTOMER_SPEC
       vDrvSetOSTGHPos(PANEL_GetHPositionByTiming(SV_DCLK_48HZ));
	#else
       vDrvSetOSTGHPos(H_POS_60HZ);
    	#endif
    }
    #endif    
    else
    {
        #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(TRUE);
        vDrvSetOSTGHPos(H_POS_50HZ);
        _OSD_AlwaysUpdateReg(FALSE);
       #else
        vDrvSetOSTGHPos(H_POS_50HZ);
        #endif
    }
}


// LVDS power------------------------------------------------------------------
void vDrvTurnOnPanel()
{

#ifndef DRV_SUPPORT_MT8283 // for turnkey, do control panel power gpio (8283 power gpio)
    do 
    { 
        UINT32 gpio, value; 
        if(DRVCUST_PanelQuery(ePanelPowerGpio, &gpio) == 0 && 
           DRVCUST_PanelQuery(ePanelPowerOnVal, &value) == 0)
        {
            vDrvGpioSet((INT32)gpio, (INT32)value);
        }
    } while (0);
#endif
}

void vDrvTurnOffPanel()
{
#ifndef DRV_SUPPORT_MT8283 // for turnkey, do control panel power gpio (8283 power gpio)
    do 
    { 
        UINT32 gpio, value; 
        if(DRVCUST_PanelQuery(ePanelPowerGpio, &gpio) == 0 && 
           DRVCUST_PanelQuery(ePanelPowerOnVal, &value) == 0)
        {
            vDrvGpioSet((INT32)gpio, (INT32)!value);
        }
    } while (0);
 #endif
}
//-----------------------------------------------------------------------------
/** turn on/off panel lvds signal. (used by MTAL)
 *  @param fgEnable: Control mode. It could be TRUE or FALSE
 *  @return
 */
//-----------------------------------------------------------------------------

void vDrvLVDSSignalOnOff(BOOL fgOnOff)
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

/**
 * @brief vDrvSetLCDTiming

 * Set the Display Output Timing for LCD Panel

 * @param  void
 * @retval void
 */
void vDrvSetLCDTiming(void)
{
    UINT32 u4SyncLen;
    UINT32 u4LeftMask;
    UINT32 u4HTotal;
  //  vDrvSwitchMTKGoodDclk(SV_OFF);

    if( _bIsImportProtection )
    {
        vDrvSwitchImportProtection(1);
    }

    {
        UINT32 u4VsWidth, u4HsWidth, u4VPos;

        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        u4VsWidth = PANEL_GetVSyncWidthByTiming(bSI_DISPLAY_DCLK_TYPE);
        u4HsWidth = PANEL_GetHSyncWidthByTiming(bSI_DISPLAY_DCLK_TYPE);
        u4VPos = PANEL_GetVPositionByTiming(bSI_DISPLAY_DCLK_TYPE);
        #else
        u4VsWidth = VS_WIDTH;
        u4HsWidth = HS_WIDTH;
        u4VPos = V_POS;
        #endif
        if (u4VsWidth > 0)
        {
            u4VsWidth = u4VsWidth - 1;
        }
        if (u4HsWidth > 0)
        {
            u4HsWidth = u4HsWidth - 1;
        }        
        vRegWriteFldAlign(OUTSTG_OS_18, u4VsWidth, OS_18_VS_WIDTH);
        vRegWriteFldAlign(OUTSTG_OS_17, u4HsWidth, OS_17_HS_WIDTH);
        vDrvSetOSTGVPos(u4VPos);        
    }

    if ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ) || 
        ((bSI_DISPLAY_DCLK_TYPE == SV_DCLK_OTHERS) && ((bOUTPUT_REFRESH_RATE >= (55*(PANEL_IsSupport120Hz()? 2: 1))) || bOUTPUT_REFRESH_RATE == 24 || bOUTPUT_REFRESH_RATE == 30) ))
    {
        //Set display output timing
        #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(TRUE);
        #endif
        u4SyncLen = HSYNCLEN_TYPI;
        u4LeftMask = LEFT_MASK_TYPI;
        u4HTotal = CONVERT_TO_INT_HTOTAL(HLEN_TOTAL_TYPI+1);
        if (u4HTotal > (HLEN_TOTAL_TYPI+1))
        {
            u4SyncLen += (u4HTotal - (HLEN_TOTAL_TYPI+1));
        }
                
        #if CC_SCPOS_VSYNC_WA1
        _u4HSyncLen = u4SyncLen;
        _u4HTotal = u4HTotal-1;
        _u1LeftMask = u4LeftMask; 
        _u1RightMask = RIGHT_MASK_TYPI;
        _u1BottomMask = BOTTOM_MASK_TYPI;
        vRegWriteFldAlign(OUTSTG_OS_27, TOP_MASK_TYPI, OS_27_TOP_MASK);
        #else
		#if defined(CC_SCPOS_3DTV_SUPPORT) &&  !defined(CC_MTK_LOADER) && !defined(CC_MT5365)
		if (u1DrvMJCIsOnOff())
		{
			u2HSyncLen = u4SyncLen-_u4OSTGDelta;
		}
		else
		{
        	vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		}
		#else
        vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		#endif
        vRegWriteFldAlign(SCPIP_PIP_OA_00, (u4HTotal-1), PIP_OA_00_MOHLEN);
        vDrvSetDENAMask(TOP_MASK_TYPI, BOTTOM_MASK_TYPI, u4LeftMask, RIGHT_MASK_TYPI);
        #endif

        vDrvSetOSTGHPos(H_POS_60HZ);
        #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(FALSE);
        #endif
        #if CC_SCPOS_VSYNC_WA1
        _u4VTotal = VLEN_TOTAL_TYPI;
        _u4OutHVChanged = 1;
        #else
        vRegWriteFldAlign(SCPIP_PIP_OA_00, VLEN_TOTAL_TYPI / _u1ScalerOutVTotalDiv, PIP_OA_00_MOVLEN);
        #endif

        #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
        //vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_TYPI+1, VLEN_TOTAL_TYPI+1, 0, 0);
        vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_TYPI+1, (VLEN_TOTAL_TYPI / _u1ScalerOutVTotalDiv+1)*_u1ScalerOutVTotalDiv, 0, 0);
        #endif

    }
    #ifdef SUPPORT_PANEL_48HZ
    else if (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_48HZ)
    {
        //Set display output timing
        _OSD_AlwaysUpdateReg(TRUE);

        u4SyncLen = HSYNCLEN_48HZ;
        u4LeftMask = LEFT_MASK_48HZ;
        u4HTotal = CONVERT_TO_INT_HTOTAL(HLEN_TOTAL_48HZ+1);
        if (u4HTotal > (HLEN_TOTAL_48HZ+1))
        {
            u4SyncLen += (u4HTotal - (HLEN_TOTAL_48HZ+1));
        }
        
        #if (CC_SCPOS_VSYNC_WA1)
        _u4HSyncLen = u4SyncLen;
        _u4HTotal = u4HTotal-1;
        _u1LeftMask = u4LeftMask; 
        _u1RightMask = RIGHT_MASK_48HZ;
        _u1BottomMask = BOTTOM_MASK_48HZ;
        vRegWriteFldAlign(OUTSTG_OS_27, TOP_MASK_48HZ, OS_27_TOP_MASK);
        #else        
		#if defined(CC_SCPOS_3DTV_SUPPORT) &&  !defined(CC_MTK_LOADER) && !defined(CC_MT5365)
		if (u1DrvMJCIsOnOff())
		{
			u2HSyncLen = u4SyncLen-_u4OSTGDelta;
		}
		else
		{
        	vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		}
		#else
        vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		#endif
        vRegWriteFldAlign(SCPIP_PIP_OA_00, u4HTotal-1, PIP_OA_00_MOHLEN);
        vDrvSetDENAMask(TOP_MASK_48HZ, BOTTOM_MASK_48HZ,(UINT16)u4LeftMask, RIGHT_MASK_48HZ);
        #endif
    
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        vDrvSetOSTGHPos(PANEL_GetHPositionByTiming(SV_DCLK_48HZ));
        #else
        vDrvSetOSTGHPos(H_POS_60HZ);
        #endif
        _OSD_AlwaysUpdateReg(FALSE);
    
        #if CC_SCPOS_VSYNC_WA1        
        _u4VTotal = VLEN_TOTAL_48HZ;
        _u4OutHVChanged = 1;
        #else
        vRegWriteFldAlign(SCPIP_PIP_OA_00, VLEN_TOTAL_48HZ / _u1ScalerOutVTotalDiv, PIP_OA_00_MOVLEN);
        #endif

        #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
        //vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_48HZ+1, VLEN_TOTAL_48HZ+1, 0, 0);
        vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_48HZ+1, (VLEN_TOTAL_48HZ / _u1ScalerOutVTotalDiv+1)*_u1ScalerOutVTotalDiv, 0, 0);
        #endif
    
    }
    #endif //SUPPORT_PANEL_48HZ
    else
    {
        //Set display output timing
        #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(TRUE);
        #endif
        
        u4SyncLen = HSYNCLEN_50HZ;
        u4LeftMask = LEFT_MASK_50HZ;
        u4HTotal = CONVERT_TO_INT_HTOTAL(HLEN_TOTAL_50HZ+1);
        if (u4HTotal > (LEFT_MASK_50HZ+1))
        {
            u4SyncLen += (u4HTotal - (HLEN_TOTAL_50HZ+1));
        }

        #if CC_SCPOS_VSYNC_WA1
        _u4HSyncLen = u4SyncLen;
        _u4HTotal = u4HTotal-1;
        _u1LeftMask = u4LeftMask; 
        _u1RightMask = RIGHT_MASK_50HZ;
        _u1BottomMask = BOTTOM_MASK_50HZ;
        vRegWriteFldAlign(OUTSTG_OS_27, TOP_MASK_50HZ, OS_27_TOP_MASK);
        #else
		#if defined(CC_SCPOS_3DTV_SUPPORT) &&  !defined(CC_MTK_LOADER) && !defined(CC_MT5365)
		if (u1DrvMJCIsOnOff())
		{
			u2HSyncLen = u4SyncLen-_u4OSTGDelta;
		}
		else
		{
        	vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		}
		#else
        vRegWriteFldAlign(SCPIP_PIP_OA_01, u4SyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
		#endif
        vRegWriteFldAlign(SCPIP_PIP_OA_00, u4HTotal-1, PIP_OA_00_MOHLEN);
        vDrvSetDENAMask(TOP_MASK_50HZ, BOTTOM_MASK_50HZ, u4LeftMask, RIGHT_MASK_50HZ);
        #endif
        
        
        if (H_POS_50HZ > 0)
        {
            vDrvSetOSTGHPos(H_POS_50HZ);
        }
        else
        {
            vDrvSetOSTGHPos(H_POS_60HZ);
        }

        #ifndef CC_COPLAT_MT82
        _OSD_AlwaysUpdateReg(FALSE);
        #endif
        #if CC_SCPOS_VSYNC_WA1
        _u4VTotal = VLEN_TOTAL_50HZ;
        _u4OutHVChanged = 1;
        #else
        vRegWriteFldAlign(SCPIP_PIP_OA_00, VLEN_TOTAL_50HZ / _u1ScalerOutVTotalDiv, PIP_OA_00_MOVLEN);
        #endif

        #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
        //vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_50HZ+1, VLEN_TOTAL_50HZ+1, 0, 0);
        vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, HLEN_TOTAL_50HZ+1, (VLEN_TOTAL_50HZ / _u1ScalerOutVTotalDiv+1)*_u1ScalerOutVTotalDiv, 0, 0);
        #endif
        
    }


    #ifdef CC_MTK_LOADER 
    #if CC_SCPOS_VSYNC_WA1
    OSTG_OnOutputVSync();
    #endif
    #endif

    if( _bIsImportProtection )
    {
	    vDrvFireImportPortection();
           _bIsImportProtection = IMPORT_PROTECTION_RECOVER;
    }

    vErrorHandleSetByTiming();

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

    u4HTotalMinByPixelClock = (((UINT32) DCLK_MIN/u2DrvGetInternalPCLKRatio()) / u4RefreshRate) / (UINT32) wOutputVTotal;
    u4HTotalMinByPixelClock = ((u4HTotalMinByPixelClock + 1) & 0xfffffffe);     //make as even value
    u4HTotalMaxByPixelClock = (((UINT32) DCLK_MAX/u2DrvGetInternalPCLKRatio()) / u4RefreshRate) / (UINT32) wOutputVTotal;
    u4HTotalMaxByPixelClock = ((u4HTotalMaxByPixelClock - 1) & 0xfffffffe);     //make as even value
    u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_MOSYNCLEN_MIN + CC_RIGHTMASK_MIN);
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
    // 5) Bigger than H-Total MIN calculate by CC_MOSYNCLEN_MIN
    // 6) Close to HLEN_TOTAL_TYPI

#ifdef CC_HTOTAL_4X_LIMIT_WA
    if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//    if (LVDS_OUTPUT_PORT == DUAL_PORT)
    {
        u4HTotal = (UINT32) ((HLEN_TOTAL_TYPI + 1) + 3) & (~3);
    }
    else
#endif
    {
        u4HTotal = (UINT32) (HLEN_TOTAL_TYPI + 1);
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
void vDrvOutputStageInit(void)
{
    UINT32 u4PanelDither = 0xffffffff;

    #ifdef CC_HVSYNC_ALIGN_WA
    #ifdef LOAD_PANEL_FROM_FLASH_CUSTOM
    _u4OSTGDelta = _u4HVAlignmentOffset;
    #else
    if (((GetCurrentPanelIndex() == PANEL_SONY_X_GY_1_FHD) ||(GetCurrentPanelIndex() == PANEL_SONY_X_GY_5_FHD) ||(GetCurrentPanelIndex() == PANEL_SONY_X_GY_6_FHD))
        || ((DRVCUST_PanelGet(ePanelFlashTableSize) > 0) && ((PANEL_GetEepromPanelIndex() == PANEL_SONY_X_GY_1_FHD) || (PANEL_GetEepromPanelIndex() == PANEL_SONY_X_GY_5_FHD) || (PANEL_GetEepromPanelIndex() == PANEL_SONY_X_GY_6_FHD))))
    {
        _u4OSTGDelta = 127;
    }
    else
    {
        _u4OSTGDelta = 0;
    }
    #endif   //LOAD_PANEL_FROM_FLASH_CUSTOM
    #else  // CC_HVSYNC_ALIGN
    _u4OSTGDelta = 0;
    #endif  // CC_HVSYNC_ALIGN

    //dwSI_DISPLAY_DCLK_TYPICAL = PANEL_GetPixelClk60Hz();
    //dwSI_DISPLAY_DCLK_50HZ = PANEL_GetPixelClk50Hz();
//    #ifdef SUPPORT_PANEL_48HZ
//    dwSI_DISPLAY_DCLK_48HZ = PANEL_GetPixelClk48Hz();
//    #endif

    #if (SRGB_FUNCTION_EN == SRGB_EN_ON)
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_DEN_EN);
    }
    #else
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_DEN_EN);
    }
    #endif

    #if (DEN_TYPE == DEN_ACIVE)
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_DENTPY);
    }
    #else
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_DENTPY);
    }
    #endif

    #if (DEN_POLARITY == DEN_LOW)
    {
        vIO32WriteFldAlign(MLVDS_P0, 1, RG_LVDS_DE_P);                
    }
    #else
    {
        vIO32WriteFldAlign(MLVDS_P0, 0, RG_LVDS_DE_P);                
    }
    #endif

    #if (FIELD_POLARITY == FIELD_LOW)
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_FIELDP);
    }
    #else
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_FIELDP);
    }
    #endif

    vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_HSYNCP);
    vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_VSYNCP);
    vIO32WriteFldAlign(MLVDS_P0, 0, RG_HSYNC_IN_P);                
    vIO32WriteFldAlign(MLVDS_P0, 0, RG_VSYNC_IN_P);                    

    //Panel Dither Configuration
    u4PanelDither = PANEL_GetDither();
    UINT32 u4Round, u4RDither, u4EDither, u4LFSR;

    if (u4PanelDither != 0xffffffff)
    {
        u4Round = (u4PanelDither & ROUND_FUNCTION)? 1: 0;
        u4RDither = (u4PanelDither & RDITHER_FUNCTION)? 1 : 0;
        u4EDither = (u4PanelDither & ERROR_DITHER_FUNCTION)? 1 : 0;
        u4LFSR = (u4PanelDither & LFSR_FUNCTION)? 1 : 0;
    }
    else
    {
        u4Round = 0;
        u4RDither = 1;
        u4EDither = 0;
        u4LFSR = 0;        
    }

    //Initialize TCON Dither
    vIO32WriteFldAlign(TD_POST_00, 0x77,TD_SYNC_P);
    vIO32Write4B(TD_POST_10, 0x810DE283);   
    //Initialize OSTG Dither

#ifndef CC_MTK_LOADER

    UINT32 u4DitherMode;
    
    if(u4Round)
    {
        u4DitherMode=ROUNDING;
    }
    else if(u4RDither)
    {
        u4DitherMode=R_DITHER;
    }
    else if(u4EDither)
    {
        u4DitherMode=E_DITHER;
    }
    else if(u4LFSR)
    {
        u4DitherMode=LFSR_DITHER;
    }
    else
    {
        u4DitherMode=DITHER_OFF;
    }
    
    vDrvDitherInit((UINT8)u4DitherMode);                 
#endif

    if(IS_OD_ENABLE)
    {
        //OSTG Dither
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_ROUND_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_RDITHER_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_EDITHER_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_DITHER_LFSR_EN);            
        //TCON Dither
        vIO32WriteFld(TD_POST_10, 2,TD_DRMOD);               
        vIO32WriteFld(TD_POST_10, u4Round,TD_REG_ROUND_EN);               
        vIO32WriteFld(TD_POST_10, u4RDither,TD_REG_RDITHER_EN);               
        vIO32WriteFld(TD_POST_10, u4EDither,TD_REG_EDITHER_EN);
        vIO32WriteFld(TD_POST_10, u4LFSR,TD_REG_LFSR_EN);               
        
    }
    else
    {        
        //OSTG Dither        
        vRegWriteFldAlign(OUTSTG_OS_15, u4Round, OS_15_ROUND_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, u4RDither, OS_15_RDITHER_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, u4EDither, OS_15_EDITHER_EN);
        vRegWriteFldAlign(OUTSTG_OS_15, u4LFSR, OS_15_DITHER_LFSR_EN);            
        //TCON Dither
        vIO32WriteFld(TD_POST_10, 0,TD_REG_ROUND_EN);               
        vIO32WriteFld(TD_POST_10, 1,TD_REG_RDITHER_EN);               
        vIO32WriteFld(TD_POST_10, 0,TD_REG_EDITHER_EN);
        vIO32WriteFld(TD_POST_10, 0,TD_REG_LFSR_EN);               
    }

            
    if ((DISP_BIT == DISP_30BIT)
        #ifdef DRV_SUPPORT_EXTMJC
        || (IS_COMPANION_CHIP_ON() && ((vDrvGetCurLvdsMode() <=  eLvdsVidRgb444TwoPort)))
        #endif
        )
    {       

        //OSTG Dither
        vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_DRMOD);
        //TCON Dither
        vIO32WriteFld(TD_POST_10, 1,TD_DRMOD);               
        
#ifdef CC_MT5395
        vRegWriteFldAlign(OUTSTG_OS_F7, 0x2, OS_F7_REG_FPHASE_CTRL);
        vRegWriteFldAlign(OUTSTG_OS_F7, 0x2, OS_F7_REG_TABLE_EN);
#endif
    }
    else
    {       
        if(IS_OD_ENABLE)
        {
            //OSTG Dither
            vRegWriteFldAlign(OUTSTG_OS_15, 1, OS_15_DRMOD);
            //TCON Dither
            vIO32WriteFld(TD_POST_10, 2,TD_DRMOD);                           
        }
        else
        {        
            //OSTG Dither
            vRegWriteFldAlign(OUTSTG_OS_15, 2, OS_15_DRMOD);
            //TCON Dither
            vIO32WriteFld(TD_POST_10, 1,TD_DRMOD);               
        }
       
#ifdef CC_MT5395
        vRegWriteFldAlign(OUTSTG_OS_F7, 0x1, OS_F7_REG_FPHASE_CTRL);
        vRegWriteFldAlign(OUTSTG_OS_F7, 0x1, OS_F7_REG_TABLE_EN);
#endif

    }

    
    if (u4PanelDither != 0xffffffff)
    {
        vRegWriteFldAlign(OUTSTG_OS_15, FPHASE_VALUE, OS_15_FPHASE);
    }
    else
    {
        vRegWriteFldAlign(OUTSTG_OS_15, 0, OS_15_FPHASE);
    }

    #if (RB_CHANNEL_INVERSE == RB_CHANNEL_INVERSE_ON)
    {
        vRegWriteFldAlign(OUTSTG_OS_17, 1, OS_17_RB_INV);
    }
    #else
    {
        vRegWriteFldAlign(OUTSTG_OS_17, 0, OS_17_RB_INV);
    }
    #endif

    #if (BIT_ORDER_INVERSE == BIT_ORDER_INVERSE_ON)
    {
        vRegWriteFldAlign(OUTSTG_OS_17, 1, OS_17_MLSB_INV);
    }
    #else
    {
        vRegWriteFldAlign(OUTSTG_OS_17, 0, OS_17_MLSB_INV);
    }
    #endif

    // Config HV alignment
    vRegWriteFldAlign(OUTSTG_OS_08, 3, OS_08_REG_VSYNC_OUT_SEL);

    if ((DISP_BIT == DISP_30BIT)
        #ifdef DRV_SUPPORT_EXTMJC
        || (IS_COMPANION_CHIP_ON() && (vDrvGetCurLvdsMode() !=  eLvdsVidNormal))
        #endif
        )
    {
        vIO32WriteFldAlign(MLVDS_SO0, 1, MLVDS_SO0_RG_10B_EN);                
    }
    else
    {
        vIO32WriteFldAlign(MLVDS_SO0, 0, MLVDS_SO0_RG_10B_EN);                
    }

#ifdef CC_SCPOS_3DTV_SUPPORT
    vIO32WriteFldAlign(MLVDS_SO0, 0x1, MLVDS_SO0_RG_RES_FLD);
    vRegWriteFldAlign(OUTSTG_OS_FF, 0x1,Fld(2, 28, AC_MSKB3));
#endif

    // local dimming initialization
    #ifdef CC_MTK_LOADER
    // fixme
    vDrvLDMHWInit(SPI_PINMUX_OPTION1,LDM_PANEL_AMS);   //initial SPI when loader boot up
    #else
    if(PANEL_IsSupportLocalDimming())
    {
        vDrvLcDimPanelInit();
        vDrvLcDinQualityInit();
		
		bLdmPanelType = PANEL_GetLDMType();
		LOG(0,"LDM panel type =%d\n",bLdmPanelType);
		vDrvLDMHWInit(SPI_PINMUX_OPTION1,bLdmPanelType);   //initial SPI
		
		bSupportLocalDimming = SV_TRUE;
    }    
    else
    {
    	bSupportLocalDimming = SV_FALSE;
    }
    #endif

    // scanning pwm initialization
    vRegWriteFldAlign(OUTSTG_OS_D9, wDrvGetOutputHTotal()*wDrvGetOutputVTotal()-1-1, OS_D9_REG_PWM_SCAN1_HIGH);
    vRegWriteFldAlign(OUTSTG_OS_DA, 0, OS_DA_REG_PWM_SCAN1_LOW);

    vRegWriteFldAlign(OUTSTG_OS_DC, wDrvGetOutputHTotal()*wDrvGetOutputVTotal()-1-1, OS_DC_REG_PWM_SCAN2_HIGH);
    vRegWriteFldAlign(OUTSTG_OS_DD, 0, OS_DD_REG_PWM_SCAN2_LOW);


#ifndef CC_MTK_LOADER
#ifdef CC_SCPOS_3DTV_SUPPORT 
    vSavePanelAttribute();
    fpDrvRegisterTDTVModeChangeCB(E_TDTV_CB_OSTG, (vTDTVModeChangeCB) vDrvDisp3DModeCB);
#endif
#endif
}

/**
 * @brief vDrvDisplayInit

 * Init the Display Settings

 * @param  void
 * @retval void
 */
void vDrvDisplayInit(void)
{
    vDDDSInit();

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
        if(!bIsPanelOn())
        {
            if (PANEL_IsSupport120Hz())
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
            if (PANEL_IsSupport120Hz())
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
            #ifdef CC_MT5395
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

    vDrvOutputStageInit();

    //SET main/pip mute works immediately
    vRegWriteFldAlign(OUTSTG_OS_1A, 1, OS_1A_REG_MUTE_SEL);

    // disable video plane
    vRegWriteFldAlign(SCPIP_PIP1_02, 0, PIP1_02_RVL_1);
    vRegWriteFldAlign(SCPIP_PIP2_02, 0, PIP2_02_RVL_2);

#ifdef CC_SCPOS_3DTV_SUPPORT 
    if ((LVDS_DISP_3D == LVDS_DISP_3D_SHUTTER) && (LVDS_DISP_SPLIT == LVDS_DISP_SPLIT_ON))
    {
    	vRegWriteFldAlign(OUTSTG_OS_EC, 1, OS_EC_REORDER_EN);
    }
#endif
    _u1ScalerOutVTotalDiv = 1;
}

void vDrvLCDSetFrame(UINT16 wFrameRate)
{
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
    else if (wFrameRate == 48)
    {
        vDrvLCDFreqSet(SV_DCLK_48HZ, wFrameRate);
    }
#endif
    else
    {
        vDrvLCDFreqSet(SV_DCLK_OTHERS, (UINT8)wFrameRate);
    }
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
    #ifdef CC_MT5395
    UINT32 u2IntFrameRate, u2HtotalRatio, u2PClkRatio;
    static UINT32 _u4InitInternalFrameRate = 0;
    UINT8 u1VTotalDiv = 1;
    
    if (u1DrvMJCIsOnOff() == SV_OFF)  // MJC is off
    {
        u2IntFrameRate = bOUTPUT_REFRESH_RATE;
        u2HtotalRatio = HTOTAL_RATIO_BASE;
        u2PClkRatio = 1;
        _u4InitInternalFrameRate = 0;
        _u1ScalerOutVTotalDiv = 1;                    
    }
    else
    {
        UINT16 u2InFrameRate;
        #ifdef CC_SCPOS_3DTV_SUPPORT
        UINT32 u4TDTVQueryResult;
        E_TDTV_DECODER_INPUT eInType;
        E_TDTV_DECODER_OUTPUT eOutType;   
        
        u4TDTVQueryResult = u4DrvTDTVScalerModeQuery((E_TDTV_DECODER_INPUT)E_TDTV_DECODER_INPUT_2D_P);
        eInType = (E_TDTV_DECODER_INPUT)(u4TDTVQueryResult & 0xff);
        eOutType = (E_TDTV_DECODER_OUTPUT)((u4TDTVQueryResult>>16) & 0xff);
        
        if(((eInType != E_TDTV_DECODER_INPUT_2D_P)||(eOutType == E_TDTV_DECODER_OUTPUT_3D_REPEAT)) 
            && (BSP_GetIcVersion() != IC_VER_5395_AA) 
            && (eOutType != E_TDTV_DECODER_OUTPUT_3D_2_2D)
            && (IS_LVDS_DISP_3D_POLARIZED)&&(bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE))
        {           
            u1VTotalDiv = 2; // set scaler output v total to 1/2 for PR panel
        }
        else
        {
            u1VTotalDiv = 1;            
        }
        if (bDrvVideoSignalStatus(SV_VP_MAIN) == SV_VDO_STABLE)
        {
            u2InFrameRate = (UINT16)(( _bForceFrameRate != 0 )? _bForceFrameRate : u4ScpipGet3DOneEyeFrameRate());       
        }
        else 
        {
            u2InFrameRate = _u2IntFrameRate;
        }      
        #else        
        u2InFrameRate = (UINT16)(( _bForceFrameRate != 0 )? _bForceFrameRate : bDrvVideoGetRefreshRate(SV_VP_MAIN));
        #endif
        
        if ((u2InFrameRate == 0) && (_u4InitInternalFrameRate))
        {
            return SV_FALSE;
        }

        if ((u2InFrameRate == 0)
            #ifdef CC_SCPOS_3DTV_SUPPORT
            || (eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V1)
            || (eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V2)
            #endif            
            )
        {
            u2PClkRatio = 1;
        }
        else
        {
            u2PClkRatio = (u2InFrameRate <= 30)? 2: 1;
            #ifdef CC_SCPOS_3DTV_SUPPORT
            // 60Hz panel and ((input is 3d) or (output is 3d repeat))
            if(!PANEL_IsSupport120Hz() &&(!IS_LVDS_DISP_3D_POLARIZED)&& 
                (((eInType != E_TDTV_DECODER_INPUT_2D_P)&&(eOutType != E_TDTV_DECODER_OUTPUT_3D_2_2D)) // input is 3d
                    ||(eOutType == E_TDTV_DECODER_OUTPUT_3D_REPEAT))) 
            {
                u2PClkRatio = 1;                
            }
            #endif            
            if (((DRVCUST_PanelGet(ePanel242530pOutput) != 0) && 
                 (u2InFrameRate==24||u2InFrameRate==25||u2InFrameRate==30))
            #ifdef CC_SCPOS_3DTV_SUPPORT
                || u4DrvGetDisp3DModeDclk() != 0
            #endif            
               )
            {
                u2PClkRatio = 1;                
            }
        }

        if (PANEL_IsSupport120Hz())
        {
            #ifdef CC_SCPOS_3DTV_SUPPORT
            if(((eInType == E_TDTV_DECODER_INPUT_2D_P)&&(eOutType != E_TDTV_DECODER_OUTPUT_3D_REPEAT)) // input is 2d & not repeat
                ||(eOutType == E_TDTV_DECODER_OUTPUT_3D_2_2D)                                          // or 3d to 2d by scaler
                ||((u1VTotalDiv ==2)&&!((eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V1)||(eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V2))))
            #endif
            {
                if (u2InFrameRate < 77)
                {
                    u2PClkRatio = u2PClkRatio * 2;
                }
                else if ((u2InFrameRate >= 77) && (u2InFrameRate < 110) && (bSI_DISPLAY_DCLK_TYPE == SV_DCLK_60HZ))
                {
                    u2PClkRatio = u2PClkRatio * 2;
                }
            }
        }

        if ((u2InFrameRate <= 30) && (u2InFrameRate > 0) // 24, 25, 30
            #ifdef CC_SCPOS_3DTV_SUPPORT
            &&!((eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V1)||(eOutType == E_TDTV_DECODER_OUTPUT_3D_FPR_V2))
            #endif
            )     
        {
            u2IntFrameRate = u2InFrameRate;
            #ifdef CC_SCPOS_3DTV_SUPPORT
            //((input is 3d) or (output is 3d repeat))
            if(((eInType != E_TDTV_DECODER_INPUT_2D_P)&&(eOutType != E_TDTV_DECODER_OUTPUT_3D_2_2D)) // input is 3d
                ||(eOutType == E_TDTV_DECODER_OUTPUT_3D_REPEAT))                                    // or output is repeat
            {
                u2IntFrameRate <<= 1; //u2IntFrameRate*=2
            }
            #endif  
        }
        else
        {
            u2IntFrameRate = (bOUTPUT_REFRESH_RATE/u2PClkRatio) * u1VTotalDiv;
        }

        /*
         * OCLK / u2PClkRatio = POCLK
         * PANEL_HTOTAL * PANEL_VTOTAL * bOUTPUT_REFRESH_RATE / u2PClkRatio = (MOHLEN+1) * (MOVLEN+1) * u2IntFrameRate
         * u2HtotalRatio = bOUTPUT_REFRESH_RATE * HTOTAL_RATIO_BASE * u1VTotalDiv / (u2PClkRatio * u2IntFrameRate);  
         */
         
        u2HtotalRatio = bOUTPUT_REFRESH_RATE * HTOTAL_RATIO_BASE * u1VTotalDiv / (u2PClkRatio * u2IntFrameRate);        
        _u4InitInternalFrameRate = 1;

        if((wDISPLAY_WIDTH == 1366)
            && !(PANEL_IsSupport120Hz())
            && (u1ScpipGetDispMode(SV_VP_MAIN) == VDP_SCPOS_PSCAN_DISPMODE)
            #ifdef CC_SCPOS_3DTV_SUPPORT
            && (u4DrvTDTV3DModeQuery() == E_TDTV_UI_3D_MODE_OFF)
            #endif
            )
        {
           u2IntFrameRate = bOUTPUT_REFRESH_RATE;
           u2HtotalRatio = HTOTAL_RATIO_BASE;
           u2PClkRatio = 1;
           u1VTotalDiv = 1;
        }      
    }

    if ((u2IntFrameRate != _u2IntFrameRate) 
        || (u2HtotalRatio != _u2HtotalRatio) 
        || (u2PClkRatio != _u2PClkRatio)
        || (u1VTotalDiv != _u1ScalerOutVTotalDiv))
    {
        UINT32 u4PCLK_Sel;

        LOG(3, "Internal Timing Changed! FrameRate(%d --> %d), HtotalRatio(%d --> %d), PClkRatio(%d --> %d)\n",
            _u2IntFrameRate, u2IntFrameRate, _u2HtotalRatio, u2HtotalRatio, _u2PClkRatio, u2PClkRatio);

        _u2IntFrameRate = u2IntFrameRate;
        _u2HtotalRatio = u2HtotalRatio;
        _u2PClkRatio = u2PClkRatio;
        _u1ScalerOutVTotalDiv = u1VTotalDiv;
        // set PCLK ratio
        if (u2PClkRatio == 4)
        {
            u4PCLK_Sel = 2;
        }
        else if (u2PClkRatio == 2)
        {
            u4PCLK_Sel = 1;
        }
        else
        {
            u4PCLK_Sel = 0;
        }

        u1PoclkRatio = u4PCLK_Sel;        
        //vIO32WriteFldAlign(CKGEN_DISP_CKCFG, u4PCLK_Sel, FLD_POCLK_SEL);                                   

        // update MJC input timing and scalar output timing
        vDrvSetLCDTiming();

        return SV_TRUE;
    }
    #endif

    return SV_FALSE;
}
#endif // CC_MTK_LOADER

UINT16 u2DrvGetInternalHtotalRatio(void)
{
    #ifdef CC_MT5395
    return _u2HtotalRatio;
    #else
    return HTOTAL_RATIO_BASE;
    #endif
}

UINT16 u2DrvGetInternalPCLKRatio(void)
{
    #ifdef CC_MT5395
    return _u2PClkRatio;
    #else
    return 1;
    #endif
}

UINT16 u2DrvGetInternalFrameRate(void)
{
    #ifdef CC_MT5395
    return _u2IntFrameRate;
    #else
    return (UINT16)bOUTPUT_REFRESH_RATE;
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
    static BOOL fgInit = FALSE;
    UINT32 dwDCLK = PANEL_GetPixelClk60Hz();
    UINT32 u4Ratio;

    //LogSB("bDCLKType=", bDCLKType);

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
    if (PANEL_IsSupport120Hz())
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
        dwDCLK = PANEL_GetPixelClk50Hz();
        bOUTPUT_REFRESH_RATE = (UINT8)(50 * u4Ratio);
        bSI_DISPLAY_DCLK_TYPE = SV_DCLK_50HZ;
        #ifndef CC_MTK_LOADER
        #ifdef CC_DRV_SUPPORT_SMICRO
        vSMicroPreSetFreq(FREQ_50HZ);
        #endif
        #endif        
        break;

    case SV_DCLK_60HZ:
        dwDCLK = PANEL_GetPixelClk60Hz();
        bOUTPUT_REFRESH_RATE = (UINT8)(60 * u4Ratio);
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
        if(PANEL_IsSupportExtAttribute()!=0)
        {
            dwDCLK = PANEL_GetPixelClk48Hz();
            bOUTPUT_REFRESH_RATE = (UINT8)(48 * u4Ratio);
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
    #endif  // SUPPORT_PANEL_48HZ
    case SV_DCLK_OTHERS:
        bOUTPUT_REFRESH_RATE = bFrameRate;
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

    if (fgInit)
    {
        BeforeOutputFrameRateChange(bOUTPUT_REFRESH_RATE);
    }

    vDrvSwitchMTKGoodDclk(SV_OFF);  //turn off DDDS 8202 (Dragon)

#ifndef CC_COPLAT_MT82
#ifndef __MODEL_slt__
    OSD_PLA_Mute();
#endif
#endif

#ifndef CC_MTK_LOADER
    if (!bDrvUpdateInternalFrameRate())
#endif
    {
        vDrvSetLCDTiming();
    }

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

    if (fgInit)
    {
        AfterOutputFrameRateChange();
    }

    #ifndef CC_MTK_LOADER
    #ifdef CC_DRV_SUPPORT_SMICRO
    {
        SMICRO_VERTICAL_FREQ eFreq;
        SMICRO_MSG rMsg;
        #ifdef SUPPORT_PANEL_48HZ
        if (bOUTPUT_REFRESH_RATE == (UINT8)(48 * u4Ratio))
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

    if (!fgInit)
    {
        fgInit = TRUE;
    }
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

    u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_MOSYNCLEN_MIN + CC_RIGHTMASK_MIN);
    u4HTotalMinBySyncMin = ((u4HTotalMinBySyncMin + 1) & 0xfffffffe);     //make as even value
    u4HTotalMaxBySyncMax = (UINT32) (wDISPLAY_WIDTH + CC_MOSYNCLEN_MAX + CC_RIGHTMASK_MAX);
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
    
    u4VTotalMinByPixelClock = (((UINT32) DCLK_MIN/u2DrvGetInternalPCLKRatio()) / u4FrameRate) / u4HTotalMaxBySyncMax;
    u4VTotalMaxByPixelClock = (((UINT32) DCLK_MAX/u2DrvGetInternalPCLKRatio()) / u4FrameRate) / u4HTotalMinBySyncMin;

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

    if( _bIsImportProtection )
    {
        vDrvSwitchImportProtection(1);
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
        UINT32 u4RightMask;
        UINT32 u4BottomMask;
        UINT32 u4LeftMask;
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


            if( _bIsImportProtection )
            {
                vDrvSwitchImportProtection(0);
            }
            return;
        }
        
        u4HTotalMinByPixelClock = (((UINT32) DCLK_MIN/u2DrvGetInternalPCLKRatio()) / u4RefreshRate) / u4VTotal;
        u4HTotalMinByPixelClock = ((u4HTotalMinByPixelClock + 1) & 0xfffffffe);     //make as even value
        u4HTotalMaxByPixelClock = (((UINT32) DCLK_MAX/u2DrvGetInternalPCLKRatio()) / u4RefreshRate) / u4VTotal;
        u4HTotalMaxByPixelClock = ((u4HTotalMaxByPixelClock - 1) & 0xfffffffe);     //make as even value
        u4HTotalMinBySyncMin = (UINT32) (wDISPLAY_WIDTH + CC_MOSYNCLEN_MIN + CC_RIGHTMASK_MIN);
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
        // 5) Bigger than H-Total MIN calculate by CC_MOSYNCLEN_MIN
        // 6) Close to HLEN_TOTAL_TYPI

#ifdef CC_HTOTAL_4X_LIMIT_WA
        if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//        if (LVDS_OUTPUT_PORT == DUAL_PORT)
        {
            u4HTotal = (UINT32) ((HLEN_TOTAL_TYPI + 1) + 3) & (~3);
        }
        else
#endif
        {
            u4HTotal = (UINT32) (HLEN_TOTAL_TYPI + 1);
        }

        if (u4HTotal > u4HTotalMaxByPixelClock)
        {
            u4HTotal = u4HTotalMaxByPixelClock;
        }

#ifdef CC_HTOTAL_4X_LIMIT_WA
        if ((LVDS_OUTPUT_PORT == DUAL_PORT) || (LVDS_OUTPUT_PORT == FOUR_PORT))
//        if (LVDS_OUTPUT_PORT == DUAL_PORT)
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

        // Pixel Clock
        u4PixelClock = (u4RefreshRate * u4VTotal) * u4HTotal *u2DrvGetInternalPCLKRatio();

        // [DTV00111681] Update related register and Do OSD_PLA_Mute only when u4VTotal or u4HTotal changed.
        // Peggy 20080109
        wOldHTotal = wDrvGetOutputHTotal();
        wOldVTotal = wDrvGetOutputVTotal(); 
        	
        if ((wOldHTotal != u4HTotal) || (wOldVTotal != u4VTotal))
        {        	
            #ifndef CC_COPLAT_MT82
            #ifndef __MODEL_slt__
            OSD_PLA_Mute();
            #endif
            #endif
        
            if (WFB_PANEL_RESET == WFB_PANEL_RESET_ON)
            {
                LVDS_SwitchDataEnable(FALSE);
            }
    
            vDrvVOFreqSet(u4PixelClock);
           #ifndef CC_MTK_LOADER		
           vDrvSetSSMDRAsInputHsync();//for dynamic MDR
	    #endif	
    
            // HSync Length
            u4HSyncLen = (u4HTotal - wDISPLAY_WIDTH) - 1;
            if (u4HSyncLen >= CC_MOSYNCLEN_MAX)
            {
                u4HSyncLen = CC_MOSYNCLEN_MAX -1;
            }
    
            // H Total
            u4HTotalFinal = u4HTotal - 1;
    
            // V Total
            u4VTotalFinal = u4VTotal - 1;

            #if CC_SCPOS_VSYNC_WA1
            _u4VTotal = u4VTotalFinal;
            _u4HSyncLen = u4HSyncLen;
            _u4HTotal = u4HTotalFinal;
            #else
            vRegWriteFldAlign(SCPIP_PIP_OA_00, u4VTotalFinal, PIP_OA_00_MOVLEN);
            #ifndef CC_COPLAT_MT82
            _OSD_AlwaysUpdateReg(TRUE);
            vRegWriteFldAlign(SCPIP_PIP_OA_00, u4HTotalFinal, PIP_OA_00_MOHLEN);
            _OSD_AlwaysUpdateReg(FALSE);
            #else
            vRegWriteFldAlign(SCPIP_PIP_OA_00, u4HTotalFinal, PIP_OA_00_MOHLEN);
            #endif
			#if defined(CC_SCPOS_3DTV_SUPPORT) &&  !defined(CC_MTK_LOADER) && !defined(CC_MT5365)
			if (u1DrvMJCIsOnOff())
			{
				u2HSyncLen = u4HSyncLen-_u4OSTGDelta;
			}
			else
			{
	        	vRegWriteFldAlign(SCPIP_PIP_OA_01, u4HSyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
			}
			#else
	        vRegWriteFldAlign(SCPIP_PIP_OA_01, u4HSyncLen-_u4OSTGDelta, PIP_OA_01_MOSYNCLEN);
			#endif
            #endif

            #ifdef CC_MT5395
            vDrvMJCSetOutputTiming(TV_WIDTH, TV_HEIGHT, u4HTotalFinal+1, u4VTotalFinal+1, 0, 0);
            #endif

            u4LeftMask = 0;            
            u4RightMask = ((u4HTotalFinal - u4HSyncLen) - wDISPLAY_WIDTH - u4LeftMask);
            u4BottomMask = (u4VTotalFinal - wDISPLAY_HEIGHT);
            
            #if CC_SCPOS_VSYNC_WA1
            _u1BottomMask = u4BottomMask;
            _u1LeftMask = u4LeftMask;
	    _u1RightMask = u4RightMask;
            _u4OutHVChanged = 1;
            vRegWriteFldAlign(OUTSTG_OS_27, 0, OS_27_TOP_MASK);
            #else        
            vDrvSetDENAMask(0, (UINT8)u4BottomMask, (UINT8)u4LeftMask, (UINT8)u4RightMask);
            #endif

            if (wOldHTotal != u4HTotal)
            {
                vDrvUpdateHPosition();
            }	
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
                vDrvLCDFreqSet(SV_DCLK_48HZ, (PANEL_IsSupport120Hz()?96 : 48));
                #else
                vDrvLCDFreqSet(SV_DCLK_60HZ, (PANEL_IsSupport120Hz()?120 : 60));
                #endif
            }
	 }
        vDrvSwitchMTKGoodDclk(SV_OFF);
    }
	
    if (WFB_PANEL_RESET == WFB_PANEL_RESET_ON)
    {
        LVDS_SwitchDataEnable(TRUE);
    }

    fgPreviousMode = BiMode;

    if( _bIsImportProtection )
    {
        vDrvFireImportPortection();
        _bIsImportProtection = IMPORT_PROTECTION_RECOVER;
    }
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
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        vDrvSetOSTGVPos(PANEL_GetVPositionByTiming(bSI_DISPLAY_DCLK_TYPE));    
        #else
        vDrvSetOSTGVPos(V_POS);
        #endif

        vDrvUpdateHPosition();
        #ifndef CC_COPLAT_MT82
        IGNORE_RET(OSD_UpdateTiming());
        #endif

      //  LOG(5, "turn off pixel shift, width=%d, height=%d\n", PANEL_GetPanelWidth(), PANEL_GetPanelHeight());
    }
}

void vDrvVideoSetMuteColor(UINT8 bPath, UINT32 u4Bg) 
{
    // luis@2006/8/14, set bgcolor at blanking
    RDispInfo *prDispInfo;
    if (bPath == SV_VP_MAIN)
    {
        prDispInfo = &_rMDispInfo;
    }
    else
    {
        prDispInfo = &_rPDispInfo;
    }

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
#ifdef SUPPORT_CW_CHANGE_BY_STEP
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
    static UINT8 u1StepControlWord = STEP_CW+1;
    static INT32 i4DeltaControlWord = 0;

    if (IO32ReadFldAlign(DDDS_00,DDDS_00_DISP_EN)== 0)    
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
        }
    }
    else
    {
        u1StepControlWord = 0;
    }

#endif

#endif // CC_MTK_LOADER
    return;
}


// OSTG Event on Output VSync
// Note: Keep this function simple since this function is called by ISR
void OSTG_OnOutputVSync(void)
{
    vDrvOstgParaUpdate();
    
#ifndef CC_MTK_PRELOADER
    if (_u4OstgUpdateEvent & OSTG_UPDATE_PIXEL_SHIFT_H)
    {
        vDrvUpdateHPosition();
        _u4OstgUpdateEvent &= ~OSTG_UPDATE_PIXEL_SHIFT_H;
    }
    if (_u4OstgUpdateEvent & OSTG_UPDATE_PIXEL_SHIFT_V)
    {
        #ifdef SUPPORT_PANEL_CUSTOMER_SPEC
        vDrvSetOSTGVPos(PANEL_GetVPositionByTiming(bSI_DISPLAY_DCLK_TYPE));
        #else
        vDrvSetOSTGVPos(V_POS);
        #endif

        _u4OstgUpdateEvent &= ~OSTG_UPDATE_PIXEL_SHIFT_V;
    }    
#endif

#ifndef CC_MTK_LOADER
    if (fgIsScposFlgSet(MAIN_MUTECOLOR_CHG))
    {
        vScposSetMuteColor(SV_VP_MAIN, _rMDispInfo.u4MuteColor);
        vClrScposFlg(MAIN_MUTECOLOR_CHG);
    }
    if (fgIsScposFlgSet(PIP_MUTECOLOR_CHG))
    {
        vScposSetMuteColor(SV_VP_PIP, _rPDispInfo.u4MuteColor);
        vClrScposFlg(PIP_MUTECOLOR_CHG);
    }

    if (_bIsImportProtection > IMPORT_PROTECTION)
    {
        _bIsImportProtection --;
        if (_bIsImportProtection == IMPORT_PROTECTION)
        {
            vDrvSwitchImportProtection(0);
        }
    }
#endif 
}


void vDrvSwitchImportProtection(UINT8 bOn_Off)
{
    if ( bOn_Off )
    {
        vRegWriteFldAlign(OUTSTG_OS_EB, 0, OS_EB_SET_DATA_RDY);
        vRegWriteFldAlign(SCPIP_PIP_OA_01, 0, PIP_OA_01_SET_DATA_SEL);
//        vRegWriteFldAlign(SCPIP_PIP_OA_13, 0, PIP_OA_13_SET_DATA_EXTERNAL_SEL);

        #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
        vDrvMJCImportProtection(SV_ON);
        #endif

    }
    else
    {
        vRegWriteFldAlign(OUTSTG_OS_EB, 1, OS_EB_SET_DATA_RDY);
        vRegWriteFldAlign(SCPIP_PIP_OA_01, 1, PIP_OA_01_SET_DATA_SEL);
//        vRegWriteFldAlign(SCPIP_PIP_OA_13, 1, PIP_OA_13_SET_DATA_EXTERNAL_SEL);

        #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
        vDrvMJCImportProtection(SV_OFF);
        #endif    
    }
}


void vDrvFireImportPortection(void)
{
//	static UINT32 u4Toggle = 0;
//    u4Toggle = (u4Toggle == 0)? 1 : 0;
//	vRegWriteFldAlign(OUTSTG_OS_1E, u4Toggle, OS_1E_SET_DATA);
    vRegWriteFldAlign(SCPIP_PIP_OA_12, 0, PIP_OA_12_SET_DATA_TRIGGER);
    vRegWriteFldAlign(SCPIP_PIP_OA_12, 1, PIP_OA_12_SET_DATA_TRIGGER);    

    #if defined(CC_MT5395) && !defined(CC_MTK_LOADER)
    vDrvMJCFireImportProtection();
    #endif    
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

    if(RegReadFldAlign(SCPIP_PIP1_0B,  PIP1_0B_F_SYNC_1)==1)
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
            //vRegWriteFldAlign(SCPIP_PIP_OA_08, 0, PIP_OA_08_DFIFO_AUTO_TUNE1);
            //vRegWriteFldAlign(SCPIP_PIP1_0B, 0, PIP1_0B_F_SYNC_1);
               if (bScaleMode == VDP_SCPOS_DISPMODE_AUTO || bScaleMode == VDP_SCPOS_DISPMODE_MANUAL) 
               	{
         	   vScpipSetDispModeFreeRunCount(150);
               	} 
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

    LOG(9, "======u1FlipMirrorEnable %d\n\n", u1FlipMirrorEnable);
    return u1FlipMirrorEnable;
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
    else if(u2FrameRate == 48 || u2FrameRate == 96)
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
    vErrorHandleSetByPanel();
    vDrvSetLCDTiming();
    #ifdef DRV_SUPPORT_EXTMJC
    vDrvExtmjcRedoTwoChipConnection( SV_VP_MAIN, SV_TRUE);
    #endif
#ifdef CC_SCPOS_3DTV_SUPPORT 	
#ifdef DRV_SUPPORT_MT8283
    fgForceChangeRegion = TRUE;
#endif
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

#ifdef CC_FAST_INIT
void vDrvDisplayInit_pm_resume(void)
{
  bSI_DISPLAY_DCLK_TYPE = SV_DCLK_UNKNOW;
  VOPLL_pm_resume();
  #if SUPPORT_DISPLAY
  printf("[SA7]vApiHwInit\n");	
  vDrvDisplayInit();
  vDrvLVDSInit();
  LOG(0, "Display Init");
  #endif
}
#endif

LINT_RESTORE
