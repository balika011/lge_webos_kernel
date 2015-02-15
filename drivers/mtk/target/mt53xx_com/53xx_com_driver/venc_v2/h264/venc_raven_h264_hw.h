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
 * $RCSfile: venc_raven_h264_hw.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_raven_h264_hw.h
 *  This header file declares HW setting of H.264.
 */


#ifndef VENC_RAVEN_H264_HW_H
#define VENC_RAVEN_H264_HW_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_typedef.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//#define REG_DUMP  //dump register

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct
{
    UINT8       u1ENABLE_FRAME_UPDATE_RC;
    UINT8       u1ENABLE_FRM_RC;
    UINT8       u1CRC_CTRL_ENABLE;
    UINT8       u1SAVE_RC_DATA;
    UINT8       u1LOAD_RC_DATA;
    UINT8       u1ENABLE_SEQ_RC;
    UINT8       u1LOAD_RC_RAM_CODE;
    UINT8       u1insert_keyframe;
    UINT8       u1First_SPS_Flag;
    UINT8       u1PAUSE_MODE;
    UINT8       u1PHOTO_MODE;
    UINT32      u4SMI_GULTRA_CTRL;
    UINT8       u1EN_RATECONTROL;
    UINT8       u1VIDEO_FORMAT;
} VENC_HW_MODE_SEL_T;     //3A000

typedef struct
{
    UINT8       u1QP_I_FRM;
    UINT8       u1NUM_B_FRM;
    UINT32      u4PERIOD_I_FRM;
    UINT32      u4PERIOD_IDR_FRM;
} VENC_ENCODER_INFO_0_T;  //3A004

typedef struct
{
    UINT8       u1QP_P_FRM;
    UINT8       u1P_SEARCH_V;
    UINT8       u1P_SEARCH_H;
    UINT8       u1P_16X16;
    UINT8       u1P_16X08;
    UINT8       u1P_08X16;
    UINT8       u1P_08X08;
} VENC_ENCODER_INFO_1_T;  //3A008

typedef struct
{
    UINT8       u1RDO_InterTr8Mode;
    UINT8       u1RDO_InterTr4Mode;
    UINT8       u1RDO_IntraTr8Mode;
    UINT8       u1RDO_IntraTr4Mode;
    UINT8       u1RDO_Intra16Enable;
    UINT8       u1RDO_SkipEnable;
    UINT8       u1RDO_BDIRECTEnable;
} VENC_ENCODER_INFO_2_T;  //3A00C

typedef struct
{
    UINT8       u1IME_REFINE_MODE;
    UINT8       u1PFRM_NONREF_P;
    UINT8       u1CHANGE_B_2_NONREF_P;
    UINT8       u1GEN_REC_FRM;
    UINT8       u1COMV_SIGN;
    UINT8       u1COMV_SCALER;
    UINT8       u1INTRA_WEIGHT_COST_EN;
    UINT8       u1COMVSEARCH_EN;
    UINT8       u1VENC_PDIR;
    UINT8       u1IDR_PIC_ID;
    UINT8       u1IS_IDR_FRM;
    UINT8       u1VENC_IMG_TYPE;
} VENC_ENCODER_INFO_3_T;  //3A010

/*
typedef struct
{
    UINT32      u4VENC_FRAME_CNT;
} VENC_ENCODER_INFO_4_T;  //3A014

typedef struct
{
    UINT32      u4VENC_FRAME_IDX;
} VENC_ENCODER_INFO_5_T;  //3A018

typedef struct
{
    UINT32      u4VENC_LAST_IDR_FRAME_IDX;
} VENC_ENCODER_INFO_6_T;  //3A01C
*/

//3A020 nothing

typedef struct
{
    UINT16      u2IMAGE_NUM_MB;
    UINT32      u4IMAGE_HEIGHT_DIV16;
    UINT32      u4IMAGE_WIDTH_DIV16;
} VENC_IMAGE_INFO_0_T;    //3A024

typedef struct
{
    UINT8       u1source_scan_type;
    UINT32      u4uv_pitch_width_div16;
    UINT32      u4pitch_width_div16;
    UINT8       u1IMAGE_CROP_RIGHT;
    UINT8       u1IMAGE_CROP_BOTTOM;
} VENC_IMAGE_INFO_1_T;    //3A028

//3A02C nothing

