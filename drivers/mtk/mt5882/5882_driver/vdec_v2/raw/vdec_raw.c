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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_raw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_raw.c
 *  This file contains implementation of exported APIs of VDEC.
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_drvif.h"
#include "vdec_raw.h"
#include "fbm_drvif.h"
#include "gfx_if.h"
#include "gfx_drvif.h"
#include "x_assert.h"
#include "x_debug.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

#ifdef VDEC_VENC_PATH
#define RAW_SUPPORT_VENC
#endif
//#define RAW_YUY2 // if define this flag, vdec-raw will queue 1 frame in vfifo for 422 to 420 tmp buf


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define RAW_MAX_WIDTH 1280
#define RAW_MAX_HEIGHT 720


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

VDEC_RAW_INSTANCE_T _arVdecRawInfo[VDEC_MAX_ES];


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static VDEC_RAW_INSTANCE_T *_VdecRawGetInst(UCHAR ucEsId)
{
    if (ucEsId >= (UCHAR)VDEC_MAX_ES)
    {
        LOG(1, "%s: invalid id(%d)\n", ucEsId);
        ASSERT(ucEsId < (UCHAR)VDEC_MAX_ES);
        ucEsId = 0;
    }

    return &_arVdecRawInfo[ucEsId];
}

#ifdef RAW_YUY2
static VOID _VdecRaw422To420(
    UCHAR ucEsId,
    VDEC_PES_INFO_T *prPesInfo,
    UINT8 *pu1DstYAddr,
    UINT8 *pu1DstCbCrAddr,
    UINT32 u4Width,
    UINT32 u4Height)
{
    VDEC_RAW_INSTANCE_T *prVdecRawInfo = _VdecRawGetInst(ucEsId);
    UINT8 *pu1SrcAddr;
    UINT32 u4VldReadPtr;
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4FifoSize;
    UINT32 u4PicSz;
    UINT32 u4Pitch = u4Width * 2; // size of 1 YCbYCr pixel is 2 bytes, so pitch = width * 2

    u4FifoStart = prPesInfo->u4FifoStart;
    u4FifoEnd = prPesInfo->u4FifoEnd;
    u4FifoSize = u4FifoEnd - u4FifoStart;
    u4VldReadPtr = prPesInfo->u4VldReadPtr;
    ASSERT(u4VldReadPtr < u4FifoEnd);
    ASSERT(u4VldReadPtr >= u4FifoStart);

    u4PicSz = u4Width * u4Height * 2;
    if (u4VldReadPtr + u4PicSz > u4FifoEnd)
    {
        UINT32 u4Size1 = u4FifoEnd - u4VldReadPtr;
        UINT32 u4Size2 = u4PicSz - u4Size1;
        UINT32 u4SrcAddr = u4VldReadPtr - u4PicSz;
        ASSERT(u4SrcAddr < u4FifoEnd);
        ASSERT(u4SrcAddr >= u4FifoStart);
        ASSERT(0 == (u4Size1 & 3));
        ASSERT(0 == (u4Size2 & 3));
        GFX_Lock();
        _GFX_DMA((UINT32 *)u4SrcAddr, (UINT32 *)u4VldReadPtr, u4Size1 >> 2);
        _GFX_DMA((UINT32 *)u4SrcAddr + u4Size1, (UINT32 *)u4FifoStart, u4Size2 >> 2);
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
        pu1SrcAddr= (UINT8 *)u4SrcAddr;
    }
    else
    {
        pu1SrcAddr= (UINT8 *)u4VldReadPtr;
    }

    GFX_Lock();
    _GFX_ColorConvert422To420Sep(pu1SrcAddr, pu1DstYAddr, pu1DstCbCrAddr,
        u4Width, u4Height, u4Pitch, 0, 0, 0, 0);
    GFX_Unlock();

    prVdecRawInfo->u4ReadPtr = prPesInfo->u4VldReadPtr;
}
#else
static VOID _VdecRawWriteFB(
    UCHAR ucEsId,
    VDEC_PES_INFO_T *prPesInfo,
    UINT32 u4YAddr,
    UINT32 u4CAddr,
    UINT32 u4YSize)
{
    VDEC_RAW_INSTANCE_T *prVdecRawInfo;
    UINT32 u4VldReadPtr;
    UINT32 u4FifoStart;
    UINT32 u4FifoEnd;
    UINT32 u4FifoSize;
    UINT32 u4GfxDmaSize;
    UINT32 u4LumaSize;
    UINT32 u4ChromaSize;

    prVdecRawInfo = _VdecRawGetInst(ucEsId);

    u4FifoStart = prPesInfo->u4FifoStart;
    u4FifoEnd = prPesInfo->u4FifoEnd;
    u4FifoSize = u4FifoEnd - u4FifoStart;
    u4VldReadPtr = prPesInfo->u4VldReadPtr;
    ASSERT(u4VldReadPtr < u4FifoEnd);
    ASSERT(u4VldReadPtr >= u4FifoStart);

    // copy from V-FIFO to VDEC frame buffer
    GFX_Lock();

    // Write Luma buffer
    u4LumaSize = u4YSize;
    if (u4VldReadPtr + u4LumaSize > u4FifoEnd)
    {
        u4GfxDmaSize = u4FifoEnd - u4VldReadPtr;
        _GFX_DMA((UINT32 *)u4YAddr, (UINT32 *)u4VldReadPtr, u4GfxDmaSize >> 2);
        u4VldReadPtr = u4FifoStart;
        u4LumaSize -= u4GfxDmaSize;
        u4YAddr += u4GfxDmaSize;
    }
    _GFX_DMA((UINT32 *)u4YAddr, (UINT32 *)u4VldReadPtr, u4LumaSize >> 2);
    u4VldReadPtr += u4LumaSize;
    if (u4VldReadPtr >= u4FifoEnd)
    {
        u4VldReadPtr -= u4FifoSize;
    }
    ASSERT(u4VldReadPtr < u4FifoEnd);
    ASSERT(u4VldReadPtr >= u4FifoStart);

    // Write Chroma buffer
    u4ChromaSize = u4YSize >> 1;
    if (u4VldReadPtr + u4ChromaSize > u4FifoEnd)
    {
        u4GfxDmaSize = u4FifoEnd - u4VldReadPtr;
        _GFX_DMA((UINT32 *)u4CAddr, (UINT32 *)u4VldReadPtr, u4GfxDmaSize >> 2);
        u4VldReadPtr = u4FifoStart;
        u4ChromaSize -= u4GfxDmaSize;
        u4CAddr += u4GfxDmaSize;
    }
    _GFX_DMA((UINT32 *)u4CAddr, (UINT32 *)u4VldReadPtr, u4ChromaSize >> 2);
    u4VldReadPtr += u4ChromaSize;
    if (u4VldReadPtr >= u4FifoEnd)
    {
        u4VldReadPtr -= u4FifoSize;
    }
    ASSERT(u4VldReadPtr < u4FifoEnd);
    ASSERT(u4VldReadPtr >= u4FifoStart);

    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();

    prVdecRawInfo->u4ReadPtr = u4VldReadPtr;
}
#endif


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


