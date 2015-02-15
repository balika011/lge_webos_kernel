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
 * $RCSfile: vdp_frc.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdp_frc.h
 *  Brief of file vdp_frc.h.
 *  Details of file vdp_frc.h (optional).
 */

#ifndef VDP_FRC_H
#define VDP_FRC_H


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_typedef.h"
#include "b2r_if.h"

#ifndef CC_DISABLE_EARLY_DISPLAY_MUTE
#define VDP_MUTE_EARLY_DISPLAY
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef enum
{
    VDP_SMLS_MODE_NONE,   
    VDP_SMLS_PREPARE,     // Set vVRMSetEventFlg(path, VRM_EVENT_BY_B2R) for NPTV to prepare new w/h cfg.
    VDP_SMLS_READY,       // Check NPTV ready or not by bVRMReadyForB2R(ucVdpId).
    VDP_SMLS_FRAME_COMING,  // Resolution change frames coming, current must be ready status.
    VDP_SMLS_PREPARE_WHILE_START_PLAY,  //Start play with NPTV seamless, prepare
    VDP_SMLS_READY_WHILE_START_PLAY,    //Start play with NPTV seamless, ready
}SEAMLESS_MODE_E;



//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

EXTERN void _VDP_FrcInit(void);
EXTERN UINT32 _VDP_FrcHandlerEvent(UCHAR ucB2rId, UINT32 u4Event);
EXTERN UINT32 _VDP_FrcProc(UCHAR ucB2rId, UCHAR ucBottom, UCHAR ucRightView);

EXTERN void _VDP_FrcProcEarly(UCHAR ucVdpId, UCHAR ucBottom);
EXTERN BOOL VDP_FrcGetUnMuteStatus(UCHAR ucB2rId);
EXTERN UCHAR VDP_FrcGetLockFrameBuffer(UCHAR ucB2rId);
EXTERN UINT32 _VDP_GetCurrentPts(UCHAR ucB2rId);
EXTERN VOID _VDP_SetCurrentTick(UCHAR ucVdpId, UINT32 u4Tick);
EXTERN UINT32 _VDP_GetLastPts(UCHAR ucB2rId);
EXTERN UINT32 _VDP_GetDispingPts(UCHAR ucB2rId);
EXTERN UCHAR _VDP_GetFbg(UCHAR ucB2rId);

EXTERN UCHAR _VDP_GetOutFrameRate(UCHAR ucVdpId);
EXTERN UINT32 _VDP_GetFilePosition(UCHAR ucVdpId,
                                   INT32 *pi4TempRef, UINT16 *pu2DecodingOrder,
                                   UINT64 *pu8Offset, UINT64 *pu8OffsetI);
EXTERN UINT32 _VDP_GetPosInfo(UCHAR ucVdpId, VDP_POS_INTO_T *prPosInfo);
EXTERN UINT32 _VDP_SetABInfo(UCHAR ucVdpId, VDP_AB_INTO_T *prAB);
EXTERN UCHAR _VDP_GetPendingFB(UCHAR ucVdpId, BOOL fgSubFbg);
EXTERN void _VDP_CleanPendingFB(UCHAR ucVdpId, BOOL fgSubFbg);

EXTERN void _VDP_SetReleaseDispQ(UCHAR ucVdpId, BOOL fgSubFbg);

EXTERN void _VDP_CopyFrcPrm(UCHAR ucVdpId);
EXTERN void _VDP_FrcSetLipSyncCb(UCHAR ucVdpId, UCHAR ucPort, UINT32 u4Thrsd, UINT32 u4FrmCnt);

EXTERN void _VDP_FrcSetLipSyncNGOKCb(UCHAR ucNgOk, UCHAR ucRemove, UINT32 u4Thrsd, UINT32 u4FrmCnt);

EXTERN UINT32 _VDP_TrickModeSkipFrameNo(UCHAR u1B2rId);

/* VDP trigger mode + audio master to do trigger mode */
EXTERN void _VDPTriggerAudReceive(UINT8 u1B2rId, UINT32 u4Pts);

