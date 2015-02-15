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
 * $RCSfile: venc_h264api.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_h264api.c
 *  This file contains implementation of exported APIs of H.264.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "venc_drvif.h"
#include "venc_h264api.h"
#include "venc_raven_h264_hal.h"

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_mid.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
#include "x_chip_id.h"
#include "x_debug.h"
LINT_EXT_HEADER_END;

#define VENC_DUMP_DRAM  0

#if VENC_DUMP_DRAM
#include "../fm/x_fm.h"
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/param.h>
#include <linux/err.h>
#endif


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

#ifndef VENC_EMU
#define VENC_DISABLE_MID
#endif


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define VENC_H264_MAX_COARSE_IMAGE_SIZE VENC_ALIGN_MASK((VENC_H264_MAX_VIDEO_RESOLUTION / 16), (VIDEOCODEC_HAL_DEF_DRAM_PAGE_SIZE - 1))

#define VENC_H264_BUF_ALIGN_MASK (31)
#define VENC_H264_SW_NALU_BUF_SIZE (4 * 1024)
#define VENC_H264_SW_NALU_SIZE_THRD (64)
#define VENC_H264_ROM_CODE_SIZE (4 * 1024 * 8 + 8 * 8)
#define VENC_H264_DUMP_INFO_SIZE (2048)
#define VENC_H264_UPPER_INFO_MB_SIZE ((VENC_H264_MAX_VIDEO_RESOLUTION_WIDTH / 16) * 64)
#define VENC_H264_COL_INFO_SIZE VENC_ALIGN_MASK((((VENC_H264_MAX_VIDEO_RESOLUTION_WIDTH / 16) * (VENC_H264_MAX_VIDEO_RESOLUTION_HEIGHT / 16) * 4) / 8), 63)
#define VENC_H264_CS_INFO_SIZE (((((VENC_H264_MAX_VIDEO_RESOLUTION_WIDTH / 4) / 8) * ((((VENC_H264_MAX_VIDEO_RESOLUTION_HEIGHT / 4) / 8 + 3) >> 2) << 2) + 63 + 64) >> 6) << 9)

#ifdef CC_VENC_DUMP_INFO
#define VENC_H264_WORKING_BUF_SIZE (VENC_H264_UPPER_INFO_MB_SIZE + VENC_H264_COL_INFO_SIZE + VENC_H264_CS_INFO_SIZE + VENC_H264_DUMP_INFO_SIZE)
#else
#define VENC_H264_WORKING_BUF_SIZE (VENC_H264_UPPER_INFO_MB_SIZE + VENC_H264_COL_INFO_SIZE + VENC_H264_CS_INFO_SIZE)
#endif

#define VENC_H264_THUMBNAIL_ARGB_SIZE VENC_THUMBNAIL_SIZE
#define VENC_H264_THUMBNAIL_YUV420_SIZE (VENC_H264_THUMBNAIL_ARGB_SIZE * 3 / 8)
#define VENC_H264_AUD_NALU_BUF_SIZE (1024)

#define VENC_RAVEN_BUF_ADD  (1024*4)

#define VENC_RAVEN_RAMCODE_BUF_SIZE     (1024 * 16 + VENC_RAVEN_BUF_ADD)
#define VENC_RAVEN_RCINFO_BUF_SIZE      (1024 + VENC_RAVEN_BUF_ADD)

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define SEI_BUFFERING_PERIOD                     0
#define SEI_PIC_TIMING                           1
#define SEI_PAN_SCAN_RECT                        2
#define SEI_FILLER_PAYLOAD                       3
#define SEI_USER_DATA_REGISTERED_ITU_T_T35       4
#define SEI_USER_DATA_UNREGISTERED               5
#define SEI_RECOVERY_POINT                       6
#define SEI_DEC_REF_PIC_MARKING_REPETITION       7
#define SEI_SPARE_PIC                            8
#define SEI_SCENE_INFO                           9
#define SEI_SUB_SEQ_INFO                         10
#define SEI_SUB_SEQ_LAYER_CHARACTERISTICS        11
#define SEI_SUB_SEQ_CHARACTERISTICS              12
#define SEI_FULL_FRAME_FREEZE                    13
#define SEI_FULL_FRAME_FREEZE_RELEASE            14
#define SEI_FULL_FRAME_SNAPSHOT                  15
#define SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START 16
#define SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END   17
#define SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET   18
#define SEI_FILM_GRAIN_CHARACTERISTICS           19
#define SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE 20
#define SEI_STEREO_VIDEO_INFO                    21


#define SEI_FUNC_MARK_CALC_SIZE   0x01
#define SEI_FUNC_MARK_WRIT_DATA   0x02


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define _H264_NEED_COARSE(_w,_h) (!(((_w) < 128) || ((_h) < 120)))

#define SEI_PUT_BIT(name, val, n) if (mask&SEI_FUNC_MARK_WRIT_DATA){PUT_BIT(val,n);} size+=n;
#define SEI_PUT_BIT_UE(name, val) if (mask&SEI_FUNC_MARK_WRIT_DATA){PUT_BIT_UE(val);} size+=bits_ue_size(val);
#define SEI_PUT_BIT_SE(name, val) if (mask&SEI_FUNC_MARK_WRIT_DATA){PUT_BIT_SE(val);} size+=bits_se_size(val);
#define SEI_ALIGN_BIT()           if (mask&SEI_FUNC_MARK_WRIT_DATA){ALIGN_BIT();}

//#define DECLARE_SEI_FUNC(type) static void _H264SEIPayload##type(VENC_BITS_BUFFER *PBITS_BUFFER,UINT32 *pSize,UINT8 mask)
//#define IMPLEMENT_SEI_FUNC(type) DECLARE_SEI_FUNC(type)

#define PUT_SEI_PAYLOAD(obj,type) _H264SEIPayload##type(obj,PBITS_BUFFER, 0, SEI_FUNC_MARK_WRIT_DATA)
#define GET_SEI_PAYLOAD_SIZE(obj,type,size) _H264SEIPayload##type(obj,0, &size, SEI_FUNC_MARK_CALC_SIZE)

#define PUT_SEI_MESSAGE(obj,type)                                   \
do                                                                  \
{                                                                   \
    UINT16 payloadType;                                             \
    UINT32 payloadSize;                                             \
    payloadType = SEI_##type;                                       \
    GET_SEI_PAYLOAD_SIZE(obj,type,payloadSize);                     \
    while (payloadType/0xFF) {PUT_BIT(0xFF,8);payloadType -= 0xFF;} \
    PUT_BIT(payloadType,8);                                         \
    while (payloadSize/0xFF) {PUT_BIT(0xFF,8);payloadSize -= 0xFF;} \
    PUT_BIT(payloadSize,8);                                         \
    PUT_SEI_PAYLOAD(obj,type);                                      \
    PUT_BIT(1,1);ALIGN_BIT();                                       \
} while (0)

#define UP_SW_NALU_WP(ctrl, size)                       \
do {                                                    \
    (ctrl)->u4NaluBufWp += (size);                      \
    ASSERT((size) < 64);                                \
    if ((ctrl)->u4NaluBufWp + 64 > (ctrl)->u4NaluBufEa) \
    {                                                   \
        (ctrl)->u4NaluBufWp = (ctrl)->u4NaluBufSa;      \
    }                                                   \
} while(0)

/*
- If any of the following is true, the value of CpbDpbDelaysPresentFlag shall be set equal to 1.
- nal_hrd_parameters_present_flag is present in the bitstream and is equal to 1
- vcl_hrd_parameters_present_flag is present in the bitstream and is equal to 1
- the need for presence of CPB and DPB output delays to be present in the bitstream in picture timing SEI
messages is determined by the application, by some means not specified in this Recommendation | International
Standard.
- Otherwise, the value of CpbDpbDelaysPresentFlag shall be set equal to 0.
*/
#define CpbDpbDelaysPresentFlag (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
#define bit_rate_scale (4)
#define cpb_size_scale (6)
#define cpb_cnt_minus1 (0)
#define Extended_SAR (255)



//just for dev
#define _H264_DBG(fmt...)     LOG(7, fmt)
#define _H264_FUNC_BEGIN()    _H264_DBG("_VENC<H264>::func Begin:%s\n", __FUNCTION__)
#define _H264_FUNC_END(ret)   _H264_DBG("_VEHC<H264>::func end:  %s, ret:%d\n", __FUNCTION__, ret)
#define _H264_FUNC_ENTER()    _H264_DBG("_VENC<H264>::func Enter:%s\n", __FUNCTION__)
#define _H264_FUNC_LEAVE()    _H264_DBG("_VENC<H264>::func leave:%s\n", __FUNCTION__)


#define _H264_MEMBER_OFFSET(type, member)  ((UINT32)(&(((type*)0)->member)))

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#ifdef INPUT_BUF_FROM_SCALER
BOOL fgTriggerScaler = TRUE; //add this variable for controlling scaler easily
#endif

static UINT8 pic_struct_present_flag = 1;
static UINT8 nal_hrd_parameters_present_flag = 1;
static UINT8 vcl_hrd_parameters_present_flag = 0;
static UINT8 cpb_removal_delay_length_minus1 = 7;
static UINT8 dpb_output_delay_length_minus1= 7;

// the following global variable can be local if code size reduction needed
UINT8 aspect_ratio_info_present_flag = 0;
UINT8 overscan_info_present_flag = 0;
UINT8 video_signal_type_present_flag = 0;
UINT8 chroma_loc_info_present_flag = 0;
UINT8 timing_info_present_flag = 1;
UINT8 bitstream_restriction_flag = 0;

#ifdef VENC_EMU
#define REZ_REC_N 256
UINT32 _u4RzYAddr[REZ_REC_N];
UINT32 _u4RzData[REZ_REC_N];
UINT32 _u4RzIdx;
#endif

UINT32 _u4BsTmp;

#ifdef VENC_TIME_PROFILE
static HAL_TIME_T _rTimeHwE;
static HAL_TIME_T _rTimeHwDt;
static UINT32 _u4FrmCnt, _u4EncTime;
#endif

BOOL _fgFixQp = FALSE;

//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------

#ifdef VENC_EMU
extern VOID _VEncH264EmuSetSkypeInfo(VENC_OBJECT_T *this);
#endif

#ifdef INPUT_BUF_FROM_SCALER
extern UINT8 u1DrvScpipGetVencModule(void);
extern void u1DrvScpipH264FreeCurrentFb(UINT32 u4YAddr);
#endif

extern UINT32 StrToInt(const CHAR* pszStr);

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------
//DECLARE_SEI_FUNC(PIC_TIMING);
static void _H264SEIPayloadPIC_TIMING(VENC_OBJECT_T *this, VENC_BITS_BUFFER *PBITS_BUFFER, UINT32 *pSize, UINT8 mask);
static UINT32 _H264AddAUD(VENC_OBJECT_T *this, UINT32 * u4SwOutAddr,UINT32 *u4SwOutSize);

static void vVenc_SetVUI(VENC_OBJECT_T *this);
static void vVenc_SeqMmrSet(VENC_OBJECT_T *this);
static void vVenc_FrmMmrSet(VENC_OBJECT_T *this);
static UINT32 u4VENC_H264GENSPS_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size);
static UINT32 u4VENC_H264GENPPS_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size);
static UINT32 u4VENC_H264GENFRM_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size);
static VOID vVenc_H264_LoadRCCode(VENC_OBJECT_T *this);
static UINT32 _H264CalcDesParm(VENC_OBJECT_T *this);

#ifdef __MODEL_slt__
static UINT32 u4CrcBuf[50] = 
{
    0xf0ca07dc,     //sps VENC_CRC_BSDMA
    0xd24be864,     //pps VENC_CRC_BSDMA
    0x350df890,     //Frm 0    VENC_CRC_BSDMA
    0xe6165a60,     //Frm 0    VENC_CRC_CUR_LUMA
    0x00000000,     //Frm 0    VENC_CRC_REF_LUMA
    0xc2dd2f01,     //Frm 0    VENC_CRC_CUR_CHROMA
    0x00000000,     //Frm 0    VENC_CRC_REF_CHROMA
    0x3beaadfd,     //Frm 0    VENC_CRC_REC_FRM
    0x00000000,     //Frm 0    VENC_CRC_RD_COMV
    0x00000000,     //Frm 0    VENC_CRC_SV_COMV
    0xdd67db49,     //Frm 1    VENC_CRC_BSDMA
    0x910cbb1f,     //Frm 1    VENC_CRC_CUR_LUMA
    0x918c5a42,     //Frm 1    VENC_CRC_REF_LUMA
    0xc8eb31e3,     //Frm 1    VENC_CRC_CUR_CHROMA
    0xbc6f2e9b,     //Frm 1    VENC_CRC_REF_CHROMA
    0x561ac012,     //Frm 1    VENC_CRC_REC_FRM
    0x00000000,     //Frm 1    VENC_CRC_RD_COMV
    0x00bb07d3,     //Frm 1    VENC_CRC_SV_COMV
    0x8f209e13,     //Frm 2    VENC_CRC_BSDMA
    0x36edfed4,     //Frm 2    VENC_CRC_CUR_LUMA
    0x1dbd98d4,     //Frm 2    VENC_CRC_REF_LUMA
    0x18d739c2,     //Frm 2    VENC_CRC_CUR_CHROMA
    0x365247e6,     //Frm 2    VENC_CRC_REF_CHROMA
    0x7113cbe8,     //Frm 2    VENC_CRC_REC_FRM
    0x0000d707,     //Frm 2    VENC_CRC_RD_COMV
    0x00996ed0,     //Frm 2    VENC_CRC_SV_COMV
    0x943c46db,     //Frm 3    VENC_CRC_BSDMA
    0x122525e2,     //Frm 3    VENC_CRC_CUR_LUMA
    0x1a015cbc,     //Frm 3    VENC_CRC_REF_LUMA
    0xeadbc11f,     //Frm 3    VENC_CRC_CUR_CHROMA
    0xebb99503,     //Frm 3    VENC_CRC_REF_CHROMA
    0x06ee5d61,     //Frm 3    VENC_CRC_REC_FRM
    0x00003208,     //Frm 3    VENC_CRC_RD_COMV
    0x0039e338,     //Frm 3    VENC_CRC_SV_COMV
    0xf26da6c5,     //Frm 4    VENC_CRC_BSDMA
    0x05659164,     //Frm 4    VENC_CRC_CUR_LUMA
    0xe4d0e0a0,     //Frm 4    VENC_CRC_REF_LUMA
    0x1b17cdc9,     //Frm 4    VENC_CRC_CUR_CHROMA
    0xad058dc0,     //Frm 4    VENC_CRC_REF_CHROMA
    0x0d8abcf2,     //Frm 4    VENC_CRC_REC_FRM
    0x0000f500,     //Frm 4    VENC_CRC_RD_COMV
    0x00c10116,     //Frm 4    VENC_CRC_SV_COMV
    0x7f8e853b,     //Frm 5    VENC_CRC_BSDMA
    0xeedb3728,     //Frm 5    VENC_CRC_CUR_LUMA
    0xad283896,     //Frm 5    VENC_CRC_REF_LUMA
    0xd4db321b,     //Frm 5    VENC_CRC_CUR_CHROMA
    0xa8bb0ec7,     //Frm 5    VENC_CRC_REF_CHROMA
    0x794c3e9c,     //Frm 5    VENC_CRC_REC_FRM
    0x000013e8,     //Frm 5    VENC_CRC_RD_COMV
    0x0021c962,     //Frm 5    VENC_CRC_SV_COMV
    0xc2e1a4ea,     //Frm 6    VENC_CRC_BSDMA
    0xde93a08d,     //Frm 6    VENC_CRC_CUR_LUMA
    0x5e7759db,     //Frm 6    VENC_CRC_REF_LUMA
    0xc7f734f4,     //Frm 6    VENC_CRC_CUR_CHROMA
    0x6db57d68,     //Frm 6    VENC_CRC_REF_CHROMA
    0x92d869dd,     //Frm 6    VENC_CRC_REC_FRM
    0x0000ef3e,     //Frm 6    VENC_CRC_RD_COMV
    0x00c766ee     //Frm 6    VENC_CRC_SV_COMV
};
static UINT32 u4CrcFileRdOft = 0;
static UINT32 _u4CompareErr = 0;

static void vVenc_CompareCrc(UINT8 u1CrcCnt, VENC_RAVEN_CRC_T* prCrc)
{
    UINT32* pu4CrcGolden = (UINT32*)(u4CrcBuf + u4CrcFileRdOft);

    if (1 == u1CrcCnt)
    {
        if (prCrc->u4BsCrc != pu4CrcGolden[0])
        {
            LOG(0, "[raven] [ERR] compare Sps/Pps Crc, enc 0x%x, golden 0x%x\n", 
                prCrc->u4BsCrc, pu4CrcGolden[0]);
            ASSERT(0);
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
            LOG(0, "[raven] [ERR] compare Frm Crc, encode crc, golden crc\n");
            LOG(0, "[raven] [ERR] BS               0x%08x, 0x%08x\n", prCrc->u4BsCrc, pu4CrcGolden[0]);
            LOG(0, "[raven] [ERR] Curr Luma        0x%08x, 0x%08x\n", prCrc->u4CurLumaCrc, pu4CrcGolden[1]);
            LOG(0, "[raven] [ERR] Ref Luma         0x%08x, 0x%08x\n", prCrc->u4RefLumaCrc, pu4CrcGolden[2]);
            LOG(0, "[raven] [ERR] Curr Chroma      0x%08x, 0x%08x\n", prCrc->u4CurChromaCrc, pu4CrcGolden[3]);
            LOG(0, "[raven] [ERR] Ref Chroma       0x%08x, 0x%08x\n", prCrc->u4RefChromaCrc, pu4CrcGolden[4]);
            LOG(0, "[raven] [ERR] Rec Frm          0x%08x, 0x%08x\n", prCrc->u4RecFrmCrc, pu4CrcGolden[5]);
            LOG(0, "[raven] [ERR] RD COMV          0x%08x, 0x%08x\n", prCrc->u4RdComvCrc, pu4CrcGolden[6]);
            LOG(0, "[raven] [ERR] SV COMV          0x%08x, 0x%08x\n", prCrc->u4SvComvCrc, pu4CrcGolden[7]);
            
            ASSERT(0);
            _u4CompareErr |= 0x10;
        }
    }
    else
    {
        ASSERT(0);
    }

    u4CrcFileRdOft += u1CrcCnt;
}
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static VOID _H264RezSendFrm(VENC_OBJECT_T *this, VENC_FRMQ_DATA_T *prFrm)
{
    INT32 i4Ret;
    SIZE_T zMsgSize = sizeof(VENC_FRMQ_DATA_T);
    H264_CTRL_T *pCtrl = (H264_CTRL_T *)this->hCtrl;

    _H264_FUNC_ENTER();

    i4Ret = x_msg_q_send(pCtrl->hRezFrmQ, prFrm, zMsgSize, VENC_FRMQ_PRIORITY);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        LOG(0, "VEnc SendFrm Queue, OSR_TOO_MANY\n");
        i4Ret = x_msg_q_send(pCtrl->hRezFrmQ, prFrm, zMsgSize, VENC_FRMQ_PRIORITY);
    }

    if (OSR_OK != i4Ret)
    {
        LOG(1, "%s: fail(%d)\n", __FUNCTION__, i4Ret);
        ASSERT(i4Ret == OSR_OK);
    }
}


static UINT32 _H264RezSendCmd(VENC_OBJECT_T *this, VENC_CMD_T eRezCmd)
{
    INT32 i4Ret;
    UINT32 u4Cmd = eRezCmd;
    SIZE_T zMsgSize = sizeof(UINT32);
    H264_CTRL_T *pCtrl = (H264_CTRL_T *)this->hCtrl;
    VENC_FRMQ_DATA_T rFrm;

    i4Ret = x_msg_q_send(pCtrl->hRezCmdQ, &u4Cmd, zMsgSize, VENC_CMDQ_PRIORITY);
    while (i4Ret == OSR_TOO_MANY)
    {
        x_thread_delay(10);
        i4Ret = x_msg_q_send(pCtrl->hRezCmdQ, &u4Cmd, zMsgSize, VENC_CMDQ_PRIORITY);
    }

    if (OSR_OK != i4Ret)
    {
        LOG(1, "%s: fail(%d)\n", __FUNCTION__, i4Ret);
        ASSERT(i4Ret == OSR_OK);
    }

    rFrm.eType = VENC_FRMQ_TYPE_DUMMY;
    _H264RezSendFrm(this, &rFrm);

    return VENC_OK;
}


static UINT32 _H264RezReceiveCmd(H264_CTRL_T *pCtrl, VOID *pvData)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;

    ASSERT(pCtrl);

    zMsgSize = sizeof(UINT32);

    if (pCtrl->eRezCmdState != VENC_STATE_RUN)
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(pCtrl->hRezCmdQ), 1, X_MSGQ_OPTION_WAIT);
    }
    else
    {
        i4Ret = x_msg_q_receive(&u2MsgQIdx, pvData, &zMsgSize,
                                &(pCtrl->hRezCmdQ), 1, X_MSGQ_OPTION_NOWAIT);
    }

    ASSERT(zMsgSize == sizeof(UINT32));

    if (i4Ret != OSR_OK)
    {
        return VENC_INTERNAL_ERR;
    }

    return VENC_OK;
}

static UINT32 _H264RezProcessCmd(VENC_OBJECT_T *this, UINT32 u4Cmd, BOOL *pfgRun)
{
    VENC_CMD_T eCmd = (VENC_CMD_T)u4Cmd;
    H264_CTRL_T *pCtrl = (H264_CTRL_T *)this->hCtrl;

    _H264_FUNC_ENTER();

    LOG(6, "%s: %d\n", __FUNCTION__, u4Cmd);

    if (eCmd == VENC_CMD_START_ASYNC || eCmd == VENC_CMD_START_SYNC)
    {
        if (pCtrl->eRezCmdState != VENC_STATE_STOP)
        {
            return VENC_OK;
        }
        pCtrl->eRezCmdState   = VENC_STATE_RUN;
        pCtrl->eRezFrmState   = H264_REZ_STATE_REZ;
    }
    else if (eCmd == VENC_CMD_STOP_ASYNC || eCmd == VENC_CMD_STOP_SYNC)
    {
        pCtrl->eRezCmdState    = VENC_STATE_STOP;
        pCtrl->eRezFrmState   = H264_REZ_STATE_IDLE;

    }
    else if (eCmd == VENC_CMD_EXIT_ASYNC || eCmd == VENC_CMD_EXIT_SYNC)
    {
        pCtrl->eRezCmdState    = VENC_STATE_STOP;
        pCtrl->eRezFrmState   = H264_REZ_STATE_IDLE;
        *pfgRun = FALSE;
    }

    if ((((UINT8)eCmd)>>4) == 0X1)   //SYNC
    {
        _VEncNotifySyncCmdDone(this);
    }

    return VENC_OK;
}


