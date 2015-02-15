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
 * @author chiachi_chiang@mtk.com.tw
 * @author darren_weng@mtk.com.tw
 * @author daniel_hsiao@mtk.com.tw
 */
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "general.h"
#include "hw_hdmi2.h"
#include "drv_hdmi2.h"
#include "hw_dvi.h"
#include "hw_hdtv.h"
#include "video_def.h"
#include "mute_if.h"
#include "drv_hdmi2com.h"
#include "drv_dvi2.h"
#include "drv_dvi.h"
#include "hw_hdmi.h"
#include "dvi_debug.h"
#include "video_def.h"
#include "drv_tdtv_drvif.h"
#include "vdo_misc.h"
#ifdef CC_SUPPORT_W2DR2R
#include "drv_w2dr2r.h"
#endif
LINT_EXT_HEADER_END

#define DVI2_TMP_THR 2
#define WA_3D_FA 1

DviStatus_t DviSta[2];

extern E_HDMI_SWITCH_NUM eActiveHdmiPort;
extern E_HDMI_SWITCH_NUM eActiveHdmiPort1;
extern HdmiRxObject_t Hdmi2RxObj[HDMI_RX_PORT_MAX];
extern E_TDTV_DECODER_INPUT _bCurInpMode;
extern UINT32 _bQueryMode;
extern UINT32 _bCurQueryMode;
void vCCIRSelW2D(UINT8 u1Type, BOOL bSelW2d);
#ifdef CC_HDMI_2_0_HDCP_BIN
extern BOOL fgHDMI20CheckCap(void);
#endif

// Local Function
/**
 * @brief 	Set CCIR input horizontal start.
 * @param	wStart - CCIR input horizontal start
 * @retval	None
 */
void vCCIR2SetAvStart(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_01, wStart, CCH_AV_START);
}

void vCCIR2SetNewAvStart(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_01_NEW, wStart, CCH_AV_START_PIP);
}

void vCCIR2SetAvWidth(UINT16 wWidth)
{
    vIO32WriteFldAlign(DVI_03, wWidth, CCH_AV_WIDTH);
}

void vCCIR2SetNewAvWidth(UINT16 wWidth)
{
    vIO32WriteFldAlign(CCIR_03_NEW, wWidth, CCH_AV_WIDTH_PIP);
}

void vCCIR2SetVsStart(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_HDTV_06, wStart, CCH_VS_START);
}

void vCCIR2SetNewVsStart(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_HDTV_08, wStart, CCH_VS_START_PIP);
}

void vCCIR2SetVdeWidth(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_HDTV_06, wStart, CCH_VDE_WIDTH);
}

void vCCIR2SetNewVdeWidth(UINT16 wStart)
{
	vIO32WriteFldAlign(CCIR_HDTV_08, wStart, CCH_VDE_WIDTH_PIP);
}

UINT16 wCCIR2GetAvStart(void)
{
    return IO32ReadFldAlign(CCIR_01, CCH_AV_START);
}

UINT16 wCCIR2GetNewAvStart(void)
{
    return IO32ReadFldAlign(CCIR_01_NEW, CCH_AV_START_PIP);
}

UINT16 wCCIR2GetAvWidth(void)
{
	return IO32ReadFldAlign(DVI_03, CCH_AV_WIDTH);
}

UINT16 wCCIR2GetNewAvWidth(void)
{
	return IO32ReadFldAlign(CCIR_03_NEW, CCH_AV_WIDTH_PIP);
}

UINT16 wCCIR2GetVsStart(void)
{
	return IO32ReadFldAlign(CCIR_HDTV_06, CCH_VS_START);
}

UINT16 wCCIR2GetNewVsStart(void)
{
	return IO32ReadFldAlign(CCIR_HDTV_08, CCH_VS_START_PIP);
}


UINT16 wCCIR2GetVdeWidth(void)
{
	return IO32ReadFldAlign(CCIR_HDTV_06, CCH_VDE_WIDTH);	
}

UINT16 wCCIR2GetNewVdeWidth(void)
{
	return IO32ReadFldAlign(CCIR_HDTV_08, CCH_VDE_WIDTH_PIP);	
}


/**
 * @brief 	DVI initial function for each mode change or connect/disconnect.
 * @param	None
 * @retval	None
 */
void vDvi2Initial(UINT8 bchannel)
{
    DviSta[bchannel]._bDviTiming = MODE_WAIT;
    // Set Polarity
    //vDviPolarityUniform();
    // reset DVI's blank level
    vIO32Write4BMsk(DVI_02, Fld2Msk32(DVI_BLANK_RESET) , Fld2Msk32(DVI_BLANK_RESET));
	vIO32WriteFldAlign(HDTV_06, 0, MHL_FS_FIX_SEL);

    vIO32WriteFldAlign(LPF_PIP_00, 0, DVI_EN);
	
    // disable 4xSel
    vIO32WriteFldAlign(LPF_PIP_00, 0, CCH_CEN_4XSEL);
    // CCH field out, default non-3D
    //vIO32WriteFldAlign(PQCRC_02, 0, CCH_3D_FPSEL);
	vCCIRSelW2D(bchannel, FALSE);
}

/**
 * @brief 	DVI Call-back function for source select connect or disconnect.
 * @param	bchannel - SV_VP_MAIN / SV_VP_PIP
 * @param	fgIsOn - SV_ON/SV_OFF
 * @retval	None
 */
extern UINT8 _bMAIN_COLOR_SYS;
extern UINT8 _bPIP_COLOR_SYS;
void vDvi2Connect(UINT8 bchannel, UINT8 fgIsOn)
{

	UINT8 activePot;

	activePot = bchannel ? eActiveHdmiPort1 : eActiveHdmiPort;
	LOG(0,"vDvi2Connect,activePot=%d_____________\n",activePot);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
	if((activePot < u1GetMinHDMI20Port()) || (!fgHDMI2GetDuleMode((HdmiRxPort)eActiveHdmiPort)))
#else
	if(activePot < u1GetMinHDMI20Port())
#endif
	{
		vDviConnect(bchannel,fgIsOn);
		return;
	}
	
	if(fgIsOn == SV_ON)
	{
		_bDviExtSrc = SV_DV_HDMI;
		if(bchannel == SV_VP_MAIN) /*-------------------------------CCIR decoder initial------------------------------*/
	    {
	    	if(bHDMI2InputType(eActiveHdmiPort))
			{
				//RGB
				_bMAIN_COLOR_SYS = SV_COLOR_HDMI_RGB;
			}
			else
			{
				//Ycbcr
				if(bHDMI2Clocrimetry(eActiveHdmiPort))
				{
					//601
					_bMAIN_COLOR_SYS = SV_COLOR_HDMI_422_601;
				}
				else
				{
					//709
					_bMAIN_COLOR_SYS = SV_COLOR_HDMI_422_709;
				}
			}
        vIO32WriteFldAlign(LPF_PIP_00, 0, CCH_3D_DISP_POL);

        vIO32WriteFldAlign(SYS_01, 1, CCH_VHS_END);;	// WT solve 262.5 line dvd player
        vIO32WriteFldAlign(LPF_PIP_00, 1, CCH_VHS_END_MODE);
        vIO32WriteFldAlign(CCIR_01, 1, CCH_HMID_SEL);
        vIO32WriteFldAlign(CCIR_00, 1, CCIR656_VSRC);// ccir video 8 from DVI pin
        vIO32WriteFldAlign(CCIR_00, 0, CCH_MUTE_SEL);//CCIR mute sel from HDMI
        vIO32Write4BMsk(CCIR_00, 0x0, Fld2Msk32(CCH_SP1_FLD));// CCH_FLD_DET
        //CCIR initial for 422
        vIO32WriteFldAlign(CCIR_00, 1, CCH_LSB_EN);//12bit
        vIO32WriteFldAlign(CCIR_00, 0, CCIR656_VSRC);
        vIO32WriteFldAlign(CCIR_00, 0, CCH_SRC8_SEL);//video16
        vIO32WriteFldAlign(CCIR_00, 1, CCH_CH_SW);
        vIO32WriteFldAlign(CCIR_00, 0x3, CCH_MODE);
        vIO32WriteFldAlign(CCIR_00, 1, CCH_FLD_DET);
        vIO32WriteFldAlign(CCIR_00, 0, CCH_FLD_DET_INI);
        vIO32WriteFldAlign(CCIR_00, 1, CCH_HS_SEL);
        vIO32WriteFldAlign(CCIR_00, 1, CCH_VS_SEL);
		vIO32WriteFldAlign(CCIR_00, 1, CCH_SRC24_SEL);
		
		vIO32WriteFldAlign(OMUX_00, 0x4 , OMUX_MAIN_SEL);//ccir1
			 vVDOINIrqOn(MSK_MAIN_DET);//open HDMI vdoin interrupt
			_rDVIStat.bIsMain = TRUE;
        
    }
	else if(bchannel == SV_VP_PIP)
	{
			if(bHDMI2InputType(eActiveHdmiPort1))
			{
				//RGB
				_bPIP_COLOR_SYS = SV_COLOR_HDMI_RGB;
			}
			else
			{
				//Ycbcr
				if(bHDMI2Clocrimetry(eActiveHdmiPort1))
				{
					//601
					_bPIP_COLOR_SYS = SV_COLOR_HDMI_422_601;
				}
				else
				{
					//709
					_bPIP_COLOR_SYS = SV_COLOR_HDMI_422_709;
				}
			}
		 vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_3D_DISP_POL_PIP);
		vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VHS_END_PIP);
		vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VHS_END_MODE_PIP);
		vIO32WriteFldAlign(CCIR_01_NEW, 1, CCH_HMID_SEL_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 1, CCIR656_VSRC_PIP);
		vIO32WriteFldAlign(CCIR_01_NEW, 0, CCH_MUTE_SEL_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 0, CCH_SP1_FLD_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_LSB_EN_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 0, CCIR656_VSRC_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 0, CCH_SRC8_SEL_PIP);//video16
		vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_CH_SW_PIP);
				
		vIO32WriteFldAlign(CCIR_00_NEW, 0x3, CCH_MODE_PIP);
        vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_FLD_DET_PIP);
        vIO32WriteFldAlign(CCIR_00_NEW, 0, CCH_FLD_DET_INI_PIP);
        vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_HS_SEL_PIP);
        vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_VS_SEL_PIP);
		vIO32WriteFldAlign(CCIR_00_NEW, 1, CCH_SRC24_SEL_PIP);
		
	    vIO32WriteFldAlign(OMUX_00, 0x5 , OMUX_PIP_SEL);//ccir2
			vVDOINIrqOn(MSK_PIP_DET);//open HDMI vdoin interrupt
			_rDVIStat.bIsPip = TRUE;
		   
		}

		DviSta[bchannel]._bDviModeChgFlg = 0;
	    vDvi2Initial(bchannel);
		vDvi2SetModeChange(bchannel);
		
		_IsDviDetectDone = TRUE;
		DviSta[bchannel]._bDviChkState = DVI_NO_SIGNAL;
		
	}
	else if(fgIsOn == SV_OFF)
    {
        if(fgIsMainDVI())
        {
            _vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_MAIN, 0, FALSE);
        }
        else if(fgIsPipDVI())
        {
            _vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_PIP, 0, FALSE);
        }
		
        LOG(0, "bchannel=%d, vDviConnect Off______\n",bchannel);

        if(bchannel == SV_VP_MAIN)
        {
            vVDOINIrqOff(MSK_MAIN_DET);//dis hdmi vdoin main interrupt
            _rDVIStat.bIsMain = FALSE;
        }
        else
        {
            vVDOINIrqOff(MSK_PIP_DET);//dis hdmi vdoin sub interrupt
            _rDVIStat.bIsPip = FALSE;
        }

		_bDviExtSrc = SV_DV_NONE;
        _IsDviDetectDone = TRUE;
		DviSta[bchannel]._bDviModeChgFlg = 0;
    }
 
}

