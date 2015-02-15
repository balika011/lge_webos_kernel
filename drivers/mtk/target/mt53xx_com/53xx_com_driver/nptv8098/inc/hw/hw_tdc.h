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
 * $RCSfile: hw_tdc.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#ifndef _HW_TDC_H_
#define _HW_TDC_H_

#include "hw_vdoin.h"


#define IO_COMB_BASE (VDOIN_BASE+0x2000)

//Page COMB
#define STA_COMB_00 (IO_COMB_BASE + 0x0C0)
    #define BIN12PIXCNTSTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_01 (IO_COMB_BASE + 0x0C4)
    #define BIN34PIXCNTSTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_02 (IO_COMB_BASE + 0x0C8)
    #define CZPCNTSTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_03 (IO_COMB_BASE + 0x0CC)
    #define NOISTIPISUM Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_04 (IO_COMB_BASE + 0x0D0)
    #define INPIXCNTSTA Fld(20,0,AC_MSKDW)//[19:0]
#define STA_COMB_05 (IO_COMB_BASE + 0x0D4)
    #define MOPIXCNTSTA Fld(20,0,AC_MSKDW)//[19:0]
#define STA_COMB_06 (IO_COMB_BASE + 0x0D8)
    #define MBPIXCNTSTA Fld(20,0,AC_MSKDW)//[19:0]
#define STA_COMB_07 (IO_COMB_BASE + 0x0DC)
    #define LUMASUMSTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_08 (IO_COMB_BASE + 0x0E0)
    #define COLORSUMSTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_09 (IO_COMB_BASE + 0x0E4)
    #define LUMAEDGESTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_0A (IO_COMB_BASE + 0x0E8)
    #define LUMAEDPISTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_0B (IO_COMB_BASE + 0x0EC)
    #define COLOREDGESTA Fld(32,0,AC_FULLDW)//[31:0]
#define STA_COMB_0C (IO_COMB_BASE + 0x0F0)
    #define FIFO_EVERFULL Fld(1,22,AC_MSKB2)//[22:22]
    #define FIFOEMPTY Fld(1,21,AC_MSKB2)//[21:21]
    #define FIFOEMPTY_A Fld(1,20,AC_MSKB2)//[20:20]
    #define FIFOEMPTY_AA Fld(1,19,AC_MSKB2)//[19:19]
    #define BIG_MOTION Fld(1,18,AC_MSKB2)//[18:18]
    #define NONVCR3D Fld(1,17,AC_MSKB2)//[17:17]
    #define NONVCRTG Fld(1,16,AC_MSKB2)//[16:16]
    #define SMALL_MOTION Fld(1,15,AC_MSKB1)//[15:15]
    #define STILL_FRAME Fld(1,14,AC_MSKB1)//[14:14]
    #define WHOLE_FRAME_C3D_SMALL Fld(1,13,AC_MSKB1)//[13:13]
    #define NOISEFG Fld(1,4,AC_MSKB0)//[4:4]
    #define CZP_DETECT Fld(1,3,AC_MSKB0)//[3:3]
    #define SWEEP_DETECTION Fld(1,2,AC_MSKB0)//[2:2]
    #define WHOLE_FRAME_VERTICAL_SMOOTH Fld(1,1,AC_MSKB0)//[1:1]
    #define NOISY_SMALL Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_00 (IO_COMB_BASE + 0x640)
    #define REG_HFY_VYD_GAIN Fld(4,28,AC_MSKB3)//[31:28]
    #define REG_HFY_VCD_GAIN Fld(4,24,AC_MSKB3)//[27:24]
    #define REG_HFY_HYD_GAIN Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_HFY_HCD_GAIN Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_HFY_MBVTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_HFY_HDGAIN Fld(4,4,AC_MSKB0)//[7:4]
    #define ENGOH Fld(1,3,AC_MSKB0)//[3:3]
    #define ENVPERS Fld(1,2,AC_MSKB0)//[2:2]
    #define COMB_ATPG_CT Fld(1,1,AC_MSKB0)//[1:1]
    #define COMB_ATPG_OB Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_01 (IO_COMB_BASE + 0x644)
    #define REG_MHFYK Fld(4,28,AC_MSKB3)//[31:28]
    #define REG_ENMHFY Fld(1,27,AC_MSKB3)//[27:27]
    #define ENSVNO Fld(1,24,AC_MSKB3)//[24:24]
    #define SP_VAR_MAX_RANGE1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SP_VAR_MAX_RANGE2 Fld(8,8,AC_FULLB1)//[15:8]
    #define VEXP_C Fld(1,2,AC_MSKB0)//[2:2]
    #define VEXP Fld(1,1,AC_MSKB0)//[1:1]
#define COMB2D_02 (IO_COMB_BASE + 0x648)
    #define DYLOMIN_THR Fld(8,24,AC_FULLB3)//[31:24]
    #define SVNOCGTH Fld(8,8,AC_FULLB1)//[15:8]
    #define ENFVY Fld(1,1,AC_MSKB0)//[1:1]
    #define ENFSVNO Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_03 (IO_COMB_BASE + 0x64C)
    #define K1CHROMAEGTH Fld(8,24,AC_FULLB3)//[31:24]
    #define EN_Y5X3OUT Fld(1,16,AC_MSKB2)//[16:16]
    #define K1LUMAVEGTH Fld(8,8,AC_FULLB1)//[15:8]
    #define HFYTBSEL Fld(1,7,AC_MSKB0)//[7:7]
    #define YTH_HFY Fld(7,0,AC_MSKB0)//[6:0]
