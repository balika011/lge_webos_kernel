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
 * $RCSfile: vdec_h265_psr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

#define DEFINE_IS_LOG VDEC_IsLog
#include "vdec_h265_dec.h"
#include "vdec_drvif.h"


const UINT32 H265_scalingListSize[4] = {16,64,256,1024}; 
const UINT32 H265_scalingListSizeX[4] = { 4, 8, 16,  32};
const UINT32 H265_scalingListNum[SCALING_LIST_SIZE_NUM]={6,6,6,2};
const INT32  H265_invQuantScales[6] =
{
  40,45,51,57,64,72
};

INT32 H265_quantTSDefault4x4[16] =
{
  16,16,16,16,
  16,16,16,16,
  16,16,16,16,
  16,16,16,16
};

INT32 H265_quantIntraDefault8x8[64] =
{
  16,16,16,16,17,18,21,24,
  16,16,16,16,17,19,22,25,
  16,16,17,18,20,22,25,29,
  16,16,18,21,24,27,31,36,
  17,17,20,24,30,35,41,47,
  18,19,22,27,35,44,54,65,
  21,22,25,31,41,54,70,88,
  24,25,29,36,47,65,88,115
};

INT32 H265_quantInterDefault8x8[64] =
{
  16,16,16,16,17,18,20,24,
  16,16,16,17,18,20,24,25,
  16,16,17,18,20,24,25,28,
  16,17,18,20,24,25,28,33,
  17,18,20,24,25,28,33,41,
  18,20,24,25,28,33,41,54,
  20,24,25,28,33,41,54,71,
  24,25,28,33,41,54,71,91
};


#define _MAKE_64X(val) ((((val) + 0x3F) >> 6) << 6)

static HANDLE_T ghVdecH265FinSema[4] = {(HANDLE_T)NULL,(HANDLE_T)NULL,(HANDLE_T)NULL,(HANDLE_T)NULL};
#ifdef VDEC_H265_Performance
HAL_TIME_T rTimeE[4];
UINT32 gu4MCcnt[4];
#endif

// *********************************************************************
// Function    : void vVDecH265InitIsr(UINT32 u4InstID)
// Description : register ISR function
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecH265InitIsr(UINT32 u4InstID)
{
    UNUSED(u4InstID);
    
    if(ghVdecH265FinSema[VDEC_VLD0] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&(ghVdecH265FinSema[VDEC_VLD0]), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    _VDEC_RegIsr(VDEC_VLD0_VECTOR, vVDecH265Isr);
    
    #ifdef CC_DUAL_CORE_DEC
    if(ghVdecH265FinSema[VDEC_VLD1] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&(ghVdecH265FinSema[VDEC_VLD1]), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    _VDEC_RegIsr(VDEC_VLD1_VECTOR, vVDecH265Isr);

    //LAE0
    if(ghVdecH265FinSema[VDEC_LAE0] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&(ghVdecH265FinSema[VDEC_LAE0]), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    _VDEC_RegIsr(VDEC_LAE0_VECTOR, vVDecH265Isr);
    
    //LAE1
    if(ghVdecH265FinSema[VDEC_LAE1] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&(ghVdecH265FinSema[VDEC_LAE1]), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    _VDEC_RegIsr(VDEC_LAE1_VECTOR, vVDecH265Isr);
    #endif
}

// *********************************************************************
// Function    : void vVDecH265Isr(UINT16 u4Vector)
// Description : ISR function
// Parameter   : u4Vector
// Return      : None
// *********************************************************************
void vVDecH265Isr(UINT16 u4Vector)
{
    if (u4Vector == VDEC_VLD0_VECTOR)
    {
        #ifdef VDEC_H265_Performance
        HAL_GetTime(&(rTimeE[0]));
        gu4MCcnt[0] = u4VDecReadMC(0,(478 << 2));
        #endif
        BIM_ClearIrq(VDEC_VLD0_VECTOR);
        VERIFY(x_sema_unlock(ghVdecH265FinSema[VDEC_VLD0]) == OSR_OK);
    }
    else if (u4Vector == VDEC_VLD1_VECTOR)
    {
        BIM_ClearIrq(VDEC_VLD1_VECTOR);
        VERIFY(x_sema_unlock(ghVdecH265FinSema[VDEC_VLD1]) == OSR_OK);
    }
    else if (u4Vector == VDEC_LAE0_VECTOR) //LAE0 isr Clear
    {
        BIM_ClearIrq(VDEC_LAE0_VECTOR);
        VERIFY(x_sema_unlock(ghVdecH265FinSema[VDEC_LAE0]) == OSR_OK);
    }
    else if(u4Vector == VDEC_LAE1_VECTOR) //LAE1 isr Clear
    {
        BIM_ClearIrq(VDEC_LAE1_VECTOR);
        VERIFY(x_sema_unlock(ghVdecH265FinSema[VDEC_LAE1]) == OSR_OK);
    }
}


// *********************************************************************
// Function    : void vVDecH265WaitDecFinish(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : Check decode finish
// Parameter   : u4InstID; u4VDecID; prH265DecPrm
// Return      : None
// *********************************************************************
void vVDecH265WaitDecFinish(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4DecodeErr = 0;
    BOOL fgDecTimeout = FALSE;
    
    if (x_sema_lock_timeout(ghVdecH265FinSema[u4VDecID], 100) != OSR_OK)
    {
        fgDecTimeout = TRUE;
    }

    if(((prH265DecPrm->ucVDecStatus == REAL_DECODE) && (!prH265DecPrm->ucDualCoreMode))
        || (prH265DecPrm->ucVDecStatus == LAE_DECODE))
    {
        u4DecodeErr = u4VDEC_HAL_H265_GetErrMsg(u4VDecID);
    }
    
    if((u4DecodeErr != 0) || fgDecTimeout)
    {
        prH265DecPrm->u4DecodeErrCnt ++;
        if(fgDecTimeout)
        {
            VDecPrintf(u4InstID,VDEC_LOG_DROPPIC,"H265 Decode Timeout\n");   
        }
        VDecPrintf(u4InstID,VDEC_LOG_DROPPIC,"H265 Decode hw %d error type 0x%08x\n",u4VDecID,u4DecodeErr);
    }
    else
    {
        prH265DecPrm->u4DecodeOkCnt ++;
    }
}


// *********************************************************************
// Function    :INT32 * vVDecH265GetSLDefaultAddress( INT32 sizeId, INT32 listId)
// Description : Handle SL default address
// Parameter   : None
// Return      : (INT32 *) default SL address
// *********************************************************************

INT32 * vVDecH265GetSLDefaultAddress(INT32 u4SizeId, INT32 u4ListId)
{
  INT32 *src = 0;
  switch(u4SizeId)
  {
    case SCALING_LIST_4x4:
      src = H265_quantTSDefault4x4;
      break;
    case SCALING_LIST_8x8:
      src = (3 > u4ListId) ? H265_quantIntraDefault8x8 : H265_quantInterDefault8x8;
      break;
    case SCALING_LIST_16x16:
      src = (3 > u4ListId) ? H265_quantIntraDefault8x8 : H265_quantInterDefault8x8;
      break;
    case SCALING_LIST_32x32:
      src = (1 > u4ListId) ? H265_quantIntraDefault8x8 : H265_quantInterDefault8x8;
      break;
    default:
      src = NULL;
      break;
  }
  return src;
}


void vVDecH265Ref_Pic_List_Modification(UINT32 u4InstID,UINT32 u4VDecID)
{
    UNUSED(u4InstID);
    vVDEC_HAL_H265_RPL_Modification(u4VDecID);
}

// *********************************************************************
// Function    : void vVDecH265FlushDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : Flush all ready FBM to display Q
// Parameter   : u4InstID; prH265DecPrm;
// Return      : None
// *********************************************************************
void vVDecH265FlushDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4FbCnt;
    UCHAR ucMinPOCFBufIdx;
    FBM_PIC_HDR_T * prFbmPicHdr;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    for(u4FbCnt = 0; u4FbCnt < prH265DecPrm->ucMaxFBufNum; u4FbCnt++)
    {
        ucMinPOCFBufIdx = ucVDecH265GetMinPOC(u4InstID,prH265DecPrm);
        if(ucMinPOCFBufIdx == FBUF_ID_UNKNOWN)
        {
            continue;
        }
        else
        {
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId);
            if(prFbmPicHdr == NULL)
            {
                LOG(2, "[H265-INFO] prFbmPicHdr NULL in vVDecH265FlushDPB \n");
                break;
            }
            
            if(FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId) == FBM_FB_STATUS_READY)
            {
                if(VDEC_IsNotDisplay(u4InstID,prFbmPicHdr->u4PTS,prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset) && (!prH265DecPrm->fgEndOfSeq))
                {
                    VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"PQ POC %d,PTS %d Fbm[%d,%d] Flush to Empty For not display\n",prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].i4POC,prFbmPicHdr->u4PTS,prH265DecPrm->ucFbgId,ucMinPOCFBufIdx);
                    FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId,FBM_FB_STATUS_EMPTY);
                }
                else
                {
                    #ifdef CC_REALD_3D_SUPPORT
                    _VDEC_Check2D3DSwitch(u4InstID,prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId);
                    #endif
                    _VDEC_JobBeforePutDispQ(u4InstID,prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId);
                    FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId,FBM_FB_STATUS_DISPLAYQ);
                    prVdecEsInfo->u4DisplayQPicCnt ++;
                    VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"PQ POC %d,PTS %d Fbm[%d,%d] FLUSHQ\n",prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].i4POC,prFbmPicHdr->u4PTS,prH265DecPrm->ucFbgId,ucMinPOCFBufIdx);
                }
                prH265DecPrm->i4ReadyOutCntDPB --;
            }
            else if(FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId) == FBM_FB_STATUS_DECODE)
            {
                prH265DecPrm->i4ReadyOutCntDPB --;
                VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"PQ POC %d,PTS %d Fbm[%d,%d] Flush to Empty for FBM not ready\n",prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].i4POC,prFbmPicHdr->u4PTS,prH265DecPrm->ucFbgId,ucMinPOCFBufIdx);
                FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId,FBM_FB_STATUS_EMPTY);
            }

            if(prH265DecPrm->i4ReadyOutCntDPB < 0)
            {
                VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"VDEC has no picture [%d] for output\n",prH265DecPrm->i4ReadyOutCntDPB);
            }
            
            FBM_UpdateReferenceList(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucMinPOCFBufIdx].ucFbmFbId, FALSE);
            vVDecH265ClrFBufInfo(u4InstID,ucMinPOCFBufIdx,prH265DecPrm);
            
            if(prH265DecPrm->fgIsMMPlay)
            {
                UNUSED(VDEC_CheckStopStatus(u4InstID, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));    
            }
        }
    }
}

// *********************************************************************
// Function    : void vVDecH265ClrDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : Clear FBM kept in VDEC
// Parameter   : u4InstID; prH265DecPrm;
// Return      : None
// *********************************************************************
void vVDecH265ClrDPB(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4FbCnt;
    FBM_PIC_HDR_T * prFbmPicHdr;
    for(u4FbCnt = 0; u4FbCnt < prH265DecPrm->ucMaxFBufNum; u4FbCnt++)
    {
        if((FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId) == FBM_FB_STATUS_READY)
           || (FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId) == FBM_FB_STATUS_DECODE))
        {
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId);
            VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"Clear DPB POC %d PTS %d Fbm[%d,%d]\n",prH265DecPrm->prFBufInfo[u4FbCnt].i4POC,prFbmPicHdr->u4PTS,prH265DecPrm->ucFbgId,u4FbCnt);
            FBM_UpdateReferenceList(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId, FALSE);
            FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId,FBM_FB_STATUS_EMPTY);
            vVDecH265ClrFBufInfo(u4InstID,u4FbCnt,prH265DecPrm);
            prH265DecPrm->i4ReadyOutCntDPB --;
        }
        else
        {
            FBM_UpdateReferenceList(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[u4FbCnt].ucFbmFbId, FALSE);
            vVDecH265ClrFBufInfo(u4InstID,u4FbCnt,prH265DecPrm);
        }
              
    }

    prH265DecPrm->rLastInfo.i4LastPOC = -MAX_POC;
}

// *********************************************************************
// Function    : UCHAR ucVDecH265GetMinPOCToOutPut(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : Get min POC for output
// Parameter   : u4InstID; prH265DecPrm;
// Return      : Min POC & ready status fbuf idx
// *********************************************************************
UCHAR ucVDecH265GetMinPOCToOutPut(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UCHAR ucFbCnt,ucMinPOCFBufIdx = FBUF_ID_UNKNOWN;
    INT32 i4MinPOC = 0x7FFFFFFF;
    UCHAR ucFbmSt;
    for(ucFbCnt = 0; ucFbCnt < prH265DecPrm->ucMaxFBufNum; ucFbCnt++)
    {
        if((i4MinPOC > prH265DecPrm->prFBufInfo[ucFbCnt].i4POC) &&
           (prH265DecPrm->prFBufInfo[ucFbCnt].ucFBufStatus != NO_PIC))
        {
            ucFbmSt = FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucFbCnt].ucFbmFbId);
            if((ucFbmSt == FBM_FB_STATUS_READY) && (prH265DecPrm->prFBufInfo[ucFbCnt].eH265DpbStatus != H265_DPB_STATUS_OUTPUTTED))
            {
                i4MinPOC = prH265DecPrm->prFBufInfo[ucFbCnt].i4POC;
                ucMinPOCFBufIdx = ucFbCnt;
            }
        }

//        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-FB] %d POC %d FbmST %d DpbST %d ucMinFbidx %d\n",ucFbCnt,prH265DecPrm->prFBufInfo[ucFbCnt].i4POC,
//                FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucFbCnt].ucFbmFbId),
//                prH265DecPrm->prFBufInfo[ucFbCnt].eH265DpbStatus,ucMinPOCFBufIdx);
    }

    return ucMinPOCFBufIdx;
}


// *********************************************************************
// Function    : UCHAR ucVDecH265GetMinPOC(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : Get min POC fbuf
// Parameter   : u4InstID; prH265DecPrm;
// Return      : Min POC fbuf idx
// *********************************************************************
UCHAR ucVDecH265GetMinPOC(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UCHAR ucFbCnt,ucMinPOCFBufIdx = FBUF_ID_UNKNOWN;
    INT32 i4MinPOC = 0x7FFFFFFF;
    
    for(ucFbCnt = 0; ucFbCnt < prH265DecPrm->ucMaxFBufNum; ucFbCnt++)
    {
        if((i4MinPOC > prH265DecPrm->prFBufInfo[ucFbCnt].i4POC) &&
           (prH265DecPrm->prFBufInfo[ucFbCnt].ucFBufStatus != NO_PIC))
        {
            i4MinPOC = prH265DecPrm->prFBufInfo[ucFbCnt].i4POC;
            ucMinPOCFBufIdx = ucFbCnt;
        }

//        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-FB] %d POC %d FbmST %d DpbST %d ucMinFbidx %d\n",ucFbCnt,prH265DecPrm->prFBufInfo[ucFbCnt].i4POC,
//                FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[ucFbCnt].ucFbmFbId),
//                prH265DecPrm->prFBufInfo[ucFbCnt].eH265DpbStatus,ucMinPOCFBufIdx);
    }

    return ucMinPOCFBufIdx;
}