typedef struct
{
    UINT8       u1CABAC;
    UINT8       u1PROFILE;
    UINT8       u1MBAFF;
    UINT8       u1H264_LEVEL;
    UINT8       u1USE_TR8X8;
    UINT32      u4SLICE_ALPHA_C0_OFFSET_DIV2;
    UINT32      u4SLICE_BETA_C0_OFFSET_DIV2;
} VENC_H264_ENC_INFO_0_T; //3A030

//3A034 nothing

typedef struct
{
    UINT8       u1RC_CBR;
    UINT8       u1RC_INI_QP;
    UINT32      u4RC_TARGET_BIT_RATE;
} VENC_RATECONTROL_INFO_0_T;  //3A048

typedef struct
{
    UINT8       u1ENABLE_EIS;
    UINT8       u1ENABLE_ROI;
    UINT8       u1AIFI;
    UINT8       u1SKYPE_MODE;
    UINT8       u1AFPS;
    UINT8       u1ATBR;
    UINT8       u1RC_FPS;
    UINT8       u1BfrmQLimter;
    UINT8       u1PfrmQLimter;
} VENC_RATECONTROL_INFO_1_T;  //3A04C

//3A050 nothing

//3A054 nothing

typedef struct
{
    UINT8       u1RELEASE_PAUSE_FRM;
    UINT8       u1RELEASE_BS_DRAM;
    UINT8       u1ENC_FRM;
    UINT8       u1ENC_PPS;
    UINT8       u1ENC_SPS;
} VENC_CODEC_CTRL_T;  //3A058

typedef struct
{
    UINT8       u1PAUSE_FRM_INT;
    UINT8       u1BS_DRAM_FULL_INT;
    UINT8       u1ENC_FRM_INT;
    UINT8       u1ENC_PPS_INT;
    UINT8       u1ENC_SPS_INT;
} VENC_IRQ_STATUS_T;  //3A05C

typedef struct
{
    UINT8       u1PAUSE_FRM_ACK;
    UINT8       u1BS_DRAM_FULL_ACK;
    UINT8       u1ENC_FRM_ACK;
    UINT8       u1ENC_PPS_ACK;
    UINT8       u1ENC_SPS_ACK;
} VENC_IRQ_ACK_T; //3A060

/*
typedef struct
{
    UINT32      u4BITSTREAM_BUF_DRAM_ADDR_DIV16;
} VENC_BITSTREAM_BUF_DRAM_ADDR_T; //3A064

typedef struct
{
    UINT32      u4BITSTREAM_BUF_DRAM_SIZE_DIV128;
} VENC_BITSTREAM_BUF_DRAM_SIZE_T; //3A068

typedef struct
{
    UINT32      u4FRM_CURR_Y_DRAM_ADDR_DIV16;
} VENC_FRM_CUR_Y_DRAM_ADDR_T; //3A06C

typedef struct
{
    UINT32      u4FRM_CURR_UV_DRAM_ADDR_DIV16;
} VENC_FRM_CUR_UV_DRAM_ADDR_T;    //3A070

typedef struct
{
    UINT32      u4FRM_REF_Y_DRAM_ADDR_DIV16;
} VENC_FRM_REF_Y_DRAM_ADDR_T; //3A074

typedef struct
{
    UINT32      u4FRM_REF_UV_DRAM_ADDR_DIV16;
} VENC_FRM_REF_UV_DRAM_ADDR_T;    //3A078

typedef struct
{
    UINT32      u4FRM_REC_Y_DRAM_ADDR_DIV16;
} VENC_FRM_REC_Y_DRAM_ADDR_T;     //3A07C

typedef struct
{
    UINT32      u4FRM_REC_UV_DRAM_ADDR_DIV16;
} VENC_FRM_REC_UV_DRAM_ADDR_T;    //3A080

typedef struct
{
    UINT32      u4LOAD_COL_INFO_DRAM_ADDR_DIV16;
} VENC_LOAD_COL_INFO_DRAM_ADDR_T; //3A084

typedef struct
{
    UINT32      u4SAVE_COL_INFO_DRAM_ADDR_DIV16;
} VENC_SAVE_COL_INFO_DRAM_ADDR_T; //3A088

typedef struct
{
    UINT32      u4RC_CODE_DRAM_ADDR_DIV16;
} VENC_RC_CODE_DRAM_ADDR_T;   //3A08C

typedef struct
{
    UINT32      u4RC_INFO_DRAM_ADDR_DIV16;
} VENC_RC_INFO_DRAM_ADDR_T;   //3A090

typedef struct
{
    UINT32      u4CLK_CG_CTRL;
} VENC_CLK_CG_CTRL_T;     //3A094

typedef struct
{
    UINT32      u4PIC_BITSTREAM_BYTE_CNT;
} VENC_PIC_BITSTREAM_BYTE_CNT_T;  //3A098

typedef struct
{
    UINT32      u4CODEC_COUNTER0;
} VENC_CODEC_CNT0_T;  //3A09C
*/

