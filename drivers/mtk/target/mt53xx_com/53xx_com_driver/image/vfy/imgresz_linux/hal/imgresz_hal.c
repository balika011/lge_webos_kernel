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

//#include "x_hal_ic.h"
#include "drv_common.h"
#include "imgresz_hal.h"
#include "imgresz_hal_if.h"
#include "imgresz_hal_errcode.h"
#include "x_printf.h"
#include "x_debug.h"
#include <linux/module.h>

#include "x_hal_5381.h"
#include "x_drv_map.h"

BOOL fg_showReg = FALSE;

#if CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8550
#define IMGRESZ_HAL_SUBLINE_ADJUST 0
#elif CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555
#define IMGRESZ_HAL_SUBLINE_ADJUST 1
#else
#define IMGRESZ_HAL_SUBLINE_ADJUST 0

#endif

#if (CONFIG_DRV_VERIFY_SUPPORT)  //emulation or verify code 

extern UINT32 u4SwapMode;

#endif
#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3
 extern IMGRESZ_HAL_INFO_T _rImgReszHalInfo[HW_IMGRESZ_NUM];   // HAL local information for each HW. 
#endif

UINT32 _au4IMGRZ_FILTERCOEF_H[FACTOR_MAX][18] =
{
  //      0                 1                    2               3                   4                5                     6                   7                 8                   9                 10              11                  12                13                 14                15                16                17
  //FACTOR_0
  {0x40202020, 0x20202020, 0x20202020 , 0x20202020, 0x20202020, 0x20202020 , 0x20202020, 0x20202020, 0x20202020 , 0x20202020, 0x20202020, 0x20202020 , 0x20202020, 0x20202020, 0x20202020 , 0x20202020, 0x00000000, 0x00000000},
  //FACTOR_0_0625
  {0x3f21201f, 0x2121201f, 0x2121201f , 0x2121201f, 0x2121201f, 0x2121201f , 0x2121201f, 0x2121201e, 0x2121201e , 0x2121201e, 0x2021201e, 0x2021201e , 0x21211f1e, 0x22201f1e, 0x22201f1e , 0x22201f1e, 0x00000000, 0x00000000},
  //FACTOR_0_125
  {0x3c24211d, 0x2524211c, 0x2524211c , 0x2524201b, 0x2524201b, 0x2523201b , 0x25231f1a, 0x25231f1a, 0x24231f1a , 0x24231f19, 0x26221e19, 0x25221e18 , 0x24221e18, 0x25221d18, 0x24221d17 , 0x25211d17, 0x00000000, 0x00000000},
  //FACTOR_0_25
  {0x3833220f, 0x3831210e, 0x3830200d , 0x382f1f0c, 0x382f1d0b, 0x372e1c0a , 0x372d1b09, 0x372c1a08, 0x362b1807 , 0x342a1706, 0x34291605, 0x34281504 , 0x33271403, 0x34251302, 0x34241102 , 0x34231001, 0x00000000, 0x00000000},
  //FACTOR_0_5
  {0x845000ee, 0x844bfdef, 0x8345faf0 , 0x8340f7f1, 0x813af5f2, 0x7f35f3f3 , 0x7d2ff1f4, 0x7a29eff6, 0x7724eef7 , 0x741fedf8, 0x6e1aedfa, 0x6915edfb , 0x6610ecfc, 0x5f0cedfd, 0x5c07edfe , 0x5404eeff, 0x33333331, 0x33333333},
  //FACTOR_1
  {0x00000000, 0xfef40300, 0xf9ea0500 , 0xf0e30700, 0xe5de0800, 0xd6db0800 , 0xc5da0800, 0xb1db0700, 0x9ddd0600 , 0x87e10500, 0x70e50400, 0x5aea0300 , 0x44ef0200, 0x31f40100, 0x20f80000 , 0x0ffc0000, 0x44444448, 0x44444444},
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)    
  //FACTOR_RM
  {0x00000000, 0xf0000000, 0xe0000000 , 0xd0000000, 0xc0000000, 0xb0000000 , 0xa0000000, 0x90000000, 0x80000000 , 0x70000000, 0x60000000, 0x50000000 , 0x40000000, 0x30000000, 0x20000000 , 0x10000000, 0x00000008, 0x00000000}
#endif
};


 UINT32 _au4IMGRZ_FILTERCOEF_V[FACTOR_MAX][9] =
{
  //      0                 1                    2               3                   4                5                     6                   7                 8
  //FACTOR_0
  {0x40408040, 0x40404040, 0x40404040 , 0x40404040, 0x40404040, 0x40404040 , 0x40404040, 0x40404040, 0x00000000},
  //FACTOR_0_0625
  {0x41408040, 0x41404140, 0x41404140 , 0x40404140, 0x413f4040, 0x403f403f , 0x403f403f, 0x403f403f, 0x00000000},
  //FACTOR_0_125
  {0x43417e41, 0x42404340, 0x423f4240 , 0x423e423f, 0x423e423e, 0x423d423d , 0x423c413d, 0x403c413c, 0x00000000},
  //FACTOR_0_25
  {0x4b427a43, 0x4a3f4b40, 0x493c4a3e , 0x483a493b, 0x47374838, 0x47344636 , 0x45324533, 0x442f4431, 0x00000000},
  //FACTOR_0_5
  {0x72417446, 0x6f36703b, 0x6a2c6c31 , 0x65226827, 0x6019621e, 0x59115c15 , 0x510a560d, 0x4a034f06, 0x00000000},
  //FACTOR_1
  {0x01f40000, 0xf8e2ffea, 0xdfd9eddd , 0xbad9ced8, 0x8edfa5db, 0x5fe976e4 , 0x33f348ee, 0x0ffc1ff8, 0x00000005},
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)    
  //FACTOR_RM
  {0xf0000000, 0xd000e000, 0xb000c000 , 0x9000a000, 0x70008000, 0x50006000 , 0x30004000, 0x10002000, 0x00000001}
#endif
};


const static UINT32 u4HwImgReszOffset[HW_IMGRESZ_NUM] = {
    IMGRESZ0_REG_OFFSET,
    IMGRESZ1_REG_OFFSET,
    IMGRESZ2_REG_OFFSET
};


INLINE void vHwImgReszWrite32(UINT32 u4HwId,UINT32 addr,UINT32 value)
{
    *(volatile UINT32 *)(u4HwImgReszOffset[u4HwId] + addr ) = value;
}


INLINE UINT32 u4HwImgReszRead32(UINT32 u4HwId,UINT32 addr)
{
    return *(volatile UINT32 *)(u4HwImgReszOffset[u4HwId] + addr);
}

void vImgreszPrintRegister(UINT32 u4HwId)
{
    UINT32 u4Base = 0x0;
    UINT32 u4Temp = 0;

    if(FALSE == fg_showReg)
        return ;

    for (u4Temp = 0; u4Temp < /*32*/132 + 32 + 16; u4Temp ++)
    {
        if ((u4Temp % 4) == 0)
        {
            if (u4HwId == 0)
                UTIL_Printf("[0x%8x]",0xf0020000+u4Base + u4Temp * 4);
            else if(u4HwId == 1)
                UTIL_Printf("[0x%8x]",0xf006c000+u4Base + u4Temp * 4);
			else
			    UTIL_Printf("[0x%8x]",0xf006b000+u4Base + u4Temp * 4);
        }
        UTIL_Printf("0x%8x   ",u4HwImgReszRead32(u4HwId,(u4Base + u4Temp * 4)));
        if ((u4Temp % 4) == 3)
        {
            UTIL_Printf("\n");
        }
    }
}
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)   //sun for 8560
void vHwImgRezeDMAReset(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= IMG_RESZ_DMA_SW_RST;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
    //u4RegVal &= ~IMG_RESZ_DMA_SW_RST;
   // vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}

void vHwImgRezeSetWaitWRDone(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= IMG_RESZ_WAIT_WR_DONE;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}

#endif

void vHwImgRezeReset(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= (IMG_RESZ_SW_RESET_ON|IMG_RESZ_MMU_RESET_ON);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
 #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)   
    u4RegVal |= IMG_RESZ_REGISTER_RESET_ON;//add register reset by lu.sun
 #endif
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
 #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)  
    u4RegVal &= ~IMG_RESZ_REGISTER_RESET_ON;//add register reset by lu.sun
  #endif
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
    u4RegVal &= ~(IMG_RESZ_SW_RESET_ON|IMG_RESZ_MMU_RESET_ON);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)  
    u4RegVal &=~ IMG_RESZ_DMA_SW_RST;
#endif
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);  
}

void vHwImgReszEnable(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START); mb();
    u4RegVal |= IMG_RESZ_ENABLE+IMG_RESZ_CHK_SUM_CLR;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}

void vHwImgReszDisable(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);mb();
    u4RegVal &= ~IMG_RESZ_ENABLE;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)
void vHwImgReszEnableMMU(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= IMG_RESZ_MMU_ENABLE;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}
void vHwImgReszDisableMMU(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal &= ~IMG_RESZ_MMU_ENABLE;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}
#endif

void vHwImgReszPrintRegister(UINT32 u4HwId)
{
    UINT32 u4Base = 0x0000;
    UINT32 u4Temp = 0;
    

    for (u4Temp = 0; u4Temp < 128; u4Temp ++)
    {
        if ((u4Temp % 4) == 0)
        {
            UTIL_Printf("[0x%8x]",0xF0020000+u4Base + u4Temp * 4);
        }
        UTIL_Printf("0x%8x   ",u4HwImgReszRead32(u4HwId,(u4Base + u4Temp * 4)));
        if ((u4Temp % 4) == 3)
        {
            UTIL_Printf("\n");
        }
    }
}

//sun start for turning on interrupt
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
void vHwImgIntResize(UINT32 u4HwId)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= IMG_RESZ_INT_ON;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}
#endif
//sun end

void vHwImgReszResize(UINT32 u4HwId)
{
    UINT32 u4RegVal;
    //vImgreszPrintRegister(u4HwId);
    #if 0
    u4RegVal = u4HwImgReszRead32(u4HwId,0x18);
    u4RegVal |= 0x3 << 30;
    vHwImgReszWrite32(u4HwId,0x18,u4RegVal);
    #endif
    

    if(TRUE == fg_showReg)
    {
        vImgreszPrintRegister(u4HwId);
    }

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal |= IMG_RESZ_ACTIVATE;
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}


//#if defined(IMGRESZ_HAL_MT8550) || defined(IMGRESZ_HAL_MT8530)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

BOOL fgHwImgReszResizeWorking(UINT32 u4HwId)
{
    // No working does not mean finish.
    // In picture mode, before finish, it will sometimes working and sometime rest.
    
    if((u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_INTERFACE_MONITOR_REG) & 0xFF) == 0)
    {
        return FALSE;
    } else
    {
        return TRUE;
    }
}


BOOL fgHwImgReszInterruptExist(UINT32 u4HwId)
{
    if(((u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_INTERFACE_MONITOR_REG) >> 16) & 1) == 0)
    {
        return FALSE;
    } else
    {
        return TRUE;
    }
}
#endif


BOOL fgHwImgReszResizeFinish(UINT32 u4HwId)
{
    return(u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_DONE) & 1);
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)  //sun for 8560
BOOL fgHwImgReszDMARstFinish(UINT32 u4HwId)
{
    return(u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_DONE) & 0x2);
}
#endif

INT32 i4HwImgReszSetResizeMode(UINT32 u4HwId,
                               IMGRESZ_HAL_RESIZE_MODE_T eResizeMode,
                               IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);

    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            u4RegVal &= ~IMG_RESZ_JPEG_MODE;
            u4RegVal &= ~IMG_RESZ_OSD_PARTIAL_MODE;
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            u4RegVal |= IMG_RESZ_JPEG_MODE;
            u4RegVal &= ~IMG_RESZ_OSD_PARTIAL_MODE;            
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
            u4RegVal &= ~IMG_RESZ_JPEG_MODE;
            switch(eResizeMode)
            {
                case IMGRESZ_HAL_RESIZE_MODE_FRAME:
                    u4RegVal &= ~IMG_RESZ_OSD_PARTIAL_MODE;
                    break;
                case IMGRESZ_HAL_RESIZE_MODE_PARTIAL:
                    u4RegVal |= IMG_RESZ_OSD_PARTIAL_MODE;
                    break;
            }
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetResampleMethod(UINT32 u4HwId,
                                   IMGRESZ_HAL_RESAMPLE_METHOD_T eHResampleMethod,
                                   IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod)
{
    UINT32 u4RegVal;

    // Set Horizontal resample method
    switch(eHResampleMethod)
    {
        case IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR:
            // Both bilinear and 8-tap are always turn on.
            break;
        case IMGRESZ_HAL_RESAMPLE_METHOD_8_TAP:
            // Both bilinear and 8-tap are always turn on.
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    // Set Vertical resample method
    switch(eVResampleMethod)
    {
        case IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR:
            u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);
            u4RegVal &= ~IMG_RESZ_V_4_TAP_FILTER;
            vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4RegVal);
            break;
        case IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP:
            u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);
            u4RegVal |= IMG_RESZ_V_4_TAP_FILTER;
            vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4RegVal);
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    return S_IMGRESZ_HAL_OK;
}


//////////////////////////////////////////////////////////////////////////////////
//
//     Source Buffer Setting
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetSrcBufFormat(UINT32 u4HwId,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
{
    UINT32 u4ReszType;
    UINT32 u4OsdModeSetting;
    UINT32 u4AddrSwapSetting;
    
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

    UINT32 u4WTSetting;
#endif

    u4ReszType = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);
    u4OsdModeSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    u4AddrSwapSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE);

//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

    u4WTSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_WT_SET);
#endif

    u4AddrSwapSetting &= ~(IMG_RESZ_IN_ADDR_SWAP_MODE_0 | IMG_RESZ_IN_ADDR_SWAP_MODE_1 | IMG_RESZ_IN_ADDR_SWAP_MODE_2);
    
