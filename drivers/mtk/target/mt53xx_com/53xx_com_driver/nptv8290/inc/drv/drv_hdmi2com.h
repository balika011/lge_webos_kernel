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
 * $Date  $
 * $RCSfile: drv_hdmi.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/
#ifndef _DRV_HDMICOM_H_
#define _DRV_HDMICOM_H_

#include "typedef.h"
#include "x_timer.h"

typedef enum
{
    HDMI2ANA_DELAY_SAOSC,
    HDMI2ANA_DELAY_EQLOCK_HALF,
    HDMI2ANA_DELAY_EQLOCK_FULL,
    HDMI2ANA_DELAY_EQINCREASE,
    HDMI2ANA_DELAY_MAX
}HDMI2ANA_DELAY_ITEM;

typedef struct _HDMI2ANA_LEQ_ITEM
{
    BOOL fgLeak;
    BOOL fgAvg;
    BOOL fgLeak_Increase;
    BOOL fgLeak_Decrease;
    BOOL fgLEQ_Reduce;
}HDMI2ANA_LEQ_ITEM;

typedef struct _HDMI2ANA_CONFIG
{
    UINT16 au2DelayRatio[HDMI2ANA_DELAY_MAX]; //ratio*10%
    HDMI2ANA_LEQ_ITEM tLEQItem;
    UINT8 u1SAOSC_LFSEL;
    UINT8 u1EQOSC_LFSEL;
    BOOL fgSupport3GHDMI20;
    BOOL fgForce40xMode;
}HDMI2ANA_CONFIG;

extern void vHDMI2ComAnaEQ_OS_LF_SELSet(UINT16 u2HdmiAnaBase,UINT8 u1LfSel);

extern UINT8 bHDMI2ComAnaEQ_OS_LF_SELGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaLFSelValue_Set(UINT8 u1SAOSC_LFSelValue,UINT8 u1EQOSC_LFSelValue);

extern void vHDMI2ComAnaHYBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1HYBias);

extern UINT8 bHDMI2ComAnaHYBiasGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaD2SBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1D2SBias);

extern UINT8 bHDMI2ComAnaD2SBiasGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQBiasSet(UINT16 u2HdmiAnaBase,UINT8 u1EQBias);

extern UINT8 bHDMI2ComAnaEQBiasGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaKPGainSet(UINT16 u2HdmiAnaBase,UINT8 u1KPGain);

extern UINT8 bHDMI2ComAnaKPGainGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaRXModeSet(UINT16 u2HdmiAnaBase,UINT8 u1RXMode);

extern UINT8 bHDMI2ComAnaRXModeGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQGainSet(UINT16 u2HdmiAnaBase,UINT8 u1EQGain);

extern UINT8 bHDMI2ComAnaEQGainGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaGainBufSet(UINT16 u2HdmiAnaBase,UINT8 u1GainBuf);

extern UINT8 bHDMI2ComAnaGainBufGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaData_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaEQALG_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaCDR_RST(UINT16 u2HdmiAnaBase,UINT8 u1Rst); //0:reset release 1:reset

extern void vHDMI2ComAnaSAOSC_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaEQOSC_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaEQ_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaEQIncrease_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaAdjust_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb); //0:reset 1:reset release

extern void vHDMI2ComAnaCKDIV2_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb);

extern void vHDMI2ComAnaCKGEN_RSTB(UINT16 u2HdmiAnaBase,UINT8 u1Rstb);

extern void vHDMI2ComAnaPLL_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable);

extern void vHDMI2ComAnaDeepPLL_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable);

extern void vHDMI2ComAnaDebug1Ctrl_EN(UINT16 u2HdmiAnaBase,UINT8 u1Enable);

extern void vHDMI2ComAnaDelaySet(HDMI2ANA_DELAY_ITEM eDelayItem,UINT16 u2Delay);

extern UINT16 wHDMI2ComAnaDelayGet(HDMI2ANA_DELAY_ITEM eDelayItem);

extern void vHDMI2ComAnaSupport3GHDMI20(BOOL fgEnable);

extern void vHDMI2ComAnaForce40xMode(BOOL fgEnable);

extern void vHDMI2ComAnaConTestSet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaConControl(UINT16 u2HdmiAnaBase, UINT8 u1ConValue, UINT8 u1ConType);

extern void vHDMI2ComAnaLeqItemSet(HDMI2ANA_LEQ_ITEM *ptLeqItem);

