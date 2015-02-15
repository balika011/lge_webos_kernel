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
 * $RCSfile: drv_tcon.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifdef CC_UP8032_ATV
#include "general_mt82.h"
#endif

#ifdef CC_COPLAT_MT82
#include "util.h"
#endif

#include "general.h"

#include "x_pinmux.h"
#include "x_bim.h"
#include "x_assert.h"

#include "hw_ckgen.h"
#include "hw_tcon.h"
#include "hw_lvds.h"
#include "hw_mlvds.h"
#include "hw_tg.h"

#include "drv_tcon.h"
#include "drv_vdoclk.h"
#include "drv_lvds.h"
#include "drv_pwm.h"

#include "sif_if.h"
#include "panel.h"
#include "drv_display.h"

#include "nptv_debug.h"

#ifdef CC_SUPPORT_FLASH_TCON
#include "nor_if.h"
#ifdef CC_NAND_ENABLE
#include "nand_if.h"
#endif
#endif

#include "drvcust_if.h"

#ifdef SUPPORT_LOCK_FROM_SRVAD
#include "x_pdwnc.h"
#define SERVOADC_TCON_LOCK   3
#endif

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#define TCON_ADDR_BASE 0xf0000000

static UINT8 _u1TconCmd;
static UINT8 _u1GammaReady=0;
static UINT8 _u1TconReady;
static UINT8 _u1TconForceMute;
static UINT8 _u1TconState;
static UINT8 _u1TconStateOld;
static BOOL _fgTconCustSel=FALSE;
static VIDEO_TCON_TABLE_T _u1TconCustSel = EPIV12FHD120_42;
#ifdef CC_POWER_MERGE_TEST
static UINT8 _fgTconTestEnable=1;
static UINT8 _fgTCONTestcount=0;
#endif

//static BOOL _fgTconCuston=FALSE;

#ifndef CC_MTK_LOADER
static CHAR * _aszTconState[] =
{
    "TCON_STATE_NOTREADY",
    "TCON_STATE_INIT",
    "TCON_STATE_STANDBY",
    "TCON_STATE_MUTE",
    "TCON_STATE_NORMAL",
    "TCON_STATE_ERROR",
    "TCON_STATE_MAX"    
};
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
// DPM power control
static void vTconDPMCtrl(UINT8 u1Val)
{
  u1TconCustDPMCtrlEN1(u1Val);
  u1TconCustDPMCtrlEN2(u1Val);  	
}

void vTconRegWrite(UINT32 u4addr, UINT32 val)
{
    #ifndef CC_MTK_LOADER
    UINT32 u4tmp;
    #endif

	if ((u4addr == 0xffffffff) || ((u4addr&TCON_ADDR_BASE) != TCON_ADDR_BASE))
	{
		LOG(1,"Write addr Err!!! w 0x%x 0x%x\n", u4addr, val);
	}
    else
    {
	    vIO32Write4B(u4addr, val);
        #ifndef CC_MTK_LOADER
   	    u4tmp = u4IO32Read4B(u4addr);
        LOG(1,"0x%8x, 0x%8x, 0xffffffff,\n", u4addr, u4tmp);
        #endif
    }
}

UINT32 u4TconRegRead(UINT32 u4addr)
{
    UINT32 u4tmp;
	
	if ((u4addr == 0xffffffff) || ((u4addr&TCON_ADDR_BASE) != TCON_ADDR_BASE))
	{
		LOG(1,"Read addr Err!!! w 0x%x\n", u4addr);
		return (0);
	}
    else
    {
   	    u4tmp = u4IO32Read4B(u4addr);
        LOG(1,"0x%8x, 0x%8x, 0xffffffff,\n", u4addr, u4tmp);		
		return (u4tmp);
    }
}

static void vLogTconStateChange(UINT8 u1State)
{
    // Check if the state is correct
    if (u1State < TCON_STATE_MAX)
    {
        if (_u1TconStateOld != u1State)
        {
            #ifndef CC_MTK_LOADER
            LOG(1, "Tcon state change from %s to %s\n", _aszTconState[_u1TconStateOld],_aszTconState[u1State]);
            #endif
            _u1TconStateOld = u1State;
        }
    }
    else // Undefined state
    {
        if (_u1TconStateOld != u1State)
        {
            #ifndef CC_MTK_LOADER
            LOG(1, "Tcon state change from %s to UNDEFINED_STATE\n", _aszTconState[_u1TconStateOld]);
            #endif
            _u1TconStateOld = TCON_STATE_NOTREADY;
        }
    }
}

// Panel  Interface Pattern
void vDrvMLVDSPatternGen(UINT8 u1OnOff)
{

	 vIO32WriteFldAlign(VINTF_B6, 0, VINTF_PTGEN_MIRROR);
	 
    if (IS_PANEL_L12R12)
    {
     vIO32WriteFldAlign(VINTF_B6, 3, VINTF_PTGEN_TYPE);    
     vIO32WriteFldAlign(VINTF_B12, 3, VINTF_PTGEN_OSD_TYPE); 	 	 
     vIO32WriteFldAlign(VINTF_B6, 0, VINTF_PTGEN_SEQ);       
    }
	else
	{
     vIO32WriteFldAlign(VINTF_B6, 2, VINTF_PTGEN_TYPE);    	
     vIO32WriteFldAlign(VINTF_B12, 2, VINTF_PTGEN_OSD_TYPE); 	 
	 vIO32WriteFldAlign(VINTF_B6, 1, VINTF_PTGEN_SEQ);	
	}
    
    vIO32WriteFldAlign(VINTF_B1, PANEL_GetPanelWidth(), VINTF_PTGEN_H_ACTIVE);
    vIO32WriteFldAlign(VINTF_B1, wDrvGetOutputHTotal(), VINTF_PTGEN_H_TOTAL);    
    vIO32WriteFldAlign(VINTF_B2, PANEL_GetPanelHeight(), VINTF_PTGEN_V_ACTIVE);
    vIO32WriteFldAlign(VINTF_B11, PANEL_GetPanelHeight(), VINTF_PTGEN_V_OSD_ACTIVE);	
    vIO32WriteFldAlign(VINTF_B2, wDrvGetOutputVTotal(), VINTF_PTGEN_V_TOTAL);

    vIO32WriteFldAlign(VINTF_B6, (PANEL_GetPanelHeight()/4), VINTF_PTGEN_COLOR_BAR_TH);

    vIO32WriteFldAlign(VINTF_B4, IO32ReadFldAlign(EH_0C, EH_DE_HS_WIDTH), VINTF_PTGEN_H_WIDTH);
	
    if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)||(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
    {
     vIO32WriteFldAlign(VINTF_B4, 1, VINTF_PTGEN_V_WIDTH);

	 vIO32WriteFldAlign(PIXEL_REORDER_01, 0, RG_HS_OUT_POL);
	 
	 vIO32WriteFldAlign(PIXEL_REORDER_01, 1, RG_VS_OUT_POL);
	 vIO32WriteFldAlign(PIXEL_REORDER_15, 1, RG_VWIDTH_R);
	 vIO32WriteFldAlign(PIXEL_REORDER_16, 8, RG_VDE_VSTART_R);
	 vIO32WriteFldAlign(MUTE_PATTERN_05, 8, MUTE_V_START);
	 vIO32WriteFldAlign(VINTF_B3, 8, VINTF_PTGEN_V_START);
	 vIO32WriteFldAlign(EH_13,8,EH_PTGEN_V_START);

	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI) 
		{
		  vIO32WriteFldAlign(TMGR3, 4, TMGR3_RG_TCON_OUT_SEL);
		}
	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS)
		{
		
		vIO32WriteFldAlign(TMGR3, 1, TMGR3_RG_TCON_OUT_SEL);
		}
    }
	else
	{
	 if (IS_SEPARATE_OSD_AND_DATA)
	 {
	 	vIO32WriteFldAlign(VINTF_B11, 6, VINTF_PTGEN_V_OSD_START);
	 }	
	 	vIO32WriteFldAlign(VINTF_B4, IO32ReadFldAlign(EH_0D, EH_DE_VS_WIDTH)+1, VINTF_PTGEN_V_WIDTH);  
	}
    if (u1OnOff)
    {
        vIO32WriteFldAlign(VINTF_B6, 1, VINTF_PTGEN_EN);
        vIO32WriteFldAlign(EH_01, 0, EH_USE_SC_TIMING_EN);		
    }
    else
    {
        vIO32WriteFldAlign(VINTF_B6, 0, VINTF_PTGEN_EN);
        vIO32WriteFldAlign(EH_01, 1, EH_USE_SC_TIMING_EN);				
    }
}


/**
 * @brief u1DrvTconTestPattern

 * TCON test pattern

 * @param  void
 * @retval 0: fail, 1: success
 */

UINT8 u1DrvTconTP(UINT8 val)
{
		UINT8 u1Ret = 0;
		
#ifdef CC_MT5881
		//Pixel Reoder Pattern
	
		vIO32WriteFldAlign(MUTE_PATTERN_01, 0, MANUAL_PATTERN_ENABLE);
	  
	   if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)||(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
	   {
		vIO32WriteFldAlign(MUTE_PATTERN_03, PANEL_GetPanelWidth(), MUTE_H_ACTIVE);
		vIO32WriteFldAlign(MUTE_PATTERN_02, wDrvGetOutputHTotal(), MUTE_H_TOTAL);	
		vIO32WriteFldAlign(MUTE_PATTERN_05, PANEL_GetPanelHeight(), MUTE_V_ACTIVE);  
		vIO32WriteFldAlign(MUTE_PATTERN_04, wDrvGetOutputVTotal(), MUTE_V_TOTAL);
	
		vIO32WriteFldAlign(MUTE_PATTERN_05, 8, MUTE_V_START);
		vIO32WriteFldAlign(MUTE_PATTERN_03, 0x20, MUTE_H_START);
		
		vIO32WriteFldAlign(MUTE_PATTERN_02, IO32ReadFldAlign(EH_0C, EH_DE_HS_WIDTH), MUTE_H_WIDTH);
		vIO32WriteFldAlign(MUTE_PATTERN_04, 1, MUTE_V_WIDTH); 
		 
		vIO32WriteFldAlign(PIXEL_REORDER_01, 0, RG_HS_OUT_POL);
		vIO32WriteFldAlign(PIXEL_REORDER_01, 1, RG_VS_OUT_POL);
		
		vIO32WriteFldAlign(PIXEL_REORDER_15, 1, RG_VWIDTH_R);
		vIO32WriteFldAlign(PIXEL_REORDER_16, 8, RG_VDE_VSTART_R);
		
	   //TCON TOP
	   if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI) 
		   {
			 vIO32WriteFldAlign(TMGR3, 4, TMGR3_RG_TCON_OUT_SEL);
		   }
	   if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS)
		   {
		   
		   vIO32WriteFldAlign(TMGR3, 1, TMGR3_RG_TCON_OUT_SEL);
		   }
		}
	   // Pixel Reoder Pattern Setting
	   switch(val)
	   {
		   case TCON_TP_OFF:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 0, MANUAL_PATTERN_ENABLE);
			   break;
		   //Full Red   
		   case TCON_TP_R:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xF004, RG_BIST0);
			   break;
		   //Full Green	   
		   case TCON_TP_G:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xF02, RG_BIST0);
	
			   break;
		   //Full Blue   
		   case TCON_TP_B:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xF1, RG_BIST0);    
			   break;
		   //Full black   
		   case TCON_TP_BLACK:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0, RG_BIST0);		   
			   break;
		   //Full White	   
		   case TCON_TP_WHITE:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xFFF7, RG_BIST0);		  
			   break;
			   
		   case TCON_TP_COLORRAMP:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xC, RG_BIST0);
			   vIO32WriteFldAlign(MUTE_PATTERN_11, 0x10D, RG_V_COLOR_BAR_TH);
			   break;
			   
		   case TCON_TP_FLIKER_H:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xFFFF, RG_BIST0);
			   break;
			   
		   case TCON_TP_FLIKER_V:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 0, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xFFFE, RG_BIST0);
			   break;
			   
		   case TCON_TP_AUTO:
			   vIO32WriteFldAlign(MUTE_PATTERN_01, 1, MANUAL_PATTERN_ENABLE);
			   vIO32WriteFldAlign(MUTE_PATTERN_06, 6, RG_BIST_CNT);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xF004, RG_BIST0);
			   vIO32WriteFldAlign(MUTE_PATTERN_07, 0xF02, RG_BIST1);
			   vIO32WriteFldAlign(MUTE_PATTERN_08, 0xF1, RG_BIST2);
			   vIO32WriteFldAlign(MUTE_PATTERN_08, 0, RG_BIST3);
			   vIO32WriteFldAlign(MUTE_PATTERN_09, 0xFFF7, RG_BIST4);
			   vIO32WriteFldAlign(MUTE_PATTERN_09, 0xFFFF, RG_BIST5);
			   vIO32WriteFldAlign(MUTE_PATTERN_10, 0xFFFE, RG_BIST6);
			   break;
			   
		   default:
			   break;
		}
#else
		// Error Handling Pattern
		vIO32WriteFldAlign(EH_00, 0, EH_MUTE_MLVDS_EN);
		vIO32WriteFldAlign(EH_14,1,EH_PTGEN_TYPE_EN);
	
	 if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)||(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS))
		{
		
		 vIO32WriteFldAlign(PIXEL_REORDER_01, 0, RG_HS_OUT_POL);
		 vIO32WriteFldAlign(PIXEL_REORDER_01, 1, RG_VS_OUT_POL);
		 vIO32WriteFldAlign(PIXEL_REORDER_16, 8, RG_VDE_VSTART_R);
		 vIO32WriteFldAlign(EH_12,wDrvGetOutputVTotal()-1,EH_PTGEN_V_TOTAL);	  
		 vIO32WriteFldMulti(EH_13, P_Fld(1, EH_PTGEN_V_WIDTH)|P_Fld(8, EH_PTGEN_V_START));
	
		// TCON_TOP
		if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI) 
			{
			  vIO32WriteFldAlign(TMGR3, 4, TMGR3_RG_TCON_OUT_SEL);
			}
		if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS)
			{
			
			vIO32WriteFldAlign(TMGR3, 1, TMGR3_RG_TCON_OUT_SEL);
			}
		}
	
		// Error Handling Pattern Setting
		switch(val)
		{
			case TCON_TP_OFF:
				vIO32WriteFldAlign(EH_00, 0, EH_MUTE_MLVDS_EN);
				vIO32WriteFldAlign(EH_15, 2, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_TYPE_EN);
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_H_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_V_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_AUTO_CHG_PTGEN_EN);
				break;
				
			case TCON_TP_R:
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_R);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_G);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_B);
				break;
				
			case TCON_TP_G:
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_R);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_G);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_B);
				break;
				
			case TCON_TP_B:
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_R);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_G);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_B);			
				break;
				
			case TCON_TP_BLACK:
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_R);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_G);
				vIO32WriteFldAlign(EH_14, 0, EH_PTGEN_B);			
				break;
				
			case TCON_TP_WHITE:
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_R);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_G);
				vIO32WriteFldAlign(EH_14, 0xff, EH_PTGEN_B);		
				break;
				
			case TCON_TP_COLORRAMP:
				vIO32WriteFldAlign(EH_15, 2, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_H_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_V_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_AUTO_CHG_PTGEN_EN);
				break;
				
			case TCON_TP_MOVING:
				vIO32WriteFldAlign(EH_15, 2, EH_PTGEN_TYPE);
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_H_RUN);
				vIO32WriteFldAlign(EH_15, 1, EH_PTGEN_V_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_AUTO_CHG_PTGEN_EN); 		
				break;
				
			case TCON_TP_AUTO:
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_H_RUN);
				vIO32WriteFldAlign(EH_15, 0, EH_PTGEN_V_RUN);
				vIO32WriteFldAlign(EH_15, 1, EH_AUTO_CHG_PTGEN_EN);
				break;
				
			default:
				break;
	
		}
#endif
	
		return(u1Ret);	
	}


