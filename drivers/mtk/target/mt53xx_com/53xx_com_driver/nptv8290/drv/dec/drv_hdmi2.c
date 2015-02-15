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

/**
 * @file HDMI control driver
 *
 * @note PWR5V -> vHDMIHPDHigh()
 * @note !PWR5V -> vHDMIHPDLow()
 * @note INIT(see below) -> PWOFF -> PWR5V -> PWON -> PWON2 (TERM_ON, RX_EN, HPD_HIGH)
 * -> SCDT (reset fifo) -> AUTH
 * @note !PWR5V -> INIT (HPD_LOW, TERM_OFF, RESET, MUTE)
 * @note vVdoModeChgDone -> fgApiVideoIsVgaMode -> _rMChannel.bIs444 = SV_TURE or SV_FALSE
 *       fgApiVideoIsVgaMode -> _bHdmiMode -> _bDviTiming -> _rMChannel.bIs444 -> _bMAIN_COLOR_SYS = SV_COLOR_RGB
 *
 * @author ciwu
 */
#include "x_lint.h"
#include "x_gpio.h"

#include "general.h"
#include "util.h"
#include "hdmi_debug.h"
#include "video_def.h"
#include "hw_hdmi2.h"
#include "hw_hdmi.h"
#include "drv_hdmi2.h"
#include "x_timer.h"
#include "drv_hdmi2com.h"
#include "x_assert.h"
#include "aud_if.h"
#include "aud_drvif.h"
#include "drv_dvi.h"
#include "drv_dvi2.h"
#include "x_bim.h"
#include "hw_pdwnc.h"
#include "mute_if.h"

#define HDMI2_RES_STABLE 10
#define HDMI2_HDCP_STABLE 10
#define HDMI2_NO_SCDT_WITH_CKDT_WAIT_COUNT 3
#define HDMI2_TIMER_PERIOD  5     // ms
#define HDMI2_HPD_Period  160     // 800ms

static x_os_isr_fct _pfnHdmi2Isr = NULL; 
static HANDLE_T _hHDMI2Timer;
#ifdef CC_HDMI_OLD_NEW_SUPPORT
static HANDLE_T _hHdmi2ThreadHdl;
#endif
HdmiRxObject_t Hdmi2RxObj[HDMI_RX_PORT_MAX];

extern UINT8 u1IsHdmi2Cap;
#ifdef CC_HDMI_2_0_HDCP_BIN
extern void vHDMI20LoadHdcpKey(void);
extern BOOL fgHDMI20CheckCap(void);
#endif
extern INT32 x_os_drv_reg_isr_ex(UINT16 ui2_vec_id,
                                 x_os_isr_fct   pf_isr,
                                 x_os_isr_fct   *ppf_old_isr);
UINT8 u1GetMinHDMI20Port(void)
{
    UINT8 u1MinHDMI20Port = HDMI_RX_PORT_MAX;
    if(IS_IC_5861())
    {
        u1MinHDMI20Port = HDMI_RX_PORT_4;
    }
    else if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //5890 E3 or later
    {
        u1MinHDMI20Port = HDMI_RX_PORT_3;
    }

    return u1MinHDMI20Port;
}

UINT32 u4GetHdmi2RegBase(HdmiRxPort ePort)
{
    UINT32 u4TmpHdmiRegBase = 0x000;
	
	if(ePort < HDMI_RX_PORT_MAX && ePort > HDMI_RX_PORT_Init)
	{
		if(IS_IC_5861())
		{
			u4TmpHdmiRegBase =0;
		}
	}


    return (u4TmpHdmiRegBase);
}

UINT32 u4GetHdmi2AnalogRegBase(HdmiRxPort ePort)
{
    UINT32 u4TmpAnalogRegBase = 0x0;

	if(ePort < HDMI_RX_PORT_MAX && ePort > HDMI_RX_PORT_Init)
	{
		if(IS_IC_5861())
		{
			u4TmpAnalogRegBase = 0x2300;
		}
        else if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //5890 E3 or later
        {
            switch(ePort)
            {
                case HDMI_RX_PORT_3:
                    u4TmpAnalogRegBase = 0x4200;
                    break;
                case HDMI_RX_PORT_4:
                    u4TmpAnalogRegBase = 0x4300;
                    break;
                default:
                    break;
            }
        }
	}
	
	return u4TmpAnalogRegBase;
}

void vHDMI2SetBaseAddr(HdmiRxPort portId)
{
	if(portId < HDMI_RX_PORT_MAX && portId > HDMI_RX_PORT_Init)
	{
		Hdmi2RxObj[portId]._bHDMIRxBaseAddr = (UINT16)u4GetHdmi2RegBase(portId);
		Hdmi2RxObj[portId]._bHDMIAnaBaseAddr = (UINT16)u4GetHdmi2AnalogRegBase(portId);	
	}

}

UINT16 u2HDMI2GetDigBaseAddr(HdmiRxPort ePort)
{
	if(ePort < HDMI_RX_PORT_MAX && ePort > HDMI_RX_PORT_Init)
	{
		return Hdmi2RxObj[ePort]._bHDMIRxBaseAddr;
	}
	return 0;
}

UINT16 u2HDMI2GetAnaBaseAddr(HdmiRxPort ePort)
{
	if(ePort < HDMI_RX_PORT_MAX && ePort > HDMI_RX_PORT_Init)
	{
		return Hdmi2RxObj[ePort]._bHDMIAnaBaseAddr;
	}
	return 0;
}

static void vHDMI2Aud14ClkConfig(HdmiRxPort ePort, UINT8 u1CurrentAudFreq) //For 5890 E3 audio issue: no MUX to audio for new digital, set data path as new dig -> old dig -> audio
{
    UINT32 u4RegBase;
    UINT8 u1PreDiv, u1PostDiv;
    u4RegBase = u4GetHdmiRegBase((E_HDMI_SWITCH_NUM)ePort);
    if(BSP_GetIcVersion() == IC_VER_5890_AC) //Only workaround for 5890 E3
    {       
        if(u1CurrentAudFreq != Hdmi2RxObj[ePort]._bHDMILastAudFreq)
        {
            //Turn off clk firstly to avoid gabarge output
            vIO32WriteFldAlign(AUDRX_CTRL + u4RegBase,0,I2S_MODE);
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,0,POST_HW_SW_SEL);
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,0,CTS_HW_SW_SEL);
            //Set divider
            vIO32WriteFldAlign(APLL1 + u4RegBase,1,PRESDIVSELOVR);
            switch(u1CurrentAudFreq)
            {
                case HDMI2_AUD_FS_32K:
                    u1PreDiv = 0;
                    u1PostDiv = 0xc;
                    break;
                case HDMI2_AUD_FS_44K:
                case HDMI2_AUD_FS_48K:
                    u1PreDiv = 1;
                    u1PostDiv = 0x6;
                    break;
                case HDMI2_AUD_FS_88K:
                case HDMI2_AUD_FS_96K:    
                    u1PreDiv = 1;
                    u1PostDiv = 0x3;
                    break;
                case HDMI2_AUD_FS_176K:
                case HDMI2_AUD_FS_192K:
                    u1PreDiv = 0;
                    u1PostDiv = 0x2;
                    break;
                default: //default as 48K
                    u1PreDiv = 1;
                    u1PostDiv = 0x6;
                    break;
            }
            vIO32WriteFldAlign(APLL1 + u4RegBase,u1PreDiv,PREDIVSELOVRSEL);
            vIO32WriteFldAlign(UPLL_HVAL + u4RegBase,u1PostDiv,POST_VAL_SW);
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,1,POST_HW_SW_SEL);
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,1,CTS_HW_SW_SEL);
            //Reset WP
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,0,ACR_INIT_WP);
            vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,1,ACR_INIT_WP);
            //Turn on clk
            vIO32WriteFldAlign(AUDRX_CTRL + u4RegBase,1,I2S_MODE);
            Hdmi2RxObj[ePort]._bHDMILastAudFreq = u1CurrentAudFreq;
        }
    }
}


//Workaround for 5890 E3 Port C deep color issue
static void vHDMI2SwSetDeepColorDivider(HdmiRxPort ePort)
{
    UINT32 u4HDMI14RxBaseAddr;
    UINT8 u1CurrentDeepColor;
    UINT16 u2HDMIRxBaseAddr;
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        if(HDMI_RX_PORT_3 == ePort)
        {
            u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
            u4HDMI14RxBaseAddr = u4GetHdmiRegBase((E_HDMI_SWITCH_NUM)ePort);
            
            vIO32WriteFldAlign(N_HDMI_CTRL + u4HDMI14RxBaseAddr,1,TT0_CD_SW_SEL);
            u1CurrentDeepColor = bHDMI2ComGetDeepSta(u2HDMIRxBaseAddr);
            switch(u1CurrentDeepColor)
            {
                default:
                case HDMI_RX_BIT_DEPTH_8_BIT:
                    vIO32WriteFldAlign(N_HDMI_CTRL1 + u4HDMI14RxBaseAddr,0x4,TT88_1_CD_SW);
                    break;
                case HDMI_RX_BIT_DEPTH_10_BIT:
                    vIO32WriteFldAlign(N_HDMI_CTRL1 + u4HDMI14RxBaseAddr,0x5,TT88_1_CD_SW);
                    break;
                case HDMI_RX_BIT_DEPTH_12_BIT:
                    vIO32WriteFldAlign(N_HDMI_CTRL1 + u4HDMI14RxBaseAddr,0x6,TT88_1_CD_SW);
                    break;
                case HDMI_RX_BIT_DEPTH_16_BIT:
                    vIO32WriteFldAlign(N_HDMI_CTRL1 + u4HDMI14RxBaseAddr,0x7,TT88_1_CD_SW);
                    break;
            }
        }
    }
}