#define COMB2D_04 (IO_COMB_BASE + 0x650)
    #define ENMK4 Fld(1,31,AC_MSKB3)//[31:31]
    #define ENMK3 Fld(1,30,AC_MSKB3)//[30:30]
    #define ENMK2 Fld(1,29,AC_MSKB3)//[29:29]
    #define ENMK1 Fld(1,28,AC_MSKB3)//[28:28]
    #define ENFWEAKC Fld(1,26,AC_MSKB3)//[26:26]
    #define FORCEWEAKC Fld(1,25,AC_MSKB3)//[25:25]
    #define SELSTD5X5C Fld(1,24,AC_MSKB3)//[24:24]
    #define HFSEL Fld(2,22,AC_MSKB2)//[23:22]
    #define HFTPSEL Fld(2,20,AC_MSKB2)//[21:20]
    #define MK4 Fld(4,12,AC_MSKB1)//[15:12]
    #define MK3 Fld(4,8,AC_MSKB1)//[11:8]
    #define MK2 Fld(4,4,AC_MSKB0)//[7:4]
    #define MK1 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_05 (IO_COMB_BASE + 0x654)
    #define PVNCSEL Fld(2,30,AC_MSKB3)//[31:30]
    #define PVSEL Fld(2,28,AC_MSKB3)//[29:28]
    #define HFYOSEL Fld(2,26,AC_MSKB3)//[27:26]
    #define REG_PURE1D Fld(1,3,AC_MSKB0)//[3:3]
    #define CUSE5X3 Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_06 (IO_COMB_BASE + 0x658)
    #define COMB_DE_GEN_VERYLP Fld(8,16,AC_FULLB2)//[23:16]
    #define MATRIX_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define ENMBNEW_C Fld(1,7,AC_MSKB0)//[7:7]
    #define ENMBNEW_Y Fld(1,6,AC_MSKB0)//[6:6]
    #define CTPSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define ENAD024B23 Fld(1,3,AC_MSKB0)//[3:3]
    #define ENM024B23 Fld(1,2,AC_MSKB0)//[2:2]
    #define ENAD024B1 Fld(1,1,AC_MSKB0)//[1:1]
    #define ENM024B1 Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_07 (IO_COMB_BASE + 0x65C)
    #define ENSHARP Fld(1,31,AC_MSKB3)//[31:31]
    #define SHARP_TBTH Fld(7,24,AC_MSKB3)//[30:24]
    #define C5X3EGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define CAMPGTH Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_08 (IO_COMB_BASE + 0x660)
    #define SHARP_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define SHARP_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define SHARP_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define SHARP_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define SHARP_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define SHARP_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define SHARP_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define SHARP_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_09 (IO_COMB_BASE + 0x664)
    #define K3_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define K3_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define K3_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define K3_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define K3_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define K3_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define K3_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define K3_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_0A (IO_COMB_BASE + 0x668)
    #define MBBOUNDARY_Y Fld(1,31,AC_MSKB3)//[31:31]
    #define MBBOUNDARY_C Fld(1,30,AC_MSKB3)//[30:30]
    #define ENPUREHORMB Fld(1,29,AC_MSKB3)//[29:29]
    #define HORMBTH2D Fld(5,24,AC_MSKB3)//[28:24]
    #define PUREHORSEL Fld(1,23,AC_MSKB2)//[23:23]
    #define MBBOUNDSEL Fld(1,22,AC_MSKB2)//[22:22]
    #define VERMBTH2D Fld(5,16,AC_MSKB2)//[20:16]
    #define PADDGTH Fld(8,8,AC_FULLB1)//[15:8]
    #define ENPUREHORXX Fld(1,7,AC_MSKB0)//[7:7]
    #define PUREVERPAL Fld(1,4,AC_MSKB0)//[4:4]
    #define ENUNIFORM Fld(1,3,AC_MSKB0)//[3:3]
    #define ENSGDIRIN Fld(1,2,AC_MSKB0)//[2:2]
    #define INVSGDIRIN Fld(1,1,AC_MSKB0)//[1:1]
    #define ENINVDIADIR Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_0B (IO_COMB_BASE + 0x66C)
    #define ENPALVCD Fld(1,31,AC_MSKB3)//[31:31]
    #define TBTH_K3 Fld(7,0,AC_MSKB0)//[6:0]
