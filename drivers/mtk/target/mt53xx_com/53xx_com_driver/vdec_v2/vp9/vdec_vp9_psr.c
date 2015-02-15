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
 * $RCSfile: vdec_vp8psr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_vp8par.c
 *  This source file include vp8 decode flow and memory contral
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "fbm_drvif.h"
#include "vdec_drvif.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "x_timer.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END
#include "vdec_vp9_psr.h"
#include "vdec_vp9_dec.h"
#include "vdec_info_vp9.h"
#include "vdec_hal_if_vp9.h"

INT32 i4VP9ReadBit(UCHAR* pucBuf, UINT32* pu4BitOffset) {
    UINT32 u4Off = *pu4BitOffset;
    UCHAR P = u4Off / 8;
    INT32 Q = 8 - 1 - (INT32)u4Off % 8;
    INT32 i4Bit = (pucBuf[P] & (1 << Q)) >> Q;
    *pu4BitOffset = u4Off + 1;
    return i4Bit;
}

INT32 i4VP9ReadLiteral(UCHAR* pucBuf, UINT32* pu4BitOffset, UCHAR ucBits) {
    INT32 i4Value = 0, i4Bit;
    for (i4Bit = ucBits - 1; i4Bit >= 0; i4Bit--)
        i4Value |= i4VP9ReadBit(pucBuf, pu4BitOffset) << i4Bit;
    return i4Value;
}

INT32 i4VP9CheckSyncCode(UINT32 u4CoreId) {
  if (u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 8) != VP9_SYNC_CODE_0 ||
      u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 8) != VP9_SYNC_CODE_1 ||
      u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 8) != VP9_SYNC_CODE_2) 
  {
      VP9_ERROR("invalid frame sync code");
      return VP9_FAIL;
  }
  return VP9_OK;
}

TX_MODE eVP9ReadTxMode(UINT32 u4CoreId) {
    TX_MODE eTxMode = (TX_MODE)(u4VDEC_HAL_VP9_Read_Literal(u4CoreId, 2));

    if (eTxMode == ALLOW_32X32)
    {
        eTxMode += u4VDEC_HAL_VP9_Read_Bit(u4CoreId);
    }
    return eTxMode;
}

BOOL bVP9IsCompoundRefAllowed(VP9_COMMON_T * prCommon) {
    UINT32 i;
    for (i = 1; i < REFS_PER_FRAME; ++i)
    {
        if (prCommon->REF_FRAME_SIGN_BIAS[i + 1] != prCommon->REF_FRAME_SIGN_BIAS[1])
          return TRUE;
    }
    return FALSE;
}

REFERENCE_MODE eVP9ReadFrameReferenceMode(VP9_COMMON_T * prCommon, UINT32 u4CoreId) {
    if (bVP9IsCompoundRefAllowed(prCommon)) {
        REFERENCE_MODE eResult;
        eResult = u4VDEC_HAL_VP9_Read_Bit(u4CoreId) ? (u4VDEC_HAL_VP9_Read_Bit(u4CoreId) ? REFERENCE_MODE_SELECT
                                                  : COMPOUND_REFERENCE) : SINGLE_REFERENCE;
        return eResult;
    } else {
        return SINGLE_REFERENCE;
    }
}

void vVP9ReadFrameRefModeProbs(VP9_COMMON_T * prCommon, UINT32 u4CoreId) {
    if (prCommon->eRefMode== REFERENCE_MODE_SELECT)
    {
        vVDEC_HAL_VP9_UPDATE_COMP_INTER_PROBS(u4CoreId);
    }

    if (prCommon->eRefMode != COMPOUND_REFERENCE)
    {
        vVDEC_HAL_VP9_UPDATE_SINGLE_REF_PROBS(u4CoreId);
    }

    if (prCommon->eRefMode != SINGLE_REFERENCE)
    {
        vVDEC_HAL_VP9_UPDATE_COMP_REF_PROBS(u4CoreId);
    }
}

void vVP9ReadMvProbs(UINT32 u4AllowHp, UINT32 u4CoreId) {
    vVDEC_HAL_VP9_UPDATE_MVD_INT_PROBS(u4CoreId);
    vVDEC_HAL_VP9_UPDATE_MVD_FP_PROBS(u4CoreId);

    if (u4AllowHp) {
        vVDEC_HAL_VP9_UPDATE_MVD_HP_PROBS(u4CoreId);
    }
}

