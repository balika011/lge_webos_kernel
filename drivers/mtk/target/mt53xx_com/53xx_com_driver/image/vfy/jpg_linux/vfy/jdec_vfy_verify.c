/*
*
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
*
* \par Project
*    MT8520
*
* \par Description
*    Jpeg Decoder Verification Code
*
* \par Author_Name
*    CK Hu
*
* \par Last_Changed
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#include "x_assert.h"
#include "drv_config.h"

//#include "x_hal_8520.h"
//#include "x_hal_5381.h.h"
#include "x_typedef.h"
//#include "x_rtos.h"
#include "x_stl_lib.h"
#include "x_bim.h"
//#include "file_io.h"
//#include <stdio.h>
#if 1// CONFIG_DRV_LINUX
#include "x_os.h"
#else
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include "x_assert.h"
#include "x_printf.h"

#include "jdec_drv.h"
#include "jdec_com_jfif.h"
#include "jdec_hal_if.h"
#include "jdec_debug.h"
//#include "jdec_drv.h"
#include "x_ckgen.h"
//#include "chip_ver.h"
#include "x_debug.h"
#include "x_drv_cli.h"
#if PERFORMANCE_TEST
#include "x_timer.h"
#endif

//#include "sys_config.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif
//#include <mach/cache_operation.h> 

extern void vJdecMapPrintf(const char *format_string, ...);
#if JDEC_MM_MODE_ENABLE
static BOOL _fgMmmodeFirstMCUROW = TRUE;
#endif

BOOL _fgTestBasliePic =FALSE;
UINT32 _ui4TestDelay =0;
#define SAVE_GOLDEN_CHECK_SUM 1
UINT32 _i4FrameYChecksum;
UINT32 _i4FrameCChecksum;
UINT32 _i4OrgJpegFilesize =0;

#if 1//(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
#include "../../../../../../mt5880/diag/UDVT_IF.h"
#define CONFIG_JPG_FATS_SUPPORT  1
#else
#define CONFIG_JPG_FATS_SUPPORT  0
#define JDEC_VFY_FILE_IO_SEMI_HOSTING 1
#define JDEC_VFY_FILE_IO_RVD_UTILITY  2
#define JDEC_READ_USB_DIRECT 3
#define JDEC_VFY_FILE_IO_TYPE JDEC_READ_USB_DIRECT
#endif

#if 0//CONFIG_DRV_LINUX
#define strstr  x_strstr
#define strcpy  x_strcpy
#define strlen  x_strlen
#define strncpy x_strncpy
#define strcat  x_strcat
//#define sprintf x_sprintf
#define strcmp  x_strcmp
#define strtol  x_strtoll
#define memset  x_memset
#define memcpy  x_memcpy
#endif

#if 0 // mark by pingzhao for 8520 verify compile error
#define DMXCMD_READ32(offset)			IO_READ32(TSMUX0_BASE, ((offset) * 4))
#endif
extern BOOL _debug_decode_done_log;
extern UINT8 _fgEnableRegisterLog;
extern INLINE UINT32 u4HwJdecRead32(UINT32 u4HwId,UINT32 addr);
extern INLINE UINT32 u4HwImgReszRead32(UINT32 u4HwId,UINT32 addr);

#if PERFORMANCE_TEST
extern HAL_TIME_T _rJdecStratTime, _rJdecEndTime, _rJdecTotalTime;
#endif

#if JDEC_IO_MMU_TEST
extern void  vIOMMU_OverRead_Protection(UINT32 ui4_Agent,
                                               UINT32 ui4_Mode,
                                               UINT32 ui4_StartAddr0,
                                               UINT32 ui4_Buff_sz0,
                                               UINT32 ui4_StartAddr1,
                                               UINT32 ui4_Buff_sz1);
#endif
//-------------------------------



INT32 i4SaveOrignFile(void);
extern void vHwJdecPrintRegister(UINT32 u4HwId);
extern void vHwImgReszPrintRegister(UINT32 u4HwId);
UINT32 _ui4McuRowNumberPerTime =20; 
#if JDEC_IO_MMU_TEST
void* x_jpg_vmem(UINT32 u4Size)
{
#if 1
   //extern void *GGT_get_vmem(UINT32 u4Size);
   //extern void *IOMMU_get_fragment_vmem(UINT32 u4Size);
   VOID *ptr = NULL;
   //ptr = IOMMU_get_fragment_vmem(u4Size);
   ptr = x_mem_alloc_virtual(u4Size);
   	ASSERT(ptr != NULL);
   x_memset(ptr,0,u4Size);
   x_kmem_sync_table(ptr,u4Size);
#else
   //extern void *GGT_get_vmem(UINT32 u4Size);
   //extern void *IOMMU_get_fragment_vmem(UINT32 u4Size);
   UTIL_Printf("x_jpg_vmem [%x]\n",u4Size);
   extern void* BSP_VAllocFragmentMem(UINT32 u4Size);
   VOID *ptr = NULL;
   ptr = x_alloc_vmem(u4Size);// x_alloc_vmem(u4Size);//
   	ASSERT(ptr != NULL);
	UTIL_Printf("ptr [%x]\n",(UINT32)ptr);
   x_memset(ptr,0,u4Size);
   UTIL_Printf("clean ptr [%x]\n",(UINT32)ptr);
   //x_kmem_sync_table(ptr,u4Size);
#endif
   return ptr;

}
#endif
#define JDEC_MAP_UNMAP_CACHE 1
#if !JDEC_MAP_UNMAP_CACHE
#define vJdecMapPrintf  UTIL_Printf 
#else
extern void vJdecMapPrintf(const char *format_string, ...);
#endif

#include "x_hal_arm.h"
void vJdecVfyMapBuf(UINT32 i4Address, UINT32 i4Len, UINT32 i4Option)
{ 
#if 0
	switch(i4Option)
	{
		case TO_DEVICE:
			 vJdecMapPrintf("MAP from[0x%x] length[0x%x] TO_DEVICE\n",i4Address,i4Len);
             #if JDEC_MAP_UNMAP_CACHE
              BSP_dma_map_vaddr((UINT32) i4Address, i4Len, i4Option);
            #else
              BSP_CleanDCacheRange((UINT32)i4Address, i4Len);            
            #endif 
            break;
		case FROM_DEVICE:
			vJdecMapPrintf("MAP from[0x%x] length[0x%x] FROM_DEVICE\n",i4Address,i4Len);
    	    #if JDEC_MAP_UNMAP_CACHE
             BSP_dma_map_vaddr((UINT32) i4Address, i4Len, i4Option);
            #else
             BSP_InvDCacheRange((UINT32)i4Address, i4Len);            
            #endif 
    	   break;
		case BIDIRECTIONAL:	
			vJdecMapPrintf("MAP from[0x%x] length[0x%x] BIDIRECTIONAL\n",i4Address,i4Len);
		   #if JDEC_MAP_UNMAP_CACHE
             BSP_dma_map_vaddr((UINT32) i4Address, i4Len, i4Option);
           #else
             BSP_FlushDCacheRange((UINT32)i4Address, i4Len);            
           #endif 
		   break;	
	   	default:
		  ASSERT(0);
		  break;
	}   
#else
	switch(i4Option)
	{
		case TO_DEVICE:
			  vJdecMapPrintf("MAP from[0x%x] length[0x%x] TO_DEVICE\n",i4Address,i4Len);
			  HalFlushDCacheMultipleLine((UINT32)i4Address, i4Len);
        break;
		case FROM_DEVICE:
			  vJdecMapPrintf("MAP from[0x%x] length[0x%x] FROM_DEVICE\n",i4Address,i4Len);
			  HalInvalidateDCacheMultipleLine((UINT32)i4Address, i4Len);
	         break;
		case BIDIRECTIONAL:
    		         vJdecMapPrintf("MAP from[0x%x] length[0x%x] BIDIRECTIONAL\n",i4Address,i4Len);
    	                 HalFlushInvalidateDCacheMultipleLine((UINT32)i4Address, i4Len);
		   break;
		default:
		  ASSERT(0);

		  break;
       }
#endif
	//vJdecMapPrintf("MAP from[0x%x] length[0x%x] done\n",i4Address,i4Len);      	
	 return;
}
void vJdecVfyUnMapBuf(UINT32 i4Address, UINT32 i4Len, UINT32 i4Option)
{
#if 0
	switch(i4Option)
	{
		case TO_DEVICE:
			vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] TO_DEVICE\n",i4Address,i4Len);
			 #if JDEC_MAP_UNMAP_CACHE
             BSP_dma_unmap_vaddr((UINT32) i4Address, i4Len, TO_DEVICE);
           #else
             BSP_CleanDCacheRange((UINT32)i4Address, i4Len);            
           #endif 
           break;
		case FROM_DEVICE:
			 vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] FROM_DEVICE\n",i4Address,i4Len);
			 #if JDEC_MAP_UNMAP_CACHE
               BSP_dma_unmap_vaddr((UINT32) i4Address, i4Len, FROM_DEVICE);
             #else
             BSP_InvDCacheRange((UINT32)i4Address, i4Len);            
             #endif 			
	         break;
		case BIDIRECTIONAL:		
    		vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] BIDIRECTIONAL\n",i4Address,i4Len);
    	   #if JDEC_MAP_UNMAP_CACHE
             BSP_dma_unmap_vaddr((UINT32) i4Address, i4Len, i4Option);
           #else
             BSP_InvDCacheRange((UINT32)i4Address, i4Len);            
           #endif 
		   break;
		default:
		  ASSERT(0);
			
		  break;	
	}           	
#else
	switch(i4Option)
	{
		case TO_DEVICE:
		vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] TO_DEVICE\n",i4Address,i4Len);
		  break;
		case FROM_DEVICE:                
                vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] FROM_DEVICE\n",i4Address,i4Len);                
                HalInvalidateDCacheMultipleLine((UINT32)i4Address, i4Len);
		case BIDIRECTIONAL:
                vJdecMapPrintf("UNMAP from[0x%x] length[0x%x] BIDIRECTIONAL\n",i4Address,i4Len);
        	 HalInvalidateDCacheMultipleLine((UINT32)i4Address, i4Len);
		   break;
		default:
		  ASSERT(0);

		  break;
	}
	return;
#endif
}

#if CONFIG_JPG_FATS_SUPPORT 
INT32 i4JdecVfyReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    FILE *fp;
    INT32 i4RealReadSize;

    if((fp =  (FILE *)UDVT_IF_OpenFile(pcFileName,"rb")) != NULL)
    {
        i4RealReadSize = UDVT_IF_ReadFile((void *)u4FileBufSa,sizeof(char),u4BufSize,(UINT32)fp);

        UDVT_IF_CloseFile((UINT32)fp);
    } else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;

}

INT32 i4JdecVfyWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{

    FILE *fp;
    INT32 i4RealReadSize;

    if((fp = (FILE *)UDVT_IF_OpenFile(pcFileName,"wb")) != NULL)
    {
        i4RealReadSize = UDVT_IF_WriteFile((void *)u4FileBufSa,sizeof(char),u4BufSize,(UINT32)fp);

        UDVT_IF_CloseFile((UINT32)fp);
    } else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;
}


INT32 i4JdecVfyFopen(char *pcFileName,char *pcMode)
{
    FILE *fp;
    fp =  (FILE *)UDVT_IF_OpenFile(pcFileName,pcMode);

    return (INT32)fp;
}


char _abJdecPrintMsg[256];
INT32 i4JdecVfyFprintf(INT32 i4FileHandle,const char *format, ...)
{

    sprintf(_abJdecPrintMsg,format);

    UDVT_IF_WriteFile((void *)_abJdecPrintMsg,sizeof(char),strlen(_abJdecPrintMsg),(UINT32)i4FileHandle);

    return 0;
}


INT32 i4JdecVfyFclose(INT32 i4FileHandle)
{
    return UDVT_IF_CloseFile((UINT32)i4FileHandle);
}

#define i4JdecVfyPrintf UTIL_Printf
#elif(JDEC_VFY_FILE_IO_TYPE == JDEC_READ_USB_DIRECT)
#include "../../../../../../mt5880/diag/UDVT_IF.h"
INT32 i4JdecVfyReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
     INT32 fp;
    INT32 i4RealReadSize;
	UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile open USB %s \n" ,pcFileName);
    if((DrvFSUSBOpenFile(pcFileName,0, &fp)) == 0)
    {
    	UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile read  USB %s \n" ,pcFileName);
        i4RealReadSize = DrvFSUSBReadFile(fp,(void *)u4FileBufSa,u4BufSize);
         UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile done 0x%x \n" ,i4RealReadSize);
        DrvFSUSBCloseFile(fp);
    } else
    {
        i4RealReadSize = -1;
        UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile open USB %s failed \n" ,pcFileName);
    }

    return i4RealReadSize;

}
INT32 i4JdecVfyWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    INT32 fp;
    INT32 i4RealReadSize;

    if((DrvFSUSBOpenFile(pcFileName,1, &fp)) == 0)
    {
        i4RealReadSize = DrvFSUSBWriteFile(fp, (void *)u4FileBufSa,u4BufSize);
        x_thread_delay(100);

        DrvFSUSBCloseFile(fp);
    } else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;
}


INT32 i4JdecVfyFopen(char *pcFileName,char *pcMode)
{
    INT32 fp;
    INT32 u4OpenMode;
    if(*pcMode == 'w')
    {
       u4OpenMode = 1;
    }else
    {
       u4OpenMode = 0;
    }
    DrvFSUSBOpenFile(pcFileName,u4OpenMode, &fp);

}


char _abJdecPrintMsg[256];
INT32 i4JdecVfyFprintf(INT32 i4FileHandle,const char *format, ...)
{
    sprintf(_abJdecPrintMsg,format);
    DrvFSUSBWriteFile(i4FileHandle, (void *)_abJdecPrintMsg,strlen(_abJdecPrintMsg));
    return 0;
}


INT32 i4JdecVfyFclose(INT32 i4FileHandle)
{
    return DrvFSUSBCloseFile((FILE *)i4FileHandle);
}

#define i4JdecVfyPrintf UTIL_Printf
#elif(JDEC_VFY_FILE_IO_TYPE == JDEC_VFY_FILE_IO_SEMI_HOSTING)
/*typedef struct
{
  INT32 handle;
  INT32 offset;
  UINT32 filesize;
}
FILE;*/
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

extern  FILE *fopen(const UINT8 *filename, const UINT8 *mode);
extern	INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern	INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern	INT32 fseek(FILE *stream, INT32 offset, INT32 whence);
extern	INT32 fclose(FILE *stream);
extern	INT64 ftell(FILE *stream);

INT32 i4JdecVfyReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
#if 0 // mark by pingzhao for compile error
    UINT32 u4RealReadSize;

    if(fgReadFileName((void *)u4FileBufSa,u4BufSize,pcFileName,0,&u4RealReadSize))
        return u4RealReadSize;
    else
#endif 
    FILE *fp;
    INT32 i4RealReadSize;
 UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile open %s \n" ,pcFileName);
    if((fp = fopen(pcFileName,"rb")) != NULL)
    {
        UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile read %s \n" ,pcFileName);
        i4RealReadSize = fread((void *)u4FileBufSa,sizeof(char),u4BufSize,fp);
        UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile done 0x%x \n" ,i4RealReadSize);
        fclose(fp);
    } else
    {
    UTIL_Printf("[JDEC_VFY] i4JdecVfyReadFile open %s failed \n" ,pcFileName);
        i4RealReadSize = -1;
    }

    return i4RealReadSize;

}


INT32 i4JdecVfyWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    #if 0 // mark by pingzhao for compile error
    if(fgWriteData2PC((void *)u4FileBufSa,u4BufSize,pcFileName))
        return 0;
    else
#endif
    //return -1;
    FILE *fp;
    INT32 i4RealReadSize;
    UTIL_Printf("[JDEC_VFY] %s Write 0x%x from 0x%x\n",pcFileName,u4BufSize,u4FileBufSa);
    if((fp = fopen(pcFileName,"wb")) != NULL)
    {
        i4RealReadSize = fwrite((void *)u4FileBufSa,sizeof(char),u4BufSize,fp);

        fclose(fp);
    } else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;
}


INT32 i4JdecVfyFopen(char *pcFileName,char *pcMode)
{
    FILE *fp;
    fp = fopen(pcFileName,pcMode);

    return (INT32)fp;
}


char _abJdecPrintMsg[256];
INT32 i4JdecVfyFprintf(INT32 i4FileHandle,const char *format, ...)
{
#if 0
    va_list ap;

    // Parameter setting
    va_start(ap, format);
    vsprintf(_abJdecPrintMsg, format, ap);
    va_end(ap);

    return fprintf((FILE *)i4FileHandle,_abJdecPrintMsg,"");
#else
    sprintf(_abJdecPrintMsg,format);

    fwrite((void *)_abJdecPrintMsg,sizeof(char),strlen(_abJdecPrintMsg),(FILE *)i4FileHandle);

    return 0;
#endif
}


INT32 i4JdecVfyFclose(INT32 i4FileHandle)
{
    return fclose((FILE *)i4FileHandle);
}


#define i4JdecVfyPrintf UTIL_Printf
#elif(JDEC_VFY_FILE_IO_TYPE == JDEC_VFY_FILE_IO_RVD_UTILITY)
#define i4JdecVfyReadFile i4RvdReadFile
#define i4JdecVfyWriteFile i4RvdWriteFile
#define i4JdecVfyFopen i4RvdFopen
#define i4JdecVfyFprintf i4RvdFprintf
#define i4JdecVfyFclose i4RvdFclose
#define i4JdecVfyPrintf i4RvdPrintf



void i4JdecAsiToInt(INT32 u4CharBufSa,INT32 i4Len)
{
    INT32 i;
    for ( i = 0; i < i4Len; i++)
    {
      if(*(BYTE*)( u4CharBufSa)>='0' && *(BYTE*)( u4CharBufSa)<='9')
	 *(BYTE*)( u4CharBufSa) = *(BYTE*)( u4CharBufSa) - '0';
      else
	  *(BYTE*)( u4CharBufSa) = *(BYTE*)( u4CharBufSa) - 'a' + 10;
	   u4CharBufSa ++;
    	}
}

#endif

//-------------------------------


extern void vImgReszVfyJpegPicModeInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput
);
extern INT32 i4ImgReszVfyJpegPicModeGetResizeStatus(UINT32 u4HwId);
extern void vImgReszVfyJpegPicModeUninit(UINT32 u4HwId);
extern void vImgReszVfyJpegPicModeWorkaroundYC2AYUV(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                                           UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                                           UINT32 u4SrcBufSA1,UINT32 u4SrcBufSA2,
                                                           UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                                           UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                                           UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                                           UINT32 u4DestBufSA1);
extern void vImgReszVfyJpegPreloadInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBufSA1,UINT32 u4SrcBufSA2,UINT32 u4SrcBufSA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgYExist,BOOL fgCbExist, BOOL fgCrExist
);
extern void vImgReszVfyJpegPreloadInitEnhanceMCUROW(UINT32 u4HwId,UINT32 u4SrcBufWidth,
            UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
            UINT32 u4SrcBufSA1,UINT32 u4SrcBufSA2,UINT32 u4SrcBufSA3,
            UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
            UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
            UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
            UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
            UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
            UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
            UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
            BOOL fgYExist,BOOL fgCbExist, BOOL fgCrExist,
            BOOL fgAYUVOutput,BOOL fgFirstScan
                                                       );
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
extern void vImgReszVfyJpegScanModeInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput,BOOL fgYExist,
                                              BOOL fgCbExist,BOOL fgCrExist
);
extern void vImgReszVfyJpegScanModePreloadInit(UINT32 u4HwId,UINT32 u4SrcBufWidth,
                                              UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
                                              UINT32 u4SrcBuf1SA1,UINT32 u4SrcBuf1SA2,UINT32 u4SrcBuf1SA3,
                                              UINT32 u4SrcBuf2SA1,UINT32 u4SrcBuf2SA2,UINT32 u4SrcBuf2SA3,
                                              UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
                                              UINT32 u4DestImgWidth,UINT32 u4DestImgHeight,
                                              UINT32 u4DestImgXOff,UINT32 u4DestImgYOff,
                                              UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
                                              UINT32 u4HSampleFactor0,UINT32 u4VSampleFactor0,
                                              UINT32 u4HSampleFactor1,UINT32 u4VSampleFactor1,
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2,
                                              BOOL fgAYUVOutput,BOOL fgYExist,
                                              BOOL fgCbExist,BOOL fgCrExist,BOOL fgPreload
);
#endif
extern INT32 i4ImgReszVfyJpegPreloadGetResizeStatus(UINT32 u4HwId);
extern void vImgReszVfyJpegPreloadUninit(UINT32 u4HwId);

extern void vImgReszVfyBufDeBlock(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufWidth,UINT32 u4BufHeight,
                           UINT32 u4BlockWidth,UINT32 u4BlockHeight);
extern void vImgReszVfyBufDeAddrSwap(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufLen);

extern void vHwJdecSetInterrupt(UINT32 u4HwId, BOOL fgEnable);

//extern INT32 i4ImgReszVfyJpegPicModeStopResize(UINT32 u4HwId);

extern BOOL fgHwJdecIsFoundMarker(UINT32 u4HwId);

extern UINT32 u4HwJdecGetFoundMarker(UINT32 u4HwId);

void vTxDataForRingTest(BYTE *pDest, BYTE *pSrc, UINT32 u4Size);
#if 1
#define JDEC_COLOR_BUF_WIDTH   4280
#define JDEC_COLOR_BUF_HEIGHT  3000
#else
#define JDEC_COLOR_BUF_WIDTH   4400//1920
#define JDEC_COLOR_BUF_HEIGHT  4000//1088
#endif
#define JDEC_FRAME_BUF_WIDTH   352
#define JDEC_FRAME_BUF_HEIGHT  240

