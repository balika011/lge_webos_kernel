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
 * $RCSfile: vdec_h264psr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_h264psr.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "vdec_h264dec.h"
#include "vdec_h264util.h"
#include "vdec_h264hw.h"
#include "fbm_drvif.h"
#include "x_timer.h"
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
#include "x_bim.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
const UINT8 NumClockTS[9] =
{
   1,  1,  1,  2,  2,  3,  3,  2,  3
};

const CHAR ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const CHAR ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

//Error handling
#ifdef FBM_DPB_CTRL
#define CHKSPS_PARAM(u4Param, u4HBound)\
    if(u4Param > u4HBound)\
    {\
        LOG(2, "SPS Hdr err %s(L:%d)\n", __FUNCTION__, __LINE__);\
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;\
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);\
        return MPEG_FRAME_RATE_UNKNOWN;\
    }

#define CHKPARAM(u4Param, u4HBound)\
    if(u4Param > u4HBound)\
    {\
        if(prDecPrm->prCurrFBufInfo && (prDecPrm->u1CurViewIdx < MAX_VIEW_NUM))\
        {\
            if(FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId) == FBM_FB_STATUS_DECODE)\
            {\
                FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_EMPTY);\
                _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx + (prDecPrm->u1CurViewIdx<<8));\
            }\
        }\
        LOG(2, "Hdr err %s(L:%d)\n", __FUNCTION__, __LINE__);\
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;\
        *pi4RetValue = 1;\
        return;\
    }

#define CHKPARAM_R(i4Param, i4LBound, i4HBound)\
    if((i4Param < i4LBound) || (i4Param > i4HBound))\
    {\
        if(prDecPrm->prCurrFBufInfo && (prDecPrm->u1CurViewIdx < MAX_VIEW_NUM))\
        {\
            if(FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId) == FBM_FB_STATUS_DECODE)\
            {\
                FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_EMPTY);\
                _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx + (prDecPrm->u1CurViewIdx<<8));\
            }\
        }\
        LOG(2, "Hdr err %s(L:%d)\n", __FUNCTION__, __LINE__);\
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;\
        *pi4RetValue = 1;\
        return;\
    }

#else
#define CHKPARAM(u4Param, u4HBound)\
    if(u4Param > u4HBound)\
    {\
        LOG(2, "Hdr err %s(L:%d)\n", __FUNCTION__, __LINE__);\
        *pi4RetValue = 1;\
        return;\
    }

#define CHKPARAM_R(i4Param, i4LBound, i4HBound)\
    if((i4Param < i4LBound) || (i4Param > i4HBound))\
    {\
        LOG(2, "Hdr err %s(L:%d)\n", __FUNCTION__, __LINE__);\
        *pi4RetValue = 1;\
        return;\
    }
    
#endif

#define AFD_IDENTIFIER_DTG1 0x44544731
#define ATSC_IDENTIFIER  0x47413934
#define EIA_CC_TYPE 0x3
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static BOOL _fg_boot_profile = FALSE;
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static BOOL _fgIsMoreRbspData(UINT32 u4VDecID, UINT32 u4RbspEndAddr);

static UINT32 _InitSPS(VDEC_AVC_SPS_T* prSPS);

static void _vInitSliceHdr(VDEC_AVC_SLICE_HDR_T* prSliceHdr, UINT32 u4NumRefIdxL0, UINT32 u4NumRefIdxL1);

static void _vHrdParameters(UINT32 u4VDecID, VDEC_AVC_HRD_PARAM_T* prHrdPara);

static void _vRefPicMarking(UINT32 u4VDecID, BOOL bIsIDR, VDEC_AVC_SLICE_HDR_T* prSliceHdr);

static BOOL _vInterpretSEI_PicTiming(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm);

static void _VDEC_GetProfileLevel(VDEC_AVC_SPS_T* prSPS, VDEC_ES_INFO_T* prVdecEsInfo);

static void _VDEC_WaitRAPToDisp(UCHAR ucEsId, VDEC_AVC_DecPrm* prDecPrm);

#ifdef FBM_DPB_CTRL
extern void FBM_UpdateReferenceList(UCHAR ucFbgId, UCHAR ucFbId, BOOL fgReference);
#endif


// *********************************************************************
// Function    : BOOL fgIsMoreRbspData()
// Description : 
// Parameter   : None
// Return      : TRUE: (i)the next bit is 0
//                            (ii)the next bit is 1, but the remaining bits != 0
//                   FALSE: otherse
// *********************************************************************
static BOOL _fgIsMoreRbspData(UINT32 u4VDecID, UINT32 u4RbspEndAddr)
{
    UINT32 u4RemainedBits;
    UINT32 u4Temp;
    UINT32 i;
    UINT32 u4CurRptr;

    u4RemainedBits = (_VDEC_AVCVLDShiftBits(u4VDecID) % 8); //0~7
    //u4RemainedBits = (8 - (((dReadAVCVLD(RW_AVLD_CTRL) >> 16) & 0x3F) % 8));  
    u4Temp = 0xffffffff;
    for(i=0; i<=u4RemainedBits; i++)
    {
        u4Temp &= (~((UINT32)1<<i));
    }

    u4CurRptr = _VDEC_VldRPtr(u4VDecID);
    if((_VDEC_AVCGetBitStream(u4VDecID, 0) & u4Temp) == (UINT32)(0x80000000))
    {
        // no more
        return FALSE;
    }
    else if((u4RbspEndAddr > 2) && (u4CurRptr >= (u4RbspEndAddr - 2)))
    {
        LOG(1, "_fgIsMoreRbspData u4CurRptr 0x%x >= u4RbspEndAddr-2 0x%x", u4CurRptr, (u4RbspEndAddr - 2));
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

// *********************************************************************
// Function    : void vInitSPS(TSeqParameterSetRbsp *ptSPS)
// Description : Init SPS related fields
// Parameter   : None
// Return      : None
// *********************************************************************
static UINT32 _InitSPS(VDEC_AVC_SPS_T* prSPS)
{
    INT32 i;
    UINT32 u4DpbSize;

    ASSERT(prSPS!=NULL);
      
    prSPS->u4ChromaFormatIdc = 1;
    prSPS->u4BitDepthLumaMinus8 = 0;
    prSPS->u4BitDepthChromaMinus8 = 0;
    prSPS->fgQpprimeYZeroTransformBypassFlag = FALSE;  
    prSPS->fgSeqScalingMatrixPresentFlag = FALSE;
    for(i=0; i<8; i++)
    {
        prSPS->fgSeqScalingListPresentFlag[i] = FALSE;  
    }

    switch (prSPS->u4LevelIdc)
    {
        case 10:
            u4DpbSize = 152064;
            break;
        case 11:
            u4DpbSize = 345600;
            break;
        case 12:
            u4DpbSize = 912384;
            break;
        case 13:
            u4DpbSize = 912384;
            break;
        case 20:
            u4DpbSize = 912384;
            break;
        case 21:
            u4DpbSize = 1824768;
            break;
        case 22:
            u4DpbSize = 3110400;
            break;
        case 30:
            u4DpbSize = 3110400;
            break;
        case 31:
            u4DpbSize = 6912000;
            break;
        case 32:
            u4DpbSize = 7864320;
            break;
        default:    //40 or upper
            u4DpbSize = 12582912;
            break;
    }
    return u4DpbSize;
  
}

static void _vInitSliceHdr(VDEC_AVC_SLICE_HDR_T* prSliceHdr, UINT32 u4NumRefIdxL0, UINT32 u4NumRefIdxL1)
{
    ASSERT(prSliceHdr!=NULL);
    
    prSliceHdr->ai4DeltaPicOrderCnt[0] = 0;
    prSliceHdr->ai4DeltaPicOrderCnt[1] = 0;
    prSliceHdr->u4RedundantPicCnt = 0;  
    prSliceHdr->u4DisableDeblockingFilterIdc = 0;  
    prSliceHdr->i4SliceAlphaC0OffsetDiv2 = 0;    
    prSliceHdr->i4SliceBetaOffsetDiv2 = 0;    
    prSliceHdr->u4NumRefIdxL0ActiveMinus1 = u4NumRefIdxL0;
    prSliceHdr->u4NumRefIdxL1ActiveMinus1 = u4NumRefIdxL1;
    prSliceHdr->u4LongTermFrameIdx = 0xffffffff; 
    prSliceHdr->u4LongTermPicNum = 0;  
    prSliceHdr->fgDirectSpatialMvPredFlag = FALSE;
    prSliceHdr->u4IdrPicId = 0;
}

// *********************************************************************
// Function    : void vInitSliceHdr()
// Description : 
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vHrdParameters(UINT32 u4VDecID, VDEC_AVC_HRD_PARAM_T* prHrdPara)
{
    UINT32 u4SchedSelIdx;

    ASSERT(prHrdPara!=NULL);
    prHrdPara->u4CpbCntMinus1 = _VDEC_UeCodeNum(u4VDecID);
    prHrdPara->u4BitRateScale = _VDEC_AVCGetRealShtBitStream(u4VDecID, 4);
    prHrdPara->u4CpbSizeScale = _VDEC_AVCGetRealShtBitStream(u4VDecID, 4);
    if(prHrdPara->u4CpbCntMinus1 >= MAXIMUMVALUEOFcpb_cnt)
    {
        LOG(2, "HrdPrs Err\n");
        return;
    }
    for(u4SchedSelIdx=0; u4SchedSelIdx<=  prHrdPara->u4CpbCntMinus1; u4SchedSelIdx++)
    {
        prHrdPara->u4BitRateValueMinus1[u4SchedSelIdx] = _VDEC_UeCodeNum(u4VDecID);
        prHrdPara->u4CpbSizeValueMinus1[u4SchedSelIdx] = _VDEC_UeCodeNum(u4VDecID);
        prHrdPara->fgCbrFlag[u4SchedSelIdx] = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    }
    prHrdPara->u4InitialCpbRemovalDelayLengthMinus1 = _VDEC_AVCGetRealShtBitStream(u4VDecID, 5);  
    prHrdPara->u4CpbRemovalDelayLengthMinus1 = _VDEC_AVCGetRealShtBitStream(u4VDecID, 5);  
    prHrdPara->u4DpbOutputDelayLengthMinus1 = _VDEC_AVCGetRealShtBitStream(u4VDecID, 5);  
    prHrdPara->u4TimeOffsetLength = _VDEC_AVCGetRealShtBitStream(u4VDecID, 5);  
}

// *********************************************************************
// Function    : void vDec_Ref_Pic_Marking(void)
// Description : mark ref pic
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vRefPicMarking(UINT32 u4VDecID, BOOL bIsIDR, VDEC_AVC_SLICE_HDR_T* prSliceHdr)
{    
    UINT32 u4Cnt;

    ASSERT(prSliceHdr!=NULL);

    if(bIsIDR)
    {
        prSliceHdr->fgNoOutputOfPriorPicsFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        prSliceHdr->fgLongTermReferenceFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSliceHdr->fgLongTermReferenceFlag)
        {
        }
        else
        {
        }
    }
    else
    {
        prSliceHdr->fgAdaptiveRefPicMarkingModeFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSliceHdr->fgAdaptiveRefPicMarkingModeFlag)
        {
            u4Cnt = 0;
            do
            {
                if(u4Cnt >= 17)
                {
                    LOG(2, "Error! MMCO OperationNum >= 17\n");
                    break;
                }            
                prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] = _VDEC_UeCodeNum(u4VDecID);
                if((prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 1))
                {
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 8);
                }
                else if(prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 2)
                {
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 8);
                }
                else if(prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 3)
                {
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 8);
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 16);          
                }
                else if(prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 4)
                {
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 8);                    
                }
                else if(prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 5)
                {
                    prSliceHdr->fgMmco5 = TRUE;
                }
                else if(prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] == 6)
                {
                    prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] |= (_VDEC_UeCodeNum(u4VDecID) << 8);          
                }        
                u4Cnt ++;
            }
            while (prSliceHdr->au4MemoryManagementControlOperation[u4Cnt-1] != 0);
        }
    }
}


// *********************************************************************
// Function    : void vScalingList(void)
// Description : 
// Parameter   : None
// Return      : None
// *********************************************************************
void _vScalingList(UINT32 u4VDecID, CHAR* pcScalingList, UINT32 u4SizeOfScalingList, BOOL *fgUseDefaultScalingMatrixFlag)
{
    UINT32 i;
    UINT8 u1Scanj;
    INT32 i4LastScale;
    INT32 i4NextScale;
    INT32 i4DeltaScale;

     ASSERT(pcScalingList!=NULL);
     ASSERT(fgUseDefaultScalingMatrixFlag!=NULL);
     
    i4LastScale = 8;
    i4NextScale = 8;

    for(i=0; i<u4SizeOfScalingList; i++)
    {
        u1Scanj = (UINT8)((u4SizeOfScalingList==16) ? ZZ_SCAN[i]:ZZ_SCAN8[i]);
    
        if(i4NextScale != 0)
        {
            i4DeltaScale = _VDEC_SeCodeNum(u4VDecID);
            i4NextScale = (i4LastScale + i4DeltaScale + 256) % 256;
            *fgUseDefaultScalingMatrixFlag = ((u1Scanj == 0) && (i4NextScale == 0))? TRUE : FALSE;
        }
        pcScalingList[u1Scanj] = (CHAR)((i4NextScale == 0)? i4LastScale : i4NextScale);
        i4LastScale = pcScalingList[u1Scanj];
    }
}


// *********************************************************************
// Function    : void Set_ScalingList(pRBSP_BitStream pRBSP_BitStreamInst, CHAR* pcScalingList, 
                    //UINT32 u4SizeOfScalingList, BOOL *fgUseDefaultScalingMatrixFlag)
// Description : scaling list
// Parameter   : None
// Return      : None
// *********************************************************************
#ifdef CC_SUPPORT_VDEC_PREPARSE
void Set_ScalingList(pRBSP_BitStream pRBSP_BitStreamInst, CHAR* pcScalingList, UINT32 u4SizeOfScalingList, BOOL *fgUseDefaultScalingMatrixFlag)
{
    UINT32 i;
    UINT8 u1Scanj;
    INT32 i4LastScale;
    INT32 i4NextScale;
    INT32 i4DeltaScale;

    ASSERT(pRBSP_BitStreamInst !=NULL);
    ASSERT(pcScalingList!=NULL);
    ASSERT(fgUseDefaultScalingMatrixFlag!=NULL);

    i4LastScale = 8;
    i4NextScale = 8;

    for(i=0; i<u4SizeOfScalingList; i++)
    {
        u1Scanj = (UINT8)((u4SizeOfScalingList==16) ? ZZ_SCAN[i]:ZZ_SCAN8[i]);

        if(i4NextScale != 0)
        {
            i4DeltaScale = RBSPGetSE(pRBSP_BitStreamInst);//se(v)
            i4NextScale = (i4LastScale + i4DeltaScale + 256) % 256;
            *fgUseDefaultScalingMatrixFlag = ((u1Scanj == 0) && (i4NextScale == 0))? TRUE : FALSE;
        }
        pcScalingList[u1Scanj] = (CHAR)((i4NextScale == 0)? i4LastScale : i4NextScale);
        i4LastScale = pcScalingList[u1Scanj];
    }
}
#endif


