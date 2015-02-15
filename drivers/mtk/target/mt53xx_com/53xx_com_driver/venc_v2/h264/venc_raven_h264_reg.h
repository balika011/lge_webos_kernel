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
 * $RCSfile:  $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_raven_h264_reg.h
 *  This header file declares register map of H.264 video encoder HW.
 */


#ifndef VENC_RAVEN_H264_REG_H
#define VENC_RAVEN_H264_REG_H

#include "x_hal_io.h"
#include "x_os.h"
#include "x_debug.h"


#define IO_VIRT     0xf0000000

#define IO_VENC_RAVEN_READ_BITS(RegName,FieldName)        IO32ReadFldAlign((IO_VIRT+RegName) ,(FieldName))   
#define IO_VENC_RAVEN_READ_REG(RegName)                   u4IO32Read4B((IO_VIRT+RegName)) 
#define IO_VENC_RAVEN_WRITE_BITS(RegName,FieldName,value) vIO32WriteFldAlign((IO_VIRT+RegName) ,(value) ,(FieldName))
#define IO_VENC_RAVEN_WRITE_REG(RegName, value)           vIO32Write4B((IO_VIRT+RegName), (value))

#define VENC 0x0003A000

#define VENC_HW_MODE_SEL (VENC + 0x000)
    //#define DLA_WDONE_STATUS Fld(1,28,AC_MSKB3)//[28:28]
    #define ENABLE_FRAME_UPDATE_RC Fld(1,27,AC_MSKB3)//[27:27]
    #define ENABLE_FRM_RC Fld(1,26,AC_MSKB3)//[26:26]
    #define CRC_CTRL_ENABLE Fld(1,25,AC_MSKB3)//[25:25]
    #define SAVE_RC_DATA Fld(1,24,AC_MSKB3)//[24:24]
    #define LOAD_RC_DATA Fld(1,23,AC_MSKB2)//[23:23]
    #define ENABLE_SEQ_RC Fld(1,22,AC_MSKB2)//[22:22]
    #define LOAD_RC_RAM_CODE Fld(1,21,AC_MSKB2)//[21:21]
    #define INSERT_KEYFRAME Fld(1,20,AC_MSKB2)//[20:20]
    #define FIRST_SPS_FLAG Fld(1,19,AC_MSKB2)//[19:19]
    #define PAUSE_MODE Fld(1,18,AC_MSKB2)//[18:18]
    #define PHOTO_MODE Fld(1,16,AC_MSKB2)//[16:16]
    #define SMI_GULTRA_CTRL Fld(11,5,AC_MSKW10)//[15:5]
    #define EN_RATECONTROL Fld(1,4,AC_MSKB0)//[4:4]
    #define VIDEO_FORMAT Fld(2,0,AC_MSKB0)//[1:0]
#define VENC_ENCODER_INFO_0 (VENC + 0x004)
    #define QP_I_FRM Fld(6,26,AC_MSKB3)//[31:26]
    #define NUM_B_FRM Fld(2,24,AC_MSKB3)//[25:24]
    #define PERIOD_I_FRM Fld(10,14,AC_MSKW21)//[23:14]
    #define PERIOD_IDR_FRM Fld(10,0,AC_MSKW10)//[9:0]
