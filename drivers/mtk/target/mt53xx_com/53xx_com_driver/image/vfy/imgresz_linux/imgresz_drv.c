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
#include "x_rtos.h"
#include "imgresz_drv_inst.h"
#include "imgresz_drv.h"
#include "imgresz_hal_if.h"
#include "imgresz_hal.h"
#include "drv_imgresz_errcode.h"
#include "drv_thread.h"
#include "drv_config.h"
#include "x_hal_1176.h"

#include "sys_config.h"
#include "pm.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
HANDLE_T    h_imgresz_thread_0 = NULL_HANDLE;
HANDLE_T    h_imgresz_thread_1 = NULL_HANDLE;

IMGRESZ_HW_INST_T   _arImgreszHwInst[IMGRESZ_HW_INST_NUM];

BOOL        _fgImgreszInit = FALSE;

IMGRESZ_HAL_RESAMPLE_METHOD_T _eHResampleMethod = IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR;
IMGRESZ_HAL_RESAMPLE_METHOD_T _eVResampleMethod = IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR;


#if CONFIG_SUSPEND_TO_DRAM
int i4ImgResz_Suspend(void *param);
int i4ImgResz_Resume(void *param);
struct pm_operations imgresz_pm_ops = {
  .suspend = i4ImgResz_Suspend,
  .resume = i4ImgResz_Resume,
};
#endif


void vImgreszCopyMemory(void *pvDestination,void *pvSource,SIZE_T Length)
{
    x_memcpy(pvDestination,pvSource,Length);
}


void vImgreszSetMemory(void *pvDestination,UINT8 u1Value,SIZE_T Length)
{
    x_memset(pvDestination,0,Length);
}


INT32 ImgReszHwInstNotifyCallback(INT32 i4State,void *pvPrivData)
{
    INT32 i4Ret;

    i4Ret = x_ev_group_set_event(_arImgreszHwInst[(UINT32)pvPrivData].hEventHandle, IMGRESZ_EV_FINISH_SCALE, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);

    return 0;
}

extern void vHwImgReszWaitCountReset(UINT32 u4HwId);

void vImgReszHwInstResz(UINT32 u4HwId,IMGRESZ_INST_T *prImgReszInst)
{
    IMGRESZ_HAL_IMG_INFO_T rSrcImgInfo;
    IMGRESZ_HAL_IMG_INFO_T rDestImgInfo;
    IMGRESZ_HAL_IMG_INFO_T rBldImgInfo;
    IMGRESZ_HAL_NOTIFY_CB_REG_T rNofifyCallback;
    BOOL fgPartialMode = (prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE)
                      || (prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE);
    BOOL fgInterlaced = _arImgreszHwInst[u4HwId].fgInterlaced;
    IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod = _eHResampleMethod;//IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR;
    IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod = _eVResampleMethod;//IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR;

#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
    if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
        fgPartialMode = FALSE;
#endif

#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if(!fgPartialMode || prImgReszInst->ptImgReszPartialBufInfo->fgFirstRow)
#else
    if(!fgPartialMode || prImgReszInst->tImgReszPartialBufInfo.fgFirstRow)
#endif        
    {
        // Initialization
    i4ImgResz_HAL_Set_IOMMUTable(u4HwId,prImgReszInst->fgUserMMUTable);
    i4ImgResz_HAL_Init(u4HwId);
        // Set resize mode
    if(fgPartialMode)
        i4ImgResz_HAL_Set_Resize_Mode(u4HwId,IMGRESZ_HAL_RESIZE_MODE_PARTIAL);
    else
        i4ImgResz_HAL_Set_Resize_Mode(u4HwId,IMGRESZ_HAL_RESIZE_MODE_FRAME);
        // Set resize method      
    if (prImgReszInst->fgLumaKeyEnable)
    {
        eVResampleMethod = IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP;
    }   
    i4ImgResz_HAL_Set_Resample_Method(u4HwId,eHResampleMethod,
                                             eVResampleMethod);

        // Set luma key
    i4ImgResz_HAL_Set_LumaKey(u4HwId, prImgReszInst->fgLumaKeyEnable, prImgReszInst->u1LumaKey);

    // Set 1:1 scaling
    if (prImgReszInst->fg1To1Scale)
    {
        i4ImgResz_HAL_Set_Scale1to1(u4HwId, prImgReszInst->fg1To1Scale);
    }

    // Set 1/4 scaling , Y component only
    if (prImgReszInst->fgYSrcOnly)
    {
        i4ImgResz_HAL_Set_Scale4to1(u4HwId, prImgReszInst->fgYSrcOnly);
    }
    
        // Set source buffer info
    x_memset(&rSrcImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
        rSrcImgInfo.rBufferFormat.fgBlockMode = FALSE;
        rSrcImgInfo.rBufferFormat.fgAddrSwap = FALSE;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    switch(prImgReszInst->ptImgReszSrcBufInfo->eSrcColorMode)
#else
    switch(prImgReszInst->tImgReszSrcBufInfo.eSrcColorMode)
#endif        
    {
        case IMGRESZ_DRV_INPUT_COL_MD_ARGB_8888:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_ARGB_4444:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_ARGB_1555:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_1555;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_RGB_565:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_0565;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_8BPP_IDX:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER;
            rSrcImgInfo.rBufferFormat.eIndexBufferFormat = IMGRESZ_HAL_INDEX_BUFFER_FORMAT_8BPP;
            rSrcImgInfo.rBufferFormat.prColorPallet = (IMGRESZ_HAL_ARGB_COLOR_T *)prImgReszInst->abColorPallet;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_4BPP_IDX:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER;
            rSrcImgInfo.rBufferFormat.eIndexBufferFormat = IMGRESZ_HAL_INDEX_BUFFER_FORMAT_4BPP;
            rSrcImgInfo.rBufferFormat.prColorPallet = (IMGRESZ_HAL_ARGB_COLOR_T *)prImgReszInst->abColorPallet;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_2BPP_IDX:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER;
            rSrcImgInfo.rBufferFormat.eIndexBufferFormat = IMGRESZ_HAL_INDEX_BUFFER_FORMAT_2BPP;
            rSrcImgInfo.rBufferFormat.prColorPallet = (IMGRESZ_HAL_ARGB_COLOR_T *)prImgReszInst->abColorPallet;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_AYUV:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER;
            rSrcImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
            break;
        case IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF:
            rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[0] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[1] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorH;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[2] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorH;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorV;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] = prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorV;
            #else
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[0] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[1] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorH;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[2] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorH;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorV;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] = prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorV;
            #endif
            break;
            case IMGRESZ_DRV_INPUT_COL_MD_420_BLK:
                rSrcImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
                rSrcImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
                rSrcImgInfo.rBufferFormat.fgBlockMode = TRUE;
                rSrcImgInfo.rBufferFormat.fgAddrSwap = TRUE;
                break;
        default:
            break;
    }
    rSrcImgInfo.rBufferFormat.fgProgressiveFrame = TRUE;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rSrcImgInfo.u4BufWidth = prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
    rSrcImgInfo.u4BufHeight = prImgReszInst->ptImgReszSrcBufInfo->u4BufHeight;
    rSrcImgInfo.u4ImgWidth = prImgReszInst->ptImgReszSrcBufInfo->u4PicWidth;
#else
    rSrcImgInfo.u4BufWidth = prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
    rSrcImgInfo.u4BufHeight = prImgReszInst->tImgReszSrcBufInfo.u4BufHeight;
    rSrcImgInfo.u4ImgWidth = prImgReszInst->tImgReszSrcBufInfo.u4PicWidth;
#endif    
#if IMGRESZ_HW_SOURCE_WIDTH_2_LIMIT
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if((prImgReszInst->ptImgReszSrcBufInfo->u4PicWidth == 1) &&
       !(prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) &&
       (prImgReszInst->ptImgReszSrcBufInfo->u4PicPosX == 0) &&
       (prImgReszInst->ptImgReszSrcBufInfo->u4PicPosY == 0))
#else
    if((prImgReszInst->tImgReszSrcBufInfo.u4PicWidth == 1) &&
       !(prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) &&
       (prImgReszInst->tImgReszSrcBufInfo.u4PicPosX == 0) &&
       (prImgReszInst->tImgReszSrcBufInfo.u4PicPosY == 0))
#endif       
    {
        rSrcImgInfo.u4ImgWidth = 4;//2;
    }
#endif
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rSrcImgInfo.u4ImgHeight = prImgReszInst->ptImgReszSrcBufInfo->u4PicHeight;
#else
    rSrcImgInfo.u4ImgHeight = prImgReszInst->tImgReszSrcBufInfo.u4PicHeight;
#endif
#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if(prImgReszInst->ptImgReszSrcBufInfo->u4PicHeight == 1)
    #else
    if(prImgReszInst->tImgReszSrcBufInfo.u4PicHeight == 1)
    #endif
    {
        UINT32 u4AddrOffset = 0;

        rSrcImgInfo.u4ImgHeight = 2;

        _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = 0;
        _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = 0;
        _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = 0;

		// Duplicate 1st line to 2nd line
        switch(rSrcImgInfo.rBufferFormat.eBufferMainFormat)
        {
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
                // This case is more complicated, remember to implement.            
                break;
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
            #if CONFIG_SYS_MEM_PHASE2
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
            #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
            #endif
            #elif CONFIG_SYS_MEM_PHASE3
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
            #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
            #endif
            #endif
                #else
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,16);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,16);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,16);
            #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,16);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,16);
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,16);
            #endif    
                #endif

                if(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 != 0)
                {
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1+prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    #else
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1+prImgReszInst->tImgReszSrcBufInfo.u4BufWidth),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    #endif
                }
                if(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2 != 0)
                {
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4CbBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2+prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4CbBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    #else
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4CbBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2+prImgReszInst->tImgReszSrcBufInfo.u4BufWidth),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4CbBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    #endif
                }
                if(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3 != 0)
                {
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4CrBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3+prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth),
                             (void *)prImgReszInst->ptImgReszSrcBufInfo->u4CrBufAddr,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    #else
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4CrBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3+prImgReszInst->tImgReszSrcBufInfo.u4BufWidth),
                             (void *)prImgReszInst->tImgReszSrcBufInfo.u4CrBufAddr,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    #endif
                }
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                    // flush cache memory
                    //HalFlushInvalidateDCache();
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1, 
                                      prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2, TO_DEVICE);
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2, 
                                      prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2, TO_DEVICE);
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3,
                                      prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1,
                                         prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2);
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2,
                                         prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2);
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3,
                                         prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2);
                    #endif
                    #else
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1, 
                                      prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2, TO_DEVICE);
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2,
                                      prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2, TO_DEVICE);
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3, 
                                      prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1,
                                         prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2);
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2,
                                         prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2);
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3,
                                         prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2);
                    #endif
                    #endif
                #endif
                break;
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                #if CONFIG_SYS_MEM_PHASE2
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                #endif
                #elif CONFIG_SYS_MEM_PHASE3
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,32);
                #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,32);
                #endif
                #endif
                #else
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,16);
                #else
                _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2,16);
                #endif
                #endif
 
                if(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1 != 0)
                {
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    u4AddrOffset = prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * prImgReszInst->ptImgReszSrcBufInfo->u4PicPosY;

                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1),
                             (void *)(prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr+u4AddrOffset),prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1+prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth),
                             (void *)(prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr+u4AddrOffset),prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth);
                    #else
                    u4AddrOffset = prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * prImgReszInst->tImgReszSrcBufInfo.u4PicPosY;

                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1),
                             (void *)(prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr+u4AddrOffset),prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    x_memcpy((void *)(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1+prImgReszInst->tImgReszSrcBufInfo.u4BufWidth),
                             (void *)(prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr+u4AddrOffset),prImgReszInst->tImgReszSrcBufInfo.u4BufWidth);
                    #endif
                }
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                    // flush cache memory
                    //HalFlushInvalidateDCache();
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1, 
                                      prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1,
                                         prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * 2);
                    #endif
                    #else
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1,
                                      prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(_arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1,
                                         prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * 2);
                    #endif
                    #endif
                #endif
                break;
            default:
                break;
        }
    }
