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
 * $RCSfile: b2r_if.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdp_if.c
 *  Video Plane driver - public interface
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "b2r_drvif.h"
#include "b2r_if.h"
#include "fbm_drvif.h"
#include "vdp_if.h"
#include "vdp_drvif.h"
#include "drv_scaler.h"
#include "vdp_vsync.h"
#include "vdp_frc.h"
#include "vdp_hal.h"
#include "vdp_image.h"
#include "vdp_debug.h"
#include "drv_video.h"
#include "nptv_if.h"
#include "vdp_display.h"
#include "mute_if.h"
#include "video_def.h"
#include "sv_const.h"
#include "vdec_drvif.h"
#include "srm_drvif.h"
#include "drv_mpeg.h"

#ifdef CHANNEL_CHANGE_LOG
#include "x_timer.h"
#endif
#ifdef TIME_MEASUREMENT
#include "x_time_msrt.h"
#include "u_time_msrt_name.h"
#endif

#ifdef CC_FAST_INIT
#include "x_bim.h"
#endif

#include "x_lint.h"
#include "x_assert.h"
LINT_EXT_HEADER_END


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

#ifndef CC_CLI
#define CC_CLI
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


EXTERN void _VDPSetChkPTS(UINT32 u4Pts);
EXTERN void _VDP_SetEncTrigger(BOOL fgEnable);
EXTERN BOOL _VDP_SendB2RChgFrameMsg(VDP_B2R_CHG_FRAME_MSG_T* prMsg);
EXTERN BOOL _VDP_FlushB2RChgFrameMsg(UCHAR ucVdpId);
EXTERN VOID _VDP_SetEnableLog(UCHAR ucVdpId,BOOL fgEnableGstLog, BOOL fgEnableVideoLog);
EXTERN BOOL _VDP_GetEnableGstLog(VOID);

typedef void (*VDP_GET_PIC_TYPE_CB_FUNC)(VDEC_PICTYPE_CB_T* prPicType);

//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

extern VDP_CB_FUNC_T _rVdpCbFunc; /* To include VDP callback function */

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static VDP_CONF_T* _prVdpConf[VDP_NS];
static B2R_PRM_T*    _prB2rPrm[B2R_NS];
static UCHAR _aucImageConnected[VDP_NS];

/* Digital decoder variable-dram-size notification */
static HANDLE_T _hB2RVdecConfMutex = (HANDLE_T)(NULL);

static UCHAR _aucThumbnailMode[VDP_NS]; // Thumbnail mode in MM mode

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define VERIFY_VDP_ID(id, ret)                        \
        do {                                          \
            if ((id) >= VDP_NS) { return (ret); }     \
        } while (0)
#define VERIFY_VDP_ID_RET_VOID(id)                    \
        do {                                          \
            if ((id) >= VDP_NS) { return ; }          \
        } while (0) 

#define VERIFY_B2R_ID(id, ret)                        \
        do {                                          \
            if ((id) >= B2R_NS) { return (ret); }     \
        } while (0)

#define VERIFY_B2R_ID_RET_VOID(id)                    \
        do {                                          \
            if ((id) >= B2R_NS) { return ; }          \
        } while (0) 

#define VDP_MUTEX_LOCK                  (VERIFY(x_sema_lock(VDP_GetMutex(), X_SEMA_OPTION_WAIT) == OSR_OK))

#define VDP_MUTEX_UNLOCK                (VERIFY(x_sema_unlock(VDP_GetMutex()) == OSR_OK))

#define VDP2B2RID(ucVdpId)              (_prVdpConf[ucVdpId]->ucB2rId) 

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static void _B2RSrmVDecMemConfCb(UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3, UINT32 u4Arg4)
{
    UNUSED(u4Arg1);
    UNUSED(u4Arg2);
    UNUSED(u4Arg3);
    UNUSED(u4Arg4);
    LOG(9, "[SRM] Callback _B2RSrmVDecMemConfCb\n");
    VERIFY (x_sema_unlock(_hB2RVdecConfMutex) == OSR_OK);
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

/**
 * VDP DTV frame buffer group ready check
 *
 * @param void
 * @return void
 */
#ifdef CC_53XX_SWDMX_V2
void _VdpCheckFbgReady(UCHAR ucFbgId, UCHAR ucEsId)
{
    UCHAR  ucGetFbgId;
    UINT32 u4EsId = (UINT32)ucEsId;
    UINT32 u4FbgNs;
    //UINT32 u4PortIdx;
    UINT32 u4FbgIdx;
    UINT32 u4VdpId;
    UINT32 u4DisplayReady =0;

    UNUSED(u4VdpId);

    u4FbgNs = FBM_GetFbgNs();
    for (u4FbgIdx = 0; u4FbgIdx < u4FbgNs; u4FbgIdx++)
    {
        u4EsId = (UINT32)ucEsId;
        u4DisplayReady = 0;
        if(u4EsId >= 0xFF)
        {
            for(u4EsId =0 ; u4EsId< MAX_ES_NS ; u4EsId++)
            {
                ucGetFbgId = FBM_GetFbgByEs(u4EsId);
                if (ucGetFbgId == (UCHAR)u4FbgIdx)
                {
                    u4DisplayReady = 1;
                    break;
                }
            }
        }
        else
        {
            if(u4EsId < MAX_ES_NS)
            {
                ucGetFbgId = FBM_GetFbgByEs(u4EsId);
                if (ucGetFbgId == (UCHAR)u4FbgIdx)
                {
                    u4DisplayReady = 1;
                    break;
                }
            }
            else
            {
                ASSERT((u4EsId < MAX_ES_NS));
            }
        }
        if(u4DisplayReady == 1)
        {
            break;
        }
    }

    if(u4FbgIdx < u4FbgNs)
    {
        if (u4DisplayReady)
        {
            FBM_SetFrameBufferFlag((UCHAR)u4FbgIdx, FBM_FLAG_DISP_READY);
        }
        else
        {
            FBM_ClrFrameBufferFlag((UCHAR)u4FbgIdx, FBM_FLAG_DISP_READY);
            FBM_SetFrameBufferFlag((UCHAR)u4FbgIdx, FBM_FLAG_RESET);
        }
    }

#if 1//ndef CC_SECOND_B2R_SUPPORT
    for (u4VdpId = 0; u4VdpId < VDP_NS; u4VdpId++)
    {
        u4EsId = _prVdpConf[u4VdpId]->ucInputPort[0];

        if (u4EsId >= MAX_ES_NS)
        {
            return;
        }

        ucGetFbgId = FBM_GetFbgByEs(u4EsId);
        if ((ucGetFbgId == FBM_FBG_ID_UNKNOWN) &&
                (_aucImageConnected[u4VdpId] == 0))
        {
            _VDP_StatusNotify(u4VdpId, VDP_B2R_NO_SIGNAL);
        }
    }
#endif
    UNUSED(ucFbgId);
}
#else
void _VdpCheckFbgReady(void)
{
    UCHAR  ucFbgId;
    UCHAR  ucEsId;
    UINT32 u4FbgNs;
    UINT32 u4PortIdx;
    UINT32 u4FbgIdx;
    UINT32 u4VdpId;
    UINT32 u4DisplayReady;

    u4FbgNs = FBM_GetFbgNs();
    for (u4FbgIdx = 0; u4FbgIdx < u4FbgNs; u4FbgIdx++)
    {
        u4DisplayReady = 0;
        for (u4VdpId = 0; u4VdpId < VDP_NS; u4VdpId++)
        {
//            if ((_prVdpConf[u4VdpId]->ucEnable) &&
//                (_prVdpConf[u4VdpId]->ucMode != VDP_MODE_BG))
            if (_prVdpConf[u4VdpId]->ucEnable)
            {
                for (u4PortIdx = 0; u4PortIdx < VDP_MAX_INPORT_NS; u4PortIdx++)
                {
                    ucEsId = _prVdpConf[u4VdpId]->ucInputPort[u4PortIdx];

                    if (ucEsId < MAX_ES_NS)
                    {
                        ucFbgId = FBM_GetFbgByEs(ucEsId);
                        if (ucFbgId == u4FbgIdx)
                        {
                            u4DisplayReady = 1;
                            u4PortIdx = VDP_MAX_INPORT_NS;
                            u4VdpId = VDP_NS;
                        }
                    }
                }
            }
        }

        if (u4DisplayReady)
        {
            FBM_SetFrameBufferFlag(u4FbgIdx, FBM_FLAG_DISP_READY);
        }
        else
        {
            FBM_ClrFrameBufferFlag(u4FbgIdx, FBM_FLAG_DISP_READY);
            FBM_SetFrameBufferFlag(u4FbgIdx, FBM_FLAG_RESET);
        }
    }

    for (u4VdpId = 0; u4VdpId < VDP_NS; u4VdpId++)
    {
        ucEsId = _prVdpConf[u4VdpId]->ucInputPort[0];

        if (ucEsId >= MAX_ES_NS)
        {
            return;
        }

        ucFbgId = FBM_GetFbgByEs(ucEsId);
        if ((ucFbgId == FBM_FBG_ID_UNKNOWN) &&
                (_aucImageConnected[u4VdpId] == 0))
        {
            _VDP_StatusNotify(u4VdpId, VDP_B2R_NO_SIGNAL);
        }
    }
}
#endif
/**
 * VDP DTV frame buffer group change notify
 *
 * @param ucFbgId specify the frame buffer group id.
 * @param ucEsId specify the video decoder id.
 * @return void
 */
void _VdpFbgChgNotify(UCHAR ucFbgId, UCHAR ucEsId)
{
    UINT32 u4VdpIdx;
    UINT32 u4PortIdx;
#ifdef CC_DYNAMIC_MPEG_SIZE /* Digital decoder variable-dram-size notification */
    UINT32 u4FbWidth;
    UINT32 u4FbHeight;
#endif

    ASSERT(ucEsId < MAX_ES_NS);

    VDP_MUTEX_LOCK;

    for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
    {
        for (u4PortIdx = 0; u4PortIdx < VDP_MAX_INPORT_NS; u4PortIdx++)
        {
            if (_prVdpConf[u4VdpIdx]->ucInputPort[u4PortIdx] == ucEsId)
            {
            #ifndef FBM_VIRTUAL_MPEG
                UINT8 bType;
                bType = bGetVideoDecType(u4VdpIdx);
                if((SV_VD_MPEGHD == bType) || (SV_VD_MPEGSD == bType))
            #endif
                {
                    //_VDP_SetScposDelayUnMute(u4VdpIdx, 10, 1);
                    _vDrvVideoSetMute(MUTE_MODULE_B2R, u4VdpIdx, 10, TRUE);
                    _VDP_VsyncSendCmd(VDP2B2RID(u4VdpIdx), VDP_CMD_SET_INPUT);
                }
                u4PortIdx = VDP_MAX_INPORT_NS;

            }
        }
    }

#ifdef CC_53XX_SWDMX_V2
    _VdpCheckFbgReady(ucFbgId, ucEsId);
#else
    _VdpCheckFbgReady();
#endif

    VDP_MUTEX_UNLOCK;

#ifdef CC_DYNAMIC_MPEG_SIZE /* Digital decoder variable-dram-size notification */
    if (ucFbgId < FBM_FBG_ID_UNKNOWN)
    {
        for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
        {
            for (u4PortIdx = 0; u4PortIdx < VDP_MAX_INPORT_NS; u4PortIdx++)
            {
                if (_prVdpConf[u4VdpIdx]->ucInputPort[u4PortIdx] == ucEsId)
                {
                    if(u4VdpIdx ==0)
                    {
                        FBM_GetFrameBufferSize(ucFbgId, &u4FbWidth, &u4FbHeight);
                        SRM_SendEvent(SRM_DRV_SCPOS, (SRM_SCPOS_EVENT_MPEG_SIZE + (UINT32)u4VdpIdx),
                                      (u4FbWidth*u4FbHeight),
                                      FBM_GetRunningVdecMemSize(ucFbgId));
                        vMpegModeChg(u4VdpIdx);
                        vMpegModeDetDone(u4VdpIdx);
                        VERIFY (x_sema_lock(_hB2RVdecConfMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
                    }
                }
            }
        }
    }
#endif
}

#if 1//def CC_SUPPORT_TVE
extern UCHAR _ucSameB2rVdp;
#endif

/**
 * VDP DTV video frontend enable/disable
 *
 * @param ucVdpId specify the video plane id.
 * @return void
 */
void _B2rSetEnable(UCHAR ucVdpId)
{
    UCHAR ucEnable;
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    
#ifdef FBM_VIRTUAL_MPEG
    _prVdpConf[ucVdpId]->ucB2rId = B2R_1;
#endif
    LOG(1, "B2R_IF: VDP(%d) Enable(%d -> %d) B2R(%d)\n", ucVdpId, _prVdpConf[ucVdpId]->ucEnable, _prVdpConf[ucVdpId]->ucVdpEnable,  _prVdpConf[ucVdpId]->ucB2rId);
    ucEnable = _prVdpConf[ucVdpId]->ucEnable;
#ifdef CC_SCPOS_EN
    if ((_prVdpConf[ucVdpId]->ucB2rId < B2R_NS) &&
            (_prVdpConf[ucVdpId]->ucVdpEnable))
    {
        // B2R enable = switch to B2R & VDP enable
        _prVdpConf[ucVdpId]->ucEnable = 1;
    }
    else
    {
        _prVdpConf[ucVdpId]->ucEnable = 0;
    }
#else
    _prVdpConf[ucVdpId]->ucEnable = _prVdpConf[ucVdpId]->ucVdpEnable;
#endif
    if (ucEnable != _prVdpConf[ucVdpId]->ucEnable)
    {
        if (_prVdpConf[ucVdpId]->ucEnable)
		{
			LOG(1, "B2R_IF: VDP(%d) Enable B2R(%d)\n", ucVdpId, _prVdpConf[ucVdpId]->ucB2rId);
		}
		else
		{
			LOG(1, "VDP(%d) disable B2R(%d)\n", ucVdpId, _prVdpConf[ucVdpId]->ucB2rId);
		}
		if (_prVdpConf[1 - ucVdpId]->ucEnable && !(_prVdpConf[ucVdpId]->ucEnable) && 
			_prVdpConf[ucVdpId]->ucB2rId == _prVdpConf[1 - ucVdpId]->ucB2rId)
		{
			return;
		}
        _prVdpConf[ucVdpId]->ucStatus = VDP_STATUS_NOSIGNAL;

#ifdef CC_53XX_SWDMX_V2
        _VdpCheckFbgReady(0xFF, 0xFF);
#else
        _VdpCheckFbgReady();
#endif
        if (!(_prVdpConf[ucVdpId]->ucEnable))
        {
            VDP_EnableBurstRead(_prVdpConf[ucVdpId]->ucB2rId, TRUE);
        }
        _VDP_VsyncSendCmd(_prVdpConf[ucVdpId]->ucB2rId, VDP_CMD_SET_ENABLE);
        _VDP_FinishSeekDoneBeforeEnding(ucVdpId);
        if (_prVdpConf[1 - ucVdpId]->ucEnable && _prVdpConf[ucVdpId]->ucEnable && 
            _prVdpConf[ucVdpId]->ucB2rId == _prVdpConf[1 - ucVdpId]->ucB2rId &&
            _prVdpConf[1 - ucVdpId]->ucStatus == VDP_STATUS_STABLE)
        {
            _prVdpConf[ucVdpId]->ucStatus = _prVdpConf[1-ucVdpId]->ucStatus;
            vMpegModeChg(ucVdpId);
            vMpegModeDetDone(ucVdpId);
            return;
        }
    }
}

/**
 * DTV status notify
 *
 * @param ucVdpId specify the video plane id.
 * @param u4Status specify DTV status.
 * @return void
 */
void _VDP_StatusNotify(UCHAR ucVdpId, UINT32 u4Status)
{
    LOG(3, "DTV(%d) Event(%d)\n", ucVdpId, u4Status);
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));

    VDP_MUTEX_LOCK;

#ifdef __MODEL_slt__
    if (_prVdpConf[ucVdpId]->ucB2rId < B2R_NS)
    {
        if (u4Status == VDP_B2R_START_PLAY)
        {
            _prVdpConf[ucVdpId]->ucStatus = VDP_STATUS_STABLE;
        }
        vMpegModeChg(ucVdpId);
        vMpegModeDetDone(ucVdpId);
    }

#else

#ifdef CC_SCPOS_EN
    if (_prVdpConf[ucVdpId]->ucB2rId < B2R_NS)
    {
        UINT32 u4IssueModeChange = 0;

        // When DTV signal not ready, mute SCPOS
        if ((u4Status == VDP_B2R_NO_SIGNAL))
        {
            _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);
            LOG(3, "DTV Mute B2R No Signal VDP(%d)\n", ucVdpId);
        }
        else if (u4Status == VDP_B2R_START_PLAY)
        {
#ifdef CC_SUPPORT_TVE
            BOOL bIsScartOut = (_prVdpConf[VDP_1]->ucB2rId == B2R_1) &&
                                (_prVdpConf[VDP_2]->ucB2rId == B2R_1)&&
                                (_prVdpConf[VDP_1]->ucEnable == 1) &&
                                (_prVdpConf[VDP_2]->ucEnable == 1);
#endif
            if(_prB2rPrm[VDP2B2RID(ucVdpId)]->ucReady == 1)
            {
#ifdef VDP_MUTE_EARLY_DISPLAY
                if(_prVdpConf[ucVdpId]->ucEarlyDisp == 1)
                {
                    LOG(3, "Original seq chg + video frame sync ready(%d) VDP(%d) earlydisp(%d)\n",_prB2rPrm[VDP2B2RID(ucVdpId)]->ucReady, ucVdpId, _prVdpConf[ucVdpId]->ucEarlyDisp);
                    _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);
#ifdef CC_SUPPORT_TVE
                    if (bIsScartOut)
                    {
                        LOG(3, "Original seq chg + video frame sync ready(%d) VDP(%d) earlydisp(%d)\n",_prB2rPrm[VDP2B2RID(1-ucVdpId)]->ucReady, (1-ucVdpId), _prVdpConf[1-ucVdpId]->ucEarlyDisp);
                        _vDrvVideoSetMute(MUTE_MODULE_DTV, (1 - ucVdpId), FOREVER_MUTE, FALSE);
                    }
#endif
                }
                else
#endif
                {
                    #ifdef TIME_MEASUREMENT
                    TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "B2R Set Unmute");
                    #endif
                    LOG(3, "Original seq chg + video frame sync ready(%d) VDP(%d) unearlydisp(%d)\n",_prB2rPrm[VDP2B2RID(ucVdpId)]->ucReady, ucVdpId, _prVdpConf[ucVdpId]->ucEarlyDisp);
                    _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, 0, FALSE);
#ifdef CC_SUPPORT_TVE
                    if (bIsScartOut)
                    {
                        LOG(3, "Original seq chg + video frame sync ready(%d) VDP(%d) unearlydisp(%d)\n",_prB2rPrm[VDP2B2RID(1-ucVdpId)]->ucReady, (1-ucVdpId), _prVdpConf[1-ucVdpId]->ucEarlyDisp);
                        _vDrvVideoSetMute(MUTE_MODULE_DTV, (1-ucVdpId), 0, FALSE);
                    }
#endif
                }
            }
            else
            {   
                LOG(3, "seq chg speedup + video frame sync unready(%d) VDP(%d)\n",_prB2rPrm[VDP2B2RID(ucVdpId)]->ucReady, ucVdpId);
                _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, FOREVER_MUTE, FALSE);