typedef struct
{
    UINT8       u1PAUSE_FRM_IRQ_ENABLE;
    UINT8       u1BS_DRAM_FULL_IRQ_ENABLE;
    UINT8       u1ENC_FRM_IRQ_ENABLE;
    UINT8       u1ENC_PPS_IRQ_ENABLE;
    UINT8       u1ENC_SPS_IRQ_ENABLE;
} VENC_IRQ_MODE_SEL_T;    //3A0A4

typedef struct
{
    UINT8       u1SW_HRST_N;
} VENC_SW_HRST_N_T;   //3A0A8

typedef struct
{
    UINT8       u1SW_PAUSE;
} VENC_SW_PAUSE_T;    //3A0AC

typedef struct
{
    UINT32      u4PAUSE_MB_CNT;
} VENC_PAUSE_MODE_INFO_T;     //3A0B0

typedef struct
{
    UINT32      u4CRC_EC_BSDMA;
} VENC_CRC_BSDMA_T;   //3A0BC

typedef struct
{
    UINT32      u4CRC_IFME_CUR_LUMA;
} VENC_CRC_CUR_LUMA_T;    //3A0C0

typedef struct
{
    UINT32      u4CRC_IFME_REF_LUMA;
} VENC_CRC_REF_LUMA_T;    //3A0C4

typedef struct
{
    UINT32      u4CRC_IMRR_CUR_CHROMA;
} VENC_CRC_CUR_CHROMA_T;  //3A0C8

typedef struct
{
    UINT32      u4CRC_IMRR_REF_CHROMA;
} VENC_CRC_REF_CHROMA_T;  //3A0CC

typedef struct
{
    UINT32      u4CRC_IFME_RD_COMV;
} VENC_CRC_RD_COMV_T;     //3A0D0

typedef struct
{
    UINT32      u4CRC_IMRR_SV_COMV;
} VENC_CRC_SV_COMV_T;     //3A0D4

typedef struct
{
    UINT32      u4CRC_DB_REC_FRM;
} VENC_CRC_REC_FRM_T;     //3A0D8

typedef struct
{
    UINT32      u4CRC_EC_BSDMA_1;
} VENC_CRC_BSDMA_1_T;     //3A0DC

typedef struct
{
    UINT32      u4VENC_PIC_BITSTREAM_BYTE_CNT1;
} VENC_PIC_BITSTREAM_BYTE_CNT1_T; //3A0E8

typedef struct
{
    UINT8       u1CE;
} VENC_CE_T;  //3A0EC

typedef struct
{
    UINT8       u1CLK_DCM_CTRL;
} VENC_CLK_DCM_CTRL_T;    //3A0F4

typedef struct
{
    UINT8       u1ANRP_ENABLE;
    UINT8       u1AVSR_ENABLE;
    UINT8       u1ANRP_NUM_PREV_NRP;
} VENC_AVSR_ANRP_INFO_1_T;    //3A128

typedef struct
{
    UINT8       u1EnableVUISupport;
    UINT8       u1VUI_aspect_ratio_info_present_flag;
    UINT8       u1VUI_aspect_ratio_idc;
    UINT8       u1VUI_overscan_info_present_flag;
    UINT8       u1VUI_overscan_appropriate_flag;
    UINT8       u1VUI_video_signal_type_present_flag;
    UINT8       u1VUI_video_format;
    UINT8       u1VUI_video_full_range_flag;
    UINT8       u1VUI_colour_description_present_flag;
    UINT8       u1VUI_chroma_location_info_present_flag;
    UINT8       u1VUI_chroma_sample_loc_type_top_field;
    UINT8       u1VUI_chroma_sample_loc_type_bottom_field;
    UINT8       u1VUI_timing_info_present_flag;
    UINT8       u1VUI_fixed_frame_rate_flag;
    UINT8       u1VUI_nal_hrd_parameters_present_flag;
    UINT8       u1VUI_vcl_hrd_parameters_present_flag;
    UINT8       u1VUI_low_delay_hrd_flag;
    UINT8       u1VUI_pic_struct_present_flag;
    UINT8       u1VUI_bitstream_restriction_flag;
} VENC_VUI_PARAMETER_INFO_0_T;    //3A180