// *********************************************************************
// Function    : BOOL fgVDecH265IsNonRefFBuf(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : check buf whether ref
// Parameter   : u4InstID; prH265DecPrm;
// Return      : ref : true; non-ref : false
// *********************************************************************
BOOL fgVDecH265IsNonRefFBuf(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    return (prH265DecPrm->prFBufInfo[u4FBufIdx].ucFBufRefType == NREF_PIC);
}


/************************************************************/
/*UCHAR ucVDecH265GetNonRefFBuf(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)*/
/*Param: */
/*u4InstID : Instant ID*/
/*prH265DecPrm : */
/*return : Non Ref FBuf idx*/
/*************************************************************/
UCHAR ucVDecH265GetNonRefFBuf(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UCHAR ucFbCnt,ucNonRefFBufIdx = FBUF_ID_UNKNOWN;
    for(ucFbCnt = 0; ucFbCnt < prH265DecPrm->ucMaxFBufNum; ucFbCnt++)
    {
        if(fgVDecH265IsNonRefFBuf(u4InstID,ucFbCnt,prH265DecPrm))
        {
            ucNonRefFBufIdx = ucFbCnt;
            break;
        }
    }
    return ucNonRefFBufIdx;
}


void vVDecH265SetRefFBuf(VDEC_INFO_H265_FBUF_INFO_T *prFBufInfo,UCHAR ucFbIdx,UCHAR ucRef)
{
    prFBufInfo[ucFbIdx].ucFBufRefType = ucRef;
}

// *********************************************************************
// Function    : void vVDecH265ClrFBufInfo(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description : clear fbuf 
// Parameter   : u4InstID; u4FBufIdx; prH265DecPrm;
// Return      : non
// *********************************************************************
void vVDecH265ClrFBufInfo(UINT32 u4InstID,UINT32 u4FBufIdx,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    VDEC_INFO_H265_FBUF_INFO_T *pClrFBuf = NULL;
    pClrFBuf = &(prH265DecPrm->prFBufInfo[u4FBufIdx]);

    VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-TEST-%d] Clear FBuf %d,i4POC %d\n",u4InstID,u4FBufIdx,pClrFBuf->i4POC);
    
    pClrFBuf->eH265DpbStatus = H265_DPB_STATUS_EMPTY;
    pClrFBuf->ucFBufStatus = NO_PIC;    
    pClrFBuf->ucFBufRefType = NREF_PIC;
    pClrFBuf->i4POC = MAX_POC;
    pClrFBuf->u4PicCnt = 0;
    pClrFBuf->bIsUFOEncoded = 0;
    pClrFBuf->u4POCBits = 0xffffffff;
    pClrFBuf->bLtMsbPresentFlag = 0;
    pClrFBuf->bFirstSliceReferenced = 1;
    pClrFBuf->bUsedByCurr = 0;
    pClrFBuf->bUsedAsLongTerm = 0;
    pClrFBuf->fgMVUsed = 0;
}

INT32 i4VDecH265AllocateFBG(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4FbgWidth,u4FbgHeight;
    UINT32 u4PrevWidth,u4PrevHeight;
    UCHAR ucFbgType;
    UCHAR ucMaxFBMInFBG = 0,ucMaxDecFBCnt = 0;
    BOOL fgReCreateFBG = FALSE;
    INT32 i4Ret = E_VDEC_OK;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    FBM_CREATE_FBG_PAR_T rPar;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    ASSERT(prH265DecPrm!=NULL);
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    
    prH265DecPrm->u4PicW = prH265DecPrm->prSPS->u4PicWidthInLumaSamples;
    prH265DecPrm->u4PicH = prH265DecPrm->prSPS->u4PicHeightInLumaSamples;
    u4FbgWidth = ((prH265DecPrm->u4PicW + 63) >> 6) << 6;
    u4FbgHeight = ((prH265DecPrm->u4PicH + 63) >> 6) << 6;
    u4PrevWidth = ((prH265DecPrm->rLastInfo.u4LastPicWidthInLumaSamples + 63) >> 6) << 6;
    u4PrevHeight = ((prH265DecPrm->rLastInfo.u4LastPicHeightInLumaSamples + 63) >> 6) << 6;

    fgReCreateFBG = VDEC_ChkSeamlessModeChg(u4InstID,
        prH265DecPrm->prSPS->u4PicWidthInLumaSamples,
        prH265DecPrm->prSPS->u4PicHeightInLumaSamples);
    prH265DecPrm->fgSeamlessDisp = ((prVdecEsInfo->eSeamlessMode != SEAMLESS_NONE) && !(prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV));

    if((u4FbgWidth != u4PrevWidth) || (u4FbgHeight != u4PrevHeight))
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
    
    if((prH265DecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN) || fgReCreateFBG)
    {
        vVDecH265FlushDPB(u4InstID,prH265DecPrm);
        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
        rPar.u1VdecId = u4InstID;
        
        #ifdef ENABLE_MULTIMEDIA
        if (prVdecEsInfo->eSeamlessMode != SEAMLESS_NONE)
        {
            rPar.u4TargetW = prVdecEsInfo->u4SeamlessWidth;
            rPar.u4TargetH = prVdecEsInfo->u4SeamlessHeight;
        }
        
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS)// ??? first time if 0xFF
        {
            rPar.ucCftFbgId = prH265DecPrm->ucFbgId + 1;
        }
        else if (prH265DecPrm->ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            FBM_ReleaseGroup(prH265DecPrm->ucFbgId);
            prH265DecPrm->ucFbgId = FBM_FBG_TYPE_UNKNOWN;
            rPar.ucCftFbgId = 0;
        }
        rPar.eSeamlessMode = prVdecEsInfo->eSeamlessMode;
        
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS)
        {
            u4FbgWidth = prVdecEsInfo->u4SeamlessWidth;
            u4FbgHeight = prVdecEsInfo->u4SeamlessHeight;
        }        

        ucFbgType = FBM_SelectGroupType(u4FbgWidth, u4FbgHeight);
        
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
        else if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
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
            rPar.u1AppMode = FBM_FBG_APP_NORMAL;
        }

        if (prVdecEsInfo->u4RenderPicCnt)
        {
            rPar.fgThumbnailMode = TRUE;
        }  

        if(prH265DecPrm->fgIsUFOMode)
        {
            rPar.fgUFO = TRUE;
        }
        
        if(prH265DecPrm->ucMain10Mode == HEVC_COMPACT_MODE)
        {
            rPar.fg10Bit = TRUE;
            rPar.fgUFO = TRUE;//prH265DecPrm->fgIsUFOMode ? TRUE : prH265DecPrm->fgSeamlessDisp;
        }

        prH265DecPrm->ucFbgId = FBM_CreateGroupExt(ucFbgType,FBM_VDEC_H265, u4FbgWidth,u4FbgHeight, &rPar);
        if(prH265DecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            LOG(0,"[H265-ERR] Allocate FBG Fail\n");
            i4Ret = E_VDEC_FAIL;
            return i4Ret;
        }

        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265] Allocate FBG [%d,%d] to [%d,%d] PrevFbgID %d\n",u4PrevWidth,u4PrevHeight,u4FbgWidth,
                                                                        u4FbgHeight,prH265DecPrm->ucFbgId);
                                                                        
        prVdecEsInfo->ucFbgId = prH265DecPrm->ucFbgId;
        prH265DecPrm->ucSeqChgMode |= RESOLUTION_CHG;
        
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
                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, u4InstID);
            if(prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, u4InstID);
            }
        }
        
        if(prVdecEsInfo->fgMMPlayback==FALSE && prVdecInfo->pfPicInfo)
        {
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_DISPLAY_START, (UINT32)_Vdec_FrameDisplayStart,u4InstID);
        }
		//need confirm
        //u4MaxTLayersMinus1 0 ~ 6, u4MaxDecPicBuffering[i] > u4MaxDecPicBuffering[i - 1]
        ucMaxFBMInFBG = FBM_GetFrameBufferNs(prH265DecPrm->ucFbgId);  // 2 lock frame
        ucMaxDecFBCnt = FBM_GetFrameBufferMvNs(prH265DecPrm->ucFbgId); //total ref fbm count
        prH265DecPrm->ucMaxFBufNum = (ucMaxDecFBCnt > H265_MAX_FB_NUM) ? H265_MAX_FB_NUM : ucMaxDecFBCnt;
        if(prH265DecPrm->ucMaxFBufNum < (UCHAR)prH265DecPrm->prSPS->u4MaxDecPicBuffering[prH265DecPrm->prSPS->u4MaxTLayersMinus1] + 1)
        {
            LOG(0,"[H265-ERR-%d] Need FB (%d) > Pool Ref FB (%d) Total FB (%d)\n",u4InstID,(prH265DecPrm->prSPS->u4MaxDecPicBuffering[prH265DecPrm->prSPS->u4MaxTLayersMinus1] + 1),
                                                                prH265DecPrm->ucMaxFBufNum,ucMaxFBMInFBG);
            prH265DecPrm->fgH265NotSupport = TRUE;
            i4Ret = E_VDEC_FAIL;
        }
        //FHD0 -> SD ->FHD1, LAE Q will become 4 groups, and buffers are overlap in SCORE <-> MCORE change
        #ifdef ENABLE_MULTIMEDIA
        if(VDEC_IS_SEAMLESS(prVdecEsInfo->eMMSrcType) && IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_FORCE_SCORE))
        {
            vClearLaeInputMsgQ(prVdecEsInfo->ucLaeEsInfoId);
        }
        #endif
        //Init LAE buffer 
        if(prH265DecPrm->ucDualCoreMode)
        {
            prH265DecPrm->rLaeInfo.u4PicW = prH265DecPrm->prSPS->u4PicWidthInLumaSamples;
            prH265DecPrm->rLaeInfo.u4PicH = prH265DecPrm->prSPS->u4PicHeightInLumaSamples;
            if(fgLAEAllocateInfoBuf(u4InstID,VDEC_FMT_H265,&(prH265DecPrm->rLaeInfo)))
            {
                
            }
            else
            {
                
            }
        }

        
        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-TEST-%d] Need FB(%d) & Pool FB(%d)\n",u4InstID,prH265DecPrm->ucMaxFBufNum,ucMaxFBMInFBG);
        //Set SeqHdr info
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prH265DecPrm->ucFbgId);
        prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;

        //Those 2 flag were set in SetParam 
        prFbmSeqHdr->fgRRYEn = FALSE;
        prFbmSeqHdr->fgRRCEn = FALSE;
        
        if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                    ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) &&
                     (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
        {
            prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
            prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            prFbmSeqHdr->u2HSize = (UINT16)prH265DecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prH265DecPrm->u4PicH;
        }

        
        if(prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV)
        {
            prVdecEsInfo->u2LineSize = align64_hevc(prH265DecPrm->u4PicW);
            prFbmSeqHdr->u2LineSize = align64_hevc(prH265DecPrm->u4PicW);
            prFbmSeqHdr->u2OrgWidth  = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgHeight = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2OrgHSize = (UINT16)prH265DecPrm->u4PicW;
            prFbmSeqHdr->u2OrgVSize = (UINT16)prH265DecPrm->u4PicH;
            prFbmSeqHdr->u2HSize = (UINT16)prH265DecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prH265DecPrm->u4PicH;
            //temp solution
            prFbmSeqHdr->u4ResizeWidth = prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u4ResizeHeight = prVdecEsInfo->u4SeamlessHeight;
            LOG(2,"Seamless with NPTV OrgWidth/OrgHeight [%d,%d] Picth(%d)\n",prFbmSeqHdr->u2OrgWidth, 
                                                                                prFbmSeqHdr->u2OrgHeight, 
                                                                                prVdecEsInfo->u2LineSize);
        }
        else if (prVdecEsInfo->eSeamlessMode & (SEAMLESS_ONE_FBS | SEAMLESS_TWO_FBS))
        {
            prFbmSeqHdr->u2OrgWidth  = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgHeight = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2HSize = (prVdecEsInfoKeep->fgVPush) ? prH265DecPrm->u4PicW : (UINT16)prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u2VSize = (prVdecEsInfoKeep->fgVPush) ? prH265DecPrm->u4PicH : (UINT16)prVdecEsInfo->u4SeamlessHeight;
            prFbmSeqHdr->u2OrgHSize = (UINT16)prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u2OrgVSize = (UINT16)prVdecEsInfo->u4SeamlessHeight;
            prFbmSeqHdr->u2LineSize = prVdecEsInfo->u4SeamlessWidth;
            
            //temp solution
            prFbmSeqHdr->u4ResizeWidth = prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u4ResizeHeight = prVdecEsInfo->u4SeamlessHeight;
            
            if (0)//(prDecPrm->fgIsNewMode)
            {
                prVdecEsInfo->u2LineSize = align16_hevc(prH265DecPrm->u4PicW);
            }
            else
            {
                prVdecEsInfo->u2LineSize = align64_hevc(prH265DecPrm->u4PicW);
            }
            
            LOG(2,"Seamless with imgrz OrgWidth/OrgHeight [%d,%d] Picth(%d)\n",prFbmSeqHdr->u2OrgWidth, prFbmSeqHdr->u2OrgHeight, prVdecEsInfo->u2LineSize);
        }
        else
        {
            prFbmSeqHdr->u2LineSize = _MAKE_64X(prH265DecPrm->u4PicW);
            prFbmSeqHdr->u2OrgHSize = prH265DecPrm->u4PicW;
            prFbmSeqHdr->u2OrgVSize = prH265DecPrm->u4PicH;
        }
        
        if(prH265DecPrm->prSPS->bConformanceWindowFlag)
        {
            prFbmSeqHdr->fgCropping = prH265DecPrm->prSPS->bConformanceWindowFlag;
            prFbmSeqHdr->u4CropX = prH265DecPrm->prSPS->u4ConfWinLeftOffset * prH265DecPrm->prSPS->u4SubWidthC;
            prFbmSeqHdr->u4CropY = prH265DecPrm->prSPS->u4ConfWinTopOffset * prH265DecPrm->prSPS->u4SubHeightC;
            prFbmSeqHdr->u4CropWidth = (prH265DecPrm->u4PicW - prFbmSeqHdr->u4CropX) - 
                                        prH265DecPrm->prSPS->u4ConfWinRightOffset * prH265DecPrm->prSPS->u4SubWidthC;
            prFbmSeqHdr->u4CropHeight = (prH265DecPrm->u4PicH - prFbmSeqHdr->u4CropY) - 
                                        prH265DecPrm->prSPS->u4ConfWinBottomOffset * prH265DecPrm->prSPS->u4SubHeightC;

            VDecPrintf(u4InstID,VDEC_LOG_SEQINFO,"[H265-INFO-%d] Cropping info X/Y[%d,%d] CorpW/H[%d,%d]\n",u4InstID,prFbmSeqHdr->u4CropX,prFbmSeqHdr->u4CropY,
                                        prFbmSeqHdr->u4CropWidth,prFbmSeqHdr->u4CropHeight);
        }

        //Get frame rate
        prFbmSeqHdr->ucFrmRatCod = ucVDecH265GetFrameRate(u4InstID,prH265DecPrm);

        //update last info
        prH265DecPrm->rLastInfo.u4LastPicWidthInLumaSamples = prH265DecPrm->u4PicW;
        prH265DecPrm->rLastInfo.u4LastPicHeightInLumaSamples = prH265DecPrm->u4PicH;
        
        FBM_FbgChgNotify(prH265DecPrm->ucFbgId,u4InstID);
        prFbmSeqHdr->fgSeamlessPlay = prVdecEsInfo->fgSeamlessPlay;
        prFbmSeqHdr->eSeamlessMode = prVdecEsInfo->eSeamlessMode;
    }

    return i4Ret;
}

