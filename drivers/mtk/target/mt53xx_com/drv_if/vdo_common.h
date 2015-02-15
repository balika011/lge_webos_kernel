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
 * $RCSfile: vdo_common.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdo_common.h
 *  Driver interface: common VDO part implementation
 */

#ifndef VDO_COMMON_H
#define VDO_COMMON_H

#include "drv_if_config.h"

#define DRV_VDO_NUM_PLANE 2

#define DRV_VDO_STATE_STOP 0
#define DRV_VDO_STATE_PLAY 1


// The structure for DRV_VDO_SetParameter
typedef struct
{
    UINT8 u1Pidx;
    UINT8 u1State;
    UINT8 u1TOverscan;
    UINT8 u1BOverscan;
    UINT8 u1LOverscan;
    UINT8 u1ROverscan;        
    DRV_VDO_INFO_T rInfo;
} DRV_VDO_INTERNAL_T;

extern DRV_ERR_CODE_T _DRV_VDO_Init(void);

extern DRV_ERR_CODE_T _DRV_VDO_SetParameter(UINT8 u1VideoIndex, 
	DRV_VDO_PARAM_T* prParam);

extern DRV_ERR_CODE_T _DRV_VDO_GetParameter(UINT8 u1VideoIndex, 
	DRV_VDO_PARAM_T** pprParam);

extern DRV_ERR_CODE_T _DRV_VDO_GetInternal(UINT8 u1VideoIndex, 
	DRV_VDO_INTERNAL_T** prInternal);

extern DRV_ERR_CODE_T _DRV_VDO_GetResolution(UINT8 u1VideoIndex,
	DRV_VDO_INFO_T* prInfo, UINT32 u4Tag);

extern DRV_ERR_CODE_T _DRV_VDO_Start(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_Stop(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_Freeze(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_Resume(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_Mute(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_Unmute(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_GetInfo(UINT8 u1VideoIndex, 
	DRV_VDO_INFO_T* prInfo);

extern DRV_ERR_CODE_T _DRV_VDO_Set3DTV(UINT8 u1VideoIndex, 
    DRV_VDO_3DTV_STATUS_T r3DStatus);

extern DRV_ERR_CODE_T _DRV_VDO_SetOutputWindow(UINT8 u1VideoIndex, 
	DRV_VDO_WINDOW_T* prWindow);

extern DRV_ERR_CODE_T _DRV_VDO_SetOverScanRatio(UINT8 u1VideoIndex,
    UINT32 u4T, UINT32 u4B, UINT32 u4L, UINT32 u4R);

extern DRV_ERR_CODE_T _DRV_VDO_InitQuality(void);

extern DRV_ERR_CODE_T _DRV_VDO_SetQuality(UINT8 u1VideoIndex, 
	UINT32 u4Mask, DRV_VDO_QUALITY_T* prQuality);

extern DRV_ERR_CODE_T _DRV_VDO_SetDispFmt(UINT8 u1VideoIndex, 
	DRV_VDO_DISP_FMT_T rFmt);

extern DRV_ERR_CODE_T _DRV_VDO_DriverMute(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_DriverUnmute(UINT8 u1VideoIndex);

extern UINT32 _DRV_VDO_GetState(UINT8 u1VideoIndex);

extern DRV_ERR_CODE_T _DRV_VDO_GetMessage(INT32 i4Tag);

#endif