BOOL fgDvi2IsModeChg(UINT8 bchannel)
{
	return (BOOL)DviSta[bchannel]._bDviModeChgFlg;
}

void vCCIRSelW2D(UINT8 u1Type, BOOL bSelW2d)
{
	UINT8 activePot;
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;

#ifdef CC_SUPPORT_W2DR2R
	if(bSelW2d)
	{
		//vIO32WriteFldAlign(0xf0034010,1,Fld(1,0,AC_MSKB0));
	}
	else
	{
		vW2DDisable(0);
		vW2DDisable(1);
		vR2RSetPatternEn(1,0,0,0);
		//vIO32WriteFldAlign(0xf0034010,0,Fld(1,0,AC_MSKB0));
	}
#endif
	if(activePot < u1GetMinHDMI20Port())
	{
		return;
	}
    if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //Oryx E3 or later
	{
		if(bSelW2d)
		{
			vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_CCIR0);
	        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_CCIR1);
	        vIO32WriteFldAlign(HDMI2_PATH_SEL,1,FLD_HDMI2_PATH_W2D);
			vIO32WriteFldAlign(HDMI2_SEL,1,RG_HDMI2_REG_MAIN_SUB_CLK);
		}
		else
		{
			vIO32WriteFldAlign(HDMI2_PATH_SEL,1,FLD_HDMI2_PATH_CCIR0);
	        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_CCIR1);
	        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_W2D);
			vIO32WriteFldAlign(HDMI2_SEL,0,RG_HDMI2_REG_MAIN_SUB_CLK);
		}
	}
}

void vCCIR2GetStatus(UINT8 bchannel)
{
	if(bchannel == SV_VP_MAIN) 
	{
		Printf("CCIR0 status:\n");
		Printf("Hactive:%d\n",IO32ReadFldAlign(CCIR_HDTV_08, STA_CCH_HDELEN));
		Printf("Htotal:%d\n",IO32ReadFldAlign(CCIR_HDTV_08, STA_CCH_HTOTAL));
		Printf("Vslen:%d\n",IO32ReadFldAlign(CCIR_HDTV_08, STA_CCH_VSLEN));
		Printf("Vtotal:%d\n",IO32ReadFldAlign(CCIR_HDTV_08, STA_CCH_VTOTAL));
	}
	else if(bchannel == SV_VP_PIP)
	{
		Printf("CCIR1 status:\n");
		Printf("Hactive:%d\n",IO32ReadFldAlign(CCIR_05_NEW, STA_CCH_NEW_HDE));
		Printf("Htotal:%d\n",IO32ReadFldAlign(CCIR_05_NEW, STA_CCH_NEW_HTOTAL));
		Printf("Vslen:%d\n",IO32ReadFldAlign(CCIR_06_NEW, STA_CCH_NEW_VSLEN));
		Printf("Vtotal:%d\n",IO32ReadFldAlign(CCIR_06_NEW, STA_CCH_NEW_VTOTAL));
	}
}

UINT16 wDvi2InputHTotal(UINT8 u1Type)
{
    UINT16 wHTotal = 0;

	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        wHTotal= wDviInputHTotal();
    }
    else
    {
        
#ifdef CC_HDMI_OLD_NEW_SUPPORT
        if(!fgHDMI2GetDuleMode(activePot))
        {
            wHTotal= wDviInputHTotal();
        }
        else
#endif
        {
            if(bDvi23DPacketVaild(u1Type))
            {
                wHTotal = dwDvi2Get3DHtotal(u1Type);
            }
            else
            {
                wHTotal = dwHDMI2ComHTotal(0);

                if(fgHDMI2ComGetScramblingBySCDC(0) && (fgHDMI2Get420Mode(4) == FALSE))
                {
                	if(IS_IC_5861()) //For 5861, the video is spilted to dule mode, so the width should be divided by 2
            		{
                		wHTotal /= 2;
            		}
                }
				if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
                {
                    wHTotal /= 2;
                }
#ifdef CC_SUPPORT_UHD
				if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //5890 E3 or later
				{
					if(fgHDMI2Get420Mode(activePot)) //YC420 should tell 4KDD width * 2
					{
						wHTotal *= 2;
					}
				}
#endif
            }
        }
    }
	
    return wHTotal;
}

/**
 * @brief 	DVI call-back function for getting DVI input signal width.
 * @param	None
 * @retval	Height of input signal.
 */
UINT16 wDvi2InputVTotal(UINT8 u1Type)
{
    UINT16 wVTotal = 0;
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        wVTotal = wDviInputVTotal();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			wVTotal= wDviInputVTotal();
		}
		else
#endif
		{
			if(bDvi23DPacketVaild(u1Type))
			{
				wVTotal = dwDvi2Get3DVtotal(u1Type);
			}
			else
			{
				wVTotal = dwHDMI2ComVTotal(0);
			}
		}
    }

    return wVTotal;
}

/**
 * @brief 	DVI call-back function for getting DVI input signal width.
 * @param	None
 * @retval	Hactive of input signal.
 */
UINT16 wDvi2InputWidth(UINT8 u1Type)
{
	UINT16 u2Hactive;
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        u2Hactive = wDviInputWidth();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			u2Hactive= wDviInputWidth();
		}
		else
#endif
		{
			if(bDvi23DPacketVaild(u1Type))
			{
				u2Hactive = dwDvi2Get3DHactive(u1Type);
			}
			else
			{
				u2Hactive = dwHDMI2ComHActive(0);
				if(fgHDMI2ComGetTmdsClkRatio(0) && (!fgHDMI2Get420Mode(activePot)))
				{
					if(IS_IC_5861()) //For 5861, the video is spilted to dule mode, so the width should be divided by 2
					{
						u2Hactive /= 2;
					}
				}
                if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
                {
                    u2Hactive /= 2;
                }
#ifdef CC_SUPPORT_UHD
				if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //5890 E3 or later
				{
					if(fgHDMI2Get420Mode(activePot)) //YC420 should tell 4KDD width * 2
					{
						u2Hactive *= 2;
					}
				}
#endif
			}
		}
    }

	return u2Hactive;
}

/**
 * @brief 	DVI call-back function for getting DVI input signal height.
 * @param	None
 * @retval	Vactive of input signal.
 */
UINT16 wDvi2InputHeight(UINT8 u1Type)
{
   UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        return wDviInputHeight();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return wDviInputHeight();
		}
#endif
		if(bDvi23DPacketVaild(u1Type))
		{
			return dwDvi2Get3DVactive(u1Type);
		}
		else
		{
			return (UINT16)dwHDMI2ComVActive(0);	
		}
    }
    
}

UINT8 bDvi2RefreshRate(UINT8 u1Type)
{
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        return bDviRefreshRate();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return bDviRefreshRate();
		}
#endif
		if(bDvi23DPacketVaild(u1Type))
		{
			return bDvi2Get3DRefreshRate(u1Type);
		}
		else
		{
			return (UINT8)dwHDMI2ComFrameRate(0);
		}	
    }
    
}

UINT8 bDvi2SigStatus(UINT8 u1Type)
{

	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;    
    if(activePot < u1GetMinHDMI20Port())
    {
        return bDviSigStatus();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return bDviSigStatus();
		}
#endif
		if(DviSta[u1Type]._bDviTiming == MODE_DE_MODE)
	    {
	        return (UINT8)(SV_VDO_STABLE);
	    }

	    if(!_IsDviDetectDone)
	    {
	        return (UINT8)SV_VDO_UNKNOWN;
	    }
	    else
	    {
	        switch(DviSta[u1Type]._bDviTiming)
	        {
	            case MODE_NOSIGNAL:
	                return (UINT8)(SV_VDO_NOSIGNAL);

	            case MODE_WAIT:
	                return (UINT8)(SV_VDO_UNKNOWN);

	            case MODE_NOSUPPORT:
	                return (UINT8)(SV_VDO_NOSUPPORT);

	            default:
	                return (UINT8)(SV_VDO_STABLE);
	        }
	    }
    }

}

UINT8 bDvi2Interlace(UINT8 u1Type)
{

	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        return bDviInterlace();
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return bDviInterlace();
		}
#endif
		return bDvi2GetInterlaced(u1Type);
    }

}

BOOL fgIsDvi2VideoTiming(UINT8 u1Type)
{
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
    if(activePot < u1GetMinHDMI20Port())
    {
        return (BOOL)fgIsVideoTiming(_bDviTiming);
    }
    else
    {
#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return fgIsVideoTiming(_bDviTiming);
		}
#endif
		return (BOOL)fgIsVideoTiming(DviSta[u1Type]._bDviTiming);
    }
}

void vDvi2SetModeChange(UINT8 u1Type)
{
	if(DviSta[u1Type]._bDviModeChgFlg)
	{
		return;
	}
	if(u1Type == SV_VP_MAIN)
    {
        // test for fast mode change /done bug
        vClrMainFlg(MAIN_FLG_MODE_DET_DONE);
        vSetMainFlg(MAIN_FLG_MODE_CHG);
		Printf("____main mode change_____\n");
    }
	else
    {
        // test for fast mode change /done bug
        vClrPipFlg(PIP_FLG_MODE_DET_DONE);
        vSetPipFlg(PIP_FLG_MODE_CHG);
		Printf("____sub mode change_____\n");
    }
	DviSta[u1Type]._bDviModeChgFlg =1;
}

UINT32 dwDVI2GetVtotal(UINT16 u2HdmiBase)	
{
 	return dwHDMI2ComVTotal(u2HdmiBase);
}
UINT32 dwDVI2GetHtotal(UINT16 u2HdmiBase)
{
	return dwHDMI2ComHTotal(u2HdmiBase);
}
BOOL fgDVI2GetHAct(UINT16 u2HdmiBase)
{
	return fgHDMI2ComGetSCDT(u2HdmiBase);
}
UINT32 dwDVI2GetWidth(UINT16 u2HdmiBase)
{
	return dwHDMI2ComHActive(u2HdmiBase);
}
UINT32 dwDVI2GetHeight(UINT16 u2HdmiBase)
{
	return dwHDMI2ComVActive(u2HdmiBase);
}

UINT32 dwDvi2PIXClock(UINT16 u2HdmiBase)
{
	return dwHDMI2ComPixelClk(u2HdmiBase);
}

UINT32 dwDvi2HSClock(UINT16 u2HdmiBase)
{
	return dwHDMI2ComHClk(u2HdmiBase);
}

UINT32 dwDvi2VSClock(UINT16 u2HdmiBase)
{
	return dwHDMI2ComFrameRate(u2HdmiBase);
}

#define FP_progressive	1
#define FP_interlace 		2
#define SBS_full_progressive 3
#define SBS_full_interlace 4
#define Field_Alternative 5

/**
 * @brief 	Check 3D progres or interlace frame packing timing
 * @retval	1 is progressive ,2 is interlace,3 is other
 */
	
