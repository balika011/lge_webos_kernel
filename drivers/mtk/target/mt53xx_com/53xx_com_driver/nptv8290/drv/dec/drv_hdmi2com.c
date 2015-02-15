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
 * @author lingling.fang@mtk.com

**/


#include "hw_hdmi2.h"
#include "drv_hdmi2.h"
#include "drv_hdmi2com.h"
#include "hw_hdmi.h"
#include "x_hal_io.h"
#include "hw_vdoin.h"
#include "hw_ckgen.h"
#include "x_ckgen.h"
#include "util.h"
#include "x_printf.h"
#include "nptv_debug.h"
#include "hw_pdwnc.h"

/* Defintions */
#define HDMI2_ANA_ADJUST_RATIO_PARA 229
#define HDMI2_HDCP2_CCHK_DONE_CHECK_COUNT 20

/* Static Variables */
static BOOL fgSAOSCDone = FALSE;
static BOOL fgAnaFirstInit = TRUE;
static HDMI2ANA_CONFIG tAnaConfig;
static BOOL fgIs5861 = TRUE;
static UINT8 u1CchkCheckCount = 0;


/*Configurable Settings*/
void vHDMI2ComAnaEQ_OS_LF_SELSet(UINT16 u2HdmiAnaBase,UINT8 u1LfSel)
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,u1LfSel,HDMI2_CFG16_RG_HDMI_SAOSC_EQ_OS_LF_SEL);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,u1LfSel,HDMI2_CFG17_RG_HDMI_SAOSC_EQ_OS_LF_SEL);
    }

    return;
}

UINT8 bHDMI2ComAnaEQ_OS_LF_SELGet(UINT16 u2HdmiAnaBase)
{
    if(fgIs5861)
    {
        return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,HDMI2_CFG16_RG_HDMI_SAOSC_EQ_OS_LF_SEL);
    }
    else
    {
        return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,HDMI2_CFG17_RG_HDMI_SAOSC_EQ_OS_LF_SEL);
    }
}

void vHDMI2ComAnaLFSelValue_Set(UINT8 u1SAOSC_LFSelValue,UINT8 u1EQOSC_LFSelValue)
{
    tAnaConfig.u1SAOSC_LFSEL = u1SAOSC_LFSelValue;
    tAnaConfig.u1EQOSC_LFSEL = u1EQOSC_LFSelValue;

    return;
}

void vHDMI2ComAnaHYBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1HYBias)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,u1HYBias,HDMI2_CFG1_RG_HDMI_HYBIAS);

    return;
}

UINT8 bHDMI2ComAnaHYBiasGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,HDMI2_CFG1_RG_HDMI_HYBIAS);
}

void vHDMI2ComAnaD2SBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1D2SBias)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,u1D2SBias,HDMI2_CFG1_RG_HDMI_D2SBIAS);

    return;
}

UINT8 bHDMI2ComAnaD2SBiasGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,HDMI2_CFG1_RG_HDMI_D2SBIAS);
}

void vHDMI2ComAnaEQBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1EQBias)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,u1EQBias,HDMI2_CFG1_RG_HDMI_EQBIAS);
    
    return;
}

UINT8 bHDMI2ComAnaEQBiasGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,HDMI2_CFG1_RG_HDMI_EQBIAS);
}

void vHDMI2ComAnaKPGainSet(UINT16 u2HdmiAnaBase,UINT8 u1KPGain)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,u1KPGain,HDMI2_CFG1_RG_HDMI_KPGAIN);
    
    return;
}

UINT8 bHDMI2ComAnaKPGainGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,HDMI2_CFG1_RG_HDMI_KPGAIN);
}

void vHDMI2ComAnaRXModeSet(UINT16 u2HdmiAnaBase,UINT8 u1RXMode)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,u1RXMode,HDMI2_CFG2_RG_HDMI_RXMODE);

    return;
}

UINT8 bHDMI2ComAnaRXModeGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,HDMI2_CFG2_RG_HDMI_RXMODE);
}

void vHDMI2ComAnaEQGainSet(UINT16 u2HdmiAnaBase,UINT8 u1EQGain)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,u1EQGain,HDMI2_CFG2_RG_HDMI_EQ_GAIN);

    return;
}

UINT8 bHDMI2ComAnaEQGainGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,HDMI2_CFG2_RG_HDMI_EQ_GAIN);
}

void vHDMI2ComAnaGainBufSet(UINT16 u2HdmiAnaBase,UINT8 u1GainBuf)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,u1GainBuf,HDMI2_CFG4_RG_HDMI_GAIN_BUF);

    return;
}

UINT8 bHDMI2ComAnaGainBufGet(UINT16 u2HdmiAnaBase)
{
    return IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,HDMI2_CFG4_RG_HDMI_GAIN_BUF);
}

void vHDMI2ComAnaData_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG8_RG_HDMI_DATA_RSTB);
    return;
}

void vHDMI2ComAnaEQALG_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    return;
}

void vHDMI2ComAnaCDR_RST(UINT16 u2HdmiAnaBase,UINT8 u1Rst) //0:reset release 1:reset
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,(u1Rst?1:0),HDMI2_CFG0_RG_HDMI_CDR_RST);
    return;
}

void vHDMI2ComAnaSAOSC_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG16_RG_HDMI_SAOSC_RSTB);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG17_RG_HDMI_SAOSC_RSTB);
    }
    return;
}

void vHDMI2ComAnaEQOSC_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG16_RG_HDMI_EQOSC_RSTB);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG17_RG_HDMI_EQOSC_RSTB);
    }
    return;
}

void vHDMI2ComAnaEQ_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG8_RG_HDMI_EQALG_LEQ_RSTB);
    return;
}

void vHDMI2ComAnaEQIncrease_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_RSTB);
    return;
}

void vHDMI2ComAnaAdjust_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb) //0:reset 1:reset release
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_RSTB);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG14_RG_HDMI_EQALG_ADJUST_RSTB);
    }
    return;
}

void vHDMI2ComAnaCKDIV2_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG1_RG_HDMI_CKDIV2_RSTB);
    return;
}

void vHDMI2ComAnaCKGEN_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,(u1Rstb?1:0),HDMI2_CFG1_RG_HDMI_CKGEN_RSTB);
    return;
}

void vHDMI2ComAnaPLL_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG5_RG_HDMI_PLL_EN);
    return;
}

void vHDMI2ComAnaDeepPLL_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable)
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG18_RG_HDMI_DEEPPLL_EN);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG19_RG_HDMI_DEEPPLL_EN);
    }
    return;
}

void vHDMI2ComAnaDebug1Ctrl_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG4_RG_HDMI_DEBUG1_CTRL_EN);
    return;
}

void vHDMI2ComAnaDebug1Cnt_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable)
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_EN);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG15_RG_HDMI_DEBUG1_CNT_EN);
    }
    return;
}

void vHDMI2ComAnaDebug1SAOSC_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable)
{
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG16_RG_HDMI_SAOSC_EN);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,(u1Enable?1:0),HDMI2_CFG17_RG_HDMI_SAOSC_EN);
    }
    return;
}

void vHDMI2ComAnaDelaySet(HDMI2ANA_DELAY_ITEM eDelayItem,UINT16 u2Delay)
{
    tAnaConfig.au2DelayRatio[eDelayItem] = u2Delay;
    return;
}

UINT16 wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_ITEM eDelayItem)
{
    return tAnaConfig.au2DelayRatio[eDelayItem];
}

void vHDMI2ComAnaSupport3GHDMI20(BOOL fgEnable)
{
    tAnaConfig.fgSupport3GHDMI20 = fgEnable;
    return;
}

void vHDMI2ComAnaForce40xMode(BOOL fgEnable)
{
    tAnaConfig.fgForce40xMode = fgEnable;
    return;
}

void vHDMI2ComAnaConTestSet(UINT16 u2HdmiAnaBase)
{
	vHDMI2ComAnaSetConMon(u2HdmiAnaBase,0x20);
	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,1,HDMI2_CFG3_RG_HDMI_TESTE);
	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,1,HDMI2_CFG0_RG_HDMI_ATEST_EN);
	if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,0x6,HDMI2_CFG17_RG_HDMI_EQALG_RESERVE);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0x6,HDMI2_CFG18_RG_HDMI_EQALG_RESERVE);
    }
	vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,0);
	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB);
	vIO32WriteFldAlign(0xf0028900,0x33,Fld(6,0,AC_MSKB0));
	
	return;
}

void vHDMI2ComAnaConControl(UINT16 u2HdmiAnaBase, UINT8 u1ConValue, UINT8 u1ConType)
{
	switch(u1ConType)
	{
		case 0:
			vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,u1ConValue,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P1);
			break;
		case 1:
			vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,u1ConValue,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P2);
			break;
		case 2:
			vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,u1ConValue,HDMI2_CFG16_RG_HDMI_DEBUG1_PAR2);
			break;
		default:
			break;
	}

	return;
}

void vHDMI2ComAnaLeqItemSet(HDMI2ANA_LEQ_ITEM *ptLeqItem)
{
    if(NULL == ptLeqItem)
    {
        Printf("%s:Error!ptLeqItem is NULL!",__FUNCTION__);
        return;
    }
    tAnaConfig.tLEQItem.fgAvg = ptLeqItem->fgAvg;
    tAnaConfig.tLEQItem.fgLeak = ptLeqItem->fgLeak;
    tAnaConfig.tLEQItem.fgLeak_Decrease = ptLeqItem->fgLeak_Decrease;
    tAnaConfig.tLEQItem.fgLeak_Increase = ptLeqItem->fgLeak_Increase;
    tAnaConfig.tLEQItem.fgLEQ_Reduce = ptLeqItem->fgLEQ_Reduce;

    return;
}

void vHDMI2ComAnaLeqItemGet(HDMI2ANA_LEQ_ITEM *ptLeqItem)
{
    if(NULL == ptLeqItem)
    {
        Printf("%s:Error!ptLeqItem is NULL!",__FUNCTION__);
        return;
    }
	ptLeqItem->fgAvg = tAnaConfig.tLEQItem.fgAvg;
	ptLeqItem->fgLeak = tAnaConfig.tLEQItem.fgLeak;
	ptLeqItem->fgLeak_Decrease = tAnaConfig.tLEQItem.fgLeak_Decrease;
	ptLeqItem->fgLeak_Increase = tAnaConfig.tLEQItem.fgLeak_Increase;
	ptLeqItem->fgLEQ_Reduce = tAnaConfig.tLEQItem.fgLEQ_Reduce;
    
    return;
}

void vHDMI2ComAnaDelayInit(void)
{
    UINT8 i;
    for(i=0;i<HDMI2ANA_DELAY_MAX;i++)
    {
        tAnaConfig.au2DelayRatio[i] = 10; //ratio*10% = 1
    }
    
    return;
}

void vHDMI2ComAnaLeqItemInit(void)
{
    tAnaConfig.tLEQItem.fgAvg = FALSE;
    tAnaConfig.tLEQItem.fgLeak = TRUE;
    tAnaConfig.tLEQItem.fgLeak_Decrease = TRUE;
    tAnaConfig.tLEQItem.fgLeak_Increase = FALSE;
    tAnaConfig.tLEQItem.fgLEQ_Reduce = FALSE;

    return;
}

void vHDMI2ComAnaDebugResetPar(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG1_PAR1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG1_PAR2);

    return;
}

void vHDMI2ComAnaEQMacroInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_DATA_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    
    return;
}

void vHDMI2ComAnaSAOSCInit(UINT16 u2HdmiAnaBase) 
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,1,HDMI2_CFG0_RG_HDMI_CDR_RST);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_DES_HF_MODE);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaDebug1SAOSC_EN(u2HdmiAnaBase,0);

    return;
}

void vHDMI2ComAnaEQOSCInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,1,HDMI2_CFG0_RG_HDMI_CDR_RST);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_EQ_CALEN);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,0);
    
    return;
}

void vHDMI2ComAnaEQDIGInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,8,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_VAL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,8,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_GAIN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_GAIN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xc,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_TARGET);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,4,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_AVG_GAIN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,4,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_GAIN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0xf,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_THRESHOLD);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_TARGET);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0x10,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_TOP_LIM);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG10 + u2HdmiAnaBase,0,HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_BOT_LIM);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG10 + u2HdmiAnaBase,2,HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_STEP);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG10 + u2HdmiAnaBase,0,HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_INIT_VAL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG10 + u2HdmiAnaBase,0xa,HDMI2_CFG10_RG_HDMI_EQALG_LEQ_REDUCE_ADJUST_THRESHOLD);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,8,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_EN);

    return;
}

void vHDMI2ComAnaEQIncreaseInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,8,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_GAIN_P1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,8,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_GAIN_P2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,0,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,0,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_INIT_VAL_P2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,0x3f,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_TOP_LIM_P2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG12 + u2HdmiAnaBase,0x40,HDMI2_CFG12_RG_HDMI_EQALG_LEQ_INCREASE_BOT_LIM_P2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,2,HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_LOGIC1_LOGIC);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,3,HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_LOGIC2_LOGIC);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0,HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_ALG_SEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,1,HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_N1_LOGIC_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0,HDMI2_CFG13_RG_HDMI_EQALG_LEQ_INCREASE_SATURATE_PROT_EN);

    return;
}

void vHDMI2ComAnaAdjustInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0x1e,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_TOP_LIM);
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0x75,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_BOT_LIM);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,0x75,HDMI2_CFG14_RG_HDMI_EQALG_ADJUST_BOT_LIM);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,7,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_RATIO);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_EN);

    return;
}

void vHDMI2ComAnaDebug1Init(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,0,HDMI2_CFG15_RG_HDMI_DEBUG1_PAT_MASK);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,0,HDMI2_CFG15_RG_HDMI_DEBUG1_TARGET_PAT);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,0xc8,HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_TARGET);
    vHDMI2ComAnaDebugResetPar(u2HdmiAnaBase);
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,0,HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_GATING);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,1,HDMI2_CFG14_RG_HDMI_DEBUG1_RSTB);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,0,HDMI2_CFG15_RG_HDMI_DEBUG1_CNT_GATING);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,1,HDMI2_CFG15_RG_HDMI_DEBUG1_RSTB);
    }
    vHDMI2ComAnaDebug1Cnt_EN(u2HdmiAnaBase,0);
    
    return;
}

void vHDMI2ComAnaDebug2Init(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_CDRLPI);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,2,HDMI2_CFG16_RG_HDMI_DEBUG2_KPGAIN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_CAL_EN);
    
    return;
}

void vHDMI2ComAnaPLLInit(UINT16 u2HdmiAnaBase)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_DIVCTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf5,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0,HDMI2_CFG5_RG_HDMI_PLL_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0x13,HDMI2_CFG5_RG_HDMI_PLL_FBKDIV);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,5,HDMI2_CFG5_RG_HDMI_PLL_IR);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_BC);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BR);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0xc,HDMI2_CFG5_RG_HDMI_PLL_BP);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_BCA);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_LOAD);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_PI_EN);

    return;
}

void vHDMI2ComAnaDeepPLLInit(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaDeepPLL_EN(u2HdmiAnaBase,0);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,1,HDMI2_CFG4_RG_HDMI_DEEPPLL_POSTDIV2); //2nd post divider
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,1,HDMI2_CFG19_RG_HDMI_DEEPPLL_POSDIV);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,2,HDMI2_CFG19_RG_HDMI_DEEPPLL_PREDIV);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_LVROD_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_MONCK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_MONVC_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,1,HDMI2_CFG19_RG_HDMI_DEEPPLL_MONREF_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_FRA_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW_CHG);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0x40,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_DET_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_CKSEL_MON);
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_BS);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_RESERVE);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG20 + u2HdmiAnaBase,0,HDMI2_CFG20_RG_HDMI_DEEPPLL_BS);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG20 + u2HdmiAnaBase,0,HDMI2_CFG20_RG_HDMI_DEEPPLL_RESERVE);
    }

    return;
}


