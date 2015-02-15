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
 * $RCSfile: hw_uhd_proc2.h$
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

#if defined(CC_SUPPORT_UHD)
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#define IO_UHD_CTRL_BASE   0xf0002000
#define IO_UHD_SCCTRL_BASE 0xf0003000
#define IO_UHD_2DGFX_BASE  0xf0004000
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//Page UHD_CTRL
#define UHD_CLOCK (IO_UHD_CTRL_BASE + 0x000)
    #define UHD_CLOCK_PSC_CKEN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_RESET (IO_UHD_CTRL_BASE + 0x004)
    #define BIM_SOFT_RST Fld(1,7,AC_MSKB0)//[7:7]
    #define OSD_MERGE_SOFT_RST Fld(1,6,AC_MSKB0)//[6:6]
    #define POST_PROC_SOFT_RST Fld(1,5,AC_MSKB0)//[5:5]
    #define GFX_OSD_SOFT_RST Fld(1,4,AC_MSKB0)//[4:4]
    #define PRE_PROC_SOFT_RST Fld(1,3,AC_MSKB0)//[3:3]
    #define S2P_SOFT_RST Fld(1,2,AC_MSKB0)//[2:2]
    #define REF_TIME_GEN_SOFT_RST Fld(1,1,AC_MSKB0)//[1:1]
    #define PSC_SOFT_RST Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_CTRL0 (IO_UHD_CTRL_BASE + 0x008)
    #define REF_VSYNC_DELAY Fld(16,16,AC_FULLW32)//[31:16]
    #define REF_VSYNC_AUTO_LINE Fld(8,8,AC_FULLB1)//[15:8]
    #define REF_VSYNC_AUTO Fld(1,7,AC_MSKB0)//[7:7]
    #define REF_VSYNC_SEL Fld(3,4,AC_MSKB0)//[6:4]
    #define UHD_P2S_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define BY_MC_ORI Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_SRC_SEL Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_CTRL_IMPORT_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_CTRL1 (IO_UHD_CTRL_BASE + 0x00C)
    #define UHD_DEBUG_SEL Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_422_FILTER_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define UV_VERTICAL_422_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST0 (IO_UHD_CTRL_BASE + 0x010)
    #define MBIST_BACKGROUND Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_CTRL_ATPG_OB Fld(1,5,AC_MSKB0)//[5:5]
    #define UHD_CTRL_ATPG_CT Fld(1,4,AC_MSKB0)//[4:4]
    #define MBIST_DEBUG Fld(1,2,AC_MSKB0)//[2:2]
    #define MBIST_HOLDB Fld(1,1,AC_MSKB0)//[1:1]
    #define MBIST_RSTB_PRE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST1 (IO_UHD_CTRL_BASE + 0x014)
    #define HCOEF_SRAM3_130X94_MBIST_MODE Fld(1,13,AC_MSKB1)//[13:13]
    #define HCOEF_SRAM4_130X94_MBIST_MODE Fld(1,12,AC_MSKB1)//[12:12]
    #define UHD_DISP_FIFO_MBIST_MODE Fld(1,11,AC_MSKB1)//[11:11]
    #define CM_SRAM_MBIST_MODE Fld(1,10,AC_MSKB1)//[10:10]
    #define AMBI_ACC_SRAM_MBIST_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define AMBI_RESULT_SRAM_MBIST_MODE Fld(1,8,AC_MSKB1)//[8:8]
    #define LD_SRAM_STROAGE_MBIST_MODE Fld(1,7,AC_MSKB0)//[7:7]
    #define LD_SRAM_ANALYSIS_MBIST_MODE Fld(1,6,AC_MSKB0)//[6:6]
    #define TDPROCSRAM_0_MBIST_MODE Fld(1,5,AC_MSKB0)//[5:5]
    #define CBESRAM_0_MBIST_MODE Fld(1,4,AC_MSKB0)//[4:4]
    #define OSGAMMA_A_MBIST_MODE Fld(1,3,AC_MSKB0)//[3:3]
    #define OSGAMMA_B_MBIST_MODE Fld(1,2,AC_MSKB0)//[2:2]
    #define REORDERSRAM_A_MBIST_MODE Fld(1,1,AC_MSKB0)//[1:1]
    #define REORDERSRAM_B_MBIST_MODE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST2 (IO_UHD_CTRL_BASE + 0x018)
    #define HCOEF_SRAM3_130X94_MBIST_DONE Fld(1,13,AC_MSKB1)//[13:13]
    #define HCOEF_SRAM4_130X94_MBIST_DONE Fld(1,12,AC_MSKB1)//[12:12]
    #define UHD_DISP_FIFO_MBIST_DONE Fld(1,11,AC_MSKB1)//[11:11]
    #define CM_SRAM_MBIST_DONE Fld(1,10,AC_MSKB1)//[10:10]
    #define AMBI_ACC_SRAM_MBIST_DONE Fld(1,9,AC_MSKB1)//[9:9]
    #define AMBI_RESULT_SRAM_MBIST_DONE Fld(1,8,AC_MSKB1)//[8:8]
    #define LD_SRAM_STROAGE_MBIST_DONE Fld(1,7,AC_MSKB0)//[7:7]
    #define LD_SRAM_ANALYSIS_MBIST_DONE Fld(1,6,AC_MSKB0)//[6:6]
    #define TDPROCSRAM_0_MBIST_DONE Fld(1,5,AC_MSKB0)//[5:5]
    #define CBESRAM_0_MBIST_DONE Fld(1,4,AC_MSKB0)//[4:4]
    #define OSGAMMA_A_MBIST_DONE Fld(1,3,AC_MSKB0)//[3:3]
    #define OSGAMMA_B_MBIST_DONE Fld(1,2,AC_MSKB0)//[2:2]
    #define REORDERSRAM_A_MBIST_DONE Fld(1,1,AC_MSKB0)//[1:1]
    #define REORDERSRAM_B_MBIST_DONE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST3 (IO_UHD_CTRL_BASE + 0x01C)
    #define REORDERSRAM_A_MBIST_FAIL Fld(1,30,AC_MSKB3)//[30:30]
    #define REORDERSRAM_B_MBIST_FAIL Fld(1,29,AC_MSKB3)//[29:29]
    #define LD_SRAM_ANALYSIS_MBIST_FAIL_0 Fld(1,28,AC_MSKB3)//[28:28]
    #define HCOEF_SRAM3_130X94_MBIST_FAIL_0 Fld(1,27,AC_MSKB3)//[27:27]
    #define HCOEF_SRAM4_130X94_MBIST_FAIL_0 Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_DISP_FIFO_MBIST_FAIL_0 Fld(1,25,AC_MSKB3)//[25:25]
    #define UHD_DISP_FIFO_MBIST_FAIL_1 Fld(1,24,AC_MSKB3)//[24:24]
    #define TDPROCSRAM_0_MBIST_FAIL_0 Fld(1,23,AC_MSKB2)//[23:23]
    #define TDPROCSRAM_0_MBIST_FAIL_1 Fld(1,22,AC_MSKB2)//[22:22]
    #define TDPROCSRAM_0_MBIST_FAIL_2 Fld(1,21,AC_MSKB2)//[21:21]
    #define TDPROCSRAM_0_MBIST_FAIL_3 Fld(1,20,AC_MSKB2)//[20:20]
    #define TDPROCSRAM_0_MBIST_FAIL_4 Fld(1,19,AC_MSKB2)//[19:19]
    #define TDPROCSRAM_0_MBIST_FAIL_5 Fld(1,18,AC_MSKB2)//[18:18]
    #define TDPROCSRAM_0_MBIST_FAIL_6 Fld(1,17,AC_MSKB2)//[17:17]
    #define TDPROCSRAM_0_MBIST_FAIL_7 Fld(1,16,AC_MSKB2)//[16:16]
    #define TDPROCSRAM_0_MBIST_FAIL_8 Fld(1,15,AC_MSKB1)//[15:15]
    #define TDPROCSRAM_0_MBIST_FAIL_9 Fld(1,14,AC_MSKB1)//[14:14]
    #define TDPROCSRAM_0_MBIST_FAIL_10 Fld(1,13,AC_MSKB1)//[13:13]
    #define TDPROCSRAM_0_MBIST_FAIL_11 Fld(1,12,AC_MSKB1)//[12:12]
    #define CBESRAM_0_MBIST_FAIL_0 Fld(1,11,AC_MSKB1)//[11:11]
    #define CBESRAM_0_MBIST_FAIL_1 Fld(1,10,AC_MSKB1)//[10:10]
    #define CBESRAM_0_MBIST_FAIL_2 Fld(1,9,AC_MSKB1)//[9:9]
    #define CBESRAM_0_MBIST_FAIL_3 Fld(1,8,AC_MSKB1)//[8:8]
    #define CM_SRAM_MBIST_FAIL_0 Fld(1,7,AC_MSKB0)//[7:7]
    #define CM_SRAM_MBIST_FAIL_1 Fld(1,6,AC_MSKB0)//[6:6]
    #define CM_SRAM_MBIST_FAIL_2 Fld(1,5,AC_MSKB0)//[5:5]
    #define CM_SRAM_MBIST_FAIL_3 Fld(1,4,AC_MSKB0)//[4:4]
    #define CM_SRAM_MBIST_FAIL_4 Fld(1,3,AC_MSKB0)//[3:3]
    #define CM_SRAM_MBIST_FAIL_5 Fld(1,2,AC_MSKB0)//[2:2]
    #define AMBI_ACC_SRAM_MBIST_FAIL_0 Fld(1,1,AC_MSKB0)//[1:1]
    #define AMBI_RESULT_SRAM_MBIST_FAIL_0 Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST4 (IO_UHD_CTRL_BASE + 0x020)
    #define LD_SRAM_STROAGE_MBIST_FAIL_0 Fld(1,31,AC_MSKB3)//[31:31]
    #define LD_SRAM_STROAGE_MBIST_FAIL_1 Fld(1,30,AC_MSKB3)//[30:30]
    #define LD_SRAM_STROAGE_MBIST_FAIL_2 Fld(1,29,AC_MSKB3)//[29:29]
    #define LD_SRAM_STROAGE_MBIST_FAIL_3 Fld(1,28,AC_MSKB3)//[28:28]
    #define LD_SRAM_STROAGE_MBIST_FAIL_4 Fld(1,27,AC_MSKB3)//[27:27]
    #define LD_SRAM_STROAGE_MBIST_FAIL_5 Fld(1,26,AC_MSKB3)//[26:26]
    #define LD_SRAM_STROAGE_MBIST_FAIL_6 Fld(1,25,AC_MSKB3)//[25:25]
    #define LD_SRAM_STROAGE_MBIST_FAIL_7 Fld(1,24,AC_MSKB3)//[24:24]
    #define OSGAMMA_A_MBIST_FAIL_0 Fld(1,23,AC_MSKB2)//[23:23]
    #define OSGAMMA_A_MBIST_FAIL_1 Fld(1,22,AC_MSKB2)//[22:22]
    #define OSGAMMA_A_MBIST_FAIL_2 Fld(1,21,AC_MSKB2)//[21:21]
    #define OSGAMMA_A_MBIST_FAIL_3 Fld(1,20,AC_MSKB2)//[20:20]
    #define OSGAMMA_A_MBIST_FAIL_4 Fld(1,19,AC_MSKB2)//[19:19]
    #define OSGAMMA_A_MBIST_FAIL_5 Fld(1,18,AC_MSKB2)//[18:18]
    #define OSGAMMA_A_MBIST_FAIL_6 Fld(1,17,AC_MSKB2)//[17:17]
    #define OSGAMMA_A_MBIST_FAIL_7 Fld(1,16,AC_MSKB2)//[16:16]
    #define OSGAMMA_A_MBIST_FAIL_8 Fld(1,15,AC_MSKB1)//[15:15]
    #define OSGAMMA_A_MBIST_FAIL_9 Fld(1,14,AC_MSKB1)//[14:14]
    #define OSGAMMA_A_MBIST_FAIL_10 Fld(1,13,AC_MSKB1)//[13:13]
    #define OSGAMMA_A_MBIST_FAIL_11 Fld(1,12,AC_MSKB1)//[12:12]
    #define OSGAMMA_B_MBIST_FAIL_0 Fld(1,11,AC_MSKB1)//[11:11]
    #define OSGAMMA_B_MBIST_FAIL_1 Fld(1,10,AC_MSKB1)//[10:10]
    #define OSGAMMA_B_MBIST_FAIL_2 Fld(1,9,AC_MSKB1)//[9:9]
    #define OSGAMMA_B_MBIST_FAIL_3 Fld(1,8,AC_MSKB1)//[8:8]
    #define OSGAMMA_B_MBIST_FAIL_4 Fld(1,7,AC_MSKB0)//[7:7]
    #define OSGAMMA_B_MBIST_FAIL_5 Fld(1,6,AC_MSKB0)//[6:6]
    #define OSGAMMA_B_MBIST_FAIL_6 Fld(1,5,AC_MSKB0)//[5:5]
    #define OSGAMMA_B_MBIST_FAIL_7 Fld(1,4,AC_MSKB0)//[4:4]
    #define OSGAMMA_B_MBIST_FAIL_8 Fld(1,3,AC_MSKB0)//[3:3]
    #define OSGAMMA_B_MBIST_FAIL_9 Fld(1,2,AC_MSKB0)//[2:2]
    #define OSGAMMA_B_MBIST_FAIL_10 Fld(1,1,AC_MSKB0)//[1:1]
    #define OSGAMMA_B_MBIST_FAIL_11 Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_MBIST5 (IO_UHD_CTRL_BASE + 0x024)
    #define UHD_OSD_MERGE_MBIST_MODE Fld(1,15,AC_MSKB1)//[15:15]
    #define S2P_SRAM_MBIST_MODE Fld(1,14,AC_MSKB1)//[14:14]
    #define UHD_OSD_MERGE_MBIST_DONE Fld(1,13,AC_MSKB1)//[13:13]
    #define S2P_SRAM_MBIST_DONE Fld(1,12,AC_MSKB1)//[12:12]
    #define UHD_OSD_MERGE_MBIST_FAIL_0 Fld(1,11,AC_MSKB1)//[11:11]
    #define UHD_OSD_MERGE_MBIST_FAIL_1 Fld(1,10,AC_MSKB1)//[10:10]
    #define UHD_OSD_MERGE_MBIST_FAIL_2 Fld(1,9,AC_MSKB1)//[9:9]
    #define UHD_OSD_MERGE_MBIST_FAIL_3 Fld(1,8,AC_MSKB1)//[8:8]
    #define S2P_SRAM_MBIST_FAIL_0 Fld(1,7,AC_MSKB0)//[7:7]
    #define S2P_SRAM_MBIST_FAIL_1 Fld(1,6,AC_MSKB0)//[6:6]
    #define S2P_SRAM_MBIST_FAIL_2 Fld(1,5,AC_MSKB0)//[5:5]
    #define S2P_SRAM_MBIST_FAIL_3 Fld(1,4,AC_MSKB0)//[4:4]
    #define S2P_SRAM_MBIST_FAIL_4 Fld(1,3,AC_MSKB0)//[3:3]
    #define S2P_SRAM_MBIST_FAIL_5 Fld(1,2,AC_MSKB0)//[2:2]
    #define S2P_SRAM_MBIST_FAIL_6 Fld(1,1,AC_MSKB0)//[1:1]
    #define S2P_SRAM_MBIST_FAIL_7 Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_S2P0 (IO_UHD_CTRL_BASE + 0x030)
    #define MC_ORI_ALIGN_V_RISE Fld(1,8,AC_MSKB1)//[8:8]
    #define S2P_DELSEL Fld(4,4,AC_MSKB0)//[7:4]
    #define RG_DIS_2SEC_VSYNC Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_2SEC_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_1TO2CHL_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_S2P1 (IO_UHD_CTRL_BASE + 0x034)
    #define RG_H_TOTAL Fld(12,16,AC_MSKW32)//[27:16]
    #define RG_H_VALID Fld(11,0,AC_MSKW10)//[10:0]
