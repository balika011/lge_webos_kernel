/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * Description:
 *         The main routine of boot preloader
 *
 *---------------------------------------------------------------------------*/


/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2769 $
 * ----------------------------------------------------------------------------
 */

#include "x_bim.h"
#include "tzst.h"

EXTERN UINT32 PA_LOAD_BASE, PA_VFIFO_START;
UINT32 TZ_DRAM_START = 0, TZ_DRAM_END = 0;
static UINT8 pu1Key[256+32];

#ifdef CC_TVP_SUPPORT
extern BOOL MID_Init(void);
#endif

void BIM_Trustzone_DRAM_Protection(UINT32 base, UINT32 size)
{
    UINT32 u4DramSc;

    TZ_DRAM_START = base;
    TZ_DRAM_END   = base + size;

    tz_printf("TZ dram: start=0x%08X, end=0x%08X\n", base, base+size);

#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5861) || defined(CC_MT5882)
    // 64-bytes alignment for mustang bim
    UNUSED(u4DramSc);
    if((TZ_DRAM_START & 0x3f) != 0 || (TZ_DRAM_END & 0x3f) != 0)
    {
        Printf("TZ dram protection range should be 64-bytes aligned. Invalid range 0x%08x~0x%08x\n", TZ_DRAM_START, TZ_DRAM_END);
    }

    BIM_WRITE32(0x10, TZ_DRAM_START);   //DRAM_SC_BEGIN
    BIM_WRITE32(0x14, TZ_DRAM_END);   //DRAM_SC_END
    BIM_WRITE32(0x8, BIM_READ32(0x8) | (1 << 4));  // SECURE_MAP

	#ifdef CC_TVP_SUPPORT
	MID_Init();
	#endif

#else
    if (size == 0x4000)         // 16KB
    {
        u4DramSc = 0b000;
    }
    else if (size == 0x10000)   // 64KB
    {
        u4DramSc = 0b001;
    }
    else if (size == 0x40000)   // 256KB
    {
        u4DramSc = 0b010;
    }
    else if (size == 0x100000)  // 1MB
    {
        u4DramSc = 0b011;
    }
    else if (size == 0x200000)  // 2MB
    {
        u4DramSc = 0b100;
    }
    else if (size == 0x400000)  // 4MB
    {
        u4DramSc = 0b101;
    }
    else if (size == 0x800000)  // 8MB
    {
        u4DramSc = 0b110;
    }
    else if (size == 0x1000000) // 16MB
    {
        u4DramSc = 0b111;
    }
    else
    {
        while (1);
    }

    //u4DramSc = 0b011;  // temporaly protect 1MB, cause bim alignment issue
    BIM_WRITE32(0x10, base);   //DRAM_SC_BASE
    BIM_WRITE32(0x8, BIM_READ32(0x8) | (u4DramSc << 5) | (1 << 4));  // SECURE_MAP
#endif
}

void BIM_Trustzone_VFIFO(UINT32 *pbase, UINT32 *psize)
{
    *(UINT32 *)pbase = PA_VFIFO_START;
#if  defined(CC_SUPPORT_4K2K) && defined(CC_SVP_SUPPORT)
    *(UINT32 *)psize=FBM_DMX_SIZE_TZ;
#elif defined(CC_SSIF_SUPPORT) && defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB)
    *(UINT32 *)psize = (PA_VFIFO_LEN - 0x500000) >> 1;
#elif defined(CC_SSIF_SUPPORT) && defined(CC_VOMX_TV_COEXIST)
    *(UINT32 *)psize = ((PA_VFIFO_LEN - 0x500000) >> 1) + 0x500000;
#elif defined(CC_FBM_TWO_FBP_SHARED_WITH_DFB) || defined(CC_VOMX_TV_COEXIST)
    *(UINT32 *)psize = PA_VFIFO_LEN >> 1;
#else
    *(UINT32 *)psize = PA_VFIFO_LEN;
#endif
}

void BIM_Trustzone_VFIFO2(UINT32 *pbase, UINT32 *psize)
{
    UINT32 base1, size1;
    BIM_Trustzone_VFIFO(&base1, &size1);

    *(UINT32 *)pbase = base1 + size1;
    *(UINT32 *)psize = PA_VFIFO_LEN - size1;
}

#if defined(CC_MT5890) || defined(CC_MT5861) || defined(CC_MT5882)
int get_core_index(void)
{
	unsigned int cpunum =0;
	__asm__("mrc p15,0,%0,c0,c0,5":"=r"(cpunum));
	cpunum &= 0x0F;
	Printf("now core index id %d\n",cpunum);
	return cpunum;
}
#endif
/* ----------------------------------------------------------------------------
 * This function is for mmu table in secure world, and dram protection setup in BIM.
 * ------------------------------------------------------------------------- */
void Boot_PAtoVA(int resume)
{
    // Init memory c-lib and protection (only for core0)
#ifdef CC_MT5881
    if (1) /* ARM11 without VFP/L2$ & it's single core */
#elif CC_MT5399
    if (L2C_READ32(0x2054) != 0xfff)
#elif defined(CC_MT5890) || defined(CC_MT5861) || defined(CC_MT5882)
	if(get_core_index() == 0)
#endif
    {
        // Printf("resume:%x\n", resume);
        if(!resume)
        {
            drv_s_dram_MemUtil_Init();
        }
        BIM_Trustzone_DRAM_Protection(PA_LOAD_BASE, TZ_DRAM_SC_SIZE);
    }

    // Init MMU
#ifdef CC_MT5881
    if(!resume)
    {
        HalInitMMU(PA_LOAD_BASE);
    }
    else /* resume, let's skip page table creation */
    {
    	int flag;
        /* set page table base */
        __asm__ ("MCR     p15, 0, %0, c2, c0, 0" : : "r" (PA_LOAD_BASE));
        /* enable domain 0 access */
        flag = 1;
        __asm__ ("MCR     p15, 0, %0, c3, c0, 0" : : "r" (flag));

        /* enable mmu/cache */
        HalEnableMMU();
        HalEnableCaches();
    }
#else
	if(!resume)
	{
    	HalInitMMU(PA_LOAD_BASE);
	}
	else
	{
		HalResumeMMU(PA_LOAD_BASE);
	}
    // NOTE: before HalInitMMU, need to set correct dram size in sram
#endif
}

