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
 * $RCSfile: drv_vbyone.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#include "general.h"
#include "hw_vbyone.h"
#include "x_typedef.h"
#include "drv_lvds.h"
#include "hw_mlvds.h"
#include "hw_lvds.h"
#include "hw_tg.h"
#include "hw_scpos.h"
#include "drv_vbyone.h"
#include "drv_vdoclk.h"
#include "x_pinmux.h"

#define VB1_OSD_PATH_INIT 1
#define VB1_VIDEO_PATH_LANE3_7_INIT 1
#define ORYX_VB1_NEED_CHECK


// #define CC_VB1_DISABLE

/**
 * @brief vVByOneAnalogSetting
 * VB1 analog setting  
 * @param  void
 * @retval void
 */
void vVByOneAnalogSetting(void)
{
#ifndef CC_VB1_DISABLE
    UINT32 u4Value;
	UINT8 u1PreEmpEn;
	UINT8 u1PreEmpVal;

	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_VSW_CALIB_SEL_D11); 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG18, 0x1c, RG_LVDSA_ABIST_VL_ODD_SEL_D11); 
	
	u4Value = PANEL_GetDrivingCurrent();
    if (u4Value <= 15)
    {
      vDrvLVDSSetDriving(u4Value);
    }
    else
    {
      vDrvLVDSSetDriving(7);
    }

#ifdef CC_PANEL_FROM_FLASH
	// u1PreEmpEn should be from the flash panel data.
	// u1PreEmpVal should be from the flash panel data.
#else
	u1PreEmpEn = PANEL_GetPreEmphasisEn();
	u1PreEmpVal = PANEL_GetPreEmphasisVal();
#endif
	if (!u1PreEmpEn)
	{
		u1PreEmpEn = 0xFF;
	}
	else
	{
		u1PreEmpEn = 0;
	}
	
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x4, RG_LVDSA_TVCM);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0x4, RG_LVDSA_BIAS_SEL); 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x6, RG_LVDSA_NSRC);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x6, RG_LVDSA_PSRC);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0xFFF, RG_LVDSA_EXT_EN_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG15, 0x0, RG_LVDSA_LVDS_SEL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14, 0x0, RG_LVDSA_CLK_IQ_SEL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_ENEDGE_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_RESYNC_CLK_IQ_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_RESYNC_LVDS_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x0, RG_LVDSA_BIAS_ODD_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x0, RG_LVDSA_BIAS_EVEN_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_LDO_BIAS_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_LDO_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_LDO_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0x0, RG_VB1_LANE0_7_DRV_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14, u1PreEmpEn, RG_VB1_LANE0_7_PRE_PWD);
	vVByOnePreEmphasisSet(u1PreEmpVal);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0x4, RG_LVDSA_NSRC_PRE);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0x4, RG_LVDSA_PSRC_PRE);
    vIO32WriteFldMulti(REG_LVDS_TX_CFG15, P_Fld(1, RG_LVDSA_HSN_MODE_EVEN_EN_D11)|P_Fld(1, RG_LVDSA_HSP_MODE_EVEN_EN_D11)
                                         |P_Fld(1, RG_LVDSA_HSN_MODE_ODD_EN_D11)|P_Fld(1, RG_LVDSA_HSP_MODE_ODD_EN_D11));
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x0, RG_LVDSA_EN);
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x1, RG_LVDSA_EN);	
#endif
}

void vVByOneOSDAnalogSetting(void)
{
#ifndef CC_VB1_DISABLE
	UINT8 u1OSDPreEmpEn;

#ifdef CC_PANEL_FROM_FLASH
	// u1OSDPreEmpEn should be from the flash panel data.
#else
	u1OSDPreEmpEn = PANEL_GetPreEmphasisEn();
#endif
	if (!u1OSDPreEmpEn)
	{
		u1OSDPreEmpEn = 0xF;
	}
	else
	{
		u1OSDPreEmpEn = 0;
	}

	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0x0, RG_VB1_LANE8_11_DRV_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14, u1OSDPreEmpEn, RG_VB1_LANE8_11_PRE_PWD);

	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x0, RG_LVDSA_EN);
	HAL_Delay_us(LVDSA_EN_REST_DELAYTIME);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x1, RG_LVDSA_EN);	
#endif
}

/**
 * @brief vVByOneAnalogOffSetting
 * VB1 analog setting  
 * @param  void
 * @retval void
 */
void vVByOneAnalogOffSetting(void)
{
#ifndef CC_VB1_DISABLE
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0, RG_LVDSA_EN);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0, RG_LVDSA_EXT_EN_D11);
    vIO32WriteFldMulti(REG_LVDS_TX_CFG15, P_Fld(0, RG_LVDSA_HSN_MODE_EVEN_EN_D11)|P_Fld(0, RG_LVDSA_HSP_MODE_EVEN_EN_D11)
                                         |P_Fld(0, RG_LVDSA_HSN_MODE_ODD_EN_D11)|P_Fld(0, RG_LVDSA_HSP_MODE_ODD_EN_D11));
	vVByOnePreEmphasisOnOff(SV_OFF);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0xFFF, RG_LVDSA_DRV_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 1, RG_LVDSA_LDO_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 1, RG_LVDSA_LDO_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 1, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 1, RG_LVDSA_LDO_BIAS_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 1, RG_LVDSA_BIAS_EVEN_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 1, RG_LVDSA_BIAS_ODD_PWD);	
#endif
}

void vVByOneOSDAnalogOffSetting(void)
{
#ifndef CC_VB1_DISABLE
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14, 0xF, RG_VB1_LANE8_11_PRE_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13, 0xF, RG_VB1_LANE8_11_DRV_PWD);
#endif
}

