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
#include "x_bim.h"
#include "fbm_drvif.h"

#include "png_hal_dec.h"
#include "png_hal_priv.h"
#include "png_hal_reg.h"
#include "x_ckgen.h"
#include "x_timer.h"
#include "png_drv.h"
#include "png_debug.h"

#include "jpg_if.h"
#define PngWriteREG(base, arg, val)  IO_WRITE32(IMAGE_BASE, ((base + arg) & 0xFFFFF), val)

/*
#define PngWriteREG(base, arg, val)   \
{vPngLog_L(" Write val: 0x%08x to Reg : 0x%08x\n",val, ((base + arg) & 0xFFFFF));\
IO_WRITE32(IMAGE_BASE, ((base + arg) & 0xFFFFF), val);\
}\
*/
#define PngReadREG(base, arg)         IO_READ32(IMAGE_BASE, ((base + arg) & 0xFFFFF))

#define PngWriteWTREG(arg, val)   IO_WRITE32(IMAGE2_BASE, (arg & 0xFFF), val)
#define PngReadWTREG(arg)         IO_READ32(IMAGE2_BASE, (arg & 0xFFF))
#define PNGIOMMU_READ32(base, arg)         IO_READ32(IO_MMU_BASE, (base + arg))
#define PNGIOMMU_WRITE32(base, arg, val)         IO_WRITE32(IO_MMU_BASE, (base + arg), val)
#define PNG_SUPPORT_POWEROFF 0
#define MIN_MEMORY_ALIGNMENT        32
#define MEMORY_ALIGN_PRUDENCE       0x100000

extern UINT32 u4hPNGSemaphoreDBGCnt;
extern UINT32 u4hPNGSemaphoreDBG[256][2];
extern UINT32 u4hPngSema;
extern UINT32 u4hPng_Hal_Sema;
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
extern BOOL _bPngMapAddr;
#endif
extern void u4hPNGSemaphoreDBGInsert(UINT32 type, UINT32 value);

//#define LINEBUFFER_DEBUG 
void* x_alloc_aligned_nc_mem(UINT32 u4Size, UINT32 u4Align)
{
    void   *pMem;
    UINT32 *p, u4Mem;
#if MEMORY_ALIGN_PRUDENCE
    void   *pMem1;
    UINT32  u4Mem1;
#endif


    if (u4Align < MIN_MEMORY_ALIGNMENT)
    {
        u4Align = MIN_MEMORY_ALIGNMENT;
    }
    u4Align &= ~(MIN_MEMORY_ALIGNMENT - 1);

    pMem = x_mem_alloc(u4Size + u4Align + 2 * sizeof(UINT32));
    if (NULL == pMem)
    {
        return pMem;
    }

#if MEMORY_ALIGN_PRUDENCE
    if (MEMORY_ALIGN_PRUDENCE <= u4Align)
    {
        pMem1 = x_mem_alloc(u4Size + u4Align + 2 * sizeof(UINT32));
        if (NULL != pMem1)
        {
            u4Mem  = ((UINT32) pMem  + u4Align + 2 * sizeof(UINT32)) & ~(u4Align - 1);
            u4Mem1 = ((UINT32) pMem1 + u4Align + 2 * sizeof(UINT32)) & ~(u4Align - 1);
            if ((u4Mem - (UINT32) pMem) > (u4Mem1 - (UINT32) pMem1))
            {
                x_mem_free(pMem);
                pMem = pMem1;
            }
            else
            {
                x_mem_free(pMem1);
            }
        }
    }
#endif

    p = (UINT32 *) (((UINT32) pMem + u4Align + 2 * sizeof(UINT32)) & ~(u4Align - 1));
    *(p - 1) = (UINT32) p - (UINT32) pMem;
    *(p - 2) = (UINT32) pMem;

    u4Mem = (UINT32) p;
    u4Mem = NONCACHE(u4Mem);

    HalFlushInvalidateDCache();

    return (void *) u4Mem;
}

void x_free_nc_mem(void *pUser)
{
    UINT32  *p, u4Mem;
    void    *pMem;

    if (NULL == pUser)
    {
        return;
    }

    p = (UINT32 *) pUser;
    pMem = (void *) *(p - 2);

    u4Mem = (UINT32) pMem;
    u4Mem = NONCACHE(u4Mem) + (*(p - 1));

    if (u4Mem != (UINT32) pUser)
    {
        ASSERT(0);
    }

    x_mem_free(pMem);
}

void x_free_aligned_nc_mem(void *pUser)
{
    x_free_nc_mem(pUser);
}
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
BOOL _fgPngHalInitIrq = FALSE;
PNG_HAL_INST_T  _tPngHalHwInst[PNG_HAL_HW_INST_NUM];
static HANDLE_T     _h_Png_Hal_Sema = NULL_HANDLE;
static HANDLE_T     _h_Png_CkgenClk_Sema = NULL_HANDLE;
UINT32 _u4RefCount = 0;
#if (PNG_MMU_TEST)
void vPrintfMMUStatus(void)
{
    UINT32 u4RegVal = 0;
    //vIIOMMU_Init();
    // IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_GIF, 0);
    //u4RegVal = 0x1E|(0x1<<0);
    //IOMMU_WRITE32(REG_RW_IOMMU_CFG0, IOMMU_PNG, u4RegVal);
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG0, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
    //IOMMU_WRITE32(REG_RW_IOMMU_CFG1, IOMMU_PNG, u4HalGetTTB1());
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
    //IOMMU_WRITE32(REG_RW_IOMMU_CFG2, IOMMU_PNG, 0x1B0019);
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG2, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
}
void vPngMMUInit(void)
{
    UINT32 u4RegVal = 0;
    //vIIOMMU_Init();
    // IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_GIF, 0);
    u4RegVal = 0xFE|(0x1<<0);
    PNGIOMMU_WRITE32(REG_RW_IOMMU_CFG0, PNG_MMU_BASE, u4RegVal);
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG0, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU REG_RW_IOMMU_CFG0 config value is %x . \n",u4RegVal);
    PNGIOMMU_WRITE32(REG_RW_IOMMU_CFG1, PNG_MMU_BASE, u4HalGetTTB1());
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU REG_RW_IOMMU_CFG1 config value is %x . \n",u4RegVal);
    PNGIOMMU_WRITE32(REG_RW_IOMMU_CFG2, PNG_MMU_BASE, 0x1B0019);//0x1B0019
    //IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_PNG, 0xA);
    u4RegVal = PNGIOMMU_READ32 (REG_RW_IOMMU_CFG3, PNG_MMU_BASE);
    vPngLog_L("[PNG_VFY] MMU  REG_RW_IOMMU_CFG3 config value is %x . \n",u4RegVal);
}
void vFlushPNGMMU(void)
{
#if PNG_MMU_TEST

    PNGIOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE,(PNGIOMMU_READ32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE) | 0x80000000));

    while ((PNGIOMMU_READ32(REG_RW_IOMMU_CFG8, PNG_MMU_BASE) & 0x20000000) != 0);