UINT8 bDvi2Check3DFormat(UINT8 u1Type)
{
    if((DviSta[u1Type]._bDviTiming == MODE_3D_720p_50_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_720p_60_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080p_24_FP)|| (DviSta[u1Type]._bDviTiming == MODE_3D_1080p_60_FP)||
       (DviSta[u1Type]._bDviTiming == MODE_3D_480p_60_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_576p_50_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_720p_24_FP)|| (DviSta[u1Type]._bDviTiming == MODE_3D_1080p_50_FP) ||
       (DviSta[u1Type]._bDviTiming == MODE_3D_720p_30_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080p_30_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_640_480p_60_FP)||
       (DviSta[u1Type]._bDviTiming == MODE_3D_1440_240p_60_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1440_288p_50_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1440_576p_50_FP)||
    	(DviSta[u1Type]._bDviTiming == MODE_3D_720p_25_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080p_25_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1440_480p_60_FP) || (DviSta[u1Type]._bDviTiming == MODE_3D_240p_263LINES))
    {
        return FP_progressive;
    }

    if((DviSta[u1Type]._bDviTiming == MODE_3D_1080I_60_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_480I_60_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_576I_60_FP)||
	   (DviSta[u1Type]._bDviTiming == MODE_3D_1080I_50_FP)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_1250TOTAL_50_FP))
    {
        return FP_interlace;
    }
	if((DviSta[u1Type]._bDviTiming == MODE_3D_1080p_30_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080p_25_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080p_24_SBS_FULL)||
	   (DviSta[u1Type]._bDviTiming == MODE_3D_720p_24_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_720p_30_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_720p_50_SBS_FULL)||
	   (DviSta[u1Type]._bDviTiming == MODE_3D_720p_60_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_480p_60_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_576p_50_SBS_FULL)||
	   (DviSta[u1Type]._bDviTiming == MODE_3D_640_480p_60_SBS_FULL))
    {
        return SBS_full_progressive;
    }

    if((DviSta[u1Type]._bDviTiming == MODE_3D_1080I_50_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_60_SBS_FULL)||
       (DviSta[u1Type]._bDviTiming == MODE_3D_480I_60_SBS_FULL)||(DviSta[u1Type]._bDviTiming == MODE_3D_576I_50_SBS_FULL))
    {
        return SBS_full_interlace;
    }
	if ((DviSta[u1Type]._bDviTiming == MODE_3D_480I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_50_FA)||
	    (DviSta[u1Type]._bDviTiming == MODE_3D_MASTER_1080I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_MASTER_1080I_50_FA)||\
	     (DviSta[u1Type]._bDviTiming == MODE_3D_576I_50_FA))
	{
		return Field_Alternative;
	}
#if 0	
	//for mhl fs 3d format, treat it as FP, but there is no de in the second vblank,so we search timing as la
	if((_bIsMhlDevice == 1) && (eActiveHdmiPort == HDMI_SWITCH_1) &&\
       ((_bDviTiming == MODE_3D_720p_60_LA)||(_bDviTiming == MODE_3D_720p_50_LA)||\
       (_bDviTiming == MODE_3D_1080p_24_LA)))
    {
		return FP_progressive;
	}
#endif		
    return 6;
}

/**
 * @brief 	Check 3D packet vaild or not
 * @retval	1 is vaild
 */
UINT8 bDvi23DPacketVaild(UINT8 u1Type)
{
	UINT8 activePot;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	//vHDMI2ParserVsiInfo(activePot);

    if (Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Enable)
    {
        return 1;
    }
    else
    {
		if((bDvi2Check3DFormat(u1Type)>=FP_progressive)&&(bDvi2Check3DFormat(u1Type)<=SBS_full_interlace))  // no Vendor Specific InfoFrame, but is 3D format(FramePacking, side-by-side full)
		{
			return 1;
		}
		else
		{
			return 0;
        }
    }
}

UINT8 bDvi2Get3DFormat(UINT8 u1Type)
{
	UINT8 activePot;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Enable)
	{
		return Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure;
	}
	else
	{
		return bDvi2Check3DFormat(u1Type);
	}
}

/**
 * @brief 	guery AP force 3D mode
 * @param
 * @retval
 */
UINT32 wDvi2QueryAP3DSetting(UINT8 u1Type)
{
    UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
     
	if(bDvi23DPacketVaild(u1Type) == 0) //input 2D mode
	{
		if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_2D_I;
		}
		else
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_2D_P;
		}
	}
	else  //input 3D mode
	{
		if(bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_LineAlternative)
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_LI_P;
		}
		else if(bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_FieldAlternative)
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_FP_I;
		}
		else if(FP_progressive==bDvi2Check3DFormat(u1Type))
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_FP_P;
		}
		else if(FP_interlace==bDvi2Check3DFormat(u1Type))
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_FP_I;
		}
		else if(SBS_full_progressive==bDvi2Check3DFormat(u1Type))
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_SBS_P;
		}
		else if(SBS_full_interlace==bDvi2Check3DFormat(u1Type))
		{
		    _bCurInpMode=E_TDTV_DECODER_INPUT_SBS_I;
		}
		else if((DviSta[u1Type]._bDviTiming==MODE_525I)||(DviSta[u1Type]._bDviTiming==MODE_625I)||(DviSta[u1Type]._bDviTiming==MODE_1080i_50)||(DviSta[u1Type]._bDviTiming==MODE_1080i))
		{
		    if(bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_SideBySideHalf)
		    {
		        _bCurInpMode=E_TDTV_DECODER_INPUT_SBS_I;
		    }

		    if(bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_TopBottom)
		    {
		        _bCurInpMode=E_TDTV_DECODER_INPUT_TB_I;
		    }
		}
		else
		{
		    if(bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_FramePacking)
		    {
		        _bCurInpMode=E_TDTV_DECODER_INPUT_FP_P;
		    }

		    if((bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_SideBySideFull) || (bDvi2Get3DFormat(u1Type) == HDMI_3D_Structure_SideBySideHalf))
		    {
		        if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
		        {
		            _bCurInpMode=E_TDTV_DECODER_INPUT_SBS_I;
		        }
		        else
		        {
		            _bCurInpMode=E_TDTV_DECODER_INPUT_SBS_P;
		        }
		    }

		    if(bDvi2Get3DFormat(u1Type)== HDMI_3D_Structure_TopBottom)
		    {
		        if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
		        {
		            _bCurInpMode=E_TDTV_DECODER_INPUT_TB_I;
		        }
		        else
		        {
		            _bCurInpMode=E_TDTV_DECODER_INPUT_TB_P;
		        }
		    }
		}
	}

    return u4DrvTDTVHDMIModeQuery((E_TDTV_DECODER_INPUT)_bCurInpMode);
	
}

void vDvi2Set3DConfig(UINT8 u1Type)
{
    UINT32 wVactivewidth;
    UINT32 wVprochwidth;

    if(bDvi23DPacketVaild(u1Type) == 0)
    {
        // CCH field out
        vIO32WriteFldAlign(PQCRC_02, 0, CCH_3D_FPSEL);
        vIO32Write4B(CCIR_HDTV_07,0);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_VACT_SP1);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_VACT_SP2);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_SEL);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_MODE);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, CCH_3D_INTTGL);
        return;
    }
	wVprochwidth=Get_VGAMODE_IPV_STA(DviSta[u1Type]._bDviTiming);
    if(FP_progressive == bDvi2Check3DFormat(u1Type)) //progressive frame packing timing
    {
		wVactivewidth =(Get_VGAMODE_IPV_LEN(DviSta[u1Type]._bDviTiming) - Get_VGAMODE_IPV_STA(DviSta[u1Type]._bDviTiming)) >> 1;
		
        if((DviSta[u1Type]._bDviTiming == MODE_3D_1440_240p_60_FP)&&(wHDMIVTotal() == 526))
        {
            vIO32Write4B(CCIR_HDTV_07,wVactivewidth-1);
            vIO32WriteFldAlign(VFE_EF, wVprochwidth, CCH_3D_VACT_SP1);
        }
        else
        {
            vIO32Write4B(CCIR_HDTV_07,wVactivewidth-1);
            vIO32WriteFldAlign(VFE_EF, wVprochwidth-1, CCH_3D_VACT_SP1);
        }
        vIO32WriteFldAlign(VFE_EF, 0x1, CCH_3D_SEL);
        vIO32WriteFldAlign(VFE_EF, 0x2, CCH_3D_MODE);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, CCH_3D_INTTGL);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, DVI_3D_MANUAL);//work around
        // CCH field out
       vIO32WriteFldAlign(PQCRC_02, 0, CCH_3D_FPSEL);            
    }
    else if(FP_interlace==bDvi2Check3DFormat(u1Type)) //interlace frame packing timing
    {
        wVactivewidth=(Get_VGAMODE_IPV_LEN(DviSta[u1Type]._bDviTiming) - Get_VGAMODE_IPV_STA(DviSta[u1Type]._bDviTiming) * 3 - 2) >> 2;//22(real), 23(by decoder), 22(by decoder, this value is in timing table), 23(by decoder)
		vIO32Write4B(CCIR_HDTV_07,wVactivewidth-1);
        vIO32WriteFldAlign(VFE_EF, wVprochwidth, CCH_3D_VACT_SP1);
        if (_bDviTiming == MODE_3D_1080I_1250TOTAL_50_FP)
        {
            vIO32WriteFldAlign(VFE_EF, wVprochwidth, CCH_3D_VACT_SP2);
        } else {
            vIO32WriteFldAlign(VFE_EF, wVprochwidth-1, CCH_3D_VACT_SP2);
        }
        
        vIO32WriteFldAlign(VFE_EF, 0x1, CCH_3D_SEL);
        vIO32WriteFldAlign(VFE_EF, 0x3, CCH_3D_MODE);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, CCH_3D_INTTGL);
        vIO32WriteFldAlign(LPF_PIP_00, 0x1, DVI_3D_MANUAL);//work around
        // CCH field out
        vIO32WriteFldAlign(PQCRC_02, 1, CCH_3D_FPSEL);            
   }
	else if(Field_Alternative==bDvi2Check3DFormat(u1Type))
	{
		wVactivewidth=(Get_VGAMODE_IPV_LEN(DviSta[u1Type]._bDviTiming) - Get_VGAMODE_IPV_STA(DviSta[u1Type]._bDviTiming)) >> 1;
		vIO32Write4B(CCIR_HDTV_07,wVactivewidth-1);
		vIO32WriteFldAlign(VFE_EF, wVprochwidth-1, CCH_3D_VACT_SP1);
		vIO32WriteFldAlign(VFE_EF, 0x1, CCH_3D_SEL);
        vIO32WriteFldAlign(VFE_EF, 0x2, CCH_3D_MODE);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, CCH_3D_INTTGL);
        vIO32WriteFldAlign(LPF_PIP_00, 0x1, DVI_3D_MANUAL);//work around
        // CCH field out
        vIO32WriteFldAlign(PQCRC_02, 0, CCH_3D_FPSEL);
    }
    else
    {
        vIO32Write4B(CCIR_HDTV_07,0x00);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_VACT_SP1);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_SEL);
        vIO32WriteFldAlign(VFE_EF, 0x0, CCH_3D_MODE);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, CCH_3D_INTTGL);
        vIO32WriteFldAlign(LPF_PIP_00, 0x0, DVI_3D_MANUAL);//work around
        // CCH field out
        vIO32WriteFldAlign(PQCRC_02, 0, CCH_3D_FPSEL);
    }
   
}

/**
 * @brief 	Get htotal,vtotal,hactive,vactive of 3D
 * @param 1
 * @retval type value 1=Vtotal , 2=Vactive , 3=Htotal, 4=Hactive,  5=reflash rate
 */
UINT32 dwDvi2Get3DVtotal(UINT8 u1Type)
{
	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
	{
		case HDMI_3D_Structure_FramePacking:
			if((FP_interlace==bDvi2Check3DFormat(u1Type))||(FP_progressive==bDvi2Check3DFormat(u1Type)))
			{
				return Get_VGAMODE_IVTOTAL(DviSta[u1Type]._bDviTiming) >> 1;
			}
			else
			{
				return dwHDMI2ComVTotal(u4TmpHdmiRegBase) >> 1;
			}

		case HDMI_3D_Structure_FieldAlternative:
			if((DviSta[u1Type]._bDviTiming == MODE_3D_480I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_1080I_50_FA)||
			(DviSta[u1Type]._bDviTiming == MODE_3D_MASTER_1080I_60_FA)||(DviSta[u1Type]._bDviTiming == MODE_3D_MASTER_1080I_50_FA) ||\
			(DviSta[u1Type]._bDviTiming == MODE_3D_576I_50_FA))
			{
				return Get_VGAMODE_IVTOTAL(DviSta[u1Type]._bDviTiming);
			}
			else if (_bDviTiming == MODE_DE_MODE)
			{
				return dwHDMI2ComVTotal(u4TmpHdmiRegBase);
			}

		case HDMI_3D_Structure_SideBySideFull:
			if((SBS_full_progressive==bDvi2Check3DFormat(u1Type)) || (SBS_full_interlace==bDvi2Check3DFormat(u1Type)))
			{
				return Get_VGAMODE_IVTOTAL(DviSta[u1Type]._bDviTiming);
			}
			else
			{
				if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
				{
					return dwHDMI2ComVTotal(u4TmpHdmiRegBase) << 1;
				}
				else
				{
					return dwHDMI2ComVTotal(u4TmpHdmiRegBase);
				}
			}

		case HDMI_3D_Structure_LineAlternative:
			return dwHDMI2ComVTotal(u4TmpHdmiRegBase);
		case HDMI_3D_Structure_SideBySideHalf:
		case HDMI_3D_Structure_TopBottom:
			if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
			{
				if(fgIsDvi2VideoTiming(u1Type))
				{
					return Get_VGAMODE_IVTOTAL(DviSta[u1Type]._bDviTiming);
				}
			else
			{
				return dwHDMI2ComVTotal(u4TmpHdmiRegBase) << 1;
			}
			}
			else
			{
				return dwHDMI2ComVTotal(u4TmpHdmiRegBase);
			}

		default:
			return dwHDMI2ComVTotal(u4TmpHdmiRegBase);
	}
}