void vVByOne_Impedance_Cal(void)
{
#ifndef CC_VB1_DISABLE
	UINT8 i = 0;
	UINT32 u4tmp = 0, u4TVO_Backup = 0;

	u4TVO_Backup = IO32ReadFldAlign(REG_LVDS_TX_CFG12, RG_LVDSA_TVO);

	vIO32WriteFldAlign(VB1SYS6, 0, VB1SYS6_START);
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_START);	
	#endif
	vIO32WriteFldAlign(TMGR16,0xfffff,TMGR16_RG_TMDS_CTRL_REG_EN);
	vIO32WriteFldAlign(TMGR17,0x3ff,TMGR17_RG_TMDS_REG_VALUE);
	vIO32WriteFldAlign(TMGR5,0x7,TMGR5_RG_FIFO_CTRL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0x1,RG_LVDSA_VSW_CALIB_SEL_D11); 
	vIO32WriteFldAlign(REG_LVDS_TX_CFG19,0x14,RG_LVDSA_ABIST_VL_EVEN_SEL_D11);
	vDrvLVDSSetDriving(6); // default 6mA 	vIO32WriteFldAlign(REG_LVDS_TX_CFG11, 0xd,RG_LVDSA_TVO);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0x4,RG_LVDSA_TVCM);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13,0x4,RG_LVDSA_BIAS_SEL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0x6,RG_LVDSA_NSRC);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0x6,RG_LVDSA_PSRC);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0xFFF,RG_LVDSA_EXT_EN_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG15,0,RG_LVDSA_LVDS_SEL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14,0,RG_LVDSA_CLK_IQ_SEL);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_ENEDGE_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_RESYNC_CLK_IQ_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_RESYNC_LVDS_SEL_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0,RG_LVDSA_BIAS_ODD_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0,RG_LVDSA_BIAS_EVEN_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_LDO_BIAS_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_LDO_BIAS_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_LDO_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0,RG_LVDSA_LDO_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13,0x0,RG_LVDSA_DRV_PWD);
	vVByOnePreEmphasisOnOff(SV_OFF);
	//vVByOnePreEmphasisSet(1);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13,0x4,RG_LVDSA_NSRC_PRE);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG13,0x4,RG_LVDSA_PSRC_PRE);
	vIO32WriteFldMulti(REG_LVDS_TX_CFG15, P_Fld(1, RG_LVDSA_HSN_MODE_EVEN_EN_D11)|P_Fld(1, RG_LVDSA_HSP_MODE_EVEN_EN_D11)
	                                 |P_Fld(1, RG_LVDSA_HSN_MODE_ODD_EN_D11)|P_Fld(1, RG_LVDSA_HSP_MODE_ODD_EN_D11));
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12,0x1,RG_LVDSA_EN);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20,0x1,RG_LVDSA_IMP_COMP_EN_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG21,0x1,RG_VB1_REV);

	vUtDelay2us(100);
	vIO32WriteFldMulti(TMGR7, P_Fld(0, TMGR7_RG_LOOPPAT_2TO1)|P_Fld(0, TMGR7_RG_LOOPPAT_EN)|P_Fld(1, TMGR7_IMP_CAL_SET)
							|P_Fld(0, TMGR7_RG_RXIMP)|P_Fld(0, TMGR7_RG_AUTOK_RX_IMP)|P_Fld(0, TMGR7_RG_TXIMP)
                            |P_Fld(0, TMGR7_REG_IMP_DP_DIS)|P_Fld(0, TMGR7_RG_IMPFREQSEL)|P_Fld(1, TMGR7_RG_AUTOK_TX_IMP)
                            |P_Fld(1, TMGR7_REG_SEL_1_ALL)|P_Fld(0, TMGR7_SAPIS_MODE_SYNC)|P_Fld(0, TMGR7_CTL_EN_CALIBRATION));
	
	vUtDelay2us(100);
	vIO32WriteFldMulti(TMGR7, P_Fld(0, TMGR7_RG_LOOPPAT_2TO1)|P_Fld(0, TMGR7_RG_LOOPPAT_EN)|P_Fld(1, TMGR7_IMP_CAL_SET)
							|P_Fld(0, TMGR7_RG_RXIMP)|P_Fld(0, TMGR7_RG_AUTOK_RX_IMP)|P_Fld(0, TMGR7_RG_TXIMP)
                            |P_Fld(0, TMGR7_REG_IMP_DP_DIS)|P_Fld(0, TMGR7_RG_IMPFREQSEL)|P_Fld(1, TMGR7_RG_AUTOK_TX_IMP)
                            |P_Fld(1, TMGR7_REG_SEL_1_ALL)|P_Fld(1, TMGR7_SAPIS_MODE_SYNC)|P_Fld(1, TMGR7_CTL_EN_CALIBRATION));

	do
	{
		u4tmp = IO32ReadFldAlign(TMGR24,TMGR24_HOLD);
		vUtDelay2us(10);
		i++;
		if(i>200)
		{
				printf("VB1 impedance cal error then break......\n ");
				break;
		}
		
	}while(!u4tmp);
	
	u4tmp = IO32ReadFldAlign(TMGR24,TMGR24_DA_IMPCAL_TX_LAT);
	#if 0
	if(u4tmp>0 && u4tmp<0xf)
	{
			printf("VB1 VB1 impedance cal  pass and value =%x .................\n",u4tmp);
	}
	else
	#else
	{
		//printf("VB1 VB1 impedance cal  fail .........................\n");
		printf("VB1 VB1 impedance cal  set to default terminaion .........................\n");
		vIO32WriteFldAlign(TMGR7, 0, TMGR7_IMP_CAL_SET);
		vIO32WriteFldMulti(TMGR8, P_Fld(0x8, TMGR8_RG_LVDSA_TERM_ECK)|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O0)|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O1)
									|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O2)|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O3)|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O4)
									|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_O5)|P_Fld(0x8, TMGR8_RG_LVDSA_TERM_OCK));
		vIO32WriteFldMulti(TMGR9, P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E0)|P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E1)|P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E2)
									|P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E3)|P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E4)|P_Fld(0x8, TMGR9_RG_LVDSA_TERM_E5));
	}
	#endif
	
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x0, RG_LVDSA_IMP_COMP_EN_D11);

	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, u4TVO_Backup, RG_LVDSA_TVO);


#endif
}

void VByOne_PreInit_E3(void)
{
/*
	vIO32Write4BMsk(VB1SYS0,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS1,0x7D634400,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS2,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS3,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS4,0x54321000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS6,0xBA987600,0xFFFFFFFF);
	vIO32Write4BMsk(VB1SYS8,0x00000003,0xFFFFFFFF);
	vIO32Write4BMsk(VB1ANA0,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1RES, 0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1ANA2,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1ANA3,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1ANA4,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1ANA5,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1ANA6,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1ANA7,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1ANA8,0x00000000,0xFFFFFFFF);

	vIO32Write4BMsk(VB1OSDSYS0,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDSYS1,0x7D634400,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDSYS2,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDSYS3,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1OSDSYS4,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDSYS6,0x00000000,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDSYS8,0x00000003,0xFFFFFFFF);
	vIO32Write4BMsk(VB1OSDANA0,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1OSDRES, 0x00000000,0xFFFFFFFF);		
	vIO32Write4BMsk(VB1OSDANA2,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1OSDANA3,0x00000000,0xFFFFFFFF);	
	vIO32Write4BMsk(VB1OSDANA4,0x00000000,0xFFFFFFFF);	
*/
}
#if defined(CONFIG_OPM) || defined(CC_SUPPORT_STR)
    BOOL fgVB1Init = FALSE;
#else
    static BOOL fgVB1Init = FALSE;
#endif