static VOID _H264RezRoutine(VENC_OBJECT_T *this)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;
    VENC_FRMQ_DATA_T tData;
    H264_CTRL_T *pCtrl;

    if (!this)
    {
        LOG(1, "%s: this is NULL!!\n", __FUNCTION__);
        ASSERT(this);
        return;
    }

    zMsgSize = sizeof(VENC_FRMQ_DATA_T);
    pCtrl    = (H264_CTRL_T *)(this->hCtrl);

    if (!pCtrl)
    {
        LOG(1, "%s: pCtrl is NULL!!\n", __FUNCTION__);
        ASSERT(pCtrl);
        return;
    }

    i4Ret    = x_msg_q_receive(&u2MsgQIdx, (VOID*)&tData, &zMsgSize,
                            &(pCtrl->hRezFrmQ), 1, X_MSGQ_OPTION_WAIT);

    if (OSR_OK != i4Ret)
    {
        LOG(1, "%s: fail(%d)\n", __FUNCTION__, i4Ret);
        ASSERT(i4Ret == OSR_OK);
        return;
    }

    _H264_DBG("_VENC<H264>::rez get %d\n", tData.eType);

    if (VENC_FRMQ_TYPE_FRM == tData.eType)
    {
        UINT32 u4Ret;
        _H264_FUNC_ENTER();

        if (this->fgResizeSrc || this->u4GfxConfig)//(tData.u.frm.tIn.fgRzSrc)
        {
            VencRezSrc(this, &tData.u.frm.tIn);
        }

        if (this->fgGetThumbnail)
        {
            if (VencGetThumbnail(this, &tData.u.frm.tIn, pCtrl->u4ThumbnailYuv420, pCtrl->u4ThumbnailARGB))
            {
                this->fgGetThumbnail = FALSE;
            }
        }

        if (pCtrl->fgCoarse)
        {
            ASSERT(pCtrl && pCtrl->ptCoarseBuffMgr && pCtrl->ptCoarseBuffMgr->Get);
            u4Ret = pCtrl->ptCoarseBuffMgr->Get(pCtrl->ptCoarseBuffMgr, (VOID *)(&(tData.u.frm.tPram.data)), VENC_BUFMGR_WAIT);
            ASSERT(u4Ret == VENC_BUFMGR_OK);
            _H264_DBG("_VENC<H264>::rez frame (src,tag) => (0x%08x, 0x%08x)\n", tData.u.frm.tIn.u4YAddr, tData.u.frm.tPram.data);
#ifdef VENC_EMU
            _u4RzYAddr[_u4RzIdx] = tData.u.frm.tIn.u4YAddr;
            _u4RzData[_u4RzIdx] = tData.u.frm.tPram.data;
            if (++_u4RzIdx >= REZ_REC_N) { _u4RzIdx = 0; }
#endif
            ASSERT(pCtrl && pCtrl->ptRezObj && pCtrl->ptRezObj->Resize);
            pCtrl->ptRezObj->Resize(pCtrl->ptRezObj, tData.u.frm.tIn.u4YAddr, tData.u.frm.tPram.data);
        }

        _VENC_SendFrm(this, &tData);
    }
    else if (VENC_FRMQ_TYPE_RT_PARM == tData.eType)
    {
        VENC_RT_PARM_T *ptParm = (VENC_RT_PARM_T *)&(tData.u.tRTParm);
        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_RES)
        {
#if 1 // move from _H264EncRoutine for VencRezSrc
            this->u2Width  = ptParm->u2Width;
            this->u2Height = ptParm->u2Height;
            this->u2Pitch  = ptParm->u2Pitch;
#endif
        }
        _VENC_SendFrm(this, &tData);
    }
    else if (VENC_FRMQ_TYPE_RT_CTRL == tData.eType)
    {
        _H264_DBG("_VENC<H264>::rez send RT CTRL, flag -> 0x%x\n", tData.u.tRTCtrl.u4Flag);
        _VENC_SendFrm(this, &tData);
    }
}


static VOID _H264RezMainLoop(VOID *pvArg)
{
    VENC_OBJECT_T *this;
    H264_CTRL_T* pCtrl;
    UINT32 u4Cmd;
    BOOL fgRun = TRUE;

    _H264_FUNC_ENTER();
    ASSERT(pvArg);

    this  = (VENC_OBJECT_T *)(*(UINT32 *)pvArg);
    pCtrl = (H264_CTRL_T *)(this->hCtrl);

    _H264_DBG("Rez Object:0x%08x\n", (UINT32)this);


    while (fgRun)
    {
        if (VENC_OK == _H264RezReceiveCmd(pCtrl, &u4Cmd))
        {
            _H264RezProcessCmd(this, u4Cmd, &fgRun);
        }

        if (H264_REZ_STATE_REZ == pCtrl->eRezFrmState)
        {
            _H264RezRoutine(this);
        }
    }

    _H264_FUNC_END(VENC_OK);

    x_thread_exit();

}

static UINT32 _H264RezInit(VENC_OBJECT_T *this, H264_CTRL_T* pCtrl)
{
    UINT32 u4Ret = VENC_OK;

    _H264_FUNC_ENTER();

    ASSERT(this && pCtrl);

    if (!(pCtrl->ptRezObj))
    {
        pCtrl->ptRezObj = VencCreateRezObject();//VencCreate68SwRezObject();
        ASSERT(pCtrl->ptRezObj);
        if (!pCtrl->ptRezObj)
        {
            return VENC_FAIL;
        }
    }

    pCtrl->ptRezObj->ePixFmt   = this->ePixFmt;

    pCtrl->ptRezObj->u4SrcH    = this->u2Height;
    pCtrl->ptRezObj->u4SrcW    = this->u2Width;
    pCtrl->ptRezObj->u4SrcPitch= this->u2Pitch;

    pCtrl->ptRezObj->u4TgH     = pCtrl->u2DesHeidht;
    pCtrl->ptRezObj->u4TgW     = pCtrl->u2DesWidth;
    pCtrl->ptRezObj->u4TgPitch = pCtrl->u2DesPitch;

    ASSERT(pCtrl->ptRezObj->Init);
    pCtrl->ptRezObj->Init(pCtrl->ptRezObj);

    //async mode resizer mainloop

    if (VENC_SUPPORT_ASYNC_MODE(this->eSMode))
    {
        INT32 i4Ret = OSR_OK;
        CHAR acName[16];
        UINT16 u2Hash = (UINT32)this;

        // create rez command queue
        if (NULL_HANDLE == pCtrl->hRezCmdQ)
        {
            x_snprintf(acName, 16, "ERCQ%4X", u2Hash);
            i4Ret = x_msg_q_create(&(pCtrl->hRezCmdQ), acName/*"VREZ_CMDQ"*/,
                sizeof(UINT32), VENC_CMDQ_SIZE);
            if (i4Ret != OSR_OK)
            {
                LOG(0, "%s: create msg %s fail(%d)\n", __FUNCTION__, acName, i4Ret);
                ASSERT(i4Ret == OSR_OK);
                return VENC_FAIL;
            }
        }

        // create rez frame queue
        if (NULL_HANDLE == pCtrl->hRezFrmQ)
        {
            x_snprintf(acName, 16, "ERFQ%4X", u2Hash);
            i4Ret = x_msg_q_create(&(pCtrl->hRezFrmQ), acName/*"VREZ_FRMQ"*/,
                sizeof(VENC_FRMQ_DATA_T), VENC_FRMQ_SIZE);

            if (i4Ret != OSR_OK)
            {
                LOG(0, "%s: create msg %s fail(%d)\n", __FUNCTION__, acName, i4Ret);
                ASSERT(i4Ret == OSR_OK);
                return VENC_FAIL;
            }
        }

        //sema for sync cmd
        // create rez main loop thread for command receiving
        if (NULL_HANDLE == pCtrl->hRezThread)
        {
            UINT32 u4RetryCnt = 0;
            UINT32 arg = (UINT32)this;
            x_snprintf(acName, 16, "ERzT%4X", u2Hash);
            i4Ret = x_thread_create(&(pCtrl->hRezThread),
                acName/*"RezThread"*/, VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
                (x_os_thread_main_fct)_H264RezMainLoop, sizeof(UINT32), (VOID *)&arg);

            while (OSR_EXIST == i4Ret) // if stress close/open, this might happen
            {
                if (u4RetryCnt > 100)
                {
                    break;
                }
                x_thread_delay(u4RetryCnt);
                ++u4RetryCnt;
                LOG(0, "%s: thread %s does not exit yet, retry %d\n", __FUNCTION__, acName, u4RetryCnt);
                i4Ret = x_thread_create(&(pCtrl->hRezThread),
                    acName/*"RezThread"*/, VENC_STACK_SIZE, VENC_THREAD_PRIORITY,
                    (x_os_thread_main_fct)_H264RezMainLoop, sizeof(UINT32), (VOID *)&arg);
            }

            if (i4Ret != OSR_OK)
            {
                LOG(0, "%s: create thread %s fail(%d)\n", __FUNCTION__, acName, i4Ret);
                ASSERT(i4Ret == OSR_OK);
                return VENC_FAIL;
            }
        }

        UNUSED(acName);
        UNUSED(u2Hash);
    }

    _H264_FUNC_LEAVE();

    return u4Ret;
}

static UINT32 _H264BuffInit(VENC_OBJECT_T *this, H264_CTRL_T* pCtrl)
{
    UINT32 u4Ret = VENC_OK;
    UINT32 u4TmpAddr;
    UINT32 u4Idx;

    UINT32 u4YSize;
    UINT32 u4Value;
    _H264_FUNC_ENTER();

    u4TmpAddr = this->u4BuffAddr + this->u4UsedSize;

    u4YSize   = VENC_ALIGN_MASK(this->u2Width, 63) * VENC_ALIGN_MASK(this->u2Height, 31);

    u4Value = MAX(u4YSize, VENC_H264_MAX_VIDEO_RESOLUTION); //For changing encode parameters in run-time
    
    //ref & rec_yc_buf
    for (u4Idx = 0; u4Idx < VENC_RAVEN_SRCBUF_CNT; u4Idx ++)
    {
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1023);
        pCtrl->rHwBuf.au4SrcBufY[u4Idx] = u4TmpAddr;
        u4TmpAddr += u4Value;
        
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1023);
        pCtrl->rHwBuf.au4SrcBufC[u4Idx] = u4TmpAddr;
        u4TmpAddr += (u4Value >> 1);

        LOG(1,"SrcBufY[%d] = 0x%x, SrcBufC[%d] = 0x%x\n",u4Idx,pCtrl->rHwBuf.au4SrcBufY[u4Idx],u4Idx,pCtrl->rHwBuf.au4SrcBufC[u4Idx]);
        
        //u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1024);
        //pCtrl->rHwBuf.au4SrcBufV[u4Idx] = u4TmpAddr; //this buffer may be not used for the current applications.
        //u4TmpAddr += (u4Value >> 1);
    }
    
    //two col info buf
    u4Value = (u4YSize / 16  / 16 * 4 + VENC_RAVEN_BUF_ADD);
    u4Value = MAX(u4Value, (VENC_H264_MAX_VIDEO_RESOLUTION / 16 / 16 * 4 + VENC_RAVEN_BUF_ADD));
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1023);
    pCtrl->rHwBuf.au4ColMvInfoBuf[0] = u4TmpAddr;
    LOG(1,"ColMvInfoBuf[0] = 0x%x\n",pCtrl->rHwBuf.au4ColMvInfoBuf[0]);
    u4TmpAddr += u4Value;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1023);
    pCtrl->rHwBuf.au4ColMvInfoBuf[1] = u4TmpAddr;
    u4TmpAddr += u4Value;
    LOG(1,"ColMvInfoBuf[1] = 0x%x\n",pCtrl->rHwBuf.au4ColMvInfoBuf[1]);

    #ifndef __MODEL_slt__
    // SW NALU buffer
    pCtrl->u4NaluBufSa = u4TmpAddr;
    LOG(1,"NaluBufSa = 0x%x\n",pCtrl->u4NaluBufSa);
    pCtrl->u4NaluBufWp = u4TmpAddr;
    pCtrl->u4NaluBufSz = VENC_H264_SW_NALU_BUF_SIZE;
    u4TmpAddr += VENC_H264_SW_NALU_BUF_SIZE;
    pCtrl->u4NaluBufEa = u4TmpAddr;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, VENC_H264_BUF_ALIGN_MASK);

    // Dump info buffer
    pCtrl->u4DumpInfoAddr = u4TmpAddr;
    LOG(1,"DumpInfoAddr = 0x%x\n",pCtrl->u4DumpInfoAddr);
    u4TmpAddr += VENC_H264_DUMP_INFO_SIZE;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, VENC_H264_BUF_ALIGN_MASK);

    // Thumbnail buffer
    pCtrl->u4ThumbnailYuv420 = u4TmpAddr;
    LOG(1,"u4ThumbnailYuv420 = 0x%x\n",pCtrl->u4ThumbnailYuv420);
    u4TmpAddr += VENC_H264_THUMBNAIL_YUV420_SIZE;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, VENC_H264_BUF_ALIGN_MASK);

    pCtrl->u4ThumbnailARGB = u4TmpAddr;
    LOG(1,"u4ThumbnailARGB = 0x%x\n",pCtrl->u4ThumbnailARGB);
    u4TmpAddr += VENC_H264_THUMBNAIL_ARGB_SIZE;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, VENC_H264_BUF_ALIGN_MASK);

    //AUD Nalu buffer
    pCtrl->u4AudNaluBufSa = u4TmpAddr;
    LOG(1,"u4AudNaluBufSa = 0x%x\n",pCtrl->u4AudNaluBufSa);
    pCtrl->u4AudNaluBufWp = u4TmpAddr;
    pCtrl->u4AudNaluBufSz = VENC_H264_AUD_NALU_BUF_SIZE;
    u4TmpAddr += VENC_H264_AUD_NALU_BUF_SIZE;
    pCtrl->u4AudNaluBufEa = u4TmpAddr;
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, VENC_H264_BUF_ALIGN_MASK);
    #endif
    
    //output bitstream buf
    for (u4Idx = 0; u4Idx < VENC_RAVEN_BSBUF_CNT; u4Idx ++)
    {
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 1023);
        pCtrl->rHwBuf.u4OutputBuf[u4Idx] = u4TmpAddr;
        u4TmpAddr += VENC_H264_BS_SIZE;
        pCtrl->rHwBuf.u4OutputBuf[u4Idx] = (pCtrl->rHwBuf.u4OutputBuf[u4Idx] + 127) / 128 * 128;
        LOG(1,"OutputBuf[%d] = 0x%x\n",u4Idx,pCtrl->rHwBuf.u4OutputBuf[u4Idx]);
        pCtrl->rBsBufStatus[u4Idx] = BS_BUF_EMPTY;
    }
    
    // ram code buf
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 127);
    pCtrl->rHwBuf.u4RamCodeBuf = u4TmpAddr;
    LOG(1,"u4RamCodeBuf = 0x%x\n",pCtrl->rHwBuf.u4RamCodeBuf);
    u4TmpAddr += VENC_RAVEN_RAMCODE_BUF_SIZE;
    
    //rc info buf
    u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 127);
    pCtrl->rHwBuf.u4RCInfoBuf = u4TmpAddr;
    LOG(1,"u4RCInfoBuf = 0x%x\n",pCtrl->rHwBuf.u4RCInfoBuf);
    u4TmpAddr += VENC_RAVEN_RCINFO_BUF_SIZE;

    LOG(1,"[xiaolei]: buf end = 0x%x\n",u4TmpAddr);
    //check if buffer is enough
    if (u4TmpAddr > (this->u4BuffAddr + this->u4BuffSize))
    {
        LOG(0, "Need extra %d bytes memory for venc support\n", u4TmpAddr - (this->u4BuffAddr + this->u4BuffSize));
        ASSERT(u4TmpAddr <= (this->u4BuffAddr + this->u4BuffSize));
        u4Ret = VENC_NO_MEMORY;
    }

    this->u4UsedSize = u4TmpAddr - this->u4BuffAddr;

    _H264_FUNC_LEAVE();

    return u4Ret;
}

static UINT32 _H264MidInit(VENC_OBJECT_T *this)
{
    BOOL fgRet = TRUE;
    #ifndef VENC_DISABLE_MID

    H264_CTRL_T * pCtrl;
    MID_AGENT_ID_T eAgentId;
    UINT32 u4AgentList;
    UINT32 u4LowAddr, u4HighAddr;
    UINT32 u4Region;

    _H264_FUNC_ENTER();

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);


    fgRet &= MID_Init();
    fgRet &= MID_Reset();

    eAgentId = MID_AGENT_NONE;
    u4AgentList = 0;
    u4Region = 0;

    // Protect VENC not to intrude other's memory
    do
    {
        u4LowAddr  = pCtrl->rHwBuf.u4RamCodeBuf;
        u4HighAddr = pCtrl->rHwBuf.u4RamCodeBuf + VENC_RAVEN_RAMCODE_BUF_SIZE;
        if (u4LowAddr < u4HighAddr)
        {
            eAgentId = MID_AGENT_CPU;
            fgRet &= MID_SetRegion(u4Region, eAgentId, u4LowAddr, u4HighAddr);
            fgRet &= MID_EnableRegionProtect(u4Region);
            _H264_DBG("Region %d: %08x~%08x %s\n", u4Region, u4LowAddr, u4HighAddr, (fgRet)? "ok" : "NG");
            if (++u4Region >= MID_MAX_REGIONS)
            {
                _H264_DBG("_VENC_MidSetting: Mid regions are exhausted\n");
                break;
            }
        }
    }while (0);

    _H264_FUNC_LEAVE();

    #endif /*VENC_DISABLE_MID*/

    UNUSED(this);

    return fgRet ? VENC_OK : VENC_FAIL;

}
static UINT32 _H264VarInit(VENC_OBJECT_T *this)
{
    H264_CTRL_T * pCtrl     = (H264_CTRL_T * )this->hCtrl;
    H264_MMR_PARAM_T * pMMR = (H264_MMR_PARAM_T *)this->hParam;

    if (!pCtrl->u1RepeatHeader)
    {
        pCtrl->u1RepeatHeader = 1;  //ONCE
    }
    pCtrl->fgWriteSEI     = (pMMR->write_pic_timing)? TRUE : FALSE;
    pCtrl->u2IDRFrmPeriod = pMMR->mmr_period_idr_frm_10;
    pCtrl->u2IFrmPeriod   = pMMR->mmr_period_i_frm_10;

    return VENC_OK;
}

//i add this function for "u2DesWidth" "u2DesHeidht"   xiaolei
static UINT32 _H264CalcDesParm(VENC_OBJECT_T *this) 
{
    UINT32 u4BufWidth, u4BufHeight;
    H264_CTRL_T * pCtrl     = (H264_CTRL_T * )this->hCtrl;
    H264_MMR_PARAM_T * pMMR = (H264_MMR_PARAM_T *)this->hParam;

    u4BufWidth = VENC_ALIGN_MASK(this->u2Width, 15);
    if (pMMR->mmr_use_mbaff_1)
    {
        u4BufHeight = VENC_ALIGN_MASK(this->u2Height, 31);
    }
    else
    {
        u4BufHeight = VENC_ALIGN_MASK(this->u2Height, 15);
    }

    pCtrl->u2DesWidth  = ((u4BufWidth >>2) >> 4) << 4;
    pCtrl->u2DesHeidht = VENC_ALIGN_MASK((u4BufHeight >>2), 7);
    pCtrl->u2DesPitch  = VENC_ALIGN_MASK(pCtrl->u2DesWidth, 15);//pCtrl->u2DesWidth;

    return VENC_OK;

}

