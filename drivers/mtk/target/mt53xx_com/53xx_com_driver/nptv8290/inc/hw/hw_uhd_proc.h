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


#if defined(CC_SUPPORT_UHD)
#define IO_UHD_PROC_BASE 0xF0009000

//Page SHARPNESS
#define SHARP_00 (IO_UHD_PROC_BASE + 0x000)
    #define TDS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define TDS_BLUR Fld(1,29,AC_MSKB3)//[29:29]
    #define TDS_FLT_LTI_V Fld(2,24,AC_MSKB3)//[25:24]
    #define TDS_FLT_LTI_H2 Fld(2,22,AC_MSKB2)//[23:22]
    #define TDS_FLT_LTI_H1 Fld(2,20,AC_MSKB2)//[21:20]
    #define TDS_DISABLE_LTI_V Fld(1,19,AC_MSKB2)//[19:19]
    #define TDS_DISABLE_LTI_H2 Fld(1,18,AC_MSKB2)//[18:18]
    #define TDS_DISABLE_LTI_H1 Fld(1,17,AC_MSKB2)//[17:17]
    #define TDS_DISABLE_V3 Fld(1,16,AC_MSKB2)//[16:16]
    #define TDS_DISABLE_V2 Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_DISABLE_V1 Fld(1,14,AC_MSKB1)//[14:14]
    #define TDS_DISABLE_D3 Fld(1,13,AC_MSKB1)//[13:13]
    #define TDS_DISABLE_D2 Fld(1,12,AC_MSKB1)//[12:12]
    #define TDS_DISABLE_D1 Fld(1,11,AC_MSKB1)//[11:11]
    #define TDS_DISABLE_H3 Fld(1,10,AC_MSKB1)//[10:10]
    #define TDS_DISABLE_H2 Fld(1,9,AC_MSKB1)//[9:9]
    #define TDS_DISABLE_H1 Fld(1,8,AC_MSKB1)//[8:8]
    #define SHP_COMBINE_MODE Fld(1,6,AC_MSKB0)//[6:6]
    #define TDS_FLT_V_3D Fld(2,4,AC_MSKB0)//[5:4]
    #define TDS_INK_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define SHARP_03 (IO_UHD_PROC_BASE + 0x00C)
    #define SHP_H1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_04 (IO_UHD_PROC_BASE + 0x010)
    #define SHP_H1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_05 (IO_UHD_PROC_BASE + 0x014)
    #define SHP_H1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_H1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_06 (IO_UHD_PROC_BASE + 0x018)
    #define SHP_H1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_07 (IO_UHD_PROC_BASE + 0x01C)
    #define SHP_H2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_08 (IO_UHD_PROC_BASE + 0x020)
    #define SHP_H2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_09 (IO_UHD_PROC_BASE + 0x024)
    #define SHP_H2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_H2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_0A (IO_UHD_PROC_BASE + 0x028)
    #define SHP_H2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_0B (IO_UHD_PROC_BASE + 0x02C)
    #define SHP_H3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_0C (IO_UHD_PROC_BASE + 0x030)
    #define SHP_H3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_H3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_H3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_H3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_0D (IO_UHD_PROC_BASE + 0x034)
    #define SHP_H3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_H3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_0E (IO_UHD_PROC_BASE + 0x038)
    #define SHP_H3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_H3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_0F (IO_UHD_PROC_BASE + 0x03C)
    #define SHP_V1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_10 (IO_UHD_PROC_BASE + 0x040)
    #define SHP_V1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_11 (IO_UHD_PROC_BASE + 0x044)
    #define SHP_V1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_V1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_12 (IO_UHD_PROC_BASE + 0x048)
    #define SHP_V1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_13 (IO_UHD_PROC_BASE + 0x04C)
    #define SHP_V2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_14 (IO_UHD_PROC_BASE + 0x050)
    #define SHP_V2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_15 (IO_UHD_PROC_BASE + 0x054)
    #define SHP_V2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_V2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_16 (IO_UHD_PROC_BASE + 0x058)
    #define SHP_V2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_17 (IO_UHD_PROC_BASE + 0x05C)
    #define SHP_V3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_18 (IO_UHD_PROC_BASE + 0x060)
    #define SHP_V3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_V3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_V3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_V3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_19 (IO_UHD_PROC_BASE + 0x064)
    #define SHP_V3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_V3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_1A (IO_UHD_PROC_BASE + 0x068)
    #define SHP_V3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_V3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_1B (IO_UHD_PROC_BASE + 0x06C)
    #define SHP_D1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_1C (IO_UHD_PROC_BASE + 0x070)
    #define SHP_D1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_1D (IO_UHD_PROC_BASE + 0x074)
    #define SHP_D1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_D1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_1E (IO_UHD_PROC_BASE + 0x078)
    #define SHP_D1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_1F (IO_UHD_PROC_BASE + 0x07C)
    #define SHP_D2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_20 (IO_UHD_PROC_BASE + 0x080)
    #define SHP_D2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_21 (IO_UHD_PROC_BASE + 0x084)
    #define SHP_D2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_D2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_22 (IO_UHD_PROC_BASE + 0x088)
    #define SHP_D2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_23 (IO_UHD_PROC_BASE + 0x08C)
    #define SHP_D3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_24 (IO_UHD_PROC_BASE + 0x090)
    #define SHP_D3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_D3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_D3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_D3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_25 (IO_UHD_PROC_BASE + 0x094)
    #define SHP_D3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_D3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_26 (IO_UHD_PROC_BASE + 0x098)
    #define SHP_D3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_D3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_27 (IO_UHD_PROC_BASE + 0x09C)
    #define SHP_COMBINED_NEG_GAIN Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_COMBINED_POS_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_30 (IO_UHD_PROC_BASE + 0x0C0)
    #define SHP_DETAIL_GAIN_MB Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_EDGE_GAIN_MB Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_DETAIL_GAIN_LB Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_EDGE_GAIN_LB Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_31 (IO_UHD_PROC_BASE + 0x0C4)
    #define SHP_EDGE_RANK_SEL Fld(2,18,AC_MSKB2)//[19:18]
    #define SHP_DETAIL_RANK_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define SHP_DETAIL_GAIN_HB Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_EDGE_GAIN_HB Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_32 (IO_UHD_PROC_BASE + 0x0C8)
    #define SHP_FLAT_STEP Fld(4,28,AC_MSKB3)//[31:28]
    #define SHP_FLAT_TH Fld(6,22,AC_MSKW32)//[27:22]
    #define SHP_DETAIL_STEP Fld(4,18,AC_MSKB2)//[21:18]
    #define SHP_DETAIL_TH Fld(6,12,AC_MSKW21)//[17:12]
    #define SHP_EDGE_STEP Fld(4,8,AC_MSKB1)//[11:8]
    #define SHP_EDGE_TH Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_33 (IO_UHD_PROC_BASE + 0x0CC)
    #define SHP_MIN_MAX_HF Fld(2,30,AC_MSKB3)//[31:30]
    #define SHP_MIN_MAX_XS Fld(11,16,AC_MSKW32)//[26:16]
    #define SHP_MIN_MAX_XE Fld(11,0,AC_MSKW10)//[10:0]
#define SHARP_34 (IO_UHD_PROC_BASE + 0x0D0)
    #define SHP_MIN_MAX_VF Fld(2,30,AC_MSKB3)//[31:30]
    #define SHP_MIN_MAX_YS Fld(11,16,AC_MSKW32)//[26:16]
    #define SHP_MIN_MAX_YE Fld(11,0,AC_MSKW10)//[10:0]
#define SHARP_35 (IO_UHD_PROC_BASE + 0x0D4)
    #define SHP_REGIONAL_MAX Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_REGIONAL_MIN Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_36 (IO_UHD_PROC_BASE + 0x0D8)
    #define TDS_SAT_PROC Fld(6,20,AC_MSKW32)//[25:20]
    #define TDS_SAT_CONSERVATIVE Fld(1,17,AC_MSKB2)//[17:17]
    #define TDS_SAT_121 Fld(1,16,AC_MSKB2)//[16:16]
    #define TDS_SAT_WHITE Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_SAT_BLACK Fld(5,0,AC_MSKB0)//[4:0]
#define SHARP_40 (IO_UHD_PROC_BASE + 0x100)
    #define TDS_HV_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define TDS_HV_SLOPE Fld(3,24,AC_MSKB3)//[26:24]
    #define TDS_HV_MIN Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_HV_THR Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_HV_COR Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_41 (IO_UHD_PROC_BASE + 0x104)
    #define TDS_AC_LPF_AUTO Fld(1,24,AC_MSKB3)//[24:24]
    #define TDS_AC_LPF_COE_AUTO_STEP Fld(4,20,AC_MSKB2)//[23:20]
    #define TDS_AC_LPF_COE Fld(4,16,AC_MSKB2)//[19:16]
    #define TDS_AC_LPF_HIGH Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_AC_LPF_LOW Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_42 (IO_UHD_PROC_BASE + 0x108)
    #define TDS_TAP_MODE Fld(1,22,AC_MSKB2)//[22:22]
    #define TDS_TAP_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define TDS_TAP_CLIP_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define TDS_TAP_LIMIT Fld(6,12,AC_MSKW21)//[17:12]
    #define TDS_TAP_RATIO Fld(5,4,AC_MSKW10)//[8:4]
    #define TDS_TAP_SRATIO Fld(4,0,AC_MSKB0)//[3:0]
#define SHARP_43 (IO_UHD_PROC_BASE + 0x10C)
    #define TDS_NEG_EN_H Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_NEG_RATIO_H Fld(5,24,AC_MSKB3)//[28:24]
    #define TDS_NEG_LIMIT_H Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_NEG_EN_V Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_NEG_RATIO_V Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_NEG_LIMIT_V Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_44 (IO_UHD_PROC_BASE + 0x110)
    #define TDS_NEG_EN_D Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_NEG_RATIO_D Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_NEG_LIMIT_D Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_45 (IO_UHD_PROC_BASE + 0x114)
    #define TDS_CLIP_EN_MD Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_CLIP_TAP_MD Fld(1,30,AC_MSKB3)//[30:30]
    #define TDS_CLIP_RATIO_MD Fld(5,24,AC_MSKB3)//[28:24]
    #define TDS_CLIP_THR_MD Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_CLIP_EN_LH Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_CLIP_TAP_LH Fld(1,14,AC_MSKB1)//[14:14]
    #define TDS_CLIP_RATIO_LH Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_CLIP_THR_LH Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_46 (IO_UHD_PROC_BASE + 0x118)
    #define TDS_CLIP_EN_LTI_V Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_CLIP_TAP_LTI_V Fld(1,30,AC_MSKB3)//[30:30]
    #define TDS_CLIP_RATIO_LTI_V Fld(5,24,AC_MSKB3)//[28:24]
    #define TDS_CLIP_THR_LTI_V Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_CLIP_EN_HV Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_CLIP_TAP_HV Fld(1,14,AC_MSKB1)//[14:14]
    #define TDS_CLIP_RATIO_HV Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_CLIP_THR_HV Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_47 (IO_UHD_PROC_BASE + 0x11C)
    #define TDS_YLEV_P000 Fld(8,24,AC_FULLB3)//[31:24]
    #define TDS_YLEV_P064 Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_YLEV_P128 Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_YLEV_P192 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_48 (IO_UHD_PROC_BASE + 0x120)
    #define TDS_YLEV_P256 Fld(8,24,AC_FULLB3)//[31:24]
    #define TDS_YLEV_P320 Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_YLEV_P384 Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_YLEV_P448 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_49 (IO_UHD_PROC_BASE + 0x124)
    #define TDS_YLEV_P512 Fld(8,24,AC_FULLB3)//[31:24]
    #define TDS_YLEV_P576 Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_YLEV_P640 Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_YLEV_P704 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_4A (IO_UHD_PROC_BASE + 0x128)
    #define TDS_YLEV_P768 Fld(8,24,AC_FULLB3)//[31:24]
    #define TDS_YLEV_P832 Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_YLEV_P896 Fld(8,8,AC_FULLB1)//[15:8]
    #define TDS_YLEV_P960 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_4B (IO_UHD_PROC_BASE + 0x12C)
    #define TDS_YLEV_P1024 Fld(8,24,AC_FULLB3)//[31:24]
    #define TDS_YLEV_ALPHA Fld(6,16,AC_MSKB2)//[21:16]
    #define TDS_YLEV_SEL Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_YLEV_EN Fld(1,14,AC_MSKB1)//[14:14]
#define SHARP_4C (IO_UHD_PROC_BASE + 0x130)
    #define TDS_VERT_TAP_APT_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_VERT_TAP_APT_SLOPE Fld(6,24,AC_MSKB3)//[29:24]
    #define TDS_VERT_TAP_APT_EDGE Fld(6,18,AC_MSKB2)//[23:18]
    #define TDS_VERT_TAP_APT_DIFF Fld(6,12,AC_MSKW21)//[17:12]
    #define TDS_VERT_TAP_APT_BACK Fld(6,6,AC_MSKW10)//[11:6]