void vVByOneInit(void)
{
#ifndef CC_VB1_DISABLE
	UINT8 i = 0;
	UINT32 u4tmp = 0;	
    UINT32 *pu4PinMap,*pu4PNSwap;

    if (IO32ReadFldAlign(DDDS_08, FLAG_VB1_INITIAL) == 1)
    {
        fgVB1Init = TRUE;
    }

	if (!fgVB1Init)
	{

	if (IS_IC_5890_ES3())
	{
		VByOne_PreInit_E3();
	}
	
	do
	{
		u4tmp = u4IO32Read4B(RGS_LVDSTX_PLL_CFG0);
		vUtDelay2us(10);
		i++;
		if(i>200)
		{
			printf("VB1 Vpll clock check error......\n ");
			break;
		}
		
	}while((u4tmp &0xC0000000)!=0x80000000); //bit 31 30


    // switch to LVDS TX mode               
	vIO32WriteFldAlign(FIFO_CTRL_01, 3, TCLK_FIFO_ACTIVE_PORT); 
	vIO32WriteFldAlign(TMGR2, 2, TMGR2_RG_FIFO_CK_SEL); 
	vIO32WriteFldAlign(FIFO_CTRL_01, 1, TCLK_FIFO_START); 


	vIO32WriteFldMulti(VB1SYS0, P_Fld(0, VB1SYS0_BIT_INV)|
								#if VB1_VIDEO_PATH_LANE3_7_INIT
								P_Fld(1, VB1SYS0_LANE_7_EN)|
								P_Fld(1, VB1SYS0_LANE_6_EN)|
								P_Fld(1, VB1SYS0_LANE_5_EN)|
								P_Fld(1, VB1SYS0_LANE_4_EN)|
								#endif
								P_Fld(1, VB1SYS0_LANE_3_EN)|
								P_Fld(1, VB1SYS0_LANE_2_EN)|
								P_Fld(1, VB1SYS0_LANE_1_EN)|
								P_Fld(1, VB1SYS0_LANE_0_EN)|
								P_Fld(0, VB1SYS0_FIELDBET)|
								P_Fld(0, VB1SYS0_PATGEN_START)|
								P_Fld(1, VB1SYS0_DE_RST_CTRL)|
								P_Fld(VB1SYS0_LANE_NUM_4, VB1SYS0_LANE_NUM)|
								P_Fld(VB1SYS0_COL_4BYTES, VB1SYS0_COL));
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldMulti(VB1OSDSYS0, P_Fld(0, VB1OSDSYS0_BIT_INV)|
								P_Fld(0, VB1OSDSYS0_VSYN_INV)|
								P_Fld(0, VB1OSDSYS0_HSYN_INV)|
								P_Fld(1, VB1OSDSYS0_LANE_3_EN)|
								P_Fld(1, VB1OSDSYS0_LANE_2_EN)|
								P_Fld(1, VB1OSDSYS0_LANE_1_EN)|
								P_Fld(1, VB1OSDSYS0_LANE_0_EN)|
								P_Fld(0, VB1OSDSYS0_FIELDBET)|
								P_Fld(0, VB1OSDSYS0_PATGEN_START)|
								P_Fld(1, VB1OSDSYS0_DE_RST_CTRL)|
								P_Fld(VB1OSDSYS0_LANE_NUM_4, VB1OSDSYS0_LANE_NUM)|
								P_Fld(VB1OSDSYS0_COL_4BYTES, VB1OSDSYS0_COL));
	#endif

	#ifdef CC_SCPOS_3DTV_SUPPORT
	vIO32WriteFldAlign(VB1SYS0, 1, VB1SYS0_VB1_3DLR_EN); 
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldAlign(VB1OSDSYS0, 0, VB1OSDSYS0_VB1_3DLR_EN);	
	#endif
	#endif


	vIO32WriteFldMulti(VB1SYS3, P_Fld(0, VB1SYS3_CRC_EN)|
								P_Fld(0, VB1SYS3_LANE_PRBS_EN)|
								P_Fld(1, VB1SYS3_LANE3_ANA)|
								P_Fld(1, VB1SYS3_LANE2_ANA)|
								P_Fld(1, VB1SYS3_LANE1_ANA)|
								P_Fld(1, VB1SYS3_LANE0_ANA));

	vIO32WriteFldMulti(VB1SYS1, 
					#if VB1_VIDEO_PATH_LANE3_7_INIT
								P_Fld(7, VB1SYS1_LANE7_MUX)|
								P_Fld(6, VB1SYS1_LANE6_MUX)|
								P_Fld(5, VB1SYS1_LANE5_MUX)|
								P_Fld(4, VB1SYS1_LANE4_MUX)|	
					#endif
								P_Fld(3, VB1SYS1_LANE3_MUX)|
								P_Fld(2, VB1SYS1_LANE2_MUX)|
								P_Fld(1, VB1SYS1_LANE1_MUX)|
								P_Fld(0, VB1SYS1_LANE0_MUX));	
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldMulti(VB1OSDSYS1, P_Fld(3, VB1OSDSYS1_LANE3_MUX)|
								P_Fld(2, VB1OSDSYS1_LANE2_MUX)|
								P_Fld(1, VB1OSDSYS1_LANE1_MUX)|
								P_Fld(0, VB1OSDSYS1_LANE0_MUX));			
	#endif

	// ==Lane Swap ==
	if(NULL == (pu4PinMap = (UINT32*)DRVCUST_PanelGet(ePanelVB1LaneSwap)))
	{
		#ifdef SUPPORT_PANEL_GRAPH_MODE
		vIO32WriteFldMulti(VB1SYS6, P_Fld(VB1_GRAPH_LANE3, VB1SYS6_VB1_OSD_LANE_3_SWAP)|
									P_Fld(VB1_GRAPH_LANE2, VB1SYS6_VB1_OSD_LANE_2_SWAP)|
									P_Fld(VB1_GRAPH_LANE1, VB1SYS6_VB1_OSD_LANE_1_SWAP)|
									P_Fld(VB1_GRAPH_LANE0, VB1SYS6_VB1_OSD_LANE_0_SWAP));			
		#endif									
		
		#if VB1_VIDEO_PATH_LANE3_7_INIT
		vIO32WriteFldMulti(VB1SYS6, P_Fld(VB1_DATA_LANE7, VB1SYS6_LANE_7_SWAP)|
									P_Fld(VB1_DATA_LANE6, VB1SYS6_LANE_6_SWAP));									
		vIO32WriteFldMulti(VB1SYS4, P_Fld(VB1_DATA_LANE5, VB1SYS4_LANE_5_SWAP)|
									P_Fld(VB1_DATA_LANE4, VB1SYS4_LANE_4_SWAP));	
		#endif
		
		vIO32WriteFldMulti(VB1SYS4, P_Fld(VB1_DATA_LANE3, VB1SYS4_LANE_3_SWAP)|
									P_Fld(VB1_DATA_LANE2, VB1SYS4_LANE_2_SWAP)|
									P_Fld(VB1_DATA_LANE1, VB1SYS4_LANE_1_SWAP)|
									P_Fld(VB1_DATA_LANE0, VB1SYS4_LANE_0_SWAP));	
	}
	else
	{
		#ifdef SUPPORT_PANEL_GRAPH_MODE
		vIO32WriteFldMulti(VB1SYS6, P_Fld(pu4PinMap[11], VB1SYS6_VB1_OSD_LANE_3_SWAP)|
									P_Fld(pu4PinMap[10], VB1SYS6_VB1_OSD_LANE_2_SWAP)|
									P_Fld(pu4PinMap[9], VB1SYS6_VB1_OSD_LANE_1_SWAP)|
									P_Fld(pu4PinMap[8], VB1SYS6_VB1_OSD_LANE_0_SWAP));			
		#endif									
		
		#if VB1_VIDEO_PATH_LANE3_7_INIT
		vIO32WriteFldMulti(VB1SYS6, P_Fld(pu4PinMap[7], VB1SYS6_LANE_7_SWAP)|
									P_Fld(pu4PinMap[6], VB1SYS6_LANE_6_SWAP));									
		vIO32WriteFldMulti(VB1SYS4, P_Fld(pu4PinMap[5], VB1SYS4_LANE_5_SWAP)|
									P_Fld(pu4PinMap[4], VB1SYS4_LANE_4_SWAP));	
		#endif
		
		vIO32WriteFldMulti(VB1SYS4, P_Fld(pu4PinMap[3], VB1SYS4_LANE_3_SWAP)|
									P_Fld(pu4PinMap[2], VB1SYS4_LANE_2_SWAP)|
									P_Fld(pu4PinMap[1], VB1SYS4_LANE_1_SWAP)|
									P_Fld(pu4PinMap[0], VB1SYS4_LANE_0_SWAP));			
	}
	
#if VB1_8LANE_00112233SWAP  // need to remove after rerification
		if (GetCurrentPanelIndex()==PANEL_VB1_V420DK1_KS1)	// for 8-lane VB1 pre-test
		{
			Printf("[VB1]swap FOR lane7~lane0: 3 3 2 2 1 1 0 0\n");
		#if VB1_VIDEO_PATH_LANE3_7_INIT
			vIO32WriteFldMulti(VB1SYS6, P_Fld(VB1_DATA_LANE3, VB1SYS6_LANE_7_SWAP)|
										P_Fld(VB1_DATA_LANE3, VB1SYS6_LANE_6_SWAP));									
			vIO32WriteFldMulti(VB1SYS4, P_Fld(VB1_DATA_LANE2, VB1SYS4_LANE_5_SWAP)|
										P_Fld(VB1_DATA_LANE2, VB1SYS4_LANE_4_SWAP));	
		#endif
			
			vIO32WriteFldMulti(VB1SYS4, P_Fld(VB1_DATA_LANE1, VB1SYS4_LANE_3_SWAP)|
										P_Fld(VB1_DATA_LANE1, VB1SYS4_LANE_2_SWAP)|
										P_Fld(VB1_DATA_LANE0, VB1SYS4_LANE_1_SWAP)|
										P_Fld(VB1_DATA_LANE0, VB1SYS4_LANE_0_SWAP));
		}
#endif	

#ifdef PANEL_FHD_BACKEND
	if (GetCurrentPanelIndex()==PANEL_FHD_BACKEND)
	{
		vIO32WriteFldMulti(VB1SYS4, P_Fld(VB1_DATA_LANE1, VB1SYS4_LANE_3_SWAP)|
									P_Fld(VB1_DATA_LANE0, VB1SYS4_LANE_2_SWAP));
	}
#endif

	// == PN Swap ==
	if(NULL == (pu4PNSwap = (UINT32*)DRVCUST_PanelGet(ePanelVB1LanePNSwap)))
	{
		#ifdef SUPPORT_PANEL_GRAPH_MODE
		vIO32WriteFldMulti(VB1OSDSYS3,	P_Fld(0, VB1OSDSYS3_LANE3_INV)|
										P_Fld(0, VB1OSDSYS3_LANE2_INV)|
										P_Fld(0, VB1OSDSYS3_LANE1_INV)|
										P_Fld(0, VB1OSDSYS3_LANE0_INV));
		#endif	
		
		vIO32WriteFldMulti(VB1SYS3, 
									#if VB1_VIDEO_PATH_LANE3_7_INIT
									P_Fld(0, VB1SYS3_LANE7_INV)|
									P_Fld(0, VB1SYS3_LANE6_INV)|
									P_Fld(0, VB1SYS3_LANE5_INV)|
									P_Fld(0, VB1SYS3_LANE4_INV)|
									#endif
									P_Fld(0, VB1SYS3_LANE3_INV)|
									P_Fld(0, VB1SYS3_LANE2_INV)|
									P_Fld(0, VB1SYS3_LANE1_INV)|
									P_Fld(0, VB1SYS3_LANE0_INV));			
								
	}
	else
	{

		#ifdef SUPPORT_PANEL_GRAPH_MODE
		vIO32WriteFldMulti(VB1OSDSYS3,	P_Fld(pu4PNSwap[11], VB1OSDSYS3_LANE3_INV)|
										P_Fld(pu4PNSwap[10], VB1OSDSYS3_LANE2_INV)|
										P_Fld(pu4PNSwap[9], VB1OSDSYS3_LANE1_INV)|
										P_Fld(pu4PNSwap[8], VB1OSDSYS3_LANE0_INV));
		#endif		
		
		vIO32WriteFldMulti(VB1SYS3, 
							#if VB1_VIDEO_PATH_LANE3_7_INIT
									P_Fld(pu4PNSwap[7], VB1SYS3_LANE7_INV)|
									P_Fld(pu4PNSwap[6], VB1SYS3_LANE6_INV)|
									P_Fld(pu4PNSwap[5], VB1SYS3_LANE5_INV)|
									P_Fld(pu4PNSwap[4], VB1SYS3_LANE4_INV)|
							#endif
									P_Fld(pu4PNSwap[3], VB1SYS3_LANE3_INV)|
									P_Fld(pu4PNSwap[2], VB1SYS3_LANE2_INV)|
									P_Fld(pu4PNSwap[1], VB1SYS3_LANE1_INV)|
									P_Fld(pu4PNSwap[0], VB1SYS3_LANE0_INV));																	
	}

	vIO32WriteFldAlign(VB1SYS1, 0, VB1SYS1_MON_SEL);	  	

	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldAlign(VB1OSDSYS1,0, VB1OSDSYS1_MON_SEL);	  	  	
	#endif

	vIO32WriteFldMulti(VB1SYS3, P_Fld(0, VB1SYS3_ANA_RX_EN)|
								P_Fld(0x3FF, VB1SYS3_LANE3_ANA_VALUE));  // only use lane 3 caibration result       

	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldMulti(VB1OSDSYS3, P_Fld(0, VB1OSDSYS3_ANA_RX_EN)|
								P_Fld(0x3FF, VB1OSDSYS3_LANE3_ANA_VALUE));         
	#endif

    vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);
    vIO32WriteFldAlign(TMGR5, 1, TMGR5_RG_FIFO_EN);

	vVByOne_Impedance_Cal();

	vIO32WriteFldMulti(VB1SYS3, P_Fld(0, VB1SYS3_LANE3_ANA)|
								P_Fld(0, VB1SYS3_LANE2_ANA)|
								P_Fld(0, VB1SYS3_LANE1_ANA)|
								P_Fld(0, VB1SYS3_LANE0_ANA));
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldMulti(VB1OSDSYS3,	P_Fld(0, VB1OSDSYS3_LANE3_ANA)|
									P_Fld(0, VB1OSDSYS3_LANE2_ANA)|
									P_Fld(0, VB1OSDSYS3_LANE1_ANA)|
									P_Fld(0, VB1OSDSYS3_LANE0_ANA));
	#endif

	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x1, RG_LVDSA_LDO_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x1, RG_LVDSA_LDO_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x1, RG_LVDSA_LDO_BIAS_ODD_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG20, 0x1, RG_LVDSA_LDO_BIAS_EVEN_PWD_D11);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x1, RG_LVDSA_BIAS_ODD_PWD);
	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, 0x1, RG_LVDSA_BIAS_EVEN_PWD);

	#ifdef SUPPORT_PANEL_GRAPH_MODE
	vIO32WriteFldMulti(VB1OSDSYS6, P_Fld(1, VB1OSDSYS6_DE_EN)|P_Fld(1, VB1OSDSYS6_EN));
	vIO32WriteFldAlign(SCPIP_SCSYS_67, 0x0, SCSYS_67_GFX_OSD_VSYNC_DLY); //sony v total issue
	//vIO32WriteFldAlign(SCPIP_SCSYS_14, 0x2, SCSYS_14_SC_OSD_SRC_SEL);
	vIO32WriteFldMulti(SCPIP_SCSYS_66, P_Fld(0, SCSYS_66_GFX_OSD_HSYNC_SEL)|P_Fld(0, SCSYS_66_GFX_OSD_VSYNC_SEL));	
	#endif

	vIO32WriteFldAlign(DDDS_08, 1, FLAG_VB1_INITIAL);
	}
