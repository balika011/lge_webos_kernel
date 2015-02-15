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
 * $RCSfile: appinit.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file appinit.c
 *  BSP functions for initialization.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_linux.h"
#include "board.h"
#include "hal.h"
#include "x_assert.h"
#include "x_start_mini.h"
#include "x_mid.h"
#ifdef CC_FBM_MAPPING_ONE_BY_ONE
#include "fbm_drvif.h"
#include "srm_drvif.h"
#include <linux/list.h>
#include <linux/slab.h>
#include FBM_BOOT_MMAP_INCLUDE_FILE
#endif
#include <asm/mach/map.h>
#include <asm/cacheflush.h>
#include <linux/version.h>
#include <mach/mt53xx_linux.h>
#ifdef LINUX_TURNKEY_SOLUTION
#include <linux/module.h>
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MAX_HANDLE_NUM                  2048


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

UINT32 _ui4ResVirtStart;
UINT32 _ui4ResVirtEnd;

#ifdef CC_MAP_FBM_CHB
UINT32 _ui4Res2VirtStart;
UINT32 _ui4Res2VirtEnd;
#endif
#ifdef CC_MAP_FBM_CHC
UINT32 _ui4Res3VirtStart;
UINT32 _ui4Res3VirtEnd;
#endif
UINT32 _u4FbmPhyStart;         // FBM Physical Start address
UINT32 _u4FbmPhyEnd;           // FBM Physical End addr
#ifdef CC_MAP_FBM_CHB
UINT32 _u4Fbm2PhyStart;         // FBM in chB Physical Start address
UINT32 _u4Fbm2PhyEnd;           // FBM in chB Physical End addr
#endif
#ifdef CC_MAP_FBM_CHC
UINT32 _u4Fbm3PhyStart;         // FBM in chC Physical Start address
UINT32 _u4Fbm3PhyEnd;           // FBM in chC Physical End addr
#endif
UINT32 _u4MaxLowPhy;           // Max Direct mapped addr
UINT32 _u4TzPhyStart;          // Trustzone physical start address
UINT32 _u4TzPhyEnd;            // Trustzone physical end address
extern VOID x_sys_start(VOID* pv_mem_addr, SIZE_T z_mem_size, VOID* pv_arg);
extern void mt53xx_initCore1Stub(void);

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void* BSP_AllocateReservedAlign(UINT32 u4Size, UINT32 u4Align)
{
    UINT32 u4AlignSize, u4Start, u4AlignStart;

    u4AlignSize = (u4Size % u4Align) == 0 ?
                  u4Size :
                  ((u4Size / u4Align) + 1) * u4Align;

    u4Start = (UINT32)FBM_START_PHY_ADDR - u4AlignSize;

    u4AlignStart = (u4Start % u4Align) == 0 ?
                   u4Start :
                   ((u4Start / u4Align) + 1) * u4Align;

    printk(KERN_DEBUG "BSP_AllocateReservedAlign :\n"
           "u4Size=%d, u4Align=%d, u4AlignSize=%d,\n"
           "u4Start=0x%x, u4AlignStart=0x%x\n",
           (int)u4Size, (int)u4Align, (int)u4AlignSize,
           (unsigned int)u4Start, (unsigned int)u4AlignStart);

    return (void *)u4AlignStart;
}


#ifdef CC_FBM_MAPPING_ONE_BY_ONE

#if 0
#define FBM_RESERVED_MEM_DBG(fmt...) printk(fmt)
#else
#define FBM_RESERVED_MEM_DBG(fmt...)
#endif

#define CC_DYNAMIC_FBM_MAPPING_SUPPORT 1
    #if CC_DYNAMIC_FBM_MAPPING_SUPPORT
    static DEFINE_RWLOCK(res_mem_list_lock);
    //static DEFINE_SPINLOCK(res_mem_list_lock);
    #define FBM_RES_MEM_LIST_WRITE_LOCK(x, y)        write_lock_irqsave(x, y) //spin_lock_irqsave(x, y) //write_lock_irqsave(x, y)
    #define FBM_RES_MEM_LIST_WRITE_UNLOCK(x, y)      write_unlock_irqrestore(x, y) //spin_unlock_irqrestore(x, y) //write_unlock_irqrestore(x, y)
    #define FBM_RES_MEM_LIST_READ_LOCK(x, y)         read_lock_irqsave(x, y) //spin_lock_irqsave(x, y) //read_lock_irqsave(x, y)
    #define FBM_RES_MEM_LIST_READ_UNLOCK(x, y)       read_unlock_irqrestore(x, y) //spin_unlock_irqrestore(x, y) //read_unlock_irqrestore(x, y)
    #else
    #define FBM_RES_MEM_LIST_WRITE_LOCK(x, y)
    #define FBM_RES_MEM_LIST_WRITE_UNLOCK(x, y)
    #define FBM_RES_MEM_LIST_READ_LOCK(x, y)
    #define FBM_RES_MEM_LIST_READ_UNLOCK(x, y)
    #endif