static BOOL _vInterpretSEI_PicTiming(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm)
{
    VDEC_AVC_SEI_T* prSEI;
    VDEC_AVC_VUI_T* prVUI;
    UINT8 u1Idx;
    VDEC_ES_INFO_T *prVdecEsInfo;
    
    ASSERT(prDecPrm != NULL);
    prSEI = &prDecPrm->rSEI;
    if((prDecPrm->prSPS != NULL) && prDecPrm->prSPS->fgSPSValid)
    {
        prVUI = &prDecPrm->prSPS->rVUI;
    }
    else
    {
        return FALSE;
    }
    
    if(prVUI->fgNalHrdParametersPresentFlag)
    {
        prSEI->u4CpbRemovalDelay = _VDEC_AVCGetRealShtBitStream
            (u4VDecID, prVUI->rNalHrdParameters.u4CpbRemovalDelayLengthMinus1 + 1);
        prSEI->u4DpbOutputDelay = _VDEC_AVCGetRealShtBitStream
            (u4VDecID, prVUI->rNalHrdParameters.u4DpbOutputDelayLengthMinus1 + 1);        
    }
    else if(prVUI->fgVclHrdParametersPresentFlag)
    {
        prSEI->u4CpbRemovalDelay = _VDEC_AVCGetRealShtBitStream
            (u4VDecID, prVUI->rVclHrdParameters.u4CpbRemovalDelayLengthMinus1 + 1);
        prSEI->u4DpbOutputDelay = _VDEC_AVCGetRealShtBitStream
            (u4VDecID, prVUI->rVclHrdParameters.u4DpbOutputDelayLengthMinus1 + 1);
    }

    if(prVUI->fgPicStructPresentFlag)
    {
        prSEI->u1PicStruct = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 4);
        if(prSEI->u1PicStruct > 8)
        {
            LOG(2, "PicStructPrs Err\n");
            return TRUE;
        }
        prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
        prVdecEsInfo->u1SEIPicStruct = prSEI->u1PicStruct;
        prVdecEsInfo->fgSEIValid = TRUE;
        
        ASSERT(NumClockTS[prSEI->u1PicStruct] < 5);
        for(u1Idx = 0; (u1Idx < NumClockTS[prSEI->u1PicStruct]) && (u1Idx < 5); u1Idx++)
        {
            prSEI->u1ClkTimeStampFlag[u1Idx] = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
            if(prSEI->u1ClkTimeStampFlag[u1Idx] > 0)
            {
                prSEI->u1CtType = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 2);
                prSEI->u1NuitFieldBasedFlag = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                prSEI->u1CountingType = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 5);
                prSEI->u1FullTimeStampFlag = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                prSEI->u1DiscontinuityFlag = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                prSEI->u1CntDroppedFlag = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                prSEI->u1NFrames = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 8);
                if(prSEI->u1FullTimeStampFlag > 0)
                {
                    prSEI->u1SecondsValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 6);
                    prSEI->u1MinutesValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 6);
                    prSEI->u1HoursValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 5);
                }
                else
                {
                    if(_VDEC_AVCGetRealShtBitStream(u4VDecID, 1) > 0)    //seconds_flag
                    {
                        prSEI->u1SecondsValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 6);
                        if(_VDEC_AVCGetRealShtBitStream(u4VDecID, 1) > 0)    //minutes_flag
                        {
                            prSEI->u1MinutesValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 6);
                            if(_VDEC_AVCGetRealShtBitStream(u4VDecID, 1) > 0)    //hours_flag
                            {
                                prSEI->u1HoursValue = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 5);
                            }
                        }
                    }
                }
                if(prVUI->rNalHrdParameters.u4TimeOffsetLength > 0)
                {
                    prSEI->i1TimeOffset = (INT8)_VDEC_AVCGetRealShtBitStream(
                        u4VDecID, prVUI->rNalHrdParameters.u4TimeOffsetLength);
                }
                else
                {
                    prSEI->i1TimeOffset = 0;
                }
            }
        }
    }

    return TRUE;
}


BOOL _vCheckAllocFBG(VDEC_AVC_DecPrm* prDecPrm)
{
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UCHAR ucFbgType;
    UINT8 u1ViewIdx;
    UINT32 u4FbgWidth;
    UINT32 u4FbgHeight;
    FBM_CREATE_FBG_PAR_T rPar;
    BOOL fgReCreateFBG = FALSE;

    ASSERT(prDecPrm!=NULL);
    if(prDecPrm == NULL)
    {
        return FALSE;
    }
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if(u1ViewIdx >= MAX_VIEW_NUM)   //dependent view, do nothing
    {
        return FALSE;
    }    

    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    ASSERT(prVdecEsInfo!=NULL);
    ASSERT(prVdecEsInfoKeep!=NULL);
    if((prVdecEsInfo == NULL) || (prDecPrm->prSPS == NULL) ||
       (prVdecEsInfoKeep == NULL))
    {
        return FALSE;
    }
    
    prDecPrm->u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prDecPrm->u4PicH = ((2 -prDecPrm->prSPS->fgFrameMbsOnlyFlag)*(prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
    
    if((prDecPrm->u4PicW > MAX_SUPPORT_W) || (prDecPrm->u4PicH > MAX_SUPPORT_H))
    {
        LOG(2, "Resolution not support %dx%d!!\n", prDecPrm->u4PicW, prDecPrm->u4PicH);
        return FALSE;
    }
    if((u1ViewIdx > 0) && (prDecPrm->aucFbgId[0] == FBM_FBG_ID_UNKNOWN))
    {
        LOG(2, "Base view invalid!!\n");
        return FALSE;
    }

    fgReCreateFBG = VDEC_ChkSeamlessModeChg(prDecPrm->ucEsId, prDecPrm->u4PicW, prDecPrm->u4PicH);
    prDecPrm->fgSeamlessDisp = ((prVdecEsInfo->eSeamlessMode != SEAMLESS_NONE) && !(prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV));

    if ((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) || 
        (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH))
    {
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS)
        {
            fgReCreateFBG = TRUE;
        }
        else if (prVdecEsInfo->fgIsTPT && prVdecEsInfo->fgSeamlessPlay)
        {
            fgReCreateFBG = FALSE;
        }
        else if (prVdecEsInfo->eSeamlessMode == SEAMLESS_NONE)
        {
            fgReCreateFBG = TRUE;
        }
    }

    if ((prDecPrm->aucFbgId[u1ViewIdx] == FBM_FBG_ID_UNKNOWN) || fgReCreateFBG)
    {
        prDecPrm->bPreAllocFBG = TRUE;
        _VDEC_FlushDPB(prDecPrm);

        //FBM_ReleaseGroup(prDecPrm->aucFbgId[u1ViewIdx]);
        //MVC setting
        if(prDecPrm->u1NalUnitType == CODED_SLICE_EXT)
        {
            prDecPrm->fgIsMVC = TRUE;
            prVdecEsInfo->e3DType = VDEC_3D_MVC;
            #ifdef CC_DUAL_CORE_DEC
            prVdecEsInfo->fgLAEBufAllocated = TRUE;
            #endif
        }

        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));

#ifdef ENABLE_MULTIMEDIA
        if (prVdecEsInfo->eSeamlessMode != SEAMLESS_NONE)
        {
            rPar.u4TargetW = prVdecEsInfo->u4SeamlessWidth;
            rPar.u4TargetH = prVdecEsInfo->u4SeamlessHeight;
        }
        
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS)
        {
            rPar.ucCftFbgId = prDecPrm->aucFbgId[u1ViewIdx] + 1;
        }
        else if (prDecPrm->aucFbgId[u1ViewIdx] != FBM_FBG_ID_UNKNOWN)
        {
            FBM_ReleaseGroup(prDecPrm->aucFbgId[u1ViewIdx]);
            prDecPrm->aucFbgId[u1ViewIdx] = FBM_FBG_TYPE_UNKNOWN;
            rPar.ucCftFbgId = 0;
        }
        rPar.eSeamlessMode = prVdecEsInfo->eSeamlessMode;
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS)
        {
            u4FbgWidth = prVdecEsInfo->u4SeamlessWidth;
            u4FbgHeight = prVdecEsInfo->u4SeamlessHeight;
            prDecPrm->fgReCreateFBG = FALSE;
        }        
        else
#endif
        {
            u4FbgWidth = prDecPrm->u4PicW;
            u4FbgHeight = prDecPrm->u4PicH;
            if (prDecPrm->aucFbgId[u1ViewIdx] != FBM_FBG_ID_UNKNOWN)
            {
                prDecPrm->fgReCreateFBG = TRUE;
            }
        }
        rPar.u1VdecId = (UINT8)prDecPrm->ucEsId;

        ucFbgType = FBM_SelectGroupType(u4FbgWidth, u4FbgHeight);
        #ifdef ENABLE_MULTIMEDIA
        if(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE ||
            prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HW_DIRECT_PLAY)
        {
            rPar.u1AppMode = FBM_FBG_APP_SKYPE;
        }
        else 
        #endif
        if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
        {
            rPar.u1AppMode = FBM_FBG_APP_OMX;
        }
        else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
        {
            rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
        }
        else if((prVdecEsInfo->e3DType == VDEC_3D_MVC) ||
            (prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM)||
            (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
        {
            rPar.u1AppMode = FBM_FBG_APP_Frame_Pack_3D;           
        }
        else
        {
            LOG(0, "CheckAllocFBG App Mode Normal\n");
            rPar.u1AppMode = FBM_FBG_APP_NORMAL;
        }
#ifdef CC_H264_LV5_SUPPORT_SINGLE_POOL
        //set memory enlarge mode if need more then 12MB(level4.0) for DPB
        if((((prDecPrm->u4PicW * prDecPrm->u4PicH * 3) >> 1) * prDecPrm->prSPS->u4NumRefFrames) > (12<<20))
        {
            rPar.u1VdecProfile = FBM_H264_LV5_PROFILE;
        }
#endif

        if (prVdecEsInfoKeep->fgVPushFBGFromInst)
        {
            LOG(3, "fgCreateFromInst == TRUE\n");
            rPar.fgCreateFromInst = TRUE;
        }
        if (prVdecEsInfo->u4RenderPicCnt)
        {
            rPar.fgThumbnailMode = TRUE;
        }            
#if (defined (CC_MT5890))
        if (prDecPrm->fgIsUFOMode)
        {
            rPar.fgUFO = TRUE;
        }
        if (prDecPrm->fgIs10BitMode)
        {
            rPar.fg10Bit = TRUE;
        }
#endif
        prDecPrm->aucFbgId[u1ViewIdx] = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_H264, u4FbgWidth,
                                                            u4FbgHeight, &rPar);

        if(u1ViewIdx == 0)
        {
            prVdecEsInfo->ucFbgId = prDecPrm->aucFbgId[0];
            prVdecEsInfo->fgRsvData = TRUE;
        }
        else
        {
            prVdecEsInfo->ucSubFbgId = prDecPrm->aucFbgId[u1ViewIdx];
        }
        if(prDecPrm->aucFbgId[u1ViewIdx] == FBM_FBG_ID_UNKNOWN)
        {
            LOG(2, "CreateFbg Fail!!\n");
            return FALSE;
        }
#ifdef CC_DUAL_CORE_DEC
        if (prDecPrm->ucDualCoreMode)
        {
            if (!prVdecEsInfo->fgLAEBufAllocated)
            {
                prDecPrm->rLAEDecParam.u4PicH = prDecPrm->u4PicH;
                prDecPrm->rLAEDecParam.u4PicW = prDecPrm->u4PicW;
                if (!fgLAEAllocateInfoBuf(prDecPrm->ucEsId, VDEC_FMT_H264, &prDecPrm->rLAEDecParam))
                {
                    //error occur
                    LOG(2, "Create LAE Buffer Fail!!\n");     
                    return FALSE;
                }                 
            }
        }
#endif
        if(prVdecEsInfo->fgPlaybackNotDisplay)
        {
            FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
        }
        if(prVdecEsInfoKeep->fgVPush)
        {
            if(prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
            }
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, prDecPrm->ucEsId);
            if(prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, prDecPrm->ucEsId);
            }   

            if (prDecPrm->fgIsMVC)  //MVC case
            {
                FBM_FbgChgNotify(prDecPrm->aucFbgId[u1ViewIdx], u1ViewIdx);  //set 3D type before mode change
            }
            else if (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM)
            {
                FBM_FbgChgNotify(prDecPrm->aucFbgId[u1ViewIdx], prVdecEsInfoKeep->rPesInfo.ucEsId);
            }
            else
            {
                FBM_FbgChgNotify(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucEsId); 
            }                
        }
        else if (prVdecEsInfoKeep->fgFrameToUser)
        {
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_Vdec_IFrameDecodeDone, prDecPrm->ucEsId);
        }
        
        if(prVdecEsInfo->fgMMPlayback==FALSE && prVdecInfo->pfPicInfo)
        {
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_DISPLAY_START, (UINT32)_Vdec_FrameDisplayStart, prDecPrm->ucEsId);
        }

        prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW = prDecPrm->u4PicW;
        prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH = prDecPrm->u4PicH;
        prDecPrm->ucPreAllocFbId =  FBM_FB_ID_UNKNOWN;
    }
    return TRUE;
}

static BOOL _vCheckAllocFB(VDEC_AVC_DecPrm* prDecPrm, BOOL* pbReqFb)
{
#ifdef FBM_DPB_CTRL
    FBM_PIC_HDR_T * prFbmPicHdr;
#endif
    BOOL fgRefChg = FALSE;
    UINT8 u1ViewIdx;
    UINT32 i;
    INT32 i4MinPOC;
    UINT32 u4MinPOCFBufIdx = 0xFF, u4ClrPOCFBufIdx = 0xFF, u4RefPOCFBufIdx = 0;
    UCHAR  ucFbStatus;
     UINT32 staussss = 0;
     
#ifdef ENABLE_MULTIMEDIA
     VDEC_ES_INFO_T* prVdecEsInfo;
#endif

    ASSERT(prDecPrm!=NULL);
    ASSERT(pbReqFb!=NULL);
    
    if(prDecPrm == NULL)
    {
        return FALSE;
    }
    else if(prDecPrm->prSliceHdr == NULL)
    {
        return FALSE;
    }
    else if(prDecPrm->prCurrFBufInfo == NULL)   //first frame
    {
        *pbReqFb = TRUE;
        return TRUE;
    }
    
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if(u1ViewIdx >= MAX_VIEW_NUM)   //dependent view, do nothing
    {
        return TRUE;
    }    
    
#ifdef ENABLE_MULTIMEDIA
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);

    if(!prVdecEsInfo)
    {
        return FALSE;
    }
