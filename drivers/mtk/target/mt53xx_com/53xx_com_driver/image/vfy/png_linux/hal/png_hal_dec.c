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
#include "drv_config.h"
#include "png_hal_dec.h"
#include "png_hal_priv.h"
#include "png_hal_reg.h"
#include "x_ckgen.h"
#include "x_timer.h"
#include <asm/system.h>
#include "x_hal_5381.h"
#include "x_mem_phase2.h"
#include "png_debug.h"
#include "x_iommu.h"
//#include "drv_img_dec.h"   ////
#include "x_drv_map.h"
#include <linux/sched.h>

#define PNG_CLK_OFFSET 0x3D0
#if 0
#define PNG_VMEM_MAP(a, b ,c) BSP_dma_map_vaddr(a, b, c) 
#define PNG_VMEM_UNMAP(a, b ,c) BSP_dma_unmap_vaddr(a, b, c) 
#else 
#include "x_hal_arm.h"

UINT32 PNG_VMEM_MAP(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode)
{
   if(i4Start & (64 -1))
   {
   	//UTIL_Printf("<MAP> sa 0x%x len 0x%x Mode %d \n", i4Start, u4Len , i4DirMode);
   	i4Start = i4Start/64*64;
        u4Len = (u4Len+64+63)/64*64;
	UTIL_Printf("<MAP> sa 0x%x len 0x%x Mode %d \n", i4Start, u4Len , i4DirMode);
   }

   switch(i4DirMode)
   {
   	case TO_DEVICE:
		HalFlushDCacheMultipleLine(i4Start, u4Len);
		break;
	case FROM_DEVICE:
		HalInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	case BIDIRECTIONAL:
		HalFlushInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	default:
		break;
   }
}
UINT32 PNG_VMEM_UNMAP(UINT32 i4Start, UINT32 u4Len, UINT32 i4DirMode)
{
   if(i4Start & (64 -1))
   {
   	//UTIL_Printf("<UNMAP> sa 0x%x len 0x%x Mode %d \n", i4Start, u4Len , i4DirMode);
   	i4Start = i4Start/64*64;
        u4Len = (u4Len+64+63)/64*64;
	UTIL_Printf("<UNMAP> sa 0x%x len 0x%x Mode %d \n", i4Start, u4Len , i4DirMode);
   }


   switch(i4DirMode)
   {
   	case TO_DEVICE:
		break;
	case FROM_DEVICE:
	case BIDIRECTIONAL:
		HalInvalidateDCacheMultipleLine(i4Start, u4Len);
		break;
	default:
		break;
   }
}
#endif
UINT32  PNG_CLOCK_SETTING= 0x5;
#define PNG_MMU_Performance 0
#if (PNG_MMU_Performance)
HAL_TIME_T sPNG_Start_Time;
HAL_TIME_T sPNG_End_Time;
HAL_TIME_T sPNG_Rst_Time;
#endif
extern UINT32 _u4RegBuf;
extern UINT32 u4ResizeAndCropTimes;
extern UINT32 au4RegBBuf[2];
extern UINT32 au4RegABuf[2];
extern UINT32 u4CntDecode;
extern UINT32 DebugReg[100];
UINT32 _u4_png_debug_loglevel =3;


#define PngWriteREG(base, arg, val)   IO_WRITE32(IMAGE_BASE, ((base + arg) & 0xFFFFF), val)
#define PngReadREG(base, arg)         IO_READ32(IMAGE_BASE, ((base + arg) & 0xFFFFF))

#define PngWriteWTREG(arg, val)   IO_WRITE32(IMAGE2_BASE, (arg & 0xFFFFF), val)
#define PngReadWTREG(arg)         IO_READ32(IMAGE2_BASE, (arg & 0xFFFFF))
#if PNG_MMU_TEST
#define IO_MMU_BASE                     0xF0068000
#define PNG_MMU_BASE                    0x180
void vPrintfMMUStatus(void);
#define IOMMU_READ32(base, arg)         IO_READ32(IO_MMU_BASE, (base + arg))
#define IOMMU_WRITE32(base, arg, val)   IO_WRITE32(IO_MMU_BASE, (base + arg), val)
#endif
#define PNG_SUPPORT_POWEROFF 0
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
BOOL _fgPngHalInitIrq = FALSE;
PNG_HAL_INST_T  _tPngHalHwInst[PNG_HAL_HW_INST_NUM];
static HANDLE_T     _h_Png_Hal_Sema = NULL_HANDLE;
static HANDLE_T     _h_Png_CkgenClk_Sema = NULL_HANDLE;
UINT32 _u4RefCount = 0;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
UINT32 _u4PngCheckSum  = 0;
#endif 
#if (PNG_MMU_TEST)
void vPrintfMMUStatus(void)
{
     UINT32 u4RegVal = 0;
     UTIL_Printf("[PNG_VFY] vPrintfMMUStatus is 0x%x+0x%x . \n",
     		IO_MMU_BASE, PNG_MMU_BASE);
     //vIIOMMU_Init();
    // IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_GIF, 0);
     //u4RegVal = 0x1E|(0x1<<0);
     //IOMMU_WRITE32(REG_RW_IOMMU_CFG0, IOMMU_PNG, u4RegVal);
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG0, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
     //IOMMU_WRITE32(REG_RW_IOMMU_CFG1, IOMMU_PNG, u4HalGetTTB1());
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
     //IOMMU_WRITE32(REG_RW_IOMMU_CFG2, IOMMU_PNG, 0x1B0019);
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG2, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU configvalue is %x . \n",u4RegVal);
}

static BOOL fgPNGIOMMUISRInited = FALSE;
void PNG_IOMMU_IrqHandle(UINT16 u2Vector)
{
    if (fgPNGIOMMUISRInited == 0)
    {
        //return;
        UTIL_Printf("RZ IOMMU fgIOMMUISRInited=FALSE \n");
        ASSERT(0);
    }

    if (u2Vector != VECTOR_MMU_PNG1)
    {
      	 UTIL_Printf("PNG IOMMU u2Vector false \n");
        ASSERT(0);
    }

    UTIL_Printf("PNG IOMMU page fault occurs\n");


    //clear BIM status bit
    VERIFY(BIM_ClearIrq(u2Vector));

    {
          UINT32 ui4_tmp0 = 0, ui4_tmp1 = 0;

          
        #if 0
                    UTIL_Printf("debug PNG IOMMU\n");
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30010A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);          
                    UTIL_Printf("[1]CFG4= 0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          
                   IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30090A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);          
                    UTIL_Printf("[2]CFG4= 0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30080A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);          
                    UTIL_Printf("[3]CFG4= 0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
                    
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x300A0A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);                 
                    UTIL_Printf("CFG4= 0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
                    
          
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
                    UTIL_Printf("[4]CFG1= 0x%08X\n", ui4_tmp0);
          
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30010A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[6]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
                    
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30110A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[7]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
          
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30210A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[8]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
          
                    
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30910A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[9]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
          
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30A10A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[10]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
          
                    
                    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x30B10A);
                    ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
                    ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG6, PNG_MMU_BASE);          
                    UTIL_Printf("[11]CFG4= 0x%08X, CFG6=0x%08X, CFG7=0x%08X, CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1,  IOMMU_READ32 (REG_RW_IOMMU_CFG7, PNG_MMU_BASE),  IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE));
          #endif
            /*pa*/
          ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x8<<8) ;
	  IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, ui4_tmp0);
          ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);
          UTIL_Printf("CFG4= 0x%08X,pa,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);
          /*va*/
          ui4_tmp0 = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);
          ui4_tmp0 &= ~(0xF<<8) ;
          ui4_tmp0 |= (0x9<<8) ;
          IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, ui4_tmp0);
          ui4_tmp1 = IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE);
          UTIL_Printf("CFG4= 0x%08X,va,CFG8=0x%08X\n", ui4_tmp0, ui4_tmp1);

	#if 1
          {
		  UINT32 i4pgtBase = 0;
		  UINT32 i4PmdSA = 0, i4Pmd = 0;
		  UINT32 i4PteSA, i4Pte;
		  i4pgtBase = IOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
		  i4PmdSA = i4pgtBase + ((ui4_tmp1>>20)*4);

		  UTIL_Printf("CFG1= 0x%08X,va,pmdSA=0x%08X, va 0x%x \n",
		  	i4pgtBase, i4PmdSA, VIRTUAL(i4PmdSA));

		  i4PmdSA = VIRTUAL(i4PmdSA);
		  i4Pmd = HAL_READ32(i4PmdSA);
		  i4Pmd = i4Pmd & 0xfffffff0;

                  UTIL_Printf("pmd=0x%08X,pmd 0x%x 0x%x 0x%x \n",
 		 	i4PmdSA, i4Pmd, HAL_READ32(i4PmdSA-4), HAL_READ32(i4PmdSA+4));

		  if(i4Pmd)
		  {
		  	i4PteSA = i4Pmd + (((ui4_tmp1>>12)&0xff)*4);
			//UTIL_Printf("pteSA 0x%x 0x%x\n", i4PteSA, VIRTUAL(i4PteSA));

			if(i4PteSA)
			{
				i4PteSA = VIRTUAL(i4PteSA);
				i4Pte= HAL_READ32(i4PteSA);
				UTIL_Printf("pteSA 0x%x pte 0x%x 0x%x 0x%x\n",
					i4PteSA, i4Pte, HAL_READ32(i4PteSA-4), HAL_READ32(i4PteSA+4));

			}
		  }
          }
	  #endif

         PNGASSERT(0);
         VERIFY(0);
     }

}
void vPngMMUInit(void)
{
     UINT32 u4RegVal = 0;   
     
     //printk(KERN_INFO "The Processs is \"%s\" (pid %i)\n", current->comm, current->pid);
     u4RegVal = 0xFE|(0x1<<0);
     IOMMU_WRITE32(REG_RW_IOMMU_CFG0, PNG_MMU_BASE, u4RegVal);
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG0, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU REG_RW_IOMMU_CFG0 config value is %x . \n",u4RegVal);
     IOMMU_WRITE32(REG_RW_IOMMU_CFG1, PNG_MMU_BASE, u4HalGetTTB1());//HalGetMMUTableAddress());
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG1, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU REG_RW_IOMMU_CFG1 config value is %x==0x%x 0x%x. \n",
     	u4RegVal, u4HalGetTTB1(),HalGetMMUTableAddress());

     IOMMU_WRITE32(REG_RW_IOMMU_CFG2, PNG_MMU_BASE, 0x1B0019);//0x1B0019
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG2, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU REG_RW_IOMMU_CFG2 config value is %x . \n",u4RegVal);

     IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, 0x030010A);
     u4RegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG3, PNG_MMU_BASE);
     UTIL_Printf("[PNG_VFY] MMU  REG_RW_IOMMU_CFG3 config value is %x . \n",u4RegVal);

     if (fgPNGIOMMUISRInited == 0)
    {
    	 x_os_isr_fct pfnOldIsr;
        fgPNGIOMMUISRInited = 1;
        //i4GCPU_HAL_ClrIntr(GCPU_INT_MASK);
        BIM_ClearIrq(VECTOR_MMU_PNG1);

        if (x_reg_isr(VECTOR_MMU_PNG1, PNG_IOMMU_IrqHandle, &pfnOldIsr) != OSR_OK)
        {
		UTIL_Printf("PNG IOMMU x_reg_isr create error \n");
		ASSERT(0);
        }
    }
 }