#endif
#if IMGRESZ_HW_SOURCE_WIDTH_2_LIMIT
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if((prImgReszInst->ptImgReszSrcBufInfo->u4PicWidth == 1) &&
       !(prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) &&
       (prImgReszInst->ptImgReszSrcBufInfo->u4PicPosX == 0) &&
       (prImgReszInst->ptImgReszSrcBufInfo->u4PicPosY == 0))
    #else
    if((prImgReszInst->tImgReszSrcBufInfo.u4PicWidth == 1) &&
       !(prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) &&
       (prImgReszInst->tImgReszSrcBufInfo.u4PicPosX == 0) &&
       (prImgReszInst->tImgReszSrcBufInfo.u4PicPosY == 0))
    #endif
   {
        UINT32 u4I;
        UINT32 u4Height,u4Height2;
        UINT32 u4Src,u4Src1,u4Src2,u4Src3;    

        if(fgPartialMode)
        {
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            u4Height = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;
            if(u4Height == 0)
                u4Height = 8 * rSrcImgInfo.rBufferFormat.u4VSampleFactor[0];
            u4Src1 = prImgReszInst->ptImgReszPartialBufInfo->u4YBufAddr;
            u4Src2 = prImgReszInst->ptImgReszPartialBufInfo->u4CbBufAddr;
            u4Src3 = prImgReszInst->ptImgReszPartialBufInfo->u4CrBufAddr;
            #else
            u4Height = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;
            if(u4Height == 0)
                u4Height = 8 * rSrcImgInfo.rBufferFormat.u4VSampleFactor[0];
            u4Src1 = prImgReszInst->tImgReszPartialBufInfo.u4YBufAddr;
            u4Src2 = prImgReszInst->tImgReszPartialBufInfo.u4CbBufAddr;
            u4Src3 = prImgReszInst->tImgReszPartialBufInfo.u4CrBufAddr;
            #endif
        }
        else
        {
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            u4Height = prImgReszInst->ptImgReszSrcBufInfo->u4PicHeight;
            u4Src1 = prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr;
            u4Src2 = prImgReszInst->ptImgReszSrcBufInfo->u4CbBufAddr;
            u4Src3 = prImgReszInst->ptImgReszSrcBufInfo->u4CrBufAddr;            
            #else
            u4Height = prImgReszInst->tImgReszSrcBufInfo.u4PicHeight;
            u4Src1 = prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr;
            u4Src2 = prImgReszInst->tImgReszSrcBufInfo.u4CbBufAddr;
            u4Src3 = prImgReszInst->tImgReszSrcBufInfo.u4CrBufAddr;            
            #endif
        }            

#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        if(prImgReszInst->ptImgReszSrcBufInfo->u4PicHeight == 1)
        #else
        if(prImgReszInst->tImgReszSrcBufInfo.u4PicHeight == 1)
        #endif
        {
            u4Height = 2;
            u4Src1 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1;
            u4Src2 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2;
            u4Src3 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3;
        }
#endif   

        switch(rSrcImgInfo.rBufferFormat.eBufferMainFormat)
        {
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
                u4Src = u4Src1;
                for(u4I=0; u4I<u4Height; u4I++)
                {
                    *(UINT8 *)(u4Src+1) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+2) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+3) = *(UINT8 *)(u4Src);
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                    #else
                    u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                    #endif
                }
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src1, prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src1,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height);
                    #endif
                #else
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src1,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src1,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height);
                    #endif
                #endif
                u4Src = u4Src2;
                u4Height2 = u4Height * rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] / rSrcImgInfo.rBufferFormat.u4VSampleFactor[0];
                for(u4I=0; u4I<u4Height2; u4I++)
                {
                    *(UINT8 *)(u4Src+1) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+2) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+3) = *(UINT8 *)(u4Src);
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                    #else
                    u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                    #endif
                }  
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src2,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src2,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height);
                    #endif
                #else
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src2,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height, TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src2,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height);
                    #endif
                #endif
                u4Src = u4Src3;
                u4Height2 = u4Height * rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] / rSrcImgInfo.rBufferFormat.u4VSampleFactor[0];
                for(u4I=0; u4I<u4Height2; u4I++)
                {
                    *(UINT8 *)(u4Src+1) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+2) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+3) = *(UINT8 *)(u4Src);
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                    #else
                    u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                    #endif
                }  
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src3, prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height,
                                      TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src3,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height);
                    #endif
                #else
                    #if IMGRESZ_SUPPORT_MAP_UNMAP
                    BSP_dma_map_vaddr(u4Src3, prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height,
                                      TO_DEVICE);
                    #else
                    BSP_FlushDCacheRange(u4Src3,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height);
                    #endif
                #endif
                break;
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
                u4Src = u4Src1;
                for(u4I=0; u4I<u4Height; u4I++)
                {
                    *(UINT8 *)(u4Src+1) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+2) = *(UINT8 *)(u4Src);
                    *(UINT8 *)(u4Src+3) = *(UINT8 *)(u4Src);
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                    #else
                    u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                    #endif
                }
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                #if IMGRESZ_SUPPORT_MAP_UNMAP
                BSP_dma_map_vaddr(u4Src1, prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height,
                                  TO_DEVICE);
                #else
                BSP_FlushDCacheRange(u4Src1,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height);
                #endif
                #else
                #if IMGRESZ_SUPPORT_MAP_UNMAP
                BSP_dma_map_vaddr(u4Src1,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height,
                                  TO_DEVICE);
                #else
                BSP_FlushDCacheRange(u4Src1,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height);
                #endif
                #endif
                break;
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
            case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                switch(rSrcImgInfo.rBufferFormat.eARGBBufferFormat)
                {
                    case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_0565:
                    case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_1555:
                    case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444:
                        u4Src = u4Src1;
                        for(u4I=0; u4I<u4Height; u4I++)
                        {
                            *(UINT16 *)(u4Src+2) = *(UINT16 *)(u4Src);
                            *(UINT16 *)(u4Src+4) = *(UINT16 *)(u4Src);
                            *(UINT16 *)(u4Src+6) = *(UINT16 *)(u4Src);
                            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                            u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                            #else
                            u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                            #endif
                        }                            
                        break;
                    case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888:
                        u4Src = u4Src1;
                        for(u4I=0; u4I<u4Height; u4I++)
                        {
                            *(UINT32 *)(u4Src+4) = *(UINT32 *)(u4Src);
                            *(UINT32 *)(u4Src+8) = *(UINT32 *)(u4Src);
                            *(UINT32 *)(u4Src+12) = *(UINT32 *)(u4Src);
                            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                            u4Src += prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth;
                            #else
                            u4Src += prImgReszInst->tImgReszSrcBufInfo.u4BufWidth;
                            #endif
                        }
                        break;
                }
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                #if IMGRESZ_SUPPORT_MAP_UNMAP
                BSP_dma_map_vaddr(u4Src1, prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height, 
                                  TO_DEVICE);
                #else
                BSP_FlushDCacheRange(u4Src1,prImgReszInst->ptImgReszSrcBufInfo->u4BufWidth * u4Height);
                #endif
                #else
                #if IMGRESZ_SUPPORT_MAP_UNMAP
                BSP_dma_map_vaddr(u4Src1, prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height, 
                                  TO_DEVICE);
                #else
                BSP_FlushDCacheRange(u4Src1,prImgReszInst->tImgReszSrcBufInfo.u4BufWidth * u4Height);
                #endif
                #endif
                break;
            default:
                break;
        }        
    }
#endif
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rSrcImgInfo.u4ImgXOff = prImgReszInst->ptImgReszSrcBufInfo->u4PicPosX;
    rSrcImgInfo.u4ImgYOff = prImgReszInst->ptImgReszSrcBufInfo->u4PicPosY;
    rSrcImgInfo.u4BufSA1 = prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr;
    rSrcImgInfo.u4BufSA2 = prImgReszInst->ptImgReszSrcBufInfo->u4CbBufAddr;
    rSrcImgInfo.u4BufSA3 = prImgReszInst->ptImgReszSrcBufInfo->u4CrBufAddr;
#else
    rSrcImgInfo.u4ImgXOff = prImgReszInst->tImgReszSrcBufInfo.u4PicPosX;
    rSrcImgInfo.u4ImgYOff = prImgReszInst->tImgReszSrcBufInfo.u4PicPosY;
    rSrcImgInfo.u4BufSA1 = prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr;
    rSrcImgInfo.u4BufSA2 = prImgReszInst->tImgReszSrcBufInfo.u4CbBufAddr;
    rSrcImgInfo.u4BufSA3 = prImgReszInst->tImgReszSrcBufInfo.u4CrBufAddr;
#endif    
#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if(prImgReszInst->ptImgReszSrcBufInfo->u4PicHeight == 1)
    #else
    if(prImgReszInst->tImgReszSrcBufInfo.u4PicHeight == 1)
    #endif
    {
        rSrcImgInfo.u4ImgYOff = 0;
        rSrcImgInfo.u4BufSA1 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA1;
        rSrcImgInfo.u4BufSA2 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA2;
        rSrcImgInfo.u4BufSA3 = _arImgreszHwInst[u4HwId].rSourceHeight2WorkAround.u4SA3;
    }
