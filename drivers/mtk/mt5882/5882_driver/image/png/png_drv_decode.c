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
#include "png_drv.h"
#include "x_img_dec.h"
#include "png_drv_decode.h"
#include "png_hal_dec.h"
#include "png_debug.h"

#include "u_rm_dev_types.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "img_lib_if.h"

/*-----------------------------------------------------------------------------
                    function extersion
 ----------------------------------------------------------------------------*/
//extern void vPngAbort(PNG_INST_T *ptPngInst, IMG_NFY_STATE_T eState, BOOL fgNfy);
//extern PNG_INST_T _artPngInst[PNG_INST_NUM];

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
#define PNG_FIFO_PERCENT    4
#if 0//def PNG_DRV_FLOW_TEST
#define PNG_HAL_WAIT_TIME   30000000
#else
#define PNG_HAL_WAIT_TIME   10000
#endif
//extern PNG_FIFO_INFO_T _artPngFifo[PNG_INST_NUM];
extern UINT32 u4PngGetSTC(void);

void vPngInitDecVal(PNG_INST_T *ptPngInst)
{
    ptPngInst->u4Progress = 0;
    ptPngInst->i4PngTrnsTimerStatus = 0;
    ptPngInst->i4PngErrorType = 0;
}


INT32 i4PngUpdateCurrentFileOfst(PNG_INST_T *ptPngInst, UINT32 u4SkipMoreData)
{
    UINT32 u4ValTmp;
    if (ptPngInst->tFifoInfo.u4RdPoint >= ptPngInst->tFifoInfo.u4PreRdPoint)
    {
        u4ValTmp = ptPngInst->tFifoInfo.u4RdPoint - ptPngInst->tFifoInfo.u4PreRdPoint;
    }
    else
    {
        u4ValTmp = ptPngInst->tFifoInfo.u4FifoEnd - ptPngInst->tFifoInfo.u4PreRdPoint;
        u4ValTmp += ptPngInst->tFifoInfo.u4RdPoint - ptPngInst->tFifoInfo.u4FifoStart;
    }
    ptPngInst->tFifoInfo.u4FileOfst += u4ValTmp;
    ptPngInst->tFifoInfo.u4PreRdPoint = ptPngInst->tFifoInfo.u4RdPoint;
    ptPngInst->tFifoInfo.u4FileOfst += u4SkipMoreData;

    if (ptPngInst->tFifoInfo.u4FileOfst > ptPngInst->u4ImgSize)
    {
        return PNG_DRV_RTN_FAIL;
    }
    else
    {
        return PNG_DRV_RTN_OK;
    }
}

INT32 i4PngUpdateImgdmaRdPoint(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;

    i4Ret = i4PngUpdateCurrentFileOfst(ptPngInst, 0);
    if (i4Ret == PNG_DRV_RTN_FAIL)
    {
        return PNG_DRV_RTN_FAIL;
    }
    return PNG_DRV_RTN_OK;
}

INT32 i4PngUpdateFifoRdPoint(PNG_INST_T *ptPngInst, UINT32 u4Advance)
{
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);

    if ((ptFifoInfo->u4RdPoint + u4Advance) >= ptFifoInfo->u4FifoEnd)
    {
        ptFifoInfo->u4RdPoint = ptFifoInfo->u4FifoStart + ((ptFifoInfo->u4RdPoint + u4Advance) - ptFifoInfo->u4FifoEnd);
    }
    else
    {
        ptFifoInfo->u4RdPoint += u4Advance;
    }
    ptFifoInfo->u4ValidData -= u4Advance;
    return PNG_DRV_RTN_OK;
}

INT32 i4PngChkFifoStatus(PNG_INST_T *ptPngInst)
{
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);
    if ((ptFifoInfo->u4ValidData >= PNG_FIFO_THRESHOLD) || (ptFifoInfo->fgEoRange))
    {
        return PNG_DRV_RTN_OK;
    }
    else
    {
        IMG_BsBufManager(ptPngInst->u4ImgId);
        return PNG_DRV_RTN_OK;
    }
}

INT32 i4PngProcPsrHdr(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    PNG_HDR_FIFO_INFO *ptPngHdrFifo = &(ptPngInst->tPngHdr.tPngHdrFifo);
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);

    for (;;)
    {
        ptPngHdrFifo->u4FifoStart = ptFifoInfo->u4FifoStart;
        ptPngHdrFifo->u4FifoEnd = ptFifoInfo->u4FifoEnd;
        ptPngHdrFifo->u4ReadPoint = ptFifoInfo->u4RdPoint;
        ptPngHdrFifo->u4WritePoint = ptFifoInfo->u4WrPoint;
        ptPngHdrFifo->u4ValidData = ptFifoInfo->u4ValidData;
        i4Ret = i4PngHdrPsr(&(ptPngInst->tPngHdr));
        ptFifoInfo->u4RdPoint = ptPngHdrFifo->u4ReadPoint;
        ptFifoInfo->u4ValidData = ptPngHdrFifo->u4ValidData;
        switch (i4Ret)
        {
        case PNG_HDR_DONE:
            vPngLog_L("IDAT done\n");
            if (i4PngUpdateCurrentFileOfst(ptPngInst,0) == PNG_DRV_RTN_FAIL)
            {
                return PNG_DRV_RTN_FAIL;
            }
            else
            {
                ptPngInst->u4IDATCount = 1;
                ptPngInst->u4RemainChunkSz = ptPngInst->u4ImgSize - ptFifoInfo->u4FileOfst - 12;
                ptPngInst->tPngHdr.u41stIDATOfst = ptFifoInfo->u4FileOfst;
                ptPngInst->ePngState = PNG_STATE_INIT_HAL;
            }
            return PNG_DRV_RTN_OK;
        case PNG_HDR_NEED_MORE_DATA:
            if (i4PngUpdateCurrentFileOfst(ptPngInst,0) == PNG_DRV_RTN_FAIL)
            {
                return PNG_DRV_RTN_FAIL;
            }
            else
            {
                //fill buffer
                if ((ptPngInst->tFifoInfo.u4FileOfst + ptPngInst->tPngHdr.tPngHdrFifo.u4JumpDataSz) > ptPngInst->u4ImgSize)
                {
                    return PNG_DRV_RTN_FAIL;
                }
                IMG_BsBufManager(ptPngInst->u4ImgId);
            }
            break;
        case PNG_HDR_SKIP_DATA:
            if (i4PngUpdateCurrentFileOfst(ptPngInst, ptPngInst->tPngHdr.tPngHdrFifo.u4JumpDataSz) == PNG_DRV_RTN_FAIL)
            {
                return PNG_DRV_RTN_FAIL;
            }
            //fill buffer or skip to a special addr
            IMG_BsBufStartTrsData(ptPngInst->u4ImgId,ptPngInst->tFifoInfo.u4FileOfst);
            break;
        case PNG_HDR_FAIL:
            return PNG_DRV_RTN_FAIL;
        case PNG_HDR_OK:
            break;
        default:
            vPngLog_H("[Png] ImgId=%x, Parse error,cannot find IDAT,to the end of file. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
            PNGASSERT(0);
            return PNG_DRV_RTN_FAIL;
            //break;
        }
    }

    //return PNG_DRV_RTN_OK;
}