static UINT32 _H264MmrMECfg(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T * pMMR = (H264_MMR_PARAM_T *)this->hParam;
    UINT32 u4Val;

    _H264_FUNC_ENTER();

#if defined(__MODEL_slt__) // SLT should enable all ME function
    UNUSED(u4Val);
    pMMR->mmr_en_b_inter16x08_1 = 1;
    pMMR->mmr_en_b_inter08x16_1 = 1;
    pMMR->mmr_en_b_inter08x08_1 = 1;
#else
    if (pMMR->mmr_num_of_b_frm_2)
    {
        //ME
        u4Val  = VENC_ALIGN_MASK(this->u2Width, 15) / 16;
        u4Val *= VENC_ALIGN_MASK(this->u2Height, 31) / 32 * 2;
        //u4NumMB
        ASSERT(u4Val);
        //u4CyclePerMB
        u4Val = VENC_H264_HW_CYCLE / (this->u1Framerate * u4Val);
        if (u4Val > VENC_H264_BME11111)
        {
            pMMR->mmr_en_b_inter16x08_1 = 1;
            pMMR->mmr_en_b_inter08x16_1 = 1;
            pMMR->mmr_en_b_inter08x08_1 = 1;
        }
        else if (u4Val > VENC_H264_BME11101)
        {
            pMMR->mmr_en_b_inter16x08_1 = 1;
            pMMR->mmr_en_b_inter08x16_1 = 0;
            pMMR->mmr_en_b_inter08x08_1 = 1;
        }
        else if (u4Val > VENC_H264_BME11001)
        {
            pMMR->mmr_en_b_inter16x08_1 = 0;
            pMMR->mmr_en_b_inter08x16_1 = 0;
            pMMR->mmr_en_b_inter08x08_1 = 1;
        }
        else if (u4Val > VENC_H264_BME11000)
        {
            pMMR->mmr_en_b_inter16x08_1 = 0;
            pMMR->mmr_en_b_inter08x16_1 = 0;
            pMMR->mmr_en_b_inter08x08_1 = 0;
        }
        else
        {
            LOG(3, "%dx%d, %d fps might be over spec.\nMaybe not to encode B frame\n",
                this->u2Width, this->u2Height, this->u1Framerate);
            pMMR->mmr_en_b_inter16x08_1 = 0;
            pMMR->mmr_en_b_inter08x16_1 = 0;
            pMMR->mmr_en_b_inter08x08_1 = 0;
        }
    }
#endif

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

static UINT32 _H264MmrImageCfg(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T * pMMR = (H264_MMR_PARAM_T *)this->hParam;
    UINT32 u4BufWidth, u4BufHeight, u4MinCompRatio;
    UINT32 u4Val;

    _H264_FUNC_ENTER();

    //IMAGE
    u4BufWidth = VENC_ALIGN_MASK(this->u2Width, 15);
    if (pMMR->mmr_use_mbaff_1)
    {
        u4BufHeight = VENC_ALIGN_MASK(this->u2Height, 31);
    }
    else
    {
        u4BufHeight = VENC_ALIGN_MASK(this->u2Height, 15);
    }
    
    pMMR->mmr_image_width_div16_7  = (u4BufWidth  >> 4);
    pMMR->mmr_image_height_div16_6 = (u4BufHeight >> 4);
    pMMR->mmr_image_crop_right_4   = u4BufWidth  - this->u2Width;
    pMMR->mmr_image_crop_bottom_5  = u4BufHeight - this->u2Height;

    u4Val = pMMR->mmr_image_width_div16_7 * pMMR->mmr_image_height_div16_6;
    pMMR->mmr_num_mb_in_frm_12 = u4Val;

    switch(pMMR->mmr_h264_level_6)
    {
    default:
        u4MinCompRatio = 2;
        break;
    case 31:
    case 32:
    case 40:
        u4MinCompRatio = 4;
        break;
    }

    pMMR->mmr_max_pic_bitstream_byte_cnt_22 = u4BufWidth * VENC_ALIGN_MASK(this->u2Height, 15) * 3 / 2 / u4MinCompRatio;
    pMMR->mmr_max_pic_bitstream_byte_cnt_22 -= pMMR->mmr_max_pic_bitstream_byte_cnt_22 % (pMMR->mmr_dram_page_size_div256_7 << 8);

    _H264_FUNC_LEAVE();

    return VENC_OK;
}


static UINT32 _H264MMRInit(VENC_OBJECT_T *this)
{
    H264_CTRL_T * pCtrl     = (H264_CTRL_T * )this->hCtrl;
    H264_MMR_PARAM_T * pMMR = (H264_MMR_PARAM_T *)this->hParam;

    _H264_FUNC_ENTER();

    //emu flow
    if (pCtrl->fgEMUDirectMode)
    {
        pCtrl->u2DesWidth  = pMMR->mmr_coarse_image_width_div8_6  << 3;
        pCtrl->u2DesHeidht = pMMR->mmr_coarse_image_height_div8_5 <<3;
        pCtrl->u2DesPitch  = VENC_ALIGN_MASK(pCtrl->u2DesWidth, 15);

        return VENC_OK;
    }

    //input source format
    pMMR->mmr_source_scan_type_2  = 0;
    pMMR->mmr_pitch_width_in_pels = this->u2Pitch;
    if (VENC_PIX_FMT_B16X32 == this->ePixFmt)
    {
        pMMR->mmr_source_scan_type_2  = 1;
    }
    else if (VENC_PIX_FMT_B64X32 == this->ePixFmt)
    {
        pMMR->mmr_source_scan_type_2  = 2;
    }

    //encode parameters
    _H264MmrMECfg(this);

    //image part
    _H264MmrImageCfg(this);

    //bitrate
    pMMR->mmr_rc_target_bit_rate_14 = this->u4Bitrate;
    pMMR->mmr_fps_8 = this->u1Framerate;

    #ifndef __MODEL_slt__
    pMMR->mmr_vui_timing_info_present_flag = 1;
    pMMR->mmr_vui_num_units_in_tick = 1;
    /* Although spec says fps = time_scale / num_units_in_tick,
     * most available content use fps = time_scale / num_units_in_tick / 2
     * That why here use time_scale = fps * num_units_in_tick * 2
     */
    pMMR->mmr_vui_time_scale =
        this->u1Framerate * pMMR->mmr_vui_num_units_in_tick * 2;
    #else
    pMMR->mmr_vui_timing_info_present_flag = 0;
    pMMR->mmr_vui_num_units_in_tick = 0x3e8;
    pMMR->mmr_vui_time_scale = 0xea60;
    #endif

#ifndef VENC_EMU
    if (_fgFixQp)
    {
        pMMR->mmr_use_rc_1 = 0;
    }
    else
    {
        #ifndef __MODEL_slt__
        pMMR->mmr_use_rc_1 = 1;
        #else
        pMMR->mmr_use_rc_1 = 0;
        #endif
    }
#endif

#ifdef __MODEL_slt__
    pMMR->mmr_crc_check_enable_1 = 1;
#endif

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

//SPS FUNCTION
UINT32 _H264GenSPS_SW(VENC_OBJECT_T *this, UINT8 *pbBuff, UINT32 *pu4Size)
{
    #define SPS_PUT_BIT(name, val, n) PUT_BIT(val,n)
    #define SPS_PUT_BIT_UE(name, val) PUT_BIT_UE(val)
    #define SPS_PUT_BIT_SE(name, val) PUT_BIT_SE(val)

    UINT32 u4Temp;
    UINT32 buff_size = 0;
    UINT8  profile_idc = 0;
    UINT8  pic_order_cnt_type = 0;
    UINT8  frame_mbs_only_flag = 0;
    UINT8  frame_cropping_flag = 0;
    UINT8  vui_parameters_present_flag = 0;
    H264_MMR_PARAM_T *pMMR = NULL;
    PUT_BIT_START(pbBuff);

    _H264_FUNC_ENTER();

    ASSERT(this && pbBuff && pu4Size);

    if (!this || !pbBuff || !pu4Size)
    {
        return VENC_INV_ARG;
    }

    pMMR = (H264_MMR_PARAM_T *)(this->hParam);

    frame_mbs_only_flag = (pMMR->mmr_use_mbaff_1)? 0 : 1;
    if (pMMR->mmr_image_crop_right_4 > 0 || pMMR->mmr_image_crop_bottom_5 > 0)
    {
        frame_cropping_flag = 1;
    }
    else
    {
        frame_cropping_flag = 0;
    }

    //Start code
    SPS_PUT_BIT("start code", 0x00000001, 32);


    //NAL UNIT
    SPS_PUT_BIT("forbidden_zero_bit", 0, 1);
    SPS_PUT_BIT("nal_ref_idc", 3, 2);
    SPS_PUT_BIT("nal_unit_type", 7, 5);


    //Sequence parameter set RBSP
    profile_idc = pMMR->mmr_h264_profile_8;
    SPS_PUT_BIT("profile_idc", profile_idc, 8);

    SPS_PUT_BIT("constraint_set0_flag", 0, 1);
    SPS_PUT_BIT("constraint_set1_flag", 0, 1);
    SPS_PUT_BIT("constraint_set2_flag", 0, 1);
    SPS_PUT_BIT("constraint_set3_flag", 0, 1);

    SPS_PUT_BIT("reserved_zero_4bits /* equal to 0 */", 0, 4);

    SPS_PUT_BIT("level_idc", pMMR->mmr_h264_level_6, 8);

    SPS_PUT_BIT_UE("seq_parameter_set_id (ue(v))", 0);
    if (profile_idc == 100 || profile_idc == 110 ||
        profile_idc == 122 || profile_idc == 144)
    {
        UINT8 chroma_format_idc = 1; // 4:2:0
        UINT8 seq_scaling_matrix_present_flag = 0;
        SPS_PUT_BIT_UE("chroma_format_idc (ue(v))", chroma_format_idc);
        if( chroma_format_idc == 3 )
        {
            SPS_PUT_BIT("residual_colour_transform_flag", 0, 1);
        }

        SPS_PUT_BIT_UE("bit_depth_luma_minus8 (ue(v))", 0);
        SPS_PUT_BIT_UE("bit_depth_chroma_minus8 (ue(v))", 0);
        SPS_PUT_BIT("qpprime_y_zero_transform_bypass_flag", 0, 1);
        SPS_PUT_BIT("seq_scaling_matrix_present_flag", seq_scaling_matrix_present_flag, 1);
        if (seq_scaling_matrix_present_flag)
        {
            UINT8 i;
            UINT8 seq_scaling_list_present_flag[8] ={0};
            for (i = 0; i < 8; i++)
            {
                if (seq_scaling_list_present_flag[i])
                {
                    if (i < 6)
                    {
                        //scaling_list( ScalingList4x4[ i ], 16,UseDefaultScalingMatrix4x4Flag[ i ])
                    }
                    else
                    {
                        //scaling_list( ScalingList8x8[ i ?6 ], 64,UseDefaultScalingMatrix8x8Flag[ i ?6 ] )
                    }
                }
            }
        }
    }


    SPS_PUT_BIT_UE("log2_max_frame_num_minus4 (ue(v))", 12);/*hard code 12*/
    SPS_PUT_BIT_UE("pic_order_cnt_type (ue(v))", pic_order_cnt_type);        /*hard code 0*/

    if (pic_order_cnt_type == 0)
    {
        SPS_PUT_BIT_UE("log2_max_pic_order_cnt_lsb_minus4 (ue(v))", 12); /*hard code 12*/
    }
    else if (pic_order_cnt_type == 1)
    {
        UINT8 num_ref_frames_in_pic_order_cnt_cycle = 0;
        UINT8 i;
        SPS_PUT_BIT("delta_pic_order_always_zero_flag ", 0, 1);
        SPS_PUT_BIT_SE("offset_for_non_ref_pic se(v) ", 0);
        SPS_PUT_BIT_SE("offset_for_top_to_bottom_field se(v) ", 0);
        SPS_PUT_BIT_SE("num_ref_frames_in_pic_order_cnt_cycle se(v) ", num_ref_frames_in_pic_order_cnt_cycle);
        for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
        {
            SPS_PUT_BIT_SE("offset_for_ref_frame[i] se(v) ", 0);
        }
    }

    SPS_PUT_BIT_UE("num_ref_frames (ue(v))", 2);
    SPS_PUT_BIT("gaps_in_frame_num_value_allowed_flag", 0, 1);

    u4Temp = (pMMR->mmr_image_width_div16_7 - 1);
    //Printf("mbs_width: %d\n", u4Temp);
    SPS_PUT_BIT_UE("pic_width_in_mbs_minus1 (ue(v))", u4Temp);

    u4Temp = ((pMMR->mmr_image_height_div16_6 / (2 - frame_mbs_only_flag)) - 1);
    //Printf("height_mbs: %d\n", u4Temp);
    SPS_PUT_BIT_UE("pic_height_in_map_units_minus1 (ue(v))", u4Temp);
    SPS_PUT_BIT("frame_mbs_only_flag", frame_mbs_only_flag, 1);
    if (!frame_mbs_only_flag)
    {
        SPS_PUT_BIT("mb_adaptive_frame_field_flag", 1, 1);
    }

    SPS_PUT_BIT("direct_8x8_inference_flag", 1, 1);  //hard code
    SPS_PUT_BIT("frame_cropping_flag", frame_cropping_flag, 1);
    if (frame_cropping_flag)
    {
        SPS_PUT_BIT_UE("frame_crop_left_offset (ue(v))", 0);
        SPS_PUT_BIT_UE("frame_crop_right_offset (ue(v))", pMMR->mmr_image_crop_right_4);
        SPS_PUT_BIT_UE("frame_crop_top_offset (ue(v))", 0);
        SPS_PUT_BIT_UE("frame_crop_bottom_offset (ue(v))", pMMR->mmr_image_crop_bottom_5);
    }

    {
    //vui_parameter()
    vui_parameters_present_flag = aspect_ratio_info_present_flag |
                                  overscan_info_present_flag|
                                  video_signal_type_present_flag|
                                  chroma_loc_info_present_flag|
                                  timing_info_present_flag|
                                  nal_hrd_parameters_present_flag|
                                  vcl_hrd_parameters_present_flag|
                                  pic_struct_present_flag|
                                  bitstream_restriction_flag;

    SPS_PUT_BIT("vui_parameters_present_flag", vui_parameters_present_flag, 1);
    if (vui_parameters_present_flag)
    {
        SPS_PUT_BIT("aspect_ratio_info_present_flag", aspect_ratio_info_present_flag, 1);
        if( aspect_ratio_info_present_flag )
        {
            UINT8 aspect_ratio_idc = 1;
            SPS_PUT_BIT("aspect_ratio_idc", aspect_ratio_idc, 8);
            if (aspect_ratio_idc == Extended_SAR)
            {
                SPS_PUT_BIT("sar_width", 1, 16);
                SPS_PUT_BIT("sar_height", 1, 16);
            }
        }
        SPS_PUT_BIT("overscan_info_present_flag", overscan_info_present_flag, 1);
        if( overscan_info_present_flag )
        {
            SPS_PUT_BIT("overscan_appropriate_flag", 0, 1);
        }

        SPS_PUT_BIT("video_signal_type_present_flag", video_signal_type_present_flag, 1);
        if( video_signal_type_present_flag )
        {
            UINT8 video_format = 5;
            UINT8 colour_description_present_flag = 1;
            SPS_PUT_BIT("video_format", video_format, 3);
            SPS_PUT_BIT("video_full_range_flag", 0, 1);
            SPS_PUT_BIT("colour_description_present_flag", colour_description_present_flag, 1);
            if( colour_description_present_flag )
            {
                SPS_PUT_BIT("colour_primaries", 6, 8);
                SPS_PUT_BIT("transfer_characteristics", 6, 8);
                SPS_PUT_BIT("matrix_coefficients", 6, 8);
            }
        }

        SPS_PUT_BIT("chroma_loc_info_present_flag", chroma_loc_info_present_flag, 1);
        if( chroma_loc_info_present_flag )
        {
            SPS_PUT_BIT_UE("chroma_sample_loc_type_top_field ue(v)", 0);
            SPS_PUT_BIT_UE("chroma_sample_loc_type_bottom_field ue(v)", 0);
        }

        SPS_PUT_BIT("timing_info_present_flag", timing_info_present_flag, 1);
        if( timing_info_present_flag )
        {
            UINT32 scale;
            scale = this->u1Framerate * 1000 * 2;
            SPS_PUT_BIT("num_units_in_tick ", 1000, 32);
            SPS_PUT_BIT("time_scale", scale, 32);
            SPS_PUT_BIT("fixed_frame_rate_flag", 0, 1);
        }

        SPS_PUT_BIT("nal_hrd_parameters_present_flag", nal_hrd_parameters_present_flag, 1);
        if( nal_hrd_parameters_present_flag )
        {
            UINT8 SchedSelIdx;
            SPS_PUT_BIT_UE("cpb_cnt_minus1", cpb_cnt_minus1);
            SPS_PUT_BIT("bit_rate_scale", bit_rate_scale, 4);
            SPS_PUT_BIT("cpb_size_scale", cpb_size_scale, 4);
            for (SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++)
            {
                SPS_PUT_BIT_UE("bit_rate_value_minus1", ((14000 * 1200) >> (bit_rate_scale + 6)) - 1); // hardcode for level 3.1
                SPS_PUT_BIT_UE("cpb_size_value_minus1", ((14000 * 1200) >> (cpb_size_scale + 4)) - 1); // hardcode for level 3.1
                SPS_PUT_BIT("cbr_flag", 0, 1);
            }
            SPS_PUT_BIT("inital_cpb_removal_delay_length_minus1", 0, 5);
            SPS_PUT_BIT("cpb_removal_delay_length_minus1", cpb_removal_delay_length_minus1, 5);
            SPS_PUT_BIT("dpb_output_delay_length_minus1", dpb_output_delay_length_minus1, 5);
            SPS_PUT_BIT("time_offset_length", 0, 5);
        }
        SPS_PUT_BIT("vcl_hrd_parameters_present_flag", vcl_hrd_parameters_present_flag, 1);
        if( vcl_hrd_parameters_present_flag )
        {
            UINT8 SchedSelIdx;
            SPS_PUT_BIT_UE("cpb_cnt_minus1", cpb_cnt_minus1);
            SPS_PUT_BIT("bit_rate_scale", bit_rate_scale, 4);
            SPS_PUT_BIT("cpb_size_scale", cpb_size_scale, 4);
            for (SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++)
            {
                SPS_PUT_BIT_UE("bit_rate_value_minus1", ((14000 * 1000) >> (bit_rate_scale + 6)) - 1); // hardcode for level 3.1
                SPS_PUT_BIT_UE("cpb_size_value_minus1", ((14000 * 1000) >> (cpb_size_scale + 4)) - 1); // hardcode for level 3.1
                SPS_PUT_BIT("cbr_flag", 0, 1);
            }
            SPS_PUT_BIT("inital_cpb_removal_delay_length_minus1", 0, 5);
            SPS_PUT_BIT("cpb_removal_delay_length_minus1", cpb_removal_delay_length_minus1, 5);
            SPS_PUT_BIT("dpb_output_delay_length_minus1", dpb_output_delay_length_minus1, 5);
            SPS_PUT_BIT("time_offset_length", 0, 5);
        }
        if( nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag )
        {
            SPS_PUT_BIT("low_delay_hrd_flag", 0, 1);
        }

        SPS_PUT_BIT("pic_struct_present_flag", pic_struct_present_flag, 1);
        SPS_PUT_BIT("bitstream_restriction_flag", bitstream_restriction_flag, 1);
        if( bitstream_restriction_flag )
        {
            SPS_PUT_BIT("motion_vectors_over_pic_boundaries_flag", 0, 1);
            SPS_PUT_BIT_UE("max_bytes_per_pic_denom ue(v)", 0);
            SPS_PUT_BIT_UE("max_bits_per_mb_denom ue(v)", 0);
            SPS_PUT_BIT_UE("log2_max_mv_length_horizontal ue(v)", 0);
            SPS_PUT_BIT_UE("log2_max_mv_length_vertical ue(v)", 0);
            SPS_PUT_BIT_UE("num_reorder_frames ue(v)", 0);
            SPS_PUT_BIT_UE("max_dec_frame_buffering ue(v)", 0);
        }

    }
    }
    //rbsp_tailing_bits()
    SPS_PUT_BIT("rbsp_tailing_bits", 1,1);
    ALIGN_BIT();

    PUT_BIT_END(buff_size);

    *pu4Size = buff_size;

    return VENC_OK;

}


//SEI FUNCTION
static VOID _H264SEIPayloadPIC_TIMING(VENC_OBJECT_T *this, VENC_BITS_BUFFER *PBITS_BUFFER, UINT32 *pu4Size, UINT8 mask)
{
    UINT32 size = 0;
    H264_MMR_PARAM_T *pMMR = NULL;

    _H264_FUNC_ENTER();

    ASSERT(this);

    pMMR = (H264_MMR_PARAM_T *)(this->hParam);

    if ( CpbDpbDelaysPresentFlag )
    {
        SEI_PUT_BIT("cpb_removal_delay", 0, cpb_removal_delay_length_minus1 + 1);
        SEI_PUT_BIT("dpb_output_delay", 0, dpb_output_delay_length_minus1 + 1);
    }

    if ( pic_struct_present_flag )
    {
        UINT8 clock_timestamp_flag[3];
        UINT8 NumClockTS;
        UINT8 time_offset_length = 0;
        UINT8 pic_struct;
        UINT8 i;

        pic_struct = pMMR->picture_display_type;

        // Table D-1
        SEI_PUT_BIT("pic_struct", pic_struct, 4);
        switch(pic_struct)
        {
        case 0:
        default:
            NumClockTS = 1;
            // frame
            break;
        case 1:
            //top field
        case 2:
            //bottom field
            NumClockTS = 1;
            break;
        case 3:
            //top filed , bottom field, in that order
        case 4:
            //bottom field, top filed , in that order
            NumClockTS = 2;
            break;
        case 5:
            //top filed , bottom field,  top filed repeat, in that order
        case 6:
            //bottom field, top filed , bottom field repeat, in that order
            NumClockTS = 3;
            break;
        case 7:
            //frame double
            NumClockTS = 2;
            break;
        case 8:
            //frame tripling
            NumClockTS = 3;
            break;
        };
        for (i = 0; i < NumClockTS ; i++ )
        {
            clock_timestamp_flag[i] = 0;
            SEI_PUT_BIT("clock_timestamp_flag[ i ]", clock_timestamp_flag[i], 1);
            if ( clock_timestamp_flag[i] )
            {
                UINT8 full_timestamp_flag = 0;
                SEI_PUT_BIT("ct_type", 1, 2);
                SEI_PUT_BIT("nuit_field_based_flag", 0, 1);
                SEI_PUT_BIT("counting_type", 2, 5);
                SEI_PUT_BIT("full_timestamp_flag", full_timestamp_flag, 1);
                SEI_PUT_BIT("discontinuity_flag", 0, 1);
                SEI_PUT_BIT("cnt_dropped_flag", 1, 1);
                SEI_PUT_BIT("n_frames", 1, 8);
                if ( full_timestamp_flag )
                {
                    SEI_PUT_BIT("seconds_value /* 0..59 */", 0, 6);
                    SEI_PUT_BIT("minutes_value /* 0..59 */", 0, 6);
                    SEI_PUT_BIT("hours_value /* 0..23 */", 0, 5);
                }
                else
                {
                    UINT8 seconds_flag = 0;
                    SEI_PUT_BIT("seconds_flag", seconds_flag, 1);
                    if ( seconds_flag )
                    {
                        UINT8 minutes_flag = 0;
                        SEI_PUT_BIT("seconds_value /* range 0..59 */", 0, 6);
                        SEI_PUT_BIT("minutes_flag ", minutes_flag, 1);
                        if ( minutes_flag )
                        {
                            UINT8 hours_flag = 0;
                            SEI_PUT_BIT("minutes_value /* 0..59 */", 0, 6);
                            SEI_PUT_BIT("hours_flag ", 0, 1);
                            if( hours_flag )
                                SEI_PUT_BIT("hours_value /* 0..23 */", 0, 5);
                        }
                    }
                }

                if( time_offset_length > 0 )
                {
                    SEI_PUT_BIT("time_offset", 0, 1);
                }
            }
        }
    }

    //rbsp
    SEI_PUT_BIT("first bit 1", 1, 1);
    SEI_ALIGN_BIT();

    if (mask&SEI_FUNC_MARK_CALC_SIZE) *pu4Size = (size + 7)/8;

}


static UINT32 _H264GenSEI_SW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    UINT8 *pbBuff = (UINT8 *)(*pu4Addr);
    PUT_BIT_START(pbBuff);

    _H264_FUNC_ENTER();

    ASSERT(this /*&& pu4Addr -- check in PUT_BIT_START */ && pu4Size);

    //Start code
    PUT_BIT(0x00000001, 32);

    //NAL UNIT
    PUT_BIT(0, 1); //"forbidden_zero_bit"
    PUT_BIT(0, 2); //"nal_ref_idc",
    PUT_BIT(6, 5); //"nal_unit_type",

    /*SEI struct
       sei_rbsp( ) { C Descriptor
           do
               sei_message( )
           while( more_rbsp_data( ) )
           rbsp_trailing_bits( )
       }
       */
    //if (PIC_TIMING)
    PUT_SEI_MESSAGE(this,PIC_TIMING);

    PUT_BIT_END(*pu4Size);

    return VENC_OK;

}

static UINT32 _H264GenAUD_SW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    UINT8 *pbBuff = (UINT8 *)(*pu4Addr);
    H264_CTRL_T *pCtrl;
    PUT_BIT_START(pbBuff);

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    //LOG(2,"_H264GenAUD_SW\n");

    _H264_FUNC_ENTER();

    ASSERT(this /*&& pu4Addr -- check in PUT_BIT_START  && pu4Size*/);
    //LOG(2,"_H264GenAUD_SW start put bit\n");
    //Start code
    PUT_BIT(0x00000001, 32);
    //LOG(2,"_H264GenAUD_SW set start code\n");

    //NAL UNIT
    PUT_BIT(0, 1); //"forbidden_zero_bit"
    PUT_BIT(0, 2); //"nal_ref_idc",
    PUT_BIT(9, 5); //"nal_unit_type",
    //LOG(2,"_H264GenAUD_SW set nal unit\n");
    //LOG(2,"pCtrl->eType is :%d\n",pCtrl->eType);
    if((pCtrl->eType == VENC_H264_PIC_TYPE_I) ||(pCtrl->eType == VENC_H264_PIC_TYPE_IDR))
    {
        PUT_BIT(0, 3); //"primary_pic_type",
        LOG(2,"Gen AUD, primary picture type is I!\n");
    }
    else if(pCtrl->eType == VENC_H264_PIC_TYPE_P)
    {
        PUT_BIT(1, 3); //"primary_pic_type",
        LOG(2,"Gen AUD, primary picture type is P!\n");
    }
    else if(pCtrl->eType== VENC_H264_PIC_TYPE_B)
    {
        PUT_BIT(2, 3); //"primary_pic_type",
        LOG(2,"Gen AUD, primary picture type is B!\n");
    }
    PUT_BIT(1, 1);  //"trailing_bits"
    //PUT_BIT(0, 4);
    PUT_BIT_END(*pu4Size);

    return VENC_OK;

}