UINT32 dwDvi2Get3DVactive(UINT8 u1Type)
{
	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	
	switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
	{

		case HDMI_3D_Structure_FramePacking:
			if(FP_progressive==bDvi2Check3DFormat(u1Type))
			{
				{
					return (Get_VGAMODE_IPV_LEN(_bDviTiming) - Get_VGAMODE_IPV_STA(_bDviTiming)) >> 1;
				}
			}
			else if(FP_interlace==bDvi2Check3DFormat(u1Type))
			{
				return (Get_VGAMODE_IPV_LEN(_bDviTiming) - Get_VGAMODE_IPV_STA(_bDviTiming) * 3 - 2) >> 1;//22(real), 23(by decoder), 22(by decoder, this value is in timing table), 23(by decoder)
			}
			else
			{
				return dwHDMI2ComVActive(u4TmpHdmiRegBase) >> 1;
			}

		case HDMI_3D_Structure_FieldAlternative:
			if((_bDviTiming == MODE_3D_480I_60_FA)||(_bDviTiming == MODE_3D_1080I_60_FA)||(_bDviTiming == MODE_3D_1080I_50_FA)||
			(_bDviTiming == MODE_3D_MASTER_1080I_60_FA)||(_bDviTiming == MODE_3D_MASTER_1080I_50_FA) ||\
			(_bDviTiming == MODE_3D_576I_50_FA))
			{
				return (Get_VGAMODE_IPV_LEN(_bDviTiming));
			}
			else if (_bDviTiming == MODE_DE_MODE)
			{
				return dwHDMI2ComVActive(u4TmpHdmiRegBase);
			}

		case HDMI_3D_Structure_SideBySideFull:
			if((SBS_full_progressive==bDvi2Check3DFormat(u1Type)) || (SBS_full_interlace==bDvi2Check3DFormat(u1Type)))
			{
				return Get_VGAMODE_IPV_LEN(_bDviTiming);
			}
			else
			{
				if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
				{
					return dwHDMI2ComVActive(u4TmpHdmiRegBase) << 1;
				}
				else
				{
					return dwHDMI2ComVActive(u4TmpHdmiRegBase);
				}
			}

		case HDMI_3D_Structure_LineAlternative:
			return dwHDMI2ComVActive(u4TmpHdmiRegBase);
		case HDMI_3D_Structure_SideBySideHalf:
		case HDMI_3D_Structure_TopBottom:
			if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
			{
				if(fgIsDvi2VideoTiming(u1Type))
				{
					return Get_VGAMODE_IPV_LEN(_bDviTiming);
				}
				else
				{
					return dwHDMI2ComVActive(u4TmpHdmiRegBase);
				}
			}
			else
			{
				return dwHDMI2ComVActive(u4TmpHdmiRegBase);
			}

		default:
			return dwHDMI2ComVActive(u4TmpHdmiRegBase);
	}
}

UINT32 dwDvi2Get3DHtotal(UINT8 u1Type)
{
	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
	{
        case HDMI_3D_Structure_FramePacking:
                if((FP_progressive==bDvi2Check3DFormat(u1Type))||(FP_interlace==bDvi2Check3DFormat(u1Type))) 
	            {
	            	if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
	            	{
	            		 return Get_VGAMODE_IHTOTAL(_bDviTiming)/2;
	            	}
					else
					{
						 return Get_VGAMODE_IHTOTAL(_bDviTiming);
					}
	               
	            }
	            else
	            {
	                return dwHDMI2ComHTotal(u4TmpHdmiRegBase);
	            }

		case HDMI_3D_Structure_FieldAlternative:
			if ((_bDviTiming == MODE_3D_480I_60_FA)||(_bDviTiming == MODE_3D_1080I_60_FA)||(_bDviTiming == MODE_3D_1080I_50_FA)||
				(_bDviTiming == MODE_3D_MASTER_1080I_60_FA)||(_bDviTiming == MODE_3D_MASTER_1080I_50_FA) ||\
				(_bDviTiming == MODE_3D_576I_50_FA))
			{
				return Get_VGAMODE_IHTOTAL(_bDviTiming);
			}
			else if (_bDviTiming == MODE_DE_MODE)
			{
				return dwHDMI2ComHTotal(u4TmpHdmiRegBase);
			}

        case HDMI_3D_Structure_SideBySideFull:
            if((SBS_full_progressive==bDvi2Check3DFormat(u1Type)) || (SBS_full_interlace==bDvi2Check3DFormat(u1Type)))
            {
                return Get_VGAMODE_IHTOTAL(_bDviTiming);
            }
            else
            {
                return dwHDMI2ComHTotal(u4TmpHdmiRegBase);
            }

        case HDMI_3D_Structure_SideBySideHalf:
        case HDMI_3D_Structure_TopBottom:
			if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
			{
				return(dwHDMI2ComHTotal(u4TmpHdmiRegBase)/2);
			}
			else
			{
				return dwHDMI2ComHTotal(u4TmpHdmiRegBase);
			}
		case HDMI_3D_Structure_LineAlternative:					
        default:
            return dwHDMI2ComHTotal(u4TmpHdmiRegBase);
	    }
}

UINT32 dwDvi2Get3DHactive(UINT8 u1Type)
{
	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
	{
		case HDMI_3D_Structure_FramePacking:
		case HDMI_3D_Structure_FieldAlternative:
		case HDMI_3D_Structure_LineAlternative:
		case HDMI_3D_Structure_SideBySideFull:
		case HDMI_3D_Structure_SideBySideHalf:
		case HDMI_3D_Structure_TopBottom:
		default:
		    if(fgIsDvi2VideoTiming(u1Type))
		    {
				if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
		    	{
		        	return (Get_VGAMODE_IPH_WID(_bDviTiming) >> 1 );
		    	}
				else
		    	{
					return Get_VGAMODE_IPH_WID(_bDviTiming);
		    	}
		    }
		    else
		    {
		        return dwHDMI2ComHActive(u4TmpHdmiRegBase);
		    }
		}
}

UINT8 bDvi2Get3DRefreshRate(UINT8 u1Type)
{
	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	UINT8 _bDviVclk = DviSta[u1Type]._bDviVclk;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
	{
		case HDMI_3D_Structure_FramePacking:
			if(_bDviTiming == MODE_DE_MODE)
			{
			    return _bDviVclk * 2;
			}
			else if(FP_interlace==bDvi2Check3DFormat(u1Type))
			{
			    return Get_VGAMODE_IVF(_bDviTiming)*4;
			}
			else
			{
			    return Get_VGAMODE_IVF(_bDviTiming)*2;
			}
		case HDMI_3D_Structure_FieldAlternative:
			if ((_bDviTiming == MODE_3D_480I_60_FA)||(_bDviTiming == MODE_3D_1080I_60_FA)||(_bDviTiming == MODE_3D_1080I_50_FA)||
				(_bDviTiming == MODE_3D_MASTER_1080I_60_FA)||(_bDviTiming == MODE_3D_MASTER_1080I_50_FA) || \
				(_bDviTiming == MODE_3D_576I_50_FA))
			{
				return Get_VGAMODE_IVF(_bDviTiming)*2;
			}
			else if (_bDviTiming == MODE_DE_MODE)
			{
				return _bDviVclk * 2;
			}
		case HDMI_3D_Structure_SideBySideFull:
			if((SBS_full_progressive==bDvi2Check3DFormat(u1Type)) || (SBS_full_interlace==bDvi2Check3DFormat(u1Type)))
			{
			    return Get_VGAMODE_IVF(_bDviTiming);
			}
			else
			{
			    return _bDviVclk;
			}

		case HDMI_3D_Structure_LineAlternative:
		case HDMI_3D_Structure_SideBySideHalf:
		case HDMI_3D_Structure_TopBottom:
		default:
			return _bDviVclk;
	}
}

UINT8 bDvi2GetInterlaced(UINT8 u1Type)
{
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	
	if(_bDviTiming == MODE_DE_MODE)
    {
        return 0 ;
    }
    if((_bDviTiming != MODE_NOSIGNAL) &&
       (_bDviTiming < MAX_TIMING_FORMAT))
    {
        if(FP_interlace==bDvi2Check3DFormat(u1Type))
    	{
            return 1;
        }
    	else if (Field_Alternative==bDvi2Check3DFormat(u1Type)) 
    	{
    		return 1;
    	}
        else
        {
            return (Get_VGAMODE_INTERLACE(_bDviTiming));
        }
    }
    else
    {
        return (0);
    }
}
void vDvi2WaitResStable(UINT16 u2HdmiBase,UINT8 u1Type)//0:mian,1:sub
{
	UINT32 wvtemp;
	UINT32 whtemp;
	UINT32 wDviWidthTemp;
	UINT32 wDviHeightTemp;

	UINT8  bWidthStableCnt;
	UINT8  bHeightStableCnt;
	E_HDMI_SWITCH_NUM activePort;
	
	whtemp = DviSta[u1Type]._wDviHtotal;
	wvtemp = DviSta[u1Type]._wDviVtotal;
	wDviWidthTemp = DviSta[u1Type]._wDviWidth;
	wDviHeightTemp = DviSta[u1Type]._wDviHeight;
	bWidthStableCnt = DviSta[u1Type]._bWidthStableCnt;
	bHeightStableCnt= DviSta[u1Type]._bHeightStableCnt;
	activePort = u1Type>0 ? eActiveHdmiPort1 : eActiveHdmiPort;

	if(((whtemp >= (dwDVI2GetHtotal(u2HdmiBase) - DVI2_TMP_THR)) && (whtemp<= (dwDVI2GetHtotal(u2HdmiBase) + DVI2_TMP_THR))) 
	&&((wvtemp >= (dwDVI2GetVtotal(u2HdmiBase) - DVI2_TMP_THR)) && (wvtemp <= (dwDVI2GetVtotal(u2HdmiBase) + DVI2_TMP_THR))))
	{

		if(wDviWidthTemp == dwDVI2GetWidth(u2HdmiBase))
		{
			bWidthStableCnt++;
			LOG(3, "+_bWidthStableCnt %d \n", bWidthStableCnt);
		}
		else
		{
			bWidthStableCnt =0;
		}

		if(wDviHeightTemp == dwDVI2GetHeight(u2HdmiBase))
		{
			bHeightStableCnt++;
			LOG(3, "+_bHeightStableCnt %d \n", bHeightStableCnt);
		}
		else
		{
			bHeightStableCnt=0;
		}
		// check if timing
		if ((bWidthStableCnt >= 30) && (bHeightStableCnt >= 30) && (bHDMI2GetStateMachine(activePort) == HDMI_STABLE_STATE) )
		{
			bWidthStableCnt =0;
			bHeightStableCnt =0;
			DviSta[u1Type]._wDviHtotal = dwDVI2GetHtotal(u2HdmiBase);
			DviSta[u1Type]._wDviVtotal = dwDVI2GetVtotal(u2HdmiBase);
			DviSta[u1Type]._wDviWidth  = dwDVI2GetWidth(u2HdmiBase);
			DviSta[u1Type]._wDviHeight = dwDVI2GetHeight(u2HdmiBase);
			DviSta[u1Type]._wDviPixclk = dwDvi2PIXClock(u2HdmiBase);
			DviSta[u1Type]._bDviVclk = dwDvi2HSClock(u2HdmiBase);
			DviSta[u1Type]._wDviHclk = dwDvi2VSClock(u2HdmiBase);
			_IsDviDetectDone = FALSE;
			vDvi2Initial(u1Type);
			vDvi2SetModeChange(u1Type);
		}
	}
	DviSta[u1Type]._wDviHtotal = dwDVI2GetHtotal(u2HdmiBase);
	DviSta[u1Type]._wDviVtotal = dwDVI2GetVtotal(u2HdmiBase);
	DviSta[u1Type]._wDviWidth  = dwDVI2GetWidth(u2HdmiBase);
	DviSta[u1Type]._wDviHeight = dwDVI2GetHeight(u2HdmiBase);
	DviSta[u1Type]._bWidthStableCnt = bWidthStableCnt;
	DviSta[u1Type]._bHeightStableCnt = bHeightStableCnt;

}

