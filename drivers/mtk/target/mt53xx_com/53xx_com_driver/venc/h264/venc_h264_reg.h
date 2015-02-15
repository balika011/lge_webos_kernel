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
 * $RCSfile: venc_h264reg.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264reg.h
 *  This header file declares register map of H.264 video encoder HW.
 */


#ifndef VENC_H264REG_H
#define VENC_H264REG_H

#define VIDEO_HW_MODE_SEL  0x3a000
  #define CABAC Fld(1, 31, AC_MSKB3) //31
  #define MAIN_PROFILE Fld(1, 30, AC_MSKB3) //30
  #define MBAFF Fld(1, 29, AC_MSKB3) //29
  #define COARSE_SEARCH Fld(1, 28, AC_MSKB3) //28
  #define NUM_B_FRM Fld(2, 26, AC_MSKB3) //26:27
  #define PERIOD_I_FRM Fld(10, 16, AC_MSKW32) //16:25
  #define SW_RST Fld(1, 15, AC_MSKB1) //15
  #define CE Fld(1, 14, AC_MSKB1) //14
  //#define rev Fld(2, 12, AC_MSKB1) //12:13
  //#define rev Fld(2, 10, AC_MSKB1) //10:11
  //#define rev Fld(2, 8, AC_MSKB1) //8:9
  //#define rev Fld(2, 6, AC_MSKB0) //6:7
  #define DEC_NOBSM_EN Fld(1, 5, AC_MSKB0) //5
  #define DM_WDONE_EN Fld(1, 4, AC_MSKB0) //4
  #define DUMP_CS_INFO Fld(1, 3, AC_MSKB0) //3
  #define MPEG4 Fld(1, 2, AC_MSKB0) //2
  #define EN_RATECONTROL Fld(1, 1, AC_MSKB0) //1
  #define TWO_STAGE_CONTROL Fld(1, 0, AC_MSKB0) //0
#define VIDEO_ENCODER_INFO_0  0x3a004
  #define QP_I_FRM Fld(6, 24, AC_MSKB3) //24:29
  #define DRAM_PAGE_SIZE Fld(7, 16, AC_MSKB2) //16:22
  #define IMAGE_CROP_RIGHT Fld(4, 10, AC_MSKB1) //10:13
  #define PERIOD_IDR_FRM Fld(10, 0, AC_MSKW10) //0:9
#define VIDEO_ENCODER_INFO_1  0x3a008
  #define QP_B_FRM Fld(6, 26, AC_MSKB3) //26:31
  #define B_SEARCH_V Fld(2, 24, AC_MSKB3) //24:25
  #define B_SEARCH_H Fld(2, 21, AC_MSKB2) //21:22
  #define B_DIRECT Fld(1, 20, AC_MSKB2) //20
  #define B_16X16 Fld(1, 19, AC_MSKB2) //19
  #define B_16X08 Fld(1, 18, AC_MSKB2) //18
  #define B_08X16 Fld(1, 17, AC_MSKB2) //17
  #define B_08X08 Fld(1, 16, AC_MSKB2) //16
  #define QP_P_FRM Fld(6, 10, AC_MSKB1) //10:15
  #define P_SEARCH_V Fld(2, 8, AC_MSKB1) //8:9
  #define P_SEARCH_H Fld(3, 5, AC_MSKB0) //5:7
  #define P_SKIP Fld(1, 4, AC_MSKB0) //4
  #define P_16X16 Fld(1, 3, AC_MSKB0) //3
  #define P_16X08 Fld(1, 2, AC_MSKB0) //2
  #define P_08X16 Fld(1, 1, AC_MSKB0) //1
  #define P_08X08 Fld(1, 0, AC_MSKB0) //0
#define VIDEO_IMAGE_INFO_0  0x3a00c
  #define COARSE_IMAGE_CROP_BOTTOM Fld(3, 28, AC_MSKB3) //28:30
  #define IMAGE_NUM_MB Fld(12, 16, AC_MSKW32) //16:27
  #define IMAGE_HEIGHT_DIV16 Fld(6, 8, AC_MSKB1) //8:13
  #define IMAGE_WIDTH_DIV16 Fld(7, 0, AC_MSKB0) //0:6
