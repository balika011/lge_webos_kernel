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
 * $RCSfile: nav_i1394.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_i1394.c
 *  This header file includes public function definitions of i1394 demo.
 */

//lint -save
//lint -e717

/*
    lint inhibition(s)
        961 - #undef (used in middleware u_common.h and gfx engine workaround
        970 - use char when CHAR is defined
        971 - use char without signed or unsigned
        683 - known function redefined (eg. printf --> Printf)
*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

/*lint -save -e961 -e970 -e971 -e683 */
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
#include "x_hal_5371.h"
#include "x_hal_926.h"
#include "x_drv_cli.h"

#include "aud_if.h"
#include "osd_if.h"
#include "gfx_if.h"
#include "ir_if.h"
#include "drv_fnt.h"
#include "drv_common.h"
#include "vdp_if.h"
#include "sif_if.h"
#include "nim_if.h"
#include "drv_aud.h"
#include "mpv_if.h"
#include "dmx_if.h"
#include "fbm_if.h"
#include "i1394_if.h"
#include "dvmgr_if.h"
#include "nptv_if.h"
#include "psipsr_if.h"
#include "sv_const.h"
#include "source_table.h"
LINT_EXT_HEADER_END

/*lint -restore */

#include "widget_if.h"
#include "nav.h"
#include "nav_event.h"
#include "nav_av.h"

#include "nav_debug.h"


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// FIX-ME-LATER
#define _I1394_BMP_INDEX 9
#define _VDP_ID VDP_1
#define _DMX_VIDEO_PID 0x810
#define _DMX_VIDEO_PID_ENTRY 0
#define _DMX_AUDIO_PID 0x814
#define _DMX_AUDIO_PID_ENTRY 1
#define _DMX_DEFAULT_ES_FIFO_SIZE 0x300000
#define _MPV_ES_ID 0

#define _MAX_I1394_DEVICE_NUM 3
#define _STATE_NOT_INIT 0
#define _STATE_NOT_INIT_MENU 1
#define _STATE_READY 2
#define _STATE_PLAY 3

#define _1394_PSI_TIMEOUT  300000


//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------

extern WIDGET_BITMAP_T arBmpTbl[];
extern NAV_VIDEOPATH_T arVideoPath[];


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

EXTERN BOOL _NAV_VideoCallback(const DMX_PES_MSG_T* prPes);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static WIDGET_MENULIST_T *_prList;
static WIDGET_MENU_T* _prMenu = NULL;
static WIDGET_MENUITEM_T *_prDetectItem = NULL;
static WIDGET_MENUITEM_T *_parDeviceItem[_MAX_I1394_DEVICE_NUM];
static PREMOTE_DEV prDeviceList;
static PREMOTE_DEV prSelectedDevice = NULL;
static BOOL _fgIsDVMode;

static UINT32 _u4ColorMode;
static UINT32 _u4State = _STATE_NOT_INIT;
static INT32 _i4DeviceNumber = 0;


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