// *********************************************************************
// Function    : VOID _VDEC_RAWInit
// Description : Initialization procedure of vdec
// Parameter   :
// Return      :
// *********************************************************************
INT32 _VDEC_RAWInit(UCHAR ucEsId)
{
    VDEC_RAW_INSTANCE_T *prVdecRawInfo = _VdecRawGetInst(ucEsId);
    x_memset((VOID*)prVdecRawInfo, 0, sizeof(VDEC_RAW_INSTANCE_T));
    prVdecRawInfo->ucEsId = ucEsId;
    prVdecRawInfo->ucFbgId = (UCHAR)FBM_FBG_ID_UNKNOWN;
    return E_VDEC_OK;
}


// *********************************************************************
// Function    : VOID _VDEC_RAWSWRst
// Description : decoding procedure of vdec
// Parameter   :
// Return      :
// *********************************************************************
INT32 _VDEC_RAWDecode(UCHAR ucMpvId, UCHAR ucEsId)
{
    VDEC_RAW_INSTANCE_T *prVdecRawInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_PES_INFO_T *prPesInfo;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;
    UINT32 u4YAddr, u4CAddr;
    UINT32 u4YSize, u4CSize;
    //UINT32 u4FbWidth, u4FbHeight;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    BOOL fgNotDisplay;

    ASSERT(ucEsId < (UCHAR)VDEC_MAX_ES);
    prVdecRawInfo =  _VdecRawGetInst(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if ((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        return (INT32)E_VDEC_FAIL;
    }

    if ((prVdecEsInfo->rMMSeqInfo.u2_width > (UINT16)RAW_MAX_WIDTH)
        ||(prVdecEsInfo->rMMSeqInfo.u2_height> (UINT16)RAW_MAX_HEIGHT))
    {
           return (INT32)E_VDEC_NOT_SUPPORT;
    }

    if (prVdecRawInfo->ucFbgId != (UINT32)FBM_FBG_ID_UNKNOWN)
    {
        ucFbgId = prVdecRawInfo->ucFbgId;
    }
    else
    {
        UCHAR ucFbgType;

        ASSERT(prVdecEsInfo->rMMSeqInfo.u2_width);
        ASSERT(prVdecEsInfo->rMMSeqInfo.u2_height);
        ucFbgType = FBM_SelectGroupType((UINT32)prVdecEsInfo->rMMSeqInfo.u2_width,(UINT32)prVdecEsInfo->rMMSeqInfo.u2_height);

        {
            FBM_CREATE_FBG_PAR_T rPar;
            x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
            rPar.u1VdecId = (UINT8)ucEsId;
            if(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE)
            {
                rPar.u1AppMode = FBM_FBG_APP_SKYPE;
            }
            else if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
            {
                rPar.u1AppMode = FBM_FBG_APP_OMX;
            }
            else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
            {
                rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
            }
            else
            {
                rPar.u1AppMode = FBM_FBG_APP_NORMAL;
            }

            if (prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                LOG(3, "fgCreateFromInst == TRUE\n");
                rPar.fgCreateFromInst = TRUE;
            }
            if (prVdecEsInfo->u4RenderPicCnt)
            {
                rPar.fgThumbnailMode = TRUE;
            }

            ucFbgId =
                FBM_CreateGroupExt(ucFbgType, FBM_VDEC_RAW, prVdecEsInfo->rMMSeqInfo.u2_width, prVdecEsInfo->rMMSeqInfo.u2_height, &rPar);
        }
        if (ucFbgId == (UINT32)FBM_FBG_ID_UNKNOWN)
        {
            LOG(0, "create FBG failed.\n");
            return (INT32)E_VDEC_FAIL;
        }
        FBM_SetPlayMode(ucFbgId, (UCHAR)FBM_FBG_MM_MODE);
        if(prVdecEsInfoKeep->fgVPush)
        {
            if(prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
            }
            FBM_RegFbgCbFunc(
                ucFbgId,
                FBM_CB_FUNC_FB_READY_EX_IND,
                (UINT32)_VPUSH_DecodeDone,
                ucEsId);
            FBM_RegFbgCbFunc(ucFbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
        }

        FBM_SetSyncStc(ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
        FBM_FbgChgNotify(ucFbgId, ucEsId);
        prVdecEsInfo->ucFbgId = ucFbgId;
        prVdecRawInfo->ucFbgId = ucFbgId;
    }

    if (FBM_ChkFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET))
    {
        FBM_ResetGroup(ucFbgId);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_NOT_READY);
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if (prFbmSeqHdr == NULL)
    {
        LOG(0, "get seq hdr failed.\n");
        return (INT32)E_VDEC_FAIL;
    }

#if 1
// for vdec to venc support, we always use FBM_GetEmptyFrameBuffer
// This will simply modification of FBM
    ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
#else
    ucFbId = FBM_GetEmptyRefFrameBuffer(ucFbgId, (UINT32)1500);
#endif
    if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        LOG(0, "get empty frame buffer failed.\n");
        return (INT32)FALSE;
    }
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4YAddr, &u4CAddr);

    prVdecEsInfo->ucFbId = ucFbId;

    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    if (prFbmPicHdr == NULL)
    {
        LOG(0, "get pic hdr failed.\n");
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
        return (INT32)E_VDEC_FAIL;
    }

    //FBM_GetFrameBufferSize(ucFbgId, &u4FbWidth, &u4FbHeight);

    prFbmSeqHdr->ucAspRatInf = (UCHAR)prVdecEsInfo->rMMSeqInfo.e_src_asp;
    prFbmSeqHdr->ucFrmRatCod = (UCHAR)prVdecEsInfo->rMMSeqInfo.e_frame_rate;
    prFbmSeqHdr->fgProgressiveSeq = TRUE;
    prFbmSeqHdr->fgBrokenLink = FALSE;
    prFbmSeqHdr->fgClosedGop = FALSE;
    prFbmSeqHdr->u2LineSize = prVdecEsInfo->rMMSeqInfo.u2_width;//(UINT16)u4FbWidth;
    prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
    prFbmSeqHdr->u2DHS = prVdecEsInfo->rMMSeqInfo.u2_width;
    prFbmSeqHdr->u2DVS = prVdecEsInfo->rMMSeqInfo.u2_height;
    prFbmSeqHdr->u2OrgHSize = prVdecEsInfo->rMMSeqInfo.u2_width;
    prFbmSeqHdr->u2OrgVSize = prVdecEsInfo->rMMSeqInfo.u2_height;
    prFbmSeqHdr->fgRasterOrder = TRUE;
    prFbmPicHdr->ucPicCdTp = (UCHAR)I_TYPE;
    prFbmPicHdr->ucPicStruct = (UCHAR)3;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    prFbmPicHdr->u4PTS = prVdecEsInfo->u4PTS;

    _VDEC_RAWGetPesStruct(ucEsId, (VOID **)&prPesInfo);
    u4YSize = prFbmSeqHdr->u2HSize * prFbmSeqHdr->u2VSize;
    u4CSize = u4YSize / 2;

    CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prFbmPicHdr->u4PTS, 0, 0);
    if (fgNotDisplay)
    {
#ifdef RAW_YUY2
        prVdecRawInfo->u4ReadPtr = prPesInfo->u4VldReadPtr;
#else
        UINT32 u4FifoStart, u4FifoEnd, u4FifoSize, u4VldReadPtr;

        u4FifoStart = prPesInfo->u4FifoStart;
        u4FifoEnd = prPesInfo->u4FifoEnd;
        u4FifoSize = u4FifoEnd - u4FifoStart;
        u4VldReadPtr = prPesInfo->u4VldReadPtr;
        ASSERT(u4VldReadPtr < u4FifoEnd);
        ASSERT(u4VldReadPtr >= u4FifoStart);
        u4VldReadPtr += u4YSize + u4CSize;
        if (u4VldReadPtr >= u4FifoEnd)
        {
            u4VldReadPtr -= u4FifoSize;
        }
        ASSERT(u4VldReadPtr < u4FifoEnd);
        ASSERT(u4VldReadPtr >= u4FifoStart);
        prVdecRawInfo->u4ReadPtr = u4VldReadPtr;
#endif

        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
    }
    else
    {
#ifdef RAW_YUY2
        _VdecRaw422To420(ucEsId, prPesInfo, (UINT8*)u4YAddr, (UINT8*)u4CAddr,
            prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize);
#else
        _VdecRawWriteFB(ucEsId, prPesInfo, u4YAddr, u4CAddr, u4YSize);
#endif

#ifdef RAW_SUPPORT_VENC
        if (_VDEC_fg2VencFlowOn(ucEsId))
        {
            VDEC_VENC_SRC_INFO_T rVdec2VencSrcInfo;
            rVdec2VencSrcInfo.u4YAddr = u4YAddr;
            rVdec2VencSrcInfo.u4CAddr = u4CAddr;
            rVdec2VencSrcInfo.u4Pitch = prFbmSeqHdr->u2LineSize;
            rVdec2VencSrcInfo.u4Width = prFbmSeqHdr->u2HSize;
            rVdec2VencSrcInfo.u4Height = prFbmSeqHdr->u2VSize;
            rVdec2VencSrcInfo.u4Pts = prFbmPicHdr->u4PTS;
            if (!_VDEC_PrepareVEncSourse(ucEsId, &rVdec2VencSrcInfo))
            {
            }
        }
#endif
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
        prVdecEsInfo->u4DisplayQPicCnt++;
        VDEC_CheckStopStatus(ucEsId, prFbmPicHdr->u4PTS, 0);
    }

    return E_VDEC_OK;
}