#ifdef CC_SUPPORT_TVE
                if (bIsScartOut)
                {
                    LOG(3, "seq chg speedup + video frame sync unready(%d) VDP(%d)\n",_prB2rPrm[VDP2B2RID(1-ucVdpId)]->ucReady, (1-ucVdpId));
                   	_vDrvVideoSetMute(MUTE_MODULE_DTV, (1-ucVdpId), FOREVER_MUTE, FALSE);
                    if(((1-ucVdpId >= 0) && (1 - ucVdpId <= 1)) && (_prB2rPrm[VDP2B2RID(1-ucVdpId)]->ucReady == 1))
                       {
                         LOG(3, "Set ucEarlyDisp =%d\n",_prVdpConf[1-ucVdpId]->ucEarlyDisp);
                        _prVdpConf[1-ucVdpId]->ucEarlyDisp = 1;
                       }
                }
#endif
            }
        }

        if (u4Status == VDP_B2R_NO_SIGNAL)
        {
            if (_prVdpConf[ucVdpId]->ucStatus != VDP_STATUS_NOSIGNAL)
            {
                u4IssueModeChange = 1;
            }

            _prVdpConf[ucVdpId]->ucStatus = VDP_STATUS_NOSIGNAL;

#ifdef CC_SRM_ON
            _prVdpConf[ucVdpId]->u4MpegHeight = 0;
            _prVdpConf[ucVdpId]->u4MpegWidth = 0;

#endif
        }
        else if (u4Status == VDP_B2R_START_PLAY)
        {
            if (_prVdpConf[ucVdpId]->ucStatus != VDP_STATUS_STABLE)
            {
                u4IssueModeChange = 1;
            }

            _prVdpConf[ucVdpId]->ucStatus = VDP_STATUS_STABLE;
        }
        else
//        if ((u4Status == VDP_B2R_OUTPUT_CHG) || (u4Status == VDP_B2R_RESOLUTION_NFY))
        {
            // RM interface change
            static UCHAR _aucLastPmxMode[VDP_NS];

            if (_prVdpConf[ucVdpId]->rOutInfo.ucPmxMode != _aucLastPmxMode[ucVdpId])
            {
                _aucLastPmxMode[ucVdpId] = _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode;

                if (_prVdpConf[ucVdpId]->ucStatus == VDP_STATUS_STABLE)
                {
                    u4IssueModeChange = 1;
                }
            }
            else if (_prVdpConf[ucVdpId]->ucStatus == VDP_STATUS_STABLE)
            {
                /* Code Review to use mode change instead of calling vApiVideoInputChangeNotify */
                u4IssueModeChange = 1;
            }
        }

        if (_aucThumbnailMode[ucVdpId] != 0)
        {
            if (u4IssueModeChange != 0)
            {
                u4IssueModeChange = 0;
                LOG(5, "Thumbnail mode, no mode change\n");
            }
        }

        if ((u4IssueModeChange != 0) && (!_prB2rPrm[VDP2B2RID(ucVdpId)]->ucSeamlessVDPMode))
        {
            LOG(3, "ucVdpId(%d) DTV Mode Change\n", ucVdpId);
#ifdef TIME_MEASUREMENT
            TMS_DIFF_EX(TMS_FLAG_CHG_CHL, TMS_CHL_CHE_TIME_DRV, "B2R Mode Change");
             TMS_DIFF_EX(TMS_FLAG_BOOT, TMS_COOL_BOOT_TIME, "B2R Mode Change");
#endif
			MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_DISPLAY_FIRST);
            _vDrvVideoSetMute(MUTE_MODULE_MODECHG, ucVdpId, 10, FALSE);

            vMpegModeChg(ucVdpId);
            vMpegModeDetDone(ucVdpId);