#endif
    if(((prDecPrm->rLastInfo[u1ViewIdx].fgLastIsRef && (!fgIsRefPic(prDecPrm->u1NalRefIdc))) ||
        (!prDecPrm->rLastInfo[u1ViewIdx].fgLastIsRef && fgIsRefPic(prDecPrm->u1NalRefIdc))) ||
        fgIsIDRPic(prDecPrm->u1NalUnitType)) //error handling for AVC strill picture frame drop (IDR missing)
    {
        fgRefChg = TRUE;
    }
    
    *pbReqFb = FALSE;
    if((prDecPrm->prCurrFBufInfo->u1FBufStatus == NO_PIC)
        || (prDecPrm->prCurrFBufInfo->u1FBufStatus & prDecPrm->u1PicStruct)
        || (prDecPrm->prSliceHdr->u4FrameNum != prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum)
        || fgRefChg)
    {
        // Check if DPB full
        i4MinPOC = 0x7fffffff;
        for(i=0; i<prDecPrm->u4MaxFBufNum; i++)
        {
            ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][i].ucFbId);
            if((prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus == NO_PIC)
            #ifdef ENABLE_MULTIMEDIA
            ||((ucFbStatus == FBM_FB_STATUS_EMPTY)&&((prDecPrm->ucSkipMode == IPMode)||(prDecPrm->ucSkipMode == IMode))&&(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV))
            #endif
                )
            {
                i4MinPOC = 0x7fffffff;
                u4MinPOCFBufIdx = i;
                break;
            }
            // miew: need to take care of field empty
            else if(fgIsNonRefFBuf(prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType, 
                prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType, prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType))
            {
                if((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) && (ucFbStatus != FBM_FB_STATUS_LOCK))
                {
                    if(i4MinPOC > prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC)
                    {
                        i4MinPOC = prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC;
                        u4MinPOCFBufIdx = i;
                    }
                    else
                    {
                        u4ClrPOCFBufIdx = i;
                    }
                }
            }
            else if((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) && (ucFbStatus != FBM_FB_STATUS_LOCK))
            {
                u4RefPOCFBufIdx = i;
            }
        }

        #if 0   //move to _VDEC_SliceNonIDR
        //clr current frame status if still in the middle of decoding process
        if(FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId)
        == FBM_FB_STATUS_DECODE)
        {
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
            FBM_FB_STATUS_READY);
        }
        #endif
        // error handling if can't find non-ref pic with minimun poc and not in displayQ
        if(u4MinPOCFBufIdx == 0xFF)
        {
            if(u4ClrPOCFBufIdx == 0xFF)
            {
                u4MinPOCFBufIdx = u4RefPOCFBufIdx;
            }
            else
            {
                u4MinPOCFBufIdx = u4ClrPOCFBufIdx;
            }
        }
        // No empty DPB, 1 FBuf output
        if(prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus != NO_PIC)
        {
            LOG(2, "///// ClrBuf in AllocFBuf, Idx %d , Status %d(>0)\n",
                u4MinPOCFBufIdx,
                (UINT32)FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId));
            //find an empty fb entry, put non-ref buf to empty
            staussss = (UINT32)FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
            if(staussss == FBM_FB_STATUS_DISPLAYQ)
            {                
                printf("gogogogo\n");
            }
            else
            {            
                if(prDecPrm->fgIsMVC && (prDecPrm->ucSkipMode == IMode) && !prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgIsNotDisp)
                {
                    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                    if(prFbmPicHdr)
                    {
                        if(prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus == TOP_FIELD)
                        {
                            prFbmPicHdr->fgTrickTopField = TRUE;
                        }
                        else
                        { 
                            prFbmPicHdr->fgTrickTopField = FALSE;
                        }
                    }
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, 
                    FBM_FB_STATUS_DISPLAYQ);
                }
                else
                {
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, 
                    FBM_FB_STATUS_EMPTY);
                }
            }
            _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + ((UINT32)u1ViewIdx<<8));
        }
        prDecPrm->u1DecFBufIdx = (UINT8)u4MinPOCFBufIdx;
        // Only new alloc needs to update current fbuf idx
        // set current buf index
        if(prDecPrm->u1DecFBufIdx > 17)
        {
            ASSERT(prDecPrm->u1DecFBufIdx <= 17);
            return FALSE;
        }
        *pbReqFb = TRUE;
    }    
        
    return TRUE;
    
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

/** .
*@param .
 */
UINT8 _VDEC_AVCGetStartCode(UINT32 u4VDecID, UINT8 u1Num)
{
    UINT32 u4Temp, i;
    BOOL fgVLDRem03;
    UINT32 u4RetryNum = 0x100000;
    
    u4Temp = _VDEC_AVCGetBitStream(u4VDecID, 0);
    fgVLDRem03 = (u1Num == BARREL1)? (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_CTRL) >> 31) : (u4VDecReadAVCVLD(u4VDecID, RO_AVLD_2ND_CTRL) & 0x40);

    if (((u4Temp >> 8) != START_CODE) || fgVLDRem03)
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_FSSR, FW_SEARCH_START_CODE);
        for (i = 0; i < u4RetryNum; i++)
        {
            if ((u4VDecReadAVCVLD(u4VDecID, RW_AVLD_FSSR) & 0x1) == 0)
            {
                break;
            }
        }
        fgVLDRem03 = (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) >> 11) & 0x1;
        if (i == u4RetryNum && !fgVLDRem03)
        {
            LOG(0, "Can not find AVC start code\n");
            ASSERT(0);
        }
    }
    u4Temp = _VDEC_AVCGetShtBitStream(u4VDecID, 32);
    return (UINT8)(u4Temp & 0xff);
}