static INT32 _StopDevice(void)
{
    INT32 i4Status;
    VDP_REGION_T rZeroRegion;

    NAV_LockAVMutex(_VDP_ID);
/*
    // stop 1394
    i4Status = I1394_SetDeviceStop(prSelectedDevice->i4NodeID);
    if (i4Status != I1394RET_OK)
    {
        LOG(1, "_StopDevice: fail\n");
        return E_NAV_MSG_ERROR;
    }
*/
    //VDP_SetMode(_VDP_ID, VDP_MODE_BG);
    //VDP_SetFreeze(_VDP_ID, 0);
    VERIFY(VDP_SetEnable(_VDP_ID, 0) == VDP_SET_OK);

    // stop DV
    if (_fgIsDVMode)
    {
        // stop 1394
        i4Status = I1394_SetDeviceStop((UINT32)prSelectedDevice->i4NodeID);
        if (i4Status != I1394RET_OK)
        {
            LOG(1, "_StopDevice: fail\n");

            NAV_UnlockAVMutex(_VDP_ID);

            return (INT32)E_NAV_MSG_ERROR;
        }

        UNUSED(DVMgr_SetMode(FALSE, 0));
        VERIFY(DMX_DV_SetMode(FALSE, FALSE, TRUE, DMX_DV_FORMAT_UNKNOWN));

	// Rocky 20060516
	//DMX_Init();

        DMX_SetToDecoder(TRUE);
        VERIFY(VDP_SetInput(_VDP_ID, 0, _MPV_ES_ID) == VDP_SET_OK);
    }
    else
    {
        DMX_PID_T rPid;
#if 1
        // stop audio
        AUD_DSPCmdStop(AUD_DEC_MAIN);
#endif

        // stop 1394
        i4Status = I1394_SetDeviceStop((UINT32)prSelectedDevice->i4NodeID);
        if (i4Status != I1394RET_OK)
        {
            LOG(1, "_StopDevice: fail\n");

            NAV_UnlockAVMutex(_VDP_ID);

            return (INT32)E_NAV_MSG_ERROR;
        }


        rPid.fgEnable = FALSE;
        VERIFY(DMX_SetPid(_DMX_VIDEO_PID_ENTRY, DMX_PID_FLAG_VALID, &rPid));

        // stop MPV
        MPV_Stop(_MPV_ES_ID);
        MPV_Flush(_MPV_ES_ID);

        // stop dmx
        #if 1
        rPid.fgEnable = FALSE;
        DMX_SetPid(_DMX_AUDIO_PID_ENTRY, DMX_PID_FLAG_VALID, &rPid);
        DMX_FreePid(_DMX_AUDIO_PID_ENTRY);
        #endif


        // NOTE: free pid after stop MPV
        VERIFY(DMX_FreePid(_DMX_VIDEO_PID_ENTRY));
    }

    // stop vdp
    rZeroRegion.u4X = 0;
    rZeroRegion.u4Y = 0;
    rZeroRegion.u4Height = 0;
    rZeroRegion.u4Width = 0;
    VERIFY(VDP_SetOutRegion(_VDP_ID, 0, rZeroRegion) == VDP_SET_OK);
    VERIFY(VDP_SetEnable(_VDP_ID, 0) == VDP_SET_OK);

    // modify state
    prSelectedDevice = NULL;
    _u4State = _STATE_READY;

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    NAV_UnlockAVMutex(_VDP_ID);

    return (INT32)E_NAV_MSG_OK;
}


//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

