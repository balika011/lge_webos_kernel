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

#define IO_UHD_PROC2_BASE 0xF000A000

//Page COLOR
#define CFG_MAIN (IO_UHD_PROC2_BASE + 0x000)
    #define YCURVE_33_EN Fld(1,26,AC_MSKB3)//[26:26]
    #define COLOR_YHIST_TYPE Fld(2,24,AC_MSKB3)//[25:24]
    #define LCN_SAT_LIMIT Fld(1,14,AC_MSKB1)//[14:14]
    #define SEQ_SEL Fld(1,13,AC_MSKB1)//[13:13]
    #define ENG_SWAP Fld(1,11,AC_MSKB1)//[11:11]
    #define YHIST_WGT_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define LCMP_BYPASS Fld(1,8,AC_MSKB1)//[8:8]
    #define ALLBP Fld(1,7,AC_MSKB0)//[7:7]
    #define CCMP_BP Fld(1,6,AC_MSKB0)//[6:6]
    #define ADAP_LUMA_BP Fld(1,5,AC_MSKB0)//[5:5]
    #define HEBP Fld(1,4,AC_MSKB0)//[4:4]
    #define SEBP Fld(1,3,AC_MSKB0)//[3:3]
    #define YEBP Fld(1,2,AC_MSKB0)//[2:2]
    #define P2CBP Fld(1,1,AC_MSKB0)//[1:1]
    #define C2PBP Fld(1,0,AC_MSKB0)//[0:0]
#define PXL_CNT_MAIN (IO_UHD_PROC2_BASE + 0x004)
    #define H_CONT Fld(16,0,AC_FULLW10)//[15:0]
#define LINE_CNT_MAIN (IO_UHD_PROC2_BASE + 0x008)
    #define V_CONT Fld(16,16,AC_FULLW32)//[31:16]
    #define LINE_CONT Fld(16,0,AC_FULLW10)//[15:0]
#define WIN_X_MAIN (IO_UHD_PROC2_BASE + 0x00C)
    #define WIN_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define WIN_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define WIN_Y_MAIN (IO_UHD_PROC2_BASE + 0x010)
    #define WIN_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define WIN_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define CH_DLY_MAIN (IO_UHD_PROC2_BASE + 0x014)
    #define CR_DEL Fld(3,6,AC_MSKW10)//[8:6]
    #define CB_DEL Fld(3,3,AC_MSKB0)//[5:3]
    #define Y_DEL Fld(3,0,AC_MSKB0)//[2:0]
#define TIMING_DETECTION_0 (IO_UHD_PROC2_BASE + 0x018)
    #define VTOTAL_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define HTOTAL_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define TIMING_DETECTION_1 (IO_UHD_PROC2_BASE + 0x01C)
    #define VDE_CNT Fld(16,16,AC_FULLW32)//[31:16]
    #define HDE_CNT Fld(16,0,AC_FULLW10)//[15:0]
#define DBG_CFG_MAIN (IO_UHD_PROC2_BASE + 0x020)
    #define SRS_EN Fld(1,27,AC_MSKB3)//[27:27]
    #define YSH_PTG_MODE Fld(2,25,AC_MSKB3)//[26:25]
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
    #define CAP_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define INK_EN Fld(1,0,AC_MSKB0)//[0:0]
#define C_BOOST_MAIN (IO_UHD_PROC2_BASE + 0x024)
    #define NEW_CBOOST_LMT_U Fld(8,24,AC_FULLB3)//[31:24]
    #define NEW_CBOOST_LMT_L Fld(8,16,AC_FULLB2)//[23:16]
    #define NEW_CBOOST_EN Fld(1,13,AC_MSKB1)//[13:13]
    #define CBOOST_YOFFSET_SEL Fld(2,11,AC_MSKB1)//[12:11]
    #define C_BOOST_LUMA_THD Fld(3,8,AC_MSKB1)//[10:8]
    #define CBOOST_GAIN Fld(8,0,AC_FULLB0)//[7:0]
#define C_BOOST_MAIN_2 (IO_UHD_PROC2_BASE + 0x028)
    #define C_NEW_BOOST_LMT_U2 Fld(8,24,AC_FULLB3)//[31:24]
    #define C_NEW_BOOST_LMT_L2 Fld(8,16,AC_FULLB2)//[23:16]
    #define HUE_HIST_STH Fld(8,8,AC_FULLB1)//[15:8]
    #define CBOOST_YOFFSET Fld(8,0,AC_FULLB0)//[7:0]
#define LUMA_ADJ (IO_UHD_PROC2_BASE + 0x02C)
    #define Y_SLOPE_LMT Fld(8,8,AC_FULLB1)//[15:8]
    #define Y_LEV_ADJ Fld(8,0,AC_FULLB0)//[7:0]
#define G_PIC_ADJ_MAIN_1 (IO_UHD_PROC2_BASE + 0x030)
    #define BRIGHTNESS Fld(11,16,AC_MSKW32)//[26:16]
    #define CONTRAST Fld(10,0,AC_MSKW10)//[9:0]
#define G_PIC_ADJ_MAIN_2 (IO_UHD_PROC2_BASE + 0x034)
    #define HUE Fld(10,16,AC_MSKW32)//[25:16]
    #define SATURATION Fld(10,0,AC_MSKW10)//[9:0]
#define G_PIC_ADJ_MAIN_3 (IO_UHD_PROC2_BASE + 0x038)
    #define POST_HUE Fld(10,0,AC_MSKW10)//[9:0]
#define POS_MAIN (IO_UHD_PROC2_BASE + 0x03C)
    #define POS_Y Fld(16,16,AC_FULLW32)//[31:16]
    #define POS_X Fld(16,0,AC_FULLW10)//[15:0]
#define INK_DATA_MAIN (IO_UHD_PROC2_BASE + 0x040)
    #define INK_DATA_CB Fld(10,16,AC_MSKW32)//[25:16]
    #define INK_DATA_Y Fld(10,0,AC_MSKW10)//[9:0]
#define INK_DATA_MAIN_CR (IO_UHD_PROC2_BASE + 0x044)
    #define INK_DATA_CR Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_IN_DATA_MAIN (IO_UHD_PROC2_BASE + 0x048)
    #define CAP_IN_CB Fld(10,16,AC_MSKW32)//[25:16]
    #define CAP_IN_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_IN_DATA_MAIN_CR (IO_UHD_PROC2_BASE + 0x04C)
    #define CAP_IN_CR Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN (IO_UHD_PROC2_BASE + 0x050)
    #define CAP_OUT_CB Fld(14,16,AC_MSKW32)//[29:16]
    #define CAP_OUT_Y Fld(10,0,AC_MSKW10)//[9:0]
#define CAP_OUT_DATA_MAIN_CR (IO_UHD_PROC2_BASE + 0x054)
    #define CAP_OUT_CR Fld(14,0,AC_MSKW10)//[13:0]
#define Y_SLOPE_1_0_MAIN (IO_UHD_PROC2_BASE + 0x058)
    #define Y_SLOPE_1 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_0 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_3_2_MAIN (IO_UHD_PROC2_BASE + 0x05C)
    #define Y_SLOPE_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_2 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_5_4_MAIN (IO_UHD_PROC2_BASE + 0x060)
    #define Y_SLOPE_5 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_4 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_7_6_MAIN (IO_UHD_PROC2_BASE + 0x064)
    #define Y_SLOPE_7 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_6 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_9_8_MAIN (IO_UHD_PROC2_BASE + 0x068)
    #define Y_SLOPE_9 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_8 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_11_10_MAIN (IO_UHD_PROC2_BASE + 0x06C)
    #define Y_SLOPE_11 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_10 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_13_12_MAIN (IO_UHD_PROC2_BASE + 0x070)
    #define Y_SLOPE_13 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_12 Fld(8,0,AC_FULLB0)//[7:0]
#define Y_SLOPE_15_14_MAIN (IO_UHD_PROC2_BASE + 0x074)
    #define Y_SLOPE_15 Fld(8,16,AC_FULLB2)//[23:16]
    #define Y_SLOPE_14 Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_1 (IO_UHD_PROC2_BASE + 0x078)
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
#define LOCAL_SAT_2 (IO_UHD_PROC2_BASE + 0x07C)
    #define SAT_CPU_SRAM_ADDR Fld(5,16,AC_MSKB2)//[20:16]
    #define R_SAT_ADDR Fld(5,0,AC_MSKB0)//[4:0]
#define LOCAL_SAT_3 (IO_UHD_PROC2_BASE + 0x080)
    #define SAT_G2_W Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G1_W Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_P2_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_P1_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_4 (IO_UHD_PROC2_BASE + 0x084)
    #define SAT_G_Y128_W Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G_Y64_W Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_G_Y0_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G3_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_5 (IO_UHD_PROC2_BASE + 0x088)
    #define LCMP_M11_W Fld(10,16,AC_MSKW32)//[25:16]
    #define SAT_G_Y256_W Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G_Y192_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_6 (IO_UHD_PROC2_BASE + 0x08C)
    #define LCMP_M21_W Fld(10,20,AC_MSKW32)//[29:20]
    #define LCMP_M13_W Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M12_W Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_7 (IO_UHD_PROC2_BASE + 0x090)
    #define LCMP_M23_W Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M22_W Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_8 (IO_UHD_PROC2_BASE + 0x094)
    #define SAT_G2_R Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G1_R Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_P2_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_P1_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_9 (IO_UHD_PROC2_BASE + 0x098)
    #define SAT_G_Y128_R Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_G_Y64_R Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_G_Y0_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G3_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_10 (IO_UHD_PROC2_BASE + 0x09C)
    #define LCMP_M11_R Fld(10,16,AC_MSKW32)//[25:16]
    #define SAT_G_Y256_R Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_G_Y192_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_SAT_11 (IO_UHD_PROC2_BASE + 0x0A0)
    #define LCMP_M21_R Fld(10,20,AC_MSKW32)//[29:20]
    #define LCMP_M13_R Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M12_R Fld(10,0,AC_MSKW10)//[9:0]
#define LOCAL_SAT_12 (IO_UHD_PROC2_BASE + 0x0A4)
    #define LCMP_M23_R Fld(10,10,AC_MSKW21)//[19:10]
    #define LCMP_M22_R Fld(10,0,AC_MSKW10)//[9:0]
#define CCMP_GROUP_1 (IO_UHD_PROC2_BASE + 0x0A8)
    #define CCMP_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define CCMP_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define CCMP_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define CCMP_0 Fld(8,0,AC_FULLB0)//[7:0]