BOOL _vCheckRes(VDEC_AVC_DecPrm* prDecPrm, BOOL fgIsField,VDEC_ES_INFO_T *prVdecEsInfo)
{
    UINT32 u4fps,u4PicW,u4PicH;
    u4fps = 0;
    u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    u4PicH = ((2 -prDecPrm->prSPS->fgFrameMbsOnlyFlag)*(prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
    if(prDecPrm->prSPS->rVUI.u4NumUnitsInTick > 0)
    {
        u4fps = (prDecPrm->prSPS->rVUI.u4TimeScale/prDecPrm->prSPS->rVUI.u4NumUnitsInTick) >> 1;
    }
    switch(u4fps)
    {
        case 24:
            break;
        case 23:
            break;
        case 25:
            break;
        case 30:
            break;
        case 29:
            break;
        case 50:
            break;
        case 60:
            break;
        case 59:
            break;
        default:
            if((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                (prVdecEsInfo->rMMSeqInfo.e_frame_rate != VDEC_SRC_FRAME_RATE_UNKNOWN))
            {
                switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
                {
                    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                        u4fps = 24;
                        break;
                    case VDEC_SRC_FRAME_RATE_24:
                        u4fps = 24;
                        break;
                    case VDEC_SRC_FRAME_RATE_25:
                        u4fps = 25;
                        break;
                    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                        u4fps = 30;
                        break;
                    case VDEC_SRC_FRAME_RATE_30:
                        u4fps = 30;
                        break;
                    case VDEC_SRC_FRAME_RATE_50:
                        u4fps = (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_50 : MPEG_FRAME_RATE_25;
                        if(u4fps == MPEG_FRAME_RATE_50)
                        {
                            u4fps = 50;
                        }
                        else
                        {
                            u4fps = 25;
                        }
                        break;
                    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                        u4fps = (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_60_ : MPEG_FRAME_RATE_30_;
                        if(u4fps == MPEG_FRAME_RATE_60_)
                        {
                             u4fps = 60;
                        }
                        else
                        {
                             u4fps = 30;
                        }
                        break;
                    case VDEC_SRC_FRAME_RATE_60:
                        u4fps = (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_60 : MPEG_FRAME_RATE_30;
                        if(u4fps == MPEG_FRAME_RATE_60)
                        {
                            u4fps = 60;
                        }
                        else
                        {
                            u4fps = 30;
                        }
                        break;
                    default:
                        u4fps = 0;
                        break;
                }
            }
            else
            {
                u4fps = 0;
            }
            break;
    }
    if(fgIsField)
    {
        if((u4PicW > 720)&&(u4PicH > 720)&&(u4fps > 60))
        {
            return TRUE;
        }
    }
    else
    {
        if((u4PicW > 720)&&(u4PicH > 720)&&(u4fps > 30))
        {
            return TRUE;
        }
    }
    return FALSE;
}


// *********************************************************************
// Function    : void vSeq_Par_Set_Rbsp(void)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_SPSrbsp(UINT32 u4VDecID, VDEC_H264_INFO_T* prVdec, INT32* pi4RetValue)
{
    UINT32 u4Temp,u4FbCnt = 0;
    UINT32 i,j;
    UINT32 u4SeqParameterSetId,u4YAddr, u4CAddr, u4MVAddr,u4DramPicSize,u4DramPicArea;
    VDEC_AVC_SPS_T* prSPS;
    UCHAR  u1WaitRAPCnt = 0, u1WaitRAPRefCnt = 0,u1WaitRAPICnt = 0;
    #ifdef FBM_DPB_CTRL
    VDEC_AVC_DecPrm* prDecPrm;
    #endif
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    UINT32 u4ViewIdx, u4FbIdx, u4FbNum;
    #endif
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;

    ASSERT(prVdec!=NULL);
    ASSERT(pi4RetValue!=NULL);
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse SPS Start\n");
#endif
    *pi4RetValue = 1;
    prDecPrm = &prVdec->rDecParam;
    if(prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(3, "SPS ViewIdx Err\n");
        *pi4RetValue = 1;
        return;
    }       
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    if(prVdecEsInfo == NULL)
    {
        return;
    }

    u4Temp = (_VDEC_AVCGetShtBitStream(u4VDecID, 24) >> 8);
    if((u4Temp & 0x700) > 0)  // [11:8]
    {
        //*pi4RetValue = 1;
        LOG(2, "reserved_zero_4bits error in Seq_Par_Set_Rbsp, 0x%x\n", u4Temp);
        //return;
    }

    // 1st
    u4SeqParameterSetId = _VDEC_UeCodeNum(u4VDecID);
    if(u4SeqParameterSetId < 32)
    {      
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        if(prDecPrm->u1NalUnitType == SUB_SPS)
        {
            prSPS = &prVdec->arSUBSPS[u4SeqParameterSetId];
        }
        else
        if((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) &&
            (prVdec->rPesInfo.ucEsId > 0))
        {
            prSPS = &prVdec->arSUBSPS[u4SeqParameterSetId];
        }
        else
        #endif
        {
            prSPS = &prVdec->arSPS[u4SeqParameterSetId];
        }
        if(prSPS->fgSPSValid)
        {
            prVdec->u1SPSIDBK = u4SeqParameterSetId;
            prVdec->fgisvalid = TRUE;
            x_memcpy((void*)&prVdec->rSPSBK, (void*)prSPS, sizeof(VDEC_AVC_SPS_T));            
        }
        x_memset((void*)prSPS, 0, sizeof(VDEC_AVC_SPS_T));
        prVdec->rDecParam.prSPS = prSPS;
        prSPS->fgSPSValid = FALSE;    //turn on the flag after all parameters are set

        prSPS->u4ProfileIdc = (u4Temp >> 16);                              // [23:16]
        prSPS->fgConstrainedSet0Flag = (u4Temp >> 15) & 0x1;      // [15]
        prSPS->fgConstrainedSet1Flag = (u4Temp >> 14) & 0x1;      // [14]
        prSPS->fgConstrainedSet2Flag = (u4Temp >> 13) & 0x1;      // [13]
        prSPS->fgConstrainedSet3Flag = (u4Temp >> 12) & 0x1;      // [12]
        prSPS->u4LevelIdc = (u4Temp & 0xff);                              // [7:0]
        
        _VDEC_GetProfileLevel(prSPS, prVdecEsInfo);
        if((!fgChkProfile((UINT8)prSPS->u4ProfileIdc)) || (!fgChkLevel((UINT8)prSPS->u4LevelIdc)))
        {
            if((prVdecEsInfoKeep)&&(prVdecEsInfoKeep->fgVPush)&&(prVdec->rDecParam.fgIsMMPlay))
            {
                *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;   // add NOT_SUPPORT filter for MMP
            }
            else
            {
                *pi4RetValue = (INT32)E_VDEC_DISP_NOT_READY;   
            }
            LOG(2, "err at SPS Profile %d/Level %d Ret %d\n", prSPS->u4ProfileIdc, prSPS->u4LevelIdc,*pi4RetValue);
            prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;
            return;   //todo, the error may not affect decoding directly, need more test
        }
        prSPS->u4SeqParameterSetId = u4SeqParameterSetId;
    }
    else
    {
        *pi4RetValue = 1;
        LOG(2, "err in SPS Num\n");
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;
        return;
    }
    prDecPrm->u4DPBSize = _InitSPS(prSPS);
    u1WaitRAPCnt = prDecPrm->rSEI.u1WaitRAPCnt;
    u1WaitRAPRefCnt = prDecPrm->rSEI.u1WaitRAPRefCnt;
    u1WaitRAPICnt = prDecPrm->rSEI.u1WaitRAPICnt;
    x_memset(&prDecPrm->rSEI, 0, sizeof(VDEC_AVC_SEI_T));
    prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_FRAME_UNKNOWN;
    prDecPrm->rSEI.u1WaitRAPCnt = u1WaitRAPCnt;
    prDecPrm->rSEI.u1WaitRAPRefCnt = u1WaitRAPRefCnt;
    prDecPrm->rSEI.u1WaitRAPICnt = u1WaitRAPICnt;

    // 2nd
    if((prSPS->u4ProfileIdc == FREXT_HP) 
        || (prSPS->u4ProfileIdc == FREXT_Hi10P) 
        || (prSPS->u4ProfileIdc == FREXT_Hi422)
        || (prSPS->u4ProfileIdc == FREXT_Hi444)
        || (prSPS->u4ProfileIdc == MVC_HIGH)
        || (prSPS->u4ProfileIdc == MVC_STEREO))
    {
        prSPS->u4ChromaFormatIdc = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSPS->u4ChromaFormatIdc, 3);
        if(prSPS->u4ChromaFormatIdc == 3)
        {
            prSPS->fgResidualColorTransformFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        }
        prSPS->u4BitDepthLumaMinus8 = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSPS->u4BitDepthLumaMinus8, 4);       
        prSPS->u4BitDepthChromaMinus8 = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSPS->u4BitDepthChromaMinus8, 4);        
#if (defined (CC_MT5890))
        if ((2 == prSPS->u4BitDepthLumaMinus8) && (2 == prSPS->u4BitDepthChromaMinus8))
        {
            prDecPrm->fgIs10BitMode = TRUE;
        }
#endif
        prSPS->fgQpprimeYZeroTransformBypassFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);        
        prSPS->fgSeqScalingMatrixPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);

        if(prSPS->fgSeqScalingMatrixPresentFlag)
        {
            for(i = 0; i < 8; i ++)
            {
                prSPS->fgSeqScalingListPresentFlag[i] = _VDEC_AVCGetBitStreamFlg(u4VDecID);
                if(prSPS->fgSeqScalingListPresentFlag[i])
                {
                    if(i < 6)
                    {
                        _vScalingList(u4VDecID, prSPS->ppcScalingList4x4[i],16, &prSPS->fgUseDefaultScalingMatrix4x4Flag[i]);
                    }
                    else
                    {
                        _vScalingList(u4VDecID, prSPS->ppcScalingList8x8[i-6],64, &prSPS->fgUseDefaultScalingMatrix8x8Flag[i-6]);
                    }
                }
            }
        }
    }

    prSPS->u4Log2MaxFrameNumMinus4 = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prSPS->u4Log2MaxFrameNumMinus4, 12);
    prSPS->u4MaxFrameNum = 1 << (prSPS->u4Log2MaxFrameNumMinus4 + 4);
    prSPS->u4PicOrderCntType = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prSPS->u4PicOrderCntType, 2);
    if(prSPS->u4PicOrderCntType == 0)
    {
        prSPS->u4Log2MaxPicOrderCntLsbMinus4 = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSPS->u4Log2MaxPicOrderCntLsbMinus4, 12);
    }
    else if(prSPS->u4PicOrderCntType == 1)
    {
        prSPS->fgDeltaPicOrderAlwaysZeroFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);

        prSPS->i4OffsetForNonRefPic = _VDEC_SeCodeNum(u4VDecID);
        prSPS->i4OffsetForTopToBottomField = _VDEC_SeCodeNum(u4VDecID);
        prSPS->u4NumRefFramesInPicOrderCntCycle = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSPS->u4NumRefFramesInPicOrderCntCycle, 255);
        for(i=0 ; i<prSPS->u4NumRefFramesInPicOrderCntCycle; i++)
        {
            prSPS->ai4OffsetForRefFrame[i] = _VDEC_SeCodeNum(u4VDecID);
        }
    }
    prSPS->u4NumRefFrames = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prSPS->u4NumRefFrames, 16);

    prSPS->fgGapsInFrameNumValueAllowedFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prSPS->u4PicWidthInMbsMinus1 = _VDEC_UeCodeNum(u4VDecID);

    prSPS->u4PicHeightInMapUnitsMinus1 = _VDEC_UeCodeNum(u4VDecID);

    prSPS->fgFrameMbsOnlyFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prVdecEsInfo->fgSPSFrameMbsOnlyFlag = prSPS->fgFrameMbsOnlyFlag;
    prVdecEsInfo->fgSPSValid = TRUE;

    prSPS->fgMbAdaptiveFrameFieldFlag = FALSE;
    if(!prSPS->fgFrameMbsOnlyFlag)
    {
        prSPS->fgMbAdaptiveFrameFieldFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    }
    prSPS->fgDirect8x8InferenceFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);

    prSPS->fgFrameCroppingFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);

    if(prSPS->fgFrameCroppingFlag)
    {
        prSPS->u4FrameCropLeftOffset = _VDEC_UeCodeNum(u4VDecID);
        prSPS->u4FrameCropRightOffset = _VDEC_UeCodeNum(u4VDecID);

        prSPS->u4FrameCropTopOffset = _VDEC_UeCodeNum(u4VDecID);
        prSPS->u4FrameCropBottomOffset = _VDEC_UeCodeNum(u4VDecID);
        if(prSPS->u4ChromaFormatIdc == 1)   // 4:2:0
        {
            prSPS->u4FrameCropLeftOffset <<= 1;
            prSPS->u4FrameCropRightOffset <<= 1;

            prSPS->u4FrameCropTopOffset <<= 1;
            prSPS->u4FrameCropBottomOffset <<= 1;
        }
        else if(prSPS->u4ChromaFormatIdc == 2)   // 4:2:2
        {
            prSPS->u4FrameCropLeftOffset <<= 1;
            prSPS->u4FrameCropRightOffset <<= 1;
        }
    }
    else
    {
        prSPS->u4FrameCropLeftOffset = 0;
        prSPS->u4FrameCropRightOffset = 0;

        prSPS->u4FrameCropTopOffset = 0;
        prSPS->u4FrameCropBottomOffset = 0;
    }
    prSPS->fgVuiParametersPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    
    #if 1
    if(prSPS->fgVuiParametersPresentFlag)
    {
        prSPS->rVUI.fgAspectRatioInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgAspectRatioInfoPresentFlag)
        {
            prSPS->rVUI.u4AspectRatioIdc = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8);
            prVdecEsInfo->u4H264IdcInfo = prSPS->rVUI.u4AspectRatioIdc;
            if(prSPS->rVUI.u4AspectRatioIdc == 255) //Extended_SAR
            {
                prSPS->rVUI.u4SarWidth = _VDEC_AVCGetRealShtBitStream(u4VDecID, 16);
                prSPS->rVUI.u4SarHeight = _VDEC_AVCGetRealShtBitStream(u4VDecID, 16);
            }
        }
        prSPS->rVUI.fgOverscanInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgOverscanInfoPresentFlag)
        {
            prSPS->rVUI.fgOverscanAppropriateFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        }
        prSPS->rVUI.fgVideoSignalTypePresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgVideoSignalTypePresentFlag)
        {
            prSPS->rVUI.u4VideoFormat = _VDEC_AVCGetRealShtBitStream(u4VDecID, 3); 
            prSPS->rVUI.fgVideoFullRangeFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
            prSPS->rVUI.fgColourDescriptionPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
            if(prSPS->rVUI.fgColourDescriptionPresentFlag)
            {
                prSPS->rVUI.u4ColourPrimaries = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8); 
                prSPS->rVUI.u4TransferCharacteristics = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8); 
                prSPS->rVUI.u4MatrixCoefficients = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8); 
            }
        }
        prSPS->rVUI.fgChromaLocationInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgChromaLocationInfoPresentFlag)
        {
            prSPS->rVUI.u4ChromaSampleLocTypeTopField = _VDEC_UeCodeNum(u4VDecID); 
            prSPS->rVUI.u4ChromaSampleLocTypeBottomField = _VDEC_UeCodeNum(u4VDecID); 
        }
        prVdecEsInfoKeep->u4ChromaFormatIdc = prSPS->u4ChromaFormatIdc;
        prVdecEsInfoKeep->fgChromaLocationInfoPresentFlag = prSPS->rVUI.fgAspectRatioInfoPresentFlag;
        if (prSPS->rVUI.fgChromaLocationInfoPresentFlag)
        {
            prVdecEsInfoKeep->u4ChromaSampleLocTypeTopField = prSPS->rVUI.u4ChromaSampleLocTypeTopField;
            prVdecEsInfoKeep->u4ChromaSampleLocTypeBottomField = prSPS->rVUI.u4ChromaSampleLocTypeBottomField;
        }

        prSPS->rVUI.fgTimingInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgTimingInfoPresentFlag)
        {
            prSPS->rVUI.u4NumUnitsInTick = _VDEC_AVCGetRealShtBitStream(u4VDecID, 32); 
            prSPS->rVUI.u4TimeScale = _VDEC_AVCGetRealShtBitStream(u4VDecID, 32); 
            prVdecEsInfo->u4VUINumUnitsInTick = prSPS->rVUI.u4NumUnitsInTick;
            prVdecEsInfo->u4VUITimeScale = prSPS->rVUI.u4TimeScale;            
            prVdecEsInfo->fgVUIValid = TRUE;
                
            prSPS->rVUI.fgFixedFrameRateFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        }
        prSPS->rVUI.fgNalHrdParametersPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgNalHrdParametersPresentFlag)
        {
            _vHrdParameters(u4VDecID, &prSPS->rVUI.rNalHrdParameters);
        }
        prSPS->rVUI.fgVclHrdParametersPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgVclHrdParametersPresentFlag)
        {
            _vHrdParameters(u4VDecID, &prSPS->rVUI.rVclHrdParameters);
        }
        if(prSPS->rVUI.fgNalHrdParametersPresentFlag || prSPS->rVUI.fgVclHrdParametersPresentFlag)
        {
            prSPS->rVUI.fgLowDelayHrdFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        }
        prSPS->rVUI.fgPicStructPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        prSPS->rVUI.fgBitstreamRestrictionFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSPS->rVUI.fgBitstreamRestrictionFlag)
        {
            prSPS->rVUI.fgMotionVectorsOverPicBoundariesFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
            prSPS->rVUI.u4MaxBytesPerPicDenom = _VDEC_UeCodeNum(u4VDecID);
            prSPS->rVUI.u4MaxBitsPerMbDenom = _VDEC_UeCodeNum(u4VDecID);
            prSPS->rVUI.u4Log2MaxMvLengthHorizontal = _VDEC_UeCodeNum(u4VDecID);
            prSPS->rVUI.u4Log2MaxMvLengthVertical = _VDEC_UeCodeNum(u4VDecID);
            prSPS->rVUI.u4NumReorderFrames = _VDEC_UeCodeNum(u4VDecID);
            prSPS->rVUI.u4MaxDecFrameBuffering = _VDEC_UeCodeNum(u4VDecID);
        }
    }
    #endif

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if((prDecPrm->u1NalUnitType == SUB_SPS) &&
        ((prSPS->u4ProfileIdc == MVC_HIGH) || (prSPS->u4ProfileIdc == MVC_STEREO)))
    {
        u4Temp = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        CHKPARAM_R(u4Temp, 1, 1);
        
        prSPS->u4NumViews = _VDEC_UeCodeNum(u4VDecID) + 1;
        CHKPARAM(prSPS->u4NumViews, 1023);
        for(u4ViewIdx = 0; u4ViewIdx < prSPS->u4NumViews; u4ViewIdx++)
        {
            if(u4ViewIdx < MAX_VIEW_NUM)
            {
                prSPS->au4ViewID[u4ViewIdx] = _VDEC_UeCodeNum(u4VDecID);
                prDecPrm->au4ViewIDList[u4ViewIdx] = prSPS->au4ViewID[u4ViewIdx];
            }
            else
            {
                UNUSED(_VDEC_UeCodeNum(u4VDecID));
            }
        }
        //Anchor
        for(u4ViewIdx = 1; u4ViewIdx < prSPS->u4NumViews; u4ViewIdx++)
        {
            u4FbNum = _VDEC_UeCodeNum(u4VDecID);
            CHKPARAM(u4FbNum, 15);
            if(u4ViewIdx < MAX_VIEW_NUM)
            {
                prSPS->au1NumAnchorRefL0[u4ViewIdx] = u4FbNum;
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    prSPS->au4AnchorRefL0[u4ViewIdx][u4FbIdx] = _VDEC_UeCodeNum(u4VDecID);
                }
            }
            else
            {
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    UNUSED(_VDEC_UeCodeNum(u4VDecID));
                }
            }
            u4FbNum = _VDEC_UeCodeNum(u4VDecID);
            CHKPARAM(u4FbNum, 15);
            if(u4ViewIdx < MAX_VIEW_NUM)
            {
                prSPS->au1NumAnchorRefL1[u4ViewIdx] = u4FbNum;
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    prSPS->au4AnchorRefL1[u4ViewIdx][u4FbIdx] = _VDEC_UeCodeNum(u4VDecID);
                }
            }
            else
            {
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    UNUSED(_VDEC_UeCodeNum(u4VDecID));
                }
            }
        }
        //Non-Anchor
        for(u4ViewIdx = 1; u4ViewIdx < prSPS->u4NumViews; u4ViewIdx++)
        {
            u4FbNum = _VDEC_UeCodeNum(u4VDecID);
            CHKPARAM(u4FbNum, 15);
            if(u4ViewIdx < MAX_VIEW_NUM)
            {
                prSPS->au1NumNonAnchorRefL0[u4ViewIdx] = u4FbNum;
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    prSPS->au4NonAnchorRefL0[u4ViewIdx][u4FbIdx] = _VDEC_UeCodeNum(u4VDecID);
                }
            }
            else
            {
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    UNUSED(_VDEC_UeCodeNum(u4VDecID));
                }
            }
            u4FbNum = _VDEC_UeCodeNum(u4VDecID);
            CHKPARAM(u4FbNum, 15);
            if(u4ViewIdx < MAX_VIEW_NUM)
            {
                prSPS->au1NumNonAnchorRefL1[u4ViewIdx] = u4FbNum;
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    prSPS->au4NonAnchorRefL1[u4ViewIdx][u4FbIdx] = _VDEC_UeCodeNum(u4VDecID);
                }
            }
            else
            {
                for(u4FbIdx = 0; u4FbIdx < u4FbNum; u4FbIdx++)
                {
                    UNUSED(_VDEC_UeCodeNum(u4VDecID));
                }
            }
        }

    }
    #endif
    _VDEC_TrailingBits(u4VDecID);

    #ifdef FBM_DPB_CTRL
    for(i=0; i<MAX_VIEW_NUM; i++)
    {
        if (FBM_ChkFrameBufferFlag(prDecPrm->aucFbgId[i], FBM_FLAG_RELEASE_FBG) || prDecPrm->fgChgRRMode)        
        {
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, prDecPrm->ucEsId);    
            FBM_WaitUnlockFrameBuffer(prDecPrm->aucFbgId[i], WAIT_DISP_UNLOCK_TIME);    // block until display unlock
            _VDEC_FlushDPB(prDecPrm);
            FBM_ReleaseGroup(prDecPrm->aucFbgId[i]);
            prDecPrm->aucFbgId[i] = FBM_FBG_ID_UNKNOWN;
            prDecPrm->fgChgRRMode = FALSE;
        }
    }
    #endif
    prSPS->fgSPSValid = TRUE;
    if(prDecPrm->u1NalUnitType == SUB_SPS)
    {
        prVdecEsInfo->fgRsvSpsMvc = TRUE;
    }
    else
    {
        prVdecEsInfo->fgRsvSps = TRUE;;
    }
    if((prSPS->u4NumRefFrames != prVdec->rSPSBK.u4NumRefFrames) && prVdec->fgisvalid)
    {
        LOG(0,"ref nums change from %d to %d\n",prVdec->rSPSBK.u4NumRefFrames,prSPS->u4NumRefFrames);
#ifdef ENABLE_MULTIMEDIA 
        if (!(prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS))
#endif
        {
            prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = prSPS->u4NumRefFrames + 1;
        }
        prDecPrm->u4MaxFBufNum = prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
        prDecPrm->u4PicW = (prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
        prDecPrm->u4PicH = ((2 - prSPS->fgFrameMbsOnlyFlag) * (prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4;
        if(prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] > 17)
        {
            prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = 17;
            LOG(0,"!!!!!!reference frame num %d is too large,resolution %d %d force play may show garabge!!!!!!\n",prSPS->u4NumRefFrames,
                    prDecPrm->u4PicW,prDecPrm->u4PicH);
        }
        u4FbCnt = (UINT32)(FBM_GetFrameBufferMvNs(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx]));  // 2 lock frame
        if(u4FbCnt >=prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx])
        {
            _VDEC_FlushDPB(prDecPrm);
            FBM_GetFrameBufferAddr(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], 0, &u4YAddr, &u4CAddr);
            FBM_GetFrameBufferAddrMv(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], 0, &u4MVAddr);

            u4DramPicSize = ((((prDecPrm->u4PicW + 15) >> 4) * ((prDecPrm->u4PicH + 31) >> 5)) << 9);
            // 1 pic area = Y + CbCr + restored MV
            u4DramPicArea = ((((u4DramPicSize * 7) >> 2) + 511) >> 9) << 9;
            ASSERT(prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] <= 18);
            for (j = prDecPrm->u1CurViewIdx; j < MAX_VIEW_NUM; j++)
            {
                prDecPrm->ucEmpMVBufMaxNum[j] = (UCHAR)prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
                for (i = 0; i < prDecPrm->ucEmpMVBufMaxNum[j]; i++)
                {
                    if (i >= MAX_FRAME_NUM)
                    {
                        LOG(3, "MVNum out of range\n");
                        break;
                    }
                    else
                    {
                        prDecPrm->aucEmpMVBufIdxMap[j][i] = (UCHAR)i;
                    }
                }
                for (i = 0; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
                {
                    prDecPrm->arFBufInfo[j][i].u4W = prDecPrm->u4PicW;
                    prDecPrm->arFBufInfo[j][i].u4H = prDecPrm->u4PicH;
                    prDecPrm->arFBufInfo[j][i].u4DramPicSize = u4DramPicSize;
                    prDecPrm->arFBufInfo[j][i].u4DramPicArea = u4DramPicArea;
                #ifdef FBM_DPB_CTRL
                    prDecPrm->arFBufInfo[j][i].u4YAddr = u4YAddr;
                    prDecPrm->arFBufInfo[j][i].u4CAddr = u4CAddr;
                    prDecPrm->arFBufInfo[j][i].u4MVBufAddr = u4MVAddr;
                #else
                    prDecPrm->arFBufInfo[j][i].u4YAddr = prDecPrm->u4DPBAddr + (i * u4DramPicArea);
                    prDecPrm->arFBufInfo[j][i].u4CAddr = prDecPrm->arFBufInfo[j][i].u4YAddr + u4DramPicSize;
                    prDecPrm->arFBufInfo[j][i].u4MVBufAddr = prDecPrm->arFBufInfo[j][i].u4CAddr + (u4DramPicSize >> 1);
                #endif
                }
            }
        }
        else
        {
            LOG(0,"!!!!!!reference frame num %d is too large, we only have !!!!!!%d, resolution %d %d\n",prSPS->u4NumRefFrames,u4FbCnt,
            prDecPrm->u4PicW,prDecPrm->u4PicH);
        }
    }
    *pi4RetValue = 0;
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse SPS End\n");
#endif
}