// *********************************************************************
// Function    : VOID _VDEC_RAWStop
// Description : stopping procedure of vdec
// Parameter   :
// Return      :
// *********************************************************************
VOID _VDEC_RAWStop(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_RAW_INSTANCE_T *prVdecRawInfo;
    UCHAR ucFbgId;

    ASSERT(ucEsId < (UCHAR)VDEC_MAX_ES);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecRawInfo =  _VdecRawGetInst(ucEsId);
    ucFbgId = prVdecRawInfo->ucFbgId;

    if (ucFbgId != (UCHAR)FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(ucFbgId, (UINT32)20);    // block until display unlock
        FBM_ReleaseGroup(ucFbgId);
        prVdecEsInfo->ucFbgId = (UCHAR)FBM_FBG_ID_UNKNOWN;
        prVdecRawInfo->ucFbgId = (UCHAR)FBM_FBG_ID_UNKNOWN;
    }
}


// *********************************************************************
// Function    : VOID _VDEC_RAWStop
// Description : Get the Pes Struct for using vdec
// Parameter   :
// Return      : OK: success
// *********************************************************************
VOID _VDEC_RAWGetPesStruct(UCHAR ucEsId, VOID **ppvPesInfo)
{
    if (ppvPesInfo)
    {
        VDEC_RAW_INSTANCE_T *prVdecRawInfo = _VdecRawGetInst(ucEsId);
        *ppvPesInfo = (VOID *)&prVdecRawInfo->rPesInfo;
    }
}


