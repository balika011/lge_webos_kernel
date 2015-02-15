/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/

#include "x_os.h"                       //For <string.h>
#include "stdio.h"                     //Should support on Verify/Emulation Buil

#include "x_lint.h"
#include "x_common.h"
#include "x_typedef.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_bim.h"
#include "x_printf.h"
#include "x_timer.h"
#include "drv_common.h"

#include "venc_raven_define.h"
#include "venc_raven_h264_hal.h"

#include "venc_raven_h264_verify.h"


#define VENC_OUTPUT_FULL_TEST   1

#if VENC_OUTPUT_FULL_TEST
    #define VENC_PART_OUTPUTBUF_SIZE   128
#endif

#define VENC_ALIGN_MASK(VAL, MSK) (((VAL) + (MSK)) & (~(MSK)))

#define VENC_MAX_W  1920
#define VENC_MAX_H  1080
#define VENC_OUTPUTBUF_SIZE     (VENC_MAX_W * VENC_MAX_H * 3 / 2 * 3)

#define VENC_SPS_SIZE   288
#define VENC_PPS_SIZE   288

#define VENC_RAVEN_BUF_ADD  (1024*1024)

#define VENC_RAVEN_ALIGN_SIZE           32

#define VENC_RAVEN_MAX_WIDTH            1920
#define VENC_RAVEN_MAX_HEIGHT           1080

#define VENC_RAVEN_ALIGN_WIDTH          (VENC_RAVEN_MAX_WIDTH + VENC_RAVEN_ALIGN_SIZE - (VENC_RAVEN_MAX_WIDTH % VENC_RAVEN_ALIGN_SIZE))
#define VENC_RAVEN_ALIGN_HEIGHT         (VENC_RAVEN_MAX_HEIGHT + VENC_RAVEN_ALIGN_SIZE - (VENC_RAVEN_MAX_HEIGHT % VENC_RAVEN_ALIGN_SIZE))

#define VENC_RAVEN_SRC_Y_BUF_SIZE       (VENC_RAVEN_ALIGN_WIDTH * VENC_RAVEN_ALIGN_HEIGHT + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_SRC_C_BUF_SIZE       (VENC_RAVEN_ALIGN_WIDTH * VENC_RAVEN_ALIGN_HEIGHT / 2 + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_COL_BUF_SIZE         (VENC_RAVEN_ALIGN_WIDTH / 16 * VENC_RAVEN_ALIGN_HEIGHT / 16 * 4 + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_OUTPUT_BUF_SIZE      (VENC_OUTPUTBUF_SIZE  + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_RAMCODE_BUF_SIZE     (1024 * 16 + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_RCINFO_BUF_SIZE      (1024 + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_CONVERT_BUF_SIZE     (VENC_RAVEN_ALIGN_WIDTH * VENC_RAVEN_ALIGN_HEIGHT * 3)
#define VENC_RAVEN_MMR_BUF_SIZE         (2 * VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_SPS_BUF_SIZE         (VENC_SPS_SIZE + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_PPS_BUF_SIZE         (VENC_PPS_SIZE + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_GOLDEN_BUF_SIZE      (10 * VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_CRC_BUF_SIZE         (2 * VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_LIST_BUF_SIZE        (1 * VENC_RAVEN_BUF_ADD)

#define VENC_VFY_EVENT_SRCBUF   1

UINT32 _u4FreeSrcBufCnt = 0;
VENC_RAVEN_VFY_INFO_T* _prVencVfyInfo = NULL;

UINT32 _u4CompareErr = 0;

char _acVfyListFileName[512] = {"D:\\raven\\raven_list.txt\0"};

char _acRCFileKey[512] =     {"\\test_vec\\raven_rcpu_code.txt\0"};
char _acLumaFileKey[512] =   {"\\test_vec\\raven_luma_cur_frm_0000.txt\0"};
char _acChromaFileKey[512] = {"\\test_vec\\raven_chroma_cur_frm_0000.txt\0"};
char _acChromaVFileKey[512] = {"\\test_vec\\raven_chroma_cur_v_frm_0000.txt\0"};
char _acSPSFileKey[512] =    {"\\test_vec\\raven_test_vec_sps.txt\0"};
char _acPPSFileKey[512] =    {"\\test_vec\\raven_test_vec_pps.txt\0"};
char _acMMRFileKey[512] =    {"\\raven_test_vec_mmr.txt\0"};
char _acGoldenFileKey[512] = {"\\test_bit2.264\0"};
char _acCrcFileKey[512] =    {"\\raven_crc.txt\0"};

char _acRCFileName[512] =     {"D:\\raven\\try\\test_vec\\raven_rcpu_code.txt\0"};
char _acLumaFileName[512] =   {"D:\\raven\\try\\test_vec\\raven_luma_cur_frm_0000.txt\0"};
char _acChromaFileName[512] = {"D:\\raven\\try\\test_vec\\raven_chroma_cur_frm_0000.txt\0"};
char _acChromaVFileName[512] = {"D:\\raven\\try\\test_vec\\raven_chroma_cur_v_frm_0000.txt\0"};
char _acSPSFileName[512] =    {"D:\\raven\\try\\test_vec\\raven_test_vec_sps.txt\0"};
char _acPPSFileName[512] =    {"D:\\raven\\try\\test_vec\\raven_test_vec_pps.txt\0"};
char _acMMRFileName[512] =    {"D:\\raven\\try\\raven_test_vec_mmr.txt\0"};
char _acGoldenFileName[512] = {"D:\\raven\\try\\test_bit2.264\0"};
char _acCrcFileName[512] =    {"D:\\raven\\try\\raven_crc.txt\0"};

char _acEncType[3][10] = {"SPS\0", "PPS\0", "FRAME\0"};

extern int     rand(void);

externC void HalFlushDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
externC void HalInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
externC void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);

extern BOOL _VEncLoadFile(CHAR *pcFileName, UINT32 u4Addr);

void vVEnc_Assert(void)
{
#if 0
    ASSERT(0);
#else
    Printf("[raven] ASSERT\n");
    while(1);
#endif
}

UINT32 u4Venc_MemAlloc(UINT32 u4MemSize, UINT32 u4AlignSize)
{
    UINT32 u4Addr = 0;
    u4Addr = BSP_AllocAlignedDmaMemory(u4MemSize, u4AlignSize);

    if (0 == u4Addr)
    {
        ASSERT(0);
    }

    return (u4Addr);
}

void vVenc_GetMmrfile(CHAR *pBuf, UINT32 *addr, UINT32 *u4Val)
{
    UINT32 offset;
    offset = *addr;
    while(!(0x0D == *(char *)(pBuf+ offset) && 0x0A == *(char *)(pBuf+ offset+1)) && (!(0x0A == *(char *)(pBuf+ offset))) )
    {
        offset ++;
    }
    if(0x0A == *(char *)(pBuf+offset))
    {
        offset ++;//*addr = *addr + 1;
    }
    else
    {
        offset +=2;//*addr= *addr + 2;
    }
    x_sscanf(pBuf+offset, "%x", u4Val);

    *addr = offset;
}

VOID vVenc_LoadMmr(CHAR *pBuf, VENC_RAVEN_H246_MMR_PARAM *ptMMR)
{
    UINT32 addr, u4Val;

    addr = 0;
    // MMR
    x_sscanf (pBuf+addr, "%x", &u4Val);
    //sscanf(pBuf+addr, "%x", &u4Val);

    // g_dwBitstreamFormat
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_bitstream_format_1);
    //Printf("%x\n", ptMMR->mmr_bitstream_format_1);

    // bUseRateControl
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_use_rc_1);
    //Printf("mmr_use_rc_1 %x\n", ptMMR->mmr_use_rc_1);

    // g_rc_stuffing_en
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->use_stuff_byte);
    //Printf("use_stuff_byte %x\n", ptMMR->use_stuff_byte);

    // g_bPhotoMode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_photo_mode);
    //Printf("mmr_photo_mode %x\n", ptMMR->mmr_photo_mode);

    // g_dwPeriodOfIDRFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_period_idr_frm_10);
    //Printf("== %x\n", ptMMR->mmr_period_idr_frm_10);

    // g_dwPeriodOfIFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_period_i_frm_10);
    //Printf("== %x\n", ptMMR->mmr_period_i_frm_10);

    // g_dwNumOfBFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_num_of_b_frm_2);
    //Printf("== %x\n", ptMMR->mmr_num_of_b_frm_2);

    // g_dwNumOfIandPFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->dwNumOfIandPFrames);
    //Printf("== %x\n", ptMMR->dwNumOfIandPFrames);

    //---- MMR inputs, QP ----//
    // g_dwQuantIFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_quant_i_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_i_frm_6);

    // g_dwQuantPFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_quant_p_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_p_frm_6);

    // g_dwQuantBFrames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_quant_b_frm_6);
    //Printf("%x\n", ptMMR->mmr_quant_b_frm_6);

    // g_bPFrmInter16x16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_p_inter16x16_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter16x16_1);

    // g_bPFrmInter16x08
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_p_inter16x08_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter16x08_1);

    // g_bPFrmInter08x16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_p_inter08x16_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter08x16_1);

    // g_bPFrmInter08x08
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_p_inter08x08_1);
    //Printf("%x\n", ptMMR->mmr_en_p_inter08x08_1);

    // g_bBFrmInter16x16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_b_inter16x16_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter16x16_1);

    // g_bBFrmInter16x08
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_b_inter16x08_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter16x08_1);

    // g_bBFrmInter08x16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_b_inter08x16_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter08x16_1);

    // g_bBFrmInter08x08
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_en_b_inter08x08_1);
    //Printf("%x\n", ptMMR->mmr_en_b_inter08x08_1);

    // g_bRefinementMode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_refine_ment_mode);
    //Printf("%x\n", ptMMR->mmr_refine_ment_mode);

    //---- MMR inputs, Searching Range, Coarse Search ----//
    // g_dwPFrameSearchRangeH_div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_p_sr_h_div16_3);
    //Printf("%x\n", ptMMR->mmr_p_sr_h_div16_3);

    // g_dwPFrameSearchRangeV_div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_p_sr_v_div16_2);
    //Printf("%x\n", ptMMR->mmr_p_sr_v_div16_2);

    // g_dwBFrameSearchRangeH_div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_b_sr_h_div16_2);
    //Printf("%x\n", ptMMR->mmr_b_sr_h_div16_2);

    // g_dwBFrameSearchRangeV_div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_b_sr_v_div16_2);
    //Printf("%x\n", ptMMR->mmr_b_sr_v_div16_2);

    // IFME_ADAPTIVE_SEARCH_RANGE
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_avsr_enable);
    //Printf("%x\n", ptMMR->mmr_avsr_enable);

    // g_dwRDO_InterTr8Mode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_interTr8_mode);
    //Printf("%x\n", ptMMR->mmr_rdO_interTr8_mode);

    // g_dwRDO_InterTr4Mode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_interTr4_mode);
    //Printf("%x\n", ptMMR->mmr_rdO_interTr4_mode);

    // g_dwRDO_Intra8x8Mode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_intra8x8_mode);
    //Printf("%x\n", ptMMR->mmr_rdO_intra8x8_mode);

    // g_dwRDO_Intra4x4Mode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_intra4x4_mode);
    //Printf("%x\n", ptMMR->mmr_rdO_intra4x4_mode);

    // g_bRDO_Intra16Enable
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_intra16_enable);
    //Printf("%x\n", ptMMR->mmr_rdO_intra16_enable);

    // g_bRDO_SkipEnable
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_skip_enable);
    //Printf("%x\n", ptMMR->mmr_rdO_skip_enable);

    // g_bRDO_BDIRECTEnable
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rdO_bdirect_enable);
    //Printf("%x\n", ptMMR->mmr_rdO_bdirect_enable);

    // g_dwImageWidth_Div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_image_width_div16_7);
    //Printf("g_dwImageWidth_Div16 %x\n", ptMMR->mmr_image_width_div16_7);

    // g_dwImageHeight_Div16
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_image_height_div16_6);
    //Printf("g_dwImageHeight_Div16 %x\n", ptMMR->mmr_image_height_div16_6);

    // g_dwImageMbNum
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_num_mb_in_frm_12);
    //Printf("g_dwImageMbNum %x\n", ptMMR->mmr_num_mb_in_frm_12);