static INT32 _PlayDevice(void)
{
    INT32 i4Ret, i4Retry;
    DMX_DECODER_CALLBACKS_T rDecoderCallbacks;

    NAV_LockAVMutex(_VDP_ID);


	//CLI_Parser("dmx.settuner 1");
    VERIFY(NAV_ClearBorder(VDP_1) == (INT32)E_NAV_MSG_OK);
    VERIFY(NAV_ClearBorder(VDP_2) == (INT32)E_NAV_MSG_OK);

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    /*
    NAV_StopAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src);
    NAV_StopAV(NAV_VDP_SLAVE, arVideoPath[NAV_VDP_SLAVE].u4Src);
    */

	// Set Demux to tuner 1
    {
        DMX_PID_T rPid;

        UNUSED(DMX_PSI_FlushBuffer(_DMX_VIDEO_PID_ENTRY));
        rPid.u1TsIndex = 1;
        VERIFY(DMX_SetPid(_DMX_VIDEO_PID_ENTRY, DMX_PID_FLAG_TS_INDEX, &rPid));
    }

    if (I1394_SelectDevice((UINT32)prSelectedDevice->i4NodeID) != I1394RET_OK)
    {
        LOG(1, "_PlayDevice: I1394_SelectDevice fail\n");
        return (INT32)E_NAV_MSG_ERROR;
    }

    UNUSED(bApiVideoSetVideoSrc(_VDP_ID, (UINT8)SV_VS_DT1));
    do
    {
        VDP_REGION_T rOutRegion;
        VDP_REGION_T rSrcRegion;
        rOutRegion.u4X = 0;
        rOutRegion.u4Y = 0;
        rOutRegion.u4Width = 0;
        rOutRegion.u4Height = 0;
        VERIFY(VDP_SetOutRegion(_VDP_ID, 1, rOutRegion) == VDP_SET_OK);
        rSrcRegion.u4X = 0;
        rSrcRegion.u4Y = 0;
        rSrcRegion.u4Width = 0;
        rSrcRegion.u4Height = 0;
        VERIFY(VDP_SetSrcRegion(_VDP_ID, 1, rSrcRegion) == VDP_SET_OK);
    } while (0);
    VERIFY(VDP_SetBg(_VDP_ID, 0) == VDP_SET_OK);
    //VDP_SetMode(_VDP_ID, VDP_MODE_NORMAL);
    VERIFY(VDP_SetEnable(_VDP_ID, 1) == VDP_SET_OK);


    rDecoderCallbacks.pfnSendVideoPes = _NAV_VideoCallback;
    rDecoderCallbacks.pfnUpdateVideoWp = MPV_UpdateVldWptr;
    rDecoderCallbacks.pfnSendAudioPes = AUD_SendAudioPes;
    DMX_SetDecoderCallbacks(&rDecoderCallbacks);

    if (I1394_SetDevicePlay((UINT32)prSelectedDevice->i4NodeID, TRUE, (UINT32)SPEED_NORMAL) !=
        I1394RET_OK)
    {
        LOG(1, "_PlayDevice: I1394_SetDevicePlay fail\n");
        NAV_UnlockAVMutex(_VDP_ID);
        return (INT32)E_NAV_MSG_ERROR;
    }

    for (i4Retry = 0; i4Retry < 5; i4Retry++)
    {
        i4Ret = I1394_SetSignalFormat((UINT32)prSelectedDevice->i4NodeID, &_fgIsDVMode);
        if (i4Ret == I1394RET_OK)
        {
            break;
        }

        x_thread_delay(1000);
    }

    if (i4Ret != I1394RET_OK)
    {
        LOG(1, "_PlayDevice: I1394_SetSignalFormat fail\n");

        UNUSED(I1394_SetDeviceStop((UINT32)prSelectedDevice->i4NodeID));
        return (INT32)E_NAV_MSG_ERROR;
    }

    if (_fgIsDVMode)
    {
        // Disable 1394 Ishochronous transaction
        UNUSED(I1394_ControlIsoTransaction(TRUE, FALSE));

        // if the signal format is DV
        // 1394 -> demux -> dv decoder -> display (samsung)
        DMX_SetToDecoder(FALSE);

        // Enable 1394 Ishochronous transaction
        UNUSED(I1394_ControlIsoTransaction(TRUE, TRUE));
        VERIFY(VDP_SetInput(_VDP_ID, 5, _MPV_ES_ID) == VDP_SET_OK);
    }
    else
    {
        UINT32 u4Ret, u4Num =16, u4VPid = 0, u4APid = 0, u4APidType, u4Retry;
        PSIPSR_TBL_PMT_T arPMTTbl[16];

        // if the signal format is mpeg ts
        // 1394 -> demux -> mpeg -> display (MPGE TS)

        // Parse PSI table
        for (u4Retry = 0; u4Retry < 5; u4Retry++)
        {
            x_memset(arPMTTbl, 0, sizeof(arPMTTbl));
            u4Ret = (UINT32)PSIPSR_ParsePSI(arPMTTbl, &u4Num, _1394_PSI_TIMEOUT, 1);

            if (u4Ret == (UINT32)E_PSIPSR_OK)
	        {
	            u4VPid = arPMTTbl[0].u4VPID;
	            u4APid = arPMTTbl[0].u4APID1;
	            u4APidType = (arPMTTbl[0].u4APID1Type & 0x80) ?
	                         (UINT32)AUD_FMT_AC3 : (UINT32)AUD_FMT_MPEG;
                LOG(5, "VPid = 0x%x, APid = 0x%x, u4ApidType = 0x%x\n",
                    u4VPid, u4APid, u4APidType);
                break;
	        }
        }

        if (u4Ret != (INT32)E_PSIPSR_OK)
	    {
            LOG(5, "PSIPSR failed \n");
	        if (u4Ret == (INT32)E_PSIPSR_TIMEOUT)
            {
                LOG(5, "PSIPSR timeout \n");
            }

//	    u4VPid = _DMX_VIDEO_PID;
//	    u4APid = _DMX_AUDIO_PID;
//	    u4APidType = AUD_FMT_MPEG;

            {
                VDP_REGION_T rZeroRegion;

                // stop 1394
                UNUSED(I1394_SetDeviceStop((UINT32)prSelectedDevice->i4NodeID));

                // stop MPV
                MPV_Stop(_MPV_ES_ID);
                MPV_Flush(_MPV_ES_ID);

                // stop vdp
                rZeroRegion.u4X = 0;
                rZeroRegion.u4Y = 0;
                rZeroRegion.u4Height = 0;
                rZeroRegion.u4Width = 0;
                VERIFY(VDP_SetOutRegion(_VDP_ID, 0, rZeroRegion) == VDP_SET_OK);
                VERIFY(VDP_SetEnable(_VDP_ID, 0) == VDP_SET_OK);

                // modify state
                prSelectedDevice = NULL;
                _u4State = _STATE_READY;

                NAV_UnlockAVMutex(_VDP_ID);
                return (INT32)E_NAV_MSG_ERROR;
            }
        }

        VERIFY(VDP_SetInput(_VDP_ID, 0, _MPV_ES_ID) == VDP_SET_OK);
        MPV_Play(_MPV_ES_ID);
        // settodecoder
        DMX_SetToDecoder(TRUE);

       // set video pid
        do
        {
            DMX_PID_T rPid;

#if 1
            FBM_POOL_T* prFbmPool;

            // Allocate video buffer from FBM
            prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
            VERIFY(prFbmPool != NULL);
            VERIFY(prFbmPool->u4Addr != NULL);

            // Setup buffer attributes
            rPid.fgAllocateBuffer = FALSE;
            rPid.u4BufAddr = prFbmPool->u4Addr;
            rPid.u4BufSize = prFbmPool->u4Size;
#else
            rPid.fgAllocateBuffer = TRUE;
            rPid.u4BufAddr = 0;
            rPid.u4BufSize = _DMX_DEFAULT_ES_FIFO_SIZE;
#endif

            rPid.u1TsIndex = 1;
            rPid.u1DeviceId = 0;
            rPid.u1KeyIndex = 0;
            rPid.fgEnable = TRUE;
            rPid.u1SteerMode = DMX_STEER_TO_FTUP;
            rPid.ePcrMode = DMX_PCR_MODE_NONE;
            rPid.eDescMode = DMX_DESC_MODE_NONE;
            rPid.pfnNotify = NULL;
            rPid.pvNotifyTag = NULL;
            rPid.u2Pid = u4VPid;
            rPid.ePidType = DMX_PID_TYPE_ES_VIDEO;

            VERIFY(DMX_SetPid(_DMX_VIDEO_PID_ENTRY, DMX_PID_FLAG_ALL, &rPid));
        } while (0);
#if 1
        // set audio pid
        do
        {
            DMX_PID_T rPid;

            rPid.u1TsIndex = 1;
            rPid.u1DeviceId = 0;
            rPid.u1KeyIndex = 0;
            rPid.fgEnable = TRUE;
            rPid.u1SteerMode = DMX_STEER_TO_FTUP;
            rPid.fgAllocateBuffer = TRUE;
            rPid.ePcrMode = DMX_PCR_MODE_NONE;
            rPid.eDescMode = DMX_DESC_MODE_NONE;
            rPid.u4BufAddr = 0;
            rPid.u4BufSize = _DMX_DEFAULT_ES_FIFO_SIZE;
            rPid.pfnNotify = NULL;
            rPid.pvNotifyTag = NULL;
            rPid.u2Pid = u4APid;
            rPid.ePidType = DMX_PID_TYPE_ES_AUDIO;

            AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_DIGITAL_TUNER,
                           u4APidType);
            AUD_SetAvSynMode(0, 2); // Open AV sync
            // Process audio FIFO specially
            if (rPid.ePidType == DMX_PID_TYPE_ES_AUDIO)
            {
                UINT32 au4BufStart[2], au4BufEnd[2];

                if (AUD_GetAudFifo(&au4BufStart[0], &au4BufEnd[0],
                                   &au4BufStart[1], &au4BufEnd[1]) != AUD_OK)
                {
                    LOG(1, "_PlayDevice: AUD_GetAudFifo fail\n");
                    NAV_UnlockAVMutex(_VDP_ID);
                    return E_NAV_MSG_ERROR;
                }

                if (au4BufStart[0] == 0 || au4BufStart[1] == 0)
                {
                    LOG(1, "_PlayDevice: AUD_GetAudFifo fail\n");
                    NAV_UnlockAVMutex(_VDP_ID);
                    return E_NAV_MSG_ERROR;
                }

                rPid.u4BufAddr = au4BufStart[rPid.u1DeviceId];
                rPid.u4BufSize = au4BufEnd[rPid.u1DeviceId] -
                                 au4BufStart[rPid.u1DeviceId];
                rPid.fgAllocateBuffer = FALSE;
            }
            DMX_SetPid(_DMX_AUDIO_PID_ENTRY, DMX_PID_FLAG_ALL, &rPid);
			AUD_DSPCmdPlay(AUD_DEC_MAIN);
        } while (0);
#endif
    }