//#ifndef IMGRESZ_HAL_MT8550
//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)

    u4AddrSwapSetting &= ~(IMG_RESZ_IN_ADDR_SWAP_MODE_4 | IMG_RESZ_IN_ADDR_SWAP_MODE_5 |
                           IMG_RESZ_IN_ADDR_SWAP_MODE_6 | IMG_RESZ_IN_ADDR_SWAP_MODE_7);
#else
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)

    u4AddrSwapSetting &= ~(IMG_RESZ_IN_ADDR_SWAP_MODE_0 | IMG_RESZ_IN_ADDR_SWAP_MODE_1 | IMG_RESZ_IN_ADDR_SWAP_MODE_2 |
                           IMG_RESZ_IN_ADDR_SWAP_MODE_4 | IMG_RESZ_IN_ADDR_SWAP_MODE_5 | IMG_RESZ_IN_ADDR_SWAP_MODE_3 |
                           IMG_RESZ_IN_ADDR_SWAP_MODE_6 | IMG_RESZ_IN_ADDR_SWAP_MODE_7);
#endif
#endif
    u4ReszType &= ~IMG_RESZ_FIELD; // If not clear, OSD mode will incorrect.

//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

    u4WTSetting |= IMG_RESZ_WT_READ_BYPASS;
#endif

    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            // Set source buffer main format
            u4ReszType &= ~IMG_RESZ_SEL_OSD_MODE;

            // Set source buffer progressive/interlace format
            if(prSrcBufferFormat->fgProgressiveFrame)
                u4ReszType &= ~IMG_RESZ_FIELD;
            else
            {
                u4ReszType |= IMG_RESZ_FIELD;
                if(prSrcBufferFormat->fgTopField)
                    u4ReszType |= IMG_RESZ_INTERLACE_TOP_FIELD;
                else
                    u4ReszType &= ~IMG_RESZ_INTERLACE_TOP_FIELD;
            }

            // Set source buffer Raster Scan/Block Mode format
            if(prSrcBufferFormat->fgBlockMode)
                u4ReszType &= ~IMG_RESZ_RASTER_SCAN_IN;
            else
                u4ReszType |= IMG_RESZ_RASTER_SCAN_IN;

#if (CONFIG_DRV_VERIFY_SUPPORT)  //emulation or verify code              
if(prSrcBufferFormat->fgAddrSwap)
            {
                switch(u4SwapMode)
                {
                    case 1:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_1;
                        break;
                    case 2:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_2;
                        break;
                    case 3:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_3;
                        break;
                    case 4:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_4;
                        break;
                    case 5:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_5;
                        break;
                    case 6:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_6;
                        break;                        
                    case 7:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_7;
                        break;
                    default:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_0;
                        break;                        
                }			
            }
#endif
#ifdef DRV_SUPPORT_ADDRESS_SWAP
            // Set address swap
            if(prSrcBufferFormat->fgAddrSwap)
            {
                switch(DRV_ADDRESS_SWAP_MODE)
                {
                    case ASM_5:
                    //#ifdef IMGRESZ_HAL_MT8530
                    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_6;
                        break;                        
                    #endif
                    default:
                        u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_1;
                        break;                        
                }
#if 0                
#if (DRV_ADDRESS_SWAP_MODE == ASM_1)
#ifdef IMGRESZ_HAL_MT8530
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_1;
#else
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_2;
#endif
#elif (DRV_ADDRESS_SWAP_MODE == ASM_2)

#ifdef IMGRESZ_HAL_MT8530
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_2;
#else
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_1;
#endif

#elif (DRV_ADDRESS_SWAP_MODE == ASM_3)
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_4;

#elif (DRV_ADDRESS_SWAP_MODE == ASM_4)
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_5;

#elif (DRV_ADDRESS_SWAP_MODE == ASM_5)
                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_6;
#elif (DRV_ADDRESS_SWAP_MODE == ASM_6)

                u4AddrSwapSetting |= IMG_RESZ_IN_ADDR_SWAP_MODE_7;
#endif
#endif
            }
#endif
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            // Set source buffer main format
            u4ReszType &= ~IMG_RESZ_SEL_OSD_MODE;

            // Set Sample Factor
            {
                UINT32 u4Value;
                u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
                u4Value &= 0xFFFFF000;
                u4Value |= (((prSrcBufferFormat->u4HSampleFactor[0] - 1) & 3) << 10);
                u4Value |= (((prSrcBufferFormat->u4VSampleFactor[0] - 1) & 3) << 8);
                u4Value |= (((prSrcBufferFormat->u4HSampleFactor[1] - 1) & 3) << 6);
                u4Value |= (((prSrcBufferFormat->u4VSampleFactor[1] - 1) & 3) << 4);
                u4Value |= (((prSrcBufferFormat->u4HSampleFactor[2] - 1) & 3) << 2);
                u4Value |= (((prSrcBufferFormat->u4VSampleFactor[2] - 1) & 3) << 0);
                vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            // Set source buffer main format
            u4ReszType |= IMG_RESZ_SEL_OSD_MODE;
            u4OsdModeSetting &= ~IMG_RESZ_OSD_DIRECT_MODE;

            // Set index buffer format
            u4OsdModeSetting &= ~(IMG_RESZ_OSD_INDEX_2BPP | IMG_RESZ_OSD_INDEX_4BPP |
                                  IMG_RESZ_OSD_INDEX_8BPP);
            switch(prSrcBufferFormat->eIndexBufferFormat)
            {
                case IMGRESZ_HAL_INDEX_BUFFER_FORMAT_2BPP:
                    u4OsdModeSetting |= IMG_RESZ_OSD_INDEX_2BPP;
                    break;
                case IMGRESZ_HAL_INDEX_BUFFER_FORMAT_4BPP:
                    u4OsdModeSetting |= IMG_RESZ_OSD_INDEX_4BPP;
                    break;
                case IMGRESZ_HAL_INDEX_BUFFER_FORMAT_8BPP:
                    u4OsdModeSetting |= IMG_RESZ_OSD_INDEX_8BPP;
                    break;
                default:
                    return E_IMGRESZ_HAL_FAIL;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
            // Set source buffer main format
            u4ReszType |= IMG_RESZ_SEL_OSD_MODE;
            u4OsdModeSetting |= IMG_RESZ_OSD_DIRECT_MODE;

            // Set ARGB buffer format
            u4OsdModeSetting &= ~(IMG_RESZ_OSD_DIRECT_RGB565 | IMG_RESZ_OSD_DIRECT_ARGB1555 |
                                  IMG_RESZ_OSD_DIRECT_ARGB4444 | IMG_RESZ_OSD_DIRECT_ARGB8888);
            switch(prSrcBufferFormat->eARGBBufferFormat)
            {
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_0565:
                    u4OsdModeSetting |= IMG_RESZ_OSD_DIRECT_RGB565;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_1555:
                    u4OsdModeSetting |= IMG_RESZ_OSD_DIRECT_ARGB1555;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444:
                    u4OsdModeSetting |= IMG_RESZ_OSD_DIRECT_ARGB4444;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888:
                    u4OsdModeSetting |= IMG_RESZ_OSD_DIRECT_ARGB8888;
                    break;
                default:
                    return E_IMGRESZ_HAL_FAIL;
            }
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

            // Set WT compression
            if(prSrcBufferFormat->fgWT)
            {
                u4WTSetting &= ~IMG_RESZ_WT_READ_BYPASS;
                switch(prSrcBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
                        u4WTSetting &= ~IMG_RESZ_WT_READ_FORMAT_AYUV;
                        break;
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                        u4WTSetting |= IMG_RESZ_WT_READ_FORMAT_AYUV;
                        break;
                    default :
                        break;
                }
            } else
            {
                u4WTSetting |= IMG_RESZ_WT_READ_BYPASS;
                switch(prSrcBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
                        u4WTSetting &= ~IMG_RESZ_WT_READ_FORMAT_AYUV;
                        break;
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                        u4WTSetting |= IMG_RESZ_WT_READ_FORMAT_AYUV;
                        break;
                    default:
                        break;
                }
            }
#endif
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4ReszType);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4OsdModeSetting);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE,u4AddrSwapSetting);
    
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_WT_SET,u4WTSetting);
#endif

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetSrcBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2, UINT32 u4BufSA3)
{
#if !IMGRESZ_HW1_IOMMU_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if(u4HwId == 1)
    {
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
        if (u4BufSA3 != 0)
        {
            u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
        }
    }
    else
#endif
    {
#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3
        if(_rImgReszHalInfo[0].eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL 
         && _rImgReszHalInfo[0].rJpegInfo.fgPictureMode)
        {
           u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
           u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
            if (u4BufSA3 != 0)
           {
             u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
           }       
        }
        else
        {
           #if !IMGRESZ_IO_MMU_TEST
           u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
           u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
            if (u4BufSA3 != 0)
           {
             u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
           }

          #endif
        }
#else
     #if !IMGRESZ_IO_MMU_TEST  
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
        if (u4BufSA3 != 0)
        {
             u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
        }
#endif
#endif
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_Y_ADDR_BASE1,(u4BufSA1>>4));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_CB_ADDR_BASE1,(u4BufSA2>>4));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_CR_ADDR_BASE1,(u4BufSA3>>4));

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetSrcPrevRowBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2, UINT32 u4BufSA3)
{
#if !IMGRESZ_HW1_IOMMU_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if(u4HwId == 1)
    {
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
        u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
    }
    else
#endif
    {
#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3
        if(_rImgReszHalInfo[0].eResizeMode == IMGRESZ_HAL_RESIZE_MODE_PARTIAL 
         && _rImgReszHalInfo[0].rJpegInfo.fgPictureMode)
        {
           u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
           u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
           u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;        
        }
        else
        {
           #if !IMGRESZ_IO_MMU_TEST
           u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
           u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
           u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
           #endif
        }
#else
    #if !IMGRESZ_IO_MMU_TEST
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
        u4BufSA3 = u4BufSA3 ? PHYSICAL(u4BufSA3) : 0;
        
    #endif
#endif
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_Y_ADDR_BASE2,(u4BufSA1>>4));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_CB_ADDR_BASE2,(u4BufSA2>>4));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_CR_ADDR_BASE2,(u4BufSA3>>4));

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)  //sun for 8560
INT32 i4HwImgReszSetSrcBufWidth(UINT32 u4HwId,UINT32 u4BufWidth, IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
{
    UINT32 u4YBufWidth;
    UINT32 u4CBufWidth;

    if (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER)
    {
        u4YBufWidth = u4BufWidth;		
	if((prSrcBufferFormat->u4HSampleFactor[1] <prSrcBufferFormat->u4HSampleFactor[0])
           && (prSrcBufferFormat->u4HSampleFactor[2] < prSrcBufferFormat->u4HSampleFactor[0]))
	{
//#if JDEC_VFY_TEST_MCUROW_ENHANCE
          //u4CBufWidth = u4BufWidth;
//#else
          u4CBufWidth =((u4BufWidth/2 + 15) /16) * 16;
//#endif
	 }else
	 {
	   u4CBufWidth = u4BufWidth;
	 }
	 vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_BUF_LEN,(((u4CBufWidth>>4)&0x3FFF)<<12)|(u4YBufWidth>>4));
     }
    else
    {
      vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_BUF_LEN,(((u4BufWidth>>4)&0x3FFF)<<12)|(u4BufWidth>>4));
    }
    return S_IMGRESZ_HAL_OK;
}
#else
INT32 i4HwImgReszSetSrcBufWidth(UINT32 u4HwId,UINT32 u4BufWidth)
{
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_BUF_LEN,u4BufWidth>>4);

    return S_IMGRESZ_HAL_OK;
}
#endif

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
INT32 i4HwImgReszSetSrcRowBufHeight(UINT32 u4HwId,UINT32 u4RowBufHeight,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat, BOOL fgRPRRacingModeEnable)
{
    UINT32 u4Value;
    UINT32 u4RowBufHeightY = 0,u4RowBufHeightCb = 0,u4RowBufHeightCr = 0;

    if(u4RowBufHeight == 0)
    {
        u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
        u4Value &= ~IMG_RESZ_LINES_ASSIGNED_DIRECTLY;
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
    } else
    {
        u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
        u4Value |= IMG_RESZ_LINES_ASSIGNED_DIRECTLY;
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
        u4RowBufHeightY = u4RowBufHeight;

	
	 if(fgRPRRacingModeEnable)                   //sun new
	 u4RowBufHeightCb = u4RowBufHeight/2;   
	 
        if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER)
        {
            u4RowBufHeightCb = u4RowBufHeight * prSrcBufferFormat->u4VSampleFactor[1] / prSrcBufferFormat->u4VSampleFactor[0];
            u4RowBufHeightCr = u4RowBufHeight * prSrcBufferFormat->u4VSampleFactor[2] / prSrcBufferFormat->u4VSampleFactor[0];
        }
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF,u4RowBufHeightY);
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF,u4RowBufHeightCb);
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF,u4RowBufHeightCr);
    }

    return S_IMGRESZ_HAL_OK;
}

#else

INT32 i4HwImgReszSetSrcRowBufHeight(UINT32 u4HwId,UINT32 u4RowBufHeight,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
{
    UINT32 u4Value;
    UINT32 u4RowBufHeightY = 0,u4RowBufHeightCb = 0,u4RowBufHeightCr = 0;

    if(u4RowBufHeight == 0)
    {
        u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
        u4Value &= ~IMG_RESZ_LINES_ASSIGNED_DIRECTLY;
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
    } else
    {
        u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
        u4Value |= IMG_RESZ_LINES_ASSIGNED_DIRECTLY;
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
        u4RowBufHeightY = u4RowBufHeight;    
	 
        if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER)
        {
            u4RowBufHeightCb = u4RowBufHeight * prSrcBufferFormat->u4VSampleFactor[1] / prSrcBufferFormat->u4VSampleFactor[0];
            u4RowBufHeightCr = u4RowBufHeight * prSrcBufferFormat->u4VSampleFactor[2] / prSrcBufferFormat->u4VSampleFactor[0];
        }
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_Y_COLOR_BUF,u4RowBufHeightY);
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_CB_COLOR_BUF,u4RowBufHeightCb);
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LINE_NUM_IN_CR_COLOR_BUF,u4RowBufHeightCr);
    }

    return S_IMGRESZ_HAL_OK;
}