#endif
#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
    if(_arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.fgAYUVToARGBWorkAround)
    {
        if(_arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.u4TotalStep == 1)
        {
            rSrcImgInfo.u4BufWidth >>= 1;
            rSrcImgInfo.u4BufHeight <<= 1;
            rSrcImgInfo.u4ImgWidth >>= 1;
            rSrcImgInfo.u4ImgHeight <<= 1;
        } else
        {
            rSrcImgInfo.u4BufWidth >>= 1;
            rSrcImgInfo.u4ImgWidth >>= 1;
            rSrcImgInfo.u4BufSA1 += rSrcImgInfo.u4BufWidth * rSrcImgInfo.u4BufHeight
                                 * _arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.u4Step;
        }
    }

    if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
    {
        if(!_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCbTo444Done)
        {
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[2] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] = 1;

            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rSrcImgInfo.u4BufHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;
            rSrcImgInfo.u4ImgWidth = (prImgReszInst->ptImgReszSrcBufInfo->u4PicWidth - 2) *
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorH /
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH;
            rSrcImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine *
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorV /
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV;
            rSrcImgInfo.u4BufSA1 = prImgReszInst->ptImgReszPartialBufInfo->u4CbBufAddr;
            #else
            rSrcImgInfo.u4BufHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;
            rSrcImgInfo.u4ImgWidth = (prImgReszInst->tImgReszSrcBufInfo.u4PicWidth - 2) *
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorH /
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH;
            rSrcImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine *
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorV /
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV;
            rSrcImgInfo.u4BufSA1 = prImgReszInst->tImgReszPartialBufInfo.u4CbBufAddr;
            #endif
        } else if(!_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCrTo444Done)
        {
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[2] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] = 1;

            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rSrcImgInfo.u4BufHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;
            rSrcImgInfo.u4ImgWidth = (prImgReszInst->ptImgReszSrcBufInfo->u4PicWidth - 2) *
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorH /
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH;
            rSrcImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine *
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorV /
            	                     prImgReszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV;
            rSrcImgInfo.u4BufSA1 = prImgReszInst->ptImgReszPartialBufInfo->u4CrBufAddr;
            #else
            rSrcImgInfo.u4BufHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;
            rSrcImgInfo.u4ImgWidth = (prImgReszInst->tImgReszSrcBufInfo.u4PicWidth - 2) *
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorH /
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH;
            rSrcImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine *
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorV /
            	                     prImgReszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV;
            rSrcImgInfo.u4BufSA1 = prImgReszInst->tImgReszPartialBufInfo.u4CrBufAddr;
            #endif
        } else
        {
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4HSampleFactor[2] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[1] = 1;
            rSrcImgInfo.rBufferFormat.u4VSampleFactor[2] = 1;

            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rSrcImgInfo.u4BufSA1 = prImgReszInst->ptImgReszPartialBufInfo->u4YBufAddr;            
            #else
            rSrcImgInfo.u4BufSA1 = prImgReszInst->tImgReszPartialBufInfo.u4YBufAddr;            
            #endif
            if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCbTo444)
                rSrcImgInfo.u4BufSA2 = _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4Cb444Sa;
            if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCrTo444)
                rSrcImgInfo.u4BufSA3 = _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4Cr444Sa;

            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rSrcImgInfo.u4BufHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;        
            rSrcImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;        
            #else
            rSrcImgInfo.u4BufHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;        
            rSrcImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;        
            #endif
            rSrcImgInfo.u4BufWidth >>= 1;
            rSrcImgInfo.u4BufHeight <<= 1;
            rSrcImgInfo.u4ImgWidth >>= 1;
            rSrcImgInfo.u4ImgHeight <<= 1;
        }
    }
