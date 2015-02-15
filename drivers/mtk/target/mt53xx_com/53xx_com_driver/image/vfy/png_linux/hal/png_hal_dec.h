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

#ifndef __PNG_HAL_DEC_H
#define __PNG_HAL_DEC_H

#include "x_typedef.h"
#include "png_com_hdr.h"
#include "drv_img_dec.h"
#include "x_common.h"
#include "x_os.h"

#if 0
#include "x_common.h"
#include "x_img_dec.h"
#include "png_debug.h"
#include "chip_ver.h"
#include "x_iommu.h"

#include <mach/cache_operation.h>
#include "sys_config.h"
#endif

#if(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
#define PNG_MMU_TEST               1
#else
#define PNG_MMU_TEST                0
#endif
#define PNG_CACHE_MAP_ENABLE      1

#define PNG_LZ77_SIZE       32*1024

#define PNG_HAL_INVALID32      0xFFFFFFFF
#define PNG_HAL_RTN_STOPPED  4
#define PNG_HAL_RTN_DEC      3
#define PNG_HAL_RTN_SRC      2
#define PNG_HAL_RTN_DST      1
#define PNG_HAL_RTN_OK       0
#define PNG_HAL_RTN_FAIL     -1
#define PNG_HAL_ALDER_ERR    -2
#define PNG_HAL_TIMEOUT     1000

#if 0
typedef enum _GFX_COLORMODE_T
{
    GFX_COLORMODE_AYCbCr_CLUT2  = 0,
    GFX_COLORMODE_AYCbCr_CLUT4  = 1,
    GFX_COLORMODE_AYCbCr_CLUT8  = 2,
    GFX_COLORMODE_CbYCrY_16     = 3,
    GFX_COLORMODE_YCbYCr_16     = 4,
    GFX_COLORMODE_AYCbCr_D8888  = 5,
    GFX_COLORMODE_ARGB_CLUT2    = 6,
    GFX_COLORMODE_ARGB_CLUT4    = 7,
    GFX_COLORMODE_ARGB_CLUT8    = 8,
    GFX_COLORMODE_RGB_D565      = 9,
    GFX_COLORMODE_ARGB_D1555    = 10,
    GFX_COLORMODE_ARGB_D4444    = 11,
    GFX_COLORMODE_ARGB_D8888    = 12,
    GFX_COLORMDOE_YUV_420_BLK   = 13,
    GFX_COLORMODE_YUV_420_RS    = 14,
    GFX_COLORMDOE_YUV_422_BLK   = 15,
    GFX_COLORMODE_YUV_422_RS    = 16,
    GFX_COLORMDOE_YUV_444_BLK   = 17,
    GFX_COLORMODE_YUV_444_RS    = 18
} GFX_COLORMODE_T;
#endif

typedef struct _PNG_HAL_PIC_INFO_T
{
    PNG_IHDR            *ptIhdr;
   
    UINT8               u4AlphaUnMatchedVal;
    UINT32              u4FifoStart;
    UINT32              u4FifoEnd;
    UINT32              u4DstBufPitch;
    UINT32              u4SliceHeight;
    UINT32              u4CropX;
    UINT32              u4CropY;
    UINT32              u4CropWidth;
    UINT32              u4CropHeight;
    UINT32              u4ResizeHor;
    UINT32              u4ResizeVrt;
	 GFX_COLORMODE_T     eOutClrMd;
} PNG_HAL_PIC_INFO_T;

typedef struct _PNG_HAL_IDAT_INFO_T
{
    UINT32              u4ChunkType;
    UINT32              u4SrtStrmAddr;
    UINT32              u4SrtStrmLen;
    UINT32              u4DstBufStart;
    UINT32              u4DstBufEnd;
    BOOL                fgLastIDATGrp;
    BOOL                fgLastSlice;
    BOOL                fg1stIDATChunk;
} PNG_HAL_IDAT_INFO_T;




#define PNG_HAL_HW_INST_NUM     1
typedef struct 
{
   UINT32 u4MemAddress;
   UINT32 u4MemLen;
   UINT32 u4Option;
   BOOL fgNoRingFifo;
}PNG_MAP_BUF_T;