typedef struct
{
    UINT8       u1VUI_colour_primaries;
    UINT8       u1VUI_transfer_characteristics;
    UINT8       u1VUI_matrix_coefficients;
} VENC_VUI_PARAMETER_INFO_1_T;    //3A184

typedef struct
{
    UINT32      u4VUI_sar_width;
    UINT32      u4VUI_sar_height;
} VENC_VUI_PARAMETER_INFO_2_T;    //3A188

typedef struct
{
    UINT32      u4VUI_num_units_in_tick;
} VENC_VUI_PARAMETER_INFO_3_T;    //3A18C

typedef struct
{
    UINT32      u4VUI_time_scale;
} VENC_VUI_PARAMETER_INFO_4_T;    //3A190

typedef struct
{
    UINT8       u1VUI_nal_bit_rate_scale;
    UINT8       u1VUI_nal_cpb_size_scale;
    UINT8       u1VUI_nal_vbr_cbr_flag;
    UINT8       u1VUI_nal_initial_cpb_removal_delay_length_minus1;
    UINT8       u1VUI_nal_cpb_removal_delay_length_minus1;
    UINT8       u1VUI_nal_dpb_output_delay_length_minus1;
    UINT8       u1VUI_nal_time_offset_length;
} VENC_VUI_PARAMETER_INFO_5_T;    //3A194

typedef struct
{
    UINT32      u4VUI_nal_bit_rate_value_minus1;
    UINT32      u4VUI_nal_cpb_size_value_minus1;
} VENC_VUI_PARAMETER_INFO_6_T;    //3A198

typedef struct
{
    UINT8       u1VUI_vcl_bit_rate_scale;
    UINT8       u1VUI_vcl_cpb_size_scale;
    UINT8       u1VUI_vcl_vbr_cbr_flag;
    UINT8       u1VUI_vcl_initial_cpb_removal_delay_length_minus1;
    UINT8       u1VUI_vcl_cpb_removal_delay_length_minus1;
    UINT8       u1VUI_vcl_dpb_output_delay_length_minus1;
    UINT8       u1VUI_vcl_time_offset_length;
} VENC_VUI_PARAMETER_INFO_7_T;    //3A19C

typedef struct
{
    UINT32      u4VUI_vcl_bit_rate_value_minus1;
    UINT32      u4VUI_vcl_cpb_size_value_minus1;
} VENC_VUI_PARAMETER_INFO_8_T;    //3A1A0

typedef struct
{
    UINT8       u1VUI_motion_vectors_over_pic_boundaries_flag;
    UINT8       u1VUI_max_bytes_per_pic_denom;
    UINT8       u1VUI_max_bits_per_mb_denom;
    UINT8       u1VUI_log2_max_mv_length_vertical ;
    UINT8       u1VUI_log2_max_mv_length_horizontal;
    UINT8       u1VUI_num_reorder_frames;
    UINT8       u1VUI_max_dec_frame_buffering;
} VENC_VUI_PARAMETER_INFO_9_T;    //3A1A4


//-----------------------------------------------------------------------------
// Prototype of public functions
//-----------------------------------------------------------------------------
extern INT32 i4Venc_Hw_VENC_HW_MODE_SEL(VENC_HW_MODE_SEL_T* prParam);

