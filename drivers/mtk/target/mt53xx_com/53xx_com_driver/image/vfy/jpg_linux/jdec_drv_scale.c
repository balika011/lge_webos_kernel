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


//#include "x_rtos.h"
#include "x_assert.h"
#include "x_os.h"
#include "jdec_drv.h"
#include "drv_imgresz_errcode.h"
//#include "x_mmap.h"
#include "x_drv_map.h"
#include "jdec_debug.h"
//#include "x_hal_1176.h"


/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
extern JDEC_INST_T  _rJdecInst[JDEC_INST_NUM];
extern void vJdecAbort(JDEC_INST_T *prJdecInst, IMG_NFY_STATE_T eState, BOOL fgNfy);
extern void vJdecSetLog(UINT32 u4LogVal);
extern void vJdecStateSwDecFinish(JDEC_INST_T *prJdecInst);

INT32 i4JdecScale(JDEC_INST_T *prJdecInst);
INT32 i4JdecScaleStop(JDEC_INST_T *prJdecInst);

void vJdecScaleSwitch2SW(JDEC_INST_T *prJdecInst, BOOL fgSW)
{
    if (fgSW)
    {
        prJdecInst->fgJdecHwScale = FALSE;
    }
    else
    {
        prJdecInst->fgJdecHwScale = TRUE;
    }
}

INT32 i4JdecScaleCB(INT32 i4State, void *pvPrivData)
{
    JDEC_INST_T *prJdecInst = &(_rJdecInst[(UINT32)pvPrivData]);
    INT32 i4Ret;

    if (prJdecInst->fgJdecHwScale == FALSE)
    {
        prJdecInst->i4JdecScaleState = 0;
        return 0;
    }
    if (i4State == S_IMGRESZ_DRV_RESIZE_FINISH)
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_SCALE_DONE, X_EV_OP_OR);
    }
    else if (i4State == S_IMGRESZ_DRV_RESIZE_STOP)
    {
        UTIL_Printf("[JDEC] Receive  S_IMGRESZ_DRV_RESIZE_STOP \n");
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_SCALE_ABORT, X_EV_OP_OR);
    }
    else if (i4State == E_IMGRESZ_DRV_RESIZE_TIMEOUT)
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_SCALE_ERR, X_EV_OP_OR);
    }
    else if (i4State == S_IMGRESZ_DRV_RESIZE_PIC_MODE_READY)
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent,JDEC_DRV_EV_SCALE_READY, X_EV_OP_OR);
    }
    else
    {
        i4Ret = x_ev_group_set_event(prJdecInst->hJdecEvent, JDEC_DRV_EV_SCALE_ERR, X_EV_OP_OR);
    }
    VERIFY(i4Ret == OSR_OK);

    return 0;
}

INT32 i4JdecScaleWaitFinish(JDEC_INST_T *prJdecInst)
{
    EV_GRP_EVENT_T  eJdecEvent;
    EV_GRP_EVENT_T  eJdecEventGet;
    INT32 i4Ret;

    if (prJdecInst->fgJdecHwScale == FALSE)
    {
        while (prJdecInst->i4JdecScaleState == -1)
        {
            x_thread_delay(1);
        }
        return JDEC_DRV_RTN_OK;
    }
    
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)  
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
        )
    {
        eJdecEvent = JDEC_DRV_EV_SCALE_DONE | JDEC_DRV_EV_STOP | JDEC_DRV_EV_SCALE_ABORT;
        i4Ret = x_ev_group_wait_event_timeout(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME, 500000);
        if (i4Ret != OSR_OK) // time out
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_SCALE_ERROR;
            i4JdecScaleStop(prJdecInst);
            return JDEC_DRV_RTN_FAIL;
        }
        else
        {
            if (eJdecEventGet & JDEC_DRV_EV_STOP)
            {
                vJdecSetLog(JDEC_LOG_SCALE_STOP);
                i4JdecScaleStop(prJdecInst);
                vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
                return JDEC_DRV_RTN_STOP;
            }
            else if (eJdecEventGet & JDEC_DRV_EV_SCALE_ABORT)
            {
                return JDEC_DRV_RTN_FAIL;
            }
            else if (eJdecEventGet & JDEC_DRV_EV_SCALE_DONE)
            {
                return JDEC_DRV_RTN_OK;
            }
            else
            {
                ASSERT(0);
                return JDEC_DRV_RTN_FAIL;
            }
        }
    }
    else
    {
        eJdecEvent = JDEC_DRV_EV_SCALE_DONE | JDEC_DRV_EV_STOP | JDEC_DRV_EV_SCALE_ABORT | JDEC_DRV_EV_SCALE_ERR;
        i4Ret = x_ev_group_wait_event(prJdecInst->hJdecEvent, eJdecEvent, &eJdecEventGet, X_EV_OP_OR_CONSUME);
        VERIFY(i4Ret == OSR_OK);

        if (eJdecEventGet & JDEC_DRV_EV_STOP)
        {
            vJdecSetLog(JDEC_LOG_SCALE_STOP);
            i4JdecScaleStop(prJdecInst);
            vJdecAbort(prJdecInst, (IMG_NFY_STATE_T)IMG_NFY_STOP_DONE, FALSE);
            if (prJdecInst->ptImgJpgObj != NULL)
            {
                vJdecStateSwDecFinish(prJdecInst);
            }
            return JDEC_DRV_RTN_STOP;
        }
        else if (eJdecEventGet & JDEC_DRV_EV_SCALE_ABORT)
        {
            return JDEC_DRV_RTN_FAIL;
        }
        else if (eJdecEventGet & JDEC_DRV_EV_SCALE_ERR)
        {
            prJdecInst->i4JdecErrorType = -E_JPG_ERR_SCALE_ERROR;
            return JDEC_DRV_RTN_FAIL;
        }
        else if (eJdecEventGet & JDEC_DRV_EV_SCALE_DONE)
        {
            return JDEC_DRV_RTN_OK;
        }
        else
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }
}

