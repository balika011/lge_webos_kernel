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
 * $RCSfile: dmx_mw.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_mw.h
 *  Demux driver - middleware interface
 */

#ifndef DMX_MW_H
#define DMX_MW_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtdmx.h"
#include "mtdmx_mwif.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_typedef.h"
#include "x_demux.h"

#ifdef TIME_SHIFT_SUPPORT
#include "x_rec_demux.h"
#endif // TIME_SHIFT_SUPPORT

LINT_EXT_HEADER_END

#include "dmx_if.h"
#ifdef ENABLE_MULTIMEDIA
#include "fvr_if.h"
#endif

#include "x_desc.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define MW_DMX_CRYPT_INVALID_KEYX           (0xFF)

#define MW_DMX_AES_KEY_LENGTH               8       // in 32-bit words
#define MW_DMX_AES_IV_KEY_LENGTH            4

#define MW_DMX_MULT2_SYSTEM_KEY_LENGTH      8
#define MW_DMX_MULT2_DATA_KEY_LENGTH        2
#define MW_DMX_MULT2_IV_KEY_LENGTH          2


#define MWDMX_CRYPTKEY_FLAG_NONE            0
#define MWDMX_CRYPTKEY_FLAG_MM_PLAY         0x00000001
#define MWDMX_CRYPTKEY_FLAG_FVR_PIDX        0x00000002
#define MWDMX_CRYPTKEY_FLAG_UPDATE_REF      0x00000004
#define MWDMX_CRYPTKEY_FLAG_STATIC_IDX      0x00000008

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef enum
{
    MW_DMX_CONN_TYPE_TUNER_0 = 0,
    MW_DMX_CONN_TYPE_TUNER_1,
    MW_DMX_CONN_TYPE_TUNER_2,
    MW_DMX_CONN_TYPE_TUNER_MAX,
    MW_DMX_CONN_TYPE_BUFAGENT_0,
    MW_DMX_CONN_TYPE_BUFAGENT_1,
    MW_DMX_CONN_TYPE_BUFAGENT_2,
    MW_DMX_CONN_TYPE_BUFAGENT_3,
    MW_DMX_CONN_TYPE_BUFAGENT_4,
    MW_DMX_CONN_TYPE_BUFAGENT_MAX,
    MW_DMX_CONN_TYPE_NONE,
} MW_DMX_PID_CONTYPE_T;

typedef struct
{
    BOOL fgValid;
    BOOL fgCached;
    UINT32 u4RMType;
    MW_DMX_PID_CONTYPE_T eConType;
	UINT8 u1TsIndex;  /*for every pid ,associate its ts_idx*/
} MW_DMX_PID_INFO_T;

typedef struct
{
    BOOL fgValid;
    BOOL fgCached;
    MW_DMX_PID_CONTYPE_T eConType;
} MW_DMX_FILTER_INFO_T;

typedef struct
{
    BOOL   fgEnable;
    UINT16 u2CompId;
    UINT16 u2DecType;
    UINT32 u4StrmId;
    UINT32 u4SubStrmId;
    UINT16 u2SrcCompId;
} DMX_MM_DEC_INFO_T;

#ifdef TIME_SHIFT_SUPPORT
typedef struct
{
    BOOL fgValid;
    UINT16 u2PidNum;
    RECDEMUX_NFY_INFO_T rNfyInfo;
    UINT32 u4TypeMask;
	MW_DMX_PID_CONTYPE_T eConType;
} DMX_MW_PVRPID_INFO_T;

typedef struct
{
    BOOL fgValid;
    MM_CRYPT_INFO_T rCryptInfo;
} MWPVR_KEY_INFO_T;
#endif // TIME_SHIFT_SUPPORT

#ifdef CI_PLUS_SUPPORT
typedef struct
{
    BOOL fgConnented;
    BOOL fgEnable;
    UINT16 u2PidNum;
    DESC_ALG_T rDescAlg;    //Descramble Algorithm
    DESC_SOURCE_T rDescSource;  //Descramble Source
    BOOL fgEven;            //Even or Odd
    UINT32 u4KeyLen;        //Key length
    UINT32 u4IvLen;         //Iv length
    UINT32 au4Key[8];       //Key value
    UINT32 au4Iv[4];        //Key value
} DMX_MW_DESC_INFO_T;
#endif // CI_PLUS_SUPPORT