//Should 
UCHAR ucVDecH265AllocateFBInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{   
    INT32 i4Cnt;
    INT32 i4MinPOC = 0x7FFFFFFF;
    UINT32 u4MinPOCFBufIdx = FBUF_ID_UNKNOWN;
    ASSERT(prH265DecPrm!=NULL);

//    if( (prH265DecPrm->prCurrFBufInfo == NULL) ||
//        (prH265DecPrm->prCurrFBufInfo->ucFBufStatus == FRAME) ||
//        (prH265DecPrm->prSliceHdr->i4POC != prH265DecPrm->rLastInfo.i4LastPOC))
    {
        for(i4Cnt = 0; i4Cnt < prH265DecPrm->ucMaxFBufNum; i4Cnt++)
        {
            if(prH265DecPrm->prFBufInfo[i4Cnt].ucFBufStatus == NO_PIC)
            {
                i4MinPOC = 0x7fffffff;
                u4MinPOCFBufIdx = i4Cnt;
                break;
            }
            else if(i4MinPOC > prH265DecPrm->prFBufInfo[i4Cnt].i4POC && fgVDecH265IsNonRefFBuf(u4InstID,i4Cnt,prH265DecPrm))
            {
                i4MinPOC = prH265DecPrm->prFBufInfo[i4Cnt].i4POC;
                u4MinPOCFBufIdx = i4Cnt;
            }
        }

        // No empty DPB, 1 FBuf output
        if((u4MinPOCFBufIdx != FBUF_ID_UNKNOWN) && prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFBufStatus != NO_PIC)
        {
            VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-WARNING] Force Clear FB[%d][%d] POC %d for Current Pic\n",u4MinPOCFBufIdx,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFBufStatus,
                                                prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].i4POC);
            vVDecH265ClrFBufInfo(u4InstID,u4MinPOCFBufIdx,prH265DecPrm);
            if((FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId) == FBM_FB_STATUS_READY)
                || (FBM_GetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId) == FBM_FB_STATUS_DECODE))
            {
                FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId,FBM_FB_STATUS_EMPTY);
                prH265DecPrm->i4ReadyOutCntDPB --;
            }
        }
        
    }

    return u4MinPOCFBufIdx;
}

//void vVDecH265SetCurrFBInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
void vVDecH265SetCurrFBInfo(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    UINT32 u4YAddr = 0,u4CAddr = 0,u4MVAddr = 0;
    UINT32 u4YLenAddr = 0,u4CLenAddr = 0;
    UCHAR u4MVBufIdx = 0xFF;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T * prFbmPicHdr;
    
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    prH265DecPrm->prCurrFBufInfo = &(prH265DecPrm->prFBufInfo[prH265DecPrm->ucDecFBufId]);
    prH265DecPrm->prCurrFBufInfo->eH265DpbStatus = H265_DPB_STATUS_READY;
    prH265DecPrm->prCurrFBufInfo->ucFBufStatus = FRAME;
    prH265DecPrm->prCurrFBufInfo->i4POC = prH265DecPrm->prSliceHdr->i4POC;
    prH265DecPrm->prCurrFBufInfo->ucFbmFbId = prH265DecPrm->ucFbId;
    
    //Get Y/Cbcr address
    FBM_GetFrameBufferAddr(prH265DecPrm->ucFbgId, prH265DecPrm->ucFbId,&u4YAddr, &u4CAddr);
    prH265DecPrm->prCurrFBufInfo->u4YStartAddr = u4YAddr;
    prH265DecPrm->prCurrFBufInfo->u4CStartAddr = u4CAddr;

    //Get Y_Len/C_Len address
    if(prH265DecPrm->fgIsUFOMode)
    {
        FBM_GetFrameBufferAddrExt(prH265DecPrm->ucFbgId, prH265DecPrm->ucFbId,&u4YLenAddr,&u4CLenAddr);
        ASSERT(u4YLenAddr != 0); //FBM porting FBM_GetFrameBufferAddrExt later
        prH265DecPrm->prCurrFBufInfo->u4YLenStartAddr = u4YLenAddr;
        prH265DecPrm->prCurrFBufInfo->u4CLenStartAddr = u4CLenAddr;
        
        //prH265DecPrm->prCurrFBufInfo->u4DramYSize = ((prH265DecPrm->prSPS->u4PicWidthInLumaSamples * prH265DecPrm->prSPS->u4PicHeightInLumaSamples + 1023) >> 10) << 10;
        prH265DecPrm->prCurrFBufInfo->u4DramYSize = u4YLenAddr - u4YAddr;
        prH265DecPrm->prCurrFBufInfo->u4DramCSize = u4CLenAddr - u4CAddr;
        prH265DecPrm->prCurrFBufInfo->bIsUFOEncoded = TRUE;
        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265%d] UFO mode bit%d YCLen[0x%x,0x%x] size 0x%x\n",u4InstID,prH265DecPrm->ucMain10Mode,
                    prH265DecPrm->prCurrFBufInfo->u4YLenStartAddr,prH265DecPrm->prCurrFBufInfo->u4CLenStartAddr,prH265DecPrm->prCurrFBufInfo->u4DramYSize);
    }
    
    //Get MV address.MV Buffer ID == Frame Buffer ID
//    u4MVBufIdx = ucVDecH265GetNonRefFBuf(u4InstID,prH265DecPrm);
    u4MVBufIdx = prH265DecPrm->ucDecFBufId;
    ASSERT(u4MVBufIdx != FBUF_ID_UNKNOWN);
    FBM_GetFrameBufferAddrMv(prH265DecPrm->ucFbgId,u4MVBufIdx,&u4MVAddr);
    prH265DecPrm->prCurrFBufInfo->u4MvStartAddr = u4MVAddr;
    prH265DecPrm->prCurrFBufInfo->fgMVUsed = TRUE;
    
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId,prH265DecPrm->ucFbId);
    prFbmPicHdr->u4PTS = prH265DrvDecInfo->rPesInfo.u4PTS;    
    prFbmPicHdr->u8PTS = prH265DrvDecInfo->rPesInfo.u8PTS;
    prFbmPicHdr->u8Offset = prH265DrvDecInfo->rPesInfo.u8Offset;
    prFbmPicHdr->u8OffsetI = prH265DrvDecInfo->rPesInfo.u8OffsetI;
    prFbmPicHdr->u8OffsetDisp = prH265DrvDecInfo->rPesInfo.u8Offset;	
    prFbmPicHdr->i4TemporalRef = prH265DecPrm->prCurrFBufInfo->i4POC;
    prFbmPicHdr->fgUfoEn = prH265DecPrm->fgIsUFOMode;

    prFbmPicHdr->u4PicWidth = prH265DecPrm->u4PicW;
    prFbmPicHdr->u4PicHeight = prH265DecPrm->u4PicH;
    prFbmPicHdr->u4PicWidthPitch = align64_hevc(prH265DecPrm->u4PicW);
    prFbmPicHdr->fgSeamlessDisp = prH265DecPrm->fgSeamlessDisp;
    prFbmPicHdr->ucSrcDepth = prH265DecPrm->prSPS->u4BitDepthLumaMinus8 + 8;
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prH265DecPrm->ucFbgId);
    if(prFbmSeqHdr)
    {
        prFbmPicHdr->fgCropping = prFbmSeqHdr->fgCropping;
        prFbmPicHdr->u4CropX = prFbmSeqHdr->u4CropX;
        prFbmPicHdr->u4CropY = prFbmSeqHdr->u4CropY;
        prFbmPicHdr->u4CropWidth = prFbmSeqHdr->u4CropWidth;
        prFbmPicHdr->u4CropHeight = prFbmSeqHdr->u4CropHeight;
    }
    
    prH265DecPrm->prCurrFBufInfo->u4DebugPTS = prH265DrvDecInfo->rPesInfo.u4PTS;
    VDecPrintf(u4InstID,VDEC_LOG_FBMST,"[H265-%d] CurFb POC (%d) DecID %d fbg/mID (%d,%d) Y/C/MV addr (0x%x,0x%x,0x%x)\n",u4InstID,prH265DecPrm->prCurrFBufInfo->i4POC,
        prH265DecPrm->ucDecFBufId,prH265DecPrm->ucFbgId,prH265DecPrm->ucFbId,u4YAddr,u4CAddr,u4MVAddr);
}


UCHAR ucVDecH265GetFrameRate(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UCHAR ucFrameRate = MPEG_FRAME_RATE_UNKNOWN;
    UCHAR ucFrameRateVDEC = MPEG_FRAME_RATE_UNKNOWN,ucFrameRateContainer = MPEG_FRAME_RATE_UNKNOWN;
    UINT32 u4fps = 0;
    H265_SPS_Data *prH265_SPS_DataInst;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;

    ASSERT(prH265DecPrm!=NULL);
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    
    if(prH265_SPS_DataInst->bVuiParametersPresentFlag)
    {
        if (prH265_SPS_DataInst->rVUI.bTimingInfoPresentFlag && 
            (prH265_SPS_DataInst->rVUI.u4NumUnitsInTick > 0))
        {
            u4fps = (prH265_SPS_DataInst->rVUI.u4TimeScale / prH265_SPS_DataInst->rVUI.u4NumUnitsInTick);
        }

        switch (u4fps)
        {
            case 12:
               ucFrameRateVDEC = MPEG_FRAME_RATE_12;
               break;
            case 15:
               ucFrameRateVDEC = MPEG_FRAME_RATE_15;
               break;
            case 20:
                ucFrameRateVDEC = MPEG_FRAME_RATE_20;
                break;
            case 24: 
                ucFrameRateVDEC = MPEG_FRAME_RATE_24;
                break;
            case 23:
                ucFrameRateVDEC = MPEG_FRAME_RATE_24_;
                break;
            case 25:
                ucFrameRateVDEC = MPEG_FRAME_RATE_25;
                break;
            case 30:
                ucFrameRateVDEC = MPEG_FRAME_RATE_30;
                break;
            case 29:
                ucFrameRateVDEC = MPEG_FRAME_RATE_30_;
                break;
            case 47:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {  
                    ucFrameRateVDEC = MPEG_FRAME_RATE_24_;
                }
                else
                {
                    LOG(1,"Framerate warning .....\n");
                }
                break;
            case 50:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {
                    ucFrameRateVDEC = MPEG_FRAME_RATE_25;
                }
                else
                {
                    ucFrameRateVDEC = MPEG_FRAME_RATE_50;
                }
                break;
            case 60:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {   
                    ucFrameRateVDEC = MPEG_FRAME_RATE_30;
                }
                else
                {
                    ucFrameRateVDEC = MPEG_FRAME_RATE_60;
                }
                break;
            case 59:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {  
                    ucFrameRateVDEC = MPEG_FRAME_RATE_30_;
                }
                else
                {
                    ucFrameRateVDEC = MPEG_FRAME_RATE_60_;
                }
            
                break;
            default:
                ucFrameRateVDEC = MPEG_FRAME_RATE_UNKNOWN;
                break;
        }
    }
    else
    {
       ucFrameRateVDEC = MPEG_FRAME_RATE_UNKNOWN; 
    }

    if(prVdecEsInfo->rMMSeqInfo.e_frame_rate != VDEC_SRC_FRAME_RATE_UNKNOWN)
    {
        switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
        {
            case VDEC_SRC_FRAME_RATE_12:
               ucFrameRateContainer = MPEG_FRAME_RATE_12;
               break;
            case VDEC_SRC_FRAME_RATE_15:
               ucFrameRateContainer = MPEG_FRAME_RATE_15;
               break;
            case VDEC_SRC_FRAME_RATE_20:
                ucFrameRateContainer = MPEG_FRAME_RATE_20;
                break;
            case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                ucFrameRateContainer = MPEG_FRAME_RATE_24_;
                break;
            case VDEC_SRC_FRAME_RATE_24:
                ucFrameRateContainer = MPEG_FRAME_RATE_24;
                break;
            case VDEC_SRC_FRAME_RATE_25:
                ucFrameRateContainer = MPEG_FRAME_RATE_25;
                break;
            case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                ucFrameRateContainer = MPEG_FRAME_RATE_30_;
                break;
            case VDEC_SRC_FRAME_RATE_30:
                ucFrameRateContainer = MPEG_FRAME_RATE_30;
                break;
            case VDEC_SRC_FRAME_RATE_50:
                ucFrameRateContainer = MPEG_FRAME_RATE_50;
                break;
            case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                ucFrameRateContainer = MPEG_FRAME_RATE_60_;
                break;
            case VDEC_SRC_FRAME_RATE_60:
                ucFrameRateContainer = MPEG_FRAME_RATE_60;
                break;
            default:
                ucFrameRateContainer = MPEG_FRAME_RATE_UNKNOWN;
                break;
        }
    }

    if(ucFrameRateVDEC != MPEG_FRAME_RATE_UNKNOWN)
    {
        ucFrameRate = ucFrameRateVDEC;
    }
    else if((ucFrameRateVDEC == MPEG_FRAME_RATE_UNKNOWN) && (ucFrameRateContainer != MPEG_FRAME_RATE_UNKNOWN))
    {
        ucFrameRate = ucFrameRateContainer;
    }
    else
    {
        ucFrameRate = MPEG_FRAME_RATE_UNKNOWN;
    }

    return ucFrameRate;
}


