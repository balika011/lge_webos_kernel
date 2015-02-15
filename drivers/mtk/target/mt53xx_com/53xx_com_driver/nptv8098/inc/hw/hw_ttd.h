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

#if defined(CC_MT5398)
#define IO_TTD_BASE (IO_VIRT +0x36000)
#else
#define IO_TTD_BASE (IO_VIRT +0x5D000)
#endif

#define TTD_00 (IO_TTD_BASE + 0xC00)
    #define TTD_FSAMPLE_ENABLE Fld(1,8,AC_MSKB1)//[8:8]
    #define TTD_CONVERGENCE_DEPTH Fld(8,0,AC_FULLB0)//[7:0]
#define TTD_01 (IO_TTD_BASE + 0xC04)
    #define TTD_OUT_LR_DLY Fld(2,15,AC_MSKW21)//[16:15]
    #define TTD_DIBR_LR_INV Fld(1,8,AC_MSKB1)//[8:8]
    #define TTD_OUT_LR_INV Fld(1,6,AC_MSKB0)//[6:6]
    #define TTD_LLRR_MODE Fld(1,5,AC_MSKB0)//[5:5]
    #define TTD_MIRROW Fld(1,0,AC_MSKB0)//[0:0]
#define TTD_02 (IO_TTD_BASE + 0xC08)
    #define TTD_EN Fld(1,31,AC_MSKB3)//[31:31]
    #define MJC_PRE_EN Fld(1,30,AC_MSKB3)//[30:30]
    #define LR_FREERUN_EN Fld(1,29,AC_MSKB3)//[29:29]
    #define SRC_3D_TYPE Fld(2,26,AC_MSKB3)//[27:26]
    #define TTD_SRC_2D_NRPT Fld(1,25,AC_MSKB3)//[25:25]
    #define TTD_SRC_3D_MODE Fld(1,24,AC_MSKB3)//[24:24]
    #define TTD_IMG_HEIGHT Fld(12,12,AC_MSKW21)//[23:12]
    #define TTD_IMG_WIDTH Fld(12,0,AC_MSKW10)//[11:0]
#define TTD_03 (IO_TTD_BASE + 0xC0C)
    #define TTD_DBG_FORCE_LR Fld(1,29,AC_MSKB3)//[29:29]
    #define TTD_DBG_FORCE_EN Fld(1,28,AC_MSKB3)//[28:28]
    #define INK_VERTICAL_GAP Fld(12,16,AC_MSKW32)//[27:16]
    #define INK_DEPTH_Y Fld(2,6,AC_MSKB0)//[7:6]
    #define INK_VP_POS Fld(1,5,AC_MSKB0)//[5:5]
    #define INK_VERTICAL_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define INK_GXGY_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTD_04 (IO_TTD_BASE + 0xC10)
    #define TTD_LM_UV_BASE Fld(8,12,AC_MSKW21)//[19:12]
    #define TTD_LM_V_WEIGHT Fld(4,8,AC_MSKB1)//[11:8]
    #define TTD_LM_U_WEIGHT Fld(4,4,AC_MSKB0)//[7:4]
    #define TTD_3DC_DECREASE_EN Fld(1,3,AC_MSKB0)//[3:3]
    #define TTD_DS4_V_MODE Fld(1,2,AC_MSKB0)//[2:2]
    #define TTD_DS4_H_MODE Fld(1,1,AC_MSKB0)//[1:1]
    #define TTD_3DC_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTD_05 (IO_TTD_BASE + 0xC14)
    #define STA_DMAP_ERR_OVER Fld(1,5,AC_MSKB0)//[5:5]
    #define DMAP_ERR_CLEAN Fld(1,4,AC_MSKB0)//[4:4]
    #define TTD_DBG_SEL Fld(4,0,AC_MSKB0)//[3:0]
