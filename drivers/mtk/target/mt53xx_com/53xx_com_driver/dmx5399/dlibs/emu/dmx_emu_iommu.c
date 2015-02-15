/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: dmx_emu_ddi.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_emu_ddi.c
 *  Demux driver - DDI interface emulation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_emu.h"
#include "dmx_emu_if.h"

#include "dmx_mm.h"
#include "fvr.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_rand.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_printf.h"
#include "x_os.h"
#include "x_hal_arm.h"
#include "x_bim.h"

LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

;
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT8 u1Pidx;
    UINT8 u1CCounter;
    UINT16 u2PidNum;
    UINT32 u4BufAddr;
    UINT32 u4BufStartAddr;
    UINT32 u4BufSize;
    UINT32 u4PesSize;
    UINT32 u4GoldenAddr;
    UINT32 u4PesAddr;
    BOOL fgIommuEnable;
} PES_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define PES_LEN_MAX         63488//62k
#define PES_LEN_MIN         2048 //2k


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static UINT8 _u1DmxPidNum = 3;
static PES_T _arPes[DMX_NUM_PID_INDEX];



//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/* DDIIOMMU_Handler
 *
 * @param  u2Vector        The IRQ vector
 * @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID DDIIOMMU_Handler(UINT16 u2Vector)
{
    static BOOL _fgISR = FALSE;

    ASSERT(!_fgISR);
    UNUSED(_fgISR);
    _fgISR = TRUE;

    Printf(">DDI IOMMU has page fault!\n");

    ASSERT(u2Vector == VECTOR_MMU_DDI);
    if (!BIM_IsIrqPending(VECTOR_MMU_DDI))
    {
        Printf("Spurious DDI IOMMU global interrupt!\n");
    }

    // Check if it's DDI IOMMU interrupt
    while (BIM_IsIrqPending(VECTOR_MMU_DDI))
    {

        Printf(">>DDI IOMMU has page fault!\n");

         Printf("IOMMU REG: CFG6~CFG9: 0x%08x,0x%08x,0x%08x,0x%08x.\n",
                IOMMU_DMX_READ32(REG_IOMMU_CFG6),
                IOMMU_DMX_READ32(REG_IOMMU_CFG7),
                IOMMU_DMX_READ32(REG_IOMMU_CFG8),
                IOMMU_DMX_READ32(REG_IOMMU_CFG9));

        //Clear IOMMU interrupt
        IOMMU_DDI_WRITE32(REG_IOMMU_CFG4, IOMMU_DDI_READ32(REG_IOMMU_CFG4) | 0x06000005);
        IOMMU_DDI_WRITE32(REG_IOMMU_CFG4, IOMMU_DDI_READ32(REG_IOMMU_CFG4) & ~(0x0610000F));

        // Clear interrupt
        VERIFY(BIM_ClearIrq(VECTOR_MMU_DDI));
    }

     //debug
    ASSERT(0);

    _fgISR = FALSE;
}

//-----------------------------------------------------------------------------
/* DDIIOMMU_Handler
 *
 * @param  u2Vector        The IRQ vector
 * @retval VOID
 */
//-----------------------------------------------------------------------------
static VOID DMXIOMMU_Handler(UINT16 u2Vector)
{
    static BOOL _fgISR = FALSE;

    ASSERT(!_fgISR);
    UNUSED(_fgISR);
    _fgISR = TRUE;

    ASSERT(u2Vector == VECTOR_MMU_DMX);
    if (!BIM_IsIrqPending(VECTOR_MMU_DMX))
    {
        Printf("Spurious DMX IOMMU global interrupt!\n");
    }

    // Check if it's DDI IOMMU interrupt
    while (BIM_IsIrqPending(VECTOR_MMU_DMX))
    {

        Printf(">>DMX IOMMU has page fault!\n");
        Printf("IOMMU REG: CFG6~CFG9: 0x%08x,0x%08x,0x%08x,0x%08x.\n",
                IOMMU_DMX_READ32(REG_IOMMU_CFG6),
                IOMMU_DMX_READ32(REG_IOMMU_CFG7),
                IOMMU_DMX_READ32(REG_IOMMU_CFG8),
                IOMMU_DMX_READ32(REG_IOMMU_CFG9));

        //Clear IOMMU interrupt
        IOMMU_DMX_WRITE32(REG_IOMMU_CFG4, IOMMU_DMX_READ32(REG_IOMMU_CFG4) | 0x06000005);
        IOMMU_DMX_WRITE32(REG_IOMMU_CFG4, IOMMU_DMX_READ32(REG_IOMMU_CFG4) & ~(0x0610000F));

        // Clear interrupt
        VERIFY(BIM_ClearIrq(VECTOR_MMU_DMX));
    }

    //debug
    ASSERT(0);

    _fgISR = FALSE;
}

//-----------------------------------------------------------------------------
/* DDI_InitIOMMUISR
 */
//-----------------------------------------------------------------------------
static void DDI_InitIOMMUISR(void)
{
    static BOOL _fgInited = FALSE;
    x_os_isr_fct pfnOldIsr;

    if (!_fgInited)
    {
        if (x_reg_isr(VECTOR_MMU_DDI, DDIIOMMU_Handler, &pfnOldIsr) != OSR_OK)
        {
            Printf("Error: failed to register DDI IOMMU ISR!\n");
        }
        UNUSED(pfnOldIsr);
        _fgInited = TRUE;
    }
}

//-----------------------------------------------------------------------------
/* DMX_InitIOMMUISR
 */
//-----------------------------------------------------------------------------
static void DMX_InitIOMMUISR(void)
{
    static BOOL _fgInited = FALSE;
    x_os_isr_fct pfnOldIsr;

    if (!_fgInited)
    {
        if (x_reg_isr(VECTOR_MMU_DMX, DMXIOMMU_Handler, &pfnOldIsr) != OSR_OK)
        {
            Printf("Error: failed to register DDI IOMMU ISR!\n");
        }
        UNUSED(pfnOldIsr);
        _fgInited = TRUE;
    }
}


VOID _DDI_EnableIOMMU(UINT32 u4Src1Start, UINT32 u4Src1End)
{
    UINT32 u4MmuTblAddr;

    u4MmuTblAddr = HalGetMMUTableAddress();

    // setup IOMMU
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG4, 0x0010000A);
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG1, u4MmuTblAddr);
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG2, 0x00000011);

    #if 1
    IOMMU_DDI_WRITE32(REG_IOMMU_CFGC, u4Src1Start & 0xfffff000);
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG9, ((u4Src1End-1) & 0xfffff000) | 0x1);
    #else
    UNUSED(u4Src1Start);
    UNUSED(u4Src1End);
    #endif

    //fire IOMMU cache
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG4, IOMMU_DDI_READ32(REG_IOMMU_CFG4) | (1U << 31));
    while((IOMMU_DDI_READ32(REG_IOMMU_CFG8) & (1 << 29)) != 0);

}

VOID _DMX_EnableIOMMU(VOID)
{
    UINT32 u4MmuTblAddr;

    u4MmuTblAddr = HalGetMMUTableAddress();

    // setup IOMMU
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG4, 0x0603000A);
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG4, 0x0010000A);
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG0, 0xFE | (0x01 << 0));
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG1, u4MmuTblAddr);
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG2, 0x00000011);


    //IOMMU_DDI_WRITE32(REG_IOMMU_CFGC, u4Src1Start & 0xfffff000);
    //IOMMU_DDI_WRITE32(REG_IOMMU_CFG9, ((u4Src1End-1) & 0xfffff000) | 0x1);

    //fire IOMMU cache
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG4, IOMMU_DMX_READ32(REG_IOMMU_CFG4) | (1U << 31));
    while((IOMMU_DMX_READ32(REG_IOMMU_CFG8) & (1 << 29)) != 0);

}