#endif
}

void vVByOneFormatConfig(void)
{
#ifndef CC_VB1_DISABLE
    UINT32 u4MSBSW;
    
    u4MSBSW = LVDS_DISP_FORMAT;

	if (u4MSBSW == LVDS_MSB_SW_ON) // NS, VESA
	{
		vIO32WriteFldAlign(VB1SYS1, 1, VB1SYS1_TTL_REORDER);	
	}
	else if (u4MSBSW == LVDS_MSB_SW_OFF) // JEIDA
	{
		vIO32WriteFldAlign(VB1SYS1, 0, VB1SYS1_TTL_REORDER);	  		
	}
#endif
}

void vDrvVB14K2KPanelInterfaceConfig(void)
{
	if (IS_DISABLE_LVDS_LINEBUFFER)
	{
		vIO32WriteFldAlign(VINTF_B0, 1,  VINTF_LINE_BUFFER_BYPASS); 
	}	
	else
	{
		vIO32WriteFldAlign(VINTF_B0, 0,  VINTF_LINE_BUFFER_BYPASS); 
	}

	if (IS_VB1_2SEC_ENABLE && IS_IC_5861())
	{
		vIO32WriteFldAlign(VINTF_P19, 1, VINTF_2SEC_EN);
	}
	else
	{
		vIO32WriteFldAlign(VINTF_P19, 0, VINTF_2SEC_EN);
	}
}

void vDrvVB1Config(void)
{   
#ifndef CC_VB1_DISABLE
    // set VB1 format
    vVByOneFormatConfig();

	// line buffer bypass & 2-section
	vDrvVB14K2KPanelInterfaceConfig();

    if (HSYNC_POLARITY == HSYNC_LOW)
    {  
        vIO32WriteFldAlign(VB1SYS0, 0, VB1SYS0_HSYN_INV);                
    }
    else
    {        
        vIO32WriteFldAlign(VB1SYS0, 1, VB1SYS0_HSYN_INV);                
    }

    if (VSYNC_POLARITY == VSYNC_LOW)
    {       
        vIO32WriteFldAlign(VB1SYS0, 0, VB1SYS0_VSYN_INV);                
    }
    else
    {
        vIO32WriteFldAlign(VB1SYS0, 1, VB1SYS0_VSYN_INV);                     
    }

#endif   
}

void vVByOneOSDFormatConfig(void)
{
#ifndef CC_VB1_DISABLE
#ifdef SUPPORT_PANEL_GRAPH_MODE
    UINT32 u4MSBSW;
	if (IS_SUPPORT_VB1_GRAPH_MODE)
		u4MSBSW = GraphMode_FORMAT;
	else
		u4MSBSW = GM_FORMAT_CASE0_SMODE1;

	if (u4MSBSW == GM_FORMAT_CASE0_SMODE1) // Case 0 -> Smode1
	{
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_SEL);
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_A_SW);			
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_MS);
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_BTI_ORDER_SW);
	}
	else if (u4MSBSW == GM_FORMAT_CASE1_SMODE0) // Case 1 -> Smode0
	{
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_SEL);
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_A_SW);			
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_MS);
		vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_BTI_ORDER_SW);
	}
	else if (u4MSBSW == GM_FORMAT_CASE2_MMODE0) // Case 2 -> Mmode0
	{
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_SEL);
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_A_SW);		
		vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_MS);
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_BTI_ORDER_SW);
	}	
	else if (u4MSBSW == GM_FORMAT_CASE3_INHOUSE) // Case 3 -> In-House Mode
	{
		vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_SEL);  // Gazelle will fix
		vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_A_SW);	
		vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_MS);
		vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_BTI_ORDER_SW);
		vVByOneOSDFormatCustConfig();
	}	

	if (IS_SUPPORT_VB1_GRAPH_MODE)
	{
		if (GRAPH_HSYNC_POLARITY == HSYNC_LOW)
	    {  
	        vIO32WriteFldAlign(VB1OSDSYS0, 0, VB1OSDSYS0_HSYN_INV);                
	    }
	    else
	    {        
	        vIO32WriteFldAlign(VB1OSDSYS0, 1, VB1OSDSYS0_HSYN_INV);                
	    }

	    if (GRAPH_VSYNC_POLARITY == VSYNC_LOW)
	    {       
	        vIO32WriteFldAlign(VB1OSDSYS0, 0, VB1OSDSYS0_VSYN_INV);                
	    }
	    else
	    {
	        vIO32WriteFldAlign(VB1OSDSYS0, 1, VB1OSDSYS0_VSYN_INV);                     
	    }
	}
#endif	
#endif
}


/**
 * @brief vVByOneDigitalReset
 * disable VB1 digital module
 * @param  void
 * @retval void
 */
void vVByOneDigitalReset(void)
{
#ifndef CC_VB1_DISABLE
    vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);
    vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_EN);
    vIO32WriteFldAlign(VB1SYS6, 0, VB1SYS6_START);    
	#ifdef SUPPORT_PANEL_GRAPH_MODE
    vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_START);    
	#endif