#endif
}
#endif
//DEFINE_CHANNEL2_MEMORY_AREA(_u4PngLz77Buf, BYTE, PNG_LZ77_SIZE, 128)

//extern void vInflateHalGetIRQ(UINT16 u2Vector);

extern void vPngSetClockOn(void)
{
#if (!CONFIG_DRV_FPGA_BOARD)
#if 0
    CKGEN_AgtOnClk(e_CLK_PNG);
    CKGEN_AgtSelClk(e_CLK_PNG, 0x5);
#endif
#endif //#if (!CONFIG_DRV_FPGA_BOARD)
}

extern void vPngSetClockOff(void)
{
#if (!CONFIG_DRV_FPGA_BOARD)
#if 0
    CKGEN_AgtOffClk(e_CLK_PNG);
#endif
#endif //#if (!CONFIG_DRV_FPGA_BOARD)
}



UINT32 u4PngGetSTC(void)
{
    /*UINT32 u4Stc;
    HAL_TIME_T tCurrentTime;

    HAL_GetTime(&tCurrentTime);
    u4Stc = tCurrentTime.u4Seconds;
    return u4Stc;*/
    return 0;
}

void vPngHalGetIrQ(UINT16 u2Vector)
{
    switch (u2Vector)
    {
    case VECTOR_PNG1:
        VERIFY(BIM_ClearIrq(u2Vector));
        VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_DEC_IRQ, X_EV_OP_OR) == OSR_OK);
        break;
    case VECTOR_PNG2:
        VERIFY(BIM_ClearIrq(u2Vector));
        VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_SRC_IRQ, X_EV_OP_OR) == OSR_OK);
        break;
    case VECTOR_PNG3:
        VERIFY(BIM_ClearIrq(u2Vector));
        VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_DST_IRQ, X_EV_OP_OR) == OSR_OK);
        break;
    }
}

BOOL fgPngHalSetIrQ(void)
{
    x_os_isr_fct pfnOldIsr;

    if (_fgPngHalInitIrq)
    {
        return TRUE;
    }
    else
    {
        _fgPngHalInitIrq = TRUE;
    }

    if (x_reg_isr(VECTOR_PNG1, vPngHalGetIrQ, &pfnOldIsr) != OSR_OK)    //dec
    {
        ASSERT(0);
        return FALSE;
    }
    if (x_reg_isr(VECTOR_PNG2, vPngHalGetIrQ, &pfnOldIsr) != OSR_OK) //src
    {
        ASSERT(0);
        return FALSE;
    }
    if (x_reg_isr(VECTOR_PNG3, vPngHalGetIrQ, &pfnOldIsr) != OSR_OK)   //dst
    {
        ASSERT(0);
        return FALSE;
    }
    return TRUE;
}

UINT32 u4PngHalReadCrc(UINT32 u4HwInstId)
{
    return PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_CRC_OUT_REG);
}

BOOL fgPngHalDisableIrQ(void)
{
    //BIM_DisableIrq(VECTOR_PNG);
    //BIM_DisableIrq(VECTOR_PNG1);
    //BIM_DisableIrq(VECTOR_PNG2);
    //return TRUE;
    x_os_isr_fct pfnOldIsr;

    if (_fgPngHalInitIrq)
    {
        _fgPngHalInitIrq = FALSE;
    }
    else
    {
        return TRUE;
    }

    if (x_reg_isr(VECTOR_PNG1, NULL, &pfnOldIsr) != OSR_OK)    //dec
    {
        ASSERT(0);
        return FALSE;
    }
    if (x_reg_isr(VECTOR_PNG2, NULL, &pfnOldIsr) != OSR_OK) //src
    {
        ASSERT(0);
        return FALSE;
    }
    if (x_reg_isr(VECTOR_PNG3, NULL, &pfnOldIsr) != OSR_OK)   //dst
    {
        ASSERT(0);
        return FALSE;
    }
    return TRUE;
}

