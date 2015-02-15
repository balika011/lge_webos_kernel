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
#include "x_drv_map.h"
#include "x_ckgen.h"
#include "x_os.h"
#include "x_bim.h"
//#include "x_hal_1176.h"
#include "imgresz_hal_if.h"
#include "imgresz_hal_errcode.h"
#include "imgresz_hal.h"
#include "drv_config.h"
//#include "chip_ver.h"
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) //chunze
#include "x_iommu.h"
#endif
#include "x_timer.h"
#include "x_debug.h"
#include "x_printf.h"
IMGRESZ_HAL_INFO_T _rImgReszHalInfo[HW_IMGRESZ_NUM];   // HAL local information for each HW.

#if (CONFIG_DRV_VERIFY_SUPPORT)    //emulation or verify define
    IMGRESZ_HAL_HW_STATUS_T _gImgresz_HwStatus_ResumeBreak = {0};
#endif

#define IMGRESZ_CACHEDRAME_EN 1

#if IMGRESZ_CACHEDRAME_EN

#if 0
#define IMGRESZ_BSP_DMA_MAP_UNMAP  1
#else //follow DTV
//#include <cache_operation.h>
#include "x_hal_arm.h"
#define IMGRESZ_BSP_DMA_MAP_UNMAP 0
#endif


UINT32 i4ImgreszCacheMap(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode)
{
#if IMGRESZ_BSP_DMA_MAP_UNMAP
   switch(i4DirMode)
   {
   	case IMGRZ_TO_DEVICE:
		BSP_dma_map_vaddr(i4Start, u4Len, TO_DEVICE);
		break;
	case IMGRZ_FROM_DEVICE:
		BSP_dma_map_vaddr(i4Start, u4Len, FROM_DEVICE);
		break;
	case IMGRZ_BIDIRECTIONAL:
		BSP_dma_map_vaddr(i4Start, u4Len, BIDIRECTIONAL);
		break;
	default:
		break;
   }
#else
   switch(i4DirMode)
   {
   	case IMGRZ_TO_DEVICE:
		HalFlushDCacheMultipleLine(i4Start, u4Len);
		break;
	case IMGRZ_FROM_DEVICE:
		HalInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	case IMGRZ_BIDIRECTIONAL:
		HalFlushInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	default:
		break;
   }
#endif
}

UINT32 i4ImgreszCacheUnMap(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode)
{
#if IMGRESZ_BSP_DMA_MAP_UNMAP
   switch(i4DirMode)
   {
   	case IMGRZ_TO_DEVICE:
	case IMGRZ_FROM_DEVICE:
	case IMGRZ_BIDIRECTIONAL:
		BSP_dma_unmap_vaddr(i4Start, u4Len, i4DirMode);
		break;
	default:
		break;
   }
#else
   switch(i4DirMode)
   {
   	case IMGRZ_TO_DEVICE:
		break;
	case IMGRZ_FROM_DEVICE:
	case IMGRZ_BIDIRECTIONAL:
		HalInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	default:
		break;
   }
#endif
}


#endif

#define IOMMU_BASE      0xf0068000

#define IOMMU_READ32(offset, bank)          IO_READ32(IOMMU_BASE+bank, offset)
#define IOMMU_WRITE32(offset, bank, value)  IO_WRITE32(IOMMU_BASE+bank, offset, (value))


//extern int UTIL_Printf(const CHAR *format, ...);
//extern void vPngSetClockOn(void);
//extern void vPngSetClockOff(void);

//extern BOOL CKGEN_SetPLL(SRC_CK_T eSource, UINT32 u4Clock0, UINT32 u4Clock1);
extern BOOL CKGEN_AgtOnClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtOffClk(e_CLK_T eAgt);
extern BOOL CKGEN_AgtSelClk(e_CLK_T eAgt, UINT32 u4Sel);
extern UINT32 CKGEN_AgtGetClk(e_CLK_T eAgt);


#define IMGRESZ_SUPPORT_AGENT_ON_OFF 0

#if IMGRESZ_IO_MMU_TEST
  //extern void vIIOMMU_Init(void);
  //extern UINT32 u4HalGetTTB1(void);
  UINT32 u4IOMMU_RESZ[HW_IMGRESZ_NUM] = {0xc0,0x140,0x1c0};
#endif

extern int rand(void);
extern void srand(int seed);

#if IMGRESZ_IO_MMU_TEST

/* extern void  vIOMMU_OverRead_Protection(UINT32 ui4_Agent,
                                        UINT32 ui4_Mode,
                                        UINT32 ui4_StartAddr0,
                                        UINT32 ui4_Buff_sz0,
                                        UINT32 ui4_StartAddr1,
                                        UINT32 ui4_Buff_sz1);

extern void x_thread_delay(UINT32 ui4_delay);  */

void vImgresz_MMUSet_After_Resize(UINT32 u4ImgReszID )
{
        IOMMU_WRITE32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID],
                      (IOMMU_READ32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID]) | 0x80000000));
        IOMMU_WRITE32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID],
                      (IOMMU_READ32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID]) | 0x80800));
        while((IOMMU_READ32(REG_RW_IOMMU_CFG8, u4IOMMU_RESZ[u4ImgReszID]) & 0x20000000) != 0);
}

void vImgreszSetMMU(UINT32 u4ImgReszID )
{
     UINT32 u4RegVal = 0;

	 IOMMU_WRITE32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID], REG_IOMMU_CFG4_RST_ALL);  ////reset all
     //vIIOMMU_Init();
     {
        IOMMU_WRITE32(REG_RW_IOMMU_CFG4, u4IOMMU_RESZ[u4ImgReszID], 0x0010010A);//REG_IOMMU_CFG4_EN);0x0030010A);
     }
     //enable mmu

    u4RegVal = 0xC00000FE|(0x1<<0); //osd partial mode

     IOMMU_WRITE32(REG_RW_IOMMU_CFG0, u4IOMMU_RESZ[u4ImgReszID], u4RegVal);
    u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG0, u4IOMMU_RESZ[u4ImgReszID]);
//     UTIL_Printf("[IMGRESZ_VFY] MMU CFG0 is %x . \n",u4RegVal);

     IOMMU_WRITE32(REG_RW_IOMMU_CFG1, u4IOMMU_RESZ[u4ImgReszID], u4HalGetTTB1());
    //     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG1, IOMMU_RESZ);
    Printf("[IMGRESZ_VFY] MMU CFG1 is 0x%x . \n",u4HalGetTTB1());

    IOMMU_WRITE32(REG_RW_IOMMU_CFG2, u4IOMMU_RESZ[u4ImgReszID], 0x00130011); //mt8580

    IOMMU_WRITE32(REG_RW_IOMMU_CFG3, u4IOMMU_RESZ[u4ImgReszID], 0x00000000);
// flush after mmu register setting

  Printf("[IMGRESZ_VFY] MMU CFG0 is 0x%x . \n",IOMMU_READ32(REG_RW_IOMMU_CFG0,u4IOMMU_RESZ[u4ImgReszID]));
  Printf("[IMGRESZ_VFY] MMU CFG1 is 0x%x . \n",IOMMU_READ32(REG_RW_IOMMU_CFG1,u4IOMMU_RESZ[u4ImgReszID]));
  Printf("[IMGRESZ_VFY] MMU CFG2 is 0x%x . \n",IOMMU_READ32(REG_RW_IOMMU_CFG2,u4IOMMU_RESZ[u4ImgReszID]));
  Printf("[IMGRESZ_VFY] MMU CFG3 is 0x%x . \n",IOMMU_READ32(REG_RW_IOMMU_CFG3,u4IOMMU_RESZ[u4ImgReszID]));
  Printf("[IMGRESZ_VFY] MMU CFG4 is 0x%x . \n",IOMMU_READ32(REG_RW_IOMMU_CFG4,u4IOMMU_RESZ[u4ImgReszID]));


}