UINT32 _H264GenPPS_SW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    _H264_FUNC_ENTER();

    UNUSED(pu4Addr);
    UNUSED(pu4Size);

    return VENC_OK;
}

static UINT32 _H264AddAUD(VENC_OBJECT_T *this, UINT32* u4SwOutAddr, UINT32* u4SwOutSize)
{
        H264_CTRL_T *pCtrl;
        UINT32 u4AudOutAddr = 0;
        UINT32 u4OutSize = 0;

        pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

        u4VENC_H264GENPPS_HW(this, &u4AudOutAddr, &u4OutSize);
        
        LOG(2,"_H264AddAUD Addr :0x%x, size :0x%x\n",u4AudOutAddr,u4OutSize );
        *u4SwOutSize = u4OutSize;
        *u4SwOutAddr = u4AudOutAddr;

        return VENC_OK;
}

#ifdef INPUT_BUF_FROM_SCALER
UINT32 _VEncH264TriggerScaler(INT32 i4Argc, const CHAR ** szArgv)
{
    BOOL fgOnOff;

    fgOnOff = (BOOL)StrToInt(szArgv[1]);
    
    if(fgOnOff)
    {
        fgTriggerScaler = TRUE;
        LOG(0, "Enable Scaler to send input buffer\n");
    }
    else
    {
        fgTriggerScaler = FALSE;
        LOG(0, "Disable Scaler to send input buffer\n");
    }

    return TRUE;
}
#endif

static void vVenc_SetBufStatus(H264_CTRL_T *pCtrl, UINT32 u4Addr, VENC_BS_BUF_STATUS rBsBufStatus)
{
    int i;
    for(i = 0; i < VENC_RAVEN_BSBUF_CNT; i++)
    {
        if(u4Addr == pCtrl->rHwBuf.u4OutputBuf[i])
        {
            LOG(7, "set buf[%d] addr 0x%x BufStatus to %d\n", i, u4Addr, rBsBufStatus);
            pCtrl->rBsBufStatus[i] = rBsBufStatus;
            break;
        }
    }

    if (VENC_RAVEN_BSBUF_CNT == i)
    {
        ASSERT(0);
    }
}

static VOID vVenc_H264_LoadRCCode(VENC_OBJECT_T *this)
{
#if 0
    H264_CTRL_T *pCtrl;
    UINT32 *pu4RCA;
    UINT32 u4j, u4i;
    UINT32 u4Val;

    _HAL_FUNC_ENTER();

    ASSERT(this);

    LOG(3, "load RC: %s\n", RC_ROM_CODE_FILE);
    pCtrl = (H264_CTRL_T *)this->hCtrl;

    pu4RCA = (UINT32 *)VIRTUAL(pCtrl->rHwBuf.u4RamCodeBuf);
    pCtrl->u4RCUAddr = pCtrl->rHwBuf.u4RamCodeBuf;
    
    pCtrl->u4RCUCodeSize = UCODE_LENGTH_ADD8;
    for (u4j = u4i = 0; (u4i >> 1) < (UCODE_LENGTH_ADD8); u4i += 2, u4j++)
    {
        u4Val = _u4DefaultRCRomCode[u4j];
        *(pu4RCA + u4i + 1) = ((u4Val & 0x000000FF) << 24)
            | ((u4Val & 0x0000FF00) << 8)
            | ((u4Val & 0x00FF0000) >> 8)
            | ((u4Val & 0xFF000000) >> 24);  
    }

    HalFlushInvalidateDCacheMultipleLine((UINT32)pu4RCA, UCODE_LENGTH_ADD8 * 8);

#else
    
    H264_CTRL_T *pCtrl;
    UINT32 *pu4RCA;
    UINT32 u4i;
    UINT32 u4Val;
        
    _HAL_FUNC_ENTER();

    ASSERT(this);

    LOG(3, "load RC: %s\n", RC_ROM_CODE_FILE);
    pCtrl = (H264_CTRL_T *)this->hCtrl;

    pu4RCA = (UINT32 *)VIRTUAL(pCtrl->rHwBuf.u4RamCodeBuf);
    pCtrl->u4RCUAddr = pCtrl->rHwBuf.u4RamCodeBuf;
    
    pCtrl->u4RCUCodeSize = UCODE_LENGTH_ADD8;
    //for (u4j = u4i = 0; (u4i >> 1) < (UCODE_LENGTH_ADD8); u4i += 2, u4j++)
    //{
    //    u4Val = _u4DefaultRCRomCode[u4j];
    //    *(pu4RCA + u4i + 1) = ((u4Val & 0x000000FF) << 24)
    //        | ((u4Val & 0x0000FF00) << 8)
    //        | ((u4Val & 0x00FF0000) >> 8)
    //        | ((u4Val & 0xFF000000) >> 24);  
    //}
    for(u4i = 0; u4i < UCODE_LENGTH_ADD8; u4i++)
    {
        u4Val = _u4DefaultRCRomCode[u4i];
        *(pu4RCA + u4i) = u4Val;
    }
    //LOG(0, "RC CODE = 0x%x 0x%x 0x%x\n", *(pu4RCA+5), *(pu4RCA+6), *(pu4RCA+7));

    HalFlushInvalidateDCacheMultipleLine((UINT32)pu4RCA, UCODE_LENGTH_ADD8 * 8);

    i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);

    LOG(3, "set RC buf done , addr 0x%x\n", pCtrl->rHwBuf.u4RamCodeBuf);
#endif
    _HAL_FUNC_LEAVE();

}

static void vVenc_SetVUI(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    //H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RAVEN_VUI_PARAM_T  rVUIParam;

    _H264_FUNC_ENTER();
    
    x_memset(&rVUIParam, 0, sizeof(VENC_RAVEN_VUI_PARAM_T));

    #if 0  //previous IC setting
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1EnableVUISupport = pMMR->mmr_enable_vui_support_1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_info_present_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_overscan_info_present_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_signal_type_present_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_location_info_present_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_timing_info_present_flag = pMMR->mmr_vui_timing_info_present_flag;
    if(pMMR->mmr_vui_timing_info_present_flag)
    {
        rVUIParam.u4VUI_num_units_in_tick = pMMR->mmr_vui_num_units_in_tick;
        rVUIParam.u4VUI_time_scale = pMMR->mmr_vui_time_scale;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_fixed_frame_rate_flag = 0;        
    }
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_nal_hrd_parameters_present_flag = pMMR->write_pic_timing;
    if(pMMR->write_pic_timing)
    {
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_bit_rate_scale = 4;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_size_scale = 6;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_bit_rate_value_minus1 = 16405;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_cpb_size_value_minus1 = 16405;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_vbr_cbr_flag = 0;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_initial_cpb_removal_delay_length_minus1 = 0;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_removal_delay_length_minus1 = 7;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_dpb_output_delay_length_minus1 = 7;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_time_offset_length = 0;
    }
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_vcl_hrd_parameters_present_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_low_delay_hrd_flag = 0;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_pic_struct_present_flag = pMMR->vui_pic_struct_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_bitstream_restriction_flag = 0;

    #else
    
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1EnableVUISupport = pMMR->mmr_enable_vui_support_1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_idc = pMMR->mmr_vui_aspect_ratio_idc;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_info_present_flag = pMMR->mmr_vui_aspect_ratio_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_bitstream_restriction_flag = 0; //pMMR->mmr_vui_bitstream_restriction_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_location_info_present_flag = 0; //pMMR->mmr_vui_chroma_location_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_sample_loc_type_bottom_field = 0; //pMMR->mmr_vui_chroma_sample_loc_type_bottom_field;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_chroma_sample_loc_type_top_field = 0; //pMMR->mmr_vui_chroma_sample_loc_type_top_field;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_colour_description_present_flag = 0; //pMMR->mmr_vui_colour_description_present_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_low_delay_hrd_flag = 0; //pMMR->mmr_vui_low_delay_hrd_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_nal_hrd_parameters_present_flag = pMMR->write_pic_timing; //pMMR->mmr_vui_nal_hrd_parameters_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_overscan_appropriate_flag = 0; //pMMR->mmr_vui_overscan_appropriate_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_overscan_info_present_flag = 0; //pMMR->mmr_vui_overscan_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_pic_struct_present_flag = pMMR->vui_pic_struct_present_flag; //pMMR->mmr_vui_pic_struct_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_timing_info_present_flag = pMMR->mmr_vui_timing_info_present_flag; //pMMR->mmr_vui_timing_info_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_vcl_hrd_parameters_present_flag = 0; //pMMR->mmr_vui_vcl_hrd_parameters_present_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_format = 5; //pMMR->mmr_vui_video_format;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_full_range_flag = 0; //pMMR->mmr_vui_video_full_range_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_video_signal_type_present_flag = 0; //pMMR->mmr_vui_video_signal_type_present_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_colour_primaries = 2; //pMMR->mmr_vui_colour_primaries;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_matrix_coefficients = 2; //pMMR->mmr_vui_matrix_coefficients;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_1_T.u1VUI_transfer_characteristics = 2; //pMMR->mmr_vui_transfer_characteristics;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_height = pMMR->mmr_vui_sar_height;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_width = pMMR->mmr_vui_sar_width;

    if(pMMR->write_pic_timing)
    {
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_bit_rate_scale = 4; //pMMR->mmr_vui_nal_bit_rate_scale;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_removal_delay_length_minus1 = 0x7; //pMMR->mmr_vui_nal_cpb_removal_delay_length_minus1;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_cpb_size_scale = 6; //pMMR->mmr_vui_nal_cpb_size_scale;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_dpb_output_delay_length_minus1 = 0x7; //pMMR->mmr_vui_nal_dpb_output_delay_length_minus1;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_initial_cpb_removal_delay_length_minus1 = 0; //pMMR->mmr_vui_nal_initial_cpb_removal_delay_length_minus1;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_time_offset_length = 0; //pMMR->mmr_vui_nal_time_offset_length;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_5_T.u1VUI_nal_vbr_cbr_flag = 0; //pMMR->mmr_vui_nal_vbr_cbr_flag;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_bit_rate_value_minus1 = 16405; //pMMR->mmr_vui_nal_bit_rate_value_minus1;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_6_T.u4VUI_nal_cpb_size_value_minus1 = 16405; //pMMR->mmr_vui_nal_cpb_size_value_minus1;
    }

    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_bit_rate_scale = 0; //pMMR->mmr_vui_vcl_bit_rate_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_cpb_removal_delay_length_minus1 = 0x17; //pMMR->mmr_vui_vcl_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_cpb_size_scale = 0; //pMMR->mmr_vui_vcl_cpb_size_scale;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_dpb_output_delay_length_minus1 = 0x17; //pMMR->mmr_vui_vcl_dpb_output_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_initial_cpb_removal_delay_length_minus1 = 0x17; //pMMR->mmr_vui_vcl_initial_cpb_removal_delay_length_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_time_offset_length = 0x18; //pMMR->mmr_vui_vcl_time_offset_length;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_7_T.u1VUI_vcl_vbr_cbr_flag = 0; //pMMR->mmr_vui_vcl_vbr_cbr_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_8_T.u4VUI_vcl_bit_rate_value_minus1 = 0; //pMMR->mmr_vui_vcl_bit_rate_value_minus1;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_8_T.u4VUI_vcl_cpb_size_value_minus1 = 0; //pMMR->mmr_vui_vcl_cpb_size_value_minus1;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_log2_max_mv_length_horizontal = 0x10; //pMMR->mmr_vui_log2_max_mv_length_horizontal;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_log2_max_mv_length_vertical = 0x10; //pMMR->mmr_vui_log2_max_mv_length_vertical;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_bits_per_mb_denom = 0; //pMMR->mmr_vui_max_bits_per_mb_denom;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_bytes_per_pic_denom = 0; //pMMR->mmr_vui_max_bytes_per_pic_denom;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_max_dec_frame_buffering = 0x10; //pMMR->mmr_vui_max_dec_frame_buffering;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_motion_vectors_over_pic_boundaries_flag = 1; //pMMR->mmr_vui_motion_vectors_over_pic_boundaries_flag;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_9_T.u1VUI_num_reorder_frames = 0x10; //pMMR->mmr_vui_num_reorder_frames;

    if(pMMR->mmr_vui_timing_info_present_flag)
    {
        rVUIParam.u4VUI_num_units_in_tick = pMMR->mmr_vui_num_units_in_tick; //0x3e8; //pMMR->mmr_vui_num_units_in_tick;
        rVUIParam.u4VUI_time_scale = pMMR->mmr_vui_time_scale; //0xea60; //pMMR->mmr_vui_time_scale;
        rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_fixed_frame_rate_flag = 0; //pMMR->mmr_vui_fixed_frame_rate_flag;
    }
    #endif

    i4Venc_Hal_VUISet(&rVUIParam);

    _H264_FUNC_LEAVE();
}

static void vVenc_SetAspectRatioIdc(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    //H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RAVEN_VUI_PARAM_T  rVUIParam;

    _H264_FUNC_ENTER();
    
    x_memset(&rVUIParam, 0, sizeof(VENC_RAVEN_VUI_PARAM_T));

    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_idc = pMMR->mmr_vui_aspect_ratio_idc;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_0_T.u1VUI_aspect_ratio_info_present_flag = pMMR->mmr_vui_aspect_ratio_info_present_flag;

    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_height = pMMR->mmr_vui_sar_height;
    rVUIParam.rVENC_VUI_PARAMETER_INFO_2_T.u4VUI_sar_width = pMMR->mmr_vui_sar_width;

    i4Venc_Hal_SetAspectRatioIdc(&rVUIParam);

    _H264_FUNC_LEAVE();
}

static void vVenc_SeqMmrSet(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    //H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RAVEN_SEQ_MMR_PARAM_T  rSeqMmr;

    _H264_FUNC_ENTER();
    
    x_memset(&rSeqMmr, 0, sizeof(VENC_RAVEN_SEQ_MMR_PARAM_T));

    rSeqMmr.rVENC_ENCODER_INFO_0.u1NUM_B_FRM = pMMR->mmr_num_of_b_frm_2;
    rSeqMmr.rVENC_ENCODER_INFO_0.u1QP_I_FRM = pMMR->mmr_quant_i_frm_6;
    rSeqMmr.rVENC_ENCODER_INFO_0.u4PERIOD_IDR_FRM = pMMR->mmr_period_idr_frm_10;
    rSeqMmr.rVENC_ENCODER_INFO_0.u4PERIOD_I_FRM = pMMR->mmr_period_i_frm_10;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_08X08 = pMMR->mmr_en_p_inter08x08_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_08X16 = pMMR->mmr_en_p_inter08x16_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_16X08 = pMMR->mmr_en_p_inter16x08_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_16X16 = pMMR->mmr_en_p_inter16x16_1;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_SEARCH_H = (pMMR->mmr_p_sr_h_div16_3) << 4;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1P_SEARCH_V = (pMMR->mmr_p_sr_v_div16_2) << 4;
    rSeqMmr.rVENC_ENCODER_INFO_1.u1QP_P_FRM = pMMR->mmr_quant_p_frm_6;

    if(0 == pMMR->mmr_h264_profile_8) //baseline profile
    {
        pMMR->mmr_use_cabac_1 = 0; // these two syntax can be set to 1, only if profile is main or high.
        pMMR->mmr_use_mbaff_1 = 0;
    }
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1CABAC = pMMR->mmr_use_cabac_1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1H264_LEVEL = pMMR->mmr_h264_level_6;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1MBAFF = pMMR->mmr_use_mbaff_1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1PROFILE = pMMR->mmr_h264_profile_8;

    rSeqMmr.rVENC_IMAGE_INFO_0.u4IMAGE_HEIGHT_DIV16 = (pMMR->mmr_image_height_div16_6) << 4;
    rSeqMmr.rVENC_IMAGE_INFO_0.u4IMAGE_WIDTH_DIV16 = (pMMR->mmr_image_width_div16_7) << 4;
    rSeqMmr.rVENC_IMAGE_INFO_0.u2IMAGE_NUM_MB = pMMR->mmr_num_mb_in_frm_12;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1IMAGE_CROP_BOTTOM = pMMR->mmr_image_crop_bottom_5;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1IMAGE_CROP_RIGHT = pMMR->mmr_image_crop_right_4;
    rSeqMmr.rVENC_IMAGE_INFO_1.u4pitch_width_div16 = pMMR->mmr_pitch_width_in_pels;
    rSeqMmr.rVENC_IMAGE_INFO_1.u1source_scan_type = pMMR->mmr_source_scan_type_2;
    rSeqMmr.rVENC_IMAGE_INFO_1.u4uv_pitch_width_div16 = pMMR->mmr_pitch_width_in_pels;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u1RC_CBR = pMMR->mmr_rc_use_cbr_1;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u1RC_INI_QP = pMMR->mmr_rc_use_init_qp_1;
    rSeqMmr.rVENC_RATECONTROL_INFO_0.u4RC_TARGET_BIT_RATE = pMMR->mmr_rc_target_bit_rate_14;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1AFPS = pMMR->mmr_rc_adaptive_fps;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1ATBR = pMMR->mmr_rc_adaptive_tbr;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1BfrmQLimter = pMMR->mmr_b_frm_q_limiter_8;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1PfrmQLimter = pMMR->mmr_p_frm_q_limiter_8;
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1RC_FPS = pMMR->mmr_fps_8;
    
    rSeqMmr.rVENC_AVSR_ANRP_INFO_1.u1ANRP_ENABLE = 0;

    // some special configuration
    if(2 == pMMR->mmr_h264_profile_8)
    {
       pMMR->mmr_use_8x8Tr = 1; //as raven function description says, use_8*8tr can only be set to 1 when profile is high.
    }

    pMMR->mmr_rdO_interTr4_mode = 4;
    pMMR->mmr_rdO_interTr8_mode = 0;
    pMMR->mmr_rdO_intra4x4_mode = 4;
    pMMR->mmr_rdO_intra8x8_mode = 0;
    pMMR->mmr_rdO_bdirect_enable = 1;
    pMMR->mmr_rdO_intra16_enable = 1;
    pMMR->mmr_rdO_skip_enable = 1;
    
    if(1 == pMMR->mmr_use_8x8Tr)
    {
        pMMR->mmr_rdO_interTr8_mode = 4;
        pMMR->mmr_rdO_intra8x8_mode = 4;
    }
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_BDIRECTEnable = pMMR->mmr_rdO_bdirect_enable;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_InterTr4Mode = pMMR->mmr_rdO_interTr4_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_InterTr8Mode = pMMR->mmr_rdO_interTr8_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_Intra16Enable = pMMR->mmr_rdO_intra16_enable;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_IntraTr4Mode = pMMR->mmr_rdO_intra4x4_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_IntraTr8Mode = pMMR->mmr_rdO_intra8x8_mode;
    rSeqMmr.rVENC_ENCODER_INFO_2.u1RDO_SkipEnable = pMMR->mmr_rdO_skip_enable;

    pMMR->mmr_slice_alpha_c0_offset_div2 = 0; //default 0
    pMMR->mmr_slice_beta_offset_div2 = 0; //default 0
    
    rSeqMmr.rVENC_H264_ENC_INFO_0.u4SLICE_ALPHA_C0_OFFSET_DIV2 = (pMMR->mmr_slice_alpha_c0_offset_div2) << 1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u4SLICE_BETA_C0_OFFSET_DIV2 = (pMMR->mmr_slice_beta_offset_div2) << 1;
    rSeqMmr.rVENC_H264_ENC_INFO_0.u1USE_TR8X8 = pMMR->mmr_use_8x8Tr;

    pMMR->mmr_skype_mode = 0; // temp setting. to do detail config later
    rSeqMmr.rVENC_RATECONTROL_INFO_1.u1SKYPE_MODE = pMMR->mmr_skype_mode;

    pMMR->mmr_avsr_enable = 0; // temp setting. to do detail config later
    rSeqMmr.rVENC_AVSR_ANRP_INFO_1.u1AVSR_ENABLE = pMMR->mmr_avsr_enable;
    
    i4Venc_Hal_SeqMmrSet(&rSeqMmr);

    _H264_FUNC_LEAVE();
}