static LIST_HEAD(res_mem_list);
static struct kmem_cache *reserved_mem_info_cachep __read_mostly;
struct RESERVED_MEM_INFO
{
    struct list_head list;
    UINT32 u4PhyAddr;
    UINT32 u4Size;
    UINT32 u4VirtAddr;
    UINT32 u4Caller;
} ;

static void _MapStaticFBM_Pools(void)
{
    FBM_POOL_T* prFbmPool;
    UINT32 u4Val;
#if 1
    UINT32 u4Virt;
    UINT32 u4Sum = 0;
#endif

    FBM_Init();

    printk("\n\n");
    if(sizeof(gStaticMapFBM_Pools))
    {
        for(u4Val = 0;u4Val < sizeof(gStaticMapFBM_Pools)/(sizeof(UINT8)); u4Val++)
        {
            prFbmPool = FBM_GetPoolInfo(gStaticMapFBM_Pools[u4Val]);
            ASSERT(prFbmPool != NULL);
            ASSERT(prFbmPool->u4Addr != (UINT32)NULL);
            //It's not necessary to implement error handling, since there is a fail checking inside it...
            if(prFbmPool->u4Size && (prFbmPool->u4Inherit == FBM_POOL_ROOT))
            {
                u4Virt = (UINT32)BSP_MapReservedMem((void*)(prFbmPool->u4Addr), prFbmPool->u4Size);
                printk("MMAP FBM_Pools Phy: 0x%08x, size: 0x%08x, Virt: 0x%08x %s\n", (prFbmPool->u4Addr), prFbmPool->u4Size, u4Virt, FBM_GetPoolName(gStaticMapFBM_Pools[u4Val]));
                u4Sum += prFbmPool->u4Size;
            }
/*
            else
            {
                FBM_RESERVED_MEM_DBG("_MapStaticFBM_Pools: BSP_MapReservedMem() index: %d skip!\n", u4Val);
            }
*/
        }
    }
    printk("_MapStaticFBM_Pools: total ioremap size: 0x%08x (%dM)\n", u4Sum, (u4Sum/1024)/1024);
    printk("\n\n");
}

static UINT32 _Remove_ReservedMem_List(void *pvVirt)
{
    struct RESERVED_MEM_INFO *res_mem_info, *next;
    UINT32 u4RemovedFailed = 1;
#ifdef CC_DYNAMIC_FBM_MAPPING_SUPPORT
    unsigned long flags;
#endif
    FBM_RES_MEM_LIST_WRITE_LOCK(&res_mem_list_lock, flags);
    list_for_each_entry_safe(res_mem_info, next, &res_mem_list, list)
    {
        if(res_mem_info->u4VirtAddr == (UINT32)pvVirt)
        {
            list_del(&(res_mem_info->list));
            kmem_cache_free(reserved_mem_info_cachep, res_mem_info);
            u4RemovedFailed = 0;
            break;
        }
    }
    FBM_RES_MEM_LIST_WRITE_UNLOCK(&res_mem_list_lock, flags);
    return u4RemovedFailed;
}

