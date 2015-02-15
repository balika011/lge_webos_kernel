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
 * $RCSfile: vdec_h264api.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_h264api.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_timer.h"
#include "x_lint.h"
#include "vdec_h264psr.h"
#include "vdec_h264dec.h"
#include "vdec_h264util.h"
#include "vdec_h264api.h"
#include "vdec_h264hw.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_chip_id.h"
#include "x_common.h"
#include "x_typedef.h"
#include "dmx_if.h"
#ifdef FBM_DPB_CTRL
#include "fbm_drvif.h"
#endif
#ifdef CC_DRIVER_DEMO
#include "vdec_h264hw.h"
#endif
#include "vdec_drvif.h"
#include "drv_dbase.h"
//for user data
#include "mpv_drvif.h"
//for clock setting
#include "x_hal_5381.h"
#include "x_mm_common.h"
#if defined(VDEC_IS_POST_MT5398)
#include "srm_drvif.h"
#endif
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#ifdef MEM_TEST
#define DPB_ADDR 0x679c000
#else
#define DPB_ADDR 0x179c000
#endif
#ifdef TIME_MEASUREMENT
static BOOL _fgVdecTrackIDR1 = TRUE;
static BOOL _fgVdecTrackIDR2 = TRUE;
#endif

#define DPB_SIZE 0xC00000//0x1200000
//actual used DPB_SIZE 0x01680000;    //size (12288*1024)=0xC00000, + cur dec 3M, + cur disp 3M,  *5/4=0x01680000(MV buffer)
#define PRED_SA 0x7ff0000//(DPB_ADDR + ((DPB_SIZE * 5)>>2))         //buffer for VLD, size 32*1024
#define PRED_SA_SZ 0x8000    //size 32*1024

#define MAX_ES_NUM VDEC_MAX_ES
#define ES_ID_0 0

#define IS_NREF_NALU(x) (((x >> 5) & 0x3) == 0)
#define IS_PIC_NALU(x) (((x & 0x1f) <= IDR_SLICE))
#define IPBMode 0x0
#define IPMode 0x1
#define IMode 0x2

#define ATSC_IDENTIFIER_LE  0x34394147
#define EIA_CC    0x03
#define MPEG2_USER_DATA_SC 0x000001b2   // start code

#define FIND_START_CODE_THRD 255//32

#define MAX_VERIFY_FRAME 16
#define WAIT_PPS_SKIP_FRM_NUM 60
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
static VDEC_H264_INFO_T *rH264DecInfo[MAX_ES_NUM] = {NULL};
#else
static VDEC_H264_INFO_T rH264DecInfo[MAX_ES_NUM];
#endif

#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
static BOOL fgIsFrmStart;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef VDEC_H264_PERFORMANCE
void _VDEC_H264Performance(UINT32 ucEsId,UINT32 u4VDecID);
#endif
#ifdef VDEC_H264_CRC_COMPARE
void _VDEC_H264CompCRC(UINT32 ucEsId,UINT32 u4VDecID);
#endif
//-----------------------------------------------------------------------------
// Typedef
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

// **************************************************************************
// Function : INT32 _VDEC_H264Init(UCHAR ucEsId, UINT32 u4VDecID);
// Description :init h264 decode info
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : Init ok
// **************************************************************************
INT32 _VDEC_H264Init(UCHAR ucEsId, UINT32 u4VDecID)
{
    VDEC_H264_INFO_T* prVdec;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    static UINT32 u4PredSA = 0;
    UINT32 i, j;

#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        rH264DecInfo[ucEsId] = (VDEC_H264_INFO_T*)x_mem_alloc_virtual(sizeof(VDEC_H264_INFO_T));
        if(rH264DecInfo[ucEsId] == NULL)
        {
            LOG(0,"Allocate driver info struct Fail\n");
            ASSERT(0);
        }
    }
    prVdec = rH264DecInfo[ucEsId];
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    
    //reset data structure
    x_memset(prVdec, 0, sizeof(VDEC_H264_INFO_T));
    //#ifdef TEST_ONOFF_RUNTIME
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    //#endif
    if(u4PredSA == 0)
    {
        u4PredSA = (UINT32)BSP_AllocAlignedDmaMemory(PRED_SA_SZ, 1024);
        ASSERT(u4PredSA);
        VERIFY(x_msg_q_create(&prVdec->rDecParam.hUsrDataQ, "AVC_UserDataQ",
            sizeof(MPV_USERDATA_CB_T), USERDATA_Q_SIZE) == OSR_OK);
    }
    prVdec->bPicQueue = FALSE;
    prVdec->rDecParam.u4MaxFBufNum = 18;
    prVdecEsInfo->fgSeqHdrH264 = FALSE;
    prVdecEsInfo->fgSwdmxSendEosDone = FALSE;
    prVdec->rDecParam.u1PlayModeInDtv = IPBMode;
    prVdec->rDecParam.fgUseMMErrMask = FALSE;
    prVdec->rDecParam.fgPOCWrap = FALSE;
    prVdec->rDecParam.fgStartPlay = FALSE;
    prVdec->rDecParam.fgIsFirstFrm = FALSE;
    #ifdef VDEC_TIME_PROFILE
    fgIsFrmStart = FALSE;
    #endif
    prVdec->rDecParam.bmvcpair = FALSE;
    //memory layout, should query from FBM
    prVdec->rDecParam.u4PredSA = u4PredSA;    //PRED_SA;   
    prVdec->rDecParam.u4DPBAddr = DPB_ADDR;    //not used if FBM_DPB_CTRL
    prVdec->rDecParam.u4DPBSize = DPB_SIZE;    //not used if FBM_DPB_CTRL
    
    //wait fb counter
    prVdec->rDecParam.u4WaitFbCnt = 0;
    prVdec->rDecParam.fgAllocFbFail = FALSE;
    
    //RR mode
    prVdec->rDecParam.fgRRModeYEn = FALSE;
    prVdec->rDecParam.fgRRModeCEn = FALSE;
    prVdec->rDecParam.u1RRModeY = 0;
    prVdec->rDecParam.u1RRModeC = 0;
    prVdec->rDecParam.u1MaxDisHbands = 3;

    //Display order
    prVdec->rDecParam.fgOutDispOrder = TRUE;
    //prVdec->rDecParam.i4LastPOC = -99999;
    prVdec->rDecParam.u4Dts = 0;
    
    //swdmx will set the param before vdec init
    if((!prVdec->rDecParam.fgIsMMPlay) || (!prVdec->rDecParam.fgEnCalPTS))
    {
        //prVdec->rDecParam.u4DispPts = 0;
        prVdec->rDecParam.u8VRemainder = 0;
        prVdec->rDecParam.u8VRemainder64 = 0;
        prVdec->rDecParam.u8PtsScale = 0;
        prVdec->rDecParam.u8PtsScale64 = 0;
        prVdec->rDecParam.u4Rate = 0;
        prVdec->rDecParam.u4Rate64 = 0;
        prVdec->rDecParam.fgEnCalPTS = FALSE;
    }
    
    //Reset Wptr
    prVdec->rDecParam.u4WPtr = 0xF000000;

    //emulation, repeat frame
    prVdec->rDecParam.u1RptFrm = 0;

    //error cocealment mode
    prVdec->rDecParam.u1ECMode = (UINT8)((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE1 << 2));   //EC to slice start pos
    prVdec->rDecParam.u4ECType = 0xC210;//0x4200;
    
    //dram bandwidth simulation
    prVdec->rDecParam.u4DramMaxLen = 0;
    prVdec->rDecParam.u4DramReqConnDis = 0;
    prVdec->rDecParam.u4DramFifoBank = 0;   //16 bank
    prVdec->rDecParam.fgLogSimPat = FALSE;
    prVdec->rDecParam.u1DramMoniter = 0;
    #if defined(VDEC_CRC_TEST) || defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    #if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    if(VDEC_IsTestCRC(ucEsId))
    #endif
    {
        vVDecEnableCRC(u4VDecID, TRUE, TRUE);
    }
    #endif

    //waiting for 1st IDR
    #ifdef EARLY_DISP
    prVdec->rDecParam.fgWaitFirstIDR = TRUE;
    #endif

    //mm playback
    prVdec->rDecParam.fgIsMMPlay = FALSE;
    prVdec->rDecParam.fgNotSupport = FALSE;
    prVdec->rDecParam.ucSkipMode = IPBMode;
    
    //flush buf info    
    prVdec->rDecParam.u1MaxLongTermFrameIdx = 0xff;
    for(j=0; j<MAX_VIEW_NUM; j++)
    {
        for(i=0; i<18; i++)
        {
            prVdec->rDecParam.arFBufInfo[j][i].ucMvBufIdx = 0xff;
            _VDEC_ClrFBufInfo(&prVdec->rDecParam, i + (j<<8));
        }
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        prVdec->rDecParam.aprCurrFBufInfo[j] = NULL;
        #endif
        prVdec->rDecParam.rLastInfo[j].u4LastFrameNum = 0xffffffff;
        prVdec->rDecParam.rLastInfo[j].u4LastPicW = 0;
        prVdec->rDecParam.rLastInfo[j].u4LastPicH = 0;
        prVdec->rDecParam.rLastInfo[j].u4LastSPos = 0;
        prVdec->rDecParam.rLastInfo[j].fgLastIsRef = FALSE;
        prVdec->rDecParam.rLastInfo[j].u4DAR = 0;
        prVdec->rDecParam.i4LastPOC[j] = -99999;
        prVdec->rDecParam.aucFbgId[j] = FBM_FBG_ID_UNKNOWN;
        if((!prVdec->rDecParam.fgIsMMPlay) || (!prVdec->rDecParam.fgEnCalPTS))
        {
            prVdec->rDecParam.au4DispPts[j] = 0;
            prVdec->rDecParam.au8DispPts[j] = 0;
        }
        //MV buffer array
        for(i=0; i<MAX_FRAME_NUM; i++)
        {
            prVdec->rDecParam.aucEmpMVBufIdxMap[j][i] = (UCHAR)i;
        }
    }
    for(i=0; i<6; i++)
    {        
        prVdec->rDecParam.arRefPicList[i].u4RefPicCnt = 0;
    }

    //flush set data
    for(i=0; i<32; i++)
    {
        prVdec->arSPS[i].fgSPSValid = FALSE;
    }

    for(i=0; i<256; i++)
    {
        prVdec->arPPS[i].fgPPSValid = FALSE;    
    }
    
    prVdec->rDecParam.prSPS = NULL;
    prVdec->rDecParam.prPPS = NULL;
    prVdec->rDecParam.prSliceHdr = NULL;
    prVdec->rDecParam.prCurrFBufInfo = NULL;
    prVdec->rDecParam.rSEI.u1ExtMatchFlg = FALSE;
    prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;
    prVdec->rDecParam.rSEI.u4UserDataWptr = 0;
    prVdec->rDecParam.rSEI.u1AFD = SEI_AFD_UNKONWN;
    prVdec->rDecParam.u4PESUserDataWptr = 0;
    prVdec->rDecParam.rSEI.u1PicStruct = (UINT8)SEI_FRAME_UNKNOWN;
    prVdec->rDecParam.fgDispCurDecPic = FALSE;
    prVdec->rDecParam.ucPreAllocFbId = FBM_FB_ID_UNKNOWN;
    prVdec->rDecParam.bPreAllocFBG = FALSE;
    prVdec->rDecParam.u1DecFBufIdx = 0;
    prVdec->rDecParam.u4PicNalType = 0x00;
    //crc
#ifdef CC_STRESS_TEST_CRC_CHK
    prVdec->rDecParam.fgEnCRC = FALSE;
    x_memset((void*)&prVdec->rDecParam.au4CrcFailCnt, 0, sizeof(prVdec->rDecParam.au4CrcFailCnt));
    prVdec->rDecParam.u4CrcPassCnt = 0;
    prVdec->rDecParam.u4BsFailCnt = 0;
#endif
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    prVdec->rDecParam.fgIsMVC = FALSE;
    prVdec->rDecParam.fgAnchorPic = FALSE;
    prVdec->rDecParam.fgInterView = FALSE;
    prVdec->rDecParam.u4CurViewID = 0;
    prVdec->rDecParam.u1CurViewIdx= 0;
    prVdec->rDecParam.u4WaitClrBufIdx = 0xFF;
#endif 
    prVdec->rDecParam.fgNalNonIdr = TRUE;
    prVdecEsInfoKeep->u4TimeoutCnt = 0;
    x_memset((void*)&prVdecEsInfo->rWFDDecodeErr, 0, sizeof(VDEC_WFD_DECODE_ERROR_T));           
    x_memset((void*)&prVdecEsInfo->rLastWFDDecodeErr, 0, sizeof(VDEC_WFD_DECODE_ERROR_T));        
    //init hw module, fifo address is 0 now
    //_VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
    _VDEC_InitVDecHW(FALSE, FALSE, FALSE, u4VDecID);
    //_VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);

    return (INT32)E_VDEC_OK;
}