#ifdef VENC_CYCLE
    _u4VEncH264NumMB = ptMMR->mmr_num_mb_in_frm_12;
#endif

    //---- MMR inputs, Image Info. ----//
    // g_dwImageCropRight
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_image_crop_right_4);
    //Printf("%x\n", ptMMR->mmr_image_crop_right_4);

    // g_dwImageCropBottom
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_image_crop_bottom_5);
    //Printf("%x\n", ptMMR->mmr_image_crop_bottom_5);


    //---- MMR source scan Info. ----//
    // g_dwSourceScanType
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_source_scan_type_2);
    //Printf("%x\n", ptMMR->mmr_source_scan_type_2);
    
    // g_dwPitchWidthInPels
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_pitch_width_in_pels);
    //Printf("%x\n", ptMMR->mmr_pitch_width_in_pels);

    // g_bUseCABAC
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_use_cabac_1);
    //Printf("%x\n", ptMMR->mmr_use_cabac_1);

    // g_dwH264Level
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_h264_level_6);
    //Printf("%x\n", ptMMR->mmr_h264_level_6);

    // g_bUseMBAFF
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_use_mbaff_1);
    //Printf("%x\n", ptMMR->mmr_use_mbaff_1);

    // g_bUse8x8Tr
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_use_8x8Tr);
    //Printf("%x\n", ptMMR->mmr_use_8x8Tr);

    // g_dwH264Profile
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_h264_profile_8);
    //Printf("%x\n", ptMMR->mmr_h264_profile_8);

    // g_dwSlice_Alpha_C0_Offset_Div2
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_slice_alpha_c0_offset_div2);
    //Printf("%x\n", ptMMR->mmr_slice_alpha_c0_offset_div2);

    // g_dwSlice_Beta_Offset_Div2
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_slice_beta_offset_div2);
    //Printf("%x\n", ptMMR->mmr_slice_beta_offset_div2);

    // g_bUseCBR
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rc_use_cbr_1);
    //Printf("%x\n", ptMMR->mmr_rc_use_cbr_1);

    // g_bUseInitialQP
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rc_use_init_qp_1);
    //Printf("%x\n", ptMMR->mmr_rc_use_init_qp_1);

    // g_dwTargetBitrateInKbps
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rc_target_bit_rate_14);
    //Printf("%x\n", ptMMR->mmr_rc_target_bit_rate_14);

#ifndef VENC_MT8580_PATTERN
    ptMMR->use_stuff_byte = ptMMR->mmr_use_rc_1;
#endif

    //---- MMR inputs, fps ----//
    // g_dwfps
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_fps_8);
    //Printf("mmr_fps_8 %x\n", ptMMR->mmr_fps_8);

    // g_PfrmQLimiter
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_p_frm_q_limiter_8);
    //Printf("%x\n", ptMMR->mmr_p_frm_q_limiter_8);

    // g_BfrmQLimiter
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_b_frm_q_limiter_8);
    //Printf("%x\n", ptMMR->mmr_b_frm_q_limiter_8);

    // g_bSkypeMode
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_skype_mode);
    //Printf("mmr_rc_adaptive_fps %x\n", ptMMR->mmr_skype_mode);

    // g_dwAdaptiveFPS
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rc_adaptive_fps);
    //Printf("mmr_rc_adaptive_fps %x\n", ptMMR->mmr_rc_adaptive_fps);

    // g_dwAdaptiveTBR
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_rc_adaptive_tbr);
    //Printf("mmr_rc_adaptive_tbr %x\n", ptMMR->mmr_rc_adaptive_tbr);
    
    //g_EnableVUISupport
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_enable_vui_support_1);
    //Printf("%x\n", ptMMR->mmr_enable_vui_support_1);

    //g_VUI_aspect_ratio_info_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_aspect_ratio_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_aspect_ratio_info_present_flag);
                
    //g_VUI_aspect_ratio_idc
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_aspect_ratio_idc);
    //Printf("%x\n", ptMMR->mmr_vui_aspect_ratio_idc);
                              
    //g_VUI_sar_width
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_sar_width);
    //Printf("%x\n", ptMMR->mmr_vui_sar_width);
                                     
    //g_VUI_sar_height
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_sar_height);
    //Printf("%x\n", ptMMR->mmr_vui_sar_height);
                                    
    //g_VUI_overscan_info_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_overscan_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_overscan_info_present_flag);
                    
    //g_VUI_overscan_appropriate_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_overscan_appropriate_flag);
    //Printf("%x\n", ptMMR->mmr_vui_overscan_appropriate_flag);
                     
    //g_VUI_video_signal_type_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_video_signal_type_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_video_signal_type_present_flag);
                
    //g_VUI_video_format
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_video_format);
    //Printf("%x\n", ptMMR->mmr_vui_video_format);
                                  
    //g_VUI_video_full_range_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_video_full_range_flag);
    //Printf("%x\n", ptMMR->mmr_vui_video_full_range_flag);
                         
    //g_VUI_colour_description_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_colour_description_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_colour_description_present_flag);
               
    //g_VUI_colour_primaries
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_colour_primaries);
    //Printf("%x\n", ptMMR->mmr_vui_colour_primaries);
                              
    //g_VUI_transfer_characteristics
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_transfer_characteristics);
    //Printf("%x\n", ptMMR->mmr_vui_transfer_characteristics);
                      
    //g_VUI_matrix_coefficients
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_matrix_coefficients);
    //Printf("%x\n", ptMMR->mmr_vui_matrix_coefficients);
                           
    //g_VUI_chroma_location_info_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_location_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_location_info_present_flag);
             
    //g_VUI_chroma_sample_loc_type_top_field
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_sample_loc_type_top_field);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_sample_loc_type_top_field);
              
    //g_VUI_chroma_sample_loc_type_bottom_field
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_chroma_sample_loc_type_bottom_field);
    //Printf("%x\n", ptMMR->mmr_vui_chroma_sample_loc_type_bottom_field);
           
    //g_VUI_timing_info_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_timing_info_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_timing_info_present_flag);
                      
    //g_VUI_num_units_in_tick
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_num_units_in_tick);
    //Printf("%x\n", ptMMR->mmr_vui_num_units_in_tick);
                             
    //g_VUI_time_scale
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_time_scale);
    //Printf("%x\n", ptMMR->mmr_vui_time_scale);
                                    
    //g_VUI_fixed_frame_rate_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_fixed_frame_rate_flag);
    //Printf("%x\n", ptMMR->mmr_vui_fixed_frame_rate_flag);
                         
    //g_VUI_nal_hrd_parameters_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_hrd_parameters_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_nal_hrd_parameters_present_flag);
               
    //g_VUI_nal_bit_rate_scale
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_bit_rate_scale);
    //Printf("%x\n", ptMMR->mmr_vui_nal_bit_rate_scale);
                            
    //g_VUI_nal_cpb_size_scale
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_size_scale);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_size_scale);
                            
    //g_VUI_nal_bit_rate_value_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_bit_rate_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_bit_rate_value_minus1);
                     
    //g_VUI_nal_cpb_size_value_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_size_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_size_value_minus1);
                     
    //g_VUI_nal_vbr_cbr_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_vbr_cbr_flag);
    //Printf("%x\n", ptMMR->mmr_vui_nal_vbr_cbr_flag);
                              
    //g_VUI_nal_initial_cpb_removal_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1);

    //g_VUI_nal_cpb_removal_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_cpb_removal_delay_length_minus1);
           
    //g_VUI_nal_dpb_output_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_dpb_output_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_nal_dpb_output_delay_length_minus1);
            
    //g_VUI_nal_time_offset_length
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_nal_time_offset_length);
    //Printf("%x\n", ptMMR->mmr_vui_nal_time_offset_length);
                        
    //g_VUI_vcl_hrd_parameters_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_hrd_parameters_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_hrd_parameters_present_flag);
               
    //g_VUI_vcl_bit_rate_scale
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_bit_rate_scale);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_bit_rate_scale);
                            
    //g_VUI_vcl_cpb_size_scale
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_size_scale);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_size_scale);
                            
    //g_VUI_vcl_bit_rate_value_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_bit_rate_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_bit_rate_value_minus1);
                     
    //g_VUI_vcl_cpb_size_value_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_size_value_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_size_value_minus1);
                     
    //g_VUI_vcl_vbr_cbr_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_vbr_cbr_flag);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_vbr_cbr_flag);
                              
    //g_VUI_vcl_initial_cpb_removal_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1);

    //g_VUI_vcl_cpb_removal_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1);
           
    //g_VUI_vcl_dpb_output_delay_length_minus1
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_dpb_output_delay_length_minus1);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_dpb_output_delay_length_minus1);
            
    //g_VUI_vcl_time_offset_length
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_vcl_time_offset_length);
    //Printf("%x\n", ptMMR->mmr_vui_vcl_time_offset_length);
                        
    //g_VUI_low_delay_hrd_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_low_delay_hrd_flag);
    //Printf("%x\n", ptMMR->mmr_vui_low_delay_hrd_flag);
                            
    //g_VUI_pic_struct_present_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_pic_struct_present_flag);
    //Printf("%x\n", ptMMR->mmr_vui_pic_struct_present_flag);
                       
    //g_VUI_bitstream_restriction_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_bitstream_restriction_flag);
    //Printf("%x\n", ptMMR->mmr_vui_bitstream_restriction_flag);
                    
    //g_VUI_motion_vectors_over_pic_boundaries_flag
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag);
    //Printf("%x\n", ptMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag);
       
    //g_VUI_max_bytes_per_pic_denom
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_max_bytes_per_pic_denom);
    //Printf("%x\n", ptMMR->mmr_vui_max_bytes_per_pic_denom);
                       
    //g_VUI_max_bits_per_mb_denom
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_max_bits_per_mb_denom);
    //Printf("%x\n", ptMMR->mmr_vui_max_bits_per_mb_denom);
                         
    //g_VUI_log2_max_mv_length_vertical
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_log2_max_mv_length_vertical);
    //Printf("%x\n", ptMMR->mmr_vui_log2_max_mv_length_vertical);
                   
    //g_VUI_log2_max_mv_length_horizontal
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_log2_max_mv_length_horizontal);
    //Printf("%x\n", ptMMR->mmr_vui_log2_max_mv_length_horizontal);
                 
    //g_VUI_num_reorder_frames
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_num_reorder_frames);
    //Printf("%x\n", ptMMR->mmr_vui_num_reorder_frames);
                            
    //g_VUI_max_dec_frame_buffering
    vVenc_GetMmrfile(pBuf, &addr, &ptMMR->mmr_vui_max_dec_frame_buffering);
    //Printf("%x\n", ptMMR->mmr_vui_max_dec_frame_buffering);

