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

#ifndef __DRV_GFX3D_VFY_SYS_H
#define __DRV_GFX3D_VFY_SYS_H

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "gfx3d_vfy_def.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#if MEMORY_256_DISPATCH
	
#if (!MEMORY_256_LARGE_MC)
#define _LIST_FILE_ADDR_        0xC4100000  
#define _LIST_FILE_SIZE_        (1024 * 300)
#define _CONFIG_FILE_ADDR_      (_LIST_FILE_ADDR_ + _LIST_FILE_SIZE_) 
#define _CONFIG_FIE_SIZE_       (1024 * 300)
#define _MAKE_FILE_ADDR_        (_CONFIG_FILE_ADDR_ + _CONFIG_FIE_SIZE_) 
#define _MAKE_FILE_SIZE_        (1024 * 424)
	
#if (!GP_LARGE_MEMORY)
#define _GP_TABLE_ADDR_         (_MAKE_FILE_ADDR_ + _MAKE_FILE_SIZE_)  
#define _GP_TABLE_SIZE_         (1024 * 1024)
	
#define _PP_TABLE_ADDR_         (_GP_TABLE_ADDR_ + _GP_TABLE_SIZE_) 
#define _PP_TABLE_SIZE_         (1024 * 1024)
	
#define _HEX_FILE_ADDR_         (_PP_TABLE_ADDR_ + _PP_TABLE_SIZE_) 
#define _HEX_FILE_SIZE          (54 * 1024 * 1024)
	
#define _GP_PHYSICAK_ADDR_      (_HEX_FILE_ADDR_ + _HEX_FILE_SIZE) 
#define _GP_PHYSICAK_SIZE       (54 * 1024 * 1024)
	
#define _PP_PHYSICAK_ADDR_      (_GP_PHYSICAK_ADDR_ + _GP_PHYSICAK_SIZE) 
#define _PP_PHYSICAK_SIZE       (54 * 1024 * 1024)
#else
	
#define _GP_TABLE_ADDR_         (_MAKE_FILE_ADDR_ + _MAKE_FILE_SIZE_)  
#define _GP_TABLE_SIZE_         (1024 * 1024)
	
#define _PP_TABLE_ADDR_         (_GP_TABLE_ADDR_)                                   
#define _PP_TABLE_SIZE_         (1024 * 1024)
	
#define _HEX_FILE_ADDR_         (_PP_TABLE_ADDR_ + _PP_TABLE_SIZE_)    
#define _HEX_FILE_SIZE          (90 * 1024 * 1024)
	
#define _GP_PHYSICAK_ADDR_      (_HEX_FILE_ADDR_ + _HEX_FILE_SIZE)       
#define _GP_PHYSICAK_SIZE       (90 * 1024 * 1024)
	
#define _PP_PHYSICAK_ADDR_      (_HEX_FILE_ADDR_ + _HEX_FILE_SIZE)      
#define _PP_PHYSICAK_SIZE       (90 * 1024 * 1024)
#endif
	
#else

#if !SMALL_MEM_SIZE
#define _LIST_FILE_ADDR_        0xC2000000
#define _LIST_FILE_SIZE_        (1024 * 300)
	
#define _CONFIG_FILE_ADDR_      (_LIST_FILE_ADDR_ + _LIST_FILE_SIZE_) 
#define _CONFIG_FIE_SIZE_       (1024 * 300)
	
#define _MAKE_FILE_ADDR_        (_CONFIG_FILE_ADDR_ + _CONFIG_FIE_SIZE_) 
#define _MAKE_FILE_SIZE_        (1024 * 300)

#define _DRAM_BUSY_ADDR_        (_MAKE_FILE_ADDR_ + _MAKE_FILE_SIZE_) 
#define _DRAM_BUSY_SIZE_        (1024*124)

#define _GP_TABLE_ADDR_         (_DRAM_BUSY_ADDR_ + _DRAM_BUSY_SIZE_) 
#define _GP_TABLE_SIZE_         (1024 * 1024)
	
#define _PP_TABLE_ADDR_         (_GP_TABLE_ADDR_ + _GP_TABLE_SIZE_)
#define _PP_TABLE_SIZE_         (1024 * 1024)
	
#define _HEX_FILE_ADDR_         (_PP_TABLE_ADDR_ + _PP_TABLE_SIZE_)
#define _HEX_FILE_SIZE          (54 * 1024 * 1024)
	