#define JDEC_FIFO_SIZE (6*1024*1024)

#define JDEC_GOLDEN_FIFO_SIZE (5 *1024)

#define JDEC_RING_FIFO_SIZE (8*1024)

#define JPEG_EV_IRQ 1

#define JDEC_COMPARE_WITH_GOLDEN 1 // if want to output color buffer, should close this flag
#define JDEC_DRAMBUSY_TEST 0
#define JDEC_VFY_TEST_MCUROW_ENHANCE 0

#define JDEC_FILE_NAME_LEN 128
UINT32  BURN_IN_TEST_COUNT =1; // only used in burn in test, work with JDEC_COMPARE_WITH_GOLDEN
BOOL _fgForceDisableInterrupt= FALSE;
BYTE *_EmuJpgFifoback;
BYTE *_EmuJpgFifo;
BYTE *_EmuRingFifo;
BYTE *_EmuRingFifo_align;
INT32 _i4JpegFileSize = 0;
JPEG_JFIF_HEADER_T *_prJPEG_JFIF_HEADER = NULL;
UINT32 _u4ErrMCUNum = 0;
UINT32 _u4MaxStc = 0;

#define JpegFileListMax 400
BYTE *_pbJpegFileList = NULL;

UINT32 _u4JpegFileListNum = 0;
UINT32 _u4CurJpegFileListNum = 0;
BYTE *_abJpegFileList[JpegFileListMax];
BYTE _pbJpegGolden[3][JDEC_FILE_NAME_LEN];

UINT32 _u4ColorBufY;
UINT32 _u4ColorBufCb;
UINT32 _u4ColorBufCr;
UINT32 _u4ColorBufY2;
UINT32 _u4ColorBufCb2;
UINT32 _u4ColorBufCr2;

 UINT32  _u4GoldenBufY;
 UINT32  _u4GoldenBufCb ;
 UINT32  _u4GoldenBufCr ;

UINT32 _u4FrameBufY;
UINT32 _u4FrameBufC;
UINT32 _u4EnhanceBufY;
UINT32 _u4EnhanceBufCb;
UINT32 _u4EnhanceBufCr;

#ifdef JDEC_IO_MMU_TEST
BYTE* _EmuJpgFifoApp;
BYTE *_EmuRingFifoApp;
UINT32 _u4FrameBufYApp;
UINT32 _u4FrameBufCApp;
#endif

UINT32 _u4ColorBufWidth;
UINT32 _u4ImageWidth;
UINT32 _u4ImageHeight;

UINT32 _u4XSampleY,_u4YSampleY;
UINT32 _u4XSampleCb,_u4YSampleCb;
UINT32 _u4XSampleCr,_u4YSampleCr;

UINT32 _u4DiffCnt;

UINT32 _u4JdecVfyHighMemBase = 0xDD000000;

BOOL _fgJdecVfyForceBreak = FALSE;
BOOL _fgJdecSetIRQ = FALSE;
BOOL _fgFirstScan = FALSE;
HANDLE_T _hJdecEventHandle;

#include "x_timer.h"
extern void HAL_GetTime(HAL_TIME_T* pTime);
extern void HAL_GetDeltaTime(HAL_TIME_T * pResult, HAL_TIME_T * pOlder, HAL_TIME_T * pNewer);
static HAL_TIME_T _rDecStartTime, _rDecEndTime, _rDecTotalTime;
UINT64  _ui8JdecDecTotalTime =0;
// Test option
BOOL _fgCompareGold = FALSE;
BOOL _fgSaveRes = FALSE;
BOOL _fgJdecVfyTestPicMode = FALSE;
BOOL _fgJdecVfyTestPicModeAYUVOutput = FALSE;
BOOL _fgJdecVfyTestPicModeAYUVOutputWorkaround = FALSE;
BOOL _fgJdecVfyTestProgressiveMCUEnhance = FALSE;
BOOL _fgJdecVfyTestProgressiveMCUMultiColltet = FALSE;
BOOL _fgJdecVfyTestProgressiveEnhance = FALSE;
BOOL _fgJdecVfyTestProgressiveScanMultiColltet = FALSE;
#if JDEC_MM_MODE_ENABLE
BOOL _fgJdecVfyTestProgressiveMCUMulitCollectMultiPass = FALSE;
#endif
BOOL _fgJdecVfyTestProgressiveScanEnhance = FALSE;
BOOL _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
BOOL _fgJdecVfyReinitTest = FALSE;
BOOL _fgJdecVfyHighMemTest = FALSE;
BOOL _fgJdecVfyTestWritePointer = FALSE;
BOOL _fgJdecVfyTestWritePointerUpdate = FALSE;
BOOL _fgJdecVfyTestVideoOutMode = FALSE;
// Test interrupt function for MCU ROW mode & PICTURE mode
BOOL _fgJdecVfyTestInterrupt = FALSE;
BOOL _fgJdecVfyTestPicModeStop = FALSE;
//Test Ring Fifo in picture mode
BOOL _fgJdecVfyTestRingFifo = FALSE;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
BOOL _fgGetResizer;
#endif
UINT32 _u4PreviousAdd = 0x0;
UINT32 _ui4StartClock = 5;
UINT32 _ui4EndClock = 5;
BOOL _fgErrorDetect = FALSE;
extern UINT32 _u4ImgReszVfyJpegPicModeTempLineBuf[2];
#if JDEC_MM_MODE_ENABLE
extern void vHwJdecHwResetMM(UINT32 u4HwId);
/*
#define GOTO_DECOD_1ST_SCAN    \
{\
  if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) &&\
  	 (prHalOuptPutInfo->u4DecodedMCURow < prHalOuptPutInfo->u4ToalMCURow-1))\
 {\
    UTIL_Printf("[Jdec_Verfiy] -----scan loop finished-------\n");\
    i4JDEC_HAL_Update_Output_Status(u4JDecID);\
    vHwJdecHwReset(u4JDecID);\
    hParser = (JPEGHANDLE)prJPEG_JFIF_HEADER;\
	x_memset((void*)prJPEG_JFIF_HEADER,0x0,sizeof(JPEG_JFIF_HEADER_T));\
    JFIF_New(hParser,_EmuJpgFifo,JDEC_FIFO_SIZE);\
    //fgJdecCalculateMcuRowWidthHeight(&prJPEG_JFIF_HEADER->rSOF);\
    //i4JDEC_HAL_Set_Read_Pointer(u4JDecID,(UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead));\
	UTIL_Printf("[Jdec_Verfiy]Fifo start read pointer :%x\n", (UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead));\
    UTIL_Printf("[Jdec_Verfiy]u1HSampFactor:u1VSampFactor [0x%x:0x%x]\n",\
                              prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1HSampFactor,\
                              prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor);\
    goto MM_MODE_RESCAN;\
  }	\
}
*/

#define GOTO_DECOD_1ST_SCAN    \
{\
  if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS) &&\
  	 (prHalOuptPutInfo->u4DecodedMCURow < prHalOuptPutInfo->u4ToalMCURow-1))\
 {\
    i4JDEC_HAL_Update_Output_Status(u4JDecID);\
    vHwJdecHwResetMM(u4JDecID);\
    hParser = (JPEGHANDLE)prJPEG_JFIF_HEADER;\
    x_memset((void*)prJPEG_JFIF_HEADER,0x0,sizeof(JPEG_JFIF_HEADER_T));\
    JFIF_New(hParser,_EmuJpgFifo,JDEC_FIFO_SIZE);\
	goto MM_MODE_RESCAN;\
  }	\
}
#endif
UINT32 u4GetSTC(void)
{
#if 0 //mark by pingzhao for 8520 verify compile error
    UINT32 u4StartStcL = 0,u4StartStcH,u4Stc;

    //u4StartStcL = DMXCMD_READ32(142);

    u4StartStcH = DMXCMD_READ32(143);
    u4Stc = ((u4StartStcH << 1) |(u4StartStcL>>31));

    return u4Stc;
#else
    return 0;
#endif
}

INT32 i4SaveEnhanceBuf(void);
INT32 i4JdecVfyDecodeOneScan(UINT32 u4JDecID,JDEC_HAL_DEC_MODE_T eDecodingMode,
                                             JDEC_HAL_PIC_INFO_T *prPicInfo,
                                             JDEC_HAL_INPUT_INFO_T *prInputInfo,
                                             JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,
                                             UINT32 u4FrameBufY,UINT32 u4FrameBufC,
                                             UINT32 u4FrameBufWidth,UINT32 u4FrameBufHeight,
                                             UINT32 u4MCUWidth,UINT32 u4MCUHeight)
{
    UINT32 u4MCUy =0;
    UINT32 u4MCUCnt;
    INT32 i4Ret = 0;
	UINT32 u4HeightPerMCU =0;

    u4HeightPerMCU =(prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor /prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor) * 8;
	UTIL_Printf("[u4HeightPerMCU====[%d]\n",u4HeightPerMCU);
    _fgJdecVfyForceBreak = FALSE;
    u4MCUCnt = 0;
	JDEC_VERY_HAL_LOG("%s: u4MCUWidth[%d]u4MCUHeight[%d]\n", __FUNCTION__, u4MCUWidth,u4MCUHeight);
#if JDEC_MM_MODE_ENABLE
    if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
    {
           EV_GRP_EVENT_T eJpegEvent;
           EV_GRP_EVENT_T eJpegEventGet;
           //JDEC_HAL_OUTPUT_INFO_T *prHalOuptPutInfo = prJDEC_HAL_Get_Output_Status(u4JDecID); //only for log ifo
            //UTIL_Printf("Curr MCU-Row Cnt: %d\n", prHalOuptPutInfo->u4DecodedMCURow);
            if (_fgJdecVfyTestInterrupt)
            {
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }
			if(!_fgJdecVfyTestRingFifo)
			{
			   if(_fgMmmodeFirstMCUROW)
               vJdecVfyMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE);
			}
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutYBufSa, u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,  FROM_DEVICE);
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCbBufSa, u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch, FROM_DEVICE);
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCrBufSa, u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch, FROM_DEVICE);  
            {
			   INT32 i4Ret =0;
               i4Ret = i4JDEC_HAL_Decode(u4JDecID); //UTIL_Printf("i4JDEC_HAL_Decode return %d\n", i4Ret);
            }
            if (_fgJdecVfyTestInterrupt)
            {
                INT32 i4Result;
			    eJpegEvent = JPEG_EV_IRQ;
			     i4Ret = x_ev_group_wait_event_timeout(_hJdecEventHandle, eJpegEvent, &eJpegEventGet, X_EV_OP_OR_CONSUME, 20000);
                if(i4Ret != OSR_OK)
                {
                     extern void vHwJdecPrintRegister(UINT32 u4HwId);
					 if(!_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 2;
					 }
                 	  vHwJdecPrintRegister(u4JDecID);
			          UTIL_Printf("[JDEC][Verify] Compare wait decode -time outtt------------\n");  
					  if(2==_fgEnableRegisterLog)
					  {
				       _fgEnableRegisterLog = 0;
					  }
                }
                ///VERIFY(i4Ret == OSR_OK);
                i4Result = i4JDEC_HAL_GET_Error_Type(u4JDecID);
                if (i4Result > 0)
                {  
                    extern void vHwJdecPrintRegister(UINT32 u4HwId);
				   	 if(!_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 2;
					 }
                     vHwJdecPrintRegister(u4JDecID);
					 if(2==_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 0;
					 }
			         i4JdecVfyPrintf("[Jdec][Verify] Decode Error %x\n",i4Result);
                     #if CONFIG_JPG_FATS_SUPPORT
                     FATS_IF_SaveLog(1,"[Jpeg][Verify]  %s Decode Error %x",_abJpegFileList[_u4CurJpegFileListNum],i4Result);
                     #endif

                     return -1;
                }
                else
                {
                    i4JDEC_HAL_Get_Decode_Status(u4JDecID);//Temp solution!
                }
            }
            else
            {

                 while(TRUE)
                 {
                     if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                     {
                         break;
                     }
					 if((u4JDEC_HAL_Get_VLD_RPtr(u4JDecID) & 0xFFFFFFF0) == (u4JDEC_HAL_Get_VLD_WPtr(u4JDecID) & 0xFFFFFFF0))
                     {
	                     #if JDEC_IO_MMU_TEST
	                     if((u4JDEC_HAL_Get_VLD_RPtr(u4JDecID) & 0xFFFFFFF0) == (((UINT32)_EmuRingFifo + JDEC_RING_FIFO_SIZE/2)& 0xFFFFFFF0))
	                     #else
	                     if((u4JDEC_HAL_Get_VLD_RPtr(u4JDecID) & 0xFFFFFFF0) == (PHYSICAL((UINT32)_EmuRingFifo + JDEC_RING_FIFO_SIZE/2)& 0xFFFFFFF0))
	                     #endif
	                     {
	                         i4JDEC_HAL_Set_Write_Pointer(u4JDecID,prInputInfo->u4InFifoSa);
	                     }
	                     else
	                     {
	                        vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead,JDEC_RING_FIFO_SIZE); 								
	                     	i4JDEC_HAL_Set_Write_Pointer(u4JDecID,(UINT32)(_EmuRingFifo + JDEC_RING_FIFO_SIZE /2));
	                     }							   				 
            	     }
                 }
            }
 	        if(!_fgJdecVfyTestRingFifo && _fgMmmodeFirstMCUROW)
 	        {	      
              vJdecVfyUnMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE); 
			  _fgMmmodeFirstMCUROW = FALSE;
 	        }
            vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutYBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,  FROM_DEVICE);
            vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCbBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch, FROM_DEVICE);
            vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCrBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch, FROM_DEVICE);
            return 0;
      }
#endif	
    for(u4MCUy=0; u4MCUy<u4MCUHeight; u4MCUy++)
    {
       extern UINT32 _u4CurMcuRow_num;
       JDEC_VERY_HAL_LOG("Curr MCU-Row Y Cnt: %d\n", u4MCUy);
	   _u4CurMcuRow_num = u4MCUy;
       if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
#else
                )
#endif
        {
            EV_GRP_EVENT_T eJpegEvent;
            EV_GRP_EVENT_T eJpegEventGet;

            UTIL_Printf("Curr MCU-Row Cnt: %d\n", u4MCUy);
            if (_fgJdecVfyTestInterrupt)
            {
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }
   	        if(!_fgJdecVfyTestRingFifo)
                vJdecVfyMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE);
#if 1	
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutYBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch, FROM_DEVICE);
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCbBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,FROM_DEVICE);
            vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCrBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,FROM_DEVICE);           
#else
			vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutYBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,FROM_DEVICE);//FROM_DEVICE);
			vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCbBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,FROM_DEVICE);//FROM_DEVICE);
			vJdecVfyMapBuf((UINT32) prOutputInfo->u4OutCrBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT, FROM_DEVICE);//FROM_DEVICE);    

#endif
			 {
			   INT32 i4Ret =0;
               i4Ret = i4JDEC_HAL_Decode(u4JDecID);// UTIL_Printf("i4JDEC_HAL_Decode return %d\n", i4Ret);
             }

            if (_fgJdecVfyTestInterrupt)
            {
                INT32 i4Result;
                eJpegEvent = JPEG_EV_IRQ;
                i4Ret = x_ev_group_wait_event_timeout(_hJdecEventHandle, eJpegEvent, &eJpegEventGet, X_EV_OP_OR_CONSUME, /*10000*/90000);
                JDEC_VERY_HAL_LOG("[JDEC_VFY]---decode done--\n ");
			    if(i4Ret != OSR_OK)
               {
                   extern void vHwJdecPrintRegister(UINT32 u4HwId);
				  _fgEnableRegisterLog = TRUE;
				   UTIL_Printf("[JDEC_VFY]---decode timeout--\n ");				  
                  vHwJdecPrintRegister(u4JDecID);
                   return -1;
                }
			    UTIL_Printf("[JDEC_VFY]---decode done--\n ");
                VERIFY(i4Ret == OSR_OK);
                i4Result = i4JDEC_HAL_GET_Error_Type(u4JDecID);
                if (i4Result > 0)
                {
                     extern void vHwJdecPrintRegister(UINT32 u4HwId);
				   	 if(!_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 2;
					 }
                     vHwJdecPrintRegister(u4JDecID);
					 if(2==_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 0;
					 }
                    i4JdecVfyPrintf("[Jdec][Verify] Decode Error %x\n",i4Result);
                    #if CONFIG_JPG_FATS_SUPPORT
                    FATS_IF_SaveLog(1,"[Jpeg][Verify]  %s Decode Error %x",_abJpegFileList[_u4CurJpegFileListNum],i4Result);
                    #endif

                    return -1;
                }
                else
                {
                    i4JDEC_HAL_Get_Decode_Status(u4JDecID);//Temp solution!
                     if(0)//_debug_decode_done_log)
			           {
			              UTIL_Printf("decode done==========\n");
                         vHwJdecPrintRegister(u4JDecID);
			           }
                }
            }
            else
            {

                 while(TRUE)
                 {
                     if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                     {
                       if(0)//_debug_decode_done_log)
			           {
			              UTIL_Printf("decode done==========\n");
                         vHwJdecPrintRegister(u4JDecID);
			           }
                       break;
                     }
                 }
            }
			
   	         if(!_fgJdecVfyTestRingFifo)
                vJdecVfyUnMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE); 
		#if 1 
             vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutYBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,FROM_DEVICE);
             vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCbBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,FROM_DEVICE);
             vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCrBufSa,u4MCUHeight*u4HeightPerMCU*prOutputInfo->u4OutYBufPitch,FROM_DEVICE);
		#else
			 vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutYBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,FROM_DEVICE);//FROM_DEVICE);
 			 vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCbBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,FROM_DEVICE);//FROM_DEVICE);
			 vJdecVfyUnMapBuf((UINT32) prOutputInfo->u4OutCrBufSa,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT, FROM_DEVICE);//FROM_DEVICE);			
		#endif
        }
        else if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#else
                )
#endif
        {
            UINT32 u4WorkaroundFrameBufY = 0;
            UINT32 u4WorkaroundFrameBufC = 0;
#if 1  // for test : don't wait picture interrupt,only polling image resizer finish flag
            EV_GRP_EVENT_T eJpegEvent;
            EV_GRP_EVENT_T eJpegEventGet;
#endif
            if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
            {
                 _fgGetResizer = TRUE;
                if(_fgJdecVfyTestPicModeAYUVOutputWorkaround)
                {
                    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                    u4WorkaroundFrameBufY = (UINT32)x_alloc_aligned_dma_mem(2* u4FrameBufWidth/4*u4FrameBufHeight,1024);
                    u4WorkaroundFrameBufC = (UINT32)x_alloc_aligned_dma_mem(2* u4FrameBufWidth/4*u4FrameBufHeight/2,1024);
                    #else
                    u4WorkaroundFrameBufY = (UINT32)x_alloc_aligned_nc_mem(2* u4FrameBufWidth/4*u4FrameBufHeight,1024);
                    u4WorkaroundFrameBufC = (UINT32)x_alloc_aligned_nc_mem(2 *u4FrameBufWidth/4*u4FrameBufHeight/2,1024);
                    #endif

                    vImgReszVfyJpegPicModeInit(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                                        prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                                        prOutputInfo->u4OutBank0YBufSa,prOutputInfo->u4OutBank0CbBufSa,prOutputInfo->u4OutBank0CrBufSa,
                                                        prOutputInfo->u4OutBank1YBufSa,prOutputInfo->u4OutBank1CbBufSa,prOutputInfo->u4OutBank1CrBufSa,
                                                        u4FrameBufWidth/4,u4FrameBufHeight,
                                                        u4FrameBufWidth/4,u4FrameBufHeight,
                                                        0,0,
                                                        u4WorkaroundFrameBufY,u4WorkaroundFrameBufC,
                                                        prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                                        prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                                        prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                                        prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                                        prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                                        prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                                        FALSE);
                }
                else
                {
                    vImgReszVfyJpegPicModeInit(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                                    prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                                    prOutputInfo->u4OutBank0YBufSa,prOutputInfo->u4OutBank0CbBufSa,prOutputInfo->u4OutBank0CrBufSa,
                                                    prOutputInfo->u4OutBank1YBufSa,prOutputInfo->u4OutBank1CbBufSa,prOutputInfo->u4OutBank1CrBufSa,
                                                    u4FrameBufWidth,u4FrameBufHeight,
                                                    (_fgJdecVfyTestPicModeAYUVOutput) ? (u4FrameBufWidth/4) : u4FrameBufWidth,
                                                    u4FrameBufHeight,
                                                    0,0,
                                                    u4FrameBufY,u4FrameBufC,
                                                    prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                                    prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                                    prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                                    prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                                    prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                                    prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                                    _fgJdecVfyTestPicModeAYUVOutput);
                }
            }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            {
                BOOL fgYExist;
                BOOL fgCbExist;
                BOOL fgCrExist;
                _fgGetResizer = FALSE;

                if(_prJPEG_JFIF_HEADER->rSOS.u1Se ==63 && _prJPEG_JFIF_HEADER->rSOS.u1Al == 0)
                {
                    _fgGetResizer = TRUE;
                    if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                    {
                        fgYExist = FALSE;
                        fgCbExist = FALSE;
                        fgCrExist = FALSE;

                        switch(_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0])
                        {
                            case 0:
                                fgYExist = TRUE;
                                break;
                            case 1:
                                fgCbExist = TRUE;
                                break;
                            case 2:
                                fgCrExist = TRUE;
                                break;
                        }
                    } else
                    {
                        fgYExist = TRUE;
                        fgCbExist = TRUE;
                        fgCrExist = TRUE;
                    }
                    vImgReszVfyJpegScanModeInit(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                              prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                              prOutputInfo->u4OutBank0YBufSa,prOutputInfo->u4OutBank0CbBufSa,prOutputInfo->u4OutBank0CrBufSa,
                                              prOutputInfo->u4OutBank1YBufSa,prOutputInfo->u4OutBank1CbBufSa,prOutputInfo->u4OutBank1CrBufSa,
                                              u4FrameBufWidth,u4FrameBufHeight,
                                                (_fgJdecVfyTestPicModeAYUVOutput) ? (u4FrameBufWidth/4) : u4FrameBufWidth,//u4FrameBufWidth,
                                                u4FrameBufHeight,
                                              0,0,
                                              u4FrameBufY,u4FrameBufC,
                                              prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                                _fgJdecVfyTestPicModeAYUVOutput,//FALSE,
                                                fgYExist,
                                              fgCbExist,fgCrExist);
                 }
            }
            if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
            {
                BOOL fgYExist;
                BOOL fgCbExist;
                BOOL fgCrExist;

                _fgGetResizer = TRUE;
                if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                {
                    fgYExist = FALSE;
                    fgCbExist = FALSE;
                    fgCrExist = FALSE;

                    switch(_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0])
                    {
                        case 0:
                           fgYExist = TRUE;
                           break;
                        case 1:
                           fgCbExist = TRUE;
                           break;
                        case 2:
                           fgCrExist = TRUE;
                           break;
                    }
                }
                else
                {
                    fgYExist = TRUE;
                    fgCbExist = TRUE;
                    fgCrExist = TRUE;
                }
                vImgReszVfyJpegScanModePreloadInit(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                              prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                              prOutputInfo->u4OutBank0YBufSa,prOutputInfo->u4OutBank0CbBufSa,prOutputInfo->u4OutBank0CrBufSa,
                                              prOutputInfo->u4OutBank1YBufSa,prOutputInfo->u4OutBank1CbBufSa,prOutputInfo->u4OutBank1CrBufSa,
                                              u4FrameBufWidth,u4FrameBufHeight,
                                                    (_fgJdecVfyTestPicModeAYUVOutput) ? (u4FrameBufWidth/4) : u4FrameBufWidth,
                                                   u4FrameBufHeight,
                                              0,0,
                                              u4FrameBufY,u4FrameBufC,
                                              prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                              prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                                   _fgJdecVfyTestPicModeAYUVOutput,//FALSE,
                                                   fgYExist,
                                              fgCbExist,fgCrExist,
                                              !_fgFirstScan);
            }
