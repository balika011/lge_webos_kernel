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
 * $RCSfile: swdmx_mp4.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_flw.h
 *  This header file declares exported APIs of MPV.
 */

#ifndef SWDMX_MP4_H
#define SWDMX_MP4_H


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "swdmx_drvif.h"
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_assert.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MP4_FIX_REPEAT_FRAME 32
#define MP4_MINIMUM_KEY_NUM 10
#define MP4_MAX_REPEAT_FRAME 20
#define MP4_MIN_REPEAT_FRAME 10
#define MP4_SUBTITLE_BUFSIZE 64 * 1024
#define DEMUX_MP4_AV_PERIOD   90000 

#define MP4_MAX_CTTS_PTS 9000           //0.1s
#define MP4_MAX_AUDIO_TRACK 8
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
/*
typedef struct {

    BOOL fgEnVideo;
    BOOL fgEnAudio;

    BOOL fgBeginToPlay;
    BOOL fgFeederInvalid;

    BOOL fgDeliverVEos;
    BOOL fgDeliverAEos;

    BOOL fgBlockVid;
    BOOL fgBlockAud;

    BOOL fgAudIsReady;

    BOOL fgPartialRangeDone;
    BOOL fgFeederLeftNotEnough;

    BOOL fgVDecIsPause;

    BOOL fgChgATrack;
    BOOL fgAudStrmDisable;
    BOOL fgWaitNewRange;
    BOOL fgRequestData;

    UINT32 ui4_data_offset;
    UINT64 u8_range_sz;
    
    UINT32 u4VDecType;
    UINT32 u4ADecType;

    UINT32 u4VidHeaderPtr;
    UINT32 u4VidHdrLength;

    UINT8 u1CurATrack;
    UINT8 u1TotalATrack;

    UINT32 u4AudHeaderPtr;
    UINT32 u4AudHdrLength;

    UINT32 u4AudSampleRate;
    UINT32 u4AudChannelNs;

    UINT32 u4PrevAStrmID;

    UINT32 u4VStrmID;
    UINT32 u4AStrmID;

    UINT64 u8VidLatestOff;
    UINT64 u8AudLatestOff;
    
    BOOL fgEnVDmx;
    BOOL fgEnADmx;

    UINT32 u4VidFps;
    
    BOOL fgRepeatRequest;
    
    BOOL fgStartTrick;
    
    // For feeder usage
    UINT32 u4FeederReqID;
    UINT32 u4LeftBytesInFeeder;
    UINT32  u1StartCodeLenMinus1;
    UINT64 u8CurFilepos;
    UINT32 u4RecFeederWPtr;

    // Record current demuxing position.
    UINT32 u4CurDmxFeederRPtr;
    INT32 i4ReqReadTimes;

    MM_RANGE_ELMT_IDXTBL_T *prVidKeyIdxTbl;

    MM_RANGE_ELMT_IDXTBL_T * prVidIdxTbl;
    MM_RANGE_ELMT_IDXTBL_T * prAudIdxTbl[MP4_MAX_AUDIO_TRACK];

    BOOL fgIsSeek;
    BOOL fgRecSeekFlag;
    BOOL fgSeekAfterPause;
    UINT64 u8VSeekPts;
    UINT64 u8ASeekPts;
    UINT32 u4CurSentKey;

    BOOL fgHasAud;
    
    UINT32 u4NumSentVid;
    //UINT32 u4NumSentAud;

    BOOL fgSpeedChange;
    INT32 i4PrevSpeed;

    UINT32 u4CurVidPts;
    UINT32 u4CurAudPts;
    UINT32 u4LastVidPts;

    UINT32 u4TriggerAudPts;

    UINT32 u4CurRepeatFrameIdx;
    UINT32 u4VidRepeatFrame;

    UINT32 u4AudBufSize;

    // Range Information
    MM_RANGE_INFO_T *prDmxRangeInfo;
} SWDMX_MP4_INFO_T;
*/

typedef enum{    
    MP4_SRC_NORM_SOURCE = 0,
    MP4_SRC_3D_FRM_ALT,
    MP4_SRC_3D_TAB,  // Top and Bottom
    MP4_SRC_3D_SBS,  // Side by side    
    MP4_SRC_3D_SBS_HALF // Side by side half
}ENUM_MP4_SRC_3D_TYPE_T;


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

BOOL _SWDMX_Mp4Init(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eType);

/**
*/
void _SWDMX_Mp4SetRange(UINT8 u1SrcId, 
    SWDMX_RANGE_LIST_T *prRangeList, 
    UINT32 u4VidStrmID, 
    UINT32 u4AudStrmID, 
    UINT32 u4SubTitleID);


BOOL _SWDMX_Mp4SetDecoderInfo(UINT8 u1SrcId,
    UINT8 u1Type, 
    UINT32 u4Para1, 
    UINT32 u4Para2, 
    UINT32 u4Para3);

BOOL _SWDMX_Mp4GetInfo(UINT8 u1SrcId,
    UINT8 u1Type,
    UINT32* pu4Para1,
    UINT32* pu4Para2,
    UINT32* pu4Para3);

BOOL _SWDMX_Mp4SetStrmID(UINT8 u1SrcId,
    UINT8 u1HdrType,  
    UINT32 u4StrmID, 
    UINT32 u4StrmSubID);

/**
*/
SWDMX_HDR_PRS_STATE_T _SWDMX_Mp4HdrPrs(
    UINT8 u1SrcId,
    SWDMX_RANGE_LIST_T* prRangeList,
    SWDMX_CUR_CMD_T* prCmd);

BOOL _SWDMX_Mp4EnableStrm(UINT8 u1SrcId,
    UINT8 u1StrmType, 
    VOID* pfnCbFunc);

BOOL _SWDMX_Mp4DisableStrm(UINT8 u1SrcId,
    UINT8 u1StrmType);

BOOL _SWDMX_Mp4AudioCb(UINT8 u1SrcId,
    ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType);


BOOL _SWDMX_Mp4VideoCallback(const DMX_PES_MSG_T* prPes);

BOOL _SWDMX_Mp4AudioCallback(const DMX_AUDIO_PES_T* prPes);

/**
*/
BOOL _SWDMX_Mp4Stop(UINT8 u1SrcId);

void _SWDMX_Mp4SetKeyTableIdx(SWDMX_INFO_T* prSwdmxInst,MM_RANGE_ELMT_IDXTBL_T* prTargetKeyTable);

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


#endif //VDEC_FLW_H