void vHDMI2Related14DigSel(HdmiRxPort ePort)
{
    UINT32 u4RegBase;
    u4RegBase = u4GetHdmiRegBase((E_HDMI_SWITCH_NUM)ePort);
    if(BSP_GetIcVersion() == IC_VER_5890_AC) //Only workaround for 5890 E3
    {
        //No audio issue
        vIO32WriteFldAlign(SYS_CTRL + u4RegBase,1,PD_ALL);
        vIO32WriteFldAlign(AUDRX_CTRL + u4RegBase,0,I2S_MODE);
        vIO32WriteFldAlign(ACR_CTRL1 + u4RegBase,0,FM_IN_VAL_SW);
        vIO32WriteFldAlign(PD_SYS + u4RegBase,1,PD_TOTAL);
        vIO32WriteFldAlign(PD_SYS + u4RegBase,1,PD_AO);
    }
    switch(ePort) //Todo:Is port A&B setting needed if HDMI2.0 is applied???
    {
        case HDMI_RX_PORT_3:
            {
                // main clk
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_MAIN_CLK_SEL0);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_MAIN_CLK_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_MAIN_CLK_SEL2);
            	//pip clk
            	vIO32WriteFldAlign(HDMI_PIP_SEL,0,REG_PIP_CLK_SEL0);
            	vIO32WriteFldAlign(HDMI_PIP_SEL,0,REG_PIP_CLK_SEL1);
            	vIO32WriteFldAlign(HDMI_PIP_SEL,1,REG_PIP_CLK_SEL2);
                // int
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_INT_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_INT_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_INT_OUT_MAVIS_SEL2);
                // acr
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_ACR_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_ACR_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_ACR_OUT_MAVIS_SEL2);
                // macro
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_REGISTER_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_REGISTER_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_REGISTER_OUT_MAVIS_SEL2);
                // aout
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_AUDIO_PATH_OUT_MAVIS_SEL2);
                // video out
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_VIDEO_PATH_OUT_MAVIS_SEL2);
                // clk
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_CLK_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_CLK_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_CLK_IN_MAVIS_SEL2);
                // audio in
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_AUDIO_PATH_IN_MAVIS_SEL2);
                // video in
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 1, REG_VIDEO_PATH_IN_MAVIS_SEL2);
            	// hdmi clk
            	vIO32WriteFldAlign(CKGEN_REG_MHL_CKCFG, 0, FLD_MHL_DPCLK_SEL);
            	vIO32WriteFldAlign(CKGEN_REG_MHL_CKCFG, 0, FLD_MHL_PCLK_SEL);
                break;
            }
        default:
        case HDMI_RX_PORT_4:
            {
                // main clk
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_MAIN_CLK_SEL0);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_MAIN_CLK_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_MAIN_CLK_SEL2);
            	//pip clk
            	vIO32WriteFldAlign(HDMI_PIP_SEL,0,REG_PIP_CLK_SEL0);
            	vIO32WriteFldAlign(HDMI_PIP_SEL,0,REG_PIP_CLK_SEL1);
            	vIO32WriteFldAlign(HDMI_PIP_SEL,0,REG_PIP_CLK_SEL2);
                // int
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_INT_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_INT_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_INT_OUT_MAVIS_SEL2);
                // acr
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_ACR_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_ACR_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_ACR_OUT_MAVIS_SEL2);
                // macro
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_REGISTER_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_REGISTER_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_REGISTER_OUT_MAVIS_SEL2);
                // aout
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_OUT_MAVIS_SEL2);
                // video out
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_OUT_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_OUT_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_OUT_MAVIS_SEL2);
                // clk
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_CLK_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_CLK_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_CLK_IN_MAVIS_SEL2);
                // audio in
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_AUDIO_PATH_IN_MAVIS_SEL2);
                // video in
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_IN_RACHEL_SEL);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_IN_MAVIS_SEL1);
                vIO32WriteFldAlign(HDMI_SEL, 0, REG_VIDEO_PATH_IN_MAVIS_SEL2);
            	// hdmi clk
            	vIO32WriteFldAlign(CKGEN_REG_MHL_CKCFG, 0, FLD_MHL_DPCLK_SEL);
            	vIO32WriteFldAlign(CKGEN_REG_MHL_CKCFG, 0, FLD_MHL_PCLK_SEL);
                break;
            }
    }
}

void vHDMI2PortSel(HdmiRxPort ePort)
{
    if(ePort < u1GetMinHDMI20Port())
    {
        return;
    }
    
    if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC))
    {
        switch(ePort)
        {
            case HDMI_RX_PORT_3:
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 1, FLD_HDMI2_R2R_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 0, FLD_HDMI2_DPCLK_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 0, FLD_HDMI2_PORTD_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 1, FLD_HDMI2_PORTC_CLK_SEL);
                vIO32WriteFldAlign(HDMI2_PORT_SEL, 0x7, RG_HDMI2_PORT_SEL);
                vIO32WriteFldAlign(PDWNC_PADCFG17, 0x7, FLD_HDMI2_DDC_SEL);
                break;
            case HDMI_RX_PORT_4:
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 1, FLD_HDMI2_R2R_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 1, FLD_HDMI2_DPCLK_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 1, FLD_HDMI2_PORTD_CLK_SEL);
                vIO32WriteFldAlign(CKGEN_REG_TOP_BACKUP5, 0, FLD_HDMI2_PORTC_CLK_SEL);
                vIO32WriteFldAlign(HDMI2_PORT_SEL, 0x0, RG_HDMI2_PORT_SEL);
                vIO32WriteFldAlign(PDWNC_PADCFG17, 0x0, FLD_HDMI2_DDC_SEL);
                break;
            default:
                break;
        }
    }
    vHDMI2Related14DigSel(ePort);
}

void vHDMI2SetStateMachine(HdmiRxPort ePort,UINT8 u1HdmiSta)
{
	Hdmi2RxObj[ePort]._bHDMIState = u1HdmiSta;
}

UINT8 bHDMI2GetStateMachine(HdmiRxPort ePort)
{
	return Hdmi2RxObj[ePort]._bHDMIState;
}
void vHDMI2StopFw(HdmiRxPort ePort)
{
    Hdmi2RxObj[ePort]._bHdmiCmd = 6;
}

void vHDMI2ResumeFw(HdmiRxPort ePort)
{
    Hdmi2RxObj[ePort]._bHdmiCmd = 0;
}

UINT8 u1HDMI2IsUnplug(HdmiRxPort ePort)
{
    return(Hdmi2RxObj[ePort]._bUnplugFlag);
}

void vHDMI2ClrUnplug(HdmiRxPort ePort)
{
    Hdmi2RxObj[ePort]._bUnplugFlag = 0;
}

void vHDMI2SetUnplug(HdmiRxPort ePort)
{
    Hdmi2RxObj[ePort]._bUnplugFlag = 1;
}

UINT8 u1HDMI2IsHpdPulse(HdmiRxPort ePort)
{
	if(ePort < u1GetMinHDMI20Port())
	{
		return u1IsHdmiHpdPulse(ePort);
	}
	else
	{
		return (!Hdmi2RxObj[ePort]._bHdmiPlug);
	}
}

void vHDMI2SetHpdPulse(HdmiRxPort ePort,UINT8 u1Val)
{
    if(ePort < u1GetMinHDMI20Port())
	{
		vHdmiSetHpdStatus(ePort,u1Val);
	}
	else
	{
		Hdmi2RxObj[ePort]._bHdmiPlug = u1Val;
	}
}

UINT8 u1HDMI2IsSyncLose(HdmiRxPort ePort)
{
	return Hdmi2RxObj[ePort]._bHDMIScdtLose;
}

void vHDMI2SetSyncLose(HdmiRxPort ePort,UINT8 u1Val)
{
	Hdmi2RxObj[ePort]._bHDMIScdtLose = u1Val;
}

UINT8 u1HDMI2GetDviModeCnt(HdmiRxPort ePort)
{
	return Hdmi2RxObj[ePort]._bHDMIDviModeCnt;
}

void vHDMI2SetDviModeCnt(HdmiRxPort ePort,UINT8 u1Val)
{
	Hdmi2RxObj[ePort]._bHDMIDviModeCnt = u1Val;
}
BOOL fgHDMI2Get420Mode(HdmiRxPort ePort)
{
    UINT16 u2HDMIRxBaseAddr;
    u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    return fgHDMI2ComGet420Mode(u2HDMIRxBaseAddr);
}

void vHDMI2SetSplit(HdmiRxPort eHdmiPort, UINT8 u1En)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	vHDMI2ComSetSplit(u2HDMIRxBaseAddr,u1En);
}
BOOL fgHDMI2IsPixelRepeate(HdmiRxPort ePort)
{

	UINT16 u2HDMIRxBaseAddr;
	BOOL bIsPP = FALSE;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	if(fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr))
	{
		if(IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) == 0x82)
		{
			if((IO32ReadFldAlign(HDMI2_AVIRX_DBYTE3 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_DBYTE5) & 0xf))
			{
				bIsPP = TRUE;
			}
		}
	}

	return bIsPP;
	
}

/**
 * @brief 	HDMI input is RGB /Ycbcr
 * @param	None
 * @retval	1=RGB ,0 =YCBCR
 */
UINT8 bHDMI2InputType(HdmiRxPort ePort)
{
    UINT8 bReadData;
	UINT8 ret;

	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	if(fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr))
	{
		if(IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82)
		{
			ret=1;
		}
		else
		{
			bReadData = (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 2) & 0x60);
			if(bReadData == 0x00)
			{
				ret=1;
			}
			else
			{
				ret=0;
			}
		}
	}
	else
	{
		ret=1;
	}

    return ret;
}

/**
 * @brief 	HDMI Clocrimetry
 * @param	None
 * @retval	1=ITU601 ,0 =ITU709
 */
UINT8 bHDMI2Clocrimetry(HdmiRxPort ePort)
{
    UINT8 bReadData;
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    bReadData = u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 3);

    if((bReadData&0xc0) == 0x40)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
full range: 0 ~ 255.
limited range: 16 ~ 235.
   00: Default, depend on video format.
         PC timing: full range.
         Video timing: limited range.
   01: limited range.
   10: full range.
   11: Reserved.
*/
extern UINT8 _bHDMIRange;
UINT8 bHDMI2RgbRange(HdmiRxPort ePort)
{
    UINT8 bMode;
	UINT16 u2HDMIRxBaseAddr;
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((ePort < u1GetMinHDMI20Port()) || !fgHDMI2GetDuleMode(ePort))
#else
	if(ePort < u1GetMinHDMI20Port())
#endif
	{
		return bHDMIRgbRange();
	}

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
		_bHDMIRange = SV_HDMI_RANGE_FORCE_AUTO;
        return SV_HDMI_RANGE_FORCE_AUTO;
	}
    else
    {
        bMode = bGetHDMIRangeMode();
        _bHDMIRange = (u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HDMIRxBaseAddr) & 0x0c) >> 2;

        switch(bMode)
        {
            default:
            case SV_HDMI_RANGE_FORCE_AUTO:
                return _bHDMIRange;
            case SV_HDMI_RANGE_FORCE_LIMIT:
                return SV_HDMI_RANGE_FORCE_LIMIT;

            case SV_HDMI_RANGE_FORCE_FULL:
                return SV_HDMI_RANGE_FORCE_FULL;
        }
    }
}

