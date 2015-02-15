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

/** @file irt_dma_hw.c
 *  irt_dma hardware interfaces
 *  purely hardware related functions, include crash detection routines,
 */

#include "x_common.h"
#include "x_typedef.h"
#include "x_timer.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_debug.h"
//#include <mach/cache_operation.h>
//#include "irqs.h" // irq in liunx
#include "x_bim.h" //irq in nucleus
#include "irt_dma_hw.h"

#include "x_hal_arm.h"

#if IRT_HW_SPEED_TEST
UINT32      _u4IRTHwSpeed_Cnt = 0;
UINT64      _u8IRTHwSpeed_Time = 0;
HAL_TIME_T  _IRTHwSpeed_Start = {0};
HAL_TIME_T  _IRTHwSpeed_End = {0};
HAL_TIME_T  _IRTHwSpeed_Calc = {0};
#endif

HANDLE_T g_hWaitIrtDmaDone = (HANDLE_T)NULL;

typedef struct _RLE_HAL_INST_T
{
    UINT32      u4HwInstId;
    HANDLE_T    hEvent;
}RLE_HAL_INST_T;


#if ((CONFIG_DRV_VERIFY_SUPPORT == 1) || (CONFIG_DRV_FPGA_BOARD == 1))
void IRT_WriteREG(UINT32 arg, UINT32 val, const CHAR *szFunction, INT32 i4Line)
{
    *(volatile UINT32 *)arg = val;

    if (0 && szFunction)
    {
      //  UTIL_Printf("[irt] 0x%08X=0x%08X @%s, %d\n", arg, val, szFunction, i4Line);
    }
    else
    {
     //   UTIL_Printf("[irt] 0x%08X=0x%08X\n", arg, val);
    }
}

UINT32 IRT_ReadREG(UINT32 arg, const CHAR *szFunction, INT32 i4Line)
{
    volatile UINT32 u4Value;

    u4Value = *(volatile UINT32 *)arg;

    if (0 && szFunction)
    {
      //  UTIL_Printf("[irt] 0x%08X=0x%08X [R] @%s, %d\n", arg, u4Value, szFunction, i4Line);
    }
    else
    {
      //  UTIL_Printf("[irt] 0x%08X=0x%08X [R]\n", arg, u4Value);
    }

    return u4Value;
}

void IRT_WriteREGMsk(UINT32 arg, UINT32 val, UINT32 msk, const CHAR *szFunction, INT32 i4Line)
{
    IRT_WriteREG((arg), (IRT_ReadREG(arg, szFunction, i4Line) & (~(msk))) | ((val) & (msk)), szFunction, i4Line);
}
#endif

//-------------------------------------------------------------------------
/*IRT_DMA_HwISR
*interrupt program,BIM clear vector
*/
//-------------------------------------------------------------------------
static void IRT_DMA_HwISR(UINT16 u2Vector)
{
    UNUSED(u2Vector);

    IRT_APIEntryEx("%u", u2Vector);

    BIM_ClearIrq(VECTOR_IRT_DMA);

#if IRT_HW_SPEED_TEST
    HAL_GetTime(&_IRTHwSpeed_End);
    HAL_GetDeltaTime(&_IRTHwSpeed_Calc, &_IRTHwSpeed_Start, &_IRTHwSpeed_End);

    _u8IRTHwSpeed_Time += _IRTHwSpeed_Calc.u4Seconds * 1000000
						+ _IRTHwSpeed_Calc.u4Micros;
#endif
    VERIFY(x_sema_unlock(g_hWaitIrtDmaDone) == OSR_OK);
}

