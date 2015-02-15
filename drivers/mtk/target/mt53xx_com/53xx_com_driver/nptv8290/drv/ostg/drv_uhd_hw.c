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

#ifdef CC_SUPPORT_UHD
#include "hw_uhd.h"
#include "drv_uhd.h"
#include "drv_lvds.h"
#include "panel.h"



extern UHD_INFO_T sUhdSysInfo;
extern UHD_INFO_T sBakUhdSysInfo;

#ifdef SUPPORT_PANEL_GRAPH_MODE
EXTERN void vDrvSwitchGFXTiming(UINT32 u4Type);
#endif

UINT8 fgUhdSetdata=OFF;
UINT8 fgUhdPowerOn=OFF;


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

/**************************************************************************
 * Global/Static variables
 *************************************************************************/

//-----------------------------------------------------------------------------
// extern functions
//-----------------------------------------------------------------------------
EXTERN UINT8 UhdPowerOn(void);
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

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief of vUhdHwInit.
 * Initial UHD related setting
 */
//-----------------------------------------------------------------------------
void vUhdRefAutoSync(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_CTRL0, bOnOff, REF_VSYNC_AUTO);
}

void vUhdRefSyncDly(UINT32 u4DlyCnt)
{
	UHD_WRITE_FLD(UHD_CTRL0, u4DlyCnt, REF_VSYNC_DELAY);
}

void vUhdrlImportEnable(UINT32 u4DlyCnt)
{
	UHD_WRITE_FLD(UHD_CTRL0, u4DlyCnt, UHD_CTRL_IMPORT_ENABLE);
}

void vUhdRefSyncSelect(UINT32 u4VsyncSel)
{
	UHD_WRITE_FLD(UHD_CTRL0, u4VsyncSel, REF_VSYNC_SEL);
}
void vUhdP2SEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_CTRL0, bOnOff, UHD_P2S_EN);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



void vUhdColorFormatTransform(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_3X3_0, bOnOff, CT_EN);
}

void vUhdColorTableSelect(UINT8 bCtSelect)
{
	UHD_WRITE_FLD(UHD_3X3_0, bCtSelect, INT_TABLE_SEL);
}


void vUhdTgtoSlaveMode(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_06, bOnOff, UHD_TG_SLAVE_MODE);
}
void vUhdTgEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_06, bOnOff, UHD_TG_ENABLE);
}

//0: vsync 1: vde
void vUhdSelectOsdVsync(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_14, bOnOff, UHD_OSD_VSYNC_SEL);
}


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void vUhdSelect2SectionCtrl(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_S2P0, bOnOff, RG_2SEC_EN);
}

//---------------------------------------------------------------------------------------------
/*
UHD sc ctrl
*/
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

void vUhdSetInputWidthHeight(UINT32 u4Width, UINT32 u4Height)
{
	UHD_WRITE_FLD(UHD_SCCTRL_09, u4Width, UHD_INPUT_WIDTH);
	UHD_WRITE_FLD(UHD_SCCTRL_09, u4Height, UHD_INPUT_HEIGHT);
}
void vUhdSetOutputWidthHeight(UINT32 u4Width, UINT32 u4Height)
{
	UHD_WRITE_FLD(UHD_SCCTRL_11, u4Width, UHD_OUTPUT_WIDTH);
	UHD_WRITE_FLD(UHD_SCCTRL_11, u4Height, UHD_OUTPUT_HEIGHT);
}

void vUhdSetVmskStartOffset(UINT32 u4Offset)
{
	u4Offset&=0x1ffff; 
	UHD_WRITE_FLD(UHD_SCCTRL_0E, u4Offset, UHD_VMSK_STR_OFST_1);
}

void vUhdSetScalingSignBit(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCCTRL_00, bOnOff, UHD_H_PHASE_INIT_SIGN);
}

void vUhdSetScalingDeltaInt(UINT8 u4DeltaInt)
{
	UHD_WRITE_FLD(UHD_SCCTRL_01, u4DeltaInt, UHD_H_PHASE_DELTA_INTG);
}