// **************************************************************************
// Function : INT32 _VDEC_H264HdrPrsNalu(UCHAR ucVDecID, UCHAR ucEsId);
// Description : SW parse H264 header info
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : parse ok or not support or not ready
// **************************************************************************
INT32 _VDEC_H264HdrPrsNalu(UCHAR ucVDecID, UCHAR ucEsId)
{
    VDEC_H264_INFO_T* prVdec;
    VDEC_ES_INFO_T *prVdecEsInfo;
    UINT8 u1NaluInfo, u1NalRefIdc, u1NalUnitType;
    BOOL fgForbidenZeroBits;
    INT32 i4RetValue = (INT32)E_VDEC_OK;
    static UINT32 u4LastPtr = 0;
    VDEC_PICTYPE_CB_T rPicType;
    #ifdef VDEC_TIME_PROFILE
    HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt;    
    #endif
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    VDEC_FBUF_INFO_T* prCurrFBufInfo;
    VDEC_INFO_T* prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    UINT32 u4CurRptr;
    #ifdef CC_DMX_PES_EXT_BUFFER
    UINT32 u4DataLen1 = 0, u4DataLen2 = 0, u4Wptr = 0, u4Rptr = 0;
    UINT32 u4DataIn = 0;
    #endif
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    UINT32 u4NaluExtData = 0, u4ByteIdx = 0;
    UINT8 u1PrefixLen = 0;
    #endif
    UINT8 u1ViewIdx = 0;
    UINT16 i = 0;
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    
    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));        
    prInfo = &(_prDbase->rMpv[ucEsId]);

    ASSERT(ucEsId < MAX_ES_NUM);
    
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);    
    ASSERT(prVdecEsInfoKeep != NULL);

    prVdec->rDecParam.ucEsId = ucEsId;

    ASSERT(prVdec->rPesInfo.ucEsId < MAX_ES_NUM);
    if(prVdec->rPesInfo.ucEsId >= MAX_ES_NUM)
    {
        LOG(3, "Pes EsId > MAX_NUM\n");
        return (INT32)E_VDEC_PARSE_NOT_READY;
    }
    
    prVdecEsInfo->fgNoFrameBuffer = FALSE;
    u1NaluInfo = prVdec->rPesInfo.ucPicType;            //find start code
    if(u1NaluInfo == 0)
    {
        if(prVdec->rDecParam.fgIsMMPlay)
        {
            return (INT32)E_VDEC_PARSE_NOT_READY;
        }
        else
        {
            _VDEC_FlushDPB(&prVdec->rDecParam);
            return (INT32)E_VDEC_BS_END;
        }
    }
    fgForbidenZeroBits = ((u1NaluInfo >> 7) & 0x01);    // bit 31
    if(fgForbidenZeroBits != 0)
    {
        //vErrInfo(FORBIDEN_ERR);
    }
    u1NalRefIdc = ((u1NaluInfo >> 5) & 0x3);    // bit 30,29
    u1NalUnitType = (u1NaluInfo & 0x1f);        // bit 28,27,26,25,24
    
    prVdec->u1NalRefIdc = u1NalRefIdc;
    prVdec->u1NalUnitType = u1NalUnitType;
    prVdec->rDecParam.u1NalRefIdc = prVdec->u1NalRefIdc;
    prVdec->rDecParam.u1NalUnitType = prVdec->u1NalUnitType;
    
    if (LAE_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        if(prVdec->rDecParam.u1NalUnitType == CODED_SLICE_EXT)
        {
            prVdec->rDecParam.fgIsMVC = TRUE;
            prVdecEsInfo->e3DType = VDEC_3D_MVC;
        }
    }
    prVdec->rDecParam.ucDualCoreMode = prVdecEsInfo->ucDualCoreMode;
    prVdec->rDecParam.ucVDecStatus = prVdecEsInfo->ucVDecStatus;

    #if 0
    //PVR, only parse SPS and SEI without decoding, skip other NALU
    if(prVdecEsInfo && (prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2))
    {
        if((prVdec->u1NalUnitType != SPS) && (prVdec->u1NalUnitType != SEI))
        {
            if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
            {
                if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
                {                    
                    LOG(1, "H264 ES(%d) DMX_UpdVRPtr Fail\n", prVdec->rPesInfo.ucEsId);
                }
                return (INT32)E_VDEC_PARSE_NOT_READY;
            }
        }
    }
    #endif
    // LG add NOT_SUPPORT filter
   if((prVdec->u1NalUnitType < IDR_SLICE) && (prVdec->u1NalUnitType > NON_IDR_SLICE))//Partition A B C D
    {
        if((prVdecEsInfoKeep->fgVPush)&&(prVdec->rDecParam.fgIsMMPlay))
        {
           prVdec->rDecParam.fgNotSupport = TRUE;
           LOG(2, "H264 NAL type(%d) not support.\n", prVdec->u1NalUnitType);
           return (INT32)E_VDEC_NOT_SUPPORT;
        }
    }

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if(prVdec->rDecParam.u4PrefixAddr < 0xFFFFFFFF)
    {
        if(prVdec->rPesInfo.u4VldReadPtr > prVdec->rDecParam.u4PrefixAddr)
        {
            u1PrefixLen = prVdec->rPesInfo.u4VldReadPtr - prVdec->rDecParam.u4PrefixAddr;
        }
        else
        {
            u1PrefixLen = (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoEnd) +
                prVdec->rPesInfo.u4VldReadPtr - prVdec->rDecParam.u4PrefixAddr;
        }
        prVdec->rDecParam.fgIsPrefixLen32Bit = (u1PrefixLen == 7) ? TRUE : FALSE;//7 should be 8?
    }
        
    if(prVdec->u1NalUnitType == PREFIX)
    {
        prVdec->rDecParam.u4PrefixAddr = prVdec->rPesInfo.u4VldReadPtr;
    }
    else
    {
        prVdec->rDecParam.u4PrefixAddr = 0xFFFFFFFF;
    }

    if((prVdec->u1NalUnitType == PREFIX) || (prVdec->u1NalUnitType == CODED_SLICE_EXT))
    {
       // HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prVdec->rPesInfo.u4VldReadPtr), NALU_EXT_SZ); 
        
        for(u4ByteIdx = 0; u4ByteIdx < NALU_EXT_SZ; u4ByteIdx++)
        {
            u4NaluExtData <<= 8;
            //u4NaluExtData += (UINT32)(*(UINT8*)VIRTUAL(prVdec->rPesInfo.u4VldReadPtr));
            u4NaluExtData += (UINT32)prVdec->rPesInfo.au1PicInfo[u4ByteIdx+1];//(*(UINT8*)VIRTUAL(prVdec->rPesInfo.u4VldReadPtr));
            prVdec->rPesInfo.u4VldReadPtr++;
            if(prVdec->rPesInfo.u4VldReadPtr == prVdec->rPesInfo.u4FifoEnd)
            {
                prVdec->rPesInfo.u4VldReadPtr = prVdec->rPesInfo.u4FifoStart;
            }
        }
        prVdec->rDecParam.fgInterView = FALSE;
        if((u4NaluExtData >> 23) == 0)  //mvc
        {
            prVdec->rDecParam.fgNalNonIdr = (u4NaluExtData >> 22);
            prVdec->rDecParam.fgAnchorPic = (u4NaluExtData >> 2) & (0x1);
            prVdec->rDecParam.fgInterView = (u4NaluExtData >> 1) & (0x1);
            prVdec->rDecParam.u4CurViewID = (u4NaluExtData >> 6) & (0x3FF);
            for(u1ViewIdx = 0; u1ViewIdx < MAX_VIEW_NUM; u1ViewIdx++)
            {
                if(prVdec->rDecParam.u4CurViewID == prVdec->rDecParam.au4ViewIDList[u1ViewIdx])
                {
                    prVdec->rDecParam.u1CurViewIdx = u1ViewIdx;
                    break;
                }
            }
            if(u1ViewIdx == MAX_VIEW_NUM)   //view idx > 2
            {
                prVdec->rDecParam.fgAnchorPic = FALSE;
                prVdec->rDecParam.fgInterView = FALSE;
                prVdec->rDecParam.u4CurViewID = 0;
                prVdec->rDecParam.u1CurViewIdx= 0;
                prVdec->rDecParam.fgNalNonIdr = TRUE;
                prVdec->rDecParam.fgIsMVCClkSet = FALSE;
                return (INT32)E_VDEC_PARSE_NOT_READY;
            }
            prVdec->rDecParam.prCurrFBufInfo = 
                prVdec->rDecParam.aprCurrFBufInfo[prVdec->rDecParam.u1CurViewIdx];
            prVdec->rDecParam.u1DecFBufIdx = prVdec->rDecParam.au1DecFBufIdx[prVdec->rDecParam.u1CurViewIdx];
            if(prVdec->rDecParam.prCurrFBufInfo)
            {
                prVdec->rDecParam.pu1DecWorkBufY = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4YAddr);
                prVdec->rDecParam.pu1DecWorkBufC = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4CAddr);
                prVdec->rDecParam.pu1DecWorkBufMV = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4MVBufAddr);
            }
        }
    }
    #endif

    if(prVdecEsInfo && (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
    {
        prVdec->rDecParam.u1CurViewIdx = prVdec->rPesInfo.ucEsId;
        u1ViewIdx = prVdec->rDecParam.u1CurViewIdx;
        if(u1ViewIdx >= MAX_VIEW_NUM)//for klock work issue
        {
            return (INT32)E_VDEC_PARSE_NOT_READY;
        }
        prVdec->rDecParam.u4MaxFBufNum = prVdec->rDecParam.au4MaxFBufNum[prVdec->rDecParam.u1CurViewIdx];
    }
        
    if(u4LastPtr > (prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart))
    {
        LOG(2, "--Nalu type %d, offset 0x%x, wptrOffset 0x%x\n", prVdec->u1NalUnitType, 
            prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart,
            prVdec->rDecParam.u4WPtr - prVdec->rPesInfo.u4FifoStart);
    }
    //u4LastPtr = (prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart);
    if(prVdecEsInfo && prVdecEsInfo->u1PVRWaitSps == 1)
    {         
        for(i=0; i<32; i++)
        {
            prVdec->arSPS[i].fgSPSValid = FALSE;
        }

        for(i=0; i<256; i++)
        {
            prVdec->arPPS[i].fgPPSValid = FALSE;    
        }
        prVdecEsInfo->u1PVRWaitSps = 2;
    }
    
    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeRstS);
    #endif
    if(prVdecEsInfo && prVdec->u1NalUnitType <= CODED_SLICE_EXT)
    {
        _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
    }
    if((prVdec->u1NalUnitType <= IDR_SLICE) || (prVdec->u1NalUnitType == CODED_SLICE_EXT))
    {
        //fix erc mode to slice mode
        if(prVdec->rDecParam.u1SliceCnt == prVdec->rPesInfo.u1SliceCount)
        {
            #ifdef ENABLE_MULTIMEDIA
            if(prVdec->rDecParam.fgIsMMPlay)
            {
                if(prVdec->rDecParam.u1SliceCnt > SLICE_CNT_THRD)
                {
                    //prVdec->rDecParam.u1ECMode = ((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE1 << 2));
                    prVdec->rDecParam.u4ECType = 0x8210;
                }
            }
            else
            {
                if(prVdec->rDecParam.u1SliceCnt > SLICE_CNT_THRD)
                {
                    //prVdec->rDecParam.u1ECMode = ((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE1 << 2));
                    prVdec->rDecParam.u4ECType = 0x8210;
                }
                else
                {
                   //prVdec->rDecParam.u1ECMode = ((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE2 << 2));
                   prVdec->rDecParam.u4ECType = 0xC210;
                }
            }
            #else
            if(prVdec->rDecParam.u1SliceCnt > SLICE_CNT_THRD)
            {
                //prVdec->rDecParam.u1ECMode = ((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE1 << 2));
                prVdec->rDecParam.u4ECType = 0x8210;
            }
            else
            {
               //prVdec->rDecParam.u1ECMode = ((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE2 << 2));
               prVdec->rDecParam.u4ECType = 0xC210;
            }
            #endif
            
        }
        else    //unknown slice cnt
        {
            prVdec->rDecParam.u1ECMode = (UINT8)((ERC_ENABLE | ERC_DED_ERR) | (ERC_MODE2 << 2));
            prVdec->rDecParam.u1SliceCnt = prVdec->rPesInfo.u1SliceCount;
        }    
        #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
        _VDEC_SetBsInfo(BARREL1, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            _VDEC_SetBsInfo(BARREL1, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);
        }
        #else
        _VDEC_SetBsInfo(BARREL1, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            _VDEC_SetBsInfo(BARREL1, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);
        }
        #endif
    }
    else if((prVdec->u1NalUnitType <= END_OF_SEQ) || (prVdec->u1NalUnitType == SUB_SPS))
    {    
        #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
        _VDEC_SetBsInfo(BARREL2, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);        
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            _VDEC_SetBsInfo(BARREL2, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);
        }
        #else
        _VDEC_SetBsInfo(BARREL2, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);        
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            _VDEC_SetBsInfo(BARREL2, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);
        }
        #endif
    }
    else
    {
        // I frame case
        if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)
        {
            if (prVdecEsInfoKeep->pfDecNfyFct)
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_ERROR, 0, 0);                
            }
        }
        if(prVdecEsInfo && (prVdec->u1NalUnitType <= CODED_SLICE_EXT))
        {
            _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        }
        return (INT32)E_VDEC_PARSE_NOT_READY;
    }
    #ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeRstE);
    HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    prVdec->rDecParam.u4CurRstMs += (rTimeRstDt.u4Micros / 1000);
    #endif
    prVdec->rDecParam.rLastInfo[u1ViewIdx].u4LastNaluType = prVdec->u1NalUnitType;
    switch(prVdec->u1NalUnitType)
    {
        case NON_IDR_SLICE:
        case IDR_SLICE:
            #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
            if(prVdec->rDecParam.u4CurViewID != prVdec->rDecParam.au4ViewIDList[0])
            {
                LOG(7, "[warning] prefix missing? Force viewIdx to 0\n");
                prVdec->rDecParam.fgNalNonIdr = TRUE;
                prVdec->rDecParam.fgAnchorPic = FALSE;
                prVdec->rDecParam.fgInterView = TRUE;
                prVdec->rDecParam.u4CurViewID = prVdec->rDecParam.au4ViewIDList[0];
                prVdec->rDecParam.u1CurViewIdx = 0;
                prVdec->rDecParam.prCurrFBufInfo = 
                        prVdec->rDecParam.aprCurrFBufInfo[prVdec->rDecParam.u1CurViewIdx];
                prVdec->rDecParam.u1DecFBufIdx = prVdec->rDecParam.au1DecFBufIdx[prVdec->rDecParam.u1CurViewIdx];
                if(prVdec->rDecParam.prCurrFBufInfo)
                {
                    prVdec->rDecParam.pu1DecWorkBufY = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4YAddr);
                    prVdec->rDecParam.pu1DecWorkBufC = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4CAddr);
                    prVdec->rDecParam.pu1DecWorkBufMV = (UINT8*)(prVdec->rDecParam.prCurrFBufInfo->u4MVBufAddr);
                }
            }
            #endif
        #ifndef CC_SW_MVC_ENABLE
        case CODED_SLICE_EXT:
        #endif
            prVdec->rDecParam.fgSkipCurFrm = FALSE;
            if(prVdec->rDecParam.u1RptFrm == 1)
            {
                prVdec->rDecParam.u1RptFrm = 2;
            }
            _VDEC_SliceNonIDR((UINT32)ucVDecID, prVdec, &i4RetValue, ucEsId);

            if(i4RetValue == E_VDEC_FRAME_RATE_NOT_SUPPORT)
            {
                return (INT32)E_VDEC_FRAME_RATE_NOT_SUPPORT;
            }
            else if(i4RetValue == E_VDEC_ALLOCATEFB)
            {
                _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
                //error handling
                //clr current frame status if still in the middle of decoding process
                if(prVdec->rDecParam.prCurrFBufInfo)
                {
                    if(FBM_GetFrameBufferStatus(prVdec->rDecParam.aucFbgId[u1ViewIdx], 
                        prVdec->rDecParam.prCurrFBufInfo->ucFbId) == FBM_FB_STATUS_DECODE)
                    {
                        FBM_SetFrameBufferStatus(prVdec->rDecParam.aucFbgId[u1ViewIdx], 
                            prVdec->rDecParam.prCurrFBufInfo->ucFbId, FBM_FB_STATUS_READY);
                    }
                }
                
                prVdec->rDecParam.ucPreAllocFbId = 
                    FBM_GetEmptyFrameBuffer(prVdec->rDecParam.aucFbgId[u1ViewIdx], prVdec->rDecParam.u4WaitFbTime);

                if(prVdec->rDecParam.ucPreAllocFbId == FBM_FB_ID_UNKNOWN)
                {
                   LOG(2, "[warning]No Empty Frame\n");
                   prVdecEsInfo->fgNoFrameBuffer = TRUE;
                   return E_VDEC_PARSE_NOT_READY;
                }

                if((prVdec->rDecParam.ucPreAllocFbId != FBM_FB_ID_UNKNOWN) &&
                    prVdec->rPesInfo.ucEsId < MAX_ES_NUM)
                {
                    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
                    #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
                    _VDEC_SetBsInfo(BARREL1, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);
                    if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
                    {
                        _VDEC_SetBsInfo(BARREL1, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);
                    }
                    #else
                    _VDEC_SetBsInfo(BARREL1, ucVDecID, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);
                    if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
                    {
                        _VDEC_SetBsInfo(BARREL1, VDEC_VLD1, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);
                    }
                    #endif
                    _VDEC_SliceNonIDR((UINT32)ucVDecID, prVdec, &i4RetValue,ucEsId);
                }
            }
            //record slice reference type for new picture detect
            prVdec->rDecParam.rLastInfo[u1ViewIdx].fgLastIsRef = fgIsRefPic(prVdec->rDecParam.u1NalRefIdc);
            //save CC data            
            if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
            {
                prCurrFBufInfo = prVdec->rDecParam.prCurrFBufInfo;
                if(prCurrFBufInfo && (prCurrFBufInfo->u1FBufStatus != NO_PIC) && 
                    ((prVdec->rDecParam.rSEI.u4UserDataWptr > 0) ||
                    (prVdec->rDecParam.u4PESUserDataWptr > 0)))
                {
                #ifdef CC_SEI_USER_DATA_ENABLE
                    if(prVdec->rDecParam.rSEI.u4UserDataWptr > 0)
                    {
                        if((prCurrFBufInfo->u4UserDataFrmWptr + prVdec->rDecParam.rSEI.u4UserDataWptr) < MAX_USERDATA_SIZE)
                        {
                            x_memcpy((void*)&prCurrFBufInfo->au1UsrDataFrm[prCurrFBufInfo->u4UserDataFrmWptr], 
                                (void*)prVdec->rDecParam.rSEI.au1UsrData, 
                                prVdec->rDecParam.rSEI.u4UserDataWptr);
                                prCurrFBufInfo->u4UserDataFrmWptr += prVdec->rDecParam.rSEI.u4UserDataWptr; // for field encoded
                        }
                        else
                        {
                            LOG(3, "Frame-based SEI_CC_Len > %d", MAX_USERDATA_SIZE);
                        }
                    }
                #endif
                #ifdef CC_DMX_PES_EXT_BUFFER
                    if(prVdec->rDecParam.u4PESUserDataWptr > 0)
                    {
                        if((prCurrFBufInfo->u4UserDataPESWptr + prVdec->rDecParam.u4PESUserDataWptr) < MAX_USERDATA_SIZE)
                        { 
                            x_memcpy((void*)&prCurrFBufInfo->au1UsrDataPES[prCurrFBufInfo->u4UserDataPESWptr], 
                                (void*)prVdec->rDecParam.au1PESUsrData, 
                                prVdec->rDecParam.u4PESUserDataWptr);
                                prCurrFBufInfo->u4UserDataPESWptr += prVdec->rDecParam.u4PESUserDataWptr; // for field encoded
                        }
                        else
                        {
                            LOG(3, "Frame-based PES_EXT_Len > %d", MAX_USERDATA_SIZE);
                        }
                    }
                #endif 
                    prVdec->rDecParam.rSEI.u4UserDataWptr = 0;
                    prVdec->rDecParam.u4PESUserDataWptr = 0;
                }

                prVdec->rDecParam.u4PicNalType = 0x00;
                //find access unit start 7.4.1.2.3, start dec last picture queue, and restore pes info in queue
                prInfo->u4VpsrDropNs++;
                if(prVdecEsInfoKeep->rPesInfo.fgEos &&(prVdec->rDecParam.ucPreAllocFbId != FBM_FB_ID_UNKNOWN))
                {
                    FBM_SetFrameBufferStatus( prVdec->rDecParam.aucFbgId[u1ViewIdx],  prVdec->rDecParam.ucPreAllocFbId,
                                                    FBM_FB_STATUS_EMPTY);
                }
                if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
                {                    
                    LOG(1, "H264 ES(%d) DMX_UpdVRPtr Fail\n", prVdec->rPesInfo.ucEsId);
                }
                if(i4RetValue == E_VDEC_ALLOCATEFB) //can't get FB
                {
                    return (INT32)E_VDEC_PARSE_NOT_READY;
                }
            }
            break;
        case SEI:
            _VDEC_SEIrbsp((UINT32)ucVDecID, prVdec, &i4RetValue);
            prVdec->rDecParam.u4PicNalType |= (NAL_SEI);
            break;
    #if defined(CC_3D_MM_DS_SUPPORT) && !defined(CC_SW_MVC_ENABLE)
        case SUB_SPS:   //todo, error handling in error bitstream
    #endif            
        case SPS:
            LOG(3, "//H264 SPS Esid=%d\n", ucEsId);
            #ifdef TIME_MEASUREMENT
            if(_fgVdecTrackIDR1)
            {
                 TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "Vdec_REC_SPS");
                 TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "Vdec_REC_SPS");
                _fgVdecTrackIDR1=FALSE;
            }
            #endif
            prVdec->fgisvalid = FALSE;
            _VDEC_SPSrbsp((UINT32)ucVDecID, prVdec, &i4RetValue);
            prVdec->rDecParam.u4PicNalType |= (NAL_SPS);
            if((prVdec->fgisvalid == TRUE)&&(i4RetValue != 0)&&(prVdec->u1SPSIDBK < 32))
            {
                x_memcpy((void*)&prVdec->arSPS[prVdec->u1SPSIDBK], (void*)&prVdec->rSPSBK, sizeof(VDEC_AVC_SPS_T));
            }
            if((prVdec->rDecParam.aucFbgId[u1ViewIdx] == FBM_FBG_ID_UNKNOWN) && (prVdecEsInfo->fgParsingInfo))
            {
                if(_vCheckAllocFBG(&prVdec->rDecParam))
                {
                    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdec->rDecParam.aucFbgId[u1ViewIdx]);
                    if(prFbmSeqHdr)
                    {
                        prFbmSeqHdr->u2HSize = (UINT16)prVdec->rDecParam.u4PicW;
                        prFbmSeqHdr->u2VSize = (UINT16)prVdec->rDecParam.u4PicH;
                        prFbmSeqHdr->fgCropping = prVdec->rDecParam.prSPS->fgFrameCroppingFlag;
                        prFbmSeqHdr->u4CropX = prVdec->rDecParam.prSPS->u4FrameCropLeftOffset;
                        prFbmSeqHdr->u4CropY = prVdec->rDecParam.prSPS->u4FrameCropTopOffset;
                        prFbmSeqHdr->u4CropWidth = (prVdec->rDecParam.u4PicW - prFbmSeqHdr->u4CropX) -
                                                                    prVdec->rDecParam.prSPS->u4FrameCropRightOffset;
                        prFbmSeqHdr->u4CropHeight = (prVdec->rDecParam.u4PicH - prFbmSeqHdr->u4CropY -
                                     ((prVdec->rDecParam.prSPS->fgFrameMbsOnlyFlag) ?
                                      prVdec->rDecParam.prSPS->u4FrameCropBottomOffset :
                                      (prVdec->rDecParam.prSPS->u4FrameCropBottomOffset << 1)));
                        LOG(1, "FBG H/W %dx%d\n", prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize);
                        if(prFbmSeqHdr->fgCropping)                        
                            LOG(1, "Crop: %d %d %d %d\n", prFbmSeqHdr->u4CropX, prFbmSeqHdr->u4CropY, prFbmSeqHdr->u4CropWidth, prFbmSeqHdr->u4CropHeight);
                    }
                    else
                    {
                        LOG(1, "FBG H/W error\n");
                    }
                }
                else
                {
                    LOG(1, "Alloc FBG fail\n");
                }
            }
            break;      
        case PPS:            
            LOG(3, "//H264 PPS Esid=%d\n", ucEsId);
            #ifdef TIME_MEASUREMENT
            if(_fgVdecTrackIDR2)
            {
                TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME, "Vdec_REC_PPS");
                 TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "Vdec_REC_PPS");
                _fgVdecTrackIDR2=FALSE;
            }
            #endif
            prVdec->fgisvalid = FALSE;
            _VDEC_PPSrbsp((UINT32)ucVDecID, prVdec, &i4RetValue);
            prVdec->rDecParam.u4PicNalType |= (NAL_PPS);
            if((prVdec->fgisvalid== TRUE)&&(i4RetValue != 0))
            {
                x_memcpy((void*)&prVdec->arPPS[prVdec->u1PPSIDBK], (void*)&prVdec->rPPSBK, sizeof(VDEC_AVC_PPS_T));
            }
            else
            {
                if (prVdecInfo->pfPicTypeCb)
                {
                    x_memset(&rPicType, 0, sizeof(VDEC_PICTYPE_CB_T));
                    rPicType.u1EsId = ucEsId;
                    rPicType.u4Pts = prVdec->rPesInfo.u4PTS;
                    rPicType.u1PicType = (UINT8)VDEC_PIC_TYPE_SEQ;
                    prVdecInfo->pfPicTypeCb(&rPicType);
                }
            }
            break;
        case AU_DELIMITER: 
            break;
        case END_OF_SEQ:          
            if ((VDEC_LAE0 != ucVDecID) && (VDEC_LAE1 != ucVDecID))
            {
                LOG(6, "//Nalu END_OF_SEQ\n");
                _VDEC_FlushDPB(&prVdec->rDecParam);
            }
            break;            
        default:
            // I frame case
            if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)
            {
                if (prVdecEsInfoKeep->pfDecNfyFct)
                {
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_ERROR, 0, 0);                
                }
            }
            i4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
            break;
    }

    #ifdef CC_DMX_PES_EXT_BUFFER
    u4Wptr = prVdec->rPesInfo.u4PesExtBufCurrentWp;
    u4Rptr = prVdec->rPesInfo.u4PesExtBufCurrentRp;
    
    ASSERT((u4Rptr >= prVdec->rPesInfo.u4PesExtBufStart) &&
        (u4Rptr <= prVdec->rPesInfo.u4PesExtBufEnd));
    ASSERT((u4Wptr >= prVdec->rPesInfo.u4PesExtBufStart) &&
        (u4Wptr <= prVdec->rPesInfo.u4PesExtBufEnd));
    
    if(u4Rptr > 0)
    {
        u4Rptr += 4;    // skip MPEG2 SC
        if(u4Rptr > prVdec->rPesInfo.u4PesExtBufEnd)
        {
            u4Rptr -= (prVdec->rPesInfo.u4PesExtBufEnd - prVdec->rPesInfo.u4PesExtBufStart);
        }
    
        if(u4Wptr > u4Rptr)
        {
            u4DataLen1 = u4Wptr - u4Rptr;
        }
        else
        {
            u4DataLen1 = prVdec->rPesInfo.u4PesExtBufEnd - u4Rptr;
            u4DataLen2 = u4Wptr - prVdec->rPesInfo.u4PesExtBufStart;
        }

        if((prVdec->rDecParam.u4PESUserDataWptr + (u4DataLen1 + u4DataLen2)) >= MAX_USERDATA_SIZE)
        {
            LOG(1, "UsrData in PES len %d > max %d\n", prVdec->rDecParam.u4PESUserDataWptr, MAX_USERDATA_SIZE);
            prVdec->rDecParam.u4PESUserDataWptr = 0;
        }
        else if((u4DataLen1 + u4DataLen2) > 5)  // ATSC_IDENT + EIA_CC
        {
            if(u4DataLen2 == 0)
            {
                x_memcpy((void*)VIRTUAL((UINT32)&prVdec->rDecParam.au1PESUsrData[prVdec->rDecParam.u4PESUserDataWptr]),
                    (void*)VIRTUAL(u4Rptr),
                    u4DataLen1);
            }
            else
            {
                x_memcpy((void*)VIRTUAL((UINT32)&prVdec->rDecParam.au1PESUsrData[prVdec->rDecParam.u4PESUserDataWptr]),
                    (void*)VIRTUAL(u4Rptr),
                    u4DataLen1);
                x_memcpy((void*)VIRTUAL((UINT32)&prVdec->rDecParam.au1PESUsrData[prVdec->rDecParam.u4PESUserDataWptr + u4DataLen1]),
                    (void*)VIRTUAL(prVdec->rPesInfo.u4PesExtBufStart),
                    u4DataLen2);
            }
            //verify if user data
            u4DataIn = *((UINT32*)&prVdec->rDecParam.au1PESUsrData[prVdec->rDecParam.u4PESUserDataWptr]);
            if((u4DataIn == ATSC_IDENTIFIER_LE) &&
                (prVdec->rDecParam.au1PESUsrData[prVdec->rDecParam.u4PESUserDataWptr + 4] == EIA_CC))
            {
                prVdec->rDecParam.u4PESUserDataWptr += (u4DataLen1 + u4DataLen2);
            }
        }
        DMX_UpdateExtBufferReadPointer(prVdec->rPesInfo.ucEsId, u4Wptr, u4Rptr);
    }
    #endif

    if(i4RetValue != (INT32)E_VDEC_OK)
    {
        prVdecInfo->arVdecEsInfo[ucEsId].arRcvPesCnt[VDEC_FMT_H264]--;
    }
    else if((i4RetValue == (INT32)E_VDEC_OK) && 
        ((prVdec->u1NalUnitType > IDR_SLICE) && (prVdec->u1NalUnitType != CODED_SLICE_EXT)))
    {
        i4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
        //I frame case
        if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)
        {
            prVdec->rPesInfo.ucPicType = _VDEC_AVCGetStartCode((UINT32)ucVDecID, BARREL1);
            _VDEC_GetRptr((UINT32)ucVDecID, &u4CurRptr);
            prVdec->rPesInfo.u4VldReadPtr = u4CurRptr;
            LOG(6,"\n [h264]:Find Start Code(1): 0x%x\n", prVdec->rPesInfo.ucPicType);
            VDEC_SendEs(&prVdec->rPesInfo);
        }
    }

    prVdec->rDecParam.rLastInfo[u1ViewIdx].u4LastSPos = prVdec->rPesInfo.u4VldReadPtr;    
    if(prVdecEsInfo && (i4RetValue != (INT32)E_VDEC_OK) && (i4RetValue != (INT32)E_VDEC_OK_RES_CHG))
    {
        if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
        {
            _VDEC_H264UpdDmxRptr((UINT32)ucVDecID, ucEsId);
        }
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    }
    return i4RetValue;
}


