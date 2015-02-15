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
 * $RCSfile: vdec_h264dec.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_h264api.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
#include "x_ckgen.h"
#include "srm_drvif.h"
#include "vdec_h264hw.h"
#include "vdec_h264util.h"
#include "vdec_h264dec.h"
#include "vdec_h264api.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "x_typedef.h"
#include "x_timer.h"
#include "stc_drvif.h"
#include "x_util.h"
#ifdef FBM_DPB_CTRL
#include "fbm_drvif.h"
#endif
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif
#include "vdec_drvif.h"
#include "drv_dbase.h"
#include "drvcust_if.h"
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
#include "tz_if.h"
#endif
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#if defined(CC_MT5890)
#include "../lae/vdec_lae.h"
#endif
#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
#include "gdma_if.h"
#endif
#ifdef __KERNEL__
#include <linux/time.h>
#endif

//for emulation
//EXTERN void _VdecNotify(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4);

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define POC_abs(x)                  (((x) >= 0) ? (x) : -(x))
#define FB_ALIGN  0xF
#define FRAME_RATE_MAX 10
#define MAX_ES_NUM VDEC_MAX_ES
#define _MAKE_64X(val) ((((val) + 0x3F) >> 6) << 6)
#define _MAKE_16X(val) ((((val) + 0xF) >> 4) << 4)

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static HAL_TIME_T rTimeHwS, rTimeHwDt;
#ifdef VDEC_TIME_PROFILE
static UINT32 u4DecCycle = 0, u4MaxDecCycle = 0;
#endif
#ifdef CC_STRESS_TEST_CRC_CHK
static UINT32 u4DramCrcMC_R = 0, u4DramCrcPP_W = 0;
#endif

//follow FBM defined MPEG_FRAME_RATE_24_...

static UINT64 au8Scale[FRAME_RATE_MAX] =
{
    0,
    375374700,
    3750,
    3600,
    300300300,
    3000,
    1800,
    150075037,
    1500,
    9000
};
static UINT32 au4Rate[FRAME_RATE_MAX] =
{
    0,
    100000,
    1,
    1,
    100000,
    1,
    1,
    100000,
    1,
    1
};

static UINT64 au8Scale64[FRAME_RATE_MAX] =
{
    0,
    (VDEC_U8_SECOND_CLOCK*100000*1000)/(23976),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(24000),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(25000),
    (VDEC_U8_SECOND_CLOCK*100000*1000)/(29970),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(30000),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(50000),
    (VDEC_U8_SECOND_CLOCK*100000*1000)/(59940),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(60000),
    (VDEC_U8_SECOND_CLOCK*1     *1000)/(10000)
};
static UINT32 au4Rate64[FRAME_RATE_MAX] =
{
    0,
    100000,
    1,
    1,
    100000,
    1,
    1,
    100000,
    1,
    1
};
#ifndef AVC_IRQ_DISABLE
static HANDLE_T _hVdecFinSema[4] = {(HANDLE_T)NULL,(HANDLE_T)NULL,(HANDLE_T)NULL,(HANDLE_T)NULL};
static BOOL _bTimeOutFlag = FALSE;
#endif

CHAR quant_intra_default[16] =
{
    6, 13, 20, 28,
    13, 20, 28, 32,
    20, 28, 32, 37,
    28, 32, 37, 42
};

CHAR quant_inter_default[16] =
{
    10, 14, 20, 24,
    14, 20, 24, 27,
    20, 24, 27, 30,
    24, 27, 30, 34
};

CHAR quant8_intra_default[64] =
{
    6, 10, 13, 16, 18, 23, 25, 27,
    10, 11, 16, 18, 23, 25, 27, 29,
    13, 16, 18, 23, 25, 27, 29, 31,
    16, 18, 23, 25, 27, 29, 31, 33,
    18, 23, 25, 27, 29, 31, 33, 36,
    23, 25, 27, 29, 31, 33, 36, 38,
    25, 27, 29, 31, 33, 36, 38, 40,
    27, 29, 31, 33, 36, 38, 40, 42
};

CHAR quant8_inter_default[64] =
{
    9, 13, 15, 17, 19, 21, 22, 24,
    13, 13, 17, 19, 21, 22, 24, 25,
    15, 17, 19, 21, 22, 24, 25, 27,
    17, 19, 21, 22, 24, 25, 27, 28,
    19, 21, 22, 24, 25, 27, 28, 30,
    21, 22, 24, 25, 27, 28, 30, 32,
    22, 24, 25, 27, 28, 30, 32, 33,
    24, 25, 27, 28, 30, 32, 33, 35
};

CHAR quant_org[16] =   //to be use if no q matrix is chosen
{
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16,
    16, 16, 16, 16
};

CHAR quant8_org[64] =   //to be use if no q matrix is chosen
{
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16
};

#ifdef VDEC_TIME_PROFILE
static UINT32 u4DelayClock;
static UINT32 u4DramCycle;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _vAssignQuantParam(UINT32 u4VDecID, VDEC_AVC_DecPrm *prDecPrm);
static void _vPartitionDPB(VDEC_AVC_DecPrm *prDecPrm);
static void _vAllocateFBuf(VDEC_AVC_DecPrm *prDecPrm);
//static void _vFillFrameNumGap(VDEC_AVC_DecPrm *prDecPrm);
static void _vPrepareFBufInfo(VDEC_AVC_DecPrm *prDecPrm);
static BOOL _fgChkRefInfo(const VDEC_AVC_DecPrm* prDecPrm, UINT32 u4FBufIdx, UINT32 u4RefType);
static void _vInsertRefPicList(const VDEC_AVC_DecPrm* prDecPrm, VDEC_REF_PIC_LIST_T* prRefPicList,
                               INT32 i4CurrPOC, UINT32 u4RefPicListInfo);
static void _vSetupPRefPicList(UINT32 u4VDecID, const VDEC_REF_PIC_LIST_T* prRefPicList,
                               VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT32 *pu4RefIdx,
                               UINT32 u4TFldListIdx, UINT32 u4BFldListIdx);
static void _vSetPRefPicList(UINT32 u4VDecId, VDEC_AVC_DecPrm* prDecPrm);
static void _vSetupBRefPicList(UINT32 u4VDecID, const VDEC_REF_PIC_LIST_T* prRefPicList,
                               VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT32 *pu4RefIdx,
                               UINT32 u4TFldListIdx, UINT32 u4BFldListIdx, BOOL *pfgDiff);

static void _vSetBRefPicList(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm);
static void _vSetPicRefType(VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT8 u1RefType);
static UINT8 _bGetPicRefType(VDEC_FBUF_INFO_T rFBufInfo, UINT8 u1PicStruct);
static void _vClrPicRefInfo(VDEC_AVC_DecPrm* prDecPrm, UINT8 u1PicType, UINT16 u2FBufIdx);
static void _vAdapRefPicmarkingProce(VDEC_AVC_DecPrm* prDecPrm);
static BOOL _bNotifyAudStartPlay(UCHAR ucStcId, UINT32 u4PTS);
//static void _vECBufSetting(const VDEC_AVC_DecPrm* prDecPrm, UINT32 u4RefCnt, UINT32 u4RefEntryCnt);
#ifdef FBM_DPB_CTRL
static UCHAR  _VDEC_GetFRFromSPS(VDEC_AVC_DecPrm* prDecPrm);
static UCHAR  _VDEC_GetFRFromContainer(VDEC_AVC_DecPrm* prDecPrm);
extern void FBM_UpdateReferenceList(UCHAR ucFbgId, UCHAR ucFbId, BOOL fgReference);
static void _VDEC_ResizeSmallImage(UCHAR ucEsId, FBM_SEQ_HDR_T *prFbmSeqHdr);
//UINT32 u4VDecFrmMapFrameRate(UINT32 u4InstID,UINT32 u4MPEGFrameRate);
#endif
#ifdef ENABLE_MULTIMEDIA
static BOOL _VDEC_IsxvYCC(VDEC_AVC_DecPrm* prDecPrm);
#endif
#ifdef CC_DUAL_CORE_DEC
void vLAEPartitionDPB(VDEC_AVC_DecPrm* prDecPrm);
void vLAEAllocateFBuf(VDEC_AVC_DecPrm* prDecPrm);
void vLAEPrepareFBufInfo(VDEC_AVC_DecPrm* prDecPrm);
VOID u4VPmode(UINT32 u4VDecID, UINT32 u4Err_mbx, UINT32 u4Err_mby, UINT32 u4mbx, UINT32 u4mby, VDEC_AVC_DecPrm *prDecPrm);
#endif
#ifndef AVC_IRQ_DISABLE
static void _VDEC_InitIsr(void);
#endif


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

// **************************************************************************
// Function : void _AVLDIsr(UINT16 u2Vector);
// Description : clear interrupt
// Parameter : u2Vector : interrupt id
// Return      : none
// **************************************************************************
#ifndef AVC_IRQ_DISABLE
void _AVLDIsr(UINT16 u2Vector)
{
    switch (u2Vector)
    {
        case VECTOR_VDEC:
    #ifdef IS_POST_MT5368
            //Choose either one, vdec_misc or bim clear, should be the same with all other codec
            BIM_ClearIrq(VECTOR_VDEC);
    #endif
            //unlock semaphore        
            VERIFY(x_sema_unlock(_hVdecFinSema[VDEC_VLD0]) == OSR_OK);
            break;
#ifdef CC_DUAL_CORE_DEC
        case VECTOR_VDEC_CORE1:
    #ifdef IS_POST_MT5368
            //Choose either one, vdec_misc or bim clear, should be the same with all other codec
            BIM_ClearIrq(VECTOR_VDEC_CORE1);
    #endif
            //unlock semaphore        
            VERIFY(x_sema_unlock(_hVdecFinSema[VDEC_VLD1]) == OSR_OK);
            break;
        case VECTOR_VDEC_LAE0:
    #ifdef IS_POST_MT5368
            //Choose either one, vdec_misc or bim clear, should be the same with all other codec
            BIM_ClearIrq(VECTOR_VDEC_LAE0);
    #endif
            //unlock semaphore        
            VERIFY(x_sema_unlock(_hVdecFinSema[VDEC_LAE0]) == OSR_OK);
            break;
        case VECTOR_VDEC_LAE1:
    #ifdef IS_POST_MT5368
            //Choose either one, vdec_misc or bim clear, should be the same with all other codec
            BIM_ClearIrq(VECTOR_VDEC_LAE1);
    #endif
            //unlock semaphore        
            VERIFY(x_sema_unlock(_hVdecFinSema[VDEC_LAE1]) == OSR_OK);
            break;
#endif
        default:
            if (u4VDecReadAVCVLD(VDEC_VLD0, RO_AVLD_COMPLETE))
            {
                _bTimeOutFlag = FALSE;
            }
            else
            {
                _bTimeOutFlag = TRUE;
            }
    #ifdef IS_POST_MT5368
            //Choose either one, vdec_misc or bim clear, should be the same with all other codec
            BIM_ClearIrq(VECTOR_VDEC);
    #endif
            //unlock semaphore        
            VERIFY(x_sema_unlock(_hVdecFinSema[VDEC_VLD0]) == OSR_OK);
            break;
    }

}


// **************************************************************************
// Function : static void _VDEC_InitIsr(void);
// Description : init interrupt semaphore
// Parameter : none
// Return      : none
// **************************************************************************
static void _VDEC_InitIsr(void)
{   
    if (_hVdecFinSema[VDEC_VLD0] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&_hVdecFinSema[VDEC_VLD0], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }    
    #ifdef CC_DUAL_CORE_DEC   
    if (_hVdecFinSema[VDEC_VLD1] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&_hVdecFinSema[VDEC_VLD1], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    if (_hVdecFinSema[VDEC_LAE0] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&_hVdecFinSema[VDEC_LAE0], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    if (_hVdecFinSema[VDEC_LAE1] == (HANDLE_T)NULL)
    {
        VERIFY(x_sema_create(&_hVdecFinSema[VDEC_LAE1], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
    #endif
}
#endif


// *********************************************************************
// Function    : BOOL _bNotifyAudStartPlay(void)
// Description : notify audio when first video frame ready to play
// Parameter   : None
// Return      : None
// *********************************************************************
static BOOL _bNotifyAudStartPlay(UCHAR ucStcId, UINT32 u4PTS)
{
    if (STC_SetStartPts(AV_SYNC_TARGET_VDO, ucStcId, u4PTS) != STC_VALID)
    {
        LOG(3, "STC_SetStartPts Invalid\n");
        return FALSE;
    }
    LOG(6, "STC_SetStartPts 0x%x\n", u4PTS);

    return TRUE;
}


// *********************************************************************
// Function : void AssignQuantParam(void)
// Description :
// Parameter :
// Return    :
// *********************************************************************
static void _vAssignQuantParam(UINT32 u4VDecID, VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 i;
    CHAR *pactList[8];

    ASSERT(prDecPrm != NULL);

    for (i = 0; i < 8; i++)
    {
        pactList[i] = quant_intra_default;        //initial
        prDecPrm->afgUserScalingListPresentFlag[i] = FALSE;
    }

    if ((!prDecPrm->prPPS->fgPicScalingMatrixPresentFlag) && (!prDecPrm->prSPS->fgSeqScalingMatrixPresentFlag))
    {
        prDecPrm->fgUserScalingMatrixPresentFlag = FALSE;
        // do nothing
    }
    else
    {
        prDecPrm->fgUserScalingMatrixPresentFlag = TRUE;
        if (prDecPrm->prSPS->fgSeqScalingMatrixPresentFlag) // check sps first
        {
            for (i = 0; i < 8; i++)
            {
                prDecPrm->afgUserScalingListPresentFlag[i] = TRUE;
                //need to have the flag for pps info setting, for hw to reload data from sram
                //_VDEC_WriteScalingList only update the sram data
                if (i < 6)
                {
                    if (!prDecPrm->prSPS->fgSeqScalingListPresentFlag[i]) // fall-back rule A
                    {
                        if ((i == 0) || (i == 3))
                        {
                            pactList[i] =  (i == 0) ? quant_intra_default : quant_inter_default;
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                        else
                        {
                            pactList[i] =  pactList[i-1];
                            prDecPrm->afgUserScalingListPresentFlag[i] = prDecPrm->afgUserScalingListPresentFlag[i-1];
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                    }
                    else // fall-back rule B
                    {
                        if (prDecPrm->prSPS->fgUseDefaultScalingMatrix4x4Flag[i])
                        {
                            pactList[i] = (i < 3) ? quant_intra_default : quant_inter_default;
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                        else
                        {
                            prDecPrm->afgUserScalingListPresentFlag[i] = TRUE;
                            pactList[i] = prDecPrm->prSPS->ppcScalingList4x4[i];
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                    }
                }
                else
                {
                    if (!prDecPrm->prSPS->fgSeqScalingListPresentFlag[i] || prDecPrm->prSPS->fgUseDefaultScalingMatrix8x8Flag[i-6]) // fall-back rule A
                    {
                        pactList[i] = (i == 6) ? quant8_intra_default : quant8_inter_default;
                        _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                    }
                    else
                    {
                        prDecPrm->afgUserScalingListPresentFlag[i] = TRUE;
                        pactList[i] = prDecPrm->prSPS->ppcScalingList8x8[i-6];
                        _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                    }
                }
            }
        }

        if (prDecPrm->prPPS->fgPicScalingMatrixPresentFlag) // then check pps
        {
            for (i = 0; i < 8; i++)
            {
                if (i < 6)
                {
                    if (!prDecPrm->prPPS->fgPicScalingListPresentFlag[i]) // fall-back rule A
                    {
                        if ((i == 0) || (i == 3))
                        {
                            if (!prDecPrm->prSPS->fgSeqScalingMatrixPresentFlag)
                            {
                                pactList[i] = (i == 0) ? quant_intra_default : quant_inter_default;
                                _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                            }
                        }
                        else
                        {
                            pactList[i] = pactList[i-1];
                            prDecPrm->afgUserScalingListPresentFlag[i] = prDecPrm->afgUserScalingListPresentFlag[i-1];
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                    }
                    else
                    {
                        if (prDecPrm->prPPS->fgUseDefaultScalingMatrix4x4Flag[i])
                        {
                            pactList[i] = (i < 3) ? quant_intra_default : quant_inter_default;
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                        else
                        {
                            pactList[i] = prDecPrm->prPPS->ppcScalingList4x4[i];
                            prDecPrm->afgUserScalingListPresentFlag[i] = TRUE;
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                    }
                }
                else
                {
                    if (!prDecPrm->prPPS->fgPicScalingListPresentFlag[i]) // fall-back rule B
                    {
                        if (!prDecPrm->prSPS->fgSeqScalingMatrixPresentFlag)
                        {
                            pactList[i] = (i == 6) ? quant8_intra_default : quant8_inter_default;
                            _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                        }
                    }
                    else if (prDecPrm->prPPS->fgUseDefaultScalingMatrix8x8Flag[i-6])
                    {
                        pactList[i] = (i == 6) ? quant8_intra_default : quant8_inter_default;
                        _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                    }
                    else
                    {
                        pactList[i] = prDecPrm->prPPS->ppcScalingList8x8[i-6];
                        prDecPrm->afgUserScalingListPresentFlag[i] = TRUE;
                        _VDEC_WriteScalingList(u4VDecID, i, pactList[i]);
                    }
                }
            }
        }
    }
}


// *********************************************************************
// Function    : void vPartitionDPB(void)
// Description : Set VDec related parameters
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vPartitionDPB(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 i, j, u4FbCnt = 0;
    UINT32 u4DramPicSize;
    UINT32 u4DramPicArea;
    UINT32 u4YAddr, u4CAddr, u4MVAddr,u4MVAddr1;
    #if (defined (CC_MT5890))
    UINT32 u4YLenAddr, u4CLenAddr;
    #endif
#ifdef ENABLE_MULTIMEDIA
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
#endif
    
    ASSERT(prDecPrm != NULL);
    if (prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "PartitionDPB ViewIdx Err!\n");
        return;
    }
    
    // Real pic size w=16x, h=32x
    u4DramPicSize = ((((prDecPrm->u4PicW + 15) >> 4) * ((prDecPrm->u4PicH + 31) >> 5)) << 9);
    // 1 pic area = Y + CbCr + restored MV
    u4DramPicArea = ((((u4DramPicSize * 7) >> 2) + 511) >> 9) << 9;
    
    #if (defined (CC_MT5890))
    if (prDecPrm->fgIs10BitMode)
    {
        u4DramPicSize = u4DramPicSize*5/4;
    }
    #endif
#ifdef FBM_DPB_CTRL
#ifdef ENABLE_MULTIMEDIA
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);

    if ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) && (!prDecPrm->fgReCreateFBG))
    {
        if((prVdecEsInfo->u4SeamlessWidth > 0) && (prVdecEsInfo->u4SeamlessHeight > 0))
        {
            //frame number is decided by seamless target width/height
            prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] =
                    (UINT32)(FBM_GetFrameBufferMvNs(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx])); // 2 lock frame
    
            if(prDecPrm->prSPS->u4NumRefFrames >= prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx])
            {
                LOG(1, "Err!! RefNum %d in seamless mode, target res %dx%d\n", 
                        prDecPrm->prSPS->u4NumRefFrames, prVdecEsInfo->u4SeamlessWidth, prVdecEsInfo->u4SeamlessHeight);
                prDecPrm->fgNotSupport = TRUE;
            }
        }
        else
        {
            LOG(1, "Err!! seamless mode, target res %dx%d\n", 
                    prVdecEsInfo->u4SeamlessWidth, prVdecEsInfo->u4SeamlessHeight);
            prDecPrm->fgNotSupport = TRUE;
        }
    }
    else if(prVdecEsInfoKeep->fgVPush)
   {
        if((prDecPrm->u4PicW >= 1920)&&(prDecPrm->u4PicH>=1080))//FHD case,shoulde have at most 4 reference, the latency will not large
        {
               prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]=
                    (prDecPrm->u4DPBSize/(((prDecPrm->u4PicW*prDecPrm->u4PicH)*3)>>1))+1;
        }
        else
        {
            if(!(prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS))
            {
                prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]=prDecPrm->prSPS->u4NumRefFrames + 1;
            }
        }
        if(prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] < prDecPrm->prSPS->u4NumRefFrames + 1)
        {
            prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]=prDecPrm->prSPS->u4NumRefFrames + 1;
        }

        if(prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] > (UINT32)(FBM_GetFrameBufferMvNs(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx])))
        {
            LOG(1, "Err!! MaxFBNum %d,Not Support\n",prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]); 
            prDecPrm->fgNotSupport = TRUE;
        }
    }
    else
    #endif
    {
        prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = prDecPrm->prSPS->u4NumRefFrames + 1;
    }
            
    //check if ref frame num over spec
    u4FbCnt = (UINT32)(FBM_GetFrameBufferMvNs(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx]));  // 2 lock frame
    if (u4FbCnt < (prDecPrm->prSPS->u4NumRefFrames +1))
    {
        prDecPrm->fgNotSupport = TRUE;
        LOG(2, "!!NumRef %d > Level Limit, Not support\n", prDecPrm->prSPS->u4NumRefFrames);
    }
        
    #if ((!defined(CC_MT5398)) && (!defined(CC_MT5399)) && (!defined(CC_MT5890)))
    if(((prDecPrm->u4PicW) > 1920) || ((prDecPrm->u4PicH) > 1088))
    {
        prDecPrm->fgNotSupport = TRUE;
        LOG(2,"!!Over HW support W&H (%d,%d)!\n",prDecPrm->u4PicW,prDecPrm->u4PicH);
    }
    #endif
        
    prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] =
        (prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] > 17) ? ((UINT32)17) : prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
    FBM_GetFrameBufferAddr(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], 0, &u4YAddr, &u4CAddr);
    FBM_GetFrameBufferAddrMv(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], 0, &u4MVAddr);
        //MC_WRITE32(RW_MC_P_LIST0, PHYSICAL(u4YAddr));            
#if (defined (CC_MT5890))
    if (prDecPrm->fgIsUFOMode)
    {
        FBM_GetFrameBufferAddrExt(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], 0, &u4YLenAddr,&u4CLenAddr);
    }
#endif
#else
    prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] =
            (prDecPrm->u4DPBSize / (((prDecPrm->u4PicW * prDecPrm->u4PicH) * 3) >> 1)) + 2;
    if (prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] > 18)
    {
        prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = 18;     //16 reference frame + 1 cur dec + 1 cur disp
    }
#endif
    ASSERT(prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] <= 18);
    for (j = prDecPrm->u1CurViewIdx; j < MAX_VIEW_NUM; j++)
            //j = prDecPrm->u1CurViewIdx;
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
            
        if(prDecPrm->aucFbgId[j] != 0xFF)
        {
            FBM_GetFrameBufferAddrMv(prDecPrm->aucFbgId[j], 0,&u4MVAddr1);
            LOG(6,"view %d,0,mvaddr1 %x\n",j,u4MVAddr1);
            FBM_GetFrameBufferAddrMv(prDecPrm->aucFbgId[j], (prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]-1),&u4MVAddr1);
            LOG(6,"view %d,0,mvaddr1max %x\n",j,u4MVAddr1);
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
        #if (defined (CC_MT5890))
            if (prDecPrm->fgIsUFOMode)
            {
                prDecPrm->arFBufInfo[j][i].u4YLenStartAddr = u4YLenAddr;
                prDecPrm->arFBufInfo[j][i].u4CLenStartAddr = u4CLenAddr;
                prDecPrm->arFBufInfo[j][i].u4YLenOffset = prDecPrm->arFBufInfo[j][i].u4YLenStartAddr - prDecPrm->arFBufInfo[j][i].u4YAddr;
                prDecPrm->arFBufInfo[j][i].u4CLenOffset = prDecPrm->arFBufInfo[j][i].u4CLenStartAddr - prDecPrm->arFBufInfo[j][i].u4CAddr;
                prDecPrm->arFBufInfo[j][i].u4YLen = (u4DramPicSize >> 8);
                prDecPrm->arFBufInfo[j][i].u4CLen = (prDecPrm->arFBufInfo[j][i].u4YLen >> 1);
            }
        #endif
#else
            prDecPrm->arFBufInfo[j][i].u4YAddr = prDecPrm->u4DPBAddr + (i * u4DramPicArea);
            prDecPrm->arFBufInfo[j][i].u4CAddr = prDecPrm->arFBufInfo[j][i].u4YAddr + u4DramPicSize;
            prDecPrm->arFBufInfo[j][i].u4MVBufAddr = prDecPrm->arFBufInfo[j][i].u4CAddr + (u4DramPicSize >> 1);
#endif
        }
    }
    // current reset to 0 when DPB partition.
    prDecPrm->prCurrFBufInfo = &prDecPrm->arFBufInfo[0][0];
}


// *********************************************************************
// Function    : void vAllocateFBuf()
// Description : Allocate decoding frm buff in DPB
// Parameter   :
// Return      : None
// *********************************************************************
static void _vAllocateFBuf(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 i;
    //INT32 i4MinPOC;
    //UINT32 u4MinPOCFBufIdx = 0xFF, u4ClrPOCFBufIdx = 0xFF, u4RefPOCFBufIdx = 0;
#ifdef VDEC_TIME_PROFILE
    HAL_TIME_T rTimeFbS, rTimeFbE, rTimeFbDt;
#endif
#ifdef FBM_DPB_CTRL
    FBM_SEQ_HDR_T *prFbmSeqHdr;
    FBM_PIC_HDR_T * prFbmPicHdr;
    UCHAR ucFbId;//, ucFbStatus;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT8 u1ViewIdx;

#endif
    HAL_TIME_T rTime;
    ASSERT(prDecPrm != NULL);
    if (prDecPrm == NULL)
    {
        LOG(2, "prDecPrm Null!!\n");
        return;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "Alloc ViewIdx Err!\n");
        return;
    }
#ifdef FBM_DPB_CTRL
    //check duplicate frame_num
    prDecPrm->u4DupFrmNumId = 0xFF;
    ASSERT(prDecPrm->u4MaxFBufNum <= 18);
    for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
    {
        if (!fgIsNonRefFBuf(prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType,
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType,
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType))
        {
            if (!_fgChkRefInfo(prDecPrm, i + (u1ViewIdx << 8), LREF_PIC) &&
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum == prDecPrm->prSliceHdr->u4FrameNum) &&
                    fgIsRefPic(prDecPrm->u1NalRefIdc))
            {
                LOG(3, "Dup Frm_Num\n");
                //can't clr buffer here, may still need to be reference, clr in _VDEC_OutputDispPic
                if (!prDecPrm->fgIsMVC)
                {
                    prDecPrm->u4DupFrmNumId = i;
                }
                else
                {
                    prDecPrm->fgAllocFbFail = TRUE;
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucPreAllocFbId,
                                             FBM_FB_STATUS_EMPTY);
                    prDecPrm->ucPreAllocFbId = FBM_FB_ID_UNKNOWN;
                    return;
                }
            }
        }
    }
#endif
    if (prDecPrm->u1DecFBufIdx > 17)
    {
        ASSERT(prDecPrm->u1DecFBufIdx <= 17);
        return;
    }
    prDecPrm->prCurrFBufInfo = &prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u1DecFBufIdx];
    if(!prDecPrm->prCurrFBufInfo)
    {
        LOG(0,"prDecPrm->prCurrFBufInfo == NULL u1ViewIdx %d prDecPrm->u1DecFBufIdx %d\n",u1ViewIdx,prDecPrm->u1DecFBufIdx);
        return;
    }
    if (prDecPrm->prSliceHdr)
    {
        prDecPrm->prCurrFBufInfo->u4SliceType = prDecPrm->prSliceHdr->u4SliceType;
    }
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
#ifdef IS_POST_MT5368
        prDecPrm->prCurrFBufInfo->u4ViewIDFb = prDecPrm->u4CurViewID;
#else
        prDecPrm->prCurrFBufInfo->u4ViewIDFb = (UINT32)prDecPrm->u1CurViewIdx;
#endif

    prDecPrm->aprCurrFBufInfo[u1ViewIdx] = prDecPrm->prCurrFBufInfo;
    prDecPrm->au1DecFBufIdx[u1ViewIdx] = prDecPrm->u1DecFBufIdx;
    //default value, will refine in SetCurrPOC function, find the frame with same POC in base view
    if (u1ViewIdx == 0)
    {
        prDecPrm->u4POCFbIdx = prDecPrm->u1DecFBufIdx;
    }
    if (prDecPrm->fgInterView)
    {
        prDecPrm->prCurrFBufInfo->fgIsInterview = TRUE;
    }
#endif

#ifdef FBM_DPB_CTRL
    //if freeze, still decoding normally, shorten delay time when unfreeze
    //but get fb from dispQ(set the attribute from disp to empty)
    if (FBM_ChkFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_FREEZE))
    {
        ucFbId = FBM_GetFrameBufferFromDispQ(prDecPrm->aucFbgId[u1ViewIdx]);
        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], ucFbId, FBM_FB_STATUS_EMPTY);
    }

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeFbS);
#endif
#if 0   //move to _VDEC_SliceNonIDR
    prDecPrm->prCurrFBufInfo->ucFbId = FBM_GetEmptyFrameBuffer(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->u4WaitFbTime);
#else
    prDecPrm->prCurrFBufInfo->ucFbId = prDecPrm->ucPreAllocFbId;
    prDecPrm->ucPreAllocFbId = FBM_FB_ID_UNKNOWN;
#endif
    prVdecEsInfo->ucFbId = prDecPrm->prCurrFBufInfo->ucFbId;
    if (!prDecPrm->fgIsFirstFrm)
    {   //!display until find suitable RAP
        prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
        #if BDP_LOG_STYLE
        VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                         "[H264 VDEC%d]: _vAllocateFBuf wait RAP PicType(0X%X), Pts(0x%x)\n",
                         prDecPrm->ucEsId,
                         prVdecEsInfoKeep->rPesInfo.ucPicType,
                         prVdecEsInfoKeep->rPesInfo.u4PTS);
        #endif
    }
    else if (prDecPrm->fgFirstFrmTag)
    {
        prDecPrm->prCurrFBufInfo->fgFirstFrmTag = TRUE;
        prDecPrm->fgFirstFrmTag = FALSE;
    }

#if !defined(CC_MT5365) && !defined(IS_POST_MT5395)
    if (prVdecEsInfo->fgMMPlayback)
    {
        if ((prVdecEsInfo->u4MMSpeed == STC_SPEED_TYPE_FORWARD_2X) &&
                ((!fgIsRefPic(prDecPrm->u1NalRefIdc))))
        {
            prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
        }
    }
#endif

    prDecPrm->u4CurDecMs = 0;
#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeFbE);
    HAL_GetDeltaTime(&rTimeFbDt, &rTimeFbS, &rTimeFbE);
    prDecPrm->u4CurWaitFbMicro += rTimeFbDt.u4Micros;///1000;
    if (prDecPrm->u4CurWaitFbMicro >
            (33000 * (FBM_GetFrameBufferNs(prDecPrm->aucFbgId[u1ViewIdx]) - prDecPrm->u4MaxFBufNum))) //33 ms
    {
        LOG(6, "WFb %dus!!\n", prDecPrm->u4CurWaitFbMicro);
    }
    prDecPrm->u4CurDecCycle = 0;
    prDecPrm->u4CurDramCycle = 0;
#endif

    if (prDecPrm->prCurrFBufInfo->ucFbId == FBM_FB_ID_UNKNOWN)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "AllocFB Fail!!\n");
        return;
    }
    else
    {
        prDecPrm->fgAllocFbFail = FALSE;
    }

#ifdef VDEC_TIME_PROFILE
    if ((prDecPrm->prSPS->rVUI.u4TimeScale > 0) &&
            ((rTimeFbDt.u4Micros / 1000) >
             ((prDecPrm->prSPS->rVUI.u4NumUnitsInTick * 1000) / prDecPrm->prSPS->rVUI.u4TimeScale)))
    {
        prDecPrm->u4WaitFbCnt++;
    }
#endif

    FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, fgIsRefPic(prDecPrm->u1NalRefIdc));
    FBM_GetFrameBufferAddr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
                           &prDecPrm->prCurrFBufInfo->u4YAddr, &prDecPrm->prCurrFBufInfo->u4CAddr);  
#if (defined (CC_MT5890))
    if (prDecPrm->fgIsUFOMode)
    {
        FBM_GetFrameBufferAddrExt(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, 
                            &prDecPrm->prCurrFBufInfo->u4YLenStartAddr, &prDecPrm->prCurrFBufInfo->u4CLenStartAddr);
        prDecPrm->prCurrFBufInfo->u4YLenOffset = prDecPrm->prCurrFBufInfo->u4YLenStartAddr - prDecPrm->prCurrFBufInfo->u4YAddr;
        prDecPrm->prCurrFBufInfo->u4CLenOffset = prDecPrm->prCurrFBufInfo->u4CLenStartAddr - prDecPrm->prCurrFBufInfo->u4CAddr;
        prDecPrm->prCurrFBufInfo->u4YLen = (prDecPrm->prCurrFBufInfo->u4DramPicSize >> 8);      
        prDecPrm->prCurrFBufInfo->u4CLen = (prDecPrm->prCurrFBufInfo->u4YLen >> 1);
        LOG(9, "Y=0x%x, C=0x%x, YLenAdd=0x%x, CLenAdd=0x%x, Ylenoff=%x, Clenoff=%x\n",
        prDecPrm->prCurrFBufInfo->u4YAddr, prDecPrm->prCurrFBufInfo->u4CAddr,
        prDecPrm->prCurrFBufInfo->u4YLenStartAddr, prDecPrm->prCurrFBufInfo->u4CLenStartAddr,
        prDecPrm->prCurrFBufInfo->u4YLenOffset, prDecPrm->prCurrFBufInfo->u4CLenOffset);
    }
#endif
    if ((prDecPrm->prCurrFBufInfo->u4YAddr == (UINT32)NULL) ||
            (prDecPrm->prCurrFBufInfo->u4CAddr == (UINT32)NULL))
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "FbAddr NULL!!\n");
        //ASSERT(0);
        return;
    }

    if (fgIsRefPic(prDecPrm->u1NalRefIdc))
    {
        ASSERT(prDecPrm->u4MaxFBufNum <= 18);
        ASSERT((prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] > 0) &&
               (prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] <= prDecPrm->u4MaxFBufNum));
        if (prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] > 0)
        {
            i = prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] - 1;
            if (i < MAX_FRAME_NUM)
            {
                prDecPrm->prCurrFBufInfo->ucMvBufIdx =
                    prDecPrm->aucEmpMVBufIdxMap[u1ViewIdx][i];
                //LOG(2, "malloc mvbufIdx %d, maxNum %d\n", prDecPrm->prCurrFBufInfo->ucMvBufIdx, prDecPrm->ucEmpMVBufMaxNum);
            }
            FBM_GetFrameBufferAddrMv(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucMvBufIdx,
                                     &prDecPrm->prCurrFBufInfo->u4MVBufAddr);
        }
        else
        {
            prDecPrm->prCurrFBufInfo->u4MVBufAddr = (UINT32)NULL;
        }
        if (prDecPrm->prCurrFBufInfo->u4MVBufAddr == (UINT32)NULL)
        {
            prDecPrm->fgAllocFbFail = TRUE;
            LOG(2, "FbMvAddr NULL!!MVIdx %d\n", prDecPrm->prCurrFBufInfo->ucMvBufIdx);
            //ASSERT(0);
            return;
        }
        prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx]--;
    }
    else
    {
        prDecPrm->prCurrFBufInfo->ucMvBufIdx = 0xFF;
        //prDecPrm->prCurrFBufInfo->u4MVBufAddr = 0;
    }
    LOG(9, "//AllocFBuf, FbId=%d, Ref=%d, Y=0x%x, C=0x%x, MV=0x%x, W=%d, H=%d, arFBufId %d\n",
        prDecPrm->prCurrFBufInfo->ucFbId, fgIsRefPic(prDecPrm->u1NalRefIdc),
        prDecPrm->prCurrFBufInfo->u4YAddr, prDecPrm->prCurrFBufInfo->u4CAddr,
        prDecPrm->prCurrFBufInfo->u4MVBufAddr, prDecPrm->u4PicW, prDecPrm->u4PicH,
        prDecPrm->u1DecFBufIdx);

    HAL_GetTime(&rTime);
    //set display parameter, Progressive flag, TFF, RFF
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
    if(prFbmPicHdr == NULL)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "GetFbgPicHdr Fail!!\n");
        //ASSERT(0);
        return;
    }
    prFbmPicHdr->fgSeqHeader = prVdecEsInfo->fgSeqHdrH264;  //set to FBM for notification if new SPS or PPS receive before the frame
    prVdecEsInfo->fgSeqHdrH264 = FALSE;
    prFbmPicHdr->fgBivl3DLR = prDecPrm->rSEI.u1CurrentFrameIsFrame0Flag;
#ifdef CC_RESIZE_SMALL_IMAGE
    prFbmPicHdr->u4PicWidth = prVdecEsInfo->u2OrgHSize;
    prFbmPicHdr->u4PicWidthPitch = prVdecEsInfo->u2LineSize;
    prFbmPicHdr->u4PicHeight = prVdecEsInfo->u2OrgVSize;
#endif

    //picture type
    if (prDecPrm->prSliceHdr)
    {
        if (fgIsISlice(prDecPrm->prSliceHdr->u4SliceType))
        {
            prFbmPicHdr->ucPicCdTp = MPEG_PIC_TYPE_I;
        }
        else if (fgIsPSlice(prDecPrm->prSliceHdr->u4SliceType))
        {
            prFbmPicHdr->ucPicCdTp = MPEG_PIC_TYPE_P;
        }
        else if (fgIsBSlice(prDecPrm->prSliceHdr->u4SliceType))
        {
            prFbmPicHdr->ucPicCdTp = MPEG_PIC_TYPE_B;
        }
    }

    //DivX Plus TPT seemless resolution change
    if (prDecPrm->fgSeamlessDisp)
    {
        //default value
        prFbmPicHdr->fgSeamlessDisp = TRUE;
        prFbmPicHdr->u4PicWidth = prDecPrm->u4PicW;
        prFbmPicHdr->u4PicHeight = prDecPrm->u4PicH;
        prFbmPicHdr->u4PicWidthPitch = prVdecEsInfo->u2LineSize;

        //modified for MM
        if (prDecPrm->fgIsMMPlay)
        {
            if (prVdecEsInfo->rMMTPTSeqInfo.u2_width > 0)
            {
                prFbmPicHdr->u4PicWidth = prVdecEsInfo->rMMTPTSeqInfo.u2_width;
                prFbmPicHdr->u4PicHeight = prVdecEsInfo->rMMTPTSeqInfo.u2_height;
            }
            else if (prVdecEsInfo->rMMSeqInfo.u2_width > 0)
            {
                prFbmPicHdr->u4PicWidth = prVdecEsInfo->rMMSeqInfo.u2_width;
                prFbmPicHdr->u4PicHeight = prVdecEsInfo->rMMSeqInfo.u2_height;
            }
        }
    }
    else
    {
        prFbmPicHdr->fgSeamlessDisp = FALSE;
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
    if(prFbmSeqHdr == NULL)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "GetFbgSeqHdr Fail!!\n");
        //ASSERT(0);
        return;
    }
    //crop info
    prFbmPicHdr->fgCropping = prFbmSeqHdr->fgCropping;
    prFbmPicHdr->u4CropX = prFbmSeqHdr->u4CropX;
    prFbmPicHdr->u4CropY = prFbmSeqHdr->u4CropY;
    prFbmPicHdr->u4CropWidth = prFbmSeqHdr->u4CropWidth;
    prFbmPicHdr->u4CropHeight = prFbmSeqHdr->u4CropHeight;
    
    if (prDecPrm->prSPS->rVUI.fgPicStructPresentFlag &&
            (prDecPrm->rSEI.u1PicStruct != (UINT8)SEI_FRAME_UNKNOWN))
    {
        prFbmPicHdr->fgProgressiveFrm =
            ((prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME) ||
             (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_DOUBLING) ||
             (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_TRIPLING)) ? TRUE : FALSE;
    }
    else    // progressive/interlace display detect
    {
        if (prDecPrm->prSliceHdr && prDecPrm->prSliceHdr->fgFieldPicFlag)
        {
            prFbmPicHdr->fgProgressiveFrm = FALSE;
            prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_TOP_FIELD;
        }
        else
        {
            //prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
            if (prFbmSeqHdr != NULL)
            {
                if (prVdecEsInfo->fgMMPlayback)
                {
                    prFbmPicHdr->fgProgressiveFrm = prFbmSeqHdr->fgProgressiveSeq;
                    if (prFbmPicHdr->fgProgressiveFrm)
                    {
                        prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_FRAME;
                    }
                    else
                    {
                        prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_TOP_FIELD;
                    }
                }
                else
                {
                    if ((prFbmSeqHdr->ucFrmRatCod == (UCHAR)MPEG_FRAME_RATE_24) ||
                            (prFbmSeqHdr->ucFrmRatCod == (UCHAR)MPEG_FRAME_RATE_24_) ||
                            (prDecPrm->u4PicW == 1280))
                    {
                        prFbmPicHdr->fgProgressiveFrm = TRUE;
                        prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_FRAME;
                    }
                    else
                    {
                        prFbmPicHdr->fgProgressiveFrm = FALSE;
                        prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_TOP_BOTTOM;
                    }
                }
            }
            else
            {
                prFbmPicHdr->fgProgressiveFrm = FALSE;
                prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_TOP_BOTTOM;
            }
        }
    }

    //detect progressive/interlace runtime change
    prFbmPicHdr->fgProgressive2Interlace = FALSE;
    #ifdef CC_FORCE_ITIMING_FRM251080
    if((prFbmSeqHdr->ucFrmRatCod == MPEG_FRAME_RATE_25) && (prDecPrm->u4PicW > 1280) && (prDecPrm->u4PicH > 720))
    {
        prFbmSeqHdr->fgProgressiveSeq = FALSE;
        if(prFbmPicHdr->fgProgressiveFrm)
        {
            prFbmPicHdr->fgProgressive2Interlace = TRUE;
            prFbmPicHdr->fgProgressiveFrm = FALSE;
            prVdecEsInfo->fgIPSeamless = TRUE;
            LOG(1,"set fgProgressive2Interlace to true\n");
        }
    }
    #endif
    if(prDecPrm->prCurrFBufInfo &&
        (prFbmPicHdr->fgProgressiveFrm != prFbmSeqHdr->fgProgressiveSeq))
    {
    #if 1//def CC_ITIMING_ALWAYS
        if( //(TRUE==prVdecEsInfoKeep->fgFixFHDInterlaceDisplay) && 
            (prFbmSeqHdr->fgProgressiveSeq == FALSE) && 
            (prFbmPicHdr->fgProgressiveFrm) /*&&
            ((prDecPrm->u4PicW > 1280) || (prDecPrm->u4PicH > 720))*/)
        {
            //prFbmPicHdr->fgProgressive2Interlace = TRUE;
            //prFbmPicHdr->fgProgressiveFrm = FALSE;
            //prVdecEsInfo->fgIPSeamless = TRUE;
            //LOG(1,"h.264 enter ip seamless \n");
            prVdecEsInfo->fgIPSeamless = FALSE;
            prFbmSeqHdr->fgProgressiveSeq = prFbmPicHdr->fgProgressiveFrm;
            FBM_SetSeqChgFbId(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
            LOG(0,"h.264 notify if sequence header change , %d %d\n",prFbmSeqHdr->fgProgressiveSeq,prFbmPicHdr->fgProgressiveFrm);
        }
        else
        {
            prVdecEsInfo->fgIPSeamless = FALSE;
            prFbmSeqHdr->fgProgressiveSeq = prFbmPicHdr->fgProgressiveFrm;
            FBM_SetSeqChgFbId(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
            LOG(0,"h.264 notify sequence header change , %d %d\n",prFbmSeqHdr->fgProgressiveSeq,prFbmPicHdr->fgProgressiveFrm);
        }
    #else
        prFbmSeqHdr->fgProgressiveSeq = prFbmPicHdr->fgProgressiveFrm;
        FBM_SetSeqChgFbId(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
        FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
    #endif
    }    
    else
    {
        prVdecEsInfo->fgIPSeamless = FALSE;
    }
    
    prFbmPicHdr->fgRepFirstFld =
        ((prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_TOP_BOTTOM_TOP) ||
         (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_BOTTOM_TOP_BOTTOM) ||
         (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_DOUBLING) ||
         (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_TRIPLING)) ? TRUE : FALSE;

    prFbmPicHdr->fgTopFldFirst =
        ((prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_TOP_BOTTOM_TOP) ||
         (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_TOP_BOTTOM) ||
         (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_TRIPLING) ||
         ((prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_TOP_FIELD) && (prDecPrm->u1PicStruct & TOP_FIELD))) ? TRUE : FALSE;

    prFbmPicHdr->ucPicStruct = prDecPrm->u1PicStruct;

    prFbmPicHdr->u4PTS = prVdecEsInfoKeep->rPesInfo.u4PTS;
    prFbmPicHdr->u8PTS = prVdecEsInfoKeep->rPesInfo.u8PTS;
    if(prDecPrm->prCurrFBufInfo)
    {
        prDecPrm->prCurrFBufInfo->fgPTSValid = prVdecEsInfoKeep->rPesInfo.fgDtsValid;
    }
    if (prVdecEsInfoKeep->rPesInfo.u4PTS == 0)
    {
        LOG(7, "Rcv PTS=0, Valid %d\n", prVdecEsInfoKeep->rPesInfo.fgDtsValid);
    }

    if (prDecPrm->fgIsMMPlay)
    {
        x_memcpy(&prFbmPicHdr->rExtra, &prDecPrm->rExtra, sizeof(FBM_PIC_EXTRA_INFO_T));
        prFbmPicHdr->u8Offset = prDecPrm->u8Offset;
        prFbmPicHdr->u8OffsetDisp = prVdecEsInfo->u8OffsetLast;
        prFbmPicHdr->u8OffsetI = prDecPrm->u8OffsetI;
        prFbmPicHdr->u4TickNum = prDecPrm->u4TickNum;
        prFbmPicHdr->u2DecodingOrder = prDecPrm->u2DecodingOrder;
        prFbmPicHdr->u4Timestap = prDecPrm->u4Timestap;
    }
#endif
    prFbmPicHdr->fgUfoEn = prFbmSeqHdr->fgUfoEn;
    prDecPrm->pu1DecWorkBufY = (UINT8*)(prDecPrm->prCurrFBufInfo->u4YAddr);
    prDecPrm->pu1DecWorkBufC = (UINT8*)(prDecPrm->prCurrFBufInfo->u4CAddr);
    prDecPrm->pu1DecWorkBufMV = (UINT8*)(prDecPrm->prCurrFBufInfo->u4MVBufAddr);
}


// *********************************************************************
// Function    : void   vFillFrameNumGap(TMpvDecPrm *tMpvDecPrm)
// Description : add the frame num for the gap
// Parameter   : None
// Return      : None
// *********************************************************************
/*static void _vFillFrameNumGap(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 u4CurrFrameNum;
    UINT32 u4UnusedShortTermFrameNum;
    INT32 tmp1, tmp2;

    ASSERT(prDecPrm!=NULL);

    tmp1 = prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0];
    tmp2 = prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[1];

    prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0] = 0;
    prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[1] = 0;

    u4UnusedShortTermFrameNum = (prDecPrm->rLastInfo.u4LastFrameNum + 1) %
                                prDecPrm->prSPS->u4MaxFrameNum;
    u4CurrFrameNum = prDecPrm->prSliceHdr->u4FrameNum;

    if((prDecPrm->prSliceHdr->u4FrameNum != prDecPrm->rLastInfo.u4LastFrameNum)
    && (u4CurrFrameNum != u4UnusedShortTermFrameNum))
    {
        LOG(6, "!!!!  Fill frame num gap works  !!!!!\n");
        while (u4CurrFrameNum != u4UnusedShortTermFrameNum)
        {
            // Create a new frame pic
            _vAllocateFBuf(prDecPrm);
            if(prDecPrm->fgAllocFbFail)
            {
                return;
            }
            prDecPrm->prCurrFBufInfo->u1FBufStatus = FRAME;
            prDecPrm->prCurrFBufInfo->i4PicNum = (INT32)u4UnusedShortTermFrameNum;
            prDecPrm->prCurrFBufInfo->u4FrameNum = u4UnusedShortTermFrameNum;
            prDecPrm->prCurrFBufInfo->fgNonExisting = TRUE;

            prDecPrm->prSliceHdr->fgAdaptiveRefPicMarkingModeFlag = 0;

            if(prDecPrm->prSPS->u4PicOrderCntType == 0)
            {
                prDecPrm->prCurrFBufInfo->i4POC = prDecPrm->rLastInfo.i4LastPOC;
                prDecPrm->prCurrFBufInfo->i4TFldPOC = prDecPrm->rLastInfo.i4LastTFldPOC;
                prDecPrm->prCurrFBufInfo->i4BFldPOC = prDecPrm->rLastInfo.i4LastBFldPOC;
            }
            else
            {
                _VDEC_SetCurrPOC(prDecPrm);
            }
            // Check if out of the Ref Frames
            _VDEC_SlidingWindowProce(prDecPrm);
            prDecPrm->prCurrFBufInfo->u1FBufRefType = SREF_PIC;
            prDecPrm->prCurrFBufInfo->u1TFldRefType = SREF_PIC;
            prDecPrm->prCurrFBufInfo->u1BFldRefType = SREF_PIC;
            prDecPrm->rLastInfo.u4LastFrameNum = u4UnusedShortTermFrameNum;
            u4UnusedShortTermFrameNum = (u4UnusedShortTermFrameNum + 1) % prDecPrm->prSPS->u4MaxFrameNum;
        }
    }

    prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0] = tmp1;
    prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[1] = tmp2;
    prDecPrm->prSliceHdr->u4FrameNum = u4CurrFrameNum;
}*/


// *********************************************************************
// Function    : void   vPrepareRefPiclist(TMpvDecPrm *tMpvDecPrm)
// Description : check pic type to send P_0, B_0, B_1
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vPrepareFBufInfo(VDEC_AVC_DecPrm *prDecPrm)
{
#ifdef FBM_DPB_CTRL
    //UCHAR ucFbgType;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;
    UINT32 u4fps = 0;//, u4FbWidth, u4FbHeight;
#endif
    UINT32 u4CurrFrameNum = 0, u4UnusedShortTermFrameNum, u4DAR = 0, u4SAR = 0, u4EstimateWidth = 0, u4EstimateHeigh = 0;
    BOOL fgAfdChg = FALSE;
    BOOL fgCroppingChg = FALSE;
    BOOL fgRefChg = FALSE;
    VDEC_INFO_T* prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UINT8 u1ViewIdx = 0;
    UCHAR ucFrmRatCodDecoder = 0xFF;
    UCHAR ucFrmRatCodContainer = 0xFF;
    UINT32 u4CurDAR, u4KeptDAR, u4Tolerance;
    #ifdef CC_MAPLE_CUST_DRV
    ENUM_CODEC_INFO_T eCodecType;
    #endif
    #ifdef CC_SUPPORT_VDEC_PREPARSE
    BOOL fgIsCheckUFO = FALSE;
    VDEC_VLDM_INPUT_T tInCond;
    #endif
    
    if (prDecPrm == NULL)
    {
        LOG(2, "prDecPrm Null!!\n");
        return;
    }
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);

    if (prVdecEsInfo == NULL)
    {
        LOG(2, "prVdecEsInfo Null!!\n");
        return;
    }
    if (prVdecEsInfoKeep == NULL)
    {
        LOG(2, "prVdecEsInfoKeep Null!!\n");
        return;
    }
    
    
    prDecPrm->u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prDecPrm->u4PicH = ((2 - prDecPrm->prSPS->fgFrameMbsOnlyFlag) * (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
    prDecPrm->u4RealPicH = (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) << 4;  // original real size
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    prDecPrm->fgIsResChg = FALSE;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "PrepareBuf ViewIdx Err!\n");
        return;
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
    if (prFbmSeqHdr == NULL)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "GetFbgSeqHdr Fail!!\n");
        return;
    }
    
#ifdef ENABLE_MULTIMEDIA
    if ((prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV) && (!prDecPrm->bPreAllocFBG))
    {
        if (prDecPrm->fgIsNewMode)
        {
            prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
        }
        else
        {
            prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
        }
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
        if(prFbmSeqHdr == NULL)
        {
            LOG(2, "GetFbgSeqHdr Fail 2!!\n");
            return;
        }
        prFbmSeqHdr->u2OrgWidth = (UINT16)(prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset);
        prFbmSeqHdr->u2OrgHeight = (UINT16)(prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
                                    prDecPrm->prSPS->u4FrameCropBottomOffset : (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
        if((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) ||
            (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH))
        {
            LOG(2,"Line(%d) Send FBM_FLAG_SEQ_CHG org[%d,%d] ==> cur[%d,%d]\n", __LINE__,
                prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW,
                prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH,
                prDecPrm->u4PicW,prDecPrm->u4PicH);
    #ifdef CC_MAPLE_CUST_DRV
            prFbmSeqHdr->u4resolutionChg++;
    #endif
            //FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
        }
        if (prVdecEsInfo->fgIsTPT && (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV))
        {
            if (prDecPrm->fgIsNewMode)
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
            }
            else
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
            }
            prFbmSeqHdr->u2OrgHSize = prDecPrm->u4PicW;
            prFbmSeqHdr->u2OrgVSize = prDecPrm->u4PicH;
            prFbmSeqHdr->u2HSize = (UINT16)prDecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prDecPrm->u4PicH;
            LOG(2,"OrgWidth/OrgHeight1 [%d,%d] Picth(%d)\n",prDecPrm->u4PicW, prDecPrm->u4PicH, prVdecEsInfo->u2LineSize);
        }
    }
    else if (((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) && (!prDecPrm->bPreAllocFBG) && (!prDecPrm->fgReCreateFBG)) ||
        ((prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS) && (!prDecPrm->bPreAllocFBG)))
    {
        prDecPrm->fgSeamlessDisp = TRUE;        
        if (prDecPrm->fgIsNewMode)
        {
            prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
        }
        else
        {
            prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
        }
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
        if(prFbmSeqHdr == NULL)
        {
            LOG(2, "GetFbgSeqHdr Fail 2!!\n");
            return;
        }
        prFbmSeqHdr->u2OrgWidth = (UINT16)(prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset);
        prFbmSeqHdr->u2OrgHeight = (UINT16)(prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
                                    prDecPrm->prSPS->u4FrameCropBottomOffset : (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
        if((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) ||
            (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH))
        {       
            LOG(2,"Send FBM_FLAG_SEQ_CHG org[%d,%d] ==> cur[%d,%d]\n",prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW,prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH,
                                                                    prDecPrm->u4PicW,prDecPrm->u4PicH);
            #ifdef CC_MAPLE_CUST_DRV
            prFbmSeqHdr->u4resolutionChg++;
            #endif
            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
        }
    }
    else
#endif
    if (((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) ||
        (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH)) &&
        prVdecEsInfo->fgMMPlayback && prVdecEsInfo->fgIsTPT &&
        (prDecPrm->aucFbgId[u1ViewIdx] != FBM_FBG_ID_UNKNOWN))
    {
        if ((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW > prDecPrm->u4PicW) ||
                (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH > prDecPrm->u4PicH))
        {
            prDecPrm->fgSeamlessDisp = TRUE;
        }
        else
        {
            prDecPrm->fgSeamlessDisp = FALSE;
        }

        if((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
            ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) &&
            (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
        {
            prFbmSeqHdr->u4ResizeWidth  = prVdecEsInfo->rMMSeqInfo.u2_width;
            prFbmSeqHdr->u4ResizeHeight = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            prFbmSeqHdr->u4ResizeWidth = (UINT16)prDecPrm->u4PicW;
            prFbmSeqHdr->u4ResizeHeight = (UINT16)prDecPrm->u4PicH;
        }
        LOG(1,"@@@@W(%d)H(%d)\n",prFbmSeqHdr->u4ResizeWidth,prFbmSeqHdr->u4ResizeHeight);
    }
    else if (prDecPrm->bPreAllocFBG)
    {
        LOG(1, "!!!!!!!!!!!!!!create fbg!!!!!!!!!!!!!!!!\n");
        fgAfdChg = TRUE;
        #ifdef CC_SUPPORT_VDEC_PREPARSE
        fgIsCheckUFO = TRUE;
        #endif
        prDecPrm->bPreAllocFBG = FALSE;
#ifdef FBM_DPB_CTRL
        prDecPrm->rLastInfo[u1ViewIdx].u4LastSPos = 0;
        prDecPrm->i4LastPOC[u1ViewIdx] = -99999;

        if (prVdecEsInfo->fgMMPlayback)
        {
            FBM_SetPlayMode(prDecPrm->aucFbgId[u1ViewIdx], FBM_FBG_MM_MODE);
        }
        if(prVdecEsInfo->fgPlaybackNotDisplay)
        {
            FBM_SetPlayMode(prDecPrm->aucFbgId[u1ViewIdx],FBM_FBG_NOTDISPLAY_MODE);
        }
        if (prVdecEsInfoKeep->fgVPush)
        {
            if (prVdecEsInfoKeep->fgVPushDecodeOnly)
            {
                FBM_SetPlayMode(prVdecEsInfo->ucFbgId, FBM_FBG_NOTDISPLAY_MODE);
            }
            if((prVdecEsInfo->ucSubFbgId !=0xFF)&&prDecPrm->fgIsMVC)
            {
                FBM_RegFbgCbFunc(prVdecEsInfo->ucSubFbgId,
                             FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, prDecPrm->ucEsId);
                LOG(0,"register callbackFBM_CB_FUNC_FB_READY_EX_IND mvc %d,%d \n",prVdecEsInfo->ucSubFbgId);
            }
            else
            {
                FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                             FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, prDecPrm->ucEsId);
                if(prVdecEsInfoKeep->fgVPushFBGFromInst)
                {
                    FBM_RegFbgCbFunc(prVdecEsInfo->ucFbgId,
                             FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, prDecPrm->ucEsId);
                }
                LOG(0,"register callbackFBM_CB_FUNC_FB_READY_EX_IND %d,%d \n",prVdecEsInfo->ucFbgId);
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

        if(VDEC_3D_BIVL == prVdecEsInfo->e3DType)
        {
            prFbmSeqHdr->u1B2R3DType = VDEC_3D_BIVL;
        }
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
        if (prDecPrm->fgIsMVC)
        {
            prVdecEsInfo->e3DType = VDEC_3D_MVC;
        }
        //for the MVC format which mw recognized, and no prefix nalu case.
#ifndef CC_SW_MVC_ENABLE
        if (prVdecEsInfo->e3DType == VDEC_3D_MVC)
        {
            prDecPrm->fgIsMVC = TRUE;
        }
#endif
#endif
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
            FBM_FbgChgNotify(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucEsId);  //set 3D type before mode change
        }
        if (FBM_ChkFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_RESET))
        {
            FBM_ResetGroup(prDecPrm->aucFbgId[u1ViewIdx]);
            FBM_ClrFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_RESET);
            FBM_ClrFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_NOT_READY);
            //FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_CLEAR_FRC_PRM);
        }
        //set afd default value
        if(prDecPrm->rSEI.u1AFD != SEI_AFD_UNKONWN)
        {
            prFbmSeqHdr->ucActFmt = prDecPrm->rSEI.u1AFD;
        }
        else
        {
            prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
        }
        
        //RR mode setting
        if (prDecPrm->fgRRModeYEn || prDecPrm->fgRRModeCEn)
        {
            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_REDUCE_REF);
        }
        else if (FBM_ChkFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_REDUCE_REF))
        {
            prDecPrm->fgRRModeYEn = TRUE;
            prDecPrm->fgRRModeCEn = TRUE;
            prDecPrm->u1RRModeY = 0;
            prDecPrm->u1RRModeC = 0;
        }
        else
        {
            FBM_ClrFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_REDUCE_REF);
        }

        if (prDecPrm->fgRRModeYEn)
        {
            prFbmSeqHdr->fgRRYEn = TRUE;
            if (prDecPrm->u1RRModeY > 0)
            {
                prFbmSeqHdr->fgYRatio34 = TRUE;
            }
            else
            {
                prFbmSeqHdr->fgYRatio34 = FALSE;
            }
        }
        else
        {
            prFbmSeqHdr->fgRRYEn = FALSE;
        }
        if (prDecPrm->fgRRModeCEn)
        {
            prFbmSeqHdr->fgRRCEn = TRUE;
            if (prDecPrm->u1RRModeC > 0)
            {
                prFbmSeqHdr->fgCRatio34 = TRUE;
            }
            else
            {
                prFbmSeqHdr->fgCRatio34 = FALSE;
            }
        }
        else
        {
            prFbmSeqHdr->fgRRCEn = FALSE;
        }
        //FBM_GetFrameBufferSize(prDecPrm->aucFbgId[u1ViewIdx], &u4FbWidth, &u4FbHeight);

        //Display parameter setting
        //prFbmSeqHdr->u2LineSize = (UINT16)u4FbWidth;//prDecPrm->u4PicW;
        if (prDecPrm->fgIsNewMode)
        {
            prFbmSeqHdr->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);//MAX(_MAKE_64X(prDecPrm->u4PicW), u4FbWidth);
        }
        else
        {
            prFbmSeqHdr->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);//MAX(_MAKE_64X(prDecPrm->u4PicW), u4FbWidth);
        }
        //UNUSED(u4FbHeight);

        if ((prVdecEsInfo->fgMMSeqInfoValid && prVdecEsInfo->fgMMPlayback) &&
                ((prVdecEsInfo->rMMSeqInfo.u2_width > 0) &&
                 (prVdecEsInfo->rMMSeqInfo.u2_height > 0)))
        {
            prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
            prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
        }
        else
        {
            prFbmSeqHdr->u2HSize = (UINT16)prDecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prDecPrm->u4PicH;
        }
#if defined(CC_USE_DDI)
// turnkey: 2D model: handled by sysbuild APP according to crop info, 3D model: display 3D
        if (((prVdecEsInfo->e3DType == VDEC_3D_SBS_LF) || (prVdecEsInfo->e3DType == VDEC_3D_SBS_RF) ||
                  (prVdecEsInfo->e3DType == VDEC_3D_TB_LF) || (prVdecEsInfo->e3DType == VDEC_3D_TB_RF)) &&
            ((prDecPrm->prSPS->u4FrameCropLeftOffset > 0) || (prDecPrm->prSPS->u4FrameCropRightOffset > (prDecPrm->u4PicW >> 3)) ||
             (prDecPrm->prSPS->u4FrameCropTopOffset > 0) || (prDecPrm->prSPS->u4FrameCropBottomOffset > (prDecPrm->u4PicH >> 3))))     // 1088 to 1080 should be allowed in 3D mode
        {
            if (((prVdecEsInfo->e3DType == VDEC_3D_SBS_RF) && (prDecPrm->prSPS->u4FrameCropRightOffset > (prDecPrm->u4PicW >> 3))) ||
                    ((prVdecEsInfo->e3DType == VDEC_3D_SBS_LF) && (prDecPrm->prSPS->u4FrameCropLeftOffset > 0)))
            {
                prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_R_VIEW;
            }
            else if (((prVdecEsInfo->e3DType == VDEC_3D_TB_RF) && (prDecPrm->prSPS->u4FrameCropBottomOffset > (prDecPrm->u4PicH >> 3))) ||
                     ((prVdecEsInfo->e3DType == VDEC_3D_TB_LF) && (prDecPrm->prSPS->u4FrameCropTopOffset > 0)))
            {
                prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_R_VIEW;
            }
            else
            {
                prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_L_VIEW;
            }
            LOG(3, "Force2D Type %d\n", prVdecEsInfo->e3DForce2DType);
            prFbmSeqHdr->fgCropping = FALSE;
        }
        else
#endif
        //check 3D model by sysbuild, always pass the crop info but APP will not use in 3D model
        {                
            prFbmSeqHdr->fgCropping = prDecPrm->prSPS->fgFrameCroppingFlag;
            prFbmSeqHdr->u4CropX = prDecPrm->prSPS->u4FrameCropLeftOffset;
            prFbmSeqHdr->u4CropY = prDecPrm->prSPS->u4FrameCropTopOffset;
            prFbmSeqHdr->u4CropWidth = (prDecPrm->u4PicW - prFbmSeqHdr->u4CropX) -
                                                        prDecPrm->prSPS->u4FrameCropRightOffset;
            prFbmSeqHdr->u4CropHeight = (prDecPrm->u4PicH - prFbmSeqHdr->u4CropY -
                         ((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
                          prDecPrm->prSPS->u4FrameCropBottomOffset :
                          (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));

        }
#ifdef ENABLE_MULTIMEDIA
        if (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV)
        {
            prFbmSeqHdr->u2OrgWidth  = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgHeight = prFbmSeqHdr->u2VSize;
            if (prDecPrm->fgIsNewMode)
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
            }
            else
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
            }
            prFbmSeqHdr->u2OrgHSize = prDecPrm->u4PicW;
            prFbmSeqHdr->u2OrgVSize = prDecPrm->u4PicH;
            prFbmSeqHdr->u2HSize = (UINT16)prDecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prDecPrm->u4PicH;
            LOG(2,"OrgWidth/OrgHeight1 [%d,%d] Picth(%d)\n",prDecPrm->u4PicW, prDecPrm->u4PicH, prVdecEsInfo->u2LineSize);
        }
        else if ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) && (!prDecPrm->fgReCreateFBG))
        {
            prFbmSeqHdr->u2OrgWidth  = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgHeight = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2HSize = (UINT16)prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u2VSize = (UINT16)prVdecEsInfo->u4SeamlessHeight;
            prFbmSeqHdr->u2OrgHSize = (UINT16)prVdecEsInfo->u4SeamlessWidth;
            prFbmSeqHdr->u2OrgVSize = (UINT16)prVdecEsInfo->u4SeamlessHeight;
            prFbmSeqHdr->u2LineSize = prVdecEsInfo->u4SeamlessWidth;
            //move down with DAR calculation
            //prFbmSeqHdr->u4ResizeWidth = prVdecEsInfo->u4SeamlessWidth;
            //prFbmSeqHdr->u4ResizeHeight = prVdecEsInfo->u4SeamlessHeight;
            prDecPrm->fgSeamlessDisp = TRUE;
            if (prDecPrm->fgIsNewMode)
            {
                prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
            }
            else
            {
                prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
            }
            LOG(2,"OrgWidth/OrgHeight1 [%d,%d] Picth(%d)\n",prFbmSeqHdr->u2OrgWidth, prFbmSeqHdr->u2OrgHeight, prVdecEsInfo->u2LineSize);
        }
        else if (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS)
        {
            prFbmSeqHdr->u2OrgWidth  = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgHeight = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
            prFbmSeqHdr->u2LineSize = prVdecEsInfo->u4SeamlessWidth;
            if (prDecPrm->fgIsNewMode)
            {
                prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
            }
            else
            {
                prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
            }
            prDecPrm->fgSeamlessDisp = TRUE;
            LOG(2,"OrgWidth/OrgHeight1 [%d,%d] Picth(%d).\n", prFbmSeqHdr->u2OrgWidth,prFbmSeqHdr->u2OrgHeight, prVdecEsInfo->u2LineSize);
        }
        else
#endif
        {
            prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
            prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
            prVdecEsInfo->u2LineSize = prFbmSeqHdr->u2LineSize;
            //kr3d will assign resize target W/H in FBM
            //prFbmSeqHdr->u4ResizeWidth = prFbmSeqHdr->u2OrgHSize;
            //prFbmSeqHdr->u4ResizeHeight = prFbmSeqHdr->u2OrgVSize;
        }
        
        _VDEC_ResizeSmallImage(prDecPrm->ucEsId, prFbmSeqHdr);

        prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth = 0;
        prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight = 0;
        prFbmSeqHdr->u4AspectRatioIdc = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
        if (prDecPrm->prSPS->rVUI.u4AspectRatioIdc != 0xFF)  //extended_SAR
        {
            prFbmSeqHdr->ucAspRatInf = (UCHAR)prDecPrm->prSPS->rVUI.u4AspectRatioIdc;    // 16:9, 4:3
        }

        if (prDecPrm->prSPS->fgVuiParametersPresentFlag)
        {
            //OverScan
            if (prDecPrm->prSPS->rVUI.fgOverscanInfoPresentFlag &&
                    (!prDecPrm->prSPS->rVUI.fgOverscanAppropriateFlag))
            {
                prFbmSeqHdr->fgNoOverScan = TRUE;
            }
            else
            {
                prFbmSeqHdr->fgNoOverScan = FALSE;
            }
            //Color Description
            if (prDecPrm->prSPS->rVUI.fgVideoSignalTypePresentFlag &&
                    prDecPrm->prSPS->rVUI.fgColourDescriptionPresentFlag)
            {
                prFbmSeqHdr->fgColorDescrip = TRUE;
                prFbmSeqHdr->ucColourPrimaries = (UCHAR)prDecPrm->prSPS->rVUI.u4ColourPrimaries;
                prFbmSeqHdr->ucTransCharacter = (UCHAR)prDecPrm->prSPS->rVUI.u4TransferCharacteristics;
                prFbmSeqHdr->ucMatrixCoeff = (UCHAR)prDecPrm->prSPS->rVUI.u4MatrixCoefficients;
            }
            else
            {
                prFbmSeqHdr->fgColorDescrip = FALSE;
            }
            //Frame Crop
            if (prDecPrm->prSPS->fgFrameCroppingFlag)
            {
                if (prDecPrm->prSPS->fgFrameMbsOnlyFlag)
                {
                    prFbmSeqHdr->u4FrmCropTop = prDecPrm->prSPS->u4FrameCropTopOffset;
                    prFbmSeqHdr->u4FrmCropBtm = prDecPrm->prSPS->u4FrameCropBottomOffset;
                }
                else
                {
                    prFbmSeqHdr->u4FrmCropTop = prDecPrm->prSPS->u4FrameCropTopOffset << 1;
                    prFbmSeqHdr->u4FrmCropBtm = prDecPrm->prSPS->u4FrameCropBottomOffset << 1;
                }
            }
            else
            {
                prFbmSeqHdr->u4FrmCropTop = 0;
                prFbmSeqHdr->u4FrmCropBtm = 0;
            }
        }
        else
        {
            prFbmSeqHdr->fgNoOverScan = FALSE;
            prFbmSeqHdr->fgColorDescrip = FALSE;
            prFbmSeqHdr->u4FrmCropTop = 0;
            prFbmSeqHdr->u4FrmCropBtm = 0;
        }
        prFbmSeqHdr->ucFrmRatCod = _VDEC_GetFRFromSPS(prDecPrm);
        ucFrmRatCodDecoder = prFbmSeqHdr->ucFrmRatCod;
        prFbmSeqHdr->ucFrmRatCod = _VDEC_GetFRFromContainer(prDecPrm);                    
        ucFrmRatCodContainer = prFbmSeqHdr->ucFrmRatCod;
        
        if((ucFrmRatCodContainer != ucFrmRatCodDecoder))
        {
            if(ucFrmRatCodContainer == MPEG_FRAME_RATE_UNKNOWN)
            {
                prFbmSeqHdr->ucFrmRatCod = ucFrmRatCodDecoder;
            }
            else if(ucFrmRatCodDecoder == MPEG_FRAME_RATE_UNKNOWN)
            {
                prFbmSeqHdr->ucFrmRatCod = ucFrmRatCodContainer;
            }
            else
            {
                prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
            }
        }
        if (prDecPrm->prSPS->rVUI.fgPicStructPresentFlag &&
                (prDecPrm->rSEI.u1PicStruct != (UINT8)SEI_FRAME_UNKNOWN))
        {
            prFbmSeqHdr->fgProgressiveSeq =
                ((prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME) ||
                 (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_DOUBLING) ||
                 (prDecPrm->rSEI.u1PicStruct == (UINT8)SEI_FRAME_TRIPLING)) ? TRUE : FALSE;
        }
        else  // progressive/interlace display detect
        {
            if (prDecPrm->prSliceHdr->fgFieldPicFlag)
            {
                prFbmSeqHdr->fgProgressiveSeq = FALSE;
            }
            else
            {
                if (prVdecEsInfo->fgMMPlayback)
                {
                    prFbmSeqHdr->fgProgressiveSeq = TRUE;
                }
                else
                {
                    prFbmSeqHdr->fgProgressiveSeq = TRUE;
                }
            }
        }

#endif
        if ((u1ViewIdx == 0) || (prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM))
        {
            _vPartitionDPB(prDecPrm);
            prDecPrm->u4MaxFBufNum = prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
        }
        if (prDecPrm->fgNotSupport) // memory not enough
        {
            return;
        }

        // set default pts scale/rate if container didn't assign via _VDEC_H264StartPTS
        if(u4fps == 10)
        {
            prDecPrm->u8PtsScale = au8Scale[9]; //frame rate 10 definition
            prDecPrm->u4Rate = au4Rate[9];
            LOG(1, "Detect fps 10\n");
        }
        else if ((prFbmSeqHdr->ucFrmRatCod != MPEG_FRAME_RATE_UNKNOWN) &&
                ((UINT32)prFbmSeqHdr->ucFrmRatCod < FRAME_RATE_MAX))
        {
            prDecPrm->u8PtsScale = au8Scale[prFbmSeqHdr->ucFrmRatCod];
            prDecPrm->u4Rate = au4Rate[prFbmSeqHdr->ucFrmRatCod];
        }
        else if(prDecPrm->u4Rate == 0)
        {
            prDecPrm->u8PtsScale = au8Scale[MPEG_FRAME_RATE_30];
            prDecPrm->u4Rate = au4Rate[MPEG_FRAME_RATE_30];
        }
        if(u4fps == 10)
        {
            prDecPrm->u8PtsScale64 = au8Scale64[9]; //frame rate 10 definition
            prDecPrm->u4Rate64 = au4Rate64[9];
            LOG(1, "Detect fps 10\n");
        }
        else if ((prFbmSeqHdr->ucFrmRatCod != MPEG_FRAME_RATE_UNKNOWN) &&
                ((UINT32)prFbmSeqHdr->ucFrmRatCod < FRAME_RATE_MAX))
        {
            prDecPrm->u8PtsScale64 = au8Scale64[prFbmSeqHdr->ucFrmRatCod];
            prDecPrm->u4Rate64 = au4Rate64[prFbmSeqHdr->ucFrmRatCod];
        }
        else if (prDecPrm->u4Rate64 == 0)
        {
            prDecPrm->u8PtsScale64 = au8Scale64[MPEG_FRAME_RATE_30];
            prDecPrm->u4Rate64 = au4Rate64[MPEG_FRAME_RATE_30];
        }
        #if defined(CC_USE_DDI)
        if(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE|| 
              prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HW_DIRECT_PLAY)
        {
            prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
        }
        #endif
        #ifdef CC_MAPLE_CUST_DRV
        eCodecType = E_VDEC_CODEC_AVC;
        FBM_SetCodecInfo(prDecPrm->aucFbgId[u1ViewIdx],eCodecType);
        #endif
    }
  
    #if BDP_LOG_STYLE
    VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_SEQINFO, "[H264 VDEC%d]: fps(%d), framerate(%d), MW framerate(%d)\n",
                       prDecPrm->ucEsId,
                       u4fps,
                       prFbmSeqHdr->ucFrmRatCod,
                    prVdecEsInfo->rMMSeqInfo.e_frame_rate);
    #endif

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if (prDecPrm->fgIsMVC)
    {
        prVdecEsInfo->e3DType = VDEC_3D_MVC;
    }
#endif

    // I frame case
    if (prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
    {
        if (prVdecEsInfo && (!prVdecEsInfo->fgIFrameDec))   // header parsing
        {
            if (prVdecEsInfoKeep && (prVdecEsInfoKeep->pfDecNfyFct))
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_I_FRAME_SET_BUFF_DONE, 0, 0);
            }
            prDecPrm->fgAllocFbFail = TRUE; //ignore decoding stage
            return;
        }
        else    //decoding
        {
            prDecPrm->prSPS->rVUI.fgPicStructPresentFlag = TRUE;
            prDecPrm->rSEI.u1PicStruct = (UINT8)SEI_FRAME;
            if (prFbmSeqHdr)
            {
                prFbmSeqHdr->fgProgressiveSeq = TRUE;
            }
        }
    }
    #ifdef CC_FORCE_ITIMING_FRM251080
    if(prFbmSeqHdr &&(prFbmSeqHdr->ucFrmRatCod == MPEG_FRAME_RATE_25) && (prDecPrm->u4PicW > 1280) && (prDecPrm->u4PicH > 720))
    {
        prFbmSeqHdr->fgProgressiveSeq = FALSE;
    }
    #endif
    if (prFbmSeqHdr == NULL)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        LOG(2, "GetFbgSeqHdr Fail!!\n");
        //ASSERT(0);
        return;
    }

    //aspect ratio change notify
#ifdef FBM_DPB_CTRL
    if (prFbmSeqHdr != NULL)
    {
        if (prDecPrm->prSPS->rVUI.u4AspectRatioIdc == 0xFF)  //extended_SAR
        {
            if ((prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth != prDecPrm->prSPS->rVUI.u4SarWidth) ||
                    (prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight != prDecPrm->prSPS->rVUI.u4SarHeight) ||
                    fgAfdChg)   //resolution change
            {
                u4DAR = ((prDecPrm->u4PicW * prDecPrm->prSPS->rVUI.u4SarWidth) * 100) /
                        (prDecPrm->u4PicH * prDecPrm->prSPS->rVUI.u4SarHeight);
                prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth = prDecPrm->prSPS->rVUI.u4SarWidth;
                prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight = prDecPrm->prSPS->rVUI.u4SarHeight;
                prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf = 0xFF;
                prDecPrm->rLastInfo[u1ViewIdx].u4DAR = u4DAR;
                fgAfdChg = TRUE;
                LOG(4,"Aspectratio change %d @ Line %d\n",u4DAR,__LINE__);
                prFbmSeqHdr->u4AspectRatioWidth = prDecPrm->prSPS->rVUI.u4SarWidth;
                prFbmSeqHdr->u4AspectRatioHeight = prDecPrm->prSPS->rVUI.u4SarHeight;
            }
        }
        else if ((prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf !=
                  prDecPrm->prSPS->rVUI.u4AspectRatioIdc) || fgAfdChg)
            //aspRatio change or resolution chage
        {
            switch (prDecPrm->prSPS->rVUI.u4AspectRatioIdc)
            {
                case 1:
                    u4SAR = 100;
                    prFbmSeqHdr->u4AspectRatioWidth = 1;
                    prFbmSeqHdr->u4AspectRatioHeight = 1;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
                    break;
                case 2:
                    u4SAR = 1200 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 12;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_12_11;
                    break;
                case 3:
                    u4SAR = 1000 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 10;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_10_11;
                    break;
                case 4:
                    u4SAR = 1600 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 16;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_16_11;
                    break;
                case 5:
                    u4SAR = 4000 / 33;
                    prFbmSeqHdr->u4AspectRatioWidth = 40;
                    prFbmSeqHdr->u4AspectRatioHeight = 33;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_40_33;
                    break;
                case 6:
                    u4SAR = 2400 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 24;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_24_11;
                    break;
                case 7:
                    u4SAR = 2000 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 20;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_20_11;
                    break;
                case 8:
                    u4SAR = 3200 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 32;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_32_11;
                    break;
                case 9:
                    u4SAR = 8000 / 33;
                    prFbmSeqHdr->u4AspectRatioWidth = 80;
                    prFbmSeqHdr->u4AspectRatioHeight = 33;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_80_33;
                    break;
                case 10:
                    u4SAR = 1800 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 18;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_18_11;
                    break;
                case 11:
                    u4SAR = 1500 / 11;
                    prFbmSeqHdr->u4AspectRatioWidth = 15;
                    prFbmSeqHdr->u4AspectRatioHeight = 11;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_15_11;
                    break;
                case 12:
                    u4SAR = 6400 / 33;
                    prFbmSeqHdr->u4AspectRatioWidth = 64;
                    prFbmSeqHdr->u4AspectRatioHeight = 33;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_64_33;
                    break;
                case 13:
                    u4SAR = 16000 / 99;
                    prFbmSeqHdr->u4AspectRatioWidth = 160;
                    prFbmSeqHdr->u4AspectRatioHeight = 99;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_160_99;
                    break;
                case 14:
                    u4SAR = 400 / 3;
                    prFbmSeqHdr->u4AspectRatioWidth = 4;
                    prFbmSeqHdr->u4AspectRatioHeight = 3;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_4_3;
                    break;
                case 15:
                    u4SAR = 300 / 2;
                    prFbmSeqHdr->u4AspectRatioWidth = 3;
                    prFbmSeqHdr->u4AspectRatioHeight = 2;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_3_2;
                    break;
                case 16:
                    u4SAR = 200;
                    prFbmSeqHdr->u4AspectRatioWidth = 2;
                    prFbmSeqHdr->u4AspectRatioHeight = 1;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_2_1;
                    break;
                default:
                    u4SAR = 100;
                    prFbmSeqHdr->u4AspectRatioWidth = 1;
                    prFbmSeqHdr->u4AspectRatioHeight = 1;
                    prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
            }
            u4DAR = (prDecPrm->u4PicW * u4SAR) / prDecPrm->u4PicH;
            if (prVdecEsInfo 
#ifdef ENABLE_MULTIMEDIA
                && ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) || (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS))
                && (!prDecPrm->fgReCreateFBG)
#endif
                && ((prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ? 
                                            prDecPrm->prSPS->u4FrameCropBottomOffset : 
                                            (prDecPrm->prSPS->u4FrameCropBottomOffset << 1))) != 0)
                )
            {
                u4DAR = (prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset) * u4SAR /
                        (prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ? 
                                            prDecPrm->prSPS->u4FrameCropBottomOffset : 
                                            (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
            }
            fgAfdChg = TRUE;            
            LOG(2, "Afd change Last(%d) Cur(%d) SAR(%d) DAR(%d).\n", 
                prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf,
                prDecPrm->prSPS->rVUI.u4AspectRatioIdc,
                u4SAR, u4DAR);
            prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth = 0;
            prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight = 0;
            prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
            prDecPrm->rLastInfo[u1ViewIdx].u4DAR = u4DAR;
        }
        else if(IS_AVC_RAP(prDecPrm->u4PicNalType) && (prDecPrm->rSEI.u1AFD == SEI_AFD_UNKONWN) && (prFbmSeqHdr->ucActFmt != ACT_FMT_DEFAULT))
        {
            prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
            fgAfdChg = TRUE;
            u4DAR = prDecPrm->rLastInfo[u1ViewIdx].u4DAR;
            LOG(4,"u4AFD change %d u4DAR %d @ Line %d\n",prFbmSeqHdr->ucActFmt,u4DAR,__LINE__);
        }

        //prFbmSeqHdr->ucOrgAsp = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
        prFbmSeqHdr->u4OrgAspWidth = prFbmSeqHdr->u4AspectRatioWidth;
        prFbmSeqHdr->u4OrgAspHeight = prFbmSeqHdr->u4AspectRatioHeight;
        if (fgAfdChg)
        {
            prFbmSeqHdr->u4AspectRatioIdc = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
        
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
            if (prDecPrm->fgIsMMPlay)
            {
                prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                #ifdef DIVX_PLUS_CER  // for divx certification. as the certification requirements says, the container's aspect ratio info has a higer priority than vdec.  xiaolei.li
                if(prVdecEsInfo && (prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w > 0) && (prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h > 0))
                {
                    if(prVdecEsInfo->rMMSeqInfo.e_asp_usr_type == VDEC_ASP_USR_TYPE_DISP)
                    {
                        prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                    }
                    prFbmSeqHdr->u4AspectRatioWidth = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w;
                    prFbmSeqHdr->u4AspectRatioHeight = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h;
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                    LOG(6,"[H264]: USE MW Aspect Ratio Info \n");
                }
                else
                {
                    if((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                        ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
                    {
                        prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                        prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
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
                #else
                if ((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                        ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
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
                        prFbmSeqHdr->fgPixelAspectRatio = TRUE;
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
                #endif
            }
            else
            {
                #if defined(CC_USE_DDI)
                if((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                    ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
                {
                    prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;
                }
                else
                {
                    prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                }
                #else
                prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                #endif
            }
            if (prVdecEsInfo 
                #ifdef ENABLE_MULTIMEDIA
                && ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) || (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS))
                && (!prDecPrm->fgReCreateFBG)
                #endif
                )
            {
                u4EstimateWidth = (u4DAR * prVdecEsInfo->u4SeamlessHeight) / 100;
                u4EstimateHeigh = (prVdecEsInfo->u4SeamlessWidth * 100) / u4DAR;

                u4EstimateWidth = VDEC_ALIGN_MASK(u4EstimateWidth, 15);
                u4EstimateHeigh = VDEC_ALIGN_MASK(u4EstimateHeigh, 15);
                                
                if (u4EstimateWidth <= prVdecEsInfo->u4SeamlessWidth)
                {
                    u4EstimateHeigh = prVdecEsInfo->u4SeamlessHeight;
                }
                else
                {
                    u4EstimateWidth = prVdecEsInfo->u4SeamlessWidth;
                }
         
                //will not change resize resolution within tolerance range
                u4CurDAR = 0;
                u4KeptDAR = 0;
                u4Tolerance = 0;
                if(u4EstimateHeigh != 0)
                {
                    u4CurDAR = (u4EstimateWidth * 1000) / u4EstimateHeigh;
                }
                if (prFbmSeqHdr->u4ResizeHeight != 0)   // prevent div by 0
                {
                    u4KeptDAR = (prFbmSeqHdr->u4ResizeWidth * 1000) / prFbmSeqHdr->u4ResizeHeight;
                    u4Tolerance = u4KeptDAR * VDP_SEAMLESS_RESIZE_TOLERANCE / 100;
                }
                
                if((u4KeptDAR == 0) ||  // first assignment
                    ((u4CurDAR < (u4KeptDAR - u4Tolerance)) || (u4CurDAR > (u4KeptDAR + u4Tolerance))))
                {
                    prFbmSeqHdr->u4ResizeWidth = u4EstimateWidth;
                    prFbmSeqHdr->u4ResizeHeight = (u4EstimateHeigh > 1080) ? 1080 : u4EstimateHeigh;
                }
                if(prVdecEsInfoKeep->fgVPush)
                {
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                }
            }
            LOG(2, "AFD %d, AspRatio Chg %d, [%d:%d], IsSAR %d\n", 
                prFbmSeqHdr->ucActFmt, prFbmSeqHdr->ucAspRatInf, 
                prFbmSeqHdr->u4AspectRatioWidth, prFbmSeqHdr->u4AspectRatioHeight, prFbmSeqHdr->fgPixelAspectRatio);
            #if BDP_LOG_STYLE
            VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_SEQINFO,
                       "[VDEC-H264(%d)] : AspectRatioIdc[%d], MW width:height[%d:%d], SarWidth:SarHeight[%d:%d], AspRatInf[%d], AspRatWidth:AspRatHeight[%d:%d]\n",
                       prDecPrm->ucEsId,
                       prDecPrm->prSPS->rVUI.u4AspectRatioIdc,
                       (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w,
                       (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h,
                       prDecPrm->prSPS->rVUI.u4SarWidth,
                       prDecPrm->prSPS->rVUI.u4SarHeight,
                       prFbmSeqHdr->ucAspRatInf,
                       prFbmSeqHdr->u4AspectRatioWidth,
                       prFbmSeqHdr->u4AspectRatioHeight);
            #endif
            //FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
        }

    	//check if cropping change
		#if defined(CC_USE_DDI)
		// turnkey: 2D model: handled by sysbuild APP according to crop info, 3D model: display 3D
	    if (((prVdecEsInfo->e3DType == VDEC_3D_SBS_LF) || (prVdecEsInfo->e3DType == VDEC_3D_SBS_RF) ||
	              (prVdecEsInfo->e3DType == VDEC_3D_TB_LF) || (prVdecEsInfo->e3DType == VDEC_3D_TB_RF)) &&
	             ((prDecPrm->prSPS->u4FrameCropLeftOffset > 0) || (prDecPrm->prSPS->u4FrameCropRightOffset > (prDecPrm->u4PicW >> 3)) ||
	              (prDecPrm->prSPS->u4FrameCropTopOffset > 0) || (prDecPrm->prSPS->u4FrameCropBottomOffset > (prDecPrm->u4PicH >> 3))))     // 1088 to 1080 should be allowed in 3D mode
	    {
	        if (((prVdecEsInfo->e3DType == VDEC_3D_SBS_RF) && (prDecPrm->prSPS->u4FrameCropRightOffset > (prDecPrm->u4PicW >> 3))) ||
	                ((prVdecEsInfo->e3DType == VDEC_3D_SBS_LF) && (prDecPrm->prSPS->u4FrameCropLeftOffset > 0)))
	        {
	            prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_R_VIEW;
	        }
	        else if (((prVdecEsInfo->e3DType == VDEC_3D_TB_RF) && (prDecPrm->prSPS->u4FrameCropBottomOffset > (prDecPrm->u4PicH >> 3))) ||
	                 ((prVdecEsInfo->e3DType == VDEC_3D_TB_LF) && (prDecPrm->prSPS->u4FrameCropTopOffset > 0)))
	        {
	            prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_R_VIEW;
	        }
	        else
	        {
	            prVdecEsInfo->e3DForce2DType = VDEC_3D_FORCE2D_L_VIEW;
	        }
	        prFbmSeqHdr->fgCropping = FALSE;
	    }
	    else
		#endif
	    if (((prFbmSeqHdr->fgCropping != prDecPrm->prSPS->fgFrameCroppingFlag) ||
	        (prFbmSeqHdr->u4CropX != prDecPrm->prSPS->u4FrameCropLeftOffset) ||
	        (prFbmSeqHdr->u4CropY != prDecPrm->prSPS->u4FrameCropTopOffset) ||
	        (prFbmSeqHdr->u4CropWidth != ((prDecPrm->u4PicW - prFbmSeqHdr->u4CropX) -
	                                                    prDecPrm->prSPS->u4FrameCropRightOffset)) ||
	        (prFbmSeqHdr->u4CropHeight != (prDecPrm->u4PicH - prFbmSeqHdr->u4CropY -
	                     ((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
	                      prDecPrm->prSPS->u4FrameCropBottomOffset :
	                      (prDecPrm->prSPS->u4FrameCropBottomOffset << 1))))))
	    {
	        prFbmSeqHdr->fgCropping = prDecPrm->prSPS->fgFrameCroppingFlag;
	        prFbmSeqHdr->u4CropX = prDecPrm->prSPS->u4FrameCropLeftOffset;
	        prFbmSeqHdr->u4CropY = prDecPrm->prSPS->u4FrameCropTopOffset;
	        prFbmSeqHdr->u4CropWidth = (prDecPrm->u4PicW - prFbmSeqHdr->u4CropX) -
	                                                    prDecPrm->prSPS->u4FrameCropRightOffset;
	        prFbmSeqHdr->u4CropHeight = (prDecPrm->u4PicH - prFbmSeqHdr->u4CropY -
	                     ((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
	                      prDecPrm->prSPS->u4FrameCropBottomOffset :
	                      (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));

	        fgCroppingChg = TRUE;
	        LOG(2, "Cropping change H,V=%d,%d, L,R,T,B=%d,%d,%d,%d\n",
	            prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize,
	            prDecPrm->prSPS->u4FrameCropLeftOffset, prDecPrm->prSPS->u4FrameCropRightOffset,
	            prDecPrm->prSPS->u4FrameCropTopOffset, prDecPrm->prSPS->u4FrameCropBottomOffset);
	    }
    /*if ((prVdecEsInfo->u4PTS != 0) && (prDecPrm->rLastInfo[u1ViewIdx].u4PTS != 0))
    {
        u4fps = (90000*100)/(abs(prVdecEsInfo->u4PTS - prDecPrm->rLastInfo[u1ViewIdx].u4PTS));
        LOG(0, "u4fps %d\n", u4fps);
        prFbmSeqHdr->ucFrmRatCod = u4VDecFrmMapFrameRate(prDecPrm->ucEsId, u4fps);
        LOG(0, "Framerate %d\n", prFbmSeqHdr->ucFrmRatCod);
    }*/
    	u4fps = _VDEC_GetFRFromSPS(prDecPrm);
    	if((u4fps != MPEG_FRAME_RATE_UNKNOWN) && (u4fps != prFbmSeqHdr->ucFrmRatCod))
	    {
	        LOG(1,"Frame rate change %d -> %d\n",prFbmSeqHdr->ucFrmRatCod,u4fps);
	        prFbmSeqHdr->ucFrmRatCod = u4fps;
	        fgAfdChg = TRUE;
	    }        
	    #if (defined (CC_MT5890))
	    #ifdef CC_SUPPORT_VDEC_PREPARSE
	    if (fgIsCheckUFO)
	    {
	        tInCond.u4Width = prDecPrm->u4PicW;
	        tInCond.u4Height = prDecPrm->u4PicH;
	        tInCond.eFmt = VDEC_FMT_H264;
	        tInCond.fg10Bit = prDecPrm->fgIs10BitMode;
	        tInCond.fgProgressive = TRUE;
	        prVdecEsInfo->fgEnableUFO = _VDEC_VLDMngrChkUFO(prDecPrm->ucEsId, &tInCond);
	    }
	    #endif
	    prDecPrm->fgIsUFOMode = (prDecPrm->prSPS->fgFrameMbsOnlyFlag && prVdecEsInfo->fgEnableUFO);
	    #endif
    	if (prDecPrm->fgIsUFOMode)
	    {
	        prFbmSeqHdr->ePackMode = FBM_PM_NORMAL;
	        prFbmSeqHdr->fgUfoEn = TRUE;
	        prFbmSeqHdr->fgRasterOrder = FALSE;
	        prFbmSeqHdr->ucBlockType = TRUE;
	    }
    	else 
	    {
	        prFbmSeqHdr->fgUfoEn = FALSE;
	        prFbmSeqHdr->fgRasterOrder = FALSE;
	        prFbmSeqHdr->ucBlockType = FALSE;
	    }
	    if (prDecPrm->fgIsNewMode)
	    {
	        prFbmSeqHdr->fgRasterOrder = FALSE;
	        prFbmSeqHdr->ucBlockType = TRUE;
	    }
	    else
	    {
	        prFbmSeqHdr->fgRasterOrder = FALSE;
	        prFbmSeqHdr->ucBlockType = FALSE;
	    }
	    if (prDecPrm->fgIs10BitMode)
	    {
	        prFbmSeqHdr->u1Depth = 10;
	        prFbmSeqHdr->ePackMode = FBM_PM_COMPACT;
	    }
	    else
	    {
	        prFbmSeqHdr->u1Depth = 8;
	        prFbmSeqHdr->ePackMode = FBM_PM_NORMAL;
	    }
	    prFbmSeqHdr->fgSeamlessPlay = prVdecEsInfo->fgSeamlessPlay;
	    prFbmSeqHdr->eSeamlessMode = prVdecEsInfo->eSeamlessMode;
	    if (fgAfdChg || fgCroppingChg)
	    {
	        VDEC_INFO_T *prVdecInfo;
	        VDEC_PICTYPE_CB_T rPicType;
	        prVdecInfo=_VDEC_GetInfo();
	        FBM_SetSeqChgFbId(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucPreAllocFbId);
	        prDecPrm->fgIsResChg = TRUE;
	        
	        #ifdef CC_ENABLE_SEQ_CHG_SPEEDUP // 3D playback can't use SEQ_CHG_SPEEDUP
	        if ((!prVdecEsInfo->fgMMPlayback)
	             #ifdef ENABLE_MULTIMEDIA
	             ||(prVdecEsInfo->fgMMPlayback && (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY))
	             #endif
	             )
	        {
	            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG_SPEEDUP);
	            #ifdef TIME_MEASUREMENT
	            TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Send FBM_FLAG_SEQ_CHG_SPEEDUP ");
	            #endif
	        }
	        else
	        {
	            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
	            #ifdef TIME_MEASUREMENT
	            TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Send FBM_FLAG_SEQ_CHG ");
	            #endif
	        }
	        #else
	        FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
	        #ifdef TIME_MEASUREMENT
	        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Send FBM_FLAG_SEQ_CHG ");
	        #endif
	        #endif        
	        LOG(2, "Send FBM_FLAG_SEQ_CHG\n");
	        LOG(2, "PackMode %d, UfoEn %d, RasterOrder %d, BlockType %d\n",
	                prFbmSeqHdr->ePackMode, prFbmSeqHdr->fgUfoEn, prFbmSeqHdr->fgRasterOrder, prFbmSeqHdr->ucBlockType);
	        if(prVdecInfo->pfPicTypeCb && ((prVdecEsInfoKeep->fgVPush) || (prDecPrm->fgIsPVR)))
	        {
	           rPicType.u1EsId=prVdecEsInfoKeep->rPesInfo.ucEsId;
	           rPicType.u4Pts=prVdecEsInfoKeep->rPesInfo.u4PTS;
	           rPicType.u1PicType=(UINT8)VDEC_PIC_TYPE_SEQ;
	           prVdecInfo->pfPicTypeCb(&rPicType);
	        }            
	    }    
	    #ifdef ENABLE_MULTIMEDIA
	    else if ((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH)
	        &&(TRUE == prVdecEsInfoKeep->rPesInfo.fgFrmChanged))
	    {
	        prFbmSeqHdr->ucFrmRatCod = 255;
	        FBM_SetSeqChgFbId(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucPreAllocFbId);
	        #ifdef CC_ENABLE_SEQ_CHG_SPEEDUP
	        FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG_SPEEDUP);
	        #else
	        FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
	        LOG(0, "Dash Send FBM_FLAG_SEQ_CHG\n");
	        #endif
	    }
	    #endif
    }
    prFbmSeqHdr->u4BitRatVal = prVdecEsInfoKeep->rPesInfo.u4BitRate;
    //set AV sync mode
    FBM_SetSyncStc(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucSyncMode, prDecPrm->ucStcId);
#endif

    //trick mode with only 1 field sending from swdmx
    if(((prDecPrm->fgIsMMPlay && (prDecPrm->ucSkipMode == IMode))||((prDecPrm->u1PlayModeInDtv == IMode))) &&
        prDecPrm->prCurrFBufInfo &&
        (prDecPrm->prCurrFBufInfo->u1FBufStatus != FRAME) &&
        (prDecPrm->u1PicStruct == prDecPrm->prCurrFBufInfo->u1FBufStatus))
    {
        if (prDecPrm->u1PicStruct == BOTTOM_FIELD)
        {
            LOG(3, "Need to rpt bottom fld!\n");
        }
        if (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId) ==
                FBM_FB_STATUS_DECODE)
        {
#ifdef CC_REALD_3D_SUPPORT
            _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId,
                                  prDecPrm->prCurrFBufInfo->ucFbId);
#endif
            _VDEC_JobBeforePutDispQ(prDecPrm->ucEsId, prDecPrm->prCurrFBufInfo->ucFbId);
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
                                     FBM_FB_STATUS_DISPLAYQ);
            LOG(0, "PQS1 T(0x%x), (-), POC %d %d\n",
                            prVdecEsInfoKeep->rPesInfo.u4PTS, prDecPrm->prCurrFBufInfo->i4POC,prDecPrm->fgIsMMPlay);
            prVdecEsInfo->u4DisplayQPicCnt++;
        }
    }

    //if(tMpvDecPrm->tSliceHdr->fgNoOutputOfPriorPicsFlag) // clear all prior pic in DPB
#ifdef CC_ALLEGRO_MVC
    if (fgIsIDRPic(prDecPrm->u1NalUnitType) && (u1ViewIdx == 0))
#else
    if (fgIsIDRPic(prDecPrm->u1NalUnitType) /*|| (!prDecPrm->fgNalNonIdr)*/)
#endif
    {
        prDecPrm->u1DecFBufIdx = 0;
        prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
        _VDEC_FlushDPB(prDecPrm);
        if (prDecPrm->fgIsMVC)
        {
            prDecPrm->rLastInfo[DEPEND_VIEW].u4LastFrameNum = 0xffffffff;
            prDecPrm->u1CurViewIdx = DEPEND_VIEW;   //for _VDEC_FlushDPB
            _VDEC_FlushDPB(prDecPrm);
            prDecPrm->u1CurViewIdx = BASE_VIEW;
        }
    }

    u4UnusedShortTermFrameNum = (prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum + 1) %
                                prDecPrm->prSPS->u4MaxFrameNum;
    u4CurrFrameNum = prDecPrm->prSliceHdr->u4FrameNum;

    if((prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME))
      && ((u4CurrFrameNum != prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum)
    && (u4CurrFrameNum != u4UnusedShortTermFrameNum))
    #ifdef ENABLE_MULTIMEDIA
    && (((!prDecPrm->fgIsMMPlay)&&(prDecPrm->u1PlayModeInDtv==IPBMode)) ||
    ((prDecPrm->u1PlayModeInDtv==IPBMode)&&((prVdecEsInfo->eContainerType>= SWDMX_FMT_MPEG2_TS) && (prVdecEsInfo->eContainerType< SWDMX_FMT_VC1_ES) && (prVdecEsInfo->u4MMSpeed < STC_SPEED_TYPE_FORWARD_2X)))
    || (prDecPrm->fgIsMMPlay && (prVdecEsInfo->u4MMSpeed <= STC_SPEED_TYPE_FORWARD_2X)))
    && (!prDecPrm->fgIsMVC))
    #else
    && (!prDecPrm->fgIsMMPlay) && (!prDecPrm->fgIsMVC))
    #endif
    {
        if (prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum < (UINT32)0xffffffff)
        {
            LOG(0, "FrmGap!%d\n",
            (u4CurrFrameNum > u4UnusedShortTermFrameNum) ?
            (u4CurrFrameNum - u4UnusedShortTermFrameNum) :
            ((prDecPrm->prSPS->u4MaxFrameNum - u4UnusedShortTermFrameNum) + u4CurrFrameNum));
            if (prDecPrm->prSPS->fgGapsInFrameNumValueAllowedFlag)
            {
                LOG(2, "FillFrmNumGap?\n");
            }

            if (!prVdecEsInfoKeep->fgVPush)
            {
            #ifdef ENABLE_MULTIMEDIA
                if (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_WIFI_DISPLAY)
            #endif
                {
                #ifndef __MODEL_slt__
                    //if (SINGLE_CORE_MODE == prDecPrm->ucDualCoreMode)
                    {
                    #ifdef TIME_MEASUREMENT                                  
                        TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "FramGap");
                        TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "FramGap");                               
                    #endif
                        _VDEC_FlushDPB(prDecPrm);
                        _VDEC_ClrDPB(prDecPrm);
                        _vPartitionDPB(prDecPrm);   //reset DPB and MV buffer
                        prDecPrm->prSPS->fgSPSValid = FALSE;
                        prDecPrm->fgAllocFbFail = TRUE;
                        prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
                        prDecPrm->i4LastPOC[u1ViewIdx] = -99999;
                        prDecPrm->fgPOCWrap = FALSE;
                        prDecPrm->rSEI.u1ExtMatchFlg = FALSE;
                        prDecPrm->rSEI.u1WaitRAPCnt = 0;
                        prDecPrm->rSEI.u1WaitRAPRefCnt = 0;
                        prDecPrm->rSEI.u1WaitRAPICnt = 0;
                        prDecPrm->fgIsFirstFrm = FALSE;
                        return;
                    }
                #endif
                }
            }
        }
    }

    // Find an empty fbuf
    if ((prDecPrm->rLastInfo[u1ViewIdx].fgLastIsRef && (!fgIsRefPic(prDecPrm->u1NalRefIdc))) ||
            (!prDecPrm->rLastInfo[u1ViewIdx].fgLastIsRef && fgIsRefPic(prDecPrm->u1NalRefIdc)))
    {
        fgRefChg = TRUE;
    }
    if ((prDecPrm->prCurrFBufInfo == NULL)
            || (prDecPrm->prCurrFBufInfo->u1FBufStatus == NO_PIC)
            || (prDecPrm->prCurrFBufInfo->u1FBufStatus & prDecPrm->u1PicStruct)
            || (u4CurrFrameNum != prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum)
            || fgRefChg)
    {
        if (prDecPrm->prCurrFBufInfo &&
                ((prDecPrm->prCurrFBufInfo->u1FBufStatus == TOP_FIELD) ||
                 (prDecPrm->prCurrFBufInfo->u1FBufStatus == BOTTOM_FIELD)))
        {
            LOG(2, "NPF!Prev field IsRef %d\n", prDecPrm->prCurrFBufInfo->fgIsBufRef);
        }
        _vAllocateFBuf(prDecPrm);    //find new prDecPrm->prCurrFBufInfo        
        if (prDecPrm->fgAllocFbFail)
        {
            return;
        }
        //_u4MaxDecCycle = 0; //Init the count for decoding cycle
    }
    if (prDecPrm->prCurrFBufInfo)
    {
        if (fgIsRefPic(prDecPrm->u1NalRefIdc))
        {
            prDecPrm->prCurrFBufInfo->fgIsBufRef = TRUE;
        }
        prDecPrm->prCurrFBufInfo->u1FBufStatus |= prDecPrm->u1PicStruct;
        prDecPrm->u1PicStructKeep = prDecPrm->prCurrFBufInfo->u1FBufStatus;
        /*if (FRAME == prDecPrm->prCurrFBufInfo->u1FBufStatus)
        {
            prDecPrm->rLastInfo[u1ViewIdx].u4PTS = prVdecEsInfo->u4PTS;
        }*/
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
    }
}


// *********************************************************************
// Function    : BOOL fgChkRefInfo(DWRD dwFBufIdx, DWRD dwRefType)
// Description : Check if reference picture should be insered to ref pic list
// Parameter   : None
// Return      : None
// *********************************************************************
static BOOL _fgChkRefInfo(const VDEC_AVC_DecPrm* prDecPrm, UINT32 u4FBufIdx, UINT32 u4RefType)
{
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = u4FBufIdx >> 8;
    u4FBufIdx &= 0xFF;
    
    if ((u4FBufIdx >= 18) || (u1ViewIdx >= MAX_VIEW_NUM))
    {
        LOG(1, "ChkRef Idx Err!\n");
        return FALSE;
    }

    if (fgIsFrmPic(prDecPrm->u1PicStruct))
    {
        // According to spec 8.2.4.2.1
        // NOTE: A non-pared reference fiedl is not used for inter prediction for decoding a frame.
        if ((prDecPrm->arFBufInfo[u1ViewIdx][u4FBufIdx].u1TFldRefType == (UINT8)u4RefType) &&
                (prDecPrm->arFBufInfo[u1ViewIdx][u4FBufIdx].u1BFldRefType == (UINT8)u4RefType))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if ((prDecPrm->arFBufInfo[u1ViewIdx][u4FBufIdx].u1TFldRefType == (UINT8)u4RefType) ||
                (prDecPrm->arFBufInfo[u1ViewIdx][u4FBufIdx].u1BFldRefType == (UINT8)u4RefType))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}


// *********************************************************************
// Function    : void vInsertRefPicList(TRefPicList *ptRefPicList, INT iCurrPOC, DWRD dwRefPicListInfo)
// Description : Instert Short ref pic list item
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vInsertRefPicList(const VDEC_AVC_DecPrm* prDecPrm, VDEC_REF_PIC_LIST_T* prRefPicList, INT32 i4CurrPOC, UINT32 u4RefPicListInfo)
{
    INT32 j;
    UINT8 u1RefType; // 1-> Short 2-> Long
    UINT8 u1ListType; // 0-> P, 1-> B_0, 2->B_1
    UINT8 u1FBufIdx, u1ViewIdx, u1LRefInverview, u1CurIdx, u1NextIdx;
    INT32 i4Comp0 = 0, i4Comp1 = 0;
    UINT32 u4Temp;
    BOOL fgSwitch;

    ASSERT(prRefPicList != NULL);
    ASSERT(prDecPrm != NULL);

    u1RefType = (UINT8)(u4RefPicListInfo & 0xf);
    // 0:P_T, 1:P_B, 2:B0_T, 3:B0_B, 4:B1_T, 5:B1_B, 6:P_T_L, 7:P_B_L, 8:B_T_L, 9:B_B_L,
    u1ListType = (UINT8)((u4RefPicListInfo >> 8) & 0xf);
    u1FBufIdx = (UINT8)((u4RefPicListInfo >> 16) & 0xff);
    u1ViewIdx = (UINT8)((u4RefPicListInfo >> 24) & 0xf);
    u1LRefInverview = (UINT8)((u4RefPicListInfo >> 28) & 0xf);

    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "InsRef ViewIdx Err!\n");
        return;
    }

    if (u1RefType == SREF_PIC)
    {
        // 1st: Insert the current to the last idx
        prRefPicList->au4FBufIdx[prRefPicList->u4RefPicCnt] =
            u1FBufIdx | (u1ViewIdx << 8) | (prDecPrm->fgIsMVC << 24);
        if (u1ViewIdx != prDecPrm->u1CurViewIdx) //inter-view
        {
            prRefPicList->au4FBufIdx[prRefPicList->u4RefPicCnt] |= (1 << 16);
        }
        // 2nd: shift Shortterm ref pic
        for (j = (INT32)prRefPicList->u4RefPicCnt - 1; j >= 0; j--)
        {
            //Klockwork checking
            u1CurIdx = prRefPicList->au4FBufIdx[j] & 0xFF;
            u1NextIdx = prRefPicList->au4FBufIdx[j+1] & 0xFF;
            if ((u1CurIdx >= MAX_FRAME_NUM) ||
                    (u1NextIdx >= MAX_FRAME_NUM))
            {
                LOG(3, "FBufIdx out of range\n");
            }
            else
            {
                fgSwitch = FALSE;
                if (u1ListType == 0)
                {
                    if (prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4TFldPicNum >
                            prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4TFldPicNum)
                    {
                        fgSwitch = TRUE;
                    }
                }
                else if (u1ListType == 1)
                {
                    if (prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4BFldPicNum >
                            prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4BFldPicNum)
                    {
                        fgSwitch = TRUE;
                    }
                }
                else if ((u1ListType == 2) || (u1ListType == 3))
                {
                    i4Comp0 = (u1ListType == 2) ? prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4TFldPOC :
                              prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4BFldPOC;
                    i4Comp1 = (u1ListType == 2) ? prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4TFldPOC :
                              prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4BFldPOC;
                    if ((fgIsFrmPic(prDecPrm->u1PicStruct) ?
                            (((i4Comp0 < i4CurrPOC) &&
                              (i4Comp1 < i4CurrPOC) && (i4Comp1 > i4Comp0)) ||
                             ((i4Comp0 >= i4CurrPOC) && (i4Comp1 < i4Comp0)) ||
                             ((i4Comp0 >= i4CurrPOC) && (i4Comp1 < i4CurrPOC))) :
                            (((i4Comp0 <= i4CurrPOC) && (i4Comp1 <= i4CurrPOC) && (i4Comp1 > i4Comp0)) ||
                             ((i4Comp0 > i4CurrPOC) && (i4Comp1 < i4Comp0)) ||
                             ((i4Comp0 > i4CurrPOC) && (i4Comp1 <= i4CurrPOC))
                            )) )
                    {
                        fgSwitch = TRUE;
                    }
                }
                else if ((u1ListType == 4) || (u1ListType == 5))
                {
                    i4Comp0 = (u1ListType == 4) ? prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4TFldPOC :
                              prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4BFldPOC;
                    i4Comp1 = (u1ListType == 4) ? prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4TFldPOC :
                              prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4BFldPOC;
                    if (((i4Comp0 <= i4CurrPOC) && (i4Comp1 > i4Comp0)) ||
                            ((i4Comp0 > i4CurrPOC) && (i4Comp1 > i4CurrPOC) && (i4Comp1 < i4Comp0)) ||
                            ((i4Comp0 <= i4CurrPOC) && (i4Comp1 > i4CurrPOC)))
                    {
                        fgSwitch = TRUE;
                    }
                }
                if (fgSwitch)
                {
                    u4Temp = prRefPicList->au4FBufIdx[j+1];
                    prRefPicList->au4FBufIdx[j+1] = prRefPicList->au4FBufIdx[j];
                    prRefPicList->au4FBufIdx[j] = u4Temp;
                }
            }
        }
        prRefPicList->u4RefPicCnt ++;
    }
    else if (u1RefType == LREF_PIC)
    {
        prRefPicList->au4FBufIdx[prRefPicList->u4RefPicCnt] =
            u1FBufIdx | (u1ViewIdx << 8) | (prDecPrm->fgIsMVC << 24);
        if (u1ViewIdx != prDecPrm->u1CurViewIdx) //inter-view
        {
            prRefPicList->au4FBufIdx[prRefPicList->u4RefPicCnt] |= (1 << 16);
            if (u1LRefInverview)
            {
                prRefPicList->au4FBufIdx[prRefPicList->u4RefPicCnt] |= (1 << 17);
            }
        }
        else
        {
            for (j = (INT32)(prRefPicList->u4RefPicCnt - 1); j >= 0; j--)
            {
                //Klockwork checking
                u1CurIdx = prRefPicList->au4FBufIdx[j] & 0xFF;
                u1NextIdx = prRefPicList->au4FBufIdx[j+1] & 0xFF;
                if ((u1CurIdx >= MAX_FRAME_NUM) ||
                        (u1NextIdx >= MAX_FRAME_NUM))
                {
                    LOG(3, "FBufIdx LT out of range\n");
                }
                else
                {
                    if (u1ListType == 6)
                    {
                        i4Comp0 = prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4TFldLongTermPicNum;
                        i4Comp1 = prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4TFldLongTermPicNum;
                    }
                    else if (u1ListType == 7)
                    {
                        i4Comp0 = prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].i4BFldLongTermPicNum;
                        i4Comp1 = prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].i4BFldLongTermPicNum;
                    }
                    else if (u1ListType == 8)
                    {
                        i4Comp0 = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].u4TFldLongTermFrameIdx;
                        i4Comp1 = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].u4TFldLongTermFrameIdx;
                    }
                    else if (u1ListType == 9)
                    {
                        i4Comp0 = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][u1CurIdx].u4BFldLongTermFrameIdx;
                        i4Comp1 = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][u1NextIdx].u4BFldLongTermFrameIdx;
                    }
                    if (i4Comp1 < i4Comp0)
                    {
                        u4Temp = prRefPicList->au4FBufIdx[j+1];
                        prRefPicList->au4FBufIdx[j+1] = prRefPicList->au4FBufIdx[j];
                        prRefPicList->au4FBufIdx[j] = u4Temp;
                    }
                }
            }
        }
        prRefPicList->u4RefPicCnt ++;
    }
}


// *********************************************************************
// Function    : void vSetupPRefPicList(DWRD *dwRefIdx, TRefPicList *ptTFldRefPicList, TRefPicList *ptBFldRefPicList)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vSetupPRefPicList(UINT32 u4VDecID, const VDEC_REF_PIC_LIST_T* prRefPicList,
                               VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT32 *pu4RefIdx,
                               UINT32 u4TFldListIdx, UINT32 u4BFldListIdx)
{
    UINT32 i ;
    UINT32 u4TotalFBuf;
    BOOL fgIsBFldInterview, fgIsTFldInterview;

    ASSERT(pu4RefIdx != NULL);
    ASSERT(prRefPicList != NULL);
    ASSERT(prFBufInfo != NULL);

    u4TotalFBuf = (prRefPicList[u4TFldListIdx].u4RefPicCnt >= prRefPicList[u4BFldListIdx].u4RefPicCnt) ?
                  prRefPicList[u4TFldListIdx].u4RefPicCnt : prRefPicList[u4BFldListIdx].u4RefPicCnt;

    if (fgIsFrmPic(u1PicStruct))
    {
        u4TFldListIdx = (prRefPicList[u4TFldListIdx].u4RefPicCnt >= prRefPicList[u4BFldListIdx].u4RefPicCnt) ?
                        u4TFldListIdx : u4BFldListIdx;
    }
    for (i = 0; i < u4TotalFBuf; i++)
    {
        fgIsTFldInterview = (BOOL)(prRefPicList[u4TFldListIdx].au4FBufIdx[i] >> 16);
        fgIsBFldInterview = (BOOL)(prRefPicList[u4BFldListIdx].au4FBufIdx[i] >> 16);

        if (fgIsFrmPic(u1PicStruct))
        {
            _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prFBufInfo,
                                    FRAME + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
            pu4RefIdx[0] ++;
        }
        else if (u1PicStruct == TOP_FIELD)
        {
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && (!fgIsTFldInterview))
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && (!fgIsBFldInterview))
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
        else if (u1PicStruct == BOTTOM_FIELD)
        {
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && (!fgIsBFldInterview))
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
            if (i < prRefPicList[u4TFldListIdx].u4RefPicCnt && (!fgIsTFldInterview))
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
    }

    //append interview field at the last of the list
    for (i = 0; i < u4TotalFBuf; i++)
    {
        fgIsTFldInterview = (BOOL)(prRefPicList[u4TFldListIdx].au4FBufIdx[i] >> 16);
        fgIsBFldInterview = (BOOL)(prRefPicList[u4BFldListIdx].au4FBufIdx[i] >> 16);

        if (u1PicStruct == TOP_FIELD)
        {
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && fgIsTFldInterview)
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && fgIsBFldInterview)
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
        else if (u1PicStruct == BOTTOM_FIELD)
        {
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && fgIsBFldInterview)
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
            if (i < prRefPicList[u4TFldListIdx].u4RefPicCnt && fgIsTFldInterview)
            {
                _VDEC_SetPRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
    }
}

// *********************************************************************
// Function    : void vVDecSetPRefPicList(void)
// Description : Set P ref pic list by Pic Num
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vSetPRefPicList(UINT32 u4VDecId, VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 i;
    UINT32 u4AddTop = 0;
    UINT32 u4AddBot = 0;
    UINT32 u4Temp;
    UINT32 u4CurrPicNum;
    UINT8 u1ViewIdx;
    #if BDP_LOG_STYLE
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;
    #endif
    ASSERT(prDecPrm != NULL); 
    
#if BDP_LOG_STYLE
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    ASSERT(prVdecEsInfoKeep != NULL); 
#endif

    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }
    prDecPrm->prCurrFBufInfo->u4FrameNum = prDecPrm->prSliceHdr->u4FrameNum;
    u4CurrPicNum = (fgIsFrmPic(prDecPrm->u1PicStruct)) ?
                   prDecPrm->prCurrFBufInfo->u4FrameNum :
                   ((prDecPrm->prCurrFBufInfo->u4FrameNum << 1) + 1);
    prDecPrm->prCurrFBufInfo->i4PicNum = (INT32)u4CurrPicNum;
    prDecPrm->arRefPicList[0].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[1].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[4].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[5].u4RefPicCnt = 0;

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "SetPRef ViewIdx Err!\n");
        return;
    }
    
    ASSERT(prDecPrm->u4MaxFBufNum <= 18);
    for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
    {
        if (prDecPrm->u1PicStruct == TOP_FIELD)
        {
            u4AddTop = 1;
            u4AddBot = 0;
        }
        else if (prDecPrm->u1PicStruct == BOTTOM_FIELD)
        {
            u4AddTop = 0;
            u4AddBot = 1;
        }
        else
        {
            u4AddTop = 0;
            u4AddBot = 0;
        }
        
    #if (defined (CC_MT5890))
        prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsUFOMode = prDecPrm->fgIsUFOMode;
    #endif
        if (_fgChkRefInfo(prDecPrm, i + (u1ViewIdx << 8), SREF_PIC))
        {
            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum > prDecPrm->prCurrFBufInfo->u4FrameNum)
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap =
                    (INT32)(prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum -
                            prDecPrm->prSPS->u4MaxFrameNum);
            }
            else
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum;
            }
            if (fgIsFrmPic(prDecPrm->u1PicStruct))
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum = prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap;
            }
            else
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum = (prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap * 2) + 1;
            }

            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC)
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldPicNum =
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap * 2) + (INT32)u4AddTop;
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[0], (INT32)i,
                                   SREF_PIC + (0 << 8) + ( i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC)
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldPicNum =
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].i4FrameNumWrap * 2) + (INT32)u4AddBot;
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[1], (INT32)i,
                                   SREF_PIC + (1 << 8) + ( i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            // set display flag according to reference frame error type
            if ((VDEC_LAE0 != u4VDecId) && (VDEC_LAE1 != u4VDecId))
            {
                if (prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsErr)
                {
                    prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                    #if BDP_LOG_STYLE
                    VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                     "[H264 VDEC%d]: _vSetPRefPicList PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                     prDecPrm->ucEsId,
                                      prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum, 
                                      prVdecEsInfoKeep->rPesInfo.ucPicType,
                                      prVdecEsInfoKeep->rPesInfo.u4PTS);
                    #endif
                }
            }
        }
        else if (_fgChkRefInfo(prDecPrm, i + (u1ViewIdx << 8), LREF_PIC))
        {
            prDecPrm->arFBufInfo[u1ViewIdx][i].i4LongTermPicNum = (INT32)prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx;

            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldLongTermPicNum =
                    (INT32)((prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx << 1) + u4AddTop);
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[4], (INT32)i,
                                   LREF_PIC + (6 << 8) + ( i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC)
            {
                prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldLongTermPicNum =
                    (INT32)((prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx << 1) + u4AddBot);
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[5],
                                   (INT32)i, LREF_PIC + (7 << 8) + ( i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            // set display flag according to reference frame error type
            if ((VDEC_LAE0 != u4VDecId) && (VDEC_LAE1 != u4VDecId))
            {
                if (prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsErr)
                {
                    prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                    #if BDP_LOG_STYLE
                    VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                     "[H264 VDEC%d]: _vSetPRefPicList2 PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                     prDecPrm->ucEsId,
                                     prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum, 
                                     prVdecEsInfoKeep->rPesInfo.ucPicType,
                                     prVdecEsInfoKeep->rPesInfo.u4PTS);
                    #endif
                }
            }
        }
    }

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if (u1ViewIdx > 0)  //link interview frame to long term list
    {
        i = prDecPrm->u4POCFbIdx;

        if (_fgChkRefInfo(prDecPrm, i, LREF_PIC))
        {
            prDecPrm->arFBufInfo[0][i].i4LongTermPicNum = (INT32)prDecPrm->arFBufInfo[0][i].u4LongTermFrameIdx;

            if (prDecPrm->arFBufInfo[0][i].u1TFldRefType == LREF_PIC)
            {
                prDecPrm->arFBufInfo[0][i].i4TFldLongTermPicNum =
                    (INT32)((prDecPrm->arFBufInfo[0][i].u4TFldLongTermFrameIdx << 1) + u4AddTop);
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[4], (INT32)i,
                                   LREF_PIC + (6 << 8) + ( i << 16) + (1 << 28));   //bit 28, longterm interview
            }
            if (prDecPrm->arFBufInfo[0][i].u1BFldRefType == LREF_PIC)
            {
                prDecPrm->arFBufInfo[0][i].i4BFldLongTermPicNum =
                    (INT32)((prDecPrm->arFBufInfo[0][i].u4BFldLongTermFrameIdx << 1) + u4AddBot);
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[5],
                                   (INT32)i, LREF_PIC + (7 << 8) + ( i << 16) + (1 << 28));   //bit 28, longterm interview
            }
        }
        else
        {
            if ((prDecPrm->u1PicStruct == TOP_FIELD) || (prDecPrm->u1PicStruct == FRAME))
            {
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[4], (INT32)i,
                                   LREF_PIC + (6 << 8) + ( i << 16));
            }
            if ((prDecPrm->u1PicStruct == BOTTOM_FIELD) || (prDecPrm->u1PicStruct == FRAME))
            {
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[5],
                                   (INT32)i, LREF_PIC + (7 << 8) + ( i << 16));
            }
        }
    }
#endif

    vVDecWriteMC(u4VDecId, RW_MC_P_LIST0_FLD, 0);
    vVDecWriteAVCVLD(u4VDecId, RW_AVLD_RESET_PIC_NUM, RESET_PIC_NUM);

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    u4Temp = 0;
    while (u4VDecReadAVCVLD(u4VDecId, RW_AVLD_RESET_PIC_NUM) != 0)
    {
        u4Temp++;
        if (u4Temp > 100)
        {
            LOG(3, "Wait PicNum rst fail.\n");
            break;
        }
    }
#endif

    vVDecWriteAVCVLD(u4VDecId, RW_AVLD_MAX_PIC_NUM, fgIsFrmPic(prDecPrm->u1PicStruct) ?
                    prDecPrm->prSPS->u4MaxFrameNum : (prDecPrm->prSPS->u4MaxFrameNum << 1));
    vVDecWriteAVCVLD(u4VDecId, RW_AVLD_CUR_PIC_NUM, u4CurrPicNum);
    u4Temp = 0;
    _vSetupPRefPicList(u4VDecId, prDecPrm->arRefPicList, (VDEC_FBUF_INFO_T *)prDecPrm->arFBufInfo, prDecPrm->u1PicStruct, &u4Temp, 0, 1);
    _vSetupPRefPicList(u4VDecId, prDecPrm->arRefPicList, (VDEC_FBUF_INFO_T *)prDecPrm->arFBufInfo, prDecPrm->u1PicStruct, &u4Temp, 4, 5);
}


// *********************************************************************
// Function    : void vSetupPRefPicList(DWRD *dwRefIdx, DWRD dwTFldListIdx, DWRD dwBFldListIdx)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vSetupBRefPicList(UINT32 u4VDecID, const VDEC_REF_PIC_LIST_T* prRefPicList,
                               VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT32 *pu4RefIdx,
                               UINT32 u4TFldListIdx, UINT32 u4BFldListIdx, BOOL *pfgDiff)
{
    UINT32 i;
    UINT32 u4TotalFBuf;
    BOOL fgIsTFldInterview, fgIsBFldInterview, fgIsDiff;

    ASSERT(pu4RefIdx != NULL);
    ASSERT(pfgDiff != NULL);
    ASSERT(prRefPicList != NULL);
    ASSERT(prFBufInfo != NULL);
    ASSERT(u4TFldListIdx < 6);
    ASSERT(u4BFldListIdx < 6);

    u4TotalFBuf = (prRefPicList[u4TFldListIdx].u4RefPicCnt >= prRefPicList[u4BFldListIdx].u4RefPicCnt) ?
                  prRefPicList[u4TFldListIdx].u4RefPicCnt : prRefPicList[u4BFldListIdx].u4RefPicCnt;
    if (fgIsFrmPic(u1PicStruct))
    {
        u4TFldListIdx = (prRefPicList[u4TFldListIdx].u4RefPicCnt >= prRefPicList[u4BFldListIdx].u4RefPicCnt) ?
                        u4TFldListIdx : u4BFldListIdx;
    }

    for (i = 0; i < u4TotalFBuf; i++)
    {
        fgIsTFldInterview = (BOOL)(prRefPicList[u4TFldListIdx].au4FBufIdx[i] >> 16);
        fgIsBFldInterview = (BOOL)(prRefPicList[u4BFldListIdx].au4FBufIdx[i] >> 16);
        
        if (fgIsFrmPic(u1PicStruct))
        {
            if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
            {
                fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                    FRAME + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
            }
            else
            {
                fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                    FRAME + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
            }
            if ((!pfgDiff[0]) && fgIsDiff)
            {
                pfgDiff[0] = TRUE;
            }
            pu4RefIdx[0] ++;           
        }
        else if (u1PicStruct == TOP_FIELD)
        {
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && !fgIsTFldInterview)
            {
                if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                {
                    fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                }
                else
                {
                    fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                }
                if (u4TFldListIdx < 4) // Short-term only
                {                
                    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                    {
                        fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx+2], prRefPicList[u4TFldListIdx+4], prFBufInfo,
                                            TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx + 2);
                    }
                    else
                    {
                        fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx+2], prRefPicList[u4TFldListIdx+4], prFBufInfo,
                                            TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx + 2);
                    }
                }
                if ((!pfgDiff[0]) && fgIsDiff)
                {
                    pfgDiff[0] = TRUE;
                }
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && !fgIsBFldInterview)
            {               
                if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                {
                    fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                }
                else
                {
                    fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                }
                if (u4BFldListIdx < 4) // Short-term only
                {                    
                    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                    {
                        fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx+2], prRefPicList[u4BFldListIdx+4], prFBufInfo,
                                            BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx + 2);
                    }
                    else
                    {
                        fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx+2], prRefPicList[u4BFldListIdx+4], prFBufInfo,
                                            BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx + 2);
                    }
                }
                if ((!pfgDiff[0]) && fgIsDiff)
                {
                    pfgDiff[0] = TRUE;
                }
                pu4RefIdx[0] ++;
            }
        }
        else if (u1PicStruct == BOTTOM_FIELD)
        {
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && !fgIsBFldInterview)
            {                
                if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                {
                    fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                }
                else
                {
                    fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                }
                if (u4BFldListIdx < 4) // Short-term only
                {
                    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                    {
                        fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx+2], prRefPicList[u4BFldListIdx+4], prFBufInfo,
                                            BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx + 2);
                    }
                    else
                    {
                        fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx+2], prRefPicList[u4BFldListIdx+4], prFBufInfo,
                                            BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx + 2);
                    }
                }
                if ((!pfgDiff[0]) && fgIsDiff)
                {
                    pfgDiff[0] = TRUE;
                }
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && !fgIsTFldInterview)
            {                
                if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                {
                    fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                }
                else
                {
                    fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                }
                if (u4TFldListIdx < 4) // Short-term only
                {                    
                    if ((VDEC_LAE0 == u4VDecID) || (VDEC_LAE1 == u4VDecID))
                    {
                        fgIsDiff = _VDEC_LAE_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx+2], prRefPicList[u4TFldListIdx+4], prFBufInfo,
                                            TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx + 2);
                    }
                    else
                    {
                        fgIsDiff = _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx+2], prRefPicList[u4TFldListIdx+4], prFBufInfo,
                                            TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx + 2);
                    }
                }
                if ((!pfgDiff[0]) && fgIsDiff)
                {
                    pfgDiff[0] = TRUE;
                }
                pu4RefIdx[0] ++;
            }
        }
    }

    //append interview field at the last of the list
    for (i = 0; i < u4TotalFBuf; i++)
    {
        fgIsTFldInterview = (BOOL)(prRefPicList[u4TFldListIdx].au4FBufIdx[i] >> 16);
        fgIsBFldInterview = (BOOL)(prRefPicList[u4BFldListIdx].au4FBufIdx[i] >> 16);

        if (u1PicStruct == TOP_FIELD)
        {
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && fgIsTFldInterview)
            {
                _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && fgIsBFldInterview)
            {
                _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
        else if (u1PicStruct == BOTTOM_FIELD)
        {
            if ((i < prRefPicList[u4BFldListIdx].u4RefPicCnt) && fgIsBFldInterview)
            {
                _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4BFldListIdx], prRefPicList[u4BFldListIdx+2], prFBufInfo,
                                        BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
                pu4RefIdx[0] ++;
            }
            if ((i < prRefPicList[u4TFldListIdx].u4RefPicCnt) && fgIsTFldInterview)
            {
                _VDEC_SetBRefPicListReg(u4VDecID, prRefPicList[u4TFldListIdx], prRefPicList[u4TFldListIdx+2], prFBufInfo,
                                        TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
                pu4RefIdx[0] ++;
            }
        }
    }
}


// *********************************************************************
// Function    : void vVDecSetBRefPicList(void)
// Description : Set Ref Pic List for P slice
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vSetBRefPicList(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 i;
    UINT32 u4Temp;
    INT32 i4CurrPOC;
    UINT32 u4TotalRPIdx;
    BOOL fgDiff;
    UINT8 u1ViewIdx;
    #if BDP_LOG_STYLE
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;
    #endif
    ASSERT(prDecPrm != NULL);
    
    #if BDP_LOG_STYLE
    prVdecEsInfoKeep =  _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    #endif

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "setBRef ViewIdx Err!\n");
        return;
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }

    //_VDEC_SetCurrPOC(prDecPrm);
    if (fgIsFrmPic(prDecPrm->u1PicStruct))
    {
        i4CurrPOC = prDecPrm->prCurrFBufInfo->i4POC;
        vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_TFLD_POC, (UINT32)(prDecPrm->prCurrFBufInfo->i4TFldPOC & 0x3ffff));
        vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_BFLD_POC, (UINT32)(prDecPrm->prCurrFBufInfo->i4BFldPOC & 0x3ffff));
        vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_POC, (UINT32)(prDecPrm->prCurrFBufInfo->i4POC & 0x3ffff));
    }
    else
    {
        i4CurrPOC = (prDecPrm->u1PicStruct == TOP_FIELD) ?
                    prDecPrm->prCurrFBufInfo->i4TFldPOC : prDecPrm->prCurrFBufInfo->i4BFldPOC;
        vVDecWriteAVCMV(u4VDecID, RW_AMV_CURR_POC, (UINT32)(i4CurrPOC & 0x3ffff));
    }

    prDecPrm->arRefPicList[0].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[1].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[2].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[3].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[4].u4RefPicCnt = 0;
    prDecPrm->arRefPicList[5].u4RefPicCnt = 0;

    ASSERT(prDecPrm->u4MaxFBufNum <= 18);
    for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
    {
        if (_fgChkRefInfo(prDecPrm, (UINT32)i + (UINT32)(u1ViewIdx << 8), SREF_PIC))
        {
            // Avoid non-existing pic into ref pic list when POC type = 0
            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC))
                //            && !((_tMpvDecPrm.tSPS->dwPicOrderCntType == 0) && (_ptFBufInfo[i].fgNonExisting)))
            {
                // B0
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[0], i4CurrPOC,
                                   SREF_PIC + (2 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
                // B1
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[2], i4CurrPOC,
                                   SREF_PIC + (4 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC)
                    && !((prDecPrm->prSPS->u4PicOrderCntType == 0)
                         && (prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting)))
            {
                // B0
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[1], i4CurrPOC,
                                   SREF_PIC + (3 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
                // B1
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[3], i4CurrPOC,
                                   SREF_PIC + (5 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            // set display flag according to reference frame error type
            if ((VDEC_LAE0 != u4VDecID) && (VDEC_LAE1 != u4VDecID))
            {
                if (prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsErr)
                {
                    prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                    #if BDP_LOG_STYLE
                    VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                 "[H264 VDEC%d]: _vSetBRefPicList PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                     prDecPrm->ucEsId,
                                     prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum, 
                                     prVdecEsInfoKeep->rPesInfo.ucPicType,
                                     prVdecEsInfoKeep->rPesInfo.u4PTS);
                    #endif
                }
            }
        }
        else if (_fgChkRefInfo(prDecPrm, (UINT32)i + (UINT32)(u1ViewIdx << 8), LREF_PIC))
        {
            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)
            {
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[4], i4CurrPOC,
                                   LREF_PIC + (8 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC)
            {
                _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[5], i4CurrPOC,
                                   LREF_PIC + (9 << 8) + ((UINT32)i << 16) + ((UINT32)u1ViewIdx << 24));
            }
            // set display flag according to reference frame error type
            if ((VDEC_LAE0 != u4VDecID) && (VDEC_LAE1 != u4VDecID))
            {
                if (prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsErr)
                {
                    prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                    #if BDP_LOG_STYLE
                    VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                     "[H264 VDEC%d]: _vSetBRefPicList2 PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                     prDecPrm->ucEsId,
                                     prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum, 
                                     prVdecEsInfoKeep->rPesInfo.ucPicType,
                                     prVdecEsInfoKeep->rPesInfo.u4PTS);
                    #endif
                }
            }
        }
    }

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    if (u1ViewIdx > 0)
    {
        i = prDecPrm->u4POCFbIdx;
        i4CurrPOC = prDecPrm->arFBufInfo[0][i].i4POC;
        if (_fgChkRefInfo(prDecPrm, i, LREF_PIC))
        {
            u4Temp = 1 << 28; //longterm interview bit
        }
        else
        {
            u4Temp = 0;
        }
        if ((prDecPrm->u1PicStruct == TOP_FIELD) || (prDecPrm->u1PicStruct == FRAME))
        {
            _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[4], i4CurrPOC,
                               LREF_PIC + (8 << 8) + ((UINT32)i << 16) + u4Temp);
        }
        if ((prDecPrm->u1PicStruct == BOTTOM_FIELD) || (prDecPrm->u1PicStruct == FRAME))
        {
            _vInsertRefPicList(prDecPrm, &prDecPrm->arRefPicList[5], i4CurrPOC,
                               LREF_PIC + (9 << 8) + ((UINT32)i << 16) + u4Temp);
        }
    }
#endif

    vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_FLD, 0);
    vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_FLD, 0);
    
    u4Temp = 0;
    fgDiff = FALSE;
    _vSetupBRefPicList(u4VDecID, prDecPrm->arRefPicList, (VDEC_FBUF_INFO_T *)prDecPrm->arFBufInfo, prDecPrm->u1PicStruct,
                       &u4Temp, 0, 1, &fgDiff);    //B0
    _vSetupBRefPicList(u4VDecID, prDecPrm->arRefPicList, (VDEC_FBUF_INFO_T *)prDecPrm->arFBufInfo, prDecPrm->u1PicStruct,
                       &u4Temp, 4, 5, &fgDiff);    //long term

    if (fgIsFrmPic(prDecPrm->u1PicStruct))
    {
        u4TotalRPIdx = (prDecPrm->arRefPicList[0].u4RefPicCnt < prDecPrm->arRefPicList[1].u4RefPicCnt) ?
                       prDecPrm->arRefPicList[1].u4RefPicCnt : prDecPrm->arRefPicList[0].u4RefPicCnt;
        u4TotalRPIdx += (prDecPrm->arRefPicList[4].u4RefPicCnt < prDecPrm->arRefPicList[5].u4RefPicCnt) ?
                        prDecPrm->arRefPicList[5].u4RefPicCnt : prDecPrm->arRefPicList[4].u4RefPicCnt;
    }
    else
    {
        u4TotalRPIdx = prDecPrm->arRefPicList[0].u4RefPicCnt + prDecPrm->arRefPicList[1].u4RefPicCnt +
                       prDecPrm->arRefPicList[4].u4RefPicCnt + prDecPrm->arRefPicList[5].u4RefPicCnt;
    }
    if (u1ViewIdx > 0)
    {
        u4TotalRPIdx -= 1;  //ignore base view frame
    }

    // in field pic, if B0 & B1 identical, switch the 1st 2 items
    if ((u4TotalRPIdx > 1) && (!fgDiff))
    {
        LOG(9, "!!! Switch 1st items for B0 & B1 !!!!\n");

        if (fgIsFrmPic(prDecPrm->u1PicStruct))
        {
            u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
#ifdef REF_TRACE
            LOG(2, "BList1 RegIdx=0, YAddr=0x%x\n", u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
#endif

            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + 4, u4Temp);
#ifdef REF_TRACE
            LOG(2, "BList1 RegIdx=1, YAddr=0x%x\n", u4Temp);
#endif
#if (defined (VDEC_IS_POST_MT5882))
            u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR);
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4));
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4, u4Temp);
#endif
            u4Temp = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL);
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4));
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 8));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 8, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 12));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 12, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR, u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 8));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 8, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 4);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 4, u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 12));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 12, u4Temp);
        }
        else
        {
            u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
#ifdef REF_TRACE
            LOG(2, "BList1 RegIdx=0, YAddr=0x%x\n", u4VDecReadMC(u4VDecID, RW_MC_B_LIST1 + 4));
#endif

            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1 + 4, u4Temp);
#ifdef REF_TRACE
            LOG(2, "BList1 RegIdx=1, YAddr=0x%x\n", u4Temp);
#endif
#if (defined (VDEC_IS_POST_MT5882))
            u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR);
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4));
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_CADDR + 4, u4Temp);
#endif

            u4Temp = u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL);
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL, u4VDecReadAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4));
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_REORD_B1_RPL + 4, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA, u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA + 4, u4Temp);

            u4Temp = u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR);
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR, u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 4));
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR + 4, u4Temp);

            u4Temp = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD) & 1; // bit 0
            // Write bit 1 to bit 0
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_FLD,  (u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD) & (~1)) | ((u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD)  >> 1) & 1));
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_FLD,  (u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD) & (~2)) | (u4Temp << 1));
        }
    }
}


// *********************************************************************
// Function    : void vSetPicRefType(BYTE bPicStruct, bPicStruct bRefType)
// Description : set pic ref type
// Parameter   : BYTE bPicType: pic struct : FRAME, TOP_FIELD, BOTTOM_FIELD
//                     BYTE bRefType: pic ref type: NREF_PIC, SREF_PIC, LREF_PIC
// Return      : None
// *********************************************************************
static void _vSetPicRefType(VDEC_FBUF_INFO_T* prFBufInfo, UINT8 u1PicStruct, UINT8 u1RefType)
{
    ASSERT(prFBufInfo != NULL);

    if (u1PicStruct & TOP_FIELD)
    {
        prFBufInfo->u1TFldRefType = u1RefType;
    }
    if (u1PicStruct & BOTTOM_FIELD)
    {
        prFBufInfo->u1BFldRefType = u1RefType;
    }
    //if(bPicStruct == FRAME)
    {
        prFBufInfo->u1FBufRefType = u1RefType;
    }
    //else
    {
        //_ptCurrFBufInfo->bFBufRefType = NREF_PIC;
    }
}


// *********************************************************************
// Function    : BYTE bGetPicRefType(BYTE bPicStruct)
// Description : get pic ref type
// Parameter   : BYTE bPicType: pic struct : FRAME, TOP_FIELD, BOTTOM_FIELD
// Return      : BYTE bRefType: pic ref type: NREF_PIC, SREF_PIC, LREF_PIC
// *********************************************************************
static UINT8 _bGetPicRefType(VDEC_FBUF_INFO_T rFBufInfo, UINT8 u1PicStruct)
{
    if (u1PicStruct == TOP_FIELD)
    {
        return rFBufInfo.u1TFldRefType;
    }
    else if (u1PicStruct == BOTTOM_FIELD)
    {
        return rFBufInfo.u1BFldRefType;
    }
    else//if(bPicStruct == FRAME)
    {
        return rFBufInfo.u1FBufRefType;
    }
}


// *********************************************************************
// Function    : void vClrPicRefInfo(BYTE bPicType, BYTE bFBufIdx)
// Description : Clear picture info in frame buffer
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vClrPicRefInfo(VDEC_AVC_DecPrm* prDecPrm, UINT8 u1PicType, UINT16 u2FBufIdx)
{
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = (UINT8)((u2FBufIdx >> 8) & 0xff);
    u2FBufIdx &= 0xFF;

    if ((u2FBufIdx >= 18) || (u1ViewIdx >= MAX_VIEW_NUM))
    {
        LOG(1, "ClrPicRef Idx Err!\n");
        return;
    }
    if (u1PicType & TOP_FIELD)
    {
        prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1TFldRefType = NREF_PIC;
    }
    if (u1PicType & BOTTOM_FIELD)
    {
        prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1BFldRefType = NREF_PIC;
    }
    prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1FBufRefType = NREF_PIC;

    if (fgIsNonRefFBuf(prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1FBufRefType,
                       prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1TFldRefType,
                       prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].u1BFldRefType) &&
            (prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].ucMvBufIdx < 0xff) &&
            (!prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].fgIsInterview))
    {
        if (prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] == (UCHAR)prDecPrm->u4MaxFBufNum)
        {
            LOG(2, "MV buf duplicate\n");
        }
        else
        {
            if (prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] >= MAX_FRAME_NUM)
            {
                ASSERT(prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx] < MAX_FRAME_NUM);
                return;
            }
            prDecPrm->aucEmpMVBufIdxMap[u1ViewIdx][prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx]] =
                prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].ucMvBufIdx;
            //LOG(2, "free mvbufIdx %d, maxNum %d\n", prDecPrm->arFBufInfo[u2FBufIdx].ucMvBufIdx, prDecPrm->ucEmpMVBufMaxNum);

            prDecPrm->ucEmpMVBufMaxNum[u1ViewIdx]++;
            prDecPrm->arFBufInfo[u1ViewIdx][u2FBufIdx].ucMvBufIdx = 0xff;
        }
    }
}


// *********************************************************************
// Function    : void vAdapRefPicmarkingProce(void)
// Description : marking the decoded ref pic with adaptive method
// Parameter   : None
// Return      : None
// *********************************************************************
static void _vAdapRefPicmarkingProce(VDEC_AVC_DecPrm* prDecPrm)
{
    INT32 i4PicNumX;
    UINT32 u4Cnt;
    UINT32 i;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "Adap ViewIdx Err!\n");
        return;
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurFBuf invalid\n");
        return;
    }
    u4Cnt = 0;
    while((u4Cnt < 17) && (prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] != 0))
    {
        switch (prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] & 0xff)
        {
            case 0:    //end loop
                break;
            case 1:    //mark a short-term ref pic as "unused for reference"
                // picNumX
                prDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 =
                    prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 8;
                //if(fgIsFrmPic())
                {
                    i4PicNumX = (prDecPrm->prCurrFBufInfo->i4PicNum -
                                 (INT32)prDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1) - 1;
                }

                for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                {
                    if (fgIsFrmPic(prDecPrm->u1PicStruct) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum == i4PicNumX) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType == SREF_PIC))
                    {
                        _vClrPicRefInfo(prDecPrm, FRAME, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                    else if ((!fgIsFrmPic(prDecPrm->u1PicStruct)) &&
                             (((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC)) ||
                              ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC))))
                    {
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, TOP_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        }
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, BOTTOM_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        }
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                }
                break;
            case 2:    //mark a long term ref pic as "unused for reference"
                prDecPrm->prSliceHdr->u4LongTermPicNum =
                    prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 8;
                i4PicNumX = (INT32)prDecPrm->prSliceHdr->u4LongTermPicNum;
                for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                {
                    if (fgIsFrmPic(prDecPrm->u1PicStruct) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].i4LongTermPicNum == i4PicNumX) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType == LREF_PIC))
                    {
                        _vClrPicRefInfo(prDecPrm, FRAME, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                    else if ((!fgIsFrmPic(prDecPrm->u1PicStruct)) &&
                             (((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldLongTermPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)) ||
                              ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldLongTermPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))))
                    {
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldLongTermPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, TOP_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        }
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldLongTermPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, BOTTOM_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        }
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                }
                break;
            case 3:    //mark a short term ref pic as "used for long-term reference" and assign a long-term index
                prDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 =
                    (prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 8) & 0xff;
                prDecPrm->prSliceHdr->u4LongTermFrameIdx =
                    prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 16;
                i4PicNumX = (INT32)prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                {
                    if (i != prDecPrm->u1DecFBufIdx)
                    {
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus == FRAME) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx == (UINT32)i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType == LREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, FRAME, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            i = prDecPrm->u4MaxFBufNum; // break
                        }
                        else if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != FRAME) &&
                                 (((prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                   (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)) ||
                                  ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                   (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))))
                        {
                            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC))
                            {
                                _vClrPicRefInfo(prDecPrm, TOP_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            }
                            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))
                            {
                                _vClrPicRefInfo(prDecPrm, BOTTOM_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            }
                            i = prDecPrm->u4MaxFBufNum; // break
                        }
                    }
                }
                // picNumX
                //if(fgIsFrmPic())
                {
                    i4PicNumX = (prDecPrm->prCurrFBufInfo->i4PicNum -
                                 (INT32)prDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1) - 1;
                }

                for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                {
                    if (fgIsFrmPic(prDecPrm->u1PicStruct) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].i4PicNum == i4PicNumX) &&
                            (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType == SREF_PIC) &&
                            (!prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting))
                    {
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType = LREF_PIC;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType = LREF_PIC;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType = LREF_PIC;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx =
                            prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx =
                            prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx =
                            prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].i4LongTermPicNum =
                            (INT32)prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldLongTermPicNum =
                            (INT32)prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldLongTermPicNum =
                            (INT32)prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                    else if ((!fgIsFrmPic(prDecPrm->u1PicStruct)) &&
                             (((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC) &&
                               (!prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting)) ||
                              ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldPicNum == i4PicNumX) &&
                               (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC) &&
                               (!prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting))))
                    {
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC) &&
                                (!prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting))
                        {
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType = LREF_PIC;
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx =
                                prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                            prDecPrm->arFBufInfo[u1ViewIdx][i].i4TFldLongTermPicNum =
                                (INT32)((prDecPrm->prSliceHdr->u4LongTermFrameIdx << 1) +
                                        ((prDecPrm->u1PicStruct == TOP_FIELD) ? 1 : 0));

                            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC)
                            {
                                prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType = LREF_PIC;
                                prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx =
                                    prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                                prDecPrm->arFBufInfo[u1ViewIdx][i].i4LongTermPicNum =
                                    (INT32)prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx;
                            }
                        }
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldPicNum == i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC) &&
                                (!prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting))
                        {
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType = LREF_PIC;
                            prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx =
                                prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                            prDecPrm->arFBufInfo[u1ViewIdx][i].i4BFldLongTermPicNum =
                                (INT32)((prDecPrm->prSliceHdr->u4LongTermFrameIdx << 1) +
                                        ((prDecPrm->u1PicStruct == BOTTOM_FIELD) ? 1 : 0));
                            if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)
                            {
                                prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType = LREF_PIC;
                                prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx =
                                    prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                                prDecPrm->arFBufInfo[u1ViewIdx][i].i4LongTermPicNum =
                                    (INT32)prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx;
                            }
                        }
                        i = prDecPrm->u4MaxFBufNum; // break
                    }
                }
                break;
            case 4:    //specify the max long-term index and
                //mark all long-term ref pic having indices greater than the value as "unused for reference"
                prDecPrm->prSliceHdr->u4MaxLongTermFrameIdxPlus1 =
                    prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 8;
                if (prDecPrm->prSliceHdr->u4MaxLongTermFrameIdxPlus1 == 0)
                {
                    prDecPrm->u1MaxLongTermFrameIdx = 0xff;
                    i4PicNumX = 0;
                }
                else
                {
                    i4PicNumX = (INT32)prDecPrm->prSliceHdr->u4MaxLongTermFrameIdxPlus1;
                }
                ASSERT(prDecPrm->u4MaxFBufNum <= 18);
                for (i = 0; i < (prDecPrm->u4MaxFBufNum) && (i < 18); i++)
                {
                    if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx >= (UINT32)i4PicNumX) &&
                            ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC) ||
                             (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC)))
                    {
                        _vClrPicRefInfo(prDecPrm, FRAME, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                        //prDecPrm->arFBufInfo[i].u1TFldRefType = NREF_PIC;
                        //prDecPrm->arFBufInfo[i].u1BFldRefType = NREF_PIC;
                        //prDecPrm->arFBufInfo[i].u1FBufRefType = NREF_PIC;
                    }
                }
                break;
            case 5:    //mark all ref pic as "unused for reference" and set MaxLongTermFrameIdx to "no long-term frame indices"
                prDecPrm->prCurrFBufInfo->u4FrameNum = 0;
                prDecPrm->prCurrFBufInfo->i4PicNum = 0;
                prDecPrm->prCurrFBufInfo->i4TFldPicNum = 0;
                prDecPrm->prCurrFBufInfo->i4BFldPicNum = 0;
                if (prDecPrm->u1PicStruct == TOP_FIELD)
                {
                    prDecPrm->prCurrFBufInfo->i4TFldPOC = 0;
                }
                else if (prDecPrm->u1PicStruct == BOTTOM_FIELD)
                {
                    prDecPrm->prCurrFBufInfo->i4BFldPOC = 0;
                }
                else if (prDecPrm->u1PicStruct == FRAME)
                {
                    prDecPrm->prCurrFBufInfo->i4TFldPOC -= prDecPrm->prCurrFBufInfo->i4POC;
                    prDecPrm->prCurrFBufInfo->i4BFldPOC -= prDecPrm->prCurrFBufInfo->i4POC;
                    prDecPrm->prCurrFBufInfo->i4POC =
                        (prDecPrm->prCurrFBufInfo->i4TFldPOC < prDecPrm->prCurrFBufInfo->i4BFldPOC) ?
                        prDecPrm->prCurrFBufInfo->i4TFldPOC : prDecPrm->prCurrFBufInfo->i4BFldPOC;
                }
                _VDEC_FlushBufRefInfo(prDecPrm);
                break;
            case 6:    //mark current pic as "used for long-term reference" and assign a long term frame index
                prDecPrm->prSliceHdr->u4LongTermFrameIdx =
                    prDecPrm->prSliceHdr->au4MemoryManagementControlOperation[u4Cnt] >> 8;
                i4PicNumX = (INT32)prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                {
                    if (i != prDecPrm->u1DecFBufIdx)
                    {
                        if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus == FRAME) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx == (UINT32)i4PicNumX) &&
                                (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType == LREF_PIC))
                        {
                            _vClrPicRefInfo(prDecPrm, FRAME, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            i = prDecPrm->u4MaxFBufNum; // break
                        }
                        else if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != FRAME) &&
                                 (((prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                   (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC)) ||
                                  ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                   (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))))
                        {
                            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC))
                            {
                                _vClrPicRefInfo(prDecPrm, TOP_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            }
                            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx == (UINT32)i4PicNumX) &&
                                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))
                            {
                                _vClrPicRefInfo(prDecPrm, BOTTOM_FIELD, (UINT16)i + ((UINT16)u1ViewIdx << 8));
                            }
                            i = prDecPrm->u4MaxFBufNum; // break
                        }
                    }
                }

                if (fgIsFrmPic(prDecPrm->u1PicStruct)) // 2 flds decoded
                {
                    prDecPrm->prCurrFBufInfo->u1FBufRefType = LREF_PIC;
                    prDecPrm->prCurrFBufInfo->u1TFldRefType = LREF_PIC;
                    prDecPrm->prCurrFBufInfo->u1BFldRefType = LREF_PIC;
                    prDecPrm->prCurrFBufInfo->u4LongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    prDecPrm->prCurrFBufInfo->u4TFldLongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    prDecPrm->prCurrFBufInfo->u4BFldLongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                }
                else if (prDecPrm->u1PicStruct & TOP_FIELD) // 1 fld decoded
                {
                    prDecPrm->prCurrFBufInfo->u1TFldRefType = LREF_PIC;
                    prDecPrm->prCurrFBufInfo->u4LongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    prDecPrm->prCurrFBufInfo->u4TFldLongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    if (prDecPrm->prCurrFBufInfo->u1BFldRefType == LREF_PIC)
                    {
                        prDecPrm->prCurrFBufInfo->u1FBufRefType = LREF_PIC;
                    }
                }
                else if (prDecPrm->u1PicStruct & BOTTOM_FIELD) // 1 fld decoded
                {
                    prDecPrm->prCurrFBufInfo->u1BFldRefType = LREF_PIC;
                    prDecPrm->prCurrFBufInfo->u4LongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    prDecPrm->prCurrFBufInfo->u4BFldLongTermFrameIdx =
                        prDecPrm->prSliceHdr->u4LongTermFrameIdx;
                    if (prDecPrm->prCurrFBufInfo->u1TFldRefType == LREF_PIC)
                    {
                        prDecPrm->prCurrFBufInfo->u1FBufRefType = LREF_PIC;
                    }
                }
                break;
            default:
                break;
        }
        u4Cnt ++;
    }
}


// *********************************************************************
// Function    : static BOOL _vECFindNeighborFrm(const VDEC_AVC_DecPrm* prDecPrm,
//                                UINT32 u4YAddr, UINT32* pu4Addr)
// Description : fine the neighbor frame buffer for error concealment
// Parameter   : 
// Return      : return true if find
// *********************************************************************
static BOOL _vECFindNeighborFrm(const VDEC_AVC_DecPrm* prDecPrm,
                                UINT32 u4YAddr, UINT32* pu4Addr)
{
    UINT32 u4Idx, u4POCDiff = 0xFFFF, u4Diff = 0;
    UINT32 u4TmpAddr = 0;
    INT32 i4FBufPOC = 0;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);
    ASSERT(pu4Addr != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "EC ViewIdx Err!\n");
        return FALSE;
    }

    for (u4Idx = 0; u4Idx < prDecPrm->u4MaxFBufNum; u4Idx++)
    {
        if ((prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].u1FBufStatus == FRAME) &&
                (prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].u4YAddr == u4YAddr))
        {
            i4FBufPOC = prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].i4POC;
        }
    }

    for (u4Idx = 0; u4Idx < prDecPrm->u4MaxFBufNum; u4Idx++)
    {
        u4TmpAddr = prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].u4YAddr;
        if ((prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].u1FBufStatus == FRAME) &&
                ((u4TmpAddr & FBUF_ERR_MASK) == 0))
        {
            u4Diff = (UINT32)POC_abs(prDecPrm->arFBufInfo[u1ViewIdx][u4Idx].i4POC - i4FBufPOC);
            if (u4Diff < u4POCDiff)
            {
                u4POCDiff = u4Diff;
                *pu4Addr = u4TmpAddr;
            }
        }
    }

    if (u4POCDiff < 0xFFFF)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// *********************************************************************
// Function    : void _VDEC_ECBufSetting(const VDEC_AVC_DecPrm* prDecPrm, 
//                          UINT32 u4RefCnt, UINT32 u4RefEntryCnt, UINT32 u4VDecID)
// Description : error concealment setting
// Parameter   : 
// Return      : None
// *********************************************************************
void _VDEC_ECBufSetting(const VDEC_AVC_DecPrm* prDecPrm, UINT32 u4RefCnt, UINT32 u4RefEntryCnt, UINT32 u4VDecID)
{
    UINT32 u4CurAddr, /*u4NextAddr, */u4NeighborAddr = 0;
    UINT8 u1RegIdx, u1PrevIdx;
    static UINT32 u4P0Addr = 0, u4B0Addr = 0, u4B1Addr = 0, u4MVAddr = 0;
    static UINT32 u4P0Para = 0, u4B0Para = 0, u4B1Para = 0;
    VDEC_FBUF_INFO_T* prCurrFBufInfo = NULL;
    UINT32 u4PList0Reg = 0;

    if (NULL == prDecPrm)
    {
        return;
    }
    if (prDecPrm->prCurrFBufInfo)
    {
        prCurrFBufInfo = prDecPrm->prCurrFBufInfo;
        u4PList0Reg = prCurrFBufInfo->u4PList0Reg;
    }

    //P0 first entry
    u4CurAddr = u4VDecReadMC(u4VDecID, RW_MC_P_LIST0);
    if (u4PList0Reg && (u4PList0Reg != u4CurAddr))
    {
        LOG(9, "[%s-%d]Plist0[SW(%x) != HW(%x)].\n", __func__, __LINE__, u4PList0Reg, u4CurAddr);
        u4CurAddr = u4PList0Reg; //MC_READ32(RW_MC_P_LIST0);
    }
    if ((u4CurAddr & FBUF_ERR_MASK) > 0)
    {
        if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
        {
            vVDecWriteMC(u4VDecID, RW_MC_P_LIST0, u4NeighborAddr);
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_P_LIST0, u4VDecReadMC(u4VDecID, RW_MC_P_LIST0) & 0x0FFFFFFF);
        }
        u4PList0Reg = u4VDecReadMC(u4VDecID, RW_MC_P_LIST0);
    }
    else if (u4CurAddr == 0) //for I frame error concealment
    {
        vVDecWriteMC(u4VDecID, RW_MC_P_LIST0, u4P0Addr);
        u4PList0Reg = u4P0Addr;
        vVDecWriteAVCMV(u4VDecID, RW_AMV_P_REF_PARA, u4P0Para);
        if (u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR) == 0)
        {
            vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR, u4MVAddr);
        }
    }
    //B0 first entry
    u4CurAddr = u4VDecReadMC(u4VDecID, RW_MC_B_LIST0);
    if ((u4CurAddr & FBUF_ERR_MASK) > 0)
    {
        if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
        {
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST0, u4NeighborAddr);
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST0, u4VDecReadMC(u4VDecID, RW_MC_B_LIST0) & 0x0FFFFFFF);
        }
    }
    else if (u4CurAddr == 0)
    {
        vVDecWriteMC(u4VDecID, RW_MC_B_LIST0, u4B0Addr);
        vVDecWriteAVCMV(u4VDecID, RW_AMV_B0_REF_PARA, u4B0Para);
    }
    //B1 first entry
    u4CurAddr = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
    if ((u4CurAddr & FBUF_ERR_MASK) > 0)
    {
        if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
        {
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4NeighborAddr);
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1) & 0x0FFFFFFF);
        }
    }
    else if (u4CurAddr == 0)
    {
        vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4B1Addr);
        vVDecWriteAVCMV(u4VDecID, RW_AMV_B1_REF_PARA, u4B1Para);
    }

    //------- frame gap handling, ref frame may missing, need to set reg (default=0)
    for (u1RegIdx = 1; u1RegIdx < (UINT8)u4RefEntryCnt; u1RegIdx++)
    {
        u1PrevIdx = (UINT8)(u1RegIdx - 1);
        //p list
        u4CurAddr = u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1RegIdx << 2)));
        if (u4CurAddr == 0)
        {
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1RegIdx << 2)), u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_P_LIST0 + (u1PrevIdx << 2))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx << 3)),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1PrevIdx << 3))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1RegIdx << 3) + 4),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_P_REF_PARA + (u1PrevIdx << 3) + 4)));
            if (prDecPrm->prSliceHdr &&
                    prDecPrm->prSliceHdr->fgFieldPicFlag && prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                vVDecWriteMC(u4VDecID, RW_MC_P_LIST0_FLD,
                           (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_P_LIST0_FLD) | (0x1 << u1RegIdx)));
            }
        }
        else if ((u4CurAddr & FBUF_ERR_MASK) > 0)
        {
            if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
            {
                vVDecWriteMC(u4VDecID, RW_MC_P_LIST0, u4NeighborAddr);
            }
            else
            {
                vVDecWriteMC(u4VDecID, RW_MC_P_LIST0, u4VDecReadMC(u4VDecID, RW_MC_P_LIST0) & 0x0FFFFFFF);
            }
            u4PList0Reg = u4VDecReadMC(u4VDecID, RW_MC_P_LIST0);
        }

        //b list 0
        u4CurAddr = u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1RegIdx << 2)));
        if (u4CurAddr == 0)
        {
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1RegIdx << 2)), u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_B_LIST0 + (u1PrevIdx << 2))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx << 3)),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1PrevIdx << 3))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1RegIdx << 3) + 4),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_B0_REF_PARA + (u1PrevIdx << 3) + 4)));
            if (prDecPrm->prSliceHdr &&
                    prDecPrm->prSliceHdr->fgFieldPicFlag && prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST0_FLD,
                           (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_B_LIST0_FLD) | (0x1 << u1RegIdx)));
            }
        }
        else if ((u4CurAddr & FBUF_ERR_MASK) > 0)
        {
            if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST0, u4NeighborAddr);
            }
            else
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST0, u4VDecReadMC(u4VDecID, RW_MC_B_LIST0) & 0x0FFFFFFF);
            }
        }
        //b list 1
        u4CurAddr = u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1RegIdx << 2)));
        if (u4CurAddr == 0)
        {
            vVDecWriteMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1RegIdx << 2)), u4VDecReadMC(u4VDecID, (UINT32)(RW_MC_B_LIST1 + (u1PrevIdx << 2))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx << 3)),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1PrevIdx << 3))));
            vVDecWriteAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1RegIdx << 3) + 4),
                           u4VDecReadAVCMV(u4VDecID, (UINT32)(RW_AMV_B1_REF_PARA + (u1PrevIdx << 3) + 4)));
            if (prDecPrm->prSliceHdr &&
                    prDecPrm->prSliceHdr->fgFieldPicFlag && prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST1_FLD,
                           (UINT32)(u4VDecReadMC(u4VDecID, RW_MC_B_LIST1_FLD) | (0x1 << u1RegIdx)));
            }
        }
        else if ((u4CurAddr & FBUF_ERR_MASK) > 0)
        {
            if (_vECFindNeighborFrm(prDecPrm, u4CurAddr, &u4NeighborAddr))
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4NeighborAddr);
            }
            else
            {
                vVDecWriteMC(u4VDecID, RW_MC_B_LIST1, u4VDecReadMC(u4VDecID, RW_MC_B_LIST1) & 0x0FFFFFFF);
            }
        }
    }

    //save last info for I frame error reconcealment
    u4P0Addr = u4PList0Reg; //u4VDecReadMC(u4VDecID, RW_MC_P_LIST0);
    u4B0Addr = u4VDecReadMC(u4VDecID, RW_MC_B_LIST0);
    u4B1Addr = u4VDecReadMC(u4VDecID, RW_MC_B_LIST1);
    u4MVAddr = u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_ADDR);
    u4P0Para = u4VDecReadAVCMV(u4VDecID, RW_AMV_P_REF_PARA);
    u4B0Para = u4VDecReadAVCMV(u4VDecID, RW_AMV_B0_REF_PARA);
    u4B1Para = u4VDecReadAVCMV(u4VDecID, RW_AMV_B1_REF_PARA);
    if (prCurrFBufInfo)
    {
        prCurrFBufInfo->u4PList0Reg = u4PList0Reg;//store
    }
    UNUSED(u4RefCnt);
}


#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
// **************************************************************************
// Function : void _VDEC_MVCRegSetting(UINT32 u4VDecID, const VDEC_AVC_DecPrm* prDecPrm);
// Description : set register for mvc decoding
// Parameter : u4VDecID : hw id
// Return      : none
// **************************************************************************
void _VDEC_MVCRegSetting(UINT32 u4VDecID, const VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 u4Idx, u4RefNum, u4LoadCntL0, u4LoadCntL1;
    UINT32* au4RefList;

    if (prDecPrm->fgIsPrefixLen32Bit)
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV,
                        (u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) | MVC_SWITCH | REORDER_MVC_SWITCH |
                        HDR_EXTENTION_SWITCH | (prDecPrm->fgIsPrefixLen32Bit << 15)));
    }
    else
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_EMU_PREV,
                        ((u4VDecReadAVCVLD(u4VDecID, RW_AVLD_EMU_PREV) | MVC_SWITCH | REORDER_MVC_SWITCH |
                         HDR_EXTENTION_SWITCH) & 0xFFFF7FFF));
    }

    if (prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "_VDEC_MVCRegSetting ViewIdx Err!\n");
        return;
    }
    //L0
    u4RefNum = prDecPrm->fgAnchorPic ?
               prDecPrm->prSPS->au1NumAnchorRefL0[prDecPrm->u1CurViewIdx] :
               prDecPrm->prSPS->au1NumNonAnchorRefL0[prDecPrm->u1CurViewIdx];

    ASSERT(u4RefNum < 16);
    if (u4RefNum >= 16)
    {
        LOG(1, "_VDEC_MVCRegSetting u4RefNum Err!\n");
        return;
    }
    u4LoadCntL0 = (u4RefNum >> 1) + (u4RefNum & 1);

    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ANCHOR_L0_NUM, u4RefNum);

    au4RefList = prDecPrm->fgAnchorPic ?
                 prDecPrm->prSPS->au4AnchorRefL0[prDecPrm->u1CurViewIdx] :
                 prDecPrm->prSPS->au4NonAnchorRefL0[prDecPrm->u1CurViewIdx];

    for (u4Idx = 0; u4Idx < 16; u4Idx++)
    {
        if (u4Idx < u4RefNum)
        {
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ANCHOR_L0 + (u4Idx << 2), au4RefList[u4Idx]);
        }    
    }
    //L1
    u4RefNum = prDecPrm->fgAnchorPic ?
               prDecPrm->prSPS->au1NumAnchorRefL1[prDecPrm->u1CurViewIdx] :
               prDecPrm->prSPS->au1NumNonAnchorRefL1[prDecPrm->u1CurViewIdx];

    ASSERT(u4RefNum < 16);
    if (u4RefNum >= 16)
    {
        LOG(1, "_VDEC_MVCRegSetting u4RefNum Err!\n");
        return;
    }
    u4LoadCntL1 = (u4RefNum >> 1) + (u4RefNum & 1);

    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ANCHOR_L1_NUM, u4RefNum);

    au4RefList = prDecPrm->fgAnchorPic ?
                 prDecPrm->prSPS->au4AnchorRefL1[prDecPrm->u1CurViewIdx] :
                 prDecPrm->prSPS->au4NonAnchorRefL1[prDecPrm->u1CurViewIdx];

    for (u4Idx = 0; u4Idx < 16; u4Idx++)
    {
        if (u4Idx < u4RefNum)
        {
            vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ANCHOR_L1 + (u4Idx << 2), au4RefList[u4Idx]);
        }    
    }

    //MVC Misc
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_ANCHOR_MISC,
                    (MVC_LOAD_ANCHOR | (u4LoadCntL1 << 8) | u4LoadCntL0));
}
#endif


// **************************************************************************
// Function : void vInitVDecHW(DWRD dwRDPtrAddr);
// Description : Init Video Decoder
// Parameter : None
// Return      : TRUE: init OK.
//                   FALSE: init failed
// **************************************************************************
void _VDEC_InitVDecHW(BOOL bIsInit, BOOL fgIsUFOMode, BOOL fgIsDualCoreMode, UINT32 u4VDecID)
{   
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]InitVDecHW Start\n");
#endif
    
#ifndef AVC_IRQ_DISABLE
    _VDEC_InitIsr();
#endif

    if (bIsInit)
    {
        _VDEC_VLDHWReset(FALSE, FALSE, u4VDecID);
#ifdef BARREL2_SUPPORT
        _VDEC_VLDHWReset2(u4VDecID, bIsInit);
#endif
    }    
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264]InitVDecHW Done\n");
#endif
}


// *********************************************************************
// Function : void _VDEC_SetBsInfo
// Description : Set Bs fifo start addr, end addr, and current read ptr
// Parameter : u4RDPtrAddr, u4BsFifoS, u4BsFifoE
// Return    : None
// *********************************************************************
void _VDEC_SetBsInfo(UINT8 u1Num, UCHAR ucVDecID, UINT32 u4RDPtrAddr, UINT32 u4WPtrAddr, VDEC_H264_INFO_T* prVdec)
{
    BOOL fgIsUFOMode = FALSE;
    BOOL fgIsDualCoreMode = FALSE;
#ifdef WAIT_SRAM_STABLE
    UINT32 u4Cnt = 0;
#endif 
    // Set for VLD boundary check 0x5000000
#ifdef BARREL2_SUPPORT
    if (u1Num == BARREL2)
    {
        _VDEC_VLDHWReset2(FALSE);
        while (!_VDEC_ResetVLD2(PHYSICAL(u4RDPtrAddr), PHYSICAL(u4WPtrAddr),
                                PHYSICAL(prVdec->rPesInfo.u4FifoStart), PHYSICAL(prVdec->rPesInfo.u4FifoEnd)))
        {
            _VDEC_VLDHWReset2(TRUE);
        }
    }
    else
#else
    UNUSED(u1Num);
#endif
    {
#ifdef WAIT_SRAM_STABLE
        if ((DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode) && (VDEC_VLD0 == ucVDecID))
        {
            vVDecResetMultiCore((UINT32)ucVDecID);
        }
        if (DUAL_CORE_MODE == prVdec->rDecParam.ucDualCoreMode)
        {
            fgIsDualCoreMode = TRUE;
        }
        if (prVdec->rDecParam.fgIsUFOMode)
        {
            fgIsUFOMode = TRUE;
        }
        _VDEC_VLDHWReset(fgIsUFOMode, fgIsDualCoreMode, (UINT32)ucVDecID);
        while (!_VDEC_ResetVLD1((UINT32)ucVDecID, PHYSICAL(u4RDPtrAddr), PHYSICAL(u4WPtrAddr), prVdec))
        {
            _VDEC_VLDHWReset(fgIsUFOMode, fgIsDualCoreMode, (UINT32)ucVDecID);
            if(u4Cnt++ > 0x10)
            {
                LOG(2, "SwRst %d\n", u4Cnt);
                break;
            }
        }
        UNUSED(u4Cnt);
#else
        _VDEC_VLDSetVFifo(PHYSICAL(prVdec->rPesInfo.u4FifoStart), PHYSICAL(prVdec->rPesInfo.u4FifoEnd));

        if (!_VDEC_VLDInitBarrelShifter((UINT32)ucVDecID, u4RDPtrAddr))
        {
            LOG(2, "Barrel Shifter1 Init failed");
        }
#endif
    }
}


// **************************************************************************
// Function : void _VDEC_ClrFBufInfo(VDEC_AVC_DecPrm *prDecPrm, UINT32 u4FBufIdx);
// Description : clear framebuffer info
// Parameter : u4FBufIdx : framebuffer id
// Return      : none
// **************************************************************************
void _VDEC_ClrFBufInfo(VDEC_AVC_DecPrm *prDecPrm, UINT32 u4FBufIdx)
{
    VDEC_FBUF_INFO_T* prFBufInfo;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = u4FBufIdx >> 8;
    u4FBufIdx &= 0xFF;

    if ((u4FBufIdx >= 18) || (u1ViewIdx >= MAX_VIEW_NUM))
    {
        LOG(1, "ClrFBuf Idx Err!\n");
        return;
    }
    prFBufInfo = &prDecPrm->arFBufInfo[u1ViewIdx][u4FBufIdx];

    LOG(7,"c i(%d),p(%d),fb(%d)\n",u4FBufIdx,prFBufInfo->i4POC,prFBufInfo->ucFbId);
    prFBufInfo->fgNonExisting = FALSE;
    prFBufInfo->fgIsBufRef = FALSE;
    prFBufInfo->fgIsErr = FALSE;
    prFBufInfo->fgIsNotDisp = FALSE;
    prFBufInfo->fgFirstFrmTag = FALSE;

    prFBufInfo->u1FBufStatus = NO_PIC;
    prFBufInfo->fgIsInterview = FALSE;

    _vClrPicRefInfo(prDecPrm, FRAME, (UINT8)u4FBufIdx + ((UINT16)u1ViewIdx << 8));
    //prFBufInfo->u1BFldRefType = NREF_PIC;
    //prFBufInfo->u1FBufRefType = NREF_PIC;
    //prFBufInfo->u1TFldRefType = NREF_PIC;

    prFBufInfo->u4FrameNum = (UINT32)0xffffffff;
    prFBufInfo->i4FrameNumWrap = (INT32)0xefffffff;
    prFBufInfo->i4PicNum = (INT32)0xefffffff;
    prFBufInfo->i4TFldPicNum = (INT32)0xefffffff;
    prFBufInfo->i4BFldPicNum = (INT32)0xefffffff;
    prFBufInfo->u4LongTermFrameIdx = (UINT32)0xff;
    prFBufInfo->u4TFldLongTermFrameIdx = (UINT32)0xff;
    prFBufInfo->u4BFldLongTermFrameIdx = (UINT32)0xff;
    prFBufInfo->i4LongTermPicNum = (INT32)0xefffffff;
    prFBufInfo->i4TFldLongTermPicNum = (INT32) 0xefffffff;
    prFBufInfo->i4BFldLongTermPicNum = (INT32)0xefffffff;

    prFBufInfo->u4TFldPara = 0;
    prFBufInfo->u4BFldPara = 0;

    prFBufInfo->i4POC = (INT32)0x7fffffff;
    prFBufInfo->i4TFldPOC = (INT32)0x7fffffff;
    prFBufInfo->i4BFldPOC = (INT32)0x7fffffff;

    prFBufInfo->u4UserDataFrmWptr = 0;
    prFBufInfo->u4UserDataPESWptr = 0;
    prFBufInfo->u4SkipFrmCnt = 0;
#ifdef FBM_DPB_CTRL
    if (REAL_DECODE == prDecPrm->ucVDecStatus)
    {
        FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prFBufInfo->ucFbId, FALSE);
    }
#endif
    prFBufInfo->ucFbId = 0xFF;
}


// *********************************************************************
// Function    : void vFlushDPB(tMpvDecPrm)
// Description : Flush pictures in DPB
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_FlushDPB(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 i, FbCnt;
    INT32 i4MinPOC;
    UINT32 u4MinPOCFBufIdx, u4YAddr, u4CAddr, u4SaveInfoIdx = 0xff;
    //VDEC_DISP_PIC_INFO_T rDispInfo;
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    STC_CLOCK rStcClk;
#ifdef FBM_DPB_CTRL
    FBM_PIC_HDR_T * prFbmPicHdr;
    FBM_PIC_HDR_T * prFbmPicHdr1;
#endif
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    MPV_EXT_INFO_T *prExtInfo;
    MPV_USERDATA_CB_T rUsrData;
    UINT8 u1ViewIdx;
#ifdef CC_SEI_USER_DATA_ENABLE
    MPV_CC_T *prMpvCc;
    CC_INFO_T *prCcInfo;
    INT32 i4Ret;
#endif

    ASSERT(prDecPrm != NULL);
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);

    prExtInfo = (MPV_EXT_INFO_T*)_MPV_GetExtInfo();

    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));
    if (prDecPrm->ucEsId < MAX_ES_NUM)
    {
        prInfo = &(_prDbase->rMpv[prDecPrm->ucEsId]);
    }
    else
    {
        LOG(1, "ucEsId Err!\n");
        return;
    }
    ASSERT(prDecPrm != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "Flush ViewIdx Err!\n");
        return;
    }

#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE) && defined(CC_ALLEGRO_MVC)
    if ((!prDecPrm->fgNalNonIdr) && (u1ViewIdx > 0))
    {
        u4SaveInfoIdx = prDecPrm->u1DecFBufIdx;
    }
#endif

    //do
    for (FbCnt = 0; FbCnt < prDecPrm->u4MaxFBufNum; FbCnt++)
    {
        // Check if DPB full
        i4MinPOC = 0x7fffffff;
        u4MinPOCFBufIdx = 0xff;
        for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
        {
            if ((i4MinPOC > prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC) &&
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != NO_PIC) &&
                    (i != u4SaveInfoIdx))
            {
                i4MinPOC = prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC;
                u4MinPOCFBufIdx = i;
            }
        }
        if (u4MinPOCFBufIdx != 0xff)
        {
#ifdef FBM_DPB_CTRL
            //calculate pts in decoder if AVI file
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
            if (prFbmPicHdr == NULL)
            {
                LOG(2, "prFbmPicHdr NULL in FlushDpb\n");
                break;
            }


            if (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId)
                    == FBM_FB_STATUS_READY)
            {

                if (prDecPrm->fgIsMMPlay)
                {
                    if (!prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgPTSValid)
                    {
                        prFbmPicHdr->u4PTS = prDecPrm->au4DispPts[u1ViewIdx];
                        prFbmPicHdr->u8PTS = prDecPrm->au8DispPts[u1ViewIdx];
                    }
                    else
                    {
                        prDecPrm->au4DispPts[u1ViewIdx] = prFbmPicHdr->u4PTS;
                        prDecPrm->au8DispPts[u1ViewIdx] = prFbmPicHdr->u8PTS;
                    }
                    if (prDecPrm->u4Rate > 0)
                    {
                        prDecPrm->au4DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale + prDecPrm->u8VRemainder),
                            prDecPrm->u4Rate, &prDecPrm->u8VRemainder) * (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4SkipFrmCnt + 1));
                        // u8Pts
                    }
                    if (prDecPrm->u4Rate64 > 0)
                    {
                        prDecPrm->au8DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale64 + prDecPrm->u8VRemainder64),
                            prDecPrm->u4Rate64, &prDecPrm->u8VRemainder64) * (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4SkipFrmCnt + 1));
                    }
                }

                FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, FALSE);

                if ((prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus != FRAME) ||
                        (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgIsNotDisp))  //field empty
                {
                    if (prDecPrm->fgIsMVC && (prDecPrm->ucSkipMode == IMode) && !prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgIsNotDisp)
                    {
                        if (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus == TOP_FIELD)
                        {
                            prFbmPicHdr->fgTrickTopField = TRUE;
                        }
                        else
                        {
                            prFbmPicHdr->fgTrickTopField = FALSE;
                        }
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                                 FBM_FB_STATUS_DISPLAYQ);
                    }
                    else if(prVdecEsInfo->fgRVUSkip)
                    {
                        {
                            LOG(6, "FlushQ PTS(0x%x), POC %d, ViewID %d, FbId %d\n",
                                prFbmPicHdr->u4PTS, 
                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].i4POC, prDecPrm->u1CurViewIdx, 
                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                        }
                        
                        #ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId, 
                            prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                        #endif
                        
                        _VDEC_JobBeforePutDispQ(prDecPrm->ucEsId,
                            prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                            
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, 
                                FBM_FB_STATUS_DISPLAYQ);
                                
                        prVdecEsInfo->fgRVUSkip = FALSE;
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                                 FBM_FB_STATUS_EMPTY);
                        prInfo->u4VpsrDropNs++;
                    }
                }
                else
                {
                    if(!prDecPrm->fgStartPlay || !prVdecEsInfo->fgSetStartPts)
                    {
                        if(prDecPrm->fgIsMMPlay)
                        {
                            prDecPrm->fgStartPlay = TRUE;
                            //prVdecEsInfo->fgSetStartPts = TRUE;
                        }
                        else
                        {
                            if(_bNotifyAudStartPlay(prDecPrm->ucStcId, prFbmPicHdr->u4PTS))
                            {
                                LOG(4,"Set start PTS 0x%x\n",prFbmPicHdr->u4PTS);
                                prDecPrm->fgStartPlay = TRUE;
                                prVdecEsInfo->fgSetStartPts = TRUE;
                            }
                        }
                    }
                    // mm checking
                    if (prDecPrm->fgIsMMPlay && VDEC_IsNotDisplay(/*prDecPrm->ucEsId*/
                                prVdecEsInfoKeep->rPesInfo.ucEsId, prFbmPicHdr->u4PTS,
                                prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset))
                    {
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                                 FBM_FB_STATUS_EMPTY);
                    }
                    else
                    {
                        #ifdef ENABLE_MULTIMEDIA
                        if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (prDecPrm->ucSkipMode == IMode))
                        {
                            prFbmPicHdr->fgPushModeTrick = TRUE;
                        }                    
                        if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (TRUE == prVdecEsInfoKeep->rPesInfo.fgBandwidthChanged))
                        {
                            prFbmPicHdr->fgBandwidthChanged = TRUE;
                            LOG(0, "Flush Set FbmPicHdr BandwidthChanged TRUE\n");
                        } 
                        if (_VDEC_IsxvYCC(prDecPrm))
                        {
                            prFbmPicHdr->eColorSpace = ColorSpace_XVColor;
                        }
                        else
                        {
                            prFbmPicHdr->eColorSpace = ColorSpace_Normal;
                        }
                        #endif
                        STC_GetSrc(0, &rStcClk);
                        {
                            LOG(6, "FlushQ PTS(0x%x), STC(0x%x), POC %d, ViewID %d, FbId %d\n",
                                prFbmPicHdr->u4PTS, rStcClk.u4Base,
                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].i4POC, prDecPrm->u1CurViewIdx,
                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                            #if BDP_LOG_STYLE 
                            VDecPrintf(prVdecEsInfoKeep->rPesInfo.ucEsId, VDEC_LOG_VDEC2VDP, 
                                             "[H264 VDEC-VDP%d] FlushQ PType=0X%08X, PTS(0x%x), STC(0x%x), POC %d, ViewID %d, FbId %d\n",  \
                                                prVdecEsInfoKeep->rPesInfo.ucEsId,
                                                prVdecEsInfoKeep->rPesInfo.ucPicType,                         
                                               prFbmPicHdr->u4PTS,
                                               rStcClk.u4Base,
                                               prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].i4POC,
                                               prDecPrm->u1CurViewIdx,
                                            prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                            #endif
                        }
#ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId,
                                              prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
#endif
                        _VDEC_JobBeforePutDispQ(prDecPrm->ucEsId,
                                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
                        if (prDecPrm->fgEos && prDecPrm->fgIsMVC)
                        {
                            FBM_SetFrameBufferPicFlag(prVdecEsInfo->ucFbgId,
                                                      prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, FBM_MM_EOS_FLAG);
                        }
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                                 FBM_FB_STATUS_DISPLAYQ);
                        prVdecEsInfo->u4DisplayQPicCnt++;
                        prVdecEsInfo->fgRVUSkip = FALSE;
                         
                        if (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgFirstFrmTag)  //continuous display from 1st frame
                        {
                            for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                            {
                                if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != NO_PIC)
                                {
                                    prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsNotDisp = FALSE;
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][i].ucFbId);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        printf("NO_PIC %x\n", prFbmPicHdr1->u4PTS);
                                    }
                                }
                            }
                            for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
                            {
                                if (prDecPrm->arFBufInfo[1-u1ViewIdx][i].u1FBufStatus != NO_PIC &&
                                        (prDecPrm->arFBufInfo[1-u1ViewIdx][i].i4POC >= prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].i4POC))
                                {
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[1-u1ViewIdx], prDecPrm->arFBufInfo[1-u1ViewIdx][i].ucFbId);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        printf("NO_PIC %x\n", prFbmPicHdr1->u4PTS);
                                    }
                                    prDecPrm->arFBufInfo[1-u1ViewIdx][i].fgIsNotDisp = FALSE;
                                }
                            }
                        }
                        prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].fgFirstFrmTag = FALSE;

                        //notify userdata
#ifdef CC_SEI_USER_DATA_ENABLE
                        prMpvCc = (MPV_CC_T*)_MPV_GetMpvCc(prDecPrm->ucEsId);
                        if (prMpvCc->fgPlayCc)
                        {
                            prCcInfo = &(prMpvCc->rCcInfo);

                            prCcInfo->fgIsScte = FALSE;
                            prCcInfo->fgBottomFirst = prFbmPicHdr->fgTopFldFirst;
                            prCcInfo->u4Size = prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr;
                            prCcInfo->u8Pts = (UINT64)prFbmPicHdr->u4PTS;
                            prCcInfo->u4BufRptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].au1UsrDataFrm,
                                                  prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr);
                            if (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr > 0)
                            {
                                i4Ret = x_msg_q_send(prMpvCc->hCcQueue,
                                                     (void *)prCcInfo, sizeof(CC_INFO_T), 255);

                                VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TOO_MANY));

                                if ((prMpvCc->pfCcNfyFct) && (i4Ret == OSR_OK))
                                {
                                    LOG(4, "atsc Notify Cc ptx 0x%x, size %d\n", (UINT32)prCcInfo->u8Pts, prCcInfo->u4Size);
                                    prMpvCc->pfCcNfyFct(prMpvCc->pvCcNfyTag,
                                                        MPV_CC_COND_DATA_ARRIVAL, (void*)(prCcInfo->u4Size));
                                }
                            }
                        }
#endif

                        if (prExtInfo->pfUserDataCb &&
                                ((prExtInfo->fgUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr > 0)) ||
                                 (prExtInfo->fgPesUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataPESWptr > 0))))
                        {
                            rUsrData.fgProgressiveFrm = prFbmPicHdr->fgProgressiveFrm;
                            rUsrData.u2TemporalRef = (UINT16)prFbmPicHdr->i4TemporalRef;
                            rUsrData.u4Pts = prFbmPicHdr->u4PTS;
                            rUsrData.ucEsId = prDecPrm->ucEsId;
                            rUsrData.ucPicStruct = prFbmPicHdr->ucPicStruct;
                            rUsrData.ucPicType = prFbmPicHdr->ucPicCdTp;
                            rUsrData.ucRepFirstFld = prFbmPicHdr->fgRepFirstFld;
                            rUsrData.ucTopFldFirst = prFbmPicHdr->fgTopFldFirst;

#ifdef CC_SEI_USER_DATA_ENABLE
                            if (prExtInfo->fgUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr > 0))
                            {
                                rUsrData.u4Rptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                    prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].au1UsrDataFrm,
                                                                     prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr);
                                rUsrData.ucDataLen = (UCHAR)prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataFrmWptr;
                                prExtInfo->pfUserDataCb(&rUsrData);
                            }
#endif
#ifdef CC_DMX_PES_EXT_BUFFER
                            if (prExtInfo->fgPesUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataPESWptr > 0))
                            {
                                rUsrData.u4Rptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                    prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].au1UsrDataPES,
                                                                     prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataPESWptr);
                                rUsrData.ucDataLen = (UCHAR)prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u4UserDataPESWptr;
                                prExtInfo->pfUserDataCb(&rUsrData);
                            }
#endif
                            UNUSED(rUsrData);
                        }
                    }
                    if (prDecPrm->fgIsMMPlay)
                    {
                        prVdecEsInfo->u4PTS = prFbmPicHdr->u4PTS;
                        UNUSED(VDEC_CheckStopStatus(/*prDecPrm->ucEsId*/
                                   prVdecEsInfoKeep->rPesInfo.ucEsId, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));
                    }
                    prDecPrm->i4LastPOC[u1ViewIdx] = i4MinPOC;
                }
                FBM_GetFrameBufferAddr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                       &u4YAddr, &u4CAddr);
            }
            else if (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId)
                     == FBM_FB_STATUS_DECODE)
            {
                //if((prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus != FRAME))
                {
                    //FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, FALSE);
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                             FBM_FB_STATUS_EMPTY);
                    prInfo->u4VpsrDropNs++;
                }
                //else
                //{
                //    u4SaveInfoIdx = u4MinPOCFBufIdx;
                //    LOG(2, "SaveInfo in FlushDPB\n");
                //}
            }
#endif
            _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + ((UINT32)u1ViewIdx << 8));
        }
    }
    //while(u4MinPOCFBufIdx != 0xff);
}


// **************************************************************************
// Function : void _VDEC_ClrDPB(VDEC_AVC_DecPrm *prDecPrm);
// Description : clear frame info in dpb buffer 
// Parameter : 
// Return      : none
// **************************************************************************
void _VDEC_ClrDPB(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 FbCnt;
    UCHAR ucFbStatus;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "ClrDPB ViewIdx Err!\n");
        return;
    }

    //do
    ASSERT(prDecPrm->u4MaxFBufNum <= 18);
    for (FbCnt = 0; (FbCnt < prDecPrm->u4MaxFBufNum) && (FbCnt < 18); FbCnt++)
    {
#ifdef FBM_DPB_CTRL
        ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][FbCnt].ucFbId);
        if ((ucFbStatus == FBM_FB_STATUS_READY) || (ucFbStatus == FBM_FB_STATUS_DECODE))
        {
            FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][FbCnt].ucFbId, FALSE);

            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][FbCnt].ucFbId,
                                     FBM_FB_STATUS_EMPTY);
            _VDEC_ClrFBufInfo(prDecPrm, FbCnt + ((UINT32)u1ViewIdx<<8));
        }
        else
        {
            FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][FbCnt].ucFbId, FALSE);
            _VDEC_ClrFBufInfo(prDecPrm, FbCnt + ((UINT32)u1ViewIdx << 8));
        }
#endif
    }

#if defined(CC_USE_DDI)
    prDecPrm->au4DispPts[u1ViewIdx] = 0;
    prDecPrm->au8DispPts[u1ViewIdx] = 0;
#endif
    prDecPrm->i4LastPOC[u1ViewIdx] = -99999;
    prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum = 0xffffffff;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastFrameNumOffset = 0;
    prDecPrm->rLastInfo[u1ViewIdx].fgLastIsRef = FALSE;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastRefTFldPOC = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastRefBFldPOC = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOC = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCLsb = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCMsb = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastTFldPOC = 0;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastBFldPOC = 0;
    prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5 = FALSE;
    LOG(2, "ClrDPB %d done.\n", u1ViewIdx);
    LOG(2, "ClrDPB done.\n");
}


// **************************************************************************
// Function : void _VDEC_SetPicInfo(UCHAR ucEsId, UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm);
// Description : set decode info to vdec hw or b2r
// Parameter : ucEsId : instance id, u4VDecID: hw id
// Return      : none
// **************************************************************************
void _VDEC_SetPicInfo(UCHAR ucEsId, UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 u4SPSInfo, u4PPSInfo1, u4PPSInfo2, u4SHDRInfo, u4Idx;

    ASSERT(prDecPrm != NULL);
    #ifdef CC_DUAL_CORE_DEC
    if (LAE_DECODE == prDecPrm->ucVDecStatus)
    {        
        vLAEPrepareFBufInfo(prDecPrm);         
        _VDEC_SetCurrPOC(prDecPrm);
    }
    #endif
    if (VDEC_VLD0 == u4VDecID)
    {
        //prepare reference picture list
        _vPrepareFBufInfo(prDecPrm);
        if (prDecPrm->fgAllocFbFail || prDecPrm->fgNotSupport)
        {
            return;
        }
        _VDEC_SetCurrPOC(prDecPrm);
        if (prDecPrm->fgAllocFbFail)
        {
            return;
        }
    }    
    if (VDEC_VLD1 == u4VDecID && (SINGLE_CORE_MODE == prDecPrm->ucDualCoreMode))
    {
        //prepare reference picture list
        _vPrepareFBufInfo(prDecPrm);
        if (prDecPrm->fgAllocFbFail || prDecPrm->fgNotSupport)
        {
            return;
        }
        _VDEC_SetCurrPOC(prDecPrm);
        if (prDecPrm->fgAllocFbFail)
        {
            return;
        }
    }
#if (defined (CC_MT5890)) //CHB access issue, init ref list at channel B address, avoid decode timeout 
    for (u4Idx = 0; u4Idx < 32; u4Idx++)
    {
        vVDecWriteMC(u4VDecID, (RW_MC_P_LIST0 + (u4Idx << 2)), ((UINT32)prDecPrm->prCurrFBufInfo->u4YAddr));
        vVDecWriteMC(u4VDecID, (RW_MC_P_LIST0_CADDR + (u4Idx << 2)), ((UINT32)prDecPrm->prCurrFBufInfo->u4CAddr));        
        vVDecWriteMC(u4VDecID, (RW_MC_B_LIST0 + (u4Idx << 2)), ((UINT32)prDecPrm->prCurrFBufInfo->u4YAddr));
        vVDecWriteMC(u4VDecID, (RW_MC_B_LIST0_CADDR + (u4Idx << 2)), ((UINT32)prDecPrm->prCurrFBufInfo->u4CAddr));       
        vVDecWriteMC(u4VDecID, (RW_MC_B_LIST1 + (u4Idx << 2)),PHYSICAL((UINT32)prDecPrm->prCurrFBufInfo->u4YAddr));            
        vVDecWriteMC(u4VDecID, (RW_MC_B_LIST1_CADDR + (u4Idx << 2)),PHYSICAL((UINT32)prDecPrm->prCurrFBufInfo->u4CAddr));
    }
#endif
    _vSetPRefPicList(u4VDecID, prDecPrm);
    _vSetBRefPicList(u4VDecID, prDecPrm);

    _vAssignQuantParam(u4VDecID, prDecPrm);
    //SPS data prepare ===========================================
    u4SPSInfo = (prDecPrm->prSPS->u4ChromaFormatIdc & 0x3); // 1~0
    u4SPSInfo |= ((prDecPrm->prSPS->u4Log2MaxFrameNumMinus4 & 0xf) << 2); //5~2
    u4SPSInfo |= ((prDecPrm->prSPS->u4PicOrderCntType & 0x3) << 6); //7~6
    u4SPSInfo |= ((prDecPrm->prSPS->u4Log2MaxPicOrderCntLsbMinus4 & 0xf) << 8);
    u4SPSInfo |= (prDecPrm->prSPS->fgDeltaPicOrderAlwaysZeroFlag << 12);
    u4SPSInfo |= ((prDecPrm->prSPS->u4NumRefFrames & 0x1f) << 13);
    u4SPSInfo |= (prDecPrm->prSPS->fgFrameMbsOnlyFlag << 18);
    u4SPSInfo |= (prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag << 19);
    u4SPSInfo |= (prDecPrm->prSPS->fgDirect8x8InferenceFlag << 20);
    u4SPSInfo |= (1 << 21);
    //move after _VDEC_SetSHDRAVLD1, 5395 limitation
    _VDEC_SetSPSAVLD(u4VDecID, u4SPSInfo, prDecPrm->prSPS->u4PicWidthInMbsMinus1,
                     prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1);

    //PPS data prepare ============================================
    u4PPSInfo1 = prDecPrm->prPPS->fgEntropyCodingModeFlag;
    u4PPSInfo1 |= (prDecPrm->prPPS->fgPicOrderPresentFlag << 1);
    u4PPSInfo1 |= (prDecPrm->prPPS->fgWeightedPredFlag << 2);
    u4PPSInfo1 |= ((prDecPrm->prPPS->u4WeightedBipredIdc & 0x03) << 3);
    u4PPSInfo1 |= ((prDecPrm->prPPS->i4PicInitQpMinus26 & 0x3f) << 5);
    u4PPSInfo1 |= ((prDecPrm->prPPS->i4ChromaQpIndexOffset & 0x1f) << 11);
    u4PPSInfo1 |= (prDecPrm->prPPS->fgDeblockingFilterControlPresentFlag << 16);
    u4PPSInfo1 |= (prDecPrm->prPPS->fgConstrainedIntraPredFlag << 17);
    u4PPSInfo1 |= (prDecPrm->prPPS->fgTransform8x8ModeFlag << 18);
    u4PPSInfo1 |= ((prDecPrm->prPPS->i4SecondChromaQpIndexOffset & 0x1f) << 19);
    u4PPSInfo2 = prDecPrm->prPPS->u4NumRefIdxL0ActiveMinus1;
    u4PPSInfo2 |= (prDecPrm->prPPS->u4NumRefIdxL1ActiveMinus1 << 5);
    for (u4Idx = 0; u4Idx < 8; u4Idx++)
    {
        u4PPSInfo2 |= (prDecPrm->afgUserScalingListPresentFlag[u4Idx] << (10 + u4Idx));
    }
    u4PPSInfo2 |= (prDecPrm->fgUserScalingMatrixPresentFlag << 18);
    _VDEC_SetPPSAVLD(u4VDecID, u4PPSInfo1, u4PPSInfo2);

    //slice header info ===============================================
    u4SHDRInfo = prDecPrm->prSliceHdr->u4FirstMbInSlice & 0x1fff;
    u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4SliceType & 0xf) << 13);
    u4SHDRInfo |= (prDecPrm->prSliceHdr->fgFieldPicFlag << 17);
    u4SHDRInfo |= (prDecPrm->prSliceHdr->fgBottomFieldFlag << 18);
    u4SHDRInfo |= (prDecPrm->prSliceHdr->fgDirectSpatialMvPredFlag << 19);
    u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4NumRefIdxL0ActiveMinus1 & 0x1f) << 20);
    u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4NumRefIdxL1ActiveMinus1 & 0x1f) << 25);
    _VDEC_SetSHDRAVLD1(u4VDecID, u4SHDRInfo, prDecPrm->prSliceHdr->u4FirstMbInSlice);

#ifdef IS_POST_MT5368
    vVDecWriteVLDTOP(u4VDecID, RW_AVLD_PIC_SIZE, (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 << 16) | prDecPrm->prSPS->u4PicWidthInMbsMinus1);
#else
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PIC_SIZE, (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 << 8) | prDecPrm->prSPS->u4PicWidthInMbsMinus1);
#endif
}


// **************************************************************************
// Function : BOOL _VDEC_RefPicListReordering(UINT32 u4VDecID);
// Description : hw do reference list reordering
// Parameter : u4VDecID : hw id
// Return      : none
// **************************************************************************
BOOL _VDEC_RefPicListReordering(UINT32 u4VDecID)
{
    return _VDEC_AVCReordering(u4VDecID);    //hw function
}


// *********************************************************************
// Function    : void vVDecSetCurrPOC(void)
// Description : Set POC : Picture order count for display order
// Parameter   : None
// Return      : None
// Note  :JM decode_POC
// *********************************************************************
void _VDEC_SetCurrPOC(VDEC_AVC_DecPrm *prDecPrm)
{
    INT32 i4PrevPOCMsb;
    INT32 i4PrevPOCLsb;
    INT32 i4MaxPicOrderCntLsb;
    INT32 i4PrevFrameNumOffset;
    INT32 i4AbsFrameNum;
    INT32 i4PicOrderCntCycleCnt = 0;
    INT32 i4FrameNumInPicOrderCntCycle = 0;
    INT32 i4ExpectedDeltaPerPicOrderCnt;
    INT32 i4ExpectedDeltaPerPicOrderCntCycle;
    INT32 i;
    INT32 i4Temp;
    UINT8 u1ViewIdx;
    #if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    FBM_PIC_HDR_T * prFbmPicHdrBase;
    FBM_PIC_HDR_T * prFbmPicHdrDepend;
    #endif
    //UINT32 u4POCFbIdx;

    ASSERT(prDecPrm != NULL);
    i4MaxPicOrderCntLsb = (INT32)(1 << (prDecPrm->prSPS->u4Log2MaxPicOrderCntLsbMinus4 + 4));
    //ptSliceHdr = &_tSliceHdr;

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "SetPOC ViewIdx Err!\n");
        return;
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }

    switch (prDecPrm->prSPS->u4PicOrderCntType)
    {
        case 0:
            if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr))
            {
                i4PrevPOCMsb = 0;
                i4PrevPOCLsb = 0;
            }
            else
            {
                if (prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5)
                {
                    if (prDecPrm->rLastInfo[u1ViewIdx].u1LastPicStruct != BOTTOM_FIELD)
                    {
                        i4PrevPOCMsb = 0;
                        i4PrevPOCLsb = prDecPrm->rLastInfo[u1ViewIdx].i4LastRefTFldPOC;
                    }
                    else
                    {
                        i4PrevPOCMsb = 0;
                        i4PrevPOCLsb = 0;
                    }
                }
                else
                {
                    i4PrevPOCMsb = prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCMsb;
                    i4PrevPOCLsb = prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCLsb;
                }
            }

            // Calculate POCMsb
            if ((prDecPrm->prSliceHdr->i4PicOrderCntLsb < i4PrevPOCLsb) &&
                    ((i4PrevPOCLsb - prDecPrm->prSliceHdr->i4PicOrderCntLsb) >= (i4MaxPicOrderCntLsb / 2)))
            {
                prDecPrm->prSliceHdr->i4PicOrderCntMsb = i4PrevPOCMsb + i4MaxPicOrderCntLsb;
            }
            else if ((prDecPrm->prSliceHdr->i4PicOrderCntLsb > i4PrevPOCLsb) &&
                     ((prDecPrm->prSliceHdr->i4PicOrderCntLsb - i4PrevPOCLsb) > (i4MaxPicOrderCntLsb / 2)))
            {
                prDecPrm->prSliceHdr->i4PicOrderCntMsb = i4PrevPOCMsb - i4MaxPicOrderCntLsb;
            }
            else
            {
                prDecPrm->prSliceHdr->i4PicOrderCntMsb = i4PrevPOCMsb;
            }

            if ((!prDecPrm->prSliceHdr->fgFieldPicFlag) || (!prDecPrm->prSliceHdr->fgBottomFieldFlag))
            {
                prDecPrm->prCurrFBufInfo->i4TFldPOC = prDecPrm->prSliceHdr->i4PicOrderCntMsb +
                                                      prDecPrm->prSliceHdr->i4PicOrderCntLsb;
            }

            if ((!prDecPrm->prSliceHdr->fgFieldPicFlag))
            {
                prDecPrm->prCurrFBufInfo->i4BFldPOC = prDecPrm->prCurrFBufInfo->i4TFldPOC +
                                                      prDecPrm->prSliceHdr->i4DeltaPicOrderCntBottom;
            }
            else if (prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                prDecPrm->prCurrFBufInfo->i4BFldPOC = prDecPrm->prSliceHdr->i4PicOrderCntMsb +
                                                      prDecPrm->prSliceHdr->i4PicOrderCntLsb;
            }
            break;

        case 1:
            if (prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5)
            {
                i4PrevFrameNumOffset = 0;
            }
            else
            {
                i4PrevFrameNumOffset = prDecPrm->rLastInfo[u1ViewIdx].i4LastFrameNumOffset;
            }
            if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr))
            {
                prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0] = 0;
                prDecPrm->i4FrmNumOffset = 0;
            }
            else if (prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum > prDecPrm->prSliceHdr->u4FrameNum)
            {
                prDecPrm->i4FrmNumOffset = i4PrevFrameNumOffset + (INT32)prDecPrm->prSPS->u4MaxFrameNum;
            }
            else
            {
                prDecPrm->i4FrmNumOffset = i4PrevFrameNumOffset;
            }

            if (prDecPrm->prSPS->u4NumRefFramesInPicOrderCntCycle != 0)
            {
                i4AbsFrameNum = prDecPrm->i4FrmNumOffset + (INT32)prDecPrm->prSliceHdr->u4FrameNum;
            }
            else
            {
                i4AbsFrameNum = 0;
            }

            if ((prDecPrm->u1NalRefIdc == 0) && (i4AbsFrameNum > 0))
            {
                i4AbsFrameNum --;
            }

            if (i4AbsFrameNum > 0)
            {
                i4PicOrderCntCycleCnt = (i4AbsFrameNum - 1) / (INT32)prDecPrm->prSPS->u4NumRefFramesInPicOrderCntCycle;
                i4FrameNumInPicOrderCntCycle = (i4AbsFrameNum - 1) %
                                               (INT32)prDecPrm->prSPS->u4NumRefFramesInPicOrderCntCycle;
            }

            i4ExpectedDeltaPerPicOrderCntCycle = 0;
            for (i = 0; i < (INT32)prDecPrm->prSPS->u4NumRefFramesInPicOrderCntCycle; i++)
            {
                i4ExpectedDeltaPerPicOrderCntCycle += prDecPrm->prSPS->ai4OffsetForRefFrame[i];
            }
            if (i4AbsFrameNum > 0)
            {
                i4ExpectedDeltaPerPicOrderCnt = i4PicOrderCntCycleCnt * i4ExpectedDeltaPerPicOrderCntCycle;
                for (i = 0; i <= i4FrameNumInPicOrderCntCycle; i++)
                {
                    i4ExpectedDeltaPerPicOrderCnt = i4ExpectedDeltaPerPicOrderCnt +
                                                    prDecPrm->prSPS->ai4OffsetForRefFrame[i];
                }
            }
            else
            {
                i4ExpectedDeltaPerPicOrderCnt = 0;
            }
            if (prDecPrm->u1NalRefIdc == 0)
            {
                i4ExpectedDeltaPerPicOrderCnt = i4ExpectedDeltaPerPicOrderCnt +
                                                prDecPrm->prSPS->i4OffsetForNonRefPic;
            }
            if (!prDecPrm->prSliceHdr->fgFieldPicFlag)
            {
                prDecPrm->prCurrFBufInfo->i4TFldPOC = i4ExpectedDeltaPerPicOrderCnt +
                                                      prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0];
                i4Temp = prDecPrm->prCurrFBufInfo->i4TFldPOC + prDecPrm->prSPS->i4OffsetForTopToBottomField;
                prDecPrm->prCurrFBufInfo->i4BFldPOC = i4Temp + prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[1];
                prDecPrm->prCurrFBufInfo->i4BFldPOC = prDecPrm->prCurrFBufInfo->i4TFldPOC +
                                                      prDecPrm->prSPS->i4OffsetForTopToBottomField +
                                                      prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[1];
            }
            else if (!prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                prDecPrm->prCurrFBufInfo->i4TFldPOC = i4ExpectedDeltaPerPicOrderCnt +
                                                      prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0];
            }
            else
            {
                prDecPrm->prCurrFBufInfo->i4BFldPOC = i4ExpectedDeltaPerPicOrderCnt +
                                                      prDecPrm->prSPS->i4OffsetForTopToBottomField +
                                                      prDecPrm->prSliceHdr->ai4DeltaPicOrderCnt[0];
            }
            break;

        case 2:
            if (prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5)
            {
                i4PrevFrameNumOffset = 0;
            }
            else
            {
                i4PrevFrameNumOffset = prDecPrm->rLastInfo[u1ViewIdx].i4LastFrameNumOffset;
            }
            if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr))
            {
                prDecPrm->i4FrmNumOffset = 0;
            }
            else if (prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum > prDecPrm->prSliceHdr->u4FrameNum)
            {
                prDecPrm->i4FrmNumOffset = i4PrevFrameNumOffset + (INT32)prDecPrm->prSPS->u4MaxFrameNum;
            }
            else
            {
                prDecPrm->i4FrmNumOffset = i4PrevFrameNumOffset;
            }
            if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr))
            {
                // Use iAbsFrameNum as tempPicOrderCnt
                i4AbsFrameNum = 0;
            }
            else if (prDecPrm->u1NalRefIdc == 0)
            {
                i4AbsFrameNum = ((prDecPrm->i4FrmNumOffset + (INT32)prDecPrm->prSliceHdr->u4FrameNum) * 2) - 1;
            }
            else
            {
                i4AbsFrameNum = ((prDecPrm->i4FrmNumOffset + (INT32)prDecPrm->prSliceHdr->u4FrameNum) * 2);
            }

            if (!prDecPrm->prSliceHdr->fgFieldPicFlag)
            {
                prDecPrm->prCurrFBufInfo->i4TFldPOC = i4AbsFrameNum;
                prDecPrm->prCurrFBufInfo->i4BFldPOC = i4AbsFrameNum;
            }
            else if (!prDecPrm->prSliceHdr->fgBottomFieldFlag)
            {
                prDecPrm->prCurrFBufInfo->i4TFldPOC = i4AbsFrameNum;
            }
            else
            {
                prDecPrm->prCurrFBufInfo->i4BFldPOC = i4AbsFrameNum;
            }
            break;

        default:
            break;
    }
    prDecPrm->prCurrFBufInfo->i4POC =
        (prDecPrm->prCurrFBufInfo->i4TFldPOC < prDecPrm->prCurrFBufInfo->i4BFldPOC) ?
        prDecPrm->prCurrFBufInfo->i4TFldPOC : prDecPrm->prCurrFBufInfo->i4BFldPOC;

    LOG(7,"cur poc = %d\n",prDecPrm->prCurrFBufInfo->i4POC);
    #if BDP_LOG_STYLE
    VDecPrintf(u1ViewIdx,
                     VDEC_LOG_DECINFO,
                    "[VDEC-H264]: ESID(%d), TopPoc(%d), BotPoc(%d),CurPoc(%d)\n",
                    u1ViewIdx,
                    prDecPrm->prCurrFBufInfo->i4TFldPOC,
                    prDecPrm->prCurrFBufInfo->i4BFldPOC,
                    prDecPrm->prCurrFBufInfo->i4POC);
    #endif
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
    //u4POCFbIdx = 0xFF;
    if (u1ViewIdx > 0)  //need this if input frames for base/dependent are not in-pair.
    {
        for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
        {
            if (prDecPrm->arFBufInfo[0][i].u1FBufStatus != NO_PIC)
            {
                if ((prDecPrm->arFBufInfo[0][i].i4POC == prDecPrm->prCurrFBufInfo->i4POC))
                      //  && prDecPrm->arFBufInfo[0][i].fgIsInterview)
                {                    
                    if(prDecPrm->fgIsMVC)
                    {
                        prFbmPicHdrDepend = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
                        prFbmPicHdrBase = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[0], prDecPrm->arFBufInfo[0][i].ucFbId);
                        if(prFbmPicHdrDepend && prFbmPicHdrBase)
                        {
                            if ((FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[0], prDecPrm->arFBufInfo[0][i].ucFbId) ==
                                FBM_FB_STATUS_DISPLAYQ)||(FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[0], prDecPrm->arFBufInfo[0][i].ucFbId) ==
                                FBM_FB_STATUS_LOCK))
                            {
                                if(prFbmPicHdrBase->u8PTS > 3600)
                                {
                                    prFbmPicHdrBase->u8PTS += 3600;
                                }
                            }
                            prFbmPicHdrDepend->u4PTS = prFbmPicHdrBase->u4PTS;
                            prFbmPicHdrDepend->u8PTS = prFbmPicHdrBase->u8PTS;
                        }
                        else
                        {
                            if (REAL_DECODE == prDecPrm->ucVDecStatus)
                            {
                                LOG(0,"(prFbmPicHdrDepend && prFbmPicHdrBase) one is null\n");
                            }
                        }
                    }
                    //u4POCFbIdx = i;
                    if(prDecPrm->arFBufInfo[0][i].fgIsInterview)
                    {
                        prDecPrm->u4POCFbIdx = i;
                        break;
                    }
                }
            }
        }
    }
    /*if(u4POCFbIdx == 0xFF)
    {
        prDecPrm->fgAllocFbFail = TRUE;
        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->ucPreAllocFbId,
        FBM_FB_STATUS_EMPTY);
        LOG(3, "BaseView same POC missing\n");
    }*/
#endif
}


// *********************************************************************
// Function    : void vSlidingWindowProce(void)
// Description : marking the decoded ref pic with sliding window method
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_SlidingWindowProce(VDEC_AVC_DecPrm *prDecPrm)
{
    UINT32 i;
    INT32 i4MinFrameNumWrap;
    INT32 i4FrameNumWrap;
    UINT32 u4NumShortTerm;
    UINT32 u4NumLongTerm;
    UINT32 u4MinFBufIdx = 0;
    UINT32 u4MinPOCFBufIdx = 0;
    INT32 i4MinPOC = 0x7fffffff;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "Sliding ViewIdx Err!\n");
        return;
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }

    // If the curr pic is the 2nd field, follow the 1st field's ref info
    if ((prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME) && (prDecPrm->u1PicStruct == BOTTOM_FIELD) &&
            (prDecPrm->prCurrFBufInfo->u1TFldRefType == SREF_PIC))
    {
        prDecPrm->prCurrFBufInfo->u1BFldRefType = SREF_PIC;
    }
    else if ((prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME) && (prDecPrm->u1PicStruct == TOP_FIELD) &&
             (prDecPrm->prCurrFBufInfo->u1BFldRefType == SREF_PIC))
    {
        prDecPrm->prCurrFBufInfo->u1TFldRefType = SREF_PIC;
    }
    else
    {
        i = 0;
        i4MinFrameNumWrap = 0xfffffff;
        u4NumShortTerm = 0;
        u4NumLongTerm = 0;
        // Remove 1 SREF pic for a new ref pic
        for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
        {
            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == SREF_PIC) ||
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == SREF_PIC))
            {
                i4FrameNumWrap = (INT32)((prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum > prDecPrm->prCurrFBufInfo->u4FrameNum) ?
                                         (prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum - prDecPrm->prSPS->u4MaxFrameNum) :
                                         prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum);
                if (i4MinFrameNumWrap > i4FrameNumWrap)
                {
                    i4MinFrameNumWrap =  i4FrameNumWrap;
                    u4MinFBufIdx = (UINT32)i;
                }
                LOG(9, "SW SREF POC %d, FrameNum %d\n",
                    prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC , prDecPrm->arFBufInfo[u1ViewIdx][i].u4FrameNum);
                u4NumShortTerm ++;
            }
            if ((prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType == LREF_PIC) ||
                    (prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType == LREF_PIC))
            {
                u4NumLongTerm ++;
            }
            // check wrap around POC
            if (i4MinPOC > prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC)
            {
                i4MinPOC = prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC;
                u4MinPOCFBufIdx = i;
            }
        }
        // check wrap around POC
        if (prDecPrm->i4LastPOC[u1ViewIdx] < prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].i4POC)
        {
            prDecPrm->fgPOCWrap = FALSE;
        }
        else if ((FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId)
                  == FBM_FB_STATUS_DECODE) && (prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME))
        {
            if (prDecPrm->fgIsMMPlay)
            {
                prDecPrm->fgPOCWrap = TRUE;
            }
            LOG(2, "POC wrap detect!\n");
        }
        // Since current pic should be ref pic, the condition should be modified as "larger" only
        // but the current one not set as ref pic at this time,
        if ((u4NumShortTerm + u4NumLongTerm) >=
                ((prDecPrm->prSPS->u4NumRefFrames > 0) ? prDecPrm->prSPS->u4NumRefFrames : 1))
        {
            if ((u4MinFBufIdx != u4MinPOCFBufIdx) || (!prDecPrm->fgPOCWrap))
            {
                // Remove the smallet FrameNumWrap item
                LOG(9, "RM SREF POC %d, FrameNum %d\n",
                    prDecPrm->arFBufInfo[u1ViewIdx][u4MinFBufIdx].i4POC , prDecPrm->arFBufInfo[u1ViewIdx][u4MinFBufIdx].u4FrameNum);
                _vClrPicRefInfo(prDecPrm, FRAME, (UINT8)u4MinFBufIdx + ((UINT16)u1ViewIdx << 8));
            }
            //prDecPrm->arFBufInfo[u1ViewIdx][u4MinFBufIdx].u1FBufRefType = NREF_PIC;
            //prDecPrm->arFBufInfo[u1ViewIdx][u4MinFBufIdx].u1TFldRefType = NREF_PIC;
            //prDecPrm->arFBufInfo[u1ViewIdx][u4MinFBufIdx].u1BFldRefType = NREF_PIC;
#if 0
            _ptFBufInfo[dwMinFBufIdx].dwFrameNum = 0xffffffff;
            _ptFBufInfo[dwMinFBufIdx].iFrameNumWrap = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].iPicNum = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].iTFldPicNum = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].iBFldPicNum = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].bLongTermFrameIdx = 0xff;
            _ptFBufInfo[dwMinFBufIdx].bTFldLongTermFrameIdx = 0xff;
            _ptFBufInfo[dwMinFBufIdx].bBFldLongTermFrameIdx = 0xff;
            _ptFBufInfo[dwMinFBufIdx].iLongTermPicNum = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].iTFldLongTermPicNum = 0xefffffff;
            _ptFBufInfo[dwMinFBufIdx].iBFldLongTermPicNum = 0xefffffff;
#endif
        }
    }
}


// *********************************************************************
// Function    : void vFlushBufRefInfo()
// Description : flush DPB Ref info
// Parameter   : None
// Return      : None
// *********************************************************************
void _VDEC_FlushBufRefInfo(VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 i, u1ViewIdx;

    ASSERT(prDecPrm != NULL);
    prDecPrm->u1MaxLongTermFrameIdx = 0xff;

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "FlushBufRef ViewIdx Err!\n");
        return;
    }
    //for(u1ViewIdx=0; u1ViewIdx<MAX_VIEW_NUM; u1ViewIdx++)
    {
        for (i = 0; (i < prDecPrm->u4MaxFBufNum && i < MAX_FRAME_NUM); i++)
        {
            prDecPrm->arFBufInfo[u1ViewIdx][i].fgNonExisting = FALSE;

            prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType = NREF_PIC;
            prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType = NREF_PIC;
            prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType = NREF_PIC;
            prDecPrm->arFBufInfo[u1ViewIdx][i].u4LongTermFrameIdx = 0xff;
            prDecPrm->arFBufInfo[u1ViewIdx][i].u4TFldLongTermFrameIdx = 0xff;
            prDecPrm->arFBufInfo[u1ViewIdx][i].u4BFldLongTermFrameIdx = 0xff;
            prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsInterview = FALSE;
        }
    }
    for (i = 0; i < 3; i++)
    {
        prDecPrm->arRefPicList[i].u4RefPicCnt = 0;
    }
}


// **************************************************************************
// Function : UINT8 _VDEC_WaitDecDone(VDEC_AVC_DecPrm rDecPrm);
// Description : WaitDecDone
// Parameter : rDecPrm
// Return      : BOOL, if decode successfully
// **************************************************************************
UINT8 _VDEC_WaitDecDone(UCHAR ucEsId, UINT32 u4VDecId, VDEC_AVC_DecPrm *prDecPrm)
{
    UINT8 u1Ret = 0;
    UINT32 u4CntTimeChk = 0;
    UINT32 u4MbX, u4MbY, u4Tmp;
    #ifdef CC_DUAL_CORE_DEC
    UINT32 u4Temp = 0, u4Err_mbx = 0, u4Err_mby = 0, u4Dec_mbx = 0, u4Dec_mby = 0;
    #endif
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    VDEC_INFO_T *prVdecInfo;
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_STATUS_CB_T rStatusCB;
    prVdecInfo = _VDEC_GetInfo();
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (prDecPrm->bWaitDecLock)
    {  
    #ifdef ENABLE_MULTIMEDIA
        if (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
        {
            if (x_sema_lock_timeout(_hVdecFinSema[u4VDecId], 30) != OSR_OK)
            {
                LOG(2, "%s: SWDMX_SRC_TYPE_WIFI_DISPLAY x_sema_lock_timeout\n", __FUNCTION__);
            }
        }
        else
    #endif
        {
            if ((VDEC_LAE0 == u4VDecId) || (VDEC_LAE1 == u4VDecId))
            {
                if (x_sema_lock_timeout(_hVdecFinSema[u4VDecId], 100) != OSR_OK)
                {
                    LOG(2, "%s, %d: x_sema_lock_timeout\n", __FUNCTION__, u4VDecId);
                }
                else
                {
                    u1Ret = 1;
                }
            }
            else
            {
                if (x_sema_lock_timeout(_hVdecFinSema[u4VDecId], 500) != OSR_OK)
                {
                    LOG(2, "%s, %d: x_sema_lock_timeout\n", __FUNCTION__, u4VDecId);
                #ifdef ENABLE_MULTIMEDIA
                    if (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH)
                    {
                        prVdecEsInfoKeep->u4TimeoutCnt++;
                    }
                #endif
                }
            }
        }
    }
    
    if ((VDEC_LAE0 != u4VDecId) && (VDEC_LAE1 != u4VDecId))
    {
        u1Ret = (UINT8)(_bTimeOutFlag ? 0 : 1);
        if (prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME)
        {
            _prDbase = DBS_Lock(&_rCritState);
            ASSERT(_prDbase);
            VERIFY(DBS_Unlock(_prDbase, _rCritState));
            prInfo = &(_prDbase->rMpv[ucEsId]);

            if (_bTimeOutFlag == 1)
            {
                prInfo->u4TimeOutNs++;
                if (prVdecInfo->pfnStatusCb)
                {
                    if (prVdecEsInfoKeep)
                    {
                        rStatusCB.u1EsId = prVdecEsInfoKeep->rPesInfo.ucEsId;
                    }
                    else
                    {
                        rStatusCB.u1EsId = 0;
                    }
                    rStatusCB.u4Status = (UINT32)VDEC_PIC_DECODING_ERROR;
                    prVdecInfo->pfnStatusCb(&rStatusCB);
                }
            }
            else
            {
                prInfo->u4DecOkNs++;
            }
        }
        if (_bTimeOutFlag)
        {
            LOG(2, "//Error! TimeOut, Error Msg 0x270=0x%x !!!\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_MESSAGE));
            LOG(2, "//AVC_VLD 0x284(161) Error acc 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_ACC));
        #ifdef IS_POST_MT5368
            LOG(2, "//AVC_VLD 80(32) decode cycle 0x%x\n", u4VDecReadVLDTOP(u4VDecId, RO_AVLD_DEC_CYCLE));
        #else //IS_POST_MT5368
            LOG(2, "//AVC_VLD 288(162) decode cycle 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_DEC_CYCLE));
        #endif
            u4CntTimeChk = DEC_RETRY_NUM_AVC;
            _bTimeOutFlag = FALSE;
        }
        else    //wait picture finish
        {
            while (((u4VDecReadAVCVLD(u4VDecId, RO_AVLD_PIC_FINISH) & AVLD_FINISH_MASK) == 0) &&
                    (u4CntTimeChk < DEC_RETRY_NUM_AVC))
            {
                u4CntTimeChk++;
            }
        #ifdef VDEC_TIME_PROFILE
            #ifdef IS_POST_MT5363
            u4DramCycle = IO_REG32(DRAM_BASE, 0x190);
            #else
            u4DramCycle = IO_REG32(DRAM_BASE, 0x3C8);
            #endif
        #endif
        }

        #ifdef CC_STRESS_TEST_CRC_CHK
        //check DRAMC CRC result
        u4DramCrcMC_R = IO_READ32(0xf0007000, 0x2C4);
        u4DramCrcPP_W = IO_READ32(0xf0007000, 0x2C0);
        //disable DRAMC moniter
        IO_WRITE32(0xf0007000, 0x258, IO_READ32(0xf0007000, 0x258) & 0xF7FFF7FF);
        #endif

        u4Tmp = u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_ACC) & ERR_MASK_OBSV;
        if (u4Tmp != 0)
        {
            LOG(5, "AVC_VLD 284(161) Error acc 0x%x, MB ErrCnt %d\n", u4Tmp, u4VDecReadMISC(u4VDecId, RO_VDEC_MISC_MB_ERR_CNT));    //ignore bit21
        }
        u4Tmp = u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_MESSAGE) & ERR_MASK_OBSV;
        if ((u4CntTimeChk == DEC_RETRY_NUM_AVC) || (u4Tmp != 0))
        {
            if (u4CntTimeChk == DEC_RETRY_NUM_AVC)
            {
                LOG(2, "\n!!!!!!!!! Decoding Timeout !!!!!!!\n");
                #ifdef CC_DUAL_CORE_DEC //mbaff case timeout special case use vp mode to do error concealment, only for oryx E1
                if (prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag
                    && (!prDecPrm->prSliceHdr->fgFieldPicFlag))
                {
                    u4Temp = u4VDecReadAVCVLD(u4VDecId, (148*4));
                    LOG(2, "avc_vld_148 %x, mbx %d, mby %d\n", u4Temp, (u4Temp >> 23), ((u4Temp << 9) >> 23));
                    u4Dec_mbx = (u4Temp >> 23);
                    u4Dec_mby = ((u4Temp << 9) >> 23);
                    u4Temp = u4VDecReadAVCVLD(u4VDecId, (185*4));
                    LOG(2, "avc_vld_185 %x, mby %d, mbx %d\n", u4Temp, (u4Temp >> 23), ((u4Temp << 9) >> 23));
                    u4Err_mbx = ((u4Temp << 9) >> 23);
                    u4Err_mby = (u4Temp >> 23);
                    u4MbX = ((prDecPrm->u4PicW + 15)>> 4) - 1;
                    u4MbY = (((prDecPrm->u4PicH >> (1-(fgIsFrmPic(u4VDecId)))) + 15)>> 4) - 1;
                    LOG(2, "mbx %d, mby %d\n", u4MbX, u4MbY);
                    if ((u4Dec_mbx == u4MbX) && (u4Dec_mby == u4MbY))
                    {                
                        u4VPmode(u4VDecId, u4Err_mbx, u4Err_mby, u4MbX, u4MbY, prDecPrm);
                    }
                }
                #endif
                //dump status register
                LOG(2, "AVC_VLD 274(157) Dec complete 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_COMPLETE));
                LOG(2, "AVC_VLD 2C8(178) 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_EMU_PREV));
                LOG(2, "AVC_VLD 284(161) Error acc 0x%x, MB ErrCnt %d\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_ACC), u4VDecReadMISC(u4VDecId, RO_VDEC_MISC_MB_ERR_CNT));
                LOG(2, "AVC_VLD 270(156) Error msg 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_ERR_MESSAGE));
                LOG(2, "AVC_VLD 84(33) 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_CTRL));
                LOG(2, "AVC_VLD 2D4(181) 0x%x\n", u4VDecReadAVCVLD(u4VDecId, (181*4)));
                LOG(2, "AVC_VLD 280(160) Error mask 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_ERR_MASK));
                LOG(2, "AVC_VLD 244(145) 0x%x\n", u4VDecReadAVCVLD(u4VDecId, (145*4)));
                u4Tmp = u4VDecReadAVCVLD(u4VDecId, RO_AVLD_SMKR);
                LOG(2, "AVC_VLD 250(148) 0x%x, x %d, y %d\n", u4Tmp, (u4Tmp >> 23), ((u4Tmp << 9) >> 23));
                #ifdef IS_POST_MT5368
                LOG(2, "VLD_TOP 50(20) timeout threshold 0x%x\n", u4VDecReadVLDTOP(u4VDecId, RW_AVLD_TIMEOUT_THD));
                LOG(2, "VLD_TOP 58(22) mc busy threshold 0x%x\n\n", u4VDecReadVLDTOP(u4VDecId, RW_AVLD_MC_BUSY_THD));
                LOG(2, "VLD_TOP 80(32) decode cycle 0x%x\n", u4VDecReadVLDTOP(u4VDecId, RO_AVLD_DEC_CYCLE));
                #else
                LOG(2, "AVC_VLD 268(154) timeout threshold 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_TIMEOUT_THD));
                LOG(2, "AVC_VLD 290(164) mc busy threshold 0x%x\n\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_MC_BUSY_THD));
                LOG(2, "AVC_VLD 288(162) decode cycle 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_DEC_CYCLE));
                #endif
                LOG(2, "VLD F8(62) Wp 0x%x\n", u4VDecReadVLD(u4VDecId, RO_VLD_VWPTR));
                LOG(2, "VLD FC(63) Rp 0x%x\n", u4VDecReadVLD(u4VDecId, RO_VLD_VRPTR));
                LOG(2, "VLD B4(45) Vstart 0x%x\n", u4VDecReadVLD(u4VDecId, RW_VLD_VSTART) << 6);
                LOG(2, "VLD B8(46) VEnd 0x%x\n\n", u4VDecReadVLD(u4VDecId, RW_VLD_VEND) << 6);
    
                LOG(2, "AVC_VLD 29C(167) Residual checksum 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_RESISUAL_SUM));
                LOG(2, "AVC_VLD 2A0(168) Bs-in checksum 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_BS_SUM));
                LOG(2, "AVC_VLD 2d0(180) ERC control 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RW_AVLD_ERC_CTRL));
                LOG(2, "AVC_VLD 248(146) slice hdr state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_SLICE_HDR_STATE));
                LOG(2, "AVC_VLD 254(149) slice data state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_SLICE_STATE));
                LOG(2, "AVC_VLD 258(150) mb state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_MB_STATE));
                LOG(2, "AVC_VLD 25C(151) mb pred state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_MB_PRED_STATE));
                LOG(2, "AVC_VLD 260(152) residual state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_RESIDUAL_STATE));
                LOG(2, "AVC_VLD 264(153) residual blk state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_RESIDUAL_BLK_STATE));
                LOG(2, "AVC_VLD 24C(147) weighted pred state 0x%x\n", u4VDecReadAVCVLD(u4VDecId, RO_AVLD_WEIGHTED_PRED_STATE));
                LOG(2, "MC 5c4(369) PP status 0x%x\n", u4VDecReadMC(u4VDecId, RO_MC_PP_STATUS));
	            u1Ret = 0;    //need log slice info
	            #ifndef STRESS_TESTING
	                while (1);
	            #endif
            }
            if (prVdecEsInfo->ucDualCoreMode)
            {
                u4Tmp = 1 + prDecPrm->prSliceHdr->fgFieldPicFlag;
                if (prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag 
                    && (!prDecPrm->prSliceHdr->fgFieldPicFlag))
                {
                    if (0 == ((prDecPrm->u4PicH/32) % 2))
                    {
                        _VDEC_MCGetMbXY(VDEC_VLD1, &u4MbX, &u4MbY);
                    }
                    else
                    {
                        _VDEC_MCGetMbXY(VDEC_VLD0, &u4MbX, &u4MbY);
                    }
                }
                else
                {
                    if (0 == ((prDecPrm->u4PicH/u4Tmp/16) % 2))
                    {
                        _VDEC_MCGetMbXY(VDEC_VLD1, &u4MbX, &u4MbY);
                    }
                    else
                    {
                        _VDEC_MCGetMbXY(VDEC_VLD0,&u4MbX, &u4MbY);
                    }
                }
            }
            else
            {
                _VDEC_MCGetMbXY(u4VDecId, &u4MbX, &u4MbY);
            }
            #ifdef ENABLE_MULTIMEDIA
            prVdecEsInfo->rWFDDecodeErr.i4TimeoutCnt++;
            #endif
        }
#ifdef VDEC_TIME_PROFILE
        if (u1Ret > 0)
        {
#ifdef IS_POST_MT5368
            u4DecCycle = VLD_TOP_READ32(RO_AVLD_DEC_CYCLE);
#else
            u4DecCycle = AVC_VLD_READ32(RO_AVLD_DEC_CYCLE);
#endif
            if (u4MaxDecCycle < u4DecCycle)
            {
                u4MaxDecCycle = u4DecCycle;
                LOG(8, "//Max Dec Cycle %d\n", u4MaxDecCycle);
            }
        }
#endif
    }
    return u1Ret;
}


// **************************************************************************
// Function : void _VDEC_DecProc(void);
// Description : Init Video Decoder
// Parameter : None
// Return      :
// **************************************************************************
void _VDEC_DecProc(UINT32 u4VDecID, const VDEC_AVC_DecPrm* prDecPrm, INT32* pi4RetValue)
{
    UINT32 u4Temp, u4SHDRInfo;	
	#ifdef CC_DUAL_CORE_DEC
	UINT32 u4PicWidthMB, u4PicHeigthMB, u4OffsetByte, u4OffsetBit, u4MaskBit, u4Idx, u4Addr, u4Data;
	#endif
    //static UINT32 u4Cnt = 0;
#if (defined(CC_MT5360B) || defined(CC_MT5392B) || defined(IS_POST_MT5363)) && defined (FBM_DPB_CTRL)
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
#endif
#if defined(IS_POST_MT5395) && defined(CC_DRIVER_DEMO)
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
#endif
    VDEC_ES_INFO_T* prVdecEsInfo;
#ifdef CC_MT5365
    UINT32 u4DramType = DRVCUST_InitGet(eDramType);
#endif

#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, _VDEC_DecProc Start\n", u4VDecID);
#endif

    ASSERT(pi4RetValue != NULL);
    ASSERT(prDecPrm != NULL);

    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
#if (defined (VDEC_IS_POST_MT5882))
    vVDecWriteMC(u4VDecID, RW_MC_Y_C_SEPERATE_ONOFF, ((u4VDecReadMC(u4VDecID, RW_MC_Y_C_SEPERATE_ONOFF))|1));
#endif

    // Fld & Frame height same?
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, prDecPrm->u4PicW);
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, prDecPrm->u4PicH);

    ASSERT(PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufY) > 0);
    ASSERT(PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufC) > 0);
    
    vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 1);
    vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufY) >> 9);
    vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufC) >> 8);
    vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 1);
    vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, (UINT32)(DBLK_Y | DBLK_C));
    vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15) >> 4) - 1);
    u4Temp = (1 - (UINT32)(fgIsFrmPic(prDecPrm->u1PicStruct)));
    u4Temp = ((prDecPrm->u4PicH >> u4Temp) + 15);
    vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (u4Temp >> 4) - 1);
    //MC_WRITE32(RW_MC_PP_MB_WIDTH, (((prDecPrm->u4PicW + 15)>> 4) + 0x3)& (~0x3));
    if (prDecPrm->fgOutDispOrder)
    {
        if (prDecPrm->fgIsUFOMode || prDecPrm->fgIsNewMode)
        {
            vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, ((prDecPrm->u4PicW + 15) >> 4));
        }
        else
        {
        	#if (defined (CC_MT5890))
			if(BSP_GetIcVersion() >= IC_VER_5890_AC)
			{
				vVDecWritePP(u4VDecID, (743*4), (vVDecReadPP(u4VDecID,(743*4))| 0x1));
			}
			#endif
            vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, ((prVdecEsInfo->u2LineSize + 15) >> 4));
        }
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, ((prDecPrm->u4PicW + 15) >> 4));
    }
    //should be multiple of 4
    vVDecWriteMC(u4VDecID, RW_MC_Y_OUT_ADDR, PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufY));

#if (!defined (VDEC_IS_POST_MT5882))
    vVDecWriteMC(u4VDecID, RW_MC_CBCR_OFFSET, (UINT32)prDecPrm->pu1DecWorkBufC - (UINT32)prDecPrm->pu1DecWorkBufY);
#endif
    if (prDecPrm->fgOutDispOrder && (!(prDecPrm->fgRRModeYEn || prDecPrm->fgRRModeCEn)))
    {
        if (prDecPrm->fgIsUFOMode || prDecPrm->fgIsNewMode)
        {
            vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, ADDR_MODE_8520);    //8520 swap mode
            #if defined(IS_POST_MT5363)
            vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP_NBM, ADDR_MODE_8520);
            vVDecWritePP(u4VDecID, RW_PP_ADDRSWAP_SEL, PP_ADDR_MODE_5351_NORMAL);
            #endif
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, ADDR_MODE_5351_NORMAL);
            #if defined(IS_POST_MT5363)
            vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP_NBM, ADDR_MODE_5351_NORMAL);
            vVDecWritePP(u4VDecID, RW_PP_ADDRSWAP_SEL, PP_ADDR_MODE_5351_NORMAL);
            #endif
        }
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, ADDR_MODE_8520);    //8520 swap mode
#if defined(IS_POST_MT5363)
        vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP_NBM, ADDR_MODE_8520);
        vVDecWritePP(u4VDecID, RW_PP_ADDRSWAP_SEL, PP_ADDR_MODE_8520);
#endif
    }
    //MC_WRITE32(RW_MC_ADDR_MODE, WADDM_RASTER |RADDM_RASTER);

#ifdef CC_MT5365
    if (u4DramType == DDR_III_x2)
    {
        vVDecWriteMC(u4VDecID, RW_MC_32B_MODE_EN1, u4VDecReadMC(u4VDecID, RW_MC_32B_MODE_EN1) | 1);
        vVDecWriteMC(u4VDecID, RW_MC_32B_MODE_EN2, u4VDecReadMC(u4VDecID, RW_MC_32B_MODE_EN2) | 1);

        if (prDecPrm->prSPS && (prDecPrm->prSPS->u4ChromaFormatIdc > 0)) //0: mono color(Y only)
        {
            vVDecWriteMC(u4VDecID, RW_MC_PP_OUT_32B_MODE_EN,
                       u4VDecReadMC(u4VDecID, RW_MC_PP_OUT_32B_MODE_EN) & 0xFFFFFFFE);
        }
        //force 64x32 block mode
        vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, ADDR_MODE_5351_NORMAL);
        vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP_NBM, ADDR_MODE_5351_NORMAL);
        vVDecWritePP(u4VDecID, RW_PP_ADDRSWAP_SEL, PP_ADDR_MODE_5351_NORMAL);
    }
#endif

    //RR mode
    if (prDecPrm->fgRRModeYEn | prDecPrm->fgRRModeCEn)
    {
        if (prDecPrm->fgRRWrapper)
        {
            vVDecWriteMC(u4VDecID, RW_MC_BYPASS_DBLK_WRAP, 0);
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_BYPASS_DBLK_WRAP, 1);
        }
        vVDecWriteMC(u4VDecID, RW_MC_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_RR_CTRL) & 0x3F000000) | RR_COMPACT |
                   ((UINT32)prDecPrm->fgRRModeYEn << EN_Y_BIT) |
                   ((UINT32)prDecPrm->fgRRModeCEn << EN_C_BIT) |
                   ((UINT32)prDecPrm->u1RRModeY << Y_RATIO_BIT) |
                   ((UINT32)prDecPrm->u1RRModeC << C_RATIO_BIT) |
                   ((UINT32)prDecPrm->u1MaxDisHbands << HBAND_BIT));
        vVDecWriteMC(u4VDecID, RW_MC_PP_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_PP_RR_CTRL) & 0x3F000000) | RR_COMPACT |
                   ((UINT32)prDecPrm->fgRRModeYEn << EN_Y_BIT) |
                   ((UINT32)prDecPrm->fgRRModeCEn << EN_C_BIT) |
                   ((UINT32)prDecPrm->u1RRModeY << Y_RATIO_BIT) |
                   ((UINT32)prDecPrm->u1RRModeC << C_RATIO_BIT) |
                   ((UINT32)prDecPrm->u1MaxDisHbands << HBAND_BIT));

#if (defined(CC_MT5360B) || defined(CC_MT5392B) || defined(IS_POST_MT5363)) && defined (FBM_DPB_CTRL)   //new field mode
        if (prDecPrm->u1CurViewIdx < MAX_VIEW_NUM)
        {
            prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
        }
        if (prFbmPicHdr != NULL)
        {
            if (!prFbmPicHdr->fgProgressiveFrm)
            {
                vVDecWriteMC(u4VDecID, RW_MC_RR_FMT, RR_FIELD_DISP_MODE);
            }
        }
        //disable MC request connection
        vVDecWriteMC(u4VDecID, RW_MC_REQ_CONNECT, u4VDecReadMC(u4VDecID, RW_MC_REQ_CONNECT) | REQ_CON_DISABLE);
#endif
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_BYPASS_DBLK_WRAP, 0);
#if defined(IS_POST_MT5363)
        vVDecWriteMC(u4VDecID, RW_MC_REQ_CONNECT, u4VDecReadMC(u4VDecID, RW_MC_REQ_CONNECT) | REQ_CON_DISABLE);
#endif
#if defined(CC_MT5360)
        {//!bypass
            vVDecWriteMC(u4VDecID, RW_MC_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_RR_CTRL) & 0xFFFFFFFE));
            vVDecWriteMC(u4VDecID, RW_MC_PP_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_PP_RR_CTRL) & 0xFFFFFFFE));
        }
#else
        {
            vVDecWriteMC(u4VDecID, RW_MC_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_RR_CTRL) & 0xFF000000) | BY_PASS);
            vVDecWriteMC(u4VDecID, RW_MC_PP_RR_CTRL, (u4VDecReadMC(u4VDecID, RW_MC_PP_RR_CTRL) & 0xFF000000) | BY_PASS);
        }
#endif
    }

    if (prDecPrm->fgOutDispOrder)
    {
#ifdef IS_POST_MT5368
        if (prDecPrm->fgIsUFOMode || prDecPrm->fgIsNewMode)
        {            
            vVDecWriteMC(u4VDecID, RW_MC_BLK_MODE_W, ((prDecPrm->u4PicW + 15) >> 4));
        }
        else
        {
            vVDecWriteMC(u4VDecID, RW_MC_BLK_MODE_W, ((prVdecEsInfo->u2LineSize + 15) >> 4));
        }
#else
        if (prDecPrm->fgIsUFOMode || prDecPrm->fgIsNewMode)
        {
            vVDecWriteVLD(u4VDecID, RW_MC_BLK_MODE_W, ((prDecPrm->u4PicW + 15) >> 4));
        }
        else
        {
            vVDecWriteVLD(u4VDecID, RW_MC_BLK_MODE_W, ((prVdecEsInfo->u2LineSize + 15) >> 4));
        }
#endif
    }
    else
    {
#ifdef IS_POST_MT5368
        vVDecWriteMC(u4VDecID, RW_MC_BLK_MODE_W, ((prDecPrm->u4PicW + 15) >> 4));
#else
        vVDecWriteVLD(u4VDecID, RW_MC_BLK_MODE_W, ((prDecPrm->u4PicW + 15) >> 4));
#endif
    }
    //restore 1/4 frame size MV info for each reference frame
    ASSERT(PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufMV) > 0);
#ifdef AVC_MV_BUF_REDUCE

#ifdef IS_POST_MT5368
    if (prDecPrm->prSPS && prDecPrm->prSPS->fgDirect8x8InferenceFlag)
    {
        vVDecWriteAVCMV(u4VDecID, RW_AMV_WR_ADDR, ((PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufMV)) >> 4) +
                       ((prDecPrm->u1PicStruct == BOTTOM_FIELD) ? 2 : 0));
    }
    else
#else
    if (prDecPrm->prSPS &&
            (prDecPrm->prSPS->fgFrameMbsOnlyFlag && (!prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag)))
    {
        vVDecWriteAVCMV(u4VDecID, RW_AMV_WR_ADDR, ((PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufMV)) >> 4) +
                       ((prDecPrm->u1PicStruct == BOTTOM_FIELD) ? 1 : 0));
    }
    else
#endif

#endif
    {
        vVDecWriteAVCMV(u4VDecID, RW_AMV_WR_ADDR, ((PHYSICAL((UINT32)prDecPrm->pu1DecWorkBufMV)) >> 4) +
                       ((prDecPrm->u1PicStruct == BOTTOM_FIELD) ? 4 : 0));
    }
    //prepare slice header info, part II
    if (prDecPrm->prSliceHdr)
    {
        u4SHDRInfo = (UINT32)((prDecPrm->prSliceHdr->i4SliceQpDelta) & 0x7f);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4DisableDeblockingFilterIdc & 0x3) << 7);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->i4SliceAlphaC0OffsetDiv2 & 0xf) << 9);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->i4SliceBetaOffsetDiv2 & 0xf) << 13);
        u4SHDRInfo |= ((prDecPrm->u1NalRefIdc & 0x3) << 17);
        u4SHDRInfo |= ((UINT32)(fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr)) << 19);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4CabacInitIdc & 0x3) << 20);
        _VDEC_SetSHDRAVLD2(u4VDecID, u4SHDRInfo);
    }
    if (prDecPrm->prPPS->fgEntropyCodingModeFlag) // CABAC only
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_INIT_CTX_SRAM, 0x1);
    }

    //MC new wrapper
#if defined(CC_MT5360B) || defined(CC_MT5392B) || defined(IS_POST_MT5363)
    vVDecWriteMC(u4VDecID, RW_MC_WRAPPER, ((u4VDecReadMC(u4VDecID, RW_MC_WRAPPER) |
                                PP_WRAPPER_Y_LINES) | PP_WRAPPER_C_LINES));
#endif
   u4Temp = u4VDecReadMC(u4VDecID, RW_MC_WRAPPER);
   u4Temp |= (0xFF00);
   vVDecWriteMC(u4VDecID, RW_MC_WRAPPER, u4Temp);

#ifdef CC_DRIVER_DEMO
#if defined(IS_POST_MT5395)
    //input
    if (prVdecEsInfoKeep)
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prVdecEsInfoKeep->rPesInfo.u4FifoStart),
                                             prVdecEsInfoKeep->rPesInfo.u4FifoEnd - prVdecEsInfoKeep->rPesInfo.u4FifoStart);
    }
    //output
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prDecPrm->pu1DecWorkBufY),
                                         ((UINT32)prDecPrm->pu1DecWorkBufC - (UINT32)prDecPrm->pu1DecWorkBufY) << 1);
#else
    HalFlushInvalidateDCache();
#endif
#endif

#ifdef IS_POST_MT5368
    //set total mbs in pic
    if (prDecPrm->prSPS && prDecPrm->prSliceHdr)
    {
        if ((!prDecPrm->prSPS->fgFrameMbsOnlyFlag) && (!prDecPrm->prSliceHdr->fgFieldPicFlag))
        {
            vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC,
                            ((prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) << 1) *
                            (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1));
        }
        else
        {
            vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC,
                            (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) *
                            (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1));
        }
    }
#endif
    if(prDecPrm->fgIsMMPlay)
    {
        vVDecWriteVLDTOP(u4VDecID, RO_VLD_TOP_TIMEOUT_THD, VLD_TOP_TIMEOUT_THD);//for 4k2k decode timeout
    }
    //Dram MC request max len
    if (prDecPrm->u4DramMaxLen > 0)
    {
        vVDecWriteMC(u4VDecID, RW_MC_REQ_MAX_LEN, MAX_LEN_EN | prDecPrm->u4DramMaxLen);
    }
    //MC_WRITE32(RW_MC_REQ_CONNECT, MC_READ32(RW_MC_REQ_CONNECT) | REQ_CON_DISABLE | FIFO_4BANK);
    vVDecWriteMC(u4VDecID, RW_MC_REQ_CONNECT, u4VDecReadMC(u4VDecID, RW_MC_REQ_CONNECT)
               | (prDecPrm->u4DramReqConnDis << 12) | (prDecPrm->u4DramFifoBank << 17));

    //profile decoding time
    HAL_GetTime(&rTimeHwS);
#ifdef VDEC_TIME_PROFILE
    u4DelayClock = (BSP_GetDomainClock(SRC_MEM_CLK) >> 1) ; //dram cycle /sec
#if defined(IS_POST_MT5363)
#if defined(CC_MT5395) || defined(CC_MT5880) || defined(CC_MT5881)
    u4DramCycle = SRM_DramMonitor(0, SRM_DRV_VDEC_MC, u4DelayClock, 0);
#else
    u4DramCycle = SRM_DramMonitor(SRM_DRV_VDEC_MC, u4DelayClock, 0);
#endif
#else
#if defined(CC_MT5395) || defined(CC_MT5880) || defined(CC_MT5881)
    u4DramCycle = SRM_DramMonitor(0, SRM_DRV_MPEG, u4DelayClock, 0);
#else
    u4DramCycle = SRM_DramMonitor(SRM_DRV_MPEG, u4DelayClock, 0);
#endif
#endif
#endif
    if (!prDecPrm->fgIsMVC)
    {
        if (prDecPrm->prSPS)
        {
            //_VDEC_ECBufSetting(prDecPrm,
                              //prDecPrm->arRefPicList[0].u4RefPicCnt,
                               //((prDecPrm->prSPS->u4NumRefFrames > 16) ? 16 : prDecPrm->prSPS->u4NumRefFrames), u4VDecID);
        }
        else
        {
            *pi4RetValue = E_VDEC_FAIL;
        }
    }

#if (defined (VDEC_IS_POST_MT5882))
    vVDecWriteVLDTOP(u4VDecID, AVC_VLD_TOP_PIC_PIX_SIZE, ((((prDecPrm->u4PicW) & 0xffff) << 0 ) | (((prDecPrm->u4PicH) & 0xffff) << 16 )));  
    
    //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0xa89ec007);
    if (prDecPrm->u4PicW < 176)
    {
        vVDecWriteMISC(u4VDecID, (60 << 2),(u4VDecReadMISC(u4VDecID, (60 << 2))| 0x1));
    }
#endif
#if (defined (CC_MT5890))
    if (prVdecEsInfo->ucDualCoreMode)
    {
        vVDecWriteAVCMV(u4VDecID, RW_AMV_CFG, (u4VDecReadAVCMV(u4VDecID, RW_AMV_CFG) | 0x5000));
    }
    if (prDecPrm->fgIsUFOMode)
    {
        vVDecWriteMC(u4VDecID, RW_MC_UFO_ONOFF, 0x11);
        vVDecWritePP(u4VDecID, (804*4), 3);
        vVDecWritePP(u4VDecID, (805*4), 0x3);
        vVDecWriteMC(u4VDecID, RW_MC_Y_LEN, PHYSICAL((UINT32)prDecPrm->prCurrFBufInfo->u4YLenStartAddr));
        vVDecWriteMC(u4VDecID, RW_MC_C_LEN, PHYSICAL((UINT32)prDecPrm->prCurrFBufInfo->u4CLenStartAddr));          
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_UFO_ONOFF, 0x0);
    }
    if (prDecPrm->fgIs10BitMode)
    {
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
        vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 1);
        vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 1);
    }
    if ((prDecPrm->u4PicW <= 1920) && (prDecPrm->u4PicH <= 1088) && (!prVdecEsInfo->ucDualCoreMode))
    {
        if (VDEC_VLD0 == u4VDecID || VDEC_VLD1 == u4VDecID)
        {
            vVDecWritePP(u4VDecID,  RW_PP_YWRAP_IDX_BASE, YWRAP_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_CWRAP_IDX_BASE, CWRAP_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_Y_TILE_IDX_BASE, Y_TILE_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_C_TILE_IDX_BASE, C_TILE_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_Y_FLAG_IDX_BASE, Y_FLAG_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_Y_TILE2_IDX_BASE, Y_TILE2_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_C_TILE2_IDX_BASE, C_TILE2_IDX_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_C_WRAP_IDX_8_BASE, C_WRAP_IDX_8_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_SAO_TILE_SIGN_Y_BASE, SAO_TILE_SIGN_Y_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_SAO_TILE_SIGN_C_BASE, SAO_TILE_SIGN_C_BASE);
            vVDecWritePP(u4VDecID,  RW_PP_SAO_TILE_PIXEL_BASE, SAO_TILE_PIXEL_BASE);
        }
        if (VDEC_VLD1 == u4VDecID)
        {
            vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_CORE1_SRAM_OFFSET, CORE1_SRAM_OFFSET);
            vVDecWritePP(u4VDecID,  RW_PP_WRAP_SRAM_OFFSET, WRAP_SRAM_OFFSET);
        }
    }	
	#ifdef CC_DUAL_CORE_DEC
	if((BSP_GetIcVersion() >= IC_VER_5890_AC) && prVdecEsInfo->ucDualCoreMode) //sw workaround for oryx E3 hw bug
	{
		UINT32 u4ECBufSize;
		u4ECBufSize = 32 * (prDecPrm->u4PicH/16); 
        u4ECBufSize = ((u4ECBufSize + 4095)/4096)*4096;
		
		u4PicWidthMB = (prDecPrm->u4PicW / 16) ;
		u4PicHeigthMB = (prDecPrm->u4PicH / 16);
		u4OffsetByte = (u4PicWidthMB / 32);
		u4OffsetBit = (u4PicWidthMB % 32);
		u4MaskBit = (1 << u4OffsetBit);
		for (u4Idx = 0; u4Idx < u4PicHeigthMB; u4Idx++)
		{			
			u4Addr = prDecPrm->rLAEDecParam.u4ECAddr + u4Idx*32 + (u4OffsetByte << 2);
			u4Data = *(UINT32*)VIRTUAL(u4Addr);	
			u4Data = (u4Data & (~u4MaskBit));	
			*(UINT32*)VIRTUAL(u4Addr) = u4Data;			
			//HalFlushInvalidateDCacheMultipleLine(VIRTUAL(u4Addr), sizeof(UINT32));
		}
		HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prDecPrm->rLAEDecParam.u4ECAddr), u4ECBufSize);
	}
	#endif
#endif

#ifdef AVC_MV_BUF_REDUCE
    #ifdef IS_POST_MT5368
    if (prDecPrm->prSPS && prDecPrm->prSPS->fgDirect8x8InferenceFlag)
    #else
    if (prDecPrm->prSPS &&
            (prDecPrm->prSPS->fgFrameMbsOnlyFlag && (!prDecPrm->prSPS->fgMbAdaptiveFrameFieldFlag)))
    #endif
    {
        vVDecWriteAVCMV(u4VDecID, RW_AMV_MV_BUF_MODE, u4VDecReadAVCMV(u4VDecID, RW_AMV_MV_BUF_MODE) | SIZE_REDUCE);
    }
#endif

    x_sema_lock(_hVdecFinSema[u4VDecID],X_SEMA_OPTION_NOWAIT);

#ifdef VDEC_SIM_DUMP
    LOG(0, "Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
    LOG(0, "%d, _VDEC_DecProc End\n", u4VDecID);
#endif
    *pi4RetValue = 0;
}


// **************************************************************************
// Function : void _VDEC_LAEDecProc(void);
// Description : Init Video LAE Decoder
// Parameter : None
// Return      :
// **************************************************************************
void _VDEC_LAEDecProc(UINT32 u4VDecID, const VDEC_AVC_DecPrm* prDecPrm, INT32* pi4RetValue)
{
    UINT32 u4SHDRInfo;
#if defined(IS_POST_MT5395) && defined(CC_DRIVER_DEMO)
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
#endif

    ASSERT(pi4RetValue != NULL);
    ASSERT(prDecPrm != NULL);
    
#ifdef VDEC_SIM_DUMP
    LOG(0, "_VDEC_LAEDecProc Start\n");
#endif

    //prepare slice header info, part II
    if (prDecPrm->prSliceHdr)
    {
        u4SHDRInfo = (UINT32)((prDecPrm->prSliceHdr->i4SliceQpDelta) & 0x7f);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4DisableDeblockingFilterIdc & 0x3) << 7);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->i4SliceAlphaC0OffsetDiv2 & 0xf) << 9);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->i4SliceBetaOffsetDiv2 & 0xf) << 13);
        u4SHDRInfo |= ((prDecPrm->u1NalRefIdc & 0x3) << 17);
        u4SHDRInfo |= ((UINT32)(fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr)) << 19);
        u4SHDRInfo |= ((prDecPrm->prSliceHdr->u4CabacInitIdc & 0x3) << 20);
        _VDEC_SetSHDRAVLD2(u4VDecID, u4SHDRInfo);
    }
    if (prDecPrm->prPPS->fgEntropyCodingModeFlag) // CABAC only
    {
        vVDecWriteAVCVLD(u4VDecID, RW_AVLD_INIT_CTX_SRAM, 0x1);
    }

#ifdef CC_DRIVER_DEMO
#if defined(IS_POST_MT5395)
    //input
    if (prVdecEsInfoKeep)
    {
        HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prVdecEsInfoKeep->rPesInfo.u4FifoStart),
                                             prVdecEsInfoKeep->rPesInfo.u4FifoEnd - prVdecEsInfoKeep->rPesInfo.u4FifoStart);
    }
    //output
    HalFlushInvalidateDCacheMultipleLine(VIRTUAL((UINT32)prDecPrm->pu1DecWorkBufY),
                                         ((UINT32)prDecPrm->pu1DecWorkBufC - (UINT32)prDecPrm->pu1DecWorkBufY) << 1);
#else
    HalFlushInvalidateDCache();
#endif
#endif

#ifdef IS_POST_MT5368
    //set total mbs in pic
    if (prDecPrm->prSPS && prDecPrm->prSliceHdr)
    {
        if ((!prDecPrm->prSPS->fgFrameMbsOnlyFlag) && (!prDecPrm->prSliceHdr->fgFieldPicFlag))
        {
            vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC,
                            ((prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) << 1) *
                            (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1));
        }
        else
        {
            vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_TOTAL_MBS_IN_PIC,
                            (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1) *
                            (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1));
        }
    }
#endif
    if(prDecPrm->fgIsMMPlay)
    {
        vVDecWriteVLDTOP(u4VDecID, RO_VLD_TOP_TIMEOUT_THD, VLD_TOP_TIMEOUT_THD);//for 4k2k decode timeout
    }
    
    //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, 0xa89ec007);
    
    vVDecWriteVLDTOP(u4VDecID, AVC_VLD_TOP_PIC_PIX_SIZE, ((((prDecPrm->u4PicW) & 0xffff) << 0 ) | (((prDecPrm->u4PicH) & 0xffff) << 16 )));  
    if (prDecPrm->fgIs10BitMode)
    {
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_BIT_DEPTH, 0x22);
        vVDecWriteMC(u4VDecID, RW_AVC_MC_CMPTMODE, 1);
        vVDecWritePP(u4VDecID, AVC_EXT_BIT_DRAM_MODE, 1);
    }
    //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_CTRL, (u4VDecReadAVCVLD(u4VDecID,RW_AVLD_CTRL) | 0x86000000));
     
#ifdef VDEC_SIM_DUMP
    LOG(0, "[H264-LAE] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
    LOG(0, "_VDEC_LAEDecProc End\n");
#endif
    *pi4RetValue = 0;
}


// **************************************************************************
// Function : void vVDEC_HAL_H264_TriggerDualCore(UINT32 u4VDecID);
// Description :trigger dual core decoded start
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void _VDEC_TriggerDualCore(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm)
{
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, vVDEC_HAL_H264_TriggerDualCore start\n", u4VDecID);
#endif  
#ifdef VDEC_H264_PERFORMANCE
    vVDecWriteVLDTOP(u4VDecID, (21 * 4), 1);
#endif
#ifdef VDEC_H264_CRC_COMPARE
	vVDecWriteMCORETOP(u4VDecID, MCORE_CRC_EN, 0x1);
#endif	
    vVDecConfigMultiCore(u4VDecID, 0);
    vVDecWriteMCORETOP(u4VDecID, RW_MCORE_TriggerDecode, u4VDecReadMCORETOP(u4VDecID,RW_MCORE_TriggerDecode) | 0x01);
#ifdef VDEC_SIM_DUMP
    LOG(0, "%d, vVDEC_HAL_H264_TriggerDualCore end\n", u4VDecID);
#endif
}


// **************************************************************************
// Function : void _VDEC_TriggerDecode(UINT32 u4VDecID);
// Description :trigger single core decoded start
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void _VDEC_TriggerDecode(UINT32 u4VDecID)
{   
#ifdef VDEC_H264_PERFORMANCE
    vVDecWriteVLDTOP(u4VDecID, (21 * 4), 1);
#endif
#ifdef VDEC_H264_CRC_COMPARE
	vVDecWriteMISC(u4VDecID, VDEC_CRC_REG_EN, u4VDecReadMISC(u4VDecID, VDEC_CRC_REG_EN)|0x00000010);
#endif
    vVDecWriteAVCVLD(u4VDecID, RW_AVLD_PROC, 0x1);
}


// **************************************************************************
// Function : void _VDEC_DoRefPicMarking(void);
// Description : do reference picture marking (adaptive mmco / sliding window)
// Parameter : rDecPrm
// Return      :
// **************************************************************************
void _VDEC_DoRefPicMarking(VDEC_AVC_DecPrm* prDecPrm)
{
    VDEC_FBUF_INFO_T* prFBufInfo;
    UINT8 u1ViewIdx;

    ASSERT(prDecPrm != NULL);

    prFBufInfo = prDecPrm->prCurrFBufInfo;
    ASSERT(prFBufInfo != NULL);

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "DoMarking ViewIdx Err!\n");
        return;
    }
    prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW = prDecPrm->u4PicW;
    prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH = prDecPrm->u4PicH;

    //Marking procedure
    if (fgIsRefPic(prDecPrm->u1NalRefIdc))
    {
        if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr)) // IDR pic
        {
            //if(tMpvDecPrm->tSliceHdr->fgNoOutputOfPriorPicsFlag) // clear all prior pic in DPB
            {
                //vFlushBufRefInfo();
            }
            if (prDecPrm->prSliceHdr->fgLongTermReferenceFlag)
            {
                _vSetPicRefType(prFBufInfo, prDecPrm->u1PicStruct, LREF_PIC);
                prDecPrm->u1MaxLongTermFrameIdx = 0;
                prFBufInfo->u4LongTermFrameIdx = 0;
                prFBufInfo->u4TFldLongTermFrameIdx = 0;
                prFBufInfo->u4BFldLongTermFrameIdx = 0;
            }
            else
            {
                prDecPrm->u1MaxLongTermFrameIdx = 0xff;
                _vSetPicRefType(prFBufInfo, prDecPrm->u1PicStruct, SREF_PIC);
            }
        }
        else // !IDR pic
        {
            if (prDecPrm->prSliceHdr->fgAdaptiveRefPicMarkingModeFlag)
            {
                _vAdapRefPicmarkingProce(prDecPrm);
            }
            else
            {
                _VDEC_SlidingWindowProce(prDecPrm);
            }
            if (_bGetPicRefType(*prFBufInfo, prDecPrm->u1PicStruct) != LREF_PIC)
            {
                _vSetPicRefType(prFBufInfo, prDecPrm->u1PicStruct, SREF_PIC);
            }
        }
    }

    prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5 = prDecPrm->prSliceHdr->fgMmco5;
    prDecPrm->rLastInfo[u1ViewIdx].u1LastPicStruct = prDecPrm->u1PicStruct;
    if (prDecPrm->rLastInfo[u1ViewIdx].fgLastMmco5)
    {
        prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum = 0;
        prDecPrm->rLastInfo[u1ViewIdx].i4LastFrameNumOffset = 0;
    }
    else
    {
        prDecPrm->rLastInfo[u1ViewIdx].u4LastFrameNum = prDecPrm->prSliceHdr->u4FrameNum;
        prDecPrm->rLastInfo[u1ViewIdx].i4LastFrameNumOffset = prDecPrm->i4FrmNumOffset;
    }
    prDecPrm->rLastInfo[u1ViewIdx].i4LastPOC =  prFBufInfo->i4POC;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastTFldPOC = prFBufInfo->i4TFldPOC;
    prDecPrm->rLastInfo[u1ViewIdx].i4LastBFldPOC = prFBufInfo->i4BFldPOC;

    if (fgIsRefPic(prDecPrm->u1NalRefIdc))
    {
        prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOC = prFBufInfo->i4POC;
        //tMpvDecPrm->tLastInfo.iLastRefPOCCntLsb = tMpvDecPrm->tSliceHdr->iPicOrderCntLsb;
        //tMpvDecPrm->tLastInfo.iLastRefPOCCntMsb = tMpvDecPrm->tSliceHdr->iPicOrderCntMsb;
        prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCLsb = prDecPrm->prSliceHdr->i4PicOrderCntLsb;
        prDecPrm->rLastInfo[u1ViewIdx].i4LastRefPOCMsb = prDecPrm->prSliceHdr->i4PicOrderCntMsb;
    }
}


// **************************************************************************
// Function : void _VDEC_OutputDispPic(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm, INT32* pi4RetValue);
// Description : put decode frame to displayQ if it can be show
// Parameter : u4VDecID : hw id, 
// Return      : none
// **************************************************************************
void _VDEC_OutputDispPic(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm, INT32* pi4RetValue)
{
    UINT32 i;
    INT32 i4MinPOC, i4DispPOC = 0x7fffffff;
    UINT32 u4MinPOCFBufIdx = 0, u4DispIdx = 0, u4YAddr, u4CAddr;
#ifdef CC_IPTV_SUPPORT
    BOOL fgTimeOut = FALSE;
#endif
#ifdef FBM_DPB_CTRL
#ifndef AVC_IRQ_DISABLE
    UINT32 u4TmpResult;
#endif
#endif

    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo = NULL;
    STC_CLOCK rStcClk;
#ifdef FBM_DPB_CTRL
    FBM_PIC_HDR_T * prFbmPicHdr = NULL;
    FBM_PIC_HDR_T * prFbmPicHdr1;
#endif
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    UCHAR ucFbStatus;
    MPV_EXT_INFO_T *prExtInfo;
    MPV_USERDATA_CB_T rUsrData;
    UINT8 u1ViewIdx;
#ifdef CC_SEI_USER_DATA_ENABLE
    MPV_CC_T *prMpvCc;
    CC_INFO_T *prCcInfo;
    INT32 i4Ret;
#endif
    UINT16 status;

    ASSERT(prDecPrm != NULL);
    if (prDecPrm == NULL)
    {
        LOG(2, "prDecPrm Null!!\n");
        return;
    }
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);

    ASSERT(prVdecEsInfo != NULL);
    if (prVdecEsInfo == NULL)
    {
        LOG(2, "prVdecEsInfo Null!!\n");
        return;
    }

    ASSERT(prVdecEsInfoKeep != NULL);
    if (prVdecEsInfoKeep == NULL)
    {
        LOG(2, "prVdecEsInfoKeep Null!!\n");
        return;
    }

    u1ViewIdx = prDecPrm->u1CurViewIdx;
    if (u1ViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "OutDisp ViewIdx Err!\n");
        return;
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }

    prExtInfo = (MPV_EXT_INFO_T*)_MPV_GetExtInfo();
    x_memset((void*)&rUsrData, 0, sizeof(MPV_USERDATA_CB_T));

    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));
    if (prDecPrm->ucEsId < MAX_ES_NUM)
    {
        prInfo = &(_prDbase->rMpv[prDecPrm->ucEsId]);
    }

    ASSERT(prDecPrm != NULL);
    ASSERT(pi4RetValue != NULL);
    ASSERT(prDecPrm->u4MaxFBufNum <= 18);

    //set if need wait decode done sema in _VDEC_WaitDecDone
    prDecPrm->bWaitDecLock = TRUE;
    prDecPrm->fgDispCurDecPic = FALSE;

    //change dpb status
#ifdef FBM_DPB_CTRL
    if (fgIsIDRPic(prDecPrm->u1NalUnitType) || (!prDecPrm->fgNalNonIdr))
    {
        prDecPrm->i4LastPOC[u1ViewIdx] = -99999;
    }
#endif
    // Check if DPB full
    i4MinPOC = 0x7fffffff;
    for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
    {
        if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus == NO_PIC)
        {
            i4MinPOC = 0x7fffffff;
            u4MinPOCFBufIdx = i;
            break;
        }
        // miew: need to take care of field empty
        else if ((i4MinPOC > prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC) &&
                 (fgIsNonRefFBuf(prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufRefType, prDecPrm->arFBufInfo[u1ViewIdx][i].u1TFldRefType,
                                 prDecPrm->arFBufInfo[u1ViewIdx][i].u1BFldRefType) || prDecPrm->fgPOCWrap))
        {
            i4MinPOC = prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC;
            u4MinPOCFBufIdx = i;
            if ((prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS) || (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV))
            {
                break;
            }
        }
    }
    LOG(7,"MinP = 0x%x,Idx = 0x%x\n",i4MinPOC,u4MinPOCFBufIdx);

    if (prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME)
    {
        //wrap around case, POC disorder
        if (((i4MinPOC < prDecPrm->i4LastPOC[u1ViewIdx]) &&
                (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId)
                 == FBM_FB_STATUS_DECODE)) &&
                ((!prDecPrm->fgIsMMPlay)
#ifdef ENABLE_MULTIMEDIA
                 || ((prVdecEsInfo->eContainerType >= SWDMX_FMT_MPEG2_TS) && (prVdecEsInfo->eContainerType <= SWDMX_FMT_H264_VIDEO_ES))
                 || prVdecEsInfoKeep->fgVPush
#endif
                )
#ifdef ENABLE_MULTIMEDIA
                && (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_IPTV)
#endif
                )   //start point?
        {
            LOG(2, "Warning--1--! POC %d < last POC %d, FlushDPB\n", i4MinPOC, prDecPrm->i4LastPOC[u1ViewIdx]);
            if (!prDecPrm->fgIsMVC)
            {
                _VDEC_FlushDPB(prDecPrm);
                prDecPrm->i4LastPOC[u1ViewIdx] = -99999;
                if (prDecPrm->fgIsMMPlay)
                {
                    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
                    SET_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
                }
                else
                {
                    prDecPrm->prSPS->fgSPSValid = FALSE;
                }
                return;
            }
        }
        //set cur decoded pic from decode state to ready state
#ifdef VDEC_TIME_PROFILE
        prDecPrm->u4CurWaitFbMicro = 0;
#endif
#ifdef FBM_DPB_CTRL
        if (prDecPrm->u1RptFrm > 0)
        {
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_DISPLAYQ);
            prVdecEsInfo->u4DisplayQPicCnt++;
            *pi4RetValue = (INT32)E_VDEC_OK;
            return;
        }
        else if (prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
        {
#ifdef CC_REALD_3D_SUPPORT
            _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId,
                                  prDecPrm->prCurrFBufInfo->ucFbId);
#endif
            _VDEC_JobBeforePutDispQ(prDecPrm->ucEsId,
                                    prDecPrm->prCurrFBufInfo->ucFbId);
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_DISPLAYQ);
            prVdecEsInfo->u4DisplayQPicCnt++;
            _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx + ((UINT32)u1ViewIdx << 8));
            if (prVdecEsInfoKeep && prVdecEsInfoKeep->pfDecNfyFct)
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_I_FRAME_DONE, 0, 0);
            }
            prDecPrm->i4LastPOC[u1ViewIdx] = -99999;
            *pi4RetValue = (INT32)E_VDEC_OK;
            return;
        }
        else
        {
            if (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId)
                    == FBM_FB_STATUS_DECODE)
            {
                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId);
                if (prFbmPicHdr)
                {
                    prFbmPicHdr->i4TemporalRef = prDecPrm->prCurrFBufInfo->i4POC;
                }

                //decoding order= display order
                if((prDecPrm->u1PlayModeInDtv == IMode) ||(prDecPrm->fgIsMMPlay 
                  #ifdef ENABLE_MULTIMEDIA 
                    && (((prDecPrm->ucSkipMode == IMode)
                     && ((prVdecEsInfo->eContainerType< SWDMX_FMT_MPEG1_DAT) 
                       ||(prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS_TIME_SHIFT)
                       ||(prVdecEsInfo->eContainerType> SWDMX_FMT_VC1_WMV)))
                     || (prVdecEsInfo->eDecodeMode == VDEC_DECODE_MODE_NO_REORDER)
                     || ((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE) 
                       || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY) 
                       || (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HW_DIRECT_PLAY)
                       || ((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV) && 
                            ((prDecPrm->ucSkipMode == IMode) || (prDecPrm->ucSkipMode == IPMode)))
                       || (prDecPrm->prSPS && (prDecPrm->prSPS->u4PicOrderCntType == 2)))
                    )
                  #endif
                  )
                  )
                {
                #ifdef ENABLE_MULTIMEDIA
                    #ifndef AVC_IRQ_DISABLE
                    if (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
                    {
                        if (x_sema_lock_timeout(_hVdecFinSema[u4VDecID], 30) != OSR_OK)
                        {
                            LOG(2, "%s: SWDMX_SRC_TYPE_WIFI_DISPLAY x_sema_lock_timeout\n", __FUNCTION__);
                        }
                    }
                    else
                    {
                        if (x_sema_lock_timeout(_hVdecFinSema[u4VDecID], 500) != OSR_OK)
                        {
                            #ifdef CC_IPTV_SUPPORT
                            fgTimeOut = TRUE;
                            #endif
                            LOG(2, "%s: x_sema_lock_timeout\n", __FUNCTION__);
                        }
                        #ifdef CC_IPTV_SUPPORT
                        if(prVdecEsInfo && (fgTimeOut == TRUE))
                        {
                            prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                            fgTimeOut = FALSE;
                        }
                        #endif
                    }
                    prDecPrm->bWaitDecLock = FALSE;
                    u4TmpResult = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_ERR_ACC) & ERR_MASK_OBSV;
                    if(((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV)&&(u4TmpResult != 0))
                        || ((prDecPrm->u1PlayModeInDtv != IMode) && (u4TmpResult != 0) && 
                            (prVdecEsInfo->eContainerType> SWDMX_FMT_MPEG2_TS) && 
                            (prVdecEsInfo->eContainerType<= SWDMX_FMT_H264_VIDEO_ES)))
                    {
                        prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                        #if BDP_LOG_STYLE
                        if (prFbmPicHdr)
                        {
                            VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                         "[H264 VDEC%d]: _VDEC_OutputDispPic IMode PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                         prDecPrm->ucEsId,
                                         prDecPrm->prCurrFBufInfo->i4PicNum, 
                                         prVdecEsInfoKeep->rPesInfo.ucPicType,
                                         prFbmPicHdr->u4PTS);
                        }
                        #endif                    
                    }
                    #endif
                #endif
                    #ifdef CC_IPTV_SUPPORT
                    UNUSED(fgTimeOut);
                    #endif
                    
                    if (prFbmPicHdr == NULL)
                    {
                        LOG(2, "OutPic prFbmPicHdr NULL\n");
                    }
                    else
                    {
                        prVdecEsInfo->u4PTS = prFbmPicHdr->u4PTS;

                        //for skype particular camera issue
                        #ifdef ENABLE_MULTIMEDIA
                        if (((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE) ||
                            (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HW_DIRECT_PLAY))&&
                                (prDecPrm->prCurrFBufInfo->i4POC != 0) && (prDecPrm->prCurrFBufInfo->i4POC == prDecPrm->i4LastPOC[u1ViewIdx]))
                        {
                            prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                            #if BDP_LOG_STYLE                           
                            if (prFbmPicHdr)
                            {
                                VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                             "[H264 VDEC%d]: _VDEC_OutputDispPic IMode2 PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                             prDecPrm->ucEsId,
                                             prDecPrm->prCurrFBufInfo->i4PicNum, 
                                             prVdecEsInfoKeep->rPesInfo.ucPicType,
                                             prFbmPicHdr->u4PTS);
                            }
                            #endif        
                        }
                        if((prDecPrm->prSPS && (prDecPrm->prSPS->u4PicOrderCntType == 2))||
                            (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_IPTV) ||
                            (((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE) ||
                            (prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_HW_DIRECT_PLAY)) &&
                           (prVdecEsInfoKeep->fgVPush)))
                        {
                            if(!prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u1DecFBufIdx].fgPTSValid)
                            {
                                prFbmPicHdr->u4PTS = prDecPrm->au4DispPts[u1ViewIdx];
                                prFbmPicHdr->u8PTS = prDecPrm->au8DispPts[u1ViewIdx];
                            }
                            else
                            {
                                prDecPrm->au4DispPts[u1ViewIdx] = prFbmPicHdr->u4PTS;
                                prDecPrm->au8DispPts[u1ViewIdx] = prFbmPicHdr->u8PTS;
                            }
                            if(prDecPrm->u4Rate > 0)
                            {
                                prDecPrm->au4DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale + prDecPrm->u8VRemainder),
                                    prDecPrm->u4Rate, &prDecPrm->u8VRemainder) * (prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u1DecFBufIdx].u4SkipFrmCnt + 1));
                            }
                            if(prDecPrm->u4Rate64 > 0)
                            {
                                prDecPrm->au8DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale64 + prDecPrm->u8VRemainder64),
                                    prDecPrm->u4Rate64, &prDecPrm->u8VRemainder64) * (prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u1DecFBufIdx].u4SkipFrmCnt + 1));
                            }
                        }
                        #endif

                        prVdecEsInfo->u4PTS = prFbmPicHdr->u4PTS;
                        prVdecEsInfo->u8PTS = prFbmPicHdr->u8PTS;
                        
                        if (prDecPrm->prCurrFBufInfo->fgIsNotDisp)
                        {
                            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
                                                     FBM_FB_STATUS_EMPTY);
                        }
                        else
                        {
                            if (VDEC_IsNotDisplay(prDecPrm->ucEsId, prFbmPicHdr->u4PTS,
                                                  prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset))
                            {
                                FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
                                                         FBM_FB_STATUS_EMPTY);
                            }
                            else
                            {
                                #ifdef CC_REALD_3D_SUPPORT
                                _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId,
                                                      prDecPrm->prCurrFBufInfo->ucFbId);
                                #endif
                                #ifdef ENABLE_MULTIMEDIA
                                if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (prDecPrm->ucSkipMode == IMode))
                                {
                                    prFbmPicHdr->fgPushModeTrick = TRUE;
                                }                            
                                if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (TRUE == prVdecEsInfoKeep->rPesInfo.fgBandwidthChanged))
                                {
                                    prFbmPicHdr->fgBandwidthChanged = TRUE;
                                    LOG(0, "Set FbmPicHdr BandwidthChanged TRUE\n");
                                } 
                                else if(prVdecEsInfo && prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_WIFI_DISPLAY)
                                {
                                    if((FBM_ChkWfdFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_WFD_FRAME_MONITOR)))
                                    {
                                        if(!(FBM_ChkWfdFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_WFD_FRAME_COUNTING)))
                                        {
                                            FBM_SetWfdFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_WFD_FRAME_COUNTING);
                                        }
                                        else
                                        {
                                            FBM_AccumulateWfdFrameCnt(prDecPrm->aucFbgId[u1ViewIdx]);
                                        }
                                    }
                                }
                                if (_VDEC_IsxvYCC(prDecPrm))
                                {
                                    prFbmPicHdr->eColorSpace = ColorSpace_XVColor;
                                }
                                else
                                {
                                    prFbmPicHdr->eColorSpace = ColorSpace_Normal;
                                }
                                #endif
                                _VDEC_JobBeforePutDispQ(prDecPrm->ucEsId,
                                                        prDecPrm->prCurrFBufInfo->ucFbId);
                                FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId,
                                                         FBM_FB_STATUS_DISPLAYQ);
                                #ifdef TIME_MEASUREMENT                                
                                if (TRUE == prVdecEsInfoKeep->rPesInfo.fgWFDTMSLogEnable)
                                {        
                                    HAL_GetTime(&prFbmPicHdr->rTimePutDispQ);
                                    TMS_DIFF_EX(TMS_FLAG_WFD_LATENCY, "WFD_DATA:", "Vdec Put DispQ");    
                                    prFbmPicHdr->fgWFDTMSLogEnable = TRUE;
                                }
                                #endif
                                prFbmPicHdr->u8_vid_pts = prVdecEsInfoKeep->rPesInfo.u8_vid_pts;                                
                                prFbmPicHdr->u8_push_time = prVdecEsInfoKeep->rPesInfo.u8_push_time;
								#ifdef __KERNEL__
                                if(prVdecEsInfoKeep->rPesInfo.u8_vid_pts != 0)                                
                                {                                    
                                    UINT64 u8CurTime = 0;                                    
                                    UINT64 u8Remainder = 0;                                    
                                    struct timeval tv = {0};                                                                        

                                    do_gettimeofday(&tv);                                                                        
                                    u8CurTime = (UINT64)tv.tv_sec * 1000000;                                                    
                                    u8CurTime = u8CurTime + (UINT64)tv.tv_usec;                                    
                                    u8CurTime = u8Div6432(u8CurTime, 1000, &u8Remainder);                                                                        
                                    LOG(2, "[VDEC][%s]PTS(0x%llx),Time[%llu],CurTime[%llu],Cost[%llu]\n", __FUNCTION__,                                         
                                        prFbmPicHdr->u8_vid_pts, prFbmPicHdr->u8_push_time, u8CurTime, u8CurTime - prFbmPicHdr->u8_push_time);                                
                                }
								#endif
                                LOG(6, "PutQ 0x%x\n", prFbmPicHdr->u4PTS);
                                
                                prVdecEsInfo->u4DisplayQPicCnt++;
                                prVdecEsInfo->fgRVUSkip = FALSE;
                            }
                            
                            UNUSED(VDEC_CheckStopStatus(prDecPrm->ucEsId, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));
                            prDecPrm->i4LastPOC[u1ViewIdx] = prDecPrm->prCurrFBufInfo->i4POC;
                        }
                    }
                }
                else
                {
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_READY);
                    LOG(9, "POC %d, FbId %d ready\n", prDecPrm->prCurrFBufInfo->i4POC, prDecPrm->prCurrFBufInfo->ucFbId);
                }
            }
            else if ((!prDecPrm->fgIsMVC) && (prVdecEsInfo->e3DType != VDEC_3D_REALD_DUAL_STREAM))
            {
                LOG(3, "Err!Try to set FB from %d to ready\n",
                    (UINT32)FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->prCurrFBufInfo->ucFbId));
            }
        }
#endif
    }

    // No free FBuf, output 1 fbuf is needed
    if ((prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].u1FBufStatus != NO_PIC)
#ifdef FBM_DPB_CTRL
            && (prDecPrm->prCurrFBufInfo->u1FBufStatus == FRAME)
#endif
       )
    {
        prDecPrm->u1MinPOCBufIdx = (UINT8)u4MinPOCFBufIdx;

#ifdef FBM_DPB_CTRL
        //FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId, FALSE);
        LOG(9, "<");
        do
        {
            i4DispPOC = i4MinPOC;
            u4DispIdx = u4MinPOCFBufIdx;
            for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
            {
                if ((i4DispPOC > prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC) &&
                        (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][i].ucFbId)
                         == FBM_FB_STATUS_READY) &&
                        (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus == FRAME))
                {
                    i4DispPOC = prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC;
                    u4DispIdx = i;
                }
            }
            LOG(7,"DispP = %d,Idx = %d,fbid = %d\n",i4DispPOC,u4DispIdx,prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId);
            if (FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId)
                    == FBM_FB_STATUS_READY)
            {
                FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                        ((i4DispPOC != i4MinPOC) ? TRUE : FALSE));

                    if (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId == prDecPrm->prCurrFBufInfo->ucFbId)
                    {
                        //VERIFY(x_sema_lock(_hVdecFinSema, X_SEMA_OPTION_WAIT) == OSR_OK);    //wait decode done
                        if (x_sema_lock_timeout(_hVdecFinSema[u4VDecID], 500) != OSR_OK)
                        {
                            LOG(3, "%s: x_sema_lock_timeout\n", __FUNCTION__);
                        }
                        prDecPrm->bWaitDecLock = FALSE;
                        prDecPrm->fgDispCurDecPic = TRUE;
                        
                        u4TmpResult = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_ERR_ACC) & ERR_DISP_MASK;
                        if(!prDecPrm->fgIsMMPlay && u4TmpResult)
                        {
                            prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
                            #if BDP_LOG_STYLE
                            if (prFbmPicHdr)
                            {
                                VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                                             "[H264 VDEC%d]: _VDEC_OutputDispPic VLD ERR PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                                             prDecPrm->ucEsId,
                                             prDecPrm->prCurrFBufInfo->i4PicNum, 
                                             prVdecEsInfoKeep->rPesInfo.ucPicType,
                                             prFbmPicHdr->u4PTS);
                            }
                            #endif        
                            LOG(0,"Drop current picture!\n");
                        }
                    }

                //calculate pts in decoder if AVI file
                prFbmPicHdr = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId);
                if (prFbmPicHdr == NULL)
                {
                    LOG(2, "OutPic prFbmPicHdr NULL\n");
                    break;
                }
                if (prDecPrm->fgIsMMPlay)
                {
                    if (!prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgPTSValid)
                    {
                        prFbmPicHdr->u4PTS = prDecPrm->au4DispPts[u1ViewIdx];
                        prFbmPicHdr->u8PTS = prDecPrm->au8DispPts[u1ViewIdx];
                    }
                    else
                    {
                        prDecPrm->au4DispPts[u1ViewIdx] = prFbmPicHdr->u4PTS;
                        prDecPrm->au8DispPts[u1ViewIdx] = prFbmPicHdr->u8PTS;
                    }
                    if (prDecPrm->u4Rate > 0)
                    {
                        prDecPrm->au4DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale + prDecPrm->u8VRemainder),
                            prDecPrm->u4Rate, &prDecPrm->u8VRemainder) * (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4SkipFrmCnt + 1));
                    }
                    if (prDecPrm->u4Rate64 > 0)
                    {
                        prDecPrm->au8DispPts[u1ViewIdx] += ((UINT32)u8Div6432((prDecPrm->u8PtsScale64 + prDecPrm->u8VRemainder64),
                            prDecPrm->u4Rate64, &prDecPrm->u8VRemainder64) * (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4SkipFrmCnt + 1));
                    }
                }
                if ((prDecPrm->i4LastPOC[u1ViewIdx] > i4DispPOC)
                    #ifdef ENABLE_MULTIMEDIA
                    &&(!prVdecEsInfoKeep->fgVPush || 
                    (!((prVdecEsInfo->eContainerType == SWDMX_FMT_MPEG2_TS)&&(prDecPrm->ucSkipMode == IMode))&& prVdecEsInfoKeep->fgVPush))
                    #endif
                    )  //temp before FBM modification ready
                {
                    LOG(2, "Warning--2---! POC %d < last POC %d\n", i4DispPOC, prDecPrm->i4LastPOC[u1ViewIdx]);
                    prDecPrm->i4LastPOC[u1ViewIdx] = i4DispPOC;
                    FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                             FBM_FB_STATUS_EMPTY);
                    if (prInfo)
                    {
                        prInfo->u4VpsrDropNs++;
                    }
                }
                else if ((prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u1FBufStatus != FRAME) ||
                         (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgIsNotDisp))  //field empty, or reference error frame
                {
                    if (prDecPrm->fgIsMVC && (prDecPrm->ucSkipMode == IMode) && !prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgIsNotDisp)
                    {
                        if (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u1FBufStatus == TOP_FIELD)
                        {
                            prFbmPicHdr->fgTrickTopField = TRUE;
                        }
                        else
                        {
                            prFbmPicHdr->fgTrickTopField = FALSE;
                        }
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                                 FBM_FB_STATUS_DISPLAYQ);
                    }
                    else
                    {
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                                 FBM_FB_STATUS_EMPTY);
                        if (prInfo)
                        {
                            prInfo->u4VpsrDropNs++;
                        }
                    }
                }
                else
                {
                    //notify audio start to play
                    if(!prDecPrm->fgStartPlay || !prVdecEsInfo->fgSetStartPts)
                    {
                        if (prDecPrm->fgIsMMPlay)
                        {
                            prDecPrm->fgStartPlay = TRUE;
                            //prVdecEsInfo->fgSetStartPts = TRUE;
                        }
                        else
                        {
                            if (_bNotifyAudStartPlay(prDecPrm->ucStcId, prFbmPicHdr->u4PTS))
                            {
                                LOG(4,"Set start PTS 0x%x\n",prFbmPicHdr->u4PTS);
                                prDecPrm->fgStartPlay = TRUE;
                                prVdecEsInfo->fgSetStartPts = TRUE;
                            }
                        }
                    }
                    // mm checking
                    prVdecEsInfo->u4PTS = prFbmPicHdr->u4PTS;
                    if (prDecPrm->fgIsMMPlay && VDEC_IsNotDisplay(/*prDecPrm->ucEsId*/
                                prVdecEsInfoKeep->rPesInfo.ucEsId, prFbmPicHdr->u4PTS,
                                prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset))
                    {
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                                 FBM_FB_STATUS_EMPTY);
                    }
                    else
                    {
                        #ifdef ENABLE_MULTIMEDIA
                        if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (prDecPrm->ucSkipMode == IMode))
                        {
                            prFbmPicHdr->fgPushModeTrick = TRUE;
                        }                        
                        if((prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_DASH) && (TRUE == prVdecEsInfoKeep->rPesInfo.fgBandwidthChanged))
                        {
                            prFbmPicHdr->fgBandwidthChanged = TRUE;
                            LOG(0, "Set FbmPicHdr BandwidthChanged TRUE\n");
                        } 
                        if (_VDEC_IsxvYCC(prDecPrm))
                        {
                            prFbmPicHdr->eColorSpace = ColorSpace_XVColor;
                        }
                        else
                        {
                            prFbmPicHdr->eColorSpace = ColorSpace_Normal;
                        }
                        #endif
                        //log put dispQ timing
                        STC_GetSrc(0, &rStcClk);
                        LOG(6, "PutQ PTS(0x%x), STC(0x%x), POC %d, ViewID %d, FbId %d\n",
                            prFbmPicHdr->u4PTS, rStcClk.u4Base, i4DispPOC, prDecPrm->u1CurViewIdx,
                            prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId);
                        #ifdef TIME_MEASUREMENT
                        if (TRUE == prVdecEsInfo->fgPrintOutput)
                        {
                            TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "Vdec Put First Pic to DisplayQ ");
                            prVdecEsInfo->fgPrintOutput = FALSE;
                        }
                        #endif
                        #if BDP_LOG_STYLE 
                        VDecPrintf(prVdecEsInfoKeep->rPesInfo.ucEsId, VDEC_LOG_VDEC2VDP, 
                                         "[VDEC-H264-VDP%d] PType=0X%08X, PTS(0x%x), STC(0x%x), POC %d, ViewID %d, FbId %d\n",
                                            prVdecEsInfoKeep->rPesInfo.ucEsId,
                                            prVdecEsInfoKeep->rPesInfo.ucPicType,                        
                                           prFbmPicHdr->u4PTS,
                                           rStcClk.u4Base,
                                           i4DispPOC,
                                           prDecPrm->u1CurViewIdx,
                                        prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId);
                        #endif
						
#ifdef CC_REALD_3D_SUPPORT
                        _VDEC_Check2D3DSwitch(prDecPrm->fgIsMVC ? u1ViewIdx : prDecPrm->ucEsId,
                                              prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId);
#endif
                        _VDEC_JobBeforePutDispQ(prVdecEsInfoKeep->rPesInfo.ucEsId,
                                                prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId);
                        FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                                 FBM_FB_STATUS_DISPLAYQ);
                        prVdecEsInfo->u4DisplayQPicCnt++;
   
                        if (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgFirstFrmTag && prDecPrm->fgIsMVC)   //continuous display from 1st frame
                        {

                            for (i = 0; (i < (prDecPrm->u4MaxFBufNum + 2)) && (i < 18); i++)
                            {
                                status = FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], i);
                                if (status == FBM_FB_STATUS_DISPLAYQ)
                                {
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], i);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        LOG(1, "check %d, %x,poc = %d\n", u1ViewIdx, prFbmPicHdr1->u4PTS, prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC);
                                    }
                                }
                            }
                            for (i = 0; (i < (prDecPrm->u4MaxFBufNum + 2)) && (i < 18); i++)
                            {
                                status = FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[1-u1ViewIdx], i);
                                if (status == FBM_FB_STATUS_DISPLAYQ)
                                {
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[1-u1ViewIdx], i);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        LOG(1, "check %d, %x,poc = %d\n", (1 - u1ViewIdx), prFbmPicHdr1->u4PTS, prDecPrm->arFBufInfo[1-u1ViewIdx][i].i4POC);
                                    }
                                }
                            }
                            for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
                            {
                                if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != NO_PIC)
                                {
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][i].ucFbId);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        LOG(1, "NO_PIC %d,%x,poc = %d\n", u1ViewIdx, prFbmPicHdr1->u4PTS, prDecPrm->arFBufInfo[u1ViewIdx][i].i4POC);
                                    }
                                    prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsNotDisp = FALSE;
                                }
                            }
                            for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
                            {
                                if ((prDecPrm->arFBufInfo[1-u1ViewIdx][i].u1FBufStatus != NO_PIC) &&
                                        (prDecPrm->arFBufInfo[1-u1ViewIdx][i].i4POC >= prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].i4POC))
                                {
                                    prFbmPicHdr1 = FBM_GetFrameBufferPicHdr(prDecPrm->aucFbgId[1-u1ViewIdx], prDecPrm->arFBufInfo[1-u1ViewIdx][i].ucFbId);
                                    if (prFbmPicHdr1 != NULL)
                                    {
                                        LOG(1, "NO_PIC%d, %x,poc=%d\n", (1 - u1ViewIdx), prFbmPicHdr1->u4PTS, prDecPrm->arFBufInfo[1-u1ViewIdx][i].i4POC);
                                    }
                                    prDecPrm->arFBufInfo[1-u1ViewIdx][i].fgIsNotDisp = FALSE;
                                }
                            }
                        }
                        else
                        {
                            if (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgFirstFrmTag)  //continuous display from 1st frame
                            {
                                for (i = 0; (i < prDecPrm->u4MaxFBufNum) && (i < 18); i++)
                                {
                                    if (prDecPrm->arFBufInfo[u1ViewIdx][i].u1FBufStatus != NO_PIC)
                                    {
                                        prDecPrm->arFBufInfo[u1ViewIdx][i].fgIsNotDisp = FALSE;
                                    }
                                }
                            }
                        }
                        prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].fgFirstFrmTag = FALSE;

                        //notify userdata
                        if (prDecPrm->ucEsId < MAX_ES_NUM)
                        {
                            #ifdef CC_SEI_USER_DATA_ENABLE
                            prMpvCc = (MPV_CC_T*)_MPV_GetMpvCc(prDecPrm->ucEsId);
                            if(prMpvCc->fgPlayCc)
                            {
                                prCcInfo = &(prMpvCc->rCcInfo);

                                prCcInfo->fgIsScte= FALSE;
                                prCcInfo->fgBottomFirst = prFbmPicHdr->fgTopFldFirst;
                                prCcInfo->u4Size = prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr;
                                prCcInfo->u8Pts = (UINT64)prFbmPicHdr->u4PTS;
                                prCcInfo->u4BufRptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                                      prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].au1UsrDataFrm,
                                                      prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr);
                                if (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr > 0)
                                {
                                    i4Ret = x_msg_q_send(prMpvCc->hCcQueue,
                                                         (void *)prCcInfo, sizeof(CC_INFO_T), 255);

                                    VERIFY((i4Ret == OSR_OK) || (i4Ret == OSR_TOO_MANY));

                                    if ((prMpvCc->pfCcNfyFct) && (i4Ret == OSR_OK))
                                    {
                                        if (!prDecPrm->fgDispCurDecPic)
                                        {
                                            LOG(4, "atsc Notify Cc pts 0x%x, size %d\n", (UINT32)prCcInfo->u8Pts, prCcInfo->u4Size);
                                            prMpvCc->pfCcNfyFct(prMpvCc->pvCcNfyTag,
                                                                MPV_CC_COND_DATA_ARRIVAL, (void*)(prCcInfo->u4Size));
                                        }
                                        else
                                        {
                                            LOG(4, "atsc Notify Cc DispCurDecPic\n");
                                        }
                                    }
                                }
                            }
#endif
                            if (prExtInfo->pfUserDataCb &&
                                    ((prExtInfo->fgUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr > 0)) ||
                                     (prExtInfo->fgPesUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataPESWptr > 0))))
                            {
                                rUsrData.fgProgressiveFrm = prFbmPicHdr->fgProgressiveFrm;
                                rUsrData.u2TemporalRef = (UINT16)prFbmPicHdr->i4TemporalRef;
                                rUsrData.u4Pts = prFbmPicHdr->u4PTS;
                                rUsrData.ucEsId = prDecPrm->ucEsId;
                                rUsrData.ucPicStruct = prFbmPicHdr->ucPicStruct;
                                rUsrData.ucPicType = prFbmPicHdr->ucPicCdTp;
                                rUsrData.ucRepFirstFld = prFbmPicHdr->fgRepFirstFld;
                                rUsrData.ucTopFldFirst = prFbmPicHdr->fgTopFldFirst;

#ifdef CC_SEI_USER_DATA_ENABLE
                                if (prExtInfo->fgUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr > 0))
                                {
                                    rUsrData.u4Rptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                                                         prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].au1UsrDataFrm,
                                                                         prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr);
                                    rUsrData.ucDataLen = (UCHAR)prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataFrmWptr;

                                //if(!prDecPrm->fgDispCurDecPic)
                                    {   //notify user data
                                        prExtInfo->pfUserDataCb(&rUsrData);
                                    }
                                 /*   else    //current frm user data !ready, wait next AU, save to Q
                                    {
                                        x_msg_q_send(prDecPrm->hUsrDataQ, (void *)&rUsrData,
                                                     sizeof(MPV_USERDATA_CB_T), 255);
                                    }*/
                                }
                                #endif
                                #ifdef CC_DMX_PES_EXT_BUFFER
                                if(prExtInfo->fgPesUserData[prDecPrm->ucEsId] && (prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataPESWptr > 0))
                                {
                                    rUsrData.u4Rptr = _VDEC_SaveUserData(prDecPrm->ucEsId,
                                                                         prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].au1UsrDataPES,
                                                                         prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataPESWptr);
                                    rUsrData.ucDataLen = (UCHAR)prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4UserDataPESWptr;

                                    if (!prDecPrm->fgDispCurDecPic)
                                    {   //notify user data
                                        prExtInfo->pfUserDataCb(&rUsrData);
                                    }
                                    else    //current frm user data !ready, wait next AU, save to Q
                                    {
                                        x_msg_q_send(prDecPrm->hUsrDataQ, (void *)&rUsrData,
                                                     sizeof(MPV_USERDATA_CB_T), 255);
                                    }
                                }
#endif
                            }
                        }
                    }
                    if (prDecPrm->fgIsMMPlay)
                    {
                        prVdecEsInfo->u4PTS = prFbmPicHdr->u4PTS;
                        UNUSED(VDEC_CheckStopStatus(/*prDecPrm->ucEsId*/
                                   prVdecEsInfoKeep->rPesInfo.ucEsId, prFbmPicHdr->u4PTS, prFbmPicHdr->i4TemporalRef));
                    }
                    prDecPrm->i4LastPOC[u1ViewIdx] = i4DispPOC;

                    FBM_GetFrameBufferAddr(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId,
                                           &u4YAddr, &u4CAddr);
                    LOG(9, "Put FbId %d, FrameNum %d, POC %d to DispQ, Y 0x%x, C 0x%x, IsRef %d\n",
                        prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].ucFbId, prDecPrm->arFBufInfo[u1ViewIdx][u4DispIdx].u4FrameNum,
                        i4DispPOC, u4YAddr, u4CAddr, ((i4DispPOC != i4MinPOC) ? 1 : 0));
                    if((prDecPrm->fgIsMMPlay)
                        && (IMode == prDecPrm->ucSkipMode)
                        && !VDEC_IsNotDisplay(/*prDecPrm->ucEsId*/
                                prVdecEsInfoKeep->rPesInfo.ucEsId, prFbmPicHdr->u4PTS,
                                prFbmPicHdr->i4TemporalRef, prFbmPicHdr->u8Offset)
                        #ifdef ENABLE_MULTIMEDIA
                        &&((prVdecEsInfo->eContainerType >= SWDMX_FMT_MPEG1_DAT)
                        && (prVdecEsInfo->eContainerType <= SWDMX_FMT_VC1_WMV))
                        #endif
                    )
                    {
                        if(i4DispPOC != i4MinPOC)
                        {
                            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId,
                                                 FBM_FB_STATUS_EMPTY);
                        }
                        break;
                    }
                }
            }
            else
            {
              LOG(7,"chosen one already showed,poc = %d\n",i4DispPOC);
            }
        }
        while (i4DispPOC != i4MinPOC);
        LOG(9, ">\n");
#endif

        if (prDecPrm->u1RptFrm > 0)
        {
            _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u1DecFBufIdx + ((UINT32)u1ViewIdx << 8));
        }
        else
        {
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
            if ((prDecPrm->u4WaitClrBufIdx < 0xFF) && (u1ViewIdx > 0))
            {
                //LOG(3, "clr posponed buf %d\n", prDecPrm->u4WaitClrBufIdx);
                _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u4WaitClrBufIdx); //should be base view
                prDecPrm->u4WaitClrBufIdx = 0xFF;
            }
            if (prDecPrm->fgIsMVC && (u1ViewIdx == 0) &&
                    (prDecPrm->prCurrFBufInfo->ucFbId == prDecPrm->arFBufInfo[u1ViewIdx][u4MinPOCFBufIdx].ucFbId))
            {
                //LOG(3, "pospone buf clr in base view\n");
                prDecPrm->u4WaitClrBufIdx = u4MinPOCFBufIdx;
            }
            else
#endif
            {
                _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + ((UINT32)u1ViewIdx << 8));
            }
        }
        *pi4RetValue = (INT32)E_VDEC_OK;
    }
    else
    {
        if (prDecPrm->fgOutDispOrder)
        {
            *pi4RetValue = (INT32)E_VDEC_DISP_NOT_READY; //display order
        }
        else
        {
            *pi4RetValue = (INT32)E_VDEC_OK;                 //decode order comparison
        }
    }

    // clear frame buffer if frameNum is duplicated
    if (prDecPrm->u4DupFrmNumId < 18/*0xFF*/)
    {
        ucFbStatus = FBM_GetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u4DupFrmNumId].ucFbId);
        if ((ucFbStatus != FBM_FB_STATUS_DISPLAYQ) && (ucFbStatus != FBM_FB_STATUS_LOCK))
        {
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u4DupFrmNumId].ucFbId,
                                     FBM_FB_STATUS_EMPTY);
        }
        else
        {
            FBM_UpdateReferenceList(prDecPrm->aucFbgId[u1ViewIdx], prDecPrm->arFBufInfo[u1ViewIdx][prDecPrm->u4DupFrmNumId].ucFbId, FALSE);
        }
        if (prDecPrm->fgOutDispOrder)   //skip this in emulation
        {
            _VDEC_ClrFBufInfo(prDecPrm, prDecPrm->u4DupFrmNumId + ((UINT32)u1ViewIdx << 8));
        }
        prDecPrm->u4DupFrmNumId = 0xFF;
        LOG(3, "Clr Dup Frm_Num\n");
    }
}


// **************************************************************************
// Function : void _VDEC_GetRptr(UINT32 u4VDecID, UINT32* pu4Rptr);
// Description : get readpointer value
// Parameter : u4VDecID : hw id, pu4Rptr: read pointer value
// Return      : none
// **************************************************************************
void _VDEC_GetRptr(UINT32 u4VDecID, UINT32* pu4Rptr)
{
    ASSERT(pu4Rptr != NULL);
    *pu4Rptr = _VDEC_VldRPtr(u4VDecID);
}


// **************************************************************************
// Function : void _VDEC_SetWptr(UCHAR ucEsId, UINT32 u4VDecID, UINT32 u4Wptr);
// Description : set write pointer
// Parameter : ucEsId : instamce id, u4VDecID: hw id, u4Wptr: write pointer value
// Return      : none
// **************************************************************************
void _VDEC_SetWptr(UCHAR ucEsId, UINT32 u4VDecID, UINT32 u4Wptr)
{
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (ucEsId == prVdecEsInfoKeep->rPesInfo.ucEsId)
    {
        _VDEC_SetVldWPtr(u4VDecID, u4Wptr, prVdecEsInfoKeep->rPesInfo.u4FifoStart, prVdecEsInfoKeep->rPesInfo.u4FifoEnd);
    }
}


// **************************************************************************
// Function : BOOL _VDEC_SkipFrm(VDEC_AVC_DecPrm* prDecPrm);
// Description : if need to skip frame or not
// Parameter : 
// Return      : return true if skip
// **************************************************************************
BOOL _VDEC_SkipFrm(VDEC_AVC_DecPrm* prDecPrm)
{
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
#ifdef CC_SEI_USER_DATA_ENABLE
    MPV_CC_T* prMpvCc;
    MPV_EXT_INFO_T *prExtInfo;

#endif
    VDEC_ES_INFO_T* prVdecEsInfo;

    ASSERT(prDecPrm != NULL);
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    ASSERT(prVdecEsInfoKeep != NULL);
    
    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));
    if (prDecPrm->ucEsId < MAX_ES_NUM)
    {
        prInfo = &(_prDbase->rMpv[prDecPrm->ucEsId]);
        prInfo->u4EsmSkipNs++;
    }

#ifdef FBM_DPB_CTRL
    if (((prDecPrm->prCurrFBufInfo == NULL) ||
            (prDecPrm->prSliceHdr == NULL)) ||
            (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME) ||
            (!prDecPrm->fgIsFirstFrm) ||
            (prVdecEsInfo->fgRenderFromFirstPic) ||
            (prVdecEsInfo->fgRenderFromPos))
    {
        return FALSE;
    }

    if((prVdecEsInfo->e3DType == VDEC_3D_REALD_DUAL_STREAM) || (prVdecEsInfo->e3DType == VDEC_3D_BIVL))
    {
        return FALSE;
    }

    if ((prDecPrm->prCurrFBufInfo->u1FBufStatus == TOP_FIELD) ||
            (prDecPrm->prCurrFBufInfo->u1FBufStatus == BOTTOM_FIELD))
    {
        if (fgIsNonRefFBuf(prDecPrm->prCurrFBufInfo->u1FBufRefType,
                           prDecPrm->prCurrFBufInfo->u1TFldRefType,
                           prDecPrm->prCurrFBufInfo->u1BFldRefType) &&
                (!prDecPrm->fgIsMVC) &&
                (prDecPrm->u1CurViewIdx < MAX_VIEW_NUM))
        {
#ifdef CC_SEI_USER_DATA_ENABLE
            prMpvCc = (MPV_CC_T*)_MPV_GetMpvCc(prDecPrm->ucEsId);
            prExtInfo = (MPV_EXT_INFO_T*)_MPV_GetExtInfo();
            if(prMpvCc->fgPlayCc || prExtInfo->fgUserData[prDecPrm->ucEsId])

            {
                return FALSE;
            }
#endif
            LOG(3, "Put Non-Ref Frm to Empty in SkipFrm\n");
            prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
            #if BDP_LOG_STYLE
            VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                             "[H264 VDEC%d]: _VDEC_SkipFrm PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                             prDecPrm->ucEsId,
                             prDecPrm->prCurrFBufInfo->i4PicNum, 
                             prVdecEsInfoKeep->rPesInfo.ucPicType,
                             prVdecEsInfoKeep->rPesInfo.u4PTS);
            #endif        
            prDecPrm->prCurrFBufInfo->u1FBufStatus = FRAME;
            FBM_SetFrameBufferStatus(prDecPrm->aucFbgId[prDecPrm->u1CurViewIdx],
                                     prDecPrm->prCurrFBufInfo->ucFbId, FBM_FB_STATUS_READY);
            LOG(3, "Skip B\n");
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else if (prDecPrm->prSliceHdr->fgFieldPicFlag || prDecPrm->fgIsMVC)  //field encoded, MVC
    {
        return FALSE;
    }
    else
    {
#ifdef CC_SEI_USER_DATA_ENABLE
        prMpvCc = (MPV_CC_T*)_MPV_GetMpvCc(prDecPrm->ucEsId);
        prExtInfo = (MPV_EXT_INFO_T*)_MPV_GetExtInfo();
        if(prMpvCc->fgPlayCc || prExtInfo->fgUserData[prDecPrm->ucEsId])

        {
            return FALSE;
        }
#endif
        prDecPrm->prCurrFBufInfo->u4SkipFrmCnt++;
        LOG(3, "Skip B\n");
        prDecPrm->rSEI.u4UserDataWptr = 0;  //skip cc as well
        prDecPrm->fgSkipCurFrm = TRUE;
        return TRUE;
    }

#endif
}


// **************************************************************************
// Function : void _VDEC_FrmFin(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm);
// Description : error concealment for IDR error
// Parameter : u4VDecID : hw id
// Return      : none
// **************************************************************************
void _VDEC_FrmFin(UINT32 u4VDecID, VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 u4ErrAcc = 0, u4PList0 = 0;
    #if BDP_LOG_STYLE
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL; 
    #endif
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
    UINT32 u4PrevAddr = 0;
    VDEC_TO_TZ_FB_OPERATION_T prVdecFbOp;
#endif
    ASSERT(prDecPrm != NULL);
    
#if BDP_LOG_STYLE
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
#endif

    //save decode time
    prDecPrm->u4CurDecMs += (rTimeHwDt.u4Micros / 1000);

#ifdef VDEC_TIME_PROFILE
    prDecPrm->u4CurDecCycle += u4DecCycle;
    prDecPrm->u4CurDramCycle += u4DramCycle;
#endif

#ifdef CC_STRESS_TEST_CRC_CHK
    prDecPrm->u4DramMC_R_CRC = u4DramCrcMC_R;
    prDecPrm->u4DramPP_W_CRC = u4DramCrcPP_W;
#endif

    if (prDecPrm->prSliceHdr->fgFieldPicFlag)
    {
        return; //need more test on field encoded bitstream
    }
    if (!prDecPrm->prCurrFBufInfo)
    {
        LOG(3, "CurBuf invalid\n");
        return;
    }

    //check error type
    u4ErrAcc = u4VDecReadAVCVLD(u4VDecID, RO_AVLD_ERR_ACC) & ERR_MASK_OBSV;
    if ((!prDecPrm->fgIsMMPlay) &&
            (((u4ErrAcc & ERR_DISP_MASK) > 0) &&
             (prDecPrm->prCurrFBufInfo->u1FBufStatus != NO_PIC)))
    {
        prDecPrm->prCurrFBufInfo->fgIsNotDisp = TRUE;
        #if BDP_LOG_STYLE
        VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_DROPPIC, 
                         "[H264 VDEC%d]: _VDEC_FrmFin PicNum(%d), PicType(0X%X), Pts(0x%x)\n",
                         prDecPrm->ucEsId,
                         prDecPrm->prCurrFBufInfo->i4PicNum, 
                         prVdecEsInfoKeep->rPesInfo.ucPicType,
                         prVdecEsInfoKeep->rPesInfo.u4PTS);
        #endif

        //copy previous frame if IDR is broken, sw workaround
        if (prDecPrm->u1NalUnitType == IDR_SLICE)
        {
            u4PList0 = u4VDecReadMC(u4VDecID, RW_MC_P_LIST0) & 0xFFFFFFFF;
            if (prDecPrm->prCurrFBufInfo->u4PList0Reg != u4PList0)
            {
                LOG(9, "[%s-%d]Plist0[SW(%x) != HW(%x)].\n", __func__, __LINE__, prDecPrm->prCurrFBufInfo->u4PList0Reg, u4PList0);
                u4PList0 = prDecPrm->prCurrFBufInfo->u4PList0Reg;
            }
            //check null pointer
            if (((u4PList0 > 0) && (prDecPrm->prCurrFBufInfo->u4YAddr > 0)) &&
                    (prDecPrm->prCurrFBufInfo->u4CAddr > 0))
            {
            #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
                u4PrevAddr = (u4PList0 ? VIRTUAL(u4PList0) : 0);
                LOG(6, "PL0(P:0x%08x V:0x%08x).\n", u4PList0, u4PrevAddr);
                x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                prVdecFbOp.u4DesAddr = prDecPrm->prCurrFBufInfo->u4YAddr;
                prVdecFbOp.u4SrcAddr = u4PrevAddr;
                prVdecFbOp.u4Size = prDecPrm->prCurrFBufInfo->u4DramPicSize;
                TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_MEMCPY);
                x_memset(&prVdecFbOp,0x0,sizeof(VDEC_TO_TZ_FB_OPERATION_T));
                prVdecFbOp.u4DesAddr = prDecPrm->prCurrFBufInfo->u4CAddr;
                prVdecFbOp.u4SrcAddr = u4PrevAddr + u4VDecReadMC(u4VDecID, RW_MC_CBCR_OFFSET);
                prVdecFbOp.u4Size = (prDecPrm->prCurrFBufInfo->u4DramPicSize) >> 1;
                TZ_VDEC_FB_Operation((VOID *)&prVdecFbOp, TZ_VDEC_FB_MEMCPY);
            #else
                #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
                GDMA_MemCpy((void*)(prDecPrm->prCurrFBufInfo->u4YAddr),
                         (void*)(u4PList0),
                         prDecPrm->prCurrFBufInfo->u4DramPicSize);
                GDMA_MemCpy((void*)(prDecPrm->prCurrFBufInfo->u4CAddr),
                         (void*)(u4PList0 +  u4VDecReadMC(u4VDecID, RW_MC_CBCR_OFFSET)),
                         (prDecPrm->prCurrFBufInfo->u4DramPicSize >> 1));
                #else
                x_memcpy((void*)VIRTUAL(prDecPrm->prCurrFBufInfo->u4YAddr),
                         (void*)VIRTUAL(u4PList0),
                         prDecPrm->prCurrFBufInfo->u4DramPicSize);
                x_memcpy((void*)VIRTUAL(prDecPrm->prCurrFBufInfo->u4CAddr),
                         (void*)VIRTUAL(u4PList0 +  u4VDecReadMC(u4VDecID, RW_MC_CBCR_OFFSET)),
                                     (prDecPrm->prCurrFBufInfo->u4DramPicSize >> 1));
                #endif

            #endif
            }
        }
    }
}


// **************************************************************************
// Function : UINT32 _VDEC_SaveUserData(UCHAR ucEsId, const UINT8* pu1UserData, UINT32 u4Size);
// Description : save cc data info
// Parameter : ucEsId : instance id, pu1UserData: source buffer address, u4Size: cc data size
// Return      : dest buffer address
// **************************************************************************
UINT32 _VDEC_SaveUserData(UCHAR ucEsId, const UINT8* pu1UserData, UINT32 u4Size)
{
    MPV_EXT_INFO_T *prExtInfo;
    MPV_CC_T *prMpvCc;
    CC_BUF_INFO_T *prCcBufInfo;
    UCHAR *pucCcAddr;
    UINT32 u4Sz = 0, u4CpSz = 0, u4Rptr = 0;

    if (pu1UserData == NULL)
    {
        LOG(2, "Null UserDataPtr\n");
        return 0;
    }
    prExtInfo = (MPV_EXT_INFO_T*)_MPV_GetExtInfo();
    prMpvCc = (MPV_CC_T*)_MPV_GetMpvCc(ucEsId);
    prCcBufInfo = &(prMpvCc->rCcBufInfo);
    pucCcAddr = prMpvCc->pucCcBuf;

    if (prExtInfo->pfUserDataCb)
    {
        u4Rptr = prExtInfo->au4UserDataWptr[ucEsId];
        u4Sz = prExtInfo->au4UserDataWptr[ucEsId] + u4Size;

        u4CpSz = (u4Sz < MAX_USERDATA_SIZE) ? u4Size : (MAX_USERDATA_SIZE - prExtInfo->au4UserDataWptr[ucEsId]);
        x_memcpy((void*)&prExtInfo->aucUserDataBuf[ucEsId][prExtInfo->au4UserDataWptr[ucEsId]],
                 (void*)pu1UserData,
                 u4CpSz);
        prExtInfo->au4UserDataWptr[ucEsId] += u4CpSz;
        if (prExtInfo->au4UserDataWptr[ucEsId] == MAX_USERDATA_SIZE)
        {
            prExtInfo->au4UserDataWptr[ucEsId] = 0;
        }

        if (u4CpSz < u4Size) // ring data
        {
            u4Sz = u4Size - u4CpSz;
            x_memcpy((void*)prExtInfo->aucUserDataBuf[ucEsId],
                     (void*)(pu1UserData + u4CpSz),
                     u4Sz);
            prExtInfo->au4UserDataWptr[ucEsId] = u4Sz;
        }
    }
    if (prMpvCc->fgPlayCc && pucCcAddr)
    {
        u4Rptr = prCcBufInfo->u4Wptr;
        u4Sz = prCcBufInfo->u4Wptr + u4Size;

        u4CpSz = (u4Sz < MAX_USERDATA_SIZE) ? u4Size : (MAX_USERDATA_SIZE - prCcBufInfo->u4Wptr);
        x_memcpy((void*)VIRTUAL((UINT32)&pucCcAddr[prCcBufInfo->u4Wptr]),
                 (void*)VIRTUAL((UINT32)pu1UserData),
                 u4CpSz);
        prCcBufInfo->u4Wptr += u4CpSz;
        if (prCcBufInfo->u4Wptr == MAX_USERDATA_SIZE)
        {
            prCcBufInfo->u4Wptr = 0;
        }

        if (u4CpSz < u4Size) // ring data
        {
            u4Sz = u4Size - u4CpSz;
            x_memcpy((void*)VIRTUAL((UINT32)pucCcAddr),
                     (void*)VIRTUAL((UINT32)(pu1UserData + u4CpSz)),
                     u4Sz);
            prCcBufInfo->u4Wptr = u4Sz;
        }
    }

    return u4Rptr;
}


// **************************************************************************
// Function : void _VDEC_ResetHW(BOOL fgIsUFOMode, BOOL fgIsDualCoreMode, UINT32 u4VDecID);
// Description : do hw reset
// Parameter : fgIsUFOMode : ufo or non-ufo, fgIsDualCoreMode: dual core or single core, u4VDecID: hw id
// Return      : none
// **************************************************************************
void _VDEC_ResetHW(BOOL fgIsUFOMode, BOOL fgIsDualCoreMode, UINT32 u4VDecID)
{  
    if (fgIsDualCoreMode)
    {
        vVDecResetMultiCore(0);
    }
    _VDEC_VLDHWReset(fgIsUFOMode, fgIsDualCoreMode, u4VDecID);
}


#ifdef FBM_DPB_CTRL
// **************************************************************************
// Function : static UCHAR  _VDEC_GetFRFromSPS(VDEC_AVC_DecPrm* prDecPrm);
// Description : get framerate from sps info
// Parameter : 
// Return      : framerate value
// **************************************************************************
static UCHAR  _VDEC_GetFRFromSPS(VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 u4FrameRateFrmSPS = 0xff;
    UCHAR ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    
    if (prDecPrm->prSPS->fgVuiParametersPresentFlag)
    {
        if (prDecPrm->prSPS->rVUI.u4NumUnitsInTick > 0)
        {
            u4FrameRateFrmSPS = (prDecPrm->prSPS->rVUI.u4TimeScale / prDecPrm->prSPS->rVUI.u4NumUnitsInTick) >> 1;
        }

        switch (u4FrameRateFrmSPS)
        {
            case 12:
                ucFrmRatCod = MPEG_FRAME_RATE_12;
                break;
            case 15:
                ucFrmRatCod = MPEG_FRAME_RATE_15;
                break;
            case 20:
                ucFrmRatCod = MPEG_FRAME_RATE_20;
                break;
            case 24: //distinguish 24 and 25, add (prDecPrm->prSPS->rVUI.u4NumUnitsInTick >> 1) for precision issue
                if ((((prDecPrm->prSPS->rVUI.u4TimeScale + (prDecPrm->prSPS->rVUI.u4NumUnitsInTick >> 1)) /
                                prDecPrm->prSPS->rVUI.u4NumUnitsInTick) >> 1) == 25)
                {
                    ucFrmRatCod = MPEG_FRAME_RATE_25;
                }
                else
                {
                    ucFrmRatCod = MPEG_FRAME_RATE_24;
                }
                break;
            case 23:
                ucFrmRatCod = MPEG_FRAME_RATE_24_;
                break;
            case 25:
                ucFrmRatCod = MPEG_FRAME_RATE_25;
                break;
            case 30:
                ucFrmRatCod = MPEG_FRAME_RATE_30;
                break;
            case 29:
                ucFrmRatCod = MPEG_FRAME_RATE_30_;
                break;
            case 47:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    ucFrmRatCod = MPEG_FRAME_RATE_24_;
                }
                else
                {
                    LOG(1,"Framerate warning .....\n");
                }
                break;
            case 50:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {   
                    ucFrmRatCod = MPEG_FRAME_RATE_25;
                }
                else
                {
                    ucFrmRatCod = MPEG_FRAME_RATE_50;
                }
                break;
            case 60:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                {    
                    ucFrmRatCod = MPEG_FRAME_RATE_30;
                }
                else
                {
                    ucFrmRatCod = MPEG_FRAME_RATE_60;
                }
                break;
            case 59:
                if(prVdecEsInfo->e3DType == VDEC_3D_BIVL)
                { 
                    ucFrmRatCod = MPEG_FRAME_RATE_30_;
                }
                else
                {
                    ucFrmRatCod = MPEG_FRAME_RATE_60_;
                }
                break;
            case 120:
                ucFrmRatCod = MPEG_FRAME_RATE_120;
                break;
            case 240:
                ucFrmRatCod = MPEG_FRAME_RATE_240;
                break;
            default:
                ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                break;
        }
    }
    else
    {
        ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
    }

    return ucFrmRatCod;
}


// **************************************************************************
// Function : static UCHAR  _VDEC_GetFRFromContainer(VDEC_AVC_DecPrm* prDecPrm);
// Description : get framerate value from container info
// Parameter : 
// Return      : framerate value
// **************************************************************************
static UCHAR  _VDEC_GetFRFromContainer(VDEC_AVC_DecPrm* prDecPrm)
{
    UCHAR ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
    VDEC_ES_INFO_T* prVdecEsInfo;

    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    //if ES header info is invalid, use container's info
    if(prVdecEsInfo->fgMMPlayback &&
         (prVdecEsInfo->rMMSeqInfo.e_frame_rate != VDEC_SRC_FRAME_RATE_UNKNOWN))
    {
        switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
        {
            case VDEC_SRC_FRAME_RATE_12:
               ucFrmRatCod = MPEG_FRAME_RATE_12;
               break;
            case VDEC_SRC_FRAME_RATE_15:
               ucFrmRatCod = MPEG_FRAME_RATE_15;
               break;
            case VDEC_SRC_FRAME_RATE_20:
                ucFrmRatCod = MPEG_FRAME_RATE_20;
                break;
            case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
                ucFrmRatCod = MPEG_FRAME_RATE_24_;
                break;
            case VDEC_SRC_FRAME_RATE_24:
                ucFrmRatCod = MPEG_FRAME_RATE_24;
                break;
            case VDEC_SRC_FRAME_RATE_25:
                ucFrmRatCod = MPEG_FRAME_RATE_25;
                break;
            case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
                ucFrmRatCod = MPEG_FRAME_RATE_30_;
                break;
            case VDEC_SRC_FRAME_RATE_30:
                ucFrmRatCod = MPEG_FRAME_RATE_30;
                break;
            case VDEC_SRC_FRAME_RATE_50:
                ucFrmRatCod =
                    (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_50 : MPEG_FRAME_RATE_25;
                break;
            case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
                ucFrmRatCod =
                    (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_60_ : MPEG_FRAME_RATE_30_;
                break;
            case VDEC_SRC_FRAME_RATE_60:
                ucFrmRatCod =
                    (!prDecPrm->prSliceHdr->fgFieldPicFlag) ? MPEG_FRAME_RATE_60 : MPEG_FRAME_RATE_30;
                break;
            case VDEC_SRC_FRAME_RATE_120:
                ucFrmRatCod = MPEG_FRAME_RATE_120;
                break;
            case VDEC_SRC_FRAME_RATE_240:
                ucFrmRatCod = MPEG_FRAME_RATE_240;
                break;
            default:
                ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
                break;
        }
    }    
    return ucFrmRatCod;
}


// **************************************************************************
// Function : UINT32 u4VDecFrmMapFrameRate(UINT32 u4InstID,UINT32 u4MPEGFrameRate);
// Description : mapping framerate value for pre-parse 
// Parameter : u4InstID : instance id, u4MPEGFrameRate : framerate parse from vdec
// Return      : framerate value
// **************************************************************************
/*UCHAR u4VDecFrmMapFrameRate(UINT32 u4InstID,UINT32 u4MPEGFrameRate)
{
    UINT32 u4RetVal = 0;
    
    switch(u4MPEGFrameRate)
    {
        case 1200:
           u4RetVal = MPEG_FRAME_RATE_12;
           break;
        case 1500:
           u4RetVal = MPEG_FRAME_RATE_15;
           break;
        case 2000:
            u4RetVal = MPEG_FRAME_RATE_20;
            break;
        case 2397: // 24000/1001 (23.976...) 
            u4RetVal = MPEG_FRAME_RATE_24_;
            break;
        case 2400:
            u4RetVal = MPEG_FRAME_RATE_24;
            break;
        case 2500:
            u4RetVal = MPEG_FRAME_RATE_25;
            break;
        case 2997: // 30000/1001 (29.97...) 
            u4RetVal = MPEG_FRAME_RATE_30_;
            break;
        case 3000:
            u4RetVal = MPEG_FRAME_RATE_30;
            break;
        case 5000:
            u4RetVal = MPEG_FRAME_RATE_50;
            break;
        case 5994: // 60000/1001 (59.94...) 
            u4RetVal = MPEG_FRAME_RATE_60_;
            break;
        case 6000:
            u4RetVal = MPEG_FRAME_RATE_60;
            break;
        default:
            u4RetVal = 3000;
            break;
    }   

    return u4RetVal;
}*/


// **************************************************************************
// Function : static void _VDEC_ResizeSmallImage(UCHAR ucEsId, FBM_SEQ_HDR_T *prFbmSeqHdr);
// Description : enlarge picture for small picture
// Parameter : ucEsId : instance id, prFbmSeqHdr: sequnce header
// Return      : none
// **************************************************************************
static void _VDEC_ResizeSmallImage(UCHAR ucEsId, FBM_SEQ_HDR_T *prFbmSeqHdr)
{
    VDEC_ES_INFO_T* prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

#ifdef CC_RESIZE_SMALL_IMAGE
    prVdecEsInfo->u2OrgHSize = prFbmSeqHdr->u2OrgHSize;
    prVdecEsInfo->u2OrgVSize = prFbmSeqHdr->u2OrgVSize;

    if (((prFbmSeqHdr->u2HSize < FBM_IMG_RESIZE_LIMITATION)
#ifndef IS_POST_MT5395
            || (prFbmSeqHdr->u2VSize < FBM_IMG_RESIZE_LIMITATION)
#endif
        ) &&
            (!((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))))
    {
        prFbmSeqHdr->fgResizeSmallPic = TRUE;
        if (prFbmSeqHdr->u2HSize < FBM_IMG_RESIZE_LIMITATION)
        {
            prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
            prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
            prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
        }
        else if (prFbmSeqHdr->u2HSize > FBM_IMG_RESIZE_LIMITATION_MAX)
        {
            prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
            prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
            prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
        }
        else
        {
            prFbmSeqHdr->u2LineSize = AVC_ALIGN_MASK(prFbmSeqHdr->u2HSize,
                                      AVC_BLK_ALIGMENT);
            prFbmSeqHdr->u2HSize = prFbmSeqHdr->u2LineSize;
            prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2LineSize;
        }
        if (prFbmSeqHdr->u2VSize < FBM_IMG_RESIZE_LIMITATION)
        {
            prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
            prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
        }
        else if (prFbmSeqHdr->u2VSize > FBM_IMG_RESIZE_LIMITATION_MAX)
        {
            prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
            prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
        }
        else
        {
            prFbmSeqHdr->u2VSize = AVC_ALIGN_MASK(prFbmSeqHdr->u2VSize,
                                                  AVC_BLK_ALIGMENT);
            prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
        }

        LOG(6, "-> 0 OrgHSize %d, OrgVSize %d, HSize %d, VSize %d, LineSize %d\n",
            prFbmSeqHdr->u2OrgHSize, prFbmSeqHdr->u2OrgVSize,
            prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize, prFbmSeqHdr->u2LineSize);
    }
#endif
}


// **************************************************************************
// Function : static BOOL  _VDEC_CheckAspInfo(VDEC_AVC_DecPrm* prDecPrm);
// Description : get aspect ratio info
// Parameter : 
// Return      : none
// **************************************************************************
/*static VOID  _VDEC_GetAspInfo(VDEC_AVC_DecPrm* prDecPrm)
{
    UINT8 u1ViewIdx;
    UINT32 u4DAR = 0, u4SAR = 0, u4EstimateWidth = 0, u4EstimateHeigh = 0;
    UINT32 u4CurDAR, u4KeptDAR, u4Tolerance;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;

    if (prDecPrm == NULL)
    {
        LOG(2, "prDecPrm Null!!\n");
        return;
    }
    
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
    
    if(prFbmSeqHdr == NULL)
    {
        LOG(2, "GetFbgSeqHdr Fail 2!!\n");
        return;
    }
    
    if (prDecPrm->prSPS->rVUI.u4AspectRatioIdc == 0xFF)  //extended_SAR
    {
        if ((prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth != prDecPrm->prSPS->rVUI.u4SarWidth) ||
                (prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight != prDecPrm->prSPS->rVUI.u4SarHeight) ||
                prDecPrm->fgAfdChg)   //resolution change
        {
            u4DAR = ((prDecPrm->u4PicW * prDecPrm->prSPS->rVUI.u4SarWidth) * 100) /
                    (prDecPrm->u4PicH * prDecPrm->prSPS->rVUI.u4SarHeight);
            prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth = prDecPrm->prSPS->rVUI.u4SarWidth;
            prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight = prDecPrm->prSPS->rVUI.u4SarHeight;
            prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf = 0xFF;
            prDecPrm->rLastInfo[u1ViewIdx].u4DAR = u4DAR;
            prDecPrm->fgAfdChg = TRUE;
            LOG(4,"Aspectratio change %d @ Line %d\n",u4DAR,__LINE__);
            prFbmSeqHdr->u4AspectRatioWidth = prDecPrm->prSPS->rVUI.u4SarWidth;
            prFbmSeqHdr->u4AspectRatioHeight = prDecPrm->prSPS->rVUI.u4SarHeight;
        }
    }
    else if ((prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf !=
                  prDecPrm->prSPS->rVUI.u4AspectRatioIdc) || prDecPrm->fgAfdChg)
            //aspRatio change or resolution chage
    {
        switch (prDecPrm->prSPS->rVUI.u4AspectRatioIdc)
        {
            case 1:
                u4SAR = 100;
                prFbmSeqHdr->u4AspectRatioWidth = 1;
                prFbmSeqHdr->u4AspectRatioHeight = 1;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
                break;
            case 2:
                u4SAR = 1200 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 12;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_12_11;
                break;
            case 3:
                u4SAR = 1000 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 10;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_10_11;
                break;
            case 4:
                u4SAR = 1600 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 16;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_16_11;
                break;
            case 5:
                u4SAR = 4000 / 33;
                prFbmSeqHdr->u4AspectRatioWidth = 40;
                prFbmSeqHdr->u4AspectRatioHeight = 33;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_40_33;
                break;
            case 6:
                u4SAR = 2400 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 24;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_24_11;
                break;
            case 7:
                u4SAR = 2000 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 20;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_20_11;
                break;
            case 8:
                u4SAR = 3200 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 32;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_32_11;
                break;
            case 9:
                u4SAR = 8000 / 33;
                prFbmSeqHdr->u4AspectRatioWidth = 80;
                prFbmSeqHdr->u4AspectRatioHeight = 33;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_80_33;
                break;
            case 10:
                u4SAR = 1800 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 18;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_18_11;
                break;
            case 11:
                u4SAR = 1500 / 11;
                prFbmSeqHdr->u4AspectRatioWidth = 15;
                prFbmSeqHdr->u4AspectRatioHeight = 11;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_15_11;
                break;
            case 12:
                u4SAR = 6400 / 33;
                prFbmSeqHdr->u4AspectRatioWidth = 64;
                prFbmSeqHdr->u4AspectRatioHeight = 33;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_64_33;
                break;
            case 13:
                u4SAR = 16000 / 99;
                prFbmSeqHdr->u4AspectRatioWidth = 160;
                prFbmSeqHdr->u4AspectRatioHeight = 99;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_160_99;
                break;
            case 14:
                u4SAR = 400 / 3;
                prFbmSeqHdr->u4AspectRatioWidth = 4;
                prFbmSeqHdr->u4AspectRatioHeight = 3;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_4_3;
                break;
            case 15:
                u4SAR = 300 / 2;
                prFbmSeqHdr->u4AspectRatioWidth = 3;
                prFbmSeqHdr->u4AspectRatioHeight = 2;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_3_2;
                break;
            case 16:
                u4SAR = 200;
                prFbmSeqHdr->u4AspectRatioWidth = 2;
                prFbmSeqHdr->u4AspectRatioHeight = 1;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_2_1;
                break;
            default:
                u4SAR = 100;
                prFbmSeqHdr->u4AspectRatioWidth = 1;
                prFbmSeqHdr->u4AspectRatioHeight = 1;
                prFbmSeqHdr->ucOrgAsp = VDEC_ASPECT_RATIO_1_1;
        }
        u4DAR = (prDecPrm->u4PicW * u4SAR) / prDecPrm->u4PicH;
        if (prVdecEsInfo 
#ifdef ENABLE_MULTIMEDIA
            && ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) || (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS))
            && (!prDecPrm->fgReCreateFBG)
#endif
            && ((prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ? 
                                        prDecPrm->prSPS->u4FrameCropBottomOffset : 
                                        (prDecPrm->prSPS->u4FrameCropBottomOffset << 1))) != 0)
            )
        {
            u4DAR = (prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset) * u4SAR /
                    (prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ? 
                                        prDecPrm->prSPS->u4FrameCropBottomOffset : 
                                        (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
        }
        prDecPrm->fgAfdChg = TRUE;            
        LOG(2, "Afd change Last(%d) Cur(%d) SAR(%d) DAR(%d).\n", 
            prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf,
            prDecPrm->prSPS->rVUI.u4AspectRatioIdc,
            u4SAR, u4DAR);
        prDecPrm->rLastInfo[u1ViewIdx].u4SARWidth = 0;
        prDecPrm->rLastInfo[u1ViewIdx].u4SARHeight = 0;
        prDecPrm->rLastInfo[u1ViewIdx].u4AspRatInf = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
        prDecPrm->rLastInfo[u1ViewIdx].u4DAR = u4DAR;
    }
    else if(IS_AVC_RAP(prDecPrm->u4PicNalType) && (prDecPrm->rSEI.u1AFD == SEI_AFD_UNKONWN) && (prFbmSeqHdr->ucActFmt != ACT_FMT_DEFAULT))
    {
        prFbmSeqHdr->ucActFmt = ACT_FMT_DEFAULT;
        prDecPrm->fgAfdChg = TRUE;
        u4DAR = prDecPrm->rLastInfo[u1ViewIdx].u4DAR;
        LOG(4,"u4AFD change %d u4DAR %d @ Line %d\n",prFbmSeqHdr->ucActFmt,u4DAR,__LINE__);
    }
    
    prFbmSeqHdr->u4OrgAspWidth = prFbmSeqHdr->u4AspectRatioWidth;
    prFbmSeqHdr->u4OrgAspHeight = prFbmSeqHdr->u4AspectRatioHeight;
    
    if (prDecPrm->fgAfdChg)
    {
        prFbmSeqHdr->u4AspectRatioIdc = prDecPrm->prSPS->rVUI.u4AspectRatioIdc;
    
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
        if (prDecPrm->fgIsMMPlay)
        {
            prFbmSeqHdr->fgPixelAspectRatio = TRUE;
            #ifdef DIVX_PLUS_CER  // for divx certification. as the certification requirements says, the container's aspect ratio info has a higer priority than vdec.  xiaolei.li
            if(prVdecEsInfo && (prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w > 0) && (prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h > 0))
            {
                if(prVdecEsInfo->rMMSeqInfo.e_asp_usr_type == VDEC_ASP_USR_TYPE_DISP)
                {
                    prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                }
                prFbmSeqHdr->u4AspectRatioWidth = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w;
                prFbmSeqHdr->u4AspectRatioHeight = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h;
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
                LOG(6,"[H264]: USE MW Aspect Ratio Info \n");
            }
            else
            {
                if((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                    ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
                {
                    prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                    prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
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
            #else
            if ((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                    ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
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
                    prFbmSeqHdr->fgPixelAspectRatio = TRUE;
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
            #endif
        }
        else
        {
            #if defined(CC_USE_DDI)
            if((!prDecPrm->prSPS->fgVuiParametersPresentFlag) ||
                ((prDecPrm->prSPS->fgVuiParametersPresentFlag) && (!prDecPrm->prSPS->rVUI.fgAspectRatioInfoPresentFlag)))
            {
                prFbmSeqHdr->fgPixelAspectRatio = FALSE;
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_16_9;
            }
            else
            {
                prFbmSeqHdr->fgPixelAspectRatio = TRUE;
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_TRANSMIT;
            }
            #else
            prFbmSeqHdr->fgPixelAspectRatio = FALSE;
            #endif
        }
        if (prVdecEsInfo 
            #ifdef ENABLE_MULTIMEDIA
            && ((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) || (prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS))
            && (!prDecPrm->fgReCreateFBG)
            #endif
            )
        {
            u4EstimateWidth = (u4DAR * prVdecEsInfo->u4SeamlessHeight) / 100;
            u4EstimateHeigh = (prVdecEsInfo->u4SeamlessWidth * 100) / u4DAR;

            u4EstimateWidth = VDEC_ALIGN_MASK(u4EstimateWidth, 15);
            u4EstimateHeigh = VDEC_ALIGN_MASK(u4EstimateHeigh, 15);
                            
            if (u4EstimateWidth <= prVdecEsInfo->u4SeamlessWidth)
            {
                u4EstimateHeigh = prVdecEsInfo->u4SeamlessHeight;
            }
            else
            {
                u4EstimateWidth = prVdecEsInfo->u4SeamlessWidth;
            }
     
            //will not change resize resolution within tolerance range
            u4CurDAR = 0;
            u4KeptDAR = 0;
            u4Tolerance = 0;
            if(u4EstimateHeigh != 0)
            {
                u4CurDAR = (u4EstimateWidth * 1000) / u4EstimateHeigh;
            }
            if (prFbmSeqHdr->u4ResizeHeight != 0)   // prevent div by 0
            {
                u4KeptDAR = (prFbmSeqHdr->u4ResizeWidth * 1000) / prFbmSeqHdr->u4ResizeHeight;
                u4Tolerance = u4KeptDAR * VDP_SEAMLESS_RESIZE_TOLERANCE / 100;
            }
            
            if((u4KeptDAR == 0) ||  // first assignment
                ((u4CurDAR < (u4KeptDAR - u4Tolerance)) || (u4CurDAR > (u4KeptDAR + u4Tolerance))))
            {
                prFbmSeqHdr->u4ResizeWidth = u4EstimateWidth;
                prFbmSeqHdr->u4ResizeHeight = (u4EstimateHeigh > 1080) ? 1080 : u4EstimateHeigh;
            }
            if(prVdecEsInfoKeep->fgVPush)
            {
                prFbmSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
            }
        }
        LOG(2, "AFD %d, AspRatio Chg %d, [%d:%d], IsSAR %d\n", 
            prFbmSeqHdr->ucActFmt, prFbmSeqHdr->ucAspRatInf, 
            prFbmSeqHdr->u4AspectRatioWidth, prFbmSeqHdr->u4AspectRatioHeight, prFbmSeqHdr->fgPixelAspectRatio);
        #if BDP_LOG_STYLE
        VDecPrintf(prDecPrm->ucEsId, VDEC_LOG_SEQINFO,
                   "[VDEC-H264(%d)] : AspectRatioIdc[%d], MW width:height[%d:%d], SarWidth:SarHeight[%d:%d], AspRatInf[%d], AspRatWidth:AspRatHeight[%d:%d]\n",
                   prDecPrm->ucEsId,
                   prDecPrm->prSPS->rVUI.u4AspectRatioIdc,
                   (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_w,
                   (UINT32)prVdecEsInfo->rMMSeqInfo.u2_asp_usr_h,
                   prDecPrm->prSPS->rVUI.u4SarWidth,
                   prDecPrm->prSPS->rVUI.u4SarHeight,
                   prFbmSeqHdr->ucAspRatInf,
                   prFbmSeqHdr->u4AspectRatioWidth,
                   prFbmSeqHdr->u4AspectRatioHeight);
        #endif
        //FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
    }
}


// **************************************************************************
// Function : static VOID  _VDEC_SetSeamlessinfo(VDEC_AVC_DecPrm* prDecPrm);
// Description : set seamless info to sequence header
// Parameter : 
// Return      : none
// **************************************************************************
static VOID  _VDEC_SetSeamlessInfo(VDEC_AVC_DecPrm* prDecPrm)
{
    UINT8 u1ViewIdx;
    VDEC_ES_INFO_T* prVdecEsInfo;
    FBM_SEQ_HDR_T *prFbmSeqHdr = NULL;

    if (prDecPrm == NULL)
    {
        LOG(2, "prDecPrm Null!!\n");
        return;
    }
    
    u1ViewIdx = prDecPrm->u1CurViewIdx;
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prDecPrm->aucFbgId[u1ViewIdx]);
    if(prFbmSeqHdr == NULL)
    {
        LOG(2, "GetFbgSeqHdr Fail 2!!\n");
        return;
    }
    
    if ((prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV) && (!prDecPrm->bPreAllocFBG))
    {
        if (prDecPrm->fgIsNewMode)
        {
            prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
        }
        else
        {
            prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
        }

        prFbmSeqHdr->u2OrgWidth = (UINT16)(prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset);
        prFbmSeqHdr->u2OrgHeight = (UINT16)(prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
                                    prDecPrm->prSPS->u4FrameCropBottomOffset : (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
        if((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) ||
        (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH))
        {
            prDecPrm->fgIsResChg = TRUE;
            LOG(2,"Line(%d) Send FBM_FLAG_SEQ_CHG org[%d,%d] ==> cur[%d,%d]\n", __LINE__,
                prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW,
                prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH,
                prDecPrm->u4PicW,prDecPrm->u4PicH);
    #ifdef CC_MAPLE_CUST_DRV
            prFbmSeqHdr->u4resolutionChg++;
    #endif
            //FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
        }
        if (prVdecEsInfo->fgIsTPT && (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV))
        {
            if (prDecPrm->fgIsNewMode)
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
            }
            else
            {
                prFbmSeqHdr->u2LineSize = prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
            }
            prFbmSeqHdr->u2OrgHSize = prDecPrm->u4PicW;
            prFbmSeqHdr->u2OrgVSize = prDecPrm->u4PicH;
            prFbmSeqHdr->u2HSize = (UINT16)prDecPrm->u4PicW;
            prFbmSeqHdr->u2VSize = (UINT16)prDecPrm->u4PicH;
            LOG(2,"OrgWidth/OrgHeight1 [%d,%d] Picth(%d)\n",prDecPrm->u4PicW, prDecPrm->u4PicH, prVdecEsInfo->u2LineSize);
        }
    }
    else if (((prVdecEsInfo->eSeamlessMode & SEAMLESS_ONE_FBS) && (!prDecPrm->bPreAllocFBG) && (!prDecPrm->fgReCreateFBG)) ||
        ((prVdecEsInfo->eSeamlessMode & SEAMLESS_TWO_FBS) && (!prDecPrm->bPreAllocFBG)))
    {
        prDecPrm->fgSeamlessDisp = TRUE;        
        if (prDecPrm->fgIsNewMode)
        {
            prVdecEsInfo->u2LineSize = _MAKE_16X(prDecPrm->u4PicW);
        }
        else
        {
            prVdecEsInfo->u2LineSize = _MAKE_64X(prDecPrm->u4PicW);
        }

        prFbmSeqHdr->u2OrgWidth = (UINT16)(prDecPrm->u4PicW - prDecPrm->prSPS->u4FrameCropRightOffset);
        prFbmSeqHdr->u2OrgHeight = (UINT16)(prDecPrm->u4PicH -((prDecPrm->prSPS->fgFrameMbsOnlyFlag) ?
                                    prDecPrm->prSPS->u4FrameCropBottomOffset : (prDecPrm->prSPS->u4FrameCropBottomOffset << 1)));
        if((prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW != prDecPrm->u4PicW) ||
        (prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH != prDecPrm->u4PicH))
        {
            prDecPrm->fgIsResChg = TRUE;
            LOG(2,"Send FBM_FLAG_SEQ_CHG org[%d,%d] ==> cur[%d,%d]\n",prDecPrm->rLastInfo[u1ViewIdx].u4LastPicW,prDecPrm->rLastInfo[u1ViewIdx].u4LastPicH,
                                                                    prDecPrm->u4PicW,prDecPrm->u4PicH);
            #ifdef CC_MAPLE_CUST_DRV
            prFbmSeqHdr->u4resolutionChg++;
            #endif
            FBM_SetFrameBufferFlag(prDecPrm->aucFbgId[u1ViewIdx], FBM_FLAG_SEQ_CHG);
        }
    }
}*/
#endif


#ifdef ENABLE_MULTIMEDIA
// **************************************************************************
// Function : static BOOL _VDEC_IsxvYCC(VDEC_AVC_DecPrm* prDecPrm);
// Description : judge if xvYcc color space or not
// Parameter : 
// Return      :  if xvYCC return true
// **************************************************************************
static BOOL _VDEC_IsxvYCC(VDEC_AVC_DecPrm* prDecPrm)
{
    VDEC_ES_INFO_T* prVdecEsInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    if ((prVdecEsInfo->eContainerType == SWDMX_FMT_MP4) &&
        prDecPrm->prSPS->rVUI.fgVideoSignalTypePresentFlag && 
        prDecPrm->prSPS->rVUI.fgColourDescriptionPresentFlag && 
        (11 == prDecPrm->prSPS->rVUI.u4TransferCharacteristics))
    {
        LOG(1, "SPS Parse xvYCC info\n");
        return TRUE;
    }
    else if ((prDecPrm->rSEI.rxvYCCInfo.ucFormatFlag) &&
                (2 == prDecPrm->rSEI.rxvYCCInfo.ucGEDColorPrecision) &&
                (2 == prDecPrm->rSEI.rxvYCCInfo.ucGEDColorSpace) &&
                (0x9B5 == ((prDecPrm->rSEI.rxvYCCInfo.u4RedData & 0x00fff000)>>12)) &&
                (0x2F4 == (prDecPrm->rSEI.rxvYCCInfo.u4RedData & 0x00000fff)) &&
                (0x8D7 == ((prDecPrm->rSEI.rxvYCCInfo.u4GreenData & 0x00fff000)>>12)) &&
                (0x296 == (prDecPrm->rSEI.rxvYCCInfo.u4GreenData & 0x00000fff)) &&
                (0x8CC == ((prDecPrm->rSEI.rxvYCCInfo.u4BlueData & 0x00fff000)>>12)) &&
                (0x292 == (prDecPrm->rSEI.rxvYCCInfo.u4BlueData & 0x00000fff)))
    {
        LOG(1, "SEI Parse xvYCC info\n");
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif


#ifdef CC_DUAL_CORE_DEC
// **************************************************************************
// Function : void vLAEPartitionDPB(VDEC_AVC_DecPrm* prDecPrm);
// Description : patition lae dpb buffer, but the buffer not used
// Parameter : 
// Return      : none
// **************************************************************************
void vLAEPartitionDPB(VDEC_AVC_DecPrm* prDecPrm)
{
    INT32 i, k;
    UINT32 u4DramPicSize; //u4YLen, u4CLen;
    static UINT32 u4DPBSA = 0;//, u4MvSA[2] = {0};

    ASSERT(prDecPrm != NULL);
    if (prDecPrm->u1CurViewIdx >= MAX_VIEW_NUM)
    {
        LOG(1, "PartitionDPB ViewIdx Err!\n");
        return;
    }
        
    if (u4DPBSA == 0)
    {
        u4DPBSA = (UINT32)BSP_AllocAlignedDmaMemory(34*1024,1024);
    }
    prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx] = prDecPrm->prSPS->u4NumRefFrames + 1;
    
    //For DDR3
    // Real pic size w=64x, h=32x  
    u4DramPicSize = 1024;//((((prDecPrm->u4PicW + 15) >> 4) * ((prDecPrm->u4PicH + 31) >> 5)) << 9);

    if (prDecPrm->fgIs10BitMode)
    {
        u4DramPicSize = u4DramPicSize*5/4;
    }
  
    for (k = prDecPrm->u1CurViewIdx; k < MAX_VIEW_NUM; k++)
    {
        for(i = 0; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
        {
            prDecPrm->arFBufInfo[k][i].u4W = prDecPrm->u4PicW;
            prDecPrm->arFBufInfo[k][i].u4H = prDecPrm->u4PicH;
            prDecPrm->arFBufInfo[k][i].u4DramPicSize = u4DramPicSize;            
            prDecPrm->arFBufInfo[k][i].u4YAddr = prDecPrm->u4DPBAddr + (i * u4DramPicSize);
            prDecPrm->arFBufInfo[k][i].u4CAddr = prDecPrm->arFBufInfo[k][i].u4YAddr + u4DramPicSize;
            prDecPrm->arFBufInfo[k][i].u4MVBufAddr = prDecPrm->arFBufInfo[k][i].u4CAddr + (u4DramPicSize >> 1);
        }
    }
    // current reset to 0 when DPB partition.
    prDecPrm->prCurrFBufInfo = &prDecPrm->arFBufInfo[0][0];
}


// **************************************************************************
// Function : void vLAEAllocateFBuf(VDEC_AVC_DecPrm* prDecPrm);
// Description : allocate lae buffer
// Parameter : 
// Return      : none
// **************************************************************************
void vLAEAllocateFBuf(VDEC_AVC_DecPrm* prDecPrm)
{
    INT32 i;
    INT32 iMinPOC;
    UINT32 u4MinPOCFBufIdx = 0;

    // Check if DPB full
    iMinPOC = 0x7fffffff;
    for(i=0; i < prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx]; i++)
    {
        if(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1FBufStatus == NO_PIC)
        {
            iMinPOC = 0x7fffffff;
            u4MinPOCFBufIdx = i;        
            break;
        }
        // miew: need to take care of field empty
        else if((iMinPOC > prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC) && 
                    fgIsNonRefFBuf(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1FBufRefType,
                            prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1TFldRefType,
                            prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1BFldRefType))
        {
            iMinPOC = prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC;
            u4MinPOCFBufIdx = i;
        }
    }
    if(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][u4MinPOCFBufIdx].u1FBufStatus != NO_PIC)
    {           
        _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + (((UINT32)prDecPrm->u1CurViewIdx)<<8));
    }
    prDecPrm->u1DecFBufIdx = u4MinPOCFBufIdx;
    // Only new alloc needs to update current fbuf idx
    prDecPrm->prCurrFBufInfo = &prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][prDecPrm->u1DecFBufIdx];
    if(!prDecPrm->prCurrFBufInfo)
    {
        LOG(0,"prDecPrm->prCurrFBufInfo == NULL u1ViewIdx %d prDecPrm->u1DecFBufIdx %d\n",prDecPrm->u1CurViewIdx,prDecPrm->u1DecFBufIdx);
        return;
    }
#if defined(CC_MVC_SUPPORT) && !defined(CC_VDEC_MVC_DISABLE)
#ifdef IS_POST_MT5368
    prDecPrm->prCurrFBufInfo->u4ViewIDFb = prDecPrm->u4CurViewID;
#else
    prDecPrm->prCurrFBufInfo->u4ViewIDFb = (UINT32)prDecPrm->u1CurViewIdx;
#endif

    prDecPrm->aprCurrFBufInfo[prDecPrm->u1CurViewIdx] = prDecPrm->prCurrFBufInfo;
    prDecPrm->au1DecFBufIdx[prDecPrm->u1CurViewIdx] = prDecPrm->u1DecFBufIdx;
    //default value, will refine in SetCurrPOC function, find the frame with same POC in base view
    if (prDecPrm->u1CurViewIdx == 0)
    {
        prDecPrm->u4POCFbIdx = prDecPrm->u1DecFBufIdx;
    }
    if (prDecPrm->fgInterView)
    {
        prDecPrm->prCurrFBufInfo->fgIsInterview = TRUE;
    }
#endif
    prDecPrm->pu1DecWorkBufY = (UINT8*)(prDecPrm->prCurrFBufInfo->u4YAddr);
    prDecPrm->pu1DecWorkBufC = (UINT8*)(prDecPrm->prCurrFBufInfo->u4CAddr);
    prDecPrm->pu1DecWorkBufMV = (UINT8*)(prDecPrm->prCurrFBufInfo->u4MVBufAddr);
}


// **************************************************************************
// Function : void vLAEPrepareFBufInfo(VDEC_AVC_DecPrm* prDecPrm);
// Description : prepare LAE decode info
// Parameter : 
// Return      : none
// **************************************************************************
void vLAEPrepareFBufInfo(VDEC_AVC_DecPrm* prDecPrm)
{
    BOOL fgRefChg = FALSE;
    UCHAR ucMainEsId = 0;
    UINT32 u4CurrFrameNum, u4UnusedShortTermFrameNum; 
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
	VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeepMain;
	
	ucMainEsId = ucVDecGetMcoreDecInst(prDecPrm->ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(prDecPrm->ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(prDecPrm->ucEsId);
	prVdecEsInfoKeepMain = _VDEC_GetEsInfoKeep(ucMainEsId);
    
    prDecPrm->u4PicW = (prDecPrm->prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
    prDecPrm->u4PicH = ((2 - prDecPrm->prSPS->fgFrameMbsOnlyFlag) * (prDecPrm->prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32x
	if (TRUE == prVdecEsInfoKeepMain->fgVPush)
	{
		prVdecEsInfoKeep->fgVPush = TRUE;
	}

    if((0 == prDecPrm->u1CurViewIdx) &&
        ((prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastPicW != prDecPrm->u4PicW) || 
        (prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastPicH != prDecPrm->u4PicH)))
    {
        LOG(0, "vLAEPartitionDPB\n");
        vLAEPartitionDPB(prDecPrm);
        prDecPrm->u4MaxFBufNum = prDecPrm->au4MaxFBufNum[prDecPrm->u1CurViewIdx];
    }
    
    if(fgIsIDRPic(prDecPrm->u1NalUnitType))
    {        
        prDecPrm->u1DecFBufIdx = 0;
        prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum = 0xffffffff;
        vLAEFlushDPB(prDecPrm);
        
        if (prDecPrm->fgIsMVC)
        {
            prDecPrm->rLastInfo[DEPEND_VIEW].u4LastFrameNum = 0xffffffff;
            prDecPrm->u1CurViewIdx = DEPEND_VIEW;   //for _VDEC_FlushDPB
            vLAEFlushDPB(prDecPrm);
            prDecPrm->u1CurViewIdx = BASE_VIEW;
        }
    } 

    u4UnusedShortTermFrameNum = (prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum + 1) %
                                prDecPrm->prSPS->u4MaxFrameNum;
    u4CurrFrameNum = prDecPrm->prSliceHdr->u4FrameNum;

    if((prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState != VDEC_ST_PLAY_I_FRAME))
      && ((u4CurrFrameNum != prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum)
    && (u4CurrFrameNum != u4UnusedShortTermFrameNum))
    #ifdef ENABLE_MULTIMEDIA
    && (((!prDecPrm->fgIsMMPlay)&&(prDecPrm->u1PlayModeInDtv==IPBMode)) ||
    ((prDecPrm->u1PlayModeInDtv==IPBMode)&&((prVdecEsInfo->eContainerType>= SWDMX_FMT_MPEG2_TS) && (prVdecEsInfo->eContainerType< SWDMX_FMT_VC1_ES) && (prVdecEsInfo->u4MMSpeed < STC_SPEED_TYPE_FORWARD_2X)))
    || (prDecPrm->fgIsMMPlay && (prVdecEsInfo->u4MMSpeed <= STC_SPEED_TYPE_FORWARD_2X)))
    && (!prDecPrm->fgIsMVC))
    #else
    && (!prDecPrm->fgIsMMPlay) && (!prDecPrm->fgIsMVC))
    #endif
    {
        if (prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum < (UINT32)0xffffffff)
        {
            LOG(0, "LAE FrmGap!%d\n",
            (u4CurrFrameNum > u4UnusedShortTermFrameNum) ?
            (u4CurrFrameNum - u4UnusedShortTermFrameNum) :
            ((prDecPrm->prSPS->u4MaxFrameNum - u4UnusedShortTermFrameNum) + u4CurrFrameNum));
            if (prDecPrm->prSPS->fgGapsInFrameNumValueAllowedFlag)
            {
                LOG(2, "FillFrmNumGap?\n");
            }

            if (!prVdecEsInfoKeep->fgVPush)
            {
            #ifdef ENABLE_MULTIMEDIA
                if (prVdecEsInfo->eMMSrcType != SWDMX_SRC_TYPE_WIFI_DISPLAY)
            #endif
                {
                    vLAEFlushDPB(prDecPrm);
                    _VDEC_ClrDPB(prDecPrm);
                    vLAEPartitionDPB(prDecPrm);   //reset DPB and MV buffer
                    prDecPrm->prSPS->fgSPSValid = FALSE;
                    prDecPrm->fgAllocFbFail = TRUE;
                    prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum = 0xffffffff;
                    prDecPrm->i4LastPOC[prDecPrm->u1CurViewIdx] = -99999;
                    prDecPrm->fgPOCWrap = FALSE;
                    prDecPrm->rSEI.u1ExtMatchFlg = FALSE;
                    prDecPrm->rSEI.u1WaitRAPCnt = 0;
                    prDecPrm->rSEI.u1WaitRAPRefCnt = 0;
                    prDecPrm->rSEI.u1WaitRAPICnt = 0;
                    prDecPrm->fgIsFirstFrm = FALSE;
                    return;
                }
            }
        }
    }
    if ((prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].fgLastIsRef && (!fgIsRefPic(prDecPrm->u1NalRefIdc))) ||
            (!prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].fgLastIsRef && fgIsRefPic(prDecPrm->u1NalRefIdc)))
    {
        fgRefChg = TRUE;
    }
    if ((prDecPrm->prCurrFBufInfo == NULL)|| 
                (prDecPrm->prCurrFBufInfo->u1FBufStatus == NO_PIC)
            || (prDecPrm->prCurrFBufInfo->u1FBufStatus & prDecPrm->u1PicStruct)
            || (u4CurrFrameNum != prDecPrm->rLastInfo[prDecPrm->u1CurViewIdx].u4LastFrameNum)
            || fgRefChg)
    {
        vLAEAllocateFBuf(prDecPrm);    //find new prDecPrm->prCurrFBufInfo
    }
    if (prDecPrm->prCurrFBufInfo)
    {
        if (fgIsRefPic(prDecPrm->u1NalRefIdc))
        {
            prDecPrm->prCurrFBufInfo->fgIsBufRef = TRUE;
        }
        prDecPrm->prCurrFBufInfo->u1FBufStatus |= prDecPrm->u1PicStruct;
        prDecPrm->u1PicStructKeep = prDecPrm->prCurrFBufInfo->u1FBufStatus;

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
    }   
}


// **************************************************************************
// Function : UINT32 u4VPmode(UINT32 u4VDecID, UINT32 u4Err_mbx, UINT32 u4Err_mby,
//                                     UINT32 u4mbx, UINT32 u4mby, VDEC_AVC_DecPrm *prDecPrm);
// Description : hw copy frame for error concealment 
// Parameter : u4VDecID : hw id, u4Err_mbx: error mbx, u4Err_mby: error mby, u4mbx: source mbx.
//                  u4mby: source mby, prDecPrm: H264 param
// Return      : none
// **************************************************************************
VOID u4VPmode(UINT32 u4VDecID, UINT32 u4Err_mbx, UINT32 u4Err_mby, UINT32 u4mbx, UINT32 u4mby, VDEC_AVC_DecPrm *prDecPrm)
{
    UCHAR ucRefFBIndex = 0;
    UINT32 u4MinDPOC = 0, i = 0;
    
    LOG(2, "VP mode start\n");
    //soft reset
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 1);
    vVDecWriteVLD(u4VDecID, WO_VLD_SRST, 0);

    //Apply the RISC setting in order
    vVDecWriteMISC(u4VDecID, INT_WAIT_BITS_NOP, 1); //set 1 for int_wait_bits_nop    
    vVDecWriteVLDTOP(u4VDecID, RW_VLD_PIC_MB_SIZE_M1, (u4mby << 16)| u4mby); //set mbx/mby
    vVDecWriteVLDTOP(u4VDecID, AVC_VLD_TOP_PIC_PIX_SIZE, (((prDecPrm->u4PicW & 0xffff) << 0 ) | ((prDecPrm->u4PicH & 0xffff) << 16 )));

    //for partial picture mode
    if (u4Err_mby - 3 < 0)
    {
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_START_MB, 0);        
    }
    else
    {
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_START_MB, (u4Err_mby - 3)); //start mvx/mby
    }
    vVDecWriteVLDTOP(u4VDecID, VLD_TOP_END_MB, ((u4mbx << 16) | u4mby)); //end mbx/mby
    
    if (1) 
    {
        // conceal-to-end from start x,y
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING, u4VDecReadVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING) & 0xffffffef);
    }
    else //partial picture mode, oryx E1 do not use this  
    {
        //non-conceal-to-end mode(set VLD_TOP_55)
        vVDecWriteVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING, u4VDecReadVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING) | 0x10);
    }
    vVDecWriteVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING, u4VDecReadVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING) | 0x2); //trun on vp mode flag    

    //find the neighbor poc
    u4MinDPOC = 0x7FFFFFFF;
    for (i = 0; i < prDecPrm->u4MaxFBufNum; i++ )
    {
        if (i == prDecPrm->u1DecFBufIdx)
        {
            continue;
        }
        if (abs(prDecPrm->prCurrFBufInfo->i4POC - prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC) < u4MinDPOC)
        {
            u4MinDPOC = abs(prDecPrm->prCurrFBufInfo->i4POC - prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC);
            ucRefFBIndex = i;
        } 
    }
    if (u4MinDPOC == 0x7FFFFFFF)
    {
        ucRefFBIndex= prDecPrm->u1PreFBIndex;
    }
    
    vVDecWriteMC(u4VDecID, AVC_MC_VP_GOLDEN_REF_YADDR, 
                ((UINT32)PHYSICAL(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][ucRefFBIndex].u4YAddr))>>9); //golden ref y address
    vVDecWriteMC(u4VDecID, AVC_MC_VP_GOLDEN_REF_CADDR, 
                ((UINT32)PHYSICAL(prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][ucRefFBIndex].u4CAddr))>>8); //golden ref c address
    vVDecWriteMC(u4VDecID, AVC_MC_VP_OUTPUT_YADDR, ((UINT32)PHYSICAL(prDecPrm->prCurrFBufInfo->u4YAddr))>>9); //output y address
    vVDecWriteMC(u4VDecID, AVC_MC_VP_OUTPUT_CADDR, ((UINT32)PHYSICAL(prDecPrm->prCurrFBufInfo->u4CAddr))>>8); //output c address
    
    vVDecWriteMC(u4VDecID, RW_MC_OPBUF, 0x1); //select mc 2/3 are the output buffers
    vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB, ((prDecPrm->u4PicW + 15)>> 4)); //horizontal_size_in_mb 
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, prDecPrm->u4PicW); //umv width
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, prDecPrm->u4PicH); //umv height
    
    if (1) //conceal-to-end
    {
        vVDecWriteMC(u4VDecID, (611 * 4), u4VDecReadMC(u4VDecID, (611 * 4)) & 0xfffffeff);
    }
    else //partial picture mode, oryx E1 do not use this  
    {
        vVDecWriteMC(u4VDecID, (611 * 4), u4VDecReadMC(u4VDecID, (611 * 4)) | 0x100);
    }
    vVDecWriteVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING,  u4VDecReadVLDTOP(u4VDecID, VLD_TOP_VP_MODE_SETTING) |0x1 );  // Trigger VP mode 
    LOG(2, "VP mode end\n");
    #ifndef AVC_IRQ_DISABLE
    LOG(2, "VP mode finish\n");
    #else
    //risc_val1 = u4VDecReadMISC(u4VDecID, RW_HEVC_DEC_COMPLETE);
    //vVDecWriteMISC (u4VDecID, RW_HEVC_DEC_COMPLETE, risc_val1 & (~(0x1<<12)) ) ;
    //return Wait_decode_finished( jiffies );
    #endif
}


// **************************************************************************
// Function : void vLAEFlushDPB(VDEC_AVC_DecPrm* prDecPrm);
// Description : LAE flush DPB
// Parameter : prDecPrm : AVC param
// Return      : none
// **************************************************************************
void vLAEFlushDPB(VDEC_AVC_DecPrm* prDecPrm)
{
    UINT32 u4FbCnt, i, u4MinPOCFBufIdx, u4SaveInfoIdx = 0xff;    
    INT32 i4MinPOC;
    
    for (u4FbCnt = 0; u4FbCnt < prDecPrm->u4MaxFBufNum; u4FbCnt++)
    {
        // Check if DPB full
        i4MinPOC = 0x7fffffff;
        u4MinPOCFBufIdx = 0xff;
        for (i = 0; i < prDecPrm->u4MaxFBufNum; i++)
        {
            if ((i4MinPOC > prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC) &&
                    (prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].u1FBufStatus != NO_PIC) &&
                    (i != u4SaveInfoIdx))
            {
                i4MinPOC = prDecPrm->arFBufInfo[prDecPrm->u1CurViewIdx][i].i4POC;
                u4MinPOCFBufIdx = i;
            }
        }
        if (u4MinPOCFBufIdx != 0xff)
        {
            _VDEC_ClrFBufInfo(prDecPrm, u4MinPOCFBufIdx + (((UINT32)prDecPrm->u1CurViewIdx) << 8));
        }
    }
}


// **************************************************************************
// Function : BOOL fgAVCMCoreMatchLAE(UINT32 u4InstID, VDEC_H264_INFO_T* prVdec);
// Description : match lae and dual core decoding frame
// Parameter : u4InstID : instance id, prVdec: vdec param
// Return      : return true if it can match
// **************************************************************************
BOOL fgAVCMCoreMatchLAE(UINT32 u4InstID, VDEC_H264_INFO_T* prVdec)
{
    VDEC_LAE_DecPrm *pLaeDecPrm;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_AVC_DecPrm *prDecPrm;
    
    prVdecEsInfo = _VDEC_GetEsInfo(u4InstID);
    prDecPrm = &prVdec->rDecParam;
    pLaeDecPrm = &(prDecPrm->rLAEDecParam);

    while(pLaeDecPrm->u4PesRdptr != prVdec->rPesInfo.u4VldReadPtr)
    {
        //Put used lae buffer to empty Q
        vPutBufferToLae(prVdecEsInfo->ucLaeEsInfoId,&(prDecPrm->rLAEDecParam));
        ucGetBufferForMcoreDec(u4InstID,&(prDecPrm->rLAEDecParam));
    }

    return TRUE;
}
#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



