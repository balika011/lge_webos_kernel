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
 * $RCSfile: vdec_wmvhw.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_wmvhw.c
 *  This header file declares exported APIs of wmv.
 */

#include "x_lint.h"
#include "vdec_wmvpsr.h"
#include "vdec_wmvhw.h"
#include "vdec_wmvutil.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"

#ifdef USE_FBM
#include "fbm_drvif.h"
#endif

LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END


#ifndef CC_WMV_EMULATION
BOOL _fgWmvRestore = TRUE;
#else
BOOL _fgWmvRestore = FALSE;
#endif

BOOL _fgWmvFwBp = FALSE;  // set this to enable software bit-plane decoding


EXTERN void FBM_SetPlayMode(UCHAR ucFbgId, UCHAR ucPlayMode);

VOID* vWMVGetHdlr(UCHAR ucEsId)
{
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    return (VOID*)prWMVDecInfo[ucEsId];
#else
    return (VOID*)&rWMVDecInfo[ucEsId];
#endif
}


void WriteREG(UINT32 address, UINT32 value)
{
    _VDEC_CheckLockStatus(VLD0);

    if (FALSE)
    {
        UINT32 u4Offset;
        u4Offset = (address & 0xFFF);
        switch (address & 0xFFFFF000)
        {
        case MC_REG_OFST:
            LOG(0,"MC_Write(%08x,%08x)\n", (u4Offset), (value));
            break;
        case VLD_REG_OFST:
            LOG(0,"VLD_Write(%08x,%08x)\n", u4Offset, value);
            break;
        default:
            LOG(0,"WriteREG(%08x,%08x)\n", address, value);
            break;
        }
    }
    HAL_WRITE32((address), (value));

}

UINT32 ReadREG(UCHAR ucEsId,UINT32 address)
{
    VDEC_WMV_INFO_T* prVdec = NULL;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "ReadREG : ucEsId >= MAX_ES_NUM");
        return 0;
    }

    _VDEC_CheckLockStatus(VLD0);

    if (prVdec->rOther.fgDumpSimPat && FALSE)
    {
        UINT32 u4Offset;

        u4Offset = (address & 0xFFF)>>2;
        switch (address & 0xFFFFF000)
        {
        case MC_REG_OFST:
            LOG(0, "RISCRead_MC(%d);\n", u4Offset);
            break;
        case VLD_REG_OFST:
            LOG(0, "RISCRead_VLD(%d);\n", u4Offset);
            break;
        default:
            LOG(0, "ReadREG(%d);\n", address);
            break;
        }
    }
    return (HAL_READ32((address)));
}

VOID vInitVDec(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec =NULL;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vInitVDec : ucEsId >= MAX_ES_NUM");
        return;
    }
    prVdec->rOther.u1DecState = VDEC_NORM_INIT_PRM;
    prVdec->rOther.u1GetSeqHdr = 0;
    //prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
    vResetVDec(ucEsId);
    // initiate register files

    //vInitVdoMem(ucEsId);

    //vInitWMVMiscMem(ucEsId);
    vSetMcBufPtr(ucEsId);
    // vResetVDec(ucEsId);
    vSetVldVFifo(ucEsId);
    vSetFRefBuf(ucEsId,0);
    vSetBRefBuf(ucEsId,1);
}

VOID vInitVdoMem(UCHAR ucEsId)
{
    UINT8 u1FbgType;
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
    UINT32 u4FbWidth, u4FbHeight;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    }
    else
    {
        LOG(0, "vInitVdoMem : ucEsId >= MAX_ES_NUM");
        return;
    }

    //prVdec->rDram.pu1VFifo = (UINT8 *)V_FIFO_SA;
    prVdec->rDram.pu1VFifo = (UINT8 *)prVdec->rPesInfo.u4FifoStart;
#ifndef USE_FBM
    prVdec->rDram.pu1Pic0Y = (UINT8 *)PIC0Y_SA; //byte address
    prVdec->rDram.pu1Pic0C = (UINT8 *)PIC0C_SA;
    prVdec->rDram.pu1Pic1Y = (UINT8 *)PIC1Y_SA;
    prVdec->rDram.pu1Pic1C = (UINT8 *)PIC1C_SA;
    prVdec->rDram.pu1Pic2Y = (UINT8 *)PIC2Y_SA;
    prVdec->rDram.pu1Pic2C = (UINT8 *)PIC2C_SA;
#else
    //FBM_ReleaseGroup(prVdec->rOther.u1FbgId);
    //UCHAR ucFbgType = FBM_SelectGroupType(prVdec->rSeqHdrR.u4PicWidthSrc, prVdec->rSeqHdrR.u4PicHeightSrc);
    if (FBM_ChkFrameBufferFlag(prVdec->rOther.u1FbgId, FBM_FLAG_RELEASE_FBG))
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(prVdec->rOther.u1FbgId, 20);    // block until display unlock
        FBM_ReleaseGroup(prVdec->rOther.u1FbgId);
        prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
    }

    //u1FbgType = FBM_SelectGroupType(prDecPrm->u4DramWidth, prDecPrm->u4DramHeight);
    //LOG(0,"hello it's the resolution : %dx%d\n",prVdec->rSeqHdrR.u4PicWidthSrc,prVdec->rSeqHdrR.u4PicHeightSrc);
#ifdef ENABLE_MULTIMEDIA

    if (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_NETWORK_NETFLIX && !prVdecEsInfo->fgSeamlessPlay)
    {
        u1FbgType = FBM_SelectGroupType(prVdec->rSeqHdrR.u4PicWidthSrc, prVdec->rSeqHdrR.u4PicHeightSrc);
    }
    else
    {
        u1FbgType = FBM_SelectGroupType(1920,1088);
    }
#endif
    prVdec->rSeqHdrR.u4PrevPicWidthSrc = prVdec->rSeqHdrR.u4PicWidthSrc;
    prVdec->rSeqHdrR.u4PrevPicHeightSrc = prVdec->rSeqHdrR.u4PicHeightSrc;

    if (prVdec->rOther.u1FbgId == FBM_FBG_ID_UNKNOWN)
    {
#if defined(IS_WMV_POST_MT5368)
        // 5368 and later supports larger resolution
        // ToDo: add maximum resolution checking for 5368
        if (prVdec->rSeqHdrR.u4PicWidthSrc > 3840 || prVdec->rSeqHdrR.u4PicHeightSrc > 2160)
        {
            prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
        }
        else
#else
        // 5395 and prior supports up to 2048 x 2048
        if (prVdec->rSeqHdrR.u4PicWidthSrc > 2048 || prVdec->rSeqHdrR.u4PicHeightSrc > 2048)
        {
            prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
        }
        else
#endif
        {
            {
                FBM_CREATE_FBG_PAR_T rPar;
                x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
                rPar.u1VdecId = (UINT8)ucEsId;
            if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
            {
                    rPar.u1AppMode = FBM_FBG_APP_OMX;
            }
            else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
            {
                    rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
            }
            else if(prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM)
            {
                    rPar.u1AppMode = FBM_FBG_APP_Frame_Pack_3D;
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

            prVdec->rOther.u1FbgId =
                    FBM_CreateGroupExt(u1FbgType, FBM_VDEC_VC1, prVdec->rSeqHdrR.u4PicWidthSrc, prVdec->rSeqHdrR.u4PicHeightSrc, &rPar);
            }
        }
        if (prVdec->rOther.u1FbgId != FBM_FBG_ID_UNKNOWN)
        {
#if !defined(CC_WMV_EMULATION)
  #if !defined(IS_WMV_POST_MT5368)
            // 5395 and prior supports H/W bit-plane decoding up to height of 1088
            if (prVdec->rSeqHdrR.u4PicHeightSrc > 1088)
            {
                // use software bit-plane decoding
                _fgWmvRestore = FALSE;
                _fgWmvFwBp = TRUE;
            }
            else
            {
                // use hardware bit-plane decoding
                _fgWmvRestore = TRUE;
                _fgWmvFwBp = FALSE;
            }
  #endif
#endif
#ifdef __MODEL_slt__
            _fgWmvRestore = FALSE;
#endif
            FBM_SetPlayMode(prVdec->rOther.u1FbgId,FBM_FBG_MM_MODE);
            if(prVdecEsInfoKeep->fgVPush)
            {
                if(prVdecEsInfoKeep->fgVPushDecodeOnly)
                {
                    FBM_SetPlayMode(prVdec->rOther.u1FbgId,FBM_FBG_NOTDISPLAY_MODE);
                }
                FBM_RegFbgCbFunc(prVdec->rOther.u1FbgId,
                    FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prVdec->rOther.u1FbgId,
                    FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
                }
            }
            //ASSERT(prVdec->rOther.u1FbgId != FBM_FBG_ID_UNKNOWN);
            prVdecEsInfo->ucFbgId = prVdec->rOther.u1FbgId;
            FBM_SetSyncStc(prVdec->rOther.u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prVdec->rOther.u1FbgId, &u4FbWidth, &u4FbHeight);
            prVdec->rOther.u4LineSize = u4FbWidth;
            FBM_FbgChgNotify(prVdec->rOther.u1FbgId, ucEsId);
            FBM_SetFrameBufferFlag(prVdec->rOther.u1FbgId, FBM_FLAG_SEQ_CHG);            
            //prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
            //prDecPrm->u4LineSize = u4FbWidth;
            //prDecPrm->u1FbgType = u1FbgType;
        }
    }
    else
    {
        /*
        if (u1FbgType != prDecPrm->u1FbgType)
        {
            LOG(3, "Fbg Type Change %d\n", u1FbgType);
            FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);

            FBM_WaitUnlockFrameBuffer(prDecPrm->u1FbgId,20);    // block until display unlock

            FBM_ReleaseGroup(prDecPrm->u1FbgId);
            prDecPrm->u1FbgId = FBM_FBG_ID_UNKNOWN;

            prDecPrm->u1FbgId = FBM_CreateGroup(u1FbgType, FBM_VDEC_MPEG4, 0, 0);
            ASSERT(prDecPrm->u1FbgId != FBM_FBG_ID_UNKNOWN);
            prVdecEsInfo->ucFbgId = prDecPrm->u1FbgId;
            FBM_SetSyncStc(prDecPrm->u1FbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
            FBM_GetFrameBufferSize(prDecPrm->u1FbgId, &u4FbWidth, &u4FbHeight);
              FBM_FbgChgNotify(prDecPrm->u1FbgId, ucEsId);
              prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->u1FbgId);
              prDecPrm->u4LineSize = u4FbWidth;
        }
        */
    }
    //ucFbgType = FBM_SelectGroupType(1280, 720);
    //prVdec->rOther.u1FbgId = FBM_CreateGroup(ucFbgType, 2,0,0);
    //FBM_FbgChgNotify(prVdec->rOther.u1FbgId, ucEsId);
#endif
    vInitWMVMiscMem(ucEsId);
}

VOID vInitWMVMiscMem(UCHAR ucEsId)
{
#if 0
    UINT32 u4PPSize = (((720/16)*9*8)*2);
    UINT32 u4DCACSize = 7*16*(720/16);  //(720*4);  new mode uses larger!!!
    UINT32 u4MV1Size = (720/16)*(576/16)*16;
    UINT32 u4MV23Size = (720);
    UINT32 u4BitplaneSize = (576);
#else
#if defined(IS_WMV_POST_MT5368)
    UINT32 u4PPSize = (((3840/16)*9*8)*2);
    UINT32 u4DCACSize = 7*16*(3840/16);  //(1920*4);  new mode uses larger!!!
    UINT32 u4MV1Size = (3840/16)*(2176/16)*16;
    UINT32 u4MV23Size = (3840);
    UINT32 u4BitplaneSize = (2176*2);  // *2: new bp mode when width > 2048!!!
#else
    UINT32 u4PPSize = (((2048/16)*9*8)*2);
    UINT32 u4DCACSize = 7*16*(2048/16);  //(1920*4);  new mode uses larger!!!
    UINT32 u4MV1Size = (2048/16)*(2048/16)*16;
    UINT32 u4MV23Size = (2048);
    UINT32 u4BitplaneSize = (2048);
#endif
#endif
    UINT32 u4WorkingSize = 0;
    UINT32 u4WorkingBuffer = 0;
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    }
    else
    {
        LOG(0, "vInitWMVMiscMem : ucEsId >= MAX_ES_NUM");
        return;
    }
#ifdef USE_FBM
  #if 1
    if (prVdec->rOther.u1FbgId != FBM_FBG_ID_UNKNOWN)
    {
        #if 0
        if (prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
        {
            // do nothing since the working buffer is not available
            //u4WorkingBuffer = u4WorkingSize = 0;
            LOG(3, "vInitWMVMiscMem: Parsing info only!!!\n");
        }
        else
        #endif
        {
            FBM_GetWorkingBuffer(prVdec->rOther.u1FbgId,&u4WorkingBuffer,&u4WorkingSize);
            ASSERT(u4WorkingSize > 0);
        }
    }
    //u4WorkingBuffer = DCAC_SA;  5365/5395 emulation test code, only for reference
  #else
    u4WorkingSize = u4DCACSize * 2 + u4MV1Size * 2 + u4MV23Size * 2 + u4BitplaneSize * 4 + u4PPSize * 2;
    u4WorkingSize = (u4WorkingSize + 1023) / 1024 * 1024;
    u4WorkingBuffer = PHYSICAL((UINT32)BSP_AllocAlignedDmaMemory(u4WorkingSize, 1024));
    ASSERT(u4WorkingBuffer);
  #endif
    if (prVdec->rOther.u1FbgId == FBM_FBG_ID_UNKNOWN)
    {
        return;
    }
    else if (u4WorkingBuffer != 0 && u4WorkingSize != 0)
    {
        prVdec->rDram.pu1Dcac = (UINT8*)u4WorkingBuffer;
        prVdec->rDram.pu1Mv_1 = prVdec->rDram.pu1Dcac + u4DCACSize;
        prVdec->rDram.pu1Mv_2 = prVdec->rDram.pu1Mv_1 + u4MV1Size;
        prVdec->rDram.pu1Bp_1 = prVdec->rDram.pu1Mv_2 + u4MV23Size;
        prVdec->rDram.pu1Bp_2 = prVdec->rDram.pu1Bp_1 + u4BitplaneSize;
        prVdec->rDram.pu1Bp_3 = prVdec->rDram.pu1Bp_2 + u4BitplaneSize;
        prVdec->rDram.pu1Bp_4 = prVdec->rDram.pu1Bp_3 + u4BitplaneSize;
        prVdec->rDram.pu1Mv_3 = prVdec->rDram.pu1Bp_4 + u4BitplaneSize;
        prVdec->rDram.pu1Mv_1_2 = prVdec->rDram.pu1Mv_3 + u4MV23Size;
        prVdec->rDram.pu1Dcac_2 = prVdec->rDram.pu1Mv_1_2 + u4MV1Size;
        prVdec->rDram.pu1Pp_1 = prVdec->rDram.pu1Dcac_2 + u4DCACSize;
        prVdec->rDram.pu1Pp_2 = prVdec->rDram.pu1Pp_1 + u4PPSize;
        if ((UINT32)prVdec->rDram.pu1Pp_2 + u4PPSize - (UINT32)(prVdec->rDram.pu1Dcac) > u4WorkingSize)
        {
            LOG(0, "VDEC #%d working buffer size needed:%d, but only %d got!!!!!!!!!!!!\n",
                       ucEsId,
                       (UINT32)prVdec->rDram.pu1Pp_2 + u4PPSize - (UINT32)(prVdec->rDram.pu1Dcac),
                       u4WorkingSize / 2);
            ASSERT(0);
        }
        #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
        FBM_WorkBufPartMap(prVdec->rOther.u1FbgId, (UINT32)prVdec->rDram.pu1Bp_1, u4BitplaneSize*3);
        #endif
    }
#else
#ifdef CC_WMV_EMULATION
    u4WorkingBuffer = DCAC_SA;
    prVdec->rDram.pu1Dcac = (UINT8*)u4WorkingBuffer;
    prVdec->rDram.pu1Mv_1 = prVdec->rDram.pu1Dcac + u4DCACSize;
    prVdec->rDram.pu1Mv_2 = prVdec->rDram.pu1Mv_1 + u4MV1Size;
    prVdec->rDram.pu1Bp_1 = prVdec->rDram.pu1Mv_2 + u4MV23Size;
    prVdec->rDram.pu1Bp_2 = prVdec->rDram.pu1Bp_1 + u4BitplaneSize;
    prVdec->rDram.pu1Bp_3 = prVdec->rDram.pu1Bp_2 + u4BitplaneSize;
    prVdec->rDram.pu1Bp_4 = prVdec->rDram.pu1Bp_3 + u4BitplaneSize;
    prVdec->rDram.pu1Mv_3 = prVdec->rDram.pu1Bp_4 + u4BitplaneSize;
    prVdec->rDram.pu1Mv_1_2 = prVdec->rDram.pu1Mv_3 + u4MV23Size;
    prVdec->rDram.pu1Dcac_2 = prVdec->rDram.pu1Mv_1_2 + u4MV1Size;
    prVdec->rDram.pu1Pp_1 = prVdec->rDram.pu1Dcac_2 + u4DCACSize;
    prVdec->rDram.pu1Pp_2 = prVdec->rDram.pu1Pp_1 + u4PPSize;
    //prVdec->rDram.pu1Dcac = (UINT8 *)DCAC_TEST_SA;
#else
    prVdec->rDram.pu1Dcac = (UINT8 *) BSP_AllocAlignedDmaMemory(u4DCACSize,16);//BSP_AllocAlignedDmaMemory(0x1f00,16);
    prVdec->rDram.pu1Mv_1 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4MV1Size,16);
    prVdec->rDram.pu1Mv_2 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4MV23Size,16);
    prVdec->rDram.pu1Bp_1 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4BitplaneSize,16);
    prVdec->rDram.pu1Bp_2 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4BitplaneSize,16);
    prVdec->rDram.pu1Bp_3 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4BitplaneSize,16);
    prVdec->rDram.pu1Bp_4 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4BitplaneSize,16);
    prVdec->rDram.pu1Mv_3 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4MV23Size,16);
    prVdec->rDram.pu1Mv_1_2 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4MV23Size,16);
    prVdec->rDram.pu1Dcac_2 = (UINT8 *) BSP_AllocAlignedDmaMemory(u4DCACSize,16);
    prVdec->rDram.pu1Pp_1 = (UINT8*) BSP_AllocAlignedDmaMemory(u4PPSize,16);
    prVdec->rDram.pu1Pp_2 = (UINT8*) BSP_AllocAlignedDmaMemory(u4PPSize,16);
    ASSERT(prVdec->rDram.pu1Dcac);
    ASSERT(prVdec->rDram.pu1Mv_1);
    ASSERT(prVdec->rDram.pu1Mv_2);
    ASSERT(prVdec->rDram.pu1Bp_1);
    ASSERT(prVdec->rDram.pu1Bp_2);
    ASSERT(prVdec->rDram.pu1Bp_3);
    ASSERT(prVdec->rDram.pu1Bp_4);
    ASSERT(prVdec->rDram.pu1Mv_3);
    ASSERT(prVdec->rDram.pu1Mv_1_2);
    ASSERT(prVdec->rDram.pu1Dcac_2);
    ASSERT(prVdec->rDram.pu1Pp_1);
    ASSERT(prVdec->rDram.pu1Pp_2);
#endif
#endif

    //prVdec->rDram.pu1Pp_1 = (UINT8*) BSP_AllocAlignedDmaMemory(u4PPSize,16);
    //prVdec->rDram.pu1Pp_2 = (UINT8*) BSP_AllocAlignedDmaMemory(u4PPSize,16);

    vInitPred(ucEsId);
}

VOID vInitVld(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vInitVld : ucEsId >= MAX_ES_NUM");
        return;
    }
    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        //Robert: Enable 0x03 replancement before initializing barrel shifter.
        WriteREG(RW_VLD_WMV_ABS, 0x1);
    }
    else
    {
        WriteREG(RW_VLD_WMV_ABS, 0x0);
    }
    WriteREG(RW_VLD_VSTART, PHYSICAL((UINT32)prVdec->rDram.pu1VFifo)>>6); // (128-bit address >> 6)
    //spork
    WriteREG(RW_VLD_VEND, PHYSICAL((UINT32)prVdec->rDram.pu1FifoEnd)>>6);
    //fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo);  // megaa 20100603: no need

    // ilii, clean register
    WriteREG(RW_VLD_DECSTART, 1<<8);
    WriteREG(RW_VLD_DECSTART, 0);
    WriteREG(RW_VLD_VDOUFM, 1);
#if defined(IS_WMV_POST_MT5368)
    WriteREG(RW_VLD_TOP_OUTRANGE_MODE, 0x80000000);
#endif
}

VOID vInitPred(UCHAR ucEsId)
{
    UINT32 a194, a198, a201, a202;
    VDEC_WMV_INFO_T* prVdec = NULL;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vInitPred : ucEsId >= MAX_ES_NUM");
        return;
    }
    WriteREG(RW_VLD_DCACSA, PHYSICAL((UINT32)prVdec->rDram.pu1Dcac) >> 2); //192 DCAC
    a194 = (((UINT32)prVdec->rDram.pu1Mv_1 >> 2 ) & 0x003FFFFF);
    WriteREG(RW_VLD_MVSA, PHYSICAL(a194)); //194 Mv_1#1(frame/top field)
    a202 = (((UINT32)prVdec->rDram.pu1Mv_1 >> 24) & 0x3F);
    WriteREG(VLD_REG_202, PHYSICAL(a202)); //202
    // ilii

    //MPEG-4 use    WriteREG(RW_VLD_BMB1, 0x2d124f80); //196 set once
    //MPEG-4 use    WriteREG(RW_VLD_BMB2, 0x2d13d620); //197 set once
    a198 = (((UINT32)prVdec->rDram.pu1Bp_1 >> 2) & 0x00FFFFFF)
           + (((UINT32)prVdec->rDram.pu1Bp_1 << 2) & 0xF0000000)
           + (prVdec->rOther.i4pred_use_wdle << 25) + //wdle
           ( 1 << 26);
    WriteREG(RW_VLD_BCODE_SA, a198); //(UINT32)prVdec->rDram.pu1Bp_1 >> 3); //198 Bp_1
    // ilii
    //MPEG-4 use    WriteREG(RW_VLD_DIRE_MD_IL, 0x00000000); //199 set once
    a201 =0x00900002 + (1 << 16) + //ming add for test
          (((prVdec->rSeqHdrR.u4NumMBX == 1) ? 1 : 0) << 22);/* 0x00100000 +
        (prVdec->rOther.i4pred_use_wdle << 20)+
        (prVdec->rOther.i4pred_use_wdle << 1 );*/
    if (prVdec->rOther.i4CodecVersion == WMV1
            || prVdec->rOther.i4CodecVersion == WMV2)
    {
        LOG(6, "setting a201 for wmv7 and wmv8\n");
        a201 += ((1<<28) + (1<<29)); //turn of the wmv7,wmv8 timeout bug
    }
    WriteREG(RW_VLD_MBDRAM_SEL, a201); //201 set once
    WriteREG(RW_VLD_MV3_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Mv_3 >> 2)); //214 Mv_3 for new RTL
    WriteREG(RW_VLD_BP2_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_2 >> 2)); //203 Bp_2
    WriteREG(RW_VLD_BP3_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_3 >> 2)); //204 Bp_3
    WriteREG(RW_VLD_BP4_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_4 >> 2)); //205 Bp_4
    WriteREG(RW_VLD_MV2_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Mv_2 >> 2)); //211 Mv_2
    WriteREG(RW_VLD_DCAC2_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Dcac_2 >> 2)); //213 Dcac2
    //WriteREG(RW_VLD_MV2_ADDR, 0x00011170); //211 Mv_2
    //WriteREG(RW_VLD_MVSA, 0x00013880); //194 Mv_1#2(bottom field)

    //MC post process
    WriteREG(RW_MC_PP_DBLK_Y_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Pp_1 >> 4));
    WriteREG(RW_MC_PP_DBLK_C_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Pp_2 >> 4));
}

VOID vResetVDec(UCHAR ucEsId)
{
#ifdef TEST_ONOFF_RUNTIME
    VDEC_INFO_T *prVdecInfo = NULL;
    prVdecInfo = _VDEC_GetInfo();
    prVdecInfo->arVdecVldInfo[0].fgSwrst = TRUE;
#endif
#ifdef WAIT_SRAM_STABLE
#if !defined(IS_WMV_POST_MT5368)
    UINT32 u4Tmp = 0;
#endif
#endif
#if defined(IS_WMV_POST_MT5368) || defined(CC_DYNAMIC_POWER_ONOFF)
    VDEC_WMV_INFO_T* prVdec;

    if (ucEsId < MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        ASSERT(0);
        return;
    }
#endif

#ifdef WAIT_SRAM_STABLE
#if !defined(IS_WMV_POST_MT5368)
    while ((ReadREG(ucEsId,RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
    {
        u4Tmp++;
#ifdef __MODEL_slt__
        if (u4Tmp >= 0x10000)
            break;
#endif
        if (u4Tmp >= 0x1000)
        {
            //indicate that this is first time hardware reset
            WriteREG(WO_VLD_SRST, 1);
            WriteREG(WO_VLD_SRST, 0);
            u4Tmp = 0;
            //ASSERT(0);
        }
    }
#endif  // IS_WMV_POST_MT5368
#endif  // WAIT_SRAM_STABLE

#if defined(IS_WMV_POST_MT5368)
    WriteREG(WO_VLD_SRST, 0x101);

    _VDEC_CkgenSel(ucEsId, VDEC_FMT_WMV, 0);

#ifdef CC_DYNAMIC_POWER_ONOFF
#if defined(VDEC_IS_POST_MT5882)
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        WriteREG(WO_PDN_CON_SPEC, PDN_CON_SPEC_WMV_E3);        
    }
    else
#endif        
    {
        WriteREG(WO_PDN_CON_SPEC, PDN_CON_SPEC_WMV);    
    }

    if (prVdec->rOther.i4CodecVersion == WMVA || prVdec->rOther.i4CodecVersion == WMV3)
    {
        WriteREG(WO_PDN_CON_MODULE, PDN_CON_MODULE_W9A);
    }
    else
    {
        WriteREG(WO_PDN_CON_MODULE, PDN_CON_MODULE_W78);
    }

#if defined(VDEC_IS_POST_MT5882)
    if(BSP_GetIcVersion() == IC_VER_5890_AC)
    {
        WriteREG(WO_PDN_CON_SPEC2, PDN_CON_SPEC_94_E3);
    }
    else
    {
        WriteREG(WO_PDN_CON_SPEC2, PDN_CON_SPEC_94);
    }
#endif

#endif

    _VDEC_SysClkSel(ucEsId, VDEC_FMT_WMV, 0);

#else   // IS_WMV_POST_MT5368
    WriteREG(WO_VLD_SRST, 1);
#endif  // IS_WMV_POST_MT5368

    WriteREG(WO_VLD_SRST, 0);

#if defined(IS_WMV_POST_MT5368) || defined(CC_DYNAMIC_POWER_ONOFF)
    UNUSED(prVdec);
#endif
}

VOID vSetVldVFifo(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vSetVldVFifo : ucEsId >= MAX_ES_NUM");
        return;
    }

    #if defined(CC_MT5399) || defined(VDEC_IS_POST_MT5881)
	WriteREG(RW_VLD_WMV_MODE, 0x00000001);
	#endif

    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        //Robert: Enable 0x03 replancement before initializing barrel shifter.
        WriteREG(RW_VLD_WMV_ABS, 0x1);
        //cbZeroThree->Checked = true;
    }
    else
    {
        WriteREG(RW_VLD_WMV_ABS, 0x0);
        //cbZeroThree->Checked = false;
    }
    WriteREG(RW_VLD_VSTART, (UINT32)prVdec->rDram.pu1VFifo >> 6);
    //  WriteREG(RW_VLD_VEND, (UINT32)(prVdec->rDram.pu1VFifo + V_FIFO_SZ) >> 6);
    //WriteREG(RW_VLD_VEND, (UINT32)0xffffffff);
    //spork
    WriteREG(RW_VLD_VEND, (UINT32)prVdec->rDram.pu1FifoEnd>>6);

    //ilii, refer to Video Fifo setting, fifo should be ring, should be implement in driver implement.
    //prVdec->rOther.u4WPtr = 0xFFFFFFFF;
    //WriteREG(WO_VLD_VWPTR, (UINT32)(0xFFFFFFFF)); // max wptr
    //prVdec->rOther.u4Datain = ReadREG(ucEsId,RO_VLD_VWPTR); // max wptr
}