#define UHD_S2P2 (IO_UHD_CTRL_BASE + 0x038)
    #define RG_H_END Fld(12,16,AC_MSKW32)//[27:16]
    #define RG_H_START Fld(12,0,AC_MSKW10)//[11:0]
#define UHD_S2P3 (IO_UHD_CTRL_BASE + 0x03C)
    #define RG_V_WIDTH Fld(7,16,AC_MSKB2)//[22:16]
    #define RG_V_TOTAL Fld(12,0,AC_MSKW10)//[11:0]
#define UHD_OSD_SC0 (IO_UHD_CTRL_BASE + 0x040)
    #define RG_HUSC_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define RG_FACC_OFST Fld(15,16,AC_MSKW32)//[30:16]
    #define RG_FACC_STEP Fld(15,1,AC_MSKW10)//[15:1]
    #define RG_HUSC_RPTL Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_OSD_SC1 (IO_UHD_CTRL_BASE + 0x044)
    #define RG_DST_HSIZE Fld(13,19,AC_MSKW32)//[31:19]
    #define RG_AEDGE_TH Fld(8,11,AC_MSKW21)//[18:11]
    #define RG_FSAMPLE_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define RG_OUT_DLY_1_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define RG_GFX_PRE_MUL Fld(1,2,AC_MSKB0)//[2:2]
    #define RG_AEDGEDET_EN_SETRGB Fld(1,1,AC_MSKB0)//[1:1]
    #define RG_AEDGEDET_EN_NEAR Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_INT0 (IO_UHD_CTRL_BASE + 0x048)
    #define INT_TDSHARP_CLR Fld(1,27,AC_MSKB3)//[27:27]
    #define INT_UHD_OUT_VSYNC_F_CLR Fld(1,26,AC_MSKB3)//[26:26]
    #define INT_UHD_OUT_VSYNC_R_CLR Fld(1,25,AC_MSKB3)//[25:25]
    #define INT_POST_OUT_VSYNC_F_CLR Fld(1,24,AC_MSKB3)//[24:24]
    #define INT_POST_OUT_VSYNC_R_CLR Fld(1,23,AC_MSKB2)//[23:23]
    #define INT_UHD_V_CNT_CLR Fld(1,22,AC_MSKB2)//[22:22]
    #define INT_SC_OUT_VDE_F_CLR Fld(1,21,AC_MSKB2)//[21:21]
    #define INT_SC_OUT_VDE_R_CLR Fld(1,20,AC_MSKB2)//[20:20]
    #define INT_SC_OUT_VSYNC_F_CLR Fld(1,19,AC_MSKB2)//[19:19]
    #define INT_SC_OUT_VSYNC_R_CLR Fld(1,18,AC_MSKB2)//[18:18]
    #define INT_IN_VSYNC_F_CLR Fld(1,17,AC_MSKB2)//[17:17]
    #define INT_IN_VSYNC_R_CLR Fld(1,16,AC_MSKB2)//[16:16]
    
    #define INT_TDSHARP_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define INT_UHD_OUT_VSYNC_F_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define INT_UHD_OUT_VSYNC_R_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define INT_POST_OUT_VSYNC_F_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define INT_POST_OUT_VSYNC_R_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define INT_UHD_V_CNT_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define INT_SC_OUT_VDE_F_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define INT_SC_OUT_VDE_R_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define INT_SC_OUT_VSYNC_F_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define INT_SC_OUT_VSYNC_R_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define INT_UHD_IN_VSYNC_F_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define INT_UHD_IN_VSYNC_R_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_INT1 (IO_UHD_CTRL_BASE + 0x04C)
    #define INT_SC_V_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define INT_STA_TDSHARP Fld(1,11,AC_MSKB1)//[11:11]
    #define INT_STA_UHD_OUT_VSYNC_F Fld(1,10,AC_MSKB1)//[10:10]
    #define INT_STA_UHD_OUT_VSYNC_R Fld(1,9,AC_MSKB1)//[9:9]
    #define INT_STA_POST_OUT_VSYNC_F Fld(1,8,AC_MSKB1)//[8:8]
    #define INT_STA_POST_OUT_VSYNC_R Fld(1,7,AC_MSKB0)//[7:7]
    #define INT_STA_UHD_V_CNT Fld(1,6,AC_MSKB0)//[6:6]
    #define INT_STA_SC_OUT_VDE_F Fld(1,5,AC_MSKB0)//[5:5]
    #define INT_STA_SC_OUT_VDE_R Fld(1,4,AC_MSKB0)//[4:4]
    #define INT_STA_SC_OUT_VSYNC_F Fld(1,3,AC_MSKB0)//[3:3]
    #define INT_STA_SC_OUT_VSYNC_R Fld(1,2,AC_MSKB0)//[2:2]
    #define INT_STA_IN_VSYNC_F Fld(1,1,AC_MSKB0)//[1:1]
    #define INT_STA_IN_VSYNC_R Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_3X3_0 (IO_UHD_CTRL_BASE + 0x050)
    #define EXT_TABLE_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define INT_TABLE_SEL Fld(4,4,AC_MSKB0)//[7:4]
    #define O_SIGNED Fld(1,3,AC_MSKB0)//[3:3]
    #define I_SIGNED Fld(1,2,AC_MSKB0)//[2:2]
    #define CT_BYPASS Fld(1,1,AC_MSKB0)//[1:1]
    #define CT_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_3X3_1 (IO_UHD_CTRL_BASE + 0x054)
    #define EXT_C00 Fld(13,16,AC_MSKW32)//[28:16]
    #define EXT_C01 Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_3X3_2 (IO_UHD_CTRL_BASE + 0x058)
    #define EXT_C02 Fld(13,16,AC_MSKW32)//[28:16]
    #define EXT_C10 Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_3X3_3 (IO_UHD_CTRL_BASE + 0x05C)
    #define EXT_C11 Fld(13,16,AC_MSKW32)//[28:16]
    #define EXT_C12 Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_3X3_4 (IO_UHD_CTRL_BASE + 0x060)
    #define EXT_C20 Fld(13,16,AC_MSKW32)//[28:16]
    #define EXT_C21 Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_3X3_5 (IO_UHD_CTRL_BASE + 0x064)
    #define EXT_C22 Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_3X3_6 (IO_UHD_CTRL_BASE + 0x068)
    #define EXT_PRE_ADD_2 Fld(9,16,AC_MSKW32)//[24:16]
    #define EXT_PRE_ADD_1 Fld(9,0,AC_MSKW10)//[8:0]