// **************************************************************************
// Function : INT32 _VDEC_H264DecStart(UCHAR ucVDecID, UCHAR ucEsId);
// Description : Set register & trigger HW decode
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : Trigger ok
// **************************************************************************
INT32 _VDEC_H264DecStart(UCHAR ucVDecID, UCHAR ucEsId)
{
    VDEC_H264_INFO_T* prVdec;
    INT32 i4RetValue;
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif
        
    if (LAE_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        _VDEC_LAEDecProc((UINT32)ucVDecID, &prVdec->rDecParam, &i4RetValue);        
    }    
    else
    {
        _VDEC_DecProc((UINT32)ucVDecID, &prVdec->rDecParam, &i4RetValue);
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            _VDEC_DecProc(VDEC_VLD1, &prVdec->rDecParam, &i4RetValue);
        }
    }
    if ((DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode) && (REAL_DECODE == prVdec->rDecParam.ucVDecStatus))
    {
        _VDEC_TriggerDualCore((UINT32)ucVDecID, &prVdec->rDecParam);
    }
    else
    {       
        _VDEC_TriggerDecode((UINT32)ucVDecID);
    }
    if(i4RetValue != (INT32)E_VDEC_OK)
    {
       if(ucEsId < MAX_ES_NUM)
       {
           if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
           {
               _VDEC_H264UpdDmxRptr((UINT32)ucVDecID, ucEsId);
           }
       }
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    }
    
    return i4RetValue;
}


// **************************************************************************
// Function : INT32 _VDEC_H264DecFin(UCHAR ucVDecID, UCHAR ucEsId);
// Description : Wait decode done & output pic to displayQ
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      :  decode finish
// **************************************************************************
INT32 _VDEC_H264DecFin(UCHAR ucVDecID, UCHAR ucEsId)
{
    VDEC_H264_INFO_T* prVdec;
    UINT8 u1Ret;
    INT32 i4RetValue;
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    UINT32 u4CurRptr, u4ErrAcc;    
    BOOL fgIsUFOMode = FALSE;
    BOOL fgIsDualCoreMode = FALSE;    
#ifdef CC_DUAL_CORE_DEC
    UCHAR ucMainEsId;
#endif
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif
    
    if(prVdec->rDecParam.u1RptFrm == 0)
    {
        _VDEC_DoRefPicMarking(&prVdec->rDecParam);
    }
    prVdec->rDecParam.rSEI.u1AFD = SEI_AFD_UNKONWN;

    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE) && defined(CC_ALLEGRO_MVC)
    //FUNNY_A_S00_5x6_r1.0, frm103, 321
    if((prVdec->rDecParam.u1CurViewIdx > 0) &&
        (!prVdec->rDecParam.fgNalNonIdr))
    {
        _VDEC_FlushDPB(&prVdec->rDecParam);
    }
    #endif
    
    if (LAE_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        prVdec->rDecParam.bWaitDecLock = TRUE;
    }
    if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
    {       
        _VDEC_OutputDispPic((UINT32)ucVDecID, &prVdec->rDecParam, &i4RetValue);    //vChkOutputFBuf
    }

    if((u1Ret = _VDEC_WaitDecDone(ucEsId, (UINT32)ucVDecID, &prVdec->rDecParam)) > 0)
    {
        if(u1Ret > 1)
        {
#ifdef CC_IPTV_SUPPORT
            UINT32 u4TmpResult;
            u4TmpResult = u4VDecReadAVCVLD((UINT32)ucVDecID, RO_AVLD_ERR_ACC); 
#endif
            LOG(8, "//DecFin - SliceType %d, NaluOft %d\n", prVdec->rSliceHdr.u4SliceType, 
            prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart);
            // timeout, u1Ret = 2
            if(prVdecEsInfoKeep->fgVPush)
            {
                //prVdec->rDecParam.prCurrFBufInfo->fgIsNotDisp = TRUE;
                LOG(1, "Ignore Timeout, SetNotDisp\n");
                #if BDP_LOG_STYLE
                VDecPrintf(ucEsId, VDEC_LOG_DROPPIC, 
                                 "[H264 VDEC%d]: _VDEC_H264DecFin PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                 ucEsId,
                                 prVdec->rDecParam.prCurrFBufInfo->i4PicNum, 
                                 prVdecEsInfoKeep->rPesInfo.ucPicType,
                                 prVdecEsInfoKeep->rPesInfo.u4PTS);
                #endif
            }
#ifdef ENABLE_MULTIMEDIA
#ifdef CC_IPTV_SUPPORT
            if(prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV)            
            &&
            ((u1Ret == 2)||
            ((((u4TmpResult & ERR_MASK_OBSV) != 0)))))
            {
                prVdec->rDecParam.prCurrFBufInfo->fgIsNotDisp = TRUE;
                if(fgIsRefPic(prVdec->rDecParam.u1NalRefIdc)
                &&(prVdec->rDecParam.ucSkipMode == IPBMode))
                {
                    LOG(1,"IPTV Find Timeout Ref Pic in IPB mode, Restart play\n");
                    _VDEC_FlushDPB(&prVdec->rDecParam);
                    _VDEC_ClrDPB(&prVdec->rDecParam);
                    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
                }
            }
#endif
#endif
        }
        if (LAE_DECODE == prVdec->rDecParam.ucVDecStatus)
        {
            i4RetValue = (UINT32)E_VDEC_LAE_OK;
        }
        else
        {
            i4RetValue = (INT32)E_VDEC_OK;
        }
    }
    else
    {
        i4RetValue = (INT32)E_VDEC_FAIL; //Timeout, decode error
    }
    
    #ifdef CC_DUAL_CORE_DEC
    if (LAE_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        //Send LAE decode buffer to hMsgQLaeOuput for Real_deocde usage
        ucMainEsId = ucVDecGetMcoreDecInst(ucEsId);
        prVdec->rDecParam.rLAEDecParam.u4PesRdptr = prVdec->rPesInfo.u4VldReadPtr;
        vPutBufferToMcore(ucMainEsId,&(prVdec->rDecParam.rLAEDecParam));
        //prVdec->rDecParam.rLastInfo[prVdec->rDecParam.u1CurViewIdx].u4LastPicW = prVdec->rDecParam.u4PicW;        
        //prVdec->rDecParam.rLastInfo[prVdec->rDecParam.u1CurViewIdx].u4LastPicH = prVdec->rDecParam.u4PicH;
    }
    else
    #endif
    {       
    #ifdef CC_DUAL_CORE_DEC
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {                 
            vPutBufferToLae(prVdecEsInfo->ucLaeEsInfoId, &(prVdec->rDecParam.rLAEDecParam));
        }
    #endif		
#ifdef VDEC_H264_PERFORMANCE
		_VDEC_H264Performance(ucEsId, (UINT32)ucVDecID);
		VDecPrintf(ucEsId, VDEC_LOG_DecTime, "Hw dec-time %d MC_478 %d\n",
					u4VDecReadVLDTOP(ucVDecID,(40 << 2)), u4VDecReadMC(ucVDecID, (478 << 2)));
#endif
#ifdef VDEC_H264_CRC_COMPARE
		_VDEC_H264CompCRC(ucEsId, ucVDecID);
#endif
        _VDEC_FrmFin((UINT32)ucVDecID, &prVdec->rDecParam);

        //error notify
        u4ErrAcc = u4VDecReadAVCVLD((UINT32)ucVDecID, RO_AVLD_ERR_ACC)& ERR_MASK_MM;
        if(prVdec->rDecParam.prCurrFBufInfo->fgIsNotDisp ||
            (u4ErrAcc > 0))
        {
            prVdecEsInfo->fgDecodeErr = TRUE;
            prVdecEsInfo->u4DecErrMbCnt=u4VDecReadMISC(ucVDecID, RO_VDEC_MISC_MB_ERR_CNT);
        }

#ifdef ENABLE_MULTIMEDIA
#ifdef CC_IPTV_SUPPORT
        u4ErrAcc = u4VDecReadAVCVLD((UINT32)ucVDecID, RO_AVLD_ERR_ACC);                        
        if(prVdecEsInfo && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV) && ((u4ErrAcc & ERR_MASK_OBSV) != 0))
        {
            prVdec->rDecParam.prCurrFBufInfo->fgIsNotDisp = TRUE;
            LOG(7, "[%s-%d]: Drop error u4TmpResult(%d).\n", __FUNCTION__, __LINE__, u4ErrAcc);
        }
#endif
#endif
        //I frame case
        if(prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
        {
            if((prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus == TOP_FIELD) ||
                (prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus == BOTTOM_FIELD))
            {
                prVdec->rPesInfo.ucPicType = _VDEC_AVCGetStartCode((UINT32)ucVDecID, BARREL1);
                _VDEC_GetRptr((UINT32)ucVDecID, &u4CurRptr);
                prVdec->rPesInfo.u4VldReadPtr = u4CurRptr;
                LOG(6,"\n [h264]:Find Start Code(1): 0x%x\n", prVdec->rPesInfo.ucPicType);
                VDEC_SendEs(&prVdec->rPesInfo);            
            }
        }
        
        if(prVdec->rPesInfo.fgEos)
        {
            _VDEC_FlushDPB(&prVdec->rDecParam);
            if(prVdec->rDecParam.fgIsMVC)
            {
                prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
                _VDEC_FlushDPB(&prVdec->rDecParam);
                prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
            }
        }
    } 
#if defined(CC_MT5890) //for oryx E1 error concealment
    prVdec->rDecParam.u1PreFBIndex = prVdec->rDecParam.u1DecFBufIdx;
#endif
    if(ucEsId < MAX_ES_NUM)
    {
        if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
        {
            _VDEC_H264UpdDmxRptr((UINT32)ucVDecID, ucEsId);
        }
    }
    #ifdef TEST_ONOFF_RUNTIME
    if(i4RetValue != (INT32)E_VDEC_OK)
    {
    #endif
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            fgIsDualCoreMode = TRUE;
        }
        if (prVdec->rDecParam.fgIsUFOMode)
        {
            fgIsUFOMode = TRUE;
        }
        _VDEC_ResetHW(fgIsUFOMode, fgIsDualCoreMode, (UINT32)ucVDecID);
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    #ifdef TEST_ONOFF_RUNTIME
    }
    #endif

    return i4RetValue;
}


// **************************************************************************
// Function : void _VDEC_H264SWRst(UCHAR ucVdecId, UCHAR ucEsId);
// Description : SW reset used for demo
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void _VDEC_H264SWRst(UCHAR ucVdecId, UCHAR ucEsId)
{    
    #ifdef CC_DRIVER_DEMO
    VDEC_H264_INFO_T* prVdec;
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif
    _VDEC_LogSimPat(ucVdecId, prVdec->rDecParam.fgLogSimPat);
    #endif
    UNUSED(ucVdecId);
    UNUSED(ucEsId);
}


// **************************************************************************
// Function : void _VDEC_H264Stop(UCHAR ucEsId);
// Description : mw stop vdec
// Parameter : ucEsId : instance id
// Return      : None
// **************************************************************************
void _VDEC_H264Stop(UCHAR ucEsId)
{
    UINT32 u4Idx;
    VDEC_H264_INFO_T* prVdec;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;    
#ifdef CC_DUAL_CORE_DEC
    SIZE_T zMsgSize;
    UINT16 u2MsgQIdx;    
    VDEC_LAE_DecPrm rLAEDecPrm;
#endif
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);    
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    prVdec->rDecParam.u8VRemainder = 0;
    prVdec->rDecParam.u8VRemainder64 = 0;
    prVdec->rDecParam.u8PtsScale = 0;
    prVdec->rDecParam.u8PtsScale64 = 0;
    prVdec->rDecParam.u4Rate = 0;
    prVdec->rDecParam.u4Rate64 = 0;
    prVdec->rDecParam.fgEnCalPTS = FALSE;
    prVdec->rDecParam.rSEI.u1ExtMatchFlg = FALSE;
    prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;
    prVdec->rDecParam.rSEI.u4UserDataWptr = 0;
    prVdec->rDecParam.rSEI.u1PicStruct = (UINT8)SEI_FRAME_UNKNOWN;
    prVdec->rDecParam.fgIsFirstFrm = FALSE;
    prVdec->rDecParam.fgIsMMPlay = FALSE;
    prVdec->rDecParam.fgNotSupport = FALSE;
    prVdec->rDecParam.u4PESUserDataWptr = 0;
    prVdec->rDecParam.fgSeamlessDisp = FALSE;
    prVdec->rDecParam.u1PlayModeInDtv = IPBMode;
    prVdec->rDecParam.fgIsPVR = FALSE;
    prVdec->rDecParam.ucBNum = 0;
    
    prVdec->rDecParam.fgNalNonIdr = TRUE;
    prVdecEsInfo->fgIsMVCPtsCmp = FALSE;
    prVdecEsInfo->fgRsvSpsMvc = FALSE;
    prVdecEsInfo->fgRsvSps = FALSE;
    prVdecEsInfo->fgRsvData = FALSE;
    prVdecEsInfo->fgSwdmxSendEosDone = FALSE;
    prVdecEsInfo->fgLAEBufAllocated = FALSE;
    if(prVdec->rDecParam.prSPS)
    {
        prVdec->rDecParam.prSPS->fgSPSValid = FALSE;
    }

    for(u4Idx=0; u4Idx<MAX_VIEW_NUM; u4Idx++)
    {
        if(prVdec->rDecParam.aucFbgId[u4Idx] != FBM_FBG_ID_UNKNOWN)
        {
            prVdec->rDecParam.u1CurViewIdx = u4Idx;
            _VDEC_FlushDPB(&prVdec->rDecParam);
            
            if(prVdec->rDecParam.fgIsMVC)
            {
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, u4Idx);
                FBM_WaitUnlockFrameBuffer(prVdec->rDecParam.aucFbgId[u4Idx],WAIT_DISP_UNLOCK_TIME);
            }
            else
            {
                FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
                FBM_WaitUnlockFrameBuffer(prVdec->rDecParam.aucFbgId[u4Idx],WAIT_DISP_UNLOCK_TIME);
            }
            FBM_ReleaseGroup(prVdec->rDecParam.aucFbgId[u4Idx]);
            if (prVdecEsInfo->ucFbgId == prVdec->rDecParam.aucFbgId[u4Idx])
            {
                prVdecEsInfo->ucFbgId = FBM_FBG_ID_UNKNOWN;
            }
            prVdec->rDecParam.aucFbgId[u4Idx] = FBM_FBG_ID_UNKNOWN;
        }
        prVdec->rDecParam.bmvcpair = FALSE;
        prVdec->rDecParam.au4DispPts[u4Idx] = 0;
        prVdec->rDecParam.au8DispPts[u4Idx] = 0;
    }
    if(prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);         
        FBM_WaitUnlockFrameBuffer(prVdecEsInfo->ucFbgId,WAIT_DISP_UNLOCK_TIME);
        FBM_ReleaseGroup(prVdecEsInfo->ucFbgId);
        prVdecEsInfo->ucFbgId = FBM_FBG_ID_UNKNOWN;
    }
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    //move after _VDEC_FlushDPB
    #if defined(CC_MT5395)
    if(prVdec->rDecParam.fgIsMVCClkSet)
    {
        UINT32 u4Clk;
        u4Clk = IO_READ32(CKGEN_BASE, 0x224);
        IO_WRITE32(CKGEN_BASE, 0x224, (u4Clk & 0xFFFFFFF0) | prVdec->rDecParam.u1PrevVLDClk);   //mt5395 MVC 288MHz

        u4Clk = IO_READ32(CKGEN_BASE, 0x228);
        IO_WRITE32(CKGEN_BASE, 0x228, (u4Clk & 0xFFFFFFF0) | prVdec->rDecParam.u1PrevMCClk);   //mt5395 MVC 288MHz
        prVdec->rDecParam.fgIsMVCClkSet = FALSE;
    }
    #endif
    prVdec->rDecParam.fgAnchorPic = FALSE;
    prVdec->rDecParam.fgInterView = FALSE;
    prVdec->rDecParam.u4CurViewID = 0;
    prVdec->rDecParam.u4WaitClrBufIdx = 0xFF;
    #endif
    prVdec->rDecParam.fgIsMVC = FALSE;
    prVdec->rDecParam.u1CurViewIdx= 0;

    x_memset((void*)&prVdecEsInfo->rWFDDecodeErr, 0, sizeof(VDEC_WFD_DECODE_ERROR_T));
    x_memset((void*)&prVdecEsInfo->rLastWFDDecodeErr, 0, sizeof(VDEC_WFD_DECODE_ERROR_T));
    prVdecEsInfoKeep->u4TimeoutCnt = 0;