INT32 i4JdecScaleInit(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    INT32 i4Ret;
    IMG_BASIC_INFO_T *pt_info = NULL;

    if (prJdecInst->ptImgJpgObj == NULL)
    {
        if ((prJdecInst->u4SrcCropPosX != 0) || (prJdecInst->u4SrcCropPosY != 0) ||
            (prJdecInst->u4SrcCropWidth != prSof->u2ImageWidth) ||
            (prJdecInst->u4SrcCropHeight!= prSof->u2ImageHeight) ||
            (prJdecInst->eDstRotate != IMG_ROTATE_NONE))
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }
    else
    {
        pt_info = prJdecInst->ptImgJpgObj->pt_current_sel;
        if ((prJdecInst->u4SrcCropPosX != 0) || (prJdecInst->u4SrcCropPosY != 0) ||
            (prJdecInst->u4SrcCropWidth != pt_info->ui2_image_width) ||
            (prJdecInst->u4SrcCropHeight!= pt_info->ui2_image_height) ||
            (prJdecInst->eDstRotate != IMG_ROTATE_NONE))
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }


    i4Ret = i4ImgResz_Drv_SetPriority(prImgReszTicket, IMGRESZ_DRV_PRIORITY_LOW);
    VERIFY(i4Ret >= 0);
    if ((prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)        
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        && (prJdecInst->eJdecDecMode != JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif        
        )
    {
        i4Ret = i4ImgResz_Drv_SetLock(prImgReszTicket, TRUE);
        VERIFY(i4Ret >= 0);
    }

    // Set scale mode
    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530) 
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
        )
    {
        i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_JPEG_PIC_SCALE);
    }
    else
    {
        i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_PARTIAL_SCALE);
    }
    VERIFY(i4Ret >= 0);

    x_memset((void *)&rSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    x_memset((void *)&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    
    // Set source buffer info
    rSrcBufInfo.eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF;
    rSrcBufInfo.u4BufWidth = prJdecInst->au4ColorBufPitch[0];
    rSrcBufInfo.u4BufHeight = 0; //prJdecInst->u4SrcCropHeight;
    rSrcBufInfo.u4PicWidth = prJdecInst->u4SrcCropWidth;
    rSrcBufInfo.u4PicHeight = prJdecInst->u4SrcCropHeight;
    rSrcBufInfo.u4PicPosX = prJdecInst->u4SrcCropPosX;
    rSrcBufInfo.u4PicPosY = prJdecInst->u4SrcCropPosY;
    rSrcBufInfo.u4YBufAddr = prJdecInst->au4ColorBufSa[0][0];
    rSrcBufInfo.u4CbBufAddr = prJdecInst->au4ColorBufSa[0][1];
    rSrcBufInfo.u4CrBufAddr = prJdecInst->au4ColorBufSa[0][2];
    rSrcBufInfo.u4ColorPalletSa = 0;

    if (prJdecInst->ptImgJpgObj == NULL)
    {
        rSrcBufInfo.rCompFactor.u1YCompFactorH = prSof->arSofComp[0].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = prSof->arSofComp[1].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = prSof->arSofComp[2].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1YCompFactorV = prSof->arSofComp[0].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = prSof->arSofComp[1].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = prSof->arSofComp[2].u1VSampFactor;
#if JDEC_SUPPORT_BROKEN_TARGET
        if ((prJdecInst->fgBrokenTarget)/* && (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)*/)
        {
            if (rSrcBufInfo.rCompFactor.u1YCompFactorV == 2)
            {
                rSrcBufInfo.rCompFactor.u1CbCompFactorV = 2;//prSof->arSofComp[1].u1VSampFactor;
                rSrcBufInfo.rCompFactor.u1CrCompFactorV = 2;//prSof->arSofComp[2].u1VSampFactor;
            }
        }
#endif            
    }
    else    //sw decode
    {
        rSrcBufInfo.rCompFactor.u1YCompFactorH = (UINT8)(pt_info->at_comp_info[0].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = (UINT8)(pt_info->at_comp_info[1].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = (UINT8)(pt_info->at_comp_info[2].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1YCompFactorV = (UINT8)(pt_info->at_comp_info[0].i2_v_samp_factor);
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = (UINT8)(pt_info->at_comp_info[1].i2_v_samp_factor);
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = (UINT8)(pt_info->at_comp_info[2].i2_v_samp_factor);
    }

    rSrcBufInfo.fgInterlaced = FALSE;
    rSrcBufInfo.fgTopField = TRUE;
    rSrcBufInfo.fgBottomField = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket, &rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    switch(prJdecInst->eDstColorMode)
    {
        case GFX_COLORMODE_AYCbCr_D8888:
        case GFX_COLORMODE_ARGB_D8888:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_AYUV;
            break;
        case GFX_COLORMDOE_YUV_420_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
            break;
        case GFX_COLORMDOE_YUV_422_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_BLK;
            break;
        case GFX_COLORMODE_YUV_420_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_RS;
            break;
        case GFX_COLORMODE_YUV_422_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_RS;
            break;
        default:
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
    }
    rDstBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rDstBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    if ((rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK) ||
        (rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_RS))
    {
        rDstBufInfo.u4PicWidth = (rDstBufInfo.u4PicWidth + 1)/2 * 2;
    }
    rDstBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
#if JDEC_SUPPORT_BROKEN_TARGET
    if ((prJdecInst->fgBrokenTarget) && (prJdecInst->fgBrokenRow))
    {
        rDstBufInfo.u4YBufAddr = prJdecInst->u4TempAYUVBuf;
        rDstBufInfo.u4CBufAddr = 0;
    }
    else
#endif
    {
    rDstBufInfo.u4YBufAddr = (UINT32)(prJdecInst->pvDstYBuf);
    rDstBufInfo.u4CBufAddr = (UINT32)(prJdecInst->pvDstCBuf);
    }
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
    VERIFY(i4Ret >= 0);

    rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
    rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleFire(JDEC_INST_T *prJdecInst)
{
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_DO_SCALE_T rDoScale;
    INT32 i4Ret;

    if (prJdecInst->fgJdecHwScale == FALSE)
    {
        return i4JdecScale(prJdecInst);
    }

    prJdecInst->i4JdecScaleState = -1;
    //rDoScale.eScalePriority = IMGRESZ_DRV_PRIORITY_LOW;
    i4Ret = i4ImgResz_Drv_DoScale(prImgReszTicket, &rDoScale);
    VERIFY(i4Ret >= 0);

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleStop(JDEC_INST_T *prJdecInst)
{
    if (i4ImgResz_Drv_StopScale(&prJdecInst->rImgReszTicket) != S_IMGRESZ_DRV_OK)
    {
        ASSERT(0);
        return JDEC_DRV_RTN_FAIL;
    }
    else
    {
        i4JdecScaleWaitFinish(prJdecInst);
    }
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleSetNextPartialBuf(JDEC_INST_T *prJdecInst)
{
    IMGRESZ_DRV_PARTIAL_INFO_T rPartialBufInfo;
    UINT32 u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow % JDEC_CLR_BUF_NUM);
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    INT32 i4Ret;

    if (prJdecInst->fgJdecHwScale == FALSE)
    {
        return JDEC_DRV_RTN_OK;
    }

    if(prJdecInst->u4CurrDecodeMCURow == 0)
    {
        rPartialBufInfo.fgFirstRow = TRUE;
    }
    else
    {
        rPartialBufInfo.fgFirstRow = FALSE;
    }
    if(prJdecInst->u4CurrDecodeMCURow == (prJdecInst->u4MCURowNum - 1))
    {
        rPartialBufInfo.fgLastRow = TRUE;
    }
    else
    {
        rPartialBufInfo.fgLastRow = FALSE;
    }
    rPartialBufInfo.u4YBufLine = 0; //prJdecInst->u4SrcCropHeight;
    rPartialBufInfo.u4CbBufLine = 0; //prJdecInst->u4SrcCropHeight;
    rPartialBufInfo.u4CrBufLine = 0; //prJdecInst->u4SrcCropHeight;

    rPartialBufInfo.u4YBufAddr = prJdecInst->au4ColorBufSa[u4CurrentBufId][0];
    rPartialBufInfo.u4CbBufAddr = prJdecInst->au4ColorBufSa[u4CurrentBufId][1];
    rPartialBufInfo.u4CrBufAddr = prJdecInst->au4ColorBufSa[u4CurrentBufId][2];

    if ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530) 
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
        || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif
        )
    {
        rPartialBufInfo.u4YBufAddr = prJdecInst->au4ColorBufSa[1][0];
        rPartialBufInfo.u4CbBufAddr = prJdecInst->au4ColorBufSa[1][1];
        rPartialBufInfo.u4CrBufAddr = prJdecInst->au4ColorBufSa[1][2];
    }
     if(prJdecInst->fgSwDecode)
     {
      vJdecLog_L("[Jdec]software case \n");  
 #if JDEC_USE_MAP_UNMAP
  // vJdecMapPrintf("[JDEC_SW]Map color buffer Y from 0x%x to 0x%x , point 3 \n",rPartialBufInfo.u4YBufAddr, (rPartialBufInfo.u4YBufAddr + prJdecInst->au4ColorBufSz[0]));
   BSP_dma_map_vaddr(rPartialBufInfo.u4YBufAddr, prJdecInst->au4ColorBufSz[0], BIDIRECTIONAL);
   BSP_dma_map_vaddr(rPartialBufInfo.u4CbBufAddr, prJdecInst->au4ColorBufSz[1], BIDIRECTIONAL);
   BSP_dma_map_vaddr(rPartialBufInfo.u4CrBufAddr, prJdecInst->au4ColorBufSz[2], BIDIRECTIONAL);   
#else
   BSP_FlushDCacheRange(rPartialBufInfo.u4YBufAddr, prJdecInst->au4ColorBufSz[0]);
   BSP_FlushDCacheRange(rPartialBufInfo.u4CbBufAddr, prJdecInst->au4ColorBufSz[1]);
   BSP_FlushDCacheRange(rPartialBufInfo.u4CrBufAddr, prJdecInst->au4ColorBufSz[2]);
#endif       
     }
    i4Ret = i4ImgResz_Drv_SetPartialBufInfo(prImgReszTicket, &rPartialBufInfo);
     VERIFY(i4Ret >= 0);

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleSetJpegInfo(JDEC_INST_T *prJdecInst)
{
    IMGRESZ_DRV_JPEG_INFO_T rJpegInfo;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
	JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    INT32 i4Ret, i4;

    rJpegInfo.fgExistY = prJdecInst->fgDefaultScale[0];
    rJpegInfo.fgExistCb = prJdecInst->fgDefaultScale[1];
    rJpegInfo.fgExistCr = prJdecInst->fgDefaultScale[2];
    rJpegInfo.fgPreload = FALSE;

    if (prJdecInst->ptImgJpgObj == NULL)
    {
        if (prSos->u1CompInScan > MAX_JPEG_COMPONENT)
        {
            ASSERT(0);
        }
    for (i4=0; i4<prSos->u1CompInScan; i4++)
    {
            switch(prSos->au1CompIdx[i4])
        {
                case 0:
            rJpegInfo.fgExistY = TRUE;
                    break;
                case 1:
            rJpegInfo.fgExistCb = TRUE;
                    break;
                case 2:
            rJpegInfo.fgExistCr = TRUE;
                    break;
        }
            if ((prJdecInst->au4CompScanCount[prSos->au1CompIdx[i4]] > 1) &&
            ((prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
             || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
             || (prJdecInst->eJdecDecMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#endif        
            ))
        {
            rJpegInfo.fgPreload = TRUE;
        }
    }
    }
    else
    {
        rJpegInfo.fgExistY = TRUE;
        rJpegInfo.fgExistCb = TRUE;
        rJpegInfo.fgExistCr = TRUE;
        rJpegInfo.fgPreload = FALSE;
    }
    rJpegInfo.u4HwId = prJdecInst->u4HwId; // use image resizer1 to work with jdec hw 1
    i4Ret = i4ImgResz_Drv_SetJpegInfo(prImgReszTicket, &rJpegInfo);
    VERIFY(i4Ret >= 0);
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleYCbCr2RGB(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;

    i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    x_memset((void *)&rSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    x_memset((void *)&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    // Set source buffer info
    rSrcBufInfo.eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_AYUV;
    rSrcBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rSrcBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rSrcBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    rSrcBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rSrcBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rSrcBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
    rSrcBufInfo.u4YBufAddr = (UINT32)(prJdecInst->pvDstYBuf);
    rSrcBufInfo.u4CbBufAddr = 0;
    rSrcBufInfo.u4CrBufAddr = 0;
    rSrcBufInfo.u4ColorPalletSa = 0;
    rSrcBufInfo.rCompFactor.u1YCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1CbCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1CrCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1YCompFactorV = 0;
    rSrcBufInfo.rCompFactor.u1CbCompFactorV = 0;
    rSrcBufInfo.rCompFactor.u1CrCompFactorV = 0;
    rSrcBufInfo.fgInterlaced = FALSE;
    rSrcBufInfo.fgTopField = TRUE;
    rSrcBufInfo.fgBottomField = FALSE;
    rSrcBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket, &rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
    rDstBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rDstBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    rDstBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
    rDstBufInfo.u4YBufAddr = (UINT32)(prJdecInst->pvDstYBuf);
    rDstBufInfo.u4CBufAddr = (UINT32)(prJdecInst->pvDstCBuf);
    rDstBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
    VERIFY(i4Ret >= 0);

    rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
    rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    i4JdecScaleFire(prJdecInst);
    i4Ret = i4JdecScaleWaitFinish(prJdecInst);
    vJdecSetLog((UINT32)(i4Ret) + 0x80000000);
    if (i4Ret != JDEC_DRV_RTN_OK)
    {
        return JDEC_DRV_RTN_FAIL;
    }

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScaleRs2Blk(JDEC_INST_T *prJdecInst)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;

    i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    x_memset(&rSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    // Set source buffer info
    rSrcBufInfo.eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_420_RS;
    rSrcBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rSrcBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rSrcBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    rSrcBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rSrcBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rSrcBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
    //Modify by pinghao ,2008/9/22, temp solution
    rSrcBufInfo.u4YBufAddr = 0;//(UINT32)(g_pvJpegBuffer);
    rSrcBufInfo.u4CbBufAddr = 0;//(UINT32)(g_pvJpegBuffer)+(1920*1088);
    rSrcBufInfo.u4CrBufAddr = 0;
    rSrcBufInfo.u4ColorPalletSa = 0;
    rSrcBufInfo.rCompFactor.u1YCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1CbCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1CrCompFactorH = 0;
    rSrcBufInfo.rCompFactor.u1YCompFactorV = 0;
    rSrcBufInfo.rCompFactor.u1CbCompFactorV = 0;
    rSrcBufInfo.rCompFactor.u1CrCompFactorV = 0;
    rSrcBufInfo.fgInterlaced = FALSE;
    rSrcBufInfo.fgTopField = TRUE;
    rSrcBufInfo.fgBottomField = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket, &rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    x_memset(&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    // Set target buffer info
    rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
    rDstBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rDstBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    rDstBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
    rDstBufInfo.u4YBufAddr = (UINT32)(prJdecInst->pvDstYBuf);
    rDstBufInfo.u4CBufAddr = (UINT32)(prJdecInst->pvDstCBuf);
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
    VERIFY(i4Ret >= 0);

    rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
    rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    i4JdecScaleFire(prJdecInst);
    i4Ret = i4JdecScaleWaitFinish(prJdecInst);
    vJdecSetLog((UINT32)(i4Ret) + 0x80000000);
    if (i4Ret != JDEC_DRV_RTN_OK)
    {
        return JDEC_DRV_RTN_FAIL;
    }

    return JDEC_DRV_RTN_OK;
}

INT32 i4Jdec420RsScale(JDEC_INST_T *prJdecInst)
{
#if 0    
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
	JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    INT32 i4, j4, k4, l4, m4;
    UINT32 u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow % JDEC_CLR_BUF_NUM);
    UINT8 *pu1Src;
    UINT32 *pu4Src;
    UINT32 u4MCUClrBufHeight; // = (((prSof->u2ImageHeight / prJdecInst->u4MCURowNum) + 7) / 8) * 8;
    UINT8 *puYDst; // = (UINT8 *)((UINT32)prJdecInst->pvDstYBuf + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight);
    UINT8 *puCDst; // = (UINT8 *)((UINT32)prJdecInst->pvDstCBuf + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight / 2);
    UINT8 *pu1Dst = NULL;
    UINT32 *pu4Dst;
    UINT32 u4CompIdx;
    UINT32 u4HFactor, u4VFactor;
    UINT32 u4ValTmp;
    INT32  i4DstCount, i4SrcCount;
    UINT32 u4CompClrBufPitch = 0;
    UINT32 u4WidthCount;
    UINT32 u4CbCrNextPixel, u4CbCrNextLine;
    UINT32 u4Temp = 0;

    prJdecInst->i4JdecScaleState = -1;

    m4 = 0;
    l4 = 0;
    i4 = 0;
    j4 = 0;

    if (prJdecPicInfo->u4MaxVFactor == 2)
    {
        u4MCUClrBufHeight = 16;
    }
    else
    {
        u4MCUClrBufHeight = 8;
    }

    //Modify by pingzhao for remove g_pvJpegBuffer
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    u4Temp = (UINT32)x_alloc_aligned_dma_mem(1920*1088*2, 32);
    #else
    u4Temp = (UINT32)x_alloc_aligned_nc_mem(1920*1088*2, 32);
    #endif
    VERIFY(u4Temp != NULL);
    puYDst = (UINT8 *)((UINT32)u4Temp + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight);
    puCDst = (UINT8 *)((UINT32)u4Temp + 1920*1088 + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight / 2);
    //puYDst = (UINT8 *)((UINT32)g_pvJpegBuffer + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight);
    //puCDst = (UINT8 *)((UINT32)g_pvJpegBuffer + 1920*1088 + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight / 2);

    if (prSos->u1CompInScan > MAX_JPEG_COMPONENT)
    {
        ASSERT(0);
    }
    for (i4=0; i4<prSos->u1CompInScan; i4++)
    {
        u4CompIdx = prSos->au1CompIdx[i4];
        pu1Src = (UINT8 *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][u4CompIdx]);
        u4HFactor = prJdecPicInfo->u4MaxVFactor / prSof->arSofComp[u4CompIdx].u1VSampFactor;
        u4VFactor = prJdecPicInfo->u4MaxHFactor / prSof->arSofComp[u4CompIdx].u1HSampFactor;
        u4CompClrBufPitch = prJdecInst->au4ColorBufPitch[u4CompIdx];
        if (u4CompIdx == 0)
        {
            pu4Dst = (UINT32 *)puYDst;
            pu4Dst += (prJdecInst->u4DstBufPitch * prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropPosX) /4;
            pu4Src = (UINT32 *)pu1Src;
            u4WidthCount = (prSof->u2ImageWidth + 3) / 4;
            for (j4=0; j4<u4MCUClrBufHeight; j4++)
            {
                i4DstCount = 0;
                i4SrcCount = 0;
                for (k4=0; k4<u4WidthCount; k4++)
                {
                    if (prJdecInst->au4CompScanCount[u4CompIdx] == 1)
                    {
                        //pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = pu1Src[u4CompIdx][prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                        pu4Dst[i4DstCount] = pu4Src[i4SrcCount];
                        i4DstCount ++;
                        i4SrcCount ++;
                    }
                    else
                    {
                        u4ValTmp = (UINT32)(pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)]);
                        u4ValTmp += pu1Src[prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                        u4ValTmp -= 0x80;
                        pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = (UINT8)(u4ValTmp & 0xFF);
                    }
                }
                pu4Dst += (prJdecInst->u4DstBufPitch / 4);
                pu4Src += (u4CompClrBufPitch / 4);
            }
        }
        else
        {
            if (u4HFactor == 1)
            {
                u4CbCrNextPixel = 2;    //444
            }
            else
            {
                u4CbCrNextPixel = 1;    //420 or 422
            }
            if (u4VFactor == 1)
            {
                u4CbCrNextLine = 2; //444 or 422
            }
            else
            {
                u4CbCrNextLine = 1; //420
            }
            pu1Dst = puCDst;
            pu1Dst += prJdecInst->u4DstBufPitch * (prJdecInst->u4DstCropPosY/2) + prJdecInst->u4DstCropPosX;
            if (u4CompIdx == 2)
            {
                pu1Dst += 1;
            }
            u4WidthCount = prSof->u2ImageWidth;
            for (j4=0; j4<(u4MCUClrBufHeight/u4VFactor); j4++)
            {
                i4DstCount = 0;
                i4SrcCount = 0;
                for (k4=0; k4<u4WidthCount; k4++)
                {
                    if (prJdecInst->au4CompScanCount[u4CompIdx] == 1)
                    {
                        //pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = pu1Src[u4CompIdx][prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                        pu1Dst[i4DstCount] = pu1Src[i4SrcCount];
                        i4DstCount += 2;
                        i4SrcCount += u4CbCrNextPixel;
                    }
                    else
                    {
                        u4ValTmp = (UINT32)(pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)]);
                        u4ValTmp += pu1Src[prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                        u4ValTmp -= 0x80;
                        pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = (UINT8)(u4ValTmp & 0xFF);
                    }
                }
                pu1Dst += prJdecInst->u4DstBufPitch;
                pu1Src += (u4CompClrBufPitch * u4CbCrNextLine);
            }
        }
    }

    prJdecInst->i4JdecScaleState = 0;
#endif    
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecAYCbCr8888Scale(JDEC_INST_T *prJdecInst)
{
    JDEC_PIC_INFO_T *prJdecPicInfo = (JDEC_PIC_INFO_T *)prJdecInst->pvPicCache;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
	JDEC_JFIF_SOS_T *prSos = &(prJfifInst->rSOS);
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    INT32 i4, j4, k4, l4, m4;
    UINT32 u4CurrentBufId = (prJdecInst->u4CurrDecodeMCURow % JDEC_CLR_BUF_NUM);
    UINT8 *pu1Src[3];
    UINT32 u4MCUClrBufHeight = prSof->u2ImageHeight / prJdecInst->u4MCURowNum;
    UINT8 *pu1Dst = (UINT8 *)((UINT32)prJdecInst->pvDstYBuf + prJdecInst->u4CurrDecodeMCURow * prJdecInst->u4DstBufPitch * u4MCUClrBufHeight);
    UINT32 u4CompIdx;
    UINT32 u4HFactor, u4VFactor;
    UINT32 u4ValTmp;

    prJdecInst->i4JdecScaleState = -1;
    pu1Dst += prJdecInst->u4DstBufPitch * prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropPosX * 4;

    pu1Src[0] = (UINT8 *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][0]);
    pu1Src[1] = (UINT8 *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][1]);
    pu1Src[2] = (UINT8 *)(prJdecInst->au4ColorBufSa[u4CurrentBufId][2]);

    if (prSos->u1CompInScan > MAX_JPEG_COMPONENT)
    {
        ASSERT(0);
    }
    for (i4=0; i4<prSos->u1CompInScan; i4++)
    {
        u4CompIdx = prSos->au1CompIdx[i4];
        if (u4CompIdx > (MAX_JPEG_COMPONENT - 1))
        {
            ASSERT(0);
        }
        u4HFactor = prJdecPicInfo->u4MaxVFactor / prSof->arSofComp[u4CompIdx].u1VSampFactor;
        u4VFactor = prJdecPicInfo->u4MaxHFactor / prSof->arSofComp[u4CompIdx].u1HSampFactor;
        for (j4=0; j4<u4MCUClrBufHeight; j4++)
        {
            for (m4=0; m4<u4VFactor; m4++)
            {
                for (k4=0; k4<prSof->u2ImageWidth; k4++)
                {
                    for (l4=0; l4<u4HFactor; l4++)
                    {
                        if (prJdecInst->au4CompScanCount[u4CompIdx] == 1)
                        {
                            pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = pu1Src[u4CompIdx][prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                        }
                        else
                        {
                            u4ValTmp = (UINT32)(pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)]);
                            u4ValTmp += pu1Src[u4CompIdx][prJdecInst->au4ColorBufPitch[u4CompIdx]*j4/u4VFactor + k4/u4HFactor];
                            u4ValTmp -= 0x80;
                            pu1Dst[prJdecInst->u4DstBufPitch*(j4+m4) + (k4+l4)*4 + (2-u4CompIdx)] = (UINT8)(u4ValTmp & 0xFF);
                        }
                    }
                    k4 += (u4HFactor - 1);
                }
            }
            j4 += (u4VFactor - 1);
        }
    }

    prJdecInst->i4JdecScaleState = 0;
    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecScale(JDEC_INST_T *prJdecInst)
{
    if (prJdecInst->eDstColorMode == GFX_COLORMDOE_YUV_420_BLK)
    {
        return i4Jdec420RsScale(prJdecInst);
    }
    else
    {
        return i4JdecAYCbCr8888Scale(prJdecInst);
    }
}

#if JDEC_SUPPORT_BROKEN_TARGET

extern void vJdecCopyPreloadData(JDEC_INST_T *prJdecInst);

INT32 i4JdecSliceScaleInit(JDEC_INST_T *prJdecInst)
{
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;
    JDEC_JFIF_SOF_T *prSof = &(prJfifInst->rSOF);
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    INT32 i4Ret;
    IMG_BASIC_INFO_T *pt_info = NULL;

    if (prJdecInst->ptImgJpgObj == NULL)
    {
        if ((prJdecInst->u4SrcCropPosX != 0) || (prJdecInst->u4SrcCropPosY != 0) ||
            (prJdecInst->u4SrcCropWidth != prSof->u2ImageWidth) ||
            (prJdecInst->u4SrcCropHeight!= prSof->u2ImageHeight) ||
            (prJdecInst->eDstRotate != IMG_ROTATE_NONE))
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }
    else
    {
        pt_info = prJdecInst->ptImgJpgObj->pt_current_sel;
        if ((prJdecInst->u4SrcCropPosX != 0) || (prJdecInst->u4SrcCropPosY != 0) ||
            (prJdecInst->u4SrcCropWidth != pt_info->ui2_image_width) ||
            (prJdecInst->u4SrcCropHeight!= pt_info->ui2_image_height) ||
            (prJdecInst->eDstRotate != IMG_ROTATE_NONE))
        {
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
        }
    }


    i4Ret = i4ImgResz_Drv_SetPriority(prImgReszTicket, IMGRESZ_DRV_PRIORITY_LOW);
    VERIFY(i4Ret >= 0);

    i4Ret = i4ImgResz_Drv_SetLock(prImgReszTicket, TRUE);
    VERIFY(i4Ret >= 0);

    
    i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_PARTIAL_SCALE);
    
    VERIFY(i4Ret >= 0);

    x_memset((void *)&rSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    x_memset((void *)&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    
    // Set source buffer info
    rSrcBufInfo.eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF;
    rSrcBufInfo.u4BufWidth = prJdecInst->au4ColorBufPitch[0];
    rSrcBufInfo.u4BufHeight = 0; //prJdecInst->u4SrcCropHeight;
    rSrcBufInfo.u4PicWidth = prJdecInst->u4SrcCropWidth;
    rSrcBufInfo.u4PicHeight = prSof->arSofComp[0].u1VSampFactor * 8;
    rSrcBufInfo.u4PicPosX = prJdecInst->u4SrcCropPosX;
    rSrcBufInfo.u4PicPosY = prJdecInst->u4SrcCropPosY;
    rSrcBufInfo.u4YBufAddr = prJdecInst->au4ColorBufSa[0][0];
    rSrcBufInfo.u4CbBufAddr = prJdecInst->au4ColorBufSa[0][1];
    rSrcBufInfo.u4CrBufAddr = prJdecInst->au4ColorBufSa[0][2];
    rSrcBufInfo.u4ColorPalletSa = 0;

    if (prJdecInst->ptImgJpgObj == NULL)
    {
        rSrcBufInfo.rCompFactor.u1YCompFactorH = prSof->arSofComp[0].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = prSof->arSofComp[1].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = prSof->arSofComp[2].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1YCompFactorV = prSof->arSofComp[0].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = prSof->arSofComp[1].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = prSof->arSofComp[2].u1VSampFactor;
#if JDEC_SUPPORT_BROKEN_TARGET
        if (prJdecInst->fgBrokenTarget)
        {
        rSrcBufInfo.rCompFactor.u1YCompFactorH = 2;//prSof->arSofComp[0].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = 2;//prSof->arSofComp[1].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = 2;//prSof->arSofComp[2].u1HSampFactor;
        rSrcBufInfo.rCompFactor.u1YCompFactorV = 2;//prSof->arSofComp[0].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = 2;//prSof->arSofComp[1].u1VSampFactor;
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = 2;//prSof->arSofComp[2].u1VSampFactor;
        }
#endif        
    }
    else    //sw decode
    {
        rSrcBufInfo.rCompFactor.u1YCompFactorH = (UINT8)(pt_info->at_comp_info[0].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = (UINT8)(pt_info->at_comp_info[1].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = (UINT8)(pt_info->at_comp_info[2].i2_h_samp_factor);
        rSrcBufInfo.rCompFactor.u1YCompFactorV = (UINT8)(pt_info->at_comp_info[0].i2_v_samp_factor);
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = (UINT8)(pt_info->at_comp_info[1].i2_v_samp_factor);
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = (UINT8)(pt_info->at_comp_info[2].i2_v_samp_factor);
    }

    rSrcBufInfo.fgInterlaced = FALSE;
    rSrcBufInfo.fgTopField = TRUE;
    rSrcBufInfo.fgBottomField = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket, &rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    switch(prJdecInst->eDstColorMode)
    {
        case GFX_COLORMODE_AYCbCr_D8888:
        case GFX_COLORMODE_ARGB_D8888:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_AYUV;
            break;
        case GFX_COLORMDOE_YUV_420_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
            break;
        case GFX_COLORMDOE_YUV_422_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_BLK;
            break;
        case GFX_COLORMODE_YUV_420_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_RS;
            break;
        case GFX_COLORMODE_YUV_422_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_RS;
            break;
        default:
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
    }
    rDstBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rDstBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    if ((rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK) ||
        (rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_RS))
    {
        rDstBufInfo.u4PicWidth = (rDstBufInfo.u4PicWidth + 1)/2 * 2;
    }
    rDstBufInfo.u4PicHeight = prSof->arSofComp[0].u1VSampFactor * 8;
    rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
    rDstBufInfo.u4YBufAddr = prJdecInst->u4TempAYUVBuf;//(UINT32)(prJdecInst->pvDstYBuf);
    rDstBufInfo.u4CBufAddr = 0;//(UINT32)(prJdecInst->pvDstCBuf);
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
    VERIFY(i4Ret >= 0);

    rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
    rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    return JDEC_DRV_RTN_OK;
}

INT32 i4JdecSetSliceDstInfo(JDEC_INST_T *prJdecInst)
{
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    INT32 i4Ret = 0;
    INT32 i4Len = 0;
    UINT32 u4TempLine = 0;
    JPEG_JFIF_HEADER_T *prJfifInst = (JPEG_JFIF_HEADER_T *)prJdecInst->hJfifInst;

    
    x_memset((void *)&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    // Set target buffer info
    switch(prJdecInst->eDstColorMode)
    {
        case GFX_COLORMODE_AYCbCr_D8888:
        case GFX_COLORMODE_ARGB_D8888:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_AYUV;
            break;
        case GFX_COLORMDOE_YUV_420_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
            break;
        case GFX_COLORMDOE_YUV_422_BLK:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_BLK;
            break;
        case GFX_COLORMODE_YUV_420_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_RS;
            break;
        case GFX_COLORMODE_YUV_422_RS:
            rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_422_RS;
            break;
        default:
            ASSERT(0);
            return JDEC_DRV_RTN_FAIL;
    }
    rDstBufInfo.u4BufWidth = prJdecInst->u4DstBufPitch;
    rDstBufInfo.u4BufHeight = prJdecInst->u4DstCropPosY + prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicWidth = prJdecInst->u4DstCropWidth;
    if ((rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK) ||
        (rDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_RS))
    {
        rDstBufInfo.u4PicWidth = (rDstBufInfo.u4PicWidth + 1)/2 * 2;
    }
    rDstBufInfo.u4PicHeight = prJdecInst->u4DstCropHeight;
    rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
    rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;

    /* How to judge when to broken row and reset address
    1. If the block leave space is not enough to fill a mcu row ,then it comes a broken row
    2. Imgresz partial mode is a special mode, for example, if mcu row height is 8 lines, 
       for first row, image resizer will output 7line to target buffer ,and keep one line in 
       temp line buffer , in order to do some mixed with the first in next row , so begin from second 
       row, imgresz will output 8 lines each time , one line will output to last row last line ,the other
       7 lines will output to current row, loop until last, for last row ,it will output all lines left

    3. In mmu mode, in the first block ,we don't use reset address ,and let imgresz to calculate the 
       address itself, but at the end of the block, if the leave size is between the range :
       left size < mcu row length < left size + dest picth ,
       actually ,we don't need use broken row ,only reset the address ,and let the blk ofst 
       move back (about one line pitch size ) and the let the imgresz to output directly

       if the left size is in the range :
       mcu row length > left size + dest picth

       so we use broken line, first , let the imgresz output to the temp buffer,
       after resize done,we copy the data from the temp buffer to the end of the 
       last block and start of the new block

    */
    // Check if broken row

    u4TempLine = prJdecInst->u4DstBufPitch;

    if ((prJfifInst->rSOS.u1CompInScan == 1) && 
        (prJfifInst->rSOF.arSofComp[0].u1VSampFactor == 2) && 
        (prJfifInst->rSOF.arSofComp[prJfifInst->rSOS.au1CompIdx[0]].u1VSampFactor != 2))
    {
        u4TempLine = prJdecInst->u4DstBufPitch * 2;
    }
    i4Len = prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst;
    if (i4Len < 0)
    {
        prJdecInst->fgBrokenRow = TRUE;
    }   
    else if (prJdecInst->rBlkInfo.u4BlkIdx !=  (prJdecInst->rBlkInfo.u4TotalBlk)) // not last block
    {
        if (prJdecInst->u4McuRowLength > (prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst))
        {
            if (prJdecInst->rBlkInfo.u4BlkIdx == 0)
            {
                if (prJdecInst->u4McuRowLength > (prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst + u4TempLine /*prJdecInst->u4DstBufPitch*/))
                {
                    prJdecInst->fgBrokenRow = TRUE;
                    prJdecInst->fgNeedJumpBack = TRUE;
                }
                else
                {
                    prJdecInst->fgBrokenRow = FALSE;
                }
            }
            else
            {
                prJdecInst->fgBrokenRow = TRUE;
            }
        }
        else
        {
            prJdecInst->fgBrokenRow = FALSE;
        }
    }
    else
    {
        prJdecInst->fgBrokenRow = FALSE;
    }

    vJdecLog_L("[JDEC] i4JdecSetSliceDstInfo Blk Index = %d blk offset = %x fgBroken row = %d \n",
                prJdecInst->rBlkInfo.u4BlkIdx,prJdecInst->rBlkInfo.u4BlkOfst,prJdecInst->fgBrokenRow);
    if (prJdecInst->fgBrokenRow)
    {
        rDstBufInfo.u4YBufAddr = prJdecInst->u4TempAYUVBuf;
        rDstBufInfo.u4CBufAddr = 0;
        vJdecCopyPreloadData(prJdecInst);
    }
    else
    {
        rDstBufInfo.u4YBufAddr = prJdecInst->rBlkInfo.u4BlkSa + prJdecInst->rBlkInfo.u4BlkOfst;
        rDstBufInfo.u4CBufAddr = 0;
    }

    vJdecLog_L("[JDEC_SCALE] Dest buffer = 0x%x \n",rDstBufInfo.u4YBufAddr);

    if ((prJdecInst->rBlkInfo.u4BlkIdx != 0) || prJdecInst->fgBrokenRow) //(prJdecInst->u4CurrDecodeMCURow > 1)
    {
        rDstBufInfo.fgResetSA = TRUE;
    }
    else
    {
        rDstBufInfo.fgResetSA = FALSE;
    }
    // special case

    if ((prJdecInst->rBlkInfo.u4BlkIdx == 0) && (prJdecInst->u4McuRowLength > (prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst)) &&
         (prJdecInst->u4McuRowLength <= (prJdecInst->rBlkInfo.u4BlkSize - prJdecInst->rBlkInfo.u4BlkOfst + u4TempLine /*prJdecInst->u4DstBufPitch*/)))
    {
        rDstBufInfo.fgResetSA = TRUE;
        prJdecInst->rBlkInfo.u4BlkOfst -= /*prJdecInst->u4DstBufPitch*/u4TempLine;
        rDstBufInfo.u4YBufAddr -= u4TempLine;
        prJdecInst->fgNeedJumpBack = FALSE;
        UTIL_Printf("[JDEC] Reset address and roll back block ofst \n");
    }
    
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
    VERIFY(i4Ret >= 0);

    rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
    rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    return JDEC_DRV_RTN_OK;

}

INT32 i4JdecScaleSlicedYCbCr2RGB(JDEC_INST_T *prJdecInst)
{
    UINT32 u4Temp = 0;
    INT32 i4Ret = 0;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &(prJdecInst->rImgReszTicket);
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    HANDLE_T hTemp = NULL_HANDLE;
    UINT32 u4TempSA = 0;

    hTemp = prJdecInst->rBlkInfo.hMem;//x_kmem_block_handle(prJdecInst->pvDstYBuf);
    
    while (1)
    {
        i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket, IMGRESZ_DRV_FRAME_SCALE);
        VERIFY(i4Ret >= 0);

        x_memset((void *)&rSrcBufInfo, 0, sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
        x_memset((void *)&rDstBufInfo, 0, sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
        // Set source buffer info
        rSrcBufInfo.eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_AYUV;
        //if (prJdecInst->rBlkInfo.u4BlkSize % 8 == 0)
        vJdecLog_L("[JDEC] u4Temp = %d \n",u4Temp);
        if (u4Temp != (prJdecInst->rBlkInfo.u4TotalBlk))
        {
            rSrcBufInfo.u4BufWidth = prJdecInst->rBlkInfo.u4BlkSize / 256;
            rSrcBufInfo.u4BufHeight = 256;

            rSrcBufInfo.u4PicWidth = prJdecInst->rBlkInfo.u4BlkSize / 256 / 4;
            rSrcBufInfo.u4PicHeight = 256;
            rSrcBufInfo.u4YBufAddr = (UINT32)x_kmem_kernel_address(hTemp, prJdecInst->rBlkInfo.u4BlkSize * u4Temp);
        }
        else
        {
            if (prJdecInst->rBlkInfo.u4LastBlkSize % 256  != 0)
            {
                //UTIL_Printf("[JDEC]last block size = %x \n",prJdecInst->rBlkInfo.u4LastBlkSize);
                //ASSERT(0);
                UINT32 u4Size = 0;
                UINT32 u4SA = 0;

                u4SA = (UINT32)x_kmem_kernel_address(hTemp, prJdecInst->rBlkInfo.u4BlkSize * u4Temp);

                u4Size = (prJdecInst->rBlkInfo.u4LastBlkSize + 255)/256*256;
                u4TempSA = (UINT32)x_mem_aligned_alloc(u4Size, 32);
                //UTIL_Printf("[JDEC] allocate 0x%x for scale %x\n",u4Size,u4TempSA);
                x_memset((void *)u4TempSA, 0, u4Size);
                x_memcpy((void *)u4TempSA, (void *)u4SA, prJdecInst->rBlkInfo.u4LastBlkSize);
                #if !JDEC_USE_MAP_UNMAP
                BSP_FlushDCacheRange(u4TempSA, prJdecInst->rBlkInfo.u4LastBlkSize);
                #endif
                rSrcBufInfo.u4BufWidth = 256;
                rSrcBufInfo.u4BufHeight = u4Size / 256;
                rSrcBufInfo.u4PicWidth = 256 / 4;
                rSrcBufInfo.u4PicHeight = u4Size / 256;
                rSrcBufInfo.u4YBufAddr = u4TempSA;
            }
            else
            {
                rSrcBufInfo.u4BufWidth = 256;
                rSrcBufInfo.u4BufHeight = prJdecInst->rBlkInfo.u4LastBlkSize / 256;
                rSrcBufInfo.u4PicWidth = 256 / 4;
                rSrcBufInfo.u4PicHeight = prJdecInst->rBlkInfo.u4LastBlkSize / 256;
                rSrcBufInfo.u4YBufAddr = (UINT32)x_kmem_kernel_address(hTemp, prJdecInst->rBlkInfo.u4BlkSize * u4Temp);
            }
            
        }
        
        vJdecLog_L("[JDEC] Scale src width = %d height = %d \n",
                     rSrcBufInfo.u4BufWidth,
                     rSrcBufInfo.u4BufHeight);
        
        rSrcBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
        rSrcBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
        rSrcBufInfo.u4CbBufAddr = 0;
        rSrcBufInfo.u4CrBufAddr = 0;
        rSrcBufInfo.u4ColorPalletSa = 0;
        rSrcBufInfo.rCompFactor.u1YCompFactorH = 0;
        rSrcBufInfo.rCompFactor.u1CbCompFactorH = 0;
        rSrcBufInfo.rCompFactor.u1CrCompFactorH = 0;
        rSrcBufInfo.rCompFactor.u1YCompFactorV = 0;
        rSrcBufInfo.rCompFactor.u1CbCompFactorV = 0;
        rSrcBufInfo.rCompFactor.u1CrCompFactorV = 0;
        rSrcBufInfo.fgInterlaced = FALSE;
        rSrcBufInfo.fgTopField = TRUE; //?
        rSrcBufInfo.fgBottomField = FALSE;
        rSrcBufInfo.fgWTEnable = FALSE;
        i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket, &rSrcBufInfo);
        VERIFY(i4Ret >= 0);

        // Set target buffer info
        rDstBufInfo.eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;

        if (u4Temp != (prJdecInst->rBlkInfo.u4TotalBlk))
        {
            rDstBufInfo.u4BufWidth = prJdecInst->rBlkInfo.u4BlkSize / 256;
            rDstBufInfo.u4BufHeight = 256;
            rDstBufInfo.u4PicWidth = prJdecInst->rBlkInfo.u4BlkSize / 256/4;
            rDstBufInfo.u4PicHeight = 256;
        }
        else
        {
            rDstBufInfo.u4BufWidth = rSrcBufInfo.u4BufWidth;
            rDstBufInfo.u4BufHeight = rSrcBufInfo.u4BufHeight;
            rDstBufInfo.u4PicWidth = rSrcBufInfo.u4PicWidth;
            rDstBufInfo.u4PicHeight = rSrcBufInfo.u4PicHeight;
        }
        vJdecLog_L("[JDEC] Scale dst width = %d height = %d \n",
                     rDstBufInfo.u4BufWidth,
                     rDstBufInfo.u4BufHeight);
        rDstBufInfo.u4PicPosX = prJdecInst->u4DstCropPosX;
        rDstBufInfo.u4PicPosY = prJdecInst->u4DstCropPosY;
        rDstBufInfo.u4YBufAddr = rSrcBufInfo.u4YBufAddr;//(UINT32)x_kmem_kernel_address(hTemp, prJdecInst->rBlkInfo.u4BlkSize * u4Temp);
        rDstBufInfo.u4CBufAddr = 0;
        rDstBufInfo.fgWTEnable = FALSE;
        rDstBufInfo.fgResetSA = FALSE;

        vJdecLog_L("[JDEC] Src buff SA = %x Dst buff SA = %x \n",rSrcBufInfo.u4YBufAddr,rDstBufInfo.u4YBufAddr);
        i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket, &rDstBufInfo);
        VERIFY(i4Ret >= 0);

        rNotifyCallbackReg.pvCallBackFunc = i4JdecScaleCB;
        rNotifyCallbackReg.pvPrivData = (void *)((UINT32)prJdecInst->u2CompId);
        i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket, &rNotifyCallbackReg);
        VERIFY(i4Ret >= 0);

        i4JdecScaleFire(prJdecInst);
        i4Ret = i4JdecScaleWaitFinish(prJdecInst);
        vJdecSetLog((UINT32)(i4Ret) + 0x80000000);
        if (i4Ret != JDEC_DRV_RTN_OK)
        {
            vJdecLog_L("[JDEC] Scale fail \n");
            return JDEC_DRV_RTN_FAIL;
        }
        if ((u4Temp == (prJdecInst->rBlkInfo.u4TotalBlk)) && (prJdecInst->rBlkInfo.u4LastBlkSize % 256  != 0) )
        {
            void *pvTemp = NULL;

            pvTemp = x_kmem_kernel_address(hTemp, prJdecInst->rBlkInfo.u4BlkSize * u4Temp);
            x_memcpy(pvTemp,(void *)(rDstBufInfo.u4YBufAddr),prJdecInst->rBlkInfo.u4LastBlkSize);
            #if !JDEC_USE_MAP_UNMAP
            BSP_FlushDCacheRange((UINT32)pvTemp, prJdecInst->rBlkInfo.u4LastBlkSize);
            #endif
            if (u4TempSA != 0)
            {
                x_mem_aligned_free((VOID *)u4TempSA);
                //UTIL_Printf("[JDEC] Free scale memory 0x%x \n",u4TempSA);
                u4TempSA = 0;
            }
        }

        u4Temp ++;
        
        if (u4Temp >= (prJdecInst->rBlkInfo.u4TotalBlk + 1))
            break;
    }
    return JDEC_DRV_RTN_OK;
}

#endif

