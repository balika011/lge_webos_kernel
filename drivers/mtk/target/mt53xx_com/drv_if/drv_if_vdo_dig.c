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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_if_vdo_dig.c,v $
 * $Revision: #1 $
 * $Revision: #1 $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdo_dig.c
 *  Driver interface: digital VDO part implementation
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_assert.h"
#include "x_os.h"
#include "vdp_drvif.h"
#include "mpv_if.h"
#include "dmx_if.h"
#include "nptv_if.h"
#include "aud_if.h"
#include "aud_drvif.h"
#include "vdec_if.h"
#include "vdec_drvif.h"

LINT_EXT_HEADER_END
#include "x_drv_if.h"
#include "vdo_common.h"
#include "aud_common.h"
#include "drv_if_dmx.h"
#include "pcr.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define DRV_VDO_MPEGCC_FIFO_SIZE 2048
#define STREAM_TYPE_VIDEO		2
#define STREAM_TYPE_MPEG1		3
#define STREAM_TYPE_MPEG2		4
#define STREAM_TYPE_PRIVATE     6
#define STREAM_TYPE_H264		0x1B
#define STREAM_TYPE_AVS         0x42
#define STREAM_TYPE_CYPHER		0x80
#define STREAM_TYPE_UNKNOWN		0xFF


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------
extern void VDEC_SetWptr(UINT8 u1VdecId, UINT32 u4Wptr);


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static PFN_DMX_NOTIFY _pfScrambleChgNfy = NULL;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**  _NAV_VideoCallback
 *  Callback from demux to mpeg decoder
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static BOOL _MPEGVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rMpvPes;

    ASSERT(prPes != NULL);
    x_memset((void*)&rMpvPes, 0, sizeof(VDEC_PES_INFO_T)); //reset structure passed to vdec_driver

    switch (prPes->u4FrameType)
    {
    case PIC_TYPE_I:
        rMpvPes.ucPicType = MPV_I_TYPE;
        break;

    case PIC_TYPE_P:
        rMpvPes.ucPicType = MPV_P_TYPE;
        break;

    case PIC_TYPE_B:
        rMpvPes.ucPicType = MPV_B_TYPE;
        break;

    default:
    	  // might sequence end, h264 picture type now.
    	 	return TRUE;
        //ASSERT(0);
        //break;
    }

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;
    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;
    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    rMpvPes.fgGop = prPes->fgGop;
    rMpvPes.fgEos = prPes->fgEOS;

    //return (MPV_CallBackEsm(&rMpvPes) == 1);
    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}