static UINT32 _Add_ReservedMem_List(void *pvVirt, void *pv_Phys, UINT32 z_Size, void *caller)
{
    struct RESERVED_MEM_INFO *res_mem_info;
#ifdef CC_DYNAMIC_FBM_MAPPING_SUPPORT
    unsigned long flags;
#endif
    res_mem_info = kmem_cache_zalloc(reserved_mem_info_cachep, GFP_KERNEL);
	if (unlikely(!res_mem_info))
    {
        printk("_Add_ReservedMem_List: kmem_cache_zalloc error\n");
        ASSERT(0);
		return 0;
    }

    res_mem_info->u4PhyAddr = (UINT32)pv_Phys;
    res_mem_info->u4Size = z_Size;
    res_mem_info->u4VirtAddr = (UINT32)pvVirt;
    res_mem_info->u4Caller = (UINT32)caller;
    FBM_RES_MEM_LIST_WRITE_LOCK(&res_mem_list_lock, flags);
    list_add_tail(&res_mem_info->list, &res_mem_list);
    FBM_RES_MEM_LIST_WRITE_UNLOCK(&res_mem_list_lock, flags);
    return 1;
}

void BSP_UnMapReservedMem(void *pv_Virt)
{
    iounmap(pv_Virt);
    if(_Remove_ReservedMem_List(pv_Virt))
    {
        printk("_Remove_ReservedMem_List fail, caller: %ps, virt_addr=0x%8x\n", __builtin_return_address(0), pv_Virt);
        ASSERT(0);
    }
}

static UINT32 __VIRTUAL__(UINT32 addr)
{
    struct RESERVED_MEM_INFO *res_mem_info;
#ifdef CC_DYNAMIC_FBM_MAPPING_SUPPORT
    unsigned long flags;
#endif
    FBM_RES_MEM_LIST_READ_LOCK(&res_mem_list_lock, flags);
    list_for_each_entry(res_mem_info, &res_mem_list, list)
    {
        if((addr >= res_mem_info->u4PhyAddr) && (addr <= (res_mem_info->u4PhyAddr + res_mem_info->u4Size)))
        {
            FBM_RES_MEM_LIST_READ_UNLOCK(&res_mem_list_lock, flags);
            return res_mem_info->u4VirtAddr + (addr - res_mem_info->u4PhyAddr);
        }
    }
    FBM_RES_MEM_LIST_READ_UNLOCK(&res_mem_list_lock, flags);
    return __VIRT_DEBUG_CHECK(addr);
}

static UINT32 __PHYSICAL__(UINT32 addr)
{
    struct RESERVED_MEM_INFO *res_mem_info;
#ifdef CC_DYNAMIC_FBM_MAPPING_SUPPORT
    unsigned long flags;
#endif
    FBM_RES_MEM_LIST_READ_LOCK(&res_mem_list_lock, flags);
    list_for_each_entry(res_mem_info, &res_mem_list, list)
    {
        if((addr >= res_mem_info->u4VirtAddr) && (addr <= (res_mem_info->u4VirtAddr + res_mem_info->u4Size)))
        {
            FBM_RES_MEM_LIST_READ_UNLOCK(&res_mem_list_lock, flags);
            return res_mem_info->u4PhyAddr + (addr - res_mem_info->u4VirtAddr);
        }
    }
    FBM_RES_MEM_LIST_READ_UNLOCK(&res_mem_list_lock, flags);
    return __PHY_DEBUG_CHECK(addr);
}
#endif
//-----------------------------------------------------------------------------
/** Map a physical memory of reserved region to virtual address.
 *  @param  pv_Phys     Physical memory to map.
 *  @param  z_Size      Size of the physical memory (bytes).
 *  @return If success, the mapped virtual address. \n
 *          Otherwise, NULL.
 */
