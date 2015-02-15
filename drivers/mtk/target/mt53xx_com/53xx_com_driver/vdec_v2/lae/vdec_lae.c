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
 * $RCSfile: vdec_laeapi.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_laeapi.c
 *  This file contains implementation of exported APIs of VDEC LAE part.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_lae.h"
#include "../hw_common/vdec_hw_common.h"
#include "../vdec_flw.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_chip_id.h"
#include "x_common.h"
#include "x_typedef.h"
#include "dmx_if.h"
#include "drv_dbase.h"
//for clock setting
#include "x_hal_5381.h"
#include "x_mm_common.h"
#if defined(VDEC_IS_POST_MT5398)
#include "srm_drvif.h"
#endif
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
VDEC_LAE_DecPrm _arLAEBufGroup[MAX_LAE_BUF_GROUP_NUM][MAX_LAE_BUF_NS_PER_GROUP];
BOOL  _fgOcuupied[4] = {FALSE,FALSE,FALSE,FALSE};
UCHAR _gVDecMapLAE[4] = {LAE_INST_UNKNOWN,LAE_INST_UNKNOWN,LAE_INST_UNKNOWN,LAE_INST_UNKNOWN};


UCHAR ucLAEGetFreeLaeInst(UCHAR ucMainESid)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    UCHAR ucFreeLAE = 0xFF,ucEsId;
    //add sem ?

    for(ucEsId = (VDEC_MAX_ES - 1) ; ucEsId >= 0; ucEsId--)
    {
        if(ucEsId == ucMainESid)
            continue;
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        if((prVdecEsInfoKeep->eCurState == VDEC_ST_STOP) && (_fgOcuupied[ucEsId] == FALSE))
        {
            ucFreeLAE = ucEsId;
            break;
        }
    }

    return ucFreeLAE;
}

VOID vLAESetFree(UCHAR ucMainESid)
{
    UINT32 u4LaeId = 0xff;
    LOG(2,"LAE[%d] free done\n",ucMainESid);
    u4LaeId = _gVDecMapLAE[ucMainESid];
    _gVDecMapLAE[ucMainESid] = LAE_INST_UNKNOWN;
    _fgOcuupied[u4LaeId] = FALSE;
}


UCHAR ucVDecGetMcoreDecInst(UCHAR ucLaeESid)
{
    UCHAR ucMainEsid = 0xFF,i;
    
    for(i = 0; i < 4; i++)
    {
        if(_gVDecMapLAE[i] == ucLaeESid)
        {
            ucMainEsid = i;
        }
    }

    return ucMainEsid;
}

UCHAR ucVDecConnectLAE(UCHAR ucMainESid)
{
    UCHAR ucFreeLae;
    VDEC_ES_INFO_T *prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucMainESid);

    if((_gVDecMapLAE[ucMainESid] != LAE_INST_UNKNOWN) || (prVdecEsInfo->ucVDecStatus == LAE_DECODE))
    {
        LOG(0,"VdecMapLae[%d] %d,Vdec Status %d\n",ucMainESid,_gVDecMapLAE[ucMainESid],prVdecEsInfo->ucVDecStatus);
        return LAE_INST_REPEAT_ALLOC;
    }
    
    ucFreeLae = VDEC_MAX_ES - ucMainESid - 1;//ucLAEGetFreeLaeInst(ucMainESid);
    if(ucFreeLae == 0xFF)
    {
        prVdecEsInfo->ucLaeEsInfoId = 0xFF;
        //Temp fix VDEC_LAE0
        prVdecEsInfo->ucLaeId = VDEC_MAX_VLD;
        return 0xFF;
        LOG(0,"[H265-LAE] Err No free LAE for decode \n");
    }
    else
    {
        _fgOcuupied[ucFreeLae] = TRUE;
        _gVDecMapLAE[ucMainESid] = ucFreeLae;
        prVdecEsInfo->ucLaeEsInfoId = ucFreeLae;
        //Temp fix VDEC_LAE0
        prVdecEsInfo->ucLaeId = VDEC_LAE0;
    }
    
    return ucFreeLae;
}