void vPngMMUFlush(void)
{
	UINT32 u4PngRegVal;                

	u4PngRegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE);	//
     IOMMU_WRITE32(REG_RW_IOMMU_CFG4, PNG_MMU_BASE, u4PngRegVal|0x80000000);////////8, for self fire
   #if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
        while (IOMMU_READ32 (REG_RW_IOMMU_CFG8, PNG_MMU_BASE)&0x20000000);
   #else
	while (IOMMU_READ32 (REG_RW_IOMMU_CFG4, PNG_MMU_BASE)&0x10000000);
    #endif 	
}

void vCheckAddress(UINT32 u4Addr)
{
    if(!(u4Addr & (~0x3FFFFFFF)))
    {
        UTIL_Printf("!!!Address %x Fail Before Programming!!!\n",u4Addr);
        ASSERT(0);
    }
}
#if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
void PNG_MMU_ENABLE(void)
{
        UINT32 u4Val =0;
	u4Val = PngReadREG(0x6E000, 0x04);

	PngWriteREG(0x6E000,0x04,u4Val|2); 		///FOR PNG to MMU enable
 
}
#endif
#endif
//DEFINE_CHANNEL2_MEMORY_AREA(_u4PngLz77Buf, BYTE, PNG_LZ77_SIZE, 128)

//extern void vInflateHalGetIRQ(UINT16 u2Vector);
extern void vPngSetClockOn(void)
{
     x_sema_lock(_h_Png_CkgenClk_Sema, X_SEMA_OPTION_WAIT);
     IO_WRITE32(CKGEN_BASE, PNG_CLK_OFFSET, PNG_CLOCK_SETTING);
    UTIL_Printf(" %s PNG clock ============= 0x%x ==============\n", __FUNCTION__,PNG_CLOCK_SETTING);
    x_sema_unlock(_h_Png_CkgenClk_Sema);
}

extern void vPngSetClockOff(void)
{
	#if 0
    x_sema_lock(_h_Png_CkgenClk_Sema, X_SEMA_OPTION_WAIT);
    if(0 == _u4RefCount)
    {
      vPngLog_H("[Png] vPngSetClockOff error, _u4RefCount < 0. \n");
    }
    _u4RefCount--;
    if(0 == _u4RefCount)
    {
      #if (!CONFIG_DRV_FPGA_BOARD)
       #if (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8520)
      // CKGEN_AgtOffClk(e_CLK_PNG);
       #endif
      #endif //#if (!CONFIG_DRV_FPGA_BOARD)
    }
    
    x_sema_unlock(_h_Png_CkgenClk_Sema);
    #endif
}
void vPngPngHalSetRingFifo(UINT32 u4HwInstId, UINT32 u4FifoStart, UINT32 u4FifoEnd)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    vPngLog_M("[PNG] Enter %s RingFifo[0x%x~ 0x%x]\n",__FUNCTION__,u4FifoStart, u4FifoEnd);	
    ptPngHwInst->u4RingFifoEnd = u4FifoEnd;
     ptPngHwInst->u4RingFifoStart = u4FifoStart;		
#endif 	 
}

void vPngPngHalUseTmpDstBufer(UINT32 u4HwInstId, BOOL fgTmpUsed)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    vPngLog_M("[PNG] Enter %s fgTmpUsed[0x%x]\n",__FUNCTION__,fgTmpUsed);	
    ptPngHwInst->fgUseClrBuf= fgTmpUsed;
#endif 	
}

void vPngHalMapSrcAddress(UINT32 u4HwInstId, UINT32 u4MemStart, UINT32 u4MemLen, UINT32 u4Option, BOOL fgNoRingFifo)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    vPngLog_M("[PNG] RingFifo[0x%x~ 0x%x]\n",ptPngHwInst->u4RingFifoStart, ptPngHwInst->u4RingFifoEnd);	
    if(!fgNoRingFifo  
	&& ((u4MemStart+  u4MemLen) > ptPngHwInst->u4RingFifoEnd)
     )
    {
          UINT32 u4Len1=0;
	   u4Len1 = ptPngHwInst->u4RingFifoEnd-u4MemStart;
	   //vPngLog_M("[PNG-Src ]Map[0x%x  length0x%x]\n",u4MemStart, u4Len1);
	   PNG_VMEM_MAP( u4MemStart, u4Len1, u4Option);
	   vPngLog_M("[PNG-Src ]Map[0x%x  length0x%x]\n", ptPngHwInst->u4RingFifoStart,u4MemLen-u4Len1);	
	   PNG_VMEM_MAP( ptPngHwInst->u4RingFifoStart, u4MemLen-u4Len1, u4Option);
    }
    else
    {	   
           vPngLog_M("[PNG-Src ]Map[0x%x  length0x%x]\n", u4MemStart,u4MemLen);	
 	     PNG_VMEM_MAP( u4MemStart, u4MemLen, u4Option);	 
    }
    ptPngHwInst->rSrc.u4MemAddress = u4MemStart;
    ptPngHwInst->rSrc.u4MemLen= u4MemLen;
    ptPngHwInst->rSrc.u4Option= u4Option;
    ptPngHwInst->rSrc.fgNoRingFifo= fgNoRingFifo;	
#endif
}

