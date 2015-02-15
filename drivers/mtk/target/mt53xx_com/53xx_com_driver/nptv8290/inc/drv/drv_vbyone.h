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
 * $RCSfile: drv_vbyone.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#include "hw_vbyone.h" 
// Data Path
// Control
#define vDrvVByOneCrtlBitTRIG(bfg) vIO32WriteFldAlign(VB1ANA0,bfg,VB1ANA0_TRIG_CTL_BIT)
#define vDrvVByOneCrtlBitEnable(bEn) vIO32WriteFldAlign(VB1ANA0,bEn,VB1ANA0_SN_CTL_BIT)
#define vDrvVByOneCrtlBitDisTRIG(bDisEn) vIO32WriteFldAlign(VB1ANA0,bDisEn,VB1ANA0_DISABLE_TRIG)
#define vDrvVByOneCrtlBitVEDGE(bEdge) vIO32WriteFldAlign(VB1ANA0,bEdge,VB1ANA0_VEDGE_SEL)
// Encode
#define vDrvVByOneCrtlBitVP(bVP) vIO32WriteFldAlign(VB1ANA0,bVP,VB1ANA0_CTL_BIT_LANE0_VP)
#define vDrvVByOneCrtlBitRA(bRA) vIO32WriteFldAlign(VB1ANA0,bRA,VB1ANA0_CTL_BIT_LANE0_RA)
#define vDrvVByOneCrtlBit3DST(u13DST) vIO32WriteFldAlign(VB1ANA0,u13DST,VB1ANA0_CTL_BIT_LANE0_3DST)
#define vDrvVByOneCrtlBitPICHSize(u2PIC_HSize) vIO32WriteFldAlign(VB1ANA0,u2PIC_HSize,VB1ANA0_CTL_BIT_LANE0_PIC_HSIZE)

#define vDrvVByOneCrtlBitCBV(bCBV) vIO32WriteFldAlign(VB1ANA2,bCBV,VB1ANA2_CTL_BIT_LANE1_CBV)
#define vDrvVByOneCrtlBitCF(u1CF) vIO32WriteFldAlign(VB1ANA2,u1CF,VB1ANA2_CTL_BIT_LANE1_CF)
#define vDrvVByOneCrtlBitRF(bRF) vIO32WriteFldAlign(VB1ANA2,bRF,VB1ANA2_CTL_BIT_LANE1_RF)
#define vDrvVByOneCrtlBitPICVSize(u2PIC_VSize) vIO32WriteFldAlign(VB1ANA2,u2PIC_VSize,VB1ANA2_CTL_BIT_LANE1_PIC_VSIZE)

#define vDrvVByOneCrtlBitAT(bAT) vIO32WriteFldAlign(VB1ANA3,bAT,VB1ANA3_CTL_BIT_LANE2_AT)
#define vDrvVByOneCrtlBitCROPHPos(u2CROP_HPos) vIO32WriteFldAlign(VB1ANA3,u2CROP_HPos,VB1ANA3_CTL_BIT_LANE2_CROP_HPOS)

#define vDrvVByOneCrtlBitARLsb(u1ARLSB) vIO32WriteFldAlign(VB1ANA4,u1ARLSB,VB1ANA4_CTL_BIT_LANE3_AR_LSB)
#define vDrvVByOneCrtlBitCROPVPos(u2CROP_VPos) vIO32WriteFldAlign(VB1ANA4,u2CROP_VPos,VB1ANA4_CTL_BIT_LANE3_CROP_VPOS)

#define vDrvVByOneCrtlBitARMsb(bARMSB) vIO32WriteFldAlign(VB1ANA5,bARMSB,VB1ANA5_CTL_BIT_LANE4_AR_MSB)
#define vDrvVByOneCrtlBitCROPHSize(u2CROP_HSize) vIO32WriteFldAlign(VB1ANA5,u2CROP_HSize,VB1ANA5_CTL_BIT_LANE4_CROP_HSIZE)

#define vDrvVByOneCrtlBitCTLsb(u1CTLSB) vIO32WriteFldAlign(VB1ANA6,u1CTLSB,VB1ANA6_CTL_BIT_LANE5_CT_LSB)
#define vDrvVByOneCrtlBitCROPVSize(u2CROP_VSize) vIO32WriteFldAlign(VB1ANA6,u2CROP_VSize,VB1ANA6_CTL_BIT_LANE5_CROP_VSIZE)

