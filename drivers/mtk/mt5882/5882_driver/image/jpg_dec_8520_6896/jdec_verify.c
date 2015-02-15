/*! \file hal_vdec_mpeg_if.h
*
*  \par LEGAL DISCLAIMER
*
* (Header of MediaTek Software/Firmware Release or Documentation)
*
* BY OPENING OR USING THIS FILE, USER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* ARE PROVIDED TO USER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS
* ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED
* IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND USER AGREES TO LOOK ONLY TO
* SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL
* ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO USER'S
* SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* USER HEREBY ACKNOWLEDGES THE CONFIDENTIALITY OF MEDIATEK SOFTWARE AND AGREES
* NOT TO DISCLOSE OR PERMIT DISCLOSURE OF ANY MEDIATEK SOFTWARE TO ANY THIRD
* PARTY OR TO ANY OTHER PERSON, EXCEPT TO DIRECTORS, OFFICERS, EMPLOYEES OF
* USER WHO ARE REQUIRED TO HAVE THE INFORMATION TO CARRY OUT THE PURPOSE OF
* OPENING OR USING THIS FILE.
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

//#include <string.h>
#include "x_hal_5381.h"
#include "x_hal_926.h"
#include "x_typedef.h"
#include "x_stl_lib.h"

#include "jpg_if.h"
#include "jdec_hal_if.h"
#include "jdec_hal.h"
#include "jdec_drv.h"
#include "img_lib_if.h"
#include "imgrz_if.h"
#include "../jpg_6896/jpg_debug.h"
#define DMXCMD_READ32(offset)			IO_READ32(DEMUX0_BASE, ((offset) * 4))

#ifdef JPEG_AUTO_TEST
extern INT32 _JpgVerifyCmdGetParam(INT32 i4ParamType, INT32 Param);
#define IMGRZ_TAY_ADDR (_JpgVerifyCmdGetParam(6, 0))
#define IMGRZ_TAC_ADDR (_JpgVerifyCmdGetParam(7, 0))
#else
#define IMGRZ_TAY_ADDR (0x1F00800)
#define IMGRZ_TAC_ADDR (0x1FB0000)
#endif

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
                                              UINT32 u4HSampleFactor2,UINT32 u4VSampleFactor2
);
extern INT32 i4ImgReszVfyJpegPicModeGetResizeStatus(UINT32 u4HwId);
extern void vImgReszVfyJpegPicModeUninit(UINT32 u4HwId);
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
extern INT32 i4ImgReszVfyJpegPreloadGetResizeStatus(UINT32 u4HwId);
extern void vImgReszVfyJpegPreloadUninit(UINT32 u4HwId);

extern void vImgReszVfyBufDeBlock(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufWidth,UINT32 u4BufHeight,
                           UINT32 u4BlockWidth,UINT32 u4BlockHeight);
extern void vImgReszVfyBufDeAddrSwap(UINT32 u4SrcBufSA,UINT32 u4DestBufSA,UINT32 u4BufLen);
extern INT32 i4JDEC_HAL_Pre_Init(UINT32 u4JDecID);

//for verification
static UINT32 _u4ImageWidth_RZ;
static UINT32 _u4ImageHeight_RZ;
static UINT32 _u4ImagePitch_RZ;

#ifdef JPEG_AUTO_TEST
#ifndef CC_EMULATION
#define CC_EMULATION
#endif
typedef struct
{
    UINT32 u4ByteSize;
    CHAR *szFileName;
	UINT32 u4rCheckSum[4];
}JPGCLI_PROG_SET_T;
#define T32_TEST
static INT32 i4CurDecIndex=0;
extern INT32 _JpgVerifyCmdSetParam(INT32 i4ParamType,INT32 Param);
#define LIST_MAX 14
static JPGCLI_PROG_SET_T _arProgressive[] =
{
	{8317, "small.jpg",{0x326fcb12,0xad8ef61b}},
	{181053, "beach.jpg",{0x75b3b9af,0x5644953d}},
	{399917, "girl1.jpg",{0x72bf761a,0xe032711c}},
	{90567, "girl2.jpg",{0xbcacd7d0,0xe91a9f43}},
	{5655, "rose.jpg",{0xc52ec1e9,0xabaf8499}},
	{19456, "sun.jpg",{0x60ef6bc5,0x7959c863}},
	{522, "yellow.jpg",{0xb8b8b880,0x50095000}},
	{42150, "pkoala.jpg",{0x2e571145,0xffb942b9}},
	{106222, "cuore.jpg",{0xbf726edd,0x97184c55}},
	{111856, "catfrog.jpg",{0x60b93e5d,0xbb23d521}},
	{51409, "autumn.jpg",{0xdaccf71b,0x776f9cfc}},
	{247300, "Progressive.jpg",{0x9d35902f,0xeee880e9}},
	{1231, "small_kitty.jpg",{0x5744eb5f,0xfca9ba}},	//jpg.de 12
	{409250, "000430-1210-34-progressive.jpg",{0x372db4ed,0xcd04e87d}},
	{211178,"waterwalk.jpg",{0x989b4fc6,0x3d6b5c74}},
	{33415,"dawoo.jpg",{0xfa779bcf,0xb2d6ab71}},
	{11029,"liang.jpg",{0xea0a7e5a,0x1061fbd2}},
	{82042,"joeunsw.jpg",{0xa3d52d4b,0xfad6cfe}},
	{247603,"girl3.jpg",{0x1fecbff3,0xbaf111bc}},
	{114162,"skyheart.jpg",{0x24ced793,0xebfbf54f}},
	{283983,"lgp2000.jpg",{0x807ec991,0x1e458793}},
	{766299,"girl4.jpg",{0xc2271eb7,0xa498ebe}},
	{26267,"italy.jpg",{0x3e1c9887,0xa204d017}},
	{347325,"hiteker.jpg",{0xb33d03d,0xa59469c1}},
	{303223, "prog_small_fox_0.jpg",{0x4579c3d3,0xb95473b0}},	//jpg.de 24
	{260167, "prog_small_fox_1.jpg",{0x38f23a34,0xf696cce9}},	
	{2573879, "Big_progressive_420.jpg",{0x2ec0e860,0x3c326f9a}},  //jpg.de 26
	{2837260, "Big_progressive_422.jpg",{0xe9446455,0xda13169e}},	
	{3207044, "Big_progressive_444.jpg",{0x2d0ed7d0,0xcbde14a7}},	
	{0, NULL,{0,0,0,0}}
};
#endif
//#define JDEC_COLOR_BUF_WIDTH   3264
//#define JDEC_COLOR_BUF_HEIGHT  2448

#ifdef PHOTO_EMP_DDI_CONFIG
	#define JDEC_COLOR_BUF_WIDTH   1920
	#define JDEC_COLOR_BUF_HEIGHT  1080
	//#define JDEC_FIFO_SIZE (/*4**/1024*1024)
	#define JDEC_COLLECT_LIMIT  (1280 * 1024)//(1920 * 1440)//(1920 * 1080)
#else
	#define JDEC_COLOR_BUF_WIDTH   1280
	#define JDEC_COLOR_BUF_HEIGHT  1024
	//#define JDEC_FIFO_SIZE (/*4**/1024*1024)
	#define JDEC_COLLECT_LIMIT  (1024 * 1024)
#endif

#define JDEC_VERIFY_LIMIT  (1024*768)
#define JDEC_FULLNESS   (400*1024)

static BYTE *_EmuJpgFifo=NULL;
static INT32 _i4JpegFileSize = 0;
static JPEG_JFIF_HEADER_T *_prJPEG_JFIF_HEADER = NULL;
static UINT32 _u4ErrMCUNum = 0;


#define JpegFileListMax 64

static UINT32 _u4ColorBufY;
static UINT32 _u4ColorBufCb;
static UINT32 _u4ColorBufCr;
static UINT32 _u4ColorBufY2;
static UINT32 _u4ColorBufCb2;
static UINT32 _u4ColorBufCr2;
//static UINT32 _u4FrameBufY;
//static UINT32 _u4FrameBufC;
//static UINT32 _u4EnhanceBufY;
//static UINT32 _u4EnhanceBufCb;
//static UINT32 _u4EnhanceBufCr;

static UINT32 _u4ColorBufWidth;
static UINT32 _u4ImageWidth;
static UINT32 _u4ImageHeight;

static UINT32 _u4XSampleY,_u4YSampleY;
static UINT32 _u4XSampleCb,_u4YSampleCb;
static UINT32 _u4XSampleCr,_u4YSampleCr;

static UINT32 _u4DiffCnt;

static BOOL _fgStop = FALSE;
static UINT32 _u4CurScan;
//#define JDEC_VFY_TEST_PROGRESSIVE_PRELOAD 1
//static BOOL _fgReinitTest = TRUE;

static BOOL _fgVerifyFlow;

VOID vJpegSetProgressiveStop(void)
{
    _fgStop = TRUE;
}