#define COMB2D_0C (IO_COMB_BASE + 0x670)
    #define FIRVERDIFTH Fld(8,24,AC_FULLB3)//[31:24]
    #define DISCFIRINCBG Fld(1,23,AC_MSKB2)//[23:23]
    #define FIRINSEL Fld(1,22,AC_MSKB2)//[22:22]
    #define DISCVBS01234SM Fld(1,21,AC_MSKB2)//[21:21]
    #define DELAYSEL Fld(2,19,AC_MSKB2)//[20:19]
    #define ENFIROK Fld(1,18,AC_MSKB2)//[18:18]
    #define FORCEFIR Fld(1,17,AC_MSKB2)//[17:17]
    #define SKINPH Fld(5,12,AC_MSKW21)//[16:12]
    #define SKINSEL Fld(2,10,AC_MSKB1)//[11:10]
    #define UVSMSEL2D Fld(2,8,AC_MSKB1)//[9:8]
    #define UVSMALLTH2 Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_0D (IO_COMB_BASE + 0x674)
    #define ENDIRIN Fld(1,31,AC_MSKB3)//[31:31]
    #define PALSW Fld(2,26,AC_MSKB3)//[27:26]
    #define MBLSEL Fld(1,25,AC_MSKB3)//[25:25]
    #define MB51 Fld(1,24,AC_MSKB3)//[24:24]
    #define GOHTH Fld(8,16,AC_FULLB2)//[23:16]
    #define MBTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_LSMOOTH Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_0E (IO_COMB_BASE + 0x678)
    #define K1SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define K1TBTH Fld(7,24,AC_MSKB3)//[30:24]
    #define PVCVBSVERTH Fld(8,16,AC_FULLB2)//[23:16]
    #define VIPTH Fld(8,8,AC_FULLB1)//[15:8]
    #define HDVDTH Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_0F (IO_COMB_BASE + 0x67C)
    #define K1_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define K1_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define K1_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define K1_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define K1_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define K1_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define K1_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define K1_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_10 (IO_COMB_BASE + 0x680)
    #define REG_CTMBMBTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_CTMBHPFTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_D3CTMBGAIN1 Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D3CTMBGAIN2 Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D3CTMBGAIN3 Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_DISSWEEP_VERLINE Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_DELAYSEL Fld(2,1,AC_MSKB0)//[2:1]
    #define REG_STILLYSEL Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_11 (IO_COMB_BASE + 0x684)
    #define REG_PERIOD_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_DISMAPPINGTB Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_DISCTMBEDGE Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_HSMAXMINSEL Fld(2,20,AC_MSKB2)//[21:20]
    #define REG_DARKSEL Fld(2,18,AC_MSKB2)//[19:18]
    #define REG_DARKSIZESEL Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_CDARKTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_YDARKTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_12 (IO_COMB_BASE + 0x688)
    #define REG_DISANTDET Fld(1,30,AC_MSKB3)//[30:30]
    #define REG_SMALLONSEL Fld(1,27,AC_MSKB3)//[27:27]
    #define REG_BETTERTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_WHSWEEPSUMTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_D2SMALLTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_13 (IO_COMB_BASE + 0x68C)
    #define REG_PALMOTION45TH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_D3GAIN_Y_SM Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3GAIN_C_SM Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_D3GAIN_CV_SM Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D2GAIN_Y_SM Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2GAIN_C_SM Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2GAIN_CV_SM Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_14 (IO_COMB_BASE + 0x690)
    #define PCNTH2_2 Fld(8,24,AC_FULLB3)//[31:24]
    #define PCNTH2_1 Fld(24,0,AC_MSKDW)//[23:0]
#define C_COMB_15 (IO_COMB_BASE + 0x694)
    #define PCNTH1_2 Fld(8,24,AC_FULLB3)//[31:24]
    #define PCNTH1_1 Fld(24,0,AC_MSKDW)//[23:0]
#define C_COMB_16 (IO_COMB_BASE + 0x698)
    #define REG_WHLUMASUMTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_WHIVPHTH Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_ENWHDCDIF Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_ENWHBIN1 Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_ENWHBIN Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_ENWHIVPH Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_ENWHLUMA Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_ENWHMBPIX Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_ENWHCEDGE Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_ENWHYDEPI Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_ENWHEDGE Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_ENWHCOLOR Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_WHYDETH Fld(10,0,AC_MSKW10)//[9:0]
#define C_COMB_17 (IO_COMB_BASE + 0x69C)
    #define REG_EN_FLASH_STATUS Fld(1,29,AC_MSKB3)//[29:29]
    #define REG_METRIC_INK Fld(1,28,AC_MSKB3)//[28:28]
    #define REG_WHBINRANGE Fld(3,25,AC_MSKB3)//[27:25]
    #define REG_WHITEINK Fld(1,24,AC_MSKB3)//[24:24]
    #define REG_ENPALCCS_NEW Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_DISSWEETDET Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_SWININK Fld(1,21,AC_MSKB2)//[21:21]
    #define REG_WININK Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_DRAM_REQ_INK Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_PALSWEEPINK Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_TOTALINK Fld(6,12,AC_MSKW21)//[17:12]
    #define REG_INKON Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_WITHCLRTH_1 Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_INKTH Fld(10,0,AC_MSKW10)//[9:0]
#define C_COMB_18 (IO_COMB_BASE + 0x6A0)
    #define REG_PAL2DVERCVBSSEL Fld(2,26,AC_MSKB3)//[27:26]
    #define REG_SKINDETSEL Fld(1,25,AC_MSKB3)//[25:25]
    #define REG_GOHONLY Fld(1,21,AC_MSKB2)//[21:21]
    #define REG_DISGOHONLY Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_NOISYSTILLSEL Fld(2,18,AC_MSKB2)//[19:18]
    #define REG_DISSTILLFLIGHT Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_WEIGHTLPFSEL Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_EN3DDIFF3FRAME Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_STILLMAXMINSEL Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_ENSTFI Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_ENMRDET Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_ENBSWDETPALHIGH Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_ENBSWDETPAL Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_ENSWDETPAL Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_ENSWDET Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_ENTMB Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_ENHSBIGMO Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_19 (IO_COMB_BASE + 0x6A4)
    #define REG_WHSWEEPEDGETH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_D3GAIN_Y_SW Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3GAIN_C_SW Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_D3GAIN_CV_SW Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D2GAIN_Y_SW Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2GAIN_C_SW Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2GAIN_CV_SW Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_1A (IO_COMB_BASE + 0x6A8)
    #define REG_SENCECHANGETH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_D2HORCVBSSEL Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_D2D3SMALLTH Fld(7,16,AC_MSKB2)//[22:16]
    #define REG_DISKILLYCOMBI Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_D2VERCVBSSEL Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_D3VERHORSEL Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_D2VERHORSEL Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_D3BIGTH Fld(10,0,AC_MSKW10)//[9:0]