#ifdef CC_DUAL_CORE_DEC
    if(prVdecEsInfo->ucVDecStatus == LAE_DECODE)
    {
        //Flush LAE Q 
        //clear LAE buffer input Q
        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        while (x_msg_q_receive(&u2MsgQIdx, (void*)&rLAEDecPrm, &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeInput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
        }
    }
    if (prVdecEsInfo->ucDualCoreMode)
    {
        zMsgSize = sizeof(VDEC_LAE_DecPrm);
        //clear LAE buffer output Q
        while (x_msg_q_receive(&u2MsgQIdx, (void*)&rLAEDecPrm, &zMsgSize,
                &(prVdecEsInfoKeep->hMsgQLaeOuput), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
        {
        }
        prVdecEsInfo->ucDualCoreMode = FALSE;
    }
#endif
}


// **************************************************************************
// Function : void _VDEC_H264Flush(UCHAR ucEsId, BOOL bIsDisp);
// Description :flush dpb
// Parameter : ucEsId : instance id, bIsDisp : display or not
// Return      : None
// **************************************************************************
void _VDEC_H264Flush(UCHAR ucEsId, BOOL bIsDisp)
{
    VDEC_H264_INFO_T* prVdec;
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    if(bIsDisp)
    {
        _VDEC_FlushDPB(&prVdec->rDecParam);
    }
    else
    {
        _VDEC_ClrDPB(&prVdec->rDecParam);
    }
}


// **************************************************************************
// Function : void _VDEC_H264GetPesStruct(UCHAR ucEsId, void **ppvPesInfo);
// Description :get pesinfo
// Parameter : ucEsId : instance id, ppvPesInfo : pes info
// Return      : None
// **************************************************************************
void _VDEC_H264GetPesStruct(UCHAR ucEsId, void **ppvPesInfo)
{
    VDEC_H264_INFO_T* prVdec;
    
    ASSERT(ppvPesInfo !=NULL);    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    if(prVdec->rDecParam.u1RptFrm == 2)
    {
        *ppvPesInfo = NULL;//(void*)(&prVdec->rPesInfoSkip);    //emulation repeat flw, skip pes info        
    }
    else
    {
        *ppvPesInfo = (void*)(&prVdec->rPesInfo);
    }
}


// **************************************************************************
// Function : void _VDEC_H264GetDispPic(UCHAR ucEsId, void* pvInfo);
// Description :get display picture info
// Parameter : ucEsId : instance id, pvInfo : display pictrue info
// Return      :  None
// **************************************************************************
void _VDEC_H264GetDispPic(UCHAR ucEsId, void* pvInfo)
{
    VDEC_H264_INFO_T* prVdec;
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    //#ifdef TEST_ONOFF_RUNTIME
    #if defined(TEST_ONOFF_RUNTIME) || defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)    
        VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);    
    #endif
    UINT8 u1BufIdx = 0;
    #ifdef CC_STRESS_TEST_CRC_CHK
    BOOL fgCRCFail = FALSE;
    #endif
    #ifdef CRCFLOWTEST
    UINT8 i = 0;
    BOOL fgIsEqual = TRUE;
    #endif
    UINT8 u1ViewIdx;
    UINT32 u4VDecID = 0;

    UNUSED(u4VDecID);
    ASSERT(pvInfo !=NULL);
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfoKeep == NULL)
    {
        #ifdef TEST_ONOFF_RUNTIME
         _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        #endif
        return;
    }
    if(prVdec->rDecParam.fgOutDispOrder && (!prVdecEsInfoKeep->fgVdecMidEnable))
    {
        u1BufIdx = prVdec->rDecParam.u1MinPOCBufIdx;
    }
    else
    {
        u1BufIdx = prVdec->rDecParam.u1DecFBufIdx;
    }
    if(u1BufIdx >= 18)
    {
        #ifdef TEST_ONOFF_RUNTIME
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        #endif
        ASSERT(u1BufIdx < 18);
        return;
    }

    u1ViewIdx = prVdec->rDecParam.u1CurViewIdx;
    if(u1ViewIdx >= MAX_VIEW_NUM)
    {
        #ifdef TEST_ONOFF_RUNTIME
         _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        #endif
        LOG(1, "ViewIdx Err!\n");
        return;
    }
    prDispInfo->pvYAddr = (void*)prVdec->rDecParam.prCurrFBufInfo->u4YAddr;
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    prDispInfo->u1PicStruct = prVdec->rDecParam.u1PicStructKeep;    //current decoded frame
    //buf status may be clear if display/buf clr right after decode
    prDispInfo->u4H =
        (UINT16)(prVdec->rDecParam.prCurrFBufInfo->u4H -
        ((prVdec->rDecParam.prSPS->fgFrameMbsOnlyFlag) ?
        prVdec->rDecParam.prSPS->u4FrameCropBottomOffset :
        (prVdec->rDecParam.prSPS->u4FrameCropBottomOffset << 1)));
    //mvc, compare by frame
    if((prDispInfo->u1PicStruct != FRAME) && prVdec->rDecParam.fgIsMVC)
    {
        prDispInfo->pvYAddr = NULL;
        //LOG(3, "[MVC]skip field cmp\n");
    }
    #else
    prDispInfo->u1PicStruct = prVdec->rDecParam.u1PicStruct;
    prDispInfo->u4H = prVdec->rDecParam.prCurrFBufInfo->u4H;
    #endif
    prDispInfo->u4YSize = prVdec->rDecParam.prCurrFBufInfo->u4DramPicSize;
    prDispInfo->pvCAddr = (void*)prVdec->rDecParam.prCurrFBufInfo->u4CAddr;
    prDispInfo->u4CSize = prDispInfo->u4YSize >> 1;
    prDispInfo->pvMVAddr = (void*)prVdec->rDecParam.prCurrFBufInfo->u4MVBufAddr;
    
    prDispInfo->u4W = prVdec->rDecParam.prCurrFBufInfo->u4W;
    prDispInfo->u4ChromaFormatIdc = prVdec->rDecParam.prSPS->u4ChromaFormatIdc;

    #if defined(CRCFLOWTEST) || defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)
    #if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT) && !defined(CRCFLOWTEST)
    if(VDEC_IsTestCRC(ucEsId))
    #endif
    {
        _VDEC_AVCYC(ucEsId, prDispInfo);
    }
    #endif

    #ifdef TEST_ONOFF_RUNTIME
    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    #endif

    #ifdef CC_STRESS_TEST_CRC_CHK
    if(prVdec->rDecParam.u4RecordingCRC > 0)
    {
        if(prVdec->rPesInfo.u4PTS == prVdec->rDecParam.u4ChkPTS)
        {
            prVdec->rDecParam.fgStartChkCRC = TRUE;
        }
        if(prVdec->rDecParam.fgStartChkCRC)
        {
            //PP CRC
            prVdec->rDecParam.u4RecordingCRC--;
            for(u1BufIdx = 0; u1BufIdx < PP_CRC_NUM; u1BufIdx++)
            {
                prVdec->rDecParam.au4RecYcrc[prVdec->rDecParam.u4RecordingCRC][u1BufIdx] = prDispInfo->u4Ycrc[u1BufIdx];
                prVdec->rDecParam.au4RecCcrc[prVdec->rDecParam.u4RecordingCRC][u1BufIdx] = prDispInfo->u4Ccrc[u1BufIdx];
            }
            //Bs input/VLD
            prVdec->rDecParam.u4RecBsCrc[prVdec->rDecParam.u4RecordingCRC] = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_RESISUAL_SUM);
            // 0:3 CRC check, 4:7 MC input check, 8:9 MC check, 10 MC Dram check
            for(u1BufIdx = PP_CRC_NUM; u1BufIdx < CRC_TOTAL_CNT; u1BufIdx++)
            {
                prVdec->rDecParam.au4RecYcrc[prVdec->rDecParam.u4RecordingCRC][u1BufIdx] = prDispInfo->u4Ycrc[u1BufIdx];
            }
            prVdec->rDecParam.au4RecDramcrc[prVdec->rDecParam.u4RecordingCRC][0] = prVdec->rDecParam.u4DramMC_R_CRC;
            prVdec->rDecParam.au4RecDramcrc[prVdec->rDecParam.u4RecordingCRC][1] = prVdec->rDecParam.u4DramPP_W_CRC;

            LOG(6, "CRC Record OK, pts 0x%x~\n", prVdec->rPesInfo.u4PTS);
        }
        if(prVdec->rDecParam.u4RecordingCRC == 0)
        {
            prVdec->rDecParam.fgStartChkCRC = FALSE;
        }
    }

    if(prVdec->rDecParam.fgEnCRC && (!prVdec->rDecParam.fgStartChkCRC))
    {
        if(prVdec->rPesInfo.u4PTS == prVdec->rDecParam.u4ChkPTS)
        {
            prVdec->rDecParam.u4VerifyCRC = CRC_CHK_FRM_NUM;
        }
        if(prVdec->rDecParam.u4VerifyCRC > 0)
        {
            prVdec->rDecParam.u4VerifyCRC--;
            if(fgIsTargetFrm(prVdec->rDecParam.u4VerifyCRC))
            {
                LOG(6, "CRC check, pts 0x%x, ", prVdec->rPesInfo.u4PTS);
                //Bs input/VLD
                if(prVdec->rDecParam.u4RecBsCrc[prVdec->rDecParam.u4VerifyCRC] != u4VDecReadAVCVLD(u4VDecID, RO_AVLD_RESISUAL_SUM))
                {
                    LOG(5, "Bs Input Checksum mismatch! 0x%x\n", u4VDecReadAVCVLD(u4VDecID, RO_AVLD_RESISUAL_SUM));
                    prVdec->rDecParam.u4BsFailCnt++;
                    prVdec->rDecParam.u4VerifyCRC = 0;
                }
                else
                {
                    //PP CRC
                    for(u1BufIdx = 0; u1BufIdx < PP_CRC_NUM; u1BufIdx++)
                    {
                        if(prVdec->rDecParam.au4RecYcrc[prVdec->rDecParam.u4VerifyCRC][u1BufIdx] != prDispInfo->u4Ycrc[u1BufIdx])
                        {
                            LOG(5, "PP CRC Y Fail~\n");
                            prVdec->rDecParam.au4CrcFailCnt[0]++;
                            fgCRCFail = TRUE;
                            break;
                        }
                        if(prVdec->rDecParam.au4RecCcrc[prVdec->rDecParam.u4VerifyCRC][u1BufIdx] != prDispInfo->u4Ccrc[u1BufIdx])
                        {
                            LOG(5, "PP CRC C Fail~\n");
                            prVdec->rDecParam.au4CrcFailCnt[1]++;
                            fgCRCFail = TRUE;
                            break;
                        }
                    }
                    //MC Input
                    for(u1BufIdx = PP_CRC_NUM; u1BufIdx < CRC_TOTAL_CNT; u1BufIdx++)
                    {
                        if(prVdec->rDecParam.au4RecYcrc[prVdec->rDecParam.u4VerifyCRC][u1BufIdx] != prDispInfo->u4Ycrc[u1BufIdx])
                        {
                            LOG(5, "Reg %d mismatch~\n", u1BufIdx);
                            prVdec->rDecParam.au4CrcFailCnt[u1BufIdx]++;
                            fgCRCFail = TRUE;
                        }
                    }
                    //DramC crc
                    if(prVdec->rDecParam.au4RecDramcrc[prVdec->rDecParam.u4VerifyCRC][0] != prVdec->rDecParam.u4DramMC_R_CRC)
                    {
                        LOG(5, "DramC Crc MC R mismatch~\n");
                        fgCRCFail = TRUE;
                    }
                    if(prVdec->rDecParam.au4RecDramcrc[prVdec->rDecParam.u4VerifyCRC][1] != prVdec->rDecParam.u4DramPP_W_CRC)
                    {
                        LOG(5, "DramC Crc PP W mismatch~\n");
                        fgCRCFail = TRUE;
                    }
                    if(!fgCRCFail)
                    {
                        LOG(5, "CRC Pass~\n");
                        prVdec->rDecParam.u4CrcPassCnt++;
                    }
                    else
                    {
                        ASSERT(0);
                    }
                }
            }
        }
    }
    #endif
    
    #ifdef CRCFLOWTEST
    for(i=0;i<4;i++)
    {
        if(prVdecEsInfoKeep->u4Ycrc[i] != prDispInfo->u4Ycrc[i])
        {
            fgIsEqual = FALSE;
            break;
        }
    }

    for(i=0;i<4;i++)
    {
        if(prVdecEsInfoKeep->u4Ccrc[i] != prDispInfo->u4Ccrc[i])
        {
            fgIsEqual = FALSE;
            break;
        }
    }

    if(!fgIsEqual)
    {
        LOG(0,"crc error has happened!!!\n")
    }

    #endif
    #if defined(VDEC_CRC_TEST) || defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    #if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    if(VDEC_IsTestCRC(ucEsId))
    #endif
    {
        UINT8 i;
        for(i=0; i<4;i++)
        {
            prVdecEsInfo->u4CrcResult ^= prDispInfo->u4Ycrc[i];
            prVdecEsInfo->u4CrcResult ^= prDispInfo->u4Ccrc[i];
        }
    }
    #endif
}


// **************************************************************************
// Function : void _VDEC_H264SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3);
// Description : set parameter
// Parameter : ucEsId : instance id, u4Type : set type, u4Param : parameter
// Return      : None
// **************************************************************************
void _VDEC_H264SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    VDEC_H264_INFO_T* prVdec;
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    if(u4Type == (UINT32)RRMODE)
    {
        if(u4Param2 == 0)        //Y
        {
            if(u4Param1 > 0)    //Enable
            {
                prVdec->rDecParam.fgRRModeYEn = TRUE;
                prVdec->rDecParam.u1RRModeY = (UINT8)u4Param3;
                if(u4Param1 == 2)
                {
                    prVdec->rDecParam.fgRRWrapper = FALSE;
                }
                else
                {
                    prVdec->rDecParam.fgRRWrapper = TRUE;
                }
            }
            else
            {
                prVdec->rDecParam.fgRRModeYEn = FALSE;
                prVdec->rDecParam.u1RRModeY = (UINT8)u4Param3;
            }
        }
        else if(u4Param2 == 1)    //C
        {
            if(u4Param1 > 0)        //Enable
            {
                prVdec->rDecParam.fgRRModeCEn = TRUE;
                prVdec->rDecParam.u1RRModeC = (UINT8)u4Param3;
                if(u4Param1 == 2)
                {
                    prVdec->rDecParam.fgRRWrapper = FALSE;
                }
                else
                {
                    prVdec->rDecParam.fgRRWrapper = TRUE;
                }                
            }
            else
            {
                prVdec->rDecParam.fgRRModeCEn = FALSE;
                prVdec->rDecParam.u1RRModeC = (UINT8)u4Param3;
            }
        }
        else if(u4Param2 == 2)      //Max_dis_hbands
        {
            prVdec->rDecParam.u1MaxDisHbands = (UINT8)u4Param3;
        }
        prVdec->rDecParam.fgChgRRMode = TRUE;
    }
    else if(u4Type == (UINT32)OUTPUT_DISP_ORDER)
    {
        if(u4Param1 > 0)            //compare enable, decoding order
        {
            prVdec->rDecParam.fgOutDispOrder = FALSE;
        }
        else
        {
            prVdec->rDecParam.fgOutDispOrder = TRUE;
        }
    }
    else if(u4Type == (UINT32)REPEAT_FRM)
    {
        //repeat frame
        if(u4Param1 > 0)
        {
            if(prVdec->rDecParam.u1RptFrm==0)
            {
                prVdec->rDecParam.u1RptFrm = (UINT8)u4Param1;
            }
        }
        else
        {
            prVdec->rDecParam.u1RptFrm = 0;
        }
    }
    else if(u4Type == (UINT32)ECC_MODE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        prVdec->rDecParam.u1ECMode = (UINT8)((u4Param1 | (u4Param2 << 2)) | (u4Param3 << 1));
    }
    else if(u4Type == (UINT32)ECC_TYPE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        prVdec->rDecParam.u4ECType = u4Param1;
    }    
    else if(u4Type == (UINT32)DRAM_BANDWITH_SIMU)
    {
        prVdec->rDecParam.u4DramMaxLen = u4Param1;
        prVdec->rDecParam.u4DramReqConnDis = u4Param2;
        prVdec->rDecParam.u4DramFifoBank = u4Param3;
    }
    else if(u4Type == (UINT32)LOG_SIMU_PAT)
    {
        prVdec->rDecParam.fgLogSimPat = ((u4Param1>0)?TRUE:FALSE);
    }
    else if(u4Type == (UINT32)WAIT_FB_TIME)
    {
        prVdec->rDecParam.u4WaitFbTime = u4Param1;
    }
    else if(u4Type == (UINT32)AV_SYNC_INFO)
    {
        prVdec->rDecParam.ucSyncMode = (UCHAR)u4Param1;
        prVdec->rDecParam.ucStcId = (UCHAR)u4Param2;
    }
    else if(u4Type == (UINT32)TIME_PROFILE2)
    {
        prVdec->rDecParam.u1DramMoniter = (UINT8)u4Param1;
    }
    else if(u4Type == (UINT32)MM_PLAY_DTV)
    {
        prVdec->rDecParam.fgIsPVR = TRUE;
        prVdec->rDecParam.u1PlayModeInDtv = (UINT8)u4Param1;
    }
    else if(u4Type == (UINT32)USE_MM_ERR_MASK)
    {
        prVdec->rDecParam.fgUseMMErrMask = (u4Param1>0)?TRUE:FALSE;
    }
    else if(u4Type==(UINT32)MM_PARAM)
    {
        prVdec->rDecParam.fgIsMMPlay = (u4Param1>0)?TRUE:FALSE;
        if(prVdec->rDecParam.fgIsMMPlay)
        {
            prVdec->rDecParam.u4WaitFbTime = VDEC_AVSYNC_WAIT_DISP_TIME;
        }
    }
    else if (u4Type==(UINT32)MM_PVR_PLAY_MODE)
    {
        prVdec->rDecParam.fgIsPVR = (BOOL)u4Param1;
        prVdec->rDecParam.u1PlayModeInDtv = (UINT8)u4Param2;
    }
#ifdef CC_STRESS_TEST_CRC_CHK
    else if(u4Type==(UINT32)RECORD_CRC)
    {
        prVdec->rDecParam.u4ChkPTS = u4Param1;
        prVdec->rDecParam.u4RecordingCRC = CRC_CHK_FRM_NUM;
        vVDecEnableCRC((UINT32)ucEsId, TRUE, TRUE);
    }
    else if(u4Type==(UINT32)CRC_CHK)
    {
        prVdec->rDecParam.fgEnCRC = (BOOL)u4Param1;
        vVDecEnableCRC((UINT32)ucEsId, prVdec->rDecParam.fgEnCRC, TRUE);
        if(!prVdec->rDecParam.fgEnCRC)
        {
            x_memset((void*)&prVdec->rDecParam.au4CrcFailCnt, 0, sizeof(prVdec->rDecParam.au4CrcFailCnt));
            prVdec->rDecParam.u4CrcPassCnt = 0;
            prVdec->rDecParam.u4BsFailCnt = 0;
        }
    }
#endif 
    else if (u4Type == (UINT32)VDEC_STATUS)
    {
        prVdec->rDecParam.ucVDecStatus = u4Param1;
        LOG(2, "set status %d\n", u4Param1);
    }
    else if (u4Type == (UINT32)VDEC_UFO_MODE)
    {
        prVdec->rDecParam.fgIsUFOMode = TRUE;
        prVdec->rDecParam.fgIsNewMode = TRUE;
        LOG(2, "set UFO mode on\n");
    }
    else if (u4Type == (UINT32)VDEC_BLK_MODE)
    {
        prVdec->rDecParam.fgIsNewMode = TRUE;
        LOG(2, "set New Block mode on\n");
    }
}