INT32 i4PngChkDstBuf(PNG_INST_T *ptPngInst)
{
    PNG_IHDR *ptPngIhdr = &(ptPngInst->tPngHdr.tPngIhdr);
    INT32       i4Count;
    UINT32      u4ValTmp = 0;
    FBM_POOL_T* prFbmPool;

    switch (ptPngInst->eDstCm)
    {
        //case GFX_COLORMODE_RGB_D565:
    case GFX_COLORMODE_ARGB_D1555:
    case GFX_COLORMODE_ARGB_D4444:
        ptPngInst->u4ClrPixelNum = 2;
        break;
    case GFX_COLORMODE_ARGB_D8888:
        ptPngInst->u4ClrPixelNum = 4;
        break;
    case GFX_COLORMODE_ARGB_CLUT8:
        //vPngLog_L("[Png] GFX_COLORMODE_ARGB_CLUT8 mode!\n");
        ptPngInst->u4ClrPixelNum = 1;
        break;
    default:
        PNGASSERT(0);
        vPngLog_H("[Png] ImgId=%x, not support output color mode. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UNSUPPORT_CLR_MD;
        return PNG_DRV_RTN_FAIL;
    }

    ptPngInst->u4ClrBufPitch = ((ptPngIhdr->u4Width * ptPngInst->u4ClrPixelNum + 15) / 16) * 16;
    ptPngInst->u4PicTotalSz = ptPngInst->u4ClrBufPitch * ptPngIhdr->u4Height;
#ifdef PNG_DECODING_USING_RZ_IOMMU
    if (1)  /*Only IMGRZ MMU configured!*/
#else
    if ((ptPngInst->u4SrcWidth != ptPngInst->u4DstWidth) ||
            (ptPngInst->u4SrcHeight != ptPngInst->u4DstHeight) ||
            (ptPngInst->eDispColorMode == AYCBCR8888_DIRECT32)
       )
#endif
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        //prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PVR);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0);
        //u4ValTmp=  VIRTUAL((UINT32)prFbmPool->u4Addr + PNG_CLR_BUF_OFFSET);  //32 alignment
        u4ValTmp=  ((UINT32)prFbmPool->u4Addr + PNG_CLR_BUF_OFFSET);  //32 alignment

        if (u4ValTmp == 0)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_NO_AVAILABLE_CLR_BUF;
            return PNG_DRV_RTN_FAIL;
        }
        else
        {
            ptPngInst->u4ClrBufAddr = u4ValTmp;
            for (i4Count = 0; i4Count < PNG_CLR_BUF_NUM; i4Count++)
            {
                ptPngInst->aru4ClrBufAddr[i4Count] = u4ValTmp + PNG_CLR_BUF_SIZE * i4Count;
            }
        }
        ptPngInst->fgUseClrBuf = TRUE;
        ptPngInst->u4SliceCount = 0;
        #ifdef PNG_DECODING_USING_RZ_IOMMU
        if (1)  /*Only IMGRZ MMU configured!*/
        #else
        if ((PNG_CLR_BUF_SIZE * PNG_CLR_BUF_NUM) < ptPngInst->u4PicTotalSz)
        #endif
        {
            ptPngInst->u4SliceHeight = PNG_CLR_BUF_SIZE / ptPngInst->u4ClrBufPitch;
            ptPngInst->u4TotalSlice = (ptPngIhdr->u4Height + ptPngInst->u4SliceHeight - 1) / ptPngInst->u4SliceHeight;
            ptPngInst->u4ClrBufSz = ptPngInst->u4SliceHeight * ptPngInst->u4ClrBufPitch;
            ptPngInst->ePngDecMd = PNG_DRV_DECMD_CLRBUF;
            vPngLog_L("PNG_DRV_DECMD_CLRBUF mode\n");
            vPngLog_L("ptPngInst->u4SliceHeight = %d, ptPngInst->u4TotalSlice = %d\n",
                      ptPngInst->u4SliceHeight,ptPngInst->u4TotalSlice);
            vPngLog_L("ptPngInst->u4ClrBufAddr = 0x%08x,ptPngInst->u4ClrBufSz = 0x%08x\n ",ptPngInst->u4ClrBufAddr,ptPngInst->u4ClrBufSz);
        }
        else
        {
            ptPngInst->u4SliceHeight = ptPngIhdr->u4Height;
            ptPngInst->u4TotalSlice = 1;
            ptPngInst->u4ClrBufSz = ptPngInst->u4PicTotalSz;
            ptPngInst->ePngDecMd = PNG_DRV_DECMD_PIC;
            vPngLog_L("PNG_DRV_DECMD_PIC mode\n");
        }
    }
    else
    {
        ptPngInst->fgUseClrBuf = FALSE;
        ptPngInst->ePngDecMd = PNG_DRV_DECMD_PIC;
    }
    vPngLog_L("fgUseClrBuf = %d\n",ptPngInst->fgUseClrBuf );
    //added for 8 bit png decoder.
    if (ptPngInst->eDstCm == GFX_COLORMODE_ARGB_CLUT8)
    {
        //vPngLog_L("[PNG_debug]8 bits mode GFX_COLORMODE_ARGB_CLUT8 Mode\n");
        if (!ptPngInst->tPngHdr.fgPngPalette || ptPngInst->fgUseClrBuf)
        {
            vPngLog_L("[PNG_debug]Cannot decoder without plantt for 8 bits\n");
            vPngLog_L("[PNG_debug]Plantt exist = 0x%x\n", ptPngInst->tPngHdr.fgPngPalette);
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UNSUPPORT_CLR_MD;
            return PNG_DRV_RTN_FAIL;
        }
    }
    return PNG_DRV_RTN_OK;
}

INT32 i4PngInitPNGHal(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    PNG_HAL_PIC_INFO_T rPngHalPicInfo;

    i4PngHalSetAutoSkipDisable(ptPngInst->u4HwInstId);

    i4Ret = i4PngHalEnable(ptPngInst->u4HwInstId);
    if (i4Ret != PNG_HAL_RTN_OK)
    {
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_ENABLED_ERR;
        return PNG_DRV_RTN_FAIL;
    }
    //init HAL
    i4PngHalReset(ptPngInst->u4HwInstId);
    rPngHalPicInfo.ptIhdr = &(ptPngInst->tPngHdr.tPngIhdr);
    rPngHalPicInfo.eOutClrMd = ptPngInst->eDstCm;
    if (ptPngInst->fgUseClrBuf == TRUE)
    {
        rPngHalPicInfo.u4SliceHeight = ptPngInst->u4SliceHeight;
        rPngHalPicInfo.u4DstBufPitch = ptPngInst->u4ClrBufPitch;
    }
    else
    {
        rPngHalPicInfo.u4SliceHeight = ptPngInst->tPngHdr.tPngIhdr.u4Height;
        rPngHalPicInfo.u4DstBufPitch = ptPngInst->u4DstPitch;
    }
    if ((ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF) && (ptPngInst->tPngHdr.tPngIhdr.fgInterlace))
    {
        rPngHalPicInfo.u4CropX = 0;
        rPngHalPicInfo.u4CropY = ptPngInst->u4SliceCount * ptPngInst->u4SliceHeight;
        rPngHalPicInfo.u4CropWidth = ptPngInst->tPngHdr.tPngIhdr.u4Width;
        rPngHalPicInfo.u4CropHeight = ptPngInst->u4SliceHeight;
    }
    else
    {
        rPngHalPicInfo.u4CropX = 0;
        rPngHalPicInfo.u4CropY = 0;
        rPngHalPicInfo.u4CropWidth = ptPngInst->tPngHdr.tPngIhdr.u4Width;
        rPngHalPicInfo.u4CropHeight = ptPngInst->tPngHdr.tPngIhdr.u4Height;
    }
    rPngHalPicInfo.u4AlphaUnMatchedVal = 0xFF;
    rPngHalPicInfo.u4FifoStart = ptPngInst->tFifoInfo.u4FifoStart;
    rPngHalPicInfo.u4FifoEnd = ptPngInst->tFifoInfo.u4FifoEnd;
    rPngHalPicInfo.u4FifoEnd += 16;
    i4Ret = i4PngHalSetPicInfo(ptPngInst->u4HwInstId, &rPngHalPicInfo);
    if (i4Ret != PNG_HAL_RTN_OK)
    {
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_SET_PICINFO_ERR;
        return PNG_DRV_RTN_FAIL;
    }
    vPngSWReset(0);
    if (ptPngInst->tPngHdr.fgPngPalette)
    {
        i4Ret = i4PngHalSetPLTE(ptPngInst->u4HwInstId, &(ptPngInst->tPngHdr.tPngPlte));
        if (i4Ret == PNG_HAL_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_SET_PLT_ERR;
            return PNG_DRV_RTN_FAIL;
        }
    }
    if (ptPngInst->tPngHdr.fgPngtRNS)
    {
        i4Ret = i4PngHalSettRNS(ptPngInst->u4HwInstId, &(ptPngInst->tPngHdr.tPngTrns), ptPngInst->tPngHdr.fgPngPalette);
        if (i4Ret == PNG_HAL_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_SET_TRNS_ERR;
            return PNG_DRV_RTN_FAIL;
        }
    }

    if ((ptPngInst->u4SrcWidth==0)||(ptPngInst->u4SrcHeight==0))
    {
        ptPngInst->u4SrcWidth = rPngHalPicInfo.ptIhdr->u4Width;
        ptPngInst->u4SrcHeight =  rPngHalPicInfo.ptIhdr->u4Height;
    }

    /*
    if (ptPngInst->tPngHdr.fgPngbKGD)
    {
        i4Ret = i4PngHalSetbKGD(ptPngInst->u4HwInstId, &(ptPngInst->tPngHdr.tPngBkgd), &(ptPngInst->tPngHdr.tPngPlte), &(ptPngInst->tPngHdr.tPngTrns), ptPngInst->tPngHdr.fgPngPalette, ptPngInst->tPngHdr.fgPngtRNS);
        if (i4Ret == PNG_HAL_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_SET_BKGD_ERR;
            return PNG_DRV_RTN_FAIL;
        }
    }
    */
    i4PngHalSetAutoSkipEnable(ptPngInst->u4HwInstId);
    return PNG_DRV_RTN_OK;
}