VOID vSetMcBufPtr(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vSetMcBufPtr : ucEsId >= MAX_ES_NUM");
        return;
    }
    WriteREG(RW_MC_R1Y, PHYSICAL((UINT32)prVdec->rDram.pu1Pic0Y) >> 9); // div 512
    WriteREG(RW_MC_R1C, PHYSICAL((UINT32)prVdec->rDram.pu1Pic0C) >> 8); // div 256
    WriteREG(RW_MC_R2Y, PHYSICAL((UINT32)prVdec->rDram.pu1Pic1Y) >> 9); // div 512
    WriteREG(RW_MC_R2C, PHYSICAL((UINT32)prVdec->rDram.pu1Pic1C) >> 8); // div 256
    WriteREG(RW_MC_BY, PHYSICAL((UINT32)prVdec->rDram.pu1Pic2Y)  >> 8); // div 256
    WriteREG(RW_MC_BC, PHYSICAL((UINT32)prVdec->rDram.pu1Pic2C)  >> 7); // div 128
    WriteREG(RW_MC_BY1, PHYSICAL((UINT32)prVdec->rDram.pu1Pic2Y) >> 9); // div 256
    WriteREG(RW_MC_BC1, PHYSICAL((UINT32)prVdec->rDram.pu1Pic2C) >> 8); // div 128
}

VOID vSetVSyncPrmBufPtr(UCHAR ucEsId,UINT32 dwBufIdx)
{
    UCHAR ucFbId =0 ;
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    VDEC_WMV_INFO_T* prVdec = NULL;
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vSetVSyncPrmBufPtr : ucEsId >= MAX_ES_NUM");
        return;
    }
    switch (dwBufIdx)
    {
    case 0:
        prVdec->rOther.rVSyncPrm.dwPtrY = (UINT32)prVdec->rDram.pu1Pic0Y;
        prVdec->rOther.rVSyncPrm.dwPtrC = (UINT32)prVdec->rDram.pu1Pic0C;
        ucFbId = prVdec->rOther.u1FbId0;
        break;
    case 1:
        prVdec->rOther.rVSyncPrm.dwPtrY = (UINT32)prVdec->rDram.pu1Pic1Y;
        prVdec->rOther.rVSyncPrm.dwPtrC = (UINT32)prVdec->rDram.pu1Pic1C;
        ucFbId = prVdec->rOther.u1FbId1;
        break;
    case 2:
        prVdec->rOther.rVSyncPrm.dwPtrY = (UINT32)prVdec->rDram.pu1Pic2Y;
        prVdec->rOther.rVSyncPrm.dwPtrC = (UINT32)prVdec->rDram.pu1Pic2C;
        ucFbId = prVdec->rOther.u1FbId2;
        break;
    }

//#ifdef USE_FBM  // megaa 20100113 ToDo: check the matching
    if ((prVdec->rPicLayer.u1FrameCodingMode != INTERLACEFIELD) || (prVdec->rPicLayer.i4CurrentTemporalField == 1)) {
        if ((prVdec->rPicLayer.u1PicType == BVOP) || (prVdec->rPicLayer.u1PicType == BIVOP)) //prog_B
        {
            BOOL fgNotDisplay;
            //FBM_PIC_HDR_T * prFbmPicHdr;
            VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);//maiyou

#ifdef USE_FBM  // megaa 20100113 ToDo: check the matching
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prVdec->rOther.u1FbgId, ucFbId);


            if (prFbmPicHdr)
            {
            #ifdef ENABLE_MULTIMEDIA
                if (prVdecEsInfo->u4MMSpeed == (UINT32)STC_SPEED_TYPE_FORWARD_1X || prVdecEsInfo->eContainerType == SWDMX_FMT_VC1_ES)

                {
                    if (prVdec->rOther.fgFirstDispFrame)
                    {
                        prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                        prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                        prVdec->rOther.fgFirstDispFrame = FALSE;
                    }
                    else
                    {
                        if (prVdec->rOther.i4CodecVersion != WMVA)
                        {
                            prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                            prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                        }
                        else
                        {
                        #ifdef ENABLE_MULTIMEDIA
                            if (prVdecEsInfo->eContainerType != SWDMX_FMT_MPEG2_TS
                                    && prVdecEsInfo->eContainerType != SWDMX_FMT_MKV
                                    && prVdecEsInfo->eContainerType != SWDMX_FMT_VC1_ES
                                    && prVdecEsInfo->eContainerType != SWDMX_FMT_MPEG2_TS_192
                                    && prVdecEsInfo->eContainerType != SWDMX_FMT_MPEG2_TS_ZERO_192
                                    && prVdecEsInfo->eContainerType != SWDMX_FMT_MPEG2_TS_TIME_SHIFT)

                            {
                                if ((prVdec->rOther.fgBindingByte == TRUE) &&
                                    (prVdec->rOther.u1BindingByte & 0x02))
                                {
                                    LOG(6, "%s(L:%d) lastpts(0x%x) <- u4pts(0x%x)\n",
                                        __FUNCTION__, __LINE__,
                                        prVdec->rOther.u4LastPTS, prFbmPicHdr->u4PTS);
                                    // Only I&P frames, no need to re-calculate PTS.
                                    prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                                    prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                                }
                                else if(prVdec->rOther.fgVC1NoCalPts)
                                {
                                    prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                                    prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                                }
								
								else if (prVdec->rOther.fgEncrypted)  // megaa 0816
								{
									   prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
									   prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
								}
                                else
                                {
                                    UINT32 u4HalfDeltaPTS = prVdec->rOther.u4DeltaPTS>>1;
                                UINT64 u8HalfDeltaPTS = prVdec->rOther.u8DeltaPTS>>1;
                                    if ((prFbmPicHdr->u4PTS <= (prVdec->rOther.u4LastPTS+prVdec->rOther.u4DeltaPTS+u4HalfDeltaPTS))
                                            && prFbmPicHdr->u4PTS >= (prVdec->rOther.u4LastPTS + u4HalfDeltaPTS))
                                    {
                                        LOG(6, "%s(L:%d) lastpts(0x%x) <- u4pts(0x%x)\n",
                                            __FUNCTION__, __LINE__,
                                            prVdec->rOther.u4LastPTS, prFbmPicHdr->u4PTS);
                                        prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                                    }
                                    else
                                    {
                                        LOG(6, "%s(L:%d) u4pts(0x%x) <- lastpts(0x%x) + delta(0x%x)\n",
                                            __FUNCTION__, __LINE__,
                                            prFbmPicHdr->u4PTS, prVdec->rOther.u4LastPTS, prVdec->rOther.u4DeltaPTS);
                                        prFbmPicHdr->u4PTS = prVdec->rOther.u4LastPTS + prVdec->rOther.u4DeltaPTS;
                                        prVdec->rOther.u4LastPTS += prVdec->rOther.u4DeltaPTS;
                                    }
                                    if ((prFbmPicHdr->u8PTS <= (prVdec->rOther.u8LastPTS+prVdec->rOther.u8DeltaPTS+u8HalfDeltaPTS))
                                            && prFbmPicHdr->u8PTS >= (prVdec->rOther.u8LastPTS + u8HalfDeltaPTS))
                                    {
                                        prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                                    }
                                    else
                                    {
                                        prFbmPicHdr->u8PTS = prVdec->rOther.u8LastPTS + prVdec->rOther.u8DeltaPTS;
                                        prVdec->rOther.u8LastPTS += prVdec->rOther.u8DeltaPTS;
                                    }
                                }
                            }
                            else
                            {

                                if ((!prVdec->rPesInfo.fgDtsValid) || prVdecEsInfo->eContainerType == SWDMX_FMT_VC1_ES)

                                {
                                    LOG(6, "%s(L:%d) u4pts(0x%x) <- lastpts(0x%x) + delta(0x%x)\n",
                                        __FUNCTION__, __LINE__,
                                        prFbmPicHdr->u4PTS, prVdec->rOther.u4LastPTS, prVdec->rOther.u4DeltaPTS);
                                    prFbmPicHdr->u4PTS = prVdec->rOther.u4LastPTS + prVdec->rOther.u4DeltaPTS;
                                    prVdec->rOther.u4LastPTS += prVdec->rOther.u4DeltaPTS;
                                    prFbmPicHdr->u8PTS = prVdec->rOther.u8LastPTS + prVdec->rOther.u8DeltaPTS;
                                    prVdec->rOther.u8LastPTS += prVdec->rOther.u8DeltaPTS;
                                }
                                else
                                {
                                    LOG(6, "%s(L:%d) lastpts(0x%x) <- u4pts(0x%x)\n",
                                        __FUNCTION__, __LINE__,
                                        prVdec->rOther.u4LastPTS, prFbmPicHdr->u4PTS);
                                    prVdec->rOther.u4LastPTS = prFbmPicHdr->u4PTS;
                                    prVdec->rOther.u8LastPTS = prFbmPicHdr->u8PTS;
                                }

                            }
							#endif
                        }
                    }
                    prFbmPicHdr->i4TemporalRef = prVdec->rPicLayer.i4TemporalRef;
                    LOG(6,"B(%d) type(%d) pts(0x%x) tempref(%d)\n",
                        ucEsId,
                        prFbmPicHdr->ucPicCdTp,
                        prFbmPicHdr->u4PTS,
                        prFbmPicHdr->i4TemporalRef);
                    prVdec->rPicLayer.i4TemporalRef++;
                }
				#endif
//#endif  // megaa 20100113 ToDo: check the matching
                fgNotDisplay = VDEC_IsNotDisplay(ucEsId, prFbmPicHdr->u4PTS,
                                                 prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset);
            }
            else
#endif  // megaa 20100113 ToDo: check the matching
            {
                fgNotDisplay = VDEC_IsNotDisplay(ucEsId, 0, 0, 0);//maiyou
            }
             if(prVdec->rOther.u4CurrPicNo == 1)
             {
                fgNotDisplay = TRUE;
                LOG(3,"If WMV Second Frame is B,Drop!!!!!!!!!!!\n");
             }
            if (fgNotDisplay || prVdec->rOther.fgSkipFrame)  // ??? !!!1123
            {
#ifdef WMV_PRINT_FBM_STATUS
                UCHAR u1state;
                u1state = FBM_GetFrameBufferStatus(prVdec->rOther.u1FbgId,ucFbId);
                LOG(3,"WMV set B frame FBM status : %d %d EMPTY\n",ucFbId,u1state);
#endif
                prVdec->rOther.fgSkipFrame = FALSE;
                FBM_SetFrameBufferStatus(prVdec->rOther.u1FbgId, ucFbId, FBM_FB_STATUS_EMPTY);
            }
            else if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I) || prVdec->rOther.u4NextIPTS)
            {
#ifdef WMV_PRINT_FBM_STATUS
                UCHAR u1state;
                u1state = FBM_GetFrameBufferStatus(prVdec->rOther.u1FbgId,ucFbId);
                LOG(3,"WMV set B frame FBM status : %d %d EMPTY\n",ucFbId,u1state);
#endif
                //prVdec->rOther.fgSkipFrame = FALSE;

                FBM_SetFrameBufferStatus(prVdec->rOther.u1FbgId, ucFbId, FBM_FB_STATUS_EMPTY);
            }
            else if (!prVdec->rOther.fgEarlyDisp)
            {
                vPutDispQForB(ucEsId, ucFbId);
            }
            if (prFbmPicHdr)
            {
                VDEC_CheckStopStatus(ucEsId, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef );//maiyou
            }
            else
            {
                VDEC_CheckStopStatus(ucEsId, 0, 0);//maiyou
            }

        }
        /*else if (prVdec->rPicLayer.u1PicType == SKIPFRAME)
        {
            FBM_SetFrameBufferStatus(prVdec->rOther.u1FbgId, ucFbId,
                                     FBM_FB_STATUS_EMPTY);
        }*/
        else
        {
            vWmvPrntFbmStatus(prVdec->rOther.u1FbgId, ucFbId,
                                     FBM_FB_STATUS_READY,__LINE__);
            FBM_SetFrameBufferStatus(prVdec->rOther.u1FbgId, ucFbId,
                                     FBM_FB_STATUS_READY);
        }
    }
}

VOID vVSyncWriteVdoReg(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vVSyncWriteVdoReg : ucEsId >= MAX_ES_NUM");
        return;
    }
    WriteREG(RW_VDO_PTRY, PHYSICAL(prVdec->rOther.rVSyncPrm.dwPtrY >> 3));
    WriteREG(RW_VDO_PTRC, PHYSICAL(prVdec->rOther.rVSyncPrm.dwPtrC >> 3));
}

VOID _VDEC_UpdateVldWMVWPtr(UCHAR ucEsId,UINT32 u4WPtr)
{
/*
#if defined(IS_WMV_POST_MT5368)
    UNUSED(ucEsId);
    WriteREG(WO_VLD_VWPTR,u4WPtr);
#else
*/
    UINT32  u4WPtrAlign, u4VStart, u4VEnd;
    UINT32  u4VLD44;
    UINT32  u4WPtrTemp;

    // check whether write pointer exceed read pointer
    u4VLD44 = ReadREG(ucEsId,RW_VLD_RPTR) ;
    u4WPtrTemp = ((u4WPtr + RPTR_ALIGN - 1) & (~((UINT32)RPTR_ALIGN - 1)));
    u4WPtrAlign = ((u4WPtr + RPTR_ALIGN - 1) & (~((UINT32)RPTR_ALIGN - 1))) + WPTR_OFFSET;
    
    if((u4WPtrTemp < u4VLD44) && (u4WPtrAlign > u4VLD44)){
        LOG(0,"u4WPtrAlign=0x%x,u4WPtrTemp=0x%x\n",u4WPtrAlign,u4WPtrTemp);
        u4WPtrAlign = u4WPtrTemp;
    }

    u4VStart = ReadREG(ucEsId,RW_VLD_VSTART)<<6;
    u4VEnd = ReadREG(ucEsId,RW_VLD_VEND)<<6;

    if (u4WPtrAlign < u4VStart)
    {
        u4WPtrAlign = u4VEnd - (u4VStart - u4WPtrAlign);
    }
    if (u4WPtrAlign > u4VEnd)
    {
        u4WPtrAlign = u4VStart + (u4WPtrAlign - u4VEnd);
    }
    WriteREG(WO_VLD_VWPTR,u4WPtrAlign);
//#endif
}

UINT32 dwVldRPtr(UCHAR ucEsId,UINT32 *pdwBits)
{
#if (defined(CC_MT5398) || defined(CC_MT5399) || defined(VDEC_IS_POST_MT5880))
    UINT32 dwDramRptr;
    UINT32 dwSramRptr, dwSramWptr;
    UINT32 dwSramDataSz;
    UINT32 dwByteAddr;
    VDEC_WMV_INFO_T* prVdec = NULL;
    UINT32 u4SramCtrl;
#ifdef WAIT_SRAM_STABLE
    UINT32 u4Tmp = 0;
#endif

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "dwVldRPtr : ucEsId >= MAX_ES_NUM");
        return 0;
    }
#ifdef WAIT_SRAM_STABLE
#if defined(IS_WMV_POST_MT5368)
    if (((ReadREG(ucEsId,RO_VLD_SRAMCTRL) >> 15) & AA_FIT_TARGET_D) == 1)
#endif
    {
        while ((ReadREG(ucEsId,RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Tmp++;
            if (u4Tmp >= 0x10000)
            {
                LOG(0, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n",
                    ReadREG(ucEsId,RO_VLD_SRAMCTRL), ReadREG(ucEsId,RW_VLD_RPTR), ReadREG(ucEsId,RO_VLD_VRPTR),
                    ReadREG(ucEsId,RO_VLD_VBAR), ReadREG(ucEsId,RO_VLD_FETCHOK));
                return FALSE;
            }
        }
    }
#endif

    dwDramRptr = ReadREG(ucEsId,RO_VLD_VRPTR);              // DramReadPtr = VLD_reg_63
    dwSramRptr = dwSramWptr = ReadREG(ucEsId,RO_VLD_VBAR);
    u4SramCtrl = (dwSramRptr >> 24) & 0x3;                  // SramCtr  = VLD_reg_59[25:24]
    dwSramRptr &= 0x000000FF;                               // SramRptr = VLD_reg_59[7:0]
    dwSramWptr = (dwSramWptr >> 8) & 0x000000FF;            // SramWPtr = VLD_reg_59[15:8]

    if (dwSramWptr > dwSramRptr)
    {
        dwSramDataSz = dwSramWptr - dwSramRptr;
    }
    else
    {
        dwSramDataSz = 16 - (dwSramRptr - dwSramWptr);
    }

    *pdwBits = ReadREG(ucEsId,RW_VLD_BITCOUNT) & 0x3f;
    //*pdwBits = (ReadREG(ucEsId,RW_VLD_BITCOUNT)>>16) & 0x3f;

    if (ReadREG(ucEsId,RW_VLD_WMV_ABS) & 0x1) // 03 replacement enable!
    {
        dwByteAddr = dwDramRptr - dwSramDataSz * 16 + u4SramCtrl * 4 - 20 + (*pdwBits / 8);  // !!!1006 check with HM
    }
    else
    {
        dwByteAddr = dwDramRptr - dwSramDataSz * 16 + u4SramCtrl * 4 - 16 + (*pdwBits / 8);
    }

    if (dwByteAddr < (UINT32)prVdec->rDram.pu1VFifo)
    {
        dwByteAddr = dwByteAddr + ((UINT32)prVdec->rDram.pu1FifoEnd - (UINT32)prVdec->rDram.pu1VFifo);
    }

    //spork
    *pdwBits &= 0x7;
    dwByteAddr -= ((UINT32)prVdec->rDram.pu1VFifo);

	
    return (dwByteAddr);

#else // CC_MT5398
    UINT32 dwDramRptr;
    UINT32 dwSramRptr, dwSramWptr;
    UINT32 dwSramDataSz;
    UINT32 dwByteAddr;
    VDEC_WMV_INFO_T* prVdec = NULL;
    UINT32 u4SramCtrl;
#ifdef WAIT_SRAM_STABLE
    UINT32 u4Tmp = 0;
#endif

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "dwVldRPtr : ucEsId >= MAX_ES_NUM");
        return 0;
    }
#ifdef WAIT_SRAM_STABLE
#if defined(IS_WMV_POST_MT5368)
    if (((ReadREG(ucEsId,RO_VLD_SRAMCTRL) >> 15) & AA_FIT_TARGET_D) == 1)
