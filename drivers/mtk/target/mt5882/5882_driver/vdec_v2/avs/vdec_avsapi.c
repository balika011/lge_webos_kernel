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
 * $RCSfile: vdec_avsapi.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_avsapi.c
 *  This file contains implementation of exported APIs of VDEC.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_timer.h"
#include "x_lint.h"
#include "vdec_avspsr.h"
#include "vdec_avsdec.h"
#include "vdec_avsutil.h"
#include "vdec_avsapi.h"
#include "vdec_avshw.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_chip_id.h"
#include "x_common.h"
#include "x_typedef.h"
#include "dmx_if.h"
#ifdef FBM_DPB_CTRL
#include "fbm_drvif.h"
#endif
LINT_EXT_HEADER_END

#include "vdec_drvif.h"
#include "drv_dbase.h"
//for user data
#include "mpv_drvif.h"

#ifdef MULTIMEDIA_AUTOTEST_SUPPORT
#include "x_mm_common.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MAX_ES_NUM VDEC_MAX_ES
#define ES_ID_0 0

#define ATSC_IDENTIFIER_LE  0x34394147
#define EIA_CC    0x03
#define MPEG2_USER_DATA_SC 0x000001b2   // start code

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static VDEC_AVS_INFO_T rAVSDecInfo[MAX_ES_NUM];

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Typedef
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

/** .
*@param .
 */
INT32 _VDEC_AVSInit(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    UINT32 i = 0;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = & (rAVSDecInfo[ucEsId]);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);

    //reset data structure
    x_memset((void *)prVdec, 0, sizeof(VDEC_AVS_INFO_T));    

    prVdec->rDecParam.bWaitDecLock = TRUE;

    prVdec->rDecParam.ucFbgId = FBM_FBG_ID_UNKNOWN;
    prVdec->rDecParam.ucFbId = FBM_FBG_ID_UNKNOWN;

    prVdec->rDecParam.u4WaitFbTime = AVS_WAIT_DISP_TIME;

    prVdec->rPicHdr.u4PicRefFg = 1;
    prVdec->rPicHdr.u4AdvPredModeDisable = 1;

    for (i = 0; i < MAX_FRM_BUFNUM; i++)
    {
        _VDEC_ClrAvsFBufInfo(&prVdec->rDecParam, i, TRUE);
    }

#ifdef CC_AVS_EMULATION
#ifdef CC_AVS_MODIFICATION_FOR_DDR3
    prVdec->rDecParam.u4AddrMode = ADDR_MODE_5351_NORMAL;
#else 
    prVdec->rDecParam.u4AddrMode = ADDR_MODE_8520;
#endif
    prVdec->rDecParam.u4WPtr = 0xFFFFFFFF;
#else
#ifdef CC_AVS_MODIFICATION_FOR_DDR3
    prVdec->rDecParam.u4AddrMode = ADDR_MODE_5351_NORMAL;
#else
    prVdec->rDecParam.u4AddrMode = ADDR_MODE_5351_NORMAL;
#endif
#endif

    // Enable PP
    prVdec->rDecParam.fgEnPP = TRUE;

    // Reset bit count
    prVdec->rDecParam.u4BitCnt = 0;

    for (i = 0; i < MAX_REF_PICNUM; i++)
    {
        prVdec->rDecParam.arRefFBufInfo[i].ucFbId = FBM_FB_ID_UNKNOWN;
        prVdec->rDecParam.arRefFBufInfo[i].fgValid = TRUE;
    }
    
    LOG(6, "%s, ES %d\n", __FUNCTION__, ucEsId);

    prVdec->rDecParam.u1LastRefIdx = 0;

#ifndef CC_AVS_EMULATION
    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prVdec->rPesInfo.ucMpvId, VLD_LOCK_LOG_AVS_BASE+1);
    #endif
#endif

    //init hw module, fifo address is 0 now
    _VDEC_InitAVSVDecHW(ucEsId,FALSE);

#ifndef CC_AVS_EMULATION
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 1);
    #endif
    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);    
#endif

    return (INT32)E_VDEC_OK;
}


/** unit of es packet is a nalu.
*@param .
 */
INT32 _VDEC_AVSHdrPrsHdr(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    UINT32 u4ByteDif = 0;
    INT32 i4RetValue = (INT32)E_VDEC_OK;
#ifdef VDEC_TIME_PROFILE
    HAL_TIME_T rTimeRstS, rTimeRstE, rTimeRstDt;
#endif
    DRV_DBASE_T* _prDbase = NULL;
    CRIT_STATE_T _rCritState;
    PARAM_MPV_T* prInfo;
    VDEC_INFO_T* prVdecInfo = _VDEC_GetInfo();
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    _prDbase = DBS_Lock(&_rCritState);
    ASSERT(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, _rCritState));
    prInfo = &(_prDbase->rMpv[0]);

    UNUSED(ucVdecId);
    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    prVdec->rDecParam.u4ByteCnt = 0;
    prVdec->rDecParam.fgSecField = FALSE;

    if(!prVdecEsInfo->fgMMPlayback)
    {
        u4ByteDif = (prVdec->rDecParam.u4WPtr >= prVdec->rPesInfo.u4VldReadPtr) ?
                          (prVdec->rDecParam.u4WPtr - prVdec->rPesInfo.u4VldReadPtr) : 
                          ((prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4VldReadPtr) + (prVdec->rDecParam.u4WPtr - prVdec->rPesInfo.u4FifoStart));
	 
        if (u4ByteDif < WPTR_OFFSET)
        {
            x_thread_delay(1);
            return E_VDEC_REPARSE;
        }
    }

#ifdef CC_AVS_EMULATION
    if ( prVdec->rPesInfo.fgEos)
    {
        return E_VDEC_BS_END;
    }

    if (prVdec->rPesInfo.u4VldReadPtr == prVdec->u4PrevPrsPtr)
    {
        LOG(3, "ES %d Duplicate read pointer\n",ucEsId);
        return E_VDEC_NOT_CODED_FRAME;
    }

    prVdec->u4PrevPrsPtr = prVdec->rPesInfo.u4VldReadPtr;

    //LOG(3, "Video offset = 0x%08x\n", (prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart));

#ifdef CC_DUMP_AVS_RISC_SETTING
    if (//((prVdec->rDecParam.u4PicCnt == 10)) &&
        ((prVdec->rPesInfo.ucPicType == I_PICTURE_SC) || (prVdec->rPesInfo.ucPicType == PB_PICTURE_SC)))
    {
        prVdec->rDecParam.fgDumpSetting = TRUE;
    }
    else
    {
        prVdec->rDecParam.fgDumpSetting = FALSE;
    }
#endif

#endif

//    _VDEC_AVSSWRst(ucVdecId, ucEsId);

#ifndef CC_AVS_EMULATION
    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_AVS_BASE+2);
    #endif
#endif

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeRstS);
#endif

#ifndef CC_AVS_MODIFICATION_AFTER_MT5368
        _VDEC_VLDAvsWaitForSramStable();