BOOL fgHDMI2GetDuleMode(HdmiRxPort ePort)
{
	if(ePort >= u1GetMinHDMI20Port())
	{
		if(fgHDMI2Get420Mode(ePort) || Hdmi2RxObj[ePort]._ScdcInfo.u1ClkRatio)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void vHDMI2EQCIRCUITCTRL(HdmiRxPort ePort,BOOL bOnOff)
{
	 UINT16 u2HDMIAnaBaseAddr;

	 u2HDMIAnaBaseAddr = u2HDMI2GetAnaBaseAddr(ePort);

	 if(bOnOff)
	 {
	 	/* turn on TMDS*/
		
		vHDMI2ComAnaTMDSCtrl(u2HDMIAnaBaseAddr,TRUE);
	 }
		
}


static void _HDMI2TimerHandle(HANDLE_T ptTmHandle, VOID* pvTag)
{
	/*ADD----Control HPD high/Low period*/
	HdmiRxPort eHdmiPort=0;
	
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	/**Note----Becasue EEPROM or flash may not ready in vHDMI2Init state**/
 	/*ADD----Load HDCP2.2 key and 1.4 key from EEPROM or Flash and push into internal SRAM*/
	/*ADD----Load HDCP2.2 ROMcode form  Flash and push into internal SRAM*/
	/*ADDwithSampleCode----Stop and resume FW*/

	for(eHdmiPort = u1GetMinHDMI20Port();eHdmiPort <HDMI_RX_PORT_MAX;eHdmiPort++)
	{
		if(fgHDMI2ComGetScramblingBySCDC(u2HDMIRxBaseAddr))
		{
			if(IS_IC_5861_ES1())
			{
				vHDMI2ComSetScrambling(u2HDMIRxBaseAddr,1);
			}
			Hdmi2RxObj[eHdmiPort]._ScdcInfo.u1ScramblingOn =1;
		}
		else
		{
			if(IS_IC_5861_ES1())
			{
				vHDMI2ComSetScrambling(u2HDMIRxBaseAddr,0);
			}
			Hdmi2RxObj[eHdmiPort]._ScdcInfo.u1ScramblingOn =0;
		}

		if(fgHDMI2ComGetTmdsClkRatio(u2HDMIRxBaseAddr))
		{
			Hdmi2RxObj[eHdmiPort]._ScdcInfo.u1ClkRatio=1;
			Hdmi2RxObj[eHdmiPort]._HdmiMode = HDMI_RX_MODE_HDMI2X;
			//vHDMI2ComSetSplit(u2HDMIRxBaseAddr,1);
		}
		else
		{
			Hdmi2RxObj[eHdmiPort]._ScdcInfo.u1ClkRatio=0;
			Hdmi2RxObj[eHdmiPort]._HdmiMode = HDMI_RX_MODE_HDMI1X;
		}
		
		if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr))
		{
			Hdmi2RxObj[eHdmiPort]._HdmiMode = HDMI_RX_MODE_DVI;
		}
		if(Hdmi2RxObj[eHdmiPort]._bHDMIHpdTimerCnt)
		{
			Hdmi2RxObj[eHdmiPort]._bHDMIHpdTimerCnt--;
			if(Hdmi2RxObj[eHdmiPort]._bHDMIHpdTimerCnt == 94)
			{
				vHDMI2TMDSCTRL(eHdmiPort,TRUE);
				vHDMI2DDCBusCTRL(eHdmiPort,TRUE);
			}

			if(Hdmi2RxObj[eHdmiPort]._bHDMIHpdTimerCnt == 0)
			{
				vHDMIHPDHigh(eHdmiPort);
				//Hdmi2RxObj[eHdmiPort]._bHdmiPlug =1;
				vHDMI2SetHpdPulse(eHdmiPort,1);
			}
		}

		
		
	}
	
}



void vHDMI2SetHpdLowTimer(HdmiRxPort ePort,UINT8 u1TimerCnt)
{
	Hdmi2RxObj[ePort]._bHDMIHpdTimerCnt = u1TimerCnt;
}

void vHDMI2TMDSCTRL(HdmiRxPort ePort,BOOL bOnOff)
{
  	  UINT16 u2HDMIAnaBaseAddr;

	  u2HDMIAnaBaseAddr = u2HDMI2GetAnaBaseAddr(ePort);

	  if(bOnOff)
	  {
	  	 vIO32Write1B(PD_SYS_2, 0xff);
	  	 vHDMI2ComAnaTermCtrl(u2HDMIAnaBaseAddr,TRUE);
	  }
	  else
	  {
		/* turn off TMDS*/
		vIO32Write1B(PD_SYS_2, 0xcf);
        vHDMI2ComAnaTermCtrl(u2HDMIAnaBaseAddr,FALSE);
		vHDMI2ComAnaTMDSCtrl(u2HDMIAnaBaseAddr,FALSE);
	  }
}


void vHDMI2DDCBusCTRL(HdmiRxPort ePort,BOOL bOnOff)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	
	vHDMI2ComSetHdcpDDC(u2HDMIRxBaseAddr,bOnOff);
}

void vHDMI2HwInit(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr,u2HDMIRXAnaBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    u2HDMIRXAnaBaseAddr = u2HDMI2GetAnaBaseAddr(ePort);

	vHDMI2ComAnaInit(u2HDMIRXAnaBaseAddr);
	vHDMI2ComRxInit(u2HDMIRxBaseAddr);
	//select apll status
	vHDMI2AudStatusSet(u2HDMIRxBaseAddr,1);
}

void vHDMI2MuteAudio(HdmiRxPort ePort,BOOL bEn)
{
	UINT16 u2HDMIRxBaseAddr;
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((ePort != (HdmiRxPort)eActiveHdmiPort) || (!fgHDMI2GetDuleMode(ePort)))
#else
	if((ePort != (HdmiRxPort)eActiveHdmiPort))
#endif
	{
		return;
	}
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	
	vHDMI2ComAudMuteSet(u2HDMIRxBaseAddr,bEn);
	AUD_HdmiDrvModeMute(AUD_DEC_MAIN, bEn);
}

/**
 * @brief		Enable HDMI Video Output
 * @param	None
 * @retval	None
 */
void vHDMI2AudioOutOn(void)
{

}
/**
 * @brief 	HDMI Audio Config (audio initial)
 * @param	None
 * @retval	None
 */
PRIVATE void vHDMI2AudConfig(HdmiRxPort ePort)
{
#if 0
	// Load audio configuration
    if(!AUD_GetAinCfg(AUD_STREAM_FROM_HDMI, &_rAudCfg))
    {
        // If audio input not initialed, use the following setting
        _rAudCfg.eStrSrc =      AUD_STREAM_FROM_HDMI;
        _rAudCfg.eFormat =      FORMAT_RJ;
        _rAudCfg.eBits =        DAC_24_BIT;
        _rAudCfg.eCycle =       LRCK_CYC_32;
        _rAudCfg.eSampleFreq =  MCLK_256FS;
        _rAudCfg.fgDataInvert = FALSE;
        _rAudCfg.fgLRInvert =   FALSE;
    }
#endif
  //  vHDMI2AudioOutOn();
  	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComAudCfg(u2HDMIRxBaseAddr);
	return;
}
void vHDMI2AudIntClear(void)
{

}

BOOL fgHDMI2GetHdmiMode(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	
	return fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr);
}

void vHDMI2VideoMute(HdmiRxPort ePort,UINT8 u1En)
{
	
	UINT16 u2HDMIRxBaseAddr;
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((ePort != (HdmiRxPort)eActiveHdmiPort) || (!fgHDMI2GetDuleMode(ePort)))
#else
	if((ePort != (HdmiRxPort)eActiveHdmiPort))
#endif
	{
		return;
	}
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	vHDMI2ComVideMuteSet(u2HDMIRxBaseAddr,u1En);
}
void vHDMI2AudMute(HdmiRxPort ePort,UINT8 u1En)
{
	UINT16 u2HDMIRxBaseAddr;
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((ePort != (HdmiRxPort)eActiveHdmiPort) || (!fgHDMI2GetDuleMode(ePort)))
#else
	if((ePort != (HdmiRxPort)eActiveHdmiPort))
#endif
	{
		return;
	}
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComAudMuteSet(u2HDMIRxBaseAddr,u1En);
}

void vHDMI2AudAacSet(HdmiRxPort ePort,UINT8 u1En)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComAacEn(u2HDMIRxBaseAddr,u1En);
}

BOOL fgHDMI2GetAudPam(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	return RegReadFldAlign(HDMI2_ACR_CONTROL5 + u2HDMIRxBaseAddr, RG_HDMI2_SDINOVR);
}

void vHDMI2SetAudPam(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vIO32WriteFldAlign(HDMI2_ACR_CONTROL5 + u2HDMIRxBaseAddr,0,RG_HDMI2_SDINOVR);
}

BOOL fgHDMI2AudFifoFault(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	if(bHDMI2ComGetAudFifo(u2HDMIRxBaseAddr) & 0x1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	
}

BOOL fgHDMI2GetNewPacket(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	return RegReadFldAlign(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr, RG_HDMI2_REG_INTR2_STAT1);
}

void vHDMI2ClearAudFifoFault(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);

	vIO32WriteFldAlign(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr,1,RG_HDMI2_REG_INTR4_STAT0);

	vIO32WriteFldAlign(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr,1,RG_HDMI2_REG_INTR4_STAT1);
	
}

void vHDMI2ClearNewPacket(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
		
	vIO32WriteFldAlign(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr,1,RG_HDMI2_REG_INTR2_STAT1);
}

BOOL fgHDMI2GetAcrStatus(HdmiRxPort ePort)
{
	
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	return RegReadFldAlign(HDMI2_TOP_MISC + u2HDMIRxBaseAddr, RG_HDMI2_ACR_PLL_LOCK);
}


void vHDMI2APLL2Reset(HdmiRxPort ePort)
{
    UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    if(!fgIsExtSrcHDMI())
    {
        return;
    }
	vHDMI2ComAudioSigmaDeltaSetting(u2HDMIRxBaseAddr,0, 0, 0);
}

void vHDMI2FifoReset(HdmiRxPort ePort)
{
    UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    vHDMI2ComFifoRst(u2HDMIRxBaseAddr);
}

void vHDMI2AcrReset(HdmiRxPort ePort)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComAcrRst(u2HDMIRxBaseAddr);
}
void vHDMI2AudStatusSet(HdmiRxPort ePort,UINT8 u1On)
{
	UINT16 u2HDMIRxBaseAddr;

	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComAudStatusSel(u2HDMIRxBaseAddr,u1On);
}

extern BOOL _fgHdmiAudInit;
extern UINT8 u1ACR_DPLL_LOCK;
extern UINT8 u1AudFifoFault;
extern UINT8 u1AudPacket;
extern UINT8 _bHDMIAudFIFOflag;
extern E_HDMI_SWITCH_NUM eActivePort;
extern UINT8 _bHdmiAudFreq;
#define AUDPACKET_INVALID 0
#define AUDPACKET_1 1
#define AUDPACKET_MAX 200
#define AUDPACKET_GOOD 201