#define UHD_3X3_7 (IO_UHD_CTRL_BASE + 0x06C)
    #define EXT_PRE_ADD_0 Fld(9,16,AC_MSKW32)//[24:16]
    #define EXT_POST_ADD_2 Fld(9,0,AC_MSKW10)//[8:0]
#define UHD_3X3_8 (IO_UHD_CTRL_BASE + 0x070)
    #define EXT_POST_ADD_1 Fld(9,16,AC_MSKW32)//[24:16]
    #define EXT_POST_ADD_0 Fld(9,0,AC_MSKW10)//[8:0]
#define UHD_OSD_MERGE0 (IO_UHD_CTRL_BASE + 0x074)
    #define OSD_MERGE_WIDTH_IN Fld(16,16,AC_FULLW32)//[31:16]
    #define OSD_MERGE_HEIGHT_IN Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_OSD_MERGE1 (IO_UHD_CTRL_BASE + 0x078)
    #define OSD_MERGE_HTOTAL_OUT Fld(16,16,AC_FULLW32)//[31:16]
    #define OSD_MERGE_HACT_OUT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_OSD_MERGE2 (IO_UHD_CTRL_BASE + 0x07C)
    #define OSD_MERGE_PRE_MUL_A Fld(1,8,AC_MSKB1)//[8:8]
    #define OSD_MERGE_DELSEL Fld(4,4,AC_MSKB0)//[7:4]
    #define OSD_MERGE_DLY_1T Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SBS_LI_0 (IO_UHD_CTRL_BASE + 0x080)
    #define SBS_LI_WIDTH_L Fld(16,16,AC_FULLW32)//[31:16]
    #define SBS_LI_WIDTH_R Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SBS_LI_1 (IO_UHD_CTRL_BASE + 0x084)
    #define SBS_LI_HEIGHT Fld(16,16,AC_FULLW32)//[31:16]
    #define SBS_LI_VTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SBS_LI_2 (IO_UHD_CTRL_BASE + 0x088)
    #define SBS_LI_HFP Fld(16,16,AC_FULLW32)//[31:16]
    #define SBS_LI_HBP Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SBS_LI_3 (IO_UHD_CTRL_BASE + 0x08C)
    #define OUTPUT_R_FIRST Fld(1,3,AC_MSKB0)//[3:3]
    #define DROP_LINE_MODE Fld(1,2,AC_MSKB0)//[2:2]
    #define BYPASS_SBS_LI Fld(1,1,AC_MSKB0)//[1:1]
    #define SBS_LI_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_STATUS0 (IO_UHD_CTRL_BASE + 0x094)
    #define STA_VB8_VCNT_0 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VB8_HCNT_0 Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_STATUS1 (IO_UHD_CTRL_BASE + 0x098)
    #define STA_VB8_VCNT_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VB8_HCNT_1 Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_STATUS2 (IO_UHD_CTRL_BASE + 0x09C)
    #define STA_VB8_VCNT_2 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VB8_HCNT_2 Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_STATUS3 (IO_UHD_CTRL_BASE + 0x0A0)
    #define STA_VB8_VCNT_3 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VB8_HCNT_3 Fld(16,0,AC_FULLW10)//[15:0]

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//Page UHD_SCCTRL
//----------------------------------------------------------------------
#define UHD_SCCTRL_00 (IO_UHD_SCCTRL_BASE + 0x200)
    #define UHD_H_PHASE_INIT_SIGN Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_H_PHASE_INIT_INTG Fld(3,28,AC_MSKB3)//[30:28]
    #define UHD_H_PHASE_INIT_FRAC Fld(20,8,AC_MSKDW)//[27:8]
