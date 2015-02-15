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
 * $RCSfile: io_mtvenc.c,v $
 * $Revision: #1 $
 * $Author: p4admin $
 * $SWAuthor: wen han $
 * $MD5HEX:  $
 *---------------------------------------------------------------------------*/

/** @file io_mtvenc.c
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtvenc.h"
#include "mtutil.h"
#include <linux/mtal_ioctl.h>

#ifdef CC_SUPPORT_VENC
#include "x_assert.h"
#include "venc_if.h"
#include "venc_drvif.h"
#include "venc_util_drvif.h"
#include "vdec_drvif.h"
#include "vdp_venc_ctrl.h"

#ifdef CC_SUPPORT_MUXER
#include "muxer_if.h"
#endif

// DI control related header
#include "gfx_if.h"

#include "sv_const.h"
#include "drv_video.h"
#include "drv_display.h"
#include "video_def.h"
#include "vdo_if.h"
#include "drv_dvi.h"

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define _VENC_MAX_WIDTH  (1920)
#define _VENC_MAX_HEIGHT (1080)
#define _VENC_MAX_RES (1920 * 1080) //5399 supoorts 1920*1080 
#define _MTVENC_HALFFPS_THRD (1920 * 1080 * 30) //5399 supoorts 1920*1080*30 (1280 * 720 * 30)

#define _MNTOUT_PREDOWN_MAX_WIDTH (1280)
#else
#define _VENC_MAX_WIDTH  (1280)
#define _VENC_MAX_HEIGHT (720)
#define _VENC_MAX_RES (1280 * 736) 
#define _MTVENC_HALFFPS_THRD (1280 * 720 * 30) 
#endif

#define COPYDATA_FROME_DRVIER
#define VENC_KEEPDATA_QSIZE (256)



//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

VENC_BUFMGR_DECLARE(VENC_RZ_FRM, VENC_FRM_T, 10);

VENC_BUFMGR_DECLARE(VENC_FRM, VENC_FRM_T, 10);

typedef struct _MTVENC_INFO_T
{
    BOOL                     fgIsCtrlOn;
    BOOL                     fgIsRunning;
    BOOL                     fgInterlace; // for replay
    BOOL                     fg2Usr;
    BOOL                     fgAddAud;
	BOOL                     fgSecure;
    UCHAR                    ucMxId;
    UCHAR                    ucRzId;

    UINT16                   u2SrcCompId; // for vdec

    UINT32                   u4Width;
    UINT32                   u4Height;
    UINT32                   u4Pitch;
    UINT32                   u4Fps;
    UINT32                   u4Bps;
    UINT32                   u4SrcFps;    // for replay
    UINT32                   u4SrcWidth;  // for replay
    UINT32                   u4SrcHeight; // for replay
    UINT32                   u4SyncMode;
    UINT32                   u4EncType;
    UINT32                   u4Profile;
    UINT32                   u4Level;

    VENC_PROFILE_T           eProfileDef;
    VENC_PIX_FMT_T           ePixFmt;
    MTVENC_SRC_TYPE_T        eSrcType;
    MTVENC_FMT_TYPE_T        eEncFmt;
    MTVENC_SAR_INFO_T        eSarInfo;
    
    VENC_HANDLE              hVEnc;
    VENC_FILTER_T*           ptFilter;
    VENC_BUFMGR_BASE_T*      ptFrmBuffMgr;
    VENC_BUFMGR_BASE_T*      ptResizeFrmBuffMgr;
    INT32 (*SetFmtInfo)      (struct _MTVENC_INFO_T *, VOID *);
    INT32 (*ConnectInput)    (struct _MTVENC_INFO_T *);
    INT32 (*DisconnectInput) (struct _MTVENC_INFO_T *);
    INT32 (*SetInputParam)   (struct _MTVENC_INFO_T *);
} MTVENC_INFO_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

//lint --e{717} do ... while (0);
#define MTVENC_SET_VALIDATE(TYPE)         \
do                                      \
{                                       \
    if (NULL == pvData)            \
    {                                   \
        return MTR_NOT_OK;    \
    }                                   \
                                        \
    if (sizeof(TYPE) > u4Size)  \
    {                                   \
        return MTR_NOT_OK;    \
    }                                   \
} while (0)


//-----------------------------------------------------------------------------
// Imported functions
//-----------------------------------------------------------------------------

extern INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);

//-----------------------------------------------------------------------------
// Static function forward declarations
//-----------------------------------------------------------------------------

static INT32 _MtVEncSetFramerate(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData);


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static MTVENC_INFO_T _arMtVEncInfo[MTVENC_COMP_NS];



//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static INT32 _MtVEncSetApType(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData);

static MTVENC_INFO_T *_MtVEncGetInfoInst(UINT16 u2CompId)
{
    ASSERT((UINT16)MTVENC_COMP_NS > u2CompId);
    return ((UINT16)MTVENC_COMP_NS > u2CompId)? &_arMtVEncInfo[u2CompId] : &_arMtVEncInfo[0];
}


static VOID _MtVEncResetInfoInst(MTVENC_INFO_T *prMtVEncInfo)
{
    if (prMtVEncInfo)
    {
        x_memset((VOID *)prMtVEncInfo, 0, sizeof(MTVENC_INFO_T));
        prMtVEncInfo->hVEnc = VENC_NULL_HANDLE;
        prMtVEncInfo->ucMxId = 0xFF;
        prMtVEncInfo->u4Fps  = 0;
        prMtVEncInfo->eProfileDef = VENC_PROFILE_MAX;
        #if defined(CC_MT5399) || defined(CC_MT5890)|| defined(CC_MT5882)
            vDrvVencVDPSetEncFldType(MTVENC_ENC_TOP_FLD);
        #else
            vDrvVencVDPSetEncFldType(MTVENC_ENC_BOTH_FLD);
        #endif
        
        vDrvVencVDPSetEncInput(E_VDP_VENC_NOT_SELECTED);
    }
}


static BOOL _MTVENC_Destroy(MTVENC_INFO_T *prMtVEncInfo, BOOL fgDiscInput)
{
    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        UINT32 u4Ret;

        //STOP VENC
        if (prMtVEncInfo->fgIsRunning)
        {
            if (fgDiscInput)
            {
                //disconnect  input source
                if (prMtVEncInfo->DisconnectInput && (MTR_OK != prMtVEncInfo->DisconnectInput(prMtVEncInfo)))
                {
                    MTVENC_PRINT("- disconnect input source failed!\n");
                    return FALSE;
                }
            }

            u4Ret = VENC_Stop(prMtVEncInfo->hVEnc);
            if (VENC_OK != u4Ret)
            {
                MTVENC_PRINT("- venc stop fail(%d)\n", u4Ret);
                return FALSE;
            }

            prMtVEncInfo->fgIsRunning = FALSE;
        }

        //CLOSE VENC
        u4Ret = VENC_Close(prMtVEncInfo->hVEnc);
        if (VENC_OK != u4Ret)
        {
            MTVENC_PRINT("- venc close fail(%d)\n", u4Ret);
            return FALSE;
        }
        prMtVEncInfo->hVEnc = VENC_NULL_HANDLE;
    }

    if (prMtVEncInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prMtVEncInfo->ptFrmBuffMgr));
        prMtVEncInfo->ptFrmBuffMgr = NULL;
    }

    if (prMtVEncInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prMtVEncInfo->ptResizeFrmBuffMgr));
        prMtVEncInfo->ptResizeFrmBuffMgr = NULL;
    }

    return TRUE;
}


// map from VENC_H264_PIC_TYPE to MTVENC_H264_PIC_TYPE
static MTVENC_H264_PIC_TYPE _MTVENC_PicTypeMap(VENC_H264_PIC_TYPE ePicType)
{
    MTVENC_H264_PIC_TYPE eType = VENC_PIX_FMT_MAX;

    switch (ePicType)
    {
    default:
        MTVENC_PRINT("Unknown ePicType(%d)\n", ePicType);
        eType = MTVENC_H264_PIC_TYPE_MAX;
        break;
    case VENC_H264_PIC_TYPE_SPS:
        eType = MTVENC_H264_PIC_TYPE_SPS;
        break;
    case VENC_H264_PIC_TYPE_PPS:
        eType = MTVENC_H264_PIC_TYPE_PPS;
        break;
    case VENC_H264_PIC_TYPE_SEI:
        eType = MTVENC_H264_PIC_TYPE_SEI;
        break;
    case VENC_H264_PIC_TYPE_IDR:
        eType = MTVENC_H264_PIC_TYPE_IDR;
        break;
    case VENC_H264_PIC_TYPE_I:
        eType = MTVENC_H264_PIC_TYPE_I;
        break;
    case VENC_H264_PIC_TYPE_P:
        eType = MTVENC_H264_PIC_TYPE_P;
        break;
    case VENC_H264_PIC_TYPE_B:
        eType = MTVENC_H264_PIC_TYPE_B;
        break;
    }

    return eType;
}


static VOID _MtVEncCb(VOID *pvArg, VENC_IN_PIC_T * ptIn, VENC_OUT_PIC_T * ptOut, UINT32 type)
{
    MTVENC_INFO_T *prMtVEncInfo = (MTVENC_INFO_T *)pvArg;
    if (ptIn && (NULL != prMtVEncInfo->ptFrmBuffMgr)) //free input buffer for DI. xiaolei
    {
        VENC_FRM_T rFrm;
        rFrm.u4YAddr  = ptIn->u4YAddr;
        rFrm.u4UVAddr = ptIn->u4CAddr;
        //MTVENC_PRINT("_MtVEncCb _BUFMGR_OBJ_Free Y 0x%x  C 0x %x\n", ptIn->u4YAddr, ptIn->u4CAddr);
        _BUFMGR_OBJ_Free(prMtVEncInfo->ptFrmBuffMgr, (VOID *)&rFrm);
        //Printf("_emu_demo_cb free: src(Y,UV) => (0x%08x, 0x%08x)\n", ptIn->u4YAddr, ptIn->u4CAddr);
    }
#ifdef CC_SUPPORT_MUXER
    if (ptOut && (0xFF != prMtVEncInfo->ucMxId))
    {
        VENC_PES_INFO_T rVEncPes;
        x_memset(&rVEncPes, 0, sizeof(rVEncPes));
        rVEncPes.u4FifoStart = ptOut->u4RStart;
        rVEncPes.u4FifoEnd   = ptOut->u4REnd;
        rVEncPes.u4StartAddr = ptOut->u4Addr;
        rVEncPes.u4FrameSize = ptOut->u4Size;
        rVEncPes.u4VideoType = type;
        rVEncPes.u4Pts       = ptOut->u4Pts;
        rVEncPes.pvTag       = prMtVEncInfo->hVEnc;
        MUXER_SendVideoPes(prMtVEncInfo->ucMxId, &rVEncPes);
    }
#endif

    if (ptOut && prMtVEncInfo->fg2Usr)
    {
        MTVENC_CB_T tVencCb;
        tVencCb.eCond = MTVENC_COND_DATA;
        tVencCb.u.tOut.eType    = _MTVENC_PicTypeMap(type);
        tVencCb.u.tOut.u4Pts    = ptOut->u4Pts;
        tVencCb.u.tOut.u4Addr   = ptOut->u4Addr;
        tVencCb.u.tOut.u4Size   = ptOut->u4Size;
        tVencCb.u.tOut.u4RStart = ptOut->u4RStart;
        tVencCb.u.tOut.u4REnd   = ptOut->u4REnd;

        _CB_PutEvent(CB_MTAL_VENC_DATA_NFY, sizeof(tVencCb), &tVencCb);
    }
}

///// vdp related static function /////////


//////////////////////////////////////////////

static INT32 _MtVEncConnectVDP(MTVENC_INFO_T *prMtVEncInfo)
{
    vDrvVencVDPStart((void *)prMtVEncInfo->hVEnc,
                     (void *)prMtVEncInfo->ptFrmBuffMgr,
                     (void *)prMtVEncInfo->ptResizeFrmBuffMgr,
                     (void *)prMtVEncInfo->ptFilter);
    return MTR_OK;
}


static INT32 _MtVEncDisconnectVDP(MTVENC_INFO_T *prMtVEncInfo)
{    
    vDrvVencVDPStop();
    
    return MTR_OK;
}

static INT32 _MtVEncSetParamVDP(MTVENC_INFO_T *prMtVEncInfo)
{
    vDrvVencVDPSetParam(prMtVEncInfo->eProfileDef, prMtVEncInfo->u4Width, prMtVEncInfo->u4Height);
    return MTR_OK;
}



#ifdef VDEC_VENC_PATH
static INT32 _MtVEncConnectVdec(MTVENC_INFO_T *prMtVEncInfo)
{
    UCHAR ucEsId = prMtVEncInfo->u2SrcCompId;

    if (ucEsId >= (UINT16)VDEC_MAX_ES)
    {
        return MTR_NOT_OK;
    }

    _VDEC_SetVencHandle(
        ucEsId,
        (VOID *)prMtVEncInfo->hVEnc,
        (VOID *)prMtVEncInfo->ptFrmBuffMgr,
        (VOID *)prMtVEncInfo->ptFilter);
    _VDEC_SetVencFrmSz(ucEsId, prMtVEncInfo->u4Width, prMtVEncInfo->u4Height, prMtVEncInfo->u4Width);
    _VDEC_Set2VencFlow(ucEsId, TRUE);

    return MTR_OK;
}


static INT32 _MtVEncDisconnectVdec(MTVENC_INFO_T *prMtVEncInfo)
{
    UCHAR ucEsId = prMtVEncInfo->u2SrcCompId;
    _VDEC_Set2VencFlow(ucEsId, FALSE);

    return MTR_OK;
}


#if 0
static INT32 _MtVEncSetParamVdec(MTVENC_INFO_T *prMtVEncInfo)
{
    return MTR_OK;
}
#endif

#endif /*VDEC_VENC_PATH*/


static INT32 _MtVEncSetH264Info(
    MTVENC_INFO_T *prMtVEncInfo,
    VOID *pvFmt)
{
    UINT32 u4Width, u4Height, u4Fps, u4Bps;
    MTVENC_COMMON_INFO_T  *ptCmmInfo;
    MTVENC_FRAME_RESOLUTION_T  *ptResolution;
	UINT32 u4Profile;
	UINT32 u4Level;
    MTVENC_H264_INFO_T *ptFmtH264 = (MTVENC_H264_INFO_T *)pvFmt;


    if (NULL == prMtVEncInfo)
    {
        MTVENC_PRINT("- prMtVEncInfo is NULL\n");
        return MTR_NOT_OK;
    }

    if (NULL == ptFmtH264)
    {
        MTVENC_PRINT("- ptFmtH264 is NULL\n");
        return MTR_NOT_OK;
    }

    ptCmmInfo = &ptFmtH264->tCmmInfo;
    u4Bps           = ptCmmInfo->u4Bps;
    u4Fps           = ptCmmInfo->u4Fps;
    ptResolution   = &ptCmmInfo->tResolution;
    u4Width         = ptResolution->u4Width;
    u4Height        = ptResolution->u4Height;
	u4Profile       = (UINT32)ptFmtH264->e_profile;
	u4Level         = (UINT32)ptFmtH264->e_level;

    if (prMtVEncInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_RES;
        rRTParm.u2Width     = u4Width;
        rRTParm.u2Height    = u4Height;
        rRTParm.u2Pitch     = VENC_ALIGN_MASK(u4Width, 15);

        //bitrate
        rRTParm.u4Flag     |= VENC_RT_PARM_FLAG_BITRATE;
        rRTParm.u4Bitrate   = u4Bps;

        //framterate
        rRTParm.u4Flag     |= VENC_RT_PARM_FLAG_FRAMERATE;
        rRTParm.u1Framerate = u4Fps;

        if (VENC_OK != VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm))
        {
            return MTR_NOT_OK;
        }
    }
    else
    {
        prMtVEncInfo->u4Profile = 0;
        prMtVEncInfo->u4Level   = 0;
        prMtVEncInfo->u4Width   = u4Width;
        prMtVEncInfo->u4Height  = u4Height;
        prMtVEncInfo->u4Fps     = u4Fps;
        prMtVEncInfo->u4Bps     = u4Bps;
		prMtVEncInfo->u4Profile = u4Profile;
		prMtVEncInfo->u4Level   = u4Level;
    }

    return MTR_OK;
}


