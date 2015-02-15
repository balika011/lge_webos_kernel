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
 * $RCSfile: drv_scaler_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#define _DRV_SCPOS_C_
#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif
#ifndef CC_COPLAT_MT82
#include "x_os.h"
#include "x_hal_arm.h"
#endif
#include "x_hal_5381.h"
#include "x_printf.h"
#include "general.h"
#include "hw_scpos.h"
#include "hw_ckgen.h"
#include "drv_scaler.h"
#include "drv_scaler_gfx.h"
#include "drv_scpos.h"
#include "drv_upscaler.h"
#include "drv_display.h"
#include "scpos_debug.h"
#include "source_select.h"
#include "vdo_misc.h"
#include "tve_if.h"
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif

#ifndef CC_MTK_LOADER   	   	
#ifdef CC_SUPPORT_UHD
#include "drv_uhd.h"
#include "panel.h"
#endif
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
// in 5365/5395, min dram unit is 16
#define CC_MINIMUN_DRAM_RW_UNIT 16


/**************************************************************************
 * Global/Static variables
 *************************************************************************/

//-----------------------------------------------------------------------------
// extern functions
//-----------------------------------------------------------------------------
#if  CC_FLIP_MIRROR_SUPPORT
extern void vReinitFlipMirrorConfig(void);
#endif


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

EXTERN VDP_SCALER_PRM_T _arScalePrm[VDP_NS];


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifndef CC_MTK_LOADER
static void _vScpipInitPOCLK(void);
#ifdef	USE_FRAME_TRACK_WA
EXTERN UINT32 u4FrmTrkStep;
#endif

#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

#ifndef CC_MTK_LOADER

void vScpipSubSrcSel(UINT8 u1Sel)
{
    if (u1Sel >= SUB_SRC_SEL_MAX)
        return;

    SCPIP_WRITE_FLD(SCPIP_SCSYS_07, u1Sel , SCSYS_07_SC_PIP_SRC_SEL );
    if (u1Sel == SUB_OSTG)
    {
        vScpipSetSubCLKSelReg(FLD_POCLK_SEL_OCLK);
    }
    else
    {
        if ( IS_IC_5861() && PANEL_IsSupport4K2KDD() )
          	vScpipSetSubCLKSelReg(FLD_POCLK_SEL_MIB_OCLK_DIV2);
		else	
          	vScpipSetSubCLKSelReg(FLD_POCLK_SEL_MIB_OCLK);
    }
}

 UINT32 vScpipGetSubSrcSel(void)
 {
    return SCPIP_READ_FLD(SCPIP_SCSYS_07 , SCSYS_07_SC_PIP_SRC_SEL);
 }


void vScpipOstgMonSrcSel(UINT8 u1Sel)
{
    LOG(1, "vScpipOstgMonSrcSel to %d\n", u1Sel);
    SCPIP_WRITE_FLD(OSTG_00, u1Sel , R_OSTG2MON_SEL );
}



#if SUPPORT_HDTV_HARDWARE_MUTE
static void vOSTGLVDSHWMute(UINT8 mode)
{
    SCPIP_WRITE_FLD(MUTE_00, mode, R_MUTE_POST_SEL);
}

void vOSTGVideoPlaneHWMute(UINT8 bPath,UINT8 mode)
{
    if(bPath == SV_VP_MAIN)
    {
        SCPIP_WRITE_FLD(MUTE_00, mode, R_M_MUTE_FRONT_SEL);
    }
    else if(bPath == SV_VP_PIP)
    {
        SCPIP_WRITE_FLD(MUTE_00, mode, R_P_MUTE_FRONT_SEL);
    }
}

void vScpipConfigHardwareMute(void)
{
    UINT8 u1Dispmode;
    E_QUICK_BLANK_MODE eMode = QUICK_BLANK_NO_FUNCTION;
    
    u1Dispmode = u1ScpipGetDispMode(SV_VP_MAIN);

    if(u1Dispmode ==VDP_SCPOS_LINESYNC_DISPMODE)
    {
        //only enable in YPbPr and VGA & HDMI
        if (((bGetSignalType(SV_VP_MAIN) == SV_ST_YP) || (bGetSignalType(SV_VP_MAIN) == SV_ST_VGA) || (bGetSignalType(SV_VP_MAIN) == SV_ST_DVI)))
        {
            eMode = QUICK_BLANK_BY_MAIN;
        }
        else
        {
            eMode = QUICK_BLANK_NO_FUNCTION;
        }
    
    }

    if (eMode == QUICK_BLANK_NO_FUNCTION)
    {
        //disable LVDS quick blank mute
        vOSTGLVDSHWMute(SV_OFF);
        //enable vdp quick mute
        vOSTGVideoPlaneHWMute(SV_VP_MAIN, SV_ON);
    }
    else 
    {
        //enable LVDS quick blank mute
        vOSTGLVDSHWMute(SV_ON);
        //disable vdp quick mute
        vOSTGVideoPlaneHWMute(SV_VP_MAIN, SV_OFF);
    }
}
#endif

//review this
UINT32 u4ScpipGetMinSrcRegionHeight(UINT8 u4VdpId)
{
    return 0;
}

/**
 *  @6896 No change
 */ 
UINT32 u4ScpipGetMinSrcRegionWidth(UINT8 u4VdpId)
{
    UINT32 ret;
    UINT16 inputWidth;
	UINT16 u2SclMinWidth = 16;
   
    ret = 0;
    inputWidth = wDrvVideoInputWidth(u4VdpId);
    if(inputWidth==0)
    {
        return ret;
    }
    ret = (MAX(u2SclMinWidth, u4DrvDIMiniInputWidth(u4VdpId))*VDP_MAX_REGION_WIDTH) /inputWidth;
    
    if(ret > VDP_MAX_REGION_WIDTH)//Not support zoom when input width < CC_MINIMUN_DRAM_RW_UNIT*6 or CC_MINIMUN_DRAM_RW_UNIT*16/3
    {
        return VDP_MAX_REGION_WIDTH;
    }
    return ret;
}

//@Cobra. Don't support.
UINT32 u4ScpipDirectSetDRAM(UINT32 u4VdpId, UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height, UINT32 u4GBRYCbCr)
{
    return 0;
}

// repeat status
UINT32 u4ScpipGetRepeatStatus(UINT32 u4VdpId)
{
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);

    return SCPIP_READ_FLD(SCPIP_DRAM_M_02 + dramOffset, DRAM_M_02_STA_RD_REPEAT);
}

// overflow/oob status
UINT32 u4ScpipGetDramOverflowStatus(UINT32 u4VdpId)
{
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);
    
    return SCPIP_READ_FLD(SCPIP_DRAM_M_02 + dramOffset, DRAM_M_02_STA_OVERFLOW);
}

UINT32 u4ScpipGetGfxOverflowStatus()
{
    return SCPIP_READ_FLD(SCPIP_DRAM_M_02, DRAM_M_02_STA_GFX_OVERFLOW);
}

UINT32 u4ScpipGetDramOOBStatus(UINT32 u4VdpId)
{
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);
    
    return SCPIP_READ_FLD(SCPIP_DRAM_M_02 + dramOffset, DRAM_M_02_STA_WRITE_OOB);
}

UINT32 u4ScpipGetGfxOOBStatus()
{
    return SCPIP_READ_FLD(SCPIP_DRAM_M_02, DRAM_M_02_STA_GFX_WRITE_OOB);
}

UINT32 u4ScpipGetUnderflowStatus(UINT32 u4VdpId)
{
    UINT32 u4Underflow;;

    if (u4VdpId == VDP_1)
    {
        u4Underflow = SCPIP_READ_FLD(SCPIP_SCSYS_01, SCSYS_01_SC_STA_M_TG_UNDERFLOW);
    }
    else
    {
        u4Underflow = SCPIP_READ_FLD(SCPIP_SCSYS_01, SCSYS_01_SC_STA_P_TG_UNDERFLOW);
    }

    return u4Underflow;
}

void vScpipClearUnderflow(UINT32 u4VdpId)
{
    if (u4VdpId == VDP_1)
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, 1, SCSYS_01_SC_CLR_MTG_UDFL_FLG);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, 0, SCSYS_01_SC_CLR_MTG_UDFL_FLG);
    }
    else
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, 1, SCSYS_01_SC_CLR_PTG_UDFL_FLG);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, 0, SCSYS_01_SC_CLR_PTG_UDFL_FLG);
    }    
}

/*******************************************************/
/*** Start of Function for Dram overflow cli command ***/
/*******************************************************/

/**
 *  @Cobra porting done
 */
UINT32 u4ScpipCheckFifoOverflow(UINT32 u4VdpId)
{
    UINT8 bPath;
    UINT16 dramOffset;
    UINT8 fOverflow;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);

    if(u1ScpipGetDispMode(bPath) != VDP_SCPOS_DISPMODE_OFF)
    {
        LOG(6, "FIFO overflow/underflow flag is not supported in display mode\n");
        return 4; // not supported in display mode
    }
    else
    {        
        //return overflow status
        fOverflow = SCPIP_READ_FLD(SCPIP_DRAM_M_02 + dramOffset, DRAM_M_02_STA_OVERFLOW);
        return fOverflow;
    }   
}

/**
 *  @Cobra porting done
 */
void vScpipResetFifoOverflow(UINT32 u4VdpId)
{
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);
    
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_00 + dramOffset, 1, DRAM_M_00_DA_OVERFLOW_CLR);
    //x_thread_delay(20);   //remove thread delay. need to check
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_00 + dramOffset, 0, DRAM_M_00_DA_OVERFLOW_CLR);
}

/*******************************************************/
/***  end of Function for Dram overflow cli command  ***/
/*******************************************************/

//-----------------------------------------------------------------------------
/** Brief of vScpipHwInit.
 * Initial SCPOS related setting
 * @param  bPath
 * @retval void
 * @6896 porting done
 */
//-----------------------------------------------------------------------------
void vScpipHwInit(UINT8 bPath)
{    
    //Reset write enable to off state in initialization
    #if defined(CC_MT5890) 
    vScpipSetHsyncFp(E_FSC_TG, 0);
    vScpipSetHsyncBp(E_FSC_TG, 0);
    vScpipSetVsyncFp(E_FSC_TG, 0);
    vScpipSetVsyncBp(E_FSC_TG, 0);
    vScpipSetData(1);
	vScpipSetPureEdgeBlankNum(120);
    #endif
	
    vScpipSubSrcSel(SUB_FSC);   //mt5399 defualt set to original sub video path
    
	#if defined(CC_MT5880)
	vScpipSetPureEdgeBlankNum(18);
	#endif
	
    vScpipSetDramWrite(bPath, SV_OFF);
    vScpipSetDramSpacing(bPath, 1); // default
    vScpipSetTgImportVCnt(5); // default, for window position garbage
    vScpipSetDramReadReqDly(bPath, 0);
    if (BSP_GetIcVersion() != IC_VER_5399_AA) // after ECO, set to frame mode as default, set to field mode when VENC
    {
        vScpipH264SetFrameMode(E_VENC_MODULE_MAIN_SCALER, 1); // frame mode
        vScpipH264SetFrameMode(E_VENC_MODULE_SUB_SCALER,  1);
        vScpipH264SetFrameMode(E_VENC_MODULE_MON_SCALER,  1);
    }

    //default turn-off snake mapping
    vScpipSetDramSnakeMap(bPath, SV_OFF);

    if (bPath == SV_VP_MAIN)
    { 
        #if SUPPORT_HDTV_HARDWARE_MUTE
        vScpipConfigHardwareMute();
        #endif        
		               
        vScpipUpdateDelay();

        //init poclk
        _vScpipInitPOCLK();

       // vScpipSetPipRstCnt(4);

        //to do : check later
       // vScpipSetDramRdreqDly(bPath,0);
	    #if defined(CC_MT5890) 
	    vScpipMuxSetTgImportOnOff( SV_ON );
	    #endif
		
		#if	 CC_FLIP_MIRROR_SUPPORT
    	vReinitFlipMirrorConfig();
		#endif
    }
    else
    {
        #if SUPPORT_HDTV_HARDWARE_MUTE
        //enable sub path quick off function by default
        vOSTGVideoPlaneHWMute(SV_VP_PIP,SV_ON);
        #endif
        
        /* initially hide sub window output region */
        vScpipSetImportOnOff(bPath, SV_OFF);
        vScpipSetOutputResolution(bPath,0,0);
        vScpipSetImportOnOff(bPath, SV_ON);       
    }
    
#ifndef CC_FPGA_SUPPORT
    vScpipSetISR();
#endif

#ifdef DRV_USE_EXTERNAL_3D_FRC
    //Inverse Output DISP_R because DISP_R Trainsition not ready befroe VSync,    
    vIO32WriteFldAlign(SCPIP_SCSYS_11, SV_ON ,SCSYS_11_SC_INV_MIB_DISP_R);
#endif
}

/**
 * @brief vScpipImportOnOff 
 * Turn on/off import protection
 * @param  bPath: SV_VP_MAIN/SV_VP_PIP
 *         bOnOff: SV_ON/SV_OFF
 * @retval void
 * @Cobra porting done
 */
void vScpipSetImportOnOff(UINT8 bPath, UINT8 bOnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A+getSCOffset(bPath), (bOnOff==SV_ON)?1:0, SCCTRL1_0A_SC_SET_RES_TOGGLE_1);    
}

/**
 *  @6896 new
 */
static void _vScpipInitPOCLK(void)
{
    //1. set POCLK to divider PLL
    vScpipSetPOCLKSelReg(FLD_POCLK_SEL_DPLL);
    //2. set divider PLL source to System PLL / 2 (324Mhz)
    vScpipSetPOCLKDPllSelReg(FLD_POCLK_DPLL_SEL_SYSPLL_DIV2);
    //3. set divider m/ n to 0x4000/0x8000 as default value
    vScpipSetPOCLKDividerReg(0x8000,0x8000);
    //4. set PIP POCLK_TVE to divier PLL
    vScpipSetOclkTveSelReg(FLD_OCLK_TVE_SEL_DPLL);
}

void vScpipSetPOCLKDPllSelReg(UINT8 poclkDpllSel)
{
    if(poclkDpllSel > FLD_POCLK_DPLL_SEL_TAPLL_DIV2)
    {
        return;
    }

    SCPIP_WRITE_FLD(CKGEN_VPCLK_CFG, poclkDpllSel, FLD_POCLK_DPLL_SEL);    
}

void vScpipSetOclkTveSelReg(UINT8 oclkTveSel)
{
    if(oclkTveSel > FLD_OCLK_TVE_SEL_TVE27_CK)
    {
        return;
    }
    
    SCPIP_WRITE_FLD(CKGEN_OCLK_TEST, oclkTveSel, FLD_OCLK_TVE_SEL);
}

void vScpipSetPOCLKSelReg(UINT8 poclkSel)
{
    UINT8 tveEnable;
    TVE_GetEnable(TVE_1, &tveEnable);    

    if(poclkSel > FLD_POCLK_SEL_MIB_OCLK)
    {
        return;
    }
    //Set POCLK (for main path)
    SCPIP_WRITE_FLD(CKGEN_DISP_CKCFG, poclkSel, FLD_POCLK_SEL);
    //if TVE is not enabled 
    if(tveEnable==SV_FALSE)
    {
        //set OCLK_TVE (for sub path)
        vScpipSetOclkTveSelReg(poclkSel);
    }
}

void vScpipSetPOCLKDividerReg(UINT32 dividend, UINT32 divisor)
{
    //1. set divider m/ n 
    SCPIP_WRITE_FLD(CKGEN_POCLK_DPLL_DIVCFG, dividend, FLD_POCLK_DPLL_IDEAL_PLL_M);
    SCPIP_WRITE_FLD(CKGEN_POCLK_DPLL_DIVCFG, divisor, FLD_POCLK_DPLL_IDEAL_PLL_N);
}

void vScpipSetMonOutCLKSelReg(UINT8 u1MonOutClkSel)
{
    if(u1MonOutClkSel > FLD_POCLK_SEL_MIB_OCLK)
    {
        return;
    }

    SCPIP_WRITE_FLD(CKGEN_MON_OUT_CKCFG, u1MonOutClkSel, FLD_MON_OUT_CK_SEL);
}

void vScpipSetSubCLKSelReg(UINT8 u1SubClkSel)
{
    if(u1SubClkSel > FLD_POCLK_SEL_MIB_OCLK_DIV2)
    {
        return;
    }

    SCPIP_WRITE_FLD(CKGEN_MON_OUT_CKCFG, u1SubClkSel, FLD_SC_PIP_ICLK_SEL);
}

void vScpipSetGfxCLKSelReg(UINT8 u1GfxClkSel)
{
    if(u1GfxClkSel > FLD_POCLK_SEL_MIB_OCLK)
    {
        return;
    }

    SCPIP_WRITE_FLD(CKGEN_MON_OUT_CKCFG, u1GfxClkSel, FLD_SC_GFX_CK_SEL);
}

#endif


void vScpipSetHInitPhase(UINT8 bPath, UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_00+getSCOffset(bPath), u4InitSign, SCCTRL1_00_SC_H_PHASE_INIT_SIGN_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_00+getSCOffset(bPath), u4InitIntg, SCCTRL1_00_SC_H_PHASE_INIT_INTG_1);
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCCTRL1_00, u4InitFrac, SCCTRL1_00_SC_H_PHASE_INIT_FRAC_1)
					 : SCPIP_WRITE_FLD(SCPIP_SCCTRL2_00, u4InitFrac, SCCTRL2_00_SC_H_PHASE_INIT_FRAC_2);
}