#define VIDEO_IMAGE_INFO_1  0x3a010
  #define H264_LEVEL Fld(6, 26, AC_MSKB3) //26:31
  #define COARSE_IMAGE_NUM_BLK Fld(10, 16, AC_MSKW32) //16:25
  #define COARSE_IMAGE_HEIGHT_DIV8 Fld(5, 11, AC_MSKB1) //11:15
  #define COARSE_IMAGE_WIDTH_DIV8 Fld(6, 5, AC_MSKW10) //5:10
  #define IMAGE_CROP_BOTTOM Fld(5, 0, AC_MSKB0) //0:4
#define VIDEO_RATECONTROL_INFO_1  0x3a014
  #define RCU_CODE_LENGTH_ADD8 Fld(13, 16, AC_MSKW32) //16:28
  #define RC_CBR Fld(1, 15, AC_MSKB1) //15
  #define RC_INI_QP Fld(1, 14, AC_MSKB1) //14
  #define RC_TARGET_BIT_RATE Fld(14, 0, AC_MSKW10) //0:13
#define VIDEO_BITSTREAM_BUF_DRAM_INI_ADDR  0x3a018
  #define BITSTREAM_BUF_DRAM_INI_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_BITSTREAM_BUF_BYTE_SIZE  0x3a01c
  #define BITSTREAM_BUF_BYTE_SIZE Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_CURR_Y_DRAM_ADDR 0x3a020
  #define FRM_CURR_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_CURR_UV_DRAM_ADDR 0x3a024
  #define FRM_CURR_UV_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_REC0_Y_DRAM_ADDR 0x3a028
  #define FRM_REC0_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_REC1_Y_DRAM_ADDR 0x3a02c
  #define FRM_REC1_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_REC0_UV_DRAM_ADDR 0x3a030
  #define FRM_REC0_UV_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_FRM_REC1_UV_DRAM_ADDR 0x3a034
  #define FRM_REC1_UV_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_COARSE_FRM_CUR_Y_DRAM_ADDR0 0x3a038
  #define COARSE_FRM_CUR_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_COARSE_FRM_RECF_Y_DRAM_ADDR0 0x3a03c
  #define COARSE_FRM_RECF_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_COARSE_FRM_RECB_Y_DRAM_ADDR0 0x3a040
  #define COARSE_FRM_RECB_Y_DRAM_ADDR Fld(29, 0, AC_MSKDW) //0:28
#define VIDEO_IMAGE_INFO_2 0x3a044
  #define insert_keyframe Fld(1, 0, AC_MSKB0) //0
#define VIDEO_UPPER_INFO_MB_DRAM_ADDR 0x3a060
  #define UPPER_INFO_MB_DRAM_ADDR Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_COL_INFO_DRAM_ADDR 0x3a064
  #define COL_INFO_DRAM_ADDR Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_SPARE_REG0 0x3a068
  #define SPAREREG0 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_SPARE_REG1 0x3a06c
  #define SPARE_REG1 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_SPARE_REG2 0x3a070
  #define SPARE_REG2 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_SPARE_REG3 0x3a074
  #define SPARE_REG3 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_SOURCE_INFO 0x3a078
  #define source_scan_type Fld(2, 8, AC_MSKB1) //8:9
  #define pitch_width_div16 Fld(8, 0, AC_FULLB0) //0:7
#define VIDEO_RC_CTL 0x3a07c
  #define AFPS Fld(1, 25, AC_MSKB3) //25
  #define ATBR Fld(1, 24, AC_MSKB3) //24
  #define RC_FPS Fld(8, 16, AC_FULLB2) //16:23
  #define BfrmQLimter Fld(8, 8, AC_FULLB1) //8:15
  #define PfrmQLimter Fld(8, 0, AC_FULLB0) //0:7
