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

#ifndef MT_DREC_H
#define MT_DREC_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mttype.h"
#include "mttsrec.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTDREC_MAX_PID                     (64)

#define MTDREC_MAX_IDX			            (4)

#define MTDREC_PID_FLAG_ENABLE             0x00000001
#define MTDREC_PID_FLAG_TSINDEX            0x00000002
#define MTDREC_PID_FLAG_PID                0x00000004
#define MTDREC_PID_FLAG_ALL                0xFFFFFFFF

//-----------------------------------------------------------------------------
// Function bitmap of PID operations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    BOOL fgUsed;
    UINT8 u1TsIdx;
    UINT32 u4FbmAddr;
    UINT32 u4FbmSize;
    UINT32 u4ChBaseAddr;
    UINT32 u4ChBaseSize;
    UINT32 u4ChStreamAddr;
    UINT32 u4ChStreamSize;
} MTDREC_BUFPTR_T;

typedef enum
{
    MTDREC_NOTIFY_CODE_STREAM_DATA,
    MTDREC_NOTIFY_CODE_SCRAMBLE_STATE,
    MTDREC_NOTIFY_CODE_RAW_SCRAMBLE_STATE
} MTDREC_NOTIFY_CODE_T;

typedef enum
{
    DREC_TYPE_VIDEO_MPEG2,
    DREC_TYPE_VIDEO_MPEG4,
    DREC_TYPE_VIDEO_H264,
    DREC_TYPE_VIDEO_AVS,
    DREC_TYPE_AUDIO,
    DREC_TYPE_PCR,
    DREC_TYPE_OTHERS
} MTDREC_PID_TYPE_T;

typedef enum _DDI_PVR_INDEX_TYPE_T
{
	MTDREC_PVR_INDEX_TYPE_SEQ = 0x01,
	MTDREC_PVR_INDEX_TYPE_I_PIC = 0x02,
	MTDREC_PVR_INDEX_TYPE_SEQ_I_PIC = 0x03,
 	MTDREC_PVR_INDEX_TYPE_P_PIC = 0x04,
	MTDREC_PVR_INDEX_TYPE_B_PIC = 0x08,
	MTDREC_PVR_INDEX_TYPE_DIT = 0x80,
	MTDREC_PVR_INDEX_TYPE_NONE = 0xFF
} MTDREC_PVR_INDEX_TYPE_T;

typedef struct _DDI_PVR_INDEX_T
{
	MTDREC_PVR_INDEX_TYPE_T	eIndexType;
	UINT32				    u4Offset;
    UINT64                  u8Pts33;
    MTDREC_PID_TYPE_T       eDrecPidType;
    UINT16                  u2VideoPid;
} MTDREC_PVR_INDEX_T;

typedef enum
{
	MTDREC_PVR_FR_FORBIDDEN = 0,
	MTDREC_PVR_FR_23_976HZ = 1,
	MTDREC_PVR_FR_24HZ,
	MTDREC_PVR_FR_25HZ,
	MTDREC_PVR_FR_29_97HZ,
	MTDREC_PVR_FR_30HZ,
	MTDREC_PVR_FR_50HZ,
	MTDREC_PVR_FR_59_94HZ,
	MTDREC_PVR_FR_60HZ,
	MTDREC_PVR_FR_UNKNOWN
} MTDREC_PVR_FRAMERATE_T;

typedef enum
{
	MTDREC_CIPHER_NONE = 0,
	MTDREC_CIPHER_AES_ECB,
	MTDREC_CIPHER_AES_CBC
} MTDREC_CIPHER_ALGORM_T;

typedef enum
{
    MTDREC_CIPHER_KEY_EVEN = 0,
    MTDREC_CIPHER_KEY_ODD,
    MTDREC_CIPHER_SECURE_KEY0_EVEN,
    MTDREC_CIPHER_SECURE_KEY0_ODD
} MTDREC_CIPHER_KEY_T;

typedef enum
{
	MTDREC_TIMESTAMP_TYPE_NONE = 0,
	MTDREC_TIMESTAMP_TYPE_4BYTE
} MTDREC_TIMESTAMP_T;

typedef struct
{
	MTDREC_CIPHER_ALGORM_T  eAlgorithm;
	MTDREC_CIPHER_KEY_T		eKeyType;
	UINT32					u4KeyLen;
} MTDREC_CRYPTO_INFO_T;

typedef struct
{
    MTDREC_TIMESTAMP_T eTimestamp;
    MTDREC_CIPHER_ALGORM_T  eAlgorithm;
} MTDREC_CONFIG_T;

typedef enum
{
    MTDREC_SCRAMBLE_STATE_CLEAR,           ///< Clear data
    MTDREC_SCRAMBLE_STATE_SCRAMBLED,       ///< Scrambled data
    MTDREC_SCRAMBLE_STATE_UNKNOWN          ///< Unknown data
} MTDREC_SCRAMBLE_STATE_T;

typedef struct
{
    BOOL fgValid;
    UINT32 u4Addr;
    UINT32 u4Size;
    UINT32 u4IndexAddr;
    UINT32 u4IndexSize;
} MTDREC_STREAM_DATA_T;