#endif
    {
        while ((ReadREG(ucEsId,RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Tmp++;
            if (u4Tmp >= 0x10000)
            {
                LOG(0, "//===SRAM !Stable 0xF4=0x%x, Rptr 0xB0=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n",
                    ReadREG(ucEsId,RO_VLD_SRAMCTRL), ReadREG(ucEsId,RW_VLD_RPTR), ReadREG(ucEsId,RO_VLD_VRPTR),
                    ReadREG(ucEsId,RO_VLD_VBAR), ReadREG(ucEsId,RO_VLD_FETCHOK));
                return FALSE;
            }
        }
    }
#endif

    dwDramRptr = ReadREG(ucEsId,RO_VLD_VRPTR);// & 0xffffff;
    dwSramRptr = dwSramWptr = ReadREG(ucEsId,RO_VLD_VBAR);
#if defined(IS_WMV_POST_MT5368)
    u4SramCtrl = (dwSramRptr >> 24) & 0x3;
    dwSramRptr &= 0xF;
    dwSramWptr = (dwSramWptr >> 8) & 0xF;
#else
    UNUSED(u4SramCtrl);
    dwSramRptr = (UINT32)((((dwSramRptr & 0xf) * 4) + ((dwSramRptr >> 24) & 0x3)));
    dwSramWptr = (UINT32)((((dwSramWptr >> 8) & 0xf) * 4));
#endif

    /*if(!fgWaitVldFetchOk(ucEsId))
     {
     LOG(3,"VLD RPtr wait vld not ok\n");
     }*/

    //dwDramRptr = ReadREG(ucEsId,RO_VLD_VRPTR);// & 0xffffff;
    //dwSramRptr = ReadREG(ucEsId,RO_VLD_VBAR)&0x3f;// & 0xff;
    //dwSramWptr = (ReadREG(ucEsId,RO_VLD_VBAR) >> 16)&0x3f;// & 0xff;

    if (dwSramWptr > dwSramRptr)
    {
        dwSramDataSz = dwSramWptr - dwSramRptr;
    }
    else
    {
#if defined(IS_WMV_POST_MT5368)
        dwSramDataSz = 16 - (dwSramRptr - dwSramWptr);
#else
        dwSramDataSz = 64 - (dwSramRptr - dwSramWptr);
#endif
    }

    *pdwBits = ReadREG(ucEsId,RW_VLD_BITCOUNT) & 0x3f;
    //*pdwBits = (ReadREG(ucEsId,RW_VLD_BITCOUNT)>>16) & 0x3f;

    if (ReadREG(ucEsId,RW_VLD_WMV_ABS) & 0x1) // 03 replacement enable!
    {
#if defined(IS_WMV_POST_MT5368)
        dwByteAddr = dwDramRptr - dwSramDataSz * 16 + u4SramCtrl * 4 - 20 + (*pdwBits / 8);  // !!!1006 check with HM
#else
        dwByteAddr = dwDramRptr - (dwSramDataSz + 4) * 4 + (*pdwBits / 8);
#endif
    }
    else
    {
#if defined(IS_WMV_POST_MT5368)
        dwByteAddr = dwDramRptr - dwSramDataSz * 16 + u4SramCtrl * 4 - 16 + (*pdwBits / 8);
#else
        dwByteAddr = dwDramRptr - (dwSramDataSz + 3)* 4 + (*pdwBits / 8);
#endif
    }

    if (dwByteAddr < (UINT32)prVdec->rDram.pu1VFifo)
    {
        dwByteAddr = dwByteAddr + ((UINT32)prVdec->rDram.pu1FifoEnd - (UINT32)prVdec->rDram.pu1VFifo);
    }

    //spork
    *pdwBits &= 0x7;
    dwByteAddr -= ((UINT32)prVdec->rDram.pu1VFifo);
    return (dwByteAddr);
#endif // CC_MT5398
}

void WriteDram64b(UINT32 dwAddr, UINT32 dw64HiDW, UINT32 dw64LoDW)
{
    //UINT32 *pAddr = (UINT32*)dwAddr;
    //VIRTUAL(pAddr[0]) = dw64LoDW;
    x_memcpy((VOID*)(VIRTUAL(dwAddr)),(VOID*)(VIRTUAL((UINT32)&dw64LoDW)),4);
    //x_memset(VIRTUAL(pAddr[0]),)
}
void WriteDram8b(UINT32 dwAddr, UINT32 dw64HiDW, UINT32 dw64LoDW)
{
    //UINT8 *pAddr = (UINT8*)dwAddr;
    //VIRTUAL(pAddr[0]) = (UINT8)dw64LoDW;
    x_memcpy((VOID*)(VIRTUAL(dwAddr)),(VOID*)(VIRTUAL((UINT32)&dw64LoDW)),1);
}

//copy from jup.c

/* Intensity compensation */
void cal_icomp(int *iScale, int *iShift, int m_iLuminanceScale,
               int m_iLuminanceShift)
{
    /* derived from interpolate_wmv9.c IntensityCompensation() */
    if (m_iLuminanceShift > 31)
        m_iLuminanceShift -= 64;

    // remap luminance scale and shift
    if (m_iLuminanceScale == 0)
    {
        *iScale = - 64;
        *iShift = 255 * 64 - m_iLuminanceShift * 2 * 64;
    }
    else
    {
        *iScale = m_iLuminanceScale + 32;
        *iShift = m_iLuminanceShift * 64;
    }
}

/*****************************************/
/*  NAME: print_vop_header_parameter     */
/*  Description: to print picture-layer  */
/*           information */
/*****************************************/

void print_vop_header_parameter(UCHAR ucEsId,/*int umv_from_mb,*/int vopnum, int fref_ctrl,
        int out_ctrl)
{

    //Printf("entering vop header parameter function\n");
    int iScale, iShift;
    long    a34, a36, a37, a38, a39, a42, a35, a131, a132, a133, a136, a137, a138,
    a140, a142, a195, b_fraction_chk, a193, a198, a200, a206;
    int tmp, TRB, TRD;
    unsigned long high_rate_switch;
    //int dump_size;
    int use_rule_b, ttfrm, use_quarter_pel_mv, use_interpredictor;
    int height; //for interlace_field
    int i;
    int vop_type;
    int iScaleTop = 0, iShiftTop = 0;
    int iScaleBot = 0, iShiftBot = 0;
    //int u4MC_DBK;
    //ilii
    VDEC_WMV_INFO_T* prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    prVdec->rOther.i4pred_use_wdle = 1;//0;
    //_fgPrintWReg = TRUE;
    //_fgRealWReg = (cbWMVFWBP) ? FALSE : TRUE;
    //vop_type = (prVdec->rPicLayer.u1PicType == PVOP) ? PVOP: (prVdec->rPicLayer.u1PicType == BVOP)? BVOP : IVOP; //VLD view
    if (prVdec->rPicLayer.u1PicType == PVOP)
    {
        vop_type = PVOP;
    }
    else if (prVdec->rPicLayer.u1PicType == BVOP)
    {
        vop_type = BVOP;
    }
    else
    {
        vop_type = IVOP;
    }
    //LOG(0,"entering print_vop_header_parameter\n\n");
    //LOG(0,"entering print_vop_header_parameter\n\n");
    //LOG(0,"entering print_vop_header_parameter\n\n");
    //LOG(0,"entering print_vop_header_parameter\n\n");
    //Printf("vop type : %d\n",vop_type);
    if (((prVdec->rPicLayer.u1PicType == IVOP)
            || (prVdec->rPicLayer.u1PicType == PVOP)
            || (prVdec->rPicLayer.u1PicType == SKIPFRAME)) && (/* first field picture */
                ((prVdec->rPicLayer.fgFieldMode == TRUE)
                 && (prVdec->rPicLayer.i4CurrentTemporalField== 0)) ||
                /* frame picture */
                (prVdec->rPicLayer.fgFieldMode == FALSE) ))
    {
        prVdec->rDram.u4ForwardRefPicType
        = prVdec->rDram.u4BackwardRefPicType;

        if ((prVdec->rOther.i4CodecVersion == WMVA)
                && (prVdec->rPicLayer.fgInterlaceV2))
        {
            if (prVdec->rPicLayer.fgFieldMode == TRUE)
                prVdec->rDram.u4BackwardRefPicType = INTERLACEFIELD;
            else
                prVdec->rDram.u4BackwardRefPicType = INTERLACEFRAME;
        }
        else
        {
            prVdec->rDram.u4BackwardRefPicType = PROGRESSIVE;
        }
    }

    if ((prVdec->rSMProfile.fgXintra8) && (prVdec->rPicLayer.u1PicType
            == IVOP))
    {
        return;
    }

    //print pattern for fpga
    //-------------------------------------------------
    // global setting 只要一開始設一次即可
    //-------------------------------------------------
    /* vSetMcBufPtr(ucEsId) will set the following registers
     WriteREG(RW_MC_R1Y, ((UINT32)(prVdec->rDram.pu1Pic0Y) >> _iMemBase) >> 9); //ref_pic1_y_base_addr
     WriteREG(RW_MC_R1C, ((UINT32)(prVdec->rDram.pu1Pic0C) >> _iMemBase) >> 8); //ref_pic1_c_base_addr
     WriteREG(RW_MC_R2Y, ((UINT32)(prVdec->rDram.pu1Pic1Y) >> _iMemBase) >> 9); //ref_pic2_y_base_addr
     WriteREG(RW_MC_R2C, ((UINT32)(prVdec->rDram.pu1Pic1C) >> _iMemBase) >> 8); //ref_pic2_y_base_addr
     WriteREG(RW_MC_BY1, ((UINT32)(prVdec->rDram.pu1Pic2Y) >> _iMemBase) >> 9); //prog_y_base_addr
     WriteREG(RW_MC_BC1, ((UINT32)(prVdec->rDram.pu1Pic2C) >> _iMemBase) >> 8); //prog_c_base_addr
     */
    //  WriteREG(RW_MC_2FLDMD, 0x00000000);
    WriteREG(RW_MC_QUARTER_SAMPLE, 0x00000001); //always set to 1 for WMV case
    WriteREG(RW_MC_PP_DBLK_THD, 0x000f0606); //always set to this value for WMV case
    //  WriteREG(RW_MC_PP_DBLK_Y_ADDR, DEC_PP_1);
    //  WriteREG(RW_MC_PP_DBLK_C_ADDR, DEC_PP_2);

    //spork
#ifdef CC_WMV_EMULATION
    WriteREG(RW_VLD_SRAM_TEST_WRITE_ADDR, 200);  // to speed up DRAM access (emulation only)
#endif

    //  WriteREG(0x50c, 0xe00); //err_conceal off
    //WriteREG(0xD7c, 0x1); //turn off the ext_sram
    //WriteREG(0x7f8, 0x1); //turn on check sum shift mode
    //WriteREG(VLD_REG_OFST + 0x258, 1000);

    //------------------------------
    //   MC RISC WRITE PATTERN
    //------------------------------
    //----------------------------------------------------------
    // MC_reg_9 : MC output buffer setting
    //            (0:ref_pic1_buf  1:ref_pic2_buf  4:prg_B_pic1)
    //----------------------------------------------------------
    if ((prVdec->rPicLayer.u1PicType == BVOP)
            || (prVdec->rPicLayer.u1PicType == BIVOP))
    {
        WriteREG(RW_MC_OPBUF, 0x00000004);
        WriteREG(RW_MC_FWDP, fref_ctrl);
    }
    else //I, P
    {
        WriteREG(RW_MC_OPBUF, out_ctrl);
        WriteREG(RW_MC_FWDP, fref_ctrl);
    }

    //----------------------------------------------------------
    // MC_reg_8 : indicate if this is the 2nd field picture
    //        (0:1st field picture or frame picture
    //         1:2nd field picture)
    //----------------------------------------------------------
    WriteREG(RW_MC_2FLDMD, prVdec->rPicLayer.i4CurrentTemporalField);

    //----------------------------------------------------------
    // MC_reg_180 : mixed mv resolution (1: turn on)
    //----------------------------------------------------------
    WriteREG(RW_MC_WMV8_MIX_PEL, prVdec->rPicLayer.fgMvResolution);

    //----------------------------------------------------------
    // MC_reg_127 : quarter pel type (set 0:TYPE_14496)
    //----------------------------------------------------------
    WriteREG(RW_MC_QPEL_TYPE, 0x00000000); //always set to 0 for WMV

    //----------------------------------------------------------
    //MC_reg_129 : rounding_control (1:turn on)
    //----------------------------------------------------------
    WriteREG(RW_MC_ROUNDING_CTRL, prVdec->rPicLayer.i4RndCtrl);

    //--------------------------------------------------------------------
    // MC_reg_130, 131 : UMV Padding boundary (取16的倍數, 或實際pic大小)
    //--------------------------------------------------------------------
    if (prVdec->rOther.i4CodecVersion <= WMV3)
    {
        WriteREG(RW_MC_UMV_PIC_WIDTH, prVdec->rSeqHdrR.u4PicWidthDec);
        WriteREG(RW_MC_UMV_PIC_HEIGHT, prVdec->rSeqHdrR.u4PicHeightDec);
    }
    else /* UMV Padding directly from picture boundary */
    {
        WriteREG(RW_MC_UMV_PIC_WIDTH, prVdec->rSeqHdrR.u4PicWidthSrc);
        WriteREG(RW_MC_UMV_PIC_HEIGHT, prVdec->rSeqHdrR.u4PicHeightSrc);
    }

    //--------------------------------------------------------------------
    // MC_reg_156 : scale type (0:NO_SCALE, 1:SCALE_DOWN, 2:SCALE_UP)
    //--------------------------------------------------------------------
    if (prVdec->rPicLayer.u1PicType == IVOP)
    {
        prVdec->rOther.bPreProcessFrameStatus = NO_SCALE;
        WriteREG(RW_MC_WMV9_PRE_PROC, NO_SCALE);
    }
    else if ( (prVdec->rPicLayer.u1PicType == PVOP)
              || (prVdec->rPicLayer.u1PicType == SKIPFRAME)) /* only for PVOP */
    {
        if (prVdec->rEntryPoint.i4ReconRangeState == 0)//pWMVDec->mprVdec->rEntryPoint.i4ReconRangeState == 0)
        {
            if (prVdec->rEntryPoint.i4RangeState == 1)
            {
                // JUP comment
                // Previous not scale range
                // Current scale down range
                // Hence, scan DOWN previous frame to using it for current frame's motion compensation
                //
                // reduce by 2
                prVdec->rOther.bPreProcessFrameStatus = SCALE_DOWN;
                WriteREG(RW_MC_WMV9_PRE_PROC, SCALE_DOWN);
            }
            else
            {
                prVdec->rOther.bPreProcessFrameStatus = NO_SCALE;
                WriteREG(RW_MC_WMV9_PRE_PROC, NO_SCALE);
            }
        }
        else if (prVdec->rEntryPoint.i4ReconRangeState == 1)//pWMVDec->mprVdec->rEntryPoint.i4ReconRangeState == 1)
        {
            if (prVdec->rEntryPoint.i4RangeState == 0)
            {
                // JUP comment
                // Previous scale down range
                // Current not scale range
                // Hence, scan UP previous frame to using it for current frame's motion compensation
                //
                // increase by 2
                prVdec->rOther.bPreProcessFrameStatus = SCALE_UP;
                WriteREG(RW_MC_WMV9_PRE_PROC, SCALE_UP);
            }
            else
            {
                prVdec->rOther.bPreProcessFrameStatus = NO_SCALE;
                WriteREG(RW_MC_WMV9_PRE_PROC, NO_SCALE);
            }
        }
    }
    else// if (prVdec->rPicLayer.u1PicType == BVOP) , Ju said both for BIVOP and BVOP
    {
        /* just follow the setting of last decoded P picture */
        WriteREG(RW_MC_WMV9_PRE_PROC, prVdec->rOther.bPreProcessFrameStatus);
    }

    //
    // Update Icomp parameters for both IVOP and PVOP
    //
    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        if ( (prVdec->rPicLayer.u1PicType == PVOP)
                || (prVdec->rPicLayer.u1PicType == IVOP)
                || (prVdec->rPicLayer.u1PicType == SKIPFRAME))
        {
            if (prVdec->rPicLayer.fgFieldMode == TRUE) //field picture
            {
                if (prVdec->rPicLayer.i4CurrentTemporalField== 0) // the first P field picture
                {
                    prVdec->rOther.i4BoundaryUMVIcomp = 0;

                    /* Step 1: Update Icomp parameters, Old <= New */
                    //prVdec->rOther.rNewTopField.Old = prVdec->rOther.rNewTopField.New;
                    //prVdec->rOther.rNewBotField.Old = prVdec->rOther.rNewBotField.New;

                    if (prVdec->rPicLayer.i4CurrentField == 0) //current decode field is top field
                    {
                        /* Step 1: Update Icomp parameters     */
                        /* OLD_BF <= NEW_BF                */

                        prVdec->rOther.rOldBotField.Old
                        = prVdec->rOther.rNewBotField.Old;
                        prVdec->rOther.rOldBotField.New
                        = prVdec->rOther.rNewBotField.New;

                        /* reset NEW_BF */
                        prVdec->rOther.rNewBotField.Old.iEnable = 0;
                        prVdec->rOther.rNewBotField.New.iEnable = 0;

                        /* update NEW_TF, OLD <= NEW */
                        prVdec->rOther.rNewTopField.Old
                        = prVdec->rOther.rNewTopField.New;

                        if ( /* forward reference picture is frame picture */
                            /* and decode the first field picture now */
                            (prVdec->rDram.u4ForwardRefPicType == PROGRESSIVE)
                            || (prVdec->rDram.u4ForwardRefPicType
                                == INTERLACEFRAME))
                        {
                            prVdec->rOther.rNewTopField.Old.iEnable = 0;
                        }

                    }
                    else // current decode field is bottom field
                    {
                        /* Step 1: Update Icomp parameters     */
                        /* OLD_TF <= NEW_TF                */

                        prVdec->rOther.rOldTopField.Old
                        = prVdec->rOther.rNewTopField.Old;
                        prVdec->rOther.rOldTopField.New
                        = prVdec->rOther.rNewTopField.New;

                        /* reset NEW_TF */
                        prVdec->rOther.rNewTopField.Old.iEnable = 0;
                        prVdec->rOther.rNewTopField.New.iEnable = 0;

                        /* update NEW_BF, OLD <= NEW */
                        prVdec->rOther.rNewBotField.Old
                        = prVdec->rOther.rNewBotField.New;

                        if ( /* forward reference picture is frame picture */
                            /* and decode the first field picture now */
                            (prVdec->rDram.u4ForwardRefPicType == PROGRESSIVE)
                            || (prVdec->rDram.u4ForwardRefPicType
                                == INTERLACEFRAME))
                        {
                            prVdec->rOther.rNewBotField.Old.iEnable = 0;
                        }
                    }

                    /* Step 2: calculate Icomp parameters, New <= Icomp */
                    if (prVdec->rOther.fgLuminanceWarpTop == TRUE)
                    {
                        cal_icomp(&iScaleTop, &iShiftTop,
                                  prVdec->rOther.i4LumScaleTop,
                                  prVdec->rOther.i4LumShiftTop);
                        prVdec->rOther.rNewTopField.New.iShift = iShiftTop;
                        prVdec->rOther.rNewTopField.New.iScale = iScaleTop;
                        prVdec->rOther.rNewTopField.New.iEnable = 1;
                    }
                    else
                    {
                        prVdec->rOther.rNewTopField.New.iEnable = 0;
                    }

                    if (prVdec->rOther.fgLuminanceWarpBottom == TRUE)
                    {
                        cal_icomp(&iScaleBot, &iShiftBot,
                                  prVdec->rOther.i4LumScaleBottom,
                                  prVdec->rOther.i4LumShiftBottom);
                        prVdec->rOther.rNewBotField.New.iShift = iShiftBot;
                        prVdec->rOther.rNewBotField.New.iScale = iScaleBot;
                        prVdec->rOther.rNewBotField.New.iEnable = 1;
                    }
                    else
                    {
                        prVdec->rOther.rNewBotField.New.iEnable = 0;
                    }
                }
                else // the second P field picture
                {
                    if ( (prVdec->rDram.u4ForwardRefPicType == PROGRESSIVE)
                            && (prVdec->rDram.u4BackwardRefPicType
                                == INTERLACEFIELD)
                            /* && this is a second field */
                       )
                    {
                        prVdec->rOther.i4BoundaryUMVIcomp = 1;
                    }
                    else
                    {
                        prVdec->rOther.i4BoundaryUMVIcomp = 0;
                    }

                    prVdec->rOther.i4SecondFieldParity
                    = prVdec->rPicLayer.i4CurrentField;

                    if (prVdec->rPicLayer.i4CurrentField == 0) //current decode field is top field
                    {
                        /* Step 1: Update Icomp parameters     */
                        /* OLD_BF <= NEW_BF  */
                        prVdec->rOther.rOldBotField.Old
                        = prVdec->rOther.rNewBotField.Old;
                        prVdec->rOther.rOldBotField.New
                        = prVdec->rOther.rNewBotField.New;

                        /* reset NEW_BF */
                        prVdec->rOther.rNewBotField.Old.iEnable = 0;
                        prVdec->rOther.rNewBotField.New.iEnable = 0;

                        /* update NEW_TF: Old <= New   */
                        prVdec->rOther.rNewTopField.Old
                        = prVdec->rOther.rNewTopField.New;
                    }
                    else // current decode field is bottom field
                    {
                        /* Step 1: Update Icomp parameters     */
                        /* OLD_TF <= NEW_TF                */
                        prVdec->rOther.rOldTopField.Old
                        = prVdec->rOther.rNewTopField.Old;
                        prVdec->rOther.rOldTopField.New
                        = prVdec->rOther.rNewTopField.New;

                        /* reset NEW_TF */
                        prVdec->rOther.rNewTopField.Old.iEnable = 0;
                        prVdec->rOther.rNewTopField.New.iEnable = 0;

                        /* update NEW_BF: Old <= New            */
                        prVdec->rOther.rNewBotField.Old
                        = prVdec->rOther.rNewBotField.New;
                    }

                    /* Step 2: calculate Icomp parameters, both NEW_BF.New amd NEW_TF.New <= Icomp */
                    if (prVdec->rOther.fgLuminanceWarpTop == TRUE)
                    {
                        cal_icomp(&iScaleTop, &iShiftTop,
                                  prVdec->rOther.i4LumScaleTop,
                                  prVdec->rOther.i4LumShiftTop);
                        prVdec->rOther.rNewTopField.New.iShift = iShiftTop;
                        prVdec->rOther.rNewTopField.New.iScale = iScaleTop;
                        prVdec->rOther.rNewTopField.New.iEnable = 1;
                    }
                    else
                    {
                        prVdec->rOther.rNewTopField.New.iEnable = 0;
                    }

                    if (prVdec->rOther.fgLuminanceWarpBottom == TRUE)
                    {
                        cal_icomp(&iScaleBot, &iShiftBot,
                                  prVdec->rOther.i4LumScaleBottom,
                                  prVdec->rOther.i4LumShiftBottom);
                        prVdec->rOther.rNewBotField.New.iShift = iShiftBot;
                        prVdec->rOther.rNewBotField.New.iScale = iScaleBot;
                        prVdec->rOther.rNewBotField.New.iEnable = 1;
                    }
                    else
                    {
                        prVdec->rOther.rNewBotField.New.iEnable = 0;
                    }
                }
            } //end : if (prVdec->rPicLayer.fgFieldMode == TRUE)
            else // frame picture
            {
                prVdec->rOther.i4BoundaryUMVIcomp = 0;

                /* Step 1: Update Icomp parameters, both NEW_BF and NEW_TF, Old <= New */
                prVdec->rOther.rNewTopField.Old
                = prVdec->rOther.rNewTopField.New;
                prVdec->rOther.rNewBotField.Old
                = prVdec->rOther.rNewBotField.New;

                if ( /* forward reference picture is frame picture */
                    (prVdec->rDram.u4ForwardRefPicType == PROGRESSIVE)
                    || (prVdec->rDram.u4ForwardRefPicType
                        == INTERLACEFRAME))
                {
                    prVdec->rOther.rNewTopField.Old.iEnable = 0;
                    prVdec->rOther.rNewBotField.Old.iEnable = 0;
                }
                else /* prVdec->rDram.u4ForwardRefPicType == INTERLACE_FIELD */
                {
                    /* previous reference second field is top field */
                    if (prVdec->rOther.i4SecondFieldParity == 0)
                    {
                        prVdec->rOther.rNewTopField.Old.iEnable = 0;
                    }
                    /* previous reference second field is bottom field */
                    else
                    {
                        prVdec->rOther.rNewBotField.Old.iEnable = 0;
                    }
                }

                /* Step 2: calculate Icomp parameters, both NEW_BF and NEW_TF, New <= Icomp */
                if (prVdec->rOther.fgLuminanceWarp == TRUE)
                {
                    cal_icomp(&iScaleTop, &iShiftTop,
                              prVdec->rOther.i4LumScale,
                              prVdec->rOther.i4LumShift);
                    prVdec->rOther.rNewTopField.New.iShift = iShiftTop;
                    prVdec->rOther.rNewTopField.New.iScale = iScaleTop;
                    prVdec->rOther.rNewTopField.New.iEnable = 1;
                }
                else
                {
                    prVdec->rOther.rNewTopField.New.iEnable = 0;
                }

                if (prVdec->rOther.fgLuminanceWarp == TRUE)
                {
                    cal_icomp(&iScaleBot, &iShiftBot,
                              prVdec->rOther.i4LumScale,
                              prVdec->rOther.i4LumShift);
                    prVdec->rOther.rNewBotField.New.iShift = iShiftBot;
                    prVdec->rOther.rNewBotField.New.iScale = iScaleBot;
                    prVdec->rOther.rNewBotField.New.iEnable = 1;
                }
                else
                {
                    prVdec->rOther.rNewBotField.New.iEnable = 0;
                }
            } //end : frame picture
        } //end : IVOP PVOP SKIPFRAME
    } //end : if (prVdec->rOther.i4CodecVersion == WMVA)


    //-------------------------------------------------------
    // intensity compensation
    //
    //------------------------------------------------------

    if ((prVdec->rPicLayer.u1PicType == IVOP)
            && (prVdec->rOther.i4CodecVersion == WMVA))
    {
        WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
        WriteREG(RW_MC_ICOMP2_EN, 0);
    }
    else if (( (prVdec->rPicLayer.u1PicType == PVOP)
               || (prVdec->rPicLayer.u1PicType == SKIPFRAME) )
             && (prVdec->rOther.i4CodecVersion == WMVA))
    {
        /* Step 3: output MC parameters */
        // icomp_set1 is forward top field new icomp parameters
        if (prVdec->rOther.rNewTopField.New.iEnable == 1)
        {
            WriteREG(RW_MC_ISCALE1_X1, (iScaleTop * 1));
            WriteREG(RW_MC_ISCALE1_X3, (iScaleTop * 3));
            WriteREG(RW_MC_ISCALE1_X5, (iScaleTop * 5));
            WriteREG(RW_MC_ISCALE1_X7, (iScaleTop * 7));
            WriteREG(RW_MC_ISCALE1_X9, (iScaleTop * 9));
            WriteREG(RW_MC_ISCALE1_X11, (iScaleTop * 11));
            WriteREG(RW_MC_ISCALE1_X13, (iScaleTop * 13));
            WriteREG(RW_MC_ISCALE1_X15, (iScaleTop * 15));
            WriteREG(RW_MC_YSHIFT_OFF1, (iShiftTop + 32));
            WriteREG(RW_MC_CSHIFT_OFF1, ((128 * 64) + 32 - (128*iScaleTop)));
        }

        // icomp_set2 is forward bottom field new icomp parameters
        if (prVdec->rOther.rNewBotField.New.iEnable == 1)
        {
            WriteREG(RW_MC_ISCALE2_X1, (iScaleBot * 1));
            WriteREG(RW_MC_ISCALE2_X3, (iScaleBot * 3));
            WriteREG(RW_MC_ISCALE2_X5, (iScaleBot * 5));
            WriteREG(RW_MC_ISCALE2_X7, (iScaleBot * 7));
            WriteREG(RW_MC_ISCALE2_X9, (iScaleBot * 9));
            WriteREG(RW_MC_ISCALE2_X11, (iScaleBot * 11));
            WriteREG(RW_MC_ISCALE2_X13, (iScaleBot * 13));
            WriteREG(RW_MC_ISCALE2_X15, (iScaleBot * 15));
            WriteREG(RW_MC_YSHIFT_OFF2, (iShiftBot + 32));
            WriteREG(RW_MC_CSHIFT_OFF2, ((128 * 64) + 32 - (128*iScaleBot)));
        }

        if (prVdec->rOther.rNewTopField.Old.iEnable == 1)
        {
            // icomp_set3 is forward top field old icomp parameters
            WriteREG(RW_MC_ISCALE3_X1,
                     (prVdec->rOther.rNewTopField.Old.iScale * 1));
            WriteREG(RW_MC_ISCALE3_X3,
                     (prVdec->rOther.rNewTopField.Old.iScale * 3));
            WriteREG(RW_MC_ISCALE3_X5,
                     (prVdec->rOther.rNewTopField.Old.iScale * 5));
            WriteREG(RW_MC_ISCALE3_X7,
                     (prVdec->rOther.rNewTopField.Old.iScale * 7));
            WriteREG(RW_MC_ISCALE3_X9,
                     (prVdec->rOther.rNewTopField.Old.iScale * 9));
            WriteREG(RW_MC_ISCALE3_X11,
                     (prVdec->rOther.rNewTopField.Old.iScale * 11));
            WriteREG(RW_MC_ISCALE3_X13,
                     (prVdec->rOther.rNewTopField.Old.iScale * 13));
            WriteREG(RW_MC_ISCALE3_X15,
                     (prVdec->rOther.rNewTopField.Old.iScale * 15));
            WriteREG(RW_MC_YSHIFT_OFF3,
                     (prVdec->rOther.rNewTopField.Old.iShift + 32));
            WriteREG(RW_MC_CSHIFT_OFF3, ((128 * 64) + 32 - (128
                                         *prVdec->rOther.rNewTopField.Old.iScale)));
        }

        if (prVdec->rOther.rNewBotField.Old.iEnable == 1)
        {
            // icomp_set4 is forward bottom field old icomp parameters
            WriteREG(RW_MC_ISCALE4_X1,
                     (prVdec->rOther.rNewBotField.Old.iScale * 1));
            WriteREG(RW_MC_ISCALE4_X3,
                     (prVdec->rOther.rNewBotField.Old.iScale * 3));
            WriteREG(RW_MC_ISCALE4_X5,
                     (prVdec->rOther.rNewBotField.Old.iScale * 5));
            WriteREG(RW_MC_ISCALE4_X7,
                     (prVdec->rOther.rNewBotField.Old.iScale * 7));
            WriteREG(RW_MC_ISCALE4_X9,
                     (prVdec->rOther.rNewBotField.Old.iScale * 9));
            WriteREG(RW_MC_ISCALE4_X11,
                     (prVdec->rOther.rNewBotField.Old.iScale * 11));
            WriteREG(RW_MC_ISCALE4_X13,
                     (prVdec->rOther.rNewBotField.Old.iScale * 13));
            WriteREG(RW_MC_ISCALE4_X15,
                     (prVdec->rOther.rNewBotField.Old.iScale * 15));
            WriteREG(RW_MC_YSHIFT_OFF4,
                     (prVdec->rOther.rNewBotField.Old.iShift + 32));
            WriteREG(RW_MC_CSHIFT_OFF4, ((128 * 64) + 32 - (128
                                         *prVdec->rOther.rNewBotField.Old.iScale)));
        }

        prVdec->rOther.i4BoundaryUMVIcompEnable = 0;
        if (prVdec->rOther.i4BoundaryUMVIcomp == 1)
        {
            /* top field picture */
            if (prVdec->rOther.i4SecondFieldParity == 0)
            {
                if (prVdec->rOther.rOldBotField.New.iEnable == 1)
                {
                    prVdec->rOther.i4BoundaryUMVIcompEnable = 1;
                    // icomp_set6 is forward bottom field old icomp parameters
                    WriteREG(
                        RW_MC_ISCALE6_X1,
                        (prVdec->rOther.rOldBotField.New.iScale * 1));
                    WriteREG(
                        RW_MC_ISCALE6_X3,
                        (prVdec->rOther.rOldBotField.New.iScale * 3));
                    WriteREG(
                        RW_MC_ISCALE6_X5,
                        (prVdec->rOther.rOldBotField.New.iScale * 5));
                    WriteREG(
                        RW_MC_ISCALE6_X7,
                        (prVdec->rOther.rOldBotField.New.iScale * 7));
                    WriteREG(
                        RW_MC_ISCALE6_X9,
                        (prVdec->rOther.rOldBotField.New.iScale * 9));
                    WriteREG(RW_MC_ISCALE6_X11,
                             (prVdec->rOther.rOldBotField.New.iScale
                              * 11));
                    WriteREG(RW_MC_ISCALE6_X13,
                             (prVdec->rOther.rOldBotField.New.iScale
                              * 13));
                    WriteREG(RW_MC_ISCALE6_X15,
                             (prVdec->rOther.rOldBotField.New.iScale
                              * 15));
                    WriteREG(RW_MC_YSHIFT_OFF6,
                             (prVdec->rOther.rOldBotField.New.iShift
                              + 32));
                    WriteREG(RW_MC_CSHIFT_OFF6, ((128 * 64) + 32 - (128
                                                 *prVdec->rOther.rOldBotField.New.iScale)));
                }
            }
            /* bottom field picture */
            else
            {
                if (prVdec->rOther.rOldTopField.New.iEnable == 1)
                {
                    prVdec->rOther.i4BoundaryUMVIcompEnable = 1;
                    // icomp_set6 is forward bottom field old icomp parameters
                    WriteREG(
                        RW_MC_ISCALE6_X1,
                        (prVdec->rOther.rOldTopField.New.iScale * 1));
                    WriteREG(
                        RW_MC_ISCALE6_X3,
                        (prVdec->rOther.rOldTopField.New.iScale * 3));
                    WriteREG(
                        RW_MC_ISCALE6_X5,
                        (prVdec->rOther.rOldTopField.New.iScale * 5));
                    WriteREG(
                        RW_MC_ISCALE6_X7,
                        (prVdec->rOther.rOldTopField.New.iScale * 7));
                    WriteREG(
                        RW_MC_ISCALE6_X9,
                        (prVdec->rOther.rOldTopField.New.iScale * 9));
                    WriteREG(RW_MC_ISCALE6_X11,
                             (prVdec->rOther.rOldTopField.New.iScale
                              * 11));
                    WriteREG(RW_MC_ISCALE6_X13,
                             (prVdec->rOther.rOldTopField.New.iScale
                              * 13));
                    WriteREG(RW_MC_ISCALE6_X15,
                             (prVdec->rOther.rOldTopField.New.iScale
                              * 15));
                    WriteREG(RW_MC_YSHIFT_OFF6,
                             (prVdec->rOther.rOldTopField.New.iShift
                              + 32));
                    WriteREG(RW_MC_CSHIFT_OFF6, ((128 * 64) + 32 - (128
                                                 *prVdec->rOther.rOldTopField.New.iScale)));
                }
            }
        }

        if ( (prVdec->rOther.rNewTopField.New.iEnable == 1)
                || (prVdec->rOther.rNewBotField.New.iEnable == 1)
                || (prVdec->rOther.rNewTopField.Old.iEnable == 1)
                || (prVdec->rOther.rNewBotField.Old.iEnable == 1)
                || (prVdec->rOther.i4BoundaryUMVIcompEnable == 1))
        {
            // Icomp enable bits
            {
                int b208 = (prVdec->rOther.i4BoundaryUMVIcompEnable)+ (0
                           << 4)+ (prVdec->rOther.rNewBotField.Old.iEnable
                                   << 8)+ (prVdec->rOther.rNewTopField.Old.iEnable
                                           << 12)+ (prVdec->rOther.rNewBotField.New.iEnable
                                                    << 16)+ (prVdec->rOther.rNewTopField.New.iEnable
                                                             << 20);

                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN + ICOMP_Y_EN);
                WriteREG(RW_MC_ICOMP2_EN, b208);
                WriteREG(RW_MC_SAME_ICOMP, NO_USE_SAME_ICOPM1_FOR_FRAME);
            }
        }
        else
        {
            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
            WriteREG(RW_MC_ICOMP2_EN, 0);
            WriteREG(RW_MC_SAME_ICOMP, NO_USE_SAME_ICOPM1_FOR_FRAME);
        }
    }//end : PVOP
    else if ((prVdec->rPicLayer.u1PicType == BVOP)
             && (prVdec->rOther.i4CodecVersion == WMVA))
    {
        if (prVdec->rPicLayer.fgFieldMode == TRUE) //field picture
        {
            if (prVdec->rPicLayer.i4CurrentTemporalField== 0)// the first B field picture
            {
                prVdec->rOther.i4BoundaryUMVIcomp = 0;

                //
                // Step1: No update in the Icomp parameters
                //

                //
                // Step2: output MC parameters
                //

                //
                // Last second P field picture is Top Field
                //
                // (1) Forward TF == NEW_TF
                // (2) Forward BF == OLD_BF
                // (3) Backward TF == NULL
                // (4) Backward BF == NEW_BF

                if (prVdec->rOther.i4SecondFieldParity == 0)
                {
                    // icomp_set1 is forward top field new icomp parameters
                    if (prVdec->rOther.rNewTopField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE1_X1,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE1_X3,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE1_X5,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE1_X7,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE1_X9,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE1_X11,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE1_X13,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE1_X15,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF1,
                                 (prVdec->rOther.rNewTopField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF1,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewTopField.New.iScale)));
                    }

                    // icomp_set2 is forward bottom field new icomp parameters
                    if (prVdec->rOther.rOldBotField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE2_X1,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE2_X3,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE2_X5,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE2_X7,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE2_X9,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE2_X11,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE2_X13,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE2_X15,
                                 (prVdec->rOther.rOldBotField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF2,
                                 (prVdec->rOther.rOldBotField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF2,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rOldBotField.New.iScale)));
                    }

                    // icomp_set3 is forward top field old icomp parameters
                    if (prVdec->rOther.rNewTopField.Old.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE3_X1,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE3_X3,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE3_X5,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE3_X7,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE3_X9,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE3_X11,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE3_X13,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE3_X15,
                                 (prVdec->rOther.rNewTopField.Old.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF3,
                                 (prVdec->rOther.rNewTopField.Old.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF3,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewTopField.Old.iScale)));
                    }

                    // icomp_set4 is forward bottom field old icomp parameters
                    if (prVdec->rOther.rOldBotField.Old.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE4_X1,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE4_X3,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE4_X5,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE4_X7,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE4_X9,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE4_X11,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE4_X13,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE4_X15,
                                 (prVdec->rOther.rOldBotField.Old.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF4,
                                 (prVdec->rOther.rOldBotField.Old.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF4,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rOldBotField.Old.iScale)));
                    }

                    // icomp_set5 is backward top or bottom field icomp parameters
                    if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE5_X1,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE5_X3,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE5_X5,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE5_X7,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE5_X9,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE5_X11,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE5_X13,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE5_X15,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF5,
                                 (prVdec->rOther.rNewBotField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF5,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewBotField.New.iScale)));
                        WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_BTM_FLD);
                    }

                    if ( (prVdec->rOther.rNewTopField.New.iEnable == 1)
                            || (prVdec->rOther.rOldBotField.New.iEnable
                                == 1)
                            || (prVdec->rOther.rNewTopField.Old.iEnable
                                == 1)
                            || (prVdec->rOther.rOldBotField.Old.iEnable
                                == 1)
                            || (prVdec->rOther.rNewBotField.New.iEnable
                                == 1))
                    {
                        // Icomp enable bits
                        {
                            int
                            b208 =
                                (0)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 4)
                                + (prVdec->rOther.rOldBotField.Old.iEnable
                                   << 8)
                                + (prVdec->rOther.rNewTopField.Old.iEnable
                                   << 12)
                                + (prVdec->rOther.rOldBotField.New.iEnable
                                   << 16)
                                + (prVdec->rOther.rNewTopField.New.iEnable
                                   << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                    else
                    {
                        WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                 + ICOMP_Y_OFF);
                        WriteREG(RW_MC_ICOMP2_EN, 0);
                        WriteREG(RW_MC_SAME_ICOMP,
                                 NO_USE_SAME_ICOPM1_FOR_FRAME);
                    }
                }
                else
                {

                    //
                    // Last second P field picture is Bottom Field
                    //
                    // (1) Forward TF == OLD_TF
                    // (2) Forward BF == NEW_BF
                    // (3) Backward TF == NEW_TF
                    // (4) Backward BF == NULL

                    // icomp_set1 is forward top field new icomp parameters
                    if (prVdec->rOther.rOldTopField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE1_X1,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE1_X3,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE1_X5,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE1_X7,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE1_X9,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE1_X11,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE1_X13,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE1_X15,
                                 (prVdec->rOther.rOldTopField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF1,
                                 (prVdec->rOther.rOldTopField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF1,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rOldTopField.New.iScale)));
                    }
                    // icomp_set2 is forward bottom field new icomp parameters
                    if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE2_X1,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE2_X3,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE2_X5,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE2_X7,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE2_X9,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE2_X11,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE2_X13,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE2_X15,
                                 (prVdec->rOther.rNewBotField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF2,
                                 (prVdec->rOther.rNewBotField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF2,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewBotField.New.iScale)));
                    }

                    // icomp_set3 is forward top field old icomp parameters
                    if (prVdec->rOther.rOldTopField.Old.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE3_X1,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE3_X3,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE3_X5,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE3_X7,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE3_X9,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE3_X11,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE3_X13,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE3_X15,
                                 (prVdec->rOther.rOldTopField.Old.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF3,
                                 (prVdec->rOther.rOldTopField.Old.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF3,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rOldTopField.Old.iScale)));
                    }

                    // icomp_set4 is forward bottom field old icomp parameters
                    if (prVdec->rOther.rNewBotField.Old.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE4_X1,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE4_X3,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE4_X5,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE4_X7,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE4_X9,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE4_X11,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE4_X13,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE4_X15,
                                 (prVdec->rOther.rNewBotField.Old.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF4,
                                 (prVdec->rOther.rNewBotField.Old.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF4,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewBotField.Old.iScale)));
                    }

                    // icomp_set5 is backward top or bottom field icomp parameters
                    if (prVdec->rOther.rNewTopField.New.iEnable == 1)
                    {
                        WriteREG(RW_MC_ISCALE5_X1,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 1));
                        WriteREG(RW_MC_ISCALE5_X3,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 3));
                        WriteREG(RW_MC_ISCALE5_X5,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 5));
                        WriteREG(RW_MC_ISCALE5_X7,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 7));
                        WriteREG(RW_MC_ISCALE5_X9,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 9));
                        WriteREG(RW_MC_ISCALE5_X11,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 11));
                        WriteREG(RW_MC_ISCALE5_X13,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 13));
                        WriteREG(RW_MC_ISCALE5_X15,
                                 (prVdec->rOther.rNewTopField.New.iScale
                                  * 15));
                        WriteREG(RW_MC_YSHIFT_OFF5,
                                 (prVdec->rOther.rNewTopField.New.iShift
                                  + 32));
                        WriteREG(
                            RW_MC_CSHIFT_OFF5,
                            ((128 * 64) + 32
                             - (128
                                *prVdec->rOther.rNewTopField.New.iScale)));
                        WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_TOP_FLD);
                    }

                    if ( (prVdec->rOther.rOldTopField.New.iEnable == 1)
                            || (prVdec->rOther.rNewBotField.New.iEnable
                                == 1)
                            || (prVdec->rOther.rOldTopField.Old.iEnable
                                == 1)
                            || (prVdec->rOther.rNewBotField.Old.iEnable
                                == 1)
                            || (prVdec->rOther.rNewTopField.New.iEnable
                                == 1))
                    {
                        // Icomp enable bits
                        {
                            int
                            b208 =
                                (0)
                                + (prVdec->rOther.rNewTopField.New.iEnable
                                   << 4)
                                + (prVdec->rOther.rNewBotField.Old.iEnable
                                   << 8)
                                + (prVdec->rOther.rOldTopField.Old.iEnable
                                   << 12)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 16)
                                + (prVdec->rOther.rOldTopField.New.iEnable
                                   << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                    else
                    {
                        WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                 + ICOMP_Y_OFF);
                        WriteREG(RW_MC_ICOMP2_EN, 0);
                        WriteREG(RW_MC_SAME_ICOMP,
                                 NO_USE_SAME_ICOPM1_FOR_FRAME);
                    }
                }
            } //end : first B field picture
            else // Second B field picture
            {
                if ( (prVdec->rDram.u4ForwardRefPicType == PROGRESSIVE)
                        && (prVdec->rDram.u4BackwardRefPicType
                            == INTERLACEFIELD)
                        /* && this is a second field */
                   )
                {
                    prVdec->rOther.i4BoundaryUMVIcomp = 1;
                }
                else
                {
                    prVdec->rOther.i4BoundaryUMVIcomp = 0;
                }

                if (prVdec->rPicLayer.i4CurrentField == 0) // current field is top field
                {
                    //
                    // Step1: No update in the Icomp parameters
                    //

                    //
                    // Step2: output MC parameters
                    //

                    //
                    // Last second P field picture is Top Field
                    //
                    // (1) Forward TF == NEW_TF
                    //
                    // (2) Forward BF == NULL
                    //
                    // (3) Backward TF == NULL
                    // (4) Backward BF == NEW_BF

                    if (prVdec->rOther.i4SecondFieldParity == 0)
                    {
                        // icomp_set1 is forward top field new icomp parameters
                        if (prVdec->rOther.rNewTopField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE1_X1,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE1_X3,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE1_X5,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE1_X7,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE1_X9,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE1_X11,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE1_X13,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE1_X15,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF1,
                                (prVdec->rOther.rNewTopField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF1,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewTopField.New.iScale)));
                        }

                        // icomp_set3 is forward top field old icomp parameters
                        if (prVdec->rOther.rNewTopField.Old.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE3_X1,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE3_X3,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE3_X5,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE3_X7,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE3_X9,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE3_X11,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE3_X13,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE3_X15,
                                (prVdec->rOther.rNewTopField.Old.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF3,
                                (prVdec->rOther.rNewTopField.Old.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF3,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewTopField.Old.iScale)));
                        }

                        // icomp_set5 is backward top or bottom field icomp parameters
                        if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE5_X1,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE5_X3,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE5_X5,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE5_X7,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE5_X9,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE5_X11,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE5_X13,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE5_X15,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF5,
                                (prVdec->rOther.rNewBotField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF5,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.New.iScale)));
                            WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_BTM_FLD);
                        }

                        prVdec->rOther.i4BoundaryUMVIcompEnable = 0;

                        if (prVdec->rOther.i4BoundaryUMVIcomp == 1)
                        {
                            /* top field picture */
                            if (prVdec->rOther.i4SecondFieldParity == 0)
                            {
                                if (prVdec->rOther.rOldBotField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldBotField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldBotField.New.iScale)));
                                }
                            }
                            /* bottom field picture */
                            else
                            {
                                if (prVdec->rOther.rOldTopField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldTopField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldTopField.New.iScale)));
                                }
                            }
                        }

                        if ( (prVdec->rOther.rNewTopField.New.iEnable == 1)
                                || (prVdec->rOther.rNewTopField.Old.iEnable
                                    == 1)
                                || (prVdec->rOther.rNewBotField.New.iEnable
                                    == 1)
                                || (prVdec->rOther.i4BoundaryUMVIcompEnable
                                    == 1))
                        {
                            // Icomp enable bits
                            int
                            b208 =
                                (prVdec->rOther.i4BoundaryUMVIcompEnable)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 4)+ (0 << 8)
                                + (prVdec->rOther.rNewTopField.Old.iEnable
                                   << 12)+ (0
                                            << 16)
                                + (prVdec->rOther.rNewTopField.New.iEnable
                                   << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                        else
                        {
                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                     + ICOMP_Y_OFF);
                            WriteREG(RW_MC_ICOMP2_EN, 0);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                    else
                    {
                        //
                        // Last second P field picture is Bottom Field
                        //
                        //
                        // (1) Forward TF == NULL
                        //
                        // (2) Forward BF == NEW_BF
                        // (3) Backward TF == NEW_TF
                        // (4) Backward BF == NULL

                        // icomp_set2 is forward bottom field new icomp parameters
                        if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE2_X1,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE2_X3,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE2_X5,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE2_X7,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE2_X9,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE2_X11,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE2_X13,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE2_X15,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF2,
                                (prVdec->rOther.rNewBotField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF2,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.New.iScale)));
                        }

                        // icomp_set4 is forward bottom field old icomp parameters
                        if (prVdec->rOther.rNewBotField.Old.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE4_X1,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE4_X3,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE4_X5,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE4_X7,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE4_X9,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE4_X11,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE4_X13,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE4_X15,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF4,
                                (prVdec->rOther.rNewBotField.Old.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF4,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.Old.iScale)));
                        }

                        // icomp_set5 is backward top or bottom field icomp parameters

                        if (prVdec->rOther.rNewTopField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE5_X1,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE5_X3,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE5_X5,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE5_X7,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE5_X9,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE5_X11,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE5_X13,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE5_X15,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF5,
                                (prVdec->rOther.rNewTopField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF5,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewTopField.New.iScale)));
                            WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_TOP_FLD);
                        }

                        prVdec->rOther.i4BoundaryUMVIcompEnable = 0;

                        if (prVdec->rOther.i4BoundaryUMVIcomp == 1)
                        {
                            /* top field picture */
                            if (prVdec->rOther.i4SecondFieldParity == 0)
                            {
                                if (prVdec->rOther.rOldBotField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldBotField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldBotField.New.iScale)));
                                }
                            }
                            /* bottom field picture */
                            else
                            {
                                if (prVdec->rOther.rOldTopField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldTopField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldTopField.New.iScale)));
                                }
                            }
                        }

                        if ( (prVdec->rOther.rNewBotField.New.iEnable == 1)
                                || (prVdec->rOther.rNewBotField.Old.iEnable
                                    == 1)
                                || (prVdec->rOther.rNewTopField.New.iEnable
                                    == 1)
                                || (prVdec->rOther.i4BoundaryUMVIcompEnable
                                    == 1))
                        {
                            // Icomp enable bits
                            int
                            b208 =
                                (prVdec->rOther.i4BoundaryUMVIcompEnable)
                                + (prVdec->rOther.rNewTopField.New.iEnable
                                   << 4)
                                + (prVdec->rOther.rNewBotField.Old.iEnable
                                   << 8)
                                + (0 << 12)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 16)+ (0
                                            << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                        else
                        {
                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                     + ICOMP_Y_OFF);
                            WriteREG(RW_MC_ICOMP2_EN, 0);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                }
                else// current field is bottom field
                {

                    //
                    // Step1: No update in the Icomp parameters
                    //

                    //
                    // Step2: output MC parameters
                    //

                    //
                    // Last second P field picture is Top Field
                    //
                    //
                    // (1) Forward TF == NULL
                    //
                    // (2) Forward BF == OLD_BF
                    // (3) Backward TF == NULL
                    // (4) Backward BF == NEW_BF

                    if (prVdec->rOther.i4SecondFieldParity == 0)
                    {
                        // icomp_set2 is forward bottom field new icomp parameters
                        if (prVdec->rOther.rOldBotField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE2_X1,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE2_X3,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE2_X5,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE2_X7,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE2_X9,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE2_X11,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE2_X13,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE2_X15,
                                (prVdec->rOther.rOldBotField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF2,
                                (prVdec->rOther.rOldBotField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF2,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rOldBotField.New.iScale)));
                        }

                        // icomp_set4 is forward bottom field old icomp parameters
                        if (prVdec->rOther.rOldBotField.Old.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE4_X1,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE4_X3,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE4_X5,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE4_X7,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE4_X9,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE4_X11,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE4_X13,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE4_X15,
                                (prVdec->rOther.rOldBotField.Old.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF4,
                                (prVdec->rOther.rOldBotField.Old.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF4,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rOldBotField.Old.iScale)));
                        }

                        // icomp_set5 is backward top or bottom field icomp parameters
                        if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE5_X1,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE5_X3,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE5_X5,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE5_X7,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE5_X9,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE5_X11,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE5_X13,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE5_X15,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF5,
                                (prVdec->rOther.rNewBotField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF5,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.New.iScale)));
                            WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_BTM_FLD);
                        }

                        prVdec->rOther.i4BoundaryUMVIcompEnable = 0;

                        if (prVdec->rOther.i4BoundaryUMVIcomp == 1)
                        {
                            /* top field picture */
                            if (prVdec->rOther.i4SecondFieldParity == 0)
                            {
                                if (prVdec->rOther.rOldBotField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldBotField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldBotField.New.iScale)));
                                }
                            }
                            /* bottom field picture */
                            else
                            {
                                if (prVdec->rOther.rOldTopField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldTopField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldTopField.New.iScale)));
                                }
                            }
                        }

                        if ( (prVdec->rOther.rOldBotField.New.iEnable == 1)
                                || (prVdec->rOther.rOldBotField.Old.iEnable
                                    == 1)
                                || (prVdec->rOther.rNewBotField.New.iEnable
                                    == 1)
                                || (prVdec->rOther.i4BoundaryUMVIcompEnable
                                    == 1))
                        {
                            // Icomp enable bits
                            int
                            b208 =
                                (prVdec->rOther.i4BoundaryUMVIcompEnable)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 4)
                                + (prVdec->rOther.rOldBotField.Old.iEnable
                                   << 8)
                                + (0 << 12)
                                + (prVdec->rOther.rOldBotField.New.iEnable
                                   << 16)+ (0
                                            << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                        else
                        {
                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                     + ICOMP_Y_OFF);
                            WriteREG(RW_MC_ICOMP2_EN, 0);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                    else
                    {
                        //
                        // Last second P field picture is Bottom Field
                        //
                        //
                        // (1) Forward TF == NULL
                        //
                        // (2) Forward BF == NEW_BF
                        // (3) Backward TF == NEW_TF
                        // (4) Backward BF == NULL

                        // icomp_set2 is forward bottom field new icomp parameters
                        if (prVdec->rOther.rNewBotField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE2_X1,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE2_X3,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE2_X5,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE2_X7,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE2_X9,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE2_X11,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE2_X13,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE2_X15,
                                (prVdec->rOther.rNewBotField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF2,
                                (prVdec->rOther.rNewBotField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF2,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.New.iScale)));
                        }

                        // icomp_set4 is forward bottom field old icomp parameters
                        if (prVdec->rOther.rNewBotField.Old.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE4_X1,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE4_X3,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE4_X5,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE4_X7,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE4_X9,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE4_X11,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE4_X13,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE4_X15,
                                (prVdec->rOther.rNewBotField.Old.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF4,
                                (prVdec->rOther.rNewBotField.Old.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF4,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewBotField.Old.iScale)));
                        }

                        // icomp_set5 is backward top or bottom field icomp parameters

                        if (prVdec->rOther.rNewTopField.New.iEnable == 1)
                        {
                            WriteREG(
                                RW_MC_ISCALE5_X1,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 1));
                            WriteREG(
                                RW_MC_ISCALE5_X3,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 3));
                            WriteREG(
                                RW_MC_ISCALE5_X5,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 5));
                            WriteREG(
                                RW_MC_ISCALE5_X7,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 7));
                            WriteREG(
                                RW_MC_ISCALE5_X9,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 9));
                            WriteREG(
                                RW_MC_ISCALE5_X11,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 11));
                            WriteREG(
                                RW_MC_ISCALE5_X13,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 13));
                            WriteREG(
                                RW_MC_ISCALE5_X15,
                                (prVdec->rOther.rNewTopField.New.iScale
                                 * 15));
                            WriteREG(
                                RW_MC_YSHIFT_OFF5,
                                (prVdec->rOther.rNewTopField.New.iShift
                                 + 32));
                            WriteREG(
                                RW_MC_CSHIFT_OFF5,
                                ((128 * 64) + 32
                                 - (128
                                    *prVdec->rOther.rNewTopField.New.iScale)));
                            WriteREG(RW_MC_BWD_ICOMP_FLD, BWD_TOP_FLD);
                        }

                        prVdec->rOther.i4BoundaryUMVIcompEnable = 0;

                        if (prVdec->rOther.i4BoundaryUMVIcomp == 1)
                        {
                            /* top field picture */
                            if (prVdec->rOther.i4SecondFieldParity == 0)
                            {
                                if (prVdec->rOther.rOldBotField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldBotField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldBotField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldBotField.New.iScale)));
                                }
                            }
                            else /* bottom field picture */
                            {
                                if (prVdec->rOther.rOldTopField.New.iEnable
                                        == 1)
                                {
                                    prVdec->rOther.i4BoundaryUMVIcompEnable
                                    = 1;
                                    // icomp_set4 is forward bottom field old icomp parameters
                                    WriteREG(
                                        RW_MC_ISCALE6_X1,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 1));
                                    WriteREG(
                                        RW_MC_ISCALE6_X3,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 3));
                                    WriteREG(
                                        RW_MC_ISCALE6_X5,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 5));
                                    WriteREG(
                                        RW_MC_ISCALE6_X7,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 7));
                                    WriteREG(
                                        RW_MC_ISCALE6_X9,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 9));
                                    WriteREG(
                                        RW_MC_ISCALE6_X11,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 11));
                                    WriteREG(
                                        RW_MC_ISCALE6_X13,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 13));
                                    WriteREG(
                                        RW_MC_ISCALE6_X15,
                                        (prVdec->rOther.rOldTopField.New.iScale
                                         * 15));
                                    WriteREG(
                                        RW_MC_YSHIFT_OFF6,
                                        (prVdec->rOther.rOldTopField.New.iShift
                                         + 32));
                                    WriteREG(
                                        RW_MC_CSHIFT_OFF6,
                                        ((128 * 64) + 32
                                         - (128
                                            *prVdec->rOther.rOldTopField.New.iScale)));
                                }
                            }
                        }

                        if ( (prVdec->rOther.rNewBotField.New.iEnable == 1)
                                || (prVdec->rOther.rNewBotField.Old.iEnable
                                    == 1)
                                || (prVdec->rOther.rNewTopField.New.iEnable
                                    == 1)
                                || (prVdec->rOther.i4BoundaryUMVIcompEnable
                                    == 1))
                        {
                            // Icomp enable bits
                            int
                            b208 =
                                (prVdec->rOther.i4BoundaryUMVIcompEnable)
                                + (prVdec->rOther.rNewTopField.New.iEnable
                                   << 4)
                                + (prVdec->rOther.rNewBotField.Old.iEnable
                                   << 8) + (0
                                            << 12)
                                + (prVdec->rOther.rNewBotField.New.iEnable
                                   << 16) + (0
                                             << 20);

                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN
                                     + ICOMP_Y_EN);
                            WriteREG(RW_MC_ICOMP2_EN, b208);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                        else
                        {
                            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF
                                     + ICOMP_Y_OFF);
                            WriteREG(RW_MC_ICOMP2_EN, 0);
                            WriteREG(RW_MC_SAME_ICOMP,
                                     NO_USE_SAME_ICOPM1_FOR_FRAME);
                        }
                    }
                }
            }
        }
        else// B frame pciture
        {
            prVdec->rOther.i4BoundaryUMVIcomp = 0;

            // (1) Forward TF == NEW_TF
            // (2) Forward BF == NEW_BF
            // (3) Backward TF == NULL
            // (4) Backward BF == NULL

            /* Step 3: output MC parameters */
            // icomp_set1 is forward top field new icomp parameters
            if (prVdec->rOther.rNewTopField.New.iEnable == 1)
            {
                WriteREG(RW_MC_ISCALE1_X1,
                         (prVdec->rOther.rNewTopField.New.iScale * 1));
                WriteREG(RW_MC_ISCALE1_X3,
                         (prVdec->rOther.rNewTopField.New.iScale * 3));
                WriteREG(RW_MC_ISCALE1_X5,
                         (prVdec->rOther.rNewTopField.New.iScale * 5));
                WriteREG(RW_MC_ISCALE1_X7,
                         (prVdec->rOther.rNewTopField.New.iScale * 7));
                WriteREG(RW_MC_ISCALE1_X9,
                         (prVdec->rOther.rNewTopField.New.iScale * 9));
                WriteREG(RW_MC_ISCALE1_X11,
                         (prVdec->rOther.rNewTopField.New.iScale * 11));
                WriteREG(RW_MC_ISCALE1_X13,
                         (prVdec->rOther.rNewTopField.New.iScale * 13));
                WriteREG(RW_MC_ISCALE1_X15,
                         (prVdec->rOther.rNewTopField.New.iScale * 15));
                WriteREG(RW_MC_YSHIFT_OFF1,
                         (prVdec->rOther.rNewTopField.New.iShift + 32));
                WriteREG(RW_MC_CSHIFT_OFF1, ((128 * 64) + 32 - (128
                                             *prVdec->rOther.rNewTopField.New.iScale)));
            }

            //  icomp_set2 is forward bottom field new icomp parameters
            if (prVdec->rOther.rNewBotField.New.iEnable == 1)
            {
                WriteREG(RW_MC_ISCALE2_X1,
                         (prVdec->rOther.rNewBotField.New.iScale * 1));
                WriteREG(RW_MC_ISCALE2_X3,
                         (prVdec->rOther.rNewBotField.New.iScale * 3));
                WriteREG(RW_MC_ISCALE2_X5,
                         (prVdec->rOther.rNewBotField.New.iScale * 5));
                WriteREG(RW_MC_ISCALE2_X7,
                         (prVdec->rOther.rNewBotField.New.iScale * 7));
                WriteREG(RW_MC_ISCALE2_X9,
                         (prVdec->rOther.rNewBotField.New.iScale * 9));
                WriteREG(RW_MC_ISCALE2_X11,
                         (prVdec->rOther.rNewBotField.New.iScale * 11));
                WriteREG(RW_MC_ISCALE2_X13,
                         (prVdec->rOther.rNewBotField.New.iScale * 13));
                WriteREG(RW_MC_ISCALE2_X15,
                         (prVdec->rOther.rNewBotField.New.iScale * 15));
                WriteREG(RW_MC_YSHIFT_OFF2,
                         (prVdec->rOther.rNewBotField.New.iShift + 32));
                WriteREG(RW_MC_CSHIFT_OFF2, ((128 * 64) + 32 - (128
                                             *prVdec->rOther.rNewBotField.New.iScale)));
            }

            if (prVdec->rOther.rNewTopField.Old.iEnable == 1)
            {
                // icomp_set3 is forward top field old icomp parameters
                WriteREG(RW_MC_ISCALE3_X1,
                         (prVdec->rOther.rNewTopField.Old.iScale * 1));
                WriteREG(RW_MC_ISCALE3_X3,
                         (prVdec->rOther.rNewTopField.Old.iScale * 3));
                WriteREG(RW_MC_ISCALE3_X5,
                         (prVdec->rOther.rNewTopField.Old.iScale * 5));
                WriteREG(RW_MC_ISCALE3_X7,
                         (prVdec->rOther.rNewTopField.Old.iScale * 7));
                WriteREG(RW_MC_ISCALE3_X9,
                         (prVdec->rOther.rNewTopField.Old.iScale * 9));
                WriteREG(RW_MC_ISCALE3_X11,
                         (prVdec->rOther.rNewTopField.Old.iScale * 11));
                WriteREG(RW_MC_ISCALE3_X13,
                         (prVdec->rOther.rNewTopField.Old.iScale * 13));
                WriteREG(RW_MC_ISCALE3_X15,
                         (prVdec->rOther.rNewTopField.Old.iScale * 15));
                WriteREG(RW_MC_YSHIFT_OFF3,
                         (prVdec->rOther.rNewTopField.Old.iShift + 32));
                WriteREG(RW_MC_CSHIFT_OFF3, ((128 * 64) + 32 - (128
                                             *prVdec->rOther.rNewTopField.Old.iScale)));
            }

            if (prVdec->rOther.rNewBotField.Old.iEnable == 1)
            {
                // icomp_set4 is forward bottom field old icomp parameters
                WriteREG(RW_MC_ISCALE4_X1,
                         (prVdec->rOther.rNewBotField.Old.iScale * 1));
                WriteREG(RW_MC_ISCALE4_X3,
                         (prVdec->rOther.rNewBotField.Old.iScale * 3));
                WriteREG(RW_MC_ISCALE4_X5,
                         (prVdec->rOther.rNewBotField.Old.iScale * 5));
                WriteREG(RW_MC_ISCALE4_X7,
                         (prVdec->rOther.rNewBotField.Old.iScale * 7));
                WriteREG(RW_MC_ISCALE4_X9,
                         (prVdec->rOther.rNewBotField.Old.iScale * 9));
                WriteREG(RW_MC_ISCALE4_X11,
                         (prVdec->rOther.rNewBotField.Old.iScale * 11));
                WriteREG(RW_MC_ISCALE4_X13,
                         (prVdec->rOther.rNewBotField.Old.iScale * 13));
                WriteREG(RW_MC_ISCALE4_X15,
                         (prVdec->rOther.rNewBotField.Old.iScale * 15));
                WriteREG(RW_MC_YSHIFT_OFF4,
                         (prVdec->rOther.rNewBotField.Old.iShift + 32));
                WriteREG(RW_MC_CSHIFT_OFF4, ((128 * 64) + 32 - (128
                                             *prVdec->rOther.rNewBotField.Old.iScale)));
            }

            if ( (prVdec->rOther.rNewTopField.New.iEnable == 1)
                    || (prVdec->rOther.rNewBotField.New.iEnable == 1)
                    || (prVdec->rOther.rNewTopField.Old.iEnable == 1)
                    || (prVdec->rOther.rNewBotField.Old.iEnable == 1))
            {
                // Icomp enable bits
                int b208 = (prVdec->rOther.rNewBotField.Old.iEnable
                            << 8)+ (prVdec->rOther.rNewTopField.Old.iEnable
                                    <<12)+ (prVdec->rOther.rNewBotField.New.iEnable
                                            <<16)+ (prVdec->rOther.rNewTopField.New.iEnable
                                                    <<20);

                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN + ICOMP_Y_EN);
                WriteREG(RW_MC_ICOMP2_EN, b208);
                WriteREG(RW_MC_SAME_ICOMP, NO_USE_SAME_ICOPM1_FOR_FRAME);
            }
            else
            {
                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
                WriteREG(RW_MC_ICOMP2_EN, 0);
                WriteREG(RW_MC_SAME_ICOMP, NO_USE_SAME_ICOPM1_FOR_FRAME);
            }
        }

    } //}
    else /* pure frame picture case in WMV9 */
    {
        if (prVdec->rOther.i4CodecVersion >= WMV3)
        {
            WriteREG(RW_MC_SAME_ICOMP, USE_SAME_ICOMP1_FOR_FRAME);
        }

        if (prVdec->rPicLayer.u1PicType == IVOP)
        {
            prVdec->rOther.i4FirstFieldIntensityComp = 0;
            WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
            if (prVdec->rOther.i4CodecVersion >= WMV3)
            {
                WriteREG(RW_MC_ICOMP_TYPE, NO_ICOMP);
            }
        }
        else if ( (prVdec->rPicLayer.u1PicType == PVOP)
                  || (prVdec->rPicLayer.u1PicType == SKIPFRAME))/* only for PVOP */
        {
            if (prVdec->rOther.fgLuminanceWarp)
            {
                prVdec->rOther.i4FirstFieldIntensityComp = 1;
                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN + ICOMP_Y_EN);
                if (prVdec->rOther.i4CodecVersion >= WMV3)
                {
                    WriteREG(RW_MC_ICOMP_TYPE, FRAME_ICOMP);
                }
            }
            else
            {
                prVdec->rOther.i4FirstFieldIntensityComp = 0;
                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
                if (prVdec->rOther.i4CodecVersion >= WMV3)
                {
                    WriteREG(RW_MC_ICOMP_TYPE, NO_ICOMP);
                }
            }
        }
        else if (prVdec->rPicLayer.u1PicType == BVOP)
        {
            /* just follow the setting of last decoded P picture */
            if (prVdec->rOther.i4FirstFieldIntensityComp == 1)
            {
                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_EN + ICOMP_Y_EN);
                if (prVdec->rOther.i4CodecVersion >= WMV3)
                {
                    WriteREG(RW_MC_ICOMP_TYPE, FRAME_ICOMP);
                }
            }
            else
            {
                WriteREG(RW_MC_WMV9_ICOMP_EN, ICOMP_C_OFF + ICOMP_Y_OFF);
                if (prVdec->rOther.i4CodecVersion >= WMV3)
                {
                    WriteREG(RW_MC_ICOMP_TYPE, NO_ICOMP);
                }
            }
        }

        /* derived from interpolate_wmv9.c IntensityCompensation() */
        if (prVdec->rOther.i4LumShift > 31)
            prVdec->rOther.i4LumShift -= 64;

        // remap luminance scale and shift
        if (prVdec->rOther.i4LumScale == 0)
        {
            iScale = - 64;
            iShift = 255 * 64 - prVdec->rOther.i4LumShift * 2 * 64;
        }
        else
        {
            iScale = prVdec->rOther.i4LumScale + 32;
            iShift = prVdec->rOther.i4LumShift * 64;
        }
        {
            int c208;

            if (prVdec->rOther.i4FirstFieldIntensityComp == 1)
            {
                c208 = (0) + (0 << 4) + (0 << 8) + (0 << 12) + (0 << 16) + (1
                        << 20);
            }
            else
            {
                c208 = 0;
            }
            WriteREG(RW_MC_ISCALE1_X1, (iScale * 1));
            WriteREG(RW_MC_ISCALE1_X3, (iScale * 3));
            WriteREG(RW_MC_ISCALE1_X5, (iScale * 5));
            WriteREG(RW_MC_ISCALE1_X7, (iScale * 7));
            WriteREG(RW_MC_ISCALE1_X9, (iScale * 9));
            WriteREG(RW_MC_ISCALE1_X11, (iScale * 11));
            WriteREG(RW_MC_ISCALE1_X13, (iScale * 13));
            WriteREG(RW_MC_ISCALE1_X15, (iScale * 15));
            WriteREG(RW_MC_YSHIFT_OFF1, (iShift + 32));
            WriteREG(RW_MC_CSHIFT_OFF1, ((128 * 64) + 32 - (128*iScale)));
            if (prVdec->rOther.i4CodecVersion >= WMV3)
                WriteREG(RW_MC_ICOMP2_EN, c208);
        }
    }

    //-------------------------------------------------------
    // MC_reg_168 : MV type (0:BILINEAR, 1:BICUBIC)
    //-------------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        if (prVdec->rOther.i4X9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        { /* (pWMVDec->m_iFilterType == FILTER_BICUBIC) */
            WriteREG(RW_MC_FILTER_TYPE, (Y_BILINEAR + C_BILINEAR));
        }
        else
        {/* FILTER_BILINEAR */
            WriteREG(RW_MC_FILTER_TYPE, (Y_BICUBIC + C_BILINEAR));
        }
    }
    else /* prVdec->rOther.i4CodecVersion == WMV2  */
    {
        if ( (prVdec->rOther.i4CodecVersion == WMV2)
                && (prVdec->rPicLayer.fgMvResolution == 1))
        {
            WriteREG(RW_MC_FILTER_TYPE, (Y_BICUBIC + C_BILINEAR));
        }
        else
        {
            WriteREG(RW_MC_FILTER_TYPE, (Y_BILINEAR + C_BILINEAR));
        }
    }

    //-------------------------------------------------------
    // MC_reg_169 : MC clip signal (1:turn on)
    //-------------------------------------------------------
    if (prVdec->rPicLayer.i4Overlap & 1)
    {/* clip operation will be handled by overlap_smooth module */
        WriteREG(RW_MC_WRITE_BUS_TYPE, UNCLIP_TYPE);
    }
    else
    {
        WriteREG(RW_MC_WRITE_BUS_TYPE, CLIP_0_255_TYPE);
    }

    //----------------------------------------------------------------------
    // MC_reg_170 : indicate if the intra_blocks need to add 128 (1:turn on)
    //----------------------------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        //simple/main I
        if ((prVdec->rPicLayer.u1PicType == IVOP
                || prVdec->rPicLayer.u1PicType == BIVOP)
                && (prVdec->rOther.i4CodecVersion == WMV3))
        {
            WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_OFF);
        }
        else
        {
            if (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFRAME)
            {
                WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_ON);
            }
            else if (prVdec->rPicLayer.i4Overlap & 1)
            {/* add 128 will be handled by overlap_smooth module */
                WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_OFF);
            }
            else
            {/* enable add 128 to intra mb */
                WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_ON);
            }
        }
    }
    else /* prVdec->rOther.i4CodecVersion == WMV2 */
    {
        if (prVdec->rPicLayer.u1PicType == PVOP)
        {
            WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_OFF);
        }
        else /* IVOP */
        {
            WriteREG(RW_MC_INTRA_BLK_ADD128, ADD128_OFF);
        }
    }

    //------------------------------------------
    // MC_reg_173 : fast_uvmc (1:turn on)
    //------------------------------------------
    WriteREG(RW_MC_FAST_UVMC, prVdec->rEntryPoint.fgUVHpelBilinear);

    //
    // In-loop deblocking parameters
    //

    //------------------------------------------
    // MC_reg_136,148 : post process (1:turn on)
    //------------------------------------------
    if ((prVdec->rEntryPoint.fgLoopFilter == 1)
            || (prVdec->rPicLayer.i4Overlap & 1))
    {
        WriteREG(RW_MC_PP_ENABLE, 0x00000001);
        WriteREG(RW_MC_PP_WB_BY_POST, 0x00000001);
    }
    else
    {
        WriteREG(RW_MC_PP_ENABLE, 0x00000000);
        WriteREG(RW_MC_PP_WB_BY_POST, 0x00000000);
    }

    //------------------------------------------
    // MC_reg_142 : loopfielter (3:turn on)
    //------------------------------------------
    if (prVdec->rEntryPoint.fgLoopFilter == 1)
    {
        WriteREG(RW_MC_PP_DBLK_MODE, 0x00000003);
    }
    else
    {
        WriteREG(RW_MC_PP_DBLK_MODE, 0x00000000);
    }

    //------------------------------------------
    // MC_reg_175 : overlap smoothing (1:turn on)
    //------------------------------------------
    if (prVdec->rPicLayer.i4Overlap & 1)
    {
        WriteREG(RW_MC_OVL_SMTH_FILTER, OVL_EN);
    }
    else
    {
        WriteREG(RW_MC_OVL_SMTH_FILTER, OVL_OFF);
    }

    //------------------------------------------
    // MC_reg_152,153 : start_MB # & end_MB #
    //------------------------------------------
    WriteREG(RW_MC_PP_X_RANGE, prVdec->rSeqHdrR.u4NumMBX-1);
    WriteREG(RW_MC_PP_Y_RANGE, prVdec->rSeqHdrR.u4NumMBY-1);

    //------------------------------------------
    // MC_reg_139 : picture width in MB
    //------------------------------------------