#endif




INT32 i4HwImgReszSetSrcImageWidthHeight(UINT32 u4HwId,UINT32 u4Width,UINT32 u4Height,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
{
    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_Y,((u4Width&0xFFFF)<<16) | (u4Height&0xFFFF));
            switch(prSrcBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CB,(((u4Width/2)&0xFFFF)<<16) | ((u4Height/2)&0xFFFF));
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CB,(((u4Width/2)&0xFFFF)<<16) | (u4Height&0xFFFF));
                    break;
                default:
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            {
                UINT32 u4SrcWidth,u4SrcHeight;

                vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_Y,((u4Width&0xFFFF)<<16) | (u4Height&0xFFFF));

                if((prSrcBufferFormat->u4HSampleFactor[1] != 0) && (prSrcBufferFormat->u4VSampleFactor[1] != 0))
                {
                    u4SrcWidth = u4Width * prSrcBufferFormat->u4HSampleFactor[1] / prSrcBufferFormat->u4HSampleFactor[0];
                    u4SrcHeight = u4Height * prSrcBufferFormat->u4VSampleFactor[1] / prSrcBufferFormat->u4VSampleFactor[0];
                    // For jpeg picture mode, prevent source height 401 come two interrupt (Y interrupt and C interrupt)
                    if((u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[0]) != (u4Height * prSrcBufferFormat->u4VSampleFactor[1]))
                        u4SrcHeight++;
                    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CB,((u4SrcWidth&0xFFFF)<<16) |(u4SrcHeight&0xFFFF));
                }

                if((prSrcBufferFormat->u4HSampleFactor[2] != 0) && (prSrcBufferFormat->u4VSampleFactor[2] != 0))
                {
                    u4SrcWidth = u4Width * prSrcBufferFormat->u4HSampleFactor[2] / prSrcBufferFormat->u4HSampleFactor[0];
                    u4SrcHeight = u4Height * prSrcBufferFormat->u4VSampleFactor[2] / prSrcBufferFormat->u4VSampleFactor[0];
                    // For jpeg picture mode, prevent source height 401 come two interrupt (Y interrupt and C interrupt)
                    if((u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[0]) != (u4Height * prSrcBufferFormat->u4VSampleFactor[2]))
                        u4SrcHeight++;
                    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CR,((u4SrcWidth&0xFFFF)<<16) |(u4SrcHeight&0xFFFF));
                }
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_Y,((u4Width&0xFFFF)<<16) | (u4Height&0xFFFF));
            break;
    }

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetSrcImageOffset(UINT32 u4HwId,UINT32 u4XOff,UINT32 u4YOff,
                                   IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                   UINT32 u4SrcHeight,UINT32 u4TgtHeight)
{
    UINT32 u4XOff1=0,u4YOff1=0;
    UINT32 u4XOff2=0,u4YOff2=0;
    UINT32 u4XOff3=0,u4YOff3=0;

    u4XOff1 = u4XOff;
    u4YOff1 = u4YOff;

    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            switch(prSrcBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                    u4XOff2 = u4XOff>>1;
                    u4YOff2 = u4YOff>>1;
#if IMGRESZ_HAL_SUBLINE_ADJUST                    
                    // Adjust subline
                    if((!prSrcBufferFormat->fgProgressiveFrame) &&
                       (!prSrcBufferFormat->fgTopField) &&
                       (u4SrcHeight>u4TgtHeight))
                    {
                        UINT32 u4Q = (u4SrcHeight - u4TgtHeight) /(u4TgtHeight * 2 );
                        //UINT32 u4R_norm =  ( ((u4SrcHeight - u4TgtHeight) - (u4Q * u4TgtHeight *2)) * 2048 ) / (2 * u4SrcHeight );

                        u4YOff1 += u4Q * 2;
                        u4YOff2 += u4Q * 2;
                    }
#endif                    
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                    u4XOff2 = u4XOff>>1;
                    u4YOff2 = u4YOff;
                    break;
                default :
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            {
                UINT32 u4HSample,u4VSample;

                if((prSrcBufferFormat->u4HSampleFactor[1] != 0) && (prSrcBufferFormat->u4VSampleFactor[1] != 0))
                {
                    u4HSample = prSrcBufferFormat->u4HSampleFactor[0] / prSrcBufferFormat->u4HSampleFactor[1];
                    u4VSample = prSrcBufferFormat->u4VSampleFactor[0] / prSrcBufferFormat->u4VSampleFactor[1];
                    u4XOff2 = u4XOff/u4HSample;
                    u4YOff2 = u4YOff/u4VSample;
                } else
                {
                    u4XOff2 = 0;
                    u4YOff2 = 0;
                }

                if((prSrcBufferFormat->u4HSampleFactor[2] != 0) && (prSrcBufferFormat->u4VSampleFactor[2] != 0))
                {                
                    u4HSample = prSrcBufferFormat->u4HSampleFactor[0] / prSrcBufferFormat->u4HSampleFactor[2];
                    u4VSample = prSrcBufferFormat->u4VSampleFactor[0] / prSrcBufferFormat->u4VSampleFactor[2];
                    u4XOff3 = u4XOff/u4HSample;
                    u4YOff3 = u4YOff/u4VSample;
                } else
                {
                    u4XOff3 = 0;
                    u4YOff3 = 0;
                }
            }
            break;
        default:
            break;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_OFFSET_Y,((u4XOff1&0xFFF)<<12) | (u4YOff1&0xFFF));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_OFFSET_CB,((u4XOff2&0xFFF)<<12) | (u4YOff2&0xFFF));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_OFFSET_CR,((u4XOff3&0xFFF)<<12) | (u4YOff3&0xFFF));

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

INT32 i4HwImgReszSetRPR(UINT32 u4HwId,BOOL fgRPRMode, BOOL fgRPRRacingModeEnable)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_RPR);
    if(fgRPRMode)
    {
        u4Value |= IMG_RESZ_RPR_FLAG_ON;
    } else
    {
        u4Value &= ~IMG_RESZ_RPR_FLAG_ON;
    }

    if(fgRPRRacingModeEnable)
    {  
        u4Value &= ~(0x1<<10);
        u4Value |= IMG_RESZ_TRC_VDEC_EN;
	 u4Value |= IMG_RESZ_TRC_VDEC_INT;
    } else
    {
        u4Value &= ~IMG_RESZ_TRC_VDEC_EN;
	 u4Value &= ~IMG_RESZ_TRC_VDEC_INT;
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_RPR,u4Value);

    return S_IMGRESZ_HAL_OK;
}
#endif

INT32 i4HwImgReszSetSrcFirstRow(UINT32 u4HwId,BOOL fgFirstRow)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
    if(fgFirstRow)
    {
        u4Value |= IMG_RESZ_FIRST_BLOCK_LINE;
    } else
    {
        u4Value &= ~IMG_RESZ_FIRST_BLOCK_LINE;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetSrcLastRow(UINT32 u4HwId,BOOL fgLastRow)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);
    if(fgLastRow)
    {
        u4Value |= IMG_RESZ_LAST_BLOCK_LINE;
    } else
    {
        u4Value &= ~IMG_RESZ_LAST_BLOCK_LINE;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetARGB2RGBA(UINT32 u4HwId, BOOL fgArgb2Rgba)
{
 #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_OSD_MODE_SETTING);
    if(fgArgb2Rgba)
    {
        u4Value |= IMG_RESZ_OSD_ARGB_RGBA;
    } else
    {
        u4Value &= ~IMG_RESZ_OSD_ARGB_RGBA;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4Value);
#endif
    return S_IMGRESZ_HAL_OK;
}
INT32 i4HwImgReszSetVideoCbCrSwap(UINT32 u4HwId, BOOL fgCbCrSwap)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_TYPE);
    if(fgCbCrSwap)
    {
        u4Value |= IMG_RESZ_CBCRSWAP;
    } else
    {
        u4Value &= ~IMG_RESZ_CBCRSWAP;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4Value);

    return S_IMGRESZ_HAL_OK;
}
INT32 i4HwImgReszSetAYUVCbCrSwap(UINT32 u4HwId, BOOL fgCbCrSwap)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_OSD_MODE_SETTING);
    if(fgCbCrSwap)
    {
        u4Value |= IMG_RESZ_OSD_SWITCH_CBCR;
    } else
    {
        u4Value &= ~IMG_RESZ_OSD_SWITCH_CBCR;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4Value);

    return S_IMGRESZ_HAL_OK;
}
INT32 i4HwImgReszSetAYUVYONLY(UINT32 u4HwId, BOOL fgYOnly)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_TYPE);
    if(fgYOnly)
    {
        u4Value |= IMG_RESZ_CBCR_PADDING;
        UTIL_Printf("[IMGRESZ] Y only 0x%x 0x%x\n", u4Value, RW_IMG_RESZ_TYPE);
    } else
    {
        u4Value &= ~IMG_RESZ_CBCR_PADDING;
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4Value);

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetOSDFLIP(UINT32 u4HwId, BOOL fgFlipOpen)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_FLIP_FUNC);
    if(fgFlipOpen)
    {
        u4Value |= RW_IMG_RESZ_FLIP_OPEN;
    } else
    {
        u4Value &= ~RW_IMG_RESZ_FLIP_OPEN;
    }

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_FLIP_FUNC,u4Value);

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszResetDestPartialBuf(UINT32 u4HwId, BOOL fgReset)
{
    UINT32 u4Value;

    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE);

    if (fgReset)
    {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)        
        u4Value = u4Value | IMG_RESZ_REST_ADDRESS;
#endif
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_MEM_IF_MODE, u4Value);
    }
    else
    {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)        
        u4Value = u4Value & ~IMG_RESZ_REST_ADDRESS;
#endif
    }
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_MEM_IF_MODE, u4Value);

    return S_IMGRESZ_HAL_OK;
}

//////////////////////////////////////////////////////////////////////////////////
//
//     Destination Buffer Setting
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetDestBufFormat(UINT32 u4HwId,IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                                IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat)
{
    UINT32 u4RegVal;
    UINT32 u4OsdModeSetting;
    UINT32 u4AddrSwapSetting;
    UINT32 u4CscSetting;
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    UINT32 u4WTSetting;
#endif

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);
    u4OsdModeSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    u4AddrSwapSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE);
    u4CscSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_CSC_SETTING);
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

    u4WTSetting = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_WT_SET);
#endif

    u4AddrSwapSetting &= ~(IMG_RESZ_OUT_ADDR_SWAP_MODE_0 | IMG_RESZ_OUT_ADDR_SWAP_MODE_1 | IMG_RESZ_OUT_ADDR_SWAP_MODE_2);

//#ifdef IMGRESZ_HAL_MT8530
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
    u4AddrSwapSetting &= ~(IMG_RESZ_OUT_ADDR_SWAP_MODE_4 | IMG_RESZ_OUT_ADDR_SWAP_MODE_5 |
                           IMG_RESZ_OUT_ADDR_SWAP_MODE_6 | IMG_RESZ_OUT_ADDR_SWAP_MODE_7);
#else
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
    u4AddrSwapSetting &= ~(IMG_RESZ_OUT_ADDR_SWAP_MODE_0 | IMG_RESZ_OUT_ADDR_SWAP_MODE_1 | IMG_RESZ_OUT_ADDR_SWAP_MODE_2 |
                           IMG_RESZ_OUT_ADDR_SWAP_MODE_3 | IMG_RESZ_OUT_ADDR_SWAP_MODE_4 | IMG_RESZ_OUT_ADDR_SWAP_MODE_5 |
                           IMG_RESZ_OUT_ADDR_SWAP_MODE_6 | IMG_RESZ_OUT_ADDR_SWAP_MODE_7);