void vUhdSetScalingDeltaFrac(UINT32 u4DeltaFrac)
{
	UHD_WRITE_FLD(UHD_SCCTRL_01, u4DeltaFrac, UHD_H_PHASE_DELTA_FRAC);
}


void vUhdWidthScalingCal(UINT32 U4InWidth,UINT32 u4OutWidth)
{
    UINT32 u4ScFactor;
	UINT32 u4PhaseDelta_Int,U4PhaseDelta_Frac;

	u4ScFactor=(U4InWidth * 0x100000 /u4OutWidth);
	u4PhaseDelta_Int  = u4ScFactor >> 20;
	U4PhaseDelta_Frac = u4ScFactor & 0xfffff;
    vUhdSetScalingSignBit(1);   //almost fixed at "1"
    vUhdSetScalingDeltaInt(u4PhaseDelta_Int);
	vUhdSetScalingDeltaFrac(U4PhaseDelta_Frac);
}


void vUhdSetScImportEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCCTRL_0A, bOnOff, UHD_SET_RES_IMPORT_ENABLE);
}

void vUhdScSetResToggle(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCCTRL_0A, bOnOff, UHD_SET_RES_TOGGLE);
}


//---------------------------------------------------------------------------------------------
//panel timing
void vUhdSetPanelHtotal(UINT32 u4Htotal)
{
	UHD_WRITE_FLD(UHD_SCSYS_03, u4Htotal, UHD_PNL_HTOTAL);
}

UINT32 u4UhdGetPanelHtotal(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_03, UHD_PNL_HTOTAL));
}

void vUhdSetPanelVtotal( UINT32 u4Vtotal)
{
	UHD_WRITE_FLD(UHD_SCSYS_03, u4Vtotal, UHD_PNL_VTOTAL);
}

UINT32 u4UhdGetPanelVtotal(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_03, UHD_PNL_VTOTAL));
}

void vUhdSetPanelHtotalVtotal(UINT32 u4Htotal, UINT32 u4Vtotal)
{
	UHD_WRITE_FLD(UHD_SCSYS_03, u4Htotal, UHD_PNL_HTOTAL);
	UHD_WRITE_FLD(UHD_SCSYS_03, u4Vtotal, UHD_PNL_VTOTAL);
}

void vUhdSetPanelWidth(UINT32 u4Width)
{
	UHD_WRITE_FLD(UHD_SCSYS_02, u4Width, UHD_PNL_WIDTH);
}

UINT32 u4UhdGetPanelWidth(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_02, UHD_PNL_WIDTH));
}

void vUhdSetPanelHeight(UINT32 u4Height)
{
	UHD_WRITE_FLD(UHD_SCSYS_02, u4Height, UHD_PNL_HEIGHT);
}

UINT32 u4UhdGetPanelHeight(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_02, UHD_PNL_HEIGHT));
}

void vUhdSetPanelWidthHeight(UINT32 u4Width, UINT32 u4Height)
{
	UHD_WRITE_FLD(UHD_SCSYS_02, u4Width, UHD_PNL_WIDTH);
	UHD_WRITE_FLD(UHD_SCSYS_02, u4Height, UHD_PNL_HEIGHT);
}

void vUhdSetOutputPanelHBackporch(UINT32 u4BackProch)
{
	UHD_WRITE_FLD(UHD_SCSYS_04, u4BackProch, UHD_PNL_BACK_HPORCH);
}


void vUhdSetOutputPanelHFrontporch(UINT32 u4FntProch)
{
	UHD_WRITE_FLD(UHD_SCSYS_04, u4FntProch, UHD_PNL_FRNT_HPORCH);
}

UINT32 vUhdGetOutputPanelHBackporch(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_04, UHD_PNL_BACK_HPORCH));
}

UINT32 vUhdGetOutputPanelHFrontporch(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_04, UHD_PNL_FRNT_HPORCH));
}

void vUhdSetOutputPanelVBackporch(UINT32 u4BackProch)
{
	UHD_WRITE_FLD(UHD_SCSYS_05, u4BackProch, UHD_PNL_BACK_VPORCH);
}

void vUhdSetOutputPanelVFrontporch( UINT32 u4FntProch)
{
	UHD_WRITE_FLD(UHD_SCSYS_05, u4FntProch, UHD_PNL_FRNT_VPORCH);
}