void vScpipSetVScalingMode(UINT8 bPath, UINT8 bVScalingMode)
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_30+getSCOffset(bPath), bVScalingMode, SCCTRL1_30_VSC_PART_SEL_1);
}

void vScpipSetVPartialSumFactor(UINT8 bPath, UINT32 u4Factor, UINT32 u4InitL, UINT32 u4InitR)
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_29+getSCOffset(bPath), u4Factor, SCCTRL1_29_PART_DNSC_V_1);
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_29+getSCOffset(bPath), u4InitL, SCCTRL1_29_PART_DNSC_V_INIT_1);
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_30+getSCOffset(bPath), u4InitR, SCCTRL1_30_PART_DNSC_V_INIT_RIGHT_1);
}

void vScpipSetHDeltaPhase(UINT8 bPath, UINT32 u4DeltaIntg, UINT32 u4DeltaFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_01+getSCOffset(bPath), u4DeltaIntg, SCCTRL1_01_SC_H_PHASE_DELTA_INTG_1);
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCCTRL1_01, u4DeltaFrac, SCCTRL1_01_SC_H_PHASE_DELTA_FRAC_1)
					 : SCPIP_WRITE_FLD(SCPIP_SCCTRL2_01, u4DeltaFrac, SCCTRL2_01_SC_H_PHASE_DELTA_FRAC_2);	
}

void vScpipSetVInitPhase(UINT8 bPath, UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_02+getSCOffset(bPath), u4InitSign, SCCTRL1_02_SC_V_PHASE_INIT_SIGN_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_02+getSCOffset(bPath), u4InitIntg, SCCTRL1_02_SC_V_PHASE_INIT_INTG_1);
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCCTRL1_02, u4InitFrac, SCCTRL1_02_SC_V_PHASE_INIT_FRAC_1)
					 : SCPIP_WRITE_FLD(SCPIP_SCCTRL2_02, u4InitFrac, SCCTRL2_02_SC_V_PHASE_INIT_FRAC_2);
}

void vScpipSetVDeltaPhase(UINT8 bPath, UINT32 u4DeltaIntg, UINT32 u4DeltaFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_03+getSCOffset(bPath), u4DeltaIntg, SCCTRL1_03_SC_V_PHASE_DELTA_INTG_1);
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCCTRL1_03, u4DeltaFrac, SCCTRL1_03_SC_V_PHASE_DELTA_FRAC_1)
					 : SCPIP_WRITE_FLD(SCPIP_SCCTRL2_03, u4DeltaFrac, SCCTRL2_03_SC_V_PHASE_DELTA_FRAC_2);		
}

/**
 * @brief Set UV initial phase in 422 domain (only in MAIN path)
 * @8098 porting done
 */
void vScpipSetHUVInitPhase(UINT8 bPath,UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
    if(bPath == SV_VP_MAIN) //only main support this function
    {
    	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u4InitSign, SCCTRL1_04_SC_H_UV_PHASE_INIT_SIGN_1);
    	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u4InitIntg, SCCTRL1_04_SC_H_UV_PHASE_INIT_INTG_1);
    	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u4InitFrac, SCCTRL1_04_SC_H_UV_PHASE_INIT_FRAC_1);	
    }
}

void vScpipSetTimeGenDelay(UINT8 bPath, UINT32 u4Delay)
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_26+getSCOffset(bPath), u4Delay, SCCTRL1_26_SC_TG_DLY_1);
	LOG(3, "PATH(%d) SET Time Gen DLY TO(%d)\n",bPath,u4Delay);
}

void vScpipSetTimeGenDlyImportOnOff(UINT8 bPath, UINT32 u4OnOff)
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_26+getSCOffset(bPath), u4OnOff, SCCTRL1_26_SC_TG_DLY_IMPORT_ENABLE_1);
	LOG(3, "PATH(%d) SET TG Dly Import OnOff(%d)\n",bPath,u4OnOff);
}

UINT32 vScpipGetTimeGenDlyImportOnOff(UINT8 bPath)
{
    return  SCPIP_READ_FLD(SCPIP_SCCTRL1_26+getSCOffset(bPath), SCCTRL1_26_SC_TG_DLY_IMPORT_ENABLE_1);
}

UINT32 vScpipGetInputWidth(UINT8 bPath)
{   
    return SCPIP_READ_FLD(SCPIP_SCCTRL1_09+getSCOffset(bPath), SCCTRL1_09_SC_INPUT_WIDTH_1);
}

UINT32 vScpipGetInputHeight(UINT8 bPath)
{ 
    return SCPIP_READ_FLD(SCPIP_SCCTRL1_09+getSCOffset(bPath), SCCTRL1_09_SC_INPUT_HEIGHT_1);
}

void vScpipSetInputResolution(UINT8 bPath, UINT32 u4InputWidth, UINT32 u4InputHeight)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_09+getSCOffset(bPath), u4InputWidth, SCCTRL1_09_SC_INPUT_WIDTH_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_09+getSCOffset(bPath), u4InputHeight, SCCTRL1_09_SC_INPUT_HEIGHT_1);	
}

void vScpipSetDramSrcWidth(UINT8 bPath, UINT32 u4DramWidth)
{
    if(u4DramWidth != 0)
    {
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0C+getSCOffset(bPath), u4DramWidth, SCCTRL1_0C_SC_DRAM_SRC_W_WIDTH_1);	
    }
}

void vScpipSetDramSrcHeight(UINT8 bPath, UINT32 u4DramHeight)
{
    if(u4DramHeight != 0)
    {
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_26+getSCOffset(bPath), u4DramHeight, SCCTRL1_26_SC_DRAM_SRC_W_HEIGHT_1);	
    }
}

void vScpipSetDramWidthHeight(UINT8 bPath, UINT32 u4DramWidth, UINT32 u4DramHeight)
{
    if(u4DramWidth != 0)
    {
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1C+getSCOffset(bPath), u4DramWidth, SCCTRL1_1C_SC_DRAM_W_WIDTH_1);
    }
    
    if(u4DramHeight != 0)
    {
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1C+getSCOffset(bPath), u4DramHeight, SCCTRL1_1C_SC_DRAM_W_HEIGHT_1);
    }
}

void vScpipSetDramHVPitch(UINT8 bPath, UINT32 u4DramWidth, UINT32 u4DramHeight)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1F+getSCOffset(bPath), u4DramWidth, SCCTRL1_1F_SC_WR_DRAM_H_PITCH_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1E+getSCOffset(bPath), u4DramHeight, SCCTRL1_1E_SC_WR_DRAM_V_PITCH_1);
}

void vScpipSetOutputResolution(UINT8 bPath, UINT32 u4OutputWidth, UINT32 u4OutputHeight)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath), u4OutputWidth, SCCTRL1_11_SC_OUTPUT_WIDTH_1);

	if(u4OutputHeight !=0)
	{
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath), u4OutputHeight, SCCTRL1_11_SC_OUTPUT_HEIGHT_1);	
	}
}

/**
 * @brief Set RPT_GEN_VCNT to the last line automatically.
 * @5399 porting done
 */
void vScpipSetReadPosGenAuto(UINT8 bPath, UINT32 u4OnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), u4OnOff, SCCTRL1_1B_SC_RPT_GEN_AUTO_1);		
}


/**
 * @brief Set RPT_GEN_VCNT to the last line automatically.
 * @5399 porting done
 */
void vScpipSetHybridModeFlipMirror(UINT8 bPath, UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_01+getDramOffset(bPath), u4Mode, DRAM_M_01_DA_RD_DISPLAY_MODE);
}

void vScpipSetRptGenVtotalSel(UINT8 bPath, UINT32 u4OnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), u4OnOff, SCCTRL1_1B_SC_RPT_GEN_VTOT_SEL_1);		
}

void vScpipSetScRptDisp_R_EN(UINT32 u4OnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B, u4OnOff, SCCTRL1_1B_SC_RPT_DISP_R_EN);		
}

void vScpipSetScRptDisp_R( UINT32 u4OnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B, u4OnOff, SCCTRL1_1B_SC_RPT_DISP_R);		
}


//in nromal mode: frc line pulse select to FSC 
//in hybrid mode: Frc line pulse select to MIB
void vScpipFscFrcLinePulseSel(UINT8 bOnOff)
{
  if (IS_IC_5861())
  {
     SCPIP_WRITE_FLD( SCPIP_SCCTRL1_0A , bOnOff, SCCTRL1_0A_SC_FRC_PUL_SEL_MIB);		   
  }
}  	
//in nromal mode: fsc dram read pointer select to FSC 
//in hybrid mode: fsc dram read pointer select to MIB
void vScpipFscDramReadPtrSel(UINT8 bOnOff)
{
  if (IS_IC_5861())
  {
    SCPIP_WRITE_FLD( SCPIP_SCCTRL1_0A , bOnOff, SCCTRL1_0A_SC_DRAM_RPTR_SEL_MIB);		   
  }
}  	

//0: normal mode
//1: MIB input 3D LR
void vScpipFscMainDispR_Sel(UINT8 bOnOff)
{
  SCPIP_WRITE_FLD( SCPIP_SCCTRL1_06 , bOnOff, SCSYS_06_SC_M_DISP_R_SEL);		   
}  	

//0: normal mode
//1:Main FSC input vsync from MIB
void vScpipFscMainVsync_Sel(UINT8 bOnOff)
{
	if (IS_IC_5861())
	{
       SCPIP_WRITE_FLD(  SCPIP_SCSYS_06   , bOnOff, SCSYS_06_SC_M_VSYNC_SEL);		   
	}
}  	


//dram_read_share mode
//0: hdmi 4k60    --> FSC sub is no use
//1: hybrid mode --> FSC sub is aviable in 4k30 mode
//2: main large   --> Mian path got 2/3 fifo  sub got 1/3 
//3:sub large      --> Main path have 1/3 fifo, sub got 2/3
void vScpipDramReadShareMode(UINT8 u1DramMode)
{
  SCPIP_WRITE_FLD( SCPIP_DRAM_M_00 , u1DramMode, DRAM_M_00_DA_RD_SHARE_MODE);		   
}  	

//dram_write_share mode
void vScpipDramWriteShareMode(UINT8 u1DramMode)
{
  SCPIP_WRITE_FLD( SCPIP_DRAM_M_00 , u1DramMode, DRAM_M_00_DA_WR_SHARE_MODE);		   
}  	


/**
 * @brief Set vcnt, read begin after writed vcnt lines in the same frame buffer.
 * @8098 porting done
 */
void vScpipSetReadPosGen(UINT8 bPath, UINT32 u4VCnt)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1E+getSCOffset(bPath), u4VCnt, SCCTRL1_1E_SC_RPT_GEN_VCNT_1);		
}

/**
 * @brief switch for color domain which scaler work on (only in MAIN path)
 * @param: u4OnOff 1: one-step 422 to 444 enable (SC work on 422 domain)
 *                 0: one-step 422 to 444 disable (SC work on 444 domain)
 * @8098 porting done
 */
void vScpipSet422Mode(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	if(bPath == SV_VP_MAIN)
	{
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, value, SCCTRL1_04_SC_H_422_MODE_EN_1);
	}
}

/**
 * @brief reorder UV sequence (only in MAIN path)
 * @param: u4Inverse 1: y0cr y1cb (when mirror)
 *                              	  0: y0cb y1cr
 * @8098 porting done
 */
void vScpipReorderUV(UINT32 u4Inverse)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u4Inverse, SCCTRL1_04_SC_REORDER_UV_INV_1);
}

/**
 * @brief set 422 mirror enable, if mirror made before scaler (ex. scaler front dram mode) 
 * @and scaler work in 422 domain, need enable this.
 * @8098 porting done
 */
void vScpipSet422MirrorEnable(UINT8 bPath, UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	if(bPath == SV_VP_MAIN)
	{
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, value, SCCTRL1_0A_SC_H_422_MIRROR_EN);
	}
}

/**
 * @brief select dram data format.
 * @param: u4Sel10b 1: 10bit / 0: 8bit 
 * @8098 porting done
 */
void vScpipSetDram10bFmt(UINT8 bPath, UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	if(bPath == SV_VP_MAIN)
	{
	    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B, value, SCCTRL1_1B_SC_DRAM_WR_10B_SEL_1);
	}
}

/**
 * @brief select dram data format
 * @param: u4Sel444 1: 444 / 0: 422
 * @8098 porting done
 */
void vScpipSetDram444Fmt(UINT8 bPath, UINT32 u4Sel444)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), u4Sel444, SCCTRL1_1B_SC_DRAM_W_444_1);
}

void vScpipSetFrontMode(UINT8 bPath, UINT32 u4Mode)
{    
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_13+getSCOffset(bPath), (u4Mode==SV_ON)?1:0, SCCTRL1_13_SC_FRN_DRAM_MODE_1);
}

void vScpipSetBackMode(UINT8 bPath, UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_13+getSCOffset(bPath), (u4Mode==SV_ON)?1:0, SCCTRL1_13_SC_BCK_DRAM_MODE_1);
}


/**
 * @brief enable 444 to 422 format transfer before DRAM
 * @param: u4Enable 0: original fmt to dram / 1: 444 to 422 enable
 * @8098 porting done
 */
void vScpipEnable444To422BeforeDram(UINT8 bPath, UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_13+getSCOffset(bPath), u4Enable, SCCTRL1_13_SC_DRAM_YUV2YC_EN_1);
}


/**
 * @brief enable 422 to 444 format transfer after DRAM
 * @param: u4Enable 0: original fmt from dram / 1: 422 to 444 enable
 * @8098 porting done
 */
void vScpipEnable422To444AfterDram(UINT8 bPath, UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_13+getSCOffset(bPath), u4Enable, SCCTRL1_13_SC_DRAM_YC2YUV_EN_1);
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_13+getSCOffset(bPath), u4Enable, SCCTRL1_13_SC_DRAM_YC2YUV_H_BOUND_EN_1);
}

/**
 * @brief enable 422 to 444 format transfer before CTI
 * @param: u4Enable 0: 422 to 444 enable / 1: 422 to 444 disable
 * @8098 porting done
 */
void vScpipEnable422To444BeforeCTI(UINT8 bPath, UINT32 u4Enable)
{
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCSYS_51, u4Enable, SCSYS_51_SC_M_CTI_SEL444)
					 : SCPIP_WRITE_FLD(SCPIP_SCSYS_51, u4Enable, SCSYS_51_SC_P_CTI_SEL444);		
}

/**
 * @brief enable 444 to 422 format transfer after CTI
 * @param: u4Enable 0: 444 to 422 enable / 1: 444 to 422 disable
 * @8098 porting done
 */
void vScpipEnable444To422AfterCTI(UINT8 bPath, UINT32 u4Enable)
{
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCSYS_1E, u4Enable, SCSYS_1E_SC_M_AFCTI_SEL444)
					 : SCPIP_WRITE_FLD(SCPIP_SCSYS_1E, u4Enable, SCSYS_1E_SC_P_AFCTI_SEL444);		
}

/**
 * @brief enable setting resolution import protection
 * @param: u4OnOff 1: ON / 0: OFF
 * @8098 porting done
 */
void vScpipSetResImportEnable(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;
	
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A+getSCOffset(bPath), value, SCCTRL1_0A_SC_SET_RES_IMPORT_ENABLE_1);
}

void vScpipDramImportOnOff(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), value, SCCTRL1_1B_SC_DRAM_IMPORT_ENABLE_1);	
}

void vScpipSetDramFrmDely(UINT8 bPath, UINT32 u4Delay)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), u4Delay, SCCTRL1_1B_SC_DRAM_FRM_DLY_1);
	LOG(3, "PATH(%d) SET DRAM FRM DLY TO(%d)\n",bPath,u4Delay);
}

void vScpipSetDramSpacing(UINT8 bPath, UINT32 u4Space)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_01+getDramOffset(bPath), u4Space, DRAM_M_01_DA_READ_REQ_SPACING);
}

//@8098 review the name of function
void vScpipBobVScalingOnOff(UINT32 u43DVUpOnOff, UINT32 u4FieldVUpOnOff, UINT32 u4EnableDiffInitPhase)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u43DVUpOnOff, SCCTRL1_04_SC_V_3D_EN);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_04, u4FieldVUpOnOff, SCCTRL1_04_SC_V_FIELD_EN);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_18, u4EnableDiffInitPhase, SCCTRL1_18_SC_V_PHASE_INIT_3D_FIELD_EN);
}

void vScpipSetBobBtmLeftInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_18, u4InitSign, SCCTRL1_18_SC_V_PHASE_INIT_SIGN_BTM_LEFT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_18, u4InitIntg, SCCTRL1_18_SC_V_PHASE_INIT_INTG_BTM_LEFT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_18, u4InitFrac, SCCTRL1_18_SC_V_PHASE_INIT_FRAC_BTM_LEFT_1);	
}

/**
 * @brief dram write freeze,  if scaler is not in dram mode, this function should fail.
 * @8098 porting done
 */
void vScpipDramWriteFreeze(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;
	
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1B, value, SCCTRL1_1B_SC_DRAM_W_FREEZE_1);	
}


void vScpipSetBobTopRightInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_19, u4InitSign, SCCTRL1_19_SC_V_PHASE_INIT_SIGN_TOP_RIGHT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_19, u4InitIntg, SCCTRL1_19_SC_V_PHASE_INIT_INTG_TOP_RIGHT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_19, u4InitFrac, SCCTRL1_19_SC_V_PHASE_INIT_FRAC_TOP_RIGHT_1);	
}

void vScpipSetBobBtmRightInitPhase(UINT32 u4InitSign, UINT32 u4InitIntg, UINT32 u4InitFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1A, u4InitSign, SCCTRL1_1A_SC_V_PHASE_INIT_SIGN_BTM_RIGHT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1A, u4InitIntg, SCCTRL1_1A_SC_V_PHASE_INIT_INTG_BTM_RIGHT_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_1A, u4InitFrac, SCCTRL1_1A_SC_V_PHASE_INIT_FRAC_BTM_RIGHT_1);	
}


/**
 * @brief Nonlinear scaling on off (only in MAIN path)
 * @param: u4OnOff 1: ON / 0: OFF
 * 		     u4AddMinusSel: 1: side delta phase < middle delta phase 
 *						   0: side delta phase > middle delta phase
 * @8098 porting done
 */
void vScpipNonlinearOnOff(UINT32 u4OnOff, UINT32 u4AddMinusSel)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_05, u4OnOff, SCCTRL1_05_SC_NONL_H_ENABLE);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_05, u4AddMinusSel, SCCTRL1_05_SC_NONL_H_ADD_MINUS_SEL);		
}

void vScpipSetNonlinearPos(UINT32 u4StartPos, UINT32 u4StopPos, UINT32 u4MiddlePos)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_05, u4StartPos, SCCTRL1_05_SC_NONL_H_START_POS);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_05, u4StopPos, SCCTRL1_05_SC_NONL_H_STOP_POS);	
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_07, u4MiddlePos, SCCTRL1_07_SC_NONL_H_MIDDLE_POS);		
}

/**
 * @brief set Nonlinear DeltaPhase increament on slope
 * @8098 porting done
 */
void vScpipSetNonlinearDeltaPhase(UINT32 u4DeltaIntg, UINT32 u4DeltaFrac)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_06, u4DeltaIntg, SCCTRL1_06_SC_NONL_H_PHASE_DELTA);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_06, u4DeltaFrac, SCCTRL1_06_SC_NONL_H_PHASE_DELTA_FLOAT);		
}


void vScpipSetDispFifoWaterLevel(UINT8 bPath, UINT32 u4Level)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0B+getSCOffset(bPath), u4Level, SCCTRL1_0B_SC_DISP_FF_WTLV_1);
}

/**
 * @brief set Scaler 3D mode
 * @param u4Mode 0:2d / 2:frame packing / 3:frame seq / 4:t&b / 8:s by s / c:line interleave
 * @8098 porting done
 */
void vScpipSetSC3DMode(UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_05, u4Mode, SCCTRL1_05_SC_3D_MODE);	
}

void vScpipSetDispRFreerun(UINT32 u4OnOff)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, ((u4OnOff==SV_ON)?1:0), SCSYS_01_SC_TG_DISP_R_FREERUN_EN);	
}

void vScpipSetMibDispRMode(UINT32 u4Mode)
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0C, u4Mode, SCCTRL1_0C_SC_MSK_DISPFF_MIB_DISP_R);	
}


/**
 * @brief main dram read request after delay
 */
void vScpipSetDramReadReqDly(UINT8 bPath, UINT32 u4Delay)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0B+getSCOffset(bPath), u4Delay, SCCTRL1_0B_SC_DRAM_RDREQ_DLY_1);
}

/***
  * @ sc_msk_dispff_mib_disp/ sc_dram_3d_enable_1 / sc_eng_input_end_dly_1 to be added
  * @ scctrl?_atpg_ct / scctrl?_atpg_ob
  */

// Time Gen 
void vScpipSetData(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_SC_SET_DATA);
}

void vScpipSetTgImportOnOff(UINT32 u4OnOff)
{
	UINT8 value;
	#ifdef CC_SUPPORT_UHD
	value=1;
	#else
    value = (u4OnOff==SV_ON)?1:0;
	#endif

	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_SC_TG_IMPORT_ENABLE);	
}

void vScpipSetTgImportVCnt(UINT32 u4Vcunt)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, u4Vcunt, SCSYS_01_SC_TG_IMPORT_VCNT);	
}

#if defined(CC_MT5890) 
void vScpipRefSetData(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, value, SCSYS_6D_REF_SET_DATA);
}

void vScpipRefSetTgImportOnOff(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_00, value, SCSYS_00_REF_TG_IMPORT_ENABLE);	
}

void vScpipRefSetTgImportVCnt(UINT32 u4Vcnt)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Vcnt, SCSYS_6D_REF_TG_IMPORT_VCNT);	
}

void vScpipGfxSetData(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_65, value, SCSYS_65_GFX_SET_DATA);
}

void vScpipGfxSetTgImportOnOff(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_GFX_TG_IMPORT_ENABLE);	
}

void vScpipGfxSetTgImportVCnt(UINT32 u4Vcnt)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_65, u4Vcnt, SCSYS_65_GFX_TG_IMPORT_VCNT);	
}

void vScpipMuxSetTgImportOnOff(UINT32 u4OnOff)
{
	UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_MUX_TG_IMPORT_ENABLE);	
}
#endif

/**
 * @brief set Main Mask Start (like RHP/RVP)
 * @8098 porting done
 */
void vScpipSetMaskStart(UINT8 bPath, UINT32 u4Hofst, UINT32 u4VOfst)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_25+getSCOffset(bPath), u4Hofst, SCCTRL1_25_SC_HMSK_STR_OFST_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_27+getSCOffset(bPath), u4VOfst, SCCTRL1_27_SC_VMSK_STR_OFST_1);
}

void vScpipSetVideoPixelShift(UINT8 bPath, UINT32 u4Hofst, UINT32 u4VOfst)
{
    UINT32 u4OutputWidth,u4OutputHeight;
    
    u4OutputWidth=SCPIP_READ_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath),  SCCTRL1_11_SC_OUTPUT_WIDTH_1);
	u4OutputHeight=SCPIP_READ_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath), SCCTRL1_11_SC_OUTPUT_HEIGHT_1);
    
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_25+getSCOffset(bPath), u4Hofst, SCCTRL1_25_SC_HMSK_STR_OFST_1);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_27+getSCOffset(bPath), u4VOfst, SCCTRL1_27_SC_VMSK_STR_OFST_1);

    if(u4OutputWidth-u4Hofst > 0)
    {
        SCPIP_WRITE_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath), u4OutputWidth-u4Hofst, SCCTRL1_11_SC_OUTPUT_WIDTH_1);
    }
    if (u4OutputHeight-u4VOfst > 0)
    {
        SCPIP_WRITE_FLD(SCPIP_SCCTRL1_11+getSCOffset(bPath), u4OutputHeight-u4VOfst, SCCTRL1_11_SC_OUTPUT_HEIGHT_1);
    }
    
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 1, SCCTRL1_0A_SC_SET_RES_TOGGLE_1);
	x_thread_delay(100);
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 0, SCCTRL1_0A_SC_SET_RES_TOGGLE_1);
    
}

void vScpipSetSubMaskDelay(UINT32 u4HMaskDelay, UINT32 u4VMaskDelay)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_13, u4HMaskDelay, SCSYS_13_SC_P_HMSK_DLY);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_23, u4VMaskDelay, SCSYS_23_SC_P_VMSK_DLY);		
}

void vScpipUpdateDelay(void)
{
    UINT16 hmsk_dly = SUB_HMSK_DLY;
    UINT16 vmsk_dly = SUB_VMSK_DLY;

#ifndef CC_MTK_LOADER
    UINT16 byp_delay_tab[BYPASS_MAX][2] =
    {
        { 59, 3}, //BYPASS_SHARPNESS
        { 27, 1}, //BYPASS_CBE_CTL
        { 21, 0}, //BYPASS_BITPLUS_Y
        { 21, 0}, //BYPASS_BITPLUS_S
        { 4,  0}, //BYPASS_3DCOLORMAP
        { 3,  5}, //BYPASS_SRS
    };

    if(u1DrvSharpnessIsBypass())
    {
        LOG(6, "DBG u1DrvSharpnessIsBypass\n");
        hmsk_dly -= byp_delay_tab[BYPASS_SHARPNESS][0];
        vmsk_dly -= byp_delay_tab[BYPASS_SHARPNESS][1];
    }

    if(u1DrvCBEIsBypass())
    {
        LOG(6, "DBG u1DrvCBEIsBypass\n");
        hmsk_dly -= byp_delay_tab[BYPASS_CBE_CTL][0];
        vmsk_dly -= byp_delay_tab[BYPASS_CBE_CTL][1];
    }

    if(u1DrvBitPlusYIsBypass())
    {
        LOG(6, "DBG u1DrvBitPlusYIsBypass\n");
        hmsk_dly -= byp_delay_tab[BYPASS_BITPLUS_Y][0];
        vmsk_dly -= byp_delay_tab[BYPASS_BITPLUS_Y][1];
    }

    if(u1DrvBitPlusSIsBypass())
    {
        LOG(6, "DBG u1DrvBitPlusSIsBypass\n");
        hmsk_dly -= byp_delay_tab[BYPASS_BITPLUS_S][0];
        vmsk_dly -= byp_delay_tab[BYPASS_BITPLUS_S][1];
    }

    if(u1Drv3DCMSIsBypass())
    {
        LOG(6, "DBG u1Drv3DCMSIsBypass\n");
        hmsk_dly -= byp_delay_tab[BYPASS_3DCOLORMAP][0];
        vmsk_dly -= byp_delay_tab[BYPASS_3DCOLORMAP][1];
    }

    if(0) // c_srs_bypass F0033504[1] = 1: bypass
    {
        LOG(6, "DBG BYPASS_SRS\n");
        hmsk_dly -= byp_delay_tab[BYPASS_SRS][0];
        vmsk_dly -= byp_delay_tab[BYPASS_SRS][1];
    }

    if (u4DrvTDTVModelWithoutMJC())
    {
        LOG(6, "DBG TTD at Post\n");
        hmsk_dly += 288;
    }
#endif

    LOG(6, "Sub Delay %d %d\n", hmsk_dly, vmsk_dly);
    vScpipSetSubMaskDelay(hmsk_dly, vmsk_dly);
}

/**
 * @brief scaler set panel width
 * @8098 porting done
 */
void vScpipSetPanelWidth(E_TG_MODULE eModule, UINT32 u4Width)
{  
    switch ( eModule )
    {
       case E_FSC_TG:
#ifndef CC_MTK_LOADER	   		   		   	
#ifdef CC_SUPPORT_UHD	   	
  	    	vUhdSetPanelWidth(u4Width);
			u4Width=u4Width>>1;
#endif		   	
#endif
          SCPIP_WRITE_FLD(SCPIP_SCSYS_02, u4Width, SCSYS_02_SC_PNL_WIDTH);
	      SCPIP_WRITE_FLD(SCPIP_SCSYS_49, u4Width, SCSYS_49_SC_SET_DATA_HACT);	
       break;
#ifdef CC_MT5890              
       case  E_GFX_TG:        
          SCPIP_WRITE_FLD(SCPIP_SCSYS_61, u4Width, SCSYS_61_GFX_PNL_WIDTH);
          SCPIP_WRITE_FLD(SCPIP_SCSYS_7A, u4Width, SCSYS_7A_GFX_SET_DATA_HACT);
       break;
       case  E_REF_TG:        
          SCPIP_WRITE_FLD(SCPIP_SCSYS_4A, u4Width, SCSYS_4A_REF_PNL_WIDTH);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
#endif       
    }
}
/**
 * @brief scaler set panel height
 */
void vScpipSetPanelHeight(E_TG_MODULE eModule, UINT32 u4Height)
{ 
    switch ( eModule )
    {
       case E_FSC_TG:
#ifndef CC_MTK_LOADER	   		   		   	
#ifdef CC_SUPPORT_UHD	   	
		   	vUhdSetPanelHeight(u4Height);
#endif
#endif
         SCPIP_WRITE_FLD(SCPIP_SCSYS_02, u4Height, SCSYS_02_SC_PNL_HEIGHT);
       break;       
#ifdef CC_MT5890       
       case E_GFX_TG:        
          SCPIP_WRITE_FLD(SCPIP_SCSYS_61, u4Height, SCSYS_61_GFX_PNL_HEIGHT);
       break;
       case E_REF_TG:
           SCPIP_WRITE_FLD(SCPIP_SCSYS_4A, u4Height, SCSYS_4A_REF_PNL_HEIGHT);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
    }
}

/**
 * @brief scaler set panel htotal
 */
void vScpipSetPanelHtotal(E_TG_MODULE eModule, UINT32 u4HTotal)
{    
    switch ( eModule )
    {
       case E_FSC_TG:   
#ifndef CC_MTK_LOADER	   		   		   	
#ifdef CC_SUPPORT_UHD	   	
		    vUhdSetPanelHtotal(u4HTotal);
		 	u4HTotal=u4HTotal>>1;
#endif
#endif
	       SCPIP_WRITE_FLD(SCPIP_SCSYS_03, u4HTotal, SCSYS_03_SC_PNL_HTOTAL);
	       SCPIP_WRITE_FLD(SCPIP_SCSYS_49, (u4HTotal-1), SCSYS_49_SC_SET_DATA_HTOT);		
       break;
#ifdef CC_MT5890       
       case E_GFX_TG:
          SCPIP_WRITE_FLD(SCPIP_SCSYS_62, u4HTotal, SCSYS_62_GFX_PNL_HTOTAL);
          SCPIP_WRITE_FLD(SCPIP_SCSYS_6B, (u4HTotal-1), SCSYS_6B_GFX_SET_DATA_HTOT);
       break;
       case E_REF_TG:        
          SCPIP_WRITE_FLD(SCPIP_SCSYS_4B, u4HTotal, SCSYS_4B_REF_PNL_HTOTAL);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
    }
}

/**
 * @brief scaler set panel vtotal
 */
void vScpipSetPanelVtotal(E_TG_MODULE eModule,UINT32 u4VTotal )
{
    switch ( eModule )
    {
       case E_FSC_TG:
#ifndef CC_MTK_LOADER	   		   		   	
#ifdef CC_SUPPORT_UHD	   	
			 	vUhdSetPanelVtotal(u4VTotal);
#endif
#endif
         SCPIP_WRITE_FLD(SCPIP_SCSYS_03, u4VTotal, SCSYS_03_SC_PNL_VTOTAL);
	     SCPIP_WRITE_FLD(SCPIP_SCSYS_50, (u4VTotal-1), SCSYS_50_SC_SET_DATA_VTOT);	
       break;
#ifdef CC_MT5890       
       case E_GFX_TG:
           SCPIP_WRITE_FLD(SCPIP_SCSYS_62, u4VTotal, SCSYS_62_GFX_PNL_VTOTAL);    
           SCPIP_WRITE_FLD(SCPIP_SCSYS_6B, (u4VTotal-1), SCSYS_6B_GFX_SET_DATA_VTOT);
       break;
       case E_REF_TG:        
           SCPIP_WRITE_FLD(SCPIP_SCSYS_4B, u4VTotal, SCSYS_4B_REF_PNL_VTOTAL);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
    }
}

/**
 * @brief scaler set panel hsync back porch
 * @8098 porting done
 */
 
void vScpipSetHsyncBp(E_TG_MODULE eModule,UINT32 u4BckPorch)
{
    switch ( eModule )
    {
       case E_FSC_TG:
#ifndef CC_MTK_LOADER	   	
#ifdef CC_SUPPORT_UHD	   	
		    vUhdSetOutputPanelHBackporch(u4BckPorch);
		 	u4BckPorch=u4BckPorch>>1;
#endif		 	
#endif
         SCPIP_WRITE_FLD(SCPIP_SCSYS_04, u4BckPorch, SCSYS_04_SC_PNL_BACK_HPORCH);
       break;
#ifdef CC_MT5890             
       case E_GFX_TG:
         SCPIP_WRITE_FLD(SCPIP_SCSYS_63, u4BckPorch, SCSYS_63_GFX_PNL_BACK_HPORCH);
       break;
       case E_REF_TG:
         SCPIP_WRITE_FLD(SCPIP_SCSYS_4C, u4BckPorch, SCSYS_4C_REF_PNL_BACK_HPORCH);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
     }
}

/**
 * @brief scaler set panel hsync front porch
 */
void vScpipSetHsyncFp(E_TG_MODULE eModule, UINT32 u4FrnPorch)
{
    switch ( eModule )
    {
       case E_FSC_TG:
#ifndef CC_MTK_LOADER	   		   	
#ifdef CC_SUPPORT_UHD	   	
		    vUhdSetOutputPanelHFrontporch(u4FrnPorch);
			u4FrnPorch=u4FrnPorch>>1;
#endif			
#endif
         SCPIP_WRITE_FLD(SCPIP_SCSYS_04, u4FrnPorch, SCSYS_04_SC_PNL_FRNT_HPORCH);  
         break;
#ifdef CC_MT5890             
       case E_GFX_TG:
          SCPIP_WRITE_FLD(SCPIP_SCSYS_63, u4FrnPorch, SCSYS_63_GFX_PNL_FRNT_HPORCH);
       break;
       case E_REF_TG:          
          SCPIP_WRITE_FLD(SCPIP_SCSYS_4C, u4FrnPorch, SCSYS_4C_REF_PNL_FRNT_HPORCH);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
   }
}