static void vVenc_FrmMmrSet(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    UINT32 u4FrmIdx = pCtrl->u4FrameNo;
    
    VENC_RAVEN_FRM_MMR_PARAM_T  rFrmMmr;

    _H264_FUNC_ENTER();
    
    x_memset(&rFrmMmr, 0, sizeof(VENC_RAVEN_FRM_MMR_PARAM_T));
    
    rFrmMmr.rVENC_ENCODER_INFO_3.u1CHANGE_B_2_NONREF_P = 0;             //V
    rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SIGN = 0;                       //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1GEN_REC_FRM = 1;                     //V
    rFrmMmr.rVENC_ENCODER_INFO_3.u1IDR_PIC_ID = 0;                      //?
    
    pMMR->mmr_refine_ment_mode = 1; //temp setting . to do detail configuration later
    rFrmMmr.rVENC_ENCODER_INFO_3.u1IME_REFINE_MODE = pMMR->mmr_refine_ment_mode;                 //?
   
    rFrmMmr.rVENC_ENCODER_INFO_3.u1INTRA_WEIGHT_COST_EN = 1;            //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1PFRM_NONREF_P = 0;                   //?
    rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_PDIR = 0;                       //?
    if (pMMR->mmr_period_i_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMVSEARCH_EN = (1 >= u4FrmIdx % pMMR->mmr_period_i_frm_10)?0:1;  //?
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SCALER = (u4FrmIdx % pMMR->mmr_period_i_frm_10)?0:6;   //?
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMVSEARCH_EN = (1 >= u4FrmIdx)?0:1;  //?
        rFrmMmr.rVENC_ENCODER_INFO_3.u1COMV_SCALER = (1 > u4FrmIdx)?6:0;   //?
    }
    
    if (pMMR->mmr_period_idr_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1IS_IDR_FRM = (u4FrmIdx % pMMR->mmr_period_idr_frm_10)?0:1;  //V
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1IS_IDR_FRM = (1 > u4FrmIdx)?1:0;  //V
    }
    
    if (pMMR->mmr_period_i_frm_10)
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_IMG_TYPE = (u4FrmIdx % pMMR->mmr_period_i_frm_10)?0:2; //V
    }
    else
    {
        rFrmMmr.rVENC_ENCODER_INFO_3.u1VENC_IMG_TYPE = (1 > u4FrmIdx)?2:0; //V
    }

    rFrmMmr.rVENC_RATECONTROL_INFO_0.u1RC_CBR = pMMR->mmr_rc_use_cbr_1;
    rFrmMmr.rVENC_RATECONTROL_INFO_0.u1RC_INI_QP = pMMR->mmr_rc_use_init_qp_1;
    rFrmMmr.rVENC_RATECONTROL_INFO_0.u4RC_TARGET_BIT_RATE = pMMR->mmr_rc_target_bit_rate_14;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1AFPS = pMMR->mmr_rc_adaptive_fps;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1AIFI = _prVencVfyInfo->rMmr.;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ATBR = pMMR->mmr_rc_adaptive_tbr;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1BfrmQLimter = pMMR->mmr_b_frm_q_limiter_8;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_EIS = _prVencVfyInfo->rMmr.;
    //rFrmMmr.rVENC_RATECONTROL_INFO_1.u1ENABLE_ROI = _prVencVfyInfo->rMmr.;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1PfrmQLimter = pMMR->mmr_p_frm_q_limiter_8;
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1RC_FPS = pMMR->mmr_fps_8;

    pMMR->mmr_skype_mode = 0; // temp setting. to do detail config later
    rFrmMmr.rVENC_RATECONTROL_INFO_1.u1SKYPE_MODE = pMMR->mmr_skype_mode;
    
    rFrmMmr.u4RcInfoAddr = pCtrl->rHwBuf.u4RCInfoBuf;
    rFrmMmr.u4ColLoadAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4ColMvInfoBuf[0]:pCtrl->rHwBuf.au4ColMvInfoBuf[1];
    rFrmMmr.u4ColSaveAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4ColMvInfoBuf[1]:pCtrl->rHwBuf.au4ColMvInfoBuf[0];

    rFrmMmr.u4FrmCurYAddr = pCtrl->rHwBuf.u4InputBufY; 
    rFrmMmr.u4FrmCurCAddr = pCtrl->rHwBuf.u4InputBufC; 

    LOG(1, "FRM MMR Y add 0x%x, C add 0x%x\n", rFrmMmr.u4FrmCurYAddr, rFrmMmr.u4FrmCurCAddr);
    
    if (3 == pMMR->mmr_source_scan_type_2)
    {
        rFrmMmr.u4FrmCurVAddr = pCtrl->rHwBuf.u4InputBufV;
    }
    
    rFrmMmr.u4FrmRecYAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4SrcBufY[0]:pCtrl->rHwBuf.au4SrcBufY[1];
    rFrmMmr.u4FrmRecCAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4SrcBufC[0]:pCtrl->rHwBuf.au4SrcBufC[1];
    
    rFrmMmr.u4FrmRefYAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4SrcBufY[1]:pCtrl->rHwBuf.au4SrcBufY[0];
    rFrmMmr.u4FrmRefCAddr = (u4FrmIdx % 2)?pCtrl->rHwBuf.au4SrcBufC[1]:pCtrl->rHwBuf.au4SrcBufC[0];

    rFrmMmr.u4FrmCnt = u4FrmIdx;
    rFrmMmr.u4FrmIdx = u4FrmIdx;
    if (pMMR->mmr_period_idr_frm_10)
    {
        rFrmMmr.u4LastIDRFrmIdx = 
            (u4FrmIdx % pMMR->mmr_period_idr_frm_10)?(u4FrmIdx / pMMR->mmr_period_idr_frm_10 * pMMR->mmr_period_idr_frm_10):u4FrmIdx;
    }
    else
    {
        rFrmMmr.u4LastIDRFrmIdx = 0;
    }

    i4Venc_Hal_FrmMmrSet(&rFrmMmr);

    _H264_FUNC_LEAVE();
}

static UINT32 u4VENC_H264GENSPS_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    VENC_RAVEN_ENC_PARAM_T rEncParam;
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RESULT_INFO_T  rEncResultInfo = {0};
    VENC_RAVEN_CRC_T    rCrc = {0};
    UINT32 i, j, u4temp = 0;

    _H264_FUNC_ENTER();

    for(j = 0; ; j++)  //temp solution
    {
        for(i = 0; i < VENC_RAVEN_BSBUF_CNT; i++)
        {
            if(BS_BUF_EMPTY == pCtrl->rBsBufStatus[i])
            {
                rEncParam.u4OutputBufAddr = pCtrl->rHwBuf.u4OutputBuf[i];
                rEncParam.u4OutputBufSize = VENC_H264_BS_SIZE;
                pCtrl->rBsBufStatus[i] = BS_BUF_ENCODING;
                u4temp = 1;
                break;
            }
        }
        if(1 == u4temp)
        {
            break;
        }

        x_thread_delay(1);
    }
    
    if(0 == u4temp)
    {
        LOG(1, "!!!!!!!!!!!!Warning: BS buffers are all being used now[SPS]!!!!!!!!!!!!\n");
    }
    HalInvalidateDCacheMultipleLine(VIRTUAL(rEncParam.u4OutputBufAddr), rEncParam.u4OutputBufSize);
    
    if(pCtrl->fg1stHeader)
        rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 1;
    else
        rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1; //set 0 if wanna save power
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = pMMR->mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;

    //photo mode is only set to 1 when encoder is used to support multi-frame application
    pMMR->mmr_photo_mode = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = pMMR->mmr_photo_mode;

    pMMR->mmr_bitstream_format_1 = 0; // video format = (vp8)?2:0
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = pMMR->mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;
        
    i4Venc_Hal_Enc(VENC_RAVEN_ENC_SPS, &rEncParam);
    
    i4Venc_Hal_GetResult(&rEncResultInfo);
    i4Venc_Hal_GetCrc(&rCrc);

    #ifdef __MODEL_slt__
    vVenc_CompareCrc(1, &rCrc);
    if (0 == (_u4CompareErr&0x10))
    {
        LOG(1, "[raven] compare SPS Crc pass\n");
    }
    else
    {
        LOG(1, "[raven] compare SPS Crc fail\n");
    }
    #endif

    *pu4Addr = rEncResultInfo.u4BitstreamStartAddr;
    *pu4Size = rEncResultInfo.u4BitstreamByteCnt;

    //debug info
    pCtrl->u4NextCpSa = *pu4Addr ;
    x_memcpy(&pCtrl->rCrc, &rCrc, sizeof(VENC_RAVEN_CRC_T));

    _H264_FUNC_LEAVE();
    return VENC_OK;
}

static UINT32 u4VENC_H264GENPPS_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    VENC_RAVEN_ENC_PARAM_T rEncParam;
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RESULT_INFO_T  rEncResultInfo = {0};
    VENC_RAVEN_CRC_T    rCrc = {0};
    UINT32 i, j, u4temp = 0;

    _H264_FUNC_ENTER();

    for(j = 0; ; j++) //temp solution
    {
        for(i = 0; i < VENC_RAVEN_BSBUF_CNT; i++)
        {
            if(BS_BUF_EMPTY == pCtrl->rBsBufStatus[i])
            {
                rEncParam.u4OutputBufAddr = pCtrl->rHwBuf.u4OutputBuf[i];
                rEncParam.u4OutputBufSize = VENC_H264_BS_SIZE;
                pCtrl->rBsBufStatus[i] = BS_BUF_ENCODING;
                u4temp = 1;
                break;
            }
        }
        if(1 == u4temp)
        {
            break;
        }
        x_thread_delay(1);
    }
    
    if(0 == u4temp)
    {
        LOG(1, "!!!!!!!!!!!!Warning: BS buffers are all being used now[PPS]!!!!!!!!!!!!\n");
    }
    HalInvalidateDCacheMultipleLine(VIRTUAL(rEncParam.u4OutputBufAddr), rEncParam.u4OutputBufSize);

    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1; //set 0 if wanna save power
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = pMMR->mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;

    //photo mode is only set to 1 when encoder is used to support multi-frame application
    pMMR->mmr_photo_mode = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = pMMR->mmr_photo_mode;

    pMMR->mmr_bitstream_format_1 = 0; // video format = (vp8)?2:0
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = pMMR->mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;
    
    i4Venc_Hal_Enc(VENC_RAVEN_ENC_PPS, &rEncParam);
    
    i4Venc_Hal_GetResult(&rEncResultInfo);
    i4Venc_Hal_GetCrc(&rCrc);

    #ifdef __MODEL_slt__
    vVenc_CompareCrc(1, &rCrc);
    if (0 == (_u4CompareErr&0x10))
    {
        LOG(1, "[raven] compare PPS Crc pass\n");
    }
    else
    {
        LOG(1, "[raven] compare PPS Crc fail\n");
    }
    #endif

    *pu4Addr = rEncResultInfo.u4BitstreamStartAddr;
    *pu4Size = rEncResultInfo.u4BitstreamByteCnt;

    //debug info
    pCtrl->u4NextCpSa = *pu4Addr ;
    x_memcpy(&pCtrl->rCrc, &rCrc, sizeof(VENC_RAVEN_CRC_T));

    _H264_FUNC_LEAVE();
    return VENC_OK;
}

#if VENC_DUMP_DRAM
#define VENC_POSIX_MEM_SIZE     1024

static UINT32 _u4PosixMem = 0;
static UINT32 _u4EncNum = 0;

VOID vVenc_IntToStr(UINT32 u4Int, UCHAR* pucStr)
{
    UINT32 u4A = 0;
    UINT32 u4B = 0;

    u4A = u4Int / 1000;
    u4B = u4Int % 1000;
    pucStr[0] = u4A + 0x30;

    u4A = u4B / 100;
    u4B = u4B % 100;
    pucStr[1] = u4A + 0x30;

    u4A = u4B / 10;
    u4B = u4B % 10;
    pucStr[2] = u4A + 0x30;

    pucStr[3] = u4B + 0x30;
}

#define VENC_DUMP_ONE_PATH  1

VOID vVenc_DumpDram(UINT32 u4Addr, UINT32 u4Size, UINT32 u4FrameNum)
{
#if !(VENC_DUMP_ONE_PATH)
    UINT32 u4Idx = 0;
#endif
    UINT32 u4TtNum = 0;
    UINT32 u4WriteCnt = 0;
    UCHAR ucStr[5] = {0};
    struct file *filep;
    mm_segment_t oldfs;
    
#if (VENC_DUMP_ONE_PATH)
    UCHAR *pcFileName = "/mnt/usbdisk/rc_info_frm0000.raw";
#else
    UCHAR *pcFileName[7] = {"/mnt/usb/sda1/rc_info_frm0000.raw",
                            "/mnt/usb/sdb1/rc_info_frm0000.raw",
                            "/mnt/usb/sdc1/rc_info_frm0000.raw",
                            "/mnt/usb/sda/rc_info_frm0000.raw",
                            "/mnt/usb/sdb/rc_info_frm0000.raw",
                            "/mnt/usb/sdc/rc_info_frm0000.raw",
                            "/mnt/sdcard/rc_info_frm0000.raw"};*/
#endif

    u4TtNum = _u4EncNum;
    _u4EncNum ++;

    vVenc_IntToStr(u4TtNum, ucStr);
    LOG(1, "[VENC-DUMP] IntToStr Int %d Str %s \n", u4TtNum, ucStr);

#if (VENC_DUMP_ONE_PATH)
    x_memcpy((pcFileName + 24), ucStr, 4);
#else
    x_memcpy((pcFileName[0] + 25), ucStr, 4);
    x_memcpy((pcFileName[1] + 25), ucStr, 4);
    x_memcpy((pcFileName[2] + 25), ucStr, 4);
    x_memcpy((pcFileName[3] + 24), ucStr, 4);
    x_memcpy((pcFileName[4] + 24), ucStr, 4);
    x_memcpy((pcFileName[5] + 24), ucStr, 4);
    x_memcpy((pcFileName[6] + 23), ucStr, 4);
#endif

    if (200 < u4FrameNum)
    {
        LOG(0, "[VENC-DUMP] FrameNum[%d] > 200\n", u4FrameNum);
        
        if (_u4PosixMem)
        {
            x_mem_free((VOID*)_u4PosixMem);
            _u4PosixMem = 0;
        }

        return;
    }

    if (0 == u4TtNum)
    {
        _u4PosixMem = (UINT32)(x_mem_alloc(VENC_POSIX_MEM_SIZE));
        if (0 == _u4PosixMem)
        {
            LOG(0, "[VENC-DUMP] alloc posix mem fail\n");
            ASSERT(0);
        }
        LOG(0, "[VENC-DUMP] alloc posix mem success, addr 0x%x\n", _u4PosixMem);
    }

    HalFlushDCacheMultipleLine(u4Addr, u4Size);

#if (VENC_DUMP_ONE_PATH)
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    filep = filp_open(pcFileName, (O_CREAT | O_WRONLY | O_APPEND), 0);
    if(IS_ERR(filep))
    {
        LOG(1, "[VENC-DUMP] FrameNum[%d] filp_open %s fail\n", u4TtNum, pcFileName);
        set_fs(oldfs);
    }
    else
    {
        LOG(1, "[VENC-DUMP] FrameNum[%d] filp_open %s success\n", u4TtNum, pcFileName);
    }
#else
    do
    {
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        
        filep = filp_open(pcFileName[u4Idx], (O_CREAT | O_WRONLY | O_APPEND), 0);
        if(IS_ERR(filep))
        {
            LOG(1, "[VENC-DUMP] FrameNum[%d] filp_open %s fail\n", u4TtNum, pcFileName[u4Idx]);
            set_fs(oldfs);
        }
        else
        {
            LOG(1, "[VENC-DUMP] FrameNum[%d] filp_open %s success\n", u4TtNum, pcFileName[u4Idx]);
        }
        
        u4Idx ++;
    }while(IS_ERR(filep) && (u4Idx < 7));
#endif

    if (IS_ERR(filep))
    {
    #if (VENC_DUMP_ONE_PATH)
        LOG(0, "[VENC-DUMP] FrameNum[%d] filp_open one path fail\n", u4TtNum);
    #else
        LOG(0, "[VENC-DUMP] FrameNum[%d] filp_open all fail\n", u4TtNum);

        for (u4Idx = 0; u4Idx < 7; u4Idx ++)
        {
            LOG(0, "[VENC-DUMP] FrameNum[%d] filp_open %s check\n", u4TtNum, pcFileName[u4Idx]);
        }
    #endif
    }
    else
    {
        LOG(1, "[VENC-DUMP] FrameNum[%d] start to copy\n", u4TtNum);

        x_memcpy((VOID *)_u4PosixMem, (VOID *)u4Addr, VENC_POSIX_MEM_SIZE);
        u4WriteCnt = filep->f_op->write(filep, (VOID*)_u4PosixMem, VENC_POSIX_MEM_SIZE, &filep->f_pos);
        if (VENC_POSIX_MEM_SIZE != u4WriteCnt)
        {
            LOG(0, "[VENC-DUMP] FrameNum[%d] write active cnt %d\n", u4TtNum, u4WriteCnt);
            ASSERT(0);
        }

        filp_close(filep, 0);
        set_fs(oldfs);
        
        LOG(0, "[VENC-DUMP] ori Tt[%d] Frm[%d] end write, addr 0x%x\n", u4TtNum, u4FrameNum, u4Addr);
    }

    return;
}
#endif

static UINT32 u4VENC_H264GENFRM_HW(VENC_OBJECT_T *this, UINT32 *pu4Addr, UINT32 *pu4Size)
{
    VENC_RAVEN_ENC_PARAM_T rEncParam;
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;
    H264_CTRL_T *pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    VENC_RESULT_INFO_T  rEncResultInfo = {0};
    VENC_RAVEN_CRC_T    rCrc = {0};
    UINT32 i, j, u4temp = 0;

    _H264_FUNC_ENTER();

    for(j = 0; ; j++) //temp solution
    {
        for(i = 0; i < VENC_RAVEN_BSBUF_CNT; i++)
        {
            if(BS_BUF_EMPTY == pCtrl->rBsBufStatus[i])
            {
                rEncParam.u4OutputBufAddr = pCtrl->rHwBuf.u4OutputBuf[i];
                rEncParam.u4OutputBufSize = VENC_H264_BS_SIZE;
                pCtrl->rBsBufStatus[i] = BS_BUF_ENCODING;
                u4temp = 1;
                break;
            }
        }
        if(1 == u4temp)
        {
            break;
        }

        LOG(7, "%s wait output buf\n", __FUNCTION__);

        x_thread_delay(1);
    }
    
    if(0 == u4temp)
    {
        LOG(0, "!!!!!!!!!!!!Warning: BS buffers are all being used now[FRM]!!!!!!!!!!!!\n");
    }
    else
    {
        LOG(7, "%s get output buf[%d] 0x%x\n", __FUNCTION__, i, rEncParam.u4OutputBufAddr);
    }
    HalInvalidateDCacheMultipleLine(VIRTUAL(rEncParam.u4OutputBufAddr), rEncParam.u4OutputBufSize);

    rEncParam.rVENC_HW_MODE_SEL.u1CRC_CTRL_ENABLE = 1; //set 0 if wanna save power
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRAME_UPDATE_RC = pMMR->mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_FRM_RC = pMMR->mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1ENABLE_SEQ_RC = ((1 == pMMR->mmr_use_rc_1)&&(pCtrl->u4FrameNo < 1))?1:0;
    rEncParam.rVENC_HW_MODE_SEL.u1EN_RATECONTROL = pMMR->mmr_use_rc_1;
    rEncParam.rVENC_HW_MODE_SEL.u1First_SPS_Flag = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1insert_keyframe = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_DATA = (pCtrl->u4FrameNo < 1)?0:1;
    rEncParam.rVENC_HW_MODE_SEL.u1SAVE_RC_DATA = 1;
    rEncParam.rVENC_HW_MODE_SEL.u1LOAD_RC_RAM_CODE = ((1 == pMMR->mmr_use_rc_1) && (pCtrl->u4FrameNo < 1))?1:0;
    rEncParam.rVENC_HW_MODE_SEL.u1PAUSE_MODE = 0;

    //photo mode is only set to 1 when encoder is used to support multi-frame application
    pMMR->mmr_photo_mode = 0;
    rEncParam.rVENC_HW_MODE_SEL.u1PHOTO_MODE = pMMR->mmr_photo_mode;

    pMMR->mmr_bitstream_format_1 = 0; // video format = (vp8)?2:0
    rEncParam.rVENC_HW_MODE_SEL.u1VIDEO_FORMAT = pMMR->mmr_bitstream_format_1;
    rEncParam.rVENC_HW_MODE_SEL.u4SMI_GULTRA_CTRL = 0;

    LOG(7, "%s start to enc frm\n", __FUNCTION__);
    i4Venc_Hal_Enc(VENC_RAVEN_ENC_FRM, &rEncParam);
    LOG(7, "%s end enc frm\n", __FUNCTION__);

    #ifdef INPUT_BUF_FROM_SCALER
    if (!this->fgResizeSrc && !this->u4GfxConfig)
    {
        //To avoid define Module name again, apply number directly here
        //E_VENC_MODULE_MAIN_SCALER  (0), E_VENC_MODULE_SUB_SCALER   (1)
        //E_VENC_MODULE_MON_SCALER   (2), E_VENC_MODULE_SUB_DI       (3)
        //E_VENC_MODULE_MAX          (4)
        if((u1DrvScpipGetVencModule() == 1)
           || (u1DrvScpipGetVencModule() == 2))
        {
            H264_CTRL_T *pCtrl;
            pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
            u1DrvScpipH264FreeCurrentFb(pCtrl->rHwBuf.u4InputBufY);
        }
    }
    #endif
    
    i4Venc_Hal_GetResult(&rEncResultInfo);

#if VENC_DUMP_DRAM
    vVenc_DumpDram(VIRTUAL(pCtrl->rHwBuf.u4RCInfoBuf), 1024, pCtrl->u4FrameNo);
#endif

    pCtrl->u4FrameNo++;
    
    i4Venc_Hal_GetCrc(&rCrc);

    #ifdef __MODEL_slt__
    vVenc_CompareCrc(8, &rCrc);
    if (0 == (_u4CompareErr&0x10))
    {
        LOG(1, "[raven] compare FRM[%d] Crc pass\n", (pCtrl->u4FrameNo - 1));
    }
    else
    {
        LOG(1, "[raven] compare FRM[%d] Crc fail\n", (pCtrl->u4FrameNo - 1));
    }
    #endif

    *pu4Addr = rEncResultInfo.u4BitstreamStartAddr;
    *pu4Size = rEncResultInfo.u4BitstreamByteCnt;

    //debug info
    pCtrl->u4NextCpSa = *pu4Addr ;
    x_memcpy(&pCtrl->rCrc, &rCrc, sizeof(VENC_RAVEN_CRC_T));

    _H264_FUNC_LEAVE();
    return VENC_OK;
}

