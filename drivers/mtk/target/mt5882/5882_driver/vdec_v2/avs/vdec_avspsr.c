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
 * $RCSfile: vdec_avspsr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_avspsr.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "vdec_avsdec.h"
#include "vdec_avsutil.h"
#include "vdec_avshw.h"
#include "fbm_drvif.h"
#include "x_timer.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//Error handling
#define CHKPARAM(u4Param, u4HBound)\
    if(u4Param > u4HBound)\
    {\
        if(prDecPrm->prCurrFBufInfo)\
        {\
            if(FBM_GetFrameBufferStatus(prDecPrm->ucFbgId, prDecPrm->prCurrFBufInfo->ucFbId) == FBM_FB_STATUS_DECODE)\
            {\
                FBM_SetFrameBufferStatus(prDecPrm->ucFbgId, prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_EMPTY);\
                _VDEC_ClrAvsFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx, FALSE);\
            }\
        }\
        LOG(2, "Hdr err\n");\
        *pi4RetValue = E_VDEC_FAIL;\
        return;\
    }

#define CHKPARAM_R(i4Param, i4LBound, i4HBound)\
    if((i4Param < i4LBound) || (i4Param > i4HBound))\
    {\
        if(prDecPrm->prCurrFBufInfo)\
        {\
            if(FBM_GetFrameBufferStatus(prDecPrm->ucFbgId, prDecPrm->prCurrFBufInfo->ucFbId) == FBM_FB_STATUS_DECODE)\
            {\
                FBM_SetFrameBufferStatus(prDecPrm->ucFbgId, prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_EMPTY);\
                _VDEC_ClrAvsFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx, FALSE);\
            }\
        }\
        LOG(2, "Hdr err\n");\
        *pi4RetValue = E_VDEC_FAIL;\
        return;\
    }

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
extern void FBM_UpdateReferenceList(UCHAR ucFbgId, UCHAR ucFbId, BOOL fgReference);


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
INT32 _VDEC_AvsChkSignedParam (UINT32 u4Value);

// *********************************************************************
// Function    : void _VDEC_AvsSeqHdr(void)
// Description : Handle sequence header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_AvsSeqHdr(UCHAR ucEsId,VDEC_AVS_INFO_T* prVdec, INT32* pi4RetValue)
{
    VDEC_AVS_SEQ_T rSeqHdr = {0};
    VDEC_AVS_SEQ_T* prSeqHdr;
    VDEC_AVS_DECPRM_T* prDecPrm;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T *prVdecEsInfo;
    #ifdef CC_MAPLE_CUST_DRV
    ENUM_CODEC_INFO_T eCodecType;
    #endif
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    ASSERT(prVdec != NULL);
    ASSERT(pi4RetValue != NULL);
    ASSERT(prVdecInfo != NULL);
    ASSERT(prVdecEsInfo != NULL);

    prSeqHdr = &prVdec->rSeqHdr;
    prDecPrm = &prVdec->rDecParam;
    //x_memset((void*)prSeqHdr, 0, sizeof(prSeqHdr));

    prDecPrm->u4BitCnt = 0;

    LOG(5, "ES(%d) ==== Avs Seq Hdr ====\n",ucEsId);
    // Shift 0x000001 for read pointer
    _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 32);

    rSeqHdr.u4ProfileID = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);
    rSeqHdr.u4LevelID = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);

    if(rSeqHdr.u4ProfileID == GUANGBO)
    {
#ifdef VDEC_SUPPORT_AVSPLUS
        LOG(3,"This is AVS Plus Bitstream\n");
#else
        *pi4RetValue = (INT32)E_VDEC_NOT_SUPPORT;
        LOG(3,"AVS Profile Not Support\n");
        return;
#endif
    }

#if 0
    if ((!fgChkProfile((UINT8)prSeqHdr->u4ProfileID)) || (!fgChkLevel((UINT8)prSeqHdr->u4LevelID)))
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(2, "err at SeqHdr Profile/Level\n");
        return;
    }
#endif
    rSeqHdr.u4IsProgSeq = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    rSeqHdr.u4HSize = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 14);
    rSeqHdr.u4VSize = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 14);
    LOG(5, "ES(%d) prSeqHdr->u4IsProgSeq = %d, H size = %d, V size = %d\n",ucEsId,
        rSeqHdr.u4IsProgSeq,
        rSeqHdr.u4HSize,
        rSeqHdr.u4VSize);
#ifndef CC_AVS_MODIFICATION_FOR_HIGH
    if (((rSeqHdr.u4HSize == 0) || (rSeqHdr.u4VSize == 0)) ||
#if defined( CC_FBM_FULL_3D_SUPPORT) || defined (CC_SUPPORT_4K2K)
            ((rSeqHdr.u4HSize > 3840) || (rSeqHdr.u4VSize > 2160)))
#else
            ((rSeqHdr.u4HSize > 1920) || (rSeqHdr.u4VSize > 1088)))
#endif
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(2, "ES(%d) err at SeqHdr HSize/VSize\n",ucEsId);
        return;
    }