VOID _DDI_DisableIOMMU(void)
{
    IOMMU_DDI_WRITE32(REG_IOMMU_CFG0, 0);
}

VOID _DMX_DisableIOMMU(void)
{
    IOMMU_DMX_WRITE32(REG_IOMMU_CFG0, 0);
}


static BOOL _DDI_IOMMU_Transfer(UINT32 u4BufStart, UINT32 u4BufEnd, UINT32 u4StartAddr, UINT32 u4DataSize)
{
    UINT32 u4BufSize, u4Rp, u4Wp;
    BOOL fgRet;

    ASSERT(u4BufStart != 0);
    ASSERT(u4BufEnd   != 0);
    ASSERT(u4StartAddr     != 0);
    ASSERT(_DMX_IsAligned(u4BufStart, DDI_BUF_ALIGNMENT));
    ASSERT(_DMX_IsAligned(u4BufEnd,   DDI_BUF_ALIGNMENT));

    u4BufSize = u4BufEnd - u4BufStart;
    ASSERT(u4BufSize >= u4DataSize);
    ASSERT((u4StartAddr >= u4BufStart) && (u4StartAddr < u4BufEnd));

    u4Rp = u4StartAddr;
    u4Wp = u4StartAddr + u4DataSize;
    if (u4Wp >= u4BufEnd) // DMA never stops if (WP == Buffer End).
    {
        u4Wp -= u4BufSize;
    }

    // Setup DMA
    DDI_WRITE32(DDI_REG_DMA_BUF_START, u4BufStart);     // Buffer start
    DDI_WRITE32(DDI_REG_DMA_BUF_END, u4BufEnd);         // Buffer end
    DDI_WRITE32(DDI_REG_DMA_RP_INIT, u4Rp);             // Initial RP
    DDI_WRITE32(DDI_REG_DMA_WP, u4Wp);                  // Write pointer

    DMXCMD_WRITE32(49, 0); //0xf00170c4 FTUP TS PROCESSING RATE REGISTER 1
    DMXCMD_WRITE32(51, 0); //0xf00170cc FTUP TS PROCESSING RATE REGISTER 2

    // Start DMA operation
    DDI_WRITE32(DDI_REG_DMA_CTRL, 5);

    // Waiting for completion by comparing RP with WP
    fgRet = FALSE;

    //HW limitation ,hw ddi wp/rp need 4 byte alignment,
    //u4Wp = _DMX_Align(u4Wp, DDI_POINTER_ALIGNMENT);

#if 0
    for (i = 0; i < MAX_WAIT_COUNT; i++)
    {
        UINT32 u4Rp;

        u4Rp = DDI_READ32(DDI_REG_DMA_RP);
        if (u4Rp == u4Wp)
        {
            // Complete
            fgRet = TRUE;
            Printf("   Wait count: %u\n", i);
            break;
        }

//        VERIFY(DMX_DRAMMeasure(DMX_LOCAL_ARBITOR_DDI, 500));
        _Delay(1);
    }
#else
    {
        HAL_TIME_T rTime1, rTime2, rTimeDiff;

        HAL_GetTime(&rTime1);
        while (1)
        {
            UINT32 u4Rp;

            u4Rp = DDI_READ32(DDI_REG_DMA_REAL_RP);
            if (u4Rp == u4Wp)
            {
                // Complete
                fgRet = TRUE;
                //Printf("   Wait count: %u\n", i);
                break;
            }
            //_Delay(1);
        }
        HAL_GetTime(&rTime2);
        HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
        LOG(0, "DDI time: %d.%06d\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
    }
#endif

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DDI_IOMMU1
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DDI_IOMMU1(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    UINT32 u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4GoldenFileSize, u4BufSize, u4SrcOffset,pbSrcBufAddr, pbDstBufAddr;
    UINT32 pbGoldenBufAddr, i;
    UINT32 u4Size = 0x8000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8100000, 0x9000000); //phy=129M, virt=144M
    HalSet1MSectionTable(0x8000000, 0x9100000); //phy=128M, virt=145M

    HalFlushInvalidateDCache();

    u4GoldenFileSize = u4Size - (u4Size % 188);
    u4BufSize = _DMX_Align(u4GoldenFileSize + 188, DMX_CA_BUFFER_ALIGNMENT); //pbSrcBufAddr + u4BufSize = u4EndAdd;
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = 0x90FF000;
    pbDstBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_CA_BUFFER_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    Printf("Transfer Size = %u .\n", u4GoldenFileSize);

    //generate random golden data.
    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    //copy golden data to src buffer
    if (!_DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize))
    {
        goto _exit;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set dst buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, pbDstBufAddr, pbDstBufAddr + u4BufSize,
                                u4BufSize))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

     HalFlushInvalidateDCache();

    //enable IOMMU
    _DDI_EnableIOMMU(pbSrcBufAddr, pbSrcBufAddr + u4BufSize);

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(pbSrcBufAddr, pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset, u4GoldenFileSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    // Check result
    if (!_MemCmp(pbGoldenBufAddr, pbDstBufAddr, u4GoldenFileSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

    //Disable DDI IOmmu
    _DDI_DisableIOMMU();

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DDI_IOMMU2
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DDI_IOMMU2(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    UINT32 u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4GoldenFileSize, u4BufSize, u4SrcOffset,pbSrcBufAddr, pbDstBufAddr;
    UINT32 pbGoldenBufAddr, i;
    UINT32 u4Size = 0x8000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);

    for(i = 0; i < 0x100000; i+=0x2000)
    {
        HalSet4KPageTable(0x8501000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
        HalSet4KPageTable(0x8500000 + i, 0x9601000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
    }

    HalFlushInvalidateDCache();

    u4GoldenFileSize = u4Size - (u4Size % 188);
    u4BufSize = _DMX_Align(u4GoldenFileSize + 188, DMX_CA_BUFFER_ALIGNMENT); //pbSrcBufAddr + u4BufSize = u4EndAdd;
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = 0x9600000;
    pbDstBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_CA_BUFFER_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    Printf("Transfer Size = %u .\n", u4GoldenFileSize);

    //generate random golden data.
    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    //copy golden data to src buffer
    if (!_DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize))
    {
        goto _exit;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set dst buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, pbDstBufAddr, pbDstBufAddr + u4BufSize,
                                u4BufSize))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

     HalFlushInvalidateDCache();

    //enable IOMMU
    _DDI_EnableIOMMU(pbSrcBufAddr, pbSrcBufAddr + u4BufSize);

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(pbSrcBufAddr, pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset, u4GoldenFileSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    // Check result
    if (!_MemCmp(pbGoldenBufAddr, pbDstBufAddr, u4GoldenFileSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

     //Disable DDI IOmmu
    _DDI_DisableIOMMU();

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DDI_IOMMU3
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DDI_IOMMU3(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    UINT32 u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4GoldenFileSize, u4BufSize, u4SrcOffset,pbSrcBufAddr, pbDstBufAddr;
    UINT32 pbGoldenBufAddr, i;
    UINT32 u4Size = 0x10000;

    UINT32 u4SrcVA = 0xc0000000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8700000, u4SrcVA); //phy=135M, virt=3584M

    HalFlushInvalidateDCache();

    u4GoldenFileSize = u4Size - (u4Size % 188);
    u4BufSize = _DMX_Align(u4GoldenFileSize + 188, DMX_CA_BUFFER_ALIGNMENT); //pbSrcBufAddr + u4BufSize = u4EndAdd;
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = u4SrcVA;
    pbDstBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_CA_BUFFER_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    Printf("Transfer Size = %u .\n", u4GoldenFileSize);

    //generate random golden data.
    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    //copy golden data to src buffer
    if (!_DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize))
    {
        goto _exit;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set dst buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, pbDstBufAddr, pbDstBufAddr + u4BufSize,
                                u4BufSize))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

     HalFlushInvalidateDCache();

    //enable IOMMU
    _DDI_EnableIOMMU(pbSrcBufAddr, pbSrcBufAddr + u4BufSize);

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(pbSrcBufAddr, pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset, u4GoldenFileSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    Printf("pbGoldenBufAddr: 0x%08x, DDI_start: 0x%08x, Dst:0x%08x.\n", pbGoldenBufAddr, pbSrcBufAddr + u4SrcOffset, pbDstBufAddr);
    // Check result
    if (!_MemCmp(pbGoldenBufAddr, pbDstBufAddr, u4GoldenFileSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }

    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

     //Disable DDI IOmmu
    _DDI_DisableIOMMU();

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DDI_IOMMU4
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DDI_IOMMU4(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    UINT32 u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4GoldenFileSize, u4BufSize, u4SrcOffset,pbSrcBufAddr, pbDstBufAddr;
    UINT32 pbGoldenBufAddr, i = 0;
    UINT32 u4Size = 0x1000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);

    HalSet4KPageTable(0x8501000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M

    HalFlushInvalidateDCache();

    u4GoldenFileSize = u4Size - (u4Size % 188);
    u4BufSize = _DMX_Align(u4GoldenFileSize, DMX_CA_BUFFER_ALIGNMENT); //pbSrcBufAddr + u4BufSize = u4EndAdd;

    //make rp reach the BufEnd Addr
    u4SrcOffset = 0x400;

    //0x9602000 has no valid page table for overread happen
    //u4GoldenFileSize = 3948, u4BufSize = 3968 = 0xF80,
    //make BufEnd Add is 0x9600FFF, so SrcBufAddr is 0x9601000 - 0xF80 = 0x9600080;
    pbSrcBufAddr = 0x9600080;
    pbDstBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_CA_BUFFER_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);

    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    Printf("Transfer Size = %u .\n", u4GoldenFileSize);

    //generate random golden data.
    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    //copy golden data to src buffer
    if (!_DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize))
    {
        goto _exit;
    }

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set dst buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, pbDstBufAddr, pbDstBufAddr + u4BufSize,
                                u4BufSize))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    //register DDI IOMMU ISR
    DDI_InitIOMMUISR();

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

     HalFlushInvalidateDCache();

    //enable IOMMU
    _DDI_EnableIOMMU(pbSrcBufAddr, pbSrcBufAddr + u4BufSize);

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(pbSrcBufAddr, pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset, u4GoldenFileSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();


    // Check result
    if (!_MemCmp(pbGoldenBufAddr, pbDstBufAddr, u4GoldenFileSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    fgRet = TRUE;

_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

     //Disable DDI IOmmu
    _DDI_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DDI_IOMMU5
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DDI_IOMMU5(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    //UINT32 u4SrcDiff, u4DstDiff;
    BOOL fgRet = FALSE;
    UINT32 u4GoldenFileSize, u4BufSize, u4SrcOffset,pbSrcBufAddr, pbDstBufAddr;
    UINT32 pbGoldenBufAddr, i;
    UINT32 u4Size = 0x3000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);

    HalSet4KPageTable(0x8501000, 0x9600000, (UINT32*)0xa000000); //phy=133M, virt=150M
    //HalSet4KPageTable(0x8500000, 0x9601000, (UINT32*)0xa000000); //phy=133M, virt=150M


    HalFlushInvalidateDCache();

    u4GoldenFileSize = u4Size - (u4Size % 188);
    u4BufSize = _DMX_Align(u4GoldenFileSize + 188, DMX_CA_BUFFER_ALIGNMENT); //pbSrcBufAddr + u4BufSize = u4EndAdd;
    u4SrcOffset = rand() % u4BufSize;
    pbSrcBufAddr = 0x9600000;
    pbDstBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_CA_BUFFER_ALIGNMENT);
    pbGoldenBufAddr = BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    if (pbSrcBufAddr == 0 || pbDstBufAddr == 0 || pbGoldenBufAddr == 0)
    {
        ASSERT(0);
    }

    //generate random golden data.
    for(i = 0; i <u4GoldenFileSize; i++)
    {
        *(BYTE *)(pbGoldenBufAddr+i) = rand() % 0xff;
    }

    //Mark copy Ring buffer ,because buffer overread
    #if 0
    //copy golden data to src buffer
    if (!_DMX_CopyDestRingBuffer(pbSrcBufAddr + u4SrcOffset, pbSrcBufAddr,
        pbSrcBufAddr + u4BufSize, pbGoldenBufAddr, u4GoldenFileSize))
    {
        goto _exit;
    }
    #endif

    // Set to bypass mode
    if (!_DMX_SetBypassMode(u1TsIndex, 188, FALSE, TRUE)) // Size is irrelevant
    {
        goto _exit;
    }

    // Set dst buffer
    if (!_DMX_SetCaOutputBuffer(TRUE, TRUE, pbDstBufAddr, pbDstBufAddr + u4BufSize,
                                u4BufSize))
    {
        goto _exit;
    }

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    //register DDI IOMMU ISR
    DDI_InitIOMMUISR();

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

     HalFlushInvalidateDCache();

    //enable IOMMU
    _DDI_EnableIOMMU(pbSrcBufAddr, pbSrcBufAddr + u4BufSize);

    Printf("BufStart: 0x%x, BufEnd: 0x%x, BufBegin: 0x%x.\n",
            pbSrcBufAddr,pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset);

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(pbSrcBufAddr, pbSrcBufAddr + u4BufSize, pbSrcBufAddr + u4SrcOffset, u4GoldenFileSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();


_exit:

    // Free memory
    VERIFY(BSP_FreeAlignedDmaMemory(pbDstBufAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(pbGoldenBufAddr));

     //Disable DDI IOmmu
    _DDI_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_DDI_IOMMU
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_DDI_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode)
{
    if (u1Mode == 1)
    {
        return _DMX_EMU_DDI_IOMMU1(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 2)
    {
        return _DMX_EMU_DDI_IOMMU2(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 3)
    {
        return _DMX_EMU_DDI_IOMMU3(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 4)
    {
        return _DMX_EMU_DDI_IOMMU4(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 5)
    {
        return _DMX_EMU_DDI_IOMMU5(u1TsIndex, fgBypassFramer);
    }
    else
    {
        Printf("Mode input Error!\n");
        return FALSE;
    }
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU1
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMX_IOMMU1(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x8000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8100000, 0x9000000); //phy=129M, virt=144M
    HalSet1MSectionTable(0x8000000, 0x9100000); //phy=128M, virt=145M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;
    u4DstOffset = rand() % u4BufSize;
    // Allocate source and destination buffers

    u4DstAddr = 0x90FF000;
    u4DstStartAddr = u4DstAddr + u4DstOffset;

    x_memset((void *)u4DstAddr, 0 , u4TsSize);

    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr

    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);


    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU2
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMX_IOMMU2(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x10000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;
    UINT32 i;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);

    for(i = 0; i < 0x100000; i+=0x2000)
    {
        HalSet4KPageTable(0x8501000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
        HalSet4KPageTable(0x8500000 + i, 0x9601000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
    }

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;
    u4DstOffset = rand() % u4BufSize;
    // Allocate source and destination buffers

    u4DstAddr = 0x9600000;
    u4DstStartAddr = u4DstAddr + u4DstOffset;

    x_memset((void *)u4DstAddr, 0 , u4TsSize);

    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr

    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);


    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU3
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMX_IOMMU3(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x10000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4SrcVA = 0xc0000000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8700000, u4SrcVA); //phy=135M, virt=3584M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;
    u4DstOffset = rand() % u4BufSize;
    // Allocate source and destination buffers

    u4DstAddr = u4SrcVA;
    u4DstStartAddr = u4DstAddr + u4DstOffset;

    x_memset((void *)u4DstAddr, 0 , u4TsSize);

    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr

    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);


    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU4
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMX_IOMMU4(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid1,rPid2;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr1, u4PesSize1 = 0, u4PesAddr2, u4PesSize2 = 0, u4Flags;
    const UINT8 u1Pidx1 = 0;
    const UINT8 u1Pidx2 = 1;
    BOOL fgRet = FALSE;
    UINT16 u2Pid1 = 0x100;
    UINT16 u2Pid2= 0x200;
    UINT32 u4RandomSize = 0x8000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4DstAddr2, u4SrcAddr, u4DstOffset, u4DstOffset2, u4DstStartAddr, u4DstStartAddr2;
    PID_STRUCT_T* prPidStruct;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8100000, 0x9000000); //phy=129M, virt=144M
    HalSet1MSectionTable(0x8000000, 0x9100000); //phy=128M, virt=145M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;
    u4DstOffset = rand() % u4BufSize;
    // Allocate source and destination buffers

    u4DstAddr = 0x90FF000;
    u4DstStartAddr = u4DstAddr + u4DstOffset;
    x_memset((void *)u4DstAddr, 0 , u4TsSize);

    u4DstAddr2 = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1,DMX_ES_FIFO_ALIGNMENT);
    u4DstOffset2 = rand() % u4BufSize;
    u4DstStartAddr2 = u4DstAddr2 + u4DstOffset2;
    x_memset((void *)u4DstAddr2, 0 , u4TsSize);

    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4BufSize * 2 + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr

    u4PesAddr1 = (UINT32)x_mem_alloc(u4BufSize);

    u4PesAddr2 = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4SrcAddr == 0) || (u4DstAddr2 == 0) || (u4PesAddr1 == 0) || (u4PesAddr2 == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize * 2);

    // Translate to TS format , First pid ts
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4TsSize, u2Pid1, u4PesAddr1,
                                 &u4PesSize1))
    {
        goto _exit;
    }
    Printf("First Golden data, TS size %u, PES Addr: %x, PES size = %u\n", u4TsSize, u4PesAddr1 ,u4PesSize1);

    // Translate to TS format , Section pid ts
    if (!_GenerateSimpleTsHeader(u4SrcAddr + u4TsSize, u4TsSize, u2Pid2, u4PesAddr2,
                                 &u4PesSize2))
    {
        goto _exit;
    }

    Printf("Second Golden data, TS size %u, PES Addr: %x, PES size = %u\n", u4TsSize, u4PesAddr2 ,u4PesSize2);


    // Setup First  PID
    x_memset((VOID *)&rPid1,0,sizeof(DMX_PID_T));
    rPid1.fgEnable = TRUE;
    rPid1.fgAllocateBuffer = FALSE;
    rPid1.fgPrimary = TRUE;
    rPid1.u1TsIndex = u1TsIndex;
    rPid1.u1DeviceId = 0;
    rPid1.u1KeyIndex = 0;
    rPid1.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid1.u2Pid = u2Pid1;
    rPid1.u4BufAddr = u4DstAddr;
    rPid1.u4BufSize = u4BufSize;
    rPid1.ePcrMode = DMX_PCR_MODE_NONE;
    rPid1.ePidType = DMX_PID_TYPE_PES;
    rPid1.eDescMode = DMX_DESC_MODE_NONE;
    rPid1.pfnNotify = NULL;
    rPid1.pvNotifyTag = NULL;
    rPid1.pfnScramble = NULL;
    rPid1.pvScrambleTag = NULL;
    rPid1.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx1, u4Flags, &rPid1, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx1);
    PID_S_W(u1Pidx1, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx1, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx1, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();


    // Setup Second  PID
    x_memset((VOID *)&rPid2,0,sizeof(DMX_PID_T));
    rPid2.fgEnable = TRUE;
    rPid2.fgAllocateBuffer = FALSE;
    rPid2.fgPrimary = TRUE;
    rPid2.u1TsIndex = u1TsIndex;
    rPid2.u1DeviceId = 0;
    rPid2.u1KeyIndex = 0;
    rPid2.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid2.u2Pid = u2Pid2;
    rPid2.u4BufAddr = u4DstAddr2;
    rPid2.u4BufSize = u4BufSize;
    rPid2.ePcrMode = DMX_PCR_MODE_NONE;
    rPid2.ePidType = DMX_PID_TYPE_PES;
    rPid2.eDescMode = DMX_DESC_MODE_NONE;
    rPid2.pfnNotify = NULL;
    rPid2.pvNotifyTag = NULL;
    rPid2.pfnScramble = NULL;
    rPid2.pvScrambleTag = NULL;
    rPid2.fgIommuEnable = FALSE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx2, u4Flags, &rPid2, FALSE))
    {
      return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
    prPidStruct = _DMX_GetPidStruct(u1Pidx2);
    PID_S_W(u1Pidx2, 7) = u4DstStartAddr2;     // PES_start
    PID_S_W(u1Pidx2, 8) = u4DstStartAddr2;     // Write pointer
    PID_S_W(u1Pidx2, 9) = u4DstStartAddr2;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr2;
    prPidStruct->u4Rp = u4DstStartAddr2;
    prPidStruct->u4SectionRp = u4DstStartAddr2;
    prPidStruct->u4PesRp = u4DstStartAddr2;
    _DMX_Unlock();


    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4TsSize * 2))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr1, u4PesAddr1,
        u4PesAddr1 + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize1))
    {
        Printf("DMX IOMMU  Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("Pid index 0 DMX IOMMU Pass.  \n");
    }

    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr2, u4PesAddr2,
        u4PesAddr2 + u4BufSize, u4DstStartAddr2,
        u4DstAddr2, u4DstAddr2 + u4BufSize, u4PesSize2))
    {
        Printf("Pid index 1 DMX Disable IOMMU Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("Pid index 1 DMX Disable IOMMU Pass.  \n");
    }
    fgRet = TRUE;

_exit:

    // Disable PID
    rPid1.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx1, DMX_PID_FLAG_VALID, &rPid1, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx1));

    rPid2.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx2, DMX_PID_FLAG_VALID, &rPid2, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx2));

    // Free memory
    x_mem_free((void*)u4PesAddr1);
    x_mem_free((void*)u4PesAddr2);
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));
    VERIFY(BSP_FreeAlignedDmaMemory(u4DstAddr2));


    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU5
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMX_IOMMU5(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x10000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);

    //only setup 4K table, 0x10000 size will cause table miss,
    //This test will check IOMMU HW ISR
    HalSet4KPageTable(0x8501000, 0x9600000, (UINT32*)0xa000000); //phy=133M, virt=150M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;
    u4DstOffset = 0;
    // Allocate source and destination buffers

    u4DstAddr = 0x9600000;
    u4DstStartAddr = u4DstAddr + u4DstOffset;

    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4BufSize + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr

    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4SrcAddr == 0) || (u4DstAddr == 0) || (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4SrcAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);


    // Setup PID
    x_memset((void*)&rPid, 0, sizeof(rPid));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    //register DMX IOMMU ISR
    DMX_InitIOMMUISR();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4TsSize))
    {
        // Error on transferring data
        Printf("Error on transferring data!\n");
        goto _exit;
    }