//for async
static void _H264EncRefTypeAnalyse(H264_CTRL_T *pCtrl)
{
    if (pCtrl->u4FrameNo == 0)
    {
        pCtrl->eType = VENC_H264_PIC_TYPE_IDR;
    }
    else if (pCtrl->fgForceIDR)
    {
        pCtrl->eType = VENC_H264_PIC_TYPE_IDR;
        pCtrl->fgForceIDR = FALSE;
        pCtrl->u2IDRFrmIdx = 0;
        pCtrl->u2IFrmIdx   = 0;
    }
    else if (pCtrl->u2IDRFrmPeriod > 0 && pCtrl->u2IDRFrmIdx == 0)
    {
        pCtrl->eType = VENC_H264_PIC_TYPE_IDR;
    }
    else if (pCtrl->u2IFrmPeriod > 0 && pCtrl->u2IFrmIdx == 0)
    {
        pCtrl->eType = VENC_H264_PIC_TYPE_I;
    }
    else
    {
        pCtrl->eType = VENC_H264_PIC_TYPE_P;
    }

    if (pCtrl->u2IDRFrmPeriod > 0)
    {
        pCtrl->u2IDRFrmIdx++;
        if (pCtrl->u2IDRFrmIdx >= pCtrl->u2IDRFrmPeriod)
        {
            pCtrl->u2IDRFrmIdx = 0;
        }
    }

    if (pCtrl->u2IFrmPeriod > 0)
    {
        pCtrl->u2IFrmIdx++;
        if (pCtrl->u2IFrmIdx >= pCtrl->u2IFrmPeriod)
        {
            pCtrl->u2IFrmIdx = 0;
        }
    }
}

//comment from xiaolei for 5399 venc new driver
//i recognize fgReqAddAud FALSE as default
//because just LG uses fgReqAddAud, and Chunyue said
//she would modify fgReqAddAud later.
static UINT32  _H264EncFrameA (VENC_OBJECT_T *this, VENC_IN_PIC_T *pIn, VENC_OUT_PIC_T *pOut, H246_PIC_PRAM* pPram)
{
    UINT32 u4Ret = VENC_OK;
    VENC_IN_PIC_T  *pCurrIn   = pIn;
    UINT32 u4BsBufIdx;
    H264_CTRL_T *pCtrl;
    UINT32 u4OutAddr = 0;
    UINT32 u4AudOutAddr = 0;
    UINT32 u4OutSize = 0;
    UINT32 u4TempOutAddr = 0;
    UINT32 u4AudOutSize = 0;
    UINT32 u4AddAudSize;
    UINT32 u4SwOutAddr;
    UINT32 u4NalAudSize = 6;
    //UINT32 u4Addr = 0;
    //UINT32 u4Size = 0;

    VENC_OUT_PIC_T rOut;
    H264_MMR_PARAM_T* pMMR  = (H264_MMR_PARAM_T *)this->hParam;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this)
    {
        return VENC_INV_ARG;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    
    rOut.u4Size = 0;
    //INIT
    if (pCtrl->fg1stHeader)
    {
        LOG(1, "re-encode sps\n");
        i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);
        if (pMMR->mmr_enable_vui_support_1)
        {
            vVenc_SetVUI(this);
        }
        vVenc_SeqMmrSet(this);
        u4VENC_H264GENSPS_HW(this, &u4OutAddr, &u4OutSize);

        
        rOut.u4RStart = u4OutAddr;
        rOut.u4REnd = u4OutAddr + VENC_H264_BS_SIZE;
        
        if(pCtrl->fgReqAddAud)
        {
            pCtrl->eType = VENC_H264_PIC_TYPE_IDR;
            _H264AddAUD(this, &u4SwOutAddr, &u4AddAudSize);
             //NAL access unite delimiter 6 byte
            ASSERT(u4AddAudSize >= u4NalAudSize);
            u4AudOutAddr = VIRTUAL(pCtrl->u4AudNaluBufWp);
            pCtrl->u4RecordAddr = pCtrl->u4NaluBufWp;
            _H264GenAUD_SW(this, &u4AudOutAddr, &u4AudOutSize);
            u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
            x_memset((VOID *)u4TempOutAddr, 0, u4AudOutSize);
            x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4AudOutAddr, u4AudOutSize);
            UP_SW_NALU_WP(pCtrl, u4AudOutSize);
            pCtrl->u4AudNaluBufWp = pCtrl->u4AudNaluBufSa;
            pCtrl->u4RecordSize = u4AudOutSize;
            LOG(7,"Gen AUD Addr: 0x%x, AUD size:0x%x, aud size:0x%x\n",rOut.u4Addr, u4AudOutSize, pCtrl->u4RecordSize);
        }
        rOut.u4Pts = pIn->u4Pts;
        if(pCtrl->fgReqAddAud)
        {
            i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);
            if (pMMR->mmr_enable_vui_support_1)
            {
                vVenc_SetVUI(this);
            }
            vVenc_SeqMmrSet(this);
            u4VENC_H264GENSPS_HW(this, &u4OutAddr, &u4OutSize);
            
            //copy SPS data to Nalu buffer
            u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
            u4OutAddr = VIRTUAL(u4OutAddr);
            x_memset((VOID *)u4TempOutAddr, 0, u4OutSize);
            x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4OutAddr, u4OutSize);
            UP_SW_NALU_WP(pCtrl, u4OutSize);
            pCtrl->u4RecordSize += u4OutSize;
            LOG(7, "SPS data  Addr:0x%x, Size:0x%x, save sps addr:0x%x, aud + sps :0x%x\n",u4OutAddr,u4OutSize,u4TempOutAddr, pCtrl->u4RecordSize);
        }
        else
        {
            rOut.u4Addr = u4OutAddr;
            rOut.u4Size = u4OutSize;
    
            LOG(7, "SPS data callback Addr:0x%x, Size:0x%x\n",u4OutAddr,u4OutSize);
            if (this->OutputCallback)
            {
                #ifdef __MODEL_slt__
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                        break;
                    }
                }
                this->OutputCallback(this->pvCallbackData, pCurrIn, &rOut, VENC_H264_PIC_TYPE_SPS);
                #else
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_COPYING;
                        break;
                    }
                }
                this->OutputCallback(this->pvCallbackData, NULL, &rOut, VENC_H264_PIC_TYPE_SPS);
                #endif
            }
            else
            {
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                        break;
                    }
                }
            }
        }
        pCtrl->fg1stHeader  = FALSE;
        pCtrl->fgSkipHeaderOnce = TRUE;

        //write pps
         LOG(1, "re-encode pps\n");
        
        u4VENC_H264GENPPS_HW(this, &u4OutAddr, &u4OutSize);

        rOut.u4RStart = u4OutAddr;
        rOut.u4REnd = u4OutAddr + VENC_H264_BS_SIZE;
        
        if(pCtrl->fgReqAddAud)
        {
            ////copy PPS data to Nalu buffer
            u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
            u4OutAddr = VIRTUAL(u4OutAddr);
            x_memset((VOID *)u4TempOutAddr, 0, u4OutSize);
            x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4OutAddr, u4OutSize);
            UP_SW_NALU_WP(pCtrl, u4OutSize);
            pCtrl->u4RecordSize += u4OutSize;
            LOG(7, "PPS data  Addr:0x%x, Size:0x%x, save pps  addr:0x%x, aud + sps + pps :0x%x\n",u4OutAddr,u4OutSize, u4TempOutAddr, pCtrl->u4RecordSize);
        }
        else
        {
            rOut.u4Addr = u4OutAddr;
            rOut.u4Size = u4OutSize;
            
            LOG(7, "PPS data callback Addr:0x%x, Size:0x%x\n",u4OutAddr,u4OutSize);
            if (this->OutputCallback)
            {
                #ifdef __MODEL_slt__
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                        break;
                    }
                }
                this->OutputCallback(this->pvCallbackData, pCurrIn, &rOut, VENC_H264_PIC_TYPE_PPS); 
                #else
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_COPYING;
                        break;
                    }
                }
                this->OutputCallback(this->pvCallbackData, NULL, &rOut, VENC_H264_PIC_TYPE_PPS); 
                #endif
            }
            else
            {
                for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                {
                    if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                       && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                    {
                        pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                        break;
                    }
                }
            }
        }
    }

    //encode seq. 1.IPPPPP
    //                  2.BBIBBPBBPBBP
    //                  3.BIBPBP
    //
    rOut.u4Pts  = pCurrIn->u4Pts;
    //gen ref frame first
    _H264EncRefTypeAnalyse(pCtrl);
    if (VENC_H264_PIC_TYPE_IDR == pCtrl->eType)
    {
        //encoder header
        if (pCtrl->u1RepeatHeader > 0)
        {
            //write sps
            if (pCtrl->fgSkipHeaderOnce)
            {
                pCtrl->fgSkipHeaderOnce = FALSE;
            }
            else
            {
                if(pCtrl->fgReqAddAud)
                {
                    _H264AddAUD(this, &u4SwOutAddr, &u4AddAudSize);
                     //NAL access unite delimiter 6 byte
                    ASSERT(u4AddAudSize >= u4NalAudSize);
                    u4AudOutAddr = VIRTUAL(pCtrl->u4AudNaluBufWp);
                    u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
                    pCtrl->u4RecordAddr = pCtrl->u4NaluBufWp;
                    _H264GenAUD_SW(this, &u4AudOutAddr, &u4AudOutSize);
                    x_memset((VOID *)u4TempOutAddr, 0, u4AudOutSize);
                    x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4AudOutAddr, u4AudOutSize);
                    UP_SW_NALU_WP(pCtrl, u4AudOutSize);
                    HalFlushDCacheMultipleLine(u4TempOutAddr, u4AudOutSize);
                    pCtrl->u4AudNaluBufWp = pCtrl->u4AudNaluBufSa;
                     pCtrl->u4RecordSize += u4AudOutSize;
                    LOG(7,"Gen AUD Addr: 0x%x, AUD size:0x%x, save aud addr:0x%x\n",rOut.u4Addr, u4AudOutSize, u4TempOutAddr);
                }
                 
                LOG(1, "encode sps\n");
                i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);
                if (pMMR->mmr_enable_vui_support_1)
                {
                    vVenc_SetVUI(this);
                }
                vVenc_SeqMmrSet(this);
                u4VENC_H264GENSPS_HW(this, &u4OutAddr, &u4OutSize);

                rOut.u4RStart = u4OutAddr;
                rOut.u4REnd = u4OutAddr + VENC_H264_BS_SIZE;
                
                if(pCtrl->fgReqAddAud)
                {
                     //NAL access unite delimiter 6 byte
                    u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
                    u4OutAddr = VIRTUAL(u4OutAddr);
                    x_memset((VOID *)u4TempOutAddr, 0, u4OutSize);
                    x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4OutAddr, u4OutSize);
                    UP_SW_NALU_WP(pCtrl, u4OutSize);
                    HalFlushDCacheMultipleLine(u4TempOutAddr, u4OutSize);
                    pCtrl->u4RecordSize += u4OutSize;
                    LOG(7, "SPS data  Addr:0x%x, Size:0x%x,save sps addr:0x%x\n",u4OutAddr, u4OutSize, u4TempOutAddr);
                }
                else
                {
                    rOut.u4Addr = u4OutAddr;
                    rOut.u4Size = u4OutSize;
            
                    LOG(7, "SPS data callback Addr:0x%x, Size:0x%x\n",u4OutAddr,u4OutSize);
                    if (this->OutputCallback)
                    {
                        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                        {
                            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                            {
                                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_COPYING;
                                break;
                            }
                        }
                        this->OutputCallback(this->pvCallbackData, NULL, &rOut, VENC_H264_PIC_TYPE_SPS);
                    }
                    else
                    {
                        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                        {
                            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                            {
                                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                                break;
                            }
                        }
                    }
                }
                //write pps
                LOG(1, "encode pps\n");
                u4VENC_H264GENPPS_HW(this, &u4OutAddr, &u4OutSize);

                rOut.u4RStart = u4OutAddr;
                rOut.u4REnd = u4OutAddr + VENC_H264_BS_SIZE;
                if(pCtrl->fgReqAddAud)
                {
                    ////copy PPS data to Nalu buffer
                    u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
                    u4OutAddr = VIRTUAL(u4OutAddr);
                    x_memset((VOID *)u4TempOutAddr, 0, u4OutSize);
                    x_memcpy((VOID *)u4TempOutAddr, (VOID *)u4OutAddr, u4OutSize);
                    UP_SW_NALU_WP(pCtrl, u4OutSize);
                    HalFlushDCacheMultipleLine(u4TempOutAddr, u4OutSize);
                    pCtrl->u4RecordSize += u4OutSize;
                    LOG(7, "PPS data  Addr:0x%x, Size:0x%x, save pps addr:0x%x\n",u4OutAddr,u4OutSize,u4TempOutAddr);
                }
                else
                {
                    rOut.u4Addr = u4OutAddr;
                    rOut.u4Size = u4OutSize;
            
                    LOG(7, "PPS data callback Addr:0x%x, Size:0x%x\n",u4OutAddr,u4OutSize);
                    if (this->OutputCallback)
                    {
                        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                        {
                            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                            {
                                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_COPYING;
                                break;
                            }
                        }
                        this->OutputCallback(this->pvCallbackData, NULL, &rOut, VENC_H264_PIC_TYPE_PPS);
                    }
                    else
                    {
                        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
                        {
                            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
                               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
                            {
                                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                                break;
                            }
                        }
                    }
                }
            }

            //write sei
            if (pCtrl->fgWriteSEI)
            {
#ifndef VENC_DUMP_BS
                VENC_OUT_PIC_T rSeiOut;
                x_memset(&rSeiOut, 0, sizeof(rSeiOut));
                u4OutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
                _H264GenSEI_SW(this, &u4OutAddr, &u4OutSize);
                HalFlushDCacheMultipleLine(u4OutAddr, u4OutSize);
                rSeiOut.u4Pts  = pCurrIn->u4Pts;
                rSeiOut.u4RStart = pCtrl->u4NaluBufSa;
                rSeiOut.u4REnd = pCtrl->u4NaluBufEa;
                rSeiOut.u4Addr = PHYSICAL(u4OutAddr);
                rSeiOut.u4Size = u4OutSize;
                UP_SW_NALU_WP(pCtrl, u4OutSize);
                if (this->OutputCallback)
                {
                    this->OutputCallback(this->pvCallbackData, NULL, &rSeiOut, VENC_H264_PIC_TYPE_SEI);
                }
#else
                LOG(1, "encode sei\n");
                u4OutAddr = VIRTUAL(pCtrl->u4AudNaluBufWp);
                _H264GenSEI_SW(this, &u4OutAddr, &u4OutSize);
                 pCtrl->u4AudNaluBufWp = pCtrl->u4AudNaluBufSa;

                rOut.u4RStart = u4OutAddr;
                rOut.u4REnd = u4OutAddr + u4OutSize;
                
                if(pCtrl->fgReqAddAud)
                {
                    u4TempOutAddr = VIRTUAL(pCtrl->u4NaluBufWp);
                    x_memset(u4TempOutAddr, 0, u4OutSize);
                    x_memcpy(u4TempOutAddr, u4OutAddr, u4OutSize);
                    UP_SW_NALU_WP(pCtrl, u4OutSize);
                    HalFlushDCacheMultipleLine(u4TempOutAddr, u4OutSize);
                    pCtrl->u4RecordSize += u4OutSize;
                    LOG(7, "SEI data Size:0x%x , save sei addr:0x%x, aud+sps+pps+sei :0x%x\n", u4OutSize,u4TempOutAddr, pCtrl->u4RecordSize);
                }
                else
                {
                    #if 0
                    ASSERT(u4OutSize <= rOut.u4Size);
                    x_memset(u4SwOutAddr, 0, rOut.u4Size);
                    x_memcpy(u4SwOutAddr, u4OutAddr, u4OutSize);
                    HalFlushDCacheMultipleLine(u4SwOutAddr, u4OutSize);
                    rOut.u4Size = u4OutSize;
                    #else
                    rOut.u4Addr = pCtrl->u4AudNaluBufWp;
                    rOut.u4Size = VENC_ALIGN_MASK(u4OutSize, 7);
                    #endif
            
                    LOG(7, "SEI data callback Addr:0x%x, Size:0x%x\n",u4OutAddr,u4OutSize);
                    if (this->OutputCallback)
                    {
                        this->OutputCallback(this->pvCallbackData, NULL, &rOut, VENC_H264_PIC_TYPE_SEI);
                    }
                }
#endif
            }

            if (pCtrl->u1RepeatHeader == 1)
            {
                pCtrl->u1RepeatHeader = 0;
            }
        }
    }
    if(pCtrl->fgReqAddAud)
    {
        //access unite delimiter inserted before I and P frame
        if((pCtrl->eType== VENC_H264_PIC_TYPE_I) ||(pCtrl->eType== VENC_H264_PIC_TYPE_P))
        {
             //gen pps for add access unite delimiter
             _H264AddAUD(this, &u4SwOutAddr, &u4AddAudSize);
             LOG(7,"Gen PPS for AUD!\n");
             pCtrl->u4RecordSize = u4NalAudSize;
        }
    }

    LOG(1,"[xiaolei]: gen frm: y 0x%x, c 0x%x\n", pCurrIn->u4YAddr,pCurrIn->u4CAddr);
    pCtrl->rHwBuf.u4InputBufY = pCurrIn->u4YAddr; // set source buffer add
    pCtrl->rHwBuf.u4InputBufC = pCurrIn->u4CAddr; //
    vVenc_FrmMmrSet(this);
    u4VENC_H264GENFRM_HW(this, &u4OutAddr, &u4OutSize);

    rOut.u4RStart = u4OutAddr;
    rOut.u4REnd = u4OutAddr + VENC_H264_BS_SIZE;
    
#ifdef VENC_TIME_PROFILE
#define THRD (180 * 30)
    if (_u4FrmCnt < THRD)
    {
        HAL_GetTime(&_rTimeHwE);
        //LOG(1, "%d %d\n", _rTimeHwE.u4Seconds, _rTimeHwE.u4Micros);
        HAL_GetDeltaTime(&_rTimeHwDt, &pIn->rTimeHwS, &_rTimeHwE);
        //ASSERT(_rTimeHwDt.u4Seconds == 0);
        //LOG(1, "%d\n", _rTimeHwDt.u4Micros);
        _u4FrmCnt++;
        _u4EncTime += _rTimeHwDt.u4Micros;
    }
    else if (_u4FrmCnt == THRD)
    {
        LOG(0, "There're %d frames\n", _u4FrmCnt);
        LOG(0, "Total time is %d us\n", _u4EncTime);
        LOG(0, "Average time is %d us\n", _u4EncTime/_u4FrmCnt);
        _u4FrmCnt++;
    }
#endif
      if(pCtrl->fgReqAddAud)
     {
         //NAL access unite delimiter 6 byte
        //ASSERT(u4AddAudSize >= u4NalAudSize);
        LOG(7,"user data size:0x%x\n",pCtrl->u4RecordSize);
        u4SwOutAddr = u4OutAddr - pCtrl->u4RecordSize;
        rOut.u4Addr = u4SwOutAddr;
        u4SwOutAddr = VIRTUAL(u4SwOutAddr);
        u4TempOutAddr = VIRTUAL(pCtrl->u4RecordAddr);
        if(pCtrl->eType == VENC_H264_PIC_TYPE_IDR)
       {
            LOG(7,"Copy IDR data addr:0x%x\n", pCtrl->u4RecordAddr);
            x_memset((VOID *)u4SwOutAddr, 0, pCtrl->u4RecordSize);
            x_memcpy((VOID *)u4SwOutAddr, (VOID *)u4TempOutAddr, pCtrl->u4RecordSize);
            HalFlushDCacheMultipleLine(u4SwOutAddr, pCtrl->u4RecordSize);
        }
        else
        {
             u4AudOutAddr = VIRTUAL(pCtrl->u4AudNaluBufWp);
             _H264GenAUD_SW(this, &u4AudOutAddr, &u4AudOutSize);
             pCtrl->u4AudNaluBufWp = pCtrl->u4AudNaluBufSa;
            u4SwOutAddr = VIRTUAL(u4SwOutAddr);
             LOG(7,"Gen I P frame aud!\n");
             x_memset((VOID *)u4SwOutAddr, 0, u4AudOutSize);
             x_memcpy((VOID *)u4SwOutAddr, (VOID *)u4AudOutAddr, u4AudOutSize);
             HalFlushDCacheMultipleLine(u4SwOutAddr, u4AudOutSize);
             pCtrl->u4AudNaluBufWp = pCtrl->u4AudNaluBufSa;
         }
         rOut.u4Size = pCtrl->u4RecordSize + u4OutSize;
        LOG(1,"Frame data call back Addr: 0x%x, call back size:0x%x\n",rOut.u4Addr, rOut.u4Size);
     }
     else
     {
          rOut.u4Addr = u4OutAddr;
          rOut.u4Size = u4OutSize;
      }
    if (this->OutputCallback)
    {
        #ifdef __MODEL_slt__
        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
        {
            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
            {
                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                break;
            }
        }
        #else
        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
        {
            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
            {
                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_COPYING;
                break;
            }
        }
        #endif
        this->OutputCallback(this->pvCallbackData, pCurrIn, &rOut, pCtrl->eType);
        this->u4FrameCnt ++;
        pCtrl->u4RecordSize = 0;
    }
    else
    {
        for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
        {
            if((BS_BUF_ENCODING == pCtrl->rBsBufStatus[u4BsBufIdx])
               && (u4OutAddr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
            {
                pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
                break;
            }
        }
    }

    _H264_FUNC_LEAVE();

    return u4Ret;
}

static UINT32 _H264HWReset(VENC_OBJECT_T *this)
{
    H264_CTRL_T *pCtrl;
    H264_MMR_PARAM_T * pMMR;

    UINT32 u4RP, u4WP;

    VENC_RESULT_INFO_T  rEncResultInfo = {0};
    
    ASSERT(this);
    _H264_FUNC_ENTER();

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam, H264_MMR_PARAM_T);

    pCtrl->u1RepeatHeader = pCtrl->u1RepeatHeader ? pCtrl->u1RepeatHeader : 1;

    pCtrl->fg1stHeader    = TRUE;
    pCtrl->u1BSeqIdx      = 0;
    pCtrl->u1RefSeqIdx    = 0;
    pCtrl->u2IDRFrmIdx    = 0;
    pCtrl->u2IFrmIdx      = 0;
    pCtrl->u4FrameNo      = 0;

    //pitch
    pMMR->mmr_pitch_width_in_pels = this->u2Pitch;

    //encode parameters
    _H264MmrMECfg(this);

    //image part
    _H264MmrImageCfg(this);

    //bitrate
    pMMR->mmr_rc_target_bit_rate_14 = this->u4Bitrate;
    pMMR->mmr_fps_8 = this->u1Framerate;
    // .........

    //hw reset
    i4Venc_Hal_GetResult(&rEncResultInfo);
    
    u4WP = rEncResultInfo.u4BitstreamStartAddr;
    u4RP = rEncResultInfo.u4BitstreamByteCnt;

    i4Venc_Hal_Stop();
    i4Venc_Hal_Start();
    //buffer reg setting
    i4Venc_Hal_SetRCInfoBuf(pCtrl->rHwBuf.u4RCInfoBuf);

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

static UINT32 _H264RtReset(VENC_OBJECT_T *this)
{
    H264_MMR_PARAM_T *pMMR;

    ASSERT(this);
    pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam, H264_MMR_PARAM_T);

    //Mmr
    _H264MmrMECfg(this);

    //bitrate
    pMMR->mmr_rc_target_bit_rate_14 = this->u4Bitrate;

    pMMR->mmr_fps_8 = this->u1Framerate;

    return VENC_OK;
}