UCHAR ucVDecH265GetAspInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm,UINT32 *u4AspW,UINT32 *u4AspH,UINT32 *u4SAR)
{
    UCHAR ucOrgAsp;
    H265_SPS_Data *prH265_SPS_DataInst;
    
    ASSERT(prH265DecPrm!=NULL);
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    ASSERT(prH265_SPS_DataInst != NULL);
    
    switch (prH265_SPS_DataInst->rVUI.i4AspectRatioIdc)
    {
        case 1:
            *u4SAR = 100;
            *u4AspW = 1;
            *u4AspH= 1;
            ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
            break;
        case 2:
            *u4SAR = 1200 / 11;
            *u4AspW = 12;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_12_11;
            break;
        case 3:
            *u4SAR = 1000 / 11;
            *u4AspW = 10;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_10_11;
            break;
        case 4:
            *u4SAR = 1600 / 11;
            *u4AspW = 16;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_16_11;
            break;
        case 5:
            *u4SAR = 4000 / 33;
            *u4AspW = 40;
            *u4AspH = 33;
            ucOrgAsp = VDEC_ASPECT_RATIO_40_33;
            break;
        case 6:
            *u4SAR = 2400 / 11;
            *u4AspW = 24;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_24_11;
            break;
        case 7:
            *u4SAR = 2000 / 11;
            *u4AspW = 20;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_20_11;
            break;
        case 8:
            *u4SAR = 3200 / 11;
            *u4AspW = 32;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_32_11;
            break;
        case 9:
            *u4SAR = 8000 / 33;
            *u4AspW = 80;
            *u4AspH = 33;
            ucOrgAsp = VDEC_ASPECT_RATIO_80_33;
            break;
        case 10:
            *u4SAR = 1800 / 11;
            *u4AspW = 18;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_18_11;
            break;
        case 11:
            *u4SAR = 1500 / 11;
            *u4AspW = 15;
            *u4AspH = 11;
            ucOrgAsp = VDEC_ASPECT_RATIO_15_11;
            break;
        case 12:
            *u4SAR = 6400 / 33;
            *u4AspW = 64;
            *u4AspH = 33;
            ucOrgAsp = VDEC_ASPECT_RATIO_64_33;
            break;
        case 13:
            *u4SAR = 16000 / 99;
            *u4AspW = 160;
            *u4AspH = 99;
            ucOrgAsp = VDEC_ASPECT_RATIO_160_99;
            break;
        case 14:
            *u4SAR = 400 / 3;
            *u4AspW = 4;
            *u4AspH = 3;
            ucOrgAsp = VDEC_ASPECT_RATIO_4_3;
            break;
        case 15:
            *u4SAR = 300 / 2;
            *u4AspW = 3;
            *u4AspH = 2;
            ucOrgAsp = VDEC_ASPECT_RATIO_3_2;
            break;
        case 16:
            *u4SAR = 200;
            *u4AspW = 2;
            *u4AspH = 1;
            ucOrgAsp = VDEC_ASPECT_RATIO_2_1;
            break;
        default:
            *u4SAR = 1200 / 11;
            *u4AspW = 1;
            *u4AspH = 1;
            ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
    }
    return ucOrgAsp;
}

INT32 i4VDecH265PrepareRefList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i = 0,i4LostPOC;
    UINT32 u4NalType;
    INT32 i4Ret = E_VDEC_OK;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;

    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    ASSERT(prH265_Slice_Hdr_DataInst != NULL);
    u4NalType = prH265DecPrm->u4NalType;

    if(prH265DecPrm->i4RAPOC == MAX_POC)
    {
        if (u4NalType == NAL_UNIT_CODED_SLICE_CRA
        || u4NalType == NAL_UNIT_CODED_SLICE_BLANT
        || u4NalType == NAL_UNIT_CODED_SLICE_BLA_N_LP
        || u4NalType == NAL_UNIT_CODED_SLICE_BLA )
        {
            prH265DecPrm->i4RAPOC = prH265_Slice_Hdr_DataInst->i4POC;
        }
        else if((u4NalType == NAL_UNIT_CODED_SLICE_IDR) || (u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP))
        {
            prH265DecPrm->i4RAPOC = - MAX_POC;
        }
        else
        {
            //
        }
    }

    while((i4LostPOC = i4VDecH265ChkRefPicsAreAvailable(u4InstID, prH265DecPrm)) > 0)
    {
        vVDecH265CreateLostPic(u4InstID,(i4LostPOC - 1),prH265DecPrm);
        LOG(2,"[H265-TEST-%d] Create Lost POC[%d] %d\n",u4InstID,i,i4LostPOC);
        if(i ++ > 32)
        {
            break;
        }
    }

    //error handle for fram gap.
    //RASL must be skipped which associate with CRA.
    if((prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_CRA) && (i4LostPOC == - MAX_POC))
    {
        prH265DecPrm->fgNoRaslOutput = TRUE;
        vVDecH265ClrDPB(u4InstID,prH265DecPrm);
        prH265DecPrm->i4ReadyOutCntDPB = 0;
        LOG(2,"[H265-TEST-%d] Create Lost POC[%d] %d for CRA\n",u4InstID,i,i4LostPOC);
    }

    vVDecH265ApplyRefPicList(u4InstID, prH265DecPrm);
    u4VDecH265SetRefPicList(u4InstID, prH265DecPrm);

    if ( u4NalType == NAL_UNIT_CODED_SLICE_BLA
        || u4NalType == NAL_UNIT_CODED_SLICE_BLANT
        || u4NalType == NAL_UNIT_CODED_SLICE_BLA_N_LP
        || u4NalType == NAL_UNIT_CODED_SLICE_IDR
        || u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP )  // IDR or BLA picture
    {
        VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"vVDecH265FlushDPB NalType %d\n",u4NalType);
        vVDecH265FlushDPB(u4InstID, prH265DecPrm);
        prH265DecPrm->rLastInfo.i4LastPOC = -MAX_POC;
    }
    
    return i4Ret;
}


INT32 i4VDecH265ChkRefPicsAreAvailable(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    BOOL fgIsAvailable = FALSE,fgAtLeastOneLost = FALSE,fgAtLeastOneMoved = FALSE;
    INT32 i,j;
    INT32 i4LostPOC;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;
    H265_RPS_Data *pCurrRPS = NULL;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;
    
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    pCurrRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    
    i = 0;
    // loop through all long-term pictures in the Reference Picture Set
    // to see if the picture should be kept as reference picture
    for( i = pCurrRPS->u4NumberOfNegativePictures + pCurrRPS->u4NumberOfPositivePictures; i < pCurrRPS->u4NumberOfPictures; i++)
    {
        if (i >= MAX_NUM_REF_PICS)
        {
            LOG(0,"[H265-ERR] Short RPS Count(%d) overflow\n",i);
            break;
        }

        for(j = 0; j < prH265DecPrm->ucMaxFBufNum; j++)
        {
            if(prTempFBufInfo[j].ucFBufStatus == NO_PIC)
            {
                continue;
            }
            
            if(pCurrRPS->bCheckLTMSB[i])
            {
                if(prTempFBufInfo[j].bUsedAsLongTerm && (prTempFBufInfo[j].i4POC == pCurrRPS->i4POC[i]))
                {
                    fgIsAvailable = TRUE;
                }
            }
            else
            {
                if((prTempFBufInfo[j].bUsedAsLongTerm) &&
                   ((prTempFBufInfo[j].i4POC%(1 << prTempFBufInfo[j].u4POCBits)) == (pCurrRPS->i4POC[i]%(1 << prTempFBufInfo[j].u4POCBits))))
                {
                    fgIsAvailable = TRUE;
                }
            }
        }

        if(!fgIsAvailable)
        {
            for(j = 0; j < prH265DecPrm->ucMaxFBufNum; j++)
            {
                if(prTempFBufInfo[j].ucFBufStatus == NO_PIC)
                {
                    continue;
                }

                if(pCurrRPS->bCheckLTMSB[i])
                {
                    if((prTempFBufInfo[j].i4POC == pCurrRPS->i4POC[i]) && (prTempFBufInfo[j].bFirstSliceReferenced))
                    {
                        fgIsAvailable = TRUE;
                        prTempFBufInfo[j].ucFBufRefType = LREF_PIC;
                        prTempFBufInfo[j].bUsedAsLongTerm = 1;
                        break;
                    }
                }
                else
                {
                    if( (prTempFBufInfo[j].bFirstSliceReferenced) &&
                        ((prTempFBufInfo[j].i4POC%(1 << prTempFBufInfo[j].u4POCBits)) == (pCurrRPS->i4POC[i]%(1 << prTempFBufInfo[j].u4POCBits))))
                    {
                        fgIsAvailable = TRUE;
                        prTempFBufInfo[j].ucFBufRefType = LREF_PIC;
                        prTempFBufInfo[j].bUsedAsLongTerm = 1;
                        break;
                    }
                }
            }
        }
        
        if(!fgIsAvailable)
        {
            if((prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] >= prH265DecPrm->i4RAPOC))
            {
                if(!pCurrRPS->bUsed[i])
                {
                    LOG(2,"[H265-WARNING] Long-term reference picture with POC = %3d seems to have been removed or not correctly decoded!!\n\n",  prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] );
                    fgAtLeastOneMoved = TRUE;
                }
                else
                {
                    LOG(2,"[H265-WARNING] Long-term reference picture with POC = %3d is lost or not correctly decoded!!\n\n",  prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] );
                    fgAtLeastOneLost= TRUE;
                    i4LostPOC = prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i];
                }
            }
        }
    }


    // loop through all short-term pictures in the Reference Picture Set
    // to see if the picture should be kept as reference picture
    for( i = 0; i < pCurrRPS->u4NumberOfNegativePictures + pCurrRPS->u4NumberOfPositivePictures; i++)
    {
        if(i >= MAX_NUM_REF_PICS)
        {
            break;
        }
        
        fgIsAvailable = FALSE;
        for(j = 0; j < prH265DecPrm->ucMaxFBufNum; j++)
        {
            if(prTempFBufInfo[j].ucFBufStatus == NO_PIC)
            {
                continue;
            }
            
            if( (!(prTempFBufInfo[j].bUsedAsLongTerm)) && 
                (prTempFBufInfo[j].i4POC == (prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i])) &&
                (prTempFBufInfo[j].bFirstSliceReferenced) )
            {
                fgIsAvailable = TRUE;
            }
            
            VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-TEST-%d] FBuf[%d] fgLt %d,POC %d,fgRefType %d,SlicePOC&RpsPOC[%d,%d]\n",u4InstID,
                    j,prTempFBufInfo[j].bUsedAsLongTerm,prTempFBufInfo[j].i4POC,prTempFBufInfo[j].ucFBufRefType,
                    prH265_Slice_Hdr_DataInst->i4POC,pCurrRPS->i4DeltaPOC[i]);
        }

        if(!fgIsAvailable)
        {
            if( prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] >= prH265DecPrm->i4RAPOC )
            {
                if (!pCurrRPS->bUsed[i])
                {
                    LOG(2,"[H265-WARNING-%d] Short-term reference picture with POC = %3d seems to have been removed or not correctly decoded!!\n\n",u4InstID,prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] );
                    fgAtLeastOneMoved = TRUE;
                }
                else
                {
                    LOG(2,"[H265-WARNING-%d] Short-term reference picture with POC = %3d is lost or not correctly decoded!!\n\n",u4InstID,prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i] );
                    fgAtLeastOneLost = TRUE;
                    i4LostPOC = prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i];
                }
            }
        }
    }

    if(fgAtLeastOneLost)
    {
        LOG(2,"[H265-WARNING] i4VDecH265ChkRefPicsAreAvailable() return lost iPocLost %d!!\n", i4LostPOC);
        return (i4LostPOC + 1);
    }
    
    if(fgAtLeastOneMoved) 
    {
        return -MAX_POC;
    }
    else
    {
        return E_VDEC_OK;
    }
}


void vVDecH265CreateLostPic(UINT32 u4InstID,INT32 i4LostPOC,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i,i4ClosestPOC = MAX_POC;
    UCHAR ucClosetFBufIdx = FBUF_ID_UNKNOWN,ucReplaceLostFBufIdx = FBUF_ID_UNKNOWN;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    ASSERT(prH265_Slice_Hdr_DataInst != NULL);
    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    ASSERT(prTempFBufInfo != NULL);
    
    for(i = 0; i < prH265DecPrm->ucMaxFBufNum; i++)
    {
        if((abs_hevc(prTempFBufInfo[i].i4POC - i4LostPOC) < i4ClosestPOC) &&
           (prTempFBufInfo[i].i4POC != i4LostPOC) && (prTempFBufInfo[i].i4POC != prH265DecPrm->prSliceHdr->i4POC))
        {
            i4ClosestPOC = abs_hevc(prTempFBufInfo[i].i4POC - i4LostPOC);
            ucClosetFBufIdx = i;
        }
    }

    ucReplaceLostFBufIdx = ucVDecH265AllocateFBInfo(u4InstID,prH265DecPrm);
    if(ucReplaceLostFBufIdx != FBUF_ID_UNKNOWN)
    {
        //prTempFBufInfo[ucReplaceLostFBufIdx]
        x_memcpy(&(prTempFBufInfo[ucReplaceLostFBufIdx]),&(prTempFBufInfo[ucClosetFBufIdx]),sizeof(VDEC_INFO_H265_FBUF_INFO_T));
        prTempFBufInfo[ucReplaceLostFBufIdx].i4POC = i4LostPOC;
        prTempFBufInfo[ucReplaceLostFBufIdx].ucFBufRefType = SREF_PIC;
        prTempFBufInfo[ucReplaceLostFBufIdx].ucFBufStatus = FRAME;
        prTempFBufInfo[ucReplaceLostFBufIdx].eH265DpbStatus = H265_DPB_STATUS_OUTPUTTED;
        prTempFBufInfo[ucReplaceLostFBufIdx].bFirstSliceReferenced = 1;
        prTempFBufInfo[ucReplaceLostFBufIdx].bUsedAsLongTerm = 0;
        if(prH265DecPrm->i4RAPOC == MAX_POC)
        {
            prH265DecPrm->i4RAPOC = i4LostPOC;
        }
    }
    else
    {
        LOG(2,"[H265-WARNING] Search Copy of Lost POC FBuf Fail\n");
    }
}

