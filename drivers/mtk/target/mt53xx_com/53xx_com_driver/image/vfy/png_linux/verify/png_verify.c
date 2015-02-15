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
/*
* \par Project
*    MT8520
*
* \par Description
*    Png Decoder Verification Code
*
* \par Author_Name
*    CJYANG
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
#if 0
#include "x_common.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_rtos.h"
#include "x_bim.h"
#include "x_hal_ic.h"
#include "rvd_util.h"
#include "png_hal_dec.h"
#include "png_debug.h"
#include "x_mmap.h"
#include "x_hal_1176.h"
#include "x_debug.h"
#include "x_timer.h"
#include "sys_config.h"
#include "asm/uaccess.h"
#include <mach/cache_operation.h>
#include <linux/module.h>
#if CONFIG_SYS_MEM_PHASE2|| PNG_MMU_TEST
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3 
#include "x_kmem.h"
#endif

#endif
#define PNG_SUPPORT_CROSS_MEM 0
//#include "x_rand.h"
#include "x_iommu.h"
#include "png_hal_dec.h"
#include "png_debug.h"
#include "drv_img_dec.h"

#include "x_drv_map.h"
#include "x_assert.h"
#include "drv_config.h"

#include "x_stl_lib.h"
#include "x_bim.h"
#if CONFIG_DRV_LINUX
#include "x_os.h"
#else
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include "x_assert.h"
#include "x_printf.h"
#include "x_hal_arm.h"
#include "x_ckgen.h"
#include "x_debug.h"
#include "x_drv_cli.h"
#include "../../../../../../mt5880/diag/UDVT_IF.h"
#define MT8520_IMGDEC_PNG_VFY

#if  (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
//#include "UDVT_IF.h"
#define CONFIG_FATS_SUPPORT  1
#else
#define CONFIG_FATS_SUPPORT  0
#endif
/**************************/
#define DEC_IMAGE_LARGE         0
#define PNG_ERR_DEBUG           1
/****************************/

#define i4PngPrintf Printf
#define i4RvdPrintf Printf
extern INT32  i4PngHalTransVfy(UINT32 u4HwInstId);
extern INT32  i4PngHalCheckSumSwithVfy(UINT32 u4HwInstId, UINT32 u4Case);

#if   PNG_MMU_TEST
extern int x_kmem_sync_table(void *ptr, size_t size);
#define PNG_MMU_SYNC_TABLE(a, b)   x_kmem_sync_table(a, b)
//extern void* x_alloc_vmem(UINT32 u4Size);
//extern void* x_free_vmem(void* pUser);
#endif
//////////////////// Test Defined ////////////////////////////////////// 
typedef struct {
    UINT32 u4DataLength;
    UINT32 u4Type;
    UINT32 u4CRC;
    BOOL   fgLastChunk;
} PngChunkInfo;

typedef struct {
    UINT32 u4Width;
    UINT32 u4Height;
    UINT8  u1BitDepth;
    UINT8  u1ColorType;
    BOOL   fgInterlace;
} PngIHDRHeader;

typedef struct {
    UINT32 u4DispLeft;
    UINT32 u4DispTop;
    UINT32 u4DispWidth;
    UINT32 u4DispHeight;
} PngDispPara;

typedef struct {
    UINT32 u4ClipLeft;
    UINT32 u4ClipTop;
    UINT32 u4ClipWidth;
    UINT32 u4ClipHeight;
    BOOL   fgClipEn;
} PngClipPara;
typedef  struct
{
        UINT16   bfType;
        UINT32   bfSize;
        UINT16   bfReserved1;
        UINT16   bfReserved2;
        UINT32   bfOffBits;
} BmpBITMAPFILEHEADER;
typedef  struct
{
        UINT32      biSize;
        UINT32      biWidth;
        UINT32      biHeight;
        UINT16      biPlanes;
        UINT16      biBitCount;
        UINT32      biCompression;
        UINT32      biSizeImage;
        UINT32      biClrUsed;
        UINT32      bfReserved1;
        UINT32      biClrImportant;
		UINT32      bfReserved2;
		UINT32      bR;
		UINT32      bG;
		UINT32      bB;
		UINT32      bA;
} BmpBITMAPINFOHEADER;

extern unsigned long get_drvmem_mem(void);
#if (!PNG_MMU_TEST)
#define PNG_BIG_MEM_USE 1
#define PNG_STM_IN_SIZE     24*1024*1024//6*1024*1024
#define PNG_STM_OUT_SIZE    66*1024*1024 //4096:66M
#define PNG_GOLDEN_DATA     66*1024*1024
#else
#define PNG_BIG_MEM_USE 0
#define PNG_STM_IN_SIZE      6*1024*1024//6*1024*1024
#define PNG_STM_OUT_SIZE     10*1024*1024
#define PNG_GOLDEN_DATA      10*1024*1024
#endif 
#define PNG_LZ77_SIZE       32*1024
#define PNG_LINE_BUF_SIZE   4096 * 8 //width * pixel length

#define PNG_RING_SIZE       (64*1024)
#define PNG_RING_FIFO_SIZE  ((PNG_RING_SIZE * 32) / 2)
#define PNG_SLICE_HEIGHT    10
#define PNG_COLOR_BUF_SIZE  PNG_SLICE_HEIGHT * 4096 * 8



#define       PNG_PERFORMANCE_TEST         0//0//
#define       PNG_NEW_PARSING              1//  1   //if PNG_PERFORMANCE_TEST == 1, it = 0
#define       PNG_MMU_WT_TOGTHER           1

#define       PNG_INVALID32          0xFFFFFFFF
#define       PNG_DECODE_WAIT_TIME   300000

UINT32          au4RegBBuf[2];
UINT32          au4RegABuf[2];
UINT32          u4CntDecode = 0;
//////////////////// Global Variable ///////////////////////////////////
 #if PNG_MMU_TEST 
     //extern void* IOMMU_get_fragment_vmem(UINT32 u4Size);
   //#define IOMMU_get_fragment_vmem  x_alloc_vmem
      #define PNG_VFY_MEM_FREE x_free_vmem
   #elif CONFIG_SYS_MEM_PHASE2
        #define PNG_VFY_MEM_FREE x_free_aligned_dma_mem
   #elif CONFIG_SYS_MEM_PHASE3
      #define PNG_VFY_MEM_FREE x_free_aligned_nc_mem   
  #endif	  

#define IMAGE_BASE 0xf0000000
#define PngWriteREG_vfy(base, arg, val)   IO_WRITE32(IMAGE_BASE, ((base + arg) & 0xFFFFF), val)
#define PngReadREG_vfy(base, arg)         IO_READ32(IMAGE_BASE, ((base + arg) & 0xFFFFF))


PNG_HAL_PIC_INFO_T rPngHalPicInfo;
PNG_HAL_IDAT_INFO_T rPngHalIdatInfo;
//PNG_HDR_INFO_T rPngHdrInfo;
PNG_PLTE     tPngPlte;
PNG_TRNS     tPngTrns;  
BOOL         _fgDecPngFile = FALSE;
BOOL         _fgPngDecUseRingFifo = FALSE;
BOOL         _fgPngDecUseColorBuf =FALSE;
BOOL         _fgLoadingfile = TRUE;
BYTE*        _pbPngFilePreFix;
BYTE*        _pbPngFileMidFix;
UINT32       _u4PngGroupFileNum;
UINT32       _u4RegBuf;


UINT32 u4CheckSumRunCnt =1; //only used for checksum compare

#if !CONFIG_FATS_SUPPORT
#define PNG_READ_FROM_USB 1
#else
#define PNG_READ_FROM_USB 0
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)


BYTE _arbPngGoldenPostFix[6][32] = {
#if PNG_READ_FROM_USB
   "/gold_argb1555.bin",
   "/gold_argb4444.bin",
   "/gold_argb8888.bin",
   "/gold_pltt_idx.bin",
   "/check_sum.bin",
   "WTOffsetGolden.bin"
#else 
    "\\gold_argb1555.bin",
    "\\gold_argb4444.bin",
    "\\gold_argb8888.bin",
    "\\gold_pltt_idx.bin",
    "\\check_sum.bin",
    "WTOffsetGolden.bin"
#endif    
};
#else
BYTE _arbPngGoldenPostFix[7][32] = {
    "\\gold_argb1555.bin",
    "\\gold_rgb565.bin",
    "\\gold_argb4444.bin",
    "\\gold_argb8888.bin",
    "\\gold_pltt_idx.bin",
    "\\check_sum.bin",
    "WTOffsetGolden.bin"
};
#endif

PngChunkInfo    _rPngChunkInfo ;
PNG_IHDR        _rPPngIHDRHeader = {0,0,0,0,0};
PngDispPara     _rPngDispPara;
PngClipPara     _rPngClipPara;
UINT32          _u4PngImgDataAddr;
UINT32          _u4PngHorScale = 0;
UINT32          _u4PngVrtScale = 0;
UINT32          _u4PngIntScale = 7;
GFX_COLORMODE_T _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
BOOL            _fgOurClrPalette = FALSE;
UINT32          _u4PngPixelByte = 4;
UINT32          _u4PngOutStmCount = 0;
UINT32          _u4PngInStmCount = 0;
UINT32          _u4PngColorBufSize = 0;
BOOL            _fgPngPalette;
BOOL            _fgPngTranEn;
BOOL            _fgPngDispOut = FALSE;
UINT32          _u4PngIDATCount = 0;
UINT32          _u4CropNum= 0;
INT32           _iPngTestPattenGroup = 0;
UINT8          *_pu1PngPlt;
UINT8          *_pu1PngTrans;
BOOL            fgCompareChecksum = FALSE;
#ifdef MT8520_IMGDEC_PNG_VFY
UINT8          *_pbPngStrmIn;
UINT8          *_pbPngStrmIn1;
UINT8          *_pbPngStrmOut;
UINT8          *_pu4PngWbHdrMem;
UINT8          *_pbPngGoldenData;
UINT8          *_pbPngClrBuf[2];
UINT8          *_pbPngRingFifo;
#ifdef PNG_MMU_TEST
UINT8           *_pbPngStrmInApp;
UINT8          *_pbPngStrmOutApp;
#endif
#endif

#define PNG_VFY_NORMAL_ALL  1 //test all output format
#define PNG_CLIP_TEST_EN  0
#define PNG_DST_NOTALIGN_TEST_EN 0
#define PNG_GRACERESET_EN  0


#if PNG_DST_NOTALIGN_TEST_EN
UINT8          * _pbPngStrmOutNew;
#endif

#define       PNG_PERFORMANCE_TEST         0//0//
#define       PNG_NEW_PARSING              1//  1   //if PNG_PERFORMANCE_TEST == 1, it = 0
#define       PNG_MMU_WT_TOGTHER           1

#define       PNG_INVALID32          0xFFFFFFFF
#define       PNG_DECODE_WAIT_TIME   300000
//#define       PngReadREG(arg)        IO_READ32(IMAGE_BASE, (arg & 0x3FFF))

UINT32        _u4PngOutFormatSelect = 0x8;
BOOL          fgPngAutoskipEnable = TRUE;
BOOL          fgResumeEnable      = FALSE;
BOOL          fgResizeEnable      = FALSE;
BOOL          fgPngOutFormatTest  = FALSE;
UINT32        u4ResizeAndCropTimes = 0;
UINT32        u4ResizeAndCropTimesForWt = 0;
BOOL          fgLoadData = FALSE;
UINT32        u4FileCnt = 0;
BOOL          fgWtEnable = TRUE;///FALSE;   /////////////////////////////////
BOOL          fgTransVery = FALSE;
BOOL         fgTestU11U18 = FALSE;
BOOL          fgMemCrossChannel = FALSE;
BOOL           fgTestCheckSum = FALSE;	
BOOL           fgTest1M4Kswitch = FALSE;

INT32          _u4CheckSumIdex=0;
UINT32        u4DstShift = 0;
UINT32       _u4PreDstBufStartVal = 0;
UINT32       _u4PreSrcBufStartVal = 0;
UINT32       _u4HwInistId = PNG_INVALID32;
BYTE         _arbPngFile[512];
                                     //UINT32 iiii=0;

#if PNG_PERFORMANCE_TEST
HAL_TIME_T   _rPtxStart = {0};
HAL_TIME_T   _rPtxEnd = {0};
HAL_TIME_T   _rTest = {0};
HAL_TIME_T   _rTime = {0};
extern void HAL_GetTime(HAL_TIME_T* pTime);
extern void HAL_GetDeltaTime(HAL_TIME_T * pResult, HAL_TIME_T * pOlder, HAL_TIME_T * pNewer);
#endif

UINT32       _u4PngUnMatchedRegVal = 0xFF;
UINT32       _u4BitStreamCount = 0;
UINT32       _u4PngSliceCount = 0;
UINT32       _u4PngGoldenSize = PNG_GOLDEN_DATA;
UINT8        _PicNum = 48;
BYTE         _arbPngGoldenFile[512];
BYTE         _arbPngLargeFile[512];   ////////////////////////for large image
UINT32       DebugReg[100];
#if PNG_ERR_DEBUG
BYTE         _arbDebugFile[512];
#endif
BYTE         _arbPngCheckSumFile[512];
UINT32       u4SilceNum = 0;
#define      PngFileListMax 2000
UINT8*       _pbPngFileList;
UINT32 _pbPngGoldenData1;
UINT32       _u4PngFileListNum = 0;
UINT32       _u4CurPngFileListNum = 0;
BYTE *       _abPngFileList[PngFileListMax];
INT32        i4CurrentGoldenType = 0;
BOOL         _fgPNG_SAVE_TO_BMP  = FALSE;//if save decoded result to bmp format
UINT32       u4FifoReadPoint = 0;
#if PNG_MMU_TEST
extern UINT32 u4HalGetTTB(UINT32 ui4_mem_ptr);
UINT32       ui4_tmp0;
UINT32       ui4_tmp1;
UINT32       ui4_tmp2;
#endif
#define PNG_READ_FILE
#ifdef PNG_READ_FILE
#if !CONFIG_FATS_SUPPORT
   #if !PNG_READ_FROM_USB
       #define PNG_VFY_FILE_OPEN(a,b)   fopen(a,b)
       #define PNG_VFY_FILE_TELL(a)    ftell(a) 
       #define PNG_VFY_FILE_READ(a,b,c,d)    fread((void*)a,b,c,d)
       #define PNG_VFY_FILE_SEEK(a,b,c)   fseek(a,b,c)
       #define PNG_VFY_FILE_CLOSE(a)   fclose(a)	
       #define PNG_VFY_FILE_WRITE(a,b,c,d)   fwrite((void*)a,b,c,d)	
   #else
	  static INT32 DrvFsTell(INT32 iFd)
   	  {
   	    UINT32 u4RealReadSize=0;
        if ((DrvFSUSBGetFileSize(iFd, &u4RealReadSize)) < 0)
       	{
       	  UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSGetFileSize USB Error!!!!!! \n");\
       	  return -1;
       	}
		return u4RealReadSize;
   	  }
	   static INT32 DrvFsOpen(char *filename, char *mode)
       {  
          UINT32 u4DrvNo = 0;
          UINT32 u4DrvFSTag = 0;
          INT32 i4Fd =0;
		  INT32 i4Flag = DRV_FS_RW_C;
       //   if((DrvFSUSBMount(u4DrvNo, &u4DrvFSTag)) < 0)\
        //  {
        //       UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSMount USB Error!!!!!! \n");\
        //       return 0;
        //  }    
		  if(0==strcmp("rb", mode))
		  {
		    UTIL_Printf("[PNG_VERIFY][i4PngReadFile] READ only mode!!!!!! \n");\
		    i4Flag = DRV_FS_RDONLY;
		  }
		  if ((DrvFSUSBOpenFile(filename, i4Flag, &i4Fd)) < 0)
		  {
		   UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSOpenFile USB Error!!!!!! \n");
		//   if (DrvFSUSBUnMount() < 0)\
	    //    {\
	     //       UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSUnMount  Error!!!!!! \n");\
	      //      return 0;\
	      //  }\
		    return 0;
		  }
		  return i4Fd;
       }  
	   
       #define PNG_VFY_FILE_OPEN(a,b) DrvFsOpen(a,b)
       #define PNG_VFY_FILE_TELL(a)    DrvFsTell(a) 
       #define PNG_VFY_FILE_READ(a,b,c,d)    DrvFSUSBReadFile(d,( void* )a,b)
 #define PNG_VFY_FILE_SEEK(a,b,c)   DrvFSUSBSeekFile(a,b,c)
       #define PNG_VFY_FILE_WRITE(a,b,c,d)   DrvFSUSBWriteFile(d,(const void *)a,c) ; 
	       //(void*)a,b,c,d)	
	    #define PNG_VFY_FILE_CLOSE(a)   DrvFSUSBCloseFile(a)
	   // {\
			//if (DrvFSUSBCloseFile(a) < 0)\
	      //  {\
	       //     UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSCloseFile  Error!!!!!! \n");\
	       //     return -1;\
	      //  }\
	      //  if (DrvFSUSBUnMount() < 0)\
	       // {\
	         //   UTIL_Printf("[PNG_VERIFY][i4PngReadFile] DrvFSUnMount  Error!!!!!! \n");\
	         //   return -1;\
	      //  }\
	   // }
   #endif
#else 
  // #include "UDVT_IF.h"
   #define PNG_VFY_FILE_OPEN(a,b)     UDVT_IF_OpenFile(a, b)   
   #define PNG_VFY_FILE_TELL(a)      UDVT_IF_GetFileLength((UINT32)a)
   #define PNG_VFY_FILE_READ(a,b,c,d)     UDVT_IF_ReadFile((void*)a,(UINT32)b,(UINT32)c,(UINT32)d) 
   #define PNG_VFY_FILE_SEEK(a,b,c)      UDVT_IF_SeekFile((UINT32)a,(INT32)b,(UINT32)c) 
   #define PNG_VFY_FILE_CLOSE(a)   UDVT_IF_CloseFile((UINT32)a)
   #define PNG_VFY_FILE_WRITE(a,b,c,d)  UDVT_IF_WriteFile((void*)a,(UINT32)b,(UINT32)c,(UINT32)d) 	
#endif 

#define SEEK_SET        0       /* Seek from beginning of file.  */
#define SEEK_CUR        1       /* Seek from current position.  */
#define SEEK_END        2       /* Set file pointer to EOF plus "offset" */

INT32 i4PngReadFile(
      #if PNG_READ_FROM_USB
      char *pcFileName,
      #else
      const char *pcFileName,
      #endif
      UINT32 u4FileBufSa,
      UINT32 u4BufSize)
{
#if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
       FILE* fFileHandle = NULL;
#else
       UINT32 fFileHandle= 0;
#endif 
	 UINT32 u4PatternSize =0;
        UTIL_Printf(" =========i4PngReadFile %s\n",pcFileName);
	  fFileHandle = PNG_VFY_FILE_OPEN(pcFileName, "rb");
#if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
	 if(fFileHandle == NULL)
#else 
	 if(fFileHandle == 0)
#endif	 	
{
         UTIL_Printf(" fail =========open %s\n",pcFileName);
	    return 0;
}
        PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_END);
        u4PatternSize = PNG_VFY_FILE_TELL(fFileHandle);
        PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
        PNG_VFY_FILE_READ(u4FileBufSa, u4PatternSize, 1, fFileHandle);
        PNG_VFY_FILE_CLOSE(fFileHandle);
	 UTIL_Printf(" =========i4PngReadFile %s, sz %d OK\n",pcFileName, u4PatternSize);
	 return u4PatternSize;
}

#define i4RvdReadFile i4PngReadFile
#endif
//#define PNG_SAVE_LOG_TO_FILE

UINT32 u4PngUpdateFifoRdPoint(UINT32 u4Advance)
{
    if ((u4FifoReadPoint + u4Advance) >= PNG_RING_FIFO_SIZE)
    {
        u4FifoReadPoint = (u4FifoReadPoint + u4Advance) - PNG_RING_FIFO_SIZE;
    }
    else
    {
        u4FifoReadPoint += u4Advance;
    }

    return 1;
}