INT32 i4VP9UnCompressHdrPrs(VP9_COMMON_T * prCommon, VP9_UNCOMPRESSED_HEADER_T * prUnCompressed, UINT32 u4CoreId)
{
    UINT32 i;
    UINT32 u4QIndex;
    UINT32 u4BitsRead;
    prCommon->u4NewFbIdx = -1;
    prCommon->u4LastFrameType = prUnCompressed->u4FrameType;  
    prCommon->pCurrentFB = NULL;
    prCommon->prFrameToShow = NULL;
    if (u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 2) != VP9_FRAME_MARKER)
    {        
        VP9_PRINT_ERROR(prCommon->ucEsId, "Invalid frame marker");
        return VP9_FAIL;
    }
    prUnCompressed->u4Profile = (u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) | u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) << 1);
    if (prUnCompressed->u4Profile >= MAX_PROFILES)
    {
        VP9_PRINT_ERROR(prCommon->ucEsId, "unsupported bitstream profile %d", prUnCompressed->u4Profile);
        return VP9_FAIL;
    }
    
    prUnCompressed->u4ShowExisting = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (prUnCompressed->u4ShowExisting) 
    {
        // show an existing frame directly
        prUnCompressed->u4FrameToShow = prCommon->REF_FRAME_MAP[u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 3)];
        if (prCommon->FRAME_BUFS[prUnCompressed->u4FrameToShow].u4RefCount < 1)
        {            
            VP9_PRINT_ERROR(prCommon->ucEsId, "buffer %d does not contain a decoded frame", prUnCompressed->u4FrameToShow);
            return VP9_FAIL;
        }
        
        vVP9RefCntFb(prCommon, &prCommon->u4NewFbIdx, prUnCompressed->u4FrameToShow, FALSE);
        prUnCompressed->u4RefreshFrameFlags = 0;
        prUnCompressed->rLoopFilter.filter_level = 0;
        prUnCompressed->u4ShowFrame = 1;        
        prUnCompressed->u4UnCompressSize = 0;
        return VP9_SKIP_FRAME;
    }

    prUnCompressed->u4FrameType = (FRAME_TYPE) u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    prUnCompressed->u4ShowFrame = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    prUnCompressed->u4ErrResilenceMode = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    if (prUnCompressed->u4FrameType == KEY_FRAME) 
    {
        if(VP9_OK != i4VP9CheckSyncCode(u4CoreId))
        {            
            return VP9_FAIL;
        }
        
        if (prUnCompressed->u4Profile > PROFILE_1)
        {
            prUnCompressed->u4BitDepth = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId) ? BITS_12 : BITS_10;
        }
        
        prUnCompressed->u4ColorSpace = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 3);  // colorspace        
        if (prUnCompressed->u4ColorSpace != SRGB) 
        {
            u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);  // [16,235] (including xvycc) vs [0,255] range
            if (prUnCompressed->u4Profile >= PROFILE_1) 
            {
                prUnCompressed->u4SubSampling_X = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
                prUnCompressed->u4SubSampling_Y = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
                u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);  // has extra plane
            }
            else 
            {
                prUnCompressed->u4SubSampling_Y = prUnCompressed->u4SubSampling_X = 1;
            }
        }
        else
        {
            if (prUnCompressed->u4Profile >= PROFILE_1)
            {
                prUnCompressed->u4SubSampling_Y = prUnCompressed->u4SubSampling_X = 0;
                u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);  // has extra plane
            }
            else
            {
                VP9_PRINT_ERROR(prCommon->ucEsId, "RGB not supported in profile 0");
                return VP9_FAIL;
            }
            VP9_PRINT_ERROR(prCommon->ucEsId, "RGB not supported yet");
            return VP9_FAIL;
        }
        prUnCompressed->u4RefreshFrameFlags = (1 << REF_FRAMES) - 1;
        if(VP9_FAIL == i4VP9SetupFrameSize(u4CoreId, prCommon))
            return VP9_FAIL;
        for (i = 0; i < REFS_PER_FRAME; ++i)
        {
            prCommon->FRAME_REFS[i].u4Idx = prCommon->u4NewFbIdx;
            prCommon->FRAME_REFS[i].prBuf = &prCommon->FRAME_BUFS[prCommon->u4NewFbIdx].rBuf;
        }
    }
    else 
    {
        prUnCompressed->u4IntraOnly = prUnCompressed->u4ShowFrame ? 0 : u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);        
        prUnCompressed->u4ResetFrameContext = prUnCompressed->u4ErrResilenceMode ? 0 : u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 2);        
        if (prUnCompressed->u4IntraOnly)
        {
            if(VP9_OK != i4VP9CheckSyncCode(u4CoreId))
            {
                return VP9_FAIL; 
            }
            prUnCompressed->u4RefreshFrameFlags = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, REF_FRAMES);
            i4VP9SetupFrameSize(u4CoreId, prCommon);
        }
        else
        {
            prUnCompressed->u4RefreshFrameFlags = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, REF_FRAMES);
            for (i = 0; i < REFS_PER_FRAME; ++i) 
            {                
                UINT32 u4Ref = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, REF_FRAMES_LOG2);
                UINT32 u4FbIdx = prCommon->REF_FRAME_MAP[u4Ref];
                prCommon->FRAME_REFS[i].u4Idx = u4FbIdx;
                prCommon->FRAME_REFS[i].prBuf = &prCommon->FRAME_BUFS[u4FbIdx].rBuf;
                prCommon->REF_FRAME_SIGN_BIAS[LAST_FRAME + i] = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);             
            }

            if(!prCommon->fgLAEThread)
                VP9_PRINT_VERBOSE(prCommon->ucEsId, "ref list -> (%d,%d) (%d,%d) (%d,%d)", prCommon->FRAME_REFS[0].prBuf->u4FbgId, prCommon->FRAME_REFS[0].prBuf->u4FbId,
                    prCommon->FRAME_REFS[1].prBuf->u4FbgId, prCommon->FRAME_REFS[1].prBuf->u4FbId, prCommon->FRAME_REFS[2].prBuf->u4FbgId, prCommon->FRAME_REFS[2].prBuf->u4FbId);
            if(VP9_OK != i4VP9SetupFrameSizeWithRefs(u4CoreId, prCommon))
                return VP9_FAIL;

            prUnCompressed->u4AllowHighPrecisionMv = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
            prUnCompressed->eInterpFilterType = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId)? SWITCHABLE : literal_to_filter[u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 2)];

            for (i = 0; i < REFS_PER_FRAME; ++i) 
            {
                 VP9_REF_BUF_T *rRefBuf = &prCommon->FRAME_REFS[i];
                 vVP9_Setup_Scale_Factors_For_Frame(&rRefBuf->rScaleFactors, rRefBuf->prBuf->u4YCropWidth, rRefBuf->prBuf->u4YCropHeight,
                                                     prCommon->rUnCompressedHeader.u4Width, 
                                                     prCommon->rUnCompressedHeader.u4Height);
            }
        }
    }

    if (!prUnCompressed->u4ErrResilenceMode) 
    {
        prUnCompressed->fgUse_Prev_MI = TRUE;
        prUnCompressed->u4RefreshFrameContext = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
        prUnCompressed->u4FrameParallelDecodingMode = u4VDEC_HAL_VP9_Read_Bit_Raw(u4CoreId);
    } 
    else 
    {
        prUnCompressed->fgUse_Prev_MI = FALSE;
        prUnCompressed->u4RefreshFrameContext = 0;
        prUnCompressed->u4FrameParallelDecodingMode = 1;
    }

    // This flag will be overridden by the call to vp9_setup_past_independence
    // below, forcing the use of context 0 for those frame types.
    prUnCompressed->u4FrameContextIdx = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, FRAME_CONTEXTS_LOG2);
    
    if ((prUnCompressed->u4FrameType == KEY_FRAME || prUnCompressed->u4IntraOnly) || prUnCompressed->u4ErrResilenceMode)
    {
        vVP9SetupPastIndependence(prCommon);
    }

    vVP9SetupLoopFilter(u4CoreId, &(prUnCompressed->rLoopFilter));
    vVP9SetupQuantization(prCommon, prUnCompressed, &prCommon->rMBD, u4CoreId);
    vVP9SetupSegmentation(&prUnCompressed->seg, u4CoreId);
    vVP9SetupTileInfo(prUnCompressed, u4CoreId);

    if(prUnCompressed->seg.enabled)
    {
        for(i = 0 ; i < 8; i ++)
        {
            u4QIndex = vp9_get_qindex(&prUnCompressed->seg, i, prUnCompressed->u4BaseQIdx);
            prCommon->au4DeQuant[i][0] = prCommon->au4Y_Dequant[u4QIndex][0];        
            prCommon->au4DeQuant[i][1] = prCommon->au4Y_Dequant[u4QIndex][1];
            prCommon->au4DeQuant[i][2] = prCommon->au4UV_Dequant[u4QIndex][0];
            prCommon->au4DeQuant[i][3] = prCommon->au4UV_Dequant[u4QIndex][1];
        }
    }
    else
    {
        prCommon->au4DeQuant[0][0] = prCommon->au4Y_Dequant[prUnCompressed->u4BaseQIdx][0];        
        prCommon->au4DeQuant[0][1] = prCommon->au4Y_Dequant[prUnCompressed->u4BaseQIdx][1];
        prCommon->au4DeQuant[0][2] = prCommon->au4UV_Dequant[prUnCompressed->u4BaseQIdx][0];
        prCommon->au4DeQuant[0][3] = prCommon->au4UV_Dequant[prUnCompressed->u4BaseQIdx][1];
    }
    
    prUnCompressed->u4FirstPartitionSize = u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, 16);
    if (prUnCompressed->u4FirstPartitionSize == 0)
    {
        VP9_PRINT_ERROR(prCommon->ucEsId, "invalid header size");
        return VP9_FAIL;
    }

    //align vld rp and reset bits count
    u4BitsRead = u4VDEC_HAL_VP9_Get_Bits_Count(u4CoreId);
    if(u4BitsRead & 0x7)
    {
        u4VDEC_HAL_VP9_Read_Literal_Raw(u4CoreId, (8 - (u4BitsRead & 0x7)));//shift vld to byte align
    }
    prUnCompressed->u4UnCompressSize = (u4BitsRead / 8) + ((u4BitsRead&0x7) > 0);
    return VP9_OK;
}