UINT32 u4GetProgressiveCurScan(void)
{
   return _u4CurScan;
}
INT32 vJpegDecode(UINT32 u4JDecID,JDEC_HAL_DEC_MODE_T eDecodingMode,UINT32 u4ColorBufY,UINT32 u4ColorBufCb,UINT32 u4ColorBufCr,
                                                                    UINT32 u4ColorBufY2,UINT32 u4ColorBufCb2,UINT32 u4ColorBufCr2)
                                                                   //UINT32 u4FrameBufY,UINT32 u4FrameBufC,
                                                                   //UINT32 u4FrameBufWidth,UINT32 u4FrameBufHeight)
{
    JPEG_JFIF_HEADER_T *prJPEG_JFIF_HEADER = _prJPEG_JFIF_HEADER;
    JPEGHANDLE hParser;
    JDEC_HAL_PIC_INFO_T rPicInfo = { 0 };
    JDEC_HAL_INPUT_INFO_T rInputInfo = { 0 };
    JDEC_HAL_OUTPUT_INFO_T rOutputInfo = { 0 };
    UINT32 au4MCUWidth[3] = {0},au4MCUHeight[3] = {0};
    UINT32 u4CombMCUWidth,u4CombMCUHeight;
    UINT32 u4MCUWidth,u4MCUHeight;
    UINT32 u4MCUx,u4MCUy;
    UINT32 u4YBufSa,u4CbBufSa,u4CrBufSa,u4PitchPerRow;
    UINT32 u4WaitCnt = 0;
    UINT32 u4MCUCnt;
    UINT32 u4ReadAddrBytes,u4ReadAddrBits;
    UINT32 u4ScanCnt;
    JDEC_HAL_DEC_PROG_T eProgMode;
    INT32 i4Ret = E_IMG_OK;
    UINT32 u4TotalComp = 1;
    BOOL fgPreload = FALSE;
    BOOL LastScan=FALSE;    
    BOOL fgLastScan[3] = {FALSE};
    UINT32 u4NewRP, u4UsedLen;

	UINT32	au4RowWidth[3];		
	UINT32    au4RowHeight[3];	
	#ifdef JPG_PROGRASSIVE_V4
	UINT32 u4Temp;
	#endif

    i4JDEC_HAL_Init(u4JDecID);
    i4JDEC_HAL_Set_Decoding_Mode(u4JDecID,eDecodingMode);
    rPicInfo.prSofInfo = (JDEC_JFIF_SOF_T *)&(_prJPEG_JFIF_HEADER->rSOF);
    rPicInfo.prHuffTblInfo = (JDEC_JFIF_DHT_T *)&(_prJPEG_JFIF_HEADER->rDHT);
    rPicInfo.prQTblInfo = (JDEC_JFIF_DQT_T *)&(_prJPEG_JFIF_HEADER->rDQT);
    rPicInfo.u4RestartInterval = _prJPEG_JFIF_HEADER->u2RestartInterval;
    i4JDEC_HAL_Set_Pic_Info(u4JDecID,&rPicInfo);
    x_memcpy(&(_prJPEG_JFIF_HEADER->rSOS), &(_prJPEG_JFIF_HEADER->arSOS[0]), 
        sizeof(JPEG_JFIF_SOS_T));
    i4JDEC_HAL_Set_Scan_Info(u4JDecID,&(_prJPEG_JFIF_HEADER->rSOS));
    rInputInfo.u4InFifoSa = (UINT32)_EmuJpgFifo;
#if 1
    rInputInfo.u4InFifoSz = JDEC_FIFO_SIZE;
#else
    rInputInfo.u4InFifoSz = _prJPEG_JFIF_HEADER->u4ImageByteSize;
#endif
    rInputInfo.u4BitstreamSa = (UINT32)(_prJPEG_JFIF_HEADER->pu1CurrRead);

    i4JDEC_HAL_Set_Input_Info(u4JDecID,&rInputInfo);

    {
        UINT32 u4WidthPerMCU = (UINT32)(rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8);
        u4PitchPerRow = ((rPicInfo.prSofInfo->u2ImageWidth +
                                   u4WidthPerMCU - 1) /
                                   u4WidthPerMCU) * u4WidthPerMCU; // Align to u4WidthPerMCU
        u4PitchPerRow = ((u4PitchPerRow + 15) / 16) * 16; // Align to 16's
        _u4ColorBufWidth = u4PitchPerRow;
    }
	
    rOutputInfo.u4OutYBufPitch = u4PitchPerRow;
#ifdef JPG_PROGRASSIVE_V4
    if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
        || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
        || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)
    {
        if(rPicInfo.prSofInfo->u1NumComponents > 1)
        {
            u4Temp = rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor;
            rOutputInfo.u4OutCbBufPitch = (((u4PitchPerRow /u4Temp) + 15) /16) * 16;
            u4Temp = rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor;
            rOutputInfo.u4OutCrBufPitch = (((u4PitchPerRow /u4Temp) + 15) /16) * 16;
        }
        else
        {
            rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
            rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
        }
    }
    else
    {
    	rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
    	rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
    }
#else
    rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
    rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