#endif
    
        _VDEC_VLDAvsHWReset(ucVdecId);

    if (prVdec->rPesInfo.ucPicType <= VIDEO_EDIT_SC)
    {
        _VDEC_SetAvsBsInfo(ucEsId,BARREL1, &prVdec->rDecParam, prVdec->rPesInfo.u4VldReadPtr, prVdec->rDecParam.u4WPtr,
                           prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd,
                           prVdec->rDecParam.u1ECMode, prVdec->rDecParam.u4ECType);
    }
    else
    {
        // I frame case
        if (prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
        {
            if (prVdecEsInfoKeep && (prVdecEsInfoKeep->pfDecNfyFct))
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_ERROR, 0, 0);
            }
        }
        
        LOG(6, "AVS ES(%d) E_VDEC_PARSE_NOT_READY, ucPicType:0x%x\n", ucEsId, prVdec->rPesInfo.ucPicType);

        #ifndef CC_AVS_EMULATION
        #if VLD_LOCK_LOG
        u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 7);
        #endif

        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        #endif

        return (INT32)E_VDEC_PARSE_NOT_READY;
    }

#ifdef VDEC_TIME_PROFILE
    HAL_GetTime(&rTimeRstE);
    HAL_GetDeltaTime(&rTimeRstDt, &rTimeRstS, &rTimeRstE);
    prVdec->rDecParam.u4CurRstMs += (rTimeRstDt.u4Micros / 1000);
#endif

    // Turn Off Bit Stuffing Check
    AVS_VLD_WRITE32(RW_AVS_VLD_BITSTUFFING_SWITCH, 0);
    LOG(9, "AVS ES(%d) , ucPicType:0x%x\n", ucEsId, prVdec->rPesInfo.ucPicType);

    // Record the offset of each picture
    prVdec->rDecParam.u8Offset = prVdec->rPesInfo.u8Offset;

    switch (prVdec->rPesInfo.ucPicType)
    {
        case SEQ_HDR_SC:            
            _VDEC_AvsSeqHdr(ucEsId,prVdec, &i4RetValue);
            break;
        case I_PICTURE_SC:
        case PB_PICTURE_SC:
            _VDEC_AvsPicHdr(ucEsId,prVdec, &i4RetValue);

            if ((i4RetValue != (INT32)E_VDEC_OK) && (i4RetValue != (INT32)E_VDEC_OK_RES_CHG))
            {
                prInfo->u4VpsrDropNs++;
                if (!VDEC_SetRptr(ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
                {
                    LOG(1, "AVS ES(%d) DMX_UpdVRPtr Fail\n",ucEsId);
                }
            }
            break;
        case SEQ_END_SC:
            LOG(5, "ES(%d)//END_OF_SEQ\n",ucEsId);
            //clean FBM buffer?
            //SWDMX will send EOS when the whole bitstream finished
            LOG(5, "Ignore SEQ_END_SC,Skip this ES\n");
            i4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
            break;
        case EXTENSION_SC:
            _VDEC_AvsExtHdr(ucEsId, prVdec, &i4RetValue);
            break;
        case USER_DATA_SC:
        case VIDEO_EDIT_SC:
        default:
            // I frame case
            if (prVdecEsInfoKeep && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
            {
                if (prVdecEsInfoKeep && (prVdecEsInfoKeep->pfDecNfyFct))
                {
                    prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_COND_ERROR, 0, 0);
                }
            }
            i4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
            break;
    }

    // Record the last picture type
    prVdec->rDecParam.ucLastUnitType = prVdec->rPesInfo.ucPicType;
    if ( prVdec->rDecParam.ucLastUnitType == SEQ_HDR_SC )
    {
        prVdec->rDecParam.fgAfterSeqHdrBeforePicHdr = TRUE;
    }
    if ( prVdec->rDecParam.fgAfterSeqHdrBeforePicHdr )
    {
        if ( prVdec->rDecParam.ucLastUnitType == I_PICTURE_SC  || prVdec->rDecParam.ucLastUnitType == PB_PICTURE_SC  )
        {
            prVdec->rDecParam.fgAfterSeqHdrBeforePicHdr = FALSE;
        }
    }

    if ((i4RetValue != (INT32)E_VDEC_OK) && (i4RetValue != (INT32)E_VDEC_OK_RES_CHG))
    {
        prVdecInfo->arVdecEsInfo[ucEsId].arRcvPesCnt[VDEC_FMT_AVS]--;
    }

    if ((!((i4RetValue == E_VDEC_OK) || (i4RetValue == E_VDEC_OK_RES_CHG) || (i4RetValue == E_VDEC_FAIL))) && (!(prVdec->rPesInfo.fgEos)))
    {
#ifndef CC_AVS_EMULATION
        #if VLD_LOCK_LOG
        u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 2);
        #endif
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
#endif
    }

    //LOG(0, "AVS ES(%d) \n",ucEsId);

    return i4RetValue;
}


/** .
*@param .
 */
INT32 _VDEC_AVSDecStart(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    UINT32 u4ByteDif = 0;
    INT32 i4RetValue = E_VDEC_OK;

    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    UNUSED(ucVdecId);
    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (!prVdecEsInfo)
    {
        return E_VDEC_FAIL;
    }

#if 0
    if ((prVdec->rPicHdr.u4ProgFrm) ||
            ((!prVdec->rPicHdr.u4ProgFrm) && (!prVdec->rDecParam.fgSecField)))
#endif
        if ((prVdec->rSeqHdr.u4IsProgSeq) ||
                ((!prVdec->rSeqHdr.u4IsProgSeq) && (!prVdec->rDecParam.fgSecField)))
        {
            UINT32 u4CurRptr = 0;
            
            u4CurRptr = _VDEC_AVS_Get_VldRptr(ucEsId);
            if (!prVdecEsInfo->fgMMPlayback)
            {
                u4ByteDif = (prVdec->rDecParam.u4WPtr >= u4CurRptr) ?
                                     (prVdec->rDecParam.u4WPtr - u4CurRptr) : 
                                     ((prVdec->rPesInfo.u4FifoEnd - u4CurRptr) + (prVdec->rDecParam.u4WPtr - prVdec->rPesInfo.u4FifoStart));

                if (u4ByteDif < WPTR_OFFSET)
                {
                    x_thread_delay(1);
                    return E_VDEC_RETRIGGER;
                }
            }	  
            #if VLD_LOCK_LOG
            u4VLDLogWr(ucEsId, ucVdecId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 3);  
            #endif
            _VDEC_UnlockVld(ucEsId, ucVdecId);
            _VDEC_AvsPrepareFBufInfo(ucEsId,prVdec);
            _VDEC_LockVld(ucEsId,ucVdecId);
            #if VLD_LOCK_LOG
            u4VLDLogWr(ucEsId,ucVdecId, VLD_LOCK_LOG_AVS_BASE+3);
            #endif
            
            _VDEC_AVSSWRst(ucVdecId, ucEsId);
            
            _VDEC_SetAvsBsInfo(ucEsId,BARREL1, &prVdec->rDecParam,  u4CurRptr, prVdec->rDecParam.u4WPtr,
                           prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd,
                           prVdec->rDecParam.u1ECMode, prVdec->rDecParam.u4ECType);
            
            prVdec->rDecParam.u4PicCnt++;
            if (prVdec->rDecParam.fgAllocFbFail)
            {
                LOG(0, "ES(%d) allocate FB fail!!\n", ucEsId);
                return E_VDEC_FAIL;
            }
        }

    if ((!prVdec->rSeqHdr.u4IsProgSeq) && (prVdec->rDecParam.fgSecField))
    {
        UINT32 u4ReadPtr = prVdec->rPesInfo.u4VldReadPtr;

        // In order to keep R/W ptr's distance, add this kind of waiting mechanism for DTV.
        if (!prVdecEsInfo->fgMMPlayback)
        {
            u4ByteDif = (prVdec->rDecParam.u4WPtr >= u4ReadPtr) ?
                                 (prVdec->rDecParam.u4WPtr - u4ReadPtr) : 
                                 ((prVdec->rPesInfo.u4FifoEnd - u4ReadPtr) + (prVdec->rDecParam.u4WPtr - prVdec->rPesInfo.u4FifoStart));

            if (u4ByteDif < WPTR_OFFSET)
            {
                x_thread_delay(1);
                return E_VDEC_RETRIGGER;
            }
        }

        _VDEC_AVSSWRst(ucVdecId, ucEsId);

        _VDEC_SetAvsBsInfo(ucEsId,BARREL1,
                           &prVdec->rDecParam,
                           u4ReadPtr,
                           prVdec->rDecParam.u4WPtr,
                           prVdec->rPesInfo.u4FifoStart,
                           prVdec->rPesInfo.u4FifoEnd,
                           prVdec->rDecParam.u1ECMode,
                           prVdec->rDecParam.u4ECType);
        //Printf("Decoder's Input Window: 0x%8x\n", AVS_VLD_READ32(0));
    }

    _VDEC_AvsDecProc(ucEsId,prVdec, &i4RetValue);
#ifdef VDEC_DEC_REG_QUERY
    VDEC_PrintDecReg( ucVdecId, e_DECODE_START_REG_DUMP );
#endif // VDEC_DEC_REG_QUERY
    return i4RetValue;
}