#endif    
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rSrcImgInfo.rBufferFormat.fgWT = prImgReszInst->ptImgReszSrcBufInfo->fgWTEnable;
    #else
    rSrcImgInfo.rBufferFormat.fgWT = prImgReszInst->tImgReszSrcBufInfo.fgWTEnable;
    #endif
    #endif

    i4ImgResz_HAL_Set_Source_Image_Info(u4HwId,&rSrcImgInfo);
        // Set destination buffer info
    x_memset(&rDestImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
        rDestImgInfo.rBufferFormat.fgBlockMode = FALSE;
        rDestImgInfo.rBufferFormat.fgAddrSwap = FALSE;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    switch(prImgReszInst->ptImgReszDstBufInfo->eDstColorMode)
#else
    switch(prImgReszInst->tImgReszDstBufInfo.eDstColorMode)
#endif        
    {
        case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888:
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
            break;
        case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_4444:
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
            rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444;
            break;
        case IMGRESZ_DRV_OUTPUT_COL_MD_AYUV:
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER;
            rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
            break;
        case IMGRESZ_DRV_OUTPUT_COL_MD_420_RS:
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
            rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
            break;
            case IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK:
                rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
                rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
                rDestImgInfo.rBufferFormat.fgBlockMode = TRUE;
                rDestImgInfo.rBufferFormat.fgAddrSwap = TRUE;
                break;
        default:
            break;
    }
    rDestImgInfo.rBufferFormat.fgProgressiveFrame = TRUE;
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rDestImgInfo.u4BufWidth = prImgReszInst->ptImgReszDstBufInfo->u4BufWidth;
    rDestImgInfo.u4BufHeight = prImgReszInst->ptImgReszDstBufInfo->u4BufHeight;
    rDestImgInfo.u4ImgWidth = prImgReszInst->ptImgReszDstBufInfo->u4PicWidth;
#else
    rDestImgInfo.u4BufWidth = prImgReszInst->tImgReszDstBufInfo.u4BufWidth;
    rDestImgInfo.u4BufHeight = prImgReszInst->tImgReszDstBufInfo.u4BufHeight;
    rDestImgInfo.u4ImgWidth = prImgReszInst->tImgReszDstBufInfo.u4PicWidth;
#endif    
    if (rDestImgInfo.u4ImgWidth == 1)
    {
        rDestImgInfo.u4ImgWidth = 2;
    }
#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rDestImgInfo.u4ImgHeight = prImgReszInst->ptImgReszDstBufInfo->u4PicHeight;
    
    rDestImgInfo.u4ImgXOff = prImgReszInst->ptImgReszDstBufInfo->u4PicPosX;
    rDestImgInfo.u4ImgYOff = prImgReszInst->ptImgReszDstBufInfo->u4PicPosY;
    rDestImgInfo.u4BufSA1 = prImgReszInst->ptImgReszDstBufInfo->u4YBufAddr;
    rDestImgInfo.u4BufSA2 = prImgReszInst->ptImgReszDstBufInfo->u4CBufAddr;
#else
    rDestImgInfo.u4ImgHeight = prImgReszInst->tImgReszDstBufInfo.u4PicHeight;
    
    rDestImgInfo.u4ImgXOff = prImgReszInst->tImgReszDstBufInfo.u4PicPosX;
    rDestImgInfo.u4ImgYOff = prImgReszInst->tImgReszDstBufInfo.u4PicPosY;
    rDestImgInfo.u4BufSA1 = prImgReszInst->tImgReszDstBufInfo.u4YBufAddr;
    rDestImgInfo.u4BufSA2 = prImgReszInst->tImgReszDstBufInfo.u4CBufAddr;
#endif    

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if (prImgReszInst->ptImgReszRmInfo->fgRPRMode)
    #else
    if (prImgReszInst->tImgReszRmInfo.fgRPRMode)
    #endif
    {
        // RPR mode should be 16 alignment
        rDestImgInfo.u4ImgHeight = (rDestImgInfo.u4ImgHeight + 15) / 16 * 16;
    }
#endif

#if IMGRESZ_HW_TARGET_HEIGHT_2_LIMIT
    if (rDestImgInfo.u4ImgHeight == 1)
    {
        rDestImgInfo.u4ImgHeight = 2;

        _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = 0;
        _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA2 = 0;
        _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA3 = 0;

#if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        switch (prImgReszInst->ptImgReszDstBufInfo->eDstColorMode)
#else
        switch (prImgReszInst->tImgReszDstBufInfo.eDstColorMode)
#endif            
        {
            case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888:
            case IMGRESZ_DRV_OUTPUT_COL_MD_AYUV:
            case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_1555:
            case IMGRESZ_DRV_OUTPUT_COL_MD_RGB_565:
            case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_4444:
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(rDestImgInfo.u4BufWidth * 2, 32);
#elif CONFIG_SYS_MEM_PHASE3
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(rDestImgInfo.u4BufWidth * 2, 32);
#endif
                #else
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(rDestImgInfo.u4BufWidth * 2, 16);
                #endif
                rDestImgInfo.u4BufSA1 = _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1;
                break;
            case IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK:
            case IMGRESZ_DRV_OUTPUT_COL_MD_420_RS:
            case IMGRESZ_DRV_OUTPUT_COL_MD_422_BLK:
            case IMGRESZ_DRV_OUTPUT_COL_MD_422_RS:
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_dma_mem(rDestImgInfo.u4BufWidth * 2, 32);
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_dma_mem(rDestImgInfo.u4BufWidth * 2, 32);
#elif CONFIG_SYS_MEM_PHASE3
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_mem_aligned_alloc(rDestImgInfo.u4BufWidth * 2, 32);
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA2 = (UINT32)x_mem_aligned_alloc(rDestImgInfo.u4BufWidth * 2, 32);
#endif
                #else
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1 = (UINT32)x_alloc_aligned_nc_mem(rDestImgInfo.u4BufWidth * 2, 16);
                _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA2 = (UINT32)x_alloc_aligned_nc_mem(rDestImgInfo.u4BufWidth * 2, 16);
                #endif
                rDestImgInfo.u4BufSA1 = _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA1;
                rDestImgInfo.u4BufSA2 = _arImgreszHwInst[u4HwId].rTargetHeight2WorkAround.u4SA2;
                break;
            
        }
    }
    
#endif    
    


    
   
#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
    if(_arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.fgAYUVToARGBWorkAround)
    {
        if(_arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.u4TotalStep == 1)
        {
            rDestImgInfo.u4BufWidth >>= 1;
            rDestImgInfo.u4BufHeight <<= 1;
            rDestImgInfo.u4ImgWidth >>= 1;
            rDestImgInfo.u4ImgHeight <<= 1;
        } else
        {
            rDestImgInfo.u4BufWidth >>= 1;
            rDestImgInfo.u4ImgWidth >>= 1;
            rDestImgInfo.u4BufSA1 += rDestImgInfo.u4BufWidth * rDestImgInfo.u4BufHeight
                                  * _arImgreszHwInst[u4HwId].rAYUVToARGBWorkAround.u4Step;            
        }
    }

    if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
    {
        if(!_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCbTo444Done)
        {
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
            rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4BufWidth = prImgReszInst->ptImgReszDstBufInfo->u4PicWidth;
            #else
            rDestImgInfo.u4BufWidth = prImgReszInst->tImgReszDstBufInfo.u4PicWidth;
            #endif
            rDestImgInfo.u4ImgWidth -= 2;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;
            #else
            rDestImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;
            #endif
            rDestImgInfo.u4BufSA1 = _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4Cb444Sa;
        } else if(!_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCrTo444Done)
        {
            rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
            rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;     
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4BufWidth = prImgReszInst->ptImgReszDstBufInfo->u4PicWidth;
            #else
            rDestImgInfo.u4BufWidth = prImgReszInst->tImgReszDstBufInfo.u4PicWidth;
            #endif
            rDestImgInfo.u4ImgWidth -= 2;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;            
            #else
            rDestImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;            
            #endif
            rDestImgInfo.u4BufSA1 = _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4Cr444Sa;
        } else
        {
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4BufSA1 += _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4LineCnt *
                                     prImgReszInst->ptImgReszDstBufInfo->u4BufWidth;
            rDestImgInfo.u4ImgHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;
            #else
            rDestImgInfo.u4BufSA1 += _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.u4LineCnt *
                                     prImgReszInst->tImgReszDstBufInfo.u4BufWidth;
            rDestImgInfo.u4ImgHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;
            #endif
            rDestImgInfo.u4BufWidth >>= 1;
            rDestImgInfo.u4BufHeight <<= 1;
            rDestImgInfo.u4ImgWidth >>= 1;
            rDestImgInfo.u4ImgHeight <<= 1;        
        }
    }
#endif    
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rDestImgInfo.rBufferFormat.fgWT = prImgReszInst->ptImgReszDstBufInfo->fgWTEnable;
    #else
    rDestImgInfo.rBufferFormat.fgWT = prImgReszInst->tImgReszDstBufInfo.fgWTEnable;
    #endif
    #endif
    i4ImgResz_HAL_Set_Destination_Image_Info(u4HwId,&rDestImgInfo);
    // Set blending buffer info
    x_memset(&rBldImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
    //rBldImgInfo.rBufferFormat // Blending buffer format is the same as destination buffer
    //rBldImgInfo.u4BufWidth    // Blending buffer width is the same as destination buffer
    //rBldImgInfo.u4ImgHeight   // Blending buffer height is the same as destination buffer
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    rBldImgInfo.u4BufSA1 = prImgReszInst->ptImgReszBldBufInfo->u4YBufAddr;
    rBldImgInfo.u4BufSA2 = prImgReszInst->ptImgReszBldBufInfo->u4CbBufAddr;
    rBldImgInfo.u1Alpha = prImgReszInst->ptImgReszBldBufInfo->u1Alpha;
    #else
    rBldImgInfo.u4BufSA1 = prImgReszInst->tImgReszBldBufInfo.u4YBufAddr;
    rBldImgInfo.u4BufSA2 = prImgReszInst->tImgReszBldBufInfo.u4CbBufAddr;
    //rBldImgInfo.u4BufSA3 = prImgReszInst->tImgReszBldBufInfo.u4CrBufAddr;
    rBldImgInfo.u1Alpha = prImgReszInst->tImgReszBldBufInfo.u1Alpha;
    #endif
    i4ImgResz_HAL_Set_Blending_Image_Info(u4HwId,&rBldImgInfo);
        // Register notification callback function
    rNofifyCallback.pvCallBackFunc = ImgReszHwInstNotifyCallback;
    rNofifyCallback.pvPrivData = (void *)u4HwId;
    i4ImgResz_HAL_Reg_Notify_Callback(u4HwId,&rNofifyCallback);
    
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
    if (prImgReszInst->ptImgReszRmInfo->fgRPRMode)
    {
        IMGRESZ_HAL_RM_INFO_T rRMInfo;

        x_memset(&rRMInfo, 0, sizeof(IMGRESZ_HAL_RM_INFO_T));

        rRMInfo.fgRPRMode = prImgReszInst->ptImgReszRmInfo->fgRPRMode;
        rRMInfo.fgRPRRacingModeEnable = prImgReszInst->ptImgReszRmInfo->fgRacingMode;

        i4ImgResz_HAL_Set_RM_Info(u4HwId, &rRMInfo);
    }
    #else
    if (prImgReszInst->tImgReszRmInfo.fgRPRMode)
    {
        IMGRESZ_HAL_RM_INFO_T rRMInfo;

        x_memset(&rRMInfo, 0, sizeof(IMGRESZ_HAL_RM_INFO_T));

        rRMInfo.fgRPRMode = prImgReszInst->tImgReszRmInfo.fgRPRMode;
        rRMInfo.fgRPRRacingModeEnable = prImgReszInst->tImgReszRmInfo.fgRacingMode;

        i4ImgResz_HAL_Set_RM_Info(u4HwId, &rRMInfo);
    }
    #endif
#endif

        // Set Jpeg info
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        if(prImgReszInst->ptImgReszSrcBufInfo->eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF)
        #else
        if(prImgReszInst->tImgReszSrcBufInfo.eSrcColorMode == IMGRESZ_DRV_INPUT_COL_MD_JPG_DEF)
        #endif
        {
            IMGRESZ_HAL_JPEG_INFO_T rJpegInfo;

            //x_memset(&rJpegInfo,0,sizeof(IMGRESZ_HAL_JPEG_INFO_T));

            rJpegInfo.fgPictureMode = (prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE);
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rJpegInfo.fgPreloadMode = prImgReszInst->ptImgReszJpegInfo->fgPreload;
            rJpegInfo.fgYExist = prImgReszInst->ptImgReszJpegInfo->fgExistY;
            rJpegInfo.fgCbExist = prImgReszInst->ptImgReszJpegInfo->fgExistCb;
            rJpegInfo.fgCrExist = prImgReszInst->ptImgReszJpegInfo->fgExistCr;
            #else
            rJpegInfo.fgPreloadMode = prImgReszInst->tImgReszJpegInfo.fgPreload;
            rJpegInfo.fgYExist = prImgReszInst->tImgReszJpegInfo.fgExistY;
            rJpegInfo.fgCbExist = prImgReszInst->tImgReszJpegInfo.fgExistCb;
            rJpegInfo.fgCrExist = prImgReszInst->tImgReszJpegInfo.fgExistCr;
            #endif
#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
            if(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
            {
                if(!(_arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCbTo444Done &&
                     _arImgreszHwInst[u4HwId].rYCbCrToAYUVWorkAround.fgCrTo444Done))
                {
                    rJpegInfo.fgYExist = TRUE;
                    rJpegInfo.fgCbExist = FALSE;
                    rJpegInfo.fgCrExist = FALSE;
                }
            }
#endif            
            i4ImgResz_HAL_Set_Jpeg_Info(u4HwId,&rJpegInfo);
        }
    }

    if(prImgReszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) // Jpeg picture mode
    {
        IMGRESZ_HAL_PARTIAL_BUF_INFO_T rSrcRowBufInfo = { 0 };

        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
        #if CONFIG_SYS_MEM_PHASE2
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,32);
        #else
        prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,32);
        #endif
        #elif CONFIG_SYS_MEM_PHASE3
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        prImgReszInst->u4TempLineBufSa = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,32);
        #else
        prImgReszInst->u4TempLineBufSa = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,32);
        #endif
        #endif
        #else
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,16);
        #else
        prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,16);
        #endif
        #endif
        VERIFY(prImgReszInst->u4TempLineBufSa != 0);
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        rSrcRowBufInfo.u4RowBufHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;

        rSrcRowBufInfo.fgFirstRowBuf = prImgReszInst->ptImgReszPartialBufInfo->fgFirstRow;
        rSrcRowBufInfo.fgLastRowBuf = prImgReszInst->ptImgReszPartialBufInfo->fgLastRow;

        rSrcRowBufInfo.u4CurRowBufSA1 = prImgReszInst->ptImgReszSrcBufInfo->u4YBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA2 = prImgReszInst->ptImgReszSrcBufInfo->u4CbBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA3 = prImgReszInst->ptImgReszSrcBufInfo->u4CrBufAddr;

        rSrcRowBufInfo.u4PrevRowBufSA1 = prImgReszInst->ptImgReszPartialBufInfo->u4YBufAddr;
        rSrcRowBufInfo.u4PrevRowBufSA2 = prImgReszInst->ptImgReszPartialBufInfo->u4CbBufAddr;
        rSrcRowBufInfo.u4PrevRowBufSA3 = prImgReszInst->ptImgReszPartialBufInfo->u4CrBufAddr;
        #else
        rSrcRowBufInfo.u4RowBufHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;

        rSrcRowBufInfo.fgFirstRowBuf = prImgReszInst->tImgReszPartialBufInfo.fgFirstRow;
        rSrcRowBufInfo.fgLastRowBuf = prImgReszInst->tImgReszPartialBufInfo.fgLastRow;

        rSrcRowBufInfo.u4CurRowBufSA1 = prImgReszInst->tImgReszSrcBufInfo.u4YBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA2 = prImgReszInst->tImgReszSrcBufInfo.u4CbBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA3 = prImgReszInst->tImgReszSrcBufInfo.u4CrBufAddr;

        rSrcRowBufInfo.u4PrevRowBufSA1 = prImgReszInst->tImgReszPartialBufInfo.u4YBufAddr;
        rSrcRowBufInfo.u4PrevRowBufSA2 = prImgReszInst->tImgReszPartialBufInfo.u4CbBufAddr;
        rSrcRowBufInfo.u4PrevRowBufSA3 = prImgReszInst->tImgReszPartialBufInfo.u4CrBufAddr;
        #endif

        i4ImgResz_HAL_Set_Partial_Mode_Info(u4HwId,&rSrcRowBufInfo,prImgReszInst->u4TempLineBufSa);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        //vHwImgReszWaitCountReset(u4HwId);
