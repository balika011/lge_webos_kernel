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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: hw_ycproc.h $
 *
 * Project:
 * --------
 *   MT5381
 *
 * Description:
 * ------------
 *   LCPROC register definition
 *
 * Author:
 * -------
 *   Jeffrey Xuan
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Modtime: 07/04/06 1:22p $
 *
 * $Revision: #1 $
****************************************************************************/

#ifndef _HW_YCPROC_H_
#define _HW_YCPROC_H_

#include "hw_vdoin.h"

#define IO_PREPROC_BASE   (VDOIN_BASE+0x3000)
#define IO_SCALER_BASE    (VDOIN_BASE+0x5000)
#define IO_POSTPROC_BASE  (VDOIN_BASE+0x7000)
#define IO_POSTCOLOR_BASE (VDOIN_COLOR_BASE+0xD000)

// TOP
#define TOP_MAIN_00 (IO_PREPROC_BASE + 0x000)
    #define C_METRIC_DATA_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define C_METRIC_V_INV Fld(1,26,AC_MSKB3)//[26:26]
    #define C_DET_DATA_SEL Fld(1,25,AC_MSKB3)//[25:25]
    #define C_DET_V_INV Fld(1,24,AC_MSKB3)//[24:24]
    #define BYPASS_DIV2_REGEN_M Fld(1,23,AC_MSKB2)//[23:23]
    #define C_M2V_INV Fld(1,22,AC_MSKB2)//[22:22]
    #define C_M2F_INV Fld(1,21,AC_MSKB2)//[21:21]
    #define C_MAIN_V_RST Fld(1,20,AC_MSKB2)//[20:20]
    #define C_MAIN_OUT_RGB Fld(1,5,AC_MSKB0)//[5:5]
    #define C_MAIN_OUT_BLANK Fld(1,4,AC_MSKB0)//[4:4]
    #define C_CRC_SEL Fld(2,2,AC_MSKB0)//[3:2]
    #define C_NR_SRC_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define C_MAIN_NR_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TOP_MAIN_01 (IO_PREPROC_BASE + 0x004)
    #define C_RST_CTRL Fld(8,24,AC_FULLB3)//[31:24]
    #define C_PRE_PROC_DBG_MODE Fld(4,5,AC_MSKW10)//[8:5]
    #define C_DBG_OUT_SEL Fld(5,0,AC_MSKB0)//[4:0]
#define TOP_PIP_00 (IO_PREPROC_BASE + 0x200)
    #define C_SHOW_POS_PIP_POSITION Fld(1,31,AC_MSKB3)//[31:31]
    #define C_SHOW_PIP_STEP Fld(1,27,AC_MSKB3)//[27:27]
    #define BYPASS_DIV2_REGEN_P Fld(1,23,AC_MSKB2)//[23:23]
    //#define C_P2H_INV Fld(1,23,AC_MSKB2)//[23:23]
    #define C_P2V_INV Fld(1,22,AC_MSKB2)//[22:22]
    #define C_P2F_INV Fld(1,21,AC_MSKB2)//[21:21]
    #define C_PIP2_V_RST Fld(1,20,AC_MSKB2)//[20:20]
    #define C_PIP_OUT_RGB Fld(1,5,AC_MSKB0)//[5:5]
    #define C_PIP_OUT_BLANK Fld(1,4,AC_MSKB0)//[4:4]
    #define C_PIP_SRC_SEL Fld(1,0,AC_MSKB0)//[0:0]
#define TOP_PIP_01 (IO_PREPROC_BASE + 0x204)
    #define C_SHOW_POS_PIP_X Fld(11,16,AC_MSKW32)//[26:16]
    #define C_SHOW_POS_PIP_Y Fld(11,0,AC_MSKW10)//[10:0]

// CRC
#define CRC_00 (IO_PREPROC_BASE + 0x020)
#define CRC_PIP_00 (IO_PREPROC_BASE + 0x220)
    #define C_BOT_L_EN_MAIN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_TOP_L_EN_MAIN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_V_IN_MASK_MAIN Fld(10,20,AC_MSKW32)//[29:20]
    #define C_C_IN_MASK_MAIN Fld(10,10,AC_MSKW21)//[19:10]
    #define C_Y_IN_MASK_MAIN Fld(10,0,AC_MSKW10)//[9:0]
#define CRC_01 (IO_PREPROC_BASE + 0x024)
#define CRC_PIP_01 (IO_PREPROC_BASE + 0x224)
    #define C_BOT_R_EN_MAIN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_TOP_R_EN_MAIN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_CHECK_SUM_MODE_MAIN Fld(1,29,AC_MSKB3)//[29:29]
    #define C_STILL_CHECK_TRIG_MAIN Fld(1,28,AC_MSKB3)//[28:28]
    #define C_STILL_CHECK_MAX_MAIN Fld(8,20,AC_MSKW32)//[27:20]
    #define C_CRC_CLR Fld(1,0,AC_MSKB0)//[0:0]
#define CRC_02 (IO_PREPROC_BASE + 0x028)
#define CRC_PIP_02 (IO_PREPROC_BASE + 0x228)
    #define C_CLIP_H_START_MAIN Fld(13,12,AC_MSKDW)//[24:12]
    #define C_CLIP_V_START_MAIN Fld(12,0,AC_MSKW10)//[11:0]
#define CRC_03 (IO_PREPROC_BASE + 0x02C)
#define CRC_PIP_03 (IO_PREPROC_BASE + 0x22C)
    #define PIP_ATPG_CT Fld(1,31,AC_MSKB3)//[31:31]
    #define PIP_ATPG_OB Fld(1,30,AC_MSKB3)//[30:30]
    #define C_CLIP_H_END_MAIN Fld(13,12,AC_MSKDW)//[24:12]
    #define C_CLIP_V_END_MAIN Fld(12,0,AC_MSKW10)//[11:0]

#define STA_CRC_MAIN_01 (IO_PREPROC_BASE + 0x044)
    #define STA_CRC_H_OUT Fld(32,0,AC_FULLDW)//[31:0]
#define STA_CRC_MAIN_02 (IO_PREPROC_BASE + 0x048)
    #define STA_CRC_V_OUT Fld(32,0,AC_FULLDW)//[31:0]
#define STA_CRC_MAIN_03 (IO_PREPROC_BASE + 0x04C)
    #define MAIN_ATPG_CT Fld(1,31,AC_MSKB3)//[31:31]
    #define MAIN_ATPG_OB Fld(1,30,AC_MSKB3)//[30:30]
    #define STA_NON_STILL_CNT Fld(4,2,AC_MSKB0)//[5:2]
    #define STA_STILL_CHECK_DONE Fld(1,1,AC_MSKB0)//[1:1]
    #define STA_CRC_MAIN_RDY Fld(1,0,AC_MSKB0)//[0:0]

// YCBCR2YC
#define YCBCR2YC_MAIN_00 (IO_PREPROC_BASE + 0x1F0)
#define YCBCR2YC_PIP_00 (IO_PREPROC_BASE + 0x3F0)
    #define C_MAIN_LEFT_PADDING Fld(1,7,AC_MSKB0)//[7:7]
    #define C_MAIN_MIRROR_MODE Fld(1,6,AC_MSKB0)//[6:6]
    #define C_MAIN_EVEN_EXTEND Fld(1,5,AC_MSKB0)//[5:5]
    #define C_MAIN_NOSYNC_BLOCK Fld(1,4,AC_MSKB0)//[4:4]
    #define C_MAIN_KEEP_LAST_DATA Fld(1,3,AC_MSKB0)//[3:3]
    #define C_MAIN_HSYNC_BLACK Fld(1,2,AC_MSKB0)//[2:2]
    #define C_MAIN_FILTER_ON Fld(1,1,AC_MSKB0)//[1:1]
    #define C_MAIN_ENA Fld(1,0,AC_MSKB0)//[0:0]

// PAT_GEN
#define PAT_MAIN_00 (IO_PREPROC_BASE + 0x0C0)
#define PAT_PIP_00 (IO_PREPROC_BASE + 0x2B0)
    #define C_MAIN_SELFGEN_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_MAIN_SELF_DISP_R_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_MAIN_SELF_FLD_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define C_MAIN_SELF_VT Fld(13,16,AC_MSKW32)//[28:16]
    #define C_MAIN_TIME_MODE Fld(2,14,AC_MSKB1)//[15:14]
    #define C_MAIN_SELF_HT Fld(14,0,AC_MSKW10)//[13:0]
#define PAT_MAIN_01 (IO_PREPROC_BASE + 0x0C4)
#define PAT_PIP_01 (IO_PREPROC_BASE + 0x2C4)
    #define C_MAIN_TIME_EN Fld(4,28,AC_MSKB3)//[31:28]
    #define C_MAIN_V_ACT Fld(12,16,AC_MSKW32)//[27:16]
    #define C_MAIN_TIME_STEP Fld(3,13,AC_MSKB1)//[15:13]
    #define C_MAIN_H_ACT Fld(13,0,AC_MSKW10)//[12:0]
#define PAT_MAIN_02 (IO_PREPROC_BASE + 0x0C8)
#define PAT_PIP_02 (IO_PREPROC_BASE + 0x2C8)
    #define C_MAIN_SELF_DE_N Fld(5,27,AC_MSKB3)//[31:27]
    #define C_MAIN_NOISE_EN Fld(3,24,AC_MSKB3)//[26:24]
    #define C_MAIN_V_OFFSET Fld(8,16,AC_FULLB2)//[23:16]
    #define C_MAIN_PAT_TYPE Fld(4,12,AC_MSKB1)//[15:12]
    #define C_MAIN_NOISE_LEVEL Fld(4,8,AC_MSKB1)//[11:8]
    #define C_MAIN_H_OFFSET Fld(8,0,AC_FULLB0)//[7:0]
