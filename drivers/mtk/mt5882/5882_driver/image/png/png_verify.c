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
#include "jpg_if.h"
#if ((defined(PNG_VFY_AUTO_TEST) || defined(PNG_DRV_FLOW_TEST)) || defined(__MODEL_slt__))
#include "x_common.h"
#include "x_assert.h"
#include "x_os.h"
//#include "x_rtos.h"
#include "x_bim.h"
#include "x_hal_5381.h"
#include "x_hal_926.h"

#include "x_typedef.h"
#include "x_stl_lib.h"
//#include "x_hal_ic.h"
//#include "rvd_util.h"
#include "png_hal_dec.h"
#include "png_drv.h"
//#include "dmx_hw.h"
#include "png_debug.h"
//#include "x_mmap.h"
//#include "x_hal_1176.h"
//#include "x_debug.h"
//#include "fci_if.h"
#include "drv_t32.h"
#include "png_verify.h"

#define MT8520_IMGDEC_PNG_VFY
//#define MMU_4K_1M_SWTICH

//////////////////// Test Defined //////////////////////////////////////
typedef struct
{
    UINT32 u4DataLength;
    UINT32 u4Type;
    UINT32 u4CRC;
    BOOL   fgLastChunk;
} PngChunkInfo;

typedef struct
{
    UINT32 u4Width;
    UINT32 u4Height;
    UINT8  u1BitDepth;
    UINT8  u1ColorType;
    BOOL   fgInterlace;
} PngIHDRHeader;

typedef struct
{
    UINT32 u4DispLeft;
    UINT32 u4DispTop;
    UINT32 u4DispWidth;
    UINT32 u4DispHeight;
} PngDispPara;

typedef struct
{
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

#define PNG_STM_IN_SIZE     24*1024*1024
#define PNG_STM_OUT_SIZE    90*1024*1024
#define PNG_GOLDEN_DATA     90*1024*1024
//#define PNG_LZ77_SIZE       32*1024
//#define PNG_LINE_BUF_SIZE   4096 * 8 //width * pixel length

#define PNG_RING_SIZE       (64*1024)
#define PNG_RING_FIFO_SIZE  ((PNG_RING_SIZE * 5) / 2)
#define PNG_SLICE_HEIGHT    10
#define PNG_COLOR_BUF_SIZE  PNG_SLICE_HEIGHT * 4096 * 8

#define PNG_NEW_PARSING         1
#define PNG_INVALID32      0xFFFFFFFF
#define PNG_DECODE_WAIT_TIME   30000
//#define PngReadREG(arg)        IO_READ32(IMAGE_BASE, (arg & 0x3FFF))
#define PngReadREG(base, arg)         IO_READ32(IMAGE_BASE, ((base + arg) & 0xFFFFF))
#define PngWriteREG(base, arg, val)  IO_WRITE32(IMAGE_BASE, ((base + arg) & 0xFFFFF), val)

UINT32 au4RegBBuf[2];
UINT32 au4RegABuf[2];
UINT32 u4CntDecode = 0;
//////////////////// Global Variable ///////////////////////////////////
#ifndef MT8520_IMGDEC_PNG_VFY
static OSS_TASK _rPngEMUTask;
#endif
PNG_HAL_PIC_INFO_T rPngHalPicInfo;
PNG_HAL_IDAT_INFO_T rPngHalIdatInfo;
//PNG_HDR_INFO_T rPngHdrInfo;
PNG_PLTE tPngPlte;
PNG_TRNS tPngTrns;

BOOL _fgDecPngFile = FALSE;
BOOL _fgPngDecUseRingFifo = TRUE;
BOOL _fgPngDecUseColorBuf = TRUE;
BOOL _fgLoadingfile = TRUE;
BYTE* _pbPngFilePreFix;
BYTE* _pbPngFileMidFix;
UINT32 _u4PngGroupFileNum;
UINT32 _u4RegBuf;
BYTE _arbPngGoldenPostFix[4][32] =
{
    "\\gold_argb8888.bin",
    "\\gold_argb4444.bin",
    "\\gold_argb1555.bin",
    "\\gold_pltt_idx.bin"
};

PngChunkInfo    _rPngChunkInfo ;
PNG_IHDR   _rPPngIHDRHeader = {0,0,0,0,0};
PngDispPara     _rPngDispPara;
PngClipPara     _rPngClipPara;

UINT32 _u4PngImgDataAddr;

UINT32 _u4PngHorScale = 0;
UINT32 _u4PngVrtScale = 0;
UINT32 _u4PngIntScale = 7;
GFX_COLORMODE_T  _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
BOOL _fgOurClrPalette = FALSE;
UINT32 _u4PngPixelByte = 4;

UINT32 _u4PngOutStmCount = 0;
UINT32 _u4PngInStmCount = 0;

UINT32 _u4PngColorBufSize = 0;
BOOL   _fgPngPalette;
BOOL   _fgPngTranEn;
BOOL   _fgPngDispOut = FALSE;
UINT32 _u4PngIDATCount = 0;

INT32 _iPngTestPattenGroup = 0;
UINT8 *_pu1PngPlt;
UINT8 *_pu1PngTrans;
BOOL  fgCompareChecksum = TRUE;

//add for auto test
UINT32 _u4PngOutFormatSelect = 0x10;
BOOL fgPngAutoskipEnable = TRUE;
BOOL fgResumeEnable      = FALSE;
BOOL fgResizeEnable      = FALSE;
BOOL fgPngOutFormatTest  = FALSE;
UINT32 u4ResizeAndCropTimes = 0;
UINT32 u4ResizeAndCropTimesForWt = 0;
BOOL fgLoadData = FALSE;
UINT32 u4FileCnt = 0;

BOOL fgWtEnable = FALSE;
UINT32 u4DstShift = 0;
//end of auto test support val
//HANDLE_T _hPngEventHandle;
UINT32 _u4PreDstBufStartVal = 0;
UINT32 _u4PreSrcBufStartVal = 0;
UINT32 _u4HwInistId = PNG_INVALID32;

BYTE _arbPngFile[512];

UINT32 _u4PngUnMatchedRegVal = 0xFF;
UINT32 _u4BitStreamCount = 0;
UINT32 _u4PngSliceCount = 0;
UINT32 _u4PngGoldenSize = PNG_GOLDEN_DATA;
UINT8 _PicNum = 48;
BYTE _arbPngGoldenFile[512];
BYTE _arbPngCheckSumFile[512];
UINT32 u4SilceNum = 0;

//load png file related var&definition
#define PngFileListMax 200
UINT8* _pbPngFileList;
UINT32 _pbPngGoldenData1;
UINT32 _u4PngFileListNum = 0;
UINT32 _u4CurPngFileListNum = 0;
BYTE *_abPngFileList[PngFileListMax];
INT32 i4CurrentGoldenType = 0;
UINT32 u4FifoReadPoint = 0;
UINT32 u4PngFileListSize = 360;

BOOL fgCropTest =TRUE;
BOOL fgColorfmtTest = TRUE;
BOOL fgSrcDstResumeTest = TRUE;
BOOL fgNormalTest = TRUE;

BOOL _stopPngVerify = FALSE;
UINT32 PngCheckSum[PngFileListMax][6] =
{
#include "png_checksum.h"
};
#if (PNG_MMU_TEST)
/*1M table won't work on Cobra*/
#define PNG_4K_Table (1)
//void HalSet4KPageTable(UINT32 u4PhyAddr, UINT32 u4VirtAdr, UINT32* pu4PageTableAddress)
void GGT_get_4K(UINT32 u4StartAdd)
{
    UINT32 i = 0;
    UINT32* u4PageTableAdd;

    u4PageTableAdd = (UINT32 *)BSP_AllocAlignedDmaMemory(0x4000, 0x10000);

    for (i = 0; i < 0x100; i++)
    {
        HalSet4KPageTable(
            PHYSICAL((UINT32)u4StartAdd + (0xFF - i)*0x1000),
            ((UINT32)u4StartAdd + i * 0x1000),
            u4PageTableAdd
        );
    }
}
void* GGT_get_vmem(UINT32 u4Size)
{
    UINT32 i = 0;
    UINT32 *u4Add;

    u4Size = ((u4Size + 0xFFFFF) / 0x100000) * 0x100000;
    u4Add = (UINT32 *)BSP_AllocAlignedDmaMemory(u4Size, 0x100000);
    Printf("GGT_get_vmem: 0x%x from 0x%x\n", u4Size, u4Add);
    ASSERT(u4Add != NULL);    
    for (i=0; i< u4Size /0x100000; i++)
    {
#if PNG_4K_Table
        GGT_get_4K(((UINT32)u4Add + i * 0x100000));
#else
        HalSet1MSectionTable(
            PHYSICAL((UINT32)u4Add + (u4Size/0x100000 - i - 1) * 0x100000),
            ((UINT32)u4Add + i * 0x100000)
        );
#endif
    }

    return u4Add;
}
//extern void* x_free_vmem(void* pUser);
#endif

#ifndef PNG_SEMI_HOST
#define PNG_SEMI_HOST
#endif

//////////////////// Buffer ////////////////////////////////////////////
#ifdef MT8520_IMGDEC_PNG_VFY
//[CJYANG] imgdec_vfy, need allocate memory
//UINT8 *_pbPngEMUTaskStack;
UINT8 *_pbPngStrmIn;
#if (PNG_MMU_TEST)
//UINT8 *_pbPngStrmIn1 = 0;
#endif
UINT8 *_pbPngStrmOut;
UINT8 *_pu4PngWbHdrMem;
UINT8 *_pbPngGoldenData;
UINT8 *_pbPngClrBuf[2];
UINT8 *_pbPngRingFifo;
#endif
//////////////////// External Functions ////////////////////////////////

extern void HalFlushInvalidateDCache(void);
extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern INT32 _PngVerifyCmdGetParam(INT32 i4ParamType);
//////////////////// Internal Functions ////////////////////////////////

UINT32 _CalcPngCheckSum(void* pvData, UINT32 u4ByteLen);


INT32 i4PngReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
#ifdef PNG_SEMI_HOST
    Printf("[PNG_EMU]Load File:%s\n",pcFileName);
    HalFlushInvalidateDCacheMultipleLine(u4FileBufSa,u4BufSize);
    UNUSED(T32_HostExec(E_T32_EXEC, "d.load.binary \"%s\" 0x%08x /ny", pcFileName,u4FileBufSa));
    HalFlushInvalidateDCacheMultipleLine(u4FileBufSa,u4BufSize);
#endif

    return u4PngFileListSize;
}