INT32 i4PngHalWaitDec(UINT32 u4HwInstId, UINT32 u4WaitTime)
{
    EV_GRP_EVENT_T      ePngEvent;
    EV_GRP_EVENT_T      ePngEventGet;
    INT32               i4Ret;
    UINT32              u4PngMonitorReg;
    UINT32              u4PngRegVal;
    UINT32              u4WaitDataToDTimes;

    ePngEvent = PNG_HAL_EV_DEC_IRQ | PNG_HAL_EV_SRC_IRQ | PNG_HAL_EV_DST_IRQ | PNG_HAL_EV_STOP_IRQ;
    i4Ret = x_ev_group_wait_event_timeout(_tPngHalHwInst[u4HwInstId].hPngEvent, ePngEvent, &ePngEventGet, X_EV_OP_OR_CONSUME, u4WaitTime);
    u4PngMonitorReg = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_MONITOR_REG);    
    /*vPngLog_L("[Png]  decoding checksum 0x30= 0x%08x,  0x34= 0x%08x, 0x38= 0x%08x,  0x3c= 0x%08x\n",
                      PngReadREG(PNG_REG_BASE2, 0x30),
                      PngReadREG(PNG_REG_BASE2, 0x34),
                      PngReadREG(PNG_REG_BASE2, 0x38),
                      PngReadREG(PNG_REG_BASE2, 0x3c));    */
    vPngLog_L("[Png]i4PngHalWaitDec---i4Ret = %d, png_monitor=0x%x\n",i4Ret, u4PngMonitorReg);
    if (i4Ret == OSR_OK)
    {
        u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
        u4PngRegVal &= PNG_REG_IRQ0_BIT_MASK;
        PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_DECIRQ_CLEAN_EN);
        PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
        vPngLog_L("[Png]i4PngHalWaitDec---ePngEventGet = %d\n",ePngEventGet);

        if (ePngEventGet & PNG_HAL_EV_STOP_IRQ)
        {
            return PNG_HAL_RTN_STOPPED;
        }
        if (ePngEventGet & PNG_HAL_EV_DEC_IRQ)
        {            
            if ((u4PngMonitorReg & PNG_REG_MONITOR_ALDER_ERR) == PNG_REG_MONITOR_ALDER_ERR)
            {
                return PNG_HAL_ALDER_ERR;
            }
            if ((u4PngMonitorReg & PNG_REG_MONITOR_DEC_ERR) == PNG_REG_MONITOR_DEC_ERR)
            {
                return PNG_HAL_RTN_FAIL;
            }
            u4WaitDataToDTimes = 0;
            #ifndef LINEBUFFER_DEBUG
            while ( (PngReadREG(PNG_REG_BASE2, 0x40) != PngReadREG(PNG_REG_BASE2, 0x50)) && (u4WaitDataToDTimes < 20))
            {
                x_thread_delay(2);
                vPngLog_H("[Png] Wait data to dram value match!, (6E140=0x%08x) != (6E150=0x%08x), Current STC %d\n",  PngReadREG(PNG_REG_BASE2, 0x40),
                          PngReadREG(PNG_REG_BASE2, 0x50), u4PngGetSTC());
                u4WaitDataToDTimes ++;
            }
            if ( PngReadREG(PNG_REG_BASE2, 0x40) != PngReadREG(PNG_REG_BASE2, 0x50))
            {
                vPngLog_H("[Png]  Decoding error, (6E140=0x%08x) != (6E150=0x%08x), Current STC %d\n", PngReadREG(PNG_REG_BASE2, 0x40),
                          PngReadREG(PNG_REG_BASE2, 0x50), u4PngGetSTC());
                return PNG_HAL_RTN_FAIL;
            }
            #else
            UNUSED(u4WaitDataToDTimes);
            #endif
            return PNG_HAL_RTN_DEC;
        }
        if (ePngEventGet & PNG_HAL_EV_SRC_IRQ)
        {
            u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
            u4PngRegVal &= PNG_REG_IRQ1_BIT_MASK;
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_SRCIRQ_CLEAN_EN);
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
            //vPngLog_M("[PNG--]PNG_HAL_RTN_SRC\n");
            if (ePngEventGet & PNG_HAL_EV_DST_IRQ)
            {
                VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_DST_IRQ, X_EV_OP_OR) == OSR_OK);
                vPngLog_H("[PNG--]To-Keep--PNG_HAL_EV_DST_IRQ\n");
            }
            return PNG_HAL_RTN_SRC;
        }
        if (ePngEventGet & PNG_HAL_EV_DST_IRQ)
        {
            u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
            u4PngRegVal &= PNG_REG_IRQ2_BIT_MASK;
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_DSTIRQ_CLEAN_EN);
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
            //vPngLog_M("[PNG--]PNG_HAL_RTN_DST\n");
            if (ePngEventGet & PNG_HAL_EV_SRC_IRQ)
            {
                VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_SRC_IRQ, X_EV_OP_OR) == OSR_OK);
                vPngLog_H("[PNG--]To-Keep--PNG_HAL_EV_SRC_IRQ\n");
            }
            return PNG_HAL_RTN_DST;
        }
        else
        {
            PNGASSERT(0);
            vPngLog_H("[Png] Undefined event. Current STC %d\n", u4PngGetSTC());
            return PNG_HAL_RTN_FAIL;
        }
    }
    else
    {
        UINT32 u4Tempval = 0;
        vPngLog_H("[Png]  Decoding time out. Current STC %d\n", u4PngGetSTC());
        for (u4Tempval = 0; u4Tempval < 32; u4Tempval++)
        {
            //x_thread_delay(5);
            vPngLog_H("[PngW]  decoding Reg 0x%08x = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
                   (PNG_REG_BASE1 + (u4Tempval*16 + 0)),
                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 0)),
                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 4)),
                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 8)),
                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 12)));
        }
        return PNG_HAL_RTN_STOPPED;
    }
}


INT32 i4PngHalGetHwInst(UINT32 *pu4HwInstId)
{
    INT32 i4 = 0;
    INT32 i4Ret = PNG_HAL_RTN_OK;

#if (PNG_SUPPORT_POWEROFF)
    vPngSetClockOn();
#endif
        u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 0xff);   //start code for debug
		u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 1);
    x_sema_lock(_h_Png_Hal_Sema, X_SEMA_OPTION_WAIT);
		u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 2);
    for (i4 = 0; i4 < PNG_HAL_HW_INST_NUM; i4++)
    {
        if (_tPngHalHwInst[i4].fgActive == FALSE)
        {
            _tPngHalHwInst[i4].fgActive = TRUE;
            *pu4HwInstId = (UINT32)i4;
            break;
        }
    }
    if (i4 == PNG_HAL_HW_INST_NUM)
    {
        i4Ret = PNG_HAL_RTN_FAIL;
    }
    u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 3);
    x_sema_unlock(_h_Png_Hal_Sema);
    u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 4);
    return i4Ret;
}

INT32 i4PngHalRelHwInst(UINT32 u4HwInstId)
{
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
#if (PNG_SUPPORT_POWEROFF)
    vPngSetClockOff();
#endif
		u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 5);
    x_sema_lock(_h_Png_Hal_Sema, X_SEMA_OPTION_WAIT);
    u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 6);
    _tPngHalHwInst[u4HwInstId].fgActive = FALSE;
    x_sema_unlock(_h_Png_Hal_Sema);
    u4hPNGSemaphoreDBGInsert(u4hPng_Hal_Sema, 7);
    return PNG_HAL_RTN_OK;
}

BOOL fgPngHalIsBusy(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return FALSE;
    }

    if(0x00 != (0x3F&PngReadREG(u4RegBase, 0xB4))|| /*ZLIB state*/       \
       0x00 != (0x0F&PngReadREG(u4RegBase, 0xF4))|| /*Parser state*/       \
       0x10 != (0x1F&PngReadREG(u4RegBase, 0xC4)) )   /*Defilter  state*/
    {
        return TRUE;
    }
    return FALSE;
}

INT32 i4PngHalSetStop(UINT32 u4HwInstId)
{
UINT32 u4PngRegVal = 0;
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
	u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
			PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_DECIRQ_CLEAN_EN);
			PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);

   	vPngSWReset(5);
    VERIFY(x_ev_group_set_event(_tPngHalHwInst[u4HwInstId].hPngEvent, PNG_HAL_EV_STOP_IRQ, X_EV_OP_OR) == OSR_OK);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalReset(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    INT32               i4Ret;

    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
   while ( ((PngReadREG(u4RegBase, 0xC8) & 0xF) != 0x8) || ((PngReadREG(u4RegBase, 0xCC) & 0x70) != 0x10))
    {
        x_thread_delay(1);
        vPngLog_H("[Png] Wait reset value match!, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
    }
	    PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_START);
		x_thread_delay(2);
		PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);

#if (PNG_MMU_TEST)
		PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, 0x0f00);
		x_thread_delay(1);
		PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
#endif
		PngWriteREG(u4RegBase, PNG_DEC_SRAM_CHIP_SELECT_REG, PNG_REG_SRAM_ON);
        #if 1//(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
				PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, PNG_REG_DRAM_REQ_4); //set 4e30 as 0 for 8555 from Jia Chen's suggestion.
        #else
		    PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, PNG_REG_DRAM_REQ_4);
        #endif

    PngWriteREG(PNG_REG_BASE2, 0x04, 0x1);
    PngWriteREG(PNG_REG_BASE2, 0x00, 0x0);

    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, 0);

    //clean event
    i4Ret = x_ev_group_set_event(_tPngHalHwInst[u4HwInstId].hPngEvent, 0x00, X_EV_OP_CLEAR);
    VERIFY(i4Ret == OSR_OK);
    return PNG_HAL_RTN_OK;
}