#endif
#endif

    u4RegVal &= ~IMG_RESZ_V2OSD; // If not clear, OSD mode will incorrect.
    u4CscSetting &= ~IMG_RESZ_OSD_CSC_ENABLE;
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    u4WTSetting |= IMG_RESZ_WT_WRITE_BYPASS;
#endif 

    switch(prDestBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            // Set destination buffer YUV format
            u4RegVal &= ~(IMG_RESZ_420_OUT | IMG_RESZ_422_OUT | IMG_RESZ_444_OUT);
            switch(prDestBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                    u4RegVal |= IMG_RESZ_420_OUT;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                    u4RegVal |= IMG_RESZ_422_OUT;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_444:
                    u4RegVal |= IMG_RESZ_444_OUT;
                    break;
            }

            // Set destination buffer Raster Scan/Block Mode format
            if(prDestBufferFormat->fgBlockMode)
                u4RegVal &= ~IMG_RESZ_RASTER_SCAN_OUT;
            else
                u4RegVal |= IMG_RESZ_RASTER_SCAN_OUT;

#if (CONFIG_DRV_VERIFY_SUPPORT)  //emulation or verify code

            if(prDestBufferFormat->fgAddrSwap)
            {
                switch(u4SwapMode)
                {
                    case 1:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_1;
                        break;
                    case 2:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_2;
                        break;
                    case 3:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_3;
                        break;
                    case 4:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_4;
                        break;
                    case 5:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_5;
                        break;
                    case 6:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_6;
                        break;
                    case 7:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_7;
                        break;
                    default:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_0;
                        break;
                }
            }
#endif
#ifdef DRV_SUPPORT_ADDRESS_SWAP
            // Set address swap
            if(prDestBufferFormat->fgAddrSwap)
            {
                switch(DRV_ADDRESS_SWAP_MODE)
                {
                    case ASM_5:
               //#ifdef IMGRESZ_HAL_MT8530
               #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_6;
                        break;                        
               #endif
                    default:
                        u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_1;
                        break;                        
                }               
#if 0                
#if (DRV_ADDRESS_SWAP_MODE == ASM_1)
#ifdef IMGRESZ_HAL_MT8530
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_1;
#else
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_2;
#endif
#elif (DRV_ADDRESS_SWAP_MODE == ASM_2)
#ifdef IMGRESZ_HAL_MT8530
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_2;
#else
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_1;
#endif
#elif (DRV_ADDRESS_SWAP_MODE == ASM_3)
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_4;
#elif (DRV_ADDRESS_SWAP_MODE == ASM_4)
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_5;
#elif (DRV_ADDRESS_SWAP_MODE == ASM_5)
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_6;
#elif (DRV_ADDRESS_SWAP_MODE == ASM_6)
                u4AddrSwapSetting |= IMG_RESZ_OUT_ADDR_SWAP_MODE_7;
#endif
#endif
            }
#endif
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
            u4OsdModeSetting &= ~(IMG_RESZ_OSD_OUTPUT_RGB565 | IMG_RESZ_OSD_OUTPUT_ARGB1555 |
                                  IMG_RESZ_OSD_OUTPUT_ARGB4444 | IMG_RESZ_OSD_OUTPUT_ARGB8888);
            switch(prDestBufferFormat->eARGBBufferFormat)
            {
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_0565:
                    u4OsdModeSetting |= IMG_RESZ_OSD_OUTPUT_RGB565;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_1555:
                    u4OsdModeSetting |= IMG_RESZ_OSD_OUTPUT_ARGB1555;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_4444:
                    u4OsdModeSetting |= IMG_RESZ_OSD_OUTPUT_ARGB4444;
                    break;
                case IMGRESZ_HAL_ARGB_BUFFER_FORMAT_8888:
                    u4OsdModeSetting |= IMG_RESZ_OSD_OUTPUT_ARGB8888;
                    break;
            }

            if(prDestBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER)
            {
                switch(prSrcBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
                        u4RegVal |= IMG_RESZ_V2OSD;
                        u4RegVal &= ~(IMG_RESZ_420_OUT | IMG_RESZ_422_OUT | IMG_RESZ_444_OUT);
                        u4RegVal |= IMG_RESZ_444_OUT;
                        break;
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
                        u4CscSetting |= IMG_RESZ_OSD_CSC_ENABLE;
#if 1
                        // Do not down-scale Y
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YIN_D16;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_CIN_D128;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YOUT_A16;
                        u4CscSetting |= IMG_RESZ_OSD_CSC_COUT_A128;
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF11,0x132);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF12,0x259);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF13,0x75);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF21,0x1F50);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF22,0x1EA5);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF23,0x20B);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF31,0x20B);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF32,0x1E4A);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF33,0x1FAB);
#else
                        // Down-scale Y
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YIN_D16;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_CIN_D128;
                        u4CscSetting |= IMG_RESZ_OSD_CSC_YOUT_A16;
                        u4CscSetting |= IMG_RESZ_OSD_CSC_COUT_A128;
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF11,0x107);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF12,0x204);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF13,0x64);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF21,0x1F68);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF22,0x1ED6);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF23,0x1C2);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF31,0x1C2);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF32,0x1E87);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF33,0x1FB7);
#endif                        
                        break;
                    default:
                        break;
                }
            } else if(prDestBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER)
            {
                switch(prSrcBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                        u4CscSetting |= IMG_RESZ_OSD_CSC_ENABLE;
#if 1
                        // Do not up-scale Y
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YIN_D16;
                        u4CscSetting |= IMG_RESZ_OSD_CSC_CIN_D128;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YOUT_A16;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_COUT_A128;
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF11,0x400);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF12,0x0);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF13,0x57C);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF21,0x400);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF22,0x1EA8);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF23,0x1D35);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF31,0x400);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF32,0x6EE);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF33,0x0);                        
#else
                        // Up-scale Y
                        u4CscSetting |= IMG_RESZ_OSD_CSC_YIN_D16;
                        u4CscSetting |= IMG_RESZ_OSD_CSC_CIN_D128;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_YOUT_A16;
                        u4CscSetting &= ~IMG_RESZ_OSD_CSC_COUT_A128;
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF11,0x4A8);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF12,0x0);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF13,0x662);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF21,0x4A8);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF22,0x1E70);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF23,0x1CBF);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF31,0x4A8);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF32,0x812);
                        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_COEF33,0x0);
#endif                        
                        break;
                    default:
                        break;
                }
            }
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

            // Set WT compression
            if(prDestBufferFormat->fgWT)
            {
                u4WTSetting &= ~IMG_RESZ_WT_WRITE_BYPASS;
                switch(prDestBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
                        u4WTSetting &= ~IMG_RESZ_WT_WRITE_FORMAT_AYUV;
                        break;
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                        u4WTSetting |= IMG_RESZ_WT_WRITE_FORMAT_AYUV;
                        break;
                    default:
                        break;
                }
            } else
            {
                u4WTSetting |= IMG_RESZ_WT_WRITE_BYPASS;
                switch(prDestBufferFormat->eBufferMainFormat)
                {
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
                        u4WTSetting &= ~IMG_RESZ_WT_WRITE_FORMAT_AYUV;
                        break;
                    case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
                        u4WTSetting |= IMG_RESZ_WT_WRITE_FORMAT_AYUV;
                        break;
                    default:
                        break;
                }
            }
#endif
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;

    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4RegVal);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4OsdModeSetting);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE,u4AddrSwapSetting);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_CSC_SETTING,u4CscSetting);
    
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_WT_SET,u4WTSetting);
#endif

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetDestBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2)
{
#if !IMGRESZ_HW1_IOMMU_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if(u4HwId == 1)
    {
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
    }
    else
#endif
    {
#if !IMGRESZ_IO_MMU_TEST
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
#endif
    }

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_Y_ADDR_BASE,(u4BufSA1>>4));
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_C_ADDR_BASE,(u4BufSA2>>4));

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetDestBufWidth(UINT32 u4HwId,UINT32 u4BufWidth)
{
    UINT32 u4Value;

//#if !defined(IMGRESZ_HAL_MT8530) && !defined(IMGRESZ_HAL_MT8550)
//#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8530) && (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8550))
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8530) 
    if(u4BufWidth == 0x4000)
    	u4BufWidth = 0x4000 - 16;
#endif

    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TGT_BUF_LEN);
#if 0
#ifndef IMGRESZ_HAL_MT8550
#ifdef IMGRESZ_HAL_MT8530
    u4Value = (u4Value & 0xFFFFF000) | (u4BufWidth>>4);
#else
    u4Value = (u4Value & 0xFFFFFC00) | (u4BufWidth>>4);
#endif
#else
    u4Value = (u4Value & 0xFFFFF000) | (u4BufWidth>>4);
#endif
#endif

#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8530)
    u4Value = (u4Value & 0xFFFFFC00) | (u4BufWidth>>4);
#elif (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)   //sun for 8560
    u4Value = (u4Value & 0xFF000000) |(((u4BufWidth>>4)&0x3FFF) <<12 )|(u4BufWidth>>4);
#else
    u4Value = (u4Value & 0xFFFFF000) | (u4BufWidth>>4);
#endif

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_BUF_LEN,u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetDestImageWidthHeight(UINT32 u4HwId,UINT32 u4Width,UINT32 u4Height)
{
//#if !defined(IMGRESZ_HAL_MT8550) && !defined(IMGRESZ_HAL_MT8530)
#if ((CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8530) \
    && (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8550) \
    && (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8580))
    if(u4Width == 0x1000)
    	u4Width = 0xFFF;
#endif

//#if defined(IMGRESZ_HAL_MT8550) || defined(IMGRESZ_HAL_MT8530)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_SIZE,((u4Width&0xFFFF)<<16) | (u4Height&0xFFFF));
#else
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_SIZE,((u4Width&0xFFF)<<12) | (u4Height&0xFFF));
#endif

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetDestImageOffset(UINT32 u4HwId,UINT32 u4XOff,UINT32 u4YOff)
{
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TGT_OFFSET,((u4XOff&0xFFF)<<12) | (u4YOff&0xFFF));

    return S_IMGRESZ_HAL_OK;
}
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
void vHwImgReszReadBurstLength(UINT32 u4HwId,BOOL fgEnable)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_START);
    u4RegVal &= ~(IMG_RESZ_RD_BURST_ON);

    if(fgEnable)
      {
        u4RegVal |= IMG_RESZ_RD_BURST_ON;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        //u4RegVal |= IMG_RESZ_DMA_SAFE_MODE;
#endif
    	}
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_START,u4RegVal);
}
#endif
//////////////////////////////////////////////////////////////////////////////////
//
//     Alpha Blending
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetAlphaBlendingLevel(UINT32 u4HwId,UINT32 u4Alpha)
{
    UINT32 u4RegVal;

//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

    if(u4Alpha > 128)
        u4Alpha = 128;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    u4RegVal &= ~(((UINT32)0xFF)<<IMG_RESZ_A_BLEND_SHIFT);
    u4RegVal |= u4Alpha<<IMG_RESZ_A_BLEND_SHIFT;
#else
    u4Alpha = (u4Alpha * (8+1)) / 255;
    if(u4Alpha > 8)
        u4Alpha = 8;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    u4RegVal &= ~(0xF<<IMG_RESZ_A_BLEND_SHIFT);
    u4RegVal |= u4Alpha<<IMG_RESZ_A_BLEND_SHIFT;
#endif

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetPreloadBufAddr(UINT32 u4HwId,UINT32 u4BufSA1,UINT32 u4BufSA2)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if(u4HwId == 1)
    {
        #if !IMGRESZ_IO_MMU_TEST
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
        #endif
    }
    else
#endif
    {
#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3 //for before 8560ECO) //for before 8560ECO)
       {
//#if !IMGRESZ_IO_MMU_TEST
        u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
        u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
//#endif
       }
#else
       {
       #if !IMGRESZ_IO_MMU_TEST
           u4BufSA1 = u4BufSA1 ? PHYSICAL(u4BufSA1) : 0;
           u4BufSA2 = u4BufSA2 ? PHYSICAL(u4BufSA2) : 0;
       #endif
       }
#endif
    }    
    UTIL_Printf("hw id:%d, u4BufSA1:0x%x\n", u4HwId, u4BufSA1);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_Y_PRELOAD_OW_ADDR_BASE,u4BufSA1>>4);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_C_PRELOAD_OW_ADDR_BASE,u4BufSA2>>4);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszPerformaceGetCycle(UINT32 u4HwId)
{
    UINT32 u4RegVal;
    if(u4HwId == 1)
    {
        u4RegVal = u4HwImgReszRead32(u4HwId, 0x220);
        UTIL_Printf("<imgresz>0x220 0x%x \n", u4RegVal);
    }
    return 0;
}

INT32 i4HwImgReszSetAlphaChangeScalingType(UINT32 u4HwId,UINT32 u4ScalingType)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    u4RegVal &= ~(IMG_RESZ_OSD_ALPHA_SCALE_NORMAL | IMG_RESZ_OSD_ALPHA_SCALE_REF_LEFT | IMG_RESZ_OSD_ALPHA_SCALE_REF_NEAREST);
    switch(u4ScalingType)
    {
        case 0:
            u4RegVal |= IMG_RESZ_OSD_ALPHA_SCALE_NORMAL;
            break;
        case 1:
            u4RegVal |= IMG_RESZ_OSD_ALPHA_SCALE_REF_LEFT;
            break;
        case 2:
            u4RegVal |= IMG_RESZ_OSD_ALPHA_SCALE_REF_NEAREST;
            break;
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

INT32 i4HwImgReszSetAlphaChangeBilinearBoundary(UINT32 u4HwId,BOOL fgBilinearBoundary)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    if(fgBilinearBoundary)
        u4RegVal |= IMG_RESZ_OSD_ALPHA_BILINEAR_BOUNDARY;
    else
        u4RegVal &= ~IMG_RESZ_OSD_ALPHA_BILINEAR_BOUNDARY;        
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetOnlyDistinguishAlpha(UINT32 u4HwId,BOOL fgOnlyDistinguishAlpha)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING);
    if(fgOnlyDistinguishAlpha)
        u4RegVal |= IMG_RESZ_OSD_ONLY_DISTINGUISH_ALPHA;
    else
        u4RegVal &= ~IMG_RESZ_OSD_ONLY_DISTINGUISH_ALPHA;        
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_MODE_SETTING,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}
#endif


INT32 i4HwImgReszSetLumaKeyEnable(UINT32 u4HwId,BOOL fgEnable)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_LUMA_KEY);
    if(fgEnable)
    {
        u4RegVal |= IMG_RESZ_LUMA_KEY_ENABLE;
    } else
    {
        u4RegVal &= ~IMG_RESZ_LUMA_KEY_ENABLE;
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LUMA_KEY,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetLumaKeyScalingType(UINT32 u4HwId,UINT32 u4ScalingType)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_LUMA_KEY);
    u4RegVal &= ~(IMG_RESZ_LUMA_KEY_SCALE_NORMAL | IMG_RESZ_LUMA_KEY_SCALE_REF_LEFT | IMG_RESZ_LUMA_KEY_SCALE_REF_NEAREST);
    switch(u4ScalingType)
    {
        case 0:
            u4RegVal |= IMG_RESZ_LUMA_KEY_SCALE_NORMAL;
            break;
        case 1:
            u4RegVal |= IMG_RESZ_LUMA_KEY_SCALE_REF_LEFT;
            break;
        case 2:
            u4RegVal |= IMG_RESZ_LUMA_KEY_SCALE_REF_NEAREST;
            break;
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LUMA_KEY,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetLumaKey(UINT32 u4HwId,UINT8 u1LumaKey)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_LUMA_KEY);
    u4RegVal &= ~(0xFF<<IMG_RESZ_LUMA_KEY_SHIFT);
    u4RegVal |= (u1LumaKey<<IMG_RESZ_LUMA_KEY_SHIFT);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LUMA_KEY,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