/** .
*@param .
 */
INT32 _VDEC_AVSDecFin(UCHAR ucVdecId, UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    INT32 i4RetValue;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;
    VDEC_INFO_T* prVdecInfo = _VDEC_GetInfo();

    UNUSED(ucVdecId);
    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);

    _VDEC_OutputAvsDispPic(&prVdec->rDecParam, &i4RetValue);    //vChkOutputFBuf

    if (_VDEC_WaitAvsDecDone(ucEsId,
                             prVdec,
                             prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus,
                             prVdec->rDecParam.prCurrFBufInfo->u4W,
                             prVdec->rDecParam.prCurrFBufInfo->u4H,
                             prVdec->rDecParam.bWaitDecLock) > 0)
    {
        if (prVdec->rSeqHdr.u4IsProgSeq)
        {
            i4RetValue = (INT32)E_VDEC_OK;
            //prVdec->rDecParam.u4PicCnt++;
        }
        else
        {
            if (prVdec->rPicHdr.u4ProgFrm)
            {
                LOG(5, "ES(%d) Succeed to decode the progressive frame in the interlace bitstream\n", ucEsId);
                i4RetValue = (INT32)E_VDEC_OK;
                prVdec->rDecParam.fgSecField = FALSE;   //Can check if finish a frame
            }

            // Need to re-trigger video decoder
            if (!prVdec->rPicHdr.u4PicStruct)
            {
                if (!prVdec->rDecParam.fgSecField)
                {
                    LOG(6, "ES(%d) Re-Trigger HW\n", ucEsId);
                    i4RetValue = (INT32)E_VDEC_RETRIGGER;
                    prVdec->rDecParam.fgSecField = TRUE;
                    prVdec->rDecParam.u4ByteCnt = _VDEC_AvsGetByteCnt();
                }
                else
                {
                    LOG(6, "ES(%d) Succeed to re-trigger HW\n", ucEsId);
                    i4RetValue = (INT32)E_VDEC_OK;
                    prVdec->rDecParam.fgSecField = FALSE;   //Can check if finish a frame
                    //prVdec->rDecParam.u4PicCnt++;
                }
            }
        }
    }
    else
    {
        i4RetValue = (INT32)E_VDEC_FAIL; //Timeout, decode error
        LOG(3, "ES(%d) Fail to decode %d\n", prVdec->rDecParam.u4PicCnt, ucEsId);
        //prVdec->rDecParam.u4PicCnt++;
    }

    if (i4RetValue != E_VDEC_RETRIGGER)
    {
        _VDEC_AvsUpdateFBList(ucEsId,prVdec/*, 1*/);
    }

    _VDEC_AvsFrmFin(&prVdec->rDecParam);

#if defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)
#if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    if(VDEC_IsTestCRC(ucEsId))
#endif
    {
        if ( i4RetValue == (INT32)E_VDEC_FAIL )
        {
            //prVdecEsInfo->u4CrcResult = 0x0;
        }
        else
        {
            UINT8 i = 0;

            if (prVdec->rSeqHdr.u4IsProgSeq)
            {
                for (i = 0; i < 4; i++)
                {
                    prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_Y_0 + i * 4);
                    prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_C_0 + i * 4);
                }
            }
            else
            {
                if ( i4RetValue == (INT32)E_VDEC_RETRIGGER ) //TOP
                {
                    for (i = 0; i < 4; i++)
                    {
                        prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_Y_0 + i * 4);
                        prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_C_0 + i * 4);
                    }
                }
                if ( i4RetValue == (INT32)E_VDEC_OK ) //BOTTOM
                {
                    for (i = 0; i < 4; i++)
                    {
                        prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_Y_0 + i * 4);
                        prVdecEsInfo->u4CrcResult ^= AVS_CRC_READ32(RW_CRC_C_0 + i * 4);
                    }
                }
            }
        }
    }
#endif