#define TTDPO_00 (IO_TTD_BASE + 0xC18)
    #define DELAY_CYCLES Fld(9,23,AC_MSKW32)//[31:23]
    #define POST_TTD_BYPASS Fld(1,22,AC_MSKB2)//[22:22]
    #define DLY_CLR Fld(1,21,AC_MSKB2)//[21:21]
    #define TTD_422_444_CHRM_DEL_EN Fld(1,20,AC_MSKB2)//[20:20]
    #define TTD_422_444_UVINV Fld(1,19,AC_MSKB2)//[19:19]
    #define TTD_444_422_IN_MIRROR Fld(1,18,AC_MSKB2)//[18:18]
    #define TTD_444_422_LINE_EXT Fld(1,17,AC_MSKB2)//[17:17]
    #define TTD_444_422_LPF_EN Fld(1,16,AC_MSKB2)//[16:16]
    #define TTD_444_422_UVINV Fld(1,15,AC_MSKB1)//[15:15]
    #define TTD_444_422_444_EN Fld(1,14,AC_MSKB1)//[14:14]
    #define POST_DEBUG_SEL Fld(4,9,AC_MSKB1)//[12:9]
    #define POST_HS_NUM Fld(8,1,AC_MSKW10)//[8:1]
    #define POST_TTD_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDPO_01 (IO_TTD_BASE + 0xC1C)
    #define POST_VSYNC_MJC Fld(8,24,AC_FULLB3)//[31:24]
    #define POST_HS_BNK_WIDTH Fld(12,12,AC_MSKW21)//[23:12]
    #define POST_HS_TOTAL_WIDTH Fld(12,0,AC_MSKW10)//[11:0]
#define TTDPO_02 (IO_TTD_BASE + 0xC20)
    #define POST_TTD_WIDTH_8X_EN Fld(1,3,AC_MSKB0)//[2:2]
    #define STA_IN_DISPR Fld(1,2,AC_MSKB0)//[2:2]
    #define POST_OUT_LR_INV Fld(1,1,AC_MSKB0)//[1:1]
    #define POST_IN_LR_INV Fld(1,0,AC_MSKB0)//[0:0]
#define TTD_06 (IO_TTD_BASE + 0xC24)
    #define STA_TTD_VERT_EDGE_CNT_TOP Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_TTD_VERT_EDGE_CNT_MID Fld(16,0,AC_FULLW10)//[15:0]
#define TTD_07 (IO_TTD_BASE + 0xC28)
    #define STA_TTD_VERT_EDGE_CNT_BTM Fld(16,16,AC_FULLW32)//[31:16]
#define TTD_08 (IO_TTD_BASE + 0xC2C)
    #define IR_BLACK_L_MSB Fld(3,8,AC_MSKB1)//[10:8]
    #define IR_BLACK_R_MSB Fld(3,11,AC_MSKB1)//[13:11]
#define TTDDM_00 (IO_TTD_BASE + 0xDC0)
    #define CID_FRM_SIZE Fld(16,16,AC_FULLW32)//[31:16]
    #define DSO_FRM_SIZE Fld(16,0,AC_FULLW10)//[15:0]
#define TTDDM_01 (IO_TTD_BASE + 0xDC4)
    #define TTD_DSO_STR_ADR_L Fld(24,8,AC_MSKDW)//[31:8]
    #define CID_FRM_WT_NUM Fld(4,4,AC_MSKB0)//[7:4]
    #define DSO_FRM_WT_NUM Fld(4,0,AC_MSKB0)//[3:0]
#define TTDDM_02 (IO_TTD_BASE + 0xDC8)
    #define DRAM_DS_ERR_OVERFLOW_CLR Fld(1,26,AC_MSKB3)//[26:26]
    #define STA_DRAM_DS_ERR_OVERFLOW Fld(1,25,AC_MSKB3)//[25:25]
    #define TTD_DRAM_WT_DSO_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define TTD_DSO_STR_ADR_R Fld(24,0,AC_MSKDW)//[23:0]
#define TTDDM_03 (IO_TTD_BASE + 0xDCC)
    #define DRAM_CID_ERR_OVERFLOW_CLR Fld(1,26,AC_MSKB3)//[26:26]
    #define STA_DRAM_CID_ERR_OVERFLOW Fld(1,25,AC_MSKB3)//[25:25]
    #define TTD_DRAM_WT_CID_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define TTD_CID_STR_ADR_L Fld(24,0,AC_MSKDW)//[23:0]
#define TTDDM_04 (IO_TTD_BASE + 0xDD0)
    #define TTD_CID_STR_ADR_R Fld(24,0,AC_MSKDW)//[23:0]
