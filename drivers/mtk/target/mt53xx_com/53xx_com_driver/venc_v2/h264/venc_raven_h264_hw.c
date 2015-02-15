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
 * $RCSfile: venc_raven_h264_hw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_raven_h264_hw.c
 *  Implementation of HW setting and Interrupt Service Routine of H.264 Encoder.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_debug.h"
#include "venc_raven_define.h"
#include "venc_raven_h264_reg.h"
#include "venc_raven_h264_hw.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define MUSTANG_CKGEN_BASE 0xD000
#define VENC_CKCFG (MUSTANG_CKGEN_BASE + 0x370) //mustang venc ckgen reg offset is 0x370
#define VENC_CK_SEL (1) // 1 --> usb_phy_pll_d2_ck 480/240M 2-->sawlesspll_d2_ck 432/216M  Mustang must use 1 as CLK input
#define VENC_CK_PD  (0x80) // 1 --> usb_phy_pll_d2_ck 480/240M 2-->sawlesspll_d2_ck 432/216M  Mustang must use 1 as CLK input

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
INT32 i4Venc_Hw_VENC_HW_MODE_SEL(VENC_HW_MODE_SEL_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, ENABLE_FRAME_UPDATE_RC, prParam->u1ENABLE_FRAME_UPDATE_RC);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, ENABLE_FRM_RC,          prParam->u1ENABLE_FRM_RC);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, CRC_CTRL_ENABLE,        prParam->u1CRC_CTRL_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, SAVE_RC_DATA,           prParam->u1SAVE_RC_DATA);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, LOAD_RC_DATA,           prParam->u1LOAD_RC_DATA);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, ENABLE_SEQ_RC,          prParam->u1ENABLE_SEQ_RC);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, LOAD_RC_RAM_CODE,       prParam->u1LOAD_RC_RAM_CODE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, INSERT_KEYFRAME,        prParam->u1insert_keyframe);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, FIRST_SPS_FLAG,         prParam->u1First_SPS_Flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, PAUSE_MODE,             prParam->u1PAUSE_MODE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, PHOTO_MODE,             prParam->u1PHOTO_MODE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, SMI_GULTRA_CTRL,        prParam->u4SMI_GULTRA_CTRL);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, EN_RATECONTROL,         prParam->u1EN_RATECONTROL);
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, VIDEO_FORMAT,           prParam->u1VIDEO_FORMAT);
    
    return 0;
}

VOID i4Venc_Hw_VENC_InsertKFrame(BOOL fgInsert)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_HW_MODE_SEL, INSERT_KEYFRAME, fgInsert);
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_0(VENC_ENCODER_INFO_0_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_0, QP_I_FRM,       prParam->u1QP_I_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_0, NUM_B_FRM,      prParam->u1NUM_B_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_0, PERIOD_I_FRM,   prParam->u4PERIOD_I_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_0, PERIOD_IDR_FRM, prParam->u4PERIOD_IDR_FRM);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_1(VENC_ENCODER_INFO_1_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, QP_P_FRM,   prParam->u1QP_P_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_SEARCH_V, (prParam->u1P_SEARCH_V) >> 4);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_SEARCH_H, (prParam->u1P_SEARCH_H) >> 4);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_16X16,    prParam->u1P_16X16);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_16X08,    prParam->u1P_16X08);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_08X16,    prParam->u1P_08X16);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_1, P_08X08,    prParam->u1P_08X08);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_2(VENC_ENCODER_INFO_2_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_INTERTR8MODE,  prParam->u1RDO_InterTr8Mode);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_INTERTR4MODE,  prParam->u1RDO_InterTr4Mode);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_INTRATR8MODE,  prParam->u1RDO_IntraTr8Mode);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_INTRATR4MODE,  prParam->u1RDO_IntraTr4Mode);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_INTRA16ENABLE, prParam->u1RDO_Intra16Enable);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_SKIPENABLE,    prParam->u1RDO_SkipEnable);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_2, RDO_BDIRECTENABLE, prParam->u1RDO_BDIRECTEnable);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_3(VENC_ENCODER_INFO_3_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, IME_REFINE_MODE,      prParam->u1IME_REFINE_MODE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, PFRM_NONREF_P,        prParam->u1PFRM_NONREF_P);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, CHANGE_B_2_NONREF_P,  prParam->u1CHANGE_B_2_NONREF_P);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, GEN_REC_FRM,          prParam->u1GEN_REC_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, COMV_SIGN,            prParam->u1COMV_SIGN);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, COMV_SCALER,          prParam->u1COMV_SCALER);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, INTRA_WEIGHT_COST_EN, prParam->u1INTRA_WEIGHT_COST_EN);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, COMVSEARCH_EN,        prParam->u1COMVSEARCH_EN);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, VENC_PDIR,            prParam->u1VENC_PDIR);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, IDR_PIC_ID,           prParam->u1IDR_PIC_ID);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, IS_IDR_FRM,           prParam->u1IS_IDR_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_3, VENC_IMG_TYPE,        prParam->u1VENC_IMG_TYPE);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_4(UINT32 u4VENC_FRAME_CNT)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_4, VENC_FRAME_CNT, u4VENC_FRAME_CNT);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_5(UINT32 u4VENC_FRAME_IDX)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_5, VENC_FRAME_IDX, u4VENC_FRAME_IDX);

    return 0;
}