#endif
        i4ImgResz_HAL_Resize(u4HwId);

        // Notify Jpeg driver that image resizer is ready for picture mode.
        vImgResz_Inst_NotifyCallback(&(_arImgreszHwInst[u4HwId]),S_IMGRESZ_DRV_RESIZE_PIC_MODE_READY);
    } else if(!fgPartialMode) // Frame mode
    {
        if(fgInterlaced) // Interlaced frame
        {
            rSrcImgInfo.rBufferFormat.fgProgressiveFrame = FALSE;
            rSrcImgInfo.u4ImgHeight /= 2;
            rDestImgInfo.u4ImgHeight /= 2;
            i4ImgResz_HAL_Set_Destination_Image_Info(u4HwId,&rDestImgInfo);

            // Field resize
            rSrcImgInfo.rBufferFormat.fgTopField = _arImgreszHwInst[u4HwId].fgCurrTopField;
            i4ImgResz_HAL_Set_Source_Image_Info(u4HwId,&rSrcImgInfo);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            //vHwImgReszWaitCountReset(u4HwId);
#endif            
            i4ImgResz_HAL_Resize(u4HwId);
        } else  // Progressive frame
        {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            //vHwImgReszWaitCountReset(u4HwId);
#endif            
            i4ImgResz_HAL_Resize(u4HwId);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            if (prImgReszInst->ptImgReszRmInfo->fgRPRMode && prImgReszInst->ptImgReszRmInfo->fgRacingMode)
            #else
            if (prImgReszInst->tImgReszRmInfo.fgRPRMode && prImgReszInst->tImgReszRmInfo.fgRacingMode)
            #endif
            {
                vImgResz_Inst_NotifyCallback(&(_arImgreszHwInst[u4HwId]),S_IMGRESZ_DRV_RESIZE_PIC_MODE_READY);
            }
#endif

        }
    } else // Partial mode
    {
        IMGRESZ_HAL_PARTIAL_BUF_INFO_T rSrcRowBufInfo = { 0 };

        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        if(prImgReszInst->ptImgReszPartialBufInfo->fgFirstRow)
        #else
        if(prImgReszInst->tImgReszPartialBufInfo.fgFirstRow)
        #endif            
        {
            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,32);
            #else
            prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_dma_mem(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,32);
            #endif
#elif CONFIG_SYS_MEM_PHASE3
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            prImgReszInst->u4TempLineBufSa = (UINT32)x_mem_aligned_alloc(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,32);
            #else
            prImgReszInst->u4TempLineBufSa = (UINT32)x_mem_aligned_alloc(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,32);
            #endif
#endif
            #else
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->ptImgReszDstBufInfo->u4BufWidth*3,16);
            #else
            prImgReszInst->u4TempLineBufSa = (UINT32)x_alloc_aligned_nc_mem(prImgReszInst->tImgReszDstBufInfo.u4BufWidth*3,16);
            #endif
            #endif
            VERIFY(prImgReszInst->u4TempLineBufSa != 0);
        }

        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        rSrcRowBufInfo.u4RowBufHeight = prImgReszInst->ptImgReszPartialBufInfo->u4YBufLine;

        rSrcRowBufInfo.fgFirstRowBuf = prImgReszInst->ptImgReszPartialBufInfo->fgFirstRow;
        rSrcRowBufInfo.fgLastRowBuf = prImgReszInst->ptImgReszPartialBufInfo->fgLastRow;
        #else
        rSrcRowBufInfo.u4RowBufHeight = prImgReszInst->tImgReszPartialBufInfo.u4YBufLine;

        rSrcRowBufInfo.fgFirstRowBuf = prImgReszInst->tImgReszPartialBufInfo.fgFirstRow;
        rSrcRowBufInfo.fgLastRowBuf = prImgReszInst->tImgReszPartialBufInfo.fgLastRow;
        #endif

        rSrcRowBufInfo.u4PrevRowBufSA1 = prImgReszInst->u4PrevRowBufSa1;
        rSrcRowBufInfo.u4PrevRowBufSA2 = prImgReszInst->u4PrevRowBufSa2;
        rSrcRowBufInfo.u4PrevRowBufSA3 = prImgReszInst->u4PrevRowBufSa3;
        
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        rSrcRowBufInfo.u4CurRowBufSA1 = prImgReszInst->ptImgReszPartialBufInfo->u4YBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA2 = prImgReszInst->ptImgReszPartialBufInfo->u4CbBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA3 = prImgReszInst->ptImgReszPartialBufInfo->u4CrBufAddr;
        #else
        rSrcRowBufInfo.u4CurRowBufSA1 = prImgReszInst->tImgReszPartialBufInfo.u4YBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA2 = prImgReszInst->tImgReszPartialBufInfo.u4CbBufAddr;
        rSrcRowBufInfo.u4CurRowBufSA3 = prImgReszInst->tImgReszPartialBufInfo.u4CrBufAddr;
        #endif

        i4ImgResz_HAL_Set_Partial_Mode_Info(u4HwId,&rSrcRowBufInfo,prImgReszInst->u4TempLineBufSa);

#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
        i4ImgResz_HAL_Set_DstBuf_Reset(u4HwId, prImgReszInst->ptImgReszDstBufInfo->fgResetSA);
        if (prImgReszInst->ptImgReszDstBufInfo->fgResetSA)
        #else
        i4ImgResz_HAL_Set_DstBuf_Reset(u4HwId, prImgReszInst->tImgReszDstBufInfo.fgResetSA);
        if (prImgReszInst->tImgReszDstBufInfo.fgResetSA)
        #endif
        {
            // reset desination image info

            x_memset(&rDestImgInfo,0,sizeof(IMGRESZ_HAL_IMG_INFO_T));
                     rDestImgInfo.rBufferFormat.fgBlockMode = FALSE;
                     rDestImgInfo.rBufferFormat.fgAddrSwap = FALSE;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            switch(prImgReszInst->ptImgReszDstBufInfo->eDstColorMode)
            #else
            switch(prImgReszInst->tImgReszDstBufInfo.eDstColorMode)
            #endif
            {
                case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888:
                    rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
                    rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
                    break;
                case IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_4444:
                    rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER;
                    rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444;
                    break;
                case IMGRESZ_DRV_OUTPUT_COL_MD_AYUV:
                    rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER;
                    rDestImgInfo.rBufferFormat.eARGBBufferFormat = IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888;
                    break;
                case IMGRESZ_DRV_OUTPUT_COL_MD_420_RS:
                    rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
                    rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
                    break;
                    case IMGRESZ_DRV_OUTPUT_COL_MD_420_BLK:
                        rDestImgInfo.rBufferFormat.eBufferMainFormat = IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER;
                        rDestImgInfo.rBufferFormat.eYUVFormat = IMGRESZ_HAL_IMG_YUV_FORMAT_420;
                        rDestImgInfo.rBufferFormat.fgBlockMode = TRUE;
                        rDestImgInfo.rBufferFormat.fgAddrSwap = TRUE;
                        break;
                default:
                    break;
            }
            rDestImgInfo.rBufferFormat.fgProgressiveFrame = TRUE;
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4BufWidth = prImgReszInst->ptImgReszDstBufInfo->u4BufWidth;
            rDestImgInfo.u4BufHeight = prImgReszInst->ptImgReszDstBufInfo->u4BufHeight;
            rDestImgInfo.u4ImgWidth = prImgReszInst->ptImgReszDstBufInfo->u4PicWidth;
            #else
            rDestImgInfo.u4BufWidth = prImgReszInst->tImgReszDstBufInfo.u4BufWidth;
            rDestImgInfo.u4BufHeight = prImgReszInst->tImgReszDstBufInfo.u4BufHeight;
            rDestImgInfo.u4ImgWidth = prImgReszInst->tImgReszDstBufInfo.u4PicWidth;
            #endif
            if (rDestImgInfo.u4ImgWidth == 1)
            {
                rDestImgInfo.u4ImgWidth = 2;
            }
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.u4ImgHeight = prImgReszInst->ptImgReszDstBufInfo->u4PicHeight;
            
            rDestImgInfo.u4ImgXOff = prImgReszInst->ptImgReszDstBufInfo->u4PicPosX;
            rDestImgInfo.u4ImgYOff = prImgReszInst->ptImgReszDstBufInfo->u4PicPosY;
            rDestImgInfo.u4BufSA1 = prImgReszInst->ptImgReszDstBufInfo->u4YBufAddr;
            rDestImgInfo.u4BufSA2 = prImgReszInst->ptImgReszDstBufInfo->u4CBufAddr;
            #else
            rDestImgInfo.u4ImgHeight = prImgReszInst->tImgReszDstBufInfo.u4PicHeight;
            
            rDestImgInfo.u4ImgXOff = prImgReszInst->tImgReszDstBufInfo.u4PicPosX;
            rDestImgInfo.u4ImgYOff = prImgReszInst->tImgReszDstBufInfo.u4PicPosY;
            rDestImgInfo.u4BufSA1 = prImgReszInst->tImgReszDstBufInfo.u4YBufAddr;
            rDestImgInfo.u4BufSA2 = prImgReszInst->tImgReszDstBufInfo.u4CBufAddr;
            #endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
            rDestImgInfo.rBufferFormat.fgWT = prImgReszInst->ptImgReszDstBufInfo->fgWTEnable;
            #else
            rDestImgInfo.rBufferFormat.fgWT = prImgReszInst->tImgReszDstBufInfo.fgWTEnable;
            #endif
#endif
            i4ImgResz_HAL_Set_Destination_Image_Info(u4HwId,&rDestImgInfo);

        }
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        //vHwImgReszWaitCountReset(u4HwId);
#endif        
        i4ImgResz_HAL_Resize(u4HwId);

        prImgReszInst->u4PrevRowBufSa1 = rSrcRowBufInfo.u4CurRowBufSA1;
        prImgReszInst->u4PrevRowBufSa2 = rSrcRowBufInfo.u4CurRowBufSA2;
        prImgReszInst->u4PrevRowBufSa3 = rSrcRowBufInfo.u4CurRowBufSA3;
    }

}


void vImgReszHwInstStopScale(IMGRESZ_HW_INST_T* ptImgReszHwInst)
{
    VERIFY(!ptImgReszHwInst->fgWaitLock);

    if (ptImgReszHwInst->fgImgReszActive)
    {
        //notify stop callback
        //i4ImgResz_Inst_NotifyCallbackProc(ptImgReszHwInst->u2ImgReszCompId,S_IMGRESZ_DRV_RESIZE_STOP);
        
        vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback(ptImgReszHwInst,FALSE,S_IMGRESZ_DRV_RESIZE_STOP);
    } else
    {
        ASSERT(0);
    }
}


void vImgReszHwInstTimeout(IMGRESZ_HW_INST_T* ptImgReszHwInst)
{
    if (ptImgReszHwInst->fgImgReszActive)
    {
        //stop HAL
        i4ImgResz_HAL_Uninit(ptImgReszHwInst->u2ImgReszHwId);

        //notify stop callback
        //i4ImgResz_Inst_NotifyCallbackProc(ptImgReszHwInst->u2ImgReszCompId,E_IMGRESZ_DRV_RESIZE_TIMEOUT);
        
        vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback(ptImgReszHwInst,FALSE,E_IMGRESZ_DRV_RESIZE_TIMEOUT);
    } else
    {
            ASSERT(0);
    }
}