#define VIDEO_RC_CODE_DRAM_ADDR  0x3a084
  #define RC_CODE_DRAM_ADDR Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_PIC_BITSTREAM_BYTE_CNT  0x3a088
  #define PIC_BITSTREAM_BYTE_CNT Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_PIC_BITSTREAM_DRAM_BYTE_ADDR_DIV8  0x3a08c
  #define PIC_BITSTREAM_DRAM_BYTE_ADDR_DIV8 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_CODEC_CONTROL  0x3a09c
  #define FIRST_SPS_FLAG Fld(1, 24, AC_MSKB3) //24
  #define MB_PIPELINE_START Fld(1, 23, AC_MSKB2) //23
  #define DEBUG_1 Fld(1, 22, AC_MSKB2) //22
  #define COPY_B Fld(1, 21, AC_MSKB2) //21
  #define RCU_LD Fld(1, 20, AC_MSKB2) //20
  #define ENC_FRM Fld(1, 19, AC_MSKB2) //19
  #define ENC_PPS Fld(1, 18, AC_MSKB2) //18
  #define ENC_SPS Fld(1, 17, AC_MSKB2) //17
  #define ENC_DIP Fld(1, 16, AC_MSKB2) //16
  #define DEC_ENDSEQ Fld(1, 14, AC_MSKB1) //14
  #define DEC_NO_MORE_BSM Fld(1, 11, AC_MSKB1) //11
  #define DEC_NO_START_CODE Fld(1, 10, AC_MSKB1) //10
  #define DEC_MB_ERR Fld(1, 9, AC_MSKB1) //9
  #define DEC_H_ERR Fld(1, 8, AC_MSKB1) //8
  #define DEC_UPD Fld(1, 3, AC_MSKB0) //3
  #define DEC_SH Fld(1, 2, AC_MSKB0) //2
  #define DEC_SPS Fld(1, 0, AC_MSKB0) //0
#define VIDEO_HW_INTERRUPT 0x3a0a0
  #define ENC_CS_INT Fld(1, 23, AC_MSKB2) //23
  #define FRAME_BIT_FLAG Fld(1, 22, AC_MSKB2) //22
  #define MB_BIT_FLAG Fld(1, 21, AC_MSKB2) //21
  #define RC_INT Fld(1, 20, AC_MSKB2) //20
  #define ENC_UPDIP_INT Fld(1, 19, AC_MSKB2) //19
  #define ENC_FRM_INT Fld(1, 18, AC_MSKB2) //18
  #define ENC_PPS_INT Fld(1, 17, AC_MSKB2) //17
  #define ENC_SPS_INT Fld(1, 16, AC_MSKB2) //16
#define VIDEO_MAX_PIC_BYTECNT_ADDR 0x3a0ac
  #define MAX_PIC_BITSTREAM_BYTE_CNT Fld(22, 0, AC_MSKDW) //0:21
#define VIDEO_DEBUG_INFO_0 0x3a0c0
  #define STOP_MB_NUM Fld(12, 16, AC_MSKW32) //16:27
  #define STOP_FRM_NUM Fld(16, 0, AC_FULLW10) //0:15
#define VIDEO_DEBUG_INFO_1 0x3a0c4
  #define DUMP_REG_DATA_END_ADDR Fld(12, 16, AC_MSKW32) //16:27
  #define DUMP_REG_DATA_INI_ADDR Fld(12, 0, AC_MSKW10) //0:11
#define VIDEO_DUMP_INFO_DRAM_ADDR 0x3a0c8
  #define DUMP_INFO_DRAM_ADDR Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_COARSE_MULTIPLIER 0x3a0cc
  #define COARSE_P_SEARCH_H Fld(3, 28, AC_MSKB3) //28:30
  #define COARSE_P_SEARCH_V Fld(2, 24, AC_MSKB3) //24:25
  #define COARSE_B_SEARCH_H Fld(2, 20, AC_MSKB2) //20:21
  #define COARSE_B_SEARCH_V Fld(2, 16, AC_MSKB2) //16:17
  #define COARSE_MULTIPLIER Fld(12, 0, AC_MSKW10) //0:11
#define VIDEO_REPORT_QP  0x3a0d0
  #define REPORT_DIP_QP Fld(6, 0, AC_MSKB0) //0:5
#define VIDEO_CODEC_INTM  0x3a0d4
  #define FCS_INTM Fld(1, 31, AC_MSKB3) //31
  #define RCU_INTM Fld(1, 28, AC_MSKB3) //28
  #define ENC_UPDIP_INTM Fld(1, 27, AC_MSKB3) //27
  #define ENC_FRM_INTM Fld(1, 26, AC_MSKB3) //26
  #define ENC_PPS_INTM Fld(1, 25, AC_MSKB3) //25
  #define ENC_SPS_INTM Fld(1, 24, AC_MSKB3) //24