#define CCMP_GROUP_2 (IO_UHD_PROC2_BASE + 0x0AC)
    #define CCMP_7 Fld(8,24,AC_FULLB3)//[31:24]
    #define CCMP_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define CCMP_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define CCMP_4 Fld(8,0,AC_FULLB0)//[7:0]
#define CCMP_GROUP_3 (IO_UHD_PROC2_BASE + 0x0B0)
    #define CCMP_8 Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_WINDOW_1 (IO_UHD_PROC2_BASE + 0x0B4)
    #define W1_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W1_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W1_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W1_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_WINDOW_2 (IO_UHD_PROC2_BASE + 0x0B8)
    #define W2_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W2_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W2_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W2_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_WINDOW_3 (IO_UHD_PROC2_BASE + 0x0BC)
    #define W3_SAT_UPPER Fld(8,24,AC_FULLB3)//[31:24]
    #define W3_SAT_LOWER Fld(8,16,AC_FULLB2)//[23:16]
    #define W3_HUE_UPPER Fld(8,8,AC_FULLB1)//[15:8]
    #define W3_HUE_LOWER Fld(8,0,AC_FULLB0)//[7:0]
#define TWO_D_W1_RESULT (IO_UHD_PROC2_BASE + 0x0C0)
    #define W1_RESULT Fld(23,0,AC_MSKDW)//[22:0]
#define TWO_D_W2_RESULT (IO_UHD_PROC2_BASE + 0x0C4)
    #define W2_RESULT Fld(23,0,AC_MSKDW)//[22:0]
#define TWO_D_W3_RESULT (IO_UHD_PROC2_BASE + 0x0C8)
    #define W3_RESULT Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_SUM (IO_UHD_PROC2_BASE + 0x0CC)
    #define LUMA_SUM Fld(32,0,AC_FULLDW)//[31:0]
#define LUMA_MIN_MAX (IO_UHD_PROC2_BASE + 0x0D0)
    #define COLOR_INFO_3D_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define COLOR_INFO_3D_EVEN Fld(1,30,AC_MSKB3)//[30:30]
    #define LUMA_SUM_SHIFT Fld(1,29,AC_MSKB3)//[29:29]
    #define SAT_SUM_SHIFT Fld(1,28,AC_MSKB3)//[28:28]
    #define LUMA_MAX Fld(10,16,AC_MSKW32)//[25:16]
    #define LUMA_MIN Fld(10,0,AC_MSKW10)//[9:0]
#define SAT_SUM (IO_UHD_PROC2_BASE + 0x0D4)
    #define SAT_SUM Fld(32,0,AC_FULLDW)//[31:0]
#define LOCAL_HUE_1 (IO_UHD_PROC2_BASE + 0x0D8)
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
#define LOCAL_HUE_2 (IO_UHD_PROC2_BASE + 0x0DC)
    #define HUE_CPU_SRAM_ADDR Fld(5,16,AC_MSKB2)//[20:16]
    #define R_HUE_ADDR Fld(5,0,AC_MSKB0)//[4:0]
#define LOCAL_HUE_3 (IO_UHD_PROC2_BASE + 0x0E0)
    #define Y128_TO_HUE_W Fld(8,24,AC_FULLB3)//[31:24]
    #define Y64_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define Y0_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_4 (IO_UHD_PROC2_BASE + 0x0E4)
    #define S32_TO_HUE_W Fld(8,24,AC_FULLB3)//[31:24]
    #define S0_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define Y256_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define Y192_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_5 (IO_UHD_PROC2_BASE + 0x0E8)
    #define S128_TO_HUE_W Fld(8,16,AC_FULLB2)//[23:16]
    #define S96_TO_HUE_W Fld(8,8,AC_FULLB1)//[15:8]
    #define S64_TO_HUE_W Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_6 (IO_UHD_PROC2_BASE + 0x0EC)
    #define Y128_TO_HUE_R Fld(8,24,AC_FULLB3)//[31:24]
    #define Y64_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define Y0_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_7 (IO_UHD_PROC2_BASE + 0x0F0)
    #define S32_TO_HUE_R Fld(8,24,AC_FULLB3)//[31:24]
    #define S0_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define Y256_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define Y192_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]
#define LOCAL_HUE_8 (IO_UHD_PROC2_BASE + 0x0F4)
    #define S128_TO_HUE_R Fld(8,16,AC_FULLB2)//[23:16]
    #define S96_TO_HUE_R Fld(8,8,AC_FULLB1)//[15:8]
    #define S64_TO_HUE_R Fld(8,0,AC_FULLB0)//[7:0]
#define SAT_HIST_CFG_MAIN (IO_UHD_PROC2_BASE + 0x0F8)
    #define SAT_BOUND_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define SAT_BOUND_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_BOUND_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_BOUND_1 Fld(8,0,AC_FULLB0)//[7:0]
#define SAT_HIST_CFG_MAIN_2 (IO_UHD_PROC2_BASE + 0x0FC)
    #define SAT_BOUND_7 Fld(8,16,AC_FULLB2)//[23:16]
    #define SAT_BOUND_6 Fld(8,8,AC_FULLB1)//[15:8]
    #define SAT_BOUND_5 Fld(8,0,AC_FULLB0)//[7:0]
#define SAT_HIST_X_CFG_MAIN (IO_UHD_PROC2_BASE + 0x100)
    #define SAT_WIN_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_WIN_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_Y_CFG_MAIN (IO_UHD_PROC2_BASE + 0x104)
    #define SAT_WIN_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_WIN_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_1_0_MAIN (IO_UHD_PROC2_BASE + 0x108)
    #define SAT_HIST_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_0 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_3_2_MAIN (IO_UHD_PROC2_BASE + 0x10C)
    #define SAT_HIST_3 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_2 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_5_4_MAIN (IO_UHD_PROC2_BASE + 0x110)
    #define SAT_HIST_5 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_4 Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_7_6_MAIN (IO_UHD_PROC2_BASE + 0x114)
    #define SAT_HIST_7 Fld(16,16,AC_FULLW32)//[31:16]
    #define SAT_HIST_6 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_CFG_MAIN (IO_UHD_PROC2_BASE + 0x120)
    #define HUE_BOUND_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define HUE_BOUND_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define HUE_BOUND_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_BOUND_0 Fld(8,0,AC_FULLB0)//[7:0]
#define HUE_HIST_CFG_MAIN_1 (IO_UHD_PROC2_BASE + 0x124)
    #define HUE_BOUND_7 Fld(8,24,AC_FULLB3)//[31:24]
    #define HUE_BOUND_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define HUE_BOUND_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define HUE_BOUND_4 Fld(8,0,AC_FULLB0)//[7:0]
#define HUE_HIST_1_1_MAIN (IO_UHD_PROC2_BASE + 0x128)
    #define HUE_HIST_1 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_0 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_3_2_MAIN (IO_UHD_PROC2_BASE + 0x12C)
    #define HUE_HIST_3 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_2 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_5_4_MAIN (IO_UHD_PROC2_BASE + 0x130)
    #define HUE_HIST_5 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_4 Fld(16,0,AC_FULLW10)//[15:0]
#define HUE_HIST_7_6_MAIN (IO_UHD_PROC2_BASE + 0x134)
    #define HUE_HIST_7 Fld(16,16,AC_FULLW32)//[31:16]
    #define HUE_HIST_6 Fld(16,0,AC_FULLW10)//[15:0]
#define BITPLUS_1 (IO_UHD_PROC2_BASE + 0x138)
    #define Y_BITPLUS_Q4 Fld(6,26,AC_MSKB3)//[31:26]
    #define Y_BITPLUS_Q3 Fld(6,20,AC_MSKW32)//[25:20]
    #define Y_BITPLUS_Q2 Fld(6,14,AC_MSKW21)//[19:14]
    #define Y_BITPLUS_Q1 Fld(6,8,AC_MSKB1)//[13:8]
    #define Y_BITPLUS_RANDOM_FIX Fld(1,6,AC_MSKB0)//[6:6]
    #define Y_BITPLUS_YRANGE Fld(6,0,AC_MSKB0)//[5:0]
#define BITPLUS_2 (IO_UHD_PROC2_BASE + 0x13C)
    #define Y_BITPLUS_INK_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_BITPLUS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_BITPLUS_SDIFF_SHIFT Fld(3,27,AC_MSKB3)//[29:27]
    #define Y_BITPLUS_P3 Fld(9,18,AC_MSKW32)//[26:18]
    #define Y_BITPLUS_P2 Fld(9,9,AC_MSKW21)//[17:9]
    #define Y_BITPLUS_P1 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_3 (IO_UHD_PROC2_BASE + 0x140)
    #define Y_BITPLUS_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_BITPLUS_RANDOM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_BITPLUS_SHIFT Fld(2,28,AC_MSKB3)//[29:28]
    #define Y_BITPLUS_G34 Fld(6,21,AC_MSKW32)//[26:21]
    #define Y_BITPLUS_G23 Fld(6,15,AC_MSKW21)//[20:15]
    #define Y_BITPLUS_G12 Fld(6,9,AC_MSKB1)//[14:9]
    #define Y_BITPLUS_P4 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_4 (IO_UHD_PROC2_BASE + 0x144)
    #define S_BITPLUS_Q4 Fld(6,26,AC_MSKB3)//[31:26]
    #define S_BITPLUS_Q3 Fld(6,20,AC_MSKW32)//[25:20]
    #define S_BITPLUS_Q2 Fld(6,14,AC_MSKW21)//[19:14]
    #define S_BITPLUS_Q1 Fld(6,8,AC_MSKB1)//[13:8]
    #define S_BITPLUS_RANDOM_FIX Fld(1,6,AC_MSKB0)//[6:6]
    #define S_BITPLUS_SRANGE Fld(6,0,AC_MSKB0)//[5:0]
#define BITPLUS_5 (IO_UHD_PROC2_BASE + 0x148)
    #define S_BITPLUS_INK_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define S_BITPLUS_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define S_BITPLUS_SDIFF_SHIFT Fld(3,27,AC_MSKB3)//[29:27]
    #define S_BITPLUS_P3 Fld(9,18,AC_MSKW32)//[26:18]
    #define S_BITPLUS_P2 Fld(9,9,AC_MSKW21)//[17:9]
    #define S_BITPLUS_P1 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_6 (IO_UHD_PROC2_BASE + 0x14C)
    #define S_BITPLUS_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define S_BITPLUS_RANDOM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define S_BITPLUS_SHIFT Fld(2,28,AC_MSKB3)//[29:28]
    #define S_BITPLUS_G34 Fld(6,21,AC_MSKW32)//[26:21]
    #define S_BITPLUS_G23 Fld(6,15,AC_MSKW21)//[20:15]
    #define S_BITPLUS_G12 Fld(6,9,AC_MSKB1)//[14:9]
    #define S_BITPLUS_P4 Fld(9,0,AC_MSKW10)//[8:0]