void vImgreszSetUserTable(UINT32 u4ImgReszID )

{

    IOMMU_WRITE32(REG_RW_IOMMU_CFG1, u4IOMMU_RESZ[u4ImgReszID], u4HalGetTTB1());

}
#endif
#include "x_debug.h"
void vHwImgReszClk(void)
{
	#if 0//
   UINT32 u4SysClock = 3 ;//200
   IO_WRITE32(CKGEN_BASE, 0x3c4, u4SysClock);
   UTIL_Printf("[img] Vfy clock[0x%x]\n",u4SysClock);
#endif
}

#if  IMGRESZ_PERFORMANCE_TEST
extern HAL_TIME_T _rImgreszEnd, _rImgreszTime, _rImgreszStart;
#endif

void vImgResz_HAL_ISR(UINT16 vector_id)
{
    UINT32 u4ImgReszID = 0;
    BIM_ClearIrq(vector_id);

    if(vector_id == VECTOR_IMGRZ)
        u4ImgReszID = 0;
    else if(vector_id == VECTOR_IMGRZ2)
        u4ImgReszID = 1;
#if defined(CC_MT5890)
    else if(vector_id == VECTOR_IMGRZ3)
        u4ImgReszID = 2;
#endif
    #if IMGRESZ_PERFORMANCE_TEST
        HAL_GetTime(&_rImgreszEnd);
        HAL_GetDeltaTime(&_rImgreszTime, &_rImgreszStart, &_rImgreszEnd);
        UTIL_Printf ("[IMG_RESZ] ISR Video Resize finish time=%d.%d\n",
               _rImgreszTime.u4Seconds,_rImgreszTime.u4Micros);
    #endif
    _rImgReszHalInfo[u4ImgReszID].fgResizeComplete = TRUE;
    _rImgReszHalInfo[u4ImgReszID].fgScaling = FALSE;
    // Callback
    if(_rImgReszHalInfo[u4ImgReszID].rNofifyCallback.pvCallBackFunc != NULL)
        _rImgReszHalInfo[u4ImgReszID].rNofifyCallback.pvCallBackFunc(0,
            _rImgReszHalInfo[u4ImgReszID].rNofifyCallback.pvPrivData);

}


/// Initialize Image Resizer HAL when boot up
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Boot_Init(
    void
)
{
    x_os_isr_fct pf_old_isr = NULL;
    x_reg_isr(VECTOR_IMGRZ,vImgResz_HAL_ISR,&pf_old_isr);
    x_reg_isr(VECTOR_IMGRZ2,vImgResz_HAL_ISR,&pf_old_isr);
#if defined(CC_MT5890)
    x_reg_isr(VECTOR_IMGRZ3,vImgResz_HAL_ISR,&pf_old_isr);
#endif
    // Image resizer and Graphics use the same clock setting,
    // so the clock is set by Graphics driver.
    // But, image resizer should check clock itself for verification.
    vHwImgReszClk();

    return S_IMGRESZ_HAL_OK;
}


/// Uninitialize Image Resizer HAL when boot down
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Boot_Uninit(
    void
)
{
    x_os_isr_fct pf_old_isr = NULL;
    x_reg_isr(VECTOR_IMGRZ,NULL,&pf_old_isr);
    x_reg_isr(VECTOR_IMGRZ2,NULL,&pf_old_isr);

    return S_IMGRESZ_HAL_OK;
}


/// Initialize Image Resizer HAL
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Init(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    //vPngSetClockOn();
#if IMGRESZ_SUPPORT_AGENT_ON_OFF
    vHwImgReszClk();
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
    CKGEN_AgtOnClk(e_CLK_RESZ);
#else
    CKGEN_AgtOnClk(e_CLK_PNG);
#endif
#endif


#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    vHwImgRezeReset(u4ImgReszID);
    vHwImgReszEnable(u4ImgReszID);
#else
    vHwImgReszEnable(u4ImgReszID);
#if IMGRESZ_IO_MMU_TEST
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
    vHwImgReszEnableMMU(u4ImgReszID);
#endif
#endif
    vHwImgRezeReset(u4ImgReszID);
#endif
    i4HwImgReszSetDramReqBurstLimit(u4ImgReszID,8); // Set Dram Request Burst Limit.0
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    vHwImgIntResize(u4ImgReszID);//sun for turning on  interrupt
    #endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)//set wait dram write done
    vHwImgRezeSetWaitWRDone(u4ImgReszID);
#endif

    x_memset(&(_rImgReszHalInfo[u4ImgReszID]),0,sizeof(IMGRESZ_HAL_INFO_T));

#if !IMGRESZ_IOMMU_SOLUTION2_SOLUTION3
#if IMGRESZ_IO_MMU_TEST
   // if (u4ImgReszID == 0)
		  {

			 vImgreszSetMMU(u4ImgReszID);
		 if (_rImgReszHalInfo[u4ImgReszID].fgUserTable)
		    vImgreszSetUserTable(u4ImgReszID);
		  }
#endif
#endif
    // Variable initialization
    _rImgReszHalInfo[u4ImgReszID].rJpegInfo.fgYExist = TRUE;
    _rImgReszHalInfo[u4ImgReszID].rJpegInfo.fgCbExist = TRUE;
    _rImgReszHalInfo[u4ImgReszID].rJpegInfo.fgCrExist = TRUE;
    _rImgReszHalInfo[u4ImgReszID].rBldImgInfo.u1Alpha= 0xFF;
#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
    _rImgReszHalInfo[u4ImgReszID].fgResetDstBuf=FALSE;
#endif
   _rImgReszHalInfo[u4ImgReszID].fgScale1to1=FALSE;
   _rImgReszHalInfo[u4ImgReszID].fgScale4to1=FALSE;
   _rImgReszHalInfo[u4ImgReszID].fgLumaKeyEnable=FALSE;
    return S_IMGRESZ_HAL_OK;
}


/// Uninitialize Image Resizer HAL
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Uninit(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    //vPngSetClockOff();
	if(u4ImgReszID == 1)
	{
        UTIL_Printf("imgresz performance == 0x%x ===\n",*(volatile UINT32 *)0xf006c220);
	}

#if IMGRESZ_IO_MMU_TEST
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
   // if (u4ImgReszID == 0)
        vImgresz_MMUSet_After_Resize(u4ImgReszID);
    #endif
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
        vHwImgReszDisableMMU(u4ImgReszID);
    #endif
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    vHwImgRezeReset(u4ImgReszID);
#endif
    vHwImgReszDisable(u4ImgReszID);
#if IMGRESZ_SUPPORT_AGENT_ON_OFF
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
    CKGEN_AgtOffClk(e_CLK_RESZ);
#else
    CKGEN_AgtOffClk(e_CLK_PNG);
#endif
#endif

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
/// Gracefully reset
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_Gracefully_Reset(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;


    vHwImgRezeDMAReset(u4ImgReszID);
    while(TRUE)
    {
      if(fgHwImgReszDMARstFinish(u4ImgReszID))
        break;
    }

    vHwImgRezeReset(u4ImgReszID);

    return S_IMGRESZ_HAL_OK;
}
#endif

/// Set Image Resizer HAL resize mode
/// \return If return value < 0, it's failed. Please reference imgresz_hal_errcode.h.
INT32 i4ImgResz_HAL_Set_Resize_Mode(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RESIZE_MODE_T eResizeMode            ///< [IN] Resize mode
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].eResizeMode = eResizeMode;

    return S_IMGRESZ_HAL_OK;
}