// **************************************************************************
// Function : void _VDEC_H264GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3);
// Description : get parameter
// Parameter : ucEsId : instance id, u4Type : set type, u4Param : parameter
// Return      : None
// **************************************************************************
void _VDEC_H264GetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{   
    UINT32 u4VDecID = 0;
    VDEC_H264_INFO_T* prVdec;
    
    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pu4Param1 !=NULL);
    ASSERT(pu4Param2 !=NULL);
    ASSERT(pu4Param3 !=NULL);
    
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    if(u4Type == (UINT32)DISP_IDX)
    {
        if(prVdec->rDecParam.u1CurViewIdx >= MAX_VIEW_NUM)
        {
            LOG(1, "u1CurViewIdx Err!\n");
            return;
        }
        *pu4Param1 = (UINT32)prVdec->rDecParam.aucFbgId[prVdec->rDecParam.u1CurViewIdx];
        if(prVdec->rDecParam.prCurrFBufInfo)
        {
            *pu4Param2 = (UINT32)prVdec->rDecParam.prCurrFBufInfo->ucFbId;
        }
        if(prVdec->rDecParam.prSPS != NULL)
        {
            if(prVdec->rDecParam.prSPS->rVUI.fgVideoSignalTypePresentFlag)
            {
                *pu4Param3 = prVdec->rDecParam.prSPS->rVUI.u4VideoFormat;
            }
        }
    }
    else if(u4Type == (UINT32)RRMODE)
    {
        if(*pu4Param2 == 0)        //Y
        {
            if(prVdec->rDecParam.fgRRModeYEn)
            {
                if(prVdec->rDecParam.fgRRWrapper)
                {
                    *pu4Param1 = 1;
                }
                else
                {
                    *pu4Param1 = 2;
                }
                *pu4Param3 = (UINT32)prVdec->rDecParam.u1RRModeY;
            }
            else
            {
                *pu4Param1 = 0;
                *pu4Param3 = (UINT32)prVdec->rDecParam.u1RRModeY;
            }
        }
        else if(*pu4Param2 == 1)    //C
        {
            if(prVdec->rDecParam.fgRRModeCEn)
            {
                if(prVdec->rDecParam.fgRRWrapper)
                {
                    *pu4Param1 = 1;
                }
                else
                {
                    *pu4Param1 = 2;
                }
                *pu4Param3 = (UINT32)prVdec->rDecParam.u1RRModeC;
            }
            else
            {
                *pu4Param1 = 0;
                *pu4Param3 = (UINT32)prVdec->rDecParam.u1RRModeC;
            }
        }
    }
    else if(u4Type == (UINT32)OUTPUT_DISP_ORDER)
    {
        if(prVdec->rDecParam.fgOutDispOrder)            //compare enable, decoding order
        {
            *pu4Param1 = 0;
        }
        else
        {
            *pu4Param1 = 1;
        }
    }
    else if(u4Type == (UINT32)REPEAT_FRM)
    {
        *pu4Param1 = prVdec->rDecParam.u1RptFrm;
    }
    else if(u4Type == (UINT32)ECC_MODE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        *pu4Param1 = (UINT32)(prVdec->rDecParam.u1ECMode & 0x1);
        *pu4Param2 = (UINT32)(prVdec->rDecParam.u1ECMode & 0xFC) >> 2;
        *pu4Param3 = (UINT32)(prVdec->rDecParam.u1ECMode & 0x2) >> 1;
    }
    else if(u4Type == (UINT32)ECC_TYPE)
    {
        *pu4Param1 = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_ERC_DED_ERR_TYPE);
    }    
    else if(u4Type == (UINT32)DRAM_BANDWITH_SIMU)
    {
        *pu4Param1 = prVdec->rDecParam.u4DramMaxLen;
        *pu4Param2 = prVdec->rDecParam.u4DramReqConnDis;
        *pu4Param3 = prVdec->rDecParam.u4DramFifoBank;
    }
    else if(u4Type == (UINT32)TIME_PROFILE)
    {
        if(prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus == FRAME)
        {
            *pu4Param1 = prVdec->rDecParam.aucFbgId[0];
            *pu4Param2 = prVdec->rDecParam.u4CurDecMs;       //hw dec
            *pu4Param3 = prVdec->rDecParam.u4CurWaitFbMicro/1000;   //wait fb time
        }
        else
        {
            *pu4Param1 = 0xFF;
        }
    }
    else if(u4Type == (UINT32)TIME_PROFILE2)
    {
         *pu4Param1 = prVdec->rDecParam.u4CurRstMs;   //wait fb time
         prVdec->rDecParam.u4CurRstMs = 0;
         if(prVdec->rDecParam.prSliceHdr)
         {
             *pu4Param2 = prVdec->rDecParam.prSliceHdr->u4SliceType;
         }
         if(prVdec->rDecParam.prCurrFBufInfo)
         {
             *pu4Param3 = (UINT32)((prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus == FRAME)?1:0);
         }
    }
    else if(u4Type == (UINT32)TIME_PROFILE3)
    {
        *pu4Param1 = prVdec->rDecParam.u4CurSwMicro;
        *pu4Param2 = prVdec->rDecParam.u4CurDecCycle;
        *pu4Param3 = prVdec->rDecParam.u4CurDramCycle;
    }
    else if(u4Type == (UINT32)BS_TYPE)
    {
        if(prVdec->rDecParam.prSPS)
        {
            *pu4Param1 = prVdec->rDecParam.prSPS->u4ProfileIdc;
            *pu4Param2 = prVdec->rDecParam.prSPS->u4LevelIdc;
        }
        if(prVdec->rDecParam.prPPS)
        {
            *pu4Param3 = (UINT32)prVdec->rDecParam.prPPS->fgEntropyCodingModeFlag;
        }
    }
    else if(u4Type == (UINT32)SEQ_HDR_INFO)
    {    
        if(prVdec->rDecParam.prSPS)
        {
            *pu4Param1 = (prVdec->rDecParam.prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
            *pu4Param2 = ((2 -prVdec->rDecParam.prSPS->fgFrameMbsOnlyFlag)*
                (prVdec->rDecParam.prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32
            if (prVdec->rDecParam.prSPS->fgVuiParametersPresentFlag)
            {
                if (prVdec->rDecParam.prSPS->rVUI.u4NumUnitsInTick > 0)
                {
                    *pu4Param3 = (prVdec->rDecParam.prSPS->rVUI.u4TimeScale / prVdec->rDecParam.prSPS->rVUI.u4NumUnitsInTick) >> 1;
                }
            }
        }
    }
#ifdef CC_STRESS_TEST_CRC_CHK
    else if(u4Type==(UINT32)CRC_CHK)
    {
        *pu4Param1 = (UINT32)&prVdec->rDecParam.au4CrcFailCnt;
        *pu4Param2 = prVdec->rDecParam.u4BsFailCnt;
        *pu4Param3 = prVdec->rDecParam.u4CrcPassCnt;
    }
#endif    
    else if(u4Type == (UINT32)VDEC_STATUS)
    {
        *pu4Param1 = prVdec->rDecParam.ucVDecStatus;
        VDecPrintf(ucEsId,VDEC_LOG_DECINFO,"[H264-GET] i[%d],t[%d],p[%d]\n",ucEsId,u4Type,*pu4Param1);
    }
}



// **************************************************************************
// Function : void _VDEC_H264UpdDmxRptr(UINT32 u4VDecID, UCHAR ucEsId);
// Description : update read pointer
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void _VDEC_H264UpdDmxRptr(UINT32 u4VDecID, UCHAR ucEsId)
{
    VDEC_H264_INFO_T* prVdec;
    UINT32 u4CurRptr, u4UsedSz = 0;
    
    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    

    _VDEC_GetRptr(u4VDecID, &u4CurRptr);

    u4UsedSz = (u4CurRptr > prVdec->rPesInfo.u4VldReadPtr)?(u4CurRptr - prVdec->rPesInfo.u4VldReadPtr):
        ((prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4VldReadPtr) + (u4CurRptr - prVdec->rPesInfo.u4FifoStart));

    if(u4UsedSz > (WPTR_OFFSET + RPTR_ALIGN))
    {
        u4CurRptr -= (WPTR_OFFSET + RPTR_ALIGN);
    }
    if(u4CurRptr < prVdec->rPesInfo.u4FifoStart)
    {
        u4CurRptr += (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
    }
    //update dmx pointer, use current pes start addr, hw rptr may go beyond frame end
    //if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, u4CurRptr, u4CurRptr))
    if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
    {                    
        #ifndef CC_DRIVER_DEMO
        LOG(2, "H264 ES(%d) DMX_UpdVRPtr Fail CurRptr 0x%x, VS 0x%x, VE 0x%x, PicS 0x%x, DmxWptr 0x%x\n", 
            prVdec->rPesInfo.ucEsId, u4CurRptr, 
            prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd, 
            prVdec->rPesInfo.u4VldReadPtr, prVdec->rDecParam.u4WPtr);
        #endif
    }
    
    if(prVdec->rDecParam.ucDualCoreMode == DUAL_CORE_MODE)
    {
        vVDecResetMultiCore(VDEC_VLD0);
    }
}


// **************************************************************************
// Function : void _VDEC_H264SetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr);
// Description : set dmx write pointer
// Parameter : ucEsId : instance id, u4Wptr : write pointer
// Return      : None
// **************************************************************************
void _VDEC_H264SetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    VDEC_H264_INFO_T* prVdec;
    VDEC_H264_INFO_T* prLAEVdec;
    VDEC_ES_INFO_T* prVdecEsInfo;

    ASSERT(ucEsId < MAX_ES_NUM);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prLAEVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[prVdecEsInfo->ucLaeEsInfoId];
    
    prVdec->rDecParam.u4WPtr = u4Wptr;
    if (DUAL_CORE_MODE == prVdecEsInfo->ucDualCoreMode)
    {
        prLAEVdec->rDecParam.u4WPtr = u4Wptr;
    }
}


// **************************************************************************
// Function : BOOL _VDEC_H264EsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode);
// Description : skip es or not
// Parameter : ucEsId : instance id, pucVldId : vld id, i1EsBufStatus: buffer status, ucSkipMode: skip mode
// Return      :  skip ES if return FALSE
// **************************************************************************
BOOL _VDEC_H264EsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    UCHAR ucFbgId;
    VDEC_H264_INFO_T* prVdec;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_T* prVdecEsInfoSub;
#if defined(CC_USE_DDI)
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;
#endif
#ifdef CC_DUAL_CORE_DEC
    UCHAR ucMainEsID;
    VDEC_H264_INFO_T *prLaeVdec;
#endif
    //FBM_SEQ_HDR_T *prFbmSeqHdr;
    UINT8 u1ViewIdx;
    BOOL b_3d_multi_track_available = TRUE;

    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pucVldId !=NULL);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
#if defined(CC_USE_DDI)
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
#endif
    u1ViewIdx = prVdec->rDecParam.u1CurViewIdx;
    if(u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "u1ViewIdx Err!\n");
        return FALSE;
    }

    if (prVdecEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    ucFbgId = prVdec->rDecParam.aucFbgId[u1ViewIdx];

    #if defined(VDEC_IS_POST_MT5398)
    switch(SRM_GetCurrFBMMode())
    {
        case FBM_NETMODE:
            b_3d_multi_track_available = FALSE;
            break;
        case FBM_TVMODE:
        case FBM_MMMODE:
        default:
            b_3d_multi_track_available = TRUE;
            break;
    }
    #endif
    //skip depend view when trick mode and FBM 1 mpeg buffer mode
    if(((ucSkipMode != IPBMode) || (!b_3d_multi_track_available))
        && (((prVdec->rDecParam.fgIsMVC) &&( IS_MVC_DEPEND_PIC_NALU(prVdec->rPesInfo.ucPicType)||((prVdecEsInfo->fgIsDependView)&&(!IS_AVC_PIC_NALU(prVdec->rPesInfo.ucPicType))))) ||
        ((!b_3d_multi_track_available) &&( IS_MVC_DEPEND_PIC_NALU(prVdec->rPesInfo.ucPicType))) ||
        ((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) && (prVdec->rPesInfo.ucEsId > 0))))
    {
        #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        //clr MVC base view pending buffer
        if(prVdec->rDecParam.u4WaitClrBufIdx < 0xFF)
        {
            //LOG(3, "clr posponed buf %d\n", prDecPrm->u4WaitClrBufIdx);
            _VDEC_ClrFBufInfo(&prVdec->rDecParam, prVdec->rDecParam.u4WaitClrBufIdx); //should be base view
            prVdec->rDecParam.u4WaitClrBufIdx = 0xFF;
        }
        #endif
        if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, 
            prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
        {                    
            LOG(2, "EsFlwCtrl H264 ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);                
        }
        return TRUE;
    }

    if(prVdecEsInfo->fgSpeedChg)
    {
        LOG(0, "SpeedChg %d->%d\n", prVdec->rDecParam.ucSkipMode, ucSkipMode);
        if((prVdec->rDecParam.fgIsMVC)&&((prVdec->rDecParam.ucSkipMode == IPMode)&&(ucSkipMode == IPBMode)))
        {
            SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
            SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
            FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
            _VDEC_ClrDPB(&prVdec->rDecParam);
            FBM_ReleaseDispQ(prVdecEsInfo->ucFbgId);
            prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
            if(prVdecEsInfo->ucSubFbgId != FBM_FBG_ID_UNKNOWN)
            {
                FBM_SetFrameBufferFlag(prVdecEsInfo->ucSubFbgId, FBM_FLAG_SEEK_MODE);
                _VDEC_ClrDPB(&prVdec->rDecParam);
                FBM_ReleaseDispQ(prVdecEsInfo->ucSubFbgId);
                FBM_ClrFrameBufferFlag(prVdecEsInfo->ucSubFbgId, FBM_FLAG_SEEK_MODE);
            }
            prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
        }
        prVdecEsInfo->fgSpeedChg = FALSE;
    }
    //prVdec->rDecParam.ucSkipMode = ucSkipMode;
#ifdef CC_DUAL_CORE_DEC
    if(prVdecEsInfo->ucDualCoreMode)
    {
        prLaeVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[prVdecEsInfo->ucLaeEsInfoId];
        if((ucSkipMode == IPBMode) && (prVdec->rDecParam.ucSkipMode == IPMode))
        {
            if(((prVdecEsInfo->fgSetTargetRdptr) && (prVdecEsInfo->u4TargetRdptr != prVdec->rPesInfo.u4VldReadPtr))
                || (prLaeVdec->rDecParam.ucSkipMode != IPBMode))
            {
                ucSkipMode = prVdec->rDecParam.ucSkipMode;
            }
            else
            {
                prVdecEsInfo->fgSetTargetRdptr = FALSE;
                LOG(1,"VDEC[%d] Skip mode a change %d -> %d\n",ucEsId, prVdec->rDecParam.ucSkipMode,ucSkipMode);
                prVdec->rDecParam.ucSkipMode = ucSkipMode;
            }
        }
        else
        {
            LOG(7,"VDEC[%d] Skip mode b change %d -> %d\n",ucEsId, prVdec->rDecParam.ucSkipMode,ucSkipMode);
            prVdec->rDecParam.ucSkipMode = ucSkipMode;
        }
    }
    else
#endif
    {
    #ifdef CC_DUAL_CORE_DEC
        if((prVdecEsInfo->ucVDecStatus == LAE_DECODE) && (ucSkipMode == IPBMode) && (prVdec->rDecParam.ucSkipMode == IPMode))
        {   
            VDEC_ES_INFO_T* prVdecMcoreEsInfo;
            //Set Mcore target PES
            ucMainEsID = ucVDecGetMcoreDecInst(ucEsId);
            prVdecMcoreEsInfo = _VDEC_GetEsInfo(ucMainEsID);
            prVdecMcoreEsInfo->fgSetTargetRdptr = TRUE;
            prVdecMcoreEsInfo->u4TargetRdptr = prVdec->rPesInfo.u4VldReadPtr;
            LOG(2,"VDEC[%d] Set Target rdptr 0x%x\n",ucEsId, prVdecMcoreEsInfo->u4TargetRdptr);
        }
    #endif
        LOG(7,"VDEC[%d] Skip mode c change %d -> %d\n",ucEsId, prVdec->rDecParam.ucSkipMode,ucSkipMode);
        prVdec->rDecParam.ucSkipMode = ucSkipMode;
    }
#if defined(CC_USE_DDI)
    //in Imode, when rec es pts==0, memory non 0 pts. only for TS container.
    //Temp solution, need improve in the future.
    if((prVdecEsInfoKeep->fgVPush)&&(SWDMX_FMT_MPEG2_TS == prVdecEsInfo->eContainerType))
    {
        if(!(IS_PIC_NALU(prVdec->rPesInfo.ucPicType)))
        {
            prVdec->rDecParam.fglastNonPicDtsValid = prVdec->rPesInfo.fgDtsValid;
            if(prVdec->rPesInfo.fgDtsValid)
            {
                prVdec->rDecParam.u8lastNonPicPts = prVdecEsInfoKeep->rPesInfo.u8PTS;
                prVdec->rDecParam.u4lastNonPicPts = prVdecEsInfoKeep->rPesInfo.u4PTS;
                LOG(1,"Save Pts %x %llx\n",prVdecEsInfoKeep->rPesInfo.u4PTS,prVdecEsInfoKeep->rPesInfo.u8PTS);
            }
        }
        else if(IS_PIC_NALU(prVdec->rPesInfo.ucPicType))
        { 
            if(IMode == prVdec->rDecParam.ucSkipMode)
            {
                if((!prVdec->rPesInfo.fgDtsValid)&&(prVdec->rDecParam.u8lastNonPicPts > prVdec->rDecParam.au8DispPts[u1ViewIdx]))
                {
                    prVdecEsInfoKeep->rPesInfo.u8PTS = prVdec->rDecParam.u8lastNonPicPts;
                    prVdecEsInfoKeep->rPesInfo.u4PTS = prVdec->rDecParam.u4lastNonPicPts;
                    prVdecEsInfoKeep->rPesInfo.fgDtsValid = TRUE;
                    prVdec->rPesInfo.fgDtsValid = TRUE;
                    LOG(1,"Change Pts %x %llx\n",prVdecEsInfoKeep->rPesInfo.u4PTS,prVdecEsInfoKeep->rPesInfo.u8PTS);
                }
            }
            else 
            {
                if((!prVdec->rPesInfo.fgDtsValid)&&(prVdec->rDecParam.u8lastNonPicPts > 0)
                &&(prVdec->rDecParam.fglastNonPicDtsValid == TRUE))
                {
                prVdecEsInfoKeep->rPesInfo.u8PTS = prVdec->rDecParam.u8lastNonPicPts;
                prVdecEsInfoKeep->rPesInfo.u4PTS = prVdec->rDecParam.u4lastNonPicPts;
                prVdecEsInfoKeep->rPesInfo.fgDtsValid = TRUE;
                prVdec->rPesInfo.fgDtsValid = TRUE;
                LOG(1,"Nor Change Pts %x %llx\n",prVdecEsInfoKeep->rPesInfo.u4PTS,prVdecEsInfoKeep->rPesInfo.u8PTS);
                }
            }
        }
    }
#endif
    // ask mpeg decode to skip one picture due to repeat
    // if ESM-Q Ns > Underflow Threshold, otherwise, ignore
    
    if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        if (IS_NREF_NALU(prVdec->rPesInfo.ucPicType) && 
            IS_PIC_NALU(prVdec->rPesInfo.ucPicType)) //nal_ref_idc == 0
        {
            #ifdef FBM_DPB_CTRL
            if (ucFbgId != FBM_FBG_ID_UNKNOWN)
            {
                if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC)|| (prVdec->rDecParam.u1PlayModeInDtv!=IPBMode))
                {
                    //FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC);
                #if BDP_LOG_STYLE
                    VDecPrintf(prVdec->rDecParam.u1CurViewIdx, VDEC_LOG_DROPPIC, 
                                    "[H264 VDEC%d]: _VDEC_FrmFin PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                    prVdec->rDecParam.u1CurViewIdx,
                                    prVdec->rDecParam.u4PicNum, 
                                    prVdec->rPesInfo.ucPicType,
                                    prVdec->rPesInfo.u4PTS);
                #endif
                    if (i1EsBufStatus >= 0)
                    {
                        //update dmx
                        if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, 
                            prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
                        {                    
                            LOG(2, "EsFlwCtrl H264 ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);                
                        }

                          // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
                        if(!_VDEC_SkipFrm(&prVdec->rDecParam))
                        {
                            return FALSE;
                        }
                        else
                        {
                            FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC);
                            return TRUE;    //skip current packet
                        }
                    }
                    // [LOG] Video plane ask mpeg to drop 1 picture, mpeg don't drop since Q is near empty
                }
                else if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC_NO_PTS))
                {
                    if (prVdec->rPesInfo.u4PTS == 0)
                    {
                        FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC_NO_PTS);

                        if (i1EsBufStatus >= 0)
                        {
                            LOG(3, "Q S P VLD(%d) Es(%d) G(%d) RP(0x%08x)\n",
                            *pucVldId, ucEsId, ucFbgId, prVdec->rPesInfo.u4VldReadPtr);

                            //update dmx
                            if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, 
                                prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
                            {                    
                                LOG(2, "EsFlwCtrl H264 ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);                
                            }
                            // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
                            if(!_VDEC_SkipFrm(&prVdec->rDecParam))
                            {
                                return FALSE;
                            }

                            *pucVldId = VDEC_MAX_VLD;
                            return FALSE;
                        }
                    }
                    // [LOG] Video plane ask mpeg to drop 1 picture, mpeg don't drop since Q is near empty
                }
            }
            #endif
            
            //dmx input speed faster than consumption
            if (i1EsBufStatus > 0)
            {
                LOG(3, "Q %d ucDropBNs\n", ucEsId);

                //update dmx
                if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, 
                    prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))                
                {                    
                    LOG(2, "EsFlwCtrl H264 ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);                
                }  
                // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
                if(!_VDEC_SkipFrm(&prVdec->rDecParam))
                {
                    return FALSE;
                }
                else
                {
                    return TRUE;
                }
            }
        }
    }
    #ifdef FBM_DPB_CTRL
    //prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        if(prVdecEsInfo->e3DType == VDEC_3D_MVC)
        {
            LOG(3, "DEC_FLG_WAIT_NEXT_I _VDEC_ClrDPB, View%d\n", prVdec->rDecParam.u1CurViewIdx);
            _VDEC_ClrDPB(&prVdec->rDecParam);
            prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
            _VDEC_ClrDPB(&prVdec->rDecParam);
            prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
        }
        else
        {
            LOG(3, "DEC_FLG_WAIT_NEXT_I FlushDPB, View%d, esid%d\n", prVdec->rDecParam.u1CurViewIdx, ucEsId);
            #ifdef CC_DUAL_CORE_DEC
            if (prVdecEsInfo->ucVDecStatus == LAE_DECODE)
            {               
                vLAEFlushDPB(&prVdec->rDecParam);
            }
            else if (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
            #endif
            {
                _VDEC_FlushDPB(&prVdec->rDecParam);
            }
        }
        CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    }
    //if(prVdec->rDecParam.fgIsMVC)
    if(((prVdecEsInfo->e3DType == VDEC_3D_MVC) || 
        (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
        && (ucEsId != prVdec->rPesInfo.ucEsId))
    {
        prVdecEsInfoSub = _VDEC_GetEsInfo(prVdec->rPesInfo.ucEsId);
        if (IS_DECFLAG_SET(prVdecEsInfoSub, DEC_FLG_WAIT_NEXT_I))
        {
            LOG(3, "DEC_FLG_WAIT_NEXT_I FlushDPB, View%d, esid%d\n", prVdec->rPesInfo.ucEsId, ucEsId);
            prVdec->rDecParam.u1CurViewIdx = prVdec->rPesInfo.ucEsId;
            #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
            if(prVdec->rPesInfo.ucEsId < MAX_VIEW_NUM)
            {
                prVdec->rDecParam.u4CurViewID = prVdec->rDecParam.au4ViewIDList[prVdec->rPesInfo.ucEsId];
            }
            #endif
            #ifdef CC_DUAL_CORE_DEC
            if (prVdecEsInfo->ucVDecStatus == LAE_DECODE)
            {               
                vLAEFlushDPB(&prVdec->rDecParam);
            }
            else if (prVdecEsInfo->ucVDecStatus == REAL_DECODE)
            #endif
            {
                _VDEC_FlushDPB(&prVdec->rDecParam);
            }
            CLR_DECFLAG(prVdecEsInfoSub, DEC_FLG_WAIT_NEXT_I);
        }
    }
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP))
    {
        LOG(3, "DEC_FLG_WAIT_AVC_RAP\n");
        prVdec->rDecParam.rSEI.u1ExtMatchFlg = FALSE;
        prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
        prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
        prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;
        prVdec->rDecParam.fgIsFirstFrm = FALSE;
        prVdec->rDecParam.u1CurViewIdx = 0;
        if(prVdec->rDecParam.fgIsMVC)
        {
            prVdec->rDecParam.fgWaitBaseView = TRUE;
            #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
            prVdec->rDecParam.u4WaitClrBufIdx = 0xFF;
            #endif
        }
        #ifdef ENABLE_MULTIMEDIA
        if(((prVdecEsInfo->eContainerType >= SWDMX_FMT_MPEG2_TS) &&
            (prVdecEsInfo->eContainerType <= SWDMX_FMT_MPEG2_TS_TIME_SHIFT)) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_H264_VIDEO_ES))
        {
            prVdec->rDecParam.fgWaitPPS = TRUE;
            prVdec->rDecParam.u1WaitPPSSkipFrmCnt =
                (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HIGH_SPEED_STORAGE) ? WAIT_PPS_SKIP_FRM_NUM : WAIT_PPS_SKIP_FRM_NUM*3;
            prVdec->rDecParam.ucBNum = 0;
        }
        prVdec->rDecParam.i4LastPOC[u1ViewIdx] = -99999;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastFrameNumOffset = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].fgLastIsRef = FALSE;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefTFldPOC = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefBFldPOC = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefPOC = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefPOCLsb = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefPOCMsb = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastTFldPOC = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastBFldPOC = 0;
        prVdec->rDecParam.rLastInfo[u1ViewIdx].fgLastMmco5 = FALSE;
        if(((prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT) ||
            (prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_ENCRYPT_192))
            && prVdec->rDecParam.prSPS) // find new parameter set
        {   
            prVdec->rDecParam.prSPS->fgSPSValid = FALSE;
            
            prVdec->rDecParam.rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
            prVdec->rDecParam.i4LastPOC[u1ViewIdx] = -99999;
            prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefPOCMsb = 0;
            prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastRefPOCLsb = 0;
            prVdec->rDecParam.rLastInfo[u1ViewIdx].i4LastFrameNumOffset = 0;

            if(prVdec->rDecParam.prSliceHdr)
            {
                prVdec->rDecParam.prSliceHdr->ai4DeltaPicOrderCnt[0] = 0;
            }
            prVdec->rDecParam.i4FrmNumOffset = 0;
        }   
        #endif
        CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
    }
    //start from base view after wait RAP
    if((prVdec->rDecParam.fgIsMVC) && IS_MVC_DEPEND_PIC_NALU(prVdec->rPesInfo.ucPicType) &&
        prVdec->rDecParam.fgWaitBaseView)
    {
        if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, 
        prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
        {
           LOG(2, "EsFlwCtrl H264 ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);  
        }
        return TRUE;
    }
    else if(prVdec->rDecParam.fgWaitBaseView && IS_MVC_BASE_PIC_NALU(prVdec->rPesInfo.ucPicType))
    {
       prVdec->rDecParam.fgWaitBaseView = FALSE;
    }
    if (REAL_DECODE == prVdec->rDecParam.ucVDecStatus)
    {
        if ((ucFbgId != FBM_FBG_ID_UNKNOWN) && (!prVdec->rDecParam.bPreAllocFBG))
        {
            // Check if reset is required
            if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_RESET))
            {
                LOG(3, "Disp rst flag on~\n");
                if(prVdec->rDecParam.prSPS)
                {
                    prVdec->rDecParam.prSPS->fgSPSValid = FALSE;
                }        
                _VDEC_FlushDPB(&prVdec->rDecParam);
                
                if(u1ViewIdx < MAX_VIEW_NUM)
                {
                    prVdec->rDecParam.i4LastPOC[u1ViewIdx] = -99999;
                    prVdec->rDecParam.rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
                }
                prVdec->rDecParam.fgPOCWrap = FALSE;
                prVdec->rDecParam.fgStartPlay = FALSE;
                
                prVdec->rDecParam.rSEI.u1ExtMatchFlg = FALSE;
                prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
                prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
                prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;            
                prVdec->rDecParam.fgIsFirstFrm = FALSE;

                FBM_ResetGroup(ucFbgId);
                FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_RESET);
                FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_NOT_READY);
                FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_CLEAR_FRC_PRM);

                *pucVldId = VDEC_MAX_VLD;
                return TRUE;    //skip current packet
            }
            #ifndef FBM_VIRTUAL_DISPLAY
            // Check if display is ready, skip picture if NOT
            if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISP_READY))
            {
                LOG(3, "Disp Not Ready\n");
                *pucVldId = VDEC_MAX_VLD;
                return TRUE;    //skip current packet
            }
            #endif
        }    
        #endif
    }
    return FALSE;
}