// *********************************************************************
// Function    : void vPic_Par_Set_Rbsp(void)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_PPSrbsp(UINT32 u4VDecID, VDEC_H264_INFO_T* prVdec, INT32* pi4RetValue)
{
    UINT32 i;
    UINT32 u4PPSID;
    VDEC_AVC_SPS_T* prSPS;
    VDEC_AVC_PPS_T* prPPS;
    #ifdef FBM_DPB_CTRL
    VDEC_AVC_DecPrm* prDecPrm;
    #endif
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    ASSERT(prVdec!=NULL);
    ASSERT(pi4RetValue!=NULL);
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse PPS Start\n");
#endif
    #ifdef FBM_DPB_CTRL
    prDecPrm = &prVdec->rDecParam;
    #endif
    *pi4RetValue = 1;
    if(prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(3, "PPS ViewIdx Err\n");
        *pi4RetValue = 1;
        return;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
	
    ASSERT(prVdecEsInfo!=NULL);
    if(prVdecEsInfo == NULL)
    {
        return;
    }

    u4PPSID = _VDEC_UeCodeNum(u4VDecID);
    if(u4PPSID < 256)
    {
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        if((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) &&
            (prVdec->rPesInfo.ucEsId > 0))
        {
            prPPS = &prVdec->arSUBPPS[u4PPSID];
        }
        else
        #endif
        {
            prPPS = &prVdec->arPPS[u4PPSID];
            if(prPPS->fgPPSValid == TRUE)
            {
                prVdec->u1PPSIDBK = u4PPSID;
                prVdec->fgisvalid = TRUE;
                x_memcpy((void*)&prVdec->rPPSBK, (void*)prPPS, sizeof(VDEC_AVC_PPS_T));
            }
        }
        x_memset((void*)prPPS, 0, sizeof(VDEC_AVC_PPS_T));
        prPPS->fgPPSValid = FALSE; // FALSE until set completely
    }
    else
    {
        *pi4RetValue = 1;
        LOG(2, "err in PPS Num %d, InWin 0x%x\n", u4PPSID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;
        return;
    }

    prPPS->u4SeqParameterSetId = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prPPS->u4SeqParameterSetId, 31);
    prSPS = &prVdec->arSPS[prPPS->u4SeqParameterSetId];
    if(!prSPS->fgSPSValid)
    {    
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        prSPS = &prVdec->arSUBSPS[prPPS->u4SeqParameterSetId];
        if(!prSPS->fgSPSValid)
        {
        #endif
        
        *pi4RetValue = 1;
        LOG(2, "SPS Num in PPS err\n");
        return;
            
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        }
        #endif
    }

    prPPS->fgEntropyCodingModeFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prPPS->fgPicOrderPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);

    prPPS->u4NumSliceGroupsMinus1 = _VDEC_UeCodeNum(u4VDecID);

    if(prPPS->u4NumSliceGroupsMinus1 >= MAXnum_slice_groups_minus1)
    {
        LOG(2, "Warning!!! num_slice_groups_minus1 size isn't enough = %d\n",prPPS->u4NumSliceGroupsMinus1);
        *pi4RetValue = 1;
        return;
    }

    if(prPPS->u4NumSliceGroupsMinus1 > 0)
    {
        prPPS->u4SliceGroupMapType = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prPPS->u4SliceGroupMapType, 6);

        if(prPPS->u4SliceGroupMapType == 0)
        {
            for(i = 0; i <= prPPS->u4NumSliceGroupsMinus1; i++)
            {
                prPPS->u4RunLengthMinus1[i] = _VDEC_UeCodeNum(u4VDecID);
            }
        }// add NOT_SUPPORT filter
        else if(prPPS->u4SliceGroupMapType == 1)//SliceGroup==1 not support
        {
            if((prVdecEsInfoKeep)&&(prVdecEsInfoKeep->fgVPush)&&(prVdec->rDecParam.fgIsMMPlay))
            {
                prVdec->rDecParam.fgNotSupport = TRUE;
                LOG(2, "H264 SliceGroupMap type(%u) not support.\n", prPPS->u4SliceGroupMapType);
                *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
                return;
            }
        }
        else if(prPPS->u4SliceGroupMapType == 2)
        {
            for(i = 0; i < prPPS->u4NumSliceGroupsMinus1; i++)
            {
                prPPS->u4TopLeft[i] = _VDEC_UeCodeNum(u4VDecID);
                prPPS->u4BottomRight[i] = _VDEC_UeCodeNum(u4VDecID);
            }
        }
        else if((prPPS->u4SliceGroupMapType == 3) ||
            (prPPS->u4SliceGroupMapType == 4) ||
            (prPPS->u4SliceGroupMapType == 5))
        {
            prPPS->fgSliceGroupChangeDirectionFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
            prPPS->u4SliceGroupChangeRateMinus1 = _VDEC_UeCodeNum(u4VDecID);
        }
        else if(prPPS->u4SliceGroupMapType == 6)
        {
            prPPS->u4PicSizeInMapUnitsMinus1 = _VDEC_UeCodeNum(u4VDecID);
            if(prPPS->u4PicSizeInMapUnitsMinus1 > 7 )
            {
                LOG(2, "Warning!!! u4PicSizeInMapUnitsMinus1 size isn't too big = %d\n",prPPS->u4PicSizeInMapUnitsMinus1);
                *pi4RetValue = 1;
                return;
            }
            for(i = 0; i <= prPPS->u4PicSizeInMapUnitsMinus1; i++)
            {
                prPPS->au4SliceGroupId[i] = _VDEC_UeCodeNum(u4VDecID);
            }
        }
    }

    prPPS->u4NumRefIdxL0ActiveMinus1 = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prPPS->u4NumRefIdxL0ActiveMinus1, 31);
    prPPS->u4NumRefIdxL1ActiveMinus1 = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prPPS->u4NumRefIdxL1ActiveMinus1, 31);
    
    prPPS->fgWeightedPredFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prPPS->u4WeightedBipredIdc = _VDEC_AVCGetRealShtBitStream(u4VDecID, 2);
    CHKPARAM(prPPS->u4WeightedBipredIdc, 2);
    prPPS->i4PicInitQpMinus26 = _VDEC_SeCodeNum(u4VDecID);
    //CHKPARAM_R(prPPS->i4PicInitQpMinus26, -(26 + (6*(INT32)prSPS->u4BitDepthLumaMinus8)), 25);
    prPPS->i4PicInitQsMinus26 = _VDEC_SeCodeNum(u4VDecID);
    CHKPARAM_R(prPPS->i4PicInitQsMinus26, -26, 25);

    prPPS->i4ChromaQpIndexOffset = _VDEC_SeCodeNum(u4VDecID);
    CHKPARAM_R(prPPS->i4ChromaQpIndexOffset, -12, 12);
    prPPS->fgDeblockingFilterControlPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prPPS->fgConstrainedIntraPredFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    prPPS->fgRedundantPicCntPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    // add NOT_SUPPORT filter
    if(prPPS->fgRedundantPicCntPresentFlag)//Redundant Picture Not support.
    {
        if((prVdecEsInfoKeep)&&(prVdecEsInfoKeep->fgVPush)&&(prVdec->rDecParam.fgIsMMPlay))
        {
            prVdec->rDecParam.fgNotSupport = TRUE;
            LOG(2, "H264 fgRedundantPicCntPresentFlag not support.\n");
            *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
            return;
        }
    }
    if(_fgIsMoreRbspData(u4VDecID, prVdec->rPesInfo.u4VldReadPtrEnd))
    {
        prPPS->fgTransform8x8ModeFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        prPPS->fgPicScalingMatrixPresentFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prPPS->fgPicScalingMatrixPresentFlag)
        {
            for(i=0; i<(((UINT32)prPPS->fgTransform8x8ModeFlag << 1) + 6); i++)
            {
                prPPS->fgPicScalingListPresentFlag[i] = _VDEC_AVCGetBitStreamFlg(u4VDecID);
                if(prPPS->fgPicScalingListPresentFlag[i])
                {
                    if(i < 6)
                    {
                        _vScalingList(u4VDecID, prPPS->ppcScalingList4x4[i], 16, &prPPS->fgUseDefaultScalingMatrix4x4Flag[i]);
                    }
                    else
                    {
                        _vScalingList(u4VDecID, prPPS->ppcScalingList8x8[i-6], 64, &prPPS->fgUseDefaultScalingMatrix8x8Flag[i-6]);
                    }
                }
            }
        }
        prPPS->i4SecondChromaQpIndexOffset = _VDEC_SeCodeNum(u4VDecID);
        CHKPARAM_R(prPPS->i4SecondChromaQpIndexOffset, -12, 12);
    }
    else
    {
        prPPS->fgTransform8x8ModeFlag = 0;
        prPPS->fgPicScalingMatrixPresentFlag = FALSE;
        prPPS->i4SecondChromaQpIndexOffset = prPPS->i4ChromaQpIndexOffset;
    }
  
    _VDEC_TrailingBits(u4VDecID);
    prPPS->fgPPSValid = TRUE;
    prDecPrm->fgWaitPPS = FALSE;
    prDecPrm->u1WaitPPSSkipFrmCnt = 0;
    prVdecEsInfo->fgSeqHdrH264 = TRUE;
    *pi4RetValue = 0;
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse PPS End\n");
#endif
}


/** Get decoding handler
*@param .
 */
