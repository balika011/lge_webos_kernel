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
 * $RCSfile: vdec_vp9_api.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp9_8api.c
 *  This header file declares exported APIs of vp9.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_vp9_api.h"
#include "x_chip_id.h"
#include <linux/vmalloc.h>

static VP9_COMMON_T *_rVp9Common = 0;
static HANDLE_T _hDecodeFinSema[VDEC_MAX_VLD] = {NULL_HANDLE, NULL_HANDLE, NULL_HANDLE, NULL_HANDLE};
//VP9_LAE_INFO_T _rLaeInfo[MAX_LAE_BUF_NS_PER_GROUP];
static VP9_LAE_INFO_T *_pLaeInfo = 0;

VP9_LAE_INFO_T *GetLaeInfo(void)
{
    if (_pLaeInfo == 0)
    {
        _pLaeInfo = (VP9_LAE_INFO_T*)vmalloc(sizeof(VP9_LAE_INFO_T)*MAX_LAE_BUF_NS_PER_GROUP);
        x_memset(_pLaeInfo, 0, sizeof(VP9_LAE_INFO_T)*MAX_LAE_BUF_NS_PER_GROUP);
    }
    return _pLaeInfo;
}
VP9_COMMON_T* prVP9GetCommon(UINT32 u4InstID)
{
    if(u4InstID >= VDEC_MAX_ES)
    {
        VP9_ERROR("prVP9GetCommon u4InstID(%d) > VDEC_INST_MAX, oops...\n", u4InstID);
        return NULL;
    }
    if (_rVp9Common==0)
    {
        _rVp9Common = (VP9_COMMON_T *)vmalloc(sizeof(VP9_COMMON_T )*VDEC_MAX_ES);
        x_memset(_rVp9Common, 0, sizeof(VP9_COMMON_T )*VDEC_MAX_ES);
    }
    return &(_rVp9Common[u4InstID]);
}

VOID _VDEC_Vp9ChkVldStatus(UCHAR ucEsId, UINT32 u4CoreId)
{
    UINT32 u4ErrValue = u4VDEC_HAL_VP9_Get_VLD_Err(u4CoreId);
    switch (u4ErrValue)
    {
        case 0x0:
            //no error
            break;
        default:            
            VP9_PRINT_ERROR(ucEsId, "unknown hw error: 0x%x\n", u4ErrValue);
            VP9_PRINT_ERROR(ucEsId, "unknown hw error: 0x%x\n", u4ErrValue);
            VP9_PRINT_ERROR(ucEsId, "unknown hw error: 0x%x\n", u4ErrValue);
            _VDEC_Vp9ChkStatus();
            //vVDEC_HAL_VP9_VDec_DumpReg(0, 0, TRUE);
            //x_thread_delay(60000);
            break;
    }
}