#define TTDDM_05 (IO_TTD_BASE + 0xDD4)
    #define TTD_DRAM_ADR_LOW Fld(24,0,AC_MSKDW)//[23:0]
#define TTDDM_06 (IO_TTD_BASE + 0xDD8)
    #define TTD_DRAM_ADR_HIGH Fld(24,0,AC_MSKDW)//[23:0]
#define TTDDM_07 (IO_TTD_BASE + 0xDDC)
    #define FW_DMA_READ_TRIG Fld(1,26,AC_MSKB3)//[26:26]
    #define HW_AUTO_DMA_READ_EN Fld(1,25,AC_MSKB3)//[25:25]
    #define TTD_DRAM_RD_DMAP_EN Fld(1,24,AC_MSKB3)//[24:24]
    #define TTD_MV_STR_ADR Fld(24,0,AC_MSKDW)//[23:0]

#define TTDVP_00 (IO_TTD_BASE + 0xC40)
    #define VP_CID_INV_THR Fld(24,6,AC_MSKDW)//[29:6]
#define TTDVP_01 (IO_TTD_BASE + 0xC44)
    #define CID_UV_BASE Fld(8,8,AC_FULLB1)//[15:8]
    #define CID_Y_BASE Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_02 (IO_TTD_BASE + 0xC48)
    #define VP_EDGE_THR Fld(11,20,AC_MSKW32)//[30:20]
#define TTDVP_03 (IO_TTD_BASE + 0xC4C)
    #define LOCAL_INV_SUCC_COUNT_THR Fld(4,21,AC_MSKW32)//[24:21]
    #define VP_ROI_X_STA Fld(11,10,AC_MSKW21)//[20:10]
    #define HIS_AVG_BINSIZE Fld(10,0,AC_MSKW10)//[9:0]
#define TTDVP_04 (IO_TTD_BASE + 0xC50)
    #define VP_ROI_Y_STA Fld(11,20,AC_MSKW32)//[30:20]
    #define VP_LAST_TILE_PXL_WEIGHT Fld(4,4,AC_MSKB0)//[7:4]
#define TTDVP_05 (IO_TTD_BASE + 0xC54)
    #define GM_HIST_WINDOW_SIZE Fld(4,25,AC_MSKB3)//[28:25]
    #define GM_HORI_SMOOTH_WEIGHT Fld(11,9,AC_MSKW21)//[19:9]
    #define VP_VFLIP_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define VP_HFLIP_EN Fld(1,7,AC_MSKB0)//[7:7]
    #define VP_TRANSITION_STAT Fld(6,1,AC_MSKB0)//[6:1]
    #define VP_TRANSITION_CTRL_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDVP_06 (IO_TTD_BASE + 0xC58)
    #define GM_VH_SMOOTH_WEIGHT Fld(11,21,AC_MSKW32)//[31:21]
    #define GM_VERT_MODEL_GAIN Fld(9,12,AC_MSKW21)//[20:12]
    #define GM_VERT_MAX_GAIN Fld(6,6,AC_MSKW10)//[11:6]
    #define GM_MINUS_VERT_MIN_GAIN Fld(6,0,AC_MSKB0)//[5:0]
#define TTDVP_07 (IO_TTD_BASE + 0xC5C)
    #define GM_VERT_SMOOTH_WEIGHT Fld(11,21,AC_MSKW32)//[31:21]
    #define GM_VERT_GAIN Fld(6,15,AC_MSKW21)//[20:15]
    #define GM_VERT_CENTER Fld(11,4,AC_MSKW10)//[14:4]
    #define GM_VERT_GAIN_SIGN Fld(1,3,AC_MSKB0)//[3:3]
    #define GM_VERT_CTRL_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define CID_UV_INV_EN Fld(1,1,AC_MSKB0)//[1:1]
#define TTDVP_08 (IO_TTD_BASE + 0xC60)
    #define VP_INK_H_EDGE_THR Fld(12,20,AC_MSKW32)//[31:20]
    #define VP_INK_V_EDGE_THR Fld(12,8,AC_MSKW21)//[19:8]
    #define VP_TRANSIENT_STEP Fld(6,2,AC_MSKB0)//[7:2]
    #define VP_SOBEL_INK_DBG Fld(2,0,AC_MSKB0)//[1:0]