#define BITPLUS_7 (IO_UHD_PROC2_BASE + 0x150)
    #define S_BITPLUS_HRANGE Fld(6,0,AC_MSKB0)//[5:0]
#define Y_FTN_1_0_MAIN (IO_UHD_PROC2_BASE + 0x200)
    #define Y_FTN_1 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_0 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_3_2_MAIN (IO_UHD_PROC2_BASE + 0x204)
    #define Y_FTN_3 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_2 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_5_4_MAIN (IO_UHD_PROC2_BASE + 0x208)
    #define Y_FTN_5 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_4 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_7_6_MAIN (IO_UHD_PROC2_BASE + 0x20C)
    #define Y_FTN_7 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_6 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_9_8_MAIN (IO_UHD_PROC2_BASE + 0x210)
    #define Y_FTN_9 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_8 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_11_10_MAIN (IO_UHD_PROC2_BASE + 0x214)
    #define Y_FTN_11 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_10 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_13_12_MAIN (IO_UHD_PROC2_BASE + 0x218)
    #define Y_FTN_13 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_12 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_15_14_MAIN (IO_UHD_PROC2_BASE + 0x21C)
    #define Y_FTN_15 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_14 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_17_16_MAIN (IO_UHD_PROC2_BASE + 0x220)
    #define Y_FTN_17 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_16 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_19_18_MAIN (IO_UHD_PROC2_BASE + 0x224)
    #define Y_FTN_19 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_18 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_21_20_MAIN (IO_UHD_PROC2_BASE + 0x228)
    #define Y_FTN_21 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_20 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_23_22_MAIN (IO_UHD_PROC2_BASE + 0x22C)
    #define Y_FTN_23 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_22 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_25_24_MAIN (IO_UHD_PROC2_BASE + 0x230)
    #define Y_FTN_25 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_24 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_27_26_MAIN (IO_UHD_PROC2_BASE + 0x234)
    #define Y_FTN_27 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_26 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_29_28_MAIN (IO_UHD_PROC2_BASE + 0x238)
    #define Y_FTN_29 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_28 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_31_30_MAIN (IO_UHD_PROC2_BASE + 0x23C)
    #define Y_FTN_31 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_30 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_32_33_MAIN (IO_UHD_PROC2_BASE + 0x240)
    #define Y_FTN_33 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_32 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_35_34_MAIN (IO_UHD_PROC2_BASE + 0x244)
    #define Y_FTN_35 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_34 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_37_36_MAIN (IO_UHD_PROC2_BASE + 0x248)
    #define Y_FTN_37 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_36 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_39_38_MAIN (IO_UHD_PROC2_BASE + 0x24C)
    #define Y_FTN_39 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_38 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_41_40_MAIN (IO_UHD_PROC2_BASE + 0x250)
    #define Y_FTN_41 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_40 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_43_42_MAIN (IO_UHD_PROC2_BASE + 0x254)
    #define Y_FTN_43 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_42 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_45_44_MAIN (IO_UHD_PROC2_BASE + 0x258)
    #define Y_FTN_45 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_44 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_47_46_MAIN (IO_UHD_PROC2_BASE + 0x25C)
    #define Y_FTN_47 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_46 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_49_48_MAIN (IO_UHD_PROC2_BASE + 0x260)
    #define Y_FTN_49 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_48 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_51_50_MAIN (IO_UHD_PROC2_BASE + 0x264)
    #define Y_FTN_51 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_50 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_53_52_MAIN (IO_UHD_PROC2_BASE + 0x268)
    #define Y_FTN_53 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_52 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_55_54_MAIN (IO_UHD_PROC2_BASE + 0x26C)
    #define Y_FTN_55 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_54 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_57_56_MAIN (IO_UHD_PROC2_BASE + 0x270)
    #define Y_FTN_57 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_56 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_59_58_MAIN (IO_UHD_PROC2_BASE + 0x274)
    #define Y_FTN_59 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_58 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_61_60_MAIN (IO_UHD_PROC2_BASE + 0x278)
    #define Y_FTN_61 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_60 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_63_62_MAIN (IO_UHD_PROC2_BASE + 0x27C)
    #define Y_FTN_63 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN_62 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN_64_MAIN (IO_UHD_PROC2_BASE + 0x280)
    #define Y_FTN_64 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_1_0_MAIN (IO_UHD_PROC2_BASE + 0x284)
    #define Y_FTN2_1 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_0 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_3_2_MAIN (IO_UHD_PROC2_BASE + 0x288)
    #define Y_FTN2_3 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_2 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_5_4_MAIN (IO_UHD_PROC2_BASE + 0x28C)
    #define Y_FTN2_5 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_4 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_7_6_MAIN (IO_UHD_PROC2_BASE + 0x290)
    #define Y_FTN2_7 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_6 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_9_8_MAIN (IO_UHD_PROC2_BASE + 0x294)
    #define Y_FTN2_9 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_8 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_11_10_MAIN (IO_UHD_PROC2_BASE + 0x298)
    #define Y_FTN2_11 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_10 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_13_12_MAIN (IO_UHD_PROC2_BASE + 0x29C)
    #define Y_FTN2_13 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_12 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_15_14_MAIN (IO_UHD_PROC2_BASE + 0x2A0)
    #define Y_FTN2_15 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_14 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_17_16_MAIN (IO_UHD_PROC2_BASE + 0x2A4)
    #define Y_FTN2_17 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_16 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_19_18_MAIN (IO_UHD_PROC2_BASE + 0x2A8)
    #define Y_FTN2_19 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_18 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_21_20_MAIN (IO_UHD_PROC2_BASE + 0x2AC)
    #define Y_FTN2_21 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_20 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_23_22_MAIN (IO_UHD_PROC2_BASE + 0x2B0)
    #define Y_FTN2_23 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_22 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_25_24_MAIN (IO_UHD_PROC2_BASE + 0x2B4)
    #define Y_FTN2_25 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_24 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_27_26_MAIN (IO_UHD_PROC2_BASE + 0x2B8)
    #define Y_FTN2_27 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_26 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_29_28_MAIN (IO_UHD_PROC2_BASE + 0x2BC)
    #define Y_FTN2_29 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_28 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_31_30_MAIN (IO_UHD_PROC2_BASE + 0x2C0)
    #define Y_FTN2_31 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_30 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_33_32_MAIN (IO_UHD_PROC2_BASE + 0x2C4)
    #define Y_FTN2_33 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_32 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_35_34_MAIN (IO_UHD_PROC2_BASE + 0x2C8)
    #define Y_FTN2_35 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_34 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_37_36_MAIN (IO_UHD_PROC2_BASE + 0x2CC)
    #define Y_FTN2_37 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_36 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_39_38_MAIN (IO_UHD_PROC2_BASE + 0x2D0)
    #define Y_FTN2_39 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_38 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_41_40_MAIN (IO_UHD_PROC2_BASE + 0x2D4)
    #define Y_FTN2_41 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_40 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_43_42_MAIN (IO_UHD_PROC2_BASE + 0x2D8)
    #define Y_FTN2_43 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_42 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_45_44_MAIN (IO_UHD_PROC2_BASE + 0x2DC)
    #define Y_FTN2_45 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_44 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_47_46_MAIN (IO_UHD_PROC2_BASE + 0x2E0)
    #define Y_FTN2_47 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_46 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_49_48_MAIN (IO_UHD_PROC2_BASE + 0x2E4)
    #define Y_FTN2_49 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_48 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_51_50_MAIN (IO_UHD_PROC2_BASE + 0x2E8)
    #define Y_FTN2_51 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_50 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_53_52_MAIN (IO_UHD_PROC2_BASE + 0x2EC)
    #define Y_FTN2_53 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_52 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_55_54_MAIN (IO_UHD_PROC2_BASE + 0x2F0)
    #define Y_FTN2_55 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_54 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_57_56_MAIN (IO_UHD_PROC2_BASE + 0x2F4)
    #define Y_FTN2_57 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_56 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_59_58_MAIN (IO_UHD_PROC2_BASE + 0x2F8)
    #define Y_FTN2_59 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_58 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_61_60_MAIN (IO_UHD_PROC2_BASE + 0x2FC)
    #define Y_FTN2_61 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_60 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_63_62_MAIN (IO_UHD_PROC2_BASE + 0x300)
    #define Y_FTN2_63 Fld(10,16,AC_MSKW32)//[25:16]
    #define Y_FTN2_62 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_64_MAIN (IO_UHD_PROC2_BASE + 0x304)
    #define Y_FTN2_64 Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_1 (IO_UHD_PROC2_BASE + 0x308)
    #define Y_FTN2_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define Y_FTN2_INK_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define Y_FTN2_YSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_YMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_YMIN Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_2 (IO_UHD_PROC2_BASE + 0x30C)
    #define Y_FTN2_SSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_SMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_SMIN Fld(10,0,AC_MSKW10)//[9:0]
#define Y_FTN2_SKIN_RANGE_3 (IO_UHD_PROC2_BASE + 0x310)
    #define Y_FTN2_HSLOPE Fld(4,20,AC_MSKB2)//[23:20]
    #define Y_FTN2_HMAX Fld(10,10,AC_MSKW21)//[19:10]
    #define Y_FTN2_HMIN Fld(10,0,AC_MSKW10)//[9:0]