#endif
}

void vVByOneOSDDigitalReset(void)
{
#ifndef CC_VB1_DISABLE
#ifdef SUPPORT_PANEL_GRAPH_MODE
    vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_START);   
#endif
#endif
}

/**
 * @brief vDrvVB1On
 * Power On VBYONE
 * @param  void
 * @retval void
 */
void vDrvVByOneOn(void)
{
#ifndef CC_VB1_DISABLE
    UINT32 u4DispBit;
    UINT32 u4VB1Lane;
    
    u4DispBit = DISP_BIT;
    u4VB1Lane = LVDS_OUTPUT_PORT;

    BSP_PinSet( PIN_TCON12, PINMUX_FUNCTION3);  // LOCKN
    BSP_PinSet( PIN_TCON11, PINMUX_FUNCTION3);  // HDPN
    
	switch (u4DispBit)
	{
	case DISP_36BIT:	   
		vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_5BYTES,VB1SYS0_COL);
	   break;
   	 case DISP_30BIT:   	 	
		vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_4BYTES,VB1SYS0_COL);
    	break;		
	 case DISP_24BIT:
     case DISP_18BIT:
   	 	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_3BYTES,VB1SYS0_COL);
    	break;
	default:
    	break;
	}

	vDrvVB1Config();
	if(u4VB1Lane == OCTA_PORT)
	{
	   if (PANEL_IsSupport4K2KDD()&& IS_IC_5861() && (!IS_IC_5861_ES1()))
	   vIO32WriteFldAlign(CKGEN_OCLK_TEST, 3,FLD_VB1_CK_SEL); // Gazelle ES2 : /8 Gazelle ES1 :PD
	   else
	   vIO32WriteFldAlign(CKGEN_OCLK_TEST, 2,FLD_VB1_CK_SEL);
	   
	   vVByOneAnalogSetting();
	   vVByOneDigitalReset();			   
	   vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_4,VB1SYS0_LANE_NUM);	   
	   vIO32WriteFldAlign(VB1SYS0,0xFF,VB1SYS0_LANE_EN_MUX);
	}
	else if(u4VB1Lane == FOUR_PORT)
	{
	   vIO32WriteFldAlign(CKGEN_OCLK_TEST, 2,FLD_VB1_CK_SEL);
	   vVByOneAnalogSetting();
	   vVByOneDigitalReset();			   
	   vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_4,VB1SYS0_LANE_NUM);	   
	   if(IS_VB1_ALL_LANE_ON)
	   vIO32WriteFldAlign(VB1SYS0,0xFF,VB1SYS0_LANE_EN_MUX);
	   else
	   vIO32WriteFldAlign(VB1SYS0,0xF,VB1SYS0_LANE_EN_MUX);
	}
	else if (u4VB1Lane == DUAL_PORT)
	{
	   vIO32WriteFldAlign(CKGEN_OCLK_TEST, 1,FLD_VB1_CK_SEL);
	   vVByOneAnalogSetting();
	   vVByOneDigitalReset();
	   vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_2,VB1SYS0_LANE_NUM);
	   if(IS_VB1_ALL_LANE_ON)
	   vIO32WriteFldAlign(VB1SYS0,0xFF,VB1SYS0_LANE_EN_MUX);
	   else
	   {
	   #ifdef PANEL_FHD_BACKEND
			if (GetCurrentPanelIndex()==PANEL_FHD_BACKEND)
				vIO32WriteFldAlign(VB1SYS0,0xF,VB1SYS0_LANE_EN_MUX);
			else
	   #endif
				vIO32WriteFldAlign(VB1SYS0,0x3,VB1SYS0_LANE_EN_MUX);			
	   }
			   
	}
	else if (u4VB1Lane == SINGLE_PORT)
	{
	   vIO32WriteFldAlign(CKGEN_OCLK_TEST, 0,FLD_VB1_CK_SEL);
	   vVByOneAnalogSetting();
	   vVByOneDigitalReset();
	   vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_1,VB1SYS0_LANE_NUM);
	   if(IS_VB1_ALL_LANE_ON)
	   vIO32WriteFldAlign(VB1SYS0,0xFF,VB1SYS0_LANE_EN_MUX);
	   else
	   vIO32WriteFldAlign(VB1SYS0,0x3,VB1SYS0_LANE_EN_MUX);
	}
		   
	vIO32WriteFldAlign(VB1SYS0,0x1,VB1SYS0_DE_RST_CTRL);
	vIO32WriteFldMulti(VB1SYS1, P_Fld(3, VB1SYS1_LANE3_MUX)|
								P_Fld(2, VB1SYS1_LANE2_MUX)|
								P_Fld(1, VB1SYS1_LANE1_MUX)|
								P_Fld(0, VB1SYS1_LANE0_MUX)|
								P_Fld(0, VB1SYS1_MON_SEL));

	vDrvVByOneOSDOn();
	
    vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);
    vIO32WriteFldAlign(TMGR5, 1, TMGR5_RG_FIFO_EN);
    vIO32WriteFldAlign(VB1SYS6, 1, VB1SYS6_START);	
#endif
}


/**
 * @brief vDrvVB1On
 * Power On VBYONE
 * @param  void
 * @retval void
 */
void vDrvVByOneOSDOn(void)
{
#ifndef CC_VB1_DISABLE
#ifdef SUPPORT_PANEL_GRAPH_MODE
    UINT32 u4DispBit;   
	UINT32 u4VB1Lane;

	if (IS_SUPPORT_VB1_GRAPH_MODE)
		u4VB1Lane = GM_LANE_NUM;
	else
		u4VB1Lane = GM_LANE_4;

	u4DispBit = DISP_30BIT;

    BSP_PinSet( PIN_GPIO54, PINMUX_FUNCTION2);  // LOCKN
    BSP_PinSet( PIN_GPIO53, PINMUX_FUNCTION5);  // HDPN
	
	switch (u4DispBit)
	{
	case DISP_36BIT:	   
		vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_COL_5BYTES,VB1OSDSYS0_COL);
	   break;
   	 case DISP_30BIT:   	 	
		vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_COL_4BYTES,VB1OSDSYS0_COL);
    	break;		
	 case DISP_24BIT:
     case DISP_18BIT:
   	 	vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_COL_3BYTES,VB1OSDSYS0_COL);
    	break;
	default:
    	break;
	}

	vVByOneOSDFormatConfig();

	if(u4VB1Lane == GM_LANE_4)
	{
	   vIO32WriteFldAlign(CKGEN_TCLK_TEST, 2,FLD_CLK75M2_CK_SEL);
	   vIO32WriteFldAlign(CKGEN_TCLK_TEST, 0,FLD_OCLK_DIV4_SEL);
	   vVByOneOSDAnalogSetting();
	   vVByOneOSDDigitalReset();	
	   vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_LANE_NUM_4,VB1OSDSYS0_LANE_NUM);	   
	   vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);
	}
	else if (u4VB1Lane == GM_LANE_2)
	{
		if(LVDS_OUTPUT_PORT == DUAL_PORT)
		{
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 1,FLD_CLK75M2_CK_SEL);  // 148.5MHz/2=74.25
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 0,FLD_OCLK_DIV4_SEL);	// oclk = oclk_osd =148.5MHz	
		}
		else
		{
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 2,FLD_CLK75M2_CK_SEL);  // 297 MHz/4=74.25
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 1,FLD_OCLK_DIV4_SEL);	// oclk/2 = oclk_osd = 297MHz/2 =148.5MHz
		}
		vVByOneOSDAnalogSetting();
		vVByOneOSDDigitalReset();
		vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_LANE_NUM_2,VB1OSDSYS0_LANE_NUM);
		if(IS_VB1_ALL_LANE_ON)
		vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);	   	
		else
		vIO32WriteFldAlign(VB1OSDSYS0,0x3,VB1OSDSYS0_LANE_EN_MUX);
	}
	else if (u4VB1Lane == GM_LANE_1)
	{
		if(LVDS_OUTPUT_PORT == DUAL_PORT)
		{
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 1,FLD_CLK75M2_CK_SEL);
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 1,FLD_OCLK_DIV4_SEL);
		}			
		else if(LVDS_OUTPUT_PORT == SINGLE_PORT)
		{
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 0,FLD_CLK75M2_CK_SEL);
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 0,FLD_OCLK_DIV4_SEL);
		}
		else
		{
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 2,FLD_CLK75M2_CK_SEL);
			vIO32WriteFldAlign(CKGEN_TCLK_TEST, 2,FLD_OCLK_DIV4_SEL);
		}
		vVByOneOSDAnalogSetting();
		vVByOneOSDDigitalReset();
		vIO32WriteFldAlign(VB1OSDSYS0,VB1OSDSYS0_LANE_NUM_1,VB1OSDSYS0_LANE_NUM);
		if(IS_VB1_ALL_LANE_ON)
		vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);	   	
		else
		vIO32WriteFldAlign(VB1OSDSYS0,0x3,VB1OSDSYS0_LANE_EN_MUX);
	}
		   
	vIO32WriteFldAlign(VB1OSDSYS0,0x1,VB1OSDSYS0_DE_RST_CTRL);
	vIO32WriteFldMulti(VB1OSDSYS1, P_Fld(3, VB1OSDSYS1_LANE3_MUX)|
								P_Fld(2, VB1OSDSYS1_LANE2_MUX)|
								P_Fld(1, VB1OSDSYS1_LANE1_MUX)|
								P_Fld(0, VB1OSDSYS1_LANE0_MUX)|
								P_Fld(0, VB1OSDSYS1_MON_SEL));
	
    vIO32WriteFldAlign(VB1OSDSYS6, 1, VB1OSDSYS6_START);	