UINT32 u4PngGet4Bytes(UINT8 *pu1Src)
{
    UINT32 u4Ret = 0;

    u4Ret += ((UINT32)(pu1Src[0])) << 24;
    u4Ret += ((UINT32)(pu1Src[1])) << 16;
    u4Ret += ((UINT32)(pu1Src[2])) << 8;
    u4Ret += ((UINT32)(pu1Src[3]));
    return u4Ret;
}

UINT16 u2PngGet2Bytes(UINT8 *pu1Src)
{
    UINT16 u2Ret = 0;
    u2Ret += ((UINT32)(pu1Src[0])) << 8;
    u2Ret += ((UINT32)(pu1Src[1]));
    return u2Ret;
}


void vSetPngIHDR(UINT8 *pu1Src, PNG_IHDR *ptPngIHDRHeader)
{
    ptPngIHDRHeader->u4Width = u4PngGet4Bytes(&pu1Src[0]);
    ptPngIHDRHeader->u4Height = u4PngGet4Bytes(&pu1Src[4]); 
    ptPngIHDRHeader->u1BitDepth = pu1Src[8];
    ptPngIHDRHeader->u1ColorType = pu1Src[9];
    if (pu1Src[12] == 1)
    {
        ptPngIHDRHeader->fgInterlace = TRUE;
    }
    else
    {
        ptPngIHDRHeader->fgInterlace = FALSE;
        u4SilceNum  = (ptPngIHDRHeader->u4Height + PNG_SLICE_HEIGHT -1)/PNG_SLICE_HEIGHT;
    }
    UTIL_Printf(" *************u1ColorType[0x%x]\n", ptPngIHDRHeader->u1ColorType );
}


void PNG_Dump(void)
	{
	    UINT32 ii =0;
	   UTIL_Printf(" -------------PNG--4E00-------------------------------------\n");
	   for (ii=0;ii<64 ;ii++)
		UTIL_Printf(" 0x%x: 0x%x\n",(0x6e000+4*ii) ,PngReadREG_vfy(0x6e000,4*ii));
	   UTIL_Printf(" -------------PNG--4F00-------------------------------------\n");
	   for (ii=0;ii<37 ;ii++)
		UTIL_Printf(" 0x%x: 0x%x\n",(0x6e100+4*ii), PngReadREG_vfy(0x6e100,4*ii));
	   UTIL_Printf(" ----------------------------------------------------\n");
	 
	}

	void MMU_Dump(void)
	{
     //UTIL_Printf(" ----------------------------------------------------\n");
	}


void vPngInitPicInfor(PNG_HAL_PIC_INFO_T *prPngHalPicInfo)
{
    UINT32 u4RegTmpValue = 0;
	vPngLog_L("Enter %s line %d\n", __FUNCTION__, __LINE__);
    if (_fgPngDecUseRingFifo)
    {
        #if PNG_MMU_TEST
        u4RegTmpValue = (UINT32)_pbPngRingFifo;
        #else
        u4RegTmpValue = ((UINT32)_pbPngRingFifo);
        #endif
        prPngHalPicInfo->u4FifoEnd = u4RegTmpValue + PNG_RING_FIFO_SIZE;
        //if(fgPngAutoskipEnable)
        {
          prPngHalPicInfo->u4FifoEnd += 16;
        }
    }
    else
    {
        #if PNG_MMU_TEST
        u4RegTmpValue = (UINT32)_pbPngStrmIn;
        #else
        u4RegTmpValue = ((UINT32)_pbPngStrmIn);
        #endif		        
         prPngHalPicInfo->u4FifoEnd = u4RegTmpValue + PNG_STM_IN_SIZE;
    }	
    prPngHalPicInfo->u4FifoStart = u4RegTmpValue;
    prPngHalPicInfo->u4AlphaUnMatchedVal = _u4PngUnMatchedRegVal;
  
    if (_fgPngDispOut)
    {
     prPngHalPicInfo->u4DstBufPitch = 2000*4;
     prPngHalPicInfo->u4SliceHeight  = _rPPngIHDRHeader.u4Height;
    }
    else
    {
        prPngHalPicInfo->u4DstBufPitch = _rPPngIHDRHeader.u4Width * _u4PngPixelByte;

        if (_fgPngDecUseColorBuf)
        {
            prPngHalPicInfo->u4SliceHeight  = PNG_SLICE_HEIGHT;
        }
        else
        {
            prPngHalPicInfo->u4SliceHeight  = _rPPngIHDRHeader.u4Height;
        }
    }
    
    //crop, default the function is enable
    if (_rPngClipPara.fgClipEn)
    {
       prPngHalPicInfo->u4CropX  = _rPngClipPara.u4ClipLeft;
       prPngHalPicInfo->u4CropY  = _rPngClipPara.u4ClipTop;
       prPngHalPicInfo->u4CropWidth  = _rPngClipPara.u4ClipWidth;
       prPngHalPicInfo->u4CropHeight = _rPngClipPara.u4ClipHeight;
    }
    else
    {
       prPngHalPicInfo->u4CropX  = 0;
       prPngHalPicInfo->u4CropY  = 0;
       prPngHalPicInfo->u4CropWidth  = _rPPngIHDRHeader.u4Width;
       prPngHalPicInfo->u4CropHeight = _rPPngIHDRHeader.u4Height;
    }

    prPngHalPicInfo->u4ResizeHor = _u4PngHorScale;
    prPngHalPicInfo->u4ResizeVrt  = _u4PngVrtScale;
    prPngHalPicInfo->eOutClrMd = _eOutClrMd;

}


void vPngSetIDATDecReg(INT32 _u4HwInistId , UINT32 u4ChunkType, BOOL fgLastIDAT,   PNG_HAL_IDAT_INFO_T *prPngHalIdatInfo)
{
    UINT32 u4ValidDataLen = 0;
	//UINT32 ii =0 ;
	 vPngLog_L("Enter %s line %d\n", __FUNCTION__, __LINE__);
    prPngHalIdatInfo->u4ChunkType  = u4ChunkType;
    prPngHalIdatInfo->fgLastIDATGrp = fgLastIDAT;
    //prPngHalIdatInfo->fg1stIDATChunk
  //  Printf("_u4PngImgDataAddr == 0x%x ===\n",_u4PngImgDataAddr);
    prPngHalIdatInfo->u4SrtStrmAddr = (UINT32)(_pbPngStrmIn + _u4PngImgDataAddr);
    prPngHalIdatInfo->u4SrtStrmLen   = _rPngChunkInfo.u4DataLength;

    _u4PreSrcBufStartVal = prPngHalIdatInfo->u4SrtStrmAddr ;
    
    if (!_fgPngDecUseColorBuf)
    {
        prPngHalIdatInfo->fgLastSlice = TRUE;
    }
    else
    {
        prPngHalIdatInfo->fgLastSlice = FALSE;
	if(u4SilceNum == _u4PngSliceCount)
           prPngHalIdatInfo->fgLastSlice = TRUE;
    }

    if (_fgPngDecUseRingFifo)
    {
        u4FifoReadPoint = 0;
        prPngHalIdatInfo->fgLastIDATGrp  = FALSE;
        if (PNG_RING_SIZE < _rPngChunkInfo.u4DataLength)
        {
           u4ValidDataLen = PNG_RING_SIZE;
           _rPngChunkInfo.u4DataLength -= PNG_RING_SIZE;
           x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), PNG_RING_SIZE);
	   u4PngUpdateFifoRdPoint(PNG_RING_SIZE);
           _u4PngImgDataAddr += PNG_RING_SIZE;
        
        }
        else
        {
           if(_rPngChunkInfo.fgLastChunk)
           prPngHalIdatInfo->fgLastIDATGrp  = TRUE;
           u4ValidDataLen = _rPngChunkInfo.u4DataLength;
           if(u4ValidDataLen == 0)
           {
             return ;
           }
           else
           {
             x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4ValidDataLen);	      
           }
           _u4PngImgDataAddr += u4ValidDataLen;
           _rPngChunkInfo.u4DataLength = 0;
       }	
    	prPngHalIdatInfo->u4SrtStrmLen   = u4ValidDataLen ;
        prPngHalIdatInfo->u4SrtStrmAddr = (UINT32)_pbPngRingFifo;
#if !PNG_CACHE_MAP_ENABLE
       BSP_FlushDCacheRange(prPngHalIdatInfo->u4SrtStrmAddr, u4ValidDataLen);
#endif 
    }
    _u4BitStreamCount = 0;
    if(fgPngAutoskipEnable)
    {
         if(prPngHalIdatInfo->fgLastIDATGrp)
            i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
         prPngHalIdatInfo->fgLastIDATGrp = FALSE;
    }
    if(fgTransVery)
    {
        i4PngHalTransVfy(_u4HwInistId);
    }
    i4PngHalCheckSumSwithVfy(_u4HwInistId, 0);
    if(fgTestCheckSum)
    {
	   i4PngHalCheckSumSwithVfy(_u4HwInistId, _u4CheckSumIdex);
    }
   if(fgTestU11U18)
   {
      extern INT32  i4PngHalDramU11U18SwithVfy(UINT32 u4HwInstId, BOOL fgU18);
      static BOOL fgU18Enable = TRUE;
       i4PngHalDramU11U18SwithVfy(_u4HwInistId,fgU18Enable);
	fgU18Enable = !fgU18Enable;
   }
  #if !PNG_CACHE_MAP_ENABLE
    BSP_CleanDCacheRange((UINT32)_pbPngStrmIn, PNG_STM_IN_SIZE);
  #endif 
#if PNG_DST_NOTALIGN_TEST_EN
     i4PngHalPitchEnable(_u4HwInistId, TRUE);
#endif
    vPngPngHalUseTmpDstBufer(_u4HwInistId, TRUE); 	
    i4PngHalProcIDAT(_u4HwInistId, prPngHalIdatInfo);
//	Printf("===1=====\n");

}


BOOL fgPngBitStreamEmptyResume(UINT32 u4BitStreamResumeCount)
{
    #if (!PNG_MMU_TEST)
    UINT32 u4SrcStrmSrtAddr = ((UINT32)_pbPngStrmIn);
    #else
    UINT32 u4SrcStrmSrtAddr = ((UINT32)_pbPngStrmIn);
    #endif
    UINT32 u4SrcStrmLen;
    BOOL fgLastIDATGroup = FALSE;
    UINT32 u4TempValue = 0;  
    if (!_fgPngDecUseRingFifo)//if not use fifo , go decode finish
    {
          return FALSE;
    }

    if (PNG_RING_SIZE < _rPngChunkInfo.u4DataLength)
    {
        u4SrcStrmLen = PNG_RING_SIZE;
        _rPngChunkInfo.u4DataLength -= PNG_RING_SIZE;
        if( (PNG_RING_FIFO_SIZE - u4FifoReadPoint) >= PNG_RING_SIZE)
           x_memcpy((void *)(_pbPngRingFifo + u4FifoReadPoint), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), PNG_RING_SIZE);
        else
        {
           u4TempValue = PNG_RING_FIFO_SIZE -u4FifoReadPoint;
           x_memcpy((void *)(_pbPngRingFifo + u4FifoReadPoint), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4TempValue );
           x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr + u4TempValue), PNG_RING_SIZE-u4TempValue  );
        }
        u4SrcStrmSrtAddr = (UINT32)_pbPngRingFifo + u4FifoReadPoint;	 
        u4PngUpdateFifoRdPoint(PNG_RING_SIZE);
        _u4PngImgDataAddr += PNG_RING_SIZE;
          
    }
    else
    {
        if(_rPngChunkInfo.fgLastChunk)
        fgLastIDATGroup = TRUE;
        u4SrcStrmLen = _rPngChunkInfo.u4DataLength;
        if(u4SrcStrmLen == 0)
        {
             return FALSE;
        }
        else
        {
            if( (PNG_RING_FIFO_SIZE - u4FifoReadPoint) >= u4SrcStrmLen)
                x_memcpy((void *)(_pbPngRingFifo + u4FifoReadPoint), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4SrcStrmLen);
            else
            {
                u4TempValue = PNG_RING_FIFO_SIZE -u4FifoReadPoint;
                x_memcpy((void *)(_pbPngRingFifo + u4FifoReadPoint), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4TempValue );
                x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr+u4TempValue), u4SrcStrmLen-u4TempValue );
            }
            u4SrcStrmSrtAddr = (UINT32)_pbPngRingFifo + u4FifoReadPoint;
	     u4PngUpdateFifoRdPoint(PNG_RING_SIZE);
        }
         _u4PngImgDataAddr += u4SrcStrmLen;
	 _rPngChunkInfo.u4DataLength = 0;
    }
  #if !PNG_CACHE_MAP_ENABLE	
   BSP_FlushDCacheRange((UINT32)u4SrcStrmSrtAddr, u4SrcStrmLen);
#endif
   if(fgPngAutoskipEnable)
   {
	if(fgLastIDATGroup)
        i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
     fgLastIDATGroup = FALSE;
   }
    i4PngHalProcNextSrc(_u4HwInistId, u4SrcStrmSrtAddr, u4SrcStrmLen, fgLastIDATGroup);
    return TRUE;
}



BOOL fgPngWaitDecFinish(void)
{
    UINT8 *pbSrc;
    UINT8 *pbDst;
    INT32 i4Ret;
    UINT32 u4TmpRegVal = 0;
    UINT32 u4BitStreamResumeCount = 0;
    BOOL   fgIfLastSlice = FALSE;
    INT32 i4=0;  
  //  UINT32 u4Tempval;
  
    UTIL_Printf("<png> trigger,wait %d\n", PNG_DECODE_WAIT_TIME);
    i4Ret = i4PngHalWaitDec(_u4HwInistId, PNG_DECODE_WAIT_TIME);	
    while(1)
   {
    switch(i4Ret)
    {
        case PNG_HAL_RTN_DEC:
           {
	    //#if PNG_USE_NEW_CLR_BUF
	    if(_fgPngDecUseColorBuf)
	    {  
                if(fgWtEnable)//if dst resume.	need set pixel byte number exactly
                  {
                    _u4PngPixelByte = 2;
                  }
                //copy memory
                pbSrc = _pbPngClrBuf[(_u4PngSliceCount-1)%2];
                pbDst = _pbPngStrmOut + (_u4PngSliceCount-1) * (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT);
                 for (i4=0; i4<(_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT); i4++)
                {
                    pbDst[i4] = pbSrc[i4];
                }
                u4TmpRegVal = (UINT32)(_pbPngClrBuf[_u4PngSliceCount%2]);
                _u4PngSliceCount++;
                  if(fgWtEnable)//if dst resume.	need set pixel byte number exactly
                  {
                    _u4PngPixelByte = 4;
                  }
	     }
	     //#endif
#if (!PNG_PERFORMANCE_TEST)		
              vPngLog_L( "[PNG_EMU] Decode Done %x\n", i4Ret);  
#endif   
              //i4PngPrintf( "[PNG_EMU] 1st Decode Done Register Setting\n\n\n"); 
			    // PNG_Dump();			
             u4CntDecode ++;
              goto PNG_DecFinish;
            }
            //break;
        case PNG_HAL_RTN_SRC:
           {
            //i4PngPrintf( "[PNG_EMU] bit stream empty %x\n", i4Ret);
            if (!fgPngBitStreamEmptyResume(u4BitStreamResumeCount))
            {
                goto PNG_DecFinish;
            }
            i4Ret = i4PngHalWaitDec(_u4HwInistId, PNG_DECODE_WAIT_TIME);
            u4BitStreamResumeCount++;
            }
            break;
        case PNG_HAL_RTN_DST:
           {
             // i4PngPrintf( "[PNG_EMU] Slice Done --%x\n", i4Ret);
            
		//#if PNG_USE_NEW_CLR_BUF               
		//#endif
                //resume DST
		//#if PNG_USE_NEW_CLR_BUF
                //copy memory
                if(_fgPngDecUseColorBuf)
		{
                   if(fgWtEnable)//if dst resume.	need set pixel byte number exactly
                   {
                     _u4PngPixelByte = 2;
                   }
		  
                pbSrc = _pbPngClrBuf[(_u4PngSliceCount-1)%2];
                pbDst = _pbPngStrmOut + (_u4PngSliceCount-1) * (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT);
                for (i4=0; i4<(_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT); i4++)
                {
                    pbDst[i4] = pbSrc[i4];
                }
                   #if (!PNG_MMU_TEST)
                   u4TmpRegVal = ((UINT32)((_pbPngClrBuf[_u4PngSliceCount%2])));
                   #else
                    u4TmpRegVal = ((UINT32)((_pbPngClrBuf[_u4PngSliceCount%2])));
                   #endif
                   x_memset((void *)(_pbPngClrBuf[_u4PngSliceCount%2]), 0, PNG_COLOR_BUF_SIZE);
                   _u4PngSliceCount++;
                   if(fgWtEnable)//if dst resume.	need set pixel byte number exactly
                   {
                     _u4PngPixelByte = 4;
                   }
                   if(u4SilceNum == _u4PngSliceCount)
                    fgIfLastSlice = TRUE;
	        }
                else
		{
                _u4PreDstBufStartVal += (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT);
                u4TmpRegVal = _u4PreDstBufStartVal;
		}
		//#endif
            //    PngWriteREG(PNG_DEC_PIXEL_OUT_XY_ADDR_REG, u4TmpRegVal);
          #if !PNG_CACHE_MAP_ENABLE 
             BSP_InvDCacheRange(u4TmpRegVal, PNG_COLOR_BUF_SIZE);
	#endif	  
	       i4PngHalProcNextDst(_u4HwInistId, u4TmpRegVal, u4TmpRegVal + (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT), fgIfLastSlice);
               i4Ret = i4PngHalWaitDec(_u4HwInistId, PNG_DECODE_WAIT_TIME);
            }
            break;
        default:
            {  

              i4PngPrintf( "[PNG_EMU] ********** Alter Err %x\n", i4Ret);
              i4PngPrintf( "[PNG_EMU] Decoded register value @ Err!\n");
   

	      //PNG_Dump();       /////////////////////////////for debug
              i4PngPrintf( "\n\n");

              goto  PNG_DecFinish;
            }
           // break;
    }
  }
    PNG_DecFinish:
    return TRUE;
}

#if CONFIG_DRV_VERIFY_SUPPORT
extern UINT8 _png_trans_bg_r;
extern UINT8 _png_trans_bg_g;
extern UINT8 _png_trans_bg_b;
extern UINT8 _png_trans_key_r;
extern UINT8 _png_trans_key_g;
extern UINT8 _png_trans_key_b;  
extern UINT8 _png_trans_alph_match;  
extern BOOL _png_use_org_alpha;
#endif 