#ifdef CC_AVS_EMULATION_CRC
    if ( i4RetValue == (INT32)E_VDEC_FAIL )
    {
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[0] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[1] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[2] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[3] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[4] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[5] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[6] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_0[7] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[0] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[1] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[2] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[3] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[4] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[5] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[6] = 0x0;
        prVdec->rDecParam.rPicCRC.u4PicCRC_1[7] = 0x0;

        prVdec->rDecParam.rPicCRC.u1PicCRCCnt = 0;
    }
    else
    {
#if 0
        LOG(0, "++++++++++++++++++++\n");
        LOG(0, "RW_CRC_Y_0 = 0x%X\n", AVS_CRC_READ32(RW_CRC_Y_0));
        LOG(0, "RW_CRC_Y_1 = 0x%X\n", AVS_CRC_READ32(RW_CRC_Y_1));
        LOG(0, "RW_CRC_Y_2 = 0x%X\n", AVS_CRC_READ32(RW_CRC_Y_2));
        LOG(0, "RW_CRC_Y_3 = 0x%X\n", AVS_CRC_READ32(RW_CRC_Y_3));
        LOG(0, "RW_CRC_C_0 = 0x%X\n", AVS_CRC_READ32(RW_CRC_C_0));
        LOG(0, "RW_CRC_C_1 = 0x%X\n", AVS_CRC_READ32(RW_CRC_C_1));
        LOG(0, "RW_CRC_C_2 = 0x%X\n", AVS_CRC_READ32(RW_CRC_C_2));
        LOG(0, "RW_CRC_C_3 = 0x%X\n", AVS_CRC_READ32(RW_CRC_C_3));
        LOG(0, "++++++++++++++++++++\n");
#endif
        if (prVdec->rSeqHdr.u4IsProgSeq)
        {
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[0] = AVS_CRC_READ32(RW_CRC_Y_0);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[1] = AVS_CRC_READ32(RW_CRC_Y_1);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[2] = AVS_CRC_READ32(RW_CRC_Y_2);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[3] = AVS_CRC_READ32(RW_CRC_Y_3);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[4] = AVS_CRC_READ32(RW_CRC_C_0);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[5] = AVS_CRC_READ32(RW_CRC_C_1);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[6] = AVS_CRC_READ32(RW_CRC_C_2);
            prVdec->rDecParam.rPicCRC.u4PicCRC_0[7] = AVS_CRC_READ32(RW_CRC_C_3);
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[0] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[1] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[2] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[3] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[4] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[5] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[6] = 0x0;
            prVdec->rDecParam.rPicCRC.u4PicCRC_1[7] = 0x0;

            prVdec->rDecParam.rPicCRC.u1PicCRCCnt = 1;
        }
        else
        {
            if ( i4RetValue == (INT32)E_VDEC_RETRIGGER )
            {
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[0] = AVS_CRC_READ32(RW_CRC_Y_0);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[1] = AVS_CRC_READ32(RW_CRC_Y_1);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[2] = AVS_CRC_READ32(RW_CRC_Y_2);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[3] = AVS_CRC_READ32(RW_CRC_Y_3);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[4] = AVS_CRC_READ32(RW_CRC_C_0);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[5] = AVS_CRC_READ32(RW_CRC_C_1);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[6] = AVS_CRC_READ32(RW_CRC_C_2);
                prVdec->rDecParam.rPicCRC.u4PicCRC_0[7] = AVS_CRC_READ32(RW_CRC_C_3);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[0] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[1] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[2] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[3] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[4] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[5] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[6] = 0x0;
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[7] = 0x0;
                LOG(0, "\nES(%d) ^-^ Top Or Bottom Field -> E_VDEC_RETRIGGER ^-^\n", ucEsId);
            }
            if ( i4RetValue == (INT32)E_VDEC_OK )
            {
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[0] = AVS_CRC_READ32(RW_CRC_Y_0);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[1] = AVS_CRC_READ32(RW_CRC_Y_1);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[2] = AVS_CRC_READ32(RW_CRC_Y_2);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[3] = AVS_CRC_READ32(RW_CRC_Y_3);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[4] = AVS_CRC_READ32(RW_CRC_C_0);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[5] = AVS_CRC_READ32(RW_CRC_C_1);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[6] = AVS_CRC_READ32(RW_CRC_C_2);
                prVdec->rDecParam.rPicCRC.u4PicCRC_1[7] = AVS_CRC_READ32(RW_CRC_C_3);
            }
            prVdec->rDecParam.rPicCRC.u1PicCRCCnt = 2;
        }
    }
#endif

#ifdef LOG_FOR_MC_MEASURE
{
    extern char _acLoadedBitstream[256];
    extern UINT32 _u4CurFrmIdx;
    CHAR ucPicType;
    BOOL fgIsFrame;
    
    if ( prVdec->rPicHdr.u4PicCodingType == 0 )
    {
        ucPicType = 'I';
    }
    if ( prVdec->rPicHdr.u4PicCodingType == 1 )
    {
        ucPicType = 'P';
    }
    if ( prVdec->rPicHdr.u4PicCodingType == 2 )
    {
        ucPicType = 'B';
    }
    if ( i4RetValue != (INT32)E_VDEC_FAIL )
    {
        if (prVdec->rSeqHdr.u4IsProgSeq)
        {
            fgIsFrame = TRUE;
        }
        else
        {
            fgIsFrame = FALSE;
        }

            LOG(0, "ES(%d) VDEC_perf_measure:seq_name=%s,pic_idx=%d,pic_width=%d,pic_height=%d\n", ucEsId,_acLoadedBitstream, _u4CurFrmIdx, 
                prVdec->rSeqHdr.u4HSize, prVdec->rSeqHdr.u4VSize);
            LOG(0, "ES(%d) VDEC_perf_measure2:pic_type=%c,framepic=%d,CYCLE_DRAM=%ld\n", ucEsId,ucPicType, ((fgIsFrame == TRUE) ? 1:0),
                MC_READ32(RO_MC_DECODING_CYCLE));
            LOG(0, "ES(%d) VDEC_perf_measure3:MC_DLE_NUM=%ld,MC_MBX =%ld,MC_MBY =%ld\n", ucEsId,MC_READ32(RO_MC_DLE_NUM), MC_READ32(RO_MC_MBX),
                MC_READ32(RO_MC_MBY));
            LOG(0, "ES(%d) VDEC_perf_measure4:CYC_SYS=%ld,INTRA_CNT=%ld,Y_BLK_CNT=%ld\n", ucEsId,MC_READ32(RO_MC_CYC_SYS), MC_READ32(RO_MC_INTRA_CNT),
                MC_READ32(RO_MC_Y_BLK_CNT));
            LOG(0, "ES(%d) VDEC_perf_measure5:C_BLK_CNT=%ld,WAIT_CNT=%ld,REQ_CNT=%ld\n", ucEsId,MC_READ32(RO_MC_C_BLK_CNT), MC_READ32(RO_MC_WAIT_CNT),
                MC_READ32(RO_MC_REQ_CNT));
            LOG(0, "ES(%d) VDEC_perf_measure6:PARA4_BEHAVIOR=%d,PARA8_BEHAVIOR=%d\n", ucEsId,((MC_READ32(RW_MC_PARA_BEHAVIOR)&PARA4_BEHAVIOR)?1:0), 
                ((MC_READ32(RW_MC_PARA_BEHAVIOR)&PARA8_BEHAVIOR)?1:0));            
/*
            LOG(0, "VDEC_perf_measure:n=%s,idx=%d,w=%d,h=%d,type=%c,%s,cycle=%ld\n", _acLoadedBitstream, _u4CurFrmIdx, prVdec->rSeqHdr.u4HSize,
                prVdec->rSeqHdr.u4VSize, ucPicType, ((fgIsFrame == TRUE) ? "frame" : "field"), MC_READ32(RO_MC_DECODING_CYCLE));
            LOG(0, "VDEC_perf_measure2:MC_DLE_NUM=%ld,MC_MBX=%ld,MC_MBY=%ld\n", MC_READ32(RO_MC_DLE_NUM), MC_READ32(RO_MC_MBX), MC_READ32(RO_MC_MBY));
*/
    }    
}
#endif

    //I frame case
    if (prVdecInfo && (prVdecEsInfoKeep->eCurState == VDEC_ST_PLAY_I_FRAME))
    {
    }

    if (prVdec->rPesInfo.fgEos)
    {
        _VDEC_FlushAvsDPB(prVdec);
    }

    if ((!((i4RetValue == E_VDEC_RETRIGGER) || (i4RetValue == E_VDEC_FAIL))) &&
            (!(prVdec->rPesInfo.fgEos)))
    {
#ifndef CC_AVS_EMULATION
        #if VLD_LOCK_LOG
        u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 4);
        #endif

        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);

#endif
    }
#ifdef VDEC_DEC_REG_QUERY
    VDEC_PrintDecReg( ucVdecId, e_DECODE_DONE_REG_DUMP );
#endif // VDEC_DEC_REG_QUERY

    return i4RetValue;
}


/** .
*@param .
 */