BOOL fgLAEAllocateInfoBuf(UINT32 u4InstID,UINT32 u4VDecFmt, VDEC_LAE_DecPrm* prLAEDecPrm)
{
    UINT32 u4Addr = 0, u4CABACBufSize = 0, u4ECBufSize = 0;
    UINT32 u4BgIdx, u4BufIdx;
    UCHAR u4LaeInstID;
    VDEC_LAE_DecPrm *prLaeDecPrm;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    u4LaeInstID = prVdecEsInfo->ucLaeEsInfoId;
    //if (!_fgLAEBufAllocated)
    {
        LOG(7, "==================================\n");
        LOG(2, "fgLAEAllocateInfoBuf [%d]\n",prLAEDecPrm->u4PicH);
        LOG(7, "==================================\n");
        //u4Addr = (UINT32)x_mem_alloc(2*sizeof(LAE_BUF_INFO_T));
        //ASSERT(NULL != u4Addr);
        
        if (VDEC_FMT_H264 == u4VDecFmt || VDEC_FMT_H264VER == u4VDecFmt)
        {
            u4CABACBufSize = 1152 * (prLAEDecPrm->u4PicH/16);      
        }
        else if (VDEC_FMT_H265 == u4VDecFmt)
        {
            //u4CABACBufSize = 896 * (((prLAEDecPrm->u4PicH/16 + 1) >> 1) << 1) * prLAEDecPrm->u4NumColumnsMinus1;
            //HEVC level 5.0 max Num of Column tile = 10 specified in table A.1
            u4CABACBufSize = 896 * (((prLAEDecPrm->u4PicH/16 + 1) >> 1) << 1) * 10;
        }
        else
        {
            LOG(0, "codec not support LAE\n");
            return FALSE;
        }
        u4CABACBufSize = ((u4CABACBufSize + 4095)/4096)*4096;
        
        u4ECBufSize = 32 * (prLAEDecPrm->u4PicH/16); 
        u4ECBufSize = ((u4ECBufSize + 4095)/4096)*4096;

        u4BgIdx = 0;
        for (u4BufIdx = 0; u4BufIdx < MAX_LAE_BUF_NS_PER_GROUP; u4BufIdx++)
        {
            u4Addr =  FBM_GetCabacMemAddr(prVdecEsInfo->ucFbgId,u4BufIdx);
            ASSERT(u4Addr != 0);
            _arLAEBufGroup[u4BgIdx][u4BufIdx].u4CabacAddr = u4Addr;
            u4Addr += u4CABACBufSize;
            _arLAEBufGroup[u4BgIdx][u4BufIdx].u4ECAddr = u4Addr;
            prLaeDecPrm = &(_arLAEBufGroup[u4BgIdx][u4BufIdx]);
            prLaeDecPrm->u4PicW = prLAEDecPrm->u4PicW;
            prLaeDecPrm->u4PicH = prLAEDecPrm->u4PicH;
            vPutBufferToLae(u4LaeInstID,prLaeDecPrm);
            LOG(2, "[LAE] bgidx %d, bufidx %d,cabac & ec %x,%x,PA %x,%x\n",
                u4BgIdx, u4BufIdx,
                _arLAEBufGroup[u4BgIdx][u4BufIdx].u4CabacAddr,
                _arLAEBufGroup[u4BgIdx][u4BufIdx].u4ECAddr,
                PHYSICAL(_arLAEBufGroup[u4BgIdx][u4BufIdx].u4CabacAddr),
                PHYSICAL(_arLAEBufGroup[u4BgIdx][u4BufIdx].u4ECAddr));
        }
		#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
		FBM_WorkBufPartMap(prVdecEsInfo->ucFbgId, _arLAEBufGroup[0][0].u4CabacAddr, 
			FBM_FBG_TYPE_CABAC_SIZE * MAX_LAE_BUF_NS_PER_GROUP);
		#endif
    }
    return TRUE;
}

VOID vPutBufferToLae(UINT32 u4InstID,VDEC_LAE_DecPrm *prLaeDecPrm)
{
    INT32 i4Ret;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    LOG(5,"%s (%d)\n",__FUNCTION__,u4InstID);
    i4Ret = x_msg_q_send(prVdecEsInfoKeep->hMsgQLaeInput, (void *)prLaeDecPrm,
                sizeof(VDEC_LAE_DecPrm), 255);
    if(i4Ret != OSR_OK)
    {
        LOG(0,"LAE put msgQ fail[%d]\n",i4Ret);
    }
}

VOID vPutBufferToMcore(UINT32 u4InstID,VDEC_LAE_DecPrm *prLAEDecPrm)
{
    INT32 i4Ret;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    LOG(4,"%s (%d) 0x%x 0x%x\n",__FUNCTION__,u4InstID,prLAEDecPrm->u4PesRdptr,prLAEDecPrm->u4CabacAddr);
    i4Ret = x_msg_q_send(prVdecEsInfoKeep->hMsgQLaeOuput, (void *)prLAEDecPrm,
                sizeof(VDEC_LAE_DecPrm), 255);
    if(i4Ret != OSR_OK)
    {
        LOG(0,"LAE put msgQ fail[%d]\n",i4Ret);
    }
}


