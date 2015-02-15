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
 * $RCSfile: mtdmx_desc.h,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file
 *  This header file declares exported APIs of TS record module.
 */

#ifndef MT_TSREC_H
#define MT_TSREC_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mttype.h"
#include "mtdmx.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTTSREC_MAX_PID                     (128)/***L5L, 32 -> 128***/


//#define MTTSREC_SUPPORT_TWOPVR

#define MTTSREC_MAX_IDX						(2)

#ifdef CC_MT5392B
#define MTTSREC_MAX_KEY_INDEX               (8)
#else
#define MTTSREC_MAX_KEY_INDEX               (4)
#endif

//-----------------------------------------------------------------------------
// Function bitmap of PID operations
//-----------------------------------------------------------------------------
#define MTTSREC_GBL_FLAG_NOTIFY             0x00000002
#define MTTSREC_GBL_FLAG_TIME_INFO          0x00000004
#define MTTSREC_GBL_FLAG_NONE               0x00000000
#define MTTSREC_GBL_FLAG_ALL                0xFFFFFFFF

#define MTTSREC_PID_FLAG_ENABLE             0x00000001
#define MTTSREC_PID_FLAG_PID                0x00000002
#define MTTSREC_PID_FLAG_SCRAMBLE_ENABLE    0x00000004
#define MTTSREC_PID_FLAG_DESCRAMBLE         0x00000008
#define MTTSREC_PID_FLAG_NONE               0x00000000
#define MTTSREC_PID_FLAG_ALL                0xFFFFFFFF

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef enum
{
    MTTSREC_GBL_NFY_TIME,
    MTTSREC_GBL_NFY_OVERFLOW
} MTTSREC_GBL_NOTIFY_CODE_T;

typedef struct
{
	UINT8  u1Idx;
    UINT32 u4Addr;
    UINT32 u4Size;
    UINT32 u4PktCount;
} MTTSREC_GBL_TIMENOTIFY_INFO_T;

typedef void (*PFN_MTTSREC_GBL_NOTIFY)(MTTSREC_GBL_NOTIFY_CODE_T eCode,
                                       UINT32 u4Data, const void* pvTag);

typedef struct
{
    PFN_MTTSREC_GBL_NOTIFY pfnNotifyFunc;
    void* pvNotifyTag;

    UINT32 u4TimeToNotify;
} MTTSREC_GBL_T;

typedef struct
{
    BOOL fgEnable;
    UINT16 u2Pid;
    BOOL fgScramble;
    UINT8 u1KeyIdx;
    MTDMX_DESC_MODE_T eDescMode;
    UINT8 u1DescKeyIdx;
} MTTSREC_PID_T;

typedef struct
{
    UINT32 u4StreamAddr;
    UINT32 u4StreamSize;
    UINT32 u4MetaAddr;
    UINT32 u4MetaSize;
} MTTSREC_BUFFERPTR_T;

typedef struct
{
    UINT32 u4Addr;
    UINT32 u4Size;
} MTTSREC_MEM_INFO_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** MTTSREC_Init
 *  Initialize
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_Init(void);