void _VDEC_AVSSWRst(UCHAR ucVdecId, UCHAR ucEsId)
{
    //VDEC_AVS_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    //prVdec = (VDEC_AVS_INFO_T*)&rAVSDecInfo[ucEsId];

#ifndef CC_AVS_EMULATION
    _VDEC_LockVld(ucEsId, ucVdecId);
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVdecId, VLD_LOCK_LOG_AVS_BASE+4);
    #endif
#endif

#ifndef CC_AVS_MODIFICATION_AFTER_MT5368
    _VDEC_VLDAvsWaitForSramStable();
#endif

    _VDEC_VLDAvsHWReset(ucVdecId);
#if 0
    _VDEC_LogSimPat(prVdec->rDecParam.fgLogSimPat);
#endif

#ifndef CC_AVS_EMULATION
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, ucVdecId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 5);
    #endif

    _VDEC_UnlockVld(ucEsId, ucVdecId);
#endif

    UNUSED(ucVdecId);
    UNUSED(ucEsId);
}


/** .
*@param .
 */
void _VDEC_AVSStop(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (!prVdecEsInfo)
    {
        LOG(3, "ES(%d) Error Es info is NULL in _VDEC_AVSStop\n", ucEsId);
        return;
    }

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    if (prVdec->rDecParam.ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(prVdec->rDecParam.ucFbgId,20);    // block until display unlock            
        FBM_ReleaseGroup(prVdec->rDecParam.ucFbgId);
        prVdec->rDecParam.ucFbgId = FBM_FBG_ID_UNKNOWN;
    }

/*
    _VDEC_LockFlushMutex(ucEsId);

    if (prVdecEsInfo->fgFlushEsmQ)
    {
        _VDEC_FlushEs(ucEsId);
        prVdecEsInfo->fgFlushEsmQ = FALSE;
        _VDEC_UnlockFlushSema(ucEsId);
    }
    _VDEC_ClearEsInfo(ucEsId);

    _VDEC_UnlockFlushMutex(ucEsId);
*/

    LOG(5, "ES(%d) Total Frm Cnt = %d, Timeout Cnt = %d\n", ucEsId,
        prVdec->rDecParam.u4PicCnt,
        prVdec->rDecParam.u4TimeOutNum);

#ifdef CC_AVS_EMULATION_CHK_TIMEOUT

    if ( prVdec->rDecParam.u4TimeOutNum > 0 )
    {
        LOG(0, "\n ES(%d) ********\nFail For Time Out Num\n********\n", ucEsId);
        LOG(0, "\n ES(%d) ********\nu4TimeOutNum = 0x%X\n********\n", ucEsId,prVdec->rDecParam.u4TimeOutNum);
        ASSERT(FALSE);
    }
    
#endif

}


/** .
*@param .
 */
void _VDEC_AVSGetPesStruct(UCHAR ucEsId, void **ppvPesInfo)
{
    VDEC_AVS_INFO_T* prVdec;

    ASSERT(ppvPesInfo != NULL);
    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    if (prVdec->rDecParam.u1RptFrm == 2)
    {
        *ppvPesInfo = NULL;//(void*)(&prVdec->rPesInfoSkip);    //emulation repeat flw, skip pes info
    }
    else
    {
        *ppvPesInfo = (void*)(&prVdec->rPesInfo);
    }
}


/** for emulation
*@param .
 */
void _VDEC_AVSGetDispPic(UCHAR ucEsId, void* pvInfo)
{
    VDEC_AVS_INFO_T* prVdec = NULL;
    VDEC_DISP_PIC_INFO_T* prDispInfo = NULL;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep = NULL;

    ASSERT(pvInfo != NULL);
    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;

    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

    if (prVdecEsInfoKeep == NULL)
    {
        return;
    }

    prDispInfo->u4H = prVdec->rDecParam.u4LastVSize;
    prDispInfo->u4W = prVdec->rDecParam.u4LastHSize;
    prDispInfo->pvYAddr = (void*)(prVdec->rDecParam.pucDecWorkBufY);
    prDispInfo->u4YSize = prVdec->rDecParam.u4LastHSize * prVdec->rDecParam.u4LastVSize;
    prDispInfo->pvCAddr = (void*)prVdec->rDecParam.pucDecWorkBufC;
    prDispInfo->u4CSize = prDispInfo->u4YSize >> 1;
    prDispInfo->u1PicStruct = FRAME;

#ifdef CC_AVS_EMULATION_CRC
    prDispInfo->rPicCRC.u4PicCRC_0[0] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[0];
    prDispInfo->rPicCRC.u4PicCRC_0[1] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[1];
    prDispInfo->rPicCRC.u4PicCRC_0[2] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[2];
    prDispInfo->rPicCRC.u4PicCRC_0[3] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[3];
    prDispInfo->rPicCRC.u4PicCRC_0[4] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[4];
    prDispInfo->rPicCRC.u4PicCRC_0[5] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[5];
    prDispInfo->rPicCRC.u4PicCRC_0[6] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[6];
    prDispInfo->rPicCRC.u4PicCRC_0[7] = prVdec->rDecParam.rPicCRC.u4PicCRC_0[7];
    prDispInfo->rPicCRC.u4PicCRC_1[0] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[0];
    prDispInfo->rPicCRC.u4PicCRC_1[1] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[1];
    prDispInfo->rPicCRC.u4PicCRC_1[2] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[2];
    prDispInfo->rPicCRC.u4PicCRC_1[3] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[3];
    prDispInfo->rPicCRC.u4PicCRC_1[4] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[4];
    prDispInfo->rPicCRC.u4PicCRC_1[5] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[5];
    prDispInfo->rPicCRC.u4PicCRC_1[6] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[6];
    prDispInfo->rPicCRC.u4PicCRC_1[7] = prVdec->rDecParam.rPicCRC.u4PicCRC_1[7];

    prDispInfo->rPicCRC.u1PicCRCCnt = prVdec->rDecParam.rPicCRC.u1PicCRCCnt;
#endif

#if 0
    UINT8 u1BufIdx = 0;
    if (prVdec->rDecParam.fgOutDispOrder && (!prVdecEsInfoKeep->fgVdecMidEnable))
    {
        u1BufIdx = prVdec->rDecParam.u1MinPOCBufIdx;
    }
    else
    {
        u1BufIdx = prVdec->rDecParam.u1DecFBufIdx;
    }
    if (u1BufIdx >= 18)
    {
        ASSERT(u1BufIdx < 18);
        return;
    }

    prDispInfo->pvYAddr = (void*)prVdec->rDecParam.arFBufInfo[u1BufIdx].u4YAddr;
    prDispInfo->u4YSize = prVdec->rDecParam.arFBufInfo[u1BufIdx].u4DramPicSize;
    prDispInfo->pvCAddr = (void*)prVdec->rDecParam.arFBufInfo[u1BufIdx].u4CAddr;
    prDispInfo->u4CSize = prDispInfo->u4YSize >> 1;
    prDispInfo->pvMVAddr = (void*)prVdec->rDecParam.arFBufInfo[u1BufIdx].u4MVBufAddr;

    prDispInfo->u1PicStruct = prVdec->rDecParam.u1PicStruct;
    prDispInfo->u4W = prVdec->rDecParam.prCurrFBufInfo->u4W;
    prDispInfo->u4H = prVdec->rDecParam.prCurrFBufInfo->u4H;
    prDispInfo->u4ChromaFormatIdc = prVdec->rDecParam.prSPS->u4ChromaFormatIdc;
#endif

}