#endif
            if (_fgJdecVfyTestInterrupt)
            {
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }
             if(!_fgJdecVfyTestRingFifo)
                 vJdecVfyMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE);
             if(_fgJdecVfyTestPicModeAYUVOutput)
              {
                vJdecVfyMapBuf((UINT32) u4FrameBufY, u4FrameBufWidth*u4FrameBufHeight,  FROM_DEVICE);//FROM_DEVICE);
              }
             else
             {
                vJdecVfyMapBuf((UINT32) u4FrameBufY, u4FrameBufWidth*u4FrameBufHeight, FROM_DEVICE);// FROM_DEVICE);
                vJdecVfyMapBuf((UINT32) u4FrameBufC, u4FrameBufWidth*u4FrameBufHeight, FROM_DEVICE); //FROM_DEVICE);
             }
              i4Ret= i4JDEC_HAL_Decode(u4JDecID); // UTIL_Printf("i4JDEC_HAL_Decode return %d\n", i4Ret);

#if 1 // for test : don't wait picture interrupt,only polling image resizer finish flag
            if (_fgJdecVfyTestInterrupt)
            {
                INT32 i4Result;
                eJpegEvent = JPEG_EV_IRQ;
                i4Ret = x_ev_group_wait_event_timeout(_hJdecEventHandle, eJpegEvent, &eJpegEventGet, X_EV_OP_OR_CONSUME, /*10000*/90000);
#if PERFORMANCE_TEST

                 HAL_GetTime(&_rJdecEndTime);
                 HAL_GetDeltaTime(&_rJdecTotalTime, &_rJdecStratTime, &_rJdecEndTime);                  
#endif            
			   if(i4Ret != OSR_OK)
               {
                   extern void vHwJdecPrintRegister(UINT32 u4HwId);
				  _fgEnableRegisterLog = TRUE;
				   UTIL_Printf("[JDEC_VFY]---decode timeout--\n ");				  
                  vHwJdecPrintRegister(u4JDecID);
                   return -1;
                }
			    UTIL_Printf("[JDEC_VFY]---decode done--\n ");			  
                //i4JdecVfyPrintf("[JDEC][Verify] Wait IRQ , return %d\n",i4Ret);
                //VERIFY(i4Ret == OSR_OK);

                i4Result = i4JDEC_HAL_GET_Error_Type(u4JDecID);
               // i4JdecVfyPrintf("[JDEC][Verify] Decode Error %x\n",i4Result);
                if (i4Result > 0)
                {
                     extern void vHwJdecPrintRegister(UINT32 u4HwId);
				   	 if(!_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 2;
					 }
                     vHwJdecPrintRegister(u4JDecID);
					 if(2==_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 0;
					 }
                    i4JdecVfyPrintf("[JDEC][Verify] Decode Error %x\n",i4Result);
                    #if CONFIG_JPG_FATS_SUPPORT
                    FATS_IF_SaveLog(1,"[Jpeg][Verify]  %s Decode Error %x",_abJpegFileList[_u4CurJpegFileListNum],i4Result);
                    #endif
                    //while(TRUE);
                    return -1;
                }
                else
                {
                   #if 0
                    i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID);
                   #else
                    while(TRUE)
                    {
                        if(i4JDEC_HAL_Get_Decode_Status(u4JDecID) >= 0)
                            break;//Temp solution!
                    }
                   #endif
                }
            }
            else
            {
            	UINT32 u4LoopCnt=0;
                while(TRUE)
                {                	
                    if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                    {
                        UTIL_Printf("Scan Return Here!\n");
						//vHwJdecPrintRegister(u4JDecID);
                        //ASSERT(0);
                         if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC)
                        {
                              //extern void vHwJdecPrintRegister(UINT32 u4HwId);
           				      UTIL_Printf("[JDEC_VFY]---decode done--\n");
                              //vHwJdecPrintRegister(u4JDecID);
                           					
                        }
                        break;
                    }

                    if (_fgJdecVfyTestPicModeStop)
                    {
                        if (_u4CurJpegFileListNum % 2 == 1)
                        {
                            // stop the picture mode decoding
                            UINT32 u4Loop = 0;
                            UINT32 u4Count = 0;
                            for (u4Loop = 0; u4Loop < 100; u4Loop ++)
                            {
                                u4Count ++;
								                x_thread_delay(10);
                            }
                            i4JdecVfyPrintf("[JDEC][Verify] Test picture mode stop, %x\n",u4Count);

                            vJDEC_HAL_StopPicModeDec(u4JDecID);						
                            break;
                        }
                    }
                    if(_fgJdecVfyTestRingFifo)
                    {     
                    	 UINT32 u4RPtr = u4JDEC_HAL_Get_VLD_RPtr(u4JDecID);
                    	 UINT32 u4WPtr =  u4JDEC_HAL_Get_VLD_WPtr(u4JDecID);                   
                        if((u4RPtr /*& 0xFFFFFFF0*/) == (u4WPtr/* & 0xFFFFFFF0*/))
                        {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
                          vJdecLog_L("[JDEC_TRSA] RPTR[0x%x], WPTR[0x%x] FIFO[0x%x~0x%x]\n",u4RPtr,u4WPtr, _EmuRingFifo,_EmuRingFifo + JDEC_RING_FIFO_SIZE); 
                          if((u4RPtr /*& 0xFFFFFFF0*/) == (((UINT32)_EmuRingFifo + JDEC_RING_FIFO_SIZE/2)/*& 0xFFFFFFF0*/))
        				  {
        				          i4JDEC_HAL_Set_Write_Pointer(u4JDecID,prInputInfo->u4InFifoSa);
                           }
         				   else
         				  {     
         				  	     if(u4RPtr == _EmuRingFifo)
         				  	     {
         		                   vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead,JDEC_RING_FIFO_SIZE);                                 
                                   i4JDEC_HAL_Set_Write_Pointer(u4JDecID,(UINT32)(_EmuRingFifo + JDEC_RING_FIFO_SIZE /2));
                                   vJdecLog_L("[JDEC_TRSA]  start trs\n");         				  	     
                                 }
         				  }
         				
         				  
#else
                            if((u4JDEC_HAL_Get_VLD_RPtr(u4JDecID) & 0xFFFFFFF0) == (PHYSICAL((UINT32)_EmuRingFifo + JDEC_RING_FIFO_SIZE/2)& 0xFFFFFFF0))
                                i4JDEC_HAL_Set_Write_Pointer(u4JDecID,prInputInfo->u4InFifoSa + prInputInfo->u4InFifoSz - 0x10);
                            else
                            {

                                vTxDataForRingTest((BYTE *)(_EmuRingFifo + JDEC_RING_FIFO_SIZE - 0x10) , (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, 0x10);
                                vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, (JDEC_RING_FIFO_SIZE - 0x10));
                                i4JDEC_HAL_Set_Write_Pointer(u4JDecID,(UINT32)(_EmuRingFifo + JDEC_RING_FIFO_SIZE /2));
                            }
#endif
                        }   
                        else
                        {
                        	/*
                        	 u4LoopCnt++;
                        	 if(u4LoopCnt > 1000)
                        	 {
                        	 	 extern void vHwJdecPrintRegister(UINT32 u4HwId);
					             if(!_fgEnableRegisterLog)
					             {
				                    _fgEnableRegisterLog = 2;
					             }
                 	             vHwJdecPrintRegister(u4JDecID);
			                     UTIL_Printf("[JDEC][Verify] update rdtr wrtp decode -time outtt------------\n");  
					             if(2==_fgEnableRegisterLog)
					             {
				                   _fgEnableRegisterLog = 0;
					             }
					             u4LoopCnt =0;
                        	 }*/
                        }                  
                    }
                    else
                    {
                        if(_fgJdecVfyTestWritePointerUpdate)
                        {
                             i4JDEC_HAL_Set_Write_Pointer(u4JDecID,prInputInfo->u4InFifoSa + prInputInfo->u4InFifoSz);
                        }
                    }

                }
            }
#endif
            while(TRUE)
            {
                if (_fgJdecVfyTestPicModeStop)
                {
                    UINT32 i4Cnt=0;
                    //if (_u4CurJpegFileListNum % 1 == 0)
					if (_u4CurJpegFileListNum % 2 == 1)
                    {
                       i4Cnt++;
					    if(i4Cnt >= 2000)
					    {
					     _fgEnableRegisterLog =2;
					      vHwJdecPrintRegister(0);
						  ASSERT(0);
					    }
                        if (i4JDEC_HAL_GET_STOP_STATUS(u4JDecID) >= 0)
                        {
                            i4JdecVfyPrintf("[JDEC][Verify] Pic Mode Stop \n");
                            //i4ImgReszVfyJpegPicModeStopResize(u4JDecID);
                            break;
                        }
                    }
                    else
                    {
                        if((i4Ret = i4ImgReszVfyJpegPicModeGetResizeStatus(u4JDecID)) >= 0)
                            break;
                    }
                }
                else
                {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                    if(_fgGetResizer)
                    {
                        if((i4Ret = i4ImgReszVfyJpegPicModeGetResizeStatus(u4JDecID)) >= 0)
                        {	            
                            break;
                    }
                    }
                    else
                        break;
#else
                    if((i4Ret = i4ImgReszVfyJpegPicModeGetResizeStatus(u4JDecID)) >= 0)
                        break;
#endif
                }

            }
            if ((_fgJdecVfyTestPicModeStop) && (_u4CurJpegFileListNum % 2 == 0))
            {
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
                if(_fgGetResizer)
                {
                    while(TRUE)
                    {
                        if((i4Ret = i4ImgReszVfyJpegPicModeGetResizeStatus(u4JDecID)) >= 0)
                            break;
                    }
                }
#else
                while(TRUE)
                {
                    if((i4Ret = i4ImgReszVfyJpegPicModeGetResizeStatus(u4JDecID)) >= 0)
                        break;
                }
#endif
            }

            vImgReszVfyJpegPicModeUninit(u4JDecID);

            if(_fgJdecVfyTestPicModeAYUVOutputWorkaround)
            {
                vImgReszVfyJpegPicModeWorkaroundYC2AYUV(u4JDecID,u4FrameBufWidth/4,
                                                                 u4FrameBufWidth/4,u4FrameBufHeight,
                                                                 u4WorkaroundFrameBufY,u4WorkaroundFrameBufC,
                                                                 u4FrameBufWidth,u4FrameBufHeight,
                                                                 u4FrameBufWidth/4,u4FrameBufHeight,
                                                                 0,0,
                                                                 u4FrameBufY);
                #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
                x_free_aligned_dma_mem((void *)u4WorkaroundFrameBufY);
                x_free_aligned_dma_mem((void *)u4WorkaroundFrameBufC);
                #else
                x_free_aligned_nc_mem((void *)u4WorkaroundFrameBufY);
                x_free_aligned_nc_mem((void *)u4WorkaroundFrameBufC);
                #endif
            }
	        if(!_fgJdecVfyTestRingFifo)
              vJdecVfyUnMapBuf(prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz, TO_DEVICE);
             if(_fgJdecVfyTestPicModeAYUVOutput)
              {
                 vJdecVfyUnMapBuf(u4FrameBufY, u4FrameBufWidth*u4FrameBufHeight, FROM_DEVICE);// BIDIRECTIONAL);//FROM_DEVICE);
              }
             else
             {
                 vJdecVfyUnMapBuf(u4FrameBufY,u4FrameBufWidth*u4FrameBufHeight,FROM_DEVICE);//  BIDIRECTIONAL);//FROM_DEVICE);
                 vJdecVfyUnMapBuf(u4FrameBufC, u4FrameBufWidth*u4FrameBufHeight,FROM_DEVICE);//  BIDIRECTIONAL);//FROM_DEVICE);
              }
              
              //    UTIL_Printf("0x%8x   ",u4HwJdecRead32(u4HwId,(u4Base + u4Temp * 4)));
               //   UTIL_Printf("0x%8x   ",u4HwJdecRead32(u4HwId,(u4Base + u4Temp * 4)));
            break;
        }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
        else if ( eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
         {
            EV_GRP_EVENT_T eJpegEvent;
            EV_GRP_EVENT_T eJpegEventGet;
            UINT32 i4Ret =0;
            if (_fgJdecVfyTestInterrupt)
            {
                vHwJdecSetInterrupt(u4JDecID, TRUE);
            }
		    if(!_fgJdecVfyTestRingFifo)
		   	{
              vJdecVfyMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz,TO_DEVICE);
		   	}
			vJdecVfyMapBuf((UINT32) u4FrameBufY, u4FrameBufWidth*u4FrameBufHeight,FROM_DEVICE);
            vJdecVfyMapBuf((UINT32) u4FrameBufC, u4FrameBufWidth*u4FrameBufHeight,FROM_DEVICE);
            vJdecVfyMapBuf(_u4ColorBufY,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE);
            vJdecVfyMapBuf(_u4ColorBufCb,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE);
            vJdecVfyMapBuf(_u4ColorBufCr,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE); 
                
            i4Ret =   i4JDEC_HAL_Decode(u4JDecID); UTIL_Printf("i4JDEC_HAL_Decode return %d\n", i4Ret);
            if (_fgJdecVfyTestInterrupt)
            {
                INT32 i4Result;
                eJpegEvent = JPEG_EV_IRQ;            
                i4Ret = x_ev_group_wait_event_timeout(_hJdecEventHandle, eJpegEvent, &eJpegEventGet, X_EV_OP_OR_CONSUME, /*10000*/90000);            
                  if(i4Ret != OSR_OK)
               {
                   extern void vHwJdecPrintRegister(UINT32 u4HwId);
				   UTIL_Printf("[JDEC_VFY]---decode timeout--\n ");
				    vHwJdecPrintRegister(u4JDecID);
                   return -1;
                }
			    UTIL_Printf("[JDEC_VFY]---decode done--\n ");			  
                i4Result = i4JDEC_HAL_GET_Error_Type(u4JDecID);
                if (i4Result > 0)
                {
                     extern void vHwJdecPrintRegister(UINT32 u4HwId);
				   	 if(!_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 2;
					 }
                     vHwJdecPrintRegister(u4JDecID);
					 if(2==_fgEnableRegisterLog)
					 {
				       _fgEnableRegisterLog = 0;
					 }
                    i4JdecVfyPrintf("[JDEC][Verify] Decode Error %x\n",i4Result);
                    return -1;
                }
                else
                {
                    i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID); 
					 if(_debug_decode_done_log)
			         {
			              UTIL_Printf("decode done==========\n");
                         vHwJdecPrintRegister(u4JDecID);
			         }
                 }
                #if PERFORMANCE_TEST
                 HAL_GetTime(&_rJdecEndTime);
                 HAL_GetDeltaTime(&_rJdecTotalTime, &_rJdecStratTime, &_rJdecEndTime);    
                 UTIL_Printf("The band width is %x\n",*(volatile UINT32 *)(IO_BASE + 0x00007190));
                #endif
              }
               else
              {       
                  while(TRUE)
                  {
                     if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                     {
                       if(_debug_decode_done_log)
			           {
			              UTIL_Printf("decode done==========\n");
                         vHwJdecPrintRegister(u4JDecID);
			           }
                       break;
                     }
                  }
              } 
			   
           if(!_fgJdecVfyTestRingFifo)
           	{
               vJdecVfyUnMapBuf((UINT32) prInputInfo->u4InFifoSa, prInputInfo->u4InFifoSz,TO_DEVICE);
           	}
		    vJdecVfyUnMapBuf(_u4ColorBufY,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE);
            vJdecVfyUnMapBuf(_u4ColorBufCb,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE);
            vJdecVfyUnMapBuf(_u4ColorBufCr,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), FROM_DEVICE);            
            vJdecVfyUnMapBuf((UINT32) u4FrameBufY, u4FrameBufWidth*u4FrameBufHeight, FROM_DEVICE);
            vJdecVfyUnMapBuf((UINT32) u4FrameBufC, u4FrameBufWidth*u4FrameBufHeight, FROM_DEVICE);			
            break;
         }
#endif       
    }
    if(0)//eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
	{
	   extern	 void vHwJdecPrintRegister_ex(UINT32 u4HwId);
	   vHwJdecPrintRegister_ex(u4JDecID);	
    }

    return i4Ret;
}


INT32 i4JdecVfyEnhanceBufMix(UINT32 u4JDecID,JDEC_HAL_PIC_INFO_T *prPicInfo,
                                             JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,
                                             UINT32 u4ColorBufY,UINT32 u4ColorBufCb,UINT32 u4ColorBufCr,
                                             UINT32 u4FrameBufY,UINT32 u4FrameBufC,
                                             UINT32 u4FrameBufWidth,UINT32 u4FrameBufHeight,
                                             UINT32 u4PitchPerRow)
{
    INT32 i4Ret=0;

#if JDEC_VFY_TEST_MCUROW_ENHANCE
    if(1)
#else
    if(_fgJdecVfyTestProgressiveEnhancePreload)
#endif
    {
        BOOL fgYExist,fgCbExist,fgCrExist;

        if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
        {
            fgYExist = FALSE;
            fgCbExist = FALSE;
            fgCrExist = FALSE;

            switch(_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0])
            {
                case 0:
                    fgYExist = TRUE;
                    break;
                case 1:
                    fgCbExist = TRUE;
                    break;
                case 2:
                    fgCrExist = TRUE;
                    break;
            }
        } else
        {
            fgYExist = TRUE;
            fgCbExist = TRUE;
            fgCrExist = TRUE;
        }
#if JDEC_VFY_TEST_MCUROW_ENHANCE
        vImgReszVfyJpegPreloadInitEnhanceMCUROW(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                                prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                                u4ColorBufY,u4ColorBufCb,u4ColorBufCr,
                                                u4FrameBufWidth,u4FrameBufHeight,
                                                (_fgJdecVfyTestPicModeAYUVOutput) ? (u4FrameBufWidth/4) : u4FrameBufWidth,
                                                u4FrameBufHeight,
                                                0,0,
                                                u4FrameBufY,u4FrameBufC,
                                                prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                                prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                                prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                                prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                                prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                                prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                                fgYExist,fgCbExist,fgCrExist,
                                                _fgJdecVfyTestPicModeAYUVOutput,
                                                _fgFirstScan
                                               );
#else 
        vImgReszVfyJpegPreloadInit(u4JDecID,prOutputInfo->u4OutYBufPitch,
                                            prPicInfo->prSofInfo->u2ImageWidth,prPicInfo->prSofInfo->u2ImageHeight,
                                            u4ColorBufY,u4ColorBufCb,u4ColorBufCr,
                                            u4FrameBufWidth,u4FrameBufHeight,
                                            u4FrameBufWidth,u4FrameBufHeight,
                                            0,0,
                                            u4FrameBufY,u4FrameBufC,
                                            prPicInfo->prSofInfo->arSofComp[0].u1HSampFactor,
                                            prPicInfo->prSofInfo->arSofComp[0].u1VSampFactor,
                                            prPicInfo->prSofInfo->arSofComp[1].u1HSampFactor,
                                            prPicInfo->prSofInfo->arSofComp[1].u1VSampFactor,
                                            prPicInfo->prSofInfo->arSofComp[2].u1HSampFactor,
                                            prPicInfo->prSofInfo->arSofComp[2].u1VSampFactor,
                                            fgYExist,fgCbExist,fgCrExist);

        while(TRUE)
        {
            if((i4Ret = i4ImgReszVfyJpegPreloadGetResizeStatus(u4JDecID)) >= 0)
                break;
        }
        vImgReszVfyJpegPreloadUninit(u4JDecID);

#endif
    }
    else
    {
        UINT32 u4I,u4Len = u4PitchPerRow*prPicInfo->prSofInfo->u2ImageHeight;
        INT32 i4Value1,i4Value2;

        if((_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 3) || (_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0] == 0))
        {
            for(u4I=0; u4I<u4Len; u4I++)
            {
                i4Value1 = *(UCHAR *)(u4ColorBufY+u4I);
                i4Value2 = *(UCHAR *)(_u4EnhanceBufY+u4I);

                i4Value2 = 0x80 + (i4Value2 - 0x80) + (i4Value1 - 0x80);
                if(i4Value2 > 0xFF)
                    i4Value2 = 0xFF;
                if(i4Value2 < 0)
                    i4Value2 = 0;

                *(UCHAR *)(_u4EnhanceBufY+u4I) = i4Value2;
            }
        }

        if((_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 3) || (_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0] == 1))
        {
            for(u4I=0; u4I<u4Len; u4I++)
            {
                i4Value1 = *(UCHAR *)(u4ColorBufCb+u4I);
                i4Value2 = *(UCHAR *)(_u4EnhanceBufCb+u4I);

                i4Value2 = 0x80 + (i4Value2 - 0x80) + (i4Value1 - 0x80);
                if(i4Value2 > 0xFF)
                    i4Value2 = 0xFF;
                if(i4Value2 < 0)
                    i4Value2 = 0;

                *(UCHAR *)(_u4EnhanceBufCb+u4I) = i4Value2;
            }
        }

        if((_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 3) || (_prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0] == 2))
        {
            for(u4I=0; u4I<u4Len; u4I++)
            {
                i4Value1 = *(UCHAR *)(u4ColorBufCr+u4I);
                i4Value2 = *(UCHAR *)(_u4EnhanceBufCr+u4I);

                i4Value2 = 0x80 + (i4Value2 - 0x80) + (i4Value1 - 0x80);
                if(i4Value2 > 0xFF)
                    i4Value2 = 0xFF;
                if(i4Value2 < 0)
                    i4Value2 = 0;

                *(UCHAR *)(_u4EnhanceBufCr+u4I) = i4Value2;
            }
        }
    }
    return i4Ret;
}
#if PERFORMANCE_TEST