#define PAT_MAIN_03 (IO_PREPROC_BASE + 0x0CC)
#define PAT_PIP_03 (IO_PREPROC_BASE + 0x2CC)
    #define C_MAIN_SELF_DE_M Fld(5,27,AC_MSKB3)//[31:27]
    #define PRE_PAT_MAIN_ATPG_OB Fld(1,26,AC_MSKB3)//[26:26]
    #define PRE_PAT_MAIN_ATPG_CT Fld(1,25,AC_MSKB3)//[25:25]
    #define C_MAIN_GRID_WIDTH Fld(1,24,AC_MSKB3)//[24:24]
    #define C_MAIN_V_GRID Fld(8,16,AC_FULLB2)//[23:16]
    #define C_MAIN_BORDER_EN Fld(4,12,AC_MSKB1)//[15:12]
    #define C_MAIN_MAX_F Fld(4,8,AC_MSKB1)//[11:8]
    #define C_MAIN_H_GRID Fld(8,0,AC_FULLB0)//[7:0]
#define PAT_MAIN_04 (IO_PREPROC_BASE + 0x0D0)
#define PAT_PIP_04 (IO_PREPROC_BASE + 0x2D0)
    #define C_MAIN_HV_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_MAIN_FC_CR Fld(10,20,AC_MSKW32)//[29:20]
    #define C_MAIN_FC_CB Fld(10,10,AC_MSKW21)//[19:10]
    #define C_MAIN_FC_Y Fld(10,0,AC_MSKW10)//[9:0]
#define PAT_MAIN_05 (IO_PREPROC_BASE + 0x0D4)
#define PAT_PIP_05 (IO_PREPROC_BASE + 0x2D4)
    #define C_MAIN_COLOR_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_MAIN_BC_CR Fld(10,20,AC_MSKW32)//[29:20]
    #define C_MAIN_BC_CB Fld(10,10,AC_MSKW21)//[19:10]
    #define C_MAIN_BC_Y Fld(10,0,AC_MSKW10)//[9:0]
#define PAT_MAIN_06 (IO_PREPROC_BASE + 0x0D8)
#define PAT_PIP_06 (IO_PREPROC_BASE + 0x2D8)
    #define C_MAIN_PRB_EN Fld(4,28,AC_MSKB3)//[31:28]
    #define C_MAIN_PRB_Y_POS Fld(12,16,AC_MSKW32)//[27:16]
    #define C_MAIN_PRB_CURSOR_ON Fld(1,15,AC_MSKB1)//[15:15]
    #define C_MAIN_PRB_OSD_ON Fld(1,14,AC_MSKB1)//[14:14]
    #define C_MAIN_PRB_X_POS Fld(13,0,AC_MSKW10)//[12:0]
#define PAT_MAIN_07 (IO_PREPROC_BASE + 0x0DC)
#define PAT_PIP_07 (IO_PREPROC_BASE + 0x2DC)
    #define S_MAIN_PRB_DISP_R Fld(1,31,AC_MSKB3)//[31:31]
    #define S_MAIN_PRB_FLD Fld(1,30,AC_MSKB3)//[30:30]
    #define S_MAIN_PRB_CR Fld(10,20,AC_MSKW32)//[29:20]
    #define S_MAIN_PRB_CB Fld(10,10,AC_MSKW21)//[19:10]
    #define S_MAIN_PRB_Y Fld(10,0,AC_MSKW10)//[9:0]
    
//  3x3 MATRIX
#define TRANS_MAIN_00 (IO_PREPROC_BASE + 0x084)
#define TRANS_PIP_00 (IO_PREPROC_BASE + 0x284)
    #define OUT_S2P Fld(1,31,AC_MSKB3)//[31:31]
    #define OUT_S1P Fld(1,30,AC_MSKB3)//[30:30]
    #define OUT_S0P Fld(1,29,AC_MSKB3)//[29:29]
    #define CLAMP_S2P Fld(1,28,AC_MSKB3)//[28:28]
    #define CLAMP_S1P Fld(1,27,AC_MSKB3)//[27:27]
    #define CLAMP_S0P Fld(1,26,AC_MSKB3)//[26:26]
    #define IN_S2P Fld(1,25,AC_MSKB3)//[25:25]
    #define IN_S1P Fld(1,24,AC_MSKB3)//[24:24]
    #define IN_S0P Fld(1,23,AC_MSKB2)//[23:23]
    #define IN_OFFSET_1P Fld(11,12,AC_MSKW21)//[22:12]
    #define IN_OFFSET_1P_PIP Fld(9,12,AC_MSKW21)//[20:12]
    #define MAIN_EASY_BP3X3 Fld(1,11,AC_MSKB1)//[11:11]
    #define IN_OFFSET_0P Fld(11,0,AC_MSKW10)//[10:0]
    #define IN_OFFSET_0P_PIP Fld(9,0,AC_MSKW10)//[8:0]
#define TRANS_MAIN_01 (IO_PREPROC_BASE + 0x088)
#define TRANS_PIP_01 (IO_PREPROC_BASE + 0x288)
    #define OUT_OFFSET_0P Fld(11,12,AC_MSKW21)//[22:12]
    #define OUT_OFFSET_0P_PIP Fld(9,12,AC_MSKW21)//[20:12]
    #define IN_OFFSET_2P Fld(11,0,AC_MSKW10)//[10:0]
    #define IN_OFFSET_2P_PIP Fld(9,0,AC_MSKW10)//[8:0]
#define TRANS_MAIN_02 (IO_PREPROC_BASE + 0x08C)
#define TRANS_PIP_02 (IO_PREPROC_BASE + 0x28C)
    #define OUT_OFFSET_2P Fld(11,12,AC_MSKW21)//[22:12]
    #define OUT_OFFSET_2P_PIP Fld(9,12,AC_MSKW21)//[20:12]
    #define OUT_OFFSET_1P Fld(11,0,AC_MSKW10)//[10:0]
    #define OUT_OFFSET_1P_PIP Fld(9,0,AC_MSKW10)//[8:0]
#define TRANS_MAIN_03 (IO_PREPROC_BASE + 0x090)
#define TRANS_PIP_03 (IO_PREPROC_BASE + 0x290)    
    #define COEFF_01P Fld(12,12,AC_MSKW21)//[23:12]
    #define COEFF_00P Fld(12,0,AC_MSKW10)//[11:0]
#define TRANS_MAIN_04 (IO_PREPROC_BASE + 0x094)
#define TRANS_PIP_04 (IO_PREPROC_BASE + 0x294)
    #define COEFF_10P Fld(12,12,AC_MSKW21)//[23:12]
    #define COEFF_02P Fld(12,0,AC_MSKW10)//[11:0]
#define TRANS_MAIN_05 (IO_PREPROC_BASE + 0x098)
#define TRANS_PIP_05 (IO_PREPROC_BASE + 0x298)    
    #define COEFF_12P Fld(12,12,AC_MSKW21)//[23:12]
    #define COEFF_11P Fld(12,0,AC_MSKW10)//[11:0]
#define TRANS_MAIN_06 (IO_PREPROC_BASE + 0x09C)
#define TRANS_PIP_06 (IO_PREPROC_BASE + 0x29C)
    #define COEFF_21P Fld(12,12,AC_MSKW21)//[23:12]
    #define COEFF_20P Fld(12,0,AC_MSKW10)//[11:0]
#define TRANS_MAIN_07 (IO_PREPROC_BASE + 0x0A0)
#define TRANS_PIP_07 (IO_PREPROC_BASE + 0x2A0)
    #define ERR_DFN Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_INDEX Fld(5,20,AC_MSKW32)//[24:20]
    #define INPUT_SELP Fld(4,16,AC_MSKB2)//[19:16]
    #define BP_VSYNC Fld(1,15,AC_MSKB1)//[15:15]
    #define CONTROL_SEL Fld(3,12,AC_MSKB1)//[14:12]
    #define COEFF_22P Fld(12,0,AC_MSKW10)//[11:0]
#define TRANS_MAIN_08 (IO_PREPROC_BASE + 0x0A4)
#define TRANS_PIP_08 (IO_PREPROC_BASE + 0x2A4)
    #define PRE_TRAN_MAIN_ATPG_CT Fld(1,31,AC_MSKB3)//[31:31]
    #define PRE_TRAN_MAIN_ATPG_OB Fld(1,30,AC_MSKB3)//[30:30]
    #define CONTROL_REG Fld(30,0,AC_MSKDW)//[29:0]
#define STA_3X3_MAIN_00 (IO_PREPROC_BASE + 0x080)
#define STA_3X3_PIP_00 (IO_PREPROC_BASE + 0x280)
    #define STATUS_3X3 Fld(14,0,AC_MSKW10)//[13:0]
    