#if (defined(PNG_VFY_AUTO_TEST) || defined(PNG_DRV_FLOW_TEST) )
extern INT32 _PngVerifyCmdGetParam(INT32 i4ParamType);
#endif

INT32 i4PngHalCkGen(BOOL fgEnable, UINT32 u4HwInstId)
{
/*
#if defined(CC_MT5398)||defined(CC_MT5880) || defined(CC_MT5399)
#define BLOCK_RST_CFG0  0x1C0
#define PNG_RST  ((UINT32)1 << 20)
#define BLOCK_CKEN_CFG0 0x1C8
#define PNG_CKEN ((UINT32)1 << 20)
#else
#define BLOCK_RST_CFG0 0x270
#define PNG_RST ((UINT32)1 << 2)
#define BLOCK_CKEN_CFG0 0x278
#define PNG_CKEN ((UINT32)1 << 2)
#endif
    if (fgEnable)
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~PNG_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) | PNG_CKEN);
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) | PNG_RST);
    }
    else
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~PNG_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) & ~PNG_CKEN);
    }
*/
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    if(fgEnable)
    {        
        PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, PNG_REG_ENABLE);
        PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_START);
	x_thread_delay(2);
	PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);        
    }
    else
    {
        PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
	x_thread_delay(2);
	PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_START);
        PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, 0);
    }
    UNUSED(fgEnable);
    return PNG_HAL_RTN_OK;
}

#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define  PNG_CLK_OFFSET 0x3D0
#else
#define  PNG_CLK_OFFSET 0x740
#endif
INT32 i4PngHalEnable(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
	
	UINT32 u4SysClock=PNG_CLK_USBPLL_D2;
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
#if (defined(PNG_VFY_AUTO_TEST) || defined(PNG_DRV_FLOW_TEST) )
     u4SysClock = _PngVerifyCmdGetParam(3);
#endif
    vPngLog_L("Png Clock = %d\n",u4SysClock);
#if (!PNG_SUPPORT_POWEROFF)
#if (!CONFIG_DRV_FPGA_BOARD)
    IO_WRITE32(CKGEN_BASE, PNG_CLK_OFFSET, u4SysClock);
#endif 
#endif

    PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, PNG_REG_ENABLE);
#if PNG_MMU_TEST
    PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG,
                PngReadREG(u4RegBase,PNG_DEC_ENABLE_REG) | PNG_MMU_ENABLE);
#endif
	

    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalDisable(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, 0);
    PngWriteREG(u4RegBase, PNG_DEC_SRAM_CHIP_SELECT_REG, 0);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSetAutoSkipDisable(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    //auto skip-- disable auto-skip
    PngWriteREG(u4RegBase, 0xFC, 0x0);
    PngWriteREG(PNG_REG_BASE2, 0x04, 0x0);
    return PNG_HAL_RTN_OK;
}
INT32 i4PngHalSetAutoSkipEnable(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    //auto skip-- enable auto-skip
    PngWriteREG(u4RegBase, 0xFC, 0x1);
    //clear status
    PngWriteREG(PNG_REG_BASE2, 0x08, 0x1);
    PngWriteREG(PNG_REG_BASE2, 0x08, 0x0);
    return PNG_HAL_RTN_OK;
}
UINT32 u4PngHalGetAutoSkipCrc(UINT32 u4HwInstId)
{
    return PngReadREG(PNG_REG_BASE2, 0x08);
}
INT32 i4PngHalSetScale(UINT32 u4HwInstId, UINT32 u4PngInterlaceScale, UINT32 u4PngHorScale, UINT32 u4PngVrtScale)
{

    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    PngWriteREG(u4RegBase, PNG_DEC_RESIZE_RATIO_REG, (u4PngInterlaceScale << 6) + (u4PngHorScale << 3) + u4PngVrtScale);
    return PNG_HAL_RTN_OK;
}
INT32 i4PngHalSetAutoSkipLastIdat(UINT32 u4HwInstId, BOOL fgLastIdat)
{
    if (fgLastIdat)
    {
        PngWriteREG(PNG_REG_BASE2, 0x00, 0x1);
    }
    else
    {
        PngWriteREG(PNG_REG_BASE2, 0x00, 0x0);
    }
    return PNG_HAL_RTN_OK;
}
void vPngHalFireDec(UINT32 u4RegBase, BOOL fgSrcResume, BOOL fgDstResume, BOOL fgStartDec)
{
    // PngWriteREG(u4RegBase,PNG_DEC_PELOUT_WDLT_EN_REG,PNG_DEC_PELOUT_WDLT_EN_1);
    if (fgSrcResume)
    {
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME0);
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME1);
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME0);
    }
    if (fgDstResume)
    {
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME0);
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME1);
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME0);
    }
    if (fgStartDec)
    {
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START0);
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START1);
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START0);
    }
}