#endif

    if (_fgVerifyFlow)
    {
        eProgMode = JDEC_HAL_DEC_PROG_VERIFY;
    }
    else if (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
    {
        eProgMode = JDEC_HAL_DEC_MULTI_COLLECT;
    }
    else
    {
        eProgMode = JDEC_HAL_DEC_ENHANCE;
    }


    {
        UINT32 u4CompId;

        if (rPicInfo.prSofInfo->u1NumComponents > (UINT8)3)
        {
            rPicInfo.prSofInfo->u1NumComponents = (UINT8)3;
        }
        for(u4CompId = 0; u4CompId<rPicInfo.prSofInfo->u1NumComponents; u4CompId++)
        {
            UINT32 u4WidthPerMCU = (UINT32)((rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor /
                                    rPicInfo.prSofInfo->arSofComp[u4CompId].u1HSampFactor) * 8);
            UINT32 u4HeightPerMCU = (UINT32)((rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor /
                                     rPicInfo.prSofInfo->arSofComp[u4CompId].u1VSampFactor) * 8);

            au4MCUWidth[u4CompId] = (rPicInfo.prSofInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU;
            au4MCUHeight[u4CompId] = (rPicInfo.prSofInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU;
            
            //for JDEC_RzScanMode_Vfy()
            au4RowWidth[u4CompId] = ((au4MCUWidth[u4CompId]+rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor -1)/rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor)*rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor*8;
            au4RowHeight[u4CompId] = rPicInfo.prSofInfo->arSofComp[u4CompId].u1VSampFactor*8;
        }

        u4CombMCUWidth = (UINT32)((rPicInfo.prSofInfo->u2ImageWidth + rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8 - 1) /
                         (rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8));
        u4CombMCUHeight = (UINT32)((rPicInfo.prSofInfo->u2ImageHeight + rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8 - 1) /
                          (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8));
    }
    if (rPicInfo.prSofInfo->u1NumComponents != 1)
    {
        u4TotalComp = (UINT32)rPicInfo.prSofInfo->u1NumComponents;
    }

    /*if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
    {
#ifdef JDEC_VFY_TEST_PROGRESSIVE_PRELOAD
        x_memset(u4FrameBufY,0x80,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset(u4FrameBufC,0x80,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
#else
        x_memset((void *)_u4EnhanceBufY,0x80,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void *)_u4EnhanceBufCb,0x80,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void *)_u4EnhanceBufCr,0x80,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
#endif
    }*/

    u4ScanCnt = 0;
    do
    {
        rOutputInfo.u4OutYBufSa = u4ColorBufY;
        rOutputInfo.u4OutCbBufSa = u4ColorBufCb;
        rOutputInfo.u4OutCrBufSa = u4ColorBufCr;
        i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);
        if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
        {
            UINT32 u4CompId = _prJPEG_JFIF_HEADER->rSOS.au1CompId[0];
            if (u4CompId >= (UINT32)3)
            {
                 u4CompId = (UINT32)2;
            }
            u4MCUWidth = au4MCUWidth[u4CompId];
            u4MCUHeight = au4MCUHeight[u4CompId];
        } else
        {
            u4MCUWidth = u4CombMCUWidth;
            u4MCUHeight = u4CombMCUHeight;
        }

        //Printf("[Jpeg][Verify] MCU (%d,%d)",u4MCUWidth,u4MCUHeight);

        u4MCUCnt = 0;
        for(u4MCUy=0; u4MCUy<u4MCUHeight; u4MCUy++)
        {
            u4YBufSa = u4ColorBufY;
            u4CbBufSa = u4ColorBufCb;
            u4CrBufSa = u4ColorBufCr;

            if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
            {
                 #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                 UINT32 u4OutBufMapAddr = 0;
                 u4OutBufMapAddr = (UINT32)BSP_MapReservedMem((void *)rOutputInfo.u4OutYBufSa, rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3 * 3);
                 LOG(9, "mcu enhance dynamic map %s %d u4OutBufMapAddr:0x%x\n", __FUNCTION__,__LINE__, u4OutBufMapAddr);
                 if(!u4OutBufMapAddr)
                 {
                    LOG(0, "outbut buffer dynamic map fail\n");
                    ASSERT(0);
                 }
                 #endif
                //clear color buffer 
                x_memset((void *)VIRTUAL(rOutputInfo.u4OutYBufSa), 0x80, (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);
                x_memset((void *)VIRTUAL(rOutputInfo.u4OutCbBufSa), 0x80, (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);
                x_memset((void *)VIRTUAL(rOutputInfo.u4OutCrBufSa), 0x80, (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);
                #if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
                    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(rOutputInfo.u4OutYBufSa), (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);                
                    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(rOutputInfo.u4OutCbBufSa), (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);  
                    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(rOutputInfo.u4OutCrBufSa), (rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor << 3)
                                                                * rPicInfo.prSofInfo->u2ImageWidth);  
                #endif      
                
                #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
                if(u4OutBufMapAddr)
                {
                    LOG(9, "unmap %s u4OutBufMapAddr:0x%x\n",__FUNCTION__, u4OutBufMapAddr);
                    BSP_UnMapReservedMem((void *)u4OutBufMapAddr);
                }
                #endif
            }

            //Printf("[Jpeg][Verify] MCU Row %d",u4MCUy);

            if((eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU) ||
               (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) ||
               (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT))
            {                
                
                for(u4MCUx=0; u4MCUx<u4MCUWidth; u4MCUx++)
                {                    
                    i4JDEC_HAL_Decode(u4JDecID, eProgMode);

                    for(u4WaitCnt=0; u4WaitCnt<0x10000; u4WaitCnt++)
                    {
                        if(i4JDEC_HAL_Get_Decode_Status(u4JDecID) >= 0)
                            break;
                    }

                    if(u4WaitCnt==0x10000)
                    {
                        _u4ErrMCUNum = u4MCUCnt;
                        i4Ret = E_IMG_DEC_FAIL;
                        break;
                    }

                    u4MCUCnt++;

                    if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                    {
                        u4YBufSa += 8;
                        u4CbBufSa += 8;
                        u4CrBufSa += 8;
                    } 
                    else
                    {
                        u4YBufSa += (rPicInfo.prSofInfo->arSofComp[0].u1HSampFactor * 8);
                        u4CbBufSa += (rPicInfo.prSofInfo->arSofComp[1].u1HSampFactor * 8);
                        u4CrBufSa += (rPicInfo.prSofInfo->arSofComp[2].u1HSampFactor * 8);
                    }
                }               
            
                if(u4WaitCnt==0x10000)
                {
                    break;
                }

                if (!_fgVerifyFlow)                     //BC adds for driver
                {
                     if (((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0) && 
                        (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)) ||
                        (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)) //enhance or last scan of collection 
                     {
                        UINT32 u4ColorCmp;

                        if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                        {
						    UINT32 u4CompId = _prJPEG_JFIF_HEADER->rSOS.au1CompId[0];
							u4CompId=u4CompId>=3 ? 2 : u4CompId;
							
                            switch(u4CompId)
                            {
                            case 0:
                                //If first scan only has Y, it's gray level and need CbCr padding.
                                u4ColorCmp = (u4ScanCnt) ? (UINT32)0x4 : (UINT32)0xF;
                                if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                                {
                                    fgLastScan[0] = TRUE;
                                }
                                break;
                            case 1:
                                u4ColorCmp = 0x2;
                                if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                                {
                                    fgLastScan[1] = TRUE;
                                }
                                break;
                            case 2:
                            default:    
                                u4ColorCmp = 0x1;
                                if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                                {
                                    fgLastScan[2] = TRUE;
                                }
                                break;
                            }
                        }
                        else
                        {
                            u4ColorCmp = 0x7;
                        }

                        if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) && u4ScanCnt)
                        {
                            fgPreload = TRUE;
                        }
                        else
                        {
                            fgPreload = FALSE;
                        }
                        if (JDEC_RzRow(rOutputInfo.u4OutYBufSa, rOutputInfo.u4OutCbBufSa, rOutputInfo.u4OutCrBufSa, 
                                    u4MCUy, u4MCUHeight, u4ColorCmp, fgPreload))
                        {
                            i4Ret = E_IMG_HWBUSY;
                        }
                    
                        rOutputInfo.u4OutYBufPitch = u4PitchPerRow;
                        rOutputInfo.u4OutCbBufPitch = u4PitchPerRow;
                        rOutputInfo.u4OutCrBufPitch = u4PitchPerRow;
                        if (u4MCUy % 2)                     //next output buffer
                        {
                            rOutputInfo.u4OutYBufSa = u4ColorBufY;
                            rOutputInfo.u4OutCbBufSa = u4ColorBufCb;
                            rOutputInfo.u4OutCrBufSa = u4ColorBufCr;
                        }
                        else
                        {
                            rOutputInfo.u4OutYBufSa = u4ColorBufY2;
                            rOutputInfo.u4OutCbBufSa = u4ColorBufCb2;
                            rOutputInfo.u4OutCrBufSa = u4ColorBufCr2;
                        }
                        i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);
                    }
                }

                if (!(u4MCUy % 10)) //check buffer after each 10 mcu rows
                {
                    UINT32 u4EmptySize;
                    if (prJPEG_JFIF_HEADER->fgRingBuffer && (!prJPEG_JFIF_HEADER->fgEOF) && 
                        (!_fgVerifyFlow))
                    {
                        i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);
                        u4NewRP = u4ReadAddrBytes - (UINT32)prJPEG_JFIF_HEADER->pu1RingBufStart;
                        if (u4NewRP < prJPEG_JFIF_HEADER->u4RingRP)
                        {
                            u4UsedLen = (prJPEG_JFIF_HEADER->u4RingBufSize + u4NewRP) - 
                                prJPEG_JFIF_HEADER->u4RingRP;
                        }
                        else
                        {
                            u4UsedLen = u4NewRP - prJPEG_JFIF_HEADER->u4RingRP;
                        }
                        if(prJPEG_JFIF_HEADER->u4RingData < u4UsedLen)
                        {
                            //LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", prJPEG_JFIF_HEADER->u4RingData, u4UsedLen);
                        }
    	    	        else
                        {
                            prJPEG_JFIF_HEADER->u4RingData -= u4UsedLen;
                        }	
                        prJPEG_JFIF_HEADER->u4RingRP = u4NewRP;   
                        u4EmptySize = prJPEG_JFIF_HEADER->u4RingBufSize - prJPEG_JFIF_HEADER->u4RingData;                        
                        if (u4EmptySize > JDEC_FULLNESS)
                        {
                            IMG_BsBufManager(u4JDecID);                        
                        }
#ifdef LINUX_TURNKEY_SOLUTION
                        else if(_u4ImageWidth>3500)
                        {
                            x_thread_delay(1);
                        }
#endif
                    }
                }
            } 
            else if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            {

                #ifdef JPG_PROGRASSIVE_VFY_FRAME_CKSUM
            		/*
                rOutputInfo.u4OutYBufSa = u4YBufSa;
                rOutputInfo.u4OutCbBufSa = u4CbBufSa;
                rOutputInfo.u4OutCrBufSa = u4CrBufSa;

                rOutputInfo.u4OutBank0YBufSa = u4YBufSa;
                rOutputInfo.u4OutBank0CbBufSa = u4CbBufSa;
                rOutputInfo.u4OutBank0CrBufSa = u4CrBufSa;

                rOutputInfo.u4OutBank1YBufSa = u4ColorBufY2;
                rOutputInfo.u4OutBank1CbBufSa = u4ColorBufY2;
                rOutputInfo.u4OutBank1CrBufSa = u4ColorBufY2;

                i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);                
					*/
                //for special bank0, bank 1 addr processing
                if(u4MCUy==1)
                {
                    if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                    {
                        rOutputInfo.u4OutYBufSa  += u4PitchPerRow*8;
                        rOutputInfo.u4OutCbBufSa += u4PitchPerRow*8;
                        rOutputInfo.u4OutCrBufSa += u4PitchPerRow*8;
                    } 
                    else
                    {
                        rOutputInfo.u4OutYBufSa += u4PitchPerRow*(rPicInfo.prSofInfo->arSofComp[0].u1VSampFactor * 8);
                        rOutputInfo.u4OutCbBufSa += u4PitchPerRow*(rPicInfo.prSofInfo->arSofComp[1].u1VSampFactor * 8);
                        rOutputInfo.u4OutCrBufSa += u4PitchPerRow*(rPicInfo.prSofInfo->arSofComp[2].u1VSampFactor * 8);
                    }

                    i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);
                }

                i4JDEC_HAL_Decode(u4JDecID, JDEC_HAL_DEC_PROG_VERIFY);
                for(u4WaitCnt=0; u4WaitCnt<5000; u4WaitCnt++)
                {
                    if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                        break;
                }
                if(u4WaitCnt==5000)
                {
                    _u4ErrMCUNum = u4MCUCnt;
                    break;
                }
                #else       //JPG_PROGRASSIVE_VFY_FRAME_CKSUM

                rOutputInfo.u4OutYBufSa = u4YBufSa;
                rOutputInfo.u4OutCbBufSa = u4CbBufSa;
                rOutputInfo.u4OutCrBufSa = u4CrBufSa;

                rOutputInfo.u4OutBank0YBufSa = u4YBufSa;
                rOutputInfo.u4OutBank0CbBufSa = u4CbBufSa;
                rOutputInfo.u4OutBank0CrBufSa = u4CrBufSa;

                rOutputInfo.u4OutBank1YBufSa = u4ColorBufY2;
                rOutputInfo.u4OutBank1CbBufSa = u4ColorBufCb2;
                rOutputInfo.u4OutBank1CrBufSa = u4ColorBufCr2;
                
                i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);                

                i4JDEC_HAL_Decode(u4JDecID, JDEC_HAL_DEC_PROG_VERIFY);
                for(u4WaitCnt=0; u4WaitCnt<5000; u4WaitCnt++)
                {
                    if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                        break;
                }
                if(u4WaitCnt==5000)
                {
                    _u4ErrMCUNum = u4MCUCnt;
                    break;
                }
                
                 if (((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0) && 
                    (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)) ||
                    (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)) //enhance or last scan of collection 
                 {
                    UINT32 u4ColorCmp;
                    LastScan=FALSE;
                    
                    if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                    {
                        UINT32 u4CompId = _prJPEG_JFIF_HEADER->rSOS.au1CompId[0];
                        u4CompId=u4CompId>=3 ? 2 : u4CompId;

                        switch(u4CompId)
                        {
                        case 0:
                            //If first scan only has Y, it's gray level and need CbCr padding.
                            u4ColorCmp = (u4ScanCnt) ? (UINT32)0x4 : (UINT32)0xF;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[0] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        case 1:
                            u4ColorCmp = 0x2;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[1] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        case 2:
                        default:    
                            u4ColorCmp = 0x1;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[2] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        }
                    }
                    else
                    {
                        u4ColorCmp = 0x7;
                    }

                    if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE) && u4ScanCnt)
                    {
                        fgPreload = TRUE;
                    }
                    else
                    {
                        fgPreload = FALSE;
                    }
                    if (_fgVerifyFlow)   
                    {

                        if(JDEC_RzRowMode_Vfy(rOutputInfo.u4OutYBufPitch, rOutputInfo.u4OutCbBufPitch, rPicInfo.prSofInfo->u2ImageWidth, rPicInfo.prSofInfo->u2ImageHeight, 
                        											rOutputInfo.u4OutBank0YBufSa, rOutputInfo.u4OutBank0CbBufSa, rOutputInfo.u4OutBank0CrBufSa, 
                        											rOutputInfo.u4OutBank1YBufSa, rOutputInfo.u4OutBank1CbBufSa, rOutputInfo.u4OutBank1CrBufSa, 
                        											au4RowWidth, au4RowHeight,
                        											_u4ImagePitch_RZ, _u4ImageWidth_RZ, _u4ImageHeight_RZ, IMGRZ_TAY_ADDR, IMGRZ_TAC_ADDR , 0, 0, Y_CBCR420_SEP16,
                        											u4ColorCmp, fgPreload, LastScan, u4MCUy, u4MCUHeight))
                        {
                            i4Ret = E_IMG_HWBUSY;
                        }
                    }
                    else
                    {
                        if(JDEC_RzRowMode(rOutputInfo.u4OutBank0YBufSa, rOutputInfo.u4OutBank0CbBufSa, rOutputInfo.u4OutBank0CrBufSa, 
                        											rOutputInfo.u4OutBank1YBufSa, rOutputInfo.u4OutBank1CbBufSa, rOutputInfo.u4OutBank1CrBufSa, 
                        											u4ColorCmp, fgPreload, LastScan, u4MCUy, u4MCUHeight))
                        {
                            i4Ret = E_IMG_HWBUSY;
                        }
                    }
                }

                if (!(u4MCUy % 10)) //check buffer after each 10 mcu rows
                {
                    UINT32 u4EmptySize;
                    if (prJPEG_JFIF_HEADER->fgRingBuffer && (!prJPEG_JFIF_HEADER->fgEOF) && 
                    (!_fgVerifyFlow))
                    {
                        i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);
                        u4NewRP = u4ReadAddrBytes - (UINT32)prJPEG_JFIF_HEADER->pu1RingBufStart;
                        if (u4NewRP < prJPEG_JFIF_HEADER->u4RingRP)
                        {
                            u4UsedLen = (prJPEG_JFIF_HEADER->u4RingBufSize + u4NewRP) - 
                            prJPEG_JFIF_HEADER->u4RingRP;
                        }
                        else
                        {
                            u4UsedLen = u4NewRP - prJPEG_JFIF_HEADER->u4RingRP;
                        }
                        if(prJPEG_JFIF_HEADER->u4RingData < u4UsedLen)
                        {
                        //LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", prJPEG_JFIF_HEADER->u4RingData, u4UsedLen);
                        }
                        else
                        {
                            prJPEG_JFIF_HEADER->u4RingData -= u4UsedLen;
                        }	
                        prJPEG_JFIF_HEADER->u4RingRP = u4NewRP;   
                        u4EmptySize = prJPEG_JFIF_HEADER->u4RingBufSize - prJPEG_JFIF_HEADER->u4RingData;                        
                        if (u4EmptySize > JDEC_FULLNESS)
                        {
                            IMG_BsBufManager(u4JDecID);                        
                        }
#ifdef LINUX_TURNKEY_SOLUTION
                        else if(_u4ImageWidth>3500)
                        {
                            x_thread_delay(1);
                        }
#endif
                    }
                }
                #endif  //JPG_PROGRASSIVE_VFY_FRAME_CKSUM
            } 
            else if(eDecodingMode == JDEC_HAL_DEC_MODE_BASELINE_PIC
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT
                || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)          
            {
                rOutputInfo.u4OutYBufSa = u4YBufSa;
                rOutputInfo.u4OutCbBufSa = u4CbBufSa;
                rOutputInfo.u4OutCrBufSa = u4CrBufSa;

                rOutputInfo.u4OutBank0YBufSa = u4ColorBufY;
                rOutputInfo.u4OutBank0CbBufSa = u4ColorBufCb;
                rOutputInfo.u4OutBank0CrBufSa = u4ColorBufCr;

                rOutputInfo.u4OutBank1YBufSa = u4ColorBufY2; 
                rOutputInfo.u4OutBank1CbBufSa = u4ColorBufCb2; 
                rOutputInfo.u4OutBank1CrBufSa = u4ColorBufCr2;
					
                i4JDEC_HAL_Set_Output_Info(u4JDecID,&rOutputInfo);

                if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE) || 
                    (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)) //enhance or last scan of collection 
                 {
                    UINT32 u4ColorCmp;
                    LastScan=FALSE;

                    if(_prJPEG_JFIF_HEADER->rSOS.u1CompInScan == 1)
                    {
				    		UINT32 u4CompId = _prJPEG_JFIF_HEADER->rSOS.au1CompId[0];
							u4CompId=u4CompId>=3 ? 2 : u4CompId;
					
                        switch(u4CompId)
                        {
                        case 0:
                            //If first scan only has Y, it's gray level and need CbCr padding.
                            u4ColorCmp = (u4ScanCnt) ? (UINT32)0x4 : (UINT32)0xF;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[0] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        case 1:
                            u4ColorCmp = 0x2;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[1] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        case 2:
                        default:    
                            u4ColorCmp = 0x1;
                            if ((_prJPEG_JFIF_HEADER->rSOS.u1Se == 63) && ((_prJPEG_JFIF_HEADER->rSOS.u1AhAl & 0x0f) == 0))
                            {
                                fgLastScan[2] = TRUE;
                                LastScan=TRUE;
                            }
                            break;
                        }
                    }
                    else
                    {
                        u4ColorCmp = 0x7;
                    }

                    if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE) && u4ScanCnt)
                    {
                        fgPreload = TRUE;
                    }
                    else
                    {
                        fgPreload = FALSE;
                    }
                    if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE)||((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)&&(LastScan==TRUE)))
                    {
                        if (_fgVerifyFlow)
                        {
                        	JDEC_RzScanMode_Vfy(rOutputInfo.u4OutYBufPitch, rOutputInfo.u4OutCbBufPitch, rPicInfo.prSofInfo->u2ImageWidth, rPicInfo.prSofInfo->u2ImageHeight, 
                        											rOutputInfo.u4OutBank0YBufSa, rOutputInfo.u4OutBank0CbBufSa, rOutputInfo.u4OutBank0CrBufSa, 
                        											rOutputInfo.u4OutBank1YBufSa, rOutputInfo.u4OutBank1CbBufSa, rOutputInfo.u4OutBank1CrBufSa, 
                        											au4RowWidth, au4RowHeight,
                        											_u4ImagePitch_RZ, _u4ImageWidth_RZ, _u4ImageHeight_RZ, IMGRZ_TAY_ADDR, IMGRZ_TAC_ADDR , 0, 0, Y_CBCR420_SEP16,
                        											u4ColorCmp, fgPreload, LastScan);
                        }
                        else
                        {
                            if (JDEC_RzScanMode(rOutputInfo.u4OutBank0YBufSa, rOutputInfo.u4OutBank0CbBufSa, rOutputInfo.u4OutBank0CrBufSa, 
                            											rOutputInfo.u4OutBank1YBufSa, rOutputInfo.u4OutBank1CbBufSa, rOutputInfo.u4OutBank1CrBufSa, 
                            											u4ColorCmp, fgPreload, LastScan))
                            {
                                i4Ret = E_IMG_HWBUSY;
                            }
                        }
                    }

                }

                if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                    || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
                    {
                            if (prJPEG_JFIF_HEADER->fgRingBuffer && (!prJPEG_JFIF_HEADER->fgEOF) && 
                                (!_fgVerifyFlow) && u4ScanCnt)
                            {
                                i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);
                                u4NewRP = u4ReadAddrBytes - (UINT32)prJPEG_JFIF_HEADER->pu1RingBufStart;
                                if (u4NewRP < prJPEG_JFIF_HEADER->u4RingRP)
                                {
                                    u4UsedLen = (prJPEG_JFIF_HEADER->u4RingBufSize + u4NewRP) - 
                                                prJPEG_JFIF_HEADER->u4RingRP;
                                }
                                else
                                {
                                    u4UsedLen = u4NewRP - prJPEG_JFIF_HEADER->u4RingRP;
                                }
                                if(prJPEG_JFIF_HEADER->u4RingData < u4UsedLen)
                                {
                                //LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", prJPEG_JFIF_HEADER->u4RingData, u4UsedLen);
                                }
                                else
                                {
                                    prJPEG_JFIF_HEADER->u4RingData -= u4UsedLen;
                                }   
                                prJPEG_JFIF_HEADER->u4RingRP = u4NewRP;                  
                                IMG_BsBufManager(u4JDecID);                        
#ifdef LINUX_TURNKEY_SOLUTION
                                 if(_u4ImageWidth>3500)
                                {
                                   x_thread_delay(1);
                                }
#endif
                            }
    
                    }

        	i4JDEC_HAL_Decode(u4JDecID, JDEC_HAL_DEC_PROG_VERIFY);

                for(u4WaitCnt=0; u4WaitCnt < (UINT32)0xFFFFFFFF; u4WaitCnt++)
                {
                    if((i4Ret = i4JDEC_HAL_Get_Decode_Status(u4JDecID)) >= 0)
                    {
                        break;
                    }
                    else
                    {
                        if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
                            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
                        {
                            if(VLDJPG_ReadWP(u4JDecID) == VLDJPG_ReadRP(u4JDecID))
                            {
                                    LOG(7,"Read Point == Write Point\n");
                                    if (prJPEG_JFIF_HEADER->fgRingBuffer && (!prJPEG_JFIF_HEADER->fgEOF) && 
                                        (!_fgVerifyFlow))
                                    {
                                        i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);
                                        u4NewRP = u4ReadAddrBytes - (UINT32)prJPEG_JFIF_HEADER->pu1RingBufStart;
                                        if (u4NewRP < prJPEG_JFIF_HEADER->u4RingRP)
                                        {
                                            u4UsedLen = (prJPEG_JFIF_HEADER->u4RingBufSize + u4NewRP) - 
                                                        prJPEG_JFIF_HEADER->u4RingRP;
                                        }
                                        else
                                        {
                                            u4UsedLen = u4NewRP - prJPEG_JFIF_HEADER->u4RingRP;
                                        }
                                        if(prJPEG_JFIF_HEADER->u4RingData < u4UsedLen)
                                        {
                                        //LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", prJPEG_JFIF_HEADER->u4RingData, u4UsedLen);
                                        }
                                        else
                                        {
                                            prJPEG_JFIF_HEADER->u4RingData -= u4UsedLen;
                                        }   
                                        prJPEG_JFIF_HEADER->u4RingRP = u4NewRP;                        
                                        IMG_BsBufManager(u4JDecID);                        
#ifdef LINUX_TURNKEY_SOLUTION
                                         if(_u4ImageWidth>3500)
                                        {
                                           x_thread_delay(1);
                                        }
#endif
                                    }
                                }
                            }
                        }
                }
                if(u4WaitCnt == (UINT32)0xFFFFFFFF)
                {
                    _u4ErrMCUNum = 2266;
                    break;
                }
                break;
            }
        }

        if (i4Ret != E_IMG_OK)
        {
            if(((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)&&(LastScan==TRUE))
                || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
            {
                IMGRZ_UnlockWaitIsr();
                IMGRZ_UnlockCmdque();
            }
            break;
        }
        if(((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT) &&(LastScan==TRUE))
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            IMGRZ_Wait();
            IMGRZ_Unlock();
        }
        // For progressvie picture, search next start of scan (SOS)
        if((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) 
        	 || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
            || (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE))
        {
            i4JDEC_HAL_Get_Read_Pointer(u4JDecID,&u4ReadAddrBytes,&u4ReadAddrBits);
            if(u4ReadAddrBits > 0)
                u4ReadAddrBytes++;

            if(u4ReadAddrBytes >= (rInputInfo.u4InFifoSa + rInputInfo.u4InFifoSz))
            {
                u4ReadAddrBytes -= rInputInfo.u4InFifoSa;
            }            
        #ifdef JPEG_VLD_TO_PARSE_HEADER
            if((UINT32)(u4ReadAddrBytes - rInputInfo.u4InFifoSa) > (UINT32)_i4JpegFileSize)
        #else   /*JPEG_VLD_TO_PARSE_HEADER*/
            if(((UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead) - rInputInfo.u4InFifoSa) > (UINT32)_i4JpegFileSize)
        #endif  /*JPEG_VLD_TO_PARSE_HEADER*/
            {
                Printf("(prJPEG_JFIF_HEADER->pu1CurrRead) - rInputInfo.u4InFifoSa > _i4JpegFileSize\n");
                break;
            }
            //if(u4ScanCnt == 2)
            //    break;

            prJPEG_JFIF_HEADER->fgParsed = 0;   // winton
            hParser = (JPEGHANDLE)prJPEG_JFIF_HEADER;
        #ifdef JPEG_VLD_TO_PARSE_HEADER
            if (JFIF_New(hParser, (void *)u4ReadAddrBytes, (UINT32)_i4JpegFileSize))
        #else   /*JPEG_VLD_TO_PARSE_HEADER*/
            if (JFIF_New(hParser, (void *)u4ReadAddrBytes, (UINT32)_i4JpegFileSize))
        #endif  /*JPEG_VLD_TO_PARSE_HEADER*/
            {
                if ((fgLastScan[0] & fgLastScan[1] & fgLastScan[2] & (u4TotalComp == 3)) ||
                    (fgLastScan[0] & (u4TotalComp == 1)))  //last scan done, finished
                {
                    break;
                }
                i4Ret = E_IMG_DEC_FAIL;
                break;
            }
            // When reach end of image, quit.
            if(prJPEG_JFIF_HEADER->fgEOI)
            {
                if ((!(fgLastScan[0] & fgLastScan[1] & fgLastScan[2]) && u4TotalComp == 3 )||
                    (!fgLastScan[0] && u4TotalComp == 1))
                {
                    i4Ret = E_IMG_DEC_FAIL;       //no last scan but EOI ==> error
                }
                break;
            }

            if (_fgStop)
            {
                _fgStop = FALSE;
                i4Ret = E_IMG_CANCEL;
                break;
            }
            if ((fgLastScan[0] & fgLastScan[1] & fgLastScan[2] & (u4TotalComp == 3)) ||
                (fgLastScan[0] & (u4TotalComp == 1)))  //last scan done, finished
            {
                break;
            }

            i4JDEC_HAL_Set_HuffmanTable_Info(u4JDecID,(JDEC_JFIF_DHT_T *)&(prJPEG_JFIF_HEADER->rDHT));

            x_memcpy(&(prJPEG_JFIF_HEADER->rSOS), &(prJPEG_JFIF_HEADER->arSOS[u4ScanCnt + 1]), 
                sizeof(JPEG_JFIF_SOS_T));
            i4JDEC_HAL_Set_Scan_Info(u4JDecID,&(prJPEG_JFIF_HEADER->rSOS));

        #ifdef JPEG_VLD_TO_PARSE_HEADER
        #else   /*JPEG_VLD_TO_PARSE_HEADER*/
            i4JDEC_HAL_Set_Read_Pointer(u4JDecID,(UINT32)(prJPEG_JFIF_HEADER->pu1CurrRead));
        #endif  /*JPEG_VLD_TO_PARSE_HEADER*/

		    //print the error type
		    #ifdef JPEG_AUTO_TEST
		    //if(VLDJPG_ReadErrorType_Prog(0)!=0)
		    {
				Printf("Scan %d Error detection type prog=[%x]\n", u4ScanCnt, VLDJPG_ReadErrorType_Prog(0));	   
		    }
		    #else
		    		LOG(5, "Scan %d Error detection type prog=[%x]\n", u4ScanCnt, VLDJPG_ReadErrorType_Prog(0));	   
		    #endif
	    
            u4ScanCnt++;
	_u4CurScan = u4ScanCnt;
        }

    } while((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE) 
										||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)
										||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
										||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE)
										||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT)
										||(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE));

