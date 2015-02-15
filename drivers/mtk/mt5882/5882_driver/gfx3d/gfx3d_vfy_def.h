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

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define AXI_BRDG_WR_OUTSTAND_TEST         0
#define GP_WATCH_DOG_DISABLE              1
#define PP_WATCH_DOG_DISABLE              1
#define COMPARE_ALL_WRITTEN_MEM           1
#define COMPARE_GP_STATUS                 0
#define COMPARE_PP_STATUS                 0
#define AXI_STOP_TEST                     0
#define WATCH_PERF_COUNT                  0 

#define G3D_SUPPORT_INTERNAL_MMU          1
#define G3D_SUPPORT_MP                    1
#define G3D_SUPPORT_MALIMP4               0 /* for support mali450 MP4 cores */
#define G3D_VFY_ENV                       1 /* 0 means EMU ENV */

#define MAX_READ_OUT_STAND    16
#define MAX_WRITE_OUT_STAND   8
#define MAX_REG_REC_CNT       8
#define MAX_REG_REC_IDX       70


#define  G3D_BASE_ADR_IDX    0
#define  G3D_LIST_ADR_IDX    1
#define  G3D_CONF_ADR_IDX    2
#define  G3D_MAKE_ADR_IDX    3
#define  G3D_GPTB_ADR_IDX    4
#define  G3D_PPTB_ADR_IDX    5
#define  G3D_HEXF_ADR_IDX    6
#define  G3D_GPPH_ADR_IDX    7
#define  G3D_PPPH_ADR_IDX    8

#define BIM_REG_OFFSET        0x8000
#define MALI_TOP_OFFSET       0xb0000
#define MALI_REG_OFFSET       0xc0000

#define MALI_REG_GP_OFFSET    0x0
#define MALI_REG_PP_OFFSET    0x8000
#define MALI_REG_AXI_OFFSET   0xE000

#define HEX_DEFAULT      0
#define HEX_LOAD         1
#define HEX_COMP_REG     2
#define HEX_COMP_MEM     3


#define PP0_IRQ_EN                   (0x1 << 0)
#define PP1_IRQ_EN                   (0x1 << 1)
#define PP2_IRQ_EN                   (0x1 << 2)
#define PP3_IRQ_EN                   (0x1 << 3)
#define PP0MMU_IRQ_EN                (0x1 << 25)
#define PP1MMU_IRQ_EN                (0x1 << 26)
#define PP2MMU_IRQ_EN                (0x1 << 27)
#define PP3MMU_IRQ_EN                (0x1 << 28)
#define GP_IRQ_EN                    (0x1 << 16)
#define GPMMU_IRQ_EN                 (0x1 << 24)
#define PP_BDC_IRQ_EN                (0x1 << 15)
#define PMU_IRQ_EN                   (0x1 << 17)

#define GP_CONTR_REG_INIT_RAWSTAT       0x24
#define GP_IRQ_PTR_OUT_OF_BOUNDS       (0x1 << 22)
#define GP_IRQ_SEMAPHORE_OVERFLOW      (0x1 << 21)
#define GP_IRQ_SEMAPHORE_UNDERFLOW     (0x1 << 20)
#define GP_IRQ_RESET_COMPLETE          (0x1 << 19)
#define GP_IRQ_PLB_INVALID_CMD         (0x1 << 14)

#define MMU_UNIT 0x400000

/* IRQ define */
#if !G3D_SUPPORT_MALIMP4
/* MaliMP2*/
	#define MALI_PP0_IRQ      164//100
	#define MALI_PP1_IRQ      165//101
	#define MALI_PP2_IRQ      166//102
	#define MALI_PP3_IRQ      167//103	
	#define MALI_PP_ALL 	  168//104	
	#define MALI_GP_IRQ       169//105
	#define MALI_GP_MMU_IRQ   170//106
	#define MALI_PP0_MMU_IRQ  171//107
	#define MALI_PP1_MMU_IRQ  172//108
	#define MALI_PP2_MMU_IRQ  173//109
	#define MALI_PP3_MMU_IRQ  174//110
#else
	#define MALI_PP0_IRQ      132//100
	#define MALI_PP1_IRQ      133//101
	#define MALI_PP2_IRQ      134//102
	#define MALI_PP3_IRQ      135//103	
	#define MALI_PP_ALL 	  136//104	
	#define MALI_GP_IRQ       137//105
	#define MALI_GP_MMU_IRQ   138//106
	#define MALI_PP0_MMU_IRQ  139//107
	#define MALI_PP1_MMU_IRQ  140//108
	#define MALI_PP2_MMU_IRQ  141//109
	#define MALI_PP3_MMU_IRQ  142//110
#endif



typedef unsigned long DWRD;

#define DRV_FS_RDONLY   0
#define DRV_FS_W_C      1
#define DRV_FS_RW_C     2

#define DRV_FSR_SUCCESS                0
#define DRV_FSR_FAIL                  -1
#define DRV_FSR_NULL_POINT            -2
#define DRV_FSR_BUF_ADDR_ALIGN_ERR    -3
#define DRV_FSR_ONLY_SUPPORT_RDONLY   -4
#define DRV_FSR_PARAMETER_ERR         -5

/* Mali450 Reg base */
#define MALI400_REG_BASE_GP         (0x0000)
#define MALI400_REG_BASE_L2         (0x1000)
#define MALI400_REG_BASE_PMU        (0x2000)
#define MALI400_REG_BASE_MMU_GP     (0x3000)
#define MALI400_REG_BASE_MMU_PP0    (0x4000)
#define MALI400_REG_BASE_MMU_PP1    (0x5000)
#define MALI400_REG_BASE_MMU_PP2    (0x6000)
#define MALI400_REG_BASE_MMU_PP3    (0x7000)
#define MALI400_REG_BASE_PP0        (0x8000)
#define MALI400_REG_BASE_PP1        (0xA000)
#define MALI400_REG_BASE_PP2        (0xC000)
#define MALI400_REG_BASE_PP3        (0xE000)

#define vWriteReg(dAddr, dVal)  *(volatile UINT32 *)(IO_BASE + dAddr) = dVal
#define u4ReadReg(dAddr)         *(volatile UINT32 *)(IO_BASE + dAddr)

#define vWriteMem(dAddr, dVal)  *(volatile UINT32 *)((VIRTUAL)(dAddr))= dVal
#define u4ReadMem(dAddr)        *(volatile UINT32 *)((VIRTUAL)(dAddr))


#endif // __DRV_GFX3D_VFY_DEF_H






