#define SHARP_4D (IO_UHD_PROC_BASE + 0x134)
    #define TDS_HORIZ_TAP_APT_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_HORIZ_TAP_APT_SLOPE Fld(6,24,AC_MSKB3)//[29:24]
    #define TDS_HORIZ_TAP_APT_EDGE Fld(6,18,AC_MSKB2)//[23:18]
    #define TDS_HORIZ_TAP_APT_DIFF Fld(6,12,AC_MSKW21)//[17:12]
    #define TDS_HORIZ_TAP_APT_BACK_0 Fld(6,6,AC_MSKW10)//[11:6]
    #define TDS_HORIZ_TAP_APT_BACK_1 Fld(6,0,AC_MSKB0)//[5:0]
#define SHARP_4E (IO_UHD_PROC_BASE + 0x138)
    #define TDS_HORIZ_TAP_APT_BACK_2 Fld(6,26,AC_MSKB3)//[31:26]
    #define TDS_HORIZ_TAP_APT_BACK_3 Fld(6,20,AC_MSKW32)//[25:20]
    #define TDS_HORIZ_TAP_APT_BACK_H2 Fld(6,14,AC_MSKW21)//[19:14]
    #define TDS_HORIZ_TAP_APT_EDGE_H2 Fld(6,8,AC_MSKB1)//[13:8]
#define SHARP_50 (IO_UHD_PROC_BASE + 0x140)
    #define SHP_LTI_HL_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_HL_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_HL_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_HL_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_51 (IO_UHD_PROC_BASE + 0x144)
    #define SHP_LTI_HL_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_HL_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_HL_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_HL_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_52 (IO_UHD_PROC_BASE + 0x148)
    #define SHP_LTI_HL_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_LTI_HL_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_HL_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_53 (IO_UHD_PROC_BASE + 0x14C)
    #define SHP_LTI_HL_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_HL_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_54 (IO_UHD_PROC_BASE + 0x150)
    #define SHP_LTI_HH_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_HH_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_HH_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_HH_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_55 (IO_UHD_PROC_BASE + 0x154)
    #define SHP_LTI_HH_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_HH_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_HH_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_HH_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_56 (IO_UHD_PROC_BASE + 0x158)
    #define SHP_LTI_HH_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_LTI_HH_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_HH_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_57 (IO_UHD_PROC_BASE + 0x15C)
    #define SHP_LTI_HH_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_HH_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_58 (IO_UHD_PROC_BASE + 0x160)
    #define SHP_LTI_V_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_V_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_V_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_V_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_59 (IO_UHD_PROC_BASE + 0x164)
    #define SHP_LTI_V_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define SHP_LTI_V_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SHP_LTI_V_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define SHP_LTI_V_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_5A (IO_UHD_PROC_BASE + 0x168)
    #define SHP_LTI_V_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define SHP_LTI_V_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_V_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_5B (IO_UHD_PROC_BASE + 0x16C)
    #define SHP_LTI_V_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define SHP_LTI_V_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define SHARP_5C (IO_UHD_PROC_BASE + 0x170)
    #define TDS_CLIP_EN_LTI_H2 Fld(1,31,AC_MSKB3)//[31:31]
    #define TDS_CLIP_TAP_LTI_H2 Fld(1,30,AC_MSKB3)//[30:30]
    #define TDS_CLIP_RATIO_LTI_H2 Fld(5,24,AC_MSKB3)//[28:24]
    #define TDS_CLIP_THR_LTI_H2 Fld(8,16,AC_FULLB2)//[23:16]
    #define TDS_CLIP_EN_LTI_H1 Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_CLIP_TAP_LTI_H1 Fld(1,14,AC_MSKB1)//[14:14]
    #define TDS_CLIP_RATIO_LTI_H1 Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_CLIP_THR_LTI_H1 Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_5D (IO_UHD_PROC_BASE + 0x174)
    #define TDS_CLIP_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define TDS_CLIP_TAP Fld(1,14,AC_MSKB1)//[14:14]
    #define TDS_CLIP_RATIO Fld(5,8,AC_MSKB1)//[12:8]
    #define TDS_CLIP_THR Fld(8,0,AC_FULLB0)//[7:0]
#define SHARP_5E (IO_UHD_PROC_BASE + 0x178)
    #define SHP_RESERVED Fld(32,0,AC_FULLDW)//[31:0]

//Page CTIR
#define CTIR_00 (IO_UHD_PROC_BASE + 0x280)
    #define CTIR_HUE_TIE Fld(1,28,AC_MSKB3)//[28:28]
    #define CTIR_FLPF Fld(1,12,AC_MSKB1)//[12:12]
    #define CTIR_FLPF_SEL Fld(1,11,AC_MSKB1)//[11:11]
    #define CTIR_DX_SGN Fld(1,9,AC_MSKB1)//[9:9]
    #define CTIR_PRT_ENA Fld(1,6,AC_MSKB0)//[6:6]
    #define CTIR_SGN_PRT Fld(1,5,AC_MSKB0)//[5:5]
    #define CTIR_HD Fld(1,4,AC_MSKB0)//[4:4]
    #define CTIR_INK Fld(1,3,AC_MSKB0)//[3:3]
    #define CTIR_ENA Fld(1,0,AC_MSKB0)//[0:0]
#define CTIR_01 (IO_UHD_PROC_BASE + 0x284)
    #define CTIR_LPF3 Fld(1,26,AC_MSKB3)//[26:26]
    #define CTIR_LPF3_SEL Fld(2,24,AC_MSKB3)//[25:24]
    #define CTIR_LPF2 Fld(1,22,AC_MSKB2)//[22:22]
    #define CTIR_LPF2_SEL Fld(2,20,AC_MSKB2)//[21:20]
    #define CTIR_LPF1 Fld(1,18,AC_MSKB2)//[18:18]
    #define CTIR_LPF1_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define CTIR_FIX_SZ Fld(3,12,AC_MSKB1)//[14:12]
    #define CTIR_FIX Fld(1,11,AC_MSKB1)//[11:11]
    #define CTIR_SEXT_LARGE Fld(1,10,AC_MSKB1)//[10:10]
    #define CTIR_SEXT Fld(1,9,AC_MSKB1)//[9:9]
    #define CTIR_SGN Fld(1,8,AC_MSKB1)//[8:8]
    #define CTIR_SGN_TH Fld(8,0,AC_FULLB0)//[7:0]
#define CTIR_02 (IO_UHD_PROC_BASE + 0x288)
    #define CTIR_U_STB_OFST2 Fld(8,24,AC_FULLB3)//[31:24]
    #define CTIR_U_STB_GAIN Fld(8,16,AC_FULLB2)//[23:16]
    #define CTIR_U_STB_OFST1 Fld(8,8,AC_FULLB1)//[15:8]
    #define CTIR_U_WND_SZ Fld(3,0,AC_MSKB0)//[2:0]
#define CTIR_03 (IO_UHD_PROC_BASE + 0x28C)
    #define CTIR_V_STB_OFST2 Fld(8,24,AC_FULLB3)//[31:24]
    #define CTIR_V_STB_GAIN Fld(8,16,AC_FULLB2)//[23:16]
    #define CTIR_V_STB_OFST1 Fld(8,8,AC_FULLB1)//[15:8]
    #define CTIR_V_WND_SZ Fld(3,0,AC_MSKB0)//[2:0]
#define CTIR_04 (IO_UHD_PROC_BASE + 0x290)
    #define CTIR_FLAT_OFST2 Fld(8,24,AC_FULLB3)//[31:24]
    #define CTIR_FLAT_GAIN Fld(8,16,AC_FULLB2)//[23:16]
    #define CTIR_FLAT_OFST1 Fld(8,8,AC_FULLB1)//[15:8]
    #define CTIR_FLAT_MODE Fld(1,6,AC_MSKB0)//[6:6]
    #define CTIR_FLAT_TIE Fld(1,5,AC_MSKB0)//[5:5]
    #define CTIR_FLAT_BYPASS Fld(1,4,AC_MSKB0)//[4:4]
    #define CTIR_FLAT_SZ Fld(3,0,AC_MSKB0)//[2:0]
#define CTIR_05 (IO_UHD_PROC_BASE + 0x294)
    #define CTIR_COR Fld(8,0,AC_FULLB0)//[7:0]
#define CTIR_07 (IO_UHD_PROC_BASE + 0x29C)
    #define CTIR_ATPG_CT Fld(1,31,AC_MSKB3)//[31:31]
    #define CTIR_ATPG_OB Fld(1,30,AC_MSKB3)//[30:30]

//Page CBE
#define CBE_00 (IO_UHD_PROC_BASE + 0x2C0)
    #define CBEENABLE Fld(1,31,AC_MSKB3)//[31:31]
    #define CBEINKMODE Fld(4,24,AC_MSKB3)//[27:24]
    #define CBE_THR_K7X3_IT3 Fld(7,16,AC_MSKB2)//[22:16]
    #define CBE_W_IT3 Fld(7,8,AC_MSKB1)//[14:8]
    #define CBE_W_IT5 Fld(7,0,AC_MSKB0)//[6:0]
#define CBE_01 (IO_UHD_PROC_BASE + 0x2C4)
    #define CBE_THR_K7X3_IT5 Fld(7,8,AC_MSKB1)//[14:8]
    #define CBE_THR_K5X3_IT5 Fld(7,0,AC_MSKB0)//[6:0]
#define CBE_02 (IO_UHD_PROC_BASE + 0x2C8)
    #define CBE_THR_K5X3_IT3 Fld(7,8,AC_MSKB1)//[14:8]
    #define CBE_THR_K3X3_IT3 Fld(7,0,AC_MSKB0)//[6:0]
#define CBE_05 (IO_UHD_PROC_BASE + 0x2D4)
    #define BYPASS_CBE Fld(1,2,AC_MSKB0)//[2:2]

//Page MISC
#define SP_MASK_00 (IO_UHD_PROC_BASE + 0x340)
    #define CH1_H_DMWINDOW_END Fld(16,16,AC_FULLW32)//[31:16]
    #define CH1_H_DMWINDOW_START Fld(16,0,AC_FULLW10)//[15:0]
#define SP_MASK_01 (IO_UHD_PROC_BASE + 0x344)
    #define CH1_V_DMWINDOW_END Fld(16,16,AC_FULLW32)//[31:16]
    #define CH1_V_DMWINDOW_START Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_00 (IO_UHD_PROC_BASE + 0x350)
    #define PPCM_SUB_PATGEN_VSE Fld(1,31,AC_MSKB3)//[31:31]
    #define ECTI_DEMO_SWAP Fld(1,26,AC_MSKB3)//[26:26]
    #define ECTI_DEMO_ENA Fld(1,25,AC_MSKB3)//[25:25]
    #define TDS_DEMO_SWAP Fld(1,24,AC_MSKB3)//[24:24]
    #define TDS_DEMO_ENA Fld(1,23,AC_MSKB2)//[23:23]
    #define TDPROC_BYPASS_ALL Fld(1,22,AC_MSKB2)//[22:22]
    #define NO_VERTICAL_SHARP Fld(1,20,AC_MSKB2)//[20:20]
    #define TDPROC_PATGEN_VSEL Fld(1,19,AC_MSKB2)//[19:19]
    #define TDPROC_PATGEN_USEL Fld(1,18,AC_MSKB2)//[18:18]
    #define TDPROC_PATGEN_YSEL Fld(1,17,AC_MSKB2)//[17:17]
    #define PATGEN_CTRL_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define PATGEN_HVSEL Fld(1,12,AC_MSKB1)//[12:12]
    #define TDPROC_PATGEN_DATA_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define TDPROC_PATGEN_MODE Fld(3,8,AC_MSKB1)//[10:8]
    #define LINEBUF_4SRS_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define TDPROC_DGB Fld(3,0,AC_MSKB0)//[2:0]
#define TDPROC_MISC_01 (IO_UHD_PROC_BASE + 0x354)
    #define TDPROC_RND_LD Fld(1,30,AC_MSKB3)//[30:30]
    #define TDPRCO_RND_SEED Fld(30,0,AC_MSKDW)//[29:0]
#define TDPROC_MISC_02 (IO_UHD_PROC_BASE + 0x358)
    #define TDPROC_VSW Fld(16,16,AC_FULLW32)//[31:16]
    #define TDPROC_VPORCH Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_03 (IO_UHD_PROC_BASE + 0x35C)
    #define TDPROC_HSW Fld(16,16,AC_FULLW32)//[31:16]
    #define TDPROC_HPORCH Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_04 (IO_UHD_PROC_BASE + 0x360)
    #define TDPROC_PICH Fld(16,16,AC_FULLW32)//[31:16]
    #define TDPROC_PICW Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_05 (IO_UHD_PROC_BASE + 0x364)
    #define TDPROC_INK_YPOS Fld(16,16,AC_FULLW32)//[31:16]
    #define TDPROC_INK_XPOS Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_06 (IO_UHD_PROC_BASE + 0x368)
    #define CH1_LCRC_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define CH1_RCRC_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define CH1_Y_CRC_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define CH1_CB_CRC_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define CH1_CR_CRC_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define TDPROC_MISC_07 (IO_UHD_PROC_BASE + 0x36C)
    #define CH1_CRC_CLIP_H_END Fld(13,16,AC_MSKW32)//[28:16]
    #define CH1_CRC_CLIP_H_START Fld(13,0,AC_MSKW10)//[12:0]