//-----------------------------------------------------------------------------
void* BSP_MapReservedMem(void *pv_Phys, UINT32 z_Size)
{
    void *pvVirt = NULL;
#ifndef CC_FBM_MAPPING_ONE_BY_ONE
    unsigned long start, size;

    // Check reserved area.
    mt53xx_get_reserved_area(&start, &size);
    if (start && (unsigned long)pv_Phys >= start && ((unsigned long)pv_Phys + z_Size) <= (start + size))
    {
        // Great!! It is in reserved area and kernel already have a mapping,
        // just return va.
        return phys_to_virt((unsigned long)pv_Phys);
    }
#ifdef EXTRA_VMALLOC_START
    if (z_Size >= 0x1000000)
    {
        static UINT32 vmalloc_use = EXTRA_VMALLOC_START;

        if (z_Size <= EXTRA_VMALLC_END - vmalloc_use)
        {
            unsigned long va, pa, i;
            int err;
            const struct mem_type *mtype;

            mtype = get_mem_type(MT_DEVICE_CACHED);
            if (!mtype)
                goto err_out;

            pa = (unsigned long)pv_Phys & (~(PAGE_SIZE-1));
            pvVirt = (void*)(vmalloc_use + ((unsigned long)pv_Phys & (PAGE_SIZE-1)));
            va = vmalloc_use;
            vmalloc_use += (z_Size + PAGE_SIZE*2 - 1) & (PAGE_MASK);
            for (i=0; i<z_Size; i+= PAGE_SIZE)
            {
                err = ioremap_page(va, pa, mtype);
                if (err)
                    goto err_out;

                va += PAGE_SIZE;
                pa += PAGE_SIZE;
            }

            return pvVirt;
        }
    }
    err_out:
#endif /* EXTRA_VMALLOC_START */

#else /* CC_FBM_MAPPING_ONE_BY_ONE */
    struct RESERVED_MEM_INFO *res_mem_info;
#ifdef CC_DYNAMIC_FBM_MAPPING_SUPPORT
    unsigned long flags;
#endif
    ASSERT(reserved_mem_info_cachep != NULL);
    //Based on the application of BSP_MapReservedMem(), we can't map a range of physical address more than once.
    FBM_RES_MEM_LIST_READ_LOCK(&res_mem_list_lock, flags);
    list_for_each_entry(res_mem_info, &res_mem_list, list)
    {
        if((((UINT32)pv_Phys + z_Size) <= res_mem_info->u4PhyAddr) || ( (UINT32)pv_Phys >= (res_mem_info->u4PhyAddr + res_mem_info->u4Size)))
        {
        }
        else
        {
            printk("BSP_MapReservedMem fail!, overlap on caller: %ps, phy_addr=0x%08x, size=0x%08x, virt_addr=%8x\n", __builtin_return_address(0),  res_mem_info->u4PhyAddr, res_mem_info->u4Size, res_mem_info->u4VirtAddr);
            printk(" caller: %ps, phy_addr=0x%08x, size=0x%08x\n", __builtin_return_address(0),  pv_Phys, z_Size);
            ASSERT(0);
        }
    }
    FBM_RES_MEM_LIST_READ_UNLOCK(&res_mem_list_lock, flags);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
    pvVirt = ioremap_cached((int)pv_Phys, z_Size);
#else
    pvVirt = __ioremap((int)pv_Phys, z_Size, L_PTE_BUFFERABLE | L_PTE_CACHEABLE);
#endif

    ASSERT(pvVirt != NULL);

#ifdef CC_FBM_MAPPING_ONE_BY_ONE
//Save this information to implement VIRTUAL()/PHYSICAL()
    if(!_Add_ReservedMem_List(pvVirt, pv_Phys, z_Size, __builtin_return_address(0)))
    {
        printk("_Add_ReservedMem_List fail, caller: %ps, phy_addr=0x%08x, size=0x%08x, virt_addr=%8x\n", __builtin_return_address(0),  pv_Phys, z_Size, pvVirt);
        ASSERT(0);
    }
#endif
    return pvVirt;
}

#if defined(CC_TRUSTZONE_IN_CHB)
#include "x_bim.h"
#include "x_dram.h"
#endif