#define i4RvdReadFile i4PngReadFile

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
}

void vPngInitPicInfor(PNG_HAL_PIC_INFO_T *prPngHalPicInfo)
{
    UINT32 u4RegTmpValue = 0;
    if (_fgPngDecUseRingFifo)
    {
#if (PNG_MMU_TEST)
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
#if (PNG_MMU_TEST)
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
    prPngHalIdatInfo->u4ChunkType  = u4ChunkType;
    prPngHalIdatInfo->fgLastIDATGrp = fgLastIDAT;
    //prPngHalIdatInfo->fg1stIDATChunk
    prPngHalIdatInfo->u4SrtStrmAddr = (UINT32)(_pbPngStrmIn + _u4PngImgDataAddr);
    prPngHalIdatInfo->u4SrtStrmLen   = _rPngChunkInfo.u4DataLength;
    //Printf("vPngSetIDATDecReg\n");

    _u4PreSrcBufStartVal = prPngHalIdatInfo->u4SrtStrmAddr ;

    if (!_fgPngDecUseColorBuf)
    {
        prPngHalIdatInfo->fgLastSlice = TRUE;
    }
    else
    {
        prPngHalIdatInfo->fgLastSlice = FALSE;
        if (u4SilceNum == _u4PngSliceCount)
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
            // Printf("Before copy ..... 0x%08x,0x%08x,0x%08x\n",_pbPngRingFifo,_pbPngStrmIn,_u4PngImgDataAddr);
            // x_memcpy(VIRTUAL((void *)(_pbPngRingFifo)), VIRTUAL((void *)(_pbPngStrmIn + _u4PngImgDataAddr)), PNG_RING_SIZE);
            //Printf("Before copy 222..... 0x%08x,0x%08x,0x%08x\n",_pbPngRingFifo,_pbPngStrmIn,_u4PngImgDataAddr);
            x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), PNG_RING_SIZE);
            u4PngUpdateFifoRdPoint(PNG_RING_SIZE);
            _u4PngImgDataAddr += PNG_RING_SIZE;

        }
        else
        {
            if (_rPngChunkInfo.fgLastChunk)
                prPngHalIdatInfo->fgLastIDATGrp  = TRUE;
            u4ValidDataLen = _rPngChunkInfo.u4DataLength;
            if (u4ValidDataLen == 0)
            {
                return ;
            }
            else
            {
                //Printf("Before copy 333..... 0x%08x,0x%08x,0x%08x,%d\n",_pbPngRingFifo,_pbPngStrmIn,_u4PngImgDataAddr,u4ValidDataLen);
                //x_memcpy(VIRTUAL((void *)(_pbPngRingFifo)), VIRTUAL((void *)(_pbPngStrmIn + _u4PngImgDataAddr)), u4ValidDataLen);
                //Printf("Before copy 444..... 0x%08x,0x%08x,0x%08x\n",_pbPngRingFifo,_pbPngStrmIn,_u4PngImgDataAddr);
                x_memcpy((void *)(_pbPngRingFifo), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), u4ValidDataLen);
            }
            _u4PngImgDataAddr += u4ValidDataLen;
            _rPngChunkInfo.u4DataLength = 0;
        }
        prPngHalIdatInfo->u4SrtStrmLen   = u4ValidDataLen ;
        prPngHalIdatInfo->u4SrtStrmAddr = (UINT32)_pbPngRingFifo;


    }
    _u4BitStreamCount = 0;
    if (fgPngAutoskipEnable)
    {
        if (prPngHalIdatInfo->fgLastIDATGrp)
            i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
        prPngHalIdatInfo->fgLastIDATGrp = FALSE;
    }
    HalFlushInvalidateDCache();
    i4PngHalProcIDAT(_u4HwInistId, prPngHalIdatInfo);
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
        if ( (PNG_RING_FIFO_SIZE - u4FifoReadPoint) >= PNG_RING_SIZE)
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
        if (_rPngChunkInfo.fgLastChunk)
            fgLastIDATGroup = TRUE;
        u4SrcStrmLen = _rPngChunkInfo.u4DataLength;
        if (u4SrcStrmLen == 0)
        {
            return FALSE;
        }
        else
        {
            if ( (PNG_RING_FIFO_SIZE - u4FifoReadPoint) >= u4SrcStrmLen)
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

    if (fgPngAutoskipEnable)
    {
        if (fgLastIDATGroup)
            i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
        fgLastIDATGroup = FALSE;
    }
    HalFlushInvalidateDCache();
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
    //UINT32 u4Tempval;
    //FILE* fFileHandle;
    i4Ret = i4PngHalWaitDec(_u4HwInistId, PNG_DECODE_WAIT_TIME);
    HalFlushInvalidateDCache();
    while (1)
    {
        switch (i4Ret)
        {
        case PNG_HAL_RTN_DEC:
        {
            //#if PNG_USE_NEW_CLR_BUF
            if (_fgPngDecUseColorBuf)
            {
                if (fgWtEnable)//if dst resume.	need set pixel byte number exactly
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
                if (fgWtEnable)//if dst resume.	need set pixel byte number exactly
                {
                    _u4PngPixelByte = 4;
                }
            }
            //#endif
            Printf( "[PNG_EMU] Decode Done %x\n", i4Ret);
            //Printf( "[PNG_EMU] 1st Decode Done Register Setting\n\n\n");
            /*
                        if (0 == u4ResizeAndCropTimes)
                        {
                            for (u4Tempval = 0; u4Tempval < 32; u4Tempval++)
                            {
                                x_thread_delay(5);
                                Printf("[Png] 1st decoded register = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
                                            *(UINT32*)(0xF0004E00 + u4Tempval*16 + 0),
                                            *(UINT32*)(0xF0004E00 + u4Tempval*16 + 4),
                                            *(UINT32*)(0xF0004E00 + u4Tempval*16 + 8),
                                            *(UINT32*)(0xF0004E00 + u4Tempval*16 + 12));
                            }
                        }
                        for (u4Tempval = 0; u4Tempval < 32*4*4; u4Tempval+=4)
                        {
                            *(UINT32*)(au4RegABuf[u4CntDecode % 2] + u4Tempval) = *(UINT32*)(0xF0004E00 + u4Tempval);
                        }
                        */
            u4CntDecode ++;
            goto PNG_DecFinish;
        }
        //break;
        case PNG_HAL_RTN_SRC:
        {
            Printf( "[PNG_EMU] bit stream empty %x\n", i4Ret);
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
            Printf( "[PNG_EMU] Slice Done %x\n", i4Ret);

            //copy memory
            if (_fgPngDecUseColorBuf)
            {
                if (fgWtEnable)//if dst resume.	need set pixel byte number exactly
                {
                    _u4PngPixelByte = 2;
                }
                HalFlushInvalidateDCacheMultipleLine((UINT32)_pbPngClrBuf[(_u4PngSliceCount-1)%2],PNG_COLOR_BUF_SIZE);
                // HalFlushInvalidateDCache();
                pbSrc = _pbPngClrBuf[(_u4PngSliceCount-1)%2];
                pbDst = _pbPngStrmOut + (_u4PngSliceCount-1) * (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT);
                for (i4=0; i4<(_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT); i4++)
                {
                    pbDst[i4] = pbSrc[i4];
                }
                HalFlushInvalidateDCacheMultipleLine((UINT32)(_pbPngClrBuf[_u4PngSliceCount%2]),PNG_COLOR_BUF_SIZE);
#if (!PNG_MMU_TEST)
                u4TmpRegVal = ((UINT32)((_pbPngClrBuf[_u4PngSliceCount%2])));
#else
                u4TmpRegVal = ((UINT32)((_pbPngClrBuf[_u4PngSliceCount%2])));
#endif
                x_memset((void *)(_pbPngClrBuf[_u4PngSliceCount%2]), 0, PNG_COLOR_BUF_SIZE);
                HalFlushInvalidateDCacheMultipleLine((UINT32)(_pbPngClrBuf[_u4PngSliceCount%2]),PNG_COLOR_BUF_SIZE);
                _u4PngSliceCount++;
                if (fgWtEnable)//if dst resume.	need set pixel byte number exactly
                {
                    _u4PngPixelByte = 4;
                }
                if (u4SilceNum == _u4PngSliceCount)
                    fgIfLastSlice = TRUE;
            }
            else
            {
                //u4TmpRegVal = PngReadREG(PNG_DEC_PIXEL_OUT_XY_ADDR_REG);
                _u4PreDstBufStartVal += (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT);
                u4TmpRegVal = _u4PreDstBufStartVal;
            }
            //    PngWriteREG(PNG_DEC_PIXEL_OUT_XY_ADDR_REG, u4TmpRegVal);
            i4PngHalProcNextDst(_u4HwInistId, u4TmpRegVal, u4TmpRegVal + (_rPPngIHDRHeader.u4Width * _u4PngPixelByte * PNG_SLICE_HEIGHT), fgIfLastSlice);
            i4Ret = i4PngHalWaitDec(_u4HwInistId, PNG_DECODE_WAIT_TIME);
        }
        break;
        default:
        {
            Printf( "[PNG_EMU] ********** Alter Err %x\n", i4Ret);
            goto  PNG_DecFinish;
        }
        // break;
        }
    }
PNG_DecFinish:
    return TRUE;
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
    UINT32 u4CurrentX, u4CurrentY;
    UINT32 u4PngHResize = (1 << _u4PngHorScale);
    UINT32 u4PngVResize = (1 << _u4PngVrtScale);
    UINT32 u4IdxTmp;
    UINT32 u4CompDataPixNum = 0;
//--
    for (i=0; i<(_rPPngIHDRHeader.u4Height*_rPPngIHDRHeader.u4Width); i++)
    {
        u4CurrentX = (i) % _rPPngIHDRHeader.u4Width;
        u4CurrentY = (i) / _rPPngIHDRHeader.u4Width;
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
                            Printf( "[PNG_EMU] compare fail at %d\n", i);
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
                    Printf( "[PNG_EMU] compare fail at %d\n", i);
                    Printf("PngVfyLog1.txt", "%s\n", _abPngFileList[_u4CurPngFileListNum]);

                    return FALSE;
                }
            }
        }
    }
    Printf( "[PNG_EMU] compare ok! at %d, %d, Compared Pix Total Number = %d \n", i,j,u4CompDataPixNum);
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
    HalFlushInvalidateDCacheMultipleLine((ptPngPlte->u4PlteData),u4ChunkSize);
    i4tempValue = i4PngHalSetPLTE(u4HwInstId, ptPngPlte);
    if (i4tempValue != PNG_HAL_RTN_OK)
    {
        Printf( "[PNG_EMU]read PALETTE error\n");
    }
}