void vHDMI2ComAnaInit(UINT16 u2HdmiAnaBase)
{
    if(IS_IC_5861())
    {
        fgIs5861 = TRUE;
    }
    else
    {
        fgIs5861 = FALSE;
    }
    vIO32WriteFldAlign(0xf0059800,1,Fld(1,0,AC_MSKB0)); //analog current

    vIO32WriteFldAlign(0xf002ceb4,1,Fld(1,13,AC_MSKB1)); //impedance select
    vIO32WriteFldAlign(0xf002c2b4,0x14,Fld(5,0,AC_MSKB0)); //impedance select

    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,0x0,HDMI2_CFG17_RG_HDMI_EQALG_RESERVE);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0x0,HDMI2_CFG18_RG_HDMI_EQALG_RESERVE);
    }
    
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_DATA_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,1,HDMI2_CFG0_RG_HDMI_CDR_RST);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_CDR_STOP);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_ATEST_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_ANAMON);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_CH0_CDRLPI);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_CH1_CDRLPI);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_CH2_CDRLPI);

    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_CKDIV2_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_CKGEN_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_DES_HF_MODE);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKDT_AEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_CKDT_SET);
    vHDMI2ComAnaHYBiasSet(u2HdmiAnaBase,2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CLKBUF_EN);
    vHDMI2ComAnaD2SBiasSet(u2HdmiAnaBase,2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_HY);
    vHDMI2ComAnaEQBiasSet(u2HdmiAnaBase,2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_FULL_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,2,HDMI2_CFG1_RG_HDMI_VREF);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_MHLEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_MHLMODE);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_FTMODE);
    vHDMI2ComAnaKPGainSet(u2HdmiAnaBase,3);

    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,1,HDMI2_CFG2_RG_HDMI_PLLCK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_RXAFE_EN_SW);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_RXAFECH0_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_RXAFECH1_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_RXAFECH2_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,1,HDMI2_CFG2_RG_HDMI_TERM_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_ZSEN_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,1,HDMI2_CFG2_RG_HDMI_TERM_MODE);
    vHDMI2ComAnaRXModeSet(u2HdmiAnaBase,1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_TEST_SEL);
    vHDMI2ComAnaEQGainSet(u2HdmiAnaBase,0);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_TERM_MHL_SEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_TERM_CM_ENB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,1,HDMI2_CFG2_RG_HDMI_DEEPCLRCLK_PDB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_DEEPCLRCLK_RSTN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_FM_SEL);

    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_FM_SEL2);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_TESTE);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CKMON_JITSEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CKMON_DIV4EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CKMON_JITEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_FLAG_SEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_ABIST_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_ABISTCK_SEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CKMON_CKSET);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CH0_MONEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CH1_MONEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CH2_MONEN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_CKMON_PLLSEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_EQ_CALEN);

    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,0,HDMI2_CFG4_RG_HDMI_REV);
    vHDMI2ComAnaGainBufSet(u2HdmiAnaBase,0);

    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,0,HDMI2_CFG17_RG_HDMI_LOGIC1_P2_CTRL);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0,HDMI2_CFG18_RG_HDMI_LOGIC1_P2_CTRL);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,1,HDMI2_CFG18_RG_HDMI_LOGIC2_P2_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0,HDMI2_CFG18_RG_HDMI_CON_P2_HEAD);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0,HDMI2_CFG18_RG_HDMI_CON_PWD_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,3,HDMI2_CFG18_RG_HDMI_DEBUG1_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,1,HDMI2_CFG18_RG_HDMI_CON_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,1,HDMI2_CFG18_RG_HDMI_CON_PDB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,0,HDMI2_CFG18_RG_HDMI_CON_MON);

    vHDMI2ComAnaEQMacroInit(u2HdmiAnaBase);
    vHDMI2ComAnaSAOSCInit(u2HdmiAnaBase);
    vHDMI2ComAnaEQOSCInit(u2HdmiAnaBase);
    vHDMI2ComAnaEQDIGInit(u2HdmiAnaBase);
    vHDMI2ComAnaEQIncreaseInit(u2HdmiAnaBase);
    vHDMI2ComAnaAdjustInit(u2HdmiAnaBase);
    vHDMI2ComAnaDebug2Init(u2HdmiAnaBase);
    vHDMI2ComAnaDebug1Init(u2HdmiAnaBase);
    vHDMI2ComAnaPLLInit(u2HdmiAnaBase);
    vHDMI2ComAnaDeepPLLInit(u2HdmiAnaBase);

    if(fgAnaFirstInit)
    {
    	vHDMI2ComAnaDelayInit();
        vHDMI2ComAnaLeqItemInit();
        vHDMI2ComAnaLFSelValue_Set(2,5);
        tAnaConfig.fgForce40xMode = FALSE;
        tAnaConfig.fgSupport3GHDMI20 = FALSE;
        fgAnaFirstInit = FALSE;
    }
    
    return;
}

void vHDMI2ComAnaSetConMon(UINT16 u2HdmiAnaBase,UINT8 u1MonSel)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,u1MonSel,HDMI2_CFG18_RG_HDMI_CON_MON);
    return;
}

UINT8 bHDMI2ComAnaGetCDRLPO(UINT16 u2HdmiAnaBase,HdmiTmdsDataChannel_t eChannel)
{
    UINT8 u1CDRLPO = 0;
    switch(eChannel)
    {
        case HDMI_TMDS_DATA_CHANNEL_0:
            u1CDRLPO = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG4 + u2HdmiAnaBase, HDMI2_CFG4_RGS_HDMI_CH0_CDRLPO);
            break;
        case HDMI_TMDS_DATA_CHANNEL_1:
            u1CDRLPO = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG4 + u2HdmiAnaBase, HDMI2_CFG4_RGS_HDMI_CH1_CDRLPO);
            break;
        case HDMI_TMDS_DATA_CHANNEL_2:
            u1CDRLPO = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG4 + u2HdmiAnaBase, HDMI2_CFG4_RGS_HDMI_CH2_CDRLPO);
            break;
        default:
            break;
    }
    u1CDRLPO = u1CDRLPO&0x7f;

    return u1CDRLPO;
}

UINT8 bHDMI2ComAnaGetAlgMonStatus(UINT16 u2HdmiAnaBase,UINT8 u1MonSel, HdmiTmdsDataChannel_t eChannel)
{
    UINT8 u1P = 0;
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,u1MonSel,HDMI2_CFG17_RG_HDMI_EQALG_LEQ_INCREASE_MONSEL);
    vUtDelay2us(1);
    switch(eChannel)
    {
        case HDMI_TMDS_DATA_CHANNEL_0:
            u1P = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG5 + u2HdmiAnaBase, HDMI2_CFG5_RGS_HDMI_CH0_ALG_MON);
            break;
        case HDMI_TMDS_DATA_CHANNEL_1:
            u1P = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG5 + u2HdmiAnaBase, HDMI2_CFG5_RGS_HDMI_CH1_ALG_MON);
            break;
        case HDMI_TMDS_DATA_CHANNEL_2:
            u1P = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG5 + u2HdmiAnaBase, HDMI2_CFG5_RGS_HDMI_CH2_ALG_MON);
            break;
        default:
            break;
    }
    u1P = u1P&0x7f;

    return u1P;
}

UINT8 bHDMI2ComAnaGetEQCtrl(UINT16 u2HdmiAnaBase,HdmiTmdsDataChannel_t eChannel)
{
    UINT8 u1EQCtrl = 0;
    switch(eChannel)
    {
        case HDMI_TMDS_DATA_CHANNEL_0:
            u1EQCtrl = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG3 + u2HdmiAnaBase, HDMI2_CFG3_RGS_HDMI_CH0_LEQCTRL);
            break;
        case HDMI_TMDS_DATA_CHANNEL_1:
            u1EQCtrl = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG3 + u2HdmiAnaBase, HDMI2_CFG3_RGS_HDMI_CH1_LEQCTRL);
            break;
        case HDMI_TMDS_DATA_CHANNEL_2:
            u1EQCtrl = (UINT8)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG3 + u2HdmiAnaBase, HDMI2_CFG3_RGS_HDMI_CH2_LEQCTRL);
            break;
        default:
            break;
    }
    u1EQCtrl = u1EQCtrl&0xf;

    return u1EQCtrl;
}

UINT16 wHDMI2ComAnaGetDebug1Status(UINT16 u2HdmiAnaBase,UINT8 u1EvenSel, UINT8 u1MonSel, HdmiTmdsDataChannel_t eChannel)
{
    UINT16 u2Status = 0;
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,u1EvenSel,HDMI2_CFG17_RG_HDMI_EQALG_DEBUG1_EVENSEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,u1MonSel,HDMI2_CFG17_RG_HDMI_EQALG_DEBUG1_MONSEL);
    vUtDelay2us(1);
    switch(eChannel)
    {
        case HDMI_TMDS_DATA_CHANNEL_0:
            u2Status = (UINT16)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG0 + u2HdmiAnaBase, HDMI2_CFG0_RGS_HDMI_CH0_DEBUG1_STATUS);
            break;
        case HDMI_TMDS_DATA_CHANNEL_1:
            u2Status = (UINT16)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG1 + u2HdmiAnaBase, HDMI2_CFG1_RGS_HDMI_CH1_DEBUG1_STATUS);
            break;
        case HDMI_TMDS_DATA_CHANNEL_2:
            u2Status = (UINT16)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG2 + u2HdmiAnaBase, HDMI2_CFG2_RGS_HDMI_CH2_DEBUG1_STATUS);
            break;
        default:
            break;
    }
    u2Status = u2Status&0x3ff;

    return u2Status;
}

BOOL bHDMI2ComAnaGetDebug1Ready(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand)
{
    BOOL bReady;
    switch(eBand)
    {
        case HDMI_TMDS_DATA_CHANNEL_0:
            bReady = (BOOL)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG0 + u2HdmiAnaBase, HDMI2_CFG0_RGS_HDMI_CH0_EQ_DEBUG1_CNT_RDY_O);
            break;
        case HDMI_TMDS_DATA_CHANNEL_1:
            bReady = (BOOL)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG1 + u2HdmiAnaBase, HDMI2_CFG1_RGS_HDMI_CH1_EQ_DEBUG1_CNT_RDY_O);
            break;
        case HDMI_TMDS_DATA_CHANNEL_2:
            bReady = (BOOL)IO32ReadFldAlign(HDMI2_RGS_HDMIRX_CFG2 + u2HdmiAnaBase, HDMI2_CFG2_RGS_HDMI_CH2_EQ_DEBUG1_CNT_RDY_O);
            break;
        default:
            break;
    }

    return bReady;
}

void vHDMI2ComAnaHalfRateSet(UINT16 u2HdmiAnaBase,BOOL b40xMode)
{
    if(b40xMode)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_DES_HF_MODE);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_DES_HF_MODE);
    }

    return;
}

BOOL bHDMI2ComAnaHalfRateGet(UINT16 u2HdmiAnaBase)
{
    if(IO32ReadFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,HDMI2_CFG1_RG_HDMI_DES_HF_MODE))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void vHDMI2ComAnaEQBandSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode)
{
    if(b40xMode)
    {
        if(tAnaConfig.fgSupport3GHDMI20) //To support 3G HDMI20 pattern made up by DTG5334
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,4,HDMI2_CFG2_RG_HDMI_EQBAND);
        }
        else
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_EQBAND);
        }
        tAnaConfig.tLEQItem.fgLeak_Decrease = TRUE;
    }
    else
    {
        if(eBand < HDMI_TMDS_CLOCK_40_50) //<40Mhz
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,7,HDMI2_CFG2_RG_HDMI_EQBAND);
            tAnaConfig.tLEQItem.fgLeak_Decrease = FALSE;
        }
        else
        {
            tAnaConfig.tLEQItem.fgLeak_Decrease = TRUE;
            if(eBand < HDMI_TMDS_CLOCK_140_200) //<140Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,6,HDMI2_CFG2_RG_HDMI_EQBAND);
            }
            else
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,4,HDMI2_CFG2_RG_HDMI_EQBAND);
            }
        }        
    }

    return;
}

void vHDMI2ComAnaPLLSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode)
{
    
    if(b40xMode)
    {
        if(eBand > HDMI_TMDS_CLOCK_80_100) //>100MHz
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND); 
        }
        else
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND); 
        }
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_DIVCTRL);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf5,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0x13,HDMI2_CFG5_RG_HDMI_PLL_FBKDIV);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,5,HDMI2_CFG5_RG_HDMI_PLL_IR);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_BC);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BR);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0xc,HDMI2_CFG5_RG_HDMI_PLL_BP);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_BCA);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_DIVCTRL);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0x09,HDMI2_CFG5_RG_HDMI_PLL_FBKDIV);
        if(eBand == HDMI_TMDS_CLOCK_25) //25Mhz
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0x1f,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf0,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,2,HDMI2_CFG5_RG_HDMI_PLL_IR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,2,HDMI2_CFG5_RG_HDMI_PLL_BR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0xf,HDMI2_CFG5_RG_HDMI_PLL_BP);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,2,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_BCA);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
        }
        else if(eBand < HDMI_TMDS_CLOCK_40_50)  //25~40Mhz
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0x1f,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf0,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,2,HDMI2_CFG5_RG_HDMI_PLL_IR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0xf,HDMI2_CFG5_RG_HDMI_PLL_BP);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_BCA);
            if(eBand < HDMI_TMDS_CLOCK_35_40) //25~35Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
            }
            else
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
            }
        }
        else if(eBand < HDMI_TMDS_CLOCK_140_200) //40~140Mhz
        {
            if(eBand < HDMI_TMDS_CLOCK_80_100) //40~80Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,7,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf0,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            }
            else if(eBand < HDMI_TMDS_CLOCK_120_140) //80~120Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,3,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf5,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            }
            else //120~140Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf5,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            }
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,6,HDMI2_CFG5_RG_HDMI_PLL_IR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0,HDMI2_CFG5_RG_HDMI_PLL_BC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BP);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_BCA);
            if(eBand < HDMI_TMDS_CLOCK_50_80) //40~50Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
            }
            else //50~140Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
            }
        }
        else
        {
            if(eBand < HDMI_TMDS_CLOCK_200_300) // 140~200Mhz
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xf5,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            }
            else
            {
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_VCO_BAND);
                vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,0xfa,HDMI2_CFG6_RG_HDMI_PLL_RESERVE);
            }
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_IC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,5,HDMI2_CFG5_RG_HDMI_PLL_IR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0,HDMI2_CFG5_RG_HDMI_PLL_BC);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BR);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,3,HDMI2_CFG5_RG_HDMI_PLL_BP);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,1,HDMI2_CFG7_RG_HDMI_PLL_KVSEL);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG7 + u2HdmiAnaBase,0,HDMI2_CFG7_RG_HDMI_PLL_BCA);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KS);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,3,HDMI2_CFG6_RG_HDMI_PLL_AUTOK_KF);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,2,HDMI2_CFG6_RG_HDMI_PLL_RST_DLY);
        }
    }

    return;
}

