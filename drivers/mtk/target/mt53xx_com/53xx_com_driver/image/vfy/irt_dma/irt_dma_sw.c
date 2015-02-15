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

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
//#include "sys_config.h"
#include "drv_config.h"
#include "x_printf.h"

#include "irt_dma_hw.h"
#include "irt_dma_sw.h"
#include "irt_dma_vfy.h"

INT32 i4IrtDmaSwYRotateMirror(UINT32 *pu4SrcSa,UINT32 *pu4DstSa,UINT32 u4Width, UINT32 u4Height, IRT_DMA_MODE_T eMode)
{

    UINT32 u4Width0=0,u4Height0=0;
    UINT32 i,j;
    UINT8 *pu4TempBuf = (UINT8 *)EV_IRTDMA_SW_DST_TEMP_SA;

    UINT8 *pu4TmpSrcSa = (UINT8 *)pu4SrcSa;
    UINT8 *pu4TmpDstSa = (UINT8 *)pu4DstSa;

    IRT_APIEntryEx("0x%08X, 0x%08X, %u, %u, %u", pu4SrcSa, pu4DstSa, u4Width, u4Height, eMode);

    VERIFY(pu4SrcSa != NULL);
    VERIFY(pu4DstSa != NULL);

    u4Width0 =(u4Width);
    u4Height0=(u4Height);

    switch(eMode)
    {
    case IRT_DMA_MODE_ROTATE_0:
        for(j=0;j<u4Height0;j++)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa++);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_0_MIRROR:
        for(j=0;j<u4Height0;j++)
        {
            for(i=u4Width0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*j+i-1);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_90:
        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*(i-1)+j);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_90_MIRROR:
        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*(i-1)+j);
            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Height0*j+i);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_180:
        for(j=u4Height0;j>0;j--)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*j-i-1);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_180_MIRROR:

        for(j=u4Height0;j>0;j--)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*j-i-1);

            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Height0;j++)
        {
            for(i=u4Width0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Width0*j+i);

            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_270:
        for(j=u4Width0;j>0;j--)
        {
            for(i=0;i<u4Height0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*i+j-1);

            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_270_MIRROR:
        for(j=u4Width0;j>0;j--)
        {
            for(i=0;i<u4Height0;i++)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*i+j-1);

            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Height0*j+i);

            }
        }
        break;

    default:
        break;
   }

   IRT_APILeave();

   return (RET_OK);
}


INT32 i4IrtDmaSwCRotateMirror(UINT32 *pu4SrcSa,UINT32 *pu4DstSa,UINT32 u4Width, UINT32 u4Height,IRT_DMA_MODE_T eMode)
{

    UINT32 u4Width0=0,u4Height0=0;
    UINT32 i,j;
    UINT16 *pu4TempBuf = (UINT16 *)EV_IRTDMA_SW_DST_TEMP_SA;

    UINT16 *pu4TmpSrcSa = (UINT16 *)pu4SrcSa;
    UINT16 *pu4TmpDstSa = (UINT16 *)pu4DstSa;

    IRT_APIEntryEx("0x%08X, 0x%08X, %u, %u, %u", pu4SrcSa, pu4DstSa, u4Width, u4Height, eMode);

    VERIFY(pu4SrcSa != NULL);
    VERIFY(pu4DstSa != NULL);

    u4Width0 =(u4Width/2);
    u4Height0=(u4Height/2);

    switch(eMode)
    {
    case IRT_DMA_MODE_ROTATE_0:
        for(j=0;j<u4Height0;j++)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa++);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_0_MIRROR:

        for(j=0;j<u4Height0;j++)
        {
            for(i=u4Width0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*j+i-1);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_90:
        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*(i-1)+j);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_90_MIRROR:

        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*(i-1)+j);
            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Height0*j+i);

            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_180:
        for(j=u4Height0;j>0;j--)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*j-i-1);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_180_MIRROR:
        for(j=u4Height0;j>0;j--)
        {
            for(i=0;i<u4Width0;i++)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*j-i-1);
            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Height0;j++)
        {
            for(i=u4Width0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Width0*j+i);

            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_270:
        for(j=u4Width0;j>0;j--)
        {
            for(i=0;i<u4Height0;i++)
            {
                *(pu4TmpDstSa++) = *(pu4TmpSrcSa+u4Width0*i+j-1);
            }
        }
        break;

    case IRT_DMA_MODE_ROTATE_270_MIRROR:
        for(j=u4Width0;j>0;j--)
        {
            for(i=0;i<u4Height0;i++)
            {
                *(pu4TempBuf++) = *(pu4TmpSrcSa+u4Width0*i+j-1);
            }
        }

        pu4TempBuf=pu4TempBuf-u4Width0*u4Height0-1;

        for(j=0;j<u4Width0;j++)
        {
            for(i=u4Height0;i>0;i--)
            {
                *(pu4TmpDstSa++) = *(pu4TempBuf+u4Height0*j+i);
            }
        }
        break;

    default:
        break;
   }

   IRT_APILeave();

   return (RET_OK);
}