INT32 i4VP9CompressedHdrPrs(VP9_COMMON_T * prCommon, UINT32 u4CoreId) 
{
    UINT32 u4BytesRead = 0;
    UINT32 u4BitsRead = 0;
    VERIFY(u4VDEC_HAL_VP9_Reset_Bits_Count(u4CoreId) == 0);
    vVDEC_HAL_VP9_InitBool(u4CoreId);
    prCommon->eTxMode = prCommon->rUnCompressedHeader.u4Lossless ? ONLY_4X4 : eVP9ReadTxMode(u4CoreId);

    //set tx mode
    vVDEC_HAL_VP9_Set_TxMode(u4CoreId, prCommon->eTxMode);
    
    if (prCommon->eTxMode == TX_MODE_SELECT)
    {
        vVDEC_HAL_VP9_UPDATE_TX_PROBS(u4CoreId);
    }
    
    vVDEC_HAL_VP9_UPDATE_COEF_PROBS(u4CoreId);
    vVDEC_HAL_VP9_UPDATE_MBSKIP_PROBS(u4CoreId);
    
    if (!(prCommon->rUnCompressedHeader.u4IntraOnly || prCommon->rUnCompressedHeader.u4FrameType == KEY_FRAME)) {
        vVDEC_HAL_VP9_UPDATE_INTER_MODE_PROBS(u4CoreId);
        if (prCommon->rUnCompressedHeader.eInterpFilterType == SWITCHABLE)
        {        
            vVDEC_HAL_VP9_UPDATE_SWITCHABLE_INTERP_PROBS(u4CoreId);
        }

        vVDEC_HAL_VP9_UPDATE_INTRA_INTER_PROBS(u4CoreId);        
    
        prCommon->eRefMode = eVP9ReadFrameReferenceMode(prCommon, u4CoreId);

        if (prCommon->eRefMode != SINGLE_REFERENCE)
        {
            vVP9SetupCompoundRefMode(prCommon);
        }
        vVP9ReadFrameRefModeProbs(prCommon, u4CoreId);

        vVDEC_HAL_VP9_UPDATE_Y_MODE_PROBS(u4CoreId);
        vVDEC_HAL_VP9_UPDATE_PARTITION_PROBS(u4CoreId);
        vVP9ReadMvProbs(prCommon->rUnCompressedHeader.u4AllowHighPrecisionMv, u4CoreId);
    }

    u4BitsRead = u4VDEC_HAL_VP9_Get_Bits_Count(u4CoreId);
//	    u4BytesRead = u4BitsRead / 8;
    u4BytesRead = u4BitsRead / 8 + 1;
//	    VERIFY(u4BytesRead == prCommon->rUnCompressedHeader.u4FirstPartitionSize);
    prCommon->ucBitsOverAlign = u4BitsRead&0x7;
    return VP9_OK;
}