void vParsePngGoldenFile(void)
{
    BYTE* pCurrentFile ;
    INT32 j = 0;
    INT32 i4Num = 0;
    INT32 i = 0;

    pCurrentFile = (BYTE*)_abPngFileList[_u4CurPngFileListNum];
    j = 0;
    for (j=0; j<128; j++)
    {
        _arbPngGoldenFile[j] = pCurrentFile[j];
        _arbPngCheckSumFile[j] = pCurrentFile[j];
    }
    //parsing golden file path

    for ( i = 0; i < 512; i++)
    {
        if (_arbPngGoldenFile[i] == 0x5C)
        {
            i4Num++;
        }
        if (i4Num == 4)
        {
            INT32 j = 0;
            while (_arbPngGoldenPostFix[i4CurrentGoldenType][j] != 0)
            {
                _arbPngGoldenFile[i] = _arbPngGoldenPostFix[i4CurrentGoldenType][j];
                i++;
                j++;
            }
            if ((fgResizeEnable||fgResumeEnable ||(u4DstShift >0)) && fgWtEnable)
            {
                u4ResizeAndCropTimes = 0;
                _arbPngGoldenFile[i] = u4ResizeAndCropTimesForWt + u4ResizeAndCropTimes + 0x30 + u4DstShift/10;
                _arbPngGoldenFile[i+1] = u4ResizeAndCropTimesForWt + u4ResizeAndCropTimes+ 0x30+ u4DstShift%10;
                i++;
                i++;
            }
            _arbPngGoldenFile[i]        = 0;
            _arbPngGoldenFile[i + 1]  = 0;
            break;
        }
    }
    //parsing check sum file
    i4Num = 0;
    i = 0;
    for ( i = 0; i < 512; i++)
    {
        if (_arbPngCheckSumFile[i] == 0x5C)
        {
            i4Num++;
        }
        if (i4Num == 4)
        {
            INT32 j = 0;
            while (_arbPngGoldenPostFix[4][j] != 0)
            {
                _arbPngCheckSumFile[i] = _arbPngGoldenPostFix[4][j];
                i++;
                j++;
            }
            _arbPngCheckSumFile[i]      = 0;
            _arbPngCheckSumFile[i + 1]  = 0;
            break;
        }
    }
}