void vHDMI2ComAnaDeepPLLSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode)
{
    if(b40xMode)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,1,HDMI2_CFG4_RG_HDMI_DEEPPLL_POSTDIV2); //2nd post divider, /1
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,1,HDMI2_CFG19_RG_HDMI_DEEPPLL_POSDIV); //1st post divider, /2
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,2,HDMI2_CFG19_RG_HDMI_DEEPPLL_PREDIV); //pre divider, /4
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0x40,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW); //fbk divider, /64
    }
    else
    {
        if(eBand < HDMI_TMDS_CLOCK_140_200) //<140Mhz
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,0,HDMI2_CFG4_RG_HDMI_DEEPPLL_POSTDIV2); //2nd post divider, /5
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,2,HDMI2_CFG19_RG_HDMI_DEEPPLL_POSDIV); //1st post divider, /4
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0,HDMI2_CFG19_RG_HDMI_DEEPPLL_PREDIV); //pre divider, /1
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0x50,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW); //fbk divider, /80
        }
        else
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG4 + u2HdmiAnaBase,1,HDMI2_CFG4_RG_HDMI_DEEPPLL_POSTDIV2); //2nd post divider, /1
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,2,HDMI2_CFG19_RG_HDMI_DEEPPLL_POSDIV); //1st post divider, /4
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,2,HDMI2_CFG19_RG_HDMI_DEEPPLL_PREDIV); //pre divider, /4
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG19 + u2HdmiAnaBase,0x40,HDMI2_CFG19_RG_HDMI_DEEPPLL_SDM_PCW); //fbk divider, /64
        }
    }

    return;
}

BOOL bHDMI2ComAnaAdjustRatioSet(UINT16 u2HdmiAnaBase)
{
    UINT32 u4AdjustRatio;
    UINT8 u1P1,u1P2;
    UINT32 u4P1Sum,u4P2Sum;
    UINT32 u4PRatio;
    UINT8 u1Data;

    u4P1Sum = 0;
    u4P2Sum = 0;
    u1P1 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,0,HDMI_TMDS_DATA_CHANNEL_0);
    if((u1P1 == 0) || (u1P1 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P1Sum += u1P1;
    u1P1 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,0,HDMI_TMDS_DATA_CHANNEL_1);
    if((u1P1 == 0) || (u1P1 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P1Sum += u1P1;
    u1P1 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,0,HDMI_TMDS_DATA_CHANNEL_2);
    if((u1P1 == 0) || (u1P1 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P1Sum += u1P1;
    u4P1Sum = (u4P1Sum * 100)/3;
    
    u1P2 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,1,HDMI_TMDS_DATA_CHANNEL_0);
    if((u1P2 == 0) || (u1P2 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P2Sum += u1P2;
    u1P2 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,1,HDMI_TMDS_DATA_CHANNEL_1);
    if((u1P2 == 0) || (u1P2 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P2Sum += u1P2;
    u1P2 = bHDMI2ComAnaGetAlgMonStatus(u2HdmiAnaBase,1,HDMI_TMDS_DATA_CHANNEL_2);
    if((u1P2 == 0) || (u1P2 > 0x3f)) //0 and negative value is invalid
    {
        return FALSE;
    }
    u4P2Sum += u1P2;
    u4P2Sum = (u4P2Sum * 100)/3;
    
    u4PRatio = (u4P2Sum * 100)/u4P1Sum; //*100 ratio

    u4AdjustRatio = (2*100*1000)/(u4PRatio + HDMI2_ANA_ADJUST_RATIO_PARA); //*1000 ratio
    u4AdjustRatio = (u4AdjustRatio * 8)/1000; //*1 ratio

    u1Data = (UINT8)(u4AdjustRatio & 0xf);
    if(fgIs5861)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG17 + u2HdmiAnaBase,(u1Data>>3),HDMI2_CFG17_RG_HDMI_EQALG_ADJUST_RATIO_MSB);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG18 + u2HdmiAnaBase,(u1Data>>3),HDMI2_CFG18_RG_HDMI_EQALG_ADJUST_RATIO_MSB);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,(u1Data&0x7),HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_RATIO);

    return TRUE;
}

void vHDMI2ComAnaDebug1CntTargetSet(UINT16 u2HdmiAnaBase,UINT32 u4CntTarget)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG14 + u2HdmiAnaBase,u4CntTarget,HDMI2_CFG14_RG_HDMI_DEBUG1_CNT_TARGET);
    return;
}

void vHDMI2ComAnaDebug1PatSet(UINT16 u2HdmiAnaBase,UINT16 u2PatMask, UINT16 u2TargetPat)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,u2PatMask,HDMI2_CFG15_RG_HDMI_DEBUG1_PAT_MASK);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG15 + u2HdmiAnaBase,u2TargetPat,HDMI2_CFG15_RG_HDMI_DEBUG1_TARGET_PAT);
    return;
}

void vHDMI2ComAnaDebug1ParSet(UINT16 u2HdmiAnaBase,INT8 s1Par1,INT8 s1Par2)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,s1Par1,HDMI2_CFG16_RG_HDMI_DEBUG1_PAR1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,s1Par2,HDMI2_CFG16_RG_HDMI_DEBUG1_PAR2);
    vUtDelay2us(20); //40us

    return;
}

void vHDMI2ComAnaPLLEnable(UINT16 u2HdmiAnaBase)
{
	vHDMI2ComAnaPLL_EN(u2HdmiAnaBase,0);
	vUtDelay2us(100);
    vHDMI2ComAnaPLL_EN(u2HdmiAnaBase,1);
    vUtDelay2us(200); //200us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_MONCK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG6 + u2HdmiAnaBase,1,HDMI2_CFG6_RG_HDMI_PLL_MONREF_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0x410,HDMI2_CFG2_RG_HDMI_FM_SEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_PI_EN);

    return;
}

void vHDMI2ComAnaEQMacroEnable(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    
    vUtDelay2us(1);
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    
    vUtDelay2us(1);
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);

    return;
}

void vHDMI2ComAnaDataReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,1);
    vUtDelay2us(1);
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,0);
    vUtDelay2us(1);
    vHDMI2ComAnaData_RSTB(u2HdmiAnaBase,1);

    return;
}

void vHDMI2ComAnaEQALGReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,1);
    vUtDelay2us(1);
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,0);
    vUtDelay2us(1);
    vHDMI2ComAnaEQALG_RSTB(u2HdmiAnaBase,1);

    return;
}

void vHDMI2ComAnaSAOSC(UINT16 u2HdmiAnaBase,BOOL fgResetSAOSC)
{
    BOOL bHalfRate = bHDMI2ComAnaHalfRateGet(u2HdmiAnaBase);
    UINT16 u2DelayRatio = wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_SAOSC);
    vHDMI2ComAnaDebug1Ctrl_EN(u2HdmiAnaBase,1);
    vHDMI2ComAnaDebugResetPar(u2HdmiAnaBase);
    if(!fgResetSAOSC)
    {
        vHDMI2ComAnaCDR_RST(u2HdmiAnaBase,1);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKDIV2_RSTB);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKGEN_RSTB);
    }
    vHDMI2ComAnaEQ_OS_LF_SELSet(u2HdmiAnaBase,tAnaConfig.u1SAOSC_LFSEL);
    //Set half rate for SAOSC
    vHDMI2ComAnaHalfRateSet(u2HdmiAnaBase,TRUE);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,1);
    if(!fgResetSAOSC)
    {
        vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    vHDMI2ComAnaDebug1SAOSC_EN(u2HdmiAnaBase,0);
    
    vUtDelay2us(1);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,0);
    if(!fgResetSAOSC)
    {
        vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaSAOSC_RSTB(u2HdmiAnaBase,1);
    if(!fgResetSAOSC)
    {
        vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaDebug1SAOSC_EN(u2HdmiAnaBase,1);
    vUtDelay2us((750*u2DelayRatio)/10); //1500us * ratio
    vHDMI2ComAnaDebug1SAOSC_EN(u2HdmiAnaBase,0);

    if(fgResetSAOSC)
    {
        vHDMI2ComAnaHalfRateSet(u2HdmiAnaBase,bHalfRate);
    }

    vHDMI2ComAnaDebug1Ctrl_EN(u2HdmiAnaBase,0);

    return;
}

void vHDMI2ComAnaSAOSCReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaSAOSC(u2HdmiAnaBase,TRUE);

    return;
}

void vHDMI2ComAnaEQOSC(UINT16 u2HdmiAnaBase,BOOL fgResetEQOSC)
{
    if(!fgResetEQOSC)
    {
        vHDMI2ComAnaCDR_RST(u2HdmiAnaBase,1);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKDIV2_RSTB);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKGEN_RSTB);       
    }
    vHDMI2ComAnaEQ_OS_LF_SELSet(u2HdmiAnaBase,tAnaConfig.u1EQOSC_LFSEL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_EQ_CALEN);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQOSC)
    {
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,0);
    if(!fgResetEQOSC)
    {
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaEQOSC_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQOSC)
    {
        vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    
    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,1,HDMI2_CFG3_RG_HDMI_EQ_CALEN);
    vUtDelay2us(50); //100us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG3 + u2HdmiAnaBase,0,HDMI2_CFG3_RG_HDMI_EQ_CALEN);

    return;
}

void vHDMI2ComAnaEQOSCReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaEQOSC(u2HdmiAnaBase,TRUE);

    return;
}

void vHDMI2ComAnaCDRLock(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaCKDIV2_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaCKGEN_RSTB(u2HdmiAnaBase,0);
    vHDMI2ComAnaCDR_RST(u2HdmiAnaBase,1);
    
    vUtDelay2us(1);
    vHDMI2ComAnaCKDIV2_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaCKGEN_RSTB(u2HdmiAnaBase,1);
    vHDMI2ComAnaCDR_RST(u2HdmiAnaBase,0);

    vUtDelay2us(5); //10us
    return;
}

void vHDMI2ComAnaCDRReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaCDRLock(u2HdmiAnaBase);
    return;
}

void vHDMI2ComAnaEQLock(UINT16 u2HdmiAnaBase,BOOL b40xMode, HDMI2ANA_LEQ_ITEM *ptLeqItem, BOOL fgResetEQ)
{
    BOOL fgLEQ_Reduce = FALSE;
    UINT16 u2DelayRatio_Half = wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_EQLOCK_HALF);
    UINT16 u2DelayRatio_Full = wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_EQLOCK_FULL);
    if(NULL == ptLeqItem)
    {
        Printf("%s:Error!ptLeqItem is NULL!",__FUNCTION__);
        return;
    }
    if(fgResetEQ)
    {
        fgLEQ_Reduce = FALSE;
    }
    else
    {
        fgLEQ_Reduce = ptLeqItem->fgLEQ_Reduce;
    }
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQ)
    {
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,2,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,8,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
    if(fgLEQ_Reduce)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_EN);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,0);
    if(!fgResetEQ)
    {
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,0);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    }
    
    vUtDelay2us(1);
    vHDMI2ComAnaEQ_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQ)
    {
        vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    
    vUtDelay2us(1);
    if(ptLeqItem->fgLeak)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
    if(b40xMode)
    {
        vUtDelay2us((200*u2DelayRatio_Half)/100); //40us
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
        vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
        if(ptLeqItem->fgLeak_Increase)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,2,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
            vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,3,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
            vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
        }
        if(ptLeqItem->fgLeak)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN);
        }
        if(ptLeqItem->fgLeak_Decrease)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xc,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xe,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xf,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((200*u2DelayRatio_Half)/10); //400us
        }
        if(ptLeqItem->fgAvg)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,1,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
            vUtDelay2us((100*u2DelayRatio_Half)/10); //200us
        }
        if(fgLEQ_Reduce)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,1,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
            vUtDelay2us((275*u2DelayRatio_Half)/10); //550us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,3,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
            vUtDelay2us((275*u2DelayRatio_Half)/10); //550us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,1,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_EN);
            vUtDelay2us((1000*u2DelayRatio_Half)/10); //2000us
        }
        
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
        if(ptLeqItem->fgAvg)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
        }
        if(fgLEQ_Reduce)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_REDUCE_EN);
        }
    }
    else
    {
        vUtDelay2us((1250*u2DelayRatio_Full)/100); //250us
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
        vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
        if(ptLeqItem->fgLeak_Increase)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,2,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
            vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,3,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_INCREASE_CTRL);
            vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
        }
        if(ptLeqItem->fgLeak)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_EN);
        }
        if(ptLeqItem->fgLeak_Decrease)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xc,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xe,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0xf,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_LEAK_DECREASE_CTRL);
            vUtDelay2us((1250*u2DelayRatio_Full)/10); //2500us
        }
        if(ptLeqItem->fgAvg)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,1,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
            vUtDelay2us((1000*u2DelayRatio_Full)/10); //2000us
        }
        
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
        if(ptLeqItem->fgAvg)
        {
            vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG9 + u2HdmiAnaBase,0,HDMI2_CFG9_RG_HDMI_EQALG_LEQ_AVG_EN);
        }
    }

    return;
}

void vHDMI2ComAnaFixEQ(UINT16 u2HdmiAnaBase,BOOL bFix, UINT8 u1EQInitVal)
{
    if(bFix)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,u1EQInitVal,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_VAL);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_SET);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,8,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_INIT_VAL);
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_LEQ_EN);
    }

    return;
}

void vHDMI2ComAnaEQReset(UINT16 u2HdmiAnaBase,BOOL b40xMode, HDMI2ANA_LEQ_ITEM *ptLeqItem)
{
    vHDMI2ComAnaEQLock(u2HdmiAnaBase,b40xMode,ptLeqItem,TRUE);
    return;
}

void vHDMI2ComAnaEQIncreaseEnable(UINT16 u2HdmiAnaBase,BOOL fgResetEQIncrease)
{
    UINT16 u2DelayRatio = wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_EQINCREASE);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQIncrease)
    {
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);

    vUtDelay2us(1);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,0);
    if(!fgResetEQIncrease)
    {
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    }

    vUtDelay2us(1);
    vHDMI2ComAnaEQIncrease_RSTB(u2HdmiAnaBase,1);
    if(!fgResetEQIncrease)
    {
        vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    }

    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,1,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
    vUtDelay2us((250*u2DelayRatio)/10); //500us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,3,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
    vUtDelay2us((250*u2DelayRatio)/10); //500us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG11 + u2HdmiAnaBase,0,HDMI2_CFG11_RG_HDMI_EQALG_LEQ_INCREASE_P_EN);
    return;
}

void vHDMI2ComAnaEQIncreaseReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaEQIncreaseEnable(u2HdmiAnaBase,TRUE);

    return;
}

void vHDMI2ComAnaAdjustEnable(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_EN);
    vUtDelay2us(1);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,0);
    vUtDelay2us(1);
    vHDMI2ComAnaAdjust_RSTB(u2HdmiAnaBase,1);

    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,1,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_EN);
    vUtDelay2us(10); //20us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG13 + u2HdmiAnaBase,0,HDMI2_CFG13_RG_HDMI_EQALG_ADJUST_EN);

    return;
}

void vHDMI2ComAnaAdjustReset(UINT16 u2HdmiAnaBase) //reserve for reset
{
    vHDMI2ComAnaAdjustEnable(u2HdmiAnaBase);
    return;
}

void vHDMI2ComAnaDeepPLLEnable(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaDeepPLL_EN(u2HdmiAnaBase,0);
    vHDMI2ComAnaDeepPLL_EN(u2HdmiAnaBase,1);

    return;
}

void vHDMI2ComAnaTermCtrl(UINT16 u2HdmiAnaBase,BOOL bOnOff)
{
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,(bOnOff?1:0),HDMI2_CFG2_RG_HDMI_TERM_EN);
    return;
}