// **************************************************************************
// Function : BOOL _VDEC_IsSupportH264();
// Description : support h264 or not
// Parameter : None
// Return      :  Support if return true
// **************************************************************************
BOOL _VDEC_IsSupportH264()
{
    return IS_SupportH264();
}

// **************************************************************************
// Function : UCHAR _VDEC_H264HandleOverFlw(UCHAR ucVldId, UCHAR ucEsmQId, HANDLE_T hMutex,
            //HANDLE_T hMsgQueue, const void* pvPesInfo);
// Description : handle esmq overflow
// Parameter : ucVldId : vld id, ucEsmQId : esmq id, hMutex : handle, hMsgQueue : msgq handle, pvPesInfo : pes info
// Return      : None
// **************************************************************************
UCHAR _VDEC_H264HandleOverFlw(UCHAR ucVldId, UCHAR ucEsmQId, HANDLE_T hMutex,
    HANDLE_T hMsgQueue, const void* pvPesInfo)
{
#ifndef CC_DRIVER_DEMO
    VDEC_H264_INFO_T* prVdec;
    UINT16 u2MsgQIdx;
    SIZE_T zMsgSize;
    VDEC_PES_INFO_T rPesInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsmQId);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsmQId] == NULL)
    {
        return 1;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsmQId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsmQId];
#endif    
    
    ASSERT(pvPesInfo!=NULL);
    ASSERT(prVdecEsInfo!=NULL);

    //flush Q
    zMsgSize = sizeof(VDEC_PES_INFO_T);
    while (x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
        &hMsgQueue, 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
    {
    }

    if(prVdec->rDecParam.aucFbgId[0] != FBM_FBG_ID_UNKNOWN)
    {
        //flush DPB
        _VDEC_H264Flush(ucEsmQId, FALSE);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
        SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
    }
    
#else
    ASSERT(pvPesInfo!=NULL);

    //temp code
    do
    {
        x_thread_delay(1);
    }
    while (x_msg_q_send(hMsgQueue, pvPesInfo, sizeof(VDEC_PES_INFO_T), 255) != OSR_OK);
    UNUSED(ucEsmQId);
#endif
    
    UNUSED(ucVldId);
    UNUSED(hMutex);
    return 1;
}


#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL  _VDEC_H264IsPic(UCHAR ucEsId, VDEC_PES_INFO_T* prPes)
{
    UINT8 u1NalUnitType;
    
    u1NalUnitType = (prPes->ucPicType& 0x1f);
    if((u1NalUnitType == NON_IDR_SLICE)||(u1NalUnitType == IDR_SLICE)||(u1NalUnitType == CODED_SLICE_EXT))
    {
        if(!fgIsFrmStart)
        {
            fgIsFrmStart = TRUE;
             return FALSE;
        }
        else
        {
            fgIsFrmStart = TRUE;
            return TRUE;
        }
        
    }
    else
    {
        if(fgIsFrmStart)
        {
            fgIsFrmStart = FALSE;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }


}
#endif


// **************************************************************************
// Function : void _VDEC_H264StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, UINT32 u4PtsDiff);
// Description : set start pts
// Parameter : ucEsId : instance id, fgEnCalPTS : enable calculate pts or not, u4Pts : pts value
// Return      : None
// **************************************************************************
void _VDEC_H264StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, UINT32 u4PtsDiff)
{
    VDEC_H264_INFO_T* prVdec;    
    VDEC_PTS_INFO_T* prPTSInfo;
    
    if(ucEsId >= MAX_ES_NUM)
    {
        ASSERT(ucEsId < MAX_ES_NUM);
        return;
    }   
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {
        return;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    
    prVdec->rDecParam.fgEnCalPTS = fgEnCalPTS;
    if(fgEnCalPTS)
    {            
        if(prVdec->rDecParam.u1CurViewIdx >= MAX_VIEW_NUM)
        {
            LOG(1, "u1CurViewIdx Err!\n");
            return;
        }
        prVdec->rDecParam.au4DispPts[prVdec->rDecParam.u1CurViewIdx] = u4Pts;
        prVdec->rDecParam.au8DispPts[prVdec->rDecParam.u1CurViewIdx] = u4Pts;
        prVdec->rDecParam.u8VRemainder = 0;
        prVdec->rDecParam.u8VRemainder64 = 0;
        prPTSInfo = (VDEC_PTS_INFO_T*)u4PtsDiff;
        if(prPTSInfo)
        {
            prVdec->rDecParam.u8PtsScale = prPTSInfo->u8Scale;
            prVdec->rDecParam.u8PtsScale64 = prPTSInfo->u8Scale64;
            prVdec->rDecParam.u4Rate = prPTSInfo->u4Rate;
            prVdec->rDecParam.u4Rate64 = prPTSInfo->u4Rate64;
        }
        _VDEC_H264Flush(ucEsId, TRUE);
    }
}


// **************************************************************************
// Function : void _VDEC_H264SetIsr(UCHAR ucEsId);
// Description : set isr
// Parameter : ucEsId : instance id
// Return      : None
// **************************************************************************
void _VDEC_H264SetIsr(UCHAR ucEsId, UCHAR ucVDecID)
{  
    UINT16 u2Vector = VECTOR_VDEC;
        
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
    _VDEC_RegIsr(u2Vector,_AVLDIsr);
    UNUSED(ucEsId);
}

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
// **************************************************************************
// Function : BOOL _VDEC_MVCFindLRPair(UINT32 u4VDecID, UCHAR ucEsId);
// Description : find matched L/R for mvc
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : return true if matched
// **************************************************************************
BOOL _VDEC_MVCFindLRPair(UINT32 u4VDecID, UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    HANDLE_T hMsgQ;
    UINT16 u2MsgQIdx;
    UCHAR ucMvcpare = 2;
    UCHAR ucDelayTimes = 200;
    SIZE_T zMsgSize;
    VDEC_PES_INFO_T rPesInfo;
    VDEC_PES_INFO_T* prPesInfo = NULL;
    UINT32 au4BasePTS[MAX_VERIFY_FRAME];
    UINT32 au4DepPTS[MAX_VERIFY_FRAME];
    UINT32 u4QIdx = 0, u4BaseIdx, u4DepIdx, u4CurPTS, u4SkipCnt,u4PtsDelt;
    VDEC_H264_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];

    x_memset((void*)(&rPesInfo), 0, sizeof(VDEC_PES_INFO_T));
    au4BasePTS[u4QIdx] = prVdecEsInfo->u4BasePTS;
    au4DepPTS[u4QIdx++] = prVdecEsInfoKeep->rPesInfo.u4PTS;
    zMsgSize = sizeof(VDEC_PES_INFO_T);

    hMsgQ = prVdecEsInfoKeep->hMsgQSub;
    VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2CountMvc)) == OSR_OK);
    hMsgQ = prVdecEsInfoKeep->hMsgQ;
    VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
    while(ucMvcpare&&ucDelayTimes&&(!prVdecEsInfo->fgSwdmxSendEosDone))
    {
        ucMvcpare = 2;
        hMsgQ = prVdecEsInfoKeep->hMsgQSub;
        VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2CountMvc)) == OSR_OK);
        if(prVdecEsInfo->u2CountMvc >40)
        {
            ucMvcpare--;
        }
        else
        {
            x_thread_delay(30);
            ucDelayTimes--;
        }

        hMsgQ = prVdecEsInfoKeep->hMsgQ;
        VERIFY(x_msg_q_num_msgs(hMsgQ, &(prVdecEsInfo->u2Count)) == OSR_OK);
        if(prVdecEsInfo->u2Count >40)
        {
            ucMvcpare--;
        }
        else
        {
            x_thread_delay(30);
            ucDelayTimes--;
        }
    }
    if((prVdecEsInfo->u2Count <40) && (prVdecEsInfo->u2CountMvc <40))
    {
        return FALSE;
    }
    if(prVdecEsInfo->u4BasePTS >= prVdecEsInfoKeep->rPesInfo.u4PTS)
    {
        u4PtsDelt = prVdecEsInfo->u4BasePTS - prVdecEsInfoKeep->rPesInfo.u4PTS;
    }
    else
    {
        u4PtsDelt = prVdecEsInfoKeep->rPesInfo.u4PTS - prVdecEsInfo->u4BasePTS;
    }
    LOG(4,"Dep count %d Base count %d EosDone %d BDPts[0x%x,0x%x],delta %d\n",prVdecEsInfo->u2CountMvc,
    prVdecEsInfo->u2Count,prVdecEsInfo->fgSwdmxSendEosDone,prVdecEsInfo->u4BasePTS,prVdecEsInfoKeep->rPesInfo.u4PTS,u4PtsDelt);  
    if(u4PtsDelt >= 20000)
    {
        if(prVdecEsInfo->u4BasePTS >= prVdecEsInfoKeep->rPesInfo.u4PTS)
        {
            hMsgQ = prVdecEsInfoKeep->hMsgQSub;
            while(u4PtsDelt >= 20000)
            {
                if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                    &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
                {
                    if(IS_MVC_DEPEND_PIC_NALU(rPesInfo.ucPicType))
                    {
                        if(prVdecEsInfo->u4BasePTS >= rPesInfo.u4PTS)
                        {
                            u4PtsDelt = prVdecEsInfo->u4BasePTS - rPesInfo.u4PTS;
                        }
                        else
                        {
                            u4PtsDelt = rPesInfo.u4PTS - prVdecEsInfo->u4BasePTS;
                        }
                        if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                        {
                            prVdecEsInfoKeep->pfnVdecGetPesStruct(1, (void**)&prPesInfo);
                            if(prPesInfo == NULL)
                            {
                                return FALSE;
                            }
                            else
                            {
                                x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                            }
                        }
                    //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                    _VDEC_H264UpdDmxRptr(u4VDecID, 1);
                    }
                }
                else
                {
                    return FALSE;
                }
            }
            au4DepPTS[1] = rPesInfo.u4PTS;
        }
        else
        {
            hMsgQ = prVdecEsInfoKeep->hMsgQ;
            while(u4PtsDelt >= 20000)
            {
                if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                    &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
                {
                    if(IS_MVC_BASE_PIC_NALU(rPesInfo.ucPicType))
                    {
                        if(prVdecEsInfoKeep->rPesInfo.u4PTS >= rPesInfo.u4PTS)
                        {
                            u4PtsDelt = prVdecEsInfoKeep->rPesInfo.u4PTS - rPesInfo.u4PTS;
                        }
                        else
                        {
                            u4PtsDelt = rPesInfo.u4PTS - prVdecEsInfoKeep->rPesInfo.u4PTS;
                        }
                        //get msg from dmx
                        if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                        {
                            prVdecEsInfoKeep->pfnVdecGetPesStruct(0, (void**)&prPesInfo);
                            if(prPesInfo == NULL)
                            {
                                return FALSE;
                            }
                            else
                            {
                                x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                            }
                        }
                        //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                        _VDEC_H264UpdDmxRptr(u4VDecID, 0);
                    }
                }
                else
                {
                    return FALSE;
                }
            }
            au4BasePTS[0] = rPesInfo.u4PTS;
        }
    }
    while(u4QIdx < MAX_VERIFY_FRAME)
    {
        //advance depend view
        u4SkipCnt = 0;
        hMsgQ = prVdecEsInfoKeep->hMsgQSub;
        x_memset((void*)(&rPesInfo), 0, sizeof(VDEC_PES_INFO_T));
        while(!IS_MVC_DEPEND_PIC_NALU(rPesInfo.ucPicType))
        {
            if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
            {
                if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                {
                    prVdecEsInfoKeep->pfnVdecGetPesStruct(1, (void**)&prPesInfo);
                    if(prPesInfo == NULL)
                    {
                        return FALSE;
                    }
                    else
                    {
                        x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                    }
                }
                //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                if(IS_MVC_DEPEND_PIC_NALU(rPesInfo.ucPicType))
                {
                    _VDEC_H264UpdDmxRptr(u4VDecID, 1);
                }
                if(u4SkipCnt++ > MAX_VERIFY_FRAME)
                {
                    return FALSE;
                }
            }
            else    //OSR_NO_MSG
            {
                return FALSE;   
            }
        }
        au4DepPTS[u4QIdx] = rPesInfo.u4PTS;     //dep view advance 1 frame
        u4CurPTS = au4BasePTS[u4QIdx - 1];           //current base frame

        for(u4BaseIdx = 0; u4BaseIdx < u4QIdx; u4BaseIdx++)
        {
            if(au4DepPTS[u4QIdx] == au4BasePTS[u4BaseIdx])
            {
                hMsgQ = prVdecEsInfoKeep->hMsgQSub;
                u4SkipCnt = 0;
                while((rPesInfo.u4PTS != u4CurPTS) || !IS_MVC_DEPEND_PIC_NALU(rPesInfo.ucPicType))
                {
                    if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                    &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
                    {
                        if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                        {
                            prVdecEsInfoKeep->pfnVdecGetPesStruct(1, (void**)&prPesInfo);
                            if(prPesInfo == NULL)
                            {
                                return FALSE;
                            }
                            else
                            {
                                x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                            }
                        }
                        //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                        if(IS_MVC_DEPEND_PIC_NALU(rPesInfo.ucPicType))
                        {
                            _VDEC_H264UpdDmxRptr(u4VDecID, 1);
                        }
                        if(u4SkipCnt++ > MAX_VERIFY_FRAME)
                        {
                            //can't find frame with same pts
                            return FALSE;
                        }
                    }
                    else    //OSR_NO_MSG
                    {
                        return FALSE;   
                    }
                }
                if(0)//u4CurPTS == prVdecEsInfo->u4BasePTS)
                {
                    x_memcpy((void*)&prVdecEsInfoKeep->rPesInfo, (void*)&rPesInfo, sizeof(VDEC_PES_INFO_T));
                    x_memcpy((void*)&prVdec->rPesInfo, (void*)&rPesInfo, sizeof(VDEC_PES_INFO_T));
                }
                else
                {
                    _VDEC_ClrDPB(&prVdec->rDecParam);
                    prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
                    _VDEC_ClrDPB(&prVdec->rDecParam);
                    prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
                }
                return TRUE;
            }
        }

        // advance base view
        u4SkipCnt = 0;
        hMsgQ = prVdecEsInfoKeep->hMsgQ;
        x_memset((void*)(&rPesInfo), 0, sizeof(VDEC_PES_INFO_T));
        while(!IS_MVC_BASE_PIC_NALU(rPesInfo.ucPicType))
        {
            if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
            {
                if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                {
                    prVdecEsInfoKeep->pfnVdecGetPesStruct(0, (void**)&prPesInfo);
                    if(prPesInfo == NULL)
                    {
                        return FALSE;
                    }
                    else
                    {
                        x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                    }
                }
                //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                if(IS_MVC_BASE_PIC_NALU(rPesInfo.ucPicType))
                {
                    _VDEC_H264UpdDmxRptr(u4VDecID, 0);
                }
                if(u4SkipCnt++ > MAX_VERIFY_FRAME)
                {
                    return FALSE;
                }
            }
            else    //OSR_NO_MSG
            {
                return FALSE;   
            }            
        }        
        au4BasePTS[u4QIdx] = rPesInfo.u4PTS;    //base view advance 1 frame
        u4CurPTS = au4DepPTS[u4QIdx];       //current depend frame

        for(u4DepIdx = 0; u4DepIdx <= u4QIdx; u4DepIdx++)
        {
            if(au4BasePTS[u4QIdx] == au4DepPTS[u4DepIdx])
            {
                hMsgQ = prVdecEsInfoKeep->hMsgQ;
                u4SkipCnt = 0;
                while((rPesInfo.u4PTS != u4CurPTS) || !IS_MVC_BASE_PIC_NALU(rPesInfo.ucPicType))
                {
                    if(x_msg_q_receive(&u2MsgQIdx, &rPesInfo, &zMsgSize,
                        &(hMsgQ), 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK)
                    {
                        if(prVdecEsInfoKeep->pfnVdecGetPesStruct)    //h264
                        {
                            prVdecEsInfoKeep->pfnVdecGetPesStruct(0, (void**)&prPesInfo);
                            if(prPesInfo == NULL)
                            {
                                return FALSE;
                            }
                            else
                            {
                                x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                            }
                        }
                        //x_memcpy(prPesInfo, &rPesInfo, sizeof(rPesInfo));
                        if(IS_MVC_BASE_PIC_NALU(rPesInfo.ucPicType))
                        {
                            _VDEC_H264UpdDmxRptr(u4VDecID, 0);
                        }
                        if(u4SkipCnt++ > MAX_VERIFY_FRAME)
                        {
                            //can't find frame with same pts
                            return FALSE;
                        }
                    }
                    else    //OSR_NO_MSG
                    {
                        return FALSE;   
                    }
                }
                _VDEC_ClrDPB(&prVdec->rDecParam);
                prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
                _VDEC_ClrDPB(&prVdec->rDecParam);
                prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
                return TRUE;
            }
        }
        u4QIdx++;
    }    
    return FALSE;
}