#ifdef PNG_VFY_AUTO_TEST
extern BOOL fgTransEn,fgTransOutEn,fgTrans16BitEn,fgTransOrgAlphaEn;
extern UINT8 *ctt_fname;
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
    // UINT32 u4PngCheckSumValue = 0;
#if PNG_NEW_PARSING
    UINT32 u4Png1stIDATOffset = 0;
    UINT32 u4PngIDATEndOffset = 0;
#endif

    HalFlushInvalidateDCache();

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

#if (PNG_MMU_TEST)
    vPngMMUInit();
#endif
#if PNG_NEW_PARSING
    if (fgResumeEnable)
    {
        if (_rPPngIHDRHeader.fgInterlace)
        {
            _fgPngDecUseRingFifo = TRUE;
        }
        else
        {
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
    x_memset((void *)_pbPngStrmOut, 0, PNG_STM_OUT_SIZE);

    //init PNG HW register
    vPngInitPicInfor(&rPngHalPicInfo);
    i4PngHalSetPicInfo(_u4HwInistId, &rPngHalPicInfo);
	
// add png reset avoid  png random decode error
    PngWriteREG(PNG_REG_BASE1, PNG_DEC_RESET_REG, PNG_REG_RESET_START);
   // x_thread_delay(1);
    PngWriteREG(PNG_REG_BASE1, PNG_DEC_RESET_REG, PNG_REG_RESET_END);
 
    if (fgResizeEnable && (!fgWtEnable))
    {
        if (_rPPngIHDRHeader.fgInterlace && (_u4PngIntScale == 1))
        {
            i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, 0, 0);
        }
        else
        {
            i4PngHalSetScale(_u4HwInistId, _u4PngIntScale, _u4PngHorScale, _u4PngVrtScale);
        }
    }

    i4PngHalSetAutoSkipDisable(_u4HwInistId);

    // 128 bits alignment,
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

    //read other chunk
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
        //Printf("u4ChunkType = 0x%08x\n",u4ChunkType);
        if (u4ChunkType == 0x49444154)        //IDAT
        {
            //Printf("IDAT Chunk....\n");
            if ((!_fgPngPalette) && (_fgOurClrPalette))
            {
                Printf( "[PNG_EMU]No palette in PNG file\n");
                return FALSE;
            }
#if PNG_NEW_PARSING
            //auto skip test
            if (fgPngAutoskipEnable)
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
                if (!fgPngAutoskipEnable)
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
            _fgPngPalette = TRUE;
            u4NextChunkType = u4PngGet4Bytes(&pu1Fifo[u4PngFileCount+u4ChunkSize+4+4]);
            if (u4NextChunkType == 0x74524E53)  //tRNS
            {
                rPngHalPicInfo.u4AlphaUnMatchedVal = 0xFF;
                //PngWriteREG(PNG_DEC_ALPHA_UNMATCHED_PIXEL_REG, 0xFF);
            }
            //read palette
            Printf("[PNG_EMU]Enter Palette!\n");
            vPngHwSetPLTE(_u4HwInistId, _rPngChunkInfo.u4Type, _rPngChunkInfo.u4DataLength, &tPngPlte);//set plte value
            Printf("[PNG_EMU]Out Palette!\n");
        }
        else if (u4ChunkType == 0x74524E53)   //tRNS
        {
            //read trns
            INT32 i4Ret ;
            tPngTrns.u4ChunkType = _rPngChunkInfo.u4Type;
            tPngTrns.u4ChunkSize  = _rPngChunkInfo.u4DataLength;
            //tPngTrns.u4TrnsData = (UINT32)(_pbPngStrmIn + _u4PngImgDataAddr);
            x_memcpy((void *)(tPngTrns.u4TrnsData), (void *)(_pbPngStrmIn + _u4PngImgDataAddr), tPngTrns.u4ChunkSize);
            //due to cache problem, setting TS data in non-cache memory
            HalFlushInvalidateDCacheMultipleLine((tPngTrns.u4TrnsData),tPngTrns.u4ChunkSize);
            i4Ret = i4PngHalSettRNS(_u4HwInistId, &tPngTrns, _fgPngPalette);
            if (i4Ret == PNG_HAL_RTN_FAIL)
            {
                Printf( "[PNG_EMU]read TRNS error\n");
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
        Printf( "[PNG_EMU]No palette in PNG file\n");
        return FALSE;
    }
    //auto skip test
    if (fgPngAutoskipEnable)
    {
        _u4PngImgDataAddr = u4Png1stIDATOffset - 8;
        _u4PngIDATCount = 0;
        _rPngChunkInfo.u4DataLength = u4PngIDATEndOffset - _u4PngImgDataAddr;
    }

    Printf("[PNG_EMU]Png file idat data length = %d\n",_rPngChunkInfo.u4DataLength);
    //start decode
    if (_u4PngIDATCount == 0)
    {
        rPngHalIdatInfo.fg1stIDATChunk = TRUE;
    }
    else
    {
        rPngHalIdatInfo.fg1stIDATChunk = FALSE;
    }
    if (fgPngAutoskipEnable)
    {
        i4PngHalSetAutoSkipEnable(_u4HwInistId);

        if (_fgPngDecUseRingFifo)
            i4PngHalSetAutoSkipLastIdat(_u4HwInistId, FALSE);
        else
            i4PngHalSetAutoSkipLastIdat(_u4HwInistId, TRUE);
    }
    //set register
    vPngSetIDATDecReg(_u4HwInistId, _rPngChunkInfo.u4Type, TRUE, &rPngHalIdatInfo);

    //wait decode finish
    if (fgPngWaitDecFinish())
    {
        UINT32 u4RealReadSize = 0;
        INT32 i4RvdReturnVa = 0;
        UINT32 u4Tempval;

#if (PNG_MMU_TEST)
        vFlushPNGMMU();
#endif

        u4FileCnt ++;
        //check crc
        if (fgPngAutoskipEnable)
        {
            if ((u4PngHalGetAutoSkipCrc(_u4HwInistId)&0x1) == 1)
                Printf( "[PNG_EMU] AUTO SKIP CRC ERROR \n" );
        }
        else
        {
            if (u4PngHalReadCrc(_u4HwInistId) != _rPngChunkInfo.u4CRC )
            {
                Printf( "[PNG_EMU] CRC ERROR %x\n", u4PngHalReadCrc(_u4HwInistId) );
            }
        }

        if  (_fgPngDispOut)
        {
            Printf( "[PNG_EMU]PNG Decode Finish\n");
            x_thread_delay(100);
        }

        //set auto skip disable
        i4PngHalSetAutoSkipDisable(_u4HwInistId);
        //HalFlushInvalidateDCache();
        HalFlushInvalidateDCacheMultipleLine((UINT32)_pbPngStrmOut,_rPPngIHDRHeader.u4Width * _rPPngIHDRHeader.u4Height* _u4PngPixelByte );
        //u4PngCheckSumValue =  PngReadREG(0x4F5C);
        if (fgCompareChecksum)
        {
            UINT32 u4Checksum;

            if (_rPngClipPara.fgClipEn)
            {
                u4Checksum = _CalcPngCheckSum((void*)_pbPngStrmOut,
                                              _rPngClipPara.u4ClipHeight*_rPPngIHDRHeader.u4Width*_u4PngPixelByte);
                Printf( "[PNG_EMU]check sum value =  %x \n", u4Checksum);

                if (u4Checksum == (u4ResizeAndCropTimes?
                                   PngCheckSum[_u4CurPngFileListNum][5] :PngCheckSum[_u4CurPngFileListNum][4]))
                {
                    Printf("%d,Type %d Crop test:CheckSum Pass\n",_u4CurPngFileListNum,i4CurrentGoldenType);
                }
                else
                {
                    Printf("%d,Type %d Cropt test:CheckSum Fail!!!\n",_u4CurPngFileListNum,i4CurrentGoldenType);
                }
            }
            else
            {
                u4Checksum = _CalcPngCheckSum((void*)_pbPngStrmOut,
                                              _rPPngIHDRHeader.u4Height*_rPPngIHDRHeader.u4Width*_u4PngPixelByte);
                Printf( "[PNG_EMU]check sum value =  %x \n", u4Checksum);
                if (u4Checksum == PngCheckSum[_u4CurPngFileListNum][i4CurrentGoldenType])
                {
                    Printf("%d,Type %d:CheckSum Pass\n",_u4CurPngFileListNum,i4CurrentGoldenType);
                }
                else
                {
                    Printf( "[PNG_EMU]IO_READ32(IMAGE_BASE,0x72A8) =  %x \n", IO_READ32(IMAGE_BASE,0x72A8));
                    Printf( "[PNG_EMU]IO_READ32(IMAGE_BASE,0x72Ac) =  %x \n",  IO_READ32(IMAGE_BASE,0x72Ac));
                    Printf( "[PNG_EMU]IO_READ32(CKGEN_BASE,0x740) =  %x \n",  IO_READ32(CKGEN_BASE,0x740));
                    Printf("%d,Type %d:CheckSum Fail!!!\n",_u4CurPngFileListNum,i4CurrentGoldenType);
#ifdef PNG_SEMI_HOST
                    UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary  D:/png_test/dump/upload/argb8888_error.bin 0x%08x--0x%08x",(UINT32)(_pbPngStrmOut),(UINT32)(_pbPngStrmOut)+(_rPPngIHDRHeader.u4Height*_rPPngIHDRHeader.u4Width*_u4PngPixelByte)-1));
                    Printf("[PNG_EMU]: write Error decode result to file  D:/png_test/dump/upload/argb8888_error.bin\n");
#endif
                    for (u4Tempval = 0; u4Tempval < 32; u4Tempval++)
                    {
                        //x_thread_delay(5);
                        Printf("[PngC]  decoding Reg 0x%08x = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
                               (PNG_REG_BASE1 + (u4Tempval*16 + 0)),
                               PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 0)),
                               PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 4)),
                               PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 8)),
                               PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 12)));
                    }
                    _stopPngVerify = TRUE;
                    return FALSE;
                }
            }
        }
        else
        {
            //get golden file path
            vParsePngGoldenFile();
            //compare golden data
            if (fgLoadData)
            {
                HalFlushInvalidateDCacheMultipleLine((UINT32)(_pbPngStrmOut), (UINT32)(rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4));                
#ifdef PNG_SEMI_HOST
        #ifdef PNG_VFY_AUTO_TEST
                UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary D:/png_test/dump/upload/argb8888_%s_%d_%d_%d_%d_%d_%d.bin 0x%08x--0x%08x",\
                    ctt_fname, fgTransEn, fgTransOutEn, fgTransOrgAlphaEn, fgTrans16BitEn, fgResumeEnable, fgCompareChecksum,\
                    (UINT32)(_pbPngStrmOut),(UINT32)(_pbPngStrmOut)+(rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4)-1));
                Printf("[PNG_EMU]: write Normal decode result to file  D:/png_test/dump/upload/argb8888_%s_%d_%d_%d_%d_%d_%d.bin\n",\
                    ctt_fname, fgTransEn, fgTransOutEn, fgTransOrgAlphaEn, fgTrans16BitEn, fgResumeEnable, fgCompareChecksum);                        
        #else
                UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary D:/png_test/dump/upload/argb8888.bin 0x%08x--0x%08x",\
                    (UINT32)(_pbPngStrmOut),(UINT32)(_pbPngStrmOut)+(rPngHalPicInfo.ptIhdr->u4Height*rPngHalPicInfo.ptIhdr->u4Width*4)-1));
                Printf("[PNG_EMU]: write Normal decode result to file  D:/png_test/dump/upload/argb8888.bin\n");
        #endif
#endif
                for (u4Tempval = 0; u4Tempval < 32; u4Tempval++)
                {
                    //x_thread_delay(5);
                    Printf("[PngG1]  decoding Reg 0x%08x = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
                           (PNG_REG_BASE1 + (u4Tempval*16 + 0)),
                           PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 0)),
                           PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 4)),
                           PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 8)),
                           PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 12)));
                }
            }
            else
            {
                if (u4ResizeAndCropTimes == 0)
                {
                    i4RvdReturnVa = i4RvdReadFile((char*)_arbPngGoldenFile, (UINT32)_pbPngGoldenData, _u4PngGoldenSize);
                }
                if (i4RvdReturnVa >=0 )
                {
                    Printf( "[PNG_EMU]Read Golden file OK\n");
                    Printf( "[PNG_EMU]Compare file %s, %s\n", _abPngFileList[_u4CurPngFileListNum], _arbPngGoldenFile);
                    Printf( "[PNG_EMU]Size %d, %d\n", _rPPngIHDRHeader.u4Height, _rPPngIHDRHeader.u4Width);
                    HalFlushInvalidateDCache();
                    if (fgPngCompareGolden((UINT8*)_pbPngStrmOut, (UINT8*)_pbPngGoldenData, u4RealReadSize))
                    {
                        HalFlushInvalidateDCache();
                        Printf( "[PNG_EMU]Compare data correct %s, %s\n", _abPngFileList[_u4CurPngFileListNum], _arbPngGoldenFile);
                        return TRUE;
                    }
                    else
                    {
#ifdef PNG_SEMI_HOST
                        UNUSED(T32_HostExec(E_T32_EXEC, "d.save.binary  D:/png_test/dump/upload/argb8888_error.bin 0x%08x--0x%08x",(UINT32)(_pbPngStrmOut),(UINT32)(_pbPngStrmOut)+(_rPPngIHDRHeader.u4Height*_rPPngIHDRHeader.u4Width*_u4PngPixelByte)-1));
                        Printf("[PNG_EMU]: write Error decode result to file  D:/png_test/dump/upload/argb8888_error.bin\n");
#endif

                        for (u4Tempval = 0; u4Tempval < 32; u4Tempval++)
                        {
                            //x_thread_delay(5);
                            Printf("[PngG2]  decoding Reg 0x%08x = 0x%08x, = 0x%08x, = 0x%08x, = 0x%08x \n",
                                   (PNG_REG_BASE1 + (u4Tempval*16 + 0)),
                                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 0)),
                                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 4)),
                                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 8)),
                                   PngReadREG(PNG_REG_BASE1, (u4Tempval*16 + 12)));
                        }
                        _stopPngVerify = TRUE;                        
                        return FALSE;
                    }
                }
                else
                {
                    HalFlushInvalidateDCache();
                    Printf( "[PNG_EMU]Read Golden file %s  fail\n", _arbPngGoldenFile);
                }
            }
        }
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
void vInitPngFileList(void)
{
    UINT32 u4I;
    for (u4I=0; u4I<PngFileListMax; u4I++)
        _abPngFileList[u4I] = 0;
}