#define TTDVP_09 (IO_TTD_BASE + 0xC64)
    #define VP_LOCAL_INVERSE_FLAG Fld(1,10,AC_MSKB1)//[10:10]
    #define VP_LOCAL_INVERSE_CONTROL_EN Fld(1,9,AC_MSKB1)//[9:9]
    #define CID_EDGE_GAIN Fld(8,1,AC_MSKW10)//[8:1]
    #define VP_LETTERBOX_DETECT_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDVP_10 (IO_TTD_BASE + 0xC68)
    #define CID_DIVER_TH2 Fld(11,21,AC_MSKW32)//[31:21]
    #define CID_DIVER_TH1 Fld(11,10,AC_MSKW21)//[20:10]
    #define FW_TFU_GAIN Fld(4,6,AC_MSKW10)//[9:6]
    #define FW_LOCAL_GAIN Fld(4,2,AC_MSKB0)//[5:2]
    #define TFU_GAIN_CONTROL_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define LOCAL_GAIN_CONTROL_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDVP_11 (IO_TTD_BASE + 0xC6C)
    #define VP_CID_INV_THR2 Fld(24,6,AC_MSKDW)//[29:6]
    #define CID_HIST_MAX_TH Fld(5,1,AC_MSKB0)//[5:1]
#define TTDVP_12 (IO_TTD_BASE + 0xC70)
    #define VP_LOCAL_OFFSET Fld(8,15,AC_MSKW21)//[22:15]
    #define GM_VERT_SLOPE Fld(6,9,AC_MSKB1)//[14:9]
    #define GM_VERT_SLOPE_SIGN Fld(1,8,AC_MSKB1)//[8:8]
    #define CID_EDGE_BASE Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_13 (IO_TTD_BASE + 0xC74)
    #define GM_VERT_OFFSET Fld(13,19,AC_MSKW32)//[31:19]
    #define GM_VERT_GAIN2 Fld(6,13,AC_MSKW21)//[18:13]
    #define GM_VERT_GAIN2_SIGN Fld(1,12,AC_MSKB1)//[12:12]
    #define GM_WEIGHT_CTRL_EN Fld(1,11,AC_MSKB1)//[11:11]
    #define GM_HORI_WEIGHT Fld(11,0,AC_MSKW10)//[10:0]
#define TTDVP_14 (IO_TTD_BASE + 0xC78)
    #define GM_VERT_WEIGHT Fld(11,21,AC_MSKW32)//[31:21]
    #define GM_HORI_MAX_HIST_DIST Fld(9,12,AC_MSKW21)//[20:12]
    #define GM_HORI_MIN_HIST_DIST Fld(9,3,AC_MSKW10)//[11:3]
    #define CID_TDDC_EN Fld(1,0,AC_MSKB0)//[0:0]
#define TTDVP_15 (IO_TTD_BASE + 0xC7C)
    #define CID_CHROMA_GAIN Fld(7,25,AC_MSKB3)//[31:25]
    #define VP_ROI_X_END Fld(11,14,AC_MSKDW)//[24:14]
    #define VP_ROI_Y_END Fld(11,3,AC_MSKW10)//[13:3]
#define TTDVP_16 (IO_TTD_BASE + 0xC80)
    #define VP_LETTERBOX_HIST_TH Fld(9,23,AC_MSKW32)//[31:23]
    #define GM_MAX_HIST_DIST Fld(9,14,AC_MSKW21)//[22:14]
    #define CID_Y_WEIGHT Fld(4,10,AC_MSKB1)//[13:10]
    #define CID_C_WEIGHT Fld(4,6,AC_MSKW10)//[9:6]
    #define CID_NONLINEAR Fld(1,5,AC_MSKB0)//[5:5]