//-----------------------------------------------------------------------------
/** MTTSREC_SetGBLEx
 * Set record buffer, notify and callback time
 *
 *  @param  u1Idx           Recording TS index
 *  @param  u4Flags         Function flags
 *  @param  prGBL           Global recording structure
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetGBLEx(UINT8 u1Idx, UINT32 u4Flags, MTTSREC_GBL_T *prGBL);

//-----------------------------------------------------------------------------
/** MTTSREC_SetGBL
 * Set record buffer, notify and callback time
 *
 *  @param  u4Flags         Function flags
 *  @param  prGBL           Global recording structure
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetGBL(UINT32 u4Flags, MTTSREC_GBL_T *prGBL);


//-----------------------------------------------------------------------------
/** MTTSREC_GetGBLEx
 *  Get record buffer, notify and callback time
 *
 *  @param  u1Idx           Recording TS index
 *  @param  u4Flags         Function flags
 *  @param  prGBL           Global recording structure
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetGBLEx(UINT8 u1Idx, UINT32 u4Flags, MTTSREC_GBL_T *prGBL);

//-----------------------------------------------------------------------------
/** MTTSREC_GetGBL
 *  Get record buffer, notify and callback time
 *
 *  @param  u4Flags         Function flags
 *  @param  prGBL           Global recording structure
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetGBL(UINT32 u4Flags, MTTSREC_GBL_T *prGBL);

//-----------------------------------------------------------------------------
/** MTTSREC_FreeGBLEx
 *  Free record resource
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_FreeGBLEx(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTTSREC_FreeGBL
 *  Free record resource
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_FreeGBL(void);


//-----------------------------------------------------------------------------
/** MTTSREC_SetPidEx
 *  Set record PID information
 *
 *  @param  u1Idx           Recording TS index
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure for recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetPidEx(UINT8 u1Idx, UINT8 u1Pidx, UINT32 u4Flags, MTTSREC_PID_T *prPid);

//-----------------------------------------------------------------------------
/** MTTSREC_SetPid
 *  Set record PID information
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure for recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetPid(UINT8 u1Pidx, UINT32 u4Flags, MTTSREC_PID_T *prPid);


//-----------------------------------------------------------------------------
/** MTTSREC_GetPid
 *  Get record PID information
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  u4Flags         Function flags
 *  @param  prPid           The PID structure for recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetPid(UINT8 u1Pidx, UINT32 u4Flags, MTTSREC_PID_T *prPid);

//-----------------------------------------------------------------------------
/** MTTSREC_FreePid
 *  Free record PID resource
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_FreePid(UINT8 u1Pidx);

//-----------------------------------------------------------------------------
/** MTTSREC_UnlockBufferEx
 *  Unlock record buffer to driver
 *
 *  @param  u1Idx           Recording TS index
 *  @param  u4Addr          Buffer address
 *  @param  u4Size          Buffer Size
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_UnlockBufferEx(UINT8 u1Idx, UINT32 u4Addr, UINT32 u4Size);

//-----------------------------------------------------------------------------
/** MTTSREC_UnlockBuffer
 *  Unlock record buffer to driver
 *
 *  @param  u4Addr          Buffer address
 *  @param  u4Size          Buffer Size
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_UnlockBuffer(UINT32 u4Addr, UINT32 u4Size);


//-----------------------------------------------------------------------------
/** MTTSREC_FlushBufferEx
 *  Update read/write pointer to buffer start
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_FlushBufferEx(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTTSREC_FlushBuffer
 *  Update read/write pointer to buffer start
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_FlushBuffer(void);

//-----------------------------------------------------------------------------
/** MTTSREC_StartEx
 *  Start recording
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_StartEx(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTTSREC_Start
 *  Start recording
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_Start(void);

//-----------------------------------------------------------------------------
/** MTTSREC_StopEx
 *  Stop record
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_StopEx(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTTSREC_Stop
 *  Stop record
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_Stop(void);


//-----------------------------------------------------------------------------
/** MTTSREC_IsRecordingEx
 *  Check record status
 *
 *  @param  u1Idx           Recording TS index
 *  @param  pfgIsRecording  If it is recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_IsRecordingEx(UINT8 u1Idx, BOOL *pfgIsRecording);

//-----------------------------------------------------------------------------
/** MTTSREC_IsRecording
 *  Check record status
 *
 *  @param  pfgIsRecording  If it is recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_IsRecording(BOOL *pfgIsRecording);


//-----------------------------------------------------------------------------
/** MTTSREC_SetBufferEx
 * Set record stream and meta buffer size
 * Allocate memory in kernel by default, not decided by AP
 *
 *  @param  pfgIsRecording  If it is recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetBufferEx(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTTSREC_SetBuffer
 * Set record stream and meta buffer size
 * Allocate memory in kernel by default, not decided by AP
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetBuffer(void);

//-----------------------------------------------------------------------------
/** MTTSREC_GetStreamInfoEx
 *  Get stream information
 *
 *  @param  u1Idx           Recording TS index
 *  @param  prKernelBufInfo Kernel buffer information
 *  @param  prUserBufInfo   User buffer information
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetStreamInfoEx(UINT8 u1Idx, MTTSREC_MEM_INFO_T *prKernelBufInfo,
                                         MTTSREC_MEM_INFO_T *prUserBufInfo);

//-----------------------------------------------------------------------------
/** MTTSREC_GetStreamInfo
 *  Get stream information
 *
 *  @param  prKernelBufInfo Kernel buffer information
 *  @param  prUserBufInfo   User buffer information
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetStreamInfo(MTTSREC_MEM_INFO_T *prKernelBufInfo,
                                         MTTSREC_MEM_INFO_T *prUserBufInfo);


//-----------------------------------------------------------------------------
/** MTTSREC_GetMetaInfoEx
 *
 *  @param  u1Idx           Recording TS index
 *  @param  prKernelBufInfo Kernel meta information
 *  @param  prUserBufInfo   User meta information
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetMetaInfoEx(UINT8 u1Idx, MTTSREC_MEM_INFO_T *prKernelMetaInfo,
                                       MTTSREC_MEM_INFO_T *prUserMetaInfo);

//-----------------------------------------------------------------------------
/** MTTSREC_GetMetaInfo
 *
 *  @param  prKernelBufInfo Kernel meta information
 *  @param  prUserBufInfo   User meta information
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_GetMetaInfo(MTTSREC_MEM_INFO_T *prKernelMetaInfo,
                                       MTTSREC_MEM_INFO_T *prUserMetaInfo);

//-----------------------------------------------------------------------------
/** MTTSREC_SetKey
 * Set timeshift key
 *
 *  @param  u1Idx           Key index
 *  @param  au4Key          Key value
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTTSREC_SetKey(UINT8 u1Idx, UINT32 au4Key[8]);

#endif //MT_TSREC_H

