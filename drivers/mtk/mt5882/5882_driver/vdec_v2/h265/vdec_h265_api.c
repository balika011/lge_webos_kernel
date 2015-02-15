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
 * $RCSfile: vdec_h265_api.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/
#include "fbm_drvif.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "vdec_h265_psr.h"
#include "vdec_h265_api.h"
//#include "x_hal_926.h"
//#include "x_hal_5381.h"
#include "x_chip_id.h"
//#include "x_common.h"
//#include "x_typedef.h"
//#include "dmx_if.h"
//#include "x_mm_common.h"

#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
H265_DRV_INFO_T *pH265DecInfo[MAX_ES_NUM] = {NULL};
#else
H265_DRV_INFO_T rH265DecInfo[MAX_ES_NUM]; 
#endif 

H265_DRV_INFO_T *pVDecGetDrvInfo(UCHAR ucEsId)
{
    #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    return pH265DecInfo[ucEsId];
    #else
    return (H265_DRV_INFO_T *)&rH265DecInfo[ucEsId];
    #endif
}

// **************************************************************************
// Function : BOOL vVDECIsSupportH265();
// Description : support h265 or not
// Parameter : None
// Return      :  Support if return true
// **************************************************************************
BOOL  vVDECIsSupportH265(void)
{
    return IS_SupportHEVC();
}

INT32 i4VDecH265Init(UCHAR ucEsId)
{
    INT32 i;
    H265_DRV_INFO_T *prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(pH265DecInfo[ucEsId] == NULL)
    {
        pH265DecInfo[ucEsId] = (H265_DRV_INFO_T*)x_mem_alloc_virtual(sizeof(H265_DRV_INFO_T));
        if(pH265DecInfo[ucEsId] == NULL)
        {
            LOG(0,"Allocate driver info struct Fail\n");
            ASSERT(0);
        }
    }
    #endif
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    
    x_memset(prH265DrvDecInfo,0,sizeof(H265_DRV_INFO_T));
    
    if(vVDecH265InitROM(ucEsId) == (INT32)E_VDEC_FAIL)
    {
        return (INT32)E_VDEC_FAIL;
    }
    
    if(vVDecH265InitRPS(ucEsId) == (INT32)E_VDEC_FAIL)
    {
        return (INT32)E_VDEC_FAIL;
    }
    
    if(vVDecH265InitScalingList(ucEsId) == (INT32)E_VDEC_FAIL)
    {
        return (INT32)E_VDEC_FAIL;
    }
    
    //init H265 driver parameter
    prH265DecPrm->bFirstSliceInSequence = TRUE;
    prH265DecPrm->ucFbgId = FBM_FBG_ID_UNKNOWN;
    prH265DecPrm->ucPreFbId = FBM_FB_ID_UNKNOWN;
    prH265DecPrm->rLastInfo.i4LastPOC = -MAX_POC;
    prH265DecPrm->i4RAPOC = MAX_POC;
    prH265DecPrm->prCurrFBufInfo = NULL;
    prH265DecPrm->prFBufInfo = &(prH265DrvDecInfo->arH265FbInfo[0]);
    prH265DecPrm->prPicInfo = &(prH265DrvDecInfo->rH265PicInfo);
    prH265DecPrm->ucPreFBIndex = 0;
    prH265DecPrm->fgWaitFirstIFrm = TRUE;
    prH265DecPrm->u4DecodeOkCnt = 0;
    prH265DecPrm->u4DecodeErrCnt = 0;
    prH265DecPrm->u4DumpFrameCnt = 0xFFFFFFFF;
    prH265DecPrm->ucSkipMode = IPBMode;
    prH265DecPrm->fgDecEuqalDispOrder = FALSE;
    
    for(i = 0; i < H265_MAX_FB_NUM; i++)
    {
        vVDecH265ClrFBufInfo(ucEsId,i,prH265DecPrm);
    }

    //Reg ISR API
    #ifndef VDEC_H265_IRQ_DISABLE
    #ifdef VDEC_COMMON_ISR
    vVDecH265InitIsr(ucEsId);
    #endif
    #endif
    
    return (INT32)E_VDEC_OK;
}


INT32 i4VDecH265DecCfg(UCHAR ucEsId)
{
    INT32 i4Ret = (INT32)E_VDEC_OK;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    #if defined(ENABLE_MULTIMEDIA) || defined(CC_H265_SUPPORT)
    #ifndef CC_VDEC_H265_DISABLE
    prVdecEsInfoKeep->pfnVdecSWRst = vVDecH265SWRst;
    prVdecEsInfoKeep->pfnVdecDecode = NULL;
    prVdecEsInfoKeep->pfnVdecHdrParse = i4VDecH265HdrPrsNalu;//_VDEC_H264HdrPrsPic;
    prVdecEsInfoKeep->pfnVdecDecStart = i4VDecH265DecStart;
    prVdecEsInfoKeep->pfnVdecDecFin = i4VDecH265DecFin;
    prVdecEsInfoKeep->pfnVdecDecDrop = NULL;
    prVdecEsInfoKeep->pfnVdecStop = vVDecH265Stop;
    prVdecEsInfoKeep->pfnVdecGetPesStruct = vVDecH265GetPesStruct; // should not need this?
    prVdecEsInfoKeep->pfnVdecGetDispPic = vVDecH265GetDispPic;
    prVdecEsInfoKeep->pfnVdecSetParam = vVDecH265SetParam;
    prVdecEsInfoKeep->pfnVdecGetParam = vVDecH265GetParam;
    prVdecEsInfoKeep->pfnVdecUpdRptr = NULL;
    prVdecEsInfoKeep->pfnVdecSetWptr = vVDecH265SetDmxWptr;
    prVdecEsInfoKeep->pfnVdecEsFlwCtrl = fgVDecH265EsFlwCtrl;
    prVdecEsInfoKeep->pfnVdecWaitRandomAP = NULL;
    prVdecEsInfoKeep->pfnVdecHandleOverFlw = ucVDecH265HandleOverFlw;
    prVdecEsInfoKeep->pfnVdecStartPts = vVDecH265StartPTS;
    #if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
    prVdecEsInfoKeep->pfnVdecIsPic = NULL;
    #else
    prVdecEsInfoKeep->pfnVdecIsPic = NULL;
    #endif
    //init function
    i4Ret = i4VDecH265Init(ucEsId);
    if(prVdecEsInfoKeep->pfnVdecSetParam)
    {
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)WAIT_FB_TIME, prVdecEsInfo->u4WaitDispTime, 0, 0);
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)AV_SYNC_INFO, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId, 0);
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)MM_PARAM, (UINT32)(prVdecEsInfo->fgMMPlayback?1:0), 0, 0);
        prVdecEsInfoKeep->pfnVdecSetParam(ucEsId, (UINT32)MM_PVR_PLAY_MODE, (UINT32)prVdecEsInfo->fgPvrPlayBack, 
                                            (UINT32)prVdecEsInfo->u1PvrPlayMode, 0);
    }
    prVdecEsInfo->fgNoFrameBuffer = FALSE;
    #endif
    #endif
    
    return i4Ret;
}