INT32 i4PngInitScale(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    if (ptPngInst->fgUseClrBuf == TRUE)
    {
        i4Ret = 0;//i4PngScaleInit(ptPngInst);IMGRZ_TODO
        if (i4Ret == PNG_HAL_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HW_SCALE_ERROR;
            return PNG_DRV_RTN_FAIL;
        }
    }
    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateDEC(PNG_INST_T *ptPngInst)
{
    PNG_HAL_IDAT_INFO_T tPngIdatInfo;
    PNG_HDR_INFO_T *ptPngHdr = &(ptPngInst->tPngHdr);
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);
    INT32 i4Ret;
    UINT32 u4ValTmp;

    tPngIdatInfo.u4ChunkType = ptPngHdr->tPngChunkInfo.u4ChunkType;
    tPngIdatInfo.u4SrtStrmAddr = ptFifoInfo->u4RdPoint;
    //if (ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF)
    if ((ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF) && (!ptPngHdr->tPngIhdr.fgInterlace))
    {
        if ((ptPngInst->u4TotalSlice - 1) == (ptPngInst->u4SliceCount))
        {
            tPngIdatInfo.fgLastSlice = TRUE;    //CJYang, need chk with clr buffer
        }
        else
        {
            tPngIdatInfo.fgLastSlice = FALSE;    //CJYang, need chk with clr buffer
        }
    }
    else
    {
        tPngIdatInfo.fgLastSlice = TRUE;
    }
    if (ptPngInst->fgUseClrBuf == TRUE)
    {
        tPngIdatInfo.u4DstBufStart = (UINT32)(ptPngInst->aru4ClrBufAddr[ptPngInst->u4SliceCount % PNG_CLR_BUF_NUM]);
        tPngIdatInfo.u4DstBufEnd = tPngIdatInfo.u4DstBufStart + ptPngInst->u4ClrBufSz;
    }
    else
    {
        u4ValTmp = (UINT32)(ptPngInst->pvDst);
        u4ValTmp += ptPngInst->u4DstY * ptPngInst->u4DstPitch + ptPngInst->u4DstX * ptPngInst->u4ClrPixelNum;
        tPngIdatInfo.u4DstBufStart = u4ValTmp;
        u4ValTmp = (ptPngInst->tPngHdr.tPngIhdr.u4Height + ptPngInst->u4DstY) * ptPngInst->u4DstPitch;
        tPngIdatInfo.u4DstBufEnd = tPngIdatInfo.u4DstBufStart + u4ValTmp;
    }
    if (ptPngInst->u4IDATCount == 1)
    {
        tPngIdatInfo.fg1stIDATChunk = TRUE;
    }
    else
    {
        tPngIdatInfo.fg1stIDATChunk = FALSE;
    }
    //need to +/- 4, because after update it will +4
    if (ptFifoInfo->u4ValidData > (ptPngInst->u4RemainChunkSz + 4))
    {
        //tPngIdatInfo.fgLastIDATGrp = TRUE;
        tPngIdatInfo.fgLastIDATGrp = FALSE;
        ptPngInst->u4CurrSrcStrmSz = ptPngInst->u4RemainChunkSz;
        i4PngHalSetAutoSkipLastIdat(ptPngInst->u4HwInstId, TRUE);
    }
    else
    {
        u4ValTmp = (ptFifoInfo->u4FifoEnd - ptFifoInfo->u4FifoStart) / PNG_FIFO_PERCENT;
        if (ptFifoInfo->u4ValidData > u4ValTmp)
        {
            ptPngInst->u4CurrSrcStrmSz = u4ValTmp - 4;
        }
        else
        {
            ptPngInst->u4CurrSrcStrmSz = ptFifoInfo->u4ValidData - 4;
        }
        tPngIdatInfo.fgLastIDATGrp = FALSE;
        i4PngHalSetAutoSkipLastIdat(ptPngInst->u4HwInstId, FALSE);
    }
    //--
    tPngIdatInfo.u4SrtStrmLen = ptPngInst->u4CurrSrcStrmSz;
    ptPngInst->u4RemainChunkSz -= ptPngInst->u4CurrSrcStrmSz;
    vPngLog_L("tPngIdatInfo.u4SrtStrmLen = 0x%x\n",tPngIdatInfo.u4SrtStrmLen);
    i4Ret = i4PngHalProcIDAT(ptPngInst->u4HwInstId, &tPngIdatInfo);
    VERIFY(i4Ret == PNG_HAL_RTN_OK);
    ptPngInst->ePngDecState = PNG_DEC_STATE_WAIT;

    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateWAIT(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    i4Ret = i4PngHalWaitDec(ptPngInst->u4HwInstId, PNG_HAL_WAIT_TIME);
    switch (i4Ret)
    {
    case PNG_HAL_RTN_DEC:
        //need to check CRC?
        if (ptPngInst->u4RemainChunkSz != 0)
        {
            PNGASSERT(0);
            vPngLog_M("[Png] ImgId=%x, Bitstream error. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_BITSTREAM_ERR;
            return PNG_DRV_RTN_FAIL;
        }

        if (i4PngUpdateFifoRdPoint(ptPngInst, ptPngInst->u4CurrSrcStrmSz) == PNG_DRV_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UPDATE_RDPOINT_ERR;
            return PNG_DRV_RTN_FAIL;
        }
        if (i4PngUpdateImgdmaRdPoint(ptPngInst) == PNG_DRV_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UPDATE_IMGDMA_ERR;
            return PNG_DRV_RTN_FAIL;
        }

        if (ptPngInst->fgUseClrBuf == TRUE)
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_DO_SCALE;
        }
        else
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_CHK_IDAT;
        }

        break;
    case PNG_HAL_RTN_SRC:
        if (ptPngInst->u4RemainChunkSz != 0)
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_SRC_RESUME;
        }
        else
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_CHK_IDAT;
        }
        if (i4PngUpdateFifoRdPoint(ptPngInst, ptPngInst->u4CurrSrcStrmSz) == PNG_DRV_RTN_FAIL) //skip CRC
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UPDATE_RDPOINT_ERR;
            return PNG_DRV_RTN_OK;
        }
        if (i4PngUpdateImgdmaRdPoint(ptPngInst) == PNG_DRV_RTN_FAIL)
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_UPDATE_IMGDMA_ERR;
            return PNG_DRV_RTN_OK;
        }
        if ((0 == ptPngInst->u4CurrSrcStrmSz) && (ptPngInst->tFifoInfo.fgEoRange))
        {
            vPngLog_H( "[Png] TO THE END OF FILE, Current STC %d\n", u4PngGetSTC());
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_FILE_END_ERROR;
            return PNG_DRV_RTN_FAIL;
        }
        break;
    case PNG_HAL_RTN_DST:
        //call image resz do scale
        if (ptPngInst->fgUseClrBuf == TRUE)
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_DO_SCALE;
        }
        else
        {
            ptPngInst->ePngDecState = PNG_DEC_STATE_DST_RESUME;
        }
        break;
    case PNG_HAL_RTN_STOPPED:
        vPngLog_L( "[Png] PNG_HAL_RTN_STOPPED Current STC %d\n", u4PngGetSTC());
        //vPngAbort(ptPngInst, (IMG_NFY_STATE_T)0, FALSE);
        ptPngInst->fgAbort = TRUE;
        ptPngInst->u4NfyStatus = IMG_NFY_STOP_DONE;
        break;
    case PNG_HAL_ALDER_ERR:
        //add for ALDER_ERR re-decode.
        if (!(ptPngInst->fgRedecode))
        {
            vPngLog_H("[Png] ImgId=%x, ALDER32 error --- do redecode. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
            ptPngInst->fgRedecode    = TRUE;
            ptPngInst->ePngDecState  = PNG_DEC_STATE_NEXT_DEC;
            ptPngInst->u4SliceCount  = 0;
        }
        else
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_DECODE_ERR;
        }
        break;
    default:
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_DECODE_ERR;
        break;
    }
    return PNG_DRV_RTN_OK;
}


