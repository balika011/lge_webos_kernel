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
 * $RCSfile: venc_raven_h264_hal.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_raven_h264_hal.h
 *  
 */


#ifndef VENC_RAVEN_H264_HAL_H
#define VENC_RAVEN_H264_HAL_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_typedef.h"

#include "venc_raven_h264_hw.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define VENC_RAVEN_ENC_SPS  1
#define VENC_RAVEN_ENC_PPS  2
#define VENC_RAVEN_ENC_FRM  3
#define VENC_RAVEN_ENC_RELEASE_BS_DRAM  4

#define VENC_RAVEN_OK       0

#define VENC_INVALID_B      0xff
#define VENC_INVALID_W      0xffff
#define VENC_INVALID_DW     0xffffffff

#define VENC_H264_HW_CYCLE (240000000)
// ME configuration
// BME order: DIRECT, 16x16, 16x08, 08x16, 08x08
#define VENC_H264_BME11111 (2400)
#define VENC_H264_BME11101 (2150)
#define VENC_H264_BME11001 (2000)
#define VENC_H264_BME11000 (1850)

#define VIDEOCODEC_HAL_DEF_DRAM_PAGE_SIZE                (1024)

#define _HAL_DBG(fmt...)     LOG(6, fmt)
#define _HAL_FUNC_BEGIN()    _HAL_DBG("_VENC<HAL>::func Begin:%s\n", __FUNCTION__)
#define _HAL_FUNC_END(ret)   _HAL_DBG("_VEHC<HAL>::func end:  %s, ret:%d\n", __FUNCTION__, ret)
#define _HAL_FUNC_ENTER()    _HAL_DBG("_VENC<HAL>::func Enter:%s\n", __FUNCTION__)
#define _HAL_FUNC_LEAVE()    _HAL_DBG("_VENC<HAL>::func Leave:%s\n", __FUNCTION__)

// ======== Rate Control Rom Code definition ========
#if 1
#define RC_ROM_CODE_FILE "rcpu_h264_normal_2014_07_03_for_5890_maxq36_ext.hex"

#else
#define RC_ROM_CODE_FILE "rcpu_h264_normal_MT5399_ext.txt"
#define RC_ROM_CODE_FILE "rcpu_h264_normal_MT5399_ori.txt"
#define RC_ROM_CODE_FILE "rcpu_h264_normal_MT8135_20130412_ext.txt"
#define RC_ROM_CODE_FILE "raven_rcpu_code.txt"
#endif
static const UINT32 _u4DefaultRCRomCode[] =
{
#include RC_ROM_CODE_FILE
};
static const UINT32 _u4RCRomCodeLen = sizeof(_u4DefaultRCRomCode)/sizeof(_u4DefaultRCRomCode[0]);
#define UCODE_LENGTH_ADD8 _u4RCRomCodeLen