#define TTDVP_17 (IO_TTD_BASE + 0xC84)
    #define GM_HORI_CTRL_PT_0 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_PT_1 Fld(8,16,AC_FULLB2)//[23:16]
    #define GM_HORI_CTRL_PT_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define GM_HORI_CTRL_PT_3 Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_18 (IO_TTD_BASE + 0xC88)
    #define GM_HORI_CTRL_PT_4 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_PT_5 Fld(8,16,AC_FULLB2)//[23:16]
    #define GM_HORI_CTRL_PT_6 Fld(8,8,AC_FULLB1)//[15:8]
    #define GM_HORI_CTRL_PT_7 Fld(8,0,AC_FULLB0)//[7:0]
#define TTDVP_19 (IO_TTD_BASE + 0xC8C)
    #define GM_HORI_CTRL_PT_8 Fld(8,24,AC_FULLB3)//[31:24]
    #define GM_HORI_CTRL_EN Fld(1,23,AC_MSKB2)//[23:23]
#define TTDVP_20 (IO_TTD_BASE + 0xCFC)
    #define GM_VERT_GAIN_MSB Fld(5,2,AC_MSKB0)//[6:2]
    #define GM_VERT_GAIN2_MSB Fld(5,7,AC_MSKW10)//[11:7]
#define STA_TTDVP_00 (IO_TTD_BASE + 0xC90)
    #define STA_VP_HISTOGRAM_BIN_0 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_1 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_01 (IO_TTD_BASE + 0xC94)
    #define STA_VP_HISTOGRAM_BIN_2 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_3 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_02 (IO_TTD_BASE + 0xC98)
    #define STA_VP_HISTOGRAM_BIN_4 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_5 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_03 (IO_TTD_BASE + 0xC9C)
    #define STA_VP_HISTOGRAM_BIN_6 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_7 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_04 (IO_TTD_BASE + 0xCA0)
    #define STA_VP_HISTOGRAM_BIN_8 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_9 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_05 (IO_TTD_BASE + 0xCA4)
    #define STA_VP_HISTOGRAM_BIN_10 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_11 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_06 (IO_TTD_BASE + 0xCA8)
    #define STA_VP_HISTOGRAM_BIN_12 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_13 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_07 (IO_TTD_BASE + 0xCAC)
    #define STA_VP_HISTOGRAM_BIN_14 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_15 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_08 (IO_TTD_BASE + 0xCB0)
    #define STA_VP_HISTOGRAM_BIN_16 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_17 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_09 (IO_TTD_BASE + 0xCB4)
    #define STA_VP_HISTOGRAM_BIN_18 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_19 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_10 (IO_TTD_BASE + 0xCB8)
    #define STA_VP_HISTOGRAM_BIN_20 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_21 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_11 (IO_TTD_BASE + 0xCBC)
    #define STA_VP_HISTOGRAM_BIN_22 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_23 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_12 (IO_TTD_BASE + 0xCC0)
    #define STA_VP_HISTOGRAM_BIN_24 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_25 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_13 (IO_TTD_BASE + 0xCC4)
    #define STA_VP_HISTOGRAM_BIN_26 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_27 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_14 (IO_TTD_BASE + 0xCC8)
    #define STA_VP_HISTOGRAM_BIN_28 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_29 Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_15 (IO_TTD_BASE + 0xCCC)
    #define STA_VP_HISTOGRAM_BIN_30 Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_VP_HISTOGRAM_BIN_31 Fld(16,0,AC_FULLW10)//[15:0]

#if defined(CC_MT5398)
#define STA_TTDVP_16 (IO_TTD_BASE + 0xCD0)
    #define STA_GM_VERT_GAIN Fld(22,10,AC_MSKDW)//[31:10]
    #define STA_GM_MAX_VARIANCE Fld(10,0,AC_MSKW10)//[9:0]
#define STA_TTDVP_17 (IO_TTD_BASE + 0xCD4)
    #define STA_GM_VERT_GAIN2 Fld(22,10,AC_MSKDW)//[31:10]
    #define STA_GM_MAX_DIVERSITY Fld(10,0,AC_MSKW10)//[9:0]
#else
#define STA_TTDVP_16 (IO_TTD_BASE + 0xCD0)
    #define STA_GM_VERT_GAIN Fld(22,10,AC_MSKDW)//[31:10]
#define STA_TTDVP_17 (IO_TTD_BASE + 0xCD4)
    #define STA_GM_VERT_GAIN2 Fld(22,10,AC_MSKDW)//[31:10]