void vPngHalunMapSrcAddress(UINT32 u4HwInstId)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if(ptPngHwInst->rSrc.u4MemAddress && ptPngHwInst->rSrc.u4MemLen)
    {
       if(  !ptPngHwInst->rSrc.fgNoRingFifo &&
	   	((ptPngHwInst->rSrc.u4MemAddress+  ptPngHwInst->rSrc.u4MemLen) > ptPngHwInst->u4RingFifoEnd)
    	  )
       {
         UINT32 u4Len1=0;
	  u4Len1 = ptPngHwInst->u4RingFifoEnd-ptPngHwInst->rSrc.u4MemAddress;
	  //vPngLog_M("[PNG-Src ]un-Map[0x%x  length0x%x]\n",ptPngHwInst->rSrc.u4MemAddress, u4Len1);
          PNG_VMEM_UNMAP( ptPngHwInst->rSrc.u4MemAddress, u4Len1, ptPngHwInst->rSrc.u4Option);
	  // vPngLog_M("[PNG-Src-2]un-Map[0x%x  length0x 0x%x]\n",ptPngHwInst->u4RingFifoStart, ptPngHwInst->rSrc.u4MemLen-u4Len1 );
	   PNG_VMEM_UNMAP( ptPngHwInst->u4RingFifoStart, ptPngHwInst->rSrc.u4MemLen-u4Len1, ptPngHwInst->rSrc.u4Option);
	}
	else
	{	   	
           PNG_VMEM_UNMAP( ptPngHwInst->rSrc.u4MemAddress, ptPngHwInst->rSrc.u4MemLen, ptPngHwInst->rSrc.u4Option);
	    vPngLog_M("[PNG-Src ]un-Map[0x%x  length0x%x]\n",ptPngHwInst->rSrc.u4MemAddress, ptPngHwInst->rSrc.u4MemLen);	
	}
       ptPngHwInst->rSrc.u4MemAddress = 0;
       ptPngHwInst->rSrc.u4MemLen= 0;
       ptPngHwInst->rSrc.u4Option= 0;
	ptPngHwInst->rSrc.fgNoRingFifo = TRUE;
    }
#endif	
}

void vPngHalMapDstAddress(UINT32 u4HwInstId, UINT32 u4MemStart,UINT32 u4MemLen, UINT32 u4Option)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if(!ptPngHwInst->fgUseClrBuf)
    {
       return;
    }
    //vPngLog_M("[PNG-Dst ]Map[0x%x  length0x%x]\n",u4MemStart, u4MemLen);
    PNG_VMEM_MAP(u4MemStart, u4MemLen, u4Option);
    ptPngHwInst->rDst.u4MemAddress = u4MemStart;
    ptPngHwInst->rDst.u4MemLen= u4MemLen;
    ptPngHwInst->rDst.u4Option= u4Option;
#endif
}

void vPngHalunMapDstAddress(UINT32 u4HwInstId)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if(!ptPngHwInst->fgUseClrBuf)
    {
       return;
    }
    if(ptPngHwInst->rDst.u4MemAddress && ptPngHwInst->rDst.u4MemLen)
    {
        PNG_VMEM_UNMAP( ptPngHwInst->rDst.u4MemAddress, ptPngHwInst->rDst.u4MemLen, ptPngHwInst->rDst.u4Option);
	 vPngLog_M("[PNG-Dst ]un-Map[0x%x  length0x%x]\n",ptPngHwInst->rDst.u4MemAddress, ptPngHwInst->rDst.u4MemLen);	
        ptPngHwInst->rDst.u4MemAddress = 0;
        ptPngHwInst->rDst.u4MemLen= 0;
        ptPngHwInst->rDst.u4Option= 0;
    }
#endif	
}


void vPngHalunMapLineAddress(UINT32 u4HwInstId)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if(ptPngHwInst->rLineBuf.u4MemAddress  && ptPngHwInst->rLineBuf.u4MemLen )
    {
       vPngLog_M("[PNG-LINE ]un-Map[0x%x  length0x%x]\n",ptPngHwInst->rLineBuf.u4MemAddress, ptPngHwInst->rLineBuf.u4MemLen);	
       PNG_VMEM_UNMAP( ptPngHwInst->rLineBuf.u4MemAddress, ptPngHwInst->rLineBuf.u4MemLen, ptPngHwInst->rLineBuf.u4Option );
       ptPngHwInst->rLineBuf.u4MemAddress = 0;
       ptPngHwInst->rLineBuf.u4MemLen = 0;
       ptPngHwInst->rLineBuf.u4Option = 0;
    }
#endif
}

void vPngHalMapLineAddress(UINT32 u4HwInstId, UINT32 u4MemStart, UINT32 u4MemLen, UINT32 u4Option)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    vPngHalunMapLineAddress(u4HwInstId);
    vPngLog_M("[PNG] Enter %s gFifo[0x%x~ 0x%x]\n",__FUNCTION__,u4MemStart, u4MemLen);
    if(u4MemStart && u4MemLen)
    {
       PNG_VMEM_MAP( u4MemStart, u4MemLen, u4Option);
       ptPngHwInst->rLineBuf.u4MemAddress = u4MemStart;
       ptPngHwInst->rLineBuf.u4MemLen = u4MemLen;
       ptPngHwInst->rLineBuf.u4Option = u4Option;
    }
#endif
}
void vPngHalunMapLz777Address(UINT32 u4HwInstId)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    if(ptPngHwInst->rLz777Buf.u4MemAddress  && ptPngHwInst->rLz777Buf.u4MemLen )
    {
       vPngLog_M("[PNG-LZ777 ]un-Map[0x%x  length0x%x]\n",ptPngHwInst->rLz777Buf.u4MemAddress, ptPngHwInst->rLz777Buf.u4MemLen);	
       PNG_VMEM_UNMAP( ptPngHwInst->rLz777Buf.u4MemAddress, ptPngHwInst->rLz777Buf.u4MemLen, ptPngHwInst->rLz777Buf.u4Option );
       ptPngHwInst->rLz777Buf.u4MemAddress = 0;
       ptPngHwInst->rLz777Buf.u4MemLen = 0;
       ptPngHwInst->rLz777Buf.u4Option = 0;
    }
#endif
}

void vPngHalMapLz777Address(UINT32 u4HwInstId, UINT32 u4MemStart, UINT32 u4MemLen, UINT32 u4Option)
{
#if PNG_CACHE_MAP_ENABLE
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
     vPngLog_M("[PNG] Enter %s Fifo[0x%x~ 0x%x]\n",__FUNCTION__,u4MemStart, u4MemLen);
    vPngHalunMapLz777Address(u4HwInstId);
    if(u4MemStart && u4MemLen)
    {
       PNG_VMEM_MAP( u4MemStart, u4MemLen, u4Option);
       ptPngHwInst->rLz777Buf.u4MemAddress = u4MemStart;
       ptPngHwInst->rLz777Buf.u4MemLen = u4MemLen;
       ptPngHwInst->rLz777Buf.u4Option = u4Option;
    }
#endif
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
        // Printf("[PNG_HAL] vPngHalGetIr [PNG_HAL_EV_DEC_IRQ]\n");   
        #if (PNG_MMU_Performance)
        HAL_GetTime(&sPNG_End_Time);
        HAL_GetDeltaTime(&sPNG_Rst_Time, &sPNG_Start_Time, &sPNG_End_Time);
        UTIL_Printf("Decoding Time: %d.%d se\n",sPNG_Rst_Time.u4Seconds,(sPNG_Rst_Time.u4Micros));
        #endif
        VERIFY(BIM_ClearIrq(u2Vector));
        VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_DEC_IRQ, X_EV_OP_OR) == OSR_OK);
        break;
    case VECTOR_PNG2:
        // Printf("[PNG_HAL] vPngHalGetIr [PNG_HAL_EV_SRC_IRQ]\n");   
        VERIFY(BIM_ClearIrq(u2Vector));
        VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_SRC_IRQ, X_EV_OP_OR) == OSR_OK);
        break;
    case VECTOR_PNG3:
       //Printf("[PNG_HAL] vPngHalGetIr [PNG_HAL_EV_DST_IRQ]\n"); 
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