#define C_COMB_1B (IO_COMB_BASE + 0x6AC)
    #define REG_NTSCCGAIN Fld(2,30,AC_MSKB3)//[31:30]
    #define REG_CVBSTHFOR3DDIF Fld(6,24,AC_MSKB3)//[29:24]
    #define REG_D3GAINY Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3GAINC Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_D3GAINCV Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D2GAINY Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2GAINC Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2GAINCV Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_1C (IO_COMB_BASE + 0x6B0)
    #define REG_DARKG3 Fld(4,28,AC_MSKB3)//[31:28]
    #define REG_DARKG2 Fld(4,24,AC_MSKB3)//[27:24]
    #define REG_DARKTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_ENYDIF Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_ENGAINSTL Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_ENDARKG Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_ENGAINSML Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_ENSWC3DSM Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_SELSTILLCVBS Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_ENDISCVMO Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_NCSEL Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_PAL3DDIFFM1SEL Fld(2,0,AC_MSKB0)//[1:0]
#define C_COMB_1D (IO_COMB_BASE + 0x6B4)
    #define REG_CHROMASUMBGTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_CZPSEL Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_CZPTH Fld(7,16,AC_MSKB2)//[22:16]
    #define REG_CZPPIXCNTTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_CCDTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_1E (IO_COMB_BASE + 0x6B8)
    #define REG_GAIN3DST Fld(4,28,AC_MSKB3)//[31:28]
    #define REG_GAIN2DST Fld(4,24,AC_MSKB3)//[27:24]
    #define REG_GAIN3D_C3DSM Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_GAIN2D_C3DSM Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_GAIN3DL Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_GAIN2DL Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_GAIN3DN Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_GAIN2DN Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_1F (IO_COMB_BASE + 0x6BC)
    #define REG_MOTION9X3TH_LS_NTSC Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_C3D_FIR_SEL Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_EN_C3D_FIR Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_D3BIGTH_MIN Fld(2,20,AC_MSKB2)//[21:20]
    #define REG_ENSWEEPGDET Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_ENSWEEPGAINCV Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_ENSKINDET1 Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_ENLOCALSTILLG Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_ENSPC Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_ENSPCPT Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_ENSKINDET Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_ENCTMB Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_ENSLOWDET Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_PALCBIGSEL1 Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_PALCBIGSEL2 Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_CTMBSEL1 Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_CTMBSEL2 Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_EN_PATCH_STAIR Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_PALC5X3SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_DIRINSEL Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_WITHCLRTH_5 Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_WITHCLRTH_4 Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_EN_ANT_PATCH Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_20 (IO_COMB_BASE + 0x6C0)
    #define REG_VDGYHORSMOOTHTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_D3SLOWGAIN1 Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3SLOWGAIN2 Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_D3SLOWGAIN3 Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D2SLOWGAIN1 Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2SLOWGAIN2 Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2SLOWGAIN3 Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_21 (IO_COMB_BASE + 0x6C4)
    #define REG_CVSMTHS Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_YSMTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_OFFSETTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_CVSMTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_22 (IO_COMB_BASE + 0x6C8)
    #define REG_CVBSSWEEPTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_CDIFTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_CVBSDIFFTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_C3DSMTHWF Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_23 (IO_COMB_BASE + 0x6CC)
    #define REG_MBMAXTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_LINEAR_VAR_OFFSET Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_LINEAR_VAR_MAX Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_UNIFORMTH4CCS Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_SLOW2DHPTH2 Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_24 (IO_COMB_BASE + 0x6D0)
    #define REG_SWEEPBPFTH Fld(7,25,AC_MSKB3)//[31:25]
    #define REG_DISSWEEPBP Fld(1,24,AC_MSKB3)//[24:24]
    #define REG_PALSW2 Fld(2,22,AC_MSKB2)//[23:22]
    #define REG_PALSW1 Fld(2,20,AC_MSKB2)//[21:20]
    #define REG_PHBIN4 Fld(5,15,AC_MSKW21)//[19:15]
    #define REG_PHBIN3 Fld(5,10,AC_MSKB1)//[14:10]
    #define REG_PHBIN2 Fld(5,5,AC_MSKW10)//[9:5]
    #define REG_PHBIN1 Fld(5,0,AC_MSKB0)//[4:0]
