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

#if G3D_VFY_ENV
#define G3D_ALLOC_MEM_SIZE      448*1024*1024

#if defined(__MODEL_slt__)		
#define MEM_DATA_BASE_ADDR	    0x20000000
#else
#define MEM_DATA_BASE_ADDR	    0x20000000
#endif

/* below size must match with eG3DMemLayOut definistion */
#define _LIST_FILE_SIZE_        (1024*1024)	
#define _CONFIG_FIE_SIZE_       (1024*1024)	
#define _MAKE_FILE_SIZE_        (1024*1024)
#define _HEX_FILE_SIZE          (140 * 1024 * 1024)
#define _GP_PHYSICAK_SIZE       (140 * 1024 * 1024)	
#define _PP_PHYSICAK_SIZE       (140 * 1024 * 1024)
#else
#define G3D_ALLOC_MEM_SIZE      144*1024*1024
#define MEM_DATA_BASE_ADDR	    0x6e00000
/*  below size must match with eG3DMemLayOut definistion */
#define _LIST_FILE_SIZE_        (1024*1024)	
#define _CONFIG_FIE_SIZE_       (1024*1024)	
#define _MAKE_FILE_SIZE_        (1024*1024)
#define _HEX_FILE_SIZE          (40 * 1024 * 1024)
#define _GP_PHYSICAK_SIZE       (40 * 1024 * 1024)	
#define _PP_PHYSICAK_SIZE       (40 * 1024 * 1024)
#endif


#define MALI450_VECTOR_GFX3D_PP     MALI_PP0_IRQ
#define MALI450_VECTOR_GFX3D_PP1    MALI_PP1_IRQ
#define MALI450_VECTOR_GFX3D_GP     MALI_GP_IRQ

#define MALI450_VECTOR_GFX3D_GPMMU  MALI_GP_MMU_IRQ
#define MALI450_VECTOR_GFX3D_PPMMU  MALI_PP0_MMU_IRQ
#define MALI450_VECTOR_GFX3D_PP1MMU MALI_PP1_MMU_IRQ


UINT32 u4ReadBimReg(UINT32 u4Addr);
void vWriteBimReg(UINT32 u4Addr, UINT32 u4Val);
void vMaliWriteReg(UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliReadReg(UINT32 u4Addr);
void vMaliGPWriteReg(UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliGPReadReg(UINT32 u4Addr);
void vMaliPPWriteReg(UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliPPReadReg(UINT32 u4Addr);
void vMaliAXIWriteReg(UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliAXIReadReg(UINT32 u4Addr);
void vMaliWriteMem(UINT32 u4Addr, UINT32 u4Val);
UINT32 u4MaliReadMem(UINT32 u4Addr);

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */


#if T32_HOST

extern FILE *fopen(const UINT8 *filename, const UINT8 *mode);
extern INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fseek(FILE *stream, INT32 offset, INT32 whence);
extern INT32 fclose(FILE *stream);
extern INT64 ftell(FILE *stream);
BOOL fgGPUReadPCFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength);

#elif USB_G3D_VFY

BOOL  fgReadFile(void* pvAddr, UINT32 u4BufSize, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength);

#else
BOOL fgG3DFATSFsReadFile(
	CHAR *strFileName,
	void* pvAddr,
	UINT32 u4Offset,
	UINT32 u4Length,
	UINT32 *pu4RealReadSize,
	UINT32 *pu4TotalFileLength,
	INT32 *pi4FileId);

#endif


#endif // __DRV_GFX3D_VFY_SYS_H