INT32 i4HwImgReszSetLumaKeyBilinearBoundary(UINT32 u4HwId,BOOL fgBilinearBoundary)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_LUMA_KEY);
    if(fgBilinearBoundary)
        u4RegVal |= IMG_RESZ_LUMA_KEY_BILINEAR_BOUNDARY;
    else
        u4RegVal &= ~IMG_RESZ_LUMA_KEY_BILINEAR_BOUNDARY;        
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LUMA_KEY,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
INT32 i4HwImgReszSetLumaKeyYUVMode(UINT32 u4HwId,BOOL fgYUVMode)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_LUMA_KEY);
    if(fgYUVMode)
        u4RegVal |= IMG_RESZ_LUMA_KEY_YUV_MODE;
    else
        u4RegVal &= ~IMG_RESZ_LUMA_KEY_YUV_MODE;        
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_LUMA_KEY,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) 
INT32 i4HwImgReszSetVENCSKIP(UINT32 u4HwId,BOOL fgSkip)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_VENC_SKIP);
    if(fgSkip)
        u4RegVal |= RM_IMG_RESZ_VENC_SKIP_ON;
    else
        u4RegVal &= ~RM_IMG_RESZ_VENC_SKIP_ON;        
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_VENC_SKIP,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}
#endif

//////////////////////////////////////////////////////////////////////////////////
//
//     DRAM Related
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetDramReqBurstLimit(UINT32 u4HwId,UINT32 u4Limit)
{
    UINT32 u4RegVal;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE);
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    u4RegVal &= ~(IMG_RESZ_DRAM_BURST_LIMIT_1 | IMG_RESZ_DRAM_BURST_LIMIT_2 |
                  IMG_RESZ_DRAM_BURST_LIMIT_4 | IMG_RESZ_DRAM_BURST_LIMIT_8 |
                  IMG_RESZ_DRAM_BURST_LIMIT_16);
#else
    u4RegVal &= ~(IMG_RESZ_DRAM_BURST_LIMIT_1 | IMG_RESZ_DRAM_BURST_LIMIT_2 |
                  IMG_RESZ_DRAM_BURST_LIMIT_4 | IMG_RESZ_DRAM_BURST_LIMIT_8);
#endif

#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8530)
    u4RegVal |= ((u4Limit&0xF)<<4);
#else
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
u4RegVal |= ((u4Limit&0xF)<<8);
#else
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
u4RegVal |= ((u4Limit & 0x1F) << 8);
#endif // (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
#endif //(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
#endif //(CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8530)

#if 0
#ifndef IMGRESZ_HAL_MT8550
#ifdef IMGRESZ_HAL_MT8530
    u4RegVal |= ((u4Limit&0xF)<<8);
#else
    u4RegVal |= ((u4Limit&0xF)<<4);
#endif
#else
    u4RegVal |= ((u4Limit & 0x1F) << 8);
#endif
#endif
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE,u4RegVal);

    return S_IMGRESZ_HAL_OK;
}


//////////////////////////////////////////////////////////////////////////////////
//
//     Temporary Buffer (For partial mode) Related
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetExtend16Store(UINT32 u4HwId, BOOL fgEnable)
{
    UINT32 u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_TGT_BUF_LEN);

    if(fgEnable)
        u4Value |= IMG_RESZ_BOUND_EXTEND_16_ON;
    else
        u4Value &= ~IMG_RESZ_BOUND_EXTEND_16_ON;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_TGT_BUF_LEN, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetSRAMLineBufSize(UINT32 u4HwId, UINT32 u4BufLen)
{
    UINT32 u4Len = u4BufLen;
    UINT32 u4Mask = ~(0x1F<<IMG_RESZ_LINE_BUFFER_LEN_SHIFT);
    UINT32 u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_TGT_BUF_LEN);

    // parameter checking
    if (u4BufLen > 46) //CK ???
        return E_IMGRESZ_HAL_FAIL;

    u4Value &= u4Mask;
    u4Value += (u4Len << IMG_RESZ_LINE_BUFFER_LEN_SHIFT);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_TGT_BUF_LEN, u4Value);

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)


INT32 i4HwImgReszSetTmpLineBufLen(UINT32 u4HwId,IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod,
                                  UINT32 u4SrcHeight,UINT32 u4DestHeight,UINT32 u4DestWidth)
{
    BOOL fgUseExtend16 = 0;
    UINT32 u4LineBufWidth = 11;
    INT32 i4Ret;

    switch(eVResampleMethod)
    {
        case IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR:
            if (u4SrcHeight < u4DestHeight)
            {
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

                fgUseExtend16 = 0;
                u4LineBufWidth = 0x1F;
#else
                fgUseExtend16 = 0;
                u4LineBufWidth = 0x16;
#endif                
            }
            else
            {
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)            
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

                fgUseExtend16 = 1;
                u4LineBufWidth = 0x10;
#else
                fgUseExtend16 = 1;
                u4LineBufWidth = 0xB;
#endif                
            }
            // always do these things,for BDP00119760
            {

                // hardware constrain
                while((u4DestWidth % (u4LineBufWidth << 5)) <= 8/*4*/)
                {
                    u4LineBufWidth--;
                    if(u4LineBufWidth == 0)
                        return E_IMGRESZ_HAL_FAIL;
                }
            }
            break;
        case IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP:
//#if defined(IMGRESZ_HAL_MT8530) || defined(IMGRESZ_HAL_MT8550)  //need more check!   
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)

            fgUseExtend16 = 0;
            u4LineBufWidth = 0x10;
#else
            fgUseExtend16 = 0;
            u4LineBufWidth = 0xB;
#endif            
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    // set extend 16
    i4HwImgReszSetExtend16Store(u4HwId,fgUseExtend16);
    // set temp line buffer length

    if((i4Ret = i4HwImgReszSetSRAMLineBufSize(u4HwId,u4LineBufWidth)) < 0)
        return i4Ret;

    return S_IMGRESZ_HAL_OK;
}

#else

INT32 i4HwImgReszSetTmpLineBufLen(UINT32 u4HwId,IMGRESZ_HAL_RESAMPLE_METHOD_T eVResampleMethod,
                                  UINT32 u4SrcHeight,UINT32 u4DestHeight,UINT32 u4DestWidth,
                                  BOOL fgRPRMode,
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)                              
                                  BOOL fgOSDMode,
#endif                                  
                                  BOOL fgLumaKeyEnable)
{
    BOOL fgUseExtend16 = 0;
    UINT32 u4LineBufWidth = 11;
    INT32 i4Ret;

    switch(eVResampleMethod)
    {
        case IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR:
            if (u4SrcHeight < u4DestHeight)
            {
                if(fgLumaKeyEnable)          
                   u4LineBufWidth = 0x1E;
		else
                   u4LineBufWidth = 0x1F;
                fgUseExtend16 = 0;              
            }
            else
            {
                fgUseExtend16 = 1;
                u4LineBufWidth = 0x10;             
            }
            // always do these things,for BDP00119760
            {

                // hardware constrain
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8560)  
                if(fgOSDMode)
                {
                    while(((u4DestWidth*4) % (u4LineBufWidth << 5)) <= 8/*4*/)
                    {
                        u4LineBufWidth--;
                        if(u4LineBufWidth == 0)
                            return E_IMGRESZ_HAL_FAIL;
                    }
                }else
#endif                
                {
                    while((u4DestWidth % (u4LineBufWidth << 5)) <= 8/*4*/)
                    {
                        u4LineBufWidth--;
                        if(u4LineBufWidth == 0)
                            return E_IMGRESZ_HAL_FAIL;
                    }
                }
            }
            break;
        case IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP:
            fgUseExtend16 = 0;
            u4LineBufWidth = 0x10;         
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    // set extend 16
    i4HwImgReszSetExtend16Store(u4HwId,fgUseExtend16);
    // set temp line buffer length

    if (fgRPRMode)
    {
        u4LineBufWidth = 0x10;
    }

    if((i4Ret = i4HwImgReszSetSRAMLineBufSize(u4HwId,u4LineBufWidth)) < 0)
        return i4Ret;

    return S_IMGRESZ_HAL_OK;
}
#endif


INT32 i4HwImgReszSetTempBufAddr(UINT32 u4HwId,UINT32 u4Addr)
{
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if(u4HwId == 1)
    {
        u4Addr = u4Addr ? PHYSICAL(u4Addr) : 0;
    }
    else
#endif
    {
#if IMGRESZ_IOMMU_SOLUTION2_SOLUTION3 //for before 8560ECO)
       {
#if !IMGRESZ_IO_MMU_TEST
        u4Addr = u4Addr ? PHYSICAL(u4Addr) : 0;
#endif
       }
#else
       {
          u4Addr = u4Addr ? PHYSICAL(u4Addr) : 0; //MT8560ECO, tmp no mmu
       }
#endif       
    }
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TMP_ADDR_BASE,(u4Addr>>4));

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetJpegPicMode(UINT32 u4HwId,BOOL fgPicMode)
{
    if(fgPicMode)
    	{
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_INTERFACE_SWITCH,IMG_RESZ_TRACKING_WITH_JPG_HW);
    	}
    else
    	{
        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_INTERFACE_SWITCH,IMG_RESZ_NOT_TRACKING_WITH_JPG_HW);
    	}
    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetJpegPreloadMode(UINT32 u4HwId,BOOL fgPreloadMode)
{
    UINT32 u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);

    if(fgPreloadMode)
        u4Value |= IMG_RESZ_PRELOAD_DRAM_DATA;
    else
        u4Value &= ~IMG_RESZ_PRELOAD_DRAM_DATA;

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetJpegComponent(UINT32 u4HwId,BOOL fgYExist,BOOL fgCbExist,BOOL fgCrExist)
{
    UINT32 u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);

    if(fgYExist)
        u4Value |= IMG_RESZ_RECORD_Y;
    else
        u4Value &= ~IMG_RESZ_RECORD_Y;

    if(fgCbExist)
        u4Value |= IMG_RESZ_RECORD_CB;
    else
        u4Value &= ~IMG_RESZ_RECORD_CB;

    if(fgCrExist)
        u4Value |= IMG_RESZ_RECORD_CR;
    else
        u4Value &= ~IMG_RESZ_RECORD_CR;

#if 0
    if((!(fgYExist && fgCbExist && fgCrExist)) ||fgRPRRacingModeEnable)
    {
        u4Value &= 0xFFFFF000;
        u4Value |= 0x0 << 10;
        u4Value |= 0x0 & 3 << 8;
        u4Value |= 0x0 & 3 << 6;
        u4Value |= 0x0 & 3 << 4;
        u4Value |= 0x0 & 3 << 2;
        u4Value |= 0x0 & 3 << 0;
    }

#endif
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetJpegComponentExt(UINT32 u4HwId,BOOL fgYExist,BOOL fgCbExist,BOOL fgCrExist,BOOL fgRPRRacingModeEnable)
{
    UINT32 u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_JPG_MODE);

    if((!(fgYExist && fgCbExist && fgCrExist)) ||fgRPRRacingModeEnable)
    {
        u4Value &= 0xFFFFF000;
        u4Value |= 0x0 << 10;
        u4Value |= 0x0 & 3 << 8;
        u4Value |= 0x0 & 3 << 6;
        u4Value |= 0x0 & 3 << 4;
        u4Value |= 0x0 & 3 << 2;
        u4Value |= 0x0 & 3 << 0;

        vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_JPG_MODE,u4Value);
    }
    
    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetCbCrExit(UINT32 u4HwId, BOOL fgCb, BOOL fgCr)

{

  UINT32 u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_TYPE);

  if (!fgCb)
  	u4Value |= IMG_RESZ_CB_PADDING;
  if (!fgCr)
  	u4Value |= IMG_RESZ_CR_PADDING;
  
  vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_TYPE,u4Value);
  
  return S_IMGRESZ_HAL_OK;

}
//////////////////////////////////////////////////////////////////////////////////
//
//     Index Buffer Related
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszSetIndexBufColorTranslation(UINT32 u4HwId)
{
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_COLOR_TRANSLATION0,0x03020100); // For 2BPP and 4BPP
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_COLOR_TRANSLATION1,0x07060504); // For 4BPP only
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_COLOR_TRANSLATION2,0x0B0A0908); // For 4BPP only
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_OSD_COLOR_TRANSLATION3,0x0F0E0D0C); // For 4BPP only

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetColorPalletTable(UINT32 u4HwId,UINT32 u4ColorPalletTableAddr)
{
    UINT32 u4i=0;
    UINT8 *pu1GblColTbl = (UINT8*)u4ColorPalletTableAddr;
    UINT32 u4GifScaleCptVal = 0;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, 0); mb();
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, IMG_RESZ_OSD_CPT_ENABLE);mb();
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_CPT_ADDR, 0);mb();
    for (u4i=0; u4i<256; u4i++)
    {
#if CONFIG_DRV_VERIFY_SUPPORT
        u4GifScaleCptVal = (pu1GblColTbl[u4i*4+0] << 24);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+1] << 16);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+2] << 8);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+3]);
#else
        u4GifScaleCptVal = (pu1GblColTbl[u4i*4+3] << 24);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+2] << 16);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+1] << 8);
        u4GifScaleCptVal = u4GifScaleCptVal + (pu1GblColTbl[u4i*4+0]);
#endif
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_CPT_DATA, u4GifScaleCptVal);
    }
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, 0); mb();
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, IMG_RESZ_OSD_ED_CPT); mb();
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, IMG_RESZ_OSD_CPT_ENABLE + IMG_RESZ_OSD_ED_CPT); mb();

    return S_IMGRESZ_HAL_OK;
}


//////////////////////////////////////////////////////////////////////////////////
//
//     Scale Factro Setting
//
//////////////////////////////////////////////////////////////////////////////////
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)