void vVDecH265ApplyRefPicList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i,j = 0;
    BOOL fgIsRef = FALSE;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;
    H265_RPS_Data *pCurrRPS = NULL;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;

    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    pCurrRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
    prTempFBufInfo = prH265DecPrm->prFBufInfo;

    for(j = 0; j < prH265DecPrm->ucMaxFBufNum; j++)
    {
        if(!prTempFBufInfo[j].bFirstSliceReferenced)
        {
            continue;
        }
        fgIsRef = FALSE;
        for(i = 0; i < pCurrRPS->u4NumberOfNegativePictures + pCurrRPS->u4NumberOfPositivePictures; i++)
        {
            if(i > MAX_NUM_REF_PICS)
            {
                break;
            }
            
            //if( (!(prTempFBufInfo[j].ucFBufRefType == LREF_PIC)) && prTempFBufInfo[j].i4POC == (prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i]) )
            if( (!(prTempFBufInfo[j].bUsedAsLongTerm)) && prTempFBufInfo[j].i4POC == (prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i]) )
            {
                fgIsRef = TRUE;
                prTempFBufInfo[j].bUsedByCurr = pCurrRPS->bUsed[i];
                prTempFBufInfo[j].bUsedAsLongTerm = 0;
                //Set FBM ref property
                FBM_UpdateReferenceList(prH265DecPrm->ucFbgId,prTempFBufInfo[j].ucFbmFbId,TRUE);
            }
        }

        for(; i < pCurrRPS->u4NumberOfPictures; i++)
        {
            if (i >= MAX_NUM_REF_PICS)
            {
                break;
            }
            
            if( 1 == pCurrRPS->bCheckLTMSB[i] )
            {
                //if((prTempFBufInfo[j].ucFBufRefType == LREF_PIC) && prTempFBufInfo[j].i4POC == pCurrRPS->i4POC[i] )
                if((prTempFBufInfo[j].bUsedAsLongTerm) && prTempFBufInfo[j].i4POC == pCurrRPS->i4POC[i] )
                {
                    fgIsRef = TRUE;
                    prTempFBufInfo[j].bUsedByCurr = pCurrRPS->bUsed[i];
                    prH265DecPrm->prPicInfo->i4DpbLTBuffId[prH265DecPrm->prPicInfo->i4DpbLTBuffCnt] = j;
                    prH265DecPrm->prPicInfo->i4DpbLTBuffCnt ++;
                    //Set FBM ref property
                    FBM_UpdateReferenceList(prH265DecPrm->ucFbgId,prTempFBufInfo[j].ucFbmFbId,TRUE);
                }
            }
            else
            {
                //if( (prTempFBufInfo[j].ucFBufRefType == LREF_PIC) && 
                if( (prTempFBufInfo[j].bUsedAsLongTerm) && 
                    (prTempFBufInfo[j].i4POC%(1 << prTempFBufInfo[j].u4POCBits) == pCurrRPS->i4POC[i]%(1 << prTempFBufInfo[j].u4POCBits)))
                {
                    fgIsRef = TRUE;
                    prTempFBufInfo[j].bUsedByCurr = pCurrRPS->bUsed[i];
                    prH265DecPrm->prPicInfo->i4DpbLTBuffId[prH265DecPrm->prPicInfo->i4DpbLTBuffCnt] = j;
                    prH265DecPrm->prPicInfo->i4DpbLTBuffCnt ++;
                    //Set FBM ref property
                    FBM_UpdateReferenceList(prH265DecPrm->ucFbgId,prTempFBufInfo[j].ucFbmFbId,TRUE);
                }
            }
        }

        // mark the picture as "unused for reference" if it is not in
        // the Reference Picture Set
        VDecPrintf(u4InstID,VDEC_LOG_DECREF,"[H265-INFO] vVDecH265ApplyRefPicList idx[%d] POC[%d,%d] Status %d,fgIsRef %d\n",j,prTempFBufInfo[j].i4POC,prH265_Slice_Hdr_DataInst->i4POC,
                    prTempFBufInfo[j].ucFBufStatus,fgIsRef);
        if( prTempFBufInfo[j].i4POC != prH265_Slice_Hdr_DataInst->i4POC && 
            prTempFBufInfo[j].ucFBufStatus != NO_PIC && fgIsRef == 0)    
        { 

            if(prTempFBufInfo[j].bUsedAsLongTerm)
            {
                prH265DecPrm->prPicInfo->i4DpbLTBuffId[prH265DecPrm->prPicInfo->i4DpbLTBuffCnt] = j;
                prH265DecPrm->prPicInfo->i4DpbLTBuffCnt ++;
            }

            //If this fbm is outputted clrfb directly
            if(prTempFBufInfo[j].eH265DpbStatus == H265_DPB_STATUS_OUTPUTTED)
            {
                vVDecH265ClrFBufInfo(u4InstID, j ,prH265DecPrm);
            }
            else
            {
                prTempFBufInfo[j].ucFBufRefType = NREF_PIC;
            }
            
            //Clear FBM ref property
            FBM_UpdateReferenceList(prH265DecPrm->ucFbgId,prTempFBufInfo[j].ucFbmFbId,FALSE);
        }
    }
}

UINT32 u4VDecH265SetRefPicList(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i4NumRefIdx[3],i,j;
    INT32 i4RefPicPOC = MAX_POC,i4POCDiff,i4CurrPOC;
    UCHAR ucRefPicFbufIdx = FBUF_ID_UNKNOWN;
    UINT32  RefPicSetStCurr0[16],RefPicSetStCurr1[16];
    UINT32  RefPicSetLtCurr[16];
    UINT32   rpsCurrList0[H265_MAX_PIC_LIST_NUM+1];
    UINT32   rpsCurrList1[H265_MAX_PIC_LIST_NUM+1];
    UINT32  NumPocStCurr0 = 0,NumPocStCurr1 = 0,NumPocLtCurr = 0;
    UINT32  u4TotalRefNum = 0,numPocTotalCurr;
    INT32 i4RefIdx = 0;
    BOOL bLowDelay = TRUE;
    
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;
    H265_RPS_Data *pCurrRPS = NULL;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;

    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    pCurrRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    
    i4NumRefIdx[0] = prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0];
    i4NumRefIdx[1] = prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1];

    u4TotalRefNum = pCurrRPS->u4NumberOfNegativePictures + pCurrRPS->u4NumberOfPositivePictures 
                    + pCurrRPS->u4NumberOfLongtermPictures;
    if(u4TotalRefNum > prH265DecPrm->ucMaxFBufNum)
    {
        VDecPrintf(u4InstID,VDEC_LOG_FBMST,"H265-ERR] Allocate FB Count less than activly need\n");
    }

    for(i=0; i < pCurrRPS->u4NumberOfNegativePictures; i++)
    {
        if(prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->bUsed[i])
        {
            i4RefPicPOC = prH265_Slice_Hdr_DataInst->i4POC + pCurrRPS->i4DeltaPOC[i];
            ucRefPicFbufIdx = ucVDecH265GetSTRefPicFBufIdByPOC(u4InstID, i4RefPicPOC,prH265DecPrm);
            if ( ucRefPicFbufIdx == FBUF_ID_UNKNOWN) 
            {
                continue;
            }
            prTempFBufInfo[ucRefPicFbufIdx].ucFBufRefType = SREF_PIC;
            prTempFBufInfo[ucRefPicFbufIdx].bUsedAsLongTerm = 0;
            RefPicSetStCurr0[NumPocStCurr0] = ucRefPicFbufIdx;
            prTempFBufInfo[ucRefPicFbufIdx].bLtMsbPresentFlag = 0;  
            NumPocStCurr0++;
        }
    }

    for(; i < pCurrRPS->u4NumberOfNegativePictures + pCurrRPS->u4NumberOfPositivePictures; i++)
    {
        if(prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->bUsed[i])
        {
            i4RefPicPOC = prH265_Slice_Hdr_DataInst->i4POC+pCurrRPS->i4DeltaPOC[i];
            ucRefPicFbufIdx =  ucVDecH265GetSTRefPicFBufIdByPOC(u4InstID, i4RefPicPOC, prH265DecPrm);
            if ( ucRefPicFbufIdx == FBUF_ID_UNKNOWN)
            {
                continue;
            }
            prTempFBufInfo[ucRefPicFbufIdx].ucFBufRefType = SREF_PIC;
            prTempFBufInfo[ucRefPicFbufIdx].bUsedAsLongTerm = 0;
            RefPicSetStCurr1[NumPocStCurr1] = ucRefPicFbufIdx;
            prTempFBufInfo[ucRefPicFbufIdx].bLtMsbPresentFlag = 0;  
            NumPocStCurr1++;
        }
    }

    
    if(u4TotalRefNum > 0)
    {
        for(i = u4TotalRefNum - 1; i >= (INT32)(pCurrRPS->u4NumberOfNegativePictures+pCurrRPS->u4NumberOfPositivePictures); i--)
        {
            if(prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->bUsed[i])
            {
                ucRefPicFbufIdx = ucVDecH265GetLTRefPicFBufIdByPOC(u4InstID,pCurrRPS->i4POC[i],pCurrRPS->bCheckLTMSB[i],prH265DecPrm);
                prTempFBufInfo[ucRefPicFbufIdx].ucFBufRefType = LREF_PIC;
                prTempFBufInfo[ucRefPicFbufIdx].bUsedAsLongTerm = 1;
                RefPicSetLtCurr[NumPocLtCurr] = ucRefPicFbufIdx;
                NumPocLtCurr++;
                prTempFBufInfo[ucRefPicFbufIdx].bLtMsbPresentFlag = pCurrRPS->bCheckLTMSB[i];

            }
            
            if(ucRefPicFbufIdx == FBUF_ID_UNKNOWN)
            {
                ucRefPicFbufIdx = ucVDecH265GetLTRefPicFBufIdByPOC(u4InstID,pCurrRPS->i4POC[i],pCurrRPS->bCheckLTMSB[i],prH265DecPrm);
                prTempFBufInfo[ucRefPicFbufIdx].bLtMsbPresentFlag = pCurrRPS->bCheckLTMSB[i];
            }
        }
    }

    numPocTotalCurr = NumPocStCurr0 + NumPocStCurr1 + NumPocLtCurr;
    j = 0;
    while((j < 16) && (numPocTotalCurr > 0))
    { 
        for ( i=0; (i < NumPocStCurr0) && (j < 16); j++,i++)
        {
            rpsCurrList0[j] = RefPicSetStCurr0[ i ];
        }
    
        for ( i=0; (i < NumPocStCurr1) && (j < 16); j++,i++)
        {
            rpsCurrList0[j] = RefPicSetStCurr1[ i ];
        }

        for ( i=0; (i < NumPocLtCurr) && (j < 16); j++,i++)
        {
            rpsCurrList0[j] = RefPicSetLtCurr[ i ];
        }
    }

    j = 0;
    while ((j < 16) && (numPocTotalCurr > 0))
    {
        for ( i=0; (i<NumPocStCurr1) && (j < 16); j++,i++)
        {
          rpsCurrList1[j] = RefPicSetStCurr1[ i ];
        }

        for ( i=0; (i<NumPocStCurr0) && (j < 16); j++,i++)
        {
            rpsCurrList1[j] = RefPicSetStCurr0[ i ];
        }

        for ( i=0; (i<NumPocLtCurr) && (j < 16); j++,i++)
        {
            rpsCurrList1[j] = RefPicSetLtCurr[ i ];
        }
    }
    
    prH265DecPrm->prPicInfo->i4RefListNum = numPocTotalCurr;

    for (i = 0; (i < numPocTotalCurr) & (numPocTotalCurr > 0); i ++)
    {
        i4POCDiff = prH265_Slice_Hdr_DataInst->i4POC - prTempFBufInfo[rpsCurrList0[i]].i4POC;
        i4POCDiff = (i4POCDiff < -128) ? -128 : (i4POCDiff >= 127) ? 127 : i4POCDiff;

        prH265DecPrm->prPicInfo->i4PocDiffList0[i] = i4POCDiff;
        prH265DecPrm->prPicInfo->i4LongTermList0[i] = (prTempFBufInfo[rpsCurrList0[i]].ucFBufRefType==LREF_PIC);
        prH265DecPrm->prPicInfo->i4List0DecOrderNo[i] = prTempFBufInfo[rpsCurrList0[i]].u4PicCnt;
        prH265DecPrm->prPicInfo->i4BuffIdList0[i] = rpsCurrList0[i];
    }
      

    for (i = 0; (i < numPocTotalCurr) & (numPocTotalCurr > 0); i ++)
    {
        i4POCDiff = prH265_Slice_Hdr_DataInst->i4POC -prTempFBufInfo[rpsCurrList1[i]].i4POC;
        i4POCDiff = (i4POCDiff < -128) ? -128 : (i4POCDiff >= 127) ? 127 : i4POCDiff;

        prH265DecPrm->prPicInfo->i4PocDiffList1[i] = i4POCDiff;
        prH265DecPrm->prPicInfo->i4LongTermList1[i] =  (prTempFBufInfo[rpsCurrList1[i]].ucFBufRefType==LREF_PIC);
        prH265DecPrm->prPicInfo->i4List1DecOrderNo[i] = prTempFBufInfo[rpsCurrList1[i]].u4PicCnt;
        prH265DecPrm->prPicInfo->i4BuffIdList1[i] = rpsCurrList1[i];
    }
    
    j = 0;
    for ( i=0; i<NumPocStCurr0; i++, j)
    {
      rpsCurrList0[j] = RefPicSetStCurr0[i];
    }
    for ( i=0; i<NumPocStCurr1; i++, j++)
    {
      rpsCurrList0[j] = RefPicSetStCurr1[i];
    }
    for ( i=0; i<NumPocLtCurr;  i++, j++)
    {
      rpsCurrList0[j] = RefPicSetLtCurr[i];
    }
      

    j = 0;
    for ( i=0; i<NumPocStCurr1; i++, j++)
    {
      rpsCurrList1[j] = RefPicSetStCurr1[i];
    }
    for ( i=0; i<NumPocStCurr0; i++, j++)
    {
      rpsCurrList1[j] = RefPicSetStCurr0[i];
    }
    for ( i=0; i<NumPocLtCurr;  i++, j++)
    {
      rpsCurrList1[j] = RefPicSetLtCurr[i];
    }

    // set low delay flag
    if ( numPocTotalCurr > 0 )
    {
        i4CurrPOC  = prH265_Slice_Hdr_DataInst->i4POC;
        for (i4RefIdx = 0; i4RefIdx < i4NumRefIdx[0]  && bLowDelay; i4RefIdx++)
        {
            if ( prTempFBufInfo[rpsCurrList0[i4RefIdx%numPocTotalCurr]].i4POC > i4CurrPOC )
            {
                bLowDelay = FALSE;
                break;
            }
        }

        for (i4RefIdx = 0; i4RefIdx < i4NumRefIdx[1]  && bLowDelay; i4RefIdx++)
        {
            if ( prTempFBufInfo[rpsCurrList1[i4RefIdx%numPocTotalCurr]].i4POC > i4CurrPOC )
            {
                bLowDelay = FALSE;
                break;
            }
        }        
        prH265DecPrm->prPicInfo->bLowDelayFlag = bLowDelay;
    }

    #ifdef VDEC_H265_DRV_DEBUG
    {
        INT32 i4StRefIdx = 0;
        INT32 i4LtRefIdx = 0;
        //for debug
        VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-Ref]*** Ref List Info ***\n");
        for(i = 0; i < prH265DecPrm->ucMaxFBufNum; i++)
        {
            VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-TEST-%d] FBuf[%d] POC %d FBMid %d,RefType %d Status %d\n",u4InstID,i,prTempFBufInfo[i].i4POC,prTempFBufInfo[i].ucFbmFbId,
                                                                                prTempFBufInfo[i].ucFBufRefType,prTempFBufInfo[i].ucFBufStatus);
            if(prTempFBufInfo[i].ucFBufRefType == NREF_PIC)
            {
                continue;
            }
            if(prTempFBufInfo[i].ucFBufRefType == SREF_PIC)
            {
                VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-Ref-%d] STRef[%d] FBuf[%d] POC(%d),FBMId (%d)\n",u4InstID,i4StRefIdx,i,prTempFBufInfo[i].i4POC,prTempFBufInfo[i].ucFbmFbId);
                i4StRefIdx ++;
            }
            else if(prTempFBufInfo[i].ucFBufRefType == LREF_PIC)
            {
                VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-Ref-%d] LTRef[%d] FBuf[%d] POC(%d),FBMId (%d)\n",u4InstID,i4LtRefIdx,i,prTempFBufInfo[i].i4POC,prTempFBufInfo[i].ucFbmFbId);
                i4LtRefIdx ++;
            }
        }
        VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-Ref-%d]*** Ref List Info ***\n",u4InstID);
    }
    #endif

    return 0;
}