void _VDEC_AVSSetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    VDEC_AVS_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    if (u4Type == (UINT32)AVS_OUTPUT_DISP_ORDER)
    {
        if (u4Param1 > 0)           //compare enable, decoding order
        {
            prVdec->rDecParam.fgOutDispOrder = FALSE;
        }
        else
        {
            prVdec->rDecParam.fgOutDispOrder = TRUE;
        }
    }
    else if (u4Type == (UINT32)AVS_REPEAT_FRM)
    {
        //repeat frame
        if (u4Param1 > 0)
        {
            if (prVdec->rDecParam.u1RptFrm == 0)
            {
                prVdec->rDecParam.u1RptFrm = (UINT8)u4Param1;
            }
        }
        else
        {
            prVdec->rDecParam.u1RptFrm = 0;
        }
    }
    else if (u4Type == (UINT32)AVS_ECC_MODE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        prVdec->rDecParam.u1ECMode = (UINT8)((u4Param1 | (u4Param2 << 2)) | (u4Param3 << 1));
    }
    else if (u4Type == (UINT32)AVS_ECC_TYPE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        prVdec->rDecParam.u4ECType = u4Param1;
    }
    else if (u4Type == (UINT32)AVS_LOG_SIMU_PAT)
    {
        prVdec->rDecParam.fgLogSimPat = ((u4Param1 > 0) ? TRUE : FALSE);
    }
    else if (u4Type == (UINT32)AVS_WAIT_FB_TIME)
    {
        prVdec->rDecParam.u4WaitFbTime = u4Param1;
    }
    else if (u4Type == (UINT32)AVS_AV_SYNC_INFO)
    {
        prVdec->rDecParam.ucSyncMode = (UCHAR)u4Param1;
        prVdec->rDecParam.ucStcId = (UCHAR)u4Param2;
    }
    else if (u4Type == (UINT32)AVS_TIME_PROFILE2)
    {
        prVdec->rDecParam.u1DramMoniter = (UINT8)u4Param1;
    }
    else if (u4Type == (UINT32)AVS_MM_PARAM)
    {
        prVdec->rDecParam.fgIsMMPlay = (u4Param1 > 0) ? TRUE : FALSE;
        if (prVdec->rDecParam.fgIsMMPlay)
        {
            prVdec->rDecParam.u4WaitFbTime = VDEC_AVSYNC_WAIT_DISP_TIME;
        }
    }
}


void _VDEC_AVSGetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
    VDEC_AVS_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pu4Param1 != NULL);
    ASSERT(pu4Param2 != NULL);
    ASSERT(pu4Param3 != NULL);

    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    if (u4Type == (UINT32)AVS_DISP_IDX)
    {
        *pu4Param1 = (UINT32)prVdec->rDecParam.ucFbgId;
        if (prVdec->rDecParam.prCurrFBufInfo)
        {
            *pu4Param2 = (UINT32)prVdec->rDecParam.prCurrFBufInfo->ucFbId;
        }
    }
    else if (u4Type == (UINT32)AVS_OUTPUT_DISP_ORDER)
    {
        if (prVdec->rDecParam.fgOutDispOrder)           //compare enable, decoding order
        {
            *pu4Param1 = 0;
        }
        else
        {
            *pu4Param1 = 1;
        }
    }
    else if (u4Type == (UINT32)AVS_REPEAT_FRM)
    {
        *pu4Param1 = prVdec->rDecParam.u1RptFrm;
    }
    else if (u4Type == (UINT32)AVS_ECC_MODE)
    {
        //u4Param1: Enable error re-concealment, u4Param2: Mode, u4Param3: Dedicate error
        *pu4Param1 = (UINT32)(prVdec->rDecParam.u1ECMode & 0x1);
        *pu4Param2 = (UINT32)(prVdec->rDecParam.u1ECMode & 0xFC) >> 2;
        *pu4Param3 = (UINT32)(prVdec->rDecParam.u1ECMode & 0x2) >> 1;
    }
    else if (u4Type == (UINT32)AVS_ECC_TYPE)
    {
        *pu4Param1 = AVS_VLD_READ32(RW_AVLD_ERC_DED_ERR_TYPE);
    }
    else if (u4Type == (UINT32)AVS_TIME_PROFILE)
    {
        if ( prVdec->rSeqHdr.u4IsProgSeq
                || ( (prVdec->rSeqHdr.u4IsProgSeq == FALSE) && (prVdec->rDecParam.fgSecField == FALSE) ) )
        {
        *pu4Param1 = prVdec->rDecParam.ucFbgId;
        *pu4Param2 = prVdec->rDecParam.u4CurDecMs;       //hw dec
        *pu4Param3 = prVdec->rDecParam.u4CurWaitFbMicro / 1000; //wait fb time
    }
        else
        {
            *pu4Param1 = 0xFF;
        }
    }
    else if (u4Type == (UINT32)AVS_TIME_PROFILE2)
    {
#if 1
        *pu4Param1 = prVdec->rDecParam.u4CurRstMs;   //wait fb time
        prVdec->rDecParam.u4CurRstMs = 0;
        switch (prVdec->rPicHdr.u4PicCodingType)
        {
            case I_IMG:
                *pu4Param2 = 2;
                break;
            case P_IMG:
                *pu4Param2 = 0;
                break;
            case B_IMG:
                *pu4Param2 = 1;
                break;
        }
#if 0
        *pu4Param3 = 1;
#if 0
        *pu4Param3 = (UINT32)((prVdec->rDecParam.prCurrFBufInfo->u1FBufStatus == FRAME) ? 1 : 0);
#endif
#else
        if ( prVdec->rSeqHdr.u4IsProgSeq
                || ( (prVdec->rSeqHdr.u4IsProgSeq == FALSE) && (prVdec->rDecParam.fgSecField == FALSE) ) )
        {
            *pu4Param3 = 1;
        }
        else
        {
            *pu4Param3 = 0;
        }
#endif
#endif
    }
    else if (u4Type == (UINT32)AVS_TIME_PROFILE3)
    {
        *pu4Param1 = prVdec->rDecParam.u4CurSwMicro;
        *pu4Param2 = prVdec->rDecParam.u4CurDecCycle;
        *pu4Param3 = prVdec->rDecParam.u4CurDramCycle;
    }
    else if (u4Type == (UINT32)AVS_BS_TYPE)
    {
#if 0
        *pu4Param1 = prVdec->rDecParam.prSPS->u4ProfileIdc;
        *pu4Param2 = prVdec->rDecParam.prSPS->u4LevelIdc;
        *pu4Param3 = (UINT32)prVdec->rDecParam.prPPS->fgEntropyCodingModeFlag;
#endif
    }
    else if (u4Type == (UINT32)AVS_SEQ_HDR_INFO)
    {
#if 0
        *pu4Param1 = (prVdec->rDecParam.prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
        *pu4Param2 = ((2 - prVdec->rDecParam.prSPS->fgFrameMbsOnlyFlag) *
                      (prVdec->rDecParam.prSPS->u4PicHeightInMapUnitsMinus1 + 1)) << 4; //32
#endif
    }
}