void vHDMI2ComAnaTMDSCtrl(UINT16 u2HdmiAnaBase,BOOL bOnOff)
{
    if(bOnOff)
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CLKBUF_EN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_CKDT_AEN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_FULL_EN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,1,HDMI2_CFG2_RG_HDMI_TERM_MODE);	
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_DATA_RSTB);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,1,HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,0,HDMI2_CFG0_RG_HDMI_CDR_RST);	
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,1,HDMI2_CFG5_RG_HDMI_PLL_EN);
        vHDMI2ComAnaDeepPLL_EN(u2HdmiAnaBase,1);
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_CLKBUF_EN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,0,HDMI2_CFG1_RG_HDMI_CKDT_AEN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG1 + u2HdmiAnaBase,1,HDMI2_CFG1_RG_HDMI_FULL_EN);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG2 + u2HdmiAnaBase,0,HDMI2_CFG2_RG_HDMI_TERM_MODE);		
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_DATA_RSTB);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG8 + u2HdmiAnaBase,0,HDMI2_CFG8_RG_HDMI_EQALG_RSTB);
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG0 + u2HdmiAnaBase,1,HDMI2_CFG0_RG_HDMI_CDR_RST);	
    	vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG5 + u2HdmiAnaBase,0,HDMI2_CFG5_RG_HDMI_PLL_EN);
    	vHDMI2ComAnaDeepPLL_EN(u2HdmiAnaBase,0);
    }

    return;
}

void vHDMI2ComAnaSetting(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode)
{
	if((eBand==HDMI_TMDS_CLOCK_200_300) && (!b40xMode))
	{
		vHDMI2ComAnaEQGainSet(u2HdmiAnaBase,1);
	}
    vHDMI2ComAnaEQBandSet(u2HdmiAnaBase,eBand,b40xMode);
    vHDMI2ComAnaPLLSet(u2HdmiAnaBase,eBand,b40xMode);
    vHDMI2ComAnaDeepPLLSet(u2HdmiAnaBase,eBand,b40xMode);
    
    vHDMI2ComAnaPLLEnable(u2HdmiAnaBase);
    vHDMI2ComAnaEQMacroEnable(u2HdmiAnaBase);
    if(!fgSAOSCDone)
    {
        vHDMI2ComAnaSAOSC(u2HdmiAnaBase,FALSE);
        //fgSAOSCDone = TRUE;
    }
    vHDMI2ComAnaEQOSC(u2HdmiAnaBase,FALSE);
    
    vHDMI2ComAnaHalfRateSet(u2HdmiAnaBase,b40xMode);

    vHDMI2ComAnaCDRLock(u2HdmiAnaBase);
    vHDMI2ComAnaEQLock(u2HdmiAnaBase,b40xMode,&(tAnaConfig.tLEQItem),FALSE);
    if(b40xMode)
    {
        vHDMI2ComAnaEQIncreaseEnable(u2HdmiAnaBase,FALSE);
        if(bHDMI2ComAnaAdjustRatioSet(u2HdmiAnaBase))
        {
            vHDMI2ComAnaAdjustEnable(u2HdmiAnaBase);
        }
        else
        {
            LOG(6, "Adjust ratio invalid!\n");
        }
    }
    vHDMI2ComAnaDeepPLLEnable(u2HdmiAnaBase);
    
    return;
}

void vHDMI2ComAnaDebug1Enable(UINT16 u2HdmiAnaBase,BOOL bEnable)
{
    if(bEnable)
    {
        vHDMI2ComAnaDebug1Cnt_EN(u2HdmiAnaBase,1);
    }
    else
    {
        vHDMI2ComAnaDebug1Cnt_EN(u2HdmiAnaBase,0);
    }

    return;
}

void vHDMI2ComAnaDebug2Enable(UINT16 u2HdmiAnaBase)
{
    vHDMI2ComAnaDebugResetPar(u2HdmiAnaBase);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,1,HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_CAL_EN);
    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB);
    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,1,HDMI2_CFG16_RG_HDMI_DEBUG2_RSTB);
    vUtDelay2us(1);
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,1,HDMI2_CFG16_RG_HDMI_DEBUG2_CAL_EN);
    vUtDelay2us(500); //1000us
    vIO32WriteFldAlign(HDMI2_REG_HDMIRX_CFG16 + u2HdmiAnaBase,0,HDMI2_CFG16_RG_HDMI_DEBUG2_CAL_EN);

    return;
}

UINT32 dwHDMI2ComHTotal(UINT16 u2HdmiBase)
{
	 UINT32 tmp;
     //tmp = ((( (UINT16)u1IO32Read1B(VID_HRES_3) & 0x1f) << 8) |  (UINT16)u1IO32Read1B(VID_HRES_2));
     tmp = ((UINT32)IO32ReadFldAlign(HDMI2_FDET_HSYNC_LOW_COUNT + u2HdmiBase, RG_HDMI2_FDET_HSYNC_LOW_COUNT));

     tmp += ((UINT32)IO32ReadFldAlign(HDMI2_FDET_HSYNC_LOW_COUNT + u2HdmiBase, RG_HDMI2_FDET_HSYNC_HIGH_COUNT));

     return tmp;
}

UINT32 dwHDMI2ComVTotal(UINT16 u2HdmiBase)
{
	 UINT32 tmp;
	 
	 tmp = ((UINT32)IO32ReadFldAlign(HDMI2_FDET_VSYNC_LOW_COUNT_EVEN + u2HdmiBase, RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN));
	 tmp += ((UINT32)IO32ReadFldAlign(HDMI2_FDET_VSYNC_LOW_COUNT_EVEN + u2HdmiBase, RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN));

	 if(fgHDMI2ComGetInterlaced(u2HdmiBase))
	 {
	 	 tmp += ((UINT32)IO32ReadFldAlign(HDMI2_FDET_VSYNC_LOW_COUNT_ODD + u2HdmiBase, RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD));
	 	 tmp += ((UINT32)IO32ReadFldAlign(HDMI2_FDET_VSYNC_LOW_COUNT_ODD + u2HdmiBase, RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD));
	 }
	
     return tmp;
}

UINT32 dwHDMI2ComHActive(UINT16 u2HdmiBase)
{
	 UINT32 tmp;
	 
     tmp = ((UINT32)IO32ReadFldAlign(HDMI2_FDET_PIXEL_COUNT + u2HdmiBase, RG_HDMI2_FDET_PIXEL_COUNT));


     return tmp;
}

UINT32 dwHDMI2ComVActive(UINT16 u2HdmiBase)
{
	 UINT32 tmp;
	 
	 tmp = (((UINT32)IO32ReadFldAlign(HDMI2_FDET_PIXEL_COUNT + u2HdmiBase, RG_HDMI2_FDET_LINE_COUNT))<< fgHDMI2ComGetInterlaced(u2HdmiBase));
	
     return tmp;
}


UINT32 dwHDMI2ComFrameRate(UINT16 u2HdmiBase)
{
     UINT32 tmp,u4CpuBusClock;
	 UINT32 u4FrameRate;
	 
	 tmp = ((UINT32)IO32ReadFldAlign(HDMI2_FDET_FRAME_RATE + u2HdmiBase, RG_HDMI2_FDET_FRAME_RATE));
	 if(tmp == 0)
	 {
	 	//Printf("invalid frame rate\n");
		return tmp;
	 }
     u4CpuBusClock = BSP_GetDomainClock(SRC_BUS_CLK);
     
	 tmp = (UINT32)u4CpuBusClock * 10/tmp; //frame rate = bus clock div this value;
	 tmp = tmp*10;
	
	 if((tmp >= 5950) && (tmp <= 6000))
	 {
	 	u4FrameRate = 60;
	 }
	 if((tmp >= 4950) && (tmp <= 5025))
	 {
	 	u4FrameRate = 50;
	 }
	 if((tmp >= 2350) && (tmp <= 2425))
	 {
	 	u4FrameRate = 24;
	 }
	 if((tmp >= 2450) && (tmp <= 2525))
	 {
	 	u4FrameRate = 25;
	 }
	 if((tmp >= 2950) && (tmp <= 3025))
	 {
	 	u4FrameRate = 30;
	 }
	 
     return u4FrameRate;
}

UINT32 dwHDMI2ComPixelClk(UINT16 u2HdmiBase)
{
    UINT32 tmp,u4CpuBusClock;
	 
	 tmp = (IO32ReadFldAlign(HDMI2_RX_VID_XPCNT1 + u2HdmiBase, RG_HDMI2_AAC_XCLK_IN_PCLK23_16) << 16) 
	 	 | (IO32ReadFldAlign(HDMI2_RX_VID_XPCNT1 + u2HdmiBase, RG_HDMI2_AAC_XCLK_IN_PCLK15_8) << 8)
	 	 | IO32ReadFldAlign(HDMI2_RX_VID_XPCNT1 + u2HdmiBase, RG_HDMI2_AAC_XCLK_IN_PCLK7_0);

	 if(tmp == 0)
	 {
	 	//Printf("invalid pixel clock\n");
		return tmp;
	 }
     u4CpuBusClock = BSP_GetDomainClock(SRC_BUS_CLK);
	 tmp = 2048 * (u4CpuBusClock/1000) / tmp;
	 tmp=tmp*1000;

     return tmp;
}

UINT32 dwHDMI2ComHClk(UINT16 u2HdmiBase)
{
    UINT32 ret;
    UINT32 wDiv;
    wDiv = dwHDMI2ComHTotal(u2HdmiBase);

    if(wDiv == 0)
    {
        return 1;
    }

    ret = (dwHDMI2ComPixelClk(u2HdmiBase) / (wDiv));
	ret /= 100;

    return ret;
}

//1:internal pattern gen
void vHDMI2ComPatternGenSet(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_RX_AUDO_MUTE + u2HdmiBase,u1En,RG_HDMI2_REG_BIST_VIDEO_OUT_SEL);
}

BOOL fgHDMI2ComAnaCRC(UINT16 u2HdmiBase,UINT16 u2Times)
{
	//to do
	UINT32 u4CrcResult[2];
	UINT32 u4CrcTmpData;
	UINT8 index=0;
	//to do
	while(u2Times)
	{
		u2Times--;
		vUtDelay10ms(2);
		if(index > 2)
		{
			Printf("CRC fail\n");
			return 0;
		}
		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL2 + u2HdmiBase,0, RG_HDMI2_C_CRC_START);
		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL2 + u2HdmiBase,1, RG_HDMI2_C_CRC_CLR);
		vUtDelay1ms(1);
		vIO32WriteFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,1, RG_HDMI2_C_CRC_FAIL);
		vIO32WriteFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,1, RG_HDMI2_C_CRC_READY);

		while(IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_C_CRC_READY) != 0)//read the CRC_RDY bit, if the 1, indicate the CRC Check Data is ready. STA_CRC_RDY is read only.
		{
			//Printf("in while1\n");
			vUtDelay1ms(1);
			if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)//read if the SCDT is active. If the 0, break. If the 1, wate for the STA_CRC_RDY.
			{
				break;
			}
		}
		while(IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_REG_CRC_OUT) !=0)//read the value of the CRC Check out data[7:0].
		{
			//Printf("in while2\n");
			vUtDelay1ms(1);
			if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)
			{
				break;
			}
		}
		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL2 + u2HdmiBase,0, RG_HDMI2_C_CRC_CLR);
		if(IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_C_CRC_READY) == 0)
		{

			vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL2 + u2HdmiBase,1, RG_HDMI2_C_CRC_START);
			
			while(IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_C_CRC_READY) != 1)//read the CRC_RDY bit, if the 1, indicate the CRC Check Data is ready. STA_CRC_RDY is read only.
			{
				//Printf("in while3\n");
				vUtDelay1ms(100);
				if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)
				{
					break;
				}
			}
			//vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL2 + u2HdmiBase,0, C_CRC_START);
			
			if(IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_C_CRC_READY) == 1)
			{
				u4CrcTmpData = IO32ReadFldAlign(HDMI2_DIG_PHY_STATUS1 + u2HdmiBase,RG_HDMI2_REG_CRC_OUT);

				if((u4CrcTmpData == u4CrcResult[0]) || (u4CrcTmpData == u4CrcResult[1]))
				{
					continue;
				}

				if(index <= 2)
				{
					u4CrcResult[index] = u4CrcTmpData;
					index++;
				}
			}
			else
			{
				Printf("CRC not ready\n");
				return 0;
			}
			
		}

	}

	if(fgHDMI2ComGetInterlaced(u2HdmiBase))
	{
		Printf("interlaced signal\n");
	}
	else
	{
		Printf("progressive signal\n");
	}

	if(index == 1)
	{
		Printf("assume progressive signal\n");
		Printf("CRC result:\n");
		Printf("0x%x\n", u4CrcResult[0]);
	}
	else if(index == 2)
	{
		LOG(0, "assume interlaced signal\n");
		LOG(0, "CRC result:\n");
		LOG(0, "0x%x\n", u4CrcResult[0]);
		LOG(0, "0x%x\n", u4CrcResult[1]);
	}
	return 0;
}

BOOL fgHDMI2ComCRC(UINT16 u2HdmiBase,UINT16 u2Times)
{
	UINT32 u4CrcResult[2];
	UINT32 u4CrcTmpData;
	UINT8 index=0;

	while(u2Times)
	{
		u2Times--;
		vUtDelay10ms(2);
		if(index > 2)
		{
			Printf("CRC fail\n");
			return 0;
		}

		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_CRC_Clr);
		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,0, RG_HDMI2_REG_CRC_Start);
		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,3, RG_HDMI2_REG_CRC_Mode);
		vUtDelay1ms(1);
		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,0, RG_HDMI2_REG_CRC_Ready);	
		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,1, RG_HDMI2_REG_CRC_Fail);

		while(IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_Ready) != 0)//read the CRC_RDY bit, if the 1, indicate the CRC Check Data is ready. STA_CRC_RDY is read only.
		{
			vUtDelay1ms(10);
			if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)//read if the RG_HDMI2_SCDT is active. If the 0, break. If the 1, wate for the STA_CRC_RDY.
			{
				break;
			}
		}
		while(IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_OUT) !=0)//read the value of the CRC Check out data[7:0].
		{
			vUtDelay1ms(10);
			if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)
			{
				break;
			}
		}
		
		vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_CRC_Clr);
		
		if(IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_Ready) == 0)
		{

			vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,1, RG_HDMI2_REG_CRC_Start);
			
			while(IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_Ready) != 1)//read the CRC_RDY bit, if the 1, indicate the CRC Check Data is ready. STA_CRC_RDY is read only.
			{
				vUtDelay1ms(50);
				if(RegReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCDT) == 0)//read if the RG_HDMI2_SCDT is active. If the 0, break. If the 1, wate for the STA_CRC_RDY.
				{
					break;
				}
			}
			//vIO32WriteFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,0, RG_HDMI2_REG_CRC_Start);
			
			if(IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_Ready) == 1)
			{
				u4CrcTmpData = IO32ReadFldAlign(HDMI2_CRC_CONTROL + u2HdmiBase,RG_HDMI2_REG_CRC_OUT);

				if((u4CrcTmpData == u4CrcResult[0]) || (u4CrcTmpData == u4CrcResult[1]))
				{
					continue;
				}

				if(index < 2)
				{
					u4CrcResult[index] = u4CrcTmpData;
				}
                index++;
			}
			else
			{
				Printf("CRC not ready\n");
				return 0;
			}
			
		}

	}

	if(fgHDMI2ComGetInterlaced(u2HdmiBase))
	{
		Printf("interlaced signal\n");
	}
	else
	{
		Printf("progressive signal\n");
	}

	if(index == 1)
	{
		Printf("assume progressive signal\n");
		Printf("CRC result:\n");
		Printf("0x%x\n", u4CrcResult[0]);
	}
	else if(index == 2)
	{
		LOG(0, "assume interlaced signal\n");
		LOG(0, "CRC result:\n");
		LOG(0, "0x%x\n", u4CrcResult[0]);
		LOG(0, "0x%x\n", u4CrcResult[1]);
	}
	return 1;
	
}