extern void vHDMI2ComAnaLeqItemGet(HDMI2ANA_LEQ_ITEM *ptLeqItem);

extern void vHDMI2ComAnaDelayInit(void);

extern void vHDMI2ComAnaLeqItemInit(void);

extern void vHDMI2ComAnaDebugResetPar(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQMacroInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaSAOSCInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQOSCInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQDIGInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQIncreaseInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaAdjustInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaDebug1Init(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaDebug2Init(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaPLLInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaDeepPLLInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaInit(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaSetConMon(UINT16 u2HdmiAnaBase,UINT8 u1MonSel);

extern UINT8 bHDMI2ComAnaGetCDRLPO(UINT16 u2HdmiAnaBase,HdmiTmdsDataChannel_t eChannel);

extern UINT8 bHDMI2ComAnaGetAlgMonStatus(UINT16 u2HdmiAnaBase,UINT8 u1MonSel, HdmiTmdsDataChannel_t eChannel);

extern UINT8 bHDMI2ComAnaGetEQCtrl(UINT16 u2HdmiAnaBase,HdmiTmdsDataChannel_t eChannel);

extern UINT16 wHDMI2ComAnaGetDebug1Status(UINT16 u2HdmiAnaBase,UINT8 u1EvenSel, UINT8 u1MonSel, HdmiTmdsDataChannel_t eChannel);

extern BOOL bHDMI2ComAnaGetDebug1Ready(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand);
    
extern void vHDMI2ComAnaHalfRateSet(UINT16 u2HdmiAnaBase,BOOL b40xMode);
    
extern BOOL bHDMI2ComAnaHalfRateGet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQBandSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode);

extern void vHDMI2ComAnaPLLSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode);

extern void vHDMI2ComAnaDeepPLLSet(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode);
    
extern BOOL bHDMI2ComAnaAdjustRatioSet(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaDebug1CntTargetSet(UINT16 u2HdmiAnaBase,UINT32 u4CntTarget);

extern void vHDMI2ComAnaDebug1PatSet(UINT16 u2HdmiAnaBase,UINT16 u2PatMask, UINT16 u2TargetPat);

extern void vHDMI2ComAnaDebug1ParSet(UINT16 u2HdmiAnaBase,INT8 s1Par1,INT8 s1Par2);

extern void vHDMI2ComAnaPLLEnable(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaEQMacroEnable(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaDataReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaEQALGReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaSAOSC(UINT16 u2HdmiAnaBase,BOOL fgResetSAOSC);

extern void vHDMI2ComAnaSAOSCReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaEQOSC(UINT16 u2HdmiAnaBase,BOOL fgResetEQOSC);

extern void vHDMI2ComAnaEQOSCReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaCDRLock(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaCDRReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaEQLock(UINT16 u2HdmiAnaBase,BOOL b40xMode, HDMI2ANA_LEQ_ITEM *ptLeqItem, BOOL fgResetEQ);

extern void vHDMI2ComAnaFixEQ(UINT16 u2HdmiAnaBase,BOOL bFix, UINT8 u1EQInitVal);

extern void vHDMI2ComAnaEQReset(UINT16 u2HdmiAnaBase,BOOL b40xMode, HDMI2ANA_LEQ_ITEM *ptLeqItem);

extern void vHDMI2ComAnaEQIncreaseEnable(UINT16 u2HdmiAnaBase,BOOL fgResetEQIncrease);

extern void vHDMI2ComAnaEQIncreaseReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaAdjustEnable(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaAdjustReset(UINT16 u2HdmiAnaBase); //reserve for reset

extern void vHDMI2ComAnaDeepPLLEnable(UINT16 u2HdmiAnaBase);

extern void vHDMI2ComAnaTermCtrl(UINT16 u2HdmiAnaBase,BOOL bOnOff);

extern void vHDMI2ComAnaTMDSCtrl(UINT16 u2HdmiAnaBase,BOOL bOnOff);

extern void vHDMI2ComAnaSetting(UINT16 u2HdmiAnaBase,HdmiTmdsClkRate_t eBand, BOOL b40xMode);

extern void vHDMI2ComAnaDebug1Enable(UINT16 u2HdmiAnaBase,BOOL bEnable);

extern void vHDMI2ComAnaDebug2Enable(UINT16 u2HdmiAnaBase);

extern UINT32 dwHDMI2ComHTotal(UINT16 u2HdmiBase);

extern UINT32 dwHDMI2ComVTotal(UINT16 u2HdmiBase);

extern UINT32 dwHDMI2ComHActive(UINT16 u2HdmiBase);

extern UINT32 dwHDMI2ComVActive(UINT16 u2HdmiBase);

extern UINT32 dwHDMI2ComFrameRate(UINT16 u2HdmiBase);

extern UINT32 dwHDMI2ComPixelClk(UINT16 u2HdmiBase);
    
//1:internal pattern gen
extern void vHDMI2ComPatternGenSet(UINT16 u2HdmiBase,UINT8 u1En);
extern BOOL fgHDMI2ComAnaCRC(UINT16 u2HdmiBase,UINT16 u2Times);
extern BOOL fgHDMI2ComCRC(UINT16 u2HdmiBase,UINT16 u2Times);

/**
 * @brief 	Get HDMI AVMute status
 * @param	Base address by port
 * @retval	1:interlace,0:program
 */
extern BOOL fgHDMI2ComGetInterlaced(UINT16 u2HdmiBase);

/**
 * @brief 	Get HDMI AVMute status
 * @param	Base address by port
 * @retval	1:mute,0:clear mute
 */
extern BOOL fgHDMI2ComGetMute(UINT16 u2HdmiBase);


/**
 * @brief 	Get HDMI mode status
 * @param	Base address by port
 * @retval	1:HDMI mode,0:DVI mode
 */
extern BOOL fgHDMI2ComGetHdmiMode(UINT16 u2HdmiBase);

/**
 * @brief 	Get HDMI audio mute status
 * @param	Base address by port
 * @retval	1:mute,0:active
 */
extern BOOL fgHDMI2ComGetAudioMute(UINT16 u2HdmiBase);

/**
 * @brief 	Get HDMI Tx scrambling en/dis status
 * @param	Base address by port
 * @retval	1:need descrambling input data,
 			0:not need descrambling input data
 */
extern BOOL fgHDMI2ComGetScramblingBySCDC(UINT16 u2HdmiBase);

/**
 * @brief 	Get HW en/dis scrambling status,default scrambling en/dis is controled by HW
 * @param	Base address by port
 * @retval	1:enable,
 			0:disable
 */
extern BOOL fgHDMI2ComGetHWScramblingEn(UINT16 u2HdmiBase);


/**
 * @brief 	Get TMDS clk ratio by scdc
 * @param	Base address by port
 * @retval	1:1/40,0:1/30
 */
extern BOOL fgHDMI2ComGetTmdsClkRatio(UINT16 u2HdmiBase);

/**
 * @brief 	Get TMDS clk status
 * @param	Base address by port
 * @retval	1:clk active,0:no clk
 **/
extern BOOL fgHDMI2ComGetCKDT(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern BOOL fgHDMI2ComGetSCDT(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern UINT32 dwHDMI2ComGetCTS(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern UINT32 dwHDMI2ComGetN(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern UINT8 bHDMI2ComGetFs(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern void vHDMI2ComGetChannelStatus(UINT32 HdmiInst);

/**
 * @brief 	Get audio layout status
 * @param	Base address by port
 * @retval	1:up to 8 channel,0:2 channel
 */
extern BOOL fgHDMI2ComGetAudioLayout(UINT16 u2HdmiBase);

/**
 * @brief 	Get audio layout status
 * @param	Base address by port
 * @retval	1:up to 8 channel,0:2 channel
 */
extern BOOL fgHDMI2ComGetAudFifo(UINT16 u2HdmiBase);


/**
 * @brief 	Get deep color  status
 * @param	Base address by port
 * @retval	3:16 bit
 			2:12 bit
 			1:10 bit
 			0:8 bit
 */
extern HdmiRxBitDepth_t bHDMI2ComGetDeepSta(UINT16 u2HdmiBase);

/**
 * @brief 	Get SCDT status
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern HdmiTmdsClkRate_t bHDMI2ComGetClkRate(UINT16 u2HdmiBase);

/**
 * @brief 	Get infoframe data
 * @param	info type
 * @retval	1
 **/
extern BOOL fgHDMI2ComGetInfoframe(UINT32 HdmiInst,UINT8 Type);

/**
 * @brief set DDC enable or disable
 * @param	Base address by port,enable/disable
 * @retval	none
 **/
extern void vHDMI2ComSetHdcpDDC(UINT16 u2HdmiBase,UINT8 u1En);


/**
 * @brief 	set split mode enable or disable
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
extern void vHDMI2ComSetSplit(UINT16 u2HdmiBase,UINT8 u1En);

/**
 * @brief 	set Scrambling mode enable or disable by sw,used for some 1.4 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
extern void vHDMI2ComSetScrambling(UINT16 u2HdmiBase,UINT8 u1En);

/**
 * @brief 	set 420 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
extern void vHDMI2ComSet420(UINT16 u2HdmiBase,UINT8 u1En);

/**
 * @brief 	set 420 to 422 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
extern void vHDMI2ComSet420To422(UINT16 u2HdmiBase,UINT8 u1En);

/**
 * @brief 	set 422 to 444 enable or disable,used for 4k 420 timing
 * @param	Base address by port, enable or disable
 * @retval	none
 **/
extern void vHDMI2ComSet422To444(UINT16 u2HdmiBase,UINT8 u1En);
//void vHDMI2ComDeepRst(UINT16 u2HdmiBase);
extern void vHDMI2ComFifoRst(UINT16 u2HdmiBase);
extern void vHDMI2ComAcrRst(UINT16 u2HdmiBase);
extern void vHDMI2ComDeepRst1(UINT16 u2HdmiBase);
extern void vHDMI2ComDeepRst(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComDigPhyRst1(UINT16 u2HdmiBase);
extern void vHDMI2ComDigPhyRst(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComSplitRst(UINT16 u2HdmiBase);
extern void vHDMI2ComHdcp1Rst(UINT16 u2HdmiBase);
extern void vHDMI2ComHdcp2Rst(UINT16 u2HdmiBase);
extern void vHDMI2ComHDCP2Init(UINT16 u2HdmiBase);
extern void vHDMI2ComDdcSel(UINT16 u2HdmiBase,UINT8 u1Type);
extern void vHDMI2ComSigSel(UINT16 u2HdmiBase,UINT8 u1Type);
extern void vHDMI2ComRxInit(UINT16 u2HdmiBase);
extern void vHDMI2ComSwRst(UINT16 u2HdmiBase);

extern void vHDMI2ComSet420To444(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComGetVideoStatus(UINT16 u2HdmiBase);
extern void vHDMI2ComGetAudioStatus(UINT16 u2HdmiBase);
extern UINT8 bHDMI2ComGetHdcp1Status(UINT16 u2HdmiBase);
extern UINT8 bHDMI2ComGetHdcp2Status(UINT16 u2HdmiBase);
extern void vHDMI2ComGetVideoInfoByAVI(UINT16 u2HdmiBase);
extern BOOL fgHDMI2ComGet420Mode(UINT16 u2HdmiBase);
extern void vHDMI2ComSetClk(UINT16 u2HdmiBase);
extern void vHDMI2ComNewDigSel(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComDuleModeClk(UINT16 u2HdmiBase);
extern void vHDMI2ComSingleModeClk(UINT16 u2Hdmi2Base);
extern void vHDMI2ComAudPllSel(UINT16 u2HdmiBase,UINT8 u1Pll);
extern void vHDMI2ComAudMuteSet(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComVideMuteSet(UINT16 u2HdmiBase,UINT8 u1En);
extern void vHDMI2ComAudCfg(UINT16 u2HdmiBase);
extern void vHDMI2ComSetScdc(UINT8 u1En);
extern void vHDMI2ComClearScdcStatus(void);
extern UINT32 dwHDMI2ComHClk(UINT16 u2HdmiBase);
extern void vHDMI2ComAacEn(UINT16 u2HdmiBase,UINT8 u1En);
extern UINT8 bHDMI2ComGetAudFifo(UINT16 u2HdmiBase);
extern void vHDMI2ComAudioSigmaDeltaSetting(UINT16 u2HdmiBase,UINT32 value1, UINT8 value2, UINT8 bSDINOVR);
/**
 * @brief 	Get TMDS clk frequency, cal from pixel frequency, only avaliable at stable signal
 * @param	Base address by port
 * @retval	1:sync active,0:no scdt
 */
extern UINT32 bHDMI2ComGetTMDSClkFreq(UINT16 u2HdmiBase);
extern UINT8 bHDMI2AudSampleRateCal(UINT16 u2HdmiBase);
extern void vHDMI2ComAudStatusSel(UINT16 u2HdmiBase,UINT8 u1On);
#ifdef CC_HDMI_OLD_NEW_SUPPORT
UINT8 bHDMI2ComGetClkRateForDig(UINT16 u2HdmiBase);
#endif

#endif