INT32 i4PngHalSetPicInfo(UINT32 u4HwInstId, PNG_HAL_PIC_INFO_T *ptPngPicInfo)
{
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    UINT32 u4RegBase = ptPngHwInst->u4HwRegBase;
    UINT32 u4RegTmpValue;
    UINT32 u4LineBufferSize;

    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    ptPngHwInst->tIhdr.u4Width = ptPngPicInfo->ptIhdr->u4Width;
    ptPngHwInst->tIhdr.u4Height = ptPngPicInfo->ptIhdr->u4Height;
    ptPngHwInst->tIhdr.u1BitDepth = ptPngPicInfo->ptIhdr->u1BitDepth;
    ptPngHwInst->tIhdr.u1ColorType = ptPngPicInfo->ptIhdr->u1ColorType;
    ptPngHwInst->tIhdr.fgInterlace = ptPngPicInfo->ptIhdr->fgInterlace;
    ptPngHwInst->eOutClrMd = ptPngPicInfo->eOutClrMd;

    switch (ptPngHwInst->tIhdr.u1ColorType)
    {
    case 0://colour type 0, greyscale image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth + 7 ) / 8 ;
        break;
    case 2://colour type 2, truecolourr image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width * ptPngHwInst->tIhdr.u1BitDepth * 3 + 7 ) / 8 ;
        break;
    case 3://colour type 3, indexed- colour image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width * ptPngHwInst->tIhdr.u1BitDepth + 7) / 8 ;
        break;
    case 4://colour type 4, greyscale with alpha image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth  * 2 + 7) / 8 ;
        break;
    case 6://colour type 6, truecolourr with alpha image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth  * 4 + 7) / 8 ;
        break;
    default:
        PNGASSERT(0);
        vPngLog_H("[Png]  not support input color type. Current STC %d\n",  u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }

    u4LineBufferSize = ((u4LineBufferSize + 31) / 32) * 32;//must be ow
    if (u4LineBufferSize > PNG_LINE_BUF_SIZE)
    {
        return PNG_HAL_RTN_FAIL;
    }
    if (ptPngHwInst->u4LineBufAddr == 0)
    {
        ASSERT(0);
        return PNG_HAL_RTN_FAIL;
    }
    HalFlushInvalidateDCacheMultipleLine(ptPngHwInst->u4LineBufAddr, u4LineBufferSize);
    HalFlushInvalidateDCacheMultipleLine(ptPngHwInst->u4LZ77BufAddr, PNG_LZ77_SIZE);
    HalFlushInvalidateDCacheMultipleLine(ptPngPicInfo->u4FifoStart , PNG_FIFO_SIZE);
    
    u4RegTmpValue = (ptPngHwInst->u4LZ77BufAddr) ? PHYSICAL(ptPngHwInst->u4LZ77BufAddr) : 0;
#if(!PNG_MMU_TEST)
    ptPngPicInfo->u4FifoStart = PHYSICAL(ptPngPicInfo->u4FifoStart);
    ptPngPicInfo->u4FifoEnd = PHYSICAL(ptPngPicInfo->u4FifoEnd);
#endif
    PngWriteREG(u4RegBase, PNG_DEC_LZ77_STR_OWADDR_REG, (u4RegTmpValue >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_LZ77_END_OWADDR_REG, ((u4RegTmpValue + PNG_LZ77_SIZE) >> 4));
#if (!PNG_MMU_TEST)
    u4RegTmpValue = ptPngPicInfo->u4FifoStart & PNG_REG_OW_ADDR_MASK;//0x3FFFFFFF;
#else
    u4RegTmpValue = ptPngPicInfo->u4FifoStart;
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_FIFO_SRT_OWADDR_REG, (u4RegTmpValue >> 4));
#if (!PNG_MMU_TEST)
    u4RegTmpValue = ptPngPicInfo->u4FifoEnd & PNG_REG_OW_ADDR_MASK;//0x3FFFFFFF;
#else
    u4RegTmpValue = ptPngPicInfo->u4FifoEnd;
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_FIFO_END_OWADDR_REG, ((u4RegTmpValue >> 4) - 1));    

    u4RegTmpValue = PHYSICAL(ptPngHwInst->u4LineBufAddr);
    PngWriteREG(u4RegBase, PNG_DEC_LINE_BUF_STR_OWADDR_REG, (u4RegTmpValue >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_LINE_BUF_END_OWADDR_REG, ((u4RegTmpValue + u4LineBufferSize) >> 4));

    u4RegTmpValue = 0;
    u4RegTmpValue += (ptPngHwInst->tIhdr.u1BitDepth << 4);
    u4RegTmpValue += ((ptPngHwInst->tIhdr.u1ColorType & PNG_REG_CLR_TYPE_MASK) << 1);
    if (ptPngHwInst->tIhdr.fgInterlace)
    {
        u4RegTmpValue += 0x1;
    }
    PngWriteREG(u4RegBase, PNG_DEC_SRC_FORMAT_REG, u4RegTmpValue);
    PngWriteREG(u4RegBase, PNG_DEC_SRC_WIDTH_HEIGHT_REG, ((ptPngPicInfo->ptIhdr->u4Width - 1) << PNG_REG_SRC_W_SHIFT) + (ptPngPicInfo->ptIhdr->u4Height - 1));

    PngWriteREG(u4RegBase, PNG_DEC_ALPHA_UNMATCHED_PIXEL_REG, (ptPngPicInfo->u4AlphaUnMatchedVal & PNG_REG_ALPHA_VAL_MASK));

    switch (ptPngPicInfo->eOutClrMd)
    {
    case GFX_COLORMODE_RGB_D565:
        u4RegTmpValue = ptPngPicInfo->u4DstBufPitch / 2;
        PngWriteREG(u4RegBase, PNG_DEC_OUTPUT_FORMAT_REG, PNG_REG_OUT_FORMAT_RGB565);
        break;
    case GFX_COLORMODE_ARGB_D1555:
        u4RegTmpValue = ptPngPicInfo->u4DstBufPitch / 2;
        PngWriteREG(u4RegBase, PNG_DEC_OUTPUT_FORMAT_REG, PNG_REG_OUT_FORMAT_ARGB1555);
        break;
    case GFX_COLORMODE_ARGB_D4444:
        u4RegTmpValue = ptPngPicInfo->u4DstBufPitch / 2;
        PngWriteREG(u4RegBase, PNG_DEC_OUTPUT_FORMAT_REG, PNG_REG_OUT_FORMAT_ARGB4444);
        break;
    case GFX_COLORMODE_ARGB_D8888:
        u4RegTmpValue = ptPngPicInfo->u4DstBufPitch / 4;
        PngWriteREG(u4RegBase, PNG_DEC_OUTPUT_FORMAT_REG, PNG_REG_OUT_FORMAT_ARGB8888);
        break;
    case GFX_COLORMODE_ARGB_CLUT8:
        u4RegTmpValue = ptPngPicInfo->u4DstBufPitch / 1;
        PngWriteREG(u4RegBase, PNG_DEC_OUTPUT_FORMAT_REG, PNG_REG_OUT_FORMAT_PALETTE);
        break;
    default:
        return PNG_HAL_RTN_FAIL;
    }
    u4RegTmpValue = u4RegTmpValue & PNG_REG_DISP_W_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG, ((u4RegTmpValue - 1) << PNG_REG_DISP_W_SHIFT) + (ptPngPicInfo->u4SliceHeight - 1));

    //crop, default the function is disbable
    PngWriteREG(u4RegBase, PNG_DEC_CROP_ORG_XY_REG, ((ptPngPicInfo->u4CropX) << 16) + (ptPngPicInfo->u4CropY));
    PngWriteREG(u4RegBase, PNG_DEC_CROP_WIDTH_HEIGHT_REG, ((ptPngPicInfo->u4CropWidth - 1) << 16) + (ptPngPicInfo->u4CropHeight - 1));

    //init transparency control register
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_CTRL_REG, 0);
    PngWriteREG(u4RegBase, PNG_DEC_RESIZE_RATIO_REG, PNG_REG_INTERLACE_7);

    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSetPLTE(UINT32 u4HwInstId, PNG_PLTE *ptPngPlte)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    INT32 i4Ret;

    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }

    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG, ptPngPlte->u4ChunkType);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);
    PngWriteREG(u4RegBase, PNG_DEC_READ_PLTT_CHUNK_REG, PNG_REG_READ_PLTE);
    PngWriteREG(u4RegBase, PNG_DEC_PLTT_ENTRY_NUM_REG, (ptPngPlte->u4ChunkSize/3)-1);
