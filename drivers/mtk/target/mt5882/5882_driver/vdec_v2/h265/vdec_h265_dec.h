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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_h265_dec.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

#ifndef VDEC_H265_DEC_H
#define VDEC_H265_DEC_H


#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#include "vdec_info_h265.h"
#include "vdec_hw_h265.h"
#include "vdec_hal_if_h265.h"
#include "../vdec_debug.h"

#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_timer.h"
#include "x_bim.h"
#include "vdec_drvif.h"

#define H265_UNLOCK_FRAME_TIME 20
#define VDEC_H265_GET_FB_TIMEOUT        1
#define VDEC_H265_SEAMLESS_WIDTH_THD    480
//#define VDEC_H265_EMU
#define VDEC_H265_DRV_DEBUG
//#define VDEC_H265_DEEP_DEBUG

#ifndef VDEC_SUPPORT_ISR
#define VDEC_H265_IRQ_DISABLE
#endif

//HEVC
#define     NAL_UNIT_CODED_SLICE_TRAIL_N  0   // 0
#define     NAL_UNIT_CODED_SLICE_TRAIL_R 1 // 1
#define     NAL_UNIT_CODED_SLICE_TSA_N      2
#define     NAL_UNIT_CODED_SLICE_TLA        3 // Current name in the spec: TSA_R
#define     NAL_UNIT_CODED_SLICE_STSA_N 4  // 4
#define     NAL_UNIT_CODED_SLICE_STSA_R 5
    
#define     NAL_UNIT_CODED_SLICE_RADL_N 6
#define     NAL_UNIT_CODED_SLICE_DLP        7
#define     NAL_UNIT_CODED_SLICE_RASL_N  8
#define     NAL_UNIT_CODED_SLICE_TFD         9// Current name in the spec: RASL_R

#define     NAL_UNIT_CODED_SLICE_BLA            16   // Current name in the spec: BLA_W_LP
#define     NAL_UNIT_CODED_SLICE_BLANT              17 // Current name in the spec: BLA_W_DLP
#define     NAL_UNIT_CODED_SLICE_BLA_N_LP   18
#define     NAL_UNIT_CODED_SLICE_IDR                19 // Current name in the spec: IDR_W_DLP
#define     NAL_UNIT_CODED_SLICE_IDR_N_LP   20
#define     NAL_UNIT_CODED_SLICE_CRA            21
 
#define     NAL_UNIT_VPS        32
#define     NAL_UNIT_SPS        33
#define     NAL_UNIT_PPS        34
#define     NAL_UNIT_ACCESS_UNIT_DELIMITER  35
#define     NAL_UNIT_EOS        36
#define     NAL_UNIT_EOB        37
#define     NAL_UNIT_FILLER_DATA     38
#define     NAL_UNIT_SEI            39
#define     NAL_UNIT_SEI_SUFFIX         40

#define     NAL_UNIT_SKIPP        99
#define H265_Slice_Type_MAX         3
#define MRG_MAX_NUM_CANDS           5
#define MAX_POC                     0x7FFFFFFF  ///< max. value of signed 32-bit integer
#define FBUF_ID_UNKNOWN     0xFF

#define NO_PIC 0
#define TOP_FIELD 1
#define BOTTOM_FIELD 2
#define FRAME 3

#define RESOLUTION_CHG      (1 << 0)
#define FRAMERATE_CHG       (1 << 1)

enum RefPicList
{
  REF_PIC_LIST_0 = 0,   ///< reference list 0
  REF_PIC_LIST_1 = 1,   ///< reference list 1
  REF_PIC_LIST_C = 2,   ///< combined reference list for uni-prediction in B-Slices
  REF_PIC_LIST_X = 100  ///< special mark
};

enum SliceType
{
  HEVC_B_SLICE,
  HEVC_P_SLICE,
  HEVC_I_SLICE
};


#define abs_hevc(x) (((x) >= 0) ? (x) : -(x))
#define min_hevc(x, y) (((x) < (y)) ? (x) : (y))
#define max_hevc(x, y) (((x) > (y)) ? (x) : (y))
#define align64_hevc(val) ((((val) + 0x3F) >> 6) << 6)
#define align16_hevc(val) ((((val) + 0xF) >> 4) << 4)