#ifdef CC_SUPPORT_NPTV_SEAMLESS 
            _VDP_NPTVSeamlessStatus(_prB2rPrm[VDP2B2RID(ucVdpId)], VDP_SMLS_PREPARE_WHILE_START_PLAY);
#endif
        }
    }
    else
    {
        _prVdpConf[ucVdpId]->u4MpegHeight = 0;
        _prVdpConf[ucVdpId]->u4MpegWidth = 0;

#ifdef CC_SRM_ON
#if 0 /* notification change */
        SRM_SendEvent(SRM_DRV_SCPOS, (SRM_SCPOS_EVENT_MPEG_SIZE + (UINT32)ucVdpId),
                      0, 0);
#endif
#endif
    }
#endif

#endif // __MODEL_slt__

    VDP_MUTEX_UNLOCK;
}

/**
 * DTV event notify
 *
 * @param ucVdpId specify the video plane id.
 * @param u4Arg1 specify arguement #1.
 * @param u4Arg1 specify arguement #2.
 * @param u4Arg1 specify arguement #3.
 * @return void
 */
void _VDP_EventNofify(UCHAR ucVdpId, UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3)
{
    if (u4Arg1 >= (UINT32)VDP_CB_FUNC_NS)
    {
        ASSERT(u4Arg1 < (UINT32)VDP_CB_FUNC_NS);
        return;
    }

    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    if(!_prVdpConf[ucVdpId])
    {
        return;
    }

    LOG(10, "EVENT NFY (%d %d %d %d)\n", ucVdpId, u4Arg1, u4Arg2, u4Arg3);

    if (VDP_CHECK_CB_FUNC_VERIFY(_rVdpCbFunc.au4CbFunc[u4Arg1], _rVdpCbFunc.au4CbFuncCRC[u4Arg1]))
    {
        if (u4Arg1 == (UINT32)VDP_CB_FUNC_PLAY_DONE_IND)
        {
            ((PLAYDONE_CFG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_PLAY_DONE_IND])(ucVdpId, u4Arg1, u4Arg2);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_AFD_IND)
        {
            ((AFD_CFG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_AFD_IND])(ucVdpId, u4Arg2, (FBM_ASPECT_RATIO_T*)u4Arg3);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_AFD_IND_2)
        {
            ((AFD_CFG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_AFD_IND_2])(ucVdpId, u4Arg2, (FBM_ASPECT_RATIO_T*)u4Arg3);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_PTS_IND)
        {
            ((PTS_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_PTS_IND])(ucVdpId, u4Arg2, u4Arg3);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_UNMUTE_IND)
        {
            ((UNMUTE_CFG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_UNMUTE_IND])(ucVdpId, (BOOL)u4Arg2);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_MM_COND_IND)
        {
            VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
#ifdef CC_53XX_SWDMX_V2
            ((VDP_MM_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_COND_IND])
            (ucVdpId, (VDP_COND_T)u4Arg2,(BOOL)(_prB2rPrm[VDP2B2RID(ucVdpId)]->fgFastForward),
             (UINT32)(u4Arg3));
#else
            ((VDP_MM_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_COND_IND])
            (ucVdpId, (VDP_COND_T)u4Arg2,(BOOL)(_prB2rPrm[VDP2B2RID(ucVdpId)]->fgFastForward));
#endif
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_LIPSYNC_IND)
        {
            ((VDP_LIPSYNC_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_LIPSYNC_IND])();
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_Get_PIC_TYPE)
        {
            VDEC_PICTYPE_CB_T rPicType;
            x_memset(&rPicType, 0, sizeof(VDEC_PICTYPE_CB_T));
            rPicType.u1EsId = (UINT8)u4Arg2;
            rPicType.u1PicType = (UINT8)u4Arg3;
            ((VDP_GET_PIC_TYPE_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_Get_PIC_TYPE])(&rPicType);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_MM_STEP_FIN_IND)
        {
            VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
#ifdef CC_53XX_SWDMX_V2
            ((VDP_STEP_FIN_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_STEP_FIN_IND])
            (ucVdpId, u4Arg2, u4Arg3,
             _prB2rPrm[VDP2B2RID(ucVdpId)]->u4AttachedSrcId);
#else
            ((VDP_STEP_FIN_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_STEP_FIN_IND])
            (ucVdpId, u4Arg2, u4Arg3);
#endif
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_MM_SEEK_FIN_IND)
        {
            UCHAR ucB2rId;

            ucB2rId = VDP2B2RID(ucVdpId);
            VERIFY_B2R_ID_RET_VOID(ucB2rId);
            
			MMPlaybackSpeedTestAddTimeStamp(TIME_STAMP_VDP_SEEK_DONE);
#ifdef CC_53XX_SWDMX_V2
            ((VDP_SEEK_FIN_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_SEEK_FIN_IND])(_prB2rPrm[ucB2rId]->u4AttachedSrcId, ucVdpId, u4Arg2, u4Arg3);
#else
            ((VDP_SEEK_FIN_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_MM_SEEK_FIN_IND])(ucVdpId, u4Arg2, u4Arg3);
#endif
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_REPEAT_DROP_IND)
        {
            UCHAR ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
            ((VDP_REPEAT_DROP_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_REPEAT_DROP_IND])(ucVdpId, u4Arg2, ucEsId);
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_LIPSYNC_OK_TO_NG_IND)
        {
            ((VDP_LIPSYNC_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_LIPSYNC_OK_TO_NG_IND])();
        }
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_LIPSYNC_NG_TO_OK_IND)
        {
            ((VDP_LIPSYNC_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_LIPSYNC_NG_TO_OK_IND])();
        }
		else if (u4Arg1 == (UINT32)VDP_CB_FUNC_RES_CHG_IND)
		{
			((VDP_RES_CHG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_RES_CHG_IND])(ucVdpId);
		}
        else if (u4Arg1 == (UINT32)VDP_CB_FUNC_BIT_RATE_CHG_IND)
		{
			((VDP_BIT_RATE_CHG_CB_FUNC)_rVdpCbFunc.au4CbFunc[VDP_CB_FUNC_BIT_RATE_CHG_IND])(ucVdpId);
		}
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        ASSERT(_rVdpCbFunc.au4CbFunc[u4Arg1] == 0);
    }
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

/**
 * VDP query function, show DTV status in CLI with debug level 1.
 *
 * @param void
 * @return void
 */
void VDP_PrintDtvInfo(void)
{
    UINT32 u4VdpId;

    for (u4VdpId = 0; u4VdpId < VDP_NS; u4VdpId++)
    {
        if (_prVdpConf[u4VdpId]->ucB2rId < B2R_NS)
        {
            LOG(3, "DTV INFO RESOLUTION(%d, %d) Aspect Ratio(%d) AFD(%d)\n",
                _prVdpConf[u4VdpId]->u4SrcWidth,
                _prVdpConf[u4VdpId]->u4SrcHeight,
                _prVdpConf[u4VdpId]->u4AspectRatio,
                _prVdpConf[u4VdpId]->u4Afd);
        }
    }
}

UINT32 VDP_SetPicInfo(UCHAR ucVdpId, VDP_SET_PIC_INFO_T rPicInfo)
{
    UCHAR ucFbgId;
    UCHAR ucFbId;
    UCHAR ucEsId;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;
    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];

    if (ucEsId >= MAX_ES_NS)
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);
    ucFbId = VDP_FrcGetLockFrameBuffer(VDP2B2RID(ucVdpId));

    if (ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

        if ((prFbmSeqHdr == NULL) || (prFbmPicHdr == NULL))
        {
            VDP_MUTEX_UNLOCK;
            return VDP_SET_ERROR;
        }
        else
        {
            prFbmPicHdr->u4TickNum = rPicInfo.u4TickNum;
            _VDP_SetCurrentTick(VDP2B2RID(ucVdpId), rPicInfo.u4TickNum);
        }
    }
    else
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;

    }
    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

UINT32 VDP_GetPicInfo(UINT8 ucVdpId, VDP_PIC_INFO_T* prPicInfo)
{
    UCHAR ucFbgId;
    UCHAR ucFbId;
    UCHAR ucEsId;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;
    UINT32 u4Pts;

    VERIFY_NULL(prPicInfo);
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];

    if (ucEsId >= MAX_ES_NS)
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);
    ucFbId = VDP_FrcGetLockFrameBuffer(VDP2B2RID(ucVdpId));

    if (ucFbgId != FBM_FBG_ID_UNKNOWN)
    {
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

        if ((prFbmSeqHdr == NULL) || (prFbmPicHdr == NULL))
        {
            VDP_MUTEX_UNLOCK;
            return VDP_SET_ERROR;
        }
        else
        {
            prPicInfo->ucHours = prFbmSeqHdr->ucHours;
            prPicInfo->ucMinutes = prFbmSeqHdr->ucMinutes;
            prPicInfo->ucSeconds = prFbmSeqHdr->ucSeconds;
            prPicInfo->ucPictures = prFbmSeqHdr->ucPictures;
            prPicInfo->u4TotlaTimeOffset = prFbmPicHdr->u4TotlaTimeOffset;
            prPicInfo->u4TickNum = prFbmPicHdr->u4TickNum;

            prPicInfo->u8Offset = prFbmPicHdr->u8Offset;
            prPicInfo->u8OffsetDisp = prFbmPicHdr->u8OffsetDisp;
            //prPicInfo->u4Pts = prFbmPicHdr->u4PTS;
            u4Pts = _VDP_GetCurrentPts(VDP2B2RID(ucVdpId));

            prPicInfo->u4Pts = u4Pts;

            if (prFbmPicHdr->ucTrueZeroPTS == FBM_USE_LAST_PTS_IF_ZERO)
            {
                if (u4Pts == 0)
                {
                    prPicInfo->u4Pts = _VDP_GetLastPts(VDP2B2RID(ucVdpId));
                }
            }
        }
    }
    else
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;

    }
    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/**
 * Get mute status of DTV playback
 *
 * @param ucVdpId specify the video plane id.
 * @return TRUE for mute, FALSE for not mute
 */
BOOL VDP_GetUnMuteStatus(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);
    return VDP_FrcGetUnMuteStatus(VDP2B2RID(ucVdpId));
}

/**
 * Video Image Module : Get information
 *
 * @param void
 * @return pointer of Video Image Module Capability Infomation
 */
VDP_IMAGE_INFO_T* VDP_Image_GetInfo(void)
{
    return _VDP_Image_GetInfo();
}

/**
 * Video Image Module : Connect
 *
 * @param u4VdpId: Video plane connected to
 * @return VDP_SET_OK or VDP_SET_ERROR
 */
UINT32 VDP_Image_Connect(UINT32 u4VdpId)
{
    UINT32 u4ReturnValue;
    UINT32 u4Idx;

    VERIFY_VDP_ID(u4VdpId, VDP_SET_ERROR);
    
    VDP_MUTEX_LOCK;

    for (u4Idx = 0; u4Idx < VDP_NS; u4Idx++)
    {
        _aucImageConnected[u4Idx] = 0;
    }

    u4ReturnValue = _VDP_Image_Connect(u4VdpId);

    if (u4ReturnValue == VDP_SET_OK)
    {
        _aucImageConnected[u4VdpId] = 1;
    }

    VDP_MUTEX_UNLOCK;

    return u4ReturnValue;
}

/**
 * Video Image Module : Disconnect
 *
 * @param void
 * @return VDP_SET_OK or VDP_SET_ERROR
 */