#define VIDEO_CODEC_COUNTER0 0x3a0d8
  //#define CODEC_COUNTER0 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_CODEC_COUNTER1 0x3a0dc
  //#define CODEC_COUNTER1 Fld(32, 0, AC_MSKDW) //0:31
#define VIDEO_RC_INFO_DRAM_ADDR 0x3a0e0
  //#define rev Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_CS_INFO_DRAM_ADDR 0x3a0e4
  #define CS_INFO_DRAM_ADDR Fld(27, 0, AC_MSKDW) //0:26
#define VIDEO_HW_MODE_SEL_2 0x3a0e8
  //#define rev Fld(15, 17, AC_MSKW32) //17:31
  #define STUFF_EN Fld(1, 16, AC_MSKB2) //16
  #define SRAM_SP10 Fld(2, 8, AC_MSKB1) //8:9
  #define SRAM_SP01 Fld(2, 6, AC_MSKB0) //6:7
  #define SRAM_TP10 Fld(2, 4, AC_MSKB0) //4:5
  #define SRAM_TP01 Fld(2, 2, AC_MSKB0) //2:3
  #define SRAM_TP00 Fld(2, 0, AC_MSKB0) //0:1
#define VIDEO_STUFF_REPORT 0x3a0ec
  #define STUFF_BYTE Fld(24, 0, AC_MSKDW) //0:23
#define VIDEO_SRAM_PD_CONTROL 0x3a0f0
  #define SRAM_RESET_N Fld(1, 31, AC_MSKB3) //31
  //#define rev Fld(1, 27, AC_MSKB3) //27
  //#define rev Fld(1, 26, AC_MSKB3) //26
  //#define rev Fld(26, 0, AC_MSKDW) //0:25
#define VENC_DRAM_LARB_CFG0 0x3a100
  //#define ARB0AG7~1TIM Fld(28, 4, AC_MSKDW) //4:31
  #define ARB0AG0TIM Fld(4, 0, AC_MSKB0) //0:3
#define VENC_DRAM_LARB_CFG1 0x3a104
  //#define R_ARB0AG1~7EN Fld(7, 25, AC_MSKB3) //25:31
  #define R_ARB0AG0EN Fld(1, 24, AC_MSKB3) //24
  #define R_ARB0DMMONSEL Fld(5, 16, AC_MSKB2) //16:20
  #define R_ARB0_RST Fld(1, 12, AC_MSKB1) //12
  #define R_ARB0_TIMARBEN Fld(1, 8, AC_MSKB1) //8
  #define R_ARB0AGBMEND Fld(1, 6, AC_MSKB0) //6
  #define R_ARB0AGBMBEG Fld(2, 4, AC_MSKB0) //4:5
  #define R_ARB0AGBMAGID Fld(3, 0, AC_MSKB0) //0:2
#define VENC_DRAM_LARB_CFG2 0x3a108
  #define arb0a_cycle_counter Fld(32, 0, AC_MSKDW) //0:31
#define VENC_DRAM_LARB_CFG3 0x3a10c
  #define arb0a_length_counter Fld(32, 0, AC_MSKDW) //0:31
#define VENC_DRAM_LARB_CFG4 0x3a110
  #define arb0b_cycle_counter Fld(32, 0, AC_MSKDW) //0:31
#define VENC_DRAM_LARB_CFG5 0x3a114
  #define arb0b_length_counter Fld(32, 0, AC_MSKDW) //0:31
#define VENC_DRAM_LARB_CFG6 0x3a118
  #define arb0_dmmerge_mon Fld(32, 0, AC_MSKDW) //0:31
#define VENC_DRAM_LARB_CFG7 0x3a11c
  //#define rev Fld(16, 16, AC_FULLW32) //16:31
  #define arb0_dmagbmend Fld(8, 8, AC_FULLB1) //8:15
  #define arb0_dmagbmagid Fld(8, 0, AC_FULLB0) //0:7
#define UPDATE_BITSTREAM_BUF_DRAM_START_WRITE_ADDR 0x3a120
  #define UBSMSWA Fld(1, 31, AC_MSKB3) //31
  #define BSMSWA Fld(29, 0, AC_MSKDW) //0:28
#define BITSTREAM_BUF_DRAM_START_READ_ADDR 0x3a124
  #define BSMSRA Fld(29, 0, AC_MSKDW) //0:28