_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);
    VERIFY(BSP_FreeAlignedDmaMemory(u4SrcAddr));

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}


//-----------------------------------------------------------------------------
/** DMX_EMU_DMX_IOMMU
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_DMX_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode)
{
    if (u1Mode == 1)
    {
        return _DMX_EMU_DMX_IOMMU1(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 2)
    {
        return _DMX_EMU_DMX_IOMMU2(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 3)
    {
        return _DMX_EMU_DMX_IOMMU3(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 4)
    {
        return _DMX_EMU_DMX_IOMMU4(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 5)
    {
        return _DMX_EMU_DMX_IOMMU5(u1TsIndex, fgBypassFramer);
    }
    else
    {
        Printf("Mode input Error!\n");
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU1
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMXDDI_IOMMU1(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x8000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4SrcOffset ,u4SrcStartAddr, u4GoldenBufAddr;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8100000, 0x9000000); //phy=129M, virt=144M
    HalSet1MSectionTable(0x8000000, 0x9100000); //phy=128M, virt=145M
    HalSet1MSectionTable(0x8300000, 0x9200000); //phy=131M, virt=146M
    HalSet1MSectionTable(0x8200000, 0x9300000); //phy=130M, virt=147M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;

    // Allocate source and destination buffers
    u4SrcAddr = 0x92FF000;
    u4SrcOffset = rand() % u4BufSize;
    u4SrcStartAddr = u4SrcAddr + u4SrcOffset;
    x_memset((void *)u4SrcAddr, 0 , u4TsSize);
    Printf("Src Addr: 0x%08x, Src Start: 0x%08x.\n", u4SrcAddr, u4SrcStartAddr);


    u4DstAddr = 0x90FF000;
    u4DstOffset = rand() % u4BufSize;
    u4DstStartAddr = u4DstAddr + u4DstOffset;
    x_memset((void *)u4DstAddr, 0 , u4TsSize);
    Printf("Dst Addr: 0x%08x, Dst Start: 0x%08x.\n", u4DstAddr, u4DstStartAddr);

    //
    u4GoldenBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4GoldenBufAddr == 0)|| (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4GoldenBufAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4GoldenBufAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);

    //copy ts data to src buffer
    if (!_DMX_CopyDestRingBuffer(u4SrcStartAddr, u4SrcAddr,
        u4SrcAddr + u4BufSize, u4GoldenBufAddr, u4TsSize))
    {
        goto _exit;
    }

    // Setup PID
    x_memset((VOID *)&rPid, 0 , sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    //enable IOMMU
    _DDI_EnableIOMMU(u4SrcAddr, u4SrcAddr + u4BufSize);

    HalFlushInvalidateDCache();

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(u4SrcAddr, u4SrcAddr + u4BufSize, u4SrcStartAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);

    VERIFY(BSP_FreeAlignedDmaMemory(u4GoldenBufAddr));

    //Disable DDi IOMMU
    _DDI_DisableIOMMU();

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU2
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMXDDI_IOMMU2(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x8000;
    UINT32 u4TsSize , u4BufSize, i;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4SrcOffset ,u4SrcStartAddr, u4GoldenBufAddr;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);
    x_memset((void*)0xa000400, 0, 0x400);

    for(i = 0; i < 0x100000; i+=0x2000)
    {
        HalSet4KPageTable(0x8501000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
        HalSet4KPageTable(0x8500000 + i, 0x9601000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M

        HalSet4KPageTable(0x8601000 + i, 0x9700000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M
        HalSet4KPageTable(0x8600000 + i, 0x9701000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M
    }

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;

    // Allocate source and destination buffers
    u4SrcAddr = 0x9600000;
    u4SrcOffset = rand() % u4BufSize;
    u4SrcStartAddr = u4SrcAddr + u4SrcOffset;
    x_memset((void *)u4SrcAddr, 0 , u4TsSize);
    Printf("Src Addr: 0x%08x, Src Start: 0x%08x.\n", u4SrcAddr, u4SrcStartAddr);


    u4DstAddr = 0x9700000;
    u4DstOffset = rand() % u4BufSize;
    u4DstStartAddr = u4DstAddr + u4DstOffset;
    x_memset((void *)u4DstAddr, 0 , u4TsSize);
    Printf("Dst Addr: 0x%08x, Dst Start: 0x%08x.\n", u4DstAddr, u4DstStartAddr);

    //
    u4GoldenBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4GoldenBufAddr == 0)|| (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4GoldenBufAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4GoldenBufAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);

    //copy ts data to src buffer
    if (!_DMX_CopyDestRingBuffer(u4SrcStartAddr, u4SrcAddr,
        u4SrcAddr + u4BufSize, u4GoldenBufAddr, u4TsSize))
    {
        goto _exit;
    }

    // Setup PID
    x_memset((VOID *)&rPid, 0 , sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    //enable IOMMU
    _DDI_EnableIOMMU(u4SrcAddr, u4SrcAddr + u4BufSize);

    HalFlushInvalidateDCache();

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(u4SrcAddr, u4SrcAddr + u4BufSize, u4SrcStartAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);

    VERIFY(BSP_FreeAlignedDmaMemory(u4GoldenBufAddr));

    //Disable DDi IOMMU
    _DDI_DisableIOMMU();

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU3
 */