static UINT8 u1AudWorkAround=0;
void vHDMI2AudErrorHandler(HdmiRxPort ePort)
{
	if(!fgIsExtSrcHDMI())
	{
		return;
	}
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((ePort != (HdmiRxPort)eActiveHdmiPort) || !fgHDMI2GetDuleMode((HdmiRxPort)eActiveHdmiPort))
#else
	if(ePort != (HdmiRxPort)eActiveHdmiPort)
#endif
	{
		return;
	}

	if(_fgHdmiAudInit == FALSE)
	{
		_fgHdmiAudInit = TRUE;
		vHDMI2AudConfig(ePort);
		LOG(0,"\n>>>>>>HDMI Audio Config------>\n");
	}

	if((!fgHDMI2GetHdmiMode(ePort)) || (bHDMI2GetStateMachine(ePort)!= HDMI_STABLE_STATE) || fgDvi2IsModeChg(SV_VP_MAIN))
	{
		// mute audio
		vHDMI2AudMute(ePort,1);
		vHDMI2AudAacSet(ePort,0);
		_bHDMIAudFIFOflag = 1;
		u1AudFifoFault = 0;
		u1AudPacket = AUDPACKET_INVALID;
		u1AudWorkAround =0;
	}
	else
	{
		
		if(fgHDMI2GetAudPam(ePort))
		{
			vHDMI2SetAudPam(ePort);
		}

		if(fgHDMI2AudFifoFault(ePort) || (ePort != (HdmiRxPort)eActivePort))
		{
			eActivePort = ePort;
			u1AudFifoFault ++;
			// back to integer mode

			_bHDMIAudFIFOflag = 1;

			//##2 check for audio packet
			if(fgHDMI2GetNewPacket(ePort))
			{
				vHDMI2ClearNewPacket(ePort);

				if(u1AudPacket == AUDPACKET_INVALID || u1AudPacket == AUDPACKET_GOOD)
				{
					u1AudPacket = AUDPACKET_1;
				}
				else if(u1AudPacket < AUDPACKET_MAX)
				{
					u1AudPacket++;
				}

				if(u1AudPacket > AUDPACKET_1 && fgHDMI2GetAcrStatus(ePort))
				{
					u1ACR_DPLL_LOCK++;
				}
				else
				{
					u1ACR_DPLL_LOCK = 0;
				}

				//##3 check for DDS LOCK
				if(u1ACR_DPLL_LOCK > 1)    // no clock: 243MHz, normal 270 / 293 MHz.
				{
					//##4 APLL2 RST

					u1AudWorkAround =0;
					//##5.1 AAC off
					vHDMI2AudAacSet(ePort,0);

					//##5.2 audio acr RST
					//vHDMI2AcrReset(ePort);
					//vUtDelay2us(500);
					//##5.3 audio fifo RST
					vHDMI2FifoReset(ePort);
					vUtDelay2us(500);

					// Clear interrupt
					vHDMI2ClearAudFifoFault(ePort);
					//vIO32Write4B(INTR_STATE1 + u4ActiveHdmiRegBase, Fld2Msk32(INTR5_AUDIO_MUTE));
				}
			}
			else
			{
				u1AudPacket = AUDPACKET_INVALID;
			}
		}
		else
		{
			// normal case
			_bHDMIAudFIFOflag = 0;
			u1AudFifoFault = 0;
			if(u1AudWorkAround == 0)
			{
				u1AudWorkAround =1;
				vHDMI2FifoReset(ePort);
				vUtDelay2us(500);

				// Clear interrupt
				vHDMI2ClearAudFifoFault(ePort);
			}
			if(u1AudPacket != AUDPACKET_GOOD)
			{
				u1AudPacket = AUDPACKET_GOOD;
			}

			vHDMI2AudMute(ePort,0);
			vHDMI2AudAacSet(ePort,1);

		}
	}

	if(_bHDMIAudFIFOflag == 1)
	{
	    Hdmi2RxObj[ePort]._bHDMILastAudFreq = HDMI2_AUD_FS_UNKNOWN;
		_bHdmiAudFreq = HDMI2_AUD_FS_48K;//AUD_FS_48K;   // default: 48k
	}
	else
	{
		_bHdmiAudFreq = bHDMI2AudSampleRateCal(u2HDMI2GetDigBaseAddr(ePort));
        vHDMI2Aud14ClkConfig(ePort,_bHdmiAudFreq);
		vHDMI2MuteAudio(ePort,FALSE);////active port????
	}

}
UINT8 u1HDMI2GetAudioSamplingFreq(void)
{
    return (_bHdmiAudFreq);
}

UINT8 bHDMI2AudStatus(void)
{
    return _bHDMIAudFIFOflag;
}

void vHDMI2RxIntEn(HdmiRxPort ePort,UINT8 u1En)
{
	UINT16 u2HDMIRxBaseAddr;
	
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
	
	vIO32WriteFldAlign(HDMI2_RX_INTR1_MASK1 + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR3_MASK0);
	vIO32WriteFldAlign(HDMI2_RX_INTR1_MASK1 + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR3_MASK1);
	vIO32WriteFldAlign(HDMI2_RX_INTR1_MASK1 + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR3_MASK2);
	vIO32WriteFldAlign(HDMI2_RX_INTR1_MASK1 + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR3_MASK3);
	vIO32WriteFldAlign(HDMI2_RX_INTR1_MASK1 + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR3_MASK4);
	vIO32WriteFldAlign(HDMI2_RX_INTR5_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR5_MASK6);
	vIO32WriteFldAlign(HDMI2_RX_INTR5_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR6_MASK2);
	vIO32WriteFldAlign(HDMI2_RX_INTR5_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR7_MASK6);

	vIO32WriteFldAlign(HDMI2_RX_INTR9_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR9_MASK2);
	vIO32WriteFldAlign(HDMI2_RX_INTR9_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR9_MASK3);
	vIO32WriteFldAlign(HDMI2_RX_INTR9_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR9_MASK4);
	vIO32WriteFldAlign(HDMI2_RX_INTR9_MASK + u2HDMIRxBaseAddr,u1En,RG_HDMI2_REG_INTR10_MASK1);
}
void vHDMI2RxIsr(void)
{
	UINT16 u2HDMIRxBaseAddr;
	UINT32 u4IntSta,u4IntSta1,u4IntSta2;
	UINT8 activePort;
	
	activePort = fgIsMainDVI() ? eActiveHdmiPort : eActiveHdmiPort1;
	
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr((HdmiRxPort)activePort);

	u4IntSta = u4IO32Read4B(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr);
	u4IntSta1 = u4IO32Read4B(HDMI2_RX_INTR5 + u2HDMIRxBaseAddr);
	u4IntSta2 = u4IO32Read4B(HDMI2_RX_INTR9 + u2HDMIRxBaseAddr);
	
	if(u4IntSta & HDMI_RX_NEW_AVI)
	{
		Hdmi2RxObj[activePort]._AviInfo.u1InfoID = HDMI_INFO_FRAME_ID_AVI;
		fgHDMI2ComGetInfoframe((UINT32)(&Hdmi2RxObj[activePort]),HDMI_INFO_FRAME_ID_AVI);
	}

	if(u4IntSta & HDMI_RX_NEW_SP)
	{
		Hdmi2RxObj[activePort]._AviInfo.u1InfoID = HDMI_INFO_FRAME_ID_SPD;
		fgHDMI2ComGetInfoframe((UINT32)(&Hdmi2RxObj[activePort]),HDMI_INFO_FRAME_ID_SPD);
	}
	
	if(u4IntSta & HDMI_RX_NEW_AUD)
	{
		Hdmi2RxObj[activePort]._AviInfo.u1InfoID = HDMI_INFO_FRAME_ID_AUD;
		fgHDMI2ComGetInfoframe((UINT32)(&Hdmi2RxObj[activePort]),HDMI_INFO_FRAME_ID_AUD);
	}
	
	if(u4IntSta & HDMI_RX_NEW_MPEG)
	{
		//Printf("_____new MPEG int__________\n");
	}
	
	if(u4IntSta & HDMI_RX_NEW_UNREC)
	{
		//Printf("_____new UNREC int__________\n");
	}


	if(u4IntSta1 & HDMI_RX_AAC_MUTE)
	{
		Printf("_____HDMI_RX_AAC_MUTE__________\n");
	}
	
	if(u4IntSta1 & HDMI_RX_NEW_ACP)
	{
		//Printf("_____new ACP int__________\n");
	}
	
	if(u4IntSta1 & HDMI_RX_NEW_VSIF)
	{
		Hdmi2RxObj[activePort]._AviInfo.u1InfoID = HDMI_INFO_FRAME_ID_VSI;
		fgHDMI2ComGetInfoframe((UINT32)(&Hdmi2RxObj[activePort]),HDMI_INFO_FRAME_ID_VSI);
	}

	if(u4IntSta1 & HDMI_RX_NEW_ISRC1)
	{
		//Printf("_____new ISRC1 int__________\n");
	}
	
	if(u4IntSta1 & HDMI_RX_NEW_ISRC2)
	{
		//Printf("_____new ISRC2 int__________\n");
	}
	
	if(u4IntSta1 & HDMI_RX_NEW_GCP)
	{
		//Printf("_____new GCP int__________\n");
	}
	
	if(u4IntSta1 & HDMI_RX_NEW_HF_VSIF)
	{
		//Printf("_____new HF-VSIF int__________\n");
	}

	vIO32Write4B(HDMI2_RX_INTR1 + u2HDMIRxBaseAddr,0xffffffff);
	vIO32Write4B(HDMI2_RX_INTR5 + u2HDMIRxBaseAddr,0xffffffff);
	vIO32Write4B(HDMI2_RX_INTR9	+ u2HDMIRxBaseAddr,0xffffffff);
}
void _vHDMI2Isr(UINT16 u2Vector)
{	
    if(u2Vector == VECTOR_HDMIRX_20)
    {    
    	vHDMI2RxIsr();
		VERIFY(BIM_ClearIrq(VECTOR_HDMIRX_20));
    }
}