//declare H265 decode funtion pointer

void vVDecH265SWRst(UCHAR ucVdecId, UCHAR ucEsId)
{
    
}

/************************************************************/
/*INT32 i4VDecH265HdrPrsNalu(UCHAR ucVdecId, UCHAR ucEsId)*/
/*Param: */
/*ucEsId : Instant ID*/
/*ucVdecId : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/*return : E_VDEC_OK or E_VDEC_XX*/
/*************************************************************/
INT32 i4VDecH265HdrPrsNalu(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    VDEC_INFO_H265_BS_INIT_PRM_T rH265BSInitPrm = {0,0,0,0,0,0,0};
    
    INT32 i4Ret = (INT32)E_VDEC_OK;
    UCHAR ucNalType,ucLayerId,ucNuhTemporalId;
    BOOL fgForbidenZeroBits;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);

    prH265DecPrm->ucDualCoreMode = prVdecEsInfo->ucDualCoreMode;
    prH265DecPrm->ucVDecStatus = prVdecEsInfo->ucVDecStatus;
    prH265DecPrm->fgEndOfSeq = prH265DrvDecInfo->rPesInfo.fgEos;
    rH265BSInitPrm.u4VFifoSa = prH265DrvDecInfo->rPesInfo.u4FifoStart;
    rH265BSInitPrm.u4VFifoEa = prH265DrvDecInfo->rPesInfo.u4FifoEnd;
    rH265BSInitPrm.u4VLDRdPtr = prH265DrvDecInfo->rPesInfo.u4VldReadPtr;
    rH265BSInitPrm.u4DualCoreEnable = prH265DecPrm->ucDualCoreMode;

    if((   prH265DecPrm->ucVDecStatus == LAE_DECODE)  
        && IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_FORCE_SCORE)
        && (prVdecEsInfoKeep->rPesInfo.ucPicType != NAL_UNIT_SPS))
    {
        VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"Drop[%d] ES rdptr 0x%x pts 0x%x nal 0x%x Force Score\n",ucEsId,prVdecEsInfo->u4VldReadPtr,
                                        prVdecEsInfo->u4PTS,prVdecEsInfoKeep->rPesInfo.ucPicType);
        return E_VDEC_PARSE_NOT_READY;
    }
    
    //Lock vld for decode.
    _VDEC_LockVld(ucEsId,ucVdecId);
    
    if(prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE)
    {
        //reset dual core HW
        vVDecResetMultiCore(ucVdecId);        
    }
    else if(prH265DecPrm->ucDualCoreMode == M10TILE_DUAL_CORE_MODE)
    {
        //reset dual core HW
        vVDecResetMultiCore(ucVdecId);
        vVDecConfigMultiCore(ucVdecId,1);
    }
    
    if(prH265DecPrm->ucVDecStatus == LAE_DECODE)
    {
        i4VDEC_HAL_H265_LAE_InitVDecHW(ucVdecId);
    }
    else
    {
        i4VDEC_HAL_H265_InitVDecHW(ucVdecId);
    }
    
    i4VDEC_HAL_H265_InitBarrelShifter(0,ucVdecId,&rH265BSInitPrm);

    //Parser Nal Unit Header
    fgForbidenZeroBits = fgVDecH265NalUnitHeader(ucVdecId,&ucNalType,&ucLayerId,&ucNuhTemporalId);
    if(fgForbidenZeroBits)
    {
        LOG(0,"[H265-ERR-%d] Disobey spec forbidden bit should be 0\n",ucEsId);
    }
    
    prH265DecPrm->u4NalType = ucNalType;
    prH265DecPrm->u4LayerId = ucLayerId;
    prH265DecPrm->u4NuhTemporalId = ucNuhTemporalId;
    
    if(prVdecEsInfoKeep->rPesInfo.ucPicType != prH265DecPrm->u4NalType)
    {
        LOG(0,"Warning, PES Pictype[0x%x] != Real Pasre[0x%x]!\n",prVdecEsInfoKeep->rPesInfo.ucPicType,prH265DecPrm->u4NalType);
    }
    
    //if Dual core mode,init VDEC_VLD1 
    if(prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE)
    {
        if(ucVdecId == VDEC_VLD1)
        {
            LOG(0,"[Error-%d] Set the same HW id !!!!!!!!!\n",ucEsId);
            ASSERT(0);
        }
        i4VDEC_HAL_H265_InitVDecHW(VDEC_VLD1);
        i4VDEC_HAL_H265_InitBarrelShifter(0,VDEC_VLD1,&rH265BSInitPrm);

        //Parser Nal Unit Header
        fgForbidenZeroBits = fgVDecH265NalUnitHeader(VDEC_VLD1,&ucNalType,&ucLayerId,&ucNuhTemporalId);
        if(fgForbidenZeroBits)
        {
            LOG(0,"[H265-ERR-%d] Disobey spec forbidden bit should be 0\n",ucEsId);
        }
    }
    
    switch(ucNalType)
    {
        case NAL_UNIT_CODED_SLICE_BLA:
        case NAL_UNIT_CODED_SLICE_BLANT:
        case NAL_UNIT_CODED_SLICE_BLA_N_LP:
        case NAL_UNIT_CODED_SLICE_IDR:
        case NAL_UNIT_CODED_SLICE_IDR_N_LP:
        case NAL_UNIT_CODED_SLICE_CRA:
        case NAL_UNIT_CODED_SLICE_TRAIL_R:
        case NAL_UNIT_CODED_SLICE_TRAIL_N:
        case NAL_UNIT_CODED_SLICE_TLA:
        case NAL_UNIT_CODED_SLICE_TSA_N:
        case NAL_UNIT_CODED_SLICE_STSA_R:
        case NAL_UNIT_CODED_SLICE_STSA_N:
        case NAL_UNIT_CODED_SLICE_RADL_N:
        case NAL_UNIT_CODED_SLICE_DLP:
        case NAL_UNIT_CODED_SLICE_RASL_N:
        case NAL_UNIT_CODED_SLICE_TFD:
            vVDecH265SlicePsr(ucEsId,ucVdecId,prH265DrvDecInfo,&i4Ret);            
            break;
        case NAL_UNIT_VPS:
            break;
        case NAL_UNIT_SPS:
            i4VDecH265SPSPsr(ucEsId,ucVdecId,prH265DrvDecInfo,&i4Ret);
            #ifdef CC_DUAL_CORE_DEC
            if(i4Ret == SPS_OK)
            {
                UNUSED(fgSMcoreSeamlessChg(ucEsId));
            }
            #endif
            break;
        case NAL_UNIT_PPS:
            vVDecH265PPSPsr(ucEsId,ucVdecId,prH265DrvDecInfo,&i4Ret);
            break;
        case NAL_UNIT_SEI:
            break;
        case NAL_UNIT_EOS:
            break;
        default:
            break;
    }

    //LOG(2,"[H265-%d] ucNalType (0x%x) L&T Id [%d,%d] Ret %d\n",ucEsId,ucNalType,ucLayerId,ucNuhTemporalId,i4Ret);
    
    if((i4Ret != (INT32)E_VDEC_OK) && (i4Ret != (INT32)E_VDEC_OK_RES_CHG))
    {
        //add error handle 
        if(i4Ret == SLICE_ST_DROP)
        {
            i4Ret = E_VDEC_FAIL;
            vVDecH265UpdDmxRptr(ucEsId,ucVdecId);
            if(prH265DecPrm->ucDualCoreMode)
            {
                vVDecResetMultiCore(ucVdecId);
            }
            _VDEC_UnlockVld(ucEsId,ucVdecId);
            VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"Drop[%d] ES rdptr 0x%x pts 0x%x nal 0x%x\n",ucEsId,prVdecEsInfo->u4VldReadPtr,
                                        prVdecEsInfo->u4PTS,ucNalType);
        }
        else if(i4Ret == GET_FB_TIMEOUT)//Real decode error case.
        {
            i4Ret = E_VDEC_REPARSE;
            if(prH265DecPrm->ucDualCoreMode)
            {
                vVDecResetMultiCore(ucVdecId);
            }
            _VDEC_UnlockVld(ucEsId,ucVdecId);
            //allocate empty fbm again and release VLD as soon as possible
            prH265DecPrm->ucPreFbId = FBM_GetEmptyFrameBuffer(prH265DecPrm->ucFbgId, prH265DecPrm->u4WaitFbTime);
            VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"GetFB Fail [%d] ES rdptr 0x%x pts 0x%x nal 0x%x reallocate %d \n",ucEsId,prVdecEsInfo->u4VldReadPtr,
                                        prVdecEsInfo->u4PTS,ucNalType,prH265DecPrm->ucPreFbId);
        }
        else if(i4Ret == ALLOC_FBG_FAIL)
        {
            i4Ret = E_VDEC_NOT_SUPPORT;
            _VDEC_UnlockVld(ucEsId,ucVdecId);
            VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"Allocate FBG Fail @ V%d Width&Height [%d,%d] \n",ucEsId,prH265DrvDecInfo->rH265DecParam.u4PicW,prH265DrvDecInfo->rH265DecParam.u4PicH);
        }
        else
        {
            _VDEC_UnlockVld(ucEsId,ucVdecId);
            VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"H265 Parse Fail 0x%x \n",i4Ret);
        }
    }
    else if((i4Ret == (INT32)E_VDEC_OK) && (!IsH265SliceNalUnit(ucNalType)))
    {
        i4Ret = (INT32)E_VDEC_PARSE_NOT_READY;
        _VDEC_UnlockVld(ucEsId,ucVdecId);
        vVDecH265CalcBitRate(ucEsId);
    }
    else //Slice parse OK
    {
        if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I) && IsH265RAPSlice(prH265DecPrm->u4NalType))
        {
            CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
            LOG(3,"VDEC [%d] Wait DEC_FLG_WAIT_NEXT_I done #%d\n",ucEsId,__LINE__);
        }
        vVDecH265CalcBitRate(ucEsId);
        vVDecH265CalcSliceQP(ucEsId);
    }
    
    return i4Ret;
}