#define H264_MMR_EMU \
    UINT32 mmr_vui_aspect_ratio_info_present_flag;\
    UINT32 mmr_vui_aspect_ratio_idc;\
    UINT32 mmr_vui_sar_width;\
    UINT32 mmr_vui_sar_height;\
    UINT32 mmr_vui_overscan_info_present_flag;\
    UINT32 mmr_vui_overscan_appropriate_flag;\
    UINT32 mmr_vui_video_signal_type_present_flag;\
    UINT32 mmr_vui_video_format;\
    UINT32 mmr_vui_video_full_range_flag;\
    UINT32 mmr_vui_colour_description_present_flag;\
    UINT32 mmr_vui_colour_primaries;\
    UINT32 mmr_vui_transfer_characteristics;\
    UINT32 mmr_vui_matrix_coefficients;\
    UINT32 mmr_vui_chroma_location_info_present_flag;\
    UINT32 mmr_vui_chroma_sample_loc_type_top_field;\
    UINT32 mmr_vui_chroma_sample_loc_type_bottom_field;\
    UINT32 mmr_vui_timing_info_present_flag;\
    UINT32 mmr_vui_num_units_in_tick;\
    UINT32 mmr_vui_time_scale;\
    UINT32 mmr_vui_fixed_frame_rate_flag;\
    UINT32 mmr_vui_nal_hrd_parameters_present_flag;\
    UINT32 mmr_vui_nal_bit_rate_scale;\
    UINT32 mmr_vui_nal_cpb_size_scale;\
    UINT32 mmr_vui_nal_bit_rate_value_minus1;\
    UINT32 mmr_vui_nal_cpb_size_value_minus1;\
    UINT32 mmr_vui_nal_vbr_cbr_flag;\
    UINT32 mmr_vui_nal_initial_cpb_removal_delay_length_minus1;\
    UINT32 mmr_vui_nal_cpb_removal_delay_length_minus1;\
    UINT32 mmr_vui_nal_dpb_output_delay_length_minus1;\
    UINT32 mmr_vui_nal_time_offset_length;\
    UINT32 mmr_vui_vcl_hrd_parameters_present_flag;\
    UINT32 mmr_vui_vcl_bit_rate_scale;\
    UINT32 mmr_vui_vcl_cpb_size_scale;\
    UINT32 mmr_vui_vcl_bit_rate_value_minus1;\
    UINT32 mmr_vui_vcl_cpb_size_value_minus1;\
    UINT32 mmr_vui_vcl_vbr_cbr_flag;\
    UINT32 mmr_vui_vcl_initial_cpb_removal_delay_length_minus1;\
    UINT32 mmr_vui_vcl_cpb_removal_delay_length_minus1;\
    UINT32 mmr_vui_vcl_dpb_output_delay_length_minus1;\
    UINT32 mmr_vui_vcl_time_offset_length;\
    UINT32 mmr_vui_low_delay_hrd_flag;\
    UINT32 mmr_vui_pic_struct_present_flag;\
    UINT32 mmr_vui_bitstream_restriction_flag;\
    UINT32 mmr_vui_motion_vectors_over_pic_boundaries_flag;\
    UINT32 mmr_vui_max_bytes_per_pic_denom;\
    UINT32 mmr_vui_max_bits_per_mb_denom;\
    UINT32 mmr_vui_log2_max_mv_length_vertical;\
    UINT32 mmr_vui_log2_max_mv_length_horizontal;\
    UINT32 mmr_vui_num_reorder_frames;\
    UINT32 mmr_vui_max_dec_frame_buffering;

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct
{
    UINT32 u4EncFrmAddr;
    UINT32 u4EncFrmSize;
    UINT32 u4EncStuffSize;

    UINT32 u4EncBitstCrc;
    UINT32 u4EncStuffCrc;
    UINT32 u4EncDpcmCrc;
    UINT32 u4EncIMECurCrc;

    UINT32 u4GoldenFrmAddr;
    UINT32 u4GoldenFrmSize;
    UINT32 u4GoldenStuffSize;

    UINT32 u4GoldenBitstCrc;
    UINT32 u4GoldenStuffCrc;
    UINT32 u4GoldenDpcmCrc;
    UINT32 u4GoldenIMECurCrc;
} VENC_RESULT_T;