// Pixel Reorder Pattern
void u1DrvPixelReorderTP(UINT8 val)
{

	vIO32WriteFldAlign(MUTE_PATTERN_01, val, MANUAL_PATTERN_ENABLE);
	vIO32WriteFldAlign(MUTE_PATTERN_07, 0xC, RG_BIST0);
	vIO32WriteFldAlign(MUTE_PATTERN_11, 0x86, RG_H_COLOR_BAR_TH);

}

#define EH_PANEL_TABLE
#ifdef EH_PANEL_TABLE
#define EH_PANEL_FORMULATE 0
#endif
void vErrorHandleSetByPanel(void)
{
#ifdef EH_PANEL_TABLE 
	  	UINT32 H_LOW_TH,H_HIGH_TH;
	  	UINT32 V_LOW_TH,V_HIGH_TH; 
	  	UINT32 DE_LOW_TH,DE_HIGH_TH; 
	  	UINT32 VDE_LOW_TH,VDE_HIGH_TH;	   
	  	UINT32 EH_CLK_HIGH_TH=0, EH_CLK_LOW_TH=0;
      #if EH_PANEL_FORMULATE  
	  	  UINT32 EH_CLK_TOTAL_RATION,EH_TOTAL; 	    
      #endif
	
      //#ifdef NDEBUG
		   Printf("[SA7] Error handling init \n");
		   Printf("[SA7] PANEL_GetPanelWidth=0x%x, PANEL_GetPanelHeight=0x%x , wDrvGetOutputHTotal=0x%x , wDrvGetOutputVTotal=0x%x\n",
				  PANEL_GetPanelWidth(),PANEL_GetPanelHeight(),wDrvGetOutputHTotal(),wDrvGetOutputVTotal());		
      //#endif     
	
	
      #if EH_PANEL_FORMULATE             
		   H_LOW_TH = PANEL_GetPanelWidth() + 100;
		   H_HIGH_TH = wDrvGetOutputHTotal() * 2;
	
		   if (PANEL_GetPanelHeight()>=1060)
		   {
		   V_LOW_TH = PANEL_GetPanelHeight() + 30;
		   V_HIGH_TH = wDrvGetOutputVTotal() * 2;
		   }
		   else
		   {
		   V_LOW_TH = PANEL_GetPanelHeight() + 20;
		   V_HIGH_TH = wDrvGetOutputVTotal() * 2;
		   }
	
		   DE_HIGH_TH = PANEL_GetPanelWidth()+7;
		   DE_LOW_TH = PANEL_GetPanelWidth()-8;
	
		   VDE_HIGH_TH = PANEL_GetPanelHeight();
		   VDE_LOW_TH = PANEL_GetPanelHeight();
		   
		   if (IS_XTAL_24MHZ())
		   {
				EH_TOTAL = 1024*24000 / (PANEL_GetPixelClk60Hz()/1000);
		   }
		   else if (IS_XTAL_27MHZ())
		   {
				EH_TOTAL = 1024*27000 / (PANEL_GetPixelClk60Hz()/1000);
		   }
		   EH_CLK_TOTAL_RATION = EH_TOTAL / 4; 
	
		   EH_CLK_HIGH_TH = EH_TOTAL + EH_CLK_TOTAL_RATION;
		   EH_CLK_LOW_TH = EH_TOTAL - EH_CLK_TOTAL_RATION;
      #else      
      //#ifdef NDEBUG
		   Printf("[SA7] PANEL_GetHTotalMax=0x%x, PANEL_GetHTotalMin=0x%x, PANEL_GetVTotalMax=0x%x, PANEL_GetVTotalMin=0x%x \n",
				  PANEL_GetHTotalMax(),PANEL_GetHTotalMin(),PANEL_GetVTotalMax(),PANEL_GetVTotalMin()); 	   
		   Printf("[SA7] PANEL_GetPixelClkMax=%d, PANEL_GetPixelClkMin=%d \n",
				  PANEL_GetPixelClkMax(),PANEL_GetPixelClkMin());					   
      //#endif        
		   H_HIGH_TH = PANEL_GetHTotalMax();
		   H_LOW_TH = PANEL_GetHTotalMin();
	
		   V_HIGH_TH = PANEL_GetVTotalMax();
		   V_LOW_TH = PANEL_GetVTotalMin();
	
		   DE_HIGH_TH = PANEL_GetPanelWidth()+7;
		   DE_LOW_TH = PANEL_GetPanelWidth()-8;
	
		   VDE_HIGH_TH = PANEL_GetPanelHeight();
		   VDE_LOW_TH = PANEL_GetPanelHeight(); 	  
	
		   // fix error for mt8283
		   if (IS_XTAL_24MHZ())
		   	{
				EH_CLK_HIGH_TH = (1024*24000/ (PANEL_GetPixelClkMin()/ 1000));		 
		   		EH_CLK_LOW_TH = (1024*24000/ (PANEL_GetPixelClkMax()/ 1000));
		   }
		   else if (IS_XTAL_27MHZ())
		   	{
				EH_CLK_HIGH_TH = (1024*27000/ (PANEL_GetPixelClkMin()/ 1000));		 
		   		EH_CLK_LOW_TH = (1024*27000/ (PANEL_GetPixelClkMax()/ 1000));
		   }
      #endif
		 
       #ifdef NDEBUG
			Printf("[SA7] PANEL_GetPixelClk60Hz = 0x%x \n",PANEL_GetPixelClk60Hz());
       #endif
				 
		   vIO32WriteFldAlign(EH_02,H_HIGH_TH,EH_FAST_MUTE_H_TOTAL_HIGH_TH);
		   vIO32WriteFldAlign(EH_02,H_LOW_TH,EH_FAST_MUTE_H_TOTAL_LOW_TH);		  
		 
		   vIO32WriteFldAlign(EH_03,V_HIGH_TH,EH_FAST_MUTE_V_TOTAL_HIGH_TH);
		   vIO32WriteFldAlign(EH_03,V_LOW_TH,EH_FAST_MUTE_V_TOTAL_LOW_TH);				  
				 
		   vIO32WriteFldAlign(EH_04,DE_HIGH_TH,EH_FAST_MUTE_DE_TOTAL_HIGH_TH);
		   vIO32WriteFldAlign(EH_04,DE_LOW_TH,EH_FAST_MUTE_DE_TOTAL_LOW_TH);	   
		   
		   vIO32WriteFldAlign(EH_05,VDE_HIGH_TH,EH_FAST_MUTE_VDE_TOTAL_HIGH_TH);		
		   vIO32WriteFldAlign(EH_05,VDE_LOW_TH,EH_FAST_MUTE_VDE_TOTAL_LOW_TH);					   
	
		   vIO32WriteFldAlign(EH_06,EH_CLK_HIGH_TH,EH_FAST_MUTE_CLK_TOTAL_HIGH_TH);
		   vIO32WriteFldAlign(EH_06,EH_CLK_LOW_TH,EH_FAST_MUTE_CLK_TOTAL_LOW_TH);			
	
		   vIO32WriteFldAlign(EH_0B,(PANEL_GetPanelHeight()-1),EH_CALI_RANGE_H);						
				 
       #if EH_PANEL_FORMULATE         
		   //vIO32WriteFldAlign(EH_0C,PANEL_GetPanelWidth()+60,EH_DE_HS_START); 	  
		   //vIO32WriteFldAlign(EH_0D,PANEL_GetPanelHeight()+18,EH_DE_VS_START);		  
		   //vIO32WriteFldAlign(EH_0C,PANEL_GetPanelWidth()+60,EH_DE_HS_START); 	  
		   //vIO32WriteFldAlign(EH_0D,(V_LOW_TH-IO32ReadFldAlign(EH_0D,EH_DE_VS_WIDTH)),EH_DE_VS_START);						
		   vIO32WriteFldAlign(EH_0C, PANEL_GetHSyncWidth(),DE_HS_WIDTH);				
		   // [Note] minus 4 --> Dealy 4T in DE fix mode .
		   if (PANEL_GetHPosition60Hz()>=0x800)
		   {
		   vIO32WriteFldAlign(EH_0C,(PANEL_GetPanelWidth() -4 +(PANEL_GetHPosition60Hz() - 0x800)),EH_DE_HS_START); 	  
		   }
		   else
		   {
		   vIO32WriteFldAlign(EH_0C,(PANEL_GetPanelWidth() -4 -(0x800 - PANEL_GetHPosition60Hz())),EH_DE_HS_START); 	  
		   }
		   
		   vIO32WriteFldAlign(EH_0D, PANEL_GetVSyncWidth()-1,EH_DE_VS_WIDTH); 
		   // [Note] minus OS_75_VSYNC_DELSEL
		   // [Note] minus 2 --> Dealy 1 Line in DE fix mode and 1 Line in HSYNC & VSYNC align.
		   if (PANEL_GetVPosition()>=0x400)
		   {
		   vIO32WriteFldAlign(EH_0D,(PANEL_GetPanelHeight() + (wDrvGetOutputVTotal() - PANEL_GetPanelHeight() - 6) - 2 + (PANEL_GetVPosition() - 0x400)),EH_DE_VS_START);
		   }
		   else
		   {
		   vIO32WriteFldAlign(EH_0D,(PANEL_GetPanelHeight() + (wDrvGetOutputVTotal() - PANEL_GetPanelHeight() - 6) - 2 - (0x400 -PANEL_GetVPosition())),EH_DE_VS_START);
		   }
       #else
		   vErrorHandleSetByTiming();  
       #endif
		  
		   vIO32WriteFldAlign(EH_10,PANEL_GetPanelWidth(),EH_PTGEN_H_ACTIVE);			
		   vIO32WriteFldAlign(EH_10,wDrvGetOutputHTotal(),EH_PTGEN_H_TOTAL);				   
		 
		   vIO32WriteFldAlign(EH_12,PANEL_GetPanelHeight(),EH_PTGEN_V_ACTIVE);			 
		   vIO32WriteFldAlign(EH_12,wDrvGetOutputVTotal(),EH_PTGEN_V_TOTAL);	
		 
		   vIO32WriteFldAlign(EH_18,wDrvGetOutputVTotal(),EH_DE_VS_TOTAL_SW);			
		   vIO32WriteFldAlign(EH_18,wDrvGetOutputHTotal(),EH_DE_HS_TOTAL_SW); 
		 
		   // For Set Data Function
		   // vIO32WriteFldAlign(EH_19,PANEL_GetPanelWidth()+80,EH_FIX_HTOTAL_VALUE);			 
		   // vIO32WriteFldAlign(EH_19,PANEL_GetPanelWidth()+80,EH_FIX_HTOTAL_TH); 		
		   
			vIO32WriteFldAlign(EH_1A,PANEL_GetPanelWidth()-20,EH_FIX_VDE_TH);			
/*
		//for middle line of IPS2010 Panel	 
	#if HISENSE_IPS_2010_PANEL
			vIO32WriteFldAlign(EH_1A,PANEL_GetPanelWidth() + 2,EH_FIXED_DE_TOATL); 
	 #else
			vIO32WriteFldAlign(EH_1A,PANEL_GetPanelWidth(),EH_FIXED_DE_TOATL); 
	 #endif
*/
	 
	      //WXGA Panel , Error Handling FIXED_DE_TOATL+2=1368
		  if((PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS) && (PANEL_GetPanelWidth() == 1366))
		  {
		   vIO32WriteFldAlign(EH_01, 0, EH_USE_SC_TIMING_EN);
		   vIO32WriteFldAlign(EH_0E, 1, EH_FIXED_DE_EN);		   
		   vIO32WriteFldAlign(EH_1A,PANEL_GetPanelWidth() + 2,EH_FIXED_DE_TOATL); 
		  }
		  else
		  {		  
		   vIO32WriteFldAlign(EH_1A,PANEL_GetPanelWidth(),EH_FIXED_DE_TOATL); 
		  } 
#endif

Printf("---------- [SA7] vErrorHandleSetByPanel end----------\n"); 

}