/************************************************************/
/*INT32 i4VDecH265DecStart(UCHAR ucVdecId, UCHAR ucEsId)*/
/*Param: */
/*ucEsId : Instant ID*/
/*ucVdecId : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/*return : E_VDEC_OK or E_VDEC_XX*/
/*************************************************************/
INT32 i4VDecH265DecStart(UCHAR ucVdecId, UCHAR ucEsId)
{   
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);
    if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
    {
        prH265DecPrm->i4ReadyOutCntDPB ++;
    }

    #ifdef VDEC_H265_DEEP_DEBUG
    //dump register for debug
    if((prH265DecPrm->ucVDecStatus == REAL_DECODE) && ((prH265DecPrm->u4DecodeOkCnt + prH265DecPrm->u4DecodeErrCnt) == prH265DecPrm->u4DumpFrameCnt))
    {
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET0,0,0);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET0,33,37);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET0,40,255);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,VLD_REG_OFFSET0,33,255);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,AVC_MV_BASE0,0,255);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,MC_REG_OFFSET0,0,827);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_PP_REG_OFFSET0,0,1023);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,VLD_TOP_REG_OFFSET0,0,128);
        x_thread_delay(500);
        vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,VDEC_MISC_BASE0,0,128);
        
        if(prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE)
        {
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,MVDEC_TOP_OFFSET0,0,127);
            LOG(0,"// Dump core1 register now \n");
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET1, 0, 0);      //HEVC
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET1, 33, 37);  //HEVC
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_VLD_REG_OFFSET1, 40, 255);     //HEVC
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,VLD_REG_OFFSET1, 33, 255);      //VLD
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,AVC_MV_BASE1, 0, 255);        //MV
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,MC_REG_OFFSET1, 0, 827);        //MC
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,HEVC_PP_REG_OFFSET1, 0, 1023); 
            vVDEC_HAL_H265_VDec_DumpReg(ucVdecId,VLD_TOP_REG_OFFSET1,0,128);
        }
    }
    #endif
    VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-TEST-%d] Trigger decode type %d %d\n",ucEsId,prH265DecPrm->ucVDecStatus,prH265DecPrm->ucDualCoreMode);
    i4VDEC_HAL_H265_DecStart(ucVdecId,prH265DecPrm);
    return E_VDEC_OK;
}