// Software register
#define SWRW_00 (IO_PREPROC_BASE + 0x580)   // PSCAN OSD debug index
#define SWRW_01 (IO_PREPROC_BASE + 0x584)   // Buffer Address
#define SWRW_02 (IO_PREPROC_BASE + 0x588)   // Buffer size
#define SWRW_03 (IO_PREPROC_BASE + 0x58C)   // Dump format
    #define DUMP_CTRL_HEIGHT Fld(12, 20, AC_MSKW32)  
    #define DUMP_CTRL_WIDTH  Fld(12, 8, AC_MSKW21)       
    #define DUMP_CTRL_FORMAT Fld(8, 0, AC_FULLB0) 
#define SWRW_04 (IO_PREPROC_BASE + 0x590)
    #define DUMP_CTRL_ENG_SEL Fld(6, 26, AC_MSKB3)  // Dump Engine
    #define DUMP_CTRL_STATE   Fld(2, 24, AC_MSKB3)  // Dump State
    #define DUMP_CTRL_SIZE    Fld(23, 0, AC_MSKDW)   // Dump size
#define SWRW_05 (IO_PREPROC_BASE + 0x594)
    #define DUMP_CTRL_SHRINK_W  Fld(16, 16, AC_FULLW32)
    #define DUMP_CTRL_SHRINK_H  Fld(16,  0, AC_FULLW10)
#define SWRW_06 (IO_PREPROC_BASE + 0x598)
    #define DUMP_CTRL_START_X   Fld(16, 16, AC_FULLW32)
    #define DUMP_CTRL_START_Y   Fld(16,  0, AC_FULLW10)     
#define SWRW_07 (IO_PREPROC_BASE + 0x59C)
    #define DUMP_REQ_WIDTH      Fld(16, 16, AC_FULLW32)  
    #define DUMP_REQ_HEIGHT     Fld(16,  0, AC_FULLW10)     

#define SWRW_09 (IO_PREPROC_BASE + 0x5A4)
#define SWRW_0A (IO_PREPROC_BASE + 0x5A8)
#define SWRW_0B (IO_PREPROC_BASE + 0x5AC)
#define SWRW_0C (IO_PREPROC_BASE + 0x5B0)
#define SWRW_0D (IO_PREPROC_BASE + 0x5B4)
#define SWRW_0E (IO_PREPROC_BASE + 0x5B8)
#define SWRW_0F (IO_PREPROC_BASE + 0x5BC)

// LETTERBOX
#define LBOX_00 (IO_PREPROC_BASE + 0x500)
    #define LBOX_START Fld(1,31,AC_MSKB3)//[31:31]
    #define LBOX_STATE Fld(1,30,AC_MSKB3)//[30:30]
    #define FORCE_FCNT Fld(1,29,AC_MSKB3)//[29:29]
    #define LBOX_LOW_BOUND Fld(11,12,AC_MSKW21)//[22:12]
    #define LBOX_UP_BOUND Fld(11,0,AC_MSKW10)//[10:0]
#define LBOX_01 (IO_PREPROC_BASE + 0x504)
    #define Y_THRES Fld(8,24,AC_FULLB3)//[31:24]
    #define LBOX_RIGHT_BOUND Fld(11,12,AC_MSKW21)//[22:12]
    #define LBOX_LEFT_BOUND Fld(11,0,AC_MSKW10)//[10:0]
#define LBOX_02 (IO_PREPROC_BASE + 0x508)
    #define LBOX_MIDBOUND_LOW Fld(11,12,AC_MSKW21)//[22:12]
    #define LBOX_MIDBOUND_UP Fld(11,0,AC_MSKW10)//[10:0]
#define LBOX_03 (IO_PREPROC_BASE + 0x50C)
    #define FCNT_BOUND Fld(10,0,AC_MSKW10)//[9:0]
#define SIDE_LBOX_00 (IO_PREPROC_BASE + 0x510)
    #define C_START Fld(1,31,AC_MSKB3)//[31:31]
    #define C_UP_BOUND Fld(11,12,AC_MSKW21)//[22:12]
    #define C_LOW_BOUND Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_01 (IO_PREPROC_BASE + 0x514)
    #define C_Y_THRES Fld(8,24,AC_FULLB3)//[31:24]
    #define C_LEFT_BOUND Fld(11,12,AC_MSKW21)//[22:12]
    #define C_RIGHT_BOUND Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_02 (IO_PREPROC_BASE + 0x518)
    #define C_TONE1_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE1_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE1_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE1_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_03 (IO_PREPROC_BASE + 0x51C)
    #define C_TONE2_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE2_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE2_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE2_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_04 (IO_PREPROC_BASE + 0x520)
    #define C_TONE3_U_RANGE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_TONE3_U Fld(8,16,AC_FULLB2)//[23:16]
    #define C_TONE3_V_RANGE Fld(8,8,AC_FULLB1)//[15:8]
    #define C_TONE3_V Fld(8,0,AC_FULLB0)//[7:0]
#define SIDE_LBOX_05 (IO_PREPROC_BASE + 0x524)
    #define C_DETECT_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_RIGHT_TONE_SEPERATE Fld(11,16,AC_MSKW32)//[26:16]
    #define C_LEFT_TONE_SEPERATE Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_06 (IO_PREPROC_BASE + 0x528)
    #define C_U_THRES Fld(8,16,AC_FULLB2)//[23:16]
    #define C_V_THRES Fld(8,8,AC_FULLB1)//[15:8]
#define LBOX_STA_00 (IO_PREPROC_BASE + 0x52C)
    #define STA_LBOX_Y_IS_SMALL Fld(1,25,AC_MSKB3)//[25:25]
    #define STA_LOX_STATE_CHANGE Fld(1,24,AC_MSKB3)//[24:24]
    #define STA_LBOX_Y_UP_LINE Fld(11,12,AC_MSKW21)//[22:12]
    #define STA_LBOX_Y_LOW_LINE Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_STA_00 (IO_PREPROC_BASE + 0x530)
    #define STA_SUM_BLACK_LEFT_POS_DIFF Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_SUM_BLACK_RIGHT_POS_DIFF Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_01 (IO_PREPROC_BASE + 0x534)
    #define STA_TOTAL_Y_IS_SMALL Fld(1,31,AC_MSKB3)//[31:31]
    #define STA_BLACK_LEFT_POS_MAX Fld(11,16,AC_MSKW32)//[26:16]
    #define STA_BLACK_RIGHT_POS_MAX Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_STA_02 (IO_PREPROC_BASE + 0x538)
    #define STA_BLACK_LEFT_POS_MIN Fld(11,16,AC_MSKW32)//[26:16]
    #define STA_BLACK_RIGHT_POS_MIN Fld(11,0,AC_MSKW10)//[10:0]
#define SIDE_LBOX_STA_03 (IO_PREPROC_BASE + 0x53C)
    #define STA_U_IN_TONE1_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE1_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_04 (IO_PREPROC_BASE + 0x540)
    #define STA_U_IN_TONE1_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE1_RIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_05 (IO_PREPROC_BASE + 0x544)
    #define STA_U_IN_TONE2_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE2_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_06 (IO_PREPROC_BASE + 0x548)
    #define STA_U_IN_TONE2_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE2_RIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_07 (IO_PREPROC_BASE + 0x54C)
    #define STA_U_IN_TONE3_LEFT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE3_LEFT Fld(16,0,AC_FULLW10)//[15:0]
#define SIDE_LBOX_STA_08 (IO_PREPROC_BASE + 0x550)
    #define STA_U_IN_TONE3_RIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_V_IN_TONE3_RIGHT Fld(16,0,AC_FULLW10)//[15:0]

//===============================================================================
//  5398 Color Related Register
//===============================================================================    
// SCE Config
#define SCE_CFG_MAIN (IO_POSTCOLOR_BASE + 0x000)
    #define COLOR_YHIST_TYPE Fld(2,24,AC_MSKB3)//[25:24]
    #define COLOR_YMAX_LPF_SEL Fld(2,22,AC_MSKB2)//[23:22]
    #define COLOR_YMIN_LPF_SEL Fld(2,20,AC_MSKB2)//[21:20]
    #define LCN_SAT_LIMIT Fld(1,14,AC_MSKB1)//[14:14]
    #define SEQ_SEL Fld(1,13,AC_MSKB1)//[13:13]
    #define BWS_BP Fld(1,12,AC_MSKB1)//[12:12]
    #define ENG_SWAP Fld(1,11,AC_MSKB1)//[11:11]
    #define S_BY_Y_MODE Fld(1,10,AC_MSKB1)//[10:10]
    #define COLOR_YHIST_WGT_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define LCMP_BYPASS Fld(1,8,AC_MSKB1)//[8:8]
    #define ALLBP Fld(1,7,AC_MSKB0)//[7:7]
    #define CCMP_BP Fld(1,6,AC_MSKB0)//[6:6]
    #define ADAP_LUMA_BP Fld(1,5,AC_MSKB0)//[5:5]
    #define HEBP Fld(1,4,AC_MSKB0)//[4:4]
    #define SEBP Fld(1,3,AC_MSKB0)//[3:3]
    #define YEBP Fld(1,2,AC_MSKB0)//[2:2]
    #define P2CBP Fld(1,1,AC_MSKB0)//[1:1]
    #define C2PBP Fld(1,0,AC_MSKB0)//[0:0]

// Pixel/Line Count
#define PXL_CNT_MAIN (IO_POSTCOLOR_BASE + 0x004)
    #define H_CONT Fld(16,0,AC_FULLW10)//[15:0]
#define LINE_CNT_MAIN (IO_POSTCOLOR_BASE + 0x008)
    #define V_CONT Fld(16,16,AC_FULLW32)//[31:16]
    #define LINE_CONT Fld(16,0,AC_FULLW10)//[15:0]    

