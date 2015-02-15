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

//#include "vdec_verify_vdec.h"
#include "x_drv_cli.h"
#include "x_serial.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_util.h"
#include "x_stl_lib.h"
#include "drv_config.h"
#include "chip_ver.h"
#include "drv_osd_if.h"
#include "imgresz_hal_if.h"
#include "x_timer.h"



//---------------------------------------------------------------------
// LOG macro related

#define DEFINE_IS_LOG	CLI_IsLog
#include "x_debug.h"
#include "x_rtos.h"
#include "drv_imgresz.h"
#include "drv_imgresz_errcode.h"
#include "chip_ver.h"

#include "sys_config.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

#include <linux/module.h>


#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
#define IMGRESZ_CMD_INST_NUM 14
#define IMGRESZ_CMD_COLORFMT_NUM 7
#else
#define IMGRESZ_CMD_INST_NUM 12
#define IMGRESZ_CMD_COLORFMT_NUM 6

#endif

typedef enum {
    IMGRESZ_CMD_INST_STATE_IDLE = 0,
    IMGRESZ_CMD_INST_STATE_RESIZE
} IMGRESZ_CMD_INST_STATE_T;


typedef enum {
    IMGRESZ_CMD_INST_COLOR_MODE_JPEG = 0,
    IMGRESZ_CMD_INST_COLOR_MODE_VIDEO,
    IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB,
    IMGRESZ_CMD_INST_COLOR_MODE_OSD_INDEX,
    IMGRESZ_CMD_INST_COLOR_MODE_AYUV_TO_ARGB,
    IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT,
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)    
    IMGRESZ_CMD_INST_COLOR_MODE_RM_VIDEO,
#endif    
} IMGRESZ_CMD_INST_COLOR_MODE_T;


typedef struct {
    HANDLE_T hHandle;                                      /// Thread handle
    UINT32 u4InstId;                                       /// Instance ID
    volatile IMGRESZ_CMD_INST_STATE_T eState;              /// State
    UINT32 u4ReszCnt;                                      /// Resize count
    volatile BOOL fgFinish;                                /// Resize finish
    volatile BOOL fgStop;                                  /// Stop resize
} IMGRESZ_CMD_INST_T;


// Instance data
IMGRESZ_CMD_INST_T _rImgreszCmdInst[IMGRESZ_CMD_INST_NUM];

BOOL _fgWTEnable = FALSE;
BOOL _fg8550GraphIntegrationTest = FALSE;
BOOL _fg8555GraphIntegrationTest = TRUE;
BOOL _fgUse950X540Surface = FALSE;


INT32 i4ImgReszCmdIntegrateWTDoResize(UINT32 u4TgtBufAddr);
void vImgreszBurnThread(void *arg);
void vImgreszBurnThread1(void * arg);
void vImgreszBurnThread540(void * arg);
void vImgreszWtBurnThread(void *arg);



void vImgReszCmdBufEnBlock(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufWidth,UINT32 u4BufHeight,
                           UINT32 u4BlockWidth,UINT32 u4BlockHeight)
{
	UINT32 u4Addr;
	UINT32 u4Len;
	UINT32 u4X,u4Y;

    u4X = 0;
    u4Y = 0;
    u4Len = u4BufWidth*u4BufHeight;
    u4Addr = 0;

    while(u4Addr<u4Len)
    {
        *(UCHAR *)(u4DestBufSA + u4Addr) = *(UCHAR *)(u4SrcBufSA + u4Y*u4BufWidth + u4X);
        u4Addr++;
        u4X++;
        if((u4X % u4BlockWidth) == 0)
        {
            u4X -= u4BlockWidth;
            u4Y++;
            if((u4Y % u4BlockHeight) == 0)
            {
                u4Y -= u4BlockHeight;
                u4X += u4BlockWidth;

                if(u4X>=u4BufWidth)
                {
                    u4Y += u4BlockHeight;
                    u4X = 0;
                }
            }
        }
    }
}


void vImgReszCmdBufDeBlock(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufWidth,UINT32 u4BufHeight,
                           UINT32 u4BlockWidth,UINT32 u4BlockHeight)
{
	UINT32 u4Addr;
	UINT32 u4Len;
	UINT32 u4X,u4Y;

    u4X = 0;
    u4Y = 0;
    u4Len = u4BufWidth*u4BufHeight;
    u4Addr = 0;

    while(u4Addr<u4Len)
    {
        *(UCHAR *)(u4DestBufSA + u4Y*u4BufWidth + u4X) = *(UCHAR *)(u4SrcBufSA + u4Addr);
        u4Addr++;
        u4X++;
        if((u4X % u4BlockWidth) == 0)
        {
            u4X -= u4BlockWidth;
            u4Y++;
            if((u4Y % u4BlockHeight) == 0)
            {
                u4Y -= u4BlockHeight;
                u4X += u4BlockWidth;

                if(u4X>=u4BufWidth)
                {
                    u4Y += u4BlockHeight;
                    u4X = 0;
                }
            }
        }
    }
}


void vImgReszCmdBufEnAddrSwap(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufLen)
{
    UINT32 u4Addr;
    UINT32 u4AddrSwap;

    u4BufLen >>= 4;

    for(u4Addr=0; u4Addr<u4BufLen; u4Addr++)
    {
        u4AddrSwap = (u4Addr & 0xFFFFFFC0) | ((u4Addr&0x38) >> 3) | ((u4Addr&0x6) << 2) | ((u4Addr&0x1) << 5); // Address mode (1,0)
        //u4AddrSwap = (u4Addr & 0xFFFFFFE0) | ((u4Addr&0x1E) >> 1) | ((u4Addr&0x1) << 4);  // Address mode (1,1)
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4)) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4));
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 4) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 4);
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 8) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 8);
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 12) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 12);
    }
}


void vImgReszCmdBufDeAddrSwap(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufLen)
{
    UINT32 u4Addr;
    UINT32 u4AddrSwap;

    u4BufLen >>= 4;

    for(u4Addr=0; u4Addr<u4BufLen; u4Addr++)
    {
        u4AddrSwap = (u4Addr & 0xFFFFFFC0) | ((u4Addr&0x7) << 3) | ((u4Addr&0x18) >> 2) | ((u4Addr&0x20) >> 5); // Address mode (1,0)
        //u4AddrSwap = (u4Addr & 0xFFFFFFE0) | ((u4Addr&0xF) << 1) | ((u4Addr&0x10) >> 4);  // Address mode (1,1)
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4)) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4));
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 4) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 4);
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 8) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 8);
        *(UINT32 *)(u4DestBufSA + (u4AddrSwap<<4) + 12) = *(UINT32 *)(u4SrcBufSA + (u4Addr<<4) + 12);
    }
}



INT32 ImgReszCmdNotifyCallback(INT32 i4State,void *pvPrivData)
{
    if(i4State == S_IMGRESZ_DRV_RESIZE_FINISH)
    _rImgreszCmdInst[(UINT32)pvPrivData].fgFinish = TRUE;
    else if(i4State == S_IMGRESZ_DRV_RESIZE_STOP)
        _rImgreszCmdInst[(UINT32)pvPrivData].fgStop = TRUE;
    return 0;
}


void vImgReszCmdDoResize(IMGRESZ_CMD_INST_T *prInst,IMGRESZ_CMD_INST_COLOR_MODE_T eColorMode)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T rImgReszTicket;
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_PARTIAL_INFO_T rPartialBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    //IMGRESZ_DRV_DO_SCALE_T rDoScale;
    IMGRESZ_DRV_SRC_COLOR_MODE eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF;
    IMGRESZ_DRV_DST_COLOR_MODE eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_AYUV;
    UINT32 u4ColorPallet;
    BOOL fgPartialMode = FALSE;
    BOOL fgPartialModeStop = TRUE;
    BOOL fgInterlaced = FALSE;
    BOOL fgJpegProgressivePreload = FALSE;
    BOOL fgSrcDstTheSameBuf = FALSE;
    UINT32 u4MaxResizeCnt;
    UINT32 u4ResizeCnt;

    UINT32 u4SrcBufWidth = 0;
    UINT32 u4SrcBufHeight = 64;
    UINT32 u4SrcPicWidth = 16;
    UINT32 u4SrcPicHeight = 64;
    UINT32 u4SrcPicXOff = 0;
    UINT32 u4SrcPicYOff = 0;
    UINT32 u4SrcYBufAddr;
    UINT32 u4SrcCbBufAddr;
    UINT32 u4SrcCrBufAddr;
    UINT32 u4SrcWorkBufAddr = 0;

    UINT32 u4DstBufWidth = 0;
    UINT32 u4DstBufHeight = 64;
    UINT32 u4DstPicWidth = 16;
    UINT32 u4DstPicHeight = 64;
    UINT32 u4DstPicXOff = 0;
    UINT32 u4DstPicYOff = 0;
    UINT32 u4DstYBufAddr;
    UINT32 u4DstCBufAddr;
    UINT32 u4DstWorkBufAddr = 0;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)    
    IMGRESZ_DRV_RM_INFO_T rRMInfo = {0};
#endif

    switch(eColorMode)
    {
        case IMGRESZ_CMD_INST_COLOR_MODE_JPEG:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_AYUV;
            fgPartialMode = TRUE;
            break;
        case IMGRESZ_CMD_INST_COLOR_MODE_VIDEO:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_420_BLK;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
            fgInterlaced = TRUE;
            break;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)            
        case IMGRESZ_CMD_INST_COLOR_MODE_RM_VIDEO:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_420_BLK;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK;
            fgInterlaced = TRUE;
            break;
#endif            
        case IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
            break;
        case IMGRESZ_CMD_INST_COLOR_MODE_OSD_INDEX:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_8BPP_IDX;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
            break;
        case IMGRESZ_CMD_INST_COLOR_MODE_AYUV_TO_ARGB:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_AYUV;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
            break;
        default:
            break;
    }

    switch(eSrcColorMode)
    {
        case IMGRESZ_DRV_INPUT_COL_MD_8BPP_IDX:
            u4SrcBufWidth = u4SrcPicWidth;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888:
        case IMGRESZ_DRV_INPUT_COL_MD_AYUV:
            u4SrcBufWidth = u4SrcPicWidth*4;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF:
            u4SrcBufWidth = u4SrcPicWidth;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_420_BLK:
            u4SrcBufWidth = ((u4SrcPicWidth+15)>>4)<<4;
            u4SrcBufHeight = ((u4SrcPicHeight+31)>>5)<<5;
            break;
        default:
            break;
    }

    switch(eDstColorMode)
    {
        case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888:
        case IMGRESZ_DRV_OUTPUT_COL_MD_AYUV:
            u4DstBufWidth = u4DstPicWidth*4;
            break;
        case IMGRESZ_DRV_OUTPUT_COL_MD_420_RS:
            u4DstBufWidth = u4DstPicWidth;
            break;
        case IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK:
            u4DstBufWidth = ((u4DstPicWidth+15)>>4)<<4;
            u4DstBufHeight = ((u4DstPicHeight+31)>>5)<<5;
            break;
        default:
            break;
    }

    // Allocate memory
    if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_420_BLK)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        u4SrcYBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcCbBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcWorkBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
#elif CONFIG_SYS_MEM_PHASE3
        u4SrcYBufAddr = (UINT32)x_mem_aligned_alloc(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcCbBufAddr = (UINT32)x_mem_aligned_alloc(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcWorkBufAddr = (UINT32)x_mem_aligned_alloc(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
#endif
        #else
        u4SrcYBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcCbBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        u4SrcWorkBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10,1024);
        #endif
    } else
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        u4SrcYBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4SrcBufWidth*u4SrcBufHeight,16);
        u4SrcCbBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4SrcBufWidth*u4SrcBufHeight,16);
#elif CONFIG_SYS_MEM_PHASE3
        u4SrcYBufAddr = (UINT32)x_mem_aligned_alloc(u4SrcBufWidth*u4SrcBufHeight,16);
        u4SrcCbBufAddr = (UINT32)x_mem_aligned_alloc(u4SrcBufWidth*u4SrcBufHeight,16);
#endif
        #else
        u4SrcYBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4SrcBufWidth*u4SrcBufHeight,16);
        u4SrcCbBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4SrcBufWidth*u4SrcBufHeight,16);
        #endif
    }
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    u4SrcCrBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4SrcBufWidth*u4SrcBufHeight,16);
#elif CONFIG_SYS_MEM_PHASE3
    u4SrcCrBufAddr = (UINT32)x_mem_aligned_alloc(u4SrcBufWidth*u4SrcBufHeight,16);