//-----------------------------------------------------------------------------
static BOOL _H264VideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T rVdecPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);
	x_memset((void*)&rVdecPes, 0, sizeof(VDEC_PES_INFO_T)); //reset structure passed to vdec_driver

    u4Addr = prPes->u4FrameAddr + 3;
    if(u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rVdecPes.ucPicType = *(UINT8*)VIRTUAL(u4Addr);

    rVdecPes.ucMpvId = prPes->u1DeviceId;
    rVdecPes.ucEsId = prPes->u1Channel;
    rVdecPes.u4FifoStart = prPes->u4BufStart;
    rVdecPes.u4FifoEnd = prPes->u4BufEnd;

    u4Addr = prPes->u4FrameAddr + 4;
    if(u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rVdecPes.u4VldReadPtr = u4Addr;

    rVdecPes.u4PTS = prPes->u4Pts;
    rVdecPes.u4DTS = prPes->u4Dts;
    rVdecPes.fgDtsValid = prPes->fgPtsDts;
    //rVdecPes.fgSeqHdr = prPes->fgSeqHeader;
    //rVdecPes.fgGop = prPes->fgGop;

    rVdecPes.fgEos = prPes->fgEOS;
    rVdecPes.u1SliceCount = prPes->u1SliceCount;

    return (VDEC_SendEs((void *)&rVdecPes) == 1);
}

#if defined(CC_MT5365) || defined(CC_MT5395) ||  defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
static BOOL _AVSVideoCallback(const DMX_PES_MSG_T* prPes)
{
    VDEC_PES_INFO_T  rMpvPes;
    UINT32 u4Addr;

    ASSERT(prPes != NULL);

    x_memset((void*)&rMpvPes, 0, sizeof(rMpvPes));

    u4Addr = prPes->u4FrameAddr + 3;
    if(u4Addr >= prPes->u4BufEnd)
    {
        u4Addr -= (prPes->u4BufEnd - prPes->u4BufStart);
    }
    rMpvPes.ucPicType = *(UINT8*)VIRTUAL(u4Addr);

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;

    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;

    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts; 
    //rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    //rMpvPes.fgGop = prPes->fgGop;

    rMpvPes.fgEos = prPes->fgEOS;

    return (VDEC_SendEs((void *)&rMpvPes) == 1);
}
#endif

//-----------------------------------------------------------------------------
static BOOL _VideoCallback(const DMX_PES_MSG_T* prPes)
{
    BOOL fgRet = TRUE;
#if defined(CC_MT5365) || defined(CC_MT5395)   || defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
    switch(prPes->eVideoType)
#else        
    switch(DMX_GetVideoType())
#endif
    {
    case DMX_VIDEO_MPEG:
        fgRet = _MPEGVideoCallback(prPes);
        break;

#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
    case DMX_VIDEO_AVS:
        return _AVSVideoCallback(prPes);
#endif

    case DMX_VIDEO_H264:
        fgRet = _H264VideoCallback(prPes);
        break;

    default:
        Printf("Unknown video type\n");
        break;
    }

    return fgRet;
}

static void _ResChgNfy(UINT8 u1VideoIndex,
	VDP_SOURCE_INFO_T rInfo)
{
    UINT32 u4Tag;
    PFN_DRV_CALLBACK pfnCallback;
    DRV_VDO_PARAM_T *prParam;
    DRV_VDO_INTERNAL_T *prInternal;   

    _DRV_VDO_GetParameter(u1VideoIndex, &prParam);          	
    _DRV_VDO_GetInternal(u1VideoIndex, &prInternal);          

    prInternal->rInfo.u2Width = rInfo.u4SrcWidth;
    prInternal->rInfo.u2Height = rInfo.u4SrcHeight;    
    prInternal->rInfo.u2FramePerSecond = rInfo.u4FrameRate;
    prInternal->rInfo.fgProgressive = !((BOOL)rInfo.u4Interlace);
    
    prInternal->rInfo.fgVgaTiming   = FALSE;
    prInternal->rInfo.rSigStatus    = DRV_VDO_SIGNAL_DETECTED;        
    
    pfnCallback = prParam->rCallbackList.pfnResChange; 
    if(pfnCallback != NULL)
    {
        u4Tag = (UINT32)prInternal->rInfo.rSigStatus;
        pfnCallback(u4Tag);
    }
    
}

/*
static void _VdpNotify(UCHAR ucVdpId, UINT32 u4SrcWidth, UINT32 u4SrcHeight, 
    UINT32 u4OutWidth, UINT32 u4OutHeight)
{
    //UINT8 u1VideoIndex = (UINT8)((UINT32)pvNfyTag & 0xff); 
    UINT8 u1VideoIndex;
    UINT32 u4Tag;
    PFN_DRV_CALLBACK pfnCallback;
    DRV_VDO_PARAM_T *prParam;
    DRV_VDO_INTERNAL_T *prInternal;   
    MPV_ASPECT_RATIO_T rAspectRatio;

    u1VideoIndex = ucVdpId;
    _DRV_VDO_GetParameter(u1VideoIndex, &prParam);          	
    _DRV_VDO_GetInternal(u1VideoIndex, &prInternal);          

    MPV_GetResolution(u1VideoIndex, &prInternal->rInfo.u2Width, &prInternal->rInfo.u2Height,
    
        &prInternal->rInfo.fgProgressive, &rAspectRatio, &prInternal->rInfo.u2FramePerSecond); 

    prInternal->rInfo.u2Width = (UINT16)u4SrcWidth;
    prInternal->rInfo.u2Height = (UINT16)u4SrcHeight;
    
    prInternal->rInfo.fgVgaTiming   = FALSE;
    prInternal->rInfo.rSigStatus    = DRV_VDO_SIGNAL_DETECTED;        
    
    pfnCallback = prParam->rCallbackList.pfnResChange; 
    if(pfnCallback != NULL)
    {
        u4Tag = (UINT32)prInternal->rInfo.rSigStatus;
        pfnCallback(u4Tag);
    }
    
}
*/

/*
static void _MpegCcNfy(void* pvTag,
    MPV_CC_COND_T rCcCond, 
    void* pvArg)
{    
    DRV_VDO_PARAM_T* prParam;
    PFN_DRV_CALLBACK prCallback;
    UINT8 u1VideoIndex = (UINT8)(UINT32)pvTag;
    UINT32 u4Tag = (UINT32)pvArg;    
    
    _DRV_VDO_GetParameter(u1VideoIndex, &prParam);
    prCallback = prParam->rCallbackList.pfnMpegCcData;
    if(prCallback != NULL)
    {
        if(rCcCond == MPV_CC_COND_DATA_ARRIVAL)
        {
            prCallback(u4Tag);     
        }
        else if(rCcCond == MPV_CC_COND_ERROR)
        {
            u4Tag |= (DRV_VDO_MPEGCC_ERROR << 8);
            prCallback(u4Tag);
        }
    }
}
*/

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** _DRV_VDO_DIG_Init
 *  Digital video playback related high level API initialization, excluding driver (VDP, MPV, DMX, AUD)initialization
 *
 *  @param  void
 *
 *  @retval DRV_E_OK	        Succeed
 *  @retval -(DRV_E_FAIL)	        Fail
 */
//-----------------------------------------------------------------------------
DRV_ERR_CODE_T _DRV_VDO_DIG_Init(void)
{
    return DRV_E_OK;
}

DRV_ERR_CODE_T _DRV_VDO_DIG_Start(UINT8 u1VideoIndex)
{
    DRV_VDO_PARAM_T *prParam;
    DRV_VDO_INTERNAL_T *prInternal;    
    //MPV_CC_NFY_INFO_T rCcInfo;
    DMX_PID_T rPid;
    DMX_DECODER_CALLBACKS_T rDecoderCallbacks;
    //UINT32 au4BufStart[2], au4BufEnd[2];
    FBM_POOL_T* prFbmPool;
    //UINT8 u1Pidx;

    VERIFY(_DRV_VDO_GetParameter(u1VideoIndex, &prParam) == DRV_E_OK);
    VERIFY(_DRV_VDO_GetInternal(u1VideoIndex, &prInternal) == DRV_E_OK);
    
    if(prParam->u2Pid == 0)
    {
        return DRV_E_OK;
    }
       
    // Allocate video buffer from FBM
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
    VERIFY(prFbmPool != NULL);
    VERIFY(prFbmPool->u4Addr != NULL);


    x_memset(&rPid, 0, sizeof(DMX_PID_T));
    // Setup buffer attributes
    rPid.fgAllocateBuffer   = FALSE;
    rPid.u4BufAddr          = prFbmPool->u4Addr;
    rPid.fgPrimary          = TRUE;
    rPid.u4BufSize          = prFbmPool->u4Size;
#if defined(CC_MT5360B) && defined(CC_MT5386)    
    rPid.u1TsIndex          = 1;
#else
    rPid.u1TsIndex          = 0;
#endif
    rPid.u1DeviceId         = 0;
    rPid.u1KeyIndex         = 0;
    rPid.fgEnable           = TRUE;
    rPid.u1SteerMode        = DMX_STEER_TO_FTUP;
    rPid.ePcrMode           = DMX_PCR_MODE_NONE;
    rPid.eDescMode          = DMX_DESC_MODE_NONE;
    rPid.pfnNotify          = NULL;
    rPid.pfnScramble     = _pfScrambleChgNfy;    
    rPid.pvNotifyTag        = NULL;
    rPid.ePidType           = DMX_PID_TYPE_ES_VIDEO;  


    VDP_RegCbFunc(VDP_CB_FUNC_SRC_CHG_IND, (UINT32)_ResChgNfy, 0, 0);

    /*
    rCcInfo.pfCcNfyFct      = _MpegCcNfy;
    rInfo.pvTag             = (void*)u1VideoIndex;
    VERIFY(MPV_SetCcNfyFct(u1VideoIndex, &rCcInfo)                  == MPV_DRV_OK);
    
    VERIFY(MPV_AllocCcBuf(u1VideoIndex, DRV_VDO_MPEGCC_FIFO_SIZE)   == MPV_DRV_OK);    
    */
    
    VERIFY(VDP_SetInput(u1VideoIndex, 0, 0)              == VDP_SET_OK);


    _DRV_VDO_DriverUnmute(u1VideoIndex);

	switch(prParam->u4VPIDType)
	{
		case STREAM_TYPE_H264:
		    VDEC_Play(0, VDEC_FMT_H264);
		    DMX_SetVideoType(DMX_VIDEO_H264);
		    break;
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860)||defined(CC_MT5881)
        case STREAM_TYPE_AVS:
		    VDEC_Play(0, VDEC_FMT_AVS);
		    DMX_SetVideoType(DMX_VIDEO_AVS);
		    break;
#endif
		default:
		    VDEC_Play(0, VDEC_FMT_MPV);		
		    DMX_SetVideoType(DMX_VIDEO_MPEG);		    
	}

    //VDEC_Play(0, VDEC_FMT_MPV);


    VDEC_SyncStc(0, 1, 0);

    //DMX_DV_SetMode(FALSE, FALSE, TRUE);
    DMX_SetToDecoder(TRUE);
    rDecoderCallbacks.pfnSendVideoPes   = _VideoCallback;
    rDecoderCallbacks.pfnUpdateVideoWp  = VDEC_SetWptr;
    rDecoderCallbacks.pfnSendAudioPes   = AUD_SendAudioPes;
    DMX_SetDecoderCallbacks(&rDecoderCallbacks);
    rPid.u2Pid = prParam->u2Pid;
    
    if(!_DMX_AllocatePid(&prInternal->u1Pidx))
    {
        return DRV_E_FAIL;
    }    
    if(DRV_DMX_SetPid(prInternal->u1Pidx, (DMX_PID_FLAG_ALL | DMX_PID_FLAG_SCRAMBLE_STATE), &rPid) != DRV_E_OK)
    {
        VDEC_Stop(0);   

        VERIFY(DMX_FreePid(prInternal->u1Pidx));
        return DRV_E_FAIL;
    }

    _DRV_PCR_TrySyncVideo(u1VideoIndex);

#ifndef __MODEL_slt__
    //_VDP_vDrvDIFWOnOff(0, 1);    
#endif

    
    return DRV_E_OK;
}