INT32 _VDEC_Vp9Init(UCHAR ucEsId)
{
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);    
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    SIZE_T zMsgSize = sizeof(VDEC_MSG_INTO_T);
    UINT16 u2MsgQIdx;
    INT32 i4Ret=0;
    UINT32 u4Index;
    VP9_LAE_INFO_T *_rLaeInfo;
    _rLaeInfo = GetLaeInfo();
    x_memset(prCommon, 0, sizeof(VP9_COMMON_T));
    prCommon->ucPairedEsId = VDEC_MAX_ES;
    if(prVdecEsInfo->ucVDecStatus == LAE_DECODE)
    {
        VP9_COMMON_T* prMainCommon;
        prCommon->fgLAEThread = TRUE;
        prCommon->ucPairedEsId = ucVDecGetMcoreDecInst(ucEsId);
        prMainCommon = prVP9GetCommon(prCommon->ucPairedEsId);
        prMainCommon->ucPairedEsId = ucEsId;
        prCommon->fgDualCoreFrame = TRUE;        
        prCommon->fgUFOModeEnable = prMainCommon->fgUFOModeEnable;
        VP9_PRINT_INFO(ucEsId, "lae play from main[%d], ufo[%d]\n", prCommon->ucPairedEsId, prCommon->fgUFOModeEnable);

		
        while (x_msg_q_receive(&u2MsgQIdx, (void*)(_rLaeInfo), &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            VP9_PRINT_DEBUG(ucEsId, "clear lae msgQ before play");
        }
        
        for (u4Index = 0; u4Index< MAX_LAE_BUF_NS_PER_GROUP; u4Index++)
        {
            VDEC_LAE_DecPrm rLaeIn;
            x_memset(&_rLaeInfo[u4Index], 0, sizeof(VP9_LAE_INFO_T));
            rLaeIn.pvExtraData = (void *)(&_rLaeInfo[u4Index]);
            vPutBufferToLae(ucEsId, &rLaeIn);
        }
    }
    else
    {
        if(prVdecEsInfoKeep->fgPatternMode)//wait es send done then start to decode.
            x_thread_delay(1000);
        
        prVdecEsInfoKeep->fgLowLatency = TRUE;
        prVdecEsInfo->fgKeepPes = TRUE;
        VP9_PRINT_INFO(ucEsId, "main play\n");
        while (x_msg_q_receive(&u2MsgQIdx, (void*)(_rLaeInfo), &zMsgSize, &(prVdecEsInfoKeep->hMsgQLaeOuput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
            VP9_PRINT_DEBUG(ucEsId, "clear lae msgQ before play");
        }
        _VDEC_Vp9InitIsr(ucEsId);
    }

    for(u4Index = 0; u4Index < REFS_PER_FRAME; u4Index++)
    {
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4X_scale_fp = REF_INVALID_SCALE;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4Y_scale_fp = REF_INVALID_SCALE;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4X_step_q4 = REF_INVALID_STEP;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4Y_step_q4 = REF_INVALID_STEP;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.u4Ref_Scaling_EN = 0;
    }

    for(u4Index = 0; u4Index < REF_FRAMES; u4Index ++)
    {
        prCommon->REF_FRAME_MAP[u4Index] = -1;
    }
    prCommon->COMP_FIXED_REF = INTRA_FRAME;
    prCommon->ucEsId = ucEsId;
    prCommon->ucFbgId = FBM_FBG_ID_UNKNOWN;
    prCommon->u4NewFbIdx = -1;
    prCommon->fgOccupied = TRUE;
#ifdef LINUX_TURNKEY_SOLUTION
    prCommon->u4JifMark = jiffies;
    prCommon->u4LstOutJif = jiffies;
#endif
    vVP9_Init_Dequantizer(prCommon);
    return (INT32)i4Ret;
}

VOID _VDEC_Vp9InitIsr(UCHAR uEsId)
{
    UINT32 i;
    for(i = 0; i < VDEC_MAX_VLD; i ++)
    {
        if(NULL_HANDLE == _hDecodeFinSema[i])
        {
            VERIFY(x_sema_create(&(_hDecodeFinSema[i]), X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        }
    }
}

VOID _VDEC_Vp9SetIsr(UCHAR ucEsId, UCHAR ucVldId)
{
    UINT16 u2Vector;
    UNUSED(ucEsId);
    if(ucVldId == VDEC_VLD0)
    {
        u2Vector = VDEC_VLD0_VECTOR;
    }
#ifdef CC_DUAL_CORE_DEC
    else if(ucVldId == VDEC_VLD1)
    {
        u2Vector = VDEC_VLD1_VECTOR;
    }
    else if(ucVldId == VDEC_LAE0)
    {
        u2Vector = VDEC_LAE0_VECTOR;
    }
    else if(ucVldId == VDEC_LAE1) //LAE1 isr Clear
    {
        u2Vector = VDEC_LAE1_VECTOR;
    }
    else
    {
        ASSERT(0);
    }
#endif
    _VDEC_RegIsr(u2Vector, _VDEC_Vp9Isr);
}

VOID _VDEC_Vp9Isr(UINT16 u4Vector)
{
    if (u4Vector == VDEC_VLD0_VECTOR)
    {
        BIM_ClearIrq(VDEC_VLD0_VECTOR);
        VERIFY(x_sema_unlock(_hDecodeFinSema[VDEC_VLD0]) == OSR_OK);
    }
    else if (u4Vector == VDEC_VLD1_VECTOR)
    {
        BIM_ClearIrq(VDEC_VLD1_VECTOR);
        VERIFY(x_sema_unlock(_hDecodeFinSema[VDEC_VLD1]) == OSR_OK);
    }
    else if (u4Vector == VDEC_LAE0_VECTOR) //LAE0 isr Clear
    {
        BIM_ClearIrq(VDEC_LAE0_VECTOR);
        VERIFY(x_sema_unlock(_hDecodeFinSema[VDEC_LAE0]) == OSR_OK);
    }
    else if(u4Vector == VDEC_LAE1_VECTOR) //LAE1 isr Clear
    {
        BIM_ClearIrq(VDEC_LAE1_VECTOR);
        VERIFY(x_sema_unlock(_hDecodeFinSema[VDEC_LAE1]) == OSR_OK);
    }
    else
    {
        ASSERT(0);
    }
}

INT32 _VDEC_Vp9HdrPrs(UCHAR ucVdecId, UCHAR ucEsId)
{
    UINT32 u4UnCompressedRet = 0;
    UINT32 i;
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_PES_INFO_T* pPesInfo = &prVdecEsInfoKeep->rPesInfo;
    UINT32 u4CoreId = (prVdecEsInfo->u4VldId == VDEC_LAE0) ? VP9_LAE_ID : prVdecEsInfo->u4VldId;
    UINT32 u4CoreId2 = prVdecEsInfo->u4Vld2Id;    
    VP9_UNCOMPRESSED_HEADER_T* prUnCompressed = &prCommon->rUnCompressedHeader;
#ifdef LINUX_TURNKEY_SOLUTION
    prCommon->u4JifMark = jiffies;
#endif
    if(!prVdecEsInfo->ucDualCoreMode)
    {
        if(pPesInfo->fgEos)
            VP9_PRINT_IO(ucEsId, "eos frame!!");
        VP9_PRINT_IO(ucEsId, "in %c(0x%x--0x%x) size:0x%x, ts:0x%x", ((pPesInfo->ucPicType == 1) ? 'I' : ' '), pPesInfo->u4VldReadPtr, pPesInfo->u4VldReadPtrEnd, (pPesInfo->u4VldReadPtrEnd-pPesInfo->u4VldReadPtr),pPesInfo->u4PTS);
    }
    
    if(!prCommon->fgReDecode && (prVdecEsInfo->ucDualCoreMode || prCommon->fgLAEThread))
    {
        VDEC_LAE_DecPrm rLaeInfo;
        if(prCommon->fgLAEThread)
        {
            ucGetBufferForLaeDec(ucEsId, &(rLaeInfo));
            prCommon->prVp9LaeInput = (VP9_LAE_INFO_T *)rLaeInfo.pvExtraData;
            x_memcpy(&prCommon->rLAEBuffer, &prCommon->prVp9LaeInput->rLAEBuffer, sizeof(VP9_DRAM_BUF_T));
            x_memcpy(&prCommon->rErrorBuffer, &prCommon->prVp9LaeInput->rErrorBuffer, sizeof(VP9_DRAM_BUF_T));
        }
        else
        {
            VP9_REF_CNT_BUF_T FRAME_BUFS[FRAME_BUFFERS];
            UINT32 u4FreeFbId = FBM_FB_ID_UNKNOWN;
            VP9_REF_CNT_BUF_T* pNewFB;
            ucGetBufferForMcoreDec(ucEsId,  &(rLaeInfo));
            prCommon->prVp9LaeInput = (VP9_LAE_INFO_T *)rLaeInfo.pvExtraData;
            x_memcpy(FRAME_BUFS, prCommon->FRAME_BUFS, sizeof(VP9_REF_CNT_BUF_T)*FRAME_BUFFERS);
            x_memcpy(prCommon, prCommon->prVp9LaeInput, sizeof(VP9_LAE_INFO_T));

            for (i = 0; i < FRAME_BUFFERS; ++i)
            {
                prCommon->FRAME_BUFS[i].rBuf.u4FbgId = prCommon->ucFbgId;
                prCommon->FRAME_BUFS[i].rBuf.u4FbId = FRAME_BUFS[i].rBuf.u4FbId;
                prCommon->FRAME_BUFS[i].rBuf.rBufY.u4BufPAddr = FRAME_BUFS[i].rBuf.rBufY.u4BufPAddr;
                prCommon->FRAME_BUFS[i].rBuf.rBufC.u4BufPAddr = FRAME_BUFS[i].rBuf.rBufC.u4BufPAddr;
                prCommon->FRAME_BUFS[i].rBuf.rUFO_LEN_Y.u4BufPAddr = FRAME_BUFS[i].rBuf.rUFO_LEN_Y.u4BufPAddr;
                prCommon->FRAME_BUFS[i].rBuf.rUFO_LEN_C.u4BufPAddr = FRAME_BUFS[i].rBuf.rUFO_LEN_C.u4BufPAddr;
            }
            
            if(prUnCompressed->u4ShowExisting)
            {
                VDEC_LAE_DecPrm rLaeIn;
                VP9_LAE_INFO_T* prVp9Lae = prCommon->prVp9LaeInput;
                rLaeIn.pvExtraData = (void*)(prVp9Lae);
                vPutBufferToLae(prCommon->ucPairedEsId, &rLaeIn);
                return E_VDEC_OK;
            }
            
            do
            {
                u4FreeFbId = FBM_GetEmptyFrameBuffer(prCommon->ucFbgId, 500);
                if(FBM_FB_ID_UNKNOWN == u4FreeFbId)
                {
                    VP9_PRINT_INFO(ucEsId, "get empty frame fail(maybe an error if not paused)  .....");
                    //FBM_QueryStatus(prCommon->ucFbgId);
                }

            }while(FBM_FB_ID_UNKNOWN == u4FreeFbId);

            VP9_PRINT_VERBOSE(prCommon->ucEsId, "out (%d, %d)->decode", prCommon->ucFbgId, u4FreeFbId);
            FBM_UpdateReferenceList(prCommon->ucFbgId, u4FreeFbId, TRUE);
            pNewFB = prCommon->pCurrentFB = &(prCommon->FRAME_BUFS[prCommon->u4NewFbIdx]);
            pNewFB->rBuf.u4FbId = u4FreeFbId;
            prCommon->prFrameToShow = &pNewFB->rBuf;
            FBM_GetFrameBufferAddr(prCommon->ucFbgId, pNewFB->rBuf.u4FbId, &(pNewFB->rBuf.rBufY.u4BufPAddr), &(pNewFB->rBuf.rBufC.u4BufPAddr));
            if(prCommon->fgUFOModeEnable)
            {
                FBM_GetFrameBufferAddrExt(prCommon->ucFbgId, pNewFB->rBuf.u4FbId, &(pNewFB->rBuf.rUFO_LEN_Y.u4BufPAddr), &(pNewFB->rBuf.rUFO_LEN_C.u4BufPAddr));
            }
            for (i = 0; i < REFS_PER_FRAME; ++i)
            {
                if(prCommon->FRAME_REFS[i].u4Idx != INT_MAX)
                {
                    prCommon->FRAME_REFS[i].prBuf = &prCommon->FRAME_BUFS[prCommon->FRAME_REFS[i].u4Idx].rBuf;
                }
            }
            VP9_PRINT_VERBOSE(prCommon->ucEsId, "ref list -> (%d,%d) (%d,%d) (%d,%d)", prCommon->FRAME_REFS[0].prBuf->u4FbgId, prCommon->FRAME_REFS[0].prBuf->u4FbId,
                    prCommon->FRAME_REFS[1].prBuf->u4FbgId, prCommon->FRAME_REFS[1].prBuf->u4FbId, prCommon->FRAME_REFS[2].prBuf->u4FbgId, prCommon->FRAME_REFS[2].prBuf->u4FbId);
        }
    }
    
    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
#ifdef LINUX_TURNKEY_SOLUTION
    prCommon->u4JifMark = jiffies;
#endif

    if(prVdecEsInfo->ucDualCoreMode)
    {
        //decode after lae        
        UINT32 u4CompressEnd = PHYSICAL(pPesInfo->u4VldReadPtr) + prUnCompressed->u4UnCompressSize + prUnCompressed->u4FirstPartitionSize;
        u4CompressEnd = ((u4CompressEnd - 1) >= (PHYSICAL(pPesInfo->u4FifoEnd))) ? ((PHYSICAL(pPesInfo->u4FifoStart)) + u4CompressEnd - 1 - (PHYSICAL(pPesInfo->u4FifoEnd))) : (u4CompressEnd - 1);
        vVDEC_HAL_VP9_Mcore_Enable(prCommon->fgDualCoreFrame);
        vVDEC_HAL_VP9_SW_Reset(u4CoreId, prCommon->fgUFOModeEnable);
        i4VDEC_HAL_VP9_InitBarrelShifter(u4CoreId, u4CompressEnd, (PHYSICAL(pPesInfo->u4VldReadPtrEnd)), (PHYSICAL(pPesInfo->u4FifoStart)), (PHYSICAL(pPesInfo->u4FifoEnd)));
        vVDEC_HAL_VP9_Set_TxMode(u4CoreId, prCommon->eTxMode);
        u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, prCommon->ucBitsOverAlign);
        if(prCommon->fgDualCoreFrame)
        {
            vVDEC_HAL_VP9_SW_Reset(u4CoreId2, prCommon->fgUFOModeEnable);
            i4VDEC_HAL_VP9_InitBarrelShifter(u4CoreId2, u4CompressEnd, (PHYSICAL(pPesInfo->u4VldReadPtrEnd)), (PHYSICAL(pPesInfo->u4FifoStart)), (PHYSICAL(pPesInfo->u4FifoEnd)));
            vVDEC_HAL_VP9_Set_TxMode(u4CoreId2, prCommon->eTxMode);
            u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId2, prCommon->ucBitsOverAlign);
        }
    }
    else
    {
        if(!prCommon->fgLAEThread)
        {
            vVDEC_HAL_VP9_Mcore_Enable(FALSE);
        }
        //single/lae core flow
        vVDEC_HAL_VP9_SW_Reset(u4CoreId, prCommon->fgUFOModeEnable);
        i4VDEC_HAL_VP9_InitBarrelShifter(u4CoreId, (PHYSICAL(pPesInfo->u4VldReadPtr)), (PHYSICAL(pPesInfo->u4VldReadPtrEnd)), (PHYSICAL(pPesInfo->u4FifoStart)), (PHYSICAL(pPesInfo->u4FifoEnd)));
        u4UnCompressedRet = i4VP9UnCompressHdrPrs(prCommon, prUnCompressed, u4CoreId);
        if(u4UnCompressedRet == VP9_SKIP_FRAME)
        {
            VP9_PRINT_INFO(ucEsId, "skip decode, show existing frame");
            return E_VDEC_OK;
        }
        else if(u4UnCompressedRet == VP9_FAIL)
        {
            // TODO: prepare error lae info
            _VDEC_UnlockVld(ucEsId, u4CoreId);
            if(prCommon->u4NewFbIdx >= 0)
            {
                if(!prCommon->fgLAEThread)
                {
                    FBM_SetFrameBufferStatus(prCommon->pCurrentFB->rBuf.u4FbgId, prCommon->pCurrentFB->rBuf.u4FbId, FBM_FB_STATUS_EMPTY);
                }
                vVP9RefCntFb(prCommon, &prCommon->u4NewFbIdx, -1, !prCommon->fgLAEThread);
            }

            return E_VDEC_FAIL;
        }
        
        if (!prCommon->u4KeyFrameDecoded && (prUnCompressed->u4FrameType!= KEY_FRAME))
        {
            if(!(prUnCompressed->u4IntraOnly))
            {
                VP9_PRINT_ERROR(ucEsId, "first frame should be key frame or intra frame");
                _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
                return E_VDEC_FAIL;
            }
            else
            {
                for (i = 0; i < REFS_PER_FRAME; ++i)
                {
                    prCommon->FRAME_REFS[i].u4Idx = prCommon->u4NewFbIdx;
                    prCommon->FRAME_REFS[i].prBuf = &prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf;
                }
            } 
        }
        
        if(prCommon->fgLAEThread)
        {
            if(prCommon->rUnCompressedHeader.u4Width <= 384 || prCommon->rUnCompressedHeader.u4Height <= 128)
            {
                prCommon->fgDualCoreFrame = FALSE;
            }
            else
            {
                prCommon->fgDualCoreFrame = TRUE;
            }
        }
        
        if(prCommon->u4FrameNum == 0)
        {
            VP9_PRINT_INFO(ucEsId, "1st frm info -> profile:%d, type:%d, size:%dx%d, bit depth:%d\n", prUnCompressed->u4Profile, prUnCompressed->u4FrameType, prUnCompressed->u4Width, prUnCompressed->u4Height,
                prUnCompressed->u4BitDepth);
        }
        prCommon->fc = prCommon->frame_contexts[prUnCompressed->u4FrameContextIdx];
        x_memset(&prCommon->counts, 0, sizeof(FRAME_COUNTS));
    }

    if(prCommon->fgLAEThread || prCommon->fgDualCoreFrame)
    {
        vVDEC_HAL_VP9_Set_Lae_Buf(prCommon->rLAEBuffer.u4BufPAddr, prCommon->rErrorBuffer.u4BufPAddr, prCommon->fgLAEThread);
    }
    
    vVDEC_HAL_VP9_Set_UnCompressed(u4CoreId, prUnCompressed);
    vVDEC_HAL_VP9_Set_Segmentation(u4CoreId, &prUnCompressed->seg, prCommon->rSegIdBuffer.u4BufVAddr, prCommon->rSegIdBuffer.u4BufSize, prUnCompressed->fgResetSeg);
    vVDEC_HAL_VP9_Set_Probs_Table(u4CoreId, &prCommon->fc);

    if(!prVdecEsInfo->ucDualCoreMode)
    {
        i4VP9CompressedHdrPrs(prCommon, u4CoreId);
        vVP9TileInfoParse(prCommon, pPesInfo->u4VldReadPtr, pPesInfo->u4VldReadPtrEnd, pPesInfo->u4FifoStart, pPesInfo->u4FifoEnd);
    }
    
    vVP9SetPicInfo(prCommon, u4CoreId);//this must be called after vVP9SetupCompoundRefMode
    vVDEC_HAL_VP9_Set_Tile(u4CoreId, prCommon);
    vVDEC_HAL_VP9_Clear_Counts_Table(u4CoreId);
    if((prCommon->fgDualCoreFrame) && !prCommon->fgLAEThread)
    {
        vVDEC_HAL_VP9_Set_UnCompressed(u4CoreId2, prUnCompressed);    
        vVDEC_HAL_VP9_Set_Segmentation(u4CoreId2, &prUnCompressed->seg, prCommon->rSegIdBuffer.u4BufVAddr, prCommon->rSegIdBuffer.u4BufSize, prUnCompressed->fgResetSeg);
        vVDEC_HAL_VP9_Set_Probs_Table(u4CoreId2, &prCommon->fc);
        vVP9SetPicInfo(prCommon, u4CoreId2);
        vVDEC_HAL_VP9_Set_Tile(u4CoreId2, prCommon);
        vVDEC_HAL_VP9_Clear_Counts_Table(u4CoreId2);
    }
    
    return E_VDEC_OK;
}


// **************************************************************************
// Function : INT32 _VDEC_H264DecStart(UCHAR ucVDecID, UCHAR ucEsId);
// Description : Set register & trigger HW decode
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : Trigger ok
// **************************************************************************
INT32 _VDEC_Vp9DecStart(UCHAR ucVdecId, UCHAR ucEsId)
{
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    UINT32 u4CoreId = (prVdecEsInfo->u4VldId == VDEC_LAE0) ? VP9_LAE_ID : prVdecEsInfo->u4VldId;
    //UINT32 u4CoreId2 = prVdecEsInfo->u4Vld2Id;

    if(prCommon->rUnCompressedHeader.u4ShowExisting)
    {
        return E_VDEC_OK;
    }
    
    if(prCommon->rUnCompressedHeader.u4FrameType == KEY_FRAME && !prCommon->u4KeyFrameDecoded)
    {
        prCommon->u4KeyFrameDecoded = 1;
    }
    
    if(prVdecEsInfoKeep->fgPatternMode)
        vVDEC_HAL_VP9_Crc_Enable(TRUE);

    x_sema_lock(_hDecodeFinSema[prVdecEsInfo->u4VldId], X_SEMA_OPTION_NOWAIT);//make sure it's locked before trigger decode
    // for MCore need refine
    if(prVdecEsInfo->ucDualCoreMode)
    {        
        vVDEC_HAL_VP9_Trigger_Decode(u4CoreId, VDEC_MAX_VLD, FALSE, prCommon->fgDualCoreFrame);
    }
    else
    {
        vVDEC_HAL_VP9_Trigger_Decode(u4CoreId, VDEC_MAX_VLD, prCommon->fgLAEThread, FALSE);
    }
    
    return E_VDEC_OK;
}

INT32 _VDEC_Vp9DecFin(UCHAR ucVdecId, UCHAR ucEsId)
{
    INT32 i4Ret = OSR_OK;
    UINT32 i;
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VP9_UNCOMPRESSED_HEADER_T* prUnCompress = &prCommon->rUnCompressedHeader;
    UINT32 u4CoreId = (prVdecEsInfo->u4VldId == VDEC_LAE0) ? VP9_LAE_ID : prVdecEsInfo->u4VldId;
//	    UINT32 u4CoreId2 = prVdecEsInfo->u4Vld2Id;
    
    if(prUnCompress->u4ShowExisting)
    {
        if(prCommon->fgLAEThread)
        {
            VDEC_LAE_DecPrm rLaeIn;
            VP9_LAE_INFO_T* prVp9LaeOut = prCommon->prVp9LaeInput;
            x_memcpy(&prVp9LaeOut->rUnCompressedHeader, &prCommon->rUnCompressedHeader, sizeof(VP9_UNCOMPRESSED_HEADER_T));
            rLaeIn.pvExtraData = (void*)(prVp9LaeOut);
            vPutBufferToMcore(prCommon->ucPairedEsId, &rLaeIn);
        }
    }
    else
    {
        UCHAR ucTryCount = 50;
        do
        {
            i4Ret = x_sema_lock_timeout(_hDecodeFinSema[prVdecEsInfo->u4VldId], 50);
            ucTryCount--;
            // special case sw workaround, comment by HsiuYi
            if(!prCommon->fgReDecode && !prCommon->fgLAEThread)
            {                  
                if(fgVDEC_HAL_VP9_VDec_ReadSpecialCase(prVdecEsInfo->ucDualCoreMode))
                {                    
                    if(prCommon->fgDualCoreFrame)
                    {
                        vVDEC_HAL_VP9_Mcore_Reset();
                    }
                    VP9_PRINT_ERROR(ucEsId, "Retrigger Decode....");
                    _VDEC_Vp9ChkStatus();
                    prCommon->fgReDecode = TRUE;
                    if(!prVdecEsInfo->ucDualCoreMode && prCommon->u4NewFbIdx >= 0)
                    {
                        FBM_SetFrameBufferStatus(prCommon->pCurrentFB->rBuf.u4FbgId, prCommon->pCurrentFB->rBuf.u4FbId, FBM_FB_STATUS_EMPTY);
                        vVP9RefCntFb(prCommon, &prCommon->u4NewFbIdx, -1, TRUE);
                    }
                    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
                    _VDEC_Vp9HdrPrs(ucVdecId, ucEsId);
                    _VDEC_Vp9DecStart(ucVdecId, ucEsId);
                    ucTryCount = 100;
                }
            }
        }while((i4Ret != OSR_OK) && (0 != ucTryCount));
        
        _VDEC_Vp9ChkVldStatus(ucEsId, u4CoreId);
        if(i4Ret != OSR_OK)
        {
            _VDEC_Vp9ChkStatus();
            VP9_PRINT_ERROR(ucEsId, "decode time out, wait here for 60s, please log to mtk40650......");
            vVDEC_HAL_VP9_VDec_DumpReg(0, prVdecEsInfo->ucDualCoreMode, TRUE);
            x_thread_delay(600000);
        }
        prCommon->fgReDecode = FALSE;

        if(!prVdecEsInfo->ucDualCoreMode)
        {
            // Compare Prob & Count ?
            if (prUnCompress->u4RefreshFrameContext || prCommon->fgLAEThread)
            {
                vVDEC_HAL_VP9_Get_Probs_Table(u4CoreId, prCommon->rProbTBLBuffer.u4BufVAddr, &prCommon->fc);
                if(prCommon->fgLAEThread)
                {
                    VDEC_LAE_DecPrm rLaeIn;
                    VP9_LAE_INFO_T* prVp9LaeOut = prCommon->prVp9LaeInput;
                    x_memcpy(prVp9LaeOut, prCommon, sizeof(VP9_LAE_INFO_T));
                    rLaeIn.pvExtraData = (void*)(prVp9LaeOut);
                    vPutBufferToMcore(prCommon->ucPairedEsId, &rLaeIn);
                }
            }

            if (prUnCompress->u4RefreshFrameContext && !prUnCompress->u4ErrResilenceMode && !prUnCompress->u4FrameParallelDecodingMode) 
            {
                vVDEC_HAL_VP9_Get_Counts_Table(u4CoreId, prCommon->rCountTBLBuffer.u4BufVAddr, &prCommon->counts);
                vVP9AdaptCoefProbs(prCommon);
                if (!fgVP9IntraOnly(prCommon))
                {
                  vVP9AdaptModeProbs(prCommon);
                  vVP9AdaptMvProbs(prCommon, prUnCompress->u4AllowHighPrecisionMv);
                }
            } 
            else 
            {        
            //   debug_check_frame_counts(cm);
            }
            
            if (prUnCompress->u4RefreshFrameContext)
                prCommon->frame_contexts[prUnCompress->u4FrameContextIdx] = prCommon->fc;
            
            for(i = 0; i < REFS_PER_FRAME; i++)
            {
                prCommon->FRAME_REFS[i].rScaleFactors.i4X_scale_fp = REF_INVALID_SCALE;
                prCommon->FRAME_REFS[i].rScaleFactors.i4Y_scale_fp = REF_INVALID_SCALE;
                prCommon->FRAME_REFS[i].rScaleFactors.i4X_step_q4 = REF_INVALID_STEP;        
                prCommon->FRAME_REFS[i].rScaleFactors.i4Y_step_q4 = REF_INVALID_STEP;
                prCommon->FRAME_REFS[i].rScaleFactors.u4Ref_Scaling_EN = 0;
            }
            
        }
        else
        {            
            VDEC_LAE_DecPrm rLaeIn;
            VP9_LAE_INFO_T* prVp9Lae = prCommon->prVp9LaeInput;
            rLaeIn.pvExtraData = (void*)(prVp9Lae);
            vPutBufferToLae(prCommon->ucPairedEsId, &rLaeIn);
            vVDEC_HAL_VP9_Mcore_Reset();
        }

        if(!prVdecEsInfoKeep->fgPatternMode && !prCommon->fgLAEThread)
        {
            _VDEC_Vp9UpdDmxRptr(ucEsId, prVdecEsInfoKeep->rPesInfo.u4VldReadPtr);
        }
        prCommon->u4LastWidth = prUnCompress->u4Width;
        prCommon->u4LastHeight = prUnCompress->u4Height;
        prCommon->u4LastShowFrame = prUnCompress->u4ShowFrame;
        prUnCompress->fgResetSeg = FALSE;
    }

    prCommon->u4FrameNum++;
    prCommon->u4TotalDecoded++;
    
    vVP9SwapFrameBuffers(prCommon);
    if(prVdecEsInfoKeep->fgPatternMode && !prCommon->fgLAEThread && !prUnCompress->u4ShowExisting)
    {
        for (i = 0; i < 4; i++)
        {
            if(prCommon->fgDualCoreFrame)
            {
                prCommon->prFrameToShow->u4CrcY[i] = u4VDEC_HAL_VP9_VDec_ReadCRC_Y0(i, u4CoreId);
                prCommon->prFrameToShow->u4CrcY[4 + i] = u4VDEC_HAL_VP9_VDec_ReadCRC_Y1(i, u4CoreId);
            }
            else // single core
            {                       
                prCommon->prFrameToShow->u4CrcY[i] = u4VDEC_HAL_VP9_VDec_ReadCRC_Y0(i, u4CoreId);
            }       
        }
                
        for (i = 0; i < 4; i++)
        {        
            if(prCommon->fgDualCoreFrame)
            {
                prCommon->prFrameToShow->u4CrcC[i] = u4VDEC_HAL_VP9_VDec_ReadCRC_C0(i, u4CoreId);
                prCommon->prFrameToShow->u4CrcC[4 + i] = u4VDEC_HAL_VP9_VDec_ReadCRC_C1(i, u4CoreId);
            }
            else // single core
            {
                prCommon->prFrameToShow->u4CrcC[i] = u4VDEC_HAL_VP9_VDec_ReadCRC_C0(i, u4CoreId);
            }
        }
    }

    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    
    if(prVdecEsInfoKeep->fgPatternMode)
    {
        VDEC_INFO_T *prVdecInfo = _VDEC_GetInfo();
        if(prVdecInfo->pfErrNotify && !prCommon->fgLAEThread && prVdecEsInfoKeep->rPesInfo.fgEos)
        {
            prVdecInfo->pfErrNotify((UINT32)VDEC_EVT_DEC_BS_END, 0, 0, 0);
        }
    }

    if(!prCommon->fgLAEThread)
        VDEC_CheckStopStatus(prCommon->ucEsId, prVdecEsInfoKeep->rPesInfo.u4PTS, 0);    
    return E_VDEC_OK;
}

VOID _VDEC_Vp9GetDispPic(UCHAR ucEsId, void* pvInfo)
{
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    //UINT32 u4YAddr = 0,u4CAddr = 0;
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;

    if(prCommon->rUnCompressedHeader.u4ShowExisting)
    {
        VP9_PRINT_INFO(ucEsId, "skip existing compare");
        prDispInfo->pvYAddr     = NULL;
        return;
    }
    
    prDispInfo->pvYAddr     = (void *)prCommon->prFrameToShow->rBufY.u4BufVAddr;
    prDispInfo->pvCAddr     = (void*)prCommon->prFrameToShow->rBufC.u4BufVAddr;
    prDispInfo->u4YSize     = prCommon->prFrameToShow->rBufY.u4BufSize;
    prDispInfo->u4CSize     = prCommon->prFrameToShow->rBufC.u4BufSize;
    prDispInfo->u4CurrentPicNo= prCommon->u4FrameNum - 1;
    
    prDispInfo->ucDualMode = prCommon->fgDualCoreFrame;    
    prDispInfo->ucUFOEnable = prCommon->fgUFOModeEnable;

    //crc result save
    prDispInfo->u4Ycrc[0]=prCommon->prFrameToShow->u4CrcY[0];
    prDispInfo->u4Ycrc[1]=prCommon->prFrameToShow->u4CrcY[1];
    prDispInfo->u4Ycrc[2]=prCommon->prFrameToShow->u4CrcY[2];
    prDispInfo->u4Ycrc[3]=prCommon->prFrameToShow->u4CrcY[3];
    
    prDispInfo->u4Ccrc[0]=prCommon->prFrameToShow->u4CrcC[0];
    prDispInfo->u4Ccrc[1]=prCommon->prFrameToShow->u4CrcC[1];
    prDispInfo->u4Ccrc[2]=prCommon->prFrameToShow->u4CrcC[2];
    prDispInfo->u4Ccrc[3]=prCommon->prFrameToShow->u4CrcC[3];

    if(prCommon->fgDualCoreFrame)
    {
        prDispInfo->u4Ycrc[4]=prCommon->prFrameToShow->u4CrcY[4];
        prDispInfo->u4Ycrc[5]=prCommon->prFrameToShow->u4CrcY[5];
        prDispInfo->u4Ycrc[6]=prCommon->prFrameToShow->u4CrcY[6];
        prDispInfo->u4Ycrc[7]=prCommon->prFrameToShow->u4CrcY[7];
        
        prDispInfo->u4Ccrc[4]=prCommon->prFrameToShow->u4CrcC[4];
        prDispInfo->u4Ccrc[5]=prCommon->prFrameToShow->u4CrcC[5];
        prDispInfo->u4Ccrc[6]=prCommon->prFrameToShow->u4CrcC[6];
        prDispInfo->u4Ccrc[7]=prCommon->prFrameToShow->u4CrcC[7];

    }
    //prDispInfo->u1PicStruct=prDecInfo->rDecParam.u4DbkType;
}


VOID _VDEC_Vp9Stop(UCHAR ucEsId)
{
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VP9_LAE_INFO_T *_rLaeInfo = NULL; 
    VP9_PRINT_INFO(ucEsId, "stop");

    if(prCommon->ucFbgId != FBM_FBG_ID_UNKNOWN && !prCommon->fgLAEThread)
    {
        prVdecEsInfoKeep->fgLowLatency = FALSE;
        prVdecEsInfo->fgKeepPes = FALSE;
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        //FBM_WaitUnlockFrameBuffer(prCommon->ucFbgId, WAIT_DISP_UNLOCK_TIME);
        ASSERT(prCommon->ucFbgId == prVdecEsInfo->ucFbgId);
        FBM_ReleaseGroup(prCommon->ucFbgId);
        if(prVdecEsInfo->ucDualCoreMode)
        {
            VP9_COMMON_T* prLaeCommon = prVP9GetCommon(prCommon->ucPairedEsId);
            VDEC_ES_INFO_T* prLaeVdecEsInfo = _VDEC_GetEsInfo(prCommon->ucPairedEsId);
            prLaeVdecEsInfo->ucFbgId = FBM_FBG_ID_UNKNOWN;
            prLaeCommon->ucFbgId = FBM_FBG_ID_UNKNOWN;
        }
    }
    else
    {
        if(prCommon->fgLAEThread)
        {
            UINT16 u2MsgQIdx;
            SIZE_T zMsgSize = sizeof(VDEC_MSG_INTO_T);
            _rLaeInfo = GetLaeInfo();
            while (x_msg_q_receive(&u2MsgQIdx, (void*)(_rLaeInfo), &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
            {
                VP9_PRINT_DEBUG(ucEsId, "clear lae msgQ when stop");
            }
            //VP9_PRINT_INFO(ucEsId, "lae skip stop, take care by main");
        }
    }
    prCommon->fgOccupied = FALSE;
}

BOOL _VDEC_Vp9IsSupport(VOID)
{
    return IS_SupportVP9();
}

VOID _VDEC_Vp9GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);

    if(u4Type == (UINT32)DISP_IDX)
    {
        *pu4Param1 = (UINT32)prCommon->ucFbgId;
    }
    else if(u4Type == (UINT32)VDEC_STATUS)
    {
        if(prCommon->fgLAEThread)
        {            
            *pu4Param1 = LAE_DECODE;
        }
        else
        {
            *pu4Param1 = REAL_DECODE;
        }
    }
}

VOID _VDEC_Vp9SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    if (u4Type == (UINT32)VDEC_UFO_MODE)
    {
        VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
        prCommon->fgUFOModeEnable = (BOOL)u4Param1;
        VP9_PRINT_INFO(ucEsId, "UFO Enabled");
    }
    else if(u4Type == 0xFF)
    {
        _aucVp9LogOnOff[u4Param1] = u4Param2;
    }
}

VOID _VDEC_Vp9ChkStatus(VOID)
{
    UCHAR ucEsId;
    for (ucEsId = 0; ucEsId< VDEC_MAX_ES; ucEsId++)
    {
        VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
        if(prCommon->fgOccupied)
        {
            VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
            VDEC_PES_INFO_T* pPesInfo = &(prVdecEsInfoKeep->rPesInfo);
            VP9_UNCOMPRESSED_HEADER_T* prUnCompressed = &prCommon->rUnCompressedHeader;
            if(!prCommon->fgLAEThread)
            {
                UINT32 u4TileRow;
                UINT32 u4TileCol;
                UINT32 u4TileRows = prCommon->u4TileRows;
                UINT32 u4TileCols = prCommon->u4TileCols;
                printk("\n");
                VP9_PRINT_INFO(ucEsId, "|------------------------------Current Pic Info------------------------------");
                VP9_PRINT_INFO(ucEsId, "|-- input frm addr [0x%x--0x%x] size:0x%x, fifo[0x%x--0x%x]", pPesInfo->u4VldReadPtr, pPesInfo->u4VldReadPtrEnd, (pPesInfo->u4VldReadPtrEnd - pPesInfo->u4VldReadPtr), pPesInfo->u4FifoStart, pPesInfo->u4FifoEnd);
                VP9_PRINT_INFO(ucEsId, "|-- size [%d x %d], show frame: %d, pts:0x%x", prUnCompressed->u4Width, prUnCompressed->u4Height, prUnCompressed->u4ShowFrame, pPesInfo->u4PTS);
                VP9_PRINT_INFO(ucEsId, "|-- frame type: %c, intra only: %d", ((prUnCompressed->u4FrameType == KEY_FRAME) ? 'I' : 'P'), prUnCompressed->u4IntraOnly);
                if(prUnCompressed->u4ShowExisting)
                    VP9_PRINT_INFO(ucEsId, "|-- show existing: %d, with %d", prUnCompressed->u4ShowExisting, prUnCompressed->u4FrameToShow);
                VP9_PRINT_DEBUG(ucEsId, "|-- ssx & ssy [%d x %d]", prUnCompressed->u4SubSampling_X, prUnCompressed->u4SubSampling_Y);
                VP9_PRINT_DEBUG(ucEsId, "|-- color: %d", prUnCompressed->u4ColorSpace);
                VP9_PRINT_INFO(ucEsId, "|-- seg on/off: %d", prUnCompressed->seg.enabled);
                VP9_PRINT_INFO(ucEsId, "|-- profile: %d, bit depth: %d", prUnCompressed->u4Profile, ((prUnCompressed->u4BitDepth == BITS_8) ? 8 : 10));
                VP9_PRINT_DEBUG(ucEsId, "|-- err resilence mode: %d, allow hp: %d, parallel mode: %d", prUnCompressed->u4ErrResilenceMode, prUnCompressed->u4AllowHighPrecisionMv, prUnCompressed->u4FrameParallelDecodingMode);
                VP9_PRINT_DEBUG(ucEsId, "|-- refresh flags: 0x%x", prUnCompressed->u4RefreshFrameFlags);
                VP9_PRINT_DEBUG(ucEsId, "|-- rst frm context flag: %d", prUnCompressed->u4ResetFrameContext);
                VP9_PRINT_DEBUG(ucEsId, "|-- refresh frame context: %d, frame context idx: %d", prUnCompressed->u4RefreshFrameContext, prUnCompressed->u4FrameContextIdx);
                VP9_PRINT_INFO(ucEsId, "|-- tile rows & cols [%d x %d]", prUnCompressed->u4Log2TileRows, prUnCompressed->u4Log2TileCols);
                VP9_PRINT_INFO(ucEsId, "|-- uncompress size: 0x%x, 1st partition size: 0x%x", prUnCompressed->u4UnCompressSize, prUnCompressed->u4FirstPartitionSize);
                for (u4TileRow = 0; u4TileRow < u4TileRows; ++u4TileRow)
                {
                    for (u4TileCol = 0; u4TileCol < u4TileCols; ++u4TileCol)
                    {
                        TileBuffer *pTileBuf = &(prCommon->TILE_INFOS[u4TileRow][u4TileCol]);
                        if (pTileBuf->fgValidTile)
                        {
                            VP9_PRINT_DEBUG(ucEsId, "|-- tile[%d:%d] addr:0x%x, size:0x%x", u4TileRow, u4TileCol, pTileBuf->data, pTileBuf->size);
                        }        
                    }
                }
                printk("\n");
                VP9_PRINT_INFO(ucEsId, "|------------------------------STATISTICS-----------------------------");
                if(VDEC_MAX_ES != prCommon->ucPairedEsId)
                {
                    VP9_COMMON_T* prLaeCommon = prVP9GetCommon(prCommon->ucPairedEsId);
                    VP9_PRINT_DEBUG(ucEsId, "|-- Lae MAX Cost:%dms", prLaeCommon->u4MaxDecodeTime);
                    VP9_PRINT_INFO(ucEsId, "|-- Lae AVG Cost:%dms", prLaeCommon->u4AvgDecodeTime);
                }
                VP9_PRINT_INFO(ucEsId, "|-- MAX Cost:%dms  AVG Cost:%dms", prCommon->u4MaxDecodeTime, prCommon->u4AvgDecodeTime);
                VP9_PRINT_INFO(ucEsId, "|-- Total Decoded: %d", prCommon->u4TotalDecoded);
                printk("\n");
            }
        }
    }
}

VOID _VDEC_Vp9Flush(UCHAR ucEsId,BOOL fgIsDisp)
{
    VP9_COMMON_T* prCommon = prVP9GetCommon(ucEsId);
    UINT32 u4Index=0;
    UNUSED(fgIsDisp);
    if(prCommon->fgLAEThread)
    {
        VP9_PRINT_INFO(ucEsId, "lae skip flush, take care by main");
        return;
    }
    VP9_PRINT_INFO(ucEsId, "flush");

    // TODO: flush lae first
    for (u4Index = 0; u4Index < FRAME_BUFFERS; u4Index++)
    {
        if (prCommon->FRAME_BUFS[u4Index].u4RefCount > 0)
        {
            prCommon->FRAME_BUFS[u4Index].u4RefCount = 0;
            FBM_UpdateReferenceList(prCommon->FRAME_BUFS[u4Index].rBuf.u4FbgId, prCommon->FRAME_BUFS[u4Index].rBuf.u4FbId, FALSE);
        }
    }
    
    for(u4Index = 0; u4Index< REFS_PER_FRAME; u4Index++)
    {
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4X_scale_fp = REF_INVALID_SCALE;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4Y_scale_fp = REF_INVALID_SCALE;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4X_step_q4 = REF_INVALID_STEP;        
        prCommon->FRAME_REFS[u4Index].rScaleFactors.i4Y_step_q4 = REF_INVALID_STEP;
        prCommon->FRAME_REFS[u4Index].rScaleFactors.u4Ref_Scaling_EN = 0;
    }
    
    x_memset(prCommon->REF_FRAME_SIGN_BIAS, 0, sizeof(prCommon->REF_FRAME_SIGN_BIAS));
    for(u4Index = 0; u4Index < REF_FRAMES; u4Index ++)
    {
        prCommon->REF_FRAME_MAP[u4Index] = -1;
    }
    prCommon->COMP_FIXED_REF = INTRA_FRAME;
    prCommon->u4NewFbIdx = -1;
    vVP9_Init_Dequantizer(prCommon);
    //if(FBM_FBG_ID_UNKNOWN != prCommon->ucFbgId)
    //    FBM_QueryStatus(prCommon->ucFbgId);
    prCommon->u4FrameNum = 0;

    return;
}

VOID _VDEC_Vp9UpdDmxRptr(UCHAR ucEsId , UINT32 u4DmxStopHere)
{
    if (!VDEC_SetRptr(ucEsId, u4DmxStopHere, u4DmxStopHere))                
    {
        VP9_PRINT_ERROR(ucEsId, "set rptr 0x%x to dmx failed\n", u4DmxStopHere);
    }
}

#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 _VDEC_Vp9PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
    UCHAR *pucBuf;
    UINT32 u4BitOffset = 0;
    UINT32 u4Profile;
    UINT32 u4FrameType;
    UINT32 u4ShowFrame;
    UINT32 u4ErrResilenceMode;
    UINT32 u4BitDepth = BITS_8;
    UINT32 u4IntraOnly;
    UINT32 u4ResetFrameContext;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    pucBuf = (UCHAR*)(prVdecEsInfoKeep->au1SeqInfo);
#else
    pucBuf = (UCHAR*)VIRTUAL(prVdecEsInfoKeep->rPesInfo.u4VldReadPtr);
#endif
    
    if (i4VP9ReadLiteral(pucBuf, &u4BitOffset, 2) != VP9_FRAME_MARKER)
    {        
        VP9_PRINT_ERROR(ucEsId, "invalid frame marker");
        return E_VDEC_SKIPFRAME;
    }
    u4Profile = (i4VP9ReadBit(pucBuf, &u4BitOffset) | i4VP9ReadBit(pucBuf, &u4BitOffset) << 1);
    if (u4Profile >= MAX_PROFILES)
    {
        VP9_PRINT_ERROR(ucEsId, "unsupported bitstream profile");
        return E_VDEC_SKIPFRAME;
    }
        
    if (i4VP9ReadBit(pucBuf, &u4BitOffset)) 
    {
        VP9_PRINT_ERROR(ucEsId, "show existing frame, cannot used for pre-parser");
        return E_VDEC_SKIPFRAME;
    }
    
    u4FrameType = (FRAME_TYPE)i4VP9ReadBit(pucBuf, &u4BitOffset);
    u4ShowFrame = i4VP9ReadBit(pucBuf, &u4BitOffset);
    u4ErrResilenceMode = i4VP9ReadBit(pucBuf, &u4BitOffset);
    if (u4FrameType == KEY_FRAME) {
        if (i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_0 ||
            i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_1 ||
            i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_2) 
        {
            VP9_PRINT_ERROR(ucEsId, "invalid frame sync code");
            return E_VDEC_SKIPFRAME;
        }

        if (u4Profile > PROFILE_1)
            u4BitDepth = i4VP9ReadBit(pucBuf, &u4BitOffset) ? BITS_12 : BITS_10;

        if (i4VP9ReadLiteral(pucBuf, &u4BitOffset, 3) != SRGB) {
            i4VP9ReadBit(pucBuf, &u4BitOffset);  // [16,235] (including xvycc) vs [0,255] range
            if (u4Profile >= PROFILE_1) {
                i4VP9ReadLiteral(pucBuf, &u4BitOffset, 3);
            }
        } else {
            VP9_PRINT_ERROR(ucEsId, "RGB not supported yet");
            return E_VDEC_SKIPFRAME;
        }

        prVdecEsInfo->u2OrgHSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
        prVdecEsInfo->u2OrgVSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
    }
    else {
        u4IntraOnly = u4ShowFrame ? 0 : i4VP9ReadBit(pucBuf, &u4BitOffset);        
        u4ResetFrameContext = u4ErrResilenceMode ? 0 : i4VP9ReadLiteral(pucBuf, &u4BitOffset, 2);        
        if (u4IntraOnly) {
            if (i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_0 ||
                i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_1 ||
                i4VP9ReadLiteral(pucBuf, &u4BitOffset, 8) != VP9_SYNC_CODE_2) 
            {
                VP9_PRINT_ERROR(ucEsId, "invalid frame sync code");
                return E_VDEC_SKIPFRAME;
            }
            i4VP9ReadLiteral(pucBuf, &u4BitOffset, REF_FRAMES);
            prVdecEsInfo->u2OrgHSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
            prVdecEsInfo->u2OrgVSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
        } else {
            UINT32 u4Idx;
            i4VP9ReadLiteral(pucBuf, &u4BitOffset, REF_FRAMES);
            for (u4Idx = 0; u4Idx < REFS_PER_FRAME; ++u4Idx) {                
                i4VP9ReadLiteral(pucBuf, &u4BitOffset, REF_FRAMES_LOG2);
                i4VP9ReadBit(pucBuf, &u4BitOffset);             
            }
            
            for (u4Idx = 0; u4Idx < REFS_PER_FRAME; ++u4Idx) {
              if (i4VP9ReadBit(pucBuf, &u4BitOffset)) {
                  VP9_PRINT_ERROR(ucEsId, "resolution not contained in this frame");
                  return E_VDEC_SKIPFRAME;
              }
            }
            prVdecEsInfo->u2OrgHSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
            prVdecEsInfo->u2OrgVSize = i4VP9ReadLiteral(pucBuf, &u4BitOffset, 16) + 1;
        }
    }
    
    prVdecEsInfo->u4BitDepth = (u4BitDepth == BITS_8) ? 8 : 10;
    prVdecEsInfo->fgProgressive = TRUE;

    switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
    {
    case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
        prVdecEsInfo->u4FrameRate = 2397;
        break;
    case VDEC_SRC_FRAME_RATE_24:
        prVdecEsInfo->u4FrameRate = 2400;
        break;
    case VDEC_SRC_FRAME_RATE_25:
        prVdecEsInfo->u4FrameRate = 2500;
        break;
    case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
        prVdecEsInfo->u4FrameRate = 2997;
        break;
    case VDEC_SRC_FRAME_RATE_30:
        prVdecEsInfo->u4FrameRate = 3000;
        break;
    case VDEC_SRC_FRAME_RATE_50:
        prVdecEsInfo->u4FrameRate = 5000;
        break;
    case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
        prVdecEsInfo->u4FrameRate = 5994;
        break;
    case VDEC_SRC_FRAME_RATE_60:
        prVdecEsInfo->u4FrameRate = 6000;
        break;
    case VDEC_SRC_FRAME_RATE_12:
       prVdecEsInfo->u4FrameRate = 1200;
       break;
    case VDEC_SRC_FRAME_RATE_15:
       prVdecEsInfo->u4FrameRate = 1500;
       break;
     case VDEC_SRC_FRAME_RATE_20:
        prVdecEsInfo->u4FrameRate = 2000;
        break;
    default:
        prVdecEsInfo->u4FrameRate = 0;
        break;
    }

    VP9_PRINT_INFO(ucEsId, "pre-parser ok --> [%d x %d] @ %d", prVdecEsInfo->u2OrgHSize, prVdecEsInfo->u2OrgVSize, prVdecEsInfo->u4FrameRate);
    return E_VDEC_OK;
}
#endif