BOOL fgPngWTSet(UINT32 u4HwInstId,BOOL fgEnable)
{
    #if CONFIG_DRV_VERIFY_SUPPORT
	
    #if (!MUTI_AGENT_WT)
    UINT32 u4WtReg = PngReadWTREG(0x0804);
    #endif
    #endif
    if(fgEnable)
    {
       #if CONFIG_DRV_VERIFY_SUPPORT
       #if (!MUTI_AGENT_WT)
       PngWriteWTREG(0x0804,u4WtReg & 0xFFFEFFFF);
       #endif
       #endif
       _tPngHalHwInst[u4HwInstId].fgWtEnable = TRUE;
    }
    else
    {  
       #if CONFIG_DRV_VERIFY_SUPPORT
       #if (!MUTI_AGENT_WT)
       PngWriteWTREG(0x0804,u4WtReg | 0x10000);
       #endif
       #endif
       PngWriteREG(PNG_REG_BASE2, 0x1C, 0x10000000);
       _tPngHalHwInst[u4HwInstId].fgWtEnable = FALSE;
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
    //UINT32              u4WaitDataToDTimes;
//	Printf("u4PngMonitorReg ==0x%x ====\n",PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_MONITOR_REG));

    ePngEvent = PNG_HAL_EV_DEC_IRQ | PNG_HAL_EV_SRC_IRQ | PNG_HAL_EV_DST_IRQ | PNG_HAL_EV_STOP_IRQ;
    i4Ret = x_ev_group_wait_event_timeout(_tPngHalHwInst[u4HwInstId].hPngEvent, ePngEvent, &ePngEventGet, X_EV_OP_OR_CONSUME, u4WaitTime);
    u4PngMonitorReg = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_MONITOR_REG);
//	Printf("u4PngMonitorReg ==0x%x ====\n",u4PngMonitorReg);
    /*vPngLog_L("[Png]  decoding checksum 0x30= 0x%08x,  0x34= 0x%08x, 0x38= 0x%08x,  0x3c= 0x%08x\n", 
                      PngReadREG(PNG_REG_BASE2, 0x30),
                      PngReadREG(PNG_REG_BASE2, 0x34),
                      PngReadREG(PNG_REG_BASE2, 0x38),
                      PngReadREG(PNG_REG_BASE2, 0x3c));*/

    if (i4Ret == OSR_OK)
    {
        u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
        u4PngRegVal &= PNG_REG_IRQ0_BIT_MASK;
        PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_DECIRQ_CLEAN_EN);
        PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
        if (ePngEventGet & PNG_HAL_EV_STOP_IRQ)
        {
            vPngLog_M("[PNG--]PNG_HAL_RTN_STOPPED\n");
	       vPngHalunMapSrcAddress(u4HwInstId);      
           vPngHalunMapDstAddress(u4HwInstId);      
            return PNG_HAL_RTN_STOPPED;
        }
        if (ePngEventGet & PNG_HAL_EV_DEC_IRQ)
        {
        	//   _u4PngCheckSum = PngReadREG(0x4e00,0x14c);
		  // UTIL_Printf("[PNG] checksum = 0x%x \n",_u4PngCheckSum);
		     vPngHalunMapSrcAddress(u4HwInstId);      
           vPngHalunMapDstAddress(u4HwInstId);   
            if ((u4PngMonitorReg & PNG_REG_MONITOR_ALDER_ERR) == PNG_REG_MONITOR_ALDER_ERR)
            {
                UTIL_Printf("[PNG--]PNG_HAL_ALDER_ERR\n");
                UTIL_Printf("Register Monitor Register Value: 0x%x\n\n",u4PngMonitorReg);
	        UTIL_Printf("[PNG--]RingFifo[0x%x~ 0x%x]\n", 
                                           _tPngHalHwInst[u4HwInstId].u4RingFifoStart ,
                                           _tPngHalHwInst[u4HwInstId].u4RingFifoEnd );
                return PNG_HAL_ALDER_ERR;
            }
            if ((u4PngMonitorReg & PNG_REG_MONITOR_DEC_ERR) == PNG_REG_MONITOR_DEC_ERR)
            {
                UTIL_Printf("[PNG--]PNG_REG_MONITOR_DEC_ERR\n");
                UTIL_Printf("Register Monitor Register Value: 0x%x\n\n",u4PngMonitorReg);
	         UTIL_Printf("[PNG--]RingFifo[0x%x~ 0x%x]\n", 
                                          _tPngHalHwInst[u4HwInstId].u4RingFifoStart ,
                                           _tPngHalHwInst[u4HwInstId].u4RingFifoEnd );				 
                return PNG_HAL_RTN_FAIL;
            }
            #if  0//(CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8520)
            u4WaitDataToDTimes = 0;
            while( (PngReadREG(PNG_REG_BASE2, 0x40) != PngReadREG(PNG_REG_BASE2, 0x50))&&(u4WaitDataToDTimes < 20))    	
            {    	
	            x_thread_delay(2);       
	            vPngLog_H("[Png] Wait data to dram value match!, (4f40=0x%08x) != (4f50=0x%08x), Current STC %d\n",  PngReadREG(PNG_REG_BASE2, 0x40),
                        PngReadREG(PNG_REG_BASE2, 0x50), u4PngGetSTC()); 
	            u4WaitDataToDTimes ++;  	
            } 
            if( PngReadREG(PNG_REG_BASE2, 0x40) != PngReadREG(PNG_REG_BASE2, 0x50))
            {
              vPngLog_H("[Png]  Decoding error, (4f40=0x%08x) != (4f50=0x%08x), Current STC %d\n", PngReadREG(PNG_REG_BASE2, 0x40),
                        PngReadREG(PNG_REG_BASE2, 0x50), u4PngGetSTC());
              return PNG_HAL_RTN_FAIL;
            }
            #endif
            vPngLog_M("[PNG--]PNG_HAL_RTN_DEC\n");
            return PNG_HAL_RTN_DEC;
        }
        if (ePngEventGet & PNG_HAL_EV_SRC_IRQ)
        {
             vPngHalunMapSrcAddress(u4HwInstId);      
            u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
            u4PngRegVal &= PNG_REG_IRQ1_BIT_MASK;
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_SRCIRQ_CLEAN_EN);
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
	    vPngLog_M("[PNG--]PNG_HAL_RTN_SRC\n");
            if(ePngEventGet & PNG_HAL_EV_DST_IRQ)
            {
               VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_DST_IRQ, X_EV_OP_OR) == OSR_OK);
			   vPngLog_H("[PNG--]To-Keep--PNG_HAL_EV_DST_IRQ\n");
            }			
            return PNG_HAL_RTN_SRC;
        }
        if (ePngEventGet & PNG_HAL_EV_DST_IRQ)
        {
           vPngHalunMapDstAddress(u4HwInstId);           
            u4PngRegVal = PngReadREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG);
            u4PngRegVal &= PNG_REG_IRQ2_BIT_MASK;
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal | PNG_REG_DSTIRQ_CLEAN_EN);
            PngWriteREG(_tPngHalHwInst[u4HwInstId].u4HwRegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4PngRegVal);
	    vPngLog_M("[PNG--]PNG_HAL_RTN_DST\n");
	     if(ePngEventGet & PNG_HAL_EV_SRC_IRQ)
            {
               VERIFY(x_ev_group_set_event(_tPngHalHwInst[0].hPngEvent, PNG_HAL_EV_SRC_IRQ, X_EV_OP_OR) == OSR_OK);
			   vPngLog_H("[PNG--]To-Keep--PNG_HAL_EV_SRC_IRQ\n");
            }	
            return PNG_HAL_RTN_DST;
        }
        else
        {
          UTIL_Printf("[Png] decode error--4--jinsong\n");
           vPngHalunMapSrcAddress(u4HwInstId);      
           vPngHalunMapDstAddress(u4HwInstId);      
            PNGASSERT(0);
            vPngLog_H("[Png] Undefined event. Current STC %d\n", u4PngGetSTC());
            return PNG_HAL_RTN_FAIL;
        }   
    }
    else
    {
        UINT32 u4Tempval = 0;
		UINT32 u4Address = 0x6E000;
		UTIL_Printf("[Png] decode error--3--jinsong\n");
	vPngHalunMapSrcAddress(u4HwInstId);      
        vPngHalunMapDstAddress(u4HwInstId);      
        vPngLog_H("[Png]  Decoding time out-------Current STC %d\n", u4PngGetSTC());
        for(u4Tempval = 0; u4Tempval < 128; u4Tempval++)
        {
           x_thread_delay(5);  
           vPngLog_H("[Png]0x%x: 0x%x \n",u4Address+u4Tempval*4,
	   		PngReadREG(PNG_REG_BASE1, u4Tempval*4));
        }
		vPngLog_H("\n");
		x_thread_delay(100);
        return PNG_HAL_ALDER_ERR;
    }
}