void vErrorHandleSetByTiming(void)
{
    UINT16 u2HSyncWidth,u2VSyncWidth,u2HSyncStart,u2VSyncStart;
	
    Printf("---------- [SA7] vErrorHandleSetByTiming ----------\n"); 
    u2HSyncWidth = wDrvGetOutputHTotal() - wDrvGetOutputHActive() - wDrvGetHsyncBp() - wDrvGetHsyncFp();
	u2HSyncStart = wDrvGetOutputHActive() + wDrvGetHsyncFp();

	u2VSyncWidth = wDrvGetOutputVTotal() - wDrvGetOutputVActive() - wDrvGetVsyncBp() - wDrvGetVsyncFp();
	u2VSyncStart = wDrvGetOutputVActive() + wDrvGetVsyncFp();

	
//#ifdef NDEBUG
    Printf("[SA7] u2HSyncWidth=%d, u2HSyncStart=%d, u2VSyncWidth=%d, u2VSyncStart=%d\n", u2HSyncWidth, u2HSyncStart,u2VSyncWidth,u2VSyncStart);
//#endif

    vIO32WriteFldAlign(EH_0C, u2HSyncWidth ,EH_DE_HS_WIDTH);
    // [Note] minus 4 --> Dealy 4T in DE fix mode .
    // 2 ch minus 8, 1 ch minus 4
    vIO32WriteFldAlign(EH_0C, u2HSyncStart - 4 ,EH_DE_HS_START);
	if (u2VSyncWidth >= 2)
	{
     vIO32WriteFldAlign(EH_0D, u2VSyncWidth - 1 ,EH_DE_VS_WIDTH); 
	} 
	else
	{
 	 vIO32WriteFldAlign(EH_0D, 1 ,EH_DE_VS_WIDTH); 
	}
    // [Note] minus OS_75_VSYNC_DELSEL 
    // [Note] minus 2 --> Dealy 1 Line in DE fix mode and 1 Line in HSYNC & VSYNC align.
    // [Note] 6line=1, HV_BYPASS=0, HV_hold=1, VBack porch delay 1 line
    // Need jimmy check
    if((IO32ReadFldAlign(VINTF_B0,VINTF_HV_HOLD) == 1)
       &&(IO32ReadFldAlign(VINTF_B0, VINTF_HV_BYPASS) == 0)
       && (IO32ReadFldAlign(VINTF_B0, VINTF_LINE_BUFFER_BYPASS) == 0))
    {
     vIO32WriteFldAlign(EH_0D, u2VSyncStart - 2 ,EH_DE_VS_START);
    }
    else
    {
     vIO32WriteFldAlign(EH_0D, u2VSyncStart - 1 ,EH_DE_VS_START);
    }

     // For Set Data Function
	 vIO32WriteFldAlign(EH_19,wDrvGetOutputHActive()+80,EH_FIX_HTOTAL_VALUE);			 
	 vIO32WriteFldAlign(EH_19,wDrvGetOutputHActive()+80,EH_FIX_HTOTAL_TH); 	
	 Printf("---------- [SA7] vErrorHandleSetByTiming end----------\n"); 
}
#define ErrorHandling_DEMODE 0
void vErrorHandleInit(void)
{

    Printf("---------- [SA7] vErrorHandleInit ----------\n"); 
	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
	{
        vIO32WriteFldMulti(EH_00, P_Fld(1, EH_DISPR_DETECT_EN)|P_Fld(0, EH_INPUT_SEL)|P_Fld(1, EH_FRAME_METER_EN)|P_Fld(1, EH_INV_OUT_VS_EN)|P_Fld(1, EH_INV_OUT_HS_EN)|P_Fld(0, EH_DUAL_CHAN_PROCESS)|
                                  P_Fld(1, EH_H_CALI_WO_FILTER)|P_Fld(1, EH_H_CALI_REAL_TIME_CALI_EN)|P_Fld(0, EH_MUTE_MLVDS_EN)|P_Fld(0, EH_SET_DATA_TIME_SEL)|
                                  P_Fld(0, EH_XTAL_CLK_RST)|P_Fld(0, EH_OCLK_RST)|P_Fld(0, EH_REG_RST));
    }
    else
    {
        vIO32WriteFldMulti(EH_00, P_Fld(1, EH_DISPR_DETECT_EN)|P_Fld(0, EH_INPUT_SEL)|P_Fld(1, EH_FRAME_METER_EN)|P_Fld(1, EH_INV_OUT_VS_EN)|P_Fld(1, EH_INV_OUT_HS_EN)|P_Fld(0, EH_DUAL_CHAN_PROCESS)|
                                  P_Fld(1, EH_H_CALI_WO_FILTER)|P_Fld(1, EH_H_CALI_REAL_TIME_CALI_EN)|P_Fld(1, EH_MUTE_MLVDS_EN)|P_Fld(0, EH_SET_DATA_TIME_SEL)|
                                  P_Fld(0, EH_XTAL_CLK_RST)|P_Fld(0, EH_OCLK_RST)|P_Fld(0, EH_REG_RST));
    }
    vIO32WriteFldMulti(EH_01, P_Fld(1, EH_CLK_MISS_CHECK_EN)|P_Fld(0, EH_VDE_GEN_SEL)|P_Fld(0, EH_MUTE_DDDS_EN)|P_Fld(1, EH_DE_MISS_MUTE_EN)|
                              P_Fld(1, EH_USE_SC_TIMING_EN)|P_Fld(1, EH_INV_VS_EN)|P_Fld(1, IEH_NV_HS_EN)|P_Fld(1, EH_CLK_STABLE_CHECK_EN)|
                              P_Fld(1, EH_VDE_STABLE_CHECK_EN)|P_Fld(1, EH_DE_STABLE_CHECK_EN)|P_Fld(1, EH_V_STABLE_CHECK_EN)|P_Fld(1, EH_H_STABLE_CHECK_EN)|
                              P_Fld(1, EH_STABLE_CHECK_EN)|P_Fld(1, EH_CONFIG_RDY_WO_FM)|P_Fld(1, EH_CONFIG_RDY_WO_STABLE)|/*P_Fld(0,  EH_DISP_MLVDS_WO_DDDS_LOCK)|*/
                              P_Fld(0, EH_DISP_MLVDS_WO_OD_RDY)|P_Fld(0, EH_SW_MUTE)|P_Fld(0, EH_SW_MUTE_EN)|/*P_Fld(0, EH_SCALER_MUTE_LVDS_RX_EN)|*/
                              P_Fld(1, EH_H_MISS_MUTE_EN)|P_Fld(1, EH_CLK_MUTE_EN)|P_Fld(0, EH_VDE_MUTE_EN)|P_Fld(0, EH_DE_MUTE_EN)|
                              P_Fld(1, EH_V_MUTE_EN)|P_Fld(1, EH_H_MUTE_EN)|P_Fld(1, EH_FAST_MUTE_EN));

    vIO32WriteFldMulti(EH_07, P_Fld(0, EH_CLK_MUTE_TEST_PERIOD)|P_Fld(2, EH_CLK_MISS_PERIOD_SEL)|P_Fld(5, EH_FAST_MUTE_CLK_CNT_TH)|P_Fld(0, EH_FAST_MUTE_V_CNT_TH)|
                              P_Fld(2, EH_FAST_MUTE_H_CNT_TH));                
    vIO32WriteFldMulti(EH_08, P_Fld(2, EH_V_STB_CNT_TH)|P_Fld(2, EH_V_STB_DIFF_TH)|P_Fld(5, EH_H_STB_CNT_TH)|P_Fld(0x0A, EH_H_STB_DIFF_TH));
    vIO32WriteFldMulti(EH_09, P_Fld(2, EH_VDE_STB_CNT_TH)|P_Fld(2, EH_VDE_STB_DIFF_TH)|P_Fld(5, EH_DE_STB_CNT_TH)|P_Fld(0x0A, EH_DE_STB_DIFF_TH));
    vIO32WriteFldMulti(EH_0A, P_Fld(0x80, EH_CLK_STB_CNT_TH)|P_Fld(2, EH_CLK_STB_DIFF_TH));
    // MT8283 Fail --> EH_FIXED_VDE_EN
    #if ErrorHandling_DEMODE
    vIO32WriteFldMulti(EH_0E, P_Fld(1, EH_FIXED_DE_EN)|P_Fld(0, EH_FIXED_VDE_EN)|P_Fld(1, EH_SET_LOW_HTOTAL_EN)|P_Fld(0, EH_WXGA_DATA_DELAY_SEL)|
                              P_Fld(0, EH_CALI_MONI_SEL)|P_Fld(0, EH_DE_COUNTER_RESET_SEL)|P_Fld(0, EH_DE_DEGLITCH_EN)|P_Fld(0, EH_HTOTAL_CALI_MEDAIN_EN)|
                              P_Fld(0, EH_HTOTAL_CALI_LOW_PASS_EN)|P_Fld(1, EH_MUTE_MLVDS_OFF_WHEN_PTGEN)|P_Fld(0, EH_MUTE_MLVDS_PERIOD)|P_Fld(0, EH_EH_MONI_SEL));        
	#else
    vIO32WriteFldMulti(EH_0E, P_Fld(0, EH_FIXED_DE_EN)|P_Fld(0, EH_FIXED_VDE_EN)|P_Fld(1, EH_SET_LOW_HTOTAL_EN)|P_Fld(0, EH_WXGA_DATA_DELAY_SEL)|
                              P_Fld(0, EH_CALI_MONI_SEL)|P_Fld(0, EH_DE_COUNTER_RESET_SEL)|P_Fld(0, EH_DE_DEGLITCH_EN)|P_Fld(0, EH_HTOTAL_CALI_MEDAIN_EN)|
                              P_Fld(0, EH_HTOTAL_CALI_LOW_PASS_EN)|P_Fld(1, EH_MUTE_MLVDS_OFF_WHEN_PTGEN)|P_Fld(0, EH_MUTE_MLVDS_PERIOD)|P_Fld(0, EH_EH_MONI_SEL));        
	#endif
    vIO32WriteFldMulti(EH_11, P_Fld(0x18, EH_PTGEN_H_WIDTH)|P_Fld(0x32, EH_PTGEN_H_START));
    vIO32WriteFldMulti(EH_13, P_Fld(0x10, EH_PTGEN_V_WIDTH)|P_Fld(0x13, EH_PTGEN_V_START));
    vIO32WriteFldMulti(EH_14, P_Fld(0, EH_PTGEN_TYPE_EN)|P_Fld(0, EH_PTGEN_B)|P_Fld(0, EH_PTGEN_G)|P_Fld(0, EH_PTGEN_R));
    vIO32WriteFldMulti(EH_15, P_Fld(0xFF, EH_AUTO_PTGEN_TIME_TH)|P_Fld(0, EH_PTGEN_H_RUN)|P_Fld(0, EH_PTGEN_V_RUN)|P_Fld(0, EH_AUTO_CHG_PTGEN_EN)|
                              P_Fld(2, EH_PTGEN_TYPE));
    vIO32WriteFldMulti(EH_16, P_Fld(0, EH_INTR_CLR)|P_Fld(0xFF, EH_INTR_MASK));
    vIO32WriteFldAlign(EH_17, 0x70, EH_FAST_MUTE_DE_MISS_TH);
    vIO32WriteFldAlign(EH_STATUS_05,0,EH_LVDS_RX_INTR);

    if (LVDS_OUTPUT_PORT == SINGLE_PORT) // WXGA/60
    {
      vIO32WriteFldMulti(EH_02, P_Fld(0xAAC, EH_FAST_MUTE_H_TOTAL_HIGH_TH)|P_Fld(0x578, EH_FAST_MUTE_H_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_03, P_Fld(0x600, EH_FAST_MUTE_V_TOTAL_HIGH_TH)|P_Fld(0x310, EH_FAST_MUTE_V_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_04, P_Fld(0x560, EH_FAST_MUTE_DE_TOTAL_HIGH_TH)|P_Fld(0x551, EH_FAST_MUTE_DE_TOTAL_HIGH_TH));
      vIO32WriteFldMulti(EH_05, P_Fld(0x310, EH_FAST_MUTE_VDE_TOTAL_HIGH_TH)|P_Fld(0x2F0, EH_FAST_MUTE_VDE_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_06, P_Fld(0x228, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH)|P_Fld(0x113, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH));            
      vIO32WriteFldMulti(EH_0B, P_Fld(1, EH_CALI_RANGE_SEL)|P_Fld(0, EH_CALI_WO_STABLE)|P_Fld(0x2, EH_CALI_RANGE_L)|P_Fld(0x2FF, EH_CALI_RANGE_H));
      #if ErrorHandling_DEMODE	  
      vIO32WriteFldMulti(EH_0C, P_Fld(1, EH_DE_MODE_EN)|P_Fld(0x10, EH_DE_HS_WIDTH)|P_Fld(0x568, EH_DE_HS_START));
	  #else
	  vIO32WriteFldMulti(EH_0C, P_Fld(0, EH_DE_MODE_EN)|P_Fld(0x10, EH_DE_HS_WIDTH)|P_Fld(0x568, EH_DE_HS_START));
	  #endif
      vIO32WriteFldMulti(EH_0D, P_Fld(1, EH_DE_VS_WIDTH)|P_Fld(0x310, EH_DE_VS_START));
      vIO32WriteFldMulti(EH_10, P_Fld(0x558, EH_PTGEN_H_ACTIVE)|P_Fld(0x647, EH_PTGEN_H_TOTAL));
      vIO32WriteFldMulti(EH_12, P_Fld(0x300,EH_PTGEN_V_ACTIVE)|P_Fld(0x329, EH_PTGEN_V_TOTAL));  
      vIO32WriteFldMulti(EH_18, P_Fld(0, EH_CALI_HS_USE_SW_EN)|P_Fld(0, EH_CALI_VS_USE_SW_EN)|P_Fld(0, EH_CALI_DE_USE_SW_EN)|P_Fld(0, EH_CALI_VDE_USE_SW_EN)|
                                P_Fld(0x329, EH_DE_VS_TOTAL_SW)|P_Fld(0x647, EH_DE_HS_TOTAL_SW));     
      vIO32WriteFldMulti(EH_19, P_Fld(0x5A6, EH_FIX_HTOTAL_VALUE)|P_Fld(0x5A7, EH_FIX_HTOTAL_TH));
      vIO32WriteFldMulti(EH_1A, P_Fld(0x540, EH_FIX_VDE_TH)|P_Fld(0x558, EH_FIXED_DE_TOATL));    
    }
    else  if ((LVDS_OUTPUT_PORT == DUAL_PORT)||(LVDS_OUTPUT_PORT == FOUR_PORT)) // 1080P/60
    {
      vIO32WriteFldMulti(EH_02, P_Fld(0xFFF, EH_FAST_MUTE_H_TOTAL_HIGH_TH)|P_Fld(0x7D0, EH_FAST_MUTE_H_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_03, P_Fld(0xFFF, EH_FAST_MUTE_V_TOTAL_HIGH_TH)|P_Fld(0x456, EH_FAST_MUTE_V_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_04, P_Fld(0x788, EH_FAST_MUTE_DE_TOTAL_HIGH_TH)|P_Fld(0x779, EH_FAST_MUTE_DE_TOTAL_HIGH_TH));
      vIO32WriteFldMulti(EH_05, P_Fld(0x442, EH_FAST_MUTE_VDE_TOTAL_HIGH_TH)|P_Fld(0x42E, EH_FAST_MUTE_VDE_TOTAL_LOW_TH));
      vIO32WriteFldMulti(EH_06, P_Fld(0x114, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH)|P_Fld(0x99, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH));        
      vIO32WriteFldMulti(EH_0B, P_Fld(1, EH_CALI_RANGE_SEL)|P_Fld(0, EH_CALI_WO_STABLE)|P_Fld(0x2, EH_CALI_RANGE_L)|P_Fld(0x438, EH_CALI_RANGE_H));
	  #if ErrorHandling_DEMODE
      vIO32WriteFldMulti(EH_0C, P_Fld(1, EH_DE_MODE_EN)|P_Fld(0x10, EH_DE_HS_WIDTH)|P_Fld(0x7C4, EH_DE_HS_START));
	  #else
      vIO32WriteFldMulti(EH_0C, P_Fld(0, EH_DE_MODE_EN)|P_Fld(0x10, EH_DE_HS_WIDTH)|P_Fld(0x7C4, EH_DE_HS_START));
	  #endif
      vIO32WriteFldMulti(EH_0D, P_Fld(1, EH_DE_VS_WIDTH)|P_Fld(0x44A, EH_DE_VS_START));
      vIO32WriteFldMulti(EH_10, P_Fld(0x780, EH_PTGEN_H_ACTIVE)|P_Fld(0x898, EH_PTGEN_H_TOTAL));
      vIO32WriteFldMulti(EH_12, P_Fld(0x348,EH_PTGEN_V_ACTIVE)|P_Fld(0x464, EH_PTGEN_V_TOTAL));
      vIO32WriteFldMulti(EH_18, P_Fld(0, EH_CALI_HS_USE_SW_EN)|P_Fld(0, EH_CALI_VS_USE_SW_EN)|P_Fld(0, EH_CALI_DE_USE_SW_EN)|P_Fld(0, EH_CALI_VDE_USE_SW_EN)|
                                P_Fld(0x465, EH_DE_VS_TOTAL_SW)|P_Fld(0x898, EH_DE_HS_TOTAL_SW));     
      vIO32WriteFldMulti(EH_19, P_Fld(0x7D0, EH_FIX_HTOTAL_VALUE)|P_Fld(0x7D0, EH_FIX_HTOTAL_TH));
      vIO32WriteFldMulti(EH_1A, P_Fld(0x76C, EH_FIX_VDE_TH)|P_Fld(0x780, EH_FIXED_DE_TOATL));
    }

    if (LVDS_OUTPUT_PORT == FOUR_PORT)
    {
     vIO32WriteFldMulti(EH_06, P_Fld(0x8A, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH)|P_Fld(0x4C, EH_FAST_MUTE_CLK_TOTAL_HIGH_TH));        
    }

    if(IS_LBRB_3D_ON) // sync mode , disp_r no regen on Error Handling
    {
     vIO32WriteFldAlign(EH_0C, 0 ,EH_DE_MODE_EN);     
    }
  vErrorHandleSetByPanel();
}

void vErrorHandleStatus(void)
{
	UINT32 u2MUTE_STATUS; 
	UINT32 u2Temp;
	UINT32 u2Temp_num,u2Temp_point;
	
	u2MUTE_STATUS = IO32ReadFldAlign(EH_STATUS_02,EH_FLD_ALL);
	
	Printf("---------- Error Handling Status ----------\n"); 
	
	Printf("[Panel Interface Information]\n");
	if (IO32ReadFldAlign(VINTF_P0,VINTF_PANEL_IN_SEL)==0)
	Printf("(0)From TCON-Error handling\n");
	else if (IO32ReadFldAlign(VINTF_P0,VINTF_PANEL_IN_SEL)==1)
	Printf("(1)From OD\n");
	else if (IO32ReadFldAlign(VINTF_P0,VINTF_PANEL_IN_SEL)==2)
	Printf("(2)From From SCPOS\n");

	Printf("[Error handling input]\n");    
	if (IO32ReadFldAlign(EH_00, EH_INPUT_SEL) == 1)
	Printf("[0] From OD (Test)\n");
	else if (IO32ReadFldAlign(EH_00, EH_INPUT_SEL) == 0)
	Printf("[1] From SCPOS (default) \n");
	
	Printf("[Clock Information]\n");
	if (IS_XTAL_24MHZ())
	{
		Printf("(Input)Clock=%d MHz\n",(((1024*24)/(IO32ReadFldAlign(EH_STATUS_04,EH_CLK_TOTAL_MON)))));
	}
	else if (IS_XTAL_27MHZ())
	{
		Printf("(Input)Clock=%d MHz\n",(((1024*27)/(IO32ReadFldAlign(EH_STATUS_04,EH_CLK_TOTAL_MON)))));
	}

	Printf("[Frame Rate]\n"); 
	u2Temp = IO32ReadFldAlign(EH_STATUS_0D,EH_XCLK_IN_FRAME);	
	if (IS_XTAL_24MHZ())
	{
		u2Temp_num = 24000000/u2Temp;
	  	u2Temp_point = (24000000 - u2Temp_num * u2Temp)*100/u2Temp;
		Printf("(Input) frame rate = %d.%2d Hz\n", u2Temp_num, u2Temp_point);
	}
	else
	{
		u2Temp_num = 27000000/u2Temp;
	  	u2Temp_point = (27000000 - u2Temp_num * u2Temp)*100/u2Temp;	
		Printf("(Input)frame rate = %d.%2d Hz\n", u2Temp_num, u2Temp_point);

	}
	
	Printf("[DISP_R status]\n"); 
	if(IO32ReadFldAlign(EH_STATUS_0D,EH_DISP_R_STATUS)==0)	
	{
		Printf("LRLR Mode\n");
	}
	else if(IO32ReadFldAlign(EH_STATUS_0D,EH_DISP_R_STATUS)==1)	
	{
		Printf("LLRR Mode\n");
	}
	else if(IO32ReadFldAlign(EH_STATUS_0D,EH_DISP_R_STATUS)==2)	
	{
		Printf("MUTE Mode\n");
	}
	else if(IO32ReadFldAlign(EH_STATUS_0D,EH_DISP_R_STATUS)==3)	
	{
		Printf("IDLE Mode\n");
	}
		
	Printf("[V,F Information]\n");
	Printf("(Input)DE_TOTAL_MON=%d, H_TOTAL_MON=%d, VDE_TOTAL_MON=%d, V_TOTAL_MON=%d\n",
			(IO32ReadFldAlign(EH_STATUS_00,EH_DE_TOTAL_MON)),((IO32ReadFldAlign(EH_STATUS_00,EH_H_TOTAL_MON)) + 1),
			IO32ReadFldAlign(EH_STATUS_01,EH_VDE_TOTAL_MON), (IO32ReadFldAlign(EH_STATUS_01,EH_V_TOTAL_MON)+1));		  
    if(IO32ReadFldAlign(EH_01,EH_USE_SC_TIMING_EN)==1)	
    {
		Printf("Ouput Status from Scaler \n");	
    }
	else
	{
		Printf("Ouput Status from Error Handling \n");	
	}
	u2Temp = IO32ReadFldAlign(EH_01,EH_USE_SC_TIMING_EN);
	vIO32WriteFldAlign(EH_01,1,EH_USE_SC_TIMING_EN);
	Printf("(Ouput#1 from scaler )DE_TOTAL_MON=%d, H_TOTAL_MON=%d, VDE_TOTAL_MON=%d, V_TOTAL_MON=%d\n",
			IO32ReadFldAlign(EH_STATUS_08,EH_DE_TOTAL_CALI),IO32ReadFldAlign(EH_STATUS_08,EH_H_TOTAL_CALI),
			IO32ReadFldAlign(EH_STATUS_09,EH_VDE_TOTAL_CALI), IO32ReadFldAlign(EH_STATUS_09,EH_V_TOTAL_CALI));
	vIO32WriteFldAlign(EH_01,0,EH_USE_SC_TIMING_EN);
	Printf("(Ouput#2 EH calculate)DE_TOTAL_MON=%d, H_TOTAL_MON=%d, VDE_TOTAL_MON=%d, V_TOTAL_MON=%d\n",
			IO32ReadFldAlign(EH_STATUS_08,EH_DE_TOTAL_CALI),IO32ReadFldAlign(EH_STATUS_08,EH_H_TOTAL_CALI),
			IO32ReadFldAlign(EH_STATUS_09,EH_VDE_TOTAL_CALI), IO32ReadFldAlign(EH_STATUS_09,EH_V_TOTAL_CALI));
	vIO32WriteFldAlign(EH_01,u2Temp,EH_USE_SC_TIMING_EN);
	Printf("(Ouput)H_WIDTH=%d, H_START=%d, V_WIDTH=%d, V_START=%d\n",
			IO32ReadFldAlign(EH_STATUS_0A,EH_H_WIDTH_CALI),IO32ReadFldAlign(EH_STATUS_0B,EH_H_START_CALI),
			IO32ReadFldAlign(EH_STATUS_0A,EH_V_WIDTH_CALI), IO32ReadFldAlign(EH_STATUS_0B,EH_V_START_CALI));	
	
	Printf("[UNREADY Status]\n"); 
	// if (((u2MUTE_STATUS >> EHS_OD_READY)& 0x01) == 0)
	// Printf("| OD_UNREADY");
	if (((u2MUTE_STATUS >> EHS_OD_OK)& 0x01) == 0)
	Printf("| OD_UNOK");
	if (((u2MUTE_STATUS >> EHS_DISP_OK)& 0x01) == 0)
	Printf("| DISP_UNOK");
	if (((u2MUTE_STATUS >> EHS_FAST_MUTE_READY)& 0x01) == 0)
	Printf("FAST_MUTE_UNREADY");
	Printf("\n");
	
	Printf("[MUTE Condition]\n");
	Printf("HOTAL[High = %4d,Low = %4d]\n",
			IO32ReadFldAlign(EH_02,EH_FAST_MUTE_H_TOTAL_HIGH_TH),
			IO32ReadFldAlign(EH_02,EH_FAST_MUTE_H_TOTAL_LOW_TH));
	Printf("VOTAL[High = %4d,Low = %4d]\n",
			IO32ReadFldAlign(EH_03,EH_FAST_MUTE_V_TOTAL_HIGH_TH),
			IO32ReadFldAlign(EH_03,EH_FAST_MUTE_V_TOTAL_LOW_TH));
	Printf("DE   [High = %4d,Low = %4d]\n",
			IO32ReadFldAlign(EH_04,EH_FAST_MUTE_DE_TOTAL_HIGH_TH),
			IO32ReadFldAlign(EH_04,EH_FAST_MUTE_DE_TOTAL_HIGH_TH)); 		 
	Printf("VDE  [High = %4d,Low = %4d]\n",
			IO32ReadFldAlign(EH_05,EH_FAST_MUTE_VDE_TOTAL_HIGH_TH),
			IO32ReadFldAlign(EH_05,EH_FAST_MUTE_VDE_TOTAL_LOW_TH)); 		 
	if (IS_XTAL_24MHZ())
	{
		Printf("Clock[High = %d MHz,Low = %d MHz]\n",
			(((1024*24)/(IO32ReadFldAlign(EH_06,EH_FAST_MUTE_CLK_TOTAL_LOW_TH)))),
			(((1024*24)/(IO32ReadFldAlign(EH_06,EH_FAST_MUTE_CLK_TOTAL_HIGH_TH)))));
		}
	else if (IS_XTAL_27MHZ())
	{
		Printf("Clock[High = %d MHz,Low = %d MHz]\n",
			(((1024*27)/(IO32ReadFldAlign(EH_06,EH_FAST_MUTE_CLK_TOTAL_LOW_TH)))),
			(((1024*27)/(IO32ReadFldAlign(EH_06,EH_FAST_MUTE_CLK_TOTAL_HIGH_TH)))));
	}
	
	Printf("[Mute Status]\n");	  
	if (((u2MUTE_STATUS >> EHS_OD_INTF_ERROR)& 0x01) == 1)
	Printf("OD_INTF_ERROR");
	if (((u2MUTE_STATUS >> EHS_MUTE_LVDS)& 0x01) == 1)
	Printf("| MUTE_MLVDS");
	if (((u2MUTE_STATUS >> EHS_DE_MISS_MUTE)& 0x01) == 1)
	Printf("| DE_MISS_MUTE");
	if (((u2MUTE_STATUS >> EHS_INPUT_OK)& 0x01) == 1)
	Printf("| INPUT_OK");
	// if (((u2MUTE_STATUS >> EHS_OD_BYPASS)& 0x01) == 1)
	// Printf("| OD_BYPASS");
	if (((u2MUTE_STATUS >> EHS_OD_MUTE_DDDS)& 0x01) == 1)
	Printf("| OD_MUTE_DDDS");
	Printf("\n");	 
	
	if (((u2MUTE_STATUS >> EHS_H_MISS_MUTE)& 0x01) == 1)
	Printf("| H_MISS_MUTE");
	if (((u2MUTE_STATUS >> EHS_CLOCK_MUTE)& 0x01) == 1)
	Printf("| CLOCK MUTE");
	if (((u2MUTE_STATUS >> EHS_VDE_MUTE)& 0x01) == 1)
	Printf("| VDE_MUTE");
	if (((u2MUTE_STATUS >> EHS_V_MUTE)& 0x01) == 1)
	Printf("| V_MUTE");
	if (((u2MUTE_STATUS >> EHS_DE_MUTE)& 0x01) == 1)
	Printf("| DE_MUTE");
	if (((u2MUTE_STATUS >> EHS_H_MUTE)& 0x01) == 1)
	Printf("| H_MUTE");
	if (((u2MUTE_STATUS >> EHS_MUTE)& 0x01) == 1)
	Printf("| MUTE");  
	Printf("\n");	 
	
	Printf("[UNSTABLE Status]\n");	  
	
	if (((u2MUTE_STATUS >> EHS_CONFIG_READY)& 0x01) == 0)
	Printf("Configuration_UNREADY");
	//if (((u2MUTE_STATUS >> EHS_DDDS_LOCK)& 0x01) == 0)
	//Printf("| DDDS_UNLOCK");
	if (((u2MUTE_STATUS >> EHS_CLOCK_STABLE)& 0x01) == 0)
	Printf("| CLOCK_UNSTABLE");
	if (((u2MUTE_STATUS >> EHS_VDE_STABLE)& 0x01) == 0)
	Printf("| VDE_UNSTABLE");
	if (((u2MUTE_STATUS >> EHS_V_STABLE)& 0x01) == 0)
	Printf("| V_UNSTABLE");
	if (((u2MUTE_STATUS >> EHS_DE_STABLE)& 0x01) == 0)
	Printf("| DE_UNSTABLE");
	if (((u2MUTE_STATUS >> EHS_H_STABLE)& 0x01) == 0)
	Printf("| H_UNSTABLE");
	if (((u2MUTE_STATUS >> EHS_STABLE)& 0x01) == 0)
	Printf("| UNSTABLE");  
	Printf("\n");	 
			

}

void u1ErrorHandleFixEnable(UINT8 val)
{
  vIO32WriteFldMulti(EH_0E, P_Fld(val, EH_FIXED_DE_EN)
                                       |P_Fld(val, EH_FIXED_VDE_EN)|P_Fld(val, EH_DE_DEGLITCH_EN));

}

void u1ErrorHandleMuteEnable(UINT8 val)
{
   vIO32WriteFldAlign(EH_00, val, EH_MUTE_MLVDS_EN);   
}


void u1ErrorHandlingSourceSet(UINT8 val)
{
  if (val<=1)
  {
   vIO32WriteFldAlign(EH_00, val, EH_INPUT_SEL);   
  } 
}

UINT8 u1ErrorHandlingSourceGet(void)
{
  return (IO32ReadFldAlign(EH_00, EH_INPUT_SEL));
}

void vMiniLVDS_AnalogInit(void)
{
    UINT32 u4Value;

	Printf("[LVDS] vMiniLVDS_AnalogInit \n");	
	
    #ifdef CC_MTK_LOADER
	vDrvLVDS7To4FifoEnable();
    #endif
        
    // Set Driving Current (TVO)
    u4Value = PANEL_GetDrivingCurrent();
    if (u4Value <= 15)
    {
      vDrvLVDSSetDriving(u4Value);
    }
    else
    {
      vDrvLVDSSetDriving(4); //4mA	
    }

#ifdef CC_MT5881
   //Set RG_LVDS_B_EN to Keep Low
   vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11 );
   vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1, RG_LVDSB_BIAS_EN_D11 );
   // Set (TVCM)
   vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0x0A, RG_LVDSB_TVCM_D11 );
   //Set PREEM_EN
   vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0, RG_LVDSB_PREEM_EN_EVEN_D11 );
   vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0, RG_LVDSB_PREEM_EN_ODD_D11 );
   //Set Bias
   vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0, RG_LVDSB_ENABIST_E_ODD_D11); // for mini LVDS BIAS 
   vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0, RG_LVDSB_ENABIST_E_EVEN_D11); // for mini LVDS BIAS
   //Set CLK_IQ SEL
   vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0x1, RG_LVDSB_CLK_IQ_SEL_EVEN_D11);  //set for output CLK 
   vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0x2, RG_LVDSB_CLK_IQ_SEL_ODD_D11);