#if 0
    if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
    {
        x_memcpy((void *)u4ColorBufY, (void *)_u4EnhanceBufY,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
        x_memcpy((void *)u4ColorBufCb, (void *)_u4EnhanceBufCb,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
        x_memcpy((void *)u4ColorBufCr, (void *)_u4EnhanceBufCr,JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
    }
#endif    

    UNUSED(_u4ErrMCUNum);
    UNUSED(i4Ret);

    //print the progressive cksum
    #ifdef JPEG_AUTO_TEST
	Printf("Jpgdec CKsum progressive=[%x]\n", VLDJPG_ReadCKsum_Prog(0));	   
    #endif

    i4JDEC_HAL_Uninit(u4JDecID);

    return i4Ret;
}

INT32 i4JpegCompareBuf(void)
{
    UINT32 u4X,u4Y;
    UINT32 u4ColorBufY = _u4ColorBufY;
    UINT32 u4ColorBufCb = _u4ColorBufCb;
    UINT32 u4ColorBufCr = _u4ColorBufCr;
    UINT32 u4ColorBufWidth = _u4ColorBufWidth;
    UINT32 u4GoldenBufY = _u4ColorBufY2;
    UINT32 u4GoldenBufCb = _u4ColorBufCb;
    UINT32 u4GoldenBufCr = _u4ColorBufCr2;
    UINT32 u4GoldenBufWidth = _u4ColorBufWidth;
    BYTE bColor,bGoldenColor;
    UINT32 u4XSample,u4YSample;
    UINT32 u4GoldenXSample,u4GoldenYSample;
    const UINT32 u4YThreshold = 0;
    const UINT32 u4CbThreshold = 0;
    const UINT32 u4CrThreshold = 0;
    UINT32 u4MaxErr;
    UINT32 u4MaxErrX=0,u4MaxErrY=0;
    UINT32 u4Err;

    _u4DiffCnt = 0;

    // Compare Y
    u4XSample = _u4XSampleY;
    u4YSample = _u4YSampleY;
    u4GoldenXSample = u4XSample;
    u4GoldenYSample = u4YSample;
    u4MaxErr = 0;
    for(u4Y=0; u4Y<_u4ImageHeight; u4Y++)
        for(u4X=0; u4X<_u4ImageWidth; u4X++)
        {
            // Get HW decoded color
            bColor = *(BYTE *)(u4ColorBufY + (u4Y>>u4YSample)*u4ColorBufWidth + (u4X>>u4XSample));

            // Get Goldedn color
            bGoldenColor = *(BYTE *)(u4GoldenBufY + (u4Y>>u4GoldenYSample)*u4GoldenBufWidth + (u4X>>u4GoldenXSample));

            if(((UINT32)bColor > ((UINT32)bGoldenColor+u4YThreshold)) || ((UINT32)bColor < ((UINT32)bGoldenColor-u4YThreshold)))
            {
                //Printf("[Jpeg][Verify] Compare Y, bColor(%d,%d) = %d, bGoldenColor(%d,%d) = %d",u4X,u4Y,bColor,u4X,u4Y,bGoldenColor);
                _u4DiffCnt++;
                if(bColor > bGoldenColor)
                    u4Err = bColor - bGoldenColor;
                else
                    u4Err = bGoldenColor - bColor;

                if(u4Err > u4MaxErr)
                {
                    u4MaxErr = u4Err;
                    u4MaxErrX = u4X;
                    u4MaxErrY = u4Y;
                }
            }
        }
    if(u4MaxErr>0)
        Printf("[Jpeg][Verify] Compare Y, Max err = %d, pixel(%d,%d)",u4MaxErr,u4MaxErrX,u4MaxErrY);


    // Compare Cb
    u4XSample = _u4XSampleCb;
    u4YSample = _u4YSampleCb;
    u4GoldenXSample = u4XSample;
    u4GoldenYSample = u4YSample;
    u4MaxErr = 0;
    for(u4Y=0; u4Y<_u4ImageHeight; u4Y++)
        for(u4X=0; u4X<_u4ImageWidth; u4X++)
        {
            // Get HW decoded color
            bColor = *(BYTE *)(u4ColorBufCb + (u4Y>>u4YSample)*u4ColorBufWidth + (u4X>>u4XSample));

            // Get Goldedn color
            bGoldenColor = *(BYTE *)(u4GoldenBufCb + (u4Y>>u4GoldenYSample)*u4GoldenBufWidth + (u4X>>u4GoldenXSample));

            if(((UINT32)bColor > ((UINT32)bGoldenColor+u4CbThreshold)) || ((UINT32)bColor < ((UINT32)bGoldenColor-u4CbThreshold)))
            {
                //Printf("[Jpeg][Verify] Compare Cb, bColor(%d,%d) = %d, bGoldenColor(%d,%d) = %d",u4X,u4Y,bColor,u4X,u4Y,bGoldenColor);
                _u4DiffCnt++;

                if(bColor > bGoldenColor)
                    u4Err = bColor - bGoldenColor;
                else
                    u4Err = bGoldenColor - bColor;

                if(u4Err > u4MaxErr)
                {
                    u4MaxErr = u4Err;
                    u4MaxErrX = u4X;
                    u4MaxErrY = u4Y;
                }
            }
        }
    if(u4MaxErr>0)
        Printf("[Jpeg][Verify] Compare Cb, Max err = %d, pixel(%d,%d)",u4MaxErr,u4MaxErrX,u4MaxErrY);


    // Compare Cr
    u4XSample = _u4XSampleCr;
    u4YSample = _u4YSampleCr;
    u4GoldenXSample = u4XSample;
    u4GoldenYSample = u4YSample;
    u4MaxErr = 0;
    for(u4Y=0; u4Y<_u4ImageHeight; u4Y++)
        for(u4X=0; u4X<_u4ImageWidth; u4X++)
        {
            // Get HW decoded color
            bColor = *(BYTE *)(u4ColorBufCr + (u4Y>>u4YSample)*u4ColorBufWidth + (u4X>>u4XSample));

            // Get Goldedn color
            bGoldenColor = *(BYTE *)(u4GoldenBufCr + (u4Y>>u4GoldenYSample)*u4GoldenBufWidth + (u4X>>u4GoldenXSample));

            if(((UINT32)bColor > ((UINT32)bGoldenColor+u4CrThreshold)) || ((UINT32)bColor < ((UINT32)bGoldenColor-u4CrThreshold)))
            {
                //Printf("[Jpeg][Verify] Compare Cr, bColor(%d,%d) = %d, bGoldenColor(%d,%d) = %d",u4X,u4Y,bColor,u4X,u4Y,bGoldenColor);
                _u4DiffCnt++;

                if(bColor > bGoldenColor)
                    u4Err = bColor - bGoldenColor;
                else
                    u4Err = bGoldenColor - bColor;

                if(u4Err > u4MaxErr)
                {
                    u4MaxErr = u4Err;
                    u4MaxErrX = u4X;
                    u4MaxErrY = u4Y;
                }
            }
        }
    if(u4MaxErr>0)
        Printf("[Jpeg][Verify] Compare Cr, Max err = %d, pixel(%d,%d)",u4MaxErr,u4MaxErrX,u4MaxErrY);

    if(_u4DiffCnt>0)
        return -1;
    else
        return 0;
}

#ifdef JPEG_AUTO_TEST
static UINT32 _8520CheckSum(void* pvData, UINT32 u4ByteLen)
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

static INT32 progVerifyDecMode = 4; //JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE
void i4JpegSetProgressiveDecodeMode(INT32 newMode)
{
    progVerifyDecMode = newMode;
    return;
}
INT32 i4JpegDecode(void)
{
    JPEGHANDLE hParser;
    UINT32 u4Sum[3];
    //static UINT32 u4FirstHwId = 0;
#ifdef JDEC_VFY_TEST_SPEED
    UINT32 u4I,u4StcStart,u4StcEnd,u4StcTotal;
    UINT32 u4StartStcL,u4StartStcH,u4Stc;
#endif
    _fgVerifyFlow = TRUE;

    _prJPEG_JFIF_HEADER = JFIF_AllocInstance();
	if(!_prJPEG_JFIF_HEADER)
	  return -1;

    hParser = (JPEGHANDLE)_prJPEG_JFIF_HEADER;

//    x_memset((void *)_prJPEG_JFIF_HEADER,0,sizeof(JPEG_JFIF_HEADER_T));
    i4JDEC_HAL_Pre_Init(0);
    JFIF_New(hParser,_EmuJpgFifo,JDEC_FIFO_SIZE);

    if((_prJPEG_JFIF_HEADER->rSOF.u2ImageWidth == 0) || (_prJPEG_JFIF_HEADER->rSOF.u2ImageHeight == 0))
        return -1;

    _u4ImageWidth = _prJPEG_JFIF_HEADER->rSOF.u2ImageWidth;
    _u4ImageHeight = _prJPEG_JFIF_HEADER->rSOF.u2ImageHeight;

	if((_u4ImageWidth>JDEC_COLOR_BUF_WIDTH)||(_u4ImageHeight>JDEC_COLOR_BUF_HEIGHT)    )
	{
		_u4ImageWidth_RZ=1024;
		_u4ImageHeight_RZ=768;
		_u4ImagePitch_RZ = 1024;
	}
	else
	{
		_u4ImageWidth_RZ = _u4ImageWidth;
		_u4ImageHeight_RZ = _u4ImageHeight;
		_u4ImagePitch_RZ = ((_u4ImageWidth_RZ+15)>>4)<<4;
	}

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

    {
        JDEC_HAL_DEC_MODE_T eDecodingMode;
        static UCHAR* prog_decode_mode[] = {"MCU_MULTI_COLLECT", "MCU_ENHANCE", "MCU_ROW_MULTI_COLLECT", "MCU_ROW_ENHANCE", "SCAN_MULTI_COLLECT", "SCAN_ENHANCE"};
        if(_prJPEG_JFIF_HEADER->rSOF.eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
        {
            if(progVerifyDecMode == 0)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT;
            }
            else if(progVerifyDecMode == 1)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE;
            }
            else if(progVerifyDecMode == 2)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT;
            }
            else if(progVerifyDecMode == 3)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE;
            }
            else if(progVerifyDecMode == 4)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT;
            }
            else if(progVerifyDecMode == 5)
            {
                eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE;
            }
            else
            {
                printf("progVerifyDecMode invalid:%d\n", progVerifyDecMode);
            eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE;
        }
        }
        else
        {
            eDecodingMode = JDEC_HAL_DEC_MODE_BASELINE_MCU;
        }
    	printf("<Progressive DecodingMode = %d(%s)>\n", eDecodingMode, prog_decode_mode[progVerifyDecMode]);