UINT32 vUhdGetOutputPanelVBackporch(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_05, UHD_PNL_BACK_VPORCH));
}

UINT32 vUhdGetOutputPanelVFrontporch(void)
{
	return(UHD_READ_FLD(UHD_SCSYS_05, UHD_PNL_BACK_VPORCH));
}

void vUhdTgImportEnable(UINT8 bOnOff)
{
  UHD_WRITE_FLD(UHD_SCSYS_01, bOnOff, UHD_TG_IMPORT_ENABLE);
}

void vUhdSetData(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_01, bOnOff, UHD_SET_DATA);  
}

void vUhdSetInputPatternGenBoundaryEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_21, bOnOff, UHD_INPTG_BND_EN_1);
}

void vUhdSetInputPatternGenEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_23, bOnOff, UHD_INPTG_ENABLE_1);
}

void vUhdSetInputPatternGenBoundary(UINT32 u4Hboundary,UINT32 u4Vboundary)
{
	UHD_WRITE_FLD(UHD_SCSYS_22, u4Hboundary, UHD_INPTG_BND_W_1);//width
	UHD_WRITE_FLD(UHD_SCSYS_22, u4Vboundary, UHD_INPTG_BND_H_1);//height
}

void vUhdSetInputPatternGenFreeRunEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_21, bOnOff, UHD_INPTG_FREERUN_EN_1);
}

void vUhdSetInputPatternGenAngle(UINT32 u4Angle)
{
	UHD_WRITE_FLD(UHD_SCSYS_21, u4Angle, UHD_INPTG_ANGLE_SEL_1);
}

void vUhdSetInputPatternGenFreeRunSpeed(UINT32 u4Speed)
{
	UHD_WRITE_FLD(UHD_SCSYS_21, u4Speed, UHD_INPTG_FREERUN_SPEED_1);
}

void vUhdSetOutputPatternGenEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_23, bOnOff, UHD_OUTPTG_ENABLE_1);
}

void vUhdSetOutputPatternGenAngle(UINT32 u4Angle)
{
	UHD_WRITE_FLD(UHD_SCSYS_24, u4Angle, UHD_OUTPTG_ANGLE_SEL_1);
}

void vUhdSetOutputPatternGenBoundaryEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_24, bOnOff, UHD_OUTPTG_BND_EN_1);
}
void vUhdSetOutputPatternGenBoundary(UINT32 u4Hboundary,UINT32 u4Vboundary)
{
	UHD_WRITE_FLD(UHD_SCSYS_25, u4Hboundary, UHD_OUTPTG_BND_W_1);//width
	UHD_WRITE_FLD(UHD_SCSYS_25, u4Vboundary, UHD_OUTPTG_BND_H_1);//height
}

void vUhdSetOutputPatternGenFreeRunEnable(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_SCSYS_24, bOnOff, UHD_OUTPTG_FREERUN_EN_1);
}

void vUhdSetOutputPatternGenFreeRunSpeed(UINT32 u4Speed)
{
	UHD_WRITE_FLD(UHD_SCSYS_24, u4Speed, UHD_OUTPTG_FREERUN_SPEED_1);
}

void vUhdSetPostScalerPattern(UINT32 u4InOut, UINT32 u4OnOff)
{
	UINT32 u4Width,u4Height;

	u4Width  = wDISPLAY_WIDTH;
    u4Height = wDISPLAY_HEIGHT;	
	
	if (u4InOut == 0) // uhd post scaler in
	{
        if (u4OnOff == 0)
        {
        	vUhdSetInputPatternGenBoundaryEnable(SV_OFF);
        	vUhdSetInputPatternGenEnable(SV_OFF);
            return;
        }

		vUhdSetInputPatternGenAngle(6);
		vUhdSetInputPatternGenBoundaryEnable(SV_ON);
		vUhdSetInputPatternGenBoundary(u4Width/2, u4Height - 1);
		vUhdSetInputPatternGenFreeRunEnable(1);
		vUhdSetInputPatternGenFreeRunSpeed(4);
		vUhdSetInputPatternGenEnable(SV_ON);
	}
	else // uhd post scaler out
	{
        if (u4OnOff == 0)
        {
        	vUhdSetOutputPatternGenBoundaryEnable(SV_OFF);
            vUhdSetOutputPatternGenEnable(SV_OFF);
            return;
        }

		vUhdSetOutputPatternGenAngle(6);
		vUhdSetOutputPatternGenFreeRunEnable(1);
		vUhdSetOutputPatternGenFreeRunSpeed(0xB);
		vUhdSetOutputPatternGenBoundary(u4Width, u4Height - 1);
		vUhdSetOutputPatternGenBoundaryEnable(SV_ON);
		vUhdSetOutputPatternGenEnable(SV_ON);
	}
}