#define STA_TTDVP_26 (IO_TTD_BASE + 0xCF8)
    #define STA_GM_MAX_VARIANCE Fld(14,14,AC_MSKDW)//[27:14]
    #define STA_GM_MAX_DIVERSITY Fld(14,0,AC_MSKW10)//[13:0]
#endif

#define STA_TTDVP_18 (IO_TTD_BASE + 0xCD8)
    #define STA_GM_HIST_DIST_SUM Fld(17,15,AC_MSKDW)//[31:15]
    #define STA_GM_MAX_HIST_DIST Fld(11,4,AC_MSKW10)//[14:4]
    #define STA_TFU_LOCAL_GAIN Fld(4,0,AC_MSKB0)//[3:0]
#define STA_TTDVP_19 (IO_TTD_BASE + 0xCDC)
    #define STA_FINAL_INVERSE_FLAG Fld(1,31,AC_MSKB3)//[31:31]
    #define STA_GM_EDGE_COUNT Fld(21,10,AC_MSKDW)//[30:10]
#define STA_TTDVP_20 (IO_TTD_BASE + 0xCE0)
    #define STA_TTD_IMAGE_APL Fld(24,8,AC_MSKDW)//[31:8]
    #define STA_GM_HORI_CTRL_PT_0 Fld(8,0,AC_FULLB0)//[7:0]
#define STA_TTDVP_21 (IO_TTD_BASE + 0xCE4)
    #define STA_GM_HORI_CTRL_PT_1 Fld(8,24,AC_FULLB3)//[31:24]
    #define STA_GM_HORI_CTRL_PT_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define STA_GM_HORI_CTRL_PT_3 Fld(8,8,AC_FULLB1)//[15:8]
    #define STA_GM_HORI_CTRL_PT_4 Fld(8,0,AC_FULLB0)//[7:0]
#define STA_TTDVP_22 (IO_TTD_BASE + 0xCE8)
    #define STA_GM_HORI_CTRL_PT_5 Fld(8,24,AC_FULLB3)//[31:24]
    #define STA_GM_HORI_CTRL_PT_6 Fld(8,16,AC_FULLB2)//[23:16]
    #define STA_GM_HORI_CTRL_PT_7 Fld(8,8,AC_FULLB1)//[15:8]
    #define STA_GM_HORI_CTRL_PT_8 Fld(8,0,AC_FULLB0)//[7:0]

#if defined(CC_MT5398)
#define STA_TTDVP_23 (IO_TTD_BASE + 0xCF0)
    #define STA_GM_HORI_EDGE_GRAD_SUM Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_GM_VERT_EDGE_GRAD_SUM Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_24 (IO_TTD_BASE + 0xCF4)
    #define STA_GM_VERT_EDGE_CNT_0 Fld(15,17,AC_MSKW32)//[31:17]
    #define STA_GM_VERT_EDGE_CNT_1 Fld(15,2,AC_MSKDW)//[16:2]
#define STA_TTDVP_25 (IO_TTD_BASE + 0xCF8)
    #define STA_GM_VERT_EDGE_CNT_2 Fld(15,17,AC_MSKW32)//[31:17]
#else
#define STA_TTDVP_23 (IO_TTD_BASE + 0xCEC)
    #define STA_GM_HORI_EDGE_GRAD_SUM Fld(16,16,AC_FULLW32)//[31:16]
    #define STA_GM_VERT_EDGE_GRAD_SUM Fld(16,0,AC_FULLW10)//[15:0]
#define STA_TTDVP_24 (IO_TTD_BASE + 0xCF0)
    #define STA_GM_VERT_EDGE_CNT_0 Fld(15,17,AC_MSKW32)//[31:17]
    #define STA_GM_VERT_EDGE_CNT_1 Fld(15,2,AC_MSKDW)//[16:2]
#define STA_TTDVP_25 (IO_TTD_BASE + 0xCF4)
    #define STA_GM_VERT_EDGE_CNT_2 Fld(15,17,AC_MSKW32)//[31:17]
#endif