#define C_COMB_25 (IO_COMB_BASE + 0x6D4)
    #define REG_STIFRMTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_MOTION_THBIG Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_SMLMOTHC Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_HYEDGETH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_26 (IO_COMB_BASE + 0x6D8)
    #define REG_BIGMOTH Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_DZSM2DTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_YDIFTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_27 (IO_COMB_BASE + 0x6DC)
    #define REG_SLOWY3DTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_BSWDETPIXPAL Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_SMLCNTH Fld(12,8,AC_MSKW21)//[19:8]
    #define REG_BSWDETTHPAL Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_28 (IO_COMB_BASE + 0x6E0)
    #define EN_BLOCK_BY_SEED Fld(1,24,AC_MSKB3)//[24:24]
    #define ADYSEL Fld(1,23,AC_MSKB2)//[23:23]
    #define SETZERO Fld(1,22,AC_MSKB2)//[22:22]
    #define FIFORY Fld(6,16,AC_MSKB2)//[21:16]
    #define DML_METHOD Fld(1,15,AC_MSKB1)//[15:15]
    #define DML_FSEL Fld(1,14,AC_MSKB1)//[14:14]
    #define FIFORX Fld(6,8,AC_MSKB1)//[13:8]
    #define DMEGSEL Fld(1,7,AC_MSKB0)//[7:7]
    #define DMTPSEL Fld(1,6,AC_MSKB0)//[6:6]
    #define FIFORM1 Fld(6,0,AC_MSKB0)//[5:0]
#define C_COMB_29 (IO_COMB_BASE + 0x6E4)
    #define WVSYNCTH Fld(7,25,AC_MSKB3)//[31:25]
    #define DRAMBASEADR Fld(25,0,AC_MSKDW)//[24:0]
#define C_COMB_2A (IO_COMB_BASE + 0x6E8)
    #define EN3D Fld(1,31,AC_MSKB3)//[31:31]
    #define ENFORCE3D Fld(1,30,AC_MSKB3)//[30:30]
    #define COMB_OPTION Fld(2,28,AC_MSKB3)//[29:28]
    #define B10MODE Fld(1,27,AC_MSKB3)//[27:27]
    #define FORCEDRAM Fld(1,26,AC_MSKB3)//[26:26]
    #define GAMEMODE Fld(1,25,AC_MSKB3)//[25:25]
    #define ENPGMODE Fld(1,24,AC_MSKB3)//[24:24]
    #define WITHCLRTH_0 Fld(1,23,AC_MSKB2)//[23:23]
    #define ENVCRFSIG2 Fld(1,22,AC_MSKB2)//[22:22]
    #define ENVCRFSIG1 Fld(1,21,AC_MSKB2)//[21:21]
    #define HCNT3D Fld(10,11,AC_MSKW21)//[20:11]
    #define HLEN3D Fld(11,0,AC_MSKW10)//[10:0]
#define C_COMB_2B (IO_COMB_BASE + 0x6EC)
    #define VLINEST Fld(7,25,AC_MSKB3)//[31:25]
    #define VLINECNT Fld(9,16,AC_MSKW32)//[24:16]
    #define DUMPSTAL Fld(1,15,AC_MSKB1)//[15:15]
    #define ENCKILLWFIFO Fld(1,14,AC_MSKB1)//[14:14]
    #define LBUFSEL Fld(2,12,AC_MSKB1)//[13:12]
    #define DUMPSEL Fld(4,8,AC_MSKB1)//[11:8]
    #define ICTST Fld(1,7,AC_MSKB0)//[7:7]
    #define ICTSTLOCK Fld(1,6,AC_MSKB0)//[6:6]
    #define HOLD Fld(1,5,AC_MSKB0)//[5:5]
    #define FSKBACK Fld(1,4,AC_MSKB0)//[4:4]
    #define ENSHORT Fld(1,3,AC_MSKB0)//[3:3]
    #define CLRSRAM Fld(1,2,AC_MSKB0)//[2:2]
    #define ENYCCKILL Fld(1,1,AC_MSKB0)//[1:1]
    #define ENCKILL Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_2C (IO_COMB_BASE + 0x6F0)
    #define REG_SLOW3DHPTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_SLOWD2SEL Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_DISUNIFORM Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_EN_TMB_4PV Fld(1,21,AC_MSKB2)//[21:21]
    #define REG_DISVODINMCLKGATED Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_MOTION9X3TH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_SLOW2DHPTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_2D (IO_COMB_BASE + 0x6F4)
    #define VCNT3D_M Fld(7,25,AC_MSKB3)//[31:25]
    #define VLEN3D_M Fld(9,16,AC_MSKW32)//[24:16]
    #define HSYNCST Fld(5,11,AC_MSKB1)//[15:11]
    #define WINSEL Fld(1,10,AC_MSKB1)//[10:10]
    #define HCNT3D_M Fld(10,0,AC_MSKW10)//[9:0]
#define C_COMB_2E (IO_COMB_BASE + 0x6F8)
    #define REG_CTMBEDGETH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_CTMB_CVBSTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_MBTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_HFTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_2F (IO_COMB_BASE + 0x6FC)
    #define CRCSEL Fld(1,31,AC_MSKB3)//[31:31]
    #define ENHHHP4SLOW Fld(1,28,AC_MSKB3)//[28:28]
    #define DUMPNOW Fld(1,27,AC_MSKB3)//[27:27]
    #define HLEN3D_M Fld(11,16,AC_MSKW32)//[26:16]
    #define WVSYNCTH_0 Fld(1,15,AC_MSKB1)//[15:15]
    #define ENDUMPENDADR Fld(1,14,AC_MSKB1)//[14:14]
    #define ADB4SEL Fld(2,12,AC_MSKB1)//[13:12]
    #define AD10BITMODE Fld(1,11,AC_MSKB1)//[11:11]
    #define AD8BITMODE Fld(1,10,AC_MSKB1)//[10:10]
    #define AD4BITMODE Fld(1,9,AC_MSKB1)//[9:9]
    #define DUMP_LCNTSEL Fld(9,0,AC_MSKW10)//[8:0]