EXTERN void vScpipSetTimeGenEnable( E_TG_MODULE u1Module , UINT8 u1OnOff );

void vResetUhdTimingGen(void)
{
	//Printf("--vResetUhdTimingGen-\n");
	vScpipSetTimeGenEnable( E_GFX_TG, SV_OFF );
	vScpipSetTimeGenEnable( E_FSC_TG, SV_OFF );
	vUhdTgEnable(0);

	HAL_Delay_us(30000);  //delay 20ms
	vScpipSetTimeGenEnable( E_GFX_TG, SV_ON );
	vScpipSetTimeGenEnable( E_FSC_TG, SV_ON );
	//vUhdTgEnable(1);


	//vUhdTgEnable(0);
	HAL_Delay_us(30000);  //delay 20ms
	vUhdTgEnable(1);
}

void vUhdSetOutputPanelTiming(UINT32 u4Htotal, UINT32 u4Vtotal, UINT32 u4Width, UINT32 U4Height, UINT32 u4Hbp, UINT32 u4Hfp, UINT32 u4Vbp, UINT32 u4Vfp)
{
	vUhdTgImportEnable(1);
	vUhdSetPanelHtotalVtotal(u4Htotal,u4Vtotal);
	vUhdSetPanelWidthHeight(u4Width,U4Height);
	vUhdSetOutputPanelHBackporch(u4Hbp);
	vUhdSetOutputPanelHFrontporch(u4Hfp);
	vUhdSetOutputPanelVBackporch(u4Vbp);
	vUhdSetOutputPanelVFrontporch(u4Vfp);

	if(IS_VB1_2SEC_ENABLE)
	{
		vIO32WriteFldAlign(UHD_S2P1, (u4Htotal/2) - 1, RG_H_TOTAL);
		vIO32WriteFldAlign(UHD_S2P3, u4Vtotal, RG_V_TOTAL);
		//vIO32WriteFldAlign(UHD_S2P3, U4Height, RG_V_WIDTH);
		vIO32WriteFldAlign(UHD_S2P1, (u4Width/2)-1, RG_H_VALID);
	}
}
//---------------------------------------------------------------------------------------------
void vUhdSetScaling(UINT32 u4InWidth,UINT32 u4InHeight, UINT32 u4OutWidth, UINT32 u4OutHeight )
{
	vUhdSetScImportEnable(1);
    vUhdSetInputWidthHeight(u4InWidth,u4InHeight);
	vUhdSetOutputWidthHeight(u4OutWidth,u4OutHeight);
	vUhdWidthScalingCal(u4InWidth,u4OutWidth);
	vUhdScSetResToggle(1);
	HAL_Delay_us(1000);  //delay 20ms
	vUhdScSetResToggle(0);
}

//---------------------------------------------------------------------------------------------
void vUhdSetOsdDly( UINT32 u4Hdly, UINT32 u4Vdly )
{
	vScpipSetGfxOsdDly_H( u4Hdly );
	vScpipSetGfxOsdDly_V( u4Vdly );
}
//---------------------------------------------------------------------------------------------
void vUhdSetOsdMergeHtotal( UINT32 u4Htotal )
{
	UHD_WRITE_FLD(UHD_OSD_MERGE1, u4Htotal, OSD_MERGE_HTOTAL_OUT);
}
//---------------------------------------------------------------------------------------------
void vUhdGfxOsdSoftRst(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_RESET, bOnOff, GFX_OSD_SOFT_RST);
}
void vUhdOsdMergeSoftRst(UINT8 bOnOff)
{
	UHD_WRITE_FLD(UHD_RESET, bOnOff, OSD_MERGE_SOFT_RST);

}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