//-------------------------------------------------------------------------
/*IRT_DMA_HwReset
*trig hw function
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwReset(void)
{
    IRT_APIEntry();

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_CTRL,IRT_DMA_FLD_RG_DRAM_CLK_EN(1),IRT_DMA_MSK_RG_DRAM_CLK_EN);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_CTRL,IRT_DMA_FLD_RG_RESET_B(0),IRT_DMA_MSK_RG_RESET_B);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_CTRL,IRT_DMA_FLD_RG_RESET_B(1),IRT_DMA_MSK_RG_RESET_B);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_CTRL,IRT_DMA_FLD_RG_DRAM_CLK_EN(0),IRT_DMA_MSK_RG_DRAM_CLK_EN);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_CTRL, IRT_DMA_FLD_RG_MON_EN(1),
    			IRT_DMA_MSK_RG_MON_EN);



    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

INT32 IRT_DMA_CLK(void)
{/*
   UINT32 clktmp = 0;
   UINT32 u4SysClock = 3 ;//200
   clktmp= IO_READ32(CKGEN_BASE, 0x1d0);
   UTIL_Printf("[IRT] Vfy 0x1d0 clock[0x%x]\n",clktmp);
   clktmp |= (0x1<<31);
   IO_WRITE32(CKGEN_BASE, 0x1d0, clktmp);
   UTIL_Printf("[IRT] Vfy 0x1d0 clock[0x%x]\n",clktmp);

   clktmp= IO_READ32(CKGEN_BASE, 0x1d4);
   UTIL_Printf("[IRT] Vfy 0x1d4 clock[0x%x]\n",clktmp);
   clktmp |= (0x1<<31);
   IO_WRITE32(CKGEN_BASE, 0x1d4, clktmp);
   UTIL_Printf("[IRT] Vfy 0x1d4 clock[0x%x]\n",clktmp);
   */
}
//-------------------------------------------------------------------------
/** IRT_DMA_HwInit
*  init IRT_DMA hardware
*  set control register address to default
*  set default isr
*  do hw reset
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwInit(void)
{
    x_os_isr_fct pfnOldIsr;

    IRT_APIEntry();

    IRT_DMA_HwReset();

    VERIFY(x_reg_isr((UINT16)(VECTOR_IRT_DMA),
		IRT_DMA_HwISR, &pfnOldIsr) == OSR_OK);
    VERIFY (x_sema_create(&g_hWaitIrtDmaDone,
		X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);

	IRT_DMA_CLK();

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwUnInit
*  uninit IRT_DMA hardware
*  set control register address to default
*  set default isr to NULL
*  do hw reset
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwUnInit(void)
{
    x_os_isr_fct pfnOldIsr;

    IRT_APIEntry();

    VERIFY(x_reg_isr((UINT16)VECTOR_IRT_DMA, NULL, &pfnOldIsr) == OSR_OK);
    VERIFY (x_sema_delete(g_hWaitIrtDmaDone) == OSR_OK);
    g_hWaitIrtDmaDone = (HANDLE_T)NULL;

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwModeSet
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwSrcModeSet(UINT32 u4Rg5351ModeEn, UINT32 u4Rg5351ModeSel, UINT32 u4RgScanLine)
{
    IRT_APIEntryEx("%u, %u, %u", u4Rg5351ModeEn, u4Rg5351ModeSel, u4RgScanLine);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG, IRT_DMA_FLD_RG_5351_MODE_EN(u4Rg5351ModeEn), IRT_DMA_MSK_RG_5351_MODE_EN);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG, IRT_DMA_FLD_RG_5351_MODE_SEL(u4Rg5351ModeSel), IRT_DMA_MSK_RG_5351_MODE_SEL);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG, IRT_DMA_FLD_RG_SCAN_LINE(u4RgScanLine), IRT_DMA_MSK_RG_SCAN_LINE);

	IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG,
			IRT_DMA_FLD_RG_BURSTEN(!u4RgScanLine),
			IRT_DMA_MSK_RG_BURST_EN);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

void vIRTDMAPrintRegister()
{
    UINT32 u4Temp = 0;

    for (u4Temp = 0; u4Temp < 12; u4Temp ++)
    {
        if ((u4Temp % 4) == 0)
        {
            UTIL_Printf("[0x%8x]",IRT_DMA_BASE +u4Temp * 4);
        }
        UTIL_Printf("0x%8x   ",IRT_DMA_READ32(u4Temp * 4));
        if ((u4Temp % 4) == 3)
        {
            UTIL_Printf("\n");
        }
    }
}


//-------------------------------------------------------------------------
/** IRT_DMA_HwTrig
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwTrig(void)
{
    IRT_APIEntry();

	vIRTDMAPrintRegister();

#if IRT_HW_SPEED_TEST
    _u4IRTHwSpeed_Cnt ++;
    HAL_GetTime(&_IRTHwSpeed_Start);
#endif

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG,IRT_DMA_FLD_RG_START(1),IRT_DMA_MSK_RG_START);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG,IRT_DMA_FLD_RG_START(0),IRT_DMA_MSK_RG_START);

	/*
	while(!(IRT_DMA_READ32(IRT_DMA_REG_WR_OFFSET) & (0x1<<9)))
	{
		UTIL_Printf("0x1c 0x%8x	0x20 0x%08x\n",
			IRT_DMA_READ32(IRT_DMA_REG_WR_OFFSET),
			IRT_DMA_READ32(IRT_DMA_REG_MON));
	}*/

    VERIFY(x_sema_lock(g_hWaitIrtDmaDone, X_SEMA_OPTION_WAIT) == OSR_OK);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwHVSize
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwHVSize(UINT32 u4Width, UINT32 u4Height)
{
    IRT_APIEntryEx("%u, %u", u4Width, u4Height);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_HVSIZE,IRT_DMA_FLD_RG_VSIZE(u4Height),IRT_DMA_MSK_RG_VSIZE);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_HVSIZE,IRT_DMA_FLD_RG_HSIZE(u4Width),IRT_DMA_MSK_RG_HSIZE);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwYCAndRotMode
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwYCAndRotMode(UINT32 u4Mode)
{
    IRT_APIEntryEx("%u", u4Mode);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG,IRT_DMA_FLD_RG_DMA_MODE(u4Mode),IRT_DMA_MSK_RG_DMA_MODE);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwSrcSa
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwSrcSa(UINT32 *pu4SrcYAdd,UINT32 *pu4SrcCAdd)
{
	UINT32 i4PaSrcY, i4PaSrcC;
    IRT_APIEntryEx("%p, %p", pu4SrcYAdd, pu4SrcCAdd);
	i4PaSrcY = PHYSICAL(pu4SrcYAdd);
	i4PaSrcC = PHYSICAL(pu4SrcCAdd);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_DRAM_Y_RD_STA,IRT_DMA_FLD_DRAM_Y_RD_STA(((UINT32)PHYSICAL(pu4SrcYAdd))>>4),IRT_DMA_MSK_DRAM_Y_RD_STA);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_DRAM_C_RD_STA,IRT_DMA_FLD_DRAM_C_RD_STA(((UINT32)PHYSICAL(pu4SrcCAdd))>>4),IRT_DMA_MSK_DRAM_C_RD_STA);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwDstSa
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwDstSa(UINT32 *pu4DstYAdd,UINT32 *pu4DstCAdd)
{
    IRT_APIEntryEx("%u, %u", pu4DstYAdd, pu4DstCAdd);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_DRAM_Y_WR_STA,IRT_DMA_FLD_DRAM_Y_WR_STA(((UINT32)PHYSICAL(pu4DstYAdd))>>4),IRT_DMA_MSK_DRAM_Y_WR_STA);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_DRAM_C_WR_STA,IRT_DMA_FLD_DRAM_C_WR_STA(((UINT32)PHYSICAL(pu4DstCAdd))>>4),IRT_DMA_MSK_DRAM_C_WR_STA);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