static UINT32 _H264EncRoutine(VENC_OBJECT_T *this)
{
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    INT32 i4Ret;
    VENC_FRMQ_DATA_T tData;

    _H264_FUNC_ENTER();
    
    zMsgSize = sizeof(VENC_FRMQ_DATA_T);

    i4Ret = x_msg_q_receive(&u2MsgQIdx, (VOID*)&tData, &zMsgSize,
                            &(this->hEncFrmQ), 1, X_MSGQ_OPTION_WAIT);
    if (OSR_OK != i4Ret)
    {
        LOG(1, "%s: fail(%d)\n", __FUNCTION__, i4Ret);
        ASSERT(i4Ret == OSR_OK);
    }

    _H264_DBG("_VENC<H264>::(eType) = >(%d)\n", (UINT32)tData.eType);

    if (VENC_FRMQ_TYPE_FRM == tData.eType)
    {
        _H264EncFrameA(this, &(tData.u.frm.tIn), &(tData.u.frm.tOut), (H246_PIC_PRAM *)&(tData.u.frm.tPram));
    }
    else if (VENC_FRMQ_TYPE_RT_CTRL == tData.eType)
    {
        H264_CTRL_T * pCtrl    = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
        VENC_RT_CTRL_T *ptParm = (VENC_RT_CTRL_T *)&(tData.u.tRTCtrl);
        if (ptParm->u4Flag & VENC_RT_CTRL_FLAG_INSERT_KEYFRAME)
        {
            _H264_DBG("_VENC<H264>:: insert keyframe\n");

            i4Venc_Hw_VENC_InsertKFrame(1);
            
            if (!pCtrl->u1RepeatHeader)
            {
                pCtrl->u1RepeatHeader = 1;  //ONCE
            }
            pCtrl->fgForceIDR = TRUE;
        }
    }
    else if (VENC_FRMQ_TYPE_RT_PARM == tData.eType)
    {
        VENC_RT_PARM_T *ptParm = (VENC_RT_PARM_T *)&(tData.u.tRTParm);
        BOOL fgResetHW = FALSE;
        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_RES)
        {
#if 0 // move to _H264RezRoutine
            this->u2Width  = ptParm->u2Width;
            this->u2Height = ptParm->u2Height;
            this->u2Pitch  = ptParm->u2Pitch;
#endif
            fgResetHW = TRUE;
        }

        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_BITRATE)
        {
            this->u4Bitrate = ptParm->u4Bitrate;
        }

        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_FRAMERATE)
        {
            this->u1Framerate = ptParm->u1Framerate;
        }

        if (fgResetHW)
        {
            _H264HWReset(this);
        }
        else
        {
            _H264RtReset(this);
        }

    }
    else if (VENC_FRMQ_TYPE_DUMMY != tData.eType)
    {        
        if (this->OutputCallback)
        {
            this->OutputCallback(this->pvCallbackData, &(tData.u.frm.tIn), &(tData.u.frm.tOut), (UINT32)(-1));
        }     
    }

    _H264_FUNC_LEAVE();


    return VENC_OK;
}

static VOID _H264EncMainLoop(VOID *pvArg)
{
    VENC_OBJECT_T *this = (VENC_OBJECT_T *)(*(UINT32 *)pvArg);
    UINT32 u4VEncCmd;
    BOOL fgRun = TRUE;

    _H264_FUNC_ENTER();

    ASSERT(this);

    while (fgRun)
    {
        if (VENC_OK == _VENC_ReceiveCmd(this, &u4VEncCmd))
        {
            _VENC_ProcessCmd(this, u4VEncCmd, &fgRun);
            LOG(1,"[xiaolei]: cmd = %d\n", u4VEncCmd);
        }

        LOG(1,"[xiaolei]: eEncState = %d, run = %d\n", this->eEncState, fgRun);

        if (VENC_ENC_STATE_ENC == this->eEncState)
        {
            _H264EncRoutine(this);
        }
    }

    _H264_FUNC_LEAVE();

    x_thread_exit();
}


VENC_OBJECT_T *_prH264Inst;
static VENC_OBJECT_T *_H264GetInst(void)
{
    static VENC_OBJECT_T _h264Object;

    _prH264Inst = &_h264Object;
    UNUSED(_prH264Inst);

    _H264_FUNC_ENTER();

    return &_h264Object;
}

static UINT32 _H264FreeInst(VENC_OBJECT_T *this)
{
    _H264_FUNC_ENTER();
    this->u1Used = (UINT8)~VENC_DEV_USED;
    return VENC_OK;
}

static VENC_HANDLE _H264CreateMMRParam(VENC_PROFILE_T eProfile, BOOL fgInterlace)
{
    static H264_MMR_PARAM_T _mmrParam;
    H264_MMR_PARAM_T * pMMR = &_mmrParam;
    _H264_FUNC_ENTER();

    x_memset((void *)pMMR, 0, sizeof(H264_MMR_PARAM_T));

    pMMR->mmr_h264_level_6            = 31; // 3.1

    pMMR->mmr_en_b_interdirect_1      = 1;
    pMMR->mmr_en_b_inter16x16_1       = 1;

    pMMR->mmr_dram_page_size_div256_7 = VIDEOCODEC_HAL_DEF_DRAM_PAGE_SIZE / 256;
    pMMR->mmr_use_mbaff_1             = 1;
    pMMR->mmr_use_cabac_1             = 1;
    if (_fgFixQp)
    {
        pMMR->mmr_quant_i_frm_6           = 10;
        pMMR->mmr_quant_p_frm_6           = 10;
        pMMR->mmr_quant_b_frm_6           = 10;
    }
    else
    {
        pMMR->mmr_quant_i_frm_6           = 26;
        pMMR->mmr_quant_p_frm_6           = 26;
        pMMR->mmr_quant_b_frm_6           = 29;
    }

    pMMR->mmr_en_p_interskip_1        = 1;
    pMMR->mmr_en_p_inter16x16_1       = 1;
    pMMR->mmr_en_p_inter16x08_1       = 1;
    pMMR->mmr_en_p_inter08x16_1       = 1;
    pMMR->mmr_en_p_inter08x08_1       = 1;
    pMMR->mmr_p_sr_h_div16_3          = 80 / 16;
    pMMR->mmr_p_sr_v_div16_2          = 48 / 16;
    pMMR->mmr_b_sr_h_div16_2          = 48 / 16;
    pMMR->mmr_b_sr_v_div16_2          = 32 / 16;
    pMMR->mmr_use_coarse_search_1     = 1;
    pMMR->mmr_use_rc_1                = 0;
    pMMR->mmr_rc_use_cbr_1            = 1;
    if (_fgFixQp)
    {
        pMMR->mmr_rc_use_init_qp_1        = 1;
    }
    else
    {
        pMMR->mmr_rc_use_init_qp_1        = 0;
    }
    pMMR->mmr_rc_target_bit_rate_14   = 3000;
    pMMR->use_stuff_byte              = 0;
    pMMR->mmr_enable_vui_support_1 = 0;
    pMMR->vui_pic_struct_present_flag = 0;

    switch(eProfile)
    {
    default:
        ASSERT(0);

    case VENC_PROFILE_VPHONE:
        pMMR->mmr_period_i_frm_10     = 0;
        pMMR->mmr_period_idr_frm_10   = 0;
        pMMR->mmr_num_of_b_frm_2      = 0;
        pMMR->mmr_p_frm_q_limiter_8   = 0x54;
        pMMR->mmr_b_frm_q_limiter_8   = 0x32;
        // config as baseline 3.1
        pMMR->mmr_h264_profile_8      = 0;  //baseline
        pMMR->mmr_use_mbaff_1         = 0;
        pMMR->mmr_use_cabac_1         = 0;
        pMMR->mmr_h264_level_6        = 31; // 3.1

        pMMR->mmr_enable_vui_support_1 = 1; // for timing_info (fps)

        pMMR->mmr_rc_adaptive_fps     = 1;
        pMMR->mmr_rc_adaptive_tbr     = 1;
        break;

    case VENC_PROFILE_PVR:
        pMMR->mmr_period_i_frm_10     = 30;
        pMMR->mmr_period_idr_frm_10   = 60;
        pMMR->mmr_num_of_b_frm_2      = 0; //Raven does not support B frame  
        pMMR->mmr_p_frm_q_limiter_8   = 3;
        pMMR->mmr_b_frm_q_limiter_8   = 5;
        pMMR->mmr_h264_profile_8      = 1;  //main

        pMMR->mmr_enable_vui_support_1 = 1;
        if (pMMR->mmr_enable_vui_support_1)
        {
            pMMR->picture_display_type = fgInterlace? 3 : 0;
            pMMR->vui_pic_struct_present_flag = 1;
            pMMR->write_pic_timing = (pMMR->vui_pic_struct_present_flag)? TRUE : FALSE;
        }

        pMMR->mmr_rc_adaptive_fps     = 0;
        pMMR->mmr_rc_adaptive_tbr     = 0;
        break;

    case VENC_PROFILE_MS:
        pMMR->mmr_period_i_frm_10     = 30;
        pMMR->mmr_period_idr_frm_10   = 30;
        pMMR->mmr_num_of_b_frm_2      = 0;
        pMMR->mmr_p_frm_q_limiter_8   = 3;
        pMMR->mmr_b_frm_q_limiter_8   = 5;

        // ms is config as baseline 3.0
        pMMR->mmr_h264_profile_8      = 0;  //0->baseline 1->main profile 2->high profile
        pMMR->mmr_use_mbaff_1         = 0;
        pMMR->mmr_use_cabac_1         = 0;
        pMMR->mmr_h264_level_6        = 30; // 3.0

        pMMR->mmr_enable_vui_support_1 = 1;
       
        if (pMMR->mmr_enable_vui_support_1)
        {
            pMMR->picture_display_type = fgInterlace? 3 : 0;
            pMMR->vui_pic_struct_present_flag = 1;
            pMMR->write_pic_timing = (pMMR->vui_pic_struct_present_flag)? TRUE : FALSE;
        }

        pMMR->mmr_rc_adaptive_fps     = 0;
        pMMR->mmr_rc_adaptive_tbr     = 0;
        break;

    case VENC_PROFILE_SLT:
        pMMR->mmr_period_i_frm_10     = 10;
        pMMR->mmr_period_idr_frm_10   = 20;
        pMMR->mmr_num_of_b_frm_2      = 0; //Raven does not support B frame
        pMMR->mmr_p_frm_q_limiter_8   = 3;
        pMMR->mmr_b_frm_q_limiter_8   = 5;
        
        pMMR->mmr_h264_profile_8      = 2;  //high
        pMMR->picture_display_type    = 3; //TOP_BOTTOM_FIELD
        pMMR->mmr_h264_level_6        = 41; // 4.1
        pMMR->mmr_use_mbaff_1         = 1;
        pMMR->mmr_use_cabac_1         = 1;
        pMMR->mmr_quant_i_frm_6       = 49;
        pMMR->mmr_quant_p_frm_6       = 50;
        pMMR->mmr_quant_b_frm_6       = 51;
        pMMR->mmr_p_sr_h_div16_3      = 5;
        pMMR->mmr_p_sr_v_div16_2      = 3;
        pMMR->mmr_enable_vui_support_1 = 0;
        pMMR->vui_pic_struct_present_flag = 0;

        pMMR->mmr_rc_adaptive_fps     = 0;
        pMMR->mmr_rc_adaptive_tbr     = 0;
        break;

    case VENC_PROFILE_EMU:
        pMMR->mmr_rc_adaptive_fps     = 1;
        pMMR->mmr_rc_adaptive_tbr     = 1;
        break;
    case VENC_PROFILE_CUST:
        pMMR->mmr_period_i_frm_10     = 10;
        pMMR->mmr_period_idr_frm_10   = 20;
        pMMR->mmr_num_of_b_frm_2      = 0; //Raven does not support B frame  
        pMMR->mmr_p_frm_q_limiter_8   = 3;
        pMMR->mmr_b_frm_q_limiter_8   = 5;

        // ms is config as baseline 3.0
        pMMR->mmr_h264_profile_8      = 1;  //mainline
        pMMR->mmr_h264_level_6        = 30; // 3.0

        pMMR->mmr_enable_vui_support_1 = 1;
        if (pMMR->mmr_enable_vui_support_1)
        {
            pMMR->picture_display_type = fgInterlace? 3 : 0;
            pMMR->vui_pic_struct_present_flag = 1;
            pMMR->write_pic_timing = (pMMR->vui_pic_struct_present_flag)? TRUE : FALSE;
            pMMR->mmr_vui_aspect_ratio_info_present_flag = 1;
            pMMR->mmr_vui_aspect_ratio_idc = 1;
        }

        pMMR->mmr_rc_adaptive_fps     = 0;
        pMMR->mmr_rc_adaptive_tbr     = 0;
        break;
    }

#if 1
    LOG(2, "I/IDR period = %d/%d!!!\n", pMMR->mmr_period_i_frm_10, pMMR->mmr_period_idr_frm_10);
#else
    pMMR->mmr_period_i_frm_10     = 1;
    pMMR->mmr_period_idr_frm_10   = 1;
    LOG(0, "I only!!!\n");
#endif

    return VENC_OBJECT_TO_HANDLE(&_mmrParam);

}

static UINT32 _H264FreeMMRParam(H264_MMR_PARAM_T *thisMMR)
{
    _H264_FUNC_ENTER();

    ASSERT(thisMMR);
    UNUSED(thisMMR);

    return VENC_OK;
}

static UINT32 _H264SetMMRParam(VENC_OBJECT_T *this, UINT32 type, VOID *pvParam)
{
    H264_MMR_PARAM_T *pMMR = NULL;
    H264_CTRL_T *pCtrl;
    _H264_FUNC_ENTER();

    ASSERT(this);

    if (VENC_NULL_HANDLE == this->hParam)
    {
        return VENC_INV_HANDLE;
    }

    pMMR = VENC_HANDLE_TO_OBJECT(this->hParam, H264_MMR_PARAM_T);
    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    switch(type)
    {
    case VENC_H264_PRAM_PROFILE_LEVEL:
    {
        VENC_H264_PRAM_PL_T *pPL = (VENC_H264_PRAM_PL_T *)pvParam;
        pMMR->mmr_h264_profile_8 = (UINT8)pPL->u2Profile;
        pMMR->mmr_h264_level_6   = (UINT8)pPL->u2Level;
        pMMR->mmr_num_of_b_frm_2 = 0; //Raven does not support b frame
    }
        break;
    case VENC_PRAM_ADDAUD:
        pCtrl->fgReqAddAud = (BOOL)(*(UINT32 *)pvParam);
        break;
	case VENC_PRAM_SECURE:
		pCtrl->fgSecure= (BOOL)(*(UINT32 *)pvParam);
        break;
    #ifdef VENC_EMU
    case H264_EMU_PARM:
    {
        UINT32 size;
        size = _H264_MEMBER_OFFSET(H264_MMR_PARAM_T, mmr_end);
        x_memcpy((VOID *)pMMR, pvParam, size);
    }
        break;
    #endif
    case VENC_PRAM_ASPECTRATIOIDC:
    {
        VENC_H264_SAR_INFO_T *pSar = (VENC_H264_SAR_INFO_T*)pvParam;
        pMMR->mmr_vui_aspect_ratio_idc = pSar->u4SarIdc;
        pMMR->mmr_vui_sar_width = pSar->u4SarWidth;
        pMMR->mmr_vui_sar_height = pSar->u4SarHeight;

        LOG(1,"set aspect ratio idc = %d,sar Width:%d, sar Height:%d\n", pSar->u4SarIdc,pSar->u4SarWidth,pSar->u4SarHeight);
        //Printf("***** set aspect ratio idc = %d,sar Width:%d, sar Height:%d\n", pSar->u4SarIdc,pSar->u4SarWidth,pSar->u4SarHeight);
        if (VENC_ENC_STATE_ENC == this->eEncState &&
            pMMR->mmr_enable_vui_support_1)
        {
            vVenc_SetAspectRatioIdc(this);
            LOG(1,"Set aspec ratio when encoder running!\n");
        }
    }

        break;
    default:
        return VENC_NOT_IMPL;
    }

    UNUSED(pMMR);
    UNUSED(pvParam);

    return VENC_OK;
}

static UINT32 _H264GetMMRParam(VENC_OBJECT_T *this, UINT32 type, VOID *pvParam)
{
    H264_MMR_PARAM_T *pMMR = NULL;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (VENC_NULL_HANDLE == this->hParam)
    {
        return VENC_INV_HANDLE;
    }

    pMMR = VENC_HANDLE_TO_OBJECT(this->hParam, H264_MMR_PARAM_T);

    switch(type)
    {
    case VENC_H264_PRAM_PROFILE_LEVEL:
    {
        VENC_H264_PRAM_PL_T *pPL = (VENC_H264_PRAM_PL_T *)pvParam;
        pPL->u2Profile = pMMR->mmr_h264_profile_8;
        pPL->u2Level   = pMMR->mmr_h264_level_6;
    }
        break;
    default:
        return VENC_NOT_IMPL;
    }

    UNUSED(pMMR);
    UNUSED(pvParam);

    return VENC_OK;
}


static VENC_HANDLE _H264CreateCtrl(VENC_PROFILE_T eProfile)
{
    static H264_CTRL_T _ctrl;

    _H264_FUNC_ENTER();

    x_memset((void *)(&_ctrl), 0, sizeof(H264_CTRL_T));

    switch(eProfile)
    {
    default:
        break;

    case VENC_PROFILE_MS:
        _ctrl.u1RepeatHeader = 2;
        break;

    case VENC_PROFILE_PVR:
        _ctrl.u1RepeatHeader = 2;
        break;

    case VENC_PROFILE_SLT:
        break;

    case VENC_PROFILE_VPHONE:
        break;

    case VENC_PROFILE_EMU:
        _ctrl.fgEMUDirectMode = TRUE;
        break;
    case VENC_PROFILE_CUST:
        _ctrl.u1RepeatHeader = 2;
        break;
    }

    return VENC_OBJECT_TO_HANDLE(&_ctrl);

}


static UINT32 _H264FreeCtrl(VENC_OBJECT_T *this)
{
    H264_CTRL_T *pCtrl;

    _H264_FUNC_ENTER();

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    ASSERT(pCtrl);
    UNUSED(pCtrl);

    if (NULL_HANDLE != pCtrl->hRezThread)
    {
        _VEncInitSyncCmdFlag(this);
        _H264RezSendCmd(this, VENC_CMD_EXIT_SYNC);
        _VEncWaitSyncCmdDone(this);
        pCtrl->hRezThread = NULL_HANDLE;
    }

    if (pCtrl->ptRezObj)
    {
        VencDeleteRezObject(pCtrl->ptRezObj);
        pCtrl->ptRezObj = NULL;
    }

    if (NULL_HANDLE != pCtrl->hRezCmdQ)
    {
        VERIFY(x_msg_q_delete(pCtrl->hRezCmdQ) == OSR_OK);
        pCtrl->hRezCmdQ = NULL_HANDLE;
    }

    if (NULL_HANDLE != pCtrl->hRezFrmQ)
    {
        VERIFY(x_msg_q_delete(pCtrl->hRezFrmQ) == OSR_OK);
        pCtrl->hRezCmdQ = NULL_HANDLE;
    }

    return VENC_OK;
}

static UINT32 _H264SetCtrl(VENC_OBJECT_T *this, UINT32 type, VOID *pvParam)
{
    H264_CTRL_T *pCtrl = NULL;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    switch(type)
    {
    case 0x101: //break;
        break;
    default:
        return VENC_NOT_IMPL;
    }

    UNUSED(pCtrl);
    UNUSED(pvParam);

    return VENC_OK;
}