INT32 i4PngDecStateChkIDAT(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    PNG_HDR_FIFO_INFO *ptPngHdrFifo = &(ptPngInst->tPngHdr.tPngHdrFifo);
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);
    //UINT32 u4RingBufSize,u4FreeSize;
    //if find next IDAT chunk
    //u4RingBufSize = ptFifoInfo->u4FifoEnd - ptFifoInfo->u4FifoStart;
    ptPngHdrFifo->u4FifoStart = ptFifoInfo->u4FifoStart;
    ptPngHdrFifo->u4FifoEnd = ptFifoInfo->u4FifoEnd;
    ptPngHdrFifo->u4ReadPoint = ptFifoInfo->u4RdPoint;
    ptPngHdrFifo->u4WritePoint = ptFifoInfo->u4WrPoint;
    ptPngHdrFifo->u4ValidData = ptFifoInfo->u4ValidData;
    i4Ret = i4PngHdrIDAT(&(ptPngInst->tPngHdr));
    ptFifoInfo->u4RdPoint = ptPngHdrFifo->u4ReadPoint;
    ptFifoInfo->u4ValidData = ptPngHdrFifo->u4ValidData;

    switch (i4Ret)
    {
    case PNG_HDR_DONE:
        ptPngInst->u4IDATCount += 1;
        ptPngInst->u4RemainChunkSz = ptPngInst->tPngHdr.tPngChunkInfo.u4ChunkSize;
        if ( ((ptPngInst->tPngHdr.tPngIhdr.fgInterlace) && (ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF) && (ptPngInst->u4TotalSlice > ptPngInst->u4SliceCount)) ||
                (ptPngInst->fgRedecode))
        {
            ptPngInst->u4RemainChunkSz = ptPngInst->u4ImgSize - ptFifoInfo->u4FileOfst - 4;
            ptPngInst->ePngDecState = PNG_DEC_STATE_DEC;
        }
        else
        {
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_8530_NOT_IEND;
            vPngLog_H( "[Png] IMG_PARSE_ERROR ImgId=%x, ErrCode=%d, State=%d, DecState=%d\n", ptPngInst->u4ImgId, ptPngInst->i4PngErrorType, ptPngInst->ePngState, ptPngInst->ePngDecState);
            break;
        }
        if (i4PngUpdateCurrentFileOfst(ptPngInst, 0) == PNG_DRV_RTN_OK)
        {
            return PNG_DRV_RTN_OK;
        }
        break;
    case PNG_HDR_NEED_MORE_DATA:
        if (i4PngUpdateCurrentFileOfst(ptPngInst, 0) == PNG_DRV_RTN_OK)
        {
            IMG_BsBufManager(ptPngInst->u4ImgId);
            if (!(((ptPngInst->tFifoInfo.u4FileOfst + ptPngInst->tPngHdr.tPngHdrFifo.u4JumpDataSz) > ptPngInst->u4ImgSize) ||
                    ((0 == ptPngInst->tPngHdr.tPngHdrFifo.u4JumpDataSz) && (ptPngInst->tFifoInfo.u4FileOfst + 8 > ptPngInst->u4ImgSize))))
            {
                return PNG_DRV_RTN_OK;
            }
        }
        break;
    case PNG_HDR_SKIP_DATA:
        if (i4PngUpdateCurrentFileOfst(ptPngInst, ptPngInst->tPngHdr.tPngHdrFifo.u4JumpDataSz) == PNG_DRV_RTN_OK)
        {
            IMG_BsBufStartTrsData(ptPngInst->u4ImgId,ptPngInst->tFifoInfo.u4FileOfst);
            return PNG_DRV_RTN_OK;
        }
        break;
    case PNG_HDR_IEND:
        break;
    default:
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_8530_NOT_IEND;
        vPngLog_H( "[Png] IMG_PARSE_ERROR ImgId=%x, ErrCode=%d, State=%d, DecState=%d\n", ptPngInst->u4ImgId, ptPngInst->i4PngErrorType, ptPngInst->ePngState, ptPngInst->ePngDecState);
        return PNG_DRV_RTN_FAIL;
    }

    if (ptPngInst->fgUseClrBuf)
    {
        ptPngInst->ePngDecState = PNG_DEC_STATE_DO_SCALE;
    }
    else
    {
        //vPngAbort(ptPngInst, IMG_NFY_FINISHED, TRUE);        
        #ifndef PHOTO_EMP_DDI_CONFIG
        #if defined (CC_MT5890) || defined(CC_MT5882)
        HalFlushInvalidateDCacheMultipleLine(ptFifoInfo->u4FifoStart,PNG_RZ_TEMP_BUF_OFFSET);//5890, not use HalFlushInvalidateDCache() for performance
        #else
        HalFlushInvalidateDCache();
        #endif
        #endif
        ptPngInst->fgAbort = TRUE;
        ptPngInst->u4NfyStatus = IMG_NFY_FINISHED;
    }

    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateSrcResume(PNG_INST_T *ptPngInst)
{
    UINT32 u4ValTmp;
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);

    //need to +/- 4, because after update it will +4
    if (ptFifoInfo->u4ValidData > (ptPngInst->u4RemainChunkSz + 4))
    {
        i4PngHalSetAutoSkipLastIdat(ptPngInst->u4HwInstId, TRUE);
        VERIFY(i4PngHalProcNextSrc(ptPngInst->u4HwInstId, ptFifoInfo->u4RdPoint, ptPngInst->u4RemainChunkSz, FALSE) == PNG_HAL_RTN_OK);
        ptPngInst->u4CurrSrcStrmSz = ptPngInst->u4RemainChunkSz;
        ptPngInst->u4RemainChunkSz = 0;
    }
    else
    {
        u4ValTmp = (ptFifoInfo->u4FifoEnd - ptFifoInfo->u4FifoStart) / PNG_FIFO_PERCENT;
        if (ptFifoInfo->u4ValidData > u4ValTmp)
        {
            ptPngInst->u4CurrSrcStrmSz = u4ValTmp - 4;
        }
        else
        {
            ptPngInst->u4CurrSrcStrmSz = ptFifoInfo->u4ValidData - 4;
        }
        ptPngInst->u4RemainChunkSz -= ptPngInst->u4CurrSrcStrmSz;
        VERIFY(i4PngHalProcNextSrc(ptPngInst->u4HwInstId, ptFifoInfo->u4RdPoint, ptPngInst->u4CurrSrcStrmSz, FALSE) == PNG_HAL_RTN_OK);
    }
    //--
    ptPngInst->ePngDecState = PNG_DEC_STATE_WAIT;
    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateDstResume(PNG_INST_T *ptPngInst)
{
    UINT32 u4ValTmp;

    if ((ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF) && (!ptPngInst->tPngHdr.tPngIhdr.fgInterlace))
    {
        u4ValTmp = ptPngInst->aru4ClrBufAddr[ptPngInst->u4SliceCount % PNG_CLR_BUF_NUM];
        if ((ptPngInst->u4TotalSlice - 1) == (ptPngInst->u4SliceCount))
        {
            VERIFY(i4PngHalProcNextDst(ptPngInst->u4HwInstId, u4ValTmp, (u4ValTmp + ptPngInst->u4ClrBufSz), TRUE) == PNG_HAL_RTN_OK);
        }
        else
        {
            VERIFY(i4PngHalProcNextDst(ptPngInst->u4HwInstId, u4ValTmp, (u4ValTmp + ptPngInst->u4ClrBufSz), FALSE) == PNG_HAL_RTN_OK);
        }
    }
    else
    {
        PNGASSERT(0);
        vPngLog_H("[Png] ImgId=%x, Decoding mode error. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
    }
    ptPngInst->ePngDecState = PNG_DEC_STATE_WAIT;
    return PNG_DRV_RTN_OK;
}

INT32 i4PngAYCbCrOsdToYCbCrVideo(PNG_INST_T *ptPngInst)
{
    UINT8 *s, *t, *src, *des;
    UINT32 x, y, src_pitch, des_pitch, cbcr_offset;

    src = ((UINT8 *)VIRTUAL((UINT32)(ptPngInst->pvDst)));
    des = ((UINT8 *)VIRTUAL((UINT32)(ptPngInst->pvDst_OnVideoPlane)));
    src_pitch = ptPngInst->u4DstPitch;
    des_pitch = ptPngInst->u4DstPitch/4;    //for video plane
    cbcr_offset = ptPngInst->u4CbCrOffset;

    //y
    for (y=0; y<ptPngInst->u4DstHeight + ptPngInst->u4DstY*2; y++)
    {
        s = src + y*src_pitch  + 2;  // +2 means Y component
        t = des + y*des_pitch;
        for (x=0; x<ptPngInst->u4DstWidth + ptPngInst->u4DstX*2; x++)
        {
            *t = *s;
            t++;
            s+=4;
        }
    }
    //cbcr
    for (y=0; y<ptPngInst->u4DstHeight/2 + ptPngInst->u4DstY; y++)
    {
        //Copy Cb part
        s = src + y*src_pitch*2  + 1;  // +1 means Cb component, *2 means get one every 2 lines
        t = des + cbcr_offset + y*des_pitch;
        for (x=0; x<ptPngInst->u4DstWidth/2 + ptPngInst->u4DstX; x++)
        {
            *t = *s;
            t = t+2;
            s = s+8;    //get one every 2 Cb-dots in src buffer
        }

        //Copy Cr part
        s = src + y*src_pitch*2  + 0;  // +0 means Cr component, *2 means get one every 2 lines
        t = des + cbcr_offset + y*des_pitch + 1; // +1 means Cr component
        for (x=0; x<ptPngInst->u4DstWidth/2 + ptPngInst->u4DstX; x++)
        {
            *t = *s;
            t = t+2;
            s = s+8;    //get one every 2 Cr-dots in src buffer
        }
    }
    return PNG_DRV_RTN_OK;
}

extern UINT32 VIRTUAL_DFB(UINT32 addr);
INT32 i4PngAYCbCrOsdToYCbCr422Video(PNG_INST_T *ptPngInst)
{
    UINT8 *s, *t, *src, *des,*alpha;
    UINT32 x, y, src_pitch, des_pitch, cbcr_offset;
    UINT32 stary_y,end_y;    
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
    UINT32 u4PngSrcMapAddr = 0,u4PngDisMapDesAddr = 0;
    UINT32 u4PngSrcAddr;
#endif
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
          u4PngSrcAddr = ((UINT32)ptPngInst->pvDst -0x1000) /0x1000 * 0x1000;//map start addr 4k alignment
          u4PngSrcMapAddr = (UINT32)BSP_MapReservedMem((void *)u4PngSrcAddr, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH + ptPngInst->u4DstX * 4 + 3 + 0x2000); //+0x2000: waster some memory to make sure map enough
          if(!u4PngSrcMapAddr )
          {
             LOG(0, "PNG trans video mode src dynamic map fail\n");             
             ASSERT(0);
          }
          vPngLog_L("0x%x video mode src map addr:0x%x, size:0x%x\n", u4PngSrcAddr, u4PngSrcMapAddr, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH + ptPngInst->u4DstX * 4 + 3 + 0x2000);
          vPngLog_L("source phy addr:0x%x\n", (UINT32)ptPngInst->pvDst);
          u4PngDisMapDesAddr = (UINT32)BSP_MapReservedMem(ptPngInst->pvDst_OnVideoPlane, ptPngInst->u4CbCrOffset*2);      
          if(!u4PngDisMapDesAddr)
          {
              LOG(0, "PNG trans video mode des dynamic map fail\n");              
              ASSERT(0);
          }        
          vPngLog_L("video mode des map addr:0x%x, size:0x%x\n", u4PngDisMapDesAddr, ptPngInst->u4CbCrOffset*2);
#endif

    src = ((UINT8 *)VIRTUAL((UINT32)(ptPngInst->pvDst)));
    HalInvalidateDCacheMultipleLine((UINT32)src, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH);
    
    des = ((UINT8 *)VIRTUAL_DFB((UINT32)(ptPngInst->pvDst_OnVideoPlane)));

    src_pitch = ptPngInst->u4DstPitch;
    des_pitch = ptPngInst->u4DstPitch/4;    //for video plane
    cbcr_offset = ptPngInst->u4CbCrOffset;
    stary_y =  ptPngInst->u4ScaleRowY;
    end_y =   ptPngInst->u4ScaleRowY + ptPngInst->u4ScaleRowH;

    vPngLog_L("Src Addr 0x%08x,Dst Addr 0x%08x, Physical des 0x%08x\n",src,des,(ptPngInst->pvDst_OnVideoPlane));
    vPngLog_L("Dst  X 0x%08x,Dst Y 0x%08x ,cbcr_offset %08x\n", ptPngInst->u4DstX, ptPngInst->u4DstY,cbcr_offset);
    vPngLog_L("u4ScaleRowY %08x u4ScaleRowH%08x\n",ptPngInst->u4ScaleRowY ,ptPngInst->u4ScaleRowH);
    vPngLog_L("des_pitch %08x u4DstHeight %08x\n",des_pitch ,ptPngInst->u4DstHeight);

    if(des == NULL)
    {
       return 0;
    }

    //y
    for (y=stary_y; y<end_y; y++)
    {
	    alpha = src + (y -stary_y) *src_pitch  + 3 + ptPngInst->u4DstX*4;
        s = src + (y -stary_y) *src_pitch  + 2 + ptPngInst->u4DstX*4;  // +2 means Y component
        t = des + (y + ptPngInst->u4DstY)*des_pitch + ptPngInst->u4DstX;       
        for (x=0; x<ptPngInst->u4DstWidth; x++)
        {
            *t = ((*s )* (*alpha))/255;
            t++;
            s+=4;
			alpha += 4;
        }
    }    
    //cbcr
    for (y=stary_y; y<end_y; y++)
    {
        //Copy Cb part
        alpha = src + (y -stary_y)*src_pitch  + 3 +  ptPngInst->u4DstX*4;  // +1 means Cb component, 
        s = src + (y -stary_y)*src_pitch  + 1 +  ptPngInst->u4DstX*4;  // +1 means Cb component, 
        t = des + cbcr_offset + (y + ptPngInst->u4DstY)*des_pitch +ptPngInst->u4DstX;        
        for (x=0; x<(ptPngInst->u4DstWidth)/2; x++)
        {
            *t = ((*s) * (*alpha) + (255 - (*alpha)) * 0x80) / 255;
            t = t+2;
            s = s+8;    //get one every 2 Cb-dots in src buffer
            alpha = alpha + 8;
        }

        //Copy Cr part
        alpha = src + (y -stary_y)*src_pitch  + 3 +  ptPngInst->u4DstX*4;
        s = src + (y -stary_y)*src_pitch  + 0 + ptPngInst->u4DstX*4;  // +0 means Cr component, 
        t = des + cbcr_offset + (y + ptPngInst->u4DstY)*des_pitch + 1 +  ptPngInst->u4DstX; // +1 means Cr component
        for (x=0; x<(ptPngInst->u4DstWidth)/2; x++)
        {
            *t = ((*s) * (*alpha) + (255 - (*alpha)) * 0x80) / 255;
            t = t+2;
            s = s+8;    //get one every 2 Cr-dots in src buffer
            alpha = alpha + 8;
        }
    }
    HalFlushInvalidateDCacheMultipleLine((UINT32)des,cbcr_offset*2);
    #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
    if(u4PngSrcMapAddr)
    {
        BSP_UnMapReservedMem((void *)u4PngSrcMapAddr);
        vPngLog_L("video mode src unmap\n");
    }
    if(u4PngDisMapDesAddr)
    {        
            BSP_UnMapReservedMem((void *)u4PngDisMapDesAddr);            
            vPngLog_L("video mode des unmap\n");
    }
    #endif
    return PNG_DRV_RTN_OK;
}

INT32 i4PngAYCbCrOsdToYCbCr420Video(PNG_INST_T *ptPngInst)
{
	UINT8 *s, *t, *src, *des,*alpha;
	UINT32 x, y, src_pitch, des_pitch, cbcr_offset;
	UINT32 stary_y,end_y;	 
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
	UINT32 u4PngSrcMapAddr = 0,u4PngDisMapDesAddr = 0;
	UINT32 u4PngSrcAddr;
#endif
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
		  u4PngSrcAddr = ((UINT32)ptPngInst->pvDst -0x1000) /0x1000 * 0x1000;//map start addr 4k alignment
		  u4PngSrcMapAddr = (UINT32)BSP_MapReservedMem((void *)u4PngSrcAddr, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH + ptPngInst->u4DstX * 4 + 3 + 0x2000); //+0x2000: waster some memory to make sure map enough
		  if(!u4PngSrcMapAddr )
		  {
			 LOG(0, "PNG trans video mode src dynamic map fail\n"); 			
			 ASSERT(0);
		  }
		  vPngLog_L("0x%x video mode src map addr:0x%x, size:0x%x\n", u4PngSrcAddr, u4PngSrcMapAddr, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH + ptPngInst->u4DstX * 4 + 3 + 0x2000);
		  vPngLog_L("source phy addr:0x%x\n", (UINT32)ptPngInst->pvDst);
		  u4PngDisMapDesAddr = (UINT32)BSP_MapReservedMem(ptPngInst->pvDst_OnVideoPlane, ptPngInst->u4CbCrOffset*2);	  
		  if(!u4PngDisMapDesAddr)
		  {
			  LOG(0, "PNG trans video mode des dynamic map fail\n");			  
			  ASSERT(0);
		  } 	   
		  vPngLog_L("video mode des map addr:0x%x, size:0x%x\n", u4PngDisMapDesAddr, ptPngInst->u4CbCrOffset*2);
#endif

	src = ((UINT8 *)VIRTUAL((UINT32)(ptPngInst->pvDst)));
	HalInvalidateDCacheMultipleLine((UINT32)src, ptPngInst->u4DstPitch * ptPngInst->u4ScaleRowH);
	
	des = ((UINT8 *)VIRTUAL_DFB((UINT32)(ptPngInst->pvDst_OnVideoPlane)));

	src_pitch = ptPngInst->u4DstPitch;
	des_pitch = ptPngInst->u4DstPitch/4;	//for video plane
	cbcr_offset = ptPngInst->u4CbCrOffset;
	stary_y =  ptPngInst->u4ScaleRowY;
	end_y =   ptPngInst->u4ScaleRowY + ptPngInst->u4ScaleRowH;

	vPngLog_L("Src Addr 0x%08x,Dst Addr 0x%08x, Physical des 0x%08x\n",src,des,(ptPngInst->pvDst_OnVideoPlane));
	vPngLog_L("Dst	X 0x%08x,Dst Y 0x%08x ,cbcr_offset %08x\n", ptPngInst->u4DstX, ptPngInst->u4DstY,cbcr_offset);
	vPngLog_L("u4ScaleRowY %08x u4ScaleRowH%08x\n",ptPngInst->u4ScaleRowY ,ptPngInst->u4ScaleRowH);
	vPngLog_L("des_pitch %08x u4DstHeight %08x\n",des_pitch ,ptPngInst->u4DstHeight);

	if(des == NULL)
	{
	   return 0;
	}

	//y
	for (y=stary_y; y<end_y; y++)
	{
		alpha = src + (y -stary_y) *src_pitch  + 3 + ptPngInst->u4DstX*4;
		s = src + (y -stary_y) *src_pitch  + 2 + ptPngInst->u4DstX*4;  // +2 means Y component
		t = des + (y + ptPngInst->u4DstY)*des_pitch + ptPngInst->u4DstX;	   
		for (x=0; x<ptPngInst->u4DstWidth; x++)
		{
			*t = ((*s )* (*alpha))/255;
			t++;
			s+=4;
			alpha += 4;
		}
	}	 
	//cbcr
	for (y=stary_y; y<end_y; y++)
	{
		if(y%2)
		{
			//vPngLog_H("skip y=%d\n", y);
			continue;
		}
		//Copy Cb part
		alpha = src + (y -stary_y)*src_pitch  + 3 +  ptPngInst->u4DstX*4;  // +1 means Cb component, 
		s = src + (y -stary_y)*src_pitch  + 1 +  ptPngInst->u4DstX*4;  // +1 means Cb component, 
		t = des + cbcr_offset + (y + ptPngInst->u4DstY)*des_pitch/2 +ptPngInst->u4DstX;		 
		for (x=0; x<(ptPngInst->u4DstWidth)/2; x++)
		{
			*t = ((*s) * (*alpha) + (255 - (*alpha)) * 0x80) / 255;
			t = t+2;
			s = s+8;	//get one every 2 Cb-dots in src buffer
			alpha = alpha + 8;
		}

		//Copy Cr part
		alpha = src + (y -stary_y)*src_pitch  + 3 +  ptPngInst->u4DstX*4;
		s = src + (y -stary_y)*src_pitch  + 0 + ptPngInst->u4DstX*4;  // +0 means Cr component, 
		t = des + cbcr_offset + (y + ptPngInst->u4DstY)*des_pitch/2 + 1 +  ptPngInst->u4DstX; // +1 means Cr component
		for (x=0; x<(ptPngInst->u4DstWidth)/2; x++)
		{
			*t = ((*s) * (*alpha) + (255 - (*alpha)) * 0x80) / 255;
			t = t+2;
			s = s+8;	//get one every 2 Cr-dots in src buffer
			alpha = alpha + 8;
		}
	}
	HalFlushInvalidateDCacheMultipleLine((UINT32)des,cbcr_offset*2);
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
	if(u4PngSrcMapAddr)
	{
		BSP_UnMapReservedMem((void *)u4PngSrcMapAddr);
		vPngLog_L("video mode src unmap\n");
	}
	if(u4PngDisMapDesAddr)
	{		 
			BSP_UnMapReservedMem((void *)u4PngDisMapDesAddr);			 
			vPngLog_L("video mode des unmap\n");
	}
#endif
	return PNG_DRV_RTN_OK;
}



#if defined( CC_MT5890) 
#define PNG_RESIZER_ID (IMGRZ_NUM - 1)
#else
#define PNG_RESIZER_ID E_FIRST_RESIZER
#endif
INT32 i4PngDecStateDoScale(PNG_INST_T *ptPngInst)
{
    //INT32 i4Ret;
    RZ_OSD_SCL_PARAM_SET_T rSclParam;
	FBM_POOL_T *prFbmPool;
	
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    x_memset(&rSclParam,0,sizeof(RZ_OSD_SCL_PARAM_SET_T));
    rSclParam.u4SrcBase 	= (UINT32) ptPngInst->aru4ClrBufAddr[0];
    rSclParam.u4IsSrcDirCM	= (UINT32) 1;
    rSclParam.u4SrcBufLen	= (ptPngInst->u4ClrBufPitch);
    rSclParam.u4SrcHOffset	= (UINT32) ptPngInst->u4SrcX;
    rSclParam.u4SrcVOffset	= (UINT32) ptPngInst->u4SrcY;
    rSclParam.u4SrcW		= (UINT32) ptPngInst->u4SrcWidth;
    rSclParam.u4SrcH		= (UINT32) ptPngInst->u4SrcHeight;

    rSclParam.u4TgBase		= (UINT32) ptPngInst->pvDst;
    rSclParam.u4TgHOffset	= (UINT32) ptPngInst->u4DstX;
    rSclParam.u4TgVOffset	= (UINT32) (ptPngInst->u4DstY);
    rSclParam.u4TgW 		= (UINT32) ptPngInst->u4DstWidth;
    rSclParam.u4TgH 		= (UINT32) ptPngInst->u4DstHeight;
    rSclParam.u4PngTempAddr = (UINT32) (prFbmPool->u4Addr + PNG_RZ_TEMP_BUF_OFFSET);

    switch (ptPngInst->eDstCm)
    {
    case GFX_COLORMODE_ARGB_D8888:
        rSclParam.u4SrcCM = (UINT32)E_RZ_OSD_DIR_CM_ARGB8888;
        break;
    case GFX_COLORMODE_ARGB_D4444:
        rSclParam.u4SrcCM = (UINT32)E_RZ_OSD_DIR_CM_ARGB4444;
        break;
    case  GFX_COLORMODE_ARGB_D1555:
        rSclParam.u4SrcCM =  (UINT32)E_RZ_OSD_DIR_CM_ARGB1555;
        break;
    case GFX_COLORMODE_ARGB_CLUT8:
    default:
        return PNG_DRV_RTN_FAIL;
    }
    switch (ptPngInst->eDispColorMode)
    {
    case AYCBCR8888_DIRECT32:
        rSclParam.u4TgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
        break;
    case ARGB8888_DIRECT32:
        rSclParam.u4TgCM = E_RZ_OSD_DIR_CM_ARGB8888;
        break;
    case ARGB4444_DIRECT16:
        rSclParam.u4TgCM = E_RZ_OSD_DIR_CM_ARGB4444;
        break;
    default:
        rSclParam.u4TgCM = E_RZ_OSD_DIR_CM_ARGB8888;
        break;
    }
    if (ptPngInst->eDispFinalColorMode == Y_CBCR422_SEP16 
		|| ptPngInst->eDispFinalColorMode == Y_CBCR420_SEP16)
    {
        rSclParam.u4IsPngSameaddr = 1;
    }
    rSclParam.u4TgBufLen	= (UINT32) (ptPngInst->u4DstPitch);
	if((rSclParam.u4TgW & (~0x3fff)) || (rSclParam.u4TgH & (~0x3fff))
		|| ((rSclParam.u4TgBufLen >> 4) & (~0xfff)))
	{
		LOG(0, "## Error!! [w/h=%d/%d, BufLen=%d] is too large, can not support!!", 
			rSclParam.u4TgW, rSclParam.u4TgH, rSclParam.u4TgBufLen);
	}	
    if ( (ptPngInst->eDispColorMode == AYCBCR8888_DIRECT32)
            && (ptPngInst->eDstCm== GFX_COLORMODE_ARGB_D8888))
    {
        rSclParam.u4Csc_enable      = (UINT32) TRUE;
        rSclParam.csc_src_format      = (E_RZ_CSC_FORMAT_T) E_RZ_OSD_CSC_FORMAT_ARGB8888;
        rSclParam.csc_des_format      = (E_RZ_CSC_FORMAT_T) E_RZ_OSD_CSC_FORMAT_AYUV8888;
    }

    if (ptPngInst->ePngDecMd == PNG_DRV_DECMD_PIC)
    {
        IMGRZ_Lock_Ex(PNG_RESIZER_ID);
        IMGRZ_ReInit_Ex(PNG_RESIZER_ID);
        IMGRZ_SetScaleOpt_Ex(PNG_RESIZER_ID, E_RZ_INOUT_TYPE_OSDMD);
        IMGRZ_Scale_Ex(PNG_RESIZER_ID, &rSclParam);
        IMGRZ_Flush_Ex(PNG_RESIZER_ID);
        IMGRZ_Wait_Ex(PNG_RESIZER_ID);
        IMGRZ_Unlock_Ex(PNG_RESIZER_ID);
    }
    else
    {
        if (ptPngInst->u4SliceCount == 0)
        {
            rSclParam.u4IsFstBl = 1;
        }
        else
        {
            rSclParam.u4IsFstBl = 0;
        }

        if (ptPngInst->u4SliceCount == (ptPngInst->u4TotalSlice - 1))
        {
            rSclParam.u4IsLstBl = 1;
        }
        else
        {
            rSclParam.u4IsLstBl = 0;
        }
        rSclParam.fgPartialMode = TRUE;
        rSclParam.fgBlassign = TRUE;
        rSclParam.u4RowBufHeight = ptPngInst->u4SliceHeight;
        IMGRZ_Lock_Ex(PNG_RESIZER_ID);
		if(rSclParam.csc_des_format == E_RZ_OSD_CSC_FORMAT_ARGB8888)
		{
		    IMGRZ_ReInit_Ex(PNG_RESIZER_ID);
			if (ptPngInst->u4SliceCount != 0)
			{
				IMGRZ_Resume_Ex(PNG_RESIZER_ID);
			}
		}
		else
		{
			if (ptPngInst->u4SliceCount == 0)
			{
				IMGRZ_ReInit_Ex(PNG_RESIZER_ID);
			}
		}
        IMGRZ_SetScaleOpt_Ex(PNG_RESIZER_ID, E_RZ_INOUT_TYPE_OSDMD);
#ifdef PNG_DECODING_USING_RZ_IOMMU
#ifndef __MODEL_slt__
        rSclParam.u4RzIOMMU_MMUTableAddr = ptPngInst->u4MMUTableAddr;
		if(rSclParam.u4RzIOMMU_MMUTableAddr!=0)	
		{
        	IMGRZ_Cfg_MMU_IO_Ex(PNG_RESIZER_ID, 0, 1);
		}
#endif
#endif
        IMGRZ_Scale_Ex(PNG_RESIZER_ID, &rSclParam);
        IMGRZ_Flush_Ex(PNG_RESIZER_ID);
        IMGRZ_Wait_Ex(PNG_RESIZER_ID);
		if(rSclParam.csc_des_format == E_RZ_OSD_CSC_FORMAT_ARGB8888)
		{
			IMGRZ_Break_Ex(PNG_RESIZER_ID);
		}
        IMGRZ_Unlock_Ex(PNG_RESIZER_ID);
    }
    ptPngInst->u4ScaleRowH = IMGRZ_GetScaleLine_Ex(PNG_RESIZER_ID);

    if (ptPngInst->eDispFinalColorMode == Y_CBCR422_SEP16)
    {
        i4PngAYCbCrOsdToYCbCr422Video(ptPngInst);
    }
	else if(ptPngInst->eDispFinalColorMode == Y_CBCR420_SEP16)
	{
        i4PngAYCbCrOsdToYCbCr420Video(ptPngInst);
	}

    ptPngInst->u4ScaleRowY  += ptPngInst->u4ScaleRowH;
    ptPngInst->ePngDecState = PNG_DEC_STATE_WAIT_SCALE;
    ptPngInst->u4SliceCount += 1;

    if ((ptPngInst->pvDst_OnVideoPlane!=NULL)&&(ptPngInst->u4SliceCount==ptPngInst->u4TotalSlice) 
		&&(ptPngInst->eDispFinalColorMode != Y_CBCR422_SEP16)&&(ptPngInst->eDispFinalColorMode != Y_CBCR420_SEP16))
    {
        i4PngAYCbCrOsdToYCbCrVideo(ptPngInst);
    }

    if (ptPngInst->eDispFinalColorMode == Y_CBCR420_SEP16 && (ptPngInst->pvDst_OnVideoPlane!=NULL)
		&& (ptPngInst->u4SliceCount==ptPngInst->u4TotalSlice))
    {
//    	LOG(0, "## convert to 420 here.\n");
//        i4PngAYCbCr422ToYCbCr420Video(ptPngInst);
    }

    //need add memcopy
    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateWaitScale(PNG_INST_T *ptPngInst)
{
    //INT32 i4Ret;

    if (ptPngInst->u4TotalSlice > ptPngInst->u4SliceCount)
    {
        if ((ptPngInst->tPngHdr.tPngIhdr.fgInterlace) && (ptPngInst->ePngDecMd == PNG_DRV_DECMD_CLRBUF))
        {
            vPngLog_L("i4PngDecStateWaitScale--PNG_DEC_STATE_NEXT_DEC!!!!!!!!!!!!!!!!!!!!\n\n\n");
            ptPngInst->ePngDecState = PNG_DEC_STATE_NEXT_DEC;
        }
        else
        {
            vPngLog_L("i4PngDecStateWaitScale--PNG_DEC_STATE_NEXT_DEC\n");
            ptPngInst->ePngDecState = PNG_DEC_STATE_DST_RESUME;
        }
    }
    else
    {
        //vPngAbort(ptPngInst, IMG_NFY_FINISHED, TRUE);
        ptPngInst->fgAbort = TRUE;
        ptPngInst->u4NfyStatus = IMG_NFY_FINISHED;
    }
    return PNG_DRV_RTN_OK;
}

INT32 i4PngDecStateNextDec(PNG_INST_T *ptPngInst)
{
    //UINT32 u4ValTmp;
    if (i4PngInitPNGHal(ptPngInst) != PNG_DRV_RTN_OK)
    {
        return PNG_DRV_RTN_FAIL;
    }
    ptPngInst->u4IDATCount = 0;
    ptPngInst->ePngDecState = PNG_DEC_STATE_CHK_IDAT;
    ptPngInst->tFifoInfo.u4FileOfst = ptPngInst->tPngHdr.u41stIDATOfst;
    //trigger img dma to start transfer
    IMG_BsBufStartTrsData(ptPngInst->u4ImgId,ptPngInst->tFifoInfo.u4FileOfst);
    return PNG_DRV_RTN_OK;
}

void vPngStateDECODING(PNG_INST_T *ptPngInst)
{
    //decoding
    if (i4PngChkFifoStatus(ptPngInst) == PNG_DRV_RTN_OK)
    {

        switch (ptPngInst->ePngDecState)
        {
        case PNG_DEC_STATE_DEC:
            i4PngDecStateDEC(ptPngInst);
            i4PngDecStateWAIT(ptPngInst);
            break;
        case PNG_DEC_STATE_CHK_IDAT:
            i4PngDecStateChkIDAT(ptPngInst);
            break;
        case PNG_DEC_STATE_SRC_RESUME:
            i4PngDecStateSrcResume(ptPngInst);
            i4PngDecStateWAIT(ptPngInst);
            break;
        case PNG_DEC_STATE_DST_RESUME:
            i4PngDecStateDstResume(ptPngInst);
            i4PngDecStateWAIT(ptPngInst);
            break;
        case PNG_DEC_STATE_DO_SCALE:
            i4PngDecStateDoScale(ptPngInst);
            i4PngDecStateWaitScale(ptPngInst);
            break;
        case PNG_DEC_STATE_NEXT_DEC:
            i4PngDecStateNextDec(ptPngInst);
            break;
        default:
            vPngLog_H("[Png] ImgId=%x, Inexistence decoding state. Current STC %d\n", ptPngInst->u4ImgId, u4PngGetSTC());
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_INEXISTENCE_CASE;
            break;
        }
    }
}

#ifdef PNG_MID_PROTECTION
#include "x_mid.h"
static BOOL bPngMID_Protection(void)
{
	MID_AGENT_ID_T ePPAgentId;
    FBM_POOL_T* prFbmPool=NULL;
    FBM_POOL_T* prFbmPool2=NULL;
    BOOL fgRet = TRUE;
    
	//mid setting.
	ePPAgentId = MID_AGENT_JPEG;

	//protect working buffer
	prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
	
	MID_ResetRegion(4);	
    fgRet &= MID_SetRegionEx(4, ePPAgentId,
        (UINT32)prFbmPool->u4Addr,
        (UINT32)prFbmPool->u4Addr + (UINT32)(prFbmPool->u4Size),
        MID_FLAG_CPUACCESS);

	fgRet &= MID_EnableRegionProtect(4);

	//protect display buffer
#ifndef PHOTO_EMP_DDI_CONFIG	
	#ifdef CC_DFB_SUPPORT_VDP_LAYER
		prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
		prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
		MID_ResetRegion(3);	
	    fgRet &= MID_SetRegionEx2(3, MID_AGENT_CPU,
	        (UINT32)prFbmPool->u4Addr,
	        (UINT32)prFbmPool->u4Addr + (UINT32)(prFbmPool->u4Size),
	        MID_FLAG_CPUACCESS,
	        (1<<ePPAgentId));

		fgRet &= MID_EnableRegionProtect(3);
	#else
		prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD4);
		prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
		MID_ResetRegion(3);	
	    fgRet &= MID_SetRegionEx2(3, MID_AGENT_CPU,
	        (UINT32)prFbmPool->u4Addr,
	        (UINT32)prFbmPool2->u4Addr + (UINT32)(prFbmPool2->u4Size),
	        MID_FLAG_CPUACCESS,
	        (1<<ePPAgentId));

		fgRet &= MID_EnableRegionProtect(3);
	#endif
#endif
	return fgRet;
}
#endif /*PNG_MID_PROTECTION*/

void vPngStateSTART(PNG_INST_T *ptPngInst, UINT8 *vfifo_start, UINT32 vfifo_Size)
{
    FBM_POOL_T* prFbmPool;

    //check cache state, todo
    //init decode val
    vPngInitDecVal(ptPngInst);
    //init data and fifo

    //gst png-plugin will user FBM_POOL_TYPE_DMX buffer as vfifo
    ASSERT(vfifo_start != NULL);
    ptPngInst->tFifoInfo.u4FifoStart =  VIRTUAL((UINT32)vfifo_start + PNG_FIFO_OFFSET); //128 alignment
    ptPngInst->tFifoInfo.u4FifoEnd = ptPngInst->tFifoInfo.u4FifoStart  + vfifo_Size;
    ptPngInst->tFifoInfo.u4RdPoint = ptPngInst->tFifoInfo.u4FifoStart;
    ptPngInst->tFifoInfo.u4WrPoint = ptPngInst->tFifoInfo.u4FifoStart;
    ptPngInst->tFifoInfo.u4PreRdPoint = ptPngInst->tFifoInfo.u4FifoStart;

#ifdef PNG_MID_PROTECTION
    if(bPngMID_Protection()==FALSE)
    {
        LOG(0, "PNG MID protection is failed\n");
    }
#endif
    /*kunyi*/
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
    //prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PVR);
    ASSERT(prFbmPool->u4Addr != 0);

    ptPngInst->tPngHdr.tPngPlte.u4PlteData = VIRTUAL((UINT32)(prFbmPool->u4Addr) + PNG_PLTE_OFFSET);
    ptPngInst->tPngHdr.tPngTrns.u4TrnsData =  VIRTUAL((UINT32)(prFbmPool->u4Addr) + PNG_TRNS_OFFSET); //128 alignment

    HalFlushInvalidateDCacheMultipleLine(ptPngInst->tPngHdr.tPngPlte.u4PlteData, PNG_PLTE_SIZE);
    HalFlushInvalidateDCacheMultipleLine(ptPngInst->tPngHdr.tPngTrns.u4TrnsData, PNG_TRNS_SIZE);
    HalFlushInvalidateDCacheMultipleLine(ptPngInst->tFifoInfo.u4FifoStart, vfifo_Size);

#ifdef NINE_PATCH_PNG_SUPPORT
    ptPngInst->tPngHdr.u4npTcAddr = VIRTUAL((UINT32)(prFbmPool->u4Addr) + PNG_npTc_OFFSET);
    ptPngInst->tPngHdr.u4npTcOffset = PNG_npTc_OFFSET;
    HalFlushInvalidateDCacheMultipleLine(ptPngInst->tPngHdr.u4npTcAddr, PNG_npTc_SIZE);
#endif

    ptPngInst->tFifoInfo.u4ValidData = 0;
    ptPngInst->tFifoInfo.fgEoRange = FALSE;
    ptPngInst->tFifoInfo.u4FileOfst = 0;
    //initial setting fgRedecode = false  for ALDER_ERR redecode flow
    ptPngInst->fgRedecode = FALSE;
    ptPngInst->fgAbort = FALSE;
    ptPngInst->fgActive = TRUE;
    ptPngInst->i4PngErrorType = 0;
    ptPngInst->u4Progress = 0;
    ptPngInst->u2CompId = 0;
    ptPngInst->ePngState = PNG_STATE_IDLE;
    ptPngInst->ePngDecState = PNG_DEC_STATE_IDLE;
    ptPngInst->hPngSema = IMG_GetPngSema(ptPngInst->u4ImgId);

    VERIFY(i4PngHalGetHwInst(&ptPngInst->u4HwInstId) == PNG_HAL_RTN_OK);
    ptPngInst->ePngState = PNG_STATE_INIT;
}


void vPngStateINIT(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;
    PNG_HDR_FIFO_INFO *ptPngHdrFifo = &(ptPngInst->tPngHdr.tPngHdrFifo);
    PNG_FIFO_INFO_T *ptFifoInfo = &(ptPngInst->tFifoInfo);

    if (i4PngChkFifoStatus(ptPngInst) == PNG_DRV_RTN_OK)
    {
        ptPngHdrFifo->u4FifoStart = ptFifoInfo->u4FifoStart;
        ptPngHdrFifo->u4FifoEnd = ptFifoInfo->u4FifoEnd;
        ptPngHdrFifo->u4ReadPoint = ptFifoInfo->u4RdPoint;
        ptPngHdrFifo->u4WritePoint = ptFifoInfo->u4WrPoint;
        ptPngHdrFifo->u4ValidData = ptFifoInfo->u4ValidData;
        //check "89 PNG 0D 0A 1D 0A"
        i4Ret = i4PngHdrChkPngSign(&(ptPngInst->tPngHdr));
        if (i4Ret != PNG_HDR_OK)
        {
            //notify decode fail
            ptPngInst->i4PngErrorType = PNG_ERR_TYPE_SIGN_ERR;
        }
        else
        {
            ptPngInst->tFifoInfo.u4RdPoint += 8;
            ptPngInst->tFifoInfo.u4ValidData -= 8;
            ptPngInst->ePngState = PNG_STATE_PRS_HDR;
            vPngHdrInit(&(ptPngInst->tPngHdr));
        }
    }
}

void vPngStatePRSHDR(PNG_INST_T *ptPngInst)
{
    INT32 i4Ret;

    //start parser
    i4Ret = i4PngProcPsrHdr(ptPngInst);
    if (i4Ret != PNG_DRV_RTN_OK)
    {
        //notify decode fail
        ptPngInst->i4PngErrorType = PNG_ERR_TYPE_HEADERN_ERR;
    }
}

void vPngStateINITHAL(PNG_INST_T *ptPngInst)
{
    if (i4PngChkDstBuf(ptPngInst) != PNG_DRV_RTN_OK)
    {
        return;
    }

    if (i4PngInitPNGHal(ptPngInst) != PNG_DRV_RTN_OK)
    {
        return;
    }
    if (i4PngInitScale(ptPngInst) != PNG_DRV_RTN_OK)
    {
        return;
    }
    ptPngInst->ePngState = PNG_STATE_DECODING;
    ptPngInst->ePngDecState = PNG_DEC_STATE_DEC;
}