#endif

    rSeqHdr.u4ChromaFmt = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
    rSeqHdr.u4SamplePrec = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 3);
    CHKPARAM_R((INT32)rSeqHdr.u4SamplePrec, 0, 1);
    rSeqHdr.u4AspRatio = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 4);
    CHKPARAM_R((INT32)rSeqHdr.u4AspRatio, 0, 15);
    if((rSeqHdr.u4AspRatio > 4) || (rSeqHdr.u4AspRatio ==0))
    {
        LOG(0,"ES(%d) Error AspRatio %d in AVS bitstream \n",ucEsId,rSeqHdr.u4AspRatio);
        rSeqHdr.u4AspRatio = 3;
    }
    rSeqHdr.u4FrmRate = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 4);
    CHKPARAM_R((INT32)rSeqHdr.u4FrmRate, 0, 8);

    if((prDecPrm->u4LastAspRatio != rSeqHdr.u4AspRatio) || (prDecPrm->u4LastFrmRate != rSeqHdr.u4FrmRate))
    {
        prDecPrm->u4LastAspRatio = rSeqHdr.u4AspRatio;
        prDecPrm->u4LastFrmRate = rSeqHdr.u4FrmRate;
        prDecPrm->fgAvsSeqChg = TRUE;
        LOG(5, "AVS Aspratio Framerate change!\n");
    }

    // report sequence hdr
    if (prVdecInfo->pfPicTypeCb)
    {
        VDEC_PICTYPE_CB_T rPicType;
        x_memset(&rPicType, 0, sizeof(VDEC_PICTYPE_CB_T));
        rPicType.u1EsId = ucEsId;
        rPicType.u4Pts = prVdecEsInfo->u4PTS;
        rPicType.u1PicType = (UINT8)VDEC_PIC_TYPE_SEQ;
        prVdecInfo->pfPicTypeCb(&rPicType);
    }
    LOG(5, "ES(%d) prSeqHdr->u4AspRatio = %d, prSeqHdr->u4FrmRate = %d\n",ucEsId,
        rSeqHdr.u4AspRatio,
        rSeqHdr.u4FrmRate);
    rSeqHdr.u4BitRateL = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 18);
    rSeqHdr.u4MarketBitBR = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    rSeqHdr.u4BitRateU = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 12);
    rSeqHdr.u4LowDelay = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    rSeqHdr.u4MarkerBit = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    rSeqHdr.u4BBVSize = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 18);
    rSeqHdr.u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 3);

    rSeqHdr.u4IsValid = 1;

    x_memcpy((void*)prSeqHdr, &rSeqHdr, sizeof(VDEC_AVS_SEQ_T));

    if ((prDecPrm->u4LastHSize != prVdec->rSeqHdr.u4HSize) ||
            (prDecPrm->u4LastVSize != prVdec->rSeqHdr.u4VSize) ||
            (prDecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN))
    {
        UINT8 u1FbgType;
        //UINT32 u4FbWidth = 0, u4FbHeight = 0;
        FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
        
        VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

        FBM_ReleaseGroup(prDecPrm->ucFbgId);
        _VDEC_FlushAvsDPB(prVdec);

        prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

        u1FbgType = FBM_SelectGroupType(prVdec->rSeqHdr.u4HSize , prVdec->rSeqHdr.u4VSize);

        prDecPrm->ucFbgType = u1FbgType;

        // Should add FBM_VDEC_AVS
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

            prDecPrm->ucFbgId =
                FBM_CreateGroupExt(u1FbgType, FBM_VDEC_AVS,
                    prVdec->rSeqHdr.u4HSize, prVdec->rSeqHdr.u4VSize, &rPar);
        }
        if (prDecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            LOG(3, "ES(%d) Fail to create FBM group type\n",ucEsId);
            return;
        }
        else
        {
            LOG(3, "ES(%d) FBGID = %d\n",ucEsId,prDecPrm->ucFbgId);
        }

        prVdecEsInfo->ucFbgId = prDecPrm->ucFbgId;
        if(prVdecEsInfo->fgMMPlayback)
        {
            FBM_SetPlayMode(prVdecEsInfo->ucFbgId, FBM_FBG_MM_MODE);
        }
        LOG(3, "ES(%d) fgVPush = %d, fgVPushDecodeOnly = %d\n",ucEsId, prVdecEsInfoKeep->fgVPush, prVdecEsInfoKeep->fgVPushDecodeOnly);       

        if(prVdecEsInfoKeep->fgVPush)
        {
            if(prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(prVdecEsInfo->ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
            }
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
            if(prVdecEsInfoKeep->fgVPushFBGFromInst)
            {
                FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
            }
        }

        if(prVdecEsInfo->fgMMPlayback==FALSE && prVdecInfo->pfPicInfo)
        {
            FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                FBM_CB_FUNC_FB_DISPLAY_START, (UINT32)_Vdec_FrameDisplayStart, ucEsId);
        }

        //FBM_GetFrameBufferSize(prDecPrm->ucFbgId, &u4FbWidth, &u4FbHeight);
        FBM_FbgChgNotify(prDecPrm->ucFbgId, ucEsId);

        FBM_SetSyncStc(prDecPrm->ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);

        prVdec->rDecParam.u4LineSize = VDEC_ALIGN_MASK(prVdec->rSeqHdr.u4HSize,63);

        // Check if reset is required
        if (FBM_ChkFrameBufferFlag(prDecPrm->ucFbgId, FBM_FLAG_RESET))
        {
            FBM_ResetGroup(prDecPrm->ucFbgId);
            FBM_ClrFrameBufferFlag(prDecPrm->ucFbgId, FBM_FLAG_RESET);
            FBM_ClrFrameBufferFlag(prDecPrm->ucFbgId, FBM_FLAG_NOT_READY);
        }

        prVdec->rDecParam.u4MaxFBufNum = FBM_GetFrameBufferNs(prDecPrm->ucFbgId);
        LOG(6, "ES(%d) u4MaxFBufNum = %d\n", ucEsId,prVdec->rDecParam.u4MaxFBufNum);

        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->ucFbgId);
        if (!prFbmSeqHdr)
        {
            LOG(3, "ES(%d) Fail to get FBM sequence header\n",ucEsId);
            return;
        }

        prFbmSeqHdr->u2HSize = (UINT16)prVdec->rSeqHdr.u4HSize;
        prFbmSeqHdr->u2VSize = (UINT16)prVdec->rSeqHdr.u4VSize;

        prFbmSeqHdr->u2OrgHSize = (UINT16)prVdec->rSeqHdr.u4HSize;
        prFbmSeqHdr->u2OrgVSize = (UINT16)prVdec->rSeqHdr.u4VSize;

        prFbmSeqHdr->u2LineSize = (UINT16)(prVdec->rDecParam.u4LineSize);

        prFbmSeqHdr->fgProgressiveSeq = (BOOL)prVdec->rSeqHdr.u4IsProgSeq;

        prDecPrm->u4LastHSize = prVdec->rSeqHdr.u4HSize;
        prDecPrm->u4LastVSize = prVdec->rSeqHdr.u4VSize;

        prDecPrm->fgAvsSeqChg = TRUE;

        // ToDo (aspect ratio)
        switch (prVdec->rSeqHdr.u4AspRatio)
        {
            case 1:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                LOG(5, "ES(%d) AspRatio -> 1:1\n",ucEsId);
                break;
            case 2:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_4_3;
                LOG(5, "ES(%d) AspRatio -> 4:3\n",ucEsId);
                break;
            case 3:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;
                LOG(5, "ES(%d) AspRatio -> 16:9\n",ucEsId);
                break;
            case 4:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_221_1;
                LOG(5, "ES(%d) AspRatio -> 2.21:1\n",ucEsId);
                break;
            default:
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_4_3;
                LOG(5, "ES(%d) Unknown aspect ratio\n",ucEsId);
                break;
        }

        switch (prVdec->rSeqHdr.u4FrmRate)
        {
            case 1:
                LOG(5, "ES(%d) AVS Frame rate is 23.976\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
                prVdec->rDecParam.u4FrmRate = 23976;
                break;
            case 2:
                LOG(5, "ES(%d) AVS Frame rate is 24\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
                prVdec->rDecParam.u4FrmRate = 24000;
                break;
            case 3:
                LOG(5, "ES(%d) AVS Frame rate is 25\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
                prVdec->rDecParam.u4FrmRate = 25000;
                break;
            case 4:
                LOG(5, "ES(%d) AVS Frame rate is 29.97\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                prVdec->rDecParam.u4FrmRate = 29970;
                break;
            case 5:
                LOG(5, "ES(%d) AVS Frame rate is 30\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
                prVdec->rDecParam.u4FrmRate = 30000;
                break;
            case 6:
                LOG(5, "ES(%d) AVS Frame rate is 50\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
                prVdec->rDecParam.u4FrmRate = 50000;
                break;
            case 7:
                LOG(5, "ES(%d) AVS Frame rate is 59.94\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
                prVdec->rDecParam.u4FrmRate = 59940;
                break;
            case 8:
                LOG(5, "ES(%d) AVS Frame rate is 60\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                prVdec->rDecParam.u4FrmRate = 60000;
                break;
            default:
                LOG(3, "ES(%d) AVS Unknown Frame rate\n",ucEsId);
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
                prVdec->rDecParam.u4FrmRate = 60000;
                break;
        }
         #ifdef CC_MAPLE_CUST_DRV
	     eCodecType = E_VDEC_CODEC_AVS;
		 FBM_SetCodecInfo(prDecPrm->ucFbgId,eCodecType);
		 #endif
    }

    *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
}


// *********************************************************************
// Function    : void _VDEC_AvsExtSeqDispHdr(void)
// Description : Handle sequence display extension header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_AvsExtSeqDispHdr(UCHAR ucEsId, VDEC_AVS_INFO_T* prVdec, INT32* pi4RetValue)
{
    VDEC_AVS_EXT_T* prExtHdr;
#if ( (AVS_LITI_PROFILE == AVS_LITI_PROFILE_1) || (AVS_LITI_PROFILE == AVS_LITI_PROFILE_2) )
    VDEC_AVS_SEQ_T* prSeqHdr;
#endif
    VDEC_AVS_DECPRM_T* prDecPrm;
#if (AVS_LITI_PROFILE == AVS_LITI_PROFILE_3)
    VDEC_ES_INFO_T* prVdecEsInfo;
#endif

    ASSERT(prVdec != NULL);
    ASSERT(pi4RetValue != NULL);

#if (AVS_LITI_PROFILE == AVS_LITI_PROFILE_3)
     prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
#endif

    prExtHdr = &prVdec->rExtHdr;
#if ( (AVS_LITI_PROFILE == AVS_LITI_PROFILE_1) || (AVS_LITI_PROFILE == AVS_LITI_PROFILE_2) )
    prSeqHdr = &prVdec->rSeqHdr;
#endif
    prDecPrm = &prVdec->rDecParam;

    prDecPrm->u4BitCnt = 0;

    LOG(5, "ES(%d) ==== Avs Extension Seq Disp Hdr ====\n",ucEsId);

#if 0 //read in _VDEC_AvsExtHdr
    prExtHdr->rExtSeqDispHdr.u4ExtSeqDispID = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 4);
#endif
    prExtHdr->rExtSeqDispHdr.u4VideoFmt = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 3);
    prExtHdr->rExtSeqDispHdr.u4SampleRange = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    prExtHdr->rExtSeqDispHdr.u4ColorDescription = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    if ( prExtHdr->rExtSeqDispHdr.u4ColorDescription )
    {
        prExtHdr->rExtSeqDispHdr.u4ColorPrimaries = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);
        prExtHdr->rExtSeqDispHdr.u4TransCharacter = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);
        prExtHdr->rExtSeqDispHdr.u4MatrixCoef = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);
    }
    prExtHdr->rExtSeqDispHdr.u4DispHSize = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 14);
    prExtHdr->rExtSeqDispHdr.u4MarkerBit = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    prExtHdr->rExtSeqDispHdr.u4DispVSize = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 14);