/************************************************************/
/*INT32 i4VDecH265DecFin(UCHAR ucVdecId, UCHAR ucEsId)*/
/*Param: */
/*ucEsId : Instant ID*/
/*ucVdecId : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/*return : E_VDEC_OK or E_VDEC_XX*/
/*************************************************************/
INT32 i4VDecH265DecFin(UCHAR ucVdecId, UCHAR ucEsId)
{
    UCHAR ucMainEsId;

    #ifdef VDEC_H265_DEEP_DEBUG
    UINT32 u4Y[8],u4C[8];
    #endif
    #ifdef VDEC_H265_IRQ_DISABLE 
    BOOL fgDecDone = FALSE;
    UINT32 u4WaitCount = 0;
    #endif
    
    VDEC_ES_INFO_T *prVdecEsInfo;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;


    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);

    #ifndef VDEC_H265_IRQ_DISABLE 
    vVDecH265WaitDecFinish(ucEsId,ucVdecId,prH265DecPrm);
    #else//Pollig dec finish
    fgDecDone = u4VDEC_HAL_H265_VDec_ReadFinishFlag(ucVdecId);
    while((fgDecDone != 1) && (u4WaitCount < 500))
    {
        x_thread_delay(10);
        fgDecDone = u4VDEC_HAL_H265_VDec_ReadFinishFlag(ucVdecId);
        u4WaitCount ++;
    }

    if(u4WaitCount >= 500)
    {
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-ERR-%d] Decode timeout !!!!!!!!!!!!!!!!!!!!\n",ucEsId);
    }
    else
    {
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,,"[H265-OK-%d] Decode OK ^^^^^^^^^^^^^^^^^^^^^^^^^\n",ucEsId);
    }

    //clean interrupt
    u4VDEC_HAL_H265_VDec_ClearInt(ucVdecId);
    #endif

    #ifdef VDEC_H265_DEEP_DEBUG
    /*****************************************/
    /*     VDEC crc for debug                */
    /*****************************************/
    if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
    {
        vVDecH265GetCRC(ucEsId,ucVdecId,u4Y,u4C,prH265DecPrm);
        VDecPrintf(ucEsId,VDEC_LOG_VDEC_CRC,"[CRC_Y]0x%08x 0x%08x 0x%08x 0x%08x\n",INVERSE_ENDIAN(u4Y[0]),INVERSE_ENDIAN(u4Y[1]),INVERSE_ENDIAN(u4Y[2]),INVERSE_ENDIAN(u4Y[3]));        
        //VDecPrintf(ucEsId,VDEC_LOG_VDEC_CRC,"[CRC_Y]0x%08x 0x%08x 0x%08x 0x%08x\n",INVERSE_ENDIAN(u4C[0]),INVERSE_ENDIAN(u4C[1]),INVERSE_ENDIAN(u4C[2]),INVERSE_ENDIAN(u4C[3]));               
    }
    #endif

    #ifdef VDEC_H265_Performance
    vVDecH265Performance(ucEsId,ucVdecId,prH265DecPrm);
    #endif
    
    #ifdef __MODEL_slt__
    UNUSED(fgSltCompare(ucEsId,ucVdecId));
    #endif
    
    if(prH265DecPrm->ucDualCoreMode)
    {
        vVDecResetMultiCore(ucVdecId);
    }
//    else if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
//    {
//        i4VDEC_HAL_H265_InitVDecHW(ucVdecId);
//    }
//    else if(prH265DecPrm->ucVDecStatus == LAE_DECODE)
//    {
//        i4VDEC_HAL_H265_LAE_InitVDecHW(ucVdecId);
//    }
    _VDEC_UnlockVld(ucEsId,ucVdecId);
    
    if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
    {
        prH265DecPrm->prCurrFBufInfo->eH265DpbStatus = H265_DPB_STATUS_DECODED;
    
        #ifdef VDEC_H265_EMU
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->ucFbId, FBM_FB_STATUS_EMPTY);
        #else
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->ucFbId, FBM_FB_STATUS_READY);
        //Output Pic 
        ucVDecH265PrepareDispBuf(ucEsId,prH265DecPrm);
        #endif
        prH265DecPrm->ucPreFBIndex = prH265DecPrm->ucDecFBufId;
        
        //Put used lae buffer to empty Q
        if(prH265DecPrm->ucDualCoreMode)
        {
            vPutBufferToLae(prVdecEsInfo->ucLaeEsInfoId,&(prH265DecPrm->rLaeInfo));
        }
        prH265DecPrm->rLastInfo.i4LastPOC = prH265DecPrm->prCurrFBufInfo->i4POC;
        prH265DecPrm->ucPreFbId = FBM_FB_ID_UNKNOWN;
    }
    else if(prH265DecPrm->ucVDecStatus == LAE_DECODE)
    {
        //Send LAE decode buffer to hMsgQLaeOuput for Real_deocde usage
        ucMainEsId = ucVDecGetMcoreDecInst(ucEsId);
        prH265DecPrm->rLaeInfo.u4PesRdptr = prH265DrvDecInfo->rPesInfo.u4VldReadPtr;
        vPutBufferToMcore(ucMainEsId,&(prH265DecPrm->rLaeInfo));
    }
       
    vVDecH265UpdDmxRptr(ucEsId,ucVdecId);
    return E_VDEC_OK;
}