void vVP9TileInfoParse(VP9_COMMON_T* prCommon, UINT32 u4VaFrameStart, UINT32 u4VaFrameEnd, UINT32 u4VaFifoStart, UINT32 u4VaFifoEnd)
{
    UINT32 u4TileRows = 1 << prCommon->rUnCompressedHeader.u4Log2TileRows;
    UINT32 u4TileCols = 1 << prCommon->rUnCompressedHeader.u4Log2TileCols;
    UINT32 u4TileRow;
    UINT32 u4TileCol;
    UINT32 u4TileNum;
    UINT32 u4ActualTileRows = u4TileRows;
    UINT32 u4DataStart = u4VaFrameStart + prCommon->rUnCompressedHeader.u4UnCompressSize + prCommon->rUnCompressedHeader.u4FirstPartitionSize;
    for (u4TileRow = 0; u4TileRow < u4TileRows; ++u4TileRow) 
    {
        for (u4TileCol = 0; u4TileCol < u4TileCols; ++u4TileCol) 
        {
            TileBuffer * pTileBuf = &(prCommon->TILE_INFOS[u4TileRow][u4TileCol]);
            u4DataStart = (u4DataStart >= u4VaFifoEnd) ? (u4VaFifoStart + u4DataStart - u4VaFifoEnd) : u4DataStart;
            pTileBuf->col = u4TileCol;
            pTileBuf->fgLastTile = (u4TileRow == u4TileRows - 1) && (u4TileCol == u4TileCols - 1);
            u4DataStart += get_tile_buffer(u4DataStart, u4VaFrameEnd, u4VaFifoStart, u4VaFifoEnd, pTileBuf);            
            //VP9_PRINT_VERBOSE(prCommon->ucEsId, "tile info-> addr:0x%x, size:0x%x, row:%d col:%d", pTileBuf->data, pTileBuf->size, u4TileRow, u4TileCol);
        }
    }
    
      // Load all tile information into tile_data.
    for (u4TileRow = 0; u4TileRow < u4TileRows; ++u4TileRow) {
        for (u4TileCol = 0; u4TileCol < u4TileCols; ++u4TileCol) {
            VP9_TILE_INFO_T rTile;
            UINT32 u4Yy;
            vp9_tile_init(&rTile, prCommon, u4TileRow, u4TileCol);
            u4Yy = (((rTile.i4MiRowEnd - rTile.i4MiRowStart)+7)>>3);
            if (u4Yy == 0){
                if (u4TileCol == 0)
                    u4ActualTileRows = u4ActualTileRows - 1;
            }
        }
    }
    prCommon->u4TileRows = u4TileRows;
    prCommon->u4TileCols = u4TileCols;
    prCommon->u4ActualTileRows = u4ActualTileRows;
    u4TileNum = u4TileCols * u4ActualTileRows;
    for (u4TileRow = 0; u4TileRow < u4TileRows; ++u4TileRow)
    {
        VP9_TILE_INFO_T rTile;
        vp9_tile_set_row(&rTile, prCommon, u4TileRow);
        
        for (u4TileCol = 0; u4TileCol < u4TileCols; ++u4TileCol)
        {
            TileBuffer *pTileBuf;
            UINT32 u4TileAddr;
            UINT32 u4BitOffset;
            UINT32 u4TileSize;
            vp9_tile_set_col(&rTile, prCommon, u4TileCol);
            pTileBuf = &(prCommon->TILE_INFOS[u4TileRow][u4TileCol]);
            u4TileAddr = (PHYSICAL((UINT32)pTileBuf->data));
            u4BitOffset = (u4TileAddr - (PHYSICAL(u4VaFifoStart))) << 3;
            u4TileSize = pTileBuf->size;
            pTileBuf->fgValidTile = FALSE;
            if (((((rTile.i4MiRowEnd - rTile.i4MiRowStart)+7)>>3)))
            {
                UINT32 u4RiscData1 = (u4TileSize << 3) + (u4BitOffset & 0x7f);
                UINT32 u4RiscData2 = (u4TileAddr >> 4) << 4;//16 byte aligned part(barral shifter 16 byte align)
                UINT32 u4RiscData3 = (u4TileAddr <<3) & 0x7F;//non 16 byte aligned part
                UINT32 u4Yy = ((((rTile.i4MiRowEnd - rTile.i4MiRowStart)+7)>>3)-1);
                UINT32 u4Xx = ((((rTile.i4MiColEnd - rTile.i4MiColStart)+7)>>3)-1);
                UINT32 u4RiscData4 = ((u4Yy&0x1ff)<<6) + (u4Xx&0x3f);
                pTileBuf->u4TileRisc[0] = u4RiscData1;
                pTileBuf->u4TileRisc[1] = u4RiscData2;
                pTileBuf->u4TileRisc[2] = u4RiscData3;
                pTileBuf->u4TileRisc[3] = u4RiscData4;
                pTileBuf->fgValidTile = TRUE;
            }       
        }
    }
}

