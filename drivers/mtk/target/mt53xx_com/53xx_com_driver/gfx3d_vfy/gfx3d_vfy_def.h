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

#ifndef __DRV_GFX3D_VFY_DEF_H
#define __DRV_GFX3D_VFY_DEF_H

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
//#include "x_graphics3d.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
//#define STATIC_MEMORY_DISPATCH                1
#define MEMORY_256_DISPATCH               1
#define MEMORY_256_LARGE_MC               1
#define GP_LARGE_MEMORY                   0
#define AXI_BRDG_WR_OUTSTAND_TEST         0
#define GP_WATCH_DOG_DISABLE              1
#define PP_WATCH_DOG_DISABLE              1
#define COMPARE_ALL_WRITTEN_MEM           1
#define COMPARE_GP_STATUS                 0
#define COMPARE_PP_STATUS                 0
#define AXI_STOP_TEST                     0
#define WATCH_PERF_COUNT                  0 // 1
#define USE_BIM_IRQ                       0 //  1
#define INTEGRATED_MALI_RESET             1
#define G3D_DRAM_BUSY_TEST                0


#define G3D_SUPPORT_INTERNAL_MMU          1
#define G3D_SUPPORT_MAPUNMAP              0
#define G3D_SUPPORT_PP1                   1
#define SMALL_MEM_SIZE                    1  //only test alloc 2M memory


#define MAX_READ_OUT_STAND    16
#define MAX_WRITE_OUT_STAND   8
#define MAX_REG_REC_CNT       8
#define MAX_REG_REC_IDX       70


#define  G3D_BASE_ADR_IDX    0
#define  G3D_LIST_ADR_IDX    1
#define  G3D_CONF_ADR_IDX    2
#define  G3D_MAKE_ADR_IDX    3
#define  G3D_DRAM_ADR_IDX    4
#define  G3D_GPTB_ADR_IDX    5
#define  G3D_PPTB_ADR_IDX    6
#define  G3D_HEXF_ADR_IDX    7
#define  G3D_GPPH_ADR_IDX    8
#define  G3D_PPPH_ADR_IDX    9
//#define  G3D_MAKE_ADR_EX_IDX    10



/* The possibilities for the third argument to `fseek'.
 These values should not be changed.  */
#define SEEK_SET    0   /* Seek from beginning of file.  */
#define SEEK_CUR    1   /* Seek from current position.  */
#define SEEK_END    2   /* Seek from end of file.  */

#define BIM_REG_OFFSET        0x8000
#define BIM_MALI_IRQ_30       0x30
#define BIM_MALI_GPMMU_IRQ    (0x1 << 2)
#define BIM_MALI_PP_IRQ       (0x1 << 23)
#define BIM_MALI_GP_IRQ       (0x1 << 11)

#define BIM_MALI_IRQ_48       0x48
#define BIM_MALI_PMU_IRQ      (0x1 << 22)
#define BIM_MALI_PPMMU_IRQ    (0x1 << 23)    

#define BIM_MALI_IRQ_CLR_38       0x38
#define BIM_MALI_GPMMU_IRQ_CLR   (0x1 << 2)
#define BIM_MALI_PP_IRQ_CLR      (0x1 << 23)
#define BIM_MALI_GP_IRQ_CLR      (0x1 << 11)
#define BIM_MALI_IRQ_CLR_50       0x50
#define BIM_MALI_PMU_IRQ_CLR     (0x1 << 22)
#define BIM_MALI_PPMMU_IRQ_CLR   (0x1 << 23)    

#define MALI_REG_OFFSET   0x40000

#define MALI_REG_GP_OFFSET   0x0
#define MALI_REG_PP_OFFSET   0x8000
#define MALI_REG_AXI_OFFSET 0xE000

#define HEX_DEFAULT      0
#define HEX_LOAD         1
#define HEX_COMP_REG     2
#define HEX_COMP_MEM     3

#define PP_IRQ_EN                   0x1
#define GP_IRQ_EN                  (0x1 << 16)
#define PMU_IRQ_EN                 (0x1 << 17)
#define GPMMU_IRQ_EN               (0x1 << 24)
#define PPMMU_IRQ_EN               (0x1 << 25)

#define GP_CONTR_REG_INIT_RAWSTAT       0x24
#define GP_IRQ_PTR_OUT_OF_BOUNDS       (0x1 << 22)
#define GP_IRQ_SEMAPHORE_OVERFLOW      (0x1 << 21)
#define GP_IRQ_SEMAPHORE_UNDERFLOW     (0x1 << 20)
#define GP_IRQ_RESET_COMPLETE          (0x1 << 19)
#define GP_IRQ_PLB_INVALID_CMD         (0x1 << 14)


#define PYSICAL_CONTINOUS_MEM  1
#define MMU_UNIT 0x400000
//#define MMU_UNIT 0x040000

typedef unsigned long DWRD;

#define DRV_FS_RDONLY 0
#define DRV_FS_W_C 1
#define DRV_FS_RW_C 2

#define MAX_FILENAME_SZ 50
#define MAX_READ_BYTES  32*1024

#define DRV_FSR_SUCCESS 0
#define DRV_FSR_FAIL    -1
#define DRV_FSR_NULL_POINT            -2
#define DRV_FSR_BUF_ADDR_ALIGN_ERR    -3
#define DRV_FSR_ONLY_SUPPORT_RDONLY   -4
#define DRV_FSR_PARAMETER_ERR         -5


#define vWriteReg(dAddr, dVal)  *(volatile UINT32 *)(IO_BASE + dAddr) = dVal
#define u4ReadReg(dAddr)         *(volatile UINT32 *)(IO_BASE + dAddr)

//#define vWriteMem(dAddr, dVal)  *(volatile UINT32 *)(DRAM_A_BASE + dAddr) = dVal
//#define u4ReadMem(dAddr)         *(volatile UINT32 *)(DRAM_A_BASE + dAddr)
#define DRAM_A_BASE_EX  0xC0000000
#define vWriteMem(dAddr, dVal)  *(volatile UINT32 *)(DRAM_A_BASE_EX + dAddr) = dVal
#define u4ReadMem(dAddr)        *(volatile UINT32 *)(DRAM_A_BASE_EX + dAddr)


#define x_alloc_aligned_dma_mem(size,align) VIRTUAL(BSP_AllocAlignedDmaMemory(size,align))
#define x_free_aligned_dma_mem(addr)  BSP_FreeAlignedDmaMemory(PHYSICAL(addr))

#endif // __DRV_GFX3D_VFY_DEF_H






