#define VENC_ENCODER_INFO_1 (VENC + 0x008)
    #define QP_P_FRM Fld(6,10,AC_MSKB1)//[15:10]
    #define P_SEARCH_V Fld(2,8,AC_MSKB1)//[9:8]
    #define P_SEARCH_H Fld(3,5,AC_MSKB0)//[7:5]
    #define P_16X16 Fld(1,3,AC_MSKB0)//[3:3]
    #define P_16X08 Fld(1,2,AC_MSKB0)//[2:2]
    #define P_08X16 Fld(1,1,AC_MSKB0)//[1:1]
    #define P_08X08 Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_ENCODER_INFO_2 (VENC + 0x00C)
    #define RDO_INTERTR8MODE Fld(3,29,AC_MSKB3)//[31:29]
    #define RDO_INTERTR4MODE Fld(3,26,AC_MSKB3)//[28:26]
    #define RDO_INTRATR8MODE Fld(3,23,AC_MSKW32)//[25:23]
    #define RDO_INTRATR4MODE Fld(3,20,AC_MSKB2)//[22:20]
    #define RDO_INTRA16ENABLE Fld(1,19,AC_MSKB2)//[19:19]
    #define RDO_SKIPENABLE Fld(1,18,AC_MSKB2)//[18:18]
    #define RDO_BDIRECTENABLE Fld(1,17,AC_MSKB2)//[17:17]
#define VENC_ENCODER_INFO_3 (VENC + 0x010)
    #define IME_REFINE_MODE Fld(1,20,AC_MSKB2)//[20:20]
    #define PFRM_NONREF_P Fld(1,18,AC_MSKB2)//[18:18]
    #define CHANGE_B_2_NONREF_P Fld(1,16,AC_MSKB2)//[16:16]
    #define GEN_REC_FRM Fld(1,15,AC_MSKB1)//[15:15]
    #define COMV_SIGN Fld(1,14,AC_MSKB1)//[14:14]
    #define COMV_SCALER Fld(3,11,AC_MSKB1)//[13:11]
    #define INTRA_WEIGHT_COST_EN Fld(1,10,AC_MSKB1)//[10:10]
    #define COMVSEARCH_EN Fld(1,8,AC_MSKB1)//[8:8]
    #define VENC_PDIR Fld(2,6,AC_MSKB0)//[7:6]
    #define IDR_PIC_ID Fld(1,3,AC_MSKB0)//[3:3]
    #define IS_IDR_FRM Fld(1,2,AC_MSKB0)//[2:2]
    #define VENC_IMG_TYPE Fld(2,0,AC_MSKB0)//[1:0]
#define VENC_ENCODER_INFO_4 (VENC + 0x014)
    #define VENC_FRAME_CNT Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_ENCODER_INFO_5 (VENC + 0x018)
    #define VENC_FRAME_IDX Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_ENCODER_INFO_6 (VENC + 0x01C)
    #define VENC_LAST_IDR_FRAME_IDX Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_ENCODER_INFO_7 (VENC + 0x020)
    #define VENC_ENCODER_INFO_7_ADD Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_IMAGE_INFO_0 (VENC + 0x024)
    #define IMAGE_NUM_MB Fld(16,16,AC_FULLW32)//[31:16]
    #define IMAGE_HEIGHT_DIV16 Fld(8,8,AC_FULLB1)//[15:8]
    #define IMAGE_WIDTH_DIV16 Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_IMAGE_INFO_1 (VENC + 0x028)
    #define SOURCE_SCAN_TYPE Fld(3,29,AC_MSKB3)//[31:29]
    #define UV_PITCH_WIDTH_DIV16 Fld(8,21,AC_MSKW32)//[28:21]
    #define PITCH_WIDTH_DIV16 Fld(8,11,AC_MSKW21)//[18:11]
    #define IMAGE_CROP_RIGHT Fld(6,5,AC_MSKW10)//[10:5]
    #define IMAGE_CROP_BOTTOM Fld(5,0,AC_MSKB0)//[4:0]
#define VENC_IMAGE_INFO_2 (VENC + 0x02C)
    #define REV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_H264_ENC_INFO_0 (VENC + 0x030)
    #define CABAC Fld(1,31,AC_MSKB3)//[31:31]
    #define PROFILE Fld(2,29,AC_MSKB3)//[30:29]
    #define MBAFF Fld(1,28,AC_MSKB3)//[28:28]
    #define H264_LEVEL Fld(6,22,AC_MSKW32)//[27:22]
    #define USE_TR8X8 Fld(1,21,AC_MSKB2)//[21:21]
    #define SLICE_ALPHA_C0_OFFSET_DIV2 Fld(4,17,AC_MSKB2)//[20:17]
    #define SLICE_BETA_C0_OFFSET_DIV2 Fld(4,13,AC_MSKW21)//[16:13]