UCHAR ucVDecH265GetSTRefPicFBufIdByPOC(UINT32 u4InstID,INT32 i4POC,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i;
    UCHAR ucRefPicFBufIdx = FBUF_ID_UNKNOWN;
    for(i = 0; i < prH265DecPrm->ucMaxFBufNum; i++)
    {
        if((prH265DecPrm->prFBufInfo[i].i4POC == i4POC) && (prH265DecPrm->prFBufInfo[i].ucFBufStatus != NO_PIC))
        {
            ucRefPicFBufIdx = i;
            break;
        }
    }
    return ucRefPicFBufIdx;
}

UCHAR ucVDecH265GetLTRefPicFBufIdByPOC(UINT32 u4InstID,INT32 i4POC,BOOL bMSBpresentFlag,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i;
    UCHAR ucRefPicFBufIdx = FBUF_ID_UNKNOWN;
    UCHAR ucLtFBIndex = FBUF_ID_UNKNOWN;
    INT32 i4POCCycle,i4POCFbuf;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst = NULL;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;
    
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    
    i4POCCycle = 1 << (prH265DecPrm->prSPS->u4Log2MaxPicOrderCntLsbMinus4 + 4);
    if ( !bMSBpresentFlag )
    {
        i4POC = i4POC%i4POCCycle;
    }

    for(i = 0; i < prH265DecPrm->ucMaxFBufNum; i++)
    {
        if((prTempFBufInfo[i].ucFBufStatus != NO_PIC) && (prTempFBufInfo[i].i4POC == prH265_Slice_Hdr_DataInst->i4POC))
        {
            i4POCFbuf = prTempFBufInfo[i].i4POC;
            if(!bMSBpresentFlag)
            {
                i4POCFbuf = i4POCFbuf%i4POCCycle;
            }

            if(i4POCFbuf == i4POC)
            {
                if(prTempFBufInfo[i].ucFBufRefType == LREF_PIC)
                {
                    ucRefPicFBufIdx = i;
                }
                else
                {
                    ucLtFBIndex = i;
                }
                break;
            }
        }
        
    }

    if(ucLtFBIndex < prH265DecPrm->ucMaxFBufNum)
    {
        ucRefPicFBufIdx = ucLtFBIndex;
    }

    return ucRefPicFBufIdx;
}

BOOL  fgVDecH265GetFrmToDispQ(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    return TRUE;
}


UCHAR  ucVDecH265SendSeqChgInfo(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    BOOL fgResChg = FALSE,fgCroppingChg = FALSE,fgFrameRateChg = FALSE,fgAspChg = FALSE;
    UINT32 u4SPSCropX = 0,u4SPSCropY = 0,u4SPSCropW = 0,u4SPSCropH = 0;
    UCHAR ucFrameRate = MPEG_FRAME_RATE_UNKNOWN;
    UCHAR ucResChgMod = 0x00;
    H265_SPS_Data *prH265_SPS_DataInst = NULL;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;
    
    ASSERT(prH265DecPrm!=NULL);
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prH265DecPrm->ucFbgId);
    if(prFbmSeqHdr == NULL)
    {
        VDecPrintf(u4InstID,VDEC_LOG_SEQINFO, "[H265-ERR] GetFbgSeqHdr Fail 2!!\n");
        return ucResChgMod;
    }
    
    if(prH265DecPrm->prSPS->bConformanceWindowFlag)
    {
        u4SPSCropX = prH265_SPS_DataInst->u4ConfWinLeftOffset * prH265_SPS_DataInst->u4SubWidthC;
        u4SPSCropY = prH265_SPS_DataInst->u4ConfWinTopOffset * prH265_SPS_DataInst->u4SubHeightC;
        u4SPSCropW = prH265_SPS_DataInst->u4PicWidthInLumaSamples - u4SPSCropX - 
                     prH265DecPrm->prSPS->u4ConfWinRightOffset * prH265DecPrm->prSPS->u4SubWidthC;
        u4SPSCropH = prH265_SPS_DataInst->u4PicHeightInLumaSamples - u4SPSCropY - 
                     prH265DecPrm->prSPS->u4ConfWinBottomOffset * prH265DecPrm->prSPS->u4SubHeightC;
    }
    
    if((prFbmSeqHdr->fgCropping != prH265DecPrm->prSPS->bConformanceWindowFlag) || 
       (prFbmSeqHdr->u4CropX != u4SPSCropX) || (prFbmSeqHdr->u4CropY != u4SPSCropY) ||
       (prFbmSeqHdr->u4CropWidth != u4SPSCropW) || (prFbmSeqHdr->u4CropHeight != u4SPSCropH))
    {
        prFbmSeqHdr->fgCropping = prH265DecPrm->prSPS->bConformanceWindowFlag;
        prFbmSeqHdr->u4CropX = u4SPSCropX;
        prFbmSeqHdr->u4CropY = u4SPSCropY;
        prFbmSeqHdr->u4CropWidth = u4SPSCropW;
        prFbmSeqHdr->u4CropHeight = u4SPSCropH;
        fgCroppingChg = TRUE;
    }

    if(prH265DecPrm->ucSeqChgMode & RESOLUTION_CHG)
    {
        prFbmSeqHdr->u2OrgWidth = prH265DecPrm->u4PicW - prH265DecPrm->prSPS->u4ConfWinRightOffset * prH265DecPrm->prSPS->u4SubWidthC;
        prFbmSeqHdr->u2OrgHeight = prH265DecPrm->u4PicH - prH265DecPrm->prSPS->u4ConfWinBottomOffset * prH265DecPrm->prSPS->u4SubHeightC;    
        fgResChg = TRUE;
        ucResChgMod |= RESOLUTION_CHG;// Resolution change
    }

    ucFrameRate = ucVDecH265GetFrameRate(u4InstID,prH265DecPrm);
    
    if((ucFrameRate != MPEG_FRAME_RATE_UNKNOWN) && (ucFrameRate != prFbmSeqHdr->ucFrmRatCod))
    {
        VDecPrintf(u4InstID,VDEC_LOG_SEQINFO,"[H265-INFO] Frame rate change %d -> %d\n",prFbmSeqHdr->ucFrmRatCod,ucFrameRate);
        prFbmSeqHdr->ucFrmRatCod = ucFrameRate;
        fgFrameRateChg = TRUE;
        ucResChgMod |= FRAMERATE_CHG;// Frame-Rate change
        prH265DecPrm->ucSeqChgMode |= FRAMERATE_CHG;
    }
    else if(ucFrameRate == MPEG_FRAME_RATE_UNKNOWN)
    {
        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
    }

    fgAspChg = fgVDecH265IsAspInfoChg(u4InstID,prH265DecPrm);

    //progressive vs interlace

    if(  (prH265_SPS_DataInst->rSPS_PTL.generalPTL.bProgressiveSourceFlag == 1)
      && (prH265_SPS_DataInst->rSPS_PTL.generalPTL.bInterlacedSourceFlag == 0))
    {
        prFbmSeqHdr->fgProgressiveSeq = TRUE;
    }
    else if(  (prH265_SPS_DataInst->rSPS_PTL.generalPTL.bProgressiveSourceFlag == 0)
            &&(prH265_SPS_DataInst->rSPS_PTL.generalPTL.bInterlacedSourceFlag == 1))
    {
        prFbmSeqHdr->fgProgressiveSeq = FALSE;
    }
    else if(  (prH265_SPS_DataInst->rSPS_PTL.generalPTL.bProgressiveSourceFlag == 0)
            &&(prH265_SPS_DataInst->rSPS_PTL.generalPTL.bInterlacedSourceFlag == 0))
    {
        //Specifies unknow,interpreted as interlace quality maybe better than progressive
        //prFbmSeqHdr->fgProgressiveSeq = FALSE;
        prFbmSeqHdr->fgProgressiveSeq = TRUE;
    }
    else
    {
        //Need Indicated using SEI "source_scan_type"
    }
    
    
    if(fgResChg || fgCroppingChg || fgFrameRateChg || fgAspChg)
    {
        VDEC_INFO_T *prVdecInfo;
		VDEC_PICTYPE_CB_T rPicType;
		prVdecInfo=_VDEC_GetInfo();
        FBM_SetSeqChgFbId(prH265DecPrm->ucFbgId, prH265DecPrm->prCurrFBufInfo->ucFbmFbId);

        if (prVdecEsInfo->fgMMPlayback)
        {
            FBM_SetPlayMode(prH265DecPrm->ucFbgId, FBM_FBG_MM_MODE);
        }

        //Block mode info
        prFbmSeqHdr->ucBlockType = 1; //1 : 16*32 BLOCK MODE; 0 : 64*32 BLOCK MODE
        prFbmSeqHdr->fgRasterOrder = 0;
        prFbmSeqHdr->eSwapMode = FBM_SM_NO_SWAP;
        
        //Bit-depth info
        if(prH265DecPrm->ucMain10Mode == HEVC_8BIT_MODE)
        {
            prFbmSeqHdr->u1Depth = 8;
            prVdecEsInfo->u4BitDepth = 8;
            prFbmSeqHdr->ePackMode = FBM_PM_NORMAL;
        }
        else
        {
            prFbmSeqHdr->u1Depth = 10;
            prVdecEsInfo->u4BitDepth = 10;
            prFbmSeqHdr->ePackMode = FBM_PM_COMPACT;
        }

        //UFO info
        if(prH265DecPrm->fgIsUFOMode)
        {
            prFbmSeqHdr->fgUfoEn = TRUE;
        }
        else
        {
            prFbmSeqHdr->fgUfoEn = FALSE;
        }
        
        #ifdef CC_ENABLE_SEQ_CHG_SPEEDUP
        if (!prVdecEsInfo->fgMMPlayback)
        {
            FBM_SetFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP);
        }
        else
        {
            FBM_SetFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SEQ_CHG);
        }
        #else
        FBM_SetFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SEQ_CHG);
        #endif
        
        if(prVdecInfo->pfPicTypeCb && ((prVdecEsInfoKeep->fgVPush) || (prH265DecPrm->fgIsPVR)))
        {
		   rPicType.u1EsId=prVdecEsInfoKeep->rPesInfo.ucEsId;
		   rPicType.u4Pts=prVdecEsInfoKeep->rPesInfo.u4PTS;
		   rPicType.u1PicType=(UINT8)VDEC_PIC_TYPE_SEQ;
		   prVdecInfo->pfPicTypeCb(&rPicType);
        }
        
        VDecPrintf(u4InstID,VDEC_LOG_SEQINFO,"[H265-INFO]SeqChg Res[%d,%d] Crop[%d,%d,%d,%d,%d],FR %d,BitDepth %d UFO %d\n",prH265DecPrm->u4PicW,prH265DecPrm->u4PicH,
            prFbmSeqHdr->fgCropping,prFbmSeqHdr->u4CropX,prFbmSeqHdr->u4CropY,prFbmSeqHdr->u4CropWidth,prFbmSeqHdr->u4CropHeight,
            prFbmSeqHdr->ucFrmRatCod,prFbmSeqHdr->u1Depth,prFbmSeqHdr->fgUfoEn);
    }

    //set AV sync mode
    FBM_SetSyncStc(prH265DecPrm->ucFbgId, prH265DecPrm->ucSyncMode, prH265DecPrm->ucStcId);
    return ucResChgMod;
}