/************************************************************/
/*void vVDecH265Stop(UCHAR ucEsId)*/
/*Param: */
/*ucEsId : Instant ID*/
/*************************************************************/
void vVDecH265Stop(UCHAR ucEsId)
{
    SIZE_T zMsgSize;
    UINT16 u2MsgQIdx;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeepLae;
    VDEC_LAE_DecPrm rLAEDecPrm;

    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);

    vVDecH265FlushDPB(ucEsId,prH265DecPrm);

    if(prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);         
        FBM_WaitUnlockFrameBuffer(prVdecEsInfo->ucFbgId,H265_UNLOCK_FRAME_TIME);
        FBM_ReleaseGroup(prVdecEsInfo->ucFbgId);
        prVdecEsInfo->ucFbgId = FBM_FBG_ID_UNKNOWN;
    }

    LOG(2,"H265 Stop %d Mode %d\n",ucEsId,prVdecEsInfo->ucDualCoreMode);
    if(prVdecEsInfo->ucDualCoreMode == DUAL_CORE_MODE)
    {
        vVDecResetMultiCore(VDEC_VLD0);
    }
    else if(prVdecEsInfo->ucDualCoreMode == M10TILE_DUAL_CORE_MODE) // LAE + MCore resethw
    {
        vVDEC_HAL_H265_VDec_DisableMCore(VDEC_VLD0);
        vVDecResetLAEMultiCore(VDEC_VLD0);
    }

    if(prVdecEsInfo->ucDualCoreMode)
    {
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        prVdecEsInfoKeepLae = _VDEC_GetEsInfoKeep(prVdecEsInfo->ucLaeEsInfoId);
        //Flush LAE Q 
        //clear LAE buffer input Q
        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        while (x_msg_q_receive(&u2MsgQIdx, (void*)&rLAEDecPrm, &zMsgSize,
                &(prVdecEsInfoKeepLae->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
        
        }

        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        //clear LAE buffer output Q
        while (x_msg_q_receive(&u2MsgQIdx, (void*)&rLAEDecPrm, &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeOuput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
        
        }

        prVdecEsInfo->ucDualCoreMode = FALSE;
    }
}


void vVDecH265GetPesStruct(UCHAR ucEsId, void **ppvPesInfo)
{
    H265_DRV_INFO_T* prVdec;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    
    ASSERT(ppvPesInfo !=NULL);    
    ASSERT(ucEsId < MAX_ES_NUM);
    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdec = pVDecGetDrvInfo(ucEsId);
    *ppvPesInfo = (void*)(&prVdec->rPesInfo);
    
    #ifdef VDEC_H265_Performance
    if(VDEC_FMT_MCORE_SUPPORT(prVdecEsInfoKeep->eCurFMT) && (prVdecEsInfo->ucVDecStatus == REAL_DECODE))
    {
        HAL_GetTime(&(prVdec->rH265DecParam.rTimeRcvES));
    }
    #endif

}


void vVDecH265GetDispPic(UCHAR ucEsId, void* pvInfo)
{
    UINT32 u4PicWidth,u4PicHeight;
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    H265_DRV_INFO_T *prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;

    //LAE no need check crc.
    if(prH265DecPrm->ucVDecStatus == LAE_DECODE)
    {
        prDispInfo->pvYAddr = NULL;
        return ;
    }
    
    u4PicWidth = ((prH265DecPrm->prSPS->u4PicWidthInLumaSamples + 63) >> 6) << 6;
    u4PicHeight = ((prH265DecPrm->prSPS->u4PicHeightInLumaSamples + 31) >> 5) << 5; 
    
    prDispInfo->pvYAddr = (void *)prH265DecPrm->prCurrFBufInfo->u4YStartAddr;
    prDispInfo->pvCAddr = (void *)prH265DecPrm->prCurrFBufInfo->u4CStartAddr;
    prDispInfo->u4YSize = u4PicWidth * u4PicHeight;
    prDispInfo->u4CSize = prDispInfo->u4YSize >> 1;
    if(prH265DecPrm->ucDualCoreMode)
    {
        prDispInfo->ucDualMode = prH265DecPrm->ucDualCoreMode;
    }
    prDispInfo->ucUFOEnable = prH265DecPrm->fgIsUFOMode;
    
    vVDecH265GetCRC(ucEsId,prDispInfo->u4VLDidx,prDispInfo->u4Ycrc,prDispInfo->u4Ccrc,prH265DecPrm);

}

void vVDecH265SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    H265_DRV_INFO_T* prH265DrvDecInfo;
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    
    if(u4Type == (UINT32)MM_PLAY_DTV)
    {
        prH265DrvDecInfo->rH265DecParam.fgIsPVR = TRUE;
    }
    else if(u4Type == (UINT32)AV_SYNC_INFO)
    {
        prH265DrvDecInfo->rH265DecParam.ucSyncMode = (UCHAR)u4Param1;
        prH265DrvDecInfo->rH265DecParam.ucStcId = (UCHAR)u4Param2;
    }
    else if(u4Type==(UINT32)MM_PARAM)
    {
        prH265DrvDecInfo->rH265DecParam.fgIsMMPlay = (u4Param1>0)?TRUE:FALSE;
        if(prH265DrvDecInfo->rH265DecParam.fgIsMMPlay)
        {
            prH265DrvDecInfo->rH265DecParam.u4WaitFbTime = VDEC_AVSYNC_WAIT_DISP_TIME;
        }
    }
    else if(u4Type == (UINT32)VDEC_STATUS)
    {
        prH265DrvDecInfo->rH265DecParam.ucVDecStatus = u4Param1;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-SET] i[%d],t[%d],p[%d]\n",ucEsId,u4Type,u4Param1);
    }
    else if(u4Type == (UINT32)VDEC_UFO_MODE)
    {
        prH265DrvDecInfo->rH265DecParam.fgIsUFOMode = u4Param1;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-SET] i[%d],t[%d],p[%d]\n",ucEsId,u4Type,u4Param1);        
    }
    else if(u4Type == (UINT32)VDEC_DUMP_FRM_No)
    {
        prH265DrvDecInfo->rH265DecParam.u4DumpFrameCnt = u4Param1;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-SET] i[%d],t[%d],p[%d]\n",ucEsId,u4Type,u4Param1);        
    }
}

void vVDecH265GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
    H265_DRV_INFO_T* prH265DrvDecInfo;
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);

    if(u4Type == (UINT32)VDEC_STATUS)
    {
        *pu4Param1 = prH265DrvDecInfo->rH265DecParam.ucVDecStatus;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-GET] i[%d],t[%d],p[%d]\n",ucEsId,u4Type,*pu4Param1);
    }
    else if(u4Type == (UINT32)BS_TYPE)
    {
        *pu4Param1 = prH265DrvDecInfo->rH265DecParam.prSPS->rSPS_PTL.generalPTL.u4ProfileIdc;
        *pu4Param2 = prH265DrvDecInfo->rH265DecParam.prSPS->rSPS_PTL.generalPTL.u4LevelIdc;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-GET] Profile Level[%d,%d]\n",*pu4Param1,*pu4Param2);
    }
    else if(u4Type == (UINT32)DISP_IDX)
    {
        *pu4Param1 = prH265DrvDecInfo->rH265DecParam.ucFbgId;
        //VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-GET] FBG %d\n",*pu4Param1);
    }
    else if(u4Type == (UINT32)SEQ_HDR_INFO)
    {
        if(prH265DrvDecInfo->rH265DecParam.prSPS)
        {
            *pu4Param1 = prH265DrvDecInfo->rH265DecParam.prSPS->u4PicWidthInLumaSamples;
            *pu4Param2 = prH265DrvDecInfo->rH265DecParam.prSPS->u4PicHeightInLumaSamples;
            *pu4Param3 = ucVDecH265GetFrameRate(ucEsId,&prH265DrvDecInfo->rH265DecParam);
        }
    }
    else if(u4Type == (UINT32)VDEC_SLICE_QP)
    {
        *pu4Param1 = prH265DrvDecInfo->rH265DecParam.u4SliceQP;
    }
}

void vVDecH265SetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    
}