INT32 i4PngHalGetHwInst(UINT32 *pu4HwInstId)
{
    INT32 i4=0;
    INT32 i4Ret = PNG_HAL_RTN_OK;
    
    #if (PNG_SUPPORT_POWEROFF)
    vPngSetClockOn();
    #endif

    x_sema_lock(_h_Png_Hal_Sema, X_SEMA_OPTION_WAIT);

    for (i4=0; i4<PNG_HAL_HW_INST_NUM; i4++)
    {
        if (_tPngHalHwInst[i4].fgActive == FALSE)
        {
            _tPngHalHwInst[i4].fgActive = TRUE;
            *pu4HwInstId = (UINT32)i4;
 #if PNG_CACHE_MAP_ENABLE
        _tPngHalHwInst[i4].rDst.u4MemLen =0;
        _tPngHalHwInst[i4].rDst.u4MemAddress=0;
         _tPngHalHwInst[i4].rSrc.u4MemLen =0;
        _tPngHalHwInst[i4].rSrc.u4MemAddress=0;
#endif 		
            break;
        }
    }
    if (i4 == PNG_HAL_HW_INST_NUM)
    {
        i4Ret = PNG_HAL_RTN_FAIL;
    }

    x_sema_unlock(_h_Png_Hal_Sema);
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
    x_sema_lock(_h_Png_Hal_Sema, X_SEMA_OPTION_WAIT);
    if(_tPngHalHwInst[u4HwInstId].u4LineBufAddr != PNG_HAL_INVALID32)
    {
       //vPngHalunMapLineAddress(u4HwInstId);
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void*)_tPngHalHwInst[u4HwInstId].u4LineBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void*)_tPngHalHwInst[u4HwInstId].u4LineBufAddr);
#endif
        #else
        x_free_aligned_nc_mem((void*)_tPngHalHwInst[u4HwInstId].u4LineBufAddr);
        #endif
        _tPngHalHwInst[u4HwInstId].u4LineBufAddr = PNG_HAL_INVALID32;
    }
    _tPngHalHwInst[u4HwInstId].fgActive = FALSE;
    x_sema_unlock(_h_Png_Hal_Sema);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSetStop(UINT32 u4HwInstId)
{
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC()); 
        return PNG_HAL_RTN_FAIL;
    }
    VERIFY(x_ev_group_set_event(_tPngHalHwInst[u4HwInstId].hPngEvent, PNG_HAL_EV_STOP_IRQ, X_EV_OP_OR) == OSR_OK);
    return PNG_HAL_RTN_OK;
}

void i4PngLocalAbitorReset(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;

    PngWriteREG(u4RegBase, 0x124, PngReadREG(u4RegBase,0x124)|0x40);
	mb();
    PngWriteREG(u4RegBase, 0x124, PngReadREG(u4RegBase,0x124)&0xFFFFFFBF);
	mb();

	IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_PNG, IOMMU_READ32(REG_RW_IOMMU_CFG4,IOMMU_PNG)|0x1000000);
	mb();
	IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_PNG, IOMMU_READ32(REG_RW_IOMMU_CFG4,IOMMU_PNG)|0xFEFFFFFF);

}

INT32 i4PngHalReset(UINT32 u4HwInstId)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    INT32               i4Ret;

    vPngHalunMapDstAddress(  u4HwInstId);
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        UTIL_Printf("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC()); 
        return PNG_HAL_RTN_FAIL;
    }
    #if (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8520)
    while( ((PngReadREG(u4RegBase,0xC8) & 0xF) != 0x8)||((PngReadREG(u4RegBase,0xCC) & 0x70) != 0x10))    	
    {    	
	 x_thread_delay(1);       
	 UTIL_Printf("[Png] Wait reset value match!, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());     	
    }    
 
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_START);mb();
    x_thread_delay(1);
    PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
#if 0 //(PNG_MMU_TEST)
	PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, 0x0f00);
	x_thread_delay(1);
	PngWriteREG(u4RegBase, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
#endif
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, 0);
    PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, 0x803);  //44403//////
#if PNG_MMU_TEST
    vPngMMUFlush();
#endif
    //clean event

  
	PngWriteREG(PNG_REG_BASE2, 0x04, 0x1);mb();
    PngWriteREG(PNG_REG_BASE2, 0x00, 0x0);
	
	PngWriteREG(u4RegBase, PNG_DEC_SRAM_CHIP_SELECT_REG, PNG_REG_SRAM_ON);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
   PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, PNG_REG_DRAM_REQ_1); //set 4e30 as 0 for 8555 from Jia Chen's suggestion.
#else
  #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8550)//s
    PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, PNG_REG_DRAM_REQ_2);
  #else
	PngWriteREG(u4RegBase, PNG_DEC_DRAM_REQUEST_SET_REG, PNG_REG_DRAM_REQ_4);
  #endif
	
#endif
#if PNG_MMU_TEST
    UTIL_Printf("<png> iommu test \n");
     //__asm__ volatile("isb\n");
     //x_thread_delay(2000);
     UTIL_Printf("<png>Flush pagetable \n");
#if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
    PNG_MMU_ENABLE();
#endif
    vPngMMUInit();
    vPngMMUFlush();
#endif

    i4Ret = x_ev_group_set_event(_tPngHalHwInst[u4HwInstId].hPngEvent, 0x00, X_EV_OP_CLEAR);
    VERIFY(i4Ret == OSR_OK);

    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalEnable(UINT32 u4HwInstId)
{
	UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    #if 1
	 if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC()); 
        return PNG_HAL_RTN_FAIL;
    }

    PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, PNG_REG_ENABLE);  
   
   #else
   
   PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, 0);
   PngWriteREG(u4RegBase, PNG_DEC_SRAM_CHIP_SELECT_REG, 0);
   
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
    PngWriteREG(PNG_REG_BASE2, 0x08, 0x1);mb();  
    PngWriteREG(PNG_REG_BASE2, 0x08, 0x0);mb();  
    return PNG_HAL_RTN_OK;
}
UINT32 u4PngHalGetAutoSkipCrc(UINT32 u4HwInstId)
{
   return PngReadREG(PNG_REG_BASE2, 0x08);
}
INT32 i4PngHalSetScale(UINT32 u4HwInstId, UINT32 u4PngInterlaceScale,UINT32 u4PngHorScale,UINT32 u4PngVrtScale)
{     

    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    PngWriteREG(u4RegBase, PNG_DEC_RESIZE_RATIO_REG, (u4PngInterlaceScale << 6) + (u4PngHorScale << 3) + u4PngVrtScale);
    return PNG_HAL_RTN_OK;
}
INT32 i4PngHalSetAutoSkipLastIdat(UINT32 u4HwInstId, BOOL fgLastIdat)
{
    if(fgLastIdat)
    {
        PngWriteREG(PNG_REG_BASE2, 0x00, 0x1);
    }
    else
    {
        PngWriteREG(PNG_REG_BASE2, 0x00, 0x0); 
    }
    return PNG_HAL_RTN_OK;
}
void vPngHalFireDec(UINT32 u4HwInstId, BOOL fgSrcResume, BOOL fgDstResume, BOOL fgStartDec)
{
	
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
	  

#if 0 //CONFIG_DRV_VERIFY_SUPPORT
	UINT32 u4PngCheckSum =0;
	#define PngReadREG_vfy(base, arg)         IO_READ32(IMAGE_BASE, ((base + arg) & 0xFFF))
	
	  u4PngCheckSum =	PngReadREG_vfy(0x4F00,0x38);

   // if(u4PngCheckSum)
    {
       UTIL_Printf("4F38 =%x\n",u4PngCheckSum);
    }
    u4PngCheckSum =	PngReadREG_vfy(0x4F00,0x3c);
   // if(u4PngCheckSum)
    {
        UTIL_Printf("4F3c =%x\n",u4PngCheckSum);
    }
    u4PngCheckSum =	PngReadREG_vfy(0x4F00,0x48);
   // if(u4PngCheckSum)
    {
        UTIL_Printf("4F48 =%x\n",u4PngCheckSum);
    }
    u4PngCheckSum =	PngReadREG_vfy(0x4F00,0x4c);

    //if(u4PngCheckSum)
    {
     UTIL_Printf("4F4c =%x\n",u4PngCheckSum);
    }
#endif	
   //  i4PngHalReset(u4HwInstId);
    if (fgSrcResume)
    {
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME0); mb();       
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME1); mb();
        PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_RESUME_REG, PNG_REG_SRC_RESUME0); mb();
    }
    if (fgDstResume)
    {
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME0); mb();
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME1); mb();
        PngWriteREG(u4RegBase, PNG_DEC_SLICE_RESUME_REG, PNG_REG_DST_RESUME0); mb();
    }
    if (fgStartDec)
    {
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START0); mb();
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START1); mb();
        PngWriteREG(u4RegBase, PNG_DEC_DECODE_START_REG, PNG_REG_DEC_START0); mb();
        #if (PNG_MMU_Performance)
        HAL_GetTime(&sPNG_Start_Time);
        #endif
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
    vPngLog_H("[Png] PIC info (%d %d) bitdep %d colortype %d, fgInt %d ClrMd %d\n",
    	ptPngHwInst->tIhdr.u4Width, ptPngHwInst->tIhdr.u4Height,
    	ptPngHwInst->tIhdr.u1BitDepth,
    	ptPngHwInst->tIhdr.u1ColorType,
    	ptPngHwInst->tIhdr.fgInterlace,
    	ptPngHwInst->eOutClrMd);

    switch(ptPngHwInst->tIhdr.u1ColorType)
    {
    case 0://colour type 0, greyscale image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth + 7 )/8 ;
        break;
    case 2://colour type 2, truecolourr image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width * ptPngHwInst->tIhdr.u1BitDepth * 3 + 7 )/8 ;
        break;
    case 3://colour type 3, indexed- colour image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width * ptPngHwInst->tIhdr.u1BitDepth + 7)/8 ;
        break;
    case 4://colour type 4, greyscale with alpha image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth  * 2 + 7)/8 ;
        break;
    case 6://colour type 6, truecolourr with alpha image
        u4LineBufferSize = (ptPngHwInst->tIhdr.u4Width  * ptPngHwInst->tIhdr.u1BitDepth  *4 + 7)/8 ;
        break;
     default:
        PNGASSERT(0);
        vPngLog_H("[Png]  not support input color type. Current STC %d\n",  u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
     }

     u4LineBufferSize = ((u4LineBufferSize + 31)/32)*32;//must be ow
     if (ptPngHwInst->u4LineBufAddr != PNG_HAL_INVALID32)
    {
	    //vPngHalunMapLineAddress(u4HwInstId);	 
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void*)ptPngHwInst->u4LineBufAddr );
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void*)ptPngHwInst->u4LineBufAddr );
#endif
        #else
        x_free_aligned_nc_mem((void*)ptPngHwInst->u4LineBufAddr );
        #endif
        ptPngHwInst->u4LineBufAddr = PNG_HAL_INVALID32;
    }
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    ptPngHwInst->u4LineBufAddr = (UINT32)x_alloc_aligned_dma_mem(u4LineBufferSize, 128);
#elif CONFIG_SYS_MEM_PHASE3
    ptPngHwInst->u4LineBufAddr = (UINT32)x_mem_aligned_alloc(u4LineBufferSize, 128);