#ifdef CC_HDMI_OLD_NEW_SUPPORT
UINT8 u1Tmds25Cnt=0;
static void vHDMI2Thread(void * pvArgs)
{
	HdmiRxPort eHdmiPort=0;
	
	UINT16 u2HDMIRxBaseAddr;

	UNUSED(pvArgs);
	while(1)
	{
		x_thread_delay(1);

		for(eHdmiPort =u1GetMinHDMI20Port();eHdmiPort <HDMI_RX_PORT_MAX;eHdmiPort++)
		{
			u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
			
			if(fgHDMI2ComGetCKDT(u2HDMIRxBaseAddr))
			{
				if(bHDMI2ComGetClkRateForDig(u2HDMIRxBaseAddr)==0) 				
				{
					u1Tmds25Cnt ++;
				    if(u1Tmds25Cnt == 4)
					{
				   vHDMI2ComDdcSel(u2HDMIRxBaseAddr,0); 
				   vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,0);
				   vHDMI2ComSingleModeClk(u2HDMIRxBaseAddr);
						LOG(0,"set to old digital@@@@@@@@@\n");
					}
				}
				else if(fgHDMI2GetDuleMode(eHdmiPort))
				{
					u1Tmds25Cnt = 0;
					vHDMI2ComDdcSel(u2HDMIRxBaseAddr,1); 
					vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,1);
					LOG(0,"set to new digital@@@@@@@@@\n");
				}
			}

		}

	}
	
}
#endif
void vHDMI2Init(void)
{
	/*ADD----Reset All Global variable to zero*/
	HdmiRxPort ePort=0;

	for(ePort=u1GetMinHDMI20Port();ePort< HDMI_RX_PORT_MAX;ePort++)
	{
		x_memset(&Hdmi2RxObj[ePort],0,sizeof(HdmiRxObject_t));
        Hdmi2RxObj[ePort]._bHDMILastAudFreq = HDMI2_AUD_FS_UNKNOWN; //Init as Fs unknown
		vHDMI2SetBaseAddr(ePort);
		vHDMI2HwInit(ePort);
		vHDMI2TMDSCTRL(ePort,FALSE);
		vHDMI2DDCBusCTRL(ePort,FALSE);
		vHDMI2ComDdcSel(u2HDMI2GetDigBaseAddr(ePort),1);
		vHDMIHPDLow(ePort);
		vHDMI2RxIntEn(ePort,0);
		vHDMI2SetStateMachine(ePort,HDMI_NOSIGNAL_STATE);
	    
	}
	/*ADD----Load Customerize variable */
	/*ADD----All analog inital setting*/
	/*ADD----Set clock source*/
	/*ADD----All Digital inital setting*/
	/*ADD----All audio inital setting*/
	/*ADD----enable DDC(SCDC) deglitch and Schmitt trigger*/
	/*ADD----Set HDMI inital port and set HPD low and disable TMDS termination*/
	/*Pending----If support switch then set HDMI switch inital setting*/
	/*ADD----Pullup CEC pin*/
	/*Move----move vCbusInit to vApiHwInit and following by vHDMI2Init*/
	/*Move----move vW2dR2rInit to vApiHwInit and following by vCbusInit*/
	/*ADDwithSampleCode----Create HDMI2 Timer*/
	VERIFY(x_timer_create(&_hHDMI2Timer) == OSR_OK);
	VERIFY(x_timer_start(_hHDMI2Timer,HDMI2_TIMER_PERIOD,X_TIMER_FLAG_REPEAT,_HDMI2TimerHandle,NULL) == OSR_OK);
	//
	VERIFY(x_os_drv_reg_isr_ex(VECTOR_HDMIRX_20, _vHDMI2Isr, &_pfnHdmi2Isr) == OSR_OK);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	VERIFY(x_thread_create(&_hHdmi2ThreadHdl,"Hdmi2Thread", 2048,  63, vHDMI2Thread, 0, NULL) == OSR_OK);
#endif
	
}

void vHDMI2SaveTmdsClkRate(HdmiRxPort ePort, UINT8 u1Val)
{
    if(ePort < HDMI_RX_PORT_MAX&& ePort > HDMI_RX_PORT_Init)
    {
        Hdmi2RxObj[ePort]._TmdsClkRate = u1Val;
    }
}
UINT8 u1HDMI2GetTmdsClkRate(HdmiRxPort ePort)
{
    UINT8 u1Val = HDMI_TMDS_CLOCK_UNKNOW;

    if(ePort < HDMI_RX_PORT_MAX && ePort > HDMI_RX_PORT_Init)
    {
        u1Val = Hdmi2RxObj[ePort]._TmdsClkRate;
    }

    return (u1Val);
}

static void vHDMI2PhySetting(HdmiRxPort ePort)
{
	BOOL b40xMode;
	UINT16 u2HDMIRxBaseAddr,u2HDMIRXAnaBaseAddr;
    HdmiTmdsClkRate_t eBand = HDMI_TMDS_CLOCK_UNKNOW;
   
    u2HDMIRxBaseAddr = Hdmi2RxObj[ePort]._bHDMIRxBaseAddr;
    u2HDMIRXAnaBaseAddr = Hdmi2RxObj[ePort]._bHDMIAnaBaseAddr;
    eBand = bHDMI2ComGetClkRate(u2HDMIRxBaseAddr);
    if(eBand == HDMI_TMDS_CLOCK_UNKNOW)
	{
		return;
	}
    b40xMode = fgHDMI2ComGetTmdsClkRatio(u2HDMIRxBaseAddr);//fgHDMI2GetDuleMode(ePort);
	
	vHDMI2ComAnaSetting(u2HDMIRXAnaBaseAddr,eBand,b40xMode);
	vUtDelay2us(200); //200us
    vHDMI2ComDigPhyRst1(u2HDMIRxBaseAddr);
	vHDMI2ComDeepRst1(u2HDMIRxBaseAddr);
	vHDMI2ComSplitRst(u2HDMIRxBaseAddr);
	Hdmi2RxObj[ePort]._bNoScdtWithCkdtCount = 0;

    return;
}

static void vHDMI2CheckBandChange(HdmiRxPort ePort)
{
    UINT16 u2HDMIRxBaseAddr;
    HdmiTmdsClkRate_t eBand = HDMI_TMDS_CLOCK_UNKNOW;
    u2HDMIRxBaseAddr = Hdmi2RxObj[ePort]._bHDMIRxBaseAddr;
    eBand = bHDMI2ComGetClkRate(u2HDMIRxBaseAddr);
    if(eBand == HDMI_TMDS_CLOCK_UNKNOW)
	{
		return;
	}
    if(u1HDMI2GetTmdsClkRate(ePort) != eBand)
    {
        LOG(0,"eBand=%d\n",eBand);
        vHDMI2SaveTmdsClkRate(ePort, eBand);
        vHDMI2PhySetting(ePort);
    }

    return;
}

void vHDMI2HdcpStableMonitor(HdmiRxPort ePort)
{
    UINT8 u1Buf1 = 0;
	UINT8 u1Buf2 = 0;
    UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);
    u1Buf1 = bHDMI2ComGetHdcp1Status(u4TmpHdmiResBase); //00: No HDCP; 01: Authenticating; 11: AUTH Done
    u1Buf2 = bHDMI2ComGetHdcp2Status(u4TmpHdmiResBase);

    if(u1Buf1 == 0x5)
    {
        if(Hdmi2RxObj[ePort]._bHDMIHdcp1xGood < 255)
        {
            Hdmi2RxObj[ePort]._bHDMIHdcp1xGood++;
        }
    }
    else
    {
        Hdmi2RxObj[ePort]._bHDMIHdcp1xGood = 0;
    }

	if(u1Buf2 == 0x1)
	{
		if(Hdmi2RxObj[ePort]._bHDMIHdcp2xGood < 255)
        {
            Hdmi2RxObj[ePort]._bHDMIHdcp2xGood++;
        }
    }
    else
    {
        Hdmi2RxObj[ePort]._bHDMIHdcp2xGood = 0;
    }
	
}

UINT8 u1IsHDMI2HdcpStable(HdmiRxPort ePort)
{
    return (((Hdmi2RxObj[ePort]._bHDMIHdcp1xGood > HDMI2_HDCP_STABLE) ||(Hdmi2RxObj[ePort]._bHDMIHdcp2xGood > HDMI2_HDCP_STABLE)) ? 1 : 0);
}


void vHDMI2SetRes(HdmiRxPort ePort)
{
	UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);
	
	Hdmi2RxObj[ePort]._ResInfo._u4HTotal= dwHDMI2ComHTotal(u4TmpHdmiResBase);
	Hdmi2RxObj[ePort]._ResInfo._u4VTotal= dwHDMI2ComVTotal(u4TmpHdmiResBase);
	Hdmi2RxObj[ePort]._ResInfo._u4Width= dwHDMI2ComHActive(u4TmpHdmiResBase);
	Hdmi2RxObj[ePort]._ResInfo._u4Height= dwHDMI2ComVActive(u4TmpHdmiResBase);
}
UINT8 u1IsHDMI2ResStable(HdmiRxPort ePort)
{
    UINT8 u1Ret = 0;
    UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);

    if((dwHDMI2ComHTotal(u4TmpHdmiResBase) > Hdmi2RxObj[ePort]._ResInfo._u4HTotal - 3) 
		&& (dwHDMI2ComHTotal(u4TmpHdmiResBase) < Hdmi2RxObj[ePort]._ResInfo._u4HTotal + 3)
		&&(dwHDMI2ComVTotal(u4TmpHdmiResBase) > Hdmi2RxObj[ePort]._ResInfo._u4VTotal - 2)
		&& (dwHDMI2ComVTotal(u4TmpHdmiResBase) < Hdmi2RxObj[ePort]._ResInfo._u4VTotal + 2)
	   )
    {
        u1Ret = 1;
    }
	else
	{
		vHDMI2SetRes(ePort);
	}

    return (u1Ret);
}

void vHDMI2ResStableMonitor(HdmiRxPort ePort)
{
    if(u1IsHDMI2ResStable(ePort))
    {
    	Hdmi2RxObj[ePort]._bHDMIResUnStableCnt=0;
        if(Hdmi2RxObj[ePort]._bHDMIResStableCnt < 255)
        {
            Hdmi2RxObj[ePort]._bHDMIResStableCnt += 1;
        }
    }
    else
    {
        Hdmi2RxObj[ePort]._bHDMIResStableCnt = 0;
		if(Hdmi2RxObj[ePort]._bHDMIResUnStableCnt < 255)
		{
			Hdmi2RxObj[ePort]._bHDMIResUnStableCnt +=1;
		}
    }
}

UINT8 u1IsHDMI2Stable(HdmiRxPort ePort)
{
    return ((Hdmi2RxObj[ePort]._bHDMIResStableCnt > HDMI2_RES_STABLE) ? 1:0);
}

BOOL fgHDMI2ClkChange(HdmiRxPort ePort)
{
	UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);
	
	return (BOOL)IO32ReadFldAlign(HDMI2_RX_INTR1 + u4TmpHdmiResBase,RG_HDMI2_REG_INTR2_STAT0);
}

void vHDMI2ClearClkChange(HdmiRxPort ePort)
{
	UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);
	
	vIO32WriteFldAlign(HDMI2_RX_INTR1 + u4TmpHdmiResBase,1,RG_HDMI2_REG_INTR2_STAT0);
}