#define LUMA_HIST_00 (IO_UHD_PROC2_BASE + 0x400)
    #define LUMA_HIST_00 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_01 (IO_UHD_PROC2_BASE + 0x404)
    #define LUMA_HIST_01 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_02 (IO_UHD_PROC2_BASE + 0x408)
    #define LUMA_HIST_02 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_03 (IO_UHD_PROC2_BASE + 0x40C)
    #define LUMA_HIST_03 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_04 (IO_UHD_PROC2_BASE + 0x410)
    #define LUMA_HIST_04 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_05 (IO_UHD_PROC2_BASE + 0x414)
    #define LUMA_HIST_05 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_06 (IO_UHD_PROC2_BASE + 0x418)
    #define LUMA_HIST_06 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_07 (IO_UHD_PROC2_BASE + 0x41C)
    #define LUMA_HIST_07 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_08 (IO_UHD_PROC2_BASE + 0x420)
    #define LUMA_HIST_08 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_09 (IO_UHD_PROC2_BASE + 0x424)
    #define LUMA_HIST_09 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_10 (IO_UHD_PROC2_BASE + 0x428)
    #define LUMA_HIST_10 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_11 (IO_UHD_PROC2_BASE + 0x42C)
    #define LUMA_HIST_11 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_12 (IO_UHD_PROC2_BASE + 0x430)
    #define LUMA_HIST_12 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_13 (IO_UHD_PROC2_BASE + 0x434)
    #define LUMA_HIST_13 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_14 (IO_UHD_PROC2_BASE + 0x438)
    #define LUMA_HIST_14 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_15 (IO_UHD_PROC2_BASE + 0x43C)
    #define LUMA_HIST_15 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_16 (IO_UHD_PROC2_BASE + 0x440)
    #define LUMA_HIST_16 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_17 (IO_UHD_PROC2_BASE + 0x444)
    #define LUMA_HIST_17 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_18 (IO_UHD_PROC2_BASE + 0x448)
    #define LUMA_HIST_18 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_19 (IO_UHD_PROC2_BASE + 0x44C)
    #define LUMA_HIST_19 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_20 (IO_UHD_PROC2_BASE + 0x450)
    #define LUMA_HIST_20 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_21 (IO_UHD_PROC2_BASE + 0x454)
    #define LUMA_HIST_21 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_22 (IO_UHD_PROC2_BASE + 0x458)
    #define LUMA_HIST_22 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_23 (IO_UHD_PROC2_BASE + 0x45C)
    #define LUMA_HIST_23 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_24 (IO_UHD_PROC2_BASE + 0x460)
    #define LUMA_HIST_24 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_25 (IO_UHD_PROC2_BASE + 0x464)
    #define LUMA_HIST_25 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_26 (IO_UHD_PROC2_BASE + 0x468)
    #define LUMA_HIST_26 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_27 (IO_UHD_PROC2_BASE + 0x46C)
    #define LUMA_HIST_27 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_28 (IO_UHD_PROC2_BASE + 0x470)
    #define LUMA_HIST_28 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_29 (IO_UHD_PROC2_BASE + 0x474)
    #define LUMA_HIST_29 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_30 (IO_UHD_PROC2_BASE + 0x478)
    #define LUMA_HIST_30 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_31 (IO_UHD_PROC2_BASE + 0x47C)
    #define LUMA_HIST_31 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_32 (IO_UHD_PROC2_BASE + 0x480)
    #define LUMA_HIST_32 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_33 (IO_UHD_PROC2_BASE + 0x484)
    #define LUMA_HIST_33 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_34 (IO_UHD_PROC2_BASE + 0x488)
    #define LUMA_HIST_34 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_35 (IO_UHD_PROC2_BASE + 0x48C)
    #define LUMA_HIST_35 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_36 (IO_UHD_PROC2_BASE + 0x490)
    #define LUMA_HIST_36 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_37 (IO_UHD_PROC2_BASE + 0x494)
    #define LUMA_HIST_37 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_38 (IO_UHD_PROC2_BASE + 0x498)
    #define LUMA_HIST_38 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_39 (IO_UHD_PROC2_BASE + 0x49C)
    #define LUMA_HIST_39 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_40 (IO_UHD_PROC2_BASE + 0x4A0)
    #define LUMA_HIST_40 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_41 (IO_UHD_PROC2_BASE + 0x4A4)
    #define LUMA_HIST_41 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_42 (IO_UHD_PROC2_BASE + 0x4A8)
    #define LUMA_HIST_42 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_43 (IO_UHD_PROC2_BASE + 0x4AC)
    #define LUMA_HIST_43 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_44 (IO_UHD_PROC2_BASE + 0x4B0)
    #define LUMA_HIST_44 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_45 (IO_UHD_PROC2_BASE + 0x4B4)
    #define LUMA_HIST_45 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_46 (IO_UHD_PROC2_BASE + 0x4B8)
    #define LUMA_HIST_46 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_47 (IO_UHD_PROC2_BASE + 0x4BC)
    #define LUMA_HIST_47 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_48 (IO_UHD_PROC2_BASE + 0x4C0)
    #define LUMA_HIST_48 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_49 (IO_UHD_PROC2_BASE + 0x4C4)
    #define LUMA_HIST_49 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_50 (IO_UHD_PROC2_BASE + 0x4C8)
    #define LUMA_HIST_50 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_51 (IO_UHD_PROC2_BASE + 0x4CC)
    #define LUMA_HIST_51 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_52 (IO_UHD_PROC2_BASE + 0x4D0)
    #define LUMA_HIST_52 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_53 (IO_UHD_PROC2_BASE + 0x4D4)
    #define LUMA_HIST_53 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_54 (IO_UHD_PROC2_BASE + 0x4D8)
    #define LUMA_HIST_54 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_55 (IO_UHD_PROC2_BASE + 0x4DC)
    #define LUMA_HIST_55 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_56 (IO_UHD_PROC2_BASE + 0x4E0)
    #define LUMA_HIST_56 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_57 (IO_UHD_PROC2_BASE + 0x4E4)
    #define LUMA_HIST_57 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_58 (IO_UHD_PROC2_BASE + 0x4E8)
    #define LUMA_HIST_58 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_59 (IO_UHD_PROC2_BASE + 0x4EC)
    #define LUMA_HIST_59 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_60 (IO_UHD_PROC2_BASE + 0x4F0)
    #define LUMA_HIST_60 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_61 (IO_UHD_PROC2_BASE + 0x4F4)
    #define LUMA_HIST_61 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_62 (IO_UHD_PROC2_BASE + 0x4F8)
    #define LUMA_HIST_62 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_63 (IO_UHD_PROC2_BASE + 0x4FC)
    #define LUMA_HIST_63 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_64 (IO_UHD_PROC2_BASE + 0x500)
    #define LUMA_HIST_64 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_65 (IO_UHD_PROC2_BASE + 0x504)
    #define LUMA_HIST_65 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_66 (IO_UHD_PROC2_BASE + 0x508)
    #define LUMA_HIST_66 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_67 (IO_UHD_PROC2_BASE + 0x50C)
    #define LUMA_HIST_67 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_68 (IO_UHD_PROC2_BASE + 0x510)
    #define LUMA_HIST_68 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_69 (IO_UHD_PROC2_BASE + 0x514)
    #define LUMA_HIST_69 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_70 (IO_UHD_PROC2_BASE + 0x518)
    #define LUMA_HIST_70 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_71 (IO_UHD_PROC2_BASE + 0x51C)
    #define LUMA_HIST_71 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_72 (IO_UHD_PROC2_BASE + 0x520)
    #define LUMA_HIST_72 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_73 (IO_UHD_PROC2_BASE + 0x524)
    #define LUMA_HIST_73 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_74 (IO_UHD_PROC2_BASE + 0x528)
    #define LUMA_HIST_74 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_75 (IO_UHD_PROC2_BASE + 0x52C)
    #define LUMA_HIST_75 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_76 (IO_UHD_PROC2_BASE + 0x530)
    #define LUMA_HIST_76 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_77 (IO_UHD_PROC2_BASE + 0x534)
    #define LUMA_HIST_77 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_78 (IO_UHD_PROC2_BASE + 0x538)
    #define LUMA_HIST_78 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_79 (IO_UHD_PROC2_BASE + 0x53C)
    #define LUMA_HIST_79 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_80 (IO_UHD_PROC2_BASE + 0x540)
    #define LUMA_HIST_80 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_81 (IO_UHD_PROC2_BASE + 0x544)
    #define LUMA_HIST_81 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_82 (IO_UHD_PROC2_BASE + 0x548)
    #define LUMA_HIST_82 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_83 (IO_UHD_PROC2_BASE + 0x54C)
    #define LUMA_HIST_83 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_84 (IO_UHD_PROC2_BASE + 0x550)
    #define LUMA_HIST_84 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_85 (IO_UHD_PROC2_BASE + 0x554)
    #define LUMA_HIST_85 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_86 (IO_UHD_PROC2_BASE + 0x558)
    #define LUMA_HIST_86 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_87 (IO_UHD_PROC2_BASE + 0x55C)
    #define LUMA_HIST_87 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_88 (IO_UHD_PROC2_BASE + 0x560)
    #define LUMA_HIST_88 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_89 (IO_UHD_PROC2_BASE + 0x564)
    #define LUMA_HIST_89 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_90 (IO_UHD_PROC2_BASE + 0x568)
    #define LUMA_HIST_90 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_91 (IO_UHD_PROC2_BASE + 0x56C)
    #define LUMA_HIST_91 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_92 (IO_UHD_PROC2_BASE + 0x570)
    #define LUMA_HIST_92 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_93 (IO_UHD_PROC2_BASE + 0x574)
    #define LUMA_HIST_93 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_94 (IO_UHD_PROC2_BASE + 0x578)
    #define LUMA_HIST_94 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_95 (IO_UHD_PROC2_BASE + 0x57C)
    #define LUMA_HIST_95 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_96 (IO_UHD_PROC2_BASE + 0x580)
    #define LUMA_HIST_96 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_97 (IO_UHD_PROC2_BASE + 0x584)
    #define LUMA_HIST_97 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_98 (IO_UHD_PROC2_BASE + 0x588)
    #define LUMA_HIST_98 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_99 (IO_UHD_PROC2_BASE + 0x58C)
    #define LUMA_HIST_99 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_100 (IO_UHD_PROC2_BASE + 0x590)
    #define LUMA_HIST_100 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_101 (IO_UHD_PROC2_BASE + 0x594)
    #define LUMA_HIST_101 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_102 (IO_UHD_PROC2_BASE + 0x598)
    #define LUMA_HIST_102 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_103 (IO_UHD_PROC2_BASE + 0x59C)
    #define LUMA_HIST_103 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_104 (IO_UHD_PROC2_BASE + 0x5A0)
    #define LUMA_HIST_104 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_105 (IO_UHD_PROC2_BASE + 0x5A4)
    #define LUMA_HIST_105 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_106 (IO_UHD_PROC2_BASE + 0x5A8)
    #define LUMA_HIST_106 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_107 (IO_UHD_PROC2_BASE + 0x5AC)
    #define LUMA_HIST_107 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_108 (IO_UHD_PROC2_BASE + 0x5B0)
    #define LUMA_HIST_108 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_109 (IO_UHD_PROC2_BASE + 0x5B4)
    #define LUMA_HIST_109 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_110 (IO_UHD_PROC2_BASE + 0x5B8)
    #define LUMA_HIST_110 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_111 (IO_UHD_PROC2_BASE + 0x5BC)
    #define LUMA_HIST_111 Fld(23,0,AC_MSKDW)//[22:0]