// Processing Window
#define WIN_X_MAIN (IO_POSTCOLOR_BASE + 0x00C)
    #define WIN_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define WIN_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define WIN_Y_MAIN (IO_POSTCOLOR_BASE + 0x010)
    #define WIN_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define WIN_Y_START Fld(16,0,AC_FULLW10)//[15:0]

// Channel Delay
#define CH_DLY_MAIN (IO_POSTCOLOR_BASE + 0x014)
    #define CR_DEL Fld(3,6,AC_MSKW10)//[8:6]
    #define CB_DEL Fld(3,3,AC_MSKB0)//[5:3]
    #define Y_DEL Fld(3,0,AC_MSKB0)//[2:0]

// Timing detection
#define TIMING_DETECTION_0 (IO_POSTCOLOR_BASE + 0x018)
    #define COLOR_VTOTAL_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HTOTAL_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define TIMING_DETECTION_1 (IO_POSTCOLOR_BASE + 0x01C)
    #define COLOR_VDE_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HDE_CNT Fld(16,0,AC_FULLW10)//[15:0]

// SCE Ink/Capture
//#define DBG_CFG (IO_POSTPROC_BASE + 0x420)
#define DBG_CFG_MAIN (IO_POSTCOLOR_BASE + 0x020)
    #define CR_INK_MODE Fld(3,22,AC_MSKW32)//[24:22]
    #define CB_INK_MODE Fld(3,19,AC_MSKB2)//[21:19]
    #define Y_INK_MODE Fld(3,16,AC_MSKB2)//[18:16]
    #define W3_INK_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define W2_INK_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define W1_INK_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define CR_REPLACE Fld(1,10,AC_MSKB1)//[10:10]
    #define CB_REPLACE Fld(1,9,AC_MSKB1)//[9:9]
    #define Y_REPLACE Fld(1,8,AC_MSKB1)//[8:8]
    #define SPLIT_SWAP Fld(1,4,AC_MSKB0)//[4:4]
    #define SPLIT_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define INK_EN Fld(1,0,AC_MSKB0)//[0:0]
	// add cpature data register address for they can't get the right address define in drv_ycproc_int.c
    #define SCE_CAP_EN Fld(1,2,AC_MSKB0)//[2:2]
#define POS_MAIN (IO_POSTCOLOR_BASE + 0x03C)
    #define POS_Y Fld(16,16,AC_FULLW32)//[31:16]
    #define POS_X Fld(16,0,AC_FULLW10)//[15:0]
#define CAP_IN_DATA_MAIN (IO_POSTCOLOR_BASE + 0x048)
    #define CAP_IN_CB Fld(10,16,AC_MSKW32)//[25:16]
    #define CAP_IN_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_IN_DATA_MAIN_CR (IO_POSTCOLOR_BASE + 0x04C)
    #define CAP_IN_CR Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN (IO_POSTCOLOR_BASE + 0x050)
    #define CAP_OUT_CB Fld(14,16,AC_MSKW32)//[29:16]
    #define CAP_OUT_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN_CR (IO_POSTCOLOR_BASE + 0x054)
    #define CAP_OUT_CR Fld(14,0,AC_MSKW10)//[13:0]

// Chroma Boost
//#define C_BOOST (IO_POSTPROC_BASE + 0x428)
#define C_BOOST_MAIN (IO_POSTCOLOR_BASE + 0x024)
    #define NEW_CBOOST_LMT_U Fld(8,24,AC_FULLB3)//[31:24]
    #define NEW_CBOOST_LMT_L Fld(8,16,AC_FULLB2)//[23:16]
    #define CBOOST_ENABLE Fld(1,15,AC_MSKB1)//[15:15]
    #define MINUS_CBOOST_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define NEW_CBOOST_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define RANGE_SEL Fld(3,8,AC_MSKB1)//[10:8]
    #define CBOOST_GAIN Fld(8,0,AC_FULLB0)//[7:0]
//#define C_BOOST_2 (IO_POSTPROC_BASE + 0x42C)
#define C_BOOST_MAIN_2 (IO_POSTCOLOR_BASE + 0x028)
    #define CBOOST_YOFFSET_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define HUE_HIST_STH Fld(8,8,AC_FULLB1)//[15:8]
    #define CBOOST_YOFFSET Fld(8,0,AC_FULLB0)//[7:0]

// Exceptional Luma Adjustment    
#define LUMA_ADJ (IO_POSTCOLOR_BASE + 0x02C)
    #define Y_SLOPE_LMT Fld(8,8,AC_FULLB1)//[15:8]
    #define Y_LEV_ADJ Fld(8,0,AC_FULLB0)//[7:0]
// General Color
#define G_PIC_ADJ_MAIN_1 (IO_POSTCOLOR_BASE + 0x030)
    #define BRIGHTNESS Fld(11,16,AC_MSKW32)//[26:16]
    #define CONTRAST Fld(10,0,AC_MSKW10)//[9:0]
#define G_PIC_ADJ_MAIN_2 (IO_POSTCOLOR_BASE + 0x034)
    #define HUE Fld(10,16,AC_MSKW32)//[25:16]
    #define SAT Fld(10,0,AC_MSKW10)//[9:0]
#define G_PIC_ADJ_MAIN_3 (IO_POSTCOLOR_BASE + 0x038)
    #define POST_HUE Fld(10,0,AC_MSKW10)//[9:0]

#define INK_DATA_MAIN (IO_POSTCOLOR_BASE + 0x040)
    #define INK_DATA_CB Fld(10,16,AC_MSKW32)//[25:16]
    #define INK_DATA_Y Fld(10,0,AC_MSKW10)//[9:0]
#define INK_DATA_MAIN_CR (IO_POSTCOLOR_BASE + 0x044)
    #define INK_DATA_CR Fld(10,0,AC_MSKW10)//[9:0]    

// Partial Y Function
#define Y_SLOPE_1_0_MAIN (IO_POSTCOLOR_BASE + 0x058)
    #define Y_SLOPE_1 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_0 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_3_2_MAIN (IO_POSTCOLOR_BASE + 0x05C)
    #define Y_SLOPE_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_2 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_5_4_MAIN (IO_POSTCOLOR_BASE + 0x060)
    #define Y_SLOPE_5 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_4 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_7_6_MAIN (IO_POSTCOLOR_BASE + 0x064)
    #define Y_SLOPE_7 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_6 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_9_8_MAIN (IO_POSTCOLOR_BASE + 0x068)
    #define Y_SLOPE_9 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_8 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_11_10_MAIN (IO_POSTCOLOR_BASE + 0x06C)
    #define Y_SLOPE_11 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_10 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_13_12_MAIN (IO_POSTCOLOR_BASE + 0x070)
    #define Y_SLOPE_13 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_12 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_15_14_MAIN (IO_POSTCOLOR_BASE + 0x074)
    #define Y_SLOPE_15 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_14 Fld(8,0,AC_FULLB0)//[7:0]
    
// Partial S Function
#define LOCAL_SAT_1 (IO_POSTCOLOR_BASE + 0x078)
    #define SAT_CPU_SEL_SRAM_0 Fld(1,31,AC_MSKB3)//[31:31]
    #define R_SAT_CPURW_ACTIVE Fld(1,30,AC_MSKB3)//[30:30]
    #define SAT_TABLE_WAIT_VSYNC Fld(1,29,AC_MSKB3)//[29:29]
    #define R_SAT_FILL_TABLE_READY Fld(1,28,AC_MSKB3)//[28:28]
    #define SAT_READ_FINISH Fld(1,27,AC_MSKB3)//[27:27]
    #define SAT_R_READ Fld(1,26,AC_MSKB3)//[26:26]
    #define SAT_WRITE_FINISH Fld(1,25,AC_MSKB3)//[25:25]
    #define R_SAT_WRITE Fld(1,24,AC_MSKB3)//[24:24]
    #define R_SAT_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define R_SAT_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define R_SAT_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define R_SAT_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define R_SAT_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define R_SAT_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define R_SAT_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_2 (IO_POSTCOLOR_BASE + 0x07C)
    #define SAT_CPU_SRAM_ADDR Fld(5,16,AC_MSKB2)//[20:16]
    #define R_SAT_ADDR Fld(5,0,AC_MSKB0)//[4:0]
#define LOCAL_SAT_3 (IO_POSTCOLOR_BASE + 0x080)
    #define SAT_G2_W Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G1_W Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_P2_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_P1_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_4 (IO_POSTCOLOR_BASE + 0x084)
    #define SAT_G_Y128_W Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G_Y64_W Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_G_Y0_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G3_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_5 (IO_POSTCOLOR_BASE + 0x088)
    #define LCMP_M11_W Fld(10,16,AC_MSKW32)//[25:16]
    #define SAT_G_Y256_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G_Y192_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_6 (IO_POSTCOLOR_BASE + 0x08C)
    #define LCMP_M21_W Fld(10,20,AC_MSKW32)//[29:20]
    #define LCMP_M13_W Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M12_W Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_7 (IO_POSTCOLOR_BASE + 0x090)
    #define LCMP_M23_W Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M22_W Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_8 (IO_POSTCOLOR_BASE + 0x094)
    #define SAT_G2_R Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G1_R Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_P2_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_P1_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_9 (IO_POSTCOLOR_BASE + 0x098)
    #define SAT_G_Y128_R Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G_Y64_R Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_G_Y0_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G3_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_10 (IO_POSTCOLOR_BASE + 0x09C)
    #define LCMP_M11_R Fld(10,16,AC_MSKW32)//[25:16]
    #define SAT_G_Y256_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G_Y192_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_11 (IO_POSTCOLOR_BASE + 0x0A0)
    #define LCMP_M21_R Fld(10,20,AC_MSKW32)//[29:20]
    #define LCMP_M13_R Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M12_R Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_12 (IO_POSTCOLOR_BASE + 0x0A4)
    #define LCMP_M23_R Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M22_R Fld(10,0,AC_MSKW10)//[9:0]