INT32 i4Venc_Hw_VENC_ENCODER_INFO_6(UINT32 u4VENC_LAST_IDR_FRAME_IDX)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_ENCODER_INFO_6, VENC_LAST_IDR_FRAME_IDX, u4VENC_LAST_IDR_FRAME_IDX);

    return 0;
}

INT32 i4Venc_Hw_VENC_IMAGE_INFO_0(VENC_IMAGE_INFO_0_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_0, IMAGE_NUM_MB,       prParam->u2IMAGE_NUM_MB);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_0, IMAGE_HEIGHT_DIV16, (prParam->u4IMAGE_HEIGHT_DIV16) >> 4);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_0, IMAGE_WIDTH_DIV16,  (prParam->u4IMAGE_WIDTH_DIV16) >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_IMAGE_INFO_1(VENC_IMAGE_INFO_1_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_1, SOURCE_SCAN_TYPE,     prParam->u1source_scan_type);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_1, UV_PITCH_WIDTH_DIV16, (prParam->u4uv_pitch_width_div16) >> 4);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_1, PITCH_WIDTH_DIV16,    (prParam->u4pitch_width_div16) >> 4);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_1, IMAGE_CROP_RIGHT,     prParam->u1IMAGE_CROP_RIGHT);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IMAGE_INFO_1, IMAGE_CROP_BOTTOM,    prParam->u1IMAGE_CROP_BOTTOM);

    return 0;
}

INT32 i4Venc_Hw_VENC_H264_ENC_INFO_0(VENC_H264_ENC_INFO_0_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, CABAC,                      prParam->u1CABAC);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, PROFILE,                    prParam->u1PROFILE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, MBAFF,                      prParam->u1MBAFF);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, H264_LEVEL,                 prParam->u1H264_LEVEL);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, USE_TR8X8,                  prParam->u1USE_TR8X8);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, SLICE_ALPHA_C0_OFFSET_DIV2, (prParam->u4SLICE_ALPHA_C0_OFFSET_DIV2) >> 1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_H264_ENC_INFO_0, SLICE_BETA_C0_OFFSET_DIV2,  (prParam->u4SLICE_BETA_C0_OFFSET_DIV2) >> 1);

    return 0;
}

INT32 i4Venc_Hw_VENC_RATECONTROL_INFO_0(VENC_RATECONTROL_INFO_0_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_0, RC_CBR,             prParam->u1RC_CBR);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_0, RC_INI_QP,          prParam->u1RC_INI_QP);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_0, RC_TARGET_BIT_RATE, prParam->u4RC_TARGET_BIT_RATE);

    return 0;
}

INT32 i4Venc_Hw_VENC_RATECONTROL_INFO_1(VENC_RATECONTROL_INFO_1_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, ENABLE_EIS,  prParam->u1ENABLE_EIS);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, ENABLE_ROI,  prParam->u1ENABLE_ROI);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, AIFI,        prParam->u1AIFI);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, SKYPE_MODE,  prParam->u1SKYPE_MODE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, AFPS,        prParam->u1AFPS);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, ATBR,        prParam->u1ATBR);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, RC_FPS,      prParam->u1RC_FPS);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, BFRMQLIMTER, prParam->u1BfrmQLimter);
    IO_VENC_RAVEN_WRITE_BITS(VENC_RATECONTROL_INFO_1, PFRMQLIMTER, prParam->u1PfrmQLimter);

    return 0;
}