#endif
    #else
    ptPngHwInst->u4LineBufAddr = (UINT32)x_alloc_aligned_nc_mem(u4LineBufferSize, 128);
    #endif
    if (ptPngHwInst->u4LineBufAddr == 0)
    {
        ASSERT(0);
        return PNG_HAL_RTN_FAIL;        
    }
     x_memset((void *)ptPngHwInst->u4LineBufAddr, 0, u4LineBufferSize);	
    vPngHalMapLineAddress(u4HwInstId,ptPngHwInst->u4LineBufAddr,u4LineBufferSize, TO_DEVICE);	
    vPngHalunMapLineAddress(u4HwInstId);
    x_memset((void *)ptPngHwInst->u4LZ77BufAddr, 0, PNG_LZ77_SIZE);
    vPngHalMapLz777Address(u4HwInstId,ptPngHwInst->u4LZ77BufAddr,PNG_LZ77_SIZE, TO_DEVICE);	 
    vPngHalunMapLz777Address(u4HwInstId);  
    u4RegTmpValue = ptPngHwInst->u4LZ77BufAddr & 0x3FFFFFFF;
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    u4RegTmpValue = (ptPngHwInst->u4LZ77BufAddr) ? PHYSICAL(ptPngHwInst->u4LZ77BufAddr) : 0;
    #if(!PNG_MMU_TEST)
    ptPngPicInfo->u4FifoStart = PHYSICAL(ptPngPicInfo->u4FifoStart);
    ptPngPicInfo->u4FifoEnd = PHYSICAL(ptPngPicInfo->u4FifoEnd);
    #endif
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_LZ77_STR_OWADDR_REG, (u4RegTmpValue >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_LZ77_END_OWADDR_REG, ((u4RegTmpValue + PNG_LZ77_SIZE) >> 4));
    #if (!PNG_MMU_TEST)
    u4RegTmpValue = ptPngPicInfo->u4FifoStart & 0x3FFFFFFF;
    #else
    u4RegTmpValue = ptPngPicInfo->u4FifoStart;
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_FIFO_SRT_OWADDR_REG, (u4RegTmpValue >> 4));
    #if (!PNG_MMU_TEST)
    u4RegTmpValue = ptPngPicInfo->u4FifoEnd & 0x3FFFFFFF;
    #else
    u4RegTmpValue = ptPngPicInfo->u4FifoEnd;
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_FIFO_END_OWADDR_REG, ((u4RegTmpValue >> 4) - 1));
   
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    u4RegTmpValue = PHYSICAL(ptPngHwInst->u4LineBufAddr);
    #else
    u4RegTmpValue = ptPngHwInst->u4LineBufAddr & 0x3FFFFFFF;
    #endif
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
    PngWriteREG(u4RegBase, PNG_DEC_SRC_WIDTH_HEIGHT_REG, ((ptPngPicInfo->ptIhdr->u4Width-1) << PNG_REG_SRC_W_SHIFT) + (ptPngPicInfo->ptIhdr->u4Height-1));    

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
    PngWriteREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG, ((u4RegTmpValue-1) << PNG_REG_DISP_W_SHIFT) + (ptPngPicInfo->u4SliceHeight-1));

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
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    vPngHalMapSrcAddress(u4HwInstId, ptPngPlte->u4PlteData, ptPngPlte->u4ChunkSize, TO_DEVICE,TRUE);
    #if(PNG_MMU_TEST)
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG,
    	(UINT32)((ptPngPlte->u4PlteData)?(UINT32)(ptPngPlte->u4PlteData):0) );
    #else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG,
    	(UINT32)((ptPngPlte->u4PlteData)?PHYSICAL((UINT32)(ptPngPlte->u4PlteData)):0) );
    #endif
    #else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)(ptPngPlte->u4PlteData));
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, ptPngPlte->u4ChunkSize);
    //start read plte
    vPngHalFireDec(u4HwInstId, TRUE, FALSE, TRUE);
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
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    vPngHalMapSrcAddress(u4HwInstId, ptPngTrns->u4TrnsData, ptPngTrns->u4ChunkSize, TO_DEVICE, TRUE);
    #if (PNG_MMU_TEST)
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngTrns->u4TrnsData)?(UINT32)(ptPngTrns->u4TrnsData):0));
    #else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngTrns->u4TrnsData)?PHYSICAL((UINT32)(ptPngTrns->u4TrnsData)):0));
    #endif
    #else
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, (UINT32)((ptPngTrns->u4TrnsData)));	
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, ptPngTrns->u4ChunkSize);
    //start read tRNS
    vPngHalFireDec(u4HwInstId, TRUE, FALSE, TRUE);
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
            switch(u1SrcFormat)
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
        vPngLog_H("[Png] i4PngHalSettRNSEx2.--SET PNG_REG_TRNS_MATCH_16BIT Current STC %d\n",  u4PngGetSTC());
    }
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_CTRL_REG, u4RegTmpValue);
    u4RegTmpValue = u2RVal << PNG_REG_TRNS_BG_R_SHIFT;
    u4RegTmpValue += u2GVal << PNG_REG_TRNS_BG_G_SHIFT;
    u4RegTmpValue += u2BVal << PNG_REG_TRNS_BG_B_SHIFT;
    PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4RegTmpValue);
    PngWriteREG(u4RegBase, PNG_DEC_ALPHA_MATCHED_PIXEL_REG, 0);

    return PNG_HAL_RTN_OK;
}

#if CONFIG_DRV_VERIFY_SUPPORT
  UINT8 _png_trans_bg_r=0;
  UINT8 _png_trans_bg_g=0;
  UINT8 _png_trans_bg_b=0;
  UINT8 _png_trans_key_r=0;
  UINT8 _png_trans_key_g=0;
  UINT8 _png_trans_key_b=0;  
   UINT8 _png_trans_alph_match=0;   
   BOOL _png_use_org_alpha = FALSE;