#endif
#ifdef CC_MT5399

	 //Set RG_LVDSA/B_EN to Keep Low
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11 );
	 #ifdef CC_MT5399
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0,RG_LVDSA_EN); 
	 #endif

	 // Set VCM (TVCM)
	 #ifdef CC_MT5399
	 vIO32WriteFldMulti(REG_LVDS_TX_CFG9, P_Fld(0x0A, RG_LVDSA_TVCM)|P_Fld(1, RG_LVDSA_NSRC)|P_Fld(1, RG_LVDSA_PSRC));
	 #endif
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0x0A, RG_LVDSB_TVCM_D11 );
	
	 //Set Bias Voltage (BIAS_SEL)
	 #ifdef CC_MT5399
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 4, RG_LVDSA_BIAS_SEL); // for mini LVDS BIAS
	 //vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 0x0C, RG_LVDSA_BIAS_SEL); // for EPI BIAS
	#endif
	// vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 1, RG_LVDSB_BIAS_SEL);
	
	 //Set CLK_IQ SEL
	 #ifdef CC_MT5399
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0x008, RG_LVDSA_CLK_IQ_SEL);
	 //vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0, RG_LVDSA_CLK_IQ_SEL);  // for EPI
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0xFFF, RG_LVDSA_LVDS_SEL);
	 vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0x008, RG_LVDSB_CLK_IQ_SEL);
	 #else
          #ifdef need_to_check
	 vIO32WriteFldAlign(RE_LVDS_TX_CFG2, 0x2040, RG_LVDSB_CLK_IQ_SEL);
          #endif
	 #endif
	 
	 // vIO32WriteFldMulti(LVDSTX_ANA4, P_Fld(3, LVDSB4_TVCM)|P_Fld(3, LVDSB4_NSRC)|P_Fld(4, LVDSB4_PSRC)|P_Fld(0x0, LVDSB4_PD));	
	
	 //  vIO32WriteFldMulti(LVDSTX_ANA3, P_Fld(1, LVDSA3_TERM_EN)|P_Fld(1, LVDSA3_MINI_SEL_CK2)|P_Fld(1, LVDSA3_MINI_SEL_CK1)|P_Fld(1, LVDSA3_MINI_SEL));
	 //  vIO32WriteFldMulti(LVDSTX_ANA7, P_Fld(1, LVDSB7_TERM_EN)|P_Fld(1, LVDSB7_MINI_SEL_CK2)|P_Fld(1, LVDSB7_MINI_SEL_CK1)|P_Fld(1, LVDSB7_MINI_SEL));
		 
	 //  vIO32WriteFldMulti(LVDSTX_ANA1, P_Fld(1, LVDSA1_CLK_SEL_CK2)|P_Fld(1, LVDSA1_CLK_SEL_CK1)|P_Fld(0, LVDSA1_CLK_SEL));
	 //  vIO32WriteFldMulti(LVDSTX_ANA5, P_Fld(1, LVDSB5_CLK_SEL_CK2)|P_Fld(1, LVDSB5_CLK_SEL_CK1)|P_Fld(0, LVDSB5_CLK_SEL));
				   
	
	 // set top pinmux to LDVS out
	 
	 // Enable LVDS Pad
	//mt5396 no need
	// vLVDSSetPadMuxEnable();
#endif	

} 

extern UINT8 _Lvds7To4FifoReset;

void vEPI_AnalogInit(void)
{
    UINT32 u4Value;
    // set TVO(driving current) if necessary
    u4Value = PANEL_GetDrivingCurrent();
    if (u4Value <= 15)
    {
      vDrvLVDSSetDriving(u4Value);
    }
    else
    {
      vDrvLVDSSetDriving(5);
    }
#ifdef CC_MT5881	
	//B group bias current power on
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11 );
	// Set (TVCM)
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0x05, RG_LVDSB_TVCM_D11 );
	//Set PREEM_EN
	vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0, RG_LVDSB_PREEM_EN_EVEN_D11 );
	vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0, RG_LVDSB_PREEM_EN_ODD_D11 );
	//Set Bias
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0, RG_LVDSB_ENABIST_E_ODD_D11); // for mini LVDS BIAS 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0, RG_LVDSB_ENABIST_E_EVEN_D11); // for mini LVDS BIAS
	//Set CLK_IQ SEL
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0x0, RG_LVDSB_CLK_IQ_SEL_EVEN_D11);  //set for output CLK 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0x0, RG_LVDSB_CLK_IQ_SEL_ODD_D11);
#endif	

}