/**
 * @brief scaler set panel vsync back porch
 */
void vScpipSetVsyncBp(E_TG_MODULE eModule, UINT32 u4BckPorch)
{
    switch ( eModule )
    {
       case E_FSC_TG: 	
	   	
#ifndef CC_MTK_LOADER
#ifdef CC_SUPPORT_UHD
  		    vUhdSetOutputPanelVBackporch(u4BckPorch);
#endif
#endif
	     SCPIP_WRITE_FLD(SCPIP_SCSYS_05, u4BckPorch, SCSYS_05_SC_PNL_BACK_VPORCH);
         break;        
#ifdef CC_MT5890             
       case E_GFX_TG:
          SCPIP_WRITE_FLD(SCPIP_SCSYS_64, u4BckPorch, SCSYS_64_GFX_PNL_BACK_VPORCH);
       break;
       case E_REF_TG:       
          SCPIP_WRITE_FLD(SCPIP_SCSYS_4D, u4BckPorch, SCSYS_4D_REF_PNL_BACK_VPORCH);
       break;
	   case  E_PSC_TG:                       //need to review use
	   case  E_ALL_TG:
		LOG(4, "in E_PSC_TG E_ALL_TG case \n");
	   break;	
	   
#endif       
    }
}

/**
 * @brief scaler set panel vsync front porch
 */
void vScpipSetVsyncFp(E_TG_MODULE eModule, UINT32 u4FrnPorch)
{
    switch ( eModule )
    {
        case E_FSC_TG:
#ifndef CC_MTK_LOADER			
#ifdef CC_SUPPORT_UHD
		  vUhdSetOutputPanelVFrontporch(u4FrnPorch);
#endif			
#endif
	      SCPIP_WRITE_FLD(SCPIP_SCSYS_05, u4FrnPorch, SCSYS_05_SC_PNL_FRNT_VPORCH);
        break;
#ifdef CC_MT5890             
        case E_GFX_TG:
            SCPIP_WRITE_FLD(SCPIP_SCSYS_64, u4FrnPorch, SCSYS_64_GFX_PNL_FRNT_VPORCH);
        break;
        case E_REF_TG:
            SCPIP_WRITE_FLD(SCPIP_SCSYS_4D, u4FrnPorch, SCSYS_4D_REF_PNL_FRNT_VPORCH);
        break;
		case  E_PSC_TG: 					  //need to review use
		case  E_ALL_TG:
		 LOG(4, "in E_PSC_TG E_ALL_TG case \n");
		break;	 
		
#endif
    }      
}

/**
 * @brief scaler get panel hsync back porch
 * @8098 porting done
 */
UINT32 u4ScpipGetHsyncBp(E_TG_MODULE eModule)
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
    		return vUhdGetOutputPanelHBackporch();
		}
		else
	#endif
    #endif
		{
    		return SCPIP_READ_FLD(SCPIP_SCSYS_04,SCSYS_04_SC_PNL_BACK_HPORCH);  
		}
    }
#ifdef CC_MT5890                
    else if ( eModule == E_GFX_TG )        
       return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_63,  SCSYS_63_GFX_PNL_BACK_HPORCH);
    else if ( eModule == E_REF_TG )        
       return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4C,  SCSYS_4C_REF_PNL_BACK_HPORCH);
#endif    
    else
        return 0;
}

/**
 * @brief scaler get panel hsync front porch
 */
UINT32 u4ScpipGetHsyncFp(E_TG_MODULE eModule)
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return vUhdGetOutputPanelHFrontporch();
		}
		else
	#endif
	#endif
		{
	    	return SCPIP_READ_FLD(SCPIP_SCSYS_04, SCSYS_04_SC_PNL_FRNT_HPORCH);
		}
    }
#ifdef CC_MT5890                 
    else if ( eModule == E_GFX_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_63,  SCSYS_63_GFX_PNL_FRNT_HPORCH);
    else if ( eModule == E_REF_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4C,  SCSYS_4C_REF_PNL_FRNT_HPORCH);        
#endif    
    else
        return 0;
}

/**
 * @brief scaler get panel vsync back porch
 */
UINT32 u4ScpipGetVsyncBp(E_TG_MODULE eModule)
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return vUhdGetOutputPanelVBackporch();
		}
		else
	#endif
	#endif
		{
        	return SCPIP_READ_FLD(SCPIP_SCSYS_05, SCSYS_05_SC_PNL_BACK_VPORCH);
		}
    }
#ifdef CC_MT5890             
    else if ( eModule == E_GFX_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_64,  SCSYS_64_GFX_PNL_BACK_VPORCH);
    else if ( eModule == E_REF_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4D,  SCSYS_4D_REF_PNL_BACK_VPORCH);        
#endif    
    else 
        return 0;
}

/**
 * @brief scaler get panel vsync front porch
 */
UINT32 u4ScpipGetVsyncFp(E_TG_MODULE eModule)
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return vUhdGetOutputPanelVFrontporch();
		}
	#endif
	#endif
		{
        	return SCPIP_READ_FLD(SCPIP_SCSYS_05, SCSYS_05_SC_PNL_FRNT_VPORCH);
		}
    }
#ifdef CC_MT5890             
    else if ( eModule == E_GFX_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_64,  SCSYS_64_GFX_PNL_FRNT_VPORCH);
    else if ( eModule == E_REF_TG )        
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4D,  SCSYS_4D_REF_PNL_FRNT_VPORCH);        
#endif    
    else
        return 0;
}

void vScpipSetDramRdreqDly(UINT8 bPath, UINT32 u4Delay)
{
	(bPath==SV_VP_MAIN) ? SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u4Delay, SCSYS_06_SC_DRAM_RDREQ_DLY_1)
					 : SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u4Delay, SCSYS_06_SC_DRAM_RDREQ_DLY_2);				
}



// display mode
/**
 * @brief scaler line sync mode on/off (main only)
 */
void vScpipLinesyncModeOnOff(UINT32 u4OnOff)
{
#if defined(CC_MT5890) 
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;
    
    if(u4OnOff == SV_ON)
    {
        //disable import
#ifdef CC_MTK_LOADER   
        vScpipSetResImportEnable(SV_VP_MAIN, SV_OFF);
#else   
        vScpipSetImportEnable(SV_VP_MAIN, SV_OFF);
#endif
        vScpipSetTgImportOnOff(SV_OFF);
        //set linesync mode
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_SC_LINESYNC_MODE);    
        //set front/back to 0
        vScpipSetFrontMode(SV_VP_MAIN, SV_OFF);
    	vScpipSetBackMode(SV_VP_MAIN, SV_OFF);
        //enable tg import
#ifdef CC_MTK_LOADER   
	   vScpipSetResImportEnable(SV_VP_MAIN, SV_ON);
#else    
        vScpipSetImportEnable(SV_VP_MAIN, SV_ON);
#endif
        vScpipSetTgImportOnOff(SV_ON);
    }
    else
    {
        //set linesync mode
        SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_SC_LINESYNC_MODE);
    }
#endif
}

/**
 *  @brief frame track target distance
 */
void vScpipSetFrmTrkLines(UINT32 u4Lines)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_10, u4Lines, SCSYS_10_SC_FRM_TRK_LINE);
}

UINT16 vScpipGetFrmTrkLines(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_10, SCSYS_10_SC_FRM_TRK_LINE);
}

void vScpipSetFrmTrkFrames(UINT32 u4Frames)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_10, u4Frames, SCSYS_10_SC_FRM_MASK_I);
}

/**
 *  @brief DDDS track signal inverse (fast/slow switch)
 */
void vScpipSetDddsTrkInv(UINT32 u4Inv)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_11, u4Inv, SCSYS_11_SC_DDDS_TRK_INV);
}

/**
 *  @brief enable DDDS track mechanism
 */
void vScpipFrmTrkDddsEnable(UINT32 u4Enable)
{
    UINT8 value;
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_11, value, SCSYS_11_SC_FRM_TRK_DDDS_EN);	
}
/**
*  DDDS control from
   0: FSC TG
   1: PSC TG
   2: REF TG  <-- in 4k model set to ref_tg
*/
void vScpipFrmTrkDddsSrcSel(UINT32 u4SrcSel)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u4SrcSel, SCSYS_06_SC_DDDS_SRC_SEL);	
}



/**
 * @brief select frame track referenced vsync
 * @param u4Option 0: track mib input vsync / 1: track scaler input vsync /2: hdmi input /3: reserved
 */
void vScpipSelFrmTrkRefVsync(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_11, u4Option, SCSYS_11_SC_FRM_TRK_IN_SEL);	
}

/**
 * @brief frame track result applied to next frame only once
 * @param u4Option 0:normal / 1: only once
 */
void vScpipFrmTrkResApply(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_11, u4Option, SCSYS_11_SC_FRM_TRK_ONCE_ONLY);	
}

/**
 * @brief use ddds to track frame with turbo speed
 */
void vScpipSetDddsTurboSpeed(UINT32 u4Speed)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_14, u4Speed, SCSYS_14_SC_DDDS_TURBO_RGN);
}

/**
 * @brief set frame track step (unit: line)
 * @when step is zero, frame track off
 */
void vScpipSetFrmTrkStep(UINT32 u4Step)
{
#ifndef CC_MTK_LOADER
  #ifdef	USE_FRAME_TRACK_WA
  	  u4FrmTrkStep=u4Step;
  #else
     SCPIP_WRITE_FLD(SCPIP_SCSYS_14, u4Step, SCSYS_14_SC_FRM_TRK_STEP);
  #endif
#else
    SCPIP_WRITE_FLD(SCPIP_SCSYS_14, u4Step, SCSYS_14_SC_FRM_TRK_STEP);
#endif  
}

UINT8 bScpipGetFrmTrkStep(void)
{
	return SCPIP_READ_FLD(SCPIP_SCSYS_14, SCSYS_14_SC_FRM_TRK_STEP);	
}


#ifdef CC_MT5890  
/*
 mask some frames if in/out frame rat eis difference
*/
void vScpipRefSetFrmTrkFrames(UINT32 u4Frames)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6E, u4Frames, SCSYS_6E_REF_FRM_MASK_I);
}
/**
 *  @brief frame track target distance
 */
void vScpipRefSetFrmTrkLines(UINT32 u4Lines)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6E, u4Lines, SCSYS_6E_REF_FRM_TRK_LINE);
}
UINT16 vScpipRefGetFrmTrkLines(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_6E, SCSYS_6E_REF_FRM_TRK_LINE);
}

/*
0: hdmi_rx ---   new added for by_pass path HDMI input 4k2K signal
1: mib input -- - for orginal FSC sel
2: mib output ---for orginal FSC sel
3. mjc input ---  for orginal PSC sel
*/
void vScpipRefSelFrmTrkRefVsync(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Option, SCSYS_6D_REF_FRM_TRK_IN_SEL);	
}

void vScpipRefSetFrmTrkStep(UINT32 u4Step)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6F, u4Step, SCSYS_6F_REF_FRM_TRK_STEP);	
}

UINT8 bScpipRefGetFrmTrkStep(void)
{
	return SCPIP_READ_FLD(SCPIP_SCSYS_6F, SCSYS_6F_REF_FRM_TRK_STEP);	
}
#endif


/**
 * @brief when frame track lock, how many lines are tolerated.
 */
void vScpipSetFrmTrkLockTor(UINT32 u4Lines)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_15, u4Lines, SCSYS_15_SC_FRM_LOCK_TOR);	
}

/**
 * @brief when frame track lock, how many lines are tolerated.
 */
void vScpipSetFrmTrkLockWin(UINT32 u4Width)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_15, u4Width, SCSYS_15_SC_FRM_LOCK_WIN);
}
/*
  FSC frame trk cross vsync
  0: frame tracking not cross vsync
  1: frame tracking in shortest path
*/
void vScpipFrmTrkCrossVsync(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0F, u4Enable, SCSYS_0F_SC_FRM_TRK_CROSS_VSYNC_EN );
}

/*
FSC Frm trk @ 3d mode
*/
void vScpipFrmTrk3DEnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0F, u4Enable, SCSYS_0F_SC_FRM_TRK_3D_EN);
}
/*
frm trk lock @
  0: Left
  1: Right
*/
void vScpipFrmTrkDispR(UINT32 u4FrmtrkLockat)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0F, u4FrmtrkLockat, SCSYS_0F_SC_FRM_TRK_DISP_R);
}
/**
 * @brief vScpipFrmTrkInLREnable
 * @param u4Enable
 */
void vScpipFrmTrkMaskInLREnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_10, u4Enable, SCSYS_10_SC_FRM_MASK_IN_LR_EN);
}

/*
input frm mask function for 
   0: R_FRAME
   1: L_FRAME
*/
void vScpipFrmTrkIMaskInLRSel(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_10, u4Enable, SCSYS_10_SC_FRM_MASK_IN_LR_SEL);
}

#ifdef CC_MT5890
void vScpipRefFrmTrkDddsEnable(UINT32 u4Enable)
{
    UINT8 value;
    value = (u4Enable==SV_ON)?1:0;
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, value, SCSYS_6D_REF_FRM_TRK_DDDS_EN );	
}

/*
  REF frame trk cross vsync
  0: frame tracking not cross vsync
  1: frame tracking in shortest path
*/
void vScpipRefFrmTrkCrossVsync(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Enable, SCSYS_6D_REF_FRM_TRK_CROSS_VSYNC_EN );
}

/*
REF Frm trk @ 3d mode  DDDS Mode only ...
*/
void vScpipRefFrmTrk3DEnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Enable, SCSYS_6D_REF_FRM_TRK_3D_EN);
}
/*
REF  trk lock @
  0: Left
  1: Right
*/
void vScpipRefFrmTrkDispR(UINT32 u4FrmtrkLockat)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4FrmtrkLockat, SCSYS_6D_REF_FRM_TRK_DISP_R);
}
/**
 * @brief vScpipFrmTrkInLREnable
 * @param u4Enable
 */
void vScpipRefFrmTrkMaskInLREnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Enable, SCSYS_6D_REF_FRM_MASK_OUT_LR_EN);
}

/*
input frm mask function for 
   0: R
   1: L
*/
void vScpipRefFrmTrkMaskInLRSel(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Enable, SCSYS_6D_REF_FRM_MASK_OUT_LR_SEL);
}

/**
 * @brief vsync in edge sel
 * @param u4Option 0: fall / 1: rise
 */
void vScpipRefFrmTrkVsyncInEdge(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_6D, u4Option, SCSYS_6D_REF_FRM_TRK_VIN_EDGE_SEL);
}

#endif  //cc_MT5890

/**
 * @brief vsync in edge sel
 * @param u4Option 0: fall / 1: rise
 */
void vScpipSelFrmTrkVsyncInEdge(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_11, u4Option, SCSYS_11_SC_FRM_TRK_VIN_EDGE_SEL);
}

/*Frame mask , disp R control, force input field needed to added */


// DRAM
void vScpipSetFlipMirror(UINT8 bPath, UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+getDramOffset(bPath), u4Mode, DRAM_M_00_DA_DISPLAY_MODE);
}

void vScpipSetDramLinePitch(UINT32 u4VdpId, UINT32 u4Pitch)
{
    if (u4Pitch > 0)
    {
        u4Pitch--;
    }
    
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+getDramOffset(getScalerPath(u4VdpId)), u4Pitch, DRAM_M_00_DA_DRAM_LINE_PITCH);	
}

void vScpipSetBufNum(UINT32 u4VdpId, UINT32 u4BufNum)
{
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);

    if (u4BufNum < 1 || u4BufNum > 4)
        return;

    SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+dramOffset, u4BufNum-1, DRAM_M_00_DA_FRAME_NUM);
}

void vScpipSetDramWrite(UCHAR u4VdpId, UCHAR ucOnOff)
{
    UINT8 bPath;
    UINT16 dramOffset;

    if (u4VdpId >= VDP_NS)
    {
        return;
    }

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);

    if(ucOnOff == SV_ON)
    {    
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+dramOffset, 1, DRAM_M_00_DA_WRITE_ENABLE);
    }
    else
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+dramOffset, 0, DRAM_M_00_DA_WRITE_ENABLE);
    }
    LOG(9, "vScpipSetDramWrite(Path:%d, OnOff %d)\n", bPath,ucOnOff );
}

void vScpipSetDramRead(UCHAR u4VdpId, UCHAR ucOnOff)
{
    UINT8 bPath;
    UINT16 dramOffset;

    if (u4VdpId >= VDP_NS)
    {
        return;
    }

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);    
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+dramOffset, ucOnOff, DRAM_M_00_DA_READ_ENABLE);
	
    LOG(9, "vScpipSetDramRead(Path:%d, OnOff:%d)\n", bPath,ucOnOff);
}

void vScpipSetDramSnakeMap(UINT8 bPath, UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+getDramOffset(bPath), value, DRAM_M_00_DA_SNAKE_MAPPING);			
}
UINT32 vScpipGetDramSnakeMap(UINT8 bPath)
{	
	return (UINT32)SCPIP_READ_FLD(SCPIP_DRAM_M_00+getSCOffset(bPath), DRAM_M_00_DA_SNAKE_MAPPING);
}