#if (AVS_LITI_PROFILE == AVS_LITI_PROFILE_1)
    if ( prSeqHdr->u4ProfileID == JIZHUN_X2 )
    {
        prExtHdr->rExtSeqDispHdr.u4RsvBits = 0x0;
        prExtHdr->rExtSeqDispHdr.u4StereoPackingMode = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
    }
    else
    {
        prExtHdr->rExtSeqDispHdr.u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
        prExtHdr->rExtSeqDispHdr.u4StereoPackingMode = 0x0;
    }
#elif (AVS_LITI_PROFILE == AVS_LITI_PROFILE_2)
    if ( prSeqHdr->u4ProfileID == JIZHUN_X2 || prSeqHdr->u4ProfileID == SHENZHAN_X2 || prSeqHdr->u4ProfileID == YIDONG_X2 || prSeqHdr->u4ProfileID == ZENGQIANG_X2 )
    {
        prExtHdr->rExtSeqDispHdr.u4RsvBits = 0x0;
        prExtHdr->rExtSeqDispHdr.u4StereoPackingMode = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
    }
    else
    {
        prExtHdr->rExtSeqDispHdr.u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
        prExtHdr->rExtSeqDispHdr.u4StereoPackingMode = 0x0;
    }
#elif (AVS_LITI_PROFILE == AVS_LITI_PROFILE_3)
    prExtHdr->rExtSeqDispHdr.u4StereoPackingMode = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
    if ( prExtHdr->rExtSeqDispHdr.u4StereoPackingMode == STEREO_PACKING_HORIZONTAL )
    {
        prVdecEsInfo->e3DType = VDEC_3D_SBS_LF;
    }
    else if ( prExtHdr->rExtSeqDispHdr.u4StereoPackingMode == STEREO_PACKING_VERTICAL )
    {
        prVdecEsInfo->e3DType = VDEC_3D_TB_LF;
    }
    else
    {
        // Do nothing now
    }