static void GetResVirtStart(void)
{
#ifdef CC_FBM_MAPPING_ONE_BY_ONE
	reserved_mem_info_cachep = kmem_cache_create("reserved_mem_info_cache",
					  sizeof(struct RESERVED_MEM_INFO),
					  0, 0, NULL);
	if (unlikely(!reserved_mem_info_cachep)) {
		ASSERT(0&&("GetResVirtStart: failed to create slab cache\n"));
	}
    _u4FbmPhyStart = FBM_START_PHY_ADDR;
    #if defined(CC_TRUSTZONE_IN_CHB)
    _u4FbmPhyEnd = TOTAL_DRAM_SIZE - TRUSTZONE_MEM_SIZE_CHA;
    #else
    _u4FbmPhyEnd = TOTAL_DRAM_SIZE - TRUSTZONE_SIZE;
    #endif

    _u4MaxLowPhy = __pa(high_memory-1);

	#if defined(CC_TRUSTZONE_IN_CHB)
    _u4TzPhyStart = TCMGET_CHANNELA_SIZE()*0x100000+TCMGET_CHANNELB_SIZE()*0x100000-TRUSTZONE_MEM_SIZE;
    _u4TzPhyEnd = TCMGET_CHANNELA_SIZE()*0x100000+TCMGET_CHANNELB_SIZE()*0x100000;
	#else
	_u4TzPhyStart = _u4FbmPhyEnd;
    _u4TzPhyEnd = TOTAL_DRAM_SIZE;
	#endif

	#ifndef CONFIG_TV_DRV_VFY
    _MapStaticFBM_Pools();
	#endif
#else /* CC_FBM_MAPPING_ONE_BY_ONE */

    _u4FbmPhyStart = FBM_START_PHY_ADDR;
     #if defined(CC_TRUSTZONE_IN_CHB)
    _u4FbmPhyEnd = TOTAL_DRAM_SIZE - TRUSTZONE_MEM_SIZE_CHA;
    #else
    _u4FbmPhyEnd = TOTAL_DRAM_SIZE - TRUSTZONE_SIZE;
    #endif

    _ui4ResVirtStart = (UINT32) BSP_MapReservedMem((void*) _u4FbmPhyStart, _u4FbmPhyEnd - _u4FbmPhyStart);
    _ui4ResVirtEnd = _ui4ResVirtStart + (_u4FbmPhyEnd - _u4FbmPhyStart);

#ifdef CC_MAP_FBM_CHB
    _u4Fbm2PhyStart = FBM2_START_PHY_ADDR;
    _u4Fbm2PhyEnd = FBM2_START_PHY_ADDR + FBM_CHB_SIZE;
    //_u4Fbm2PhyEnd = FBM2_START_PHY_ADDR + (256*1024*1024);
    _ui4Res2VirtStart = (UINT32) BSP_MapReservedMem((void*) _u4Fbm2PhyStart, _u4Fbm2PhyEnd - _u4Fbm2PhyStart);
    _ui4Res2VirtEnd = _ui4Res2VirtStart + (_u4Fbm2PhyEnd - _u4Fbm2PhyStart);
#endif

#ifdef CC_MAP_FBM_CHC
    _u4Fbm3PhyStart = FBM3_START_PHY_ADDR + FBM_MJC_RSV_POOL_SIZE ;
    _u4Fbm3PhyEnd = FBM3_START_PHY_ADDR + FBM_CHC_SIZE ;
    //_u4Fbm3PhyEnd = FBM3_START_PHY_ADDR + (256*1024*1024);
    _ui4Res3VirtStart = (UINT32) BSP_MapReservedMem((void*) _u4Fbm3PhyStart, _u4Fbm3PhyEnd - _u4Fbm3PhyStart);
    _ui4Res3VirtEnd = _ui4Res3VirtStart + (_u4Fbm3PhyEnd - _u4Fbm3PhyStart);
#endif

    _u4MaxLowPhy = __pa(high_memory-1);

#if defined(CC_TRUSTZONE_IN_CHB)
    _u4TzPhyStart = TCMGET_CHANNELA_SIZE()*0x100000+TCMGET_CHANNELB_SIZE()*0x100000-TRUSTZONE_MEM_SIZE;
    _u4TzPhyEnd = TCMGET_CHANNELA_SIZE()*0x100000+TCMGET_CHANNELB_SIZE()*0x100000;
#else
	_u4TzPhyStart = _u4FbmPhyEnd;
    _u4TzPhyEnd = TOTAL_DRAM_SIZE;
#endif


    printk("Mapped physical address 0x%x - 0x%x to virtual begins 0x%x\n",
           (unsigned int)_u4FbmPhyStart, (unsigned int)_u4FbmPhyEnd, (unsigned int)_ui4ResVirtStart);

#ifdef CC_MAP_FBM_CHB
    printk("Mapped FBM chB physical address 0x%x - 0x%x to virtual begins 0x%x\n",
           (unsigned int)_u4Fbm2PhyStart, (unsigned int)_u4Fbm2PhyEnd, (unsigned int)_ui4Res2VirtStart);
#endif
#ifdef CC_MAP_FBM_CHC
    printk("Mapped FBM chC physical address 0x%x - 0x%x to virtual begins 0x%x\n",
           (unsigned int)_u4Fbm3PhyStart, (unsigned int)_u4Fbm3PhyEnd, (unsigned int)_ui4Res3VirtStart);
#endif
#endif// CC_FBM_MAPPING_ONE_BY_ONE
}