UINT32 u4UhdGetOutVCnt(void)
{
    return (UINT32)SCPIP_READ_FLD(UHD_SCSYS_20, UHD_STA_V_CNT);
}

//---------------------------------------------------------------------------------------------
#define R2R_REG15   (0xF0022000 + 0xcfc)
	#define R2R_REF2R2R_VSYNC_EN Fld(1,29,AC_MSKB3)//[29:29]

void vUhd_InitR2rSlaveMode(UINT8 bOnOff)
{
	UHD_WRITE_FLD(R2R_REG15, bOnOff, R2R_REF2R2R_VSYNC_EN);
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
#define B2R_REF2B2R_VSYNC   (0xF0026000 + 0x828)
	#define B2R_HD_USE_DISP_SYNC_RST  Fld(2,8,AC_MSKB1)//[9:8]
#define NB2R_V_FRONTPORCH	(0xF0026000 + 0x814)	
	#define NB2R_V_UP_PORCH Fld(12,20,AC_MSKW32)//[31:20]
#define NB2R_V_BACKPORCH	(0xF0026000 + 0x860)	
	#define NB2R_V_DOWN_PORCH Fld(12,13,AC_MSKDW)//[24:13]

	
void vUhd_InitB2rSlaveMode(UINT8 bOnOff)
{
	UHD_WRITE_FLD(B2R_REF2B2R_VSYNC, bOnOff, B2R_HD_USE_DISP_SYNC_RST);
}

void vB2RSetVporch( UINT32 u4FrontPorch, UINT32 u4backPorch)
{
	UHD_WRITE_FLD(NB2R_V_FRONTPORCH, u4FrontPorch, NB2R_V_UP_PORCH);
	UHD_WRITE_FLD(NB2R_V_BACKPORCH, u4backPorch, NB2R_V_DOWN_PORCH);
}
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
void vUhdHwInit(void)
{  
    UINT8 result; 
	//Printf("----vUhdHwInit----\n");
	result=UhdPowerOn();

	if ( result == FALSE )
	{
	   Printf("  --vUhdHwInit Fail!!!\n");
	   return;
	}
    //initial setting


	vUhdSelect2SectionCtrl(0);  //initial 2secen at off

	vUhdTgtoSlaveMode(0);
	vUhdTgtoSlaveMode(1);
   	vUhdSetOutputPanelTiming(4400,2250,3840,2160,0x0a,0x0a,0x0e,0x05);

    vUhdColorFormatTransform(1);   //enable color transform
    vUhdColorTableSelect(1); //select color table 1;

	vUhdRefAutoSync(0); //select to not "auto_sync....
	vUhdRefSyncSelect(REF_2_PSC);  //
	
	vUhdRefSyncDly(2200);  //set to half output line 4400/2
	vUhdrlImportEnable(0); //disable UHD ctrl import enable
	vUhdP2SEnable(0);      //uhd_p2s_en=0; 

    vDrvSetPanelIFRGBInOrder(1,2,0);  //yuv to rgb order setting y->g(1)  u->b(2)  v->r(0)

#ifdef SUPPORT_PANEL_GRAPH_MODE
    vDrvSwitchGFXTiming(PANEL_TYPE_2K2K_60);
#endif

	vUhdSetOsdDly(0x189,0x00);

    vUhdSetISR();


    //--------------------------------------------
    //initial r2r slave mode
	vUhd_InitR2rSlaveMode(1);
	vUhd_InitB2rSlaveMode(0x1);
	//--------------------------------------------
	
	vScpipInitR2rTargetline(R2R_TARGET_DLY_LINE);
	vScpipInitB2rTargetline(B2R_TARGET_DLY_LINE);  
	vScpipInitPscTargetline(PSC_TARGET_DLY_LINE);  //sharpness:3 + 1.x CBE

	vScpipSetTgImportOnOff(SV_ON);
	
}

#endif  //defined(CC_SUPPORT_UHD)