#endif 
INT32  i4PngHalTransVfy(UINT32 u4HwInstId)
{
   #if CONFIG_DRV_VERIFY_SUPPORT
       UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
       UINT32 u4RegTmpValue =0;
      PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
      UTIL_Printf("[Png] <-------->i4PngHalTransVfy. STC %d\n",  u4PngGetSTC());
	
       u4RegTmpValue = PNG_REG_TRNS_EN;	
        if (ptPngHwInst->tIhdr.u1BitDepth == 16)
        {
             u4RegTmpValue += PNG_REG_TRNS_MATCH_16BIT;
             vPngLog_H("[Png] i4PngHalSettRNSEx2.--SET PNG_REG_TRNS_MATCH_16BIT Current STC %d\n",  u4PngGetSTC());
         }
	 //check origian alpha
        {        
             //u4RegTmpValue =PNG_REG_TRNS_ORG_ALPHA|u4RegTmpValue  ; 
             _png_use_org_alpha = FALSE;
        }
       PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_CTRL_REG, u4RegTmpValue);	
       PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY0_REG, (0xFFFF<< PNG_REG_TRNS_R_SHIFT));
       PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_KEY1_REG, ((0xFFFF<< PNG_REG_TRNS_G_SHIFT) + (0 << PNG_REG_TRNS_B_SHIFT)));
	   _png_trans_key_r=0xFF;
       _png_trans_key_g=0xFF;
       _png_trans_key_b=0x00;
       u4RegTmpValue =0xAA << PNG_REG_TRNS_BG_R_SHIFT;
	u4RegTmpValue += 0xBB << PNG_REG_TRNS_BG_G_SHIFT;
       u4RegTmpValue += 0xCC << PNG_REG_TRNS_BG_B_SHIFT;
	   _png_trans_bg_r =0xAA;
	   _png_trans_bg_g =0xBB;
	   _png_trans_bg_b =0xCC;
       PngWriteREG(u4RegBase, PNG_DEC_TRANSPARENCY_BACKGROUND_REG, u4RegTmpValue);
	PngWriteREG(u4RegBase, PNG_DEC_ALPHA_MATCHED_PIXEL_REG, 0xDD);	
	   _png_trans_alph_match =0xdd;
   #endif  
        return PNG_HAL_RTN_OK;
}

INT32  i4PngHalDramU11U18SwithVfy(UINT32 u4HwInstId, BOOL fgU18)
{
    #if CONFIG_DRV_VERIFY_SUPPORT
	UINT32 u4Val =0;
	u4Val = *((volatile UINT32*)(IO_BASE +0x40800));
       if(fgU18)
       {
                UTIL_Printf("[Png] **Verft test U18\n");
		  *((volatile UINT32*)(IO_BASE +0x40800)) = u4Val |8;
       }
       else
       {
               UTIL_Printf("[Png] **Verft test U11\n");
               *((volatile UINT32*)(IO_BASE +0x40800))= u4Val |0xfffffff7;
       }
   #endif  
        return PNG_HAL_RTN_OK;
}
INT32  i4PngHalCheckSumSwithVfy(UINT32 u4HwInstId, UINT32 u4Case)
{
    #if CONFIG_DRV_VERIFY_SUPPORT
//	UINT32 u4Val =0;
	switch(u4Case)
	{
	  case 0:
	   UTIL_Printf("[Png] **CheckSum Par_read+ Pelout\n");
	   PngWriteREG(PNG_REG_BASE2, 0x34, 0x1FFFFFF );
	   PngWriteREG(PNG_REG_BASE2, 0x40,0x0000000);
	   break;
	  case 1:
	   UTIL_Printf("[Png] **CheckSum Png_r+Png_w\n");
	   PngWriteREG(PNG_REG_BASE2, 0x34, 0x3FFFFFF );
	   PngWriteREG(PNG_REG_BASE2, 0x40, 0x0000002);
	   break;
	  case 2:
	   UTIL_Printf("[Png] **CheckSum Lz777_r + Lz777_W\n");
	   PngWriteREG(PNG_REG_BASE2, 0x34,  0x5FFFFFF );
	   PngWriteREG(PNG_REG_BASE2, 0x40,0x0000004);
	   break;
	  case 3:
	   UTIL_Printf("[Png] **CheckSum Inbfr_r + Inbfr_W\n");
	   PngWriteREG(PNG_REG_BASE2, 0x34, 0x7FFFFFF );
	   PngWriteREG(PNG_REG_BASE2, 0x40,0x0000006);
	   break;
	 default:
	  UTIL_Printf("[Png] **CheckSum noneeeeee\n");
	   break;	   
	}
   #endif  
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
        if(u4PlteBgIdx > 255)
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

INT32 i4PngHalPitchEnable(UINT32 u4HwInstId, BOOL fgPitchEn)
{
	UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
        UINT32 u4RegVal;

	if(fgPitchEn)
	{
	u4RegVal = PngReadREG(u4RegBase, PNG_DEC_ENABLE_REG);
	UTIL_Printf("<png>enable 0x%x \n", u4RegVal);
	u4RegVal |= PNG_REG_PITCH_EN;
	PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, u4RegVal);
	}

	return 0;
}

INT32 i4PngHalGraceReset(UINT32 u4HwInstId)
{
	UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
        UINT32 u4RegVal;

	u4RegVal = PngReadREG(u4RegBase, PNG_DEC_ENABLE_REG);
	u4RegVal |= PNG_REG_RANDOM_RESET;
	PngWriteREG(u4RegBase, PNG_DEC_ENABLE_REG, u4RegVal);

	return 0;
}

BOOL fgPngGraceResetDone(UINT32 u4HwInstId)
{
	UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
        UINT32 u4RegVal;

	u4RegVal = PngReadREG(u4RegBase, PNG_DEC_DLA_STATE);

	return !!(u4RegVal & PNG_REG_CAN_RESET);
}


INT32 i4PngHalProcIDAT(UINT32 u4HwInstId, PNG_HAL_IDAT_INFO_T *ptPngIdatInfo)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    UINT32 u4RegVal;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
    PNG_HAL_INST_T  *ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    BOOL fgWtEnable = _tPngHalHwInst[u4HwInstId].fgWtEnable;
    #endif

	
	
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    vPngHalMapDstAddress(u4HwInstId, ptPngIdatInfo->u4DstBufStart, ptPngIdatInfo->u4DstBufEnd-ptPngIdatInfo->u4DstBufStart, FROM_DEVICE);   
    vPngHalMapSrcAddress(u4HwInstId, ptPngIdatInfo->u4SrtStrmAddr, ptPngIdatInfo->u4SrtStrmLen, TO_DEVICE, FALSE);
    #if (!PNG_MMU_TEST)
    ptPngIdatInfo->u4DstBufStart = PHYSICAL(ptPngIdatInfo->u4DstBufStart);
    ptPngIdatInfo->u4DstBufEnd = PHYSICAL(ptPngIdatInfo->u4DstBufEnd);
	ptPngIdatInfo->u4SrtStrmAddr = PHYSICAL(ptPngIdatInfo->u4SrtStrmAddr);
   vPngLog_H("[Png] L %d Dst 0x%x Src 0x%x \n",
	    __LINE__,
	    ptPngIdatInfo->u4DstBufStart, ptPngIdatInfo->u4SrtStrmAddr);
    #endif
    #endif
    #if (!PNG_MMU_TEST)
    u4RegVal = ptPngIdatInfo->u4DstBufStart & PNG_REG_B_ADDR_MASK;
    #else
    u4RegVal = ptPngIdatInfo->u4DstBufStart;
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_XY_ADDR_REG, u4RegVal);
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_SRT_OWADDR_REG, (u4RegVal >> 4));
    PngWriteREG(u4RegBase, PNG_DEC_PIXEL_OUT_END_OWADDR_REG, (ptPngIdatInfo->u4DstBufEnd >> 4));
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
    if(fgWtEnable)
    {   
        if(ptPngHwInst->tIhdr.fgInterlace)
#if (!PNG_MMU_TEST)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x80000000 | ((u4RegVal >> 4)&0x03FFFFFF));
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x20000000 | ((u4RegVal >> 4)&0x03FFFFFF));   
#endif
#else
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x80000000 | (u4RegVal >> 4));
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x20000000 | (u4RegVal >> 4));
#endif
#endif
        else
#if (!PNG_MMU_TEST)
          PngWriteREG(PNG_REG_BASE2, 0x1C, (u4RegVal >> 4)&0x03FFFFFF);
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C,  u4RegVal >> 4);
#endif
    }
    #endif	
    if (ptPngIdatInfo->fgLastIDATGrp)
    {
       vPngLog_H("[Png] fgLastIDATGrp-1 \n");
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

    u4RegVal = ptPngIdatInfo->u4SrtStrmLen & PNG_REG_B_LEN_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, u4RegVal);

    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG, ptPngIdatInfo->u4ChunkType);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);