void vScpipWriteRelatchEn(UINT32 u4VdpId, UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_00+getDramOffset(getScalerPath(u4VdpId)), value, DRAM_M_00_DA_WRITE_EN_RELATCH);	
}

void vScpipSetYBaseAddr(UINT8 bPath, UINT32 u4Msb, UINT32 u4Lsb)
{
	UINT16 u2DramOffset = getDramOffset(bPath);
	
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_04+u2DramOffset, u4Msb, DRAM_M_04_DA_ADDR_BASE_MSB_Y);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_05+u2DramOffset, u4Lsb, DRAM_M_05_DA_ADDR_BASE_LSB_Y);
}

void vScpipSetUBaseAddr(UINT8 bPath, UINT32 u4Msb, UINT32 u4Lsb)
{
	UINT16 u2DramOffset = getDramOffset(bPath);
	
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_06+u2DramOffset, u4Msb, DRAM_M_06_DA_ADDR_BASE_MSB_U);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_07+u2DramOffset, u4Lsb, DRAM_M_07_DA_ADDR_BASE_LSB_U);
}

void vScpipSetVBaseAddr(UINT8 bPath, UINT32 u4Msb, UINT32 u4Lsb)
{
	UINT16 u2DramOffset = getDramOffset(bPath);

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_08+u2DramOffset, u4Msb, DRAM_M_08_DA_ADDR_BASE_MSB_V);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_09+u2DramOffset, u4Lsb, DRAM_M_09_DA_ADDR_BASE_LSB_V);
}

void vScpipSetLimitAddr(UINT8 bPath, UINT32 u4HiLimit, UINT32 u4LoLimit)
{
    UINT16 u2DramOffset = getDramOffset(bPath);

    if (u4HiLimit != 0)
    {
	    SCPIP_WRITE_FLD(SCPIP_DRAM_M_0A+u2DramOffset, u4HiLimit, DRAM_M_0A_DA_WADDR_HI_LIMIT);
    }

    if (u4LoLimit != 0xFFFFFFFF)
    {
	    SCPIP_WRITE_FLD(SCPIP_DRAM_M_0B+u2DramOffset, u4LoLimit, DRAM_M_0B_DA_WADDR_LO_LIMIT);
    }
}

UINT32 u4ScpipGetDramLimit(UINT8 bPath, UINT8 u1Opt)
{
    UINT16 u2DramOffset = getDramOffset(bPath);
    UINT32 u4AddrLimit;

    if (u1Opt == 0)
    {
        u4AddrLimit = SCPIP_READ_FLD(SCPIP_DRAM_M_0B+u2DramOffset, DRAM_M_0B_DA_WADDR_LO_LIMIT);
    }
    else
    {
        u4AddrLimit = SCPIP_READ_FLD(SCPIP_DRAM_M_0A+u2DramOffset, DRAM_M_0A_DA_WADDR_HI_LIMIT);
    }
    return u4AddrLimit;
}

void vScpipDramFrameDropEn(UINT8 bPath, UINT32 u4Enable)
{
    UINT8 value;
	UINT16 u2DramOffset;
	
    value = (u4Enable==SV_ON)?1:0;
	u2DramOffset = getDramOffset(bPath);

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+u2DramOffset, value, DRAM_M_03_DA_WRITE_FRAME_DROP_EN);
}

/**
 * @brief set frame drop ratio
 * @param u4M & u4N, ratio is (u4N /u4M)
 */
void vScpipSetFrameDropRatio(UINT8 bPath, UINT32 u4N, UINT32 u4M)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+getDramOffset(bPath), u4N, DRAM_M_03_DA_FRAME_DROP_N);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+getDramOffset(bPath), u4M, DRAM_M_03_DA_FRAME_DROP_M);
}


void vScpipSet3DInputEnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, u4Enable, DRAM_M_12_DA_3D_INPUT_EN);
}

/**
 * @brief set scaler input 3D mode
 * @param u4Mode 0: frame packing/frame sequential
 * 				    1: side by side 2: tab 3: line interleave
 */
void vScpipSet3DInputMode(UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, u4Mode, DRAM_M_12_DA_3D_INPUT_MODE);
}

void vScpipSet3DOutputEnable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, u4Enable, DRAM_M_12_DA_3D_OUTPUT_EN);
}

/**
 * @brief set scaler output 3D mode
 * @param u4Mode 0: PR 1: FPR 2: side by side 3: Shutter
 */
void vScpipSet3DOutputMode(UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, u4Mode, DRAM_M_12_DA_3D_OUTPUT_MODE);
}

void vScpipSet3DFprMode(UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, u4Mode, DRAM_M_12_DA_3D_FPR_MODE);	
}

void vScpip3DTo2DOnOff(UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;	
	
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_12, value, DRAM_M_12_DA_WRITE_3D_TO_2D_EN);	
}

// pattern gen
void vScpipInPtgEnable(UINT8 bPath, UINT32 u4Enable)
{
    UINT8 value;

	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4Enable==SV_ON)?1:0;	

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_23, value, SCSYS_23_SC_INPTG_ENABLE_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_27, value, SCSYS_27_SC_INPTG_ENABLE_2);
	}
}

void vScpipInOut3DPtgEnable(UINT8 u1InOut, UINT8 u1OnOff)
{
    UINT8 value;
	
    value = (u1OnOff==SV_ON)?1:0;	

	if(u1InOut == 0)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_23, value, SCSYS_23_SC_INPTG_ENABLE_1);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_21, value?3:0, SCSYS_21_SC_INPTG_3D_MODE_1);
	}
    else
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_23, value, SCSYS_23_SC_OUTPTG_ENABLE_1);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_24, value?3:0, SCSYS_24_SC_OUTPTG_3D_MODE_1);
    }
}

void vScpipSetSelfPtgRes(UINT32 u4Hact, UINT32 u4Vact, UINT32 u4Htot, UINT32 u4Vtot)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_45, u4Hact, SCSYS_45_SC_MIN_SELF_HACT);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_46, u4Vact, SCSYS_46_SC_MIN_SELF_VACT);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_45, u4Htot, SCSYS_45_SC_MIN_SELF_HTOT);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_46, u4Vtot, SCSYS_46_SC_MIN_SELF_VTOT);
}

void vScpipInPtgBndEnable(UINT8 bPath, UINT32 u4Enable)
{
    UINT8 value;

	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4Enable==SV_ON)?1:0;	

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, value, SCSYS_21_SC_INPTG_BND_EN_1);	
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, value, SCSYS_2A_SC_INPTG_BND_EN_2);		
	}
}

void vScpipSetInPtgBndSize(UINT8 bPath, UINT32 u4BndWidth, UINT32 u4BndHeight)
{
	if(bPath > SV_VP_PIP)
	{
		return;
	}

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_22, u4BndWidth, SCSYS_22_SC_INPTG_BND_W_1);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_22, u4BndHeight, SCSYS_22_SC_INPTG_BND_H_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_26, u4BndWidth, SCSYS_26_SC_INPTG_BND_W_2);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_26, u4BndHeight, SCSYS_26_SC_INPTG_BND_H_2);				
	}	
}

void vScpipOutPtgEnable(UINT8 bPath, UINT32 u4Enable)
{
    UINT8 value;

	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4Enable==SV_ON)?1:0;	

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_23, value, SCSYS_23_SC_OUTPTG_ENABLE_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_27, value, SCSYS_27_SC_OUTPTG_ENABLE_2);
	}	
}
void vScpipSetInPtgStyle(UINT8 bPath, UINT8 u1PtType)
{
    UINT8 u1FreeRun = 0;
    
	if(bPath > SV_VP_PIP)
	{
		return;
	}

    if(u1PtType & SCL_PTG_TYPE_MOVING)
    {
        u1FreeRun = 1;
    }

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 5, SCSYS_21_SC_INPTG_ANGLE_SEL_1);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 1, SCSYS_21_SC_INPTG_LEFT_SIDE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, u1FreeRun, SCSYS_21_SC_INPTG_FREERUN_EN_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 1, SCSYS_21_SC_INPTG_FREERUN_SPEED_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 1, SCSYS_21_SC_INPTG_COLOR_MODE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 0x1F, SCSYS_21_SC_INPTG_PAT_DUTY_CYCLE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_21, 8, SCSYS_21_SC_INPTG_YPAT1_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_23, 0xA, SCSYS_23_SC_INPTG_YPAT2_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 5, SCSYS_2A_SC_INPTG_ANGLE_SEL_2);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 1, SCSYS_2A_SC_INPTG_LEFT_SIDE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, u1FreeRun, SCSYS_2A_SC_INPTG_FREERUN_EN_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 1, SCSYS_2A_SC_INPTG_FREERUN_SPEED_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 1, SCSYS_2A_SC_INPTG_COLOR_MODE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 0x1F, SCSYS_2A_SC_INPTG_PAT_DUTY_CYCLE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_2A, 8, SCSYS_2A_SC_INPTG_YPAT1_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_27, 0xA, SCSYS_27_SC_INPTG_YPAT2_2);				
	}	
}

void vScpipSetOutPtgStyle(UINT8 bPath, UINT8 u1PtType)
{
    UINT8 u1FreeRun = 0;

	if(bPath > SV_VP_PIP)
	{
		return;
	}

    if(u1PtType & SCL_PTG_TYPE_MOVING)
    {
        u1FreeRun = 1;
    }

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 7, SCSYS_24_SC_OUTPTG_ANGLE_SEL_1);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 1, SCSYS_24_SC_OUTPTG_LEFT_SIDE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, u1FreeRun, SCSYS_24_SC_OUTPTG_FREERUN_EN_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 0xE, SCSYS_24_SC_OUTPTG_FREERUN_SPEED_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 1, SCSYS_24_SC_OUTPTG_COLOR_MODE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 0x1F, SCSYS_24_SC_OUTPTG_PAT_DUTY_CYCLE_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, 8, SCSYS_24_SC_OUTPTG_YPAT1_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_23, 0xA, SCSYS_23_SC_OUTPTG_YPAT2_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 7, SCSYS_28_SC_OUTPTG_ANGLE_SEL_2);	
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 1, SCSYS_28_SC_OUTPTG_LEFT_SIDE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, u1FreeRun, SCSYS_28_SC_OUTPTG_FREERUN_EN_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 0xE, SCSYS_28_SC_OUTPTG_FREERUN_SPEED_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 1, SCSYS_28_SC_OUTPTG_COLOR_MODE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 0x1F, SCSYS_28_SC_OUTPTG_PAT_DUTY_CYCLE_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, 8, SCSYS_28_SC_OUTPTG_YPAT1_2);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_27, 0xA, SCSYS_27_SC_OUTPTG_YPAT2_2);				
	}	
}
void vScpipOutPtgBndEnable(UINT8 bPath, UINT32 u4Enable)
{
    UINT8 value;

	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4Enable==SV_ON)?1:0;	

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_24, value, SCSYS_24_SC_OUTPTG_BND_EN_1);	
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_28, value, SCSYS_28_SC_OUTPTG_BND_EN_2);		
	}
}

void vScpipSetOutPtgBndSize(UINT8 bPath, UINT32 u4BndWidth, UINT32 u4BndHeight)
{
	if(bPath > SV_VP_PIP)
	{
		return;
	}

	if(bPath == SV_VP_MAIN)
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_25, u4BndWidth, SCSYS_25_SC_OUTPTG_BND_W_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_25, u4BndHeight, SCSYS_25_SC_OUTPTG_BND_H_1);
	}
	else
	{
		SCPIP_WRITE_FLD(SCPIP_SCSYS_29, u4BndWidth, SCSYS_29_OUTPTG_BND_W_1);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_29, u4BndHeight, SCSYS_29_OUTPTG_BND_H_1);				
	}	
}


/**
  * @brief bypass scaler
  * @param u4OnOff  0:normal / 1:bypass 
  */
void vScpipBypass(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4OnOff==SV_ON)?1:0;	
	
	(bPath==SV_VP_MAIN)?SCPIP_WRITE_FLD(SCPIP_SCSYS_00, value, SCSYS_00_SC_BYPASS_SCALER_1)
					  :SCPIP_WRITE_FLD(SCPIP_SCSYS_00, value, SCSYS_00_SC_BYPASS_SCALER_2);
}

#if defined(CC_SUPPORT_4K2K)
void vScpipInverseOutputVSync(UINT32 u4OnOff)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_10, ((u4OnOff==SV_TRUE)?1:0), SCSYS_10_SC_INV_OUTPUT_VSYNC);
}

void vScpipInverseOutputHSync(UINT32 u4OnOff)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_10, ((u4OnOff==SV_TRUE)?1:0), SCSYS_10_SC_INV_OUTPUT_HSYNC);
}

void vScpipInverseInputVDE(UINT8 bPath, UINT32 u4OnOff)
{
	UINT8 value;
	if(bPath > SV_VP_PIP)
	{
		return;
	}
	
    value = (u4OnOff==SV_ON)?1:0;	
	
	(bPath==SV_VP_MAIN)?SCPIP_WRITE_FLD(SCPIP_SCSYS_11, value, SCSYS_11_SC_INV_INPUT_MVDE)
					  :SCPIP_WRITE_FLD(SCPIP_SCSYS_11, value, SCSYS_11_SC_INV_INPUT_PVDE);    
}
#endif

// GFX

/**
  * @brief GFX SRC select
  * @param u4Sel (SCPIP_GFX_SRC_SEL) 0:Main / 1:Sub / 2:Main_DI / 3:Sub_DI / 4:Main_PDS / 5:Sub_PDS / 6:Monitor_out
  */
void vScpipGfxSrcSel(UINT32 u4Sel)
{
    if(u4Sel >= GFX_SRC_SEL_MAX)
    {
        return;
    }
    SCPIP_WRITE_FLD(SCPIP_SCSYS_0B, u4Sel, SCSYS_0B_SC_GFX_SRC_SEL);
}

/**
  * @brief Gfx write resolution
  * @param u4Width
  * @param u4Height
  */
void vScpipGfxResolution(UINT32 u4Width, UINT32 u4Height)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_0B, u4Width,  SCSYS_0B_SC_GFX_WIDTH);
    SCPIP_WRITE_FLD(SCPIP_SCSYS_0B, u4Height, SCSYS_0B_SC_GFX_HEIGHT);
}

void vScpipGfxSetResolution(UINT32 u4Enable)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_0B, u4Enable,  SCSYS_0B_SC_GFX_SET_RES_ENABLE);
}

void vScpipGfxUVInv(UINT32 u4Inv)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0C, u4Inv, SCSYS_0C_SC_GFX_UVINV);
}

void vScpipGfx422To444Enable(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0C, u4Enable, SCSYS_0C_SC_GFX_YC2YUV_EN);
}

void vScpipGfxBypass3x3(UINT32 u4Enable)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_0C, u4Enable, SCSYS_0C_SC_GFX_EASY_BP3X3);
}

void vScpipGfxFrameDropEn(UINT32 u4VdpId, UINT8 u4Enable, UINT8 u1DropM, UINT8 u1DropN)
{
    UINT8 value;
	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03, u1DropM, DRAM_M_03_DA_FRAME_DROP_M);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03, u1DropN, DRAM_M_03_DA_FRAME_DROP_N);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_03, value, DRAM_M_03_DA_GFX_FRAME_DROP_EN);
}

UINT8 u1ScpipGfxGetFrameIdx(void)
{
    return SCPIP_READ_FLD(SCPIP_DRAM_M_02, DRAM_M_02_STA_GFX_WR_FRAME_CNT);
}

UINT8 u1ScpipGFXGetWriteStatus(void)
{
    return SCPIP_READ_FLD(SCPIP_DRAM_M_13, DRAM_M_13_STA_GFX_WRITE_EN);
}


/**
  * @brief set GFX addr base
  * @param u4BaseNum 0: base_0 / 1: base_1 / 2: base_2 / 3: base _3
  */
void vScpipGfxSetAddrBase(UINT32 u4BaseNum, UINT32 u4Addr)
{
	UINT16 u2BaseOfst;
	u2BaseOfst = 4 * u4BaseNum;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_0C+u2BaseOfst, u4Addr, DRAM_M_0C_DA_GFX_ADDR_BASE_0);
}
UINT32 vScpipGfxGetAddrBase(UINT32 u4BaseNum)
{
	UINT16 u2BaseOfst;
	u2BaseOfst = 4 * u4BaseNum;

	return (UINT32)SCPIP_READ_FLD(SCPIP_DRAM_M_0C+u2BaseOfst,DRAM_M_0C_DA_GFX_ADDR_BASE_0);
}
void vScpipGfxSetAddrLimit(UINT32 u4Hi, UINT32 u4Lo)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_10, u4Hi, DRAM_M_10_DA_GFX_WADDR_HI_LIMIT);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_11, u4Lo, DRAM_M_11_DA_GFX_WADDR_LO_LIMIT);
}

void vScpipGfx3DTo2DEnable(UINT32 u4VdpId, UINT32 u4Enable)
{
    UINT8 value;
    UINT16 u2DramOffset;
	
    value = (u4Enable==SV_ON)?1:0;
    u2DramOffset = getDramOffset(getScalerPath(u4VdpId));
	
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_12+u2DramOffset, value, DRAM_M_12_DA_WRITE_3D_TO_2D_EN);
}

/**
  * @brief set GFX alpha value when gfx in ARGB mode
  */