/// Set Image Resizer HAL resample method
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Resample_Method(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod,  ///< [IN] Horizontal Resample method
    IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod   ///< [IN] Vertical Resample method
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].eHResampleMethod = eHResampleMethod;
    _rImgReszHalInfo[u4ImgReszID].eVResampleMethod = eVResampleMethod;

    return S_IMGRESZ_HAL_OK;
}


/// Set Image Resizer HAL source image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Source_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prSrcImgInfo             ///< [IN] Source image infomation.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rSrcImgInfo),prSrcImgInfo,sizeof(IMGRESZ_HAL_IMG_INFO_T));

    return S_IMGRESZ_HAL_OK;
}


/// Set Image Resizer HAL destination image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Destination_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prDestImgInfo            ///< [IN] Destination image infomation.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rDestImgInfo),prDestImgInfo,sizeof(IMGRESZ_HAL_IMG_INFO_T));

    return S_IMGRESZ_HAL_OK;
}

INT32 i4ImgResz_HAL_Set_4to1_Scale_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgSkip            ///< [IN] Destination image infomation.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgScale4to1 = fgSkip;

    return S_IMGRESZ_HAL_OK;
}

/// Set Image Resizer HAL blending image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Blending_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prBldImgInfo             ///< [IN] Blending image infomation.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rBldImgInfo),prBldImgInfo,sizeof(IMGRESZ_HAL_IMG_INFO_T));

    return S_IMGRESZ_HAL_OK;
}


/// Set Image Resizer HAL partial mode information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Partial_Mode_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_PARTIAL_BUF_INFO_T *prSrcRowBufInfo, ///< [IN] Source row buffer infomation.
    UINT32 u4TempBufSa                               ///< [IN] The start address of temporary buffer for partial mode.
                                                     ///<      The size of temp buffer is destination image width * 1 bytes.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rSrcRowBufInfo),prSrcRowBufInfo,sizeof(IMGRESZ_HAL_PARTIAL_BUF_INFO_T));
    _rImgReszHalInfo[u4ImgReszID].u4TempBufSa = u4TempBufSa;

    return S_IMGRESZ_HAL_OK;
}

#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
INT32 i4ImgResz_HAL_Set_DstBuf_Reset(
    UINT32 u4ImgReszID,
    BOOL fgReset
)
{
    if (u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgResetDstBuf = fgReset;

    return S_IMGRESZ_HAL_OK;
}
#endif

/// Set Image Resizer HAL Jpeg information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_Jpeg_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_JPEG_INFO_T *prJpegInfo              ///< [IN] Jpeg information.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rJpegInfo),prJpegInfo,sizeof(IMGRESZ_HAL_JPEG_INFO_T));

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
/// Set Image Resizer HAL RM(RPR) information
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_RM_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_RM_INFO_T *prRMInfo              ///< [IN] rm information.
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rRMInfo),prRMInfo,sizeof(IMGRESZ_HAL_RM_INFO_T));

    return S_IMGRESZ_HAL_OK;
}
#endif


INT32 i4ImgResz_HAL_Set_LumaKey(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable,                                   ///< [IN] Luma key enable flag
    UINT8 u1LumaKey                                  ///< [IN] Luma key value
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgLumaKeyEnable = fgEnable;
    _rImgReszHalInfo[u4ImgReszID].u1LumaKey = u1LumaKey;

    return S_IMGRESZ_HAL_OK;
}

INT32 i4ImgResz_HAL_Set_IOMMUTable(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgUserTable                                  ///<
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgUserTable = fgUserTable;

    return S_IMGRESZ_HAL_OK;
}
//sun for encoder new application
INT32 i4ImgResz_HAL_Set_Scale1to1(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable                                 ///< [IN] scale1:1 enable flag
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgScale1to1= fgEnable;

    return S_IMGRESZ_HAL_OK;
}

//sun for encoder new application
INT32 i4ImgResz_HAL_Set_Scale4to1(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    BOOL fgEnable                                 ///< [IN] scale4:1 enable flag
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgScale4to1= fgEnable;

    return S_IMGRESZ_HAL_OK;
}

extern void vHwImgReszPrintRegister(UINT32 u4HwId);

#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP

/// Set Image Resizer HAL PreRow image info.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_PreRow_Image_Info(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_IMG_INFO_T *prPreRowImgInfo,            ///< [IN] Destination image infomation.
    UINT32 u4RowHeight,
    BOOL   fgWebpEnable
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    x_memcpy(&(_rImgReszHalInfo[u4ImgReszID].rWebpPartialInfo.rPreSrcImgInfo),prPreRowImgInfo,sizeof(IMGRESZ_HAL_IMG_INFO_T));
    _rImgReszHalInfo[u4ImgReszID].rWebpPartialInfo.u4RowHeight = u4RowHeight;
    _rImgReszHalInfo[u4ImgReszID].fgWebpEnable = fgWebpEnable;
    return S_IMGRESZ_HAL_OK;
}