BOOL fgDvi2IsResChange(UINT16 u2HdmiBase,UINT8 u1Type)//0:mian,1:sub
{
	UINT32 wvtemp;
	UINT32 whtemp;
	UINT32 wDviWidthTemp;
	UINT32 wDviHeightTemp;
	UINT32 wDviPclkTemp;
	BOOL ret;
	
	whtemp = DviSta[u1Type]._wDviHtotal;
	wvtemp = DviSta[u1Type]._wDviVtotal;
	wDviWidthTemp = DviSta[u1Type]._wDviWidth;
	wDviHeightTemp = DviSta[u1Type]._wDviHeight;
	wDviPclkTemp  = DviSta[u1Type]._wDviPixclk;

	if(((dwDVI2GetHtotal(u2HdmiBase) >= (whtemp - 5))&& (dwDVI2GetHtotal(u2HdmiBase) <= (whtemp + 5))) 
	  &&((dwDVI2GetVtotal(u2HdmiBase) >= (wvtemp - 2)) && (dwDVI2GetVtotal(u2HdmiBase) <= (wvtemp + 2))))
	{
		DviSta[u1Type]._bModeChgCnt=0;
	}
	else
	{
		DviSta[u1Type]._bModeChgCnt++;
		LOG(1, "mc: h/v total unstable %d \n",DviSta[u1Type]._bModeChgCnt);
	}

	if(((dwDVI2GetWidth(u2HdmiBase) >= (wDviWidthTemp - 3))&& (dwDVI2GetWidth(u2HdmiBase)<= (wDviWidthTemp + 3))) 
	  &&((dwDVI2GetHeight(u2HdmiBase) >= (wDviHeightTemp- 3)) && (dwDVI2GetHeight(u2HdmiBase) <= (wvtemp + 3))))
	{
		DviSta[u1Type]._bDeChgCnt =0;
	}
	else
	{
		DviSta[u1Type]._bDeChgCnt ++;
		LOG(1, "mc: h/v active unstable %d \n",DviSta[u1Type]._bDeChgCnt);
	}

	if(((dwDvi2PIXClock(u2HdmiBase) >= (wDviPclkTemp - wDviPclkTemp/10))&& (dwDvi2PIXClock(u2HdmiBase) <= (wDviPclkTemp + wDviPclkTemp/10))))
	{
		DviSta[u1Type]._bPixelClkChgCnt =0;
	}
	else
	{
		DviSta[u1Type]._bPixelClkChgCnt ++;
		LOG(1, "mc: pixel clk unstable %d \n",DviSta[u1Type]._bPixelClkChgCnt);
	}

	if((DviSta[u1Type]._bModeChgCnt >1) || (DviSta[u1Type]._bDeChgCnt >1) ||(DviSta[u1Type]._bPixelClkChgCnt>1))
	{
		ret=TRUE;
		DviSta[u1Type]._bPixelClkChgCnt =0;
		DviSta[u1Type]._bModeChgCnt=0;
		DviSta[u1Type]._bDeChgCnt =0;
	}
	else
	{
		ret=FALSE;
	}

	DviSta[u1Type]._wDviHtotal = dwDVI2GetHtotal(u2HdmiBase);
	DviSta[u1Type]._wDviVtotal = dwDVI2GetVtotal(u2HdmiBase);
	DviSta[u1Type]._wDviWidth  = dwDVI2GetWidth(u2HdmiBase);
	DviSta[u1Type]._wDviHeight = dwDVI2GetHeight(u2HdmiBase);
	DviSta[u1Type]._wDviPixclk = dwDvi2PIXClock(u2HdmiBase);

	return ret;

}

void vDvi2CheckColorRelatedChg(UINT8 u1Type)
{
	UINT8 activePot;
	UINT8 _bHDMIScanInfo;
	UINT8 _bHDMIAspectRatio;
	UINT8 _bHDMIAFD;
	UINT8 _bHDMIHDCP1Status;
	UINT8 _bHDMIHDCP2Status;
	UINT8 _bHDMI422Input;
	UINT8 _bHDMIITCFlag;
	UINT8 _bHDMIITCContent;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	_bHDMIScanInfo    = Hdmi2RxObj[activePot]._HdmiScan ;
	_bHDMIAspectRatio = Hdmi2RxObj[activePot]._HdmiAspect;
	_bHDMIAFD         =	Hdmi2RxObj[activePot]._HdmiAFD;
	_bHDMIITCFlag     =	Hdmi2RxObj[activePot]._HdmiITC.fgItc;
	_bHDMIITCContent  = Hdmi2RxObj[activePot]._HdmiITC.ItcContent;
	_bHDMIHDCP1Status = Hdmi2RxObj[activePot]._bHDMIHdcp1xGood;
	_bHDMIHDCP2Status = Hdmi2RxObj[activePot]._bHDMIHdcp2xGood;
	_bHDMI422Input    = Hdmi2RxObj[activePot]._bIs422;
			
	if((_bHDMIScanInfo != bHDMI2ScanInfo(activePot)) || (_bHDMIAspectRatio != bHDMI2AspectRatio(activePot)) || (_bHDMIAFD != bHDMI2AFD(activePot))
	  || (_bHDMI422Input != bHDMI2422Input(activePot)) || (_bHDMIITCFlag != bHDMI2ITCFlag(activePot)) || (_bHDMIITCContent != bHDMI2ITCContent(activePot)) 
	  || (_bHDMIHDCP1Status != bHDMIHDCP1StatusGet(activePot)) || (_bHDMIHDCP2Status != bHDMIHDCP2StatusGet(activePot)))
	{
	   if(DviSta[u1Type]._bInfoChgCnt++ > 3)
	   {
		   DviSta[u1Type]._bInfoChgCnt = 0;
		   Hdmi2RxObj[activePot]._HdmiScan        = bHDMI2ScanInfo(activePot);
		   Hdmi2RxObj[activePot]._HdmiAspect      = bHDMI2AspectRatio(activePot);
		   Hdmi2RxObj[activePot]._HdmiAFD         = bHDMI2AFD(activePot);
		   Hdmi2RxObj[activePot]._bHDMIHdcp1xGood = bHDMIHDCP1StatusGet(activePot);
		   Hdmi2RxObj[activePot]._bHDMIHdcp2xGood = bHDMIHDCP2StatusGet(activePot);
		   Hdmi2RxObj[activePot]._HdmiITC.fgItc   = bHDMI2ITCFlag(activePot);
		   Hdmi2RxObj[activePot]._HdmiITC.ItcContent = bHDMI2ITCContent(activePot);
		   Hdmi2RxObj[activePot]._bIs422          = bHDMI2422Input(activePot);

		   if(_rDVIStat.bIsMain)
		   {
			   if(DviSta[u1Type]._bDviTiming == MODE_NOSUPPORT)
			   {
				   vApiNotifyResolutionChg(SV_VP_MAIN, SV_VDO_NOSUPPORT);
			   }
			   else
			   {
				   vApiNotifyResolutionChg(SV_VP_MAIN, SV_VDO_STABLE);
			   }
		   }
		   else
		   {
			   if(DviSta[u1Type]._bDviTiming == MODE_NOSUPPORT)
			   {
				   vApiNotifyResolutionChg(SV_VP_PIP, SV_VDO_NOSUPPORT);
			   }
			   else
			   {
				   vApiNotifyResolutionChg(SV_VP_PIP, SV_VDO_STABLE);
			   }
		   }
	   }
	}
	else
	{
	   DviSta[u1Type]._bInfoChgCnt = 0;
	}
}

extern UINT8 u1HdmiColorMode;
UINT8 bGetSupportHdmi2ModeVideoTiming(UINT8 bPath)
{
    UINT8  _u8Timing = 0;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8  activePot;
    UINT16 _u16Width;
    UINT16 _u16Height;
    UINT16 _u16Rate;

	activePot = bPath ? eActiveHdmiPort1 : eActiveHdmiPort;

	if(activePot != HDMI_SWITCH_4)
	{
		return bGetSupportHdmiModeVideoTiming();
	}
	else
	{
		#ifdef CC_HDMI_OLD_NEW_SUPPORT
		if(!fgHDMI2GetDuleMode(activePot))
		{
			return bGetSupportHdmiModeVideoTiming();
		}
		#endif
	}
	
	u4TmpHdmiRegBase = u4GetHdmi2RegBase(activePot);
	
	_u16Width = dwHDMI2ComHActive(u4TmpHdmiRegBase);
    _u16Height = dwHDMI2ComVActive(u4TmpHdmiRegBase);
    _u16Rate = dwHDMI2ComFrameRate(u4TmpHdmiRegBase);
	
    if((_u16Width == 1280) && (_u16Height == 720))
    {
        if(_u16Rate == 60)
        {
            _u8Timing = MODE_720p_60;
        }
        else if(_u16Rate == 50)
        {
            _u8Timing = MODE_720p_50;
        }
    }
    else if ((_u16Width == 1920) && (_u16Height == 1080) && \
             (Field_Alternative != bDvi2Check3DFormat(bPath)))
    {
#if defined(CHANGHONG_ANDROID_PICTURE)
        if (DviSta[bPath]._bDviTiming == MODE_DE_MODE)
        {
            // for DE mode , only support progressive signal because interlace signal need MDDI fine tune
            _u8Timing= DviSta[bPath]._bDviTiming;
        }
        else 
#endif
        if(_u16Rate == 60)
        {
            _u8Timing = MODE_1080p_60;
        }
        else if(_u16Rate == 50)
        {
            _u8Timing = MODE_1080p_50;
        }
        else if(_u16Rate == 30)
        {
            if ((DviSta[bPath]._wDviHtotal >= 2600) && (DviSta[bPath]._wDviHtotal <= 2680))
           {
                _u8Timing = MODE_1080P_30_2640H;
            }
            else
            {
            _u8Timing = MODE_1080p_30;
        }
        }
        else if(_u16Rate == 25)
        {
            _u8Timing = MODE_1080p_25;
        }
        else if(_u16Rate == 24)
        {
            _u8Timing = MODE_1080p_24;
        }
    }
    else if ((_u16Width == 720) && ((_u16Height == 480) || (_u16Height == 576)) && \
             (Field_Alternative != bDvi2Check3DFormat(bPath)))
    {
        if((_u16Rate == 60) && (_u16Height == 480))
        {
            _u8Timing = MODE_480P;
        }
        else if((_u16Rate == 50) && (_u16Height == 576))
        {
            _u8Timing = MODE_576P;
        }
    }
    else
    {
        if(!bDvi2Interlace(bPath))
        {
            _u8Timing = DviSta[bPath]._bDviTiming;
        }
    }

    return _u8Timing ;
}