//#define IsH265SliceNalUnit(NalType) (((NalType >= NAL_UNIT_CODED_SLICE_TRAIL_N) && (NalType <= NAL_UNIT_CODED_SLICE_TFD)) 
//                                 || ((NalType >= NAL_UNIT_CODED_SLICE_BLA) && (NalType <= NAL_UNIT_CODED_SLICE_CRA)))

#define IsH265SliceNalUnit(NalType) ((NalType <= NAL_UNIT_CODED_SLICE_TFD) \
                                 || ((NalType >= NAL_UNIT_CODED_SLICE_BLA) && (NalType <= NAL_UNIT_CODED_SLICE_CRA)))
 
                                 
#define IsH265NonRefSlice(NalType) ((NalType == NAL_UNIT_CODED_SLICE_TRAIL_N) || (NalType == NAL_UNIT_CODED_SLICE_TSA_N) \
                                  ||(NalType ==NAL_UNIT_CODED_SLICE_STSA_N) || (NalType ==NAL_UNIT_CODED_SLICE_RADL_N) \
                                  ||(NalType ==NAL_UNIT_CODED_SLICE_RASL_N)) 

#define IsH265NalUnit(NalType) ((NalType <= NAL_UNIT_CODED_SLICE_TFD) \
                                 || ((NalType >= NAL_UNIT_CODED_SLICE_BLA) && (NalType <= NAL_UNIT_CODED_SLICE_CRA)) \
                                 || ((NalType >= NAL_UNIT_VPS) && (NalType <= NAL_UNIT_SEI_SUFFIX)))

#define IsH265RAPSlice(NalType) ((NalType >= NAL_UNIT_CODED_SLICE_BLA) && (NalType <= NAL_UNIT_CODED_SLICE_CRA))

void vVDecH265Ref_Pic_List_Modification(UINT32 u4InstID,UINT32 u4VDecID);
void vVDecH265GetCRC(UINT32 u4InstID,UINT32 u4VDecID,UINT32 *pu4Y,UINT32 *pu4C,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265FlushDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetMinPOCToOutPut(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetMinPOC(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
BOOL fgVDecH265IsNonRefFBuf(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265ClrFBufInfo(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265ClrDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetNonRefFBuf(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265SetRefFBuf(VDEC_INFO_H265_FBUF_INFO_T *prFBufInfo,UCHAR ucFbIdx,UCHAR ucRef);

INT32 i4VDecH265AllocateFBG(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265AllocateFBInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
//void vVDecH265SetCurrFBInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265SetCurrFBInfo(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo);
void vVDecH265SetRegister(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
INT32 *vVDecH265GetSLDefaultAddress(INT32 u4SizeId, INT32 u4ListId);
void vVDecH265InitTilesInfo(UINT32 u4InstID,H265_SPS_Data *prSPS,H265_PPS_Data *prPPS,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo);
void vVDecH265AssignQuantParam(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);

UCHAR ucVDecH265GetFrameRate(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetAspInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm,UINT32 *u4AspW,UINT32 *u4AspH,UINT32 *u4SAR);
BOOL  fgVDecH265IsAspInfoChg(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR  ucVDecH265SendSeqChgInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);

INT32 i4VDecH265PrepareRefList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
INT32 i4VDecH265ChkRefPicsAreAvailable(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265CreateLostPic(UINT32 u4InstID,INT32 i4LostPOC,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
void vVDecH265ApplyRefPicList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UINT32 u4VDecH265SetRefPicList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetSTRefPicFBufIdByPOC(UINT32 u4InstID,INT32 i4POC,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
UCHAR ucVDecH265GetLTRefPicFBufIdByPOC(UINT32 u4InstID,INT32 i4POC,BOOL bMSBpresentFlag,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);

INT32 i4VDecH265SetPicInfo(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);
INT32 i4VDecH265LaeDecInfo(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);
INT32 i4VDecH265Core1DecInfo(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo);

UCHAR ucVDecH265PrepareDispBuf(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
BOOL  fgVDecH265GetFrmToDispQ(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);

void vVDecH265Isr(UINT16 u4Vector);
void vVDecH265InitIsr(UINT32 u4InstID);
void vVDecH265WaitDecFinish(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
BOOL fgMCoreMatchLAE(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo);
UCHAR ucSearchNearestPOCFbufIdx(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);

#ifdef VDEC_H265_Performance
void vVDecH265Performance(UINT32 ucEsId,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
#endif

BOOL fgChkSeamlessMode(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
#endif