/*
    if (I1394_SetDevicePlay(prSelectedDevice->i4NodeID, TRUE, SPEED_NORMAL) !=
        I1394RET_OK)
    {
        LOG(1, "_PlayDevice: I1394_SetDevicePlay fail\n");
        NAV_UnlockAVMutex(_VDP_ID);
        return E_NAV_MSG_ERROR;
    }
*/
    _u4State = _STATE_PLAY;
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(_VDP_ID);

    return (INT32)E_NAV_MSG_OK;
}

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

static INT32 _ActionDetectDevice(void* pvParam)
{
    UINT32 i;
    (void)pvParam;

    // stop if playing
    if (_u4State == _STATE_PLAY)
    {
        UNUSED(_StopDevice());
    }
    // chack state
    if (_u4State != _STATE_READY)
    {
        LOG(1, "_ActionDetectDevice: wrong state\n");
        return (INT32)E_NAV_MSG_WRONG_STATE;
    }

    // clear old device info and menu item
    while (_i4DeviceNumber > 0)
    {
        VERIFY(WIDGET_RemoveLastMenuItem(_prMenu) == (INT32)E_WIDGET_MSG_OK);
        _i4DeviceNumber--;
    }

    // polling
    if (I1394_GetAllDevices(&prDeviceList, &_i4DeviceNumber) != I1394RET_OK)
    {
        LOG(1, "_ActionDetectDevice: I1394_GetAllDevices fail\n");
        _i4DeviceNumber = 0;
    }
    LOG(6, "_ActionDetectDevice: _i4DeviceNumber=%d\n", _i4DeviceNumber);
    // truncate if too many device
    if (_i4DeviceNumber > _MAX_I1394_DEVICE_NUM)
    {
        LOG(3, "_ActionDetectDevice: _i4DeviceNumber is more than %d\n",
            _MAX_I1394_DEVICE_NUM);
        _i4DeviceNumber = _MAX_I1394_DEVICE_NUM;
    }

    // add device to menu item
    for (i = 0; i < (UINT32)_i4DeviceNumber; i++)
    {
        // change title
        VERIFY(WIDGET_MenuItemSetTitle((const WIDGET_MENUITEM_T*)(void*)_parDeviceItem[i], (const CHAR*)prDeviceList[i].uDevName) == (INT32)E_WIDGET_MSG_OK);
        // add to menu
        VERIFY(WIDGET_AddMenuItem(_prMenu, (WIDGET_MENUITEM_T*)(void*)_parDeviceItem[i]) == (INT32)E_WIDGET_MSG_OK);
    }
    // FIX-ME-LATER...should better return a redraw request to container
    // redraw the whole menu
    VERIFY(WIDGET_ShowMenuList(_prList) == (INT32)E_WIDGET_MSG_OK);

    return (INT32)E_NAV_MSG_OK;
}

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