BOOL fgPngCompareTransition(UINT8 *pu1StrmOut, UINT8* pu1Golden,  UINT32 u4RealReadSize)
{
  UINT32 i=0;
  UINT32 u4Cnt =0;
  BOOL fgRet = TRUE;
  if(!fgTransVery)
  {
    return FALSE;
  }
  u4Cnt =0;
  UTIL_Printf("[PNG_VFY]===Compare transparent size=0x%x \n",
  	u4RealReadSize);
  for(i=0; i<u4RealReadSize/4; i+=4)
  {
     if((pu1Golden[i] == _png_trans_key_b) &&
	 	(pu1Golden[i+1] == _png_trans_key_g) &&
	 	(pu1Golden[i+2] == _png_trans_key_r))
     {
       u4Cnt++;
       if((pu1StrmOut[i] != _png_trans_bg_b) ||
	     (pu1StrmOut[i+1] != _png_trans_bg_g) ||
	    (pu1StrmOut[i+2] != _png_trans_bg_r))
       {
	     UTIL_Printf("[PNG_VFY]: 11 Compare transparent failed==transparent error position =0x%x\n", i);	
	     UTIL_Printf("[PNG_VFY]:Golden :0x%x:0x%x:0x%x:0x%x\n", 
								 pu1Golden[i],
								 pu1Golden[i+1],
								 pu1Golden[i+2],
								 pu1Golden[i+3]); 
	   UTIL_Printf("[PNG_VFY]:Decod :0x%x:0x%x:0x%x:0x%x\n", 
								 pu1StrmOut[i],
								 pu1StrmOut[i+1],
								 pu1StrmOut[i+2],
								  pu1StrmOut[i+3]);
            fgRet = FALSE;
	        goto TRANS_CMP_EXIT;
       }	
       else
       {
       	UTIL_Printf("<PNG_VFY> Compare transparent ok1\n");
       }
	   if(_png_use_org_alpha)
	   {
	     if(pu1StrmOut[i+3]!= pu1Golden[i+3])
	     {
	         UTIL_Printf("[PNG_VFY]: 22 Compare transparent failed==transparent error position =0x%x\n", i);			   
		     UTIL_Printf("[PNG_VFY]:Golden :0x%x:0x%x:0x%x:0x%x\n", 
							   pu1Golden[i],
							   pu1Golden[i+1],
							   pu1Golden[i+2],
							   pu1Golden[i+3]); 
						   UTIL_Printf("[PNG_VFY]:Decod :0x%x:0x%x:0x%x:0x%x\n", 
							   pu1StrmOut[i],
							   pu1StrmOut[i+1],
							   pu1StrmOut[i+2],
							   pu1StrmOut[i+3]);			   
		    fgRet = FALSE;
		    goto TRANS_CMP_EXIT;
	     }
	       else
	       {
	       	UTIL_Printf("<PNG_VFY> Compare transparent ok2\n");
	     }
	   }
	   else
	   {
	      if(pu1StrmOut[i+3]!= _png_trans_alph_match)
	      {
	         UTIL_Printf("[PNG_VFY]: 33 Compare transparent failed==transparent error position =0x%x\n", i);			 
			 UTIL_Printf("[PNG_VFY]:Golden :0x%x:0x%x:0x%x:0x%x\n", 
								 pu1Golden[i],
								 pu1Golden[i+1],
								 pu1Golden[i+2],
								 pu1Golden[i+3]); 
							 UTIL_Printf("[PNG_VFY]:Decod :0x%x:0x%x:0x%x:0x%x\n", 
								 pu1StrmOut[i],
								 pu1StrmOut[i+1],
								 pu1StrmOut[i+2],
								 pu1StrmOut[i+3]);				 
			 fgRet = FALSE;
		     goto TRANS_CMP_EXIT;
	      }
	       else
	       {
	       	UTIL_Printf("<PNG_VFY> Compare transparent ok3\n");
	       }
	   }
     } 	
     else 
      {
   
         if((pu1Golden[i] != pu1StrmOut[i]) ||
            (pu1Golden[i+1] != pu1StrmOut[i+1])||
            (pu1Golden[i+2] != pu1StrmOut[i+2])||
            (pu1Golden[i+3] != pu1StrmOut[i+3]))
            {

				UTIL_Printf("[PNG_VFY]:44 Compare transparent failed==transparent error position =0x%x\n", i);	
				UTIL_Printf("[PNG_VFY]:Golden :0x%x:0x%x:0x%x:0x%x\n", 
					pu1Golden[i],
					pu1Golden[i+1],
					pu1Golden[i+2],
					pu1Golden[i+3]); 
			    UTIL_Printf("[PNG_VFY]:Decod :0x%x:0x%x:0x%x:0x%x\n", 
					pu1StrmOut[i],
					pu1StrmOut[i+1],
					pu1StrmOut[i+2],
					pu1StrmOut[i+3]); 				

				fgRet = FALSE;
		        goto TRANS_CMP_EXIT;
            }
      }
   }
 TRANS_CMP_EXIT:
 	#if 1
 	{
        #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
        FILE* fFileHandle = NULL;
        #else
        UINT32 fFileHandle= 0;
        #endif 
	UINT32 bCount =0;
	 BYTE *pbTemp = NULL;
	 if(!fgRet)
	 {
	   PNG_Dump();
	 }
	 else
	 {
	      UTIL_Printf("[PNG_VFY]:44 Compare transparent correct\n");
	 }
	 UTIL_Printf("and compared 0x%x times\n", u4Cnt);	
        x_memset(_arbDebugFile, 0, 128);
        pbTemp =	_arbPngGoldenFile;
        bCount = 0;
        while (*pbTemp != '\0')
        {
           pbTemp ++;
           bCount ++;
        }
        x_memset(_arbDebugFile, 0, 128);
        x_memcpy(_arbDebugFile, _arbPngGoldenFile,bCount);
        x_memcpy(&_arbDebugFile[bCount],".trans",x_strlen(".trans"));											
        fFileHandle = PNG_VFY_FILE_OPEN(_arbDebugFile, "wb");
        #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB  
        if (fFileHandle == NULL)
        #else 
        if (fFileHandle == 0)
        #endif 
        {
           UTIL_Printf("[PNG_EMU] Write tranparent result to File Fail \n");
        }
        PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
        PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4,fFileHandle );
        PNG_VFY_FILE_CLOSE(fFileHandle);
        UTIL_Printf("[PNG_EMU]: write transparent result to file \n");		
       
    }
#endif 	
    return fgRet;
}

//*********************************************************************
//Function    : BOOL fgPngCompareGolden(UINT8 *, UINT8 *)
//Description : compare decoded data with golden data
//Parameter   : 
//Return      : N/A
//*********************************************************************
BOOL fgPngCompareGolden(UINT8 *pu1StrmOut, UINT8* pu1Golden,  UINT32 u4RealReadSize)
{
    INT32 i = 0;
    INT32 j = 0;
    UINT32 bCount;
    BYTE* pbTemp;
 #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
    FILE* fFileHandle = NULL;
 #else
    UINT32 fFileHandle= 0;
 #endif 	
    UINT32 u4CurrentX, u4CurrentY;
    UINT32 u4PngHResize = (1 << _u4PngHorScale);
    UINT32 u4PngVResize = (1 << _u4PngVrtScale);
    UINT32 u4IdxTmp;
    UINT32 u4CompDataPixNum = 0;
	

    if(fgCompareChecksum || fgWtEnable)
    {
       for (i=0; i < u4RealReadSize; i++)
       {
           if (pu1StrmOut[i] != pu1Golden[i])
           {
              i4PngPrintf( "[PNG_EMU] Compare data fail at %d out 0x%x golden 0x%x\n",
	      	        i, pu1StrmOut[i], pu1Golden[i]);
	          // PNG_Dump();       /////////////////////////////for debug
              //MMU_Dump();
	       pbTemp =	_arbPngGoldenFile;
		bCount = 0;
		while (*pbTemp != '\0')
		{
			 pbTemp ++;
			 bCount ++;
		}
  		x_memset(_arbDebugFile, 0, 128);
  		x_memcpy(_arbDebugFile, _arbPngGoldenFile,bCount);
  		x_memcpy(&_arbDebugFile[bCount],".error",x_strlen(".error"));
												
       	   fFileHandle = PNG_VFY_FILE_OPEN(_arbDebugFile, "wb");
                #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
                if (fFileHandle == NULL)
                #else 
                if (fFileHandle == 0)
                #endif 
       	   {
       		  UTIL_Printf("[PNG_EMU] Write Err	result to File Fail \n");
       	   }
          //x_memcpy((void*)_pbPngGoldenData1, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
       	   PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
       	   PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, u4RealReadSize,fFileHandle );
       	   PNG_VFY_FILE_CLOSE(fFileHandle);
       	   UTIL_Printf("[PNG_EMU]: write Err result to file \n");
	   x_thread_delay(100);
              return FALSE;
           }
        }
    }	
    else
    {
      //i4PngPrintf( "[PNG_EMU] _u4PngHorScale %d\n", _u4PngHorScale);
      //i4PngPrintf( "[PNG_EMU] _u4PngVrtScale %d\n", _u4PngVrtScale);

//--
    for (i=0; i<(_rPPngIHDRHeader.u4Height*_rPPngIHDRHeader.u4Width); i++)
    {
        u4CurrentX = (i) % _rPPngIHDRHeader.u4Width;             ////X
        u4CurrentY = (i) / _rPPngIHDRHeader.u4Width;   /////Y
        if (_rPngClipPara.fgClipEn)
        {
            if (((u4CurrentX >= _rPngClipPara.u4ClipLeft) && (u4CurrentX < (_rPngClipPara.u4ClipLeft +  _rPngClipPara.u4ClipWidth))) &&
                ((u4CurrentY >= _rPngClipPara.u4ClipTop) && (u4CurrentY < (_rPngClipPara.u4ClipTop +  _rPngClipPara.u4ClipHeight))))
            {
                UINT32 u4XTmp = u4CurrentX - _rPngClipPara.u4ClipLeft;
                UINT32 u4YTmp = u4CurrentY - _rPngClipPara.u4ClipTop;
                if (((u4XTmp % u4PngHResize) == 0) && ((u4YTmp % u4PngVResize) == 0))
                {
                    u4IdxTmp = ((u4YTmp / u4PngVResize) * (_rPPngIHDRHeader.u4Width)) + (u4XTmp / u4PngHResize);
                    for (j=0; j<_u4PngPixelByte; j++)
                    {
                        if (j == (_u4PngPixelByte - 1))
                        {
                            switch (_u4PngOutFormatSelect)
                            {
                                case 0x1: //8bppidx
                                    break;
                                case 0x2: //1555
                                    //pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] = pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] & 0x7F;
                                    break;
                                case 0x4: //565
                                    break;
                                case 0x8: //4444
                                    //pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] = pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] & 0xF;
                                    break;
                                case 0x10://8888
                                    //pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] = pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] & 0xFF;
                                    break;
                            }
                        }
                        if (pu1StrmOut[u4IdxTmp*_u4PngPixelByte+j] != pu1Golden[i*_u4PngPixelByte+j])
                        {      
                            i4PngPrintf( "[PNG_EMU] compare fail at %d\n", i);
                            PNG_Dump();       /////////////////////////////for debug	
                            MMU_Dump();			
							pbTemp =	_arbPngGoldenFile;
							bCount = 0;
							while (*pbTemp != '\0')
							{
								 pbTemp ++;
								 bCount ++;
							}
							x_memset(_arbDebugFile, 0, 128);
							x_memcpy(_arbDebugFile, _arbPngGoldenFile,bCount);
							x_memcpy(&_arbDebugFile[bCount],".error",x_strlen(".error"));
												
						   fFileHandle = PNG_VFY_FILE_OPEN(_arbDebugFile, "wb");
                                      #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
						   if (fFileHandle == NULL)
                                      #else 
                                            if (fFileHandle == 0)
					   #endif 
						   {
							  UTIL_Printf("[PNG_EMU] Write Err	result to File Fail \n");
						   }
					   //x_memcpy((void*)_pbPngGoldenData1, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
						   PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
						   PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4,fFileHandle );
						   PNG_VFY_FILE_CLOSE(fFileHandle);
						   UTIL_Printf("[PNG_EMU]: write Err result to file \n");								
							
                            return FALSE;
                        }
                    }
                    u4CompDataPixNum++;
                }
            }
        }
        else
        {
            for (j=0; j<_u4PngPixelByte; j++)
            {
                if (pu1StrmOut[i*_u4PngPixelByte+j] != pu1Golden[i*_u4PngPixelByte+j])
                {
                				UINT32 bCount;
				              BYTE* pbTemp;
                		          #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
                                         FILE* fFileHandle = NULL;
                                     #else
                                         UINT32 fFileHandle= 0;
                                     #endif 			
                                              i4PngPrintf( "[PNG_EMU] compare fail at %d\n", i);
					  PNG_Dump();       /////////////////////////////for debug	
                                           MMU_Dump();
							  pbTemp =  _arbPngGoldenFile;
							 bCount = 0;
							 while (*pbTemp != '\0')
							 {
								 pbTemp ++;
								 bCount ++;
							 }
								 x_memset(_arbDebugFile, 0, 128);
					                  x_memcpy(_arbDebugFile, _arbPngGoldenFile,bCount);
							 x_memcpy(&_arbDebugFile[bCount],".error\0",x_strlen(".error\0"));
						
							   fFileHandle = PNG_VFY_FILE_OPEN((char*)_arbDebugFile, "wb");
       					    #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB  
							   if (fFileHandle == NULL)
                                             #else 
                                                   if (fFileHandle == 0)
       					   #endif 
							   {
								  UTIL_Printf("[PNG_EMU] Write Err	result to File Fail \n");
							   }
							   //x_memcpy((void*)_pbPngGoldenData1, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
							   PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
							   PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4,fFileHandle );
							   PNG_VFY_FILE_CLOSE(fFileHandle);
							   UTIL_Printf("[PNG_EMU]: write Err result to file %s\n", _arbDebugFile);

					//i4PngPrintf("PngVfyLog1.txt", "%s\n", _abPngFileList[_u4CurPngFileListNum]);

                    return FALSE;
                }
            }
        }
    }
}
    //i4PngPrintf( "[PNG_EMU] compare ok! at %d, %d, Compared Pix Total Number = %d \n", i,j,u4CompDataPixNum);

    return TRUE;
}


void vPngHwSetPLTE(UINT32 u4HwInstId, UINT32 u4ChunkType, UINT32 u4ChunkSize,   PNG_PLTE *ptPngPlte)
{
    //set plte information
    INT32 i4tempValue = 0;
    ptPngPlte->u4ChunkType = u4ChunkType;
    ptPngPlte->u4ChunkSize  = u4ChunkSize;
    x_memcpy((void *)(ptPngPlte->u4PlteData), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4ChunkSize);
    //due to cache problem, setting TS data in non-cache memory
    //ptPngPlte->u4PlteData = (UINT32)(_pbPngStrmIn + _u4PngImgDataAddr);
   #if !PNG_CACHE_MAP_ENABLE
    BSP_FlushDCacheRange((UINT32)ptPngPlte->u4PlteData, u4ChunkSize);
  #endif 
    i4tempValue = i4PngHalSetPLTE(u4HwInstId, ptPngPlte);
    if (i4tempValue != PNG_HAL_RTN_OK)
    {
        i4PngPrintf( "[PNG_EMU]read PALETTE error\n");
    }
}
//*********************************************************************
//Function     : BOOL fgPngSave2Bmp(BOOL)
//Description : save png decoded data to BMP fomate
//Parameter  : 
//Return        : N/A
//*********************************************************************
#if 0
BOOL fgPngSave2Bmp(BOOL fgGolden)
{
	BmpBITMAPFILEHEADER bitmapfileheader;
	BmpBITMAPINFOHEADER bitmapinfoheader;
	UINT32 *pu4BmpTgtData = (UINT32*)_pbPngStrmOut;
	UINT16 *pu2BmpTgtData = (UINT16*)_pbPngStrmOut;
       UINT32 u4FILE = 0;
       BOOL fgResult = FALSE;
       UINT32 BMInfoHdrSize = 56;
       INT32 _u4TgtBufW    = 0;
       INT32 _u4TgtBufH     = 0;
       BYTE arbBmpSave[128];

       _u4TgtBufW = _rPPngIHDRHeader.u4Width;
       _u4TgtBufH  = _rPPngIHDRHeader.u4Height;
       
	// Initialize bit map file header
	bitmapfileheader.bfType = 0x4d42;
	bitmapinfoheader.biSizeImage = _u4TgtBufW*_u4TgtBufH*4;
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 70;
   	bitmapfileheader.bfOffBits = 0;
	// Initialize bit map info header
	bitmapinfoheader.biSize = 40;
	bitmapinfoheader.biWidth = _u4TgtBufW;
	bitmapinfoheader.biHeight = _u4TgtBufH;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biCompression = 3;
	bitmapinfoheader.biClrUsed = 0;
	bitmapinfoheader.bfReserved1 = 0;
	bitmapinfoheader.biClrImportant = 0;
	bitmapinfoheader.bfReserved2 = 0;

    switch(_eOutClrMd)
    {
        case GFX_COLORMODE_ARGB_D1555:
        	bitmapinfoheader.biBitCount = 16;
        	bitmapinfoheader.biSizeImage = _u4TgtBufW*_u4TgtBufH*2;
        	bitmapfileheader.bfSize = 14 + 56 + _u4TgtBufW*_u4TgtBufH*2;
        	bitmapinfoheader.bR = 0x00007C00;
        	bitmapinfoheader.bG = 0x000003E0;
        	bitmapinfoheader.bB = 0x0000001F;
        	bitmapinfoheader.bA = 0x00008000;
            x_strcpy((CHAR *)arbBmpSave, (const CHAR *)"E:/BMP_ARGB1555.bmp");
            break;
        case GFX_COLORMODE_RGB_D565:
        	bitmapinfoheader.biBitCount = 16;
        	bitmapinfoheader.biSizeImage = _u4TgtBufW*_u4TgtBufH*2;
        	bitmapfileheader.bfSize = 14 + 56 + _u4TgtBufW*_u4TgtBufH*2;
        	bitmapinfoheader.bR = 0x0000F800;
        	bitmapinfoheader.bG = 0x000007E0;
        	bitmapinfoheader.bB = 0x0000001F;
        	bitmapinfoheader.bA = 0x00000000;
            x_strcpy((CHAR *)arbBmpSave, (const CHAR *)"E:/BMP_RGB565.bmp");
            break;
        case GFX_COLORMODE_ARGB_D4444:
        	bitmapinfoheader.biBitCount = 16;
        	bitmapinfoheader.biSizeImage = _u4TgtBufW*_u4TgtBufH*2;
        	bitmapfileheader.bfSize = 14 + 56 + _u4TgtBufW*_u4TgtBufH*2;
        	bitmapinfoheader.bR = 0x00000F00;
        	bitmapinfoheader.bG = 0x000000F0;
        	bitmapinfoheader.bB = 0x0000000F;
        	bitmapinfoheader.bA = 0x0000F000;
            x_strcpy((CHAR *)arbBmpSave, (const CHAR *)"E:/BMP_ARGB4444.bmp");
            break;
        case GFX_COLORMODE_ARGB_D8888:
        	bitmapinfoheader.biBitCount = 32;
        	bitmapinfoheader.biSizeImage = _u4TgtBufW*_u4TgtBufH*4;
        	//bitmapfileheader.bfOffBits = 14 + 56;
        	bitmapfileheader.bfSize = 14 + 56 + _u4TgtBufW*_u4TgtBufH*4;
        	bitmapinfoheader.bR = 0x00FF0000;
        	bitmapinfoheader.bG = 0x0000FF00;
        	bitmapinfoheader.bB = 0x000000FF;
        	bitmapinfoheader.bA = 0xFF000000;
        	BMInfoHdrSize = 56;
            x_strcpy((CHAR *)arbBmpSave, (const CHAR *)"E:/BMP_ARGB8888.bmp");
            break;
        default:
            break;
    }

    u4FILE = i4RvdFopen((char *)arbBmpSave, "w");
    if (0 == u4FILE)
      return FALSE;

    fgResult = i4RvdFwrite(u4FILE, (UINT32)(&bitmapfileheader), 14);
    fgResult = i4RvdFwrite(u4FILE, (UINT32)(&bitmapinfoheader), BMInfoHdrSize);

    if (_eOutClrMd == GFX_COLORMODE_ARGB_D8888)
    {
        fgResult = i4RvdFwrite(u4FILE, (UINT32)(pu4BmpTgtData), (_u4TgtBufW*_u4TgtBufH*4));
    }
    else
    {
        fgResult = i4RvdFwrite(u4FILE, (UINT32)(pu2BmpTgtData), (_u4TgtBufW*_u4TgtBufH*2));
    }

    i4RvdFclose(u4FILE);

    if (FALSE == fgResult)
    {
        //MTKPrintf("write fail: %s", arbBmpSave);
    }

    return fgResult;
}
#endif