//-----------------------------------------------------------------------------
BOOL _DMX_EMU_DMXDDI_IOMMU3(UINT8 u1TsIndex, BOOL fgBypassFramer)
{
    DMX_PID_T rPid;
    //UINT32 u4SrcDiff, u4DstDiff;
    UINT32 u4PesAddr, u4PesSize = 0, u4Flags;
    const UINT8 u1Pidx = 0;
    BOOL fgRet = FALSE;
    UINT16 u2Pid = 0x100;
    UINT32 u4RandomSize = 0x10000;
    UINT32 u4TsSize , u4BufSize;
    UINT32 u4DstAddr, u4SrcAddr, u4DstOffset, u4DstStartAddr;
    PID_STRUCT_T* prPidStruct;
    UINT32 u4SrcOffset ,u4SrcStartAddr, u4GoldenBufAddr;
    UINT32 u4SrcVA = 0xc0000000;
    UINT32 u4DesVA = 0xc0100000;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    HalSet1MSectionTable(0x8700000, u4SrcVA); //phy=135M, virt=3584M
    HalSet1MSectionTable(0x8800000, u4DesVA); //phy=136M, virt=3585M

    HalFlushInvalidateDCache();

    u4TsSize = u4RandomSize - (u4RandomSize % 188);
    u4BufSize = _DMX_Align(u4TsSize + 188 , DMX_ES_FIFO_ALIGNMENT); //u4DstAddr + u4BufSize = u4DstEnd;

    // Allocate source and destination buffers
    u4SrcAddr = u4SrcVA;
    u4SrcOffset = rand() % u4BufSize;
    u4SrcStartAddr = u4SrcAddr + u4SrcOffset;
    x_memset((void *)u4SrcAddr, 0 , u4TsSize);
    Printf("Src Addr: 0x%08x, Src Start: 0x%08x.\n", u4SrcAddr, u4SrcStartAddr);


    u4DstAddr = u4DesVA;
    u4DstOffset = rand() % u4BufSize;
    u4DstStartAddr = u4DstAddr + u4DstOffset;
    x_memset((void *)u4DstAddr, 0 , u4TsSize);
    Printf("Dst Addr: 0x%08x, Dst Start: 0x%08x.\n", u4DstAddr, u4DstStartAddr);

    //
    u4GoldenBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4BufSize, DDI_BUF_ALIGNMENT);
    u4PesAddr = (UINT32)x_mem_alloc(u4BufSize);

    if ((u4GoldenBufAddr == 0)|| (u4PesAddr == 0))
    {
        goto _exit;
    }

    // Prepare data
    _GenerateRandomPattern(u4GoldenBufAddr, u4TsSize);

    // Translate to TS format
    if (!_GenerateSimpleTsHeader(u4GoldenBufAddr, u4TsSize, u2Pid, u4PesAddr,
                                 &u4PesSize))
    {
        goto _exit;
    }

    Printf("TS size %u, PES size = %u\n", u4TsSize, u4PesSize);

    //copy ts data to src buffer
    if (!_DMX_CopyDestRingBuffer(u4SrcStartAddr, u4SrcAddr,
        u4SrcAddr + u4BufSize, u4GoldenBufAddr, u4TsSize))
    {
        goto _exit;
    }

    // Setup PID
    x_memset((VOID *)&rPid, 0 , sizeof(DMX_PID_T));
    rPid.fgEnable = TRUE;
    rPid.fgAllocateBuffer = FALSE;
    rPid.fgPrimary = TRUE;
    rPid.u1TsIndex = u1TsIndex;
    rPid.u1DeviceId = 0;
    rPid.u1KeyIndex = 0;
    rPid.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid.u2Pid = u2Pid;
    rPid.u4BufAddr = u4DstAddr;
    rPid.u4BufSize = u4BufSize;
    rPid.ePcrMode = DMX_PCR_MODE_NONE;
    rPid.ePidType = DMX_PID_TYPE_PES;
    rPid.eDescMode = DMX_DESC_MODE_NONE;
    rPid.pfnNotify = NULL;
    rPid.pvNotifyTag = NULL;
    rPid.pfnScramble = NULL;
    rPid.pvScrambleTag = NULL;
    rPid.fgIommuEnable = TRUE;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(u1Pidx, u4Flags, &rPid, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(u1Pidx);
    PID_S_W(u1Pidx, 7) = u4DstStartAddr;     // PES_start
    PID_S_W(u1Pidx, 8) = u4DstStartAddr;     // Write pointer
    PID_S_W(u1Pidx, 9) = u4DstStartAddr;     // Read pointer
    prPidStruct->u4Wp = u4DstStartAddr;
    prPidStruct->u4Rp = u4DstStartAddr;
    prPidStruct->u4SectionRp = u4DstStartAddr;
    prPidStruct->u4PesRp = u4DstStartAddr;
    _DMX_Unlock();

    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    //enable IOMMU
    _DDI_EnableIOMMU(u4SrcAddr, u4SrcAddr + u4BufSize);

    HalFlushInvalidateDCache();

    // Invoke DDI transfer
    if (!_DDI_IOMMU_Transfer(u4SrcAddr, u4SrcAddr + u4BufSize, u4SrcStartAddr, u4TsSize))
    {
        // Error on transferring data
        goto _exit;
    }

     HalFlushInvalidateDCache();

    // Check result
    #if 0
    if (!_MemCmp(u4PesAddr, u4DstAddr, u4PesSize, &u4SrcDiff, &u4DstDiff))
    {
        Printf("Differ at src: 0x%08x, dst: 0x%08x\n", u4SrcDiff, u4DstDiff);
        goto _exit;
    }
    #else
    if (0xFFFFFFFF != _MemCmpRing(u4PesAddr, u4PesAddr,
        u4PesAddr + u4BufSize, u4DstStartAddr,
        u4DstAddr, u4DstAddr + u4BufSize, u4PesSize))
    {
        Printf("DMX IOMMU  non-Continuous Fail.  \n");
        goto _exit;
    }
    else
    {
        Printf("DMX IOMMU  non-Continuous Pass.  \n");
    }
    #endif

    fgRet = TRUE;

_exit:

    // Disable PID
    rPid.fgEnable = FALSE;
    VERIFY(_DMX_SetPid(u1Pidx, DMX_PID_FLAG_VALID, &rPid, FALSE));

    // Free PID
    VERIFY(_DMX_FreePid(u1Pidx));

    // Free memory
    x_mem_free((void*)u4PesAddr);

    VERIFY(BSP_FreeAlignedDmaMemory(u4GoldenBufAddr));

    //Disable DDi IOMMU
    _DDI_DisableIOMMU();

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();
    return fgRet;
}

