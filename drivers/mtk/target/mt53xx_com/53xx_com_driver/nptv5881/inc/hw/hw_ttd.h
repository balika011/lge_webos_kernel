/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: hw_ttd.h $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

#ifndef _HW_TTD_H_
#define _HW_TTD_H_

#define IO_TTD_BASE (IO_VIRT +0x5D000)

#define TTD_00 (IO_TTD_BASE + 0xC00)
    #define TTD_FSAMPLE_ENABLE Fld(1,8,AC_MSKB1)//[8:8]
#define TTD_01 (IO_TTD_BASE + 0xC04)
    #define TTD_OUT_LR_DLY Fld(2,15,AC_MSKW21)//[16:15]
    #define TTD_DIBR_LR_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define TTD_OUT_LR_INV Fld(1,6,AC_MSKB0)//[6:6]
    #define TTD_LLRR_MODE Fld(1,5,AC_MSKB0)//[5:5]
#define TTD_02 (IO_TTD_BASE + 0xC08)
    #define TTD_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define TTD_SRC_2D_NRPT Fld(1,25,AC_MSKB3)//[25:25]
    #define TTD_SRC_3D_MODE Fld(1,24,AC_MSKB3)//[24:24]
    #define TTD_IMG_HEIGHT Fld(12,12,AC_MSKW21)//[23:12]
    #define TTD_IMG_WIDTH Fld(12,0,AC_MSKW10)//[11:0]
#define TTD_03 (IO_TTD_BASE + 0xC0C)
    #define TTD_DBG_FORCE_LR Fld(1,29,AC_MSKB3)//[29:29]
    #define TTD_DBG_FORCE_EN Fld(1,28,AC_MSKB3)//[28:28]
#define TTDPO_00 (IO_TTD_BASE + 0xC18)
    #define SYNC_DELAY_CYCLES Fld(9,23,AC_MSKW32)//[31:23]
    #define POST_TTD_BYPASS Fld(1,22,AC_MSKB2)//[22:22]
    #define POST_DEBUG_SEL Fld(4,9,AC_MSKB1)//[12:9]
    #define POST_HS_NUM Fld(8,1,AC_MSKW10)//[8:1]
    #define POST_TTD_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDPO_01 (IO_TTD_BASE + 0xC1C)
    #define POST_VSYNC_MJC Fld(4,18,AC_MSKB2)//[21:18]
    #define POST_HS_BNK_WIDTH Fld(8,10,AC_MSKW21)//[17:10]
    #define POST_HS_TOTAL_WIDTH Fld(10,0,AC_MSKW10)//[9:0]
#define TTDPO_02 (IO_TTD_BASE + 0xC20)
    #define STA_IN_DISPR Fld(1,2,AC_MSKB0)//[2:2]
    #define POST_OUT_LR_INV Fld(1,1,AC_MSKB0)//[1:1]
    #define POST_IN_LR_INV Fld(1,0,AC_MSKB0)//[0:0]
#define TTD_RESERVED (IO_TTD_BASE + 0xC2C)

#define TTDDG_00 (IO_TTD_BASE + 0xD00)
    #define DG_HIST_WINDOW_SIZE Fld(4,9,AC_MSKB1)//[12:9]
#define TTDDG_01 (IO_TTD_BASE + 0xD04)
    #define DG_GLOBAL_BASE Fld(8,16,AC_FULLB2)//[23:16]
    #define DG_LOCAL_WEIGHT Fld(4,8,AC_MSKB1)//[11:8]
    #define DG_GLOBAL_WEIGHT Fld(6,0,AC_MSKB0)//[5:0]
#define TTDDG_02 (IO_TTD_BASE + 0xD08)
    #define DG_TFU_INV_STAT Fld(6,0,AC_MSKB0)//[5:0]
#define TTDDG_03 (IO_TTD_BASE + 0xD0C)
    #define DG_DSPRT_MIN Fld(8,24,AC_FULLB3)//[31:24]
    #define DG_DSPRT_MAX Fld(8,16,AC_FULLB2)//[23:16]
    #define DG_DSPRT_BIAS Fld(8,8,AC_FULLB1)//[15:8]
#define TTDDG_04 (IO_TTD_BASE + 0xD10)
    #define DG_TFU_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define DG_TFU_BASE Fld(8,16,AC_FULLB2)//[23:16]
    #define DG_TEXTURE_WEIGHT Fld(4,12,AC_MSKB1)//[15:12]
    #define DG_TEXTURE_V Fld(4,8,AC_MSKB1)//[11:8]
    #define DG_TEXTURE_U Fld(4,4,AC_MSKB0)//[7:4]
    #define DG_TEXTURE_Y Fld(4,0,AC_MSKB0)//[3:0]