/**
 * @brief 	Get HDMI AVMute status
 * @param	Base address by port
 * @retval	1:interlace,0:program
 */
BOOL fgHDMI2ComGetInterlaced(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_FDET_CONFIG + u2HdmiBase, RG_HDMI2_INTERLACED);
}

/**
 * @brief 	Get HDMI AVMute status
 * @param	Base address by port
 * @retval	1:mute,0:clear mute
 */
BOOL fgHDMI2ComGetAvMute(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_DEC_AV_MUTE + u2HdmiBase, RG_HDMI2_HDMI_MUTE);
}

// clear avmute
void vHDMI2ComClrAvMute(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_DEC_AV_MUTE + u2HdmiBase,u1En,RG_HDMI2_REG_CLEAR_AV_MUTE);
}

// set audio mute
void vHDMI2ComAudMuteSet(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_RX_AUDO_MUTE + u2HdmiBase,u1En,RG_HDMI2_REG_AUDIO_MUTE);
}

void vHDMI2ComVideMuteSet(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_DEC_AV_MUTE + u2HdmiBase,u1En,RG_HDMI2_REG_VIDEO_MUTE);
}

//AAC enbale(bit)
void vHDMI2ComAacEn(UINT16 u2HdmiBase,UINT8 u1En)
{
	//to do
	vIO32WriteFldAlign(HDMI2_AEC2_CTRL + u2HdmiBase,u1En,RG_HDMI2_REG_AAC_EN);
	vIO32WriteFldAlign(HDMI2_AEC2_CTRL + u2HdmiBase,u1En,RG_HDMI2_REG_AAC_ALL);
	vIO32WriteFldAlign(HDMI2_AEC2_CTRL + u2HdmiBase,u1En,RG_HDMI2_REG_AAC_OUT_OFF_EN);
	vIO32WriteFldAlign(HDMI2_AEC2_CTRL + u2HdmiBase,u1En,RG_HDMI2_REG_CTRL_ACR_EN);
	
}

/**
 * @brief 	Get HDMI mode status
 * @param	Base address by port
 * @retval	1:HDMI mode,0:DVI mode
 */
BOOL fgHDMI2ComGetHdmiMode(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_DEC_AV_MUTE + u2HdmiBase, RG_HDMI2_HDMI_MODE_DET);
}

/**
 * @brief 	Get HDMI audio mute status
 * @param	Base address by port
 * @retval	1:mute,0:active
 */
BOOL fgHDMI2ComGetAudioMute(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_RX_AUDO_MUTE + u2HdmiBase, RG_HDMI2_AAC_AUDIO_MUTE);
}

/**
 * @brief 	Get HDMI Tx scrambling en/dis status
 * @param	Base address by port
 * @retval	1:need descrambling input data,
 			0:not need descrambling input data
 */
BOOL fgHDMI2ComGetScramblingBySCDC(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase, RG_HDMI2_SCRAMBLING_EN);
}

/**
 * @brief 	Get HW en/dis scrambling status,default scrambling en/dis is controled by HW
 * @param	Base address by port
 * @retval	1:enable,
 			0:disable
 */
BOOL fgHDMI2ComGetHWScramblingEn(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase, RG_HDMI2_HW_SCRAMBLING_ON_EN);
}

/**
 * @brief 	Get TMDS clk ratio by scdc
 * @param	Base address by port
 * @retval	1:1/40,0:1/30
 */
BOOL fgHDMI2ComGetTmdsClkRatio(UINT16 u2HdmiBase)
{
    if(tAnaConfig.fgForce40xMode)
    {
        return TRUE;
    }
	return IO32ReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase,RG_HDMI2_TMDS_CLK_RATIO);
}

/**
 * @brief 	Get TMDS clk status
 * @param	Base address by port
 * @retval	1:clk active,0:no clk
 **/
BOOL fgHDMI2ComGetCKDT(UINT16 u2HdmiBase)
{
	return (BOOL)IO32ReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase,RG_HDMI2_CKDT);
}

/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
BOOL fgHDMI2ComGetSCDT(UINT16 u2HdmiBase)
{
	return (BOOL)IO32ReadFldAlign(HDMI2_TOP_MISC + u2HdmiBase,RG_HDMI2_SCDT);
}

/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
UINT32 dwHDMI2ComGetCTS(UINT16 u2HdmiBase)
{
	return ((IO32ReadFldAlign(HDMI2_RX_CTS_HVAL1 + u2HdmiBase,RG_HDMI2_CTS_VAL_HW19_16) << 16) | (IO32ReadFldAlign(HDMI2_RX_CTS_HVAL1 + u2HdmiBase,RG_HDMI2_CTS_VAL_HW15_8) << 8) | (IO32ReadFldAlign(HDMI2_RX_CTS_HVAL1 + u2HdmiBase,RG_HDMI2_CTS_VAL_HW7_0)));
}

/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
UINT32 dwHDMI2ComGetN(UINT16 u2HdmiBase)
{
	return ((IO32ReadFldAlign(HDMI2_RX_N_HVAL3 + u2HdmiBase,RG_HDMI2_N_VAL_HW19_16) << 16) | (IO32ReadFldAlign(HDMI2_RX_N_SVAL2 + u2HdmiBase,RG_HDMI2_N_VAL_HW15_8) << 8) | (IO32ReadFldAlign(HDMI2_RX_N_SVAL2 + u2HdmiBase,RG_HDMI2_N_VAL_HW7_0)));
}

/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
UINT8 bHDMI2ComGetFs(UINT16 u2HdmiBase)
{
	return IO32ReadFldAlign(HDMI2_RX_LK_THRS_SVAL1 + u2HdmiBase,RG_HDMI2_RHDMI_AUD_SAMPLE_F);
	
}

/**
 * @brief 	Get TMDS clk frequency, cal from pixel frequency, only avaliable at stable signal
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
UINT32 bHDMI2ComGetTMDSClkFreq(UINT16 u2HdmiBase)
{
    UINT32 u4TMDSClkFreq, u4PixelClkFreq, u410bitClkFreq;
    
    u4PixelClkFreq = dwHDMI2ComPixelClk(u2HdmiBase);
    //The real pixel clk should be divided by 2 for YC420
    if(fgHDMI2ComGet420Mode(u2HdmiBase))
    {
        u4PixelClkFreq = u4PixelClkFreq/2;
    }
    if(bHDMI2ComGetDeepSta(u2HdmiBase)== HDMI_RX_BIT_DEPTH_10_BIT)
	{
		u410bitClkFreq=(u4PixelClkFreq/8)*10;
	}
	else if(bHDMI2ComGetDeepSta(u2HdmiBase)== HDMI_RX_BIT_DEPTH_12_BIT)
	{
		u410bitClkFreq=(u4PixelClkFreq/8)*12;
	}
	else if(bHDMI2ComGetDeepSta(u2HdmiBase)== HDMI_RX_BIT_DEPTH_16_BIT)
	{
		u410bitClkFreq=(u4PixelClkFreq/8)*16;
	}
    else //8 bit
    {
        u410bitClkFreq = u4PixelClkFreq;
    }
    if(fgHDMI2ComGetTmdsClkRatio(u2HdmiBase))
    {
        u4TMDSClkFreq = u410bitClkFreq/4;
    }
    else
    {
        u4TMDSClkFreq = u410bitClkFreq;
    }
    
    return u4TMDSClkFreq;
}

//fs cal
UINT8 bHDMI2AudSampleRateCal(UINT16 u2HdmiBase)
{
	UINT32 wCTS_HW,wN_HW;
	UINT32 wAudSampleRate,wTMDSClkFreq;
	UINT8 btmp;
	static UINT8 Ori_audio_FS;
	
	btmp=20;
	wCTS_HW = dwHDMI2ComGetCTS(u2HdmiBase);
	wN_HW = dwHDMI2ComGetN(u2HdmiBase);
    wTMDSClkFreq = bHDMI2ComGetTMDSClkFreq(u2HdmiBase);

	if(wCTS_HW == 0)
	{
		Printf("invalid CTS value\n");
		return HDMI2_AUD_FS_UNKNOWN;
	}
	if(fgHDMI2ComGetTmdsClkRatio(u2HdmiBase))
	{
		wAudSampleRate= (((4 * wTMDSClkFreq)/wCTS_HW) *((wN_HW*100)/128))/10000;
	}
	else
	{
		wAudSampleRate= ((wTMDSClkFreq/wCTS_HW) *((wN_HW*100)/128))/10000;
	}

    LOG(8,"wTMDSClkFreq is %d,wCTS_HW is %d,wN_HW is %d,wAudSampleRate is %d\n",wTMDSClkFreq,wCTS_HW,wN_HW,wAudSampleRate);
	
	if(wAudSampleRate > (320-btmp) && wAudSampleRate <(320+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_32K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_32K;
    	}
		return HDMI2_AUD_FS_32K;
	}
	else if(wAudSampleRate> (441-btmp) && wAudSampleRate<(441+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_44K)
    	{
    	   Ori_audio_FS=HDMI2_AUD_FS_44K;
      	}
		return HDMI2_AUD_FS_44K;
	}
	else if(wAudSampleRate> (480-btmp) && wAudSampleRate<(480+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_48K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_48K;
    	}

		return HDMI2_AUD_FS_48K;
	}
	else if(wAudSampleRate> (880-btmp) && wAudSampleRate<(880+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_88K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_88K;
    	}
		return HDMI2_AUD_FS_88K;
	}
	else if(wAudSampleRate> (960-btmp) && wAudSampleRate<(960+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_96K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_96K;
    	}
		return HDMI2_AUD_FS_96K;
	}
	else if(wAudSampleRate> (1760-btmp) && wAudSampleRate<(1760+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_176K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_176K;
    	}
		return HDMI2_AUD_FS_176K;
	}	
	else if(wAudSampleRate> (1920-btmp) && wAudSampleRate<(1920+btmp))
	{
	    if (Ori_audio_FS!=HDMI2_AUD_FS_192K)
    	{
    		Ori_audio_FS=HDMI2_AUD_FS_192K;
    	}

		return HDMI2_AUD_FS_192K;
	}
	else
	{
		 return HDMI2_AUD_FS_UNKNOWN;
	}

}


//N/CTS/FS by sw
void vHDMI2ComAudSetN(UINT16 u2HdmiBase,UINT32 u4N)
{
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_N_HW_SW_SEL);
	
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,u4N&0xff,RG_HDMI2_REG_N_VAL_SW7_0);
	
	vIO32WriteFldAlign(HDMI2_RX_N_SVAL2 + u2HdmiBase,(u4N>>8),RG_HDMI2_REG_N_VAL_SW15_8);
	vIO32WriteFldAlign(HDMI2_RX_N_SVAL2 + u2HdmiBase,(u4N>>16),RG_HDMI2_REG_N_VAL_SW19_16);
}

void vHDMI2ComAudSetCTS(UINT16 u2HdmiBase,UINT32 u4CTS)
{
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_CTS_HW_SW_SEL);
	
	vIO32WriteFldAlign(HDMI2_RX_N_HVAL3 + u2HdmiBase,u4CTS&0xff,RG_HDMI2_REG_CTS_VAL_SW7_0);
	
	vIO32WriteFldAlign(HDMI2_RX_N_HVAL3 + u2HdmiBase,(u4CTS>>8),RG_HDMI2_REG_CTS_VAL_SW15_8);
	vIO32WriteFldAlign(HDMI2_RX_N_HVAL3 + u2HdmiBase,(u4CTS>>16),RG_HDMI2_REG_CTS_VAL_SW19_16);
}

void vHDMI2ComAudSetFs(UINT16 u2HdmiBase,UINT8 u4Fs)
{
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_FS_HW_SW_SEL);
	
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,u4Fs&0x3f,RG_HDMI2_REG_FS_VAL_SW);

}


/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
void vHDMI2ComGetChannelStatus(UINT32 HdmiInst)
{
	HdmiRxObject_t *HdmiRx = (HdmiRxObject_t *)((void *)HdmiInst);
	UINT16 u2HdmiBase = HdmiRx->_bHDMIRxBaseAddr;
	UINT8 tmp,tmp1;

	HdmiRx->_AudStatus.chlStatus.fgCopyRight =(BOOL)(IO32ReadFldAlign(HDMI2_RX_CHST1 + u2HdmiBase, RG_HDMI2_BIT2)); //1:copyright;0:no copyright
	HdmiRx->_AudStatus.chlStatus.fgIsPCM =(BOOL)(IO32ReadFldAlign(HDMI2_RX_CHST1 + u2HdmiBase, RG_HDMI2_BIT1));// 1:LPCM,o:other purposses
	HdmiRx->_AudStatus.chlStatus.u1CategoryCode = (IO32ReadFldAlign(HDMI2_RX_CHST1 + u2HdmiBase, RG_HDMI2_BIT15_8));

	tmp = IO32ReadFldAlign(HDMI2_RX_SW_OW + u2HdmiBase, RG_HDMI2_AUD_LENGTH);

	if(tmp == 1)
	{
		tmp1 = 20;
	}
	else if(tmp == 2)
	{
		tmp1 = 22;
	}
	else if(tmp == 4)
	{
		tmp1 = 23;
	}
	else if(tmp == 5)
	{
		tmp1 = 25;
	}
	else if(tmp == 6)
	{
		tmp1 = 21;
	}
	else
	{
		tmp1 = 0;
	}
	
	if(IO32ReadFldAlign(HDMI2_RX_SW_OW + u2HdmiBase, RG_HDMI2_AUD_LENGTH_MAX) == 0)
	{
		if(tmp)
		{
			 tmp1 -= 4;
		}
	}
	
	HdmiRx->_AudStatus.chlStatus.u1WordLength = tmp1;
}

/**
 * @brief 	Get audio layout status
 * @param	Base address by port
 * @retval	1:up to 8 channel,0:2 channel
 */
BOOL fgHDMI2ComGetAudioLayout(UINT16 u2HdmiBase)
{
	return (BOOL)IO32ReadFldAlign(HDMI2_DEC_AV_MUTE + u2HdmiBase,RG_HDMI2_HDMI_LAYOUT);
}

/**
 * @brief 	Get audio layout status
 * @param	Base address by port
 * @retval	1:up to 8 channel,0:2 channel
 */
UINT8 bHDMI2ComGetAudFifo(UINT16 u2HdmiBase)
{
	UINT8 u1Fifo = 0;
	if(IO32ReadFldAlign(HDMI2_RX_INTR1 + u2HdmiBase,RG_HDMI2_REG_INTR4_STAT0))
	{
		u1Fifo |= HDMI_AUDIO_FIFO_UNDERRUN;
	}

	if(IO32ReadFldAlign(HDMI2_RX_INTR1 + u2HdmiBase,RG_HDMI2_REG_INTR4_STAT1))
	{
		u1Fifo |= HDMI_AUDIO_FIFO_OVERRUN;
	}

	if(u1Fifo == 0)
	{
		u1Fifo = HDMI_AUDIO_FIFO_NORMAL;
	}

	return u1Fifo;
		
}

void vHDMI2ComAudStatusSel(UINT16 u2HdmiBase,UINT8 u1On)
{
	//1: APLL status,0:DPLL status
	vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase,u1On,RG_HDMI2_REG_APLL_STATUS_SEL);
}

/**
 * @brief 	Get deep color  status
 * @param	Base address by port
 * @retval	3:16 bit
 			2:12 bit
 			1:10 bit
 			0:8 bit
 */