void _VDEC_SliceNonIDR(UINT32 u4VDecID, VDEC_H264_INFO_T* prVdec, INT32* pi4RetValue, UCHAR ucEsId)
{
    VDEC_AVC_SLICE_HDR_T* prSliceHdr;// = &_tSliceHdr;
    VDEC_AVC_DecPrm* prDecPrm;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_PICTYPE_CB_T rPicType;
    VDEC_INFO_T *prVdecInfo;
    UINT32 u4PPSID;
    BOOL fgVdecSupport;
    #ifdef VDEC_TIME_PROFILE
    HAL_TIME_T rTimeHwS, rTimeHwE, rTimeHwDt;
    HAL_TIME_T rTimeSwS, rTimeSwDt,rTimeSwE;
    static UINT32 u4SWTimeMicro = 0;
    #endif
    BOOL bIsReqFb = FALSE;
    VDEC_AVC_SPS_T* prSPS;
    VDEC_AVC_PPS_T* prPPS;
    
    ASSERT(prVdec!=NULL);
    ASSERT(pi4RetValue!=NULL);
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse IDR/Non-IDR Start\n");
#endif
    
    prDecPrm = &prVdec->rDecParam;
    prSliceHdr = &prVdec->rSliceHdr;  
    x_memset((void*)prSliceHdr, 0, sizeof(VDEC_AVC_SLICE_HDR_T));
    prDecPrm->prSliceHdr = prSliceHdr;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecInfo = _VDEC_GetInfo();

    if(prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(3, "Slice ViewIdx Err\n");
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        return;
    }

    ASSERT(prVdecEsInfoKeep != NULL);

    if(prDecPrm->fgNotSupport)  // check if !support but still get data
    {
        *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
        return;
    }    
    prDecPrm->rExtra = prVdec->rPesInfo.rExtra;
    prDecPrm->fgEos = prVdec->rPesInfo.fgEos;
    prDecPrm->u8Offset = prVdec->rPesInfo.u8Offset;
    prDecPrm->u8OffsetI = prVdec->rPesInfo.u8OffsetI;    
    prDecPrm->u4Timestap = prVdec->rPesInfo.u4Timestap;
    prDecPrm->u2DecodingOrder = prVdec->rPesInfo.u2DecodingOrder;
    prDecPrm->u4TickNum = prVdec->rPesInfo.u4TickNum;    

    prSliceHdr->u4FirstMbInSlice = _VDEC_UeCodeNum(u4VDecID);
    prSliceHdr->u4SliceType = _VDEC_UeCodeNum(u4VDecID);
    CHKPARAM(prSliceHdr->u4SliceType, 9);
    //add NOT_SUPPORT filter
    if (REAL_DECODE == prDecPrm->ucVDecStatus)
    {
        if((prSliceHdr->u4SliceType == SI_Slice_ALL) || (prSliceHdr->u4SliceType == SP_Slice_ALL))//SI SP all not support
        {
            if((prVdecEsInfoKeep->fgVPush)&&(prVdec->rDecParam.fgIsMMPlay))
            {
                prVdec->rDecParam.fgNotSupport = TRUE;
                LOG(2, "H264 Slice type(%d) not support.\n", prSliceHdr->u4SliceType);
                *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
                return;
            }
        }
    }
    switch(prSliceHdr->u4SliceType)
    {
        case 0:
        case 5:
           prDecPrm->u4PicNalType |= (NAL_P);
           break;
        case 1:
        case 6:
           prDecPrm->u4PicNalType |= (NAL_B);
           break;
        case 2:
        case 7:
           if(fgIsIDRPic(prDecPrm->u1NalUnitType))
           {
                prDecPrm->u4PicNalType |= (NAL_IDR);
           }
           else
           {
                prDecPrm->u4PicNalType |= (NAL_I);
           }
           break;
        case 3:
        case 8:
            prDecPrm->u4PicNalType |= (NAL_SP);
            break;
        case 4:
        case 9:
            prDecPrm->u4PicNalType |= (NAL_SI);
            break;
        default:
            prDecPrm->u4PicNalType |= (NAL_SLICE_ERROR);
            break;
    }
    //LOG(7,"PicNalType 0x%x\n",prDecPrm->u4PicNalType);
    
    u4PPSID = _VDEC_UeCodeNum(u4VDecID);
    if(u4PPSID < 256)
    {
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        if(prDecPrm->u1NalUnitType == CODED_SLICE_EXT)
        {
            prSPS = &prVdec->arSUBSPS[prVdec->arPPS[u4PPSID].u4SeqParameterSetId];
        }
        else
        if((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) &&
            (prVdec->rPesInfo.ucEsId > 0))
        {
            prSPS = &prVdec->arSUBSPS[prVdec->arSUBPPS[u4PPSID].u4SeqParameterSetId];
        }
        else
        #endif
        {
            prSPS = &prVdec->arSPS[prVdec->arPPS[u4PPSID].u4SeqParameterSetId];
        } 
    }
    else
    {
         *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(6, "err in Slice Hdr PPS Num %d err, inWin 0x%x\n", 
            u4PPSID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
        prVdecEsInfo->rWFDDecodeErr.i4OutOfSpecCnt++;
        return;
    }    
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) &&
        (prVdec->rPesInfo.ucEsId > 0))
    {
        prPPS = &prVdec->arSUBPPS[u4PPSID];
    }
    else
    #endif
    {
        prPPS = &prVdec->arPPS[u4PPSID];
    }
    if((u4PPSID < 256)
        && (prPPS->fgPPSValid)
        && (prSPS->fgSPSValid))
    {
        prDecPrm->prPPS = prPPS;
        prDecPrm->prSPS = prSPS;
    }
    else
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(6, "err in Slice Hdr PPS Num %d err, inWin 0x%x\n", 
            u4PPSID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
        prVdecEsInfo->rWFDDecodeErr.i4LackSeqHeaderCnt++;
        return;
    }
    if(prDecPrm->fgWaitPPS)
    {
        if(prVdec->rDecParam.u1WaitPPSSkipFrmCnt)
        {
            prVdec->rDecParam.u1WaitPPSSkipFrmCnt--;
            *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
            return;
        }
        else
        {
            prDecPrm->fgWaitPPS = FALSE;
        }
    }
    //prDecPrm->fgFirstFrmTag = FALSE;
    
    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeSwS);
    #endif
    
    if (REAL_DECODE == prDecPrm->ucVDecStatus)//(VDEC_LAE0 != u4VDecID)
    {
        //create FBG if neccessary
        if(!_vCheckAllocFBG(prDecPrm))
        {
            //error occur
            *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
            STC_SetStcValue(prVdecEsInfo->ucStcId, (UINT32)0);
            return;
        }
    }
    _vInitSliceHdr(prDecPrm->prSliceHdr, prDecPrm->prPPS->u4NumRefIdxL0ActiveMinus1, 
        prDecPrm->prPPS->u4NumRefIdxL1ActiveMinus1);
    prSliceHdr->u4FrameNum = _VDEC_AVCGetRealShtBitStream(u4VDecID, prDecPrm->prSPS->u4Log2MaxFrameNumMinus4 + 4);

    #ifdef AVC_MV_BUF_REDUCE
    #ifdef IS_POST_MT5368
    if(prDecPrm->prSPS->fgDirect8x8InferenceFlag)
    #else
    if(prDecPrm->prSPS->fgFrameMbsOnlyFlag && (!prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag))
    #endif
    {
        _VDEC_IsMVReduce(TRUE);
    }
    else
    {
        _VDEC_IsMVReduce(FALSE);
    }
    #endif
    prSliceHdr->fgFieldPicFlag = FALSE;
    prSliceHdr->fgBottomFieldFlag = FALSE;
    if(prDecPrm->prSPS->fgFrameMbsOnlyFlag)
    {
        prDecPrm->u1PicStruct = FRAME;
    }
    else
    {
        prSliceHdr->fgFieldPicFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSliceHdr->fgFieldPicFlag)
        {
            prSliceHdr->fgBottomFieldFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
            prDecPrm->u1PicStruct = (UINT8)((prSliceHdr->fgBottomFieldFlag) ? BOTTOM_FIELD : TOP_FIELD);            
        }
        else
        {
            prDecPrm->u1PicStruct = FRAME;           
        }
    }
    
    if (REAL_DECODE == prDecPrm->ucVDecStatus)//(VDEC_LAE0 != u4VDecID)
    {
        if(prVdecEsInfo->rMMSeqInfo.b_chk_frmrate)
        {
            fgVdecSupport = _vCheckRes(prDecPrm, prSliceHdr->fgFieldPicFlag,prVdecEsInfo);
            if(fgVdecSupport)
            {
                *pi4RetValue = (INT32)E_VDEC_FRAME_RATE_NOT_SUPPORT;
                return;
            }
        }
        //error handling for MM I mode
        if((prDecPrm->fgIsMMPlay && 
        (prDecPrm->ucSkipMode == IMode)
        #ifdef ENABLE_MULTIMEDIA
        && ((prVdecEsInfo->eContainerType< SWDMX_FMT_MPEG1_DAT)|| (prVdecEsInfo->eContainerType> SWDMX_FMT_VC1_WMV)
        ||(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS && prVdecEsInfoKeep->fgVPush) )
        #endif
        )||(prDecPrm->u1PlayModeInDtv == IMode)
        #ifdef ENABLE_MULTIMEDIA
        ||(((prDecPrm->ucSkipMode == IPMode)||(prDecPrm->ucSkipMode == IMode))&&(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV))
        #endif
        )
        {
            if(!fgIsISlice(prSliceHdr->u4SliceType))
            {
                if(prDecPrm->prCurrFBufInfo &&( 
                    (prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME)||(prDecPrm->prCurrFBufInfo->u1FBufStatus == 0)))
                {
                    if(prDecPrm->u1PicStruct == FRAME)
                    {
                        LOG(3, "SliceType %d Frm in IMode\n", prSliceHdr->u4SliceType);
                    }
                    else
                    {
                        LOG(3, "SliceType %d Fld in IMode\n", prSliceHdr->u4SliceType);
                    }                    
                    *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
                    return;
                }
            }
        }
        if(prDecPrm->ucPreAllocFbId == FBM_FB_ID_UNKNOWN)
        {
            if(_vCheckAllocFB(prDecPrm, &bIsReqFb))
            {
                if(bIsReqFb)
                {
                    *pi4RetValue = (INT32)E_VDEC_ALLOCATEFB;
                    return;
                }
            }
            else
            {
                //error occur
                *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
                return;
            }
        }
        
        if(prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
        {
            LOG(3, "Slice ViewIdx Err\n");
            *pi4RetValue = (INT32)E_VDEC_FAIL;
            return;
        }

        //reset for multi-frame in one PES, only the first frame in PES has non-zero PTS value
        //change the handling to dmx, reset pts in queue after each picture deliver
        if(prVdec->rDecParam.fgIsMMPlay && prDecPrm->prCurrFBufInfo)
        {
            prDecPrm->prCurrFBufInfo->u4SkipFrmCnt += prVdec->rPesInfo.u2SkipVidCount;
            prVdec->rPesInfo.u2SkipVidCount = 0;
        }
        
        //Enter wait RAP mechanism after frame allocation flow
        //to reduce duplicate counting       
        _VDEC_WaitRAPToDisp(ucEsId, prDecPrm);   
    }
    if(fgIsIDRPic(prVdec->u1NalUnitType) |(!prDecPrm->fgNalNonIdr)) //IDR picture
    {
        prSliceHdr->u4IdrPicId = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSliceHdr->u4IdrPicId, 65535);
    }
    if(prDecPrm->prSPS->u4PicOrderCntType == 0)
    {
        prSliceHdr->i4PicOrderCntLsb = (INT32)_VDEC_AVCGetRealShtBitStream(u4VDecID, prDecPrm->prSPS->u4Log2MaxPicOrderCntLsbMinus4 + 4);
        if(prDecPrm->prPPS->fgPicOrderPresentFlag && (!prSliceHdr->fgFieldPicFlag))
        {
            prSliceHdr->i4DeltaPicOrderCntBottom = _VDEC_SeCodeNum(u4VDecID);
        }
        else
        {
            prSliceHdr->i4DeltaPicOrderCntBottom = 0;
        }
    }
  
    if((prDecPrm->prSPS->u4PicOrderCntType == 1) && (!prDecPrm->prSPS->fgDeltaPicOrderAlwaysZeroFlag))
    {
        prSliceHdr->ai4DeltaPicOrderCnt[0] = _VDEC_SeCodeNum(u4VDecID);
        if(prDecPrm->prPPS->fgPicOrderPresentFlag && (!prSliceHdr->fgFieldPicFlag))
        {
            prSliceHdr->ai4DeltaPicOrderCnt[1] = _VDEC_SeCodeNum(u4VDecID);
        }
    }
    else
    {
        if(prDecPrm->prSPS->u4PicOrderCntType == 1)
        {
            prSliceHdr->ai4DeltaPicOrderCnt[0] = 0;
            prSliceHdr->ai4DeltaPicOrderCnt[1] = 0;
        }
    }
  
    if(prDecPrm->prPPS->fgRedundantPicCntPresentFlag)
    {
        prSliceHdr->u4RedundantPicCnt = _VDEC_UeCodeNum(u4VDecID);
    }
    if(fgIsBSlice(prSliceHdr->u4SliceType))
    {
        prSliceHdr->fgDirectSpatialMvPredFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    }
    if(fgIsPSlice(prSliceHdr->u4SliceType) || fgIsBSlice(prSliceHdr->u4SliceType))
    {
        prSliceHdr->fgNumRefIdxActiveOverrideFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        if(prSliceHdr->fgNumRefIdxActiveOverrideFlag)
        {
            prSliceHdr->u4NumRefIdxL0ActiveMinus1 = _VDEC_UeCodeNum(u4VDecID);
            if(prSliceHdr->fgFieldPicFlag)
            {
                CHKPARAM(prSliceHdr->u4NumRefIdxL0ActiveMinus1, 31);
            }
            else
            {
                CHKPARAM(prSliceHdr->u4NumRefIdxL0ActiveMinus1, 15);
            }
            if(fgIsBSlice(prSliceHdr->u4SliceType))
            {
                prSliceHdr->u4NumRefIdxL1ActiveMinus1 = _VDEC_UeCodeNum(u4VDecID);
                if(prSliceHdr->fgFieldPicFlag)
                {
                    CHKPARAM(prSliceHdr->u4NumRefIdxL1ActiveMinus1, 31);
                }
                else
                {
                    CHKPARAM(prSliceHdr->u4NumRefIdxL1ActiveMinus1, 15);
                }                
            }
        }
    }
    if(!fgIsBSlice(prSliceHdr->u4SliceType))
    {
        prSliceHdr->u4NumRefIdxL1ActiveMinus1 = 0;
    }
    if (REAL_DECODE == prDecPrm->ucVDecStatus)
    {
        if (prVdecInfo->pfPicTypeCb)
        {
            x_memset(&rPicType, 0, sizeof(VDEC_PICTYPE_CB_T));
            rPicType.u1EsId = ucEsId;
            rPicType.u4Pts = prVdec->rPesInfo.u4PTS;
            if(fgIsBSlice(prSliceHdr->u4SliceType))
            {
                rPicType.u1PicType = VDEC_PIC_TYPE_B;
                LOG(7, "CB, PicType = VDEC_PIC_TYPE_B\n");
            }
            else if(fgIsPSlice(prSliceHdr->u4SliceType))
            {
                rPicType.u1PicType = VDEC_PIC_TYPE_P;
                LOG(7, "CB, PicType = VDEC_PIC_TYPE_P\n");
            }
            else
            {
                rPicType.u1PicType = VDEC_PIC_TYPE_I;
                LOG(7, "CB, PicType = VDEC_PIC_TYPE_I\n");
            }
            prVdecInfo->pfPicTypeCb(&rPicType);
        }

    }
    
    #if defined(CC_MT5395)
    if(!prVdec->rDecParam.fgIsMVCClkSet)
    {
        UINT32 u4Clk;
        u4Clk = IO_READ32(CKGEN_BASE, 0x224);
        prVdec->rDecParam.u1PrevVLDClk =  u4Clk & 0xF;
        IO_WRITE32(CKGEN_BASE, 0x224, (u4Clk & 0xFFFFFFF0) | 0x9);   //mt5395 MVC 288MHz, sawless_pll/1.5

        u4Clk = IO_READ32(CKGEN_BASE, 0x228);
        prVdec->rDecParam.u1PrevMCClk =  u4Clk & 0xF;
        IO_WRITE32(CKGEN_BASE, 0x228, (u4Clk & 0xFFFFFFF0) | 0x9);   //mt5395 MVC 288MHz, sawless_pll/1.5

        prVdec->rDecParam.fgIsMVCClkSet = TRUE;
    }
    #endif
//end MVC setting            
    
    _VDEC_SetPicInfo(ucEsId, u4VDecID, prDecPrm);
    if (DUAL_CORE_MODE == prDecPrm->ucDualCoreMode)
    {
        _VDEC_SetPicInfo(ucEsId, VDEC_VLD1, prDecPrm);            
    }
    if(prDecPrm->fgAllocFbFail)
    {
        *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
        prDecPrm->fgAllocFbFail = FALSE;
        #ifdef VDEC_TIME_PROFILE
        u4SWTimeMicro = 0;
        #endif
        return;
    }
    else if(prDecPrm->fgNotSupport)
    {
        *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
        return;
    }

    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeHwS);
    #endif
    
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if(prDecPrm->fgIsMVC)
    {
        _VDEC_MVCRegSetting(u4VDecID, prDecPrm);
        #if !defined(CC_ALLEGRO_MVC) && defined(IS_POST_MT5368)
        if(!prDecPrm->fgNalNonIdr)
        {
            _VDEC_ECBufSetting(prDecPrm,
            prDecPrm->arRefPicList[0].u4RefPicCnt, 
            prSliceHdr->fgFieldPicFlag ? ((prDecPrm->prSPS->u4NumRefFrames << 1) + 1) : (prDecPrm->prSPS->u4NumRefFrames + 1),
            u4VDecID);
        }
        #endif
    }
    #endif

    if(!fgIsISlice(prSliceHdr->u4SliceType))
    {
        if(!_VDEC_RefPicListReordering(u4VDecID))//hw function
        {
            //CHKPARAM(1, 0); //header error flow
        }      
    }
      
    if((prDecPrm->prPPS->fgWeightedPredFlag && fgIsPSlice(prSliceHdr->u4SliceType))
        || ((prDecPrm->prPPS->u4WeightedBipredIdc == 1) && fgIsBSlice(prSliceHdr->u4SliceType)))
    {
        if(!_VDEC_PredWeightTable(u4VDecID))    //hw function
        {
            CHKPARAM(1, 0); //header error flow
        }
    }
    
    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeHwE);
    HAL_GetDeltaTime(&rTimeHwDt, &rTimeHwS, &rTimeHwE);
    #endif

    if(prDecPrm->u1NalRefIdc!= 0)
    {
        prSliceHdr->fgMmco5 = FALSE;
        _vRefPicMarking(u4VDecID, fgIsIDRPic(prDecPrm->u1NalUnitType) |(!prDecPrm->fgNalNonIdr), prSliceHdr);    
        //save marking info into data structure
    }

    if(prDecPrm->prPPS->fgEntropyCodingModeFlag && (!fgIsISlice(prSliceHdr->u4SliceType)))
    {
        prSliceHdr->u4CabacInitIdc = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSliceHdr->u4CabacInitIdc, 2);
    }
    else
    {
        prSliceHdr->u4CabacInitIdc = 0;
    }

    prSliceHdr->i4SliceQpDelta = _VDEC_SeCodeNum(u4VDecID);
    CHKPARAM_R(prSliceHdr->i4SliceQpDelta,
    -(((6*(INT32)prDecPrm->prSPS->u4BitDepthLumaMinus8) + 26) + prDecPrm->prPPS->i4PicInitQpMinus26), 
    (51 - (26 + prDecPrm->prPPS->i4PicInitQpMinus26)));

    if((prSliceHdr->u4SliceType == SI_Slice) || (prSliceHdr->u4SliceType == SP_Slice))
    {
        if(prSliceHdr->u4SliceType == SP_Slice)
        {
            prSliceHdr->fgSpForSwitchFlag = _VDEC_AVCGetBitStreamFlg(u4VDecID);
        }
        prSliceHdr->i4SliceQsDelta = _VDEC_SeCodeNum(u4VDecID);
    }

    if(prDecPrm->prPPS->fgDeblockingFilterControlPresentFlag)
    {
        prSliceHdr->u4DisableDeblockingFilterIdc = _VDEC_UeCodeNum(u4VDecID);
        CHKPARAM(prSliceHdr->u4DisableDeblockingFilterIdc, 2);
        if(prSliceHdr->u4DisableDeblockingFilterIdc != 1)
        {
            prSliceHdr->i4SliceAlphaC0OffsetDiv2 = _VDEC_SeCodeNum(u4VDecID);
            CHKPARAM_R(prSliceHdr->i4SliceAlphaC0OffsetDiv2, -6, 6);
            prSliceHdr->i4SliceBetaOffsetDiv2 = _VDEC_SeCodeNum(u4VDecID);
            CHKPARAM_R(prSliceHdr->i4SliceBetaOffsetDiv2, -6, 6);
        }
        else
        {
            prSliceHdr->i4SliceAlphaC0OffsetDiv2 = 0;
            prSliceHdr->i4SliceBetaOffsetDiv2 = 0;
        }
    }

    if((prDecPrm->prPPS->u4NumSliceGroupsMinus1 > 0) && 
        (prDecPrm->prPPS->u4SliceGroupMapType >= 3) &&
        (prDecPrm->prPPS->u4SliceGroupMapType <= 5))
    {
        prSliceHdr->u4SliceGroupChangeCycle = _VDEC_AVCGetBitStreamFlg(u4VDecID);
    }
    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeSwE);
    HAL_GetDeltaTime(&rTimeSwDt, &rTimeSwS, &rTimeSwE);
    u4SWTimeMicro += rTimeSwDt.u4Micros;
    if(prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME)
    {
        prDecPrm->u4CurSwMicro = u4SWTimeMicro - prDecPrm->u4CurWaitFbMicro;
        u4SWTimeMicro = 0;
    }
    #endif

    if(prDecPrm->fgIsResChg)
    {
        *pi4RetValue = (INT32)E_VDEC_OK_RES_CHG;
    }
    else
    {
        *pi4RetValue = (INT32)E_VDEC_OK;
    }
    
#ifdef CC_DUAL_CORE_DEC
    if (LAE_DECODE == prDecPrm->ucVDecStatus)
    {
        prDecPrm->u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
        prDecPrm->u4PicH = ((2 - prDecPrm->prSPS->fgFrameMbsOnlyFlag) * (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
        do
        {
            ucGetBufferForLaeDec(ucEsId,&(prDecPrm->rLAEDecParam));
            if ((0 == prDecPrm->rLAEDecParam.u4CabacAddr) || (0 == prDecPrm->rLAEDecParam.u4ECAddr))
            {
                *pi4RetValue = (INT32)E_VDEC_FAIL;
                LOG(0, "~~~~~~~~~~~LAE Fail~~~~~~~~~~~~~~\n");
                break;
            }
            vVDEC_HAL_SetLAEDecAddr(u4VDecID, prDecPrm->rLAEDecParam.u4CabacAddr, prDecPrm->rLAEDecParam.u4ECAddr);
        }while((prDecPrm->rLAEDecParam.u4PicH != prDecPrm->u4PicH)
                || (prDecPrm->rLAEDecParam.u4PicW != prDecPrm->u4PicW));
    }
    if (DUAL_CORE_MODE == prDecPrm->ucDualCoreMode)
    {
        ucGetBufferForMcoreDec(ucEsId, &(prDecPrm->rLAEDecParam));         
        UNUSED(fgAVCMCoreMatchLAE(ucEsId, prVdec));
        vVDecSetMCoreLaeBuf(u4VDecID,PHYSICAL(prDecPrm->rLAEDecParam.u4CabacAddr),PHYSICAL(prDecPrm->rLAEDecParam.u4ECAddr));
    }
#endif
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]Parse IDR/Non-IDR End\n");
#endif
}