INT32 i4Venc_Hw_VENC_CODEC_CTRL(VENC_CODEC_CTRL_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_CODEC_CTRL, RELEASE_PAUSE_FRM, prParam->u1RELEASE_PAUSE_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_CODEC_CTRL, RELEASE_BS_DRAM,   prParam->u1RELEASE_BS_DRAM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_CODEC_CTRL, ENC_FRM,           prParam->u1ENC_FRM);
    IO_VENC_RAVEN_WRITE_BITS(VENC_CODEC_CTRL, ENC_PPS,           prParam->u1ENC_PPS);
    IO_VENC_RAVEN_WRITE_BITS(VENC_CODEC_CTRL, ENC_SPS,           prParam->u1ENC_SPS);

    return 0;
}

INT32 i4Venc_Hw_VENC_IRQ_STATUS(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_IRQ_STATUS));
}

INT32 i4Venc_Hw_VENC_IRQ_ACK(UINT32 u4Param)
{
    IO_VENC_RAVEN_WRITE_REG(VENC_IRQ_ACK, u4Param);

    return 0;
}

INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR(UINT32 u4BITSTREAM_BUF_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_BITSTREAM_BUF_DRAM_ADDR, BITSTREAM_BUF_DRAM_ADDR_DIV16, (u4BITSTREAM_BUF_DRAM_ADDR_DIV16 >> 4));

    return 0;
}

INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR_GET(VOID)
{
    UINT32 u4Addr = IO_VENC_RAVEN_READ_BITS(VENC_BITSTREAM_BUF_DRAM_ADDR, BITSTREAM_BUF_DRAM_ADDR_DIV16);

    return (u4Addr << 4);
}

INT32 i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_SIZE(UINT32 u4BITSTREAM_BUF_DRAM_SIZE_DIV128)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_BITSTREAM_BUF_DRAM_SIZE, BITSTREAM_BUF_DRAM_SIZE_DIV128, u4BITSTREAM_BUF_DRAM_SIZE_DIV128 >> 7);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_CUR_Y_DRAM_ADDR(UINT32 u4FRM_CURR_Y_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_CUR_Y_DRAM_ADDR, FRM_CURR_Y_DRAM_ADDR_DIV16, u4FRM_CURR_Y_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_CUR_UV_DRAM_ADDR(UINT32 u4FRM_CURR_UV_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_CUR_UV_DRAM_ADDR, FRM_CURR_UV_DRAM_ADDR_DIV16, u4FRM_CURR_UV_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_REF_Y_DRAM_ADDR(UINT32 u4FRM_REF_Y_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_REF_Y_DRAM_ADDR, FRM_REF_Y_DRAM_ADDR_DIV16, u4FRM_REF_Y_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_REF_UV_DRAM_ADDR(UINT32 u4FRM_REF_UV_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_REF_UV_DRAM_ADDR, FRM_REF_UV_DRAM_ADDR_DIV16, u4FRM_REF_UV_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_REC_Y_DRAM_ADDR(UINT32 u4FRM_REC_Y_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_REC_Y_DRAM_ADDR, FRM_REC_Y_DRAM_ADDR_DIV16, u4FRM_REC_Y_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_REC_UV_DRAM_ADDR(UINT32 u4FRM_REC_UV_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_REC_UV_DRAM_ADDR, FRM_REC_UV_DRAM_ADDR_DIV16, u4FRM_REC_UV_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_LOAD_COL_INFO_DRAM_ADDR(UINT32 u4LOAD_COL_INFO_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_LOAD_COL_INFO_DRAM_ADDR, LOAD_COL_INFO_DRAM_ADDR_DIV16, u4LOAD_COL_INFO_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_SAVE_COL_INFO_DRAM_ADDR(UINT32 u4SAVE_COL_INFO_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_SAVE_COL_INFO_DRAM_ADDR, SAVE_COL_INFO_DRAM_ADDR_DIV16, u4SAVE_COL_INFO_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_RC_CODE_DRAM_ADDR(UINT32 u4RC_CODE_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_RC_CODE_DRAM_ADDR, RC_CODE_DRAM_ADDR_DIV16, u4RC_CODE_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_RC_INFO_DRAM_ADDR(UINT32 u4RC_INFO_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_RC_INFO_DRAM_ADDR, RC_INFO_DRAM_ADDR_DIV16, u4RC_INFO_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_CLK_CG_CTRL(UINT32 u4CLK_CG_CTRL)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_CLK_CG_CTRL, CLK_CG_CTRL, u4CLK_CG_CTRL);

    return 0;
}

INT32 i4Venc_Hw_VENC_PIC_BITSTREAM_BYTE_CNT(VOID)
{
    return (IO_VENC_RAVEN_READ_BITS(VENC_PIC_BITSTREAM_BYTE_CNT, PIC_BITSTREAM_BYTE_CNT));
}

INT32 i4Venc_Hw_VENC_CODEC_CNT0(VOID)
{
    return (IO_VENC_RAVEN_READ_BITS(VENC_CODEC_CNT0, CODEC_COUNTER0));
}

INT32 i4Venc_Hw_VENC_IRQ_MODE_SEL(VENC_IRQ_MODE_SEL_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_IRQ_MODE_SEL, PAUSE_FRM_IRQ_ENABLE, prParam->u1PAUSE_FRM_IRQ_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IRQ_MODE_SEL, BS_DRAM_FULL_IRQ_ENABLE, prParam->u1BS_DRAM_FULL_IRQ_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IRQ_MODE_SEL, ENC_FRM_IRQ_ENABLE, prParam->u1ENC_FRM_IRQ_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IRQ_MODE_SEL, ENC_PPS_IRQ_ENABLE, prParam->u1ENC_PPS_IRQ_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_IRQ_MODE_SEL, ENC_SPS_IRQ_ENABLE, prParam->u1ENC_SPS_IRQ_ENABLE);

    return 0;
}

INT32 i4Venc_Hw_VENC_SW_HRST_N(VENC_SW_HRST_N_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_SW_HRST_N, SW_HRST_N, prParam->u1SW_HRST_N);

    return 0;
}

INT32 i4Venc_Hw_RD_VENC_SW_HRST_N(VOID)
{
    //IO_VENC_RAVEN_READ_BITS(VENC_SW_HRST_N, SW_HRST_N);

    //return 0;
    return(IO_VENC_RAVEN_READ_BITS(VENC_SW_HRST_N, SW_HRST_N));
}

INT32 i4Venc_Hw_VENC_CRC_BSDMA(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_BSDMA));
}

INT32 i4Venc_Hw_VENC_CRC_CUR_LUMA(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_CUR_LUMA));
}

INT32 i4Venc_Hw_VENC_CRC_REF_LUMA(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_REF_LUMA));
}