UINT32 VDP_Image_Disconnect(void)
{
    UINT32 u4VdpId;

    for (u4VdpId = 0; u4VdpId < VDP_NS; u4VdpId++)
    {
        _aucImageConnected[u4VdpId] = 0;
    }

    return _VDP_Image_Disconnect();
}

/**
 * Video Image Module : Mute on/off
 *
 * @param u4OnOff: Mute on/off
 * @return VDP_SET_OK or VDP_SET_ERROR
 */
UINT32 VDP_Image_Mute(UINT32 u4OnOff)
{
    return _VDP_Image_Mute(u4OnOff);
}

/**
 * Video Image Module : Show Frame Buffer
 *
 * @param prInfo: Frame Buffer Info
 * @return VDP_SET_OK or VDP_SET_ERROR
 */
UINT32 VDP_Image_Show(VDP_IMAGE_ARG_T* prInfo)
{
    return _VDP_Image_Show(prInfo);
}

/**
 * Video Image Module : Get Frame Buffer Address
 *
 * @param u4FrameBufferId: Frame Buffer id
 * @param u4YAddr: pointer to store Y address
 * @param u4CAddr: pointer to store C address
 * @return VDP_SET_OK or VDP_SET_ERROR
 */
UINT32 VDP_Image_GetFrameBuffer(UINT32 u4FrameBufferId, UINT32* u4YAddr, UINT32* u4CAddr)
{
    return _VDP_Image_GetFrameBuffer(u4FrameBufferId, u4YAddr, u4CAddr);
}

/**
 * VDP connect to video decoder.
 *
 * @param ucVdpId specify the video plane id.
 * @param ucEsId specify video decoder id.
 * @param ucPort always 0.
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
UINT32 VDP_SetInput(UCHAR ucVdpId, UCHAR ucEsId, UCHAR ucPort)
{
#if 0
    UINT32 u4FbgId;
    UINT32 u4VdpIdx;
    UINT32 u4PortIdx;
    UINT32 u4PortFbgId;
#endif

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    if (ucPort >= VDP_MAX_INPORT_NS)
    {
        return VDP_SET_ERROR;
    }

    if (_prVdpConf[ucVdpId]->ucInputPort[ucPort] == ucEsId)
    {
        return VDP_SET_OK;
    }

    VDP_MUTEX_LOCK;

    //workaround for 3D
#if 0
    _prVdpConf[ucVdpId]->ucInputPort[ucPort] = 0;//ucEsId;
#else
    _prVdpConf[ucVdpId]->ucInputPort[ucPort] = ucEsId;
#endif

#if 0 /* Main and Sub are with DTV for SCART out/TVE application */
    if (ucEsId >= MAX_ES_NS)
    {
        u4FbgId = FBM_FBG_ID_UNKNOWN;
    }
    else
    {
        u4FbgId = B2R_GetEs2Fbg(ucEsId);
    }

    // protection for dual TV
    if (u4FbgId != FBM_FBG_ID_UNKNOWN)
    {
        for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
        {
            if (u4VdpIdx != ucVdpId)
            {
                for (u4PortIdx = 0; u4PortIdx <VDP_MAX_INPORT_NS; u4PortIdx++)
                {
                    if (_arVdpConf[u4VdpIdx].ucInputPort[u4PortIdx] >= MAX_ES_NS)
                    {
                        u4PortFbgId = FBM_FBG_ID_UNKNOWN;
                    }
                    else
                    {
                        u4PortFbgId = B2R_GetEs2Fbg(_arVdpConf[u4VdpIdx].ucInputPort[u4PortIdx]);
                    }

                    if (u4PortFbgId == u4FbgId)
                    {
                        LOG(1, "Dual TV is NOT ready! (%d, %d, %d) \n", u4VdpIdx, u4PortIdx, u4FbgId);
                        LOG(1, "Port (%d) (%d)\n"
                            , _arVdpConf[0].ucInputPort[0]
                            , _arVdpConf[1].ucInputPort[0]);
                        LOG(1, "ES (%d, %d, %d %d, %d, %d)\n"
                            , B2R_GetEs2Fbg(0)
                            , B2R_GetEs2Fbg(1)
                            , B2R_GetEs2Fbg(2)
                            , B2R_GetEs2Fbg(3)
                            , B2R_GetEs2Fbg(4)
                            , B2R_GetEs2Fbg(5));
                        //ASSERT(0);
                        _arVdpConf[ucVdpId].ucInputPort[ucPort] = MAX_ES_NS;

                    }
                }
            }
        }
    }
#endif
    VDP_VsyncSendCmd(ucVdpId, VDP_CMD_SET_INPUT);
#ifdef CC_53XX_SWDMX_V2
    _VdpCheckFbgReady(0xFF, 0xFF);
#else
    _VdpCheckFbgReady();
#endif

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/**
 * VDP switch B2R to corresponding video plane
 *
 * @param ucVdpId specify the video plane id.
 * @param ucB2rId specify B2R id (0 only)
 * @return void
 */
void VDP_B2rSwitch(UCHAR ucVdpId, UCHAR ucB2rId)
{
    // if ucB2rId = B2R_NS, it means that video plane switch to non-DTV video input
    VERIFY_VDP_ID_RET_VOID(ucVdpId);

    if (ucVdpId < VDP_NS)
    {
#if 0//ndef CC_SUPPORT_TVE
        UINT32 u4VdpIdx;
#endif

#ifdef CC_SCPOS_EN
#if !defined(CC_MT5398) && !defined(CC_MT5880) && !defined(CC_MT5860) && !defined(CC_MT5881) && !defined(CC_MT5399) && !defined(CC_MT5890)
        vDrvVideoFixedPreScaler(ucVdpId, CC_VDP_FIXED_PRESCALER, 1);
#endif
#endif
        _prVdpConf[ucVdpId]->ucB2rId = ucB2rId;
        LOG(1, "VDP(%d) connect with B2R(%d)\n", ucVdpId, ucB2rId);
#if 0//ndef CC_SUPPORT_TVE
        // remove this b2r module from all other vdp
        for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
        {
            if ((_prVdpConf[u4VdpIdx]->ucB2rId == ucB2rId) &&
                    (u4VdpIdx != ucVdpId))
            {
                _prVdpConf[u4VdpIdx]->ucB2rId = B2R_NS;
            }
        }
#else
        if ( (_prVdpConf[ucVdpId]->ucB2rId == B2R_1) &&
                (_prVdpConf[VDP_1]->ucB2rId == _prVdpConf[VDP_2]->ucB2rId))
        {
            _ucSameB2rVdp = 1-ucVdpId;
        }
        else
        {
            _ucSameB2rVdp = VDP_NS;
        }
#endif

        if (ucB2rId < B2R_NS)
        {
#ifdef CC_SCPOS_EN
            vMpegInitB2rConf();
#endif
        }
        else
        {

#ifdef CC_SCPOS_EN
            // When video plane connect to non-DTV, unmute DTV mask
            _vDrvVideoSetMute(MUTE_MODULE_DTV, ucVdpId, 0, FALSE);
            LOG(3, "DTV Unmute B2R Switch VDP(%d)\n", ucVdpId);

#endif

#if 1
#ifdef CC_SCPOS_EN
            vMpegInitB2rConf();
#endif
#endif
        }
        VDP_HalB2rSwitch(ucVdpId, ucB2rId);
        if (ucB2rId == B2R_NS)
        {
            _prVdpConf[ucVdpId]->u4Afd = ACT_FMT_DEFAULT;
        }

        _B2rSetEnable(ucVdpId);
    }
}

/**
 * VDP transfer ES id to VDP id
 *
 * @param ucEsId specify the video decoder id.
 * @return VDP id
 */
UINT32 VDP_Es2Vdp(UCHAR ucEsId)
{
    UINT32 u4VdpIdx;
    UINT32 u4PortIdx;
    UINT32 u4Return = VDP_1;

    ASSERT(ucEsId < MAX_ES_NS);

    VDP_MUTEX_LOCK;

    for (u4VdpIdx = 0; u4VdpIdx < VDP_NS; u4VdpIdx++)
    {
        for (u4PortIdx = 0; u4PortIdx < VDP_MAX_INPORT_NS; u4PortIdx++)
        {
            if (_prVdpConf[u4VdpIdx]->ucInputPort[u4PortIdx] == ucEsId)
            {
                u4Return = u4VdpIdx;
                VDP_MUTEX_UNLOCK;
                return u4Return;
            }
        }
    }

    VDP_MUTEX_UNLOCK;
    return u4Return;
}

/**
 * VDP transfer VDP id to Es id
 *
 * @param specify the VDP id .
 * @return ucEsId (video decoder id)
 */
UINT32 VDP_Vdp2Es(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, MAX_ES_NS);
    return _prVdpConf[ucVdpId]->ucInputPort[0];
}

/**
 * VDP transfer Stc id to B2R id
 *
 * @param ucStcId specify the STC id.
 * @return B2R id that reference to the STC
 */
UCHAR VDP_Stc2B2r(UCHAR ucStcId)
{
    UCHAR ucB2rId;
    UCHAR ucFbgSyncMode;
    UCHAR ucFbgStcId;

    VDP_MUTEX_LOCK;

    for (ucB2rId = B2R_1; ucB2rId < B2R_NS; ucB2rId++)
    {
        if (_prB2rPrm[ucB2rId]->ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            FBM_GetSyncStc(_prB2rPrm[ucB2rId]->ucFbgId, &ucFbgSyncMode, &ucFbgStcId);
            if (ucFbgStcId != STC_SRC_NS && ucFbgStcId == ucStcId)
            {
                VDP_MUTEX_UNLOCK;
                return ucB2rId;
            }
        }
    }

    VDP_MUTEX_UNLOCK;
    return B2R_NS;
}

VOID VDP_SetEnableLog(UCHAR ucVdpId, UINT32 u4Arg1, UINT32 u4Arg2)
{
    UNUSED(ucVdpId);
    UNUSED(u4Arg1);
    UNUSED(u4Arg2);

    VERIFY_VDP_ID_RET_VOID(ucVdpId);

    _VDP_SetEnableLog(ucVdpId,(BOOL)u4Arg1,(BOOL)u4Arg2);
}

BOOL VDP_GstEnablGstLog(UCHAR ucVdpId)
{
    UNUSED(ucVdpId);
    return _VDP_GetEnableGstLog();
}

/**
 * VDP send test pattern (for module test)
 *
 * @param void
 * @return void
 */
void VDP_Pattern(UCHAR ucVdpId, UINT32 u4Arg1, UINT32 u4Arg2, UINT32 u4Arg3)
{
    UNUSED(ucVdpId);
    UNUSED(u4Arg1);
    UNUSED(u4Arg2);
    UNUSED(u4Arg3);

    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));

#if 0
    VDP_IMAGE_ARG_T rInfo;

    VDP_SetEnable(VDP_1, 1);

    VDP_SetMode(VDP_1, VDP_MODE_NORMAL);

    x_thread_delay(500);

    VDP_Image_Connect(VDP_1);

    VDP_Image_Mute(0);

    rInfo.u4ColorMode = VDP_IMAGE_420_MODE;
    rInfo.u4FrameBufferId = 0;
    rInfo.u4Width = 1920;
    rInfo.u4Height = 1080;
    rInfo.u4Pitch = 1920;

    x_thread_delay(500);

    VDP_Image_Show(&rInfo);