#define VENC_H264_ENC_INFO_1 (VENC + 0x034)
    #define REV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_RATECONTROL_INFO_0 (VENC + 0x048)
    #define RC_CBR Fld(1,18,AC_MSKB2)//[18:18]
    #define RC_INI_QP Fld(1,17,AC_MSKB2)//[17:17]
    #define RC_TARGET_BIT_RATE Fld(17,0,AC_MSKDW)//[16:0]
#define VENC_RATECONTROL_INFO_1 (VENC + 0x04C)
    #define ENABLE_EIS Fld(1,31,AC_MSKB3)//[31:31]
    #define ENABLE_ROI Fld(1,30,AC_MSKB3)//[30:30]
    #define AIFI Fld(1,27,AC_MSKB3)//[27:27]
    #define SKYPE_MODE Fld(1,26,AC_MSKB3)//[26:26]
    #define AFPS Fld(1,25,AC_MSKB3)//[25:25]
    #define ATBR Fld(1,24,AC_MSKB3)//[24:24]
    #define RC_FPS Fld(8,16,AC_FULLB2)//[23:16]
    #define BFRMQLIMTER Fld(8,8,AC_FULLB1)//[15:8]
    #define PFRMQLIMTER Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_RATECONTROL_INFO_2 (VENC + 0x050)
    #define REV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_RATECONTROL_INFO_3 (VENC + 0x054)
    #define REV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CODEC_CTRL (VENC + 0x058)
    #define RELEASE_PAUSE_FRM Fld(1,4,AC_MSKB0)//[4:4]
    #define RELEASE_BS_DRAM Fld(1,3,AC_MSKB0)//[3:3]
    #define ENC_FRM Fld(1,2,AC_MSKB0)//[2:2]
    #define ENC_PPS Fld(1,1,AC_MSKB0)//[1:1]
    #define ENC_SPS Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_IRQ_STATUS (VENC + 0x05C)
    #define PAUSE_FRM_INT Fld(1,4,AC_MSKB0)//[4:4]
    #define BS_DRAM_FULL_INT Fld(1,3,AC_MSKB0)//[3:3]
    #define ENC_FRM_INT Fld(1,2,AC_MSKB0)//[2:2]
    #define ENC_PPS_INT Fld(1,1,AC_MSKB0)//[1:1]
    #define ENC_SPS_INT Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_IRQ_ACK (VENC + 0x060)
    #define PAUSE_FRM_ACK Fld(1,4,AC_MSKB0)//[4:4]
    #define BS_DRAM_FULL_ACK Fld(1,3,AC_MSKB0)//[3:3]
    #define ENC_FRM_ACK Fld(1,2,AC_MSKB0)//[2:2]
    #define ENC_PPS_ACK Fld(1,1,AC_MSKB0)//[1:1]
    #define ENC_SPS_ACK Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_BITSTREAM_BUF_DRAM_ADDR (VENC + 0x064)
    #define BITSTREAM_BUF_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_BITSTREAM_BUF_DRAM_SIZE (VENC + 0x068)
    #define BITSTREAM_BUF_DRAM_SIZE_DIV128 Fld(25,0,AC_MSKDW)//[24:0]