void vHDMI2PhyReset(HdmiRxPort ePort, UINT8 resettype)
{
    UINT16 u2HDMIRxBaseAddr,u2HDMIRXAnaBaseAddr;
	HDMI2ANA_LEQ_ITEM tLEQItem;
	
    u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
    u2HDMIRXAnaBaseAddr = u2HDMI2GetAnaBaseAddr(ePort);

    if(resettype == HDMI_RST_ALL)
    {
		tLEQItem.fgAvg = FALSE;
	    tLEQItem.fgLeak = TRUE;
	    tLEQItem.fgLeak_Decrease = FALSE;
	    tLEQItem.fgLeak_Increase = FALSE;
	    tLEQItem.fgLEQ_Reduce = FALSE;

		if(resettype == HDMI_RST_ALL)
		{
			Printf("HDMI_RST_ALL_________\n");
			vHDMI2ComAnaEQMacroEnable(u2HDMIRXAnaBaseAddr);
		    vHDMI2ComAnaSAOSC(u2HDMIRXAnaBaseAddr,FALSE);
		    vHDMI2ComAnaEQOSC(u2HDMIRXAnaBaseAddr,FALSE);

		    vHDMI2ComAnaCDRLock(u2HDMIRXAnaBaseAddr);
		    vHDMI2ComAnaEQLock(u2HDMIRXAnaBaseAddr,0,&tLEQItem,FALSE);
			if(0)
		    {
		        vHDMI2ComAnaEQIncreaseEnable(u2HDMIRXAnaBaseAddr,FALSE);
		        if(bHDMI2ComAnaAdjustRatioSet(u2HDMIRXAnaBaseAddr))
		        {
		            vHDMI2ComAnaAdjustEnable(u2HDMIRXAnaBaseAddr);
		        }
		        else
		        {
		           // LOG(1, "Adjust ratio invalid!\n");
		        }
		    }
			vUtDelay2us(100); //200us
		    vHDMI2ComDeepRst1(u2HDMIRxBaseAddr);
		    vHDMI2ComDigPhyRst1(u2HDMIRxBaseAddr);
		}
		else if(resettype == HDMI_RST_EQ)
		{
			vHDMI2ComAnaEQMacroEnable(u2HDMIRXAnaBaseAddr);
		    vHDMI2ComAnaEQOSC(u2HDMIRXAnaBaseAddr,FALSE);
			vHDMI2ComAnaEQLock(u2HDMIRXAnaBaseAddr,0,&tLEQItem,FALSE);
		}	
    }

    if(resettype == HDMI_RST_EQ)
    {
    
    }

    if(resettype == HDMI_RST_DIGITALPHY)
    {
    
    }

    if(resettype == HDMI_RST_RTCK)
    {
       
    }

    if(resettype == HDMI_RST_DEEPCOLOR)
    {

    }

}

void vHDMI2Hdcp2xInit(void)
{
	vHDMI2ComHDCP2Init(0);
}

void vHDMI2SwRst(HdmiRxPort ePort)
{
	UINT32 u4TmpHdmiResBase = 0;
    u4TmpHdmiResBase = u2HDMI2GetDigBaseAddr(ePort);
	vHDMI2ComSwRst(u4TmpHdmiResBase);
}

void vHDMI2SetColorRalated(HdmiRxPort ePort)
{
	UINT8 u1ColorInfo;
	UINT16 u2HDMIRxBaseAddr;
	BOOL bIs422 = FALSE;
	BOOL bIsRGB = TRUE;
	
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(ePort);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((fgIsMainDVI() || fgIsPipDVI()) && (fgHDMI2GetDuleMode(ePort)))
#else
	if(fgIsMainDVI() || fgIsPipDVI())
#endif
	{
		vHDMI2ComDuleModeClk(u2HDMIRxBaseAddr);
	}
	else
	{
		vHDMI2ComSingleModeClk(u2HDMIRxBaseAddr);
	}

	if(fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr))
	{
		if(IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) == 0x82)
		{
			u1ColorInfo = (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 2) & 0x60);
			if(u1ColorInfo == 0x20)//422
			{
				bIs422=TRUE;
			}
			if(u1ColorInfo != 0x00)//RGB
			{
				bIsRGB =FALSE;
			}
		}
	}
#ifdef CC_HDMI_OLD_NEW_SUPPORT
   if(fgHDMI2GetDuleMode(ePort))
#endif
   {
   if(bIs422)
   {
		if(fgIsMainDVI())
		{
		    vIO32WriteFldAlign(CCIR_00, 0, CCH_SRC24_SEL);//422->444
		    vIO32WriteFldAlign(CCIR_00, 0, CCH_CBCR_SW);//no CbCr Swap
		    vIO32WriteFldAlign(OMUX_00, 0x4, OMUX_MAIN_SEL);
		}
		if(fgIsPipDVI())
		{
		    vIO32WriteFldAlign(CCIR_00_NEW, 0, CCH_SRC24_SEL_PIP);
		    vIO32WriteFldAlign(CCIR_00_NEW, 0, CCH_CBCR_SW_PIP);
		    vIO32WriteFldAlign(OMUX_00, 0x5 , OMUX_PIP_SEL);//ccir2
		}
   }
   else
   {
		if(fgIsMainDVI())
		{
		    vIO32WriteFldAlign(CCIR_00, 1, CCH_SRC24_SEL);
		    vIO32WriteFldAlign(CCIR_00, 1, CCH_CBCR_SW);
		    vIO32WriteFldAlign(OMUX_00, 0x4, OMUX_MAIN_SEL);
		}
		if(fgIsPipDVI())
		{
		    vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_SRC24_SEL_PIP);
		    vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_CBCR_SW_PIP);
		    vIO32WriteFldAlign(OMUX_00, 0x5 , OMUX_PIP_SEL);//ccir2
		}
   }
   }
   if(bIsRGB)
   {
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CONFIG + u2HDMIRxBaseAddr, 1, RG_HDMI2_ENABLE_BLANKING_OVERRIDE);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CONFIG + u2HDMIRxBaseAddr, 0x100, RG_HDMI2_OUTPUT_BLANK_Y);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CB + u2HDMIRxBaseAddr, 0x100, RG_HDMI2_OUTPUT_BLANK_CB);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CB + u2HDMIRxBaseAddr, 0x100, RG_HDMI2_OUTPUT_BLANK_CB);
   }
   else
   {
   		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CONFIG + u2HDMIRxBaseAddr, 1, RG_HDMI2_ENABLE_BLANKING_OVERRIDE);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CONFIG + u2HDMIRxBaseAddr, 0x100, RG_HDMI2_OUTPUT_BLANK_Y);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CB + u2HDMIRxBaseAddr, 0x800, RG_HDMI2_OUTPUT_BLANK_CB);
		vIO32WriteFldAlign(HDMI2_OUTPUT_BLANK_CB + u2HDMIRxBaseAddr, 0x800, RG_HDMI2_OUTPUT_BLANK_CB);
   }
   //Workaround for 5890 E3 Port C deep color issue
   vHDMI2SwSetDeepColorDivider(ePort);
   	
}

void vHDMI2StateMachineCtrl(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	
	if(Hdmi2RxObj[eHdmiPort]._bHdmiPlug == 1)
	{
		if(bHDMIPort5VStatus(eHdmiPort))
		{
			Hdmi2RxObj[eHdmiPort]._bHDMIPower5vStatus =1;
			if(bHDMI2GetStateMachine(eHdmiPort)== HDMI_WAIT_CONNECT_STATE)
			{
				vHDMI2SaveTmdsClkRate(eHdmiPort, HDMI_TMDS_CLOCK_UNKNOW);
				vHDMI2EQCIRCUITCTRL(eHdmiPort,TRUE);
				vHDMI2SetStateMachine(eHdmiPort,HDMI_WAIT_STABLE_STATE);
			}
		}
		else
		{
			Hdmi2RxObj[eHdmiPort]._bHDMIPower5vStatus =0;
			if(bHDMI2GetStateMachine(eHdmiPort)== HDMI_WAIT_CONNECT_STATE)
			{

			}
			else
			{
				//Hdmi2RxObj[eHdmiPort]._bHdmiPlug =0;
				vHDMI2SetHpdPulse(eHdmiPort,0);
				vHDMI2SetStateMachine(eHdmiPort,HDMI_NOSIGNAL_STATE);
				vHDMI2SetUnplug(eHdmiPort);
				vHDMI2ComClearScdcStatus();
#ifdef CC_HDMI_OLD_NEW_SUPPORT
				vHDMI2ComDdcSel(u2HDMIRxBaseAddr,1); 
				vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,1);
#endif
				Printf("HDMI port %d no 5v--------\n",eHdmiPort);
			}
			
		}

		if( fgHDMI2ComGetCKDT(u2HDMIRxBaseAddr) && fgHDMI2ComGetSCDT(u2HDMIRxBaseAddr))
		{
		    if(Hdmi2RxObj[eHdmiPort]._fgNoScdtWithCkdt) //Last status is no scdt with ckdt, the last setting may set at unstable signal, need to set again
            {
                vHDMI2PhySetting(eHdmiPort);
                Hdmi2RxObj[eHdmiPort]._fgNoScdtWithCkdt = FALSE;
            }
            else
            {
    			if((u1IsHDMI2Stable(eHdmiPort)||u1IsHDMI2HdcpStable(eHdmiPort)) 
    				&&(bHDMI2GetStateMachine(eHdmiPort) == HDMI_WAIT_STABLE_STATE)
    			   )
    			{
					vHDMI2RxIntEn(eHdmiPort,1);
    				vHDMI2SetStateMachine(eHdmiPort,HDMI_STABLE_STATE);
    			}
				else
				{
					if(Hdmi2RxObj[eHdmiPort]._bHDMIResUnStableCnt > 5)
					{
						Hdmi2RxObj[eHdmiPort]._bHDMIResUnStableCnt =0;
						vHDMI2ComDigPhyRst1(u2HDMIRxBaseAddr);
						vHDMI2ComDeepRst1(u2HDMIRxBaseAddr);
						vHDMI2ComSplitRst(u2HDMIRxBaseAddr);
					}
				}
            }
		}
		else
		{    
			if(bHDMI2GetStateMachine(eHdmiPort) == HDMI_STABLE_STATE )
			{
				vHDMI2ComClearScdcStatus();
				Printf("HDMI port %d sync or clock lost--------\n",eHdmiPort);
				u1IsHdmi2Cap =0;
                vHDMI2VideoMute(eHdmiPort,1);//mute
				vHDMI2SetSyncLose(eHdmiPort,1);
				vHDMI2ComSingleModeClk(u2HDMIRxBaseAddr);
				vHDMI2SwRst(eHdmiPort);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
				if(u1HDMI2GetDviModeCnt(eHdmiPort) == 0)
				{
					vHDMI2ComDdcSel(u2HDMIRxBaseAddr,1); 
					vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,1);
				}
#endif
				vHDMI2SaveTmdsClkRate(eHdmiPort, HDMI_TMDS_CLOCK_UNKNOW);
				vHDMI2SetStateMachine(eHdmiPort,HDMI_WAIT_STABLE_STATE);
			}

			if(bHDMI2GetStateMachine(eHdmiPort) == HDMI_WAIT_STABLE_STATE)
			{
				if(fgHDMI2ComGetCKDT(u2HDMIRxBaseAddr))
				{	
				    Hdmi2RxObj[eHdmiPort]._bNoScdtWithCkdtCount++;
                    if(Hdmi2RxObj[eHdmiPort]._bNoScdtWithCkdtCount >= HDMI2_NO_SCDT_WITH_CKDT_WAIT_COUNT) //Need to wait SCDT stable
                    {
                        vHDMI2PhySetting(eHdmiPort);
                        Hdmi2RxObj[eHdmiPort]._bNoScdtWithCkdtCount = 0;
                        Hdmi2RxObj[eHdmiPort]._fgNoScdtWithCkdt = TRUE;
					    LOG(1,"HDMI port %d reset phy(no scdt with ckdt for %d times)--------\n",eHdmiPort,HDMI2_NO_SCDT_WITH_CKDT_WAIT_COUNT);
                    }              
				}
			}	
		}
	}
}