#ifndef CC_WMV_EMULATION  // 64 align
    WriteREG(RW_MC_PP_MB_WIDTH, (prVdec->rSeqHdrR.u4PicWidthSrc + 63) / 64 * 64 / 16);
    //WriteREG(RW_MC_PP_MB_WIDTH, prVdec->rOther.u4LineSize / 16);
#else
    WriteREG(RW_MC_PP_MB_WIDTH, prVdec->rSeqHdrR.u4NumMBX);
#endif

    //-----------------------------------------------------
    // MC_reg_137,138 : dram base address for post process
    //-----------------------------------------------------
    if ((prVdec->rPicLayer.u1PicType == BVOP)
            || (prVdec->rPicLayer.u1PicType == BIVOP)) //prog_B
    {
        WriteREG(RW_MC_PP_Y_ADDR, ((UINT32)(prVdec->rDram.pu1Pic2Y)) >> 9);
        WriteREG(RW_MC_PP_C_ADDR, ((UINT32)(prVdec->rDram.pu1Pic2C)) >> 8);
        //x_memset(prVdec->rDram.pu1Pic2Y, 0x80, PIC_Y_SZ + PIC_C_SZ);
    }
    else if (out_ctrl == 0) //pic1
    {
        WriteREG(RW_MC_PP_Y_ADDR, ((UINT32)(prVdec->rDram.pu1Pic0Y)) >> 9);
        WriteREG(RW_MC_PP_C_ADDR, ((UINT32)(prVdec->rDram.pu1Pic0C)) >> 8);
        //x_memset(prVdec->rDram.pu1Pic0Y, 0x80, PIC_Y_SZ + PIC_C_SZ);

    }
    else //pic2
    {
        WriteREG(RW_MC_PP_Y_ADDR, ((UINT32)(prVdec->rDram.pu1Pic1Y)) >> 9);
        WriteREG(RW_MC_PP_C_ADDR, ((UINT32)(prVdec->rDram.pu1Pic1C)) >> 8);
        //x_memset(prVdec->rDram.pu1Pic1Y, 0x80, PIC_Y_SZ + PIC_C_SZ);

    }

    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        //-----------------------------------------------------
        // MC_reg_176 : conditional overlap flag (1:turn on)
        //-----------------------------------------------------
        WriteREG(RW_MC_COND_OVL_FILTER,
                 (prVdec->rPicLayer.i4Overlap & 4) ? COND_OVL_EN
                 : COND_OVL_OFF);

        //-----------------------------------------------------
        // MC_reg_177 : for fix MS ref_code bug (bit[0] [8] [16])
        //-----------------------------------------------------
        WriteREG(RW_MC_PP_DBLK_OPT, VC1_ALL_SET_ONE);

        //-----------------------------------------------------
        // MC_reg_184 : Range Map flag (bit1:y, bit0:c)
        //-----------------------------------------------------

        //-----------------------------------------------------
        // MC_reg_185 : Range Map parameter
        //-----------------------------------------------------
        //Give it to Display module.
        WriteREG(
            RW_MC_RNG_PARA,
            ((prVdec->rEntryPoint.i4RangeRedY>0) ? prVdec->rEntryPoint.i4RangeRedY
             -1
             : 0)*256
            + ((prVdec->rEntryPoint.i4Range_MapUV>0) ? prVdec->rEntryPoint.i4Range_MapUV
               -1
               : 0));
    }
    else
    {
        //-----------------------------------------------------
        // MC_reg_177 : for fix MS ref_code bug (bit[0] [8] [16])
        //-----------------------------------------------------
        WriteREG(RW_MC_PP_DBLK_OPT, NOT_VC1_ALL_SET_ZERO);
    }

    //---------------------------------------------------------
    // MC_reg_231 : referece picture
    //---------------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        int
        b231 =
            ((prVdec->rDram.u4BackwardRefPicType == 0) ? BWD_PROG_REF_PIC
             : BWD_INTLCE_REF_PIC)
            + (((prVdec->rDram.u4ForwardRefPicType
                 == 0) ? FWD_PROG_REF_PIC
                : FWD_INTLCE_REF_PIC));//0: 1) << 16);
        WriteREG(RW_MC_REF_PIC_TYPE, b231);
    }

    //------------------------------
    //   PRED RISC WRITE PATTERN
    //------------------------------

    //-----------------------------------------------------
    // VLD_reg_143 : prediction parameter
    //-----------------------------------------------------

    //-----------------------------------------------------
    // VLD_reg_193 : dcac prediction rule selection
    //-----------------------------------------------------
    //for dc_pred rule selection : Rule_A or Rule_B
    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        use_interpredictor = (prVdec->rSeqLayer.i4BitRate
                              <= MAX_BITRATE_DCPred_IMBInPFrame
                              && (prVdec->rSeqHdrR.u4PicWidthSrc
                                  * prVdec->rSeqHdrR.u4PicHeightSrc < 320 * 240))
                             && (prVdec->rPicLayer.u1PicType == PVOP)
                             && (prVdec->rOther.i4CodecVersion == WMV1);
        use_rule_b
        = (((prVdec->rPicLayer.i4Overlap & 0x1) == 0)
           && (prVdec->rPicLayer.u1PicType == IVOP)
           && ((prVdec->rSeqLayer.i4WMV3Profile
                == WMV3_SIMPLE_PROFILE)
               || (prVdec->rSeqLayer.i4WMV3Profile
                   == WMV3_MAIN_PROFILE)))
          || use_interpredictor
          | (prVdec->rOther.i4CodecVersion <= WMV2);
        prVdec->rOther.use_rule_b = use_rule_b;
        use_quarter_pel_mv = 0;
        prVdec->rOther.use_quarter_pel_mv = use_quarter_pel_mv;

        a193 = //0x0021c0b4 +
            (((prVdec->rSeqHdrR.u4PicWidthSrc+15)/16)*4)+
            ((((prVdec->rSeqHdrR.u4PicWidthSrc+15)/16)*4*3)<<12)+
            (use_rule_b << 27) + (use_interpredictor << 28)
            + (use_quarter_pel_mv
               /*(pWMVDec->m_pAltTables->m_iHalfPelMV == 0)*/<< 29) + /* quarter-pel */
            ( 0 << 30); //dcac overlap

    }
    else // >= WMV3
    {
        use_interpredictor = 0;
        use_rule_b
        = ((prVdec->rPicLayer.i4Overlap & 0x1) == 0)
          && ((prVdec->rPicLayer.u1PicType == IVOP)
              || (prVdec->rPicLayer.u1PicType == BIVOP))
          && ((prVdec->rSeqLayer.i4WMV3Profile
               == WMV3_SIMPLE_PROFILE)
              || (prVdec->rSeqLayer.i4WMV3Profile
                  == WMV3_MAIN_PROFILE));
        prVdec->rOther.use_rule_b = use_rule_b;
        use_quarter_pel_mv = (prVdec->rPicLayer.u1FrameCodingMode
                              == INTERLACEFRAME) ? 1 : ((prVdec->rOther.i4X9MVMode
                                                         == ALL_1MV_HALFPEL || prVdec->rOther.i4X9MVMode
                                                         == ALL_1MV_HALFPEL_BILINEAR)) ? 0 : 1;
        prVdec->rOther.use_quarter_pel_mv = use_quarter_pel_mv;

        a193 = //0x0021c0b4 +
            (((prVdec->rSeqHdrR.u4PicWidthSrc+15)/16)*4)
            + ((((prVdec->rSeqHdrR.u4PicWidthSrc+15)/16)*4*3)
               <<12)+ (use_rule_b << 27) + (use_interpredictor
                                            << 28) + (use_quarter_pel_mv
                                                      /*(pWMVDec->m_pAltTables->m_iHalfPelMV == 0)*/<< 29) + /* quarter-pel */
            ( 0 << 30); //dcac overlap
    }

    WriteREG(RW_VLD_DCACPITCH, a193);

    //-----------------------------------------------------
    // VLD_reg_198 : bit-plane mode & address
    //-----------------------------------------------------
    //ilii
    a198 = (((UINT32)prVdec->rDram.pu1Bp_1 >> 2) & 0x00FFFFFF)
           + (((UINT32)prVdec->rDram.pu1Bp_1 << 2) & 0xF0000000)
           + (prVdec->rOther.i4pred_use_wdle << 25) + //wdle
           ( 1 << 26);
    WriteREG(RW_VLD_BCODE_SA, a198); //(UINT32)prVdec->rDram.pu1Bp_1 >> 3); //198 Bp_1


    //-----------------------------------------------------
    // VLD_reg_200 : MV resolution (half or full pel)
    //-----------------------------------------------------
    {
        int mv_sram_use_ok, mv_sram_use;
        int my_forward /*my_output*/;
        int bit22;
        if ((prVdec->rOther.i4CodecVersion == WMV1)
                || (prVdec->rOther.i4CodecVersion == WMV2))
        {
            prVdec->rOther.u4ResetMVDram = 0;
            mv_sram_use_ok = 0;
            mv_sram_use = 1;
            //ilii
            // 1:  整張picture有超過一個以上的slice
            // 0: 整張picture只有一個slice
            if (prVdec->rOther.i4CodecVersion == WMV2)
                bit22 = (!(prVdec->rSeqHdrR.u4NumMBY
                           == (prVdec->rSeqHdrR.u4NumMBY
                               / (prVdec->rSMProfile.i4SliceCode))));
            else
                bit22 = (!(prVdec->rSeqHdrR.u4NumMBY
                           == (prVdec->rSeqHdrR.u4NumMBY
                               / (prVdec->rSMProfile.i4SliceCode - 22)))); // 5bits for WMV1, -22 to 0~7

            a200 = 0x0020003a + (1<<11)+ (prVdec->rOther.i4pred_use_wdle
                                          << 6)+ (prVdec->rOther.i4pred_use_wdle << 7)
                   + (prVdec->rOther.i4pred_use_wdle << 20)
                   + (((prVdec->rOther.i4Panning & 0xf) != 0) << 9)
                   + ((prVdec->rOther.i4CodecVersion == WMV1
                       || prVdec->rOther.i4CodecVersion == WMV2)<<15)
                   + (prVdec->rOther.u4ResetMVDram << 18) +
                   //((!(prVdec->rSeqHdrR.u4NumMBY == (prVdec->rSeqHdrR.u4NumMBY / (prVdec->rSMProfile.i4SliceCode - 22)))) << 22) +
                   (bit22 << 22) + (mv_sram_use << 31);

        }
        else // >= WMV3
        {
            mv_sram_use_ok = ( (prVdec->rPicLayer.u1PicType == PVOP)
                               || ((prVdec->rPicLayer.u1PicType == BVOP)
                                   &&(prVdec->rPicLayer.u1FrameCodingMode
                                      != INTERLACEFIELD)) )
                             && (prVdec->rSeqHdrR.u4NumMBX < 46);
            mv_sram_use = mv_sram_use_ok;// // simulation use only ? (rand()% 2) : 0;

            my_forward = ReadREG(ucEsId,RW_MC_FWDP);
            //my_output = ReadREG(ucEsId,RW_MC_OPBUF);

            vSetMcBufPtr(ucEsId);

            if ((prVdec->rOther.u1FrameTypeLast == SKIPFRAME)
                    && (prVdec->rPicLayer.u1PicType == BVOP))
            {
                if (my_forward == 1)
                {
                    WriteREG(RW_MC_R1Y, ((UINT32)(prVdec->rDram.pu1Pic0Y) )
                             >> 9); // div 512
                    WriteREG(RW_MC_R1C, ((UINT32)(prVdec->rDram.pu1Pic0C) )
                             >> 8); // div 256
                    WriteREG(RW_MC_R2Y, ((UINT32)(prVdec->rDram.pu1Pic0Y) )
                             >> 9); // div 512
                    WriteREG(RW_MC_R2C, ((UINT32)(prVdec->rDram.pu1Pic0C) )
                             >> 8); // div 256
                }
                else
                {
                    WriteREG(RW_MC_R1Y, ((UINT32)(prVdec->rDram.pu1Pic1Y) )
                             >> 9); // div 512
                    WriteREG(RW_MC_R1C, ((UINT32)(prVdec->rDram.pu1Pic1C) )
                             >> 8); // div 256
                    WriteREG(RW_MC_R2Y, ((UINT32)(prVdec->rDram.pu1Pic1Y) )
                             >> 9); // div 512
                    WriteREG(RW_MC_R2C, ((UINT32)(prVdec->rDram.pu1Pic1C) )
                             >> 8); // div 256
                }
            }

            if (prVdec->rOther.u1FrameTypeLast == SKIPFRAME)
            {
                if ((prVdec->rOther.u4ResetMVDram == 1)
                        && ((prVdec->rPicLayer.u1PicType == IVOP)
                            || (prVdec->rPicLayer.u1PicType == PVOP)))
                    prVdec->rOther.u4ResetMVDram = 0;
                else
                {
                    prVdec->rOther.u4ResetMVDram = 1;
                }
            }
            else
            {
                if (((prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)
                        && (((prVdec->rPicLayer.i4CurrentField == 0)
                             &&(prVdec->rOther.u1FrameTypeLastTop
                                == IVOP))
                            || ((prVdec->rPicLayer.i4CurrentField == 1)
                                &&(prVdec->rOther.u1FrameTypeLastBot
                                   == IVOP))) )
                        || ((prVdec->rPicLayer.u1FrameCodingMode
                             != INTERLACEFIELD) && //ming modify @3/14
                            (prVdec->rOther.u1FrameTypeLast == IVOP) ))
                {
                    if (prVdec->rPicLayer.u1PicType == PVOP)
                        prVdec->rOther.u4new_entry_point = 0;
                    prVdec->rOther.u4ResetMVDram = 1;
                }
                else if (prVdec->rOther.u4new_entry_point == 1)
                {
                    if (prVdec->rPicLayer.u1PicType == PVOP)
                        prVdec->rOther.u4new_entry_point = 0;
                    prVdec->rOther.u4ResetMVDram = 1;
                }
                else
                    prVdec->rOther.u4ResetMVDram = 0;
            }

            if ((prVdec->rPicLayer.u1PicType == IVOP)
                    || (prVdec->rPicLayer.u1PicType == PVOP)
                    || (prVdec->rPicLayer.u1PicType == SKIPFRAME))
            {

#if 0
                /*#ifdef REPEAT_FRAME
                 if(prVdec->rOther.u4CurrPicNo == REPEAT_FRAME)
                 {
                 // do nothing
                 }
                 else
                 #endif*/
#endif
                {
                    prVdec->rOther.u1FrameTypeLast
                    = prVdec->rPicLayer.u1PicType;

                    if (prVdec->rPicLayer.i4CurrentField == 0) //top
                        prVdec->rOther.u1FrameTypeLastTop
                        = prVdec->rPicLayer.u1PicType;
                    else
                        //bot
                        prVdec->rOther.u1FrameTypeLastBot
                        = prVdec->rPicLayer.u1PicType;
                }
            } //ming modify@07/02/10

            a200 = 0x00200032 +
                   //a200 = 0x00200030 +
                   (prVdec->rOther.i4pred_use_wdle << 6)
                   + (prVdec->rOther.i4pred_use_wdle << 7)
                   + (prVdec->rOther.i4pred_use_wdle << 20)
                   + (((prVdec->rOther.i4Panning & 0xf) != 0) << 9)
                   + ((prVdec->rOther.i4CodecVersion == WMV1
                       || prVdec->rOther.i4CodecVersion == WMV2)<<15)
                   + (prVdec->rOther.u4ResetMVDram << 18) + (mv_sram_use
                           << 31);// +
            //((prVdec->rOther.i4X9MVMode == ALL_1MV_HALFPEL_BILINEAR) << 25) +
            //((prVdec->rEntryPoint.fgUVHpelBilinear == 1) << 26)+
            //(prVdec->rPicLayer.i4CurrentTemporalField << 27) +
            //(prVdec->rSeqLayer.fgVC1 << 28) +
            //(pWMVDec->m_bUseOppFieldForRef << 29);
        }
        //a200 = a200 | 0x1400;
        //LOG(0,"a200 %x\n",a200);
        WriteREG(RW_VLD_DCMVSEL, a200);
    }

    //-----------------------------------------------------
    // VLD_reg_206 : MV Range
    //-----------------------------------------------------
    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        a206 = (prVdec->rOther.i4CodecVersion <= WMV2) + //wmv7/8
               ((prVdec->rOther.i4CodecVersion <= WMV2) << 1)+ //wmv7/8
               ((prVdec->rOther.i4CodecVersion == WMV3) << 2)+ //wmv9
               (0 << 3)+ //wmva
               ((prVdec->rEntryPoint.i4MVRangeIndex&0xf)<<4)+//mv_range
               (0 << 8)+//interlace_420
               (0 << 9) + //interlace_411
               (((prVdec->rOther.i4X9MVMode == ALL_1MV_HALFPEL_BILINEAR)
                 || (prVdec->rOther.i4X9MVMode == ALL_1MV_HALFPEL))
                << 10) +//VWMVHalfFilterSel
               ((prVdec->rEntryPoint.fgUVHpelBilinear == 1) << 11)+ //VWMVrounding
               (0 << 12) + //VWMVHalfPelBackRef
               (0 << 13) + //VWMVPostCrescent
               (0 << 14) + //VWMVUseOppieldRef
               (0 << 15) + //VWMVinter420Field2ref
               (0 << 16)+ //VWMVinter420FieldCurr
               (0 << 17)+ //VWMVinter420FieldCurrTmp
               (0 << 18)+ //vwmv_420field_postrc1
               (prVdec->rSMProfile.fgFrmHybridMVOn << 19) + //vwmv_8hybrid_mv_on
               (prVdec->rPicLayer.fgMvResolution << 20) + //vwmv_8mvresolution
               (use_interpredictor << 21)
               + (prVdec->rPicLayer.fgInterlaceV2 << 23);
    }
    else // >= WMV3
    {
        a206 = 0 + //wmv7
               (0 << 1)+ //wmv8
               ((prVdec->rOther.i4CodecVersion == WMV3)<<2)+ //wmv9
               ((prVdec->rOther.i4CodecVersion == WMVA)<<3)+ //wmva
               ((prVdec->rEntryPoint.i4MVRangeIndex&0xf)<<4)+//mv_range
               (prVdec->rPicLayer.fgInterlaceV2 << 8)+//interlace_420
               (0 << 9) + //interlace_411
               (((prVdec->rOther.i4X9MVMode == ALL_1MV_HALFPEL)
                 || (prVdec->rOther.i4X9MVMode
                     == ALL_1MV_HALFPEL_BILINEAR)) << 10) +//VWMVHalfFilterSel
               ((prVdec->rEntryPoint.fgUVHpelBilinear == 1) << 11)+ //VWMVrounding
               //      (prVdec->rPicLayer.i4CurrentTemporalField << 12) + //VWMVHalfPelBackRef
               (prVdec->rPicLayer.fgBackRefUsedHalfPel << 12) + //VWMVHalfPelBackRef
               (prVdec->rSeqLayer.fgVC1 << 13) + //VWMVPostCrescent
               (prVdec->rPicLayer.fgUseOppFieldForRef << 14) + //VWMVUseOppieldRef
               ((prVdec->rPicLayer.fgTwoRefPictures
                 && (prVdec->rPicLayer.u1FrameCodingMode
                     == INTERLACEFIELD)) << 15) + //VWMVinter420Field2ref
               (((prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)
                 &&(prVdec->rPicLayer.i4CurrentField == 1)) << 16)+ //VWMVinter420FieldCurr
               ((prVdec->rPicLayer.i4CurrentTemporalField) << 17)+ //VWMVinter420FieldCurrTmp
               ((prVdec->rPicLayer.fgPostRC1) <<18)
               + ((prVdec->rPicLayer.fgInterlaceV2) << 23)
               + ((prVdec->rOther.i4CodecVersion == WMVA
                   && prVdec->rPicLayer.u1PicType == BVOP ) << 24)
               + (((prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)
                   || (prVdec->rPicLayer.u1FrameCodingMode
                       == INTERLACEFRAME)) <<25);
        //          ((prVdec->rPicLayer.u1FrameCodingMode == PROGRESSIVE && (prVdec->rPicLayer.u1PicType == BVOP)) <<26);
    }
    WriteREG(RW_VLD_MVVOP_SEL, a206);

    //-----------------------------------------------------
    // VLD_reg_194 : set top_mv1 or bot_mv1
    //-----------------------------------------------------
    {
        //ilii
        int a194;
        if ((prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)
                &&(prVdec->rPicLayer.i4CurrentField == 1)
                &&(prVdec->rOther.i4CodecVersion >= WMV3)) //bot
        {
            a194 = (((UINT32)prVdec->rDram.pu1Mv_1_2 >> 2) & 0x3FFFFF )+ //dram_addr_base
                   ((prVdec->rSeqHdrR.u4NumMBX * 4) << 22); //Mv_1_2
            WriteREG(VLD_REG_202, (((UINT32)prVdec->rDram.pu1Mv_1_2 >> 24)
                                   & 0x3F ));
        }
        else
        {
            a194 = (((UINT32)prVdec->rDram.pu1Mv_1 >> 2) & 0x3FFFFF )+ //dram_addr_base
                   ((prVdec->rSeqHdrR.u4NumMBX * 4) << 22); //Mv_1_2
            WriteREG(VLD_REG_202, (((UINT32)prVdec->rDram.pu1Mv_1 >> 24)
                                   & 0x3F ));
        }
        WriteREG(RW_VLD_MVSA, a194); //Robert: Temporary Marked! Interlace Field Needs it!!!!
    }

    //-----------------------------------------------------
    // VLD_reg_195 : TRB & TRD
    //-----------------------------------------------------
    TRB=prVdec->rPicLayer.i4BNumerator
        * prVdec->rPicLayer.i4BFrameReciprocal;
    TRD=prVdec->rPicLayer.i4BNumerator
        * prVdec->rPicLayer.i4BFrameReciprocal - 256;
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        a195 = (TRB << 0) + (TRD << 16);
        WriteREG(RW_VLD_DIRE_MD, a195);
    }
    else
    {
        int wmv78_slice_num;
        int a199;

        if (prVdec->rOther.i4CodecVersion == WMV2)
            wmv78_slice_num = prVdec->rSeqHdrR.u4NumMBY
                              / prVdec->rSMProfile.i4SliceCode;
        else
            wmv78_slice_num = (prVdec->rSeqHdrR.u4NumMBY
                               / (prVdec->rSMProfile.i4SliceCode - 22));

        if (prVdec->rSeqHdrR.u4NumMBY < 33)
        {
            a195 = 0;
            for (i=1; i<prVdec->rSeqHdrR.u4NumMBY; i++)
                a195 += ((i%wmv78_slice_num) == 0) << (i-1);
            a199 = 0;
        }
        else
        {
            a195 = 0;
            a199 = 0;
            for (i=1; i<33; i++)
                a195 += ((i%wmv78_slice_num) == 0) << (i-1);

            for (i=33; i<prVdec->rSeqHdrR.u4NumMBY; i++)
                a199 += ((i%wmv78_slice_num) == 0) << (i-33);
        }

        WriteREG(RW_VLD_DIRE_MD, a195);
        WriteREG(RW_VLD_DIRE_MD_IL, a199);
    }

    //-----------------------------------------------------
    // VLD_reg_207 : interlace 420
    //-----------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        int use_half = use_quarter_pel_mv ? 1 : 2;
        int a207;
        a207 = (use_half*prVdec->rPicLayer.i4MaxZone1ScaledFarMVX & 0x7f)
               + (((use_half*prVdec->rPicLayer.i4MaxZone1ScaledFarMVY)
                   & 0x1f) << 8)
               + (((use_half*prVdec->rPicLayer.i4Zone1OffsetScaledFarMVX)
                   & 0x7f) << 16)
               + (((use_half*prVdec->rPicLayer.i4Zone1OffsetScaledFarMVY)
                   & 0x1f) << 24);
        WriteREG(RW_VLD_MVF1, a207);
    }

    //-----------------------------------------------------
    // VLD_reg_208 : interlace 420
    //-----------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        int use_half = 1;//use_quarter_pel_mv ? 1 : 2;
        int a208;
        a208 = ((use_half*prVdec->rPicLayer.i4FarFieldScale1) & 0x3fff)
               + (((use_half*prVdec->rPicLayer.i4FarFieldScale2) & 0x1ff)
                  << 14) + (((use_half
                              *prVdec->rPicLayer.i4NearFieldScale) & 0x1ff) << 23);
        WriteREG(RW_VLD_MVF2, a208);
    }

    //-----------------------------------------------------
    // VLD_reg_209 : interlace 420
    //-----------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        int use_half = use_quarter_pel_mv ? 1 : 2;
        int a209;
        a209 = ((use_half*prVdec->rPicLayer.i4MaxZone1ScaledFarBackMVX)
                & 0x7f)
               + (((use_half*prVdec->rPicLayer.i4MaxZone1ScaledFarBackMVY)
                   & 0x1f) << 8) + (((use_half
                                      *prVdec->rPicLayer.i4Zone1OffsetScaledFarBackMVX) & 0x7f)
                                    << 16) + (((use_half
                                                *prVdec->rPicLayer.i4Zone1OffsetScaledFarBackMVY) & 0x1f)
                                              << 24);
        WriteREG(RW_VLD_MVF3, a209);
    }

    //-----------------------------------------------------
    // VLD_reg_210 : interlace 420
    //-----------------------------------------------------
    if (prVdec->rOther.i4CodecVersion >= WMV3)
    {
        int use_half = 1;//use_quarter_pel_mv ? 1 : 2;
        int a210;
        a210 = ((use_half*prVdec->rPicLayer.i4FarFieldScaleBack1) & 0x3fff)
               + (((use_half*prVdec->rPicLayer.i4FarFieldScaleBack2)
                   & 0x1ff) << 14) + (((use_half
                                        *prVdec->rPicLayer.i4NearFieldScaleBack) & 0x1ff) << 23);
        WriteREG(RW_VLD_MVF4, a210);
    }

    //-----------------------------------------------------
    // VLD_reg_212 : interlace 420
    //-----------------------------------------------------
    {
        int a212;
        a212 = (vop_type == 0) + //VopCodingTypeI
               ((vop_type == 1) << 1)+ //VopCodingTypeP
               ((vop_type == 2) << 2)+ //VopCodingTypeB
               (0 << 3)+ //VopCodingTypeS (mp4)
               (0 << 4)+ //MPEG1or2
               (0 << 5)+ // Divx311
               (0 << 6)+ //MP4
               (0 << 7)+ //ShortVideo
               (1 << 8)+ //WMV789A
               ((prVdec->rPicLayer.u1FrameCodingMode != INTERLACEFIELD)
                << 9); //WMVFramePic
        WriteREG(RW_VLD_VOP_TYPE, a212);
    }
    //----------------------------------------------
    // VLD_reg_116 : load sum_out_wmv(or mp4) fpga
    //----------------------------------------------
    // No need for emuSoft
    //        WriteREG(RW_VLD_116, 0x00000001);
    //        WriteREG(RW_VLD_116, 0x00000000);

    //------------------------------
    //   VLD RISC WRITE PATTERN
    //------------------------------
    //initialize before load r_ptr and shift_bit
    // HMLin suggests to mark.
    //        WriteREG(RW_VLD_WMV_MODE, 0x00000000);

    //-------------------------------------------
    // VLD_reg_44 : r_ptr (only for simulation)
    //-------------------------------------------

    //-------------------------------------------
    // VLD_reg_45 : v_start address in dram
    //-------------------------------------------

    //-------------------------------------------
    // VLD_reg_46 : v_end address in dram
    //-------------------------------------------

    //-------------------------------------------
    // VLD_reg_35 : load bitstream from dram
    //-------------------------------------------

    //-------------------------------------------
    // read VLD_reg to shift the barrel shifter
    //-------------------------------------------

    //-------------------------------------------
    // VLD_reg_34~39,42 : VLD parameter setting
    //-------------------------------------------
    if (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)
    {
        if (prVdec->rPicLayer.i4CurrentField == 0)
            tmp = 1; //top
        else
            tmp = 2; //bot
    }
    else
        tmp = 3; //frame


    a34 = (tmp <<30) + //picture_structure << 30) +
          (0<<29) + //frame_pred_frame_dct << 29) +
          (0<<28) + //concealment_motion_vectors << 28) +
          (0<<27) + //q_scale_type << 27) +
          // (top_field_first << 26) +
          // (full_pel_forward_vector << 25) +
          // (full_pel_backward_vector << 24) +
          // ((mp4_state->hdr.prediction_type) << 21) + //picture_coding_type << 21) +
          (vop_type << 21) + //picture_coding_type << 21) +
          (0<<20)+ //intra_vlc_format << 20) +
          // (intra_dc_precision << 18) +
          // (alternate_scan << 17) +
          // (reset_of_mat << 16) +
          // ((mp4_state->hdr.fcode_for) << 12)+ //f_code[0][0] << 12) +
          // ((pWMVDec->uiFCode) << 12)+ //f_code[0][0] << 12) +
          // it seems that the f_code not exists in WMV
          // ((mp4_state->hdr.fcode_back) << 8); //(f_code[0][1] << 8) +
          // ((mp4_state->hdr.fcode_back)<<4); //f_code[1][0] << 4) +
          // it seems that the f_code not exists in WMV
          (0); //f_code[1][1]);
    WriteREG(RW_VLD_PARA, a34);
    //prVdec->rOther.u4Datain = ReadREG(ucEsId,RW_VLD_PARA);

    //  height = (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)? prVdec->rSeqHdrR.u4PicHeightSrc/2 : prVdec->rSeqHdrR.u4PicHeightSrc;
    height
    = (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD) ? prVdec->rSeqHdrR.u4PicHeightSrc
      /2
      : prVdec->rSeqHdrR.u4PicHeightDec/*Main Profile only*/;
    a36 = (0<<31)+ //MPEG1_flag
          ((height + 15 ) << 16)+ //vertical_size
          (63 << 8)+ //max_mbl_mod/** b_buffer_size_s1 **/ << 8)+
          //          ((prVdec->rSeqHdrR.u4PicWidthSrc + 15)>>4); //picture_width_in_MB);
          (prVdec->rSeqHdrR.u4NumMBX);
    WriteREG(RW_VLD_PICSZ, a36);