//*********************************************************************
//Function    : void vPngDecode(void)
//Description : Png Emu Decode  loop
//Parameter   : 
//Return      : N/A
//*********************************************************************
BOOL fgPngDecode(void)
{
    UINT8 *pu1Fifo = (UINT8*)_pbPngStrmIn;
    UINT32 u4PngFileCount = 0;  //DWRD alignment
    UINT32 u4ChunkSize;
    UINT32 u4ChunkType;
    UINT32 u4NextChunkType;
#if PNG_NEW_PARSING
    UINT32 u4Png1stIDATOffset = 0;
    UINT32 u4PngIDATEndOffset = 0;
#endif				
#if PNG_GRACERESET_EN
    static unsigned int fgGraceReset = FALSE;
#endif
    _u4PngOutStmCount = 0;
    _u4PngInStmCount = 0;
    _fgPngPalette = FALSE;
    _fgPngTranEn = FALSE;
    _fgPngDecUseRingFifo = FALSE;
    _fgPngDecUseColorBuf = FALSE;	
    _u4PngIDATCount = 0;
    _u4PngSliceCount = 0;
    i4PngHalGetHwInst(&_u4HwInistId);
    i4PngHalEnable(_u4HwInistId);
#if PNG_MMU_TEST    
    UTIL_Printf("sync table\n");
    PNG_MMU_SYNC_TABLE(_pbPngStrmOut, PNG_STM_OUT_SIZE);
    PNG_MMU_SYNC_TABLE(_pbPngStrmIn, PNG_STM_IN_SIZE);        
    PNG_MMU_SYNC_TABLE(_pbPngRingFifo, PNG_RING_FIFO_SIZE);
#endif
    i4PngHalReset(_u4HwInistId);
    
    vPngLog_L("Enter fgPngDecode, autoskip[%d] \n", fgPngAutoskipEnable);	
    u4PngFileCount += 8; //PNG signature
    u4PngFileCount += 8; //size + IHDR
    vSetPngIHDR(&pu1Fifo[u4PngFileCount], &_rPPngIHDRHeader);
    rPngHalPicInfo.ptIhdr = &_rPPngIHDRHeader;
    u4PngFileCount += (13 + 4); //IHDR data + CRC
    
    if(fgWtEnable)//setting WT enable register
    {
       fgPngWTSet(_u4HwInistId, TRUE);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
        i4CurrentGoldenType = 5;//if need save WT result,set this flage.
#else
       i4CurrentGoldenType = 6;//if need save WT result,set this flage.
#endif
       _u4PngPixelByte = 4;
       _u4PngHorScale = 0;
       _u4PngVrtScale = 0;
       _u4PngIntScale = 7;
    }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)

#endif
#if PNG_NEW_PARSING
    if(fgResumeEnable)
    {
    if (_rPPngIHDRHeader.fgInterlace)
    {
        UTIL_Printf("!!!The Bitstream is interlaced!!!\n");
        _fgPngDecUseRingFifo = TRUE;
    }
    else
    {
    	UTIL_Printf("!!!The Bitstream is Not interlaced!!!\n");
        _fgPngDecUseRingFifo = TRUE;
        _fgPngDecUseColorBuf = TRUE;
    }
    }
#endif
   #if PNG_DST_NOTALIGN_TEST_EN
   if (_rPPngIHDRHeader.fgInterlace)
   {
   	UTIL_Printf("<png>Interlace Don't test dst not align\n");
	return FALSE;
   }
   #endif
    //  
    x_memset((void *)(tPngTrns.u4TrnsData), 0, 256*3);
    x_memset((void *)(tPngPlte.u4PlteData), 0, 256*3);
    x_memset((void *)_pbPngClrBuf[0], 0, PNG_COLOR_BUF_SIZE);
    x_memset((void *)_pbPngClrBuf[1], 0, PNG_COLOR_BUF_SIZE);
#if PNG_DST_NOTALIGN_TEST_EN
    x_memset((void *)_pbPngStrmOutNew, 0, PNG_STM_OUT_SIZE);
#else
    x_memset((void *)_pbPngStrmOut, 0, PNG_STM_OUT_SIZE);
#endif

    vPngInitPicInfor(&rPngHalPicInfo);
    if(!_fgPngDecUseRingFifo)
   {
       vPngPngHalSetRingFifo(_u4HwInistId, rPngHalPicInfo.u4FifoStart, rPngHalPicInfo.u4FifoEnd);
   }
   else
   {
      vPngPngHalSetRingFifo(_u4HwInistId, rPngHalPicInfo.u4FifoStart, rPngHalPicInfo.u4FifoEnd-16);
   }
    i4PngHalSetPicInfo(_u4HwInistId, &rPngHalPicInfo);
    if(fgResizeEnable && (!fgWtEnable))
    {
      if(_rPPngIHDRHeader.fgInterlace && (_u4PngIntScale == 1))
      {      
        i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, 0, 0);        
      }
	  else
      {
        i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, _u4PngHorScale, _u4PngVrtScale);
      }
    }
    if(!fgPngAutoskipEnable)
    {
       i4PngHalSetAutoSkipDisable(_u4HwInistId);
    }
    if (_fgPngDecUseColorBuf)
    {
	rPngHalIdatInfo.u4DstBufStart = (UINT32)(_pbPngClrBuf[_u4PngSliceCount%2]);
        rPngHalIdatInfo.u4DstBufEnd   =    rPngHalIdatInfo.u4DstBufStart + _rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT;
        _u4PngSliceCount ++;		
    }
    else
    {
         rPngHalIdatInfo.u4DstBufStart = (UINT32)(_pbPngStrmOut );
        rPngHalIdatInfo.u4DstBufEnd   =    rPngHalIdatInfo.u4DstBufStart + PNG_STM_OUT_SIZE;
        _u4PreDstBufStartVal = (UINT32)(_pbPngStrmOut);
    }

      _rPngChunkInfo.fgLastChunk = FALSE;
    while (1)
    {
        u4ChunkSize = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4;
        u4ChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4;
        _rPngChunkInfo.u4Type = u4ChunkType;
        _rPngChunkInfo.u4DataLength = u4ChunkSize;
        _u4PngImgDataAddr = u4PngFileCount;
        if (u4ChunkType == 0x49444154)        //IDAT
        {
            if ((!_fgPngPalette) && (_fgOurClrPalette))             
            {
                i4PngPrintf( "[PNG_EMU]No palette in PNG file\n");
                return FALSE;
            }
#if PNG_NEW_PARSING
            if(fgPngAutoskipEnable)
            {
            if (u4Png1stIDATOffset == 0)
            {
                u4Png1stIDATOffset = _u4PngImgDataAddr;
            }
            u4PngIDATEndOffset = u4PngFileCount + u4ChunkSize + 4;
            }			         
#endif
            u4NextChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount+u4ChunkSize+4+4]); //"current + size + crc" + "length"
            if (u4NextChunkType == 0x49444154)  //IDAT
            {
             if(!fgPngAutoskipEnable)
             {
                if (_u4PngIDATCount == 0)
                {
                    rPngHalIdatInfo.fg1stIDATChunk = TRUE;
                }
                else
                {
                    rPngHalIdatInfo.fg1stIDATChunk = FALSE;
                }

                vPngSetIDATDecReg(_u4HwInistId, _rPngChunkInfo.u4Type, FALSE, &rPngHalIdatInfo);
				
                _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount + u4ChunkSize ]);
                fgPngWaitDecFinish();
                x_thread_delay(2);
             }                                  //////²»Ö´ÐÐ
                _u4PngIDATCount++;
            }
            else
            {
                u4PngFileCount += u4ChunkSize; // chunk data
                _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
                u4PngFileCount += 4; // crc
                _rPngChunkInfo.fgLastChunk = TRUE;
                break;
            }                                      

        }
        else if (u4ChunkType == 0x504C5445)   //PLTE
        {
            i4PngPrintf("[Png][Verify]  PLTE--------EXIST\n");
            _fgPngPalette = TRUE;
            u4NextChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount+u4ChunkSize+4+4]);
            if (u4NextChunkType == 0x74524E53)  //tRNS
            {
                rPngHalPicInfo.u4AlphaUnMatchedVal = 0xFF;
            }
            vPngHwSetPLTE(_u4HwInistId, _rPngChunkInfo.u4Type, _rPngChunkInfo.u4DataLength, &tPngPlte);//set plte value
        }
        else if (u4ChunkType == 0x74524E53)   //tRNS
        {
            INT32 i4Ret ;
	       i4PngPrintf("[Png][Verify]  tRNS--------EXIST\n");
            tPngTrns.u4ChunkType = _rPngChunkInfo.u4Type;
            tPngTrns.u4ChunkSize  = _rPngChunkInfo.u4DataLength;
            x_memcpy((void *)(tPngTrns.u4TrnsData), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), tPngTrns.u4ChunkSize);
            i4Ret = i4PngHalSettRNS(_u4HwInistId, &tPngTrns, _fgPngPalette);
            if (i4Ret == PNG_HAL_RTN_FAIL)
            {
                    i4PngPrintf( "[PNG_EMU]read TRNS error\n");
                    return FALSE;
             }
            _fgPngTranEn = TRUE;
        }
        else
        {
            //skip this chunk
        }
        u4PngFileCount += u4ChunkSize; // chunk data
        _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4; // crc
    }
    if ((!_fgPngPalette) && (_fgOurClrPalette))
    {
        i4PngPrintf( "[PNG_EMU]No palette in PNG file\n");
        return FALSE;
    }
    if(fgPngAutoskipEnable)
    {
    _u4PngImgDataAddr = u4Png1stIDATOffset - 8;
    _u4PngIDATCount = 0;
    _rPngChunkInfo.u4DataLength = u4PngIDATEndOffset - _u4PngImgDataAddr;
    }

     if (_u4PngIDATCount == 0)
     {
         rPngHalIdatInfo.fg1stIDATChunk = TRUE;
     }
     else
    {
         rPngHalIdatInfo.fg1stIDATChunk = FALSE;
     }
    
    if(fgPngAutoskipEnable)
    {
    i4PngHalSetAutoSkipEnable(_u4HwInistId);
	
    if(_fgPngDecUseRingFifo)
        i4PngHalSetAutoSkipLastIdat(_u4HwInistId, FALSE);
    else
        i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
    }
         //set register
#if PNG_PERFORMANCE_TEST
    HAL_GetTime(&_rPtxStart);
#endif		
    vPngSetIDATDecReg(_u4HwInistId, _rPngChunkInfo.u4Type, TRUE, &rPngHalIdatInfo);
	//PNG_Dump(); 
  
#if PNG_GRACERESET_EN
   if(FALSE == fgGraceReset)
   {
	UTIL_Printf("<png> GraceReset Trigger \n");
        i4PngHalGraceReset(_u4HwInistId);
        while(!fgPngGraceResetDone(_u4HwInistId))
        {
		UTIL_Printf("<png> GraceReset Don't Done \n");
		x_thread_delay(1);
	}
	fgGraceReset = TRUE;
	UTIL_Printf("<png> GraceReset Reset Ok\n");
	return FALSE;
   }
   else
   {
   	fgGraceReset = FALSE;
      UTIL_Printf("<png> GraceReset Test Normal \n");
   }
#endif

    if (fgPngWaitDecFinish())
    {         
  #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB
        #ifdef PNG_READ_FILE
        FILE* fFileHandle = 0;
        #else
        INT32 i4FileHanle;
        #endif
  #else
        UINT32 fFileHandle= 0;
  #endif 
        BYTE* pCurrentFile ;
        INT32 j = 0;
        INT32 i4Num = 0;
        INT32 i = 0;
#if PNG_PERFORMANCE_TEST
        UINT32 u4Sec = 0;
        UINT32 u4Mic = 0;
#endif

#if PNG_PERFORMANCE_TEST
        HAL_GetTime(&_rPtxEnd);
        HAL_GetDeltaTime(&_rTime, &_rPtxStart, &_rPtxEnd);
	    u4Sec = u4Sec +_rTime.u4Seconds;
	    u4Mic = u4Mic +_rTime.u4Micros;
	    UTIL_Printf("[PNG_EMU] Decode Finish Time is %d.%d\n",u4Sec,u4Mic);		
#endif


	 {
		static UINT32 u4PngCheckSum38tmp =0;
		static UINT32 u4PngCheckSum3ctmp =0;
		static UINT32 u4PngCheckSum48tmp =0;
		static UINT32 u4PngCheckSum4ctmp =0;

		//MMU_Dump();

		u4PngCheckSum38tmp =   PngReadREG_vfy(0x6e000,0x138);
		u4PngCheckSum3ctmp =   PngReadREG_vfy(0x6e000,0x13c);
		u4PngCheckSum48tmp =   PngReadREG_vfy(0x6e000,0x148);
		u4PngCheckSum4ctmp =   PngReadREG_vfy(0x6e000,0x14c);
		i4PngPrintf( "[PNG_EMU]check sum value 0x6E138 =  %x \n", u4PngCheckSum38tmp);
		i4PngPrintf( "[PNG_EMU]check sum value 0x6E13c=	%x \n", u4PngCheckSum3ctmp);
		i4PngPrintf( "[PNG_EMU]check sum value 0x6E148=	%x \n", u4PngCheckSum48tmp);
		i4PngPrintf( "[PNG_EMU]check sum value 0x6E14c=  %x \n", u4PngCheckSum4ctmp);


	    if(fgTestCheckSum)
	    {
	    	  static UINT32 u4PngCheckSum38 =0;
		   static UINT32 u4PngCheckSum3c =0;
		   static UINT32 u4PngCheckSum48 =0;
		   static UINT32 u4PngCheckSum4c =0;

	   	PNG_Dump();		 /////////////////////////////for debug
		  if(u4CheckSumRunCnt==0)
		  {
			u4PngCheckSum38 =u4PngCheckSum38tmp;
			u4PngCheckSum3c =u4PngCheckSum3ctmp;
			u4PngCheckSum48 =u4PngCheckSum48tmp;
			u4PngCheckSum4c =u4PngCheckSum4ctmp;	   
			   
		  }
		  else	if(u4CheckSumRunCnt>0)
		  {
			 if( (u4PngCheckSum38 !=u4PngCheckSum38tmp)||
				 (u4PngCheckSum3c !=u4PngCheckSum3ctmp)||
				 (u4PngCheckSum48 !=u4PngCheckSum48tmp)||
				 (u4PngCheckSum4c !=u4PngCheckSum4ctmp))
			   {	  
				  i4PngPrintf( "[PNG_EMU]check sum compare failed,index [%d] cnt[%d]\n",_u4CheckSumIdex,u4CheckSumRunCnt);
				  i4PngPrintf( "[PNG_EMU]check sum value 0x4F38=  %x golden[ %x]\n", u4PngCheckSum38tmp,u4PngCheckSum38);
				  i4PngPrintf( "[PNG_EMU]check sum value 0x4F3c=  %x golden[ %x]\n", u4PngCheckSum3ctmp,u4PngCheckSum3c);
				  i4PngPrintf( "[PNG_EMU]check sum value 0x4F48=  %x golden[ %x]\n", u4PngCheckSum48tmp,u4PngCheckSum48);
				  i4PngPrintf( "[PNG_EMU]check sum value 0x4F4c=  %x golden[ %x]\n", u4PngCheckSum4ctmp,u4PngCheckSum4c);
				 	
			   }   
			   else
			   {
				  i4PngPrintf( "[PNG_EMU]check sum compare success\n");
			   }
		  }
		  
	   }
	    }

//	  PNG_Dump(); 
      //if(!_fgPngDecUseColorBuf)
      u4FileCnt ++;
       //check crc
       //auto skip
      if(fgPngAutoskipEnable)
      {
         if((u4PngHalGetAutoSkipCrc(_u4HwInistId)&0x1) == 1)
         i4PngPrintf( "[PNG_EMU] AUTO SKIP CRC ERROR \n" );
      }
      else		
      {     
       if (u4PngHalReadCrc(_u4HwInistId) != _rPngChunkInfo.u4CRC )
       {
            i4PngPrintf( "[PNG_EMU] CRC ERROR %x\n", u4PngHalReadCrc(_u4HwInistId) );
        }
      }		
        if  (_fgPngDispOut)
        {
            i4PngPrintf( "[PNG_EMU]PNG Decode Finish\n");
            x_thread_delay(100);
           // return FALSE;
        }
	
        pCurrentFile = (BYTE*)_abPngFileList[_u4CurPngFileListNum];
        j = 0;
        for (j=0; j<512; j++)
        {
           _arbPngGoldenFile[j] = pCurrentFile[j];
           _arbPngLargeFile[j] =  pCurrentFile[j];
            _arbPngCheckSumFile[j] = pCurrentFile[j];
        }
        //parsing golden file
        
        for( i = 0; i < 512; i++)
        {
      #if   PNG_READ_FROM_USB
           UINT32 u4GapNum  =7;
	  if(_arbPngGoldenFile[i] == '/')		   /*	\	 */

	  #else
             UINT32 u4GapNum  = 6;
           if(_arbPngGoldenFile[i] == 0x5C)         /*   \    */
		#endif   	
           {
              i4Num++; 
           }
		  // i4PngPrintf("got golden file path i4Num %d \n", i4Num);
           if(i4Num == u4GapNum)
           {
              INT32 j = 0;
              while (_arbPngGoldenPostFix[i4CurrentGoldenType][j] != 0)
              {
              
	      #if DEC_IMAGE_LARGE
			      _arbPngLargeFile[i] = _arbPngGoldenPostFix[i4CurrentGoldenType][j];
          #else
                  _arbPngGoldenFile[i] = _arbPngGoldenPostFix[i4CurrentGoldenType][j];
	      #endif			 
                 i++;
                 j++;
              }
			  
           #if DEC_IMAGE_LARGE
              if ( fgResumeEnable)
              x_memcpy(& _arbPngLargeFile[i - 4],".lg",x_strlen(".lg"));
           else
              x_memcpy(& _arbPngLargeFile[i - 4],".lf",x_strlen(".lf"));
              _arbPngLargeFile[i-1] =0;
              i4PngPrintf("Large file path== %s \n", _arbPngLargeFile);	  ///////debug
           #endif	    
              if((fgResizeEnable||fgResumeEnable ||(u4DstShift >0)) && fgWtEnable)
              {
                 UTIL_Printf("fgResizeEnable[%d],fgResumeEnable[%d],u4DstShift[%d], fgWtEnable[%d]\n",
					 fgResizeEnable,fgResumeEnable,u4DstShift,fgWtEnable);

				 
                 u4ResizeAndCropTimes = 0;
                 _arbPngGoldenFile[i] = u4ResizeAndCropTimesForWt + u4ResizeAndCropTimes + 0x30 + u4DstShift/10;
                 _arbPngGoldenFile[i+1] = u4ResizeAndCropTimesForWt + u4ResizeAndCropTimes+ 0x30+ u4DstShift%10;i++;
                 i++;
				  i4PngPrintf("WT Golden file path== %s \n", _arbPngGoldenFile);    ///////debug
              }
              if(fgLoadData)
              {
                 _arbPngGoldenFile[i] = u4FileCnt + 0x30;
                 i++;
              }
               _arbPngGoldenFile[i]        = 0;
               _arbPngGoldenFile[i + 1]  = 0;
             break;
            }
        }
        //parsing check sum file
   #if 0 //need not parsing check sum file ,removed by bin.liu     
        i4Num = 0;
        i = 0;
        for( i = 0; i < 512; i++)
        {
			if(_arbPngCheckSumFile[i] == 0x5C)
			{
				i4Num++; 
			}
			if(i4Num == 4)
			{
				INT32 j = 0;
				while (_arbPngGoldenPostFix[5][j] != 0)
				{
					_arbPngCheckSumFile[i] = _arbPngGoldenPostFix[5][j];
					i++;
					j++;
				}
				_arbPngCheckSumFile[i]      = 0;
				_arbPngCheckSumFile[i + 1]  = 0;
				 i4PngPrintf("Golden  Check Sum file path== %s\n\n\n\n", _arbPngCheckSumFile);    ///////debug
				break;
            }
        }
#endif 		
        //set auto skip disable
        i4PngHalSetAutoSkipDisable(_u4HwInistId);
        //load result to BMP
        
        if(0)//fgWtEnable) added by bin liu and WT also commpared with golden
        {
           u4ResizeAndCropTimes++;//for skip load golden file, and cal decode file times
          // if( /*(u4ResizeAndCropTimes == 1)||*/(fgResizeEnable)||(fgResumeEnable))
           {
             //if first time decode this file, save decode result to golden memory as compare needed golden file.
             if((!fgResizeEnable)&&(u4ResizeAndCropTimes == 1))
               x_memcpy((void*)_pbPngGoldenData, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
               
             //save decode result to file
         #ifdef PNG_READ_FILE
	fFileHandle = PNG_VFY_FILE_OPEN((char*)_arbPngGoldenFile, "wb");
         #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB	   
             if (fFileHandle == NULL)
          #else 
             if (fFileHandle == 0)
          #endif 
             {
                UTIL_Printf("[PNG_EMU] Write WT result to File Fail \n");
             }
             x_memcpy((void*)_pbPngGoldenData1, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
	      PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
             PNG_VFY_FILE_WRITE((_pbPngGoldenData1), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2,fFileHandle );
             PNG_VFY_FILE_CLOSE(fFileHandle);
	      UTIL_Printf("[PNG_EMU]: write WT result to file <%s>", _arbPngGoldenFile);
       #else
             i4FileHanle = i4RvdFopen((char*)_arbPngGoldenFile, "wb");
             if (i4FileHanle < 0)
             {
                UTIL_Printf("[PNG_EMU] Write WT result to File Fail \n");
             }
             i4RvdFwrite(i4FileHanle, (UINT32)(_pbPngStrmOut), 1100*2000*2 );
             i4RvdFclose(i4FileHanle);
      #endif
           }
           fgPngWTSet(_u4HwInistId, FALSE);
           _u4PngPixelByte = 2; //for WT only
        }
	UTIL_Printf("[PNG_EMU]: open golden <%s> fgload %d\n",
		 _arbPngGoldenFile, fgLoadData);
        if(fgLoadData)
        {
            fFileHandle = PNG_VFY_FILE_OPEN((char*)_arbPngGoldenFile, "wb");
            //x_memcpy((void*)_pbPngGoldenData1, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2);
			//PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
            PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4,fFileHandle);
            PNG_VFY_FILE_CLOSE(fFileHandle);
	        UTIL_Printf("[PNG_EMU]: write Normal decode result to file <%s>", _arbPngGoldenFile);
        }
        else
	{
	     if(1)//wt also compared with golden modifyed by bin.liu (!fgWtEnable)
        {
        
#if DEC_IMAGE_LARGE
	    fFileHandle = PNG_VFY_FILE_OPEN((char*)_arbPngLargeFile, "wb");
        #if !CONFIG_FATS_SUPPORT && !PNG_READ_FROM_USB  
		 if(fFileHandle == NULL)
        #else 
         if (fFileHandle == 0)
        #endif 
  		 {
  			   UTIL_Printf("[PNG_EMU] Write Large Image result to File Fail \n");
  		  }
		  PNG_VFY_FILE_SEEK(fFileHandle,0,SEEK_SET);
          PNG_VFY_FILE_WRITE((_pbPngStrmOut), 1, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width* _u4PngPixelByte,fFileHandle );
                                          
          PNG_VFY_FILE_CLOSE(fFileHandle);
          UTIL_Printf("[PNG_EMU]: write Large Image result to file <%s>", _arbPngLargeFile);
#else
	    INT32 i4RvdReturnVa = 0; 
	    UINT32 u4RealReadSize =0 ;
		if(u4ResizeAndCropTimes == 0)
        {
           i4RvdReturnVa = i4RvdReadFile((char*)_arbPngGoldenFile, (UINT32)_pbPngGoldenData, _u4PngGoldenSize);
           
        }
        if(i4RvdReturnVa >=0 )
        {
            i4PngPrintf( "[PNG_EMU]Read Golden file OK\n");
            i4PngPrintf( "[PNG_EMU]Compare file %s, %s\n", _abPngFileList[_u4CurPngFileListNum], _arbPngGoldenFile);
            i4PngPrintf( "[PNG_EMU]Size %d, %d\n", _rPPngIHDRHeader.u4Height, _rPPngIHDRHeader.u4Width);
	    i4PngPrintf( "[PNG_EMU]Size %d, %d- golde sz 0x%x- %d\n",
	    	rPngHalPicInfo.ptIhdr->u4Height,
	    	rPngHalPicInfo.ptIhdr->u4Width,
	    	rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4,
	    	i4RvdReturnVa);
          /*********                  ***********/
		  if(fgWtEnable)
		  {
		      u4RealReadSize = rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*2;
			  UTIL_Printf("[PNG_EMU]: EEEEEEE=====Compare WT byte size =0x%x\n", u4RealReadSize);
		  }
		  if(fgTransVery)
		  {
		   
		     u4RealReadSize = rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4;
		     fgPngCompareTransition((UINT8*)_pbPngStrmOut, (UINT8*)_pbPngGoldenData, u4RealReadSize);
		  }
                else if (fgPngCompareGolden((UINT8*)_pbPngStrmOut, (UINT8*)_pbPngGoldenData, u4RealReadSize))    
                /**********                  **********/		
       	     {
                   i4PngPrintf( "[PNG_EMU]Compare data correct %s, %s\n", _abPngFileList[_u4CurPngFileListNum], _arbPngGoldenFile);
                   if(fgWtEnable)
                       _u4PngPixelByte = 4;
                   return TRUE;
                   }
                   else
                   {
                     if(fgWtEnable)
                       _u4PngPixelByte = 4;
                  return FALSE;
		              //////////////////////////
            }
        }
        else
        {
            if(fgWtEnable)
               _u4PngPixelByte = 4;
            i4PngPrintf( "[PNG_EMU]Read Golden file %s  fail\n", _arbPngGoldenFile);
        }
#endif
    }
    	}
    }
    return TRUE;
}
BOOL fgPngDecodeSpecialForWT(void)
{
    UINT8 *pu1Fifo = (UINT8*)_pbPngStrmIn;
    UINT32 u4PngFileCount = 0;  //DWRD alignment
    UINT32 u4ChunkSize;
    UINT32 u4ChunkType;
    UINT32 u4NextChunkType;
#if PNG_NEW_PARSING
    //INT32 i = 0;
    //UINT32 *pu4Tmp;
    //if(fgPngAutoskipEnable)
    UINT32 u4Png1stIDATOffset = 0;
    UINT32 u4PngIDATEndOffset = 0;
    //UINT32 u48520ModeIdatDataLen = 0;
//#endif
#endif
			
    _u4PngOutStmCount = 0;
    _u4PngInStmCount = 0;
    _fgPngPalette = FALSE;
    _fgPngTranEn = FALSE;
    _u4PngIDATCount = 0;
    _u4PngSliceCount = 0;
    i4PngHalGetHwInst(&_u4HwInistId);
    i4PngHalEnable(_u4HwInistId);
    i4PngHalReset(_u4HwInistId);
    
      
    u4PngFileCount += 8; //PNG signature
    //read IHDR chunk
    u4PngFileCount += 8; //size + IHDR
    vSetPngIHDR(&pu1Fifo[u4PngFileCount], &_rPPngIHDRHeader);
    rPngHalPicInfo.ptIhdr = &_rPPngIHDRHeader;
    u4PngFileCount += (13 + 4); //IHDR data + CRC

    _fgPngDecUseRingFifo = FALSE;
    _fgPngDecUseColorBuf = FALSE;
    fgPngWTSet(_u4HwInistId, FALSE);
    if(fgWtEnable)//setting WT enable register
    {
       fgPngWTSet(_u4HwInistId, TRUE);
       //_tPngHalHwInst[0].fgWtEnable = TRUE;
       i4CurrentGoldenType = 5;//if need save WT result,set this flage. 
       _u4PngPixelByte = 4;
       _u4PngHorScale = 0;
       _u4PngVrtScale = 0;
       _u4PngIntScale = 7;
    }

#if PNG_MMU_TEST
    vPngMMUInit();
#endif

#if PNG_NEW_PARSING
    if(fgResumeEnable)
    {
    if (_rPPngIHDRHeader.fgInterlace)
    {
        UTIL_Printf("!!!The Bitstream is interlaced!!!\n");
        _fgPngDecUseRingFifo = TRUE;
    }
    else
    {
    	UTIL_Printf("!!!The Bitstream is Not interlaced!!!\n");
        _fgPngDecUseRingFifo = TRUE;
        _fgPngDecUseColorBuf = TRUE;
    }
    }
#endif
    //  
    x_memset((void *)(tPngTrns.u4TrnsData), 0, 256);
    x_memset((void *)(tPngPlte.u4PlteData), 0, 256*3);
    x_memset((void *)_pbPngClrBuf[0], 0, PNG_COLOR_BUF_SIZE);
    x_memset((void *)_pbPngClrBuf[1], 0, PNG_COLOR_BUF_SIZE);

    //init PNG HW register
    vPngInitPicInfor(&rPngHalPicInfo);
   if(!_fgPngDecUseRingFifo)
   {
       vPngPngHalSetRingFifo(_u4HwInistId, rPngHalPicInfo.u4FifoStart, rPngHalPicInfo.u4FifoEnd);
   }
   else
   {
      vPngPngHalSetRingFifo(_u4HwInistId, rPngHalPicInfo.u4FifoStart, rPngHalPicInfo.u4FifoEnd-16);
   }
   i4PngHalSetPicInfo(_u4HwInistId, &rPngHalPicInfo);
    if(fgResizeEnable && (!fgWtEnable))
    {
      if(_rPPngIHDRHeader.fgInterlace && (_u4PngIntScale == 1))
      {      
        i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, 0, 0);        
      }
	  else
      {
        i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, _u4PngHorScale, _u4PngVrtScale);
      }
    }
    //if(fgPngAutoskipEnable)
    {
    i4PngHalSetAutoSkipDisable(_u4HwInistId);
    }
    // 128 bits alignment,