INT32 i4FileHandleSavePerformanceTime;

#endif
BOOL fgJdecCalculateMcuRowWidthHeight(JDEC_JFIF_SOF_T *prSofInfo)      
{
    UINT32 u4CompId;
    for(u4CompId = 0; u4CompId<prSofInfo->u1NumComponents; u4CompId++)
    {
        UINT32 u4WidthPerMCU = (prSofInfo->arSofComp[0].u1HSampFactor /
                                prSofInfo->arSofComp[u4CompId].u1HSampFactor) * 8;
        UINT32 u4HeightPerMCU = (prSofInfo->arSofComp[0].u1VSampFactor /
                                 prSofInfo->arSofComp[u4CompId].u1VSampFactor) * 8;

        prSofInfo->au4ImgMCUWidth[u4CompId] = (prSofInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU;
        prSofInfo->au4ImgMCUHeight[u4CompId] = (prSofInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU;
        prSofInfo->au4ImgLastMCUHeight[u4CompId]= ((prSofInfo->u2ImageHeight +7) /8)%prSofInfo->arSofComp[u4CompId].u1VSampFactor;
	    if(prSofInfo->au4ImgLastMCUHeight[u4CompId]==0)
	    {
	       prSofInfo->au4ImgLastMCUHeight[u4CompId]=prSofInfo->arSofComp[u4CompId].u1VSampFactor;
	    }
		JDEC_VERY_HAL_LOG("prSofInfo->au4ImgMCUHeight[0x%x]:%x!\n",(UINT32)u4CompId, prSofInfo->au4ImgMCUHeight[u4CompId] );
		JDEC_VERY_HAL_LOG("prSofInfo->au4ImgLastMCUHeight[0x%x]:%x!\n",(UINT32)u4CompId, prSofInfo->au4ImgLastMCUHeight[u4CompId]);
    }
    prSofInfo->u4ImgComMCUWidth  = (prSofInfo->u2ImageWidth + prSofInfo->arSofComp[0].u1HSampFactor * 8 - 1) /
                     (prSofInfo->arSofComp[0].u1HSampFactor * 8);
    prSofInfo->u4ImgComMCUHeight = (prSofInfo->u2ImageHeight + prSofInfo->arSofComp[0].u1VSampFactor * 8 - 1) /
                      (prSofInfo->arSofComp[0].u1VSampFactor * 8);     
	JDEC_VERY_HAL_LOG("prSofInfo->u4ImgComMCUHeight:%x!\n",prSofInfo->u4ImgComMCUHeight);
	return TRUE;
}


void vJdecVfyDecode(UINT32 u4JDecID,JDEC_HAL_DEC_MODE_T eDecodingMode,UINT32 u4ColorBufY,UINT32 u4ColorBufCb,UINT32 u4ColorBufCr,
                                                                   UINT32 u4FrameBufY,UINT32 u4FrameBufC,
                                                                   UINT32 u4FrameBufWidth,UINT32 u4FrameBufHeight)
{
    BOOL fgFirstLoop = TRUE;
    JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };
    JDEC_HAL_INPUT_INFO_T rInputInfo = { 0 };
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo = { 0 };
    UINT32 u4MCUWidth=0,u4MCUHeight=0;
    UINT32 u4PitchPerRow =0;
    UINT32 u4ReadAddrBytes,u4ReadAddrBits;
    UINT32 u4ScanCnt;
    UINT32 u4StcStart,u4StcEnd,u4StcTotal;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
    UINT32 u4Temp;
   #endif
    u4StcStart = u4GetSTC();
    UTIL_Printf("eDecodingMode:%x!\n",eDecodingMode);

    i4JDEC_HAL_Init(u4JDecID, 0);
    i4JDEC_HAL_Set_Decoding_Mode(u4JDecID,eDecodingMode);
    rPicInfo.prSofInfo = &(_prJPEG_JFIF_HEADER->rSOF);
    rPicInfo.prHuffTblInfo = &(_prJPEG_JFIF_HEADER->rDHT);
    rPicInfo.prQTblInfo = &(_prJPEG_JFIF_HEADER->rDQT);
    rPicInfo.u4RestartInterval = _prJPEG_JFIF_HEADER->u2RestartInterval;
    i4JDEC_HAL_Set_Pic_Info(u4JDecID,&rPicInfo);
#if  JDEC_MM_MODE_ENABLE
	 if(JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS == eDecodingMode)
	 {
	    _fgMmmodeFirstMCUROW = TRUE;
	 }
     MM_MODE_RESCAN:   
#endif 	
	i4JDEC_HAL_Set_Scan_Info(u4JDecID,&(_prJPEG_JFIF_HEADER->rSOS));

    #if 0//JDEC_IO_MMU_TEST
    vIOMMU_OverRead_Protection(IOMMU_JPG, 1, (UINT32)_EmuJpgFifo, JDEC_FIFO_SIZE,
	 	                                          (UINT32)_EmuRingFifo,JDEC_RING_FIFO_SIZE);
    #endif
    if(_fgJdecVfyTestRingFifo)
    {
        //Copy the data from Jpg buffer to Ring fifo!
        _u4PreviousAdd = (UINT32)_prJPEG_JFIF_HEADER->pu1CurrRead;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, JDEC_RING_FIFO_SIZE);
#else
        vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, (JDEC_RING_FIFO_SIZE - 0x10));
#endif
        //Update Infomation
        rInputInfo.u4InFifoSa = (UINT32)_EmuRingFifo;
        rInputInfo.u4InFifoSz = JDEC_RING_FIFO_SIZE;
        rInputInfo.u4RdPoint = (UINT32)(_EmuRingFifo);
        if(_fgJdecVfyTestWritePointer)
            rInputInfo.u4WrPoint = rInputInfo.u4InFifoSa + JDEC_RING_FIFO_SIZE/2;
        else
            rInputInfo.u4WrPoint = rInputInfo.u4InFifoSa + rInputInfo.u4InFifoSz;
    }
    else
    {
        rInputInfo.u4InFifoSa = (UINT32)_EmuJpgFifo;
        rInputInfo.u4InFifoSz = JDEC_FIFO_SIZE;
        rInputInfo.u4RdPoint = (UINT32)(_prJPEG_JFIF_HEADER->pu1CurrRead);

        if(_fgJdecVfyTestWritePointer)
          #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
            rInputInfo.u4WrPoint = rInputInfo.u4InFifoSa + rInputInfo.u4InFifoSz -0x10;
          #else
            rInputInfo.u4WrPoint = rInputInfo.u4InFifoSa + _i4JpegFileSize/2;
          #endif
        else
            rInputInfo.u4WrPoint = rInputInfo.u4InFifoSa + rInputInfo.u4InFifoSz;
    }
    i4JDEC_HAL_Set_Input_Info(u4JDecID,&rInputInfo);

    if(_fgJdecVfyTestWritePointerUpdate)
    {
        i4JDEC_HAL_Set_Write_Pointer(u4JDecID,rInputInfo.u4WrPoint);
    }

    fgJdecCalculateMcuRowWidthHeight(rPicInfo.prSofInfo);
    if(fgFirstLoop)
    {
        UINT32 u4WidthPerMCU = (rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8);
        u4PitchPerRow = ((rPicInfo.prSofInfo->u2ImageWidth +
                                       u4WidthPerMCU - 1) /
                                       u4WidthPerMCU) * u4WidthPerMCU; // Align to u4WidthPerMCU
        u4PitchPerRow = ((u4PitchPerRow + 15) / 16) * 16; // Align to 16's
        _u4ColorBufWidth = u4PitchPerRow;
        rOutputInfo.u4OutYBufPitch = u4PitchPerRow;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) //matiching imgresz new feature
        if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
            || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
          #if JDEC_MM_MODE_ENABLE
             || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
          #endif
            || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
        {
            u4Temp = rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor;
            rOutputInfo.u4OutCbBufPitch = (((u4PitchPerRow /u4Temp) + 15) /16) * 16;
            u4Temp = rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor;
            rOutputInfo.u4OutCrBufPitch = (((u4PitchPerRow /u4Temp) + 15) /16) * 16;
         }else
         {
          rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
          rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
         }
    #else
          rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
          rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
    #endif
    
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
        /*for video ouput mode, only two output buffer*/
        if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
        {
           rOutputInfo.u4OutYBufSa = u4ColorBufY;
           rOutputInfo.u4OutCbBufSa = u4ColorBufCb;
           rOutputInfo.u4OutCrBufSa = u4ColorBufCb;
        }
        else
        {
           rOutputInfo.u4OutYBufSa = u4ColorBufY;
           rOutputInfo.u4OutCbBufSa = u4ColorBufCb;
           rOutputInfo.u4OutCrBufSa = u4ColorBufCr;
        }
    #else
        rOutputInfo.u4OutYBufSa = u4ColorBufY;
        rOutputInfo.u4OutCbBufSa = u4ColorBufCb;
        rOutputInfo.u4OutCrBufSa = u4ColorBufCr;
    #endif
    
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
         if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
           ||eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW)   //sun for 8560 new feature
         	{
         	   if(rPicInfo.prSofInfo->u1NumComponents > 1)
         	     i4JDEC_HAL_Set_Burst_Mode(u4JDecID,(rPicInfo.prSofInfo), FALSE);
    	       else
    	         i4JDEC_HAL_Set_Burst_Mode(u4JDecID,(rPicInfo.prSofInfo), FALSE);
         	}
            else
         	{
    	        i4JDEC_HAL_Set_Burst_Mode(u4JDecID,(rPicInfo.prSofInfo), FALSE);
         	}
    #endif
    
        if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
    #if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
          #if JDEC_MM_MODE_ENABLE
             || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS
          #endif  
           || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
    #else
           )
    #endif
        {
            rOutputInfo.u4OutBank0YBufSa = u4ColorBufY;
            rOutputInfo.u4OutBank0CbBufSa = u4ColorBufCb;
            rOutputInfo.u4OutBank0CrBufSa = u4ColorBufCr;
            rOutputInfo.u4OutBank1YBufSa = u4ColorBufY + 16*rOutputInfo.u4OutYBufPitch;
            rOutputInfo.u4OutBank1CbBufSa = u4ColorBufCb + 16*rOutputInfo.u4OutYBufPitch;
            rOutputInfo.u4OutBank1CrBufSa = u4ColorBufCr + 16*rOutputInfo.u4OutYBufPitch;
        }
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
          /*for video ouput mode, only two output buffer,reuse bank0 register*/
        if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT)
          {
            rOutputInfo.u4OutBank0YBufSa = u4ColorBufY;
            rOutputInfo.u4OutBank0CbBufSa = u4ColorBufCb;
            rOutputInfo.u4OutBank0CrBufSa = u4ColorBufCb;      
          }
    #endif

#if JDEC_MM_MODE_ENABLE
	     if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
	     {
	        rOutputInfo.u4ToalMCURow =rPicInfo.prSofInfo->u4ImgComMCUHeight;
			JDEC_VERY_HAL_LOG("rOutputInfo.u4ToalMCURow:%x!\n",rOutputInfo.u4ToalMCURow);
			rOutputInfo.ui4McuRowNumber = _ui4McuRowNumberPerTime;
			if(rOutputInfo.ui4McuRowNumber > rOutputInfo.u4ToalMCURow)
			{
			   rOutputInfo.ui4McuRowNumber = rOutputInfo.u4ToalMCURow;
			}
			JDEC_VERY_HAL_LOG("rOutputInfo.ui4McuRowNumber:%x!\n",rOutputInfo.ui4McuRowNumber);
	     }
#endif  
        #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo,&rOutputInfo);
        #else
        i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);
        #endif
    	fgFirstLoop = FALSE;
    }

    // Set default value ,don't use interrupt
    vHwJdecSetInterrupt(u4JDecID,FALSE);
    // Set enhance buffer as center value (0x80)
    if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
       || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE
       || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
#else
       )
#endif
    {
        if(_fgJdecVfyTestProgressiveEnhancePreload)
        {
 //   	 memset((void*)u4FrameBufY,0x80,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT));
 //           memset((void*)u4FrameBufC,0x80,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT));
        }
        else
        {
            x_memset((void*)_u4EnhanceBufY,0x80,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));    
            x_memset((void*)_u4EnhanceBufCb,0x80,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));    
            x_memset((void*)_u4EnhanceBufCr,0x80,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
            vJdecVfyMapBuf( _u4EnhanceBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyMapBuf( _u4EnhanceBufCb,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyMapBuf( _u4EnhanceBufCr,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE); 
            vJdecVfyUnMapBuf(_u4EnhanceBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyUnMapBuf(_u4EnhanceBufCb,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyUnMapBuf(_u4EnhanceBufCr,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE); 
        }
    }
   #if DEBUG_MMMMM_ON


   #endif
    // Scan loop
    u4ScanCnt = 0;
    do
    {
       UTIL_Printf("Start Decode Scan No. %d\n", u4ScanCnt);
#if DEBUG_MMMMM_ON
        _ui4ScanNumber =u4ScanCnt;
#endif
	   if((_prJPEG_JFIF_HEADER->rSOS.u1Ah !=0) && (_prJPEG_JFIF_HEADER->rSOS.u1Ss!=0))
     {
   	     //  UTIL_Printf("*********AC refine scan***************\n"); 
   	    }
	    //UTIL_Printf("%s 0xf0 = 0x%8x	\n", __FUNCTION__,u4HwJdecRead32(u4JDecID,0xF0));
    
        if (u4ScanCnt == 0)
	    _fgFirstScan = TRUE;
	    else
	    _fgFirstScan = FALSE;
        // Clean IDCT buffer to prevent MCU with EOBRUN != 0 do not output to IDCT buffer.
        if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
#else
           )
#endif
        {
            memset((void*)u4ColorBufY,0x80, (u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
            memset((void*)u4ColorBufCb,0x80,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
            memset((void*)u4ColorBufCr,0x80,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
			vJdecVfyMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyUnMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
            vJdecVfyUnMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE); 
            vJdecVfyUnMapBuf(u4ColorBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight), TO_DEVICE);
        }

        // Decide MCU width, MCU height
        if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
        {
            UINT32 u4CompIdx = _prJPEG_JFIF_HEADER->rSOS.au1CompIdx[0];
            u4MCUWidth = _prJPEG_JFIF_HEADER->rSOF.au4ImgMCUWidth[u4CompIdx];
            u4MCUHeight = _prJPEG_JFIF_HEADER->rSOF.au4ImgMCUHeight[u4CompIdx];
        } else
        {
            u4MCUWidth = _prJPEG_JFIF_HEADER->rSOF.u4ImgComMCUWidth;
            u4MCUHeight = _prJPEG_JFIF_HEADER->rSOF.u4ImgComMCUHeight;
        }
        // Decode one scan
        if((i4JdecVfyDecodeOneScan(u4JDecID,eDecodingMode,&rPicInfo,&rInputInfo,&rOutputInfo,
                                        u4FrameBufY,u4FrameBufC,u4FrameBufWidth,u4FrameBufHeight,
                                        u4MCUWidth,u4MCUHeight) < 0))
        {
           _fgSaveRes = TRUE;
		   UTIL_Printf("return for decoding failure. save res[%d]. %d\n", u4ScanCnt,_fgSaveRes);
		   return;
        }
        // Mix enhance buffer with IDCT buffer
        if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
           ||eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
#else
           )
#endif
           {
                i4JdecVfyEnhanceBufMix(u4JDecID,&rPicInfo,&rOutputInfo,
                                            u4ColorBufY,u4ColorBufCb,u4ColorBufCr,
                                            u4FrameBufY,u4FrameBufC,
                                            u4FrameBufWidth,u4FrameBufHeight,
                                            u4PitchPerRow);
            }

        // For progressvie picture, search next start of scan (SOS)
        if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
           (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
           #if JDEC_MM_MODE_ENABLE 
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
           #endif
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
#else
            )
#endif
        {
            JPEG_JFIF_HEADER_T *prJPEG_JFIF_HEADER = _prJPEG_JFIF_HEADER;
            JPEGHANDLE hParser;
		#if JDEC_MM_MODE_ENABLE  	
            JDEC_HAL_OUTPUT_INFO_T *prHalOuptPutInfo = prJDEC_HAL_Get_Output_Status(u4JDecID);
	   #endif	
            if(!_fgJdecVfyTestRingFifo)
            {
                i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);

                if(u4ReadAddrBits > 0)
                    u4ReadAddrBytes++;

                if(u4ReadAddrBytes >= (rInputInfo.u4InFifoSa + rInputInfo.u4InFifoSz))
                {
                    u4ReadAddrBytes -= rInputInfo.u4InFifoSa;
                }

               // UTIL_Printf("Read Pointer After One Scan:%x\n", (u4ReadAddrBytes- rInputInfo.u4InFifoSa));

                if(((UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead) - rInputInfo.u4InFifoSa) > _i4JpegFileSize)
                {
#if JDEC_MM_MODE_ENABLE  
                    GOTO_DECOD_1ST_SCAN			  	
#endif 
                    break;
                }

                //Check if reach FFD9,it yes ,break
                if ((fgHwJdecIsFoundMarker(u4JDecID))&& (0xD9 == u4HwJdecGetFoundMarker(u4JDecID)))
                {
#if JDEC_MM_MODE_ENABLE  
                    GOTO_DECOD_1ST_SCAN			  	
#endif 
                    break;
                }
                //if(u4ScanCnt == 1)
            //      break;
                //UTIL_Printf("Data For Next Parser:%x\n", u4ReadAddrBytes);
                hParser = (JPEGHANDLE)prJPEG_JFIF_HEADER;
                JFIF_New(hParser,(void*)u4ReadAddrBytes,_i4JpegFileSize);

                // When reach end of image, quit.
                if(prJPEG_JFIF_HEADER->fgEOI)
                {
               #if JDEC_MM_MODE_ENABLE
                    GOTO_DECOD_1ST_SCAN			  	
		#endif 
                    break;
                  }
                if((u4ReadAddrBytes - rInputInfo.u4InFifoSa) > _i4JpegFileSize)
                 {
               #if JDEC_MM_MODE_ENABLE
                    GOTO_DECOD_1ST_SCAN			  	
                  #endif 
                    break;
                 }
           }
           else
           {
                //Check if reach FFD9,it yes ,break
                if ((fgHwJdecIsFoundMarker(u4JDecID))&& (0xD9 == u4HwJdecGetFoundMarker(u4JDecID)))
                {
               #if JDEC_MM_MODE_ENABLE 
                    GOTO_DECOD_1ST_SCAN			  	
			   #endif 
                    break;
                }
                //UTIL_Printf("Data For Next Parser:%x\n", _prJPEG_JFIF_HEADER->pu1CurrRead - _EmuRingFifo);
                //_prJPEG_JFIF_HEADER->pu1CurrRead = (UINT8 *)_u4PreviousAdd;
                //UTIL_Printf("Start of Scan: %x \n", *(UINT32 *)_prJPEG_JFIF_HEADER->pu1CurrRead);
                //prJPEG_JFIF_HEADER->pu1CurrRead = _prJPEG_JFIF_HEADER->pu1CurrRead;
                hParser = (JPEGHANDLE)prJPEG_JFIF_HEADER;
                JFIF_New(hParser,(void*)(_u4PreviousAdd),_i4JpegFileSize);
                _prJPEG_JFIF_HEADER->pu1CurrRead = prJPEG_JFIF_HEADER->pu1CurrRead;
                _u4PreviousAdd = (UINT32)_prJPEG_JFIF_HEADER->pu1CurrRead;
              //  UTIL_Printf("Next Address for parser:%x\n", (_u4PreviousAdd - (UINT32)_EmuJpgFifo));

                // When reach end of image, quit.
                if(prJPEG_JFIF_HEADER->fgEOI)
                {
              #if JDEC_MM_MODE_ENABLE  
                    GOTO_DECOD_1ST_SCAN			  	
			   #endif 
                    break;
                  }
            }
		    u4ScanCnt++;
#if DEBUG_MMMMM_ON
           _ui4ScanNumber =u4ScanCnt;
#endif	
            i4JDEC_HAL_Set_HuffmanTable_Info(u4JDecID,&(prJPEG_JFIF_HEADER->rDHT));
            i4JDEC_HAL_Set_Scan_Info(u4JDecID,&(prJPEG_JFIF_HEADER->rSOS));
            if(!_fgJdecVfyTestRingFifo)
            {
               i4JDEC_HAL_Set_Read_Pointer(u4JDecID,(UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead));
            }
            else
            {
               //UTIL_Printf("Start of Scan: %x \n", *(UINT32 *)_prJPEG_JFIF_HEADER->pu1CurrRead);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
                vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, JDEC_RING_FIFO_SIZE);
#else
                vTxDataForRingTest(_EmuRingFifo, (BYTE *)_prJPEG_JFIF_HEADER->pu1CurrRead, (JDEC_RING_FIFO_SIZE - 0x10));
#endif
                i4JDEC_HAL_Set_Write_Pointer(u4JDecID,(UINT32)_EmuRingFifo + (UINT32)JDEC_RING_FIFO_SIZE /2);
                i4JDEC_HAL_Set_Read_Pointer(u4JDecID,(UINT32)_EmuRingFifo);      
	        }
         
        }
    } while((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
            (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
            ||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
         #if JDEC_MM_MODE_ENABLE
            ||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)  
         #endif   
            ||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            ||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            ||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE));
#else
            );