#define TDPROC_MISC_08 (IO_UHD_PROC_BASE + 0x370)
    #define CH1_CRC_CLIP_V_END Fld(12,16,AC_MSKW32)//[27:16]
    #define CH1_CRC_CLIP_V_START Fld(12,0,AC_MSKW10)//[11:0]
#define TDPROC_MISC_09 (IO_UHD_PROC_BASE + 0x374)
    #define CH1_CRC_RESULT Fld(32,0,AC_FULLDW)//[31:0]
#define TDPROC_MISC_0D (IO_UHD_PROC_BASE + 0x384)
    #define CH2_CRC_RESULT Fld(32,0,AC_FULLDW)//[31:0]
#define TDPROC_MISC_0E (IO_UHD_PROC_BASE + 0x388)
    #define CH2_CRC_STILL_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define CH1_CRC_STILL_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define TDPROC_MISC_0F (IO_UHD_PROC_BASE + 0x38C)
    #define CORE_RST Fld(1,31,AC_MSKB3)//[31:31]
    #define DISPRCNTEN Fld(1,29,AC_MSKB3)//[29:29]
    #define CRCIN_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define CH1_CRC_STILL_MAX Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BYPASS_DISP_R Fld(1,5,AC_MSKB0)//[5:5]
    #define C_CRC_STILL_CNT_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define CH1_CRC_STILL_TRG Fld(1,2,AC_MSKB0)//[2:2]

//Page CDS
#define CDS_00 (IO_UHD_PROC_BASE + 0x600)
    #define CDS1_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_MODE_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define CDS1_ALPHA_1 Fld(12,16,AC_MSKW32)//[27:16]
    #define CDS1_D_THETA Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D_RADIUS Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_01 (IO_UHD_PROC_BASE + 0x604)
    #define CDS1_THETA_C1 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS1_RADIUS_C1 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS1_BETA Fld(12,0,AC_MSKW10)//[11:0]
#define CDS_02 (IO_UHD_PROC_BASE + 0x608)
    #define CDS1_THETA_C2 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS1_RADIUS_C2 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS1_MAX_RADIUS_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_03 (IO_UHD_PROC_BASE + 0x60C)
    #define CDS1_ALPHA Fld(12,20,AC_MSKW32)//[31:20]
    #define CDS1_EDGE_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define CDS1_EDGE_THRESHOLD Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_EDGE_SLOPE Fld(6,0,AC_MSKB0)//[5:0]
#define CDS_04 (IO_UHD_PROC_BASE + 0x610)
    #define CDS1_CONF_GAIN Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_LPF_STRENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_LPF_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define CDS1_MAX_THETA_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_06 (IO_UHD_PROC_BASE + 0x618)
    #define CDS1_H1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_07 (IO_UHD_PROC_BASE + 0x61C)
    #define CDS1_H1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_08 (IO_UHD_PROC_BASE + 0x620)
    #define CDS1_H1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_H1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_09 (IO_UHD_PROC_BASE + 0x624)
    #define CDS1_H1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_0A (IO_UHD_PROC_BASE + 0x628)
    #define CDS1_H2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_0B (IO_UHD_PROC_BASE + 0x62C)
    #define CDS1_H2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_0C (IO_UHD_PROC_BASE + 0x630)
    #define CDS1_H2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_H2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_0D (IO_UHD_PROC_BASE + 0x634)
    #define CDS1_H2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_0E (IO_UHD_PROC_BASE + 0x638)
    #define CDS1_H3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_0F (IO_UHD_PROC_BASE + 0x63C)
    #define CDS1_H3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_H3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_H3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_H3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_10 (IO_UHD_PROC_BASE + 0x640)
    #define CDS1_H3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_H3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_11 (IO_UHD_PROC_BASE + 0x644)
    #define CDS1_H3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_H3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_12 (IO_UHD_PROC_BASE + 0x648)
    #define CDS1_V1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_13 (IO_UHD_PROC_BASE + 0x64C)
    #define CDS1_V1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_14 (IO_UHD_PROC_BASE + 0x650)
    #define CDS1_V1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_V1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_15 (IO_UHD_PROC_BASE + 0x654)
    #define CDS1_V1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_16 (IO_UHD_PROC_BASE + 0x658)
    #define CDS1_V2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_17 (IO_UHD_PROC_BASE + 0x65C)
    #define CDS1_V2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_18 (IO_UHD_PROC_BASE + 0x660)
    #define CDS1_V2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_V2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_19 (IO_UHD_PROC_BASE + 0x664)
    #define CDS1_V2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_1A (IO_UHD_PROC_BASE + 0x668)
    #define CDS1_V3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_1B (IO_UHD_PROC_BASE + 0x66C)
    #define CDS1_V3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_V3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_V3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_V3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_1C (IO_UHD_PROC_BASE + 0x670)
    #define CDS1_V3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_V3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_1D (IO_UHD_PROC_BASE + 0x674)
    #define CDS1_V3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_V3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_1E (IO_UHD_PROC_BASE + 0x678)
    #define CDS1_D1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_1F (IO_UHD_PROC_BASE + 0x67C)
    #define CDS1_D1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_20 (IO_UHD_PROC_BASE + 0x680)
    #define CDS1_D1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_D1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_21 (IO_UHD_PROC_BASE + 0x684)
    #define CDS1_D1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_22 (IO_UHD_PROC_BASE + 0x688)
    #define CDS1_D2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_23 (IO_UHD_PROC_BASE + 0x68C)
    #define CDS1_D2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_24 (IO_UHD_PROC_BASE + 0x690)
    #define CDS1_D2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_D2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_25 (IO_UHD_PROC_BASE + 0x694)
    #define CDS1_D2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_26 (IO_UHD_PROC_BASE + 0x698)
    #define CDS1_D3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_27 (IO_UHD_PROC_BASE + 0x69C)
    #define CDS1_D3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS1_D3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS1_D3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS1_D3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_28 (IO_UHD_PROC_BASE + 0x6A0)
    #define CDS1_D3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS1_D3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_29 (IO_UHD_PROC_BASE + 0x6A4)
    #define CDS1_D3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS1_D3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_2A (IO_UHD_PROC_BASE + 0x6A8)
    #define CDS2_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_MODE_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define CDS2_ALPHA_1 Fld(12,16,AC_MSKW32)//[27:16]
    #define CDS2_D_THETA Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D_RADIUS Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_2B (IO_UHD_PROC_BASE + 0x6AC)
    #define CDS2_THETA_C1 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS2_RADIUS_C1 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS2_BETA Fld(12,0,AC_MSKW10)//[11:0]
#define CDS_2C (IO_UHD_PROC_BASE + 0x6B0)
    #define CDS2_THETA_C2 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS2_RADIUS_C2 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS2_MAX_RADIUS_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_2D (IO_UHD_PROC_BASE + 0x6B4)
    #define CDS2_ALPHA Fld(12,20,AC_MSKW32)//[31:20]
    #define CDS2_EDGE_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define CDS2_EDGE_THRESHOLD Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_EDGE_SLOPE Fld(6,0,AC_MSKB0)//[5:0]
#define CDS_2E (IO_UHD_PROC_BASE + 0x6B8)
    #define CDS2_CONF_GAIN Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_LPF_STRENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_LPF_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define CDS2_MAX_THETA_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_30 (IO_UHD_PROC_BASE + 0x6C0)
    #define CDS2_H1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_31 (IO_UHD_PROC_BASE + 0x6C4)
    #define CDS2_H1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_32 (IO_UHD_PROC_BASE + 0x6C8)
    #define CDS2_H1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_H1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_33 (IO_UHD_PROC_BASE + 0x6CC)
    #define CDS2_H1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_34 (IO_UHD_PROC_BASE + 0x6D0)
    #define CDS2_H2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_35 (IO_UHD_PROC_BASE + 0x6D4)
    #define CDS2_H2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_36 (IO_UHD_PROC_BASE + 0x6D8)
    #define CDS2_H2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_H2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_37 (IO_UHD_PROC_BASE + 0x6DC)
    #define CDS2_H2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_38 (IO_UHD_PROC_BASE + 0x6E0)
    #define CDS2_H3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_39 (IO_UHD_PROC_BASE + 0x6E4)
    #define CDS2_H3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_H3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_H3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_H3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_3A (IO_UHD_PROC_BASE + 0x6E8)
    #define CDS2_H3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_H3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_3B (IO_UHD_PROC_BASE + 0x6EC)
    #define CDS2_H3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_H3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_3C (IO_UHD_PROC_BASE + 0x6F0)
    #define CDS2_V1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_3D (IO_UHD_PROC_BASE + 0x6F4)
    #define CDS2_V1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_3E (IO_UHD_PROC_BASE + 0x6F8)
    #define CDS2_V1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_V1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_3F (IO_UHD_PROC_BASE + 0x6FC)
    #define CDS2_V1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_40 (IO_UHD_PROC_BASE + 0x700)
    #define CDS2_V2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_41 (IO_UHD_PROC_BASE + 0x704)
    #define CDS2_V2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_42 (IO_UHD_PROC_BASE + 0x708)
    #define CDS2_V2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_V2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_43 (IO_UHD_PROC_BASE + 0x70C)
    #define CDS2_V2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_44 (IO_UHD_PROC_BASE + 0x710)
    #define CDS2_V3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_45 (IO_UHD_PROC_BASE + 0x714)
    #define CDS2_V3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_V3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_V3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_V3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_46 (IO_UHD_PROC_BASE + 0x718)
    #define CDS2_V3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_V3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_47 (IO_UHD_PROC_BASE + 0x71C)
    #define CDS2_V3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_V3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_48 (IO_UHD_PROC_BASE + 0x720)
    #define CDS2_D1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_49 (IO_UHD_PROC_BASE + 0x724)
    #define CDS2_D1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_4A (IO_UHD_PROC_BASE + 0x728)
    #define CDS2_D1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_D1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_4B (IO_UHD_PROC_BASE + 0x72C)
    #define CDS2_D1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_4C (IO_UHD_PROC_BASE + 0x730)
    #define CDS2_D2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_4D (IO_UHD_PROC_BASE + 0x734)
    #define CDS2_D2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_4E (IO_UHD_PROC_BASE + 0x738)
    #define CDS2_D2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_D2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_4F (IO_UHD_PROC_BASE + 0x73C)
    #define CDS2_D2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_50 (IO_UHD_PROC_BASE + 0x740)
    #define CDS2_D3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_51 (IO_UHD_PROC_BASE + 0x744)
    #define CDS2_D3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS2_D3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS2_D3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS2_D3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_52 (IO_UHD_PROC_BASE + 0x748)
    #define CDS2_D3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS2_D3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_53 (IO_UHD_PROC_BASE + 0x74C)
    #define CDS2_D3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS2_D3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_54 (IO_UHD_PROC_BASE + 0x750)
    #define CDS3_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_MODE_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define CDS3_ALPHA_1 Fld(12,16,AC_MSKW32)//[27:16]
    #define CDS3_D_THETA Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D_RADIUS Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_55 (IO_UHD_PROC_BASE + 0x754)
    #define CDS3_THETA_C1 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS3_RADIUS_C1 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS3_BETA Fld(12,0,AC_MSKW10)//[11:0]
#define CDS_56 (IO_UHD_PROC_BASE + 0x758)
    #define CDS3_THETA_C2 Fld(10,22,AC_MSKW32)//[31:22]
    #define CDS3_RADIUS_C2 Fld(10,12,AC_MSKW21)//[21:12]
    #define CDS3_MAX_RADIUS_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_57 (IO_UHD_PROC_BASE + 0x75C)
    #define CDS3_ALPHA Fld(12,20,AC_MSKW32)//[31:20]
    #define CDS3_EDGE_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define CDS3_EDGE_THRESHOLD Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_EDGE_SLOPE Fld(6,0,AC_MSKB0)//[5:0]