#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL _VDEC_AVSIsPic(UCHAR ucEsId, VDEC_PES_INFO_T *prPes)
{
    VDEC_AVS_INFO_T* prVdec;
    UCHAR *pucBuf;
    UINT32 u4Addr;

    UNUSED(pucBuf);
    UNUSED(u4Addr);

    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    u4Addr = (UINT32)prPes->au1PicInfo;
    pucBuf = (UCHAR*)(u4Addr);

    if (pucBuf[0] == 0xB3 || pucBuf[0] == 0xB6)
    {
        if (prVdec->rSeqHdr.u4IsProgSeq)
        {
            return TRUE;
        }
        else
        {
            if ( prVdec->rDecParam.fgSecField == TRUE )
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }    
        }
    }
    else
    {
        return FALSE;
    }
}
#endif

void _VDEC_AVSDropFrame(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    UINT32 u4CurRpt;
    VDEC_ES_INFO_T *prVdecEsInfo = NULL;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    ASSERT(prVdecEsInfo != NULL);

    u4CurRpt = prVdec->rPesInfo.u4VldReadPtr;

    if (!VDEC_SetRptr(ucEsId, u4CurRpt, u4CurRpt))
    {
        LOG(3, "AVS ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);
    }

#ifndef CC_AVS_EMULATION
    #if VLD_LOCK_LOG
    u4VLDLogWr(ucEsId, prVdecEsInfo->u4VldId, VLD_LOCK_LOG_UNLOCK_FLAG + VLD_LOCK_LOG_AVS_BASE + 6);
    #endif

    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
#endif
}


void _VDEC_AVSUpdDmxRptr(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    //UINT32 u4CurRptr, u4UsedSz = 0;
    ASSERT(ucEsId < MAX_ES_NUM);

    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

#if 0
    _VDEC_GetAvsRptr(&u4CurRptr);

    u4UsedSz = (u4CurRptr > prVdec->rPesInfo.u4VldReadPtr) ? (u4CurRptr - prVdec->rPesInfo.u4VldReadPtr) :
               ((prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4VldReadPtr) + (u4CurRptr - prVdec->rPesInfo.u4FifoStart));

    if (u4UsedSz > (WPTR_OFFSET + RPTR_ALIGN))
    {
        u4CurRptr -= (WPTR_OFFSET + RPTR_ALIGN);
    }
    if (u4CurRptr < prVdec->rPesInfo.u4FifoStart)
    {
        u4CurRptr += (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
    }
#endif

    //update dmx pointer, use current pes start addr, hw rptr may go beyond frame end
    //if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, u4CurRptr, u4CurRptr))
     if (!VDEC_SetRptr(ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
    {
        LOG(2, "AVS ES(%d) DMX_UpdVRPtr Fail VS 0x%x, VE 0x%x, PicS 0x%x, DmxWptr 0x%x\n",
            prVdec->rPesInfo.ucEsId,
            prVdec->rPesInfo.u4FifoStart, prVdec->rPesInfo.u4FifoEnd,
            prVdec->rPesInfo.u4VldReadPtr, prVdec->rDecParam.u4WPtr);
    }

}

UINT32 _VDEC_AVS_Get_VldRptr(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    UINT32 u4CurRptr, u4UsedSz = 0;
    ASSERT(ucEsId < MAX_ES_NUM);

    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    _VDEC_GetAvsRptr(&u4CurRptr);

    u4UsedSz = (u4CurRptr > prVdec->rPesInfo.u4VldReadPtr) ? (u4CurRptr - prVdec->rPesInfo.u4VldReadPtr) :
               ((prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4VldReadPtr) + (u4CurRptr - prVdec->rPesInfo.u4FifoStart));

    if (u4UsedSz > (WPTR_OFFSET + RPTR_ALIGN))
    {
        u4CurRptr -= (WPTR_OFFSET + RPTR_ALIGN);
    }
    if (u4CurRptr < prVdec->rPesInfo.u4FifoStart)
    {
        u4CurRptr += (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
    }

    return u4CurRptr;
}

void _VDEC_AVSSetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    VDEC_AVS_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    prVdec->rDecParam.u4WPtr = u4Wptr;

#if 0
    _VDEC_SetAvsWptr(u4Wptr);
#endif
}


//skip ES if return FALSE
BOOL _VDEC_AVSEsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    UCHAR *pucPicType = NULL;
    UCHAR ucPBCodeType = 0x0;
    UINT32 u4Temp;

    VDEC_AVS_INFO_T* prVdec;
    VDEC_ES_INFO_T* prVdecEsInfo;

    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pucVldId != NULL);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
    if (prVdecEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    prVdec->rDecParam.ucSkipMode = ucSkipMode;
#if 1
    u4Temp =(UINT32)prVdec->rPesInfo.au1PicInfo;
#else
	u4Temp = prVdec->rPesInfo.u4VldReadPtr + 6;
	
    if (u4Temp >= prVdec->rPesInfo.u4FifoEnd)
    {
        u4Temp = prVdec->rPesInfo.u4FifoStart + (u4Temp - prVdec->rPesInfo.u4FifoEnd);
    }
#endif
    pucPicType = (UCHAR*)VIRTUAL(u4Temp);
    if (pucPicType)
    {
        ucPBCodeType = (*(pucPicType+3)) >> 6; //picture_coding_type
    }
    else
    {
        LOG(3, "ES(%d) Error Pic Type\n",ucEsId);
        return FALSE;
    }

    if (ucSkipMode == IPMode)
    {
        if ((prVdec->rPesInfo.ucPicType == PB_PICTURE_SC) && (ucPBCodeType == B_IMG))
        {
            _VDEC_AvsCalPTS(prVdec);
            return TRUE;
        }
    }
    else if (ucSkipMode == IMode)
    {
        if (prVdec->rPesInfo.ucPicType == PB_PICTURE_SC)
        {
            return TRUE;
        }
    }

    return FALSE;

#if 0
    // ask mpeg decode to skip one picture due to repeat
    // if ESM-Q Ns > Underflow Threshold, otherwise, ignore
    {
        //don't skip, for 2x->1x, decode but not display
        /*if(ucSkipMode == IPMode)
        {
            if(!_VDEC_SkipFrm(&prVdec->rDecParam))
            {
                return FALSE;
            }
            else
            {
                return TRUE;    //skip current packet
            }
        }*/

        UCHAR ucFbgId;

        VDEC_ES_INFO_T* prVdecEsInfo;

        ucFbgId = prVdec->rDecParam.ucFbgId;
        if (ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC))
            {
                //FBM_ClrFrameBufferFlag(ucFbgId, FBM_FLAG_SKIP_PIC);

                if (i1EsBufStatus >= 0)
                {
                    //update dmx
                    if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId,
                                                    prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
                    {
                        LOG(2, "EsFlwCtrl AVS ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);
                    }

                    // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
                    if (!_VDEC_AvsSkipFrm(&prVdec->rDecParam))
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
                                LOG(2, "EsFlwCtrl AVS ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);
                            }
                            // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
                            if (!_VDEC_AvsSkipFrm(&prVdec->rDecParam))
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

        //dmx input speed faster than consumption
        if (i1EsBufStatus > 0)
        {
            LOG(3, "Q %d ucDropBNs\n", ucEsId);

            //update dmx
            if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId,
                                            prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
            {
                LOG(2, "EsFlwCtrl AVS ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);
            }
            // [LOG] Video plane ask mpeg to drop 1 picture, set fbm empty if decode partial frame
            if (!_VDEC_AvsSkipFrm(prVdec))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        LOG(3, "DEC_FLG_WAIT_NEXT_I FlushDPB\n");
        _VDEC_FlushDPB(&prVdec->rDecParam);
        CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    }
    if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP))
    {
        LOG(3, "DEC_FLG_WAIT_AVC_RAP\n");
        prVdec->rDecParam.rSEI.u1ExtMatchFlg = FALSE;
        prVdec->rDecParam.rSEI.u1WaitRAPCnt = 0;
        prVdec->rDecParam.rSEI.u1WaitRAPRefCnt = 0;
        prVdec->rDecParam.rSEI.u1WaitRAPICnt = 0;
        prVdec->rDecParam.fgIsFirstFrm = FALSE;
#ifdef ENABLE_MULTIMEDIA
        if (((prVdecEsInfo->eContainerType >= SWDMX_FMT_MPEG2_TS) &&
                (prVdecEsInfo->eContainerType <= SWDMX_FMT_MPEG2_TS_TIME_SHIFT)) ||
                (prVdecEsInfo->eContainerType == SWDMX_FMT_AVS_VIDEO_ES))
        {
            prVdec->rDecParam.fgWaitPPS = TRUE;
        }
#endif
        CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_AVC_RAP);
    }
    if (ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        // Check if reset is required
        if (FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_RESET))
        {
            LOG(3, "Disp rst flag on~\n");

            _VDEC_FlushDPB(&prVdec->rDecParam);
            prVdec->rDecParam.rLastInfo.u4LastFrameNum = 0xffffffff;
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
        // Check if display is ready, skip picture if NOT
        if (!FBM_ChkFrameBufferFlag(ucFbgId, FBM_FLAG_DISP_READY))
        {
            LOG(3, "Disp Not Ready\n");
            *pucVldId = VDEC_MAX_VLD;
            return TRUE;    //skip current packet
        }
    }
#endif
}


void _VDEC_AVSFlush(UCHAR ucEsId, BOOL bIsDisp)
{
    VDEC_AVS_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];

    _VDEC_FlushAvsDPB(prVdec);
    UNUSED(bIsDisp);
}

BOOL _VDEC_IsSupportAVS()
{
    return IS_SupportAVS();
}

#if 0 //no used in current flow
UCHAR _VDEC_AVSHandleOverFlw(UCHAR ucVldId, UCHAR ucEsmQId, HANDLE_T hMutex,
                             HANDLE_T hMsgQueue, const void* pvPesInfo)
{
    ASSERT(pvPesInfo != NULL);
    //temp code
    do
    {
        x_thread_delay(1);
    }
    while (x_msg_q_send(hMsgQueue, pvPesInfo, sizeof(VDEC_PES_INFO_T), 255) != OSR_OK);

    UNUSED(ucVldId);
    UNUSED(ucEsmQId);
    UNUSED(hMutex);
    return 1;
}
#endif
VOID _VDEC_AVSSetIsr(UCHAR ucEsId)
{
    VDEC_AVS_INFO_T* prVdec;
    prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
    
    _VDEC_AvsInitIsr(prVdec);
}

#ifdef CC_SUPPORT_VDEC_PREPARSE
#define AVS_Seq_PICTYPE (0x000001B0)
INT32 _VDEC_AvsPrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
  UINT32 u4TempSC = 0;
  UCHAR u1TempB,u1TempFrameRate = 0;
  VDEC_ES_INFO_T* prVdecEsInfo;
  UCHAR *pucBuf;
  UINT32 u4Addr;
  INT32 i4RetValue = 0;
  VDEC_AVS_INFO_T* prVdec;
  VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep = NULL;

  prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
  prVdec = (VDEC_AVS_INFO_T*) & rAVSDecInfo[ucEsId];
  prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
  #if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
  u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
  #else
  u4Addr = prVdec->rPesInfo.u4VldReadPtr;
  #endif
  pucBuf = (UCHAR*)VIRTUAL(u4Addr);
  u4TempSC = (pucBuf[0] << 24) | (pucBuf[1] << 16) |(pucBuf[2]<<8) | (pucBuf[3]);
  prVdecEsInfo->fgProgressive = TRUE;

  if (u4TempSC == AVS_Seq_PICTYPE)
  {
	   u1TempB = ((*(pucBuf+6))&0x80);
	   if((u1TempB >> 7) == 0x1)
	   {
		  prVdecEsInfo->fgProgressive = TRUE;
	   }
	   else
	   {
		  prVdecEsInfo->fgProgressive = FALSE;
		
       }
   	   prVdecEsInfo->u2OrgHSize = 0;
       prVdecEsInfo->u2OrgVSize = 0;
       
       u1TempB = ((*(pucBuf+6))&0x7f);
       prVdecEsInfo->u2OrgHSize |= ((UINT16)u1TempB<<7);
       u1TempB = ((*(pucBuf+7))&0xfe)>>1;
       prVdecEsInfo->u2OrgHSize |= ((UINT16)u1TempB); 

       u1TempB = ((*(pucBuf+7))&0x01);
       prVdecEsInfo->u2OrgVSize |= ((UINT16)u1TempB<<13);
       u1TempB = ((*(pucBuf+8))&0xff);
       prVdecEsInfo->u2OrgVSize |= ((UINT16)u1TempB<<5);
       u1TempB = ((*(pucBuf+9))&0xf8)>>3;
       prVdecEsInfo->u2OrgVSize |= ((UINT16)u1TempB);

       u1TempB = ((*(pucBuf+10))&0x03);
       u1TempFrameRate |= u1TempB << 2;
       u1TempB = ((*(pucBuf+11))&0xc0)>>6;
       u1TempFrameRate |= u1TempB;
       
       switch(u1TempFrameRate)
       {
       case 0x01: /* 24000/1001 (23.976...) */
          prVdecEsInfo->u4FrameRate = 2397;
          break;
       case 0x02:
          prVdecEsInfo->u4FrameRate = 2400;
          break;
       case 0x03:
          prVdecEsInfo->u4FrameRate = 2500;
          break;
       case 0x04: /* 30000/1001 (29.97...) */
          prVdecEsInfo->u4FrameRate = 2997;
          break;
       case 0x05:
          prVdecEsInfo->u4FrameRate = 3000;
          break;
       case 0x06:
          prVdecEsInfo->u4FrameRate = 5000;
          break;
       case 0x07: /* 60000/1001 (59.94...) */
          prVdecEsInfo->u4FrameRate = 5994;
          break;
       case 0x08:
          prVdecEsInfo->u4FrameRate = 6000;
          break;
       default:
          prVdecEsInfo->u4FrameRate = 0;
          break;
       }
       LOG(1,"Hsize %d,Vsize %d,Fps %d\n",prVdecEsInfo->u2OrgHSize,prVdecEsInfo->u2OrgVSize,prVdecEsInfo->u4FrameRate);
  }
  else
  {
       LOG(0,"[AVS]: StartCode: 0x%x, PrePase Not Ready, Wait for next ES\n",u4TempSC);
       i4RetValue = E_VDEC_PARSE_NOT_READY;
  }
  return i4RetValue;
}
#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