INT32 i4IrtDmaSwCbCrAlignMode(UINT32 *pu4SrcYSa,UINT32 *pu4SrcCSa,UINT32 *pu4DstYSa,UINT32 u4Width, UINT32 u4Height)
{

    UINT32 u4Width0=0,u4Height0=0;
    UINT32 i,j;

    UINT8 *pu4TmpSrcYSa = (UINT8 *)pu4SrcYSa;
    UINT8 *pu4TmpSrcCSa = (UINT8 *)pu4SrcCSa;
    UINT16 *pu4TmpDstCbCrSa = (UINT16 *)pu4DstYSa;

    IRT_APIEntry();

    VERIFY(pu4SrcYSa != NULL);
    VERIFY(pu4SrcCSa != NULL);
    VERIFY(pu4DstYSa != NULL);

    u4Width0 =(u4Width/2);
    u4Height0=(u4Height);

    for(j=0;j<u4Height0;j++)
    {
        for(i=0;i<u4Width0;i++)
        {
            *(pu4TmpDstCbCrSa++) = (*(pu4TmpSrcCSa++)<<8)|(*(pu4TmpSrcYSa++));
        }
    }

    IRT_APILeave();

    return (RET_OK);
}

/*
 * i4FrameW align with 16.
 * i4FrameH align with 32.
 */
INT32 i4IrtDmaSwConvertSL420ToMB420(INT32 i4FrameW, INT32 i4FrameH, UINT32 *pu4SrcBufferY, UINT32 *pu4SrcBufferC, UINT32 *pu4OutBufferY, UINT32 *pu4OutBufferC)
{
    INT32 i4OutFrameW;
    INT32 h, w;
    INT32 i4WTotalBlock;
    INT32 i4BlockSize;
    BYTE *bSrcBufferY, *bSrcBufferC, *bOutBufferY, *bOutBufferC;

    IRT_APIEntryEx("%u, %u, 0x%08X, 0x%08X, 0x%08X, 0x%08X", i4FrameW, i4FrameH, pu4SrcBufferY, pu4SrcBufferC, pu4OutBufferY, pu4OutBufferC);

    ASSERT(0 == (i4FrameW % 16));
    ASSERT(0 == (i4FrameH % 32));

    bSrcBufferY = (BYTE *)pu4SrcBufferY;
    bSrcBufferC = (BYTE *)pu4SrcBufferC;
    bOutBufferY = (BYTE *)pu4OutBufferY;
    bOutBufferC = (BYTE *)pu4OutBufferC;

    i4OutFrameW = i4FrameW; // mb420, W align with 16 byte.

    i4WTotalBlock = i4OutFrameW / 16;
    i4BlockSize = 16 * 32;

    // height align with 32
    for (h = 0; h < i4FrameH; h++)
    {
        //UTIL_Printf("Processing line %u/%u\n", h+1, i4FrameH);

        // source width align with 16, output widht align with 16.
        for (w = 0; w < i4FrameW; w++)
        {
            INT32 u4BlockOffset;
            INT32 u4SubOffset; /* */
            INT32 i4WBlock = w / 16; // horizontal
            INT32 i4HBlock = h / 32; // vertical

            u4BlockOffset = i4HBlock * (i4WTotalBlock * i4BlockSize);
            u4BlockOffset += i4WBlock * i4BlockSize;
            u4SubOffset = (h % 32) * 16 + (w % 16);

            // Y
            *(bOutBufferY + u4BlockOffset + u4SubOffset) = *(bSrcBufferY + h * i4FrameW + w);

            //UTIL_Printf("[Y][%d][%d] block[%d][%d], offset=%u, sub_offset=%d, Y=%02X (%d)\r\n",
            //    h, w, i4HBlock, i4WBlock, u4BlockOffset, u4SubOffset,
            //    *(bSrcBufferY + h * i4FrameW + w), *(bSrcBufferY + h * i4FrameW + w));

            // there is 1 line of CbCr per lines of Y.
            if (0 == (h % 2))
            {
                // There is 1 CbCr line per 2 Y line.
                // Y block size is 16*32, CbCr block size is 16*16
                u4BlockOffset = i4HBlock * (i4WTotalBlock * (i4BlockSize >> 1));
                u4BlockOffset += i4WBlock * (i4BlockSize >> 1);
                u4SubOffset = ((h/2) % 16) * 16 + (w % 16);
                // Cb or Cr
                // Cb when w = 0, 2, 4,...
                // Cr When w = 1, 3, 5,...
                *(bOutBufferC + u4BlockOffset + u4SubOffset) = *(bSrcBufferC + h/2 * i4FrameW + w);

                //UTIL_Printf("[C][%d][%d] block[%d][%d], offset=%u, sub_offset=%d, C=%02X (%d)\r\n",
                //    h/2, w, i4HBlock, i4WBlock, u4BlockOffset, u4SubOffset,
                //    *(bSrcBufferC + h/2 * i4FrameW + w), *(bSrcBufferC + h/2 * i4FrameW + w));
            }
        }
    }

    IRT_APILeave();

    return 0;
}


