/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_VDECOMX)

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include <linux/cdev.h>
#include <linux/fs.h>          //For file_operations
#include "x_debug.h"
#include "x_typedef.h"
#include "x_os.h"
#include "x_printf.h"
#include "x_assert.h"

#include "vdpomx_mod.h"
#include "vdpomx_drv_if.h"

// tunneled video playback support
#include "vdecomx_drv_if.h"

#include <linux/cb_data.h>
#include <linux/version.h>

#include <asm/uaccess.h>
/////////////////////////////////////
//driver include file
#include "vdp_if.h"
#include "mute_if.h"
#include "vdo_if.h"
#include "b2r_drvif.h"
#include "srm_drvif.h"

//#define  CC_HWCOMP_UT
#ifdef CC_HWCOMP_UT
#include "fbm_drvif.h"
#endif
/////////////////////////////////////

#ifdef CC_HWCOMP_UT
static UCHAR _aucFbgId[VDP_NS] = {FBM_FBG_ID_UNKNOWN, FBM_FBG_ID_UNKNOWN};
static INT32 _ai4TemporalRef[VDP_NS] = {0, 0};
static BOOL _afgForceSequenceChange[VDP_NS] = {FALSE,FALSE};
#endif

static BOOL _afgVdpConnected[VDP_NS] = {FALSE,FALSE};

//#define DEBUG

#ifdef DEBUG
#define VDPOMX_DBG(fmt...)    printk("%s:","[vdpomx_mod]");  printk(fmt)
#else
    #define VDPOMX_DBG(fmt...)
#endif

int vdp_set_out_region(VDPOMX_SET_REGION_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_SET_REGION_PARM_T rParam ;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_SET_REGION_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_SET_REGION_PARM_T);    
    
    VDP_SetOutRegion((UCHAR)(rParam.ucChId), (UCHAR)0, *(VDP_REGION_T*)&(rParam.rRegion));
    VDPOMX_DBG(" - eVdpId = %d\n"
	 "rSrcRegion.u4Y = %d\n"
	 "rSrcRegion.u4Y = %d\n"
        "rSrcRegion.u4Width = %d\n"
        "rSrcRegion.u4Height = %d\n", 
        rParam.ucChId,
        (int)rParam.rRegion.u4X,
        (int)rParam.rRegion.u4Y,
        (int)rParam.rRegion.u4Width,
        (int)rParam.rRegion.u4Height);        
    return i4Ret;
}

int vdp_set_src_region(VDPOMX_SET_REGION_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_SET_REGION_PARM_T rParam ;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_SET_REGION_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_SET_REGION_PARM_T);    
    
    VDP_SetSrcRegion((UCHAR)(rParam.ucChId), (UCHAR)0, *(VDP_REGION_T*)&(rParam.rRegion));
    VDPOMX_DBG(" - eVdpId = %d\n"
	 "rSrcRegion.u4Y = %d\n"
	 "rSrcRegion.u4Y = %d\n"
        "rSrcRegion.u4Width = %d\n"
        "rSrcRegion.u4Height = %d\n", 
        rParam.ucChId,
        (int)rParam.rRegion.u4X,
        (int)rParam.rRegion.u4Y,
        (int)rParam.rRegion.u4Width,
        (int)rParam.rRegion.u4Height);        
    return i4Ret;
}

int vdp_set_out_channel(VDPOMX_SET_CHANNEL_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_SET_CHANNEL_PARM_T rParam ;
    UINT8 u1Src ;
	UINT32 u4UnMuteDleay ;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_SET_CHANNEL_PARM_T);   
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_SET_CHANNEL_PARM_T);    

    if(rParam.fgOnOff)
    {        
        u1Src = (rParam.ucChId == VDP_DISP_CHANNEL_0)?SV_VS_DTV1 :SV_VS_DTV2;
        u4UnMuteDleay = 0 ; 

        if(bApiVideoSetVideoSrc(rParam.ucChId, u1Src) == SV_FAIL)
        {
            i4Ret = VDPOMX_ERR;
        }

        if(VDP_SetEnable(rParam.ucChId, rParam.fgOnOff) != VDP_SET_OK)
        {
            i4Ret = VDPOMX_ERR;
        }
    }
    else
    {
        VDP_REGION_T rRegion ;
        u1Src = SV_VS_MAX ;
        memset(&rRegion, 0, sizeof(VDP_REGION_T));
        VDP_SetOutRegion((UCHAR)(rParam.ucChId), (UCHAR)0, *(VDP_REGION_T*)&(rRegion));     
		u4UnMuteDleay = FOREVER_MUTE ; 

        if(VDP_SetEnable(rParam.ucChId, rParam.fgOnOff) != VDP_SET_OK)
        {
            i4Ret = VDPOMX_ERR;
        }
        
        if(bApiVideoSetVideoSrc(rParam.ucChId, u1Src) == SV_FAIL)
        {
            i4Ret = VDPOMX_ERR;
        }
    }
   
    _vDrvVideoSetMute(MUTE_MODULE_API_FORCE, (UINT32) rParam.ucChId, u4UnMuteDleay , FALSE);