//-------------------------------------------------------------------------
/** IRT_DMA_HwWROffset
*/
//-------------------------------------------------------------------------
INT32 IRT_DMA_HwWROffset(UINT32 u4Width, UINT32 u4Height, IRT_DMA_MODE_T eMode)
{
    IRT_APIEntryEx("%u, %u, %u", u4Width, u4Height, eMode);

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_HVSIZE,IRT_DMA_FLD_RG_VSIZE(u4Height),IRT_DMA_MSK_RG_VSIZE);
    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_HVSIZE,IRT_DMA_FLD_RG_HSIZE(u4Width),IRT_DMA_MSK_RG_HSIZE);

    switch(eMode)
    {
    case IRT_DMA_MODE_ROTATE_0:
    case IRT_DMA_MODE_ROTATE_0_MIRROR:
    case IRT_DMA_MODE_ROTATE_180:
    case IRT_DMA_MODE_ROTATE_180_MIRROR:
        IRT_DMA_MSK_WRITE32(IRT_DMA_REG_WR_OFFSET,IRT_DMA_FLD_RG_WR_OFFSET(u4Width/16)|IRT_DMA_FLD_RG_RD_OFFSET(u4Width/16),
            IRT_DMA_MSK_RG_WR_OFFSET|IRT_DMA_MSK_RG_RD_OFFSET);
        break;

    case IRT_DMA_MODE_ROTATE_90:
    case IRT_DMA_MODE_ROTATE_90_MIRROR:
    case IRT_DMA_MODE_ROTATE_270:
    case IRT_DMA_MODE_ROTATE_270_MIRROR:
        IRT_DMA_MSK_WRITE32(IRT_DMA_REG_WR_OFFSET,IRT_DMA_FLD_RG_WR_OFFSET(u4Height/16)|IRT_DMA_FLD_RG_RD_OFFSET(u4Width/16),
            IRT_DMA_MSK_RG_WR_OFFSET|IRT_DMA_MSK_RG_RD_OFFSET);
        break;
        //add align mode for cb cr 8byte
    case IRT_DMA_MODE_CBCR_ALIGN:

        IRT_DMA_MSK_WRITE32(IRT_DMA_REG_WR_OFFSET,IRT_DMA_FLD_RG_WR_OFFSET(u4Width/16),IRT_DMA_MSK_RG_WR_OFFSET);

    default:
        break;
    }

    IRT_DMA_MSK_WRITE32(IRT_DMA_REG_TIRG,IRT_DMA_FLD_RG_DMA_MODE(eMode),IRT_DMA_MSK_RG_DMA_MODE);

    IRT_APILeave();

    return (INT32)IRT_DMA_OK;
}