#endif
    #else
    u4SrcCrBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4SrcBufWidth*u4SrcBufHeight,16);
    #endif
    if(eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        u4DstYBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstCBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstWorkBufAddr = (UINT32)x_alloc_aligned_dma_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
#elif CONFIG_SYS_MEM_PHASE3
        u4DstYBufAddr = (UINT32)x_mem_aligned_alloc(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstCBufAddr = (UINT32)x_mem_aligned_alloc(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstWorkBufAddr = (UINT32)x_mem_aligned_alloc(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
#endif
        #else
        u4DstYBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstCBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        u4DstWorkBufAddr = (UINT32)x_alloc_aligned_nc_mem(((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10,1024);
        #endif
    } else
    {
        if(fgSrcDstTheSameBuf)
            u4DstYBufAddr = u4SrcYBufAddr;
        else
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            u4DstYBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4DstBufWidth*u4DstBufHeight,16);
#elif CONFIG_SYS_MEM_PHASE3
            u4DstYBufAddr = (UINT32)x_mem_aligned_alloc(u4DstBufWidth*u4DstBufHeight,16);
#endif
            #else
            u4DstYBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4DstBufWidth*u4DstBufHeight,16);
            #endif

        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        u4DstCBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4DstBufWidth*u4DstBufHeight,16);
#elif CONFIG_SYS_MEM_PHASE3
        u4DstCBufAddr = (UINT32)x_mem_aligned_alloc(u4DstBufWidth*u4DstBufHeight,16);
#endif
        #else
        u4DstCBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4DstBufWidth*u4DstBufHeight,16);
        #endif
    }
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    u4ColorPallet = (UINT32)x_alloc_aligned_dma_mem(256*4,1);
#elif CONFIG_SYS_MEM_PHASE3
    u4ColorPallet = (UINT32)x_mem_aligned_alloc(256*4,1);
#endif
    #else
    u4ColorPallet = (UINT32)x_alloc_aligned_nc_mem(256*4,1);
    if ((UINT8 *)u4ColorPallet == NULL)
    {
        ASSERT(0);
    }
    #endif

    if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_8BPP_IDX)
    {
        x_memset((void *)u4SrcYBufAddr,0x0,u4SrcBufWidth*u4SrcBufHeight);
        *(UINT8 *)(u4ColorPallet) = 0x80;
        *(UINT8 *)(u4ColorPallet+1) = 0x80;
        *(UINT8 *)(u4ColorPallet+2) = 0x80;
        *(UINT8 *)(u4ColorPallet+3) = 0x80;
    } else if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_AYUV)
    {
        UINT32 u4I;

        for(u4I=0; u4I<(u4SrcBufWidth*u4SrcBufHeight); u4I++)
        {
            switch(u4I & 3)
            {
                case 0:
                    *(UINT8 *)(u4SrcYBufAddr + u4I) = 0x80;
                    break;
                case 1:
                    *(UINT8 *)(u4SrcYBufAddr + u4I) = 0x80;
                    break;
                case 2:
                    *(UINT8 *)(u4SrcYBufAddr + u4I) = 0xF0;
                    break;
                case 3:
                    *(UINT8 *)(u4SrcYBufAddr + u4I) = 0x80;
                    break;
            }
        }
    } else
    {
        if(fgJpegProgressivePreload)
        {
            x_memset((void *)u4SrcYBufAddr,0x90,u4SrcBufWidth*u4SrcBufHeight);
            x_memset((void *)u4SrcCbBufAddr,0x90,u4SrcBufWidth*u4SrcBufHeight);
            x_memset((void *)u4SrcCrBufAddr,0x90,u4SrcBufWidth*u4SrcBufHeight);
        } else
        {
            x_memset((void *)u4SrcYBufAddr,0x80,u4SrcBufWidth*u4SrcBufHeight);
            x_memset((void *)u4SrcCbBufAddr,0x80,u4SrcBufWidth*u4SrcBufHeight);
            x_memset((void *)u4SrcCrBufAddr,0x80,u4SrcBufWidth*u4SrcBufHeight);
        }
    }

    // Raster Scan To Block and En Address swap
    if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_420_BLK)
    {
        vImgReszCmdBufEnBlock(u4SrcYBufAddr,u4SrcWorkBufAddr,u4SrcBufWidth,u4SrcBufHeight,16,32);
        vImgReszCmdBufEnAddrSwap(u4SrcWorkBufAddr,u4SrcYBufAddr,((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10);
        vImgReszCmdBufEnBlock(u4SrcCbBufAddr,u4SrcWorkBufAddr,u4SrcBufWidth,u4SrcBufHeight,16,32);
        vImgReszCmdBufEnAddrSwap(u4SrcWorkBufAddr,u4SrcCbBufAddr,((u4SrcBufWidth*u4SrcBufHeight+1023)>>10)<<10);
    }

    if(fgJpegProgressivePreload)
    {
        x_memset((void *)u4DstYBufAddr,0x0,u4DstBufWidth*u4DstBufHeight);
        x_memset((void *)u4DstCBufAddr,0x0,u4DstBufWidth*u4DstBufHeight);
    } else
    {
        if(!fgSrcDstTheSameBuf)
            x_memset((void *)u4DstYBufAddr,0x11,u4DstBufWidth*u4DstBufHeight);
        x_memset((void *)u4DstCBufAddr,0x11,u4DstBufWidth*u4DstBufHeight);
    }

    // Get ticket
    i4Ret = i4ImgResz_Drv_GetTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Set priority
    i4Ret = i4ImgResz_Drv_SetPriority(&rImgReszTicket,IMGRESZ_DRV_PRIORITY_LOW);

    // Set lock
    if(fgPartialMode)
    {
        i4Ret = i4ImgResz_Drv_SetLock(&rImgReszTicket,TRUE);
    }

    // Set scale mode
    if(fgPartialMode)
        i4Ret = i4ImgResz_Drv_SetScaleMode(&rImgReszTicket,IMGRESZ_DRV_PARTIAL_SCALE);
    else
        i4Ret = i4ImgResz_Drv_SetScaleMode(&rImgReszTicket,IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    // Set source buffer info
    x_memset(&rSrcBufInfo,0,sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    rSrcBufInfo.eSrcColorMode = eSrcColorMode;
    rSrcBufInfo.u4BufWidth = u4SrcBufWidth;
    rSrcBufInfo.u4BufHeight = u4SrcBufHeight;
    rSrcBufInfo.u4PicWidth = u4SrcPicWidth;
    rSrcBufInfo.u4PicHeight = u4SrcPicHeight;
    rSrcBufInfo.u4PicPosX = u4SrcPicXOff;
    rSrcBufInfo.u4PicPosY = u4SrcPicYOff;
    rSrcBufInfo.u4YBufAddr = u4SrcYBufAddr;
    rSrcBufInfo.u4CbBufAddr = u4SrcCbBufAddr;
    rSrcBufInfo.u4CrBufAddr = u4SrcCrBufAddr;
    rSrcBufInfo.u4ColorPalletSa = u4ColorPallet;
    rSrcBufInfo.rCompFactor.u1YCompFactorH = 2;
    rSrcBufInfo.rCompFactor.u1CbCompFactorH = 1;
    rSrcBufInfo.rCompFactor.u1CrCompFactorH = 1;
    rSrcBufInfo.rCompFactor.u1YCompFactorV = 2;
    rSrcBufInfo.rCompFactor.u1CbCompFactorV = 1;
    rSrcBufInfo.rCompFactor.u1CrCompFactorV = 1;
    if(fgInterlaced)
    {
        rSrcBufInfo.fgInterlaced = TRUE;
        rSrcBufInfo.fgTopField = TRUE;
        rSrcBufInfo.fgBottomField = TRUE;
    }
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(&rImgReszTicket,&rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    x_memset(&rDstBufInfo,0,sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    rDstBufInfo.eDstColorMode = eDstColorMode;
    rDstBufInfo.u4BufWidth = u4DstBufWidth;
    rDstBufInfo.u4BufHeight = u4DstBufHeight;
    rDstBufInfo.u4PicWidth = u4DstPicWidth;
    rDstBufInfo.u4PicHeight = u4DstPicHeight;
    rDstBufInfo.u4PicPosX = u4DstPicXOff;
    rDstBufInfo.u4PicPosY = u4DstPicYOff;
    rDstBufInfo.u4YBufAddr = u4DstYBufAddr;
    rDstBufInfo.u4CBufAddr = u4DstCBufAddr;
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(&rImgReszTicket,&rDstBufInfo);
    VERIFY(i4Ret >= 0);

    // Register notify callback function
    rNotifyCallbackReg.pvCallBackFunc = ImgReszCmdNotifyCallback;
    rNotifyCallbackReg.pvPrivData = (void *)prInst->u4InstId;
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(&rImgReszTicket,&rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    rRMInfo.fgRPRMode = TRUE;
    rRMInfo.fgRacingMode = FALSE;
    i4Ret = i4ImgResz_Drv_SetRmInfo(&rImgReszTicket, &rRMInfo);
#endif

    // Set Jpeg info
    if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF)
    {
        IMGRESZ_DRV_JPEG_INFO_T rJpegInfo;

        x_memset(&rJpegInfo,0,sizeof(IMGRESZ_DRV_JPEG_INFO_T));
        // Notice, you can set all component to TRUE.
        // or Just one component can be TRUE.
        rJpegInfo.fgExistY = TRUE;
        rJpegInfo.fgExistCb = TRUE;
        rJpegInfo.fgExistCr = TRUE;
        rJpegInfo.fgPreload = fgJpegProgressivePreload;
        i4ImgResz_Drv_SetJpegInfo(&rImgReszTicket,&rJpegInfo);
    }

    if(fgJpegProgressivePreload)
        u4MaxResizeCnt = 8;
    else
        u4MaxResizeCnt = 1;

    u4ResizeCnt = 0;

    while(u4ResizeCnt < u4MaxResizeCnt)
    {
    if(fgPartialMode)
    {
        UINT32 u4RowBufY;
        UINT32 u4RowBufHeight = 16;

        for(u4RowBufY=0; u4RowBufY<rSrcBufInfo.u4PicHeight; u4RowBufY+=u4RowBufHeight)
        {
            // Set partial buffer info
            x_memset(&rPartialBufInfo,0,sizeof(IMGRESZ_DRV_PARTIAL_INFO_T));
            if(u4RowBufY == 0)
                rPartialBufInfo.fgFirstRow = TRUE;
            else
                rPartialBufInfo.fgFirstRow = FALSE;

            if((u4RowBufY + u4RowBufHeight) < rSrcBufInfo.u4PicHeight)
                rPartialBufInfo.fgLastRow= FALSE;
            else
                rPartialBufInfo.fgLastRow = TRUE;

            rPartialBufInfo.u4YBufLine = u4RowBufHeight;
            rPartialBufInfo.u4YBufAddr = rSrcBufInfo.u4YBufAddr + rSrcBufInfo.u4BufWidth * u4RowBufY;
            rPartialBufInfo.u4CbBufAddr = rSrcBufInfo.u4CbBufAddr + rSrcBufInfo.u4BufWidth * u4RowBufY;
            rPartialBufInfo.u4CrBufAddr = rSrcBufInfo.u4CrBufAddr + rSrcBufInfo.u4BufWidth * u4RowBufY;
            i4ImgResz_Drv_SetPartialBufInfo(&rImgReszTicket,&rPartialBufInfo);

            prInst->fgFinish = FALSE;

            // Do scale
            while((i4Ret = i4ImgResz_Drv_DoScale(&rImgReszTicket,NULL)) == E_IMGRESZ_DRV_LOCK_FAIL)
            {
                x_thread_delay(1);
            }

            VERIFY(i4Ret >= 0);

            // Partial mode stop function test
            if(fgPartialModeStop)
            {
                IMGRESZ_DRV_SCALE_STATE_T rImgReszState;

                prInst->fgStop = FALSE;
                
                rImgReszState.eState = IMGRESZ_DRV_STATE_NONE;
                do
                {
                    x_thread_delay(1);
                    i4Ret = i4ImgResz_Drv_GetState(&rImgReszTicket,&rImgReszState);
                    VERIFY(i4Ret >= 0);
                } while(rImgReszState.eState == IMGRESZ_DRV_STATE_START);
                
                i4Ret = i4ImgResz_Drv_StopScale(&rImgReszTicket);
                VERIFY(i4Ret >= 0);

                while(!prInst->fgStop)
                {
                    x_thread_delay(10);            
                }
                break;
            }

            while(!prInst->fgFinish)
            {
                x_thread_delay(10);
            }
        }
    } else
    {
        prInst->fgFinish = FALSE;

        // Do scale
        while((i4Ret = i4ImgResz_Drv_DoScale(&rImgReszTicket,NULL)) == E_IMGRESZ_DRV_LOCK_FAIL)
        {
            x_thread_delay(1);
        }

        VERIFY(i4Ret >= 0);

        while(!prInst->fgFinish)
        {
            x_thread_delay(10);
        }
    }

        u4ResizeCnt++;
    }

    // Set unlock
    if(fgPartialMode)
        i4Ret = i4ImgResz_Drv_SetLock(&rImgReszTicket,FALSE);

    // Release ticket
    i4Ret = i4ImgResz_Drv_ReleaseTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    if(!fgPartialModeStop)
    {

    // De Address swap and Block To Raster Scan
    if(eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK)
    {
        vImgReszCmdBufDeAddrSwap(u4DstYBufAddr,u4DstWorkBufAddr,((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10);
        vImgReszCmdBufDeBlock(u4DstWorkBufAddr,u4DstYBufAddr,u4DstBufWidth,u4DstBufHeight,16,32);
        vImgReszCmdBufDeAddrSwap(u4DstCBufAddr,u4DstWorkBufAddr,((u4DstBufWidth*u4DstBufHeight+1023)>>10)<<10);
        vImgReszCmdBufDeBlock(u4DstWorkBufAddr,u4DstCBufAddr,u4DstBufWidth,u4DstBufHeight,16,32);
    }

    // Check output accuracy
    {
        UINT32 u4I;

        for(u4I=0; u4I<(u4DstBufWidth*u4DstBufHeight); u4I++)
        {
            if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_AYUV)
            {
                switch(u4I & 3)
                {
                    case 0:
                        if(*(BYTE *)(u4DstYBufAddr + u4I) != 0xFF)
                            VERIFY(FALSE);
                        break;
                    case 1:
                        if(*(BYTE *)(u4DstYBufAddr + u4I) != 0xFF)
                            VERIFY(FALSE);
                        break;
                    case 2:
                        if(*(BYTE *)(u4DstYBufAddr + u4I) != 0xFF)
                            VERIFY(FALSE);
                        break;
                    case 3:
                        if(*(BYTE *)(u4DstYBufAddr + u4I) != 0x80)
                            VERIFY(FALSE);
                        break;
                }
            } else if((eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_AYUV) && ((u4I & 3) == 3))
            { // Do not compare alpha data.
            }
            else if(*(BYTE *)(u4DstYBufAddr + u4I) != 0x80)
                VERIFY(FALSE);
        }

        if((eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_RS) ||
           (eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK))
        {
            for(u4I=0; u4I<((u4DstBufWidth*u4DstBufHeight)>>1); u4I++)
            {
                if(*(BYTE *)(u4DstCBufAddr + u4I) != 0x80)
                    VERIFY(FALSE);
            }
        }
    }

    }

    // Free memory
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    x_free_aligned_dma_mem((void *)u4SrcYBufAddr);
    x_free_aligned_dma_mem((void *)u4SrcCbBufAddr);
    x_free_aligned_dma_mem((void *)u4SrcCrBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
    x_mem_aligned_free((void *)u4SrcYBufAddr);
    x_mem_aligned_free((void *)u4SrcCbBufAddr);
    x_mem_aligned_free((void *)u4SrcCrBufAddr);
#endif
    #else
    x_free_aligned_nc_mem((void *)u4SrcYBufAddr);
    x_free_aligned_nc_mem((void *)u4SrcCbBufAddr);
    x_free_aligned_nc_mem((void *)u4SrcCrBufAddr);
    #endif
    if(!fgSrcDstTheSameBuf)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)u4DstYBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)u4DstYBufAddr);
#endif
        #else
        x_free_aligned_nc_mem((void *)u4DstYBufAddr);
        #endif

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY 
#if CONFIG_SYS_MEM_PHASE2
    x_free_aligned_dma_mem((void *)u4DstCBufAddr);
    x_free_aligned_dma_mem((void *)u4ColorPallet);
#elif CONFIG_SYS_MEM_PHASE3
    x_mem_aligned_free((void *)u4DstCBufAddr);
    x_mem_aligned_free((void *)u4ColorPallet);
#endif
    #else
    x_free_aligned_nc_mem((void *)u4DstCBufAddr);
    x_free_aligned_nc_mem((void *)u4ColorPallet);
    #endif
    if(eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_420_BLK)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY 
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)u4SrcWorkBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)u4SrcWorkBufAddr);
#endif
        #else
        x_free_aligned_nc_mem((void *)u4SrcWorkBufAddr);
        #endif
    if(eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK)
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY 
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)u4DstWorkBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void *)u4DstWorkBufAddr);
#endif
        #else
        x_free_aligned_nc_mem((void *)u4DstWorkBufAddr);
        #endif
}