// CCMP
#define CCMP_GROUP_1 (IO_POSTCOLOR_BASE + 0x0A8)
    #define CCMP_Y96 Fld(8,24,AC_FULLB3)//[31:24]
    #define CCMP_Y64 Fld(8,16,AC_FULLB2)//[23:16]
    #define CCMP_Y32 Fld(8,8,AC_FULLB1)//[15:8]
    #define CCMP_Y0 Fld(8,0,AC_FULLB0)//[7:0]
#define CCMP_GROUP_2 (IO_POSTCOLOR_BASE + 0x0AC)
    #define CCMP_Y224 Fld(8,24,AC_FULLB3)//[31:24]
    #define CCMP_Y192 Fld(8,16,AC_FULLB2)//[23:16]
    #define CCMP_Y160 Fld(8,8,AC_FULLB1)//[15:8]
    #define CCMP_Y128 Fld(8,0,AC_FULLB0)//[7:0]
#define CCMP_GROUP_3 (IO_POSTCOLOR_BASE + 0x0B0)
    #define CCMP_Y256 Fld(8,0,AC_FULLB0)//[7:0] 

// 2D Window
#define TWO_D_WINDOW_1 (IO_POSTCOLOR_BASE + 0x0B4)
    #define W1_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W1_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W1_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W1_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_WINDOW_2 (IO_POSTCOLOR_BASE + 0x0B8)
    #define W2_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W2_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W2_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W2_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_WINDOW_3 (IO_POSTCOLOR_BASE + 0x0BC)
    #define W3_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W3_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W3_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W3_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_W1_RESULT (IO_POSTCOLOR_BASE + 0x0C0)
    #define W1_RESULT Fld(23,0,AC_MSKDW)//[22:0]
#define TWO_D_W2_RESULT (IO_POSTCOLOR_BASE + 0x0C4)
    #define W2_RESULT Fld(23,0,AC_MSKDW)//[22:0]
#define TWO_D_W3_RESULT (IO_POSTCOLOR_BASE + 0x0C8)
    #define W3_RESULT Fld(23,0,AC_MSKDW)//[22:0]   
    
// Luma/Saturation Sum
#define LUMA_SUM_MAIN (IO_POSTCOLOR_BASE + 0x0CC)
    #define SCE_LUMA_SUM Fld(32,0,AC_FULLDW)//[31:0]
#define LUMA_MIN_MAX_MAIN (IO_POSTCOLOR_BASE + 0x0D0)
    #define SCE_LUMA_MAX Fld(10,16,AC_MSKW32)//[25:16]
    #define SCE_LUMA_MIN Fld(10,0,AC_MSKW10)//[9:0]
#define SAT_SUM_MAIN (IO_POSTCOLOR_BASE + 0x0D4)
    #define SCE_SAT_SUM Fld(32,0,AC_FULLDW)//[31:0]

// Partial H Function
#define LOCAL_HUE_1 (IO_POSTCOLOR_BASE + 0x0D8)
    #define HUE_CPU_SEL_SRAM_0 Fld(1,31,AC_MSKB3)//[31:31]
    #define R_HUE_CPURW_ACTIVE Fld(1,30,AC_MSKB3)//[30:30]
    #define HUE_TABLE_WAIT_VSYNC Fld(1,29,AC_MSKB3)//[29:29]
    #define R_HUE_FILL_TABLE_READY Fld(1,28,AC_MSKB3)//[28:28]
    #define HUE_READ_FINISH Fld(1,27,AC_MSKB3)//[27:27]
    #define HUE_R_READ Fld(1,26,AC_MSKB3)//[26:26]
    #define HUE_WRITE_FINISH Fld(1,25,AC_MSKB3)//[25:25]
    #define R_HUE_WRITE Fld(1,24,AC_MSKB3)//[24:24]
    #define R_HUE_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define R_HUE_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define R_HUE_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define R_HUE_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define R_HUE_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define R_HUE_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define R_HUE_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_2 (IO_POSTCOLOR_BASE + 0x0DC)
    #define HUE_CPU_SRAM_ADDR Fld(5,16,AC_MSKB2)//[20:16]
    #define R_HUE_ADDR Fld(5,0,AC_MSKB0)//[4:0]
#define LOCAL_HUE_3 (IO_POSTCOLOR_BASE + 0x0E0)
    #define Y128_TO_HUE_W Fld(8,24,AC_FULLB3)//[31:24]
    #define Y64_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define Y0_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_4 (IO_POSTCOLOR_BASE + 0x0E4)
    #define S32_TO_HUE_W Fld(8,24,AC_FULLB3)//[31:24]
    #define S0_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define Y256_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define Y192_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_5 (IO_POSTCOLOR_BASE + 0x0E8)
    #define S128_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define S96_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define S64_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_6 (IO_POSTCOLOR_BASE + 0x0EC)
    #define Y128_TO_HUE_R Fld(8,24,AC_FULLB3)//[31:24]
    #define Y64_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define Y0_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_7 (IO_POSTCOLOR_BASE + 0x0F0)
    #define S32_TO_HUE_R Fld(8,24,AC_FULLB3)//[31:24]
    #define S0_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define Y256_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define Y192_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_8 (IO_POSTCOLOR_BASE + 0x0F4)
    #define S128_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define S96_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define S64_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]

// Saturation Histogram
#define SAT_HIST_CFG_MAIN (IO_POSTCOLOR_BASE + 0x0F8)
    #define SAT_BOUND_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_BOUND_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_BOUND_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_BOUND_1 Fld(8,0,AC_FULLB0)//[7:0]
#define SAT_HIST_CFG_MAIN_2 (IO_POSTCOLOR_BASE + 0x0FC)
    #define SAT_BOUND_7 Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_BOUND_6 Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_BOUND_5 Fld(8,0,AC_FULLB0)//[7:0]
#define SAT_HIST_X_CFG_MAIN (IO_POSTCOLOR_BASE + 0x100)
    #define SAT_WIN_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_WIN_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_Y_CFG_MAIN (IO_POSTCOLOR_BASE + 0x104)
    #define SAT_WIN_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_WIN_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_1_0_MAIN (IO_POSTCOLOR_BASE + 0x108)
    #define SAT_HIST_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_0 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_3_2_MAIN (IO_POSTCOLOR_BASE + 0x10C)
    #define SAT_HIST_3 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_2 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_5_4_MAIN (IO_POSTCOLOR_BASE + 0x110)
    #define SAT_HIST_5 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_4 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_7_6_MAIN (IO_POSTCOLOR_BASE + 0x114)
    #define SAT_HIST_7 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_6 Fld(16,0,AC_FULLW10)//[15:0]    
#define SAT_HIST_CFG_MAIN_3 (IO_POSTCOLOR_BASE + 0x5E0)
    #define COLOR_HIST_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HIST_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_CFG_MAIN_4 (IO_POSTCOLOR_BASE + 0x5E4)
    #define COLOR_HIST_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HIST_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_CFG_MAIN_5 (IO_POSTCOLOR_BASE + 0x5E8)
	#define COLOR_HIST_WIN_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define SAT_HIST_HMIN Fld(13,16,AC_MSKW32)//[28:16]
    #define SAT_HIST_HMAX Fld(13,0,AC_MSKW10)//[12:0]    
// Black/White Stretch
#define BWS_1 (IO_POSTCOLOR_BASE + 0x118)
    #define B_SLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define B_CUTOFF Fld(10,0,AC_MSKW10)//[9:0]
#define BWS_2 (IO_POSTCOLOR_BASE + 0x11C)
    #define W_SLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define W_CUTOFF Fld(10,0,AC_MSKW10)//[9:0]

// Hue Histogram
#define HUE_HIST_CFG_MAIN (IO_POSTCOLOR_BASE + 0x120)
    #define HUE_BOUND_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define HUE_BOUND_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define HUE_BOUND_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_BOUND_0 Fld(8,0,AC_FULLB0)//[7:0]
#define HUE_HIST_CFG_MAIN_1 (IO_POSTCOLOR_BASE + 0x124)
    #define HUE_BOUND_7 Fld(8,24,AC_FULLB3)//[31:24]
    #define HUE_BOUND_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define HUE_BOUND_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_BOUND_4 Fld(8,0,AC_FULLB0)//[7:0]
#define HUE_HIST_1_0_MAIN (IO_POSTCOLOR_BASE + 0x128)
    #define HUE_HIST_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_0 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_3_2_MAIN (IO_POSTCOLOR_BASE + 0x12C)
    #define HUE_HIST_3 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_2 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_5_4_MAIN (IO_POSTCOLOR_BASE + 0x130)
    #define HUE_HIST_5 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_4 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_7_6_MAIN (IO_POSTCOLOR_BASE + 0x134)
    #define HUE_HIST_7 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_6 Fld(16,0,AC_FULLW10)//[15:0]