#define BITSTREAM_BUF_DRAM_CURRENT_WRITE_ADDR 0x3a128
  #define BSMCWA Fld(29, 0, AC_MSKDW) //0:28
#define VUI_PARAMETER_INFO_0 0x3a180
  #define EnableVUISupport Fld(1, 31, AC_MSKB3) //31
  #define VUI_aspect_ratio_info_present_flag Fld(1, 30, AC_MSKB3) //30
  #define VUI_aspect_ratio_idc Fld(8, 22, AC_MSKW32) //22:29
  #define VUI_overscan_info_present_flag Fld(1, 21, AC_MSKB2) //21
  #define VUI_overscan_appropriate_flag Fld(1, 20, AC_MSKB2) //20
  #define VUI_video_signal_type_present_flag Fld(1, 19, AC_MSKB2) //19
  #define VUI_video_format Fld(3, 16, AC_MSKB2) //16:18
  #define VUI_video_full_range_flag Fld(1, 15, AC_MSKB1) //15
  #define VUI_colour_description_present_flag Fld(1, 14, AC_MSKB1) //14
  #define VUI_chroma_location_info_present_flag Fld(1, 13, AC_MSKB1) //13
  #define VUI_chroma_sample_loc_type_top_field Fld(3, 10, AC_MSKB1) //10:12
  #define VUI_chroma_sample_loc_type_bottom_field Fld(3, 7, AC_MSKW10) //7:9
  #define VUI_timing_info_present_flag Fld(1, 6, AC_MSKB0) //6
  #define VUI_fixed_frame_rate_flag Fld(1, 5, AC_MSKB0) //5
  #define VUI_nal_hrd_parameters_present_flag Fld(1, 4, AC_MSKB0) //4
  #define VUI_vcl_hrd_parameters_present_flag Fld(1, 3, AC_MSKB0) //3
  #define VUI_low_delay_hrd_flag Fld(1, 2, AC_MSKB0) //2
  #define VUI_pic_struct_present_flag Fld(1, 1, AC_MSKB0) //1
  #define VUI_bitstream_restriction_flag Fld(1, 0, AC_MSKB0) //0
#define VUI_PARAMETER_INFO_1 0x3a184
  #define VUI_colour_primaries Fld(8, 24, AC_FULLB3) //24:31
  #define VUI_transfer_characteristics Fld(8, 16, AC_FULLB2) //16:23
  #define VUI_matrix_coefficients Fld(8, 8, AC_FULLB1) //8:15
#define VUI_PARAMETER_INFO_2 0x3a188
  #define VUI_sar_width Fld(16, 16, AC_FULLW32) //16:31
  #define VUI_sar_height Fld(16, 0, AC_FULLW10) //0:15
#define VUI_PARAMETER_INFO_3 0x3a18c
  #define VUI_num_units_in_tick Fld(32, 0, AC_MSKDW) //0:31
#define VUI_PARAMETER_INFO_4 0x3a190
  #define VUI_time_scale Fld(32, 0, AC_MSKDW) //0:31
#define VUI_PARAMETER_INFO_5 0x3a194
  #define VUI_nal_bit_rate_scale Fld(4, 25, AC_MSKB3) //25:28
  #define VUI_nal_cpb_size_scale Fld(4, 21, AC_MSKW32) //21:24
  #define VUI_nal_vbr_cbr_flag Fld(1, 20, AC_MSKB2) //20
  #define VUI_nal_initial_cpb_removal_delay_length_minus1 Fld(5, 15, AC_MSKW21) //15:19
  #define VUI_nal_cpb_removal_delay_length_minus1 Fld(5, 10, AC_MSKB1) //10:14
  #define VUI_nal_dpb_output_delay_length_minus1 Fld(5, 5, AC_MSKW10) //5:9
  #define VUI_nal_time_offset_length Fld(5, 0, AC_MSKB0) //0:4
#define VUI_PARAMETER_INFO_6 0x3a198
  #define VUI_nal_bit_rate_value_minus1 Fld(15, 16, AC_MSKW32) //16:30
  #define VUI_nal_cpb_size_value_minus1 Fld(15, 0, AC_MSKW10) //0:14