#define UHD_SCCTRL_01 (IO_UHD_SCCTRL_BASE + 0x204)
    #define UHD_H_PHASE_DELTA_INTG Fld(3,28,AC_MSKB3)//[30:28]
    #define UHD_H_PHASE_DELTA_FRAC Fld(20,8,AC_MSKDW)//[27:8]
#define UHD_SCCTRL_05 (IO_UHD_SCCTRL_BASE + 0x214)
    #define UHD_NONL_H_START_POS Fld(12,20,AC_MSKW32)//[31:20]
    #define UHD_NONL_H_STOP_POS Fld(12,8,AC_MSKW21)//[19:8]
    #define UHD_NONL_H_ADD_MINUS_SEL Fld(1,7,AC_MSKB0)//[7:7]
    #define UHD_NONL_H_ENABLE Fld(1,6,AC_MSKB0)//[6:6]
    #define UHD_3D_MODE Fld(4,0,AC_MSKB0)//[3:0]
#define UHD_SCCTRL_06 (IO_UHD_SCCTRL_BASE + 0x218)
    #define UHD_NONL_H_PHASE_DELTA Fld(20,12,AC_MSKDW)//[31:12]
    #define UHD_NONL_H_PHASE_DELTA_FLOAT Fld(12,0,AC_MSKW10)//[11:0]
#define UHD_SCCTRL_09 (IO_UHD_SCCTRL_BASE + 0x224)
    #define UHD_INPUT_WIDTH Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_INPUT_HEIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCCTRL_0A (IO_UHD_SCCTRL_BASE + 0x228)
    #define UHD_LR_PAIR_CTRL Fld(2,6,AC_MSKB0)//[7:6]
    #define UHD_CLR_STA_SET_RES_O Fld(1,5,AC_MSKB0)//[5:5]
    #define UHD_STA_SET_RES_O Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_CLR_STA_SET_RES_I Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_STA_SET_RES_I Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_SET_RES_TOGGLE Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_SET_RES_IMPORT_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCCTRL_0B (IO_UHD_SCCTRL_BASE + 0x22C)
    #define UHD_STA_DISPFF_OVFL Fld(1,28,AC_MSKB3)//[28:28]
    #define UHD_CLR_DISPFF_FLG Fld(1,27,AC_MSKB3)//[27:27]
    #define UHD_DRAM_RDREQ_DLY Fld(4,20,AC_MSKB2)//[23:20]
    #define UHD_DISP_FF_WTLV Fld(11,0,AC_MSKW10)//[10:0]
#define UHD_SCCTRL_0C (IO_UHD_SCCTRL_BASE + 0x230)    //????
    #define UHD__MJC_INIT_DAT_RATIO Fld(10,0,AC_MSKW10)//[9:0]
#define UHD_SCCTRL_0D (IO_UHD_SCCTRL_BASE + 0x234)
    #define UHD_HMSK_STR_OFST_1 Fld(17,0,AC_MSKDW)//[16:0]
#define UHD_SCCTRL_0E (IO_UHD_SCCTRL_BASE + 0x238)
    #define UHD_VMSK_STR_OFST_1 Fld(17,0,AC_MSKDW)//[16:0]
#define UHD_SCCTRL_11 (IO_UHD_SCCTRL_BASE + 0x244)
    #define UHD_OUTPUT_WIDTH Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_OUTPUT_HEIGHT Fld(16,0,AC_FULLW10)//[15:0]

//Page UHD_SCSYS
#define UHD_SCSYS_00 (IO_UHD_SCCTRL_BASE + 0x300)
    #define UHDOCLK_CLKEN Fld(1,3,AC_MSKB0)//[3:3]
    #define UHDUSE_INTL_CKEN Fld(1,2,AC_MSKB0)//[2:2]
    #define UHDPSC_CLK_SWRST Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_01 (IO_UHD_SCCTRL_BASE + 0x304)
    #define UHD_SET_DATA Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_CLR_INPUT_MEASURE Fld(1,28,AC_MSKB3)//[28:28]
    #define UHD_CLR_PTG_UDFL_FLG Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_CLR_MTG_UDFL_FLG Fld(1,21,AC_MSKB2)//[21:21]
    #define UHD_STA_M_TG_UNDERFLOW Fld(1,20,AC_MSKB2)//[20:20]
    #define UHD_STA_SET_DATA Fld(1,19,AC_MSKB2)//[19:19]
    #define UHD_TG_DISP_R_FREERUN_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define UHD_TG_IMPORT_ENABLE Fld(1,16,AC_MSKB2)//[16:16]
    #define UHD_TG_IMPORT_VCNT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_02 (IO_UHD_SCCTRL_BASE + 0x308)
    #define UHD_PNL_WIDTH Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_PNL_HEIGHT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_03 (IO_UHD_SCCTRL_BASE + 0x30C)
    #define UHD_PNL_HTOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_PNL_VTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_04 (IO_UHD_SCCTRL_BASE + 0x310)
    #define UHD_PNL_BACK_HPORCH Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_PNL_FRNT_HPORCH Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_05 (IO_UHD_SCCTRL_BASE + 0x314)
    #define UHD_PNL_BACK_VPORCH Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_PNL_FRNT_VPORCH Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_06 (IO_UHD_SCCTRL_BASE + 0x318)
    #define UHD_STA_MC_ORI Fld(1,8,AC_MSKB1)//[8:8]
    #define UHD_TG_SLAVE_INIT_DISP_R Fld(1,6,AC_MSKB0)//[6:6]
    #define UHD_INV_MC_ORI Fld(1,5,AC_MSKB0)//[5:5]
    #define UHD_MC_ORI_SEL Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_TG_FORCE_ZERO_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_TG_SLAVE_SYNC_MODE Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_TG_SLAVE_MODE Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_TG_ENABLE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_07 (IO_UHD_SCCTRL_BASE + 0x31C)
    #define UHD_SCALER_DEBUG_SEL Fld(8,24,AC_FULLB3)//[31:24]
#define UHD_SCSYS_09 (IO_UHD_SCCTRL_BASE + 0x324)
    #define UHD_STA_OUT_DISP_R Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_STA_IN_DISP_R Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_FORCE_IN_DISP_R_VAL Fld(1,29,AC_MSKB3)//[29:29]
    #define UHD_FORCE_IN_DISP_R Fld(1,28,AC_MSKB3)//[28:28]
    #define UHD_FRM_MASK_O_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define UHD_FRM_MASK_I_SEL Fld(4,20,AC_MSKB2)//[23:20]
    #define UHD_INV_OUTPUT_DISP_R Fld(1,19,AC_MSKB2)//[19:19]
    #define UHD_CLR_STA_INPUTV_ABNORMAL Fld(1,18,AC_MSKB2)//[18:18]
#define UHD_SCSYS_0B (IO_UHD_SCCTRL_BASE + 0x32C)
    #define UHD_CLR_DISPFF_FLG Fld(1,27,AC_MSKB3)//[27:27]
#define UHD_SCSYS_0F (IO_UHD_SCCTRL_BASE + 0x33C)
    #define UHD_FRM_TRK_CROSS_VSYNC_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_FRM_TRK_3D_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_FRM_TRK_DISP_R Fld(1,29,AC_MSKB3)//[29:29]
#define UHD_SCSYS_10 (IO_UHD_SCCTRL_BASE + 0x340)
    #define UHD_INV_OUTPUT_HSYNC Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_INV_OUTPUT_VSYNC Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_INV_OUTPUT_DE Fld(1,29,AC_MSKB3)//[29:29]
    #define UHD_FRM_MASK_IN_LR_SEL Fld(1,27,AC_MSKB3)//[27:27]
    #define UHD_FRM_MASK_IN_LR_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_FRM_MASK_OUT_LR_SEL Fld(1,25,AC_MSKB3)//[25:25]
    #define UHD_FRM_MASK_OUT_LR_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define UHD_FRM_MASK_I Fld(4,20,AC_MSKB2)//[23:20]
    #define UHD_FRM_MASK_O Fld(4,16,AC_MSKB2)//[19:16]
    #define UHD_FRM_TRK_LINE Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_11 (IO_UHD_SCCTRL_BASE + 0x344)
    #define UHD_DDDS_TRK_INV Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_FRM_TRK_DDDS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_INV_OSD_DISP_R Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_INV_OSD_HSYNC Fld(1,25,AC_MSKB3)//[25:25]
    #define UHD_INV_OSD_VSYNC Fld(1,24,AC_MSKB3)//[24:24]
    #define UHD_INV_INPUT_M_DISP_R Fld(1,23,AC_MSKB2)//[23:23]
    #define UHD_INV_REF_DISP_R Fld(1,22,AC_MSKB2)//[22:22]
    #define UHD_INV_REF_VSYNC_IN Fld(1,21,AC_MSKB2)//[21:21]
    #define UHD_INV_INPUT_PVDE Fld(1,20,AC_MSKB2)//[20:20]
    #define UHD_INV_INPUT_PHDE Fld(1,19,AC_MSKB2)//[19:19]
    #define UHD_INV_INPUT_MVDE Fld(1,18,AC_MSKB2)//[18:18]
    #define UHD_INV_INPUT_MHDE Fld(1,17,AC_MSKB2)//[17:17]
    #define UHD_INV_MSK Fld(1,15,AC_MSKB1)//[15:15]
    #define UHD_FRM_TRK_OUT_SEL Fld(2,13,AC_MSKB1)//[14:13]
    #define UHD_FRM_TRK_ONCE_ONLY Fld(1,11,AC_MSKB1)//[11:11]
    #define UHD_FRM_TRK_VIN_EDGE_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define UHD_TG_DAT_RATIO Fld(10,0,AC_MSKW10)//[9:0]