#define TTDDG_05 (IO_TTD_BASE + 0xD14)
    #define TFU_NBR_CLIP_Y Fld(6,22,AC_MSKW32)//[27:22]
    #define MAX_TUNE_THR Fld(6,16,AC_MSKB2)//[21:16]
    #define TFU_THR_HIGH Fld(11,0,AC_MSKW10)//[10:0]
#define TTDDG_RESERVED (IO_TTD_BASE + 0xD20)

#define TTDIR_00 (IO_TTD_BASE + 0xD80)
    #define IR_BLACK_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define IR_PR_INFO Fld(1,17,AC_MSKB2)//[17:17]
    #define IR_PR_MODE Fld(1,16,AC_MSKB2)//[16:16]
#define TTDIR_01 (IO_TTD_BASE + 0xD84)
    #define IR_BLACK_R Fld(11,11,AC_MSKW21)//[21:11]
    #define IR_BLACK_L Fld(11,0,AC_MSKW10)//[10:0]
#define TTDIR_02 (IO_TTD_BASE + 0xD88)
    #define IR_FIFO_LENGTH Fld(8,8,AC_FULLB1)//[15:8]
    #define IR_ERR_CLEAN Fld(1,4,AC_MSKB0)//[4:4]
    #define IR_LEFT_VIEW_OFF Fld(1,1,AC_MSKB0)//[1:1]
    #define IR_RIGHT_VIEW_OFF Fld(1,0,AC_MSKB0)//[0:0]
#define TTDIR_03 (IO_TTD_BASE + 0xD8C)
    #define IR_BLACK_ASYM Fld(1,31,AC_MSKB3)//[31:31]
    #define IR_BLACK_SINGLE_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define IR_BLACK_SINGLE_INV Fld(1,29,AC_MSKB3)//[29:29]
    #define IR_BLACK_SINGLE_WIDTH_R Fld(8,8,AC_FULLB1)//[15:8]
    #define IR_BLACK_SINGLE_WIDTH_L Fld(8,0,AC_FULLB0)//[7:0]
#define TTDIR_RESERVED (IO_TTD_BASE + 0xD90)
#define TTDIR_04 (IO_TTD_BASE + 0xD94)
    #define IR_GLOBAL_SHIFT Fld(8,16,AC_FULLB2)//[23:16]
    #define IR_DISPT_MIN Fld(8,8,AC_FULLB1)//[15:8]
    #define IR_DISPT_MAX Fld(8,0,AC_FULLB0)//[7:0]
#define TTDIR_05 (IO_TTD_BASE + 0xD98)
    #define IR_DISPT_POSTOFFSET Fld(10,18,AC_MSKW32)//[27:18]
    #define IR_DISPT_PREOFFSET Fld(8,10,AC_MSKW21)//[17:10]
    #define IR_DISPT_SCALAR Fld(10,0,AC_MSKW10)//[9:0]
#define TTDIR_06 (IO_TTD_BASE + 0xD9C)
    #define INK_TFU_SCALAR Fld(8,24,AC_FULLB3)//[31:24]
    #define INK_VP_SCALAR Fld(8,16,AC_FULLB2)//[23:16]
#define STA_TTDIR_00 (IO_TTD_BASE + 0xDB0)
    #define STA_IR_ERR_OVER Fld(1,1,AC_MSKB0)//[1:1]
    #define STA_IR_ERR_UNDER Fld(1,0,AC_MSKB0)//[0:0]

#define TTDVP_17 (IO_TTD_BASE + 0xC84)
    #define GM_HORI_CTRL_PT_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_PT_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define GM_HORI_CTRL_PT_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define GM_HORI_CTRL_PT_0 Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_18 (IO_TTD_BASE + 0xC88)
    #define GM_HORI_CTRL_PT_7 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_PT_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define GM_HORI_CTRL_PT_5 Fld(8,8,AC_FULLB1)//[15:8]
    #define GM_HORI_CTRL_PT_4 Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_19 (IO_TTD_BASE + 0xC8C)
    #define GM_HORI_CTRL_PT_8 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_PT_BTM Fld(8,16,AC_FULLB2)//[23:16]
    #define GM_HORI_CTRL_PT_TOP Fld(8,8,AC_FULLB1)//[15:8]


#endif // #ifndef _HW_TTD_H_