typedef struct
{
    // MMR variables start
    UINT32 mmr_period_i_frm_10; 
    UINT32 mmr_period_idr_frm_10; 
    UINT32 mmr_num_of_b_frm_2; 
    UINT32 mmr_bitstream_format_1;
    UINT32 mmr_h264_profile_8;
    UINT32 mmr_h264_level_6;
    UINT32 mmr_dram_page_size_div256_7;
    UINT32 mmr_gen_b_rec_frm_1;
    UINT32 mmr_use_copy_b_1;
    UINT32 mmr_use_mbaff_1;
    UINT32 mmr_use_cabac_1;
    UINT32 mmr_quant_i_frm_6;
    UINT32 mmr_quant_p_frm_6;
    UINT32 mmr_quant_b_frm_6;
    UINT32 mmr_en_p_interskip_1;
    UINT32 mmr_en_p_inter16x16_1;
    UINT32 mmr_en_p_inter16x08_1;
    UINT32 mmr_en_p_inter08x16_1;
    UINT32 mmr_en_p_inter08x08_1;
    UINT32 mmr_en_b_interdirect_1;
    UINT32 mmr_en_b_inter16x16_1;
    UINT32 mmr_en_b_inter16x08_1;
    UINT32 mmr_en_b_inter08x16_1;
    UINT32 mmr_en_b_inter08x08_1;
    UINT32 mmr_p_sr_h_div16_3;
    UINT32 mmr_p_sr_v_div16_2;
    UINT32 mmr_b_sr_h_div16_2;
    UINT32 mmr_b_sr_v_div16_2;
    UINT32 mmr_use_coarse_search_1;
    UINT32 mmr_p_coarse_sr_h_div8_3;
    UINT32 mmr_p_coarse_sr_v_div8_2;
    UINT32 mmr_b_coarse_sr_h_div8_2;
    UINT32 mmr_b_coarse_sr_v_div8_2;
    UINT32 mmr_use_rc_1;
    UINT32 mmr_rc_use_cbr_1;
    UINT32 mmr_rc_use_init_qp_1;
    UINT32 mmr_rc_target_bit_rate_14;
    UINT32 mmr_fps_8;
    UINT32 mmr_p_frm_q_limiter_8;
    UINT32 mmr_b_frm_q_limiter_8;

    UINT32 mmr_source_scan_type_2;
    UINT32 mmr_pitch_width_in_pels;

    UINT32 mmr_image_crop_right_4;
    UINT32 mmr_image_crop_bottom_5;
    UINT32 mmr_image_width_div16_7;
    UINT32 mmr_image_height_div16_6;
    UINT32 mmr_num_mb_in_frm_12;
    UINT32 mmr_coarse_image_crop_bottom_3;
    UINT32 mmr_coarse_image_width_div8_6;
    UINT32 mmr_coarse_image_height_div8_5;
    UINT32 mmr_num_blk_in_coarse_frm_10;
    UINT32 mmr_ini_bitstream_dram_byte_div8_26;
    UINT32 mmr_max_bitstream_byte_size_29;
    UINT32 mmr_max_pic_bitstream_byte_cnt_22;
    //    UINT32 mmr_frm_buf_y_dram_byte_addr_div8_26 [8];
    //    UINT32 mmr_frm_buf_uv_dram_byte_addr_div8_26 [8];
    UINT32 mmr_upper_mb_info_dram_byte_addr_div8_26;
    UINT32 mmr_col_info_dram_byte_addr_div8_26;
    //    UINT32 mmr_coarse_frm_buf_y_dram_byte_addr_div8_26 [7];
    UINT32 mmr_coarse_multiplier_7;

    UINT32 mmr_crc_check_enable_1;
    UINT32 mmr_enable_vui_support_1;
    
    UINT32 mmr_rc_adaptive_fps;
    UINT32 mmr_rc_adaptive_tbr;
    UINT32 use_stuff_byte;
#ifdef VENC_NEW_EMU
    H264_MMR_EMU
    UINT32 dwNumOfIandPFrames;
    UINT32 mmr_end;
#endif
    UINT32 mmr_avsr_enable;

    UINT32 mmr_photo_mode;
    UINT32 mmr_refine_ment_mode;    //1
    UINT32 mmr_rdO_interTr8_mode;
    UINT32 mmr_rdO_interTr4_mode;
    UINT32 mmr_rdO_intra8x8_mode;
    UINT32 mmr_rdO_intra4x4_mode;
    UINT32 mmr_rdO_intra16_enable;
    UINT32 mmr_rdO_skip_enable;
    UINT32 mmr_rdO_bdirect_enable;
    UINT32 mmr_use_8x8Tr;
    UINT32 mmr_slice_alpha_c0_offset_div2;
    UINT32 mmr_slice_beta_offset_div2;
    UINT32 mmr_skype_mode;
    // MMR variables end

    //coded picture type and display type begin
    BOOL write_pic_timing;
    UINT8 vui_pic_struct_present_flag;
    UINT8 picture_display_type;
    //coded picture type and display type end
} VENC_RAVEN_H246_MMR_PARAM;


/// VEnc Result Info Structure
typedef struct
{
    UINT32 u4BitstreamStartAddr;                               //New Result Start Addr
    UINT32 u4BitstreamByteCnt;                                  //New Result Data Length
} VENC_RESULT_INFO_T;

typedef struct
{
    VENC_ENCODER_INFO_0_T       rVENC_ENCODER_INFO_0;
    VENC_ENCODER_INFO_1_T       rVENC_ENCODER_INFO_1;
    VENC_ENCODER_INFO_2_T       rVENC_ENCODER_INFO_2;
    VENC_IMAGE_INFO_0_T         rVENC_IMAGE_INFO_0;
    VENC_IMAGE_INFO_1_T         rVENC_IMAGE_INFO_1;
    VENC_H264_ENC_INFO_0_T      rVENC_H264_ENC_INFO_0;
    VENC_RATECONTROL_INFO_0_T   rVENC_RATECONTROL_INFO_0;
    VENC_RATECONTROL_INFO_1_T   rVENC_RATECONTROL_INFO_1;
    VENC_AVSR_ANRP_INFO_1_T     rVENC_AVSR_ANRP_INFO_1;
} VENC_RAVEN_SEQ_MMR_PARAM_T;