void vEPI_AnalogOn(void)
{

#ifdef CC_MT5399
    //power down B group
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_EN_D11 );
	//B group main driver power down
	vIO32WriteFldMulti(REG_LVDS_TX_CFG1, P_Fld(0, RG_LVDSB_DRV_EN_EVEN_D11)|P_Fld(0,RG_LVDSB_DRV_EN_ODD_D11));  
	//B group pre driver power down
        #ifdef need_to_check
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0xfff, RG_LVDSB_PRE_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_EXT_EN_D11);
	//B group clk regulator power down
	vIO32WriteFldAlign(REG_LVDS_TX_CFG8, 1, RG_LVDSB_LDO_PWD_D11);
        #endif
    //B group bias current power down
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1, RG_LVDSB_BIAS_EN_D11 );

    //A group ch1~6 AE3 AE2 AE0 AO4 AO3 AOCK
    //A group bias current power on
	#ifdef CC_MT5399
	vIO32WriteFldMulti(REG_LVDS_TX_CFG9, P_Fld(0, RG_LVDSA_BIAS_ODD_PWD)|P_Fld(0, RG_LVDSA_BIAS_EVEN_PWD));
	//A group clk regulator power on
    vIO32WriteFldMulti(REG_LVDS_TX_CFG17, P_Fld(0, RG_LVDSA_LDO_ODD_PWD_D11)|P_Fld(0, RG_LVDSA_LDO_BIAS_ODD_PWD_D11)|
                                          P_Fld(0, RG_LVDSA_LDO_EVEN_PWD_D11)|P_Fld(0, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11));

    if(fgIsSupport120HzPanel())
    {
        vIO32WriteFldMulti(REG_LVDS_TX_CFG9, P_Fld(0xA, RG_LVDSA_TVO)|P_Fld(4, RG_LVDSA_TVCM)|P_Fld(4, RG_LVDSA_NSRC)|
                                             P_Fld(4, RG_LVDSA_PSRC));
        vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 1, RG_LVDSA_TVO_PRE);
        vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0xc87, RG_LVDSA_PRE_PWD);
    }
    else
    {
        vIO32WriteFldMulti(REG_LVDS_TX_CFG9, P_Fld(0x8, RG_LVDSA_TVO)|P_Fld(0x0B, RG_LVDSA_TVCM)|P_Fld(4, RG_LVDSA_NSRC)|
                                         P_Fld(4, RG_LVDSA_PSRC));
        vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 0, RG_LVDSA_TVO_PRE);
        vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0xFFF, RG_LVDSA_PRE_PWD);
    }
    vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0x378, RG_LVDSA_EXT_EN_D11);
    vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 0xc87, RG_LVDSA_DRV_PWD);
//    vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0xc87, RG_LVDSA_LVDS_SEL);

    //power on A group
	vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0x0, RG_LVDSA_EN);
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
    vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0x1, RG_LVDSA_EN);
  #endif
#endif 



#ifdef CC_MT5881	 
	
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1, RG_LVDSB_BIAS_EN_D11 );
	 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0x7f,RG_LVDSB_DRV_EN_EVEN_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0x7E,RG_LVDSB_DRV_EN_ODD_D11);
	
	
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);	
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1,RG_LVDSB_EN_D11);
	
#endif



#ifdef CC_MTK_LOADER
   vDrvLVDS7To4FifoEnable();
#else
   _Lvds7To4FifoReset = 2;
#endif
}
void vEPI_AnalogOff(void)
{
	Printf("[LVDS] vEPIAnalogOff \n");

#ifdef CC_MT5399
    //power down B group
	vIO32WriteFldMulti(REG_LVDS_TX_CFG0, P_Fld(0, RG_LVDSB_EN_D11 )|P_Fld(1, RG_LVDSB_BIAS_EN_D11 ));
	//B group main driver power down
	vIO32WriteFldMulti(REG_LVDS_TX_CFG1, P_Fld(0, RG_LVDSB_DRV_EN_EVEN_D11)|P_Fld(0,RG_LVDSB_DRV_EN_ODD_D11));  
	//B group pre driver power down
	vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0xfff, RG_LVDSB_PRE_PWD);
	//B group clk regulator power down
	vIO32WriteFldAlign(REG_LVDS_TX_CFG8, 1, RG_LVDSB_LDO_PWD_D11);

    //A group ch1~6 AE3 AE2 AE0 AO4 AO3 AOCK
    //power down A group
    vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0, RG_LVDSA_EN);
	//A group main driver power down
    vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 0xfff, RG_LVDSA_DRV_PWD);
    vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0, RG_LVDSA_EXT_EN_D11);
	//A group pre driver power down
	vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0xfff, RG_LVDSA_PRE_PWD);

	//A group clk regulator power off
    vIO32WriteFldMulti(REG_LVDS_TX_CFG17, P_Fld(1, RG_LVDSA_LDO_ODD_PWD_D11)|P_Fld(1, RG_LVDSA_LDO_BIAS_ODD_PWD_D11)|
                                          P_Fld(1, RG_LVDSA_LDO_EVEN_PWD_D11)|P_Fld(1, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11));
    //A group bias current power down
    vIO32WriteFldMulti(REG_LVDS_TX_CFG9, P_Fld(1, RG_LVDSA_BIAS_ODD_PWD)|P_Fld(1, RG_LVDSA_BIAS_EVEN_PWD));


#else	

/*
          #ifdef need_to_check
    //power down B group
    vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_EN_D11);
    //B group main driver power down
	vIO32WriteFldMulti(REG_LVDS_TX_CFG1, P_Fld(0, RG_LVDSB_DRV_EN_EVEN_D11)|P_Fld(0,RG_LVDSB_DRV_EN_ODD_D11));  
    //B group pre driver power down
    vIO32WriteFldAlign(REG_LVDS_TX_CFG2, 0xfff, RG_LVDSB_PRE_PWD);
    //vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_EXT_EN_D11);
    //B group clk regulator power down
 //   vIO32WriteFldAlign(REG_LVDS_TX_CFG8, 1, RG_LVDSB_LDO_PWD_D11);
    //B group bias current power down
    vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1, RG_LVDSB_BIAS_EN_D11 ));
   #endif
*/
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);
  	
    vIO32WriteFldMulti(REG_LVDS_TX_CFG1, P_Fld(0, RG_LVDSB_DRV_EN_EVEN_D11)|P_Fld(0,RG_LVDSB_DRV_EN_ODD_D11));  
    vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_BIAS_EN_D11 );
	
    vIO32WriteFldAlign(REG_VOPLL_CFG3, 1, RG_PPLL_PWD);
    vIO32WriteFldAlign(REG_VOPLL_CFG0, 1, RG_VPLL_PWD);
	vIO32WriteFldAlign(REG_VOPLL_CFG0, 1, RG_VPLL_BG_PWD);	


#endif
}

/**
 * @brief vDrvErrorHandleEN

 * Error handle enable control

 * @param  bEn
 * @retval void
 */
/*
static void vDrvErrorHandleEN(UINT8 bEn) 
{
    vIO32WriteFldAlign(EH_00,bEn,EH_MUTE_MLVDS_EN);
    vIO32WriteFldMulti(EH_01,P_Fld(bEn,EH_FAST_MUTE_EN)|P_Fld(bEn,EH_DE_MISS_MUTE_EN));

}
*/
static void vDrvTconGammaInit(void)
{   
	#if  HISENSE_IPS_2010_PANEL 
	   x_thread_delay(15);
	#endif

    if (u1TconCustGammaInit() == 1)
    {
        _u1GammaReady = 1;
    }
    else
    {
        _u1GammaReady = 0;
    }
    
	LOG(1,"Tcon Gamma init\n");
}

static void vDrvMiniLvdsOn(void)
{

	Printf("[LVDS] vDrvMiniLvdsOn \n");	
	//_Lvds7To4FifoReset = 2;

    //power up bandgap and all reference circuit
    // vIO32WriteFldAlign(LVDSTX_ANA4, 0, LVDSB4_BIAS_PD);    
    //A Group Power on Sequence
#ifdef CC_MT5881

vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0x7f,RG_LVDSB_DRV_EN_EVEN_D11);
vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0x7E,RG_LVDSB_DRV_EN_ODD_D11);


vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);	
HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1,RG_LVDSB_EN_D11);

#endif
	
#ifdef CC_MT5399    
    #ifdef CC_MT5399
    vIO32WriteFldMulti(REG_LVDS_TX_CFG9,P_Fld( 0, RG_LVDSA_BIAS_ODD_PWD)|P_Fld( 0, RG_LVDSA_BIAS_EVEN_PWD));
    vIO32WriteFldMulti(REG_LVDS_TX_CFG17,P_Fld( 0, RG_LVDSA_LDO_BIAS_ODD_PWD_D11)|P_Fld( 0, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11));
    vIO32WriteFldMulti(REG_LVDS_TX_CFG17, P_Fld( 0, RG_LVDSA_LDO_ODD_PWD_D11)|P_Fld( 0, RG_LVDSA_LDO_EVEN_PWD_D11));
    vIO32WriteFldAlign(REG_LVDS_TX_CFG10, 0, RG_LVDSA_DRV_PWD);    
    vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0xFFF, RG_LVDSA_EXT_EN_D11);    
    #endif
    #ifdef need_to_check
    //B Group Power on Sequence
      vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_BIAS_EN_D11 );
    vIO32WriteFldAlign(REG_LVDS_TX_CFG8, 0, RG_LVDSB_LDO_PWD_D11);
    vIO32WriteFldAlign(REG_LVDS_TX_CFG1, 0, RG_LVDSB_DRV_PWD);    
     #endif
    #ifdef CC_MT5399
    vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0XFFF, RG_LVDSB_EXT_EN_D11);    
    #else
    //vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0X3FFF, RG_LVDSB_EXT_EN_D11);    
    #endif

	//Enable A/B group
	#ifdef CC_MT5399
	//Enable B group
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);	
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1,RG_LVDSB_EN_D11);
	//Enable A group
	vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 0,RG_LVDSA_EN);	
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG9, 1,RG_LVDSA_EN);
	#else
	//Enable B group
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);	
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 1,RG_LVDSB_EN_D11);
	#endif	

	#ifdef CC_MTK_LOADER
       vDrvLVDS7To4FifoEnable();
	#else
       _Lvds7To4FifoReset = 2;
	#endif
#endif


#ifdef CC_COPLAT_MT82
vUtDelay2us(5);
#else
HAL_Delay_us(10);
#endif

//vIO32WriteFldAlign(LVDSTX_ANA0, 0, LVDSA0_RST);// analog LVDS normal
//vIO32WriteFldAlign(LVDSTX_ANA4, 0, LVDSB4_RST);// analog LVDS normal    

}

void vDrvMiniLVDSOff(void)
{
#ifdef CC_MT5881 //MT5881
				
	vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0,RG_LVDSB_EN_D11);
  	
    vIO32WriteFldMulti(REG_LVDS_TX_CFG1, P_Fld(0, RG_LVDSB_DRV_EN_EVEN_D11)|P_Fld(0,RG_LVDSB_DRV_EN_ODD_D11));  
    vIO32WriteFldAlign(REG_LVDS_TX_CFG0, 0, RG_LVDSB_BIAS_EN_D11 );
	
    vIO32WriteFldAlign(REG_VOPLL_CFG3, 1, RG_PPLL_PWD);
    vIO32WriteFldAlign(REG_VOPLL_CFG0, 1, RG_VPLL_PWD);
	vIO32WriteFldAlign(REG_VOPLL_CFG0, 1, RG_VPLL_BG_PWD);	
	
#endif
}

static void vDrvTgPreInit(void)
{
	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI )
    {
        vIO32WriteFldMulti(GR0, P_Fld(1, GR0_TC_EN)|P_Fld(0x100, GR0_TCTG_FCR)|P_Fld(0, GR0_TCTG_OEN));
    }
    else
    {
	vIO32WriteFldMulti(GR0, P_Fld(1, GR0_TC_EN)|P_Fld(0x002, GR0_TCTG_FCR)|P_Fld(0, GR0_TCTG_OEN));
    }
	vIO32WriteFldMulti(GR7, P_Fld(0, GR7_TCO_MUTE)|P_Fld(0, GR7_TCO_DEFAULT));
	vIO32WriteFldMulti(GR8, P_Fld(0, GR8_TCGPO)|P_Fld(0, GR8_TCGPOEN));
}

UINT8 vDrvMiniTgOnOff(MINI_TCON_POWER_SEQ_T MiniTgOn, UINT8 val)
{
	UINT8 u1Ret = 0;

	LOG(1,"%d On !!!\n", MiniTgOn);

	switch (MiniTgOn)
		{
			case MINI_TCON_0: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON0);
				break;
			case MINI_TCON_1: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON1);
				break;
			case MINI_TCON_2: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON2);
				break;
			case MINI_TCON_3: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON3);
				break;
			case MINI_TCON_4: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON4);
				break;
			case MINI_TCON_5: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON5);
				break;
			case MINI_TCON_6: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON6);
				break;
            case MINI_TCON_7: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON7);
				break;
			case MINI_TCON_8: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON8);
				break;
			case MINI_TCON_9: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCON9);
				break;
			case MINI_TCON_A: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCONA);
				break;
			case MINI_TCON_B: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCONB);
				break;
			case MINI_TCON_C: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCONC);
				break;
			case MINI_TCON_D: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCOND);
				break;
		    case MINI_TCON_E: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCONE);
				break;
			case MINI_TCON_F: vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_TCONF);
				break;
			case MINI_TCON_ALL:
				if (val)
					vIO32WriteFldAlign(GR0, 0xFFFF, GR0_TCTG_OEN);
				else
					vIO32WriteFldAlign(GR0, 0, GR0_TCTG_OEN);
				break;
			default:
				break;
		}
	
		if(IO32ReadFldAlign(GR0, GR0_TC_EN))
			u1Ret = 1;
		else
			u1Ret = 0;
		
		return u1Ret;	
}

/**
 * @brief vDrvTconMainLoop

 * TCON main loop

 * @param  void
 * @retval void
 */