#define COMB2D_30 (IO_COMB_BASE + 0x700)
    #define FW_RESERVE Fld(32,0,AC_FULLDW)//[31:0]
#define COMB2D_31 (IO_COMB_BASE + 0x704)
    #define HFY_YTH2 Fld(7,24,AC_MSKB3)//[30:24]
    #define VWY2CHROMATH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_SPC_HALFTH Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_SPC_SUMTH Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_ENUNIFORM_SPCLR Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_ENBOUND Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_BOUNDLEVELSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define CSEL Fld(2,2,AC_MSKB0)//[3:2]
    #define YSEL Fld(2,0,AC_MSKB0)//[1:0]
#define COMB2D_32 (IO_COMB_BASE + 0x708)
    #define K2H4MB Fld(4,28,AC_MSKB3)//[31:28]
    #define K2HSEL Fld(2,26,AC_MSKB3)//[27:26]
    #define TBTH_K2B Fld(7,16,AC_MSKB2)//[22:16]
    #define K2HVERTH Fld(8,8,AC_FULLB1)//[15:8]
    #define CVARTH Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_33 (IO_COMB_BASE + 0x70C)
    #define VYD_GAIN_HFY2 Fld(4,28,AC_MSKB3)//[31:28]
    #define VCD_GAIN_HFY2 Fld(4,24,AC_MSKB3)//[27:24]
    #define HYD_GAIN_HFY2 Fld(4,20,AC_MSKB2)//[23:20]
    #define HCD_GAIN_HFY2 Fld(4,16,AC_MSKB2)//[19:16]
    #define MBVTH_HFY2 Fld(8,8,AC_FULLB1)//[15:8]
    #define HDGAIN_HFY2 Fld(4,4,AC_MSKB0)//[7:4]
    #define HFYTBSEL2 Fld(1,2,AC_MSKB0)//[2:2]
    #define ENPALVCD2 Fld(1,1,AC_MSKB0)//[1:1]
    #define ENFIXVCD2 Fld(1,0,AC_MSKB0)//[0:0]
#define COMB2D_34 (IO_COMB_BASE + 0x710)
    #define K2_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define K2_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define K2_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define K2_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define K2_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define K2_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define K2_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define K2_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_35 (IO_COMB_BASE + 0x714)
    #define LEGALCH_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define LEGALCH_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define LEGALCH_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define LEGALCH_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define LEGALCH_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define LEGALCH_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define LEGALCH_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define LEGALCH_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_36 (IO_COMB_BASE + 0x718)
    #define LEGALCV_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define LEGALCV_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define LEGALCV_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define LEGALCV_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define LEGALCV_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define LEGALCV_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define LEGALCV_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define LEGALCV_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_37 (IO_COMB_BASE + 0x71C)
    #define ILLEGALC_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define ILLEGALC_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define ILLEGALC_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define ILLEGALC_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define ILLEGALC_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define ILLEGALC_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define ILLEGALC_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define ILLEGALC_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define COMB2D_38 (IO_COMB_BASE + 0x720)
	#define CHANNEL_B_SEL Fld(1,29,AC_MSKB3)//[29:29]
	#define DRAMBASEADR_MSB Fld(1,28,AC_MSKB3)//[28:28]
    #define CON4_B4 Fld(1,27,AC_MSKB3)//[27:27]
    #define CON4_B1 Fld(1,26,AC_MSKB3)//[26:26]
    #define CON4_B2 Fld(1,25,AC_MSKB3)//[25:25]
    #define CON4_B3 Fld(1,24,AC_MSKB3)//[24:24]
    #define U1_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_SPC_UVTH Fld(8,8,AC_FULLB1)//[15:8]
    #define CLGTH Fld(8,0,AC_FULLB0)//[7:0]
#define COMB2D_39 (IO_COMB_BASE + 0x724)
    #define INKSEL Fld(1,30,AC_MSKB3)//[30:30]
    #define INK Fld(6,24,AC_MSKB3)//[29:24]
    #define TBTH_K2 Fld(7,16,AC_MSKB2)//[22:16]
    #define TBTH_K1V Fld(7,8,AC_MSKB1)//[14:8]
    #define TBTH_K1H Fld(7,0,AC_MSKB0)//[6:0]
#define C_COMB_3A (IO_COMB_BASE + 0x728)
    #define CLRFULL Fld(1,3,AC_MSKB0)//[3:3]
    #define CLREMPTY Fld(1,2,AC_MSKB0)//[2:2]
    #define FULLSTA Fld(1,1,AC_MSKB0)//[1:1]
    #define EMPTYSTA Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_3B (IO_COMB_BASE + 0x72C)
    #define REG_DISSLOWCHROMA Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_ENCOMBTP_Y Fld(1,29,AC_MSKB3)//[29:29]
    #define REG_ENCOMBTP_C Fld(1,28,AC_MSKB3)//[28:28]
    #define REG_HCNT3DSYNC Fld(10,16,AC_MSKW32)//[25:16]
    #define REG_CZPPIXCNTTH2 Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_SLOWCHROMATH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_3C (IO_COMB_BASE + 0x730)
    #define COMB_DE_GEN_B3_MAX Fld(4,28,AC_MSKB3)//[31:28]
    #define COMB_DE_GEN_C_MAX Fld(4,24,AC_MSKB3)//[27:24]
    #define COMB_DE_GEN_C_TH Fld(6,16,AC_MSKB2)//[21:16]
    #define COMB_V_EDGE_TH Fld(6,8,AC_MSKB1)//[13:8]
    #define COMB_NEW_2D_C_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define COMB_NEW_2D_EN Fld(1,0,AC_MSKB0)//[0:0]