void vHDMI2VideoSetting(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	
	if(IS_IC_5861())
	{
		if(fgHDMI2GetDuleMode(eHdmiPort))
		{
			vHDMI2ComSetSplit(u2HDMIRxBaseAddr,1);
		}
		else
		{
			vHDMI2ComSetSplit(u2HDMIRxBaseAddr,0);
		}
	}
	if(fgHDMI2Get420Mode(eHdmiPort))
	{
		vHDMI2ComSet420To444(u2HDMIRxBaseAddr,1);
	}
	else
	{
		vHDMI2ComSet420To444(u2HDMIRxBaseAddr,0);
	}
	if(eHdmiPort == (HdmiRxPort)eActiveHdmiPort)
    { 
    	vHDMI2VideoMute(eHdmiPort,0);
    	_bHdmiMode = fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr);
		_bHdmiModeActive = fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr);
		vHDMI2SetColorRalated(eHdmiPort);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if((_bHdmiMode == 0) && !fgHDMI2GetDuleMode(eHdmiPort) && (u1HDMI2GetDviModeCnt(eHdmiPort) == 0))
		{
			vHDMI2SetDviModeCnt(eHdmiPort,5);
			vHDMI2SetStateMachine(eHdmiPort,HDMI_NOSIGNAL_STATE);
			vHDMI2ComDdcSel(u2HDMIRxBaseAddr,0); 
			vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,0);
			LOG(0,"\n>> >> HDMI2.0 port DVI mode,repull hpd************\n");
		}

		if(u1HDMI2GetDviModeCnt(eHdmiPort))
		{
			Hdmi2RxObj[eHdmiPort]._bHDMIDviModeCnt --;
			//vHDMI2SetDviModeCnt(eHdmiPort,(u1HDMI2GetDviModeCnt(eHdmiPort)--));
		}
#endif
    }
}
UINT8 bHDMI2GetHdmiMode(HdmiRxPort eHdmiPort)
{
	return Hdmi2RxObj[eHdmiPort]._HdmiMode;
}
void vHDMI2ParserAviInfo(HdmiRxPort eHdmiPort)
{
	UINT8 u1Colorimetry;
	UINT8 u1ExtColorrimetry;
	UINT8 u1ColorInfo;
	UINT8 u1Quant;
	UINT8 u1Yquant;

	if(bHDMI2GetHdmiMode(eHdmiPort)== HDMI_RX_MODE_DVI)
	{
		Hdmi2RxObj[eHdmiPort]._HdmiClrSpc=0;
		Hdmi2RxObj[eHdmiPort]._HdmiRange =0;
		Hdmi2RxObj[eHdmiPort]._HdmiScan =0;
		Hdmi2RxObj[eHdmiPort]._HdmiAspect=0;
		Hdmi2RxObj[eHdmiPort]._HdmiAFD=0;
		Hdmi2RxObj[eHdmiPort]._HdmiITC.fgItc=0;
		Hdmi2RxObj[eHdmiPort]._HdmiITC.ItcContent=0;
		return;
	}
	if(Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoID != HDMI_INFO_FRAME_ID_AVI)
	{
		Hdmi2RxObj[eHdmiPort]._HdmiClrSpc=0;
		Hdmi2RxObj[eHdmiPort]._HdmiRange =0;
		Hdmi2RxObj[eHdmiPort]._HdmiScan =0;
		Hdmi2RxObj[eHdmiPort]._HdmiAspect=0;
		Hdmi2RxObj[eHdmiPort]._HdmiAFD=0;
		Hdmi2RxObj[eHdmiPort]._HdmiITC.fgItc=0;
		Hdmi2RxObj[eHdmiPort]._HdmiITC.ItcContent=0;
		return;
	}
	
	u1Colorimetry =	(Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[5]& 0xC0) >>6;
	u1ExtColorrimetry = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[6]& 0x70) >>4;
	u1ColorInfo = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[4] & 0x60) | (u1Colorimetry <<3) | u1ExtColorrimetry;
	u1Quant = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[6] & 0x06) >>2;
	u1Yquant = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[8] & 0xc0)>>6;
#if 0
	Hdmi2RxObj[eHdmiPort]._HdmiScan = Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[4] & 0x03;
	Hdmi2RxObj[eHdmiPort]._HdmiAspect = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[5] & 0x30)>>4;
	Hdmi2RxObj[eHdmiPort]._HdmiAFD = Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[5] & 0x0f;
	Hdmi2RxObj[eHdmiPort]._HdmiITC.fgItc = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[6] & 0x80) >> 7;
	if(Hdmi2RxObj[eHdmiPort]._HdmiITC.fgItc)
	{
		Hdmi2RxObj[eHdmiPort]._HdmiITC.ItcContent = (Hdmi2RxObj[eHdmiPort]._AviInfo.u1InfoData[8] & 0x30)>>4;
	}
#endif
	switch(u1ColorInfo)
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc =HDMI_RX_CLRSPC_RGB;
			break;
		case 0x1C:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc =HDMI_RX_CLRSPC_Adobe_RGB;
			break;
		case 0x1E:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc =HDMI_RX_CLRSPC_BT_2020_RGB_non_const_luminous;
			break;
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC422_601;
			break;
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC422_709;
			break;
		case 0x38:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC422_601;
			break;
		case 0x39:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC422_709;
			break;
		case 0x3A:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_sYCC422_601;
			break;
		case 0x3B:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_Adobe_YCC422_601;
			break;
		case 0x3D:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC422_const_luminous;
			break;
		case 0x3E:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC422_non_const_luminous;
			break;
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:
		case 0x48:
		case 0x49:
		case 0x4a:
		case 0x4b:
		case 0x4c:
		case 0x4d:
		case 0x4e:
		case 0x4f:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC444_601;
			break;
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x56:
		case 0x57:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC444_709;
			break;
		case 0x58:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC444_601;
			break;
		case 0x59:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC444_709;
			break;
		case 0x5A:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_sYCC444_601;
			break;
		case 0x5B:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_Adobe_YCC444_601;
			break;
		case 0x5D:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC444_const_luminous;
			break;
		case 0x5E:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC444_non_const_luminous;
			break;
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
	    case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
		case 0x6c:
		case 0x6d:
		case 0x6e:
		case 0x6f:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC420_601;
			break;
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_YC420_709;
			break;
		case 0x78:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC420_601;
			break;
		case 0x79:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_XVYC420_709;
			break;
		case 0x7A:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_sYCC420_601;
			break;
		case 0x7B:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_Adobe_YCC420_601;
			break;
		case 0x7D:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC420_const_luminous;
			break;
		case 0x7E:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_BT_2020_YCC420_non_const_luminous;
			break;
		default:
			Hdmi2RxObj[eHdmiPort]._HdmiClrSpc = HDMI_RX_CLRSPC_RGB;
			break;	
	}

	if(u1Quant ==1)
	{
		//RGB limit
		Hdmi2RxObj[eHdmiPort]._HdmiRange = HDMI_RX_RGB_LIMT;
	}
	else if(u1Quant ==2)
	{
		//RGB full
		Hdmi2RxObj[eHdmiPort]._HdmiRange = HDMI_RX_RGB_FULL;
	}

	if(u1Yquant == 1)
	{
		//YCC limit
		Hdmi2RxObj[eHdmiPort]._HdmiRange = HDMI_RX_YCC_LIMT;
	}
	else if(u1Yquant ==2)
	{
		//YCC full
		Hdmi2RxObj[eHdmiPort]._HdmiRange = HDMI_RX_YCC_FULL;
	}
}

void vHDMI2ParserVsiInfo(HdmiRxPort eHdmiPort)
{
	UINT8 u1IeeeCode[3]={0};
	
	if(bHDMI2GetHdmiMode(eHdmiPort)== HDMI_RX_MODE_DVI)
	{
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable =0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Video_Format = 0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = 0;
		return;
	}
	
	if(Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoID != HDMI_INFO_FRAME_ID_VSI)
	{
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable =0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Video_Format = 0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = 0;
		return;
	}
	
	u1IeeeCode[0] = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[4];
	u1IeeeCode[1] = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[5];
	u1IeeeCode[2] = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[6];
	
	if((u1IeeeCode[0] == 0x03) && (u1IeeeCode[1] == 0x0C) && (u1IeeeCode[2] == 0x00))//HDMI vsi
	{
		if((Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[7] >> 5) == 0x2)
		{
			Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable =1;
			Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Video_Format = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[7] >> 5;
			Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[8]>>4;
			if(Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_SideBySideHalf)
			{
				Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_EXTDATA = Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[9]>> 4;
			}
			else
			{
				Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_EXTDATA =0;
			}
			
		}
	}
	else if((u1IeeeCode[0] == 0x1d) && (u1IeeeCode[1] == 0xa6) && (u1IeeeCode[0] == 0x7c))//mhl vsif
	{
		if((Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[7] & 0x01) == 0x01)
		{
			Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable =1;
			switch((Hdmi2RxObj[eHdmiPort]._VsiInfo.u1InfoData[7] >>2) & 0x0f)
			{
				case 0x00:
					Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = HDMI_3D_Structure_FramePacking;
					break;
				case 0x01:
					Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = HDMI_3D_Structure_TopBottom;
					break;
				case 0x02:
					Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = HDMI_3D_Structure_SideBySideHalf;
					break;
				default:
					break;	
			}
			Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_EXTDATA =0;
		}
	}
	else
	{
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable =0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Video_Format = 0;
		Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure = 0;
	}
    
}
void vHDMI2ParserInfo(HdmiRxPort eHdmiPort)
{
	vHDMI2ParserAviInfo(eHdmiPort);
	vHDMI2ParserVsiInfo(eHdmiPort);
}