extern VOID i4Venc_Hw_VENC_InsertKFrame(BOOL fgInsert);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_0(VENC_ENCODER_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_1(VENC_ENCODER_INFO_1_T* prParam);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_2(VENC_ENCODER_INFO_2_T* prParam);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_3(VENC_ENCODER_INFO_3_T* prParam);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_4(UINT32 u4VENC_FRAME_CNT);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_5(UINT32 u4VENC_FRAME_IDX);

extern INT32 i4Venc_Hw_VENC_ENCODER_INFO_6(UINT32 u4VENC_LAST_IDR_FRAME_IDX);

extern INT32 i4Venc_Hw_VENC_IMAGE_INFO_0(VENC_IMAGE_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_IMAGE_INFO_1(VENC_IMAGE_INFO_1_T* prParam);

extern INT32 i4Venc_Hw_VENC_H264_ENC_INFO_0(VENC_H264_ENC_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_RATECONTROL_INFO_0(VENC_RATECONTROL_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_RATECONTROL_INFO_1(VENC_RATECONTROL_INFO_1_T* prParam);

extern INT32 i4Venc_Hw_VENC_CODEC_CTRL(VENC_CODEC_CTRL_T* prParam);

extern INT32 i4Venc_Hw_VENC_IRQ_STATUS(VOID);

extern INT32 i4Venc_Hw_VENC_IRQ_ACK(UINT32 u4Param);

extern INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR(UINT32 u4BITSTREAM_BUF_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR_GET(VOID);

extern INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_SIZE(UINT32 u4BITSTREAM_BUF_DRAM_SIZE_DIV128);

extern INT32 i4Venc_Hw_VENC_FRM_CUR_Y_DRAM_ADDR(UINT32 u4FRM_CURR_Y_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_FRM_CUR_UV_DRAM_ADDR(UINT32 u4FRM_CURR_UV_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_FRM_REF_Y_DRAM_ADDR(UINT32 u4FRM_REF_Y_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_FRM_REF_UV_DRAM_ADDR(UINT32 u4FRM_REF_UV_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_FRM_REC_Y_DRAM_ADDR(UINT32 u4FRM_REC_Y_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_FRM_REC_UV_DRAM_ADDR(UINT32 u4FRM_REC_UV_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_LOAD_COL_INFO_DRAM_ADDR(UINT32 u4LOAD_COL_INFO_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_SAVE_COL_INFO_DRAM_ADDR(UINT32 u4SAVE_COL_INFO_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_RC_CODE_DRAM_ADDR(UINT32 u4RC_CODE_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_RC_INFO_DRAM_ADDR(UINT32 u4RC_INFO_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_CLK_CG_CTRL(UINT32 u4CLK_CG_CTRL);

extern INT32 i4Venc_Hw_VENC_PIC_BITSTREAM_BYTE_CNT(VOID);

extern INT32 i4Venc_Hw_VENC_CODEC_CNT0(VOID);

extern INT32 i4Venc_Hw_VENC_IRQ_MODE_SEL(VENC_IRQ_MODE_SEL_T* prParam);

extern INT32 i4Venc_Hw_VENC_SW_HRST_N(VENC_SW_HRST_N_T* prParam);

extern INT32 i4Venc_Hw_RD_VENC_SW_HRST_N(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_BSDMA(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_CUR_LUMA(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_REF_LUMA(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_CUR_CHROMA(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_REF_CHROMA(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_RD_COMV(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_SV_COMV(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_REC_FRM(VOID);

extern INT32 i4Venc_Hw_VENC_CRC_BSDMA_1(VOID);

extern INT32 i4Venc_Hw_VENC_PIC_BITSTREAM_BYTE_CNT1(VENC_PIC_BITSTREAM_BYTE_CNT1_T* prParam);

extern INT32 i4Venc_Hw_VENC_CE(VENC_CE_T* prParam);

extern INT32 i4Venc_Hw_VENC_CLK_DCM_CTRL(VENC_CLK_DCM_CTRL_T* prParam);

extern INT32 i4Venc_Hw_VENC_AVSR_ANRP_INFO_1(VENC_AVSR_ANRP_INFO_1_T* prParam);

extern INT32 i4Venc_Hw_VENC_FRM_CUR_V_DRAM_ADDR(UINT32 u4FRM_CURR_V_DRAM_ADDR_DIV16);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0_ASPECT_RATIO_IDC(VENC_VUI_PARAMETER_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0(VENC_VUI_PARAMETER_INFO_0_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_1(VENC_VUI_PARAMETER_INFO_1_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_2(VENC_VUI_PARAMETER_INFO_2_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_3(UINT32 u4VUI_num_units_in_tick);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_4(UINT32 u4VUI_time_scale);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_5(VENC_VUI_PARAMETER_INFO_5_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_6(VENC_VUI_PARAMETER_INFO_6_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_7(VENC_VUI_PARAMETER_INFO_7_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_8(VENC_VUI_PARAMETER_INFO_8_T* prParam);

extern INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_9(VENC_VUI_PARAMETER_INFO_9_T* prParam);
extern INT32 i4Venc_Hw_VENC_CLK_SETTING(BOOL fgPowerOn);

#ifdef REG_DUMP
extern INT32 i4Venc_Hal_SeqLelRegDump(VOID);
extern INT32 i4Venc_Hal_SPSEncRegDump(VOID);
extern INT32 i4Venc_Hal_PPSEncRegDump(VOID);
extern INT32 i4Venc_Hal_FrmLelRegDump(UINT32 u4FrmIdx);
#endif

#endif //VENC_RAVEN_H264_HW_H