void vImgReszCmdDoWtResize(IMGRESZ_CMD_INST_T *prInst,IMGRESZ_CMD_INST_COLOR_MODE_T eColorMode)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T rImgReszTicket;
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    IMGRESZ_DRV_SRC_COLOR_MODE eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888;
    IMGRESZ_DRV_DST_COLOR_MODE eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;

    UINT32 u4SrcBufWidth = 0;
    UINT32 u4SrcBufHeight = 64;
    UINT32 u4SrcPicWidth = 16;
    UINT32 u4SrcPicHeight = 64;
    UINT32 u4SrcPicXOff = 0;
    UINT32 u4SrcPicYOff = 0;
    UINT32 u4SrcYBufAddr;

    UINT32 u4WtBufWidth = 0;
    UINT32 u4WtBufHeight = 64;
    UINT32 u4WtPicWidth = 16;
    UINT32 u4WtPicHeight = 64;
    UINT32 u4WtPicXOff = 0;
    UINT32 u4WtPicYOff = 0;
    UINT32 u4WtYBufAddr;    

    UINT32 u4DstBufWidth = 0;
    UINT32 u4DstBufHeight = 64;
    UINT32 u4DstPicWidth = 16;
    UINT32 u4DstPicHeight = 64;
    UINT32 u4DstPicXOff = 0;
    UINT32 u4DstPicYOff = 0;
    UINT32 u4DstYBufAddr;


    switch(eColorMode)
    {
        case IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT:
            u4SrcBufWidth = u4SrcPicWidth*4;
            u4WtBufWidth = u4WtPicWidth*4;
            u4DstBufWidth = u4DstPicWidth*4;            
            break;
        default:
            break;
    }


    // Allocate memory
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    u4SrcYBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4SrcBufWidth*u4SrcBufHeight,16);
    u4WtYBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4WtBufWidth*u4WtBufHeight,16);
    u4DstYBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4DstBufWidth*u4DstBufHeight,16);
#elif CONFIG_SYS_MEM_PHASE3
    u4SrcYBufAddr = (UINT32)x_mem_aligned_alloc(u4SrcBufWidth*u4SrcBufHeight,16);
    u4WtYBufAddr = (UINT32)x_mem_aligned_alloc(u4WtBufWidth*u4WtBufHeight,16);
    u4DstYBufAddr = (UINT32)x_mem_aligned_alloc(u4DstBufWidth*u4DstBufHeight,16);