#define VENC_FRM_CUR_Y_DRAM_ADDR (VENC + 0x06C)
    #define FRM_CURR_Y_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_FRM_CUR_UV_DRAM_ADDR (VENC + 0x070)
    #define FRM_CURR_UV_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_FRM_REF_Y_DRAM_ADDR (VENC + 0x074)
    #define FRM_REF_Y_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_FRM_REF_UV_DRAM_ADDR (VENC + 0x078)
    #define FRM_REF_UV_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_FRM_REC_Y_DRAM_ADDR (VENC + 0x07C)
    #define FRM_REC_Y_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_FRM_REC_UV_DRAM_ADDR (VENC + 0x080)
    #define FRM_REC_UV_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_LOAD_COL_INFO_DRAM_ADDR (VENC + 0x084)
    #define LOAD_COL_INFO_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_SAVE_COL_INFO_DRAM_ADDR (VENC + 0x088)
    #define SAVE_COL_INFO_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_RC_CODE_DRAM_ADDR (VENC + 0x08C)
    #define RC_CODE_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_RC_INFO_DRAM_ADDR (VENC + 0x090)
    #define RC_INFO_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_CLK_CG_CTRL (VENC + 0x094)
    #define CLK_CG_CTRL Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_PIC_BITSTREAM_BYTE_CNT (VENC + 0x098)
    #define PIC_BITSTREAM_BYTE_CNT Fld(24,0,AC_MSKDW)//[23:0]
#define VENC_CODEC_CNT0 (VENC + 0x09C)
    #define CODEC_COUNTER0 Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_IRQ_MODE_SEL (VENC + 0x0A4)
    #define PAUSE_FRM_IRQ_ENABLE Fld(1,4,AC_MSKB0)//[4:4]
    #define BS_DRAM_FULL_IRQ_ENABLE Fld(1,3,AC_MSKB0)//[4:4]
    #define ENC_FRM_IRQ_ENABLE Fld(1,2,AC_MSKB0)//[4:4]
    #define ENC_PPS_IRQ_ENABLE Fld(1,1,AC_MSKB0)//[4:4]
    #define ENC_SPS_IRQ_ENABLE Fld(1,0,AC_MSKB0)//[4:4]
#define VENC_SW_HRST_N (VENC + 0x0A8)
    #define SW_HRST_N Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_SW_PAUSE (VENC + 0x0AC)
    #define SW_PAUSE Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_PAUSE_MODE_INFO (VENC + 0x0B0)
    #define PAUSE_MB_CNT Fld(15,0,AC_MSKW10)//[14:0]
#define VENC_CRC_BSDMA (VENC + 0x0BC)
    #define CRC_EC_BSDMA Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_CUR_LUMA (VENC + 0x0C0)
    #define CRC_IFME_CUR_LUMA Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_REF_LUMA (VENC + 0x0C4)
    #define CRC_IFME_REF_LUMA Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_CUR_CHROMA (VENC + 0x0C8)
    #define CRC_IMRR_CUR_CHROMA Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_REF_CHROMA (VENC + 0x0CC)
    #define CRC_IMRR_REF_CHROMA Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_RD_COMV (VENC + 0x0D0)
    #define CRC_IFME_RD_COMV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_SV_COMV (VENC + 0x0D4)
    #define CRC_IMRR_SV_COMV Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_REC_FRM (VENC + 0x0D8)
    #define CRC_DB_REC_FRM Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CRC_BSDMA_1 (VENC + 0x0DC)
    #define CRC_EC_BSDMA_1 Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_PIC_BITSTREAM_BYTE_CNT1 (VENC + 0x0E8)
    #define VENC_PIC_BITSTREAM_BYTE_CNT1_ADD Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_CE (VENC + 0x0EC)
    #define CE Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_CLK_DCM_CTRL (VENC + 0x0F4)
    #define CLK_DCM_CTRL Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_ROI_INDEX_0 (VENC + 0x100)
    #define MB_Y_MAX_0 Fld(8,24,AC_FULLB3)//[31:24]
    #define MB_X_MAX_0 Fld(8,16,AC_FULLB2)//[23:16]
    #define MB_Y_MIN_0 Fld(8,8,AC_FULLB1)//[15:8]
    #define MB_X_MIN_0 Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_ROI_INDEX_1 (VENC + 0x104)
    #define MB_Y_MAX_1 Fld(8,24,AC_FULLB3)//[31:24]
    #define MB_X_MAX_1 Fld(8,16,AC_FULLB2)//[23:16]
    #define MB_Y_MIN_1 Fld(8,8,AC_FULLB1)//[15:8]
    #define MB_X_MIN_1 Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_ROI_INDEX_2 (VENC + 0x108)
    #define MB_Y_MAX_2 Fld(8,24,AC_FULLB3)//[31:24]
    #define MB_X_MAX_2 Fld(8,16,AC_FULLB2)//[23:16]
    #define MB_Y_MIN_2 Fld(8,8,AC_FULLB1)//[15:8]
    #define MB_X_MIN_2 Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_ROI_INDEX_3 (VENC + 0x10C)
    #define MB_Y_MAX_3 Fld(8,24,AC_FULLB3)//[31:24]
    #define MB_X_MAX_3 Fld(8,16,AC_FULLB2)//[23:16]
    #define MB_Y_MIN_3 Fld(8,8,AC_FULLB1)//[15:8]
    #define MB_X_MIN_3 Fld(8,0,AC_FULLB0)//[7:0]