#define vDrvVByOneCrtlBitCTMsb(bCTMSB) vIO32WriteFldAlign(VB1ANA7,bCTMSB,VB1ANA7_CTL_BIT_LANE6_CT_MSB)
#define vDrvVByOneCrtlBitSARHSize(u2SAR_HSize) vIO32WriteFldAlign(VB1ANA7,u2SAR_HSize,VB1ANA7_CTL_BIT_LANE6_SAR_HSIZE)

#define vDrvVByOneCrtlBitSARVSize(u2SAR_VSize) vIO32WriteFldAlign(VB1ANA8,u2SAR_VSize,VB1ANA8_CTL_BIT_LANE7_SAR_VSIZE)

#define VB1_8LANE_00112233SWAP 1  // need to remove after verification

void vDrvVByOneOn(void);
void vDrvVByOneOff(void);
void vVByOneInit(void);
void vVByOneLane4_8bit_script(void);
void vVByOneLane4_10bit_script(void);
void vVByOneLane4_12bit_script(void);
void vVByOne_Impedance_Cal(void);
void vVByOneAnalogOffSetting(void);
void vGetVByOneStatus(void);
void vDrvVByOneCRCCheck(void);
void vVByOneTestMode(void);
void vVByOneFormatConfig(void);
void vGetVByOneUnLock(void);
void vGetVByOneUnLockCnt(void);
void vSetVByOneUnLockChk(UINT8 u1En);
void vDrvVB14K2KPanelInterfaceConfig(void);
void vDrvVB1LockCtrl(UINT8 bOnOff, UINT8 bValue);
void vVByOneLaneSwapForTest(UINT8 u1Mode);
void vVByOneSingleLanePadEn(UINT8 u1LaneEn);
void vVByOneVPLLBandwidthSet(UINT8 u1Bandwidth);
void vVByOnePreEmphasisOnOff(UINT8 u1OnOff);
void vVByOnePreEmphasisSet(UINT8 value);
UINT8 vVByOneGetPreEmphasis(void);

// OSD Path
// Control
#define vDrvVByOneGMCrtlBitTRIG(bfg) vIO32WriteFldAlign(VB1OSDANA0,bfg,VB1OSDANA0_OSD_TRIG_CTL_BIT)
#define vDrvVByOneGMCrtlBitEnable(bEn) vIO32WriteFldAlign(VB1OSDANA0,bEn,VB1OSDANA0_OSD_SN_CTL_BIT)
#define vDrvVByOneGMCrtlBitDisTRIG(bDisEn) vIO32WriteFldAlign(VB1OSDANA0,bDisEn,VB1OSDANA0_OSD_DISABLE_TRIG)
#define vDrvVByOneGMCrtlBitVEDGE(bEdge) vIO32WriteFldAlign(VB1OSDANA0,bEdge,VB1OSDANA0_OSD_VEDGE_SEL)
// Encode
#define vDrvVByOneGMCrtlBitVF(bVF) vIO32WriteFldAlign(VB1OSDANA0,bVF,VB1OSDANA0_CTL_BIT_OSD_LANE0_VF)
#define vDrvVByOneGMCrtlBitGF(bGF) vIO32WriteFldAlign(VB1OSDANA0,bGF,VB1OSDANA0_CTL_BIT_OSD_LANE0_GF)
#define vDrvVByOneGMCrtlBitHTotal(u2HTotal) vIO32WriteFldAlign(VB1OSDANA0,u2HTotal,VB1OSDANA0_CTL_BIT_OSD_LANE0_HTOTAL)
#define vDrvVByOneGMCrtlBitVTotal(u2VTotal) vIO32WriteFldAlign(VB1OSDANA2,u2VTotal,VB1OSDANA2_CTL_BIT_OSD_LANE1_VTOTAL)
#define vDrvVByOneGMCrtlBitHDEStart(u2HDEStart) vIO32WriteFldAlign(VB1OSDANA3,u2HDEStart,VB1OSDANA3_CTL_BIT_OSD_LANE2_HDEStart)
#define vDrvVByOneGMCrtlBitVDEStart(u2VDEStart) vIO32WriteFldAlign(VB1OSDANA4,u2VDEStart,VB1OSDANA4_CTL_BIT_OSD_LANE3_VDEStart)

void vDrvVByOneOSDOn(void);
void vDrvVByOneOSDOff(void);
void vDrvVByOneOSDCRCCheck(void);
void vVByOneOSDFormatConfig(void);

void VByOne_Register_Mapping_Write_E3(void);