#endif
#endif
}

/**
 * @brief vDrvVB1Off
 * Power off VBYONE digital
 * @param  void
 * @retval void
 */
void vDrvVByOneOff(void)
{
#ifndef CC_VB1_DISABLE
     vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_CTRL);
     vIO32WriteFldAlign(TMGR5, 0, TMGR5_RG_FIFO_EN);
     vIO32WriteFldAlign(VB1SYS6, 0, VB1SYS6_START);
#endif
}

/**
 * @brief vDrvVB1OSDOff
 * Power off digital VBYONE OSD path
 * @param  void
 * @retval void
 */
void vDrvVByOneOSDOff(void)
{
#ifndef CC_VB1_DISABLE
	#ifdef SUPPORT_PANEL_GRAPH_MODE
     vIO32WriteFldAlign(VB1OSDSYS6, 0, VB1OSDSYS6_START);
	#endif
#endif
}

/**
 * @brief vDrvVByOneCRCCheck
 * Data Path crc check
 * @param  void
 * @retval void
 */
void vDrvVByOneCRCCheck(void)
{
#ifndef CC_VB1_DISABLE
	UINT32 u4VB1Lane;
	UINT8 u1Back3DLREN;

	u4VB1Lane = LVDS_OUTPUT_PORT;
	u1Back3DLREN = IO32ReadFldAlign(VB1SYS0,VB1SYS0_VB1_3DLR_EN);
    
	vIO32WriteFldAlign(VB1SYS0,0x0,VB1SYS0_VB1_3DLR_EN);
	vIO32WriteFldAlign(VB1RES,0x1,VB1RES_DISP_R_CONTRL);
	HAL_Delay_us(4* 20000); // delay 4vsync
	
	vIO32WriteFldAlign(VB1SYS3,0x0,VB1SYS3_CRC_EN);
	vIO32WriteFldAlign(VB1SYS3,0x1,VB1SYS3_CRC_EN);
	HAL_Delay_us(4* 20000); // delay 4vsync
	
	if(u4VB1Lane == OCTA_PORT)
	{
		if(IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC2_FAIL)||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC3_FAIL)
		|| IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC4_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC5_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC6_FAIL)||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC7_FAIL))
		{
			printf("8 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC2_FAIL)|IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC3_FAIL)
													   || IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC4_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC5_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC6_FAIL)|IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC7_FAIL));
		}
		else
		{
			printf("8 lane CRC pass\n");
		}
	}	
	else if(u4VB1Lane == FOUR_PORT)
	{
		if(IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC2_FAIL)||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC3_FAIL))
		{
			printf("4 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC2_FAIL)|IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC3_FAIL));
		}
		else
		{
			printf("4 lane CRC pass\n");
		}
	}
	else if (u4VB1Lane == DUAL_PORT)
	{   
		if(IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) ||IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) )
		{
			printf("2 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL) |IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC1_FAIL) );
		}	
		else
		{
			printf("2 lane CRC pass\n");
		}
	}
	else if (u4VB1Lane == SINGLE_PORT)
	{
		if(IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL))
		{
	    	printf("1 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1SYS8, VB1SYS8_CRC0_FAIL));
		}	
			else
		{
	    	printf("1 lane CRC pass\n");
		}
	}
	vIO32WriteFldAlign(VB1SYS0,u1Back3DLREN,VB1SYS0_VB1_3DLR_EN);
	
#endif
}

/**
 * @brief vDrvVByOneOSDCRCCheck
 * OSD path crc check
 * @param  void
 * @retval void
 */
void vDrvVByOneOSDCRCCheck(void)
{
#ifndef CC_VB1_DISABLE
#ifdef SUPPORT_PANEL_GRAPH_MODE
	UINT32 u4VB1Lane;

	if (IS_SUPPORT_VB1_GRAPH_MODE)
    	u4VB1Lane = GM_LANE_NUM;
	else
    	u4VB1Lane = GM_LANE_4;

	vIO32WriteFldAlign(VB1OSDRES,0x1,VB1OSDRES_DISP_R_CONTRL);
	HAL_Delay_us(4* 20000); // delay 4vsync	
	vIO32WriteFldAlign(VB1OSDSYS3,0x0,VB1OSDSYS3_CRC_EN);
	vIO32WriteFldAlign(VB1OSDSYS3,0x1,VB1OSDSYS3_CRC_EN);
	HAL_Delay_us(4* 20000); // delay 4vsync
	
	if(u4VB1Lane == GM_LANE_4)
	{
		if(IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL) ||IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC1_FAIL) ||IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC2_FAIL)||IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC3_FAIL))
		{
			printf("4 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL) |IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC1_FAIL) |IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC2_FAIL)|IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC3_FAIL));
		}
		else
		{
			printf("4 lane CRC pass\n");
		}
	}
	else if (u4VB1Lane == GM_LANE_2)
	{   
		if(IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL) ||IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC1_FAIL) )
		{
			printf("2 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL) |IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC1_FAIL) );
		}	
		else
		{
			printf("2 lane CRC pass\n");
		}
	}
	else if (u4VB1Lane == GM_LANE_1)
	{
		if(IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL))
		{
	    	printf("1 lane CRC fail and Fali value =%x\n",IO32ReadFldAlign(VB1OSDSYS8, VB1OSDSYS8_CRC0_FAIL));
		}	
			else
		{
	    	printf("1 lane CRC pass\n");
		}
	}
#endif
#endif
}


/**
 * @brief vGetVByOneStatus
 * Get VBYONE status
 * @param  void
 * @retval void
 */
void vGetVByOneStatus(void)
{
#ifndef CC_VB1_DISABLE	
	printf("[VB1] LOCKN = %d \n", IO32ReadFldAlign(VB1SYS8,VB1SYS8_LOCKN));
	printf("[VB1] HTDPN = %d \n", IO32ReadFldAlign(VB1SYS8,VB1SYS8_HTDPN));	
	#ifdef SUPPORT_PANEL_GRAPH_MODE
	printf("[VB1_OSD] LOCKN = %d \n", IO32ReadFldAlign(VB1OSDSYS8,VB1OSDSYS8_LOCKN));
	printf("[VB1_OSD] HTDPN = %d \n", IO32ReadFldAlign(VB1OSDSYS8,VB1OSDSYS8_HTDPN));	
	#endif	
#endif	
}

#ifndef CC_VB1_DISABLE
UINT16 u2UnlockCnt = 0;
UINT16 u2UnHTDP = 0;
UINT8 u1VB1Chk = 0;
#endif	

void vSetVByOneUnLockChk(UINT8 u1En)
{
#ifndef CC_VB1_DISABLE	
	u1VB1Chk = u1En;
#endif
}

void vGetVByOneUnLock(void)
{
#ifndef CC_VB1_DISABLE	
	if (IO32ReadFldAlign(VB1SYS8,VB1SYS8_LOCKN)==1)
	{
	u2UnlockCnt++;
	printf("[VB1] UnLOCKN Count=%d \n", u2UnlockCnt);
	}
	if (IO32ReadFldAlign(VB1SYS8,VB1SYS8_HTDPN)==1)	
	{
	u2UnHTDP++;
	printf("[VB1] HTDPN_FAIL Count=%d \n", u2UnHTDP);	
	}
#endif	
}

void vGetVByOneUnLockCnt(void)
{
#ifndef CC_VB1_DISABLE	
	printf("[VB1] u2UnlockCnt=%d \n", u2UnlockCnt);
	printf("[VB1] u2UnHTDP=%d \n", u2UnHTDP);
#endif	

}