static INT32 _MtVEncSetEncFmt(
    MTVENC_INFO_T *prMtVEncInfo,
    MTVENC_FMT_TYPE_T eEncFmt)
{
    UINT32 type = 0;
    switch (eEncFmt)
    {
    default:
        prMtVEncInfo->SetFmtInfo = NULL;
        return MTR_NOT_OK;

    case MTVENC_FMT_H264:
        type = VENC_H264;
        prMtVEncInfo->SetFmtInfo = _MtVEncSetH264Info;
        break;
    }
    prMtVEncInfo->u4EncType = type;
    prMtVEncInfo->eEncFmt = eEncFmt;
    return MTR_OK;
}


static INT32 _MtVEncSetSrcType(
    MTVENC_INFO_T *prMtVEncInfo,
    MTVENC_SRC_TYPE_T eSrcType)
{
    switch (eSrcType)
    {
    default:
        return MTR_NOT_OK;

    case MTVENC_SRC_TYPE_VDEC:
        break;

    case MTVENC_SRC_TYPE_VDP:
        break;
    }

    prMtVEncInfo->eSrcType = eSrcType;

    return MTR_OK;
}


static INT32 _MtVEncSetFmtInfo(
    MTVENC_INFO_T *prMtVEncInfo,
    VOID *pvFmt)
{

    if (NULL == prMtVEncInfo->SetFmtInfo)
    {
        return MTR_NOT_OK;
    }

    return prMtVEncInfo->SetFmtInfo(prMtVEncInfo, pvFmt);
}


static INT32 _MtVEncH264VphonePlay(MTVENC_INFO_T *prMtVEncInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;

    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        MTVENC_PRINT("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prMtVEncInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_VPHONE, FALSE);
    if (VENC_NULL_HANDLE == hVenc)
    {
        MTVENC_PRINT("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prMtVEncInfo->hVEnc = hVenc;

    u4SrcWidth = prMtVEncInfo->u4Width;
    u4SrcHeight = prMtVEncInfo->u4Height;
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);

    u4Value = prMtVEncInfo->u4Fps;
    MTVENC_PRINT("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prMtVEncInfo->u4Bps;
    MTVENC_PRINT("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    MTVENC_PRINT("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prMtVEncInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       MTVENC_PRINT("- Set pixfmt failed(%d)!\n", u4Ret);
       return MTR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prMtVEncInfo;
    rCb.cb     = _MtVEncCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
    prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    ASSERT(prMtVEncInfo->ptFrmBuffMgr);

    _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(u4SrcWidth, 15) * VENC_ALIGN_MASK(u4SrcHeight, 31);
    u4Size = MAX(u4Size, _VENC_MAX_RES); //for changing res in runtime
    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 4; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    prMtVEncInfo->fgIsRunning = TRUE;

    return VENC_OK;
}


static INT32 _MtVEncH264AtvPvrPlay(MTVENC_INFO_T *prMtVEncInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth;
    UINT32 u4SrcHeight;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    VDP_VENC_INPUT_RES_t tInputRes;
    UINT32 u4Fps;
    
    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        MTVENC_PRINT("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prMtVEncInfo->hVEnc);
        return VENC_DEV_BUSY;
    }

    vDrvVencVDPGetEncInRes(&tInputRes);
    prMtVEncInfo->u4SrcWidth   = tInputRes.u4EncInWidth;
    prMtVEncInfo->u4SrcHeight  = tInputRes.u4EncInHeight;
    prMtVEncInfo->fgInterlace  = tInputRes.fgInterlace;
    prMtVEncInfo->u4SrcFps     = tInputRes.u4Fps;
    u4Fps                      = tInputRes.u4Fps;
    if (tInputRes.fgInterlace)
    {
        u4Fps >>= 1;
        prMtVEncInfo->u4SrcFps >>= 1;
        if (bDrvVencVDPIsOneFldMode())
        {
            prMtVEncInfo->fgInterlace = FALSE;
        }
    }
    u4SrcWidth  = tInputRes.u4EncInWidth;
    u4SrcHeight = tInputRes.u4EncInHeight;
    
    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_PVR, prMtVEncInfo->fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        MTVENC_PRINT("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prMtVEncInfo->hVEnc = hVenc;

    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);    


    MTVENC_PRINT("- AtvPvrPlay EncW %d, EncH %d \n",u4SrcWidth,u4SrcHeight);

    if ( u4Fps > 90 ) // our decoder cannot support higher frame rate
    {
       u4Fps >>= 1;     
    }   
    if (u4SrcWidth * u4SrcHeight * u4Fps > _MTVENC_HALFFPS_THRD)
    {
        u4Fps >>= 1;
    }
    MTVENC_PRINT("- drop frame M = %d, N = %d, \n", prMtVEncInfo->u4SrcFps , u4Fps);
    vDrvVencVDPDropFrame( (u4Fps != prMtVEncInfo->u4SrcFps), prMtVEncInfo->u4SrcFps, u4Fps);
    prMtVEncInfo->u4Fps = u4Fps;
    
    MTVENC_PRINT("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, prMtVEncInfo->u4Fps);

    u4Value = prMtVEncInfo->u4Fps;
    MTVENC_PRINT("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate.
    u4Value = prMtVEncInfo->u4Bps;
    MTVENC_PRINT("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4SrcWidth;
    res.u2Height = (UINT16)u4SrcHeight;
    MTVENC_PRINT("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prMtVEncInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4SrcPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       MTVENC_PRINT("- Set pixfmt failed(%d)!\n", u4Ret);
       return MTR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prMtVEncInfo;
    rCb.cb     = _MtVEncCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
    UINT32 u4VencWidth, u4VencHeight, u4FrameSize;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);


    u4VencWidth = VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4VencHeight = VENC_ALIGN_MASK(u4SrcHeight, 31);
    u4FrameSize = (u4VencWidth * u4VencHeight * 3 * 3)/2;    //(Ysize+Usize) *1.5 *3

    MTVENC_PRINT("FBM Pool Add = 0x%x, Size = %d \n", prFbmPool->u4Addr, prFbmPool->u4Size);
    
    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);

    if (E_VDP_VENC_SUB_DI == eDrvVencVDPGetEncModule())
    {
        //raw data buffer
        ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
        prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
        ASSERT(prMtVEncInfo->ptFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
        u4Size = VENC_ALIGN_MASK(u4SrcWidth, 15) * VENC_ALIGN_MASK(u4SrcHeight, 31);

        for (u4Idx = 2; u4Idx < 8; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
        }
    }
    #if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    else
    {
        MTVENC_PRINT("SetScalerBuf start = 0x%x, VencW = %d, VencH = %d, size = 0x%x \n", u4TmpAddr,u4VencWidth,u4VencHeight,u4FrameSize);
        vDrvVencVDPSetScalerBuf( u4TmpAddr, (u4VencWidth*u4VencHeight));
        u4TmpAddr += u4FrameSize; //3 input buffer
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 63);
    }
    #endif
    
    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    prMtVEncInfo->fgIsRunning = TRUE;

    return VENC_OK;
}


static UINT32 _MtVEncH264MsPlay(MTVENC_INFO_T *prMtVEncInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4Fps; //set default value
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth = 1280; //set default value
    UINT32 u4SrcHeight = 720; //set default value
    UINT32 u4TgtPitch;
    UINT32 u4TgtWidth;
    UINT32 u4TgtHeight;
	#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	BOOL   fgSecure;
	#endif
	VENC_H264_PRAM_PL_T tProfileLevel;
	VENC_H264_SAR_INFO_T tSarInfo;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_CTRL_PIXFMT_T pix;
    VENC_CTRL_RZ_T rRz;
    
    BOOL fgResize = FALSE;
    UINT32 u4GfxConfig = 0;
    UINT32 u4GfxTmpY = 0, u4GfxTmpC = 0;
    BOOL fgInterlace = prMtVEncInfo->fgInterlace;
    VDP_VENC_INPUT_RES_t tInputRes;

    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        MTVENC_PRINT("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prMtVEncInfo->hVEnc);
        return VENC_DEV_BUSY;
    }
    vDrvVencVDPGetEncInRes(&tInputRes);
    prMtVEncInfo->u4SrcWidth   = tInputRes.u4EncInWidth;
    prMtVEncInfo->u4SrcHeight  = tInputRes.u4EncInHeight;
    prMtVEncInfo->u4SrcFps     = tInputRes.u4Fps;
    u4Fps                      = u4DrvVencVDPCtrlCheckFrameRate(prMtVEncInfo->u4Fps, tInputRes.u4Fps);
    fgInterlace                = tInputRes.fgInterlace;
    if (fgInterlace)
    {
        u4Fps >>= 1;
        prMtVEncInfo->u4SrcFps >>= 1;
        if (bDrvVencVDPIsOneFldMode())
        {
            fgInterlace = FALSE;
        }
    }
    prMtVEncInfo->fgInterlace = fgInterlace; 
        
    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_MS, fgInterlace);
    
    if (VENC_NULL_HANDLE == hVenc)
    {
        MTVENC_PRINT("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }

    
    ////////////////////////////
    prMtVEncInfo->hVEnc = hVenc;
    u4SrcWidth  = prMtVEncInfo->u4SrcWidth;
    u4SrcHeight = prMtVEncInfo->u4SrcHeight;
    u4TgtWidth  = prMtVEncInfo->u4Width;
    u4TgtHeight = prMtVEncInfo->u4Height;


    MTVENC_PRINT("- H264MsPlay EncW %d, EncH %d, tgtw %d, tgth %d\n",u4SrcWidth,u4SrcHeight,u4TgtWidth,u4TgtHeight);

    u4TgtPitch = VENC_ALIGN_MASK(u4TgtWidth, 15);
    u4SrcPitch = VENC_ALIGN_MASK(u4SrcWidth, 15);
    
    if ( u4Fps > 90 )   // decoder doesn't support high frame rate
    {
       u4Fps >>= 1;     
    }    
    if (u4TgtWidth * u4TgtHeight * u4Fps > _MTVENC_HALFFPS_THRD)
    {                  //check the maximum bit rate for VENC 
        u4Fps >>= 1;   //then drop the frame.
    }    
    MTVENC_PRINT("- drop frame , M = %d, N = %d, \n", prMtVEncInfo->u4SrcFps , u4Fps);
    vDrvVencVDPDropFrame( (u4Fps != prMtVEncInfo->u4SrcFps), prMtVEncInfo->u4SrcFps, u4Fps);
    prMtVEncInfo->u4Fps = u4Fps;

    if ((u4SrcWidth != u4TgtWidth) || (u4SrcHeight != u4TgtHeight))
    {
        fgResize = TRUE;
        MTVENC_PRINT("- %dx%d resize to %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4TgtWidth, u4TgtHeight, u4Fps);
    }
    else
    {
        MTVENC_PRINT("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4Fps);
    }

#ifdef CC_FLIP_MIRROR_SUPPORT
    if (E_VDP_VENC_OSTG     == eDrvVencVDPGetEncInput()|| 
        E_VDP_VENC_OSTG_OSD == eDrvVencVDPGetEncInput())
    {
        #if !defined(CC_MT5399) && !defined(CC_MT5890) //5399 monitor out will do flip mir
        UINT8 u1FlipMirrorConfig = u1GetFlipMirrorConfig();
        
        if (SYS_MIRROR_CONFIG_ON & u1FlipMirrorConfig)
        {
            u4GfxConfig |= D_GFXFLAG_MIRROR;
            MTVENC_PRINT("- mirror on %d, %d\n", u4GfxConfig, D_GFXFLAG_MIRROR);
        }

        if (SYS_FLIP_CONFIG_ON & u1FlipMirrorConfig)
        {
            u4GfxConfig |= D_GFXFLAG_FLIP;
            MTVENC_PRINT("- flip on %d, %d\n", u4GfxConfig, D_GFXFLAG_FLIP);
        }
        #endif
    }
#endif

    //##framerate. common setting
    u4Value = u4Fps;
    MTVENC_PRINT("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##bitrate. common setting
    u4Value = prMtVEncInfo->u4Bps;
    MTVENC_PRINT("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution. common setting
    res.u2Width  = (UINT16)u4TgtWidth;
    res.u2Height = (UINT16)u4TgtHeight;
    MTVENC_PRINT("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##profile level
    tProfileLevel.u2Profile = prMtVEncInfo->u4Profile;
    tProfileLevel.u2Level = prMtVEncInfo->u4Level;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_PROFILELEVEL, (VOID*)&tProfileLevel);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	//##secure buffer
    fgSecure = prMtVEncInfo->fgSecure;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_SECURE, (VOID*)&fgSecure);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set fgSecure failed(%d)!\n", u4Ret);
        return u4Ret;
    }
#endif

    //##Aspect ratio info
    tSarInfo.u4SarIdc = prMtVEncInfo->eSarInfo.u4SarIdc;
    tSarInfo.u4SarWidth = prMtVEncInfo->eSarInfo.u4SarWidth;
    tSarInfo.u4SarHeight = prMtVEncInfo->eSarInfo.u4SarHeight;
    MTVENC_PRINT("MTVENC_PRAM_ASPECTRATIOIDC, u4SarIdc = %d, ,u4SarWidth:%d, u4SarHeight:%d, u4Ret = %d\n", tSarInfo.u4SarIdc,tSarInfo.u4SarWidth, tSarInfo.u4SarHeight,u4Ret);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_ASPECTRATIOIDC, (VOID*)&tSarInfo);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set Aspect ratio info failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prMtVEncInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4TgtPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       MTVENC_PRINT("- Set pixfmt failed(%d)!\n", u4Ret);
       return MTR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Output Callback
    rCb.pvData = (VOID *)prMtVEncInfo;
    rCb.cb     = _MtVEncCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

//set buffer
{
    UINT32 u4Size = 0;
    UINT32 u4VencWidth ;
    UINT32 u4VencHeight;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr,u4FrameSize;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
    
    u4VencWidth = VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4VencHeight = VENC_ALIGN_MASK(u4SrcHeight, 31);
    u4FrameSize = (u4VencWidth * u4VencHeight * 3 * 3)/2;    //(Ysize+Usize) *1.5 *3

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);
    
    MTVENC_PRINT("FBM Pool Add = 0x%x, Size = %d \n", prFbmPool->u4Addr, prFbmPool->u4Size);
    
    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);

    if (E_VDP_VENC_SUB_DI == eDrvVencVDPGetEncModule())
    {
        //raw data buffer
        ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
        prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
        ASSERT(prMtVEncInfo->ptFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
        u4Size = VENC_ALIGN_MASK(u4TgtWidth, 15) * VENC_ALIGN_MASK(u4TgtHeight, 31);

        for (u4Idx = 2; u4Idx < 4; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
            MTVENC_PRINT("InBuf[%d] Y = 0x%x, C = 0x%x \n", u4Idx, rFrm.u4YAddr, rFrm.u4UVAddr);
        }

        if (fgResize || u4GfxConfig)
        {
            ASSERT(NULL == prMtVEncInfo->ptResizeFrmBuffMgr);
            prMtVEncInfo->ptResizeFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_RZ_FRM);
            ASSERT(prMtVEncInfo->ptResizeFrmBuffMgr);

            _BUFMGR_OBJ_Reset(prMtVEncInfo->ptResizeFrmBuffMgr);
            u4Size = u4SrcPitch * u4SrcHeight;

            for (u4Idx = 0; u4Idx < VENC_RESIZE_BUF_N; u4Idx++)
            {
                rFrm.u4YAddr  = u4TmpAddr;
                u4TmpAddr    += u4Size;
                u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
                rFrm.u4UVAddr = u4TmpAddr;
                u4TmpAddr    += (u4Size >> 1);
                u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
                _BUFMGR_OBJ_Add(prMtVEncInfo->ptResizeFrmBuffMgr, &rFrm);
            }
        }
    }
    #if defined(CC_MT5399) || defined(CC_MT5890)|| defined(CC_MT5882)
    else
    {
        MTVENC_PRINT("SetScalerBuf start = 0x%x, VencW = %d, VencH = %d, size = 0x%x \n", u4TmpAddr,u4VencWidth,u4VencHeight,u4FrameSize);
        vDrvVencVDPSetScalerBuf( u4TmpAddr, (u4VencWidth*u4VencHeight));
        u4TmpAddr += u4FrameSize; //3 input buffer
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 63);

        if (fgResize || u4GfxConfig)
        {
            ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
            prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
            ASSERT(prMtVEncInfo->ptFrmBuffMgr);

            _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
            u4Size = VENC_ALIGN_MASK(u4TgtWidth, 15) * VENC_ALIGN_MASK(u4TgtHeight, 31);

            //u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
            for (u4Idx = 2; u4Idx < 4; u4Idx++)
            {
                rFrm.u4YAddr  = u4TmpAddr;
                u4TmpAddr    += u4Size;
                u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
                rFrm.u4UVAddr = u4TmpAddr;
                u4TmpAddr    += (u4Size >> 1);
                u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
                MTVENC_PRINT("[MTVENC] FrmBuffMgr target buf[%d] Y = 0x%x, C = 0x%x \n", u4Idx, rFrm.u4YAddr, rFrm.u4UVAddr);
                _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
            }
        }
    }
    #endif

    if (fgResize && u4GfxConfig)
    {
        u4GfxTmpY = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        u4GfxTmpC = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
    }
    
    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    if (fgResize || u4GfxConfig)
    {
        MTVENC_PRINT("- rz: %d, gfx:0x%08x\n", fgResize, u4GfxConfig);
        rRz.fgResizeSrc = fgResize;
        rRz.u4GfxConfig = u4GfxConfig;
        rRz.u4GfxTmpY   = u4GfxTmpY;
        rRz.u4GfxTmpC   = u4GfxTmpC;
        rRz.u4SrcPitch  = u4SrcPitch;
        rRz.u4SrcWidth  = u4SrcWidth;
        rRz.u4SrcHeight = u4SrcHeight;
        rRz.pvIn        = (void *)prMtVEncInfo->ptResizeFrmBuffMgr;
        rRz.pvOut       = (void *)prMtVEncInfo->ptFrmBuffMgr;
        u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_RZ, (VOID*)&rRz);
        if (VENC_OK != u4Ret)
        {
            MTVENC_PRINT("- Set resize error(%d)!\n", u4Ret);
            return -1;
        }
    }

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }
    
    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    prMtVEncInfo->fgIsRunning = TRUE;

    return VENC_OK;
}

  
static UINT32 _MtVEncH264CustPlay(MTVENC_INFO_T *prMtVEncInfo)
{
    VENC_HANDLE hVenc = NULL;
    UINT32 u4Value;
    UINT32 u4Ret;
    UINT32 u4SrcPitch;
    UINT32 u4SrcWidth = 0;
    UINT32 u4SrcHeight = 0;
    UINT32 u4TgtPitch;
    UINT32 u4TgtWidth;
    UINT32 u4TgtHeight;
    BOOL fgAddAud;
    VENC_CTRL_OUTCB_T rCb;
    VENC_PRAM_RES_T res;
    VENC_H264_PRAM_PL_T tProfileLevel;
    VENC_CTRL_PIXFMT_T pix;
    VENC_CTRL_RZ_T rRz;
    VENC_H264_SAR_INFO_T tSarInfo;
    BOOL fgResize = FALSE;
    VDP_VENC_INPUT_RES_t tInputRes;
    UINT32  u4Fps;

    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        MTVENC_PRINT("- handle of venc is not release yet!\n");
        ASSERT(VENC_NULL_HANDLE == prMtVEncInfo->hVEnc);
        return VENC_DEV_BUSY;
    }


    vDrvVencVDPGetEncInRes(&tInputRes);
    prMtVEncInfo->u4SrcWidth   = tInputRes.u4EncInWidth;
    prMtVEncInfo->u4SrcHeight  = tInputRes.u4EncInHeight;
    prMtVEncInfo->fgInterlace  = tInputRes.fgInterlace;
    prMtVEncInfo->u4SrcFps     = tInputRes.u4Fps;
    u4Fps                      = u4DrvVencVDPCtrlCheckFrameRate(prMtVEncInfo->u4Fps, tInputRes.u4Fps);
    if (tInputRes.fgInterlace)
    {
        u4Fps >>= 1;
        prMtVEncInfo->u4SrcFps >>= 1;
        if (bDrvVencVDPIsOneFldMode())
        {
            prMtVEncInfo->fgInterlace = FALSE;
        }
    }
    
    // special case
    prMtVEncInfo->u4Width  = (0 == prMtVEncInfo->u4Width) ? tInputRes.u4EncInWidth  : prMtVEncInfo->u4Width;
    prMtVEncInfo->u4Height = (0 == prMtVEncInfo->u4Height)? tInputRes.u4EncInHeight : prMtVEncInfo->u4Height;
    
    hVenc = VENC_Open(VENC_H264, VENC_PROFILE_CUST, prMtVEncInfo->fgInterlace);
    if (VENC_NULL_HANDLE == hVenc)
    {
        MTVENC_PRINT("- Open venc fail!\n");
        return VENC_INV_HANDLE;
    }
    prMtVEncInfo->hVEnc = hVenc;
#if 0
    {
        UINT16 u2MsgQIdx;
		MTVENC_CB_T rCbData;
		SIZE_T zMsgSize=sizeof(MTVENC_CB_T);
		while (x_msg_q_receive(&u2MsgQIdx, &rCbData, &zMsgSize,
				&prMtVEncInfo->hDataQ, 1, X_MSGQ_OPTION_NOWAIT) == OSR_OK);
    }
#endif
    u4TgtWidth = prMtVEncInfo->u4Width;
    u4TgtHeight = prMtVEncInfo->u4Height;
    u4TgtPitch = VENC_ALIGN_MASK(u4TgtWidth, 15);
    
	u4SrcWidth  = prMtVEncInfo->u4SrcWidth;
	u4SrcHeight = prMtVEncInfo->u4SrcHeight;
    u4SrcPitch= VENC_ALIGN_MASK(u4SrcWidth, 15);
    

    MTVENC_PRINT("- H264CustPlay u4Profile %d fps %d, fgInterlace %d\n",prMtVEncInfo->u4Profile, prMtVEncInfo->u4Fps, prMtVEncInfo->fgInterlace);
    MTVENC_PRINT("- H264CustPlay EncW %d EncH %d tgtw %d tgth %d\n",u4SrcWidth,u4SrcHeight,u4TgtWidth,u4TgtHeight);
    
    while (u4Fps > 50 ||
           u4TgtWidth * u4TgtHeight * u4Fps > _MTVENC_HALFFPS_THRD)
    {                  //check the maximum bit rate for VENC 
        u4Fps >>= 1;   //then drop the frame.
    }    
    MTVENC_PRINT("- drop frame , M = %d, N = %d, \n", prMtVEncInfo->u4SrcFps , u4Fps);
    vDrvVencVDPDropFrame( (u4Fps != prMtVEncInfo->u4SrcFps), prMtVEncInfo->u4SrcFps, u4Fps);
    prMtVEncInfo->u4Fps = u4Fps;
    

    if ((u4SrcWidth != u4TgtWidth) || (u4SrcHeight != u4TgtHeight))
    {
        fgResize = TRUE;
        MTVENC_PRINT("- %dx%d resize to %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, u4TgtWidth, u4TgtHeight, prMtVEncInfo->u4Fps);
    }
    else
    {
        MTVENC_PRINT("- %dx%d @ %d fps\n", u4SrcWidth, u4SrcHeight, prMtVEncInfo->u4Fps);
    }

    u4Value = prMtVEncInfo->u4Fps;
    MTVENC_PRINT("- set %d fps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_FRAMERATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set framerate failed(%d)!\n", u4Ret);
        return u4Ret;
    }
    //##bitrate.
    u4Value = prMtVEncInfo->u4Bps;
    MTVENC_PRINT("- set %d kbps\n", u4Value);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_BITRATE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set bitrate failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##resolution
    res.u2Width  = (UINT16)u4TgtWidth;
    res.u2Height = (UINT16)u4TgtHeight;
    MTVENC_PRINT("- set %dx%d\n", res.u2Width, res.u2Height);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_RESOLUTION, (VOID*)&res);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }
    //##profile level
    tProfileLevel.u2Profile = prMtVEncInfo->u4Profile;
    tProfileLevel.u2Level = prMtVEncInfo->u4Level;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_PROFILELEVEL, (VOID*)&tProfileLevel);
    fgAddAud = prMtVEncInfo->fgAddAud;
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_ADDAUD, (VOID*)&fgAddAud);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set resolution failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##Aspect ratio info
    tSarInfo.u4SarIdc = prMtVEncInfo->eSarInfo.u4SarIdc;
    tSarInfo.u4SarWidth = prMtVEncInfo->eSarInfo.u4SarWidth;
    tSarInfo.u4SarHeight = prMtVEncInfo->eSarInfo.u4SarHeight;
    MTVENC_PRINT("MTVENC_PRAM_ASPECTRATIOIDC, u4SarIdc = %d, ,u4SarWidth:%d, u4SarHeight:%d, u4Ret = %d\n", tSarInfo.u4SarIdc,tSarInfo.u4SarWidth, tSarInfo.u4SarHeight,u4Ret);
    u4Ret = VENC_SetParam(hVenc, VENC_PRAM_ASPECTRATIOIDC, (VOID*)&tSarInfo);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set Aspect ratio info failed(%d)!\n", u4Ret);
        return u4Ret;
    }

    //##pixel format
    pix.ePixFmt  = prMtVEncInfo->ePixFmt;
    pix.u2Pitch  = (UINT16)u4TgtPitch;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_PIX_FMT, (VOID*)&pix);
    if (VENC_OK != u4Ret)
    {
       MTVENC_PRINT("- Set pixfmt failed(%d)!\n", u4Ret);
       return MTR_NOT_OK;
    }

    //SYNC MODE
    u4Value = (UINT32)VENC_SMODE_ASYNC;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_SYNC_MODE, (VOID*)&u4Value);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set sync mode error(%d)!\n", u4Ret);
        return u4Ret;
    }
	prMtVEncInfo->fg2Usr = TRUE;

    //Output Callback
    rCb.pvData = (VOID *)prMtVEncInfo;
    rCb.cb     = _MtVEncCb;
    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set callback error(%d)!\n", u4Ret);
        return u4Ret;
    }

    //_MtVEncSetApType(prMtVEncInfo, (VOID *)prMtVEncInfo->eProfileDef, 0);