// Y BitPlus
#define BITPLUS_1 (IO_POSTCOLOR_BASE + 0x138)
    #define Y_BITPLUS_Q4 Fld(6,26,AC_MSKB3)//[31:26]
    #define Y_BITPLUS_Q3 Fld(6,20,AC_MSKW32)//[25:20]
    #define Y_BITPLUS_Q2 Fld(6,14,AC_MSKW21)//[19:14]
    #define Y_BITPLUS_Q1 Fld(6,8,AC_MSKB1)//[13:8]
    #define Y_BITPLUS_YRANGE Fld(6,0,AC_MSKB0)//[5:0]
#define BITPLUS_2 (IO_POSTCOLOR_BASE + 0x13C)
    #define Y_BITPLUS_INK_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_BITPLUS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_BITPLUS_SDIFF_SHIFT Fld(3,27,AC_MSKB3)//[29:27]    
    #define Y_BITPLUS_P3 Fld(9,18,AC_MSKW32)//[26:18]
    #define Y_BITPLUS_P2 Fld(9,9,AC_MSKW21)//[17:9]
    #define Y_BITPLUS_P1 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_3 (IO_POSTCOLOR_BASE + 0x140)
    #define Y_BITPLUS_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_BITPLUS_RANDOM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_BITPLUS_SHIFT Fld(2,28,AC_MSKB3)//[29:28]
    #define Y_BITPLUS_G34 Fld(6,21,AC_MSKW32)//[26:21]
    #define Y_BITPLUS_G23 Fld(6,15,AC_MSKW21)//[20:15]
    #define Y_BITPLUS_G12 Fld(6,9,AC_MSKB1)//[14:9]
    #define Y_BITPLUS_P4 Fld(9,0,AC_MSKW10)//[8:0]

// S BitPlus
#define BITPLUS_4 (IO_POSTCOLOR_BASE + 0x144)
    #define S_BITPLUS_Q4 Fld(6,26,AC_MSKB3)//[31:26]
    #define S_BITPLUS_Q3 Fld(6,20,AC_MSKW32)//[25:20]
    #define S_BITPLUS_Q2 Fld(6,14,AC_MSKW21)//[19:14]
    #define S_BITPLUS_Q1 Fld(6,8,AC_MSKB1)//[13:8]
    #define S_BITPLUS_SRANGE Fld(6,0,AC_MSKB0)//[5:0]
#define BITPLUS_5 (IO_POSTCOLOR_BASE + 0x148)
    #define S_BITPLUS_INK_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define S_BITPLUS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define S_BITPLUS_SDIFF_SHIFT Fld(3,27,AC_MSKB3)//[29:27]    
    #define S_BITPLUS_P3 Fld(9,18,AC_MSKW32)//[26:18]
    #define S_BITPLUS_P2 Fld(9,9,AC_MSKW21)//[17:9]
    #define S_BITPLUS_P1 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_6 (IO_POSTCOLOR_BASE + 0x14C)
    #define S_BITPLUS_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define S_BITPLUS_RANDOM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define S_BITPLUS_SHIFT Fld(2,28,AC_MSKB3)//[29:28]
    #define S_BITPLUS_G34 Fld(6,21,AC_MSKW32)//[26:21]
    #define S_BITPLUS_G23 Fld(6,15,AC_MSKW21)//[20:15]
    #define S_BITPLUS_G12 Fld(6,9,AC_MSKB1)//[14:9]
    #define S_BITPLUS_P4 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_7 (IO_POSTCOLOR_BASE + 0x150)
    #define S_BITPLUS_HRANGE Fld(6,0,AC_MSKB0)//[5:0]
    
// AL Curve
#define Y_FTN_1_0_MAIN (IO_POSTCOLOR_BASE + 0x200)
    #define Y_FTN_1 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_0 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_3_2_MAIN (IO_POSTCOLOR_BASE + 0x204)
    #define Y_FTN_3 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_2 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_5_4_MAIN (IO_POSTCOLOR_BASE + 0x208)
    #define Y_FTN_5 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_4 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_7_6_MAIN (IO_POSTCOLOR_BASE + 0x20C)
    #define Y_FTN_7 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_6 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_9_8_MAIN (IO_POSTCOLOR_BASE + 0x210)
    #define Y_FTN_9 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_8 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_11_10_MAIN (IO_POSTCOLOR_BASE + 0x214)
    #define Y_FTN_11 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_10 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_13_12_MAIN (IO_POSTCOLOR_BASE + 0x218)
    #define Y_FTN_13 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_12 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_15_14_MAIN (IO_POSTCOLOR_BASE + 0x21C)
    #define Y_FTN_15 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_14 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_17_16_MAIN (IO_POSTCOLOR_BASE + 0x220)
    #define Y_FTN_17 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_16 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_19_18_MAIN (IO_POSTCOLOR_BASE + 0x224)
    #define Y_FTN_19 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_18 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_21_20_MAIN (IO_POSTCOLOR_BASE + 0x228)
    #define Y_FTN_21 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_20 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_23_22_MAIN (IO_POSTCOLOR_BASE + 0x22C)
    #define Y_FTN_23 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_22 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_25_24_MAIN (IO_POSTCOLOR_BASE + 0x230)
    #define Y_FTN_25 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_24 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_27_26_MAIN (IO_POSTCOLOR_BASE + 0x234)
    #define Y_FTN_27 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_26 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_29_28_MAIN (IO_POSTCOLOR_BASE + 0x238)
    #define Y_FTN_29 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_28 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_31_30_MAIN (IO_POSTCOLOR_BASE + 0x23C)
    #define Y_FTN_31 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_30 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_33_32_MAIN (IO_POSTCOLOR_BASE + 0x240)
    #define Y_FTN_33 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_32 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_35_34_MAIN (IO_POSTCOLOR_BASE + 0x244)
    #define Y_FTN_35 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_34 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_37_36_MAIN (IO_POSTCOLOR_BASE + 0x248)
    #define Y_FTN_37 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_36 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_39_38_MAIN (IO_POSTCOLOR_BASE + 0x24C)
    #define Y_FTN_39 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_38 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_41_40_MAIN (IO_POSTCOLOR_BASE + 0x250)
    #define Y_FTN_41 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_40 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_43_42_MAIN (IO_POSTCOLOR_BASE + 0x254)
    #define Y_FTN_43 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_42 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_45_44_MAIN (IO_POSTCOLOR_BASE + 0x258)
    #define Y_FTN_45 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_44 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_47_46_MAIN (IO_POSTCOLOR_BASE + 0x25C)
    #define Y_FTN_47 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_46 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_49_48_MAIN (IO_POSTCOLOR_BASE + 0x260)
    #define Y_FTN_49 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_48 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_51_50_MAIN (IO_POSTCOLOR_BASE + 0x264)
    #define Y_FTN_51 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_50 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_53_52_MAIN (IO_POSTCOLOR_BASE + 0x268)
    #define Y_FTN_53 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_52 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_55_54_MAIN (IO_POSTCOLOR_BASE + 0x26C)
    #define Y_FTN_55 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_54 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_57_56_MAIN (IO_POSTCOLOR_BASE + 0x270)
    #define Y_FTN_57 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_56 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_59_58_MAIN (IO_POSTCOLOR_BASE + 0x274)
    #define Y_FTN_59 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_58 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_61_60_MAIN (IO_POSTCOLOR_BASE + 0x278)
    #define Y_FTN_61 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_60 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_63_62_MAIN (IO_POSTCOLOR_BASE + 0x27C)
    #define Y_FTN_63 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_62 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_64_MAIN (IO_POSTCOLOR_BASE + 0x280)
    #define Y_FTN_64 Fld(10,0,AC_MSKW10)//[9:0]