void vDrvVB1LockCtrl(UINT8 bOnOff, UINT8 bValue)
{
	vIO32WriteFldAlign(TMGR18,bOnOff,TMGR18_RG_LOCK_CTRL);
	vIO32WriteFldAlign(TMGR18,bValue,TMGR18_RG_LOCK_VALUE);
}

static void VByOneToggleVOPLL(void)
{
#ifndef CC_VB1_DISABLE	
	// toggle VOPLL Power
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG0, 1, REG_LVDSTX_PLL_CFG0_VPLL_BG_LVDS_PWD);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG0, 1, REG_LVDSTX_PLL_CFG0_VPLL_BG_VB1_TX_PWD);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG1, 1, REG_LVDSTX_PLL_CFG1_VPLL_PWD);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG4, 1, REG_LVDSTX_PLL_CFG4_PPLL_PWD); 	  
	HAL_Delay_us(400);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG1, 0, REG_LVDSTX_PLL_CFG1_VPLL_PWD); 
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG4, 0, REG_LVDSTX_PLL_CFG4_PPLL_PWD);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG0, 0, REG_LVDSTX_PLL_CFG0_VPLL_BG_LVDS_PWD);
	vIO32WriteFldAlign(REG_LVDSTX_PLL_CFG0, 0, REG_LVDSTX_PLL_CFG0_VPLL_BG_VB1_TX_PWD);
	HAL_Delay_us(20);
#endif	
}

void vVByOneLane4_8bit_script(void)
{
#ifndef CC_VB1_DISABLE							   
	#ifdef ORYX_VB1_NEED_CHECK
	vIO32WriteFldMulti(REG_LVDSTX_PLL_CFG1, P_Fld(0x1D, REG_LVDSTX_PLL_CFG1_VPLL_FBKDIV)|P_Fld(1, REG_LVDSTX_PLL_CFG1_VPLL_FBKSEL));
	#endif
	VByOneToggleVOPLL();
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_3BYTES,VB1SYS0_COL);
	vIO32WriteFldAlign(VB1SYS1,0,VB1SYS1_TTL_REORDER);
	vIO32WriteFldAlign(CKGEN_OCLK_TEST, 2,FLD_VB1_CK_SEL);
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_4,VB1SYS0_LANE_NUM);
	vIO32WriteFldAlign(VB1SYS0,0xff,VB1SYS0_LANE_EN_MUX);
	vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);
#endif
}

void vVByOneLane4_10bit_script(void)
{
#ifndef CC_VB1_DISABLE
	#ifdef ORYX_VB1_NEED_CHECK
	vIO32WriteFldMulti(REG_LVDSTX_PLL_CFG1, P_Fld(0x13, REG_LVDSTX_PLL_CFG1_VPLL_FBKDIV)|P_Fld(3, REG_LVDSTX_PLL_CFG1_VPLL_FBKSEL));
	#endif
	VByOneToggleVOPLL();
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_4BYTES,VB1SYS0_COL);
	vIO32WriteFldAlign(CKGEN_OCLK_TEST, 2,FLD_VB1_CK_SEL);
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_4,VB1SYS0_LANE_NUM);
	vIO32WriteFldAlign(VB1SYS0,0xff,VB1SYS0_LANE_EN_MUX);
	vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);
#endif  
}

void vVByOneLane4_12bit_script(void)
{
#ifndef CC_VB1_DISABLE
	#ifdef ORYX_VB1_NEED_CHECK
	vIO32WriteFldMulti(REG_LVDSTX_PLL_CFG1, P_Fld(0x18, REG_LVDSTX_PLL_CFG1_VPLL_FBKDIV)|P_Fld(3, REG_LVDSTX_PLL_CFG1_VPLL_FBKSEL));       
	#endif
	VByOneToggleVOPLL();
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_COL_5BYTES,VB1SYS0_COL);
	vIO32WriteFldAlign(VB1SYS1,0,VB1SYS1_TTL_REORDER);
	vIO32WriteFldAlign(CKGEN_OCLK_TEST, 2,FLD_VB1_CK_SEL);
	vIO32WriteFldAlign(VB1SYS0,VB1SYS0_LANE_NUM_4,VB1SYS0_LANE_NUM);
	vIO32WriteFldAlign(VB1SYS0,0xff,VB1SYS0_LANE_EN_MUX);
	vIO32WriteFldAlign(VB1OSDSYS0,0xF,VB1OSDSYS0_LANE_EN_MUX);
#endif  
}

void vVByOneTestMode(void)
{
#ifndef CC_VB1_DISABLE
	vIO32WriteFldAlign(REG_LVDS_TX_CFG14,0xfff,RG_LVDSA_PRE_PWD);
	vIO32WriteFldAlign(VB1SYS3,0x1,VB1SYS3_LANE3_ANA);
	vIO32WriteFldAlign(VB1SYS3,0x1,VB1SYS3_LANE2_ANA);
	vIO32WriteFldAlign(VB1SYS3,0x1,VB1SYS3_LANE1_ANA);
	vIO32WriteFldAlign(VB1SYS3,0x1,VB1SYS3_LANE0_ANA);	
	vIO32WriteFldAlign(VB1SYS3,0x3FF,VB1SYS3_LANE3_ANA_VALUE);		
#endif  
}

void vVByOneLaneSwapForTest(UINT8 u1Mode)
{
	UINT32 u4Lane0123=0;
	UINT32 u4Lane4567=0;
	UINT32 u4OSDLane0123=0;
	u4Lane0123 = IO32ReadFldAlign(VB1SYS4, VB1SYS4_LANE0123_SWAP);
	u4Lane4567 = (IO32ReadFldAlign(VB1SYS6,VB1SYS6_LANE67_SWAP)<<8)+IO32ReadFldAlign(VB1SYS4,VB1SYS4_LANE45_SWAP);
	u4OSDLane0123 = IO32ReadFldAlign(VB1SYS6,VB1SYS6_VB1_OSD_LANE0123_SWAP);
	Printf("[VB1][Test]lane0123=0x%x, lane4567=0x%x, OSDlane0123=0x%x\n", u4Lane0123, u4Lane4567, u4OSDLane0123);
	switch(u1Mode)
	{
		case 1:  // for VB1 analog measure
			vIO32WriteFldAlign(TMGR18,0x1,TMGR18_RG_LOCK_CTRL);
			vIO32WriteFldAlign(TMGR18,0x1,TMGR18_RG_LOCK_CTRL_OSD);
			vIO32WriteFldAlign(VB1SYS4,0x10,VB1SYS4_LANE45_SWAP);  // lane4/5/6/7 swap to lane0/1/2/3
			vIO32WriteFldAlign(VB1SYS6,0x32,VB1SYS6_LANE67_SWAP);  
			vIO32WriteFldAlign(VB1SYS6,0x3210,VB1SYS6_VB1_OSD_LANE0123_SWAP);  // OSDlane0/1/2/3 swap to lane0/1/2/3
			Printf("[VB1][Test][mode_1]lane4567:0x%x, OSDlane0123:0x%x\n",
				(IO32ReadFldAlign(VB1SYS6,VB1SYS6_LANE67_SWAP)<<8)+IO32ReadFldAlign(VB1SYS4,VB1SYS4_LANE45_SWAP),
				IO32ReadFldAlign(VB1SYS6,VB1SYS6_VB1_OSD_LANE0123_SWAP));
			break;
		case 0: // resume
			vIO32WriteFldAlign(TMGR18,0x0,TMGR18_RG_LOCK_CTRL);
			vIO32WriteFldAlign(TMGR18,0x0,TMGR18_RG_LOCK_CTRL_OSD);
			vIO32WriteFldAlign(VB1SYS4,0x3210,VB1SYS4_LANE0123_SWAP);
			vIO32WriteFldAlign(VB1SYS4,0x54,VB1SYS4_LANE45_SWAP);
			vIO32WriteFldAlign(VB1SYS6,0x76,VB1SYS6_LANE67_SWAP);
			vIO32WriteFldAlign(VB1SYS6,0xba98,VB1SYS6_VB1_OSD_LANE0123_SWAP);
			Printf("[VB1][Test][mode_0]lane0123:0x%x, lane4567:0x%x, OSDlane0123:0x%x\n",
				IO32ReadFldAlign(VB1SYS4,VB1SYS4_LANE0123_SWAP),
				(IO32ReadFldAlign(VB1SYS6,VB1SYS6_LANE67_SWAP)<<8)+IO32ReadFldAlign(VB1SYS4,VB1SYS4_LANE45_SWAP),
				IO32ReadFldAlign(VB1SYS6,VB1SYS6_VB1_OSD_LANE0123_SWAP));
		default:
			break;
	}

}

