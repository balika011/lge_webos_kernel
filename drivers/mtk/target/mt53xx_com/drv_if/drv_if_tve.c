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
 * $RCSfile: drv_if_tuner.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file tuner.c
 *  Driver interface: TUNER part implementation
 */

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_os.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_drv_if.h"

#include "vdo_common.h"
#include "video_def.h"
#include "nptv_if.h"
#include "tve_if.h"
#include "vdp_drvif.h"
#include "source_select.h"
#include "b2r_if.h"


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define SCART_OUT_NS 2
#define SCART_1_IDX 1
#define SCART_2_IDX 0

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#ifdef CC_SUPPORT_TVE
#ifndef __MODEL_slt__
static TveUsage_t _rTveUsed = {0xf, 0x0};
static UINT8 _au1ScartOutSource[SCART_OUT_NS] = {SV_VS_MAX, SV_VS_MAX};
static UINT8 _au1ParamSource[SCART_OUT_NS] = {SV_VS_MAX, SV_VS_MAX};
#endif
#endif

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Extern functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported functionss
//-----------------------------------------------------------------------------

DRV_ERR_CODE_T DRV_TVE_Init()
{
#ifdef CC_SUPPORT_TVE
    TVE_Init();
    Bypass_Init();
#endif

    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TVE_SetSource(UINT8 u1ScartIdx, UINT8 u1Source)
{
#ifndef __MODEL_slt__
#ifdef CC_SUPPORT_TVE
    _au1ParamSource[u1ScartIdx] = u1Source;
#endif
#endif

    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TVE_Start(UINT8 u1ScartIdx)
{
#ifndef __MODEL_slt__
    if(_DRV_VDO_GetState(1) == DRV_VDO_STATE_PLAY) // sub is playing
    {
        return DRV_E_FAIL;
    }

#ifdef CC_SUPPORT_TVE
    _au1ScartOutSource[u1ScartIdx] = _au1ParamSource[u1ScartIdx];
    vApiTVESetDacMuteMask(u1ScartIdx, TVE_DAC_MUTE_MASK_DRV, TRUE);
    vApiTVESetScartOutCtrl(u1ScartIdx, &_au1ScartOutSource[u1ScartIdx], &_rTveUsed);

    if(u1ScartIdx == _rTveUsed.TveIsUsedBy)
    {
        VDP_SetEnable(VDP_2, TRUE);
        TVE_SetEnable(TVE_1, TRUE);
    }
    vApiTVESetDacMuteMask(u1ScartIdx, TVE_DAC_MUTE_MASK_MW, FALSE);
#endif
#endif

    return DRV_E_OK;
}

DRV_ERR_CODE_T DRV_TVE_Stop(UINT8 u1ScartIdx)
{
#ifndef __MODEL_slt__
#ifdef CC_SUPPORT_TVE
    if(_rTveUsed.TveIsUsedBy == u1ScartIdx)
    {
        TVE_SetEnable(TVE_1, FALSE);
        VDP_SetEnable(VDP_2, FALSE);
        if(VSS_MAJOR(_au1ScartOutSource[u1ScartIdx]) == VSS_DTV)
        {
            VDP_SetInput(VDP_2, 0xff, 0);
        }
        bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_MAX);
        vApiTVESetOutputFormat(SV_TVE_OFMT_NONE);
        _rTveUsed.TveIsUsedBy = 0xf;
    }
    _au1ScartOutSource[u1ScartIdx] = SV_VS_MAX;
    vApiTVESetScartOutCtrl(u1ScartIdx, &_au1ScartOutSource[u1ScartIdx], &_rTveUsed);
    vApiTVESetDacMuteMask(u1ScartIdx, TVE_DAC_MUTE_MASK_MW, TRUE);
#endif
#endif
    return DRV_E_OK; 
}