//Get LAE buffer for LAE decode
UCHAR ucGetBufferForLaeDec(UINT32 u4InstID,VDEC_LAE_DecPrm *prLAEDecPrm)
{
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    zMsgSize = sizeof(VDEC_LAE_DecPrm);
    i4Ret = x_msg_q_receive(&u2MsgQIdx, (void*)prLAEDecPrm, &zMsgSize,
            &(prVdecEsInfoKeep->hMsgQLaeInput), 1, X_MSGQ_OPTION_WAIT);
    if(i4Ret != OSR_OK)
    {
        LOG(0,"LAE get msgQ fail[%d]\n",i4Ret);
        return 0xFF;
    }
    LOG(5,"%s (%d)\n",__FUNCTION__,u4InstID);
    return 0;
}

UCHAR ucGetBufferForMcoreDec(UINT32 u4InstID,VDEC_LAE_DecPrm *prLAEDecPrm)
{
    INT32 i4Ret;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    zMsgSize = sizeof(VDEC_LAE_DecPrm);
    LOG(4,"%s (%d) 0x%x 0x%x\n",__FUNCTION__,u4InstID,prLAEDecPrm->u4PesRdptr,prLAEDecPrm->u4CabacAddr);
    i4Ret = x_msg_q_receive(&u2MsgQIdx, (void*)prLAEDecPrm, &zMsgSize,
            &(prVdecEsInfoKeep->hMsgQLaeOuput), 1, X_MSGQ_OPTION_WAIT);
    if(i4Ret != OSR_OK)
    {
        LOG(0,"LAE get msgQ fail[%d]\n",i4Ret);
        return 0xFF;
    }

    return 0;
}