#if(PNG_MMU_TEST)
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngPlte->u4PlteData)?(UINT32)(ptPngPlte->u4PlteData):0) );
#else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngPlte->u4PlteData)?PHYSICAL((UINT32)(ptPngPlte->u4PlteData)):0) );
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, ptPngPlte->u4ChunkSize);

    HalFlushInvalidateDCacheMultipleLine(ptPngPlte->u4PlteData, PNG_PLTE_SIZE);
    //start read plte
    vPngHalFireDec(u4RegBase, TRUE, FALSE, TRUE);
    i4Ret = i4PngHalWaitDec(u4HwInstId, PNG_HAL_TIMEOUT);
    PngWriteREG(u4RegBase, PNG_DEC_READ_PLTT_CHUNK_REG, 0);
    if (i4Ret == PNG_HAL_RTN_DEC)
    {
        return PNG_HAL_RTN_OK;
    }
    else
    {
        return i4Ret;
    }
}


INT32 i4PngHalSettRNSEx1(UINT32 u4HwInstId, PNG_TRNS *ptPngTrns)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    INT32 i4Ret;


    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG, ptPngTrns->u4ChunkType);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);
    PngWriteREG(u4RegBase, PNG_DEC_READ_TRNS_CHUNK_REG, PNG_REG_READ_PLTE);
    PngWriteREG(u4RegBase, PNG_DEC_TRNS_ENTRY_NUM_REG, ptPngTrns->u4ChunkSize - 1);
#if (PNG_MMU_TEST)
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngTrns->u4TrnsData)?(UINT32)(ptPngTrns->u4TrnsData):0));
#else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngTrns->u4TrnsData)?PHYSICAL((UINT32)(ptPngTrns->u4TrnsData)):0));
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, ptPngTrns->u4ChunkSize);
    HalFlushInvalidateDCacheMultipleLine(ptPngTrns->u4TrnsData, PNG_TRNS_SIZE);

    //start read tRNS
    vPngHalFireDec(u4RegBase, TRUE, FALSE, TRUE);
    i4Ret = i4PngHalWaitDec(u4HwInstId, PNG_HAL_TIMEOUT);
    PngWriteREG(u4RegBase, PNG_DEC_READ_TRNS_CHUNK_REG, 0);
    if (i4Ret == PNG_HAL_RTN_DEC)
    {
        return PNG_HAL_RTN_OK;
    }
    else
    {
        return i4Ret;
    }
}

INT32 i4PngtRNSColorTx(UINT16 u2Src, UINT16 *pu2Dst, UINT8 u1SrcFormat, GFX_COLORMODE_T eOutClrMd)
{
    switch (eOutClrMd)
    {
    case GFX_COLORMODE_RGB_D565:
    case GFX_COLORMODE_ARGB_D1555:
    case GFX_COLORMODE_ARGB_D4444:
    case GFX_COLORMODE_ARGB_D8888:
        switch (u1SrcFormat)
        {
        case 1:
            *pu2Dst = (u2Src & 0x1);
            *pu2Dst = (*pu2Dst) + (*pu2Dst << 1) + (*pu2Dst << 2) +
                      (*pu2Dst << 3) + (*pu2Dst << 4) + (*pu2Dst << 5) +
                      (*pu2Dst << 6) + (*pu2Dst << 7);
            break;
        case 2:
            *pu2Dst = (u2Src & 0x3);
            *pu2Dst = (*pu2Dst) + (*pu2Dst << 2) + (*pu2Dst << 4) +
                      (*pu2Dst << 6);
            break;
        case 4:
            *pu2Dst = (u2Src & 0xF);
            *pu2Dst = (*pu2Dst) + (*pu2Dst << 4);
            break;
        case 8:
            *pu2Dst = (u2Src & 0xFF);
            break;
        case 16:
            *pu2Dst = ((u2Src & 0xFF00) >> 8);
            if ((u2Src & 0x80) && (*pu2Dst != 0xFF))
            {
                *pu2Dst += 1;
            }
            break;
        }
        break;
    default:
        PNGASSERT(0);
        vPngLog_H("[Png]  not support output color mode. Current STC %d\n",  u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSettRNSEx2(UINT32 u4HwInstId, PNG_TRNS *ptPngTrns)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    UINT16 u2R, u2G, u2B;
    UINT16 u2RVal, u2GVal, u2BVal;
    UINT32 u4RegTmpValue = 0;
    UINT8* puTransData = (UINT8*)(ptPngTrns->u4TrnsData);

    u2R = puTransData[0] << 8;
    u2R += puTransData[1];
    u2G = puTransData[2] << 8;
    u2G += puTransData[3];
    u2B = puTransData[4] << 8;
    u2B += puTransData[5];

    HalFlushInvalidateDCacheMultipleLine(ptPngTrns->u4TrnsData, PNG_TRNS_SIZE);
    switch (ptPngHwInst->tIhdr.u1ColorType)
    {
    case 0: // grayscale image
    case 4: // grayscale image
        i4PngtRNSColorTx(u2R, &u2RVal, ptPngHwInst->tIhdr.u1BitDepth, ptPngHwInst->eOutClrMd);
        u2GVal = u2RVal;
        u2BVal = u2RVal;
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY0_REG, (u2R << PNG_REG_TRNS_GREY_SHIFT));
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY1_REG, (0));
        break;
    case 2: // rgb triple
    case 6: // rgb triple
        i4PngtRNSColorTx(u2R, &u2RVal, ptPngHwInst->tIhdr.u1BitDepth, ptPngHwInst->eOutClrMd);
        i4PngtRNSColorTx(u2G, &u2GVal, ptPngHwInst->tIhdr.u1BitDepth, ptPngHwInst->eOutClrMd);
        i4PngtRNSColorTx(u2B, &u2BVal, ptPngHwInst->tIhdr.u1BitDepth, ptPngHwInst->eOutClrMd);
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY0_REG, (u2R << PNG_REG_TRNS_R_SHIFT));
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY1_REG, ((u2G << PNG_REG_TRNS_G_SHIFT) + (u2B << PNG_REG_TRNS_B_SHIFT)));
        break;
    default:
        PNGASSERT(0);
        vPngLog_H("[Png] Bitstream color type error. Current STC %d\n",  u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    u4RegTmpValue = PNG_REG_TRNS_EN;

    if (ptPngHwInst->tIhdr.u1BitDepth == 16)
    {
        u4RegTmpValue += PNG_REG_TRNS_MATCH_16BIT;
    }
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_CTRL_REG, u4RegTmpValue);
    u4RegTmpValue = u2RVal << PNG_REG_TRNS_BG_R_SHIFT;
    u4RegTmpValue += u2GVal << PNG_REG_TRNS_BG_G_SHIFT;
    u4RegTmpValue += u2BVal << PNG_REG_TRNS_BG_B_SHIFT;
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4RegTmpValue);
    PngWriteREG(u4RegBase, PNG_DEC_ALPHA_MATCHED_PIXEL_REG, 0);

    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSettRNS(UINT32 u4HwInstId, PNG_TRNS *ptPngTrns, BOOL fgPlte)
{
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    if (fgPlte)
    {
        return i4PngHalSettRNSEx1(u4HwInstId, ptPngTrns);
    }
    else
    {
        return i4PngHalSettRNSEx2(u4HwInstId, ptPngTrns);
    }
}