#ifdef CC_HWCOMP_UT
    VDP_SetInput(rParam.ucChId, 0, 0);
    _afgForceSequenceChange[rParam.ucChId] = TRUE;
#endif    

    _afgVdpConnected[rParam.ucChId] = FALSE;
    
    VDPOMX_DBG(" - eVdpId = %d OnOff = %d\n",
        rParam.ucChId,
        (int)rParam.fgOnOff);        
    return i4Ret;
}

int vdp_set_display_buf(VDPOMX_SET_DISPLAY_BUF_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_SET_DISPLAY_BUF_PARM_T rParam;
    VDP_B2R_CHG_FRAME_MSG_T rB2rDispMsg;
    UCHAR ucFbgId = FBM_FBG_ID_UNKNOWN;
    UCHAR ucFbId = FBM_FB_ID_UNKNOWN;

    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_SET_DISPLAY_BUF_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_SET_DISPLAY_BUF_PARM_T);

    FBM_GetFbByDispTag(rParam.u4Addr, &ucFbgId, &ucFbId);

    if(ucFbgId==FBM_FBG_ID_UNKNOWN || ucFbId==FBM_FB_ID_UNKNOWN)
    {
       VDPOMX_DBG("vdp_set_display_buf FBM_GetFbByDispTag ERROR\n");
       return VDPOMX_ERR;
    }
    
#ifndef CC_HWCOMP_UT
    if (rParam.ucChId < VDP_NS)
    {
        if (_afgVdpConnected[rParam.ucChId] == FALSE)
        {
            // Use buffer address to check real vdec es id.
            // And set es id to b2r input port
            UCHAR ucEsId;

            if (ucFbgId != FBM_FBG_ID_UNKNOWN)
            {
                // Check real decoder es id, and set to B2R input port
                ucEsId = FBM_GetDecoderSrcId(ucFbgId);
                VDP_SetInput(rParam.ucChId, ucEsId, 0);
                _afgVdpConnected[rParam.ucChId] = TRUE;
                VDP_SetB2rNoLock(rParam.ucChId, TRUE);
            }
        }
    }
#else
    if (rParam.ucChId < VDP_NS)
    {
        UCHAR ucVdpId = rParam.ucChId;
        FBM_SEQ_HDR_T *prSeqHdr = NULL;
        FBM_PIC_HDR_T *prPicHdr = NULL;
        
        if (ucFbgId != FBM_FBG_ID_UNKNOWN && ucFbId != FBM_FB_ID_UNKNOWN)
        {
            if (ucFbgId != _aucFbgId[ucVdpId] || _afgForceSequenceChange[ucVdpId] == TRUE)
            {
                FBM_FbgChgNotify(ucFbgId, VDP_Vdp2Es(ucVdpId));
                
                // Fbg changed, reset sequence info
                prSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
                if (prSeqHdr != NULL)
                {
                    prSeqHdr->ucAspRatInf = MPEG_ASPECT_RATIO_1_1;
                    prSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
                    prSeqHdr->fgProgressiveSeq = TRUE;
                    prSeqHdr->u2LineSize = 176;
                    prSeqHdr->u2HSize = 176;
                    prSeqHdr->u2VSize = 144;
                    prSeqHdr->u2OrgHSize = 176;
                    prSeqHdr->u2OrgVSize = 144;
                    prSeqHdr->u4ResizeWidth = 176;
                    prSeqHdr->u4ResizeHeight = 144;

                    prSeqHdr->fgRasterOrder = TRUE;
                    prSeqHdr->fgResizeSmallPic = FALSE;
                    prSeqHdr->fgB2R3DEnable= FALSE;
                    prSeqHdr->fgColorDescrip = FALSE;
                    prSeqHdr->fgCropping = FALSE;
                    
                    prSeqHdr->u4CropX = 0;
                    prSeqHdr->u4CropY = 0;
                    prSeqHdr->u4CropWidth = 0; 
                    prSeqHdr->u4CropHeight = 0;
                    prSeqHdr->u4FrmCropTop = 0;
                    prSeqHdr->u4FrmCropBtm = 0;                   

                    prSeqHdr->fgSwDecoder = TRUE;

                    VDPOMX_DBG(" - HWCOMP_UT trigger sequence change (FbgId = %d)\n", ucFbgId);
                    FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP);  // trigger seq chg immediately
                }

                _aucFbgId[ucVdpId] = ucFbgId;
                _ai4TemporalRef[ucVdpId] = 0;
                _afgForceSequenceChange[ucVdpId] = FALSE;
            }

            prPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
            if (prPicHdr != NULL)
            {
                prPicHdr->u4PTS = 0;
                prPicHdr->u4PicWidthPitch = 176;
                prPicHdr->u4PicWidth = 176;
                prPicHdr->u4PicHeight = 144;
                prPicHdr->i4TemporalRef = _ai4TemporalRef[ucVdpId]++;
                prPicHdr->fgProgressiveFrm = TRUE;
                
                prPicHdr->fgSeqHeader = FALSE;
                prPicHdr->fgSeamlessDisp = FALSE;
                prPicHdr->fgProgressive2Interlace = FALSE;
                prPicHdr->fgTrackingMode = FALSE;
                prPicHdr->fgRepFirstFld = FALSE;
                prPicHdr->fgTopFldFirst = FALSE;
                prPicHdr->fgDummyFrame = FALSE;
            }
        }
    }