typedef struct
{
    UINT32 u4BitRate;
    UINT32 u4FrameRate;
    MTDREC_PVR_FRAMERATE_T eFrameRate;
    BOOL fgFoundSeqSPS;
    UINT32 u4SeqSPSOffset;
    UINT32 u4SPSNextStartLen;       // For H264 frame rate search range
    MTDREC_PID_TYPE_T eDrecPidType;
} MTDREC_STREAM_INFO_T;

typedef BOOL (*PFN_MTDREC_PID_DETECTSCRAMBLE)(UINT8 u1Pidx, MTDREC_SCRAMBLE_STATE_T eState);

/// PID data structure
typedef struct
{
    BOOL fgEnable;                  ///< Enable or disable
    UINT16 u2Pid;                   ///< PID
    MTDMX_PID_TYPE_T ePidType;      ///< PID type
    UINT8 u1TsIndex;                ///< TS index
    UCHAR ucEsId;                   ///< video channel id
    BOOL fgDetectScramble;
    PFN_MTDREC_PID_DETECTSCRAMBLE pfnScramble;      ///< Callback function of scramble state
    PFN_MTDREC_PID_DETECTSCRAMBLE pfnRawScramble;   ///< Callback function of raw scramble state
} MTDREC_PID_T;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** MTDREC_Init
 *  Initialize
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_Init(void);