/** Get SEI information
*@param .
 */
void _VDEC_SEIrbsp(UINT32 u4VDecID, VDEC_H264_INFO_T* prVdec, INT32* pi4RetValue)
{
    VDEC_AVC_DecPrm* prDecPrm;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    UINT32 u4PayloadType = 0, u4PayloadSize, u4PayloadSizeOrg, u4Byte, u4DataStart = 0, u4DataLen = 0;
    UINT32 u4AFDId, u4Temp;
    UINT32 i = 0;
    UINT32 u4TempPayLoadSize = 0;
    #ifndef DIVX_PLUS_CER
    CHAR s_divx_check_buf[16] = {0};
    #endif

    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    VDEC_3D_TYPE_T e3DType;
    UINT32 u4Cnt = 0, u4SEICnt = 0, u4SEINaluSz = 0;
    
    #ifdef CC_SEI_USER_DATA_ENABLE
    #if defined(CC_DIRECTTV_USER_DATA) || defined(CC_VERIZON_CAPTION_USER_DATA)
    UINT8* pu1SEIPtr;   
    #endif
    #endif
    
    ASSERT(prVdec!=NULL);
    ASSERT(pi4RetValue!=NULL);
    
    prDecPrm = &prVdec->rDecParam;
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prVdec->rDecParam.ucEsId);

    //skip SEI data if in the half of field-encoded bitstream, save parsing time
    if(prDecPrm->prCurrFBufInfo)
    {
        if((prDecPrm->prCurrFBufInfo->u1FBufStatus == TOP_FIELD) ||
            (prDecPrm->prCurrFBufInfo->u1FBufStatus == BOTTOM_FIELD))
        {
            *pi4RetValue = (INT32)E_VDEC_OK;
            return;
        }
    }
    //prDecPrm->rSEI.u1AFD = SEI_AFD_UNKONWN;
    u4SEINaluSz = (prVdec->rPesInfo.u4VldReadPtrEnd > prVdec->rPesInfo.u4VldReadPtr)?
        (prVdec->rPesInfo.u4VldReadPtrEnd - prVdec->rPesInfo.u4VldReadPtr):
        (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4VldReadPtr + 
         (prVdec->rPesInfo.u4VldReadPtrEnd - prVdec->rPesInfo.u4FifoStart));
         
    do
    {
        u4PayloadType = 0;
        u4Cnt = 0;
        u4DataStart = 0;
        while ((u4Byte = (_VDEC_AVCGetShtBitStream(u4VDecID, 8) >> 24)) == 0xFF)
        {
            u4PayloadType += 255;
            if(u4Cnt++ == 0x10)
            {
                *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
                LOG(2, "SEI Err\n");
                return;
            }
        }
        u4PayloadType += u4Byte;   // this is the last byte

        u4PayloadSize = 0;
        u4Cnt = 0;
        while ((u4Byte = (_VDEC_AVCGetShtBitStream(u4VDecID, 8) >> 24)) == 0xFF)
        {
            u4PayloadSize += 255;
            if(u4Cnt++ == 0x10)
            {
                *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
                LOG(2, "SEI Err\n");
                return;
            }
        }
        u4PayloadSize += u4Byte;   // this is the last byte

        if((u4PayloadSize == 0) || (u4PayloadSize > u4SEINaluSz))
        {
            //LOG(2, "SEI PayloadSz=0, rptr0x%x\n", _VDEC_VldRPtr());
            *pi4RetValue = (INT32)E_VDEC_OK;
            return;
        }
        else
        {
            u4SEINaluSz -= u4PayloadSize;
        }
        switch ( u4PayloadType )
        {
            case  SEI_PIC_TIMING:
                if(!_vInterpretSEI_PicTiming(u4VDecID, prDecPrm))
                {
                    while(u4PayloadSize > 0)
                    {
                        u4Byte = _VDEC_AVCGetBitStream(u4VDecID, 8);
                        u4PayloadSize --;
                    }
                }
                *pi4RetValue = (INT32)E_VDEC_OK;
                break;
            case SEI_USER_DATA_REGISTERED_ITU_T_T35:
#ifdef CC_DIRECTTV_USER_DATA
                u4DataStart = _VDEC_VldRPtr(u4VDecID);
#endif
                u4DataLen = u4PayloadSize;    
                u4PayloadSizeOrg = u4PayloadSize;
                if(u4PayloadSize > 8)
                {
                    UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 8));    //coountry_code
                    UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 16));   //provider_code
                    u4AFDId = _VDEC_AVCGetRealShtBitStream(u4VDecID, 32);   //afd_identifier
                    u4PayloadSize -= 7;
                    if(u4AFDId == AFD_IDENTIFIER_DTG1)
                    {
                        UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 1));    //zero_bit
                        u4Byte = _VDEC_AVCGetRealShtBitStream(u4VDecID, 1);   //active_format_flag
                        UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 6));   //alignment_bits
                        u4PayloadSize --;
                        if(u4Byte == 1)
                        {
                            UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 4));//reserved
                            u4Byte = _VDEC_AVCGetRealShtBitStream(u4VDecID, 4);//active_format
                            u4PayloadSize--;
                            prDecPrm->rSEI.u1AFD = u4Byte;
                            LOG(7,"SEI AFD %d\n",prDecPrm->rSEI.u1AFD);
                            if(prDecPrm->u1CurViewIdx < MAX_VIEW_NUM)
                            {
                                prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx]);
                            }
                            if(prFbmSeqHdr != NULL)
                            {
                                if(prFbmSeqHdr->ucActFmt != (UCHAR)u4Byte)
                                {
                                    LOG(2, "AFD Chg %d, AspRatio %d\n", u4Byte, prFbmSeqHdr->ucAspRatInf);
                                    prFbmSeqHdr->ucActFmt = (UCHAR)u4Byte;
                                    FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], FBM_FLAG_SEQ_CHG);
                                }
                            }
                        }
                    }
                    #ifndef CC_DIRECTTV_USER_DATA
                    else if(u4AFDId == ATSC_IDENTIFIER)
                    {
                        u4DataStart = _VDEC_VldRPtr(u4VDecID);
                        
                        #if defined(CC_VERIZON_CAPTION_USER_DATA)
                        u4DataStart -= 4;
                        if(u4DataStart < prVdec->rPesInfo.u4FifoStart)
                        {
                            u4DataStart = prVdec->rPesInfo.u4FifoEnd - (prVdec->rPesInfo.u4FifoStart - u4DataStart);
                        }
                        u4DataLen -= 3;
                        #endif
                    }
                    #endif
                }
                while(u4PayloadSize > 0)
                {
                    u4Byte = _VDEC_AVCGetBitStream(u4VDecID, 8);
                    u4PayloadSize --;
                }
            #ifdef CC_SEI_USER_DATA_ENABLE
                #if defined(CC_DIRECTTV_USER_DATA) || defined(CC_VERIZON_CAPTION_USER_DATA)
                //save user data to data structure
                if(u4DataStart > 0)
                {
                    if((prDecPrm->rSEI.u4UserDataWptr + u4DataLen) >= MAX_USERDATA_SIZE)
                    {
                        LOG(1, "UsrData in SEI len %d > max %d\n", prDecPrm->rSEI.u4UserDataWptr, MAX_USERDATA_SIZE);
                        prDecPrm->rSEI.u4UserDataWptr = 0;
                    }
                    else
                    {
                        //u4Sz = prVdec->rPesInfo.u4FifoEnd - u4DataStart;
                        pu1SEIPtr = &prDecPrm->rSEI.au1UsrData[prDecPrm->rSEI.u4UserDataWptr];
                        if(u4DataLen < MAX_USERDATA_SIZE)
                        {
                            prDecPrm->rSEI.u4UserDataWptr += u4DataLen;
                            _VDEC_SetBsInfo(BARREL1, (UCHAR)u4VDecID, u4DataStart, prVdec->rPesInfo.u4VldReadPtrEnd, prVdec);
                            u4PayloadSize = u4PayloadSizeOrg - u4DataLen;
                            while(u4DataLen-- > 0)
                            {
                                *pu1SEIPtr++ = (UINT8)_VDEC_AVCGetBitStream(u4VDecID, 8);
                            }
                        }

                    }
                }
                #else //defined(CC_DIRECTTV_USER_DATA) || defined(CC_VERIZON_CAPTION_USER_DATA)
                if(u4DataStart > 0)
                {   
                    UINT32 u4Sz = 0;
                    
                    //u4DataStart += 8;//u4UserDataStartCode , u4ATSCIdentifier, u1UserDataTypeCode 
                    if(u4DataStart >= prVdec->rPesInfo.u4FifoEnd)
                    {
                        u4DataStart = prVdec->rPesInfo.u4FifoStart + (prVdec->rPesInfo.u4FifoEnd - u4DataStart);
                    }
                    UINT8 u1DataVal, au1TmpBuf[96];
                    UINT32 u4CcCount, u4IdxSrc, u4IdxDst;
                    BOOL fgProcCc;
                    u1DataVal = (*(UINT8*)VIRTUAL(u4DataStart));
                    if(u1DataVal == EIA_CC_TYPE)
                    {
                        u4DataStart += 1;//u1CCCount:5, u1AdditionalDataFlag:1, u1ProcessCCFlag:1, u1ProcessEMFlag:1
                        if(u4DataStart >= prVdec->rPesInfo.u4FifoEnd)
                        {
                            u4DataStart = prVdec->rPesInfo.u4FifoStart + (prVdec->rPesInfo.u4FifoEnd - u4DataStart);
                        }
                        u1DataVal = (*(UINT8*)VIRTUAL(u4DataStart));
                        fgProcCc = (BOOL)((u1DataVal & 0x40) >> 6);        // 1bit
                        //fgAddData = (BOOL)((u1DataVal & 0x20) >> 5);      // 1 bit
                        u4CcCount = (UINT32)((u1DataVal & 0x1F));      //5 bits
                       //ASSERT(u4CcCount <= 2);

                        u4DataStart += 2;//u1EMData
                        if(u4DataStart >= prVdec->rPesInfo.u4FifoEnd)
                        {
                            u4DataStart = prVdec->rPesInfo.u4FifoStart + (prVdec->rPesInfo.u4FifoEnd - u4DataStart);
                        }

                        u4DataLen = (u4CcCount << 2) - u4CcCount; //24 bit for each u4CcCount
                        u4Sz = prVdec->rPesInfo.u4FifoEnd - u4DataStart;                        
                        if(u4DataLen <= u4Sz)  // input ring buffer
                        {
                            x_memcpy((void*)au1TmpBuf, 
                            (void*)VIRTUAL(u4DataStart), u4DataLen);
                            
                        }
                        else
                        {
                            x_memcpy((void*)au1TmpBuf, 
                                (void*)VIRTUAL(u4DataStart), u4Sz);
                            x_memcpy((void*)&au1TmpBuf[u4Sz], 
                                (void*)VIRTUAL(prVdec->rPesInfo.u4FifoStart), u4DataLen - u4Sz);
                        }

                        //prDecPrm->rSEI.au1UsrData[prDecPrm->rSEI.u4UserDataWptr]
                        u4IdxDst = prDecPrm->rSEI.u4UserDataWptr;
                        if(fgProcCc)
                        {
                            for(u4IdxSrc = 0; u4IdxSrc < u4DataLen; u4IdxSrc +=3)
                            {
                                prDecPrm->rSEI.au1UsrData[u4IdxDst++] = 0;
                                prDecPrm->rSEI.au1UsrData[u4IdxDst++] = au1TmpBuf[u4IdxSrc] & 0x7;
                                prDecPrm->rSEI.au1UsrData[u4IdxDst++] = au1TmpBuf[u4IdxSrc + 1];
                                prDecPrm->rSEI.au1UsrData[u4IdxDst++] = au1TmpBuf[u4IdxSrc + 2];
                            }
                        }
                        prDecPrm->rSEI.u4UserDataWptr += (u4CcCount << 2);
                    }
                }
                #endif
            #else //CC_SEI_USER_DATA_ENABLE
                //UNUSED(u4Sz);
                UNUSED(u4DataLen);
                UNUSED(u4DataStart);
            #endif
                break;
            case SEI_RECOVERY_POINT:
                prDecPrm->rSEI.u4RecoveryFrmCnt = _VDEC_UeCodeNum(u4VDecID);
                prDecPrm->rSEI.u1ExtMatchFlg = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 3));
                break;
            case SEI_USER_DATA_UNREGISTERED:
                #ifndef DIVX_PLUS_CER
                u4TempPayLoadSize = 0;
                #endif
                #ifdef CRCFLOWTEST//for haitao debug
                VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
                UCHAR i=0;
                prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);

                if(u4PayloadSize >= 16)
                {
                    for(i=0;i<4;i++)
                    {
                        prVdecEsInfoKeep->u4Ycrc[i] = _VDEC_AVCGetRealShtBitStream(u4VDecID, 32);
                        u4TempPayLoadSize += 4;
                    }
                    
                    for(i=0;i<4;i++)
                    {
                        prVdecEsInfoKeep->u4Ccrc[i] = _VDEC_AVCGetRealShtBitStream(u4VDecID, 32);
                        u4TempPayLoadSize += 4;
                    }
                }
                #else //CRCFLOWTEST
                #ifndef DIVX_PLUS_CER //Divx plus check                               
                //move 128 bit UUID first
                //UINT32 u4Data;
                if(u4PayloadSize >= 16)
                {
                    for(i = 0; i < 4; i++)
                    {                     
                        _VDEC_AVCGetRealShtBitStream(u4VDecID, 32);
                        u4TempPayLoadSize += 4;
                    }
                }
                if(AVCHD_XVYCC_IDENTIFIER == _VDEC_AVCGetShtBitStream(u4VDecID, 0))
                {
                    _VDEC_AVCGetBitStream(u4VDecID, 32);
                    u4Temp = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8);   // Format_Flg to GBD_Color_Space
                    prDecPrm->rSEI.rxvYCCInfo.ucFormatFlag = ((u4Temp & 0x90) >> 6) ;
                    prDecPrm->rSEI.rxvYCCInfo.ucGEDColorPrecision = ((u4Temp & 0x19) >> 3);
                    prDecPrm->rSEI.rxvYCCInfo.ucGEDColorSpace = (u4Temp & 0x7);
                    prDecPrm->rSEI.rxvYCCInfo.u4RedData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_R Min_R
                    prDecPrm->rSEI.rxvYCCInfo.u4GreenData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_G Min_G
                    prDecPrm->rSEI.rxvYCCInfo.u4BlueData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_B Min_B
                }
                else
                {
                    i = 0;
                    //follow divx plus spec "Divx Plus" at next 9 byte
                    while((u4TempPayLoadSize < u4PayloadSize) && (i < 9))
                    {
                        s_divx_check_buf[i++] = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8);
                        u4TempPayLoadSize++;
                    }

                    u4PayloadSize -= u4TempPayLoadSize;
                    while(u4PayloadSize > 0)
                    {
                        u4Byte = _VDEC_AVCGetBitStream(u4VDecID, 8);
                        u4PayloadSize --;
                    }

                    if (x_strncmp(s_divx_check_buf, "DivX Plus", 9) == 0)
                    {
                        *pi4RetValue = (INT32)E_VDEC_BITSTREAM_NOT_SUPPORT;
                        LOG(0, "%s: DivX Plus bitstream not support.\n", __FUNCTION__);
                        return;
                    }
                }
                #else//parse xvYCC color 
                if(u4PayloadSize >= 16)
                {
                    for(i = 0; i < 4; i++)
                    {                     
                        _VDEC_AVCGetRealShtBitStream(u4VDecID, 32);
                        u4TempPayLoadSize += 4;
                    }
                }
                // check TypeIndicator
                if(AVCHD_XVYCC_IDENTIFIER == _VDEC_AVCGetShtBitStream(u4VDecID, 0))
                {
                    _VDEC_AVCGetBitStream(u4VDecID, 32);
                    u4Temp = _VDEC_AVCGetRealShtBitStream(u4VDecID, 8);   // Format_Flg to GBD_Color_Space
                    prDecPrm->rSEI.rxvYCCInfo.ucFormatFlag = ((u4Temp & 0x90) >> 6) ;
                    prDecPrm->rSEI.rxvYCCInfo.ucGEDColorPrecision = ((u4Temp & 0x19) >> 3);
                    prDecPrm->rSEI.rxvYCCInfo.ucGEDColorSpace = (u4Temp & 0x7);
                    prDecPrm->rSEI.rxvYCCInfo.u4RedData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_R Min_R
                    prDecPrm->rSEI.rxvYCCInfo.u4GreenData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_G Min_G
                    prDecPrm->rSEI.rxvYCCInfo.u4BlueData = _VDEC_AVCGetRealShtBitStream(u4VDecID, 24);   // Max_B Min_B
                }
                #endif
                #endif
                break;                        
            case SEI_FRAME_PACKING_ARRANGE:
                prDecPrm->rSEI.u4FramePackingID = _VDEC_UeCodeNum(u4VDecID);
                prDecPrm->rSEI.u1FramePackingFlag = 1 - (((UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1)) & 0x1);
                if(prDecPrm->rSEI.u1FramePackingFlag)
                {
                    prDecPrm->rSEI.u1FramePackingType = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 7);
                    prDecPrm->rSEI.u1QuincunxSamplingFlag = (UINT8)_VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                    e3DType = prVdecEsInfo->e3DType;
                    
                    if(prDecPrm->rSEI.u1FramePackingType == 3)
                    {
                        e3DType = VDEC_3D_SBS_LF;
                    }
                    else if(prDecPrm->rSEI.u1FramePackingType == 4)
                    {
                        e3DType = VDEC_3D_TB_LF;
                    }
                    if(prVdecEsInfo->e3DType <= VDEC_3D_TB_RF)
                    {
                        LOG(3, "Detect SEI 3D Type Change %d->%d, Quincunx %d\n", 
                            prVdecEsInfo->e3DType, e3DType, prDecPrm->rSEI.u1QuincunxSamplingFlag);
                            prVdecEsInfo->e3DType = e3DType;
                    }
                    else
                    {
                        LOG(6, "Skip SEI 3D Info %d, pipeline conflict with original %d\n", e3DType, prVdecEsInfo->e3DType);
                    }
                    UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 9));
                    prDecPrm->rSEI.u1CurrentFrameIsFrame0Flag = _VDEC_AVCGetRealShtBitStream(u4VDecID, 1);
                    UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 2));
                    if((!prDecPrm->rSEI.u1QuincunxSamplingFlag) && (prDecPrm->rSEI.u1FramePackingType != 5))
                    {
                        UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 16));
                    }
                    UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 8));
                    UNUSED(_VDEC_UeCodeNum(u4VDecID));
                }
                UNUSED(_VDEC_AVCGetRealShtBitStream(u4VDecID, 1));
                break;
            default:
                if(u4PayloadType >= (UINT32)SEI_MAX_ELEMENTS)
                {
                    //for error bitstream, but dmx keep updating wptr, 
                    //vdec may spend too much time in this function                
                    x_thread_delay(1);
                    if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)//for iframe special case
                    {
                        *pi4RetValue = (INT32)E_VDEC_OK;
                    }
                    else
                    {
                        *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
                    }
                    return;
                }
                //reserved_sei_message
                while(u4PayloadSize > 0)
                {
                    u4Byte = _VDEC_AVCGetBitStream(u4VDecID, 8);
                    u4PayloadSize --;
                }
                break;
        }
        //payloadtype is legal but still possible to be error data
        //stop parsing if the count > SEI_MAX_ELEMENTS
        if(u4SEICnt++ > SEI_MAX_ELEMENTS) 
        {
            *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
            LOG(2, "SEI Err, Max SEICnt\n");
            return;
        }
        _VDEC_TrailingBits(u4VDecID);
    }
    while (_fgIsMoreRbspData(u4VDecID, prVdec->rPesInfo.u4VldReadPtrEnd));   // more_rbsp_data()  msg[offset] != 0x80
    
    *pi4RetValue = (INT32)E_VDEC_OK;
}