INT32 i4ImgResz_HAL_Prepare_VideoPartialMode(UINT32 u4ImgReszID)
{
    IMGRESZ_HAL_INFO_T *prImgReszHalInfo;
    UINT32  u4SrcRowStartY = 0, u4SrcRowEndY = 0;//
    //UINT32  u4SrcRowEndC = 0;//u4SrcRowStartC = 0,
    UINT32 u4TargetStartY = 0, u4TargetEndY = 0;
    UINT32 u4TargetHeightY = 0;
    UINT32 u4CurYRowLines = 0;
    UINT32 u4CurCRowLines = 0;
    UINT32 u4SrcPos, u4DestPos;
    INT32 u4Y;
    UINT32 u4Tmp = 0;
    prImgReszHalInfo = &(_rImgReszHalInfo[u4ImgReszID]);

    if(prImgReszHalInfo->rWebpPartialInfo.u4RowCnt == 0)//first row
    {
        prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY =
                          (0x40000 * prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight +
                          (prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight>>1))/prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight; // same as video mode
        prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightC =
                          (0x40000 * (prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight>>1) +
                          (prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight>>2))/(prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight>>1);  //
        prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines = 0;
        prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines= 0;
        //prImgReszHalInfo->rWebpPartialInfo.u4RowHeight = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight;
        u4TargetStartY = 0;
        u4CurYRowLines = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight;
        u4CurCRowLines = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight/2;
    }
    else
    {
        u4CurYRowLines = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight + prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines;
        u4CurCRowLines = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight/2 + prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines;

         //UTIL_Printf("[ImgResz][Verify] borrowYlines= %d\n", prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines);
         if(prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines>0)
         {
             //Move current Row Buf
             for(u4Y=(prImgReszHalInfo->rSrcImgInfo.u4ImgHeight-1); u4Y>=0; u4Y--)
             {
                 u4SrcPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA1 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4Y) * prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
                 u4DestPos= prImgReszHalInfo->rSrcImgInfo.u4BufSA1 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4Y + prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines) *
                     prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
                 x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
             }
             //Combine current row buf with pre row buf
             for(u4Y= 0;u4Y<prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines;u4Y++ )//Y buf
             {
                 u4SrcPos= prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufSA1 +
                     (prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgYOff + u4Y) *
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgXOff;
                 u4DestPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA1 +
                     (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff + u4Y) *
                       prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;

                 x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
             }

         }
         if(prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines>0)
         {
             for(u4Y = prImgReszHalInfo->rSrcImgInfo.u4ImgHeight/2-1; u4Y >=0; u4Y-- )
             {
                 u4SrcPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA2 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4Y) * prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
                 u4DestPos= prImgReszHalInfo->rSrcImgInfo.u4BufSA2 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4Y + prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines) *
                     prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
                 x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
             }
             for(u4Y= 0;u4Y<prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines;u4Y++ )//C buf
             {
                 u4SrcPos= prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufSA2 +
                     (prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgYOff + u4Y) *
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgXOff;
                 u4DestPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA2 +
                     (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff + u4Y ) *
                       prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;

                x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
             }

         }

         u4TargetStartY = prImgReszHalInfo->rDestImgInfo.u4ImgYOff +
                          prImgReszHalInfo->rDestImgInfo.u4ImgHeight;
    }
    if(((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) * prImgReszHalInfo->rWebpPartialInfo.u4RowHeight)
         >= prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight)
    {
        u4TargetEndY = prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight-1;
		u4SrcRowEndY = prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight-1;
		//u4SrcRowEndC = prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight>>1;
    }
    else
    {

        u4TargetEndY = (0x40000 * (prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) *
                        prImgReszHalInfo->rWebpPartialInfo.u4RowHeight) /
                        prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY - 1;
		u4SrcRowEndY = (u4TargetEndY * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY) /
		                0x40000 + 2;
		while (u4SrcRowEndY  >= ((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) *
		        prImgReszHalInfo->rWebpPartialInfo.u4RowHeight))  // src_end cannot exceed row boundary
        {
             u4TargetEndY = u4TargetEndY -1;
             u4SrcRowEndY = (u4TargetEndY * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY) /
                             0x40000 + 2;
        }
        u4TargetHeightY = u4TargetEndY - u4TargetStartY + 1;
        if(u4TargetHeightY % 2 == 1)
        {
             u4TargetEndY = u4TargetEndY - 1;
             u4TargetHeightY = u4TargetEndY - u4TargetStartY + 1;
             u4SrcRowEndY = (u4TargetEndY * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY) /
                             0x40000 + 2;
        }
        //u4SrcRowEndC = (u4TargetEndY/2 * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightC) /
        //                0x40000 + 2;
     }
     u4TargetHeightY = u4TargetEndY - u4TargetStartY + 1;

     if(prImgReszHalInfo->rWebpPartialInfo.u4RowCnt > 0)//Not First row
     {
         u4SrcRowEndY = u4SrcRowEndY - ((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt) *
		        prImgReszHalInfo->rWebpPartialInfo.u4RowHeight) +
		        prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines;
		 //u4SrcRowEndC = u4SrcRowEndC - ((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt) *
		 //       prImgReszHalInfo->rWebpPartialInfo.u4RowHeight/2) +
		 //       prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines;
     }
     //Setting and trigger resize
     //to do
     prImgReszHalInfo->rSrcImgInfo.u4ImgYOff = 0;
     prImgReszHalInfo->rSrcImgInfo.u4ImgHeight = u4SrcRowEndY + 1;
     prImgReszHalInfo->rDestImgInfo.u4ImgYOff = u4TargetStartY;
     prImgReszHalInfo->rDestImgInfo.u4ImgHeight = u4TargetHeightY;
     //calculate lines that have to combine with next row
     if(((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) *
          prImgReszHalInfo->rWebpPartialInfo.u4RowHeight)
         < prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight)//Not last row
     {
         // calculate filter offset for next row
         u4TargetStartY = u4TargetEndY + 1;
         u4Tmp = u4TargetStartY * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightY;
         u4SrcRowStartY = u4Tmp / 0x40000 - 1;
         //u4SrcRowY4tapsOffset = u4Tmp & 0x3ffff + 0x40000;
         //u4Tmp = u4TargetStartY/2 * prImgReszHalInfo->rWebpPartialInfo.u4RatioHeightC;
         //u4SrcRowStartC = u4Tmp / 0x40000 - 1;

         prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines =
                            ((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) *
             prImgReszHalInfo->rWebpPartialInfo.u4RowHeight) - u4SrcRowStartY ;

         //prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines =
        //                    ((prImgReszHalInfo->rWebpPartialInfo.u4RowCnt+1) *
        //     (prImgReszHalInfo->rWebpPartialInfo.u4RowHeight>>1)) - u4SrcRowStartC ;
         if(prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines%2 == 0)
         {
             prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines =
                 prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines/2;
         }else
         {
             prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines =
                 prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines/2+1;
         }
         if(prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines<0)
             prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines=0;
         if(prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines<0)
             prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines=0;

         //backup current row info
         for(u4Y= 0;u4Y<prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines;u4Y++ )//Y buf
         {
             u4SrcPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA1 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4CurYRowLines-(prImgReszHalInfo->rWebpPartialInfo.u4BorrowYLines-u4Y)) *
                     prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
             u4DestPos= prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufSA1 +
                     (prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgYOff + u4Y) *
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgXOff;
             x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
         }

         for(u4Y= 0;u4Y<prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines;u4Y++ )//C buf
         {

             u4SrcPos = prImgReszHalInfo->rSrcImgInfo.u4BufSA2 + (prImgReszHalInfo->rSrcImgInfo.u4ImgYOff +
                     u4CurCRowLines-(prImgReszHalInfo->rWebpPartialInfo.u4BorrowCLines-u4Y) ) * prImgReszHalInfo->rSrcImgInfo.u4BufWidth +
                     prImgReszHalInfo->rSrcImgInfo.u4ImgXOff;
             u4DestPos= prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufSA2 +
                     (prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgYOff + u4Y) *
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4BufWidth +
                       prImgReszHalInfo->rWebpPartialInfo.rPreSrcImgInfo.u4ImgXOff;
             x_memcpy((void *)u4DestPos,(void *)u4SrcPos,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth);
        }

     }
     prImgReszHalInfo->rWebpPartialInfo.u4RowCnt++;

     return S_IMGRESZ_HAL_OK;
}

#endif