#else //#if (AVS_LITI_PROFILE == AVS_LITI_PROFILE_NONE)
    prExtHdr->rExtSeqDispHdr.u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
#endif
}


// *********************************************************************
// Function    : void _VDEC_AvsExtHdr(void)
// Description : Handle extension header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_AvsExtHdr(UCHAR ucEsId, VDEC_AVS_INFO_T* prVdec, INT32* pi4RetValue)
{
    VDEC_AVS_EXT_T* prExtHdr;
    VDEC_AVS_SEQ_T* prSeqHdr;
    VDEC_AVS_DECPRM_T* prDecPrm;

    ASSERT(prVdec != NULL);
    ASSERT(pi4RetValue != NULL);

    prSeqHdr = &prVdec->rSeqHdr;

    if (!prSeqHdr->u4IsValid)
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(2, "SeqHdr !valid in ExtHdr\n");
        return;
    }

    prExtHdr = &prVdec->rExtHdr;
    prDecPrm = &prVdec->rDecParam;

    x_memset((void*)prExtHdr, 0, sizeof(VDEC_AVS_EXT_T));

    prDecPrm->u4BitCnt = 0;

    LOG(5, "==== Avs Extension Hdr ====\n");

    // Shift 0x000001 for read pointer
    _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 32);

    prExtHdr->u4ExtID = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 4);

    if ( prDecPrm->ucLastUnitType == SEQ_HDR_SC || prDecPrm->ucLastUnitType == I_PICTURE_SC || prDecPrm->ucLastUnitType == PB_PICTURE_SC
            || prDecPrm->ucLastUnitType == USER_DATA_SC
       )
    {
        switch ( prExtHdr->u4ExtID )
        {
            case EXT_SEQ_DISP:
                if ( prDecPrm->fgAfterSeqHdrBeforePicHdr == FALSE ) //Should be after Sequence Header
                {
                    *pi4RetValue = (INT32)E_VDEC_FAIL;
                    LOG(2, "err at ExtHdr EXT_SEQ_DISP\n");
                    return;
                }
                _VDEC_AvsExtSeqDispHdr(ucEsId, prVdec, pi4RetValue);
                break;

            case EXT_COPYRIGHT:
                break;

            case EXT_PIC_DISP:
                if ( prDecPrm->fgAfterSeqHdrBeforePicHdr == TRUE ) //Should be after Picture Header
                {
                    *pi4RetValue = (INT32)E_VDEC_FAIL;
                    LOG(2, "err at ExtHdr EXT_PIC_DISP\n");
                    return;
                }
                break;

            case EXT_CAM_PARA:
                break;

            default:
                break;
        }
    }
    else
    {
        //Should not occurs
        LOG(3, "Wrong position of extension data\n");
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        return;
    }

    *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
}