extern void vImgReszInstLog(UINT32 u4Event,UINT32 u4Value);
void vImgReszHwInstScaleNotify(BOOL fgForceHwId,UINT32 *pu4HwId)
{
    INT32 i4,i4Ret;

    vImgReszInstLog(0x11,fgForceHwId);

    if(fgForceHwId)
{
        vImgReszInstLog(0x12,*pu4HwId);

        //sent a event to related HW instance
        i4Ret = x_ev_group_set_event(_arImgreszHwInst[*pu4HwId].hEventHandle, IMGRESZ_EV_DO_SCALE, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    } else
    {
    for (i4=0; i4<IMGRESZ_HW_INST_NUM; i4++)
    {
            if ((_arImgreszHwInst[i4].fgImgReszActive == FALSE) && !_arImgreszHwInst[i4].fgLock)
        {
                vImgReszInstLog(0x12,i4);

            //sent a event to related HW instance
            i4Ret = x_ev_group_set_event(_arImgreszHwInst[i4].hEventHandle, IMGRESZ_EV_DO_SCALE, X_EV_OP_OR);
            VERIFY(i4Ret == OSR_OK);
            break;
        }
    }
    }

}


INT32 i4ImgReszHwInstLockNotify(UINT32 u4HwInstId,BOOL fgLock)
{
    INT32 i4Ret;

    if(fgLock)
    {
        //sent a event to related HW instance
        i4Ret = x_ev_group_set_event(_arImgreszHwInst[u4HwInstId].hEventHandle, IMGRESZ_EV_LOCK, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    } else
    {
        //sent a event to related HW instance
        i4Ret = x_ev_group_set_event(_arImgreszHwInst[u4HwInstId].hEventHandle, IMGRESZ_EV_UNLOCK, X_EV_OP_OR);
        VERIFY(i4Ret == OSR_OK);
    }

    return 0;
}


INT32 i4ImgReszHwInstStopNotify(UINT32 u4HwInstId)
{
    INT32 i4Ret;

    i4Ret = x_ev_group_set_event(_arImgreszHwInst[u4HwInstId].hEventHandle, IMGRESZ_EV_STOP_SCALE, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);
    
    return S_IMGRESZ_DRV_OK;
}


INT32 i4ImgReszHwInstGetNum(void)
{
    return IMGRESZ_HW_INST_NUM;
}


BOOL fgImgReszHwInstIsActive(UINT32 u4HwInstId)
{
    return _arImgreszHwInst[u4HwInstId].fgImgReszActive;
}


void vImgReszHwInstMain(void *arg)
{
    INT32               i4Ret = 1;
    UINT32              u4ThreadId = *((UINT32 *)arg);
    IMGRESZ_HW_INST_T*  ptImgReszHwInst = NULL;
    EV_GRP_EVENT_T      eImgReszEvent;
    EV_GRP_EVENT_T      eImgReszEventGet;

    if (u4ThreadId > (IMGRESZ_HW_INST_NUM - 1))
    {
        ASSERT(0);
    }
    ptImgReszHwInst = &(_arImgreszHwInst[u4ThreadId]);
    ptImgReszHwInst->u2ImgReszHwId = u4ThreadId;

    while(TRUE)
    {
        if (!ptImgReszHwInst->fgImgReszActive)
        {
            eImgReszEvent = IMGRESZ_EV_FINISH |
                            IMGRESZ_EV_DO_SCALE | IMGRESZ_EV_LOCK | IMGRESZ_EV_UNLOCK |
                            IMGRESZ_EV_FINISH_SCALE;
            i4Ret = x_ev_group_wait_event(ptImgReszHwInst->hEventHandle, eImgReszEvent, &eImgReszEventGet, X_EV_OP_OR_CONSUME);
            VERIFY(i4Ret == OSR_OK);

            // Last stop cause un-serviced finish-scale event.
            // Skip finish scale event.
            eImgReszEvent &= ~IMGRESZ_EV_FINISH_SCALE;
        }
        else
        {
            UINT32 u4TimeOut;
            IMGRESZ_INST_T *prImgreszInst;

            i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);
            if((prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                || (prImgreszInst->ptImgReszRmInfo->fgRPRMode && prImgreszInst->ptImgReszRmInfo->fgRacingMode)
            #else
               || (prImgreszInst->tImgReszRmInfo.fgRPRMode && prImgreszInst->tImgReszRmInfo.fgRacingMode)
            #endif
#endif               
                )
            {
                u4TimeOut = 0xFFFFFFFF;
            } else
            {
                u4TimeOut = 1000;
            }
            eImgReszEvent = IMGRESZ_EV_FINISH |
                            IMGRESZ_EV_DO_SCALE | IMGRESZ_EV_STOP_SCALE | IMGRESZ_EV_FINISH_SCALE |
                            IMGRESZ_EV_LOCK | IMGRESZ_EV_UNLOCK;
            i4Ret = x_ev_group_wait_event_timeout(ptImgReszHwInst->hEventHandle, eImgReszEvent, &eImgReszEventGet, X_EV_OP_OR_CONSUME, u4TimeOut);
            if(i4Ret == OSR_TIMEOUT)
            {
                vImgReszHwInstTimeout(ptImgReszHwInst);
                continue;
            }
            else
            VERIFY(i4Ret == OSR_OK);
        }

        // Terminate thread
        if(eImgReszEventGet & IMGRESZ_EV_FINISH)
            ptImgReszHwInst->fgWaitThreadFinish = TRUE;        

        // State machine for lock and unlock
        if ((eImgReszEventGet & IMGRESZ_EV_LOCK) || (eImgReszEventGet & IMGRESZ_EV_UNLOCK))
        {
            BOOL fgLock;
            UINT32 u4InstId;

            i4Ret = i4ImgResz_Inst_GetLockInstance(ptImgReszHwInst,&fgLock,&u4InstId);
            VERIFY(i4Ret >= 0);

            if(ptImgReszHwInst->fgImgReszActive)
            {
                if(!ptImgReszHwInst->fgLock && fgLock)
                    ptImgReszHwInst->fgWaitLock = TRUE;
                else if(ptImgReszHwInst->fgLock && !fgLock)
                    VERIFY(FALSE);
            }

            // When lock to unlock, trigger itself to search other unserviced instance.
            if(ptImgReszHwInst->fgLock && !fgLock)
                eImgReszEventGet |= IMGRESZ_EV_DO_SCALE;

            if(!ptImgReszHwInst->fgWaitLock)
            {
                ptImgReszHwInst->fgLock = fgLock;
                ptImgReszHwInst->u2ImgReszCompId = u4InstId;
            } else
            {
                ptImgReszHwInst->u4WaitLockInstId = u4InstId;
            }
        }

        //check component state, and then do scale or stop
        if (ptImgReszHwInst->fgImgReszActive)
        {
            IMGRESZ_INST_T *prImgreszInst;

            i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);

            if (eImgReszEventGet & IMGRESZ_EV_STOP_SCALE)
            {
                IMGRESZ_INST_T *prImgreszInst;

                //stop HAL
                i4ImgResz_HAL_Uninit(ptImgReszHwInst->u2ImgReszHwId);

                i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);

                // Free memory
#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                if(prImgreszInst->ptImgReszSrcBufInfo->u4PicHeight == 1)
                #else
                if(prImgreszInst->tImgReszSrcBufInfo.u4PicHeight == 1)
                #endif
                {
                    // Free work around buffer
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
#elif CONFIG_SYS_MEM_PHASE3
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
                        #endif
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
#elif CONFIG_SYS_MEM_PHASE3
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
                        #endif
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
#elif CONFIG_SYS_MEM_PHASE3
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
                        #endif

                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1 = 0;
                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2 = 0;
                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3 = 0;                
                }
#endif                
                if((prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) ||
                   (prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE))
                {
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    x_free_aligned_dma_mem((void *)prImgreszInst->u4TempLineBufSa);
#elif CONFIG_SYS_MEM_PHASE3
                    x_mem_aligned_free((void *)prImgreszInst->u4TempLineBufSa);
#endif
                    #else
                    x_free_aligned_nc_mem((void *)prImgreszInst->u4TempLineBufSa);
                    #endif
                    prImgreszInst->u4TempLineBufSa = 0;
                }

                //stop scale
                vImgReszHwInstStopScale(ptImgReszHwInst);
            }
            else if (eImgReszEventGet & IMGRESZ_EV_FINISH_SCALE)
            {
                IMGRESZ_INST_T *prImgreszInst;

                i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                vHwImgReszWaitCountReset(ptImgReszHwInst->u2ImgReszCompId);
#endif

                // State machine for multi-resize (interlaced video)
                if(ptImgReszHwInst->fgInterlaced)
                {
                    IMGRESZ_INST_T *prImgreszInst;
    
                    i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    if(ptImgReszHwInst->fgCurrTopField && prImgreszInst->ptImgReszSrcBufInfo->fgBottomField)
                    #else
                    if(ptImgReszHwInst->fgCurrTopField && prImgreszInst->tImgReszSrcBufInfo.fgBottomField)
                    #endif
                    {
                        // Keep doing scale bottom field
                        ptImgReszHwInst->fgCurrTopField = FALSE;
                        vImgReszHwInstResz((UINT32)(ptImgReszHwInst->u2ImgReszHwId),prImgreszInst);
                        continue;
                    }
                }


#if IMGRESZ_HW_SOURCE_HEIGHT_2_LIMIT
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                if(prImgreszInst->ptImgReszSrcBufInfo->u4PicHeight == 1)
                #else
                if(prImgreszInst->tImgReszSrcBufInfo.u4PicHeight == 1)
                #endif
                {
                    // Free work around buffer
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
#elif CONFIG_SYS_MEM_PHASE3
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1);
                        #endif
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
#elif CONFIG_SYS_MEM_PHASE3
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2);
                        #endif
                    if(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3 != 0)
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
#elif CONFIG_SYS_MEM_PHASE3
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3, 
                                            prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #else
                        BSP_dma_unmap_vaddr(ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3,
                                            prImgreszInst->tImgReszSrcBufInfo->u4BufWidth * 2,
                                            TO_DEVICE);
                        #endif
                        #endif
                        x_mem_aligned_free((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3);
                        #endif

                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA1 = 0;
                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA2 = 0;
                    ptImgReszHwInst->rSourceHeight2WorkAround.u4SA3 = 0;
                }
#endif

#if IMGRESZ_HW_TARGET_HEIGHT_2_LIMIT
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                if (prImgreszInst->ptImgReszDstBufInfo->u4PicHeight == 1)
                #else
                if (prImgreszInst->tImgReszDstBufInfo.u4PicHeight == 1)
                #endif
                {
                    if (ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1 != 0)
                    {
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        x_memcpy((void *)prImgreszInst->ptImgReszDstBufInfo->u4YBufAddr,
                                 (void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1,
                                 prImgreszInst->ptImgReszDstBufInfo->u4BufWidth);
                        #if IMGRESZ_SUPPORT_MAP_UNMAP // Ping Zhao , temp solution
                        BSP_dma_map_vaddr(prImgreszInst->ptImgReszDstBufInfo->u4YBufAddr, 
                                          prImgreszInst->ptImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        BSP_dma_unmap_vaddr(prImgreszInst->ptImgReszDstBufInfo->u4YBufAddr, 
                                            prImgreszInst->ptImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        #endif
                        #else
                        x_memcpy((void *)prImgreszInst->tImgReszDstBufInfo.u4YBufAddr,
                                 (void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1,
                                 prImgreszInst->tImgReszDstBufInfo.u4BufWidth);
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP // Ping Zhao , temp solution
                        BSP_dma_map_vaddr(prImgreszInst->tImgReszDstBufInfo->u4YBufAddr, 
                                          prImgreszInst->tImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        BSP_dma_unmap_vaddr(prImgreszInst->tImgReszDstBufInfo->u4YBufAddr, 
                                            prImgreszInst->tImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        #endif
                        #endif
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA1);
                        #endif
                    }
                    if (ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2 != 0)
                    {
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        x_memcpy((void *)prImgreszInst->ptImgReszDstBufInfo->u4CBufAddr,
                                 (void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2,
                                 prImgreszInst->ptImgReszDstBufInfo->u4BufWidth);
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP // Ping Zhao , temp solution
                        BSP_dma_map_vaddr(prImgreszInst->ptImgReszDstBufInfo->u4CBufAddr, 
                                          prImgreszInst->ptImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        BSP_dma_unmap_vaddr(prImgreszInst->ptImgReszDstBufInfo->u4CBufAddr, 
                                            prImgreszInst->ptImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        #endif
                        #else
                        x_memcpy((void *)prImgreszInst->tImgReszDstBufInfo.u4CBufAddr,
                                 (void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2,
                                 prImgreszInst->tImgReszDstBufInfo.u4BufWidth);
                        
                        #if IMGRESZ_SUPPORT_MAP_UNMAP // Ping Zhao , temp solution
                        BSP_dma_map_vaddr(prImgreszInst->tImgReszDstBufInfo->u4CBufAddr, 
                                          prImgreszInst->tImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        BSP_dma_unmap_vaddr(prImgreszInst->tImgReszDstBufInfo->u4CBufAddr, 
                                            prImgreszInst->tImgReszDstBufInfo->u4BufWidth, TO_DEVICE);
                        #endif
                        #endif
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA2);
                        #endif
                    }
                    if (ptImgReszHwInst->rTargetHeight2WorkAround.u4SA3 != 0)
                    {
                        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                        x_free_aligned_dma_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA3);
#elif CONFIG_SYS_MEM_PHASE3
                        x_mem_aligned_free((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA3);
#endif
                        #else
                        x_free_aligned_nc_mem((void *)ptImgReszHwInst->rTargetHeight2WorkAround.u4SA3);
                        #endif
                    }
                }
#endif

#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
                // State machine for multi-resize (AYUV to ARGB work around)
                if(ptImgReszHwInst->rAYUVToARGBWorkAround.fgAYUVToARGBWorkAround)
                {
                    ptImgReszHwInst->rAYUVToARGBWorkAround.u4Step++;

                    if(ptImgReszHwInst->rAYUVToARGBWorkAround.u4Step < ptImgReszHwInst->rAYUVToARGBWorkAround.u4TotalStep)
                    {
                        i4ImgResz_HAL_Uninit(u4ThreadId);
                    
                        // Keep doing scale next step
                        vImgReszHwInstResz((UINT32)(ptImgReszHwInst->u2ImgReszHwId),prImgreszInst);
                        continue;
                    }
                }

                // State machine for multi-resize (YCbCr to AYUV work around)
                if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
                {
                    if(!ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444Done)
                    {                        
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444Done = TRUE;

                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        if(prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorH)
                        #else
                        if(prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorH)
                        #endif
                        {
                            UINT32 u4I,u4SrcSa,u4TrgSa;
                            UINT8 u1Value;
                            
                            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                            u4SrcSa = prImgreszInst->ptImgReszSrcBufInfo->u4CbBufAddr;
                            #else
                            u4SrcSa = prImgreszInst->tImgReszSrcBufInfo.u4CbBufAddr;
                            #endif
                            u4TrgSa = ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa;

                            #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                            for(u4I=0; u4I<prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine; u4I++)
                            {
                                u1Value = *(UINT8 *)(u4SrcSa + (prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth>>1) - 1);
                                *(UINT8 *)(u4TrgSa + prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth - 2) = u1Value;
                                *(UINT8 *)(u4TrgSa + prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth - 1) = u1Value;

                                if((prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorV) &&
                                   ((u4I & 1) == 0))
                                {
                                } else
                                {
                                    u4SrcSa += prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth;
                                }
                                u4TrgSa += prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth;
                            }
                            #else
                            for(u4I=0; u4I<prImgreszInst->tImgReszPartialBufInfo.u4YBufLine; u4I++)
                            {
                                u1Value = *(UINT8 *)(u4SrcSa + (prImgreszInst->tImgReszSrcBufInfo.u4PicWidth>>1) - 1);
                                *(UINT8 *)(u4TrgSa + prImgreszInst->tImgReszSrcBufInfo.u4PicWidth - 2) = u1Value;
                                *(UINT8 *)(u4TrgSa + prImgreszInst->tImgReszSrcBufInfo.u4PicWidth - 1) = u1Value;

                                if((prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorV) &&
                                   ((u4I & 1) == 0))
                                {
                                } else
                                {
                                    u4SrcSa += prImgreszInst->tImgReszSrcBufInfo.u4PicWidth;
                                }
                                u4TrgSa += prImgreszInst->tImgReszSrcBufInfo.u4PicWidth;
                            }
                            #endif
                        }

                        // Treat as frame mode.
                        i4ImgResz_HAL_Uninit(u4ThreadId);
                        
                        // Keep doing scale next step
                        vImgReszHwInstResz((UINT32)(ptImgReszHwInst->u2ImgReszHwId),prImgreszInst);
                        continue;                        
                    } else if(!ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444Done)
                    {
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444Done = TRUE;

                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        if(prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorH)
                        {
                            UINT32 u4I,u4SrcSa,u4TrgSa;
                            UINT8 u1Value;

                            u4SrcSa = prImgreszInst->ptImgReszSrcBufInfo->u4CrBufAddr;
                            u4TrgSa = ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa;
                            for(u4I=0; u4I<prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine; u4I++)
                            {
                                u1Value = *(UINT8 *)(u4SrcSa + (prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth>>1) - 1);
                                *(UINT8 *)(u4TrgSa + prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth - 2) = u1Value;
                                *(UINT8 *)(u4TrgSa + prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth - 1) = u1Value;

                                if((prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorV) &&
                                   ((u4I & 1) == 0))
                                {
                                } else
                                {
                                    u4SrcSa += prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth;
                                }
                                u4TrgSa += prImgreszInst->ptImgReszSrcBufInfo->u4PicWidth;
                            }
                        }
                        #else
                        if(prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorH)
                        {
                            UINT32 u4I,u4SrcSa,u4TrgSa;
                            UINT8 u1Value;

                            u4SrcSa = prImgreszInst->tImgReszSrcBufInfo.u4CrBufAddr;
                            u4TrgSa = ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa;
                            for(u4I=0; u4I<prImgreszInst->tImgReszPartialBufInfo.u4YBufLine; u4I++)
                            {
                                u1Value = *(UINT8 *)(u4SrcSa + (prImgreszInst->tImgReszSrcBufInfo.u4PicWidth>>1) - 1);
                                *(UINT8 *)(u4TrgSa + prImgreszInst->tImgReszSrcBufInfo.u4PicWidth - 2) = u1Value;
                                *(UINT8 *)(u4TrgSa + prImgreszInst->tImgReszSrcBufInfo.u4PicWidth - 1) = u1Value;

                                if((prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorV) &&
                                   ((u4I & 1) == 0))
                                {
                                } else
                                {
                                    u4SrcSa += prImgreszInst->tImgReszSrcBufInfo.u4PicWidth;
                                }
                                u4TrgSa += prImgreszInst->tImgReszSrcBufInfo.u4PicWidth;
                            }
                        }
                        #endif

                        // Treat as frame mode.
                        i4ImgResz_HAL_Uninit(u4ThreadId);
                        
                        // Keep doing scale next step
                        vImgReszHwInstResz((UINT32)(ptImgReszHwInst->u2ImgReszHwId),prImgreszInst);
                        continue;                        
                    } else
                    {
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4LineCnt += prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine;
                        #else
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4LineCnt += prImgreszInst->tImgReszPartialBufInfo.u4YBufLine;
                        #endif
                    
                        // Free temp buffer
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                            x_free_aligned_dma_mem((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa);
#elif CONFIG_SYS_MEM_PHASE3
                            x_mem_aligned_free((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa);
#endif
                            #else
                            x_free_aligned_nc_mem((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa);
                            #endif
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                            x_free_aligned_dma_mem((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa);
#elif CONFIG_SYS_MEM_PHASE3
                            x_mem_aligned_free((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa);
#endif
                            #else
                            x_free_aligned_nc_mem((void *)ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa);
                            #endif
                    }
                }
#endif                

#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
                if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround)
                {
                    // Treat as frame mode.
                    i4ImgResz_HAL_Uninit(u4ThreadId);
                } else
                {
#endif
                #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                if(!(prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE) || prImgreszInst->ptImgReszPartialBufInfo->fgLastRow)
                #else
                if(!(prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE) || prImgreszInst->tImgReszPartialBufInfo.fgLastRow)
                #endif
                {
                    i4ImgResz_HAL_Uninit(u4ThreadId);
                }

                if((prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_JPEG_PIC_SCALE) ||
                   ((prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE) &&
                   #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    (prImgreszInst->ptImgReszPartialBufInfo->fgLastRow)))
                   #else
                    (prImgreszInst->tImgReszPartialBufInfo.fgLastRow)))
                   #endif
                {
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                    x_free_aligned_dma_mem((void *)prImgreszInst->u4TempLineBufSa);
#elif CONFIG_SYS_MEM_PHASE3
                    x_mem_aligned_free((void *)prImgreszInst->u4TempLineBufSa);
#endif
                    #else
                    x_free_aligned_nc_mem((void *)prImgreszInst->u4TempLineBufSa);
                    #endif
                    prImgreszInst->u4TempLineBufSa = 0;
                }

#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT
                }
#endif
                //notify finish callback
                //i4ImgResz_Inst_NotifyCallbackProc(ptImgReszHwInst->u2ImgReszCompId,S_IMGRESZ_DRV_RESIZE_FINISH);

                vImgResz_Inst_ReleaseServicedInstanceAndNotifyCallback(ptImgReszHwInst,TRUE,S_IMGRESZ_DRV_RESIZE_FINISH);

                if(ptImgReszHwInst->fgWaitLock)
                {
                    ptImgReszHwInst->fgWaitLock = FALSE;
                    ptImgReszHwInst->fgLock = TRUE;
                    ptImgReszHwInst->u2ImgReszCompId = ptImgReszHwInst->u4WaitLockInstId;
                    i4Ret = x_ev_group_set_event(ptImgReszHwInst->hEventHandle, IMGRESZ_EV_DO_SCALE, X_EV_OP_OR);
                    VERIFY(i4Ret == OSR_OK);
                }
            }
        }
        else
        {
            // Terminate thread    
            if(ptImgReszHwInst->fgWaitThreadFinish)
            {
                ptImgReszHwInst->fgWaitThreadFinish = FALSE;
                break;
            }
        
            if (eImgReszEventGet & IMGRESZ_EV_DO_SCALE)
            {
                IMGRESZ_INST_T *prImgreszInst;

                i4Ret = i4ImgResz_Inst_GetUnservicedInstance(ptImgReszHwInst);
                //start do scale
                if(i4Ret>=0)
                {
                    i4ImgResz_Inst_GetInstanceObject(ptImgReszHwInst->u2ImgReszCompId,&prImgreszInst);
                    // Interlaced mode preprocess
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    ptImgReszHwInst->fgInterlaced = prImgreszInst->ptImgReszSrcBufInfo->fgInterlaced;
                    if(prImgreszInst->ptImgReszSrcBufInfo->fgInterlaced)
                    {
                        if(prImgreszInst->ptImgReszSrcBufInfo->fgTopField)
                            ptImgReszHwInst->fgCurrTopField = TRUE;
                        else
                            ptImgReszHwInst->fgCurrTopField = FALSE;
                    }
                    #else
                    ptImgReszHwInst->fgInterlaced = prImgreszInst->tImgReszSrcBufInfo.fgInterlaced;
                    if(prImgreszInst->tImgReszSrcBufInfo.fgInterlaced)
                    {
                        if(prImgreszInst->tImgReszSrcBufInfo.fgTopField)
                            ptImgReszHwInst->fgCurrTopField = TRUE;
                        else
                            ptImgReszHwInst->fgCurrTopField = FALSE;
                    }
                    #endif
#if IMGRESZ_HW_DEST_WIDTH_4096_LIMIT                    
                    // AYUV to ARGB preprocess
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    if((prImgreszInst->ptImgReszDstBufInfo->eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888) &&
                       (prImgreszInst->ptImgReszDstBufInfo->u4PicWidth == 4096))
                    #else
                    if((prImgreszInst->tImgReszDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_ARGB_8888) &&
                       (prImgreszInst->tImgReszDstBufInfo.u4PicWidth == 4096))
                    #endif
                    {
                        ptImgReszHwInst->rAYUVToARGBWorkAround.fgAYUVToARGBWorkAround = TRUE;
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        if((prImgreszInst->ptImgReszDstBufInfo->u4PicHeight << 1) < 4096)
                        #else
                        if((prImgreszInst->tImgReszDstBufInfo.u4PicHeight << 1) < 4096)
                        #endif
                        {
                            ptImgReszHwInst->rAYUVToARGBWorkAround.u4TotalStep = 1;
                        } else
                        {
                            ptImgReszHwInst->rAYUVToARGBWorkAround.u4TotalStep = 2;
                        }
                        ptImgReszHwInst->rAYUVToARGBWorkAround.u4Step = 0;
                    } else
                    {
                        ptImgReszHwInst->rAYUVToARGBWorkAround.fgAYUVToARGBWorkAround = FALSE;
                    }

                    // YCbCr to AYUV preprocess
                    #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                    if((prImgreszInst->ptImgReszDstBufInfo->eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_AYUV) &&
                       (prImgreszInst->ptImgReszDstBufInfo->u4PicWidth == 4096))
                    #else
                    if((prImgreszInst->tImgReszDstBufInfo.eDstColorMode == IMGRESZ_DRV_OUTPUT_COL_MD_AYUV) &&
                       (prImgreszInst->tImgReszDstBufInfo.u4PicWidth == 4096))
                    #endif                       
                    {
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround = TRUE;
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444 =
                            (prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorH)
                         || (prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CbCompFactorV);
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444 =
                            (prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorH != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorH)
                         || (prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1YCompFactorV != prImgreszInst->ptImgReszSrcBufInfo->rCompFactor.u1CrCompFactorV);
                        if(prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine == 0)
                            prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine = 16;
                        // Allocate temp buffer
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa =
                                (UINT32)x_alloc_aligned_dma_mem(prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth *
                                                               prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine,32);
                            #else
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa =
                                (UINT32)x_alloc_aligned_nc_mem(prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth *
                                                               prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine,16);
                            #endif
                                //(UINT32)x_alloc_aligned_nc_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                //                               prImgreszInst->tImgReszSrcBufInfo.u4BufHeight,16);
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa =
                                (UINT32)x_alloc_aligned_dma_mem(prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth *
                                                               prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine,32);
                            #else
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa =
                                (UINT32)x_alloc_aligned_nc_mem(prImgreszInst->ptImgReszSrcBufInfo->u4BufWidth *
                                                               prImgreszInst->ptImgReszPartialBufInfo->u4YBufLine,16);
                            #endif
                        #else
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444 =
                            (prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorH)
                         || (prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CbCompFactorV);
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444 =
                            (prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorH != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorH)
                         || (prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1YCompFactorV != prImgreszInst->tImgReszSrcBufInfo.rCompFactor.u1CrCompFactorV);
                        if(prImgreszInst->tImgReszPartialBufInfo.u4YBufLine == 0)
                            prImgreszInst->tImgReszPartialBufInfo.u4YBufLine = 16;
                        // Allocate temp buffer
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa =
#if CONFIG_SYS_MEM_PHASE2
                                (UINT32)x_alloc_aligned_dma_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,32);
#elif CONFIG_SYS_MEM_PHASE3
                                (UINT32)x_mem_aligned_alloc(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,32);
#endif                          
                            #else
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cb444Sa =
                                (UINT32)x_alloc_aligned_nc_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,16);
                            #endif
                                //(UINT32)x_alloc_aligned_nc_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                //                               prImgreszInst->tImgReszSrcBufInfo.u4BufHeight,16);
                        if(ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444)
                            #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa =
                                (UINT32)x_alloc_aligned_dma_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,32);
#elif CONFIG_SYS_MEM_PHASE3
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa =
                                (UINT32)x_mem_aligned_alloc(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,32);
#endif
                            #else
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4Cr444Sa =
                                (UINT32)x_alloc_aligned_nc_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                                               prImgreszInst->tImgReszPartialBufInfo.u4YBufLine,16);
                            #endif
                         #endif
                                //(UINT32)x_alloc_aligned_nc_mem(prImgreszInst->tImgReszSrcBufInfo.u4BufWidth *
                                //                               prImgreszInst->tImgReszSrcBufInfo.u4BufHeight,16);
                        // State machine state initialize
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444Done = !ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCbTo444;
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444Done = !ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgCrTo444;
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        if(prImgreszInst->ptImgReszPartialBufInfo->fgFirstRow)
                        #else
                        if(prImgreszInst->tImgReszPartialBufInfo.fgFirstRow)
                        #endif
                            ptImgReszHwInst->rYCbCrToAYUVWorkAround.u4LineCnt = 0;
                    } else
                    {
                        ptImgReszHwInst->rYCbCrToAYUVWorkAround.fgYCbCrToAYUVWorkAround = FALSE;
                    }
#endif                    
                    if(prImgreszInst->eImgReszScaleMd == IMGRESZ_DRV_PARTIAL_SCALE)
                    {
                        #if IMGRESZ_MEMORY_DYNAMIC_ALLOC
                        if(prImgreszInst->ptImgReszPartialBufInfo->fgFirstRow)
                        #else
                        if(prImgreszInst->tImgReszPartialBufInfo.fgFirstRow)
                        #endif
                            prImgreszInst->u4PartialRowCnt = 0;
                        else
                            prImgreszInst->u4PartialRowCnt++;
                    }
                    vImgReszHwInstResz((UINT32)(ptImgReszHwInst->u2ImgReszHwId),prImgreszInst);
                }
            }
        }
    }

    ptImgReszHwInst->fgThreadFinish = TRUE;
}

INT32 i4ImgResz_Drv_Init(void)
{
    INT32   i4Ret = 1;
    INT32   i4;
    UINT32  u4_thread_id;

    if (_fgImgreszInit)
    {
        return 0;
    }
    
    // HAL boot init
    i4ImgResz_HAL_Boot_Init();

    //zero instance data
    vImgreszSetMemory((void *)_arImgreszHwInst,0, sizeof(_arImgreszHwInst));


    for (i4=0; i4<IMGRESZ_HW_INST_NUM; i4++)
    {
        _arImgreszHwInst[i4].u2ImgReszCompId = IMGRESZ_INVALID16;
        _arImgreszHwInst[i4].fgImgReszActive = FALSE;
    }

    //create event
    i4Ret = x_ev_group_create(&_arImgreszHwInst[0].hEventHandle, "ImgReszEv0", IMGRESZ_EV_INITIAL);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_ev_group_create(&_arImgreszHwInst[1].hEventHandle, "ImgReszEv1", IMGRESZ_EV_INITIAL);
    VERIFY(i4Ret == OSR_OK);

    // Instance init
    vImgResz_Inst_Init();

    //create imgresz thread
    u4_thread_id = 0;
    i4Ret = x_thread_create (&h_imgresz_thread_0,
                              IMGRESZ0_THREAD_NAME,
                              IMGRESZ_STACK_SIZE,     /* stack size */
                              IMGRESZ_THREAD_PRIORITY,      /* priority */
                              vImgReszHwInstMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);
    u4_thread_id = 1;
    i4Ret = x_thread_create (&h_imgresz_thread_1,
                              IMGRESZ1_THREAD_NAME,
                              IMGRESZ_STACK_SIZE,     /* stack size */
                              IMGRESZ_THREAD_PRIORITY,      /* priority */
                              vImgReszHwInstMain,
                              sizeof(UINT32),
                              (VOID*)&u4_thread_id);
    VERIFY(i4Ret == OSR_OK);

    #if CONFIG_SUSPEND_TO_DRAM
    register_pm_ops(&imgresz_pm_ops);
    #endif
    _fgImgreszInit = TRUE;
    return i4Ret;
}


INT32 i4ImgResz_DrvUninit(UINT32 u4Case)
{
    INT32   i4Ret;

    if (!_fgImgreszInit)
    {
        return 0;
    }
    // Destroy thread
    i4Ret = x_ev_group_set_event(_arImgreszHwInst[0].hEventHandle, IMGRESZ_EV_FINISH, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);        

    i4Ret = x_ev_group_set_event(_arImgreszHwInst[1].hEventHandle, IMGRESZ_EV_FINISH, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);        

    // Wait thread finish
    while(!_arImgreszHwInst[0].fgThreadFinish)
    {
        x_thread_delay(1);
    }

    while(!_arImgreszHwInst[1].fgThreadFinish)
    {
        x_thread_delay(1);
    }

    // Instance uninit
    vImgResz_Inst_Uninit();


    //Delete event
    i4Ret = x_ev_group_delete(_arImgreszHwInst[0].hEventHandle);
    VERIFY(i4Ret == OSR_OK);
    
    i4Ret = x_ev_group_delete(_arImgreszHwInst[1].hEventHandle);
    VERIFY(i4Ret == OSR_OK);

    // HAL boot uninit
    i4ImgResz_HAL_Boot_Uninit();

    _fgImgreszInit = FALSE;

    #if CONFIG_SUSPEND_TO_DRAM
    unregister_pm_ops(&imgresz_pm_ops);
    #endif

    return i4Ret;
}

INT32 i4ImgResz_Drv_Uninit(void)
{
    i4ImgResz_DrvUninit(0);
    return 0;
}

#if CONFIG_SUSPEND_TO_DRAM

int i4ImgResz_Suspend(void *param)
{

   vHwImgReszDisable(0);
   vHwImgReszDisable(1);
    return 0;
}


int i4ImgResz_Resume(void *param)
{

  vHwImgReszEnable(0);
  vHwImgReszEnable(1);
  return 0;
}
#endif