DRV_ERR_CODE_T _DRV_VDO_DIG_Stop(UINT8 u1VideoIndex)
{
    DMX_PID_T rPid;
    DRV_VDO_PARAM_T *prParam;
    DRV_VDO_INTERNAL_T* prInternal;
    
    _DRV_VDO_GetParameter(u1VideoIndex, &prParam);
    _DRV_VDO_GetInternal(u1VideoIndex, &prInternal);  
    
    if(prInternal->u1State == DRV_VDO_STATE_STOP)
    {
        return DRV_E_OK;
    }
    
    if(prParam->u2Pid == 0)
    {
        return DRV_E_OK;
    }

    VERIFY(VDP_SetInput(u1VideoIndex, 255, 0) == VDP_SET_OK);
    
    _DRV_VDO_DriverMute(u1VideoIndex);
    
    rPid.fgEnable = FALSE;

    VERIFY(DRV_DMX_SetPid(prInternal->u1Pidx, DMX_PID_FLAG_VALID, &rPid)==DRV_E_OK);
    
    VDEC_Stop(0);    

    //MPV_StopCc(u1VideoIndex);       
    //VERIFY(MPV_FreeCcBuf(u1VideoIndex) == MPV_DRV_OK);
    
    _DRV_PCR_Unsync();
    
    VERIFY(DMX_FreePid(prInternal->u1Pidx));
    _DMX_ReleasePid(prInternal->u1Pidx);     
    
    return DRV_E_OK;
}