// *********************************************************************
// Function    : void _VDEC_AvsPicHdr(void)
// Description : Handle picture header
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_AvsPicHdr(UCHAR ucEsId,VDEC_AVS_INFO_T* prVdec, INT32* pi4RetValue)
{
    BOOL bIsIFrm;
    UINT32 u4Temp = 0xFFFFFFFF;

    UINT32 u4Rptr, u4Wptr;

    VDEC_AVS_SEQ_T* prSeqHdr;
    VDEC_AVS_PIC_T* prPicHdr;
    VDEC_AVS_DECPRM_T* prDecPrm;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_INFO_T* prVdecInfo;

    UINT32 u4PrevByteCount = 0, u4CurByteCount = 0;
    UINT32 u4Timer = 0;
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    ASSERT(prVdec != NULL);
    ASSERT(pi4RetValue != NULL);
    ASSERT(prVdecInfo != NULL);
    ASSERT(prVdecEsInfo != NULL);

    // Turn On Bit Stuffing Check
    AVS_VLD_WRITE32(RW_AVS_VLD_BITSTUFFING_SWITCH, 1);

    prSeqHdr = &prVdec->rSeqHdr;
    if (!prSeqHdr->u4IsValid)
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(2, "ES(%d) SeqHdr !valid in PicHdr\n",ucEsId);
        return;
    }
    prPicHdr = &prVdec->rPicHdr;
    x_memset((void*)prPicHdr, 0, sizeof(VDEC_AVS_PIC_T));

    prDecPrm = &prVdec->rDecParam;

    // Reset bit count
    prDecPrm->u4BitCnt = 0;

    // Shift 0x000001 for read pointer
    _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 32);

    bIsIFrm = (prVdec->rPesInfo.ucPicType == I_PICTURE_SC) ? TRUE : FALSE;

    prPicHdr->u4BBVDelay = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 16);

    if(prSeqHdr->u4ProfileID == GUANGBO)
    {
        prPicHdr->u4MarkerBit0 = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        prPicHdr->u4BBVDelayExternsion = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 7);
        
        prPicHdr->u4BBVDelay <<= 7;
        prPicHdr->u4BBVDelay |= prPicHdr->u4BBVDelayExternsion;
    }

    if (bIsIFrm)
    {
        prPicHdr->u4TimeCodeFg = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        if (prPicHdr->u4TimeCodeFg)
        {
            prPicHdr->u4TimeCode = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 24);
        }
        prPicHdr->u4MarkerBit = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        prPicHdr->u4PicCodingType = I_IMG;

        // Record the offset of I
        prDecPrm->u8OffsetI = prDecPrm->u8Offset;
        prDecPrm->i4TemporalRef = 0;
    }
    else
    {
        prPicHdr->u4PicCodingType = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
        CHKPARAM_R(prPicHdr->u4PicCodingType, 1, 2);

        if (prPicHdr->u4PicCodingType == P_IMG)
        {
            prDecPrm->i4TemporalRef++;
        }
    }

    LOG(6, "ES(%d) AVS coding type = %d\n",ucEsId, prPicHdr->u4PicCodingType);
    // report pic type
    if (prVdecInfo->pfPicTypeCb)
    {
        BOOL fgNfy = FALSE;
        VDEC_PICTYPE_CB_T rPicType;
        x_memset(&rPicType, 0, sizeof(VDEC_PICTYPE_CB_T));
        rPicType.u1EsId = ucEsId;
        rPicType.u4Pts = prVdecEsInfo->u4PTS;
        
        if(prPicHdr->u4PicCodingType == B_IMG)
        {
            rPicType.u1PicType = VDEC_PIC_TYPE_B;
            fgNfy = TRUE;
        }
        else if(prPicHdr->u4PicCodingType == P_IMG)
        {
            rPicType.u1PicType = VDEC_PIC_TYPE_P;
            fgNfy = TRUE;
        }
        else if(prPicHdr->u4PicCodingType == I_IMG)
        {
            rPicType.u1PicType = VDEC_PIC_TYPE_I;
            fgNfy = TRUE;
        }
        if(fgNfy)
        {
            prVdecInfo->pfPicTypeCb(&rPicType);
        }
    }


    LOG(6, "ES(%d) AVS coding type = %d\n",ucEsId, prPicHdr->u4PicCodingType);

#ifdef CC_AVS_EMULATION
    if ( prPicHdr->u4PicCodingType == I_IMG )
    {
        LOG(0, "ES(%d) AVS Coding Type is I_IMG \n",ucEsId);
    }
    if ( prPicHdr->u4PicCodingType == B_IMG )
    {
        LOG(0, "ES(%d) AVS Coding Type is B_IMG \n",ucEsId);
    }
    if ( prPicHdr->u4PicCodingType == P_IMG )
    {
        LOG(0, "ES(%d) AVS Coding Type is P_IMG \n",ucEsId);
    }