#if defined(IS_WMV_POST_MT5368)
    // !!!1006 take care of interlace case
    a36 = ((prVdec->rSeqHdrR.u4NumMBY - 1) << 16) | (prVdec->rSeqHdrR.u4NumMBX - 1);
    WriteREG(RW_VLD_TOP_PIC_MB_SIZE_M1, a36);
    a36 = (prVdec->rSeqHdrR.u4PicHeightDec << 16) | (prVdec->rSeqHdrR.u4PicWidthDec);
    WriteREG(RW_VLD_TOP_PIC_PIX_SIZE, a36);
#endif

    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        a37 = (0<<24)+ //part_dec_sta_y /* mc_start_mb_row*/ << 24) +
              (63<<16)+ //part_dec_end_y /* mc_end_mb_row*/  << 16) +
              (0<<8); //b_buf_start  /*b_buffer_start_row*/ << 8);
    }
    else
    {
        a37 = (0<<24)+ //part_dec_sta_y /* mc_start_mb_row*/ << 24) +
              (127<<16)+ //part_dec_end_y /* mc_end_mb_row*/  << 16) +
              (0<<8); //b_buf_start  /*b_buffer_start_row*/ << 8);
    }
    WriteREG(RW_VLD_MBROWPRM, a37);

    a38 = (0 /* digest_mb_x */<< 16) + (0 /* digest_mb_y */);
    WriteREG(RW_VLD_DIGMBSA, a38);

    a39 = (0<<24) + //H_scale /*h_scale*/ << 24) +
          (0<<16) + //V_scale /*v_scale*/  << 16) +
          (0<<8); //write_to_digest_buf /*w_to_dig_buf*/ << 8);
    WriteREG(RW_VLD_SCALE, a39);

    a42 = (0 /* digest_MB_y_offset1 */<< 16)
          + (0 /* digest_MB_y_offset1 */<< 8) + (0
                                                 /* digest_MB_y_offset1 */<< 0);
    WriteREG(RW_VLD_DIGMBYOFF, a42);

    a35 = (1 << 27) + //dec_b_pic_all
          (0 << 8) + //mc_start_mb_addr
          (prVdec->rSeqHdrR.u4NumMBX); //mc_end_mb_addr
    WriteREG(RW_VLD_PROC, a35);

    //-------------------------------------------
    // VLD_reg_135 : load_sum_risc (for fpga)
    //-------------------------------------------
    ReadREG(ucEsId,VLD_REG_OFST);
    WriteREG(RW_VLD_WMV_LOAD_SUM, 0x00000001);
    WriteREG(RW_VLD_WMV_LOAD_SUM, 0x00000000);