//set buffer
{
    UINT32 u4FrameSize;
    UINT32 u4VencWidth ;
    UINT32 u4VencHeight;
    UINT32 u4Size;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
/// need to assigned buffer to scaler
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    u4VencWidth = VENC_ALIGN_MASK(u4SrcWidth, 15);
    u4VencHeight = VENC_ALIGN_MASK(u4SrcHeight, 31);
    u4FrameSize = (u4VencWidth * u4VencHeight * 3 * 3)/2;    //(Ysize+Usize) *1.5 *3

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);

    if (E_VDP_VENC_SUB_DI == eDrvVencVDPGetEncModule())
    {
        //raw data buffer
        ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
        prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
        ASSERT(prMtVEncInfo->ptFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
        u4Size = VENC_ALIGN_MASK(u4TgtWidth, 15) * VENC_ALIGN_MASK(u4TgtHeight, 31);

        for (u4Idx = 2; u4Idx < 8; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
        }
    }
    #if defined(CC_MT5399) || defined(CC_MT5890)|| defined(CC_MT5882)
    else
    {
        MTVENC_PRINT("SetScalerBuf start = 0x%x, VencW = %d, VencH = %d, size = 0x%x \n", u4TmpAddr,u4VencWidth,u4VencHeight,u4FrameSize);
        vDrvVencVDPSetScalerBuf( u4TmpAddr, (u4VencWidth*u4VencHeight));
        u4TmpAddr += u4FrameSize; //3 input buffer
        u4TmpAddr = VENC_ALIGN_MASK(u4TmpAddr, 63);
    }
    #endif

    if (fgResize)
    {
        ASSERT(NULL == prMtVEncInfo->ptResizeFrmBuffMgr);
        prMtVEncInfo->ptResizeFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_RZ_FRM);
        ASSERT(prMtVEncInfo->ptResizeFrmBuffMgr);

        _BUFMGR_OBJ_Reset(prMtVEncInfo->ptResizeFrmBuffMgr);
        u4Size = 1920 * 1088;

        for (u4Idx = 0; u4Idx < VENC_RESIZE_BUF_N; u4Idx++)
        {
            rFrm.u4YAddr  = u4TmpAddr;
            u4TmpAddr    += u4Size;
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            rFrm.u4UVAddr = u4TmpAddr;
            u4TmpAddr    += (u4Size >> 1);
            u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
            _BUFMGR_OBJ_Add(prMtVEncInfo->ptResizeFrmBuffMgr, &rFrm);
        }
    }
    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set buffer error(%d)!\n", u4Ret);
        return u4Ret;
    }
}

    if (fgResize)
    {
        rRz.u4SrcPitch = u4SrcPitch;
        rRz.u4SrcWidth = u4SrcWidth;
        rRz.u4SrcHeight = u4SrcHeight;
        rRz.pvIn = (void *)prMtVEncInfo->ptResizeFrmBuffMgr;
        rRz.pvOut = (void *)prMtVEncInfo->ptFrmBuffMgr;
        u4Ret = VENC_SetCtrl(hVenc, VENC_CTRL_RZ, (VOID*)&rRz);
        if (VENC_OK != u4Ret)
        {
            MTVENC_PRINT("- Set resize error(%d)!\n", u4Ret);
            return -1;
        }
    }

    //Init
    u4Ret = VENC_Init(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Init fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    //Run
    u4Ret = VENC_Run(hVenc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Run fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}

static INT32 _MtVEncH264Play(MTVENC_INFO_T *prMtVEncInfo)
{
    UINT32 u4Ret;

    
    //set input source parameters
    if (prMtVEncInfo->SetInputParam && MTR_OK != (u4Ret = prMtVEncInfo->SetInputParam(prMtVEncInfo)))
    {
        if (MTR_ERR_INV == u4Ret)
        {
            MTVENC_PRINT("- Signal Not Stable Return OK !\n");
            return MTR_OK;
        }
        else
        {
            MTVENC_PRINT("- Set input source parameters failed!\n");
            return MTR_NOT_OK;
        }   
    }

    switch (prMtVEncInfo->eProfileDef)
    {
    default:
        MTVENC_PRINT("- unexpected profile(%d)!\n", prMtVEncInfo->eProfileDef);
        ASSERT(0);
        return MTR_NOT_OK;

    case VENC_PROFILE_VPHONE:
        u4Ret = _MtVEncH264VphonePlay(prMtVEncInfo);
        break;

    case VENC_PROFILE_PVR:
        u4Ret = _MtVEncH264AtvPvrPlay(prMtVEncInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _MtVEncH264MsPlay(prMtVEncInfo);
        break;
    case VENC_PROFILE_CUST:
        u4Ret = _MtVEncH264CustPlay(prMtVEncInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- play fail %d!\n", u4Ret);
        ASSERT(0);
        return MTR_NOT_OK;
    }

    //##connect input source
    if (prMtVEncInfo->ConnectInput && (MTR_OK != prMtVEncInfo->ConnectInput(prMtVEncInfo)))
    {
        MTVENC_PRINT("- Connect to input source failed!\n");
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static INT32 _MtVEncH264Stop(MTVENC_INFO_T *prMtVEncInfo)
{
    UINT32 u4Ret;
    INT32 i4Ret = MTR_OK;

    //disconnect  input source
    if (prMtVEncInfo->DisconnectInput && (MTR_OK != prMtVEncInfo->DisconnectInput(prMtVEncInfo)))
    {
        MTVENC_PRINT("- disconnect input source failed!\n");
        return MTR_NOT_OK;
    }

    //stop
    u4Ret = VENC_Stop(prMtVEncInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- venc stop failed!\n");
        i4Ret = MTR_NOT_OK;
    }
    prMtVEncInfo->fgIsRunning = FALSE;

    //CLOSE VENC
    u4Ret = VENC_Close(prMtVEncInfo->hVEnc);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- venc close fail(%d)\n", u4Ret);
        i4Ret = MTR_NOT_OK;
    }
    prMtVEncInfo->hVEnc = VENC_NULL_HANDLE;

    if (prMtVEncInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prMtVEncInfo->ptFrmBuffMgr));
        prMtVEncInfo->ptFrmBuffMgr = NULL;
    }

    if (prMtVEncInfo->ptResizeFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_RZ_FRM, (VENC_BUFMGR_T(VENC_RZ_FRM) *)(prMtVEncInfo->ptResizeFrmBuffMgr));
        prMtVEncInfo->ptResizeFrmBuffMgr = NULL;
    }

    return i4Ret;
}


static INT32 _MtVEncSetCtrl(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    BOOL fgCtrl;
    INT32 i4Ret = MTR_OK;

    fgCtrl = (pvData)? TRUE : FALSE;

    if (prMtVEncInfo->fgIsCtrlOn != fgCtrl)
    {
        if (fgCtrl)
        {
            switch (prMtVEncInfo->eEncFmt)
            {
            default:
                MTVENC_PRINT("- unexpected type(%d)!\n", prMtVEncInfo->eEncFmt);
                ASSERT(0);
                return MTR_NOT_OK;

            case MTVENC_FMT_H264:
                i4Ret = _MtVEncH264Play(prMtVEncInfo);
                break;
            }
        }
        else
        {
            switch (prMtVEncInfo->eEncFmt)
            {
            default:
                MTVENC_PRINT("- unexpected type(%d)!\n", prMtVEncInfo->eEncFmt);
                ASSERT(0);
                return MTR_NOT_OK;

            case MTVENC_FMT_H264:
                i4Ret = _MtVEncH264Stop(prMtVEncInfo);
                break;
            }
        }
        prMtVEncInfo->fgIsCtrlOn = fgCtrl;
    }

    return i4Ret;
}


static UINT32 _MtVEncH264AtvPvrRePlay(MTVENC_INFO_T *prMtVEncInfo)
{
    UINT32 u4Ret;

    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        // stop DI write mode
        vDrvVencCtrlStopModule();

        if (!_MTVENC_Destroy(prMtVEncInfo, FALSE))
        {
            return VENC_INTERNAL_ERR;
        }
    }

    u4Ret = _MtVEncH264AtvPvrPlay(prMtVEncInfo);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- _MtVEncH264AtvPvrPlay fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}