#ifdef JPG_PROGRASSIVE_VFY_FRAME_CKSUM	
        x_memset((void *)_u4ColorBufY,0x0,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void *)_u4ColorBufCb,0x0,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        x_memset((void *)_u4ColorBufCr,0x0,(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufY), (UINT32)( JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCb), (UINT32)( JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCr), (UINT32)(  JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
#else
		HalFlushInvalidateDCache();
#endif
        vJpegDecode(0,eDecodingMode,_u4ColorBufY,_u4ColorBufCb,_u4ColorBufCr,_u4ColorBufY2,_u4ColorBufCb2,_u4ColorBufCr2);
        u4Sum[0] = _8520CheckSum((void *)_u4ColorBufY, (UINT32)(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        u4Sum[1] = _8520CheckSum((void *)_u4ColorBufCb, (UINT32)(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        u4Sum[2] = _8520CheckSum((void *)_u4ColorBufCr, (UINT32)(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT));
        printf(" Y Checksum: 0x%x\n", u4Sum[0]);
        printf("Cr Checksum: 0x%x\n", u4Sum[1]);
        printf("Cb Checksum: 0x%x\n", u4Sum[2]);
        //printf("Total Checksum: 0x%x\n", u4Sum[0] + u4Sum[1] + u4Sum[2]);
#else

        x_memset((void *)_u4ColorBufY,0x0,(JDEC_COLOR_BUF_WIDTH*16));
        x_memset((void *)_u4ColorBufCb,0x0,(JDEC_COLOR_BUF_WIDTH*16));
        x_memset((void *)_u4ColorBufCr,0x0,(JDEC_COLOR_BUF_WIDTH*16));
        x_memset((void *)_u4ColorBufY2,0x0,(JDEC_COLOR_BUF_WIDTH*16));
        x_memset((void *)_u4ColorBufCb2,0x0,(JDEC_COLOR_BUF_WIDTH*16));
        x_memset((void *)_u4ColorBufCr2,0x0,(JDEC_COLOR_BUF_WIDTH*16));
		x_memset((void *)IMGRZ_TAY_ADDR,0x0,((((_u4ImageWidth_RZ+15)>>4)<<4)*_u4ImageHeight_RZ));
		x_memset((void *)IMGRZ_TAC_ADDR ,0x0,(_u4ImageWidth_RZ*_u4ImageHeight_RZ));               	

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufY), (UINT32)( JDEC_COLOR_BUF_WIDTH*16));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCb), (UINT32)(  JDEC_COLOR_BUF_WIDTH*16));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCr), (UINT32)(  JDEC_COLOR_BUF_WIDTH*16));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufY2), (UINT32)(  JDEC_COLOR_BUF_WIDTH*16));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCb2), (UINT32)(  JDEC_COLOR_BUF_WIDTH*16));
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_u4ColorBufCr2), (UINT32)(  JDEC_COLOR_BUF_WIDTH*16));

        HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(IMGRZ_TAY_ADDR), (UINT32)(((((_u4ImageWidth_RZ+15)>>4)<<4)*_u4ImageHeight_RZ)));
        HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(IMGRZ_TAC_ADDR ), (UINT32)((_u4ImageWidth_RZ*_u4ImageHeight_RZ)));        