#if (defined(CC_MT5398) || defined(CC_MT5399) || defined(VDEC_IS_POST_MT5880))
    WriteREG(RW_VLD_116, ReadREG(ucEsId, RW_VLD_116) & (~HDRPRS)); // VLD_0x1D0[8] = 0
#endif

    //-------------------------------------------
    // VLD_reg_131 : wmv version setting
    //-------------------------------------------
    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        a131 = (prVdec->rOther.i4CodecVersion == WMV3) + //wmv9
               ((prVdec->rOther.i4CodecVersion == WMV2)<<1)+ //wmv8
               ((prVdec->rOther.i4CodecVersion == WMV1)<<2)+ //wmv7
               (1 << 8) + // profile : [main]
               (0 << 9) + // profile : [advance]
               (prVdec->rSMProfile.fgYUV411 << 16) + //interlace_411 flag
               (0 << 17)+ //interlace_420 flag
               (1 << 24) + // frame_picture
               (0 << 25); //bottom_field
    }
    else
    {
        a131 = (((prVdec->rSeqLayer.i4WMV3Profile & 0x2) == 0) << 0) + // wmv9_flag (this code only can decode the WMV9 bitstream)
               ((prVdec->rSeqLayer.i4WMV3Profile & 0x2) << 8) + // profile : [advanced //main]
               (prVdec->rSMProfile.fgYUV411 << 16) + //interlace_411 flag
               (prVdec->rPicLayer.fgInterlaceV2 << 17)+ //interlace_420 flag
               ((prVdec->rPicLayer.u1FrameCodingMode != INTERLACEFIELD)
                << 24) + // frame_picture
               ((tmp == 2) << 25) + //top field:0  bottom_field:1
               (((prVdec->rPicLayer.i4CurrentTemporalField == 1)
                 &&(prVdec->rPicLayer.u1FrameCodingMode
                    == INTERLACEFIELD)) << 26); //1st field:0, 2nd field:1
    }
    WriteREG(RW_VLD_WMV_MODE, a131);

    //-------------------------------------------------
    // VLD_reg_132 : Quantizer type , half_step, slice
    //-------------------------------------------------
    a132 = ((prVdec->rPicLayer.fgUse3QPDZQuantizer) << 0)
           + ((prVdec->rPicLayer.fgHalfStep) << 8) //+
           //((_fgSliceWMVA) << 16)
           //((prVdec->rOther.i4FrameXformMode) << 16) //+
           //// { 0: XFORMMODE_8x8,
           ////   1: XFORMMODE_8x4,
           ////   2: XFORMMODE_4x8,
           ////   3: XFORMMODE_MBSWITCH};
           //((pmbmd->m_chMBMode) << 20)
           + (prVdec->rOther.u4BPRawFlag << 16);
    WriteREG(RW_VLD_WMV_PARA1, a132);

    //-------------------------------------------------
    // VLD_reg_133 : TTMB, QP_value, ...
    //-------------------------------------------------
    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        high_rate_switch = (prVdec->rPicLayer.i4PicQtIdx <= MAXHIGHRATEQP)
                           && (prVdec->rOther.i4CodecVersion == WMV3);
        ttfrm = (prVdec->rOther.fgMBXformSwitching == 1) ? 0
                : (prVdec->rOther.i4FrameXformMode == 4) ? 3
                : (prVdec->rOther.i4FrameXformMode & 0x3);

        a133 = (prVdec->rOther.i4CBPTable << 0)
               + ((prVdec->rOther.fgDCPred_IMBInPFrame) << 3)
               + ((prVdec->rOther.fgMBXformSwitching == 1) << 4) + //TTMBF
               ((ttfrm) << 5) + ((prVdec->rPicLayer.fgMvResolution) << 7 )
               + ((prVdec->rPicLayer.fgIntraDCTDCTable) << 8)
               + ((prVdec->rPicLayer.fgDCTTable_MB) << 9)
               + ((high_rate_switch) << 10)
               + ((prVdec->rSMProfile.fgRotatedIdct == 0) << 11)
               + ((prVdec->rPicLayer.u4DCTACIntraTableIndx) << 12)
               + ((prVdec->rPicLayer.u4DCTACInterTableIndx) << 14) + //ming added
               ((prVdec->rPicLayer.i4DCStepSize) << 16)
               + ((prVdec->rPicLayer.i4StepSize) << 24);
    }
    else
    {
        high_rate_switch = prVdec->rPicLayer.i4PicQtIdx <= MAXHIGHRATEQP;// && prVdec->rOther.i4CodecVersion == WMV3;
        ttfrm = (prVdec->rOther.fgMBXformSwitching == 1) ? 0
                : (prVdec->rOther.i4FrameXformMode == 4) ? 3
                : (prVdec->rOther.i4FrameXformMode & 0x3);

        a133 = (prVdec->rOther.i4CBPTable << 0)
               + ((prVdec->rOther.fgMBXformSwitching == 0) << 4) + //TTMBF
               ((ttfrm) << 5) + ((prVdec->rPicLayer.fgIntraDCTDCTable)
                                 << 8) + ((prVdec->rPicLayer.fgDCTTable_MB) << 9)
               + ((high_rate_switch) << 10)
               + ((prVdec->rSMProfile.fgRotatedIdct == 0) << 11)
               + ((prVdec->rPicLayer.u4DCTACIntraTableIndx) << 12)
               + ((prVdec->rPicLayer.u4DCTACInterTableIndx) << 14) + //ming added
               ((prVdec->rPicLayer.i4DCStepSize) << 16)
               + ((prVdec->rPicLayer.i4StepSize) << 24);
    }
    //Printf("a133 : %08x\n",a133);
    //Printf("prVdec->rPicLayer.i4DCStepSize :%d\n",prVdec->rPicLayer.i4DCStepSize);
    //Printf("prVdec->rPicLayer.i4StepSize :%d\n",prVdec->rPicLayer.i4StepSize);

    WriteREG(RW_VLD_WMV_PARA2, a133);

    //-------------------------------------------------
    // VLD_reg_136 : MV parameter setting for VLD
    //-------------------------------------------------
    b_fraction_chk = ((prVdec->rPicLayer.i4BNumerator
                       * prVdec->rPicLayer.i4BFrameReciprocal) >> 7) ? 1 : 0;

    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        a136 = ((prVdec->rEntryPoint.i4MVRangeIndex) << 0)
               + ((prVdec->rOther.i4X9MVMode) << 8) + ((b_fraction_chk)
                       << 16) + (0 << 17) + (0 << 18) + (0 << 19) + (0 << 24); //((lookup_mv_tbl_number)     << 24);
    }
    else
    {
        a136 = ((prVdec->rEntryPoint.i4MVRangeIndex) << 0)
               + ((prVdec->rOther.i4X9MVMode) << 8) + ((b_fraction_chk)
                       << 16) + ((prVdec->rPicLayer.fgTwoRefPictures
                                  && (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD))
                                 << 17) + (prVdec->rEntryPoint.i4ExtendedDMVX << 18)
               + (prVdec->rEntryPoint.i4ExtendedDMVY << 19)
               + ((prVdec->rOther.i4MvTable) << 24);
    }
    WriteREG(RW_VLD_WMV_PARA3, a136);

    if ((prVdec->rOther.i4CodecVersion == WMV1)
            || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        a137 = (prVdec->rSMProfile.i4skip_bit_mode_v87 << 8);
#if defined(IS_WMV_POST_MT5368)
        if (prVdec->rSeqHdrR.u4PicWidthSrc > 2048)
        {
            a137 |= (1 << 21);
        }
#endif
        WriteREG(RW_VLD_WMV_BP, a137);
    }

    //-------------------------------------------------
    // VLD_reg_148 : VLD mode setting
    //-------------------------------------------------
    a138 = 0 + (0 << 25);