void vDrvTconMainLoop(void)
{

    if(_u1TconReady == 0) return;

    vLogTconStateChange(_u1TconState);		
    switch (_u1TconState)
    {
        case TCON_STATE_INIT:
            break;
			
        case TCON_STATE_STANDBY:
            break;
			
        case TCON_STATE_MUTE:
            break;

        case TCON_STATE_NORMAL:
            break;
			
        case TCON_STATE_ERROR:
            break;
			
        default:
            _u1TconState = TCON_STATE_INIT;
            break;
    }
	
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
/**
 * @brief u1DrvTconPreInit

 * Init TCON at DC on

 * @param  void
 * @retval 0: fail, 1: success
 */
UINT8 u1DrvTconPreInit(void)
{
    UINT8 u1Ret = 0;
	
	// init HW at DC on

    // mute Tcon output

	//TODO_JOSH
    // gpio(default as input) or pinmux for tcon function
    // pinmux as tcon function

	// tg output ctrl
	/*
	1. DPM, gamma power
	2. GSP
	3. FLK --> high or low, according to customer spec.
	*/
	//vIO32WriteFldAlign(GR0, 0, GR0_TCTG_OEN);

    // minilvds
    
    // disable gamma power
	vTconDPMCtrl(0);
    
	return(u1Ret);
}

/**
 * @brief u1DrvTconMLDSVOPLLFreqSet

 * Set vopll for MiniLVDS

 * @param  UINT32 u4Freq, (Hz)
 * @param  MLVDS_PORT: Single(0), Dual(4), Four(8)
 * @param  MLVDS_RATION: 1(0), 1P5(1), 2(2), 3(3), 4(4)
 * @retval void
 */


void u1DrvTconMLDSVOPLLFreqSet(UINT32 u4Freq, UINT8 MLVDS_PORT,UINT8 MLVDS_RATION)
{
 vDrvMLDSVOPLLFreqSet(u4Freq,MLVDS_PORT,MLVDS_RATION);
}

UINT8 u1DrvMlvdsPreInit(void)
{
   // TCON_TOP
//   vIO32WriteFldMulti(TMGR2, P_Fld(0, TMGR2_TCLK_PLLRST)|P_Fld(1, TMGR2_TCLK_PLLSEL)|P_Fld(0, TMGR2_TCLK_PLLBP)
    vIO32WriteFldMulti(TMGR2, P_Fld(0, TMGR2_TCLK_PLLRST)|P_Fld(0, TMGR2_TCLK_PLLBP)
							|P_Fld(1, TMGR2_RG_FIFO_CK_SEL)|P_Fld(0xFFFF, TMGR2_CKEN_CFG1));
   vIO32WriteFldMulti(TMGR0, P_Fld(1, CKEN_CFG_EH)|P_Fld(1, CKEN_CFG_TCON)|P_Fld(1, CKEN_CFG_PWM_SCAN)
     						|P_Fld(1, CKEN_CFG_MLVDS)|P_Fld(0, CKEN_CFG_VB1)|P_Fld(0, CKEN_CFG_iDP)
							|P_Fld(0, CKEN_CFG_EPI)|P_Fld(0, CKEN_CFG_LVDS_ANA)|P_Fld(0, CKEN_CFG_PPML)
							|P_Fld(0, CKEN_CFG_LVDS_TX)|P_Fld(1, CKEN_CFG_Pixel_Reorder)|P_Fld(1, CKEN_CFG_2Port_FIFO));
	 
   vIO32WriteFldMulti(TMGR3, P_Fld(1, TMGR3_RG_TCON_OUT_SEL));
   vIO32WriteFldMulti(TMGR5, P_Fld(4, TMGR5_RG_FIFO_CTRL));

   // WXGA mini-LVDS Mute Pattern
   if(PANEL_GetPanelWidth()==1366)
   {
	   vIO32WriteFldMulti(MUTE_PATTERN_02, P_Fld(wDrvGetOutputHTotal(), MUTE_H_TOTAL)|P_Fld(8, MUTE_H_WIDTH));
	   vIO32WriteFldMulti(MUTE_PATTERN_03, P_Fld(0x20, MUTE_H_START)|P_Fld(0x556, MUTE_H_ACTIVE));
	   vIO32WriteFldMulti(MUTE_PATTERN_04, P_Fld(wDrvGetOutputVTotal(), MUTE_V_TOTAL)|P_Fld(1, MUTE_V_WIDTH));
	   vIO32WriteFldMulti(MUTE_PATTERN_05, P_Fld(6, MUTE_V_START)|P_Fld(0x300, MUTE_V_ACTIVE));
   }
   return 1;
}

UINT8 u1DrvMlvdsPostInit(void)
{
   
	//mini LVDS FIFO Reset
    vIO32WriteFldMulti(MLVDS_CTRL_09, P_Fld(1, ASYNC_FIFO_RST)|P_Fld(1, RG_FIFO_SEL));
	vIO32WriteFldMulti(MLVDS_CTRL_09, P_Fld(0, ASYNC_FIFO_RST)|P_Fld(1, RG_FIFO_SEL));

	//mini LVDS TCLK Start
	vIO32WriteFldMulti(FIFO_CTRL_01, P_Fld(1, TCLK_FIFO_START));
	
	//Enable D/A Interface FIFO
	vIO32WriteFldMulti(TMGR5, P_Fld(1, TMGR5_RG_FIFO_EN));
	
	return 1;
}

/**
 * @brief u1DrvTconInit

 * Init TCON

 * @param  void
 * @retval 0: fail, 1: success
 */
UINT8 u1DrvTconInit(void)
{
    UINT8 u1Ret = 0;

	
	// init settings at booting
    // SW setting
    _u1TconState = TCON_STATE_INIT;
    _u1TconStateOld = TCON_STATE_NOTREADY;

    _u1TconForceMute = 0;
	_u1GammaReady = 0;

	// Error Handling
    vErrorHandleInit();        

    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
    {
        // EPI Init
        u1TconCustEPIInit();        
         vEPI_AnalogInit();
        
        #ifdef SUPPORT_LOCK_FROM_SRVAD
           vIO32WriteFldAlign(TMGR18, 1, TMGR18_RG_LOCK_CTRL);
           vIO32WriteFldAlign(TMGR18, 0, TMGR18_RG_LOCK_VALUE);
        #endif
    }
    else
    {
        // miniLVDS Init
        u1DrvMlvdsPreInit();    
	    u1TconCustMlvdsInit();
		u1DrvMlvdsPostInit(); 
        vMiniLVDS_AnalogInit();
    }

	// TCON Init
    vDrvTgPreInit();     
	
    if(_fgTconCustSel == FALSE)
    {
    	u1TconCustTgInit();
    }
    else
    {
        vDrvTCONCustSel(_u1TconCustSel);
    }

    // Set TCON Pinmux and EPI LOCKN    
    _vDrvTconSetPinMux();

    // Flash TCON Init
    #ifdef CC_SUPPORT_FLASH_TCON
        vFlashTconInit();
    #endif

	// TCON On
  if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_MLVDS)
  	{
	  vDrvMiniTgOnOff(MINI_TCON_ALL,1);
  	}  
	
	// Enable TCON PMIC
    if(PANEL_GetDisplayType()!= PANEL_DISPLAY_TYPE_EPI)
    {
    #ifndef __MLVDS_slt__
	    vTconDPMCtrl(1);
    #else
        vTconDPMCtrl(0); //disable TCON power for SLT
    #endif
    }

	// Set Gamma Voltage
    if (_u1GammaReady == 0)
        vDrvTconGammaInit();

	// Analog TX ON
    if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
    {
        // EPI
        //vDrvLVDSOn(); 
    }
    else
    {
        // MiniLVDS Analog ON
	    vDrvMiniLvdsOn();
    }

    #if !defined(CC_FPGA) && !defined(CC_UBOOT)
        // tcon on
        vDrvTconOn();
    #endif
	_u1TconReady = 1;

	return(u1Ret);
}

/**
 * @brief u1DrvTcon_Enable

 * Enable TCON

 * @param  void
 * @retval 0: fail, 1: success
 */
void u1DrvTcon_Enable(BOOL fgOnOff)
{
	if (fgOnOff)
		vIO32WriteFldAlign(GR0, 0xFFFF, GR0_TCTG_OEN);
	else
		vIO32WriteFldAlign(GR0, 0, GR0_TCTG_OEN);

	if(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI)
	{
		if(fgOnOff)
		{
			vEPI_AnalogOn();
		}
		else
		{
			vEPI_AnalogOff();
		}
	}
	else
	{
		if(fgOnOff)
		{
			vDrvMiniLvdsOn();
		}
		else
		{
			vDrvMiniLVDSOff();
		}
	}
}

/**
 * @brief vDrvTconConfig

 * Configure TCON

 * @param  config
 * @retval 0: fail, 1: success
 */
UINT8 u1DrvTconConfig(UINT8 config)
{
    UINT8 u1Ret = 0;
	
    if(_u1TconReady == 0) return (0);

	// config Tcon settings
	
	return(u1Ret);	
}

void vDrvMiniLvdsConfig(UINT8 u1port, UINT8 u1pair, UINT8 u1bits)
{
    UINT8 u1tmp = 0;

	// minilvds ports
	u1tmp = 1;
	switch (u1port)
	{
		case MLVDS_SINGLE_PORT:
		    u1tmp = 0;
			break;
		case MLVDS_DUAL_PORT:
		    u1tmp = 1;
			break;
		case MLVDS_QUAL_PORT:
		    u1tmp = 3;
			break;
        default:
			LOG(1,"miniLVDS port number error %d!!!", u1port);
			break;
	}
    vIO32WriteFldAlign(FIFO_CTRL_01, u1tmp, TCLK_FIFO_ACTIVE_PORT);

	// minilvds pairs
	u1tmp = 6;
	switch (u1pair)
	{
		case MLVDS_3_PAIR:
		    u1tmp = 3;
			break;
		case MLVDS_4_PAIR:
		    u1tmp = 4;
			break;
		case MLVDS_5_PAIR:
		    u1tmp = 5;
			break;
		case MLVDS_6_PAIR:
		    u1tmp = 6;
			break;
		case MLVDS_8_PAIR:
		    u1tmp = 8;
			break;
        default:
			LOG(1,"miniLVDS pair number error %d!!!", u1pair);
			break;
	}
    vIO32WriteFldAlign(MLVDS_CTRL_05, u1tmp, RG_PAIR_SEL);

	// minilvds bit length
	u1tmp = 1;
	switch (u1bits)
	{
		case MLVDS_6_BIT:
		    u1tmp = 0;
			break;
		case MLVDS_8_BIT:
		    u1tmp = 1;
			break;
		case MLVDS_10_BIT:
		    u1tmp = 2;
			break;
        default:
			LOG(1,"miniLVDS bit length error %d!!!", u1bits);
			break;
	}

	if(u1tmp==0)
    vIO32WriteFldAlign(FIFO_CTRL_01, 1, RG_MLVDSM_6BIT_EN);
	else
	vIO32WriteFldAlign(FIFO_CTRL_01, 0, RG_MLVDSM_6BIT_EN);

	LOG(1,"miniLvds config %d port %d pair %d bit\n", u1port, u1pair, u1bits);	
}


UINT8 bbypass_isr=0;
//UINT8 init_VCC18=0;
/**
 * @brief u1DrvTCONOffOn

 * TCON power control

 * @param  void
 * @retval 0: fail, 1: success
 */
UINT8 u1DrvTconOffOn(VIDEO_TCON_POWER_SEQ_T eTconPowSeq, BOOL val)
{
    UINT8 u1Ret = 0;

	switch (eTconPowSeq)
	{
        case VIDEO_TCON_GST:
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_GST);
//            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_GST_END);
            break;
        case VIDEO_TCON_MCLK:
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_MCLK);
            break;
        case VIDEO_TCON_GCLK:
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_GCLK);
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_GCLK_END);
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_GST_END);
            break;
        case VIDEO_TCON_EO:
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_EO);
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_EO2);
            break;
        case VIDEO_TCON_VCOM_DYN:
            vIO32WriteFldAlign(GR0, val, GR0_TCTG_OEN_VCOM_DYN);
            break;
        case VIDEO_TCON_EPISIGNAL:
            if (val)
            {
                vEPI_AnalogOn();
#if 0//def SUPPORT_LOCK_FROM_SRVAD//workaround
            if(fgIsSupport120HzPanel()&&(init_VCC18==0))
            {
                init_VCC18 = 1;
                vIO32WriteFldAlign(REG_VOPLL_CFG0, 1, RG_VPLL_PREDIV);
                vUtDelay1ms(100);
                vIO32WriteFldAlign(REG_VOPLL_CFG0, 0, RG_VPLL_PREDIV);
            }
#endif
            }
            else
                vEPI_AnalogOff();
            break;
        case VIDEO_TCON_ALL:
            if (val)
                vIO32WriteFldAlign(GR0, 0xffff, GR0_TCTG_OEN);
            else
                vIO32WriteFldAlign(GR0, 0, GR0_TCTG_OEN);
            break;

        default:
            bbypass_isr = val;
            break;
    }

    if(IO32ReadFldAlign(GR0, GR0_TC_EN))
        u1Ret = 1;
    else
        u1Ret = 0;
	return u1Ret;	
}

/**
 * @brief u1DrvTconMute

 * mute / unmute TCON

 * @param  void
 * @retval 0: fail, 1: success
 */
UINT8 u1DrvTconForceMute(UINT8 val)
{
    UINT8 u1Ret = 0;
	
    if(_u1TconReady == 0) return (0);

    if (val)
    {
    	// mute
    	_u1TconForceMute = 1;
    }
    else
    {
    	// unmute
    	_u1TconForceMute = 0;
    }

    return(u1Ret);
}


/**
 * @brief u1DrvTconIsReady

 * TCON Ready or not

 * @param  void
 * @retval 0: not ready, 1: ready
 */
UINT8 u1DrvTconIsReady(void)
{ 
    return (_u1TconReady ? 1:0);
}

/**
 * @brief u1DrvTconState

 * TCON state

 * @param  void
 * @retval state
 */
UINT8 u1DrvTconState(void)
{
    return (_u1TconState);
}

/**
 * @brief vDrvTconDump

 * TCON Dump

 * @param  void
 * @retval 0: fail, 1: success
 */
/*
UINT8 u1DrvTconRegDump(void)
{
    UINT8 u1Ret = 1;
	UINT8 i = 0;
	UINT8 u1count;
	UINT32 u4tmp;
	  
    // CKGEN

	// error handling

	// post

	// minilvds
  	LOG(1,"minilvds reg dump\n");
	u1count = (sizeof(sRegMini) / sizeof(struct sTCONREGTABLE));
    for ( i = 0 ; i < u1count ; i++)
    {
        u4tmp = sRegMini[i].u4Addr;
		u4TconRegRead(u4tmp);
    }
    LOG(1,"Tcon reg dump %d counts, %d / %d\n", i, sizeof(sRegMini),sizeof(struct sTCONREGTABLE));
	


	// tg
  	LOG(1,"tg reg dump\n");
	u1count = (sizeof(sRegTg) / sizeof(struct sTCONREGTABLE));
    for ( i = 0 ; i < u1count ; i++)
    {
        u4tmp = sRegTg[i].u4Addr;
		u4TconRegRead(u4tmp);
    }
    LOG(1,"Tcon reg dump %d counts, %d / %d\n", i, sizeof(sRegTg),sizeof(struct sTCONREGTABLE));

	return(u1Ret);
}
*/
/**
 * @brief u1DrvTconRegDiff

 * TCON register diff

 * @param  void
 * @retval 0: fail, 1: success
 */
/*
UINT8 u1DrvTconRegDiff(void)
{
    UINT8 u1Ret = 1;
	UINT8 i = 0;
	UINT8 u1count;
	UINT32 u4tmp, u4tmp1;
	
	// minilvds
  	LOG(1,"minilvds reg diff\n");
	u1count = (sizeof(sRegMini) / sizeof(struct sTCONREGTABLE));
    for ( i = 0 ; i < u1count ; i++)
    {
        u4tmp = sRegMini[i].u4Addr;
		u4tmp1 = u4TconRegRead(u4tmp);
		if (u4tmp1 != sRegMini[i].u4Val)
		{
            LOG(1,"!!! diff addr 0x%8x, 0x%8x --> 0x%8x\n", u4tmp, u4tmp1, sRegMini[i].u4Val);			
		}
    }
    LOG(1,"Tcon reg diff %d counts, %d / %d\n", i, sizeof(sRegMini),sizeof(struct sTCONREGTABLE));

	// tg
  	LOG(1,"tg reg diff\n");
	u1count = (sizeof(sRegTg) / sizeof(struct sTCONREGTABLE));
    for ( i = 0 ; i < u1count ; i++)
    {
        u4tmp = sRegTg[i].u4Addr;
		u4tmp1 = u4TconRegRead(u4tmp);
		if (u4tmp1 != sRegTg[i].u4Val)
		{
            LOG(1,"!!! diff addr 0x%8x, 0x%8x --> 0x%8x\n", u4tmp, u4tmp1, sRegTg[i].u4Val);			
		}
    }
    LOG(1,"Tcon reg diff %d counts, %d / %d\n", i, sizeof(sRegTg),sizeof(struct sTCONREGTABLE));

    return (u1Ret);
}
*/
/**
 * @brief vDrvTconDmsg

 * TCON status for debugging

 * @param  void
 * @retval void
 */
void vDrvTconDmsg(void)
{   
    UNUSED(_u1TconForceMute);
    // dump debug message
    LOG(1,"Tcon force muting: %d \n", _u1TconForceMute);

	// dump gamma chip settings
    //vDrvTconGammaDump();
}

/**
 * @brief vDrvTconCmd

 * TCON cmds

 * @param  void
 * @retval void
 */