#endif

    if (!prDecPrm->fgFirstI)
    {
        if (prPicHdr->u4PicCodingType != I_IMG)
        {
            LOG(3, "ES(%d) Wait for the first I frame\n",ucEsId);
            *pi4RetValue = (INT32)E_VDEC_FAIL;
            return;
        }
        else
        {
            prDecPrm->fgFirstI = TRUE;
            CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        }
    }

    if ((prPicHdr->u4PicCodingType == B_IMG) &&
            (prDecPrm->u1LastRefIdx < 1))
    {
        LOG(3, "ES(%d) Not enough ref frame buf for B\n",ucEsId);
#ifdef CC_AVS_EMULATION        
        *pi4RetValue = (INT32)E_VDEC_SKIPFRAME;
#else
        *pi4RetValue = (INT32)E_VDEC_FAIL;
#endif
        return;
    }


    prPicHdr->u4PicDistance = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 8);
    if (prSeqHdr->u4LowDelay)
    {
        //bbv_check_times
        prPicHdr->u4BBVCheckTimes = _VDEC_AvsUeCodeNum();
    }
    prPicHdr->u4ProgFrm = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    if (!prPicHdr->u4ProgFrm)
    {
        prPicHdr->u4PicStruct = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        if (!prPicHdr->u4PicStruct && !bIsIFrm)
        {
            prPicHdr->u4AdvPredModeDisable = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        }
        else
        {
            prPicHdr->u4AdvPredModeDisable = 1;
        }

        if (prPicHdr->u4PicStruct)
        {
            prPicHdr->u4ProgFrm = TRUE;
        }
    }
    else
    {
        prPicHdr->u4PicStruct = FRM_ENCODE;
        prPicHdr->u4AdvPredModeDisable = 1;
    }

    prPicHdr->u4TFT = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    prPicHdr->u4RFF = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    prPicHdr->u4FixedPicQP = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    prPicHdr->u4PicQP = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 6);

    if (!bIsIFrm)
    {
        if (!((prPicHdr->u4PicCodingType == B_IMG) &&
                (prPicHdr->u4PicStruct == FRM_ENCODE) &&
                (prPicHdr->u4AdvPredModeDisable)))
        {
            prPicHdr->u4PicRefFg = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        }
        prPicHdr->u4NoForwardRefFg = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        prPicHdr->u4PBFieldEnhancedFlag = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        prPicHdr->u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
    }

    if ((!prPicHdr->u4ProgFrm && !prPicHdr->u4PicStruct) || (!bIsIFrm))
    {
        prPicHdr->u4SkipModeFg = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    }
    if (bIsIFrm)
    {
        prPicHdr->u4RsvBits = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 4);
    }



    // loop filter
    prPicHdr->u4LoopFilterDisable = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
#ifdef CC_DUMP_AVS_RISC_SETTING
    if (prDecPrm->fgDumpSetting)
    {
        Printf("RISCWrite(`AVS_VLD_BASE + 49*4, 32'h1);\n");
        Printf("AVS_VLD_SHIFT_BIT(%d);\n", prDecPrm->u4BitCnt);
    }