INT32 i4HwImgReszSetH8TapsScale_Y(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFE000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x00001FFF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_Y, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetH8TapsScale_Cb(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFE000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x00001FFF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_CB, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetH8TapsScale_Cr(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFE000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x00001FFF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_CR, u4Value);

    return S_IMGRESZ_HAL_OK;
}
#else
INT32 i4HwImgReszSetH8TapsScale_Y(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;
     // parameter checking
    if (0 != (u4Factor & 0xFC000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_Y, u4Value);

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAP_OFSET_Y, u4Offset);
    
    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetH8TapsScale_Cb(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;

     // parameter checking
    if (0 != (u4Factor & 0xFC000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_CB, u4Value);

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAP_OFSET_CB, u4Offset);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetH8TapsScale_Cr(UINT32 u4HwId,UINT32 u4Offset,UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;

     // parameter checking
    if (0 != (u4Factor & 0xFC000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAPS_SCL_CR, u4Value);

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H8TAP_OFSET_CR, u4Offset);

    return S_IMGRESZ_HAL_OK;

}
#endif


INT32 i4HwImgReszSetH8tapsCoefficients(UINT32 u4HwId, IMGRZ_SCALE_FACTOR eFactor)
{
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF0, _au4IMGRZ_FILTERCOEF_H[eFactor][0]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF1, _au4IMGRZ_FILTERCOEF_H[eFactor][1]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF2, _au4IMGRZ_FILTERCOEF_H[eFactor][2]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF3, _au4IMGRZ_FILTERCOEF_H[eFactor][3]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF4, _au4IMGRZ_FILTERCOEF_H[eFactor][4]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF5, _au4IMGRZ_FILTERCOEF_H[eFactor][5]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF6, _au4IMGRZ_FILTERCOEF_H[eFactor][6]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF7, _au4IMGRZ_FILTERCOEF_H[eFactor][7]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF8, _au4IMGRZ_FILTERCOEF_H[eFactor][8]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF9, _au4IMGRZ_FILTERCOEF_H[eFactor][9]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF10, _au4IMGRZ_FILTERCOEF_H[eFactor][10]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF11, _au4IMGRZ_FILTERCOEF_H[eFactor][11]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF12, _au4IMGRZ_FILTERCOEF_H[eFactor][12]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF13, _au4IMGRZ_FILTERCOEF_H[eFactor][13]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF14, _au4IMGRZ_FILTERCOEF_H[eFactor][14]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF15, _au4IMGRZ_FILTERCOEF_H[eFactor][15]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF16, _au4IMGRZ_FILTERCOEF_H[eFactor][16]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_H_COEF17, _au4IMGRZ_FILTERCOEF_H[eFactor][17]);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetV4tapsCoefficients(UINT32 u4HwId, IMGRZ_SCALE_FACTOR eFactor)
{
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF0, _au4IMGRZ_FILTERCOEF_V[eFactor][0]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF1, _au4IMGRZ_FILTERCOEF_V[eFactor][1]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF2, _au4IMGRZ_FILTERCOEF_V[eFactor][2]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF3, _au4IMGRZ_FILTERCOEF_V[eFactor][3]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF4, _au4IMGRZ_FILTERCOEF_V[eFactor][4]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF5, _au4IMGRZ_FILTERCOEF_V[eFactor][5]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF6, _au4IMGRZ_FILTERCOEF_V[eFactor][6]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF7, _au4IMGRZ_FILTERCOEF_V[eFactor][7]);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_COEF8, _au4IMGRZ_FILTERCOEF_V[eFactor][8]);

    return S_IMGRESZ_HAL_OK;
}


IMGRZ_SCALE_FACTOR eHwImgReszCalcScaleFactor(UINT32 u4SrcSize, UINT32 u4TgtSize)
{
    UINT32 u4Scale = u4TgtSize * 10000 / u4SrcSize;
    if (u4Scale >= 10000)
      return FACTOR_1;
    else if (u4Scale >= 5000)
      return FACTOR_0_5;
    else if (u4Scale >= 2500)
      return FACTOR_0_25;
    else if (u4Scale >= 1250)
      return FACTOR_0_125;
    else if (u4Scale >= 625)
      return FACTOR_0_0625;
    else
      return FACTOR_0;
}


INT32 i4HwImgReszSetHecScale_Y(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x00000FFF;

    u4Value += ((u4Offset & 0x000007FF) << 12);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_HSA_SCL_Y, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetHecScale_Cb(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x00000FFF;

    u4Value += ((u4Offset & 0x000007FF) << 12);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_HSA_SCL_CB, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetHecScale_Cr(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x00000FFF;

    u4Value += ((u4Offset & 0x000007FF) << 12);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_HSA_SCL_CR, u4Value);

    return S_IMGRESZ_HAL_OK;
}

//#ifndef IMGRESZ_HAL_MT8550 
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)

INT32 i4HwImgReszSetV4TapsScale_Y(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFF800)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_Y, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetV4TapsScale_Cb(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFF800)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_CB, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetV4TapsScale_Cr(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x0007FFFF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFF80000)) || ( 0 != (u4Offset & 0xFFFFF800)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 19);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_CR, u4Value);

    return S_IMGRESZ_HAL_OK;
}
#else 
INT32 i4HwImgReszSetV4TapsScale_Y(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;

     // parameter checking
    if (0 != (u4Factor & 0xFC000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_Y, u4Value);

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAP_OFSET_Y, u4Offset);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetV4TapsScale_Cb(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;

     // parameter checking
    if (0 != (u4Factor & 0xF3000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_CB, u4Value);

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAP_OFSET_C, u4Offset);
    
    return S_IMGRESZ_HAL_OK;
}

// Maybe the following function is not needed for v4-Tap is not supported in JPEG mode.
INT32 i4HwImgReszSetV4TapsScale_Cr(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor)
{
    UINT32 u4Value = u4Factor & 0x03FFFFFF;

     // parameter checking
    if (0 != (u4Factor & 0xF3000000))
        return E_IMGRESZ_HAL_FAIL;

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAPS_SCL_CR, u4Value);

    ///Maybe it is not needed.Check it.
    //vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V4TAP_OFSET_C, u4Offset); 

    return S_IMGRESZ_HAL_OK;
}
#endif


INT32 i4HwImgReszSetVScale_Y(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x000007FF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFFFF800)) || ( 0 != (u4Offset & 0xFFFFF000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 12) + ((fgScaleUp & 0x1) << 11);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_SCL_Y, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetVScale_Cb(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x000007FF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFFFF800)) || ( 0 != (u4Offset & 0xFFFFF000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 12) + ((fgScaleUp & 0x1) << 11);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_SCL_CB, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetVScale_Cr(UINT32 u4HwId, UINT32 u4Offset, UINT32 u4Factor, BOOL fgScaleUp)
{
    UINT32 u4Value = u4Factor & 0x000007FF;

     // parameter checking
    if ((0 != (u4Factor & 0xFFFFF800)) || ( 0 != (u4Offset & 0xFFFFF000)))
        return E_IMGRESZ_HAL_FAIL;

    u4Value += ((u4Offset & 0x000007FF) << 12) + ((fgScaleUp & 0x1) << 11);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_V_SCL_CR, u4Value);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetHScaleFactor(UINT32 u4HwId,UINT32 u4SrcWidth,UINT32 u4TgtWidth,
                                 IMGRESZ_HAL_RESAMPLE_METHOD_T eResampleMethod,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat)
{
    UINT32 u4Factor, u4Offset;
    UINT32 u4SrcWidthY = 0,u4SrcWidthCb = 0,u4SrcWidthCr = 0;
    UINT32 u4TgtWidthY = 0,u4TgtWidthCb = 0,u4TgtWidthCr = 0;

    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            switch(prSrcBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                    u4SrcWidthY = u4SrcWidth;
                    u4SrcWidthCb = u4SrcWidth >> 1;
                    u4SrcWidthCr = u4SrcWidth >> 1;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_444:
                    u4SrcWidthY = u4SrcWidth;
                    u4SrcWidthCb = u4SrcWidth;
                    u4SrcWidthCr = u4SrcWidth;
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            u4SrcWidthY = u4SrcWidth;
            u4SrcWidthCb = u4SrcWidth * prSrcBufferFormat->u4HSampleFactor[1] / prSrcBufferFormat->u4HSampleFactor[0];
            u4SrcWidthCr = u4SrcWidth * prSrcBufferFormat->u4HSampleFactor[2] / prSrcBufferFormat->u4HSampleFactor[0];
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            u4SrcWidthY = u4SrcWidth;
            break;
    }

    switch(prDestBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            switch(prDestBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                    u4TgtWidthY = u4TgtWidth;
                    u4TgtWidthCb = u4TgtWidth >> 1;
                    u4TgtWidthCr = u4TgtWidth >> 1;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_444:
                    u4TgtWidthY = u4TgtWidth;
                    u4TgtWidthCb = u4TgtWidth;
                    u4TgtWidthCr = u4TgtWidth;
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            u4TgtWidthY = u4TgtWidth;
            u4TgtWidthCb = u4TgtWidth;
            u4TgtWidthCr = u4TgtWidth;
            break;
        default:
            break;
    }

    // Y
    if ((eResampleMethod == IMGRESZ_HAL_RESAMPLE_METHOD_8_TAP) || (u4SrcWidthY <= u4TgtWidthY))
    {
        if(u4SrcWidthY == u4TgtWidthY)
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcWidthY + (u4TgtWidthY >> 1)) / u4TgtWidthY;
#else
        u4Factor = (2048 * u4SrcWidthY + (u4TgtWidthY >> 1)) / u4TgtWidthY;
#endif
        else
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * (u4SrcWidthY - 1) + ((u4TgtWidthY - 1) >> 1)) / (u4TgtWidthY - 1); // For centralize
#else
            u4Factor = (2048 * (u4SrcWidthY - 1) + ((u4TgtWidthY - 1) >> 1)) / (u4TgtWidthY - 1); // For centralize
#endif
        u4Offset = 0;
        i4HwImgReszSetH8TapsScale_Y(u4HwId, u4Offset, u4Factor);
        i4HwImgReszSetH8tapsCoefficients(u4HwId,eHwImgReszCalcScaleFactor(u4SrcWidth, u4TgtWidth));
        i4HwImgReszSetHecScale_Y(u4HwId,0,0x800,TRUE);
    }
    else
    {
        u4Factor = (2048 * u4TgtWidthY + (u4SrcWidthY >> 1)) / u4SrcWidthY;
        u4Offset = 2048 - u4Factor;
        i4HwImgReszSetHecScale_Y(u4HwId, u4Offset, u4Factor, TRUE);
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        i4HwImgReszSetH8TapsScale_Y(u4HwId, 0, 0x40000);
#else
        i4HwImgReszSetH8TapsScale_Y(u4HwId, 0, 0x800);
#endif
    }

    if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER) ||
       (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER) ||
       (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER))
        return S_IMGRESZ_HAL_OK;

    // Cb
    if ((eResampleMethod == IMGRESZ_HAL_RESAMPLE_METHOD_8_TAP) || (u4SrcWidthCb <= u4TgtWidthCb))
    {
        if((u4SrcWidthCb == u4TgtWidthCb) || (u4TgtWidthCb == 1))
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcWidthCb + (u4TgtWidthCb >> 1)) / u4TgtWidthCb;
#else
        u4Factor = (2048 * u4SrcWidthCb + (u4TgtWidthCb >> 1)) / u4TgtWidthCb;
#endif
        else
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * (u4SrcWidthCb - 1) + ((u4TgtWidthCb - 1) >> 1)) / (u4TgtWidthCb - 1); // For centralize
#else
            u4Factor = (2048 * (u4SrcWidthCb - 1) + ((u4TgtWidthCb - 1) >> 1)) / (u4TgtWidthCb - 1); // For centralize
#endif
        u4Offset = 0;
        i4HwImgReszSetH8TapsScale_Cb(u4HwId, u4Offset, u4Factor);
        i4HwImgReszSetHecScale_Cb(u4HwId,0,0x800,TRUE);
    }
    else
    {
        u4Factor = (2048 * u4TgtWidthCb + (u4SrcWidthCb >> 1)) / u4SrcWidthCb;
        u4Offset = 2048 - u4Factor;
        i4HwImgReszSetHecScale_Cb(u4HwId, u4Offset, u4Factor, TRUE);
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        i4HwImgReszSetH8TapsScale_Cb(u4HwId, 0, 0x40000);
#else
        i4HwImgReszSetH8TapsScale_Cb(u4HwId, 0, 0x800);
#endif
    }

    if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER)
        return S_IMGRESZ_HAL_OK;

    // Cr
    if ((eResampleMethod == IMGRESZ_HAL_RESAMPLE_METHOD_8_TAP) || (u4SrcWidthCr <= u4TgtWidthCr))
    {
        if((u4SrcWidthCr == u4TgtWidthCr) || (u4TgtWidthCb == 1))
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcWidthCr + (u4TgtWidthCr >> 1)) / u4TgtWidthCr;
#else
        u4Factor = (2048 * u4SrcWidthCr + (u4TgtWidthCr >> 1)) / u4TgtWidthCr;
#endif
        else
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * (u4SrcWidthCr - 1) + ((u4TgtWidthCr - 1) >> 1)) / (u4TgtWidthCr - 1); // For centralize
#else
            u4Factor = (2048 * (u4SrcWidthCr - 1) + ((u4TgtWidthCr - 1) >> 1)) / (u4TgtWidthCr - 1); // For centralize
#endif
        u4Offset = 0;
        i4HwImgReszSetH8TapsScale_Cr(u4HwId, u4Offset, u4Factor);
        i4HwImgReszSetHecScale_Cr(u4HwId,0,0x800,TRUE);
    }
    else
    {
        u4Factor = (2048 * u4TgtWidthCr + (u4SrcWidthCr >> 1)) / u4SrcWidthCr;
        u4Offset = 2048 - u4Factor;
        i4HwImgReszSetHecScale_Cr(u4HwId, u4Offset, u4Factor, TRUE);
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        i4HwImgReszSetH8TapsScale_Cr(u4HwId, 0, 0x40000);
#else
        i4HwImgReszSetH8TapsScale_Cr(u4HwId, 0, 0x800);
#endif
    }

    return S_IMGRESZ_HAL_OK;
}