static INT32 _ActionSelectDevice(void* pvParam)
{
    UINT32 u4Idx = (UINT32)pvParam;
    LOG(6, "_ActionSelectDevice: u4Idx=%d\n", u4Idx);

    if(arVideoPath[NAV_VDP_MASTER].u4Src != (UINT32)E_NAV_VS_DT2)
    {
    	return (INT32)E_NAV_MSG_OK;
    }
    // select the same device
    if ( (prSelectedDevice != NULL) && (prSelectedDevice == &prDeviceList[u4Idx]))
    {
        UNUSED(_StopDevice());
        return (INT32)E_NAV_MSG_OK;
    }

    if (_u4State == _STATE_PLAY)
    {
        UNUSED(_StopDevice());
    }
    else // maybe it's playing other source
    {

    	/*
        NAV_StopAV(VDP_1, arVideoPath[VDP_1].u4Src);
        NAV_StopAV(VDP_2, arVideoPath[VDP_2].u4Src);
	*/

    }
    if (_u4State == _STATE_READY)
    {
        prSelectedDevice = &prDeviceList[u4Idx];
        return _PlayDevice();
    }
    LOG(1, "_ActionDetectDevice: wrong state\n");
    prSelectedDevice = NULL;
    _u4State = _STATE_READY;
    return (INT32)E_NAV_MSG_WRONG_STATE;
}




//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