/** Get Profile/Level information
*@param .
 */
static void _VDEC_GetProfileLevel(VDEC_AVC_SPS_T* prSPS, VDEC_ES_INFO_T* prVdecEsInfo)
{
    switch (prSPS->u4ProfileIdc)
    {
        case 66:
            prVdecEsInfo->eProfile = VDEC_PROFILE_BASELINE;
            break;
        case 77:
            prVdecEsInfo->eProfile = VDEC_PROFILE_MAIN;
            break;
        case 88:
            prVdecEsInfo->eProfile = VDEC_PROFILE_EXTENDED;
            break;
        case 100:
            prVdecEsInfo->eProfile = VDEC_PROFILE_HIGH;
            break;
        case 110:
            prVdecEsInfo->eProfile = VDEC_PROFILE_HIGH_10;
            break;
        case 122:
            prVdecEsInfo->eProfile = VDEC_PROFILE_HIGH_4_2_2;
            break;
        case 144:
            prVdecEsInfo->eProfile = VDEC_PROFILE_HIGH_4_4_4;
            break;
        default:
            prVdecEsInfo->eProfile = VDEC_PROFILE_UNKNOWN;
            break;
    }
    switch (prSPS->u4LevelIdc)
    {
        case 9:
            prVdecEsInfo->eLevel = VDEC_LEVEL_1_B;
            break;
        case 10:
            prVdecEsInfo->eLevel = VDEC_LEVEL_1;
            break;
        case 11:
            prVdecEsInfo->eLevel = VDEC_LEVEL_1_1;
            break;
        case 12:
            prVdecEsInfo->eLevel = VDEC_LEVEL_1_2;
            break;
        case 13:
            prVdecEsInfo->eLevel = VDEC_LEVEL_1_3;
            break;
        case 20:
            prVdecEsInfo->eLevel = VDEC_LEVEL_2;
            break;
        case 21:
            prVdecEsInfo->eLevel = VDEC_LEVEL_2_1;
            break;
        case 22:
            prVdecEsInfo->eLevel = VDEC_LEVEL_2_2;
            break;
        case 30:
            prVdecEsInfo->eLevel = VDEC_LEVEL_3;
            break;
        case 31:
            prVdecEsInfo->eLevel = VDEC_LEVEL_3_1;
            break;
        case 32:
            prVdecEsInfo->eLevel = VDEC_LEVEL_3_2;
            break;
        case 40:
            prVdecEsInfo->eLevel = VDEC_LEVEL_4;
            break;
        case 41:
            prVdecEsInfo->eLevel = VDEC_LEVEL_4_1;
            break;
        case 42:
            prVdecEsInfo->eLevel = VDEC_LEVEL_4_2;
            break;
        case 50:
            prVdecEsInfo->eLevel = VDEC_LEVEL_5;
            break;
        case 51:
            prVdecEsInfo->eLevel = VDEC_LEVEL_5_1;
            break;
        default:
            prVdecEsInfo->eLevel = VDEC_LEVEL_UNKNOWN;
            break;
    }
}


static void _VDEC_WaitRAPToDisp(UCHAR ucEsId, VDEC_AVC_DecPrm* prDecPrm)
{    
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);    
#ifdef TIME_MEASUREMENT
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    CHAR szBuf[15];
#endif

    UNUSED(prVdecEsInfo);
    UNUSED(prVdecEsInfoKeep);
#ifdef TIME_MEASUREMENT
    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));        
    prInfo = &(_prDbase->rMpv[prDecPrm->ucEsId]);

    if(!prDecPrm->fgIsFirstFrm)
    {
        prInfo->u4PsrDropNs++;
    }
#endif
    
    if(!prDecPrm->fgIsFirstFrm) //first display
    {
        if(!prDecPrm->bmvcpair)
        {
            if(((!prDecPrm->fgIsPVR)&&(!prDecPrm->fgIsMMPlay)) || (prDecPrm->rSEI.u1WaitRAPICnt > 0))  // start counting from I frame  // start counting from I frame
            {
                if(fgIsBSlice(prDecPrm->prSliceHdr->u4SliceType))
                {
                    prDecPrm->ucBNum++;
                }
            }
            //wait RAP
            if(fgIsIDRPic(prDecPrm->u1NalUnitType) && (prDecPrm->u1CurViewIdx == 0)/* ||
                            prDecPrm->rSEI.u1ExtMatchFlg*/)     //DTV00139283, can't use as RAP
            {
                LOG(2, "1st Frm Start Prs\n");
                prDecPrm->fgIsFirstFrm = TRUE;
                prDecPrm->fgFirstFrmTag = TRUE;
                prDecPrm->ucBNum = 0;
                if (!_fg_boot_profile)
                {
                    x_os_drv_set_timestamp("REC_1st_I, IDR");
                    _fg_boot_profile = TRUE;
                }
                #ifdef TIME_MEASUREMENT
                SPrintf(szBuf, "REC_1st_I to Parse(%3d)", prInfo->u4PsrDropNs);
                TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, szBuf);
                 TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, szBuf);
                //TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "REC_1st_I");
                #endif
            }
            else 
            {
                if(((!prDecPrm->fgIsPVR)&&(!prDecPrm->fgIsMMPlay)) 
                    || (prDecPrm->rSEI.u1WaitRAPICnt > 0))  // start counting from I frame
                {
                    prDecPrm->rSEI.u1WaitRAPCnt++;
                }
                prDecPrm->ucBThrd = 1;
                if(prDecPrm->fgIsMMPlay && ((prDecPrm->rSEI.u1WaitRAPCnt > WAIT_RAP_FRM_CNT) ||
                    (prDecPrm->rSEI.u1WaitRAPRefCnt > (UINT8)(prDecPrm->prSPS->u4NumRefFrames << 1)) ||
                    (prDecPrm->rSEI.u1WaitRAPICnt > (UINT8)prDecPrm->prSPS->u4NumRefFrames)))
                {  
                    if(prDecPrm->prSliceHdr->fgFieldPicFlag)
                    {
                        prDecPrm->ucBThrd *= 2;
                    }
                    if(prDecPrm->prSPS->u4NumRefFrames <= 2)
                    {
                        prDecPrm->ucBThrd *= 2;
                    }
                    if(!(prDecPrm->rSEI.u1WaitRAPICnt > (UINT8)prDecPrm->prSPS->u4NumRefFrames*prDecPrm->ucBThrd))
                    {
                        if((prDecPrm->rSEI.u1WaitRAPCnt - prDecPrm->ucBNum)<4*(1 + prDecPrm->prSliceHdr->fgFieldPicFlag))
                        {
                            prDecPrm->ucBThrd *= 2;
                        }
                    } 
                }
                if((prDecPrm->u1CurViewIdx == 0) && ((prDecPrm->rSEI.u1WaitRAPCnt > WAIT_RAP_FRM_CNT) ||
                    (prDecPrm->rSEI.u1WaitRAPRefCnt > ((UINT8)(prDecPrm->prSPS->u4NumRefFrames << 1)*prDecPrm->ucBThrd)) ||
                    (prDecPrm->rSEI.u1WaitRAPICnt > ((UINT8)prDecPrm->prSPS->u4NumRefFrames)*prDecPrm->ucBThrd)))
                {  
                    LOG(2, "1st Frm Start Disp(WaitRAP Cnt %d, Ref I%d/Ref%d/Seq%d, Type %d)\n", 
                        prDecPrm->rSEI.u1WaitRAPCnt,
                        prDecPrm->rSEI.u1WaitRAPICnt, prDecPrm->rSEI.u1WaitRAPRefCnt,
                        prDecPrm->prSPS->u4NumRefFrames,
                        prDecPrm->prSliceHdr->u4SliceType);
                    prDecPrm->fgIsFirstFrm = TRUE;
                    prDecPrm->fgFirstFrmTag = TRUE;
                    prDecPrm->ucBNum = 0;
                    if (!_fg_boot_profile)
                    {
                        x_os_drv_set_timestamp("REC_1st_I");
                        _fg_boot_profile = TRUE;
                    }
                    #ifdef TIME_MEASUREMENT
                    SPrintf(szBuf, "REC_1st_I to Display(%3d)", prInfo->u4PsrDropNs);
                    TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, szBuf);
                      TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, szBuf);
                    //TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "REC_1st_I");
                    #endif
                }
                else
                {
                    if(fgIsRefPic(prDecPrm->u1NalRefIdc))
                    {   //decode but not display
                        if(fgIsISlice(prDecPrm->prSliceHdr->u4SliceType))
                        {
                            prDecPrm->rSEI.u1WaitRAPICnt++;
                            #if defined(CC_USE_DDI)
                            if (((!prVdecEsInfoKeep->fgVPush) && (!prDecPrm->fgIsPVR))  || 
                                 (prVdecEsInfo->fgThumbMethod2 && !prVdecEsInfo->fgThumbStartTime))// only for DTV case.
                            {
                                prVdecEsInfo->fgThumbStartTime = TRUE;
                                prDecPrm->fgIsFirstFrm = TRUE;
                                prDecPrm->fgFirstFrmTag = TRUE;
                                prDecPrm->fgWaitPPS = FALSE;
                                LOG(2, "1st IFrm Start Disp(WaitRAP Cnt %d, Ref I%d/Ref%d/Seq%d, Type %d,ST(%d))\n", 
                                        prDecPrm->rSEI.u1WaitRAPCnt,
                                        prDecPrm->rSEI.u1WaitRAPICnt, prDecPrm->rSEI.u1WaitRAPRefCnt,
                                        prDecPrm->prSPS->u4NumRefFrames,
                                        prDecPrm->prSliceHdr->u4SliceType,prVdecEsInfo->fgThumbStartTime);
                            }
                            else
                            #endif
                        #ifdef ENABLE_MULTIMEDIA 
                            // LWB: For I mode timeshift, we just need the first I frame 
                            if ((IMode == prDecPrm->ucSkipMode) &&
                                (SWDMX_FMT_MPEG2_TS_TIME_SHIFT == prVdecEsInfo->eContainerType))
                            {
                                prDecPrm->fgIsFirstFrm = TRUE;
                                prDecPrm->fgFirstFrmTag = TRUE;
                                prDecPrm->fgWaitPPS = FALSE;
                                LOG(2, "1st IFrm Start Disp(quick mode)(WaitRAP Cnt %d, Ref I%d/Ref%d/Seq%d, Type %d)\n", 
                                        prDecPrm->rSEI.u1WaitRAPCnt,
                                        prDecPrm->rSEI.u1WaitRAPICnt, prDecPrm->rSEI.u1WaitRAPRefCnt,
                                        prDecPrm->prSPS->u4NumRefFrames,
                                        prDecPrm->prSliceHdr->u4SliceType);
                            }
                            else
                        #endif
                            {
                            }
                        }
                        
                        if (prDecPrm->rSEI.u1WaitRAPICnt > 0)    // start counting from I frame
                        {
                            prDecPrm->rSEI.u1WaitRAPRefCnt++;
                        }                    
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