#define CDS_58 (IO_UHD_PROC_BASE + 0x760)
    #define CDS3_CONF_GAIN Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_LPF_STRENGTH Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_LPF_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define CDS3_MAX_THETA_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_59 (IO_UHD_PROC_BASE + 0x764)
    #define CDS3_H1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_5A (IO_UHD_PROC_BASE + 0x768)
    #define CDS3_H1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_5B (IO_UHD_PROC_BASE + 0x76C)
    #define CDS3_H1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_H1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_5C (IO_UHD_PROC_BASE + 0x770)
    #define CDS3_H1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_5D (IO_UHD_PROC_BASE + 0x774)
    #define CDS3_H2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_5E (IO_UHD_PROC_BASE + 0x778)
    #define CDS3_H2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_5F (IO_UHD_PROC_BASE + 0x77C)
    #define CDS3_H2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_H2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_60 (IO_UHD_PROC_BASE + 0x780)
    #define CDS3_H2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_61 (IO_UHD_PROC_BASE + 0x784)
    #define CDS3_H3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_62 (IO_UHD_PROC_BASE + 0x788)
    #define CDS3_H3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_H3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_H3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_H3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_63 (IO_UHD_PROC_BASE + 0x78C)
    #define CDS3_H3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_H3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_64 (IO_UHD_PROC_BASE + 0x790)
    #define CDS3_H3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_H3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_65 (IO_UHD_PROC_BASE + 0x794)
    #define CDS3_V1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_66 (IO_UHD_PROC_BASE + 0x798)
    #define CDS3_V1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_67 (IO_UHD_PROC_BASE + 0x79C)
    #define CDS3_V1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_V1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_68 (IO_UHD_PROC_BASE + 0x7A0)
    #define CDS3_V1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_69 (IO_UHD_PROC_BASE + 0x7A4)
    #define CDS3_V2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_6A (IO_UHD_PROC_BASE + 0x7A8)
    #define CDS3_V2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_6B (IO_UHD_PROC_BASE + 0x7AC)
    #define CDS3_V2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_V2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_6C (IO_UHD_PROC_BASE + 0x7B0)
    #define CDS3_V2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_6D (IO_UHD_PROC_BASE + 0x7B4)
    #define CDS3_V3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_6E (IO_UHD_PROC_BASE + 0x7B8)
    #define CDS3_V3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_V3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_V3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_V3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_6F (IO_UHD_PROC_BASE + 0x7BC)
    #define CDS3_V3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_V3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_70 (IO_UHD_PROC_BASE + 0x7C0)
    #define CDS3_V3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_V3_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_71 (IO_UHD_PROC_BASE + 0x7C4)
    #define CDS3_D1_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D1_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D1_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D1_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_72 (IO_UHD_PROC_BASE + 0x7C8)
    #define CDS3_D1_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D1_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D1_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D1_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_73 (IO_UHD_PROC_BASE + 0x7CC)
    #define CDS3_D1_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_D1_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D1_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_74 (IO_UHD_PROC_BASE + 0x7D0)
    #define CDS3_D1_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D1_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_75 (IO_UHD_PROC_BASE + 0x7D4)
    #define CDS3_D2_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D2_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D2_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D2_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_76 (IO_UHD_PROC_BASE + 0x7D8)
    #define CDS3_D2_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D2_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D2_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D2_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_77 (IO_UHD_PROC_BASE + 0x7DC)
    #define CDS3_D2_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_D2_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D2_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_78 (IO_UHD_PROC_BASE + 0x7E0)
    #define CDS3_D2_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D2_S34 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_79 (IO_UHD_PROC_BASE + 0x7E4)
    #define CDS3_D3_X1 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D3_Y1 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D3_X2 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D3_Y2 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_7A (IO_UHD_PROC_BASE + 0x7E8)
    #define CDS3_D3_X3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CDS3_D3_Y3 Fld(8,16,AC_FULLB2)//[23:16]
    #define CDS3_D3_X4 Fld(8,8,AC_FULLB1)//[15:8]
    #define CDS3_D3_Y4 Fld(8,0,AC_FULLB0)//[7:0]
#define CDS_7B (IO_UHD_PROC_BASE + 0x7EC)
    #define CDS3_D3_BYPASS_CP4 Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS3_D3_S01 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D3_S12 Fld(14,0,AC_MSKW10)//[13:0]
#define CDS_7C (IO_UHD_PROC_BASE + 0x7F0)
    #define CDS_ATPG_CT Fld(1,31,AC_MSKB3)//[31:31]
    #define CDS_ATPG_OB Fld(1,30,AC_MSKB3)//[30:30]
    #define CDS3_D3_S23 Fld(14,16,AC_MSKW32)//[29:16]
    #define CDS3_D3_S34 Fld(14,0,AC_MSKW10)//[13:0]

//Page OSTG
#define OSTG_00 (IO_UHD_PROC_BASE + 0x800)
    #define R_LD_SRC_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define R_OSTG2MON_SEL Fld(2,26,AC_MSKB3)//[27:26]
    #define C_ITG_RES Fld(2,24,AC_MSKB3)//[25:24]
    #define C_ITG_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define R_OSTG2VDOIN_SEL Fld(1,22,AC_MSKB2)//[22:22]
    #define C_DBUF_POCLK_ON Fld(1,21,AC_MSKB2)//[21:21]
    #define C_DBUF_OCLK_ON Fld(1,20,AC_MSKB2)//[20:20]
    #define C_BYPASS_TTD Fld(1,19,AC_MSKB2)//[19:19]
    #define R_POST_MIX_GC_CLK_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define R_POST_OCLK_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define R_POST_POCLK_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_PTGEN_FOR_XVYCC Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_XVYCC_CLIP_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define C_SET_DATA_VS_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_SET_DATA_READY Fld(1,4,AC_MSKB0)//[4:4]
    #define CLK3_RST_CTRL Fld(1,3,AC_MSKB0)//[3:3]
    #define CLK2_RST_CTRL Fld(1,2,AC_MSKB0)//[2:2]
    #define CLK_RST_CTRL Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_RST_CTRL Fld(1,0,AC_MSKB0)//[0:0]
#define OSTG_01 (IO_UHD_PROC_BASE + 0x804)
    #define R_R2Y_TABLE_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define REG_BYPASS_OP3 Fld(1,23,AC_MSKB2)//[23:23]
    #define R_R2Y_BYPASS Fld(1,22,AC_MSKB2)//[22:22]
    #define R_R2Y_S12IN_U10OUT Fld(1,21,AC_MSKB2)//[21:21]
    #define R_BYPASS_OD Fld(1,20,AC_MSKB2)//[20:20]
    #define REG_3DTV_OUT_SEL Fld(2,17,AC_MSKB2)//[18:17]
    #define REG_C_LINE_EXT Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_LPF_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_UVINV Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_YUV2YC_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define R_R2Y_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_MAIN_TOP_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_OP_SEL Fld(2,2,AC_MSKB0)//[3:2]
    #define C_MJC_MASK_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_MJC_IN_EN Fld(1,0,AC_MSKB0)//[0:0]
#define YCV_OFFSET_00 (IO_UHD_PROC_BASE + 0x808)
    #define R_YCV_PTGEN_CBAR Fld(1,30,AC_MSKB3)//[30:30]
    #define REG_PTGEN_PIP_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define REG_PTGEN_MAIN_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define CIN_OFFSET_MAIN Fld(12,16,AC_MSKW32)//[27:16]
    #define YIN_OFFSET_MAIN Fld(11,0,AC_MSKW10)//[10:0]
#define YCV_OFFSET_01 (IO_UHD_PROC_BASE + 0x80C)
    #define YIN_OFFSET_PIP Fld(11,16,AC_MSKW32)//[26:16]
    #define VIN_OFFSET_MAIN Fld(12,0,AC_MSKW10)//[11:0]
#define YCV_OFFSET_02 (IO_UHD_PROC_BASE + 0x810)
    #define VIN_OFFSET_PIP Fld(12,16,AC_MSKW32)//[27:16]
    #define CIN_OFFSET_PIP Fld(12,0,AC_MSKW10)//[11:0]
#define YCV_OFFSET_03 (IO_UHD_PROC_BASE + 0x814)
    #define M_YSUM Fld(32,0,AC_FULLDW)//[31:0]
#define YCV_OFFSET_04 (IO_UHD_PROC_BASE + 0x818)
    #define P_YSUM Fld(32,0,AC_FULLDW)//[31:0]
#define YCV_OFFSET_05 (IO_UHD_PROC_BASE + 0x81C)
    #define M_YMAX Fld(8,24,AC_FULLB3)//[31:24]
    #define P_YMAX Fld(8,16,AC_FULLB2)//[23:16]
    #define M_YMIN Fld(8,8,AC_FULLB1)//[15:8]
    #define P_YMIN Fld(8,0,AC_FULLB0)//[7:0]
#define YCBCR2RGB_00 (IO_UHD_PROC_BASE + 0x820)
    #define YUV2RGB_BYPASS_MAIN Fld(1,31,AC_MSKB3)//[31:31]
    #define YUV2RGB_BYPASS_PIP Fld(1,30,AC_MSKB3)//[30:30]
    #define Y2R_MATRIX00 Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX01 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_01 (IO_UHD_PROC_BASE + 0x824)
    #define Y2R_MATRIX10 Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX02 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_02 (IO_UHD_PROC_BASE + 0x828)
    #define Y2R_MATRIX12 Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX11 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_03 (IO_UHD_PROC_BASE + 0x82C)
    #define Y2R_MATRIX21 Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX20 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_04 (IO_UHD_PROC_BASE + 0x830)
    #define Y2R_MATRIX00_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX22 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_05 (IO_UHD_PROC_BASE + 0x834)
    #define Y2R_MATRIX02_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX01_PIP Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_06 (IO_UHD_PROC_BASE + 0x838)
    #define Y2R_MATRIX11_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX10_PIP Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_07 (IO_UHD_PROC_BASE + 0x83C)
    #define Y2R_MATRIX12_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX20_PIP Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_08 (IO_UHD_PROC_BASE + 0x840)
    #define Y2R_MATRIX22_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2R_MATRIX21_PIP Fld(14,0,AC_MSKW10)//[13:0]
#define RGBOFFSET_00 (IO_UHD_PROC_BASE + 0x844)
    #define R_OFFSET_PIP Fld(11,16,AC_MSKW32)//[26:16]
    #define R_OFFSET_MAIN Fld(11,0,AC_MSKW10)//[10:0]
#define RGBOFFSET_01 (IO_UHD_PROC_BASE + 0x848)
    #define G_OFFSET_PIP Fld(11,16,AC_MSKW32)//[26:16]
    #define G_OFFSET_MAIN Fld(11,0,AC_MSKW10)//[10:0]
#define RGBOFFSET_02 (IO_UHD_PROC_BASE + 0x84C)
    #define B_OFFSET_PIP Fld(11,16,AC_MSKW32)//[26:16]
    #define B_OFFSET_MAIN Fld(11,0,AC_MSKW10)//[10:0]
#define BLUESTCH_00 (IO_UHD_PROC_BASE + 0x850)
    #define C_BS_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define C_BS_SAT_THD Fld(7,24,AC_MSKB3)//[30:24]
    #define C_BS_Y_THD Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_BASE_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_ONE_GAIN_MODE Fld(1,3,AC_MSKB0)//[3:3]
    #define C_BS_EN_PIP Fld(1,2,AC_MSKB0)//[2:2]
    #define C_BS_INK_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define C_BS_EN_MAIN Fld(1,0,AC_MSKB0)//[0:0]
#define BLUESTCH_01 (IO_UHD_PROC_BASE + 0x854)
    #define C_BS_BASE Fld(8,24,AC_FULLB3)//[31:24]
    #define C_BS_GAIN_B Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_GAIN_G Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_GAIN_R Fld(8,0,AC_FULLB0)//[7:0]
#define MUTE_00 (IO_UHD_PROC_BASE + 0x858)
    #define B_BACKGROUND Fld(8,24,AC_FULLB3)//[31:24]
    #define G_BACKGROUND Fld(8,16,AC_FULLB2)//[23:16]
    #define R_BACKGROUND Fld(8,8,AC_FULLB1)//[15:8]
    #define R_MUTE_POST_SEL Fld(2,6,AC_MSKB0)//[7:6]
    #define R_P_MUTE_FRONT_SEL Fld(1,5,AC_MSKB0)//[5:5]
    #define R_M_MUTE_FRONT_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define R_MUTE_VS_LATCH_OFF Fld(1,3,AC_MSKB0)//[3:3]
    #define R_MUTE_POST_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define R_P_MUTE_FRONT_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define R_M_MUTE_FRONT_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MUTE_01 (IO_UHD_PROC_BASE + 0x85C)
    #define C_MUTE_FRAME_CNT Fld(3,28,AC_MSKB3)//[30:28]
    #define G_MUTE_MAIN Fld(12,16,AC_MSKW32)//[27:16]
    #define B_MUTE_MAIN Fld(12,0,AC_MSKW10)//[11:0]
#define MUTE_02 (IO_UHD_PROC_BASE + 0x860)
    #define B_MUTE_PIP Fld(12,16,AC_MSKW32)//[27:16]
    #define R_MUTE_MAIN Fld(12,0,AC_MSKW10)//[11:0]
#define MUTE_03 (IO_UHD_PROC_BASE + 0x864)
    #define R_MUTE_PIP Fld(12,16,AC_MSKW32)//[27:16]
    #define G_MUTE_PIP Fld(12,0,AC_MSKW10)//[11:0]
#define MUTE_04 (IO_UHD_PROC_BASE + 0x868)
    #define B_BACKGROUND_MJC Fld(8,24,AC_FULLB3)//[31:24]
    #define G_BACKGROUND_MJC Fld(8,16,AC_FULLB2)//[23:16]
    #define R_BACKGROUND_MJC Fld(8,8,AC_FULLB1)//[15:8]
    #define R_MUTE_MJC_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define R_MUTE_MJC_EN Fld(1,0,AC_MSKB0)//[0:0]
#define MUTE_05 (IO_UHD_PROC_BASE + 0x86C)
    #define G_MUTE_MAIN_MJC Fld(12,16,AC_MSKW32)//[27:16]
    #define B_MUTE_MAIN_MJC Fld(12,0,AC_MSKW10)//[11:0]
#define MUTE_06 (IO_UHD_PROC_BASE + 0x870)
    #define R_MUTE_MAIN_MJC Fld(12,0,AC_MSKW10)//[11:0]
#define WINDOW_00 (IO_UHD_PROC_BASE + 0x874)
    #define REG_WINDOW_2_ON Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_WINDOW_1_ON Fld(1,30,AC_MSKB3)//[30:30]
    #define WINDOW_COLOR1 Fld(30,0,AC_MSKDW)//[29:0]