#define VENC_EIS_SAD_CUR_FRM (VENC + 0x110)
    #define EIS_SAD_CUR_FRM Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_EIS_SAD_PREV_FRM_0 (VENC + 0x114)
    #define EIS_SAD_PREV_FRM_0 Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_EIS_SAD_PREV_FRM_1 (VENC + 0x118)
    #define EIS_SAD_PREV_FRM_1 Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_EIS_SAD_PREV_FRM_2 (VENC + 0x11C)
    #define EIS_SAD_PREV_FRM_2 Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_EIS_INFO_0 (VENC + 0x120)
    #define EIS_NUM_H_WINDOW Fld(3,24,AC_MSKB3)//[26:24]
    #define EIS_NUM_V_WINDOW Fld(4,16,AC_MSKB2)//[19:16]
    #define EIS_NUM_H_RP Fld(5,8,AC_MSKB1)//[12:8]
    #define EIS_NUM_V_RP Fld(3,0,AC_MSKB0)//[2:0]
#define VENC_AVSR_ANRP_INFO_0 (VENC + 0x124)
    #define ANRP_FRM_NUM Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_AVSR_ANRP_INFO_1 (VENC + 0x128)
    #define ANRP_ENABLE Fld(1,20,AC_MSKB2)//[20:20]
    #define AVSR_ENABLE Fld(1,16,AC_MSKB2)//[16:16]
    #define ANRP_NUM_PREV_NRP Fld(2,8,AC_MSKB1)//[9:8]
#define VENC_SMI_GULTRA_THRES (VENC + 0x12C)
    #define GULTRA_THRES Fld(12,16,AC_MSKW32)//[27:16]
    #define GPREULTRA_THRES Fld(12,0,AC_MSKW10)//[11:0]
#define VENC_FRM_CUR_V_DRAM_ADDR (VENC + 0x130)
    #define FRM_CURR_V_DRAM_ADDR_DIV16 Fld(28,0,AC_MSKDW)//[27:0]