#define _GP_PHYSICAK_ADDR_      (_HEX_FILE_ADDR_ + _HEX_FILE_SIZE) 
#define _GP_PHYSICAK_SIZE       (54 * 1024 * 1024)
	
#define _PP_PHYSICAK_ADDR_      (_GP_PHYSICAK_ADDR_ + _GP_PHYSICAK_SIZE ) 
#define _PP_PHYSICAK_SIZE       (54 * 1024 * 1024)

#define _MALI_MMU_BASE_ADDR_    (_PP_PHYSICAK_ADDR_ + _PP_PHYSICAK_SIZE) 
#define _MALI_MMU_BASE_SIZE_    (4 * 1024 * 1024)

#define _MALI_BUFFER_BASE_      (_MALI_MMU_BASE_ADDR_ + _MALI_MMU_BASE_SIZE_)
#define _MALI_BUFFER_SIZE_      (18 * 1024 * 1024)

#define _MALI_PP_DTE_ADDR_      (_MALI_BUFFER_BASE_ + _MALI_BUFFER_SIZE_)
#define _MALI_PP_DTE_SIZE_      (4 * 1024 * 1024)

#define _MALI_GP_DTE_ADDR_      (_MALI_PP_DTE_ADDR_ + _MALI_PP_DTE_SIZE_)
#define _MALI_GP_DTE_SIZE_      (4 * 1024 * 1024)

#define _MALI_PP1_DTE_ADDR_     (_MALI_GP_DTE_ADDR_ + _MALI_GP_DTE_SIZE_)
#define _MALI_PP1_DTE_SIZE_     (4 * 1024 * 1024)

#else

static UINT32  _LIST_FILE_ADDR_  =      0;
static UINT32 _LIST_FILE_SIZE_   =      0;
	
static UINT32 _CONFIG_FILE_ADDR_ =      0;
static UINT32 _CONFIG_FIE_SIZE_  =      0;
	
static UINT32 _MAKE_FILE_ADDR_   =      0;
static UINT32 _MAKE_FILE_SIZE_   =      0;

static UINT32 _DRAM_BUSY_ADDR_   =      0 ;
static UINT32 _DRAM_BUSY_SIZE_   =      0;

static UINT32 _GP_TABLE_ADDR_    =      0; 
static UINT32 _GP_TABLE_SIZE_    =      0;
	
static UINT32 _PP_TABLE_ADDR_    =      0;
static UINT32 _PP_TABLE_SIZE_    =      0;
	
static UINT32 _HEX_FILE_ADDR_    =      0;
static UINT32 _HEX_FILE_SIZE     =      0;
	
static UINT32 _GP_PHYSICAK_ADDR_ =      0;
static UINT32 _GP_PHYSICAK_SIZE  =      0;
	
static UINT32 _PP_PHYSICAK_ADDR_ =      0;
static UINT32 _PP_PHYSICAK_SIZE  =      0;

static UINT32 _MALI_MMU_BASE_ADDR_ =    0;
static UINT32 _MALI_MMU_BASE_SIZE_ =    0;

static UINT32 _MALI_BUFFER_BASE_   =    0;
static UINT32 _MALI_BUFFER_SIZE_   =    0;

static UINT32 _MALI_PP_DTE_ADDR_   =    0;
static UINT32 _MALI_PP_DTE_SIZE_   =    0;

static UINT32 _MALI_GP_DTE_ADDR_   =    0;
static UINT32 _MALI_GP_DTE_SIZE_   =    0;

static UINT32 _MALI_PP1_DTE_ADDR_  =    0;
static UINT32 _MALI_PP1_DTE_SIZE_  =    0;

#endif

#endif
	
#endif



UINT32 u4ReadBimReg(UINT32 u4VDecID, UINT32 u4Addr);
void vWriteBimReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
void vMaliWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliReadReg(UINT32 u4VDecID, UINT32 u4Addr);
void vMaliGPWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliGPReadReg(UINT32 u4VDecID, UINT32 u4Addr);
void vMaliPPWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliPPReadReg(UINT32 u4VDecID, UINT32 u4Addr);
void vMaliAXIWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliAXIReadReg(UINT32 u4VDecID, UINT32 u4Addr);
void vMaliWriteMem(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliReadMem(UINT32 u4VDecID, UINT32 u4Addr);

INT32 i4Gfx3DVfyReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize);
INT32 i4Gfx3DVfyWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize);
BOOL  fgReadFile(void* pvAddr, UINT32 u4BufSize, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength);



#endif // __DRV_GFX3D_VFY_SYS_H