extern  BOOL fgSameBuf;
/// Image Resizer HAL do resize.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Resize(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)
{
    IMGRESZ_HAL_INFO_T *prImgReszHalInfo;

    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;
#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
    if(_rImgReszHalInfo[u4ImgReszID].fgWebpEnable)
    {
        i4ImgResz_HAL_Prepare_VideoPartialMode(u4ImgReszID);
    }
#endif
    prImgReszHalInfo = &(_rImgReszHalInfo[u4ImgReszID]);


#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3
    #if IMGRESZ_IO_MMU_TEST
     // if (u4ImgReszID == 0)
      {
         // _Imgresz_ResizeMode = eResizeMode;
          vImgreszSetMMU(u4ImgReszID);
		 if (prImgReszHalInfo->fgUserTable)
		 	vImgreszSetUserTable(u4ImgReszID);
      }
   #endif
#endif

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    //0. Set RM mode to HW
    if (prImgReszHalInfo->rRMInfo.fgRPRMode)
    {
        if(prImgReszHalInfo->rRMInfo.fgRPRRacingModeEnable)
	       i4HwImgReszSetRPR(u4ImgReszID,TRUE,TRUE);
	    else
	       i4HwImgReszSetRPR(u4ImgReszID,TRUE,FALSE);
    }
    else
	    i4HwImgReszSetRPR(u4ImgReszID,FALSE,FALSE);
#endif

    if((prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_FRAME) ||
       ((prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL) &&
        (prImgReszHalInfo->rSrcRowBufInfo.fgFirstRowBuf || prImgReszHalInfo->fgResume)))
    {

        // 1. Set Resize Mode to HW
        i4HwImgReszSetDstPitchEnable(u4ImgReszID,
                                     prImgReszHalInfo->rDestImgInfo.fgPitchEn,
                                     prImgReszHalInfo->rDestImgInfo.u4BufWidth_Byte,
                                     prImgReszHalInfo->rDestImgInfo.u4BufAddr_Byte
                                     );
        i4HwImgReszSetResizeMode(u4ImgReszID,prImgReszHalInfo->eResizeMode,
                                           &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));

        // 2. Set Resample Method to HW
        i4HwImgReszSetResampleMethod(u4ImgReszID,prImgReszHalInfo->eHResampleMethod,
                                                 prImgReszHalInfo->eVResampleMethod);

        // 3. Set Source Information to HW
        i4HwImgReszSetSrcBufFormat(u4ImgReszID,&(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
        if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_FRAME)
        {
#if (!CONFIG_DRV_VERIFY_SUPPORT)  //driver code
#if IMGRESZ_IO_MMU_TEST
       /*     if(0 == u4ImgReszID)
            {
                  vIOMMU_OverRead_Protection(u4IOMMU_RESZ,
                               1,
                               prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
                               (prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight),
                               prImgReszHalInfo->rDestImgInfo.u4BufSA1,
                               (prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight));
            }            */
#endif
#endif
            i4HwImgReszSetSrcBufAddr(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
                                                 prImgReszHalInfo->rSrcImgInfo.u4BufSA2,
                                                 prImgReszHalInfo->rSrcImgInfo.u4BufSA3);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
	        if (prImgReszHalInfo->rRMInfo.fgRPRRacingModeEnable)   //sun new
	            prImgReszHalInfo->rSrcImgInfo.u4BufHeight  = 0x00000020;
            i4HwImgReszSetSrcRowBufHeight(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                                          &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                          prImgReszHalInfo->rRMInfo.fgRPRRacingModeEnable);
#endif
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)

            i4HwImgReszSetSrcRowBufHeight(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                                                                    &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
#endif
        }
        else if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL)
        {
#if (!CONFIG_DRV_VERIFY_SUPPORT)  //driver code
#if IMGRESZ_IO_MMU_TEST
            /*if(0 == u4ImgReszID && !prImgReszHalInfo->rJpegInfo.fgPictureMode)
            {
                  vIOMMU_OverRead_Protection(IOMMU_RESZ,
                               1,
                               prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA1,
                               (prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcRowBufInfo.u4RowBufHeight),
                               prImgReszHalInfo->rDestImgInfo.u4BufSA1,
                               (prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight));
            }             */
#endif
#endif
            i4HwImgReszSetSrcBufAddr(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA1,
                                                 prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA2,
                                                 prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA3);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            i4HwImgReszSetSrcRowBufHeight(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4RowBufHeight,
                                          &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),FALSE); //sun new
#else
            i4HwImgReszSetSrcRowBufHeight(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4RowBufHeight,
                                          &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
#endif

            if(prImgReszHalInfo->rJpegInfo.fgPictureMode || prImgReszHalInfo->fgResume)
                i4HwImgReszSetSrcPrevRowBufAddr(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA1,
                                                            prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA2,
                                                            prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA3);
        }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        i4HwImgReszSetSrcBufWidth(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4BufWidth, &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
#else
        i4HwImgReszSetSrcBufWidth(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4BufWidth);
#endif
#if     (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)   //80 OR 61         // (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
        if(prImgReszHalInfo->fgScale4to1)
        {
            i4HwImgReszSetSrcImageWidthHeight(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth,
                                                        (prImgReszHalInfo->rSrcImgInfo.u4ImgHeight/4),
                                                        &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
        }
        else
#endif
        {
        i4HwImgReszSetSrcImageWidthHeight(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth,
                                                        (prImgReszHalInfo->rSrcImgInfo.u4ImgHeight),
                                                      &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
        }
        i4HwImgReszSetSrcImageOffset(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgXOff,
                                                 prImgReszHalInfo->rSrcImgInfo.u4ImgYOff,
                                                 &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                                 prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                                 prImgReszHalInfo->rDestImgInfo.u4ImgHeight);
        if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_FRAME)
        {
            i4HwImgReszSetSrcFirstRow(u4ImgReszID,TRUE);

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
	      if (prImgReszHalInfo->rRMInfo.fgRPRRacingModeEnable)
                i4HwImgReszSetSrcLastRow(u4ImgReszID,FALSE);
	      else
#endif
            i4HwImgReszSetSrcLastRow(u4ImgReszID,TRUE);
        }
        else if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL)
        {
            i4HwImgReszSetSrcFirstRow(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.fgFirstRowBuf);
            i4HwImgReszSetSrcLastRow(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.fgLastRowBuf);
        }
        if(prImgReszHalInfo->rSrcImgInfo.rBufferFormat.eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER)
        {
            i4HwImgReszSetIndexBufColorTranslation(u4ImgReszID);
            i4HwImgReszSetColorPalletTable(u4ImgReszID,(UINT32)prImgReszHalInfo->rSrcImgInfo.rBufferFormat.prColorPallet);
        }

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
        if(prImgReszHalInfo->fgScale4to1)   //sun for encode new application
        {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
           i4HwImgReszSetJpegComponent(u4ImgReszID,TRUE, FALSE,FALSE);
#else
           i4HwImgReszSetJpegComponent(u4ImgReszID,TRUE, FALSE,FALSE);
#endif
        }
#endif

        // 4. Set Destination Information to HW
        i4HwImgReszSetDestBufFormat(u4ImgReszID,&(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                                &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
        i4HwImgReszSetDestBufAddr(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4BufSA1,
                                              prImgReszHalInfo->rDestImgInfo.u4BufSA2);
        i4HwImgReszSetDestBufWidth(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4BufWidth);
        i4HwImgReszSetDestImageWidthHeight(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4ImgWidth,
                                                       prImgReszHalInfo->rDestImgInfo.u4ImgHeight);
        i4HwImgReszSetDestImageOffset(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4ImgXOff,
                                                  prImgReszHalInfo->rDestImgInfo.u4ImgYOff);
        // 5. Set Blending Buffer Information to HW
        if (prImgReszHalInfo->rBldImgInfo.u1Alpha != 0xFF)
        {
            i4HwImgReszSetAlphaBlendingLevel(u4ImgReszID,(UINT32)(prImgReszHalInfo->rBldImgInfo.u1Alpha));
            UTIL_Printf("sw preload buffer:0x%x\n", prImgReszHalInfo->rBldImgInfo.u4BufSA1);
            i4HwImgReszSetPreloadBufAddr(u4ImgReszID,prImgReszHalInfo->rBldImgInfo.u4BufSA1,
                                                 prImgReszHalInfo->rBldImgInfo.u4BufSA2);
        }


        // 6. Set Scale Factor to HW
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561)//(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
        i4HwImgReszSetVENCSKIP(u4ImgReszID, FALSE);
#endif
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        if(prImgReszHalInfo->rRMInfo.fgRPRMode)
        {
            i4HwImgReszSetRPRHScaleFactor(u4ImgReszID,(INT32)prImgReszHalInfo->rSrcImgInfo.u4ImgWidth,
		  	                                            (INT32)prImgReszHalInfo->rDestImgInfo.u4ImgWidth);
	        i4HwImgReszSetRPRVScaleFactor(u4ImgReszID,(INT32)prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
		  	                                            (INT32)prImgReszHalInfo->rDestImgInfo.u4ImgHeight);
        }else
        {
#endif
            if(prImgReszHalInfo->fgScale1to1)   //sun for encode new application
            {
              i4HwImgReszSetScale1to1Factor(u4ImgReszID,
			  	                                        &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat));
            }
	     else if(prImgReszHalInfo->fgScale4to1)
	     {
              i4HwImgReszSetHScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth,
                                               (prImgReszHalInfo->rSrcImgInfo.u4ImgWidth),// (prImgReszHalInfo->rSrcImgInfo.u4ImgWidth/4),
                                               prImgReszHalInfo->eHResampleMethod,
                                               &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                               &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
              #if    (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561)// 8580 OR 8561  ///(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
              i4HwImgReszSetVENCSKIP(u4ImgReszID, TRUE);
              i4HwImgReszSetVScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                               (prImgReszHalInfo->rSrcImgInfo.u4ImgHeight),
                                               prImgReszHalInfo->eVResampleMethod,
                                               &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                               &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
              #else
              i4HwImgReszSetVScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                               (prImgReszHalInfo->rSrcImgInfo.u4ImgHeight/4),
                                               prImgReszHalInfo->eVResampleMethod,
                                               &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                               &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
              #endif
	     }
	     else
            {

            i4HwImgReszSetHScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgWidth,
                                               prImgReszHalInfo->rDestImgInfo.u4ImgWidth,
                                               prImgReszHalInfo->eHResampleMethod,
                                               &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                               &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
#if CONFIG_SYS_MEM_PHASE3
     if ((prImgReszHalInfo->rJpegInfo.fgCbExist || prImgReszHalInfo->rJpegInfo.fgCrExist) &&
         (!prImgReszHalInfo->rJpegInfo.fgYExist) && (!prImgReszHalInfo->rJpegInfo.fgPreloadMode) &&
         (!prImgReszHalInfo->rJpegInfo.fgPictureMode)
         && (prImgReszHalInfo->rSrcImgInfo.rBufferFormat.u4VSampleFactor[0] != 1))
      {
            i4HwImgReszSetVScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
            prImgReszHalInfo->rDestImgInfo.u4ImgHeight * 2,
            prImgReszHalInfo->eVResampleMethod,
            &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
            &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
     }
     else
#endif
           {
#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
         if(prImgReszHalInfo->fgWebpEnable)
         {
            i4HwImgReszSetVScaleFactorWebpTap4(u4ImgReszID,
                                         prImgReszHalInfo->rSrcImgInfo.u4ImgRealHeight, //src real size
                                         prImgReszHalInfo->rDestImgInfo.u4ImgRealHeight,//Target real size
                                         prImgReszHalInfo->rDestImgInfo.u4ImgYOff);
         }
         else
#endif
         {
            i4HwImgReszSetVScaleFactor(u4ImgReszID,prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                               prImgReszHalInfo->rDestImgInfo.u4ImgHeight,
                                               prImgReszHalInfo->eVResampleMethod,
                                               &(prImgReszHalInfo->rSrcImgInfo.rBufferFormat),
                                               &(prImgReszHalInfo->rDestImgInfo.rBufferFormat));
          }
            }
     }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        }
#endif

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

        i4HwImgReszSetTmpLineBufLen(u4ImgReszID,prImgReszHalInfo->eVResampleMethod,
                                                prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                                prImgReszHalInfo->rDestImgInfo.u4ImgHeight,
                                                prImgReszHalInfo->rDestImgInfo.u4ImgWidth,
                                                prImgReszHalInfo->rRMInfo.fgRPRMode,
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)   //(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
                                                (prImgReszHalInfo->rDestImgInfo.rBufferFormat.eBufferMainFormat==IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER
                                                ||prImgReszHalInfo->rDestImgInfo.rBufferFormat.eBufferMainFormat==IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER),
#endif
                                                prImgReszHalInfo->fgLumaKeyEnable);
#else
        i4HwImgReszSetTmpLineBufLen(u4ImgReszID,prImgReszHalInfo->eVResampleMethod,
                                                prImgReszHalInfo->rSrcImgInfo.u4ImgHeight,
                                                prImgReszHalInfo->rDestImgInfo.u4ImgHeight,
                                                prImgReszHalInfo->rDestImgInfo.u4ImgWidth);
#endif

        // 7. Jpeg info setting
        if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            ||prImgReszHalInfo->rRMInfo.fgRPRMode == TRUE //sun new
#endif
            )
        {
            i4HwImgReszSetTempBufAddr(u4ImgReszID,prImgReszHalInfo->u4TempBufSa);
        }
        i4HwImgReszSetJpegPicMode(u4ImgReszID,prImgReszHalInfo->rJpegInfo.fgPictureMode);
        i4HwImgReszSetJpegPreloadMode(u4ImgReszID,prImgReszHalInfo->rJpegInfo.fgPreloadMode);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        if (prImgReszHalInfo->rRMInfo.fgRPRRacingModeEnable) //sun new
        {
            i4HwImgReszSetJpegComponent(u4ImgReszID,TRUE, TRUE,TRUE);
            i4HwImgReszSetJpegComponentExt(u4ImgReszID, TRUE, TRUE, TRUE, TRUE);
	    }
        else
	    {
#endif
            i4HwImgReszSetJpegComponent(u4ImgReszID,prImgReszHalInfo->rJpegInfo.fgYExist,
                                                prImgReszHalInfo->rJpegInfo.fgCbExist,
                                                prImgReszHalInfo->rJpegInfo.fgCrExist);


#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            if ((prImgReszHalInfo->rJpegInfo.fgPictureMode) ||
                (prImgReszHalInfo->rSrcImgInfo.rBufferFormat.eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER))
            {
                i4HwImgReszSetJpegComponentExt(u4ImgReszID,prImgReszHalInfo->rJpegInfo.fgYExist,
                                               prImgReszHalInfo->rJpegInfo.fgCbExist,
                                               prImgReszHalInfo->rJpegInfo.fgCrExist,
                                               FALSE);
            }
			i4HwImgReszSetCbCrExit(u4ImgReszID,prImgReszHalInfo->rJpegInfo.fgCbExist,prImgReszHalInfo->rJpegInfo.fgCrExist);
	    }