//-----------------------------------------------------------------------------
/** DMX_EMU_DMX_IOMMU
 */
//-----------------------------------------------------------------------------
BOOL DMX_EMU_DMXDDI_IOMMU(UINT8 u1TsIndex, BOOL fgBypassFramer, UINT8 u1Mode)
{
    if (u1Mode == 1)
    {
        return _DMX_EMU_DMXDDI_IOMMU1(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 2)
    {
        return _DMX_EMU_DMXDDI_IOMMU2(u1TsIndex, fgBypassFramer);
    }
    else if (u1Mode == 3)
    {
        return _DMX_EMU_DMXDDI_IOMMU3(u1TsIndex, fgBypassFramer);
    }
    else
    {
        Printf("Mode input Error!\n");
        return FALSE;
    }
}


//-----------------------------------------------------------------------------
/** _DMX_EMU_DMX_IOMMU2
  *  Enalbe 3 pid,
     2 pid enalbel IOMMU, and the other pid disable IOMMU  test performance
     1 pid enalbe IOMMU, and The other two  pid disalbe IOMMU,Test performance
 */
//-----------------------------------------------------------------------------

BOOL DMX_EMU_IOMMU_PerformanceTest(UINT8 u1TsIndex, BOOL fgBypassFramer, BOOL fgIommu1, BOOL fgIommu2, BOOL fgIommu3)
{
    DMX_PID_T rPid1,rPid2,rPid3;
    UINT32 i;
    UINT32 u4Flags;
    BOOL fgRet = FALSE;
    UINT32 u4TsSize = 0x200000; //2M
    UINT32 u4PerPesTS_Size , u4RandomSize;
    UINT32 u4SrcAddr, u4SrcAddr_Tmp,u4SrcOffset;
    UINT8 u1PatIdx = 0;
    UINT32 u4RemainSize, u4PesSize_tmp;
    PID_STRUCT_T* prPidStruct;

    ASSERT(u1TsIndex <= MAX_TS_INDEX);

    //change mmu table
    x_memset((void*)0xa000000, 0, 0x400);
    x_memset((void*)0xa000400, 0, 0x400);
    x_memset((void*)0xa000800, 0, 0x400);
    x_memset((void*)0xa000c00, 0, 0x400);
    x_memset((void*)0xa001000, 0, 0x400);
    x_memset((void*)0xa001400, 0, 0x400);

    for(i = 0; i < 0x100000; i+=0x2000)
    {
        HalSet4KPageTable(0x8201000 + i, 0x9600000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M
        HalSet4KPageTable(0x8700000 + i, 0x9601000 + i, (UINT32*)0xa000000); //phy=133M, virt=150M

        HalSet4KPageTable(0x8701000 + i, 0x9700000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M
        HalSet4KPageTable(0x8200000 + i, 0x9701000 + i, (UINT32*)0xa000400); //phy=134M, virt=151M

        HalSet4KPageTable(0x8601000 + i, 0x9500000 + i, (UINT32*)0xa000800);
        HalSet4KPageTable(0x8100000 + i, 0x9501000 + i, (UINT32*)0xa000800);

        HalSet4KPageTable(0x8101000 + i, 0x9400000 + i, (UINT32*)0xa000c00);
        HalSet4KPageTable(0x8600000 + i, 0x9401000 + i, (UINT32*)0xa000c00);

        HalSet4KPageTable(0x8501000 + i, 0x9300000 + i, (UINT32*)0xa001000);
        HalSet4KPageTable(0x8000000 + i, 0x9301000 + i, (UINT32*)0xa001000);

        HalSet4KPageTable(0x8001000 + i, 0x9200000 + i, (UINT32*)0xa001400);
        HalSet4KPageTable(0x8500000 + i, 0x9201000 + i, (UINT32*)0xa001400);
    }

    HalFlushInvalidateDCache();


    //Allocate src addr and goldel pes addr
    u4SrcAddr =(UINT32)BSP_AllocAlignedDmaMemory(u4TsSize + 1, DDI_BUF_ALIGNMENT); //DDI Src Addr


    // Allocate source and destination buffers
    for (i = 0; i < _u1DmxPidNum; i++)
    {
        x_memset((void *)&_arPes[i], 0 , sizeof(PES_T));
    }

    //Dest 1,
    if (fgIommu1)
    {
        _arPes[0].u4BufAddr = 0x9200000;
    }
    else
    {
        _arPes[0].u4BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4TsSize + 1,DMX_ES_FIFO_ALIGNMENT);
    }

    u4SrcOffset = 0;//rand() % u4TsSize;
    _arPes[0].u4BufStartAddr= _arPes[0].u4BufAddr + u4SrcOffset;
    _arPes[0].fgIommuEnable = fgIommu1;
    _arPes[0].u1Pidx = 0;
    _arPes[0].u2PidNum = 0x100;
    _arPes[0].u4GoldenAddr = (UINT32)x_mem_alloc(u4TsSize);
    _arPes[0].u4BufSize = u4TsSize;
    _arPes[0].u4PesAddr= _arPes[0].u4GoldenAddr;
    _arPes[0].u1CCounter = 0xF;

    if ((_arPes[0].u4GoldenAddr == 0) || (_arPes[0].u4BufAddr == 0))
    {
        goto _exit;
    }

    x_memset((void *)_arPes[0].u4GoldenAddr, 0 , u4TsSize);
    x_memset((void *)_arPes[0].u4BufAddr, 0 , u4TsSize);

    //Dest 2,
    if (fgIommu2)
    {
        _arPes[1].u4BufAddr = 0x9400000;
    }
    else
    {
        _arPes[1].u4BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4TsSize + 1,DMX_ES_FIFO_ALIGNMENT);
    }

     u4SrcOffset = 0;//rand() % u4TsSize;
    _arPes[1].u4BufStartAddr= _arPes[1].u4BufAddr + u4SrcOffset;
    _arPes[1].fgIommuEnable = fgIommu2;
    _arPes[1].u1Pidx = 1;
    _arPes[1].u2PidNum = 0x200;
    _arPes[1].u4GoldenAddr = (UINT32)x_mem_alloc(u4TsSize);
    _arPes[1].u4BufSize = u4TsSize;
    _arPes[1].u4PesAddr= _arPes[1].u4GoldenAddr;
    _arPes[1].u1CCounter = 0xF;

    if ((_arPes[1].u4GoldenAddr == 0) || (_arPes[1].u4BufAddr == 0))
    {
        goto _exit;
    }

    x_memset((void *)_arPes[1].u4GoldenAddr, 0 , u4TsSize);
    x_memset((void *)_arPes[1].u4BufAddr, 0 , u4TsSize);


    //Dest3,
    if (fgIommu3)
    {
        _arPes[2].u4BufAddr = 0x9600000;
    }
    else
    {
        _arPes[2].u4BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(u4TsSize + 1,DMX_ES_FIFO_ALIGNMENT);
    }

    u4SrcOffset = 0;//rand() % u4TsSize;
    _arPes[2].u4BufStartAddr= _arPes[2].u4BufAddr + u4SrcOffset;
    _arPes[2].fgIommuEnable = fgIommu3;
    _arPes[2].u1Pidx = 2;
    _arPes[2].u2PidNum = 0x300;
    _arPes[2].u4GoldenAddr = (UINT32)x_mem_alloc(u4TsSize);
    _arPes[2].u4BufSize = u4TsSize;
    _arPes[2].u4PesAddr= _arPes[2].u4GoldenAddr;
    _arPes[2].u1CCounter = 0xF;

    if ((_arPes[2].u4GoldenAddr == 0) || (_arPes[2].u4BufAddr == 0))
    {
        goto _exit;
    }

    x_memset((void *)_arPes[2].u4GoldenAddr, 0 , u4TsSize);
    x_memset((void *)_arPes[2].u4BufAddr, 0 , u4TsSize);


    // Prepare data
    _GenerateRandomPattern(u4SrcAddr, u4TsSize);

    u4RemainSize = u4TsSize;
    u4SrcAddr_Tmp = u4SrcAddr;
    //Translate to TS format ,
    while (u4RemainSize > 0)
    {
        while (1)
        {
            u4RandomSize = 0xf000; //60k  //(UINT32)rand() %  PES_LEN_MAX;
            if (u4RandomSize > PES_LEN_MIN)
            {
                break;
            }
        }

        //change 188 ts size
        u4PerPesTS_Size = u4RandomSize - (u4RandomSize % 188);

        if (u4PerPesTS_Size > u4RemainSize)   // Not enough buffer size
        {
            break;
        }

        //find avaible pidindex
#if 0
        while (1)
        {
            if (_u1DmxPidNum == 1)
            {
                u1PatIdx = 0;
            }
            else
            {
                u1PatIdx = 0; //(UINT8)(rand() % _u1DmxPidNum);
            }
            if ((_arPes[u1PatIdx].u4BufSize - _arPes[u1PatIdx].u4PesSize) > u4PerPesTS_Size)
            {
                break;
            }
        }
#else
        u1PatIdx++;
        u1PatIdx = u1PatIdx % _u1DmxPidNum;
        if ((_arPes[u1PatIdx].u4BufSize - _arPes[u1PatIdx].u4PesSize) < u4PerPesTS_Size)
        {
            Printf("Buffer size is not enough!\n");
            goto _exit;
        }
#endif


        // Translate to TS format , First pid ts
        if (!_GenerateSimpleTsHeaderEx(u4SrcAddr_Tmp, u4PerPesTS_Size, _arPes[u1PatIdx].u2PidNum, _arPes[u1PatIdx].u4PesAddr,
                                     &u4PesSize_tmp, &_arPes[u1PatIdx].u1CCounter))
        {
            goto _exit;
        }

        u4SrcAddr_Tmp += u4PerPesTS_Size;
        u4RemainSize -= u4PerPesTS_Size;
        _arPes[u1PatIdx].u4PesSize += u4PesSize_tmp;
        _arPes[u1PatIdx].u4PesAddr += u4PesSize_tmp;
    }


    Printf("First Golden data, DstAddr 0x%x, PES Addr: 0x%x, PES size = 0x%x\n",
                        _arPes[0].u4BufAddr, _arPes[0].u4GoldenAddr,_arPes[0].u4PesSize);
    Printf("First Section data, DstAddr 0x%x, PES Addr: 0x%x, PES size = 0x%x\n",
                        _arPes[1].u4BufAddr, _arPes[1].u4GoldenAddr,_arPes[1].u4PesSize);
    Printf("First Third data, DstAddr 0x%x, PES Addr: 0x%x, PES size = 0x%x\n",
                        _arPes[2].u4BufAddr, _arPes[2].u4GoldenAddr,_arPes[2].u4PesSize);

    // Setup First  PID
    x_memset((VOID *)&rPid1,0,sizeof(DMX_PID_T));
    rPid1.fgEnable = TRUE;
    rPid1.fgAllocateBuffer = FALSE;
    rPid1.fgPrimary = TRUE;
    rPid1.u1TsIndex = u1TsIndex;
    rPid1.u1DeviceId = 0;
    rPid1.u1KeyIndex = 0;
    rPid1.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid1.u2Pid = _arPes[0].u2PidNum;
    rPid1.u4BufAddr = _arPes[0].u4BufAddr;
    rPid1.u4BufSize = _arPes[0].u4BufSize;
    rPid1.ePcrMode = DMX_PCR_MODE_NONE;
    rPid1.ePidType = DMX_PID_TYPE_PES;
    rPid1.eDescMode = DMX_DESC_MODE_NONE;
    rPid1.pfnNotify = NULL;
    rPid1.pvNotifyTag = NULL;
    rPid1.pfnScramble = NULL;
    rPid1.pvScrambleTag = NULL;
    rPid1.fgIommuEnable = _arPes[0].fgIommuEnable;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(_arPes[0].u1Pidx, u4Flags, &rPid1, FALSE))
    {
        return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
     prPidStruct = _DMX_GetPidStruct(_arPes[0].u1Pidx);
    PID_S_W(_arPes[0].u1Pidx, 7) = _arPes[0].u4BufStartAddr;     // PES_start
    PID_S_W(_arPes[0].u1Pidx, 8) = _arPes[0].u4BufStartAddr;     // Write pointer
    PID_S_W(_arPes[0].u1Pidx, 9) = _arPes[0].u4BufStartAddr;     // Read pointer
    prPidStruct->u4Wp = _arPes[0].u4BufStartAddr;
    prPidStruct->u4Rp = _arPes[0].u4BufStartAddr;
    prPidStruct->u4SectionRp = _arPes[0].u4BufStartAddr;
    prPidStruct->u4PesRp = _arPes[0].u4BufStartAddr;
    _DMX_Unlock();


    // Setup Second  PID
    x_memset((VOID *)&rPid2,0,sizeof(DMX_PID_T));
    rPid2.fgEnable = TRUE;
    rPid2.fgAllocateBuffer = FALSE;
    rPid2.fgPrimary = TRUE;
    rPid2.u1TsIndex = u1TsIndex;
    rPid2.u1DeviceId = 0;
    rPid2.u1KeyIndex = 0;
    rPid2.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid2.u2Pid = _arPes[1].u2PidNum;
    rPid2.u4BufAddr = _arPes[1].u4BufAddr;
    rPid2.u4BufSize = _arPes[1].u4BufSize;
    rPid2.ePcrMode = DMX_PCR_MODE_NONE;
    rPid2.ePidType = DMX_PID_TYPE_AV_PES;
    rPid2.eDescMode = DMX_DESC_MODE_NONE;
    rPid2.pfnNotify = NULL;
    rPid2.pvNotifyTag = NULL;
    rPid2.pfnScramble = NULL;
    rPid2.pvScrambleTag = NULL;
    rPid2.fgIommuEnable = _arPes[1].fgIommuEnable;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(_arPes[1].u1Pidx, u4Flags, &rPid2, FALSE))
    {
      return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
    prPidStruct = _DMX_GetPidStruct(_arPes[1].u1Pidx);
    PID_S_W(_arPes[1].u1Pidx, 7) = _arPes[1].u4BufStartAddr;     // PES_start
    PID_S_W(_arPes[1].u1Pidx, 8) = _arPes[1].u4BufStartAddr;     // Write pointer
    PID_S_W(_arPes[1].u1Pidx, 9) = _arPes[1].u4BufStartAddr;     // Read pointer
    prPidStruct->u4Wp = _arPes[1].u4BufStartAddr;
    prPidStruct->u4Rp = _arPes[1].u4BufStartAddr;
    prPidStruct->u4SectionRp = _arPes[1].u4BufStartAddr;
    prPidStruct->u4PesRp = _arPes[1].u4BufStartAddr;
    _DMX_Unlock();


    // Setup Third  PID
    x_memset((VOID *)&rPid3,0,sizeof(DMX_PID_T));
    rPid3.fgEnable = TRUE;
    rPid3.fgAllocateBuffer = FALSE;
    rPid3.fgPrimary = TRUE;
    rPid3.u1TsIndex = u1TsIndex;
    rPid3.u1DeviceId = 0;
    rPid3.u1KeyIndex = 0;
    rPid3.u1SteerMode = DMX_STEER_TO_FTUP;
    rPid3.u2Pid = _arPes[2].u2PidNum;
    rPid3.u4BufAddr = _arPes[2].u4BufAddr;
    rPid3.u4BufSize = _arPes[2].u4BufSize;
    rPid3.ePcrMode = DMX_PCR_MODE_NONE;
    rPid3.ePidType = DMX_PID_TYPE_AV_PES;
    rPid3.eDescMode = DMX_DESC_MODE_NONE;
    rPid3.pfnNotify = NULL;
    rPid3.pvNotifyTag = NULL;
    rPid3.pfnScramble = NULL;
    rPid3.pvScrambleTag = NULL;
    rPid3.fgIommuEnable = _arPes[2].fgIommuEnable;

    u4Flags = DMX_PID_FLAG_ALL;
    if (!_DMX_SetPid(_arPes[2].u1Pidx, u4Flags, &rPid3, FALSE))
    {
      return FALSE;
    }

    //modify buffer wp/rp/pes_start, make ringbuffer circles
    _DMX_Lock();
    prPidStruct = _DMX_GetPidStruct(_arPes[2].u1Pidx);
    PID_S_W(_arPes[2].u1Pidx, 7) = _arPes[2].u4BufStartAddr;     // PES_start
    PID_S_W(_arPes[2].u1Pidx, 8) = _arPes[2].u4BufStartAddr;     // Write pointer
    PID_S_W(_arPes[2].u1Pidx, 9) = _arPes[2].u4BufStartAddr;     // Read pointer
    prPidStruct->u4Wp = _arPes[2].u4BufStartAddr;
    prPidStruct->u4Rp = _arPes[2].u4BufStartAddr;
    prPidStruct->u4SectionRp = _arPes[2].u4BufStartAddr;
    prPidStruct->u4PesRp = _arPes[2].u4BufStartAddr;
    _DMX_Unlock();


    //Set DBM input source, only ts index 2 and 3 can confige DBM input source
    if ((u1TsIndex == 2) || (u1TsIndex == 3))
    {
        if (fgBypassFramer)
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_DDI);
        }
        else
        {
            DMX_SetDbm_InputSource(u1TsIndex, DMX_DBM_INPUT_FRAMER);
        }
    }

    // Init DDI
    if (!_DMX_EMU_DDI_Init(u1TsIndex))
    {
        goto _exit;
    }

    //Enable DMX IOMMU
    _DMX_EnableIOMMU();

    // Invoke DDI transfer
    if (!_DMX_EMU_DDI_Transfer(0, u4SrcAddr, u4SrcAddr_Tmp- u4SrcAddr))
    {
        // Error on transferring data
        goto _exit;
    }

    // Check result
    for (i = 0 ; i < _u1DmxPidNum ; i++)
    {
        if (0xFFFFFFFF != _MemCmpRing(_arPes[i].u4GoldenAddr, _arPes[i].u4GoldenAddr,
        _arPes[i].u4GoldenAddr + _arPes[i].u4BufSize, _arPes[i].u4BufStartAddr,
        _arPes[i].u4BufAddr, _arPes[i].u4BufAddr + _arPes[i].u4BufSize, _arPes[i].u4PesSize))
        {
            Printf("Mem check fail: Pidx : 0x%x. \n", i);
            goto _exit;
        }
        else
        {
            Printf("Mem check OK: Pidx : 0x%x. \n", i);
        }
    }

    fgRet = TRUE;

_exit:

    for (i = 0 ; i < _u1DmxPidNum ; i++)
    {
        // Disable PID
        rPid1.fgEnable = FALSE;
        VERIFY(_DMX_SetPid(_arPes[i].u1Pidx, DMX_PID_FLAG_VALID, &rPid1, FALSE));

        // Free PID
        VERIFY(_DMX_FreePid(_arPes[i].u1Pidx));

        // Free memory
        x_mem_free((void*)_arPes[i].u4GoldenAddr);

        if (!_arPes[i].fgIommuEnable)
        {
            VERIFY(BSP_FreeAlignedDmaMemory(_arPes[i].u4BufAddr));
        }
    }

    //Disable DMX IOMMU
    _DMX_DisableIOMMU();

    return fgRet;
}