BOOL fgVDecH265EsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    H265_DRV_INFO_T *prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    VDEC_ES_INFO_T* prVdecEsInfo;
    #ifdef CC_DUAL_CORE_DEC
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    H265_DRV_INFO_T *prH265LaeDrvDecInfo;
    UINT32 u4Mode;
    #endif
    UCHAR ucMainEsID;
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);


    if(ucSkipMode != prH265DecPrm->ucSkipMode)
    {
        
        if(ucSkipMode == IMode)
        {
            prH265DecPrm->fgDecEuqalDispOrder = TRUE;
        }
        else
        {
            prH265DecPrm->fgDecEuqalDispOrder = FALSE;
        }

        #ifdef CC_DUAL_CORE_DEC
        if(prVdecEsInfo->ucDualCoreMode)
        {
            prH265LaeDrvDecInfo = pVDecGetDrvInfo(prVdecEsInfo->ucLaeEsInfoId);
            if((ucSkipMode == IPBMode) && (prH265DecPrm->ucSkipMode == IPMode))
            {
                if( ((prVdecEsInfo->fgSetTargetRdptr) && (prVdecEsInfo->u4TargetRdptr != prH265DrvDecInfo->rPesInfo.u4VldReadPtr))
                    || (prH265LaeDrvDecInfo->rH265DecParam.ucSkipMode != IPBMode))
                {
                    ucSkipMode = prH265DecPrm->ucSkipMode;
                }
                else
                {
                    prVdecEsInfo->fgSetTargetRdptr = FALSE;
                    LOG(1,"VDEC[%d] Skip mode a change %d -> %d\n",ucEsId,prH265DecPrm->ucSkipMode,ucSkipMode);
                    prH265DecPrm->ucSkipMode = ucSkipMode;
                }
            }
            else
            {
                LOG(1,"VDEC[%d] Skip mode b change %d -> %d\n",ucEsId,prH265DecPrm->ucSkipMode,ucSkipMode);
                prH265DecPrm->ucSkipMode = ucSkipMode;
            }
        }
        else
        #endif
        {
            if((prVdecEsInfo->ucVDecStatus == LAE_DECODE) && (ucSkipMode == IPBMode) && (prH265DecPrm->ucSkipMode == IPMode))
            {   
                VDEC_ES_INFO_T* prVdecMcoreEsInfo;
                //Set Mcore target PES
                ucMainEsID = ucVDecGetMcoreDecInst(ucEsId);
                prVdecMcoreEsInfo = _VDEC_GetEsInfo(ucMainEsID);
                prVdecMcoreEsInfo->fgSetTargetRdptr = TRUE;
                prVdecMcoreEsInfo->u4TargetRdptr = prH265DrvDecInfo->rPesInfo.u4VldReadPtr;
                LOG(2,"VDEC[%d] Set Target rdptr 0x%x\n",ucEsId,prVdecMcoreEsInfo->u4TargetRdptr);
                
            }
            LOG(1,"VDEC[%d] Skip mode c change %d -> %d\n",ucEsId,prH265DecPrm->ucSkipMode,ucSkipMode);
            prH265DecPrm->ucSkipMode = ucSkipMode;
            
        }
    }
    
    if(  ucSkipMode == IPMode || ucSkipMode == IMode 
      || (0 && (prH265DecPrm->ucFbgId != FBM_FBG_ID_UNKNOWN) && FBM_ChkFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SKIP_PIC)))
    {
        //Drop nonRef Slice
        if(IsH265NonRefSlice(prH265DrvDecInfo->rPesInfo.ucPicType))
        {
            if((prH265DecPrm->ucFbgId != FBM_FBG_ID_UNKNOWN) && FBM_ChkFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SKIP_PIC))
            {
                LOG(3,"VDEC%d Force drop by B2R \n",ucEsId);
                FBM_ClrFrameBufferFlag(prH265DecPrm->ucFbgId, FBM_FLAG_SKIP_PIC);
            }
            VDecPrintf(ucEsId,VDEC_LOG_DROPPIC,"Drop CurES skip mode %d\n",ucSkipMode);
            return TRUE;
        }
    }
    
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        #ifdef CC_DUAL_CORE_DEC
        prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
        prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,NULL,NULL);
        if(u4Mode == REAL_DECODE)
        {
            vVDecH265FlushDPB(ucEsId,prH265DecPrm);   
        }
        prH265DecPrm->fgWaitFirstIFrm = TRUE;
        #else
        vVDecH265FlushDPB(ucEsId,prH265DecPrm);
        prH265DecPrm->fgWaitFirstIFrm = TRUE;
        #endif
    }

    
    
    return FALSE;
}

UCHAR ucVDecH265HandleOverFlw(UCHAR ucVldId, UCHAR ucEsmQId, HANDLE_T hMutex,HANDLE_T hMsgQueue, const void* pvPesInfo)
{
    return 1;
}

void vVDecH265StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, UINT32 u4PtsDiff)
{
    
}

void vVDecH265Flush(UCHAR ucEsId,BOOL fgDisp)
{
    H265_DRV_INFO_T *prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    
    VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H265-GET] vVDecH265Flush %d\n",ucEsId,fgDisp ? "TRUE" : "FALSE");
    if(fgDisp)
    {
        vVDecH265FlushDPB(ucEsId,prH265DecPrm);
    }
    else
    {
        vVDecH265ClrDPB(ucEsId,prH265DecPrm);
    }
}

// **************************************************************************
// Function : void vVDecH265FlushDecInfo(UCHAR ucEsId)
// Description : Flush Decode parameter info
// Parameter : None
// Return      :  
// **************************************************************************
void vVDecH265FlushDecInfo(UCHAR ucEsId)
{
    H265_DRV_INFO_T *prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;

    if(prH265DecPrm->ucPreFbId != FBM_FB_ID_UNKNOWN)
    {
        //Release decode buffer which will not be used any more because of flush
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->ucPreFbId, FBM_FB_STATUS_READY);
        FBM_SetFrameBufferStatus(prH265DecPrm->ucFbgId, prH265DecPrm->ucPreFbId, FBM_FB_STATUS_EMPTY);
        prH265DecPrm->ucPreFbId = FBM_FB_ID_UNKNOWN;
    }

    prH265DecPrm->bFirstSliceInSequence = TRUE; 
}

void vVDecH265UpdDmxRptr(UCHAR ucEsId,UCHAR ucVdecId)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    UINT32 u4DecodeSZ,u4PicSZ;
    UINT32 u4Rptr,u4Bits;
    #ifdef CC_DUAL_CORE_DEC
    UINT32 u4Mode;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    #endif
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    #ifdef CC_DUAL_CORE_DEC
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,NULL,NULL);
    if(u4Mode == LAE_DECODE)
    {
        return ;
    }
    else
    #endif
    {
        if(prVdecEsInfo->u4VldReadPtrEnd > prVdecEsInfo->u4VldReadPtr)
        {
            u4PicSZ = prVdecEsInfo->u4VldReadPtrEnd - prVdecEsInfo->u4VldReadPtr;
        }
        else
        {
            u4PicSZ = (prVdecEsInfo->u4FifoEnd - prVdecEsInfo->u4VldReadPtr) + 
                        (prVdecEsInfo->u4VldReadPtrEnd - prVdecEsInfo->u4FifoStart);
        }

        u4Rptr = u4VDEC_HAL_H265_ReadRdPtr(0,ucVdecId,prVdecEsInfo->u4FifoStart,&u4Bits) + prVdecEsInfo->u4FifoStart;

        if(u4Rptr > prVdecEsInfo->u4VldReadPtr)
        {
            u4DecodeSZ = u4Rptr - prVdecEsInfo->u4VldReadPtr;
        }
        else
        {
            u4DecodeSZ = (prVdecEsInfo->u4FifoEnd - prVdecEsInfo->u4VldReadPtr) + 
                        (u4Rptr - prVdecEsInfo->u4FifoStart);
        }

        if(u4DecodeSZ > u4PicSZ + 0x100) // THD value need confirm ????
        {
            LOG(5,"VDEC[%d] decode overflow pic & dec size [0x%x,0x%x]\n",ucEsId,u4PicSZ,u4DecodeSZ);
        }

        if (!VDEC_SetRptr(ucEsId, prVdecEsInfo->u4VldReadPtr, prVdecEsInfo->u4VldReadPtr))
        {
            LOG(2,"VDEC[%d] update dmx rdptr fail\n",ucEsId);
        }
    }


}