#else
    /* Change to B2R pattern directly */
    if (u4Arg1 == 99)
    {
#ifdef CC_SECOND_B2R_SUPPORT
        _VDP_HalStatusTest(VDP2B2RID(ucVdpId));
#else
    _VDP_HalStatusTest();
#endif
        return;
    }
    else if (u4Arg1 == 98)
    {
#ifdef CC_SECOND_B2R_SUPPORT
        _VDP_HalDisablePattern(VDP2B2RID(ucVdpId));
#else
    _VDP_HalDisablePattern();
#endif
        return;
    }

    _VDP_VsyncReset(VDP2B2RID(ucVdpId));

    _prVdpConf[ucVdpId]->rSrcRegion.u4X = 0;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Y = 0;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Width = VDP_MAX_REGION_WIDTH;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Height = VDP_MAX_REGION_HEIGHT;

    switch (u4Arg1)
    {
    case VDP_B2R_MODE_480P_60:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_480P_60;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 480;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 858;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 525;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_027;
        LOG(1, "Pattern : VDP_B2R_MODE_480P_60\n");

        break;

    case VDP_B2R_MODE_576I_50:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_576I_50;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 576;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 864;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 625;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 0;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_000;
        LOG(1, "Pattern : VDP_B2R_MODE_576I_50\n");
        break;

    case VDP_B2R_MODE_576P_50:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_576P_50;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 50;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 576;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 864;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 625;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_000;
        LOG(1, "Pattern : VDP_B2R_MODE_576P_50\n");
        break;

    case VDP_B2R_MODE_720P_60:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_720P_60;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 720;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 1650;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 750;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_576I_50\n");
        break;

    case VDP_B2R_MODE_1080I_60:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080I_60;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2200;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 0;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_148_500;
        LOG(1, "Pattern : VDP_B2R_MODE_1080I_60\n");
        break;

    case VDP_B2R_MODE_1080P_60:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080P_60;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2200;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_148_500;
        LOG(1, "Pattern : VDP_B2R_MODE_1080P_60\n");
        break;

    case VDP_B2R_MODE_480P_24:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_480P_24;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 24;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 480;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 858;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 525;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 2;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_027;
        LOG(1, "Pattern : VDP_B2R_MODE_480P_24\n");
        break;

    case VDP_B2R_MODE_480P_30:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_480P_30;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 30;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 480;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 858;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 525;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_027;
        LOG(1, "Pattern : VDP_B2R_MODE_480P_30\n");
        break;

    case VDP_B2R_MODE_576P_25:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_576P_25;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 25;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 576;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 864;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 625;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_000;
        LOG(1, "Pattern : VDP_B2R_MODE_576P_25\n");
        break;

    case VDP_B2R_MODE_720P_24:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_720P_24;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 24;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 720;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 1650;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 750;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 2;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_720P_24\n");
        break;

    case VDP_B2R_MODE_720P_25:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_720P_25;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 25;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 720;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 1980;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 750;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_720P_25\n");
        break;

    case VDP_B2R_MODE_720P_30:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_720P_30;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 30;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 720;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 1650;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 750;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_720P_30\n");
        break;

    case VDP_B2R_MODE_720P_50:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_720P_50;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 50;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1280;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 720;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 1980;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 750;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_720P_50\n");
        break;

    case VDP_B2R_MODE_1080P_24:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080P_24;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 24;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2750;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_1080P_24\n");
        break;

    case VDP_B2R_MODE_1080P_25:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080P_25;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 25;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2640;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_1080P_25\n");
        break;

    case VDP_B2R_MODE_1080P_30:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080P_30;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 30;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2200;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_74_250;
        LOG(1, "Pattern : VDP_B2R_MODE_1080P_30\n");
        break;

    case VDP_B2R_MODE_1080I_50:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080I_50;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 50;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2640;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 0;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_148_500;
        LOG(1, "Pattern : VDP_B2R_MODE_1080I_50\n");
        break;

    case VDP_B2R_MODE_1080P_50:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_1080P_50;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 50;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 1920;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 1080;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 2640;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 1125;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 1;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 0;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_148_500;
        LOG(1, "Pattern : VDP_B2R_MODE_1080P_50\n");
        break;

    default:
    case VDP_B2R_MODE_480I_60:
        _prVdpConf[ucVdpId]->rOutInfo.ucPmxMode = VDP_B2R_MODE_480I_60;

        _prVdpConf[ucVdpId]->rOutInfo.ucFrameRate = 60;
        _prVdpConf[ucVdpId]->u4SrcLineSize = 768;
        _prVdpConf[ucVdpId]->rOutInfo.u4Width = 720;
        _prVdpConf[ucVdpId]->rOutInfo.u4Height = 480;

        _prVdpConf[ucVdpId]->rOutInfo.u4TotalWidth = 858;
        _prVdpConf[ucVdpId]->rOutInfo.u4TotalHeight = 525;
        _prVdpConf[ucVdpId]->rOutInfo.ucPrg = 0;
        _prVdpConf[ucVdpId]->rOutInfo.ucDoubleClk = 1;
        _prVdpConf[ucVdpId]->rOutInfo.u4ClkMode = VDP_B2R_CLK_27_027;
        LOG(1, "Pattern : VDP_B2R_MODE_480I_60\n");
        break;
    }


    _prVdpConf[ucVdpId]->u4SrcWidth = _prVdpConf[ucVdpId]->rOutInfo.u4Width;
    _prVdpConf[ucVdpId]->u4SrcHeight = _prVdpConf[ucVdpId]->rOutInfo.u4Height;

    _prVdpConf[ucVdpId]->u4MpegWidth = _prVdpConf[ucVdpId]->rOutInfo.u4Width;
    _prVdpConf[ucVdpId]->u4MpegHeight = _prVdpConf[ucVdpId]->rOutInfo.u4Height;

    _VDP_VsyncSendCmd(VDP2B2RID(ucVdpId), VDP_CMD_SET_OUTR);

    _prVdpConf[ucVdpId]->ucMode = VDP_MODE_BG;
    _VDP_VsyncSendCmd(VDP2B2RID(ucVdpId), VDP_CMD_SET_MODE);

    _prVdpConf[ucVdpId]->ucVdpEnable = 1;
    _prVdpConf[ucVdpId]->ucEnable = 1;
    _VDP_VsyncSendCmd(VDP2B2RID(ucVdpId), VDP_CMD_SET_ENABLE);

#ifdef CC_SCPOS_EN
    _prVdpConf[ucVdpId]->rSrcRegion.u4X = 0;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Y = 0;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Width = VDP_MAX_REGION_WIDTH;
    _prVdpConf[ucVdpId]->rSrcRegion.u4Height = VDP_MAX_REGION_HEIGHT;

    _prVdpConf[ucVdpId]->rOutRegion.u4X = 0;
    _prVdpConf[ucVdpId]->rOutRegion.u4Y = 0;
    _prVdpConf[ucVdpId]->rOutRegion.u4Width = VDP_MAX_REGION_WIDTH;
    _prVdpConf[ucVdpId]->rOutRegion.u4Height = VDP_MAX_REGION_HEIGHT;

#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890))
    if (bApiVideoSetSrcRegion(ucVdpId, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT) != 1)
    {
    }
#else
    if (bApiVideoSetSrcPosSize(ucVdpId, 0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT) != 1)
    {
    }
#endif

#endif

    _VDP_StatusNotify(ucVdpId, VDP_B2R_START_PLAY);

    x_thread_delay(1000);
#ifdef CC_SECOND_B2R_SUPPORT
    _VDP_HalStatusTest(ucVdpId);
#else
    _VDP_HalStatusTest();
#endif

#endif
}

/**
 * VDP set video plane with Pause MM feature.
 *
 * @param ucVdpId specify the video plane id.
 * @param ucPause value 0:disable, 1:Pause MM feature enable.
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
UINT32 VDP_SetPauseMm(UINT8 ucVdpId, BOOL fgPause)
{

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VDP_MUTEX_LOCK;

#if 0
    if (_prB2rPrm[ucVdpId]->fgPauseMM != ucPause)
    {
        _prB2rPrm[ucVdpId]->fgPauseMM = ucPause;
    }
#else
    _VDP_SetPauseMM(VDP2B2RID(ucVdpId),fgPause);
#endif
    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/**
 * VDP set video plane with Pause MM feature.
 *
 * @param ucVdpId specify the video plane id.
 * @param ucPause value 0:disable, 1:Pause MM feature enable.
 * @return VDP_SET_ERROR or VDP_SET_OK
 */
UINT32 VDP_SetB2rNoLock(UINT8 ucVdpId, BOOL fgNoLock)
{

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VDP_MUTEX_LOCK;
    _VDP_SetB2rNoLock(VDP2B2RID(ucVdpId),fgNoLock);
    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

UINT32 VDP_SetSpeed(UINT8 ucVdpId, STC_SPEED_TYPE_T eSpeed)
{
    UINT32 u4Speed;
    BOOL fgDiv10 = 0;
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VDP_MUTEX_LOCK;

    switch (eSpeed)
    {
    case STC_SPEED_TYPE_FORWARD_2X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_2X;
        break;
    case STC_SPEED_TYPE_FORWARD_3X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_3X;
        break;
    case STC_SPEED_TYPE_FORWARD_4X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_4X;
        break;
    case STC_SPEED_TYPE_FORWARD_8X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_8X;
        break;
    case STC_SPEED_TYPE_FORWARD_16X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_16X;
        break;
    case STC_SPEED_TYPE_FORWARD_32X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_32X;
        break;
    case STC_SPEED_TYPE_FORWARD_64X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_64X;
        break;
    case STC_SPEED_TYPE_FORWARD_128X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_128X;
        break;
    case STC_SPEED_TYPE_FORWARD_256X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_256X;
        break;
    case STC_SPEED_TYPE_FORWARD_512X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_512X;
        break;
    case STC_SPEED_TYPE_FORWARD_1024X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1024X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_2X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_2X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_3X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_3X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_4X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_4X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_8X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_8X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_16X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_16X;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DIV_32X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DIV_32X;
        break;
    case STC_SPEED_TYPE_FORWARD_0_DOT_6X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_0_DOT_6X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_0_DOT_7X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_0_DOT_7X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_0_DOT_8X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_0_DOT_8X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_0_DOT_9X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_0_DOT_9X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DOT_1X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DOT_1X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DOT_2X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DOT_2X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DOT_3X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DOT_3X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DOT_4X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DOT_4X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1_DOT_5X:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1_DOT_5X;
        fgDiv10 = TRUE;
        break;
    case STC_SPEED_TYPE_FORWARD_1X:
    default:
        u4Speed =  (UINT32)B2R_SPEED_TYPE_FORWARD_1X;
        break;
    }

    _VDP_HalStcSpeed(VDP2B2RID(ucVdpId),u4Speed, fgDiv10);
    _VDP_FrcStcSpeed(VDP2B2RID(ucVdpId),eSpeed);

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;

}

void VDP_SetB2rUpdateStc(UINT8 ucVdpId,BOOL fgEnable)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    _VDP_HalEnB2rUpdateStc(VDP2B2RID(ucVdpId),fgEnable);
}
void VDP_SetRenderRegion(UINT8 ucVdpId, VDP_REGION_T *prRendRegion)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    _VDP_HalSetRenderRegion(VDP2B2RID(ucVdpId), prRendRegion);
}

BOOL VDP_GetB2rUpdateStc(UINT8 ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);
    return _VDP_HalGetUpdateStc(VDP2B2RID(ucVdpId));
}

BOOL VDP_Is4To1024XSpeed(UINT32 u4Speed)
{
#if 0
    if ( ( _VDP_GetFrcStcSpeed(u1B2rId) >= STC_SPEED_TYPE_REWIND_4X ) &&
            ( _VDP_GetFrcStcSpeed(u1B2rId) <= STC_SPEED_TYPE_REWIND_1024X ) )
    {
        return TRUE;
    }
#endif

    if ( (u4Speed >= B2R_SPEED_TYPE_FORWARD_4X )&&
            (u4Speed <= B2R_SPEED_TYPE_FORWARD_1024X ) )
    {
        return TRUE;
    }

    return FALSE;
}