HdmiRxBitDepth_t bHDMI2ComGetDeepSta(UINT16 u2HdmiBase)
{
	return (HdmiRxBitDepth_t)(IO32ReadFldAlign(HDMI2_RX_AUDP_FILT + u2HdmiBase,RG_HDMI2_RO_COLOUR_DEPTH) & 0x3);
}
#ifdef CC_HDMI_OLD_NEW_SUPPORT
UINT8 bHDMI2ComGetClkRateForDig(UINT16 u2HdmiBase)
{
	UINT8 u1Val=0;
	UINT8 ret=1;
	
	u1Val = IO32ReadFldAlign(HDMI2_TMDS_CKDT2 + u2HdmiBase,RG_HDMI2_TMDS_CK_PERIOD);

	if((u1Val == 60) )
	{
		ret=0;//HDCP 1.4 test
	}
	else
	{
		ret=1;
	}

	return ret;
}
#endif
/**
 * @brief 	Get RG_HDMI2_SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
HdmiTmdsClkRate_t bHDMI2ComGetClkRate(UINT16 u2HdmiBase)
{
	UINT8 u1Val=0;
	UINT8 i=0;
	UINT8 u1ClkRate = HDMI_TMDS_CLOCK_UNKNOW;
#if 1
	UINT8 u1TmdsClk25StbCnt=0;
	UINT8 u1TmdsClk25_35StbCnt=0;
	UINT8 u1TmdsClk35_40StbCnt=0;
	UINT8 u1TmdsClk40_50StbCnt=0;
	UINT8 u1TmdsClk50_80StbCnt=0;
	UINT8 u1TmdsClk80_100StbCnt=0;
	UINT8 u1TmdsClk100_120StbCnt=0;
	UINT8 u1TmdsClk120_140StbCnt=0;
	UINT8 u1TmdsClk140_200StbCnt=0;
	UINT8 u1TmdsClk200_300StbCnt=0;

	while(i< 10)
	{
		u1Val = IO32ReadFldAlign(HDMI2_TMDS_CKDT2 + u2HdmiBase,RG_HDMI2_TMDS_CK_PERIOD);
		
		if(u1Val > 0x3D)
		{
			u1TmdsClk25StbCnt ++;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val > 0x2B)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt++;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val > 0x26)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt++;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val >= 0x1E)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt++;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val > 0x13)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt++;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val > 0xF)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt++;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val >= 0xD)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt++;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val > 0xB)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt++;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt=0;
		}
		else if(u1Val >= 0x7)
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt++;
			u1TmdsClk200_300StbCnt=0;
		}
		else
		{
			u1TmdsClk25StbCnt =0;
			u1TmdsClk25_35StbCnt=0;
			u1TmdsClk35_40StbCnt=0;
			u1TmdsClk40_50StbCnt=0;
			u1TmdsClk50_80StbCnt=0;
			u1TmdsClk80_100StbCnt=0;
			u1TmdsClk100_120StbCnt=0;
			u1TmdsClk120_140StbCnt=0;
			u1TmdsClk140_200StbCnt=0;
			u1TmdsClk200_300StbCnt++;
		}
		i++;
		vUtDelay2us(50);
	}

	if(u1TmdsClk25StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_25;
	}
	else if(u1TmdsClk25_35StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_25_35;
	}
	else if(u1TmdsClk35_40StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_35_40;
	}
	else if(u1TmdsClk40_50StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_40_50;
	}
	else if(u1TmdsClk50_80StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_50_80;
	}
	else if(u1TmdsClk80_100StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_80_100;
	}
	else if(u1TmdsClk100_120StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_100_120;
	}
	else if(u1TmdsClk120_140StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_120_140;
	}
	else if(u1TmdsClk140_200StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_140_200;
	}
	else if(u1TmdsClk200_300StbCnt >= 8)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_200_300;
	}
#endif	

#if 0
	u1Val = IO32ReadFldAlign(HDMI2_TMDS_CKDT2 + u2HdmiBase,RG_HDMI2_TMDS_CK_PERIOD);

    //24M    0x3D:25.18    0x26:40.42    0x13:80.84   0xC:128  0xB:139.636 0x7:219.428
	if(u1Val > 0x3D)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_25;
	}
	else if(u1Val > 0x2B)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_25_35;
	}
	else if(u1Val > 0x26)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_35_40;
	}
	else if(u1Val > 0x1E)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_40_50;
	}
	else if(u1Val > 0x13)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_50_80;
	}
	else if(u1Val > 0xF)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_80_100;
	}
	else if(u1Val >= 0xD)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_100_120;
	}
	else if(u1Val >= 0xB)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_120_140;
	}
	else if(u1Val >= 0x7)
	{
		u1ClkRate = HDMI_TMDS_CLOCK_140_200;
	}
	else
	{
		u1ClkRate = HDMI_TMDS_CLOCK_200_300;
	}
#endif
	return (HdmiTmdsClkRate_t)u1ClkRate;
}


/**
 * @brief 	Get infoframe data
 * @param	info type
 * @retval	1
 **/
BOOL fgHDMI2ComGetInfoframe(UINT32 HdmiInst,UINT8 Type)
{
	UINT8 i=0;
	HdmiRxObject_t *HdmiRx = (HdmiRxObject_t *)((void *)HdmiInst);
	UINT16 u2HdmiBase = HdmiRx->_bHDMIRxBaseAddr;
	UINT32 u1StartAddr=0;

	switch(Type)
	{
		case HDMI_INFO_FRAME_ID_AVI:
			u1StartAddr = HDMI2_RX_AUTO_CLR_PKT1 + u2HdmiBase + 2;
			HdmiRx->_AviInfo.u1InfoID =HDMI_INFO_FRAME_ID_AVI;
			for(i=0;i<18;i++)
			{
				HdmiRx->_AviInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_SPD:
			u1StartAddr = HDMI2_SPDRX_TYPE + u2HdmiBase;
			HdmiRx->_SpdInfo.u1InfoID =HDMI_INFO_FRAME_ID_SPD;
			for(i=0;i<31;i++)
			{
				HdmiRx->_SpdInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_AUD:
			u1StartAddr = HDMI2_AUDRX_TYPE + u2HdmiBase;
			HdmiRx->_AudInfo.u1InfoID =HDMI_INFO_FRAME_ID_AUD;
			for(i=0;i<31;i++)
			{
				HdmiRx->_AudInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_MPEG:
			u1StartAddr = HDMI2_MPEGRX_TYPE + u2HdmiBase;
			HdmiRx->_MpegInfo.u1InfoID =HDMI_INFO_FRAME_ID_MPEG;
			for(i=0;i<31;i++)
			{
				HdmiRx->_MpegInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_UNREC:
			u1StartAddr = HDMI2_RX_UNREC_BYTE1 + u2HdmiBase;
			HdmiRx->_UnrecInfo.u1InfoID =HDMI_INFO_FRAME_ID_UNREC;
			for(i=0;i<31;i++)
			{
				HdmiRx->_UnrecInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_ACP:
			u1StartAddr = HDMI2_RX_ACP_BYTE1 + u2HdmiBase;
			HdmiRx->_AcpInfo.u1InfoID =HDMI_INFO_FRAME_ID_ACP;
			for(i=0;i<31;i++)
			{
				HdmiRx->_AcpInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_VSI:
			u1StartAddr = HDMI2_VSI_CTRL3 + u2HdmiBase + 1;
			HdmiRx->_VsiInfo.u1InfoID =HDMI_INFO_FRAME_ID_VSI;
			for(i=0;i<31;i++)
			{
				HdmiRx->_VsiInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
			}
			break;
		case HDMI_INFO_FRAME_ID_ISRC0:
			u1StartAddr = HDMI2_RX_ISRC1_TYPE + u2HdmiBase;
			HdmiRx->_Isrc0Info.u1InfoID =HDMI_INFO_FRAME_ID_ISRC0;
			for(i=0;i<19;i++)
			{
				
				HdmiRx->_Isrc0Info.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
				if(i == 2)
				{
					u1StartAddr = HDMI2_RX_ISRC1_TYPE + u2HdmiBase + 1;
				}
			}
			break;
		case HDMI_INFO_FRAME_ID_ISRC1:
			u1StartAddr = HDMI2_RX_ISRC2_TYPE + u2HdmiBase;
			HdmiRx->_Isrc1Info.u1InfoID =HDMI_INFO_FRAME_ID_ISRC1;
			for(i=0;i<19;i++)
			{
				
				HdmiRx->_Isrc1Info.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
				if(i == 2)
				{
					u1StartAddr = HDMI2_RX_ISRC1_TYPE + u2HdmiBase + 1;
				}
			}
			break;
		case HDMI_INFO_FRAME_ID_GCP:
			u1StartAddr = HDMI2_RX_GCP_TYPE + u2HdmiBase;
			HdmiRx->_GcpInfo.u1InfoID =HDMI_INFO_FRAME_ID_GCP;
			for(i=0;i<10;i++)
			{
				
				HdmiRx->_GcpInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
				if(i == 2)
				{
					u1StartAddr = HDMI2_RX_ISRC1_TYPE + u2HdmiBase + 1;
				}
			}
			break;
		case HDMI_INFO_FRAME_ID_HFVSI:
			u1StartAddr = HDMI2_HF_VSIRX_TYPE + u2HdmiBase;
			HdmiRx->_GcpInfo.u1InfoID =HDMI_INFO_FRAME_ID_HFVSI;
			for(i=0;i<31;i++)
			{
				
				HdmiRx->_GcpInfo.u1InfoData[i] = u1IO32Read1B(u1StartAddr + i);
				if(i == 2)
				{
					u1StartAddr = HDMI2_RX_ISRC1_TYPE + u2HdmiBase + 1;
				}
			}
			
			break;
		default:
			break;

	}

	return 1;
}

//parser AVI infoframe
HdmiVideoInfo_t bHDMI2ComAviParser(UINT16 u2HdmiBase)
{
	UINT8 u1Colorimetry;
	UINT8 u1ExtColorrimetry;
	UINT8 u1ColorInfo;
	UINT8 u1Quant;
	UINT8 u1Yquant;

	HdmiVideoInfo_t vInfo;
	
	u1Colorimetry =	(u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HdmiBase + 3) & 0xC0) >>6;
	u1ExtColorrimetry = (u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HdmiBase) & 0x70) >>4;
	u1ColorInfo = (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HdmiBase + 2) & 0x60) | (u1Colorimetry <<3) | u1ExtColorrimetry;
	u1Quant = (u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HdmiBase) & 0x06) >>2;
	u1Yquant = (u1IO32Read1B(HDMI2_AVIRX_DBYTE3 + u2HdmiBase + 2) & 0xc0)>>6;
	
	switch(u1ColorInfo)
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			vInfo.clrSpc =HDMI_RX_CLRSPC_RGB;
			break;
		case 0x1C:
			vInfo.clrSpc =HDMI_RX_CLRSPC_Adobe_RGB;
			break;
		case 0x1E:
			vInfo.clrSpc =HDMI_RX_CLRSPC_BT_2020_RGB_non_const_luminous;
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
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC422_601;
			break;
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC422_709;
			break;
		case 0x38:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC422_601;
			break;
		case 0x39:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC422_709;
			break;
		case 0x3A:
			vInfo.clrSpc = HDMI_RX_CLRSPC_sYCC422_601;
			break;
		case 0x3B:
			vInfo.clrSpc = HDMI_RX_CLRSPC_Adobe_YCC422_601;
			break;
		case 0x3D:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC422_const_luminous;
			break;
		case 0x3E:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC422_non_const_luminous;
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
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC444_601;
			break;
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x55:
		case 0x56:
		case 0x57:
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC444_709;
			break;
		case 0x58:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC444_601;
			break;
		case 0x59:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC444_709;
			break;
		case 0x5A:
			vInfo.clrSpc = HDMI_RX_CLRSPC_sYCC444_601;
			break;
		case 0x5B:
			vInfo.clrSpc = HDMI_RX_CLRSPC_Adobe_YCC444_601;
			break;
		case 0x5D:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC444_const_luminous;
			break;
		case 0x5E:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC444_non_const_luminous;
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
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC420_601;
			break;
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
			vInfo.clrSpc = HDMI_RX_CLRSPC_YC420_709;
			break;
		case 0x78:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC420_601;
			break;
		case 0x79:
			vInfo.clrSpc = HDMI_RX_CLRSPC_XVYC420_709;
			break;
		case 0x7A:
			vInfo.clrSpc = HDMI_RX_CLRSPC_sYCC420_601;
			break;
		case 0x7B:
			vInfo.clrSpc = HDMI_RX_CLRSPC_Adobe_YCC420_601;
			break;
		case 0x7D:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC420_const_luminous;
			break;
		case 0x7E:
			vInfo.clrSpc = HDMI_RX_CLRSPC_BT_2020_YCC420_non_const_luminous;
			break;
		default:
			vInfo.clrSpc =HDMI_RX_CLRSPC_RGB;
			break;	
	}

	if(u1Quant ==1)
	{
		//RGB limit
		vInfo.range = HDMI_RX_RGB_LIMT;
	}
	else if(u1Quant ==2)
	{
		//RGB full
		vInfo.range = HDMI_RX_RGB_FULL;
	}

	if(u1Yquant == 1)
	{
		//YCC limit
		vInfo.range = HDMI_RX_YCC_LIMT;
	}
	else if(u1Yquant ==2)
	{
		//YCC full
		vInfo.range = HDMI_RX_YCC_FULL;
	}

	return vInfo;
}
void vHDMI2ComHfVsiParser(UINT16 u2HdmiBase)
{
	
}

void vHDMI2ComVsiParser(UINT16 u2HdmiBase)
{
	
}

/**
 * @brief set DDC enable or disable
 * @param	Base address by port,enable/disable
 * @retval	none
 **/
void vHDMI2ComSetHdcpDDC(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,u1En,RG_HDMI2_REG_DDC_EN);
}

// scdc set
void vHDMI2ComSetScdc(UINT8 u1En)
{
	vIO32WriteFldAlign(SCDC_REG00,u1En,REG_SCDC_DDC_EN);
}
void vHDMI2ComClearScdcStatus(void)
{
	vIO32WriteFldAlign(SCDC_REG10,0,REG_PWR5V);
	vIO32WriteFldAlign(SCDC_REG10,0,REG_HPD_IN);
	
	vUtDelay2us(1);
	
	vIO32WriteFldAlign(SCDC_REG10,1,REG_PWR5V);
	vIO32WriteFldAlign(SCDC_REG10,1,REG_HPD_IN);
}