#define WINDOW_01 (IO_UHD_PROC_BASE + 0x878)
    #define WINDOW_COLOR2 Fld(30,0,AC_MSKDW)//[29:0]
#define WINDOW_02 (IO_UHD_PROC_BASE + 0x87C)
    #define WINDOW_1_HPOSLR Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_1_HPOSLL Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_03 (IO_UHD_PROC_BASE + 0x880)
    #define WINDOW_1_HPOSRR Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_1_HPOSRL Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_04 (IO_UHD_PROC_BASE + 0x884)
    #define WINDOW_1_VPOSUD Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_1_VPOSUU Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_05 (IO_UHD_PROC_BASE + 0x888)
    #define WINDOW_1_VPOSDD Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_1_VPOSDU Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_06 (IO_UHD_PROC_BASE + 0x88C)
    #define WINDOW_2_HPOSLR Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_2_HPOSLL Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_07 (IO_UHD_PROC_BASE + 0x890)
    #define WINDOW_2_HPOSRR Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_2_HPOSRL Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_08 (IO_UHD_PROC_BASE + 0x894)
    #define WINDOW_2_VPOSUD Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_2_VPOSUU Fld(16,0,AC_FULLW10)//[15:0]
#define WINDOW_09 (IO_UHD_PROC_BASE + 0x898)
    #define WINDOW_2_VPOSDD Fld(16,16,AC_FULLW32)//[31:16]
    #define WINDOW_2_VPOSDU Fld(16,0,AC_FULLW10)//[15:0]
#define GAINOSET_00 (IO_UHD_PROC_BASE + 0x89C)
    #define RGB_GAIN_SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define R_GAIN_MAIN Fld(10,20,AC_MSKW32)//[29:20]
    #define G_GAIN_MAIN Fld(10,10,AC_MSKW21)//[19:10]
    #define B_GAIN_MAIN Fld(10,0,AC_MSKW10)//[9:0]
#define GAINOSET_01 (IO_UHD_PROC_BASE + 0x8A0)
    #define R_GAIN_PIP Fld(10,20,AC_MSKW32)//[29:20]
    #define G_GAIN_PIP Fld(10,10,AC_MSKW21)//[19:10]
    #define B_GAIN_PIP Fld(10,0,AC_MSKW10)//[9:0]
#define GAINOSET_02 (IO_UHD_PROC_BASE + 0x8A4)
    #define R_OFST1_MAIN Fld(10,22,AC_MSKW32)//[31:22]
    #define G_OFST1_MAIN Fld(10,12,AC_MSKW21)//[21:12]
    #define B_OFST1_MAIN Fld(10,2,AC_MSKW10)//[11:2]
#define GAINOSET_03 (IO_UHD_PROC_BASE + 0x8A8)
    #define R_OFST2_MAIN Fld(10,22,AC_MSKW32)//[31:22]
    #define G_OFST2_MAIN Fld(10,12,AC_MSKW21)//[21:12]
    #define B_OFST2_MAIN Fld(10,2,AC_MSKW10)//[11:2]
#define GAINOSET_04 (IO_UHD_PROC_BASE + 0x8AC)
    #define R_OFST1_PIP Fld(10,22,AC_MSKW32)//[31:22]
    #define G_OFST1_PIP Fld(10,12,AC_MSKW21)//[21:12]
    #define B_OFST1_PIP Fld(10,2,AC_MSKW10)//[11:2]
    #define REG_BG_GAIN_OFS Fld(2,0,AC_MSKB0)//[1:0]
#define GAINOSET_05 (IO_UHD_PROC_BASE + 0x8B0)
    #define R_OFST2_PIP Fld(10,22,AC_MSKW32)//[31:22]
    #define G_OFST2_PIP Fld(10,12,AC_MSKW21)//[21:12]
    #define B_OFST2_PIP Fld(10,2,AC_MSKW10)//[11:2]
#define MAXMETER_00 (IO_UHD_PROC_BASE + 0x8B4)
    #define C_MAX_METER_X_END Fld(13,13,AC_MSKDW)//[25:13]
    #define C_MAX_METER_X_START Fld(13,0,AC_MSKW10)//[12:0]
#define MAXMETER_01 (IO_UHD_PROC_BASE + 0x8B8)
    #define C_MAX_METER_SRC_FMT Fld(1,26,AC_MSKB3)//[26:26]
    #define C_MAX_METER_VS_POL Fld(1,25,AC_MSKB3)//[25:25]
    #define C_MAX_METER_HS_POL Fld(1,24,AC_MSKB3)//[24:24]
    #define C_MAX_METER_Y_END Fld(12,12,AC_MSKW21)//[23:12]
    #define C_MAX_METER_Y_START Fld(12,0,AC_MSKW10)//[11:0]
#define MAXMETER_02 (IO_UHD_PROC_BASE + 0x8BC)
    #define CNT00 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_03 (IO_UHD_PROC_BASE + 0x8C0)
    #define CNT01 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_04 (IO_UHD_PROC_BASE + 0x8C4)
    #define CNT02 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_05 (IO_UHD_PROC_BASE + 0x8C8)
    #define CNT03 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_06 (IO_UHD_PROC_BASE + 0x8CC)
    #define CNT04 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_07 (IO_UHD_PROC_BASE + 0x8D0)
    #define CNT05 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_08 (IO_UHD_PROC_BASE + 0x8D4)
    #define CNT06 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_09 (IO_UHD_PROC_BASE + 0x8D8)
    #define CNT07 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_10 (IO_UHD_PROC_BASE + 0x8DC)
    #define CNT08 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_11 (IO_UHD_PROC_BASE + 0x8E0)
    #define CNT09 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_12 (IO_UHD_PROC_BASE + 0x8E4)
    #define CNT10 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_13 (IO_UHD_PROC_BASE + 0x8E8)
    #define CNT11 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_14 (IO_UHD_PROC_BASE + 0x8EC)
    #define CNT12 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_15 (IO_UHD_PROC_BASE + 0x8F0)
    #define CNT13 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_16 (IO_UHD_PROC_BASE + 0x8F4)
    #define CNT14 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_17 (IO_UHD_PROC_BASE + 0x8F8)
    #define CNT15 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_18 (IO_UHD_PROC_BASE + 0x8FC)
    #define CNT16 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_19 (IO_UHD_PROC_BASE + 0x900)
    #define CNT17 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_20 (IO_UHD_PROC_BASE + 0x904)
    #define CNT18 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_21 (IO_UHD_PROC_BASE + 0x908)
    #define CNT19 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_22 (IO_UHD_PROC_BASE + 0x90C)
    #define CNT20 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_23 (IO_UHD_PROC_BASE + 0x910)
    #define CNT21 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_24 (IO_UHD_PROC_BASE + 0x914)
    #define CNT22 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_25 (IO_UHD_PROC_BASE + 0x918)
    #define CNT23 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_26 (IO_UHD_PROC_BASE + 0x91C)
    #define CNT24 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_27 (IO_UHD_PROC_BASE + 0x920)
    #define CNT25 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_28 (IO_UHD_PROC_BASE + 0x924)
    #define CNT26 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_29 (IO_UHD_PROC_BASE + 0x928)
    #define CNT27 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_30 (IO_UHD_PROC_BASE + 0x92C)
    #define CNT28 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_31 (IO_UHD_PROC_BASE + 0x930)
    #define CNT29 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_32 (IO_UHD_PROC_BASE + 0x934)
    #define CNT30 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_33 (IO_UHD_PROC_BASE + 0x938)
    #define CNT31 Fld(23,0,AC_MSKDW)//[22:0]
#define MAXMETER_34 (IO_UHD_PROC_BASE + 0x93C)
    #define CNT32 Fld(23,0,AC_MSKDW)//[22:0]
#define DBL_00 (IO_UHD_PROC_BASE + 0x940)
    #define DBL_ENABLE Fld(1,31,AC_MSKB3)//[31:31]
    #define G_SLOPE Fld(11,0,AC_MSKW10)//[10:0]
#define DBL_01 (IO_UHD_PROC_BASE + 0x944)
    #define G2 Fld(8,24,AC_FULLB3)//[31:24]
    #define G1 Fld(8,16,AC_FULLB2)//[23:16]
    #define P2 Fld(8,8,AC_FULLB1)//[15:8]
    #define P1 Fld(8,0,AC_FULLB0)//[7:0]
#define GAMMA_00 (IO_UHD_PROC_BASE + 0x948)
    #define C_GM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_GM_SWAP_POST Fld(1,28,AC_MSKB3)//[28:28]
    #define R_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define R_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define R_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define R_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define R_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define R_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define R_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define GAMMA_01 (IO_UHD_PROC_BASE + 0x94C)
    #define CPU_SRAM_ADDR Fld(8,16,AC_FULLB2)//[23:16]
    #define R_ADDR Fld(8,0,AC_FULLB0)//[7:0]
#define GAMMA_02 (IO_UHD_PROC_BASE + 0x950)
    #define R_WDATA_G Fld(12,16,AC_MSKW32)//[27:16]
    #define R_WDATA_R Fld(12,0,AC_MSKW10)//[11:0]
#define GAMMA_03 (IO_UHD_PROC_BASE + 0x954)
    #define R_WDATA_B Fld(12,0,AC_MSKW10)//[11:0]
#define GAMMA_04 (IO_UHD_PROC_BASE + 0x958)
    #define R_RDATA_G Fld(12,16,AC_MSKW32)//[27:16]
    #define R_RDATA_R Fld(12,0,AC_MSKW10)//[11:0]
#define GAMMA_05 (IO_UHD_PROC_BASE + 0x95C)
    #define R_RDATA_B Fld(12,0,AC_MSKW10)//[11:0]
#define GAMMA_06 (IO_UHD_PROC_BASE + 0x960)
    #define CPU_SEL_SRAM_0 Fld(1,7,AC_MSKB0)//[7:7]
    #define R_CPURW_ACTIVE Fld(1,6,AC_MSKB0)//[6:6]
    #define TABLE_WAIT_VSYNC Fld(1,5,AC_MSKB0)//[5:5]
    #define R_FILL_TABLE_READY Fld(1,4,AC_MSKB0)//[4:4]
    #define READ_FINISH Fld(1,3,AC_MSKB0)//[3:3]
    #define R_READ Fld(1,2,AC_MSKB0)//[2:2]
    #define WRITE_FINISH Fld(1,1,AC_MSKB0)//[1:1]
    #define R_WRITE Fld(1,0,AC_MSKB0)//[0:0]
#define GAMMA_09 (IO_UHD_PROC_BASE + 0x96C)
    #define C_GM_256_R Fld(13,0,AC_MSKW10)//[12:0]
#define GAMMA_10 (IO_UHD_PROC_BASE + 0x970)
    #define C_GM_256_B Fld(13,16,AC_MSKW32)//[28:16]
    #define C_GM_256_G Fld(13,0,AC_MSKW10)//[12:0]
#define OSD_MIX_0 (IO_UHD_PROC_BASE + 0x984)
    #define REG_MIX3_BOT_ALPHA Fld(8,20,AC_MSKW32)//[27:20]
    #define REG_MIX_SIGN_AUTO_OFF Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_MIX1_SIGN_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_MIX2_SIGN_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_MIX3_SIGN_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define MIX3_BYPASS Fld(1,13,AC_MSKB1)//[13:13]
    #define MIX3_MASK_OR_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define MIX3_TOP_MIX_BG Fld(1,11,AC_MSKB1)//[11:11]
    #define MIX2_BYPASS Fld(1,10,AC_MSKB1)//[10:10]
    #define MIX2_MASK_OR_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define MIX2_TOP_MIX_BG Fld(1,8,AC_MSKB1)//[8:8]
    #define MIX12_BYPASS Fld(1,7,AC_MSKB0)//[7:7]
    #define MIX12_MASK_OR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define MIX12_TOP_MIX_BG Fld(1,5,AC_MSKB0)//[5:5]
    #define MIX11_BYPASS Fld(1,4,AC_MSKB0)//[4:4]
    #define MIX11_MASK_OR_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define MIX11_TOP_MIX_BG Fld(1,2,AC_MSKB0)//[2:2]
    #define BOT_MASK_SEL Fld(1,1,AC_MSKB0)//[1:1]
#define OSD_MIX_1 (IO_UHD_PROC_BASE + 0x988)
    #define REG_MIX2_BOT_ALPHA Fld(8,24,AC_FULLB3)//[31:24]
    #define REG_MIX12_BOT_ALPHA Fld(8,16,AC_FULLB2)//[23:16]
    #define REG_MIX11_BOT_ALPHA Fld(8,8,AC_FULLB1)//[15:8]
    #define REG_MIX3_SEL_BOT_ALPHA Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_MIX2_SEL_BOT_ALPHA Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_MIX12_SEL_BOT_ALPHA Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_MIX11_SEL_BOT_ALPHA Fld(1,0,AC_MSKB0)//[0:0]