void vDVI2FixColor(UINT8 bPath)
{
    if(bGetSupportHdmi2ModeVideoTiming(bPath) != 0)
    {
        #if defined(CHANGHONG_ANDROID_PICTURE)
        if(bGetSupportHdmi2ModeVideoTiming(bPath) <= HDTV_SEARCH_END)//add by wsf for vodeotiming not support pc mode
            return;
        #endif      
        
        switch(u1HdmiColorMode)
        {
            case SV_HDMI_MODE_AUTO:
                break;

            case SV_HDMI_MODE_GRAPHIC:
                DviSta[bPath]._bDviTiming = MODE_DE_MODE;
                break;

            case SV_HDMI_MODE_VIDEO:
                DviSta[bPath]._bDviTiming = bGetSupportHdmi2ModeVideoTiming(bPath);
                break;

            default:
                break;
        }
    }
}
void vDvi2UnplugCheck(UINT8 u1Type,HdmiRxPort ePort)
{
	if(u1HDMI2IsUnplug(ePort))
	{
		vHDMI2ClrUnplug(ePort);
		LOG(1, "mc: from activity detected to no signal when _bUnplugFlag\n");
		vDvi2Initial(u1Type);
		DviSta[u1Type]._bDviChkState = DVI_NO_SIGNAL ;
		DviSta[u1Type]._bDviTiming = MODE_NOSIGNAL;
		vDvi2SetModeChange(u1Type);
		vDvi2SetModeDone(u1Type);
	}
}

UINT8 u1IsHdmi2Cap=0;
void vDvi2DualModeCheck(UINT8 u1Type,HdmiRxPort ePort)
{
	if(!fgHDMI20CheckCap() && fgHDMI2GetDuleMode(ePort) &&(u1IsHdmi2Cap ==0))
	{
		LOG(0, "mc: from check cap to no signal when HDMI2.0\n");
		u1IsHdmi2Cap =1;
		vDvi2Initial(u1Type);
		DviSta[u1Type]._bDviChkState = DVI_NO_SIGNAL ;
		DviSta[u1Type]._bDviTiming = MODE_NOSIGNAL;
		vDvi2SetModeChange(u1Type);
		vDvi2SetModeDone(u1Type);
	}
}
void vDvi2ChkModeChange(UINT8 u1Type)
{
	UINT8 _bDviChkState;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	u4TmpHdmiRegBase = u4GetHdmi2RegBase(activePot);
	_bDviChkState = DviSta[u1Type]._bDviChkState;
	
	vDvi2UnplugCheck(u1Type,activePot);
	vDvi2DualModeCheck(u1Type,activePot);

	if(!(fgIsMainDVI() || fgIsPipDVI()) || fgIsDviFlgSet(DVI_DEBUG_MODE))
    {
    	vCCIRSelW2D(u1Type,FALSE);
    }
	
	if(!((bHDMI2GetStateMachine(activePot) == HDMI_STABLE_STATE) || (bHDMI2GetStateMachine(activePot) == HDMI_WAIT_CONNECT_STATE)|| u1HDMI2IsSyncLose(activePot)) || u1IsHdmi2Cap)
	{
		return;
	}

	if(_bDviChkState != DVI_NO_SIGNAL)
	{
		if(!fgHDMI2ComGetSCDT(u4TmpHdmiRegBase))
		{
			if(++DviSta[u1Type]._bNoSigalCnt > 200)
			{
				LOG(1,"mc: from activity detected to force mode done\n");
				_bDviChkState = DVI_NO_SIGNAL;
				vDvi2SetModeChange(u1Type);
				vDvi2Initial(u1Type);
				DviSta[u1Type]._bDviTiming = MODE_NOSIGNAL;
				DviSta[u1Type]._bNoSigalCnt =0;
				vDvi2SetModeDone(u1Type);
			}
		}
	}
    switch(_bDviChkState)
    {
        case DVI_NO_SIGNAL :
            if(fgHDMI2ComGetSCDT(u4TmpHdmiRegBase))
            {
              	LOG(1,"mc: from no signal to wait stable\n");
                _bDviChkState = DVI_WAIT_STABLE ;
            }
            else
            {
            	DviSta[u1Type]._bNoConnectCnt ++;
            	if(DviSta[u1Type]._bDviModeChgFlg && (DviSta[u1Type]._bNoConnectCnt >3))
                {
                    LOG(1, "mc: DVI_NO_SIGNAL to force mode done \n");
                    DviSta[u1Type]._bNoConnectCnt = 0;
                    DviSta[u1Type]._bDviTiming = MODE_NOSIGNAL;;
                    vDvi2SetModeDone(u1Type);
                }
            }
            break;

        case DVI_CHK_MODECHG:
			if(_bQueryMode!=_bCurQueryMode)
            {
                _bQueryMode=_bCurQueryMode;
                _bDviChkState = DVI_WAIT_STABLE;
                vDvi2SetModeChange(u1Type);
				vDvi2Initial(u1Type);
            }
			if(bHDMI2InputType(activePot) != _bHDMIColorSpace)
            {
                vHDMI2VideoMute(eActiveHdmiPort,1);//mute

				if(fgIsMainDVI())
				{
				    vDVI2FixColor(SV_VP_MAIN);
				}

				if(fgIsPipDVI())
				{
				    vDVI2FixColor(SV_VP_PIP);
				}
				_bHDMIColorSpace = bHDMI2InputType(activePot);
                
                LOG(0, "mc: color space change and trigger mode change bHDMI2InputType() =%d \n", bHDMIInputType());
                _bDviChkState = DVI_WAIT_STABLE ;
				
				vDvi2Initial(SV_VP_MAIN);
				vDvi2SetModeChange(SV_VP_MAIN);

				DviSta[SV_VP_PIP]._bDviChkState = DVI_WAIT_STABLE;
				vDvi2Initial(SV_VP_PIP);
				vDvi2SetModeChange(SV_VP_PIP);
            }
			vDvi2CheckColorRelatedChg(u1Type);
			if(fgDvi2IsResChange(u4TmpHdmiRegBase,u1Type) || (_bHdmiMD != fgHDMI2ComGetHdmiMode(u4TmpHdmiRegBase)))
			{
				if(fgIsMainDVI())
				{
					_vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_MAIN, FOREVER_MUTE, FALSE);
				}
				else if(fgIsPipDVI())
				{
					_vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_PIP, FOREVER_MUTE, FALSE);
				}

				if(_bHdmiMD != fgHDMI2ComGetHdmiMode(u4TmpHdmiRegBase))
				{
					_bHdmiMD = fgHDMI2ComGetHdmiMode(u4TmpHdmiRegBase);
					LOG(1,"mc:HDMI mode change to %d___\n",_bHdmiMD);
				}

				if(u1HDMI2IsSyncLose(activePot))
				{
					vHDMI2SetSyncLose(activePot,0);
					LOG(1,"mc:sync lost___\n");
				}

				x_memset(&DviSta[u1Type],0,sizeof(DviStatus_t));
				_bDviChkState = DVI_WAIT_STABLE ;
				vDvi2Initial(u1Type);
				vDvi2SetModeChange(u1Type);
			}
            break;
		case DVI_WAIT_STABLE:
			if(_bHdmiMD != fgHDMI2ComGetHdmiMode(u4TmpHdmiRegBase))
			{
				_bHdmiMD = fgHDMI2ComGetHdmiMode(u4TmpHdmiRegBase);
			}
			vDvi2WaitResStable(u4TmpHdmiRegBase,u1Type);
			break;
        default:
            break;
   }

   DviSta[u1Type]._bDviChkState = _bDviChkState;
}