#endif

        if(prImgReszHalInfo->rJpegInfo.fgPreloadMode)
        	{

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561)
   vHwImgReszSetPreloadBuf_iommu(u4ImgReszID);
#endif
            i4HwImgReszSetPreloadBufAddr(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4BufSA1,
                                                     prImgReszHalInfo->rDestImgInfo.u4BufSA2);
        	}
        // 8. Miscellaneous setting
        {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            static UINT32 u4DramBurstLimit = 16;
#else
            static UINT32 u4DramBurstLimit = 8;
#endif

#if 0//(CONFIG_DRV_VERIFY_SUPPORT)  //emulation or verify code
            if( TRUE == _rImgReszHalInfo[u4ImgReszID].fgBurstLimit)
            {
                 u4DramBurstLimit = (UINT32) _rImgReszHalInfo[u4ImgReszID].u4BurstLimit;
            }
	   //UTIL_Printf("[ImgResz][Verify] Resize Burstlimit is %d.\n",u4DramBurstLimit);
#endif

            i4HwImgReszSetDramReqBurstLimit(u4ImgReszID,u4DramBurstLimit);
        }
//#if defined(IMGRESZ_HAL_MT8530)
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

        if(prImgReszHalInfo->rDestImgInfo.rBufferFormat.fgWT == FALSE && prImgReszHalInfo->rDestImgInfo.rBufferFormat.fgWT == FALSE)
            //vHwImgReszReadBurstLength(u4ImgReszID,TRUE);
            vHwImgReszReadBurstLength(u4ImgReszID,FALSE); // for imgresz time out bug
        else
            vHwImgReszReadBurstLength(u4ImgReszID,FALSE);