void vScpipGfxSetAlpha(UINT32 u4Alpha)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Alpha, DRAM_M_13_DA_GFX_ALPHA);	
}

/**
  * @brief set GFX addr base
  * @param u4Option 0: ARGB 1: RGBA
  */
void vScpipGfxAlphaPosSel(UINT32 u4Option)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Option, DRAM_M_13_DA_GFX_ALPHA_POS_SEL);	
}

void vScpipGfxSetFrameNum(UINT32 u4Num)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Num, DRAM_M_13_DA_GFX_FRAME_NUM);		
}

UINT8 u1ScpipGfxGetFrameNum(void)
{
	return SCPIP_READ_FLD(SCPIP_DRAM_M_13, DRAM_M_13_DA_GFX_FRAME_NUM);		
}

/**
  * @brief set GFX addr base
  * @param u4Mode 0: 32bit YCC 4-2-2 / 1: 32bit ARGB 4-4-4-4
  *                         2: 16bit RGB 5-6-5 / 3: 32bit ARGB 2-10-10-10
  */
void vScpipGfxSetMode(UINT32 u4Mode)
{
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Mode, DRAM_M_13_DA_GFX_MODE);
}

/**
  * @brief select data channel as R or G or B
  * @param (ex. u4MuxR) 0: from Y channel / 1: from U channel / 2: from V channel
  */
void vScpipGfxSetMux(UINT32 u4MuxR, UINT32 u4MuxG, UINT32 u4MuxB)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4MuxR, DRAM_M_13_DA_GFX_MUX_R);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4MuxG, DRAM_M_13_DA_GFX_MUX_G);
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4MuxB, DRAM_M_13_DA_GFX_MUX_B);
}

/**
  * @brief enable interlace source, write field only
  * @param 
  */
void vScpipGfxP2IEnable(UINT32 u4Enable)
{
	UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, value, DRAM_M_13_DA_GFX_P2I_EN);
}

/**
  * @brief write field only mode
  * @param 0: write top field only / 1: write bottom field only
  */
void vScpipGfxP2ITorB(UINT32 u4Sel)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Sel, DRAM_M_13_DA_GFX_P2I_TB);
}

void vScpipGfxRoundEn(UINT32 u4Enable)
{
	UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, value, DRAM_M_13_DA_GFX_ROUND_EN);
}

/**
  * @brief enable Gfx write
  */
void vScpipGfxWriteEn(UINT32 u4Enable)
{
	UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, value, DRAM_M_13_DA_GFX_WRITE_ENABLE);
}

/**
  * @brief Gfx write enable mode
  * @param u4Mode 0: flag mode / 1: trigger mode
  */
void vScpipGfxSetWriteEnMode(UINT32 u4Mode)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Mode, DRAM_M_13_DA_GFX_WRITE_EN_MODE);
}

/**
  * @brief Gfx write trigger
  * @param u4Trigger: 0->1
  */
void vScpipGfxWriteTrigger(UINT32 u4Trigger)
{
	SCPIP_WRITE_FLD(SCPIP_DRAM_M_13, u4Trigger, DRAM_M_13_DA_GFX_WRITE_TRIGGER);
}



void vScpipTveImportEn(UINT32 u4Enable)
{
	UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCCTRL2_0A, value, SCSYS_01_SC_TG_TVE_IMPORT_ENABLE);
}
void vScpipTveImportTg(UINT32 u4Enable)
{
    UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_01, value, SCSYS_01_SC_TVE_SET_TOGGLE);
}

void vScpipSetTveTiming(UINT32 u4Hact, UINT32 u4Htot, UINT32 u4Vact, UINT32 u4Vtot)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_17, u4Hact, SCSYS_17_SC_TVE_WIDTH);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_16, u4Htot, SCSYS_16_SC_TVE_HTOTAL);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_17, u4Vact, SCSYS_17_SC_TVE_HEIGHT);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_16, u4Vtot, SCSYS_16_SC_TVE_VTOTAL);
}

void vScpipSetTvePorch(UINT32 u4Hfp, UINT32 u4Hbp, UINT32 u4Vfp, UINT32 u4vbp)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_18, u4Hfp, SCSYS_18_SC_TVE_FRNT_HPORCH);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_18, u4Hbp, SCSYS_18_SC_TVE_BACK_HPORCH);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_19, u4Vfp, SCSYS_19_SC_TVE_FRNT_VPORCH);
	SCPIP_WRITE_FLD(SCPIP_SCSYS_19, u4vbp, SCSYS_19_SC_TVE_BACK_VPORCH);	
}
void vScpipTveSrcSel(UINT8 u4Sel)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_14, u4Sel, SCSYS_14_SC_TVE_SRC_SEL);
}

UINT32 vScpipGetTveSrcSel(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_14, SCSYS_14_SC_TVE_SRC_SEL);
}

void vScpipToTveEnable(UINT32 u4Enable)
{
	UINT8 value;	
    value = (u4Enable==SV_ON)?1:0;

	SCPIP_WRITE_FLD(SCPIP_SCSYS_07, value, SCSYS_07_SC_PIP2TVE_ENABLE);
}
void vScpipSetOsdTveTiming(UINT32 u4HLen, UINT32 u4VLen)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_20, u4HLen, SCSYS_20_SC_OSD_TVE_HLEN);
    SCPIP_WRITE_FLD(SCPIP_SCSYS_20, u4VLen, SCSYS_20_SC_OSD_TVE_VLEN);
}
void vScpipInvOsdTveVsync(UINT8 uOnOff)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_11, uOnOff, SCSYS_11_SC_INV_OSD_TVE_VSYNC);
}
void vScpipSetOsdTveVsyncSel(UINT8 uOnOff)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_14, uOnOff, SCSYS_14_SC_OSD_TVE_VSYNC_SEL);
}
void vScpipSetOsdTveVsyncDly(UINT32 u4Value)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_15, u4Value, SCSYS_15_SC_OSD_TVE_VSYNC_DLY);
}
void vScpipSetPureEdgeBlankNum(UINT32 u4BlankNum)
{
	SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0C, u4BlankNum, SCCTRL1_0C_SC_PE_BLANK_NUM);
}

#ifdef CC_SUPPORT_STR
extern UINT8 vGetPowerDownSts(void );
#endif
UINT32 u4ScpipGetDramRStatus(UINT8 bPath)
{
    #ifdef CC_SUPPORT_STR
    #ifndef CC_MTK_LOADER
    if (vGetPowerDownSts())
        return 0;
    else
    #endif
    #endif
	return (UINT32)SCPIP_READ_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), SCCTRL1_1B_SC_STA_DRAM_RPTR_1);
}
UINT32 u4ScpipGetDramWStatus(UINT8 bPath)
{
    #ifdef CC_SUPPORT_STR
    #ifndef CC_MTK_LOADER
    if (vGetPowerDownSts())
        return 0;
    else
    #endif
    #endif
	    return (UINT32)SCPIP_READ_FLD(SCPIP_SCCTRL1_1B+getSCOffset(bPath), SCCTRL1_1B_SC_STA_DRAM_WPTR_1);
}


/**
  * @brief reorder UV sequence (only in MAIN path)
  * @return: u4Inverse  1: y0cr y1cb (when mirror)
  *                              	  0: y0cb y1cr
  */
UINT32 u4ScpipGetUVOrder(void)
{
	return (UINT32)SCPIP_READ_FLD(SCPIP_SCCTRL1_04, SCCTRL1_04_SC_REORDER_UV_INV_1);
}

UINT32 u4ScpipGetSetResInStatus(UINT8 bPath)
{
	return (UINT32)SCPIP_READ_FLD(SCPIP_SCCTRL1_0A+getSCOffset(bPath), SCCTRL1_0A_SC_STA_SET_RES_I_1);
}

UINT32 u4ScpipGetSetResOutStatus(UINT8 bPath)
{
	return (UINT32)SCPIP_READ_FLD(SCPIP_SCCTRL1_0A+getSCOffset(bPath), SCCTRL1_0A_SC_STA_SET_RES_O_1);
}

UINT32 u4ScpipGetFrmTrkLockStatus(void)
{
	return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_14, SCSYS_14_SC_STA_FRM_TRK_LOCK_OK);
}

/**
  * @brief scaler get panel width
  * @8098 porting done
  */
UINT32 u4ScpipGetPanelWidth(E_TG_MODULE eModule)
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return u4UhdGetPanelWidth();
		}
		else
	#endif
	#endif
		{
	    	return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_02, SCSYS_02_SC_PNL_WIDTH);
		}
    }
#ifdef CC_MT5890                  
    else if ( eModule == E_GFX_TG )
         return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_61,  SCSYS_61_GFX_PNL_WIDTH);
    else if ( eModule == E_REF_TG )
        return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4A, SCSYS_4A_REF_PNL_WIDTH);
#endif    
    else
        return 0;  
}


/**
  * @brief scaler get panel height
  */
UINT32 u4ScpipGetPanelHeight(E_TG_MODULE eModule )
{
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return u4UhdGetPanelHeight();
		}
		else
	#endif
	#endif
		{
        	return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_02, SCSYS_02_SC_PNL_HEIGHT);
		}
    }
#ifdef CC_MT5890                  
    else if ( eModule == E_GFX_TG )        
         return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_61,  SCSYS_61_GFX_PNL_HEIGHT);
    else if ( eModule == E_REF_TG )        
         return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4A, SCSYS_4A_REF_PNL_HEIGHT);
#endif    
    else 
        return 0;
}

/**
  * @brief scaler get panel htotal
  */
UINT32 u4ScpipGetPanelHtotal( E_TG_MODULE eModule )
{    
    if ( eModule == E_FSC_TG )
    {
    #ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
		{
			return u4UhdGetPanelHtotal();
		}
		else
	#endif
	#endif
		{
    		return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_03, SCSYS_03_SC_PNL_HTOTAL);
		}
    }
#ifdef CC_MT5890                  
    else if ( eModule == E_GFX_TG )
       return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_62, SCSYS_62_GFX_PNL_HTOTAL);
    else if ( eModule == E_REF_TG )
       return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4B, SCSYS_4B_REF_PNL_HTOTAL);
#endif    
    else
       return 0;  
}

/**
  * @brief scaler get panel vtotal
  */
UINT32 u4ScpipGetPanelVtotal(E_TG_MODULE eModule)
{   
    if ( eModule == E_FSC_TG )
   	{
   	#ifdef CC_SUPPORT_UHD
	#ifndef CC_MTK_LOADER
		if (PANEL_IsSupport4K2KDD())
			{
			return u4UhdGetPanelVtotal();
			}
		else
	#endif
	#endif
		{
  			return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_03, SCSYS_03_SC_PNL_VTOTAL);
		}
   	}
#ifdef CC_MT5890                 
   else if ( eModule == E_GFX_TG )    
      return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_62,  SCSYS_62_GFX_PNL_VTOTAL);
   else if ( eModule == E_REF_TG )
      return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_4B, SCSYS_4B_REF_PNL_VTOTAL);
#endif   
   else
      return 0;
    
}

UINT32 u4ScpipGetInputHTotalStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_2D, SCSYS_2D_SC_STA_HTOTAL_1);
}

UINT32 u4ScpipGetInputHActiveStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_2D, SCSYS_2D_SC_STA_HACTIVE_1);
}

UINT32 u4ScpipGetInputVTotalStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VTOTAL_1);
}

UINT32 u4ScpipGetInputVActiveStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VACTIVE_1);
}

UINT32 u4ScpipGetOutputHActiveStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_48, SCSYS_48_SC_OUT_MEASURE_HACTIVE);
}

UINT32 u4ScpipGetOutputVActiveStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_48, SCSYS_48_SC_OUT_MEASURE_VACTIVE);
}

UINT32 u4ScpipGetOutputHTotalStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_HTOTAL);
}

UINT32 u4ScpipGetOutputVTotalStatus(UINT8 bPath)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_VTOTAL);
}

UINT32 u4ScpipGetOutVCnt(void)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_47, SCSYS_47_SC_STA_V_CNT);
}

#if defined(CC_MT5890)
UINT32 u4ScpipGetREFOutVCnt(void)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_74, SCSYS_74_REF_V_CNT);
}

UINT32 u4ScpipGetGFXOutVCnt(void)
{
    return (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_6A, SCSYS_6A_GFX_STA_V_CNT);
}
#endif


/**
* @brief Get scaler input/output V sync distance count
*/
UINT32 u4ScpipGetInOutVDistance(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_13, SCSYS_13_SC_STA_FRM_VSYNC_OUT_EARLY);
}

UINT32 u4ScpipGetFrametrackABSDistance(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_2B, SCSYS_2B_STA_FRM_TRK_ABS_DIS);
}

UINT32 u4ScpipGetFrametrackVTotalStatus(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_2B, SCSYS_2B_STA_FRM_TRK_VTOTAL);
}

UINT32 u4ScpipGetFrametrackLockStatus(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_14, SCSYS_14_SC_STA_FRM_TRK_LOCK_OK);
}

#ifdef CC_SUPPORT_STR
extern UINT8 vGetPowerDownSts(void );
#endif
/**
  *  @brief get status of input disp_r
  */
UINT8 u1ScpipGetInDispR(void)
{
    #ifdef CC_SUPPORT_STR
    #ifndef CC_MTK_LOADER
    if (vGetPowerDownSts())
        return 0;
    else
    #endif
    #endif
    return SCPIP_READ_FLD(SCPIP_SCSYS_1E, SCSYS_1E_SC_STA_INPUT_DISP_R);
}

/**
  *  @brief get status of output disp_r
  */
UINT8 u1ScpipGetOutDispR(void)
{
    return SCPIP_READ_FLD(SCPIP_SCSYS_1E, SCSYS_1E_SC_STA_OUTPUT_DISP_R);
}

UINT32 u4ScpipGetBufNum(UINT32 u4VdpId)
{
    UINT32 u4FrameNum;
    UINT8 bPath;
    UINT16 dramOffset;

    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);

    u4FrameNum = SCPIP_READ_FLD(SCPIP_DRAM_M_00+dramOffset,DRAM_M_00_DA_FRAME_NUM) + 1;

    return u4FrameNum;    
}

UINT8 bScpipGetDramWrite(UINT32 u4VdpId)
{
    UINT8 bRet = SV_OFF;
    UINT8 bPath;
    UINT16 dramOffset;

    if (u4VdpId >= VDP_NS)
    {
        return bRet;
    }
    
    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);
    if(SCPIP_READ_FLD(SCPIP_DRAM_M_00+dramOffset, DRAM_M_00_DA_WRITE_ENABLE)==1)
    {
        bRet = SV_ON;
    }

    return bRet;
}

UINT8 bScpipGetDramRead(UINT32 u4VdpId)
{
    UINT8 bRet = SV_OFF;
    UINT8 bPath;
    UINT16 dramOffset;

    if (u4VdpId >= VDP_NS)
    {
        return bRet;
    }
    
    bPath = getScalerPath(u4VdpId);
    dramOffset = getDramOffset(bPath);
    if(SCPIP_READ_FLD(SCPIP_DRAM_M_00+dramOffset, DRAM_M_00_DA_READ_ENABLE)==1)
    {
        bRet = SV_ON;
    }

    return bRet;
}

void vScpipHwInitOnPanelInit(UINT8 bPath)
{
    //set output window to 0, show background color
	vScpipSetOutputResolution(bPath,0,0);
    vScpipSetResImportEnable(bPath,0);
}


/**
 * @brief Clear Scaler set resolution status
 * @param u1VdpId VDP_1/VDP_2
 */
void vDrvScpipClrSetRes(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType)
{

    if (u1VdpId == VDP_1)
    {
        if (eStatusType == SET_RES_OUTPUT|| eStatusType == SET_RES_BOTH)
        {
            SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 1, SCCTRL1_0A_SC_CLR_STA_SET_RES_O_1);
            SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 0, SCCTRL1_0A_SC_CLR_STA_SET_RES_O_1);
        }
        if (eStatusType == SET_RES_INPUT|| eStatusType == SET_RES_BOTH)
        {
            SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 1, SCCTRL1_0A_SC_CLR_STA_SET_RES_I_1);
            SCPIP_WRITE_FLD(SCPIP_SCCTRL1_0A, 0, SCCTRL1_0A_SC_CLR_STA_SET_RES_I_1);
        }
    }
    else
    {
        if (eStatusType == SET_RES_OUTPUT || eStatusType == SET_RES_BOTH)
        {
            SCPIP_WRITE_FLD(SCPIP_SCCTRL2_0A, 1, SCCTRL2_0A_SC_CLR_STA_SET_RES_O_2);
            SCPIP_WRITE_FLD(SCPIP_SCCTRL2_0A, 0, SCCTRL2_0A_SC_CLR_STA_SET_RES_O_2);
        }
        if (eStatusType == SET_RES_INPUT|| eStatusType == SET_RES_BOTH)
        {
            SCPIP_WRITE_FLD(SCPIP_SCCTRL2_0A, 1, SCCTRL2_0A_SC_CLR_STA_SET_RES_I_2);
            SCPIP_WRITE_FLD(SCPIP_SCCTRL2_0A, 0, SCCTRL2_0A_SC_CLR_STA_SET_RES_I_2);
        }
    }
}

/**
 * @brief Get Scaler set resolution status
 * @param u1VdpId VDP_1/VDP_2
 * @param eStatusType     SET_RES_INPUT, SET_RES_OUTPUT, SET_RES_BOTH,
 */