#ifdef USING_WMV_FIND_HDR_FIX
    a138 |= (ReadREG(ucEsId, RW_VLD_BP_MODE_FIRE) & BP_MODE_WMV_FIND_HDR);
#endif
    WriteREG(RW_VLD_BP_MODE_FIRE, a138);
    //-------------------------------------------------
    // VLD_reg_140 : VOPQUANT setting (MBQUANT)
    //-------------------------------------------------
    a140 = ((prVdec->rOther.i4Panning & 0xf) << 0)
           + (prVdec->rOther.fgDQuantOn << 8)
           + ((prVdec->rOther.u1DiffQtProfile == 3) << 9)
           + ((prVdec->rOther.fgDQuantBiLevel & 0x1) <<10)
           + ((prVdec->rOther.u1DQuantBiLevelStepSize & 0x1f) <<16);
    //Printf("prVdec->rOther.u1DQuantBiLevelStepSize : %d\n",(prVdec->rOther.u1DQuantBiLevelStepSize & 0x1f));
    WriteREG(RW_VLD_WMV_QM, a140);

    //-------------------------------------------------
    // VLD_reg_142 : MV table index for VLD
    //-------------------------------------------------
    a142 = ((prVdec->rOther.i4X9MVMode == MIXED_MV)
            &&((prVdec->rPicLayer.u1PicType == PVOP)
               ||(prVdec->rPicLayer.u1PicType == BVOP)))
           + (((prVdec->rOther.i4CodecVersion <= WMV2)
               && (prVdec->rOther.i4MvTable)) << 1)
           + (((prVdec->rPicLayer.i4Overlap & 4) != 0) << 8)
           + (prVdec->rOther.i4MBModeTable << 16)
           + (prVdec->rOther.i42MVBPTable << 24)
           + (prVdec->rOther.i44MVBPTable << 28);
    WriteREG(RW_VLD_WMV_TAB, a142);

    //-------------------------------------------------
    // VLD_reg_146 : parse slice picture header (unit:bit)
    //-------------------------------------------------
    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        WriteREG(RW_VLD_WMV_PBN, prVdec->rPicLayer.i4SlicePicHeaderNum);
    }
    //spork
    //WriteREG(RW_VLD_SRAM_TEST_WRITE_DATA, 0x101); //ming add for debug 12/15

    // WriteREG(0x4f8, 24); //ming add for debug 12/15

    //  WriteREG(0x04dc, 500);
    //-------------------------------------------------
    // RW_VLD_WMV_LOAD_SUM : load sum_risc (only for simulation)
    //-------------------------------------------------

    //-------------------------------------------------
    // VLD_reg_130 : hw_decode_start flag
    //-------------------------------------------------

    //--------------------------------------------
    // Dump Dram data for comparation with golden
    //--------------------------------------------
    //dump_size = (((prVdec->rSeqHdrR.u4PicWidthSrc+15)/16) * 2) * (((prVdec->rSeqHdrR.u4PicHeightSrc+31)/32)* 32);

    //_fgPrintWReg = FALSE; //Stop printing WriteREG();

    //test, //ilii, HM comment, 0 -> 1
    WriteREG(MC_REG_453, 0x00010800);
}


#ifdef WMV_MINST
void vCheckEmuByte(UCHAR ucEsId, UINT32 nBit, UINT32 u4InputWnd, UINT32 u4VFifoSa, UINT32 u4VldPosByte)
{
    UINT32 u4PosBit;
    UINT32 u4Match = 0;
	VDEC_WMV_INFO_T* prVdec = NULL;
	if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "ucEsId >= MAX_ES_NUM");
        return;
    }

    do
    {
        if (dwGetBitStream(ucEsId, 1) == u4InputWnd)
        {
            u4Match++;
        }
    } while (dwVldRPtr(ucEsId, &u4PosBit) % 16);

    if (u4Match)
    {
        LOG(7, "%d additional IW(%08X) found @ %08X\n", u4Match, u4InputWnd, u4VFifoSa + u4VldPosByte);
        //ASSERT(0);
    }
    else
    {
        LOG(7, "vCheckEmuByte passed\n");
    }

    // reset to the original position
   // fgVldInitBarrelShifter(ucEsId, u4VFifoSa + u4VldPosByte / 16 * 16);
    if(((UINT32)prVdec->rDram.pu1VFifo + u4VldPosByte / 16 * 16) > ((UINT32)prVdec->rDram.pu1FifoEnd))
	{
		fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + u4VldPosByte / 16 * 16) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
	}
	else
	{
		fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + u4VldPosByte / 16 * 16);
	}
    while (nBit)
    {
        dwGetBitStream(ucEsId, 1);
        nBit--;
    }
    ASSERT(dwGetBitStream(ucEsId, 0) == u4InputWnd);
}

#if 1
void vCheckBP(UCHAR ucEsId)
{
    UINT32 i=0;
    UINT32 u4PosByte, u4PosBit;
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
    UINT32 nBit, u4BitCountBak;
	UINT32 u4Addr,dwVLDBit,dwVLDByte;
    
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    }
    else
    {
        LOG(0, "vCheckBP : ucEsId >= MAX_ES_NUM");
        return;
    }
    
    if (!_fgWmvRestore || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_NETFLIX || prVdecEsInfo->fgSeamlessPlay))
    {
        return;
    }     
	
	if (prVdec->rOther.i4CodecVersion == WMVA)
    	WriteREG(RW_VLD_WMV_ABS,1); //xiaolei
		
    u4PosByte = dwVldRPtr(ucEsId,&u4PosBit);
    
    u4BitCountBak = prVdec->rOther.u4BitCount;

	nBit = 0;
	dwVLDByte = 0;
	dwVLDBit = 0;
	u4Addr = prVdec->rPesInfo.u4VldReadPtr + 3;
	if (u4Addr >= prVdec->rPesInfo.u4FifoEnd)
	{
        u4Addr -= (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
    }
	for (i = 0; i < prVdec->rOther.u4DecBPCnt; i++)
	{
		if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte) > ((UINT32)prVdec->rDram.pu1FifoEnd))
		{
			fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
		}
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte);
		}
	
        dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);

		if (prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
		{
			LOG(7,"!!WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte, prVdec->rOther.arDecBPPos[i].u4VldPosBit, prVdec->rOther.arDecBPPos[i].u4BPInputWnd, dwGetBitStream(ucEsId, 0));
			nBit= (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte) - (u4Addr + 1));//*8 + prVdec->rOther.arDecBPPos[0].u4VldPosBit;
	    	fgVldInitBarrelShifter(ucEsId,u4Addr+1);
			while((dwVLDByte != prVdec->rOther.arDecBPPos[i].u4VldPosByte)
					|| (dwVLDBit !=  prVdec->rOther.arDecBPPos[i].u4VldPosBit))
			{
				
				dwGetBitStream(ucEsId, 1);
				dwVLDByte = dwVldRPtr(ucEsId,&dwVLDBit);
				
				if(dwVLDByte > prVdec->rOther.arDecBPPos[i].u4VldPosByte)
				{
					LOG(7,"dwVLDByte 0x%x, dwVLDBit %d, rby 0x%x, rbit %d\n",dwVLDByte,dwVLDBit,prVdec->rOther.arDecBPPos[i].u4VldPosByte,prVdec->rOther.arDecBPPos[i].u4VldPosBit);
					break;
				}
			}
		}
		
		if (prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
		{
			//for(nBit=0;nBit<(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte)%16+4);nBit++) //for debug
			//	printf("%d ",*(UCHAR *)VIRTUAL(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte)/16*16+nBit));
			//printf("\n");
			LOG(7,"!!WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte, prVdec->rOther.arDecBPPos[i].u4VldPosBit, prVdec->rOther.arDecBPPos[i].u4BPInputWnd, dwGetBitStream(ucEsId, 0));
			
			fgVldInitBarrelShifter(ucEsId, ((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte));
			dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);
			if(prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
			{
				fgVldInitBarrelShifter(ucEsId, ((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte)-1);
				dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);
				if(prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
				{
					fgVldInitBarrelShifter(ucEsId, ((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte)+1);
					dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);
					if(prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
					{
						fgVldInitBarrelShifter(ucEsId, ((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte)-2);
						dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);
					}
				}
			}
			if(prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
				LOG(1,"!!WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte, prVdec->rOther.arDecBPPos[i].u4VldPosBit, prVdec->rOther.arDecBPPos[i].u4BPInputWnd, dwGetBitStream(ucEsId, 0));
		}
		
		fgHWDecBP(ucEsId, prVdec->rOther.arDecBPPos[i].u4BPNum, FALSE);
    		
	}
    if (i > 0)
    {
        prVdec->rOther.u4VldPosByte = u4PosByte;
        prVdec->rOther.u4VldPosBit = u4PosBit;
#if 1
        nBit = u4BitCountBak;
        //fgVldInitBarrelShifter(ucEsId, u4SetPosBak);  megaa 20100603: no need to do BS init here because we have already set BS to the last BP end point
        while (nBit)
        {
            if (nBit >= 24)
            {
                dwGetBitStream(ucEsId, 24);
                nBit -= 24;
            }
            else
            {
                dwGetBitStream(ucEsId, 1);
                nBit--;
            }
        }
        //ASSERT(prVdec->rOther.u4SavedInputWnd == dwGetBitStream(ucEsId, 0));  // !!!0603
#else
        fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte);
        dwGetBitStream(ucEsId, prVdec->rOther.u4VldPosBit);

        if (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
        {
            LOG(7, "WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte, prVdec->rOther.u4VldPosBit, prVdec->rOther.u4SavedInputWnd, dwGetBitStream(ucEsId, 0));
            //ASSERT(0);
            fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.u4VldPosByte / 16 * 16));
            nBit = 0;
            while (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
            {
                dwGetBitStream(ucEsId, 1);
                nBit++;
            }
            vCheckEmuByte(ucEsId, nBit, prVdec->rOther.u4SavedInputWnd, (UINT32)prVdec->rDram.pu1VFifo, prVdec->rOther.u4VldPosByte);
        }
#endif
    }
}

#endif

#if 0
void vCheckBP(UCHAR ucEsId)
{
    UINT32 i;
    UINT32 u4PosByte, u4PosBit;
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;
    UINT32 nBit, u4BitCountBak;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    }
    else
    {
        LOG(0, "vCheckBP : ucEsId >= MAX_ES_NUM");
        return;
    }
   #ifdef ENABLE_MULTIMEDIA
    if (!_fgWmvRestore || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_NETFLIX || prVdecEsInfo->fgSeamlessPlay))
   #endif
    {
        return;
    }

    u4PosByte = dwVldRPtr(ucEsId,&u4PosBit);

    u4BitCountBak = prVdec->rOther.u4BitCount;

    for (i = 0; i < prVdec->rOther.u4DecBPCnt; i++)
    {
        //fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte);
	    if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte) > ((UINT32)prVdec->rDram.pu1FifoEnd))
		{
			fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
		}
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte);
		}
	
        dwGetBitStream(ucEsId, prVdec->rOther.arDecBPPos[i].u4VldPosBit);
        if (prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
        {
            LOG(5, "!!WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[i].u4VldPosByte, prVdec->rOther.arDecBPPos[i].u4VldPosBit, prVdec->rOther.arDecBPPos[i].u4BPInputWnd, dwGetBitStream(ucEsId, 0));
            //ASSERT(0);
            //fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.arDecBPPos[i].u4VldPosByte / 16 * 16));
            if(((UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.arDecBPPos[i].u4VldPosByte / 16 * 16)) > ((UINT32)prVdec->rDram.pu1FifoEnd))
			{
				fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.arDecBPPos[i].u4VldPosByte / 16 * 16)) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
			}
			else
			{
				fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.arDecBPPos[i].u4VldPosByte / 16 * 16));
			}
            nBit = 0;
            while (prVdec->rOther.arDecBPPos[i].u4BPInputWnd != dwGetBitStream(ucEsId, 0))
            {
                dwGetBitStream(ucEsId, 1);
                nBit++;
            }
            vCheckEmuByte(ucEsId, nBit, prVdec->rOther.arDecBPPos[i].u4BPInputWnd, (UINT32)prVdec->rDram.pu1VFifo, prVdec->rOther.arDecBPPos[i].u4VldPosByte);
        }
        fgHWDecBP(ucEsId, prVdec->rOther.arDecBPPos[i].u4BPNum, FALSE);
    }

    if (i > 0)
    {
        prVdec->rOther.u4VldPosByte = u4PosByte;
        prVdec->rOther.u4VldPosBit = u4PosBit;
#if 1
        nBit = u4BitCountBak;
        //fgVldInitBarrelShifter(ucEsId, u4SetPosBak);  megaa 20100603: no need to do BS init here because we have already set BS to the last BP end point
        while (nBit)
        {
            if (nBit >= 24)
            {
                dwGetBitStream(ucEsId, 24);
                nBit -= 24;
            }
            else
            {
                dwGetBitStream(ucEsId, 1);
                nBit--;
            }
        }
        //ASSERT(prVdec->rOther.u4SavedInputWnd == dwGetBitStream(ucEsId, 0));  // !!!0603
#else
        fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte);
        dwGetBitStream(ucEsId, prVdec->rOther.u4VldPosBit);

        if (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
        {
            LOG(7, "WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte, prVdec->rOther.u4VldPosBit, prVdec->rOther.u4SavedInputWnd, dwGetBitStream(ucEsId, 0));
            //ASSERT(0);
            fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.u4VldPosByte / 16 * 16));
            nBit = 0;
            while (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
            {
                dwGetBitStream(ucEsId, 1);
                nBit++;
            }
            vCheckEmuByte(ucEsId, nBit, prVdec->rOther.u4SavedInputWnd, (UINT32)prVdec->rDram.pu1VFifo, prVdec->rOther.u4VldPosByte);
        }
#endif
    }
}
#endif

#endif

BOOL fgHWDecBP(UCHAR ucEsId, UINT32 dwBpNum, BOOL fgRecStatus)
{
    //int i, a = 0;
    int vop_type;
    UINT32 u4Tmp;
    //UINT32 dwInputWindow2;
    VDEC_WMV_INFO_T* prVdec = NULL;
    int height, a36, a37, a35, a137;
    unsigned long dwTmp;
#ifdef WMV_MINST
        UINT32 u4DecBPCnt;
#endif

#if defined(IS_WMV_POST_MT5368)
  #ifdef CC_WMV_EMULATION
    UINT32 u4VldRpByte0, u4VldRpBit0, u4VldRpByte1, u4VldRpBit1, u4BpBitCnt, u4RpMove;
  #endif
#endif

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "fgHWDecBP : ucEsId >= MAX_ES_NUM");
        return 0;
    }

#ifdef WMV_MINST
    u4DecBPCnt = prVdec->rOther.u4DecBPCnt;
    if (fgRecStatus)
    {
        prVdec->rOther.arDecBPPos[u4DecBPCnt].u4BPNum = dwBpNum;
        prVdec->rOther.arDecBPPos[u4DecBPCnt].u4VldPosByte = dwVldRPtr(ucEsId,&prVdec->rOther.arDecBPPos[u4DecBPCnt].u4VldPosBit);
        prVdec->rOther.arDecBPPos[u4DecBPCnt].u4BPInputWnd = dwGetBitStream(ucEsId, 0);
        if (prVdec->rOther.arDecBPPos[u4DecBPCnt].u4BPInputWnd == 0)
        {
            LOG(7, "BP IW == 0 @ %08X(bit%d)\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.arDecBPPos[u4DecBPCnt].u4VldPosByte, prVdec->rOther.arDecBPPos[u4DecBPCnt].u4VldPosBit);
        }
        prVdec->rOther.u4DecBPCnt++;
    }
#else
    UNUSED(fgRecStatus);
#endif

    //  int height = (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD)? prVdec->rSeqHdrR.u4PicHeightSrc/2 : prVdec->rSeqHdrR.u4PicHeightSrc;
    height =
    (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD) ? prVdec->rSeqHdrR.u4PicHeightSrc
    /2
    : prVdec->rSeqHdrR.u4PicHeightDec/*Main Profile only*/;
    a36 = (0<<31)+ //MPEG1_flag
          (((height) + 15 ) << 16)+ //vertical_size
          (63 << 8)+ //max_mbl_mod/** b_buffer_size_s1 **/ << 8)+
          //        ((prVdec->rSeqHdrR.u4PicWidthSrc + 15)>>4); //picture_width_in_MB);
          (prVdec->rSeqHdrR.u4NumMBX);
    a37 = (0<<24)+ //part_dec_sta_y /* mc_start_mb_row*/ << 24) +
          (127<<16)+//(63<<16)+ //part_dec_end_y /* mc_end_mb_row*/  << 16) +
          (0<<8); //b_buf_start  /*b_buffer_start_row*/ << 8);
    a35 = (1 << 27) + //dec_b_pic_all
          (0 << 8) + //mc_start_mb_addr
          (prVdec->rSeqHdrR.u4NumMBX); //mc_end_mb_addr

    dwTmp = (dwGetBitStream(ucEsId,0) >> 27) & 0xF;

    if (dwTmp == 0)
    {
        switch (dwBpNum)
        {
        case 0: //SKIP
            prVdec->rOther.u4BPRawFlag |= 0x1;
            break;
        case 1: //4MV
            prVdec->rOther.u4BPRawFlag |= 0x2;
            break;
        case 3: //DIRECT
            prVdec->rOther.u4BPRawFlag |= 0x4;
            break;
        case 4: //FIELD
            prVdec->rOther.u4BPRawFlag |= 0x8;
            break;
        case 5: //ACPRED
            prVdec->rOther.u4BPRawFlag |= 0x10;
            break;
        case 6: //OVERLAP
            prVdec->rOther.u4BPRawFlag |= 0x20;
            break;
        case 7: //FORWARD
            prVdec->rOther.u4BPRawFlag |= 0x40;
            break;
        default:
            prVdec->rOther.u4BPRawFlag = 0;
            break;
        }
    }

    if (prVdec->rOther.i4CodecVersion == WMV2)
    {
        a137 = (dwBpNum)+ (1<<16)+ //wmv8
               (prVdec->rSMProfile.i4wmv8_bp_mode<<17);
    }
    else
    {
        a137 = dwBpNum;
    }
#if defined(IS_WMV_POST_MT5368)
    if (prVdec->rSeqHdrR.u4PicWidthSrc > 2048)
    {
        a137 |= (1 << 21);
    }
#endif

    before_bp(ucEsId);

    vop_type = (prVdec->rPicLayer.u1PicType == PVOP) ? 1
               : (prVdec->rPicLayer.u1PicType == BVOP) ? 2 : 0;

    //  WriteREG(RW_VLD_WMV_ABS, (prVdec->rOther.i4CodecVersion == WMVA)); // Step 2 : Handle 0x00 0x00 0x03 0x0?
    WriteREG(RW_VLD_PARA, 0x00000000);
    WriteREG(RW_VLD_PROC, a35);
    WriteREG(RW_VLD_PICSZ, a36); // Step 1 : Set Picture Size
    WriteREG(RW_VLD_MBROWPRM, a37);
    WriteREG(RW_VLD_DIGMBSA, 0x00000000);
    WriteREG(RW_VLD_SCALE, 0x00000000);
    WriteREG(RW_VLD_DIGMBYOFF, 0x00000000);

#if defined(IS_WMV_POST_MT5368)
    // !!!1006 take care of interlace case
    a36 = ((prVdec->rSeqHdrR.u4NumMBY - 1) << 16) | (prVdec->rSeqHdrR.u4NumMBX - 1);
    WriteREG(RW_VLD_TOP_PIC_MB_SIZE_M1, a36);
    a36 = (prVdec->rSeqHdrR.u4PicHeightDec << 16) | (prVdec->rSeqHdrR.u4PicWidthDec);
    WriteREG(RW_VLD_TOP_PIC_PIX_SIZE, a36);
#endif

    //***********************************************************************
    //-----------------------------------------------------
    // VLD_reg_212 : interlace 420
    //-----------------------------------------------------
    {
        int a212;
        a212 = (vop_type == 0) + //VopCodingTypeI
               ((vop_type == 1) << 1)+ //VopCodingTypeP
               ((vop_type == 2) << 2)+ //VopCodingTypeB
               (0 << 3)+ //VopCodingTypeS (mp4)
               (0 << 4)+ //MPEG1or2
               (0 << 5)+ // Divx311
               (0 << 6)+ //MP4
               (0 << 7)+ //ShortVideo
               (1 << 8)+ //WMV789A
               ((prVdec->rPicLayer.u1FrameCodingMode != INTERLACEFIELD)
                << 9); //WMVFramePic
        WriteREG(RW_VLD_VOP_TYPE, a212);
    }

    //bp mode
    WriteREG(RW_VLD_WMV_BP, a137/*dwBpNum*/); // Step 3 : bit-plane num

    //load sum_risc
    ReadREG(ucEsId,VLD_REG_OFST);
    WriteREG(RW_VLD_WMV_LOAD_SUM, 0x1/*0x00000101*/); // Step 4 : load mpeg2_sum to wmv_sum
    WriteREG(RW_VLD_WMV_LOAD_SUM, 0x00000000); // Step 4

    //change mpeg2_mode to wmv_mode
    if (prVdec->rOther.i4CodecVersion == WMV1)
        WriteREG(RW_VLD_WMV_MODE, 0x00000004); // Step 5 : change to WMV mode
    else if (prVdec->rOther.i4CodecVersion == WMV2)
        WriteREG(RW_VLD_WMV_MODE, 0x00000002); // Step 5 : change to WMV mode
    else
        WriteREG(RW_VLD_WMV_MODE, 0x00000001); // Step 5 : change to WMV mode

    //start bp_decode
    //dwInputWindow2 = dwGetBitStream(ucEsId,0);
    prVdec->rOther.u4VldPosByte = dwVldRPtr(ucEsId,&prVdec->rOther.u4VldPosBit);

    //Printf("Rptr 0x%x, input win 0x%x, 60 = 0x%x, 56 = 0x%x\n", ReadREG(ucEsId,RO_VLD_VRPTR), ReadREG(ucEsId,RO_VLD_BARL), ReadREG(ucEsId,RO_VLD_INPUTWND), ReadREG(ucEsId,RO_VLD_FSBI));

    //test, //ilii, HM comment, 0 -> 1
    WriteREG(MC_REG_453, 0x00010800);

#ifdef CC_WMV_EMULATION
#if defined(CC_MT5395) || defined(IS_WMV_POST_MT5368)
    HalFlushInvalidateDCacheMultipleLine(prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
#else
    HalFlushInvalidateDCache();
#endif
#endif

#if 0  // debug code
    if (ucEsId == 0)
    {
        vPrintWmvPair(1);
    }
#endif

    prVdec->rOther.u4BitCount = 0;  // megaa 20100603: clear it to begin recording number of shifted bits

#if defined(IS_WMV_POST_MT5368)
  #ifdef CC_WMV_EMULATION
    {
        u4VldRpByte0 = dwVldRPtr(ucEsId, &u4VldRpBit0);
        LOG(0, "B4 BP dec, Rp = 0x%08X.%d, IW = 0x%08X\n", u4VldRpByte0, u4VldRpBit0, ReadREG(ucEsId, RO_VLD_INPUTWND));

        if (u4VldRpByte0 == 0x0000FE97)
        {
            LOG(0, "halt here!!!\n");
            //while (_fgHaltWmv);
        }
    }
  #endif
#endif

#if (defined(CC_MT5398) || defined(CC_MT5399) || defined(VDEC_IS_POST_MT5880))
    WriteREG(RW_VLD_116, ReadREG(ucEsId, RW_VLD_116) & (~HDRPRS)); // VLD_0x1D0[8] = 0
#endif

    WriteREG(RW_VLD_WMVDEC, 0x00000100); // Step 6 : start bit-plane decoding
    WriteREG(RW_VLD_WMVDEC, 0x00000000); // Step 6

    #if 1
    LOG(8, "Before waiting BP decode finish...\n");

    while (1)
    {
        //UINT32 static u4Timer = 0;
        //x_thread_delay(1);
        if ((ReadREG(ucEsId,RO_VLD_BP_DEC_END) & 0x100) == 0x100) // Step 7 : Wait for bit-plane decode finish
        {
            break;
        }
    }

    LOG(8, "BP decode finish!!!\n");
    #else
    {
        UINT32 u4Cnt1 = 0;
        UINT32 u4Cnt2 = 0;
        while (1)
        {
            u4Cnt1++;
            //UINT32 static u4Timer = 0;
            //x_thread_delay(1);
            if ((ReadREG(ucEsId,RO_VLD_BP_DEC_END) & 0x100) == 0x100) // Step 7 : Wait for bit-plane decode finish
            {
                if(u4Cnt2)
                {
                    LOG(5, "BP decode ok\n");
                }
                break;
            }
            if(u4Cnt1 > 100000000) // ~ 1 sec
            {
                if(!u4Cnt2)
                {
                    LOG(5, "BP decoding...\n");
                }
                u4Cnt2++;
                u4Cnt1 = 0;
            }
            if(u4Cnt2 > 10000000)
            {
                LOG(0, "BP decode NG...\n");
                return FALSE;
            }
        }
    }
    #endif

#ifdef WAIT_SRAM_STABLE
    u4Tmp = 0;
#if defined(IS_WMV_POST_MT5368)
    if (0)//(((ReadREG(ucEsId,RO_VLD_SRAMCTRL) >> 15) & AA_FIT_TARGET_D) == 1)
#endif
    {
        while ((ReadREG(ucEsId,RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D)==0)
        {
            u4Tmp++;
            if (u4Tmp >= 0x10000)
            {
                LOG(0, "wait SRAM stable after decoding BP failed!!!\n");
                ASSERT(0);
            }
        }
    }
#endif

    //  prVdec->rOther.u4BPRawFlag = (ReadREG(ucEsId,RW_VLD_WMV_PARA1) >>16) & 0x7F; //Save BP RAW Mode Flag.

    prVdec->rOther.fgBPDirty = TRUE;  // megaa 20100321

    // VLD_reg_116 : load sum_out_wmv(or mp4)
    WriteREG(RW_VLD_116, 0x00000001); // Step 8 : load wmv_sum to mpeg2_sum
    WriteREG(RW_VLD_116, 0x00000000); // Step 8

#if (defined(CC_MT5398) || defined(CC_MT5399) || defined(VDEC_IS_POST_MT5880))
    WriteREG(RW_VLD_116, ReadREG(ucEsId, RW_VLD_116) | HDRPRS); // VLD_0x1D0[8] = 1
#endif

#if defined(IS_WMV_POST_MT5368)
  #ifdef CC_WMV_EMULATION
    u4BpBitCnt = ReadREG(ucEsId, RW_VLD_WMV_BP_DEC_BIT_CNT);  // should read it before changing mode
  #endif
#endif

#if (!defined(CC_MT5398) && !defined(CC_MT5399) && !defined(VDEC_IS_POST_MT5880))
    //change wmv_mode to mpeg2_mode
    WriteREG(RW_VLD_WMV_MODE, 0x00000000); // Step 9 : change to mpeg2 mode
#endif

#if defined(IS_WMV_POST_MT5368)
  #ifdef CC_WMV_EMULATION
    {
        u4VldRpByte1 = dwVldRPtr(ucEsId, &u4VldRpBit1);
        u4RpMove = (u4VldRpByte1 * 8 + u4VldRpBit1) - (u4VldRpByte0 * 8 + u4VldRpBit0);
        LOG(0, "A4 BP dec, Rp = 0x%08X.%d, IW = 0x%08X\n", u4VldRpByte1, u4VldRpBit1, ReadREG(ucEsId, RO_VLD_INPUTWND));
        LOG(0, "BP dec cnt = %d Rp move = %d", u4BpBitCnt, u4RpMove);
        if (u4BpBitCnt != u4RpMove)
            LOG(0, " ==> Mismatch due to 0x03!!!!!!!!!!!!\n");
        else
            LOG(0, "\n");
        //ASSERT(u4BpBitCnt == u4RpMove || u4BpBitCnt == u4RpMove - 8);
    }
  #endif
#endif

    //  WriteREG(RW_VLD_WMV_ABS, 0x00000000);
    after_bp(ucEsId);
    prVdec->rOther.u4VldPosByte
    = dwVldRPtr(ucEsId,&prVdec->rOther.u4VldPosBit);
    //dwInputWindow2 = dwGetBitStream(ucEsId,0);
    return TRUE;
}


#ifdef WMV_MINST
void vRestoreVDec(UCHAR ucEsId, BOOL fgChkInputWnd)
{
    UINT32 nBit;
    UINT32 u4Temp;
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    }
    else
    {
        LOG(0, "vRestoreVDec : ucEsId >= MAX_ES_NUM");
        return;
    }
#ifdef ENABLE_MULTIMEDIA
    if (!_fgWmvRestore || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_NETFLIX || prVdecEsInfo->fgSeamlessPlay))
#endif
    {
        return;
    }

// vInitVDec begin
#if 0
    VDEC_WMV_INFO_T* prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    prVdec->rOther.u1DecState = VDEC_NORM_INIT_PRM;
    prVdec->rOther.u1GetSeqHdr = 0;
    prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
#endif
    vResetVDec(ucEsId);
    // initiate register files

    //vInitVdoMem(ucEsId);

    //vInitWMVMiscMem(ucEsId);
    vSetMcBufPtr(ucEsId);
    // vResetVDec(ucEsId);
    vSetVldVFifo(ucEsId);
#if 0  // !!!
    vSetFRefBuf(ucEsId,0);
    vSetBRefBuf(ucEsId,1);
#endif
// vInitVDec end

// vInitVld begin
    prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        //Robert: Enable 0x03 replancement before initializing barrel shifter.
        WriteREG(RW_VLD_WMV_ABS, 0x1);
    }
    else
    {
        WriteREG(RW_VLD_WMV_ABS, 0x0);
    }
    WriteREG(RW_VLD_VSTART, PHYSICAL((UINT32)prVdec->rDram.pu1VFifo)>>6); // (128-bit address >> 6)
    //spork
    WriteREG(RW_VLD_VEND, PHYSICAL((UINT32)prVdec->rDram.pu1FifoEnd)>>6);
    //fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo);

    // ilii, clean register
    WriteREG(RW_VLD_DECSTART, 1<<8);
    WriteREG(RW_VLD_DECSTART, 0);
    WriteREG(RW_VLD_VDOUFM, 1);