#define C_COMB_3D (IO_COMB_BASE + 0x734)
    #define REG_3D_FW_COST Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_2D_FW_COST Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_NOISETH Fld(8,8,AC_FULLB1)//[15:8]
    #define DIS_BACKGROUND_SAME Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_SAMECSEL Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_ENCZPHS Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_ENSAMEC Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_DELAYSEL2 Fld(2,0,AC_MSKB0)//[1:0]
#define C_COMB_3E (IO_COMB_BASE + 0x738)
    #define COMB_DE_GEN_B4_TH Fld(6,24,AC_MSKB3)//[29:24]
    #define COMB_DE_GEN_B3_TH Fld(6,16,AC_MSKB2)//[21:16]
    #define COMB_DE_GEN_B2_TH Fld(6,8,AC_MSKB1)//[13:8]
    #define COMB_DE_GEN_B1_TH Fld(6,0,AC_MSKB0)//[5:0]
#define C_COMB_3F (IO_COMB_BASE + 0x73C)
    #define REG_PATCH_TMB_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define WRONG_EDGE_TH Fld(3,21,AC_MSKB2)//[23:21]
    #define DIS_WRONG_EDGE Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_DISCLUMAEDGE Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_PALY101SEL Fld(2,17,AC_MSKB2)//[18:17]
    #define REG_ENPHDIF2 Fld(1,16,AC_MSKB2)//[16:16]
    #define ONE_LINE_VER_DIFF Fld(8,8,AC_FULLB1)//[15:8]
    #define ONE_LINE_HOR_TH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_40 (IO_COMB_BASE + 0x740)
    #define REG_F2BLEN Fld(4,28,AC_MSKB3)//[31:28]
    #define REG_F1BLEN Fld(4,24,AC_MSKB3)//[27:24]
    #define REG_F0BLEN Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3SWEEPGAIN3 Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_CHECKFTH Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_CHECKMAXF Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_KILLY_COMBITH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_41 (IO_COMB_BASE + 0x744)
    #define REG_UNIFORMTH4SKIN Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_DISUNIFORMTH4SKIN Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_ENWHOLEVSMOOTH Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_DISHORMO4CVBS Fld(1,21,AC_MSKB2)//[21:21]
    #define REG_ENFORCEC3D Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_ENFORCEY3D Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_ENFORCEY2D Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_DISMO4PV Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_DISMO4MB Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_MOTH4PV Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_MOTH4MB Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_42 (IO_COMB_BASE + 0x748)
    #define REG_SLOWVERTH Fld(6,26,AC_MSKB3)//[31:26]
    #define REG_DISSLOWVER Fld(1,25,AC_MSKB3)//[25:25]
    #define REG_DRAMENDADR Fld(25,0,AC_MSKDW)//[24:0]
#define C_COMB_43 (IO_COMB_BASE + 0x74C)
    #define REG_CVHPSMTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_DISSKINMOTION Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_DISSKIN4LOCAL Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_MOTIONTH4SKIN Fld(5,16,AC_MSKB2)//[20:16]
    #define REG_EN_PUREVER_NEW Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_V_EDGE_TH Fld(4,11,AC_MSKB1)//[14:11]
    #define REG_GRAYVERSMSEL Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_UNIFORMSEL Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_MOTIONTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_44 (IO_COMB_BASE + 0x750)
    #define REG_MBTHSEL Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_CVBSTH2 Fld(6,24,AC_MSKB3)//[29:24]
    #define REG_ENCMOTION4PURE Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_DISSTILLSKIN Fld(1,22,AC_MSKB2)//[22:22]
    #define REG_CVBSTH1 Fld(6,16,AC_MSKB2)//[21:16]
    #define REG_UNIAMPSM Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_KYLINETH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_45 (IO_COMB_BASE + 0x754)
    #define FLASH_NEI_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define FLASH_MAX_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define GAIN_SLOP Fld(8,8,AC_FULLB1)//[15:8]
    #define SLOP_OFFSET Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_46 (IO_COMB_BASE + 0x758)
    #define SP_VAR_MAX Fld(8,24,AC_FULLB3)//[31:24]
    #define SP_VAR_TH Fld(8,16,AC_FULLB2)//[23:16]
    #define MANUAL_NOISE_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define MANUAL_NOISE Fld(1,14,AC_MSKB1)//[14:14]
    #define EN_TWO_LINE_MAX Fld(1,13,AC_MSKB1)//[13:13]
    #define FLASH_FRM_MAX Fld(4,9,AC_MSKB1)//[12:9]
    #define BENABLENEW2D Fld(1,8,AC_MSKB1)//[8:8]
    #define EN_VERYLP Fld(1,7,AC_MSKB0)//[7:7]
    #define BENABLENEW3D Fld(1,6,AC_MSKB0)//[6:6]
    #define DHYSEL Fld(1,5,AC_MSKB0)//[5:5]
    #define D2D3SMALL_PRIORITY Fld(1,4,AC_MSKB0)//[4:4]
    #define FLASH_FRM Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_47 (IO_COMB_BASE + 0x75C)
    #define CHROMA3D_OFFSET Fld(8,24,AC_FULLB3)//[31:24]
    #define CHROMA3D_SLOP Fld(8,16,AC_FULLB2)//[23:16]
    #define LUMAHOR_OFFSET Fld(8,8,AC_FULLB1)//[15:8]
    #define LUMAHOR_SLOP Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_48 (IO_COMB_BASE + 0x760)
    #define ENFIXVCD Fld(1,31,AC_MSKB3)//[31:31]
    #define VCD_TBTH Fld(7,24,AC_MSKB3)//[30:24]
    #define INKTH2D Fld(8,8,AC_FULLB1)//[15:8]
    #define TBTH_CVAR Fld(7,0,AC_MSKB0)//[6:0]