#define VENC_VUI_PARAMETER_INFO_0 (VENC + 0x180)
    #define ENABLEVUISUPPORT Fld(1,31,AC_MSKB3)//[31:31]
    #define VUI_ASPECT_RATIO_INFO_PRESENT_FLAG_ Fld(1,30,AC_MSKB3)//[30:30]
    #define VUI_ASPECT_RATIO_IDC_ Fld(8,22,AC_MSKW32)//[29:22]
    #define VUI_OVERSCAN_INFO_PRESENT_FLAG_ Fld(1,21,AC_MSKB2)//[21:21]
    #define VUI_OVERSCAN_APPROPRIATE_FLAG_ Fld(1,20,AC_MSKB2)//[20:20]
    #define VUI_VIDEO_SIGNAL_TYPE_PRESENT_FLAG_ Fld(1,19,AC_MSKB2)//[19:19]
    #define VUI_VIDEO_FORMAT_ Fld(3,16,AC_MSKB2)//[18:16]
    #define VUI_VIDEO_FULL_RANGE_FLAG_ Fld(1,15,AC_MSKB1)//[15:15]
    #define VUI_COLOUR_DESCRIPTION_PRESENT_FLAG_ Fld(1,14,AC_MSKB1)//[14:14]
    #define VUI_CHROMA_LOCATION_INFO_PRESENT_FLAG_ Fld(1,13,AC_MSKB1)//[13:13]
    #define VUI_CHROMA_SAMPLE_LOC_TYPE_TOP_FIELD_ Fld(3,10,AC_MSKB1)//[12:10]
    #define VUI_CHROMA_SAMPLE_LOC_TYPE_BOTTOM_FIELD_ Fld(3,7,AC_MSKW10)//[9:7]
    #define VUI_TIMING_INFO_PRESENT_FLAG_ Fld(1,6,AC_MSKB0)//[6:6]
    #define VUI_FIXED_FRAME_RATE_FLAG_ Fld(1,5,AC_MSKB0)//[5:5]
    #define VUI_NAL_HRD_PARAMETERS_PRESENT_FLAG_ Fld(1,4,AC_MSKB0)//[4:4]
    #define VUI_VCL_HRD_PARAMETERS_PRESENT_FLAG_ Fld(1,3,AC_MSKB0)//[3:3]
    #define VUI_LOW_DELAY_HRD_FLAG_ Fld(1,2,AC_MSKB0)//[2:2]
    #define VUI_PIC_STRUCT_PRESENT_FLAG_ Fld(1,1,AC_MSKB0)//[1:1]
    #define VUI_BITSTREAM_RESTRICTION_FLAG_ Fld(1,0,AC_MSKB0)//[0:0]
#define VENC_VUI_PARAMETER_INFO_1 (VENC + 0x184)
    #define VUI_COLOUR_PRIMARIES_ Fld(8,24,AC_FULLB3)//[31:24]
    #define VUI_TRANSFER_CHARACTERISTICS_ Fld(8,16,AC_FULLB2)//[23:16]
    #define VUI_MATRIX_COEFFICIENTS_ Fld(8,8,AC_FULLB1)//[15:8]
#define VENC_VUI_PARAMETER_INFO_2 (VENC + 0x188)
    #define VUI_SAR_WIDTH_ Fld(16,16,AC_FULLW32)//[31:16]
    #define VUI_SAR_HEIGHT_ Fld(16,0,AC_FULLW10)//[15:0]
#define VENC_VUI_PARAMETER_INFO_3 (VENC + 0x18C)
    #define VUI_NUM_UNITS_IN_TICK_ Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_VUI_PARAMETER_INFO_4 (VENC + 0x190)
    #define VUI_TIME_SCALE_ Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_VUI_PARAMETER_INFO_5 (VENC + 0x194)
    #define VUI_NAL_BIT_RATE_SCALE_ Fld(4,25,AC_MSKB3)//[28:25]
    #define VUI_NAL_CPB_SIZE_SCALE_ Fld(4,21,AC_MSKW32)//[24:21]
    #define VUI_NAL_VBR_CBR_FLAG_ Fld(1,20,AC_MSKB2)//[20:20]
    #define VUI_NAL_INITIAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1 Fld(5,15,AC_MSKW21)//[19:15]
    #define VUI_NAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1 Fld(5,10,AC_MSKB1)//[14:10]
    #define VUI_NAL_DPB_OUTPUT_DELAY_LENGTH_MINUS1_ Fld(5,5,AC_MSKW10)//[9:5]
    #define VUI_NAL_TIME_OFFSET_LENGTH_ Fld(5,0,AC_MSKB0)//[4:0]
#define VENC_VUI_PARAMETER_INFO_6 (VENC + 0x198)
    #define VUI_NAL_BIT_RATE_VALUE_MINUS1_ Fld(15,16,AC_MSKW32)//[30:16]
    #define VUI_NAL_CPB_SIZE_VALUE_MINUS1_ Fld(15,0,AC_MSKW10)//[14:0]