BOOL bDrvScpipIsSetResDone(UINT8 u1VdpId, SET_RES_STATUS_TYPE_E eStatusType)
{
    UINT8 bInStatus, bOutStatus;
    if (u1VdpId == VDP_1)
    {
        bInStatus  = SCPIP_READ_FLD(SCPIP_SCCTRL1_0A, SCCTRL1_0A_SC_STA_SET_RES_I_1);
        bOutStatus = SCPIP_READ_FLD(SCPIP_SCCTRL1_0A, SCCTRL1_0A_SC_STA_SET_RES_O_1);
    }
    else
    {
        bInStatus  = SCPIP_READ_FLD(SCPIP_SCCTRL2_0A, SCCTRL2_0A_SC_STA_SET_RES_I_2);
        bOutStatus = SCPIP_READ_FLD(SCPIP_SCCTRL2_0A, SCCTRL2_0A_SC_STA_SET_RES_O_2);
    }
    
    if (eStatusType == SET_RES_INPUT)
    {
        return bInStatus;
    }
    else if (eStatusType == SET_RES_OUTPUT)
    {
        return bOutStatus;
    }
    else
    {
        return (bInStatus & bOutStatus);
    }
    
}

void vScpipSetPipRstCnt(UINT8 uRstCnt)
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_07, uRstCnt, SCSYS_07_SC_PIP_RST_VCNT);
}

void vScpipInOutStatusDump(UINT8 bPath)
{

    #if 0  // Several Status can not be trusted  
    //SCL IN
    if(SV_VP_MAIN==bPath)
    {
        LOG(0,"\tSCL  IN HT:%4d VT:%4d HA:%4d VA:%4d\n",
            IO32ReadFldAlign(SCPIP_SCSYS_2D, SCSYS_2D_SC_STA_HTOTAL_1),                        
            IO32ReadFldAlign(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VTOTAL_1),            
            IO32ReadFldAlign(SCPIP_SCSYS_2D, SCSYS_2D_SC_STA_HACTIVE_1),                
            IO32ReadFldAlign(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VACTIVE_1)
            );
    }
    else
    {
        LOG(0,"\tSCL  IN HT:%4d VT:%4d HA:%4d VA:%4d\n",
            IO32ReadFldAlign(SCPIP_SCSYS_2F, SCSYS_2F_SC_STA_HTOTAL_2),            
            IO32ReadFldAlign(SCPIP_SCSYS_30, SCSYS_30_SC_STA_VTOTAL_2),        
            IO32ReadFldAlign(SCPIP_SCSYS_2F, SCSYS_2F_SC_STA_HACTIVE_2),
            IO32ReadFldAlign(SCPIP_SCSYS_30, SCSYS_30_SC_STA_VACTIVE_2)
            );
    }
    //SCL OUT    
    LOG(0,"\tSCL OUT HT:%4d VT:%4d HA:%4d VA%4d\n",
        IO32ReadFldAlign(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_HTOTAL),                        
        IO32ReadFldAlign(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_VTOTAL),            
        IO32ReadFldAlign(SCPIP_SCSYS_48, SCSYS_48_SC_OUT_MEASURE_HACTIVE),                
        IO32ReadFldAlign(SCPIP_SCSYS_48, SCSYS_48_SC_OUT_MEASURE_VACTIVE)
        );
    #else

    //SCL IN
    if(SV_VP_MAIN==bPath)
    {
        LOG(0,"\tSCL  IN HT: N/A VT:%4d HA:%4d VA:%4d\n",
            IO32ReadFldAlign(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VTOTAL_1),            
            IO32ReadFldAlign(SCPIP_SCSYS_2D, SCSYS_2D_SC_STA_HACTIVE_1),                
            IO32ReadFldAlign(SCPIP_SCSYS_2E, SCSYS_2E_SC_STA_VACTIVE_1)
            );
    }
    else
    {
        LOG(0,"\tSCL  IN HT: N/A VT:%4d HA:%4d VA:%4d\n",
            IO32ReadFldAlign(SCPIP_SCSYS_30, SCSYS_30_SC_STA_VTOTAL_2),        
            IO32ReadFldAlign(SCPIP_SCSYS_2F, SCSYS_2F_SC_STA_HACTIVE_2),
            IO32ReadFldAlign(SCPIP_SCSYS_30, SCSYS_30_SC_STA_VACTIVE_2)
            );
    }
    
    //SCL OUT     
     LOG(0,"\tSCL OUT HT:%4d VT:%4d HA: N/A VA: N/A\n",
         IO32ReadFldAlign(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_HTOTAL),                        
         IO32ReadFldAlign(SCPIP_SCSYS_1F, SCSYS_1F_SC_OUT_MEASURE_VTOTAL)
         );

    #endif 
}

void vScpipSetAddrFldInv(UINT8 bPath, UINT32 u4En)
{
    UINT16 dramOffset;

    dramOffset = getDramOffset_2(bPath);
    
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_12+dramOffset, u4En, DRAM_M_12_DA_ADDR_FLD_INV);
}

void vScpipSetCtrlFldInv(UINT8 bPath, UINT32 u4En)
{
    UINT16 dramOffset;

    dramOffset = getDramOffset_2(bPath);
    
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_12+dramOffset, u4En, DRAM_M_12_DA_CTRL_FLD_INV);
}

UINT8 u1ScpipGetDramFldInfo(UINT8 bPath)
{
    UINT16 dramOffset;

    dramOffset = getDramOffset_2(bPath);

    return (UINT8)SCPIP_READ_FLD(SCPIP_DRAM_M_12 + dramOffset, DRAM_M_12_STA_WR_FLD);
}

UINT8 vScpipGetMainFldInfo(void)
{
    return (UINT8)SCPIP_READ_FLD(SCPIP_SCSYS_1E, SCSYS_1E_SC_STA_MAIN_FIELD_IN);
}

UINT8 vScpipGetPipFldInfo(void)
{
    return (UINT8)SCPIP_READ_FLD(SCPIP_SCSYS_1E, SCSYS_1E_SC_STA_PIP_FIELD_IN);
}


// scaler VENC
void vScpipH264SetWriteMode(UINT8 u1Src, UINT8 u1En)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u1En, DRAM_M_20_DA_WRITE_MODE);
}

void vScpipH264SetFrameMode(UINT8 u1Src, UINT8 u1Frame)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u1Frame, DRAM_M_20_DA_H264_FRAME_MODE);
}

UINT8 vScpipH264GetFrameMode(UINT8 u1Src)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return VENC_NS;
    }
    
    h264Offset = getH264Offset(u1Src);
    return (UINT8)SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_DA_H264_FRAME_MODE);
}

void vScpipH264SetChromaLine(UINT8 u1Src, UINT8 u1Line)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u1Line, DRAM_M_20_DA_420_SHIFT);
}

void vScpipH264SetEndVCnt(UINT8 u1Src, UINT8 u1En, UINT16 u2VCnt)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u1En,   DRAM_M_20_DA_WR_END_LINE_CNT_EN);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u2VCnt, DRAM_M_20_DA_WR_END_LINE_CNT);
}

void vScpipH264SetReadyMode(UINT8 u1Src, UINT8 u1Sel)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, u1Sel, DRAM_M_20_DA_WR_READY_MODE);
}

UINT8 vScpipH264GetReadyMode(UINT8 u1Src)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return VENC_NS;// invalid value
    }
    
    h264Offset = getH264Offset(u1Src);
    return (UINT8)SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_DA_WR_READY_MODE);
}

UINT8 u1ScpipH264GetFrameIdx(UINT8 u1Src)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS)
    {
        return VENC_NS; // invalid value
    }
    
    h264Offset = getH264Offset(u1Src);
    return (UINT8)SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_STA_FRM_WPTR);
}

UINT8 u1ScpipH264GetReadyIdx(UINT8 u1Src, UINT8 u1Idx)
{
    UINT8  u1Rdy;
    UINT16 h264Offset;

    if (u1Src >= VENC_NS || u1Idx >= VENC_BUF_NS)
    {
        return VENC_NS; // invalid value
    }
    
    h264Offset = getH264Offset(u1Src);
    if (u1Idx == 0)
    {
        u1Rdy = SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_STA_FRM_READY0);
    }
    else if (u1Idx == 1)
    {
        u1Rdy = SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_STA_FRM_READY1);
    }
    else
    {
        u1Rdy = SCPIP_READ_FLD(SCPIP_DRAM_M_20+h264Offset, DRAM_M_20_STA_FRM_READY2);
    }

    return u1Rdy;
}

void vScpipH264ClrReadyIdx(UINT8 u1Src, UINT8 u1Idx)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS || u1Idx >= VENC_BUF_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    if (u1Idx == 0)
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, 1, DRAM_M_20_DA_CLR_FRM_READY0);
    }
    else if (u1Idx == 1)
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, 1, DRAM_M_20_DA_CLR_FRM_READY1);
    }
    else
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_20+h264Offset, 1, DRAM_M_20_DA_CLR_FRM_READY2);
    }
}

void vScpipH264SetDropFrame(UINT8 u1Src, UINT8 fgOnOff, UINT8 u1dropM, UINT8 u1dropN)
{
    UINT16 u2DramOffset = getDramOffset(u1Src);

    if (u1Src >= VENC_NS || u1dropN >u1dropM)
    {
        return;
    }
 
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+u2DramOffset, fgOnOff, DRAM_M_03_DA_WRITE_FRAME_DROP_EN);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+u2DramOffset, u1dropM, DRAM_M_03_DA_FRAME_DROP_M);
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_03+u2DramOffset, u1dropN, DRAM_M_03_DA_FRAME_DROP_N);
}


void vScpipH264SetDramAddr(UINT8 u1Src, UINT8 u1Idx, UINT32 u4Yaddr, UINT32 u4Caddr)
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS || u1Idx >= VENC_BUF_NS)
    {
        return;
    }
    
    h264Offset = getH264Offset(u1Src);
    if (u1Idx == 0)
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_21+h264Offset, u4Yaddr, DRAM_M_21_DA_H264_ADDR_BASE_Y0);
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_22+h264Offset, u4Caddr, DRAM_M_22_DA_H264_ADDR_BASE_C0);
    }
    else if (u1Idx == 1)
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_23+h264Offset, u4Yaddr, DRAM_M_23_DA_H264_ADDR_BASE_Y1);
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_24+h264Offset, u4Caddr, DRAM_M_24_DA_H264_ADDR_BASE_C1);
    }
    else
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_25+h264Offset, u4Yaddr, DRAM_M_25_DA_H264_ADDR_BASE_Y2);
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_26+h264Offset, u4Caddr, DRAM_M_26_DA_H264_ADDR_BASE_C2);
    }
}

UINT8 vScpipH264GetDramAddr(UINT8 u1Src, UINT8 u1Idx, UINT32 *u4BufAdr_Y, UINT32 *u4BufAdr_C )
{
    UINT16 h264Offset;

    if (u1Src >= VENC_NS || u1Idx >= VENC_BUF_NS)
    {
        return FALSE;
    }
    
    h264Offset = getH264Offset(u1Src);
    if (u1Idx == 0)
    {
        *u4BufAdr_Y = SCPIP_READ_FLD(SCPIP_DRAM_M_21+h264Offset,  DRAM_M_21_DA_H264_ADDR_BASE_Y0);
        *u4BufAdr_C = SCPIP_READ_FLD(SCPIP_DRAM_M_22+h264Offset,  DRAM_M_22_DA_H264_ADDR_BASE_C0);
    }
    else if (u1Idx == 1)
    {
        *u4BufAdr_Y = SCPIP_READ_FLD(SCPIP_DRAM_M_23+h264Offset,  DRAM_M_23_DA_H264_ADDR_BASE_Y1);
        *u4BufAdr_C = SCPIP_READ_FLD(SCPIP_DRAM_M_24+h264Offset,  DRAM_M_24_DA_H264_ADDR_BASE_C1);
    }
    else
    {
        *u4BufAdr_Y = SCPIP_READ_FLD(SCPIP_DRAM_M_25+h264Offset,  DRAM_M_25_DA_H264_ADDR_BASE_Y2);
        *u4BufAdr_C = SCPIP_READ_FLD(SCPIP_DRAM_M_26+h264Offset,  DRAM_M_26_DA_H264_ADDR_BASE_C2);
    }	
	return TRUE;
}

/*
   set interrupt when interlace source
   0: each field write end interrupt
   1: pair         write end interrupt
*/
void vScpipSetFieldISREn(UINT8 bPath, UINT32 u4En)
{
    UINT16 dramOffset;

    dramOffset = getDramOffset_2(bPath);
    
    SCPIP_WRITE_FLD(SCPIP_DRAM_M_12+dramOffset, u4En, DRAM_M_12_DA_FLD_INT_EN);
}

UINT32 u4ScpipGetISRstatus(UINT32 u4Sel)
{
    UINT32 u4status;

    switch(u4Sel)
    {
    case ISR_FSC_MAIN_VSYNC_IN_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_M_IN_VSYNC_R);
        break;
    case ISR_FSC_MAIN_VSYNC_IN_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_M_IN_VSYNC_F);
        break;
    case ISR_FSC_SUB_VSYNC_IN_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_P_IN_VSYNC_R);
        break;
    case ISR_FSC_SUB_VSYNC_IN_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_P_IN_VSYNC_F);
        break;
    case ISR_FSC_VSYNC_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_FSC_OUT_VSYNC_R);
        break;
    case ISR_FSC_VSYNC_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_FSC_OUT_VSYNC_F);
        break;
    case ISR_FSC_VDE_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_FSC_OUT_VDE_R);
        break;
    case ISR_FSC_VDE_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_FSC_OUT_VDE_F);
        break;
    case ISR_PSC_VSYNC_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_PSC_OUT_VSYNC_R);
        break;
    case ISR_PSC_VSYNC_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_PSC_OUT_VSYNC_F);
        break;
    case ISR_PSC_VDE_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_PSC_OUT_VDE_R);
        break;
    case ISR_PSC_VDE_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_PSC_OUT_VDE_F);
        break;
    case ISR_DRAM_MAIN_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_M_DRAM_WR_END);
        break;
    case ISR_DRAM_SUB_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_P_DRAM_WR_END);
        break;
    case ISR_DRAM_GFX_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_DRAM_WR_END);
        break;
    case ISR_DRAM_MON_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_MON_DRAM_WR_END);
        break;
    case ISR_GFX_VSYNC_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_VSYNC_R);
        break;
    case ISR_GFX_VSYNC_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_VSYNC_F);
        break;
    case ISR_GFX_VDE_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_VDE_R);
        break;
    case ISR_GFX_VDE_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_VDE_F);
        break;
    case ISR_GFX_OSD_VSYNC_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_OSD_VSYNC_R);
        break;
    case ISR_GFX_OSD_VSYNC_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_OSD_VSYNC_F);
        break;
    case ISR_GFX_OSD_VDE_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_OSD_VDE_R);
        break;
    case ISR_GFX_OSD_VDE_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_GFX_OSD_VDE_F);
        break;
    case ISR_REF_FSC_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_REF2FSC_PUL);
        break;
    case ISR_REF_PSC_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_REF2PSC_PUL);
        break;
    case ISR_REF_GFX_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_REF2GFX_PUL);
        break;
    case ISR_REF_B2R_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_REF2B2R_PUL);
        break;
    case ISR_REF_R2R_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_REF2R2R_PUL);
        break;
	case ISR_PQ_TDSHARP:
		u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_0A, SCSYS_0A_INT_STA_TDSHARP);
		break;
    default:
        u4status = (UINT32)u2IO32Read2B(SCPIP_SCSYS_0A);
        break;
    }

    return u4status;
}

