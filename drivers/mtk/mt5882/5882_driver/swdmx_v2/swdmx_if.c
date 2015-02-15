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
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: swdmx_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file swdmx_if.c
 *  Software program stream demux driver - public interface
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "swdmx_if.h"
#include "swdmx_drvif.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Imported functions
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** SWDMX_Init
 *  Initialize software program stream demux driver
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
VOID SWDMX_Init(VOID)
{
    _SWDMX_Init();
}


//-----------------------------------------------------------------------------
/** SWDMX_GetStreamInfo
 *  
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_GetStreamInfo(UINT8 u1SrcId,
    ENUM_SWDMX_FMT_T eType, 
        UINT32 u4InfoMask,
        UINT64 u8FileSize,
        SWDMX_PROGRAM_INFO_T* prPInfo)
{
    return _SWDMX_GetStreamInfo(u1SrcId, eType, u4InfoMask, u8FileSize, prPInfo);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetInfo
 *  
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32 u4Para1,UINT32 u4Para2,UINT32 u4Para3)
{
    return _SWDMX_SetInfo(u1SrcId, u1Type, u4Para1, u4Para2, u4Para3);
}

//-----------------------------------------------------------------------------
/** SWDMX_GetInfo
 *  
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_GetInfo(UINT8 u1SrcId, UINT8 u1Type,UINT32* pu4Para1,UINT32* pu4Para2,UINT32* pu4Para3)
{
    return _SWDMX_GetInfo(u1SrcId, u1Type, pu4Para1, pu4Para2, pu4Para3);
}

BOOL SWDMX_FilterLog(UINT8 u1SrcId, BOOL fgAdd, UINT32 u4LogFilter)
{
    return _SWDMX_FilterLog(u1SrcId, fgAdd, u4LogFilter);
}
//-----------------------------------------------------------------------------
/** SWDMX_Play
 *  Start to run software program stream demux
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_Play(UINT8 u1SrcId)
{
    return _SWDMX_Play(u1SrcId);
}


BOOL SWDMX_PlayOneFrm(UINT8 u1SrcId)
{
    return _SWDMX_PlayOneFrm(u1SrcId);
}
//-----------------------------------------------------------------------------
/** SWDMX_Pause
 *  Pause software demux
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_Pause(UINT8 u1SrcId)
{
    return _SWDMX_Pause(u1SrcId);
}

//-----------------------------------------------------------------------------
/** SWDMX_WaitForPause
 *  Pause software demux, return still swedmx stopped
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_WaitForPause(UINT8 u1SrcId, UINT32 u4WaitTime)
{
    return _SWDMX_WaitForPause(u1SrcId, u4WaitTime);
}

//-----------------------------------------------------------------------------
/** SWDMX_Stop
 *  Stop software program stream demux
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_Stop(UINT8 u1SrcId)
{
    return _SWDMX_Stop(u1SrcId);
}


//-----------------------------------------------------------------------------
/** SWDMX_Flush
 *  Flush A/V data
 *
 *  @retval VOID
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_Flush(UINT8 u1SrcId)
{
    return _SWDMX_Flush(u1SrcId);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetSpeed
 *  Set the playback speed 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetSpeed(UINT8 u1SrcId, INT32 i4Speed)
{
    return _SWDMX_SetSpeed(u1SrcId, i4Speed);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetPlayMode
 *  Set the playback mode 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------

BOOL SWDMX_SetPlayMode(UINT8 u1SrcId, SWDMX_PLAY_MODE_T ePlayMode)
{
    return _SWDMX_SetPlayMode(u1SrcId,ePlayMode);
}

//-----------------------------------------------------------------------------
/** SWDMX_SeekTime
 *  Set the file position 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SeekTime(UINT8 u1SrcId, UINT32 u4SeekTime, UINT64* pu8FilePos)
{
    *(pu8FilePos) = (UINT64)(-1);
    return _SWDMX_SeekTime(u1SrcId, u4SeekTime, pu8FilePos);
}

//-----------------------------------------------------------------------------
/** SWDMX_SeekPos
 *  Set the file position 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SeekPos(UINT8 u1SrcId, UINT64 u8SeekPos, UINT64 u8SeekPosI)
{
    return _SWDMX_SeekPos(u1SrcId, u8SeekPos, u8SeekPosI);
}

//-----------------------------------------------------------------------------
/** SWDMX_SeekOffset
 *  Set the file offset 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SeekOffset(UINT8 u1SrcId, VOID *prPosition)
{
    return _SWDMX_SeekOffset(u1SrcId, prPosition);
}

//-----------------------------------------------------------------------------
/** SWDMX_SeekPercent
 *  Set the file position 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SeekPercent(UINT8 u1SrcId, UINT32 u4SeekPercent, UINT64* pu8FilePos)
{
    return _SWDMX_SeekPercent(u1SrcId, u4SeekPercent, pu8FilePos);
}


//-----------------------------------------------------------------------------
/** SWDMX_RenderFromPos
 *  Set the file position 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_RenderFromPos(UINT8 u1SrcId, VOID *prPosition)
{
    return _SWDMX_RenderFromPos(u1SrcId, prPosition);
}

//-----------------------------------------------------------------------------
/** SWDMX_Add_Range
 *  Set the demux range 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_AddRange(UINT8 u1SrcId, VOID* pvRangeInfo, UINT32 u4TagID)
{
    MM_RANGE_INFO_T* prRangeInfo = (MM_RANGE_INFO_T*)pvRangeInfo;
    
    return _SWDMX_AddRange(u1SrcId, prRangeInfo, u4TagID);
}


//-----------------------------------------------------------------------------
/** SWDMX_Del_Range
 *  Delete the demux range 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_DelRange(UINT8 u1SrcId, VOID* pvRangeInfo, UINT32 u4TagID, BOOL fgDelAllRange)
{
    MM_RANGE_INFO_T* prRangeInfo = (MM_RANGE_INFO_T*)pvRangeInfo;
    
    return _SWDMX_DelRange(u1SrcId, prRangeInfo, u4TagID, fgDelAllRange);
}


//-----------------------------------------------------------------------------
/** SWDMX_SetContainerType
 *  Set the Container Type
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetContainerType(UINT8 u1SrcId, ENUM_SWDMX_FMT_T eContainerType)
{
	return _SWDMX_SetContainerType(u1SrcId, eContainerType);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetStrmID
 *  Set the demux range 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetStrmID(UINT8 u1SrcId, UINT32 u4StrmID, UINT32 u4StrmSubID, ENUM_SWDMX_STRM_TYPE_T eStrmType)
{
    switch(eStrmType)
    {
        case eSWDMX_STRM_TYPE_VID:
        	return _SWDMX_SetVideoStreamID(u1SrcId, u4StrmID);
        case eSWDMX_STRM_TYPE_VID2:
        	return _SWDMX_SetVideo2StreamID(u1SrcId, u4StrmID);
        case eSWDMX_STRM_TYPE_AUD:
        	return _SWDMX_SetAudioStreamID(u1SrcId, u4StrmID, u4StrmSubID);
        case eSWDMX_STRM_TYPE_AUD2:
        	return _SWDMX_SetAudio2StreamID(u1SrcId, u4StrmID);
        case eSWDMX_STRM_TYPE_AUD3:  // 0831
        	return _SWDMX_SetAudio3StreamID(u1SrcId, u4StrmID);
        case eSWDMX_STRM_TYPE_SUBTITLE:
        	return _SWDMX_SetSubTitleStreamID(u1SrcId, u4StrmID);
        case eSWDMX_STRM_TYPE_PCR:
        	return _SWDMX_SetPcrStreamID(u1SrcId, u4StrmID);
        default:
        	return FALSE;
    }
}

//-----------------------------------------------------------------------------
/** SWDMX_PidChgDone
 *  update vid/aud pid 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------


BOOL SWDMX_CodecChgDone(UINT8 u1SrcId,UINT8 u1Type,VOID* pvCodecChg)
{
    return _SWDMX_CodecChgDone(u1SrcId, u1Type,pvCodecChg);
}

//-----------------------------------------------------------------------------
/** SWDMX_PidChgDone
 *  update vid/aud pid 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------

BOOL SWDMX_PidChgDone(UINT8 u1SrcId,UINT8 u1Type,VOID* pvPidChg)
{
    
    return _SWDMX_PidChgDone(u1SrcId, u1Type,pvPidChg);

}
//-----------------------------------------------------------------------------
/** SWDMX_RegPidChgCallback
 *  register vid/aud callback 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------

BOOL SWDMX_RegPidChgCallback(UINT8 u1SrcId,UINT8 u1Type,VOID* pvRegPidChgCb)
{
    
    return _SWDMX_RegPidChgCallback(u1SrcId, u1Type,pvRegPidChgCb);

}


//-----------------------------------------------------------------------------
/** SWDMX_QueryStatus
 *  Query the swdmx status 
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
ENUM_SWDMX_PLAY_STATE_T SWDMX_QueryStatus(UINT8 u1SrcId)
{
    return _SWDMX_QueryStatus(u1SrcId);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetRangeCb
 *  Set range callback function
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetRangeCb(UINT8 u1SrcId, VOID* pfnCbFunc)
{
    return _SWDMX_SetRangeCb(u1SrcId, pfnCbFunc);
}

//-----------------------------------------------------------------------------
/** SWDMX_SetRangeCb
 *  Set range callback function
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_SetRangeCbEx(UINT8 u1SrcId, VOID* pfnCbFunc)
{
    return _SWDMX_SetRangeCbEx(u1SrcId, pfnCbFunc);
}

//-----------------------------------------------------------------------------
/** SWDMX_QueryRangeInfo
 *  Query swdmx range information, including id and start PTS
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------

VOID SWDMX_QueryRangeInfo(UINT8 u1SrcId, SWDMX_RANGE_INFO_T* prRangeInfo)
{
    _SWDMX_QueryRangeInfo(u1SrcId, prRangeInfo);
    return;
}

//-----------------------------------------------------------------------------
/** SWDMX_SetDmxNfy
 *  Set notify functio for ctrl
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
INT32 SWDMX_SetDmxNfy(UINT8 u1SrcId, const SWDMX_NFY_INFO_T* prSwdmxNfyInfo)
{
    return _SWDMX_SetDmxNfy(u1SrcId, prSwdmxNfyInfo);
}


//-----------------------------------------------------------------------------
/** SWDMX_SetDmxNfy
 *  Set notify functio for ctrl
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_ReadBuffer(UINT8 u1SrcId, ENUM_SWDMX_STRM_TYPE_T eStrmType, SWDMX_READ_INFO_T *prReadInfo)
{
    return _SWDMX_ReadBuffer(u1SrcId, eStrmType, prReadInfo);
}


//-----------------------------------------------------------------------------
/** SWDMX_ReleaseBuffer
 *  Release (unlock) buffer
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_ReleaseBuffer(UINT8 u1SrcId, ENUM_SWDMX_STRM_TYPE_T eStrmType, SWDMX_READ_INFO_T *prReadInfo)
{
    return _SWDMX_ReleaseBuffer(u1SrcId, eStrmType, prReadInfo);
}


//-----------------------------------------------------------------------------
/** SWDMX_AudioNotify
 *  Set notify functio for audio down
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_AudioNotify(UINT8 u1SrcId, ENUM_SWDMX_AUDIO_NOTIFY_TYPE_T eAudioNotifyType)
{
    return _SWDMX_AudioNotify(u1SrcId, eAudioNotifyType);
}


//-----------------------------------------------------------------------------
/** SWDMX_PcrCallBack
 *  Set notify function pcr data
 *
 *  @retval void
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_PcrCallBack(UINT32 u4PcrBase)
{
    return _SWDMX_PcrCallBack(u4PcrBase);
}


#ifdef CC_SWDMX_RENDER_PTS
//-----------------------------------------------------------------------------
/** SWDMX_SetRenderPts
 *  Set render pts
 *
 *  @retval BOOL
 */