/* ----------------------------------------------------------------------------
 * This function is for GIC (Generic Interrupt Controller) / SCU (Snoop Control Unit) setting.
 * ------------------------------------------------------------------------- */
void Boot_PostInit(void)
{
    UINT32 i;

    /* Invalidate TLB to ensure that old mappings are removed. */
    HalInvalidateTLB();

#ifndef CC_MT5881 /* ARM11 without VFP/L2$ */
#if !defined(CC_MT5890) && !defined(CC_MT5882) && !defined(CC_MT5861)
    /* CA9-MPCore private memory region. Refer to "IHI0407E_cortex_a9_mpcore_r2p0_trm.pdf" (1.3)
        The offset from PERIPHBASE[31:13] is 0x79001 -> 0xF2002000
        0x0000 - 0x00FC   SCU registers
        0x0100 - 0x01FF    Interrupt controller interfaces
        0x0200 - 0x02FF    Global timer
        0x0600 - 0x06FF    Private timers and watchdogs
        0x1000 - 0x1FFF    Interrupt Distribute
     */

    // CPU Interface Control Register (ICCICR), banked
    // Enable signalling of Secure/Non-secure interrupts
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf" (4.4.1)
    L2C_WRITE32(0x2100, 0x3);

    // Interrupt Priority Mask Register (ICCPMR), banked
    // If the priority of an interrupt is higher than the value indicated by this field, the interface signals the interrupt to the processor
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf"  (4.4.2)
    L2C_WRITE32(0x2104, 0xff);

    // Interrupt Security Registers (ICDISRn), banked
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf" (4.3.4)
    L2C_WRITE32(0x3080, 0xffffffff);

    // only for core0
    if (L2C_READ32(0x2054) != 0xfff)
    {
        // Interrupt Security Registers (ICDISRn), non-banked
        // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf" (4.3.4)
        L2C_WRITE32(0x3084, 0xffffffff);
        L2C_WRITE32(0x3088, 0xffffffff);
        L2C_WRITE32(0x308c, 0xffffffff);

        // SCU Non-secure Access Control Register (SNSAC), non-banked
        // component access, private timer, global timer
        // Refer to "IHI0407E_cortex_a9_mpcore_r2p0_trm.pdf" (2.2.8)
        L2C_WRITE32(0x2054, 0xfff);

        // SCU Invalidate All Registers in Secure State Register
        // Refer to "IHI0407E_cortex_a9_mpcore_r2p0_trm.pdf" (2.2.4)
        L2C_WRITE32(0x200c, 0xffff);
    }
#else  // CC_MT5890
    // CPU Interface Control Register (ICCICR), banked
    // Enable signalling of Secure/Non-secure interrupts
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf" (4.4.1)
    GICC_WRITE32(GIC_CPU_CTRL, 0x3);

    // Interrupt Priority Mask Register (ICCPMR), banked
    // If the priority of an interrupt is higher than the value indicated by this field, the interface signals the interrupt to the processor
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf"  (4.4.2)
    GICC_WRITE32(GIC_CPU_PRIMASK, 0xff);

    // Interrupt Security Registers (GICD_IGROUPRn), banked
    // Refer to "IHI0048A_gic_architecture_spec_v1_0.pdf" (4.3.4)
	for (i = 0; i < GIC_IRQS; i += 32)
	{
		GICD_WRITE32(GIC_DIST_IGROUPR + (i * 4 / 32), 0xffffffff);
	}
#endif // CC_MT5890
#endif
}

BOOL BIM_GetSecureUpgRsaKey(UINT8 *pu1RsaKey, UINT32 u4Size)
{
    if((pu1Key == NULL) || (u4Size < 256))
    {
        return FALSE;
    }

    memcpy(pu1RsaKey, pu1Key, 256);
}

BOOL BIM_GetSecureUpgAesKey(UINT8 *pu1AesKey, UINT32 u4KeySize, UINT8 *pau1AesIv, UINT32 u4IvSize)
{
    if((pu1Key == NULL) || (u4KeySize < 16) || (u4IvSize < 16))
    {
        return FALSE;
    }

    memcpy(pu1AesKey, pu1Key + 256, 16);
    memcpy(pau1AesIv, pu1Key + 256 + 16, 16);
}

/* ----------------------------------------------------------------------------
 *     The C entry point for the Secure World which is called after the boot
 *     code has initialized the MMU and the C library stack and heap.
 *     This function should never return.
 * ------------------------------------------------------------------------- */
int TZ_Main(int resume)
{
    // unlock VLD
    BIM_WRITE32(0x2c, BIM_READ32(0x2c) | 0x800000);

	// Enable SMC Accessible in normal world.
    BIM_WRITE32(0x2c, BIM_READ32(0x2c) | 0x10000000);

    Boot_PAtoVA(resume);

    /* Perform any platform-specific initialization that may be required. */
    Boot_PostInit();

    // Setup DLMv2 RSA & AES key from SRAM then clear SRAM
    memcpy(pu1Key, 0xfb005000, 256 + 32);
    memset(0xfb005000, 0, 256 + 32);

	return 0;
}