typedef union {
    struct {
        UINT32 au4Key[MW_DMX_AES_KEY_LENGTH];      // key value
        UINT32 au4Iv[MW_DMX_AES_IV_KEY_LENGTH];    // IV value
    } rAESKeySet;

    struct {
        UINT32 au4SystemKey[MW_DMX_MULT2_SYSTEM_KEY_LENGTH];   // Even system key value
        UINT32 au4DataKey[MW_DMX_MULT2_DATA_KEY_LENGTH];       // Even data key value
        UINT32 au4Iv[MW_DMX_MULT2_IV_KEY_LENGTH];              // Even IV value
    } rMulti2KeySet;
} MW_DMX_CRYPT_KEY_UNION_T;

typedef struct
{
    BOOL fgEven;
    DESC_ALG_T eDescAlg;
    MW_DMX_CRYPT_KEY_UNION_T *puCryptKey;
} MW_DMX_CRYPT_KEY_INFO_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototype of inter-file functions
//-----------------------------------------------------------------------------

EXTERN UINT8 _MwDmxContypeToTsIdx(MW_DMX_PID_CONTYPE_T eConType);

/// Demux PSI part initialization
EXTERN BOOL _MW_DMX_PSI_Init(void);

/// PES part initialization
EXTERN BOOL _MW_DMX_PES_Init(void);

EXTERN BOOL _MW_DMX_SetPid(UINT8 u1Pidx, UINT32 u4Flags, const MTDMX_PID_T* prPid);

EXTERN BOOL _MW_DMX_CISetFrontEnd(DMX_FRONTEND_T eFEType);

EXTERN BOOL _MW_DMX_ConnectInfo(UINT8 u1Pidx, UINT32 u4RMType, DRV_TYPE_T eType, UINT16 u2Id);

EXTERN BOOL _MW_DMX_DisConnectInfo(UINT8 u1Pidx);

#ifdef CI_PLUS_SUPPORT
EXTERN BOOL _MwDmxTraverseFvrDesc(UINT8 u1TsIdx);
#endif

//#ifdef ENABLE_MULTIMEDIA
EXTERN BOOL _MW_DMX_IsPidConnedted(UINT8 u1Pidx);

EXTERN BOOL _MW_DMX_FlushAllCachePid(void);

EXTERN void _MW_DMX_SetCacheOn(BOOL fgCacheOn);

EXTERN BOOL _MW_DMX_GetCacheStatus(void);

EXTERN MW_DMX_PID_CONTYPE_T _MW_DMX_GetPidConType(UINT8 u1Pidx);

EXTERN BOOL _MW_DMX_FlushAllCacheFilter(void);

//#endif

/// Set notification callback function
EXTERN BOOL _MW_DMX_SetCallback(UINT8 u1Pidx,
    const DEMUX_NFY_INFO_T* prNfyInfo);

/// Reset notification callback function
EXTERN BOOL _MW_DMX_ResetCallback(UINT8 u1Pidx);


/// Demux Multimedia part initialization
EXTERN BOOL _MW_DMX_MM_Init(void);


#ifdef CC_53XX_SWDMX_V2
EXTERN VOID MW_DMX_NotifyStepDone(UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret, UINT32 u4SwdmxSrcId);
/// Demux Multimedia part, hook _AudSetCtrl
EXTERN VOID _MW_DMX_MM_AudSetCtrl(UINT8 u1SwdmxSrcId, UINT8 u1DecId, UINT32 u4AudCtrl);
/// Demux Multimedia part, hook _AudMwNfyFunc
EXTERN VOID _MW_DMX_MM_AudMwNfyFunc(void *pvNfyTag, UINT8 u1DecId,
                UINT32 u4NfyCond, UINT32 u4Data1, UINT32 u4Data2, UINT32 u4Data3);