#define OS_DITHER_00 (IO_UHD_PROC_BASE + 0x98C)
    #define REG_FPHASE_BIT Fld(3,29,AC_MSKB3)//[31:29]
    #define REG_FPHASE_SEL Fld(2,27,AC_MSKB3)//[28:27]
    #define REG_FPHASE_CTRL Fld(2,25,AC_MSKB3)//[26:25]
    #define FPHASE Fld(6,19,AC_MSKW32)//[24:19]
    #define REG_FPHASE_R Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_FPHASE_EN Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_SUB_B Fld(2,15,AC_MSKW21)//[16:15]
    #define REG_SUB_G Fld(2,13,AC_MSKB1)//[14:13]
    #define REG_SUB_R Fld(2,11,AC_MSKB1)//[12:11]
    #define REG_SUBPIX_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_TABLE_EN Fld(2,8,AC_MSKB1)//[9:8]
    #define DITHER_LFSR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define RDITHER_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define EDITHER_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define ROUND_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_LEFT_EN Fld(1,2,AC_MSKB0)//[2:2]
#define OS_DITHER_01 (IO_UHD_PROC_BASE + 0x990)
    #define REG_INK_DATA_G Fld(12,16,AC_MSKW32)//[27:16]
    #define REG_INK_DATA_R Fld(12,0,AC_MSKW10)//[11:0]
#define OS_DITHER_02 (IO_UHD_PROC_BASE + 0x994)
    #define REG_INK Fld(1,31,AC_MSKB3)//[31:31]
    #define LSB_OFF Fld(1,30,AC_MSKB3)//[30:30]
    #define DRMOD_B Fld(2,16,AC_MSKB2)//[17:16]
    #define DRMOD_G Fld(2,14,AC_MSKB1)//[15:14]
    #define DRMOD_R Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_INK_DATA_B Fld(12,0,AC_MSKW10)//[11:0]
#define OS_DITHER_03 (IO_UHD_PROC_BASE + 0x998)
    #define REG_FPHASE_BIT_POST Fld(3,29,AC_MSKB3)//[31:29]
    #define REG_FPHASE_SEL_POST Fld(2,27,AC_MSKB3)//[28:27]
    #define REG_FPHASE_CTRL_POST Fld(2,25,AC_MSKB3)//[26:25]
    #define REG_FPHASE_POST Fld(6,19,AC_MSKW32)//[24:19]
    #define REG_FPHASE_R_POST Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_FPHASE_EN_POST Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_SUB_B_POST Fld(2,15,AC_MSKW21)//[16:15]
    #define REG_SUB_G_POST Fld(2,13,AC_MSKB1)//[14:13]
    #define REG_SUB_R_POST Fld(2,11,AC_MSKB1)//[12:11]
    #define REG_SUBPIX_EN_POST Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_TABLE_EN_POST Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_LFSR_EN_POST Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_RDITHER_EN_POST Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_EDITHER_EN_POST Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_ROUND_EN_POST Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_LEFT_EN_POST Fld(1,2,AC_MSKB0)//[2:2]
#define OS_DITHER_04 (IO_UHD_PROC_BASE + 0x99C)
    #define REG_INK_DATA_G_POST Fld(12,16,AC_MSKW32)//[27:16]
    #define REG_INK_DATA_R_POST Fld(12,0,AC_MSKW10)//[11:0]
#define OS_DITHER_05 (IO_UHD_PROC_BASE + 0x9A0)
    #define REG_INK_POST Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_LSB_OFF_POST Fld(1,30,AC_MSKB3)//[30:30]
    #define REG_DRMOD_B_POST Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_DRMOD_G_POST Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_DRMOD_R_POST Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_INK_DATA_B_POST Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_00 (IO_UHD_PROC_BASE + 0x9A4)
    #define BSEL Fld(2,8,AC_MSKB1)//[9:8]
    #define GSEL Fld(2,6,AC_MSKB0)//[7:6]
    #define RSEL Fld(2,4,AC_MSKB0)//[5:4]
    #define RB_MSBINV Fld(1,3,AC_MSKB0)//[3:3]
    #define MLSB_INV Fld(1,2,AC_MSKB0)//[2:2]
    #define RB_INV Fld(1,1,AC_MSKB0)//[1:1]
    #define DUMP_EN Fld(1,0,AC_MSKB0)//[0:0]
#define DUMP_01 (IO_UHD_PROC_BASE + 0x9A8)
    #define POS_Y Fld(16,16,AC_FULLW32)//[31:16]
    #define POS_X Fld(16,0,AC_FULLW10)//[15:0]
#define DUMP_02 (IO_UHD_PROC_BASE + 0x9AC)
    #define POS_G Fld(12,16,AC_MSKW32)//[27:16]
    #define POS_R Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_03 (IO_UHD_PROC_BASE + 0x9B0)
    #define POS_B Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_04 (IO_UHD_PROC_BASE + 0x9B4)
    #define POS_OUT_G Fld(12,16,AC_MSKW32)//[27:16]
    #define POS_OUT_R Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_05 (IO_UHD_PROC_BASE + 0x9B8)
    #define POS_OUT_B Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_06 (IO_UHD_PROC_BASE + 0x9BC)
    #define R_CMI_3DTV_1ST_G Fld(12,16,AC_MSKW32)//[27:16]
    #define R_CMI_3DTV_1ST_R Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_07 (IO_UHD_PROC_BASE + 0x9C0)
    #define R_CMI_3DTV_2ND_R Fld(12,16,AC_MSKW32)//[27:16]
    #define R_CMI_3DTV_1ST_B Fld(12,0,AC_MSKW10)//[11:0]
#define DUMP_08 (IO_UHD_PROC_BASE + 0x9C4)
    #define R_CMI_3DTV_FMT_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define R_CMI_3DTV_AUTO_DIS Fld(1,30,AC_MSKB3)//[30:30]
    #define R_CMI_3DTV_2ND_B Fld(12,16,AC_MSKW32)//[27:16]
    #define R_CMI_3DTV_2ND_G Fld(12,0,AC_MSKW10)//[11:0]
#define PTGEN_00 (IO_UHD_PROC_BASE + 0x9C8)
    #define SETRES_UPD_SEL Fld(1,28,AC_MSKB3)//[28:28]
    #define SETRES_SRC_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define CSSEL Fld(1,26,AC_MSKB3)//[26:26]
    #define VSYNC_DELSEL2 Fld(3,23,AC_MSKW32)//[25:23]
    #define VSYNC_DELSEL Fld(3,20,AC_MSKB2)//[22:20]
    #define DEN_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define VS_SEL Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_VSYNC_OUT_SEL Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_VSYNC_SEL Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_PTGEN_HV_MODE Fld(2,12,AC_MSKB1)//[13:12]
    #define VCOUNT_ENABLE Fld(1,11,AC_MSKB1)//[11:11]
    #define HCOUNT_ENABLE Fld(1,10,AC_MSKB1)//[10:10]
    #define VSYNCP Fld(1,9,AC_MSKB1)//[9:9]
    #define HSYNCP Fld(1,8,AC_MSKB1)//[8:8]
    #define DENP Fld(1,7,AC_MSKB0)//[7:7]
    #define DENTPY Fld(1,6,AC_MSKB0)//[6:6]
    #define PATGEN_SEL Fld(2,4,AC_MSKB0)//[5:4]
    #define PATGEN_RSEL Fld(3,1,AC_MSKB0)//[3:1]
    #define PATGEN_EN Fld(1,0,AC_MSKB0)//[0:0]
#define PTGEN_01 (IO_UHD_PROC_BASE + 0x9CC)
    #define REG_VSYNC_OUT_H Fld(16,16,AC_FULLW32)//[31:16]
    #define HS_WIDTH Fld(16,0,AC_FULLW10)//[15:0]
#define PTGEN_02 (IO_UHD_PROC_BASE + 0x9D0)
    #define V_POS Fld(16,16,AC_FULLW32)//[31:16]
    #define H_POS Fld(16,0,AC_FULLW10)//[15:0]
#define PTGEN_03 (IO_UHD_PROC_BASE + 0x9D4)
    #define VS_WIDTH Fld(4,0,AC_MSKB0)//[3:0]
#define CRC32_00 (IO_UHD_PROC_BASE + 0x9D8)
    #define R_CRC_STILL_CHECK_DONE Fld(1,20,AC_MSKB2)//[20:20]
    #define R_CRC_NON_STILL_CNT Fld(4,16,AC_MSKB2)//[19:16]
    #define R_CRC_STILL_CHECK_MAX Fld(8,8,AC_FULLB1)//[15:8]
    #define R_CRC_STILL_CHECK_TRIG Fld(1,7,AC_MSKB0)//[7:7]
    #define R_CRC_3D_L_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define R_CRC_3D_R_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define R_CRC_MUX_SEL Fld(5,0,AC_MSKB0)//[4:0]
#define CRC32_01 (IO_UHD_PROC_BASE + 0x9DC)
    #define R_CRC_H_END Fld(13,16,AC_MSKW32)//[28:16]
    #define R_CRC_H_START Fld(13,0,AC_MSKW10)//[12:0]
#define CRC32_02 (IO_UHD_PROC_BASE + 0x9E0)
    #define R_CRC_V_END Fld(12,16,AC_MSKW32)//[27:16]
    #define R_CRC_V_START Fld(12,0,AC_MSKW10)//[11:0]
#define CRC32_03 (IO_UHD_PROC_BASE + 0x9E4)
    #define R_CRC_V_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define R_CRC_C_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define R_CRC_Y_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define CRC32_04 (IO_UHD_PROC_BASE + 0x9E8)
    #define R_CRC_OUT Fld(32,0,AC_FULLDW)//[31:0]
#define DEBUG_00 (IO_UHD_PROC_BASE + 0x9EC)
    #define INPUT_HACTIVE Fld(16,16,AC_FULLW32)//[31:16]
    #define REG_OUT_MON_SEL Fld(4,12,AC_MSKB1)//[15:12]
    #define REG_IN_MON_SEL Fld(4,8,AC_MSKB1)//[11:8]
    #define R_DEBUG_OSTG_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define R_CRC_SEL_GC Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_OUT_MON_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_IN_MON_EN Fld(1,4,AC_MSKB0)//[4:4]
#define DEBUG_01 (IO_UHD_PROC_BASE + 0x9F0)
    #define PANEL_DISP_R Fld(1,24,AC_MSKB3)//[24:24]
    #define R_OUT_SEL_OTHER Fld(2,2,AC_MSKB0)//[3:2]
    #define R_HVM_INPUT_SEL Fld(2,0,AC_MSKB0)//[1:0]
#define DEBUG_02 (IO_UHD_PROC_BASE + 0x9F4)
    #define INPUT_VTOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define INPUT_HTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define DEBUG_03 (IO_UHD_PROC_BASE + 0x9F8)
    #define HVM_VTOTAL_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define HVM_HTOTAL_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define DEBUG_04 (IO_UHD_PROC_BASE + 0x9FC)
    #define HVM_VDE_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define HVM_HDE_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define SPARE_00 (IO_UHD_PROC_BASE + 0xA04)
    #define OSTG_SPARE_REG0 Fld(32,0,AC_FULLDW)//[31:0]
#define SPARE_01 (IO_UHD_PROC_BASE + 0xA08)
    #define OSTG_SPARE_REG1 Fld(32,0,AC_FULLDW)//[31:0]
#define OSD_TIMING_00 (IO_UHD_PROC_BASE + 0xA14)
    #define OSD_VS_END Fld(3,29,AC_MSKB3)//[31:29]
    #define OSD_VS_START Fld(3,26,AC_MSKB3)//[28:26]
    #define OSD_HS_END Fld(10,16,AC_MSKW32)//[25:16]
    #define OSD_HS_START Fld(8,8,AC_FULLB1)//[15:8]
    #define OSD_HS_SEL Fld(2,6,AC_MSKB0)//[7:6]
    #define OSD_VS_SEL Fld(2,4,AC_MSKB0)//[5:4]
    #define OSD_HSYNCP Fld(1,2,AC_MSKB0)//[2:2]
    #define OSD_VSYNCP Fld(1,1,AC_MSKB0)//[1:1]
    #define VSOSDA1 Fld(1,0,AC_MSKB0)//[0:0]
#define OSD_TIMING_01 (IO_UHD_PROC_BASE + 0xA18)
    #define TOP_MASK Fld(10,16,AC_MSKW32)//[25:16]
    #define BOTTOM_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define OSD_TIMING_02 (IO_UHD_PROC_BASE + 0xA1C)
    #define LEFT_MASK Fld(10,16,AC_MSKW32)//[25:16]
    #define RIGHT_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define OSD_TIMING_03 (IO_UHD_PROC_BASE + 0xA20)
    #define REG_VSYNC_SEL_OSD Fld(2,24,AC_MSKB3)//[25:24]
    #define REG_VSYNC_OUT_SEL_OSD Fld(2,22,AC_MSKB2)//[23:22]
    #define REG_VCOUNT_ENABLE_OSD Fld(1,21,AC_MSKB2)//[21:21]
    #define REG_HCOUNT_ENABLE_OSD Fld(1,20,AC_MSKB2)//[20:20]
    #define BOTTOM_MASK_OSD Fld(10,10,AC_MSKW21)//[19:10]
    #define TOP_MASK_OSD Fld(10,0,AC_MSKW10)//[9:0]
#define OSD_TIMING_04 (IO_UHD_PROC_BASE + 0xA24)
    #define LEFT_MASK_OSD Fld(10,10,AC_MSKW21)//[19:10]
    #define RIGHT_MASK_OSD Fld(10,0,AC_MSKW10)//[9:0]