INT32 i4Venc_Hw_VENC_CRC_CUR_CHROMA(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_CUR_CHROMA));
}

INT32 i4Venc_Hw_VENC_CRC_REF_CHROMA(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_REF_CHROMA));
}

INT32 i4Venc_Hw_VENC_CRC_RD_COMV(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_RD_COMV));
}

INT32 i4Venc_Hw_VENC_CRC_SV_COMV(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_SV_COMV));
}

INT32 i4Venc_Hw_VENC_CRC_REC_FRM(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_REC_FRM));
}

INT32 i4Venc_Hw_VENC_CRC_BSDMA_1(VOID)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_CRC_BSDMA_1));
}

INT32 i4Venc_Hw_VENC_PIC_BITSTREAM_BYTE_CNT1(VENC_PIC_BITSTREAM_BYTE_CNT1_T* prParam)
{
    return (IO_VENC_RAVEN_READ_REG(VENC_PIC_BITSTREAM_BYTE_CNT1));
}

INT32 i4Venc_Hw_VENC_CE(VENC_CE_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_CE, CE, prParam->u1CE);

    return 0;
}

INT32 i4Venc_Hw_VENC_CLK_DCM_CTRL(VENC_CLK_DCM_CTRL_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_CLK_DCM_CTRL, CLK_DCM_CTRL, prParam->u1CLK_DCM_CTRL);

    return 0;
}

INT32 i4Venc_Hw_VENC_AVSR_ANRP_INFO_1(VENC_AVSR_ANRP_INFO_1_T* prParam)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_AVSR_ANRP_INFO_1, ANRP_ENABLE,       prParam->u1ANRP_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_AVSR_ANRP_INFO_1, AVSR_ENABLE,       prParam->u1AVSR_ENABLE);
    IO_VENC_RAVEN_WRITE_BITS(VENC_AVSR_ANRP_INFO_1, ANRP_NUM_PREV_NRP, prParam->u1ANRP_NUM_PREV_NRP);

    return 0;
}