UINT8 u1DrvTconDbgCmd(UINT8 cmd)
{
    UINT8 u1Ret = 0;

    switch (_u1TconCmd)
    {
    	case TCON_CMD_NONE:
            break;
			
        case TCON_CMD_INIT:
            break;
	    case TCON_CMD_GAMMA:
			vDrvTconGammaInit();
			break;
			
        case TCON_CMD_STANDBY:
            break;
			
        case TCON_CMD_POWERON:
            break;
			
        case TCON_CMD_POWEROFF:
            break;
			
        case TCON_CMD_MUTE:
            break;
			
        case TCON_CMD_UNMUTE:
            break;
			
        case TCON_CMD_REGDUMP:
            break;
			
        case TCON_CMD_DMSG:
            break;
			
        case TCON_CMD_DLEVEL:
            break;
			
        case TCON_CMD_FW_STOP:
            break;
			
        case TCON_CMD_FW_RESUME:
            break;
		default:
			break;
    }	
	
    return(u1Ret);
}

//#define CC_POWER_MERGE_TEST
#ifdef CC_POWER_MERGE_TEST
static UINT8 tbypass_isr=0;
#endif
/**
 * @brief u1DrvTconTest

 * TCON Test

 * @param  void
 * @retval  void
 */
UINT8 u1DrvTconTest(UINT8 val)
{
	UINT8 u1Ret = 0;

#ifdef CC_POWER_MERGE_TEST

		//TCON INIT
		vIO32WriteFldAlign(GR0,0, GR0_TC_EN);
		vIO32WriteFldAlign(GR0,0xFFFF, GR0_TCTG_OEN);

		//GR4
		vIO32Write4B(0xf0034810,0x00000000);

		//GR5
		vIO32Write4B(0xf0034814,0x00000000);

		//GR6
		vIO32Write4B(0xf0034818,0x00000000);

		//TCON0
		vIO32Write4B(0xf0034840,0x0064014F);
		vIO32Write4B(0xf0034844,0x00000000);
		vIO32Write4B(0xf0034848,0x0064014F);
		vIO32Write4B(0xf003484C,0x04410004);
		vIO32Write4B(0xf0034850,0x20000000);
		vIO32Write4B(0xf0034854,0x80000000);
		vIO32Write4B(0xf0034858,0x00000007);
		vIO32Write4B(0xf003485C,0x00000000);
				
		//TCON1
		vIO32Write4B(0xf0034860,0x0064014F);
		vIO32Write4B(0xf0034864,0x00000000);
		vIO32Write4B(0xf0034868,0x0064014F);
		vIO32Write4B(0xf003486C,0x04410004);
		vIO32Write4B(0xf0034870,0x20000000);
		vIO32Write4B(0xf0034874,0x80000000);
		vIO32Write4B(0xf0034878,0x00000007);
		vIO32Write4B(0xf003487C,0x00000000);
				
		//TCON2
		vIO32Write4B(0xf0034880,0x0064014F);
		vIO32Write4B(0xf0034884,0x00000000);
		vIO32Write4B(0xf0034888,0x0064014F);
		vIO32Write4B(0xf003488C,0x04410004);
		vIO32Write4B(0xf0034890,0x20000000);
		vIO32Write4B(0xf0034894,0x80000000);
		vIO32Write4B(0xf0034898,0x00000007);
		vIO32Write4B(0xf003489C,0x00000000);

		//TCON3
		vIO32Write4B(0xf00348A0,0x0064014F);
		vIO32Write4B(0xf00348A4,0x00000000);
		vIO32Write4B(0xf00348A8,0x0064014F);
		vIO32Write4B(0xf00348AC,0x04410004);
		vIO32Write4B(0xf00348B0,0x20000000);
		vIO32Write4B(0xf00348B4,0x80000000);
		vIO32Write4B(0xf00348B8,0x00000007);
		vIO32Write4B(0xf00348BC,0x00000000);

		//TCON4
		vIO32Write4B(0xf00348C0,0x0064014F);
		vIO32Write4B(0xf00348C4,0x00000000);
		vIO32Write4B(0xf00348C8,0x0064014F);
		vIO32Write4B(0xf00348CC,0x04410004);
		vIO32Write4B(0xf00348D0,0x20000000);
		vIO32Write4B(0xf00348D4,0x80000000);
		vIO32Write4B(0xf00348D8,0x00000007);
		vIO32Write4B(0xf00348DC,0x00000000);

		//TCON5	
		vIO32Write4B(0xf00348E0,0x0064014F);
		vIO32Write4B(0xf00348E4,0x00000000);
		vIO32Write4B(0xf00348E8,0x0064014F);
		vIO32Write4B(0xf00348EC,0x04410004);
		vIO32Write4B(0xf00348F0,0x20000000);
		vIO32Write4B(0xf00348F4,0x80000000);
		vIO32Write4B(0xf00348F8,0x00000007);
		vIO32Write4B(0xf00348FC,0x00000000);
		
		//TCON6
		vIO32Write4B(0xf0034900,0x0064014F);
		vIO32Write4B(0xf0034904,0x00000000);
		vIO32Write4B(0xf0034908,0x0064014F);
		vIO32Write4B(0xf003490C,0x04410004);
		vIO32Write4B(0xf0034910,0x20000000);
		vIO32Write4B(0xf0034914,0x80000000);
		vIO32Write4B(0xf0034918,0x00000007);
		vIO32Write4B(0xf003491C,0x00000000);	
		
		//TCON7		
		vIO32Write4B(0xf0034920,0x0064014F);
		vIO32Write4B(0xf0034924,0x00000000);
		vIO32Write4B(0xf0034928,0x0064014F);
		vIO32Write4B(0xf003492C,0x04410004);
		vIO32Write4B(0xf0034930,0x20000000);
		vIO32Write4B(0xf0034934,0x80000000);
		vIO32Write4B(0xf0034938,0x00000007);
		vIO32Write4B(0xf003493C,0x00000000);

		//TCON8		
		vIO32Write4B(0xf0034940,0x0064014F);
		vIO32Write4B(0xf0034944,0x00000000);
		vIO32Write4B(0xf0034948,0x0064014F);
		vIO32Write4B(0xf003494C,0x04410004);
		vIO32Write4B(0xf0034950,0x20000000);
		vIO32Write4B(0xf0034954,0x80000000);
		vIO32Write4B(0xf0034958,0x00000007);
		vIO32Write4B(0xf003495C,0x00000000);

		//Set Pinmux
		BSP_PinSet( PIN_TCON0, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON1, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON2, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON3, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON4, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON5, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON6, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON7, PINMUX_FUNCTION1);
		BSP_PinSet( PIN_TCON8, PINMUX_FUNCTION1);
		 
		if(val == 1)
		{
			tbypass_isr = 1;
			u1Ret=1;
		}
		else
		{
			tbypass_isr = 0;
			u1Ret=0;
		}
#endif

	return u1Ret;	
}

void vDrvEPISLT(void)
{
#ifdef CC_MT5398
vIO32Write4B(0xf0060030, 0x00000000);
vIO32Write4B(0xf006002C, 0x0FFF0000);
vIO32Write4B(0xf0060028, 0x0C480000);
vIO32Write4B(0xf0060044, 0x00000000);
vIO32Write4B(0xf0060000, 0x000EC000);
vIO32Write4B(0xf0060004, 0x01000FFF);
vIO32Write4B(0xf0060008, 0x0FFF0000);
vIO32Write4B(0xf0060020, 0x0001C000);
vIO32Write4B(0xf0060024, 0xA4490FFF);
vIO32Write4B(0xf0034014, 0x00710000);
vIO32Write4B(0xf003402C, 0x2010012C);
vIO32Write4B(0xf0034030, 0x06008010);
vIO32Write4B(0xf0034034, 0x00000003);
vIO32Write4B(0xf0034038, 0x00210000);

#endif
}

#ifdef CC_SUPPORT_FLASH_TCON
INT32 i4FlashTconRead(UINT16 u2Offset, UINT8 *u1Data, UINT32 u4Length)
{
    UINT8 u1AQPartition;
    UINT32 u2AQSize;    
    INT32 i4Ret = 0;
    UINT64 u8Offset;
    
    UNUSED(u2AQSize);
    u1Data[0] = '\0';        
    u1AQPartition = (UINT8)DRVCUST_InitGet(eTconFlashTconPartition);
    if (u1AQPartition == 0xFF)
    {
        LOG(3,"AudioFlashAQPartition is not defined !!\n");
        return -1;
    }

#if 0    
    // check boundary 
    //u2AQSize = (UINT32)DRVCUST_OptGet(eAudioFlashAQSize);
    if (u2Offset > u2AQSize)
    {
        LOG(0,"Fail! AQ test read length larger than 0x%x\n", u2AQSize);
        return;
    }
#endif

#ifndef CC_NOR_DISABLE
     //   return ((INT32)NORSTG_Read(DRVCUST_OptGet(eTconFlashTconPartition), u2Offset, u1Data, u4Length));
            u8Offset = 0;
	     i4Ret = (INT32)DRVCUST_InitGet(eTconFlashTconPartition);
	    if (i4Ret > 1)
	    {
	        Printf("eTconFlashTconPartition %d", i4Ret);
	        u8Offset |= (((UINT64)(UINT32)i4Ret) << 32);	
	        i4Ret = NORPART_Read(u8Offset, (UINT32)u1Data, u4Length);
	        return i4Ret;
	    }   
#endif  //CC_NOR_DISABLE

    return (i4Ret);

}


void vFlashTconWrite(UINT16 u2Offset, UINT8 *u1Data, UINT32 u4size)
{
    UINT8 u1AQPartition;
    UINT32 u2AQSize;
    INT32 i4Ret;
    UINT64 u8Offset;
//    UINT32 u4Offset;
    
    UNUSED(u2AQSize);
    u1AQPartition = (UINT8)DRVCUST_InitGet(eTconFlashTconPartition);
    if (u1AQPartition == 0xFF)
    {
        LOG(3,"eTconFlashTconPartition is not defined !!\n");
        return;
    }    

#if 1
    // check boundary.
    //u2AQSize = (UINT32)DRVCUST_OptGet(eAudioFlashAQSize)
    if ((u2Offset + u4size) > FLASH_TCON_SIZE)
    {
        LOG(0,"TCON test write length larger than 0x%x\n", FLASH_TCON_SIZE);
        return;
    }
#endif    
//    u4Offset = ((UINT64)u1AQPartition << 32) | ((UINT64)u2Offset);
//    NANDPART_Write(u8Offset, (UINT32)u1Data, u4size);

#ifndef CC_NOR_DISABLE
      //  NORSTG_Write(DRVCUST_OptGet(eTconFlashTconPartition), u2Offset, u1Data, u4size);
            u8Offset = 0;
	    i4Ret = (INT32)DRVCUST_InitGet(eTconFlashTconPartition);
	    if (i4Ret > 1)
	    {
	        u8Offset |= (((UINT64)(UINT32)i4Ret) << 32);
			
		 i4Ret = NORPART_Erase(u8Offset, 1);
 	        i4Ret |= NORPART_Write(u8Offset, (UINT32)u1Data, u4size);
	    }      
#endif  //CC_NOR_DISABLE

}

void vFlashTconInit(void)
{
    UINT8 pau1Buf[FLASH_TCON_BUF_SIZE];
    UINT32 i;
    struct sTCONREGTABLE *pArray;    

    // check for header
    pau1Buf[0] = '\0';
    
    i4FlashTconRead(0, (UINT8*)&pau1Buf[0], FLASH_TCON_BUF_SIZE);
    
    if (x_strncmp((CHAR*)pau1Buf, "TCON", 4) !=0)
    {
        Printf("Flash TCON header is not correct, using default value\n");
        return;
    }

  
    Printf("\nFlash Tcon init reading...\n");
        
    for (i=0; i<FLASH_TCON_BUF_SIZE; i++)
    {
        if (i % 12 == 0) 
        {
        //    Printf("\n");
        }
        //Printf("0x%x ", pau1Buf[i]);
    }

   // Printf("\n");

    pArray = (struct sTCONREGTABLE *) &pau1Buf[0];

    if (pArray[0].u4Msk != GetCurrentPanelIndex())
    {
        Printf("Flash TCON panel ID is not matching, using default value\n");
        return;
    }

    for (i=0 ; i < 255 && i < pArray[0].u4Val ; i++)
    {
    //    Printf("tcon reg write %x %x %x \n", pArray[i].u4Addr, pArray[i].u4Val, pArray[i].u4Msk);
        vTconRegWrite(pArray[i].u4Addr,pArray[i].u4Val);
    }

     Printf("Flash Tcon init done...PID: %d, %d cmds\n", pArray[0].u4Msk, pArray[0].u4Val);
    
}

#define FLASH_TCON_HEADER_SZIE
void vFlashTconSave(void)
{
    UINT8 pau1Data[FLASH_TCON_BUF_SIZE] = {0};
    UINT32 i;
    struct sTCONREGTABLE *pArray;
    struct sTCONREGTABLE *pBufArray = NULL;
    UINT32 u4tmp = 0;
    UINT32 u4Length = 0;
    UINT32 u4TotalBytes = 0;    
    UINT32 u4CmdCnt = 0;
    UINT32 u4TotalCmdCnt = 0;    
    
    u4TotalBytes = 0;
    u4TotalCmdCnt = 0;
    // check for header
    pau1Data[0] = '\0';
    
    i4FlashTconRead(0, (UINT8*) &pau1Data[0], 12);
    
    if (x_strncmp((CHAR*)pau1Data, "TCON", 4) !=0)
    {
        LOG(0, "Flash TCON header is not correct, using default value\n");
        return;
    }

    // tg setting
    pArray = pTconCustTgGet(&u4CmdCnt);
    u4Length = sizeof(struct sTCONREGTABLE ) * u4CmdCnt;
    
    if (pArray == NULL)
    {
        LOG(0, "Flash TCON array not existed\n");
        return;
    }    
    
    if (u4CmdCnt ==0)
    {
        LOG(0, "Flash TCON data length is zero\n");
        return;
    }
   
    if (u4TotalCmdCnt + u4Length > FLASH_TCON_BUF_SIZE)
    {
        LOG(0, "Flash TCON data length over FLASH_TCON_BUF_SIZE %d\n", FLASH_TCON_BUF_SIZE);
        return;
    }
    
    pBufArray = (struct sTCONREGTABLE *) &pau1Data[0];
    
    for ( i = 0 ; i < u4CmdCnt ; i++)
    {
        u4tmp = pArray[i].u4Addr;
        pBufArray[i+1].u4Addr = u4tmp;
        pBufArray[i+1].u4Val = u4TconRegRead(u4tmp);
        pBufArray[i+1].u4Msk = pArray[i].u4Msk;
        Printf("Get reg %x %8x %x \n", pBufArray[i+1].u4Addr, pBufArray[i+1].u4Val, pBufArray[i+1].u4Msk);
    }
    
    u4TotalBytes += u4Length;
    u4TotalCmdCnt += u4CmdCnt;

    pBufArray[0].u4Val = 0x78796784;
    pBufArray[0].u4Val = u4TotalCmdCnt;
    pBufArray[0].u4Msk = GetCurrentPanelIndex();
    Printf("Set buf %x %8x %x \n", pBufArray[0].u4Addr, pBufArray[0].u4Val, pBufArray[0].u4Msk);

    Printf("save to flash\n");

    vFlashTconWrite(0, &pau1Data[0], u4TotalBytes);
    
    LOG(0,"Tcon reg save %d bytes\n", u4TotalBytes);
    
}

static UINT8 tcondata[12] = {0};
void vFlashTconWriteDefault(void)
{
    vFlashTconWrite(0, tcondata, sizeof(tcondata));
}
#endif  /*#ifdef CC_SUPPORT_FLASH_TCON*/

#define TCON_LOCK_HI_TH 100 //1.28V    
#define TCON_LOCK_LO_TH 50 //0.64V