void VDP_SetB2rStc(UINT8 ucVdpId,UINT32 u4B2rStc)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    _VDP_HalSetB2rStc(VDP2B2RID(ucVdpId),u4B2rStc);
}

UINT32 VDP_GetB2rStc(UINT8 ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    return _VDP_HalGetB2rStc(VDP2B2RID(ucVdpId));
}

UINT32 VDP_SetFrmCrop(UCHAR ucVdpId, UINT32 u4FrmCropTop, UINT32 u4FrmCropBtm)
{

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    _prVdpConf[ucVdpId]->u4FrmCropTop = u4FrmCropTop;
    _prVdpConf[ucVdpId]->u4FrmCropBtm = u4FrmCropBtm;

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

UINT32 VDP_GetPendingFBPts(UCHAR ucVdpId)
{
    UCHAR ucFbgId;
    UCHAR ucPendingFbId;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    
    VERIFY_VDP_ID(ucVdpId, (UINT32)-1);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), (UINT32)-1);

    ucFbgId = _VDP_GetFbg(VDP2B2RID(ucVdpId));
    ucPendingFbId = VDP_GetPendingFB(VDP2B2RID(ucVdpId), ucFbgId);
    
    if ((ucFbgId != FBM_FBG_ID_UNKNOWN) && (ucPendingFbId != FBM_FB_ID_UNKNOWN))
    {
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucPendingFbId);
    
        if (prFbmPicHdr != NULL)
        {
            return prFbmPicHdr->u4PTS;
        }
    }
    return (UINT32)-1;
}


UINT32 VDP_GetPts(UCHAR ucVdpId)
{
    UINT32 u4Pts;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    u4Pts = _VDP_GetCurrentPts(VDP2B2RID(ucVdpId));

    VDP_MUTEX_UNLOCK;

    return u4Pts;
}

UINT32 VDP_GetLockFrameBufferAddr(UCHAR ucVdpId, VDP_CAPTURE_INTO_T* prCapInfo)
{

    UCHAR ucEsId;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];

    if (ucEsId >= MAX_ES_NS)
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);

    ucFbId = VDP_FrcGetLockFrameBuffer(VDP2B2RID(ucVdpId));

    VDP_MUTEX_UNLOCK;

    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &(prCapInfo->u4AddrY), &(prCapInfo->u4AddrC));

    FBM_GetFrameBufferSize(ucFbgId, &(prCapInfo->u4MemHSize), &(prCapInfo->u4MemVSize));

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

    if (prFbmSeqHdr == NULL)
    {
        return VDP_SET_ERROR;
    }
    else
    {
        /* Thumbnail usage: Image resizer will use this as pitch. */
        if (prFbmPicHdr && prFbmSeqHdr->fgResizeSmallPic)
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmPicHdr->u4PicWidthPitch;

            prCapInfo->u4HSize = (UINT32)prFbmPicHdr->u4PicWidth;
            prCapInfo->u4VSize = (UINT32)prFbmPicHdr->u4PicHeight;
        }
        else
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmSeqHdr->u2LineSize;

            prCapInfo->u4HSize = (UINT32)prFbmSeqHdr->u2OrgHSize;
            prCapInfo->u4VSize = (UINT32)prFbmSeqHdr->u2OrgVSize;
        }

        prCapInfo->u4RRMode = (UINT32)prFbmSeqHdr->fgRRYEn;
    }

    return VDP_SET_OK;

}


UINT32 VDP_GetDisplayFrameBufferAddr(UCHAR ucVdpId, VDP_CAPTURE_INTO_T* prCapInfo)
{
    UINT32 u4FbNum;
    UINT32 u4Idx;
    UCHAR ucEsId;
    UCHAR ucFbgId;
    UCHAR ucFbId;
    FBM_SEQ_HDR_T* prFbmSeqHdr;
    FBM_PIC_HDR_T* prFbmPicHdr;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];

    if (ucEsId >= MAX_ES_NS)
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);
    ucFbId = 0xFF;
    u4FbNum = (UINT32)FBM_GetFrameBufferNs(ucFbgId);
    for(u4Idx = 0; u4Idx < u4FbNum; u4Idx++)
    {
        if(FBM_GetFrameBufferStatus(ucFbgId, u4Idx)
            == FBM_FB_STATUS_DISPLAYQ)  //already freeze at mpv_mw.c
        {
            ucFbId = (UCHAR)u4Idx;
            break;
        }
    }

    if (ucFbId == 0xFF)
    {
        VDP_MUTEX_UNLOCK;
        return VDP_SET_ERROR;
    }
    VDP_MUTEX_UNLOCK;

    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &(prCapInfo->u4AddrY), &(prCapInfo->u4AddrC));

    FBM_GetFrameBufferSize(ucFbgId, &(prCapInfo->u4MemHSize), &(prCapInfo->u4MemVSize));

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);

    if (prFbmSeqHdr == NULL)
    {
        return VDP_SET_ERROR;
    }
    else
    {
        /* Thumbnail usage: Image resizer will use this as pitch. */
        if (prFbmPicHdr && prFbmSeqHdr->fgResizeSmallPic)
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmPicHdr->u4PicWidthPitch;

            prCapInfo->u4HSize = (UINT32)prFbmPicHdr->u4PicWidth;
            prCapInfo->u4VSize = (UINT32)prFbmPicHdr->u4PicHeight;
        }
        else
        {
            prCapInfo->u4MemHSize = (UINT32)prFbmSeqHdr->u2LineSize;

            prCapInfo->u4HSize = (UINT32)prFbmSeqHdr->u2OrgHSize;
            prCapInfo->u4VSize = (UINT32)prFbmSeqHdr->u2OrgVSize;
        }

        prCapInfo->u4RRMode = (UINT32)prFbmSeqHdr->fgRRYEn;
    }

    return VDP_SET_OK;

}


BOOL VDP_GetFilePosition(UCHAR ucVdpId,
                         INT32 *pi4TempRef, UINT16 *pu2DecodingOrder,
                         UINT64 *pu8Offset, UINT64 *pu8OffsetI)
{
    UINT32 u4Ret;
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);

    VDP_MUTEX_LOCK;

    u4Ret = _VDP_GetFilePosition(VDP2B2RID(ucVdpId), pi4TempRef, pu2DecodingOrder,
                                 pu8Offset, pu8OffsetI);

    VDP_MUTEX_UNLOCK;

    return (u4Ret == 0) ? FALSE : TRUE;
}

BOOL VDP_GetPosInfo(UINT8 ucVdpId, VDP_POS_INTO_T *prPosInfo)
{
    UINT32 u4Ret;
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);

    VDP_MUTEX_LOCK;

    u4Ret = _VDP_GetPosInfo(VDP2B2RID(ucVdpId), prPosInfo);

    VDP_MUTEX_UNLOCK;

    return (u4Ret == 0) ? FALSE : TRUE;
}

BOOL VDP_SetABInfo(UCHAR ucVdpId, VDP_AB_INTO_T *prAB)
{
    UINT32 u4Ret;
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);

    VDP_MUTEX_LOCK;

    u4Ret = _VDP_SetABInfo(VDP2B2RID(ucVdpId), prAB);

    VDP_MUTEX_UNLOCK;

    return (u4Ret == 0) ? FALSE : TRUE;
}

UCHAR VDP_GetPendingFB(UCHAR ucVdpId, UCHAR ucFbgId)
{
    BOOL fgSubFbg = FALSE;

    VERIFY_VDP_ID(ucVdpId, FBM_FB_ID_UNKNOWN);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FBM_FB_ID_UNKNOWN);

    if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucFbgId == ucFbgId)
    {
        fgSubFbg = FALSE;
    }
#ifdef CC_3D_MM_DS_SUPPORT
    else if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucSubFbgId == ucFbgId)
    {
        fgSubFbg = TRUE;
    }
#endif
    else
    {
    	return FBM_FB_ID_UNKNOWN;
    }

    return _VDP_GetPendingFB(VDP2B2RID(ucVdpId), fgSubFbg);
}

void VDP_CleanPendingFB(UCHAR ucVdpId, UCHAR ucFbgId)
{
    BOOL fgSubFbg = FALSE;

    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));

    if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucFbgId == ucFbgId)
    {
        fgSubFbg = FALSE;
    }
#ifdef CC_3D_MM_DS_SUPPORT
    else if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucSubFbgId == ucFbgId)
    {
        fgSubFbg = TRUE;
    }
#endif
    else
    {
        return;
    }

    _VDP_CleanPendingFB(VDP2B2RID(ucVdpId), fgSubFbg);
}

void VDP_SetReleaseDispQ(UCHAR ucVdpId, UCHAR ucFbgId)
{
    BOOL fgSubFbg = FALSE;

    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    
    if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucFbgId == ucFbgId)
    {
        fgSubFbg = FALSE;
    }
#ifdef CC_3D_MM_DS_SUPPORT
    else if (_prB2rPrm[VDP2B2RID(ucVdpId)]->ucSubFbgId == ucFbgId)
    {
        fgSubFbg = TRUE;
    }
#endif
    else
    {
        return;
    }

    _VDP_SetReleaseDispQ(VDP2B2RID(ucVdpId), fgSubFbg);
}


void VDP_VsyncMutexLock(void)
{
    _VDP_VsyncMutexLock();
}

void VDP_VsyncMutexUnlock(void)
{
    _VDP_VsyncMutexUnlock();
}

/* Trick Play at 20081001 */
#ifdef CC_53XX_SWDMX_V2
UINT32 VDP_SetTrickMode(UINT8 ucVdpId,UCHAR ucTrick)
{
#else
UINT32 VDP_SetTrickMode(UCHAR ucTrick)
{
    UINT8 ucVdpId = VDP_1;
#endif
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VDP_MUTEX_LOCK;

    VDP_HalSetTrickMode(VDP2B2RID(ucVdpId),ucTrick);

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/* Step Forward at 20081030 */
UINT32 VDP_SetStepForward(UINT8 ucVdpId, UCHAR ucSet)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    VDP_HalSetStepForward(VDP2B2RID(ucVdpId),ucSet);

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/* Step Forward at 20081030 */
UCHAR VDP_GetStepForward(UINT8 ucVdpId)
{
    UCHAR ucResult;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    ucResult = VDP_HalGetStepForward(VDP2B2RID(ucVdpId));

    VDP_MUTEX_UNLOCK;

    return ucResult;
}

/* Slow Forward at 20090506 */
UINT32 VDP_SetSlowForward(UINT8 ucVdpId, UCHAR ucSet)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    VDP_HalSetSlowForward(VDP2B2RID(ucVdpId),ucSet);

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

/* Slow Forward at 20090506 */
UCHAR VDP_GetSlowForward(UINT8 ucVdpId)
{
    UCHAR ucResult;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

    ucResult = VDP_HalGetSlowForward(VDP2B2RID(ucVdpId));

    VDP_MUTEX_UNLOCK;

    return ucResult;
}
UINT32 VDP_SetFBNoTimeout(UINT8 ucVdpId, UCHAR ucSet)
{
    UCHAR ucEsId;
    UCHAR ucFbgId;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
    if (ucEsId >= MAX_ES_NS)
    {
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);

    VDP_MUTEX_LOCK;

    if (ucSet != 0)
    {
        FBM_SetFrameBufferGlobalFlag(ucFbgId, FBM_FLAG_FB_NO_TIMEOUT);
    }
    else
    {
        FBM_ClrFrameBufferGlobalFlag(ucFbgId, FBM_FLAG_FB_NO_TIMEOUT);
    }

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;

}


/* VDP trigger mode + audio master to do trigger mode */
void VDP_TriggerAudReceive(UINT8 ucVdpId, UINT32 u4Pts)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    _VDPTriggerAudReceive(VDP2B2RID(ucVdpId), u4Pts);
}

/** Disable VDP Trigger action */
BOOL VDP_TriggerAudReceiveEx(UINT8 ucVdpId, BOOL fgEnable, UINT32 u4Pts, VDP_TRIG_AUD_ARRIVE_CB_T *prCbInfo)
{
    return _VDPTriggerAudReceiveEx(ucVdpId, fgEnable, u4Pts, prCbInfo);
}

/* whether Update STC when VDP start output or not */
BOOL VDP_SetPendingForStc(UINT8 ucVdpId, BOOL fgPending)
{
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);
    return _VDPSetPendingForStc(VDP2B2RID(ucVdpId), fgPending);
}

