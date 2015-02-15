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
 * $RCSfile: drv_scaler.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef _DRV_SCALER_UHD_H_
#define _DRV_SCALER_UHD_H_

#include "hw_uhd.h"
#include "panel.h"
#include "drv_display.h"
#include "frametrack_drvif.h"



#if defined(CC_SUPPORT_UHD)

#define UHD_WRITE_FLD(reg,value,fld) vIO32WriteFldAlign(reg, value, fld)
#define UHD_READ_FLD(reg,fld)        IO32ReadFldAlign(reg, fld)


#define  PSC_TARGET_DLY_LINE 	2     //sharpness:3 + 1.x CBE 
#define  R2R_TARGET_DLY_LINE 	PSC_TARGET_DLY_LINE     //
#define  B2R_TARGET_DLY_LINE 	(4+PSC_TARGET_DLY_LINE)     // PSC_TARGET_DLY_LINE + Vsync+Cbp - 8



//*****************************************************************************
typedef struct
{
	UINT8  u1SrcSelect;   //0: PQ_path. 1: BYPASS_PATH
	UINT32 u4InputWidth;
	UINT32 u4InputHeight;
	UINT32 u4OutWidth;
	UINT32 u4OutHeight;

	UINT32 u4PanelHtotal;
	UINT32 u4PanelVtotal;
	UINT32 u4PanelWidth;	
	UINT32 u4PanelHeight;	
	UINT32 u4PanelH_BP;	
	UINT32 u4PanelH_FP;	
	UINT32 u4PanelV_BP;	
	UINT32 u4PanelV_FP;
	UINT8  u1OutFrameRate;	
}UHD_INFO_T;



enum  UHD_VSYNC_SEL
{
	REF_2_B2R=0,
	REF_2_R2R,
	REF_2_FSC,
	REF_2_PSC,
	REF_2_GFX
};

enum  UHD_INPUT_SEL
{
	vPQ_PATH=0,
    vBYPASS_PATH
};


typedef enum {
	ISR_UHD_TDSHARP					 =0,  
	ISR_UHD_OUT_VSYNC_FALLING		 =1,  
	ISR_UHD_OUT_VSYNC_RISING		 =2,  
	ISR_POST_OUT_VSYNC_FALLING		 =3,  
	INT_POST_OUT_VSYNC_RISING		 =4, 
	ISR_UHD_V_CNT                    =5,
	ISR_SC_OUT_VDE_FALLING			 =6,  
	ISR_SC_OUT_VDE_RISING			 =7,  
	ISR_SC_OUT_VSYNC_FALLING		 =8,  
	ISR_SC_OUT_VSYNC_RISING		 	 =9,  
	ISR_IN_VSYNC_FALLING			 =10,  
	ISR_IN_VSYNC_RISING				 =11,  
	ISR_UHD_UNKNOWN
}ISR_UHD_SEL;


void vUhdHwInit(void);
void vUhdSwInit(void);
void vUhdSetISR(void);

void vUhdNormProc(void);
void vResetUhdTimingGen(void);
void vUhdSetData(UINT8 bOnOff);
void vUHD_VdoModeChgDone(void);
UINT32 vUhdGetOutputPanelHFrontporch(void);
UINT32 vUhdGetOutputPanelHBackporch(void);
void vUhdSetOutputPanelVBackporch(UINT32 u4BackProch);
void vUhdSetOutputPanelVFrontporch( UINT32 u4FntProch);
void vUhdSetOutputPanelHBackporch(UINT32 u4BackProch);
void vUhdSetOutputPanelHFrontporch(UINT32 u4FntProch);
UINT32 vUhdGetOutputPanelVBackporch(void);
UINT32 vUhdGetOutputPanelVFrontporch(void);
void vUhdSetPanelHtotal(UINT32 u4Htotal);
void vUhdSetPanelVtotal(UINT32 u4Vtotal);
void vUhdSetPanelWidth(UINT32 u4Width);
void vUhdSetPanelHeight(UINT32 u4Height);
UINT32 u4UhdGetPanelHtotal(void);
UINT32 u4UhdGetPanelVtotal(void);
UINT32 u4UhdGetPanelWidth(void);
UINT32 u4UhdGetPanelHeight(void);
void vUhdP2SEnable(UINT8 bOnOff);
void vUhdWidthScalingCal(UINT32 U4InWidth,UINT32 u4OutWidth);
void vUhdSetScaling(UINT32 u4InWidth,UINT32 u4InHeight, UINT32 u4OutWidth, UINT32 u4OutHeight );
void vDrvApplyUHDTiming(LCDTIMING_INFO_T *tLcdTiming);
void vUhdSetOutputPanelTiming(UINT32 u4Htotal, UINT32 u4Vtotal, UINT32 u4Width, UINT32 U4Height, UINT32 u4Hbp, UINT32 u4Hfp, UINT32 u4Vbp, UINT32 u4Vfp);
void vUhdSetOsdDly( UINT32 u4Hdly, UINT32 u4Vdly );
void vUhdSelectOsdVsync(UINT8 bOnOff);
void vUhdSetInputWidthHeight(UINT32 u4Width, UINT32 u4Height);
void vUhdSetOutputWidthHeight(UINT32 u4Width, UINT32 u4Height);
void vUhdQueryStatus(void);
void vUhdSetPostScalerPattern(UINT32 u4InOut, UINT32 u4OnOff);
void vUhdSetVmskStartOffset(UINT32 u4Offset);
void vUhdGfxOsdSoftRst(UINT8 bOnOff);
void vUhdOsdMergeSoftRst(UINT8 bOnOff);

void vUhdSetOsdMergeHtotal( UINT32 u4Htotal );
void vUhd_InitB2rSlaveMode(UINT8 bOnOff);
void vB2RSetVporch( UINT32 u4FrontPorch, UINT32 u4backPorch);
void vUhdRefSyncDly(UINT32 u4DlyCnt);

UINT32 u4UhdGetOutVCnt(void);
void vUhdTgImportEnable(UINT8 bOnOff);
void vResetUhdTimingGen(void);


EXTERN UINT32 IS_Support4KDisplay(void);
EXTERN UINT32 IS_Support4KDD(void);

#endif //#if defined(CC_SUPPORT_UHD)
#endif
