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
 * $RCSfile: venc_h264api.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264api.h
 *  This header file declares exported APIs of H.264.
 */


#ifndef VENC_H264API_H
#define VENC_H264API_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "fbm_drvif.h"
#include "../venc_debug.h"
#include "venc_util_drvif.h"
#include "venc_drvif.h"
#include "venc_if.h"

#include "venc_raven_h264_hal.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

// Macro VENC_H264_FILL_STUFF_BYTES_BY_FW
// is used for rate control fitting target bitrate via adding stuff bytes
// if inaccurate bitrate is allowed, mark this macro for better performance
//#define VENC_H264_FILL_STUFF_BYTES_BY_FW


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define VENC_H264_MAX_VIDEO_RESOLUTION_WIDTH (1920)
#define VENC_H264_MAX_VIDEO_RESOLUTION_HEIGHT (1080)

#define VENC_H264_MAX_VIDEO_RESOLUTION (VENC_H264_MAX_VIDEO_RESOLUTION_WIDTH * VENC_H264_MAX_VIDEO_RESOLUTION_HEIGHT)

#define VENC_H264_BS_SIZE (0x2F7600) //1920*1080*1.5

#define VENC_H264_MAX_COARSE_RES        (58880)  // 1280/4 * 736/4


#define _fgIsValidPic(type) ((type) >= VENC_H264_PIC_TYPE_SPS && (type) <= VENC_H264_PIC_TYPE_B)

#define _fgIsRealPic(type) \
    (VENC_H264_PIC_TYPE_IDR == (type) \
    || VENC_H264_PIC_TYPE_I == (type) \
    || VENC_H264_PIC_TYPE_P == (type) \
    || VENC_H264_PIC_TYPE_B == (type))


#define H264_BFRAME_MAX  (2)

#define H264_EMU_PARM (VENC_H264 + 0xFF)

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

#define VENC_RAVEN_SRCBUF_CNT     (2)
#ifdef __MODEL_slt__
#define VENC_RAVEN_BSBUF_CNT      1
#else
#define VENC_RAVEN_BSBUF_CNT      3
#endif