/*
    if (ptMMR->mmr_source_scan_type_2 == 0)
    {
        u4Val = (ptMMR->mmr_image_width_div16_7 << 4) - ptMMR->mmr_image_crop_right_4;;
        ptMMR->mmr_pitch_width_in_pels = VENC_ALIGN_MASK(u4Val,15);
    }
*/

#ifdef VENC_CYCLE
    ptMMR->mmr_en_p_interskip_1 = _u4ForcePinterskip_1;
    ptMMR->mmr_en_p_inter16x16_1 = _u4ForcePinter16x16_1;
    ptMMR->mmr_en_p_inter16x08_1 = _u4ForcePinter16x08_1;
    ptMMR->mmr_en_p_inter08x16_1 = _u4ForcePinter08x16_1;
    ptMMR->mmr_en_p_inter08x08_1 = _u4ForcePinter08x08_1;
    ptMMR->mmr_en_b_interdirect_1 = _u4ForceBinterdirect_1;
    ptMMR->mmr_en_b_inter16x16_1 = _u4ForceBinter16x16_1;
    ptMMR->mmr_en_b_inter16x08_1 = _u4ForceBinter16x08_1;
    ptMMR->mmr_en_b_inter08x16_1 = _u4ForceBinter08x16_1;
    ptMMR->mmr_en_b_inter08x08_1 = _u4ForceBinter08x08_1;
#endif

#ifdef VENC_DRAM_PROFILE
    ptMMR->mmr_use_mbaff_1 = _u4ForceMbaffOn;
    u4Val = (ptMMR->mmr_num_of_b_frm_2 + 1) * ptMMR->dwNumOfIandPFrames;
    u4Val = u4Val / 6 * 6;
    ptMMR->dwNumOfIandPFrames = u4Val / (_u4ForceNumOfBFrm + 1); 
    ptMMR->mmr_num_of_b_frm_2 = _u4ForceNumOfBFrm; 
#endif
}

VOID vVenc_ConvertData(CHAR* pOldBuf, CHAR* pNewBuf, UINT32 u4OldBufSize, UINT32 u4Width, UINT32 u4Pitch)
{
    //UINT32  u4WidthIdx = 0;
    UINT32  u4Loop = 0;
    UINT32  u4OldBufIdx = 0;
    UINT32  u4NewBufIdx = 0;
    UINT8   u1Data = 0;
    UINT32  u4Temp = 0;

    for (u4OldBufIdx = 0; u4OldBufIdx < u4OldBufSize; u4OldBufIdx ++)
    {
        if (!(((0x30 <= pOldBuf[u4OldBufIdx]) && (0x39 >= pOldBuf[u4OldBufIdx])) ||
            ((0x41 <= pOldBuf[u4OldBufIdx]) && (0x46 >= pOldBuf[u4OldBufIdx])) ||
            ((0x61 <= pOldBuf[u4OldBufIdx]) && (0x66 >= pOldBuf[u4OldBufIdx]))))
        {
            continue;
        }

        if ((0x30 <= pOldBuf[u4OldBufIdx]) && (0x39 >= pOldBuf[u4OldBufIdx]))
        {
            u1Data = pOldBuf[u4OldBufIdx] - 0x30;
        }

        if ((0x41 <= pOldBuf[u4OldBufIdx]) && (0x46 >= pOldBuf[u4OldBufIdx]))
        {
            u1Data = pOldBuf[u4OldBufIdx] - 0x41 + 0xA;
        }

        if ((0x61 <= pOldBuf[u4OldBufIdx]) && (0x66 >= pOldBuf[u4OldBufIdx]))
        {
            u1Data = pOldBuf[u4OldBufIdx] - 0x61 + 0xA;
        }

        if (0xF < u1Data)
        {
            ASSERT(0);
        }

        switch (u4Loop)
        {
            case 0:
                u4Temp |= u1Data << 28;
                break;
            case 1:
                u4Temp |= u1Data << 24;
                break;
            case 2:
                u4Temp |= u1Data << 20;
                break;
            case 3:
                u4Temp |= u1Data << 16;
                break;
            case 4:
                u4Temp |= u1Data << 12;
                break;
            case 5:
                u4Temp |= u1Data << 8;
                break;
            case 6:
                u4Temp |= u1Data << 4;
                break;
            case 7:
                u4Temp |= u1Data << 0;
                break;
            default:
                ASSERT(0);
                break;
            }
        
        u4Loop ++;

        if (8 == u4Loop)
        {
            *((UINT32*)(pNewBuf + u4NewBufIdx)) = u4Temp;
            u4NewBufIdx += 4;
            //u4WidthIdx += 4;
        
            u4Loop = 0;
            u1Data = 0;
            u4Temp = 0;
        }

        /*if (u4Width && u4Pitch)
        {
            if (u4WidthIdx == u4Width)
            {
                u4WidthIdx = 0;
                u4NewBufIdx += (u4Pitch - u4Width);
            }
        }*/
    }
}

VOID vVenc_ExtractData(CHAR* pOldBuf, CHAR* pNewBuf)
{
    UINT32  u4OldBufIdx = 0;
    UINT32  u4NewBufIdx = 0;

    for (u4OldBufIdx = 0; u4OldBufIdx < (VENC_RAVEN_CONVERT_BUF_SIZE - 1); u4OldBufIdx ++)
    {
        if (0x0A == pOldBuf[u4OldBufIdx])
        {
            if (!(((0x30 <= pOldBuf[u4OldBufIdx + 1]) && (0x39 >= pOldBuf[u4OldBufIdx + 1])) ||
                ((0x41 <= pOldBuf[u4OldBufIdx + 1]) && (0x46 >= pOldBuf[u4OldBufIdx + 1])) ||
                ((0x61 <= pOldBuf[u4OldBufIdx + 1]) && (0x66 >= pOldBuf[u4OldBufIdx + 1]))))
            {
                break;
            }
        }
    
        if (0 != u4OldBufIdx)
        {
            if (0x0A == pOldBuf[u4OldBufIdx - 1])
            {
                vVenc_ConvertData((pOldBuf + u4OldBufIdx), (pNewBuf + u4NewBufIdx), 8, 0, 0);
                u4OldBufIdx += 8;
                u4NewBufIdx += 4;
            }
        }
        else
        {
            vVenc_ConvertData((pOldBuf + u4OldBufIdx), (pNewBuf + u4NewBufIdx), 8, 0, 0);
            u4OldBufIdx += 8;
            u4NewBufIdx += 4;
        }
    }
}