#if IMGRESZ_VIDEO_MODE_SUPPORT_WEBP
INT32 i4HwImgReszSetVScaleFactorWebpTap4(UINT32 u4HwId,
                                         UINT32 u4SrcHeight,
                                         UINT32 u4TgtHeight,
                                         UINT32 u4TgtStartY)
{
    UINT32 u4RatioHeightY = (0x40000 * u4SrcHeight + (u4TgtHeight>>1))/u4TgtHeight; // same as video mode
    UINT32 u4RatioHeightC = (0x40000 * (u4SrcHeight>>1) +(u4TgtHeight>>2))/(u4TgtHeight>>1);  //

    UINT32 u4Temp = u4TgtStartY * u4RatioHeightY;
    UINT32 u4Tap4Offset = (u4Temp & 0x3ffff) + 0x40000;  	 // temp[17:0] + ¡®h40000
    i4HwImgReszSetV4TapsScale_Y(u4HwId, u4Tap4Offset, u4RatioHeightY);
    u4Temp = (u4TgtStartY>>1) * u4RatioHeightC;
    u4Tap4Offset = (u4Temp & 0x3ffff) + 0x40000;  	 // temp[17:0] + ¡®h40000   
    i4HwImgReszSetV4TapsScale_Cb(u4HwId, u4Tap4Offset, u4RatioHeightC);  
    //i4HwImgReszSetV4TapsScale_Cr(u4HwId, u4Tap4Offset, u4RatioHeightC);  
    return S_IMGRESZ_HAL_OK;
}

#endif

INT32 i4HwImgReszSetVScaleFactor(UINT32 u4HwId,
                                 UINT32 u4SrcHeight,UINT32 u4TgtHeight,
                                 IMGRESZ_HAL_RESAMPLE_METHOD_T eResampleMethod,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prDestBufferFormat)
{
    UINT32 u4Factor, u4Offset;
    UINT32 u4SrcHeightY = 0,u4SrcHeightCb = 0,u4SrcHeightCr = 0;
    UINT32 u4TgtHeightY = 0,u4TgtHeightCb = 0,u4TgtHeightCr = 0;

    switch(prSrcBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            switch(prSrcBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                    u4SrcHeightY = u4SrcHeight;
                    u4SrcHeightCb = u4SrcHeight >> 1;
                    u4SrcHeightCr = u4SrcHeight >> 1;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                case IMGRESZ_HAL_IMG_YUV_FORMAT_444:
                    u4SrcHeightY = u4SrcHeight;
                    u4SrcHeightCb = u4SrcHeight;
                    u4SrcHeightCr = u4SrcHeight;
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_CB_CR_BUFFER:
            u4SrcHeightY = u4SrcHeight;
            u4SrcHeightCb = u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[1] / prSrcBufferFormat->u4VSampleFactor[0];
            u4SrcHeightCr = u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[2] / prSrcBufferFormat->u4VSampleFactor[0];
            
            // For jpeg picture mode, prevent source height 401 come two interrupt (Y interrupt and C interrupt)
            if((u4SrcHeightCb * prSrcBufferFormat->u4VSampleFactor[0]) != (u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[1]))
                u4SrcHeightCb++;
            if((u4SrcHeightCr * prSrcBufferFormat->u4VSampleFactor[0]) != (u4SrcHeight * prSrcBufferFormat->u4VSampleFactor[2]))
                u4SrcHeightCr++;
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            u4SrcHeightY = u4SrcHeight;
            break;
    }

    switch(prDestBufferFormat->eBufferMainFormat)
    {
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER:
            switch(prDestBufferFormat->eYUVFormat)
            {
                case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                    u4TgtHeightY = u4TgtHeight;
                    u4TgtHeightCb = u4TgtHeight >> 1;
                    u4TgtHeightCr = u4TgtHeight >> 1;
                    break;
                case IMGRESZ_HAL_IMG_YUV_FORMAT_422:
                case IMGRESZ_HAL_IMG_YUV_FORMAT_444:
                    u4TgtHeightY = u4TgtHeight;
                    u4TgtHeightCb = u4TgtHeight;
                    u4TgtHeightCr = u4TgtHeight;
                    break;
            }
            break;
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER:
        case IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER:
            u4TgtHeightY = u4TgtHeight;
            u4TgtHeightCb = u4TgtHeight;
            u4TgtHeightCr = u4TgtHeight;
            break;
        default:
            break;
    }

    switch(eResampleMethod)
    {
        case IMGRESZ_HAL_RESAMPLE_METHOD_4_TAP:
            // Y
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcHeightY + (u4TgtHeightY >> 1)) / u4TgtHeightY;
#else
            u4Factor = (2048 * u4SrcHeightY + (u4TgtHeightY >> 1)) / u4TgtHeightY;
#endif
            u4Offset= 0;
            i4HwImgReszSetV4TapsScale_Y(u4HwId, u4Offset, u4Factor);

            if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER) ||
               (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER) ||
               (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER))
                break;

            // Cb
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcHeightCb + (u4TgtHeightCb >> 1)) / u4TgtHeightCb;
#else
            u4Factor = (2048 * u4SrcHeightCb + (u4TgtHeightCb >> 1)) / u4TgtHeightCb;
#endif
            u4Offset= 0;
            i4HwImgReszSetV4TapsScale_Cb(u4HwId, u4Offset, u4Factor);

            if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER)
                break;

            // Cr
//#ifdef IMGRESZ_HAL_MT8550
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            u4Factor = (0x40000 * u4SrcHeightCr + (u4TgtHeightCr >> 1)) / u4TgtHeightCr;
#else
            u4Factor = (2048 * u4SrcHeightCr + (u4TgtHeightCr >> 1)) / u4TgtHeightCr;
#endif
            u4Offset= 0;
            i4HwImgReszSetV4TapsScale_Cr(u4HwId, u4Offset, u4Factor);
            //
            i4HwImgReszSetV4tapsCoefficients(u4HwId, eHwImgReszCalcScaleFactor(u4SrcHeight, u4TgtHeight));
            break;
        case IMGRESZ_HAL_RESAMPLE_METHOD_BILINEAR:
            // Y
            if (u4TgtHeightY < u4SrcHeightY)  //scale dowm, source accumulator
            {
                u4Factor = (2048 * u4TgtHeightY + (u4SrcHeightY >> 1)) / u4SrcHeightY;
                u4Offset = 2048 - u4Factor;
#if IMGRESZ_HAL_SUBLINE_ADJUST
                // Adjust subline.
                if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER) &&
                   (!prSrcBufferFormat->fgProgressiveFrame) &&
                   (!prSrcBufferFormat->fgTopField))
                {
                    switch(prSrcBufferFormat->eYUVFormat)
                    {
                        case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                            {
				    UINT32 u4Value;			
                                UINT32 u4Q = (u4SrcHeight - u4TgtHeight) /(u4TgtHeight * 2 );
                                UINT32 u4R_norm =  ( ((u4SrcHeight - u4TgtHeight) - (u4Q * u4TgtHeight *2)) * 2048 ) / (2 * u4SrcHeight );                    

                                u4Offset += u4R_norm;
								
				    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_SRC_SIZE_Y);
				    u4Value &=0xFFFF0000;
				    u4Value |=((u4SrcHeightY -u4Q)&0xFFFF);
				    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_Y,u4Value);
                            }
                            break;
                        default:
                            break;
                    }
                }
#endif                
                i4HwImgReszSetVScale_Y(u4HwId, u4Offset, u4Factor, FALSE);
            }
            else    //scale up, bilinear
            {
                if(u4TgtHeightY == u4SrcHeightY)
                {
                    u4Factor = 0;
                    u4Offset = 0;
                } else
                {
                    //u4Factor = (2048 * u4SrcHeightY + (u4TgtHeightY >> 1)) / u4TgtHeightY;
                    u4Factor = (2048 * (u4SrcHeightY - 1) + ((u4TgtHeightY - 1) >> 1)) / (u4TgtHeightY - 1); // For centralize
                    u4Offset = 0;
                }
                i4HwImgReszSetVScale_Y(u4HwId, u4Offset, u4Factor, TRUE);
            }

            if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER) ||
               (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER) ||
               (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER))
                break;

            // Cb
            if (u4TgtHeightCb < u4SrcHeightCb)  //scale dowm, source accumulator
            {
                u4Factor = (2048 * u4TgtHeightCb + (u4SrcHeightCb >> 1)) / u4SrcHeightCb;
                u4Offset = 2048 - u4Factor;
#if IMGRESZ_HAL_SUBLINE_ADJUST                
                // Adjust subline.
                if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER) &&
                   (!prSrcBufferFormat->fgProgressiveFrame) &&
                   (!prSrcBufferFormat->fgTopField))                    
                {
                    switch(prSrcBufferFormat->eYUVFormat)
                    {
                        case IMGRESZ_HAL_IMG_YUV_FORMAT_420:
                            {
				    UINT32 u4Value;
                                UINT32 u4Q = (u4SrcHeight - u4TgtHeight) /(u4TgtHeight * 2 );
                                UINT32 u4R_norm =  ( ((u4SrcHeight - u4TgtHeight) - (u4Q * u4TgtHeight *2)) * 2048 ) / (2 * u4SrcHeight );                    

                                u4Offset += u4R_norm;
								
				    u4Value = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CB);
				    u4Value &=0xFFFF0000;
				    u4Value |=((u4SrcHeightCb-u4Q)&0xFFFF);
				    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_SRC_SIZE_CB,u4Value);
                            }
                            break;
                        default:
                            break;
                    }
                }                
#endif                
                i4HwImgReszSetVScale_Cb(u4HwId, u4Offset, u4Factor, FALSE);
            }
            else    //scale up, bilinear
            {
                if(u4TgtHeightCb == u4SrcHeightCb)
                {
                    u4Factor = 0;
                    u4Offset = 0;
                } else
                {
                    //u4Factor = (2048 * u4SrcHeightCb + (u4TgtHeightCb >> 1)) / u4TgtHeightCb;
                    u4Factor = (2048 * (u4SrcHeightCb - 1) + ((u4TgtHeightCb - 1) >> 1)) / (u4TgtHeightCb - 1); // For centralize
                    u4Offset = 0;
                }
                i4HwImgReszSetVScale_Cb(u4HwId, u4Offset, u4Factor, TRUE);
            }

            if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER)
                break;

            // Cr
            if (u4TgtHeightCr < u4SrcHeightCr)  //scale dowm, source accumulator
            {
                u4Factor = (2048 * u4TgtHeightCr + (u4SrcHeightCr >> 1)) / u4SrcHeightCr;
                u4Offset = 2048 - u4Factor;
                i4HwImgReszSetVScale_Cr(u4HwId, u4Offset, u4Factor, FALSE);
            }
            else    //scale up, bilinear
            {
                if(u4TgtHeightCr == u4SrcHeightCr)
                {
                    u4Factor = 0;
                    u4Offset = 0;
                } else
                {
                    //u4Factor = (2048 * u4SrcHeightCr + (u4TgtHeightCr >> 1)) / u4TgtHeightCr;
                    u4Factor = (2048 * u4SrcHeightCr + (u4TgtHeightCr >> 1)) / u4TgtHeightCr; // For centralize
                    u4Offset = 0;
                }
                i4HwImgReszSetVScale_Cr(u4HwId, u4Offset, u4Factor, TRUE);
            }
            break;
        default:
            return E_IMGRESZ_HAL_FAIL;
    }

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)