// *********************************************************************
// Function    : VOID _VDEC_RAWUpdDmxRptr
// Description : update dmx read pointer for using vdec
// Parameter   :
// Return      :
// *********************************************************************
void _VDEC_RAWUpdDmxRptr(UCHAR ucEsId)
{
    VDEC_RAW_INSTANCE_T *prVdecRawInfo;
    UINT32 u4CurRptr;

    ASSERT(ucEsId < (UCHAR)VDEC_MAX_ES);
    prVdecRawInfo = _VdecRawGetInst(ucEsId);
    u4CurRptr = prVdecRawInfo->u4ReadPtr;

    //update dmx pointer
    if (!DMX_UpdateVideoReadPointer(
        (UINT8)ucEsId, u4CurRptr, u4CurRptr))
    {
        LOG(0, "RAW ES(%d) DMX_UpdateVideoReadPointer Fail 0x%08x\n",
            ucEsId, u4CurRptr);
    }
}


// *********************************************************************
// Function    : VOID _VDEC_RAWEsFlwCtrl
// Description : skip one frame
// Parameter   :
// Return      :
// *********************************************************************
BOOL _VDEC_RAWEsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    static UINT8 u1SkipCnt = 0;
    VDEC_RAW_INSTANCE_T *prVdecRawInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    UCHAR ucFbgId;
    BOOL fgSkip = FALSE;

    ASSERT(ucEsId < (UCHAR)VDEC_MAX_ES);
    prVdecRawInfo = _VdecRawGetInst(ucEsId);
    ucFbgId = prVdecRawInfo->ucFbgId;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if (ucFbgId == (UCHAR)FBM_FBG_ID_UNKNOWN)
    {
        return FALSE;
    }

    if (prVdecEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    if (FBM_ChkFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC)
        && (ucSkipMode != (UCHAR)IMode))
    {
        fgSkip = TRUE;
        if (u1SkipCnt > (UINT8)5)
        {
            fgSkip = FALSE;
            u1SkipCnt = 0;
        }

        if (fgSkip)
        {
            VDEC_PES_INFO_T *prPesInfo;
            _VDEC_RAWGetPesStruct(ucEsId, (VOID **)&prPesInfo);
            //update dmx
            if (!DMX_UpdateVideoReadPointer(
                (UINT8)prPesInfo->ucEsId,
                prPesInfo->u4VldReadPtr,
                prPesInfo->u4VldReadPtr))
            {
                LOG(0, "EsFlwCtrl RAW ES(%d) DMX_UpdateVideoReadPointer Fail 0x%08x\n",
                    prPesInfo->ucEsId, prPesInfo->u4VldReadPtr);
            }

            FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC);
            u1SkipCnt++;

            return TRUE;
        }
    }
    u1SkipCnt = 0;

    return FALSE;
}


// *********************************************************************
// Function    : BOOL _VDEC_IsSupportRAW
// Description :
// Parameter   :
// Return      :
// *********************************************************************
BOOL _VDEC_IsSupportRAW(VOID)
{
    return TRUE;
}