/** Disable&Enable VDP Trigger action */
EXTERN BOOL _VDPTriggerAudReceiveEx(UINT8 u1B2rId, BOOL fgEnable, UINT32 u4Pts, VDP_TRIG_AUD_ARRIVE_CB_T *prCbInfo);

/* whether Update STC when VDP start output or not */
EXTERN BOOL _VDPSetPendingForStc(UINT8 u1B2rId, BOOL fgPending);

/* VDP Seek done notification */
EXTERN void _VDPSetSeek(UCHAR ucVdpId, UCHAR ucSet, UINT32 u4Pts);

/* VDP AB Repeat done notification */
EXTERN void _VDPSetABRepeat(UCHAR ucB2rId, UCHAR ucSet);

/*VDP Set Current MM Speed*/
EXTERN VOID _VDP_FrcStcSpeed(UINT8 u1B2rId,STC_SPEED_TYPE_T eSpeed);
EXTERN STC_SPEED_TYPE_T _VDP_GetFrcStcSpeed(UINT8 u1B2rId);

#ifdef VDP_B2R_PSCAN_CRC_FLOW
/* B2R CRC check on PTS value */
EXTERN void _VDPSetChkPTS(UINT32 u4Pts);
EXTERN void _VDP_SetEncTrigger(BOOL fgEnable);

#endif

EXTERN B2R_PRM_T* _VDP_GetVdpPrm(UINT8 u1B2rId);
EXTERN VOID _VDP_SetPauseMM(UINT8 u1B2rId,BOOL fgPauseMM);

EXTERN VOID _VDP_SetB2rNoLock(UINT8 u1B2rId, BOOL fgNoLock);

EXTERN VOID _VDP_Set3DInfo(UINT8 u1B2rId,UINT8 u13DType,BOOL fg3DEnable);
EXTERN BOOL _VDP_IsStartToPlay(UCHAR u1B2rId);
EXTERN UINT32 _VDP_GetDispBufLocked(UCHAR ucVdpId, UINT32 u4BufAddr, BOOL *pfgLocked);


EXTERN INT32 _VDP_GetPtsStcDelta(UCHAR ucB2rId);
EXTERN VOID _VDP_SetRepeatFrame(UCHAR ucVdpId);
EXTERN VOID _VDP_SetDropFrame(UCHAR ucVdpId);
EXTERN VOID _VDP_SetTimeShiftMode(UINT8 u1B2rId,BOOL fgEnable);
EXTERN VOID _VDP_SetTimeShiftSpeed(UINT8 u1B2rId,UINT32 u4SpeedValue);
EXTERN UCHAR _VDP_GetFbg(UINT8 u1B2rId);

#ifdef CC_SUPPORT_NPTV_SEAMLESS 
EXTERN VOID _VDP_NPTVSeamlessStatus(B2R_PRM_T* prFrcPrm, UCHAR ucMode);
#endif
EXTERN void _VDP_FinishSeekDoneBeforeEnding(UCHAR ucVdpId);

#ifdef CC_MAPLE_CUST_DRV
INT32 _B2R_Get_Cust_info(UCHAR              ucB2rId,
                              B2R_CUST_INFO_T*    pt_cust_info);
INT32 _B2R_GetAspInfo(UCHAR     ucB2rId,
                           UINT8*    pu1_asp_info);
#endif

VOID _B2R_RTCliActive(UCHAR ucB2rId,  UCHAR ucEnable);
VOID _B2R_SetLatency(UCHAR ucB2rId,  UINT32 u4Latency);
EXTERN UINT32 _VDP_HalSetRenderRegion(UCHAR ucB2rId, VDP_REGION_T* prRenderRegion);

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
#ifdef DIVX_PLUS_CER
#ifdef   CC_B2R_CHAPTER_CHG
EXTERN VOID _VDP_FrcSetChapterEnable(VDP_CHAPTER_T*    prChapter);
#endif
#endif

INT32 _B2R_Height_Align(UINT32*  pu4_height);

#endif    // VDP_FRC_H