void vScpipClearISRstatus(UINT32 u4Sel)
{
    switch(u4Sel)
    {
    case ISR_FSC_MAIN_VSYNC_IN_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_M_IN_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_M_IN_VSYNC_R_CLR);
        break;
    case ISR_FSC_MAIN_VSYNC_IN_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_M_IN_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_M_IN_VSYNC_F_CLR);
        break;
    case ISR_FSC_SUB_VSYNC_IN_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_P_IN_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_P_IN_VSYNC_R_CLR);
        break;
    case ISR_FSC_SUB_VSYNC_IN_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_P_IN_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_P_IN_VSYNC_F_CLR);
        break;
    case ISR_FSC_VSYNC_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_FSC_OUT_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_FSC_OUT_VSYNC_R_CLR);
        break;
    case ISR_FSC_VSYNC_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_FSC_OUT_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_FSC_OUT_VSYNC_F_CLR);
        break;
    case ISR_FSC_VDE_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_FSC_OUT_VDE_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_FSC_OUT_VDE_R_CLR);
        break;
    case ISR_FSC_VDE_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_FSC_OUT_VDE_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_FSC_OUT_VDE_F_CLR);
        break;
    case ISR_PSC_VSYNC_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_PSC_OUT_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_PSC_OUT_VSYNC_R_CLR);
        break;
    case ISR_PSC_VSYNC_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_PSC_OUT_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_PSC_OUT_VSYNC_F_CLR);
        break;
    case ISR_PSC_VDE_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_PSC_OUT_VDE_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_PSC_OUT_VDE_R_CLR);
        break;
    case ISR_PSC_VDE_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_PSC_OUT_VDE_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_PSC_OUT_VDE_F_CLR);
        break;
    case ISR_DRAM_MAIN_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_M_DRAM_WR_END_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_M_DRAM_WR_END_CLR);
        break;
    case ISR_DRAM_SUB_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_P_DRAM_WR_END_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_P_DRAM_WR_END_CLR);
        break;
    case ISR_DRAM_GFX_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_GFX_DRAM_WR_END_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_GFX_DRAM_WR_END_CLR);
        break;
    case ISR_DRAM_MON_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 1, SCSYS_09_INT_MON_DRAM_WR_END_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, 0, SCSYS_09_INT_MON_DRAM_WR_END_CLR);
        break;
    case ISR_GFX_VSYNC_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_VSYNC_R_CLR);
        break;
    case ISR_GFX_VSYNC_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_VSYNC_F_CLR);
        break;
    case ISR_GFX_VDE_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_VDE_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_VDE_R_CLR);
        break;
    case ISR_GFX_VDE_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_VDE_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_VDE_F_CLR);
        break;
    case ISR_GFX_OSD_VSYNC_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_OSD_VSYNC_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_OSD_VSYNC_R_CLR);
        break;
    case ISR_GFX_OSD_VSYNC_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_OSD_VSYNC_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_OSD_VSYNC_F_CLR);
        break;
    case ISR_GFX_OSD_VDE_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_OSD_VDE_R_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_OSD_VDE_R_CLR);
        break;
    case ISR_GFX_OSD_VDE_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_GFX_OSD_VDE_F_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_GFX_OSD_VDE_F_CLR);
        break;
    case ISR_REF_FSC_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_REF2FSC_PUL_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_REF2FSC_PUL_CLR);
        break;
    case ISR_REF_PSC_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_REF2PSC_PUL_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_REF2PSC_PUL_CLR);
        break;
    case ISR_REF_GFX_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_REF2GFX_PUL_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_REF2GFX_PUL_CLR);
        break;
    case ISR_REF_B2R_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_REF2B2R_PUL_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_REF2B2R_PUL_CLR);
        break;
    case ISR_REF_R2R_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_REF2R2R_PUL_CLR);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_REF2R2R_PUL_CLR);
        break;
	case ISR_PQ_TDSHARP:		
		SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 1, SCSYS_7B_INT_TDSHARP_CLR);
		SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, 0, SCSYS_7B_INT_TDSHARP_CLR);
		break;
    default:
        break;
    }
}

void vScpipSetISREn(UINT32 u4Sel, UINT32 u4En)
{
    switch(u4Sel)
    {
    case ISR_FSC_MAIN_VSYNC_IN_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_M_IN_VSYNC_R_EN);
        break;
    case ISR_FSC_MAIN_VSYNC_IN_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_M_IN_VSYNC_F_EN);
        break;
    case ISR_FSC_SUB_VSYNC_IN_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_P_IN_VSYNC_R_EN);
        break;
    case ISR_FSC_SUB_VSYNC_IN_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_P_IN_VSYNC_F_EN);
        break;
    case ISR_FSC_VSYNC_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_FSC_OUT_VSYNC_R_EN);
        break;
    case ISR_FSC_VSYNC_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_FSC_OUT_VSYNC_F_EN);
        break;
    case ISR_FSC_VDE_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_FSC_OUT_VDE_R_EN);
        break;
    case ISR_FSC_VDE_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_FSC_OUT_VDE_F_EN);
        break;
    case ISR_PSC_VSYNC_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_PSC_OUT_VSYNC_R_EN);
        break;
    case ISR_PSC_VSYNC_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_PSC_OUT_VSYNC_F_EN);
        break;
    case ISR_PSC_VDE_OUT_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_PSC_OUT_VDE_R_EN);
        break;
    case ISR_PSC_VDE_OUT_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_PSC_OUT_VDE_F_EN);
        break;
    case ISR_DRAM_MAIN_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_M_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_SUB_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_P_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_GFX_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_GFX_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_MON_WRITE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_09, u4En, SCSYS_09_INT_MON_DRAM_WR_END_EN);
        break;
    case ISR_GFX_VSYNC_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_VSYNC_R_EN);
        break;
    case ISR_GFX_VSYNC_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_VSYNC_F_EN);
        break;
    case ISR_GFX_VDE_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_VDE_R_EN);
        break;
    case ISR_GFX_VDE_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_VDE_F_EN);
        break;
    case ISR_GFX_OSD_VSYNC_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_OSD_VSYNC_R_EN);
        break;
    case ISR_GFX_OSD_VSYNC_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_OSD_VSYNC_F_EN);
        break;
    case ISR_GFX_OSD_VDE_RISING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_OSD_VDE_R_EN);
        break;
    case ISR_GFX_OSD_VDE_FALLING:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_GFX_OSD_VDE_F_EN);
        break;
    case ISR_REF_FSC_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_REF2FSC_PUL_EN);
        break;
    case ISR_REF_PSC_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_REF2PSC_PUL_EN);
        break;
    case ISR_REF_GFX_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_REF2GFX_PUL_EN);
        break;
    case ISR_REF_B2R_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_REF2B2R_PUL_EN);
        break;
    case ISR_REF_R2R_PULSE:
        SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_REF2R2R_PUL_EN);
        break;
	case ISR_PQ_TDSHARP:
		SCPIP_WRITE_FLD(SCPIP_SCSYS_7B, u4En, SCSYS_7B_INT_TDSHARP_EN);
	break;
    default:
        break;
    }
}

UINT32 u4ScpipGetISREn(UINT32 u4Sel)
{
    UINT32 u4status;

    switch(u4Sel)
    {
    case ISR_FSC_MAIN_VSYNC_IN_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_M_IN_VSYNC_R_EN);
        break;
    case ISR_FSC_MAIN_VSYNC_IN_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_M_IN_VSYNC_F_EN);
        break;
    case ISR_FSC_SUB_VSYNC_IN_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_P_IN_VSYNC_R_EN);
        break;
    case ISR_FSC_SUB_VSYNC_IN_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_P_IN_VSYNC_F_EN);
        break;
    case ISR_FSC_VSYNC_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_FSC_OUT_VSYNC_R_EN);
        break;
    case ISR_FSC_VSYNC_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_FSC_OUT_VSYNC_F_EN);
        break;
    case ISR_FSC_VDE_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_FSC_OUT_VDE_R_EN);
        break;
    case ISR_FSC_VDE_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_FSC_OUT_VDE_F_EN);
        break;
    case ISR_PSC_VSYNC_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_PSC_OUT_VSYNC_R_EN);
        break;
    case ISR_PSC_VSYNC_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_PSC_OUT_VSYNC_F_EN);
        break;
    case ISR_PSC_VDE_OUT_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_PSC_OUT_VDE_R_EN);
        break;
    case ISR_PSC_VDE_OUT_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_PSC_OUT_VDE_F_EN);
        break;
    case ISR_DRAM_MAIN_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_M_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_SUB_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_P_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_GFX_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_GFX_DRAM_WR_END_EN);
        break;
    case ISR_DRAM_MON_WRITE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_09, SCSYS_09_INT_MON_DRAM_WR_END_EN);
        break;
    case ISR_GFX_VSYNC_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_VSYNC_R_EN);
        break;
    case ISR_GFX_VSYNC_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_VSYNC_F_EN);
        break;
    case ISR_GFX_VDE_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_VDE_R_EN);
        break;
    case ISR_GFX_VDE_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_VDE_F_EN);
        break;
    case ISR_GFX_OSD_VSYNC_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_OSD_VSYNC_R_EN);
        break;
    case ISR_GFX_OSD_VSYNC_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_OSD_VSYNC_F_EN);
        break;
    case ISR_GFX_OSD_VDE_RISING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_OSD_VDE_R_EN);
        break;
    case ISR_GFX_OSD_VDE_FALLING:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_GFX_OSD_VDE_F_EN);
        break;
    case ISR_REF_FSC_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_REF2FSC_PUL_EN);
        break;
    case ISR_REF_PSC_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_REF2PSC_PUL_EN);
        break;
    case ISR_REF_GFX_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_REF2GFX_PUL_EN);
        break;
    case ISR_REF_B2R_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_REF2B2R_PUL_EN);
        break;
    case ISR_REF_R2R_PULSE:
        u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_REF2R2R_PUL_EN);
        break;
	case ISR_PQ_TDSHARP:
		u4status = (UINT32)SCPIP_READ_FLD(SCPIP_SCSYS_7B, SCSYS_7B_INT_TDSHARP_EN);
	break;		
    default:
        u4status = (UINT32)u2IO32Read2B(SCPIP_SCSYS_09);
        break;
    }

    return u4status;
}

void vScpipCRCsysEn(UINT8 bPath, UINT8 u1InOut, UINT8 u1En)
{
    if (bPath == SV_VP_MAIN && u1InOut == 0) // main in, top l/bottom l
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_31, u1En, SCSYS_31_SC_MIN_CRC_TOP_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_31, u1En, SCSYS_31_SC_MIN_CRC_BOT_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_31, u1En, SCSYS_31_SC_MIN_CRC_TOP_R_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_31, u1En, SCSYS_31_SC_MIN_CRC_BOT_R_EN);
    }
    else if (bPath == SV_VP_MAIN && u1InOut == 1) // main out top r?
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_TOP_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_BOT_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_TOP_R_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_BOT_R_EN);
    }
    else if (bPath == SV_VP_PIP && u1InOut == 0) // sub in, top l/bottom l
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_36, u1En, SCSYS_36_SC_PIN_CRC_TOP_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_36, u1En, SCSYS_36_SC_PIN_CRC_BOT_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_36, u1En, SCSYS_36_SC_PIN_CRC_TOP_R_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_36, u1En, SCSYS_36_SC_PIN_CRC_BOT_R_EN);
    }
    else  // sub out, top l
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_TOP_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_BOT_L_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_TOP_R_EN);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_BOT_R_EN);
    }

}

void vScpipCRCCheckSumMode(UINT8 bPath,UINT8 u1InOut, UINT8 u1En)
{
    if (bPath == SV_VP_MAIN && u1InOut == 0) // main in, top l/bottom l
    {
        SCPIP_WRITE_FLD(SCPIP_SCSYS_31, u1En, SCSYS_31_SC_MIN_CRC_CHECK_SUM_MODE);
    }
	else if ( bPath == SV_VP_MAIN )
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_CHECK_SUM_MODE);
	}
	else if (bPath == SV_VP_PIP && u1InOut == 0) // sub in, top l/bottom l
	{
	   SCPIP_WRITE_FLD(SCPIP_SCSYS_36, u1En, SCSYS_36_SC_PIN_CRC_CHECK_SUM_MODE);
	}
	else	// sub out, top l
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_CHECK_SUM_MODE);
	}
}


void vScpipCRCVsyncInv(UINT8 bPath, UINT8 u1En)
{
	if ( bPath == SV_VP_MAIN )
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3B, u1En, SCSYS_3B_SC_MOUT_CRC_VSYNC_INV);
	}
	else	// sub out, top l
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_40, u1En, SCSYS_40_SC_POUT_CRC_VSYNC_INV);
	}
}

void vScpipCRCClipWidthHeight(UINT8 bPath, UINT32 u4Width, UINT32 u4Height)
{
	if ( bPath == SV_VP_MAIN )
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3C, u4Width , SCSYS_3C_SC_MOUT_CRC_CLIP_H_END);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_3C, u4Height , SCSYS_3C_SC_MOUT_CRC_CLIP_V_END);
	}
	else
	{
        SCPIP_WRITE_FLD(SCPIP_SCSYS_32, u4Width , SCSYS_32_SC_MIN_CRC_CLIP_V_END);
        SCPIP_WRITE_FLD(SCPIP_SCSYS_32, u4Height , SCSYS_32_SC_MIN_CRC_CLIP_V_END);
	}
}


UINT32 u4ScpipCRCsysHstatus(UINT8 bPath, UINT8 u1InOut)
{
    UINT32 u4status;

    if (bPath == SV_VP_MAIN && u1InOut == 0) // main in
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_34, SCSYS_34_SC_MIN_STA_CRC_OUT_H);
    }
    else if (bPath == SV_VP_MAIN && u1InOut == 1) // main out
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_3E, SCSYS_3E_SC_MOUT_STA_CRC_OUT_H);
    }
    else if (bPath == SV_VP_PIP && u1InOut == 0) // sub in
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_39, SCSYS_39_SC_PIN_STA_CRC_OUT_H);
    }
    else  // sub out
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_43, SCSYS_43_SC_POUT_STA_CRC_OUT_H);
    }
    
    return u4status;
}

UINT32 u4ScpipCRCsysVstatus(UINT8 bPath, UINT8 u1InOut)
{
    UINT32 u4status;

    if (bPath == SV_VP_MAIN && u1InOut == 0) // main in
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_35, SCSYS_35_SC_MIN_STA_CRC_OUT_V);
    }
    else if (bPath == SV_VP_MAIN && u1InOut == 1) // main out
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_3F, SCSYS_3F_SC_MOUT_STA_CRC_OUT_V);
    }
    else if (bPath == SV_VP_PIP && u1InOut == 0) // sub in
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_3A, SCSYS_3A_SC_PIN_STA_CRC_OUT_V);
    }
    else  // sub out
    {
        u4status = SCPIP_READ_FLD(SCPIP_SCSYS_44, SCSYS_44_SC_POUT_STA_CRC_OUT_V);
    }
    
    return u4status;
}






//
//if dual hybrid mode set force 2d mode
//
void vScpipForce2dMode( UINT8 u1OnOff ) 
{
    SCPIP_WRITE_FLD(SCPIP_SCCTRL1_00, u1OnOff, SCCTRL1_00_SC_HSC_FORCE_2D_1);
}



void vScpipSetGfxOsdDly_H( UINT32 u4Dly_H ) 
{
	vIO32WriteFldAlign(SCPIP_SCSYS_67, u4Dly_H, SCSYS_67_GFX_OSD_HSYNC_DLY); //sony v total issue
}

void vScpipSetGfxOsdDly_V( UINT32 u4Dly_V ) 
{
	vIO32WriteFldAlign(SCPIP_SCSYS_67, u4Dly_V, SCSYS_67_GFX_OSD_VSYNC_DLY); //sony v total issue
}


#if defined(CC_MT5890)
/*
   set sc time gen enable/disable
*/
void vScpipFscTGEnable( UINT8 u1OnOff )
{
     SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u1OnOff, SCSYS_06_SC_TG_ENABLE );
}  
void vScpipGfxTGEnable( UINT8 u1OnOff )
{
     SCPIP_WRITE_FLD(SCPIP_SCSYS_65, u1OnOff, SCSYS_65_GFX_TG_ENABLE );
}

/*
   set sc time gen slave mode enable/disable
*/
void vScpipFscTGSalveMode( UINT8 u1OnOff )
{
      SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u1OnOff, SCSYS_06_SC_TG_SLAVE_MODE );
}  

void vScpipFscTGSalveSyncMode( UINT8 u1OnOff )
{
      SCPIP_WRITE_FLD(SCPIP_SCSYS_06, u1OnOff, SCSYS_06_SC_TG_SLAVE_SYNC_MODE );
}  

void vScpipGfxTGSlaveMode( UINT8 u1OnOff )
{
     SCPIP_WRITE_FLD(SCPIP_SCSYS_65, u1OnOff, SCSYS_65_GFX_TG_SLAVE_MODE );
}

void vScpipGfxTGSlaveSyncMode( UINT8 u1OnOff )
{
     SCPIP_WRITE_FLD(SCPIP_SCSYS_65, u1OnOff, SCSYS_65_GFX_TG_SLAVE_SYNC_MODE );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/*
0: FSC time-gen
1: GFX time-gen
*/
void vScpipTGInOutSel( UINT8 u1Swicth )
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_00, u1Swicth, SCSYS_00_SC_TIME_GEN_IN_SEL );
    SCPIP_WRITE_FLD(SCPIP_SCSYS_00, u1Swicth, SCSYS_00_SC_TIME_GEN_OUT_SEL );
}

/*
OSD timing from:
0: FSC time gen
1: PSC time gen
2: GFX time gen
*/
void vScpipOsdTgSrcSel( UINT8 u1OsdSrcSel )
{
    SCPIP_WRITE_FLD(SCPIP_SCSYS_14, u1OsdSrcSel , SCSYS_14_SC_OSD_SRC_SEL );
}



void vScpipDisableDramWriteARB(UINT8 u1Path, UINT8 u1OnOff)
{
    if (u1Path == VDP_1)
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_14, u1OnOff, DRAM_M_14_DA_ARB_MAINW_EN);
    }
    else
    {
        SCPIP_WRITE_FLD(SCPIP_DRAM_M_14, u1OnOff, DRAM_M_14_DA_ARB_PIPW_EN);
    }        
}


void vScpipInitR2rTargetline(UINT16 u2B2rTgtLine)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_79, u2B2rTgtLine, SCSYS_79_R2R_TARGET_LINE);
}


void vScpipInitB2rTargetline(UINT16 u2B2rTgtLine)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_78, u2B2rTgtLine, SCSYS_78_B2R_TARGET_LINE);
}

void vScpipInitPscTargetline(UINT16 u2PscTgtLine)
{
	SCPIP_WRITE_FLD(SCPIP_SCSYS_76, u2PscTgtLine, SCSYS_76_PSC_TARGET_LINE);
}




#endif //the function support only on MT5890