#define LUMA_HIST_112 (IO_UHD_PROC2_BASE + 0x5C0)
    #define LUMA_HIST_112 Fld(23,0,AC_MSKDW)//[22:0]
#define CRC_0 (IO_UHD_PROC2_BASE + 0x5C4)
    #define CRC_SRC_SEL Fld(4,16,AC_MSKB2)//[19:16]
    #define CRC_STILL_CHECK_MAX Fld(8,8,AC_FULLB1)//[15:8]
    #define CRC_NON_STILL_CNT Fld(4,4,AC_MSKB0)//[7:4]
    #define CRC_STILL_CHECK_DONE Fld(1,3,AC_MSKB0)//[3:3]
    #define C_CRC_STILL_CHECK_TRIG Fld(1,2,AC_MSKB0)//[2:2]
    #define CRC_L_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define CRC_R_EN Fld(1,0,AC_MSKB0)//[0:0]
#define CRC_1 (IO_UHD_PROC2_BASE + 0x5C8)
    #define CRC_CLIP_H_END Fld(13,16,AC_MSKW32)//[28:16]
    #define CRC_CLIP_H_START Fld(13,0,AC_MSKW10)//[12:0]
#define CRC_2 (IO_UHD_PROC2_BASE + 0x5CC)
    #define CRC_CLIP_V_END Fld(12,16,AC_MSKW32)//[27:16]
    #define CRC_CLIP_V_START Fld(12,0,AC_MSKW10)//[11:0]
#define CRC_3 (IO_UHD_PROC2_BASE + 0x5D0)
    #define CRC_Y_MASK Fld(10,20,AC_MSKW32)//[29:20]
    #define CRC_C_MASK Fld(10,10,AC_MSKW21)//[19:10]
    #define CRC_V_MASK Fld(10,0,AC_MSKW10)//[9:0]
#define CRC_4 (IO_UHD_PROC2_BASE + 0x5D4)
    #define CRC_RESULT Fld(32,0,AC_FULLDW)//[31:0]
#define LCOLORNOISE_1 (IO_UHD_PROC2_BASE + 0x5D8)
    #define LCN_BYPASS Fld(1,31,AC_MSKB3)//[31:31]
    #define LCN_INK_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define LCN_SAT_SRC Fld(1,29,AC_MSKB3)//[29:29]
    #define LIRE_STH Fld(7,22,AC_MSKW32)//[28:22]
    #define LIRE_YTH Fld(8,14,AC_MSKW21)//[21:14]
    #define LIRE_SSLOPE Fld(7,7,AC_MSKW10)//[13:7]
    #define LIRE_YSLOPE Fld(7,0,AC_MSKB0)//[6:0]
#define LCOLORNOISE_2 (IO_UHD_PROC2_BASE + 0x5DC)
    #define LSAT_SSLOPE Fld(7,23,AC_MSKW32)//[29:23]
    #define LSAT_STH Fld(7,16,AC_MSKB2)//[22:16]
    #define LSAT_GAIN Fld(7,8,AC_MSKB1)//[14:8]
    #define LIRE_GAIN Fld(7,0,AC_MSKB0)//[6:0]
#define SAT_HIST_CFG_MAIN_3 (IO_UHD_PROC2_BASE + 0x5E0)
    #define COLOR_HIST_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HIST_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_CFG_MAIN_4 (IO_UHD_PROC2_BASE + 0x5E4)
    #define COLOR_HIST_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define COLOR_HIST_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_CFG_MAIN_5 (IO_UHD_PROC2_BASE + 0x5E8)
    #define TWO_DIM_WIN_INV Fld(1,30,AC_MSKB3)//[30:30]
    #define COLOR_HIST_WIN_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define SAT_HIST_HMIN Fld(13,16,AC_MSKW32)//[28:16]
    #define SAT_HIST_HMAX Fld(13,0,AC_MSKW10)//[12:0]
#define SAT_HIST_CFG_MAIN_6 (IO_UHD_PROC2_BASE + 0x5EC)
    #define TWO_DIM_X_END Fld(16,16,AC_FULLW32)//[31:16]
    #define TWO_DIM_X_START Fld(16,0,AC_FULLW10)//[15:0]
#define SAT_HIST_CFG_MAIN_7 (IO_UHD_PROC2_BASE + 0x5F0)
    #define TWO_DIM_Y_END Fld(16,16,AC_FULLW32)//[31:16]
    #define TWO_DIM_Y_START Fld(16,0,AC_FULLW10)//[15:0]
#define COLOR_MAPPING_CONTROL (IO_UHD_PROC2_BASE + 0x628)
    #define CM_INK_CH Fld(2,12,AC_MSKB1)//[13:12]
    #define CM_INK_DELTA_MODE Fld(1,10,AC_MSKB1)//[10:10]
    #define CM_W3_INK_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define CM_W3_WGT_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define CM_W3_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define CM_W2_INK_EN Fld(1,6,AC_MSKB0)//[6:6]
    #define CM_W2_WGT_EN Fld(1,5,AC_MSKB0)//[5:5]
    #define CM_W2_EN Fld(1,4,AC_MSKB0)//[4:4]
    #define CM_W1_INK_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define CM_W1_WGT_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define CM_W1_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define CM_BYPASS Fld(1,0,AC_MSKB0)//[0:0]
#define COLOR_MAPPING_W1_HUE_0 (IO_UHD_PROC2_BASE + 0x62C)
    #define CM_H_W1_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W1_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W1_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_HUE_1 (IO_UHD_PROC2_BASE + 0x630)
    #define CM_H_W1_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W1_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W1_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_HUE_2 (IO_UHD_PROC2_BASE + 0x634)
    #define CM_H_W1_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_H_W1_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_H_W1_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_HUE_3 (IO_UHD_PROC2_BASE + 0x638)
    #define CM_H_W1_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_H_W1_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_H_W1_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_H_W1_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W1_HUE_4 (IO_UHD_PROC2_BASE + 0x63C)
    #define CM_H_W1_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_H_W1_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_LUMA_0 (IO_UHD_PROC2_BASE + 0x640)
    #define CM_Y_W1_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W1_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W1_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_LUMA_1 (IO_UHD_PROC2_BASE + 0x644)
    #define CM_Y_W1_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W1_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W1_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_LUMA_2 (IO_UHD_PROC2_BASE + 0x648)
    #define CM_Y_W1_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_Y_W1_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_Y_W1_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_LUMA_3 (IO_UHD_PROC2_BASE + 0x64C)
    #define CM_Y_W1_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_Y_W1_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_Y_W1_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_Y_W1_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W1_LUMA_4 (IO_UHD_PROC2_BASE + 0x650)
    #define CM_Y_W1_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_Y_W1_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_SAT_0 (IO_UHD_PROC2_BASE + 0x654)
    #define CM_S_W1_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W1_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W1_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_SAT_1 (IO_UHD_PROC2_BASE + 0x658)
    #define CM_S_W1_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W1_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W1_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_SAT_2 (IO_UHD_PROC2_BASE + 0x65C)
    #define CM_S_W1_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_S_W1_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_S_W1_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W1_SAT_3 (IO_UHD_PROC2_BASE + 0x660)
    #define CM_S_W1_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_S_W1_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_S_W1_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_S_W1_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W1_SAT_4 (IO_UHD_PROC2_BASE + 0x664)
    #define CM_S_W1_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_S_W1_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_HUE_0 (IO_UHD_PROC2_BASE + 0x668)
    #define CM_H_W2_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W2_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W2_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_HUE_1 (IO_UHD_PROC2_BASE + 0x66C)
    #define CM_H_W2_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W2_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W2_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_HUE_2 (IO_UHD_PROC2_BASE + 0x670)
    #define CM_H_W2_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_H_W2_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_H_W2_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_HUE_3 (IO_UHD_PROC2_BASE + 0x674)
    #define CM_H_W2_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_H_W2_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_H_W2_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_H_W2_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W2_HUE_4 (IO_UHD_PROC2_BASE + 0x678)
    #define CM_H_W2_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_H_W2_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_LUMA_0 (IO_UHD_PROC2_BASE + 0x67C)
    #define CM_Y_W2_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W2_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W2_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_LUMA_1 (IO_UHD_PROC2_BASE + 0x680)
    #define CM_Y_W2_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W2_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W2_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_LUMA_2 (IO_UHD_PROC2_BASE + 0x684)
    #define CM_Y_W2_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_Y_W2_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_Y_W2_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_LUMA_3 (IO_UHD_PROC2_BASE + 0x688)
    #define CM_Y_W2_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_Y_W2_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_Y_W2_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_Y_W2_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W2_LUMA_4 (IO_UHD_PROC2_BASE + 0x68C)
    #define CM_Y_W2_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_Y_W2_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_SAT_0 (IO_UHD_PROC2_BASE + 0x690)
    #define CM_S_W2_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W2_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W2_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_SAT_1 (IO_UHD_PROC2_BASE + 0x694)
    #define CM_S_W2_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W2_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W2_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_SAT_2 (IO_UHD_PROC2_BASE + 0x698)
    #define CM_S_W2_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_S_W2_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_S_W2_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W2_SAT_3 (IO_UHD_PROC2_BASE + 0x69C)
    #define CM_S_W2_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_S_W2_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_S_W2_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_S_W2_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W2_SAT_4 (IO_UHD_PROC2_BASE + 0x6A0)
    #define CM_S_W2_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_S_W2_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_HUE_0 (IO_UHD_PROC2_BASE + 0x6A4)
    #define CM_H_W3_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W3_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W3_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_HUE_1 (IO_UHD_PROC2_BASE + 0x6A8)
    #define CM_H_W3_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_H_W3_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_H_W3_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_HUE_2 (IO_UHD_PROC2_BASE + 0x6AC)
    #define CM_H_W3_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_H_W3_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_H_W3_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_HUE_3 (IO_UHD_PROC2_BASE + 0x6B0)
    #define CM_H_W3_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_H_W3_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_H_W3_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_H_W3_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W3_HUE_4 (IO_UHD_PROC2_BASE + 0x6B4)
    #define CM_H_W3_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_H_W3_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_LUMA_0 (IO_UHD_PROC2_BASE + 0x6B8)
    #define CM_Y_W3_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W3_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W3_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_LUMA_1 (IO_UHD_PROC2_BASE + 0x6BC)
    #define CM_Y_W3_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_Y_W3_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_Y_W3_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_LUMA_2 (IO_UHD_PROC2_BASE + 0x6C0)
    #define CM_Y_W3_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_Y_W3_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_Y_W3_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_LUMA_3 (IO_UHD_PROC2_BASE + 0x6C4)
    #define CM_Y_W3_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_Y_W3_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_Y_W3_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_Y_W3_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W3_LUMA_4 (IO_UHD_PROC2_BASE + 0x6C8)
    #define CM_Y_W3_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_Y_W3_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_SAT_0 (IO_UHD_PROC2_BASE + 0x6CC)
    #define CM_S_W3_POINT0 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W3_U Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W3_L Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_SAT_1 (IO_UHD_PROC2_BASE + 0x6D0)
    #define CM_S_W3_POINT3 Fld(10,20,AC_MSKW32)//[29:20]
    #define CM_S_W3_POINT2 Fld(10,10,AC_MSKW21)//[19:10]
    #define CM_S_W3_POINT1 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_SAT_2 (IO_UHD_PROC2_BASE + 0x6D4)
    #define CM_S_W3_SLOPE1 Fld(8,20,AC_MSKW32)//[27:20]
    #define CM_S_W3_SLOPE0 Fld(8,10,AC_MSKW21)//[17:10]
    #define CM_S_W3_POINT4 Fld(10,0,AC_MSKW10)//[9:0]