#define VUI_PARAMETER_INFO_7 0x3a19c
  #define VUI_vcl_bit_rate_scale Fld(4, 25, AC_MSKB3) //25:28
  #define VUI_vcl_cpb_size_scale Fld(4, 21, AC_MSKW32) //21:24
  #define VUI_vcl_vbr_cbr_flag Fld(1, 20, AC_MSKB2) //20
  #define VUI_vcl_initial_cpb_removal_delay_length_minus1 Fld(5, 15, AC_MSKW21) //15:19
  #define VUI_vcl_cpb_removal_delay_length_minus1 Fld(5, 10, AC_MSKB1) //10:14
  #define VUI_vcl_dpb_output_delay_length_minus1 Fld(5, 5, AC_MSKW10) //5:9
  #define VUI_vcl_time_offset_length Fld(5, 0, AC_MSKB0) //0:4
#define VUI_PARAMETER_INFO_8 0x3a1a0
  #define VUI_vcl_bit_rate_value_minus1 Fld(15, 16, AC_MSKW32) //16:30
  #define VUI_vcl_cpb_size_value_minus1 Fld(15, 0, AC_MSKW10) //0:14
#define VUI_PARAMETER_INFO_9 0x3a1a4
  #define VUI_motion_vectors_over_pic_boundaries_flag Fld(1, 30, AC_MSKB3) //30
  #define VUI_max_bytes_per_pic_denom Fld(5, 25, AC_MSKB3) //25:29
  #define VUI_max_bits_per_mb_denom Fld(5, 20, AC_MSKW32) //20:24
  #define VUI_log2_max_mv_length_vertical Fld(5, 15, AC_MSKW21) //15:19
  #define VUI_log2_max_mv_length_horizontal Fld(5, 10, AC_MSKB1) //10:14
  #define VUI_num_reorder_frames Fld(5, 5, AC_MSKW10) //5:9
  #define VUI_max_dec_frame_buffering Fld(5, 0, AC_MSKB0) //0:4
#define CRC_EC_BITSTREAM 0x3a1b0
  //#define CRC_EC_BITSTREAM Fld(32, 0, AC_MSKDW) //0:31
#define CRC_EC_STUFFING 0x3a1b4
  //#define CRC_EC_STUFFING Fld(32, 0, AC_MSKDW) //0:31
#define CRC_DPCM_CMB 0x3a1b8
  //#define CRC_CHROMA_CUR Fld(32, 0, AC_MSKDW) //0:31
#define CRC_FCS_REF 0x3a1bc
  //#define CRC_FCS_REF Fld(32, 0, AC_MSKDW) //0:31
#define CRC_FCS_CUR 0x3a1c0
  //#define CRC_FCS_CUR Fld(32, 0, AC_MSKDW) //0:31
#define CRC_IME_CUR 0x3a1c4
  //#define CRC_LUMA_CUR Fld(32, 0, AC_MSKDW) //0:31
#define CRC_CTRL 0x3a1c8
  #define CRC_ENABLE_CTRL Fld(1, 0, AC_MSKB0) //0
#define CRC_IME_REF 0x3a1cc
#define DLA_BYPASS_MODE 0x3a200
  #define Enable_last Fld(1, 3, AC_MSKB0) //3
  #define Bypass_Mode Fld(1, 2, AC_MSKB0) //2
  #define Disable_Frame_Done_Flush Fld(1, 1, AC_MSKB0) //1
  #define Wdle_FIFO_Empty_Detection_Enable Fld(1, 0, AC_MSKB0) //0
#define DLA_WADDR_FIFO_THR 0x3a204
  #define write_addr_fifo_threshold Fld(7, 24, AC_MSKB3) //24:30
  #define read_addr_fifo_threshold Fld(7, 16, AC_MSKB2) //16:22
  #define write_data_fifo_threshold Fld(7, 8, AC_MSKB1) //8:14
  #define read_data_fifo_threshold Fld(7, 0, AC_MSKB0) //0:6
#define DLA_TIME_OUT_W_THR 0x3a208
  #define write_addr_fifo_time_out_threhold Fld(5, 8, AC_MSKB1) //8:12
  #define read_addr_fifo_time_out_threshold Fld(5, 0, AC_MSKB0) //0:4
#define RP_WP_STATUS 0x3c838
  #define STALL Fld(1, 31, AC_MSKB3) //31
#endif // !VENC_H264REG_H