//	Printf("u4PngMonitorReg ==0x%x ====\n",PngReadREG(u4RegBase, PNG_DEC_MONITOR_REG));

    
    if (ptPngIdatInfo->fg1stIDATChunk)
    {
        vPngHalFireDec(u4HwInstId, TRUE, FALSE, TRUE);
    }
    else
    {
        vPngHalFireDec(u4HwInstId, TRUE, FALSE, FALSE);
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

    vPngLog_M("[Png] i4PngHalProcNextSrc addr =0x%x, len = 0x%x\n",u4SrcStrmSrtAddr, u4SrcStrmLen);
    u4PngRegVal = PngReadREG(u4RegBase, PNG_DEC_CHUNK_TYPE_REG);
    if (u4PngRegVal != 0x49444154)
    {
        return PNG_HAL_RTN_FAIL;
    }
    vPngHalMapSrcAddress(u4HwInstId, u4SrcStrmSrtAddr, u4SrcStrmLen, TO_DEVICE, FALSE);  
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    #if (!PNG_MMU_TEST)
    u4SrcStrmSrtAddr = PHYSICAL(u4SrcStrmSrtAddr);
    #endif
    #endif
	
    if (fgLastIDATGroup)
    {
        vPngLog_H("[Png] fgLastIDATGroup = TRUE\n");
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_LAST_IDAT_GRP);
    }
    else
    {
        vPngLog_H("[Png] fgLastIDATGroup = FALSE\n");
        PngWriteREG(u4RegBase, PNG_DEC_LAST_IDAT_GROUP_REG, PNG_REG_NOTLAST_IDAT_GRP);
    }

    u4PngRegVal = u4SrcStrmLen & PNG_REG_B_LEN_MASK;
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_LENGTH_REG, u4PngRegVal);
    #if (PNG_MMU_TEST)
    u4PngRegVal = u4SrcStrmSrtAddr;
    #else
    u4PngRegVal = u4SrcStrmSrtAddr & PNG_REG_B_ADDR_MASK;
    #endif
    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_REG, u4PngRegVal);

    PngWriteREG(u4RegBase, PNG_DEC_BITSTRM_SRT_ADDR_WR_REG, PNG_REG_SRT_ADDR_CHG);
    PngWriteREG(u4RegBase, PNG_DEC_CHUNK_TYPE_WR_REG, PNG_REG_CHUNK_WR);

    vPngHalFireDec(u4HwInstId, TRUE, FALSE, FALSE);
    return PNG_HAL_RTN_OK;
}

INT32 i4PngHalSetSliceWH(UINT32 u4HwInstId, UINT32 u4SliceHeight)

{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    UINT32 u4RegTmpValue = PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG);
	
    vPngLog_L("[Png] befor setting  i4PngHalSetSliceWH= 0x%08x\n", 
    PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG));
    u4RegTmpValue  = (u4RegTmpValue&0xFFFF0000)>>16;
    PngWriteREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG, ((u4RegTmpValue) << PNG_REG_DISP_W_SHIFT) + ((u4SliceHeight&0x0000ffff)-1));
    vPngLog_L("[Png]  i4PngHalSetSliceWH= 0x%08x\n", 
    PngReadREG(u4RegBase, PNG_DEC_DISP_WIDTH_SLICE_HEGIHT_REG));
    return PNG_HAL_RTN_OK;
}
INT32 i4PngHalProcNextDst(UINT32 u4HwInstId, UINT32 u4DstBufStart, UINT32 u4DstBufEnd, BOOL fgLastSlice)
{
    UINT32 u4RegBase = _tPngHalHwInst[u4HwInstId].u4HwRegBase;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
    PNG_HAL_INST_T	*ptPngHwInst = &(_tPngHalHwInst[u4HwInstId]);
    BOOL fgWtEnable = _tPngHalHwInst[u4HwInstId].fgWtEnable;
    #endif
    if (u4HwInstId >= PNG_HAL_HW_INST_NUM)
    {
        PNGASSERT(0);
        vPngLog_H("[Png] HW init ID error, ID = %x. Current STC %d\n",  u4HwInstId, u4PngGetSTC());
        return PNG_HAL_RTN_FAIL;
    }
	
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    vPngHalMapDstAddress(u4HwInstId, u4DstBufStart, u4DstBufEnd-u4DstBufStart, FROM_DEVICE);      
    #if (!PNG_MMU_TEST)
    u4DstBufStart = PHYSICAL(u4DstBufStart);
    u4DstBufEnd = PHYSICAL(u4DstBufEnd);
    #else
    #endif
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
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)	
    if(fgWtEnable)
    {
       if(ptPngHwInst->tIhdr.fgInterlace)
#if (!PNG_MMU_TEST)
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x80000000 | ((u4DstBufStart >> 4)&0x03FFFFFF));
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x20000000 | ((u4DstBufStart >> 4)&0x03FFFFFF));
#endif
#else
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x80000000 | (u4DstBufStart >> 4));
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C, 0x20000000 | (u4DstBufStart >> 4));
#endif
#endif
       else
#if (!PNG_MMU_TEST)
          PngWriteREG(PNG_REG_BASE2, 0x1C, (u4DstBufStart >> 4)&0x03FFFFFF);
#else
          PngWriteREG(PNG_REG_BASE2, 0x1C,  u4DstBufStart >> 4);
#endif
    }
    #endif
    vPngHalFireDec(u4HwInstId, FALSE, TRUE, FALSE);
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

    for (i4=0; i4<PNG_HAL_HW_INST_NUM; i4++)
    {
        _tPngHalHwInst[i4].u4HwInstId = (UINT32)i4;
        _tPngHalHwInst[i4].fgActive = FALSE;
        //CJYang, move to CH2
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        _tPngHalHwInst[i4].u4LZ77BufAddr = (UINT32)x_alloc_aligned_dma_mem(PNG_LZ77_SIZE, 128);
#elif CONFIG_SYS_MEM_PHASE3
        _tPngHalHwInst[i4].u4LZ77BufAddr = (UINT32)x_mem_aligned_alloc(PNG_LZ77_SIZE, 128);
#endif
        #else
        _tPngHalHwInst[i4].u4LZ77BufAddr = (UINT32)x_alloc_aligned_nc_mem(PNG_LZ77_SIZE, 128);
        #endif
        //_tPngHalHwInst[i4].u4LZ77BufAddr = (UINT32)_u4PngLz77Buf;
        //--
        _tPngHalHwInst[i4].u4LineBufAddr  = PNG_HAL_INVALID32;

        szEvName[8] = (CHAR)(i4+0x30);
        i4Ret = x_ev_group_create(&_tPngHalHwInst[i4].hPngEvent, szEvName, PNG_HAL_EV_INITIAL);
        VERIFY(i4Ret == OSR_OK);
    }
    _tPngHalHwInst[0].u4HwRegBase = PNG_REG_BASE1;
#if (!PNG_SUPPORT_POWEROFF)
#if (!CONFIG_DRV_FPGA_BOARD)
    #if (CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8520)
    IO_WRITE32(CKGEN_BASE, PNG_CLK_OFFSET, PNG_CLOCK_SETTING);
    UTIL_Printf(" %s PNG clock ============= 0x%x ==============\n", __FUNCTION__,PNG_CLOCK_SETTING);    
    #endif
#endif //#if (!CONFIG_DRV_FPGA_BOARD)
#endif
    
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

    for (i4=0; i4<PNG_HAL_HW_INST_NUM; i4++)
    {
        //CJYang, move to CH2
        vPngHalunMapSrcAddress(i4);      
        vPngHalunMapDstAddress(i4);  
	 //vPngHalunMapLz777Address(i4);
        #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
        x_free_aligned_dma_mem((void*)_tPngHalHwInst[i4].u4LZ77BufAddr);
#elif CONFIG_SYS_MEM_PHASE3
        x_mem_aligned_free((void*)_tPngHalHwInst[i4].u4LZ77BufAddr);
#endif
        #else
        x_free_aligned_nc_mem((void*)_tPngHalHwInst[i4].u4LZ77BufAddr);
        #endif
        //--
        if(PNG_HAL_INVALID32 != _tPngHalHwInst[i4].u4LineBufAddr)
        {
           // vPngHalunMapLineAddress(i4);	 
           #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
           x_free_aligned_dma_mem((void*)_tPngHalHwInst[i4].u4LineBufAddr);
#elif CONFIG_SYS_MEM_PHASE3
           x_mem_aligned_free((void*)_tPngHalHwInst[i4].u4LineBufAddr);
#endif
           #else
           x_free_aligned_nc_mem((void*)_tPngHalHwInst[i4].u4LineBufAddr);
           #endif
           _tPngHalHwInst[i4].u4LineBufAddr = PNG_HAL_INVALID32;
        }

        i4Ret = x_ev_group_delete(_tPngHalHwInst[i4].hPngEvent);
        VERIFY(i4Ret == OSR_OK);
    }
    return PNG_HAL_RTN_OK;
}