#else
		HalFlushInvalidateDCache();
#endif
        vJpegDecode(0,eDecodingMode,_u4ColorBufY,_u4ColorBufCb,_u4ColorBufCr,_u4ColorBufY2,_u4ColorBufCb2,_u4ColorBufCr2);

        u4Sum[0] = _8520CheckSum((void *)IMGRZ_TAY_ADDR, (UINT32)((((_u4ImageWidth_RZ+15)>>4)<<4)*_u4ImageHeight_RZ));
        u4Sum[1] = _8520CheckSum((void *)IMGRZ_TAC_ADDR , (UINT32)(_u4ImageWidth_RZ*_u4ImageHeight_RZ));
        
        printf(" Y Checksum: 0x%x\n", u4Sum[0]);
        printf("CbCr Checksum: 0x%x\n", u4Sum[1]);
#endif

#ifdef JPEG_AUTO_TEST
       {
        
		INT32 arCheckFileParam[10];
		INT32 arCheckSum[4],i4Index=0;
		UINT32 fmt=0;
		BOOL fgCheckPass=TRUE;
		arCheckSum[0]=3;
		arCheckSum[1]=u4Sum[0];
		arCheckSum[2]=u4Sum[1];
		#ifdef JPG_PROGRASSIVE_VFY_FRAME_CKSUM	
		arCheckSum[3]=u4Sum[2];
		#endif
 		_JpgVerifyCmdSetParam(1,(INT32)arCheckSum);
		if(i4CurDecIndex>=0)
		{
			if(_arProgressive[i4CurDecIndex].u4rCheckSum[0]==0 &&
			   _arProgressive[i4CurDecIndex].u4rCheckSum[1]==0	&&
			   _arProgressive[i4CurDecIndex].u4rCheckSum[2]==0)
			{
			   printf("No golden checksum to do compare\n");
			}
			else
			{
				#ifdef JPG_PROGRASSIVE_VFY_FRAME_CKSUM	
				for(i4Index=0;i4Index<3i4Index++)
				#else
				for(i4Index=0;i4Index<2;i4Index++)
				#endif
				{
				   if(_arProgressive[i4CurDecIndex].u4rCheckSum[i4Index]!=u4Sum[i4Index])
				   {
					  printf("Checksum=(Golden:0x%x,Result:0x%x\n",_arProgressive[i4CurDecIndex].u4rCheckSum[i4Index],u4Sum[i4Index]);
					  fgCheckPass=FALSE;
				   }
				}
				
				if(!fgCheckPass)
				{
				   printf("Progressive:%s Checksum Fail\n",_arProgressive[i4CurDecIndex].szFileName);
				}
				else
				{
				   printf("Progressive Checksum Pass\n");
				}
			}
		}

        if(eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT || eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE)
        {
            arCheckFileParam[0]=3;
            arCheckFileParam[1]=5;
            arCheckFileParam[2]=_u4ColorBufY;
            arCheckFileParam[3]=(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
            
            arCheckFileParam[4]=6;
            arCheckFileParam[5]=_u4ColorBufCb;
            arCheckFileParam[6]=(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
            
            arCheckFileParam[7]=7;
            arCheckFileParam[8]=_u4ColorBufCr;
            arCheckFileParam[9]=(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT);
        }
        else
        {

            arCheckFileParam[0]=2;
            arCheckFileParam[1]=0;
            arCheckFileParam[2]=IMGRZ_TAY_ADDR;
            arCheckFileParam[3]=((((_u4ImageWidth_RZ+15)>>4)<<4)*_u4ImageHeight_RZ);

            arCheckFileParam[4]=1;
            arCheckFileParam[5]=IMGRZ_TAC_ADDR ;
            arCheckFileParam[6]=(_u4ImageWidth_RZ*_u4ImageHeight_RZ);
        }
        _JpgVerifyCmdSetParam(2,(INT32)arCheckFileParam);
			
            if(_u4XSampleCb==2)
            {
               if(_u4YSampleCb==2)
			   	  fmt=3;
			   else if(_u4YSampleCb==1)
			   	  fmt=2;
            }
			else if(_u4XSampleCb==1)
		    {
			  if(_u4YSampleCb==2)
			    fmt=1;
			  else if(_u4YSampleCb==1)
			    fmt=0;
			}
	        _JpgVerifyCmdSetParam(22,fmt);
       }
#endif
    }
    JFIF_FreeInstance(_prJPEG_JFIF_HEADER);
	_prJPEG_JFIF_HEADER=NULL;
    return 0;
}