#define OSD_TIMING_05 (IO_UHD_PROC_BASE + 0xA28)
    #define REG_V_OUT_H_OSD Fld(16,0,AC_FULLW10)//[15:0]
#define OSD_MIX_2 (IO_UHD_PROC_BASE + 0xA30)
    #define REG_OSD_INDEX_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_OSD_INDEX_THD Fld(8,0,AC_FULLB0)//[7:0]
#define BLUESTCH_02 (IO_UHD_PROC_BASE + 0xA34)
    #define C_BS_GAIN_B_1 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_GAIN_G_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_GAIN_R_1 Fld(8,0,AC_FULLB0)//[7:0]
#define ANAG_CTRL (IO_UHD_PROC_BASE + 0xA38)
    #define ANAG_DIS_VALUE Fld(8,8,AC_FULLB1)//[15:8]
    #define ANAG_REYE_B_DIS Fld(1,7,AC_MSKB0)//[7:7]
    #define ANAG_REYE_G_DIS Fld(1,6,AC_MSKB0)//[6:6]
    #define ANAG_REYE_R_DIS Fld(1,5,AC_MSKB0)//[5:5]
    #define ANAG_LEYE_B_DIS Fld(1,4,AC_MSKB0)//[4:4]
    #define ANAG_LEYE_G_DIS Fld(1,3,AC_MSKB0)//[3:3]
    #define ANAG_LEYE_R_DIS Fld(1,2,AC_MSKB0)//[2:2]
    #define ANAG_REF_ODDEVEN Fld(1,1,AC_MSKB0)//[1:1]
    #define ANAG_3D_EN Fld(1,0,AC_MSKB0)//[0:0]
#define BLUESTCH_03 (IO_UHD_PROC_BASE + 0xA3C)
    #define C_BS_BASE_2 Fld(8,24,AC_FULLB3)//[31:24]
    #define C_BS_GAIN_B_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_GAIN_G_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_GAIN_R_2 Fld(8,0,AC_FULLB0)//[7:0]
#define BLUESTCH_04 (IO_UHD_PROC_BASE + 0xA40)
    #define C_BS_BASE_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define C_BS_GAIN_B_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_GAIN_G_3 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_GAIN_R_3 Fld(8,0,AC_FULLB0)//[7:0]
#define BLUESTCH_05 (IO_UHD_PROC_BASE + 0xA44)
    #define C_BS_BASE_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define C_BS_GAIN_B_4 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BS_GAIN_G_4 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BS_GAIN_R_4 Fld(8,0,AC_FULLB0)//[7:0]
#define GC_MIX_0 (IO_UHD_PROC_BASE + 0xA48)
    #define GC_MIX3_BOT_ALPHA Fld(8,20,AC_MSKW32)//[27:20]
    #define GC_MIX_SIGN_AUTO_OFF Fld(1,17,AC_MSKB2)//[17:17]
    #define GC_MIX1_SIGN_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define GC_MIX2_SIGN_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define GC_MIX3_SIGN_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define GC_MIX3_BYPASS Fld(1,13,AC_MSKB1)//[13:13]
    #define GC_MIX3_MASK_OR_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define GC_MIX3_TOP_MIX_BG Fld(1,11,AC_MSKB1)//[11:11]
    #define GC_MIX2_BYPASS Fld(1,10,AC_MSKB1)//[10:10]
    #define GC_MIX2_MASK_OR_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define GC_MIX2_TOP_MIX_BG Fld(1,8,AC_MSKB1)//[8:8]
    #define GC_MIX12_BYPASS Fld(1,7,AC_MSKB0)//[7:7]
    #define GC_MIX12_MASK_OR_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define GC_MIX12_TOP_MIX_BG Fld(1,5,AC_MSKB0)//[5:5]
    #define GC_MIX11_BYPASS Fld(1,4,AC_MSKB0)//[4:4]
    #define GC_MIX11_MASK_OR_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define GC_MIX11_TOP_MIX_BG Fld(1,2,AC_MSKB0)//[2:2]
    #define GC_BOT_MASK_SEL Fld(1,1,AC_MSKB0)//[1:1]
#define GC_MIX_1 (IO_UHD_PROC_BASE + 0xA4C)
    #define GC_MIX2_BOT_ALPHA Fld(8,24,AC_FULLB3)//[31:24]
    #define GC_MIX12_BOT_ALPHA Fld(8,16,AC_FULLB2)//[23:16]
    #define GC_MIX11_BOT_ALPHA Fld(8,8,AC_FULLB1)//[15:8]
    #define GC_MIX3_SEL_BOT_ALPHA Fld(1,3,AC_MSKB0)//[3:3]
    #define GC_MIX2_SEL_BOT_ALPHA Fld(1,2,AC_MSKB0)//[2:2]
    #define GC_MIX12_SEL_BOT_ALPHA Fld(1,1,AC_MSKB0)//[1:1]
    #define GC_MIX11_SEL_BOT_ALPHA Fld(1,0,AC_MSKB0)//[0:0]
#define GC_MIX_2 (IO_UHD_PROC_BASE + 0xA50)
    #define GC_VIDEO_DATA Fld(8,16,AC_FULLB2)//[23:16]
    #define GC_ALPHA Fld(1,12,AC_MSKB1)//[12:12]
    #define GC_ALPHA_SEL_MODE Fld(1,11,AC_MSKB1)//[11:11]
    #define GC_TIMING_SEL_MAIN Fld(1,10,AC_MSKB1)//[10:10]
    #define GC_VIDEO_SEL_MAIN Fld(1,9,AC_MSKB1)//[9:9]
    #define GC_OSD_INDEX_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define GC_OSD_INDEX_THD Fld(8,0,AC_FULLB0)//[7:0]
#define OS_DITHER_06 (IO_UHD_PROC_BASE + 0xA54)
    #define REG_FPHASE_BIT_GC Fld(3,29,AC_MSKB3)//[31:29]
    #define REG_FPHASE_SEL_GC Fld(2,27,AC_MSKB3)//[28:27]
    #define REG_FPHASE_CTRL_GC Fld(2,25,AC_MSKB3)//[26:25]
    #define REG_FPHASE_GC Fld(6,19,AC_MSKW32)//[24:19]
    #define REG_FPHASE_R_GC Fld(1,18,AC_MSKB2)//[18:18]
    #define REG_FPHASE_EN_GC Fld(1,17,AC_MSKB2)//[17:17]
    #define REG_SUB_B_GC Fld(2,15,AC_MSKW21)//[16:15]
    #define REG_SUB_G_GC Fld(2,13,AC_MSKB1)//[14:13]
    #define REG_SUB_R_GC Fld(2,11,AC_MSKB1)//[12:11]
    #define REG_SUBPIX_EN_GC Fld(1,10,AC_MSKB1)//[10:10]
    #define REG_TABLE_EN_GC Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_LFSR_EN_GC Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_RDITHER_EN_GC Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_EDITHER_EN_GC Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_ROUND_EN_GC Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_LEFT_EN_GC Fld(1,2,AC_MSKB0)//[2:2]
#define OS_DITHER_07 (IO_UHD_PROC_BASE + 0xA58)
    #define REG_INK_DATA_G_GC Fld(12,16,AC_MSKW32)//[27:16]
    #define REG_INK_DATA_R_GC Fld(12,0,AC_MSKW10)//[11:0]
#define OS_DITHER_08 (IO_UHD_PROC_BASE + 0xA5C)
    #define REG_INK_GC Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_LSB_OFF_GC Fld(1,30,AC_MSKB3)//[30:30]
    #define REG_DRMOD_B_GC Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_DRMOD_G_GC Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_DRMOD_R_GC Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_INK_DATA_B_GC Fld(12,0,AC_MSKW10)//[11:0]
#define GC_PTG (IO_UHD_PROC_BASE + 0xA60)
    #define GC_PTG_ALPHA Fld(8,24,AC_FULLB3)//[31:24]
    #define GC_PTG_COLOR Fld(8,16,AC_FULLB2)//[23:16]
    #define GC_PTG_4K Fld(1,9,AC_MSKB1)//[9:9]
    #define GC_PTG_BINK_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define GC_PTG_EN_CBAR Fld(1,7,AC_MSKB0)//[7:7]
    #define GC_PTG_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define GC_MASK_NONDE Fld(1,5,AC_MSKB0)//[5:5]
    #define GC_SOFT_MUTE_LATCHOFF Fld(1,4,AC_MSKB0)//[4:4]
    #define GC_SOFT_MUTE_FRAME_CNT Fld(3,1,AC_MSKB0)//[3:1]
    #define GC_SOFT_MUTE Fld(1,0,AC_MSKB0)//[0:0]
#define YCBCR2RGB_09 (IO_UHD_PROC_BASE + 0xA64)
    #define R_BT2020_CL_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define Y2RMATRIX02_BT2020_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2RMATRIX02_BT2020 Fld(14,0,AC_MSKW10)//[13:0]
#define YCBCR2RGB_10 (IO_UHD_PROC_BASE + 0xA68)
    #define Y2RMATRIX21_BT2020_PIP Fld(14,16,AC_MSKW32)//[29:16]
    #define Y2RMATRIX21_BT2020 Fld(14,0,AC_MSKW10)//[13:0]
#define PREMJC_INTR (IO_UHD_PROC_BASE + 0xA6C)
    #define R_PREMJC_INTR_DEBUG Fld(1,31,AC_MSKB3)//[31:31]
    #define R_PREMJC_INTR_TYPE Fld(1,17,AC_MSKB2)//[17:17]
    #define R_PREMJC_INTR_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define R_PREMJC_INTR_LINE Fld(16,0,AC_FULLW10)//[15:0]

//Page AMBL
#define AMBL_00 (IO_UHD_PROC_BASE + 0xB00)
    #define R_AMBI_INTR_MASK_MODE Fld(2,12,AC_MSKB1)//[13:12]
    #define R_AMBI_SHIFT_N Fld(4,8,AC_MSKB1)//[11:8]
    #define R_AMBI_START_BLKNUM Fld(5,3,AC_MSKB0)//[7:3]
#define AMBL_01 (IO_UHD_PROC_BASE + 0xB04)
    #define R_AMBI_BLK_H_LAST Fld(8,24,AC_FULLB3)//[31:24]
    #define R_AMBI_BLK_W_LAST Fld(8,16,AC_FULLB2)//[23:16]
    #define R_AMBI_BLK_H Fld(8,8,AC_FULLB1)//[15:8]
    #define R_AMBI_BLK_W Fld(8,0,AC_FULLB0)//[7:0]
#define AMBL_02 (IO_UHD_PROC_BASE + 0xB08)
    #define AMBI_RESULT_R Fld(16,16,AC_FULLW32)//[31:16]
    #define AMBI_INTR_DEBUG Fld(1,10,AC_MSKB1)//[10:10]
    #define AMBI_BLKNUM Fld(5,5,AC_MSKW10)//[9:5]
    #define AMBI_STRAP_ID Fld(5,0,AC_MSKB0)//[4:0]
#define AMBL_03 (IO_UHD_PROC_BASE + 0xB0C)
    #define AMBI_RESULT_B Fld(16,16,AC_FULLW32)//[31:16]
    #define AMBI_RESULT_G Fld(16,0,AC_FULLW10)//[15:0]

//Page XVYCC
#define XVYCC_00 (IO_UHD_PROC_BASE + 0xC00)
    #define REG_3X3_DISABLE Fld(1,31,AC_MSKB3)//[31:31]
    #define REG_DISABLE_AG Fld(1,30,AC_MSKB3)//[30:30]
    #define REG_DISABLE_G Fld(1,29,AC_MSKB3)//[29:29]
    #define REG_DISABLE_XVYCC_MAIN Fld(1,28,AC_MSKB3)//[28:28]
    #define REG_XVYCC_IN1 Fld(1,25,AC_MSKB3)//[25:25]
    #define REG_XVYCC_M_P_SW Fld(1,24,AC_MSKB3)//[24:24]
    #define REG_XVYCC_BT2020_Y_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define REG_ROUND_12TO10 Fld(1,15,AC_MSKB1)//[15:15]
    #define REG_XVYCC_GM_SRGB Fld(1,14,AC_MSKB1)//[14:14]
    #define REG_XVYCC_AGM_SRGB Fld(1,13,AC_MSKB1)//[13:13]
    #define REG_BYPASS_XV_IN2OUT Fld(1,12,AC_MSKB1)//[12:12]
    #define REG_DISABLE_XVYCC_PIP Fld(1,11,AC_MSKB1)//[11:11]
#define XVYCC_05 (IO_UHD_PROC_BASE + 0xC14)
    #define REG_XV_MATRIX00 Fld(15,16,AC_MSKW32)//[30:16]
#define XVYCC_06 (IO_UHD_PROC_BASE + 0xC18)
    #define REG_XV_MATRIX02 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XV_MATRIX01 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_07 (IO_UHD_PROC_BASE + 0xC1C)
    #define REG_XV_MATRIX11 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XV_MATRIX10 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_08 (IO_UHD_PROC_BASE + 0xC20)
    #define REG_XV_MATRIX20 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XV_MATRIX12 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_09 (IO_UHD_PROC_BASE + 0xC24)
    #define REG_XV_MATRIX22 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XV_MATRIX21 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_10 (IO_UHD_PROC_BASE + 0xC28)
    #define REG_GAMMA_G_SRAM_4096 Fld(12,16,AC_MSKW32)//[27:16]
    #define REG_GAMMA_R_SRAM_4096 Fld(12,0,AC_MSKW10)//[11:0]