#define UHD_SCSYS_12 (IO_UHD_SCCTRL_BASE + 0x348)
    #define UHD_OSD_HSYNC_DLY Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_OSD_VSYNC_DLY Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_13 (IO_UHD_SCCTRL_BASE + 0x34C)
    #define UHD_STA_FRM_VSYNC_OUT_EARLY Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_P_HMSK_DLY Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_14 (IO_UHD_SCCTRL_BASE + 0x350)
    #define UHD_DDDS_TURBO_RGN Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_OSD_HSYNC_SEL Fld(1,11,AC_MSKB1)//[11:11]
    #define UHD_OSD_VSYNC_SEL Fld(1,10,AC_MSKB1)//[10:10]
    #define UHD_STA_FRM_TRK_LOCK_OK Fld(1,8,AC_MSKB1)//[8:8]
    #define UHD_SCSYS_14_PSC_FRM_TRK_STEP Fld(8,0,AC_FULLB0)//[7:0]
#define UHD_SCSYS_15 (IO_UHD_SCCTRL_BASE + 0x354)
    #define UHD_FRM_LOCK_TOR Fld(8,24,AC_FULLB3)//[31:24]
    #define UHD_FRM_LOCK_WIN Fld(8,16,AC_FULLB2)//[23:16]
#define UHD_SCSYS_16 (IO_UHD_SCCTRL_BASE + 0x358)
    #define UHD_MIN_CRC_CLIP_V_START Fld(12,18,AC_MSKW32)//[29:18]
    #define UHD_MIN_CRC_CLIP_H_START Fld(13,5,AC_MSKDW)//[17:5]
    #define UHD_MIN_CRC_CHECK_SUM_MODE Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_MIN_CRC_BOT_R_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_MIN_CRC_TOP_R_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_MIN_CRC_BOT_L_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_MIN_CRC_TOP_L_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_17 (IO_UHD_SCCTRL_BASE + 0x35C)
    #define UHD_INV_MEASURE_H Fld(1,29,AC_MSKB3)//[29:29]
    #define UHD_INV_MEASURE_V Fld(1,28,AC_MSKB3)//[28:28]
    #define UHD_CLR_OUTPUT_MEASURE Fld(1,27,AC_MSKB3)//[27:27]
    #define UHD_OUTPUT_MEASURE_SEL Fld(2,25,AC_MSKB3)//[26:25]
    #define UHD_MIN_CRC_CLIP_V_END Fld(12,13,AC_MSKDW)//[24:13]
    #define UHD_MIN_CRC_CLIP_H_END Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_SCSYS_18 (IO_UHD_SCCTRL_BASE + 0x360)
    #define UHD_MIN_CRC_V_IN_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define UHD_MIN_CRC_C_IN_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define UHD_MIN_CRC_Y_IN_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define UHD_SCSYS_19 (IO_UHD_SCCTRL_BASE + 0x364)
    #define UHD_MIN_STA_CRC_OUT_H Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCSYS_1A (IO_UHD_SCCTRL_BASE + 0x368)
    #define UHD_MIN_STA_CRC_OUT_V Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCSYS_1B (IO_UHD_SCCTRL_BASE + 0x36C)
    #define UHD_MOUT_CRC_VSYNC_INV Fld(1,30,AC_MSKB3)//[30:30]
    #define UHD_MOUT_CRC_CLIP_V_START Fld(12,18,AC_MSKW32)//[29:18]
    #define UHD_MOUT_CRC_CLIP_H_START Fld(13,5,AC_MSKDW)//[17:5]
    #define UHD_MOUT_CRC_CHECK_SUM_MODE Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_MOUT_CRC_BOT_R_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_MOUT_CRC_TOP_R_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_MOUT_CRC_BOT_L_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_MOUT_CRC_TOP_L_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_1C (IO_UHD_SCCTRL_BASE + 0x370)
    #define UHD_MOUT_CRC_CLIP_V_END Fld(12,13,AC_MSKDW)//[24:13]
    #define UHD_MOUT_CRC_CLIP_H_END Fld(13,0,AC_MSKW10)//[12:0]
#define UHD_SCSYS_1D (IO_UHD_SCCTRL_BASE + 0x374)
    #define UHD_MOUT_CRC_V_IN_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define UHD_MOUT_CRC_C_IN_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define UHD_MOUT_CRC_Y_IN_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define UHD_SCSYS_1E (IO_UHD_SCCTRL_BASE + 0x378)
    #define UHD_SCSYS_1E_PSC_MOUT_STA_CRC_OUT_H Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCSYS_1F (IO_UHD_SCCTRL_BASE + 0x37C)
    #define UHD_MOUT_STA_CRC_OUT_V Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCSYS_20 (IO_UHD_SCCTRL_BASE + 0x380)
    #define UHD_STA_H_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_STA_V_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_21 (IO_UHD_SCCTRL_BASE + 0x384)
    #define UHD_INPTG_YPAT1_1 Fld(4,27,AC_MSKB3)//[30:27]
    #define UHD_INPTG_LEFT_SIDE_1 Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_INPTG_PAT_DUTY_CYCLE_1 Fld(5,21,AC_MSKW32)//[25:21]
    #define UHD_INPTG_ANGLE_SEL_1 Fld(3,18,AC_MSKB2)//[20:18]
    #define UHD_INPTG_FREQ_SEL_1 Fld(3,15,AC_MSKW21)//[17:15]
    #define UHD_INPTG_HOR_LINE_EN_1 Fld(1,13,AC_MSKB1)//[13:13]
    #define UHD_INPTG_COLOR_MODE_1 Fld(2,11,AC_MSKB1)//[12:11]
    #define UHD_INPTG_PAT_MODE_1 Fld(2,9,AC_MSKB1)//[10:9]
    #define UHD_INPTG_FREERUN_SPEED_1 Fld(4,5,AC_MSKW10)//[8:5]
    #define UHD_INPTG_FREERUN_EN_1 Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_INPTG_BND_EN_1 Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_INPTG_3D_MODE_1 Fld(2,1,AC_MSKB0)//[2:1]
    #define UHD_INPTG_3D_INV_1 Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_22 (IO_UHD_SCCTRL_BASE + 0x388)
    #define UHD_INPTG_BND_H_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_INPTG_BND_W_1 Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_23 (IO_UHD_SCCTRL_BASE + 0x38C)
    #define UHD_P_VMSK_DLY Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_OUTPTG_ENABLE_1 Fld(1,9,AC_MSKB1)//[9:9]
    #define UHD_OUTPTG_YPAT2_1 Fld(4,5,AC_MSKW10)//[8:5]
    #define UHD_INPTG_ENABLE_1 Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_INPTG_YPAT2_1 Fld(4,0,AC_MSKB0)//[3:0]