#ifdef JPG_SEMI_HOST_BY_USB
#include "x_fm.h"
BOOL _PJPGLoadUsbFile(CHAR *pcFileName, UINT32 u4Addr)
{
    UINT64 u8FileSize, u8Tmp;
    UINT32 u4Request, u4Read;
    HANDLE_T  h_file;

    VERIFY(FMR_OK == x_fm_open(FM_ROOT_HANDLE, pcFileName, FM_READ_ONLY, 0777, FALSE, &h_file));
    VERIFY(FMR_OK == x_fm_lseek(h_file, 0, FM_SEEK_END, &u8FileSize));
    VERIFY(FMR_OK == x_fm_lseek(h_file, 0, FM_SEEK_BGN, &u8Tmp));

    while (u8FileSize > 0)
    {
        if (u8FileSize >> 32)
        {
            u4Request = 0xffffffff;
        }
        else
        {
            u4Request = (UINT32)u8FileSize;
        }
        x_fm_read(h_file, (VOID *)u4Addr, u4Request, &u4Read);
        ASSERT(u4Request == u4Read);
        u4Addr += u4Read;
        u8FileSize -= u4Read;
    }

    VERIFY(FMR_OK == x_fm_close(h_file));

    return TRUE;
}
#endif

INT32 i4LoadJpegFile(UINT32 u4Idx)
{

#ifdef PJPG_SEMI_HOST_BY_USB
   	CHAR pcIjpgFileName4[256];
#endif

	printf("----------------\n");

#ifdef T32_TEST  

	#ifdef PJPG_SEMI_HOST_BY_USB
		x_sprintf(pcIjpgFileName4 , "/mnt/usb_0/Jpg_test/progressive/");			     
		x_strcat(pcIjpgFileName4, (CHAR *)_arProgressive[u4Idx].szFileName);
		Printf("116 imgrz load %s \n", pcIjpgFileName4);              
		Printf("117 load addr %x \n", _JpegVerParam.i4JpegSrcBufAddr);
		_PJPGLoadUsbFile(pcIjpgFileName4, _EmuJpgFifo);
	#else
	    printf("d.load.binary \"%s/%s\" 0x%08x /ny\n", "D:/jpg_test/progressive", 
        	 _arProgressive[u4Idx].szFileName, (UINT32)_EmuJpgFifo);
	    UNUSED(T32_HostExec(1, "d.load.binary \"%s\\%s\" 0x%08x /ny", "D:/jpg_test/progressive", 
	        _arProgressive[u4Idx].szFileName, (UINT32)_EmuJpgFifo));
	#endif
	
        #if defined(CC_MT5365) || defined(CC_MT5395) || defined(JPEG_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine((UINT32)(_EmuJpgFifo), (UINT32)(JDEC_FIFO_SIZE));
        #else
        HalFlushInvalidateDCache();    
        #endif
#endif
    return _arProgressive[u4Idx].u4ByteSize;
}

