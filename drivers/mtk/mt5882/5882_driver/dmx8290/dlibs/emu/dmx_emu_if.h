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
 * $RCSfile: dmx_emu_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_if.h
 *  Interface of demux emulation driver
 */


#ifndef DMX_EMU_IF_H
#define DMX_EMU_IF_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "dmx_if.h"
#include "dmx_mm_if.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
;
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
typedef enum
{
    FULL_PUSH_MODE = 0,
    HALF_PULL_PUSH_MODE = 1,
    FULL_PULL_MODE = 3
} PUL_MODE_T;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
/// Emulation
//---------------------------------------------------------------------
EXTERN BOOL DMX_EMU_DDI_Init(UINT8 u1TsIndex);

EXTERN BOOL DMX_EMU_DDI_Exit(void);

EXTERN BOOL DMX_EMU_DDI_Transfer0(UINT8 u1Type, UINT8 u1TsIndex,
                                  UINT32 u4Size ,PUL_MODE_T eMode, BOOL fgBypassFramer);

EXTERN BOOL DMX_EMU_DDI_Transfer1(UINT8 u1Type, UINT32 u4Size ,PUL_MODE_T eMode);

EXTERN BOOL DMX_EMU_FramerUnlock_Test(UINT8 u1TsIndex, UINT32 u4Size ,PUL_MODE_T eMode );


EXTERN BOOL DMX_EMU_PVR_ResetTransfer(UINT8 u1TsIndex,BOOL fgBypassFramer, UINT8 u1TsoutIdx, UINT32 u4Size, BOOL fgIntr);

EXTERN BOOL DMX_EMU_DDI_ResetTransfer(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT32 u4Size, BOOL fgIntr);
/// TS pull mode

EXTERN BOOL DMX_EMU_PullTsTransfer0(UINT8 u1TsIndex, UINT32 u4Size,
                                    UINT16 u2Pid ,PUL_MODE_T eMode, BOOL fgBypassFramer);

EXTERN BOOL DMX_EMU_PullTsTransfer1(UINT8 u1TsIndex, UINT32 u4Size,
                                    UINT16 u2Pid ,PUL_MODE_T eMode);

EXTERN BOOL DMX_EMU_PullTsTransfer3(UINT8 u1TsIndex, UINT32 u4Size,
                                    UINT16 u2Pid ,PUL_MODE_T eMode);


EXTERN BOOL DMX_EMU_PullTsTransfer2(UINT32 u4Size ,PUL_MODE_T eMode);

EXTERN BOOL DMX_EMU_InitPullTsBuffers(UINT8 u1TsIndex);

EXTERN BOOL DMX_EMU_ReleasePullTsBuffers(void);

/// ES pull mode

EXTERN BOOL DMX_EMU_PullEsTransfer0(UINT8 u1TsIndex, UINT32 u4Size,
                                    UINT16 u2Pid ,PUL_MODE_T eMode);

EXTERN BOOL DMX_EMU_PullEsTransfer1(UINT8 u1TsIndex, UINT32 u4Size,
                                    UINT16 u2Pid ,PUL_MODE_T eMode);

// DES emulation

EXTERN BOOL DMX_EMU_Des0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type);

// AES emulation

EXTERN BOOL DMX_EMU_AES_SelfTest(void);

EXTERN BOOL DMX_EMU_AES_SelfTest2(void);

EXTERN BOOL DMX_EMU_AES_SelfTest3(void);

EXTERN BOOL DMX_EMU_AES_SelfTest4(UINT8 u1TsIndex);

EXTERN BOOL DMX_EMU_Aes0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type);

// DRM emulation

EXTERN BOOL DMX_EMU_Drm0(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid,
                         DMX_DRM_MODE_T eMode);

EXTERN BOOL DMX_EMU_EncryptAes(UINT32 u4DstAddr, UINT32 u4SrcAddr, UINT32 u4Size, DMX_MM_DRM_AES_T* prAES);