#define COLOR_MAPPING_W3_SAT_3 (IO_UHD_PROC2_BASE + 0x6D8)
    #define CM_S_W3_SLOPE5 Fld(8,24,AC_FULLB3)//[31:24]
    #define CM_S_W3_SLOPE4 Fld(8,16,AC_FULLB2)//[23:16]
    #define CM_S_W3_SLOPE3 Fld(8,8,AC_FULLB1)//[15:8]
    #define CM_S_W3_SLOPE2 Fld(8,0,AC_FULLB0)//[7:0]
#define COLOR_MAPPING_W3_SAT_4 (IO_UHD_PROC2_BASE + 0x6DC)
    #define CM_S_W3_WGT_USLOPE Fld(10,16,AC_MSKW32)//[25:16]
    #define CM_S_W3_WGT_LSLOPE Fld(10,0,AC_MSKW10)//[9:0]

//Page LOCDIM2
#define LD2_MODE_00 (IO_UHD_PROC2_BASE + 0x800)
    #define C_LD_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_LD_BYPASS Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_3D_MODE Fld(2,28,AC_MSKB3)//[29:28]
    #define C_LD_BLK_VERT_NUM Fld(4,22,AC_MSKW32)//[25:22]
    #define C_LD_BLK_HOR_NUM Fld(5,17,AC_MSKB2)//[21:17]
    #define C_LD_SCENE_CHG Fld(1,16,AC_MSKB2)//[16:16]
    #define C_LD_DIS_DS Fld(1,13,AC_MSKB1)//[13:13]
    #define C_LD_DBUF_ON Fld(1,12,AC_MSKB1)//[12:12]
    #define C_LD_3D_PHASE Fld(2,10,AC_MSKB1)//[11:10]
    #define C_LD_MAX_PXL_TH Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_01 (IO_UHD_PROC2_BASE + 0x804)
    #define C_LD_BLK_NUM Fld(8,24,AC_FULLB3)//[31:24]
    #define C_LD_BLK_HEIGHT Fld(11,13,AC_MSKW21)//[23:13]
    #define C_LD_BLK_WIDTH Fld(13,0,AC_MSKW10)//[12:0]
#define LD2_MODE_02 (IO_UHD_PROC2_BASE + 0x808)
    #define C_LD_BLK_CTR_MODE Fld(3,29,AC_MSKB3)//[31:29]
    #define C_LD_BLK_CTR_V1 Fld(11,16,AC_MSKW32)//[26:16]
    #define C_LD_BLK_CTR_H1 Fld(13,0,AC_MSKW10)//[12:0]
#define LD2_MODE_03 (IO_UHD_PROC2_BASE + 0x80C)
    #define C_LD_INK Fld(4,27,AC_MSKB3)//[30:27]
    #define C_LD_BLK_CTR_V2 Fld(11,16,AC_MSKW32)//[26:16]
    #define C_LD_BLK_CTR_H2 Fld(13,0,AC_MSKW10)//[12:0]
#define LD2_MODE_04 (IO_UHD_PROC2_BASE + 0x810)
    #define C_LD_DIM_WET_3 Fld(6,24,AC_MSKB3)//[29:24]
    #define C_LD_DIM_WET_2 Fld(6,16,AC_MSKB2)//[21:16]
    #define C_LD_DIM_WET_1 Fld(6,8,AC_MSKB1)//[13:8]
    #define C_LD_DIM_WET_0 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_05 (IO_UHD_PROC2_BASE + 0x814)
    #define C_LD_DIM_WET_4 Fld(6,22,AC_MSKW32)//[27:22]
    #define C_LD_MAX_DIM Fld(10,12,AC_MSKW21)//[21:12]
    #define C_LD_MIN_DIM Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_06 (IO_UHD_PROC2_BASE + 0x818)
    #define C_LD_BLKCONF_MODE Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_BLKCONF_ZERO_STEP Fld(5,25,AC_MSKB3)//[29:25]
    #define C_LD_DIM_STEP_SHIFT Fld(2,23,AC_MSKW32)//[24:23]
    #define C_LD_DY_DIM_SLOPE Fld(6,17,AC_MSKB2)//[22:17]
    #define C_LD_DY_DIM_OFST Fld(5,12,AC_MSKW21)//[16:12]
#define LD2_MODE_07 (IO_UHD_PROC2_BASE + 0x81C)
    #define C_LD_LP_REF_COEF2 Fld(5,23,AC_MSKW32)//[27:23]
    #define C_LD_LP_REF_COEF Fld(5,18,AC_MSKB2)//[22:18]
    #define C_LD_LP_VERT_RATIO Fld(3,15,AC_MSKW21)//[17:15]
    #define C_LD_LP_HOR_RATIO Fld(3,12,AC_MSKB1)//[14:12]
    #define C_LD_LP_VERT Fld(6,6,AC_MSKW10)//[11:6]
    #define C_LD_LP_HOR Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_08 (IO_UHD_PROC2_BASE + 0x820)
    #define LD_RGN_ST Fld(3,29,AC_MSKB3)//[31:29]
    #define C_LD_SPI_HW_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define C_LD_SPI_ADDR_SEL Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_SPI_RISING_SEL Fld(2,22,AC_MSKB2)//[23:22]
    #define C_LD_SPI_LIMIT_LNUM Fld(7,15,AC_MSKW21)//[21:15]
    #define C_LD_RGB_WEIGHT Fld(4,10,AC_MSKB1)//[13:10]
    #define C_LD_LPF_WEIGHT Fld(5,5,AC_MSKW10)//[9:5]
    #define C_LD_LP_OFST Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_MODE_09 (IO_UHD_PROC2_BASE + 0x824)
    #define C_LD_FORCE_DIM_TYPE Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_FORCE_DIM_VALUE Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FORCE_DIM_EN Fld(1,19,AC_MSKB2)//[19:19]
    #define C_LD_PHASE_VERT_RATIO Fld(3,16,AC_MSKB2)//[18:16]
    #define C_LD_PHASE_HOR_RATIO Fld(3,12,AC_MSKB1)//[14:12]
    #define C_LD_PHASE_VERT Fld(5,5,AC_MSKW10)//[9:5]
    #define C_LD_PHASE_HOR Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_MODE_10 (IO_UHD_PROC2_BASE + 0x828)
    #define C_LD_PHASE_BLK_HOR_7 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_HOR_6 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_HOR_5 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_HOR_4 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_3 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_2 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_1 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_0 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_11 (IO_UHD_PROC2_BASE + 0x82C)
    #define C_LD_PHASE_BLK_HOR_15 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_HOR_14 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_HOR_13 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_HOR_12 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_11 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_10 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_9 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_8 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_12 (IO_UHD_PROC2_BASE + 0x830)
    #define C_LD_PHASE_BLK_HOR_20 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_HOR_19 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_HOR_18 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_HOR_17 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_HOR_16 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_13 (IO_UHD_PROC2_BASE + 0x834)
    #define C_LD_PHASE_BLK_VERT_7 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_VERT_6 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_VERT_5 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_VERT_4 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_3 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_2 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_1 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_0 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_14 (IO_UHD_PROC2_BASE + 0x838)
    #define C_LD_PHASE_BLK_VERT_15 Fld(4,28,AC_MSKB3)//[31:28]
    #define C_LD_PHASE_BLK_VERT_14 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_PHASE_BLK_VERT_13 Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_PHASE_BLK_VERT_12 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_11 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_10 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_9 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_8 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_15 (IO_UHD_PROC2_BASE + 0x83C)
    #define C_LD_PHASE_BLK_VERT_20 Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_PHASE_BLK_VERT_19 Fld(4,12,AC_MSKB1)//[15:12]
    #define C_LD_PHASE_BLK_VERT_18 Fld(4,8,AC_MSKB1)//[11:8]
    #define C_LD_PHASE_BLK_VERT_17 Fld(4,4,AC_MSKB0)//[7:4]
    #define C_LD_PHASE_BLK_VERT_16 Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_GAIN_LIMIT_00 (IO_UHD_PROC2_BASE + 0x840)
    #define C_LD_GAIN_LIMIT_0 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_1 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_01 (IO_UHD_PROC2_BASE + 0x844)
    #define C_LD_GAIN_LIMIT_2 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_3 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_02 (IO_UHD_PROC2_BASE + 0x848)
    #define C_LD_GAIN_LIMIT_4 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_5 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_03 (IO_UHD_PROC2_BASE + 0x84C)
    #define C_LD_GAIN_LIMIT_6 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_7 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_04 (IO_UHD_PROC2_BASE + 0x850)
    #define C_LD_GAIN_LIMIT_8 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_9 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_05 (IO_UHD_PROC2_BASE + 0x854)
    #define C_LD_GAIN_LIMIT_10 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_11 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_06 (IO_UHD_PROC2_BASE + 0x858)
    #define C_LD_GAIN_LIMIT_12 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_13 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_07 (IO_UHD_PROC2_BASE + 0x85C)
    #define C_LD_GAIN_LIMIT_14 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_15 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_08 (IO_UHD_PROC2_BASE + 0x860)
    #define C_LD_GAIN_LIMIT_16 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_17 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_09 (IO_UHD_PROC2_BASE + 0x864)
    #define C_LD_GAIN_LIMIT_18 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_19 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_10 (IO_UHD_PROC2_BASE + 0x868)
    #define C_LD_GAIN_LIMIT_20 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_21 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_11 (IO_UHD_PROC2_BASE + 0x86C)
    #define C_LD_GAIN_LIMIT_22 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_23 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_12 (IO_UHD_PROC2_BASE + 0x870)
    #define C_LD_GAIN_LIMIT_24 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_25 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_13 (IO_UHD_PROC2_BASE + 0x874)
    #define C_LD_GAIN_LIMIT_26 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_27 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_14 (IO_UHD_PROC2_BASE + 0x878)
    #define C_LD_GAIN_LIMIT_28 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_29 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_15 (IO_UHD_PROC2_BASE + 0x87C)
    #define C_LD_GAIN_LIMIT_30 Fld(14,16,AC_MSKW32)//[29:16]
    #define C_LD_GAIN_LIMIT_31 Fld(14,0,AC_MSKW10)//[13:0]