//#if PNG_USE_NEW_CLR_BUF
    if (_fgPngDecUseColorBuf)
    {
        rPngHalIdatInfo.u4DstBufStart = (UINT32)(_pbPngClrBuf[_u4PngSliceCount%2]);
        rPngHalIdatInfo.u4DstBufEnd   =    rPngHalIdatInfo.u4DstBufStart + _rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT;
        _u4PngSliceCount ++;
    }
    else
//#endif
    {
         /*if(fgWtEnable)
               {
               rPngHalIdatInfo.u4DstBufStart = (UINT32)(_pbPngStrmOut + u4DstShift+32+2000*10*4);
               }
               else*/
         {
         rPngHalIdatInfo.u4DstBufStart = (UINT32)(_pbPngStrmOut );
         }
        rPngHalIdatInfo.u4DstBufEnd   =    rPngHalIdatInfo.u4DstBufStart + PNG_STM_OUT_SIZE;
        _u4PreDstBufStartVal = (UINT32)(_pbPngStrmOut);
    }

    

    _rPngChunkInfo.fgLastChunk = FALSE;
    while (1)
    {
        u4ChunkSize = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4;
        u4ChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4;
        _rPngChunkInfo.u4Type = u4ChunkType;
        _rPngChunkInfo.u4DataLength = u4ChunkSize;
        _u4PngImgDataAddr = u4PngFileCount;
        if (u4ChunkType == 0x49444154)        //IDAT
        {
            if ((!_fgPngPalette) && (_fgOurClrPalette))
            {
                i4PngPrintf( "[PNG_EMU]No palette in PNG file\n");
                return FALSE;
            }
#if PNG_NEW_PARSING
	    //auto skip test
            if(fgPngAutoskipEnable)
            {
            if (u4Png1stIDATOffset == 0)
            {
                u4Png1stIDATOffset = _u4PngImgDataAddr;
            }
            u4PngIDATEndOffset = u4PngFileCount + u4ChunkSize + 4;
            }			         
#endif
            u4NextChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount+u4ChunkSize+4+4]); //"current + size + crc" + "length"
            if (u4NextChunkType == 0x49444154)  //IDAT
            {
             if(!fgPngAutoskipEnable)
             {
                if (_u4PngIDATCount == 0)
                {
                    rPngHalIdatInfo.fg1stIDATChunk = TRUE;
                }
                else
                {
                    rPngHalIdatInfo.fg1stIDATChunk = FALSE;
                }
                vPngSetIDATDecReg(_u4HwInistId, _rPngChunkInfo.u4Type, FALSE, &rPngHalIdatInfo);
                _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount + u4ChunkSize ]);
                fgPngWaitDecFinish();
                x_thread_delay(2);
             }
                _u4PngIDATCount++;
            }
            else
            {
                u4PngFileCount += u4ChunkSize; // chunk data
                _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
                u4PngFileCount += 4; // crc
                _rPngChunkInfo.fgLastChunk = TRUE;
                break;
            }
            //break;
        }
        else if (u4ChunkType == 0x504C5445)   //PLTE
        {
            UTIL_Printf("Meet PLTE!\n\n");
            _fgPngPalette = TRUE;
            u4NextChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount+u4ChunkSize+4+4]);
            if (u4NextChunkType == 0x74524E53)  //tRNS
            {
            rPngHalPicInfo.u4AlphaUnMatchedVal = 0xFF;
                //PngWriteREG(PNG_DEC_ALPHA_UNMATCHED_PIXEL_REG, 0xFF);
            }
            //read palette
            vPngHwSetPLTE(_u4HwInistId, _rPngChunkInfo.u4Type, _rPngChunkInfo.u4DataLength, &tPngPlte);//set plte value
        }
        else if (u4ChunkType == 0x74524E53)   //tRNS
        {
            //read trns
            INT32 i4Ret ;
            UTIL_Printf("Meet tRNS!\n\n");
            tPngTrns.u4ChunkType = _rPngChunkInfo.u4Type;
            tPngTrns.u4ChunkSize  = _rPngChunkInfo.u4DataLength;
            //tPngTrns.u4TrnsData = (UINT32)(_pbPngStrmIn + _u4PngImgDataAddr);
            x_memcpy((void *)(tPngTrns.u4TrnsData), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), tPngTrns.u4ChunkSize);
            //due to cache problem, setting TS data in non-cache memory
            i4Ret = i4PngHalSettRNS(_u4HwInistId, &tPngTrns, _fgPngPalette);
            if (i4Ret == PNG_HAL_RTN_FAIL)
            {
                    i4PngPrintf( "[PNG_EMU]read TRNS error\n");
                return FALSE;
             }
            _fgPngTranEn = TRUE;
        }
        else
        {
            //skip this chunk
        }
        u4PngFileCount += u4ChunkSize; // chunk data
        _rPngChunkInfo.u4CRC = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount]);
        u4PngFileCount += 4; // crc
    }

    if ((!_fgPngPalette) && (_fgOurClrPalette))
    {
        i4PngPrintf( "[PNG_EMU]No palette in PNG file\n");
        return FALSE;
    }
    //auto skip test
    if(fgPngAutoskipEnable)
    {
    _u4PngImgDataAddr = u4Png1stIDATOffset - 8;
    _u4PngIDATCount = 0;
    _rPngChunkInfo.u4DataLength = u4PngIDATEndOffset - _u4PngImgDataAddr;
    }


    //start decode
     if (_u4PngIDATCount == 0)
     {
         rPngHalIdatInfo.fg1stIDATChunk = TRUE;
     }
     else
    {
         rPngHalIdatInfo.fg1stIDATChunk = FALSE;
     }
    
    if(fgPngAutoskipEnable)
    {
    i4PngHalSetAutoSkipEnable(_u4HwInistId);
	
    if(_fgPngDecUseRingFifo)
        i4PngHalSetAutoSkipLastIdat(_u4HwInistId, FALSE);
    else
        i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
    }
         //set register
    vPngSetIDATDecReg(_u4HwInistId, _rPngChunkInfo.u4Type, TRUE, &rPngHalIdatInfo);

    //wait decode finish
    if (fgPngWaitDecFinish())
    {     
      #ifdef PNG_READ_FILE
      #else
      INT32 i4FileHanle;
      #endif
#if PNG_PERFORMANCE_TEST
#else
       //check crc
       //auto skip
      if(fgPngAutoskipEnable)
      {
          if((u4PngHalGetAutoSkipCrc(_u4HwInistId)&0x1) == 1)
              i4PngPrintf( "[PNG_EMU] AUTO SKIP CRC ERROR \n" );
      }
      else		
      {     
          if (u4PngHalReadCrc(_u4HwInistId) != _rPngChunkInfo.u4CRC )
          {
              i4PngPrintf( "[PNG_EMU] CRC ERROR %x\n", u4PngHalReadCrc(_u4HwInistId) );
          }
      }		
        //set auto skip disable
      i4PngHalSetAutoSkipDisable(_u4HwInistId);
        
        //compare golden data
      //u4ResizeAndCropTimes++;
      //if(fgWtEnable)
      
      {
          u4ResizeAndCropTimes++;
          {
               //if first time decode this file, save decode result to golden memory as compare needed golden file.
               if((!fgResizeEnable)&&(u4ResizeAndCropTimes == 1))
                   x_memcpy((void*)_pbPngGoldenData, (void*)(_pbPngStrmOut), rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4);
               fgPngWTSet(_u4HwInistId, FALSE);
               _u4PngPixelByte = 4; //for WT only
          }
          if(u4ResizeAndCropTimes > 1)
          {
 #if !PNG_CACHE_MAP_ENABLE 
		 BSP_InvDCacheRange((UINT32)_pbPngStrmOut, PNG_STM_OUT_SIZE);
#endif
   	      if (fgPngCompareGolden((UINT8*)_pbPngStrmOut, (UINT8*)_pbPngGoldenData, rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4))
              {
                  //i4PngPrintf( "[PNG_EMU]Compare data correct %s, %s\n", _abPngFileList[_u4CurPngFileListNum], _arbPngGoldenFile);

                  if(fgWtEnable)
                      _u4PngPixelByte = 4;
                  return TRUE;
              }
              else
              {
                  if(fgWtEnable)
                     _u4PngPixelByte = 4;
                  return FALSE;
              }
          }
       }
       
#endif
    }
    i4PngHalDisable(_u4HwInistId);
    return TRUE;
}
	