#define UHD_SCSYS_24 (IO_UHD_SCCTRL_BASE + 0x390)
    #define UHD_OUTPTG_YPAT1_1 Fld(4,27,AC_MSKB3)//[30:27]
    #define UHD_OUTPTG_LEFT_SIDE_1 Fld(1,26,AC_MSKB3)//[26:26]
    #define UHD_OUTPTG_PAT_DUTY_CYCLE_1 Fld(5,21,AC_MSKW32)//[25:21]
    #define UHD_OUTPTG_ANGLE_SEL_1 Fld(3,18,AC_MSKB2)//[20:18]
    #define UHD_OUTPTG_FREQ_SEL_1 Fld(3,15,AC_MSKW21)//[17:15]
    #define UHD_OUTPTG_HOR_LINE_EN_1 Fld(1,13,AC_MSKB1)//[13:13]
    #define UHD_OUTPTG_COLOR_MODE_1 Fld(2,11,AC_MSKB1)//[12:11]
    #define UHD_OUTPTG_PAT_MODE_1 Fld(2,9,AC_MSKB1)//[10:9]
    #define UHD_OUTPTG_FREERUN_SPEED_1 Fld(4,5,AC_MSKW10)//[8:5]
    #define UHD_OUTPTG_FREERUN_EN_1 Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_OUTPTG_BND_EN_1 Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_OUTPTG_3D_MODE_1 Fld(2,1,AC_MSKB0)//[2:1]
    #define UHD_OUTPTG_3D_INV_1 Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCSYS_25 (IO_UHD_SCCTRL_BASE + 0x394)
    #define UHD_OUTPTG_BND_H_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_OUTPTG_BND_W_1 Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_26 (IO_UHD_SCCTRL_BASE + 0x398)
    #define UHD_STA_FRM_TRK_VTOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_STA_FRM_TRK_ABS_DIS Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_27 (IO_UHD_SCCTRL_BASE + 0x39C)
    #define UHD_FRM_TRK_TURBO_RGN Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_28 (IO_UHD_SCCTRL_BASE + 0x3A0)
    #define UHD_STA_HACTIVE Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD__STA_HTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_29 (IO_UHD_SCCTRL_BASE + 0x3A4)
    #define UHD_STA_VACTIVE Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_STA_VTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_2A (IO_UHD_SCCTRL_BASE + 0x3A8)
    #define UHD_STA_OUT_MEASURE_HTOTAL Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_STA_OUT_MEASURE_VTOTAL Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_2B (IO_UHD_SCCTRL_BASE + 0x3AC)
    #define UHD_STA_OUT_MEASURE_HACTIVE Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_STA_OUT_MEASURE_VACTIVE Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_2C (IO_UHD_SCCTRL_BASE + 0x3B0)
    #define UHD_SET_DATA_HTOT Fld(16,16,AC_FULLW32)//[31:16]
    #define UHD_SET_DATA_HACT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_2D (IO_UHD_SCCTRL_BASE + 0x3B4)
    #define UHD_SET_DATA_VTOT Fld(16,0,AC_FULLW10)//[15:0]
#define UHD_SCSYS_2E (IO_UHD_SCCTRL_BASE + 0x3B8)
    #define UHD_STALL_DLY Fld(2,0,AC_MSKB0)//[1:0]


//Page UHD_SCFIR
#define UHD_SCFIR_00 (IO_UHD_SCCTRL_BASE + 0x500)
    #define UHD_SCFIR_00_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define UHD_SCFIR_00_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define UHD_SCFIR_00_RDATA_BYTE_EN_SEL Fld(2,14,AC_MSKB1)//[15:14]
    #define UHD_SCFIR_00_WDATA_BYTE_EN_SEL Fld(2,12,AC_MSKB1)//[13:12]
    #define UHD_SCFIR_00_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define UHD_SCFIR_00_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define UHD_SCFIR_00_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define UHD_SCFIR_00_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define UHD_SCFIR_00_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define UHD_SCFIR_01 (IO_UHD_SCCTRL_BASE + 0x504)
    #define UHD_SCFIR_01_CPU_SEL_SRAM_0 Fld(1,7,AC_MSKB0)//[7:7]
    #define UHD_SCFIR_01_CPURW_ACTIVE Fld(1,6,AC_MSKB0)//[6:6]
    #define UHD_SCFIR_01_TABLE_WAIT_VSYNC Fld(1,5,AC_MSKB0)//[5:5]
    #define UHD_SCFIR_01_FILL_TABLE_READY Fld(1,4,AC_MSKB0)//[4:4]
    #define UHD_SCFIR_01_READ_FINISH Fld(1,3,AC_MSKB0)//[3:3]
    #define UHD_SCFIR_01_READ Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_SCFIR_01_WRITE_FINISH Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_SCFIR_01_WRITE Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCFIR_02 (IO_UHD_SCCTRL_BASE + 0x508)
    #define UHD_SCFIR_02_COE_SEL Fld(2,30,AC_MSKB3)//[31:30]
    #define UHD_SCFIR_02_CPU_SRAM_ADDR Fld(8,16,AC_FULLB2)//[23:16]
    #define UHD_SCFIR_02_R_ADDR Fld(8,0,AC_FULLB0)//[7:0]
#define UHD_SCFIR_03 (IO_UHD_SCCTRL_BASE + 0x50C)
    #define UHD_SCFIR_03_WDATA_00 Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCFIR_04 (IO_UHD_SCCTRL_BASE + 0x510)
    #define UHD_SCFIR_04_WDATA_01 Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCFIR_05 (IO_UHD_SCCTRL_BASE + 0x514)
    #define UHD_SCFIR_05_WDATA_02 Fld(30,0,AC_MSKDW)//[29:0]
#define UHD_SCFIR_06 (IO_UHD_SCCTRL_BASE + 0x518)
    #define UHD_SCFIR_06_RDATA_00 Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCFIR_07 (IO_UHD_SCCTRL_BASE + 0x51C)
    #define UHD_SCFIR_07_RDATA_01 Fld(32,0,AC_FULLDW)//[31:0]
#define UHD_SCFIR_08 (IO_UHD_SCCTRL_BASE + 0x520)
    #define UHD_SCFIR_08_RDATA_02 Fld(30,0,AC_MSKDW)//[29:0]
#define UHD_SCFIR_09 (IO_UHD_SCCTRL_BASE + 0x524)
    #define UHD_SCFIR_09_DE_RING_EN_V Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_SCFIR_09_DE_RING_SEL_V Fld(2,28,AC_MSKB3)//[29:28]
    #define UHD_SCFIR_09_DE_RING_SLOPE_V Fld(4,20,AC_MSKB2)//[23:20]
    #define UHD_SCFIR_09_DE_RING_MAX_THRESHOLD_V Fld(8,12,AC_MSKW21)//[19:12]
#define UHD_SCFIR_0A (IO_UHD_SCCTRL_BASE + 0x528)
    #define UHD_SCFIR_0A_DE_RING_EN_H_YUV Fld(1,31,AC_MSKB3)//[31:31]
    #define UHD_SCFIR_0A_DE_RING_SEL_H_YUV Fld(2,28,AC_MSKB3)//[29:28]
    #define UHD_SCFIR_0A_DE_RING_SLOPE_H_Y Fld(4,20,AC_MSKB2)//[23:20]
    #define UHD_SCFIR_0A_DE_RING_MAX_THRESHOLD_H_Y Fld(8,12,AC_MSKW21)//[19:12]
    #define UHD_SCFIR_0A_DE_RING_FIRST_SLOPE_H_Y Fld(4,8,AC_MSKB1)//[11:8]
    #define UHD_SCFIR_0A_DE_RING_FIRST_THRESHOLD_H_Y Fld(8,0,AC_FULLB0)//[7:0]
#define UHD_SCFIR_0B (IO_UHD_SCCTRL_BASE + 0x52C)
    #define UHD_SCFIR_0B_RING_THR_H_UV Fld(8,20,AC_MSKW32)//[27:20]
    #define UHD_SCFIR_0B_SLOPE_SEL_H_UV Fld(4,16,AC_MSKB2)//[19:16]
    #define UHD_SCFIR_0B_RING_1ST_THR_H_UV Fld(8,8,AC_FULLB1)//[15:8]
    #define UHD_SCFIR_0B_SLOPE_1ST_SEL_H_UV Fld(4,4,AC_MSKB0)//[7:4]
#define UHD_SCFIR_0C (IO_UHD_SCCTRL_BASE + 0x530)
    #define UHD_SCFIR_0C_H_CV422_COEFF Fld(1,2,AC_MSKB0)//[2:2]
    #define UHD_SCFIR_0C_H_SRAM_COEFF_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_SCFIR_0C_V_SRAM_COEFF_EN Fld(1,0,AC_MSKB0)//[0:0]
#define UHD_SCFIR_ATPG (IO_UHD_SCCTRL_BASE + 0x5FC)
    #define UHD_SCFIR_ATPG_UHD_SCFIR_ATPG_OB Fld(1,1,AC_MSKB0)//[1:1]
    #define UHD_SCFIR_ATPG_UHD_SCFIR_ATPG_CT Fld(1,0,AC_MSKB0)//[0:0]

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//Page 2D_GFX
//----------------------------------------------------------------------
#define G_CONFIG (IO_UHD_2DGFX_BASE + 0x000)
    #define G_RST_RISC Fld(2,30,AC_MSKB3)//[31:30]
    #define CQ_RST Fld(2,28,AC_MSKB3)//[29:28]
    #define G_RST_CMDQ Fld(2,26,AC_MSKB3)//[27:26]
    #define GFX2D_REG_RST Fld(2,24,AC_MSKB3)//[25:24]
    #define DISABLE_BWSAVE Fld(1,18,AC_MSKB2)//[18:18]
    #define ENG_LP Fld(1,17,AC_MSKB2)//[17:17]
    #define DISABLE_LAST Fld(1,12,AC_MSKB1)//[12:12]
    #define CHECKSUM_SEL Fld(1,11,AC_MSKB1)//[11:11]
    #define SDFIFO_THRS Fld(2,9,AC_MSKB1)//[10:9]
    #define REQ_INTVAL Fld(2,6,AC_MSKB0)//[7:6]
    #define CMDFIFO_THRS_ Fld(2,4,AC_MSKB0)//[5:4]
    #define POST_THRS Fld(2,2,AC_MSKB0)//[3:2]
    #define INT_MASK Fld(1,1,AC_MSKB0)//[1:1]
    #define EN_DRAMQ Fld(1,0,AC_MSKB0)//[0:0]