/* VDP Seek done notification */
void VDP_SetSeek(UCHAR ucVdpId, UCHAR ucSet, UINT32 u4Pts)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    VDP_MUTEX_LOCK;

    _VDPSetSeek(VDP2B2RID(ucVdpId), ucSet, u4Pts);

    VDP_MUTEX_UNLOCK;
}

/* VDP AB repeat notification */
void VDP_SetABRepeat(UCHAR ucVdpId, UCHAR ucSet)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    VDP_MUTEX_LOCK;

    _VDPSetABRepeat(VDP2B2RID(ucVdpId), ucSet);

    VDP_MUTEX_UNLOCK;
}

/* B2R CRC check on PTS value */
void VDP_SetChkPTS(UINT32 u4Pts)
{
    UNUSED(u4Pts);
#ifdef VDP_B2R_PSCAN_CRC_FLOW
    _VDPSetChkPTS(u4Pts);
#endif
}

void VDP_SetEncTrigger(BOOL fgEnable)
{
    UNUSED(fgEnable);
#ifdef VDP_B2R_PSCAN_CRC_FLOW
    _VDP_SetEncTrigger(fgEnable);
#endif
}

void VDP_TestB2RSdPath(void)
{
    VDP_HalTestB2RSdPath();
}

#ifdef CC_SECOND_B2R_SUPPORT
void VDP_GetYCStart(UCHAR ucVdpId,UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    VDP_HalGetYCStart(VDP2B2RID(ucVdpId),pu4YStart, pu4CStart, pu4LineSize, pu4VActive);
}
#else
void VDP_GetYCStart(UINT32 *pu4YStart, UINT32 *pu4CStart, UINT32 *pu4LineSize, UINT32 *pu4VActive)
{
    VDP_HalGetYCStart(pu4YStart, pu4CStart, pu4LineSize, pu4VActive);
}
#endif

void VDP_SetB2RMirror(UCHAR ucVdpId, BOOL fgOn)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    VDP_SetMirrorStatus(VDP2B2RID(ucVdpId), fgOn);
}

void VDP_B2RDumpRegister(BOOL fgOn)
{
    VDP_EnableDumpRegister(fgOn);
}

UCHAR VDP_GetPlayMode(UCHAR ucVdpId)
{
    UCHAR ucPlayMode;
    UCHAR ucEsId;
    UCHAR ucFbgId;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
    if (ucEsId >= MAX_ES_NS)
    {
        return VDP_SET_ERROR;
    }
    ucFbgId = FBM_GetFbgByEs(ucEsId);

//    VDP_MUTEX_LOCK;
// This will cause Assertion fails at x_sema_lock(_hMutex, X_SEMA_OPTION_WAIT) == OSR_OK?
    FBM_GetPlayMode(ucFbgId, &ucPlayMode);

//    VDP_MUTEX_UNLOCK;
    return ucPlayMode;
}

UCHAR VDP_SetThumbnailMode(UCHAR ucVdpId, UCHAR ucSet)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);

    VDP_MUTEX_LOCK;

#if 1
    if (_aucThumbnailMode[ucVdpId] != ucSet)
    {
        _aucThumbnailMode[ucVdpId] = ucSet;
        LOG(5, "Thumbnail Mode (%d)\n", _aucThumbnailMode[ucVdpId]);
    }
#else
    if (ucSet)
    {
        SRM_SetMmMode(SRM_MM_MODE_TYPE_THUMBNAIL);
    }
    else
    {
        SRM_SetMmMode(SRM_MM_MODE_TYPE_VIDEO);
    }
    _aucThumbnailMode[ucVdpId] = ucSet;
#endif

    VDP_MUTEX_UNLOCK;

    return VDP_SET_OK;
}

void B2R_InitVdpConf(void)
{
    UCHAR i;

    for (i=0; i<B2R_NS; i++)
    {
        _aucThumbnailMode[i] = 0;
        _prB2rPrm[i] = _VDP_GetVdpPrm(i);
    }

    for (i=0; i<VDP_NS; i++)
    {
        _prVdpConf[i] = VDP_GetConf(i);
    }

    /* Digital decoder variable-dram-size notification */
    if (_hB2RVdecConfMutex == (HANDLE_T)(NULL))
    {
        VERIFY (x_sema_create(&_hB2RVdecConfMutex, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
    }
    SRM_RegisterCB(SRM_MMBUFCHG_CTRL, _B2RSrmVDecMemConfCb);
}

void B2R_SetImgConnect(UCHAR ucVdpId, UCHAR ucConnect)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    _aucImageConnected[ucVdpId] = ucConnect;
}

UCHAR B2R_GetImgConnect(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    return _aucImageConnected[ucVdpId];
}

void VDP_VsyncInit(void)
{
    _VDP_VsyncInit();
}

void VDP_Image_Init(void)
{
    _VDP_Image_Init();
}

void VDP_VsyncSendCmd(UCHAR ucVdpId, UCHAR ucCmdId)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    _VDP_VsyncSendCmd(VDP2B2RID(ucVdpId), ucCmdId);
}

void VDP_HalB2rSwitch(UCHAR ucVdpId, UCHAR ucB2rId)
{
    _VDP_HalB2rSwitch(ucVdpId, ucB2rId);
}

void VDP_VsyncReset(UCHAR ucVdpId)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    _VDP_VsyncReset(ucVdpId);
}

void VDP_FrcSetPtsCb(UCHAR ucVdpId, UCHAR ucPort, UINT32 u4CbPts, UINT32 u4CbArg)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    _VDP_FrcSetPtsCb(VDP2B2RID(ucVdpId), ucPort, u4CbPts, u4CbArg);
}

void VDP_FrcSetLipSyncCb(UCHAR ucVdpId, UCHAR ucPort, UINT32 u4Thrsd, UINT32 u4FrmCnt)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    _VDP_FrcSetLipSyncCb(VDP2B2RID(ucVdpId), ucPort, u4Thrsd, u4FrmCnt);
}

void VDP_FrcSetLipSyncNGOKCb(UCHAR ucNgOk, UCHAR ucRemove, UINT32 u4Thrsd, UINT32 u4FrmCnt)
{
    _VDP_FrcSetLipSyncNGOKCb(ucNgOk, ucRemove, u4Thrsd, u4FrmCnt);
}

UINT32 VDP_TrickModeSkipFrameNo(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    return _VDP_TrickModeSkipFrameNo(VDP2B2RID(ucVdpId));
}

UINT32 VDP_GetFrameRate(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, 30);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), 30);

    if(_prB2rPrm[VDP2B2RID(ucVdpId)])
    {
        return _prB2rPrm[VDP2B2RID(ucVdpId)]->ucInFrameRate;
    }
    return 30;
}

//void VDP_SetCb
void VDP_SetCbAttachedSrcId(UCHAR ucVdpId, UINT32 u4AttachId)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    if(_prB2rPrm[VDP2B2RID(ucVdpId)])
    {
        _prB2rPrm[VDP2B2RID(ucVdpId)]->u4AttachedSrcId = u4AttachId;
    }
}

UINT32 VDP_Set3DInfo(UCHAR ucVdpId,UINT8 u13DType,BOOL fg3DEnable)
{
    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    _VDP_Set3DInfo(VDP2B2RID(ucVdpId),u13DType,fg3DEnable);

    return VDP_SET_OK;
}

BOOL VDP_IsStartToPlay(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, FALSE);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), FALSE);
    return _VDP_IsStartToPlay(VDP2B2RID(ucVdpId));
}

#if 1
BOOL VDP_SendB2RChgFrameMsg(VDP_B2R_CHG_FRAME_MSG_T* prMsg)
{
    if(prMsg == NULL)
    {
        return FALSE;
    }
#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
    return _VDP_SendB2RChgFrameMsg(prMsg);
#else
    return TRUE;
#endif
}

BOOL VDP_FlushB2RChgFrameMsg(UCHAR ucVdpId)
{
    VERIFY_VDP_ID(ucVdpId, FALSE);
#ifdef CC_B2R_ENABLE_CHG_FRAME_MSG
    return _VDP_FlushB2RChgFrameMsg(VDP2B2RID(ucVdpId));
#else
    return TRUE;
#endif
}
#endif


BOOL VDP_GetB2R2DSupport(UINT32 u4Width,UINT32 u4Height)
{
    if (u4Width > 4096)
    {
        return FALSE;
    }
    if (u4Height > 2304)
    {
        return FALSE;   // Current IC, the VTotal/VActive is 11bits(at most 2048). Will be improved after 5860/5880.
    }
    return TRUE;
}

// Get target resize size of seamless mode
// Return TRUE: it is seamless play, and resize width/height is available.
// Return FALSE: it is not seamless play.
BOOL VDP_GetSeamlessResizeSize(UCHAR ucVdpId, UINT32 *pu4ResizeWidth, UINT32 *pu4ResizeHeight)
{
#ifdef ENABLE_MULTIMEDIA
    FBM_SEQ_HDR_T* prSeqHdr;
    UCHAR ucEsId;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VERIFY_NULL(_prB2rPrm[VDP2B2RID(ucVdpId)]);
    
    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
    prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;

    prSeqHdr = FBM_GetFrameBufferSeqHdr(_prB2rPrm[VDP2B2RID(ucVdpId)]->ucFbgId);
    if ((prVdecEsInfo == NULL) || (prSeqHdr == NULL))
    {
        //LOG(1, "[%s-%d]prSeqHdr = NULL Fail.\n", __func__, __LINE__);
        return FALSE;
    }

    if (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV)
    {
        *pu4ResizeWidth = 1920;
        *pu4ResizeHeight = 1080;
        //LOG(1, "[%s-%d]W,H=(%d, %d).\n", __func__, __LINE__, *pu4ResizeWidth, *pu4ResizeHeight);
        return TRUE;
    }
    else if (prVdecEsInfo->fgSeamlessPlay && prSeqHdr->u4ResizeWidth && prSeqHdr->u4ResizeHeight)
    {
        *pu4ResizeWidth = prSeqHdr->u4ResizeWidth;
        *pu4ResizeHeight = prSeqHdr->u4ResizeHeight;
        return TRUE;
    }
#endif
    return FALSE;
}