// **************************************************************************
// Function : void _VDEC_MvcStartptsBoth(UCHAR ucEsId, BOOL fgBaseDePendNotEuqal);
// Description : start mvc pts
// Parameter : ucEsId : instance id, fgBaseDePendNotEuqal : base and dep equal or not
// Return      : None
// **************************************************************************
void _VDEC_MvcStartptsBoth(UCHAR ucEsId, BOOL fgBaseDePendNotEuqal)
{
    VDEC_H264_INFO_T* prVdec;
    VDEC_ES_INFO_T* prVdecEsInfo;
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
    prVdec->rDecParam.bmvcpair = fgBaseDePendNotEuqal;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    //clr dpb when not compare ok. avoid the frmgap when receive next base view frame 
    if(fgBaseDePendNotEuqal)
    {
        if(prVdec->rDecParam.aucFbgId[prVdec->rDecParam.u1CurViewIdx]!= 0xFF)
        {
            _VDEC_ClrDPB(&prVdec->rDecParam);
        }
        prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
        if(prVdec->rDecParam.aucFbgId[prVdec->rDecParam.u1CurViewIdx]!= 0xFF)
        {
            _VDEC_ClrDPB(&prVdec->rDecParam);
        }
        prVdec->rDecParam.u1CurViewIdx = 1 - prVdec->rDecParam.u1CurViewIdx;
    }
    else
    {
        if(prVdecEsInfo->ucFbgId!= 0xFF)
        {
            FBM_SetFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
            FBM_ReleaseDispQ(prVdecEsInfo->ucFbgId);
            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucFbgId, FBM_FLAG_SEEK_MODE);
        }
        if(prVdecEsInfo->ucSubFbgId!= 0xFF)
        {
            FBM_SetFrameBufferFlag(prVdecEsInfo->ucSubFbgId, FBM_FLAG_SEEK_MODE);
            FBM_ReleaseDispQ(prVdecEsInfo->ucSubFbgId);
            FBM_ClrFrameBufferFlag(prVdecEsInfo->ucSubFbgId, FBM_FLAG_SEEK_MODE);
        }
    }
    prVdec->rDecParam.fgIsFirstFrm = FALSE;
    prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
    prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;
}


// **************************************************************************
// Function : void _VDEC_MvcSetViewIdx(UCHAR ucEsId, BOOL fgViewIdx);
// Description : set view index for MVC
// Parameter : ucEsId : instance id, fgViewIdx : view index
// Return      : None
// **************************************************************************
void _VDEC_MvcSetViewIdx(UCHAR ucEsId, BOOL fgViewIdx)
{
    VDEC_H264_INFO_T* prVdec;
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
    prVdec->rDecParam.u1CurViewIdx = fgViewIdx;
}
#endif


// **************************************************************************
// Function : BOOL _VDEC_ISH264_MVC(void);
// Description : Add for philiphs, tell ap if mvc or not 
// Parameter : None
// Return      : 1 is MVC, 0 is not MVC
// **************************************************************************
BOOL _VDEC_ISH264_MVC(void)
{
    VDEC_H264_INFO_T* prVdec;
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[0] == NULL)
    {
        return FALSE;
    }
    else
    {
        prVdec = rH264DecInfo[0];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[0];
#endif    

    return prVdec->rDecParam.fgIsMVC;
}


#ifdef CC_SUPPORT_VDEC_PREPARSE
// **************************************************************************
// Function : INT32 _VDEC_H264PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId);
// Description : Add for oryx pre-parse, parse w/h/framerate info 
// Parameter : ucEsId : instance id, ucMpvId : video decoder hardware ID
// Return      : 
// **************************************************************************
INT32 _VDEC_H264PrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
    UINT32 u4Temp = 0, u4SeqParameterSetId = 0, i = 0;    
    UINT32 u4FrameRateFrmSPS = 0, u4CurRptr;
    VDEC_AVC_SPS_T* prSPS;       
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    VDEC_H264_INFO_T* prVdec;
               
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(rH264DecInfo[ucEsId] == NULL)
    {        
        return E_VDEC_FAIL;
    }
    else
    {
        prVdec = rH264DecInfo[ucEsId];
    }
#else  
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif    
    
    if(prVdecEsInfo == NULL)
    {
        return E_VDEC_FAIL;
    }
        
    prVdecEsInfo->fgProgressive = TRUE;
    
    _VDEC_LockVld(ucEsId,ucMpvId);
    #ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    _VDEC_SetBsInfo(BARREL2, ucMpvId, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId]->rDecParam.u4WPtr, prVdec);  
    #else
    _VDEC_SetBsInfo(BARREL2, ucMpvId, prVdec->rPesInfo.u4VldReadPtr, rH264DecInfo[prVdec->rPesInfo.ucEsId].rDecParam.u4WPtr, prVdec);  
    #endif
    
    if ((prVdec->rPesInfo.ucPicType & 0x1f) != SPS)
    {
        if (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME)
        {
            prVdec->rPesInfo.ucPicType = _VDEC_AVCGetStartCode((UINT32)ucMpvId, BARREL1);
            _VDEC_GetRptr((UINT32)ucMpvId, &u4CurRptr);
            prVdec->rPesInfo.u4VldReadPtr = u4CurRptr;
            LOG(6,"\n [h264]:Find Start Code(1): 0x%x\n", prVdec->rPesInfo.ucPicType);
            VDEC_SendEs(&prVdec->rPesInfo);
        }
        _VDEC_UnlockVld(ucEsId,ucMpvId);
        return E_VDEC_PARSE_NOT_READY;
    }
    
    u4Temp = (_VDEC_AVCGetShtBitStream(ucMpvId, 24) >> 8);
    // 1st
    u4SeqParameterSetId = _VDEC_UeCodeNum(ucMpvId);        
    if(u4SeqParameterSetId < 32)
    {
        prSPS = &prVdec->arSPS[u4SeqParameterSetId];
    }
    else
    {
        _VDEC_UnlockVld(ucEsId,ucMpvId);
        return E_VDEC_PARSE_NOT_READY;
    }
    // 2nd   
    prSPS->u4ProfileIdc = (u4Temp >> 16);                              // [23:16]
    if((prSPS->u4ProfileIdc == FREXT_HP) 
        || (prSPS->u4ProfileIdc == FREXT_Hi10P) 
        || (prSPS->u4ProfileIdc == FREXT_Hi422)
        || (prSPS->u4ProfileIdc == FREXT_Hi444)
        || (prSPS->u4ProfileIdc == MVC_HIGH)
        || (prSPS->u4ProfileIdc == MVC_STEREO))
    {
        prSPS->u4ChromaFormatIdc = _VDEC_UeCodeNum(ucMpvId);
        if(prSPS->u4ChromaFormatIdc == 3)
        {
            prSPS->fgResidualColorTransformFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        }
        prSPS->u4BitDepthLumaMinus8 = _VDEC_UeCodeNum(ucMpvId);
        prSPS->u4BitDepthChromaMinus8 = _VDEC_UeCodeNum(ucMpvId);
        if ((2 == prSPS->u4BitDepthLumaMinus8) && (2 == prSPS->u4BitDepthChromaMinus8))
        {
            prVdecEsInfo->u4BitDepth = 10;
        }
        else
        {
            prVdecEsInfo->u4BitDepth = 8;
        }
        prSPS->fgQpprimeYZeroTransformBypassFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        prSPS->fgSeqScalingMatrixPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->fgSeqScalingMatrixPresentFlag)
        {
            for(i = 0; i < 8; i ++)
            {
                prSPS->fgSeqScalingListPresentFlag[i] = _VDEC_AVCGetBitStreamFlg(ucMpvId);
                if(prSPS->fgSeqScalingListPresentFlag[i])
                {
                    if(i < 6)
                    {
                        _vScalingList(ucMpvId, prSPS->ppcScalingList4x4[i],16, &prSPS->fgUseDefaultScalingMatrix4x4Flag[i]);
                    }
                    else
                    {
                        _vScalingList(ucMpvId, prSPS->ppcScalingList8x8[i-6],64, &prSPS->fgUseDefaultScalingMatrix8x8Flag[i-6]);
                    }
                }
            }
        }
    }

    prSPS->u4Log2MaxFrameNumMinus4 = _VDEC_UeCodeNum(ucMpvId);
    prSPS->u4PicOrderCntType = _VDEC_UeCodeNum(ucMpvId);
    if(prSPS->u4PicOrderCntType == 0)
    {
        prSPS->u4Log2MaxPicOrderCntLsbMinus4 = _VDEC_UeCodeNum(ucMpvId);
    }
    else if(prSPS->u4PicOrderCntType == 1)
    {
        prSPS->fgDeltaPicOrderAlwaysZeroFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        prSPS->i4OffsetForNonRefPic = _VDEC_SeCodeNum(ucMpvId);
        prSPS->i4OffsetForTopToBottomField = _VDEC_SeCodeNum(ucMpvId);
        prSPS->u4NumRefFramesInPicOrderCntCycle = _VDEC_UeCodeNum(ucMpvId);
        for(i=0 ; i<prSPS->u4NumRefFramesInPicOrderCntCycle; i++)
        {
            prSPS->ai4OffsetForRefFrame[i] = _VDEC_SeCodeNum(ucMpvId);
        }
    }
    prSPS->u4NumRefFrames = _VDEC_UeCodeNum(ucMpvId);
    prSPS->fgGapsInFrameNumValueAllowedFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
    prSPS->u4PicWidthInMbsMinus1 = _VDEC_UeCodeNum(ucMpvId);
    prSPS->u4PicHeightInMapUnitsMinus1 = _VDEC_UeCodeNum(ucMpvId);    
    prSPS->fgFrameMbsOnlyFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
    prVdecEsInfo->u2OrgHSize = (prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prVdecEsInfo->u2OrgVSize = ((2 - prSPS->fgFrameMbsOnlyFlag)*(prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
    if(!prSPS->fgFrameMbsOnlyFlag)
    {
        prSPS->fgMbAdaptiveFrameFieldFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
    }
    prSPS->fgDirect8x8InferenceFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);

    prSPS->fgFrameCroppingFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);

    if(prSPS->fgFrameCroppingFlag)
    {
        prSPS->u4FrameCropLeftOffset = _VDEC_UeCodeNum(ucMpvId);
        prSPS->u4FrameCropRightOffset = _VDEC_UeCodeNum(ucMpvId);

        prSPS->u4FrameCropTopOffset = _VDEC_UeCodeNum(ucMpvId);
        prSPS->u4FrameCropBottomOffset = _VDEC_UeCodeNum(ucMpvId);
    }
    prSPS->fgVuiParametersPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
    
    if(prSPS->fgVuiParametersPresentFlag)
    {
        prSPS->rVUI.fgAspectRatioInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->rVUI.fgAspectRatioInfoPresentFlag)
        {
            prSPS->rVUI.u4AspectRatioIdc = _VDEC_AVCGetRealShtBitStream(ucMpvId, 8);
            if(prSPS->rVUI.u4AspectRatioIdc == 255) //Extended_SAR
            {
                prSPS->rVUI.u4SarWidth = _VDEC_AVCGetRealShtBitStream(ucMpvId, 16);
                prSPS->rVUI.u4SarHeight = _VDEC_AVCGetRealShtBitStream(ucMpvId, 16);
            }
        }
        prSPS->rVUI.fgOverscanInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->rVUI.fgOverscanInfoPresentFlag)
        {
            prSPS->rVUI.fgOverscanAppropriateFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        }
        prSPS->rVUI.fgVideoSignalTypePresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->rVUI.fgVideoSignalTypePresentFlag)
        {
            prSPS->rVUI.u4VideoFormat = _VDEC_AVCGetRealShtBitStream(ucMpvId, 3); 
            prSPS->rVUI.fgVideoFullRangeFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
            prSPS->rVUI.fgColourDescriptionPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
            if(prSPS->rVUI.fgColourDescriptionPresentFlag)
            {
                prSPS->rVUI.u4ColourPrimaries = _VDEC_AVCGetRealShtBitStream(ucMpvId, 8); 
                prSPS->rVUI.u4TransferCharacteristics = _VDEC_AVCGetRealShtBitStream(ucMpvId, 8); 
                prSPS->rVUI.u4MatrixCoefficients = _VDEC_AVCGetRealShtBitStream(ucMpvId, 8); 
            }
        }
        prSPS->rVUI.fgChromaLocationInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->rVUI.fgChromaLocationInfoPresentFlag)
        {
            prSPS->rVUI.u4ChromaSampleLocTypeTopField = _VDEC_UeCodeNum(ucMpvId); 
            prSPS->rVUI.u4ChromaSampleLocTypeBottomField = _VDEC_UeCodeNum(ucMpvId); 
        }

        prSPS->rVUI.fgTimingInfoPresentFlag = _VDEC_AVCGetBitStreamFlg(ucMpvId);
        if(prSPS->rVUI.fgTimingInfoPresentFlag)
        {
            prSPS->rVUI.u4NumUnitsInTick = _VDEC_AVCGetRealShtBitStream(ucMpvId, 32); 
            prSPS->rVUI.u4TimeScale = _VDEC_AVCGetRealShtBitStream(ucMpvId, 32);                 
        }
        if (prSPS->rVUI.u4NumUnitsInTick > 0)
        {
            u4FrameRateFrmSPS = (prSPS->rVUI.u4TimeScale / prSPS->rVUI.u4NumUnitsInTick) >> 1;
        }
        switch (u4FrameRateFrmSPS)
        {
            case 12:
                prVdecEsInfo->u4FrameRate = 1200;
                break;
            case 15:
                prVdecEsInfo->u4FrameRate = 1500;
                break;
            case 20:
                prVdecEsInfo->u4FrameRate = 2000;
                break;
            case 24: //distinguish 24 and 25, add (prDecPrm->prSPS->rVUI.u4NumUnitsInTick >> 1) for precision issue
                if ((((prSPS->rVUI.u4TimeScale + (prSPS->rVUI.u4NumUnitsInTick >> 1)) /
                                prSPS->rVUI.u4NumUnitsInTick) >> 1) == 25)
                {
                    prVdecEsInfo->u4FrameRate = 2500;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 2400;
                }
                break;
            case 23:
                prVdecEsInfo->u4FrameRate = 2397;
                break;
            case 25:
                prVdecEsInfo->u4FrameRate = 2500;
                break;
            case 30:
                prVdecEsInfo->u4FrameRate = 3000;
                break;
            case 29:
                prVdecEsInfo->u4FrameRate = 2997;
                break;
            case 47:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    prVdecEsInfo->u4FrameRate = 2397;
                }
                else
                {
                    LOG(1,"Framerate warning .....\n");
                    prVdecEsInfo->u4FrameRate = 3000;
                }
                break;
            case 50:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {   
                    prVdecEsInfo->u4FrameRate = 2500;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 5000;
                }
                break;
            case 60:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {    
                    prVdecEsInfo->u4FrameRate = 3000;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 6000;
                }
                break;
            case 59:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    prVdecEsInfo->u4FrameRate = 2997;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 5997;
                }
                break;
            case 120:
                prVdecEsInfo->u4FrameRate = 12000;
                break;
            case 240:
                prVdecEsInfo->u4FrameRate = 24000;
                break;
            default:
                prVdecEsInfo->u4FrameRate = 0;
                break;
        }
    }
    else
    {
        prVdecEsInfo->u4FrameRate = 0;
    }
    
    _VDEC_UnlockVld(ucEsId,ucMpvId);
    return E_VDEC_OK;
}


// **************************************************************************
// Function : INT32 _VDEC_H264SWPrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId);
// Description : Add for oryx sw pre-parse, parse w/h/framerate info 
// Parameter : ucEsId : instance id, ucMpvId : video decoder hardware ID
// Return      : 
// **************************************************************************
INT32 _VDEC_H264SWPrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
    static pRBSP_BitStream pRBSP_BitStreamInst = NULL;
    UINT32 u4Idx = 0, u4FrameRateFrmSPS = 0, u4Addr, u4Temp;
    UCHAR *pucBuf;
    static VDEC_AVC_SPS_T* prSPS;       
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if(prVdecEsInfo == NULL)
    {
        return E_VDEC_FAIL;
    }
    if ((prVdecEsInfoKeep->rPesInfo.ucPicType & 0x1f) != SPS)
    {
        return E_VDEC_PARSE_NOT_READY;
    }
    prVdecEsInfo->fgProgressive = TRUE;
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
#else
    u4Addr = prVdecEsInfoKeep->rPesInfo.u4VldReadPtr;
#endif
    pucBuf = (UCHAR*)VIRTUAL(u4Addr);

    pRBSP_BitStreamInst = (pRBSP_BitStream)x_mem_alloc_virtual(sizeof(RBSP_BitStream));
    if(NULL == pRBSP_BitStreamInst)
    {
        return E_VDEC_FAIL;
    }
    x_memset(pRBSP_BitStreamInst, 0, sizeof(RBSP_BitStream));
    prSPS = (VDEC_AVC_SPS_T* )x_mem_alloc_virtual(sizeof(VDEC_AVC_SPS_T));
    if(NULL == prSPS)
    {
        return E_VDEC_FAIL;
    }
    x_memset(prSPS, 0, sizeof(VDEC_AVC_SPS_T));
    
    initRBSPBitStream(pRBSP_BitStreamInst, pucBuf, (300-1));
    
    prSPS->u4ProfileIdc = RBSPGetBits(pRBSP_BitStreamInst, 8);//u(8)
#if DEBUG_INFO
    LOG(0,"u4ProfileIdc %d\n", prSPS->u4ProfileIdc);
#endif
    prSPS->fgConstrainedSet0Flag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)    
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet0Flag %d\n", prSPS->fgConstrainedSet0Flag);
#endif
    prSPS->fgConstrainedSet1Flag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet1Flag %d\n", prSPS->fgConstrainedSet1Flag);
#endif
    prSPS->fgConstrainedSet2Flag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet2Flag %d\n", prSPS->fgConstrainedSet2Flag);
#endif
    prSPS->fgConstrainedSet3Flag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)    
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet3Flag %d\n", prSPS->fgConstrainedSet3Flag);
#endif
    
    u4Temp = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)    
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet4Flag %d\n", u4Temp);
#endif
    u4Temp = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgConstrainedSet5Flag %d\n", u4Temp);
#endif
    u4Temp = RBSPGetBits(pRBSP_BitStreamInst, 2);//u(2)
#if DEBUG_INFO
    LOG(0,"reserved zero 2 bits %d\n", u4Temp);
#endif
    prSPS->u4LevelIdc = RBSPGetBits(pRBSP_BitStreamInst, 8);
#if DEBUG_INFO
    LOG(0,"u4LevelIdc %d\n", prSPS->u4LevelIdc);
#endif
    prSPS->u4SeqParameterSetId = RBSPGetUE(pRBSP_BitStreamInst);
#if DEBUG_INFO
    LOG(0,"u4SeqParameterSetId %d\n", prSPS->u4SeqParameterSetId);