// *********************************************************************
// Function : INT32 i4LoadPngFileList(void)
// Description : 
// Parameter : None
// Return    : None
// *********************************************************************
INT32 i4LoadPngFileList(void)
{
  //  return i4RvdReadFile("/mnt/sda1/EMU/pnglist.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
#if !CONFIG_FATS_SUPPORT  
    return i4RvdReadFile("/mnt/usb/EMU/pnglist.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
 #else
  if(fgTransVery)
  	return i4RvdReadFile("D:\\IMG\\PNG\\VfySource\\pnglist_tr.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
  else
    //return i4RvdReadFile("W:\\IMAGE\\PNG\\VfySource\\pnglist.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
    return i4RvdReadFile("D:\\IMG\\PNG\\VfySource\\pnglist.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
#endif
}

// *********************************************************************
// Function : INT32 i4LoadPngFile(void)
// Description : 
// Parameter : None
// Return    : None
// *********************************************************************

INT32 i4LoadPngFile(void)
{

#if  PNG_MMU_TEST
	UINT32 i4eadSize =0;
     i4eadSize = i4RvdReadFile((char *)_abPngFileList[_u4CurPngFileListNum],
     				(UINT32)_pbPngStrmIn1,PNG_STM_IN_SIZE);
     x_memcpy(_pbPngStrmIn,_pbPngStrmIn1,i4eadSize);
	return i4eadSize;
#else
    return i4RvdReadFile((char *)_abPngFileList[_u4CurPngFileListNum],
    			(UINT32)_pbPngStrmIn,PNG_STM_IN_SIZE);
#endif
}



	
// *********************************************************************
// Function : void vParsePngFileList(void)
// Description : 
// Parameter : None
// Return    : None
// *********************************************************************
void vParsePngFileList(UINT32 u4BufSizeLimit)
{
    UINT32 u4I;
    BOOL fgInFileName;

    u4I = 0;
    _u4PngFileListNum = 0;
    fgInFileName = FALSE;
    while( *(_pbPngFileList + u4I) != 0)
    {
        if(!fgInFileName)
        {
            _abPngFileList[_u4PngFileListNum] = (_pbPngFileList + u4I);
            _u4PngFileListNum++;
            fgInFileName = TRUE;
        }

        if(*(_pbPngFileList + u4I) == 0x0D)
            *(_pbPngFileList + u4I) = 0;

        if(*(_pbPngFileList + u4I) == 0x0A)
        {
            *(_pbPngFileList + u4I) = 0;       
            fgInFileName = FALSE;
        }

        u4I++;
        if((u4I == PngFileListMax*256) || (u4I == u4BufSizeLimit))          
              break;
           
    }

	UTIL_Printf("[PNG_verify] PNG total file num %d\n",_u4PngFileListNum);
}

void vResizeAndCrop(void)
{
    //INT32 i4PicMode = 0;
    //resize test 
    UINT8* puSrcData = (UINT8*)_pbPngStrmIn;
    vSetPngIHDR(&puSrcData[16], &_rPPngIHDRHeader);
	u4ResizeAndCropTimesForWt++;
	//cropping test
    u4ResizeAndCropTimes = 0;
	_u4PngHorScale = 0;
    _u4PngVrtScale = 0;
    _u4PngIntScale = 7;
    if( ((_rPPngIHDRHeader.u4Width < 1920)&&(_rPPngIHDRHeader.u4Height < 1080))||(_rPPngIHDRHeader.u4Width > 1920) )
    {                                                     ////
       return;
    }
	if( (_rPPngIHDRHeader.u4Width > 400)&&(_rPPngIHDRHeader.u4Height > 400))
    {
       #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)//For in MT8560, No resize @Youlin Pei
       fgResizeEnable = FALSE;
       if(_rPPngIHDRHeader.fgInterlace)//interlace
       {
        _rPngClipPara.u4ClipHeight =  10*_u4CropNum+3-(10*_u4CropNum+3)%8;                              //160;
       _rPngClipPara.u4ClipWidth =    10*_u4CropNum+4-(10*_u4CropNum+4)%8;                              //160;
       _rPngClipPara.u4ClipTop =       10*_u4CropNum+5-(10*_u4CropNum+5)%8;                               //200;
       _rPngClipPara.u4ClipLeft =       10*_u4CropNum+6-(10*_u4CropNum+6)%8;                          //200;
         _u4PngHorScale = 0;
         _u4PngVrtScale = 0;
         _u4PngIntScale = 7;
       }
       else
       {
       _rPngClipPara.u4ClipHeight =  10*_u4CropNum+3;                              //160;
       _rPngClipPara.u4ClipWidth =    10*_u4CropNum+4;                              //160;
       _rPngClipPara.u4ClipTop =       10*_u4CropNum+5;                               //200;
       _rPngClipPara.u4ClipLeft =       10*_u4CropNum+6;                          //200;
         _u4PngHorScale = 0;
         _u4PngVrtScale = 0;
       }
       #else
       if(_rPPngIHDRHeader.fgInterlace)//interlace
       {
         _u4PngHorScale = 3;
         _u4PngVrtScale = 3;
         _u4PngIntScale = 1;
       }
       else
       {
         _u4PngHorScale = 3;
         _u4PngVrtScale = 3;
       }
       #endif
       _rPngClipPara.fgClipEn = TRUE;
	 #if 0  
       _rPngClipPara.u4ClipHeight =  10*_u4CropNum+3;                              //160;
       _rPngClipPara.u4ClipWidth =    10*_u4CropNum+4;                              //160;
       _rPngClipPara.u4ClipTop =       10*_u4CropNum+5;                               //200;
       _rPngClipPara.u4ClipLeft =       10*_u4CropNum+6;                          //200;
       #endif
       if(fgPngDecode())      
          i4PngPrintf("[Png][Verify]Cropping test,H-%d,W-%d,top-%d,left-%d. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_rPngClipPara.u4ClipHeight ,_rPngClipPara.u4ClipWidth,_rPngClipPara.u4ClipTop,_rPngClipPara.u4ClipLeft,_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
       else
          i4PngPrintf("[Png][Verify]Cropping test,H-%d,W-%d,top-%d,left-%d.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_rPngClipPara.u4ClipHeight ,_rPngClipPara.u4ClipWidth,_rPngClipPara.u4ClipTop,_rPngClipPara.u4ClipLeft,_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
       u4ResizeAndCropTimes++;
       u4ResizeAndCropTimesForWt++;
	   #if 0
       _rPngClipPara.u4ClipHeight = 200;
       _rPngClipPara.u4ClipWidth = 200;
       _rPngClipPara.u4ClipTop = 160;
       _rPngClipPara.u4ClipLeft = 160;
       if(fgPngDecode())      
          i4PngPrintf("[Png][Verify]Cropping test,top-left = 0x160. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
       else
          i4PngPrintf("[Png][Verify]Cropping test,top-left = 0x160.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
       u4ResizeAndCropTimes++;
       u4ResizeAndCropTimesForWt++;
	   #endif
	}
	//else

	#if 0
    {
      fgResizeEnable = TRUE;
      _rPngClipPara.fgClipEn = TRUE;
      _rPngClipPara.u4ClipHeight = _rPPngIHDRHeader.u4Height;
      _rPngClipPara.u4ClipWidth = _rPPngIHDRHeader.u4Width;
      _rPngClipPara.u4ClipTop = 0;
      _rPngClipPara.u4ClipLeft = 0;
    }
     #endif

	 
    /*if(_rPPngIHDRHeader.fgInterlace)//interlace
    {
        INT32 i4Times = 0;
        INT32 i4VerticalResize = 0;
        INT32 i4HoritolResize  = -1;
        INT32 i4Value = 0;
        for(i4Times = 7 ; i4Times > 0; i4Times--)
        {
	   i4Value = i4Times%2;
	   if(i4Value == 0)
           {
	      i4VerticalResize += 1;
           }
	   else
           {
	      i4HoritolResize += 1;
           }
           _u4PngIntScale = i4Times;
	   _u4PngHorScale = i4HoritolResize;
	   _u4PngVrtScale = i4VerticalResize;
          if(fgPngDecode())      
            i4PngPrintf("[Png][Verify]Interlace Resize test. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
          else
            i4PngPrintf("[Png][Verify]Interlace Resize test.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
          u4ResizeAndCropTimes++;
        }
        //i4PicMode++;		
    }
	else
    {
		INT32 i4VerticalResize = 0;
        INT32 i4HoritolResize  = 0;
        for(i4VerticalResize = 0 ; i4VerticalResize< 8; i4VerticalResize++)
	  for(i4HoritolResize = 0 ; i4HoritolResize< 8; i4HoritolResize++)
           {
		_u4PngHorScale = i4HoritolResize;
		_u4PngVrtScale = i4VerticalResize;
             if(fgPngDecode())      
               i4PngPrintf("[Png][Verify]Interlace Resize test. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
             else
               i4PngPrintf("[Png][Verify]Interlace Resize test.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
             u4ResizeAndCropTimes++;
            } 
			// i4PicMode++;
    }*/

	 #if 1
	//set resize val for default value
	_u4PngHorScale = 0;
    _u4PngVrtScale = 0;
    _u4PngIntScale = 7;
    u4ResizeAndCropTimes = 0;
	//set Cropping val for default value
	fgResizeEnable = FALSE;
	_rPngClipPara.fgClipEn = FALSE;
    #endif
}
    
BOOL  vParsingFormat(void)
{
    //set color mode
    UINT32 u4TempVa = 0;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
	_u4PngOutFormatSelect = 0x8;
    for(u4TempVa = 0; u4TempVa < 4; u4TempVa++)
    {
           if (_u4PngOutFormatSelect & 0x1)
           {
                 _eOutClrMd = GFX_COLORMODE_ARGB_CLUT8;
                 _fgOurClrPalette	= TRUE;
                 _u4PngPixelByte = 1;
                 i4CurrentGoldenType = 3;
                 i4PngPrintf("[Png][Verify]Output Color Format GFX_COLORMODE_ARGB_CLUT8\n");	   
           }
           if (_u4PngOutFormatSelect & 0x2)
           {
                 _eOutClrMd = GFX_COLORMODE_ARGB_D1555;
                 _fgOurClrPalette	= FALSE;
                 _u4PngPixelByte = 2;
                 i4CurrentGoldenType = 0;
                 i4PngPrintf("[Png][Verify]Output Color Format GFX_COLORMODE_ARGB_D1555\n");	   
           }
           /*if (_u4PngOutFormatSelect & 0x4)
           {
           _eOutClrMd = GFX_COLORMODE_RGB_D565;
           _fgOurClrPalette	= FALSE;
           _u4PngPixelByte = 2;
           i4CurrentGoldenType = 1;
           }*/
           if (_u4PngOutFormatSelect & 0x4)
           {
                 _eOutClrMd = GFX_COLORMODE_ARGB_D4444;
                 _fgOurClrPalette	= FALSE;
                 _u4PngPixelByte = 2;
                 i4CurrentGoldenType = 1;
                 i4PngPrintf("[Png][Verify]Output Color Format GFX_COLORMODE_ARGB_D4444\n");	  
           }
           if (_u4PngOutFormatSelect & 0x8)
           {
                 _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
                 _fgOurClrPalette	= FALSE;
                 _u4PngPixelByte = 4;
                 i4CurrentGoldenType = 2;
                 i4PngPrintf("[Png][Verify]Output Color Format GFX_COLORMODE_ARGB_D8888\n");
           } 

	 
         if(  fgPngDecode())      
             i4PngPrintf("[Png][Verify]Output Color Format test. Decode %s success! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
	     else if((!_fgPngPalette) && (_fgOurClrPalette))
	     {i4PngPrintf("<png> no palete, skip\n");} //it is ok
	     else 
             i4PngPrintf("[Png][Verify]Output Color Format test. Decode %s fail! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
         _u4PngOutFormatSelect = _u4PngOutFormatSelect >> 1;
    }
    _u4PngOutFormatSelect = 0x8;
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
    _fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
    i4CurrentGoldenType = 2;
    return TRUE;                  ////////////////////////////////////
    #else
    _u4PngOutFormatSelect = 0x10;
    for(u4TempVa = 0; u4TempVa < 5; u4TempVa++)
    {
       if (_u4PngOutFormatSelect & 0x1)
       {
          _eOutClrMd = GFX_COLORMODE_ARGB_CLUT8;
          _fgOurClrPalette	= TRUE;
          _u4PngPixelByte = 1;
          i4CurrentGoldenType = 4;
       }
       if (_u4PngOutFormatSelect & 0x2)
       {
          _eOutClrMd = GFX_COLORMODE_ARGB_D1555;
		  _fgOurClrPalette	= FALSE;
          _u4PngPixelByte = 2;
          i4CurrentGoldenType = 0;
       }
       if (_u4PngOutFormatSelect & 0x4)
       {
          _eOutClrMd = GFX_COLORMODE_RGB_D565;
		  _fgOurClrPalette	= FALSE;
          _u4PngPixelByte = 2;
          i4CurrentGoldenType = 1;
       }
       if (_u4PngOutFormatSelect & 0x8)
       {
          _eOutClrMd = GFX_COLORMODE_ARGB_D4444;
		  _fgOurClrPalette	= FALSE;
          _u4PngPixelByte = 2;
          i4CurrentGoldenType = 2;
       }
       if (_u4PngOutFormatSelect & 0x10)
       {
         _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
		 _fgOurClrPalette	= FALSE;
         _u4PngPixelByte = 4;
         i4CurrentGoldenType = 3;
       } 
       
       if(fgPngDecode())      
         
	   	 i4PngPrintf("[Png][Verify]Output Color Format test. Decode %s success! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
	   	else
         i4PngPrintf("[Png][Verify]Output Color Format test. Decode %s fail! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
       _u4PngOutFormatSelect = _u4PngOutFormatSelect >> 1;
    }
    _u4PngOutFormatSelect = 0x10;
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
	_fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
    i4CurrentGoldenType = 3;
	return TRUE;
    #endif
}

typedef enum _png_mem_type_
{
	PNG_MEM_NORMAL,
	PNG_MEM_BIGMEM_FST, //fist time
	PNG_MEM_BIGMEM_SED, //second time
}PNG_MEM_TYPE;

static UINT32 s_i4BigMemAllSA = 0;
static UINT32 s_i4BigMemCurSA = 0;

static UINT32 _u4PngMemSA = 0;
static UINT32 _u4PngUsedMem = 0;
UINT32 i4PngAllocMem(UINT32 i4Len,UINT32 i4Align,
			BOOL fgPhy, PNG_MEM_TYPE i4TypeEx)
{
	UINT32 i4BufSA = 0;
        UINT32 u4AlignSize = 0;
	if(fgPhy)
	{	            
	        if(_u4PngMemSA == 0)
                {                    
                    _u4PngMemSA = BSP_AllocVfyReserveMemory(0, 160*1024*1024, 256);                    
                    UTIL_Printf("Png Phyiscal _u4BigMemStart =0x%x\n",_u4PngMemSA);                    
                    _u4PngMemSA = VIRTUAL(_u4PngMemSA);
                     UTIL_Printf("Png Virtual _u4BigMemStart =0x%x\n",_u4PngMemSA);   
                }   
                u4AlignSize = ((i4Len + i4Align-1)/i4Align) *i4Align;
                UTIL_Printf("png alloc size=0x%x, alsize= 0x%x\n",i4Len, u4AlignSize);
                
                i4BufSA = _u4PngMemSA + _u4PngUsedMem;
                _u4PngUsedMem += u4AlignSize;
                    UTIL_Printf("png get Virtue addr=0x%x\n", i4BufSA);
                /*
		if(PNG_MEM_NORMAL == i4TypeEx)
		{
			#if CONFIG_SYS_MEM_PHASE2
			i4BufSA = (UINT32)x_alloc_aligned_dma_mem(i4Len, i4Align);
			#elif CONFIG_SYS_MEM_PHASE3
		        i4BufSA   = (UINT32)x_alloc_aligned_ch2_mem(i4Len, i4Align);
			#endif
		}
		#if 0
		else
		{
			i4BufSA = (UINT32)x_mem_alloc(i4Len);
			i4BufSA = ((i4BufSA +i4Align-1)/i4Align)*i4Align;
		}
		#endif
		#if  1//PNG_BIG_MEM_USE
		else if(PNG_MEM_BIGMEM_FST == i4TypeEx && s_i4BigMemAllSA == 0)
		{
			UINT32 i4AllPA = 0;
			//i4AllPA = get_drvmem_mem();
			i4AllPA = BSP_AllocVfyReserveMemory(0, 156*1024*1024, 256);
			s_i4BigMemAllSA = VIRTUAL(i4AllPA);

			UTIL_Printf("<Png> BigMem Init PA %x VA 0x%x\n",
				i4AllPA, s_i4BigMemAllSA);

			i4BufSA = ((s_i4BigMemAllSA +i4Align-1)/i4Align)*i4Align;

			s_i4BigMemCurSA = i4BufSA + i4Len ;
		}
		else if(PNG_MEM_BIGMEM_SED == i4TypeEx && 0 != s_i4BigMemCurSA)
		{
			//may ring, here we don't care
			i4BufSA = ((s_i4BigMemCurSA +i4Align-1)/i4Align)*i4Align;
			s_i4BigMemCurSA = i4BufSA + i4Len ;
			UTIL_Printf("<Png> BigMem Second VA %x NewBase 0x%x\n",
				i4BufSA, s_i4BigMemCurSA);
		}
		else
		{
			UTIL_Printf("<png>alloc error \n");
		}
		#endif
		*/
	}
	else //iommu
	{
		i4BufSA = x_mem_alloc_virtual(i4Len);
		//i4BufSA = x_alloc_vmem(i4Len);
		UTIL_Printf("<png>va- 0x%x -len 0x%x\n", i4BufSA, i4Len);
		x_memset((void*)i4BufSA, 0x5f, i4Len);
		#if PNG_MMU_TEST
		PNG_MMU_SYNC_TABLE(i4BufSA, i4Len);
		//x_thread_delay(200);
		#endif
	}

	if(!i4BufSA)
	{
		UTIL_Printf("<png>alloc mem error Len 0x%x fgPa 0x%x Type 0x%x \n",
				i4Len, fgPhy, i4TypeEx);
	}
	return i4BufSA;
}

UINT32 i4PngFreeMem(UINT32 i4Buffer, BOOL fgPhy, PNG_MEM_TYPE i4TypeEx)
{
	if(fgPhy)
    {
		if(PNG_MEM_NORMAL == i4TypeEx)
		{
			#if CONFIG_SYS_MEM_PHASE2
			x_free_aligned_dma_mem(i4Buffer);
			#elif CONFIG_SYS_MEM_PHASE3
			//x_free_aligned_nc_mem(i4Buffer);
			x_free_aligned_dma_mem(i4Buffer);
			#endif
		}
        else if(PNG_MEM_BIGMEM_FST == i4TypeEx)
        {
            BSP_FreeVfyReserveMemory(_u4PngMemSA);
        }
	}
	else //iommu
	{
		x_mem_free((void*)i4Buffer);
	}
}
// *********************************************************************
// Function : void vPngVerify(const CHAR** aszArgv)
// Description : 
// Parameter : None
// Return    : None
// *********************************************************************
//[CJYANG] imgdec_vfy, need to initial memory allocate
UINT32 _u4StartClock = 5;
UINT32 _u4EndClock =5;
extern UINT32  PNG_CLOCK_SETTING;
extern void vHwImgReszResetDramSetting(UINT32 u4HwId);
extern int mlock_kernel(unsigned long start, size_t len);
void vPngVerify(INT32 i4Argc, const CHAR** aszArgv)
{
    UINT32 u4PngWbHdrMem;
    UINT32 u4PngGoldenData;
    UINT32 u4PngPngRingFifo;
    UINT32 u4PngClrBuf[2];
    #if PNG_MMU_TEST
    UINT32 u4PngClrBufApp[2];
    UINT32 u4PngPngRingFifoApp;
    #endif
    UINT32 u4TempValue = 0;
    BOOL fgIOMMUDisable = FALSE;
	UINT32 u4CheckSumCnt=0;
	fgTest1M4Kswitch = FALSE;
    fgMemCrossChannel  = FALSE;
    fgPngAutoskipEnable = TRUE;
    fgWtEnable = FALSE;            /// note TV not support WT
   // fgTransVery = FALSE;
    fgTestU11U18 = FALSE;
    fgTestCheckSum = FALSE;
	//fgResumeEnable = TRUE;

   vHwImgReszResetDramSetting(0);
   UTIL_Printf(" vPngVerify \n");
   _u4CheckSumIdex =0;
#if (!PNG_MMU_TEST)   //////start from here
   fgIOMMUDisable = TRUE;

   UTIL_Printf("NONE--MMMUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU.\n");	   
   u4PngClrBuf[0]   = (UINT32)i4PngAllocMem(PNG_COLOR_BUF_SIZE, 256, 1, PNG_MEM_NORMAL);
   u4PngClrBuf[1]   = (UINT32)i4PngAllocMem(PNG_COLOR_BUF_SIZE, 256, 1, PNG_MEM_NORMAL);
   u4PngPngRingFifo = (UINT32)i4PngAllocMem(PNG_RING_FIFO_SIZE, 256, 1, PNG_MEM_NORMAL);
   tPngTrns.u4TrnsData = (UINT32)i4PngAllocMem(256*3, 128, 1, PNG_MEM_NORMAL);
   tPngPlte.u4PlteData = (UINT32)i4PngAllocMem(256*3, 128, 1, PNG_MEM_NORMAL);

#else //PNG_MMU_TEST
     UTIL_Printf("MMMMMUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU \n");	
   fgIOMMUDisable = FALSE;
  #if PNG_SUPPORT_CROSS_MEM	 
     if(!fgWtEnable)
  #endif	 	
     {
	     UTIL_Printf("allocate iommu u4PngClrBuf[0][0x%x]\n",PNG_COLOR_BUF_SIZE);
	     //u4PngClrBuf[0]   = i4PngAllocMem(PNG_COLOR_BUF_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
	     u4PngClrBufApp[0] = i4PngAllocMem(PNG_COLOR_BUF_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
             u4PngClrBuf[0] = (u4PngClrBufApp[0] + 255) &(~0xff);
	     UTIL_Printf("allocate u4PngClrBuf[1][0x%x]\n",PNG_COLOR_BUF_SIZE);
	     //u4PngClrBuf[1]   = i4PngAllocMem(PNG_COLOR_BUF_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);	     
	     u4PngClrBufApp[1]   = i4PngAllocMem(PNG_COLOR_BUF_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
             u4PngClrBuf[1] = (u4PngClrBufApp[1]  + 255)&(~0xff);
	     UTIL_Printf("allocate u4PngPngRingFifo[0x%x]\n",PNG_RING_FIFO_SIZE);
	     //u4PngPngRingFifo = i4PngAllocMem(PNG_RING_FIFO_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
	     u4PngPngRingFifoApp = i4PngAllocMem(PNG_RING_FIFO_SIZE, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
             u4PngPngRingFifo = (u4PngPngRingFifoApp + 255)&(~0xff);
	     tPngTrns.u4TrnsDataApp  = i4PngAllocMem(256*3, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
             tPngTrns.u4TrnsData = (tPngTrns.u4TrnsDataApp + 127) & (~0x7f);
	     UTIL_Printf("allocate tPngPlte.u4PlteData[0x%x]\n",256*3);
	     tPngPlte.u4PlteDataApp = i4PngAllocMem(256*3, 0, fgIOMMUDisable, PNG_MEM_NORMAL);
             tPngPlte.u4PlteData = (tPngPlte.u4PlteDataApp + 127) & (~0x7f);
     }
#if PNG_SUPPORT_CROSS_MEM	 
	  else
	  {
         u4PngClrBuf[0]   = (UINT32)x_alloc_aligned_ch1_mem(PNG_COLOR_BUF_SIZE, 256);
         u4PngClrBuf[1]   = (UINT32)x_alloc_aligned_ch1_mem(PNG_COLOR_BUF_SIZE, 256);
         u4PngPngRingFifo = (UINT32)x_alloc_aligned_ch1_mem(PNG_RING_FIFO_SIZE, 256);
         tPngTrns.u4TrnsData = (UINT32)x_alloc_aligned_ch1_mem(256*3, 128);
         tPngPlte.u4PlteData = (UINT32)x_alloc_aligned_ch1_mem(256*3, 128);
	  }
#endif 	  
#endif  //PNG_MMU_TEST

#if (!PNG_MMU_TEST)   //////start from here
         _pbPngStrmIn = i4PngAllocMem(PNG_STM_IN_SIZE, 256,
         				fgIOMMUDisable, PNG_MEM_BIGMEM_FST);
	 _pbPngStrmOut = i4PngAllocMem(PNG_STM_OUT_SIZE, 256,
	 				fgIOMMUDisable, PNG_MEM_BIGMEM_SED);
		 UTIL_Printf("allocate u4PngStrmOut[0x%x], addr[0x%x]\n",PNG_STM_OUT_SIZE,_pbPngStrmOut);  
		 ASSERT(0!=_pbPngStrmOut);
#else      
         _pbPngStrmIn1 = i4PngAllocMem(PNG_STM_IN_SIZE, 256,
         				1, PNG_MEM_BIGMEM_FST);
        UTIL_Printf("mmu alloc _pbPngStrmIn1:0x%x\n", _pbPngStrmIn1);
#if PNG_SUPPORT_CROSS_MEM	
         if(!fgMemCrossChannel)
#endif		 	
	     {
    #if PNG_SUPPORT_CROSS_MEM	  
	       if(!fgWtEnable)
	#endif	   	
		   {                  
              
			  if(fgTest1M4Kswitch)
			  {
			     //extern void *IOMMU_get_fragment_vmem(UINT32 u4Size);
				 UTIL_Printf("AAAAA no=======cross dram 1k-4m channel \n");
                 /*
                 _pbPngStrmOut    = (UINT8 *)
		 		i4PngAllocMem(PNG_STM_OUT_SIZE, 256, fgIOMMUDisable, PNG_MEM_NORMAL);
		 		*/
		 		_pbPngStrmOutApp = (UINT8 *)i4PngAllocMem(PNG_STM_OUT_SIZE, 256, fgIOMMUDisable, PNG_MEM_NORMAL);
                                _pbPngStrmOut = (UINT8 *)(((UINT32)_pbPngStrmOutApp + 255) & (~0xff));		 		
			  }
			  else
			  {
			       UTIL_Printf("AAAAA no=======cross dram channel \n");
                    // _pbPngStrmOut    = (UINT8 *)i4PngAllocMem(PNG_STM_OUT_SIZE, 256,
		     	//			fgIOMMUDisable, PNG_MEM_NORMAL);
                     		_pbPngStrmOutApp = (UINT8 *)i4PngAllocMem(PNG_STM_OUT_SIZE, 256, fgIOMMUDisable, PNG_MEM_NORMAL);
                                _pbPngStrmOut = (UINT8 *)(((UINT32)_pbPngStrmOutApp + 255) & (~0xff));	
			  }
              ASSERT(0!=_pbPngStrmOut);
              UTIL_Printf("allocate u4PngStrmOut[0x%x], addr[0x%x]\n",PNG_STM_OUT_SIZE,_pbPngStrmOut);
              //_pbPngStrmIn     = (UINT8 *)i4PngAllocMem(PNG_STM_IN_SIZE, 256,
	      	//				fgIOMMUDisable, PNG_MEM_NORMAL);
	      _pbPngStrmInApp = (UINT8 *)i4PngAllocMem(PNG_STM_IN_SIZE, 256, fgIOMMUDisable, PNG_MEM_NORMAL);
              _pbPngStrmIn = (UINT8 *)(((UINT32)_pbPngStrmInApp + 255) & (~0xff));
              UTIL_Printf("allocate _pbPngStrmIn[0x%x] addr[0x%x]\n",PNG_STM_IN_SIZE,_pbPngStrmIn);
              ASSERT(0!=_pbPngStrmIn);
		   }
		 #if PNG_SUPPORT_CROSS_MEM	    
		   else
		   {
               _pbPngStrmIn     = (UINT8 *)x_alloc_aligned_ch2_mem(PNG_STM_IN_SIZE, 256);
               ASSERT(0!=_pbPngStrmIn);
               _pbPngStrmOut    = (UINT8 *)x_alloc_aligned_ch2_mem(PNG_STM_OUT_SIZE, 256);
		   }
		  #endif 
              
	     }
	#if  PNG_SUPPORT_CROSS_MEM    
	     else
	     {
           UTIL_Printf("AAAAA cross===== dram channel \n");
           _pbPngStrmOut    = (UINT8 *)x_alloc_aligned_ch1_mem(PNG_STM_OUT_SIZE,256);	
           ASSERT(0!=_pbPngStrmOut);
           UTIL_Printf("allocate u4PngStrmOut[0x%x], addr[0x%x]\n",PNG_STM_OUT_SIZE,_pbPngStrmOut);          
           _pbPngStrmIn     = (UINT8 *)x_alloc_aligned_ch2_mem(PNG_STM_IN_SIZE,256);	//channel
           UTIL_Printf("allocate u4PngStrmIn[0x%x] addr[0x%x]\n",PNG_STM_IN_SIZE,_pbPngStrmIn);
		   ASSERT(0!=_pbPngStrmIn);
	     }
	 #endif    
#endif

#if CONFIG_SYS_MEM_PHASE2
   _pu1PngPlt = (UINT8 *)x_alloc_aligned_dma_mem(256*32, 16);	
   u4PngWbHdrMem   = (UINT32)x_alloc_aligned_dma_mem(32, 256); 
 	#if 1//PNG_BIG_MEM_USE
   //u4PngGoldenData = _pbPngStrmOut+  PNG_STM_OUT_SIZE;//(UINT32)x_alloc_aligned_dma_mem(PNG_GOLDEN_DATA, 256);
   u4PngGoldenData = i4PngAllocMem(PNG_GOLDEN_DATA, 256,
   				fgIOMMUDisable, PNG_MEM_BIGMEM_SED);
   #else 
     u4PngGoldenData = (UINT32)x_alloc_aligned_dma_mem(PNG_GOLDEN_DATA, 256);
   #endif
   _pbPngFileList = (UINT8 *)x_alloc_aligned_dma_mem(PngFileListMax*512, 16);
   _u4RegBuf = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
   au4RegBBuf[0] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
   au4RegBBuf[1] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
   au4RegABuf[0] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
   au4RegABuf[1] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
#elif CONFIG_SYS_MEM_PHASE3
            _pu1PngPlt = (UINT8 *)x_alloc_aligned_ch2_mem(256*32, 16);	
            u4PngWbHdrMem   = (UINT32)x_alloc_aligned_ch2_mem(32, 256); 
            u4PngGoldenData = (UINT32)x_alloc_aligned_ch2_mem(PNG_GOLDEN_DATA, 256);
            _pbPngFileList = (UINT8 *)x_alloc_aligned_ch2_mem(PngFileListMax*512, 16);
            _u4RegBuf = (UINT32)x_alloc_aligned_ch2_mem(32*4*4, 128);
            au4RegBBuf[0] = (UINT32)x_alloc_aligned_ch2_mem(32*4*4, 128);
            au4RegBBuf[1] = (UINT32)x_alloc_aligned_ch2_mem(32*4*4, 128);
            au4RegABuf[0] = (UINT32)x_alloc_aligned_ch2_mem(32*4*4, 128);
            au4RegABuf[1] = (UINT32)x_alloc_aligned_ch2_mem(32*4*4, 128);
#endif

   _pu4PngWbHdrMem     = (UINT8*)u4PngWbHdrMem;
   _pbPngGoldenData    = (UINT8*)u4PngGoldenData;
   _pbPngClrBuf[0]     = (UINT8*)(u4PngClrBuf[0]);
   _pbPngClrBuf[1]     = (UINT8*)(u4PngClrBuf[1]);
   _pbPngRingFifo      = (UINT8*)u4PngPngRingFifo;

#if PNG_DST_NOTALIGN_TEST_EN
     _pbPngStrmOutNew = _pbPngStrmOut;
#endif

#if PNG_CLIP_TEST_EN
   _rPngClipPara.fgClipEn = TRUE;// FALSE;
#else
   _rPngClipPara.fgClipEn =  FALSE;
#endif
   _rPngClipPara.u4ClipHeight = 200;
   _rPngClipPara.u4ClipWidth = 200;
   _rPngClipPara.u4ClipTop = 800;
   _rPngClipPara.u4ClipLeft = 800;
     //load png file 
    {
         UINT32 u4I;
         for(u4I=0; u4I<PngFileListMax*512; u4I++)
              _pbPngFileList[u4I] = 0;
         
         for(u4I=0; u4I<PngFileListMax; u4I++)
            _abPngFileList[u4I] = 0;
    }

    {
        INT32 i4FileSize;
        i4FileSize = i4LoadPngFileList();
        vParsePngFileList(i4FileSize);
    }

    i4RvdPrintf("\n");
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
    _fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
	#if  (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    i4CurrentGoldenType = 2;
	#else
	i4CurrentGoldenType = 3;
	#endif 
#if PNG_MMU_TEST
     //HalFlushDCache();
     //__asm__ volatile("isb\n");
   //  x_thread_delay(2000);
#endif
	
        u4TempValue = 0;        
        UTIL_Printf("u4PngClrBuf[0] = %x!\n",u4PngClrBuf[0]);
        UTIL_Printf("u4PngClrBuf[1] = %x!\n",u4PngClrBuf[1]);
        UTIL_Printf("u4PngPngRingFifo = %x!\n",u4PngPngRingFifo);
        UTIL_Printf("tPngTrns.u4TrnsData = %x!\n",tPngTrns.u4TrnsData);
        UTIL_Printf("tPngPlte.u4PlteData = %x!\n",tPngPlte.u4PlteData);
        if(fgTestCheckSum)
        {
           u4CheckSumCnt =3;
        }
	    else
	    {
	       u4CheckSumCnt =0;
	    }
		for(PNG_CLOCK_SETTING=_u4StartClock; PNG_CLOCK_SETTING<=_u4EndClock;PNG_CLOCK_SETTING++)
		{
		    i4PngHalInitHwInst();	
		    _u4CheckSumIdex = u4CheckSumCnt;
             for(;_u4CheckSumIdex>=0; _u4CheckSumIdex--)	 	
	        {       
	             i4PngPrintf( "[PNG_EMU] \n !!!!!!![%d] !!!!!!!!  \n",_u4CheckSumIdex);
	             i4PngPrintf( "[PNG_EMU]Begin png auto skip mode[%d] decoding  \n \n",fgPngAutoskipEnable);		  
	              for(_u4CurPngFileListNum=0; _u4CurPngFileListNum<_u4PngFileListNum; _u4CurPngFileListNum++)
			     {         
			           INT32 _i4PngFileSize ;		
			           BOOL fgDecodeSuccess = FALSE;
			           BOOL fgTestNormalMode =TRUE;     ////// FALSE;
		           #if PNG_CLIP_TEST_EN
			   fgTestNormalMode = FALSE;
			   #endif
			           u4FileCnt = 0;
					   UTIL_Printf("  --------- - ------------------------  \n"); 
				       _i4PngFileSize = i4LoadPngFile();
			             
#if !PNG_CACHE_MAP_ENABLE
			           UTIL_Printf("Flush u4PngStrmIn[0x%x] addr[0x%x]\n",PNG_STM_IN_SIZE,_pbPngStrmIn);
				  	   BSP_FlushDCacheRange((UINT32) _pbPngStrmIn, PNG_STM_IN_SIZE);
#endif 
					   UTIL_Printf("[Png][Verify] Load File %s\n",_abPngFileList[_u4CurPngFileListNum], _i4PngFileSize);
			           if(fgWtEnable)
			           {   
			                 u4ResizeAndCropTimes = 0;//for skip load golden file, and cal decode one file times
			                u4ResizeAndCropTimesForWt = 0;
			           }
			           if(_i4PngFileSize <= 0)
			           {
			                UTIL_Printf("[Png][Verify] Load File fail\n");
			           }
			           else			  
			           {
				#if PNG_DST_NOTALIGN_TEST_EN
				 //for test a png
				 unsigned int rand = 1;
				while(rand < 0x10)
				#else
		                if(fgTestNormalMode)
				#endif
		                {
		                  INT32 u4CheckSumLoop=1;
				  #if PNG_DST_NOTALIGN_TEST_EN
				  _pbPngStrmOut = _pbPngStrmOutNew+(rand++);
				  UTIL_Printf("<Png>Adjust Cur rand %d , base 0x%x, NewBase 0x%x\n",
				  	rand, _pbPngStrmOutNew, _pbPngStrmOut);
				  #endif
							  
			                  //normal mode test, no resume & resize
			                 // for(u4DstShift = 0; u4DstShift < 16; u4DstShift += 4)
			                 if(fgTestCheckSum)
			                 {
			                        UTIL_Printf("\n[PNG_EMU]check sum---]\n");
					          u4CheckSumLoop	 =4;
			                 }

				              for( u4CheckSumRunCnt=0; u4CheckSumRunCnt< u4CheckSumLoop; u4CheckSumRunCnt++)
			                 {
			                    fgResumeEnable = FALSE;                     
			                    u4ResizeAndCropTimes = 0;
				    i4PngPrintf("[Png][Verify]Output Color Format GFX_COLORMODE_ARGB_D8888 FUll\n");
			                    i4PngPrintf("[Png][Verify] Decode Png by HW - normal mode\n");
			                    fgDecodeSuccess = fgPngDecode();	
			                    if( fgDecodeSuccess)      
			                         i4PngPrintf("[Png][Verify]Normal mode Decode %s success,auto-skip mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
			                   	else
			                        i4PngPrintf("[Png][Verify]Normal mode Decode %s fail,auto-skip mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
			                 }
			                    //resume mode test, no crop & resize
				#if PNG_VFY_NORMAL_ALL
			                   					
			                 fgResumeEnable = TRUE;    //////////////////////////////
			                   

		                 if(!fgWtEnable && !fgTransVery  &&!fgTestCheckSum)       //log wt dosen't need this case.
		                 {
		                      i4PngPrintf("[Png][Verify] Decode Png by HW - SRC&DST resume mode\n");
		                      fgDecodeSuccess = fgPngDecode();
		                      if(fgDecodeSuccess)
		                          i4PngPrintf("[Png][Verify]SRC&DST resume mode Decode %s success,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
		                      else
		                         i4PngPrintf("[Png][Verify]SRC&DST resume mode Decode %s fail,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);

		                      //resuem and output color format test
		                      i4PngPrintf("[Png][Verify] Decode Png by HW - resume and output color format test\n");/////////
		                      vParsingFormat();  ////////////////////////////////
		                 }
				 #endif
					
			                }
				#if PNG_CLIP_TEST_EN
		                if(!fgWtEnable&&!fgTransVery &&!fgTestCheckSum)
			        #else
				if(0)
				#endif
			                {
			                      //crop and scale mode test, no resume & resize                    
								for(_u4CropNum =10; _u4CropNum<20; _u4CropNum++ )	  
								{
						        		i4PngPrintf("[Png][Verify] Decode Png by HW - Resize&Scale mode\n");
			                             vResizeAndCrop();
								}
			                }
			                fgResumeEnable = FALSE;
			              }
			           }
	          // fgPngAutoskipEnable = FALSE;
	         }			 
			 i4PngHalUnInitHwInst();
	    }
#if PNG_DST_NOTALIGN_TEST_EN
	 _pbPngStrmOut = _pbPngStrmOutNew;
#endif
    //free memory
      if(!fgMemCrossChannel)
      {    	
#if PNG_MMU_TEST  && PNG_SUPPORT_CROSS_MEM
	     if(fgWtEnable)
	     {
	          i4PngPrintf("[Png][Verify] Free  u4PngStrmIn=0x%x \n", _pbPngStrmIn);
	          x_free_aligned_ch1_ch2_mem((void*)_pbPngStrmIn);_pbPngStrmIn=0;
	          i4PngPrintf("[Png][Verify] Free  u4PngStrmOut=0x%x \n", _pbPngStrmOut);
	          x_free_aligned_ch1_ch2_mem((void*)_pbPngStrmOut);_pbPngStrmOut = 0; 
	     }
		 else
#endif 
	      {
	           i4PngPrintf("[Png][Verify] Free  u4PngStrmIn=0x%x \n", _pbPngStrmIn);
		   i4PngPrintf("[Png][Verify] Free  _pbPngStrmOut=0x%x \n", _pbPngStrmOut);
	           /*PNG_VFY_MEM_FREE((void*)_pbPngStrmIn);	_pbPngStrmIn=0;
	            #if !PNG_BIG_MEM_USE
	            i4PngPrintf("[Png][Verify] Free  u4PngStrmOut=0x%x \n", _pbPngStrmOut);
	            PNG_VFY_MEM_FREE((void*)_pbPngStrmOut); _pbPngStrmOut = 0;
	            #endif
	           */
		   i4PngFreeMem(_pbPngStrmIn, fgIOMMUDisable, PNG_MEM_BIGMEM_FST);
		   i4PngFreeMem(_pbPngStrmOut, fgIOMMUDisable, PNG_MEM_BIGMEM_SED);
		  }
      }
      else
      {
      #if  PNG_SUPPORT_CROSS_MEM   
            i4PngPrintf("[Png][Verify] Free  u4PngStrmIn=0x%x \n", _pbPngStrmIn);
            x_free_aligned_ch1_ch2_mem((void*)_pbPngStrmIn);_pbPngStrmIn=0;
            i4PngPrintf("[Png][Verify] Free  u4PngStrmOut=0x%x \n", _pbPngStrmOut);
            x_free_aligned_ch1_ch2_mem((void*)_pbPngStrmOut);_pbPngStrmOut = 0; 
      #endif      
      }
#if PNG_MMU_TEST && PNG_SUPPORT_CROSS_MEM   
        if(fgWtEnable)
        {
        	
            i4PngPrintf("[Png][Verify] Free  4PngClrBuf[0]=0x%x \n", u4PngClrBuf[0]);		  
            x_free_aligned_ch1_ch2_mem((void*)u4PngClrBuf[0]);u4PngClrBuf[0] =0;
            i4PngPrintf("[Png][Verify] Free  4PngClrBuf[1]=0x%x \n", u4PngClrBuf[1]);	
            x_free_aligned_ch1_ch2_mem((void*)u4PngClrBuf[1]);u4PngClrBuf[1] =0;
            i4PngPrintf("[Png][Verify] Free  _pbPngRingFifo=0x%x \n", _pbPngRingFifo);	
            x_free_aligned_ch1_ch2_mem((void*)_pbPngRingFifo);_pbPngRingFifo=0;
           i4PngPrintf("[Png][Verify] Free  u4TrnsData=0x%x \n", tPngTrns.u4TrnsData);	
            x_free_aligned_ch1_ch2_mem((void*)tPngTrns.u4TrnsData);	tPngTrns.u4TrnsData=0;
            i4PngPrintf("[Png][Verify] Free  u4PlteData=0x%x \n", tPngPlte.u4PlteData);                      
            x_free_aligned_ch1_ch2_mem((void*)tPngPlte.u4PlteData);tPngPlte.u4PlteData=0;
            }
	     else
#endif 	  
        {
            i4PngPrintf("[Png][Verify]11 Free  4PngClrBuf[0]=0x%x \n", u4PngClrBuf[0]);
            i4PngFreeMem((void*)u4PngClrBuf[0], fgIOMMUDisable, PNG_MEM_NORMAL);
	    u4PngClrBuf[0] =0;
            i4PngPrintf("[Png][Verify] Free  4PngClrBuf[1]=0x%x \n", u4PngClrBuf[1]);
            i4PngFreeMem((void*)u4PngClrBuf[1], fgIOMMUDisable, PNG_MEM_NORMAL);
	    u4PngClrBuf[1] =0;
            i4PngPrintf("[Png][Verify] Free  _pbPngRingFifo=0x%x \n", _pbPngRingFifo);
            i4PngFreeMem((void*)_pbPngRingFifo, fgIOMMUDisable, PNG_MEM_NORMAL);
	    _pbPngRingFifo=0;
           i4PngPrintf("[Png][Verify] Free  u4TrnsData=0x%x \n", tPngTrns.u4TrnsData);
            i4PngFreeMem((void*)tPngTrns.u4TrnsData, fgIOMMUDisable, PNG_MEM_NORMAL);
	    tPngTrns.u4TrnsData=0;
            i4PngPrintf("[Png][Verify] Free  u4PlteData=0x%x \n", tPngPlte.u4PlteData);
            i4PngFreeMem((void*)tPngPlte.u4PlteData, fgIOMMUDisable, PNG_MEM_NORMAL);
	    tPngPlte.u4PlteData=0;
       }
      i4PngPrintf("[Png][Verify] Free  _pu1PngPlt=0x%x \n", _pu1PngPlt);	
      x_free_aligned_dma_mem((void*)_pu1PngPlt); _pu1PngPlt=0;
      i4PngPrintf("[Png][Verify] Free  u4PngWbHdrMem=0x%x \n", u4PngWbHdrMem);
      x_free_aligned_dma_mem((void*)u4PngWbHdrMem);
#if !PNG_BIG_MEM_USE
      i4PngPrintf("[Png][Verify] Free  u4PngGoldenData=0x%x \n", u4PngGoldenData);
      x_free_aligned_dma_mem((void*)u4PngGoldenData);
#endif */
      i4PngFreeMem(u4PngGoldenData, fgIOMMUDisable, 2);
      i4PngPrintf("[Png][Verify] Free  _pbPngFileList=0x%x \n", _pbPngFileList);	                      
      x_free_aligned_dma_mem((void*)_pbPngFileList);_pbPngFileList =0;
      i4PngPrintf("[Png][Verify] Free  _u4RegBuf=0x%x \n", _pbPngFileList);	
	 x_free_aligned_dma_mem((void*)_u4RegBuf );_u4RegBuf =0;
      i4PngPrintf("[Png][Verify] Free  au4RegBBuf[0] =0x%x \n", au4RegBBuf[0] );	
      x_free_aligned_dma_mem((void*)au4RegBBuf[0] );au4RegBBuf[0]=0;
      i4PngPrintf("[Png][Verify] Free  au4RegBBuf[1] =0x%x \n", au4RegBBuf[0] );	      
       x_free_aligned_dma_mem((void*)au4RegBBuf[1] );au4RegBBuf[1]=0;
       i4PngPrintf("[Png][Verify] Free  au4RegBBuf[1] =0x%x \n", au4RegABuf[0] );	    
      x_free_aligned_dma_mem((void*)au4RegABuf[0] );au4RegABuf[0] =0;
      i4PngPrintf("[Png][Verify] Free  au4RegBBuf[1] =0x%x \n", au4RegABuf[1] );	    
      x_free_aligned_dma_mem((void*) au4RegABuf[1] );au4RegABuf[1]  =0;
     #if PNG_MMU_TEST
      // x_free_aligned_dma_mem((void*)_pbPngStrmIn1);_pbPngStrmIn1=0;
      i4PngPrintf("[Png][Verify] Free  _pbPngStrmIn1[1] =0x%x \n", _pbPngStrmIn1 );
       i4PngFreeMem((UINT32)_pbPngStrmIn1, 1/*fgIOMMUDisable*/, PNG_MEM_BIGMEM_FST);
       i4PngFreeMem(_pbPngStrmOutApp , 0, PNG_MEM_NORMAL);
       i4PngFreeMem(_pbPngStrmInApp, 0, PNG_MEM_NORMAL);
#endif    

}

void vPngWTMMUTest(void* aszArgv)
{
    UINT32 u4PngStrmIn;
    #if PNG_MMU_TEST
    UINT32 u4PngStrmIn1;
    #endif
    UINT32 u4PngStrmOut;
    UINT32 u4PngWbHdrMem;
    UINT32 u4PngGoldenData;
    UINT32 u4PngPngRingFifo;
    UINT32 u4PngClrBuf[2];
    UINT32 u4TempValue = 0;

    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
      #ifdef PNG_ALLOCATE_CH2
      u4PngStrmIn     = (UINT32)x_alloc_aligned_ch2_mem(PNG_STM_IN_SIZE, 256);
      u4PngWbHdrMem   = (UINT32)x_alloc_aligned_ch2_mem(32, 256);
      u4PngClrBuf[0]   = (UINT32)x_alloc_aligned_ch2_mem(PNG_COLOR_BUF_SIZE, 256);
      u4PngClrBuf[1]   = (UINT32)x_alloc_aligned_ch2_mem(PNG_COLOR_BUF_SIZE, 256);
      u4PngPngRingFifo = (UINT32)x_alloc_aligned_ch2_mem(PNG_RING_FIFO_SIZE, 256);
      _pu1PngPlt = (UINT8 *)x_alloc_aligned_ch2_mem(256*32, 16);	
      u4PngStrmOut    = (UINT32)x_alloc_aligned_ch2_mem(PNG_STM_OUT_SIZE, 256);
      u4PngGoldenData = (UINT32)x_alloc_aligned_ch2_mem(PNG_GOLDEN_DATA, 256);
      tPngTrns.u4TrnsData = (UINT32)x_alloc_aligned_ch2_mem(256*3, 128);	   
      tPngPlte.u4PlteData = (UINT32)x_alloc_aligned_ch2_mem(256*3, 128);
      #else
      #if (!PNG_MMU_TEST)
	  u4PngStrmIn     = (UINT32)x_alloc_aligned_dma_mem(PNG_STM_IN_SIZE, 256);
      u4PngStrmOut    = (UINT32)x_alloc_aligned_dma_mem(PNG_STM_OUT_SIZE, 256);
      u4PngClrBuf[0]   = (UINT32)x_alloc_aligned_dma_mem(PNG_COLOR_BUF_SIZE, 256);
      u4PngClrBuf[1]   = (UINT32)x_alloc_aligned_dma_mem(PNG_COLOR_BUF_SIZE, 256);
      u4PngPngRingFifo = (UINT32)x_alloc_aligned_dma_mem(PNG_RING_FIFO_SIZE, 256);
      tPngTrns.u4TrnsData = (UINT32)x_alloc_aligned_dma_mem(256*3, 128);	   
      tPngPlte.u4PlteData = (UINT32)x_alloc_aligned_dma_mem(256*3, 128);
      _u4RegBuf = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
      au4RegBBuf[0] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
      au4RegBBuf[1] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
      au4RegABuf[0] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
      au4RegABuf[1] = (UINT32)x_alloc_aligned_dma_mem(32*4*4, 128);
      #else
      u4PngStrmOut    = (UINT32)x_alloc_vmem(PNG_STM_OUT_SIZE);
      u4PngStrmIn     = (UINT32)x_alloc_vmem(PNG_STM_IN_SIZE);
      u4PngClrBuf[0]   = (UINT32)x_alloc_vmem(PNG_COLOR_BUF_SIZE);
      u4PngClrBuf[1]   = (UINT32)x_alloc_vmem(PNG_COLOR_BUF_SIZE);
      u4PngPngRingFifo = (UINT32)x_alloc_vmem(PNG_RING_FIFO_SIZE);
      tPngTrns.u4TrnsData = (UINT32)x_alloc_vmem(256*3);	   
      tPngPlte.u4PlteData = (UINT32)x_alloc_vmem(256*3);
      #endif
	  #if CONFIG_SYS_MEM_PHASE2
      _pu1PngPlt = (UINT8 *)x_alloc_aligned_dma_mem(256*32, 16);	
      u4PngWbHdrMem   = (UINT32)x_alloc_aligned_dma_mem(32, 256); 
      _pbPngFileList = (UINT8 *)x_alloc_aligned_dma_mem(PngFileListMax*512, 16);
      u4PngGoldenData = (UINT32)x_alloc_aligned_dma_mem(PNG_GOLDEN_DATA, 256);
	  
      #if PNG_MMU_TEST
      u4PngStrmIn1    = (UINT32)x_alloc_aligned_dma_mem(PNG_STM_IN_SIZE, 256);
      #endif
	  #elif CONFIG_SYS_MEM_PHASE3
	  _pu1PngPlt = (UINT8 *)x_alloc_aligned_ch2_mem(256*32, 16);	
      u4PngWbHdrMem   = (UINT32)x_alloc_aligned_ch2_mem(32, 256); 
      _pbPngFileList = (UINT8 *)x_alloc_aligned_ch2_mem(PngFileListMax*512, 16);
      u4PngGoldenData = (UINT32)x_alloc_aligned_ch2_mem(PNG_GOLDEN_DATA, 256);
	  
      #if PNG_MMU_TEST
      u4PngStrmIn1    = (UINT32)x_alloc_aligned_ch2_mem(PNG_STM_IN_SIZE, 256);
      #endif
	  #endif
      //_pbPngGoldenData1 = (UINT32)x_alloc_aligned_dma_mem(PNG_GOLDEN_DATA, 256);
      #endif
    #else
    u4PngStrmIn     = (UINT32)x_alloc_aligned_nc_mem(PNG_STM_IN_SIZE, 256);
    u4PngStrmOut    = (UINT32)x_alloc_aligned_nc_mem(PNG_STM_OUT_SIZE, 256);
    u4PngWbHdrMem   = (UINT32)x_alloc_aligned_nc_mem(32, 256);
    //u4PngGoldenData = (UINT32)x_alloc_aligned_nc_mem(PNG_GOLDEN_DATA, 256);
    //u4PngStrmOut    = (UINT32)_u4PngStrmOut; 
	//u4PngGoldenData = (UINT32)_u4PngGoldenData; 
    u4PngClrBuf[0]   = (UINT32)x_alloc_aligned_nc_mem(PNG_COLOR_BUF_SIZE, 256);
    u4PngClrBuf[1]   = (UINT32)x_alloc_aligned_nc_mem(PNG_COLOR_BUF_SIZE, 256);
    u4PngPngRingFifo = (UINT32)x_alloc_aligned_nc_mem(PNG_RING_FIFO_SIZE, 256);
    _pu1PngPlt = (UINT8 *)x_alloc_aligned_nc_mem(256*32, 16);
    #endif

    _pbPngStrmIn        = (UINT8*)u4PngStrmIn;
    #if PNG_MMU_TEST
    _pbPngStrmIn1       = (UINT8*)u4PngStrmIn1;
    #endif
    _pbPngStrmOut       = (UINT8*)u4PngStrmOut;
    _pu4PngWbHdrMem     = (UINT8*)u4PngWbHdrMem;
    _pbPngGoldenData    = (UINT8*)u4PngGoldenData;
    _pbPngClrBuf[0]     = (UINT8*)(u4PngClrBuf[0]);
    _pbPngClrBuf[1]     = (UINT8*)(u4PngClrBuf[1]);
    _pbPngRingFifo      = (UINT8*)u4PngPngRingFifo;

    _rPngClipPara.fgClipEn = FALSE;
    _rPngClipPara.u4ClipHeight = 200;
    _rPngClipPara.u4ClipWidth = 200;
    _rPngClipPara.u4ClipTop = 800;
    _rPngClipPara.u4ClipLeft = 800;

    {
        UINT32 u4I;

        //for(u4I=0; u4I<PngFileListMax*512; u4I++)
        //    _pbPngFileList[u4I] = 0;

        for(u4I=0; u4I<PngFileListMax; u4I++)
            _abPngFileList[u4I] = 0;
    }

    i4RvdPrintf("\n");
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
    _fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
    i4CurrentGoldenType = 2;

     i4PngHalInitHwInst();
     u4TempValue = 0;
     fgPngAutoskipEnable = TRUE;
     fgResumeEnable = FALSE;
     UTIL_Printf("u4PngStrmOut = %x!\n",u4PngStrmOut);
     UTIL_Printf("u4PngStrmIn = %x!\n",u4PngStrmIn);
     //UTIL_Printf("u4PngStrmIn1 = %x!\n",u4PngStrmIn1);
     UTIL_Printf("u4PngClrBuf[0] = %x!\n",u4PngClrBuf[0]);
     UTIL_Printf("u4PngClrBuf[1] = %x!\n",u4PngClrBuf[1]);
     UTIL_Printf("u4PngPngRingFifo = %x!\n",u4PngPngRingFifo);
     UTIL_Printf("tPngTrns.u4TrnsData = %x!\n",tPngTrns.u4TrnsData);
     UTIL_Printf("tPngPlte.u4PlteData = %x!\n",tPngPlte.u4PlteData);
     for(u4TempValue = 0; u4TempValue < 1; u4TempValue++)
     {
           //if(fgPngAutoskipEnable)
             //i4PngPrintf( "[PNG_EMU]Begin png auto skip mode decoding !!!!!!!! \n \n \n \n \n");
           //else	   	
             //i4PngPrintf( "[PNG_EMU]Begin png 8520 mode decoding !!!!!!!! \n \n \n \n \n");
           //for(_u4CurPngFileListNum=0; _u4CurPngFileListNum<_u4PngFileListNum; _u4CurPngFileListNum++)
           {         
              INT32 _i4PngFileSize ;
              BOOL fgDecodeSuccess = FALSE;
              BOOL fgTestNormalMode = TRUE;
              #if PNG_MMU_TEST
              _i4PngFileSize = i4RvdReadFile("D:\\1.png",(UINT32)_pbPngStrmIn1,PNG_STM_IN_SIZE);
              #else
              _i4PngFileSize = i4RvdReadFile("D:\\1.png",(UINT32)_pbPngStrmIn,PNG_STM_IN_SIZE);
              #endif
              _fgLoadingfile = FALSE;
              #if PNG_MMU_TEST
              x_memcpy((void *)_pbPngStrmIn,(void *)_pbPngStrmIn1,PNG_STM_IN_SIZE);
              #endif
              i4RvdPrintf("[Png][Verify] Load File %s",_abPngFileList[_u4CurPngFileListNum]);
              #if 0 //DRAM_BUSY_TEST
              u4TempValue1 = (UINT32)x_alloc_aligned_dma_mem(0x200000,1024);
              *(UINT32*)(0xF0007210) = PHYSICAL(u4TempValue1);
              UTIL_Printf("0xF0007210 Value = 0x%x\n",*(UINT32*)(0xF0007210));
              *(UINT32*)(0xF0007214) = 0x200000;
              *(UINT32*)(0xF0007104) = 0x0;
              *(UINT32*)(0xF0007218) = 0x8E0F110D;
              #endif
              fgWtEnable = TRUE;//   FALSE;
              //if(fgWtEnable)
              {
                 u4ResizeAndCropTimes = 0;//for skip load golden file, and cal decode one file times
                 u4ResizeAndCropTimesForWt = 0;
              }
              if(_i4PngFileSize < 0)
              {
                 i4RvdPrintf("[Png][Verify] Load File fail\n");
              }
              else
              {
              while(1)
              {
                if(fgTestNormalMode)
                {
                  
                  //u4ResizeAndCropTimes = 0;
                   //i4PngPrintf("[Png][Verify] Decode Png by HW - normal mode\n");
                   fgDecodeSuccess = fgPngDecodeSpecialForWT();	
                   if(fgDecodeSuccess)
                   {
                        if(0 == (u4ResizeAndCropTimes % 10))
                        i4PngPrintf("[Png][Verify]Normal mode Decode success 10 times,decoding mode = %d \n",fgPngAutoskipEnable);
                   }
                    else
                    {
                        i4PngPrintf("[Png][Verify]Normal mode Decode fail at %d times,decoding mode = %d \n",u4ResizeAndCropTimes,fgPngAutoskipEnable);
                    }
                   
                }
                fgResumeEnable = FALSE;
              }
              }
           }
           fgPngAutoskipEnable = FALSE;
         }

    //free memory
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
      #ifdef PNG_ALLOCATE_CH2
      x_free_aligned_ch2_mem((void*)u4PngStrmIn);
      x_free_aligned_ch2_mem((void*)u4PngStrmOut);
      x_free_aligned_ch2_mem((void*)u4PngWbHdrMem);
      x_free_aligned_ch2_mem((void*)u4PngGoldenData);
      x_free_aligned_ch2_mem((void*)u4PngClrBuf[0]);
      x_free_aligned_ch2_mem((void*)u4PngClrBuf[1]);
      x_free_aligned_ch2_mem((void*)_pbPngRingFifo);
      x_free_aligned_ch2_mem((void*)_pu1PngPlt);
      x_free_aligned_ch2_mem((void*)tPngTrns.u4TrnsData);	
      x_free_aligned_ch2_mem((void*)tPngPlte.u4PlteData);
      #else
    #if (!PNG_MMU_TEST)
	#if CONFIG_SYS_MEM_PHASE2
    x_free_aligned_dma_mem((void*)u4PngStrmIn);
    x_free_aligned_dma_mem((void*)u4PngStrmOut);
    x_free_aligned_dma_mem((void*)u4PngClrBuf[0]);
    x_free_aligned_dma_mem((void*)u4PngClrBuf[1]);
    x_free_aligned_dma_mem((void*)_pbPngRingFifo);
    x_free_aligned_dma_mem((void*)tPngTrns.u4TrnsData);	
    x_free_aligned_dma_mem((void*)tPngPlte.u4PlteData);
	#endif
    #else
    //x_free_vmem();
    //x_free_vmem((void*)u4PngStrmOut);
    //x_free_vmem((void*)u4PngClrBuf[0]);
    //x_free_vmem((void*)u4PngClrBuf[1]);
    //x_free_vmem((void*)_pbPngRingFifo);
    //x_free_vmem((void*)tPngTrns.u4TrnsData);	
    //x_free_vmem((void*)tPngPlte.u4PlteData);
    #endif
	#if CONFIG_SYS_MEM_PHASE2
	x_free_aligned_dma_mem((void*)tPngTrns.u4TrnsData);	
    x_free_aligned_dma_mem((void*)tPngPlte.u4PlteData);
    x_free_aligned_dma_mem((void*)_pbPngFileList);
    x_free_aligned_dma_mem((void*)u4PngWbHdrMem);
    //x_free_aligned_dma_mem((void*)u4PngGoldenData);
    x_free_aligned_dma_mem((void*)_pu1PngPlt);
	#endif
    #endif
    #else
    x_free_aligned_nc_mem((void*)u4PngStrmIn);
    x_free_aligned_nc_mem((void*)u4PngStrmOut);
    x_free_aligned_nc_mem((void*)u4PngWbHdrMem);
    x_free_aligned_nc_mem((void*)u4PngGoldenData);
    x_free_aligned_nc_mem((void*)u4PngClrBuf[0]);
    x_free_aligned_nc_mem((void*)u4PngClrBuf[1]);
    x_free_aligned_nc_mem((void*)_pbPngRingFifo);
    x_free_aligned_nc_mem((void*)_pu1PngPlt);
    #endif
   
    i4PngHalUnInitHwInst();

}
void vPNGWTMMUTest(void)
{
  HANDLE_T hThread1;    
  
  if (x_thread_create(&hThread1, "PNG", 1024, 160,
        vPngWTMMUTest, 0, NULL) != OSR_OK)  
  {

    ASSERT(0);
  }  
  while(_fgLoadingfile)
  {
      x_thread_delay(200);
  }
  UTIL_Printf("!!!File Loading Finished! Please Call Next CLI!!!\n");
  //x_thread_delay(200);
}