#define TTDIR_00 (IO_TTD_BASE + 0xD80)
    #define IR_OCCLUSION_MODE Fld(1,27,AC_MSKB3)//[27:27]
    #define IR_BLACK_EN Fld(1,18,AC_MSKB2)//[18:18]
    #define IR_PR_INFO Fld(1,17,AC_MSKB2)//[17:17]
    #define IR_PR_MODE Fld(1,16,AC_MSKB2)//[16:16]
#define TTDIR_01 (IO_TTD_BASE + 0xD84)
    #define IR_OCCLUSION_GAIN Fld(5,27,AC_MSKB3)//[31:27]
    #define IR_OCCLUSION_STRENGTH Fld(3,24,AC_MSKB3)//[26:24]
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
#define TTDIR_04 (IO_TTD_BASE + 0xD94)
    #define TTD_3DC_HW_MODE Fld(1,25,AC_MSKB3)//[25:25]
    #define IR_DSPRT_BLACK_CLP_EN Fld(1,24,AC_MSKB3)//[24:24]
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
    #define IR_BLACK_R Fld(8,8,AC_FULLB1)//[15:8]
    #define IR_BLACK_L Fld(8,0,AC_FULLB0)//[7:0]
#define STA_TTDIR_00 (IO_TTD_BASE + 0xDB0)
    #define STA_IR_ERR_OVER Fld(1,1,AC_MSKB0)//[1:1]
    #define STA_IR_ERR_UNDER Fld(1,0,AC_MSKB0)//[0:0]

#define TTDDG_00 (IO_TTD_BASE + 0xD00)
    #define DG_HIST_WINDOW_SIZE Fld(4,9,AC_MSKB1)//[12:9]
    #define DG_TRANSIENT_STEP Fld(6,3,AC_MSKW10)//[8:3]
    #define ADA_TFU_EN Fld(1,2,AC_MSKB0)//[2:2]
    #define ADA_LOCAL_EN Fld(1,1,AC_MSKB0)//[1:1]
    #define DG_FUSION_TYPE Fld(1,0,AC_MSKB0)//[0:0]
#define TTDDG_01 (IO_TTD_BASE + 0xD04)
    #define DG_LOCAL_BASE Fld(8,24,AC_FULLB3)//[31:24]
    #define DG_GLOBAL_BASE Fld(8,16,AC_FULLB2)//[23:16]
    #define DG_LOCAL_WEIGHT Fld(8,8,AC_FULLB1)//[15:8]
    #define DG_GLOBAL_WEIGHT Fld(8,0,AC_FULLB0)//[7:0]
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
    #define TFU_THR_HIGH Fld(16,0,AC_FULLW10)//[15:0]
#define TTDDG_06 (IO_TTD_BASE + 0xD18)
    #define DG_LOCAL_X_STR Fld(12,20,AC_MSKW32)//[31:20]
    #define DG_LOCAL_Y_STR Fld(12,0,AC_MSKW10)//[11:0]
#define TTDDG_07 (IO_TTD_BASE + 0xD1C)
    #define DG_LOCAL_X_END Fld(12,20,AC_MSKW32)//[31:20]
    #define DG_LOCAL_Y_END Fld(12,0,AC_MSKW10)//[11:0]
#define TTDDG_08 (IO_TTD_BASE + 0xD24)
    #define DG_DEMO_X_STR Fld(12,20,AC_MSKW32)//[31:20]
    #define DG_DEMO_Y_STR Fld(12,0,AC_MSKW10)//[11:0]
#define TTDDG_09 (IO_TTD_BASE + 0xD28)
    #define DG_DEMO_X_END Fld(12,20,AC_MSKW32)//[31:20]
    #define DG_DEMO_Y_END Fld(12,0,AC_MSKW10)//[11:0]
#define TTDDG_10 (IO_TTD_BASE + 0xD2C)
    #define DG_DEBUG_X_POS Fld(12,20,AC_MSKW32)//[31:20]
    #define DG_DEBUG_Y_POS Fld(12,0,AC_MSKW10)//[11:0]
#define TTDDG_11 (IO_TTD_BASE + 0xD20)
    #define GM_HORI_CTRL_PT_TOP Fld(8,0,AC_FULLB0)//[7:0]
    #define GM_HORI_CTRL_PT_BTM Fld(8,8,AC_FULLB1)//[15:8]

#endif // #ifndef _HW_TTD_H_