VOID vJpegVertifyAutoCreateBuffer(UCHAR *pBufferAddr)
{
    if(!_EmuJpgFifo)
    {
       _EmuJpgFifo=pBufferAddr;
		//_EmuJpgFifo = BSP_AllocAlignedDmaMemory(JDEC_FIFO_SIZE,2048);
		//x_memset(_EmuJpgFifo,0,JDEC_FIFO_SIZE);
    }
	   
	//if(pBufferAddr)
    //     *pBufferAddr=_EmuJpgFifo;
       return;
}

INT32 vJpegVerifyAuto(INT32 index,INT32 i4JpgSize)
{
     static BOOL sfgFirst = TRUE;
     //UINT32 u4I,i4FileSize;
	 
    if (sfgFirst)
    {
        sfgFirst = FALSE;
        _u4ColorBufY = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCb = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCr = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);

        _u4ColorBufY2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCb2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
        _u4ColorBufCr2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
   }
   
   if(index>=0)
   {
       _i4JpegFileSize = i4LoadJpegFile(index);
   }
   else
   {
      _i4JpegFileSize=i4JpgSize;
   }
  
   if(_i4JpegFileSize < 0)
   {
       Printf("[Jpeg][Verify] Load File fail");
       return -1;
   }
   
   i4CurDecIndex=index;
   if(i4JpegDecode() <0)
   {
       Printf("[Jpeg][Verify] Decode %s fail",_arProgressive[index].szFileName);
	return -1;
   }
   
   Printf("[Jpeg][Verify] Verify complete.\n");
   return 0;
}

INT32 vJpegVerify(INT32 i4Argc, const CHAR ** szArgv)
{
    UINT32 u4Idx, u4I, u4Times;
    static BOOL sfgFirst = TRUE;
    if (i4Argc < 3)
    {
        for (u4I = 0; u4I < LIST_MAX; u4I++)
        {
            printf("file list: (%d) \"%s/%s\" /ny\n", u4I, "D:/jpg_test/progressive", 
                _arProgressive[u4I].szFileName);
        }
        return -1;
    }
#ifdef CC_EMULATION    
    u4Idx = StrToInt(szArgv[1]);
    u4Times = StrToInt(szArgv[2]);
#else
    u4Idx = (UINT32)0;
    u4Times = (UINT32)1;
#endif    
    if (sfgFirst)
{
        sfgFirst = FALSE;
    // Allocate Memory
    _EmuJpgFifo = (BYTE*) BSP_AllocAlignedDmaMemory(JDEC_FIFO_SIZE,2048);

    _u4ColorBufY = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    _u4ColorBufCb = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    _u4ColorBufCr = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);

    _u4ColorBufY2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    _u4ColorBufCb2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    _u4ColorBufCr2 = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);

    //_u4FrameBufY = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    //_u4FrameBufC = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);

    //_u4EnhanceBufY = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    //_u4EnhanceBufCb = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    //_u4EnhanceBufCr = (UINT32)BSP_AllocAlignedDmaMemory(JDEC_COLOR_BUF_WIDTH*JDEC_COLOR_BUF_HEIGHT,2048);
    }
    x_memset(_EmuJpgFifo,0,JDEC_FIFO_SIZE);

    for(u4I=0; u4I < u4Times; u4I++)
    {
        //Printf("[Jpeg][Verify] Load File %s",_abJpegFileList[_u4CurJpegFileListNum]);
        _i4JpegFileSize = i4LoadJpegFile(u4Idx);
        if(_i4JpegFileSize < 0)
        {
            Printf("[Jpeg][Verify] Load File fail");
            continue;
        }

        Printf("[Jpeg][Verify] Decode Jpeg by HW");
        if(i4JpegDecode() >= 0)
        {
/*#if JDEC_VFY_TEST_PIC_MODE
            i4SaveFrameBuf();
#elif JDEC_VFY_TEST_PROGRESSIVE_PRELOAD
            i4SaveFrameBuf();
#else*/
            /*if(i4JpegCompareBuf()<0)
                Printf("[Jpeg][Verify] Hw1 does not match Hw2");

            i4SaveColorBuf();*/
//#endif
        } else
        {
        }
    }

    Printf("[Jpeg][Verify] Verify complete.");
#ifndef CHIPVERIFY
    // Free Memory
    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufY);
    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufCb);
    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufCr);

    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufY2);
    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufCb2);
    BSP_FreeAlignedDmaMemory((void *)_u4ColorBufCr2);

    BSP_FreeAlignedDmaMemory((void *)_u4FrameBufY);
    BSP_FreeAlignedDmaMemory((void *)_u4FrameBufC);

    BSP_FreeAlignedDmaMemory((void *)_EmuJpgFifo);

    BSP_FreeAlignedDmaMemory((void *)_u4EnhanceBufY);
    BSP_FreeAlignedDmaMemory((void *)_u4EnhanceBufCb);
    BSP_FreeAlignedDmaMemory((void *)_u4EnhanceBufCr);
#endif  
    return 0;
}

#endif 

INT32 vJpegProgDec(JPEGHANDLE hParser, void **pvCoefBuffer, void **pvOutBuffer, void **pvNzBuffer)
{
    JDEC_HAL_DEC_MODE_T eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT;
    INT32 i4Ret;

    _fgVerifyFlow = FALSE;
    _fgStop = FALSE;

    _prJPEG_JFIF_HEADER = (JPEG_JFIF_HEADER_T *)hParser;

    if((_prJPEG_JFIF_HEADER->rSOF.u2ImageWidth == 0) || (_prJPEG_JFIF_HEADER->rSOF.u2ImageHeight == 0))
        return -1;

    _u4ImageWidth = _prJPEG_JFIF_HEADER->rSOF.u2ImageWidth;
    _u4ImageHeight = _prJPEG_JFIF_HEADER->rSOF.u2ImageHeight;
    
      if ((_u4ImageWidth * _u4ImageHeight) > JDEC_COLLECT_LIMIT)
      {
        eDecodingMode = JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE;
      }
	
	
    _u4XSampleY = 1;
    _u4YSampleY = 1;
    if(_prJPEG_JFIF_HEADER->rSOF.u1NumComponents > 1)
    {
        _u4XSampleCb = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1HSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1HSampFactor;
        _u4YSampleCb = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[1].u1VSampFactor;
    }
    if(_prJPEG_JFIF_HEADER->rSOF.u1NumComponents > 2)
    {
        _u4XSampleCr = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1HSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1HSampFactor;
        _u4YSampleCr = _prJPEG_JFIF_HEADER->rSOF.arSofComp[0].u1VSampFactor/_prJPEG_JFIF_HEADER->rSOF.arSofComp[2].u1VSampFactor;
    }

    _u4ColorBufY = (UINT32)pvOutBuffer[0];
    _u4ColorBufCb = (UINT32)pvOutBuffer[1];
    _u4ColorBufCr = (UINT32)pvOutBuffer[2];

    _u4ColorBufY2 = (UINT32)pvOutBuffer[6];
    _u4ColorBufCb2 = (UINT32)pvOutBuffer[7];
    _u4ColorBufCr2 = (UINT32)pvOutBuffer[8];

    _i4JpegFileSize = (INT32)_prJPEG_JFIF_HEADER->u4ImageByteSize;
    _EmuJpgFifo = (BYTE *)_prJPEG_JFIF_HEADER->pu1JpegImage;
    
    i4JDEC_HAL_Set_Buf(0, pvCoefBuffer, pvNzBuffer);

    i4Ret = vJpegDecode(0,eDecodingMode,_u4ColorBufY,_u4ColorBufCb,_u4ColorBufCr,_u4ColorBufY2,_u4ColorBufCb2,_u4ColorBufCr2);
//                                                           _u4FrameBufY,_u4FrameBufC,
    //                                                       JDEC_COLOR_BUF_WIDTH,JDEC_COLOR_BUF_HEIGHT);

    i4JDEC_HAL_Unset_Buf(0);

    return i4Ret;
}