#else
EXTERN VOID MW_DMX_NotifyStepDone(UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret);
/// Demux Multimedia part, hook _AudSetCtrl
EXTERN VOID _MW_DMX_MM_AudSetCtrl(UINT8 u1DecId, UINT32 u4AudCtrl);
/// Demux Multimedia part, hook _AudMwNfyFunc
EXTERN VOID _MW_DMX_MM_AudMwNfyFunc(void *pvNfyTag, UINT8 u1DecId,
                UINT32 u4NfyCond, UINT32 u4Data1, UINT32 u4Data2);
#endif

#ifdef TIME_SHIFT_SUPPORT
// Record DMX
EXTERN DMX_MW_PVRPID_INFO_T* _MW_Rec_GetPidInfo(UINT8 u1Pidx);
#endif  // TIME_SHIFT_SUPPORT


EXTERN BOOL _MW_DMX_CRYPT_Init(void);

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_AllocateCAKeySlot
 *
 *  Allocate a key index for DTV and MM play.
 *  If MWDMX_CRYPTKEY_FLAG_UPDATE_REF is enabled, the key index reference will
 *  be updated.  If key index reference is 0, the key index which has been allocated
 *  will be invalidated. This design is used for the case that crypt keys are
 *  changed periodically. In current design, there is no key index for MM play.
 *  Thus, the flags which are MWDMX_CRYPTKEY_FLAG_FVR_PIDX and MWDMX_CRYPTKEY_FLAG_UPDATE_REF,
 *  and u1Pidx parameter are useless for MM play.
 *
 *
 *  @param  prKeyInfo                   The key information
 *  @param  u4Flags                     Function flags
 *            MWDMX_CRYPTKEY_FLAG_NONE        None
 *            MWDMX_CRYPTKEY_FLAG_MM_PLAY     Allocate key index for MM play
 *            MWDMX_CRYPTKEY_FLAG_FVR_PIDX    The given u1Pidx is a pidx of FVR
 *            MWDMX_CRYPTKEY_FLAG_UPDATE_REF  Update key index reference
 *  @param  u1Pidx                      The corresponding Pidx
 *  @param  pu1ReturnIdx                The pointer which points to the returned key index
 *
 *  @retval TRUE                        Succeed
 *  @retval FALSE                       Fail
 *
 */
//-----------------------------------------------------------------------------

EXTERN BOOL _MW_DMX_CRYPT_AllocateCAKeySlot(MW_DMX_CRYPT_KEY_INFO_T *prKeyInfo,
                                        UINT32 u4Flags, UINT8 u1Pidx,
                                        UINT8 *pu1ReturnIdx);

EXTERN BOOL _MW_DMX_CRYPT_AllocateMarlinTSKeySlot(UINT32 u4Flags, 
                                    UINT16 u2compid,
                                    UINT8 *pu1ReturnIdx);

EXTERN BOOL _MW_DMX_CRYPT_ResetCAKeySlotByKeyIdx(UINT8 u1KeyIdx);

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetCAKeySlotByPidx
 *
 * In current design, this API is only useful to reset Pidx of DTV excluding FVR
 */
//-----------------------------------------------------------------------------
EXTERN BOOL _MW_DMX_CRYPT_ResetCAKeySlotByPidx(UINT8 u1Pidx, BOOL fgFvrPidx);

EXTERN BOOL _MW_DMX_CRYPT_ResetAllCAKeySlots(BOOL fgMMplay);

EXTERN BOOL _MW_DMX_CRYPT_AllocateFvrKeySlot(MW_DMX_CRYPT_KEY_INFO_T *prKeyInfo, UINT8 *pu1ReturnIdx);
EXTERN BOOL _MW_DMX_CRYPT_ResetFvrKeySlot(UINT8 u1KeyIdx);
EXTERN BOOL _MW_DMX_CRYPT_ResetAllFvrKeySlots(void);


//-----------------------------------------------------------------------------
// Prototype of public functions
//-----------------------------------------------------------------------------


#endif  // DMX_MW_H