UCHAR VDP_GetSeamlessInfo(UCHAR ucVdpId, VDP_SEAMLESS_INFO_T* prSeamlessInfo)
{
#ifdef ENABLE_MULTIMEDIA
    FBM_SEQ_HDR_T* prSeqHdr;
    UINT32 u4SeqWidth;
    UINT32 u4SeqHeight;
    UCHAR ucEsId;
    VDEC_ES_INFO_T* prVdecEsInfo = NULL;

    VERIFY_VDP_ID(ucVdpId, VDP_SET_ERROR);
    VERIFY_B2R_ID(VDP2B2RID(ucVdpId), VDP_SET_ERROR);
    VERIFY_NULL(prSeamlessInfo);
    VERIFY_NULL(_prB2rPrm[VDP2B2RID(ucVdpId)]);
    
    ucEsId = _prVdpConf[ucVdpId]->ucInputPort[0];
    prVdecEsInfo = (ucEsId < MAX_ES_NS)? _VDEC_GetEsInfo(ucEsId) : NULL;

    prSeqHdr = FBM_GetFrameBufferSeqHdr(_prB2rPrm[VDP2B2RID(ucVdpId)]->ucFbgId);
    if (prSeqHdr == NULL)
    {
        //LOG(1, "[%s-%d]prSeqHdr = NULL Fail.\n", __func__, __LINE__);
        return VDP_SET_ERROR;
    }
    
    if (prVdecEsInfo && (prVdecEsInfo->eSeamlessMode & SEAMLESS_BY_NPTV)) // NPTV seamless play
    {
        u4SeqWidth = (UINT32) prSeqHdr->u2HSize;
        u4SeqHeight = (UINT32) prSeqHdr->u2VSize;
    }
    else
    {
        LOG(1, "[%s-%d]Not fgSeamlessPlay Fail.\n", __func__, __LINE__);
        return VDP_SET_ERROR;
    }
    
    // because of the 420 sample method
    // progressive : the height should be a multiple of 2
    // interlace : the height should be a multiple of 4
    if (prSeqHdr->fgProgressiveSeq != 0)
    {
        u4SeqHeight &= 0xFFFFFFFE;
    }
    else
    {
        u4SeqHeight &= 0xFFFFFFFC;
    }
    
#ifdef CC_B2R_3D_SUPPROT
    if (prSeqHdr->fgB2R3DEnable)
    {
        LOG(1, "[%s-%d]3D Not support NPTV SeamlessPlay Yet.\n", __func__, __LINE__);
        return VDP_SET_ERROR;
    }
#endif
    
    prSeamlessInfo->u4SrcWidth = ((u4SeqWidth > B2R_MAX_WIDTH) ? B2R_MAX_WIDTH : u4SeqWidth);
    prSeamlessInfo->u4SrcHeight = ((u4SeqHeight > B2R_MAX_HEIGHT) ? B2R_MAX_HEIGHT : u4SeqHeight);

    _B2R_Height_Align(&prSeamlessInfo->u4SrcHeight);
    LOG(2, "[%s-%d]W,H=(%d, %d).\n", __func__, __LINE__, prSeamlessInfo->u4SrcWidth, prSeamlessInfo->u4SrcHeight);
    
    return VDP_SET_OK;
#else
    return VDP_SET_ERROR;
#endif
}

void VDP_NfySetResolutionReady(UCHAR ucVdpId, BOOL fgReady)
{
    VERIFY_VDP_ID_RET_VOID(ucVdpId);
    VERIFY_B2R_ID_RET_VOID(VDP2B2RID(ucVdpId));
    UNUSED(ucVdpId);
    UNUSED(fgReady);
}

#ifdef CC_FAST_INIT
void VDP_pm_suspend(void)
{
}


void VDP_pm_resume(void)
{
    BIM_EnableIrq(VECTOR_DISPLAY);
}
#endif

#ifdef CC_SUPPORT_STR

void B2R_pm_str_suspend(void)
{
	B2R_CkgenOnOff(FALSE);
}

void B2R_pm_str_resume(void)
{
	B2R_CkgenOnOff(TRUE);
}

#endif

BOOL B2R_CkgenOnOff(BOOL fgEnable)
{
#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) 
#define BLOCK_RST_CFG0 0x270
#define B2R_RST ((UINT32)1 << 1)
#define BLOCK_CKEN_CFG0 0x278
#define B2R_CKEN ((UINT32)1 << 1)
#else
#define BLOCK_RST_CFG0 0x1C0
#define B2R_RST ((UINT32)1 << 7)
#define BLOCK_CKEN_CFG0 0x1C8
#define B2R_CKEN ((UINT32)1 << 7)
#endif

    if (fgEnable)
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~B2R_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) | B2R_CKEN);
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) | B2R_RST);
		VDP_ForceResetClk(fgEnable);
    }
    else
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~B2R_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) & ~B2R_CKEN);
    }

    UNUSED(fgEnable);
    return TRUE;
}

VOID B2R_EnableFieldLog(BOOL fgOn)
{
	VDP_EnableB2RFieldLog(fgOn);
}

UINT32 VDP_GetDispBufLocked(UCHAR ucVdpId, UINT32 u4BufAddr, BOOL *pfgLocked)
{
    UCHAR ucB2rId = VDP_GetB2rId(ucVdpId);
    return _VDP_GetDispBufLocked(ucB2rId, u4BufAddr, pfgLocked);
}

void B2R_EnalbleSmartPcr(UCHAR ucB2rId, BOOL fgEn)
{
    B2R_HalEnableSmartPcr(ucB2rId, fgEn);
}


INT32 VDP_GetPtsStcDelta(UCHAR ucB2rId)
{
    return _VDP_GetPtsStcDelta(ucB2rId);
}

VOID VDP_SetRepeatFrame(UCHAR ucVdpId)
{
    _VDP_SetRepeatFrame(ucVdpId);
}

VOID VDP_SetDropFrame(UCHAR ucVdpId)
{
    _VDP_SetDropFrame(ucVdpId);
}

B2R_PRM_T* VDP_GetVdpPrm(UINT8 u1B2rId)
{
    return _VDP_GetVdpPrm(u1B2rId);
}

VOID VDP_SetTimeShiftMode(UINT8 u1B2rId,BOOL fgEnable)
{
    _VDP_SetTimeShiftMode(u1B2rId, fgEnable);
}

VOID VDP_SetTimeShiftSpeed(UINT8 u1B2rId,UINT32 u4SpeedValue)
{
    _VDP_SetTimeShiftSpeed(u1B2rId, u4SpeedValue);
}

UCHAR VDP_GetFbg(UINT8 u1B2rId)
{
    return 
_VDP_GetFbg(u1B2rId);
}

#ifdef CC_MAPLE_CUST_DRV
INT32 B2R_Get_Cust_info (UCHAR                 ucB2rId,
                              B2R_GET_CUST_TYPE_T   e_set_type,
                              VOID*                 pv_get_info,
                              SIZE_T                z_set_info_size)
{
    INT32   i4Ret = B2R_OK;

    do
    {
        if(ucB2rId >= B2R_NS || 
            !pv_get_info)
        {
            i4Ret = B2R_INV_ARG;
            break;
        }

        switch(e_set_type)
        {
            case B2R_GET_TYPE_CUST_INFO:
            {
                B2R_CUST_INFO_T*    pt_cust_info = (B2R_CUST_INFO_T*)pv_get_info;
 
                if(sizeof(B2R_CUST_INFO_T) != z_set_info_size)
                {
                    i4Ret = B2R_INV_ARG;
                    break;
                }
                i4Ret = _B2R_Get_Cust_info(ucB2rId,pt_cust_info);
                break;
            }

            case B2R_GET_TYPE_ASPINFO:
            {
                UINT8*    pu1_asp_info = (UINT8*)pv_get_info;
                if(sizeof(UINT8) != z_set_info_size)
                {
                    i4Ret = B2R_INV_ARG;
                    break;
                }
                i4Ret = _B2R_GetAspInfo(ucB2rId,pu1_asp_info);
                break;
            }

            default:
                i4Ret = B2R_NOT_IMPL;
                break;
        }
    }while(0);

    if(i4Ret != B2R_OK)
    {
        LOG(0,"%s Failed , i4Ret : %d!\n",__FUNCTION__,i4Ret);
    }

    return i4Ret;
}

#endif

INT32 VDP_B2R_GetInfo (UCHAR            ucVdpId,
                           B2R_GET_TYPE_T   e_get_type,
                           VOID*            pv_get_info,
                           SIZE_T           z_get_info_size)
{
    INT32   i4Ret = B2R_OK;
    VERIFY_VDP_ID(ucVdpId,B2R_INV_ARG);
    if(!pv_get_info)
    {
        return B2R_INV_ARG;
    }

    switch (e_get_type)
    {
        case B2R_GET_TYPE_ASP_INFO:
        {
            VDP_CONF_T*      prVdpConf;
            B2R_ASP_INFO_T*  pt_asp_info = (B2R_ASP_INFO_T*)pv_get_info;
    
            prVdpConf = _prVdpConf[ucVdpId] ;
            if(!prVdpConf)
            {
                i4Ret = B2R_INV_ARG;
                break;
            }
            
            if(sizeof(B2R_ASP_INFO_T) != z_get_info_size)
            {
                i4Ret = B2R_INV_ARG;
                break;
            }
            pt_asp_info->u4Afd               = prVdpConf->u4Afd;
            pt_asp_info->u4AspectRatio       = prVdpConf->u4AspectRatio;
            pt_asp_info->u4AspectRatioIdc    = prVdpConf->u4AspectRatioIdc;
            pt_asp_info->u4AspectRatioWidth  = prVdpConf->u4AspectRatioWidth;
            pt_asp_info->u4AspectRatioHeight = prVdpConf->u4AspectRatioHeight;
            break;
        }
        
        default:
            break;
    }

    return i4Ret;
}

#ifdef ENABLE_MULTIMEDIA
INT32 B2R_RTSetInfo (UCHAR               ucB2rId,
                     B2R_RT_TYPE_T       e_set_type,
                     VOID*               pv_set_info,
                     SIZE_T              z_set_info_size)
{
    if(ucB2rId >= B2R_NS
        || !pv_set_info)
    {
        return VDP_SET_ERROR;
    }
    
    switch(e_set_type)
    {
        case B2R_RT_TYPE_CLI_ACTIVE:
        {
            UCHAR    ucEnable;

            if(z_set_info_size != sizeof(UCHAR))
            {
                return VDP_SET_ERROR;
            }
            ucEnable = *((UCHAR *)pv_set_info);
            _B2R_RTCliActive(ucB2rId,ucEnable);
            break;
        }

        case B2R_RT_TYPE_LARENCY_NS:
        {
            UINT32 u4_latency;

            if(z_set_info_size != sizeof(UINT32))
            {
                return VDP_SET_ERROR;
            }
            u4_latency = *((UINT32 *)pv_set_info);
            _B2R_SetLatency(ucB2rId,u4_latency);
            break;
        }

        default:
            LOG(0,"%s,  Unknow set type : %d!\n",__FUNCTION__,e_set_type);
            break;
    }

    return VDP_SET_OK;
}
#endif

/**
 * B2R call back function register
 *
 * @param eFuncType specify the call back function Info.
 * @param u4FuncPtr specify the call back function pointer.
 * @return INT32
 */
INT32 B2R_RegCbFunc(B2R_CB_FUNC_ENUM_T eFuncType, UINT32 u4FuncPtr, UINT32 u4Arg1)
{
    INT32 i4Ret = B2R_OK;
    return i4Ret;
}

/** Disable VDP Trigger action */
INT32 B2R_AudTrigger(UINT8 ucVdpId, VDP_TRIG_AUD_CFG_T* pr_trig_cfg)
{
    return 0;
}

INT32 B2R_FilterLog(UCHAR ucVdpId, UCHAR ucEnable, UINT32 u4Mask)
{
    return 0;
}