#endif

    x_memset(&rB2rDispMsg, 0, sizeof(VDP_B2R_CHG_FRAME_MSG_T));

    rB2rDispMsg.u4VdpId = rParam.ucChId;
    rB2rDispMsg.u4Flag = VDP_B2R_CHG_FRAME_MSG_SYNC;
    rB2rDispMsg.u4DispMode = VDP_B2R_CHG_FRAME_DIRECT;
    rB2rDispMsg.ucFbgId = ucFbgId;
    rB2rDispMsg.ucFbId = ucFbId;

    //FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &rB2rDispMsg.u4FrameAddrY,&rB2rDispMsg.u4FrameAddrC);
    if(VDP_SendB2RChgFrameMsg(&rB2rDispMsg) == FALSE)
    {
        i4Ret = VDPOMX_ERR;
    }

    VDPOMX_DBG(" - eVdpId = %d DispBufAddr = 0x%x\n", rParam.ucChId, rParam.u4Addr);        

    return i4Ret;
}

int vdp_get_channel_free(VDPOMX_GET_CHANNEL_FREE_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_GET_CHANNEL_FREE_PARM_T rParam;
    UINT8 u1CurSrc;

    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_GET_CHANNEL_FREE_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_GET_CHANNEL_FREE_PARM_T);

     u1CurSrc=(rParam.ucChId ==0)?_bSrcMainNew: _bSrcSubNew;

    rParam.fgFree = (u1CurSrc == SV_VS_MAX)? 1 : 0 ;
    
    COPY_TO_USER_ARG(prParm, rParam, VDPOMX_GET_CHANNEL_FREE_PARM_T);
    VDPOMX_DBG(" - eVdpId = %d free = 0x%x\n", rParam.ucChId, rParam.fgFree);        

    return i4Ret;
}

int vdp_get_buf_locked(VDPOMX_GET_BUF_LOCKED_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_GET_BUF_LOCKED_PARM_T rParam;
//    UINT8 u1CurSrc;

    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_GET_BUF_LOCKED_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_GET_BUF_LOCKED_PARM_T);

    if (VDP_GetDispBufLocked(rParam.ucChId, rParam.u4Buf, &(rParam.fgLocked)) == 0)
    {
        i4Ret = VDPOMX_ERR;
    }
    
    COPY_TO_USER_ARG(prParm, rParam, VDPOMX_GET_BUF_LOCKED_PARM_T);
    VDPOMX_DBG(" - eVdpId = %d free = 0x%x\n", rParam.ucChId, rParam.fgFree);        

    return i4Ret;
}
/*
int vdp_tunnel_connect_audio(VDPOMX_TUNNEL_CONNECT_AUD_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_TUNNEL_CONNECT_AUD_PARM_T rParam;

    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_TUNNEL_CONNECT_AUD_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_TUNNEL_CONNECT_AUD_PARM_T);

    if (!_vdecomx_tunnel_VdpOn(rParam.ucChId, rParam.u4AudioHwSync))
    {
        VDPOMX_DBG(" - eVdpId = %d AudioHwSync = 0x%x fail!!!\n", rParam.ucChId, rParam.u4AudioHwSync);
        i4Ret = VDPOMX_ERR;
    }

    VDPOMX_DBG(" - eVdpId = %d AudioHwSync = 0x%x\n", rParam.ucChId, rParam.u4AudioHwSync);

    return i4Ret;
}

int vdp_connect_vdec(VDPOMX_CONNECT_VDEC_PARM_T *prParm)
{
    INT32 i4Ret = VDPOMX_OK;
    VDPOMX_CONNECT_VDEC_PARM_T rParam;

    USR_SPACE_ACCESS_VALIDATE_ARG(prParm, VDPOMX_CONNECT_VDEC_PARM_T);
    COPY_FROM_USER_ARG(prParm, rParam, VDPOMX_CONNECT_VDEC_PARM_T);

    VDP_SetInput(rParam.ucChId,rParam.ucVdecId,rParam.ucPort);

    VDPOMX_DBG(" - ucChId = %d ucVdecId = %d ucPort = %d\n", rParam.ucChId, rParam.ucVdecId, rParam.ucPort);

    return i4Ret;
}
*/
int vdp_srm_settvmode(unsigned long arg)
{
    INT32 u4mode;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg,INT32);
    COPY_FROM_USER_ARG(arg,u4mode,INT32);
    SRM_SetTvMode((SRM_TV_MODE_T)u4mode);

	VDPOMX_DBG(" - u4mode = %d\n", u4mode);

    return 0;
}

#endif