static UINT32 _MtVEncH264MsRePlay(MTVENC_INFO_T *prMtVEncInfo)
{
    UINT32 u4Ret;
    VDP_VENC_INPUT_RES_t tRes;
    
    vDrvVencVDPGetEncInRes(&tRes);
    if (prMtVEncInfo->u4SrcWidth    == tRes.u4EncInWidth  &&
        prMtVEncInfo->u4SrcHeight   == tRes.u4EncInHeight &&
        prMtVEncInfo->fgInterlace   == tRes.fgInterlace   )
    {
        _MtVEncSetFramerate(prMtVEncInfo, (VOID *)prMtVEncInfo->u4Fps);
        return VENC_OK;
    }

    if (VENC_NULL_HANDLE != prMtVEncInfo->hVEnc)
    {
        vDrvVencCtrlStopModule();

        if (!_MTVENC_Destroy(prMtVEncInfo, FALSE))
        {
            return VENC_INTERNAL_ERR;
        }
    }

    u4Ret = _MtVEncH264MsPlay(prMtVEncInfo);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- _MtVEncH264MsPlay fail(%d)!\n", u4Ret);
        return u4Ret;
    }

    return VENC_OK;
}


static INT32 _MtVEncH264RePlay(MTVENC_INFO_T *prMtVEncInfo)
{
    UINT32 u4Ret;

    //set input source parameters
    if (prMtVEncInfo->SetInputParam && MTR_OK != (u4Ret = prMtVEncInfo->SetInputParam(prMtVEncInfo)))
    {
        if (MTR_ERR_INV == u4Ret)
        {
            MTVENC_PRINT("- Signal Not Stable Return OK !\n");
            return MTR_OK;
        }
        else
        {
            MTVENC_PRINT("- Set input source parameters failed!\n");
            return MTR_NOT_OK;
        }   
    }

    MTVENC_PRINT("- profile(%d)\n", prMtVEncInfo->eProfileDef);
    switch (prMtVEncInfo->eProfileDef)
    {
    default:
        MTVENC_PRINT("- unexpected profile(%d)!\n", prMtVEncInfo->eProfileDef);
        ASSERT(0);
        return MTR_NOT_OK;
#if 0 // not used yet
    case VENC_PROFILE_VPHONE:
        u4Ret = _MtVEncH264VphonePlay(prMtVEncInfo);
        break;
#endif
    case VENC_PROFILE_PVR:
        u4Ret = _MtVEncH264AtvPvrRePlay(prMtVEncInfo);
        break;

    case VENC_PROFILE_MS:
        u4Ret = _MtVEncH264MsRePlay(prMtVEncInfo);
        break;
    }

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("-play fail %d!\n", u4Ret);
        ASSERT(0);
        return MTR_NOT_OK;
    }

    //##connect input source
    if (prMtVEncInfo->ConnectInput && (MTR_OK != prMtVEncInfo->ConnectInput(prMtVEncInfo)))
    {
        MTVENC_PRINT("-Connect to input source failed!\n");
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static INT32 _MtVEncSetRestart(MTVENC_INFO_T *prMtVEncInfo)
{
    INT32 i4Ret = MTR_OK;

    if (!prMtVEncInfo->fgIsCtrlOn)
    {
        MTVENC_PRINT(" - ctrl is off\n");
        return MTR_NOT_OK;
    }
    
    switch (prMtVEncInfo->eEncFmt)
    {
    default:
        MTVENC_PRINT("- unexpected type(%d)!\n", prMtVEncInfo->eEncFmt);
        ASSERT(0);
        return MTR_NOT_OK;

    case MTVENC_FMT_H264:
        i4Ret = _MtVEncH264RePlay(prMtVEncInfo);
        break;
    }

    return i4Ret;
}


static INT32 _MtVEncSetSettings(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData, SIZE_T u4Size)
{
    INT32 i4Ret = MTR_NOT_OK;
    MTVENC_SETTINGS_T rSettings;

    MTVENC_SET_VALIDATE(MTVENC_SETTINGS_T);

    if (prMtVEncInfo->fgIsRunning)
    {
        MTVENC_PRINT("-venc is running\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTVENC_SETTINGS_T);
    COPY_FROM_USER_ARG(pvData, rSettings, MTVENC_SETTINGS_T);

    i4Ret = _MtVEncSetEncFmt(prMtVEncInfo, rSettings.eEncFmt);
    if (i4Ret != MTR_OK)
    {
        MTVENC_PRINT("- error enc type [%d]\n", rSettings.eEncFmt);
        return i4Ret;
    }

    i4Ret = _MtVEncSetSrcType(prMtVEncInfo, rSettings.eSrcType);
    if (i4Ret != MTR_OK)
    {
        MTVENC_PRINT("- error src type [%d]\n", rSettings.eSrcType);
        return i4Ret;
    }

    i4Ret = _MtVEncSetFmtInfo(prMtVEncInfo, (VOID *)&rSettings.uFmt);
    if (i4Ret != MTR_OK)
    {
        MTVENC_PRINT("-error fmt info\n");
        return i4Ret;
    }

    return MTR_OK;
}


static INT32 _MtVEncSetFrameResolution(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData, SIZE_T u4Size)
{
    MTVENC_FRAME_RESOLUTION_T tResolution;
    UINT32 u4Width, u4Height;
    MTVENC_SET_VALIDATE(MTVENC_FRAME_RESOLUTION_T);

    USR_SPACE_ACCESS_VALIDATE_ARG(pvData, MTVENC_FRAME_RESOLUTION_T);
    COPY_FROM_USER_ARG(pvData, tResolution, MTVENC_FRAME_RESOLUTION_T);

    u4Width  = tResolution.u4Width;
    u4Height = tResolution.u4Height;
    prMtVEncInfo->u4Width  = u4Width;
    prMtVEncInfo->u4Height = u4Height;

    if (prMtVEncInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
        rRTParm.u2Width  = u4Width;
        rRTParm.u2Height = u4Height;
        rRTParm.u2Pitch  = VENC_ALIGN_MASK(u4Width, 15);

        if (VENC_OK != VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm))
        {
            return MTR_NOT_OK;
        }
        MTVENC_PRINT("- SetFrameResolution width:%d height:%d\n",rRTParm.u2Width,rRTParm.u2Height);
    }

    return MTR_OK;
}


static INT32 _MtVEncSetFramerate(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    UINT32 u4Fps = (UINT32)pvData;

    if (!u4Fps) // fps could not be 0
    {
        return MTR_NOT_OK;
    }

    prMtVEncInfo->u4Fps = u4Fps;

    if (prMtVEncInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        //RESOLUTION
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
        rRTParm.u1Framerate = (UINT8)u4Fps;

        if (VENC_OK != VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm))
        {
            return MTR_NOT_OK;
        }
        MTVENC_PRINT("- SetFramerate:%d\n",rRTParm.u1Framerate);
    }

    return MTR_OK;
}



static INT32 _MtVEncSetBitrate(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    UINT32 u4Bps = (UINT32)pvData;

    if (!u4Bps) // bitrate could not be 0
    {
        return MTR_NOT_OK;
    }

    prMtVEncInfo->u4Bps = u4Bps;

    if (prMtVEncInfo->fgIsRunning)
    {
        VENC_RT_PARM_T rRTParm;
        rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
        rRTParm.u4Bitrate   = u4Bps;

        if (VENC_OK != VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm))
        {
            return MTR_NOT_OK;
        }
        MTVENC_PRINT("- SetBitrate:%d\n",rRTParm.u4Bitrate);
    }

    return MTR_OK;
}


static INT32 _MtVEncSetApType(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    switch ((MTVENC_AP_TYPE_T)pvData)
    {
    default:
        MTVENC_PRINT("unknown type %d\n", pvData);
        return MTR_NOT_OK;

    case MTVENC_AP_TYPE_SKYPE:
        MTVENC_PRINT("MTVENC_AP_TYPE_SKYPE\n");
        prMtVEncInfo->eProfileDef     = VENC_PROFILE_VPHONE;
        prMtVEncInfo->ePixFmt         = VENC_PIX_FMT_NV12;
        #ifdef VDEC_VENC_PATH
        prMtVEncInfo->ConnectInput    = _MtVEncConnectVdec;
        prMtVEncInfo->DisconnectInput = _MtVEncDisconnectVdec;
        prMtVEncInfo->SetInputParam   = NULL;
        #endif /*VDEC_VENC_PATH*/
        break;

    case MTVENC_AP_TYPE_BGPVR:
        MTVENC_PRINT("MTVENC_AP_TYPE_BGPVR\n");
        prMtVEncInfo->eProfileDef     = VENC_PROFILE_PVR;
        prMtVEncInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prMtVEncInfo->ConnectInput    = _MtVEncConnectVDP;
        prMtVEncInfo->DisconnectInput = _MtVEncDisconnectVDP;
        prMtVEncInfo->SetInputParam   = _MtVEncSetParamVDP;
        break;

    case MTVENC_AP_TYPE_ATVPVR:
        MTVENC_PRINT("MTVENC_AP_TYPE_ATVPVR\n");
        prMtVEncInfo->eProfileDef     = VENC_PROFILE_PVR;
        prMtVEncInfo->ePixFmt         = VENC_PIX_FMT_PSCAN;
        prMtVEncInfo->ConnectInput    = _MtVEncConnectVDP;
        prMtVEncInfo->DisconnectInput = _MtVEncDisconnectVDP;
        prMtVEncInfo->SetInputParam   = _MtVEncSetParamVDP;
        break;

    case MTVENC_AP_TYPE_MSI:
        MTVENC_PRINT("MTVENC_AP_TYPE_MSI\n");
        prMtVEncInfo->eProfileDef     = VENC_PROFILE_MS;
        prMtVEncInfo->ePixFmt         = VENC_PIX_FMT_NV12; //VENC_PIX_FMT_PSCAN;
        prMtVEncInfo->ConnectInput    = _MtVEncConnectVDP;
        prMtVEncInfo->DisconnectInput = _MtVEncDisconnectVDP;
        prMtVEncInfo->SetInputParam   = _MtVEncSetParamVDP;
        break;
    case MTVENC_AP_TYPE_CUST:
        MTVENC_PRINT("MTVENC_AP_TYPE_CUST\n");
        prMtVEncInfo->eProfileDef     = VENC_PROFILE_CUST;
        prMtVEncInfo->ePixFmt         = VENC_PIX_FMT_NV12;
        prMtVEncInfo->ConnectInput    = _MtVEncConnectVDP;
        prMtVEncInfo->DisconnectInput = _MtVEncDisconnectVDP;
        prMtVEncInfo->SetInputParam   = _MtVEncSetParamVDP;
        break;
    }

    return MTR_OK;
}