typedef struct
{
    VENC_ENCODER_INFO_3_T       rVENC_ENCODER_INFO_3;
    UINT32                      u4FrmCnt;
    UINT32                      u4FrmIdx;
    UINT32                      u4LastIDRFrmIdx;

    VENC_RATECONTROL_INFO_0_T   rVENC_RATECONTROL_INFO_0;
    VENC_RATECONTROL_INFO_1_T   rVENC_RATECONTROL_INFO_1;

    UINT32                      u4FrmCurYAddr;
    UINT32                      u4FrmCurCAddr;
    UINT32                      u4FrmCurVAddr;
    UINT32                      u4FrmRefYAddr;
    UINT32                      u4FrmRefCAddr;
    UINT32                      u4FrmRecYAddr;
    UINT32                      u4FrmRecCAddr;

    UINT32                      u4ColLoadAddr;
    UINT32                      u4ColSaveAddr;

    UINT32                      u4RcInfoAddr;
} VENC_RAVEN_FRM_MMR_PARAM_T;

typedef struct
{
    VENC_HW_MODE_SEL_T          rVENC_HW_MODE_SEL;

    UINT32                      u4OutputBufAddr;
    UINT32                      u4OutputBufSize;

    BOOL                        fgOutputBufFull;
} VENC_RAVEN_ENC_PARAM_T;

typedef struct
{
    UINT32              u4BsCrc;        //3A0BC VENC_CRC_BSDMA
    UINT32              u4CurLumaCrc;   //3A0C0 VENC_CRC_CUR_LUMA
    UINT32              u4RefLumaCrc;   //3A0C4 VENC_CRC_REF_LUMA
    UINT32              u4CurChromaCrc; //3A0C8 VENC_CRC_CUR_CHROMA
    UINT32              u4RefChromaCrc; //3A0CC VENC_CRC_REF_CHROMA
    UINT32              u4RdComvCrc;    //3A0D0 VENC_CRC_RD_COMV
    UINT32              u4SvComvCrc;    //3A0D4 VENC_CRC_SV_COMV
    UINT32              u4RecFrmCrc;    //3A0D8 VENC_CRC_REC_FRM
} VENC_RAVEN_CRC_T;

typedef struct
{
    VENC_VUI_PARAMETER_INFO_0_T     rVENC_VUI_PARAMETER_INFO_0_T;
    VENC_VUI_PARAMETER_INFO_1_T     rVENC_VUI_PARAMETER_INFO_1_T;
    VENC_VUI_PARAMETER_INFO_2_T     rVENC_VUI_PARAMETER_INFO_2_T;
    VENC_VUI_PARAMETER_INFO_5_T     rVENC_VUI_PARAMETER_INFO_5_T;
    VENC_VUI_PARAMETER_INFO_6_T     rVENC_VUI_PARAMETER_INFO_6_T;
    VENC_VUI_PARAMETER_INFO_7_T     rVENC_VUI_PARAMETER_INFO_7_T;
    VENC_VUI_PARAMETER_INFO_8_T     rVENC_VUI_PARAMETER_INFO_8_T;
    VENC_VUI_PARAMETER_INFO_9_T     rVENC_VUI_PARAMETER_INFO_9_T;

    UINT32                          u4VUI_num_units_in_tick;
    UINT32                          u4VUI_time_scale;
} VENC_RAVEN_VUI_PARAM_T;

//extern INT32 Printf(const CHAR *format, ...);

extern INT32 i4Venc_Hal_Init(void);

extern INT32 i4Venc_Hal_Uninit(void);

extern INT32 i4Venc_Hal_Stop(void);

extern INT32 i4Venc_Hal_Power(BOOL fgPowerOn);

extern INT32 i4Venc_Hal_HwReset(void);

extern INT32 i4Venc_Hal_SetRamCodeBuf(UINT32 u4RamCodeAddr);

extern INT32 i4Venc_Hal_SetRCInfoBuf(UINT32 u4Addr);

extern INT32 i4Venc_Hal_SetOutputBuf(UINT32 u4BufAddr, UINT32 u4BufSize);

extern INT32 i4Venc_Hal_Enc(UINT32 u4EncType, VENC_RAVEN_ENC_PARAM_T* prParam);

extern INT32 i4Venc_Hal_SeqMmrSet(VENC_RAVEN_SEQ_MMR_PARAM_T* prParam);

extern INT32 i4Venc_Hal_FrmMmrSet(VENC_RAVEN_FRM_MMR_PARAM_T* prParam);

extern INT32 i4Venc_Hal_VUISet(VENC_RAVEN_VUI_PARAM_T* prVUI);

extern INT32 i4Venc_Hal_GetResult(VENC_RESULT_INFO_T* pResultInfo);

extern INT32 i4Venc_Hal_GetCrc(VENC_RAVEN_CRC_T* prCrc);

extern INT32 i4Venc_Hal_Start(VOID);

extern INT32 i4Venc_Hal_SetAspectRatioIdc(VENC_RAVEN_VUI_PARAM_T* prVUI);

#endif //VENC_RAVEN_H264_HAL_H