UINT8 bDvi2StdTimingSearch(UINT8 bMode,UINT8 u1Type)
{
    UINT8 bSearch;
    UINT8 bSearchEnd;
    UINT16 wTmp;
	UINT8   _bDviVclk;
    UINT16  _wDviHclk; // timing table: table value; otherwise, the value used by timing search
	UINT16  _wDviVTotal;
	UINT16  _wDviHtotal;
	UINT32 u4TmpHdmiRegBase = 0;
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;

	_wDviHtotal = DviSta[u1Type]._wDviHtotal;
	_wDviVTotal = DviSta[u1Type]._wDviVtotal;
	_bDviVclk = DviSta[u1Type]._bDviVclk ;
	_wDviHclk =	DviSta[u1Type]._wDviHclk;

   if(u1Type)
   {
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
   }
   else
   {
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
   }
    if(bMode)
    {
        // Video Mode
        bSearch = HDTV_SEARCH_START;
        bSearchEnd = HDTV_SEARCH_END;
    }
    else
    {
        // VGA Mode
        bSearch = DVI_SEARCH_START;
        bSearchEnd = DVI_SEARCH_END;
    }

#if 1
    LOG(3, "vsync frequency: %d\n", _bDviVclk);
    LOG(3, "hsync frequency: %d\n", _wDviHclk);
    LOG(3, "Htotal: %d\n", _wDviHtotal);
#endif

    do
    {
#ifdef CC_BYPASS_AVI_INFOFRAME
        if(Get_HDMIMODE_SupportVideo(bSearch) == TRUE)
#endif
        {
            if((_bDviVclk >= (Get_VGAMODE_IVF(bSearch) - 2)) &&
               (_bDviVclk <= (Get_VGAMODE_IVF(bSearch) + 2)))
            {
                if ((_wDviHclk >= (Get_VGAMODE_IHF(bSearch) - 17)) &&
                (_wDviHclk <= (Get_VGAMODE_IHF(bSearch) + 17)))
                {
                    // check interlaced
                    if((!bDvi23DPacketVaild(u1Type) && ((Get_VGAMODE_INTERLACE(bSearch)) != (fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase)))))
                    {
                        LOG(3,"Get_VGAMODE_INTERLACE(bSearch): %d\n", Get_VGAMODE_INTERLACE(bSearch));
                        LOG(3,"fgHDMIinterlaced: %d\n", fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase));
                        continue;
                    }
                        wTmp = _wDviHtotal ;
                        LOG(3," timing search 3\n");
                        //vDSSetCLKMeasure();
                        LOG(3,"bSearch= %d\n", bSearch);
                        LOG(3, "_wDviHtotal= %d\n", _wDviHtotal);
                        LOG(3,"Get_VGAMODE_IHTOTAL= %d\n", Get_VGAMODE_IHTOTAL(bSearch));
                        LOG(3,"bDVIGetHeight()= %d\n", dwDVI2GetHeight(u4TmpHdmiRegBase));
                        LOG(3,"Get_VGAMODE_IPV_LEN=%d\n", Get_VGAMODE_IPV_LEN(bSearch));
						LOG(3,"fgHDMIinterlaced: %d\n", fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase));

                        if((wTmp > (Get_VGAMODE_IHTOTAL(bSearch) - 40)) &&
                           (wTmp < (Get_VGAMODE_IHTOTAL(bSearch) + 40)))
                        {
                            if(((dwDVI2GetHeight(u4TmpHdmiRegBase) >>(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_FieldAlternative) ) > (Get_VGAMODE_IPV_LEN(bSearch) - 3)) &&
                               ((dwDVI2GetHeight(u4TmpHdmiRegBase) >>(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_FieldAlternative)) < (Get_VGAMODE_IPV_LEN(bSearch) + 3)))
                            {
                            #if 1
                                LOG(3,"bSearch=%d\n",bSearch);
                                LOG(3,"UNRECRX0_0=%x\n",u1IO32Read1B(UNRECRX0_0 + u4ActiveHdmiRegBase));
                                LOG(3,"INTR_STATE0=%x\n",IO32ReadFldAlign(INTR_STATE0 + u4ActiveHdmiRegBase, INTR3_NEW_UNREC));
                                LOG(3,"UNRECRX1_3=%x\n",u1IO32Read1B(UNRECRX1_3 + u4ActiveHdmiRegBase) >> 5);
						    #endif
                                if(bDvi23DPacketVaild(u1Type))
                                {
                                    if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure ==HDMI_3D_Structure_FramePacking)
                                    {
                                        if(bSearch==MODE_576P_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_1440_576p_50_FP;
                                           LOG(3," 3D FramePacking, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                    }
                                    else if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure ==HDMI_3D_Structure_SideBySideFull)
                                    {
                                        if(bSearch==MODE_525I_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_480I_60_SBS_FULL;
                                            LOG(3," 3D side-by-side full, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                        else if(bSearch==MODE_625I_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_576I_50_SBS_FULL;
                                            LOG(1, " 3D side-by-side full, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                        else if(bSearch==MODE_480P_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_480p_60_SBS_FULL;
                                            LOG(3," 3D side-by-side full, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                        else if(bSearch==MODE_576P_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_576p_50_SBS_FULL;
                                            LOG(3," 3D side-by-side full, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                    }
									else if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure ==HDMI_3D_Structure_LineAlternative)
                                    {
                                        if(bSearch==MODE_480P_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_240p_60_LA;
                                            LOG(3," 3D line alternative, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                        else if(bSearch==MODE_576P_OVERSAMPLE)
                                        {
                                            bSearch=MODE_3D_288p_50_LA;
                                            LOG(3," 3D line alternative, timing table: %d\n", bSearch);
                                            return bSearch;
                                        }
                                    }
									else if (Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure ==HDMI_3D_Structure_FieldAlternative)
									{
										if(bSearch==MODE_480P_OVERSAMPLE)
										{
											bSearch=MODE_3D_480I_60_FA;
											LOG(3," 3D FieldAlternative, timing table: %d\n", bSearch);
											return bSearch;
										}
										else if(bSearch==MODE_576P_OVERSAMPLE)
										{
											bSearch=MODE_3D_576I_50_FA;
											LOG(3," 3D FieldAlternative, timing table: %d\n", bSearch);
											return bSearch;
										}
										else if(bSearch==MODE_1080p_50)
										{
											bSearch=MODE_3D_1080I_50_FA;
											LOG(3," 3D FieldAlternative, timing table: %d\n", bSearch);
											return bSearch;
										}
										else if(bSearch==MODE_1080p_60)
										{
											bSearch=MODE_3D_1080I_60_FA;
											LOG(3," 3D FieldAlternative, timing table: %d\n", bSearch);
											return bSearch;
										}									
									}
									else if (Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure ==HDMI_3D_Structure_SideBySideHalf)
									{
										if(bSearch==MODE_525I)
										{
											bSearch=MODE_3D_480I_60_SBS_HALF;
											LOG(3," 3D SBS half, timing table: %d\n", bSearch);
											return bSearch;
										}
										else if(bSearch==MODE_625I)
										{
											bSearch=MODE_3D_576I_50_SBS_HALF;
											LOG(3," 3D SBS half, timing table: %d\n", bSearch);
											return bSearch;
										}									
										else if(bSearch==MODE_1080i)
										{
											bSearch=MODE_3D_1080I_60_SBS_HALF;
											LOG(3," 3D SBS half, timing table: %d\n", bSearch);
											return bSearch;
										}	
										else if(bSearch==MODE_1080i_50)
										{
											bSearch=MODE_3D_1080I_50_SBS_HALF;
											LOG(3," 3D SBS half, timing table: %d\n", bSearch);
											return bSearch;
										}
									}

                                    LOG(3," 3D timing table: %d\n", bSearch);
                                    return bSearch;
                                }
                                else
                                {
                                    LOG(3," timing table: %d\n", bSearch);
                                    return bSearch;
                                }
                            }
                        }
                }
            }
        }
    }
    while(++bSearch <= bSearchEnd);

    return MODE_NOSUPPORT;
}

void vDvi2SetModeDone(UINT8 u1Type)
{
	UINT8 activePot;

	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	if(!DviSta[u1Type]._bDviModeChgFlg)
	{
		return;
	}
	
    _bHDMIColorSpace = bHDMI2InputType(activePot);
    if(u1Type == SV_VP_MAIN )
    {
        _vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_MAIN, 0, FALSE);
        vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
#if 1
        LOG(0, "Main Mode Done\n");
#endif
    }
	else
    {
        _vDrvVideoSetMute(MUTE_MODULE_HDMI, SV_VP_PIP, 0, FALSE);
        vSetPipFlg(PIP_FLG_MODE_DET_DONE);
#if 1
        LOG(0, "Pip Mode Done\n");
#endif
    }
	DviSta[u1Type]._bDviModeChgFlg = 0;
}

PRIVATE void vDvi2Set3DPosition(UINT8 u1Type,UINT16 wVSyncWidth)
{
	UINT8 activePot;
	UINT16 wVactive, wVblank;
	UINT8 _bDviTiming = DviSta[u1Type]._bDviTiming;
	UINT16 _wDviWidth = DviSta[u1Type]._wDviWidth;
	UINT16 _wDviHeight = DviSta[u1Type]._wDviHeight;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	if(bDvi23DPacketVaild(u1Type))
	{
		Printf("-----3D playing mode, HDMI_3D_Structure = %d-----\n", Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure);
		if(bCCIRCENStatus() || (1 == RegReadFldAlign(CCIR_00, CCH_CLK_DIV)))
		{
			_wDviWidth /= 2;
		}
		switch(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure)
		{
		    case HDMI_3D_Structure_FramePacking:
		     
		       	wVactive = (Get_VGAMODE_IPV_LEN(_bDviTiming) - Get_VGAMODE_IPV_STA(_bDviTiming)) / 2;
		        wVblank = Get_VGAMODE_IPV_STA(_bDviTiming);
				
		        vCCIR2SetAvStart(0);
		        vCCIR2SetAvWidth(_wDviWidth/2);
		        vCCIR2SetVsStart(wVSyncWidth - 2);
		        vCCIR2SetVdeWidth(wVactive);
		        break;

		case HDMI_3D_Structure_FieldAlternative:

				if((_bDviTiming == MODE_3D_480I_60_FA) || (_bDviTiming == MODE_3D_576I_50_FA))
				{
					_wDviWidth /= 2;
				}
		        vCCIR2SetAvStart(0);
		        vCCIR2SetAvWidth(_wDviWidth/2);

#if WA_3D_FA
		        vCCIR2SetVsStart( wVSyncWidth - 3 + 5);
#else
		        vCCIR2SetVsStart( wVSyncWidth - 3);
#endif
		        vCCIR2SetVdeWidth(_wDviHeight/2);
			break;
		    case HDMI_3D_Structure_LineAlternative:
		    case HDMI_3D_Structure_SideBySideFull:
		    case HDMI_3D_Structure_SideBySideHalf:
		    case HDMI_3D_Structure_TopBottom:
		    default:
		        vCCIR2SetAvStart(0);
		        vCCIR2SetAvWidth(_wDviWidth/2);
		        vCCIR2SetVsStart(wVSyncWidth - 3);
		        vCCIR2SetVdeWidth((_wDviHeight >> bDvi2GetInterlaced(u1Type)));
		        break;
		}
	}

}
PRIVATE void vDvi2SetInputCapture(UINT8 u1Type)
{
   UINT8 activePot;
   UINT16 u1Mvslen=0;
   UINT16 u1Svslen=0;
   UINT32 u4TmpHdmiRegBase = 0;
   

   activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
   
   if(u1Type)
   {
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
   }
   else
   {
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
   }

   if(u1Type == SV_VP_MAIN)
   {
   		if (fgHDMI2IsPixelRepeate(activePot))//down sample
	    {
	        if(bHDMI2422Input(activePot))
	        {
	            vIO32WriteFldAlign(CCIR_00, 0x0, CCH_CEN_SEL);
	            vIO32WriteFldAlign(CCIR_00, 0x1, CCH_CLK_DIV);
	        }
	        else
	        {
	            vIO32WriteFldAlign(CCIR_00, 0x1, CCH_CEN_SEL);
	            vIO32WriteFldAlign(CCIR_00, 0x0, CCH_CLK_DIV);
	        }
	    }
		else
		{
			 vIO32WriteFldAlign(CCIR_00, 0x0, CCH_CEN_SEL);
	         vIO32WriteFldAlign(CCIR_00, 0x0, CCH_CLK_DIV);
		}

		if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
		{
			vIO32WriteFldAlign(LPF_PIP_00, 1, CCH_FIX_VCNT);
			vIO32WriteFldAlign(SYS_02, 1, CCH_VCNT_OFFSET_F0);
			vIO32WriteFldAlign(SYS_02, 0, CCH_VCNT_OFFSET_F1);
			vIO32WriteFldAlign(SYS_01, 1, CCH_VSLEN_SEL);
			vUtDelay1ms(20);
			u1Mvslen = IO32ReadFldAlign(DVI_HDTV_04, STA_CCH_VSLEN);
			u1Mvslen += 1;
			vIO32WriteFldAlign(SYS_01, 0, CCH_VSLEN_SEL);
		}
		else
		{
			vIO32WriteFldAlign(LPF_PIP_00, 0, CCH_FIX_VCNT);
			vIO32WriteFldAlign(SYS_02, 0, CCH_VCNT_OFFSET_F0);
			u1Mvslen = IO32ReadFldAlign(DVI_HDTV_04, STA_CCH_VSLEN);
		}
		
		
		vDvi2Set3DConfig(SV_VP_MAIN);
		if(bDvi23DPacketVaild(SV_VP_MAIN))
		{
			vDvi2Set3DPosition(SV_VP_MAIN,u1Mvslen);
			return;
		}
		
		u1Mvslen -= 3;
		Printf("____main vslen =%d, vde width=%d,av width=%d__________\n",u1Mvslen,(DviSta[u1Type]._wDviHeight >> fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase)),DviSta[u1Type]._wDviWidth/2);
		vCCIR2SetAvStart(0);
		vCCIR2SetVdeWidth(DviSta[u1Type]._wDviHeight >> fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase));
		vCCIR2SetVsStart(u1Mvslen);
		if(fgHDMI2GetDuleMode(eActiveHdmiPort) && (fgHDMI2Get420Mode(eActiveHdmiPort) == FALSE))
		{
			vCCIR2SetAvWidth(DviSta[u1Type]._wDviWidth/4);
		}
		else
		{
			vCCIR2SetAvWidth(DviSta[u1Type]._wDviWidth/2);
		}
	   
   }
   else if(u1Type == SV_VP_PIP)
   {
		if(fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase))
		{
			vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_FIX_VCNT_PIP);
			vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VCNT_OFFSET_F0_PIP);
			vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_VCNT_OFFSET_F1_PIP);
			vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VSLEN_SEL_PIP);
			vUtDelay1ms(20);
			u1Svslen = IO32ReadFldAlign(CCIR_06_NEW, STA_CCH_NEW_VSLEN);
			u1Svslen += 1;
			vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_VSLEN_SEL_PIP);
		}
		else
		{
			vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_FIX_VCNT_PIP);
			vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_VCNT_OFFSET_F0_PIP);
			u1Svslen = IO32ReadFldAlign(DVI_HDTV_04, STA_CCH_VSLEN);
		}
		
		vDvi2Set3DConfig(SV_VP_PIP);
		
		u1Svslen -= 3;
		Printf("____sub vslen =%d__________\n",u1Svslen);
		vCCIR2SetNewAvStart(0);
		vCCIR2SetNewVdeWidth(DviSta[u1Type]._wDviHeight >> fgHDMI2ComGetInterlaced(u4TmpHdmiRegBase));
		vCCIR2SetNewVsStart(u1Svslen);
		if(fgHDMI2GetDuleMode(eActiveHdmiPort) && (fgHDMI2Get420Mode(eActiveHdmiPort) == FALSE))
		{
			vCCIR2SetNewAvWidth(DviSta[u1Type]._wDviWidth/4);
		}
		else
		{
			vCCIR2SetNewAvWidth(DviSta[u1Type]._wDviWidth/2);
		}

   }


}