void vVDECH265SetIsr(UCHAR ucEsId,UCHAR ucVDecID)
{
    UINT16 u2Vector;
    
    if(ucVDecID == VDEC_VLD0)
    {
        u2Vector = VECTOR_VDEC;
    }
    #ifdef CC_DUAL_CORE_DEC
    else if(ucVDecID == VDEC_VLD1)
    {
        u2Vector = VECTOR_VDEC_CORE1;
    }
    else if(ucVDecID == VDEC_LAE0)
    {
        u2Vector = VECTOR_VDEC_LAE0;
    }
    else if(ucVDecID == VDEC_LAE1)
    {
        u2Vector = VECTOR_VDEC_LAE1;
    }
    #endif
    _VDEC_RegIsr(u2Vector,vVDecH265Isr);
}


#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 _VDEC_H265PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    VDEC_INFO_H265_BS_INIT_PRM_T rH265BSInitPrm = {0,0,0,0,0,0,0};
    BOOL fgForbidenZeroBits;
    UCHAR ucNalType,ucLayerId,ucNuhTemporalId;
    UCHAR ucFrameRate = MPEG_FRAME_RATE_UNKNOWN;
    INT32 i4Ret = E_VDEC_OK,i4SpsId = 0;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);

    rH265BSInitPrm.u4VFifoSa = prH265DrvDecInfo->rPesInfo.u4FifoStart;
    rH265BSInitPrm.u4VFifoEa = prH265DrvDecInfo->rPesInfo.u4FifoEnd;
    rH265BSInitPrm.u4VLDRdPtr = prH265DrvDecInfo->rPesInfo.u4VldReadPtr;
    rH265BSInitPrm.u4DualCoreEnable = 0;

    _VDEC_LockVld(ucEsId,ucMpvId);
    
    i4VDEC_HAL_H265_InitVDecHW(ucMpvId);
    i4VDEC_HAL_H265_InitBarrelShifter(0,ucMpvId,&rH265BSInitPrm);
    
    //Parser Nal Unit Header
    fgForbidenZeroBits = fgVDecH265NalUnitHeader(ucMpvId,&ucNalType,&ucLayerId,&ucNuhTemporalId);
    if(fgForbidenZeroBits)
    {
        LOG(0,"[H265-ERR-%d] Disobey spec forbidden bit should be 0\n",ucEsId);
    }
    
    prH265DecPrm->u4NalType = ucNalType;
    prH265DecPrm->u4LayerId = ucLayerId;
    prH265DecPrm->u4NuhTemporalId = ucNuhTemporalId;

    if(ucNalType != prVdecEsInfo->ucPicType)
    {
        LOG(0,"LAE status maybe error ...............................\n");
    }
    
    if(ucNalType == NAL_UNIT_SPS)
    {
        i4VDecH265SPSPsr(ucEsId,ucMpvId,prH265DrvDecInfo,&i4Ret);
        for(i4SpsId = 0; i4SpsId < H265_MAX_SPS_NUM; i4SpsId ++)
        {
            if(prH265DrvDecInfo->arH265SPS[i4SpsId].bSPSValid)
            {
                prVdecEsInfo->u2OrgHSize = prH265DrvDecInfo->arH265SPS[i4SpsId].u4PicWidthInLumaSamples;
                prVdecEsInfo->u2OrgVSize = prH265DrvDecInfo->arH265SPS[i4SpsId].u4PicHeightInLumaSamples;
                prH265DecPrm->prSPS = &(prH265DrvDecInfo->arH265SPS[i4SpsId]);
                ucFrameRate = ucVDecH265GetFrameRate(ucEsId,prH265DecPrm);
                if(ucFrameRate != MPEG_FRAME_RATE_UNKNOWN)
                {
                    prVdecEsInfo->u4FrameRate = u4VDecMPEGFrmMapFrameRate(ucEsId,ucFrameRate);                    
                }
                else //Need handle more smart ...
                {
                    prVdecEsInfo->u4FrameRate = 0;
                }
                
                if(prH265DecPrm->ucMain10Mode == HEVC_COMPACT_MODE)
                {
                    prVdecEsInfo->u4BitDepth = 10;
                }
                else
                {
                    prVdecEsInfo->u4BitDepth = 8;
                }
                
                break;
            }
        }
    }
    else
    {
        i4Ret = E_VDEC_PARSE_NOT_READY;
    }
    
    prVdecEsInfo->fgProgressive = TRUE;
    _VDEC_UnlockVld(ucEsId,ucMpvId);
    return i4Ret;
}
#endif

UINT32 u4VDecMPEGFrmMapFrameRate(UINT32 u4InstID,UCHAR u2MPEGFrameRate)
{
    UCHAR u2TempFrameRate = u2MPEGFrameRate;
    UINT32 u4RetVal = 0;
    
    switch(u2TempFrameRate)
    {
        case MPEG_FRAME_RATE_12:
           u4RetVal = 1200;
           break;
        case MPEG_FRAME_RATE_15:
           u4RetVal = 1500;
           break;
        case MPEG_FRAME_RATE_20:
            u4RetVal = 2000;
            break;
        case MPEG_FRAME_RATE_24_: /* 24000/1001 (23.976...) */
            u4RetVal = 2397;
            break;
        case MPEG_FRAME_RATE_24:
            u4RetVal = 2400;
            break;
        case MPEG_FRAME_RATE_25:
            u4RetVal = 2500;
            break;
        case MPEG_FRAME_RATE_30_: /* 30000/1001 (29.97...) */
            u4RetVal = 2997;
            break;
        case MPEG_FRAME_RATE_30:
            u4RetVal = 3000;
            break;
        case MPEG_FRAME_RATE_50:
            u4RetVal = 5000;
            break;
        case MPEG_FRAME_RATE_60_: /* 60000/1001 (59.94...) */
            u4RetVal = 5994;
            break;
        case MPEG_FRAME_RATE_60:
            u4RetVal = 6000;
            break;
        default:
            u4RetVal = 3000;
            break;
    }   

    return u4RetVal;
}