//clear LAE Q before resolution change
void vClearLaeInputMsgQ(UINT32 u4InstID)
{
    INT32 i4MsgCnt = 0;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    VDEC_LAE_DecPrm rLAEDecPrm;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(u4InstID);
    zMsgSize = sizeof(VDEC_LAE_DecPrm);

    while (x_msg_q_receive(&u2MsgQIdx, (void*)&rLAEDecPrm, &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
        i4MsgCnt ++;
    }
    LOG(2,"Clear LAE MsgQ done %d\n",i4MsgCnt);
}

/*void vLAEPartitionDPB(VDEC_AVC_DecPrm* prDecPrm)
{
    INT32 i, j, k;
    UINT32 u4DramPicSize, u4DramPicArea, u4YLen, u4CLen;
    UINT32 u4DPBSA;

    if (u4DPBSA == 0)
    {
        u4DPBSA = (UINT32)BSP_AllocAlignedDmaMemory(16*1024,1024);
    }
    
    prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = prDecPrm->prSPS->u4NumRefFrames + 1;
    k = prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
    u4DramPicSize = 1024;
    u4DramPicArea = ((((u4DramPicSize * 7) >> 2) + 511) >> 9) << 9;
    u4YLen = (u4DramPicSize >> 8);
    u4CLen = (u4YLen >> 1);
    
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
        }
        prDecPrm->arFBufInfo[j][0].u4YAddr =  u4DPBSA;
        prDecPrm->arFBufInfo[j][0].u4YLenStartAddr = prDecPrm->arFBufInfo[j][0].u4YAddr + u4DramPicSize;
        prDecPrm->arFBufInfo[j][0].u4YLenOffset = u4DramPicSize;
        prDecPrm->arFBufInfo[j][0].u4YLen = u4YLen;
        for (i = 1; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
        {
            prDecPrm->arFBufInfo[j][i].u4YAddr = ((((prDecPrm->arFBufInfo[j][i-1].u4YLenStartAddr + u4YLen) + 8191) >> 13) << 13);
            prDecPrm->arFBufInfo[j][i].u4YLenStartAddr = prDecPrm->arFBufInfo[j][i].u4YAddr + u4DramPicSize;
            prDecPrm->arFBufInfo[j][i].u4YLenOffset = u4DramPicSize;
            prDecPrm->arFBufInfo[j][i].u4YLen = u4YLen;
        }
        prDecPrm->arFBufInfo[j][0].u4CAddr = ((((prDecPrm->arFBufInfo[j][k-1].u4YLenStartAddr + u4YLen) + 8191) >> 13) << 13);
        prDecPrm->arFBufInfo[j][0].u4CLenStartAddr = prDecPrm->arFBufInfo[j][0].u4CAddr + (u4DramPicSize >> 1);
        prDecPrm->arFBufInfo[j][0].u4CLenOffset = (u4DramPicSize >> 1);
        prDecPrm->arFBufInfo[j][0].u4CLen = u4CLen;
        for (i = 1; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
        {
            prDecPrm->arFBufInfo[j][0].u4CAddr = ((((prDecPrm->arFBufInfo[j][i-1].u4CLenStartAddr + u4CLen) + 8191) >> 13) << 13);
            prDecPrm->arFBufInfo[j][0].u4CLenStartAddr = prDecPrm->arFBufInfo[j][i].u4CAddr + (u4DramPicSize >> 1);
            prDecPrm->arFBufInfo[j][0].u4CLenOffset = (u4DramPicSize >> 1);
            prDecPrm->arFBufInfo[j][0].u4CLen = u4CLen;
        }        
    }
    
    // current reset to 0 when DPB partition.
    prDecPrm->prCurrFBufInfo = &prDecPrm->arFBufInfo[0][0];
}

void vLAEAllocateFBuf(VDEC_AVC_DecPrm* prDecPrm)
{
    INT32 i;
    INT32 iMinPOC;
    UINT32 u4MinPOCFBufIdx = 0;

    // Check if DPB full
    iMinPOC = 0x7fffffff;
    for(i = 0; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
    {
        if(prDecPrm->prCurrFBufInfo->u1FBufStatus == NO_PIC)
        {
            iMinPOC = 0x7fffffff;
            u4MinPOCFBufIdx = i;        
            break;
        }
        // miew: need to take care of field empty
        else if((iMinPOC > prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC) && 
            (fgIsNonRefFBuf(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1FBufRefType, 
                                    prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1TFldRefType,
                                 prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1BFldRefType) || prDecPrm->fgPOCWrap))
        {
            iMinPOC = prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC;
            u4MinPOCFBufIdx = i;
        }
    }  
    // No empty DPB, 1 FBuf output
    if(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][u4MinPOCFBufIdx].u1FBufStatus != NO_PIC)
    {
        _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + ((UINT32)prDecPrm->u1CurViewIdx<<8));
    }
    prDecPrm->u1DecFBufIdx = u4MinPOCFBufIdx;
    // Only new alloc needs to update current fbuf idx
    //SetCurrFBufIdx(u4InstID, _tVerMpvDecPrm[u4InstID].ucDecFBufIdx);    
}

void vLAEPrepareFBufInfo(UINT32 u4InstID, VDEC_AVC_DecPrm* prDecPrm)
{
    prDecPrm->u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prDecPrm->u4PicH = (2 - prDecPrm->prSPS->fgFrameMbsOnlyFlag)*(prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) << 4; //32x
    prDecPrm->u4RealPicH = prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 << 4;  // original real size

    if((prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastPicW != prDecPrm->u4PicW) 
        || (prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastPicH != prDecPrm->u4PicH))
    {
        vLAEPartitionDPB(prDecPrm);      
    }

    if(fgIsIDRPic(u4InstID))
    {
        prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum = 0xffffffff;
        if(prDecPrm->prSliceHdr->fgNoOutputOfPriorPicsFlag)
        {
            _VDEC_H264Flush(u4InstID, FALSE);
        }
        else
        {
            _VDEC_H264Flush(u4InstID, TRUE);
        }
    }
    // Find a empty fbuf 
    if((prDecPrm->prCurrFBufInfo->u1FBufStatus == NO_PIC)
        || (prDecPrm->prCurrFBufInfo->u1FBufStatus & prDecPrm->u1PicStruct))
    {
        vLAEAllocateFBuf(prDecPrm);   
    }
    prDecPrm->prCurrFBufInfo->u1FBufStatus |= prDecPrm->u1PicStruct;
    if (prDecPrm->u1PicStruct & TOP_FIELD)
    {
        prDecPrm->prCurrFBufInfo->u4TFldPara = (((UINT32)fgIsFrmPic(prDecPrm->u1PicStruct) ? 0 : 1) << 19) +
                                               ((UINT32)(fgIsFrmPic(prDecPrm->u1PicStruct) && prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag) << 18);
    }
    if (prDecPrm->u1PicStruct & BOTTOM_FIELD)
    {
        prDecPrm->prCurrFBufInfo->u4BFldPara = (((UINT32)fgIsFrmPic(prDecPrm->u1PicStruct) ? 0 : 1) << 19) +
                                               ((UINT32)(fgIsFrmPic(prDecPrm->u1PicStruct) && prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag) << 18);
    }
}*/