UINT8 bHDMI2ScanInfo(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        return (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 2) & 0x03);
    }
}
UINT8 bHDMI2AspectRatio(HdmiRxPort eHdmiPort)
{
    UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        return ((u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 3) & 0x30) >> 4);
    }
 
}
UINT8 bHDMI2AFD(HdmiRxPort eHdmiPort)
{
    UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        return (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 3) & 0x0f);
    }

}
UINT8 bHDMI2422Input(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        if(((u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HDMIRxBaseAddr + 2) & 0x60) >> 5)==1)
        {
        	return 1;
        }
		else
		{
			return 0;
		}
    }
}
UINT8 bHDMI2GetColorDepth(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
    return bHDMI2ComGetDeepSta(u2HDMIRxBaseAddr);
}

UINT8 bHDMI2ITCFlag(HdmiRxPort eHdmiPort)
{
    UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        return ((u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HDMIRxBaseAddr ) & 0x80) >> 7);
    }
 
}
UINT8 bHDMI2ITCContent(HdmiRxPort eHdmiPort)
{
    UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
	if(!fgHDMI2ComGetHdmiMode(u2HDMIRxBaseAddr) || (IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HDMIRxBaseAddr,RG_HDMI2_CEA_AVI_HEADER0) != 0x82))
	{
        return 0;
    }
    else
    {
        return ((u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HDMIRxBaseAddr + 2) & 0x30) >> 4);
    }
}

UINT8 bHDMIHDCP1StatusGet(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
    return  bHDMI2ComGetHdcp1Status(u2HDMIRxBaseAddr);
}

UINT8 bHDMIHDCP2StatusGet(HdmiRxPort eHdmiPort)
{
	UINT16 u2HDMIRxBaseAddr;
	u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
    return bHDMI2ComGetHdcp2Status(u2HDMIRxBaseAddr);
}

void vHDMI2PrintHdmiStatus(HdmiRxPort eHdmiPort)
{
	Printf("5v:%d\n",Hdmi2RxObj[eHdmiPort]._bHDMIPower5vStatus);
	Printf("Hpd:%d\n",Hdmi2RxObj[eHdmiPort]._bHdmiPlug);
	Printf("CKDT:%d\n",Hdmi2RxObj[eHdmiPort]._bHDMICkdt);
	Printf("SCDT:%d\n",Hdmi2RxObj[eHdmiPort]._bHDMIScdtLose);
	Printf("ClockRate:%d\n",Hdmi2RxObj[eHdmiPort]._TmdsClkRate);
	Printf("Htotal:%d\n",Hdmi2RxObj[eHdmiPort]._ResInfo._u4HTotal);
	Printf("Vtotal:%d\n",Hdmi2RxObj[eHdmiPort]._ResInfo._u4VTotal);
	Printf("Width:%d\n",Hdmi2RxObj[eHdmiPort]._ResInfo._u4Width);
	Printf("Height:%d\n",Hdmi2RxObj[eHdmiPort]._ResInfo._u4Height);
	Printf("HDMImode:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiMode);
	Printf("HDCPver:%d\n",Hdmi2RxObj[eHdmiPort]._HdcpLinkVer);
	Printf("HDCP14:%d\n",Hdmi2RxObj[eHdmiPort]._bHDMIHdcp1xGood);
	Printf("HDCP22:%d\n",Hdmi2RxObj[eHdmiPort]._bHDMIHdcp2xGood);
	Printf("Depth:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiDepth);
	Printf("Colorspc:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiClrSpc);
	Printf("Range:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiRange);
	Printf("Scan:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiScan);
	Printf("AspactRatio:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiAspect);
	Printf("AFD:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiAFD);
	Printf("ITC:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiITC.fgItc);
	Printf("ITCcontent:%d\n",Hdmi2RxObj[eHdmiPort]._HdmiITC.ItcContent);
	Printf("3D:%d\n",Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Enable);
	Printf("3DFormat:%d\n",Hdmi2RxObj[eHdmiPort]._Hdmi3DInfo.HDMI_3D_Structure);
	
}
#ifdef CC_SUPPORT_STR
void HDMI2_pm_suspend(void)
{
	HdmiRxPort eHdmiPort=0;
	
	eHdmiPort = u1GetMinHDMI20Port();

	vIO32Write1B(PD_SYS_0 ,0x00);
    vIO32Write1B(PD_SYS_2 , 0x0);
    //vIO32Write1B(PD_SYS_3,0x3f);  // except Tri-state Audio/Video output
    vIO32Write1B(PD_SYS_3 , 0x0); // except Tri-state Audio/Video output
    vIO32Write1B(PD_SYS_3 , 0x0);	//solve analog PHY bandgap start-up
    vIO32Write1B(PD_SYS_3 , 0x0); //solve analog PHY bandgap start-up

	vHDMI2TMDSCTRL(eHdmiPort,FALSE);
}

void HDMI2_pm_resume(void)
{
	
	HdmiRxPort ePort=0;
	
	for(ePort=u1GetMinHDMI20Port();ePort< HDMI_RX_PORT_MAX;ePort++)
	{
		x_memset(&Hdmi2RxObj[ePort],0,sizeof(HdmiRxObject_t));
		Hdmi2RxObj[ePort]._bHDMILastAudFreq = HDMI2_AUD_FS_UNKNOWN; //Init as Fs unknown
		vHDMI2SetBaseAddr(ePort);
		vHDMI2HwInit(ePort);
		vHDMI2TMDSCTRL(ePort,FALSE);
		vHDMI2DDCBusCTRL(ePort,FALSE);
		vHDMI2ComDdcSel(u2HDMI2GetDigBaseAddr(ePort),1);
		vHDMIHPDLow(ePort);
		vHDMI2RxIntEn(ePort,0);
		vHDMI2SetStateMachine(ePort,HDMI_NOSIGNAL_STATE);
		
	}
	
}
#endif
void vHDMI2MainLoop(void)
{
	HdmiRxPort eHdmiPort=0;
	UINT16 u2HDMIRxBaseAddr;
	UINT16 u2HDMIAnaAddr;
	static UINT8 bHdcpInit=0;

	/**Note----Becasue EEPROM or flash may not ready in vHDMI2Init state**/
 	/*ADD----Load HDCP2.2 key and 1.4 key from EEPROM or Flash and push into internal SRAM*/
	/*ADD----Load HDCP2.2 ROMcode form  Flash and push into internal SRAM*/
	/*ADDwithSampleCode----Stop and resume FW*/
	if(bHdcpInit == 0)
	{
#ifdef CC_HDMI_2_0_HDCP_BIN
		vHDMI20LoadHdcpKey();
#endif
		vHDMI2Hdcp2xInit();
		bHdcpInit++;
		return;
	}

	for(eHdmiPort = u1GetMinHDMI20Port();eHdmiPort <HDMI_RX_PORT_MAX;eHdmiPort++)
	{
		
		u2HDMIRxBaseAddr = u2HDMI2GetDigBaseAddr(eHdmiPort);
		u2HDMIAnaAddr = u2HDMI2GetAnaBaseAddr(eHdmiPort);
		if(Hdmi2RxObj[eHdmiPort]._bHdmiCmd == 6)//stop fw
		{
			return;
		}
		vHDMI2AudErrorHandler(eHdmiPort);
		vHDMI2StateMachineCtrl(eHdmiPort);
		
		if(!fgHDMI2ComGetCKDT(u2HDMIRxBaseAddr))
		{
			vHDMI2SaveTmdsClkRate(eHdmiPort, HDMI_TMDS_CLOCK_UNKNOW);
			Hdmi2RxObj[eHdmiPort]._bHDMICkdt =0;
			LOG(3,"HDMI port %d no CKDT____________\n",eHdmiPort);
		}
		else
		{
			Hdmi2RxObj[eHdmiPort]._bHDMICkdt =1;
			
			vHDMI2CheckBandChange(eHdmiPort);
			
		}

		if(Hdmi2RxObj[eHdmiPort]._bHDMIStateOld != Hdmi2RxObj[eHdmiPort]._bHDMIState)
		{
			Printf("HDMI port %d state from %d change to %d--------\n",eHdmiPort,Hdmi2RxObj[eHdmiPort]._bHDMIStateOld,Hdmi2RxObj[eHdmiPort]._bHDMIState);
			Hdmi2RxObj[eHdmiPort]._bHDMIStateOld = Hdmi2RxObj[eHdmiPort]._bHDMIState;
		}
		
		switch(Hdmi2RxObj[eHdmiPort]._bHDMIState)
		{
			case HDMI_NOSIGNAL_STATE:
				//x_memset(&Hdmi2RxObj[eHdmiPort],0,sizeof(HdmiRxObject_t));
				u1IsHdmi2Cap =0;
				vHDMI2RxIntEn(eHdmiPort,0);
				vHDMIHPDLow(eHdmiPort);
				vHDMI2TMDSCTRL(eHdmiPort,FALSE);
				vHDMI2DDCBusCTRL(eHdmiPort,FALSE);
				vHDMI2SwRst(eHdmiPort);
				vHDMI2MuteAudio(eHdmiPort,TRUE);
				vHDMI2AudConfig(eHdmiPort);
				//Hdmi2RxObj[eHdmiPort]._bHdmiPlug =0;
				vHDMI2SetHpdPulse(eHdmiPort,0);
				vHDMI2SetHpdLowTimer(eHdmiPort,HDMI2_HPD_Period);
				vHDMI2SetStateMachine(eHdmiPort,HDMI_WAIT_CONNECT_STATE);
				break;
			case HDMI_WAIT_CONNECT_STATE:
				break;
			case HDMI_WAIT_STABLE_STATE: 
                vHDMI2TMDSCTRL(eHdmiPort,TRUE);
                vHDMI2EQCIRCUITCTRL(eHdmiPort,TRUE);
                vHDMI2HdcpStableMonitor(eHdmiPort);
                vHDMI2ResStableMonitor(eHdmiPort);
                //Workaround for 5890 E3 Port C deep color issue
                vHDMI2SwSetDeepColorDivider(eHdmiPort);
				break;
			case HDMI_STABLE_STATE:
#ifdef CC_HDMI_OLD_NEW_SUPPORT
				if(fgHDMI2GetDuleMode(eHdmiPort) && (eHdmiPort == (HdmiRxPort)eActiveHdmiPort))
				{
					vHDMI2ComDdcSel(u2HDMIRxBaseAddr,1); 
					vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,1);
				}
				else
				{
					vHDMI2ComDdcSel(u2HDMIRxBaseAddr,0); 
					vHDMI2ComNewDigSel(u2HDMIRxBaseAddr,0);
				}
#endif
				vHDMI2VideoSetting(eHdmiPort);
				vHDMI2ParserInfo(eHdmiPort);
				break;
			default:
				vHDMI2SetStateMachine(eHdmiPort,HDMI_NOSIGNAL_STATE);
			break;
		}
	}


	
}
