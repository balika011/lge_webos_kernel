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

#include "x_assert.h"
#include "x_os.h"
#include "drv_imgresz_errcode.h"
#include "drv_imgresz.h"
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

INT32 i4ImgResz_Drv_GetTicket(IMGRESZ_DRV_TICKET_T *pImgReszTicket)
{
    return 0;// i4ImgResz_Inst_GetInstance(&(pImgReszTicket->u4Ticket));
}


INT32 i4ImgResz_Drv_ReleaseTicket(IMGRESZ_DRV_TICKET_T *pImgReszTicket)
{
    return 0;//i4ImgResz_Inst_ReleaseInstance(pImgReszTicket->u4Ticket);
}


INT32 i4ImgResz_Drv_GetState(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_SCALE_STATE_T *pImgReszState)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    //pImgReszState->eState = eImgResz_Inst_GetState(pImgReszTicket->u4Ticket);
    return i4Ret;
}


INT32 i4ImgResz_Drv_SetAgentID(IMGRESZ_DRV_TICKET_T *pImgReszTicket, UINT32  id)

{

  INT32 i4Ret = S_IMGRESZ_DRV_OK;


    return i4Ret;


}



INT32 i4ImgResz_Drv_SetPriority(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_SCALE_PRIORITY  eScalePriority)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    
    return i4Ret;
}


INT32 i4ImgResz_Drv_SetLock(IMGRESZ_DRV_TICKET_T *pImgReszTicket,BOOL fgLock)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}


INT32 i4ImgResz_Drv_SetScaleMode(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_SCALE_MODE eScaleMode)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

   
    return i4Ret;
}

INT32 i4ImgResz_Drv_SetLumaKey(IMGRESZ_DRV_TICKET_T *pImgReszTicket,UINT8 u1LumaKey, BOOL fgEnableLumaKey)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

   
    return i4Ret;
}


INT32 i4ImgResz_Drv_SetSrcBufInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_SRC_BUF_INFO_T *pSrcBufInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;


    return i4Ret;
}


INT32 i4ImgResz_Drv_SetDstBufInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_DST_BUF_INFO_T *pDstBufInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

   
    return i4Ret;
}

INT32 i4ImgResz_Drv_SetBldBufInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_BLD_BUF_INFO_T *pBldBufInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}

INT32 i4ImgResz_Drv_SetPartialBufInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_PARTIAL_INFO_T *pPartialBufInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}


INT32 i4ImgResz_Drv_SetJpegInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_JPEG_INFO_T *prJpegInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

  
    return i4Ret;

}

INT32 i4ImgResz_Drv_SetRmInfo(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_RM_INFO_T *prRmInfo)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}

INT32 i4ImgResz_Drv_Set_Scale1To1(IMGRESZ_DRV_TICKET_T *pImgReszTicket,BOOL fg1To1Scale)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}

INT32 i4ImgResz_Drv_Set_Y_Only(IMGRESZ_DRV_TICKET_T *pImgReszTicket, BOOL fgYOnly)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

    return i4Ret;
}

INT32 i4ImgResz_Drv_DoScale(IMGRESZ_DRV_TICKET_T *pImgReszTicket, IMGRESZ_DRV_DO_SCALE_T *pDoScale)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

   
    return i4Ret;
}


INT32 i4ImgResz_Drv_StopScale(IMGRESZ_DRV_TICKET_T *pImgReszTicket)
{
    INT32 i4Ret = S_IMGRESZ_DRV_OK;

  
    return i4Ret;
}


INT32 i4ImgResz_Drv_RegFinishNotifyCallback(IMGRESZ_DRV_TICKET_T *pImgReszTicket,IMGRESZ_DRV_NOTIFY_CB_REG_T *prNotifyCallbackReg)
{
  

    return S_IMGRESZ_DRV_OK;
}


INT32 i4ImgResz_Drv_UnregFinishNotifyCallback(IMGRESZ_DRV_TICKET_T *pImgReszTicket,IMGRESZ_DRV_NOTIFY_CB_REG_T *prNotifyCallbackReg)
{

    return S_IMGRESZ_DRV_OK;
}

#if 0
INT32 i4ImgReszVfyJpegPicModeGetResizeStatus(UINT32 u4HwId)
{
    return 0;
}
UINT32 _u4ImgReszVfyJpegPicModeTempLineBuf[2] = { 0 };

void vImgReszVfyJpegPicModeInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput
)
{
}

void vImgReszVfyJpegPicModeUninit(UINT32 u4HwId)
{
   
}


void vImgReszVfyJpegPicModeWorkaroundYC2AYUV(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                                           UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                                           UINT32 u4SrcBufSA1,UINT32 u4SrcBufSA2,
                                                           UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                                           UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                                           UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                                           UINT32 u4DestBufSA1)
{
}

void vImgReszVfyJpegPreloadInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBufSA1,UINT32 u4SrcBufSA2,UINT32 u4SrcBufSA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgYExist,BOOL fgCbExist, BOOL fgCrExist
)
{
}

INT32 i4ImgReszVfyJpegPreloadGetResizeStatus(UINT32 u4HwId)
{
    return 0;
}
void vImgReszVfyJpegPreloadUninit(UINT32 u4HwId)
{
    
}

void vImgReszVfyJpegScanModePreloadInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput,BOOL fgYExist,
                                              BOOL fgCbExist,BOOL fgCrExist,BOOL fgPreload
)
{
}

void vImgReszVfyJpegScanModeInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput,BOOL fgYExist,
                                              BOOL fgCbExist,BOOL fgCrExist
)
{
}
#endif