#endif
//#if defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
      if (prImgReszHalInfo->fgLumaKeyEnable
         || prImgReszHalInfo->fgAlphaComposition
         || prImgReszHalInfo->rSrcImgInfo.eUfoCfgType>=IMGRESZ_UFO_CFG_TYPE_8BIT)
	  {
        UTIL_Printf("<Imgresz>Disable Burst \n");
	  vHwImgReszReadBurstLength(u4ImgReszID,FALSE);
      }
      else
      {
        BOOL fgReadBurstLengthEnable = TRUE;
#if 0 //CONFIG_DRV_VERIFY_SUPPORT
        fgReadBurstLengthEnable = (BOOL) (_rImgReszHalInfo[u4ImgReszID].fgBurstReadDisable)?FALSE:TRUE;
        UTIL_Printf("[ImgResz][Verify] Resize BurstEnable %d.limit %d %d\n",
            fgReadBurstLengthEnable , _rImgReszHalInfo[u4ImgReszID].fgBurstLimit,
             _rImgReszHalInfo[u4ImgReszID].u4BurstLimit);
#endif
        vHwImgReszReadBurstLength(u4ImgReszID,fgReadBurstLengthEnable);
      }
#endif
        //UTIL_Printf("[IMGRESZ] First resetDstbuf = %d \n",prImgReszHalInfo->fgResetDstBuf);
#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
        i4HwImgReszResetDestPartialBuf(u4ImgReszID, prImgReszHalInfo->fgResetDstBuf);
#endif

#if (!CONFIG_DRV_VERIFY_SUPPORT)  //driver code  //#ifndef IMGRESZ_HAL_EMU
        i4HwImgReszSetAlphaChangeScalingType(u4ImgReszID,0);
#endif
    } else // Partial mode and not first row
    {
        // 3. Set Source Information to HW
#if (!CONFIG_DRV_VERIFY_SUPPORT)  //driver code
        #if IMGRESZ_IO_MMU_TEST
     /*       if(0 == u4ImgReszID && !prImgReszHalInfo->rJpegInfo.fgPictureMode)
            {
                  vIOMMU_OverRead_Protection(u4IOMMU_RESZ,
                               1,
                               prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA1,
                               (prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcRowBufInfo.u4RowBufHeight),
                               prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA1,
                               (prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcRowBufInfo.u4RowBufHeight));
            }             */
        #endif
 #endif
        i4HwImgReszSetSrcBufAddr(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA1,
                                             prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA2,
                                             prImgReszHalInfo->rSrcRowBufInfo.u4CurRowBufSA3);
        i4HwImgReszSetSrcPrevRowBufAddr(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA1,
                                                    prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA2,
                                                    prImgReszHalInfo->rSrcRowBufInfo.u4PrevRowBufSA3);
        i4HwImgReszSetSrcFirstRow(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.fgFirstRowBuf);
        i4HwImgReszSetSrcLastRow(u4ImgReszID,prImgReszHalInfo->rSrcRowBufInfo.fgLastRowBuf);

#if IMGRESZ_SUPPORT_RESET_DEST_BUFFER
        //UTIL_Printf("[IMGRESZ] dst buffer1 = %x dst buffer2 = %x resetDstbuf = %d \n",
        //            prImgReszHalInfo->rDestImgInfo.u4BufSA1,
        //            prImgReszHalInfo->rDestImgInfo.u4BufSA2,
        //            prImgReszHalInfo->fgResetDstBuf);
        if (prImgReszHalInfo->fgResetDstBuf)
        {
            i4HwImgReszSetDestBufAddr(u4ImgReszID,prImgReszHalInfo->rDestImgInfo.u4BufSA1,
                                    prImgReszHalInfo->rDestImgInfo.u4BufSA2);
        }
        i4HwImgReszResetDestPartialBuf(u4ImgReszID, prImgReszHalInfo->fgResetDstBuf);
#endif
    }




    if (prImgReszHalInfo->fgLumaKeyEnable)
    {
        i4HwImgReszSetLumaKeyEnable(u4ImgReszID,TRUE);
#if (!CONFIG_DRV_VERIFY_SUPPORT)  //driver code
        i4HwImgReszSetLumaKeyScalingType(u4ImgReszID,2);
 #endif
        i4HwImgReszSetLumaKey(u4ImgReszID,prImgReszHalInfo->u1LumaKey);
    }
    else
    {
        i4HwImgReszSetLumaKeyEnable(u4ImgReszID,FALSE);
    }

    // 9. Do resize
    prImgReszHalInfo->fgResizeComplete = FALSE;

//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
    prImgReszHalInfo->fgStop = FALSE;
#endif
    prImgReszHalInfo->fgScaling = TRUE;

#if (CONFIG_DRV_VERIFY_SUPPORT)    //emulation or verify
    //To Do
    if(prImgReszHalInfo->fgResume)
    {
         i4HwImgReszSetHwStatus(u4ImgReszID, &_gImgresz_HwStatus_ResumeBreak);
    }
#endif

    prImgReszHalInfo->fgResume = FALSE;


#if !IMGRESZ_PERFORMANCE_TEST
#if(IMGRESZ_CACHEDRAME_EN )
    if(TRUE == fgSameBuf)
    {
           UTIL_Printf("<imgresz> Same Buf MAP BIDIRECTIONAL \n");
    	   i4ImgreszCacheMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
                              prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                              IMGRZ_BIDIRECTIONAL);
    }
    else
    {
        if(prImgReszHalInfo->rDestImgInfo.u4BufSA1 != 0)
        {
    	    i4ImgreszCacheMap(prImgReszHalInfo->rDestImgInfo.u4BufSA1,
 		prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight,
 		IMGRZ_FROM_DEVICE);
        /*
	 i4ImgreszCacheMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
	 		prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
	 		IMGRZ_TO_DEVICE);
	 		*/

        }else
        {
           // _u4DestBufSA1Map=0;
        }

        if(prImgReszHalInfo->rSrcImgInfo.u4BufSA1 != 0)
        {
	   i4ImgreszCacheMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
	   		prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
	   		IMGRZ_TO_DEVICE);
        }
    }


    if(prImgReszHalInfo->rSrcImgInfo.u4BufSA2 != 0)
    {
	i4ImgreszCacheMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA2,
                          prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                          IMGRZ_TO_DEVICE);
    }
    if(prImgReszHalInfo->rSrcImgInfo.u4BufSA3 != 0)
    {
	i4ImgreszCacheMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA3,
                          prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                          IMGRZ_TO_DEVICE);
    }
    if(prImgReszHalInfo->rDestImgInfo.u4BufSA2 != 0)
    {
	i4ImgreszCacheMap(prImgReszHalInfo->rDestImgInfo.u4BufSA2,
                          prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight,
                          IMGRZ_FROM_DEVICE );
    }else
    {
    //    _u4DestBufSA2Map=0;
    }
#endif

#endif
#if IMGRESZ_PERFORMANCE_TEST
    if(prImgReszHalInfo->eResizeMode == IMGRESZ_HAL_RESIZE_MODE_FRAME)
    {
	    if( prImgReszHalInfo->rSrcImgInfo.rBufferFormat.eBufferMainFormat != IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER
	      || prImgReszHalInfo->rSrcImgInfo.rBufferFormat.fgProgressiveFrame
	      ||( (!prImgReszHalInfo->rSrcImgInfo.rBufferFormat.fgProgressiveFrame)
	         && prImgReszHalInfo->rSrcImgInfo.rBufferFormat.fgTopField
	        )
	      )
	    {
	    	UTIL_Printf("<imgresz>Get trigger time\n");
	    	HAL_GetTime(&_rImgreszStart);
	    }
    }