#endif
    if (!prPicHdr->u4LoopFilterDisable)
    {
        prPicHdr->u4LoopFilterParamFg = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
#ifdef CC_DUMP_AVS_RISC_SETTING
        if (prDecPrm->fgDumpSetting)
        {
            Printf("AVS_VLD_SHIFT_BIT(1);\n");
        }
#endif
        if (prPicHdr->u4LoopFilterParamFg)
        {
            UINT32 u4Temp = 0;

            u4Temp = _VDEC_AvsSeCodeNum();
            prPicHdr->i4AlphaCOffset = _VDEC_AvsChkSignedParam(u4Temp);
            //prPicHdr->i4AlphaCOffset = _VDEC_AvsSeCodeNum();
            CHKPARAM_R(prPicHdr->i4AlphaCOffset, -8, 8);

            u4Temp = _VDEC_AvsSeCodeNum();
            prPicHdr->i4BetaOffset = _VDEC_AvsChkSignedParam(u4Temp);
            //prPicHdr->i4BetaOffset = _VDEC_AvsSeCodeNum();
#ifdef CC_DUMP_AVS_RISC_SETTING
            if (prDecPrm->fgDumpSetting)
            {
                Printf("RISCRead_VDEC(`AVS_VLD_BASE + %d*4);\n", RO_AVS_VLD_SE >> 2);
                Printf("RISCRead_VDEC(`AVS_VLD_BASE + %d*4);\n", RO_AVS_VLD_SE >> 2);
            }
#endif
            CHKPARAM_R(prPicHdr->i4BetaOffset, -8, 8);
        }
    }

	
    if(prSeqHdr->u4ProfileID == GUANGBO)
    {
        UINT32 u4Index = 0;

        prPicHdr->u4WeightingQuantFlag = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
        if(prPicHdr->u4WeightingQuantFlag == 1)
        {
            prPicHdr->u4MbAdaptWeightingQuantDisable = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
            prPicHdr->u4ChromaQuantParamDisable = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
            if(prPicHdr->u4ChromaQuantParamDisable == 0)
            {
                UINT32 u4Temp = 0;
                
                u4Temp = _VDEC_AvsSeCodeNum();
                prPicHdr->i4ChromaQuantParamDeltaCb = _VDEC_AvsChkSignedParam(u4Temp);
                CHKPARAM_R(prPicHdr->i4ChromaQuantParamDeltaCb, -16, 16);
                
                u4Temp = _VDEC_AvsSeCodeNum();
                prPicHdr->i4ChromaQuantParamDeltaCr = _VDEC_AvsChkSignedParam(u4Temp);
                CHKPARAM_R(prPicHdr->i4ChromaQuantParamDeltaCr, -16, 16);
            }
            prPicHdr->u4WeightingQuantParamIndex = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2);
            prPicHdr->u4WeightingQuantModel = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 2) & 0x3;
            
            if((prPicHdr->u4WeightingQuantParamIndex == 1) ||
                ((prPicHdr->u4MbAdaptWeightingQuantDisable == 0) && (prPicHdr->u4WeightingQuantParamIndex == 3)))
            {
                for(u4Index=0; u4Index<6; u4Index++)
                {
                    UINT32 u4Temp = 0;
                    
                    u4Temp = _VDEC_AvsSeCodeNum();
                    prPicHdr->i4WeightingQuantParamDelta1[u4Index] = _VDEC_AvsChkSignedParam(u4Temp);
                    CHKPARAM_R(prPicHdr->i4WeightingQuantParamDelta1[u4Index], -128, 127);
                }
            }
            if((prPicHdr->u4WeightingQuantParamIndex == 2) ||
               ((prPicHdr->u4MbAdaptWeightingQuantDisable == 0) && (prPicHdr->u4WeightingQuantParamIndex == 3)))
            {
                for(u4Index=0; u4Index<6; u4Index++)
                {
                    UINT32 u4Temp = 0;
                    
                    u4Temp = _VDEC_AvsSeCodeNum();
                    prPicHdr->i4WeightingQuantParamDelta2[u4Index] = _VDEC_AvsChkSignedParam(u4Temp);
                    CHKPARAM_R(prPicHdr->i4WeightingQuantParamDelta2[u4Index], -128, 127);
                }
            }
        }
        prPicHdr->u4AceEnable = _VDEC_AVSGetRealShtBitStream(&prDecPrm->u4BitCnt, 1);
    }

    u4PrevByteCount = _VDEC_AvsGetByteCnt();
    //_VDEC_AVSAlign2Byte(&prDecPrm->u4BitCnt);
    AVS_VLD_WRITE32(RW_AVS_FIND_STARTCODE, 1);

    while (1)
    {
        if (AVS_VLD_READ32(RW_AVS_FIND_STARTCODE) == 0)
        {
            break;
        }
        else
        {
            u4CurByteCount = _VDEC_AvsGetByteCnt();
            if (u4PrevByteCount == u4CurByteCount)
            {
                u4Timer++;
            }
                    	
            if ((u4Timer == 100000) ||
                (u4CurByteCount > 0x100000))
            {
                *pi4RetValue = (INT32)E_VDEC_FAIL;
                return;
            }
            else
            {
                u4PrevByteCount = u4CurByteCount;
            }
        }
    }
    u4PrevByteCount = 0;
    u4CurByteCount = 0;
    u4Timer = 0;

#ifdef CC_DUMP_AVS_RISC_SETTING
    if (prDecPrm->fgDumpSetting)
    {
        Printf("RISCWrite(`AVS_VLD_BASE + 52*4, 32'h1);\n");
    }
#endif
    while (1)
    {
        u4Rptr = VLD_READ32(RO_VLD_VRPTR);
        u4Wptr = VLD_READ32(RO_VLD_VWPTR);
        while (AVAILABLE_SZ(u4Rptr, u4Wptr, ((VLD_READ32(RW_VLD_VEND) - VLD_READ32(RW_VLD_VSTART))<<6)) <= AVS_MIN_LEN)
        {
            LOG(3, "ES(%d) There is not enough data for start code search\n",ucEsId);
            *pi4RetValue = (INT32)E_VDEC_FAIL;
            break;
        }

#ifdef CC_DUMP_AVS_RISC_SETTING
        if (prDecPrm->fgDumpSetting)
        {
            Printf("RISCRead_VDEC(`AVS_VLD_BASE + 52*4);\n");
            Printf("while (data_out != 32'h0)\n");
            Printf("begin\n");
            Printf("@(posedge bclk);\n");
            Printf("RISCRead_VDEC(`AVS_VLD_BASE + 52*4);\n");
        }
#endif
        // Shift 0x000001 for read pointer
        u4Temp = _VDEC_AVSGetShtBitStream(&prDecPrm->u4BitCnt, 0);
        if (((u4Temp >> 8) == 0x000001) &&
                ((u4Temp & 0x000000FF) <= 0xAF))
        {
            break;
        }
        AVS_VLD_WRITE32(RW_AVS_FIND_NEXT_STARTCODE, 1);

        while (1)
        {
            if (AVS_VLD_READ32(RW_AVS_FIND_NEXT_STARTCODE) == 0)
            {
                break;
            }
            else
            {
                u4CurByteCount = _VDEC_AvsGetByteCnt();
                if (u4PrevByteCount == u4CurByteCount)
                {
                    u4Timer++;
                }
                    	
                if ((u4Timer == 100000) ||
                    (u4CurByteCount > 0x100000))
                {
                    *pi4RetValue = (INT32)E_VDEC_FAIL;
                    return;
                }
                else
                {
                    u4PrevByteCount = u4CurByteCount;
                }
            }
        }
        u4PrevByteCount = 0;
        u4CurByteCount = 0;
        u4Timer = 0;
#ifdef CC_DUMP_AVS_RISC_SETTING
        if (prDecPrm->fgDumpSetting)
        {
            Printf("RISCWrite(`AVS_VLD_BASE + 53*4, 32'h1)\n");
        }
#endif
    }