VOID vVenc_LoadData(UINT32 u4FrmIdx)
{
    char acFrmYUVFileName[512];

    if (0 == u4FrmIdx)
    {
        Printf("[raven] ramcode %s\n", _acRCFileName);
        Printf("[raven] mmr %s\n",     _acMMRFileName);
        Printf("[raven] sps %s\n",     _acSPSFileName);
        Printf("[raven] pps %s\n",     _acPPSFileName);
        Printf("[raven] crc %s\n",     _acCrcFileName);
        Printf("[raven] golden %s\n",  _acGoldenFileName);
        Printf("[raven] luma %s\n",    _acLumaFileName);
        Printf("[raven] chroma %s\n",  _acChromaFileName);
        Printf("[raven] chroma V %s\n",  _acChromaVFileName);

        HalFlushDCacheMultipleLine(_prVencVfyInfo->rHwBuf.u4RamCodeBuf, 4096*4);
    
        //mmr
        _VEncLoadFile(_acMMRFileName, _prVencVfyInfo->rVfyBuf.u4MmrBuf);
        vVenc_LoadMmr((CHAR *)(_prVencVfyInfo->rVfyBuf.u4MmrBuf), &(_prVencVfyInfo->rMmr));

        _prVencVfyInfo->u4SrcW = (_prVencVfyInfo->rMmr.mmr_image_width_div16_7) << 4;
        _prVencVfyInfo->u4SrcH = (_prVencVfyInfo->rMmr.mmr_image_height_div16_6) << 4;

        if (1 == _prVencVfyInfo->rMmr.mmr_source_scan_type_2)
        {
            _prVencVfyInfo->u4SrcAlignW = ((_prVencVfyInfo->u4SrcW + 15) >> 4) << 4;
            _prVencVfyInfo->u4SrcAlignH = ((_prVencVfyInfo->u4SrcH + 31) >> 5) << 5;
        }
        else if (2 == _prVencVfyInfo->rMmr.mmr_source_scan_type_2)
        {
            _prVencVfyInfo->u4SrcAlignW = ((_prVencVfyInfo->u4SrcW + 63) >> 6) << 6;
            _prVencVfyInfo->u4SrcAlignH = ((_prVencVfyInfo->u4SrcH + 31) >> 5) << 5;
        }
        else
        {
            _prVencVfyInfo->u4SrcAlignW = _prVencVfyInfo->u4SrcW;
            _prVencVfyInfo->u4SrcAlignH = _prVencVfyInfo->u4SrcH;
        }

        Printf("[raven] SRC W %d,  H %d,  Pitch %d,  AlignW %d,  AlignH %d\n", 
            _prVencVfyInfo->u4SrcW, _prVencVfyInfo->u4SrcH, 
            _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels,
            _prVencVfyInfo->u4SrcAlignW, _prVencVfyInfo->u4SrcAlignH);

        if (1 == _prVencVfyInfo->rMmr.mmr_use_rc_1)
        {
            //RamCode
            _VEncLoadFile(_acRCFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
            vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
                    (CHAR *)(_prVencVfyInfo->rHwBuf.u4RamCodeBuf),
                    40958, 0, 0);
        }

        //SPS
        _VEncLoadFile(_acSPSFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
        vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
                (CHAR *)(_prVencVfyInfo->rVfyBuf.u4SpsBuf),
                VENC_SPS_SIZE, 0, 0);

        //PPS
        _VEncLoadFile(_acPPSFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
        vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
                (CHAR *)(_prVencVfyInfo->rVfyBuf.u4PpsBuf),
                VENC_PPS_SIZE, 0, 0);

        x_memset((void*)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 0xff, VENC_RAVEN_CONVERT_BUF_SIZE);

        //CRC
        _VEncLoadFile(_acCrcFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
        vVenc_ExtractData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
                (CHAR *)(_prVencVfyInfo->rVfyBuf.u4CrcBuf));

        //Golden
        _VEncLoadFile(_acGoldenFileName, _prVencVfyInfo->rVfyBuf.u4GoldenBuf);
    }

    //luma
    x_memcpy(acFrmYUVFileName, _acLumaFileName, 512);
    if (u4FrmIdx < 10)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = u4FrmIdx + 0x30;
    }
    else if (u4FrmIdx < 100)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 31] = (u4FrmIdx / 10) + 0x30;
    }
    else if (u4FrmIdx < 1000)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 31] = (u4FrmIdx % 100 / 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 30] = (u4FrmIdx / 100) + 0x30;
    }
    else if (u4FrmIdx < 10000)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 31] = (u4FrmIdx % 1000 % 100 / 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 30] = (u4FrmIdx % 1000 / 100) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 29] = (u4FrmIdx / 1000) + 0x30;
    }
    else
    {
        ASSERT(0);
    }
    _VEncLoadFile(acFrmYUVFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
    vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
        (CHAR *)(_prVencVfyInfo->rHwBuf.au4SrcBufY[_prVencVfyInfo->u4SrcBufCurrIdx]),
        _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH * 9 / 4, _prVencVfyInfo->u4SrcW, _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels);
    HalFlushInvalidateDCacheMultipleLine((_prVencVfyInfo->rHwBuf.au4SrcBufY[_prVencVfyInfo->u4SrcBufCurrIdx]),
        _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH);

    //chroma
    x_memcpy(acFrmYUVFileName, _acChromaFileName, 512);
    if (u4FrmIdx < 10)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = u4FrmIdx + 0x30;
    }
    else if (u4FrmIdx < 100)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 33] = (u4FrmIdx / 10) + 0x30;
    }
    else if (u4FrmIdx < 1000)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 33] = (u4FrmIdx % 100 / 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = (u4FrmIdx / 100) + 0x30;
    }
    else if (u4FrmIdx < 10000)
    {
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = (u4FrmIdx % 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 33] = (u4FrmIdx % 1000 % 100 / 10) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 32] = (u4FrmIdx % 1000 / 100) + 0x30;
        acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 31] = (u4FrmIdx / 1000) + 0x30;
    }
    else
    {
        ASSERT(0);
    }
    _VEncLoadFile(acFrmYUVFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
    vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
        (CHAR *)(_prVencVfyInfo->rHwBuf.au4SrcBufC[_prVencVfyInfo->u4SrcBufCurrIdx]),
        _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH * 9 / 4 / 2, _prVencVfyInfo->u4SrcW, _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels);
    HalFlushInvalidateDCacheMultipleLine((_prVencVfyInfo->rHwBuf.au4SrcBufC[_prVencVfyInfo->u4SrcBufCurrIdx]),
        _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH / 2);

    if (3 == _prVencVfyInfo->rMmr.mmr_source_scan_type_2)
    {
        //YV12 --> V
        x_memcpy(acFrmYUVFileName, _acChromaVFileName, 512);
        if (u4FrmIdx < 10)
        {
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 36] = u4FrmIdx + 0x30;
        }
        else if (u4FrmIdx < 100)
        {
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 36] = (u4FrmIdx % 10) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 35] = (u4FrmIdx / 10) + 0x30;
        }
        else if (u4FrmIdx < 1000)
        {
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 36] = (u4FrmIdx % 10) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 35] = (u4FrmIdx % 100 / 10) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = (u4FrmIdx / 100) + 0x30;
        }
        else if (u4FrmIdx < 10000)
        {
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 36] = (u4FrmIdx % 10) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 35] = (u4FrmIdx % 1000 % 100 / 10) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 34] = (u4FrmIdx % 1000 / 100) + 0x30;
            acFrmYUVFileName[_prVencVfyInfo->u4FileHeadCnt + 33] = (u4FrmIdx / 1000) + 0x30;
        }
        else
        {
            ASSERT(0);
        }
        _VEncLoadFile(acFrmYUVFileName, _prVencVfyInfo->rVfyBuf.u4ConvertBuf);
        vVenc_ConvertData((CHAR *)(_prVencVfyInfo->rVfyBuf.u4ConvertBuf), 
            (CHAR *)(_prVencVfyInfo->rHwBuf.au4SrcBufV[_prVencVfyInfo->u4SrcBufCurrIdx]),
            _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH * 9 / 4 / 2, _prVencVfyInfo->u4SrcW, _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels);
        HalFlushInvalidateDCacheMultipleLine((_prVencVfyInfo->rHwBuf.au4SrcBufV[_prVencVfyInfo->u4SrcBufCurrIdx]),
            _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels * _prVencVfyInfo->u4SrcAlignH / 2);
    }

    Printf("[raven] load data end %s\n");
}

VOID vVenc_LoadFileList(VOID)
{
    UINT32  u4Idx = _prVencVfyInfo->u4FileListOft;
    UINT32  u4StartOft = _prVencVfyInfo->u4FileListOft;
    UINT32  u4EndOft = _prVencVfyInfo->u4FileListOft;
    CHAR*   prBuf = (CHAR*)(_prVencVfyInfo->rVfyBuf.u4ListBuf);

    if (0 == _prVencVfyInfo->u4FileListOft)
    {
        x_memset((void*)(_prVencVfyInfo->rVfyBuf.u4ListBuf), 0xff, VENC_RAVEN_LIST_BUF_SIZE);
        _VEncLoadFile(_acVfyListFileName, _prVencVfyInfo->rVfyBuf.u4ListBuf);
    }

    if (0x44 != prBuf[u4Idx])
    {
        _prVencVfyInfo->fgFileListEnd = TRUE;
        return ;
    }

    for (u4Idx = u4StartOft; u4Idx < (VENC_RAVEN_LIST_BUF_SIZE - u4StartOft - 1); u4Idx ++)
    {
        if ((0x0D == prBuf[u4Idx]) && (0x0A == prBuf[u4Idx + 1]))
        {
            u4EndOft = u4Idx;
            break;
        }
    }

    Printf("[raven] vVenc_LoadFileList u4StartOft %d, u4EndOft %d\n", u4StartOft, u4EndOft);

    if ((u4StartOft >= u4EndOft) || (450 < (u4EndOft - u4StartOft)))
    {
        Printf("[raven] vVenc_LoadFileList u4StartOft %d, u4EndOft %d\n", u4StartOft, u4EndOft);
        ASSERT(0);
    }

    _prVencVfyInfo->u4FileHeadCnt = u4EndOft - u4StartOft;

    x_memcpy(_acRCFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acRCFileName + (u4EndOft - u4StartOft)), _acRCFileKey, 50);

    x_memcpy(_acLumaFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acLumaFileName + (u4EndOft - u4StartOft)), _acLumaFileKey, 50);

    x_memcpy(_acChromaFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acChromaFileName + (u4EndOft - u4StartOft)), _acChromaFileKey, 50);

    x_memcpy(_acChromaVFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acChromaVFileName + (u4EndOft - u4StartOft)), _acChromaVFileKey, 50);

    x_memcpy(_acSPSFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acSPSFileName + (u4EndOft - u4StartOft)), _acSPSFileKey, 50);

    x_memcpy(_acPPSFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acPPSFileName + (u4EndOft - u4StartOft)), _acPPSFileKey, 50);

    x_memcpy(_acMMRFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acMMRFileName + (u4EndOft - u4StartOft)), _acMMRFileKey, 50);

    x_memcpy(_acGoldenFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acGoldenFileName + (u4EndOft - u4StartOft)), _acGoldenFileKey, 50);

    x_memcpy(_acCrcFileName, &(prBuf[u4StartOft]), (u4EndOft - u4StartOft));
    x_memcpy((_acCrcFileName + (u4EndOft - u4StartOft)), _acCrcFileKey, 50);
    
    _prVencVfyInfo->u4FileListOft = u4EndOft + 2;
    Printf("[raven] vVenc_LoadFileList u4FileListOft %d\n", _prVencVfyInfo->u4FileListOft);
}

void vVenc_MemFree(UINT32 u4Addr)
{

}