#endif
    #else
    u4SrcYBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4SrcBufWidth*u4SrcBufHeight,16);
    u4WtYBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4WtBufWidth*u4WtBufHeight,16);
    u4DstYBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4DstBufWidth*u4DstBufHeight,16);
    #endif

    x_memset((void *)u4SrcYBufAddr,0x12,u4SrcBufWidth*u4SrcBufHeight);
    x_memset((void *)u4WtYBufAddr,0x32,u4WtBufWidth*u4WtBufHeight);
    x_memset((void *)u4DstYBufAddr,0x52,u4DstBufWidth*u4DstBufHeight);

    // Turn on WT
    *((volatile UINT32*)(0x70040800)) = (*((volatile UINT32*)(0x70040800))) & 0xFFFEFFFF;

    // Source buffer to WT buffer
    switch(eColorMode)
    {
        case IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
            break;
        default:
            break;
    }

    // Get ticket
    i4Ret = i4ImgResz_Drv_GetTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Set priority
    i4Ret = i4ImgResz_Drv_SetPriority(&rImgReszTicket,IMGRESZ_DRV_PRIORITY_LOW);
    VERIFY(i4Ret >= 0);

    // Set scale mode
    i4Ret = i4ImgResz_Drv_SetScaleMode(&rImgReszTicket,IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    // Set source buffer info
    x_memset(&rSrcBufInfo,0,sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    rSrcBufInfo.eSrcColorMode = eSrcColorMode;
    rSrcBufInfo.u4BufWidth = u4SrcBufWidth;
    rSrcBufInfo.u4BufHeight = u4SrcBufHeight;
    rSrcBufInfo.u4PicWidth = u4SrcPicWidth;
    rSrcBufInfo.u4PicHeight = u4SrcPicHeight;
    rSrcBufInfo.u4PicPosX = u4SrcPicXOff;
    rSrcBufInfo.u4PicPosY = u4SrcPicYOff;
    rSrcBufInfo.u4YBufAddr = u4SrcYBufAddr;
    rSrcBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(&rImgReszTicket,&rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    x_memset(&rDstBufInfo,0,sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    rDstBufInfo.eDstColorMode = eDstColorMode;
    rDstBufInfo.u4BufWidth = u4WtBufWidth;
    rDstBufInfo.u4BufHeight = u4WtBufHeight;
    rDstBufInfo.u4PicWidth = u4WtPicWidth;
    rDstBufInfo.u4PicHeight = u4WtPicHeight;
    rDstBufInfo.u4PicPosX = u4WtPicXOff;
    rDstBufInfo.u4PicPosY = u4WtPicYOff;
    rDstBufInfo.u4YBufAddr = u4WtYBufAddr;
    rDstBufInfo.fgWTEnable = TRUE;
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(&rImgReszTicket,&rDstBufInfo);
    VERIFY(i4Ret >= 0);

    // Register notify callback function
    rNotifyCallbackReg.pvCallBackFunc = ImgReszCmdNotifyCallback;
    rNotifyCallbackReg.pvPrivData = (void *)prInst->u4InstId;
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(&rImgReszTicket,&rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    prInst->fgFinish = FALSE;

    // Do scale
    while((i4Ret = i4ImgResz_Drv_DoScale(&rImgReszTicket,NULL)) == E_IMGRESZ_DRV_LOCK_FAIL)
    {
        x_thread_delay(1);
    }

    VERIFY(i4Ret >= 0);

    while(!prInst->fgFinish)
    {
        x_thread_delay(10);
    }

    // Release ticket
    i4Ret = i4ImgResz_Drv_ReleaseTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Clear source buffer
    x_memset((void *)u4SrcYBufAddr,0x72,u4SrcBufWidth*u4SrcBufHeight);

    // WT buffer to destination buffer
    switch(eColorMode)
    {
        case IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT:
            eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888;
            eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
            break;
        default:
            break;
    }

    // Get ticket
    i4Ret = i4ImgResz_Drv_GetTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Set priority
    i4Ret = i4ImgResz_Drv_SetPriority(&rImgReszTicket,IMGRESZ_DRV_PRIORITY_LOW);
    VERIFY(i4Ret >= 0);

    // Set scale mode
    i4Ret = i4ImgResz_Drv_SetScaleMode(&rImgReszTicket,IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    // Set source buffer info
    x_memset(&rSrcBufInfo,0,sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    rSrcBufInfo.eSrcColorMode = eSrcColorMode;
    rSrcBufInfo.u4BufWidth = u4WtBufWidth;
    rSrcBufInfo.u4BufHeight = u4WtBufHeight;
    rSrcBufInfo.u4PicWidth = u4WtPicWidth;
    rSrcBufInfo.u4PicHeight = u4WtPicHeight;
    rSrcBufInfo.u4PicPosX = u4WtPicXOff;
    rSrcBufInfo.u4PicPosY = u4WtPicYOff;
    rSrcBufInfo.u4YBufAddr = u4WtYBufAddr;
    rSrcBufInfo.fgWTEnable = TRUE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(&rImgReszTicket,&rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    x_memset(&rDstBufInfo,0,sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    rDstBufInfo.eDstColorMode = eDstColorMode;
    rDstBufInfo.u4BufWidth = u4DstBufWidth;
    rDstBufInfo.u4BufHeight = u4DstBufHeight;
    rDstBufInfo.u4PicWidth = u4DstPicWidth;
    rDstBufInfo.u4PicHeight = u4DstPicHeight;
    rDstBufInfo.u4PicPosX = u4DstPicXOff;
    rDstBufInfo.u4PicPosY = u4DstPicYOff;
    rDstBufInfo.u4YBufAddr = u4DstYBufAddr;
    rDstBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(&rImgReszTicket,&rDstBufInfo);
    VERIFY(i4Ret >= 0);

    // Register notify callback function
    rNotifyCallbackReg.pvCallBackFunc = ImgReszCmdNotifyCallback;
    rNotifyCallbackReg.pvPrivData = (void *)prInst->u4InstId;
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(&rImgReszTicket,&rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    prInst->fgFinish = FALSE;

    // Do scale
    while((i4Ret = i4ImgResz_Drv_DoScale(&rImgReszTicket,NULL)) == E_IMGRESZ_DRV_LOCK_FAIL)
    {
        x_thread_delay(1);
    }

    VERIFY(i4Ret >= 0);

    while(!prInst->fgFinish)
    {
        x_thread_delay(10);
    }

    // Release ticket
    i4Ret = i4ImgResz_Drv_ReleaseTicket(&rImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Turn off WT
    *((volatile UINT32*)(0x70040800)) = (*((volatile UINT32*)(0x70040800))) | 0x10000;

    // Check output accuracy
    {
        UINT32 u4I;
        UINT32 u4Addr;

        for(u4I=0; u4I<(u4DstBufWidth*u4DstBufHeight); u4I+=4)
        {
            if(*(UINT32 *)(u4DstYBufAddr + u4I) != 0x12111111)
                VERIFY(FALSE);
        }

        u4Addr = u4WtYBufAddr + (u4WtBufWidth*u4WtBufHeight)/2;
        for(u4I=0; u4I<((u4WtBufWidth*u4WtBufHeight)/2); u4I++)
        {
            if(*(BYTE *)(u4Addr + u4I) != 0x32)
                VERIFY(FALSE);
        }
    }

    // Free memory
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY 
#if CONFIG_SYS_MEM_PHASE2
    x_free_aligned_dma_mem((void *)u4SrcYBufAddr);
    x_free_aligned_dma_mem((void *)u4WtYBufAddr);
    x_free_aligned_dma_mem((void *)u4DstYBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
    x_mem_aligned_free((void *)u4SrcYBufAddr);
    x_mem_aligned_free((void *)u4WtYBufAddr);
    x_mem_aligned_free((void *)u4DstYBufAddr);
#endif
#else
    x_free_aligned_nc_mem((void *)u4SrcYBufAddr);
    x_free_aligned_nc_mem((void *)u4WtYBufAddr);
    x_free_aligned_nc_mem((void *)u4DstYBufAddr);
#endif    
}



void vImgReszCmdThread(void *arg)
{
    UINT32 u4InstNum = *(UINT32 *)arg;
    IMGRESZ_CMD_INST_T *prInst;
    IMGRESZ_CMD_INST_COLOR_MODE_T eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_JPEG;

    prInst = &(_rImgreszCmdInst[u4InstNum]);
    prInst->u4InstId = u4InstNum;

    switch(prInst->u4InstId % IMGRESZ_CMD_COLORFMT_NUM)
    {
        case 0:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB;
            break;
        case 1:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_OSD_INDEX;
            break;
        case 2:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_JPEG;
            break;
        case 3:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_VIDEO;
            break;
        case 4:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_AYUV_TO_ARGB;
            break;
        case 5:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT;
            break;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)            
        case 6:
            eColorMode = IMGRESZ_CMD_INST_COLOR_MODE_RM_VIDEO;
            break;
#endif            
    }

    while(TRUE)
    {
        switch(prInst->eState)
        {
            case IMGRESZ_CMD_INST_STATE_IDLE:
                break;
            case IMGRESZ_CMD_INST_STATE_RESIZE:
                if(eColorMode == IMGRESZ_CMD_INST_COLOR_MODE_OSD_ARGB_WT)
                {
                    vImgReszCmdDoWtResize(prInst,eColorMode);                
                } else
                {
                    vImgReszCmdDoResize(prInst,eColorMode);
                }
                prInst->u4ReszCnt++;
                break;
        }

        x_thread_delay(20 + 7*u4InstNum);
    }
}

void vImgreszDrawLine(BYTE *u4Mem)
{
   //UINT32  aru4Clr[4] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFF808080};
   UINT32      i4H = 0 ;
   static UINT32    u4Ofst = 0;
   u4Ofst = 0;
   for (i4H=0; i4H<1080; i4H++)
   {
       u4Mem[u4Ofst + 760*2] = 0xF0;
       u4Mem[u4Ofst + 760*2 + 1] = 0x00;
       u4Mem[u4Ofst + 760*2 + 2] = 0xF0;
       u4Mem[u4Ofst + 760*2 + 3] = 0xFF;
        
       u4Ofst += 1920*2;
   }
}

void vImgreszDrawLine1(BYTE *u4Mem)
{
   //UINT32  aru4Clr[4] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFF808080};
   UINT32      i4H = 0 ;
   static UINT32    u4Ofst = 0;
   u4Ofst = 0;
   for (i4H=0; i4H<1080; i4H++)
   {
       u4Mem[u4Ofst + 760*4] = 0xFF;
       u4Mem[u4Ofst + 760*4 + 1] = 0x00;
       u4Mem[u4Ofst + 760*4 + 2] = 0xFF;
       u4Mem[u4Ofst + 760*4 + 3] = 0xFF;
        
       u4Ofst += 1920*4;
   }
}

void vImgreszDrawLine2(BYTE *u4Mem)
{
   //UINT32  aru4Clr[4] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFF808080};
   UINT32      i4H = 0 ;
   static UINT32    u4Ofst = 0;
   u4Ofst = 0;
   for (i4H=0; i4H<540; i4H++)
   {
       u4Mem[u4Ofst + 380*4] = 0xFF;
       u4Mem[u4Ofst + 380*4 + 1] = 0x00;
       u4Mem[u4Ofst + 380*4 + 2] = 0xFF;
       u4Mem[u4Ofst + 380*4 + 3] = 0xFF;
        
       u4Ofst += 960*4;
   }
}



#define OSD2_SURFACE_SIZE  (1920*1080*2) // for 8550
#define OSD2_SURFACE_SZIE1 (1920*1080*4) // for 8555
#define OSD2_SURFACE_SIZE2 (960*540*4) // for 8555

UINT32 _u4Golden = 0;
UINT32 _u4WB[2];

extern void vGfxShowOSD(UINT32 u4OsdMem1, UINT32 u4OsdMem2,
                      UINT32 *pu4RgnList1, UINT32 *pu4RgnList2,
                      UINT32 *pu4Rgn1, UINT32 *pu4Rgn2,UINT32 u4Plane);


static INT32 _Cmd_ImgResz_Init(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum;
    INT32 i4Ret;
    CHAR instName[12] = "Resz_X_Inst";

    //i4Ret = i4ImgResz_Drv_Init();
    //if(i4Ret < 0)
    //    UTIL_Printf("Image resizer driver init fail\n");


    for(u4InstNum=0; u4InstNum<IMGRESZ_CMD_INST_NUM; u4InstNum++)
    {
        x_memset(&(_rImgreszCmdInst[u4InstNum]),0,sizeof(IMGRESZ_CMD_INST_T));

        instName[5] = u4InstNum + '0';
        i4Ret = x_thread_create (&(_rImgreszCmdInst[u4InstNum].hHandle),
                                  (const CHAR*)instName,
                                  2048,     /* stack size */
                                  127,      /* priority */
                                  vImgReszCmdThread,
                                  sizeof(UINT32),
                                  (VOID*)&u4InstNum);

       if(i4Ret < 0)
           UTIL_Printf("Create instance %d error %d\n",u4InstNum,i4Ret);
    }

    return 0;
}


static INT32 _Cmd_ImgResz_Info(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum;

    for(u4InstNum=0; u4InstNum<IMGRESZ_CMD_INST_NUM; u4InstNum++)
    {
        UTIL_Printf("Instance %d, state %d, resize count %d\n",u4InstNum,_rImgreszCmdInst[u4InstNum].eState,
                                                                    _rImgreszCmdInst[u4InstNum].u4ReszCnt);
    }

    return 0;
}


static INT32 _Cmd_ImgResz_Resz(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum;

    if(i4Argc > 1)
    {
        u4InstNum = (UINT32)StrToInt(aszArgv[1]);

        if(u4InstNum >= IMGRESZ_CMD_INST_NUM)
        {
            UTIL_Printf("Instance num %d is invalid.\n",u4InstNum);
            return -1;
        }

        _rImgreszCmdInst[u4InstNum].eState = IMGRESZ_CMD_INST_STATE_RESIZE;
    }

    return 0;
}


static INT32 _Cmd_ImgResz_Stop(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum;

    if(i4Argc > 1)
    {
        u4InstNum = (UINT32)StrToInt(aszArgv[1]);

        if(u4InstNum >= IMGRESZ_CMD_INST_NUM)
        {
            UTIL_Printf("Instance num %d is invalid.\n",u4InstNum);
            return -1;
        }

        _rImgreszCmdInst[u4InstNum].eState = IMGRESZ_CMD_INST_STATE_IDLE;
    }

    return 0;
}

static INT32 _Cmd_ImgResz_Burn(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum = 0;
    INT32 i4Ret;
    CHAR arcTaskName[10] = "ImgRBurn0";
    HANDLE_T hHandle;
    UINT32 u4Temp = 0;

    if (i4Argc <= 1)
    {
        _fgWTEnable = FALSE;
    }
    else
    {
        u4Temp = (UINT32)StrToInt(aszArgv[1]);
        if (u4Temp == 0)
        {
            _fgWTEnable = FALSE;
        }
        else
        {
            _fgWTEnable = TRUE;
            arcTaskName[8] = '1';
        }
        
    }

    UTIL_Printf("[IMGRESZ] WT enable = %d \n",_fgWTEnable);
    i4Ret = x_thread_create (&hHandle,
                             (const CHAR*)arcTaskName,
                             2048,     /* stack size */
                             127,      /* priority */
                             vImgreszBurnThread,
                             sizeof(UINT32),
                             (VOID*)&u4InstNum);

   if(i4Ret < 0)
   {
       UTIL_Printf("Create image resizer burn in test thread fail \n");
   }

    return 0;
}

extern void vGFXSemaphoreCreat(void);


static INT32 _Cmd_ImgResz_Burn1(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum = 0;
    INT32 i4Ret;
    CHAR arcTaskName[10] = "ImgRBurn2";
    HANDLE_T hHandle;
    

    vGFXSemaphoreCreat();
    i4Ret = x_thread_create (&hHandle,
                             (const CHAR*)arcTaskName,
                             2048,     /* stack size */
                             127,      /* priority */
                             vImgreszBurnThread1,
                             sizeof(UINT32),
                             (VOID*)&u4InstNum);

   if(i4Ret < 0)
   {
       UTIL_Printf("Create image resizer burn in test thread fail \n");
   }

    return 0;
}

static INT32 _Cmd_ImgResz_Burn540(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum = 0;
    INT32 i4Ret;
    CHAR arcTaskName[10] = "ImgRBurn3";
    HANDLE_T hHandle;
    _fgUse950X540Surface = TRUE;
    
    i4Ret = x_thread_create (&hHandle,
                             (const CHAR*)arcTaskName,
                             2048,     /* stack size */
                             127,      /* priority */
                             vImgreszBurnThread540,
                             sizeof(UINT32),
                             (VOID*)&u4InstNum);
    
    UTIL_Printf("Create image resizer burn with 960x540 32bpp \n");

    if(i4Ret < 0)
    {
       UTIL_Printf("Create image resizer burn in test thread fail \n");
    }

    return 0;
}

static INT32 _Cmd_ImgResz_WtBurn(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4InstNum = 0;
    INT32 i4Ret;
    CHAR arcTaskName[10] = "ImgRBurn4";
    HANDLE_T hHandle;
    
    i4Ret = x_thread_create (&hHandle,
                             (const CHAR*)arcTaskName,
                             2048,     /* stack size */
                             127,      /* priority */
                             vImgreszWtBurnThread,
                             sizeof(UINT32),
                             (VOID*)&u4InstNum);
    
    UTIL_Printf("Create image resizer burn with 960x540 32bpp \n");

    if(i4Ret < 0)
    {
       UTIL_Printf("Create image resizer burn in test thread fail \n");
    }

    return 0;
}

extern IMGRESZ_HAL_RESAMPLE_METHOD_T _eHResampleMethod;
extern IMGRESZ_HAL_RESAMPLE_METHOD_T _eVResampleMethod;

static INT32 _Cmd_ImgResz_SetResampleMethod(INT32 i4Argc, const CHAR** aszArgv)
{
    INT32 Arg1,Arg2;

    if(i4Argc != 3)
    {
        UTIL_Printf("Argument loss or too much \n");
        UTIL_Printf("eg.resmtd 2 1 | Arg1 : H resample method  | Arg2 : V resample method \n");

        return 0;
    }
    else
    {
        
        Arg1 = StrToInt(aszArgv[1]);
        Arg2 = StrToInt(aszArgv[2]);
        UTIL_Printf("Arg1 = %d  Arg2 = %d \n",Arg1, Arg2);
        if ((Arg1 > 3) || (Arg2 > 3) || (Arg1 < 0) || (Arg2 < 0))
        {
            UTIL_Printf("Argument invalid \n");
            UTIL_Printf("0 : Bilnear | 1 : 4_TAP | 2 : 8_TAP \n");
            return 0;
        }
        _eHResampleMethod = Arg1;
        _eVResampleMethod = Arg2;
    }
    return 0;
    
}

extern UINT32 _au4IMGRZ_FILTERCOEF_H[7][18];
extern UINT32 _au4IMGRZ_FILTERCOEF_V[7][9];

static INT32 _Cmd_ImgResz_SetResampleCoff(INT32 i4Argc, const CHAR** aszArgv)
{
    INT32 Arg1, Arg2;
#if 0    
    INT32 Arg3, Arg4, Arg5, Arg6, Arg7,Arg8, Arg9, Arg10;
    INT32 Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17,Arg18,Arg19, Arg20;
#endif    
    UINT32 u4Temp = 0;

    
    if(((i4Argc != 21)&&(StrToInt(aszArgv[1]) == 0))||
        ((i4Argc != 12)&&(StrToInt(aszArgv[1]) == 1)))
    {
        UTIL_Printf("Arguments loss or too much \n");
        UTIL_Printf("Ex. H/V direc + line + coffecients  \n");
        UTIL_Printf("0 - H direct | 1 - V direct  \n");
        UTIL_Printf("H - 18 coffecients | V - 9 coffecients \n");

        return 0;
    }
    else
    {
        
        Arg1 = StrToInt(aszArgv[1]);
        Arg2 = StrToInt(aszArgv[2]);
#if 0        
        Arg3 = StrToInt(aszArgv[3]);
        Arg4 = StrToInt(aszArgv[4]);
        Arg5 = StrToInt(aszArgv[5]);
        Arg6 = StrToInt(aszArgv[6]);
        Arg7 = StrToInt(aszArgv[7]);
        Arg8 = StrToInt(aszArgv[8]);
        Arg9 = StrToInt(aszArgv[9]);
        Arg10 = StrToInt(aszArgv[10]);
        Arg11 = StrToInt(aszArgv[11]);
        Arg12 = StrToInt(aszArgv[12]);
        Arg13 = StrToInt(aszArgv[13]);
        Arg14 = StrToInt(aszArgv[14]);
        Arg15 = StrToInt(aszArgv[15]);
        Arg16 = StrToInt(aszArgv[16]);
        Arg17 = StrToInt(aszArgv[17]);
        Arg18 = StrToInt(aszArgv[18]);
        Arg19 = StrToInt(aszArgv[19]);
        Arg20 = StrToInt(aszArgv[20]);
#endif        
        UTIL_Printf("Arg1 = %d  Arg2 = %d \n",Arg1, Arg2);

        // H/V direction 0 - H ; 1 - V
        if (Arg1 > 1)
        {
            UTIL_Printf("Argument 1 invalid -- should less than 2 \n");
            return 0;
        }
        if (Arg2 > 6)
        {
            UTIL_Printf("Argument 2 invalid -- should less than 7 \n");
            return 0;
        }
        if (Arg1 == 0)
        {
            for (u4Temp = 0; u4Temp < 18; u4Temp ++)
            {
                _au4IMGRZ_FILTERCOEF_H[Arg2][u4Temp] = StrToInt(aszArgv[u4Temp + 3]);
            }
        }
        else
        {
            for (u4Temp = 0; u4Temp < 9; u4Temp ++)
            {
                _au4IMGRZ_FILTERCOEF_V[Arg2][u4Temp]= StrToInt(aszArgv[u4Temp + 3]);
            }
        }
    }
    return 0;
}

static BOOL _fgReszDone = FALSE;
HAL_TIME_T _rReszStart, _rReszEnd, _rReszTotal;

INT32 i4ImgReszHwInstNfyCallback(INT32 i4State,void *pvPrivData)
{
    _fgReszDone = TRUE;
    
    return 0;
}

/*If the variable is TRUE, we shold enable the iommu config in imgresz hal,
  otherwise, we should disable the iommu config in imgresz hal
*/
BOOL _fgImgreszPerformanceTestUseIommu = FALSE;

static INT32 _Cmd_ImgResz_TestPerformance(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4SrcWidth, u4SrcHeight, u4DstWidth, u4DstHeight, u4Count;
	UINT32 u4SrcSA = 0;
    UINT32 u4DstSA = 0;
//#endif
	UINT32 u4Temp;
    //#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
    UINT32 u4SrcHdl = NULL_HANDLE;
    UINT32 u4DstHdl = NULL_HANDLE;
    //#endif
    UINT32 u4x, u4y,u4i;
    UINT32 u4Buffer = 0;
    IMGRESZ_HAL_IMG_INFO_T rSrcImgInfo;
    IMGRESZ_HAL_IMG_INFO_T rDestImgInfo;
    //IMGRESZ_HAL_IMG_INFO_T rBldImgInfo;
    IMGRESZ_HAL_NOTIFY_CB_REG_T rNofifyCallback;
    HAL_TIME_T rTemp;
    UINT64 u8Total =0;
    
    
    if (i4Argc < 6)
    {
        UTIL_Printf("Arguments invalid \n");
        UTIL_Printf("source width, source height, destination width, destination height, test number \n");
        UTIL_Printf("Ex, pertest 1920 1080 960 540 5 \n");
    }
    x_memset(&rTemp, 0, sizeof(HAL_TIME_T));
    x_memset(&_rReszStart, 0, sizeof(HAL_TIME_T));
    x_memset(&_rReszEnd, 0, sizeof(HAL_TIME_T));
    x_memset(&_rReszTotal, 0, sizeof(HAL_TIME_T));
    u4SrcWidth = (StrToInt)(aszArgv[1]);
    u4SrcHeight = (StrToInt)(aszArgv[2]);
    u4DstWidth = (StrToInt)(aszArgv[3]);
    u4DstHeight = (StrToInt)(aszArgv[4]);
    u4Count = (StrToInt)(aszArgv[5]);

    UTIL_Printf("[RESZ] Src,0x%x,0x%x, Dst,0x%x,0x%x \n",u4SrcWidth,u4SrcHeight,u4DstWidth,u4DstHeight);
    /*create source & destination surface*/
    u4SrcWidth = (u4SrcWidth + 15)/16 * 16;
    u4DstWidth = (u4DstWidth + 15)/16 * 16;
    //#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)  
    if (_fgImgreszPerformanceTestUseIommu)
    {
    #if CONFIG_SYS_MEM_PHASE3
        u4SrcSA = (UINT32)x_mem_valloc(u4SrcWidth * u4SrcHeight * 4);
        u4DstSA = (UINT32)x_mem_valloc(u4DstWidth * u4DstHeight * 4);
	#endif
        if ((u4SrcSA == 0) || (u4DstSA == 0))
        {
            UTIL_Printf("[RESZ]Allocate surface fail \n" );
            return 0;
        }
    }
    else
    {
     #if CONFIG_SYS_MEM_PHASE3
        u4SrcHdl = x_kmem_block_aligned_alloc(u4SrcWidth * u4SrcHeight * 4,128);
        u4DstHdl = x_kmem_block_aligned_alloc(u4DstWidth * u4DstHeight * 4,128);
	#endif
        if ((u4SrcHdl == NULL_HANDLE) ||(u4DstHdl == NULL_HANDLE))
        {
            UTIL_Printf("[RESZ]Allocate surface fail \n");
            return 0;
        }
		 #if CONFIG_SYS_MEM_PHASE3
        u4SrcSA = (UINT32)x_kmem_kernel_address(u4SrcHdl, 0);
        u4DstSA = (UINT32)x_kmem_kernel_address(u4DstHdl, 0);
		#endif
    }
    //#endif
    UTIL_Printf("[RESZ] Source 0x%x, Destination 0x%x \n",u4SrcSA, u4DstSA);

    u4Temp = 0xFF001122;
    u4Buffer = u4SrcSA;
    /*fill source surface*/
    for (u4y = 0; u4y < u4SrcHeight; u4y ++)
    {
        for (u4x = 0; u4x < u4SrcWidth; u4x ++)
        {
            if (0xFF000102 + u4x * 0x010101 >= 0xFFEEEEEE)
            {
                *(UINT32 *)u4Buffer = 0xFF000102;
            }
            else
            {
                *(UINT32 *)u4Buffer= 0xFF000102 + u4x * 0x010101;
            }
            u4Buffer += 4;
        }
        u4Buffer = u4SrcSA + u4SrcWidth * 4 * (u4y+1);
    }
    BSP_dma_map_vaddr(u4SrcSA, u4SrcWidth * u4SrcHeight * 4, TO_DEVICE);
    
    UTIL_Printf("[RESZ] Fill source buffer done \n");
    /*do scale*/
    for (u4i = 0; u4i < u4Count; u4i ++)
    {
        i4ImgResz_HAL_Init(0);
        i4ImgResz_HAL_Set_Resize_Mode(0,IMGRESZ_HAL_RESIZE_MODE_FRAME);
        i4ImgResz_HAL_Set_Resample_Method(0,IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR,IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR);
        
        x_memset(&rSrcImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
        rSrcImgInfo.rBufferFormat.fgBlockMode = FALSE;
        rSrcImgInfo.rBufferFormat.fgAddrSwap = FALSE;
        rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
        rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;

        rSrcImgInfo.u4ImgXOff = 0;
        rSrcImgInfo.u4ImgYOff = 0;
        rSrcImgInfo.u4BufSA1 = u4SrcSA;
        rSrcImgInfo.u4BufSA2 = 0;
        rSrcImgInfo.u4BufSA3 = 0;
        rSrcImgInfo.u4BufWidth = u4SrcWidth * 4;
        rSrcImgInfo.u4BufHeight = u4SrcHeight;
        rSrcImgInfo.u4ImgWidth = u4SrcWidth;
        rSrcImgInfo.u4ImgHeight = u4SrcHeight;
        i4ImgResz_HAL_Set_Source_Image_Info(0,&rSrcImgInfo);


        x_memset(&rDestImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
        rDestImgInfo.rBufferFormat.fgBlockMode = FALSE;
        rDestImgInfo.rBufferFormat.fgAddrSwap = FALSE;
        rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
        rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
        rDestImgInfo.u4BufWidth = u4DstWidth * 4;
        rDestImgInfo.u4BufHeight = u4DstHeight;
        rDestImgInfo.u4ImgWidth = u4DstWidth;
        rDestImgInfo.u4ImgHeight = u4DstHeight;
        rDestImgInfo.u4ImgXOff = 0;
        rDestImgInfo.u4ImgYOff = 0;
        rDestImgInfo.u4BufSA1 = u4DstSA;
        rDestImgInfo.u4BufSA2 = 0;
        i4ImgResz_HAL_Set_Destination_Image_Info(0,&rDestImgInfo);

        rNofifyCallback.pvCallBackFunc = i4ImgReszHwInstNfyCallback;
        rNofifyCallback.pvPrivData = (void *)0;
        i4ImgResz_HAL_Reg_Notify_Callback(0,&rNofifyCallback);

    

        HAL_GetTime(&_rReszStart);
        i4ImgResz_HAL_Resize(0);

        while(1)
        {
            //UTIL_Printf("[RESZ] _fgReszDone = %d \n",_fgReszDone);
            if (_fgReszDone)
            {
                //UTIL_Printf("[RESZ] Scale done \n");
                i4ImgResz_HAL_Uninit(0);
                _fgReszDone = FALSE;
                break;
            }
            x_thread_delay(5);
        }
        if (_rReszEnd.u4Micros < _rReszStart.u4Micros)
        {
            rTemp.u4Micros = 1000000+ _rReszEnd.u4Micros - _rReszStart.u4Micros;
            rTemp.u4Seconds = _rReszEnd.u4Seconds - 1 - _rReszStart.u4Seconds;
        }
        else
        {
            rTemp.u4Micros = _rReszEnd.u4Micros - _rReszStart.u4Micros;
            rTemp.u4Seconds = _rReszEnd.u4Seconds- _rReszStart.u4Seconds;
        }

        if (_rReszTotal.u4Micros + rTemp.u4Micros > 1000000)
        {
            _rReszTotal.u4Micros = _rReszTotal.u4Micros + rTemp.u4Micros - 1000000;
            _rReszTotal.u4Seconds = _rReszTotal.u4Seconds + rTemp.u4Seconds + 1;
        }
        else
        {
            _rReszTotal.u4Micros = _rReszTotal.u4Micros + rTemp.u4Micros;
            _rReszTotal.u4Seconds = _rReszTotal.u4Seconds + rTemp.u4Seconds;
        }
        //UTIL_Printf("[RESZ] Start %ds %dms \n",_rStart.u4Seconds, _rStart.u4Micros);
        //UTIL_Printf("[RESZ] End %ds %dms \n",_rEnd.u4Seconds,_rEnd.u4Micros);
        //UTIL_Printf("[RESZ] Total %ds %dms \n",_rTotal.u4Seconds,_rTotal.u4Micros);
        x_memset(&rTemp, 0, sizeof(HAL_TIME_T));
        x_memset(&_rReszStart, 0, sizeof(HAL_TIME_T));
        x_memset(&_rReszEnd, 0, sizeof(HAL_TIME_T));
    }
    BSP_dma_unmap_vaddr(u4SrcSA, u4SrcWidth * u4SrcHeight * 4, TO_DEVICE);
    u8Total = _rReszTotal.u4Seconds * 1000000 + _rReszTotal.u4Micros;
    UTIL_Printf("[RESZ_CMD] Total = %lld \n", u8Total);
    UTIL_Printf("[RESZ_CMD] Average scale time is %lld us \n", u8Total/u4Count);
    
    //#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)  
    if (_fgImgreszPerformanceTestUseIommu)
    {
     #if CONFIG_SYS_MEM_PHASE3
        x_mem_vfree((void *)u4SrcSA);
        x_mem_vfree((void *)u4DstSA);
	 #endif	
    }
    else
    {
     #if CONFIG_SYS_MEM_PHASE3
        x_kmem_block_aligned_free(u4SrcHdl);
        x_kmem_block_aligned_free(u4DstHdl);
    #endif
    }
    //#else
    
    //#endif
    return 0;
}

//===========================================================================
CLI_EXEC_T _arImgReszCmdTbl[] =
{
  //Start to decode
  {"init",     "init",    _Cmd_ImgResz_Init,      NULL, "Initialization", CLI_GUEST},
  {"info",     "info",    _Cmd_ImgResz_Info,      NULL, "Information", CLI_GUEST},
  {"resz",     "resz", 	  _Cmd_ImgResz_Resz,      NULL, "Resize", CLI_GUEST},
  {"stop",     "stop", 	  _Cmd_ImgResz_Stop,      NULL, "Stop", CLI_GUEST},
  {"burn",     "burn",    _Cmd_ImgResz_Burn,      NULL, "Burn in test", CLI_GUEST},
  {"burn1",    "burn1",   _Cmd_ImgResz_Burn1,     NULL, "Burn in test for 85555", CLI_GUEST},  
  {"burn540",  "burn540", _Cmd_ImgResz_Burn540,   NULL, "Burn in test for 960x540", CLI_GUEST},  
  {"wtburn",   "wtburn",  _Cmd_ImgResz_WtBurn,    NULL, "Burn in test for WT", CLI_GUEST},  
  {"resmtd",   "resmtd",  _Cmd_ImgResz_SetResampleMethod, NULL, "Set resample method", CLI_GUEST},
  {"setcoff",  "setcoff", _Cmd_ImgResz_SetResampleCoff,  NULL, "Set resample cofficient", CLI_GUEST},
  {"pertest",  "pertest",  _Cmd_ImgResz_TestPerformance, NULL, "Test imgresz performance", CLI_GUEST},

  //CLIMOD_DEBUG_CLIENTRY(Video Decoder),
  //Last CLI command record, NULL
  {NULL, NULL, NULL, NULL, NULL, CLI_GUEST}
};

CLI_EXEC_T _rImgReszCmdTbl =
{
  "imgresz",
  NULL,
  NULL,
  _arImgReszCmdTbl,
  "Image resizer command",
  CLI_GUEST
};

CLI_EXEC_T* GetImgReszCmdTbl(void)
{
  return &_rImgReszCmdTbl;
}



//===========================================================================
// Integrate WT Test
//===========================================================================
#define IMG_RESZ_CMD_INTEGRATE_WT_SRC_BUF_NUM 2
UINT32 _u4ImgReszCmdIntegrateSrcBufIdx = 0;

IMGRESZ_DRV_TICKET_T _rImgReszCmdIntegrateWTTicket;

UINT32 _u4ImgReszCmdIntegrateSrcBufWidth = 7680;
UINT32 _u4ImgReszCmdIntegrateSrcBufHeight = 1080;
UINT32 _u4ImgReszCmdIntegrateSrcPicWidth = 1920;
UINT32 _u4ImgReszCmdIntegrateSrcPicHeight = 1080;
UINT32 _u4ImgReszCmdIntegrateSrcPicXOff = 0;
UINT32 _u4ImgReszCmdIntegrateSrcPicYOff = 0;
UINT32 _au4ImgReszCmdIntegrateSrcBufY[IMG_RESZ_CMD_INTEGRATE_WT_SRC_BUF_NUM];

UINT32 _u4ImgReszCmdIntegrateDstBufWidth = 7680;
UINT32 _u4ImgReszCmdIntegrateDstBufHeight = 1080;
UINT32 _u4ImgReszCmdIntegrateDstPicWidth = 640;
UINT32 _u4ImgReszCmdIntegrateDstPicHeight = 370;
UINT32 _u4ImgReszCmdIntegrateDstPicXOff = 0;
UINT32 _u4ImgReszCmdIntegrateDstPicYOff = 700;
BOOL   _fgFirstScale = TRUE;

UINT32 _u4ImgReszCmdIntegrateDstBufWidth1 = 7680;
UINT32 _u4ImgReszCmdIntegrateDstBufHeight1 = 1080;
UINT32 _u4ImgReszCmdIntegrateDstPicWidth1 = 640;
UINT32 _u4ImgReszCmdIntegrateDstPicHeight1 = 360;
UINT32 _u4ImgReszCmdIntegrateDstPicXOff1 = 0;
UINT32 _u4ImgReszCmdIntegrateDstPicYOff1 = 700;

UINT32 _u4ImgReszCmdIntegrateDstBufWidth2 = 3840;
UINT32 _u4ImgReszCmdIntegrateDstBufHeight2 = 540;
UINT32 _u4ImgReszCmdIntegrateDstPicWidth2 = 320;
UINT32 _u4ImgReszCmdIntegrateDstPicHeight2 = 180;
UINT32 _u4ImgReszCmdIntegrateDstPicXOff2 = 0;
UINT32 _u4ImgReszCmdIntegrateDstPicYOff2 = 360;




BOOL _fgImgReszCmdIntegrateWTResizing = FALSE;

void vImgReszCmdIntegrateSetWTInfo(void)
{
    UTIL_Printf("[IMGRESZ_CMD] vImgReszCmdIntegrateSetWTInfo \n");
    _fgWTEnable = TRUE;
    _fg8550GraphIntegrationTest = TRUE;
    _fg8555GraphIntegrationTest = FALSE;
}


INT32 i4ImgReszCmdIntegrateWTInit(void)
{
    UINT32 u4I;

    UTIL_Printf("[IMGRESZ_CMD] i4ImgReszCmdIntegrateWTInit, _fg8550GraphIntegrationTest= %d, _fgWTEnable = %d, _fg8555GraphIntegrationTest = %d \n",_fg8550GraphIntegrationTest, _fgWTEnable,_fg8555GraphIntegrationTest);
    if (_fg8550GraphIntegrationTest)
    {
        _u4ImgReszCmdIntegrateDstPicHeight = 360;
        _u4ImgReszCmdIntegrateDstPicXOff = 0;
        _u4ImgReszCmdIntegrateDstPicYOff = 700;   
        if (_fgWTEnable)
        {
            _u4ImgReszCmdIntegrateDstBufWidth = 7680;
        }
        else
        {
            _u4ImgReszCmdIntegrateDstBufWidth = 3840;
        }
    }
    else if (_fg8555GraphIntegrationTest)
    {
        _u4ImgReszCmdIntegrateDstPicXOff = 0;
        if (_fgUse950X540Surface)
        {
            _u4ImgReszCmdIntegrateDstPicHeight = 180;
            _u4ImgReszCmdIntegrateDstPicYOff = 360;   
            _u4ImgReszCmdIntegrateDstBufWidth = 3840;
        }
        else
        {
            _u4ImgReszCmdIntegrateDstPicHeight = 360;
            _u4ImgReszCmdIntegrateDstPicYOff = 700; 
            _u4ImgReszCmdIntegrateDstBufWidth = 7680;
        }
    }
    // Allocate memory
    for(u4I=0; u4I<IMG_RESZ_CMD_INTEGRATE_WT_SRC_BUF_NUM; u4I++)
    {
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        _au4ImgReszCmdIntegrateSrcBufY[u4I] = (UINT32)x_alloc_aligned_dma_mem(
            _u4ImgReszCmdIntegrateSrcBufWidth*_u4ImgReszCmdIntegrateSrcBufHeight,32);
#elif CONFIG_SYS_MEM_PHASE3
        //_au4ImgReszCmdIntegrateSrcBufY[u4I] = (UINT32)x_mem_aligned_alloc(
        //    _u4ImgReszCmdIntegrateSrcBufWidth*_u4ImgReszCmdIntegrateSrcBufHeight,32);
        #if CONFIG_ENABLE_IOMMU
        _au4ImgReszCmdIntegrateSrcBufY[u4I] = (UINT32)x_mem_valloc(
             _u4ImgReszCmdIntegrateSrcBufWidth*_u4ImgReszCmdIntegrateSrcBufHeight);
        #else
        _au4ImgReszCmdIntegrateSrcBufY[u4I] = (UINT32)x_alloc_aligned_ch2_mem(
            _u4ImgReszCmdIntegrateSrcBufWidth*_u4ImgReszCmdIntegrateSrcBufHeight,32);
#endif
#endif
        #else
        _au4ImgReszCmdIntegrateSrcBufY[u4I] = (UINT32)x_alloc_aligned_nc_mem(
            _u4ImgReszCmdIntegrateSrcBufWidth*_u4ImgReszCmdIntegrateSrcBufHeight,16);
        #endif

        ASSERT(_au4ImgReszCmdIntegrateSrcBufY[u4I] > 0);
    }

    // Draw image 
    {
        UINT32 u4X,u4Y;
        UINT32 u4Addr;

        u4Addr = _au4ImgReszCmdIntegrateSrcBufY[0];

        if ((!_fg8550GraphIntegrationTest)&& (!_fg8555GraphIntegrationTest))
        {
        for(u4Y=0; u4Y<_u4ImgReszCmdIntegrateSrcPicHeight; u4Y++)
            for(u4X=0; u4X<_u4ImgReszCmdIntegrateSrcPicWidth; u4X++)
            {
                       
                *(UINT8 *)(u4Addr + 0) = u4X & 0xFF;            
                *(UINT8 *)(u4Addr + 1) = (u4X+u4Y) & 0xFF;            
                *(UINT8 *)(u4Addr + 2) = u4Y & 0xFF;                 
                *(UINT8 *)(u4Addr + 3) = 0xFF;
                u4Addr+=4;
            }
        }
        else
        {
            for(u4Y=0; u4Y<_u4ImgReszCmdIntegrateSrcPicHeight; u4Y++)
            for(u4X=0; u4X<_u4ImgReszCmdIntegrateSrcPicWidth; u4X++)
            {               
                if(((u4X>>6) & 1) == ((u4Y>>6) & 1))
                {
                    *(UINT8 *)(u4Addr + 0) = 0xFF;            
                    *(UINT8 *)(u4Addr + 1) = 0xFF;            
                    *(UINT8 *)(u4Addr + 2) = 0xFF;                
                } else
                {
                    *(UINT8 *)(u4Addr + 0) = 0x0;            
                    *(UINT8 *)(u4Addr + 1) = 0x0;            
                    *(UINT8 *)(u4Addr + 2) = 0x0;                                
                }
              
                *(UINT8 *)(u4Addr + 3) = 0xFF;
                u4Addr+=4;
            }   
        }
    }

    {
        UINT32 u4X,u4Y;
        UINT32 u4Addr = _au4ImgReszCmdIntegrateSrcBufY[1];

        if ((!_fg8550GraphIntegrationTest) && (!_fg8555GraphIntegrationTest))
        {
             for(u4Y=0; u4Y<_u4ImgReszCmdIntegrateSrcPicHeight; u4Y++)
            for(u4X=0; u4X<_u4ImgReszCmdIntegrateSrcPicWidth; u4X++)
            {
          
                *(UINT8 *)(u4Addr + 0) = u4X & 0xFF;            
                *(UINT8 *)(u4Addr + 1) = (u4X+u4Y) & 0xFF;            
                *(UINT8 *)(u4Addr + 2) = u4Y & 0xFF;                 
                *(UINT8 *)(u4Addr + 3) = 0xFF;
                u4Addr+=4;
            }
        }
        else
        {
            for(u4Y=0; u4Y<_u4ImgReszCmdIntegrateSrcPicHeight; u4Y++)
            for(u4X=0; u4X<_u4ImgReszCmdIntegrateSrcPicWidth; u4X++)
            {               
                if(((u4X>>6) & 1) == ((u4Y>>6) & 1))
                {
                    *(UINT8 *)(u4Addr + 0) = 0xFF;            
                    *(UINT8 *)(u4Addr + 1) = 0xFF;            
                    *(UINT8 *)(u4Addr + 2) = 0xFF;                
                } else
                {
                    *(UINT8 *)(u4Addr + 0) = 0x0;            
                    *(UINT8 *)(u4Addr + 1) = 0x0;            
                    *(UINT8 *)(u4Addr + 2) = 0x0;                                
                }
              
                *(UINT8 *)(u4Addr + 3) = 0xFF;
                u4Addr+=4;
            }   
        }

                 
    }

    return 0;
}


INT32 i4ImgReszCmdIntegrateWTUninit(void)
{
    UINT32 u4I;
    
    // Free memory
    for(u4I=0; u4I<IMG_RESZ_CMD_INTEGRATE_WT_SRC_BUF_NUM; u4I++)
    {
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY 
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void *)_au4ImgReszCmdIntegrateSrcBufY[u4I]);
#elif CONFIG_SYS_MEM_PHASE3
        //x_mem_aligned_free((void *)_au4ImgReszCmdIntegrateSrcBufY[u4I]);
        x_free_aligned_ch1_ch2_mem((void *)_au4ImgReszCmdIntegrateSrcBufY[u4I]);
#endif
    #else
        x_free_aligned_nc_mem((void *)_au4ImgReszCmdIntegrateSrcBufY[u4I]);
    #endif
    }

    return 0;
}


INT32 ImgReszCmdIntegrateWTNotifyCallback(INT32 i4State,void *pvPrivData)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &_rImgReszCmdIntegrateWTTicket;

    // Release ticket
    i4Ret = i4ImgResz_Drv_ReleaseTicket(prImgReszTicket);
    VERIFY(i4Ret >= 0);

    _u4ImgReszCmdIntegrateSrcBufIdx++;
    if(_u4ImgReszCmdIntegrateSrcBufIdx >= IMG_RESZ_CMD_INTEGRATE_WT_SRC_BUF_NUM)
       _u4ImgReszCmdIntegrateSrcBufIdx = 0; 

    _fgImgReszCmdIntegrateWTResizing = FALSE;
    
    return 0;
}

BOOL fgSaveGolden(void)
{
    UINT32 u4Temp = 0;
    UINT8 * pu1Src = NULL;
    UINT8 * pu1Dst = (UINT8 *)_u4Golden;
    UINT32 u4BufferWidth = 0;
    UINT32 u4BufHeight = 0;
    UINT32 u4YOffset = 0;
    UINT32 u4Height = 0;
    UINT32 u4Width = 0;
    

    if (_u4ImgReszCmdIntegrateSrcBufIdx == 0)
    {
        pu1Src = (UINT8 *)_u4WB[1];  
    }
    else
    {
        pu1Src = (UINT8 *)_u4WB[0];
    }

    if (_fg8550GraphIntegrationTest)
    {
        u4BufferWidth = 1920*2;
        u4BufHeight =  1080;
        u4YOffset = 700;
        u4Height = 360;
        u4Width = 640*2;
    }
    else if (_fg8555GraphIntegrationTest)
    {
        if (_fgUse950X540Surface)
        {
            u4BufferWidth = 960 *4;
            u4BufHeight = 540;
            u4YOffset = 360;
            u4Height = 180;
            u4Width = 320*4;
        }
        else
        {
            u4BufferWidth =  1920*4;
            u4BufHeight = 1080;
            u4YOffset = 700;
            u4Height = 360;
            u4Width =  640*4;
        }
    }
    pu1Src += u4BufferWidth * u4YOffset;

    for (u4Temp = 0; u4Temp < u4Height; u4Temp ++)
    {
        x_memcpy((void *)pu1Dst, (void *)pu1Src, u4Width);
        pu1Src += u4BufferWidth;
        pu1Dst += u4Width;
    }
    return TRUE;
}

BOOL fgCompareGolden(void)
{
    UINT32 u4x, u4y = 0;
    UINT8 * pu1Src = NULL;
    UINT8 * pu1Dst = (UINT8 *)_u4Golden;
    UINT32 u4BufferWidth = 0;
    UINT32 u4BufHeight = 0;
    UINT32 u4YOffset = 0;
    UINT32 u4Height = 0;
    UINT32 u4Width = 0;

    if (_u4ImgReszCmdIntegrateSrcBufIdx == 0)
    {
        pu1Src = (UINT8 *)_u4WB[1];  
    }
    else
    {
        pu1Src = (UINT8 *)_u4WB[0];
    }

    if (_fg8550GraphIntegrationTest)
    {
        u4BufferWidth = 1920*2;
        u4BufHeight =  1080;
        u4YOffset = 700;
        u4Height = 360;
        u4Width = 640*2;
    }
    else if (_fg8555GraphIntegrationTest)
    {
        if (_fgUse950X540Surface)
        {
            u4BufferWidth = 960 *4;
            u4BufHeight = 540;
            u4YOffset = 360;
            u4Height = 180;
            u4Width = 320*4;
        }
        else
        {
            u4BufferWidth =  1920*4;
            u4BufHeight = 1080;
            u4YOffset = 700;
            u4Height = 360;
            u4Width =  640*4;
        }
    }
    
    pu1Src += u4BufferWidth * u4YOffset;


    for (u4y = 0; u4y < u4Height; u4y ++)
    {
        for (u4x = 0; u4x < u4Width; u4x ++)
        {
            if (*pu1Src != *pu1Dst)
            {
                UTIL_Printf("[IMG_RESZ]Burn in test compare fail, x = %d, y = %d \n",u4x,u4y);
                //ASSERT(0);
                return FALSE;
            }
            pu1Src ++;
            pu1Dst ++;
        }
        pu1Src += (u4BufferWidth - u4Width);
    }
    return TRUE;
}

BOOL fgImgReszSaveWTGolden(UINT32 u4Src)
{
    UINT32 u4Temp = 0;
    UINT8 * pu1Src = NULL;
    UINT8 * pu1Dst = NULL;

#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    _u4Golden = (UINT32)x_alloc_aligned_dma_mem(640*360*2, 32);
#elif CONFIG_SYS_MEM_PHASE3
    _u4Golden = (UINT32)x_mem_aligned_alloc(640*360*2, 32);
#endif
#else
    _u4Golden = (UINT32)x_alloc_aligned_nc_mem(640*360*2, 16);
#endif

    if (_u4Golden == 0)
    {
        UTIL_Printf("[ImgResz + WT] Allocate golden memory fail \n");
        //ASSERT(0);
        return FALSE;
    }
    pu1Dst = (UINT8 *)_u4Golden;

    
    pu1Src = (UINT8 *)u4Src;
    
    pu1Src += 1920*2*700;

    for (u4Temp = 0; u4Temp < 360; u4Temp ++)
    {
        x_memcpy((void *)pu1Dst, (void *)pu1Src, 640*2);
        pu1Src += 1920 * 2;
        pu1Dst += 640*2;
    }
    return TRUE;
}

BOOL fgImgReszCompareWTGolden(UINT32 u4Src)
{
    UINT32 u4x, u4y = 0;
    UINT8 * pu1Src = NULL;
    UINT8 * pu1Dst = (UINT8 *)_u4Golden;

    
    pu1Src = (UINT8 *)u4Src;
    pu1Src += 1920*700*2;


    for (u4y = 0; u4y < 360; u4y ++)
    {
        for (u4x = 0; u4x < 640*2; u4x ++)
        {
            if (*pu1Src != *pu1Dst)
            {
                UTIL_Printf("[IMG_RESZ]Burn in test compare fail, x = %d, y = %d \n",u4x,u4y);
                //ASSERT(0);
                return FALSE;
            }
            pu1Src ++;
            pu1Dst ++;
        }
        pu1Src += ((1920 - 640)*2);
    }
    return TRUE;
}


BOOL fgImgReszCmdIntegrateWTResizeDone(void)
{
    return !_fgImgReszCmdIntegrateWTResizing;
}


INT32 i4ImgReszCmdIntegrateWTDoResize(UINT32 u4TgtBufAddr)
{
    INT32 i4Ret;
    IMGRESZ_DRV_TICKET_T *prImgReszTicket = &_rImgReszCmdIntegrateWTTicket;
    IMGRESZ_DRV_SRC_BUF_INFO_T rSrcBufInfo;
    IMGRESZ_DRV_DST_BUF_INFO_T rDstBufInfo;
    IMGRESZ_DRV_NOTIFY_CB_REG_T rNotifyCallbackReg;
    IMGRESZ_DRV_SRC_COLOR_MODE eSrcColorMode = IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888;
    IMGRESZ_DRV_DST_COLOR_MODE eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;

    UINT32 u4SrcBufWidth = _u4ImgReszCmdIntegrateSrcBufWidth;
    UINT32 u4SrcBufHeight = _u4ImgReszCmdIntegrateSrcBufHeight;
    UINT32 u4SrcPicWidth = _u4ImgReszCmdIntegrateSrcPicWidth;
    UINT32 u4SrcPicHeight = _u4ImgReszCmdIntegrateSrcPicHeight;
    UINT32 u4SrcPicXOff = _u4ImgReszCmdIntegrateSrcPicXOff;
    UINT32 u4SrcPicYOff = _u4ImgReszCmdIntegrateSrcPicYOff;
    UINT32 u4SrcYBufAddr = _au4ImgReszCmdIntegrateSrcBufY[_u4ImgReszCmdIntegrateSrcBufIdx];

    UINT32 u4DstBufWidth = _u4ImgReszCmdIntegrateDstBufWidth;
    UINT32 u4DstBufHeight = _u4ImgReszCmdIntegrateDstBufHeight;
    UINT32 u4DstPicWidth = _u4ImgReszCmdIntegrateDstPicWidth;
    UINT32 u4DstPicHeight = _u4ImgReszCmdIntegrateDstPicHeight;
    UINT32 u4DstPicXOff = _u4ImgReszCmdIntegrateDstPicXOff;
    UINT32 u4DstPicYOff = _u4ImgReszCmdIntegrateDstPicYOff;
    UINT32 u4DstYBufAddr = u4TgtBufAddr;


    if(_fgImgReszCmdIntegrateWTResizing)
        ASSERT(FALSE);

    if ((_fg8550GraphIntegrationTest) && (!_fgWTEnable))
    {
        eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_4444;
    }

    if (_fg8555GraphIntegrationTest)
    {
        eDstColorMode = IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888;
        if (_fgUse950X540Surface)
        {
            u4DstBufWidth = _u4ImgReszCmdIntegrateDstBufWidth2;
            u4DstBufHeight = _u4ImgReszCmdIntegrateDstBufHeight2;
            u4DstPicWidth = _u4ImgReszCmdIntegrateDstPicWidth2;
            u4DstPicHeight = _u4ImgReszCmdIntegrateDstPicHeight2;
            u4DstPicXOff = _u4ImgReszCmdIntegrateDstPicXOff2;
            u4DstPicYOff = _u4ImgReszCmdIntegrateDstPicYOff2;
        }
        else
        {
            u4DstBufWidth = _u4ImgReszCmdIntegrateDstBufWidth1;
            u4DstBufHeight = _u4ImgReszCmdIntegrateDstBufHeight1;
            u4DstPicWidth = _u4ImgReszCmdIntegrateDstPicWidth1;
            u4DstPicHeight = _u4ImgReszCmdIntegrateDstPicHeight1;
            u4DstPicXOff = _u4ImgReszCmdIntegrateDstPicXOff1;
            u4DstPicYOff = _u4ImgReszCmdIntegrateDstPicYOff1;
        }
    }

   
    _fgImgReszCmdIntegrateWTResizing = TRUE;


    // Get ticket
    i4Ret = i4ImgResz_Drv_GetTicket(prImgReszTicket);
    VERIFY(i4Ret >= 0);

    // Set priority
    i4Ret = i4ImgResz_Drv_SetPriority(prImgReszTicket,IMGRESZ_DRV_PRIORITY_LOW);
    VERIFY(i4Ret >= 0);

    // Set scale mode
    i4Ret = i4ImgResz_Drv_SetScaleMode(prImgReszTicket,IMGRESZ_DRV_FRAME_SCALE);
    VERIFY(i4Ret >= 0);

    // Set source buffer info
    x_memset(&rSrcBufInfo,0,sizeof(IMGRESZ_DRV_SRC_BUF_INFO_T));
    rSrcBufInfo.eSrcColorMode = eSrcColorMode;
    rSrcBufInfo.u4BufWidth = u4SrcBufWidth;
    rSrcBufInfo.u4BufHeight = u4SrcBufHeight;
    rSrcBufInfo.u4PicWidth = u4SrcPicWidth;
    rSrcBufInfo.u4PicHeight = u4SrcPicHeight;
    rSrcBufInfo.u4PicPosX = u4SrcPicXOff;
    rSrcBufInfo.u4PicPosY = u4SrcPicYOff;
    rSrcBufInfo.u4YBufAddr = u4SrcYBufAddr;
    rSrcBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetSrcBufInfo(prImgReszTicket,&rSrcBufInfo);
    VERIFY(i4Ret >= 0);

    // Set target buffer info
    x_memset(&rDstBufInfo,0,sizeof(IMGRESZ_DRV_DST_BUF_INFO_T));
    rDstBufInfo.eDstColorMode = eDstColorMode;
    rDstBufInfo.u4BufWidth = u4DstBufWidth;
    rDstBufInfo.u4BufHeight = u4DstBufHeight;
    rDstBufInfo.u4PicWidth = u4DstPicWidth;
    rDstBufInfo.u4PicHeight = u4DstPicHeight;
    rDstBufInfo.u4PicPosX = u4DstPicXOff;
    rDstBufInfo.u4PicPosY = u4DstPicYOff;
    rDstBufInfo.u4YBufAddr = u4DstYBufAddr;
    if (_fgWTEnable)
        rDstBufInfo.fgWTEnable = TRUE;
    else
        rDstBufInfo.fgWTEnable = FALSE;
    i4Ret = i4ImgResz_Drv_SetDstBufInfo(prImgReszTicket,&rDstBufInfo);
    VERIFY(i4Ret >= 0);

    // Register notify callback function
    rNotifyCallbackReg.pvCallBackFunc = ImgReszCmdIntegrateWTNotifyCallback;
    rNotifyCallbackReg.pvPrivData = NULL;
    i4Ret = i4ImgResz_Drv_RegFinishNotifyCallback(prImgReszTicket,&rNotifyCallbackReg);
    VERIFY(i4Ret >= 0);

    // Do scale
    i4Ret = i4ImgResz_Drv_DoScale(prImgReszTicket,NULL);
    VERIFY(i4Ret >= 0);

    if ((_fg8550GraphIntegrationTest) || (_fg8555GraphIntegrationTest))
    {
        while (TRUE)
        {
            if (fgImgReszCmdIntegrateWTResizeDone())
            {
                break;
            }
            x_thread_delay(5);
        }
    }

    return 0;
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
extern void vGfxFlipOSD(UINT32 u4RgnList, UINT32 u4Plane);
extern INT32 OSD_PLA_Enable(UINT32 u4Plane, BOOL fgEnable);
#endif

void vImgreszBurnThread(void *arg)
{
//    BOOL    fgGfxBreak ;
    UINT8 *pu4SrcOsdMem ;
    UINT8   *pu4DstOsdMem;
    UINT8 *pu4SrcOsdMem1 ;
    UINT8   *pu4DstOsdMem1;
    //UINT32  *pu4TempOsdMem = (UINT32 *)x_mem_ch2_alloc(1920*1080*4+15);
    //	     pu4TempOsdMem = (UINT32 *)((UINT32)pu4TempOsdMem&(~0xf));
    //UINT32 i4Round1=0;

    UINT32 u4RgnList[2], u4Rgn[2];
    //i4Round = 1;
//    fgGfxBreak = FALSE;
#if 0
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_dma_mem(OSD2_SURFACE_SIZE, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_dma_mem (OSD2_SURFACE_SIZE,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_dma_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_nc_mem(OSD2_SURFACE_SIZE, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_nc_mem (OSD2_SURFACE_SIZE,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_nc_mem(640*360*2,16);
#endif
#else
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SIZE, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SIZE,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SIZE, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SIZE,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*2,16);
#endif

#endif
    if ((pu4SrcOsdMem1 == NULL) ||(pu4SrcOsdMem == NULL) ||
        (pu4DstOsdMem1 == NULL) || (pu4DstOsdMem == NULL))
    {
        UTIL_Printf("[IMGRESZ] Allocate memory fail in burn in test \n");
        return ;
    }
     
    //i4GfxGetCmdBuf(&u4Handle,GFX_DRV_CMDBUF_PRIORITY_HIGH);
   
    x_memset((void *)pu4SrcOsdMem, 0, OSD2_SURFACE_SIZE);
	x_memset((void *)pu4DstOsdMem,0, OSD2_SURFACE_SIZE);
	//x_memset((void *)pu4TempOsdMem,0, (1920*1080*4));
	
     vImgreszDrawLine(pu4SrcOsdMem);
     vImgreszDrawLine(pu4DstOsdMem);
	
	//set src end
     _u4WB[0] = (UINT32)pu4SrcOsdMem;
	 _u4WB[1] = (UINT32)pu4DstOsdMem;
    //init OSD
 	x_thread_delay(1000);  

     vGfxShowOSD(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1],1);
    OSD_PLA_Enable(2, TRUE);
    
    i4ImgReszCmdIntegrateWTInit();
    //start the VG draw loop
    while(TRUE)
    {
       i4ImgReszCmdIntegrateWTDoResize(_u4WB[_u4ImgReszCmdIntegrateSrcBufIdx]);
       if (_fgFirstScale)
       {
           fgSaveGolden();
           _fgFirstScale = FALSE;
       }
       else
       {
           fgCompareGolden();
       }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)       
	vGfxFlipOSD(u4RgnList[_u4ImgReszCmdIntegrateSrcBufIdx], 2);       
#endif
	x_thread_delay(1000);
	}
    // x_thread_delay(10000);
    // x_mem_free((void*)pu4ScrOsdMem1);
	// x_mem_free((void*)pu4DstOsdMem1);
	 //x_mem_free((void*)pu4TempOsdMem);
	 //i4GfxReleaseCmdBuf(u4Handle);
}

extern void vGfxShowOSD_Ex(
    UINT32 u4OsdMem1, UINT32 u4OsdMem2,
    UINT32 *pu4RgnList1, UINT32 *pu4RgnList2,
    UINT32 *pu4Rgn1, UINT32 *pu4Rgn2,
    UINT32  ui4_width, UINT32 ui4_height,
    UINT32  ui4_clrMode,
    UINT32  u4Plane);
extern void vGfxIntShowOSD(UINT32 u4OsdMem1, UINT32 u4OsdMem2,
                    UINT32 *pu4RgnList1, UINT32 *pu4RgnList2,
                    UINT32 *pu4Rgn1, UINT32 *pu4Rgn2, UINT32 u4Plane, BOOL fgWt);


void vImgreszBurnThread1(void *arg)
{
//    BOOL    fgGfxBreak ;
    UINT8 *pu4SrcOsdMem ;
    UINT8   *pu4DstOsdMem;
    UINT8 *pu4SrcOsdMem1 ;
    UINT8   *pu4DstOsdMem1;
    //UINT32  *pu4TempOsdMem = (UINT32 *)x_mem_ch2_alloc(1920*1080*4+15);
    //	     pu4TempOsdMem = (UINT32 *)((UINT32)pu4TempOsdMem&(~0xf));
    //UINT32 i4Round1=0;

    UINT32 u4RgnList[2], u4Rgn[2];
    //i4Round = 1;
//    fgGfxBreak = FALSE;
#if 0
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_dma_mem(OSD2_SURFACE_SIZE, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_dma_mem (OSD2_SURFACE_SIZE,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_dma_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_nc_mem(OSD2_SURFACE_SIZE, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_nc_mem (OSD2_SURFACE_SIZE,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_nc_mem(640*360*2,16);
#endif
#else
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch1_mem(OSD2_SURFACE_SZIE1, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch1_mem (OSD2_SURFACE_SZIE1,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch1_mem(640*360*4, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch1_mem(OSD2_SURFACE_SZIE1, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch1_mem (OSD2_SURFACE_SZIE1,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch1_mem(640*360*4,16);
#endif

#endif
    if ((pu4SrcOsdMem1 == NULL) ||(pu4SrcOsdMem == NULL) ||
        (pu4DstOsdMem1 == NULL) || (pu4DstOsdMem == NULL))
    {
        UTIL_Printf("[IMGRESZ] Allocate memory fail in burn in test \n");
        return ;
    }
     
    //i4GfxGetCmdBuf(&u4Handle,GFX_DRV_CMDBUF_PRIORITY_HIGH);
   
    x_memset((void *)pu4SrcOsdMem, 0, OSD2_SURFACE_SZIE1);
	x_memset((void *)pu4DstOsdMem,0, OSD2_SURFACE_SZIE1);
	//x_memset((void *)pu4TempOsdMem,0, (1920*1080*4));
	
     vImgreszDrawLine1(pu4SrcOsdMem);
     vImgreszDrawLine1(pu4DstOsdMem);
	
	//set src end
     _u4WB[0] = (UINT32)pu4SrcOsdMem;
	 _u4WB[1] = (UINT32)pu4DstOsdMem;
    //init OSD
 	x_thread_delay(1000);  

    //vGfxShowOSD(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1],1);
    //OSD_PLA_Enable(2, TRUE);
    vGfxShowOSD_Ex(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1], 1920, 1080, OSD_CM_ARGB8888_DIRECT32, 2);
    
    i4ImgReszCmdIntegrateWTInit();
    //start the VG draw loop
    while(TRUE)
    {
       i4ImgReszCmdIntegrateWTDoResize(_u4WB[_u4ImgReszCmdIntegrateSrcBufIdx]);
       if (_fgFirstScale)
       {
           fgSaveGolden();
           _fgFirstScale = FALSE;
       }
       else
       {
           fgCompareGolden();
       }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)       
	vGfxFlipOSD(u4RgnList[_u4ImgReszCmdIntegrateSrcBufIdx], 2);       
#endif
	x_thread_delay(200);
	}
    // x_thread_delay(10000);
    // x_mem_free((void*)pu4ScrOsdMem1);
	// x_mem_free((void*)pu4DstOsdMem1);
	 //x_mem_free((void*)pu4TempOsdMem);
	 //i4GfxReleaseCmdBuf(u4Handle);
}


void vImgreszBurnThread540(void *arg)
{
//    BOOL    fgGfxBreak ;
    UINT8 *pu4SrcOsdMem ;
    UINT8   *pu4DstOsdMem;
    UINT8 *pu4SrcOsdMem1 ;
    UINT8   *pu4DstOsdMem1;
    //UINT32  *pu4TempOsdMem = (UINT32 *)x_mem_ch2_alloc(1920*1080*4+15);
    //	     pu4TempOsdMem = (UINT32 *)((UINT32)pu4TempOsdMem&(~0xf));
    //UINT32 i4Round1=0;

    UINT32 u4RgnList[2], u4Rgn[2];
    //i4Round = 1;
//    fgGfxBreak = FALSE;
#if 0
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_dma_mem(OSD2_SURFACE_SIZE, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_dma_mem (OSD2_SURFACE_SIZE,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_dma_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_nc_mem(OSD2_SURFACE_SIZE, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_nc_mem (OSD2_SURFACE_SIZE,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_nc_mem(640*360*2,16);
#endif
#else
#if CONFIG_DRV_3D_384_SUPPORT
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SIZE2, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SIZE2,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SIZE2, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SIZE2,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*2,16);
#endif
#else
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch1_mem(OSD2_SURFACE_SIZE2, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch1_mem (OSD2_SURFACE_SIZE2,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch1_mem(640*360*2, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch1_mem(OSD2_SURFACE_SIZE2, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch1_mem (OSD2_SURFACE_SIZE2,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch1_mem(640*360*2,16);
#endif
#endif

#endif
    if ((pu4SrcOsdMem1 == NULL) ||(pu4SrcOsdMem == NULL) ||
        (pu4DstOsdMem1 == NULL) || (pu4DstOsdMem == NULL))
    {
        UTIL_Printf("[IMGRESZ] Allocate memory fail in burn in test \n");
        return ;
    }
     
    //i4GfxGetCmdBuf(&u4Handle,GFX_DRV_CMDBUF_PRIORITY_HIGH);
   
    x_memset((void *)pu4SrcOsdMem, 0, OSD2_SURFACE_SIZE2);
	x_memset((void *)pu4DstOsdMem,0, OSD2_SURFACE_SIZE2);
	//x_memset((void *)pu4TempOsdMem,0, (1920*1080*4));
	
     vImgreszDrawLine2(pu4SrcOsdMem);
     vImgreszDrawLine2(pu4DstOsdMem);
	
	//set src end
     _u4WB[0] = (UINT32)pu4SrcOsdMem;
	 _u4WB[1] = (UINT32)pu4DstOsdMem;
    //init OSD
 	x_thread_delay(1000);  

    //vGfxShowOSD(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1],1);
    //OSD_PLA_Enable(2, TRUE);
    vGfxShowOSD_Ex(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1],
        960, 540, OSD_CM_ARGB8888_DIRECT32, 2);
    
    i4ImgReszCmdIntegrateWTInit();
    //start the VG draw loop
    while(TRUE)
    {
       i4ImgReszCmdIntegrateWTDoResize(_u4WB[_u4ImgReszCmdIntegrateSrcBufIdx]);
       if (_fgFirstScale)
       {
           fgSaveGolden();
           _fgFirstScale = FALSE;
       }
       else
       {
           fgCompareGolden();
       }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)       
	vGfxFlipOSD(u4RgnList[_u4ImgReszCmdIntegrateSrcBufIdx], 2);       
#endif
	x_thread_delay(1000);
	}
    // x_thread_delay(10000);
    // x_mem_free((void*)pu4ScrOsdMem1);
	// x_mem_free((void*)pu4DstOsdMem1);
	 //x_mem_free((void*)pu4TempOsdMem);
	 //i4GfxReleaseCmdBuf(u4Handle);
}

void vImgreszWtBurnThread(void *arg)
{
    UINT8 *pu4SrcOsdMem ;
    UINT8   *pu4DstOsdMem;
    UINT8 *pu4SrcOsdMem1 ;
    UINT8   *pu4DstOsdMem1;
    UINT32 u4RgnList[2], u4Rgn[2];

#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SIZE, 32);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SIZE,32);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*4, 32);

#else
    pu4SrcOsdMem1 = (UINT8 *)x_alloc_aligned_ch2_mem(OSD2_SURFACE_SZIE1, 16);
    pu4SrcOsdMem = (UINT8  *)((UINT32)pu4SrcOsdMem1&(~0xf));
    pu4DstOsdMem1 = (UINT8  *)x_alloc_aligned_ch2_mem (OSD2_SURFACE_SZIE1,16);
	pu4DstOsdMem = (UINT8  *)((UINT32)pu4DstOsdMem1&(~0xf));
    _u4Golden = (UINT32)x_alloc_aligned_ch2_mem(640*360*4,16);
#endif

    if ((pu4SrcOsdMem1 == NULL) ||(pu4SrcOsdMem == NULL) ||
        (pu4DstOsdMem1 == NULL) || (pu4DstOsdMem == NULL))
    {
        UTIL_Printf("[IMGRESZ] Allocate memory fail in burn in test \n");
        return ;
    }
     
    //i4GfxGetCmdBuf(&u4Handle,GFX_DRV_CMDBUF_PRIORITY_HIGH);
   
    x_memset((void *)pu4SrcOsdMem, 0, OSD2_SURFACE_SIZE);
	x_memset((void *)pu4DstOsdMem,0, OSD2_SURFACE_SIZE);
	//x_memset((void *)pu4TempOsdMem,0, (1920*1080*4));
	
     //vImgreszDrawLine1(pu4SrcOsdMem);
     //vImgreszDrawLine1(pu4DstOsdMem);
	
	//set src end
     _u4WB[0] = (UINT32)pu4SrcOsdMem;
	 _u4WB[1] = (UINT32)pu4DstOsdMem;
    //init OSD
 	x_thread_delay(1000);  

    //vGfxShowOSD(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1],1);
    //OSD_PLA_Enable(2, TRUE);
    /*vGfxShowOSD_Ex*/vGfxIntShowOSD(_u4WB[0], _u4WB[1], &u4RgnList[0], &u4RgnList[1], &u4Rgn[0], &u4Rgn[1], 2, TRUE);

    vImgReszCmdIntegrateSetWTInfo();
    i4ImgReszCmdIntegrateWTInit();
    
    //start the VG draw loop
    while(TRUE)
    {
        UINT32 u4Temp = _u4WB[_u4ImgReszCmdIntegrateSrcBufIdx];

        
       i4ImgReszCmdIntegrateWTDoResize(_u4WB[_u4ImgReszCmdIntegrateSrcBufIdx]);
       if (_fgFirstScale)
       {
           fgImgReszSaveWTGolden(u4Temp);
           _fgFirstScale = FALSE;
       }
       else
       {
           fgImgReszCompareWTGolden(u4Temp);
       }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)       
	vGfxFlipOSD(u4RgnList[_u4ImgReszCmdIntegrateSrcBufIdx], 2);       
#endif
	x_thread_delay(1000);
	}
    // x_thread_delay(10000);
    // x_mem_free((void*)pu4ScrOsdMem1);
	// x_mem_free((void*)pu4DstOsdMem1);
	 //x_mem_free((void*)pu4TempOsdMem);
	 //i4GfxReleaseCmdBuf(u4Handle);
}

#if CONFIG_FASTBOOT_MULTI_PHASE_INIT_DRIVER_EN
EXPORT_SYMBOL(vImgReszCmdIntegrateSetWTInfo);
EXPORT_SYMBOL(fgImgReszSaveWTGolden);
EXPORT_SYMBOL(GetImgReszCmdTbl);
EXPORT_SYMBOL(fgImgReszCompareWTGolden);
EXPORT_SYMBOL(i4ImgReszCmdIntegrateWTDoResize);
EXPORT_SYMBOL(i4ImgReszCmdIntegrateWTInit);
#endif