#define C_COMB_49 (IO_COMB_BASE + 0x764)
    #define LUMALEVEL_TH Fld(8,24,AC_FULLB3)//[31:24]
    #define LUMALEVEL_SLOP Fld(8,16,AC_FULLB2)//[23:16]
    #define FLASH_AVG_TH Fld(8,8,AC_FULLB1)//[15:8]
    #define FLASH_PIX_TH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_4A (IO_COMB_BASE + 0x768)
    #define REG_LOSTI_LUMATH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_LOSTI_CVBSTH Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_UNIFORMTH Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_YVERDIFSMTH Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_4B (IO_COMB_BASE + 0x76C)
    #define VCD_TB7 Fld(4,28,AC_MSKB3)//[31:28]
    #define VCD_TB6 Fld(4,24,AC_MSKB3)//[27:24]
    #define VCD_TB5 Fld(4,20,AC_MSKB2)//[23:20]
    #define VCD_TB4 Fld(4,16,AC_MSKB2)//[19:16]
    #define VCD_TB3 Fld(4,12,AC_MSKB1)//[15:12]
    #define VCD_TB2 Fld(4,8,AC_MSKB1)//[11:8]
    #define VCD_TB1 Fld(4,4,AC_MSKB0)//[7:4]
    #define VCD_TB0 Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_4C (IO_COMB_BASE + 0x770)
    #define DRAM_FRAME_CNT1 Fld(3,28,AC_MSKB3)//[30:28]
    #define DRAM_FRAME_CNT2 Fld(3,24,AC_MSKB3)//[26:24]
    #define DRAM_FRAME_CNT3 Fld(3,20,AC_MSKB2)//[22:20]
    #define REG_DISSWEEPDET Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_DISSTILLPV Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_DISSTILLHS Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_ENHSTO3D Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_EN2DPUREVER1 Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_ENCVMO Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_ENMBEXIST Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_ENCVBSVERSMOOTH Fld(1,11,AC_MSKB1)//[11:11]
    #define REG_ENCVBSHORSMOOTH Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_HORMBTH Fld(5,5,AC_MSKW10)//[9:5]
    #define REG_VERMBTH Fld(5,0,AC_MSKB0)//[4:0]
#define C_COMB_4D (IO_COMB_BASE + 0x774)
    #define REG_ENLOCALSTILL Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_Y101DEFSEL Fld(2,29,AC_MSKB3)//[30:29]
    #define REG_BLDCSEL Fld(2,27,AC_MSKB3)//[28:27]
    #define REG_DIFDIFSEL Fld(1,26,AC_MSKB3)//[26:26]
    #define REG_ENYNHPSM Fld(1,25,AC_MSKB3)//[25:25]
    #define REG_ENPHDIF1 Fld(1,24,AC_MSKB3)//[24:24]
    #define REG_IVPHTH Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_UVSMSEL Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_UVSMALLTH_TDC Fld(8,0,AC_FULLB0)//[7:0]
#define C_COMB_4E (IO_COMB_BASE + 0x778)
    #define REG_SKINBRIGHTTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_D3SKINGAIN1 Fld(4,20,AC_MSKB2)//[23:20]
    #define REG_D3SKINGAIN2 Fld(4,16,AC_MSKB2)//[19:16]
    #define REG_D3SKINGAIN3 Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_D2SKINGAIN1 Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2SKINGAIN2 Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2SKINGAIN3 Fld(4,0,AC_MSKB0)//[3:0]
#define C_COMB_4F (IO_COMB_BASE + 0x77C)
    #define REG_FIRMOTIONTH Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_DISFIRINCBG Fld(1,23,AC_MSKB2)//[23:23]
    #define REG_WDATASEL Fld(2,21,AC_MSKB2)//[22:21]
    #define REG_ENADAPTCFIR Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_ENFORCECFIR Fld(1,19,AC_MSKB2)//[19:19]
    #define REG_ENPALDRAM Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_ENPAL45HS Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_CFIRSEL Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_NEWHS Fld(3,12,AC_MSKB1)//[14:12]
    #define REG_D2CTMBGAIN3 Fld(4,8,AC_MSKB1)//[11:8]
    #define REG_D2CTMBGAIN2 Fld(4,4,AC_MSKB0)//[7:4]
    #define REG_D2CTMBGAIN1 Fld(4,0,AC_MSKB0)//[3:0]


#endif // #ifndef _HW_TDC_H_