static UINT32 _H264GetCtrl(VENC_OBJECT_T *this, UINT32 type, VOID *pvParam)
{
    H264_CTRL_T *pCtrl = NULL;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    switch(type)
    {
    case 0x101: //break;
        break;
    case VENC_CTRL_INT_GET_BS_INFO:
    {
        VENC_CTRL_BS_INFO *pBsInfo = (VENC_CTRL_BS_INFO *)pvParam;

        pBsInfo->u4Addr = pCtrl->u4NextCpSa; 
        pBsInfo->u4Size = VENC_H264_BS_SIZE;
        pBsInfo->u4Wp   = pCtrl->u4NextCpSa;
        pBsInfo->u4Rp   = pCtrl->u4NextCpSa; //in my opinion, these variables are useless for Raven
    }
        break;
    default:
        return VENC_NOT_IMPL;
    }

    UNUSED(pCtrl);
    UNUSED(pvParam);

    return VENC_OK;
}


static UINT32 _H264Init(VENC_OBJECT_T *this)
{
    H264_CTRL_T * pCtrl = NULL;
    //H264_MMR_PARAM_T * pMMR;
    UINT32 u4Ret = VENC_OK;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this)
    {
        return VENC_INV_ARG;
    }
    
    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);
    //pMMR  = VENC_HANDLE_TO_OBJECT(this->hParam, H264_MMR_PARAM_T);
    if (!pCtrl)
    {
        return VENC_INV_HANDLE;
    }

    do
    {
        //mainloop for async encode
        this->pfnEncMainLoop = _H264EncMainLoop;
        //variable init
        _H264VarInit(this);
        _H264CalcDesParm(this);

        //mmr init
        _H264MMRInit(this);
        //resizer
        //if (pCtrl->fgCoarse) always do for runtime change resolution
        {
            u4Ret = _H264RezInit(this, pCtrl);
            VENC_CHK_OK_BREAK(u4Ret);
        }

        //buffer
        u4Ret = _H264BuffInit(this, pCtrl);
        VENC_CHK_OK_BREAK(u4Ret);

        //mid protection
        _H264MidInit(this);
        vVenc_H264_LoadRCCode(this);

    }while (0);


    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264Run(VENC_OBJECT_T *this)
{
    H264_CTRL_T * pCtrl = NULL;
    UINT32 u4Ret = VENC_OK;
    _H264_FUNC_ENTER();

    ASSERT(this);
    if (!this)
    {
        return VENC_INV_ARG;
    }

#ifdef VENC_TIME_PROFILE
    _u4FrmCnt = 0;
    _u4EncTime = 0;
#endif

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    do
    {
        pCtrl->fg1stHeader    = TRUE;
        pCtrl->u1BSeqIdx      = 0;
        pCtrl->u1RefSeqIdx    = 0;
        pCtrl->u2IDRFrmIdx    = 0;
        pCtrl->u2IFrmIdx      = 0;
        pCtrl->u4FrameNo      = 0;
        pCtrl->fgForceIDR     = FALSE;
        //if ((VENC_SUPPORT_ASYNC_MODE(this->eSMode)) && pCtrl->fgCoarse) always do for runtime change resolution
        if (VENC_SUPPORT_ASYNC_MODE(this->eSMode))
        {
            ASSERT((NULL_HANDLE != pCtrl->hRezFrmQ) && (NULL_HANDLE != pCtrl->hRezCmdQ));
            _VEncInitSyncCmdFlag(this);
            _H264RezSendCmd(this,VENC_CMD_START_SYNC);
            _VEncWaitSyncCmdDone(this);
        }
        i4Venc_Hal_Start();
        //buffer reg setting
        i4Venc_Hal_SetRCInfoBuf(pCtrl->rHwBuf.u4RCInfoBuf);
    }while (0);

    _H264_FUNC_LEAVE();

    return u4Ret;
}

static UINT32 _H264Stop(VENC_OBJECT_T *this)
{
    H264_CTRL_T * pCtrl = NULL;
    UINT32 u4Ret;
    _H264_FUNC_ENTER();

    ASSERT(this);
    if (!this)
    {
        return VENC_INV_ARG;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    do
    {
        //if ((VENC_SUPPORT_ASYNC_MODE(this->eSMode)) && pCtrl->fgCoarse) always do for runtime change resolution
        if (VENC_SUPPORT_ASYNC_MODE(this->eSMode))
        {
            u4Ret = (VENC_STATE_RUN != pCtrl->eRezCmdState) ? VENC_INTERNAL_ERR : VENC_OK;
            VENC_CHK_OK_BREAK(u4Ret);

            ASSERT((NULL_HANDLE != pCtrl->hRezFrmQ) && (NULL_HANDLE != pCtrl->hRezCmdQ));
            _VEncInitSyncCmdFlag(this);
            _H264RezSendCmd(this,VENC_CMD_STOP_SYNC);
            _VEncWaitSyncCmdDone(this);
        }

        i4Venc_Hal_Stop();
    } while (0);

    _H264_FUNC_LEAVE();

    return VENC_OK;
}


static UINT32 _H264Close(VENC_OBJECT_T *this)
{
    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this)
    {
        return VENC_INV_ARG;
    }

    //free resource
    if (VENC_NULL_HANDLE != this->hParam)
    {
        _H264FreeMMRParam((H264_MMR_PARAM_T *)(this->hParam));
    }

    if (VENC_NULL_HANDLE != this->hCtrl)
    {
        _H264FreeCtrl(this);
    }

#ifdef __MODEL_slt__
    u4CrcFileRdOft = 0;
    _u4CompareErr = 0;
#endif

    _H264FreeInst(this);

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

//comment from xiaolei for 5399 venc new driver
//i do not modify the output buffer status in this function
//because i think sync encoding is not used now
//and i do not know whether we should do output callback here
static UINT32  _H264EncSync (VENC_OBJECT_T *this, VENC_IN_PIC_T *pIn, VENC_OUT_PIC_T *pOut, VOID* pvPram)
{
    UINT32 u4Ret = VENC_OK;
    H264_CTRL_T *pCtrl;
    UINT32 u4OutAddr, u4OutSize, u4Addr, u4Size;
    H264_MMR_PARAM_T* pMMR;
    
    #ifdef VENC_EMU
    H264_EMU_OUT_PIC_T *ptEmuOut = (H264_EMU_OUT_PIC_T *)pOut;
    #endif

    _H264_FUNC_ENTER();

    if (!this)
    {
        LOG(0, "%s: this is NULL!!\n", __FUNCTION__);
        ASSERT(this);
        return VENC_INV_ARG;
    }

    pMMR  = (H264_MMR_PARAM_T *)this->hParam;

    if (pvPram)
    {
        //TODO
        LOG(0, "%s: pvPram is not support!\n", __FUNCTION__);
        ASSERT(!pvPram);
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    pOut->u4Type = 0;
    pOut->u4Pts  = pIn->u4Pts;
    u4Addr = 0;
    u4Size = 0;

    LOG(7, "%s fg1stHeader %d, u1RepeatHeader %d\n", __FUNCTION__, pCtrl->fg1stHeader, pCtrl->u1RepeatHeader);

    //INIT
    if (pCtrl->fg1stHeader)
    {
        //write sps
        i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);
        if (pMMR->mmr_enable_vui_support_1)
        {
            vVenc_SetVUI(this);
        }
        vVenc_SeqMmrSet(this);
        u4VENC_H264GENSPS_HW(this, &u4OutAddr, &u4OutSize);
        
        u4Addr = u4OutAddr;
        u4Size += VENC_ALIGN_MASK(u4OutSize, 7); // need alignment, because hw add is align
        #ifdef VENC_EMU
        pOut->u4Type |= VENC_H264_PIC_TYPE_SPS;
        ptEmuOut->u4SPSAddr = u4OutAddr;
        ptEmuOut->u4SPSSize = u4OutSize;
        #endif

        pCtrl->fg1stHeader  = FALSE;
        pCtrl->fgSkipHeaderOnce = TRUE;

        //write pps
        u4VENC_H264GENPPS_HW(this, &u4OutAddr, &u4OutSize);
        u4Size += VENC_ALIGN_MASK(u4OutSize, 7); // need alignment, because hw add is align

        #ifdef VENC_EMU
        pOut->u4Type |= VENC_H264_PIC_TYPE_PPS;
        ptEmuOut->u4PPSAddr = u4OutAddr;
        ptEmuOut->u4PPSSize = u4OutSize;
        #endif

    }

    //encode seq. 1.IPPPPP

    //gen ref frame first
    _H264EncRefTypeAnalyse(pCtrl);
    if (VENC_H264_PIC_TYPE_IDR == pCtrl->eType)
    {
        //encoder header
        if (pCtrl->u1RepeatHeader > 0)
        {
            //write sps
            if (pCtrl->fgSkipHeaderOnce)
            {
                pCtrl->fgSkipHeaderOnce = FALSE;
            }
            else
            {
                i4Venc_Hal_SetRamCodeBuf(pCtrl->rHwBuf.u4RamCodeBuf);
                if (pMMR->mmr_enable_vui_support_1)
                {
                    vVenc_SetVUI(this);
                }
                vVenc_SeqMmrSet(this);
                u4VENC_H264GENSPS_HW(this, &u4OutAddr, &u4OutSize);
                
                u4Addr = u4OutAddr;
                u4Size += VENC_ALIGN_MASK(u4OutSize, 7); // need alignment, because hw add is align

                #ifdef VENC_EMU
                pOut->u4Type |= VENC_H264_PIC_TYPE_SPS;
                ptEmuOut->u4SPSAddr = u4OutAddr;
                ptEmuOut->u4SPSSize = u4OutSize;
                #endif

                //write pps
                u4VENC_H264GENPPS_HW(this, &u4OutAddr, &u4OutSize);
                u4Size += VENC_ALIGN_MASK(u4OutSize, 7); // need alignment, because hw add is align

                #ifdef VENC_EMU
                pOut->u4Type |= VENC_H264_PIC_TYPE_PPS;
                ptEmuOut->u4PPSAddr = u4OutAddr;
                ptEmuOut->u4PPSSize = u4OutSize;
                #endif

            }

            //write sei
            if (pCtrl->fgWriteSEI)
            {
                LOG(0, "not support SEI at sync mode\n");
                ASSERT(0);
                _H264GenSEI_SW(this, &u4OutAddr, &u4OutSize);
            }

            if (pCtrl->u1RepeatHeader == 1)
            {
                pCtrl->u1RepeatHeader = 0;
            }
        }
    }

    pCtrl->rHwBuf.u4InputBufY = pIn->u4YAddr; // set source buffer add
    pCtrl->rHwBuf.u4InputBufC = pIn->u4CAddr; //
    vVenc_FrmMmrSet(this);
    u4VENC_H264GENFRM_HW(this, &u4OutAddr, &u4OutSize);
    vVenc_SetBufStatus(pCtrl, u4OutAddr, BS_BUF_COPYING);

    if (!u4Addr)
    {
        u4Addr = u4OutAddr;
    }
    u4Size += u4OutSize;

    #ifdef VENC_EMU
    ptEmuOut->u4FRMAddr   = u4OutAddr;
    ptEmuOut->u4FRMSize   = u4OutSize;
    #endif

    //pOut->u4Type   = pCtrl->eType;
    pOut->u4Type  |= (UINT32)pCtrl->eType;
    pOut->u4Addr   = u4Addr;
    pOut->u4Size   = u4Size;

    LOG(7, "%s complete, 0x%x, %d, 0x%x, 0x%x\n", __FUNCTION__, pOut->u4Addr, pOut->u4Size, (*((UINT32*)(VIRTUAL(pOut->u4Addr)))), (*((UINT32*)(VIRTUAL(pOut->u4Addr+4)))));

    _H264_FUNC_LEAVE();

    return u4Ret;
}

static UINT32 _H264EncAsync (VENC_OBJECT_T *this, VENC_IN_PIC_T *pIn, VENC_OUT_PIC_T *pOut, VOID* pvPram)
{
    UINT32 u4Ret = VENC_OK;
    VENC_FRMQ_DATA_T rData;
    
    _H264_FUNC_ENTER();

    if (!this)
    {
        LOG(0, "%s: this is NULL!!\n", __FUNCTION__);
        ASSERT(this);
        return VENC_INV_ARG;
    }

    if (pvPram)
    {
        //TODO
        LOG(0, "%s: pvPram is not support!\n", __FUNCTION__);
        ASSERT(!pvPram);
        return VENC_INV_ARG;
    }
    else
    {
        rData.eType = VENC_FRMQ_TYPE_FRM;
    }
    rData.u.frm.tIn   = *pIn;

    if (pOut)
    {
        rData.u.frm.tOut  = *pOut;
    }
    else
    {
        rData.u.frm.tOut.u4Addr = 0;
    }

    LOG(3, "%s, SendFrm, 0x%x, 0x%x, 0x%x, %d\n", __FUNCTION__, pIn->u4Pts, pIn->u4YAddr, pIn->u4CAddr, pIn->fgRzSrc);

    x_memset((void *)&(rData.u.frm.tPram), 0, sizeof(VENC_PIC_PRAM_T));
    _H264RezSendFrm(this, &rData);

    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264SetRtParamSync (VENC_OBJECT_T *this, VENC_RT_PARM_T *ptParm)
{
    UINT32 u4Ret = VENC_OK;
    H264_CTRL_T *pCtrl;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this || !ptParm)
    {
        return VENC_INV_ARG;
    }

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    do
    {
        BOOL fgResetHW = FALSE;
        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_RES)
        {
            this->u2Width  = ptParm->u2Width;
            this->u2Height = ptParm->u2Height;
            this->u2Pitch  = ptParm->u2Pitch;

            fgResetHW = TRUE;
        }

        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_BITRATE)
        {
            this->u4Bitrate = ptParm->u4Bitrate;
        }

        if (ptParm->u4Flag & VENC_RT_PARM_FLAG_FRAMERATE)
        {
            this->u1Framerate = ptParm->u1Framerate;
        }

        if (fgResetHW)
        {
            _H264HWReset(this);
        }
        else
        {
            _H264RtReset(this);
        }

    }while(0);

    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264SetRtParamAsync (VENC_OBJECT_T *this, VENC_RT_PARM_T *ptParm)
{
    UINT32 u4Ret = VENC_OK;
    VENC_FRMQ_DATA_T rData;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this || !ptParm)
    {
        return VENC_INV_ARG;
    }

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    rData.eType = VENC_FRMQ_TYPE_RT_PARM;
    x_memcpy((VOID *)&(rData.u.tRTParm), (VOID *)ptParm, sizeof(VENC_RT_PARM_T));

    _H264RezSendFrm(this, &rData);
    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264SetRtCtrlSync (VENC_OBJECT_T *this, VENC_RT_CTRL_T *ptParm)
{
    UINT32 u4Ret = VENC_OK;
    H264_CTRL_T *pCtrl;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this || !ptParm)
    {
        return VENC_INV_ARG;
    }

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    if (ptParm->u4Flag & VENC_RT_CTRL_FLAG_INSERT_KEYFRAME)
    {       
        i4Venc_Hw_VENC_InsertKFrame(1);
        
        if (!pCtrl->u1RepeatHeader)
        {
            pCtrl->u1RepeatHeader = 1;  //ONCE
        }
        pCtrl->fgForceIDR = TRUE;
    }

    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264SetRtCtrlAsync (VENC_OBJECT_T *this, VENC_RT_CTRL_T *ptParm)
{
    UINT32 u4Ret = VENC_OK;
    VENC_FRMQ_DATA_T rData;

    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this || !ptParm)
    {
        return VENC_INV_ARG;
    }

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    rData.eType = VENC_FRMQ_TYPE_RT_CTRL;
    x_memcpy((VOID *)&(rData.u.tRTCtrl), (VOID *)ptParm, sizeof(VENC_RT_CTRL_T));

    _H264RezSendFrm(this, &rData);

    _H264_FUNC_LEAVE();

    return u4Ret;
}


static UINT32 _H264UnlockBuf (VENC_OBJECT_T *this, UINT32 u4Addr, UINT32 u4Size)
{
    H264_CTRL_T *pCtrl;
    UINT32 u4BsBufIdx;
    
    _H264_FUNC_ENTER();

    ASSERT(this);

    if (!this)
    {
        return VENC_INV_ARG;
    }

    if (VENC_NULL_HANDLE == this->hCtrl)
    {
        return VENC_INV_HANDLE;
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    //sth to do
    LOG(6, "%s: want to unlock u4Addr=0x%x\n", __FUNCTION__, u4Addr);
    for(u4BsBufIdx = 0; u4BsBufIdx < VENC_RAVEN_BSBUF_CNT; u4BsBufIdx++)
    {
        LOG(6, "%s: buf[%d] addr 0x%x, status=%d\n", __FUNCTION__, u4BsBufIdx, pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx], pCtrl->rBsBufStatus[u4BsBufIdx]);
        if((BS_BUF_COPYING == pCtrl->rBsBufStatus[u4BsBufIdx])
            && (u4Addr == pCtrl->rHwBuf.u4OutputBuf[u4BsBufIdx]))
        {
            pCtrl->rBsBufStatus[u4BsBufIdx] = BS_BUF_EMPTY;
            LOG(6, "%s: unlock success Idx = %d, u4Addr=0x%x\n", __FUNCTION__, u4BsBufIdx, u4Addr);
            break;
        }
    }

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

static UINT32  _H264EncSeqHdrSync (VENC_OBJECT_T *this, VENC_OUT_PIC_T *pOut, VOID* pvPram)
{
    //UINT32 u4Ret = VENC_OK;
    H264_CTRL_T *pCtrl;
    UINT32 u4OutAddr1, u4OutSize1, u4OutAddr2, u4OutSize2, u4Addr, u4Size;

    _H264_FUNC_ENTER();

    if (!this)
    {
        LOG(0, "%s: this is NULL!!\n", __FUNCTION__);
        ASSERT(this);
        return VENC_INV_ARG;
    }

    if (pvPram)
    {
        //TODO
        LOG(0, "%s: pvPram is not support!\n", __FUNCTION__);
        ASSERT(!pvPram);
    }

    pCtrl = VENC_HANDLE_TO_OBJECT(this->hCtrl, H264_CTRL_T);

    pOut->u4Type = 0;
    pOut->u4Pts  = 0;
    u4Addr = 0;
    u4Size = 0;

    //INIT
    if (pCtrl->fg1stHeader)
    {
        //write 1st sps
        vVenc_SeqMmrSet(this);
        u4VENC_H264GENSPS_HW(this, &u4OutAddr1, &u4OutSize1);
        pCtrl->fg1stHeader  = FALSE;
    }
    else
    {
        //write sps
        u4VENC_H264GENPPS_HW(this, &u4OutAddr1, &u4OutSize1);
    }

    vVenc_SetBufStatus(pCtrl, u4OutAddr1, BS_BUF_COPYING);

    // sps
    u4Addr = u4OutAddr1;
    u4Size += VENC_ALIGN_MASK(u4OutSize1, 7); // need alignment, because hw add is align 
    
    //write pps
    u4VENC_H264GENPPS_HW(this, &u4OutAddr2, &u4OutSize2);
    vVenc_SetBufStatus(pCtrl, u4OutAddr2, BS_BUF_COPYING);
    _H264UnlockBuf(this, u4OutAddr2, u4OutSize2);
    u4OutAddr2 = VIRTUAL(u4OutAddr2);

    u4OutAddr1 = VIRTUAL(u4OutAddr1);
    x_memcpy((void*)(u4OutAddr1 + u4OutSize1), (void*)u4OutAddr2, u4OutSize2);
    u4Size += VENC_ALIGN_MASK(u4OutSize2, 7); // need alignment, because hw add is align 

    //write sei
    if (pCtrl->fgWriteSEI)
    {
        LOG(0, "not support SEI at sync mode\n");
        ASSERT(0);
    }

    pOut->u4Type   = (UINT32)(VENC_H264_PIC_TYPE_SPS | VENC_H264_PIC_TYPE_PPS);
    pOut->u4RStart = pCtrl->u4BsSa;
    pOut->u4REnd   = pCtrl->u4BsEa;
    pOut->u4Addr   = u4Addr;
    pOut->u4Size   = u4Size;

    if (pCtrl->u1RepeatHeader == 1)
    {
        pCtrl->u1RepeatHeader = 0;
    }

    LOG(0, "%s complete, 0x%x, %d, 0x%x, 0x%x\n", __FUNCTION__, pOut->u4Addr, pOut->u4Size, (*((UINT32*)(VIRTUAL(pOut->u4Addr)))), (*((UINT32*)(VIRTUAL(pOut->u4Addr+4)))));

    _H264_FUNC_LEAVE();

    return VENC_OK;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

VENC_OBJECT_T *H264Open(VOID)
{
    VENC_OBJECT_T *this = _H264GetInst();

    _H264_FUNC_ENTER();

    if (!IS_SupportH264EN())
    {
        LOG(0, "H264 venc bonding off, return fail!!\n");
        return VENC_NULL_HANDLE;
    }

#if defined(CC_MT5880)
    if (IS_IC_5880())
    {
        LOG(0, "Viper don't have venc, return fail!!\n");
        return VENC_NULL_HANDLE;
    }
#endif

    if (!this)
    {
        return VENC_NULL_HANDLE;
    }

    if (VENC_DEV_USED == this->u1Used)
    {
        return VENC_NULL_HANDLE;
    }

    x_memset((void*)this, 0, sizeof(VENC_OBJECT_T));
    x_strncpy((char*)(this->u1aName), "H246", 4);

    this->u1Used          = VENC_DEV_USED;
    //FUNCTION
    this->CreateParam     = _H264CreateMMRParam;
    this->SetParam        = _H264SetMMRParam;
    this->GetParam        = _H264GetMMRParam;
    this->CreateCtrl      = _H264CreateCtrl;
    this->SetCtrl         = _H264SetCtrl;
    this->GetCtrl         = _H264GetCtrl;
    this->Init            = _H264Init;
    this->Run             = _H264Run;
    this->Stop            = _H264Stop;
    this->Close           = _H264Close;
    this->EncSeqHdrSync   = _H264EncSeqHdrSync;
    this->EncFrameSync    = _H264EncSync;
    this->EncFrameAsync   = _H264EncAsync;
    this->SetRtParamSync  = _H264SetRtParamSync;
    this->SetRtParamAsync = _H264SetRtParamAsync;
    this->SetRtCtrlSync   = _H264SetRtCtrlSync;
    this->SetRtCtrlAsync  = _H264SetRtCtrlAsync;
    this->UnlockBuf       = _H264UnlockBuf;

    _H264_FUNC_LEAVE();

    return (this);

}