#ifdef CC_DUAL_CORE_DEC
BOOL fgSMcoreSeamlessChg(UCHAR ucEsId)
{
#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    BOOL fgRet = FALSE;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);
    
    if(!VDEC_IS_SEAMLESS(prVdecEsInfo->eMMSrcType))
    {
        return FALSE;
    }
    
    if(  IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_FORCE_SCORE)
      && (prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicWidthInLumaSamples >= VDEC_H265_SEAMLESS_WIDTH_THD))
    {
        CLR_DECFLAG(prVdecEsInfo, DEC_FLAG_FORCE_SCORE);
        prVdecEsInfo->ucDualCoreMode = (prH265DecPrm->ucVDecStatus == REAL_DECODE);
        fgRet = TRUE;
        LOG(2,"HEVC[%d] SCore ========> MCore [%d %d]\n",ucEsId,
                    prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicWidthInLumaSamples,
                    prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicHeightInLumaSamples);
    }
    else if(  (!IS_DECFLAG_SET(prVdecEsInfo, DEC_FLAG_FORCE_SCORE))
            && (prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicWidthInLumaSamples < VDEC_H265_SEAMLESS_WIDTH_THD))
    {
        SET_DECFLAG(prVdecEsInfo, DEC_FLAG_FORCE_SCORE);
        prVdecEsInfo->ucDualCoreMode = SINGLE_CORE_MODE;
        fgRet = TRUE;
        LOG(2,"HEVC[%d] MCore ========> SCore [%d %d] \n",ucEsId,
                    prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicWidthInLumaSamples,
                    prH265DrvDecInfo->arH265SPS[prH265DecPrm->ucSemlessSPSIdx].u4PicHeightInLumaSamples);
    }

    return fgRet;
#else
    return FALSE;
#endif
}
#endif


// **************************************************************************
// Function : void vVDecH265CalcBitRate(UCHAR ucEsId)
// Description : calc bitrate 
// Parameter : None
// Return      :  non
// **************************************************************************
void vVDecH265CalcBitRate(UCHAR ucEsId)
{
    UINT32 u4NalSize = 0,u4PicCnt = 0;
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;
    #ifdef CC_DUAL_CORE_DEC
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT32 u4Mode;
    #endif
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);
    #ifdef CC_DUAL_CORE_DEC
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,NULL,NULL);
    #endif

    if(   (prH265DecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN) 
        ||(prH265DecPrm->ucSkipMode != IPBMode)
        #ifdef CC_DUAL_CORE_DEC
        || (u4Mode != REAL_DECODE)
        #endif
      )
    {
        return ;
    }
    
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prH265DecPrm->ucFbgId);
    if (prH265DrvDecInfo->rPesInfo.u4VldReadPtrEnd > prH265DrvDecInfo->rPesInfo.u4VldReadPtr)
    {
        u4NalSize =  (prH265DrvDecInfo->rPesInfo.u4VldReadPtrEnd - prH265DrvDecInfo->rPesInfo.u4VldReadPtr);
    }
    else
    {
        u4NalSize =  ((prH265DrvDecInfo->rPesInfo.u4FifoEnd - prH265DrvDecInfo->rPesInfo.u4VldReadPtr) +
                      (prH265DrvDecInfo->rPesInfo.u4VldReadPtrEnd - prH265DrvDecInfo->rPesInfo.u4FifoStart));
    }
    
    u4PicCnt = u4VDecMPEGFrmMapFrameRate(ucEsId,prFbmSeqHdr->ucFrmRatCod)/100;
    prH265DecPrm->u4PicCnt += IsH265SliceNalUnit(prH265DrvDecInfo->rPesInfo.ucPicType);
    prH265DecPrm->u4PicSize += u4NalSize;

    if(prH265DecPrm->u4PicCnt >= u4PicCnt)
    {
        prH265DecPrm->u4Bitrate = (prH265DecPrm->u4PicSize << 3)/1000;//kbps
        prFbmSeqHdr->u4BitRatVal = prH265DecPrm->u4Bitrate;
        prH265DecPrm->u4PicCnt = 0;
        prH265DecPrm->u4PicSize = 0;
        LOG(2,"Bitrate = %08d kbps PicCnt %d\n",prH265DecPrm->u4Bitrate,u4PicCnt);
    }
}

// **************************************************************************
// Function : void vVDecH265CalcBitRate(UCHAR ucEsId)
// Description : calc QP value 
// Parameter : None
// Return      :  non
// **************************************************************************
void vVDecH265CalcSliceQP(UCHAR ucEsId)
{
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    #ifdef CC_DUAL_CORE_DEC
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT32 u4Mode;
    #endif
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);
    #ifdef CC_DUAL_CORE_DEC
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfoKeep->pfnVdecGetParam(ucEsId,VDEC_STATUS,&u4Mode,NULL,NULL);
    #endif

    if(   (prH265DecPrm->ucFbgId == FBM_FBG_ID_UNKNOWN) 
        ||(prH265DecPrm->ucSkipMode != IPBMode)
        #ifdef CC_DUAL_CORE_DEC
        || (u4Mode != REAL_DECODE)
        #endif
      )
    {
        return ;
    }

    //Update QP per I slice interval
    if(IsH265RAPSlice(prH265DecPrm->u4NalType))
    {
        if(prH265DecPrm->u4ISliceInterval > 0)
        {
            prH265DecPrm->u4SliceQP = prH265DecPrm->u4QPInterval/prH265DecPrm->u4ISliceInterval;
            LOG(2,"Slice QP %d Total/Interval %d/%d\n",prH265DecPrm->u4SliceQP,prH265DecPrm->u4QPInterval,prH265DecPrm->u4ISliceInterval);
        }
        
        prH265DecPrm->u4ISliceInterval = 0;
        prH265DecPrm->u4QPInterval = 0;
    }
    prH265DecPrm->u4ISliceInterval ++;
    prH265DecPrm->u4QPInterval += prH265DecPrm->prSliceHdr->i4SliceQp;
}

#ifdef __MODEL_slt__
BOOL fgSltCompare(UCHAR ucEsId,UCHAR ucVdecId)
{
    UINT32 u4Y[8],u4C[8];
    H265_DRV_INFO_T* prH265DrvDecInfo;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    
    prH265DrvDecInfo = pVDecGetDrvInfo(ucEsId);
    prH265DecPrm = &(prH265DrvDecInfo->rH265DecParam);

    if((prH265DecPrm->u4DecodeOkCnt + prH265DecPrm->u4DecodeErrCnt) == 64)
    {
        vVDecH265GetCRC(ucEsId,ucVdecId,u4Y,u4C,prH265DecPrm);
        if( (u4Y[0] == 0x5A8BA227) &&
            (u4Y[1] == 0xDAF982B1) &&
            (u4Y[2] == 0x2FF892FF) &&
            (u4Y[3] == 0xC6E6674E) &&
            (u4C[0] == 0x28B4141C) &&
            (u4C[1] == 0x181CA91F) &&
            (u4C[2] == 0x46CF4118) &&
            (u4C[3] == 0xAA7C92D3)
            )
        {
            Printf("\n\nMMOK!!\n\n\n");
            return TRUE;
        }
        else
        {
            Printf("\n\nMMNG!!\n\n\n");
            return FALSE;
        }
    }

    return TRUE;
}
#endif

//