BOOL  fgVDecH265IsAspInfoChg(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    BOOL fgAspChg = FALSE;
    UINT32 u4DAR = 0,u4SAR = 0;
    H265_SPS_Data *prH265_SPS_DataInst = NULL;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
    
    ASSERT(prH265DecPrm!=NULL);
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prH265DecPrm->ucFbgId);
    
    if(prH265_SPS_DataInst->bVuiParametersPresentFlag && (prH265_SPS_DataInst->rVUI.bAspectRatioInfoPresentFlag))
    {
        if(prH265_SPS_DataInst->rVUI.i4AspectRatioIdc == 0xFF)
        {
            if((prH265DecPrm->rLastInfo.i4LastSarWidth != prH265DecPrm->prSPS->rVUI.i4SarWidth) ||
               (prH265DecPrm->rLastInfo.i4LastSarHeight!= prH265DecPrm->prSPS->rVUI.i4SarHeight))
            {
                u4DAR = ((prH265DecPrm->u4PicW * prH265DecPrm->prSPS->rVUI.i4SarWidth) * 100) /
                        (prH265DecPrm->u4PicH * prH265DecPrm->prSPS->rVUI.i4SarHeight);
                prH265DecPrm->rLastInfo.i4LastSarWidth = prH265_SPS_DataInst->rVUI.i4SarWidth;
                prH265DecPrm->rLastInfo.i4LastSarHeight = prH265_SPS_DataInst->rVUI.i4SarHeight;
                prH265DecPrm->rLastInfo.u4AspRatInf = 0xFF;
                prH265DecPrm->rLastInfo.u4DAR = u4DAR;
                fgAspChg = TRUE;
                prFbmSeqHdr->u4AspectRatioWidth = prH265_SPS_DataInst->rVUI.i4SarWidth;
                prFbmSeqHdr->u4AspectRatioHeight = prH265_SPS_DataInst->rVUI.i4SarHeight;
                VDecPrintf(u4InstID,VDEC_LOG_SEQINFO,"[H265-INFO] Aspectratio change %d @ Line %d\n",u4DAR,__LINE__);
            }
        }
        else if(prH265DecPrm->rLastInfo.u4AspRatInf != prH265_SPS_DataInst->rVUI.i4AspectRatioIdc)
        {
            prFbmSeqHdr->ucOrgAsp = ucVDecH265GetAspInfo(u4InstID,prH265DecPrm,&(prFbmSeqHdr->u4AspectRatioWidth),&(prFbmSeqHdr->u4AspectRatioHeight),&u4SAR);
            u4DAR = (prH265DecPrm->u4PicW * u4SAR) / prH265DecPrm->u4PicH;
            prH265DecPrm->rLastInfo.i4LastSarWidth = 0;
            prH265DecPrm->rLastInfo.i4LastSarHeight = 0;
            prH265DecPrm->rLastInfo.u4AspRatInf = prH265_SPS_DataInst->rVUI.i4AspectRatioIdc;
            prH265DecPrm->rLastInfo.u4DAR = u4DAR;
            fgAspChg = TRUE;
            VDecPrintf(u4InstID,VDEC_LOG_SEQINFO,"[H265-INFO] Aspectratio change %d @ Line %d\n",u4DAR,__LINE__);
        }
    }
    else
    {
        
    }

    prFbmSeqHdr->u4OrgAspWidth = prFbmSeqHdr->u4AspectRatioWidth;
    prFbmSeqHdr->u4OrgAspHeight = prFbmSeqHdr->u4AspectRatioHeight;

    if (fgAspChg)
    {
        prFbmSeqHdr->u4AspectRatioIdc = prH265_SPS_DataInst->rVUI.i4AspectRatioIdc;
    
        if (((u4DAR *3) >> 2) < 110)
        {
            prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_4_3;
        }
        else if (((u4DAR *9) >> 4) < 110)
        {
            prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;
        }
        else if (((u4DAR *100) / 221) < 110)
        {
            prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_221_1;
        }
        else
        {
            prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;
        }

        //add default value for MM playback
        if (prH265DecPrm->fgIsMMPlay)
        {
            prFbmSeqHdr->fgPixelAspectRatio = TRUE;
            if(!prH265_SPS_DataInst->rVUI.bAspectRatioInfoPresentFlag)
            {
                if (prVdecEsInfo && prVdecEsInfo->rMMSeqInfo.e_src_asp == VDEC_ASPECT_RATIO_TRANSMIT)
                {
                    if (prVdecEsInfo->rMMSeqInfo.e_asp_usr_type == VDEC_ASP_USR_TYPE_DISP)
                    {
                        prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                    }
                    prFbmSeqHdr->u4AspectRatioWidth = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w;
                    prFbmSeqHdr->u4AspectRatioHeight = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h;
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                }
                else
                {
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                }
            }
            else
            {
                if((prFbmSeqHdr->ucAspRatInf >= MPEG_ASPECT_RATIO_1_1) && (prFbmSeqHdr->ucAspRatInf < MPEG_ASPECT_RATIO_TRANSMIT))
                {
                    //do nothing, as Asp Rat info has already calculate above.
                }
                else
                {
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                }   
            }
        }
        //TO DO ITEM
        //Add DTV case ASP calc
    }

    return fgAspChg;
}

void vVDecH265SetRegister(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UCHAR u2ECFbIndex = 0xFF;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;

    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    ASSERT(prTempFBufInfo != NULL);
    
    //Only Real decode flow need those register setting
    if (prH265DecPrm->ucVDecStatus == REAL_DECODE)
    {
        //search nearest poc to ref-register
        u2ECFbIndex = ucSearchNearestPOCFbufIdx(u4InstID,prH265DecPrm);
        
        u4VDEC_HAL_H265_SetUFO(u4VDecID,prH265DecPrm->prCurrFBufInfo);
        
        u4VDEC_HAL_H265_SetECInfo(u4VDecID,&(prTempFBufInfo[u2ECFbIndex]),prH265DecPrm);

        vVDEC_HAL_H265_SetRefPicListReg(u4VDecID,prH265DecPrm->prPicInfo,prH265DecPrm->prFBufInfo);

        vVDEC_HAL_H265_InitPicInfo(u4VDecID,prH265DecPrm);

        vVDecH265AssignQuantParam(u4InstID,u4VDecID,prH265DecPrm);

        //decode with core0/core1 separate
        if(!(prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE))
        {
            VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[VDEC-H265-%d]%d PIP setting \n",u4InstID,u4VDecID);
            vVDEC_HAL_H265_VDec_SetPipComReg(u4VDecID,prH265DecPrm->prSPS);
        }

        //Performance ref register
        vVDecWriteMC(u4VDecID,RW_MC_BANDWIDTH_REF,(u4VDecReadMC(u4VDecID,RW_MC_BANDWIDTH_REF) | DECODE_PERFORMANCE));
    }
    
    vVDEC_HAL_H265_SetSHDRHEVLD(u4VDecID,prH265DecPrm);
    vVDEC_HAL_H265_SetSPSHEVLD(u4VDecID,prH265DecPrm->prSPS,prH265DecPrm->prPPS);
    vVDEC_HAL_H265_SetPPSHEVLD(u4VDecID,prH265DecPrm->prSPS,prH265DecPrm->prPPS);
    vVDecH265InitTilesInfo(u4InstID,prH265DecPrm->prSPS,prH265DecPrm->prPPS,prH265DecPrm->prPicInfo);
    vVDEC_HAL_H265_SetTilesInfo(u4VDecID,prH265DecPrm->prSPS,prH265DecPrm->prPPS,prH265DecPrm->prPicInfo);

}

void vVDecH265AssignQuantParam(UINT32 u4InstID,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32  size,list,qp;
    UINT32 u4IQSramAddr = 0;
    H265_SPS_Data *prH265_SPS_DataInst = NULL;
    H265_PPS_Data *prH265_PPS_DataInst = NULL;
    H265_SL_Data * pScalingList;

    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    prH265_PPS_DataInst = prH265DecPrm->prPPS;
    
    // SQT part
    vVDecWriteHEVCPP(u4VDecID, HEVC_IQ_SL_CTRL, prH265_SPS_DataInst->bScalingListFlag);

    if(prH265_SPS_DataInst->bScalingListFlag)
    {
        pScalingList = &(prH265_SPS_DataInst->rSPS_ScalingList);
        if (prH265_PPS_DataInst->bPPSScalingListPresentFlag)
        {
            pScalingList = &(prH265_PPS_DataInst->rPPS_ScalingList);
        }
        else
        {
            
        }
        pScalingList->bUseTransformSkip = prH265_PPS_DataInst->bTransformSkipEnabledFlag;

        if (!prH265_PPS_DataInst->bPPSScalingListPresentFlag && !prH265_SPS_DataInst->bScalingListPresentFlag)
        {
            UINT32 sizeId,listId;
            for( sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
            {
                for( listId = 0;listId < H265_scalingListNum[sizeId]; listId++)
                {
                    INT32 listSize = (MAX_MATRIX_COEF_NUM < H265_scalingListSize[sizeId])? MAX_MATRIX_COEF_NUM : H265_scalingListSize[sizeId];
                    x_memcpy(pScalingList->pScalingListDeltaCoef[sizeId][listId], vVDecH265GetSLDefaultAddress(sizeId, listId), sizeof(INT32)*listSize);
                    pScalingList->i4ScalingListDC[sizeId][listId] = (INT32)SCALING_LIST_DC;
                }
            }
        }

        vVDEC_HAL_H265_SetSLPP( u4VDecID, pScalingList );
        vVDecWriteHEVCPP(u4VDecID, HEVC_IQ_SRAM_32BITS_CTRL_REG, 0x1 );
        
        for(size = 0; size < SCALING_LIST_SIZE_NUM; size++)
        {
            for(list = 0; list < H265_scalingListNum[size]; list++)
            {
                for(qp = 0;qp < SCALING_LIST_REM_NUM; qp++)
                {
                    UINT32 u4Width = H265_scalingListSizeX[size];
                    UINT32 u4Height = H265_scalingListSizeX[size];
                    INT32 *pCoeff = pScalingList->pScalingListDeltaCoef[size][list];
                    vVDEC_HAL_H265_SetSLVLD(u4VDecID, pCoeff, u4Width, u4Height, H265_invQuantScales[qp],&u4IQSramAddr);
                }
            }
        }

        vVDecWriteHEVCPP(u4VDecID, HEVC_IQ_SRAM_32BITS_CTRL_REG, 0);
    }
}

void vVDecH265InitTilesInfo(UINT32 u4InstID,H265_SPS_Data *prSPS,H265_PPS_Data *prPPS,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo)
{
    UINT32  u4TileIdx,u4ColumnIdx = 0,u4RowIdx = 0;
    UINT32  u4RightEdgePosInCU,u4BottomEdgePosInCU;
    UINT32  u4CummulativeTileWidth, u4CummulativeTileHeight;
    INT32   i, j, p;
    INT32   i4Max_cu_width, i4Max_cu_heigtht;
    INT32   i4Pic_width, i4Pic_height;
    INT32   i4Lcu_pic_width, i4Lcu_pic_height;
    INT32   i4Log2_max_cu_size;

    i4Log2_max_cu_size =  prSPS->u4Log2DiffMaxMinCodingBlockSize + prSPS->u4Log2MinCodingBlockSizeMinus3+3;
    i4Max_cu_width = 1 << i4Log2_max_cu_size;
    i4Max_cu_heigtht = 1 << i4Log2_max_cu_size;

    i4Pic_width = prSPS->u4PicWidthInLumaSamples;
    i4Pic_height = prSPS->u4PicHeightInLumaSamples;
    i4Pic_width = ((i4Pic_width + i4Max_cu_width -1) >> i4Log2_max_cu_size ) << i4Log2_max_cu_size;
    i4Pic_height = ((i4Pic_height + i4Max_cu_width -1) >> i4Log2_max_cu_size ) << i4Log2_max_cu_size;

    i4Lcu_pic_width    = ( i4Pic_width%i4Max_cu_width ) ? i4Pic_width /i4Max_cu_width  + 1 : i4Pic_width /i4Max_cu_width;
    i4Lcu_pic_height   = ( i4Pic_height%i4Max_cu_heigtht ) ? i4Pic_height/i4Max_cu_heigtht + 1 : i4Pic_height/i4Max_cu_heigtht;

    // set tiles width and height
    if( prPPS->bUniformSpacingFlag )
    {
        //set the width for each tile
        for(j = 0; j < prPPS->u4NumRowsMinus1+1; j++)
        {
            for(p = 0; p < prPPS->u4NumColumnsMinus1+1; p++)
            {
                prH265PicInfo->rTileInfo[j * (prPPS->u4NumColumnsMinus1+1) + p].u4TileWidth = 
                    (p+1)*i4Lcu_pic_width/(prPPS->u4NumColumnsMinus1+1) 
                    - (p*i4Lcu_pic_width)/(prPPS->u4NumColumnsMinus1+1);
            }
        }

        //set the height for each tile
        for(j = 0; j < prPPS->u4NumColumnsMinus1+1; j++)
        {
            for(p = 0; p < prPPS->u4NumRowsMinus1+1; p++)
            {
                prH265PicInfo->rTileInfo[p * (prPPS->u4NumColumnsMinus1+1) + j ].u4TileHeight =
                   (p+1)*i4Lcu_pic_height/(prPPS->u4NumRowsMinus1+1) 
                  - (p*i4Lcu_pic_height)/(prPPS->u4NumRowsMinus1+1) ;   
            }
        }
    }
    else
    {
        //set the width for each tile
        for(j = 0; j < prPPS->u4NumRowsMinus1+1; j++)
        {
            u4CummulativeTileWidth = 0;
            for(i = 0; i < prPPS->u4NumColumnsMinus1; i++)
            {
                prH265PicInfo->rTileInfo[j * (prPPS->u4NumColumnsMinus1+1) + i].u4TileWidth =  prPPS->u4ColumnWidthMinus1[i]+1;
                u4CummulativeTileWidth += prPPS->u4ColumnWidthMinus1[i]+1;
            }
            prH265PicInfo->rTileInfo[j * (prPPS->u4NumColumnsMinus1+1) + i].u4TileWidth = ( i4Lcu_pic_width - u4CummulativeTileWidth );
        }

        //set the height for each tile
        for(j = 0; j < prPPS->u4NumColumnsMinus1+1; j++)
        {
            u4CummulativeTileHeight = 0;
            for(i = 0; i < prPPS->u4NumRowsMinus1; i++)
            { 
                prH265PicInfo->rTileInfo[i * (prPPS->u4NumColumnsMinus1+1) + j].u4TileHeight = prPPS->u4RowHeightMinus1[i]+1;
                u4CummulativeTileHeight += prPPS->u4RowHeightMinus1[i]+1;
            }
            prH265PicInfo->rTileInfo[i * (prPPS->u4NumColumnsMinus1+1) + j].u4TileHeight = ( i4Lcu_pic_height - u4CummulativeTileHeight );
        }
    }


    //initialize each tile of the current picture
    for( u4RowIdx = 0; u4RowIdx < prPPS->u4NumRowsMinus1+1; u4RowIdx++ )
    {
        for( u4ColumnIdx = 0; u4ColumnIdx < prPPS->u4NumColumnsMinus1+1; u4ColumnIdx++ )
        {
            u4TileIdx = u4RowIdx * (prPPS->u4NumColumnsMinus1+1) + u4ColumnIdx;

            //initialize the RightEdgePosInCU for each tile
            u4RightEdgePosInCU = 0;
            for( i = 0; i <= u4ColumnIdx; i++ )
            {
                u4RightEdgePosInCU += prH265PicInfo->rTileInfo[u4RowIdx * (prPPS->u4NumColumnsMinus1+1) + i].u4TileWidth;
            }
            prH265PicInfo->rTileInfo[u4TileIdx].u4RightEdgePosInCU = (u4RightEdgePosInCU-1);

            //initialize the BottomEdgePosInCU for each tile
            u4BottomEdgePosInCU = 0;
            for( i = 0; i <= u4RowIdx; i++ )
            {
                u4BottomEdgePosInCU += prH265PicInfo->rTileInfo[i * (prPPS->u4NumColumnsMinus1+1) + u4ColumnIdx].u4TileHeight;
            }
            prH265PicInfo->rTileInfo[u4TileIdx].u4BottomEdgePosInCU = (u4BottomEdgePosInCU-1);

            //initialize the FirstCUAddr for each tile
            prH265PicInfo->rTileInfo[u4TileIdx].u4FirstCUAddr = ( (prH265PicInfo->rTileInfo[u4TileIdx].u4BottomEdgePosInCU - 
            prH265PicInfo->rTileInfo[u4TileIdx].u4TileHeight +1)*i4Lcu_pic_width + 
            prH265PicInfo->rTileInfo[u4TileIdx].u4RightEdgePosInCU - prH265PicInfo->rTileInfo[u4TileIdx].u4TileWidth + 1);
        }
    }
}


UCHAR ucVDecH265PrepareDispBuf(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4MinPOCFBufIdx = FBUF_ID_UNKNOWN;
    HAL_TIME_T rTempTime;
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo;

    ASSERT(prH265DecPrm!=NULL);
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    
    if((prH265DecPrm->fgNoRaslOutput == TRUE) && 
       ((prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_TFD) || (prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_RASL_N)))
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId, prH265DecPrm->prCurrFBufInfo->ucFbmFbId);
        VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"Force Empty PTS %d POC %d Fbm[%d,%d]\n",prFbmPicHdr->u4PTS,
                                                                                prH265DecPrm->prCurrFBufInfo->i4POC,
                                                                                prH265DecPrm->ucFbgId,
                                                                                prH265DecPrm->prCurrFBufInfo->ucFbmFbId);
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prCurrFBufInfo->ucFbmFbId,FBM_FB_STATUS_EMPTY);
        prH265DecPrm->prCurrFBufInfo->eH265DpbStatus = H265_DPB_STATUS_OUTPUTTED;
        prH265DecPrm->i4ReadyOutCntDPB --;
    }
    
    if(prH265DecPrm->fgDecEuqalDispOrder)
    { 
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId, prH265DecPrm->prCurrFBufInfo->ucFbmFbId);
        #ifdef CC_REALD_3D_SUPPORT
        _VDEC_Check2D3DSwitch(u4InstID,prH265DecPrm->prCurrFBufInfo->ucFbmFbId);
        #endif
        _VDEC_JobBeforePutDispQ(u4InstID,prH265DecPrm->prCurrFBufInfo->ucFbmFbId);
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prCurrFBufInfo->ucFbmFbId,FBM_FB_STATUS_DISPLAYQ);
        VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"PQ POC %d,PTS %d Fbm[%d,%d] Dec=Disp\n",prH265DecPrm->prCurrFBufInfo->i4POC,prFbmPicHdr->u4PTS,
                                prH265DecPrm->ucFbgId,prH265DecPrm->prCurrFBufInfo->ucFbmFbId);

        prH265DecPrm->prCurrFBufInfo->eH265DpbStatus = H265_DPB_STATUS_OUTPUTTED;
        if(prH265DecPrm->ucSkipMode == IMode)
        {
            vVDecH265ClrFBufInfo(u4InstID, prH265DecPrm->ucDecFBufId ,prH265DecPrm);
            FBM_UpdateReferenceList(prH265DecPrm->ucFbgId, prH265DecPrm->prCurrFBufInfo->ucFbmFbId, FALSE);
        }
        else
        {
            //to do list other dec=disp case
        }
        
        prH265DecPrm->i4ReadyOutCntDPB --;
        prVdecEsInfo->u4DisplayQPicCnt ++;
        if(prH265DecPrm->fgIsMMPlay)
        {
            UNUSED(VDEC_CheckStopStatus(u4InstID, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));    
        }
    }
    else if(prH265DecPrm->i4ReadyOutCntDPB > prH265DecPrm->prSPS->u4NumReorderPics[prH265DecPrm->prSPS->u4MaxTLayersMinus1])
    {
        u4MinPOCFBufIdx = ucVDecH265GetMinPOCToOutPut(u4InstID,prH265DecPrm);
        ASSERT(u4MinPOCFBufIdx != FBUF_ID_UNKNOWN);

        prFbmPicHdr = FBM_GetFrameBufferPicHdr(prH265DecPrm->ucFbgId, prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId);
        //notify audio start to play
        //TO DO ITEM

        ////////////////////////////
        if (prH265DecPrm->fgIsMMPlay && VDEC_IsNotDisplay(u4InstID, prFbmPicHdr->u4PTS,
                            prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset))
        {
            VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"Force Empty POC %d Fbm[%d,%d]\n",prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].i4POC,prH265DecPrm->ucFbgId,
                                    prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId);
            FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId,
                                     FBM_FB_STATUS_EMPTY);
        }
        else
        {
            #ifdef CC_REALD_3D_SUPPORT
            _VDEC_Check2D3DSwitch(u4InstID,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId);
            #endif
            _VDEC_JobBeforePutDispQ(u4InstID,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId);
            FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId,
                                     FBM_FB_STATUS_DISPLAYQ);
            HAL_GetTime(&rTempTime);
            VDecPrintf(u4InstID,VDEC_LOG_VDEC2VDP,"PQ POC %d,PTS %d Fbm[%d,%d] @ %d.%6d\n",prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].i4POC,prFbmPicHdr->u4PTS,
                                    prH265DecPrm->ucFbgId,prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].ucFbmFbId,rTempTime.u4Seconds,rTempTime.u4Micros);
            prVdecEsInfo->u4DisplayQPicCnt ++;
        }
        
        //Clear FBbufInfo
        if(fgVDecH265IsNonRefFBuf(u4InstID,u4MinPOCFBufIdx,prH265DecPrm))
        {
            vVDecH265ClrFBufInfo(u4InstID, u4MinPOCFBufIdx ,prH265DecPrm);
        }
        prH265DecPrm->prFBufInfo[u4MinPOCFBufIdx].eH265DpbStatus = H265_DPB_STATUS_OUTPUTTED;
        prH265DecPrm->i4ReadyOutCntDPB --;
        
        if(prH265DecPrm->fgIsMMPlay)
        {
            UNUSED(VDEC_CheckStopStatus(u4InstID, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));    
        }
    }
    
    return 0;
}

