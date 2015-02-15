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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   $Workfile: jdec_mem_api.c $
 *
 * Project:
 * --------
 *   MT8105
 *
 * Description:
 * ------------
 *    Jpeg Decoder HAL
 *
 * Author: C.K. Hu
 * -------
 *
 *
 * Last changed:
 * -------------
 * $Author: p4admin $
 *
 * $Modtime: 03/12/18 3:29p $
 *
 * $Revision: #1 $
****************************************************************************/
// *********************************************************************
// Memo
// *********************************************************************
/*

*/
// *********************************************************************
// TODO
// *********************************************************************
/*
*/
#include "drv_config.h"
#include "x_hal_5381.h"
#include "x_common.h"
#include "jdec_hal_if.h"
#include "jdec_drv.h"
#include "jdec_debug.h"
#include "drv_config.h"
#if CONFIG_SYS_MEM_PHASE2
  #include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
   #include "x_kmem.h"
#else
  #include "x_mem_phase2.h"
#endif


#define BIG_MEM_USED  1

static UINT32 _u4BigMemStart = 0;
static UINT32 _u4BigMemSize  = 0;
static UINT32 _u4BigMemused  = 0;

void  jpg_init_align_big_mem(void)
{
#if BIG_MEM_USED
	_u4BigMemStart = 0;
	_u4BigMemSize  = 0;
  	_u4BigMemused  = 0;
#endif
  return;
}

extern unsigned long get_drvmem_mem(void);
void*  jpg_alloc_align_big_mem(UINT32 u4Size, UINT32 u4Aligment)
{
#if BIG_MEM_USED
    UINT32 u4CurMemAdrss =0;
    UINT32 u4AlignSize =0;
    if(_u4BigMemStart ==0)
    {
      //_u4BigMemStart = (UINT32)get_drvmem_mem();      
      _u4BigMemStart = (UINT32)BSP_AllocVfyReserveMemory(0, 2048, 160*1024*1024);
      UTIL_Printf("jpg_alloc_align_big_mem Phyiscal _u4BigMemStart =0x%x\n",_u4BigMemStart);
      _u4BigMemStart = VIRTUAL(_u4BigMemStart);
      UTIL_Printf("jpg_alloc_align_big_mem Virtue _u4BigMemStart =0x%x\n",_u4BigMemStart);
    }
    u4AlignSize = ((u4Size + u4Aligment-1)/u4Aligment) *u4Aligment;
    UTIL_Printf("jpg_alloc_align_big_mem size=0x%x, alsize= 0x%x\n",u4Size, u4AlignSize);

    u4CurMemAdrss = _u4BigMemStart + _u4BigMemused;
    _u4BigMemused+= u4AlignSize;
    UTIL_Printf("jpg_alloc_align_big_mem Virtue =0x%x\n", u4CurMemAdrss);
    return  (void *)u4CurMemAdrss;
#else
    return x_alloc_aligned_dma_mem(u4Size, u4Aligment);
#endif
}
void  jpg_free_align_big_mem(void* ptr)
{
 #if BIG_MEM_USED
  UTIL_Printf("warning =========jpg_alloc_free_big_mem \n");
 #else
   x_free_aligned_dma_mem(ptr);
 #endif
  return;
}

static UINT32 _u4dBigMemStart = 0;
static UINT32 _u4dBigMemSize  = 0;
static UINT32 _u4dBigMemused  = 0;
void  jpg_init_align_dbig_mem(void)
{
#if BIG_MEM_USED
	_u4dBigMemStart = _u4BigMemStart + _u4BigMemused;
	_u4dBigMemSize  = 0;
  	_u4dBigMemused  = 0;
#endif
  return;
}

void*  jpg_alloc_align_dbig_mem(UINT32 u4Size, UINT32 u4Aligment)
{
#if BIG_MEM_USED
    UINT32 u4CurMemAdrss =0;
    UINT32 u4AlignSize =0;
   
    u4AlignSize = ((u4Size + u4Aligment-1)/u4Aligment) *u4Aligment;
    UTIL_Printf("jpg_alloc_align_big_mem size=0x%x, alsize= 0x%x\n",u4Size, u4AlignSize);

    u4CurMemAdrss = _u4dBigMemStart + _u4dBigMemused;
    _u4dBigMemused+= u4AlignSize;
    UTIL_Printf("jpg_alloc_align_dbig_mem Virtue =0x%x\n", u4CurMemAdrss);
    return  (void *)u4CurMemAdrss;
#else
    return x_alloc_aligned_dma_mem(u4Size, u4Aligment);
#endif
}
void  jpg_free_align_dbig_mem(void* ptr)
{
 #if BIG_MEM_USED
  UTIL_Printf("warning =========jpg_alloc_free_big_mem \n");
 #else
   x_free_aligned_dma_mem(ptr);
 #endif
  return;
}
void x_hw_mem_free(void* ptr, BOOL fgSupportMMU)
{
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY		
  #if CONFIG_SYS_MEM_PHASE2
   #if JDEC_IO_MMU_TEST
    if(fgSupportMMU)
    {
        //x_free_vmem(ptr);
         x_mem_free(ptr); 
    }
    else
   #endif
    {
        jpg_free_align_dbig_mem(ptr);
    }
   #elif CONFIG_SYS_MEM_PHASE3
    #if JDEC_IO_MMU_TEST  
    if(fgSupportMMU)
    {     
        x_mem_vfree(ptr);
    }
    else
    #endif
    {
        x_mem_aligned_free(ptr);
    }
  #else
        x_free_aligned_dma_mem(ptr);
  #endif
#else  
        x_free_aligned_nc_mem(ptr);
#endif
}

void *x_hw_mem_alloc( UINT32 u4Size, UINT32 u4_alignment,BOOL fgSupportMMU)
{
	 void *ptr = NULL;
#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
   #if CONFIG_SYS_MEM_PHASE2
	   #if JDEC_IO_MMU_TEST
	     if(fgSupportMMU)
	     {
                ptr = x_mem_alloc_virtual(u4Size);//IOMMU
                if(ptr)
                {
                  x_kmem_sync_table(ptr,u4Size);
                }	  
            }
            else                   
       #endif
           {
               ptr = jpg_alloc_align_dbig_mem(u4Size,u4_alignment);
           }
   #elif CONFIG_SYS_MEM_PHASE3
       #if CONFIG_ENABLE_IOMMU
        if(fgSupportMMU)
	     {
            ptr  = (UINT32)x_mem_valloc(u4Size);
			if(ptr)
            {
              x_kmem_sync_table(ptr,u4Size); 
            }
        }
        else
       #endif
       {
            ptr= (UINT32)x_mem_aligned_alloc(u4Size,u4_alignment);	 
       }
   #else
             ptr  = x_alloc_aligned_dma_mem(u4Size,u4_alignment);
   #endif
 #else
             ptr  =x_alloc_aligned_nc_mem(u4Size,u4_alignment);
 #endif  
    return ptr;
} 