EXTERN BOOL DMX_EMU_DrmNdSelfTest(void);

// Pattern Match
EXTERN BOOL DMX_EMU_PM_Transfer(UINT8 u1TsIndex);

// TS_output
EXTERN BOOL DMX_EMU_TSOut_Init(UINT8 u1TsIndex, UINT32 u4Size);
EXTERN BOOL DMX_EMU_TSOut_StressTest(void);

// Multi2 emulation
EXTERN BOOL DMX_EMU_Multi2Test(UINT8 u1TsIndex, UINT8 u1Type);

// DVB decryption
EXTERN BOOL DMX_EMU_DvbMain(UINT8 u1TsIndex, UINT8 u1Type, BOOL fgBypassFramer);

EXTERN BOOL DMX_EMU_SMS4(UINT8 u1TsIndex, UINT32 u4Size, UINT16 u2Pid, UINT8 u1Type);

//IOMMU
EXTERN BOOL DMX_EMU_DDI_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode);

EXTERN BOOL DMX_EMU_DMX_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode);

EXTERN BOOL DMX_EMU_DMXDDI_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode);

EXTERN BOOL DMX_EMU_IOMMU_PerformanceTest(UINT8 u1TsIndex, BOOL fgBypassFramer, BOOL fgIommu1, BOOL fgIommu2, BOOL fgIommu3);

EXTERN BOOL DMX_VideoEstest (UINT8 u1TsIdx, PUL_MODE_T eMode, BOOL fgBypassFramer);

EXTERN BOOL DMX_EMU_SecureBuffer_Test (UINT8 u1TsIdx, PUL_MODE_T eMode, BOOL fgBypassFramer);

EXTERN BOOL DMX_TsOut_ToCaOutput(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1TsoutIdx);

EXTERN BOOL DMX_TsOut_ToFtup(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1TsoutIdx);

EXTERN BOOL DMX_EMU_FVR_TimestampOnOffTest(UINT8 u1TsIndex, BOOL fgTimeStampOn, UINT8 u1TsOutIdx,UINT8 u1_PlayTsIndex);

EXTERN BOOL DMX_EMU_FVR_TimestampPauseTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex);

EXTERN BOOL DMX_EMU_FVR_TimestampErrTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex);

EXTERN BOOL DMX_EMU_FVR_TimestampResetTest(UINT8 u1TsIndex, UINT8 u1TsOutIdx,UINT8 u1_Play_TsIndex);


EXTERN BOOL DMX_EMU_DBM_Auto_SwitchTest(UINT8 u1TsOutIdx,  BOOL fgEnable_AutoSwitch);

EXTERN BOOL DMX_EMU_PSICALoopback_Test(UINT8 u1TsIdex, BOOL fgGenPat, DMX_FILTER_MODE_T eMode, UINT8 u1TsOutIdx);

EXTERN BOOL DMX_EMU_PSILoopback_Test(UINT8 u1TsIdex, BOOL fgGenPat, DMX_FILTER_MODE_T eMode,  UINT8 u1TsOutIdx);

EXTERN BOOL DMX_EMU_DDI_Bypass_Test(UINT8 u1TsIndex, BOOL fgRec);

EXTERN BOOL DMX_Emu_130TsTest(BOOL fgExtSync , UINT8 u1Tsindex);

EXTERN BOOL DMX_Emu_Tsout130_TransmitData(BOOL fgExtSync ,UINT8 u1Tsindex,UINT8 u1TsOutIndex);

EXTERN BOOL DMX_EMU_Multi_PacketSize_Test( BOOL fgFramer0Reset, BOOL fgFramer1Reset, UINT8 u1TsoutIdx);

EXTERN BOOL DMX_EMU_FVR_PES_Test(UINT8 u1TsIndex);

EXTERN BOOL DMX_EMU_DDI_TransferTest(UINT8 u1Mode, UINT8 u1TsIndex, BOOL fgBypassFramer);


#endif  // DMX_EMU_IF_H