void vVDecH265GetCRC(UINT32 u4InstID,UINT32 u4VDecID,UINT32 *pu4Y,UINT32 *pu4C,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i,j;
    //Single core crc

    #if (defined (CC_MT5890))
    for(i = 0; i < 4; i++)
    {
        pu4Y[i] = u4VDecReadMCORETOP(u4VDecID, (193 + i + (u4VDecID << 3)) << 2 );
        pu4C[i] = u4VDecReadMCORETOP(u4VDecID, (197 + i + (u4VDecID << 3)) << 2 );
    }

    if(prH265DecPrm->ucDualCoreMode)
    {
        for(j = 0; j < 4; j++)
        {
            pu4Y[i] = u4VDecReadMCORETOP(u4VDecID, (201 + j + (u4VDecID << 3)) << 2 );
            pu4C[i] = u4VDecReadMCORETOP(u4VDecID, (205 + j + (u4VDecID << 3)) << 2 );
            i++;
        }
    }
    #else //gazelle/capri score mode.
    UNUSED(j);
    for(i = 0; i < 4; i++)
    {
        pu4Y[i] = u4VDecReadHEVCMISC(u4VDecID, (95 + i) << 2 );
        pu4C[i] = u4VDecReadHEVCMISC(u4VDecID, (99 + i) << 2 );
    }
    #endif

}

BOOL fgMCoreMatchLAE(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    VDEC_LAE_DecPrm *pLaeDecPrm;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    pLaeDecPrm = &(prH265DecPrm->rLaeInfo);

    while(pLaeDecPrm->u4PesRdptr != prH265DrvDecInfo->rPesInfo.u4VldReadPtr)
    {
        //Put used lae buffer to empty Q
        vPutBufferToLae(prVdecEsInfo->ucLaeEsInfoId,&(prH265DecPrm->rLaeInfo));
        ucGetBufferForMcoreDec(u4InstID,&(prH265DecPrm->rLaeInfo));
        VDecPrintf(u4InstID,VDEC_LOG_LAEDEC,"Drop lae decQ 0x%x,0x%x @ rdptr 0x%x \n",prH265DecPrm->rLaeInfo.u4CabacAddr,prH265DecPrm->rLaeInfo.u4ECAddr,prH265DecPrm->rLaeInfo.u4PesRdptr);
    }

    return TRUE;
}

UCHAR ucSearchNearestPOCFbufIdx(UINT32 u4InstID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    INT32 i,i4MinPoc = 0x7FFFFFFF;
    UCHAR ucEcRefFbufId = 0xFF;
    VDEC_INFO_H265_FBUF_INFO_T *prTempFBufInfo = NULL;

    prTempFBufInfo = prH265DecPrm->prFBufInfo;
    
    for (i = 0; i < prH265DecPrm->ucMaxFBufNum; i++ )
    {
        if (i == prH265DecPrm->ucDecFBufId)
        {
            continue;
        }
        if (abs_hevc(prH265DecPrm->prSliceHdr->i4POC - prTempFBufInfo[i].i4POC) < i4MinPoc)
        {
            i4MinPoc = abs_hevc(prH265DecPrm->prSliceHdr->i4POC - prTempFBufInfo[i].i4POC);
            ucEcRefFbufId = i;
        } 
    }

    //First time use this buffer
    if ((ucEcRefFbufId != 0xFF) && (prTempFBufInfo[ucEcRefFbufId].i4POC == 0x7FFFFFFF) && (prTempFBufInfo[ucEcRefFbufId].u4YStartAddr == 0))
    {
        ucEcRefFbufId = prH265DecPrm->ucDecFBufId;
    }
    else if(i4MinPoc == 0x7FFFFFFF)
    {
        ucEcRefFbufId = prH265DecPrm->ucDecFBufId;
    }
    
    prTempFBufInfo = &(prH265DecPrm->prFBufInfo[ucEcRefFbufId]);
    VDecPrintf(u4InstID,VDEC_LOG_DECREF,"VDEC[%d] CurrPoc %d ,Nearest POC[%d] bufid %d\n",u4InstID,prH265DecPrm->prSliceHdr->i4POC,prTempFBufInfo->i4POC,ucEcRefFbufId);
    
    return ucEcRefFbufId;
}

#ifdef VDEC_H265_Performance
void vVDecH265Performance(UINT32 ucEsId,UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4MCBandwidth,u4PPBandwidth,u4TotalBandwidth,u4PPRatioDiv;
    UINT32 u4Ratio;
    UINT32 u4Cnt = 0;
    UINT32 u4Clk = 0;
    HAL_TIME_T rTimeDt;
    HAL_TIME_T rTimeDecLoop;
    
    u4Cnt = u4VDecReadVLDTOP(u4VDecID,(40 << 2));
    u4Clk = u4ReadReg(CKGEN_VDEC_SYS_CLK);

    if((u4Clk & 0xF) == 0xE)
    {
        if(((u4ReadReg(0x61004) >> 13) & 0x7F) == 0x50)
        {
            u4Clk = 480;
        }
        else if(((u4ReadReg(0x61004) >> 13) & 0x7F) == 0x4B)
        {
            u4Clk = 450;
        }
        else
        {
            u4Clk = 504;
        }
    }
    else if((u4Clk & 0xF) == 0xC)
    {
        u4Clk = 432;
    }
    
    HAL_GetDeltaTime(&rTimeDt, &(prH265DecPrm->rTimeS), &(rTimeE[u4VDecID]));
    HAL_GetDeltaTime(&rTimeDecLoop, &(prH265DecPrm->rTimeRcvES), &(rTimeE[u4VDecID]));
    
    VDecPrintf(ucEsId,VDEC_LOG_DecTime,"Hw dec-time %d Sw-time %d Pic-time %d MC_478 %d u4Clk %d\n",u4Cnt/u4Clk,rTimeDt.u4Micros,rTimeDecLoop.u4Micros,gu4MCcnt[u4VDecID],u4Clk);
    
    if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
    {
        if(prH265DecPrm->ucDualCoreMode)
        {
            u4MCBandwidth = (u4VDecReadMC(VDEC_VLD0,(476 << 2)) + u4VDecReadMC(VDEC_VLD1,(476 << 2))) << 4; //16Byte Unit
            u4PPRatioDiv = (u4VDecReadMC(VDEC_VLD0,(723 << 2)) + u4VDecReadMC(VDEC_VLD1,(723 << 2))) << 4;
            
            u4PPBandwidth = ((((prH265DecPrm->prSPS->u4PicWidthInLumaSamples + 15) >> 4) << 4) * 
                            (((prH265DecPrm->prSPS->u4PicHeightInLumaSamples + 31) >> 5) << 5) * 3) >> 1;

            if(prH265DecPrm->prSPS->u4BitDepthLumaMinus8 == 2)//10bit case
            {
                u4PPBandwidth = ((u4PPBandwidth * 5) >> 2);
            }
        }
        else
        {
            u4MCBandwidth = u4VDecReadMC(u4VDecID,(476 << 2)) << 4; //16Byte Unit
            u4PPRatioDiv = u4VDecReadMC(u4VDecID,(723 << 2)) << 4;
            u4PPBandwidth = ((((prH265DecPrm->prSPS->u4PicWidthInLumaSamples + 15) >> 4) << 4) * 
                            (((prH265DecPrm->prSPS->u4PicHeightInLumaSamples + 31) >> 5) << 5) * 3) >> 1;

            if(prH265DecPrm->prSPS->u4BitDepthLumaMinus8 == 2)//10bit case
            {
                u4PPBandwidth = ((u4PPBandwidth * 5) >> 2);
            }
        }
        
        if(prH265DecPrm->prCurrFBufInfo->bIsUFOEncoded)
        {
            //I frame cann't get MC_476
            if((prH265DecPrm->prSliceHdr->u4SliceType == HEVC_I_SLICE))
            {
                u4Ratio = prH265DecPrm->rLastInfo.u4LastBandwidthRatio;
                u4MCBandwidth = prH265DecPrm->rLastInfo.u4LastMCBandwidth;
                return ;
            }
            
            
            if(prH265DecPrm->prSPS->u4BitDepthLumaMinus8 == 0) //8bit
            {
                u4Ratio = (u4MCBandwidth * 10)/u4PPRatioDiv;
                u4PPBandwidth = (u4PPBandwidth * u4Ratio)/10;
            }
            else //10bit
            {
                u4Ratio = (u4MCBandwidth * 10)/((u4PPRatioDiv * 5) >> 2 );
                u4PPBandwidth = (u4PPBandwidth * u4Ratio)/10;
            }

            prH265DecPrm->rLastInfo.u4LastBandwidthRatio = u4Ratio;
            prH265DecPrm->rLastInfo.u4LastMCBandwidth = u4MCBandwidth;            
        }
        
        u4TotalBandwidth = u4MCBandwidth + u4PPBandwidth;
        VDecPrintf(ucEsId,VDEC_LOG_BANDWIDTH,"[%s-%s] Bandwidth MC %dBps PP %dBps Total %dBps PPRation %d\n",
                                                                        ((prH265DecPrm->ucDualCoreMode) ? "Mcore" : "Score"),
                                                                        ((prH265DecPrm->prCurrFBufInfo->bIsUFOEncoded) ? "UFO" : "NonUFO"),
                                                                        u4MCBandwidth,u4PPBandwidth,u4TotalBandwidth,u4PPRatioDiv); 
    }

}
#endif