INT32 i4LoadPngFileList(void)
{
    return i4RvdReadFile("D:/png_test/pnglist.txt",(UINT32)_pbPngFileList,PngFileListMax*256);
}

// *********************************************************************
// Function : INT32 i4LoadPngFile(void)
// Description :
// Parameter : None
// Return    : None
// *********************************************************************

INT32 i4LoadPngFile(void)
{
#if 0 //(PNG_MMU_TEST)
    INT32 _i4PngFileSize;
    _i4PngFileSize = i4RvdReadFile((char *)_abPngFileList[_u4CurPngFileListNum],(UINT32)_pbPngStrmIn1,PNG_STM_IN_SIZE);

    if (_i4PngFileSize > 0)
    {
        HalFlushInvalidateDCacheMultipleLine((UINT32)_pbPngStrmIn,PNG_STM_IN_SIZE);
        x_memcpy((void *)_pbPngStrmIn,(void *)_pbPngStrmIn1, PNG_STM_IN_SIZE);
        HalFlushInvalidateDCacheMultipleLine((UINT32)_pbPngStrmIn,PNG_STM_IN_SIZE);
    }

    return _i4PngFileSize;

#else
    return i4RvdReadFile((char *)_abPngFileList[_u4CurPngFileListNum],(UINT32)_pbPngStrmIn,PNG_STM_IN_SIZE);
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
    while ( *(_pbPngFileList + u4I) != 0)
    {
        if (!fgInFileName)
        {
            _abPngFileList[_u4PngFileListNum] = (_pbPngFileList + u4I);
            _u4PngFileListNum++;
            fgInFileName = TRUE;
        }

        if (*(_pbPngFileList + u4I) == 0x0D)
            *(_pbPngFileList + u4I) = 0;

        if (*(_pbPngFileList + u4I) == 0x0A)
        {
            *(_pbPngFileList + u4I) = 0;
            fgInFileName = FALSE;
        }

        u4I++;
        if ((u4I == PngFileListMax*256) || (u4I == u4BufSizeLimit))
        {
            break;
        }

    }
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
    if ( ((_rPPngIHDRHeader.u4Width < 1920)&&(_rPPngIHDRHeader.u4Height < 1080))||(_rPPngIHDRHeader.u4Width > 1920) )
    {
        Printf("[Png][Verify] WH<1920*1080\n");
        return;
    }
    if ( (_rPPngIHDRHeader.u4Width > 400)&&(_rPPngIHDRHeader.u4Height > 400))
    {
        Printf("[Png][Verify] Real do crop test\n");
        fgResizeEnable = FALSE;
        if (_rPPngIHDRHeader.fgInterlace)//interlace
        {
            _u4PngHorScale = 0;
            _u4PngVrtScale = 0;
            _u4PngIntScale = 7;
        }
        else
        {
            _u4PngHorScale = 0;
            _u4PngVrtScale = 0;
        }
        _rPngClipPara.fgClipEn = TRUE;
        _rPngClipPara.u4ClipHeight = 160;
        _rPngClipPara.u4ClipWidth = 160;
        _rPngClipPara.u4ClipTop = 200;
        _rPngClipPara.u4ClipLeft = 200;
        if (fgPngDecode())
            Printf("[Png][Verify]Cropping test,top-left = 0x200. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
        else
            Printf("[Png][Verify]Cropping test,top-left = 0x200.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
        u4ResizeAndCropTimes++;
        u4ResizeAndCropTimesForWt++;
        _rPngClipPara.u4ClipHeight = 200;
        _rPngClipPara.u4ClipWidth = 200;
        _rPngClipPara.u4ClipTop = 160;
        _rPngClipPara.u4ClipLeft = 160;
        i4LoadPngFile();
        if (fgPngDecode())
            Printf("[Png][Verify]Cropping test,top-left = 0x160. Decode %s success! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
        else
            Printf("[Png][Verify]Cropping test,top-left = 0x160.  Decode %s fail! _u4PngIntScale = 0x%x,_u4PngHorScale = 0x%x,_u4PngVrtScale = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngIntScale,_u4PngHorScale,_u4PngVrtScale);
        u4ResizeAndCropTimes++;
        u4ResizeAndCropTimesForWt++;
    }

    //set resize val for default value
    _u4PngHorScale = 0;
    _u4PngVrtScale = 0;
    _u4PngIntScale = 7;
    u4ResizeAndCropTimes = 0;
    //set Cropping val for default value
    fgResizeEnable = FALSE;
    _rPngClipPara.fgClipEn = FALSE;

}

void vParsingFormat(void)
{
    //set color mode
    UINT32 u4TempVa = 0;
    _u4PngOutFormatSelect = 0x8;
    for (u4TempVa = 0; u4TempVa < 4; u4TempVa++)
    {
        if (_u4PngOutFormatSelect & 0x1)
        {
            _eOutClrMd = GFX_COLORMODE_ARGB_CLUT8;
            _fgOurClrPalette	= TRUE;
            _u4PngPixelByte = 1;
            i4CurrentGoldenType = 3;
        }
        if (_u4PngOutFormatSelect & 0x2)
        {
            _eOutClrMd = GFX_COLORMODE_ARGB_D1555;
            _fgOurClrPalette	= FALSE;
            _u4PngPixelByte = 2;
            i4CurrentGoldenType = 2;
        }
        if (_u4PngOutFormatSelect & 0x4)
        {
            _eOutClrMd = GFX_COLORMODE_ARGB_D4444;
            _fgOurClrPalette	= FALSE;
            _u4PngPixelByte = 2;
            i4CurrentGoldenType = 1;
        }
        if (_u4PngOutFormatSelect & 0x8)
        {
            _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
            _fgOurClrPalette	= FALSE;
            _u4PngPixelByte = 4;
            i4CurrentGoldenType = 0;
        }
        i4LoadPngFile();
        if (fgPngDecode())
        {
            Printf("[Png][Verify]Output Color Format test. Decode %s success! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
        }
        else
        {
            if ((!_fgPngPalette) && (_fgOurClrPalette))
            {
                Printf("[Png][Verify]Output Color Format test. Decode %s Not Support! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
            }
            else
            {
                Printf("[Png][Verify]Output Color Format test. Decode %s fail! ColorFormat = 0x%x \n",_abPngFileList[_u4CurPngFileListNum],_u4PngOutFormatSelect);
            }
        }
        _u4PngOutFormatSelect = _u4PngOutFormatSelect >> 1;
    }
    _u4PngOutFormatSelect = 0x8;
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
    _fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
    i4CurrentGoldenType = 0;
}

// *********************************************************************
// Function : void vPngVerify(const CHAR** aszArgv)
// Description :
// Parameter : None
// Return    : None
// *********************************************************************
//[CJYANG] imgdec_vfy, need to initial memory allocate


void vPngVerify(const CHAR** aszArgv)
{
    UINT32 u4TempValue = 0;
    INT32 i4FileSize;

    _rPngClipPara.fgClipEn = FALSE;
    _rPngClipPara.u4ClipHeight = 200;
    _rPngClipPara.u4ClipWidth = 200;
    _rPngClipPara.u4ClipTop = 800;
    _rPngClipPara.u4ClipLeft = 800;

    //load png file
    vInitPngFileList();
    i4FileSize = i4LoadPngFileList();
    vParsePngFileList(i4FileSize);

    Printf("[Png][Verify]_u4PngFileListNum = %d\n",_u4PngFileListNum);
    _eOutClrMd = GFX_COLORMODE_ARGB_D8888;
    _fgOurClrPalette	= FALSE;
    _u4PngPixelByte = 4;
    i4CurrentGoldenType = 0;
    //do PNG verify
    u4TempValue = 0;
    fgPngAutoskipEnable = TRUE;
    fgResumeEnable = FALSE;

    for (u4TempValue = 0; u4TempValue < 1; u4TempValue++)
    {
        if (fgPngAutoskipEnable)
            Printf( "[Png][Verify]Begin png auto skip mode decoding !!!!!!!! \n \n \n \n ");

        for (_u4CurPngFileListNum=0; (_stopPngVerify==FALSE) && (_u4CurPngFileListNum<_u4PngFileListNum); _u4CurPngFileListNum++)
        {
            INT32 _i4PngFileSize ;
            BOOL fgDecodeSuccess = FALSE;
            u4FileCnt = 0;

            Printf("Next file _u4CurPngFileListNum = %d\n",_u4CurPngFileListNum);
            _i4PngFileSize = i4LoadPngFile();
            fgWtEnable = FALSE;

            if (_i4PngFileSize < 0)
            {
                Printf("[Png][Verify] Load File fail\n");
            }
            else
            {
                //normal mode test, no resume & resize
                if (_PngVerifyCmdGetParam(6)) 
                {
                    fgResumeEnable = FALSE;
                    u4ResizeAndCropTimes = 0;
                    Printf("\n");
                    Printf("[Png][Verify] Decode Png by HW - normal mode\n");
                    fgDecodeSuccess = fgPngDecode();
                    if (fgDecodeSuccess)
                        Printf("[Png][Verify]Normal mode Decode %s success,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
                    else
                        Printf("[Png][Verify]Normal mode Decode %s fail,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
                }

                Printf("\n");
                //resume mode test, no resume & resize
                if (_PngVerifyCmdGetParam(7))
                {
                    fgResumeEnable = TRUE;
                    Printf("[Png][Verify] Decode Png by HW - SRC&DST resume mode\n");
                    fgDecodeSuccess = fgPngDecode();
                    if (fgDecodeSuccess)
                        Printf("[Png][Verify]SRC&DST resume mode Decode %s success,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
                    else
                        Printf("[Png][Verify]SRC&DST resume mode Decode %s fail,decoding mode = %d \n",_abPngFileList[_u4CurPngFileListNum],fgPngAutoskipEnable);
                    fgResumeEnable = FALSE;

                }

                Printf("\n");
                if (_PngVerifyCmdGetParam(8))
                {
                    fgResumeEnable = TRUE;
                    //resuem and output color format test
                    Printf("[Png][Verify] Decode Png by HW - resume and output color format test\n");
                    vParsingFormat();
                    fgResumeEnable = FALSE;

                }

                Printf("\n");
                if (_PngVerifyCmdGetParam(9))
                {
                    fgResumeEnable = TRUE;
                    //fgCompareChecksum = FALSE;
                    //crop and scale mode test, no resume & resize
                    Printf("[Png][Verify] Decode Png by HW - Resize&Scale mode\n");
                    vResizeAndCrop();
                    Printf("[Png][Verify] Decode Png by HW - Resize&Scale mode Done!!!\n");
                    fgResumeEnable = FALSE;
                    //fgCompareChecksum = TRUE;
                }
            }
        }

        Printf("[Png][Verify] Verify auto test all Done!!!!!!!!\n");
        fgPngAutoskipEnable = FALSE;
    }
}

void vPngTransAlphaTest(void)
{
    BOOL fgDecodeSuccess;

    fgPngAutoskipEnable = TRUE;
    fgLoadData = TRUE;
    if (fgCompareChecksum)
    {
        return;
    }
    fgDecodeSuccess = fgPngDecode();
    if (fgDecodeSuccess)
        Printf("[Png][Verify]TransAlpha  mode Decode  success\n");
    else
        Printf("[Png][Verify]TransAlpha  mode Decode  fail \n");
    fgLoadData = FALSE;
    return;
}

#define ALIGN256(addr) ((addr+0xFF)&(0xffffff00))

void PngAllocateMemory(UINT32 u4FbmId1,UINT32 u4FbmId2,UINT32 u4SrcBuf)
{
    FBM_POOL_T*prFbmPool=NULL;
#ifdef PNG_VFY_AUTO_TEST

    UINT32 u4PngStrmIn;
#if (PNG_MMU_TEST)
//    UINT32 u4PngStrmIn1;
#endif
    UINT32 u4PngStrmOut;
    UINT32 u4PngWbHdrMem;
    UINT32 u4PngGoldenData;
    UINT32 u4PngPngRingFifo;
    UINT32 u4PngClrBuf[2];
    Printf("Here PngAllocateMemory for PNG_VFY_AUTO_TEST\n");
    //BSP_AllocAlignedDmaMemory x_alloc_aligned_nc_mem
#if (PNG_MMU_TEST)
    u4PngStrmOut    = (UINT32)GGT_get_vmem(PNG_STM_OUT_SIZE);
    u4PngStrmIn     = (UINT32)GGT_get_vmem(PNG_STM_IN_SIZE);
#else
    u4PngStrmIn     = (UINT32)BSP_AllocAlignedDmaMemory(PNG_STM_IN_SIZE, 256);
    u4PngStrmOut    = (UINT32)BSP_AllocAlignedDmaMemory(PNG_STM_OUT_SIZE, 256);
#endif
    u4PngWbHdrMem   = (UINT32)BSP_AllocAlignedDmaMemory(32, 256);
#if (PNG_MMU_TEST)
    u4PngClrBuf[0]   = (UINT32)GGT_get_vmem(PNG_COLOR_BUF_SIZE);
    u4PngClrBuf[1]   = (UINT32)GGT_get_vmem(PNG_COLOR_BUF_SIZE);
    u4PngPngRingFifo = (UINT32)GGT_get_vmem(PNG_RING_FIFO_SIZE);
#else
    u4PngClrBuf[0]   = (UINT32)BSP_AllocAlignedDmaMemory(PNG_COLOR_BUF_SIZE, 256);
    u4PngClrBuf[1]   = (UINT32)BSP_AllocAlignedDmaMemory(PNG_COLOR_BUF_SIZE, 256);
    u4PngPngRingFifo = (UINT32)BSP_AllocAlignedDmaMemory(PNG_RING_FIFO_SIZE, 256);
#endif
    _pu1PngPlt = (UINT8 *)BSP_AllocAlignedDmaMemory(256*32, 16);
#if (PNG_MMU_TEST)
    tPngTrns.u4TrnsData = (UINT32)GGT_get_vmem(256*3);
    tPngPlte.u4PlteData = (UINT32)GGT_get_vmem(256*3);
#else
    tPngTrns.u4TrnsData = (UINT32)BSP_AllocAlignedDmaMemory(256*3, 128);
    tPngPlte.u4PlteData = (UINT32)BSP_AllocAlignedDmaMemory(256*3, 128);
#endif
    _pbPngFileList = (UINT8 *)BSP_AllocAlignedDmaMemory(PngFileListMax*512, 16);
#if (PNG_MMU_TEST)
//    u4PngStrmIn1    = (UINT32)GGT_get_vmem(PNG_STM_IN_SIZE);
#endif
    //u4PngGoldenData = (UINT32)x_alloc_aligned_nc_mem(PNG_GOLDEN_DATA, 256);
    // Can't allocate another 64M Memory, use fbm memory instead.
    if (u4FbmId2>FBM_POOL_TYPE_TOTAL)
    {
        u4FbmId2=ALIGN256(u4FbmId2);
        u4PngGoldenData = u4FbmId2;
    }
    else
    {
        prFbmPool=NULL;
        prFbmPool=FBM_GetPoolInfo((UINT8)u4FbmId2);

        if (prFbmPool==NULL || prFbmPool->u4Addr==NULL || prFbmPool->u4Size==0)
        {
            LOG(0,"Get Goden Data Buffer fail\n");
            return;
        }
        u4PngGoldenData =(ALIGN256(prFbmPool->u4Addr));
    }

    _pbPngStrmIn        = (UINT8*)u4PngStrmIn;
#if (PNG_MMU_TEST)
//    _pbPngStrmIn1       = (UINT8*)u4PngStrmIn1;
#endif
    _pbPngStrmOut       = (UINT8*)u4PngStrmOut;
    _pu4PngWbHdrMem     = (UINT8*)u4PngWbHdrMem;
    _pbPngGoldenData    = (UINT8*)u4PngGoldenData;
    _pbPngClrBuf[0]     = (UINT8*)(u4PngClrBuf[0]);
    _pbPngClrBuf[1]     = (UINT8*)(u4PngClrBuf[1]);
    _pbPngRingFifo      = (UINT8*)u4PngPngRingFifo;

    Printf("[Png][Verify]_pbPngStrmOut = %x!\n",(UINT32)_pbPngStrmOut);
    Printf("[Png][Verify]_pbPngStrmIn = %x!\n",(UINT32)_pbPngStrmIn);
    Printf("[Png][Verify]_pbPngGoldenData = %x!\n",(UINT32)_pbPngGoldenData);
    //Printf("u4PngStrmIn1 = %x!\n",u4PngStrmIn1);
    Printf("[Png][Verify]_pbPngClrBuf[0] = %x!\n",(UINT32)_pbPngClrBuf[0]);
    Printf("[Png][Verify]_pbPngClrBuf[1] = %x!\n",(UINT32)_pbPngClrBuf[1]);
    Printf("[Png][Verify]_pbPngRingFifo = %x!\n",(UINT32)_pbPngRingFifo);
    Printf("[Png][Verify]tPngTrns.u4TrnsData = %x!\n",tPngTrns.u4TrnsData);
    Printf("[Png][Verify]tPngPlte.u4PlteData = %x!\n",tPngPlte.u4PlteData);
    i4PngHalInitHwInst();
#else

    _pbPngStrmIn = (UINT8 *)u4SrcBuf;
    Printf("Here PngAllocateMemory for PNG_DRV_FLOW_TEST\n");

    if (u4FbmId1 > FBM_POOL_TYPE_TOTAL)
    {
        u4FbmId1=ALIGN256(u4FbmId1);
        _pbPngStrmOut = (UINT8 *)u4FbmId1;
    }
    else
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)u4FbmId1);
        if (prFbmPool==NULL || prFbmPool->u4Addr==NULL || prFbmPool->u4Size==0)
        {
            LOG(0,"Get Target Buffer fail\n");
            return;
        }
        _pbPngStrmOut = (UINT8 *)(ALIGN256(prFbmPool->u4Addr));
    }

    if (u4FbmId2>FBM_POOL_TYPE_TOTAL)
    {
        u4FbmId2=ALIGN256(u4FbmId2);
        _pbPngGoldenData = (UINT8 *)u4FbmId2;
    }
    else
    {
        prFbmPool=NULL;
        prFbmPool=FBM_GetPoolInfo((UINT8)u4FbmId2);

        if (prFbmPool==NULL || prFbmPool->u4Addr==NULL || prFbmPool->u4Size==0)
        {
            LOG(0,"Get Goden Data Buffer fail\n");
            return;
        }
        _pbPngGoldenData = (UINT8 *)(ALIGN256(prFbmPool->u4Addr));
    }
    _pbPngFileList = (UINT8 *)BSP_AllocAlignedDmaMemory(PngFileListMax*512, 16);

    Printf("[Png][Verify]_pbPngStrmOut = %x!\n",(UINT32)_pbPngStrmOut);
    Printf("[Png][Verify]_pbPngStrmIn = %x!\n",(UINT32)_pbPngStrmIn);
    Printf("[Png][Verify]_pbPngGoldenData = %x!\n",(UINT32)_pbPngGoldenData);
    Printf("[Png][Verify]_pbPngFileList = %x!\n",(UINT32)_pbPngFileList);

#endif
}

UINT32 _CalcPngCheckSum(void* pvData, UINT32 u4ByteLen)
{
    UINT32* pu4Data;
    UINT32 u4Sum = 0;

    ASSERT(pvData!=NULL);
    pu4Data = (UINT32*)pvData;
    while (u4ByteLen > 3)
    {
        u4Sum += *pu4Data;
        if (u4ByteLen >= 4)
        {
            u4ByteLen -= 4;
        }
        else
        {
            u4ByteLen = 0;
        }
        pu4Data++;
    }

    return u4Sum;
}


#endif