#ifdef CC_DUMP_AVS_RISC_SETTING
    if (prDecPrm->fgDumpSetting)
    {
        Printf("end\n");
    }
#endif
    //Printf("Decoder's Input Window: 0x%8x\n", AVS_VLD_READ32(0));

    prPicHdr->u4IsValid = 1;

    //If resolution change, return E_VDEC_OK_RES_CHG; else return E_VDEC_OK
    if(prDecPrm->fgAvsSeqChg == TRUE)
    {
        *pi4RetValue = (INT32)E_VDEC_OK_RES_CHG;
    }
    else
    {
        *pi4RetValue = (INT32)E_VDEC_OK;
    }
}


/** Get decoding handler
*@param .
 */
#if 0 //no used in current flow
void _VDEC_AvsSliceHdr(VDEC_AVS_INFO_T* prVdec, INT32* pi4RetValue)
{
    VDEC_AVS_SEQ_T* prSeqHdr;
    VDEC_AVS_PIC_T* prPicHdr;
    VDEC_AVS_SLICE_T* prSliceHdr;
    VDEC_AVS_DECPRM_T* prDecParam;
    UINT8 u1weight_para_num = 0, u1Idx = 0;

    ASSERT(prVdec != NULL);
    ASSERT(pi4RetValue != NULL);

    prSeqHdr = &prVdec->rSeqHdr;
    prPicHdr = &prVdec->rPicHdr;
    if (!prSeqHdr->u4IsValid || !prPicHdr->u4IsValid)
    {
        *pi4RetValue = (INT32)E_VDEC_FAIL;
        LOG(2, "Seq/Pic Hdr !valid in SliceHdr\n");
        return;
    }

    prDecParam = &prVdec->rDecParam;
    prSliceHdr = &prVdec->rSliceHdr;
    x_memset((void*)prSliceHdr, 0, sizeof(prSliceHdr));

    prDecParam->u4BitCnt = 0;

    if (prSeqHdr->u4HSize > 2800)
    {
        prSliceHdr->u2VPosExt = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 3);
    }
    if (prPicHdr->u4FixedPicQP == 0)
    {
        prSliceHdr->u2FixedSliceQP = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 1);
        prSliceHdr->u2SliceQP = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 6);
    }

    if (prPicHdr->u4PicCodingType != I_IMG /*|| I bottom*/)
    {
        prSliceHdr->u2SliceWeightFg = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 1);
        if (prSliceHdr->u2SliceWeightFg)
        {
            /*if(((prPicHdr->u4PicCodingType == I_IMG) && !prPicHdr->u4PicStruct) &&
                (mb_index >= (mb_width * mb_height)>>1))  //I bottom
            {
                u1weight_para_num = 1;
            }
            else */
            if ((prPicHdr->u4PicCodingType == P_IMG) && prPicHdr->u4PicStruct) //P frame coding
            {
                u1weight_para_num = 2;
            }
            else if ((prPicHdr->u4PicCodingType == P_IMG) && !prPicHdr->u4PicStruct) //P field coding
            {
                u1weight_para_num = 4;
            }
            else if ((prPicHdr->u4PicCodingType == B_IMG) && prPicHdr->u4PicStruct) //B frame coding
            {
                u1weight_para_num = 2;
            }
            else if ((prPicHdr->u4PicCodingType == B_IMG) && !prPicHdr->u4PicStruct) //B field coding
            {
                u1weight_para_num = 4;
            }

            for (u1Idx = 0; u1Idx < u1weight_para_num; u1Idx++)
            {
                prSliceHdr->u1LumaScale[u1Idx] = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 8);
                prSliceHdr->i1LumaShift[u1Idx] = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 8);
                _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 1);
                prSliceHdr->u1ChromaScale[u1Idx] = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 8);
                prSliceHdr->i1ChromaShift[u1Idx] = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 8);
                _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 1);
            }
            prSliceHdr->u2MBWeightFg = _VDEC_AVSGetRealShtBitStream(&prDecParam->u4BitCnt, 1);
        }
    }

    *pi4RetValue = (INT32)E_VDEC_OK;
}
#endif
// *********************************************************************
// Function    : INT32 _VDEC_AvsChkSignedParam (UINT32 u4Value)
// Description : Check syntax parsed by _VDEC_AvsSeCodeNum is positive or negative, and calculated the correct value
// Parameter   : syntax value parsed by _VDEC_AvsSeCodeNum
// Return      : Correct & expected value
// *********************************************************************

INT32 _VDEC_AvsChkSignedParam (UINT32 u4Value)
{
    INT32 i4RetValue;
    
    if (u4Value & 0x10000)
    {
        i4RetValue = (INT32)(u4Value & 0xFFFF);
        i4RetValue = (INT32)((((UINT32)i4RetValue ^ 0xFFFFFFFF) + 1) & 0xFFFF) * (-1);
    }
    else
    {
        i4RetValue = (INT32)(u4Value & 0xFFFF);
    }
    
    return i4RetValue;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