extern UINT32 _wDviVstartDefault;
extern UINT8 _bBoundary_flag;
void vDvi2ModeDetect(UINT8 u1Type)
{
   // UINT16 Vproch_tmp;
	UINT32 u4TmpHdmiRegBase = 0;
	
	if(u1Type)
	{
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
	   u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}

	
	if(!fgDVI2GetHAct(u4TmpHdmiRegBase))
	{
		DviSta[u1Type]._bDviTiming = MODE_NOSIGNAL;
		LOG(0, "md: no signal\n");
	}
	else
	{

		DviSta[u1Type]._wDviHclk = dwDvi2HSClock(u4TmpHdmiRegBase);
		DviSta[u1Type]._bDviVclk = dwDvi2VSClock(u4TmpHdmiRegBase);
		DviSta[u1Type]._bDviTiming = bDvi2StdTimingSearch(1,u1Type);

		if(DviSta[u1Type]._bDviTiming == MODE_720p_60)
        {
            if(DviSta[u1Type]._wDviHtotal >=1660)
            {
                DviSta[u1Type]._bDviTiming = MODE_DE_MODE;
            }
        }
        else if(DviSta[u1Type]._bDviTiming == MODE_NOSUPPORT)
        {
            DviSta[u1Type]._bDviTiming = MODE_DE_MODE;
        }

		vDvi2SetInputCapture(u1Type);
		_bQueryMode=wDvi2QueryAP3DSetting(u1Type);
        _bCurQueryMode = _bQueryMode;
        LOG(0, "_bQueryMode:0x%8x\n", _bQueryMode);
		
		_wDviVstartDefault = wCCIR2GetVsStart();
#ifdef CC_SUPPORT_W2DR2R
		if(u1Type == 0)
		{	
			eW2DCfg = vDviTimingNeedW2d_V2();
			if(W2D_NO_NEED != eW2DCfg) //hdmi2.0 timing
			{
				if(W2D_USE_LEFT_AND_RIGHT == eW2DCfg)
				{
					vHDMI2SetSplit(eActiveHdmiPort,1);
				}
				else
				{
					vHDMI2SetSplit(eActiveHdmiPort,0);
				}
				vHDMIW2DR2RSetting(eActiveHdmiPort,eActiveHdmiPort);
				vCCIRSelW2D(u1Type,TRUE);
				printf("apply w2d r2r setting........\n");
			}
			else
			{
				vHDMI2SetSplit(eActiveHdmiPort,0);
			}
		}
#endif
		vDvi2SetModeDone(u1Type);

		_IsDviDetectDone = TRUE;

		DviSta[u1Type]._bDviChkState = DVI_CHK_MODECHG;
		
		if(fgIsMainDVI())
		{
		    vDVI2FixColor(SV_VP_MAIN);
		}

		if(fgIsPipDVI())
		{
		    vDVI2FixColor(SV_VP_PIP);
		}
	}
}
UINT16 wDvi2GetPorch(UINT8 bPath, UINT8 bPorchType)
{
	UINT16  wHRef, wVref , wHproch, wAVStart, wVsStart;
	UINT32 _wDviHtotal,_wDviWidth;
	UINT8 activePot;
  
   	activePot = bPath ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	_wDviHtotal = fgHDMI2Get420Mode(activePot) ? DviSta[bPath]._wDviHtotal*2 :DviSta[bPath]._wDviHtotal;
	_wDviWidth = fgHDMI2Get420Mode(activePot) ? DviSta[bPath]._wDviWidth*2 : DviSta[bPath]._wDviWidth;
	
	wHproch =  ((_wDviHtotal - _wDviWidth) >= 0) ? (_wDviHtotal - _wDviWidth) : 16;

	wVref = _wDviVstartDefault ;
	wAVStart = wCCIR2GetAvStart();
	wVsStart = wCCIR2GetVsStart();

	switch(bPorchType)
	{
	    case 	SV_HPORCH_CURRENT:
	        //if DE mode H need to add H porch
	        // if porch is neative than = _wDviHtotal - val
	        // min value = _wDviHtotal - bRef
	        if(wAVStart  >= (_wDviHtotal - wHproch))
	        {
	            // avstart - _wDviHtotal = -
	            wHRef = (wHproch  + wAVStart - _wDviHtotal) ;

	        }
	        else
	        {
	            wHRef = (wHproch  + wAVStart) ;
	        }

	        return wHRef ;

	    case 	SV_HPORCH_DEFAULT:
	        return wHproch;

	    case	SV_VPORCH_CURRENT:
	        if(_bBoundary_flag == TRUE)
	        {
	            wVref = wVsStart - 1 ;
	        }
	        else
	        {
	            wVref = wVsStart ;
	        }

	        return  wVref;

	    case	SV_VPORCH_DEFAULT:
	        return  wVref;

	    case SV_HPORCH_MAX:
	        return  2*wHproch;
	        
	case SV_VPORCH_MAX:
	    return  ((wDvi2InputVTotal(bPath) - wDvi2InputHeight(bPath)) * 2);
	                
	    default: /*MinMax*/
	        return wDrvVideoPorchStd(bPath, bPorchType);
	}
}

void vDvi2SetPorch(UINT8 bPath, UINT8 bPorchType, UINT16 wValue)
{
	UINT16 wHRef, wVref;
	UINT32 u4PixelRepeat;
	UINT16 wAVStartValue;
	UINT16 wVproch_tmp;
	E_TDTV_DECODER_OUTPUT outputtype;
	UINT32 _wDviHtotal,_wDviWidth;
	UINT8 activePot;
  
   	activePot = bPath ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	_wDviHtotal = fgHDMI2Get420Mode(activePot) ? DviSta[bPath]._wDviHtotal*2 :DviSta[bPath]._wDviHtotal;
	_wDviWidth = fgHDMI2Get420Mode(activePot) ? DviSta[bPath]._wDviWidth*2 : DviSta[bPath]._wDviWidth;

	outputtype=(E_TDTV_DECODER_OUTPUT)((_bQueryMode&0xffff0000)>>16);

	wHRef =  ((_wDviHtotal - _wDviWidth) >= 0) ? (_wDviHtotal - _wDviWidth) : 16;

	wVref = _wDviVstartDefault ;
	UNUSED(wVref);

	if(outputtype==E_TDTV_DECODER_OUTPUT_NATIVE)
	{
	    //to get if pixel repetition is on or off 1:means on

	    u4PixelRepeat = RegReadFldAlign(CCIR_00, CCH_CEN_SEL);

	    /*-----------------------set HPorch--------------------------*/
	    if(bPorchType == SV_HPORCH_CURRENT)
	    {
	        if(wValue <= wHRef)
	        {
	            if(_bBoundary_flag == TRUE)
	            {
	                _bBoundary_flag = FALSE;
	                wVproch_tmp = wDvi2GetPorch(0, SV_VPORCH_CURRENT);
	                vCCIR2SetVsStart(wVproch_tmp - 1);
	            }

	            if(u4PixelRepeat == 1)  //if pixel repetition is on, HPorch have to add 2.
	            {
	                wAVStartValue = _wDviHtotal + (wValue - wHRef) * 2;
	            }
	            else
	            {
	                wAVStartValue =_wDviHtotal + wValue - wHRef;
	            }

	        }
	        else
	        {
	            if(_bBoundary_flag == FALSE)
	            {
	                wVproch_tmp = wDvi2GetPorch(0, SV_VPORCH_CURRENT);
	                vCCIR2SetVsStart(wVproch_tmp + 1);
	                _bBoundary_flag = TRUE;
	            }

	            if(u4PixelRepeat == 1)
	            {
	                wAVStartValue = ( wValue - wHRef) * 2;
	            }
	            else
	            {
	                wAVStartValue =  wValue - wHRef ;
	            }
	        }

	        vCCIR2SetAvStart(wAVStartValue);
	    }
	    /*-----------------------set VPorch--------------------------*/
	    else
	    {
	        if(_bBoundary_flag == TRUE)
	        {
	            vCCIR2SetVsStart(wValue + 1);
	        }
	        else
	        {
	            vCCIR2SetVsStart(wValue);
	        }
	    }
	}
}
void vDvi2GetDebugMessage(UINT8 u1Type)
{
 	UINT8 activePot;
	UINT32 u4TmpHdmiRegBase = 0;
	
	activePot = u1Type ? eActiveHdmiPort1 : eActiveHdmiPort;
	
	  
	if(u1Type)
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort);
	}
	else
	{
		u4TmpHdmiRegBase = u4GetHdmi2RegBase(eActiveHdmiPort1);
	}
	
	
	if(bDvi23DPacketVaild(u1Type))
	{
		Printf("3D signal\n");
		if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_FramePacking)
        {
            Printf("HDMI_3D_Structure_FramePacking\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_LineAlternative)
        {
            Printf("HDMI_3D_Structure_LineAlternative\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_SideBySideFull)
        {
            Printf("HDMI_3D_Structure_SideBySideFull\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_LDepth)
        {
            Printf("HDMI_3D_Structure_LDepth\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_LDepthGraph)
        {
            Printf("HDMI_3D_Structure_LDepthGraph\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_TopBottom)
        {
            Printf("HDMI_3D_Structure_TopBottom\n");
        }

        if(Hdmi2RxObj[activePot]._Hdmi3DInfo.HDMI_3D_Structure == HDMI_3D_Structure_SideBySideHalf)
        {
            Printf("HDMI_3D_Structure_SideBySideHalf\n");
        }

	}
	else
	{
		Printf("2D signal");
	}

	Printf("TDTV Input Mode: 0x%8x\n", _bCurQueryMode);
    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_2D_I)
    {
        Printf("[E_TDTV_DECODER_INPUT_2D_I]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_2D_P)
    {
        Printf("[E_TDTV_DECODER_INPUT_2D_P]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_FP_I)
    {
        Printf("[E_TDTV_DECODER_INPUT_FP_I]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_FP_P)
    {
        Printf("[E_TDTV_DECODER_INPUT_FP_P]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_TB_I)
    {
        Printf("[E_TDTV_DECODER_INPUT_TB_I]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_TB_P)
    {
        Printf("[E_TDTV_DECODER_INPUT_TB_P]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_SBS_I)
    {
        Printf("[E_TDTV_DECODER_INPUT_SBS_I]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_SBS_P)
    {
        Printf("[E_TDTV_DECODER_INPUT_SBS_P]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_LI_I)
    {
        Printf("[E_TDTV_DECODER_INPUT_LI_I]\n");
    }

    if((_bCurQueryMode&0xffff) == E_TDTV_DECODER_INPUT_LI_P)
    {
        Printf("[E_TDTV_DECODER_INPUT_LI_P]\n");
    }

	printf("sw: DVI width: %d\n", wDvi2InputWidth(u1Type));
	printf("sw: DVI height: %d\n", wDvi2InputHeight(u1Type));
	printf("sw: DVI refresh rate: %d\n", bDvi2RefreshRate(u1Type));
	printf("sw: DVI interlace/progressive: %d\n", bDvi2Interlace(u1Type));
	printf("sw: DVI signal status: %x\n", bDvi2SigStatus(u1Type));
	// timing index
	printf("sw: _bDviTiming: %d\n", DviSta[u1Type]._bDviTiming);
	printf("sw: pixel clock: %d\n", dwDvi2PIXClock(u4TmpHdmiRegBase));

	printf("sw: _bDviVclk: %d\n", DviSta[u1Type]._bDviVclk);
	printf("sw: _wDviHclk: %d\n", DviSta[u1Type]._wDviHclk);
	printf("sw: vsync frequency: %d\n", dwDvi2VSClock(u4TmpHdmiRegBase));
	printf("sw: hsync frequency: %d\n", dwDvi2HSClock(u4TmpHdmiRegBase));
	printf("_bHDMIColorSpace (1=RGB , 0 =YCBCR) : %d\n", _bHDMIColorSpace);
}