#define G_STATUS (IO_UHD_2DGFX_BASE + 0x004)
    #define CURR_Y_LINE Fld(11,16,AC_MSKW32)//[26:16]
    #define VERSION_ID Fld(8,8,AC_FULLB1)//[15:8]
    #define HWQ_LEN Fld(5,1,AC_MSKB0)//[5:1]
    #define IDLE Fld(1,0,AC_MSKB0)//[0:0]
#define DRAMQ_STAD (IO_UHD_2DGFX_BASE + 0x008)
    #define CYC_SIZE Fld(2,30,AC_MSKB3)//[31:30]
    #define DRAMQ_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define DRAMQ_LEN_ (IO_UHD_2DGFX_BASE + 0x00C)
    #define DRAMQ_LEN Fld(18,0,AC_MSKDW)//[17:0]
#define G_MODE (IO_UHD_2DGFX_BASE + 0x010)
    #define THIRD_CM Fld(4,28,AC_MSKB3)//[31:28]
    #define THIRD_BUF_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define ALCOM_DIFF_CM Fld(1,26,AC_MSKB3)//[26:26]
    #define ALCOM_SRC_KEY_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define ALCOM_THIRD_KEY_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define APPEND_BPCOMP Fld(1,23,AC_MSKB2)//[23:23]
    #define USE_RECT_SRC Fld(1,20,AC_MSKB2)//[20:20]
    #define SRCOWN Fld(1,17,AC_MSKB2)//[17:17]
    #define DSTOWN Fld(1,16,AC_MSKB2)//[16:16]
    #define BURST_READ_MODE Fld(2,13,AC_MSKB1)//[14:13]
    #define BURST_READ_EN Fld(1,12,AC_MSKB1)//[12:12]
    #define FIRE Fld(1,11,AC_MSKB1)//[11:11]
    #define CM Fld(4,0,AC_MSKB0)//[3:0]
#define RECT_COLOR (IO_UHD_2DGFX_BASE + 0x014)
    #define GFX2D_RECT_COLOR Fld(32,0,AC_FULLDW)//[31:0]
#define SRC_BSAD (IO_UHD_2DGFX_BASE + 0x018)
    #define CHAR_CM Fld(2,30,AC_MSKB3)//[31:30]
    #define GFX2D_SRC_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define DST_BSAD (IO_UHD_2DGFX_BASE + 0x01C)
    #define GFX2D_DST_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define SRC_SIZE (IO_UHD_2DGFX_BASE + 0x028)
    #define SPU_RLDEC Fld(1,31,AC_MSKB3)//[31:31]
    #define SRC_CM Fld(4,27,AC_MSKB3)//[30:27]
    #define SRC_HEIGHT Fld(11,16,AC_MSKW32)//[26:16]
    #define RL_DEC Fld(1,15,AC_MSKB1)//[15:15]
    #define SRC_WIDTH Fld(15,0,AC_MSKW10)//[14:0]
#define S_OSD_WIDTH (IO_UHD_2DGFX_BASE + 0x02C)
    #define SRC_PITCH Fld(16,16,AC_FULLW32)//[31:16]
    #define OSD_WIDTH Fld(16,0,AC_FULLW10)//[15:0]
#define GRAD_X_DELTA (IO_UHD_2DGFX_BASE + 0x038)
    #define DELTA_X_C3 Fld(8,24,AC_FULLB3)//[31:24]
    #define DELTA_X_C2 Fld(8,16,AC_FULLB2)//[23:16]
    #define DELTA_X_C1 Fld(8,8,AC_FULLB1)//[15:8]
    #define DELTA_X_C0_ Fld(8,0,AC_FULLB0)//[7:0]
#define GRAD_Y_DELTA (IO_UHD_2DGFX_BASE + 0x03C)
    #define DELTA_Y_C3 Fld(8,24,AC_FULLB3)//[31:24]
    #define DELTA_Y_C2 Fld(8,16,AC_FULLB2)//[23:16]
    #define DELTA_Y_C1 Fld(8,8,AC_FULLB1)//[15:8]
    #define DELTA_Y_C0_ Fld(8,0,AC_FULLB0)//[7:0]
#define GRAD_XY_INC (IO_UHD_2DGFX_BASE + 0x040)
    #define GRAD_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define GRAD_Y_PIX_INC Fld(11,16,AC_MSKW32)//[26:16]
    #define GRAD_X_PIX_INC Fld(11,0,AC_MSKW10)//[10:0]
#define BITBLT_MODE (IO_UHD_2DGFX_BASE + 0x044)
    #define DST_RD_ROTATE Fld(1,29,AC_MSKB3)//[29:29]
    #define DST_WR_ROTATE Fld(1,28,AC_MSKB3)//[28:28]
    #define SRC_MIRROR Fld(1,27,AC_MSKB3)//[27:27]
    #define SRC_PITCH_DEC Fld(1,26,AC_MSKB3)//[26:26]
    #define DST_MIRROR Fld(1,25,AC_MSKB3)//[25:25]
    #define DST_PITCH_DEC Fld(1,24,AC_MSKB3)//[24:24]
    #define IDX2DIR_SRC_PITCH_EN Fld(1,23,AC_MSKB2)//[23:23]
    #define ALPHA_VALUE Fld(8,8,AC_FULLB1)//[15:8]
    #define FORCE_USE_ALPHA Fld(1,7,AC_MSKB0)//[7:7]
    #define KEYSDSEL Fld(1,5,AC_MSKB0)//[5:5]
    #define CFMT_ENA_ Fld(1,4,AC_MSKB0)//[4:4]
    #define COLCHG_ENA Fld(1,2,AC_MSKB0)//[2:2]
    #define KEYNOT_ENA Fld(1,1,AC_MSKB0)//[1:1]
    #define TRANS_ENA Fld(1,0,AC_MSKB0)//[0:0]
#define KEY_DATA0 (IO_UHD_2DGFX_BASE + 0x048)
    #define COLOR_KEY_MIN Fld(32,0,AC_FULLDW)//[31:0]
#define KEY_DATA1 (IO_UHD_2DGFX_BASE + 0x04C)
    #define COLOR_KEY_MAX Fld(32,0,AC_FULLDW)//[31:0]
#define SRCCBCR_BSAD (IO_UHD_2DGFX_BASE + 0x050)
    #define YC_FMT Fld(2,30,AC_MSKB3)//[31:30]
    #define GFX2D_SRCCBCR_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define SRCCBCR_PITCH (IO_UHD_2DGFX_BASE + 0x054)
    #define VFULL Fld(1,23,AC_MSKB2)//[23:23]
    #define FLD_PIC Fld(1,19,AC_MSKB2)//[19:19]
    #define VSCLIP Fld(1,18,AC_MSKB2)//[18:18]
    #define VSYS Fld(1,17,AC_MSKB2)//[17:17]
    #define VSTD Fld(1,16,AC_MSKB2)//[16:16]
    #define SRCCBCR_PITCH_ Fld(16,0,AC_FULLW10)//[15:0]
#define F_COLOR (IO_UHD_2DGFX_BASE + 0x05C)
    #define FORE_COLOR Fld(32,0,AC_FULLDW)//[31:0]
#define B_COLOR (IO_UHD_2DGFX_BASE + 0x060)
    #define BACK_COLOR Fld(32,0,AC_FULLDW)//[31:0]
#define STR_BLT_H (IO_UHD_2DGFX_BASE + 0x084)
    #define GFX2D_STR_BLT_H Fld(24,0,AC_MSKDW)//[23:0]
#define STR_BLT_V (IO_UHD_2DGFX_BASE + 0x088)
    #define GFX2D_STR_BLT_V Fld(24,0,AC_MSKDW)//[23:0]
#define STR_DST_SIZE (IO_UHD_2DGFX_BASE + 0x08C)
    #define STR_DST_HEIGHT Fld(11,16,AC_MSKW32)//[26:16]
    #define STR_DST_WIDTH Fld(15,0,AC_MSKW10)//[14:0]