#define LD2_GAIN_LIMIT_16 (IO_UHD_PROC2_BASE + 0x880)
    #define C_LD_GAIN_LIMIT_32 Fld(14,16,AC_MSKW32)//[29:16]
#define LD2_SRAM_IF_00 (IO_UHD_PROC2_BASE + 0x884)
    #define C_LD_FORCE_SRAM_CS_ON Fld(1,17,AC_MSKB2)//[17:17]
    #define C_LD_VA_READ_LATEST_WDATA Fld(1,16,AC_MSKB2)//[16:16]
    #define C_LD_RD_AUTO_INC_ADDR Fld(1,11,AC_MSKB1)//[11:11]
    #define C_LD_WR_AUTO_INC_ADDR Fld(1,10,AC_MSKB1)//[10:10]
    #define C_LD_PIO_MODE Fld(1,9,AC_MSKB1)//[9:9]
    #define C_LD_FORCE_WR_NO_WAIT Fld(1,8,AC_MSKB1)//[8:8]
    #define C_LD_MAX_WAIT_CYCLE Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_01 (IO_UHD_PROC2_BASE + 0x888)
    #define LD_CPU_SRAM_ADDR Fld(8,16,AC_FULLB2)//[23:16]
    #define C_LD_ADDR Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_02 (IO_UHD_PROC2_BASE + 0x88C)
    #define C_LD_WDATA_0 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_03 (IO_UHD_PROC2_BASE + 0x890)
    #define C_LD_WDATA_1 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_04 (IO_UHD_PROC2_BASE + 0x894)
    #define C_LD_WDATA_2 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_05 (IO_UHD_PROC2_BASE + 0x898)
    #define C_LD_WDATA_3 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_06 (IO_UHD_PROC2_BASE + 0x89C)
    #define C_LD_WDATA_4 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_07 (IO_UHD_PROC2_BASE + 0x8A0)
    #define C_LD_WDATA_5 Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_08 (IO_UHD_PROC2_BASE + 0x8A4)
    #define LD_RDATA_0 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_09 (IO_UHD_PROC2_BASE + 0x8A8)
    #define LD_RDATA_1 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_10 (IO_UHD_PROC2_BASE + 0x8AC)
    #define LD_RDATA_2 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_11 (IO_UHD_PROC2_BASE + 0x8B0)
    #define LD_RDATA_3 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_12 (IO_UHD_PROC2_BASE + 0x8B4)
    #define LD_RDATA_4 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_SRAM_IF_13 (IO_UHD_PROC2_BASE + 0x8B8)
    #define LD_RDATA_5 Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_SRAM_IF_14 (IO_UHD_PROC2_BASE + 0x8BC)
    #define C_LD_CPURW_SEL_0 Fld(1,9,AC_MSKB1)//[9:9]
    #define C_LD_CPURW_ACTIVE Fld(1,8,AC_MSKB1)//[8:8]
    #define LD_CPU_SEL_SRAM_1 Fld(1,7,AC_MSKB0)//[7:7]
    #define LD_CPU_SEL_SRAM_0 Fld(1,6,AC_MSKB0)//[6:6]
    #define LD_TABLE_WAIT_VSYNC Fld(1,5,AC_MSKB0)//[5:5]
    #define C_LD_FILL_TABLE_READY Fld(1,4,AC_MSKB0)//[4:4]
    #define LD_READ_FINISH Fld(1,3,AC_MSKB0)//[3:3]
    #define C_LD_READ Fld(1,2,AC_MSKB0)//[2:2]
    #define LD_WRITE_FINISH Fld(1,1,AC_MSKB0)//[1:1]
    #define C_LD_WRITE Fld(1,0,AC_MSKB0)//[0:0]
#define LD2_DBUG (IO_UHD_PROC2_BASE + 0x8C0)
    #define LD_ERR_R_INIT_LP Fld(1,31,AC_MSKB3)//[31:31]
    #define C_LD_CLEAR_ERR Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_DBG_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_STATUS_00 (IO_UHD_PROC2_BASE + 0x8C8)
    #define LD_3D_PERIOD Fld(2,30,AC_MSKB3)//[31:30]
    #define LD_MAX_INFO Fld(10,8,AC_MSKW21)//[17:8]
    #define C_LD_READ_BLK_NUM Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_STATUS_01 (IO_UHD_PROC2_BASE + 0x8CC)
    #define LD_OUT_DIM Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_TAR_DIM Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_CONFIDENCE Fld(5,0,AC_MSKB0)//[4:0]
#define LD2_INK_DATA (IO_UHD_PROC2_BASE + 0x8E4)
    #define C_LD_INK3_SHIFT Fld(2,30,AC_MSKB3)//[31:30]
    #define C_LD_INK_B Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_INK_G Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_INK_R Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DEMO_00 (IO_UHD_PROC2_BASE + 0x8E8)
    #define C_LD_DEMO_YSTART Fld(16,16,AC_FULLW32)//[31:16]
    #define C_LD_DEMO_XSTART Fld(16,0,AC_FULLW10)//[15:0]
#define LD2_DEMO_01 (IO_UHD_PROC2_BASE + 0x8EC)
    #define C_LD_DEMO_YEND Fld(16,16,AC_FULLW32)//[31:16]
    #define C_LD_DEMO_XEND Fld(16,0,AC_FULLW10)//[15:0]
#define LD2_DEMO_02 (IO_UHD_PROC2_BASE + 0x8F0)
    #define C_LD_DEMO_REGION_SWAP Fld(1,2,AC_MSKB0)//[2:2]
    #define C_LD_DEMO_NOHIT_MODE Fld(2,0,AC_MSKB0)//[1:0]