static INT32 _MtVEncSetInsertKeyframe(MTVENC_INFO_T *prMtVEncInfo)
{
    if (prMtVEncInfo->fgIsRunning)
    {
        VENC_RT_CTRL_T rRTCtrl;
        rRTCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME;
        VENC_SetRtCtrlAsync(prMtVEncInfo->hVEnc, &rRTCtrl);
        return MTR_OK;
    }

    return MTR_NOT_OK;
}

static INT32 _MtVEncSetSecure(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
	BOOL fgSecure = (UINT32)pvData;

	prMtVEncInfo->fgSecure = fgSecure;

	MTVENC_PRINT("- fgSecure:%d\n", prMtVEncInfo->fgSecure);
	return MTR_OK;
}


static INT32 _MtVEncSetEncodeModule(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    #if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
    
    E_VDP_VENC_MODULE eEncModule = (E_VDP_VENC_MODULE) pvData;
    
    vDrvVencVDPSetEncModule(eEncModule);
     
    MTVENC_PRINT(" - encoder module %d\n", (UINT8)eEncModule);
    
    #endif
    
    return MTR_OK;
}

static INT32 _MtVEncSetEncodeInput(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    #if defined(CC_MT5399) || defined(CC_MT5890)
    
    E_VDP_VENC_INPUT eEncInput = (E_VDP_VENC_INPUT) pvData;
    
    vDrvVencVDPSetEncInput(eEncInput);
     
    MTVENC_PRINT(" - encoder input from %d\n", (UINT8)eEncInput);
    
    #endif
    
    return MTR_OK;
}

/// coding of this is not good  need to modify AP & driver
static INT32 _MtVEncSetEncodeOsd(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{
    BOOL fgEncOsd;
    fgEncOsd = pvData? TRUE : FALSE; 
	
    if (fgEncOsd)
    {
    	#if defined(CC_MT5399) || defined(CC_MT5890)
        vDrvVencVDPSetEncInput(E_VDP_VENC_OSTG_OSD);//set monitor out mux with OSD
		#endif
		
        #if defined(CC_OSD_ENCODE) && (defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5882)) 
        if (!IS_IC_5880())
        {
        	vCCIROSDOnOff(SV_ON);
        	bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_OSDENCODE1);
            vSetPipFlg(PIP_FLG_MODE_CHG);
            vSetPipFlg(PIP_FLG_MODE_DET_DONE);	
        }
        #endif
    }
    else
    {
    	#if defined(CC_MT5399) || defined(CC_MT5890)
        vDrvVencVDPSetEncInput(E_VDP_VENC_OSTG);////set monitor out mux without OSD
		#endif
		
    	#if defined(CC_OSD_ENCODE) && (defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5882)) 
        if (!IS_IC_5880())
        {
			vCCIROSDOnOff(SV_OFF);
			bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_OSDENCODE1);
            vSetPipFlg(PIP_FLG_MODE_CHG);
            vSetPipFlg(PIP_FLG_MODE_DET_DONE);	
        }
        #endif
    }
    MTVENC_PRINT(" - encoder OSD is %s\n", fgEncOsd? "on" : "off");
    return MTR_OK;
}