INT32 i4HwImgReszSetRPRHScaleFactor(UINT32 u4HwId,INT32 i4SrcWidth,INT32 i4TgtWidth)
{
    UINT32 u4Factor, u4Offset;
   // UINT32 u4SrcWidthY = 0,u4SrcWidthCb = 0,u4SrcWidthCr = 0;
    //UINT32 u4TgtWidthY = 0,u4TgtWidthCb = 0,u4TgtWidthCr = 0;

    INT32 m = 0;
    INT32 u4Temp;
    INT32 Hprime;
    INT32 D;
    INT32   i4UxR;
    INT32 u4Ax_Initial;
    INT32 u4Ax_Increment;

	
   // u4SrcWidthY = u4SrcWidth;
    //u4SrcWidthCb = u4SrcWidth >> 1;
    //u4TgtWidthY = u4TgtWidth;
    //u4TgtWidthCb = u4TgtWidth >> 1;
	
    u4Temp = i4SrcWidth;
    while (u4Temp > 0)
    {
      m=m+1;
      u4Temp = u4Temp>>1;
    }
    /* check for case when u4SrcWidth is power of two */
    if (i4SrcWidth == (1<<(m-1))) 
	m=m-1;
     Hprime = 1<<m;
     D = (64*Hprime)/16;
    

  /* iUxL and iUxR are independent of row, so compute once only */
      i4UxR = ((((i4SrcWidth - i4TgtWidth)<<1))<<(4+m));    /* numerator part */
  /* complete iUxR init by dividing by H with rounding to nearest integer, */
  /* half-integers away from 0 */
   
     if (i4UxR >= 0)
       i4UxR = (i4UxR + (i4TgtWidth>>1))/i4TgtWidth;
     else
       i4UxR = (i4UxR - (i4TgtWidth>>1))/i4TgtWidth;

    

  /* initial x displacement and the x increment are independent of row */
  /* so compute once only */
       u4Ax_Initial = i4UxR + (D>>1);
       u4Ax_Increment = (Hprime<<6) + (i4UxR<<1);


	u4Factor = u4Ax_Initial<<(18-(m+6));
       u4Offset = u4Ax_Increment<<(18-(m+6));
    
	i4HwImgReszSetH8TapsScale_Y(u4HwId,u4Factor, u4Offset);
	i4HwImgReszSetH8TapsScale_Cb(u4HwId,u4Factor, u4Offset);
    i4HwImgReszSetH8tapsCoefficients(u4HwId,FACTOR_RM);
    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetRPRVScaleFactor(UINT32 u4HwId,INT32 i4SrcHeight,INT32 i4TgtHeight)
{
    UINT32 u4Factor, u4Offset;
    //UINT32 u4SrcHeightY = 0,u4SrcHeightCb = 0,u4SrcHeightCr = 0;
    //UINT32 u4TgtHeightY = 0,u4TgtHeightCb = 0,u4TgtHeightCr = 0;

    INT32 n = 0;
    INT32 u4Temp;
    INT32   i4UyLB;
    INT32   i4UyL_inc;
	
   // u4SrcHeightY = u4SrcHeight;
    //u4SrcHeightCb = u4SrcHeight >> 1;
    //u4TgtHeightY = u4TgtHeight;
    //u4TgtHeightCb = u4TgtHeight >> 1;
	
    u4Temp = i4SrcHeight;
    while (u4Temp > 0)
    {
      n=n+1;
      u4Temp = u4Temp>>1;
    }
	
     /* check for case when uInHeight is power of two */
     if (i4SrcHeight == (1<<(n-1))) 
	n=n-1;
    

     i4UyLB = ((i4SrcHeight - i4TgtHeight)<<(n+5)); /* numerator */
     /* complete iUyLB by dividing by V with rounding to nearest integer, */
     /* half-integers away from 0 */
     if (i4UyLB >= 0)
       i4UyLB = (i4UyLB + (i4TgtHeight>>1))/i4TgtHeight;
     else
       i4UyLB = (i4UyLB - (i4TgtHeight>>1))/i4TgtHeight;
       i4UyL_inc = (i4UyLB<<1);

       u4Factor = ((1<<(6+n)) + i4UyL_inc) << (18-(n+6));
       u4Offset = (i4UyLB + (1<<(1+n))) << (18-(n+6));

	i4HwImgReszSetV4TapsScale_Y(u4HwId, u4Offset, u4Factor);
	i4HwImgReszSetV4TapsScale_Cb(u4HwId, u4Offset, u4Factor);
    i4HwImgReszSetV4tapsCoefficients(u4HwId, FACTOR_RM);
    return S_IMGRESZ_HAL_OK;
}
#endif

INT32 i4HwImgReszSetScale1to1Factor(UINT32 u4HwId,
                                 IMGRESZ_HAL_IMG_BUF_FORMAT_T *prSrcBufferFormat)
                                                                                                                
{	
      // Y
      i4HwImgReszSetH8TapsScale_Y(u4HwId, 0, 0x40000);
      i4HwImgReszSetV4TapsScale_Y(u4HwId, 0, 0x40000);
      i4HwImgReszSetHecScale_Y(u4HwId,0,0x800,TRUE);
      i4HwImgReszSetVScale_Y(u4HwId,0,0x0,TRUE);   
	  
    if((prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_ARGB_BUFFER) ||
       (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_AYUV_BUFFER) ||
       (prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_INDEX_BUFFER))
        return S_IMGRESZ_HAL_OK;

    // Cb
     i4HwImgReszSetH8TapsScale_Cb(u4HwId, 0, 0x40000);	
     i4HwImgReszSetV4TapsScale_Cb(u4HwId, 0, 0x40000);
     i4HwImgReszSetHecScale_Cb(u4HwId,0,0x800,TRUE);
     i4HwImgReszSetVScale_Cb(u4HwId,0,0x0,TRUE); 
	 
    if(prSrcBufferFormat->eBufferMainFormat == IMGRESZ_HAL_IMG_BUF_MAIN_FORMAT_Y_C_BUFFER)
        return S_IMGRESZ_HAL_OK;

    // Cr
     i4HwImgReszSetH8TapsScale_Cr(u4HwId, 0, 0x40000);	
     i4HwImgReszSetV4TapsScale_Cr(u4HwId, 0, 0x40000);  
     i4HwImgReszSetHecScale_Cr(u4HwId,0,0x800,TRUE);
     i4HwImgReszSetVScale_Cr(u4HwId,0,0x0,TRUE); 
    return S_IMGRESZ_HAL_OK;
}

//////////////////////////////////////////////////////////////////////////////////
//
//  Hardware status related function
//
//////////////////////////////////////////////////////////////////////////////////
INT32 i4HwImgReszGetHwStatus(UINT32 u4HwId,IMGRESZ_HAL_HW_STATUS_T *prHwStatus)
{
    prHwStatus->u4SrcCntY = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_Y);
    prHwStatus->u4SrcCntCb = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_CB);
    prHwStatus->u4SrcCntCr = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_CR);
	prHwStatus->u4VOffsetY = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_Y);
	prHwStatus->u4VOffsetCb = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_CB);
	prHwStatus->u4VOffsetCr = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_CR);
    prHwStatus->u4VNextCY = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_Y);
    prHwStatus->u4VNextCCb = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CB);
    prHwStatus->u4VNextCCr = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CR);

    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszSetHwStatus(UINT32 u4HwId,IMGRESZ_HAL_HW_STATUS_T *prHwStatus)
{
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_Y,prHwStatus->u4SrcCntY);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_CB,prHwStatus->u4SrcCntCb);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_SRC_CNT_CR,prHwStatus->u4SrcCntCr);
	vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_Y,prHwStatus->u4VOffsetY);
	vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_CB,prHwStatus->u4VOffsetCb);
	vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_OFFSET_CR,prHwStatus->u4VOffsetCr);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_Y,prHwStatus->u4VNextCY);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CB,prHwStatus->u4VNextCCb);
    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_V_NEXT_C_CR,prHwStatus->u4VNextCCr);

    vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_HW_STATUS_CPU_ASSIGN,1); // Trigger write status

    return S_IMGRESZ_HAL_OK;
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
#define IMGRESZ_WAIT_COUNT  10000
void vHwImgReszWaitCountReset(UINT32 u4HwId)
{
    UINT32 u4RegVal = 0;

    u4RegVal = u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_FSM_MONITOR_REG);

    if ((u4RegVal & WORK_COUNTER) > 0 )
    {
        UINT32 i = 0;
            
        while((u4HwImgReszRead32(u4HwId,RO_IMG_RESZ_FSM_MONITOR_REG) & WORK_COUNTER) > 0)
        {
            i++;
            if (i > IMGRESZ_WAIT_COUNT)
            {
               // UTIL_Printf("[IMGRESZ] Last scale time out \n");
                break;
            }
        }
    }
}
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
INT32 i4HwImgReszGetHwCRC(UINT32 u4HwId)
{
    return u4HwImgReszRead32(u4HwId, RO_IMG_RESZ_CHECK_SUM_REG);
}
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561)

void vHwImgReszSetPreloadBuf_iommu(UINT32 u4HwId)
{
  UINT32 u4RegVal = 0;

    u4RegVal = u4HwImgReszRead32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE);
	u4RegVal |= ((unsigned)0x1 << 24);                                   ///for iomm id.preloade buffer
	u4RegVal &= (~((unsigned)0x1 << 25));
	vHwImgReszWrite32(u4HwId,RW_IMG_RESZ_MEM_IF_MODE,u4RegVal);
}

#endif
void i4HwImgReszSetDstPitchEnable(UINT32 u4HwId,BOOL fgPitchEnable,
                                 UINT32 u4PitchByte,UINT32 u4BufAddr_Byte)
{
   if(fgPitchEnable)
   {
       UINT32 u4RegVal = 0;

    	UTIL_Printf("[imgresz] === set pitch parameter=== \n");
    	UTIL_Printf("[imgresz] === u4PitchByte= 0x%x %d=== \n",u4PitchByte, u4BufAddr_Byte);

    	vHwImgReszWrite32(u4HwId,0x50,0);

    	u4RegVal |= u4BufAddr_Byte;

    	u4RegVal |= (u4PitchByte << 4);
    //	UTIL_Printf("[imgresz] ===u4RegVal= 0x%x === \n",u4RegVal);
    	u4RegVal |= ((unsigned)0x1 << 8);                                   ///for iommu id.preloade buffer
    	UTIL_Printf("[imgresz] ===u4RegVal= 0x%x === \n",u4RegVal);
    	vHwImgReszWrite32(u4HwId,0x50,u4RegVal);

   }

}


INT32 i4HwImgReszSetAlphaComDebugEnable(UINT32 u4HwId, BOOL fgDebug)
{
    UINT32 u4Value = 0;

    if(0 == u4HwId)
        return 0;

    if(fgDebug)
    {
        u4Value = u4HwImgReszRead32(u4HwId, 0x1E0);
        u4Value |= (0x7);
        vHwImgReszWrite32(u4HwId, 0x1E0, u4Value);
    }

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetAlphaCompositionEnable(UINT32 u4HwId, BOOL fgEnable)
{
    UINT32 u4Value = 0, u4Value1 = 0;
    UINT32 u4Value3 = 0x100000A0;//0x107800A0;
    UINT32 u4Value4 = 0;

    if(0 == u4HwId)
        return 0;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_TYPE);
    u4Value1 = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL);
    u4Value4 = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_START);
    u4Value3 = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION);
    if(fgEnable)
    {
        u4Value  |= IMG_RESZ_FIX4_ENABLE;
        u4Value1 |= IMG_RESZ_OSD_ALU_ENABLE;
        u4Value4 |= (0x1<<10);
        u4Value4 |= (0x1<<6);
       // UTIL_Printf("<imgresz>AlphaCom 0x4 0x%x 0x104 0x%x \n", u4Value, u4Value1);
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_TYPE, u4Value);

         vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_OSD_MD_CTRL, u4Value1);
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_START, u4Value4);

        u4Value3 |= (0x1<<28);
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION, u4Value3);

        u4Value3 = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION);
        UTIL_Printf("<imgresz>AlphaCom 0x200 0x%x \n", u4Value3);
    }
    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszSetAlphaCompositionRange(UINT32 u4HwId, UINT32 i4AlphaComRange)
{
    UINT32 u4Value = 0;

    if(0 == u4HwId)
        return 0;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION);

    if(i4AlphaComRange >= 0 && i4AlphaComRange <= 255)
    {
        u4Value  |= (i4AlphaComRange);
    }

    if(i4AlphaComRange == 255)
    {
         u4Value |= (0x1<<12);
    }
    else
    {
        u4Value &= ~(0x1<<12);
    }

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION, u4Value);

    return S_IMGRESZ_HAL_OK;
}
INT32 i4HwImgReszSetAlphaCompositionOpt(UINT32 u4HwId,
                                        IMGRESZ_AC_OP_MODE_T i4AlphaComOpt)
{
    UINT32 u4Value = 0, u4Value1 = 0;

    if(0 == u4HwId)
        return 0;

    if(i4AlphaComOpt >= E_AC_OP_CLEAR && i4AlphaComOpt <= E_AC_OP_ADD)
    {
        u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION);
        u4Value  |= (i4AlphaComOpt<<8);
        vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_ALPHA_COMPOSITION, u4Value);
    }

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszUFO_Enable(UINT32 u4HwId,BOOL fgEnable)
{
 /*   UINT32 u4Value = 0;

    if(1 == u4HwId) //0, 2 is ok
        return 0;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_UFO_CFG);

    if(fgEnable)
       u4Value  |= IMG_RESZ_UFO_UFO_EN;
    else
       u4Value  &= ~IMG_RESZ_UFO_UFO_EN;
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_UFO_CFG, u4Value);

    UTIL_Printf("<Imgresz>UFOEn 0x%x 0x%x \n",
          u4Value, u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_UFO_CFG));
*/
    return S_IMGRESZ_HAL_OK;
}


INT32 i4HwImgReszUFO_Config(UINT32 u4HwId,IMGRESZ_UFO_CFG_TYPE eCfgType)
{
    UINT32 u4Value = 0;

    if(1 == u4HwId) //0, 2 is ok
        return 0;

    u4Value = u4HwImgReszRead32(u4HwId, RW_IMG_RESZ_UFO_CFG);

    u4Value  |= IMG_RESZ_UFO_UFO_EN; //enable UFO

    if(eCfgType == IMGRESZ_UFO_CFG_TYPE_8BIT)
    {
        u4Value  &= ~(0x3<<13);
    }
    else if(eCfgType == IMGRESZ_UFO_CFG_TYPE_10BIT_COMPACT || eCfgType == IMGRESZ_UFO_CFG_TYPE_10BIT_COMPACT_UNCOMPRESS)
    {
        u4Value |= IMG_RESZ_UFO_UFO_CFG_10BIT_COMPACT;
    }
    else if(eCfgType == IMGRESZ_UFO_CFG_TYPE_10BIT_REORDER)
    {
        u4Value |= IMG_RESZ_UFO_UFO_CFG_10BIT_REORDER;
    }

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_UFO_CFG, u4Value);

    UTIL_Printf("<Imgresz>UFOCfg 0x%x 0x%8x \n",
          u4Value, u4HwImgReszRead32(u4HwId, 0x228));

    return S_IMGRESZ_HAL_OK;
}

INT32 i4HwImgReszUFO_SizeBuf_Addr(UINT32 u4HwId, UINT32 u4YBufSZSA, UINT32 u4CBufSZSA)
{
 //   UINT32 u4Value = 0;

    if(1 == u4HwId) //0, 2 is ok
        return 0;

#if !IMGRESZ_IO_MMU_TEST
     u4YBufSZSA = u4YBufSZSA ? PHYSICAL(u4YBufSZSA) : 0;
     u4CBufSZSA = u4CBufSZSA ? PHYSICAL(u4CBufSZSA) : 0;
#endif

    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_UFO_YBUF_LEN, u4YBufSZSA>>4);
    vHwImgReszWrite32(u4HwId, RW_IMG_RESZ_UFO_CBUF_LEN, u4CBufSZSA>>4);

    return S_IMGRESZ_HAL_OK;
}

EXPORT_SYMBOL(vImgreszPrintRegister);