typedef struct _PNG_HAL_HW_INST_T
{
    UINT32              u4HwInstId;         ///< HW inst id
    HANDLE_T            hPngEvent;          ///< the event id
    BOOL                fgActive;
    UINT32              u4HwRegBase;        ///< register base

    PNG_IHDR            tIhdr;

    UINT32              u4OutBufAddr;   //high must 8 alignment
  
    UINT32              u4LZ77BufAddr;
    UINT32              u4LineBufAddr;
    BOOL                fgWtEnable;
	BOOL                fgUseClrBuf;
#if PNG_CACHE_MAP_ENABLE
   PNG_MAP_BUF_T  rSrc;
   PNG_MAP_BUF_T rDst;
   UINT32 u4RingFifoStart;
   UINT32 u4RingFifoEnd;
   PNG_MAP_BUF_T rLineBuf;
   PNG_MAP_BUF_T rLz777Buf;   
  #endif  
    GFX_COLORMODE_T     eOutClrMd;
} PNG_HAL_INST_T;




//#define PNG_LINE_BUF_SIZE   4096*8  //width * pixel length



INT32 i4PngHalInitHwInst(void);
INT32 i4PngHalUnInitHwInst(void);
INT32 i4PngHalWaitDec(UINT32 u4HwInstId, UINT32 u4WaitTime);

INT32 i4PngHalGetHwInst(UINT32 *pu4HwInstId);
INT32 i4PngHalRelHwInst(UINT32 u4HwInstId);
INT32 i4PngHalSetScale(UINT32 u4HwInstId, UINT32 u4PngInterlaceScale,UINT32 u4PngHorScale,UINT32 u4PngVrtScale);

INT32 i4PngHalSetStop(UINT32 u4HwInstId);
void i4PngLocalAbitorReset(UINT32 u4HwInstId);
INT32 i4PngHalReset(UINT32 u4HwInstId);
INT32 i4PngHalEnable(UINT32 u4HwInstId);
INT32 i4PngHalDisable(UINT32 u4HwInstId);

INT32 i4PngHalSetPicInfo(UINT32 u4HwInstId, PNG_HAL_PIC_INFO_T *ptPngPicInfo);
INT32 i4PngHalSetPLTE(UINT32 u4HwInstId, PNG_PLTE *ptPngPlte);
INT32 i4PngHalSettRNS(UINT32 u4HwInstId, PNG_TRNS *ptPngTrns, BOOL fgPlte);
INT32 i4PngHalSetbKGD(UINT32 u4HwInstId, PNG_BKGD *ptPngBkgd, PNG_PLTE *ptPngPlte, PNG_TRNS *ptPngTrns, BOOL fgPlte, BOOL fgTrns);

INT32 i4PngHalProcIDAT(UINT32 u4HwInstId, PNG_HAL_IDAT_INFO_T *ptPngIdatInfo);
INT32 i4PngHalProcNextSrc(UINT32 u4HwInstId, UINT32 u4SrcStrmSrtAddr, UINT32 u4SrcStrmLen, BOOL fgLastIDATGroup);
INT32 i4PngHalProcNextDst(UINT32 u4HwInstId, UINT32 u4DstBufStart, UINT32 u4DstBufEnd, BOOL fgLastSlice);
UINT32 u4PngHalReadCrc(UINT32 u4HwInstId);
INT32 i4PngHalSetSliceWH(UINT32 u4HwInstId, UINT32 u4SliceHeight);

INT32 i4PngHalSetAutoSkipDisable(UINT32 u4HwInstId);
INT32 i4PngHalSetAutoSkipEnable(UINT32 u4HwInstId);
INT32 i4PngHalSetAutoSkipLastIdat(UINT32 u4HwInstId, BOOL fgLastIdat);
UINT32 u4PngHalGetAutoSkipCrc(UINT32 u4HwInstId);
BOOL fgPngWTSet(UINT32 u4HwInstId,BOOL fgEnable);
void vPngPngHalSetRingFifo(UINT32 u4HwInstId, UINT32 u4FifoStart, UINT32 u4FifoEnd);
void vPngPngHalUseTmpDstBufer(UINT32 u4HwInstId, BOOL fgTmpUsed);
void vPngHalMapSrcAddress(UINT32 u4HwInstId, UINT32 u4MemStart, UINT32 u4MemLen, UINT32 u4Option, BOOL fgNoRingFifo);
void vPngHalunMapSrcAddress(UINT32 u4HwInstId);
#if PNG_MMU_TEST
void vPngMMUInit(void);
void vPngMMUFlush(void);
#if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
void PNG_MMU_ENABLE(void);
#endif
void vPrintfMMUStatus(void);
#endif
INT32 i4PngHalPitchEnable(UINT32 u4HwInstId, BOOL fgPitchEn);
INT32 i4PngHalGraceReset(UINT32 u4HwInstId);
BOOL fgPngGraceResetDone(UINT32 u4HwInstId);

#endif // __PNG_HAL_DEC_H