/**
 * @brief 	set split mode enable or disable
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
void vHDMI2ComSetSplit(UINT16 u2HdmiBase,UINT8 u1En)
{
	UINT8 tmp = 0;
	tmp = u1En > 0 ? 0:1;
	
	vIO32WriteFldAlign(HDMI2_SPLIT_DITHER1 + u2HdmiBase,tmp,RG_HDMI2_REG_SPLIT_EN);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,(!tmp),RG_HDMI2_REG_SPLITCLK_SEL1);

	//vHDMI2ComDigPhyRst1(u2HdmiBase);
	//vHDMI2ComSplitRst(u2HdmiBase);
}

/**
 * @brief 	set Scrambling mode enable or disable by sw,used for some 1.4 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
void vHDMI2ComSetScrambling(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,u1En,RG_HDMI2_REG_SCRAMBLING_ON);
}

/**
 * @brief 	set 420 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
void vHDMI2ComSet420(UINT16 u2HdmiBase,UINT8 u1En)
{
	if(u1En)
	{
		vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_DPCLK2X_SEL1);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,1,RG_HDMI2_MUX_420_ENABLE);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,0,RG_HDMI2_SELECT_INPUT_Y);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,1,RG_HDMI2_SELECT_INPUT_CB);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,2,RG_HDMI2_SELECT_INPUT_CR);
		
	}
	else
	{
		vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_DPCLK2X_SEL1);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,0,RG_HDMI2_MUX_420_ENABLE);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,0,RG_HDMI2_SELECT_INPUT_Y);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,1,RG_HDMI2_SELECT_INPUT_CB);
		vIO32WriteFldAlign(HDMI2_INPUT_FORMAT + u2HdmiBase,2,RG_HDMI2_SELECT_INPUT_CR);
	}
	
}

/**
 * @brief 	set 420 to 422 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
void vHDMI2ComSet420To422(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_C420_C422_CONFIG + u2HdmiBase,u1En,RG_HDMI2_C420_C422_ENABLE);
}

void vHDMI2ComShermanRst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_SHERMAN_RST + u2HdmiBase,0,RG_HDMI2_REG_SHERMAN_SW_RST);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_SHERMAN_RST + u2HdmiBase,1,RG_HDMI2_REG_SHERMAN_SW_RST);
}

void vHDMI2ComSwRst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_SW_RST);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_SW_RST);
}

/**
 * @brief 	set 422 to 444 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
void vHDMI2ComSet422To444(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_C422_C444_CONFIG + u2HdmiBase,u1En,RG_HDMI2_C422_C444_ENABLE);
}


void vHDMI2ComVideoInit(UINT16 u2HdmiBase)
{
	vHDMI2ComShermanRst(u2HdmiBase);
	
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_MBIST0);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_MBIST1);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_MBIST2);
	
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_DPCLK_SEL0);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_DPCLK_SEL1);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_DPCLK2X_SEL0);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_DPCLK2X_SEL1);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_SPLITCLK_SEL0);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_SPLITCLK_SEL1);

	vIO32WriteFldAlign(HDMI2_SPLIT_DITHER1 + u2HdmiBase,1,RG_HDMI2_REG_SPLIT_EN);
	
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_CCLK_SEL);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_TMDS_CLK_INV);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,2,RG_HDMI2_REG_MCLK_SEL);//Apll gen as mclk

	//power on clk
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_PD_TMDS_CLK);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_PD_DPCLK);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,1,RG_HDMI2_REG_PD_DPCLK2X);
	
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,2,RG_HDMI2_REG_SCL_DEGLITCH_CNT);
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,3,RG_HDMI2_REG_SDA_DEGLITCH_CNT);

    vIO32WriteFldAlign(HDMI2_TOP_MISC + u2HdmiBase,4,RG_HDMI2_REG_DSCL_DEGLITCH_CNT);
	vIO32WriteFldAlign(HDMI2_TOP_MISC + u2HdmiBase,3,RG_HDMI2_REG_DSDA_DEGLITCH_CNT);
	
	//clock reset
	vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0xf,RG_HDMI2_REG_CKGEN_RST);

	if(IS_IC_5861_ES1())
	{
		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,0,RG_HDMI2_HW_SCRAMBLING_ON_EN);
	}
	else
	{
		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,1,RG_HDMI2_HW_SCRAMBLING_ON_EN);
	}
	
	//en htotal/vtotal/hactive/vactive/hfront/hback/vfront/vback/frame rate reading
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_FRAME_RATE_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,7,RG_HDMI2_FDET_FRAME_RATE_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_PIXEL_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_PIXEL_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_LINE_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_LINE_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_HSYNC_LOW_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_HSYNC_LOW_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_HSYNC_HIGH_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_HSYNC_HIGH_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_HFRONT_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_HFRONT_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_HBACK_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,3,RG_HDMI2_FDET_HBACK_COUNT_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE1 + u2HdmiBase,0,RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VSYNC_LOW_COUNT_EVEN_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_EVEN_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VFRONT_COUNT_EVEN_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VFRONT_COUNT_EVEN_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VBACK_COUNT_EVEN_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VBACK_COUNT_EVEN_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VSYNC_LOW_COUNT_ODD_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VBACK_COUNT_EVEN_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VBACK_COUNT_EVEN_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VSYNC_HIGH_COUNT_ODD_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VFRONT_COUNT_ODD_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VFRONT_COUNT_ODD_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_VBACK_COUNT_ODD_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,3,RG_HDMI2_FDET_VBACK_COUNT_ODD_RD_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE2 + u2HdmiBase,0,RG_HDMI2_FDET_FRAME_COUNT_WR_EN);
	vIO32WriteFldAlign(HDMI2_RESIGET_RW_ENABLE3 + u2HdmiBase,3,RG_HDMI2_FDET_FRAME_COUNT_RD_EN);


    /*enable HDMi2.0 edid I2C deglitch*/
    vIO32WriteFldAlign(PDWNC_EDID2_DEV0, 1, FLD_EDID2_DEGLITCH0_EN);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV0, 3, FLD_EDID2_DE_CNT0);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV1, 1, FLD_EDID2_DEGLITCH1_EN);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV1, 3, FLD_EDID2_DE_CNT1);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV2, 1, FLD_EDID2_DEGLITCH2_EN);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV2, 3, FLD_EDID2_DE_CNT2);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV3, 1, FLD_EDID2_DEGLITCH3_EN);
    vIO32WriteFldAlign(PDWNC_EDID2_DEV3, 3, FLD_EDID2_DE_CNT3);

	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,0,RG_HDMI2_REG_PREDFF_CLK_INV);
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,0,RG_HDMI2_REG_HDCP_RE_AUTH_EN);
//hdcp2 rst
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CRST_N);
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,0,RG_HDMI2_REG_PRAM_8051_CNTL_SEL);

}

void vHDMI2ComAudCfg(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_1ST_BIT);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_DATA_DIR);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_JUSTIFY);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_WS);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_MSB);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_SIZE);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_CLK_EDGE);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_INVALID_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_PCM);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_VUCP);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_MUTE_FLAT);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_MCLK_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SD0_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SD1_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SD2_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SD3_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_SD0_MAP);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SD1_MAP);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,2,RG_HDMI2_REG_SD2_MAP);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,3,RG_HDMI2_REG_SD3_MAP);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SPDIF_EN);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_SPDIF_MODE);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_I2S_MODE);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_PASS_AUD_ERR);
	vIO32WriteFldAlign(HDMI2_RX_I2S_CTRL1 + u2HdmiBase,1,RG_HDMI2_REG_PASS_SPDIF_ERR);

}

void vHDMI2ComAudioSigmaDeltaSetting(UINT16 u2HdmiBase,UINT32 value1, UINT8 value2, UINT8 bSDINOVR)
{

	
	vIO32WriteFldAlign(HDMI2_ACR_CONTROL5 + u2HdmiBase, 3, RG_HDMI2_SDREF);
    //The following is for software mode DDS Setting,just for pll default
    if((value1 == 0x205bc01a) || (bHDMI2AudSampleRateCal(u2HdmiBase) == HDMI2_AUD_FS_44K)
		||(bHDMI2AudSampleRateCal(u2HdmiBase) == HDMI2_AUD_FS_88K)
		||(bHDMI2AudSampleRateCal(u2HdmiBase) == HDMI2_AUD_FS_176K))
    {
    	vIO32WriteFldAlign(HDMI2_ACR_CONTROL11 + u2HdmiBase, 0x5A5119CE, RG_HDMI2_REG_HADDS2_PCW_SW);
    }
    else
    {
        //48KHz
        //for DDS mode HDMI setting
        vIO32WriteFldAlign(HDMI2_ACR_CONTROL11 + u2HdmiBase, 0x624DD2F1, RG_HDMI2_REG_HADDS2_PCW_SW); 
    }

    if((value1==0)&&(value2==0)&&(bSDINOVR==0))
    {
        vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase, 0, RG_HDMI2_PCW_DATA_SEL);   // 0: HW mode PCW
    }
    else
    {
        vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase, 1, RG_HDMI2_PCW_DATA_SEL);	// 1: SWmode PCW
    }

    vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase, 1, RG_HDMI2_PCW_CHG_SW);
    vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase, 0, RG_HDMI2_PCW_CHG_SW);

	vIO32WriteFldAlign(HDMI2_ACR_CONTROL12 + u2HdmiBase, 3, RG_HDMI2_PCW_DATA_DIV);
	
}


void vHDMI2ComAudioInit(UINT16 u2HdmiBase)
{
	// need to confirm sequence
	
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_ACR_INIT);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_FS_HW_SW_SEL);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_CTS_REUSED_AUTO_EN);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_N_HW_SW_SEL);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_CTS_HW_SW_SEL);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_UPLL_HW_SW_SEL);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_CTS_DROPPED_AUTO_EN);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,3,RG_HDMI2_REG_MCLK4DSD);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_MCLK4HBRA);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_MCLK4DAC);

	vIO32WriteFldAlign(HDMI2_TOP_MISC + u2HdmiBase,1,RG_HDMI2_REG_HW_DSD_FEATURE);
	vIO32WriteFldAlign(HDMI2_TOP_MISC + u2HdmiBase,1,RG_HDMI2_REG_HW_HBR_FEATURE);
	vIO32WriteFldAlign(HDMI2_TOP_MISC + u2HdmiBase,1,RG_HDMI2_REG_HW_8CH_FEATURE);

	vIO32WriteFldAlign(HDMI2_ACR_CONTROL1 + u2HdmiBase,1,RG_HDMI2_POST_DIV2_ENA);
	vIO32WriteFldAlign(HDMI2_ACR_CONTROL1 + u2HdmiBase,0,RG_HDMI2_FM_VAL_SW);
	vIO32WriteFldAlign(HDMI2_RX_ACR_CTRL1 + u2HdmiBase,0,RG_HDMI2_REG_FM_VAL_SW);
	
	vHDMI2ComAudCfg(u2HdmiBase);
	vHDMI2ComAudioSigmaDeltaSetting(u2HdmiBase,0,0,0);

}


void vHDMI2ComAudPllSel(UINT16 u2HdmiBase,UINT8 u1Pll)
{
	if(u1Pll >= 1)//APLL 
	{
		vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,2,RG_HDMI2_REG_MCLK_SEL);//Apll gen as mclk
		vIO32WriteFldAlign(HDMI2_ACR_CONTROL1 + u2HdmiBase,1,RG_HDMI2_POST_DIV2_ENA);
		vIO32WriteFldAlign(HDMI2_ACR_CONTROL1 + u2HdmiBase,0,RG_HDMI2_FM_VAL_SW);
		vHDMI2ComAudioSigmaDeltaSetting(u2HdmiBase,0,0,0);
	}
	else
	{
		vIO32WriteFldAlign(HDMI2_CKGEN_CONTROL + u2HdmiBase,0,RG_HDMI2_REG_MCLK_SEL);//Apll gen as mclk
		vIO32WriteFldAlign(CKGEN_REG_HDMI3_MON2_CKCFG,1,FLD_HDMI2_DPLL_REF_CK_SEL);//324M refence clock
		
	}
	vHDMI2ComAcrRst(u2HdmiBase);
}


// after rst,8051 state machine

//auth done/fail

UINT8 bHDMI2ComGetHdcp1Status(UINT16 u2HdmiBase)
{
	HdmiRxHdcp1Status_t hdcpSta = HDMI_RX_HDCP1_STATUS_OFF;
	
	if(IO32ReadFldAlign( HDMI2_RX_HDCP_STAT + u2HdmiBase,RG_HDMI2_HDCP_AUTHENTICATED))
	{
		hdcpSta |= HDMI_RX_HDCP1_STATUS_AUTH_DONE;
	}
	else
	{
		hdcpSta |= HDMI_RX_HDCP1_STATUS_AUTH_FAIL;
	}


	if(IO32ReadFldAlign( HDMI2_RX_HDCP_STAT + u2HdmiBase,RG_HDMI2_HDCP_DECRYPTING))
	{
		hdcpSta |= HDMI_RX_HDCP1_STATUS_DECRYPT_SUCCESS;
	}
	else
	{
		hdcpSta |= HDMI_RX_HDCP1_STATUS_DECRYPT_FAIL;
	}

	return hdcpSta;
	
}

UINT8 bHDMI2ComGetHdcp2Status(UINT16 u2HdmiBase)
{
	HdmiRxHdcp2Status_t hdcpSta = HDMI_RX_HDCP2_STATUS_OFF;

	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR0_SRC0))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_AUTH_DONE;
	}
	
	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR0_SRC1))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_AUTH_FAIL;
	}
	
	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR0_SRC4))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_CCHK_DONE;
	}
	
	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR0_SRC5))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_CCHK_FAIL;
	}
	
	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR1_SRC2))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_AKE_SENT_RCVD;
	}

	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR1_SRC2))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_SKE_SENT_RCVD;
	}
	
	if(IO32ReadFldAlign( HDMI2_RXHDCP2X_CMD_INTR0 + u2HdmiBase,RG_HDMI2_REG_INTR1_SRC6))
	{
		hdcpSta |= HDMI_RX_HDCP2_STATUS_CERT_SENT_RCVD;
	}
	return hdcpSta;
}


//HDCP1.4/2.2 mode change by sw ????

void vHDMI2ComHDCP2ReAuthSet(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,1,RG_HDMI2_REG_HDCP_RE_AUTH_EN);
}

void vHDMI2ComDdcSel(UINT16 u2HdmiBase,UINT8 u1Type)
{
	if(u1Type)//HDMI2.0 port ddc engine reply ack
	{
		vIO32WriteFldAlign(PDWNC_PDMISC,1,FLD_HDMI2_DDC_ENGIGNE_SEL);
	}
	else //only HDMI1.4 port ddc engine reply ack
	{
		vIO32WriteFldAlign(PDWNC_PDMISC,0,FLD_HDMI2_DDC_ENGIGNE_SEL);
	}
}

void vHDMI2ComSigSel(UINT16 u2HdmiBase,UINT8 u1Type)
{
	if(u1Type) 
	{
		vIO32WriteFldAlign(HDMI_SEL, 0, REG_HDMI2_AUD_SEL); //HDMi2.0 audio
		vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VIDEO_SEL);//HDMi2.0 video
	}
	else  
	{
		vIO32WriteFldAlign(HDMI_SEL, 1, REG_HDMI2_AUD_SEL); //HDMi2.0 audio
		vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_VIDEO_SEL);//HDMi2.0 video
	}
}