//-----------------------------------------------------------------------------
/** MTDREC_AllocBuffer
 *  Allocate record buffer to driver
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_AllocBuffer(UINT8 u1TsIdx);

//-----------------------------------------------------------------------------
/** MTDREC_FreeBuffer
 *  Free record buffer to driver
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_FreeBuffer(UINT8 u1TsIdx);

//-----------------------------------------------------------------------------
/** MTDREC_SetPid
 *  Set record PID information
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  eRecPid         The PID structure for recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SetPid(UINT8 u1Pidx, const MTDREC_PID_T *eRecPid);

//-----------------------------------------------------------------------------
/** MTDREC_GetPid
 *  Get record PID information
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  u4Flags         Function flags
 *  @param  eRecPid         The PID structure for recording
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetPid(UINT8 u1Pidx, UINT32 u4Flags, MTDREC_PID_T *prPid);

//-----------------------------------------------------------------------------
/** MTDREC_FreePid
 *  Free record PID
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_FreePid(UINT8 u1Pidx);

//-----------------------------------------------------------------------------
/** MTDREC_UnlockBuffer
 *  Unlock record buffer to driver
 *
 *  @param  u1Idx           Recording TS index
 *  @param  u4Addr          Buffer address
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_UnlockBuffer(UINT8 u1Idx,UINT32 u4Addr);

//-----------------------------------------------------------------------------
/** MTDREC_Start
 *  Start recording
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_Start(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTDREC_Stop
 *  Stop record
 *
 *  @param  u1Idx           Recording TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_Stop(UINT8 u1Idx);

//-----------------------------------------------------------------------------
/** MTDREC_GetDownloadStreamInfo
 *  get recorded data information
 *
 *  @param  u1Idx           TS index
 *  @param  pInfo           Stream information of recorded data
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetDownloadStreamInfo(UINT8 u1Idx, MTDREC_STREAM_INFO_T *pInfo);

//-----------------------------------------------------------------------------
/** MTDREC_GetDownloadStream
 *  get download stream buffer and start code information
 *
 *  @param  u1Idx           TS index
 *  @param  pu4StreamBuf    Stream buffer pointer
 *  @param  pu4StreamSize   Stream buffer size
 *  @param  pu4IndexArry    Start code information pointer
 *  @param  pu4IndexSize    Start code information size
 *  @param  u4Timeout       Timeout value
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetDownloadStream(UINT8 u1Idx, UINT32 *pu4StreamBuf, UINT32 *pu4StreamSize,
                UINT32 *pu4IndexArry, UINT32 *pu4IndexSize, UINT32 u4Timeout);

//-----------------------------------------------------------------------------
/** MTDREC_StopDownloadStream
 *  Stop to recording
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_StopDownloadStream(void);

//-----------------------------------------------------------------------------
/** MTDREC_SetCipher
 *  Set cipher key
 *
 *  @param  pu4IndexArry    Start code information pointer
 *  @param  pu4IndexSize    Start code information size
 *  @param  u4Timeout       Timeout value
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SetCipher(MTDREC_CRYPTO_INFO_T *prInfo, UINT8 *pu1IV, UINT8 *pu1Key);

//-----------------------------------------------------------------------------
/** MTDREC_SetConfig
 *  Setup timetamp and cipher algorithm
 *
 *  @param  prConfig        Configuration for timestamp and cipher algorithm
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SetConfig(MTDREC_CONFIG_T *prConfig);

//-----------------------------------------------------------------------------
/** MTDREC_GetTotalDownloadBufferSize
 *  Get download buffer size
 *
 *  @param  u1TsIdx         TS index
 *  @param  pu4Size         Buffer size
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetTotalDownloadBufferSize(UINT8 u1TsIdx, UINT32 *pu4Size);

//-----------------------------------------------------------------------------
/** MTDREC_SetDescramblPid
 *
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  eDescMode       Descramble mode
 *  @param  u1KeyIndex      Key index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SetDescramblPid(UINT8 u1Pidx, MTDMX_DESC_MODE_T eDescMode, UINT8 u1KeyIndex);

//-----------------------------------------------------------------------------
/** MTDREC_SetPidDecoderType
 *
 *  @param  u1TsIdx         TS index
 *  @param  u1Pidx          PID index, 0 ~ (MTDMX_NUM_PID_INDEX - 1)
 *  @param  ePidType        PID video type
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SetPidDecoderType(UINT8 u1TsIdx, UINT8 u1Pidx, MTDREC_PID_TYPE_T eDrecPidType);

//-----------------------------------------------------------------------------
/** MTDREC_GetScrambleState
 *  Get scramble state
 *
 *  @param  u1Pidx              PID index, 0 ~ 63
 *  @param  peScramble_Sstate   The pointer of scramble state
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetScrambleState(UINT8 u1Pidx, MTDREC_SCRAMBLE_STATE_T *peScramble_State);

//-----------------------------------------------------------------------------
/** MTDREC_GetRawScrambleState
 *  Get raw scramble state
 *
 *  @param  u1Pidx              PID index, 0 ~ 63
 *  @param  peScramble_Sstate   The pointer of scramble state
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_GetRawScrambleState(UINT8 u1Pidx, MTDREC_SCRAMBLE_STATE_T *peScramble_State);

//-----------------------------------------------------------------------------
/** MTDREC_SitSetPattern
 *  This API makes the transport packets from the PSI table for the specific 
 *  ts index. The transport packet will be inserted when application calls
 *  MTDREC_SitSitertPattern or MTDREC_SitSitertPatternByTimer.
 *
 *  @param  u1TsIdx         TS index
 *  @param  u2Pid           PID number
 *  @param  pu1Pattern      PSI table pointer
 *  @param  u2Len           length of PSI table
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SitSetPattern(UINT8 u1TsIdx, UINT16 u2Pid, UINT8 *pu1Pattern, UINT16 u2Len);

//-----------------------------------------------------------------------------
/** MTDREC_SitFreePattern
 *  This API frees the transport packet which is made for packet insertion.
 *
 *  @param  u1TsIdx         TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SitFreePattern(UINT8 u1TsIdx);

//-----------------------------------------------------------------------------
/** MTDREC_SitInsertPattern
 *  This API inserts new transport packets one time for each API call.
 *
 *  @param  u1TsIdx         TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SitInsertPattern(UINT8 u1TsIdx);

//-----------------------------------------------------------------------------
/** MTDREC_SitInsertPatternByTimer
 *  This API creates a periodic timer to insert the transport packets.
 *
 *  @param  u1TsIdx         TS index
 *  @param  u4MsDelay       the period fo the timer in millisecond. 
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SitInsertPatternByTimer(UINT8 u1TsIdx, UINT32 u4MsDelay);

//-----------------------------------------------------------------------------
/** MTDREC_SitStopTimer
 *  This API deletes the timer.
 *
 *  @param  u1TsIdx         TS index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SitStopTimer(UINT8 u1TsIdx);

//-----------------------------------------------------------------------------
/** MTDREC_SwapSetPattern
 *  This API makes the transport packets from the PSI table for the specific 
 *  PID index. The arrival transport packets having the specific PID value 
 *  will be replaced with new transport packets.
 *
 *  @param  u1Pidx          PID index
 *  @param  pu1Pattern      PSI table pointer
 *  @param  u2Len           the length of PSI table
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SwapSetPattern(UINT8 u1Pidx, UINT8 *pu1Pattern, UINT16 u2Len);

//-----------------------------------------------------------------------------
/** MTDREC_SwapFreePattern
 *  This API frees the transport packet which is made for packet replacement.
 *
 *  @param  u1Pidx          PID index
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SwapFreePattern(UINT8 u1Pidx);

//-----------------------------------------------------------------------------
/** MTDREC_SwapReplacePattern
 *  This API replaces the arrival transport packets having the specific 
 *  PID value with new transport packets 
 *
 *  @param  u1Pidx          PID index
 *  @param  fgEnabled       TRUE if enable, FALSE if disable
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_SwapReplacePattern(UINT8 u1Pidx, BOOL fgEnabled);

//-----------------------------------------------------------------------------
/** MTDREC_StartInsertDIT
 *  This API start inserting DIT
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_StartInsertDIT(UINT8 u1PcrPidx);

//-----------------------------------------------------------------------------
/** MTDREC_StopInsertDIT
 *  This API stop inserting DIT
 *
 *  @retval MTR_OK          Success.
 *  @retval MTR_NOT_OK      Fail.
 */
//-----------------------------------------------------------------------------
EXTERN MT_RESULT_T MTDREC_StopInsertDIT(UINT8 u1PcrPidx);

#endif //MT_DREC_H