// remap usage only
UINT32 PHYSICAL(UINT32 addr)
{
#ifdef CC_FBM_MAPPING_ONE_BY_ONE
    return __PHYSICAL__(addr);
#else
    ASSERT(_ui4ResVirtStart != 0);
    return __PHYSICAL(addr);
#endif
}

UINT32 VIRTUAL(UINT32 addr)
{
#ifdef CC_FBM_MAPPING_ONE_BY_ONE
    return __VIRTUAL__(addr);
#else
    ASSERT(_ui4ResVirtStart != 0);
    return __VIRTUAL(addr);
#endif

}
#ifdef LINUX_TURNKEY_SOLUTION
EXPORT_SYMBOL(VIRTUAL);
#endif

extern void HalInitBenchmark(void);
extern BOOL HAL_InitTimer(void);
extern void BSP_InitBoard(void);
extern INT32 mt53xx_get_rodata_addr(UINT32 *start, UINT32 *end);

VOID __init BspMainTask(void)
{
#ifdef LINUX_MID_PROTECT_AGENT
    UINT32 _u4MidRegion, _u4TopAddr;
#if defined(CC_MT5890)||defined(CC_MT5882)
    UINT32 _u4LowAddr;
#endif
#endif

#ifndef CC_FBM_MAPPING_ONE_BY_ONE
    GetResVirtStart();
#endif
    HalInitBenchmark();
    VERIFY(HAL_InitTimer());
#ifdef CC_FBM_MAPPING_ONE_BY_ONE
    GetResVirtStart();
#endif
    // Initialize OSAI and handle libraries
    VERIFY(x_start_mini(NULL, MAX_HANDLE_NUM, NULL, 0) == INITR_OK);
#ifndef STANDALONE_CLI
    // Init cli if diag is builtin with driver.
    x_sys_start(NULL, 0, NULL);
#endif
    BSP_InitBoard();

#ifdef LINUX_MID_PROTECT_AGENT
    // Init MID protect for all kernel DRAM for said agent.
    // LINUX_MID_PROTECT_AGENT is set in IC board.h, ex. mt5396/5396_driver/bsp/board.h
    if (!MID_Init())
    {
        return;
    }

    // Allocate a read-write region
    if (!MID_AllocateFreeRegion(FALSE, &_u4MidRegion))
    {
        return;
    }

    // Set all kernel to read-write
#ifndef CONFIG_TV_DRV_VFY
    _u4TopAddr = _u4FbmPhyStart - DRVCUST_OptGet(eDirectFBMemSize) - DRVCUST_OptGet(eFBMemSize);
#else
    _u4TopAddr = 0;
#endif

    if (!MID_SetRegionEx2(_u4MidRegion, MID_AGENT_CPU, 0, _u4TopAddr,
               MID_FLAG_CPUACCESS, LINUX_MID_PROTECT_AGENT))
    {
        return;
    }

    if (!MID_EnableRegionProtect(_u4MidRegion))
    {
        return;
    }
    printk("Enable MID on %08x to %08x\n", 0, _u4TopAddr);


#if defined(CC_MT5890)||defined(CC_MT5882)
    // Allocate a read-only region
    if (!MID_AllocateFreeRegion(FALSE, &_u4MidRegion))
    {
        return;
    }

    // Set kernel text to read-only
    mt53xx_get_rodata_addr(&_u4LowAddr, &_u4TopAddr);
    _u4TopAddr = __pa(_u4TopAddr);
    _u4LowAddr = __pa(_u4LowAddr);
    if (!MID_SetRegionEx(_u4MidRegion, MID_AGENT_NONE, _u4LowAddr, _u4TopAddr, MID_FLAG_PROTWR))
    {
        return;
    }

    if (!MID_EnableRegionProtect(_u4MidRegion))
    {
        return;
    }
    printk("Enable RO MID on %08x to %08x\n", _u4LowAddr, _u4TopAddr);

    // initial hotplug corestub
    printk("Initialize hotplug corestub\n");
    mt53xx_initCore1Stub();
#endif
#endif /* LINUX_MID_PROTECT_AGENT */
}


//-----------------------------------------------------------------------------
/** Dump system task state.
 */
//-----------------------------------------------------------------------------
extern void dump_system_info(void);
void BspDumpSystemTasksState(void)
{
    dump_system_info();
}