INT32 i4PngHalSetbKGD(UINT32 u4HwInstId, PNG_BKGD *ptPngBkgd, PNG_PLTE *ptPngPlte, PNG_TRNS *ptPngTrns, BOOL fgPlte, BOOL fgTrns)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    UINT32 u2A = 0x0;
    UINT32 u4RegTmpValue = 0;
    UINT8* puPlteData = (UINT8*)(ptPngPlte->u4PlteData);

    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    if (fgPlte)
    {
        UINT32 u4PlteBgIdx = ptPngBkgd->u2R;
        if (u4PlteBgIdx > 255)
        {
            return PNG_HAL_RTN_FAIL;
        }
        if (fgTrns)
        {
            u2A = puPlteData[u4PlteBgIdx];
        }
        ptPngBkgd->u2R = puPlteData[u4PlteBgIdx*3+0];
        ptPngBkgd->u2G = puPlteData[u4PlteBgIdx*3+1];
        ptPngBkgd->u2B = puPlteData[u4PlteBgIdx*3+2];
    }
    else
    {
        switch (ptPngHwInst->tIhdr.u1ColorType)
        {
        case 0: // grayscale image
        case 4: // grayscale image
            ptPngBkgd->u2G = ptPngBkgd->u2R;
            ptPngBkgd->u2B = ptPngBkgd->u2R;
            break;
        default:
            PNGASSERT(0);
            vPngLog_H("[Png] i4PngHalSetbKGD . color type error. Current STC %d\n", u4PngGetSTC());
            return PNG_HAL_RTN_FAIL;
        }
    }
    PngWriteREG(u4RegBase, PNG_DEC_ALPHA_MATCHED_PIXEL_REG, (u2A & 0xFF));
    u4RegTmpValue = (ptPngBkgd->u2R & 0xFF) << PNG_REG_TRNS_BG_R_SHIFT;
    u4RegTmpValue += (ptPngBkgd->u2G & 0xFF) << PNG_REG_TRNS_BG_G_SHIFT;
    u4RegTmpValue += (ptPngBkgd->u2B & 0xFF) << PNG_REG_TRNS_BG_B_SHIFT;
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4RegTmpValue);
    return PNG_HAL_RTN_OK;
}

#ifdef PNG_VFY_AUTO_TEST
extern UINT8 u1AlphaMatch,u1AlphaUnMatch;
extern UINT16 u2RKey,u2GKey,u2BKey,u2GrayKey;
extern UINT8 u1BGR,u1BGG,u1BGB;
extern BOOL fgTransEn,fgTransOutEn,fgTrans16BitEn,fgTransOrgAlphaEn;
#endif

INT32 i4PngHalProcIDAT(UINT32 u4HwInstId, PNG_HAL_IDAT_INFO_T *ptPngIdatInfo)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    UINT32 u4RegVal;
    //PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }

#if (!PNG_MMU_TEST)
    //ptPngIdatInfo->u4DstBufStart = PHYSICAL(ptPngIdatInfo->u4DstBufStart);
    //ptPngIdatInfo->u4DstBufEnd = PHYSICAL(ptPngIdatInfo->u4DstBufEnd);
    ptPngIdatInfo->u4SrtStrmAddr = PHYSICAL(ptPngIdatInfo->u4SrtStrmAddr);
#endif
#if (!PNG_MMU_TEST)
    u4RegVal = ptPngIdatInfo->u4DstBufStart & PNG_REG_B_ADDR_MASK;
#else
    u4RegVal = ptPngIdatInfo->u4DstBufStart;
#endif
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_XY_ADDR_REG, u4RegVal);
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_SRT_OWADDR_REG, (u4RegVal >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_END_OWADDR_REG, (ptPngIdatInfo->u4DstBufEnd >> 4));
    
    if (ptPngIdatInfo->fgLastIDATGrp)
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_LAST_IDAT_GRP);
    }
    else
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_NOTLAST_IDAT_GRP);
    }
    if (ptPngIdatInfo->fgLastSlice)
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_SLICE_REG, PNG_REG_LAST_SLICE_REG);
    }
    else
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_SLICE_REG, PNG_REG_NOTLAST_SLICE_REG);
    }
#if (PNG_MMU_TEST)
    u4RegVal = ptPngIdatInfo->u4SrtStrmAddr;
#else
    u4RegVal = ptPngIdatInfo->u4SrtStrmAddr & PNG_REG_B_ADDR_MASK;
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, u4RegVal);
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);

    u4RegVal = ptPngIdatInfo->u4SrtStrmLen & PNG_REG_B_ADDR_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, u4RegVal);

    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG, ptPngIdatInfo->u4ChunkType);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);
#ifdef PNG_VFY_AUTO_TEST
    if (fgTransEn)
    {
        vPngLog_L("Set Transparant Alpha param...\n");
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_CTRL_REG,
                    PNG_REG_TRNS_EN + fgTransOutEn * PNG_REG_TRNS_OUT + fgTransOrgAlphaEn * PNG_REG_TRNS_ORG_ALPHA + fgTrans16BitEn);
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY0_REG, ((u2GrayKey) <<PNG_REG_TRNS_GREY_SHIFT) + u2RKey);
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY1_REG, ((u2GKey) <<PNG_REG_TRNS_G_SHIFT) + u2BKey);
        PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG,
                    ((u1BGR) << PNG_REG_TRNS_BG_R_SHIFT) + ((u1BGG) << PNG_REG_TRNS_BG_G_SHIFT) + u1BGB);
        PngWriteREG(u4RegBase, PNG_DEC_ALPHA_MATCHED_PIXEL_REG, u1AlphaMatch);
        PngWriteREG(u4RegBase, PNG_DEC_ALPHA_UNMATCHED_PIXEL_REG, u1AlphaUnMatch);
    }
#endif