INT32 i4Venc_Hw_VENC_FRM_CUR_V_DRAM_ADDR(UINT32 u4FRM_CURR_V_DRAM_ADDR_DIV16)
{
    IO_VENC_RAVEN_WRITE_BITS(VENC_FRM_CUR_V_DRAM_ADDR, FRM_CURR_V_DRAM_ADDR_DIV16, u4FRM_CURR_V_DRAM_ADDR_DIV16 >> 4);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0_ASPECT_RATIO_IDC(VENC_VUI_PARAMETER_INFO_0_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1VUI_aspect_ratio_info_present_flag) & 0x1) << 30) |
            (((prParam->u1VUI_aspect_ratio_idc) & 0xFF) << 22);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_0_ASPECT_RATIO_IDC 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_ASPECT_RATIO_INFO_PRESENT_FLAG_,      prParam->u1VUI_aspect_ratio_info_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_ASPECT_RATIO_IDC_,                    prParam->u1VUI_aspect_ratio_idc);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0(VENC_VUI_PARAMETER_INFO_0_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1EnableVUISupport) & 0x1) << 31) |
            (((prParam->u1VUI_aspect_ratio_info_present_flag) & 0x1) << 30) |
            (((prParam->u1VUI_aspect_ratio_idc) & 0xFF) << 22) |
            (((prParam->u1VUI_overscan_info_present_flag) & 0x1) << 21) |
            (((prParam->u1VUI_overscan_appropriate_flag) & 0x1) << 20) |
            (((prParam->u1VUI_video_signal_type_present_flag) & 0x1) << 19) |
            (((prParam->u1VUI_video_format) & 0x7) << 16) |
            (((prParam->u1VUI_video_full_range_flag) & 0x1) << 15) |
            (((prParam->u1VUI_colour_description_present_flag) & 0x1) << 14) |
            (((prParam->u1VUI_chroma_location_info_present_flag) & 0x1) << 13) |
            (((prParam->u1VUI_chroma_sample_loc_type_top_field) & 0x7) << 10) |
            (((prParam->u1VUI_chroma_sample_loc_type_bottom_field) & 0x7) << 7) |
            (((prParam->u1VUI_timing_info_present_flag) & 0x1) << 6) |
            (((prParam->u1VUI_fixed_frame_rate_flag) & 0x1) << 5) |
            (((prParam->u1VUI_nal_hrd_parameters_present_flag) & 0x1) << 4) |
            (((prParam->u1VUI_vcl_hrd_parameters_present_flag) & 0x1) << 3) |
            (((prParam->u1VUI_low_delay_hrd_flag) & 0x1) << 2) |
            (((prParam->u1VUI_pic_struct_present_flag) & 0x1) << 1) |
            (((prParam->u1VUI_bitstream_restriction_flag) & 0x1) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_0 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, ENABLEVUISUPPORT,                         prParam->u1EnableVUISupport);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_ASPECT_RATIO_INFO_PRESENT_FLAG_,      prParam->u1VUI_aspect_ratio_info_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_ASPECT_RATIO_IDC_,                    prParam->u1VUI_aspect_ratio_idc);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_OVERSCAN_INFO_PRESENT_FLAG_,          prParam->u1VUI_overscan_info_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_OVERSCAN_APPROPRIATE_FLAG_,           prParam->u1VUI_overscan_appropriate_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_VIDEO_SIGNAL_TYPE_PRESENT_FLAG_,      prParam->u1VUI_video_signal_type_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_VIDEO_FORMAT_,                        prParam->u1VUI_video_format);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_VIDEO_FULL_RANGE_FLAG_,               prParam->u1VUI_video_full_range_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_COLOUR_DESCRIPTION_PRESENT_FLAG_,     prParam->u1VUI_colour_description_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_CHROMA_LOCATION_INFO_PRESENT_FLAG_,   prParam->u1VUI_chroma_location_info_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_CHROMA_SAMPLE_LOC_TYPE_TOP_FIELD_,    prParam->u1VUI_chroma_sample_loc_type_top_field);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_CHROMA_SAMPLE_LOC_TYPE_BOTTOM_FIELD_, prParam->u1VUI_chroma_sample_loc_type_bottom_field);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_TIMING_INFO_PRESENT_FLAG_,            prParam->u1VUI_timing_info_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_FIXED_FRAME_RATE_FLAG_,               prParam->u1VUI_fixed_frame_rate_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_NAL_HRD_PARAMETERS_PRESENT_FLAG_,     prParam->u1VUI_nal_hrd_parameters_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_VCL_HRD_PARAMETERS_PRESENT_FLAG_,     prParam->u1VUI_vcl_hrd_parameters_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_LOW_DELAY_HRD_FLAG_,                  prParam->u1VUI_low_delay_hrd_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_PIC_STRUCT_PRESENT_FLAG_,             prParam->u1VUI_pic_struct_present_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_0, VUI_BITSTREAM_RESTRICTION_FLAG_,          prParam->u1VUI_bitstream_restriction_flag);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_1(VENC_VUI_PARAMETER_INFO_1_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1VUI_colour_primaries) & 0xFF) << 24) |
            (((prParam->u1VUI_transfer_characteristics) & 0xFF) << 16) |
            (((prParam->u1VUI_matrix_coefficients) & 0xFF) << 8);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_1 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_1, VUI_COLOUR_PRIMARIES_,         prParam->u1VUI_colour_primaries);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_1, VUI_TRANSFER_CHARACTERISTICS_, prParam->u1VUI_transfer_characteristics);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_1, VUI_MATRIX_COEFFICIENTS_,      prParam->u1VUI_matrix_coefficients);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_2(VENC_VUI_PARAMETER_INFO_2_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u4VUI_sar_width) & 0xFFFF) << 16) |
            (((prParam->u4VUI_sar_height) & 0xFFFF) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_2 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_2, VUI_SAR_WIDTH_,  prParam->u4VUI_sar_width);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_2, VUI_SAR_HEIGHT_, prParam->u4VUI_sar_height);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_3(UINT32 u4VUI_num_units_in_tick)
{
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_3 0x%x\n", u4VUI_num_units_in_tick);
    
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_3, VUI_NUM_UNITS_IN_TICK_,  u4VUI_num_units_in_tick);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_4(UINT32 u4VUI_time_scale)
{
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_4 0x%x\n", u4VUI_time_scale);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_4, VUI_TIME_SCALE_,  u4VUI_time_scale);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_5(VENC_VUI_PARAMETER_INFO_5_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1VUI_nal_bit_rate_scale) & 0xF) << 25) |
            (((prParam->u1VUI_nal_cpb_size_scale) & 0xF) << 21) |
            (((prParam->u1VUI_nal_vbr_cbr_flag) & 0x1) << 20) |
            (((prParam->u1VUI_nal_initial_cpb_removal_delay_length_minus1) & 0x1F) << 15) |
            (((prParam->u1VUI_nal_cpb_removal_delay_length_minus1) & 0x1F) << 10) |
            (((prParam->u1VUI_nal_dpb_output_delay_length_minus1) & 0x1F) << 5) |
            (((prParam->u1VUI_nal_time_offset_length) & 0x1F) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_5 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_BIT_RATE_SCALE_,                         prParam->u1VUI_nal_bit_rate_scale);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_CPB_SIZE_SCALE_,                         prParam->u1VUI_nal_cpb_size_scale);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_VBR_CBR_FLAG_,                           prParam->u1VUI_nal_vbr_cbr_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_INITIAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1, prParam->u1VUI_nal_initial_cpb_removal_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1,         prParam->u1VUI_nal_cpb_removal_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_DPB_OUTPUT_DELAY_LENGTH_MINUS1_,         prParam->u1VUI_nal_dpb_output_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_5, VUI_NAL_TIME_OFFSET_LENGTH_,                     prParam->u1VUI_nal_time_offset_length);

    return 0;
}


INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_6(VENC_VUI_PARAMETER_INFO_6_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u4VUI_nal_bit_rate_value_minus1) & 0x7FFF) << 16) |
            (((prParam->u4VUI_nal_cpb_size_value_minus1) & 0x7FFF) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_6 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_6, VUI_NAL_BIT_RATE_VALUE_MINUS1_,  prParam->u4VUI_nal_bit_rate_value_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_6, VUI_NAL_CPB_SIZE_VALUE_MINUS1_,  prParam->u4VUI_nal_cpb_size_value_minus1);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_7(VENC_VUI_PARAMETER_INFO_7_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1VUI_vcl_bit_rate_scale) & 0xF) << 25) |
            (((prParam->u1VUI_vcl_cpb_size_scale) & 0xF) << 21) |
            (((prParam->u1VUI_vcl_vbr_cbr_flag) & 0x1) << 20) |
            (((prParam->u1VUI_vcl_initial_cpb_removal_delay_length_minus1) & 0x1F) << 15) |
            (((prParam->u1VUI_vcl_cpb_removal_delay_length_minus1) & 0x1F) << 10) |
            (((prParam->u1VUI_vcl_dpb_output_delay_length_minus1) & 0x1F) << 5) |
            (((prParam->u1VUI_vcl_time_offset_length) & 0x1F) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_7 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_BIT_RATE_SCALE_,                         prParam->u1VUI_vcl_bit_rate_scale);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_CPB_SIZE_SCALE_,                         prParam->u1VUI_vcl_cpb_size_scale);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_VBR_CBR_FLAG_,                           prParam->u1VUI_vcl_vbr_cbr_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_INITIAL_CPB_REMOVAL_DELAY_LENGTH_MINUS1, prParam->u1VUI_vcl_initial_cpb_removal_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_CPB_REMOVAL_DELAY_LENGTH_MINUS1_,        prParam->u1VUI_vcl_cpb_removal_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_DPB_OUTPUT_DELAY_LENGTH_MINUS1_,         prParam->u1VUI_vcl_dpb_output_delay_length_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_7, VUI_VCL_TIME_OFFSET_LENGTH_,                     prParam->u1VUI_vcl_time_offset_length);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_8(VENC_VUI_PARAMETER_INFO_8_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u4VUI_vcl_bit_rate_value_minus1) & 0x7FFF) << 16) |
            (((prParam->u4VUI_vcl_cpb_size_value_minus1) & 0x7FFF) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_8 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_8, VUI_VCL_BIT_RATE_VALUE_MINUS1_,  prParam->u4VUI_vcl_bit_rate_value_minus1);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_8, VUI_VCL_CPB_SIZE_VALUE_MINUS1_,  prParam->u4VUI_vcl_cpb_size_value_minus1);

    return 0;
}