/*
 * i4FrameW align with 16.
 * i4FrameH align with 32.
 */
INT32 i4IrtDmaSwConvertSL420To5351Mode(INT32 i4FrameW, INT32 i4FrameH, UINT32 *pu4SrcBufferY, UINT32 *pu4SrcBufferC, UINT32 *pu4OutBufferY, UINT32 *pu4OutBufferC)
{
    INT32 i4OutFrameW;
    INT32 h, w;
    INT32 i4WTotalBlock;
    INT32 i4BlockSize;
    BYTE *bSrcBufferY, *bSrcBufferC, *bOutBufferY, *bOutBufferC;

    IRT_APIEntryEx("%u, %u, 0x%08X, 0x%08X, 0x%08X, 0x%08X", i4FrameW, i4FrameH, pu4SrcBufferY, pu4SrcBufferC, pu4OutBufferY, pu4OutBufferC);

    ASSERT(0 == (i4FrameW % 16));
    ASSERT(0 == (i4FrameH % 32));

    bSrcBufferY = (BYTE *)pu4SrcBufferY;
    bSrcBufferC = (BYTE *)pu4SrcBufferC;
    bOutBufferY = (BYTE *)pu4OutBufferY;
    bOutBufferC = (BYTE *)pu4OutBufferC;

    i4OutFrameW = (i4FrameW + 63) & (~ 63); // 5351 mode, W align with 64 byte.

    i4WTotalBlock = i4OutFrameW / 64;
    i4BlockSize = 64 * 32;

    // height align with 32
    for (h = 0; h < i4FrameH; h++)
    {
        //UTIL_Printf("Processing line %u/%u\n", h+1, i4FrameH);

        // source width align with 16, output widht align with 64.
        for (w = 0; w < i4FrameW; w++)
        {
            INT32 u4BlockOffset;
            INT32 u4SubOffset; /* */
            INT32 i4WBlock = w / 64; // horizontal
            INT32 i4HBlock = h / 32; // vertical

            u4BlockOffset = i4HBlock * (i4WTotalBlock * i4BlockSize);
            u4BlockOffset += i4WBlock * i4BlockSize;
            u4SubOffset = (h % 32) * 64 + (w % 64);

            // Y
            *(bOutBufferY + u4BlockOffset + u4SubOffset) = *(bSrcBufferY + h * i4FrameW + w);

            //UTIL_Printf("[Y][%d][%d] block[%d][%d], offset=%u, sub_offset=%d, Y=%02X (%d)\r\n",
            //    h, w, i4HBlock, i4WBlock, u4BlockOffset, u4SubOffset,
            //    *(bSrcBufferY + h * i4FrameW + w), *(bSrcBufferY + h * i4FrameW + w));

            // there is 1 line of CbCr per lines of Y.
            if (0 == (h % 2))
            {
                // There is 1 CbCr line per 2 Y line.
                // Y block size is 64*32, CbCr block size is 64*16
                u4BlockOffset = i4HBlock * (i4WTotalBlock * (i4BlockSize >> 1));
                u4BlockOffset += i4WBlock * (i4BlockSize >> 1);
                u4SubOffset = ((h/2) % 16) * 64 + (w % 64);
                // Cb or Cr
                // Cb when w = 0, 2, 4,...
                // Cr When w = 1, 3, 5,...
                *(bOutBufferC + u4BlockOffset + u4SubOffset) = *(bSrcBufferC + h/2 * i4FrameW + w);

                //UTIL_Printf("[C][%d][%d] block[%d][%d], offset=%u, sub_offset=%d, C=%02X (%d)\r\n",
                //    h/2, w, i4HBlock, i4WBlock, u4BlockOffset, u4SubOffset,
                //    *(bSrcBufferC + h/2 * i4FrameW + w), *(bSrcBufferC + h/2 * i4FrameW + w));
            }
        }
    }

    IRT_APILeave();

    return 0;
}