#ifdef LINEBUFFER_DEBUG
PngWriteREG(PNG_REG_BASE2, 0x34,  PngReadREG(PNG_REG_BASE2,0x34)|0x06000000);
PngWriteREG(PNG_REG_BASE2, 0x40,  PngReadREG(PNG_REG_BASE2,0x40)|0x00000006);
PngWriteREG(PNG_REG_BASE2, 0x50,  PngReadREG(PNG_REG_BASE2,0x50)|0x00000004);
vPngLog_H("PngReadREG(PNG_REG_BASE2,0x50) = 0x%08x\n",PngReadREG(PNG_REG_BASE2,0x50));
#endif 

    if (ptPngIdatInfo->fg1stIDATChunk)
    {
        vPngHalFireDec(u4RegBase, TRUE, FALSE, TRUE);
    }
    else
    {
        vPngHalFireDec(u4RegBase, TRUE, FALSE, FALSE);
    }

    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalProcNextSrc(UINT32 u4HwInstId, UINT32 u4SrcStrmSrtAddr, UINT32 u4SrcStrmLen, BOOL fgLastIDATGroup)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    //UINT32 u4PngMonitorReg = PngReadREG(u4RegBase, PNG_DEC_MONITOR_REG);
    UINT32 u4PngRegVal;

    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
#if 0
    u4PngRegVal &= PNG_REG_IRQ1_BIT_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_SRCIRQ_CLEAN_EN);
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
#endif
    u4PngRegVal = PngReadREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG);
    if (u4PngRegVal != 0x49444154)
    {
        return PNG_HAL_RTN_FAIL;
    }

#if (!PNG_MMU_TEST)
    u4SrcStrmSrtAddr = PHYSICAL(u4SrcStrmSrtAddr);
#endif

    if (fgLastIDATGroup)
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_LAST_IDAT_GRP);
    }
    else
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_NOTLAST_IDAT_GRP);
    }

    u4PngRegVal = u4SrcStrmLen & PNG_REG_B_ADDR_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, u4PngRegVal);
#if (PNG_MMU_TEST)
    u4PngRegVal = u4SrcStrmSrtAddr;
#else
    u4PngRegVal = u4SrcStrmSrtAddr & PNG_REG_B_ADDR_MASK;
#endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, u4PngRegVal);

    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);

    vPngHalFireDec(u4RegBase, TRUE, FALSE, FALSE);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSetSliceWH(UINT32 u4HwInstId, UINT32 u4SliceHeight)

{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    UINT32 u4RegTmpValue = PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG);

    vPngLog_L("[Png] befor setting  i4PngHalSetSliceWH= 0x%08x\n",
              PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG));
    u4RegTmpValue  = (u4RegTmpValue & 0xFFFF0000) >> 16;
    PngWriteREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG, ((u4RegTmpValue) << PNG_REG_DISP_W_SHIFT) + ((u4SliceHeight&0x0000ffff) - 1));
    vPngLog_L("[Png]  i4PngHalSetSliceWH= 0x%08x\n",
              PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG));
    return PNG_HAL_RTN_OK;
}
INT32 i4PngHalProcNextDst(UINT32 u4HwInstId, UINT32 u4DstBufStart, UINT32 u4DstBufEnd, BOOL fgLastSlice)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    //PNG_HAL_INST_T	*ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }

#if (!PNG_MMU_TEST)
    //u4DstBufStart = PHYSICAL(u4DstBufStart);
    //u4DstBufEnd = PHYSICAL(u4DstBufEnd);
#endif

    if (fgLastSlice)
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_SLICE_REG, PNG_REG_LAST_SLICE_REG);
    }
    else
    {
        PngWriteREG(u4RegBase, PNG_DEC_LAST_SLICE_REG, PNG_REG_NOTLAST_SLICE_REG);
    }
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_XY_ADDR_REG, u4DstBufStart);
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_SRT_OWADDR_REG, (u4DstBufStart >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_END_OWADDR_REG, (u4DstBufEnd >> 4));
    
    vPngHalFireDec(u4RegBase, FALSE, TRUE, FALSE);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalInitHwInst(void)
{
    INT32 i4 = 0;
    INT32 i4Ret;
    CHAR  szEvName[16] = "PngHalEv0";

#if CONFIG_DRV_FAST_LOGO
    BIM_ClearIrq(VECTOR_PNG1);
    BIM_ClearIrq(VECTOR_PNG2);
    BIM_ClearIrq(VECTOR_PNG3);
#endif

    if (!fgPngHalSetIrQ())
    {
        return PNG_HAL_RTN_FAIL;
    }
    
    i4Ret = x_sema_create(&_h_Png_Hal_Sema, X_SEMA_TYPE_MUTEX, 1);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_sema_create(&_h_Png_CkgenClk_Sema, X_SEMA_TYPE_MUTEX, 1);
    VERIFY(i4Ret == OSR_OK);

    for (i4 = 0; i4 < PNG_HAL_HW_INST_NUM; i4++)
    {
        _tPngHalHwInst[i4].u4HwInstId = (UINT32)i4;
        _tPngHalHwInst[i4].fgActive = FALSE;
        //CJYang, move to CH2

        szEvName[8] = (CHAR)(i4 + 0x30);
        i4Ret = x_ev_group_create(&_tPngHalHwInst[i4].hPngEvent, szEvName, PNG_HAL_EV_INITIAL);
        VERIFY(i4Ret == OSR_OK);
    }
    _tPngHalHwInst[0].u4HwRegBase = PNG_REG_BASE1;
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalUnInitHwInst(void)
{
    INT32 i4 = 0;
    INT32 i4Ret;
    if (!fgPngHalDisableIrQ())
    {
        return PNG_HAL_RTN_FAIL;
    }

    i4Ret = x_sema_delete(_h_Png_CkgenClk_Sema);
    VERIFY(i4Ret == OSR_OK);
    i4Ret = x_sema_delete(_h_Png_Hal_Sema);
    VERIFY(i4Ret == OSR_OK);

    for (i4 = 0; i4 < PNG_HAL_HW_INST_NUM; i4++)
    {
#ifdef PNG_VFY_AUTO_TEST
        x_free_aligned_nc_mem((void*)_tPngHalHwInst[i4].u4LZ77BufAddr);
#endif
        if (PNG_HAL_INVALID32 != _tPngHalHwInst[i4].u4LineBufAddr)
        {
#ifdef PNG_VFY_AUTO_TEST
            x_free_aligned_nc_mem((void*)_tPngHalHwInst[i4].u4LineBufAddr);
#endif
            _tPngHalHwInst[i4].u4LineBufAddr = PNG_HAL_INVALID32;
        }

        i4Ret = x_ev_group_delete(_tPngHalHwInst[i4].hPngEvent);
        VERIFY(i4Ret == OSR_OK);
    }
    return PNG_HAL_RTN_OK;
}

void vPngSWReset(UINT32 delay)
{
    PngWriteREG(PNG_REG_BASE1, PNG_DEC_RESET_REG, PNG_REG_RESET_START);
    if(delay)
    {
        x_thread_delay(delay);
    }    
    PngWriteREG(PNG_REG_BASE1, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
    return;
}