#define XVYCC_11 (IO_UHD_PROC_BASE + 0xC2C)
    #define REG_GAMMA_B_SRAM_4096 Fld(12,0,AC_MSKW10)//[11:0]
#define XVYCC_12 (IO_UHD_PROC_BASE + 0xC30)
    #define REG_XV_MATRIX22_MSB Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_XV_MATRIX21_MSB Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_XV_MATRIX20_MSB Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_XV_MATRIX12_MSB Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_XV_MATRIX11_MSB Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_XV_MATRIX10_MSB Fld(2,6,AC_MSKB0)//[7:6]
    #define REG_XV_MATRIX02_MSB Fld(2,4,AC_MSKB0)//[5:4]
    #define REG_XV_MATRIX01_MSB Fld(2,2,AC_MSKB0)//[3:2]
    #define REG_XV_MATRIX00_MSB Fld(2,0,AC_MSKB0)//[1:0]
#define XVYCC_15 (IO_UHD_PROC_BASE + 0xC3C)
    #define REG_XVS_MATRIX22_MSB Fld(2,16,AC_MSKB2)//[17:16]
    #define REG_XVS_MATRIX21_MSB Fld(2,14,AC_MSKB1)//[15:14]
    #define REG_XVS_MATRIX20_MSB Fld(2,12,AC_MSKB1)//[13:12]
    #define REG_XVS_MATRIX12_MSB Fld(2,10,AC_MSKB1)//[11:10]
    #define REG_XVS_MATRIX11_MSB Fld(2,8,AC_MSKB1)//[9:8]
    #define REG_XVS_MATRIX10_MSB Fld(2,6,AC_MSKB0)//[7:6]
    #define REG_XVS_MATRIX02_MSB Fld(2,4,AC_MSKB0)//[5:4]
    #define REG_XVS_MATRIX01_MSB Fld(2,2,AC_MSKB0)//[3:2]
    #define REG_XVS_MATRIX00_MSB Fld(2,0,AC_MSKB0)//[1:0]
#define XVYCC_17 (IO_UHD_PROC_BASE + 0xC44)
    #define REG_XVS_3X3_00 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XVS_BT2020_Y_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define REG_XVS_CLIP_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define REG_XVS_ROUND_12TO10 Fld(1,7,AC_MSKB0)//[7:7]
    #define REG_XVS_GM_SRGB Fld(1,6,AC_MSKB0)//[6:6]
    #define REG_XVS_AGM_SRGB Fld(1,5,AC_MSKB0)//[5:5]
    #define REG_XVS_3X3_DISABLE Fld(1,4,AC_MSKB0)//[4:4]
    #define REG_XVS_DISABLE_AG Fld(1,3,AC_MSKB0)//[3:3]
    #define REG_XVS_DISABLE_G Fld(1,2,AC_MSKB0)//[2:2]
    #define REG_XVS_DISABLE Fld(1,1,AC_MSKB0)//[1:1]
    #define REG_XVS_BYPASS Fld(1,0,AC_MSKB0)//[0:0]
#define XVYCC_18 (IO_UHD_PROC_BASE + 0xC48)
    #define REG_XVS_3X3_02 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XVS_3X3_01 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_19 (IO_UHD_PROC_BASE + 0xC4C)
    #define REG_XVS_3X3_11 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XVS_3X3_10 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_20 (IO_UHD_PROC_BASE + 0xC50)
    #define REG_XVS_3X3_20 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XVS_3X3_12 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_21 (IO_UHD_PROC_BASE + 0xC54)
    #define REG_XVS_3X3_22 Fld(15,16,AC_MSKW32)//[30:16]
    #define REG_XVS_3X3_21 Fld(15,0,AC_MSKW10)//[14:0]
#define XVYCC_22 (IO_UHD_PROC_BASE + 0xC58)
    #define REG_XVS_GM_G_4096 Fld(12,16,AC_MSKW32)//[27:16]
    #define REG_XVS_GM_R_4096 Fld(12,0,AC_MSKW10)//[11:0]
#define XVYCC_23 (IO_UHD_PROC_BASE + 0xC5C)
    #define REG_XVS_GM_B_4096 Fld(12,0,AC_MSKW10)//[11:0]

//Page LOCDIM
#define LOCAL_DIM_00 (IO_UHD_PROC_BASE + 0xE00)
    #define C_RAM_LINE_INFO_CS Fld(1,30,AC_MSKB3)//[30:30]
    #define C_RAM_BLK_INFO_CS Fld(1,29,AC_MSKB3)//[29:29]
    #define C_RAM_LINEBUF_CS Fld(1,28,AC_MSKB3)//[28:28]
    #define C_RAM_FRAME_DLY_CS Fld(1,27,AC_MSKB3)//[27:27]
    #define C_RAM_FRAME_DLY2_CS Fld(1,26,AC_MSKB3)//[26:26]
    #define C_GAIN_POS_SEL Fld(1,25,AC_MSKB3)//[25:25]
    #define C_FRAME_DLY_SPI Fld(1,24,AC_MSKB3)//[24:24]
    #define C_LOCAL_DIMMING_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define C_MAPPING_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define C_INK_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define C_FORCE_DIMM Fld(1,16,AC_MSKB2)//[16:16]
    #define C_DS_EN Fld(1,15,AC_MSKB1)//[15:15]
    #define C_DS_MODE Fld(1,11,AC_MSKB1)//[11:11]
    #define C_DS_SHIFT Fld(3,8,AC_MSKB1)//[10:8]
    #define C_DS_DIS_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_01 (IO_UHD_PROC_BASE + 0xE04)
    #define C_AUTO_INC_OFF Fld(1,31,AC_MSKB3)//[31:31]
    #define TRIG_SEL_CTRL Fld(1,30,AC_MSKB3)//[30:30]
    #define C_OUT_WINDOW_DATA_SEL Fld(1,29,AC_MSKB3)//[29:29]
    #define C_DEMO_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define C_DEMO_SWAP Fld(1,25,AC_MSKB3)//[25:25]
    #define C_DS_DEMO_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define C_REPEAT_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define C_REPEAT_MODE Fld(2,19,AC_MSKB2)//[20:19]
    #define C_PXL_V Fld(13,0,AC_MSKW10)//[12:0]
#define LOCAL_DIM_02 (IO_UHD_PROC_BASE + 0xE08)
    #define C_PXL_H_LAST Fld(13,16,AC_MSKW32)//[28:16]
    #define C_PXL_V_LAST Fld(13,0,AC_MSKW10)//[12:0]
#define LOCAL_DIM_03 (IO_UHD_PROC_BASE + 0xE0C)
    #define C_BLK_H Fld(7,16,AC_MSKB2)//[22:16]
    #define C_BLK_V Fld(7,0,AC_MSKB0)//[6:0]
#define LOCAL_DIM_04 (IO_UHD_PROC_BASE + 0xE10)
    #define C_BOUND_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define C_BOUND_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BOUND_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BOUND_1 Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_05 (IO_UHD_PROC_BASE + 0xE14)
    #define C_AVE_SLPF_MODE Fld(2,26,AC_MSKB3)//[27:26]
    #define C_DIM_SLPF_MODE Fld(2,24,AC_MSKB3)//[25:24]
    #define C_BOUND_7 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_BOUND_6 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_BOUND_5 Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_06 (IO_UHD_PROC_BASE + 0xE18)
    #define C_SHIFT_C Fld(4,24,AC_MSKB3)//[27:24]
    #define C_MULT_B Fld(10,8,AC_MSKW21)//[17:8]
    #define C_SHIFT_A Fld(5,0,AC_MSKB0)//[4:0]
#define LOCAL_DIM_07 (IO_UHD_PROC_BASE + 0xE1C)
    #define C_DEMO_LEVEL Fld(8,24,AC_FULLB3)//[31:24]
    #define C_AVE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define C_MAX_THD Fld(16,0,AC_FULLW10)//[15:0]
#define LOCAL_DIM_08 (IO_UHD_PROC_BASE + 0xE20)
    #define C_IIR_SPEED Fld(2,26,AC_MSKB3)//[27:26]
    #define C_WHITE_SUPPRESS_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define C_DIM_GAIN Fld(9,16,AC_MSKW32)//[24:16]
    #define C_CUR_W_MIN Fld(8,8,AC_FULLB1)//[15:8]
    #define C_DIFF_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_09 (IO_UHD_PROC_BASE + 0xE24)
    #define MAPPING_CPU_SRAM_ADDR Fld(8,8,AC_FULLB1)//[15:8]
    #define R_MAPPING_ADDR Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_0A (IO_UHD_PROC_BASE + 0xE28)
    #define C_FRAME_DLY_CPU Fld(1,31,AC_MSKB3)//[31:31]
    #define DIM_RDY Fld(1,30,AC_MSKB3)//[30:30]
    #define DIM_DATA Fld(8,16,AC_FULLB2)//[23:16]
    #define C_FRAME_DLY_CPU_RADDR Fld(12,0,AC_MSKW10)//[11:0]
#define LOCAL_DIM_0B (IO_UHD_PROC_BASE + 0xE2C)
    #define C_BLK_INFO_CPU Fld(1,31,AC_MSKB3)//[31:31]
    #define C_BLK_INFO_CPU_RADDR Fld(12,0,AC_MSKW10)//[11:0]
#define LOCAL_DIM_0C (IO_UHD_PROC_BASE + 0xE30)
    #define BLK_INFO_DATA Fld(24,0,AC_MSKDW)//[23:0]
#define LOCAL_DIM_0D (IO_UHD_PROC_BASE + 0xE34)
    #define C_DS_MULT_2 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_DS_MULT_1 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_DS_MULT_0 Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_DIM_0E (IO_UHD_PROC_BASE + 0xE38)
    #define C_DS_MULT_5 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_DS_MULT_4 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_DS_MULT_3 Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_DIM_0F (IO_UHD_PROC_BASE + 0xE3C)
    #define C_DS_INT_MODE Fld(1,31,AC_MSKB3)//[31:31]
    #define C_DS_LIMIT_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_DS_MULT_8 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_DS_MULT_7 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_DS_MULT_6 Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_DIM_11 (IO_UHD_PROC_BASE + 0xE44)
    #define LD_CAP_OUT Fld(32,0,AC_FULLDW)//[31:0]
#define LOCAL_DIM_12 (IO_UHD_PROC_BASE + 0xE48)
    #define C_DEMO_BLK_Y_END Fld(7,24,AC_MSKB3)//[30:24]
    #define C_DEMO_BLK_Y_START Fld(7,16,AC_MSKB2)//[22:16]
    #define C_DEMO_BLK_X_END Fld(7,8,AC_MSKB1)//[14:8]
    #define C_DEMO_BLK_X_START Fld(7,0,AC_MSKB0)//[6:0]
#define LOCAL_DIM_13 (IO_UHD_PROC_BASE + 0xE4C)
    #define C_DEMO_PXL_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define C_DEMO_PXL_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define LOCAL_DIM_14 (IO_UHD_PROC_BASE + 0xE50)
    #define C_DEMO_PXL_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define C_DEMO_PXL_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define LOCAL_DIM_15 (IO_UHD_PROC_BASE + 0xE54)
    #define MAPPING_CPU_SEL_SRAM_0 Fld(1,31,AC_MSKB3)//[31:31]
    #define R_MAPPING_CPURW_ACTIVE Fld(1,30,AC_MSKB3)//[30:30]
    #define MAPPING_TABLE_WAIT_VSYNC Fld(1,29,AC_MSKB3)//[29:29]
    #define R_MAPPING_FILL_TABLE_READY Fld(1,28,AC_MSKB3)//[28:28]
    #define MAPPING_READ_FINISH Fld(1,27,AC_MSKB3)//[27:27]
    #define MAPPING_R_READ Fld(1,26,AC_MSKB3)//[26:26]
    #define MAPPING_WRITE_FINISH Fld(1,25,AC_MSKB3)//[25:25]
    #define R_MAPPING_WRITE Fld(1,24,AC_MSKB3)//[24:24]
    #define R_MAPPING_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define R_MAPPING_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define R_MAPPING_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define R_MAPPING_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define R_MAPPING_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define R_MAPPING_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define R_MAPPING_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_16 (IO_UHD_PROC_BASE + 0xE58)
    #define R_MAPPING_RDATA Fld(8,16,AC_FULLB2)//[23:16]
    #define R_MAPPING_WDATA Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_DIM_18 (IO_UHD_PROC_BASE + 0xE60)
    #define C_PXL_H_1 Fld(13,16,AC_MSKW32)//[28:16]
    #define C_PXL_H_0 Fld(13,0,AC_MSKW10)//[12:0]
#define LOCAL_DIM_19 (IO_UHD_PROC_BASE + 0xE64)
    #define C_PXL_H_3 Fld(13,16,AC_MSKW32)//[28:16]
    #define C_PXL_H_2 Fld(13,0,AC_MSKW10)//[12:0]

#endif //#if defined(CC_SUPPORT_UHD)