//-----------------------------------------------------------------------------
VOID SWDMX_SetRenderPts(UINT8 u1SrcId, UINT32 u4RenderPts)
{
    _SWDMX_SetRenderPts(u1SrcId, u4RenderPts);
}
#endif


#ifdef CC_SWDMX_RENDER_POSITION_TEST
EXTERN VOID SWDMX_GetRenderPosition(VOID *prRenderPos)
{
    _SWDMX_GetRenderPosition(prRenderPos);
}
EXTERN VOID SWDMX_SetRenderPosition(VOID *prRenderPos)
{
    _SWDMX_SetRenderPosition(prRenderPos);
}
#endif


BOOL SWDMX_SetFlushFeederInfo(UINT8 u1SrcId, UINT8 u1Type,VOID* pvMMFlushInfo,UINT32 u4Para2,UINT32 u4Para3)
{       
    return _SWDMX_SetFlushFeederInfo(u1SrcId,u1Type,pvMMFlushInfo,u4Para2,u4Para3);
}

BOOL SWDMX_SetGnrcStreamInfo(UINT8 u1SrcId, UINT32 u4Para1)
{       
    return _SWDMX_SetGnrcStreamInfo(u1SrcId,u4Para1);
}

BOOL SWDMX_EnableHdcpKey(UINT8 u1SrcId,UINT8 u1Type)
{
    return _SWDMX_EnableHdcpKey(u1SrcId,u1Type);
}

//-----------------------------------------------------------------------------
/** SWDMX_LockValidSrc
 *  Get the available source id
 *
 *  @retval UINT8
 */
//-----------------------------------------------------------------------------
BOOL SWDMX_LockValidSrc(UINT8 u1SrcId)
{
    return _SWDMX_LockValidSrc(u1SrcId);
}


//-----------------------------------------------------------------------------
/** SWDMX_UnlockSrc
 *  Release the locked source id
 *
 *  @retval UINT8
 */
//-----------------------------------------------------------------------------
void SWDMX_UnlockSrc(UINT8 u1SrcId)
{
    _SWDMX_UnlockSrc(u1SrcId);
}