void vVByOneSingleLanePadEn(UINT8 u1LaneEn)
{
	// LVDSA_EXT_EN_D11 {0:O0; 1:O1; 2:O2; 3:O3; 4:O4; 5:OCK; 6:E0; 7:E1; 8:E2; 9:E3; 10:E4; 11:ECK}
	UINT16 u2APortPadEn=0;
	switch (u1LaneEn)
	{
		case 1:  // lane0 --> AO0
			u2APortPadEn |= (1<<0);
			break;
		case 2:  // lane1 --> AO1
			u2APortPadEn |= (1<<1);
			break;
		case 3:  // lane2 --> AO2
			u2APortPadEn |= (1<<2);
			break;
		case 4:  // lane3 --> AOCK
			u2APortPadEn |= (1<<5);
			break;
		case 5:  // lane4 --> AO3
			u2APortPadEn |= (1<<3);
			break;
		case 6:  // lane5 --> AO4
			u2APortPadEn |= (1<<4);
			break;
		case 7:  // lane6 --> AE0
			u2APortPadEn |= (1<<6);
			break;
		case 8:  // lane7 --> AE1
			u2APortPadEn |= (1<<7);
			break;
		case 9:  // lane8 --> AE2
			u2APortPadEn |= (1<<8);
			break;
		case 10:  // lane9 --> AECK
			u2APortPadEn |= (1<<11);
			break;
		case 11:  // lane10 --> AE3
			u2APortPadEn |= (1<<9);
			break;
		case 12:  // lane11 --> AE4
			u2APortPadEn |= (1<<10);
			break;
		case 0:
		default:
			u2APortPadEn = 0xfff;
			break;
	}
	if (u1LaneEn>0)
		Printf("[VB1]lane%d enable!! u2APortPadEn=%x\n", u1LaneEn, u2APortPadEn);
	else
		Printf("[VB1] 12 lanes enable!!! u2APortPadEn=%x\n", u2APortPadEn);

	vIO32WriteFldAlign(REG_LVDS_TX_CFG12, u2APortPadEn, RG_LVDSA_EXT_EN_D11);
}

void vVByOneVPLLBandwidthSet(UINT8 u1Bandwidth)
{
	UINT8 u1BR=0;
	UINT8 u1BC=0;
	UINT8 u1DIVEN=0;
	UINT8 u1BIR=0;
	UINT8 u1BIC=0;
	UINT8 u1BP=0;
	
	switch (u1Bandwidth)
	{
		case 1:  // open-loop bandwidth=0.5MHz
			Printf("[VB1] VPLL_bandwidth=0.5MHz\n");
			u1BR = 0x1;
			u1BC = 0x3;
			u1DIVEN = 0x1;
			u1BIR = 0x1;
			u1BIC = 0x1;
			u1BP = 0xE;
			break;
		case 2:  // open-loop bandwidth=1MHz
			Printf("[VB1] VPLL_bandwidth=1MHz\n");
			u1BR = 0x1;
			u1BC = 0x3;
			u1DIVEN = 0x1;
			u1BIR = 0x3;
			u1BIC = 0x6;
			u1BP = 0x7;
			break;
		case 0:  // default setting
		default:
			Printf("[VB1] VPLL_bandwidth-->Default\n");
			u1BR = 0x3;
			u1BC = 0x3;
			u1DIVEN = 0x1;
			u1BIR = 0xA;
			u1BIC = 0x4;
			u1BP = 0xC;
			break;
	}
	vIO32WriteFldMulti(REG_LVDSTX_PLL_CFG3, P_Fld(u1BR, REG_LVDSTX_PLL_CFG3_VPLL_BR)|P_Fld(u1BP, REG_LVDSTX_PLL_CFG3_VPLL_BP)|P_Fld(u1BIR, REG_LVDSTX_PLL_CFG3_VPLL_BIR));
	vIO32WriteFldMulti(REG_LVDSTX_PLL_CFG2, P_Fld(u1BIC, REG_LVDSTX_PLL_CFG2_VPLL_BIC)|P_Fld(u1BC, REG_LVDSTX_PLL_CFG2_VPLL_BC)|P_Fld(u1DIVEN, REG_LVDSTX_PLL_CFG2_VPLL_DIVEN));
}

static UINT32 VByOne_Register_ReMapping_E3(UINT32 u4Register)
{

    switch (u4Register)
    {
    case MLVDS_CTRL_32: return(VB1SYS0);
        break;
    case MLVDS_CTRL_33: return(VB1SYS1);
        break;
    case MLVDS_CTRL_34: return(VB1SYS2);
        break;
	case MLVDS_CTRL_35: return(VB1SYS3);
		break;
	case MLVDS_CTRL_36: return(VB1SYS4);
		break;
	case MLVDS_CTRL_37: return(VB1SYS6);
		break;
	case MLVDS_CTRL_38: return(VB1SYS8);
		break;
	case MLVDS_CTRL_39: return(VB1ANA0);
		break;
	case MLVDS_CTRL_40:  return(VB1RES);
		break;
	case MLVDS_CTRL_41: return(VB1ANA2);
		break;
	case MLVDS_CTRL_42: return(VB1ANA3);
		break;
	case MLVDS_CTRL_43: return(VB1ANA4);
		break;
	case MLVDS_CTRL_44: return(VB1ANA5);
		break;
	case MLVDS_CTRL_45: return(VB1ANA6);
		break;
	case MLVDS_CTRL_46: return(VB1ANA7);
		break;
	case MLVDS_CTRL_47: return(VB1ANA8);
		break;
	case MLVDS_CTRL_48: return(VB1OSDSYS0);
		break;
	case MLVDS_CTRL_49: return(VB1OSDSYS1);
		break;
	case MLVDS_CTRL_50: return(VB1OSDSYS2);
		break;
	case MLVDS_CTRL_51: return(VB1OSDSYS3);
		break;
	case MLVDS_CTRL_52: return(VB1OSDSYS4);
		break;
	case MLVDS_CTRL_53: return(VB1OSDSYS6);
		break;
	case MLVDS_CTRL_54: return(VB1OSDSYS8);
		break;
	case MLVDS_CTRL_55: return(VB1OSDANA0);
		break;
	case MLVDS_CTRL_56:  return(VB1OSDRES);
		break;
	case MLVDS_CTRL_57: return(VB1OSDANA2);
		break;
	case MLVDS_CTRL_58: return(VB1OSDANA3);
		break;
	case MLVDS_CTRL_59: return(VB1OSDANA4);
		break;			
	case MLVDS_CTRL_60: return(GR0);
		break;	
	case MLVDS_CTRL_61: return(GR1);
		break;	
	case MLVDS_CTRL_62: return(GR12);
		break;	
		
    default:
        return (u4Register);		
    }
}

void VByOne_Register_Mapping_Write_E3(void)
{
	UINT32 u4Address=MLVDS_CTRL_32;
	for(u4Address=MLVDS_CTRL_32; u4Address<=MLVDS_CTRL_62; u4Address+=4)
	{
		vIO32Write4BMsk(VByOne_Register_ReMapping_E3(u4Address),u4IO32Read4B(u4Address),0xFFFFFFFF);

	}
}

void vVByOnePreEmphasisOnOff(UINT8 u1OnOff)
{
	if (!u1OnOff)
	{
		vIO32WriteFldAlign(REG_LVDS_TX_CFG14, 0xFFF, RG_LVDSA_PRE_PWD);
	}
	else
	{
		vIO32WriteFldAlign(REG_LVDS_TX_CFG14, 0x0, RG_LVDSA_PRE_PWD);
	}
}

#define LVDSAPreEmIndex 7
PRIVATE UINT8 code bLVDSAPreEm[LVDSAPreEmIndex+1] =
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
//  1mA, 1.5mA, 2mA, 2.5mA, 3mA, 3.5mA, 4mA, 4.5mA 
};
void vVByOnePreEmphasisSet(UINT8 value)
{
	value &= 0x7;  //0~7
    if (value > LVDSAPreEmIndex) value = LVDSAPreEmIndex;
    value = bLVDSAPreEm[value];
    vIO32WriteFldAlign(REG_LVDS_TX_CFG13, value, RG_LVDSA_TVO_PRE);
}

UINT8 vVByOneGetPreEmphasis(void)
{
    UINT8 value;
	value = IO32ReadFldAlign(REG_LVDS_TX_CFG13, RG_LVDSA_TVO_PRE);
	value &= 0x7;  //0~7
    return (value);
}