#define VENC_VUI_PARAMETER_INFO_7 (VENC + 0x19C)
    #define VUI_VCL_BIT_RATE_SCALE_ Fld(4,25,AC_MSKB3)//[28:25]
    #define VUI_VCL_CPB_SIZE_SCALE_ Fld(4,21,AC_MSKW32)//[24:21]
    #define VUI_VCL_VBR_CBR_FLAG_ Fld(1,20,AC_MSKB2)//[20:20]
    #define VUI_VCL_INITIAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1 Fld(5,15,AC_MSKW21)//[19:15]
    #define VUI_VCL_CPB_REMOVAL_DELAY_LENGTH_MINUS1_ Fld(5,10,AC_MSKB1)//[14:10]
    #define VUI_VCL_DPB_OUTPUT_DELAY_LENGTH_MINUS1_ Fld(5,5,AC_MSKW10)//[9:5]
    #define VUI_VCL_TIME_OFFSET_LENGTH_ Fld(5,0,AC_MSKB0)//[4:0]
#define VENC_VUI_PARAMETER_INFO_8 (VENC + 0x1A0)
    #define VUI_VCL_BIT_RATE_VALUE_MINUS1_ Fld(15,16,AC_MSKW32)//[30:16]
    #define VUI_VCL_CPB_SIZE_VALUE_MINUS1_ Fld(15,0,AC_MSKW10)//[14:0]
#define VENC_VUI_PARAMETER_INFO_9 (VENC + 0x1A4)
    #define VUI_MOTION_VECTORS_OVER_PIC_BOUNDARIES_FLAG Fld(1,30,AC_MSKB3)//[30:30]
    #define VUI_MAX_BYTES_PER_PIC_DENOM_ Fld(5,25,AC_MSKB3)//[29:25]
    #define VUI_MAX_BITS_PER_MB_DENOM_ Fld(5,20,AC_MSKW32)//[24:20]
    #define VUI_LOG2_MAX_MV_LENGTH_VERTICAL_ Fld(5,15,AC_MSKW21)//[19:15]
    #define VUI_LOG2_MAX_MV_LENGTH_HORIZONTAL_ Fld(5,10,AC_MSKB1)//[14:10]
    #define VUI_NUM_REORDER_FRAMES_ Fld(5,5,AC_MSKW10)//[9:5]
    #define VUI_MAX_DEC_FRAME_BUFFERING_ Fld(5,0,AC_MSKB0)//[4:0]
#define VENC_DLA_CTRL (VENC + 0x1B8)
    #define DLA_TIME_OUT_R_THRESHOLD Fld(5,0,AC_MSKB0)//[4:0]
    #define DLA_WDLE_FIFO_EMPTY_DET Fld(1,5,AC_MSKB0)//[5:5]
    #define DLA_RDONE_EMPTY_DET Fld(1,6,AC_MSKB0)//[6:6]
    #define DLA_SEAMLESS_CMD_DIS Fld(1,7,AC_MSKB0)//[7:7]
    #define DLA_TIME_OUT_W_THRESHOLD Fld(5,8,AC_MSKB1)//[12:8]
    #define DLA_RADDR_THRESHOLD Fld(6,16,AC_MSKB2)//[21:16]
    #define DLA_WADDR_THRESHOLD Fld(5,24,AC_MSKB3)//[28:24]
#define VENC_DLA_PRIORITY_G1 (VENC + 0x1BC)
    #define DLA_PRIORITY_GROUP1 Fld(32,0,AC_FULLDW)//[31:0]
#define VENC_DLA_PRIORITY_G2 (VENC + 0x1C0)
    #define DLA_PRIORITY_GROUP2 Fld(8,0,AC_FULLB0)//[7:0]

#endif // !VENC_RAVEN_H264_REG_H