/// coding of this is not good  need to modify AP & driver
static INT32 _MtVEncSetMsiOSTG(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{ 
	
    BOOL fgEncOSTG;
    fgEncOSTG = pvData? TRUE : FALSE;

    #if 0
    if(fgEncOSTG)
    {
        vDrvVencVDPSetEncInput(E_VDP_VENC_OSTG);

        #if defined(CC_OSD_ENCODE) && (defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5882))
        if (!IS_IC_5880())
        {
            bApiVideoSetVideoSrc(SV_VP_PIP, SV_VS_OSDENCODE1);
            vSetPipFlg(PIP_FLG_MODE_CHG);
            vSetPipFlg(PIP_FLG_MODE_DET_DONE);
        }
        #endif
    }
    else
    {
        vDrvVencVDPSetEncInput(E_VDP_VENC_NOT_SELECTED);
    }
	#endif
    MTVENC_PRINT(" - not used encode MsiOSTG is %s\n", fgEncOSTG? "on" : "off");
    return MTR_OK;
}

static INT32 _MtVEncSetRecFld(MTVENC_INFO_T *prMtVEncInfo, const VOID* pvData)
{  
    MTVENC_ENC_FLD_TYPE_T eEncFldType = (MTVENC_ENC_FLD_TYPE_T) pvData;

    switch (eEncFldType)
    {
        case MTVENC_ENC_TOP_FLD:
            vDrvVencVDPSetEncFldType(E_VDP_VENC_TOP_FLD);
            break;
        case MTVENC_ENC_BOTTOM_FLD:
            vDrvVencVDPSetEncFldType(E_VDP_VENC_BOTTOM_FLD);
            break;
        case MTVENC_ENC_BOTH_FLD:
            vDrvVencVDPSetEncFldType(E_VDP_VENC_BOTH_FLD);
            break;    
        default:
            vDrvVencVDPSetEncFldType(E_VDP_VENC_PROGRESSIVE);
            break;
    }

    MTVENC_PRINT(" - encode FldType is %d\n", pvData);
    return MTR_OK;
}

static INT32 _MtVEncConnect(
    MTVENC_INFO_T *prMtVEncInfo,
    UINT16           u2CompId)
{
    if (NULL == prMtVEncInfo)
    {
        return MTR_NOT_OK;
    }

    if (!_MTVENC_Destroy(prMtVEncInfo, TRUE))
    {
        return MTR_NOT_OK;
    }

    //init venc MW Info
    _MtVEncResetInfoInst(prMtVEncInfo);
    prMtVEncInfo->u2SrcCompId = u2CompId;

    MTVENC_PRINT("- video encoder connect %d successfully!\n", u2CompId);

    return MTR_OK;
}


static INT32 _MtVEncDisconnect(MTVENC_INFO_T *prMtVEncInfo)
{
    if (!_MTVENC_Destroy(prMtVEncInfo, TRUE))
    {
        return MTR_NOT_OK;
    }

    MTVENC_PRINT("-video encoder disconnect successfully!\n");

    return MTR_OK;
}


static BOOL fg1stCalled = TRUE;

static MT_RESULT_T _MTVENC_Init(void)
{
    UINT32 u2CompId;

    MTVENC_PRINT(" - void\n");

    if (!fg1stCalled)
    {
        return MTR_OK;
    }

    fg1stCalled = FALSE;

    for (u2CompId = 0; u2CompId < MTVENC_COMP_NS; u2CompId++)
    {
        MTVENC_INFO_T *prMtVEncInfo = _MtVEncGetInfoInst(u2CompId);
        _MtVEncResetInfoInst(prMtVEncInfo);
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Termint(void)
{
    MTVENC_PRINT(" - void\n");
    //add driver implementation code here
    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Reset(unsigned long arg)
{
    UINT16 u2CompId;
    MTVENC_INFO_T *prMtVEncInfo;
    u2CompId = (UINT16)arg;
    MTVENC_PRINT(" - u2CompId = %d\n",(int)u2CompId);

    prMtVEncInfo = _MtVEncGetInfoInst(u2CompId);

    //deinit venc mw info
    if (!_MTVENC_Destroy(prMtVEncInfo, TRUE))
    {
        return MTR_NOT_OK;
    }

    _MtVEncResetInfoInst(prMtVEncInfo);

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT16            u2CompId;
    MTVENC_SET_TYPE_T eSetType;
    VOID*             pvData;
    UINT32            u4Size;
    MTVENC_INFO_T*    prMtVEncInfo;
  #ifdef CC_USE_DDI  
    MTVENC_OUTVIDEO_INFO_T rOutVideoInfo;
  #endif
    MT_RESULT_T       eRet = MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eSetType = (MTVENC_SET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];
    u4Size = (UINT32)rArg.ai4Arg[3];

    MTVENC_PRINT(" - u2CompId = %d, eSetType = %d, pvData = %p\n",
        (int)u2CompId, (int)eSetType, pvData);

    //add driver implementation code here

    prMtVEncInfo = _MtVEncGetInfoInst(u2CompId);
    if (NULL == prMtVEncInfo)
    {
        return MTR_NOT_OK;
    }

    switch (eSetType)
    {
    default:
        MTVENC_PRINT("Invalid set_type - %d\n", eSetType);
        return MTR_NOT_OK;

    case MTVENC_SET_TYPE_CONNECT_SRC:
        //eRet = _MtVEncConnect(prMtVEncInfo, (UINT32)pvData);
        eRet = _MtVEncConnect(prMtVEncInfo, 2); //set srcid as monitor_out 
        break;

    case MTVENC_SET_TYPE_DISCONNECT_SRC:
        eRet = _MtVEncDisconnect(prMtVEncInfo);
        break;

    case MTVENC_SET_TYPE_CTRL:
#ifdef CC_USE_DDI
        if(pvData)
        {
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,MTVENC_OUTVIDEO_INFO_T);
            COPY_FROM_USER_ARG(pvData,rOutVideoInfo,MTVENC_OUTVIDEO_INFO_T);
            prMtVEncInfo->eEncFmt = rOutVideoInfo.encFmt;
            prMtVEncInfo->eSrcType = rOutVideoInfo.srcType;
            prMtVEncInfo->fgInterlace = rOutVideoInfo.fgInterlace;
            prMtVEncInfo->u4Bps = rOutVideoInfo.tCommonInfo.u4Bps;
            prMtVEncInfo->u4Fps = rOutVideoInfo.tCommonInfo.u4Fps;
            prMtVEncInfo->u4Height = rOutVideoInfo.tCommonInfo.tResolution.u4Height;
            prMtVEncInfo->u4Width = rOutVideoInfo.tCommonInfo.tResolution.u4Width;
            prMtVEncInfo->u4Profile = rOutVideoInfo.tProfileLevel.e_profile;
            prMtVEncInfo->u4Level = rOutVideoInfo.tProfileLevel.e_level;
            prMtVEncInfo->eSarInfo.u4SarIdc = rOutVideoInfo.tSarInfo.u4SarIdc;
            prMtVEncInfo->eSarInfo.u4SarWidth = rOutVideoInfo.tSarInfo.u4SarWidth;
            prMtVEncInfo->eSarInfo.u4SarHeight = rOutVideoInfo.tSarInfo.u4SarHeight;
            prMtVEncInfo->fgIsRunning = FALSE;
            prMtVEncInfo->fgAddAud = rOutVideoInfo.fgAddAud;
        }
#endif       
        eRet = _MtVEncSetCtrl(prMtVEncInfo, pvData);
#ifdef CC_USE_DDI
		Printf("prMtVEncInfo->hVEnc=0x%x\n",prMtVEncInfo->hVEnc);
        rArg.ai4Arg[0] = (INT32)prMtVEncInfo->hVEnc;
        COPY_TO_USER_ARG(arg,rArg,MTAL_IOCTL_4ARG_T);
#endif
        if (MTR_OK != eRet)
        {
            return eRet;
        }
        break;

    case MTVENC_SET_TYPE_RESTART:
        eRet = _MtVEncSetRestart(prMtVEncInfo);
        break;

    case MTVENC_SET_TYPE_APP_TYPE:
        eRet = _MtVEncSetApType(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_SETTINGS:
        eRet = _MtVEncSetSettings(prMtVEncInfo, pvData, u4Size);
        break;

    case MTVENC_SET_TYPE_ENC_INPUT:
        eRet = _MtVEncSetEncodeInput(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_ENCODE_OSD:
        eRet = _MtVEncSetEncodeOsd(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_MSI_OSTG:
        eRet = _MtVEncSetMsiOSTG(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_ENC_MODULE:
        eRet = _MtVEncSetEncodeModule(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_ENC_FLD:
        eRet = _MtVEncSetRecFld(prMtVEncInfo, pvData);
        break;
        
    //maybe used in runtime
    case MTVENC_SET_TYPE_FRAME_RESOLUTION:
        eRet = _MtVEncSetFrameResolution(prMtVEncInfo, pvData, u4Size);
        break;

    case MTVENC_SET_TYPE_FRAME_RATE:
        eRet = _MtVEncSetFramerate(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_BITRATE:
        eRet = _MtVEncSetBitrate(prMtVEncInfo, pvData);
        break;

    case MTVENC_SET_TYPE_INSERT_KEYFRAME:
        eRet = _MtVEncSetInsertKeyframe(prMtVEncInfo);
        break;

	case MTVENC_SET_TYPE_SECURE:
		eRet = _MtVEncSetSecure(prMtVEncInfo, pvData);
		break;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Get(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT16            u2CompId;
    MTVENC_GET_TYPE_T eGetType;
    VOID*             pvData;
    UINT32*           pu4Size;
    MTVENC_INFO_T*    prMtVEncInfo;
    MT_RESULT_T       eRet = MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    u2CompId = (UINT16)rArg.ai4Arg[0];
    eGetType = (MTVENC_GET_TYPE_T)rArg.ai4Arg[1];
    pvData = (VOID*)rArg.ai4Arg[2];

    MTVENC_PRINT(" - u2CompId = %d, eGetType = %d\n",
        (int)u2CompId, (int)eGetType);

    //add driver implementation code here
    prMtVEncInfo = _MtVEncGetInfoInst(u2CompId);
    if (NULL == prMtVEncInfo)
    {
        return MTR_NOT_OK;
    }

    switch (eGetType)
    {
    default:
        MTVENC_PRINT("Invalid get_type - %d\n", eGetType);
        return MTR_NOT_OK;

    case MTVENC_GET_TYPE_CTRL:
        {
            BOOL fgCtrl = prMtVEncInfo->fgIsCtrlOn;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,fgCtrl);
            COPY_TO_USER_ARG(pvData,fgCtrl,fgCtrl);
            eRet = MTR_OK;
        }
        break;
	case MTVENC_GET_TYPE_VENCBUF:
		{
			FBM_POOL_T *prFbmPool;
            pu4Size = (UINT32 *)rArg.ai4Arg[3];
			prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_VENC);
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            COPY_TO_USER_ARG(pvData,prFbmPool->u4Addr,UINT32);
            USR_SPACE_ACCESS_VALIDATE_ARG(pu4Size,UINT32);
            COPY_TO_USER_ARG(pu4Size,prFbmPool->u4Size,UINT32);
            eRet = MTR_OK;
		}
        break;
     case MTVENC_GET_TYPE_SIGNALREADY:
        {
            UINT32 u4SignalReady=0;
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            //only for sub  how about monitor out ???
            if (bDrvVideoSignalStatus(VDP_2) == SV_VDO_STABLE)
            {
               u4SignalReady=1;
            }
                        
            COPY_TO_USER_ARG(pvData,u4SignalReady,UINT32);
            eRet = MTR_OK;
        }
	    break;
        case MTVENC_GET_TYPE_FRAMERATE:
        {
            UINT32 pvArg = 0;
            VENC_GetParam( prMtVEncInfo->hVEnc,  VENC_PRAM_FRAMERATE, (VOID*)&pvArg);
            Printf("_MTVENC_Get framerate:%d\n",pvArg);
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            COPY_TO_USER_ARG(pvData, pvArg, UINT32);
            eRet = MTR_OK;
        }
        break;
        case MTVENC_GET_TYPE_WIDTH:
        {
            UINT32 pvArg = 0;
            VENC_GetParam( prMtVEncInfo->hVEnc,  VENC_PRAM_WIDTH, (VOID*)&pvArg);
            Printf("_MTVENC_Get width:%d\n",pvArg);
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            COPY_TO_USER_ARG(pvData, pvArg, UINT32);
            eRet = MTR_OK;
        }
        break;
        case MTVENC_GET_TYPE_HEIGHT:
        {
            UINT32 pvArg = 0;
            VENC_GetParam( prMtVEncInfo->hVEnc,  VENC_PRAM_HEIGHT, (VOID*)&pvArg);
            Printf("_MTVENC_Get height:%d\n",pvArg);
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            COPY_TO_USER_ARG(pvData, pvArg, UINT32);
            eRet = MTR_OK;
        }
        break;
        case MTVENC_GET_TYPE_NUM:
        {
            UINT32 pvArg = 0;
            VENC_GetParam( prMtVEncInfo->hVEnc,  VENC_PRAM_NUM, (VOID*)&pvArg);
            Printf("_MTVENC_Get num:%d\n",pvArg);
            USR_SPACE_ACCESS_VALIDATE_ARG(pvData,UINT32);
            COPY_TO_USER_ARG(pvData, pvArg, UINT32);
            eRet = MTR_OK;
        }
	    break;
    }
    return eRet;
}


static MT_RESULT_T _MTVENC_Open(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MT_RESULT_T       eRet = MTR_NOT_OK;
    MTVENC_HANDLE*    phVenc;
    MTVENC_FMT_TYPE_T eEncFmt;
    MTVENC_AP_TYPE_T  eProfile;
    BOOL              fgInterlace;
    VENC_HANDLE hEnc = VENC_NULL_HANDLE;
    UINT32 u4Type;
    VENC_PROFILE_T eProfileSet = VENC_PROFILE_MS;
    MTVENC_INFO_T *prMtVEncInfo;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    phVenc = (MTVENC_HANDLE*)rArg.ai4Arg[0];
    eEncFmt = (MTVENC_FMT_TYPE_T)rArg.ai4Arg[1];
    eProfile = (MTVENC_AP_TYPE_T)rArg.ai4Arg[2];
    fgInterlace = (BOOL)rArg.ai4Arg[3];
    prMtVEncInfo = _MtVEncGetInfoInst(0);

    MTVENC_PRINT(" - phVenc = %d, eEncFmt = %d, eProfile = %d, fgInterlace = %d\n",
        (int)phVenc, (int)eEncFmt, (int)eProfile, (int)fgInterlace);

    //add driver implementation code here
    switch (eEncFmt)
    {
    default:
        MTVENC_PRINT("Unknown eEncFmt(%d)\n", eEncFmt);
        return MTR_NOT_OK;

    case MTVENC_FMT_H264:
        u4Type = VENC_H264;
        break;
    }

    switch (eProfile)
    {
    default:
        MTVENC_PRINT("Unknown eProfile(%d)\n", eProfile);
        return MTR_NOT_OK;

    case MTVENC_AP_TYPE_SKYPE:
        eProfileSet = VENC_PROFILE_VPHONE;
        break;

    case MTVENC_AP_TYPE_ATVPVR:
        eProfileSet = VENC_PROFILE_PVR;
        break;

    case MTVENC_AP_TYPE_MSI:
        eProfileSet = VENC_PROFILE_MS;
        break;
    case MTVENC_AP_TYPE_QQ:
        eProfileSet = VENC_PROFILE_VPHONE;
        break;
    case MTVENC_AP_TYPE_CUST:
        eProfileSet = VENC_PROFILE_CUST;
        break;
    }

    _MTVENC_Reset(0);
    hEnc = VENC_Open(u4Type, eProfileSet, fgInterlace);
    prMtVEncInfo->hVEnc = hEnc;

    if (VENC_NULL_HANDLE != hEnc)
    {
        eRet = MTR_OK;
        USR_SPACE_ACCESS_VALIDATE_ARG(phVenc,VENC_HANDLE);
        COPY_TO_USER_ARG(phVenc,hEnc,VENC_HANDLE);
    }

    return eRet;
}


static MT_RESULT_T _MTVENC_SetParam(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MT_RESULT_T       eRet = MTR_OK;
    VENC_HANDLE hEnc;
    MTVENC_PRAM_T eParam;
    VOID *pvArg;
    UINT32 u4Ret;
    MTVENC_INFO_T *prMtVEncInfo;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    eParam = (MTVENC_PRAM_T)rArg.ai4Arg[1];
    pvArg = (VOID*)rArg.ai4Arg[2];

    MTVENC_PRINT(" - eParam = %d\n", (int)eParam);
    prMtVEncInfo = _MtVEncGetInfoInst(0);

    switch (eParam)
    {
    default:
        MTVENC_PRINT("Unknown eParam(%d)\n", eParam);
        return MTR_NOT_OK;

    case MTVENC_PRAM_FRAMERATE:
        if (!prMtVEncInfo->fgIsRunning)
        {
            UINT32 u4Fps = (UINT32)pvArg;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_FRAMERATE, &u4Fps);
            prMtVEncInfo->u4Fps = u4Fps;
        }
        else
        {
            VENC_RT_PARM_T rRTParm;
            rRTParm.u4Flag      = VENC_RT_PARM_FLAG_FRAMERATE;
            rRTParm.u1Framerate = (UINT8)(UINT32)pvArg;

            u4Ret = (VENC_SMODE_SYNC == prMtVEncInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prMtVEncInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm);
        }
        break;

    case MTVENC_PRAM_BITRATE:
        if (!prMtVEncInfo->fgIsRunning)
        {
            UINT32 u4Bps = (UINT32)pvArg;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_BITRATE, &u4Bps);
            prMtVEncInfo->u4Bps = u4Bps;
        }
        else
        {
            VENC_RT_PARM_T rRTParm;
            rRTParm.u4Flag      = VENC_RT_PARM_FLAG_BITRATE;
            rRTParm.u4Bitrate   = (UINT32)pvArg;

            u4Ret = (VENC_SMODE_SYNC == prMtVEncInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prMtVEncInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm);
        }
        break;

    case MTVENC_PRAM_RESOLUTION:
        if (!prMtVEncInfo->fgIsRunning)
        {
            MTVENC_FRAME_RESOLUTION_T tResUsr;
            VENC_PRAM_RES_T res;

            x_memset(&tResUsr, 0, sizeof(tResUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_FRAME_RESOLUTION_T);
            COPY_FROM_USER_ARG(pvArg,tResUsr,MTVENC_FRAME_RESOLUTION_T);

            res.u2Height = (UINT16)tResUsr.u4Height;
            res.u2Width = (UINT16)tResUsr.u4Width;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_RESOLUTION, &res);
            prMtVEncInfo->u4Width = tResUsr.u4Width;
            prMtVEncInfo->u4Height = tResUsr.u4Height;
        }
        else
        {
            MTVENC_FRAME_RESOLUTION_T tResUsr;
            VENC_RT_PARM_T rRTParm;

            x_memset(&tResUsr, 0, sizeof(tResUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_FRAME_RESOLUTION_T);
            COPY_FROM_USER_ARG(pvArg,tResUsr,MTVENC_FRAME_RESOLUTION_T);

            //RESOLUTION
            rRTParm.u4Flag   = VENC_RT_PARM_FLAG_RES;
            rRTParm.u2Width  = tResUsr.u4Width;
            rRTParm.u2Height = tResUsr.u4Height;
            rRTParm.u2Pitch  = VENC_ALIGN_MASK(tResUsr.u4Width, 15);

            u4Ret = (VENC_SMODE_SYNC == prMtVEncInfo->u4SyncMode)
                ? VENC_SetRtParamSync(prMtVEncInfo->hVEnc, &rRTParm)
                : VENC_SetRtParamAsync(prMtVEncInfo->hVEnc, &rRTParm);
        }
        break;
    case MTVENC_PRAM_PROFILE_LEVEL:
        {
            MTVENC_PROFILE_LEVEL_T tProLevUsr;
            VENC_H264_PRAM_PL_T prolev;
    
            x_memset(&tProLevUsr, 0, sizeof(tProLevUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_PROFILE_LEVEL_T);
            COPY_FROM_USER_ARG(pvArg,tProLevUsr,MTVENC_PROFILE_LEVEL_T);
    
            prolev.u2Profile = (UINT16)tProLevUsr.e_profile;
            prolev.u2Level = (UINT16)tProLevUsr.e_level;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_PROFILELEVEL, &prolev);
            prMtVEncInfo->u4Profile = tProLevUsr.e_profile;
            prMtVEncInfo->u4Level = tProLevUsr.e_profile;
        }
        break;
    case MTVENC_PRAM_ASPECTRATIOIDC:
        {
            MTVENC_SAR_INFO_T tSarInfoUsr;
            VENC_H264_SAR_INFO_T SarInfo;
            
            x_memset(&tSarInfoUsr, 0, sizeof(tSarInfoUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_SAR_INFO_T);
            COPY_FROM_USER_ARG(pvArg,tSarInfoUsr,MTVENC_SAR_INFO_T);
            
            SarInfo.u4SarIdc = tSarInfoUsr.u4SarIdc;
            SarInfo.u4SarWidth = tSarInfoUsr.u4SarWidth;
            SarInfo.u4SarHeight = tSarInfoUsr.u4SarHeight;
            u4Ret = VENC_SetParam(hEnc, VENC_PRAM_ASPECTRATIOIDC, &SarInfo);
            prMtVEncInfo->eSarInfo = tSarInfoUsr;

            MTVENC_PRINT("MTVENC_PRAM_ASPECTRATIOIDC, u4SarIdc = %d, ,u4SarWidth:%d, u4SarHeight:%d, u4Ret = %d\n", SarInfo.u4SarIdc,SarInfo.u4SarWidth, SarInfo.u4SarHeight,u4Ret);
        }
        break;
    }

    if (VENC_OK != u4Ret)
    {
        eRet = MTR_NOT_OK;
        MTVENC_PRINT("VENC_SetParam fail(%d)!!\n", u4Ret);
    }

    return eRet;
}


// map from MTVENC_SMODE_T to VENC_SMODE_T
static VENC_SMODE_T _MTVENC_SyncModeMap(MTVENC_SMODE_T eSmode)
{
    VENC_SMODE_T eType;

    switch (eSmode)
    {
    default:
        MTVENC_PRINT("Unknown eSmode(%d)\n", eSmode);
        eType = VENC_SMODE_DUAL;
        break;
    case MTVENC_SMODE_SYNC:
        eType = VENC_SMODE_SYNC;
        break;
    case MTVENC_SMODE_ASYNC:
        eType = VENC_SMODE_ASYNC;
        break;
    case MTVENC_SMODE_DUAL:
        eType = VENC_SMODE_DUAL;
        break;
    case MTVENC_SMODE_MAX:
        eType = VENC_SMODE_MAX;
        break;
    }

    return eType;
}


// map from MTVENC_PIX_FMT_T to VENC_PIX_FMT_T
static VENC_PIX_FMT_T _MTVENC_PixFmtMap(MTVENC_PIX_FMT_T ePixFmt)
{
    VENC_PIX_FMT_T eType = VENC_PIX_FMT_MAX;

    switch (ePixFmt)
    {
    default:
        MTVENC_PRINT("Unknown ePixFmt(%d)\n", ePixFmt);
        eType = VENC_PIX_FMT_NV12;
        break;
    case MTVENC_PIX_FMT_NV12:
        eType = VENC_PIX_FMT_NV12;
        break;
    }

    return eType;
}


static VOID _MtVenc_GetThumbnailCb(VOID *pvTag, UINT32 u4PhyAddr)
{
    MTVENC_INFO_T *prMtVEncInfo = (MTVENC_INFO_T *)pvTag;
    MTVENC_CB_T tVencCb;

    if (!prMtVEncInfo)
    {
        ASSERT(prMtVEncInfo);
        MTVENC_PRINT("prMtVEncInfo is NULL!!!\n");
        return;
    }

    tVencCb.eCond = MTVENC_COND_THUMBNAIL;
    tVencCb.u.u4ThumbnailPhyAddr = u4PhyAddr;
    _CB_PutEvent(CB_MTAL_VENC_DATA_NFY, sizeof(tVencCb), &tVencCb);
}


static MT_RESULT_T _MTVENC_SetCtrl(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MT_RESULT_T       eRet = MTR_OK;
    VENC_HANDLE hEnc;
    MTVENC_CTRL_T eCtrl;
    VOID *pvArg;
    UINT32 u4Ret;
    MTVENC_INFO_T *prMtVEncInfo;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    eCtrl = (MTVENC_CTRL_T)rArg.ai4Arg[1];
    pvArg = (VOID*)rArg.ai4Arg[2];

    MTVENC_PRINT(" - eCtrl = %d\n", (int)eCtrl);
    prMtVEncInfo = _MtVEncGetInfoInst(0);

    switch (eCtrl)
    {
    default:
        MTVENC_PRINT("Unknown eCtrl(%d)\n", eCtrl);
        return MTR_NOT_OK;

    case MTVENC_CTRL_SYNC_MODE:
        {
            UINT32 u4Value = (UINT32)_MTVENC_SyncModeMap((MTVENC_SMODE_T)pvArg);
            prMtVEncInfo->u4SyncMode = u4Value;
            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_SYNC_MODE, &u4Value);
        }
        break;

    case MTVENC_CTRL_PIX_FMT:
        {
            MTVENC_CTRL_PIXFMT_T tPixUsr;
            VENC_CTRL_PIXFMT_T pix;

            x_memset(&tPixUsr, 0, sizeof(tPixUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_CTRL_PIXFMT_T);
            COPY_FROM_USER_ARG(pvArg,tPixUsr,MTVENC_CTRL_PIXFMT_T);

            pix.ePixFmt = _MTVENC_PixFmtMap(tPixUsr.ePixFmt);
            pix.u2Pitch = (UINT16)tPixUsr.u4Pitch;

            prMtVEncInfo->ePixFmt = pix.ePixFmt;
            prMtVEncInfo->u4Pitch = pix.u2Pitch;

            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_PIX_FMT, &pix);
        }
        break;

    case MTVENC_CTRL_GET_THUMBNAIL:
        {
            MTVENC_CTRL_THUMBNAIL_T rThumbnailUsr;
            VENC_CTRL_THUMBNAIL_T rThumbnail;

            x_memset(&rThumbnailUsr, 0, sizeof(rThumbnailUsr));
            USR_SPACE_ACCESS_VALIDATE_ARG(pvArg,MTVENC_CTRL_PIXFMT_T);
            COPY_FROM_USER_ARG(pvArg,rThumbnailUsr,MTVENC_CTRL_PIXFMT_T);

            rThumbnail.u4Width = rThumbnailUsr.u4Width;
            rThumbnail.u4Height = rThumbnailUsr.u4Height;
            rThumbnail.pvTag = prMtVEncInfo;
            rThumbnail.cb = _MtVenc_GetThumbnailCb;

            u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_GET_THUMBNAIL, &rThumbnail);
        }
        break;
    }

    if (VENC_OK != u4Ret)
    {
        eRet = MTR_NOT_OK;
        MTVENC_PRINT("VENC_SetParam fail(%d)!!\n", u4Ret);
    }

    return eRet;
}


static MT_RESULT_T _MTVENC_DrvInit(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    UINT32 u4Ret;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);

    u4Ret = VENC_Init(hEnc);

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_Init fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Run(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    MTVENC_INFO_T *prMtVEncInfo = _MtVEncGetInfoInst(0);
    UINT32 u4Ret;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);

    u4Ret = VENC_Run(hEnc);

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_Run fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    prMtVEncInfo->fgIsRunning = TRUE;

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Stop(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    MTVENC_INFO_T *prMtVEncInfo = _MtVEncGetInfoInst(0);
    UINT32 u4Ret;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);

    if (prMtVEncInfo->fgIsRunning)
    {
        u4Ret = VENC_Stop(hEnc);

        if (VENC_OK != u4Ret)
        {
            MTVENC_PRINT("VENC_Stop fail(%d)!!\n", u4Ret);
            return MTR_NOT_OK;
        }

        prMtVEncInfo->fgIsRunning = FALSE;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_Close(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    MTVENC_INFO_T *prMtVEncInfo = _MtVEncGetInfoInst(0);
    UINT32 u4Ret;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);

    if (prMtVEncInfo->fgIsRunning)
    {
        u4Ret = VENC_Stop(hEnc);

        if (VENC_OK != u4Ret)
        {
            MTVENC_PRINT("VENC_Stop fail(%d)!!\n", u4Ret);
            return MTR_NOT_OK;
        }

        prMtVEncInfo->fgIsRunning = FALSE;
    }

    u4Ret = VENC_Close(hEnc);

    prMtVEncInfo->hVEnc = VENC_NULL_HANDLE;

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_Close fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}
 

static MT_RESULT_T _MTVENC_EncFrameSync(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Ret;
    MTVENC_IN_PIC_T *ptInUsr, tInKrnl;
    MTVENC_OUT_PIC_T *ptOutUsr, tOutKrnl;
    VENC_IN_PIC_T tIn;
    VENC_OUT_PIC_T tOut;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptInUsr = (MTVENC_IN_PIC_T *)rArg.ai4Arg[1];
    ptOutUsr = (MTVENC_OUT_PIC_T *)rArg.ai4Arg[2];

    x_memset(&tInKrnl, 0, sizeof(tInKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptInUsr,MTVENC_IN_PIC_T);
    COPY_FROM_USER_ARG(ptInUsr,tInKrnl,MTVENC_IN_PIC_T);
    x_memset(&tIn, 0, sizeof(tIn));
    tIn.u4YAddr = tInKrnl.u4YAddr;
    tIn.u4CAddr = tInKrnl.u4CAddr;
    tIn.u4Pts   = tInKrnl.u4Pts;

    x_memset(&tOutKrnl, 0, sizeof(tOutKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,MTVENC_OUT_PIC_T);
    COPY_FROM_USER_ARG(ptOutUsr,tOutKrnl,MTVENC_OUT_PIC_T);
    x_memset(&tOut, 0, sizeof(tOut));
    tOut.u4Pts    = tOutKrnl.u4Pts;
    tOut.u4Addr   = tOutKrnl.u4Addr;
    tOut.u4Size   = tOutKrnl.u4Size;
    tOut.u4RStart = tOutKrnl.u4RStart;
    tOut.u4REnd   = tOutKrnl.u4REnd;

    u4Ret = VENC_EncFrameSync(hEnc, &tIn, &tOut);

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_Run fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    tOutKrnl.u4Pts    = tOut.u4Pts;
    tOutKrnl.u4Addr   = tOut.u4Addr;
    tOutKrnl.u4Size   = tOut.u4Size;
    tOutKrnl.u4RStart = tOut.u4RStart;
    tOutKrnl.u4REnd   = tOut.u4REnd;
    USR_SPACE_ACCESS_VALIDATE_ARG(ptOutUsr,MTVENC_OUT_PIC_T);
    COPY_TO_USER_ARG(ptOutUsr,tOutKrnl,MTVENC_OUT_PIC_T);

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_EncFrameAsync(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Ret;
    MTVENC_IN_PIC_T *ptInUsr, tInKrnl;
    VENC_IN_PIC_T tIn;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptInUsr = (MTVENC_IN_PIC_T *)rArg.ai4Arg[1];

    x_memset(&tInKrnl, 0, sizeof(tInKrnl));
    USR_SPACE_ACCESS_VALIDATE_ARG(ptInUsr,MTVENC_IN_PIC_T);
    COPY_FROM_USER_ARG(ptInUsr,tInKrnl,MTVENC_IN_PIC_T);
    x_memset(&tIn, 0, sizeof(tIn));
    tIn.u4YAddr = tInKrnl.u4YAddr;
    tIn.u4CAddr = tInKrnl.u4CAddr;
    tIn.u4Pts   = tInKrnl.u4Pts;

    u4Ret = VENC_EncFrameAsync(hEnc, &tIn, NULL);

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_Run fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_CreateBuffMgr(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    MTVENC_HANDLE *ptFrmBuffMgrUsr;
    VENC_FRM_T rFrm;
    UINT32 u4Idx, u4TmpAddr, u4Ret, u4Size;
    FBM_POOL_T *prFbmPool;
    VENC_CTRL_BUFF_T buff;
    MTVENC_INFO_T *prMtVEncInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptFrmBuffMgrUsr = (MTVENC_HANDLE *)rArg.ai4Arg[1];

    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
    prMtVEncInfo = _MtVEncGetInfoInst(0);

    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //raw data buffer
    ASSERT(NULL == prMtVEncInfo->ptFrmBuffMgr);
    prMtVEncInfo->ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)VENC_BUFMGR_OBJ_CREATE(VENC_FRM);
    if (NULL == prMtVEncInfo->ptFrmBuffMgr)
    {
        ASSERT(prMtVEncInfo->ptFrmBuffMgr);
        return MTR_NOT_OK;
    }

    _BUFMGR_OBJ_Reset(prMtVEncInfo->ptFrmBuffMgr);
    u4Size = VENC_ALIGN_MASK(prMtVEncInfo->u4Width, 15) * VENC_ALIGN_MASK(prMtVEncInfo->u4Height, 31);

    u4TmpAddr = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    for (u4Idx = 2; u4Idx < 8; u4Idx++)
    {
        rFrm.u4YAddr  = u4TmpAddr;
        u4TmpAddr    += u4Size;
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        rFrm.u4UVAddr = u4TmpAddr;
        u4TmpAddr    += (u4Size >> 1);
        u4TmpAddr     = VENC_ALIGN_MASK(u4TmpAddr, 63);
        _BUFMGR_OBJ_Add(prMtVEncInfo->ptFrmBuffMgr, &rFrm);
    }

    //Working Buffer
    buff.u4Addr  = VENC_ALIGN_MASK(u4TmpAddr, 63);
    buff.u4Size  = prFbmPool->u4Size - (buff.u4Addr - prFbmPool->u4Addr);
    ASSERT(prFbmPool->u4Size > (buff.u4Addr - prFbmPool->u4Addr));

    u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_BUFF, (VOID*)&buff);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("- Set buffer error(%d)!\n", u4Ret);
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(ptFrmBuffMgrUsr,MTVENC_HANDLE);
    COPY_TO_USER_ARG(ptFrmBuffMgrUsr,prMtVEncInfo->ptFrmBuffMgr,MTVENC_HANDLE);

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_DeleteBuffMgr(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    VENC_HANDLE hEnc;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    MTVENC_INFO_T *prMtVEncInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    hEnc = (VENC_HANDLE)rArg.ai4Arg[0];
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[1];

    prMtVEncInfo = _MtVEncGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prMtVEncInfo->ptFrmBuffMgr);
    if (prMtVEncInfo->ptFrmBuffMgr)
    {
        VENC_BUFMGR_OBJ_DELETE(VENC_FRM, (VENC_BUFMGR_T(VENC_FRM) *)(prMtVEncInfo->ptFrmBuffMgr));
        prMtVEncInfo->ptFrmBuffMgr = NULL;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_BUFMGR_OBJ_Get(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    MTVENC_IN_PIC_T *ptDataUsr, tDataKrnl;
    UINT32 u4Option;
    MTVENC_INFO_T *prMtVEncInfo;
    VENC_FRM_T tFrm;
    UINT32 u4Ret = VENC_BUFMGR_FAIL;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[0];
    ptDataUsr = (MTVENC_IN_PIC_T *)rArg.ai4Arg[1];
    u4Option = (UINT32)rArg.ai4Arg[2];

    prMtVEncInfo = _MtVEncGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prMtVEncInfo->ptFrmBuffMgr);
    switch (u4Option)
    {
    default:
        MTVENC_PRINT("Unknown u4Option(%d)\n", u4Option);
        return MTR_NOT_OK;
    case MTVENC_BUFMGR_NOWAIT:
        u4Option = VENC_BUFMGR_NOWAIT;
        break;
    case MTVENC_BUFMGR_WAIT:
        u4Option = VENC_BUFMGR_WAIT;
        break;
    }
    u4Ret = _BUFMGR_OBJ_Get(ptFrmBuffMgr, (VOID *)&tFrm, u4Option);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        MTVENC_PRINT("_BUFMGR_OBJ_Get fail(%d)\n", u4Ret);
        return MTR_NOT_OK;
    }

    tDataKrnl.u4YAddr = tFrm.u4YAddr;
    tDataKrnl.u4CAddr= tFrm.u4UVAddr;
    USR_SPACE_ACCESS_VALIDATE_ARG(ptDataUsr,MTVENC_IN_PIC_T);
    COPY_TO_USER_ARG(ptDataUsr,tDataKrnl,MTVENC_IN_PIC_T);

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_BUFMGR_OBJ_Free(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    VENC_BUFMGR_BASE_T *ptFrmBuffMgr;
    MTVENC_IN_PIC_T *ptDataUsr, tDataKrnl;
    MTVENC_INFO_T *prMtVEncInfo;
    VENC_FRM_T tFrm;
    UINT32 u4Ret = VENC_BUFMGR_FAIL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    ptFrmBuffMgr = (VENC_BUFMGR_BASE_T *)rArg.ai4Arg[0];
    ptDataUsr = (MTVENC_IN_PIC_T *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(ptDataUsr,MTVENC_IN_PIC_T);
    COPY_FROM_USER_ARG(ptDataUsr,tDataKrnl,MTVENC_IN_PIC_T);
    tFrm.u4YAddr = tDataKrnl.u4YAddr;
    tFrm.u4UVAddr = tDataKrnl.u4CAddr;

    prMtVEncInfo = _MtVEncGetInfoInst(0);

    ASSERT(ptFrmBuffMgr == prMtVEncInfo->ptFrmBuffMgr);
    u4Ret = _BUFMGR_OBJ_Free(ptFrmBuffMgr, (VOID *)&tFrm);
    if (VENC_BUFMGR_OK != u4Ret)
    {
        MTVENC_PRINT("_BUFMGR_OBJ_Free fail(%d)\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_FlushFb(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4YAddr, u4CAddr, u4FrmSz;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    u4YAddr = (UINT32)rArg.ai4Arg[0];
    u4CAddr = (UINT32)rArg.ai4Arg[1];
    u4FrmSz = (UINT32)rArg.ai4Arg[2];

    HalFlushDCacheMultipleLine(VIRTUAL(u4YAddr), u4FrmSz);
    HalFlushDCacheMultipleLine(VIRTUAL(u4CAddr), u4FrmSz >> 1);

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_InsertKeyframe(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    VENC_RT_CTRL_T tCtrl;
    UINT32 u4Ret;
    MTVENC_INFO_T *prMtVEncInfo;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);
    prMtVEncInfo = _MtVEncGetInfoInst(0);

    tCtrl.u4Flag = VENC_RT_CTRL_FLAG_INSERT_KEYFRAME;

    if (VENC_SMODE_SYNC == prMtVEncInfo->u4SyncMode)
    {
        u4Ret = VENC_SetRtCtrlSync(hEnc, &tCtrl);
    }
    else
    {
        u4Ret = VENC_SetRtCtrlAsync(hEnc, &tCtrl);
    }

    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("VENC_RT_CTRL_FLAG_INSERT_KEYFRAME fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_RegCb(unsigned long arg)
{
    VENC_HANDLE hEnc = (VENC_HANDLE)arg;
    UINT32 u4Ret;
    MTVENC_INFO_T *prMtVEncInfo;
    VENC_CTRL_OUTCB_T rCb;

    MTVENC_PRINT(" - hEnc = %d\n",(int)hEnc);
    prMtVEncInfo = _MtVEncGetInfoInst(0);
    prMtVEncInfo->fg2Usr = TRUE;

    //Output Callback
    rCb.pvData = (VOID *)prMtVEncInfo;
    rCb.cb     = _MtVEncCb;
    u4Ret = VENC_SetCtrl(hEnc, VENC_CTRL_OUT_CB, (VOID*)&rCb);
    if (VENC_OK != u4Ret)
    {
        MTVENC_PRINT("_MTVENC_RegCb fail(%d)!!\n", u4Ret);
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


static MT_RESULT_T _MTVENC_UnlockBuf(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    VENC_HANDLE hEnc;
    UINT32 u4Addr, u4Size, u4Ret;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    hEnc   = (VENC_HANDLE)rArg.ai4Arg[0];
    u4Addr = (UINT32)rArg.ai4Arg[1];
    u4Size = (UINT32)rArg.ai4Arg[2];

    MTVENC_PRINT("[xiaolei] mtunlock add 0x%x ,size 0x%x\n", u4Addr, u4Size);

    u4Ret = VENC_UnlockBuf(hEnc, u4Addr, u4Size);

    return (VENC_OK == u4Ret)? MTR_OK : MTR_NOT_OK;
}


static MT_RESULT_T _MTVENC_CopyData(unsigned long arg)
{
#ifdef COPYDATA_FROME_DRVIER
    MTAL_IOCTL_8ARG_T rArg;
    MTVENC_INFO_T*    prMtVEncInfo;
	UINT32 u4Size,u4UserAddr,u4PhyAddr,u4CopySize,u4VirAddr;
    UINT32 u4StartAddr,u4EndAddr;
     FBM_POOL_T *prFbmPool;
	INT32 i4Ret=0;
    INT8 uPort=0;
    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);
	uPort=rArg.ai4Arg[0];
	u4UserAddr=rArg.ai4Arg[1];
	u4PhyAddr=rArg.ai4Arg[2];
	u4Size=rArg.ai4Arg[3];
    u4StartAddr=rArg.ai4Arg[4];
    u4EndAddr=rArg.ai4Arg[5];

    if(u4StartAddr==0 || u4EndAddr==0)
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_VENC);
        u4StartAddr=prFbmPool->u4Addr;
        u4EndAddr=u4StartAddr+prFbmPool->u4Size;
    }
	prMtVEncInfo = _MtVEncGetInfoInst(0);
    if((u4PhyAddr+u4Size)>u4EndAddr)
    {
       u4CopySize=u4EndAddr-u4PhyAddr;
       u4VirAddr=VIRTUAL(u4PhyAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
          printk("%s copy_to_user error1\n", __FUNCTION__);
          return MTR_ERR_INV;
       }
       
       u4UserAddr+=u4CopySize;
       u4CopySize=u4Size-u4CopySize;
       u4VirAddr=VIRTUAL(u4StartAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
          printk("%s copy_to_user error2\n", __FUNCTION__);
          return MTR_ERR_INV;
       }
    }
    else
    {
       u4CopySize=u4Size;
       u4VirAddr=VIRTUAL(u4PhyAddr);
	   HalInvalidateDCacheMultipleLine(u4VirAddr,u4CopySize);
       i4Ret=copy_to_user((void __user *)u4UserAddr,(void *)u4VirAddr,u4CopySize);
       if(i4Ret)
       {
           printk("%s copy_to_user error3\n", __FUNCTION__);
           return MTR_ERR_INV;
       }
    }
    
   return MTR_OK;
#else
   return MTR_NOT_OK;
#endif
}
#endif // CC_SUPPORT_VENC

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#ifdef  CC_SUPPORT_VSS420
extern void u1DrvScpipH264SetDramAddress(UINT32 u4BaseAddr, UINT32 u4YFrameSize);
extern void u1DrvScpipH264ReadyMode(UINT8 u1WMode);
extern void u1DrvScpipH264Enable(UINT8 u1Mode);
extern void u1DrvScpipH264forVss(UINT8 u1Vss420_OnOff);
extern void vDrvScpipVss420DropMN( UCHAR u1DropRatio);
extern UCHAR vDrvScpipGetVss420DropMN( void );
extern void u1DrvScpipH264TriggerNext(void);
extern void _VSS420Cb(void);
extern void (*_pfVSS420Cb)(void);
extern UINT8 u1DrvScpipH264GetCurrentFrameIdx(void);
extern UINT8 u1DrvScpipH264GetDramAddress(UINT8 u1Idx,UINT32* u4BufAdr_y, UINT32* u4BufAdr_c);
extern UINT8 vMonOutGetDaFrameNum(void);

static UINT8 _fgMtvssStart=0;
static UINT8 _fgMtvssInit=0;
UINT8        _fgDropFrame=0;


static MT_RESULT_T _MTVENC_Init_v(unsigned long arg)
{
	MTAL_IOCTL_8ARG_T rArg;
    int u4CapWidth, u4CapHeight;
	UCHAR u1DropMN;
	UCHAR u1InterlaceMode;
    //buffer iitial...    
    FBM_POOL_T *prFbmPool;
	UINT32 u4AcrBaseAddr,u4AcrYbuferSize;
	


    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);
	u4CapWidth=VENC_ALIGN_MASK(rArg.ai4Arg[0],15); //16 align is necessary dram 128bit/batch
	u4CapHeight=VENC_ALIGN_MASK(rArg.ai4Arg[1],7);
    u1DropMN=rArg.ai4Arg[2];
	u1InterlaceMode=rArg.ai4Arg[3];
 
    //printk("io_mtvenc get command width(%d) height(%d) dropFr(%d) u1InterlaceMode(%d).\n",u4CapWidth,u4CapHeight,u1DropMN,u1InterlaceMode);
    vDrvVencVDPSetEncInput(E_VDP_VENC_MAIN_BEFORE_PRED); //set ACR path is come from VDP_pre_Proc_Mian  
	vDrvVencVDPSetEncFldType(E_VDP_VENC_BOTH_FLD);       //if interlace input, enable the both field,
    vDrvVencVDPSetParam(VENC_PROFILE_MS,u4CapWidth,u4CapHeight);
    vDrvScpipVss420DropMN(u1DropMN);

    MTVENC_PRINT("-venc path init for acr\n");

    //Buffer initial ....
    prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_VENC);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool->u4Size != 0);

    //printk("FBM Pool Add = 0x%x, Size = %d \n", prFbmPool->u4Addr, prFbmPool->u4Size);
	
    u4AcrBaseAddr  = VENC_ALIGN_MASK(prFbmPool->u4Addr, 63);
    u4AcrYbuferSize= u4CapWidth*u4CapHeight;
    u1DrvScpipH264SetDramAddress(u4AcrBaseAddr, u4AcrYbuferSize ); 

	u1DrvScpipH264ReadyMode(1); //ready mode
	u1DrvScpipH264forVss(ON);
	u1DrvScpipH264Enable(1); //enable

    _pfVSS420Cb=_VSS420Cb;   
	_fgMtvssInit=1;
   return MTR_OK;
}
//-----------------------------------------
static MT_RESULT_T _MTVENC_GetCapureInform_v(unsigned long arg)
{
		MTAL_IOCTL_8ARG_T rArg;
		MTAL_IOCTL_8ARG_T *ptrArg;
		UINT16 width, height, FrameRate, source;
		VDP_VENC_INPUT_RES_t tInputRes;

		if (_fgMtvssInit == 0)
		{
			printk("Get Capture Information error-- not initialed)!\n");
			return MTR_NOT_OK;
		}
		ptrArg=(MTAL_IOCTL_8ARG_T *)arg;
        vDrvVencVDPGetEncInRes(&tInputRes);
        rArg.ai4Arg[0]=width = tInputRes.u4EncInWidth;
        rArg.ai4Arg[1]=height= tInputRes.u4EncInHeight;
        rArg.ai4Arg[2]=FrameRate= tInputRes.u4Fps;
		rArg.ai4Arg[3]=source=eDrvVencVDPGetEncInput();
        rArg.ai4Arg[4]=tInputRes.fgInterlace;
		rArg.ai4Arg[5]=vDrvScpipGetVss420DropMN();
		printk(" DEBUG _MTVENC_GetCapureInform_v  width(%d) height(%d) FrameRate(%d) source(%d) interlace(%d) drop(0x%x).\n", width, height, FrameRate, source,rArg.ai4Arg[4],rArg.ai4Arg[5]);

		USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
  	    COPY_TO_USER_ARG(ptrArg,rArg,MTAL_IOCTL_8ARG_T);
    return MTR_OK;
}

static MT_RESULT_T _MTVENC_StartCapure_v(unsigned long arg)
{
	MTAL_IOCTL_4ARG_T rArg;
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

	if (_fgMtvssInit == 0)
	{
		printk("Start capture error--(not initialed)!\n");
		return MTR_NOT_OK;
	}

//	u1DrvScpipH264ReadyMode(1); //ready mode
//	u1DrvScpipH264forVss(ON);
//    u1DrvScpipH264Enable(1); //enable
    u1DrvScpipH264TriggerNext();
	_fgMtvssStart=1;
	_fgDropFrame=3;  //drop first 3 frmaes
	printk("-- _ACR_Start --\n");	
	return MTR_OK;
}

static MT_RESULT_T _MTVENC_StopCapure_v(void)
{

   u1DrvScpipH264Enable(0); //enable
   u1DrvScpipH264forVss(OFF);
   _fgMtvssStart=0;
   printk("ACR_Stop\n");   
   return MTR_OK;
}


/**
 * @brief Get Frame Buffer from Scaler DRAM dump.
 Copy to user (previous frame)
 Check status, if index changed, call back
 After call back, trigger next frame, update current index
 * @param *buffer
 */
static MT_RESULT_T _MTVSS_GetFrameBuffer420(unsigned long arg)
{
    INT32 rArg;
    UINT16 u2CapWidth,u2CapHeight;
    UINT8  *buffer;
	UINT32  u4KernelAddr;
	UINT8  u1Idx;
	VDP_VENC_INPUT_RES_t    tInputRes;
	UINT32 u4BufAdr_y, u4BufAdr_c;

	UINT32 u4FrmaeSize;
	UINT8  u1DaFrameNum;
	

	//printk(" _MTVSS_GetFrameBuffer420 --- \n");

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);
    buffer     = (UINT8  *)rArg;

	if (_fgMtvssStart == 0)
	{
		printk("VSS_ON_VENC not connected (GetFrameBuffer error)!\n");
		return MTR_NOT_OK;
	}

	vDrvVencVDPGetEncInRes(&tInputRes);
	u2CapWidth = tInputRes.u4EncInWidth;
	u2CapHeight= tInputRes.u4EncInHeight;

	u4FrmaeSize = u2CapWidth * u2CapHeight * 3/2;  //YUV 420 format

    u1Idx = u1DrvScpipH264GetCurrentFrameIdx();
	//printk("  u1idx=%d ,width=%d,height=%d\n",u1Idx,u2CapWidth,u2CapHeight);
    u1DaFrameNum = vMonOutGetDaFrameNum();
    u1Idx = ( u1Idx + (u1DaFrameNum-1)) % u1DaFrameNum;
	u1DrvScpipH264GetDramAddress(u1Idx,&u4BufAdr_y,&u4BufAdr_c );
	u4KernelAddr= (u4BufAdr_y << 4);
	//printk("-- addr(0x%x / 0x%x)ko_addr=0x%x,frame_size=%d\n" ,u4BufAdr_y,u4BufAdr_c,u4KernelAddr ,u4FrmaeSize);

    USR_SPACE_ACCESS_VALIDATE_ARG(buffer, UINT8);
	//HalFlushInvalidateDCache();
	HalFlushInvalidateDCacheMultipleLine((UINT32)(VIRTUAL(u4KernelAddr)) ,u4FrmaeSize);
    if (copy_to_user((void __user *)buffer, (void *)(VIRTUAL((UINT32)u4KernelAddr)), (sizeof(UINT8)*u4FrmaeSize)))
    {                                                                          
        printk("%s copy_to_user error\n", __FUNCTION__);                       
        return MTR_DONT_CARE;                                                    
    }
   
	//printk("copy to uer done, trigger next ....\n");
	//u1DrvScpipH264TriggerNext(); // trigger WRITE
    return MTR_OK;
}

//
// trigger next frame
static MT_RESULT_T _MTVENC_GetNextFrame_v(void)
{
	u1DrvScpipH264TriggerNext(); // trigger WRITE
	return MTR_OK;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#endif // SUPPORT_VSS420


int mtal_ioctl_mtvenc(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;

    switch (cmd)
    {
    default:
        MTVENC_PRINT("Unknown cmd(%d)\n", cmd);
        i4Ret = MTR_NOT_OK;
        break;
#ifdef CC_SUPPORT_VSS420		
    case MTAL_IO_VENC_INIT_V:
        i4Ret = _MTVENC_Init_v(arg);
        break;	
    case MTAL_IO_VENC_GET_CAP_INFO:
        i4Ret = _MTVENC_GetCapureInform_v(arg);
		break;
	case MTAL_IO_VENC_START_CAPTURE:
        i4Ret = _MTVENC_StartCapure_v(arg);
		break;
    case MTAL_IO_VENC_STOP_CAPTURE:
		i4Ret = _MTVENC_StopCapure_v();
		break;
    case MTAL_IO_VSS_GET_FRAME_BUFFER_420:
		i4Ret = _MTVSS_GetFrameBuffer420(arg);
		break;
    case MTAL_IO_VSS_TRIGGER_NEXT_FRAME_420:
		i4Ret = _MTVENC_GetNextFrame_v();
		break;
#endif

#ifdef CC_SUPPORT_VENC
    case MTAL_IO_VENC_COPYDATA:
        i4Ret=_MTVENC_CopyData(arg);
	    break;
    case MTAL_IO_VENC_INIT:
        i4Ret = _MTVENC_Init();
        break;
    case MTAL_IO_VENC_TERMINT:
        i4Ret = _MTVENC_Termint();
        break;
    case MTAL_IO_VENC_RESET:
        i4Ret = _MTVENC_Reset(arg);
        break;
    case MTAL_IO_VENC_SET:
        i4Ret = _MTVENC_Set(arg);
        break;
    case MTAL_IO_VENC_GET:
        i4Ret = _MTVENC_Get(arg);
        break;
    case MTAL_IO_VENC_OPEN:
        i4Ret = _MTVENC_Open(arg);
        break;
    case MTAL_IO_VENC_SET_PARAM:
        i4Ret = _MTVENC_SetParam(arg);
        break;
    case MTAL_IO_VENC_SET_CTRL:
        i4Ret = _MTVENC_SetCtrl(arg);
        break;
    case MTAL_IO_VENC_DRV_INIT:
        i4Ret = _MTVENC_DrvInit(arg);
        break;
    case MTAL_IO_VENC_RUN:
        i4Ret = _MTVENC_Run(arg);
        break;
    case MTAL_IO_VENC_STOP:
        i4Ret = _MTVENC_Stop(arg);
        break;
    case MTAL_IO_VENC_CLOSE:
        i4Ret = _MTVENC_Close(arg);
        break;
    case MTAL_IO_VENC_ENC_FRAME_SYNC:
        i4Ret = _MTVENC_EncFrameSync(arg);
        break;
    case MTAL_IO_VENC_ENC_FRAME_ASYNC:
        i4Ret = _MTVENC_EncFrameAsync(arg);
        break;
    case MTAL_IO_VENC_CREATE_BUFFMGR:
        i4Ret = _MTVENC_CreateBuffMgr(arg);
        break;
    case MTAL_IO_VENC_DELETE_BUFFMGR:
        i4Ret = _MTVENC_DeleteBuffMgr(arg);
        break;
    case MTAL_IO_VENC_BUFFMGR_OBJ_GET:
        i4Ret = _MTVENC_BUFMGR_OBJ_Get(arg);
        break;
    case MTAL_IO_VENC_BUFFMGR_OBJ_FREE:
        i4Ret = _MTVENC_BUFMGR_OBJ_Free(arg);
        break;
    case MTAL_IO_VENC_FLUSH_FB:
        i4Ret = _MTVENC_FlushFb(arg);
        break;
    case MTAL_IO_VENC_INSERT_KEYFRM:
        i4Ret = _MTVENC_InsertKeyframe(arg);
        break;
    case MTAL_IO_VENC_REG_CB:
        i4Ret = _MTVENC_RegCb(arg);
        break;
    case MTAL_IO_VENC_UNLOCK_BUF:
        i4Ret = _MTVENC_UnlockBuf(arg);
        break;
#endif // CC_SUPPORT_VENC
    }

    return i4Ret;
}


#if defined(CC_SUPPORT_MUXER) && defined(CC_SUPPORT_VENC)
VOID MTVENC_SetMxId(UINT16  u2Id, UCHAR ucMxId)
{
    MTVENC_INFO_T *prMtVEncInfo = _MtVEncGetInfoInst(u2Id);
    prMtVEncInfo->ucMxId = ucMxId;

    MTVENC_PRINT("mux id = %d\n", ucMxId);
}
#endif