#endif
    vHwImgReszResize(u4ImgReszID);

    return S_IMGRESZ_HAL_OK;
}


//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

/// Image Resizer HAL stop resize.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Stop(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgStop = TRUE;

    return S_IMGRESZ_HAL_OK;
}
#endif


/// Image Resizer HAL Get resize status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Get_Resize_Status(
    UINT32 u4ImgReszID                               ///< [IN] Image Resizer hardware ID
)

{
   IMGRESZ_HAL_INFO_T *prImgReszHalInfo;

    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    prImgReszHalInfo = &(_rImgReszHalInfo[u4ImgReszID]);

 UTIL_Printf(" wait finish....\n ");
//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

    if(_rImgReszHalInfo[u4ImgReszID].fgStop)
    {
        // If stop, look some status to judge resizer stop.
        if(fgHwImgReszResizeWorking(u4ImgReszID))
            return E_IMGRESZ_HAL_FAIL;
        else
        {
            //BOOL fgInterruptExist = fgHwImgReszInterruptExist(u4ImgReszID);

            vHwImgRezeReset(u4ImgReszID);
        }
    } else
    {
        // If no stop, just wait interrupt.
#endif
    if(!fgHwImgReszResizeFinish(u4ImgReszID))
        return E_IMGRESZ_HAL_FAIL;
    //if(!_rImgReszHalInfo[u4ImgReszID].fgResizeComplete)
        //return E_IMGRESZ_HAL_FAIL;
//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
    }
#endif
#if !IMGRESZ_PERFORMANCE_TEST
   x_thread_delay(50);


#if (IMGRESZ_CACHEDRAME_EN )
	if(fgSameBuf)
	{
      	   	i4ImgreszCacheUnMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
                              prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
                              IMGRZ_BIDIRECTIONAL);
	}
	else
	{
		if(prImgReszHalInfo->rDestImgInfo.u4BufSA1 != 0)
		{
		       i4ImgreszCacheUnMap(prImgReszHalInfo->rDestImgInfo.u4BufSA1,
					prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight,
					IMGRZ_FROM_DEVICE);


		}else
		{
			//_u4DestBufSA1Map=0;
		}
		if(prImgReszHalInfo->rSrcImgInfo.u4BufSA1 != 0)
		{
			i4ImgreszCacheUnMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA1,
					  prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
					  IMGRZ_TO_DEVICE);

		}
		if(prImgReszHalInfo->rSrcImgInfo.u4BufSA2 != 0)
		{
			i4ImgreszCacheUnMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA2,
					  prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
					  IMGRZ_TO_DEVICE);
		}
		if(prImgReszHalInfo->rSrcImgInfo.u4BufSA3 != 0)
		{
			i4ImgreszCacheUnMap(prImgReszHalInfo->rSrcImgInfo.u4BufSA3,
								 prImgReszHalInfo->rSrcImgInfo.u4BufWidth*prImgReszHalInfo->rSrcImgInfo.u4BufHeight,
								IMGRZ_TO_DEVICE);
		}
		if(prImgReszHalInfo->rDestImgInfo.u4BufSA2 != 0)
		{
		 i4ImgreszCacheUnMap(prImgReszHalInfo->rDestImgInfo.u4BufSA2,
				 prImgReszHalInfo->rDestImgInfo.u4BufWidth*prImgReszHalInfo->rDestImgInfo.u4BufHeight,
			      IMGRZ_FROM_DEVICE );
		}else
		{
			//_u4DestBufSA2Map=0;
		}
	}
#endif
#endif
    return S_IMGRESZ_HAL_OK;
}


/// Image Resizer HAL Get HW status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Get_HW_Status(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_HW_STATUS_T *prHwStatus              ///< [OUT] Hardware status
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    if(prHwStatus == NULL)
        return E_IMGRESZ_HAL_FAIL;

    i4HwImgReszGetHwStatus(u4ImgReszID,prHwStatus);

    return S_IMGRESZ_HAL_OK;
}


/// Image Resizer HAL Set HW status.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Set_HW_Status(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_HW_STATUS_T *prHwStatus              ///< [IN] Hardware status
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    if(prHwStatus == NULL)
        return E_IMGRESZ_HAL_FAIL;

    //i4HwImgReszSetHwStatus(u4ImgReszID,prHwStatus);
    #if (CONFIG_DRV_VERIFY_SUPPORT) //emulation or verify
         x_memcpy(&_gImgresz_HwStatus_ResumeBreak, prHwStatus, sizeof(IMGRESZ_HAL_HW_STATUS_T));
    #endif
    _rImgReszHalInfo[u4ImgReszID].fgResume = TRUE;

    return S_IMGRESZ_HAL_OK;
}


/// Image Resizer HAL register notify callback function.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Reg_Notify_Callback(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_NOTIFY_CB_REG_T *prNofifyCallback    ///< ]IN] Nofity callback function
)
{
    _rImgReszHalInfo[u4ImgReszID].rNofifyCallback = *prNofifyCallback;

    return S_IMGRESZ_HAL_OK;
}
#if CONFIG_DRV_VERIFY_SUPPORT
INT32 i4ImgResz_HAL_SetBurstLimit(
    UINT32 u4ImgReszID,
    UINT32 ui4BurstLimit
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    if(ui4BurstLimit == 1 || ui4BurstLimit == 2
      || ui4BurstLimit == 4 || ui4BurstLimit == 8
      || ui4BurstLimit == 16)
    {
        _rImgReszHalInfo[u4ImgReszID].fgBurstLimit = TRUE;
        _rImgReszHalInfo[u4ImgReszID].u4BurstLimit = ui4BurstLimit;
    }
    else
    {
        _rImgReszHalInfo[u4ImgReszID].fgBurstLimit = FALSE;
    }

    return S_IMGRESZ_HAL_OK;
}
INT32 i4ImgResz_HAL_SetBurstReadDisable(
    UINT32 u4ImgReszID,
    BOOL fgDisable //default is enable, this is diable here
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgBurstReadDisable = fgDisable;

    return S_IMGRESZ_HAL_OK;
}

#endif

INT32 i4ImgResz_HAL_SetAlphaCompostion(
    UINT32 u4ImgReszID,
    BOOL fgAlphaCom
)
{
    if(u4ImgReszID >= HW_IMGRESZ_NUM)
        return E_IMGRESZ_HAL_FAIL;

    _rImgReszHalInfo[u4ImgReszID].fgAlphaComposition = fgAlphaCom;

    return S_IMGRESZ_HAL_OK;
}


#if 0
/// Image Resizer HAL unregister notify callback function.
/// \return If return value < 0, it's failed. Please reference hal_img_resizer_errcode.h.
INT32 i4ImgResz_HAL_Unreg_Notify_Callback(
    UINT32 u4ImgReszID,                              ///< [IN] Image Resizer hardware ID
    IMGRESZ_HAL_NOTIFY_CB_REG_T *prNofifyCallback    ///< ]IN] Nofity callback function
)
{
    _rImgReszHalInfo[u4ImgReszID].rNofifyCallback.pvCallBackFunc = NULL;
    _rImgReszHalInfo[u4ImgReszID].rNofifyCallback.pvPrivData = NULL;

    return S_IMGRESZ_HAL_OK;
}
#endif