INT32 i4Venc_Hw_VENC_VUI_PARAMETER_INFO_9(VENC_VUI_PARAMETER_INFO_9_T* prParam)
{
    UINT32 u4Reg = 0;
    u4Reg = (((prParam->u1VUI_motion_vectors_over_pic_boundaries_flag) & 0x1) << 30) |
            (((prParam->u1VUI_max_bytes_per_pic_denom) & 0x1F) << 25) |
            (((prParam->u1VUI_max_bits_per_mb_denom) & 0x1F) << 20) |
            (((prParam->u1VUI_log2_max_mv_length_vertical) & 0x1F) << 15) |
            (((prParam->u1VUI_log2_max_mv_length_horizontal) & 0x1F) << 10) |
            (((prParam->u1VUI_num_reorder_frames) & 0x1F) << 5) |
            (((prParam->u1VUI_max_dec_frame_buffering) & 0x1F) << 0);
    LOG(7,"[raven] reg VENC_VUI_PARAMETER_INFO_9 0x%x\n", u4Reg);

    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_MOTION_VECTORS_OVER_PIC_BOUNDARIES_FLAG, prParam->u1VUI_motion_vectors_over_pic_boundaries_flag);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_MAX_BYTES_PER_PIC_DENOM_,                prParam->u1VUI_max_bytes_per_pic_denom);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_MAX_BITS_PER_MB_DENOM_,                  prParam->u1VUI_max_bits_per_mb_denom);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_LOG2_MAX_MV_LENGTH_VERTICAL_,            prParam->u1VUI_log2_max_mv_length_vertical);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_LOG2_MAX_MV_LENGTH_HORIZONTAL_,          prParam->u1VUI_log2_max_mv_length_horizontal);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_NUM_REORDER_FRAMES_,                     prParam->u1VUI_num_reorder_frames);
    IO_VENC_RAVEN_WRITE_BITS(VENC_VUI_PARAMETER_INFO_9, VUI_MAX_DEC_FRAME_BUFFERING_,                prParam->u1VUI_max_dec_frame_buffering);

    return 0;
}

INT32 i4Venc_Hw_VENC_CLK_SETTING(BOOL fgPowerOn)
{
    if (fgPowerOn)
    {
        IO_VENC_RAVEN_WRITE_REG(VENC_CKCFG, VENC_CK_SEL);  //CKGEN_BASE is 0xD000
        LOG(7,"[raven] PowerOn, clk setting: 0x%x\n",IO_VENC_RAVEN_READ_REG(VENC_CKCFG));
    }
    else
    {
        IO_VENC_RAVEN_WRITE_REG(VENC_CKCFG, VENC_CK_PD);  //CKGEN_BASE is 0xD000
        LOG(7,"[raven] PowerDown, clk setting: 0x%x\n",IO_VENC_RAVEN_READ_REG(VENC_CKCFG));
    }

    return 0;
}