// AL Curve 2
#define Y_FTN2_1_0_MAIN (IO_POSTCOLOR_BASE + 0x284)
    #define Y_FTN2_1 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_0 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_3_2_MAIN (IO_POSTCOLOR_BASE + 0x288)
    #define Y_FTN2_3 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_2 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_5_4_MAIN (IO_POSTCOLOR_BASE + 0x28C)
    #define Y_FTN2_5 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_4 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_7_6_MAIN (IO_POSTCOLOR_BASE + 0x290)
    #define Y_FTN2_7 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_6 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_9_8_MAIN (IO_POSTCOLOR_BASE + 0x294)
    #define Y_FTN2_9 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_8 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_11_10_MAIN (IO_POSTCOLOR_BASE + 0x298)
    #define Y_FTN2_11 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_10 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_13_12_MAIN (IO_POSTCOLOR_BASE + 0x29C)
    #define Y_FTN2_13 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_12 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_15_14_MAIN (IO_POSTCOLOR_BASE + 0x2A0)
    #define Y_FTN2_15 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_14 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_17_16_MAIN (IO_POSTCOLOR_BASE + 0x2A4)
    #define Y_FTN2_17 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_16 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_19_18_MAIN (IO_POSTCOLOR_BASE + 0x2A8)
    #define Y_FTN2_19 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_18 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_21_20_MAIN (IO_POSTCOLOR_BASE + 0x2AC)
    #define Y_FTN2_21 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_20 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_23_22_MAIN (IO_POSTCOLOR_BASE + 0x2B0)
    #define Y_FTN2_23 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_22 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_25_24_MAIN (IO_POSTCOLOR_BASE + 0x2B4)
    #define Y_FTN2_25 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_24 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_27_26_MAIN (IO_POSTCOLOR_BASE + 0x2B8)
    #define Y_FTN2_27 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_26 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_29_28_MAIN (IO_POSTCOLOR_BASE + 0x2BC)
    #define Y_FTN2_29 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_28 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_31_30_MAIN (IO_POSTCOLOR_BASE + 0x2C0)
    #define Y_FTN2_31 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_30 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_33_32_MAIN (IO_POSTCOLOR_BASE + 0x2C4)
    #define Y_FTN2_33 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_32 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_35_34_MAIN (IO_POSTCOLOR_BASE + 0x2C8)
    #define Y_FTN2_35 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_34 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_37_36_MAIN (IO_POSTCOLOR_BASE + 0x2CC)
    #define Y_FTN2_37 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_36 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_39_38_MAIN (IO_POSTCOLOR_BASE + 0x2D0)
    #define Y_FTN2_39 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_38 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_41_40_MAIN (IO_POSTCOLOR_BASE + 0x2D4)
    #define Y_FTN2_41 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_40 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_43_42_MAIN (IO_POSTCOLOR_BASE + 0x2D8)
    #define Y_FTN2_43 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_42 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_45_44_MAIN (IO_POSTCOLOR_BASE + 0x2DC)
    #define Y_FTN2_45 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_44 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_47_46_MAIN (IO_POSTCOLOR_BASE + 0x2E0)
    #define Y_FTN2_47 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_46 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_49_48_MAIN (IO_POSTCOLOR_BASE + 0x2E4)
    #define Y_FTN2_49 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_48 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_51_50_MAIN (IO_POSTCOLOR_BASE + 0x2E8)
    #define Y_FTN2_51 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_50 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_53_52_MAIN (IO_POSTCOLOR_BASE + 0x2EC)
    #define Y_FTN2_53 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_52 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_55_54_MAIN (IO_POSTCOLOR_BASE + 0x2F0)
    #define Y_FTN2_55 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_54 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_57_56_MAIN (IO_POSTCOLOR_BASE + 0x2F4)
    #define Y_FTN2_57 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_56 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_59_58_MAIN (IO_POSTCOLOR_BASE + 0x2F8)
    #define Y_FTN2_59 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_58 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_61_60_MAIN (IO_POSTCOLOR_BASE + 0x2FC)
    #define Y_FTN2_61 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_60 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_63_62_MAIN (IO_POSTCOLOR_BASE + 0x300)
    #define Y_FTN2_63 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_62 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_64_MAIN (IO_POSTCOLOR_BASE + 0x304)
    #define Y_FTN2_64 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_1 (IO_POSTCOLOR_BASE + 0x308)
    #define Y_FTN2_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_FTN2_INK_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_FTN2_YSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_YMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_YMIN Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_2 (IO_POSTCOLOR_BASE + 0x30C)
    #define Y_FTN2_SSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_SMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_SMIN Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_3 (IO_POSTCOLOR_BASE + 0x310)
    #define Y_FTN2_HSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_HMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_HMIN Fld(10,0,AC_MSKW10)//[9:0]
    