void vVenc_BufAlloc(void)
{
    UINT32 u4Idx = 0;

    //input_yc_buf & ref/rec_yc_buf
    for (u4Idx = 0; u4Idx < VENC_RAVEN_SRCBUF_CNT; u4Idx ++)
    {
        _prVencVfyInfo->rHwBuf.au4SrcBufY[u4Idx] = u4Venc_MemAlloc(VENC_RAVEN_SRC_Y_BUF_SIZE, 1024);
        _prVencVfyInfo->rHwBuf.au4SrcBufC[u4Idx] = u4Venc_MemAlloc(VENC_RAVEN_SRC_C_BUF_SIZE, 1024);
        _prVencVfyInfo->rHwBuf.au4SrcBufV[u4Idx] = u4Venc_MemAlloc(VENC_RAVEN_SRC_C_BUF_SIZE, 1024);

        Printf("[raven] alloc Y[%d] buf 0x%x size %d, C[%d] buf 0x%x size %d, V[%d] buf 0x%x size %d\n",
            u4Idx, _prVencVfyInfo->rHwBuf.au4SrcBufY[u4Idx], VENC_RAVEN_SRC_Y_BUF_SIZE,
            u4Idx, _prVencVfyInfo->rHwBuf.au4SrcBufC[u4Idx], VENC_RAVEN_SRC_C_BUF_SIZE,
            u4Idx, _prVencVfyInfo->rHwBuf.au4SrcBufV[u4Idx], VENC_RAVEN_SRC_C_BUF_SIZE);
    }

    //two col info buf
    _prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[0] = u4Venc_MemAlloc(VENC_RAVEN_COL_BUF_SIZE, 1024);
    _prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[1] = u4Venc_MemAlloc(VENC_RAVEN_COL_BUF_SIZE, 1024);
    Printf("[raven] alloc col[0] buf 0x%x size %d, col[1] buf 0x%x size %d\n",
        _prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[0], VENC_RAVEN_COL_BUF_SIZE,
        _prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[1], VENC_RAVEN_COL_BUF_SIZE);

    //output bitstream buf
    _prVencVfyInfo->rHwBuf.u4OutputBuf = u4Venc_MemAlloc(VENC_RAVEN_OUTPUT_BUF_SIZE, 1024);
    _prVencVfyInfo->rHwBuf.u4OutputBuf = (_prVencVfyInfo->rHwBuf.u4OutputBuf + 127) / 128 * 128;
    Printf("[raven] alloc output bitstream buf 0x%x size %d\n",
        _prVencVfyInfo->rHwBuf.u4OutputBuf, VENC_RAVEN_OUTPUT_BUF_SIZE);

    //ram code buf
    _prVencVfyInfo->rHwBuf.u4RamCodeBuf = u4Venc_MemAlloc(VENC_RAVEN_RAMCODE_BUF_SIZE, 128); //Must be less than 16k byte + 64 bytes
    Printf("[raven] alloc ramcode buf 0x%x size %d\n",
        _prVencVfyInfo->rHwBuf.u4RamCodeBuf, VENC_RAVEN_RAMCODE_BUF_SIZE);

    //rc info buf
    _prVencVfyInfo->rHwBuf.u4RCInfoBuf = u4Venc_MemAlloc(VENC_RAVEN_RCINFO_BUF_SIZE, 128);
    Printf("[raven] alloc rc info buf 0x%x size %d\n",
        _prVencVfyInfo->rHwBuf.u4RCInfoBuf, VENC_RAVEN_RCINFO_BUF_SIZE);

    //convert buf
    _prVencVfyInfo->rVfyBuf.u4ConvertBuf = u4Venc_MemAlloc(VENC_RAVEN_CONVERT_BUF_SIZE, 16);
    Printf("[raven] alloc convert buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4ConvertBuf, VENC_RAVEN_CONVERT_BUF_SIZE);

    //mmr buf
    _prVencVfyInfo->rVfyBuf.u4MmrBuf = u4Venc_MemAlloc(VENC_RAVEN_MMR_BUF_SIZE, 16);
    Printf("[raven] alloc mmr buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4MmrBuf, VENC_RAVEN_MMR_BUF_SIZE);

    //sps buf
    _prVencVfyInfo->rVfyBuf.u4SpsBuf = u4Venc_MemAlloc(VENC_RAVEN_SPS_BUF_SIZE, 16);
    Printf("[raven] alloc sps buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4SpsBuf, VENC_RAVEN_SPS_BUF_SIZE);

    //pps buf
    _prVencVfyInfo->rVfyBuf.u4PpsBuf = u4Venc_MemAlloc(VENC_RAVEN_PPS_BUF_SIZE, 16);
    Printf("[raven] alloc pps buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4PpsBuf, VENC_RAVEN_PPS_BUF_SIZE);

    //golden bitstream buf
    _prVencVfyInfo->rVfyBuf.u4GoldenBuf = u4Venc_MemAlloc(VENC_RAVEN_GOLDEN_BUF_SIZE, 16);
    Printf("[raven] alloc golden buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4GoldenBuf, VENC_RAVEN_GOLDEN_BUF_SIZE);

    //golden crc buf
    _prVencVfyInfo->rVfyBuf.u4CrcBuf = u4Venc_MemAlloc(VENC_RAVEN_CRC_BUF_SIZE, 16);
    Printf("[raven] alloc crc buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4CrcBuf, VENC_RAVEN_CRC_BUF_SIZE);

    //list buf
    _prVencVfyInfo->rVfyBuf.u4ListBuf = u4Venc_MemAlloc(VENC_RAVEN_LIST_BUF_SIZE, 16);
    Printf("[raven] alloc list buf 0x%x size %d\n",
        _prVencVfyInfo->rVfyBuf.u4ListBuf, VENC_RAVEN_LIST_BUF_SIZE);


/*
    for (u4Idx = 0; u4Idx < VENC_RAVEN_SRCBUF_CNT; u4Idx ++)
    {
        _prVencVfyInfo->rVfyBuf.au4InputBufY[u4Idx] = u4Venc_MemAlloc((u4DefaultWidth * u4DefaultHeight), 1024);
        _prVencVfyInfo->rVfyBuf.au4InputBufU[u4Idx] = u4Venc_MemAlloc((u4DefaultWidth * u4DefaultHeight / 4), 1024);
        _prVencVfyInfo->rVfyBuf.au4InputBufV[u4Idx] = u4Venc_MemAlloc((u4DefaultWidth * u4DefaultHeight / 4), 1024);
    }
*/
}

void vVenc_BufFree(void)
{

}

void vVenc_CompareGolden(UINT32 u4EncAddr, UINT32 u4GoldenAddr, UINT32 u4Size)
{
    UINT32  u4Idx = 0;
    CHAR*   pcEnc = (CHAR*)u4EncAddr;
    CHAR*   pcGolden = (CHAR*)u4GoldenAddr;

    for (u4Idx = 0; u4Idx < u4Size; u4Idx ++)
    {
        if (pcEnc[u4Idx] != pcGolden[u4Idx])
        {
            Printf("[raven] [ERR] compare byte[%d], enc 0x%x, golden 0x%x\n", 
                u4Idx, pcEnc[u4Idx], pcGolden[u4Idx]);
            //vVEnc_Assert();
            _u4CompareErr |= 0x1;
            break;
        }
    }
}

void vVenc_CompareCrc(UINT8 u1CrcCnt, VENC_RAVEN_CRC_T* prCrc)
{
    UINT32* pu4CrcGolden = (UINT32*)(_prVencVfyInfo->rVfyBuf.u4CrcBuf + _prVencVfyInfo->u4CrcFileRdOft);

    if (1 == u1CrcCnt)
    {
        if (prCrc->u4BsCrc != pu4CrcGolden[0])
        {
            Printf("[raven] [ERR] compare Sps/Pps Crc, enc 0x%x, golden 0x%x\n", 
                prCrc->u4BsCrc, pu4CrcGolden[0]);
            //vVEnc_Assert();
            _u4CompareErr |= 0x10;
        }
    }
    else if (8 == u1CrcCnt)
    {
        if ((prCrc->u4BsCrc        != pu4CrcGolden[0]) || 
            (prCrc->u4CurLumaCrc   != pu4CrcGolden[1]) || 
            (prCrc->u4RefLumaCrc   != pu4CrcGolden[2]) || 
            (prCrc->u4CurChromaCrc != pu4CrcGolden[3]) || 
            (prCrc->u4RefChromaCrc != pu4CrcGolden[4]) || 
            (prCrc->u4RecFrmCrc    != pu4CrcGolden[5]) || 
            (prCrc->u4RdComvCrc    != pu4CrcGolden[6]) || 
            (prCrc->u4SvComvCrc    != pu4CrcGolden[7]))
        {
            Printf("[raven] [ERR] compare Frm Crc, encode crc, golden crc\n");
            Printf("[raven] [ERR] BS               0x%08x, 0x%08x\n", prCrc->u4BsCrc, pu4CrcGolden[0]);
            Printf("[raven] [ERR] Curr Luma        0x%08x, 0x%08x\n", prCrc->u4CurLumaCrc, pu4CrcGolden[1]);
            Printf("[raven] [ERR] Ref Luma         0x%08x, 0x%08x\n", prCrc->u4RefLumaCrc, pu4CrcGolden[2]);
            Printf("[raven] [ERR] Curr Chroma      0x%08x, 0x%08x\n", prCrc->u4CurChromaCrc, pu4CrcGolden[3]);
            Printf("[raven] [ERR] Ref Chroma       0x%08x, 0x%08x\n", prCrc->u4RefChromaCrc, pu4CrcGolden[4]);
            Printf("[raven] [ERR] Rec Frm          0x%08x, 0x%08x\n", prCrc->u4RecFrmCrc, pu4CrcGolden[5]);
            Printf("[raven] [ERR] RD COMV          0x%08x, 0x%08x\n", prCrc->u4RdComvCrc, pu4CrcGolden[6]);
            Printf("[raven] [ERR] SV COMV          0x%08x, 0x%08x\n", prCrc->u4SvComvCrc, pu4CrcGolden[7]);
            
            //vVEnc_Assert();
            _u4CompareErr |= 0x10;
        }
    }
    else
    {
        ASSERT(0);
    }

    _prVencVfyInfo->u4CrcFileRdOft += 4 * u1CrcCnt;
}

void vVenc_SetVUI(VOID)
{
    VENC_RAVEN_VUI_PARAM_T  rVUIParam = {0};

    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1EnableVUISupport = _prVencVfyInfo->rMmr.mmr_enable_vui_support_1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_idc = _prVencVfyInfo->rMmr.mmr_vui_aspect_ratio_idc;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_info_present_flag = _prVencVfyInfo->rMmr.mmr_vui_aspect_ratio_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_bitstream_restriction_flag = _prVencVfyInfo->rMmr.mmr_vui_bitstream_restriction_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_location_info_present_flag = _prVencVfyInfo->rMmr.mmr_vui_chroma_location_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_sample_loc_type_bottom_field = _prVencVfyInfo->rMmr.mmr_vui_chroma_sample_loc_type_bottom_field;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_sample_loc_type_top_field = _prVencVfyInfo->rMmr.mmr_vui_chroma_sample_loc_type_top_field;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_colour_description_present_flag = _prVencVfyInfo->rMmr.mmr_vui_colour_description_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_fixed_frame_rate_flag = _prVencVfyInfo->rMmr.mmr_vui_fixed_frame_rate_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_low_delay_hrd_flag = _prVencVfyInfo->rMmr.mmr_vui_low_delay_hrd_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_nal_hrd_parameters_present_flag = _prVencVfyInfo->rMmr.mmr_vui_nal_hrd_parameters_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_overscan_appropriate_flag = _prVencVfyInfo->rMmr.mmr_vui_overscan_appropriate_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_overscan_info_present_flag = _prVencVfyInfo->rMmr.mmr_vui_overscan_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_pic_struct_present_flag = _prVencVfyInfo->rMmr.mmr_vui_pic_struct_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_timing_info_present_flag = _prVencVfyInfo->rMmr.mmr_vui_timing_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_vcl_hrd_parameters_present_flag = _prVencVfyInfo->rMmr.mmr_vui_vcl_hrd_parameters_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_format = _prVencVfyInfo->rMmr.mmr_vui_video_format;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_full_range_flag = _prVencVfyInfo->rMmr.mmr_vui_video_full_range_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_signal_type_present_flag = _prVencVfyInfo->rMmr.mmr_vui_video_signal_type_present_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_colour_primaries = _prVencVfyInfo->rMmr.mmr_vui_colour_primaries;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_matrix_coefficients = _prVencVfyInfo->rMmr.mmr_vui_matrix_coefficients;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_transfer_characteristics = _prVencVfyInfo->rMmr.mmr_vui_transfer_characteristics;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_height = _prVencVfyInfo->rMmr.mmr_vui_sar_height;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_width = _prVencVfyInfo->rMmr.mmr_vui_sar_width;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_bit_rate_scale = _prVencVfyInfo->rMmr.mmr_vui_nal_bit_rate_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_removal_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_nal_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_size_scale = _prVencVfyInfo->rMmr.mmr_vui_nal_cpb_size_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_dpb_output_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_nal_dpb_output_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_initial_cpb_removal_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_nal_initial_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_time_offset_length = _prVencVfyInfo->rMmr.mmr_vui_nal_time_offset_length;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_vbr_cbr_flag = _prVencVfyInfo->rMmr.mmr_vui_nal_vbr_cbr_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_bit_rate_value_minus1 = _prVencVfyInfo->rMmr.mmr_vui_nal_bit_rate_value_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_cpb_size_value_minus1 = _prVencVfyInfo->rMmr.mmr_vui_nal_cpb_size_value_minus1;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_bit_rate_scale = _prVencVfyInfo->rMmr.mmr_vui_vcl_bit_rate_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_cpb_removal_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_vcl_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_cpb_size_scale = _prVencVfyInfo->rMmr.mmr_vui_vcl_cpb_size_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_dpb_output_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_vcl_dpb_output_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_initial_cpb_removal_delay_length_minus1 = _prVencVfyInfo->rMmr.mmr_vui_vcl_initial_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_time_offset_length = _prVencVfyInfo->rMmr.mmr_vui_vcl_time_offset_length;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_vbr_cbr_flag = _prVencVfyInfo->rMmr.mmr_vui_vcl_vbr_cbr_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_8_T.u4VUI_vcl_bit_rate_value_minus1 = _prVencVfyInfo->rMmr.mmr_vui_vcl_bit_rate_value_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_8_T.u4VUI_vcl_cpb_size_value_minus1 = _prVencVfyInfo->rMmr.mmr_vui_vcl_cpb_size_value_minus1;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_log2_max_mv_length_horizontal = _prVencVfyInfo->rMmr.mmr_vui_log2_max_mv_length_horizontal;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_log2_max_mv_length_vertical = _prVencVfyInfo->rMmr.mmr_vui_log2_max_mv_length_vertical;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_bits_per_mb_denom = _prVencVfyInfo->rMmr.mmr_vui_max_bits_per_mb_denom;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_bytes_per_pic_denom = _prVencVfyInfo->rMmr.mmr_vui_max_bytes_per_pic_denom;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_dec_frame_buffering = _prVencVfyInfo->rMmr.mmr_vui_max_dec_frame_buffering;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_motion_vectors_over_pic_boundaries_flag = _prVencVfyInfo->rMmr.mmr_vui_motion_vectors_over_pic_boundaries_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_num_reorder_frames = _prVencVfyInfo->rMmr.mmr_vui_num_reorder_frames;

    rVUIParam.u4VUI_num_units_in_tick = _prVencVfyInfo->rMmr.mmr_vui_num_units_in_tick;
    rVUIParam.u4VUI_time_scale = _prVencVfyInfo->rMmr.mmr_vui_time_scale;

    i4Venc_Hal_VUISet(&rVUIParam);
}

void vVenc_SeqMmrSet(VOID)
{
    VENC_RAVEN_SEQ_MMR_PARAM_T  rSeqMmr = {0};

    rSeqMmr.rVENC_ENCODER_INFO_0.u1NUM_B_FRM = _prVencVfyInfo->rMmr.mmr_num_of_b_frm_2;
    rSeqMmr.rVENC_ENCODER_INFO_0.u1QP_I_FRM = _prVencVfyInfo->rMmr.mmr_quant_i_frm_6;
    rSeqMmr.rVENC_ENCODER_INFO_0.u4PERIOD_IDR_FRM = _prVencVfyInfo->rMmr.mmr_period_idr_frm_10;
    rSeqMmr.rVENC_ENCODER_INFO_0.u4PERIOD_I_FRM = _prVencVfyInfo->rMmr.mmr_period_i_frm_10;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_08X08 = _prVencVfyInfo->rMmr.mmr_en_p_inter08x08_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_08X16 = _prVencVfyInfo->rMmr.mmr_en_p_inter08x16_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_16X08 = _prVencVfyInfo->rMmr.mmr_en_p_inter16x08_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_16X16 = _prVencVfyInfo->rMmr.mmr_en_p_inter16x16_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_SEARCH_H = (_prVencVfyInfo->rMmr.mmr_p_sr_h_div16_3) << 4;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_SEARCH_V = (_prVencVfyInfo->rMmr.mmr_p_sr_v_div16_2) << 4;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1QP_P_FRM = _prVencVfyInfo->rMmr.mmr_quant_p_frm_6;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_BDIRECTEnable = _prVencVfyInfo->rMmr.mmr_rdO_bdirect_enable;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_InterTr4Mode = _prVencVfyInfo->rMmr.mmr_rdO_interTr4_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_InterTr8Mode = _prVencVfyInfo->rMmr.mmr_rdO_interTr8_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_Intra16Enable = _prVencVfyInfo->rMmr.mmr_rdO_intra16_enable;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_IntraTr4Mode = _prVencVfyInfo->rMmr.mmr_rdO_intra4x4_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_IntraTr8Mode = _prVencVfyInfo->rMmr.mmr_rdO_intra8x8_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_SkipEnable = _prVencVfyInfo->rMmr.mmr_rdO_skip_enable;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1CABAC = _prVencVfyInfo->rMmr.mmr_use_cabac_1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1H264_LEVEL = _prVencVfyInfo->rMmr.mmr_h264_level_6;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1MBAFF = _prVencVfyInfo->rMmr.mmr_use_mbaff_1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1PROFILE = _prVencVfyInfo->rMmr.mmr_h264_profile_8;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u4SLICE_ALPHA_C0_OFFSET_DIV2 = (_prVencVfyInfo->rMmr.mmr_slice_alpha_c0_offset_div2) << 1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u4SLICE_BETA_C0_OFFSET_DIV2 = (_prVencVfyInfo->rMmr.mmr_slice_beta_offset_div2) << 1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1USE_TR8X8 = _prVencVfyInfo->rMmr.mmr_use_8x8Tr;
    rSeqMmr.rVENC_IMAGE_INFO_0.u4IMAGE_HEIGHT_DIV16 = (_prVencVfyInfo->rMmr.mmr_image_height_div16_6) << 4;
    rSeqMmr.rVENC_IMAGE_INFO_0.u4IMAGE_WIDTH_DIV16 = (_prVencVfyInfo->rMmr.mmr_image_width_div16_7) << 4;
    rSeqMmr.rVENC_IMAGE_INFO_0.u2IMAGE_NUM_MB = _prVencVfyInfo->rMmr.mmr_num_mb_in_frm_12;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1IMAGE_CROP_BOTTOM = _prVencVfyInfo->rMmr.mmr_image_crop_bottom_5;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1IMAGE_CROP_RIGHT = _prVencVfyInfo->rMmr.mmr_image_crop_right_4;
    rSeqMmr.rVENC_IMAGE_INFO_1.u4pitch_width_div16 = _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1source_scan_type = _prVencVfyInfo->rMmr.mmr_source_scan_type_2;
    rSeqMmr.rVENC_IMAGE_INFO_1.u4uv_pitch_width_div16 = _prVencVfyInfo->rMmr.mmr_pitch_width_in_pels;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u1RC_CBR = _prVencVfyInfo->rMmr.mmr_rc_use_cbr_1;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u1RC_INI_QP = _prVencVfyInfo->rMmr.mmr_rc_use_init_qp_1;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u4RC_TARGET_BIT_RATE = _prVencVfyInfo->rMmr.mmr_rc_target_bit_rate_14;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1AFPS = _prVencVfyInfo->rMmr.mmr_rc_adaptive_fps;
    //rSeqMmr.rVENC_RATECONTROL_INFO_1.u1AIFI = _prVencVfyInfo->rMmr.;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1ATBR = _prVencVfyInfo->rMmr.mmr_rc_adaptive_tbr;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1BfrmQLimter = _prVencVfyInfo->rMmr.mmr_b_frm_q_limiter_8;
    //rSeqMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_EIS = _prVencVfyInfo->rMmr.;
    //rSeqMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_ROI = _prVencVfyInfo->rMmr.;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1PfrmQLimter = _prVencVfyInfo->rMmr.mmr_p_frm_q_limiter_8;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1RC_FPS = _prVencVfyInfo->rMmr.mmr_fps_8;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1SKYPE_MODE = _prVencVfyInfo->rMmr.mmr_skype_mode;
    rSeqMmr.rVENC_AVSR_ANRP_INFO_1.u1ANRP_ENABLE = 0;
    rSeqMmr.rVENC_AVSR_ANRP_INFO_1.u1AVSR_ENABLE = _prVencVfyInfo->rMmr.mmr_avsr_enable;
    //rSeqMmr.rVENC_AVSR_ANRP_INFO_1.u1ANRP_NUM_PREV_NRP = 
    i4Venc_Hal_SeqMmrSet(&rSeqMmr);
}

void vVenc_FrmMmrSet(UINT32 u4FrmIdx)
{
    VENC_RAVEN_FRM_MMR_PARAM_T  rFrmMmr = {0};
    
    rFrmMmr.rVENC_ENCODER_INFO_3.u1CHANGE_B_2_NONREF_P = 0;             //V
    rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SIGN = 0;                       //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1GEN_REC_FRM = 1;                     //V
    rFrmMmr.rVENC_ENCODER_INFO_3.u1IDR_PIC_ID = 0;                      //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1IME_REFINE_MODE = _prVencVfyInfo->rMmr.mmr_refine_ment_mode;                 //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1INTRA_WEIGHT_COST_EN = 1;            //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1PFRM_NONREF_P = 0;                   //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_PDIR = 0;                       //?
    if (_prVencVfyInfo->rMmr.mmr_period_i_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMVSEARCH_EN = (1 >= u4FrmIdx % _prVencVfyInfo->rMmr.mmr_period_i_frm_10)?0:1;  //?
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SCALER = (u4FrmIdx % _prVencVfyInfo->rMmr.mmr_period_i_frm_10)?0:6;   //?
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMVSEARCH_EN = (1 >= u4FrmIdx)?0:1;  //?
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SCALER = (1 > u4FrmIdx)?6:0;   //?
    }
    
    if (_prVencVfyInfo->rMmr.mmr_period_idr_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1IS_IDR_FRM = (u4FrmIdx % _prVencVfyInfo->rMmr.mmr_period_idr_frm_10)?0:1;  //V
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1IS_IDR_FRM = (1 > u4FrmIdx)?1:0;  //V
    }
    
    if (_prVencVfyInfo->rMmr.mmr_period_i_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_IMG_TYPE = (u4FrmIdx % _prVencVfyInfo->rMmr.mmr_period_i_frm_10)?0:2; //V
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_IMG_TYPE = (1 > u4FrmIdx)?2:0; //V
    }

    rFrmMmr.rVENC_RATECONTROL_INFO_0.u1RC_CBR = _prVencVfyInfo->rMmr.mmr_rc_use_cbr_1;
    rFrmMmr.rVENC_RATECONTROL_INFO_0.u1RC_INI_QP = _prVencVfyInfo->rMmr.mmr_rc_use_init_qp_1;
    rFrmMmr.rVENC_RATECONTROL_INFO_0.u4RC_TARGET_BIT_RATE = _prVencVfyInfo->rMmr.mmr_rc_target_bit_rate_14;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1AFPS = _prVencVfyInfo->rMmr.mmr_rc_adaptive_fps;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1AIFI = _prVencVfyInfo->rMmr.;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ATBR = _prVencVfyInfo->rMmr.mmr_rc_adaptive_tbr;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1BfrmQLimter = _prVencVfyInfo->rMmr.mmr_b_frm_q_limiter_8;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_EIS = _prVencVfyInfo->rMmr.;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_ROI = _prVencVfyInfo->rMmr.;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1PfrmQLimter = _prVencVfyInfo->rMmr.mmr_p_frm_q_limiter_8;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1RC_FPS = _prVencVfyInfo->rMmr.mmr_fps_8;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1SKYPE_MODE = _prVencVfyInfo->rMmr.mmr_skype_mode;

    rFrmMmr.u4RcInfoAddr = _prVencVfyInfo->rHwBuf.u4RCInfoBuf;
    rFrmMmr.u4ColLoadAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[0]:_prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[1];
    rFrmMmr.u4ColSaveAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[1]:_prVencVfyInfo->rHwBuf.au4ColMvInfoBuf[0];

    rFrmMmr.u4FrmCurYAddr = _prVencVfyInfo->rHwBuf.au4SrcBufY[_prVencVfyInfo->u4SrcBufCurrIdx];
    rFrmMmr.u4FrmCurCAddr = _prVencVfyInfo->rHwBuf.au4SrcBufC[_prVencVfyInfo->u4SrcBufCurrIdx];
    if (3 == _prVencVfyInfo->rMmr.mmr_source_scan_type_2)
    {
        rFrmMmr.u4FrmCurVAddr = _prVencVfyInfo->rHwBuf.au4SrcBufV[_prVencVfyInfo->u4SrcBufCurrIdx];
    }
    
    rFrmMmr.u4FrmRecYAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4SrcBufY[0]:_prVencVfyInfo->rHwBuf.au4SrcBufY[1];
    rFrmMmr.u4FrmRecCAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4SrcBufC[0]:_prVencVfyInfo->rHwBuf.au4SrcBufC[1];
    
    rFrmMmr.u4FrmRefYAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4SrcBufY[1]:_prVencVfyInfo->rHwBuf.au4SrcBufY[0];
    rFrmMmr.u4FrmRefCAddr = (u4FrmIdx % 2)?_prVencVfyInfo->rHwBuf.au4SrcBufC[1]:_prVencVfyInfo->rHwBuf.au4SrcBufC[0];

    rFrmMmr.u4FrmCnt = u4FrmIdx;
    rFrmMmr.u4FrmIdx = u4FrmIdx;
    if (_prVencVfyInfo->rMmr.mmr_period_idr_frm_10)
    {
        rFrmMmr.u4LastIDRFrmIdx = 
            (u4FrmIdx % _prVencVfyInfo->rMmr.mmr_period_idr_frm_10)?(u4FrmIdx / _prVencVfyInfo->rMmr.mmr_period_idr_frm_10 * _prVencVfyInfo->rMmr.mmr_period_idr_frm_10):u4FrmIdx;
    }
    else
    {
        rFrmMmr.u4LastIDRFrmIdx = 0;
    }

    i4Venc_Hal_FrmMmrSet(&rFrmMmr);
}

void vVenc_SpsEnc(VOID)
{
    VENC_RAVEN_ENC_PARAM_T  rEncParam = {0};

    rEncParam.u4OutputBufAddr = _prVencVfyInfo->rHwBuf.u4OutputBuf;
#if 1
    rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE;
#else
    rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE - _prVencVfyInfo->u4OutputBufWriteOft;
#endif
    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = _prVencVfyInfo->rMmr.mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = _prVencVfyInfo->rMmr.mmr_photo_mode;
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = _prVencVfyInfo->rMmr.mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;
    i4Venc_Hal_Enc(VENC_RAVEN_ENC_SPS, &rEncParam);
}

void vVenc_PpsEnc(VOID)
{
    VENC_RAVEN_ENC_PARAM_T  rEncParam = {0};

#if 1
    rEncParam.u4OutputBufAddr = _prVencVfyInfo->rHwBuf.u4OutputBuf;
    rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE;
#else
    rEncParam.u4OutputBufAddr = _prVencVfyInfo->rHwBuf.u4OutputBuf + _prVencVfyInfo->u4OutputBufWriteOft;
    rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE - _prVencVfyInfo->u4OutputBufWriteOft;
#endif
    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = _prVencVfyInfo->rMmr.mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = _prVencVfyInfo->rMmr.mmr_photo_mode;
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = _prVencVfyInfo->rMmr.mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;
    i4Venc_Hal_Enc(VENC_RAVEN_ENC_PPS, &rEncParam);
}

void vVenc_FrmEnc(UINT32 u4FrmIdx)
{
    VENC_RAVEN_ENC_PARAM_T  rEncParam = {0};

    rEncParam.u4OutputBufAddr = _prVencVfyInfo->rHwBuf.u4OutputBuf;
    
#if VENC_OUTPUT_FULL_TEST
    rEncParam.u4OutputBufSize = VENC_PART_OUTPUTBUF_SIZE;
    *((UINT32*)(_prVencVfyInfo->rHwBuf.u4OutputBuf + VENC_PART_OUTPUTBUF_SIZE)) = 0x12345678;
    HalFlushInvalidateDCacheMultipleLine((_prVencVfyInfo->rHwBuf.u4OutputBuf + VENC_PART_OUTPUTBUF_SIZE), 4);
#else
    rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE;
#endif

    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = _prVencVfyInfo->rMmr.mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = _prVencVfyInfo->rMmr.mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = ((1==_prVencVfyInfo->rMmr.mmr_use_rc_1)&&(u4FrmIdx<1))?1:0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = _prVencVfyInfo->rMmr.mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = (u4FrmIdx<1)?0:1;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = ((1==_prVencVfyInfo->rMmr.mmr_use_rc_1)&&(u4FrmIdx<1))?1:0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = _prVencVfyInfo->rMmr.mmr_photo_mode;
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = _prVencVfyInfo->rMmr.mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;

    i4Venc_Hal_Enc(VENC_RAVEN_ENC_FRM, &rEncParam);

#if VENC_OUTPUT_FULL_TEST
    if (rEncParam.fgOutputBufFull)
    {
        Printf("[raven] fifo full occur\n");
        
        HalInvalidateDCacheMultipleLine(_prVencVfyInfo->rHwBuf.u4OutputBuf, VENC_PART_OUTPUTBUF_SIZE * 2);
        if (0x12345678 != *((UINT32*)(_prVencVfyInfo->rHwBuf.u4OutputBuf + VENC_PART_OUTPUTBUF_SIZE)))
        {
            Printf("[raven] fifo full write error\n");
            vVEnc_Assert();
        }
    
        rEncParam.u4OutputBufAddr = _prVencVfyInfo->rHwBuf.u4OutputBuf + VENC_PART_OUTPUTBUF_SIZE;
        rEncParam.u4OutputBufSize = VENC_OUTPUTBUF_SIZE - VENC_PART_OUTPUTBUF_SIZE;
        i4Venc_Hal_Enc(VENC_RAVEN_ENC_RELEASE_BS_DRAM, &rEncParam);

        if (rEncParam.fgOutputBufFull)
        {
            Printf("[raven] error fifo full again\n");
            vVEnc_Assert();
        }
    }
#endif
}

void vVenc_ResultChk(UINT32 u4EncType, UINT32 u4FrmIdx)
{
    VENC_RESULT_INFO_T  rEncResultInfo = {0};
    VENC_RAVEN_CRC_T    rCrc = {0};

    i4Venc_Hal_GetResult(&rEncResultInfo);
#if VENC_OUTPUT_FULL_TEST
    HalInvalidateDCacheMultipleLine(_prVencVfyInfo->rHwBuf.u4OutputBuf, rEncResultInfo.u4BitstreamByteCnt);
    vVenc_CompareGolden(_prVencVfyInfo->rHwBuf.u4OutputBuf, (_prVencVfyInfo->rVfyBuf.u4GoldenBuf+_prVencVfyInfo->u4GoldenFileRdOft), rEncResultInfo.u4BitstreamByteCnt);
#else
    HalInvalidateDCacheMultipleLine(rEncResultInfo.u4BitstreamStartAddr, rEncResultInfo.u4BitstreamByteCnt);
    vVenc_CompareGolden(rEncResultInfo.u4BitstreamStartAddr, (_prVencVfyInfo->rVfyBuf.u4GoldenBuf+_prVencVfyInfo->u4GoldenFileRdOft), rEncResultInfo.u4BitstreamByteCnt);
#endif
    _prVencVfyInfo->u4GoldenFileRdOft += rEncResultInfo.u4BitstreamByteCnt;
    if (0 == (_u4CompareErr&0x1))
    {
        Printf("[raven] compare %s[%d] pixer pass\n", _acEncType[u4EncType - 1], u4FrmIdx);
    }
    else
    {
        Printf("[raven] compare %s[%d] pixer fail\n", _acEncType[u4EncType - 1], u4FrmIdx);
    }

    i4Venc_Hal_GetCrc(&rCrc);
    if ((VENC_RAVEN_ENC_SPS == u4EncType) || (VENC_RAVEN_ENC_PPS == u4EncType))
    {
        vVenc_CompareCrc(1, &rCrc);
    }
    else
    {
        vVenc_CompareCrc(8, &rCrc);
    }

    if (0 == (_u4CompareErr&0x10))
    {
        Printf("[raven] compare %s[%d] Crc pass\n", _acEncType[u4EncType - 1], u4FrmIdx);
    }
    else
    {
        Printf("[raven] compare %s[%d] Crc fail\n", _acEncType[u4EncType - 1], u4FrmIdx);
    }

    if (_u4CompareErr)
    {
        Printf("[raven] %s, compare %s[%d] fail, ErrType 0x%x\n", _acMMRFileName, _acEncType[u4EncType - 1], u4FrmIdx, _u4CompareErr);
        //vVEnc_Assert();
    }
}

void vVenc_VerifyThread(void *param_array)
{
    UINT32 u4PassSeq = 0;
    UINT32 u4FailSeq = 0;
    UINT32 u4SeqCnt = 0;
    UINT32 u4FrmIdx = 0;

    _prVencVfyInfo = NULL;
    _prVencVfyInfo = (VENC_RAVEN_VFY_INFO_T *)u4Venc_MemAlloc(sizeof(VENC_RAVEN_VFY_INFO_T), 4);
    x_memset((void*)_prVencVfyInfo, 0, sizeof(VENC_RAVEN_VFY_INFO_T));

    //fgVEnc_FSMount();

    vVenc_BufAlloc();

    i4Venc_Hal_Init();
    
    i4Venc_Hal_Power(TRUE);

    //_u4FreeSrcBufCnt = 4;
    _prVencVfyInfo->u4SrcBufCurrIdx = 2;

    while(1)
    {
    /*
        while (0 == i4Venc_GetSrcFreeCnt())
        {
            x_thread_delay(10);
        }
    */
    
        u4FrmIdx = 0;
        _prVencVfyInfo->u4GoldenFileRdOft = 0;
        _prVencVfyInfo->u4CrcFileRdOft = 0;
        _prVencVfyInfo->u4RealEncFrmCnt = 0;
        _prVencVfyInfo->rMmr.dwNumOfIandPFrames = 1;    //firstly set to 1, after loading mmr it will be set to a real value

        vVenc_LoadFileList();
        if (_prVencVfyInfo->fgFileListEnd)
        {
            break;
        }

        i4Venc_Hal_HwReset();

        i4Venc_Hal_SetRCInfoBuf(_prVencVfyInfo->rHwBuf.u4RCInfoBuf);            

        while (u4FrmIdx < _prVencVfyInfo->rMmr.dwNumOfIandPFrames)
        {
            _u4CompareErr = 0;
        
            vVenc_LoadData(u4FrmIdx);
            
            if (0 == u4FrmIdx)
            {
                i4Venc_Hal_SetRamCodeBuf(_prVencVfyInfo->rHwBuf.u4RamCodeBuf);

                if (_prVencVfyInfo->rMmr.mmr_enable_vui_support_1)
                {
                    vVenc_SetVUI();
                }

                vVenc_SeqMmrSet();

                vVenc_SpsEnc();
                
                vVenc_ResultChk(VENC_RAVEN_ENC_SPS, u4FrmIdx);
                if (_u4CompareErr)
                {
                    break;
                }

                vVenc_PpsEnc();

                vVenc_ResultChk(VENC_RAVEN_ENC_PPS, u4FrmIdx);
                if (_u4CompareErr)
                {
                    break;
                }
            }

            //i4Venc_DecSrcFreeCnt();
            
            vVenc_FrmMmrSet(u4FrmIdx);

            vVenc_FrmEnc(u4FrmIdx);

            vVenc_ResultChk(VENC_RAVEN_ENC_FRM, u4FrmIdx);
            if (_u4CompareErr)
            {
                break;
            }

            //i4Venc_EncSrcFreeCnt();

            _prVencVfyInfo->u4RealEncFrmCnt ++;
            u4FrmIdx ++;

            /*_prVencVfyInfo->u4SrcBufCurrIdx ++;
            if (VENC_RAVEN_SRCBUF_CNT <= _prVencVfyInfo->u4SrcBufCurrIdx)
            {
                _prVencVfyInfo->u4SrcBufCurrIdx = 2;
            }*/

            Printf("[raven] Real Enc Frame Cnt = %d\n", _prVencVfyInfo->u4RealEncFrmCnt);
        }

        if (_u4CompareErr)
        {
            u4FailSeq ++;
        }
        else
        {
            u4PassSeq ++;
        }

        u4SeqCnt ++;
        Printf("[raven] %s, Finish Sequence Test , count %d, Pass %d, Fail %d\n", _acMMRFileName, u4SeqCnt, u4PassSeq, u4FailSeq);
    }

    Printf("[raven] Finish test , total count %d, Pass %d, Fail %d\n", u4SeqCnt, u4PassSeq, u4FailSeq);

    //vVencBufFree();

    //x_thread_exit();
}

void vVEnc_VerifyTest(void)
{
    HANDLE_T hEncThread;
    VOID* param[4];
    
    Printf("Encoder HW Verification Start\n");

    x_thread_create(&hEncThread, "Enc Vfy", MPV_VPRS_STACK_SIZE,
            MPV_VPRS_THREAD_PRIORITY, vVenc_VerifyThread, sizeof(VOID*)*4, param);

/*
    x_thread_create(&hEncThread, "Enc Frm", MPV_VPRS_STACK_SIZE,
            MPV_VPRS_THREAD_PRIORITY, vEncodeFrameThread, sizeof(VOID*)*4, param);

    x_thread_create(&hEncThread, "Chk Golden", MPV_VPRS_STACK_SIZE,
            MPV_VPRS_THREAD_PRIORITY, vGoldenCheckThread, sizeof(VOID*)*4, param);
*/
}

/*
VOID vVenc_SwIntUV(UINT8 *pu1U, UINT8 *pu1V, UINT8 *pu1C, UINT32 UBufSize)
{
    INT32 i, j;

    for (i = j = 0; i < UBufSize; i++)
    {
        pu1C[j++] = pu1U[i];
        pu1C[j++] = pu1V[i];
    }
}

HANDLE_T _hVencVfyEvent;

void vVenc_CreateEventGroup(void)
{
    CHAR szBuf[16] = "VENCVFY";
    VERIFY(x_ev_group_create(&_hVencVfyEvent, szBuf, 0) == OSR_OK);
}

void vVenc_DeleteEventGroup(void)
{
    x_ev_group_delete(_hVencVfyEvent);
}

void vVenc_SetEvent(UINT32 u4SetEvent)
{
    VERIFY(OSR_OK == x_ev_group_set_event(_hVencVfyEvent, u4SetEvent, X_EV_OP_OR));
}

void vVenc_WaitEvent(UINT32 u4WaitEvent)
{
    UINT32 u4GetEvent = VENC_INVALID_DW;
    VERIFY(OSR_OK == x_ev_group_wait_event(_hVencVfyEvent, u4WaitEvent, &u4GetEvent, X_EV_OP_OR_CONSUME));
}

INT32 i4Venc_DecSrcFreeCnt(void)
{
    _u4FreeSrcBufCnt --;

    return 0;
}

INT32 i4Venc_EncSrcFreeCnt(void)
{
    _u4FreeSrcBufCnt ++;

    return 0;
}

INT32 i4Venc_GetSrcFreeCnt(void)
{
    return _u4FreeSrcBufCnt;
}

UINT32 u4Venc_GetFreeOutputBufSize(void)
{
    UINT32 u4WriteOft = 0;
    UINT32 u4ReadOft = 0;
    UINT32 u4FreeSize = 0;

    u4WriteOft = _prVencVfyInfo->u4OutputBufWriteOft;
    u4ReadOft = _prVencVfyInfo->u4OutputBufReadOft;

    if (u4ReadOft < u4WriteOft)
    {
        u4FreeSize = u4ReadOft + VENC_OUTPUTBUF_SIZE - u4WriteOft;
    }
    else
    {
        u4FreeSize = u4ReadOft - u4WriteOft;
    }

    return u4FreeSize;
}


UINT32 u4Venc_ResultFreeCnt(void)
{
    UINT32 u4RdIdx, u4WrIdx;
    u4WrIdx = _prVencVfyInfo->u4ResultWriteIdx;
    u4RdIdx = _prVencVfyInfo->u4ResultReadIdx;

    if (u4RdIdx < u4WrIdx)
    {
        return (u4WrIdx + VENC_RAVEN_RESULT_CNT - u4RdIdx);
    }
    else
    {
        return (u4WrIdx - u4RdIdx);
    }
}


void vGoldenCheck_Assert(void)
{
  UINT32 u4PrintCnt = 0;
  UINT32 u4Break = 1;
  
  while (u4Break)
  {
    if (((u4PrintCnt % 100) == 0) && (u4PrintCnt<1000))
    {
      Printf("[raven] Error Assert \n");
      u4Break++;
    }
    x_thread_delay(1);
    
    u4PrintCnt++;
    u4Break++;
  }

  u4PrintCnt = 0;
  u4Break = 0;
  Printf("[raven] Assert func out \n");
}

void vEncodeFrameThread(void *param_array)
{
    UINT32      u4OutputBufFreeSize = 0;

    while (1)
    {
        x_thread_delay(10);

        if (NULL != _prVencVfyInfo)
        {
            x_thread_delay(100);
            break;
        }
    }
    
    while (1)
    {
        if (_prVencVfyInfo->u4FrmEncSrcReadIdx == _prVencVfyInfo->u4FrmEncSrcWriteIdx)
        {
            x_thread_delay(20);
            continue;
        }

        Printf("[raven] Frame[%d] encode start\n", _prVencVfyInfo->u4RealEncFrmCnt);

        //i4Venc_Hal_EncFrm();

        //i4VEnc_HAL_WaitLocalArbEmpty();

        //i4VEnc_HAL_GetCRCInfo(&rCRCInfo);

        i4Venc_EncSrcFreeCnt();

        Printf("[raven] Current Src Free Count %d\n", _u4FreeSrcBufCnt);
        if ((1 == _u4FreeSrcBufCnt) && (_prVencVfyInfo->fgWaitSrcBufFree))
        {
            vVenc_SetEvent(VENC_VFY_EVENT_SRCBUF);
        }

        //i4VEnc_HAL_GetResult(&(prVEncVerifyInfo->rEncResult));

        if (_prVencVfyInfo->fgChkCrc)
        {
            i4Venc_Hal_GetCrc();
            vVenc_CompareCrc();
        }


        if (_prVencVfyInfo->fgChkGolden)
        {
            
        
            _prVencVfyInfo->u4ResultWriteIdx ++;
            if (VENC_RAVEN_RESULT_CNT <= _prVencVfyInfo->u4ResultWriteIdx)
            {
                _prVencVfyInfo->u4ResultWriteIdx = 0;
            }
        }
        else
        {
            //_prVencVfyInfo->u4OutputBufReadOft = 
        }

        _prVencVfyInfo->u4RealEncFrmCnt ++;



        while (u4OutputBufFreeSize <= VENC_OUTPUTBUF_SIZE)
        {
            x_thread_delay(10);
            u4OutputBufFreeSize = u4Venc_GetFreeOutputBufSize();
        }

        while (u4Venc_ResultFreeCnt() <= 5)
        {
            x_thread_delay(10);
        }
    }
}

void vGoldenCheckThread(void *param_array)
{
    while (1)
    {
        x_thread_delay(10);

        if (NULL != _prVencVfyInfo)
        {
            x_thread_delay(100);
            break;
        }
    }

    Printf("[raven] Start golden check , read %d, write %d\n", 
        _prVencVfyInfo->u4ResultReadIdx, _prVencVfyInfo->u4ResultWriteIdx);
    
    while (1)
    {
        if (_prVencVfyInfo->u4ResultReadIdx == _prVencVfyInfo->u4ResultWriteIdx)
        {
            x_thread_delay(20);
            continue;
        }

        Printf("[raven] Frame[%d] check start\n", _prVencVfyInfo->u4GoldenChkFrmCnt);

        //vVenc_CompareGolden();

        vVenc_CompareCrc();

        //_prVencVfyInfo->u4OutputBufReadOft = 

        _prVencVfyInfo->u4ResultReadIdx ++;
        if (VENC_RAVEN_RESULT_CNT <= _prVencVfyInfo->u4ResultReadIdx)
        {
            _prVencVfyInfo->u4ResultReadIdx = 0;
        }

        _prVencVfyInfo->u4GoldenChkFrmCnt ++;
        Printf("[raven] Frame[%d] check check pass\n", (_prVencVfyInfo->u4GoldenChkFrmCnt-1));
    }
}
*/