typedef struct
{
    UINT32              u4OutputBuf[VENC_RAVEN_BSBUF_CNT];
    UINT32              u4RamCodeBuf;
    UINT32              u4RCInfoBuf;
    UINT32              au4SrcBufY[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4SrcBufC[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4SrcBufV[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4ColMvInfoBuf[2];
    
    UINT32              u4InputBufY; // input buffer is maintained by front module
    UINT32              u4InputBufC; // so, we do not alloc input buf here. Just alloc these three variables.
    UINT32              u4InputBufV;
} VENC_RAVEN_HW_BUF_T;

typedef struct
{
    VENC_OUT_PIC_MEMBERS

    //EMU
    UINT32 u4SPSAddr;
    UINT32 u4SPSSize;
    UINT32 u4PPSAddr;
    UINT32 u4PPSSize;
    UINT32 u4FRMAddr;
    UINT32 u4FRMSize;
} H264_EMU_OUT_PIC_T;


typedef enum
{
    H264_REZ_STATE_IDLE,
    H264_REZ_STATE_REZ,
    H264_REZ_STATE_MAX
} H264_REZ_STATE_T;

typedef struct _H246_MMR_PARAM
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
    UINT32 mmr_vui_timing_info_present_flag;
    UINT32 mmr_vui_aspect_ratio_info_present_flag;
    UINT32 mmr_vui_aspect_ratio_idc;
    UINT32 mmr_vui_sar_width;
    UINT32 mmr_vui_sar_height;
    UINT32 mmr_vui_num_units_in_tick;
    UINT32 mmr_vui_time_scale;

    UINT32 mmr_rc_adaptive_fps;
    UINT32 mmr_rc_adaptive_tbr;
    UINT32 use_stuff_byte;

    UINT32 mmr_rdO_bdirect_enable;
    UINT32 mmr_rdO_interTr4_mode;
    UINT32 mmr_rdO_interTr8_mode;
    UINT32 mmr_rdO_intra16_enable;
    UINT32 mmr_rdO_intra4x4_mode;
    UINT32 mmr_rdO_intra8x8_mode;
    UINT32 mmr_rdO_skip_enable;
    UINT32 mmr_slice_alpha_c0_offset_div2;
    UINT32 mmr_slice_beta_offset_div2;
    UINT32 mmr_use_8x8Tr;
    UINT32 mmr_skype_mode;
    UINT32 mmr_avsr_enable;
    UINT32 mmr_photo_mode;
    
    UINT32 mmr_refine_ment_mode;
    
    // MMR variables end

    //coded picture type and display type begin
    BOOL write_pic_timing;
    UINT8 vui_pic_struct_present_flag;
    UINT8 picture_display_type;
    //coded picture type and display type end
}H264_MMR_PARAM_T;

typedef struct
{
    UINT32 u4CosAddr;
}H246_PIC_PRAM;



typedef struct _H246_CTRL
{
    //Coarse
    H264_REZ_STATE_T eRezFrmState;
    VENC_STATE_T     eRezCmdState;
    HANDLE_T    hRezThread;
    HANDLE_T    hRezCmdQ;
    HANDLE_T    hRezFrmQ;
    UINT16      u2SrcWidth;
    UINT16      u2SrcHeidht;
    UINT16      u2DesWidth;
    UINT16      u2DesHeidht;
    UINT16      u2DesPitch;
    BOOL        fgCoarse;
    VENC_REZ_OBJ_T *ptRezObj;
    VENC_BUFMGR_BASE_T *ptCoarseBuffMgr;

    //encode flow
    UINT32      u4FrameNo;
    BOOL        fg1stHeader;
    BOOL        fgSkipHeaderOnce;
    BOOL        fgKeyframe;
    UINT8       u1RepeatHeader;
    BOOL        fgWriteSEI;
    BOOL        fgForceIDR;
    BOOL        fgReqAddAud;
	BOOL        fgSecure;
    VENC_H264_PIC_TYPE eType;
    UINT8       u1BFrameNo;
    UINT8       u1BFrmIdx;
    UINT32     u4RecordSize;
    UINT32     u4RecordAddr;

    VENC_IN_PIC_T  uaIn[H264_BFRAME_MAX];
    VENC_OUT_PIC_T uaOut[H264_BFRAME_MAX];
    H246_PIC_PRAM  uaPram[H264_BFRAME_MAX];

    UINT8 u1BSeqIdx;
    UINT8 u1RefSeqIdx;

    UINT32 u4FrefCSAddr;
    UINT32 u4BrefCSAddr;

    UINT16 u2IDRFrmPeriod;
    UINT16 u2IDRFrmIdx;

    UINT16 u2IFrmPeriod;
    UINT16 u2IFrmIdx;

    UINT32 u4RCUAddr;
    UINT32 u4RCUCodeSize;

    //working buffer
    UINT32 u4CFrmBuffAddr[7];
    UINT32 u4YFrmBuffAddr[2];
    UINT32 u4UVFrmBuffAddr[2];

    UINT32 u4InitWp;
    UINT32 u4BsSa;
    UINT32 u4BsSz;
    UINT32 u4BsEa;
    UINT32 u4NaluBufSa;
    UINT32 u4NaluBufSz;
    UINT32 u4NaluBufEa;
    UINT32 u4NaluBufWp;
    UINT32 u4AudNaluBufSa;
    UINT32 u4AudNaluBufSz;
    UINT32 u4AudNaluBufEa;
    UINT32 u4AudNaluBufWp;
    UINT32 u4RomCodeAddr;
    UINT32 u4UpperInfoMBAddr;
    UINT32 u4ColInfoAddr;
    UINT32 u4CSInfoAddr;
    UINT32 u4NextCpSa;
    UINT32 u4Rp;
    UINT32 u4DumpInfoAddr;
    UINT32 u4ThumbnailYuv420;
    UINT32 u4ThumbnailARGB;

    BOOL   fgEMUDirectMode;

    VENC_RAVEN_HW_BUF_T rHwBuf;
    VENC_RAVEN_CRC_T    rCrc;
    VENC_BS_BUF_STATUS rBsBufStatus[VENC_RAVEN_BSBUF_CNT];
    
    //debug info
    UINT32 u4SPScrc;
    UINT32 u4PPScrc;
    UINT32 u4FRMcrc;
} H264_CTRL_T;


//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------
VENC_OBJECT_T *H264Open(VOID);


#endif // VDEC_H264API_H