// Luma Histogram
#define LUMA_HIST_00 (IO_POSTCOLOR_BASE + 0x400)
    #define LUMA_HIST_PCNT_00 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_01 (IO_POSTCOLOR_BASE + 0x404)
    #define LUMA_HIST_PCNT_01 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_02 (IO_POSTCOLOR_BASE + 0x408)
    #define LUMA_HIST_PCNT_02 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_03 (IO_POSTCOLOR_BASE + 0x40C)
    #define LUMA_HIST_PCNT_03 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_04 (IO_POSTCOLOR_BASE + 0x410)
    #define LUMA_HIST_PCNT_04 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_05 (IO_POSTCOLOR_BASE + 0x414)
    #define LUMA_HIST_PCNT_05 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_06 (IO_POSTCOLOR_BASE + 0x418)
    #define LUMA_HIST_PCNT_06 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_07 (IO_POSTCOLOR_BASE + 0x41C)
    #define LUMA_HIST_PCNT_07 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_08 (IO_POSTCOLOR_BASE + 0x420)
    #define LUMA_HIST_PCNT_08 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_09 (IO_POSTCOLOR_BASE + 0x424)
    #define LUMA_HIST_PCNT_09 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_10 (IO_POSTCOLOR_BASE + 0x428)
    #define LUMA_HIST_PCNT_10 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_11 (IO_POSTCOLOR_BASE + 0x42C)
    #define LUMA_HIST_PCNT_11 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_12 (IO_POSTCOLOR_BASE + 0x430)
    #define LUMA_HIST_PCNT_12 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_13 (IO_POSTCOLOR_BASE + 0x434)
    #define LUMA_HIST_PCNT_13 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_14 (IO_POSTCOLOR_BASE + 0x438)
    #define LUMA_HIST_PCNT_14 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_15 (IO_POSTCOLOR_BASE + 0x43C)
    #define LUMA_HIST_PCNT_15 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_16 (IO_POSTCOLOR_BASE + 0x440)
    #define LUMA_HIST_PCNT_16 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_17 (IO_POSTCOLOR_BASE + 0x444)
    #define LUMA_HIST_PCNT_17 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_18 (IO_POSTCOLOR_BASE + 0x448)
    #define LUMA_HIST_PCNT_18 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_19 (IO_POSTCOLOR_BASE + 0x44C)
    #define LUMA_HIST_PCNT_19 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_20 (IO_POSTCOLOR_BASE + 0x450)
    #define LUMA_HIST_PCNT_20 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_21 (IO_POSTCOLOR_BASE + 0x454)
    #define LUMA_HIST_PCNT_21 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_22 (IO_POSTCOLOR_BASE + 0x458)
    #define LUMA_HIST_PCNT_22 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_23 (IO_POSTCOLOR_BASE + 0x45C)
    #define LUMA_HIST_PCNT_23 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_24 (IO_POSTCOLOR_BASE + 0x460)
    #define LUMA_HIST_PCNT_24 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_25 (IO_POSTCOLOR_BASE + 0x464)
    #define LUMA_HIST_PCNT_25 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_26 (IO_POSTCOLOR_BASE + 0x468)
    #define LUMA_HIST_PCNT_26 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_27 (IO_POSTCOLOR_BASE + 0x46C)
    #define LUMA_HIST_PCNT_27 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_28 (IO_POSTCOLOR_BASE + 0x470)
    #define LUMA_HIST_PCNT_28 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_29 (IO_POSTCOLOR_BASE + 0x474)
    #define LUMA_HIST_PCNT_29 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_30 (IO_POSTCOLOR_BASE + 0x478)
    #define LUMA_HIST_PCNT_30 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_31 (IO_POSTCOLOR_BASE + 0x47C)
    #define LUMA_HIST_PCNT_31 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_32 (IO_POSTCOLOR_BASE + 0x480)
    #define LUMA_HIST_PCNT_32 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_33 (IO_POSTCOLOR_BASE + 0x484)
    #define LUMA_HIST_PCNT_33 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_34 (IO_POSTCOLOR_BASE + 0x488)
    #define LUMA_HIST_PCNT_34 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_35 (IO_POSTCOLOR_BASE + 0x48C)
    #define LUMA_HIST_PCNT_35 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_36 (IO_POSTCOLOR_BASE + 0x490)
    #define LUMA_HIST_PCNT_36 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_37 (IO_POSTCOLOR_BASE + 0x494)
    #define LUMA_HIST_PCNT_37 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_38 (IO_POSTCOLOR_BASE + 0x498)
    #define LUMA_HIST_PCNT_38 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_39 (IO_POSTCOLOR_BASE + 0x49C)
    #define LUMA_HIST_PCNT_39 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_40 (IO_POSTCOLOR_BASE + 0x4A0)
    #define LUMA_HIST_PCNT_40 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_41 (IO_POSTCOLOR_BASE + 0x4A4)
    #define LUMA_HIST_PCNT_41 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_42 (IO_POSTCOLOR_BASE + 0x4A8)
    #define LUMA_HIST_PCNT_42 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_43 (IO_POSTCOLOR_BASE + 0x4AC)
    #define LUMA_HIST_PCNT_43 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_44 (IO_POSTCOLOR_BASE + 0x4B0)
    #define LUMA_HIST_PCNT_44 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_45 (IO_POSTCOLOR_BASE + 0x4B4)
    #define LUMA_HIST_PCNT_45 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_46 (IO_POSTCOLOR_BASE + 0x4B8)
    #define LUMA_HIST_PCNT_46 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_47 (IO_POSTCOLOR_BASE + 0x4BC)
    #define LUMA_HIST_PCNT_47 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_48 (IO_POSTCOLOR_BASE + 0x4C0)
    #define LUMA_HIST_PCNT_48 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_49 (IO_POSTCOLOR_BASE + 0x4C4)
    #define LUMA_HIST_PCNT_49 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_50 (IO_POSTCOLOR_BASE + 0x4C8)
    #define LUMA_HIST_PCNT_50 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_51 (IO_POSTCOLOR_BASE + 0x4CC)
    #define LUMA_HIST_PCNT_51 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_52 (IO_POSTCOLOR_BASE + 0x4D0)
    #define LUMA_HIST_PCNT_52 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_53 (IO_POSTCOLOR_BASE + 0x4D4)
    #define LUMA_HIST_PCNT_53 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_54 (IO_POSTCOLOR_BASE + 0x4D8)
    #define LUMA_HIST_PCNT_54 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_55 (IO_POSTCOLOR_BASE + 0x4DC)
    #define LUMA_HIST_PCNT_55 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_56 (IO_POSTCOLOR_BASE + 0x4E0)
    #define LUMA_HIST_PCNT_56 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_57 (IO_POSTCOLOR_BASE + 0x4E4)
    #define LUMA_HIST_PCNT_57 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_58 (IO_POSTCOLOR_BASE + 0x4E8)
    #define LUMA_HIST_PCNT_58 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_59 (IO_POSTCOLOR_BASE + 0x4EC)
    #define LUMA_HIST_PCNT_59 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_60 (IO_POSTCOLOR_BASE + 0x4F0)
    #define LUMA_HIST_PCNT_60 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_61 (IO_POSTCOLOR_BASE + 0x4F4)
    #define LUMA_HIST_PCNT_61 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_62 (IO_POSTCOLOR_BASE + 0x4F8)
    #define LUMA_HIST_PCNT_62 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_63 (IO_POSTCOLOR_BASE + 0x4FC)
    #define LUMA_HIST_PCNT_63 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_64 (IO_POSTCOLOR_BASE + 0x500)
    #define LUMA_HIST_PCNT_64 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_65 (IO_POSTCOLOR_BASE + 0x504)
    #define LUMA_HIST_PCNT_65 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_66 (IO_POSTCOLOR_BASE + 0x508)
    #define LUMA_HIST_PCNT_66 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_67 (IO_POSTCOLOR_BASE + 0x50C)
    #define LUMA_HIST_PCNT_67 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_68 (IO_POSTCOLOR_BASE + 0x510)
    #define LUMA_HIST_PCNT_68 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_69 (IO_POSTCOLOR_BASE + 0x514)
    #define LUMA_HIST_PCNT_69 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_70 (IO_POSTCOLOR_BASE + 0x518)
    #define LUMA_HIST_PCNT_70 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_71 (IO_POSTCOLOR_BASE + 0x51C)
    #define LUMA_HIST_PCNT_71 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_72 (IO_POSTCOLOR_BASE + 0x520)
    #define LUMA_HIST_PCNT_72 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_73 (IO_POSTCOLOR_BASE + 0x524)
    #define LUMA_HIST_PCNT_73 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_74 (IO_POSTCOLOR_BASE + 0x528)
    #define LUMA_HIST_PCNT_74 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_75 (IO_POSTCOLOR_BASE + 0x52C)
    #define LUMA_HIST_PCNT_75 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_76 (IO_POSTCOLOR_BASE + 0x530)
    #define LUMA_HIST_PCNT_76 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_77 (IO_POSTCOLOR_BASE + 0x534)
    #define LUMA_HIST_PCNT_77 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_78 (IO_POSTCOLOR_BASE + 0x538)
    #define LUMA_HIST_PCNT_78 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_79 (IO_POSTCOLOR_BASE + 0x53C)
    #define LUMA_HIST_PCNT_79 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_80 (IO_POSTCOLOR_BASE + 0x540)
    #define LUMA_HIST_PCNT_80 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_81 (IO_POSTCOLOR_BASE + 0x544)
    #define LUMA_HIST_PCNT_81 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_82 (IO_POSTCOLOR_BASE + 0x548)
    #define LUMA_HIST_PCNT_82 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_83 (IO_POSTCOLOR_BASE + 0x54C)
    #define LUMA_HIST_PCNT_83 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_84 (IO_POSTCOLOR_BASE + 0x550)
    #define LUMA_HIST_PCNT_84 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_85 (IO_POSTCOLOR_BASE + 0x554)
    #define LUMA_HIST_PCNT_85 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_86 (IO_POSTCOLOR_BASE + 0x558)
    #define LUMA_HIST_PCNT_86 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_87 (IO_POSTCOLOR_BASE + 0x55C)
    #define LUMA_HIST_PCNT_87 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_88 (IO_POSTCOLOR_BASE + 0x560)
    #define LUMA_HIST_PCNT_88 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_89 (IO_POSTCOLOR_BASE + 0x564)
    #define LUMA_HIST_PCNT_89 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_90 (IO_POSTCOLOR_BASE + 0x568)
    #define LUMA_HIST_PCNT_90 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_91 (IO_POSTCOLOR_BASE + 0x56C)
    #define LUMA_HIST_PCNT_91 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_92 (IO_POSTCOLOR_BASE + 0x570)
    #define LUMA_HIST_PCNT_92 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_93 (IO_POSTCOLOR_BASE + 0x574)
    #define LUMA_HIST_PCNT_93 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_94 (IO_POSTCOLOR_BASE + 0x578)
    #define LUMA_HIST_PCNT_94 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_95 (IO_POSTCOLOR_BASE + 0x57C)
    #define LUMA_HIST_PCNT_95 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_96 (IO_POSTCOLOR_BASE + 0x580)
    #define LUMA_HIST_PCNT_96 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_97 (IO_POSTCOLOR_BASE + 0x584)
    #define LUMA_HIST_PCNT_97 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_98 (IO_POSTCOLOR_BASE + 0x588)
    #define LUMA_HIST_PCNT_98 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_99 (IO_POSTCOLOR_BASE + 0x58C)
    #define LUMA_HIST_PCNT_99 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_100 (IO_POSTCOLOR_BASE + 0x590)
    #define LUMA_HIST_PCNT_100 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_101 (IO_POSTCOLOR_BASE + 0x594)
    #define LUMA_HIST_PCNT_101 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_102 (IO_POSTCOLOR_BASE + 0x598)
    #define LUMA_HIST_PCNT_102 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_103 (IO_POSTCOLOR_BASE + 0x59C)
    #define LUMA_HIST_PCNT_103 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_104 (IO_POSTCOLOR_BASE + 0x5A0)
    #define LUMA_HIST_PCNT_104 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_105 (IO_POSTCOLOR_BASE + 0x5A4)
    #define LUMA_HIST_PCNT_105 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_106 (IO_POSTCOLOR_BASE + 0x5A8)
    #define LUMA_HIST_PCNT_106 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_107 (IO_POSTCOLOR_BASE + 0x5AC)
    #define LUMA_HIST_PCNT_107 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_108 (IO_POSTCOLOR_BASE + 0x5B0)
    #define LUMA_HIST_PCNT_108 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_109 (IO_POSTCOLOR_BASE + 0x5B4)
    #define LUMA_HIST_PCNT_109 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_110 (IO_POSTCOLOR_BASE + 0x5B8)
    #define LUMA_HIST_PCNT_110 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_111 (IO_POSTCOLOR_BASE + 0x5BC)
    #define LUMA_HIST_PCNT_111 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_112 (IO_POSTCOLOR_BASE + 0x5C0)
    #define LUMA_HIST_PCNT_112 Fld(23,0,AC_MSKDW)//[22:0]    

// CRC
#define CRC_0 (IO_POSTCOLOR_BASE + 0x5C4)
    #define POST_CRC_SRC_SEL Fld(4,16,AC_MSKB2)//[19:16]
    #define CRC_STILL_CHECK_MAX Fld(8,8,AC_FULLB1)//[15:8]
    #define CRC_NON_STILL_CNT Fld(4,4,AC_MSKB0)//[7:4]
    #define CRC_STILL_CHECK_DONE Fld(1,3,AC_MSKB0)//[3:3]
    #define C_CRC_STILL_CHECK_TRIG Fld(1,2,AC_MSKB0)//[2:2]
    #define CRC_L_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define CRC_R_EN Fld(1,0,AC_MSKB0)//[0:0]
#define CRC_1 (IO_POSTCOLOR_BASE + 0x5C8)
    #define CRC_CLIP_H_END Fld(13,16,AC_MSKW32)//[28:16]
    #define CRC_CLIP_H_START Fld(13,0,AC_MSKW10)//[12:0]
#define CRC_2 (IO_POSTCOLOR_BASE + 0x5CC)
    #define CRC_CLIP_V_END Fld(12,16,AC_MSKW32)//[27:16]
    #define CRC_CLIP_V_START Fld(12,0,AC_MSKW10)//[11:0]
#define CRC_3 (IO_POSTCOLOR_BASE + 0x5D0)
    #define CRC_Y_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define CRC_C_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define CRC_V_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define CRC_4 (IO_POSTCOLOR_BASE + 0x5D4)
    #define CRC_RESULT Fld(32,0,AC_FULLDW)//[31:0]

// Low Chroma Noise
#define LCOLORNOISE_1 (IO_POSTCOLOR_BASE + 0x5D8)
    #define LCN_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define LCN_INK_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define LCN_SAT_SRC Fld(1,29,AC_MSKB3)//[29:29]
    #define LIRE_STH Fld(7,22,AC_MSKW32)//[28:22]
    #define LIRE_YTH Fld(8,14,AC_MSKW21)//[21:14]
    #define LIRE_SSLOPE Fld(7,7,AC_MSKW10)//[13:7]
    #define LIRE_YSLOPE Fld(7,0,AC_MSKB0)//[6:0]
#define LCOLORNOISE_2 (IO_POSTCOLOR_BASE + 0x5DC)
    #define LSAT_SSLOPE Fld(7,23,AC_MSKW32)//[29:23]
    #define LSAT_STH Fld(7,16,AC_MSKB2)//[22:16]
    #define LSAT_GAIN Fld(7,8,AC_MSKB1)//[14:8]
    #define LIRE_GAIN Fld(7,0,AC_MSKB0)//[6:0]
#endif