#define LEGAL_START (IO_UHD_2DGFX_BASE + 0x090)
    #define WR_PROT_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define LEGAL_AD_START Fld(30,0,AC_MSKDW)//[29:0]
#define LEGAL_END (IO_UHD_2DGFX_BASE + 0x094)
    #define LEGAL_AD_END Fld(30,0,AC_MSKDW)//[29:0]
#define ALCOM_LOOP (IO_UHD_2DGFX_BASE + 0x09C)
    #define PRE_COLORIZE Fld(1,26,AC_MSKB3)//[26:26]
    #define SRC_ALPHA_REPLACE Fld(1,25,AC_MSKB3)//[25:25]
    #define ALPHA_0_RGB_0 Fld(1,24,AC_MSKB3)//[24:24]
    #define ALCOM_MAX_ALPHA Fld(1,23,AC_MSKB2)//[23:23]
    #define PREMULT_OVERFLOW_EN Fld(1,21,AC_MSKB2)//[21:21]
    #define PREMULT_SRC_RD Fld(1,20,AC_MSKB2)//[20:20]
    #define PREMULT_DST_RD Fld(1,19,AC_MSKB2)//[19:19]
    #define PREMULT_DST_WR Fld(1,18,AC_MSKB2)//[18:18]
    #define ALCOM_NORMAL Fld(1,17,AC_MSKB2)//[17:17]
    #define ALCOM_RECT_SRC Fld(1,16,AC_MSKB2)//[16:16]
    #define ALCOM_OPCODE_ Fld(4,8,AC_MSKB1)//[11:8]
    #define ALCOM_AR Fld(8,0,AC_FULLB0)//[7:0]
#define ROP (IO_UHD_2DGFX_BASE + 0x0A0)
    #define CMP_FLAG Fld(1,9,AC_MSKB1)//[9:9]
    #define NO_WR Fld(1,8,AC_MSKB1)//[8:8]
    #define ROP_OPCODE_ Fld(4,0,AC_MSKB0)//[3:0]
#define IDX2DIR (IO_UHD_2DGFX_BASE + 0x0A4)
    #define LN_ST_BYTE_AL Fld(1,31,AC_MSKB3)//[31:31]
    #define MSB_LEFT Fld(1,30,AC_MSKB3)//[30:30]
    #define PAL_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define BPCOMP_CFG (IO_UHD_2DGFX_BASE + 0x0AC)
    #define LINE_SEPARATE Fld(1,3,AC_MSKB0)//[3:3]
    #define QUALITY_MODE Fld(2,1,AC_MSKB0)//[2:1]
    #define ROLL_BACK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define BPCOMP_AD_END (IO_UHD_2DGFX_BASE + 0x0B0)
    #define UHD_BPCOMP_AD_END Fld(30,0,AC_MSKDW)//[29:0]
#define BPCOMP_DBG1 (IO_UHD_2DGFX_BASE + 0x0B4)
    #define BPCOMP_STOP Fld(1,31,AC_MSKB3)//[31:31]
    #define BPCOMP_NIPPLE Fld(26,0,AC_MSKDW)//[25:0]
#define THIRD_BSAD (IO_UHD_2DGFX_BASE + 0x0D0)
    #define THIRD_CHAR_CM Fld(2,30,AC_MSKB3)//[31:30]
    #define GFX2D_THIRD_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define THIRD_IDX2DIR (IO_UHD_2DGFX_BASE + 0x0D4)
    #define IDX2DIR_THIRD_LN_ST_BYTE_AL Fld(1,31,AC_MSKB3)//[31:31]
    #define IDX2DIR_THIRD_MSB_LEFT Fld(1,30,AC_MSKB3)//[30:30]
    #define IDX2DIR_THIRD_PAL_BSAD Fld(30,0,AC_MSKDW)//[29:0]
#define THIRD_PITCH (IO_UHD_2DGFX_BASE + 0x0D8)
    #define GFX2D_THIRD_PITCH Fld(16,0,AC_FULLW10)//[15:0]
#define ALCOM_SRC_KEY (IO_UHD_2DGFX_BASE + 0x0DC)
    #define UHD_ALCOM_SRC_KEY Fld(32,0,AC_FULLDW)//[31:0]
#define ALCOM_THIRD_KEY (IO_UHD_2DGFX_BASE + 0x0E0)
    #define UHD_ALCOM_THIRD_KEY Fld(32,0,AC_FULLDW)//[31:0]
#define ARGB_ORDER (IO_UHD_2DGFX_BASE + 0x114)
    #define DST_WR_A_ORDER Fld(2,30,AC_MSKB3)//[31:30]
    #define DST_WR_R_ORDER Fld(2,28,AC_MSKB3)//[29:28]
    #define DST_WR_G_ORDER Fld(2,26,AC_MSKB3)//[27:26]
    #define DST_WR_B_ORDER Fld(2,24,AC_MSKB3)//[25:24]
    #define SRC_RD_A_ORDER Fld(2,14,AC_MSKB1)//[15:14]
    #define SRC_RD_R_ORDER Fld(2,12,AC_MSKB1)//[13:12]
    #define SRC_RD_G_ORDER Fld(2,10,AC_MSKB1)//[11:10]
    #define SRC_RD_B_ORDER Fld(2,8,AC_MSKB1)//[9:8]
    #define DST_RD_A_ORDER Fld(2,6,AC_MSKB0)//[7:6]
    #define DST_RD_R_ORDER Fld(2,4,AC_MSKB0)//[5:4]
    #define DST_RD_G_ORDER Fld(2,2,AC_MSKB0)//[3:2]
    #define DST_RD_B_ORDER Fld(2,0,AC_MSKB0)//[1:0]
#define ADDR_MSB_1 (IO_UHD_2DGFX_BASE + 0x118)
    #define MBS1_BPCOMP_AD_END Fld(2,24,AC_MSKB3)//[25:24]
    #define MBS1_SRCCBCR_BSAD Fld(2,20,AC_MSKB2)//[21:20]
    #define MBS1_DST_BSAD Fld(2,16,AC_MSKB2)//[17:16]
    #define MBS1_SRC_BSAD Fld(2,12,AC_MSKB1)//[13:12]
    #define MBS1_PAL_BSAD Fld(2,8,AC_MSKB1)//[9:8]
    #define MBS1_THIRD_BSAD Fld(2,4,AC_MSKB0)//[5:4]
    #define MBS1_THIRD_PAL_BSAD Fld(2,0,AC_MSKB0)//[1:0]
#define ADDR_MSB_2 (IO_UHD_2DGFX_BASE + 0x11C)
    #define MSB2_DRAMQ_BSAD Fld(2,8,AC_MSKB1)//[9:8]
    #define MSB2_LEGAL_AD_START Fld(2,4,AC_MSKB0)//[5:4]
    #define MSB2_LEGAL_AD_END Fld(2,0,AC_MSKB0)//[1:0]
#define ADDR_MSB_3 (IO_UHD_2DGFX_BASE + 0x120)
    #define AWR_ADDR_START Fld(2,4,AC_MSKB0)//[5:4]
    #define AWR_ADDR_END Fld(2,0,AC_MSKB0)//[1:0]
#define STR_H_INIT_PHASE (IO_UHD_2DGFX_BASE + 0x124)
    #define UHD_STR_H_INIT_PHASE Fld(25,0,AC_MSKDW)//[24:0]
#define STR_V_INIT_PHASE (IO_UHD_2DGFX_BASE + 0x128)
    #define UHD_STR_V_INIT_PHASE Fld(25,0,AC_MSKDW)//[24:0]
#define WAIT_MMU (IO_UHD_2DGFX_BASE + 0x12C)
    #define IDX2DIR_REQ_MORE2 Fld(1,25,AC_MSKB3)//[25:25]
    #define CG_LARB_MCLK Fld(1,24,AC_MSKB3)//[24:24]
#define XOR_COLOR (IO_UHD_2DGFX_BASE + 0x130)
    #define UHD_XOR_COLOR Fld(32,0,AC_FULLDW)//[31:0]
#define CMDQDBG2 (IO_UHD_2DGFX_BASE + 0x168)
    #define QUERDPTR Fld(14,16,AC_MSKW32)//[29:16]
#define RESERVED1 (IO_UHD_2DGFX_BASE + 0x284)
    #define YUYV2Y Fld(1,16,AC_MSKB2)//[16:16]
    #define SRC_HEIGHT11 Fld(1,8,AC_MSKB1)//[8:8]
    #define FORCE_REQ_INTERVAL Fld(1,0,AC_MSKB0)//[0:0]
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

#endif