#define LD2_DIM_RESULT_00 (IO_UHD_PROC2_BASE + 0x8F4)
    #define LD_DIM_READY Fld(1,30,AC_MSKB3)//[30:30]
    #define LD_DIM_002 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_001 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_000 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_01 (IO_UHD_PROC2_BASE + 0x8F8)
    #define LD_DIM_005 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_004 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_003 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_02 (IO_UHD_PROC2_BASE + 0x8FC)
    #define LD_DIM_008 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_007 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_006 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_03 (IO_UHD_PROC2_BASE + 0x900)
    #define LD_DIM_011 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_010 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_009 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_04 (IO_UHD_PROC2_BASE + 0x904)
    #define LD_DIM_014 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_013 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_012 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_05 (IO_UHD_PROC2_BASE + 0x908)
    #define LD_DIM_017 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_016 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_015 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_06 (IO_UHD_PROC2_BASE + 0x90C)
    #define LD_DIM_020 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_019 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_018 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_07 (IO_UHD_PROC2_BASE + 0x910)
    #define LD_DIM_023 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_022 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_021 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_08 (IO_UHD_PROC2_BASE + 0x914)
    #define LD_DIM_026 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_025 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_024 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_09 (IO_UHD_PROC2_BASE + 0x918)
    #define LD_DIM_029 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_028 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_027 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_10 (IO_UHD_PROC2_BASE + 0x91C)
    #define LD_DIM_032 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_031 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_030 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_11 (IO_UHD_PROC2_BASE + 0x920)
    #define LD_DIM_035 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_034 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_033 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_12 (IO_UHD_PROC2_BASE + 0x924)
    #define LD_DIM_038 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_037 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_036 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_13 (IO_UHD_PROC2_BASE + 0x928)
    #define LD_DIM_041 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_040 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_039 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_14 (IO_UHD_PROC2_BASE + 0x92C)
    #define LD_DIM_044 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_043 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_042 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_15 (IO_UHD_PROC2_BASE + 0x930)
    #define LD_DIM_047 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_046 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_045 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_16 (IO_UHD_PROC2_BASE + 0x934)
    #define LD_DIM_050 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_049 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_048 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_17 (IO_UHD_PROC2_BASE + 0x938)
    #define LD_DIM_053 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_052 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_051 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_18 (IO_UHD_PROC2_BASE + 0x93C)
    #define LD_DIM_056 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_055 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_054 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_19 (IO_UHD_PROC2_BASE + 0x940)
    #define LD_DIM_059 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_058 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_057 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_20 (IO_UHD_PROC2_BASE + 0x944)
    #define LD_DIM_062 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_061 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_060 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_21 (IO_UHD_PROC2_BASE + 0x948)
    #define LD_DIM_065 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_064 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_063 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_22 (IO_UHD_PROC2_BASE + 0x94C)
    #define LD_DIM_068 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_067 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_066 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_23 (IO_UHD_PROC2_BASE + 0x950)
    #define LD_DIM_071 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_070 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_069 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_24 (IO_UHD_PROC2_BASE + 0x954)
    #define LD_DIM_074 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_073 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_072 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_25 (IO_UHD_PROC2_BASE + 0x958)
    #define LD_DIM_077 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_076 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_075 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_26 (IO_UHD_PROC2_BASE + 0x95C)
    #define LD_DIM_080 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_079 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_078 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_27 (IO_UHD_PROC2_BASE + 0x960)
    #define LD_DIM_083 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_082 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_081 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_28 (IO_UHD_PROC2_BASE + 0x964)
    #define LD_DIM_086 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_085 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_084 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_29 (IO_UHD_PROC2_BASE + 0x968)
    #define LD_DIM_089 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_088 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_087 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_30 (IO_UHD_PROC2_BASE + 0x96C)
    #define LD_DIM_092 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_091 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_090 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_31 (IO_UHD_PROC2_BASE + 0x970)
    #define LD_DIM_095 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_094 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_093 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_32 (IO_UHD_PROC2_BASE + 0x974)
    #define LD_DIM_098 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_097 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_096 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_33 (IO_UHD_PROC2_BASE + 0x978)
    #define LD_DIM_101 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_100 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_099 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_34 (IO_UHD_PROC2_BASE + 0x97C)
    #define LD_DIM_104 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_103 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_102 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_35 (IO_UHD_PROC2_BASE + 0x980)
    #define LD_DIM_107 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_106 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_105 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_36 (IO_UHD_PROC2_BASE + 0x984)
    #define LD_DIM_110 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_109 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_108 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_37 (IO_UHD_PROC2_BASE + 0x988)
    #define LD_DIM_113 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_112 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_111 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_38 (IO_UHD_PROC2_BASE + 0x98C)
    #define LD_DIM_116 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_115 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_114 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_39 (IO_UHD_PROC2_BASE + 0x990)
    #define LD_DIM_119 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_118 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_117 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_40 (IO_UHD_PROC2_BASE + 0x994)
    #define LD_DIM_122 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_121 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_120 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_41 (IO_UHD_PROC2_BASE + 0x998)
    #define LD_DIM_125 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_124 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_123 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_42 (IO_UHD_PROC2_BASE + 0x99C)
    #define LD_DIM_128 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_127 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_126 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_43 (IO_UHD_PROC2_BASE + 0x9A0)
    #define LD_DIM_131 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_130 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_129 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_44 (IO_UHD_PROC2_BASE + 0x9A4)
    #define LD_DIM_134 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_133 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_132 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_45 (IO_UHD_PROC2_BASE + 0x9A8)
    #define LD_DIM_137 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_136 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_135 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_46 (IO_UHD_PROC2_BASE + 0x9AC)
    #define LD_DIM_140 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_139 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_138 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_47 (IO_UHD_PROC2_BASE + 0x9B0)
    #define LD_DIM_143 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_142 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_141 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_48 (IO_UHD_PROC2_BASE + 0x9B4)
    #define LD_DIM_146 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_145 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_144 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_49 (IO_UHD_PROC2_BASE + 0x9B8)
    #define LD_DIM_149 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_148 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_147 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_50 (IO_UHD_PROC2_BASE + 0x9BC)
    #define LD_DIM_152 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_151 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_150 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_51 (IO_UHD_PROC2_BASE + 0x9C0)
    #define LD_DIM_155 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_154 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_153 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_52 (IO_UHD_PROC2_BASE + 0x9C4)
    #define LD_DIM_158 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_157 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_156 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_53 (IO_UHD_PROC2_BASE + 0x9C8)
    #define LD_DIM_161 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_160 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_159 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_54 (IO_UHD_PROC2_BASE + 0x9CC)
    #define LD_DIM_164 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_163 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_162 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_55 (IO_UHD_PROC2_BASE + 0x9D0)
    #define LD_DIM_167 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_166 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_165 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_56 (IO_UHD_PROC2_BASE + 0x9D4)
    #define LD_DIM_170 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_169 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_168 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_57 (IO_UHD_PROC2_BASE + 0x9D8)
    #define LD_DIM_173 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_172 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_171 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_58 (IO_UHD_PROC2_BASE + 0x9DC)
    #define LD_DIM_176 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_175 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_174 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_59 (IO_UHD_PROC2_BASE + 0x9E0)
    #define LD_DIM_179 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_178 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_177 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_60 (IO_UHD_PROC2_BASE + 0x9E4)
    #define LD_DIM_182 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_181 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_180 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_61 (IO_UHD_PROC2_BASE + 0x9E8)
    #define LD_DIM_185 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_184 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_183 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_62 (IO_UHD_PROC2_BASE + 0x9EC)
    #define LD_DIM_188 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_187 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_186 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_63 (IO_UHD_PROC2_BASE + 0x9F0)
    #define LD_DIM_191 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_190 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_189 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_64 (IO_UHD_PROC2_BASE + 0x9F4)
    #define LD_DIM_194 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_193 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_192 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_65 (IO_UHD_PROC2_BASE + 0x9F8)
    #define LD_DIM_197 Fld(10,20,AC_MSKW32)//[29:20]
    #define LD_DIM_196 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_195 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_DIM_RESULT_66 (IO_UHD_PROC2_BASE + 0x9FC)
    #define LD_DIM_199 Fld(10,10,AC_MSKW21)//[19:10]
    #define LD_DIM_198 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_16 (IO_UHD_PROC2_BASE + 0xA00)
    #define C_LD_MAXINFO_LPF_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_LD_BLKINFO_WEIGHT Fld(6,24,AC_MSKB3)//[29:24]
    #define C_LD_BLKAVG_LSUM_SHIFT Fld(4,20,AC_MSKB2)//[23:20]
    #define C_LD_BLKAVE_SHIFT_C Fld(4,16,AC_MSKB2)//[19:16]
    #define C_LD_BLKAVE_MULT_B Fld(10,5,AC_MSKW10)//[14:5]
    #define C_LD_BLKAVE_SHIFT_A Fld(4,0,AC_MSKB0)//[3:0]
#define LD2_MODE_17 (IO_UHD_PROC2_BASE + 0xA04)
    #define C_LD_MAX_UP_DIM_STEP Fld(8,24,AC_FULLB3)//[31:24]
    #define C_LD_MAX_DN_DIM_STEP Fld(8,16,AC_FULLB2)//[23:16]
    #define C_LD_BLK_SCENE_CHG_0_15 Fld(16,0,AC_FULLW10)//[15:0]
#define LD2_MODE_18 (IO_UHD_PROC2_BASE + 0xA08)
    #define C_LD_BLK_SCENE_CHG_16_47 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_19 (IO_UHD_PROC2_BASE + 0xA0C)
    #define C_LD_BLK_SCENE_CHG_48_79 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_20 (IO_UHD_PROC2_BASE + 0xA10)
    #define C_LD_BLK_SCENE_CHG_80_111 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_21 (IO_UHD_PROC2_BASE + 0xA14)
    #define C_LD_BLK_SCENE_CHG_112_143 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_22 (IO_UHD_PROC2_BASE + 0xA18)
    #define C_LD_BLK_SCENE_CHG_144_175 Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_23 (IO_UHD_PROC2_BASE + 0xA1C)
    #define C_LD_BLK_SCENE_CHG_MODE Fld(2,30,AC_MSKB3)//[31:30]
    #define C_LD_BLK_SCENE_CHG_DN_TH Fld(3,27,AC_MSKB3)//[29:27]
    #define C_LD_BLK_SCENE_CHG_UP_TH Fld(3,24,AC_MSKB3)//[26:24]
    #define C_LD_BLK_SCENE_CHG_176_199 Fld(24,0,AC_MSKDW)//[23:0]
#define LD2_MODE_24 (IO_UHD_PROC2_BASE + 0xA20)
    #define C_LD_RGB_WEIGHT2 Fld(4,24,AC_MSKB3)//[27:24]
    #define C_LD_BLK_SCENE_CHG_HW_THD2 Fld(8,16,AC_FULLB2)//[23:16]
    #define C_LD_BLK_SCENE_CHG_HW_THD1 Fld(8,8,AC_FULLB1)//[15:8]
    #define C_LD_BLK_SCENE_CHG_HW_THD0 Fld(8,0,AC_FULLB0)//[7:0]
#define LD2_MODE_25 (IO_UHD_PROC2_BASE + 0xA24)
    #define LD_BLKINFO_004 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_003 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_002 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_001 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_000 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_26 (IO_UHD_PROC2_BASE + 0xA28)
    #define LD_BLKINFO_009 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_008 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_007 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_006 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_005 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_27 (IO_UHD_PROC2_BASE + 0xA2C)
    #define LD_BLKINFO_014 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_013 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_012 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_011 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_010 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_28 (IO_UHD_PROC2_BASE + 0xA30)
    #define LD_BLKINFO_019 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_018 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_017 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_016 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_015 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_29 (IO_UHD_PROC2_BASE + 0xA34)
    #define LD_BLKINFO_024 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_023 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_022 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_021 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_020 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_30 (IO_UHD_PROC2_BASE + 0xA38)
    #define LD_BLKINFO_029 Fld(6,24,AC_MSKB3)//[29:24]
    #define LD_BLKINFO_028 Fld(6,18,AC_MSKB2)//[23:18]
    #define LD_BLKINFO_027 Fld(6,12,AC_MSKW21)//[17:12]
    #define LD_BLKINFO_026 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_025 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_31 (IO_UHD_PROC2_BASE + 0xA3C)
    #define C_LD_BLKINFO_SEL Fld(2,30,AC_MSKB3)//[31:30]
    #define C_LD_BLKINFO_START_NUM Fld(8,16,AC_FULLB2)//[23:16]
    #define LD_BLKINFO_031 Fld(6,6,AC_MSKW10)//[11:6]
    #define LD_BLKINFO_030 Fld(6,0,AC_MSKB0)//[5:0]
#define LD2_MODE_32 (IO_UHD_PROC2_BASE + 0xA40)
    #define LD_BLKINFO_READY Fld(32,0,AC_FULLDW)//[31:0]
#define LD2_MODE_33 (IO_UHD_PROC2_BASE + 0xA44)
    #define C_LD_FIX_DIM_STEP_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define C_LD_FW_DIM_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define C_LD_FW_DIM_000 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_001 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_002 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_34 (IO_UHD_PROC2_BASE + 0xA48)
    #define C_LD_FW_DIM_003 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_004 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_005 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_35 (IO_UHD_PROC2_BASE + 0xA4C)
    #define C_LD_FW_DIM_006 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_007 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_008 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_36 (IO_UHD_PROC2_BASE + 0xA50)
    #define C_LD_FW_DIM_009 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_010 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_011 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_37 (IO_UHD_PROC2_BASE + 0xA54)
    #define C_LD_FW_DIM_012 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_013 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_014 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_38 (IO_UHD_PROC2_BASE + 0xA58)
    #define C_LD_FW_DIM_015 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_016 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_017 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_39 (IO_UHD_PROC2_BASE + 0xA5C)
    #define C_LD_FW_DIM_018 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_019 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_020 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_40 (IO_UHD_PROC2_BASE + 0xA60)
    #define C_LD_FW_DIM_021 Fld(10,20,AC_MSKW32)//[29:20]
    #define C_LD_FW_DIM_022 Fld(10,10,AC_MSKW21)//[19:10]
    #define C_LD_FW_DIM_023 Fld(10,0,AC_MSKW10)//[9:0]
#define LD2_MODE_41 (IO_UHD_PROC2_BASE + 0xA64)
    #define C_LD_FW_CURDIM_SUM Fld(18,0,AC_MSKDW)//[17:0]
#define LD2_MODE_42 (IO_UHD_PROC2_BASE + 0xA68)
    #define C_LD_CURDIM_SUM_STA Fld(18,0,AC_MSKDW)//[17:0]

#endif //#if defined(CC_SUPPORT_UHD)