void vDrvTconOutVsyncISR(void)
{
#ifdef SUPPORT_LOCK_FROM_SRVAD
UINT32 u4Lock;
    
if((bbypass_isr == 0)&&(PANEL_GetDisplayType()== PANEL_DISPLAY_TYPE_EPI))
{
    u4Lock = PDWNC_ReadServoADCChannelValue(SERVOADC_TCON_LOCK);
    if(u4Lock > TCON_LOCK_HI_TH) //LOCK is High
    {
        vIO32WriteFldAlign(TMGR18, 1, TMGR18_RG_LOCK_VALUE);
    }
    else if(u4Lock < TCON_LOCK_LO_TH) //Lock is Low
    {
        vIO32WriteFldAlign(TMGR18, 0, TMGR18_RG_LOCK_VALUE);
    }
}
#endif

#ifdef CC_POWER_MERGE_TEST
	if (tbypass_isr == 1)
	{
		vIO32WriteFldAlign(GR0,_fgTconTestEnable, GR0_TC_EN);
		_fgTCONTestcount++;
		if(_fgTCONTestcount >= 60)
		{
			_fgTconTestEnable = !(_fgTconTestEnable);
			_fgTCONTestcount = 0;
		}
	}
#endif

}


/**
 * @brief vDrvTCONSel

 * TCON power control

 * @param  void
 * @retval 0: fail, 1: success
 */
void vDrvTCONSel(VIDEO_TCON_TABLE_T eTconTableSel)
{
    _fgTconCustSel = TRUE;
    _u1TconCustSel = eTconTableSel;
    vDrvTCONCustSel(eTconTableSel);
}

UINT8 vDrvTCONRXCRC(UINT8 Mode)
{
	#ifndef CC_MT5398
    UINT8 u1CRC_Done=0;
    UINT32 u4counter=0;

	vIO32WriteFldAlign(MLVDS_CTRL_14, 1, RG_TOP_CRC_NEW);
	vIO32WriteFldAlign(MLVDS_CTRL_09, Mode, RG_FIFO_SEL); // 0: LVDS 1: mini-lvds 4:EPI
	
	//CRC Check Frame Range
	vIO32WriteFldAlign(MLVDS_CTRL_14, 0xA, RG_TOP_CRC_V_CNT);
	
	//CRC Check Lines Range
	vIO32WriteFldAlign(MLVDS_CTRL_15, 1, RG_TOP_CRC_H_START); 

  	vIO32WriteFldAlign(MLVDS_CTRL_15, PANEL_GetPanelHeight(), RG_TOP_CRC_H_END);

	vIO32WriteFldAlign(MLVDS_CTRL_16, 0x1FFF, RG_CRC_HLEN_NEW); 


	vIO32WriteFldAlign(MLVDS_CTRL_16, 0, RG_TOP_CRC_START); 

	//Clear CRC Check Result
	vIO32WriteFldAlign(MLVDS_CTRL_16, 1, RG_TOP_CRC_CLR); 
	vIO32WriteFldAlign(MLVDS_CTRL_16, 0, RG_TOP_CRC_CLR); 		
		
	//Start to do CRC Check
	vIO32WriteFldAlign(MLVDS_CTRL_16, 1, RG_TOP_CRC_START); 

	do
	{
		u4counter++;
		u1CRC_Done = IO32ReadFldAlign(MLVDS_CTRL_17,ST_TOP_CRC1_DONE)
				  && IO32ReadFldAlign(MLVDS_CTRL_18,ST_TOP_CRC2_DONE)
				  && IO32ReadFldAlign(MLVDS_CTRL_19,ST_TOP_CRC3_DONE)
				  && IO32ReadFldAlign(MLVDS_CTRL_20,ST_TOP_CRC4_DONE);
		
		if((IO32ReadFldAlign(MLVDS_CTRL_17, ST_TOP_CRC1_FAIL) == 1)
		 ||(IO32ReadFldAlign(MLVDS_CTRL_18, ST_TOP_CRC2_FAIL) == 1)
		 ||(IO32ReadFldAlign(MLVDS_CTRL_19, ST_TOP_CRC3_FAIL) == 1)
		 ||(IO32ReadFldAlign(MLVDS_CTRL_20, ST_TOP_CRC4_FAIL) == 1))
		{
			   Printf("R Channel RX_CRC Check Fail\n");
			return FALSE;
		}
		
	}while(u1CRC_Done == 0);
	
		  Printf("R Channel RX_CRC_Done = %d\n",u1CRC_Done);
		  Printf("R Channel RX_CRC OK, Loop Counter = %d\n",u4counter);
		  Printf("R Channel RX_CRC1 Value = %d\n",IO32ReadFldAlign(MLVDS_CTRL_17, ST_TOP_CRC1_VALUE));
		  Printf("R Channel RX_CRC2 Value = %d\n",IO32ReadFldAlign(MLVDS_CTRL_18, ST_TOP_CRC2_VALUE));
		  Printf("R Channel RX_CRC3 Value = %d\n",IO32ReadFldAlign(MLVDS_CTRL_19, ST_TOP_CRC3_VALUE));
		  Printf("R Channel RX_CRC4 Value = %d\n",IO32ReadFldAlign(MLVDS_CTRL_20, ST_TOP_CRC4_VALUE));
		  Printf("\n");

	#endif
	return TRUE;
}



UINT8 vDrvMLVDSCRC(void)
{
    UINT8 u1CRC_Done=0;
    UINT32 u4counter=0;
	UINT32 j,u4loopcounter=1;
	
	vIO32WriteFldAlign(MLVDS_CTRL_14, 0, RG_TOP_CRC_ALG_SEL);
	vIO32WriteFldAlign(MLVDS_CTRL_14, 1, RG_TOP_CRC_NEW);

	//CRC Check Frame Range
	vIO32WriteFldAlign(MLVDS_CTRL_14, 0xC8, RG_TOP_CRC_V_CNT);
	
	//CRC Check Lines Range
	vIO32WriteFldAlign(MLVDS_CTRL_15, 1, RG_TOP_CRC_H_START);
    vIO32WriteFldAlign(MLVDS_CTRL_15, PANEL_GetPanelHeight(), RG_TOP_CRC_H_END); 

    //Setting H_Active Length 
    vIO32WriteFldAlign(MLVDS_CTRL_16, 0xFFF, RG_CRC_HLEN_NEW);  

////////////////////////////L Channel CRC Check////////////////////////////////////////////
    #ifdef CC_MT5398
	for (j = 0 ; j < u4loopcounter ; j++)
	{
       u1CRC_Done=0;
       u4counter=0;

	vIO32WriteFldAlign(MLVDS_CTRL_48, 0, RG_TOP_CRC_START);

	//Select L/R Channel to do CRC Check
	vIO32WriteFldAlign(MLVDS_CTRL_48, 1, RG_TOP_CRC_SEL_L); //Select L Channel
		
	//Clear CRC Check Result
	vIO32WriteFldAlign(MLVDS_CTRL_48, 1, RG_TOP_CRC_CLR); 
	vIO32WriteFldAlign(MLVDS_CTRL_48, 0, RG_TOP_CRC_CLR); 		
		
	//Start to do CRC Check
	vIO32WriteFldAlign(MLVDS_CTRL_48, 1, RG_TOP_CRC_START); 

do
{
	u4counter++;
	u1CRC_Done = IO32ReadFldAlign(MLVDS_CTRL_49,ST_TOP_CRC1_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_50,ST_TOP_CRC2_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_51,ST_TOP_CRC3_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_52,ST_TOP_CRC4_DONE);
	
	if((IO32ReadFldAlign(MLVDS_CTRL_49, ST_TOP_CRC1_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_50, ST_TOP_CRC2_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_51, ST_TOP_CRC3_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_52, ST_TOP_CRC4_FAIL) == 1))
	{
		     Printf("L Channel %d CRC Check Fail\n",j+1);
		return FALSE;
	}
	
	}while(u1CRC_Done == 0);

       Printf("L Channel %d CRC_Done = %d\n",j+1,u1CRC_Done);
       Printf("L Channel %d CRC OK, Loop Counter = %d\n",j+1,u4counter);
	   Printf("L Channel %d CRC1 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_49, ST_TOP_CRC1_VALUE));
	   Printf("L Channel %d CRC2 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_50, ST_TOP_CRC2_VALUE));
	   Printf("L Channel %d CRC3 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_51, ST_TOP_CRC3_VALUE));
	   Printf("L Channel %d CRC4 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_52, ST_TOP_CRC4_VALUE));
	   Printf("\n");
	}
    #endif
////////////////////////////R Channel CRC Check////////////////////////////////////////////
	for (j = 0 ; j < u4loopcounter ; j++)
	{
    u1CRC_Done=0;
    u4counter=0;
	
	vIO32WriteFldAlign(MLVDS_CTRL_16, 0, RG_TOP_CRC_START); 

	//Select L/R Channel to do CRC Check
	vIO32WriteFldAlign(MLVDS_CTRL_16, 0, RG_TOP_CRC_SEL_L); //Select R Channel
		
	//Clear CRC Check Result
	vIO32WriteFldAlign(MLVDS_CTRL_16, 1, RG_TOP_CRC_CLR); 
	vIO32WriteFldAlign(MLVDS_CTRL_16, 0, RG_TOP_CRC_CLR); 		
		
	//Start to do CRC Check
	vIO32WriteFldAlign(MLVDS_CTRL_16, 1, RG_TOP_CRC_START); 

do
{
	u4counter++;
	u1CRC_Done = IO32ReadFldAlign(MLVDS_CTRL_17,ST_TOP_CRC1_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_18,ST_TOP_CRC2_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_19,ST_TOP_CRC3_DONE)
	          && IO32ReadFldAlign(MLVDS_CTRL_20,ST_TOP_CRC4_DONE);
	
	if((IO32ReadFldAlign(MLVDS_CTRL_17, ST_TOP_CRC1_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_18, ST_TOP_CRC2_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_19, ST_TOP_CRC3_FAIL) == 1)
	 ||(IO32ReadFldAlign(MLVDS_CTRL_20, ST_TOP_CRC4_FAIL) == 1))
	{
		   Printf("R Channel %d CRC Check Fail\n",j+1);
		return FALSE;
	}
	
	}while(u1CRC_Done == 0);

      Printf("R Channel %d CRC_Done = %d\n",j+1,u1CRC_Done);
      Printf("R Channel %d CRC OK, Loop Counter = %d\n",j+1,u4counter);
	  Printf("R Channel %d CRC1 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_17, ST_TOP_CRC1_VALUE));
	  Printf("R Channel %d CRC2 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_18, ST_TOP_CRC2_VALUE));
	  Printf("R Channel %d CRC3 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_19, ST_TOP_CRC3_VALUE));
	  Printf("R Channel %d CRC4 Value = %d\n",j+1,IO32ReadFldAlign(MLVDS_CTRL_20, ST_TOP_CRC4_VALUE));
	  Printf("\n");
	}
	
return TRUE;
}

void vDrvTCONTimingGenEn(UINT8 u1OnOff)
{
    if (!u1OnOff)
    {
	    vIO32WriteFldAlign(GR0, 0x0, GR0_TC_EN);
    }
    else
    {
	    vIO32WriteFldAlign(GR0, 0x1, GR0_TC_EN);
    }
}

void vDrvTCONTimingGenFrameCounter(UINT16 u2Frame)
{
	vIO32WriteFldAlign(GR0, u2Frame, GR0_TCTG_FCR);
}

void vDrvTCONTimingGenHsyncInvert(UINT8 u1OnOff)
{
	vIO32WriteFldAlign(GR1, u1OnOff, GR1_TCTG_HSINV);
}

void vDrvTCONTimingGenVsyncInvert(UINT8 u1OnOff)
{
	vIO32WriteFldAlign(GR1, u1OnOff, GR1_TCTG_VSINV);
}


void vDrvTCONISRSetLineCounter(UINT32 u4VFp, UINT32 u4VActive, UINT32 u4VBp)
{
	
    if (IS_PANEL_L12R12)
	{
	    vIO32WriteFldAlign(GR12, (((u4VActive/2)+u4VBp)+1), GR12_TCTG_VCNT_INT);
	}
	else
	{
		vIO32WriteFldAlign(GR12, (u4VBp +2), GR12_TCTG_VCNT_INT);
	}

}
void vDrvTCONISRSetFrameCounter(UINT16 u2INTRFrame)
{
    vIO32WriteFldAlign(GR12, u2INTRFrame, GR12_TCTG_FCNT_INT); 
}

void vDrvTCONIntrEn(void)
{
    vIO32WriteFldAlign(GR1, 0x30, GR1_TCON_INT_EN);  // [5:4]=2'b11
}

void vDrvTCONIntrDisable(void)
{
    vIO32WriteFldAlign(GR1, 0x0, GR1_TCON_INT_EN);  // [5:4]=2'b11
}

void vDrvTCONIntrClr(void)
{
    vIO32WriteFldAlign(GR1, 0x30, GR1_TCON_INT_CLR);  // [5:4]=2'b11
    vIO32WriteFldAlign(GR1, 0x00, GR1_TCON_INT_CLR);  // [5:4]=2'b00    
}

extern UINT8 fgApplyScanPWMSetData;
extern UINT8 u1ScanPwmCrossFlg;
extern UINT8 fgApplyPWMModeChange;
extern UINT8 fgScanPWMSetpControl;
extern void vResetPWMFreq(void);
extern UINT32 u4Support_ScanPWM;
extern UINT16 u2PscGetPanelVCNTStatus(void);

void vDrvTCONISR(UINT16 u2Vector)
{	
	static UINT16 u2Cnt = 0;
	static UINT8 u1PrevFcnt = 4;
	UINT8 u1ScanPWMApply = 2, u1CurrentFcnt = 0;
		
    if(u2Vector == VECTOR_TCON_TCH)  
    {
    	u1CurrentFcnt = u1DrvGetVsyncISRFCNTStatus();
	    vDrvTCONIntrClr();
		if(u1CurrentFcnt != u1PrevFcnt)
		{
			u1ScanPWMApply = 0;
		}
		else
		{
			if ((fgApplyScanPWMSetData) && (u1ScanPwmCrossFlg == 0))
			{
				u2Cnt = u2PscGetPanelVCNTStatus();
				if (u2Cnt >= u2DrvGetCrozzUpLimit())
				{
					u1ScanPWMApply = 0;			
				}
				else
				{
					u1ScanPWMApply = 1;			
					vDrvSetScanPWMSW_SETDATA();
				    //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_ON);
				    //vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_OFF);
				    vDrvSetScanPWMLatchMode(SCAN_PWM_LATCH_MODE_SETDATA_OFF,SCAN_PWM_LATCH_MODE_OFF,SCAN_PWM_ALIGN_MODE_VSYNC);  // Aligned Vsync
        			vDrvScanPWMDataFire();
				}
			}
		}

		if (u1ScanPWMApply == 1)
		{
			if (fgScanPWMSetpControl)
			{		
				if(u1DrvGetScanPWMStruct_Ready())
				{
			  		fgApplyScanPWMSetData = 0;								 //clear scanpwm setting flag
				}
				else
				{
			  	vDrvSetScanPWMSW_StepControl();		
				} 
			}
			else
			{
					fgApplyScanPWMSetData = 0;								   //clear scanpwm setting flag
			}
		}
		u1PrevFcnt = u1CurrentFcnt;
    }
    VERIFY(BIM_ClearIrq(VECTOR_TCON_TCH));
}

/** Brief
 * TCON ISR initial
 * @return void
 */
void vDrvTCONInitISR(void)
{
#ifndef CC_MTK_LOADER 
	static BOOL u1TCONISRInited = SV_FALSE;
	
	x_os_isr_fct pfnOldIsr=NULL;
	
	if (!u1TCONISRInited)
	{
		if (x_reg_isr(VECTOR_TCON_TCH, vDrvTCONISR, &pfnOldIsr) != OSR_OK)
		{
			LOG(0, "[TCON] Error: Fail to register TCON ISR!\n");
		}
		else
		{
			LOG(0, "[TCON] Success to register TCON ISR!\n");
		}
		if(!IO32ReadFldAlign(TMGR0,CKEN_CFG_TCON))
		{
			vIO32WriteFldAlign(TMGR0, 1, CKEN_CFG_TCON);
		}
        vDrvTCONTimingGenEn(SV_ON);
        vDrvTCONTimingGenFrameCounter(0x02);
        vDrvTCONISRSetFrameCounter(0x01);
        vDrvTCONTimingGenHsyncInvert(SV_ON);
        vDrvTCONTimingGenVsyncInvert(SV_ON);
		vDrvTCONIntrEn();
	
		u1TCONISRInited = SV_TRUE;
	}
#endif	
}