#endif
    
    if(prSPS->u4SeqParameterSetId >= 32)
    {
        LOG(2, "err in SPS Num\n");
        return E_VDEC_PARSE_NOT_READY;
    }
    
    if((prSPS->u4ProfileIdc == FREXT_HP) 
        || (prSPS->u4ProfileIdc == FREXT_Hi10P) 
        || (prSPS->u4ProfileIdc == FREXT_Hi422)
        || (prSPS->u4ProfileIdc == FREXT_Hi444)
        || (prSPS->u4ProfileIdc == MVC_HIGH)
        || (prSPS->u4ProfileIdc == MVC_STEREO))
    {
        prSPS->u4ChromaFormatIdc = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4ChromaFormatIdc %d\n", prSPS->u4ChromaFormatIdc);
#endif
        if(prSPS->u4ChromaFormatIdc == YUV444) //YUV444 == 3
        {
            prSPS->fgResidualColorTransformFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
    #if DEBUG_INFO
            LOG(0,"fgResidualColorTransformFlag %d\n", prSPS->fgResidualColorTransformFlag);
    #endif
        }
        prSPS->u4BitDepthLumaMinus8 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4BitDepthLumaMinus8 %d\n", prSPS->u4BitDepthLumaMinus8);
#endif
        prSPS->u4BitDepthChromaMinus8 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4BitDepthChromaMinus8 %d\n", prSPS->u4BitDepthChromaMinus8);
#endif
        if ((2 == prSPS->u4BitDepthLumaMinus8) && (2 == prSPS->u4BitDepthChromaMinus8))
        {
            prVdecEsInfo->u4BitDepth = 10;
        }
        else
        {
            prVdecEsInfo->u4BitDepth = 8;
        }
        prSPS->fgQpprimeYZeroTransformBypassFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgQpprimeYZeroTransformBypassFlag %d\n", prSPS->fgQpprimeYZeroTransformBypassFlag);
#endif
        prSPS->fgSeqScalingMatrixPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgSeqScalingMatrixPresentFlag %d\n", prSPS->fgSeqScalingMatrixPresentFlag);
#endif
        if(prSPS->fgSeqScalingMatrixPresentFlag)
        {
            for(u4Idx = 0; u4Idx < 8; u4Idx ++)
            {
                prSPS->fgSeqScalingListPresentFlag[u4Idx] = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
                if(prSPS->fgSeqScalingListPresentFlag[u4Idx])
                {
                    if(u4Idx < 6)
                    {
                        Set_ScalingList(pRBSP_BitStreamInst, prSPS->ppcScalingList4x4[u4Idx],16, &prSPS->fgUseDefaultScalingMatrix4x4Flag[u4Idx]);
                    }
                    else
                    {
                        Set_ScalingList(pRBSP_BitStreamInst, prSPS->ppcScalingList8x8[u4Idx-6],64, &prSPS->fgUseDefaultScalingMatrix8x8Flag[u4Idx-6]);
                    }
                }
            }
        }
    }

    prSPS->u4Log2MaxFrameNumMinus4 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
    LOG(0,"u4Log2MaxFrameNumMinus4 %d\n", prSPS->u4Log2MaxFrameNumMinus4);
#endif
    prSPS->u4MaxFrameNum = 2 << (prSPS->u4Log2MaxFrameNumMinus4 +4 -1);
#if DEBUG_INFO
    LOG(0,"u4MaxFrameNum %d\n", prSPS->u4MaxFrameNum);
#endif
    prSPS->u4PicOrderCntType = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
    LOG(0,"u4PicOrderCntType %d\n", prSPS->u4PicOrderCntType);
#endif
    if(0 == prSPS->u4PicOrderCntType)
    {
        prSPS->u4Log2MaxPicOrderCntLsbMinus4 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4Log2MaxPicOrderCntLsbMinus4 %d\n", prSPS->u4Log2MaxPicOrderCntLsbMinus4);
#endif
    }
    else if(1 == prSPS->u4PicOrderCntType)
    {
        prSPS->fgDeltaPicOrderAlwaysZeroFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgDeltaPicOrderAlwaysZeroFlag %d\n", prSPS->fgDeltaPicOrderAlwaysZeroFlag);
#endif
        prSPS->i4OffsetForNonRefPic = RBSPGetSE(pRBSP_BitStreamInst);//se(v)
#if DEBUG_INFO
        LOG(0,"i4OffsetForNonRefPic %d\n", prSPS->i4OffsetForNonRefPic);
#endif
        prSPS->i4OffsetForTopToBottomField = RBSPGetSE(pRBSP_BitStreamInst);//se(v)
#if DEBUG_INFO
        LOG(0,"i4OffsetForTopToBottomField %d\n", prSPS->i4OffsetForTopToBottomField);
#endif
        prSPS->u4NumRefFramesInPicOrderCntCycle = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4NumRefFramesInPicOrderCntCycle %d\n", prSPS->u4NumRefFramesInPicOrderCntCycle);
#endif
        for(u4Idx = 0 ; u4Idx < prSPS->u4NumRefFramesInPicOrderCntCycle; u4Idx++)
        {
            prSPS->ai4OffsetForRefFrame[u4Idx] = RBSPGetSE(pRBSP_BitStreamInst);//se(v)
        }
    }
    prSPS->u4NumRefFrames = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
    LOG(0,"u4NumRefFrames %d\n", prSPS->u4NumRefFrames);
#endif
    prSPS->fgGapsInFrameNumValueAllowedFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgGapsInFrameNumValueAllowedFlag %d\n", prSPS->fgGapsInFrameNumValueAllowedFlag);
#endif
    prSPS->u4PicWidthInMbsMinus1 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
    LOG(0,"u4PicWidthInMbsMinus1 %d\n", prSPS->u4PicWidthInMbsMinus1);
#endif
    prSPS->u4PicHeightInMapUnitsMinus1 = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
    LOG(0,"u4PicHeightInMapUnitsMinus1 %d\n", prSPS->u4PicHeightInMapUnitsMinus1);
#endif
    prSPS->fgFrameMbsOnlyFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgFrameMbsOnlyFlag %d\n", prSPS->fgFrameMbsOnlyFlag);
#endif
    prVdecEsInfo->u2OrgHSize = (prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prVdecEsInfo->u2OrgVSize = ((2 - prSPS->fgFrameMbsOnlyFlag)*(prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
    if(!prSPS->fgFrameMbsOnlyFlag)
    {
        prSPS->fgMbAdaptiveFrameFieldFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgMbAdaptiveFrameFieldFlag %d\n", prSPS->fgMbAdaptiveFrameFieldFlag);
#endif
    }
    prSPS->fgDirect8x8InferenceFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgDirect8x8InferenceFlag %d\n", prSPS->fgDirect8x8InferenceFlag);
#endif
    prSPS->fgFrameCroppingFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgFrameCroppingFlag %d\n", prSPS->fgFrameCroppingFlag);
#endif
    if(prSPS->fgFrameCroppingFlag)
    {
        prSPS->u4FrameCropLeftOffset = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4FrameCropLeftOffset %d\n", prSPS->u4FrameCropLeftOffset);
#endif
        prSPS->u4FrameCropRightOffset = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4FrameCropRightOffset %d\n", prSPS->u4FrameCropRightOffset);
#endif
        prSPS->u4FrameCropTopOffset = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4FrameCropTopOffset %d\n", prSPS->u4FrameCropTopOffset);
#endif
        prSPS->u4FrameCropBottomOffset = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
#if DEBUG_INFO
        LOG(0,"u4FrameCropBottomOffset %d\n", prSPS->u4FrameCropBottomOffset);
#endif
    }
    prSPS->fgVuiParametersPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
    LOG(0,"fgVuiParametersPresentFlag %d\n", prSPS->fgVuiParametersPresentFlag);
#endif

    if(prSPS->fgVuiParametersPresentFlag)
    {
        prSPS->rVUI.fgAspectRatioInfoPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgAspectRatioInfoPresentFlag %d\n", prSPS->rVUI.fgAspectRatioInfoPresentFlag);
#endif
        if(prSPS->rVUI.fgAspectRatioInfoPresentFlag)
        {
            prSPS->rVUI.u4AspectRatioIdc = RBSPGetBits(pRBSP_BitStreamInst, 8);//u(8)
    #if DEBUG_INFO
            LOG(0,"u4AspectRatioIdc %d\n", prSPS->rVUI.u4AspectRatioIdc);
    #endif
            if(prSPS->rVUI.u4AspectRatioIdc == 255) 
            {//Extended_SAR
                prSPS->rVUI.u4SarWidth = RBSPGetBits(pRBSP_BitStreamInst, 16);//u(16)
        #if DEBUG_INFO
                LOG(0,"u4SarWidth %d\n", prSPS->rVUI.u4SarWidth);
        #endif
                prSPS->rVUI.u4SarHeight = RBSPGetBits(pRBSP_BitStreamInst, 16);//u(16)
        #if DEBUG_INFO
                LOG(0,"u4SarHeight %d\n", prSPS->rVUI.u4SarHeight);
        #endif
            }
        }
        prSPS->rVUI.fgOverscanInfoPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgOverscanInfoPresentFlag %d\n", prSPS->rVUI.fgOverscanInfoPresentFlag);
#endif
        if(prSPS->rVUI.fgOverscanInfoPresentFlag)
        {
            prSPS->rVUI.fgOverscanAppropriateFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
    #if DEBUG_INFO
            LOG(0,"fgOverscanAppropriateFlag %d\n", prSPS->rVUI.fgOverscanAppropriateFlag);
    #endif
        }

        prSPS->rVUI.fgVideoSignalTypePresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgVideoSignalTypePresentFlag %d\n", prSPS->rVUI.fgVideoSignalTypePresentFlag);
#endif
        if(prSPS->rVUI.fgVideoSignalTypePresentFlag)
        {
            prSPS->rVUI.u4VideoFormat = RBSPGetBits(pRBSP_BitStreamInst, 3);//u(3)
    #if DEBUG_INFO
            LOG(0,"u4VideoFormat %d\n", prSPS->rVUI.u4VideoFormat);
    #endif
            prSPS->rVUI.fgVideoFullRangeFlag =RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
    #if DEBUG_INFO
            LOG(0,"fgVideoFullRangeFlag %d\n", prSPS->rVUI.fgVideoFullRangeFlag);
    #endif
            prSPS->rVUI.fgColourDescriptionPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
    #if DEBUG_INFO
            LOG(0,"fgColourDescriptionPresentFlag %d\n", prSPS->rVUI.fgColourDescriptionPresentFlag);
    #endif
            if(prSPS->rVUI.fgColourDescriptionPresentFlag)
            {
                prSPS->rVUI.u4ColourPrimaries = RBSPGetBits(pRBSP_BitStreamInst, 8);//u(8)
        #if DEBUG_INFO
                LOG(0,"u4ColourPrimaries %d\n", prSPS->rVUI.u4ColourPrimaries);
        #endif
                prSPS->rVUI.u4TransferCharacteristics = RBSPGetBits(pRBSP_BitStreamInst, 8);//u(8)
        #if DEBUG_INFO
                LOG(0,"u4TransferCharacteristics %d\n", prSPS->rVUI.u4TransferCharacteristics);
        #endif
                prSPS->rVUI.u4MatrixCoefficients = RBSPGetBits(pRBSP_BitStreamInst, 8);//u(8)
        #if DEBUG_INFO
                    LOG(0,"u4MatrixCoefficients %d\n", prSPS->rVUI.u4MatrixCoefficients);
        #endif
            }
        }

        prSPS->rVUI.fgChromaLocationInfoPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgChromaLocationInfoPresentFlag %d\n", prSPS->rVUI.fgChromaLocationInfoPresentFlag);
#endif
        if(prSPS->rVUI.fgChromaLocationInfoPresentFlag)
        {
            prSPS->rVUI.u4ChromaSampleLocTypeTopField = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
    #if DEBUG_INFO
            LOG(0,"u4ChromaSampleLocTypeTopField %d\n", prSPS->rVUI.u4ChromaSampleLocTypeTopField);
    #endif
            prSPS->rVUI.u4ChromaSampleLocTypeBottomField = RBSPGetUE(pRBSP_BitStreamInst);//ue(v)
    #if DEBUG_INFO
            LOG(0,"u4ChromaSampleLocTypeBottomField %d\n", prSPS->rVUI.u4ChromaSampleLocTypeBottomField);
    #endif
        }

        prSPS->rVUI.fgTimingInfoPresentFlag = RBSPGetBits(pRBSP_BitStreamInst, 1);//u(1)
#if DEBUG_INFO
        LOG(0,"fgTimingInfoPresentFlag %d\n", prSPS->rVUI.fgTimingInfoPresentFlag);
#endif
        if(prSPS->rVUI.fgTimingInfoPresentFlag)
        {
            prSPS->rVUI.u4NumUnitsInTick = RBSPGetBits(pRBSP_BitStreamInst, 32);//u(32)
    #if DEBUG_INFO
            LOG(0,"u4NumUnitsInTick %d\n", prSPS->rVUI.u4NumUnitsInTick);
    #endif
            prSPS->rVUI.u4TimeScale = RBSPGetBits(pRBSP_BitStreamInst, 32);//u(32)
    #if DEBUG_INFO
            LOG(0,"u4TimeScale %d\n", prSPS->rVUI.u4TimeScale);
    #endif
        }
        if (prSPS->rVUI.u4NumUnitsInTick > 0)
        {
            u4FrameRateFrmSPS = (prSPS->rVUI.u4TimeScale / prSPS->rVUI.u4NumUnitsInTick) >> 1;
        }
        switch (u4FrameRateFrmSPS)
        {
            case 12:
                prVdecEsInfo->u4FrameRate = 1200;
                break;
            case 15:
                prVdecEsInfo->u4FrameRate = 1500;
                break;
            case 20:
                prVdecEsInfo->u4FrameRate = 2000;
                break;
            case 24: //distinguish 24 and 25, add (prDecPrm->prSPS->rVUI.u4NumUnitsInTick >> 1) for precision issue
                if ((((prSPS->rVUI.u4TimeScale + (prSPS->rVUI.u4NumUnitsInTick >> 1)) /
                                prSPS->rVUI.u4NumUnitsInTick) >> 1) == 25)
                {
                    prVdecEsInfo->u4FrameRate = 2500;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 2400;
                }
                break;
            case 23:
                prVdecEsInfo->u4FrameRate = 2397;
                break;
            case 25:
                prVdecEsInfo->u4FrameRate = 2500;
                break;
            case 30:
                prVdecEsInfo->u4FrameRate = 3000;
                break;
            case 29:
                prVdecEsInfo->u4FrameRate = 2997;
                break;
            case 47:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    prVdecEsInfo->u4FrameRate = 2397;
                }
                else
                {
                    LOG(1,"Framerate warning .....\n");
                    prVdecEsInfo->u4FrameRate = 3000;
                }
                break;
            case 50:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {   
                    prVdecEsInfo->u4FrameRate = 2500;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 5000;
                }
                break;
            case 60:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {    
                    prVdecEsInfo->u4FrameRate = 3000;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 6000;
                }
                break;
            case 59:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    prVdecEsInfo->u4FrameRate = 2997;
                }
                else
                {
                    prVdecEsInfo->u4FrameRate = 5997;
                }
                break;
            case 120:
                prVdecEsInfo->u4FrameRate = 12000;
                break;
            case 240:
                prVdecEsInfo->u4FrameRate = 24000;
                break;
            default:
                prVdecEsInfo->u4FrameRate = 0;
                break;
        }
    }
    else
    {
        prVdecEsInfo->u4FrameRate = 0;
    }

    return E_VDEC_OK;
}
#endif


// **************************************************************************
// Function : VOID _VDEC_H264Performance(UINT32 ucEsId,UINT32 u4VDecID);
// Description : measure H264 decode time and bandwidth
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : none
// **************************************************************************
#ifdef VDEC_H264_PERFORMANCE
void _VDEC_H264Performance(UINT32 ucEsId,UINT32 u4VDecID)
{
    UINT32 u4Cnt = 0, u4Clk = 0;
    UINT32 u4MCBandwidth = 0, u4PPRatioDiv = 0;
    UINT32 u8Ratio = 0, u8PPBandwidth = 0, u8TotalBandwidth = 0;
    VDEC_H264_INFO_T* prVdec;
    
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    prVdec = rH264DecInfo[ucEsId];
#else
    prVdec = (VDEC_H264_INFO_T*)&rH264DecInfo[ucEsId];
#endif

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
    
    //VDecPrintf(ucEsId,VDEC_LOG_DecTime,"Hw dec-time %d MC_478 %d u4Clk %d\n",
                    //u4Cnt/u4Clk, u4VDecReadMC(u4VDecID, (478 << 2)), u4Clk);
    
    if(prVdec->rDecParam.ucVDecStatus == REAL_DECODE)
    {        
        if(prVdec->rDecParam.ucDualCoreMode)
        {
            u4MCBandwidth = ((u4VDecReadMC(VDEC_VLD0,(476 << 2)) + u4VDecReadMC(VDEC_VLD1,(476 << 2))) << 4); //16Byte Unit
            u4PPRatioDiv = ((u4VDecReadMC(VDEC_VLD0,(723 << 2)) + u4VDecReadMC(VDEC_VLD1,(723 << 2))) << 4);
            u8PPBandwidth = (((((prVdec->rDecParam.u4PicW + 15) >> 4) << 4) * 
                                (((prVdec->rDecParam.u4PicH + 31) >> 5) << 5) * 3) >> 1);
        }
        else
        {
            u4MCBandwidth = (u4VDecReadMC(u4VDecID,(476 << 2)) << 4); //16Byte Unit
            u4PPRatioDiv = (u4VDecReadMC(u4VDecID,(723 << 2)));
            u8PPBandwidth = (((((prVdec->rDecParam.u4PicW + 15) >> 4) << 4) * 
                                (((prVdec->rDecParam.u4PicH + 31) >> 5) << 5) * 3) >> 1);
        }
        if((prVdec->rDecParam.prSliceHdr->u4SliceType == I_Slice) ||
                (prVdec->rDecParam.prSliceHdr->u4SliceType == I_Slice_ALL))
        {
        }
        else
        {
            u8Ratio = ((u4MCBandwidth * 100)/u4PPRatioDiv);
            u8PPBandwidth = (u8PPBandwidth * u8Ratio)/100;
        }                
        u8TotalBandwidth = u4MCBandwidth + u8PPBandwidth;                 
        VDecPrintf(ucEsId,VDEC_LOG_BANDWIDTH,"[%s] Bandwidth MC %dBps PP %dBps Total %dBps \n",
                                                                        ((prVdec->rDecParam.ucDualCoreMode) ? "Mcore" : "Score"),
                                                                        u4MCBandwidth, u8PPBandwidth, u8TotalBandwidth); 
    }
}
#endif


// **************************************************************************
// Function : VOID _VDEC_H264CompCRC(UINT32 ucEsId,UINT32 u4VDecID);
// Description : get H264 crc
// Parameter : ucEsId : instance id, u4VDecID : video decoder hardware ID
// Return      : none
// **************************************************************************
#ifdef VDEC_H264_CRC_COMPARE
void _VDEC_H264CompCRC(UINT32 ucEsId,UINT32 u4VDecID)
{
	INT32 i;
	UINT32 u4HW_Y_Result[4] = {0};
	UINT32 u4HW_CbCr_Result[4] = {0};
	
	VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

	if (prVdecEsInfo->ucDualCoreMode)
	{
		printk("Core0 Y CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_Y_Result[i] = u4VDecReadMCoreCRC(u4VDecID, (i+193)<<2);		
			printk("0x%x ", u4HW_Y_Result[i]);
		}
		printk("\n");
		printk("Core0 CbCr CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_CbCr_Result[i] = u4VDecReadMCoreCRC(u4VDecID, (i+197)<<2);		
			printk("0x%x ", u4HW_CbCr_Result[i]);
		}
		printk("\n");
		printk("Core1 Y CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_Y_Result[i] = u4VDecReadMCoreCRC(u4VDecID, (i+201)<<2);
			printk("0x%x ", u4HW_Y_Result[i]);
		}	 
		printk("\n");
		printk("Core1 CbCr CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_CbCr_Result[i] = u4VDecReadMCoreCRC(u4VDecID, (i+205)<<2);
			printk("0x%x ", u4HW_CbCr_Result[i]);
		}
		printk("\n");
	}
	else
	{		
		printk("Core0 Y CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_Y_Result[i] = u4VDecReadCRC(u4VDecID, (i+2)<<2);	
			printk("0x%x ", u4HW_Y_Result[i]);
		}
		printk("\n");		
		printk("Core0 CbCr CRC: ");
		for (i = 0; i < 4; i++)
		{
			u4HW_CbCr_Result[i] = u4VDecReadCRC(u4VDecID, (i+6)<<2);		
			printk("0x%x ", u4HW_CbCr_Result[i]);
		}
		printk("\n");
	}
}
#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