#ifdef REG_DUMP
INT32 i4Venc_Hal_SeqLelRegDump(VOID)
{
    printf("========== sequence-level MMR setting ==========\n");
    printf("VENC_ENCODER_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_0));
    printf("VENC_ENCODER_INFO_1 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_1));
    printf("VENC_ENCODER_INFO_2 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_2));
    printf("VENC_IMAGE_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_IMAGE_INFO_0));
    printf("VENC_IMAGE_INFO_1 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_IMAGE_INFO_1));
    printf("VENC_H264_ENC_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_H264_ENC_INFO_0));
    printf("VENC_RATECONTROL_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_RATECONTROL_INFO_0));
    printf("VENC_IMAGE_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_RATECONTROL_INFO_1));   

    return 0;
}
INT32 i4Venc_Hal_SPSEncRegDump(VOID)
{
    printf("========== SPS Encoding ==========\n");
    printf("VENC_BITSTREAM_BUF_DRAM_SIZE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_SIZE));
    printf("VENC_BITSTREAM_BUF_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_ADDR));
    printf("VENC_HW_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_HW_MODE_SEL));
    printf("VENC_IRQ_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_IRQ_MODE_SEL));
    printf("VENC_CE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CE));
    printf("VENC_CODEC_CTRL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CODEC_CTRL));

    return 0;
}
INT32 i4Venc_Hal_PPSEncRegDump(VOID)
{
    printf("========== PPS Encoding ==========\n");
    printf("VENC_BITSTREAM_BUF_DRAM_SIZE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_SIZE));
    printf("VENC_BITSTREAM_BUF_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_ADDR));
    printf("VENC_HW_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_HW_MODE_SEL));
    printf("VENC_IRQ_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_IRQ_MODE_SEL));
    printf("VENC_CE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CE));
    printf("VENC_CODEC_CTRL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CODEC_CTRL));

    return 0;
}

INT32 i4Venc_Hal_FrmLelRegDump(UINT32 u4FrmIdx)
{
    printf("========== frame-level MMR setting(%d) ==========\n", u4FrmIdx);
    printf("VENC_ENCODER_INFO_3 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_3));
    printf("VENC_ENCODER_INFO_4 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_4));
    printf("VENC_ENCODER_INFO_5 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_5));
    printf("VENC_ENCODER_INFO_6 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_ENCODER_INFO_6));
    printf("VENC_RATECONTROL_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_RATECONTROL_INFO_0));
    printf("VENC_RATECONTROL_INFO_1 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_RATECONTROL_INFO_1));
    printf("VENC_AVSR_ANRP_INFO_0 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_AVSR_ANRP_INFO_0));
    printf("VENC_AVSR_ANRP_INFO_1 = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_AVSR_ANRP_INFO_1));
    printf("VENC_BITSTREAM_BUF_DRAM_SIZE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_SIZE));
    printf("VENC_BITSTREAM_BUF_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_BITSTREAM_BUF_DRAM_ADDR));
    printf("VENC_FRM_CUR_Y_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_CUR_Y_DRAM_ADDR));
    printf("VENC_FRM_CUR_UV_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_CUR_UV_DRAM_ADDR));
    printf("VENC_FRM_REF_Y_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_REF_Y_DRAM_ADDR));
    printf("VENC_FRM_REF_UV_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_REF_UV_DRAM_ADDR));
    printf("VENC_FRM_REC_Y_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_REC_Y_DRAM_ADDR));
    printf("VENC_FRM_REC_UV_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_FRM_REC_UV_DRAM_ADDR));
    printf("VENC_LOAD_COL_INFO_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_LOAD_COL_INFO_DRAM_ADDR));
    printf("VENC_SAVE_COL_INFO_DRAM_ADDR = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_SAVE_COL_INFO_DRAM_ADDR));
    printf("VENC_IRQ_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_IRQ_MODE_SEL));
    printf("VENC_HW_MODE_SEL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_HW_MODE_SEL));
    printf("VENC_CE = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CE));
    printf("VENC_CODEC_CTRL = 0x%x\n", IO_VENC_RAVEN_READ_REG(VENC_CODEC_CTRL));  

    return 0;
}
#endif