INT32 NAV_I1394_SetMode(UINT32 u4CtrCode)
{
	INT32 i4Ret;
	if(_u4State == _STATE_PLAY)
	{
		if(I1394_DeviceControl((UINT32)prSelectedDevice->i4NodeID, u4CtrCode) == I1394RET_ERROR)
		{
			i4Ret = (INT32)E_NAV_MSG_NOT_SUPPORT;
		}
		else
		{
			i4Ret = (INT32)E_NAV_MSG_OK;
		}
		UNUSED(I1394_CheckDevicePlayState((UINT32)prSelectedDevice->i4NodeID, _fgIsDVMode,
		                           _DMX_VIDEO_PID_ENTRY));
		return i4Ret;
	}
	return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

INT32 NAV_I1394_GetMode(void)
{
	return I1394_CheckDevicePlayState((UINT32)prSelectedDevice->i4NodeID, _fgIsDVMode,
	                                  _DMX_VIDEO_PID_ENTRY);
}

void NAV_I1394_Init(void)
{
    if (_u4State == _STATE_NOT_INIT)
    {
        _u4ColorMode = arWidgetOSD[0].u4ColorMode;
        VERIFY(DVMgr_Init());
        VERIFY(I1394_Init() == I1394RET_OK);
        _u4State = _STATE_NOT_INIT_MENU;
    }
}


//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------

void NAV_I1394_SetupMenu(WIDGET_MENULIST_T* prList)
{
    if (_u4State == _STATE_NOT_INIT_MENU)
    {
        UINT32 i;

        _prList = prList;
        // attach menu
        _prMenu = WIDGET_NewMenu("i1394", _u4ColorMode,
        &arBmpTbl[_I1394_BMP_INDEX],
        &arBmpTbl[_I1394_BMP_INDEX + 1],
        &arBmpTbl[_I1394_BMP_INDEX + 1]);
        VERIFY(WIDGET_AddMenu(prList, _prMenu) == (INT32)E_WIDGET_MSG_OK);

        // Since upon system bootup the current source is DTV, we disable I1394 menu in the beginning
        _prMenu->fgEnable = FALSE;
        _prMenu->prCurr = NULL;

        // attach detect menu item
        // MEMORY-LEAK
        _prDetectItem = (WIDGET_MENUITEM_T*)(void*)WIDGET_NewMenuItem_Action("Detect Device", _u4ColorMode, NULL);

        VERIFY(WIDGET_SetAction(_prDetectItem,
            NAV_IRHandler_MenuItem_Action,
            _ActionDetectDevice,
            (INT32(*)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32))WIDGET_DrawMenuItemAction,
            (INT32(*)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32))WIDGET_DrawMenuItemAction) == (INT32)E_WIDGET_MSG_OK);

        _prDetectItem->pvCallbackTag = (void*)0;

        VERIFY(WIDGET_AddMenuItem(_prMenu, _prDetectItem) == (INT32)E_WIDGET_MSG_OK);

        // prepare device menu item
        for (i = 0; i < _MAX_I1394_DEVICE_NUM; i++)
        {
            // MEMORY-LEAK
            _parDeviceItem[i] = (WIDGET_MENUITEM_T*)(void*)
            WIDGET_NewMenuItem_Action("i1394 device",_u4ColorMode, NULL);

            VERIFY(WIDGET_SetAction(_parDeviceItem[i],
                NAV_IRHandler_MenuItem_Action,
                _ActionSelectDevice,
                (INT32(*)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32))WIDGET_DrawMenuItemAction,
                (INT32(*)(const WIDGET_MENUITEM_T*, const WIDGET_OSDINFO_T*, UINT32))WIDGET_DrawMenuItemAction) == (INT32)E_WIDGET_MSG_OK);

            _parDeviceItem[i]->pvCallbackTag = (void*)i;
        }

        _u4State = _STATE_READY;
    }
}


INT32 NAV_I1394_GetState(void)
{
	return (INT32)_u4State;
}

INT32 NAV_I1394_StopDevice(void)
{
	return _StopDevice();
}

INT32 NAV_I1394_PlayDevice(void)
{
	return _PlayDevice();
}

INT32 NAV_I1394_DetectDevice(void)
{
	WIDGET_MENUITEM_ACTION_T* prItem;
	prItem = (WIDGET_MENUITEM_ACTION_T*)(void*)WIDGET_LookupMenuItem(_prList, "i1394", "Detect Device" );
	if(_ActionDetectDevice((void*)prItem->pvCallbackTag) == (INT32)E_NAV_MSG_OK)
	{
		return (INT32)E_NAV_MSG_OK;
	}
	return (INT32)E_NAV_MSG_ERROR;
}

//lint -restore