//after HDCP2.2 key load to SRAM, call this API
void vHDMI2ComHDCP2Init(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase,1,RG_HDMI2_REG_PRAM_8051_CNTL_SEL);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_CRST_N);
	
	
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_RSVD + u2HdmiBase,0x80,RG_HDMI2_REG_HDCP2X_CUPD_SIZE7_0);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_RSVD + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CUPD_SIZE15_8);
	
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP0 + u2HdmiBase,2,RG_HDMI2_REG_HDCP2X_TP0);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP0 + u2HdmiBase,140,RG_HDMI2_REG_HDCP2X_TP1);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP0 + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_TP2);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP0 + u2HdmiBase,0x96,RG_HDMI2_REG_HDCP2X_TP3);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP4 + u2HdmiBase,17,RG_HDMI2_REG_HDCP2X_TP4);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP4 + u2HdmiBase,100,RG_HDMI2_REG_HDCP2X_TP5);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP4 + u2HdmiBase,2,RG_HDMI2_REG_HDCP2X_TP6);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP4 + u2HdmiBase,9,RG_HDMI2_REG_HDCP2X_TP7);

	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP8 + u2HdmiBase,9,RG_HDMI2_REG_HDCP2X_TP8);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP8 + u2HdmiBase,17,RG_HDMI2_REG_HDCP2X_TP9);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP8 + u2HdmiBase,17,RG_HDMI2_REG_HDCP2X_TP10);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP8 + u2HdmiBase,167,RG_HDMI2_REG_HDCP2X_TP11);

	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP12 + u2HdmiBase,125,RG_HDMI2_REG_HDCP2X_TP12);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP12 + u2HdmiBase,13,RG_HDMI2_REG_HDCP2X_TP13);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP12 + u2HdmiBase,250,RG_HDMI2_REG_HDCP2X_TP14);
	//vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_TP12 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_TP15);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_GP_IN0 + u2HdmiBase,0x02,RG_HDMI2_REG_HDCP2X_GP_IN1);


	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_HDCPTX);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_RI_REPEATER);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_HDMIMODE);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_PRECOMPUTE);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_STM_ID);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_REAUTH_SW);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CTL3MSK);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0xf,RG_HDMI2_REG_HDCP2X_REAUTH_MSK);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_REAUTH_SW);
	
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0xa,RG_HDMI2_REG_HDCP2X_CPVER);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CUPD_DONE);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_CUPD_START);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_CUPD_DONE);

	vUtDelay2us(10);

	while(!(bHDMI2ComGetHdcp2Status(u2HdmiBase) & HDMI_RX_HDCP2_STATUS_CCHK_DONE))
	{
	    u1CchkCheckCount++;
        if(u1CchkCheckCount > HDMI2_HDCP2_CCHK_DONE_CHECK_COUNT)
        {
            LOG(0,"HDCP2 Error:Check CCHK done fail!\n");
            break;
        }
		vUtDelay2us(10);
	}
    u1CchkCheckCount = 0;

	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CUPD_DONE);
	vIO32WriteFldAlign(HDMI2_RXHDCP2X_CMD_CTRL_0 + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CUPD_START);

}

void vHDMI2ComDigPhyRst1(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,0,RG_HDMI2_REG_DIG_PHY_RSTB);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,1,RG_HDMI2_REG_DIG_PHY_RSTB);
}

void vHDMI2ComDigPhyRst(UINT16 u2HdmiBase,UINT8 u1en)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,u1en,RG_HDMI2_REG_DIG_PHY_RSTB);
}
//split reset
void vHDMI2ComSplitRst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,0,RG_HDMI2_REG_VID_SPLIT_RSTB);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL1 + u2HdmiBase,1,RG_HDMI2_REG_VID_SPLIT_RSTB);
}

void vHDMI2ComAcrRst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_ACR_RST);
	vIO32WriteFldAlign(HDMI2_SYS_TMDS_CH_MAP + u2HdmiBase,1,RG_HDMI2_REG_DACR_RST);

	vUtDelay2us(1);
	
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_ACR_RST);
	vIO32WriteFldAlign(HDMI2_SYS_TMDS_CH_MAP + u2HdmiBase,0,RG_HDMI2_REG_DACR_RST);
}

void vHDMI2ComFifoRst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_FIFO_RST);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_FIFO_RST);
}

void vHDMI2ComDeepRst1(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_DC_FIFO_RST);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_DC_FIFO_RST);
}

void vHDMI2ComDeepRst(UINT16 u2HdmiBase,UINT8 u1En)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,u1En,RG_HDMI2_REG_DC_FIFO_RST);
}

// HDCP2/1 rst
void vHDMI2ComHdcp1Rst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_HDCP_RST);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_HDCP_RST);
}
//?????
void vHDMI2ComHdcp2Rst(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,0,RG_HDMI2_REG_HDCP2X_CRST_N);
	vUtDelay2us(1);
	vIO32WriteFldAlign(HDMI2_RX_PWD_CTRL + u2HdmiBase,1,RG_HDMI2_REG_HDCP2X_CRST_N);
}

//SCDC ecc error-> reauthentication

// set reauthentication

//scdc other status

//APLL relatived


void vHDMI2ComRxInit(UINT16 u2HdmiBase)
{
    vIO32Write1B(PD_SYS_0 ,0x01);
    vIO32Write1B(PD_SYS_2 , 0xCf);
    //vIO32Write1B(PD_SYS_3,0x3f);  // except Tri-state Audio/Video output
    vIO32Write1B(PD_SYS_3 , 0xff); // except Tri-state Audio/Video output
    vIO32Write1B(PD_SYS_3 , 0xf3);	//solve analog PHY bandgap start-up
    vIO32Write1B(PD_SYS_3 , 0xff); //solve analog PHY bandgap start-up
	vHDMI2ComVideoInit(u2HdmiBase);
	vHDMI2ComAudioInit(u2HdmiBase);
}

void vHDMI2ComSet420To444(UINT16 u2HdmiBase,UINT8 u1En)
{
	vHDMI2ComSet420(u2HdmiBase,u1En);
	vHDMI2ComSet420To422(u2HdmiBase,u1En);
	vHDMI2ComSet422To444(u2HdmiBase,u1En);
}

void vHDMI2ComGetVideoInfoByAVI(UINT16 u2HdmiBase)
{
	HdmiVideoInfo_t videoInfo;
	videoInfo = bHDMI2ComAviParser(u2HdmiBase);

	if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC444_601)
	{
		Printf("YC444_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC422_601)
	{
		Printf("YC422_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC420_601)
	{
		Printf("YC420_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC444_709)
	{
		Printf("YC444_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC422_709)
	{
		Printf("YC422_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_YC420_709)
	{
		Printf("YC420_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC444_601)
	{
		Printf("XVYC444_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC422_601)
	{
		Printf("XVYC422_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC420_601)
	{
		Printf("XVYC420_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC444_709)
	{
		Printf("XVYC444_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC422_709)
	{
		Printf("XVYC422_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_XVYC420_709)
	{
		Printf("XVYC420_709\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_sYCC444_601)
	{
		Printf("sYCC444_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_sYCC422_601)
	{
		Printf("sYCC422_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_sYCC420_601)
	{
		Printf("sYCC420_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_Adobe_YCC444_601)
	{
		Printf("Adobe YCC444_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_Adobe_YCC422_601)
	{
		Printf("Adobe YCC422_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_Adobe_YCC420_601)
	{
		Printf("Adobe YCC420_601\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_RGB)
	{
		Printf("RGB\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_Adobe_RGB)
	{
		Printf("Adobe RGB\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_RGB_non_const_luminous)
	{
		Printf("BT_2020_RGB_non_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_RGB_const_luminous)
	{
		Printf("BT_2020_RGB_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC444_const_luminous)
	{
		Printf("BT_2020_YCC444_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC444_non_const_luminous)
	{
		Printf("BT_2020_YCC444_non_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC422_non_const_luminous)
	{
		Printf("BT_2020_YCC422_non_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC422_const_luminous)
	{
		Printf("BT_2020_YCC422_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC420_const_luminous)
	{
		Printf("BT_2020_YCC420_const_luminous\n");
	}
	else if(videoInfo.clrSpc == HDMI_RX_CLRSPC_BT_2020_YCC420_non_const_luminous)
	{
		Printf("BT_2020_YCC420_non_const_luminous\n");
	}
	
}
void vHDMI2ComGetVideoStatus(UINT16 u2HdmiBase)
{
	if(fgHDMI2ComGetCKDT(u2HdmiBase))
	{
		Printf("CKDT detected\n");
	}
	else
	{
		Printf("No CKDT\n");
	}

	if(fgHDMI2ComGetSCDT(u2HdmiBase))
	{
		Printf("SCDT detected\n");
	}
	else
	{
		Printf("NO SCDT\n");
	}
	
	Printf("width: %d\n", dwHDMI2ComHActive(u2HdmiBase));
	Printf("height: %d\n", dwHDMI2ComVActive(u2HdmiBase));
	Printf("htotal: %d\n", dwHDMI2ComHTotal(u2HdmiBase));
	Printf("vtotal: %d\n", dwHDMI2ComVTotal(u2HdmiBase));
	Printf("pixel freq: %dHZ\n", dwHDMI2ComPixelClk(u2HdmiBase));
	Printf("refresh rate: %d\n", dwHDMI2ComFrameRate(u2HdmiBase));

	if (fgHDMI2ComGetInterlaced(u2HdmiBase))
	{
		Printf("interface signal\n");
	}
	else
	{
		Printf("progressive signal\n");
	}

	if(fgHDMI2ComGetHdmiMode(u2HdmiBase))
	{
		Printf("HDMI mode\n");
	}
	else
	{
		Printf("DVI mode\n");
	}

	vHDMI2ComGetVideoInfoByAVI(u2HdmiBase);

	if(bHDMI2ComGetDeepSta(u2HdmiBase) == HDMI_RX_BIT_DEPTH_8_BIT)
	{
		Printf("8 bit\n");
	}
	else if(bHDMI2ComGetDeepSta(u2HdmiBase) == HDMI_RX_BIT_DEPTH_10_BIT)
	{
		Printf("10 bit\n");
	}
	else if(bHDMI2ComGetDeepSta(u2HdmiBase) == HDMI_RX_BIT_DEPTH_12_BIT)
	{
		Printf("12 bit\n");
	}
	else
	{
		Printf("16 bit\n");
	}
	
	if(fgHDMI2ComGetScramblingBySCDC(u2HdmiBase))
	{
		Printf("HDMI2.0 and Tx scrambling en\n");
	}
	else
	{
		Printf("Tx scrambling dis\n");
	}

	if(fgHDMI2ComGetTmdsClkRatio(u2HdmiBase))
	{
		Printf("TMDS clock ratio-1:40\n");
	}
	else
	{
		Printf("TMDS clock ratio-1:10\n");
	}
	
}

void vHDMI2ComGetAudioStatus(UINT16 u2HdmiBase)
{
	UINT8 u1FifoSta=0;
	UINT8 u1Fs=0;
	u1FifoSta = bHDMI2ComGetAudFifo(u2HdmiBase);
	
	if(u1FifoSta & HDMI_AUDIO_FIFO_NORMAL)
	{
		Printf("HDMI audio fifo ok\n");
	}

	if(u1FifoSta & HDMI_AUDIO_FIFO_OVERRUN)
	{
		Printf("HDMI audio fifo overrun\n");
	}

	if(u1FifoSta & HDMI_AUDIO_FIFO_UNDERRUN)
	{
		Printf("HDMI audio fifo underun\n");
	}

	u1Fs = bHDMI2ComGetFs(u2HdmiBase);

	switch(u1Fs)
	{
		case HDMI2_AUD_FS_44K:
			Printf("Audio Fs=44.1K\n");
			break;
		case HDMI2_AUD_FS_88K:
			Printf("Audio Fs=88.2K\n");
			break;
		case HDMI2_AUD_FS_176K:
			Printf("Audio Fs=176K\n");
			break;
		case HDMI2_AUD_FS_48K:
			Printf("Audio Fs=48K\n");
			break;
		case HDMI2_AUD_FS_96K:
			Printf("Audio Fs=96K\n");
			break;
		case HDMI2_AUD_FS_192K:
			Printf("Audio Fs=192K\n");
			break;
		case HDMI2_AUD_FS_32K:
			Printf("Audio Fs=32K\n");
			break;
		case HDMI2_AUD_FS_768K:
			Printf("Audio Fs=768K\n");
			break;
		default:
			Printf("invalid audio\n");
			break;
	}

	Printf("Audio CTS = %d\n",dwHDMI2ComGetCTS(u2HdmiBase));
	Printf("Audio  N  = %d\n",dwHDMI2ComGetN(u2HdmiBase));

	if(fgHDMI2ComGetAudioLayout(u2HdmiBase))
	{
		Printf("multi channel audio(up to 8)\n");
	}
	else 
	{
		Printf("2 channel audio\n");
	}
	
}

BOOL fgHDMI2ComGet420Mode(UINT16 u2HdmiBase)
{
	UINT8 u1ColorInfo;
	BOOL bIs420 = FALSE;

	if(fgHDMI2ComGetHdmiMode(u2HdmiBase))
	{
		if(IO32ReadFldAlign(HDMI2_RX_AUTO_CLR_PKT1 + u2HdmiBase,RG_HDMI2_CEA_AVI_HEADER0) == 0x82)
		{
			u1ColorInfo = (u1IO32Read1B(HDMI2_AVIRX_LENGTH + u2HdmiBase + 2) & 0x60);
			if(u1ColorInfo == 0x60)
			{
				bIs420 = TRUE;
			}
		}
	}

	return bIs420;
	
}

void vHDMI2ComSetClk(UINT16 u2HdmiBase)
{
    vHDMI2ComNewDigSel(u2HdmiBase,1);
}


void vHDMI2ComNewDigSel(UINT16 u2HdmiBase,UINT8 u1En)
{
	if(u1En)//HDMI2.0
	{
		vIO32Write4B(CKGEN_VPCLK_STOP,0);

		
		vIO32WriteFldAlign(HDMI_SEL, 0, REG_HDMI2_AUD_SEL); //HDMi2.0 audio
			
		vIO32WriteFldAlign(CCIR_04_NEW, 1, CCH_VIDEO_SEL);//HDMi2.0 video
		

		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase, 1, RG_HDMI2_REG_APLL_CNTL_SEL);

		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase, 1, RG_HDMI2_REG_ANA_CNTL_SEL);
	}
	else //HDMi1.4 
	{
		vIO32Write4B(CKGEN_VPCLK_STOP,0);
		
		vIO32WriteFldAlign(HDMI_SEL, 1, REG_HDMI2_AUD_SEL); //HDMi2.0 audio
		vIO32WriteFldAlign(CCIR_04_NEW, 0, CCH_VIDEO_SEL);//HDMi2.0 video
		

		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase, 0, RG_HDMI2_REG_APLL_CNTL_SEL);

		vIO32WriteFldAlign(HDMI2_DIG_PHY_CONTROL3 + u2HdmiBase, 0, RG_HDMI2_REG_ANA_CNTL_SEL);
	}
	
	return;
}

void vHDMI2ComDuleModeClk(UINT16 u2HdmiBase)
{
	vIO32WriteFldAlign(HDMI2_SEL, 0, RG_HDMI2_REG_CCIR_CLK_SEL);
	if(IS_IC_5861())//For 5861, there is no R2R, so disable it defaultly. For 5890, with R2R, the control will be done in drv_dvi2.c(depend on timing)
	{
		vIO32WriteFldAlign(HDMI2_SEL, 0, RG_HDMI2_REG_MAIN_SUB_CLK);
	}
#ifndef CC_HDMI_OLD_NEW_SUPPORT
    vIO32WriteFldAlign(HDMI2_SEL, 0, RG_HDMI2_REG_CCIR_CLK_SEL2);
#endif
    if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //Oryx E3 or later
    {
        vIO32WriteFldAlign(HDMI2_PATH_SEL,1,FLD_HDMI2_PATH_AUD_CLK);
    }
}

void vHDMI2ComSingleModeClk(UINT16 u2Hdmi2Base)
{
	vIO32WriteFldAlign(HDMI2_SEL, 0xf, RG_HDMI2_REG_CCIR_CLK_SEL);
	vIO32WriteFldAlign(HDMI2_SEL, 1, RG_HDMI2_REG_MAIN_SUB_CLK);    
#ifndef CC_HDMI_OLD_NEW_SUPPORT
    vIO32WriteFldAlign(HDMI2_SEL, 1, RG_HDMI2_REG_CCIR_CLK_SEL2);
#endif
    if((IS_IC_5890()) && (BSP_GetIcVersion() >= IC_VER_5890_AC)) //Oryx E3 or later
    {
        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_AUD_CLK);
        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_CCIR0);
        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_CCIR1);
        vIO32WriteFldAlign(HDMI2_PATH_SEL,0,FLD_HDMI2_PATH_W2D);
    }
}