/*
 *BD : map
 *TV : halflush
 */
#define IRT_CACHE_MAP_EN  0

void IRT_FlushDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
#if IRT_CACHE_MAP_EN
    BSP_dma_map_vaddr(u4Start, u4Len, BIDIRECTIONAL);
    BSP_dma_unmap_vaddr(u4Start, u4Len, BIDIRECTIONAL);
#else
    //BSP_FlushDCacheRange(u4Start, u4Len);
    HalFlushInvalidateDCacheMultipleLine(u4Start, u4Len);
	HalInvalidateDCacheMultipleLine(u4Start, u4Len);
#endif
}

void IRT_CleanDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
#if IRT_CACHE_MAP_EN
    BSP_dma_map_vaddr(u4Start, u4Len, TO_DEVICE);
    BSP_dma_unmap_vaddr(u4Start, u4Len, TO_DEVICE);
#else
    //BSP_CleanDCacheRange(u4Start, u4Len);
    HalFlushDCacheMultipleLine(u4Start, u4Len);
#endif
}

void IRT_InvDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
#if IRT_CACHE_MAP_EN
    BSP_dma_map_vaddr(u4Start, u4Len, FROM_DEVICE);
    BSP_dma_unmap_vaddr(u4Start, u4Len, FROM_DEVICE);
#else
    //BSP_InvDCacheRange(u4Start, u4Len);
    HalInvalidateDCacheMultipleLine(u4Start, u4Len);
	HalInvalidateDCacheMultipleLine(u4Start, u4Len);
#endif
}