#if defined(IS_WMV_POST_MT5368)
    WriteREG(RW_VLD_TOP_OUTRANGE_MODE, 0x80000000);
#endif

    //u4BitCount = prVdec->rOther.u4BitCount;

    if (!fgChkInputWnd)  // megaa 20100603: if fgChkInputWnd, we will do BS init later
    {
        //fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte);
         if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte) > ((UINT32)prVdec->rDram.pu1FifoEnd))
		{
			fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
		}
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte);
		}
        prVdec->rOther.u4Datain = dwGetBitStream(ucEsId, prVdec->rOther.u4VldPosBit);
    }
// vInitVld end

#ifdef CC_WMV_EMULATION
    WriteREG(RW_MC_ADDRSWAP, 0x3);
    WriteREG(RW_MC_NBM_ADDRSWAP, 0x3);
#else
    WriteREG(RW_MC_ADDRSWAP, 0x2);
    WriteREG(RW_MC_NBM_ADDRSWAP, 0x2);
#endif
    u4Temp = ReadREG(ucEsId,MC_REG_325);
    u4Temp |= (0x1000);
    WriteREG(MC_REG_325, u4Temp);

    u4Temp = ReadREG(ucEsId, MC_REG_515);
    u4Temp &= 0xFFEFFFFF;  // clear bit 20 to enhance DRAM performance
#if defined(IS_WMV_POST_MT5368)
    u4Temp &= 0xFBFFFFFF;  // bit 26: PRED to DRAM
    if (cbWMVFWBP)
    {
        u4Temp |= 0x08000000;  // bit 27: BP to DRAM
    }
#else
    if (cbWMVFWBP)
    {
        u4Temp |= 0x01000000;  // bit 24: BP to DRAM
    }
#endif
    WriteREG(MC_REG_515, u4Temp);

    vInitWMVMiscMem(ucEsId);  // megaa 20120718: get working buffer from FBM every time

    vSetNewMode(ucEsId);

    if (fgChkInputWnd)
    {
#if 1
        nBit = prVdec->rOther.u4BitCount;  // megaa 20100603: save u4BitCount because it will be cleared in fgVldInitBarrelShifter
        //fgVldInitBarrelShifter(ucEsId, prVdec->rOther.u4LastStartPos);  // megaa 20100603: reset to last valid position
         if(prVdec->rOther.u4LastStartPos > ((UINT32)prVdec->rDram.pu1FifoEnd))
		{
			fgVldInitBarrelShifter(ucEsId, (prVdec->rOther.u4LastStartPos - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
		}
		else
		{
			fgVldInitBarrelShifter(ucEsId,prVdec->rOther.u4LastStartPos);
		}
        while (nBit)
        {
            if (nBit >= 24)
            {
                dwGetBitStream(ucEsId, 24);
                nBit -= 24;
            }
            else
            {
                dwGetBitStream(ucEsId, 1);
                nBit--;
            }
        }
        if (prVdec->rOther.u4DecBPCnt == 0)
        {
            ASSERT(prVdec->rOther.u4SavedInputWnd == dwGetBitStream(ucEsId, 0));
        }
#else
        if (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
        {
            LOG(7, "WMV Rp mismatch!!!!!!!!!!!! Rp=%08X(bit%d), S.IW=%08X, C.IW=%08X\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte, prVdec->rOther.u4VldPosBit, prVdec->rOther.u4SavedInputWnd, dwGetBitStream(ucEsId, 0));
            //ASSERT(0);
            fgVldInitBarrelShifter(ucEsId, (UINT32)prVdec->rDram.pu1VFifo + (prVdec->rOther.u4VldPosByte / 16 * 16));
            nBit = 0;
            while (prVdec->rOther.u4SavedInputWnd != dwGetBitStream(ucEsId, 0))
            {
                dwGetBitStream(ucEsId, 1);
                nBit++;
            }
            vCheckEmuByte(ucEsId, nBit, prVdec->rOther.u4SavedInputWnd, (UINT32)prVdec->rDram.pu1VFifo, prVdec->rOther.u4VldPosByte);
        }
#endif
    }
}

void vSaveVDec(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;
    VDEC_ES_INFO_T* prVdecEsInfo;

    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    }
    else
    {
        LOG(0, "vSaveVDec : ucEsId >= MAX_ES_NUM");
        return;
    }
	#ifdef ENABLE_MULTIMEDIA
    if (!_fgWmvRestore || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_NETFLIX || prVdecEsInfo->fgSeamlessPlay))
	#endif
    {
        return;
    }

    prVdec->rOther.u4VldPosByte = dwVldRPtr(ucEsId,&prVdec->rOther.u4VldPosBit);  // megaa 20100321
    prVdec->rOther.u4VldReg132 = ReadREG(ucEsId, RW_VLD_WMV_PARA1) & 0x7F000000;
    prVdec->rOther.u4SavedInputWnd = dwGetBitStream(ucEsId, 0);
    if (prVdec->rOther.u4SavedInputWnd == 0)
    {
        LOG(7, "IW == 0 @ %08X(bit%d)\n", (UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.u4VldPosByte, prVdec->rOther.u4VldPosBit);
    }
}
#endif  // WMV_MINST

#if 0  // debug code
void vDumpWmvRegMinst(UCHAR ucEsId)
{
    UINT32 i;

    i = 0;
    printf("VLD %03X: 0x%08X%c%c", i, ReadREG(ucEsId, VLD_REG_OFST + i), 0x0D, 0x0A);
    for (i = 0x84; i < 0x410; i+=4)
    {
        if (i == 0xa0 || i == 0xa4) continue;
        printf("VLD %03X: 0x%08X%c%c", i, ReadREG(ucEsId, VLD_REG_OFST + i), 0x0D, 0x0A);
        //pu1Buf += 21;
        //*pu4Len += 21;
    }

    for (i = 0; i <= 0x514/*0xB00*/; i+=4)
    {
        printf("MC %03X: 0x%08X%c%c", i, ReadREG(ucEsId, MC_REG_OFST + i), 0x0D, 0x0A);
        //pu1Buf += 20;
        //*pu4Len += 20;
    }

    printf("MC %03X: 0x%08X%c%c", 0x514, ReadREG(ucEsId, MC_REG_OFST + 0x514), 0x0D, 0x0A);
    printf("MC %03X: 0x%08X%c%c", 0x57C, ReadREG(ucEsId, MC_REG_OFST + 0x57C), 0x0D, 0x0A);
    printf("MC %03X: 0x%08X%c%c", 0x768, ReadREG(ucEsId, MC_REG_OFST + 0x768), 0x0D, 0x0A);
    printf("MC %03X: 0x%08X%c%c", 0x80C, ReadREG(ucEsId, MC_REG_OFST + 0x80C), 0x0D, 0x0A);

        printf("MV %03X: 0x%08X\n", 131*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 131 * 4));
        printf("MV %03X: 0x%08X\n", 134*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 134 * 4));
        printf("MV %03X: 0x%08X\n", 135*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 135 * 4));
        printf("MV %03X: 0x%08X\n", 137*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 137 * 4));
        printf("MV %03X: 0x%08X\n", 138*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 138 * 4));
        printf("MV %03X: 0x%08X\n", 139*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 139 * 4));
        printf("MV %03X: 0x%08X\n", 147*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 147 * 4));
        printf("MV %03X: 0x%08X\n", 148*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 148 * 4));
        printf("MV %03X: 0x%08X\n", 149*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 149 * 4));
        printf("MV %03X: 0x%08X\n", 150*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 150 * 4));
        printf("MV %03X: 0x%08X\n", 151*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 151 * 4));
        printf("MV %03X: 0x%08X\n", 152*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 152 * 4));
        printf("MV %03X: 0x%08X\n", 180*4, ReadREG(ucEsId,WMV_MV_REG_OFST + 180 * 4));

    printf("DCAC %03X: 0x%08X%c%c", 0x0, ReadREG(ucEsId, WMV_DCAC_REG_OFST + 0x0), 0x0D, 0x0A);
    printf("DCAC %03X: 0x%08X%c%c", 0x4, ReadREG(ucEsId, WMV_DCAC_REG_OFST + 0x4), 0x0D, 0x0A);
}


UINT32 _au4SavedWmvReg[600];
UINT32 _u4SavedWmvRegCnt = 0;

void vSaveWmvReg(UCHAR ucEsId)
{
    UINT32 i;

    _u4SavedWmvRegCnt = 0;
    _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, VLD_REG_OFST);

    for (i = 0x84; i < 0x410; i+=4)
    {
        if (i == 0xa0 || i == 0xa4) continue;
        //printf("VLD %03X: 0x%08X%c%c", i, ReadREG(ucEsId, VLD_REG_OFST + i), 0x0D, 0x0A);
        _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, VLD_REG_OFST + i);
    }

    for (i = 0; i <= 0x4b0/*0xB00*/; i+=4)
    {
        //printf("MC %03X: 0x%08X%c%c", i, ReadREG(ucEsId, MC_REG_OFST + i), 0x0D, 0x0A);
        _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, MC_REG_OFST + i);
    }

    _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, MC_REG_OFST + 0x5E4);
    _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, MC_REG_OFST + 0x658);
    _au4SavedWmvReg[_u4SavedWmvRegCnt++] = ReadREG(ucEsId, MC_REG_OFST + 0x714);
}


void vPrintSavedWmvReg(void)
{
    UINT32 i, j;

    j = i = 0;
    printf("Saved reg before decoding:\n");
    printf("VLD %03X: 0x%08X%c%c", i, _au4SavedWmvReg[j++], 0x0D, 0x0A);

    for (i = 0x84; i < 0x410; i+=4)
    {
        if (i == 0xa0 || i == 0xa4) continue;
        printf("VLD %03X: 0x%08X%c%c", i, _au4SavedWmvReg[j++], 0x0D, 0x0A);
    }

    for (i = 0; i <= 0x4b0/*0xB00*/; i+=4)
    {
        printf("MC %03X: 0x%08X%c%c", i, _au4SavedWmvReg[j++], 0x0D, 0x0A);
    }

    printf("MC %03X: 0x%08X%c%c", 0x5E4, _au4SavedWmvReg[j++], 0x0D, 0x0A);
    printf("MC %03X: 0x%08X%c%c", 0x658, _au4SavedWmvReg[j++], 0x0D, 0x0A);
    printf("MC %03X: 0x%08X%c%c", 0x714, _au4SavedWmvReg[j++], 0x0D, 0x0A);
}


void vPrintWmvPair(BOOL fgBP)
{
    if (fgBP)
    {
        printf("#%04d(BP), RP = 0x%08X, WIN = 0x%08X\n",
            rWMVDecInfo[0].rOther.u4CurrPicNo,
            ReadREG(0, RW_VLD_RPTR),
            ReadREG(0, RO_VLD_BARL)
        );
    }
    else
    {
        printf("#%04d      RP = 0x%08X, WIN = 0x%08X\n",
            rWMVDecInfo[0].rOther.u4CurrPicNo,
            ReadREG(0, RW_VLD_RPTR),
            ReadREG(0, RO_VLD_BARL)
        );
    }
}
#endif  // 0, debug code


#if defined(CC_WMV_EMULATION) || defined(VDEC_DEC_REG_QUERY)
void vDumpWmvMcReg(UCHAR ucEsId)
{
#if 0
    char ucPicType = ' ';

    switch (prVdec->rPicLayer.u1PicType)
    {
    case IVOP:
        ucPicType = 'I';
        break;
    case PVOP:
        ucPicType = 'P';
        break;
    case BVOP:
        ucPicType = 'B';
        break;
    case BIVOP:
        ucPicType = 'A';
        break;
    case SKIPFRAME:
        ucPicType = 'S';
        break;
    }
    printf("Frame #%d: %c\n", prVdec->rOther.u4CurrPicNo - 1, ucPicType);
#else
    printf("MC 401: 0x%08X\n", ReadREG(ucEsId,MC_REG_OFST + 401 * 4));
    printf("MC 476: 0x%08X\n", ReadREG(ucEsId,MC_REG_OFST + 476 * 4));
    printf("MC 477: 0x%08X\n", ReadREG(ucEsId,MC_REG_OFST + 477 * 4));
    printf("MC 478: 0x%08X\n", ReadREG(ucEsId,MC_REG_OFST + 478 * 4));
    printf("MC 522: 0x%08X\n", ReadREG(ucEsId,MC_REG_OFST + 522 * 4));
#endif
}

UINT32 u4DumpWmvReg(UINT8 *pu1Buf)
{
    UINT32 i;
    UCHAR ucEsId = 0;
    UINT8 *pu1Buf0 = pu1Buf;
	VDEC_WMV_INFO_T* prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);

    for (i = 33; i <= 259; i++)
    {
        if (i == 40 || i == 41)
            continue;
        printf("VLD %3d: 0x%08X\n", i, ReadREG(ucEsId,VLD_REG_OFST + i * 4));
        if (pu1Buf)
        {
            sprintf((void *)pu1Buf, "VLD %3d: 0x%08X%c%c", i, ReadREG(ucEsId,VLD_REG_OFST + i * 4), 0x0D, 0x0A);
            pu1Buf += 21;
        }
    }

    for (i = 0; i <= 704; i++)
    {
        printf("MC %3d: 0x%08X\n", i, ReadREG(ucEsId,MC_REG_OFST + i * 4));
        if (pu1Buf)
        {
            sprintf((void *)pu1Buf, "MC %3d: 0x%08X%c%c", i, ReadREG(ucEsId,MC_REG_OFST + i * 4), 0x0D, 0x0A);
            pu1Buf += 20;
        }
    }

#if defined(IS_WMV_POST_MT5368)
    for (i = 0; i <= 99; i++)
    {
        printf("VLDTOP %3d: 0x%08X\n", i, ReadREG(ucEsId, VLD_TOP_OFST + i * 4));
        if (pu1Buf)
        {
            sprintf((void *)pu1Buf, "VLDTOP %3d: 0x%08X%c%c", i, ReadREG(ucEsId, VLD_TOP_OFST + i * 4), 0x0D, 0x0A);
            pu1Buf += 24;
        }
    }
#endif

    for (i = 1; i <= 9; i++)
    {
        printf("CRC %3d: 0x%08X\n", i, ReadREG(ucEsId, CRC_REG_OFST + i * 4));
        if (pu1Buf)
        {
            sprintf((void *)pu1Buf, "CRC %3d: 0x%08X%c%c", i, ReadREG(ucEsId, CRC_REG_OFST + i * 4), 0x0D, 0x0A);
            pu1Buf += 21;
        }
    }

    if (prVdec->rOther.fgNewMode)
    {
        printf("MV %3d: 0x%08X\n", 131, ReadREG(ucEsId,WMV_MV_REG_OFST + 131 * 4));
        printf("MV %3d: 0x%08X\n", 134, ReadREG(ucEsId,WMV_MV_REG_OFST + 134 * 4));
        printf("MV %3d: 0x%08X\n", 135, ReadREG(ucEsId,WMV_MV_REG_OFST + 135 * 4));
        printf("MV %3d: 0x%08X\n", 137, ReadREG(ucEsId,WMV_MV_REG_OFST + 137 * 4));
        printf("MV %3d: 0x%08X\n", 138, ReadREG(ucEsId,WMV_MV_REG_OFST + 138 * 4));
        printf("MV %3d: 0x%08X\n", 139, ReadREG(ucEsId,WMV_MV_REG_OFST + 139 * 4));
        for (i = 140; i <= 181; i++)
        {
            printf("MV %3d: 0x%08X\n", i, ReadREG(ucEsId,WMV_MV_REG_OFST + i * 4));
        }

        if (pu1Buf)
        {
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 131, ReadREG(ucEsId,WMV_MV_REG_OFST + 131 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 134, ReadREG(ucEsId,WMV_MV_REG_OFST + 134 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 135, ReadREG(ucEsId,WMV_MV_REG_OFST + 135 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 137, ReadREG(ucEsId,WMV_MV_REG_OFST + 137 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 138, ReadREG(ucEsId,WMV_MV_REG_OFST + 138 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", 139, ReadREG(ucEsId,WMV_MV_REG_OFST + 139 * 4), 0x0D, 0x0A);  pu1Buf += 20;
            for (i = 140; i <= 181; i++)
            {
                sprintf((void *)pu1Buf, "MV %3d: 0x%08X%c%c", i, ReadREG(ucEsId,WMV_MV_REG_OFST + i * 4), 0x0D, 0x0A);  pu1Buf += 20;
            }
        }

#if defined(IS_WMV_POST_MT5368)
        // PP registers, 3 sections (including WMV_DCAC)
        for (i = 128; i <= 191; i++)
        {
            printf("PP %3d: 0x%08X\n", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4));
            if (pu1Buf)
            {
                sprintf((void *)pu1Buf, "PP %3d: 0x%08X%c%c", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4), 0x0D, 0x0A);
                pu1Buf += 20;
            }
        }
        for (i = 320; i <= 383; i++)
        {
            printf("PP %3d: 0x%08X\n", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4));
            if (pu1Buf)
            {
                sprintf((void *)pu1Buf, "PP %3d: 0x%08X%c%c", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4), 0x0D, 0x0A);
                pu1Buf += 20;
            }
        }
        for (i = 448; i <= 639; i++)
        {
            printf("PP %3d: 0x%08X\n", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4));
            if (pu1Buf)
            {
                sprintf((void *)pu1Buf, "PP %3d: 0x%08X%c%c", i, ReadREG(ucEsId, WMV_PP_REG_OFST + i * 4), 0x0D, 0x0A);
                pu1Buf += 20;
            }
        }
#else
        for (i = 0; i <= 255; i++)
        {
            printf("DCAC %3d: 0x%08X\n", i, ReadREG(ucEsId,WMV_DCAC_REG_OFST + i * 4));
            if (pu1Buf)
            {
                sprintf((void *)pu1Buf, "DCAC %3d: 0x%08X%c%c", i, ReadREG(ucEsId,WMV_DCAC_REG_OFST + i * 4), 0x0D, 0x0A);
                pu1Buf += 22;
            }
        }
#endif
    }

    if (pu1Buf)
        return (UINT32)pu1Buf - (UINT32)pu1Buf0;
    else
        return 0;
}

void vDumpWmvInputWnd(UCHAR ucEsId)
{
    //vld0 access time out after power setting,so access vld60 instead
    printf("Input Window: 0x%X\n", ReadREG(ucEsId, RO_VLD_INPUTWND));
    return ;
}
#endif  // CC_WMV_EMULATION


void vSetNewMode(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec = NULL;
    if(ucEsId<MAX_ES_NUM)
    {
        prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
    }
    else
    {
        LOG(0, "vSetNewMode : ucEsId >= MAX_ES_NUM");
        return;
    }
    if (prVdec->rOther.fgNewMode/* && prVdec->rOther.i4CodecVersion != WMV3*/)
    {
        UINT32 a163, a164, a170, a171;
        INT32 i4OwBp;

        // VLD part
        // 163
        a163 = (WMV_NEW_FLG_EN | WMV_PRED_MOD_EN | WMV_PRED_RD_SYNC_DISABLE
                | WMV_DRAM_BURST_MODE_EN       // megaa 20100407: set this to enhance DRAM performance
                /*| WMV_BP_USE_PRED_RD_EN*/);  // megaa 20100407: set this only when BP to DRAM
        WriteREG(RW_VLD_WMV_NEW_CTRL, a163);

        // 164
        a164 = PHYSICAL((UINT32)prVdec->rDram.pu1Dcac);  // ???
#if defined(IS_WMV_POST_MT5368)
        WriteREG(RW_VLD_TOP_PRED_ADDR, a164);
#else
#if 1  // megaa 20100407: BP to SRAM or DRAM switch
        if (cbWMVFWBP)
            a164 |= WMV_BP_TO_DRAM;
        else
            a164 |= WMV_BP_TO_SRAM;
#else
        a164 |= WMV_BP_TO_DRAM;
#endif
        WriteREG(RW_VLD_WMV_DCAC_NEW_ADDR, a164);
#endif

        // 167, 168, 169
        WriteREG(RW_VLD_WMV_BP0_NEW_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_1));  // ???
        WriteREG(RW_VLD_WMV_BP1_NEW_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_2));  // ???
        WriteREG(RW_VLD_WMV_BP2_NEW_ADDR, PHYSICAL((UINT32)prVdec->rDram.pu1Bp_3));  // ???

        // 170
        a170 = (prVdec->rSeqHdrR.u4PicWidthDec / 16)  * 7  * (16/16);
        WriteREG(RW_VLD_WMV_OW_PRED, a170);

        // 171
        i4OwBp = (prVdec->rSeqHdrR.u4PicHeightDec / 16)  * (16/16);
        a171 = ( (i4OwBp << WMV_NUM_OW_BP_0) | (i4OwBp << WMV_NUM_OW_BP_1) | (i4OwBp << WMV_NUM_OW_BP_2));
        WriteREG(RW_VLD_WMV_OW_BP, a171);
    }
}

#if defined(IS_WMV_POST_MT5368)
#ifdef CC_WMV_EMULATION
void _VDEC_WMVSetDramBusy(void)
{
    IO_WRITE32((IO_VIRT + 0x07200),  0x18, IO_READ32((IO_VIRT + 0x07200), 0x18) | (1 << 25) | (1U <<31));

    IO_WRITE32((IO_VIRT + 0x07200),  0x10, 0x0f6cb2f0); // 這個是讓 DRAM 模擬 Busy 的 Buffer 小心不要蓋到您有使用的區域
    IO_WRITE32((IO_VIRT + 0x07200),  0x14, 0x10000);    // 上述 buffer 的 size

    IO_WRITE32((IO_VIRT + 0x7100), 0x4, IO_READ32((IO_VIRT + 0x7100), 0x4) & 0xFFFFFFF);

    IO_WRITE32((IO_VIRT + 0x7100), 0x14, 0xFFFFFF11);
}
#endif
#endif