DRV_ERR_CODE_T _DRV_VDO_GetMpegCc(UINT8 u1VideoIndex, 
	DRV_VDO_MPEGCC_DATA_T* prMpegCcData, UINT32 u4Tag)
{
    MPV_CC_DATA rData;   
    if(u1VideoIndex != (u4Tag & 0xff))
    {
        return DRV_E_FAIL;
    }
    if( ((u4Tag >> 8) & 0xff) == DRV_VDO_MPEGCC_ERROR )
    {
        return DRV_E_BUFFER_OVERFLOW;
    }

#if 1
    MPV_GetCc(0, &rData);
#else
    MPV_GetCc(u1VideoIndex, &rData);
#endif

    prMpegCcData->fgScteCc = rData.fgIsScte;
    prMpegCcData->u4Size = rData.u4Size;
    prMpegCcData->pu1Buf = rData.pucCcData;
    prMpegCcData->u8Pts = rData.u8Pts;
    return DRV_E_OK;
}


/* JUST SEND MESSAGE IN THIS CALLBACK, OTHERWISE THE VIDEO DRIVER WILL MALFUNCTION */
DRV_ERR_CODE_T _DRV_VDO_SetPtsNfy(UINT8 u1VideoIndex, UINT64 u8Pts)
{
    DRV_VDO_PARAM_T* prParam;
    PFN_DRV_CALLBACK pfnCallback;    

    if(u1VideoIndex != 0)
    {
        return DRV_E_INV_ARG;    
    }
    
    _DRV_VDO_GetParameter(u1VideoIndex, &prParam);
    pfnCallback = prParam->rCallbackList.pfnPtsNfy;

    if(prParam->rInputSrc != DRV_IS_MPEG_1 || prParam->rInputSrc != DRV_IS_MPEG_2)
    {
        return DRV_E_INV_ARG;
    }
    
    if(pfnCallback == NULL)
    {
        return DRV_E_INV_ARG;
    }

#if 1
    VDP_RegCbFunc(VDP_CB_FUNC_PTS_IND, (UINT32)pfnCallback, 0, (UINT32)u8Pts);
    
#else    	
    if(VDP_SetPtsCb(u1VideoIndex, pfnCallback, (UINT32)u8Pts) != VDP_SET_OK)
    {
        return DRV_E_FAIL;
    }
#endif    	

    return DRV_E_OK;
}

DRV_ERR_CODE_T _DRV_VDO_SetScrambleChgNfy(void * pvScrambleChgNfy)
{
    _pfScrambleChgNfy = (PFN_DMX_NOTIFY) pvScrambleChgNfy;
    return DRV_E_OK;
}

DRV_ERR_CODE_T _DRV_VDO_GetStc(UINT8 u1VideoIndex, UINT64* pu8Stc)
{   
    if(u1VideoIndex != 0 || pu8Stc == NULL)
    {
        return DRV_E_INV_ARG;
    }
    
#if 1
    // TODO: Should be adde back
    //*pu8Stc = VDEC_GetStc(0);   
    
#else
    *pu8Stc = MPV_GetStc(u1VideoIndex);
#endif
    return DRV_E_OK;
}