#endif

    // Copy enhance buffer data to IDCT buffer.
    if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
       || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
#else
       )
#endif
    {
        if(!_fgJdecVfyTestProgressiveEnhancePreload)
        {
            memcpy((void*)u4ColorBufY,(const void*)_u4EnhanceBufY,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
            memcpy((void*)u4ColorBufCb,(const void*)_u4EnhanceBufCb,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
            memcpy((void*)u4ColorBufCr,(const void*)_u4EnhanceBufCr,(u4PitchPerRow*rPicInfo.prSofInfo->u2ImageHeight));
        }
    }
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    i4JDEC_HAL_Uninit(u4JDecID, TRUE);
#else
    i4JDEC_HAL_Uninit(u4JDecID);
#endif

#if PERFORMANCE_TEST
    i4JdecVfyPrintf  ("[Jpeg][Verify] JDEC finish time=%d.%6d\n",_rJdecTotalTime.u4Seconds,_rJdecTotalTime.u4Micros); 
    i4JdecVfyFprintf(i4FileHandleSavePerformanceTime,"%d.%06d\n",_rJdecTotalTime.u4Seconds,_rJdecTotalTime.u4Micros); 
#endif

    u4StcEnd = u4GetSTC();
    u4StcTotal = u4StcEnd - u4StcStart;
    i4JdecVfyPrintf("[Jpeg][Verify] 0x%x mode decode 1 time\n",eDecodingMode);
}

void vJdecVfyGetIrQ(UINT16 u2Vector)
{
    INT32 i4Ret,i4ErrType;

    //UTIL_Printf("[JDEC_HAL] vJdecVfyGetIrQ===-\n");
    VERIFY(BIM_ClearIrq(u2Vector));
    i4ErrType = i4JDEC_HAL_GET_Error_Type(0);
    if (i4ErrType != 0)
    {
        _fgErrorDetect = TRUE;
		UTIL_Printf("[JDEC_HAL] decode IRQ coming- error-\n");
    }
	else
	{
		//UTIL_Printf("[JDEC_HAL] decode IRQ coming- sucess-\n");
		UTIL_Printf("Print Chksum:\n");
		i4JDEC_HAL_PrintChkSum(0);
	}
    i4Ret = x_ev_group_set_event(_hJdecEventHandle, JPEG_EV_IRQ, X_EV_OP_OR);
    VERIFY(i4Ret == OSR_OK);

}


BOOL fgJdecVfySetIrQ(void)
{
      
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)  // mark by pingzhao for 8520 verify compile fail
    x_os_isr_fct pfnOldIsr;
#endif

    if (_fgJdecSetIRQ)
    {
        return TRUE;
    }
    else
    {
        _fgJdecSetIRQ = TRUE;
    }

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520)  // mark by pingzhao for 8520 verify compile fail
	UTIL_Printf("[JDEC_HAL] fgJdecVfySetIrQ\n");
    // BIM_EnableIrq(VECTOR_JPGDEC);
	//UTIL_Printf("Enter %s! a\n", __FUNCTION__);
    //UTIL_Printf("0xf0008048 =0x%x\n",*(volatile UINT32 *)(0xf0008048));
    //UTIL_Printf("0xf000804c =0x%x\n",*(volatile UINT32 *)(0xf000804c));

    if (x_reg_isr(VECTOR_JPGDEC, vJdecVfyGetIrQ, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
	//UTIL_Printf("Enter %s! b\n", __FUNCTION__);
	//UTIL_Printf("0xf0008048 =0x%x\n",*(volatile UINT32 *)(0xf0008048));
    //UTIL_Printf("0xf000804c =0x%x\n",*(volatile UINT32 *)(0xf000804c));
#endif

    return TRUE;
}

extern void jpg_init_align_dbig_mem();
INT32 i4JdecVfyDecode(void)
{
    JPEGHANDLE hParser;
    jpg_init_align_dbig_mem();
    if(_prJPEG_JFIF_HEADER != NULL)    // Process header
    {
       JFIF_FreeInstance(_prJPEG_JFIF_HEADER);_prJPEG_JFIF_HEADER = NULL;
    }
    
    _prJPEG_JFIF_HEADER = JFIF_AllocInstance();
    VERIFY(_prJPEG_JFIF_HEADER != NULL);
    UTIL_Printf("{JDEC_VFY}_prJPEG_JFIF_HEADER =0x%x\n", (UINT32)_prJPEG_JFIF_HEADER);
    hParser = (JPEGHANDLE)_prJPEG_JFIF_HEADER;
	
    x_memset((void *)_prJPEG_JFIF_HEADER,0,sizeof(JPEG_JFIF_HEADER_T));

    JFIF_New(hParser,_EmuJpgFifo,JDEC_FIFO_SIZE);

    if((_prJPEG_JFIF_HEADER->rSOF.u2ImageWidth == 0) || (_prJPEG_JFIF_HEADER->rSOF.u2ImageHeight == 0))
        return -1;

    // Image info update
    _u4ImageWidth = _prJPEG_JFIF_HEADER->rSOF.u2ImageWidth;
    _u4ImageHeight = _prJPEG_JFIF_HEADER->rSOF.u2ImageHeight;

    _u4XSampleY = 1;
    _u4YSampleY = 1;
    if(_prJPEG_JFIF_HEADER->rSOF.u1NumComponents>1)
    {
        _u4XSampleCb = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1HSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1HSampFactor;
        _u4YSampleCb = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1VSampFactor;
    }
    if(_prJPEG_JFIF_HEADER->rSOF.u1NumComponents>2)
    {
        _u4XSampleCr = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1HSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1HSampFactor;
        _u4YSampleCr = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1VSampFactor;
    }

    if(_fgJdecVfyTestVideoOutMode)
    {
       if(_prJPEG_JFIF_HEADER->rSOF.u1NumComponents == 1)
        {
             i4JdecVfyPrintf("[Jpeg][Verify]video mode dose not support  this format \n");
             return -1;
         }     
       
       if((_u4XSampleCb==2)&&(_u4YSampleCb==1)&&(_u4XSampleCr==2)&&(_u4YSampleCr==1)) //422h
          _u4ImageHeight =((_u4ImageHeight + 7) / 8) * 8;
       else if ((_u4XSampleCb==2)&&(_u4YSampleCb==2)&&(_u4XSampleCr==2)&&(_u4YSampleCr==2)) //420
          _u4ImageHeight =((_u4ImageHeight + 15) >>4) <<4;
       else
          {
             i4JdecVfyPrintf("[Jpeg][Verify]video mode dose not support  this format \n");
             return -1;
          }
    }
    x_memset((void*)_u4FrameBufY,0x12,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT));    
    x_memset((void*)_u4FrameBufC,0x82,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT));
    #ifdef JDEC_USE_MAP_UNMAP
    vJdecVfyMapBuf(  _u4FrameBufY,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT), FROM_DEVICE);
    vJdecVfyMapBuf(  _u4FrameBufC,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT), FROM_DEVICE);
    vJdecVfyUnMapBuf(_u4FrameBufY,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT), FROM_DEVICE);
    vJdecVfyUnMapBuf(_u4FrameBufC,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT), FROM_DEVICE);
    #endif
    UTIL_Printf("[JDEC_VFY]eJpegFormat =0x%x\n",_prJPEG_JFIF_HEADER->rSOF.eJpegFormat);
	// Do decode
    if(_fgJdecVfyTestPicMode)
    {
       x_memset((void*)_u4ColorBufY,0x11,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
       x_memset((void*)_u4ColorBufCb,0x81,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
       x_memset((void*)_u4ColorBufCr,0x81,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
       #ifdef JDEC_USE_MAP_UNMAP
       vJdecVfyMapBuf(  _u4ColorBufY, (JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
       vJdecVfyMapBuf(  _u4ColorBufCb,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
       vJdecVfyMapBuf(  _u4ColorBufCr,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE); 
       vJdecVfyUnMapBuf(_u4ColorBufY, (JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
       vJdecVfyUnMapBuf(_u4ColorBufCb,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
       vJdecVfyUnMapBuf(_u4ColorBufCr,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
       #endif
        if (_fgJdecVfyTestInterrupt)
        {
            fgJdecVfySetIrQ();
        }
        vJdecVfyDecode(0,JDEC_HAL_DEC_MODE_BASELINE_PIC,_u4ColorBufY,_u4ColorBufCb,_u4ColorBufCr,
                                                               _u4FrameBufY,_u4FrameBufC,
                                                               JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
    }
    else
    {
        JDEC_HAL_DEC_MODE_T eDecodingMode;

        if(_prJPEG_JFIF_HEADER->rSOF.eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
        {
            if (_fgJdecVfyTestInterrupt)
            {
                fgJdecVfySetIrQ();
            }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)            
            if(_fgJdecVfyTestProgressiveScanMultiColltet)
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT;
#if JDEC_MM_MODE_ENABLE
			else if(_fgJdecVfyTestProgressiveMCUMulitCollectMultiPass)
			{
			   eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS;
			}
#endif 
            else
            {
                if(_fgJdecVfyTestProgressiveScanEnhance)
                    eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE;
                else
                {
#endif
                    if(_fgJdecVfyTestProgressiveMCUEnhance)
			   eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE;
                    else
                     {
		      	  if(_fgJdecVfyTestProgressiveMCUMultiColltet)
			    eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT;
			   else
			   {
#if JDEC_HAL_MT8550
                           if(_fgJdecVfyTestProgressiveEnhance)
                               eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE;
                           else
                               eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT;
#endif
                            }
                     }
                }
            }

         }
        else
        {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
            if (_fgJdecVfyTestVideoOutMode)
              eDecodingMode = JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT;
            else
#endif
               eDecodingMode = JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW;
            if (_fgJdecVfyTestInterrupt)
            {
                fgJdecVfySetIrQ();
            }
        }
        x_memset((void*)_u4ColorBufY,0xCD,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void*)_u4ColorBufCb,0xCD,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void*)_u4ColorBufCr,0xCD,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        vJdecVfyMapBuf(  _u4ColorBufY, (JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
        vJdecVfyMapBuf(  _u4ColorBufCb,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE);
        vJdecVfyMapBuf(  _u4ColorBufCr,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT), TO_DEVICE); 
        vJdecVfyDecode(0,eDecodingMode,_u4ColorBufY,_u4ColorBufCb,_u4ColorBufCr,
                       _u4FrameBufY,_u4FrameBufC,
                       JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
    }

    return 0;
}

//sun add for checksum
// Destruct 32 bit CRC table
UINT32  Table_CRC[256];
const UINT32 cnCRC_32 = 0x04C10DB7;

void BuildTable32( unsigned long aPoly )
{
   UINT32 i, j;
   UINT32 nData;
   UINT32 nAccum;

   for ( i = 0; i < 256; i++ )
   {
	 nData = ( unsigned long )( i << 24 );
	 nAccum = 0;
	 for ( j = 0; j < 8; j++ )
	 {
	  if ( ( nData ^ nAccum ) & 0x80000000 )
	   nAccum = ( nAccum << 1 ) ^ aPoly;
	  else
	   nAccum <<= 1;
	   nData <<= 1;
	  }
	Table_CRC[i] = nAccum;
   }
 }

// Caculate 32 bit CRC table
UINT32 CRC_32( BYTE * aData, UINT32 aSize )
{
  UINT32 i;
  UINT32 nAccum = 0;
  BuildTable32( cnCRC_32 );
  for ( i = 0; i < aSize; i++ )
  nAccum = ( nAccum << 8 ) ^ Table_CRC[( nAccum >> 24 ) ^ *aData++];
  return nAccum;
}

UINT32 u4Jdec32BitAccumulator(UINT32 u4BufAddr, UINT32 u4XSample, UINT32 u4YSample,  UINT32 u4Width, UINT32 u4Height )
{
  UINT32 u4X,u4Y;
  UINT8 bColor;
  UINT32 nAccum = 0;
  i4JdecVfyPrintf("[Jpeg][Verify] %s : u4Width= 0x%x, u4Height = 0x%x, \n",__FUNCTION__,u4Width,u4Height);
  for(u4Y=0; u4Y<u4Height; u4Y++)
       for(u4X=0; u4X<u4Width; u4X++)
        {
            // Get HW decoded color
            bColor = *(UINT8 *)(u4BufAddr + (u4Y>>u4YSample)*u4Width + (u4X>>u4XSample));
	     nAccum = nAccum + bColor;
        }
  return nAccum;
}

//sun end
//extern UINT32  _u4Delay_test_cnt;
INT32 i4JpegCompareBuf(void)
{
   // UINT32 u4X,u4Y;
    UINT32 u4ColorBufY = _u4ColorBufY;
    UINT32 u4ColorBufCb = _u4ColorBufCb;
    UINT32 u4ColorBufCr = _u4ColorBufCr;
    UINT32 u4GoldenBufY = _u4ColorBufY2;
    UINT32 u4GoldenBufCb = _u4ColorBufCb2;
    UINT32 u4GoldenBufCr = _u4ColorBufCr2;
    UINT32 u4Tempy;

    UINT32 u1Sum = 0;
   // UINT8 u1CompareSum = 1;
    //UINT8 bColor;
    UINT32 bGoldenColor = 0;

    _u4DiffCnt = 0;
   //x_thread_delay(1500);
    //INT32 i4FileHandle;
   //i4FileHandle = i4JdecVfyFopen("testlog.txt","w");
   if(_ui4TestDelay)
      x_thread_delay(_ui4TestDelay);
   if(_fgJdecVfyTestVideoOutMode)
   {
       UINT32 u1CompareSumY=0;
	   UINT32 u1CompareSumC=0;	   
       // Compare Y
       //caculate hw checksum
       u1Sum = u4Jdec32BitAccumulator(u4ColorBufY,0,0,_u4ColorBufWidth,_u4ImageHeight);
       // Get Goldedn color
       bGoldenColor = *(UINT32*)(u4GoldenBufY);
       //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum);  
       u1CompareSumY = bGoldenColor - u1Sum ;

       if (u1CompareSumY)
           i4JdecVfyPrintf("[Jpeg][Verify] Compare Y fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumY);
       else
        {
           u1Sum = 0;
           bGoldenColor = 0;
        }

      // For one component jpg ,only compare the Y buffer
      if (_prJPEG_JFIF_HEADER->rSOF.u1NumComponents == 1)
      {
          if(u1CompareSumY)
              return -1;
          else
              return 0;
      }

       // Compare C
       //caculate hw checksum
       u1Sum = u4Jdec32BitAccumulator(u4ColorBufCb,0,0,_u4ColorBufWidth,(_u4ImageHeight/2));
       // Get Goldedn color
       bGoldenColor = *(UINT32*)(u4GoldenBufCb);
       //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum); 
       u1CompareSumC = bGoldenColor - u1Sum ;

       if (u1CompareSumC)
           i4JdecVfyPrintf("[Jpeg][Verify] Compare C fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumC);
       else
        {
           u1Sum = 0;
           bGoldenColor = 0;
        }

        if(u1CompareSumC ||u1CompareSumC)
           return -1;
        else
           return 0;   
   }
  else if(_fgJdecVfyTestPicMode ||_fgJdecVfyTestProgressiveScanMultiColltet ||_fgJdecVfyTestProgressiveScanEnhance)
  {
       UINT32 u1CompareSumY=0;
	   UINT32 u1CompareSumC=0;	  
	   //UINT32 u1CompareSumCr=0;	  
	   
        // Compare Y
       //caculate hw checksum
       
       u1Sum = u4Jdec32BitAccumulator(_u4FrameBufY,0,0,JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
	#if SAVE_GOLDEN_CHECK_SUM
       _i4FrameCChecksum = u1Sum;
	#endif
       // Get Goldedn color
       bGoldenColor = *(UINT32*)(u4GoldenBufY);
       //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x,,real sum =0x%x\n",bGoldenColor,u1Sum);  
       u1CompareSumY = bGoldenColor - u1Sum ;

       if (u1CompareSumY)
           i4JdecVfyPrintf("[Jpeg][Verify] Compare Y fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumY);
       else
        {
 	       u1Sum = 0;
 	       bGoldenColor = 0;
        }

      // For one component jpg ,only compare the Y buffer
      if (_prJPEG_JFIF_HEADER->rSOF.u1NumComponents == 1)
      {
          if(u1CompareSumY)
              return -1;
          else
              return 0;
      }
       // Compare C
       //caculate hw checksum
       u1Sum = u4Jdec32BitAccumulator(_u4FrameBufC,0,0,JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
#if SAVE_GOLDEN_CHECK_SUM
       _i4FrameCChecksum = u1Sum;
#endif
       // Get Goldedn color
       bGoldenColor = *(UINT32*)(u4GoldenBufCb);
       //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum);  
       u1CompareSumC = bGoldenColor - u1Sum ;

       if (u1CompareSumC)
           i4JdecVfyPrintf("[Jpeg][Verify] Compare C fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumC);
       else
        {
	  	   u1Sum = 0;
	  	   bGoldenColor = 0;
        }

        if(u1CompareSumC||u1CompareSumY)
           return -1;
        else
           return 0;

  }
  else
  {
       UINT32 u1CompareSumY=0;
	   UINT32 u1CompareSumCb=0;	  
	   UINT32 u1CompareSumCr=0;	  
       // Compare Y
       //caculate hw checksum
         u1Sum = u4Jdec32BitAccumulator(u4ColorBufY,0,0,_u4ColorBufWidth,_u4ImageHeight);
            // Get Goldedn color
          bGoldenColor = *(UINT32*)(u4GoldenBufY);
	   //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum);  
	   u1CompareSumY = bGoldenColor - u1Sum ;

         if (u1CompareSumY)
          i4JdecVfyPrintf("[Jpeg][Verify] Compare Y fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumY);
         else
	    {
	       u1Sum = 0;
	       bGoldenColor = 0;
            }
    // For one component jpg ,only compare the Y buffer
    if (_prJPEG_JFIF_HEADER->rSOF.u1NumComponents == 1)
    {
        if(u1CompareSumY)
            return -1;
        else
            return 0;
    }
	/*
    else
    {
         if(u1CompareSum)
            return -1;
    }*/

      if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
        u4Tempy = 1;
      else
    u4Tempy = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor / _prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1VSampFactor;

   //Compare Cb
  	 //caculate hw checksum
     u1Sum = u4Jdec32BitAccumulator(u4ColorBufCb,0,0,_u4ColorBufWidth,_u4ImageHeight/u4Tempy);
     // Get Goldedn color
     bGoldenColor = *(UINT32*)(u4GoldenBufCb);
	 //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum); 
     u1CompareSumCb = bGoldenColor - u1Sum ;

     if (u1CompareSumCb)
     {
          i4JdecVfyPrintf("[Jpeg][Verify] Compare Cb fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumCb);
     }
     else
	 {
	       u1Sum = 0;
	       bGoldenColor = 0;
      }

      if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
         u4Tempy = 1;
      else
       u4Tempy = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor / _prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1VSampFactor;

    //Compare Cr
   	 //caculate hw checksum
     u1Sum = u4Jdec32BitAccumulator(u4ColorBufCr,0,0,_u4ColorBufWidth,_u4ImageHeight/u4Tempy);
     // Get Goldedn color
     bGoldenColor = *(UINT32*)(u4GoldenBufCr);
     //i4JdecVfyPrintf("[Jpeg][Verify] the golden color = %x, real sum =0x%x\n",bGoldenColor,u1Sum); 
     u1CompareSumCr = bGoldenColor - u1Sum ;

      if (u1CompareSumCr)
            i4JdecVfyPrintf("[Jpeg][Verify] Compare Cr fail, golden/real sum = %x:%x, check sum = %x, \n",bGoldenColor,u1Sum,u1CompareSumCr);
      else
	  {
	       u1Sum = 0;
	       bGoldenColor = 0;
       }


     if(u1CompareSumCr||u1CompareSumCb||u1CompareSumY)
        return -1;
     else
        return 0;
  }
}


void vParseJpegFileList(UINT32 u4BufSizeLimit)
{
    UINT32 u4I;
    BOOL fgInFileName;

    u4I = 0;
    _u4JpegFileListNum = 0;
    fgInFileName = FALSE;
    while(_pbJpegFileList[u4I] != 0)
    {
        if(!fgInFileName)
        {
            _abJpegFileList[_u4JpegFileListNum] = &(_pbJpegFileList[u4I]);
            _u4JpegFileListNum++;
            fgInFileName = TRUE;
        }

        if(_pbJpegFileList[u4I] == 0x0D)
            _pbJpegFileList[u4I] = 0;

        if(_pbJpegFileList[u4I] == 0x0A)
        {
            _pbJpegFileList[u4I] = 0;
            fgInFileName = FALSE;
        }

        u4I++;
        if((u4I == JpegFileListMax*JDEC_FILE_NAME_LEN) || (u4I == u4BufSizeLimit))
            break;
    }
}





INT32 i4LoadJpegFileList(void)
{
i4JdecVfyPrintf("Enter i4LoadJpegFileList \n");
#if  CONFIG_JPG_FATS_SUPPORT ||(JDEC_VFY_FILE_IO_TYPE==JDEC_VFY_FILE_IO_SEMI_HOSTING)
  if(_fgJdecVfyTestVideoOutMode)
  {
    return i4JdecVfyReadFile("D:\\Projects\\MT8560_emu\\JPGDEC\\goldengen\\v_jpeglist.txt",(UINT32)_pbJpegFileList,JpegFileListMax*JDEC_FILE_NAME_LEN);
  }
  else if(_fgTestBasliePic)
    return i4JdecVfyReadFile("D:\\Projects\\MT8560_emu\\JPGDEC\\goldengen\\b_jpeglist.txt",(UINT32)_pbJpegFileList,JpegFileListMax*JDEC_FILE_NAME_LEN);
  else
  	return i4JdecVfyReadFile("D:\\Projects\\MT8560_emu\\JPGDEC\\goldengen\\p_jpeglist.txt",(UINT32)_pbJpegFileList,JpegFileListMax*JDEC_FILE_NAME_LEN);
#else if (JDEC_VFY_FILE_IO_TYPE==JDEC_READ_USB_DIRECT)
    return i4JdecVfyReadFile("/mnt/usb/JPGDEC/goldengen/jpeglist.txt",(UINT32)_pbJpegFileList,JpegFileListMax*JDEC_FILE_NAME_LEN);
#endif
}


INT32 i4LoadJpegFile(void)
{
      UINT32  i4eadSize =0;
#if !JDEC_IO_MMU_TEST
       i4eadSize = i4JdecVfyReadFile((char *)_abJpegFileList[_u4CurJpegFileListNum],(UINT32)_EmuJpgFifo,JDEC_FIFO_SIZE);
#else
        //INT32 i4ret;
        i4eadSize = i4JdecVfyReadFile((char *)_abJpegFileList[_u4CurJpegFileListNum],(UINT32)_EmuJpgFifoback,JDEC_FIFO_SIZE);
        memcpy((char *)_EmuJpgFifo,(char *)_EmuJpgFifoback, JDEC_FIFO_SIZE);
        //return i4ret;
#endif
        _i4OrgJpegFilesize =  i4eadSize;

        vJdecVfyMapBuf((UINT32)_EmuJpgFifo, JDEC_FIFO_SIZE, TO_DEVICE);
        vJdecVfyUnMapBuf((UINT32)_EmuJpgFifo, JDEC_FIFO_SIZE, TO_DEVICE);
        return i4eadSize;
}


INT32 i4SaveColorBuf(void)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;
    JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };
	UINT32 u4Tempy =1;
    //char GoldenFileName[JDEC_FILE_NAME_LEN];
    // UINT32 u4CheckSum = 0;
	rPicInfo.prSofInfo = &(_prJPEG_JFIF_HEADER->rSOF);
	
 	strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
 	strcat(bFileName,".inf");
 	i4JdecVfyPrintf("[Jpeg][Verify] Save Color Buffer Info File %s\n",bFileName);
 	i4FileHandle = i4JdecVfyFopen(bFileName,"w");
 	i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",_u4ImageWidth); UTIL_Printf("ImageWidth=%d\n",_u4ImageWidth);
 	i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",_u4ImageHeight);UTIL_Printf("ImageHeight=%d\n",_u4ImageHeight);
 	i4JdecVfyFprintf(i4FileHandle,"BufWidth=%d\n",_u4ColorBufWidth);UTIL_Printf("BufWidth=%d\n",_u4ColorBufWidth);
 	i4JdecVfyFprintf(i4FileHandle,"HSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor);UTIL_Printf("HSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor);
 	i4JdecVfyFprintf(i4FileHandle,"VSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor);UTIL_Printf("VSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor);
 	i4JdecVfyFprintf(i4FileHandle,"HSampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor);UTIL_Printf("HSampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor);
 	i4JdecVfyFprintf(i4FileHandle,"VSampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor);UTIL_Printf("SampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor);
 	i4JdecVfyFprintf(i4FileHandle,"HSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor);UTIL_Printf("HSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor);
 	i4JdecVfyFprintf(i4FileHandle,"VSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor);UTIL_Printf("VSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor);
 	i4JdecVfyFclose(i4FileHandle);     //save hw results
    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".color.y.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Y Color Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufY,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;
    // For one component jpg ,only compare the Y buffer
    if (_prJPEG_JFIF_HEADER->rSOF.u1NumComponents == 1)
    {
     return i4Ret;
    }
	if(_prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1VSampFactor)
	{
	    u4Tempy = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor / _prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1VSampFactor;
	    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
	    strcat(bFileName,".color.cb.bin");
	    i4JdecVfyPrintf("[Jpeg][Verify] Save Cb Color Buffer File %s\n",bFileName);
		if(u4Tempy)
		{
	        if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufCb,(_u4ColorBufWidth*_u4ImageHeight/u4Tempy)))<0)
	        return i4Ret;
		}
	}
	if(_prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1VSampFactor)
    {
        u4Tempy = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor / _prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1VSampFactor;
        strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
        strcat(bFileName,".color.cr.bin");
        i4JdecVfyPrintf("[Jpeg][Verify] Save Cr Color Buffer File %s\n",bFileName);
		if(u4Tempy)
		{
           if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufCr,(_u4ColorBufWidth*_u4ImageHeight/u4Tempy)))<0)
            return i4Ret;
		}
	}
    
#if 0  //generate golden, only needed  first time
    //Save enhance HW check sum for autotest
    strcpy(GoldenFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
     {
       strcat(GoldenFileName,".enhance.color.y.bin"); 
     }
     u4CheckSum = u4Jdec32BitAccumulator(_u4ColorBufY,0,0,_u4ColorBufWidth,_u4ImageHeight);   
     i4JdecVfyPrintf("[Jpeg][Verify] Save enhance Y  Checksum File %s\n",GoldenFileName); 

    if((i4Ret = i4JdecVfyWriteFile(GoldenFileName,(UINT32)&u4CheckSum,sizeof(UINT32)))<0)
        return i4Ret;
  
    strcpy(GoldenFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
     if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
     {
       strcat(GoldenFileName,".enhance.color.cb.bin"); 
     }

     u4CheckSum = u4Jdec32BitAccumulator(_u4ColorBufCb,0,0,_u4ColorBufWidth,_u4ImageHeight);   
     i4JdecVfyPrintf("[Jpeg][Verify] Save enhance CB Checksum File %s\n",GoldenFileName); 
     if((i4Ret = i4JdecVfyWriteFile(GoldenFileName,(UINT32)&u4CheckSum,sizeof(UINT32)))<0)
        return i4Ret; 

     strcpy(GoldenFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
      if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
      {
        strcat(GoldenFileName,".enhance.color.cr.bin"); 
      }
     
      u4CheckSum = u4Jdec32BitAccumulator(_u4ColorBufCr,0,0,_u4ColorBufWidth,_u4ImageHeight);   
      i4JdecVfyPrintf("[Jpeg][Verify] Save enhance CR Checksum File %s\n",GoldenFileName); 
      if((i4Ret = i4JdecVfyWriteFile(GoldenFileName,(UINT32)&u4CheckSum,sizeof(UINT32)))<0)
         return i4Ret; 
    
   #endif
    return i4Ret;

}
INT32 i4SaveEnhanceBuf(void)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;
    JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".color.y.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Y Color Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufY,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".color.cb.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Cb Color Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufCb,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".color.cr.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Cr Color Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufCr,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;

    rPicInfo.prSofInfo = &(_prJPEG_JFIF_HEADER->rSOF);

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".inf");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Color Buffer Info File %s\n",bFileName);
    i4FileHandle = i4JdecVfyFopen(bFileName,"w");
    i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",_u4ImageWidth);
    i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",_u4ImageHeight);
    i4JdecVfyFprintf(i4FileHandle,"BufWidth=%d\n",_u4ColorBufWidth);
    i4JdecVfyFprintf(i4FileHandle,"HSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor);
    i4JdecVfyFprintf(i4FileHandle,"VSampleFactor0=%d\n",rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor);
    i4JdecVfyFprintf(i4FileHandle,"HSampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor);
    i4JdecVfyFprintf(i4FileHandle,"VSampleFactor1=%d\n",rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor);
    i4JdecVfyFprintf(i4FileHandle,"HSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor);
    i4JdecVfyFprintf(i4FileHandle,"VSampleFactor2=%d\n",rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor);
    i4JdecVfyFclose(i4FileHandle);

    return i4Ret;

}

INT32 i4SaveTestFrameBuf(UINT32 u4TestNum)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;

    //Save HW results
    sprintf(bFileName,"%s.%04d.frame.y.bin",(char *)_abJpegFileList[_u4CurJpegFileListNum],(int)u4TestNum);
    i4JdecVfyPrintf("[Jpeg][Verify] Save Y Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4FrameBufY,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT)))<0)
        return i4Ret;

    sprintf(bFileName,"%s.%04d.frame.c.bin",(char *)_abJpegFileList[_u4CurJpegFileListNum],(int)u4TestNum);
    i4JdecVfyPrintf("[Jpeg][Verify] Save C Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4FrameBufC,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT)))<0)
        return i4Ret;


    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.inf");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Frame Buffer Info File %s\n",bFileName);
    i4FileHandle = i4JdecVfyFopen(bFileName,"w");
    i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",JDEC_FRAME_BUF_WIDTH);
    i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",JDEC_FRAME_BUF_HEIGHT);
    i4JdecVfyFprintf(i4FileHandle,"BufferMainFormat=Y_C_BUFFER\n");
    i4JdecVfyFprintf(i4FileHandle,"YUVFormat=420\n");
    i4JdecVfyFclose(i4FileHandle);
    return i4Ret;
}

INT32 i4SaveFrameBuf(void)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    char GoldenFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;
    UINT32 u4CheckSum = 0;
    //JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };
    //Save HW results
    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.frame.y.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Y Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4FrameBufY,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT)))<0)
        return i4Ret;

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.frame.c.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save C Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4FrameBufC,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT)))<0)
        return i4Ret;

    //rPicInfo.prSofInfo = &(_prJPEG_JFIF_HEADER->rSOF);

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.inf");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Frame Buffer Info File %s\n",bFileName);
    i4FileHandle = i4JdecVfyFopen(bFileName,"w");
    i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",JDEC_FRAME_BUF_WIDTH);
    i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",JDEC_FRAME_BUF_HEIGHT);
    i4JdecVfyFprintf(i4FileHandle,"BufferMainFormat=Y_C_BUFFER\n");
    i4JdecVfyFprintf(i4FileHandle,"YUVFormat=420\n");
    i4JdecVfyFclose(i4FileHandle);

    //Save HW check sum for autotest
    strcpy(GoldenFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    if(_fgJdecVfyTestPicMode)
     {
	       strcat(GoldenFileName,".baseline.frame.y.bin");
      }
    else if (_fgJdecVfyTestProgressiveScanMultiColltet)
     {
	   strcat(GoldenFileName,".multicollect.frame.y.bin");
     }else
     {
       strcat(GoldenFileName,".enhance.frame.y.bin");
     }
     u4CheckSum = u4Jdec32BitAccumulator(_u4FrameBufY,0,0,JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
     i4JdecVfyPrintf("[Jpeg][Verify] Save Y Frame Checksum File %s\n",GoldenFileName);
     if((i4Ret = i4JdecVfyWriteFile(GoldenFileName,(UINT32)&u4CheckSum,sizeof(UINT32)))<0)
    return i4Ret;

    strcpy(GoldenFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    if(_fgJdecVfyTestPicMode)
     {
	    strcat(GoldenFileName,".baseline.frame.c.bin");
      }
    else if (_fgJdecVfyTestProgressiveScanMultiColltet)
     {
	    strcat(GoldenFileName,".multicollect.frame.c.bin");
     }else
     {
       strcat(GoldenFileName,".enhance.frame.c.bin");
}
     u4CheckSum = u4Jdec32BitAccumulator(_u4FrameBufC,0,0,JDEC_FRAME_BUF_WIDTH,JDEC_FRAME_BUF_HEIGHT);
     i4JdecVfyPrintf("[Jpeg][Verify] Save C Frame Checksum File %s\n",GoldenFileName);
     if((i4Ret = i4JdecVfyWriteFile(GoldenFileName,(UINT32)&u4CheckSum,sizeof(UINT32)))<0)
        return i4Ret;

    return i4Ret;

}

INT32 i4SaveAYUVBuf(void)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;
    //JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.ayuv.bin");
    i4JdecVfyPrintf("[Jpeg][Verify] Save AYUV Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4FrameBufY,(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT)))<0)
        return i4Ret;

    //rPicInfo.prSofInfo = &(_prJPEG_JFIF_HEADER->rSOF);

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.inf");
    i4JdecVfyPrintf("[Jpeg][Verify] Save AYUV Buffer Info File %s\n",bFileName);
    i4FileHandle = i4JdecVfyFopen(bFileName,"w");
    i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",JDEC_FRAME_BUF_WIDTH/4);
    i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",JDEC_FRAME_BUF_HEIGHT);
    i4JdecVfyFprintf(i4FileHandle,"BufferMainFormat=AYUV_BUFFER\n");
    i4JdecVfyFclose(i4FileHandle);

    return i4Ret;

}

INT32 i4SaveVideoOutBuf(UINT32 u4TestNum)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    INT32 i4FileHandle;

    //Save HW results
    sprintf(bFileName,"%s.%04d.frame.y.bin",(char *)_abJpegFileList[_u4CurJpegFileListNum],(int)u4TestNum);
    i4JdecVfyPrintf("[Jpeg][Verify] Save Y Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufY,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;

    sprintf(bFileName,"%s.%04d.frame.c.bin",(char *)_abJpegFileList[_u4CurJpegFileListNum],(int)u4TestNum);
    i4JdecVfyPrintf("[Jpeg][Verify] Save C Frame Buffer File %s\n",bFileName);
    if((i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_u4ColorBufCb,(_u4ColorBufWidth*_u4ImageHeight)))<0)
        return i4Ret;


    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
    strcat(bFileName,".0000.inf");
    i4JdecVfyPrintf("[Jpeg][Verify] Save Frame Buffer Info File %s\n",bFileName);
    i4FileHandle = i4JdecVfyFopen(bFileName,"w");
    i4JdecVfyFprintf(i4FileHandle,"ImageWidth=%d\n",(((_u4ImageWidth+15)/16)*16));
    i4JdecVfyFprintf(i4FileHandle,"ImageHeight=%d\n",_u4ImageHeight);
    i4JdecVfyFprintf(i4FileHandle,"BufferMainFormat=Y_C_BUFFER\n");
    i4JdecVfyFprintf(i4FileHandle,"YUVFormat=420\n");
    i4JdecVfyFclose(i4FileHandle);
    return i4Ret;
}

extern void  jpg_free_align_big_mem(void* ptr);
extern void*  jpg_alloc_align_big_mem(UINT32 u4Size, UINT32 u4Aligment);
extern void  jpg_init_align_big_mem(void);
extern void vHwImgReszResetDramSetting(UINT32 u4HwId);
void vJpegVerify(void)
{
    INT32 i4Ret;
#if JDEC_COMPARE_WITH_GOLDEN
    UINT32 u4Temp = 0;
#endif
#if JDEC_DRAMBUSY_TEST
    UINT32 u4TestAgentSA = 0;
#endif
    UINT32 u4ClockLoop =0;
         UTIL_Printf(" RESET RESZIE dram table==\n");
   // vHwImgReszResetDramSetting(0);
   
    vHwImgReszResetDramSetting(1);//imgrz dddd

    jpg_init_align_big_mem();
    if(_fgForceDisableInterrupt)
    	{
    	  _fgJdecVfyTestInterrupt = FALSE;
		  UTIL_Printf("JDEC_VER disable interrupt==\n");
    	}
    // Allocate Memory

    i4Ret = x_ev_group_create(&_hJdecEventHandle, "JpegEvent", 0);
    VERIFY(i4Ret == OSR_OK);

    if(_fgJdecVfyHighMemTest)
    {
        _EmuJpgFifo = (BYTE *)(_u4JdecVfyHighMemBase - 0x400000);
        x_memset(_EmuJpgFifo,0,JDEC_FIFO_SIZE);

        _u4ColorBufY = _u4JdecVfyHighMemBase;
        _u4ColorBufCb = _u4JdecVfyHighMemBase + 0x200000;
        _u4ColorBufCr = _u4JdecVfyHighMemBase + 0x400000;

        _u4ColorBufY2 = _u4JdecVfyHighMemBase + 0x600000;
        _u4ColorBufCb2 = _u4JdecVfyHighMemBase + 0x800000;
        _u4ColorBufCr2 = _u4JdecVfyHighMemBase + 0xA00000;

        _u4FrameBufY = _u4JdecVfyHighMemBase + 0xC00000;
        _u4FrameBufC = _u4JdecVfyHighMemBase + 0xE00000;

        _u4EnhanceBufY = _u4JdecVfyHighMemBase + 0x1000000;
        _u4EnhanceBufCb = _u4JdecVfyHighMemBase + 0x1200000;
        _u4EnhanceBufCr = _u4JdecVfyHighMemBase + 0x1400000;
	_pbJpegFileList = (BYTE *)_u4JdecVfyHighMemBase + 0x1600000;
    }
    else
    {
        _u4ColorBufY  = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCb = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCr = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufY2 = (UINT32)x_alloc_aligned_dma_mem(JDEC_GOLDEN_FIFO_SIZE,2048);
        _u4ColorBufCb2 = (UINT32)x_alloc_aligned_dma_mem(JDEC_GOLDEN_FIFO_SIZE,2048);
        _u4ColorBufCr2 = (UINT32)x_alloc_aligned_dma_mem(JDEC_GOLDEN_FIFO_SIZE,2048);
      
        if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
       {
           _u4EnhanceBufY = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
           _u4EnhanceBufCb = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
           _u4EnhanceBufCr = (UINT32)jpg_alloc_align_big_mem(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
           i4JdecVfyPrintf("[Jpeg][Verify]_u4EnhanceBufY sa = %x\n",_u4EnhanceBufY);
           i4JdecVfyPrintf("[Jpeg][Verify]_u4EnhanceBufCb = %x\n",_u4EnhanceBufCb);
           i4JdecVfyPrintf("[Jpeg][Verify]_u4EnhanceBufCr = %x\n",_u4EnhanceBufCr);
       }
       #if JDEC_IO_MMU_TEST
	    i4JdecVfyPrintf("[Jpeg]MMU enable \n"); 
      // _EmuJpgFifo = (BYTE *)x_jpg_vmem(JDEC_FIFO_SIZE);
       _EmuJpgFifoApp =  (BYTE *)x_jpg_vmem(JDEC_FIFO_SIZE + 2048);
       _EmuJpgFifo = (BYTE *)(((UINT32)_EmuJpgFifoApp + 2047) & (~0x7ff));
       _EmuJpgFifoback=(BYTE *)jpg_alloc_align_big_mem(JDEC_FIFO_SIZE,2048);
       //_EmuRingFifo = (BYTE *)x_jpg_vmem(JDEC_RING_FIFO_SIZE+128);	       
        _EmuRingFifoApp = (BYTE *)x_jpg_vmem(JDEC_RING_FIFO_SIZE+128 + 2048);	  
        _EmuRingFifo = (BYTE *)(((UINT32)_EmuRingFifoApp + 2047) & (~0x7ff));
        i4JdecVfyPrintf("jpeg iommu buffer:\nApply buffer _EmuJpgFifoApp:0x%x, _EmuRingFifoApp:0x%x, align _EmuJpgFifo:0x%x, _EmuRingFifo:0x%x\n", _EmuJpgFifoApp, _EmuRingFifoApp,  _EmuJpgFifo, _EmuRingFifo);

       {
            #if 0
            UTIL_Printf("64 byte align for ring\n");
            _EmuRingFifo =(BYTE *)( ((UINT32)_EmuRingFifo / 64) * 64);//for iommu, ringfifo
            #endif
        }
        if(!_fgJdecVfyTestPicModeAYUVOutput)
        {
            _u4FrameBufYApp = (UINT32)x_jpg_vmem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT + 2048);
            _u4FrameBufCApp = (UINT32)x_jpg_vmem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT+ 2048);
            _u4FrameBufY = (_u4FrameBufYApp + 2047) & (~0x7ff);
            _u4FrameBufC = (_u4FrameBufCApp + 2047) & (~0x7ff);
            i4JdecVfyPrintf("jpeg iommu buffer2:\nApply buffer _u4FrameBufYApp(0x%x), _u4FrameBufCApp(0x%x), align _u4FrameBufY:0x%x, _u4FrameBufC:0x%x", _u4FrameBufYApp, _u4FrameBufCApp,_u4FrameBufY, _u4FrameBufC);
            //_u4FrameBufY = (UINT32)x_jpg_vmem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT);
            //_u4FrameBufC = (UINT32)x_jpg_vmem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT);
        }
        else
        {
            _u4FrameBufY = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT,2048);
            _u4FrameBufC = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT,2048);
        }
        #else	  
            i4JdecVfyPrintf("[Jpeg]MMU disable \n"); 
            _EmuJpgFifo = (BYTE *)jpg_alloc_align_big_mem(JDEC_FIFO_SIZE,2048);
            _EmuRingFifo = (BYTE *)x_alloc_aligned_dma_mem(JDEC_RING_FIFO_SIZE, 2048);
            _u4FrameBufY = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT,2048);
            _u4FrameBufC = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*JDEC_FRAME_BUF_HEIGHT,2048);  
        #endif
        _pbJpegFileList = (BYTE *)jpg_alloc_align_big_mem(JpegFileListMax*JDEC_FILE_NAME_LEN, 16);
    }
    // For emulation ,should comment it ,if for verification ,should open it
    //CKGEN_WRITE32(0x6C, 0<<20);
    _u4ImgReszVfyJpegPicModeTempLineBuf[0] = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*3*4 *2,2048);
    _u4ImgReszVfyJpegPicModeTempLineBuf[1] = (UINT32)x_alloc_aligned_dma_mem(JDEC_FRAME_BUF_WIDTH*3*4 *2,2048);

     i4JdecVfyPrintf("[Jpeg][Verify]Vfifo sa = 0x%x, length = 0x%x \n",_EmuJpgFifo,JDEC_FIFO_SIZE);
     i4JdecVfyPrintf("[Jpeg][Verify]ringfifo sa = 0x%x, length = 0x%x \n",_EmuRingFifo,JDEC_RING_FIFO_SIZE);
     i4JdecVfyPrintf("[Jpeg][Verify]colorY sa = 0x%x\n",_u4ColorBufY);
     i4JdecVfyPrintf("[Jpeg][Verify]colorCB sa = 0x%x\n",_u4ColorBufCb);
     i4JdecVfyPrintf("[Jpeg][Verify]colorCR sa = 0x%x\n",_u4ColorBufCr);
     i4JdecVfyPrintf("[Jpeg][Verify]frameY sa = 0x%x\n",_u4FrameBufY);
     i4JdecVfyPrintf("[Jpeg][Verify]frameC sa = 0x%x\n",_u4FrameBufC);
     i4JdecVfyPrintf("[Jpeg][Verify]goldenY sa = 0x%x\n",_u4ColorBufY2);
     i4JdecVfyPrintf("[Jpeg][Verify]goldenCB sa = 0x%x\n",_u4ColorBufCb2);
     i4JdecVfyPrintf("[Jpeg][Verify]goldenCR sa = 0x%x\n",_u4ColorBufCr2);
     i4JdecVfyPrintf("[Jpeg][Verify]tempbuffer0 sa = 0x%x\n",_u4ImgReszVfyJpegPicModeTempLineBuf[0]);
     i4JdecVfyPrintf("[Jpeg][Verify]tempbuffer1 sa = 0x%x\n",_u4ImgReszVfyJpegPicModeTempLineBuf[1]);
     i4JdecVfyPrintf("[Jpeg][Verify]_pbJpegFileList sa = 0x%x\n",_pbJpegFileList);
       if ((_EmuJpgFifo == NULL) ||
        (_EmuRingFifo == NULL) ||
        (_u4ColorBufY == 0) ||
        (_u4ColorBufCb == 0) ||
        (_u4ColorBufCr == 0) ||
        (_u4FrameBufY == 0) ||
        (_u4FrameBufC == 0) ||
//        (_u4EnhanceBufY == 0)||
//        (_u4EnhanceBufCb== 0)||
//        (_u4EnhanceBufCr== 0)||
        (_pbJpegFileList== NULL)||
        (_u4ImgReszVfyJpegPicModeTempLineBuf[0]== 0)||
        (_u4ImgReszVfyJpegPicModeTempLineBuf[1]== 0)||
        (_u4ColorBufY2 == 0)||
        (_u4ColorBufCb2 == 0)||
        (_u4ColorBufCr2 == 0))

    {
        i4JdecVfyPrintf("[Jpeg][Verify] Malloc memory failed ***Attention*** \n");
        ASSERT(0);
    }

/*Set Test agent*/
#if JDEC_DRAMBUSY_TEST
     u4TestAgentSA = (UINT32)x_alloc_aligned_dma_mem(0x100000,16);
     *(volatile UINT32 *)(IO_BASE + 0x007210) = u4TestAgentSA;
     *(volatile UINT32 *)(IO_BASE + 0x007214) = 0x1000;
     *(volatile UINT32 *)(IO_BASE + 0x007104) = 0x00000000;
     *(volatile UINT32 *)(IO_BASE + 0x007218) = 0x8e0f110d;
#endif
      
    {
        UINT32 u4I;

        for(u4I=0; u4I<JpegFileListMax*JDEC_FILE_NAME_LEN; u4I++)
            _pbJpegFileList[u4I] = 0;

        for(u4I=0; u4I<JpegFileListMax; u4I++)
            _abJpegFileList[u4I] = 0;
    }

    {
        INT32 i4FileSize;
        i4FileSize = i4LoadJpegFileList();
        vParseJpegFileList(i4FileSize);
    }
#if PERFORMANCE_TEST
    i4FileHandleSavePerformanceTime= i4JdecVfyFopen("D:\\Projects\\MT8580_vfy\\JDEC\\performance\\performancetime.txt","w");
#endif
  for(u4ClockLoop =_ui4StartClock; u4ClockLoop<=_ui4EndClock; u4ClockLoop++)
  {
  	extern UINT32 _JDEC_CLOCK_SETING;
  	_JDEC_CLOCK_SETING = u4ClockLoop;
    for(_u4CurJpegFileListNum=0; _u4CurJpegFileListNum<_u4JpegFileListNum; _u4CurJpegFileListNum++)
    {
        i4JdecVfyPrintf("[Jpeg][Verify] Load source File %s\n",_abJpegFileList[_u4CurJpegFileListNum]);
	//	x_thread_delay(500);
        _i4JpegFileSize = i4LoadJpegFile();
        //x_thread_delay(1500);
        if(_i4JpegFileSize < 0)
        {
            i4JdecVfyPrintf("[Jpeg][Verify] Load File fail\n");
            continue;
        }

        i4JdecVfyPrintf("[Jpeg][Verify] Decode Jpeg by HW\n");
        _fgJdecSetIRQ = FALSE;

#if JDEC_COMPARE_WITH_GOLDEN
        {

            BYTE *pbTemp;
            BYTE bCount,bi;


            bCount = 0;
            pbTemp = _abJpegFileList[_u4CurJpegFileListNum];
            while (*pbTemp != '\0')
            {
                pbTemp ++;
                bCount ++;
            }
            for (bi = 0; bi < 3; bi ++)
            {
                memset(_pbJpegGolden[bi], 0, JDEC_FILE_NAME_LEN);
                memcpy(_pbJpegGolden[bi],_abJpegFileList[_u4CurJpegFileListNum], bCount);
            }
            if(_fgJdecVfyTestPicMode)
            {
               memcpy(&_pbJpegGolden[0][bCount],".baseline.frame.y.bin",x_strlen(".baseline.frame.y.bin"));
               memcpy(&_pbJpegGolden[1][bCount],".baseline.frame.c.bin",x_strlen(".baseline.frame.c.bin"));
             }
	     else if(_fgJdecVfyTestProgressiveScanMultiColltet)
	     {
               memcpy(&_pbJpegGolden[0][bCount],".multicollect.frame.y.bin",x_strlen(".multicollect.frame.y.bin"));
               memcpy(&_pbJpegGolden[1][bCount],".multicollect.frame.c.bin",x_strlen(".multicollect.frame.c.bin"));
	     }
	     else if(_fgJdecVfyTestProgressiveScanEnhance)
	     {
               memcpy(&_pbJpegGolden[0][bCount],".enhance.frame.y.bin",x_strlen(".enhance.frame.y.bin"));
               memcpy(&_pbJpegGolden[1][bCount],".enhance.frame.c.bin",x_strlen(".enhance.frame.c.bin"));
	     }
             else if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
             {
                  memcpy(&_pbJpegGolden[0][bCount],".enhance.color.y.bin",x_strlen(".enhance.color.y.bin"));
                  memcpy(&_pbJpegGolden[1][bCount],".enhance.color.cb.bin",x_strlen(".enhance.color.cb.bin"));
                  memcpy(&_pbJpegGolden[2][bCount],".enhance.color.cr.bin",x_strlen(".enhance.color.cr.bin"));  
              }            
             else if(_fgJdecVfyTestVideoOutMode)
             {
                 memcpy(&_pbJpegGolden[0][bCount],"_y_plane.bin",x_strlen("_y_plane.bin"));
                 memcpy(&_pbJpegGolden[1][bCount],"_c_plane.bin",x_strlen("_c_plane.bin"));        
             }                
	     else
	     {
            memcpy(&_pbJpegGolden[0][bCount],"_y.bin",x_strlen("_y.bin"));
            memcpy(&_pbJpegGolden[1][bCount],"_cb.bin",x_strlen("_cb.bin"));
            memcpy(&_pbJpegGolden[2][bCount],"_cr.bin",x_strlen("_cr.bin"));
        }
        }
#endif

#if PERFORMANCE_TEST   
    i4JdecVfyFprintf(i4FileHandleSavePerformanceTime,"filename=%s\n",_abJpegFileList[_u4CurJpegFileListNum]);
#endif

#if JDEC_COMPARE_WITH_GOLDEN
    for(u4Temp = 0; u4Temp < BURN_IN_TEST_COUNT; u4Temp ++)
    {
#endif
        BOOL fgSaveFailed =FALSE;
		UINT32 i4Ret =0;
  #if  DEBUG_MMMMM_ON 
   	{
		UINT32 iloop=0;
		for(iloop=0; iloop<14; iloop++)
	   {
	     if(_pi4JpegWindow[iloop] !=NULL)//_ui4ScanNumber
	     {
	       x_free_vmem(_pi4JpegWindow[iloop]);
		   _pi4JpegWindow[iloop] = NULL;
		   _u4WindowNum[iloop]=0;
	     }
		}
   	}
 #endif 		
        UTIL_Printf("[JDEC_VFY] Loop count ***[%d]********\n",u4Temp);
		HAL_GetTime(&_rDecStartTime);
        i4Ret =i4JdecVfyDecode();
		HAL_GetTime(&_rDecEndTime);
		HAL_GetDeltaTime(&_rDecTotalTime, &_rDecStartTime, &_rDecEndTime);	
		_ui8JdecDecTotalTime+=(_rDecTotalTime.u4Seconds * 1000000 + _rDecTotalTime.u4Micros);
	    if(i4Ret>= 0)
        {

          if (_fgCompareGold)
      	   {
              x_memset((void *)_u4ColorBufY2,0,2048);
              x_memset((void *)_u4ColorBufCb2,0,2048);
              x_memset((void *)_u4ColorBufCr2,0,2048);
      	       //_fgSaveRes = FALSE;
              //HalFlushInvalidateDCache();
              i4JdecVfyPrintf("[Jpeg][Verify] read %s!\n",_pbJpegGolden[0]);
              if((i4JdecVfyReadFile((char*)_pbJpegGolden[0], (UINT32)_u4ColorBufY2, 2048))>=0)
              {
                   //i4JdecAsiToInt(_u4ColorBufY2,8);
                 // i4JdecVfyPrintf("[Jpeg][Verify] Load Y golden OK!\n");
                  //i4JdecVfyFprintf2("JpgVfyLog.txt", "[Jpeg][Verify] Load Y golden OK!\n");
              }
              else
              {
  
                  i4JdecVfyPrintf("[Jpeg][Verify] Load Y golden fail!\n");
                  //i4JdecVfyFprintf2("JpgVfyLog.txt", "[Jpeg][Verify] Load Y golden fail!\n");
              }  
			  i4JdecVfyPrintf("[Jpeg][Verify] read %s!\n",_pbJpegGolden[1]);
              if((i4JdecVfyReadFile((char*)_pbJpegGolden[1], (UINT32)_u4ColorBufCb2,2048))>=0)
              {
                  //i4JdecAsiToInt(_u4ColorBufCb2,8);
                 // i4JdecVfyPrintf("[Jpeg][Verify] Load Cb golden OK!\n");
              }
              else
              {
                  i4JdecVfyPrintf("[Jpeg][Verify] Load Cb golden fail!\n");
              }  
			   if(!(_fgJdecVfyTestPicMode || _fgJdecVfyTestProgressiveScanMultiColltet ||_fgJdecVfyTestProgressiveScanEnhance ||_fgJdecVfyTestVideoOutMode))
              {
              // compare cr component
                   i4JdecVfyPrintf("[Jpeg][Verify] read %s!\n",_pbJpegGolden[2]);
                if((i4JdecVfyReadFile((char*)_pbJpegGolden[2], (UINT32)_u4ColorBufCr2, 2048))>=0)
              {
                  //i4JdecAsiToInt(_u4ColorBufCr2,8);
                  //i4JdecVfyPrintf("[Jpeg][Verify] Load Cr golden OK!\n");
  
              }
              else
              {
                  i4JdecVfyPrintf("[Jpeg][Verify] Load Cr golden fail!\n");
  
              }
              }
  
  	     //HalFlushInvalidateDCache();
              if(i4JpegCompareBuf()<0)
              {
                  fgSaveFailed = TRUE;  
                  i4JdecVfyPrintf("[Jpeg][Verify-Result]  %s Compare with golden fail\n",_abJpegFileList[_u4CurJpegFileListNum]);
                  #if CONFIG_JPG_FATS_SUPPORT
                  FATS_IF_SaveLog(1,"[Jpeg][Verify-Result]  %s Compare with golden fail",_abJpegFileList[_u4CurJpegFileListNum]);
                  #endif
              }
              else
              {
                  i4JdecVfyPrintf("[Jpeg][Verify-Result] %s Compare with golden OK\n",_abJpegFileList[_u4CurJpegFileListNum]);
                 //fgSaveFailed =TRUE;
              }
      	}

        if (_fgSaveRes ||fgSaveFailed)
      	{      	
            if(_fgJdecVfyTestPicMode ||
               _fgJdecVfyTestProgressiveEnhancePreload || 
               _fgJdecVfyTestProgressiveScanMultiColltet ||
               _fgJdecVfyTestProgressiveScanEnhance
			)
            {            
                if(_fgJdecVfyTestPicModeAYUVOutput)
                    i4SaveAYUVBuf();
                else
                    //i4SaveFrameBuf();
                    i4SaveTestFrameBuf(u4Temp);
             } 
            else if(_fgJdecVfyTestVideoOutMode)
             {
                   i4SaveVideoOutBuf(u4Temp); 
             }
            else
            {
                i4SaveColorBuf();
		    // ASSERT(0);
            }
            _fgSaveRes = FALSE;
      	}
     }
     else
     {
         i4JdecVfyPrintf("[Jpeg][Verify] Decode %s fail\n",_abJpegFileList[_u4CurJpegFileListNum]);
     }
#if DEBUG_MMMMM_ON	 
{
     UINT32 i=0, i1=0;
	 for(i=0; i<14; i++)
	 {
	   if((_pi4JpegWindow[i] == NULL) && _u4WindowNum[i])
	   {
	    
	    UTIL_Printf("----frame[%d]-----\n",i);
	    for(i1=0; i1<_u4WindowNum[i]; i1++)
	    {
	        UTIL_Printf("[0x%x:]",  _pi4JpegWindow[i][i1]);
	    }
		UTIL_Printf("\n");
	  }
	}
}
#endif	  

#if JDEC_COMPARE_WITH_GOLDEN
    }
#endif

    }
  }
    i4JdecVfyPrintf("[Jpeg][Verify] Verify complete.\n");
    
#if PERFORMANCE_TEST
     i4JdecVfyFclose(i4FileHandleSavePerformanceTime);
#endif

#if CONFIG_JPG_FATS_SUPPORT
     UDVT_IF_SendResult(UDVT_TEST_PASS);
#endif

    if(!_fgJdecVfyHighMemTest)
    {
 #if JDEC_IO_MMU_TEST
        i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuJpgFifoApp 0x%x\n",_EmuJpgFifoApp);
        x_mem_free((void*)_EmuJpgFifoApp);       
        _EmuJpgFifoApp=0;       
       //i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuJpgFifo 0x%x\n",_EmuJpgFifo);
        //x_mem_free((void*)_EmuJpgFifo); 
       //_EmuJpgFifo =0;       
       //x_free_vmem((void*)_EmuJpgFifo);

        i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuJpgFifoback 0x%x\n",_EmuJpgFifoback);
        jpg_free_align_big_mem((void*)_EmuJpgFifoback);_EmuJpgFifoback =0;
        // i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuRingFifo 0x%x\n",_EmuRingFifo);
        //x_free_vmem((void*)_EmuRingFifo);
        //x_mem_free((void*)_EmuRingFifo); 
        //_EmuRingFifo =0;       
        i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuRingFifoApp 0x%x\n",_EmuRingFifoApp);
        x_mem_free((void*)_EmuRingFifoApp); 
        _EmuRingFifoApp =0;
        if(!_fgJdecVfyTestPicModeAYUVOutput)
        {
#if 0
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufY 0x%x\n",_u4FrameBufY);
            x_mem_free((void*)_u4FrameBufY); 
            // x_free_vmem((void*)_u4FrameBufY);
            _u4FrameBufY =0;
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufC 0x%x\n",_u4FrameBufC);
            x_mem_free((void*)_u4FrameBufC); 
            //x_free_vmem((void*)_u4FrameBufC);
            _u4FrameBufC =0;
#endif
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufYApp 0x%x\n",_u4FrameBufYApp);
            x_mem_free((void*)_u4FrameBufYApp);             
            _u4FrameBufYApp =0;
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufCApp 0x%x\n",_u4FrameBufCApp);
            x_mem_free((void*)_u4FrameBufCApp);            
            _u4FrameBufCApp =0;
        }
        else
        {
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufY 0x%x\n",_u4FrameBufY);
            x_free_aligned_dma_mem((void*)_u4FrameBufY);_u4FrameBufY =0;
            i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufC 0x%x\n",_u4FrameBufC);
            x_free_aligned_dma_mem((void*)_u4FrameBufC);_u4FrameBufC =0;
        }	   
#else
       i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuJpgFifo 0x%x\n",_EmuJpgFifo);
       jpg_free_align_big_mem((void*)_EmuJpgFifo);_EmuJpgFifo =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _EmuRingFifo 0x%x\n",_EmuRingFifo);
       x_free_aligned_dma_mem((void*)_EmuRingFifo);_EmuRingFifo=0;
	    i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufY 0x%x\n",_u4FrameBufY);
        x_free_aligned_dma_mem((void*)_u4FrameBufY);_u4FrameBufY =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4FrameBufC 0x%x\n",_u4FrameBufC);
       x_free_aligned_dma_mem((void*)_u4FrameBufC);_u4FrameBufC =0;
#endif
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufY 0x%x\n",_u4ColorBufY);
       jpg_free_align_big_mem((void*)_u4ColorBufY);_u4ColorBufY =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufCb 0x%x\n",_u4ColorBufCb);
       jpg_free_align_big_mem((void*)_u4ColorBufCb);_u4ColorBufCb =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufCr 0x%x\n",_u4ColorBufCr);
       jpg_free_align_big_mem((void*)_u4ColorBufCr);_u4ColorBufCr =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufY2 0x%x\n",_u4ColorBufY2);
       x_free_aligned_dma_mem((void*)_u4ColorBufY2);_u4ColorBufY2 =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufCb2 0x%x\n",_u4ColorBufCb2);
       x_free_aligned_dma_mem((void*)_u4ColorBufCb2);_u4ColorBufCb2 =0;
       i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ColorBufCr2 0x%x\n",_u4ColorBufCr2);
       x_free_aligned_dma_mem((void*)_u4ColorBufCr2);_u4ColorBufCr2=0;  
       i4JdecVfyPrintf("[Jpeg][Verify] Free _pbJpegFileList 0x%x\n",_pbJpegFileList);
       jpg_free_align_big_mem((void*)_pbJpegFileList);_pbJpegFileList =0;
       if(_fgJdecVfyTestProgressiveMCUEnhance ||_fgJdecVfyTestProgressiveEnhance)
       {
         i4JdecVfyPrintf("[Jpeg][Verify] Free _u4EnhanceBufY 0x%x\n",_u4EnhanceBufY);
         x_free_aligned_dma_mem((void*)_u4EnhanceBufY);_u4EnhanceBufY =0;
		 i4JdecVfyPrintf("[Jpeg][Verify] Free _u4EnhanceBufCb 0x%x\n",_u4EnhanceBufCb);
         x_free_aligned_dma_mem((void*)_u4EnhanceBufCb);_u4EnhanceBufCb =0;
		 i4JdecVfyPrintf("[Jpeg][Verify] Free _u4EnhanceBufCr 0x%x\n",_u4EnhanceBufCr);
         x_free_aligned_dma_mem((void*)_u4EnhanceBufCr);_u4EnhanceBufCr =0;
      	}
  #if JDEC_DRAMBUSY_TEST
          if(u4TestAgentSA)  x_free_aligned_dma_mem((void *)u4TestAgentSA);u4TestAgentSA=0;
  #endif
    }
	i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ImgReszVfyJpegPicModeTempLineBuf[0]\n");
	 x_free_aligned_dma_mem((void*)_u4ImgReszVfyJpegPicModeTempLineBuf[0]);_u4ImgReszVfyJpegPicModeTempLineBuf[0] =0;
	i4JdecVfyPrintf("[Jpeg][Verify] Free _u4ImgReszVfyJpegPicModeTempLineBuf[1]\n");
	x_free_aligned_dma_mem((void*)_u4ImgReszVfyJpegPicModeTempLineBuf[1]);_u4ImgReszVfyJpegPicModeTempLineBuf[1] =0;
	i4JdecVfyPrintf("[Jpeg][Verify] delete _hJdecEventHandle\n");
    i4Ret = x_ev_group_delete(_hJdecEventHandle); _hJdecEventHandle = 0;
    VERIFY(i4Ret == OSR_OK);

}

void vTxDataForRingTest(BYTE *pDest, BYTE *pSrc, UINT32 u4Size)
{
    UINT32 u4Tmp;
    BYTE *pstart = NULL;
    pstart  = pDest;
    ////_u4PreviousAdd = (UINT32)_prJPEG_JFIF_HEADER->pu1CurrRead;
   // UTIL_Printf("Copy pSrc:0x%x, size[0x%x]\n",(UINT32)pSrc,u4Size);
    if(0 == u4Size || NULL == pDest || NULL == pSrc)
        return;

    for(u4Tmp = 0; u4Tmp < u4Size; u4Tmp++)
    {
        *pDest = *pSrc;
        pDest ++;
        pSrc++;
    }
	vJdecVfyMapBuf((UINT32)pstart, u4Size,TO_DEVICE);
        UTIL_Printf("ring fifo map  u4Size:0x%x\n", u4Size);
	//x_thread_delay(3000);
	_prJPEG_JFIF_HEADER->pu1CurrRead += u4Size;
	return;
}



void vJpegVerifyCLI(UINT32 u4TestCase, BOOL fgBasliePic, UINT32 ui4Delay)
{

  _fgTestBasliePic =fgBasliePic;
  if(ui4Delay!=0)
  {
    _ui4McuRowNumberPerTime = ui4Delay;
  }

  i4JdecVfyPrintf("[JDEC][Verify] _fgTestBasliePic[%d] _ui4McuRowNumberPerTime [%d]\n",_fgTestBasliePic,_ui4McuRowNumberPerTime);
  x_memset(&_rDecTotalTime,0,sizeof(HAL_TIME_T));
  x_memset(&_rDecStartTime,0,sizeof(HAL_TIME_T));
  x_memset(&_rDecEndTime,0,sizeof(HAL_TIME_T));  
  _ui8JdecDecTotalTime =0;
  //fgDisableErrConceal = TRUE; //only fro debug
  switch(u4TestCase)
    {
        case 1:
            // baseline + multicollect mcu row mode with golden compare
            {
              	fgDisableErrConceal = TRUE; //only fro debug
	            _fgSaveRes = FALSE;
	            _fgCompareGold = TRUE;
	            _fgJdecVfyTestInterrupt = TRUE;
	            i4JdecVfyPrintf("[JDEC][Verify] baseline + multcollect mcu row mode with golden compare test start.\n");
	            vJpegVerify();
	            i4JdecVfyPrintf("[JDEC][Verify] baseline + multcollect mcu row mode with golden compare test complete.\n");
	            _fgJdecVfyTestInterrupt = FALSE;
	            _fgCompareGold =  FALSE;
	            _fgSaveRes = FALSE;
				fgDisableErrConceal = FALSE; //only fro debug
            }
            break;
        case 2:
            // enhance mcu row test
            {
              //_fgSaveRes = TRUE;
                 	fgDisableErrConceal = TRUE; //only fro debug
              _fgCompareGold = TRUE;
              _fgJdecVfyTestInterrupt = TRUE;
              _fgJdecVfyTestProgressiveEnhance =TRUE;
              i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row test start.\n");
              vJpegVerify();
              i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row test complete.\n");
              _fgJdecVfyTestProgressiveEnhance = FALSE;
              _fgJdecVfyTestInterrupt = FALSE;
              _fgCompareGold = FALSE;
              _fgSaveRes = FALSE;
			   fgDisableErrConceal = FALSE; //only fro debug
            }
            break;
        case 3:
            // baseline picture mode test
            {
             	//_fgSaveRes = TRUE;
             	fgDisableErrConceal = TRUE; //only fro debug
             	_fgCompareGold = TRUE;
             	_fgJdecVfyTestPicMode = TRUE;
             	_fgJdecVfyTestWritePointer = TRUE;
             	_fgJdecVfyTestWritePointerUpdate = TRUE;
             	_fgJdecVfyTestInterrupt = TRUE;
             	_fgJdecVfyTestPicModeAYUVOutput =FALSE;
             	i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode test start.\n");
             	vJpegVerify();
             	i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode test complete.\n");
             	_fgSaveRes = FALSE;
				fgDisableErrConceal = FALSE; //only fro debug
             	_fgCompareGold = FALSE;
             	_fgJdecVfyTestPicModeAYUVOutput =FALSE;
             	_fgJdecVfyTestPicMode = FALSE;
             	_fgJdecVfyTestWritePointer = FALSE;
             	_fgJdecVfyTestWritePointerUpdate = FALSE;
             	_fgJdecVfyTestInterrupt = FALSE;
            }
            break;
        case 4:
            // multicollect scan mode test
            {
               //_fgSaveRes = TRUE;
               _fgCompareGold = TRUE;
               _fgJdecVfyTestProgressiveScanMultiColltet = TRUE;
               _fgJdecVfyTestWritePointer = TRUE;
               _fgJdecVfyTestWritePointerUpdate = TRUE;
               _fgJdecVfyTestInterrupt = TRUE; 
               i4JdecVfyPrintf("[JDEC][Verify] multicollect scan mode test start.\n");
               vJpegVerify();
               i4JdecVfyPrintf("[JDEC][Verify] multicollect scan mode test complete.\n");
               _fgSaveRes = FALSE;
               _fgCompareGold = FALSE;
               _fgJdecVfyTestPicMode = FALSE;
               _fgJdecVfyTestProgressiveScanMultiColltet = FALSE;
               _fgJdecVfyTestWritePointerUpdate = FALSE;
               _fgJdecVfyTestInterrupt = FALSE;
            }
            break;
        case 5:
            // enhance scan mode test YC output comared wiht 29
            {
               _fgCompareGold = TRUE;
               //_fgSaveRes = TRUE;
               _fgJdecVfyTestProgressiveScanEnhance = TRUE;
               _fgJdecVfyTestWritePointer = TRUE;
               _fgJdecVfyTestWritePointerUpdate = TRUE;
               _fgJdecVfyTestInterrupt = TRUE;
               _fgJdecVfyTestProgressiveEnhancePreload = TRUE;
               _fgJdecVfyTestPicModeAYUVOutput =FALSE;
               i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode test start.\n");
               vJpegVerify();
               i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode test complete.\n");
               _fgSaveRes = FALSE;
               _fgJdecVfyTestPicModeAYUVOutput =FALSE;
               _fgCompareGold = FALSE;
               _fgJdecVfyTestPicMode = FALSE;
               _fgJdecVfyTestProgressiveScanEnhance = FALSE;
               _fgJdecVfyTestWritePointerUpdate = FALSE;
               _fgJdecVfyTestInterrupt = FALSE;
               _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
            }
            break;          
        case 6:
            // baseline picture mode ringfifo test
        	{
	        	//_fgJdecVfyTestPicModeStop = TRUE;
	        	// _fgSaveRes = TRUE;
	        	_fgCompareGold = TRUE;
	        	_fgJdecVfyTestInterrupt = FALSE;
	        	_fgJdecVfyTestPicMode = TRUE;
	        	_fgJdecVfyTestWritePointer = TRUE;
	        	_fgJdecVfyTestWritePointerUpdate = TRUE;
	        	_fgJdecVfyTestRingFifo = TRUE;
	        	i4JdecVfyPrintf("[JDEC][Verify] baseline ringfifo test start.\n");
	        	vJpegVerify();
	        	i4JdecVfyPrintf("[JDEC][Verify] baseline ringfifo test complete.\n");
	        	_fgSaveRes = FALSE;
	        	_fgJdecVfyTestPicMode = FALSE;
	        	_fgJdecVfyTestWritePointer = FALSE;
	        	_fgJdecVfyTestWritePointerUpdate = FALSE;
	        	_fgJdecVfyTestRingFifo = FALSE;
	        	_fgCompareGold = FALSE;
	        	_fgJdecVfyTestPicModeStop = FALSE;
        	}
            break;
        case 7:
            // multicollect scan ringfifo test
            {
                //_fgSaveRes = TRUE;
                _fgCompareGold = TRUE;
                _fgJdecVfyTestInterrupt = FALSE;
                _fgJdecVfyTestProgressiveScanMultiColltet = TRUE;
                _fgJdecVfyTestWritePointer = TRUE;
                _fgJdecVfyTestWritePointerUpdate = TRUE;
                _fgJdecVfyTestRingFifo = TRUE;
                i4JdecVfyPrintf("[JDEC][Verify] multicollect ringfifo test start.\n");
                vJpegVerify();
                i4JdecVfyPrintf("[JDEC][Verify] multicollect ringfifo test complete.\n");
                _fgSaveRes = FALSE;
                _fgJdecVfyTestProgressiveScanMultiColltet = FALSE;
                _fgJdecVfyTestWritePointer = FALSE;
                _fgJdecVfyTestWritePointerUpdate = FALSE;
                _fgJdecVfyTestRingFifo = FALSE;
                _fgCompareGold = FALSE;
            }
            break;
        case 8: //--failed
            //  enhance ringfifo test
           {
	           //_fgSaveRes = TRUE;
	           _fgCompareGold = TRUE;
	           _fgJdecVfyTestInterrupt = FALSE;
	           _fgJdecVfyTestProgressiveScanEnhance = TRUE;
	           _fgJdecVfyTestWritePointer = TRUE;
	           _fgJdecVfyTestWritePointerUpdate = TRUE;
	           _fgJdecVfyTestProgressiveEnhancePreload = TRUE;
	           _fgJdecVfyTestRingFifo = TRUE;
	           i4JdecVfyPrintf("[JDEC][Verify] enhance ringfifo test start.\n");
	           vJpegVerify();
	           i4JdecVfyPrintf("[JDEC][Verify] enhance ringfifo test complete.\n");
	           _fgSaveRes = FALSE;
	           _fgJdecVfyTestProgressiveScanEnhance = FALSE;
	           _fgJdecVfyTestWritePointer = FALSE;
	           _fgJdecVfyTestWritePointerUpdate = FALSE;
	           _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
	           _fgJdecVfyTestRingFifo = FALSE;
	           _fgCompareGold = FALSE;
           
           }
            break;
        case 9:
            //  baseline + multicollect mcu row error concealment
            {
               //_fgSaveRes = TRUE;
               _fgCompareGold = TRUE;
               i4JdecVfyPrintf("[JDEC][Verify] baseline + multicollect mcu row error concealment test start.\n");
               vJpegVerify();
               i4JdecVfyPrintf("[JDEC][Verify] baseline + multicollect mcu row error concealment test complete.\n");
               _fgSaveRes = FALSE;
            }
            break;
        case 10:
            //  baseline picture mode error concealment
           {
	           //_fgSaveRes = TRUE;
	           _fgCompareGold = TRUE;
	           _fgJdecVfyTestPicMode = TRUE;
	           _fgJdecVfyTestWritePointer = TRUE;
	           _fgJdecVfyTestWritePointerUpdate = TRUE;
	           i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode error concealment test start.\n");
	           vJpegVerify();
	           i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode error concealment test complete.\n");
	           _fgJdecVfyTestPicMode = FALSE;
	           _fgJdecVfyTestWritePointer = FALSE;
	           _fgJdecVfyTestWritePointerUpdate = FALSE;
	           _fgSaveRes = FALSE;
           }
            break;
        case 11:
            //   multicollect scan mode error concealment
            {
	            //_fgSaveRes = TRUE;
	            _fgCompareGold = TRUE;
	            _fgJdecVfyTestProgressiveScanMultiColltet = TRUE;
	            _fgJdecVfyTestWritePointer = TRUE;
	            _fgJdecVfyTestWritePointerUpdate = TRUE;
	            i4JdecVfyPrintf("[JDEC][Verify] multicollect picture mode error concealment test start.\n");
	            vJpegVerify();
	            i4JdecVfyPrintf("[JDEC][Verify] multicollect picture mode error concealment test complete.\n");
	            _fgJdecVfyTestProgressiveScanMultiColltet = FALSE;
	            _fgJdecVfyTestWritePointer = FALSE;
	            _fgJdecVfyTestWritePointerUpdate = FALSE;
	            _fgSaveRes = FALSE;
            }
            break;
        case 12:
            //   enhance mcu row mode error concealment
            {
	            //_fgSaveRes = TRUE;
	            _fgCompareGold = TRUE;
	            _fgJdecVfyTestProgressiveEnhance = TRUE;
	            i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row mode error concealment test start.\n");
	            vJpegVerify();
	            i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row mode error concealment test complete.\n");
	            _fgJdecVfyTestProgressiveEnhance = FALSE;
	            _fgSaveRes = FALSE;
            }
            break;
        case 13:
            //   enhance scan mode error concealment
            {
             // _fgSaveRes = TRUE;
              _fgCompareGold = TRUE;
              _fgJdecVfyTestProgressiveScanEnhance = TRUE;
              _fgJdecVfyTestWritePointer = TRUE;
              _fgJdecVfyTestWritePointerUpdate = TRUE;
              _fgJdecVfyTestProgressiveEnhancePreload = TRUE;
              i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode error concealment test start.\n");
              vJpegVerify();
              i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode error concealment test complete.\n");
              _fgJdecVfyTestProgressiveScanEnhance = FALSE;
              _fgJdecVfyTestWritePointer = FALSE;
              _fgJdecVfyTestWritePointerUpdate = FALSE;
              _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
              _fgSaveRes = FALSE;
            }
            break;
        case 14:
            //  baseline + multicollect mcu row error detection
            {
                _fgCompareGold = TRUE;
                _fgJdecVfyTestInterrupt = TRUE;
                i4JdecVfyPrintf("[JDEC][Verify] baseline + multicollect mcu row error detection test start.\n");
                vJpegVerify();
                i4JdecVfyPrintf("[JDEC][Verify] baseline + multicollect mcu row error detection test complete.\n");
                _fgJdecVfyTestInterrupt = FALSE;
            }
            break;
        case 15:
            //  baseline picture mode error detection
            {
              _fgCompareGold = TRUE;
              _fgJdecVfyTestPicMode = TRUE;
              _fgJdecVfyTestWritePointer = TRUE;
              _fgJdecVfyTestWritePointerUpdate = TRUE;
              _fgJdecVfyTestInterrupt = TRUE;
              i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode error detection test start.\n");
              vJpegVerify();
              i4JdecVfyPrintf("[JDEC][Verify] baseline picture mode error detection detection test complete.\n");
              _fgJdecVfyTestInterrupt = FALSE;
              _fgJdecVfyTestPicMode = FALSE;
              _fgJdecVfyTestWritePointer = FALSE;
              _fgJdecVfyTestWritePointerUpdate = FALSE;
            }
            break;
        case 16:
            //   multicollect scan mode error detection
            {
               _fgCompareGold = TRUE;
               _fgJdecVfyTestProgressiveScanMultiColltet = TRUE;
               _fgJdecVfyTestWritePointer = TRUE;
               _fgJdecVfyTestWritePointerUpdate = TRUE;
               _fgJdecVfyTestInterrupt = TRUE;
               i4JdecVfyPrintf("[JDEC][Verify] multicollect scan mode error detection test start.\n");
               vJpegVerify();
               i4JdecVfyPrintf("[JDEC][Verify] multicollect scan mode error detection test complete.\n");
               _fgJdecVfyTestProgressiveScanMultiColltet = FALSE;
               _fgJdecVfyTestWritePointer = FALSE;
               _fgJdecVfyTestWritePointerUpdate = FALSE;
               _fgJdecVfyTestInterrupt = FALSE;
            }
            break;
        case 17:
            //   enhance mcu row mode error detection
            {
              _fgCompareGold = TRUE;
              _fgJdecVfyTestProgressiveEnhance = TRUE;
              _fgJdecVfyTestInterrupt = TRUE;
              i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row mode error detection test start.\n");
              vJpegVerify();
              i4JdecVfyPrintf("[JDEC][Verify] enhance mcu row mode error detection test complete.\n");
              _fgJdecVfyTestProgressiveEnhance = FALSE;
              _fgJdecVfyTestInterrupt = FALSE;
            }
            break;
        case 18:
            //   enhance scan mode error detection
           {
	           _fgCompareGold = TRUE;
	           _fgJdecVfyTestProgressiveScanEnhance = TRUE;
	           _fgJdecVfyTestWritePointer = TRUE;
	           _fgJdecVfyTestWritePointerUpdate = TRUE;
	           _fgJdecVfyTestProgressiveEnhancePreload = TRUE;
	           _fgJdecVfyTestInterrupt = TRUE;
	           i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode error detection test start.\n");
	           vJpegVerify();
	           i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode error detection test complete.\n");
	           _fgJdecVfyTestProgressiveScanEnhance = FALSE;
	           _fgJdecVfyTestWritePointer = FALSE;
	           _fgJdecVfyTestWritePointerUpdate = FALSE;
	           _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
	           _fgJdecVfyTestInterrupt = FALSE;
           }
            break;
        case 19:
        // video output test //for MT8580 new feature
        {  
        	fgDisableErrConceal = TRUE;
           _fgCompareGold = TRUE;
           //_fgSaveRes = TRUE;
           _fgJdecVfyTestVideoOutMode = TRUE;
           _fgJdecVfyTestInterrupt = TRUE;
           i4JdecVfyPrintf("[JDEC][Verify] baseline video output mode with golden compare test start.\n");
           vJpegVerify();
           i4JdecVfyPrintf("[JDEC][Verify] baseline video output mode with golden compare test complete.\n");
           _fgSaveRes = FALSE;
           _fgJdecVfyTestInterrupt = FALSE;  
           _fgJdecVfyTestVideoOutMode = FALSE;
           _fgCompareGold =  FALSE;   
        } 
       break;   
#if JDEC_MM_MODE_ENABLE  
	   case 20:
       {
	   	 //_fgSaveRes = TRUE;
         _fgJdecVfyTestProgressiveMCUMulitCollectMultiPass = TRUE;
         _fgJdecVfyTestWritePointer = TRUE;
         _fgJdecVfyTestWritePointerUpdate = TRUE;
         _fgJdecVfyTestInterrupt = TRUE;
		 _fgCompareGold = TRUE;		 
		 _fgJdecVfyTestRingFifo = FALSE;
         i4JdecVfyPrintf("[JDEC][Verify] multicollect MUC ROW mode error detection test start.\n");
         vJpegVerify();
         i4JdecVfyPrintf("[JDEC][Verify] multicollect MUC ROW mode error detection test complete.\n");
      	_fgSaveRes = FALSE;
         _fgJdecVfyTestProgressiveMCUMulitCollectMultiPass = FALSE;
         _fgJdecVfyTestWritePointer = FALSE;
         _fgJdecVfyTestWritePointerUpdate = FALSE;
         _fgJdecVfyTestInterrupt = FALSE;
		 _fgCompareGold = FALSE;
		  _fgJdecVfyTestRingFifo = FALSE;
        }
		break;
     case 21:
		     {
	   	 //_fgSaveRes = TRUE;
         _fgJdecVfyTestProgressiveMCUMulitCollectMultiPass = TRUE;
         _fgJdecVfyTestWritePointer = TRUE;
         _fgJdecVfyTestWritePointerUpdate = TRUE;
         _fgJdecVfyTestInterrupt = FALSE;
		 _fgCompareGold = TRUE;		 
		 _fgJdecVfyTestRingFifo = TRUE;
         i4JdecVfyPrintf("[JDEC][Verify] multicollect  MUC ROW  mode rifing fifo error detection test start.\n");
         vJpegVerify();
         i4JdecVfyPrintf("[JDEC][Verify] multicollect  MUC ROW  mode rifing fifo error detection test complete.\n");
      	_fgSaveRes = FALSE;
         _fgJdecVfyTestProgressiveMCUMulitCollectMultiPass = FALSE;
         _fgJdecVfyTestWritePointer = FALSE;
         _fgJdecVfyTestWritePointerUpdate = FALSE;
         _fgJdecVfyTestInterrupt = FALSE;
		 _fgCompareGold = FALSE;
		  _fgJdecVfyTestRingFifo = FALSE;
        }
		break;
#endif	
		
		case 22:
			  // enhance scan mode test
			  {
				 //_fgCompareGold = TRUE;
				 _fgSaveRes = TRUE;
		  _fgJdecVfyTestProgressiveScanEnhance = TRUE;
			 _fgJdecVfyTestWritePointer = TRUE;
				_fgJdecVfyTestWritePointerUpdate = TRUE;
		  _fgJdecVfyTestInterrupt = TRUE;
		  _fgJdecVfyTestProgressiveEnhancePreload = TRUE;
			  _fgJdecVfyTestPicModeAYUVOutput =TRUE;
				  i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode test start.\n");
				  vJpegVerify();
				  i4JdecVfyPrintf("[JDEC][Verify] enhance scan mode test complete.\n");
		  _fgSaveRes = FALSE;
				_fgJdecVfyTestPicModeAYUVOutput =FALSE;
				_fgCompareGold = FALSE;
		  _fgJdecVfyTestPicMode = FALSE;
			 _fgJdecVfyTestProgressiveScanEnhance = FALSE;
				_fgJdecVfyTestWritePointerUpdate = FALSE;
		  _fgJdecVfyTestInterrupt = FALSE;
		  _fgJdecVfyTestProgressiveEnhancePreload = FALSE;
			  }
			  break;
	default:
            // Descript test case
            i4JdecVfyPrintf("[JDEC][Verify] no such test case.\n");
            break;
  	}

    UTIL_Printf("*************_ui4JdecDecTotalTime = [%lld]******************\n",_ui8JdecDecTotalTime);
}
INT32 i4SaveOrignFile(void)
{
    INT32 i4Ret;
    char bFileName[JDEC_FILE_NAME_LEN];
    //INT32 i4FileHandle;
    //JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };

    strcpy(bFileName,(char *)_abJpegFileList[_u4CurJpegFileListNum]);
     strcat(bFileName,".error.jpg");
    i4JdecVfyPrintf("[Jpeg][Verify] Save orign Buffer File %s\n",bFileName);
    i4Ret = i4JdecVfyWriteFile(bFileName,(UINT32)_EmuJpgFifo,_i4OrgJpegFilesize);
        return i4Ret;

}
