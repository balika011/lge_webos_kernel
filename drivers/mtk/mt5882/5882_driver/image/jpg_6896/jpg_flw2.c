/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/

#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_bim.h"
#include "jpg_common.h"
#include "jpg_if.h"
#include "jpg_jfif.h"
#include "jpg_hw.h"
#include "jpg_mem.h"

#include "jpg_flw2.h"
#include "jpg_debug.h"
#include "osd_if.h"
#include "pmx_if.h"
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
#include "fbm_drvif.h"

#ifdef JPEG_VLD_V3
#ifdef VLD0_BASE
#undef VLD0_BASE
#define VLD0_BASE   JPG_VLD_BASE
#endif
#endif

typedef struct
{
   UINT32 au4CompBlock[4];
   UINT32 u4McuWidth;
   UINT32 u4McuHeight;
   UINT32 u4MaxHFactor;
   UINT32 u4MaxVFactor;
   UINT32 u4ColorBuf[6];
   UINT32 u4HufTable[2];
   UINT32 u4RowSize[4];
   UINT32 u4RzWorkBuf;
   UINT32 u4DataAddr;
}MJPEG_DEC_PARAM;

typedef struct
{
   HANDLE_T hDevice;
   JPEG_JFIF_HEADER_T rJpgHeader;
   JPEG_VLDJPG_DATA_T rJpgHw;
   PFN_MJPG_NOTIFY fpDevCb;
   MJPG_OUTPARAM_T *prOutParam;
   MJPG_INPARAM_T *prInParam;
   MJPEG_DEC_PARAM rDecParam;
   UINT32 u4WorkBuffer;
   UINT32 u4WorkBufSize;
   UINT32 u4DataBuffer;
   UINT32 u4DataBufSize;
   UINT32 u4AsyncMode;
}MJPEG_DECODER;
#ifdef JPEG_AUTO_TEST
extern INT32 _JpgVerifyCmdGetParam(INT32 i4ParamType, INT32 Param);
extern INT32 _JpgVerifyCmdSetParam(INT32 i4ParamType,INT32 Param);
#endif
static VOID _JDEC_SetRzV(RZ_JPG_SCL_PARAM_SET_T *prJpgParam, UINT32 *pu4RowHeight);

#ifdef PHOTO_EMP_DDI_CONFIG
#define JPG_COLLECT_LIMIT (1280 * 1024)//(1920 * 1440)
#define JPG_COLLECT_WIDTH_LIMIT 10000 //collect mode buffer limit
#else
#define JPG_COLLECT_LIMIT (1024 * 1024)
#define JPG_COLLECT_WIDTH_LIMIT 10000 //collect mode buffer limit
#endif

static const UINT8 _JpgBitsDcLuminance[17] =
{ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static const UINT8 _JpgValDcLuminance[] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static const UINT8 _JpgBitsDcChrominance[17] =
{ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static const UINT8 _JpgValDcChrominance[] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static const UINT8 _JpgBitsAcLuminance[17] =
{ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static const UINT8 _JpgValAcLuminance[] =
{ 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
  0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
  0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
  0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
  0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
  0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
  0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
  0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
  0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
  0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
  0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
  0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
  0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
  0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
  0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
  0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
  0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
  0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
  0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
  0xf9, 0xfa 
};

static const UINT8 _JpgBitsAcChrominance[17] =
{ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };

static const UINT8 _JpgValAcChrominance[] =
{ 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
  0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
  0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
  0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
  0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
  0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
  0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
  0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
  0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
  0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
  0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
  0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
  0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
  0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
  0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
  0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
  0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
  0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
  0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
  0xf9, 0xfa 
};
#ifdef __MODEL_slt__
static UINT32 u4BytePerPixel[] = {0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 1, 1, 1, 2, 0}; //left shift value
#endif

//static INT32 _i4JpgDecMode;
//static void  _jpg_SetDecMode5391(INT32 i4DecMode);
//static INT32 _jpg_GetDecMode5391(void);
#ifdef JPEG_AUTO_TEST
#define JPGHW_TIME_PROFILE
JPEG_PARTIAL_OUTPUT_WINDOW_T	_rJpgPartialWindow;
#endif

//#define MJPEG_VIDEOMODE_ENABLE

#ifdef JPGHW_TIME_PROFILE
static HAL_TIME_T _rDecStart, _rRZStart;
static HAL_TIME_T _rDecEnd, _rRZEnd;
static HAL_TIME_T _rDecAcc, _rRZAcc;
static HAL_TIME_T _rDecdif, _rRZdif;
static HAL_TIME_T _rHwDecStart, _rHwDecEnd;
static HAL_TIME_T _rHwDecDif, _rRZRowDif;
static HAL_TIME_T  _rRZRowStart, _rRZRowEnd;
#endif
static UINT32 _u4JpgRzTempBuf;
static IMG_LIB_INFO_T *prJpgImg;
static ENUM_IMG_ROTATE_PHASE eJpgRotateDegree;
static UINT32 _u4JPGMode = 0;
static ENUM_IMG_BUFFER_TYPE _eJPGMemClass = LINEAR_IN_LINEAR_OUT;
static INT32 _jpg_VideoOut5391Drv(JPEGHANDLE hFlow, UINT32 u4RowIdx, 
     UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr);

static INT32 _jpg_RowRz5391Drv(JPEGHANDLE hFlow, UINT32 u4RowIdx, 
     UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr);
#ifdef JPEG_AUTO_TEST
static INT32 _jpg_Flow			(JPEGHANDLE	hFlow);
static INT32 _jpg_Row			(JPEGHANDLE	hFlow, UINT32 u4Row);
#endif
static INT32 _jpg_Glob			(JPEGHANDLE	hFlow);
static INT32 _jpg_Scan			(JPEGHANDLE	hFlow, INT32 i4Scan);
static INT32 _jpg_Misc			(JPEGHANDLE	hFlow);
static BOOL _fgJpgFlw2Stop = FALSE;
#if 0
static UCHAR _acRange[900] = {0};
static INT32 _ai4RR[1024];
static INT32 _ai4BB[1024];
static INT32 _ai4RG[1024];
static INT32 _ai4BG[1024];
#endif
//static UINT16 _au2GammaR[256], _au2GammaG[256], _au2GammaB[256];
extern UINT32 u4GetProgressiveCurScan(void);

static UINT32 _u4YTgWidth = 512;
static UINT32 _u4YTgHeight = 256;
UINT32 _JPG_GetYTgW(void)
{
    return _u4YTgWidth;
}

UINT32 _JPG_GetYTgH(void)
{
    return _u4YTgHeight;
}

void _JPG_SetYTgW(UINT32 u4Width)
{
    _u4YTgWidth = u4Width;
}

void _JPG_SetYTgH(UINT32 u4Height)
{
    _u4YTgHeight = u4Height;
}
#if 0
void _JPG_SetSwGamma(BOOL fgEnable)
{
}

void JDEC_BuildTable(void)
{
    INT32 i4I, i4X;    
    UCHAR *ucTable = _acRange;   
    static BOOL fgInit = FALSE;

    if (!fgInit)
    {
        for (i4I = 0, i4X = -128; i4I <= 255; i4I++, i4X++)    
        {        
            _ai4RR[i4I] = (91882 * i4X + 32768) >> 16;        
            _ai4BB[i4I] = (116130 * i4X + 32768) >> 16;        
            _ai4RG[i4I] = -46802 * i4X;        
            _ai4BG[i4I] = (-22554 * i4X) + 32768;    
        }
        ucTable += 256;    
        for (i4I = 0; i4I < 256; i4I++)    
        {        
            ucTable[i4I] = i4I;    
        }    
        ucTable += 128;    
        for (i4I = 128; i4I < 512; i4I++)    
        {        
            ucTable[i4I] = 255;    
        }
        fgInit = TRUE;
    }
    //vDrvGetOSDGammaCurve(_au2GammaR, _au2GammaG, _au2GammaB);
}
#endif
/* base	jpeg decoding flow function	*/
static INT32 _jpg_Glob(JPEGHANDLE hFlow)
{
	/*
		1. set progressive flag
		2. set glob	n comp
		3. set h v factor of each comp
		4. set picture width hieght
		5. set huff	table, dc and ac
		6. if progressive flag,	set	nz buffer
		7. set q table
		8. set restart interval
		9. validate	glob data
	*/

	INT32 i4Ret	= (INT32)E_JPG_OK;
	UINT8 u1Comp;
	UINT32 u4Count;

	UINT32 u4MaxWidth;
	UINT32 u4MaxHeight;
	
	ASSERT(hFlow);

	/* set progressive flag	*/
	if ((JPGHDR->rSOF.eJpegFormat ==	E_JPG_BASELINE)||
		(JPGHDR->rSOF.eJpegFormat ==E_JPG_EX_SEQ_HUFFMAN))
	{
		TRY2( VLDJPG_SetGlobProgressive(H_JPGHW,	FALSE) );
		JPGMEMBS->fgProgScan = FALSE;
	}
	else if	(JPGHDR->rSOF.eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
	{
		TRY2( VLDJPG_SetGlobProgressive(H_JPGHW,	TRUE) );
		JPGMEMBS->fgProgScan = TRUE;
		//TRY2( JFIF_ParseProgScan(H_JPGHDR) );
//             JPGMEM->pfnClean(H_JPGMEM);
//             return -(INT32)E_JPG_ERR_UNSUPPORT;		
	}
	else
	{
	        Printf(" %s Format %d Unsupport\n", __FUNCTION__,JPGHDR->rSOF.eJpegFormat);
            JPGMEM->pfnClean(H_JPGMEM);
            return -(INT32)E_JPG_ERR_UNSUPPORT;
	}

	/*
		set	jpeg mem before	decode flow
			UINT32	u4Width;
			UINT32	u4Height;
	*/
	JPGMEMBS->u4Width	= (UINT32)JPGHDR->rSOF.u2ImageWidth;
	JPGMEMBS->u4Height	= (UINT32)JPGHDR->rSOF.u2ImageHeight;

	/* set num of components in	the	picture	*/
	TRY2( VLDJPG_SetGlobNumComp(H_JPGHW,	JPGHDR->rSOF.u1NumComponents) );

	/* set h v factor of each component	*/
	u4MaxWidth = 0;
	u4MaxHeight	= 0;
	for	(u1Comp	= 0; u1Comp	< JPGHDR->rSOF.u1NumComponents;	u1Comp++)
	{
		UINT8 u1FactorH;
		UINT8 u1FactorV;
		if (u1Comp >= (UINT8)3)
		{
		    return -(INT32)E_JPG_ERR_UNSUPPORT;
		}
		u1FactorH =	JPGHDR->rSOF.arSofComp[u1Comp].u1HSampFactor;
		u1FactorV =	JPGHDR->rSOF.arSofComp[u1Comp].u1VSampFactor;
		/* set hw decoder object */
		TRY2( VLDJPG_SetGlobBlkWidth(H_JPGHW, u1Comp, u1FactorH)	);
		TRY2( VLDJPG_SetGlobBlkHeight(H_JPGHW, u1Comp, u1FactorV) );
		u4MaxWidth = MAX(u4MaxWidth, (UINT32)u1FactorH);
		u4MaxHeight	= MAX(u4MaxHeight, (UINT32)u1FactorV);
		/* set jpg mem object */
		JPGMEMBS->aau4CompFactor[u1Comp][HORI] = u1FactorH;
		JPGMEMBS->aau4CompFactor[u1Comp][VERT] = u1FactorV;
		JPGMEMBS->au4CompBlock[u1Comp] = u1FactorH * u1FactorV;
        }
    /* add for motion jpeg*/
    if  (_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
    {
             if (!(((JPGMEMBS->aau4CompFactor[0][HORI]  == 2)&&(JPGMEMBS->aau4CompFactor[1][HORI] == 1)) && 
    	(((JPGMEMBS->aau4CompFactor[0][VERT] == 2)&&(JPGMEMBS->aau4CompFactor[1][VERT] == 1)) //420 format
    	|| ((JPGMEMBS->aau4CompFactor[0][VERT] == 1)&&(JPGMEMBS->aau4CompFactor[1][VERT] == 1)))))
                {
                   JDEC_SetDecodeMode(E_JPG_DEC_MODE_BASELINE_PIC_RZ,_eJPGMemClass);
                }
    }

	if (JPGHDR->rSOF.u1NumComponents == 1)
	{
	    JPGMEMBS->aau4CompFactor[1][HORI] = 0;
		JPGMEMBS->aau4CompFactor[1][VERT] = 0;
		JPGMEMBS->au4CompBlock[1] = 0;
        JPGMEMBS->aau4CompFactor[2][HORI] = 0;
		JPGMEMBS->aau4CompFactor[2][VERT] = 0;
		JPGMEMBS->au4CompBlock[2] = 0;
	}
	JPGMEMBS->au4MaxFactor[HORI] = u4MaxWidth;
	JPGMEMBS->au4MaxFactor[VERT] = u4MaxHeight;

	/* set picture width and height	*/
	TRY2( VLDJPG_SetGlobPicWidth(H_JPGHW, (UINT32)JPGHDR->rSOF.u2ImageWidth)	);
	TRY2( VLDJPG_SetGlobPicHeight(H_JPGHW, (UINT32)JPGHDR->rSOF.u2ImageHeight) );

	/*
	  set huff table / q table in baseline mode	or first scan,
	  other	scan of	progressive	mode should	set	in scan
	  and this condition is always true :
		if (i4Scan == 0	|| (! JPGHW->fgProgressive))
	  actually,	these code can remove safely. the reason to	leave'em
	  here is just for flow	compatibility to dc's original code
	*/

	
	if (JPGHDR->rDHT.u4NumDcTbl == 0)
	{
	    x_memcpy((void *)JPGHDR->rDHT.arDcTbl[0].au1Bits, 
	            (void *)_JpgBitsDcLuminance, (UINT32)sizeof(_JpgBitsDcLuminance));
   	    x_memcpy((void *)JPGHDR->rDHT.arDcTbl[0].au1HuffVal, 
	            (void *)_JpgValDcLuminance, (UINT32)sizeof(_JpgValDcLuminance));
   	    x_memcpy((void *)JPGHDR->rDHT.arDcTbl[1].au1Bits, 
	            (void *)_JpgBitsDcChrominance, (UINT32)sizeof(_JpgBitsDcChrominance));
	    x_memcpy((void *)JPGHDR->rDHT.arDcTbl[1].au1HuffVal, 
	            (void *)_JpgValDcChrominance, (UINT32)sizeof(_JpgValDcChrominance));

	    x_memcpy((void *)JPGHDR->rDHT.arAcTbl[0].au1Bits, 
	            (void *)_JpgBitsAcLuminance, (UINT32)sizeof(_JpgBitsAcLuminance));
   	    x_memcpy((void *)JPGHDR->rDHT.arAcTbl[0].au1HuffVal, 
	            (void *)_JpgValAcLuminance, (UINT32)sizeof(_JpgValAcLuminance));
   	    x_memcpy((void *)JPGHDR->rDHT.arAcTbl[1].au1Bits, 
	            (void *)_JpgBitsAcChrominance, (UINT32)sizeof(_JpgBitsAcChrominance));
	    x_memcpy((void *)JPGHDR->rDHT.arAcTbl[1].au1HuffVal, 
	            (void *)_JpgValAcChrominance, (UINT32)sizeof(_JpgValAcChrominance));
	    
        JPGHDR->rDHT.fgDcTblLoaded = (UINT32)0x3;
        JPGHDR->rDHT.fgAcTblLoaded = (UINT32)0x3;
	}

	/* set huff	table */
	for	(u4Count = 0; u4Count < ((UINT32)D_MAX_JPEG_HUFF_TBL)/*JPGHDR->rDHT.u4NumDcTbl*/; u4Count++)
	{
		if (JPGHDR->rDHT.fgDcTblLoaded & D_BIT(u4Count))
		{
	        //LOG(3,"Derive DC Huff Tbl %d\n", u4Count);
			TRY2( VLDJPG_SetGlobDcHuffTbl(H_JPGHW, (UINT8)u4Count,
										 JPGHDR->rDHT.arDcTbl[u4Count].au1Bits,
										 JPGHDR->rDHT.arDcTbl[u4Count].au1HuffVal) );
		}
		if (JPGHDR->rDHT.fgAcTblLoaded & D_BIT(u4Count))
		{
	        //LOG(3,"Derive AC Huff Tbl %d\n", u4Count);
			TRY2( VLDJPG_SetGlobAcHuffTbl(H_JPGHW, (UINT8)u4Count,
										 JPGHDR->rDHT.arAcTbl[u4Count].au1Bits,
										 JPGHDR->rDHT.arAcTbl[u4Count].au1HuffVal) );
		}
	}

	/* set q table */
	for	(u4Count = 0; u4Count < D_MAX_JPEG_QTBL/*(UINT32)JPGHDR->rDQT.u1NumQ*/; u4Count++)
	{
		TRY2( VLDJPG_SetGlobQtbl(H_JPGHW, (UINT8)u4Count, &JPGHDR->rDQT.aau1Qtbl[u4Count][0]) );
	}


	/* calculate mcu width-height and mcu size */
        if(u4MaxWidth == 4)
        {
            JPGMEMBS->u4McuWidth = ((JPGHW->u4PicWidth + 31) >>5);
        }
        else if (u4MaxWidth == 2)
        {
            JPGMEMBS->u4McuWidth = ((JPGHW->u4PicWidth + 15) >>4);
        }
        else
        {
            JPGMEMBS->u4McuWidth =((JPGHW->u4PicWidth +  7) >>	3);
        }
        if(u4MaxHeight == 4)
        {
            JPGMEMBS->u4McuHeight = ((JPGHW->u4PicHeight + 31) >>5);
        }
        else if (u4MaxHeight == 2)
        {
            JPGMEMBS->u4McuHeight = ((JPGHW->u4PicHeight + 15) >>4);
        }
        else
        {
            JPGMEMBS->u4McuHeight =((JPGHW->u4PicHeight +  7) >>3);
        }
        /*
	JPGMEMBS->u4McuWidth = (u4MaxWidth == 2) ?
						   ((JPGHW->u4PicWidth + 15) >>	4) :
						   ((JPGHW->u4PicWidth +  7) >>	3);
	JPGMEMBS->u4McuHeight =	(u4MaxHeight ==	2) ?
							((JPGHW->u4PicHeight + 15) >> 4) :
							((JPGHW->u4PicHeight +	7) >> 3);*/
	JPGMEMBS->u4McuCount = JPGMEMBS->u4McuWidth	* JPGMEMBS->u4McuHeight;

	/* allocate	all	buffers	we need	using mem object */
	JPGMEM->pfnInit(H_JPGMEM);

	TRY2( VLDJPG_SetRestartInterval(H_JPGHW,	(UINT32)(JPGHDR->u2RestartInterval)) );

	TRY2( VLDJPG_ValidateGlobData(H_JPGHW) );


	CATCH_ERROR()
	{
		if (i4Ret)
		{
			JPGMEM->pfnClean(H_JPGMEM);
		}
		return i4Ret;
	}
}

static INT32 _jpg_Scan(JPEGHANDLE hFlow, INT32 i4Scan)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	UINT32 u4Comp, u4Block,	u4Count;
	UINT32 u4ProgMode;

	ASSERT(hFlow);
	/*
		1. parsing scan	related	table
		2. set scan	data start
		3. set comp	in scan
		4. associate q tbl with	components,	dc/ac id with blocks
		5. set scan	alah
		6. set scan	sess
		7. set mcu row start and row end
		8. validate	scan data
	*/

	//BC add
	JPGHW->u4McuCount = 0;

	/* parsing scan	related	table */
	if (JPGHW->fgProgressive &&	JPGHDR->arSOS[i4Scan].pu1ScanTableStart	&& i4Scan)
	{
		/* set current header parsing to scan table	start */
		JPGHDR->pu1CurrRead	= JPGHDR->arSOS[i4Scan].pu1ScanTableStart;
		/* parsing header again	*/
		Printf("Parsing	progressive	scan DQT and DHT 0x%x\n", (UINT32)JPGHDR->pu1CurrRead);
		TRY2( JFIF_ParseProgMarker(H_JPGHDR, (UINT32)i4Scan) );
		Printf("Parsing	progressive	scan DQT and DHT --	done\n");

		/* set huff	table */
		for	(u4Count = 0; u4Count <	JPGHDR->rDHT.u4NumDcTbl; u4Count++)
		{
			if (JPGHDR->rDHT.fgDcTblLoaded & D_BIT(u4Count))
			{
				TRY2( VLDJPG_SetGlobDcHuffTbl(H_JPGHW, (UINT8)u4Count,
											 JPGHDR->rDHT.arDcTbl[u4Count].au1Bits,
											 JPGHDR->rDHT.arDcTbl[u4Count].au1HuffVal) );
			}
			if (JPGHDR->rDHT.fgAcTblLoaded & D_BIT(u4Count))
			{
				TRY2( VLDJPG_SetGlobAcHuffTbl(H_JPGHW, (UINT8)u4Count,
											 JPGHDR->rDHT.arAcTbl[u4Count].au1Bits,
											 JPGHDR->rDHT.arAcTbl[u4Count].au1HuffVal) );
			}
		}

		/* set q table */
		for	(u4Count = 0; ((u4Count < (UINT32)JPGHDR->rDQT.u1NumQ) && u4Count < D_MAX_JPEG_QTBL); u4Count++)
		{
			TRY2( VLDJPG_SetGlobQtbl(H_JPGHW, (UINT8)u4Count, &JPGHDR->rDQT.aau1Qtbl[u4Count][0]) );
		}
	}

	/* set data	start */
	TRY2( VLDJPG_SetScanData(H_JPGHW, JPGHDR->arSOS[i4Scan].pu1ScanDataStart) );

	/* set comp	in scan	*/
	TRY2( VLDJPG_SetScanCompInScan(H_JPGHW, JPGHDR->arSOS[i4Scan].u1CompInScan,
								  (UINT8*)JPGHDR->arSOS[i4Scan].au1CompId) );

	/* associate q tbl to comp,	dc/ac id to	block */
	for	((u4Comp = 0), (u4Block = 0);
		u4Comp < (UINT32)JPGHDR->arSOS[i4Scan].u1CompInScan;
		u4Comp++)
	{
	    if (u4Comp >= (UINT32)3)
	    {
	        return -(INT32)E_JPG_ERR_UNSUPPORT;
	    }
		TRY2( VLDJPG_SetScanCompQTbl(H_JPGHW, (UINT8)u4Comp,	JPGHDR->rSOF.arSofComp[u4Comp].u1QuantTblNo) );
		for	(u4Count = 0; u4Count <	JPGMEMBS->au4CompBlock[u4Comp];	u4Count++, u4Block++)
		{
			TRY2( VLDJPG_SetScanBlkDc(H_JPGHW, (UINT8)u4Block, JPGHDR->arSOS[i4Scan].au1DcId[u4Comp]) );
			TRY2( VLDJPG_SetScanBlkAc(H_JPGHW, (UINT8)u4Block, JPGHDR->arSOS[i4Scan].au1AcId[u4Comp]) );
		}
	}

	/* set huff	table in progressive mode, baseline	mode should	set	in glob	*/
	/* FIXME: remove 'if' line,	and	the	huff/q setup in	glob */
	if (JPGHW->fgProgressive &&	i4Scan)
	{
		/* set huff	table */
		for	(u4Count = 0; u4Count <	JPGHDR->rDHT.u4NumDcTbl; u4Count++)
		{
			if (JPGHDR->rDHT.fgDcTblLoaded & D_BIT(u4Count))
			{
				TRY2( VLDJPG_SetGlobDcHuffTbl(H_JPGHW, (UINT8)u4Count,
											 JPGHDR->rDHT.arDcTbl[u4Count].au1Bits,
											 JPGHDR->rDHT.arDcTbl[u4Count].au1HuffVal) );
			}
			if (JPGHDR->rDHT.fgAcTblLoaded & D_BIT(u4Count))
			{
				TRY2( VLDJPG_SetGlobAcHuffTbl(H_JPGHW, (UINT8)u4Count,
											 JPGHDR->rDHT.arAcTbl[u4Count].au1Bits,
											 JPGHDR->rDHT.arAcTbl[u4Count].au1HuffVal) );
			}
		}
		/* set q table */
		for	(u4Count = 0; ((u4Count < (UINT32)JPGHDR->rDQT.u1NumQ) && u4Count < D_MAX_JPEG_QTBL); u4Count++)
		{
			TRY2( VLDJPG_SetGlobQtbl(H_JPGHW, (UINT8)u4Count, &JPGHDR->rDQT.aau1Qtbl[u4Count][0]) );
		}
	}


	/* set al ah */
	TRY2( VLDJPG_SetScanAlAh(H_JPGHW, JPGHDR->arSOS[i4Scan].u1AhAl) );
	/* set se ss */
	TRY2( VLDJPG_SetScanSeSs(H_JPGHW, JPGHDR->arSOS[i4Scan].u1Se, JPGHDR->arSOS[i4Scan].u1Ss) );
	/* set progressive mode	: dc_first,	dc_refine, ac_first, ac_refine */

	if (JPGHW->fgProgressive)
	{
		if (JPGHDR->arSOS[i4Scan].u1AhAl & 0xf0)
		{
			u4ProgMode = (JPGHDR->arSOS[i4Scan].u1Ss) ?	(UINT32)D_JPEG_AC_REFINE : (UINT32)D_JPEG_DC_REFINE;
		}
		else
		{
			u4ProgMode = (JPGHDR->arSOS[i4Scan].u1Ss) ?	(UINT32)D_JPEG_AC_FIRST	: (UINT32)D_JPEG_DC_FIRST;
		}
	}
	else
	{
		u4ProgMode = D_JPEG_BASELINE;
#if defined(JPEG_VLD_V3)||defined(JPEG_VLD_V4)		
		VLDJPG_UseNewEngine();
        VLDJPG_SetErrorConcealment();
        VLDJPG_SetErrorDetection();
#endif
	}
	TRY2( VLDJPG_SetProgressiveMode((UINT8)u4ProgMode) );
	JPGHW->u4ProgMode =	u4ProgMode;

	/* set last	scan flag */
	TRY2( VLDJPG_SetScanLast(H_JPGHW,
							(((UINT32)i4Scan	== JPGHDR->au4CompLastScan[0]) ||
							 ((UINT32)i4Scan	== JPGHDR->au4CompLastScan[1]) ||
							 ((UINT32)i4Scan	== JPGHDR->au4CompLastScan[2]))));
	//i4Scan ==	((INT32)JPGHDR->u4NumScan -	1)

	/* mtk patent */
	TRY2( VLDJPG_SetScanMcuRowStart(H_JPGHW,	0) );
	TRY2( VLDJPG_SetScanMcuRowEnd(H_JPGHW, JPGMEMBS->u4McuHeight	- 1) );

	/* set nz buffer */
	if (JPGHW->fgProgressive)
	{
		TRY2( VLDJPG_SetGlobNonZeroBuf(H_JPGHW, (UINT8*)JPGMEMBS->apvNzBuffer[JPGHW->u1FirstCompInScan])	);
		//TRY2( VLDJPG_SetProgressiveNoneZeroHistory( (UINT8*)JPGMEMBS->apvNzBuffer[0] )	);
	}

	/* validate	scan data */
	TRY2( VLDJPG_ValidateScanData(H_JPGHW) );

	JPGHW->i4SavedEOBRUN = 0;

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

static INT32 _jpg_Misc(JPEGHANDLE hFlow)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;

	ASSERT(hFlow);

	TRY2( VLDJPG_SetMiscDefBlkAddr(H_JPGHW) );
	TRY2( VLDJPG_ValidateMiscData(H_JPGHW) );
	TRY2( VLDJPG_Switch2CleanBuffer() );

	CATCH_ERROR()
	{
		return i4Ret;
	}
}


static VOID _jpg_UpdateReadPtr(JPEGHANDLE hFlow)
{
    UINT32 u4NewRP;
    UINT32 u4UsedLen;
    			
    u4NewRP = IO_READ32(VLD0_BASE, 0xfc) - PHYSICAL((UINT32) JPGHDR->pu1RingBufStart);
    if (u4NewRP < JPGHDR->u4RingRP)
    {
        u4UsedLen = (JPGHDR->u4RingBufSize + u4NewRP) - JPGHDR->u4RingRP;
    }
    else
    {
        u4UsedLen = u4NewRP - JPGHDR->u4RingRP;
    }    			
    if(JPGHDR->u4RingData < u4UsedLen)
    {
        LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", JPGHDR->u4RingData, u4UsedLen);
    }
    else
    {
        JPGHDR->u4RingData -= u4UsedLen;
    }			
	JPGFLW->u4DecodedSize+=u4UsedLen;
    JPGHDR->u4RingRP = u4NewRP;		
}

#ifdef JPEG_AUTO_TEST

#ifdef IMGRZ_AUTO_TEST
extern INT32 RzEmuGetParam(INT32 i4ParamType,INT32 i4Param);
#endif
static INT32 _jpg_Row(JPEGHANDLE hFlow,	UINT32 u4Row)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	
	ASSERT(hFlow);

	TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4Row) );
	//TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, pu1Y,	au4RowSize[0]) );
	//TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, pu1U,	au4RowSize[1]) );
	//TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, pu1V,	au4RowSize[2]) );
	TRY2( VLDJPG_ValidateMcuRowData(H_JPGHW)	);

	CATCH_ERROR()
	{
		return i4Ret;
	}
}
static INT32 _jpg_Row5391(JPEGHANDLE hFlow,	UINT32 u4RowIdx, 
    UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr)
{
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;

    JMEM_MW_T *prMemMw = NULL;
    JPEG_OUTPUT_T	 *prMem;
    JPEG_FLOW_DATA_T *prFlow;
	JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
	
    UINT32 au4RowSize[3];
	INT32 i4Ret	= (INT32)E_JPG_OK;
    
	ASSERT(hFlow);
	ASSERT(apu1OutputAddr != NULL);

    VERIFY(VLDJPG_SetCurrMcuRowNum5391(u4RowIdx) == (INT32)E_HWJPG_OK);

    prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
    ASSERT(prMemYuv != NULL);

    prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
    ASSERT(prMemMw != NULL);

    prFlow = (JPEG_FLOW_DATA_T *)hFlow;
    prMem = prFlow->hJpegMem;

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);


    #ifdef JPEG_VLD_V4	

		JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);
    
		if (prMem->rParent.eJpegClass == E_JC_MEM_YUV)
		{
			au4RowSize[0] = prMemYuv->au4RowSize[0];
			au4RowSize[1] = prMemYuv->au4RowSize[1];
			au4RowSize[2] = prMemYuv->au4RowSize[2];
		}
		else
		{
			au4RowSize[0] = prMemMw->au4RowSize[0];
			au4RowSize[1] = prMemMw->au4RowSize[1];
			au4RowSize[2] = prMemMw->au4RowSize[2];
		}

		TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx) );

		// output bank0
		if(u4RowIdx%2==1)
		{
			/*
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0]+au4RowSize[0], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1]+au4RowSize[1], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2]+au4RowSize[2], au4RowSize[2]) );		
			*/
			// apu1OutputAddr[6 7 8] addr is one more row than apu1OutputAddr[0 1 2]
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );			
		}
		else
		{
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]) );
		}
		
		// output bank1
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );

		TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);

    #else
    
		if ((u4RowIdx % 2) == 0)
		{
			JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);
			
			if (prMem->rParent.eJpegClass == E_JC_MEM_YUV)
			{
				au4RowSize[0] = prMemYuv->au4RowSize[0];
				au4RowSize[1] = prMemYuv->au4RowSize[1];
				au4RowSize[2] = prMemYuv->au4RowSize[2];
			}
			else
			{
				au4RowSize[0] = prMemMw->au4RowSize[0];
				au4RowSize[1] = prMemMw->au4RowSize[1];
				au4RowSize[2] = prMemMw->au4RowSize[2];
			}

			TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx) );

			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]) );

			// output bank1
			TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );

			TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);
		}
    
    #endif
	// decode
	i4Ret = VLDJPG_DecodeOneMcuRow5391(H_JPGHW, u4RowIdx);

	// update read pointer and data size of ring buffer
	if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
	{
	    _jpg_UpdateReadPtr(hFlow);
        }

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

static INT32 _jpg_RowRz5391(JPEGHANDLE hFlow,	UINT32 u4RowIdx, 
    UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr)
{
    static UINT32 su4IsRzInited = FALSE;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    //static UINT32 su4RzYTgW, su4RzYTgH, su4RzCTgW, su4RzCTgH;
    static UINT32 su4RzYSrcL;
	UINT32 u4RzTagWidht=0,u4RzTagHeight=0;
	UINT32 u4BufferAddr=0;//u4BufferSize=0;
#ifdef IMGRZ_AUTO_TEST
	UINT32 *pImgrzOffsetData=NULL;
#endif
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;

    x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
	JMEM_YUV_LINEAR_T *prMemYuv = NULL;
	JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
	
    UINT32 au4RowSize[3];
	INT32 i4Ret	= (INT32)E_JPG_OK;
    
	ASSERT(hFlow);
	ASSERT(apu1OutputAddr != NULL);

    VERIFY(VLDJPG_SetCurrMcuRowNum5391(u4RowIdx) == (INT32)E_HWJPG_OK);

    prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
    ASSERT(prMemYuv != NULL);

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

   #ifdef JPEG_VLD_V4	

		JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);
    
			au4RowSize[0] = prMemYuv->au4RowSize[0];
			au4RowSize[1] = prMemYuv->au4RowSize[1];
			au4RowSize[2] = prMemYuv->au4RowSize[2];

		TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx) );

		// output bank0
		if(u4RowIdx%2==1)
		{
			/*
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0]+au4RowSize[0], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1]+au4RowSize[1], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2]+au4RowSize[2], au4RowSize[2]) );		
			*/
			// apu1OutputAddr[6 7 8] addr is one more row than apu1OutputAddr[0 1 2]
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );			
		}
		else
		{
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]) );
			TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]) );
		}
		
		// output bank1
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
		TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );

		TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);
        // for imgrz
        if (u4RowIdx == 0)
        {
            su4RzPrevYAddr  = (UINT32)apu1OutputAddr[0];
            su4RzPrevCbAddr = (UINT32)apu1OutputAddr[1];
            su4RzPrevCrAddr = (UINT32)apu1OutputAddr[2];
            if(_JpgVerifyCmdGetParam(21,0))
            {
				_rRZAcc.u4Seconds = 0;
				_rRZAcc.u4Micros  = 0;
            }
        }
        else
        {
	        su4RzPrevYAddr  = su4RzCurrYAddr;
	        su4RzPrevCbAddr = su4RzCurrCbAddr;
	        su4RzPrevCrAddr = su4RzCurrCrAddr;        
        }

		su4RzCurrYAddr  = (UINT32)apu1OutputAddr[0];
		su4RzCurrCbAddr = (UINT32)apu1OutputAddr[1];
		su4RzCurrCrAddr = (UINT32)apu1OutputAddr[2];
		
    #else
    if ((u4RowIdx % 2) == 0)
    {
    	JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);

    	au4RowSize[0] = prMemYuv->au4RowSize[0];
    	au4RowSize[1] = prMemYuv->au4RowSize[1];
    	au4RowSize[2] = prMemYuv->au4RowSize[2];

    	TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx) );
    	
    	TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]) );
    	TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]) );
    	TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]) );

        // output bank1
    	TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
    	TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
    	TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );
    	
    	TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);

        // for imgrz
        if (u4RowIdx == 0)
        {
            su4RzPrevYAddr  = (UINT32)apu1OutputAddr[0];
            su4RzPrevCbAddr = (UINT32)apu1OutputAddr[1];
            su4RzPrevCrAddr = (UINT32)apu1OutputAddr[2];
            if(_JpgVerifyCmdGetParam(21,0))
            {
				_rRZAcc.u4Seconds = 0;
				_rRZAcc.u4Micros  = 0;
            }
        }
        else
        {
            su4RzPrevYAddr  = su4RzCurrYAddr;
            su4RzPrevCbAddr = su4RzCurrCbAddr;
            su4RzPrevCrAddr = su4RzCurrCrAddr;
        }
        
        su4RzCurrYAddr  = (UINT32)apu1OutputAddr[0];
        su4RzCurrCbAddr = (UINT32)apu1OutputAddr[1];
        su4RzCurrCrAddr = (UINT32)apu1OutputAddr[2];
    }
    else
    {
        // for imgrz
        su4RzPrevYAddr  = su4RzCurrYAddr;
        su4RzPrevCbAddr = su4RzCurrCbAddr;
        su4RzPrevCrAddr = su4RzCurrCrAddr;

        su4RzCurrYAddr  = (UINT32)apu1OutputAddr[6];
        su4RzCurrCbAddr = (UINT32)apu1OutputAddr[7];
        su4RzCurrCrAddr = (UINT32)apu1OutputAddr[8];
    }
    #endif
	// decode
	i4Ret = VLDJPG_DecodeOneMcuRow5391(H_JPGHW, u4RowIdx);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
    su4RzYSrcW  = prMemYuv->au4RowWidth[0];
    su4RzCbSrcW = prMemYuv->au4RowWidth[1];
    su4RzCrSrcW = prMemYuv->au4RowWidth[2];
    su4RzYSrcH  = prMemYuv->au4RowHeight[0];
    su4RzCbSrcH = prMemYuv->au4RowHeight[1];
    su4RzCrSrcH = prMemYuv->au4RowHeight[2];
	_JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);	

    LOG(3,"RZ prev Y  addr = 0x%08x \n", su4RzPrevYAddr);
    LOG(3,"RZ prev Cb addr = 0x%08x \n", su4RzPrevCbAddr);
    LOG(3,"RZ prev Cr addr = 0x%08x \n", su4RzPrevCrAddr);
    LOG(3,"RZ curr Y  addr = 0x%08x \n", su4RzCurrYAddr);
    LOG(3,"RZ curr Cb addr = 0x%08x \n", su4RzCurrCbAddr);
    LOG(3,"RZ curr Cr addr = 0x%08x \n", su4RzCurrCrAddr);

#ifdef IMGRZ_AUTO_TEST
		RzEmuGetParam(1,(INT32)(&pImgrzOffsetData));
		rJpgParam.u4YSrcHOffset = pImgrzOffsetData[0];
		rJpgParam.u4YSrcVOffset = pImgrzOffsetData[1];
		rJpgParam.u4CbSrcHOffset = pImgrzOffsetData[2];
		rJpgParam.u4CbSrcVOffset = pImgrzOffsetData[3];
		rJpgParam.u4CrSrcHOffset = pImgrzOffsetData[2];
		rJpgParam.u4CrSrcVOffset = pImgrzOffsetData[3];
	    rJpgParam.u4YTgHOffset = pImgrzOffsetData[4];;
        rJpgParam.u4YTgVOffset = pImgrzOffsetData[5];
#else
	   rJpgParam.u4YSrcHOffset = 0;
	   rJpgParam.u4YSrcVOffset = 0;
	   rJpgParam.u4CbSrcHOffset = 0;
	   rJpgParam.u4CbSrcVOffset = 0;
	   rJpgParam.u4CrSrcHOffset = 0;
	   rJpgParam.u4CrSrcVOffset = 0;
	   rJpgParam.u4YTgHOffset = 0;
	   rJpgParam.u4YTgVOffset = 0;
#endif
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzYSrcL;
    rJpgParam.u4YSrcW = su4RzYSrcW;
    rJpgParam.u4YSrcH = (su4RzYSrcH * u4TotalRows);
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcW = su4RzCbSrcW;
    rJpgParam.u4CbSrcH = (su4RzCbSrcH * u4TotalRows);
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcW = su4RzCrSrcW;
    rJpgParam.u4CrSrcH = (su4RzCrSrcH * u4TotalRows);

	if(_JpgVerifyCmdGetParam(14,0))
	{
	    rJpgParam.u4CbSrcW = rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW;
        rJpgParam.u4CrSrcH = rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH;
        rJpgParam.fgGray = 1;
	}
	else
	{
	   rJpgParam.fgGray = 0;
	}
	rJpgParam.u4ColorComp = 0x7;
    rJpgParam.fgPreload = 0;
    rJpgParam.u4IsJpgSameaddr = 0;

    u4RzTagWidht=_JPG_GetYTgW();
    u4RzTagHeight=_JPG_GetYTgH();

   if(_JpgVerifyCmdGetParam(10,0)==1)
   { 
   
     rJpgParam.u4IsRsOut = 1;	  // block|raster based output
     rJpgParam.u4OutSwap = 0;
     rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
     rJpgParam.u4YTgCM=_JpgVerifyCmdGetParam(11,0);
     _JpgVerifyCmdGetParam(6,(INT32)(&u4BufferAddr));
     rJpgParam.u4YTgBase=u4BufferAddr;       // y
     rJpgParam.u4CTgBase = u4BufferAddr;//0x0e69c000; // cbcr
    
    if(rJpgParam.u4YTgCM==E_RZ_OSD_DIR_CM_AYCbCr8888)
    {
      rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
	  rJpgParam.u4YTgBufLen = u4RzTagWidht * 4;
	  rJpgParam.u4CTgBufLen = u4RzTagWidht * 4;
	  rJpgParam.u4CTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
    }
	else
	{
        rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
		rJpgParam.u4YTgBufLen = u4RzTagWidht*2;
		rJpgParam.u4CTgBufLen = u4RzTagWidht*2;
		rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset)>>1;
	}
	
	rJpgParam.u4YTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
	rJpgParam.u4CTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
    rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
    rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
    rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;

	{
      INT32 u4FilePrarm[4];
      u4FilePrarm[0]=1;
	  u4FilePrarm[1]=4;
	  u4FilePrarm[2]=rJpgParam.u4YTgBase;
	  u4FilePrarm[3]=rJpgParam.u4YTgBufLen * (rJpgParam.u4YTgH+rJpgParam.u4YTgVOffset);
	  _JpgVerifyCmdSetParam(2,(INT32)u4FilePrarm);
    }
 }
 else
 {
    rJpgParam.u4IsRsOut = _JpgVerifyCmdGetParam(12,0);	  // block|raster based output
    rJpgParam.u4OutSwap = _JpgVerifyCmdGetParam(17,0);
    rJpgParam.u4IsVdo2Osd = 0;	  // output in Y/C
	if(_JpgVerifyCmdGetParam(13,0)>=0)
	{
		 rJpgParam.u4OutMode=_JpgVerifyCmdGetParam(13,0);

		 if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_444)
		 {
			 rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			 rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			 rJpgParam.u4CTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			 rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
		 }
		 else if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_422)
		 {
			 rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			 rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			 rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
			 rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
		 }
		 else if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_420)
		 {
			 rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			 rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			 rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
			 rJpgParam.u4CTgH = (u4RzTagHeight-rJpgParam.u4YTgVOffset)>> 1;
		 }
	 }
	 else
	 {
		  if (prMemYuv->u4ColorFMT == E_JPEG_YUV420 || prMemYuv->u4ColorFMT == E_JPEG_YUV422V)
		  {
			 rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
			 rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			 rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			 rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
			 rJpgParam.u4CTgH = (u4RzTagHeight-rJpgParam.u4YTgVOffset) >> 1;
		  }
		 else if ((prMemYuv->u4ColorFMT == E_JPEG_YUV422) || (prMemYuv->u4ColorFMT == E_JPEG_YUV444))
		 {
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
			rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
			rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
			rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
		 }
		 else
		 {
			Printf("Can NOT support this jpg fmt = %u\n", prMemYuv->u4ColorFMT);
			ASSERT(0);
		 }

	}
	rJpgParam.u4YTgCM = 0;
    rJpgParam.u4YTgBufLen = u4RzTagWidht;
    rJpgParam.u4YTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
    rJpgParam.u4CTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
	_JpgVerifyCmdGetParam(6,(INT32)(&u4BufferAddr));
    rJpgParam.u4YTgBase = u4BufferAddr;       // y

    _JpgVerifyCmdGetParam(7,(INT32)(&u4BufferAddr));
	 rJpgParam.u4CTgBase = u4BufferAddr; // cbcr   
	 
     {
       INT32 u4FileParam[7]; 
	   UINT32 u4CVData=0;
	   u4FileParam[0]=2;
	   u4FileParam[1]=0;
       u4FileParam[2]=rJpgParam.u4YTgBase;
       u4FileParam[3]=rJpgParam.u4YTgBufLen * (rJpgParam.u4YTgH+rJpgParam.u4YTgVOffset);
	   u4FileParam[4]=1;
       u4FileParam[5]=rJpgParam.u4CTgBase;
	   u4CVData= rJpgParam.u4CTgH+ (rJpgParam.u4OutMode ==E_RZ_VDO_OUTMD_422 ? rJpgParam.u4YTgVOffset : (rJpgParam.u4YTgVOffset >>1));;
       u4FileParam[6]=(rJpgParam.u4CTgW+(rJpgParam.u4YTgHOffset>>1)) *u4CVData*2;
       _JpgVerifyCmdSetParam(2,(INT32)u4FileParam);
     }
   }

   LOG(3,"RZ target Y addr = 0x%08x \n", rJpgParam.u4YTgBase);
   LOG(3,"RZ target C addr = 0x%08x \n", rJpgParam.u4CTgBase);
    
    // for imgrz
    if (u4RowIdx == 0)
    {
        rJpgParam.u4IsFstBl = 1;
        rJpgParam.u4IsLstBl = 0;
    }
    else if (u4RowIdx == (u4TotalRows - 1))
    {
        rJpgParam.u4IsFstBl = 0;
        rJpgParam.u4IsLstBl = 1;        
    }
    else
    {
        rJpgParam.u4IsFstBl = 0;
        rJpgParam.u4IsLstBl = 0;
    }

    rJpgParam.u4IsJpgRzOn = 0;    // diable jpg-rz hand-off
    rJpgParam.u4JpgTempAddr = 0;
#ifdef IMGRZ_AUTO_TEST
	if(RzEmuGetParam(6,0) && su4IsRzInited && !(u4RowIdx%10))
	{
	   Printf("_IMGRZ_Break\n");
       IMGRZ_Break();    
       IMGRZ_OFF();
	   IMGRZ_Resume();
	}
#endif
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));

	if(_JpgVerifyCmdGetParam(21,0))
	{
		HAL_GetTime(&_rRZStart);
	}
    IMGRZ_Flush();
    IMGRZ_Wait();

    if(_JpgVerifyCmdGetParam(21,0))
    {
	   HAL_GetTime(&_rRZEnd);
	   HAL_GetDeltaTime(&_rRZdif, &_rRZStart, &_rRZEnd);
	   _rRZAcc.u4Seconds += _rRZdif.u4Seconds;
	   _rRZAcc.u4Micros  += _rRZdif.u4Micros;
	   if (_rRZAcc.u4Micros >= 1000000)
	   {
		   _rRZAcc.u4Seconds += _rRZAcc.u4Micros / 1000000;
		   _rRZAcc.u4Micros  %= 1000000;
	   }
    }

    if (u4RowIdx == (u4TotalRows - 1))
    {
        su4IsRzInited = FALSE;
        VLDJPG_SetPowerOff();
		if(_JpgVerifyCmdGetParam(21,0))
		{
			LOG(0,"RZ %u.%06u\n",_rRZAcc.u4Seconds, _rRZAcc.u4Micros);		
		}
    }

	// update read pointer and data size of ring buffer
	if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
	{
            _jpg_UpdateReadPtr(hFlow);
	}

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

#ifdef MT5368_JPG_PARTIAL_OUTPUT
UINT32 temp_x_offset=0;
UINT32 temp_y_offset=64;
UINT32 temp_w=0;
UINT32 temp_h=0;
#endif

static INT32 _jpg_PicRz5391(JPEGHANDLE hFlow,	UINT32 u4RowIdx, 
    UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr)
{
    static UINT32 su4IsRzInited = FALSE;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    //static UINT32 su4RzYTgW, su4RzYTgH, su4RzCTgW, su4RzCTgH;
    static UINT32 su4RzYSrcL;
	UINT32 u4BufferAddr=0;//,u4BufferSize=0;
#ifdef IMGRZ_AUTO_TEST
	UINT32 *pImgrzOffsetData=NULL;
#endif
#ifdef MT5368_JPG_PARTIAL_OUTPUT
	UINT32 *pJpgPartialData=NULL;
#endif
	UINT32 u4RzTagWidht=0,u4RzTagHeight=0;
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;

	x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
	JMEM_YUV_LINEAR_T *prMemYuv = NULL;
	JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
	
    UINT32 au4RowSize[3];
	INT32 i4Ret	= (INT32)E_JPG_OK;
    
	ASSERT(hFlow);
	ASSERT(apu1OutputAddr != NULL);

    VERIFY(VLDJPG_SetCurrMcuRowNum5391(u4RowIdx) == (INT32)E_HWJPG_OK);
    
    JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);

    prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
    ASSERT(prMemYuv != NULL);
    au4RowSize[0] = prMemYuv->au4RowSize[0];
    au4RowSize[1] = prMemYuv->au4RowSize[1];
    au4RowSize[2] = prMemYuv->au4RowSize[2];

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    TRY2( VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx) );
    	
    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]) );
    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]) );
    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]) );

    // output bank1
    TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]) );
    TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]) );
    TRY2( VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]) );


	#ifdef JPEG_VLD_V4
	//output skype video format
	if(VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
	{
       _JpgVerifyCmdGetParam(6,(INT32)(&u4BufferAddr));
       rJpgParam.u4YTgBase = u4BufferAddr;       // y
       _JpgVerifyCmdGetParam(7,(INT32)(&u4BufferAddr));
       rJpgParam.u4CTgBase = u4BufferAddr; // cbcr

	    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, (UINT8 *)rJpgParam.u4YTgBase, au4RowSize[0]) );
	    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, (UINT8 *)rJpgParam.u4CTgBase, au4RowSize[1]) );
	    TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, (UINT8 *)rJpgParam.u4CTgBase, au4RowSize[2]) );
       
	}
	#endif

	#ifdef MT5368_JPG_PARTIAL_OUTPUT
	_JpgVerifyCmdGetParam(23,(INT32)&pJpgPartialData);
	temp_x_offset = pJpgPartialData[0];
	temp_y_offset = pJpgPartialData[1];
	temp_w = pJpgPartialData[2];
	temp_h = pJpgPartialData[3];
	LOG(1,"temp_x_offset %d temp_y_offset %d temp_w %d temp_h %d\n"
		,temp_x_offset,temp_y_offset,temp_w,temp_h);
	_rJpgPartialWindow.u4Partial_x = temp_x_offset;
	_rJpgPartialWindow.u4Partial_y = temp_y_offset;
	if(temp_w==0)
		_rJpgPartialWindow.u4Partial_w = prJpgHw->u4PicWidth-temp_x_offset;
	else
		_rJpgPartialWindow.u4Partial_w = temp_w;

	if(temp_h==0)
		_rJpgPartialWindow.u4Partial_h = prJpgHw->u4PicHeight-_rJpgPartialWindow.u4Partial_y;
	else
		_rJpgPartialWindow.u4Partial_h = temp_h;
	
	_rJpgPartialWindow.u4Partial_skip_mcu_row = (_rJpgPartialWindow.u4Partial_y/prMemYuv->au4RowHeight[0]);
	TRY2( VLDJPG_SetSkipMcuRowNum(H_JPGHW, _rJpgPartialWindow.u4Partial_skip_mcu_row) );

	#endif
    	
    TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);

    // for imgrz
    su4RzCurrYAddr  = (UINT32)apu1OutputAddr[0];
    su4RzCurrCbAddr = (UINT32)apu1OutputAddr[1];
    su4RzCurrCrAddr = (UINT32)apu1OutputAddr[2];

    su4RzPrevYAddr  = (UINT32)apu1OutputAddr[6];
    su4RzPrevCbAddr = (UINT32)apu1OutputAddr[7];
    su4RzPrevCrAddr = (UINT32)apu1OutputAddr[8];

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
    su4RzYSrcW  = prMemYuv->au4RowWidth[0];
    su4RzCbSrcW = prMemYuv->au4RowWidth[1];
    su4RzCrSrcW = prMemYuv->au4RowWidth[2];
    su4RzYSrcH  = prMemYuv->au4RowHeight[0];
    su4RzCbSrcH = prMemYuv->au4RowHeight[1];
    su4RzCrSrcH = prMemYuv->au4RowHeight[2];
	_JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);

    LOG(3,"RZ prev Y  addr = 0x%08x \n", su4RzPrevYAddr);
    LOG(3,"RZ prev Cb addr = 0x%08x \n", su4RzPrevCbAddr);
    LOG(3,"RZ prev Cr addr = 0x%08x \n", su4RzPrevCrAddr);
    LOG(3,"RZ curr Y  addr = 0x%08x \n", su4RzCurrYAddr);
    LOG(3,"RZ curr Cb addr = 0x%08x \n", su4RzCurrCbAddr);
    LOG(3,"RZ curr Cr addr = 0x%08x \n", su4RzCurrCrAddr);

#ifdef IMGRZ_AUTO_TEST
    RzEmuGetParam(1,(INT32)&pImgrzOffsetData);
    rJpgParam.u4YSrcHOffset = pImgrzOffsetData[0];
    rJpgParam.u4YSrcVOffset = pImgrzOffsetData[1];
    rJpgParam.u4CbSrcHOffset = pImgrzOffsetData[2];
    rJpgParam.u4CbSrcVOffset = pImgrzOffsetData[3];
    rJpgParam.u4CrSrcHOffset = pImgrzOffsetData[2];
    rJpgParam.u4CrSrcVOffset = pImgrzOffsetData[3];
    rJpgParam.u4YTgHOffset = pImgrzOffsetData[4];
    rJpgParam.u4YTgVOffset = pImgrzOffsetData[5];
#else
   rJpgParam.u4YSrcHOffset = 0;
   rJpgParam.u4YSrcVOffset = 0;
   rJpgParam.u4CbSrcHOffset = 0;
   rJpgParam.u4CbSrcVOffset = 0;
   rJpgParam.u4CrSrcHOffset = 0;
   rJpgParam.u4CrSrcVOffset = 0;
   rJpgParam.u4YTgHOffset = 0;
   rJpgParam.u4YTgVOffset =0;

#endif
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzYSrcL;
    rJpgParam.u4YSrcW = su4RzYSrcW;
    rJpgParam.u4YSrcH = (su4RzYSrcH * u4TotalRows);
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcW = su4RzCbSrcW;
    rJpgParam.u4CbSrcH = (su4RzCbSrcH * u4TotalRows);
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcW = su4RzCrSrcW;
    rJpgParam.u4CrSrcH = (su4RzCrSrcH * u4TotalRows);

    _JpgVerifyCmdSetParam(22,prMemYuv->u4ColorFMT);

	if(_JpgVerifyCmdGetParam(14,0))
	{
	    rJpgParam.u4CbSrcW = rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW;
        rJpgParam.u4CrSrcH = rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH;
        rJpgParam.fgGray = 1;
	}
	else
	{
	   rJpgParam.fgGray = 0;
	}
	
    rJpgParam.u4ColorComp = 0x7;
    rJpgParam.fgPreload = 0;
    rJpgParam.u4IsJpgSameaddr = 0;
	u4RzTagWidht=_JPG_GetYTgW();
	u4RzTagHeight=_JPG_GetYTgH();
    if(VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
    {
        INT32 u4FileParam[7];
        u4FileParam[0]=2;
        u4FileParam[1]=0;
        u4FileParam[2]=rJpgParam.u4YTgBase;
        u4FileParam[3]=prJpgHw->u4CompPitchRow[0] * prJpgHw->u4PicHeight;
        
        u4FileParam[4]=1;
        u4FileParam[5]=rJpgParam.u4CTgBase;
        u4FileParam[6]=prJpgHw->u4CompPitchRow[0] * prJpgHw->u4PicHeight / 2;
        _JpgVerifyCmdSetParam(2,(INT32)u4FileParam);
    }
    else
    {
   if(_JpgVerifyCmdGetParam(10,0)==1)
   {
   	   rJpgParam.u4IsRsOut =1;// _JpgVerifyCmdGetParam(12,0);	   // block|raster based output
	   rJpgParam.u4OutSwap =0;// _JpgVerifyCmdGetParam(17,0);
       rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
	   rJpgParam.u4YTgCM=_JpgVerifyCmdGetParam(11,0);
       _JpgVerifyCmdGetParam(6,(INT32)(&u4BufferAddr));
       rJpgParam.u4YTgBase = u4BufferAddr;//0x0e19c000;       // y
       rJpgParam.u4CTgBase = u4BufferAddr;//0x0e69c000; // cbcr
    
       if(rJpgParam.u4YTgCM==E_RZ_OSD_DIR_CM_AYCbCr8888)
	   {
	      rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
	      rJpgParam.u4YTgBufLen = u4RzTagWidht * 4;
	      rJpgParam.u4CTgBufLen = u4RzTagWidht * 4;
	      rJpgParam.u4CTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
	   }
	   else
	   {
	      rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
	      rJpgParam.u4YTgBufLen = u4RzTagWidht*2;
	      rJpgParam.u4CTgBufLen = u4RzTagWidht*2;
		  rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
	   }
	
	   rJpgParam.u4YTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
	   rJpgParam.u4CTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
	   rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
	   rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
	   rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
       
	   {
          INT32 u4FileParam[4];
	      u4FileParam[0]=1;
	      u4FileParam[1]=4;
	      u4FileParam[2]=rJpgParam.u4YTgBase;
	      u4FileParam[3]=rJpgParam.u4YTgBufLen * (rJpgParam.u4YTgH+rJpgParam.u4YTgVOffset);
          _JpgVerifyCmdSetParam(2,(INT32)u4FileParam);
       }
   }
   else
   {

   	   rJpgParam.u4IsRsOut = _JpgVerifyCmdGetParam(12,0);	   // block|raster based output
	   rJpgParam.u4OutSwap = _JpgVerifyCmdGetParam(17,0);
       rJpgParam.u4IsVdo2Osd = 0;	  // output in osd format ?
	   if(_JpgVerifyCmdGetParam(13,0)>=0)
		{
			rJpgParam.u4OutMode=_JpgVerifyCmdGetParam(13,0);
			if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_444)
			{
				rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
				rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
				rJpgParam.u4CTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
				rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			}
			else if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_422)
			{
				rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
				rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
				rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
				rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
			}
			else if(rJpgParam.u4OutMode==E_RZ_VDO_OUTMD_420)
			{
				rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
				rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
				rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
				rJpgParam.u4CTgH = (u4RzTagHeight-rJpgParam.u4YTgVOffset)>> 1;
			}
		}
		else
		{
             if (prMemYuv->u4ColorFMT == E_JPEG_YUV420 || prMemYuv->u4ColorFMT == E_JPEG_YUV422V)
             {
                rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
                rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
                rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
                rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
                rJpgParam.u4CTgH = (u4RzTagHeight-rJpgParam.u4YTgVOffset) >> 1;
             }
             else if ((prMemYuv->u4ColorFMT == E_JPEG_YUV422) || (prMemYuv->u4ColorFMT == E_JPEG_YUV444))
             {
                rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
                rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
                rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
                rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
                rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
             }
             else
             {
                Printf("Can NOT support this jpg fmt = %u\n", prMemYuv->u4ColorFMT);
                ASSERT(0);
             }
	   }
		
	   rJpgParam.u4YTgCM = 0;
	   rJpgParam.u4YTgBufLen =u4RzTagWidht;
	   rJpgParam.u4YTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
	   rJpgParam.u4CTgBufLen=(rJpgParam.u4YTgBufLen+0xf)&(~0xf);
       _JpgVerifyCmdGetParam(6,(INT32)(&u4BufferAddr));
       rJpgParam.u4YTgBase = u4BufferAddr;       // y
       _JpgVerifyCmdGetParam(7,(INT32)(&u4BufferAddr));
       rJpgParam.u4CTgBase = u4BufferAddr; // cbcr
       
       {
          INT32 u4FileParam[7];
		  UINT32 u4CVData;
	      u4FileParam[0]=2;
	      u4FileParam[1]=0;
	      u4FileParam[2]=rJpgParam.u4YTgBase;
	      u4FileParam[3]=rJpgParam.u4YTgBufLen * (rJpgParam.u4YTgH+rJpgParam.u4YTgVOffset);
	
	      u4FileParam[4]=1;
	      u4FileParam[5]=rJpgParam.u4CTgBase;
		  u4CVData= rJpgParam.u4CTgH+ (rJpgParam.u4OutMode ==E_RZ_VDO_OUTMD_422 ? rJpgParam.u4YTgVOffset : (rJpgParam.u4YTgVOffset >>1));
	      u4FileParam[6]=(rJpgParam.u4CTgW+(rJpgParam.u4YTgHOffset>>1)) * u4CVData*2;
          _JpgVerifyCmdSetParam(2,(INT32)u4FileParam);      
       }
    }
    }

#ifdef MT5368_JPG_PARTIAL_OUTPUT
	 rJpgParam.fgJpgModePartialOutput =1;


    rJpgParam.u4YTgHOffset = 0;
    rJpgParam.u4YTgVOffset = 0;

    rJpgParam.u4YSrcHOffset = _rJpgPartialWindow.u4Partial_x;
    if(_rJpgPartialWindow.u4Partial_y==0)
    {
        rJpgParam.u4YSrcVOffset = 0;
    }
    else
    {
        rJpgParam.u4YSrcVOffset = (_rJpgPartialWindow.u4Partial_y%(_rJpgPartialWindow.u4Partial_skip_mcu_row*su4RzYSrcH));
    }
    rJpgParam.u4CbSrcHOffset = (_rJpgPartialWindow.u4Partial_x)/(su4RzYSrcW/su4RzCbSrcW);
    rJpgParam.u4CbSrcVOffset = (rJpgParam.u4YSrcVOffset)/(su4RzYSrcH/su4RzCbSrcH);
    rJpgParam.u4CrSrcHOffset = (_rJpgPartialWindow.u4Partial_x)/(su4RzYSrcW/su4RzCrSrcW);
    rJpgParam.u4CrSrcVOffset = (rJpgParam.u4YSrcVOffset)/(su4RzYSrcH/su4RzCrSrcH);   

	/*
    rJpgParam.u4YSrcW = su4RzYSrcW - rJpgParam.u4YSrcHOffset;
    rJpgParam.u4YSrcH = (su4RzYSrcH * u4TotalRows)-(_rJpgPartialWindow.u4Partial_skip_mcu_row*su4RzYSrcH);
    rJpgParam.u4CbSrcW = su4RzCbSrcW - rJpgParam.u4CbSrcHOffset;
    rJpgParam.u4CbSrcH = (su4RzCbSrcH * u4TotalRows) - (_rJpgPartialWindow.u4Partial_skip_mcu_row*su4RzCbSrcH);
    rJpgParam.u4CrSrcW = su4RzCrSrcW - rJpgParam.u4CrSrcHOffset;
    rJpgParam.u4CrSrcH = (su4RzCrSrcH * u4TotalRows) - (_rJpgPartialWindow.u4Partial_skip_mcu_row*su4RzCrSrcH);    
	*/
	
    rJpgParam.u4YSrcW = _rJpgPartialWindow.u4Partial_w;
    rJpgParam.u4YSrcH = _rJpgPartialWindow.u4Partial_h;
    rJpgParam.u4CbSrcW = _rJpgPartialWindow.u4Partial_w/(su4RzYSrcW/su4RzCbSrcW);
    rJpgParam.u4CbSrcH = _rJpgPartialWindow.u4Partial_h/(su4RzYSrcH/su4RzCbSrcH);
    rJpgParam.u4CrSrcW = _rJpgPartialWindow.u4Partial_w/(su4RzYSrcW/su4RzCrSrcW);
    rJpgParam.u4CrSrcH = _rJpgPartialWindow.u4Partial_h/(su4RzYSrcH/su4RzCrSrcH);

	//if skip mcu row num is odd, pls change the color buffer addr each other
	if(_rJpgPartialWindow.u4Partial_skip_mcu_row%2==1)
	{
	    rJpgParam.u4YSrcBase1 = su4RzPrevYAddr;     // y1
	    rJpgParam.u4YSrcBase2 = su4RzCurrYAddr;     // y2
	    rJpgParam.u4CbSrcBase1 = su4RzPrevCbAddr;    // cb1
	    rJpgParam.u4CbSrcBase2 = su4RzCurrCbAddr;    // cb2
	    rJpgParam.u4CrSrcBase1 = su4RzPrevCrAddr;    // cr1
	    rJpgParam.u4CrSrcBase2 = su4RzCurrCrAddr;    // cr2
	}

     if (prMemYuv->u4ColorFMT == E_JPEG_YUV420 || prMemYuv->u4ColorFMT == E_JPEG_YUV422V)
     {
        rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
        rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
        rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
        rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
        rJpgParam.u4CTgH = (u4RzTagHeight-rJpgParam.u4YTgVOffset) >> 1;
     }
     else if ((prMemYuv->u4ColorFMT == E_JPEG_YUV422) || (prMemYuv->u4ColorFMT == E_JPEG_YUV444))
     {
        rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
        rJpgParam.u4YTgW = u4RzTagWidht-rJpgParam.u4YTgHOffset;
        rJpgParam.u4YTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
        rJpgParam.u4CTgW = (u4RzTagWidht-rJpgParam.u4YTgHOffset) >> 1;
        rJpgParam.u4CTgH = u4RzTagHeight-rJpgParam.u4YTgVOffset;
     }
     else
     {
        Printf("Can NOT support this jpg fmt = %u\n", prMemYuv->u4ColorFMT);
        ASSERT(0);
     }   
#endif


    LOG(3,"RZ target Y addr = 0x%08x \n", rJpgParam.u4YTgBase);
    LOG(3,"RZ target C addr = 0x%08x \n", rJpgParam.u4CTgBase);

    if(VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
    {
		if(_JpgVerifyCmdGetParam(21,0))
			HAL_GetTime(&_rRZStart);
		
		i4Ret = VLDJPG_DecodeOnePic5391(H_JPGHW, u4RowIdx);
    }
    else
    {    
	    // for imgrz
	    if (u4RowIdx == 0)
	    {
	        rJpgParam.u4IsFstBl = 1;
	        rJpgParam.u4IsLstBl = 0;        
	    }

	    rJpgParam.u4IsJpgRzOn = 1;    // enable jpg-rz hand-off
	    rJpgParam.u4JpgTempAddr = 0;

	    if (su4IsRzInited == FALSE)
	    {
	        IMGRZ_ReInit();
	        su4IsRzInited = TRUE;
	    }
	    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
	    IMGRZ_Scale((void *)(&rJpgParam));
		
		if(_JpgVerifyCmdGetParam(21,0))
			HAL_GetTime(&_rRZStart);
	    IMGRZ_Flush();
		// decode

		i4Ret = VLDJPG_DecodeOnePic5391(H_JPGHW, u4RowIdx);

	    IMGRZ_Wait();  // wait imgrz resizer
	}
    
   if(_JpgVerifyCmdGetParam(21,0))
   {
	  HAL_GetTime(&_rRZEnd);
	  HAL_GetDeltaTime(&_rRZdif, &_rRZStart, &_rRZEnd);
	  LOG(0, "RZ %u.%06us\n",_rRZdif.u4Seconds, _rRZdif.u4Micros);
   }

	// update read pointer and data size of ring buffer
	if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
	{
            _jpg_UpdateReadPtr(hFlow);
	}
    su4IsRzInited = FALSE;
	VLDJPG_SetPowerOff();

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

static INT32 _jpg_Mcu(JPEGHANDLE hFlow,	UINT8 **apu1OutputAddr)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;

	enum
	{
		E_COMP_Y, E_COMP_U,	E_COMP_V
	};
	enum
	{
		E_IDCT_Y, E_IDCT_U,	E_IDCT_V,
		E_COEF_Y, E_COEF_U,	E_COEF_V
	};
	
	ASSERT(hFlow);
	ASSERT(apu1OutputAddr != NULL);
	
	JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);

	if (JPGMEMBS->fgProgScan)
	{
		if (!((JPGHW->u4ProgMode ==	D_JPEG_AC_FIRST) &&
				(JPGHW->i4SavedEOBRUN) &&
				(JPGHW->fgLastScan)))
		{
			// set eobrun
			TRY2( VLDJPG_SetEndOfBandRun((UINT32)JPGHW->i4SavedEOBRUN) );

			// set coef	buffer
			TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_Y, apu1OutputAddr[E_COEF_Y]) );
			TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_U, apu1OutputAddr[E_COEF_U]) );
			TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_V, apu1OutputAddr[E_COEF_V]) );
			if (JPGHW->fgLastScan)
			{
				// set idct	buffer
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_Y,	apu1OutputAddr[E_IDCT_Y]) );
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_U,	apu1OutputAddr[E_IDCT_U]) );
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_V,	apu1OutputAddr[E_IDCT_V]) );
			}
			else
			{
				// set idct	buffer
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_Y,	apu1OutputAddr[E_COEF_Y]) );
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_U,	apu1OutputAddr[E_COEF_U]) );
				TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_V,	apu1OutputAddr[E_COEF_V]) );
			}

			TRY2( VLDJPG_ValidateMcuData(H_JPGHW) );
			// decode
			TRY2( VLDJPG_DecodeOneMcu(H_JPGHW) );
			// get eobrun
			TRY2( VLDJPG_GetEndOfBandRun((UINT32*)&JPGHW->i4SavedEOBRUN)	);
		}
		else
		{
			Printf("AC_FIRST &&	EOBRUN && Last Scan\n");
			if ((JPGHW->u4ProgMode == D_JPEG_AC_FIRST) && (JPGHW->i4SavedEOBRUN))
			{
				JPGHW->i4SavedEOBRUN--;
				Printf("EOBRUN--\n");
			}
		}
	}
	else
	{
		// set idct	buffer only
		TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_Y,	apu1OutputAddr[E_IDCT_Y]) );
		TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_U,	apu1OutputAddr[E_IDCT_U]) );
		TRY2( VLDJPG_SetMcuOutBuf(H_JPGHW, E_COMP_V,	apu1OutputAddr[E_IDCT_V]) );
		// dup idct	buffer to coef buffer
		TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_Y, apu1OutputAddr[E_IDCT_Y]) );
		TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_U, apu1OutputAddr[E_IDCT_U]) );
		TRY2( VLDJPG_SetMcuCoefBuf(H_JPGHW, E_COMP_V, apu1OutputAddr[E_IDCT_V]) );

		TRY2( VLDJPG_ValidateMcuData(H_JPGHW) );

		// decode
		i4Ret = VLDJPG_DecodeOneMcu(H_JPGHW);
		// update read pointer and data size of ring buffer
		if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
		{
			_jpg_UpdateReadPtr(hFlow);
		}
	}


	CATCH_ERROR()
	{
		return i4Ret;
	}
}

#ifdef JPEG_AUTO_TEST
static UINT32 _u4ChkSum = 0;
UINT32 _Jpg_GetDecChkSum(void)
{
	return _u4ChkSum;
}
#endif
static INT32 _jpg_Flow(JPEGHANDLE hFlow)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	INT32 i4Scan, i4Row, i4Col;
	INT32 i4ScanTotal, i4Height, i4Width;
	BOOL fgLastScan	= FALSE;

    // comp	 0 1 2 
    // coef  3 4 5
    // comp1 6 7 8
	UINT8 *apu1OutputAddr[9];

	ASSERT(hFlow);
	
	TRY2( _jpg_Glob(hFlow) );

	// sync	ram	and	data
        //HalFlushInvalidateDCache();

	i4ScanTotal		= (INT32)JPGHDR->u4NumScan;
	i4Height		= (INT32)JPGMEMBS->u4McuHeight;
	i4Width			= (INT32)JPGMEMBS->u4McuWidth;

	for	(i4Scan	= 0; i4Scan	< i4ScanTotal; i4Scan++)
	{
		LOG(3,"\nscan %d\n", i4Scan);
		TRY2( _jpg_Scan(hFlow, i4Scan) );
		TRY2( _jpg_Misc(hFlow) );			// FIXME
		LOG(3,"cur	scan %d, total %d\n", i4Scan, i4ScanTotal);
		if (((UINT32)i4Scan == JPGHDR->au4CompLastScan[0]) ||
			((UINT32)i4Scan == JPGHDR->au4CompLastScan[1]) ||
			((UINT32)i4Scan == JPGHDR->au4CompLastScan[2]))
		{
			LOG(3,"last scan(%d) :	%d %d %d\n",
				   i4Scan,
				   JPGHDR->au4CompLastScan[0],
				   JPGHDR->au4CompLastScan[1],
				   JPGHDR->au4CompLastScan[2]);
			fgLastScan = TRUE;
		}
		else
		{
			fgLastScan = FALSE;
		}
		JPGHW->fgLastScan =	fgLastScan;

        if (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU)
        {
      		for	(i4Row = 0;	i4Row <	i4Height; i4Row++)
       		{
       			TRY2( _jpg_Row(hFlow, (UINT32)i4Row)	);
       			for	(i4Col = 0;	i4Col <	i4Width; i4Col++)
       			{
       				apu1OutputAddr[3] = NULL;
       				apu1OutputAddr[4] = NULL;
       				apu1OutputAddr[5] = NULL;				
       				TRY2( _jpg_Mcu(hFlow, apu1OutputAddr) );
       				if ((i4Row == 0) && (i4Col	== 0))
       				{
       					LOG(3,"- idct(%08x:%08x:%08x) coef(%08x:%08x:%08x)\n",
        						   (UINT32)apu1OutputAddr[0], (UINT32)apu1OutputAddr[1],
        						   (UINT32)apu1OutputAddr[2], (UINT32)apu1OutputAddr[3],
        						   (UINT32)apu1OutputAddr[4], (UINT32)apu1OutputAddr[5]);
       				}

       				if ((i4Row == 0) && (i4Col == 0))
       				{
       					TRY2( VLDJPG_Switch2KeepBuffer()	);
       				}
                    JPGMEM->pfnMcuFin(H_JPGMEM);
                }
                JPGMEM->pfnRowFin(H_JPGMEM);
            }
    		JPGMEM->pfnScanFin(H_JPGMEM);
        }
        else if (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU_ROW)
        {
      		for	(i4Row = 0;	i4Row <	i4Height; i4Row++)
       		{
       			apu1OutputAddr[3] = NULL;
       			apu1OutputAddr[4] = NULL;
    			apu1OutputAddr[5] = NULL;
       			TRY2( _jpg_Row5391(hFlow, (UINT32)i4Row, (UINT32)i4Width, (UINT32)i4Height, apu1OutputAddr)	);
                
    			if (i4Row == 0)
       			{
       				TRY2( VLDJPG_Switch2KeepBuffer()	);
       			}

                JPGMEM->pfnRowFin(H_JPGMEM);
            }
    		JPGMEM->pfnScanFin(H_JPGMEM);
        }
        else if (VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU_ROW_RZ)
        {
      		for	(i4Row = 0;	i4Row <	i4Height; i4Row++)
       		{
       			apu1OutputAddr[3] = NULL;
       			apu1OutputAddr[4] = NULL;
    			apu1OutputAddr[5] = NULL;
       			TRY2( _jpg_RowRz5391(hFlow, (UINT32)i4Row, (UINT32)i4Width, (UINT32)i4Height, apu1OutputAddr)	);
                
    			if (i4Row == 0)
       			{
       				TRY2( VLDJPG_Switch2KeepBuffer()	);
       			}

                JPGMEM->pfnRowFin(H_JPGMEM);
            }
    		JPGMEM->pfnScanFin(H_JPGMEM);
        }
        else if ((VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_RZ)||(VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
        {
            apu1OutputAddr[3] = NULL;
            apu1OutputAddr[4] = NULL;
            apu1OutputAddr[5] = NULL;

            i4Row = 0;
            TRY2( VLDJPG_Switch2KeepBuffer()	);
            
            TRY2( _jpg_PicRz5391(hFlow, (UINT32)i4Row, (UINT32)i4Width, (UINT32)i4Height, apu1OutputAddr)	);
        }
	}

	CATCH_ERROR()
	{
		#ifdef JPEG_VLD_V4
		#ifdef JPEG_AUTO_TEST
		_u4ChkSum = VLDJPG_ReadCKsum();
		#endif
		Printf("Jpgdec CKsum =[%x]\n", VLDJPG_ReadCKsum());
		Printf("Jpgdec CRC =[%x]\n", VLDJPG_ReadCRC());
		Printf("Error detection type=[%x]\n", VLDJPG_ReadErrorType());
		#endif
		return i4Ret;
	}
}

INT32 JDEC_DecodeYUV(JPEGHANDLE	hDec, void *pvImage, SIZE_T	zByteSize, JMEM_YUV_LINEAR_T **ppYuvImg)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	JPEG_BASE_CLASS_T	*prBase;
	JPEG_FLOW_DATA_T	*prFlow;
	JPEG_OUTPUT_T		*prMem;

	ASSERT(ppYuvImg);
	
	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}

	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass != E_JC_HW_FLOW) ||
		(prBase->zObjectSize != sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}

	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	TRY2( VLDJPG_New(prFlow->hJpegHw) );							// create hardware instance
	TRY2( JFIF_New(prFlow->hJpegHdr,	pvImage, zByteSize)	);		// parsing jpeg	header
	TRY2( JMEM_New(prFlow->hJpegMem,	E_JC_MEM_YUV) );			// create linear yuv memory


	prMem		= (JPEG_OUTPUT_T*) prFlow->hJpegMem;
	// start jpeg decode
	TRY2( _jpg_Flow(hDec) );
	// set return value
	*ppYuvImg =	(JMEM_YUV_LINEAR_T*)prMem->pvLocalData;

	CATCH_ERROR()
	{
		return i4Ret;
	}
}
#endif
static INT32 _jpg_BaselineStart(JPEGHANDLE hFlow)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	INT32 i4Scan;

	ASSERT(hFlow);
	TRY2( _jpg_Glob(hFlow) );

	// sync	ram	and	data

#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
    HalFlushInvalidateDCacheMultipleLine((UINT32)(JPGHDR->pu1RingBufStart), (UINT32)(JPGHDR->u4RingBufSize));
#else
    HalFlushInvalidateDCache();
#endif
	// setup ring buffer
	if(JPGHDR->fgRingBuffer)
	{
		JPGHW->fgRingBuffer		= TRUE;
		JPGHW->u4RingBufferSize	= JPGHDR->u4RingBufSize;
		JPGHW->pu1RingBufStart = JPGHDR->pu1RingBufStart;
	}
	else
	{
		JPGHW->fgRingBuffer		= FALSE;
		JPGHW->u4RingBufferSize	= 0;
	}

	i4Scan = 0;
	TRY2( _jpg_Scan(hFlow, i4Scan) );
	TRY2( _jpg_Misc(hFlow) );			// FIXME
	JPGHW->fgLastScan =	TRUE;

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

static INT32 _jpg_BaselineEnd(JPEGHANDLE hFlow)
{
	ASSERT(hFlow);
	
	JPGMEM->pfnScanFin(H_JPGMEM);
	return (INT32)E_JPG_OK;
}

static INT32 _jpg_BaselineRow(JPEGHANDLE hFlow,	UINT8 *apu1Addr[9])
{
    INT32 i4Ret	= (INT32)E_JPG_OK;
    INT32 i4CurRow = 0;

    ASSERT(hFlow);
    i4CurRow = JPGFLW->i4CurRow;
    LOG(7,"_jpg_BaselineRow u1MemClass = %d. _u4JPGMode = %d\n",prJpgImg->u1MemClass,_u4JPGMode);
    if((prJpgImg->u1MemClass == (UINT32)EXTERNAL_RING_IN_DOUBLE_OUT  || 
		prJpgImg->u1MemClass == (UINT32)RING_IN_DOUBLE_OUT_FOR_CAMERA) && 
       (_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
    {
        TRY2(_jpg_VideoOut5391Drv(hFlow, (UINT32)JPGFLW->i4CurRow, JPGMEMBS->u4McuWidth, JPGMEMBS->u4McuHeight, apu1Addr));
    }
    else
    {
        TRY2(_jpg_RowRz5391Drv(hFlow, (UINT32)JPGFLW->i4CurRow, JPGMEMBS->u4McuWidth, JPGMEMBS->u4McuHeight, apu1Addr));
    }
    if (i4CurRow == 0)
    {
        TRY2(VLDJPG_Switch2KeepBuffer());
    }
	
    JPGMEM->pfnRowFin(H_JPGMEM);
    JPGFLW->i4CurRow++;

    CATCH_ERROR()
    {
       return i4Ret;
    }
}

///////////////////////////////////////////////////////////////////////////////

JPEGHANDLE JDEC_AllocInstance(void)
{
	// allocate	memory,	memset it to zero, set its type	to JDEC
	void *pvMem;
	JPEG_BASE_CLASS_T *prBase;

	pvMem =	x_mem_alloc(sizeof(JPEG_FLOW_DATA_T));
	if (pvMem == NULL)
	{
	      LOG(0, "JDEC mem_alloc failed\n");
		return pvMem;
	}

	x_memset(pvMem,	0, sizeof(JPEG_FLOW_DATA_T));
	prBase = (JPEG_BASE_CLASS_T*)pvMem;
	prBase->zObjectSize	= sizeof(JPEG_FLOW_DATA_T);
	prBase->eJpegClass = E_JC_HW_FLOW;

	return (JPEGHANDLE)pvMem;
}

INT32 JDEC_New(JPEGHANDLE hDec)
{
	// release all handle if exists
	// re-new all handle
	JPEG_BASE_CLASS_T *prBase;
	JPEG_FLOW_DATA_T *prFlow;

	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}

	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass != E_JC_HW_FLOW) ||
		(prBase->zObjectSize	!= sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}

	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	if (prFlow->hJpegHdr)
	{
		JFIF_FreeInstance(prFlow->hJpegHdr);
	}
	prFlow->hJpegHdr = JFIF_AllocInstance();
	if(prFlow->hJpegHdr == NULL)
	{
		return -(INT32)E_JPG_ERR_ALLOC_MEM;
	}

	if (prFlow->hJpegHw)
	{
		VLDJPG_FreeInstance(prFlow->hJpegHw);
	}
	prFlow->hJpegHw	= VLDJPG_AllocInstance();
	if(prFlow->hJpegHw == NULL)
	{
		return -(INT32)E_JPG_ERR_ALLOC_MEM;
	}

	if (prFlow->hJpegMem)
	{
		JMEM_FreeInstance(prFlow->hJpegMem);
	}
	prFlow->hJpegMem = JMEM_AllocInstance();
	if(prFlow->hJpegMem == NULL)
	{
		return -(INT32)E_JPG_ERR_ALLOC_MEM;
	}
	
	return (INT32)E_JPG_OK;
}

void JDEC_FreeInstance(JPEGHANDLE hDec)
{
	// free	related	handle
	// clean its contain
	// free	this handle
	JPEG_BASE_CLASS_T *prBase;
	JPEG_FLOW_DATA_T *prFlow;

	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return;
	}

	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass != E_JC_HW_FLOW) ||
		(prBase->zObjectSize != sizeof(JPEG_FLOW_DATA_T)))
	{
		return;
	}

	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	if (prFlow->hJpegHdr)
	{
		JFIF_FreeInstance(prFlow->hJpegHdr);
		prFlow->hJpegHdr = (JPEGHANDLE)NULL;
	}

	if (prFlow->hJpegHw)
	{
		VLDJPG_FreeInstance(prFlow->hJpegHw);
		prFlow->hJpegHw = (JPEGHANDLE)NULL;
	}

	if (prFlow->hJpegMem)
	{
		JMEM_FreeInstance(prFlow->hJpegMem);
		prFlow->hJpegMem = (JPEGHANDLE)NULL;
	}

	x_mem_free((void*)hDec);
}


INT32 JDEC_MwGetMemLayout(JPEGHANDLE hDec, /*JMEM_MW_T*/void **pprMem)
{
	JPEG_BASE_CLASS_T	*prBase;
	JPEG_FLOW_DATA_T	*prFlow;
	JPEG_OUTPUT_T		*prMem;

	/* check return	pointer	*/
	if (pprMem == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	/* check input handle */
	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}

	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass != E_JC_HW_FLOW) ||
		(prBase->zObjectSize != sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	prMem =	(JPEG_OUTPUT_T*)prFlow->hJpegMem;
	/* validate	prMem */
	if ((prMem == NULL)	||
			//(prMem->rParent.eJpegClass	!= E_JC_MEM_MW)	||
			(prMem->rParent.zObjectSize	!= sizeof(JPEG_OUTPUT_T)))
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	/* return JMEM_MW_T	to caller */

	if(prMem->rParent.eJpegClass==E_JC_MEM_MW)
	{
	    *pprMem	= (JMEM_MW_T*)prMem->pvLocalData;
	}
      else
      	{
      	    *pprMem	= (JMEM_YUV_LINEAR_T*)prMem->pvLocalData;
      	}
	return (INT32)E_JPG_OK;
}

INT32 JDEC_MwSetRowBuffer(JPEGHANDLE hDec, void * const * ppvBuffer)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	JPEG_BASE_CLASS_T	*prBase;
	JPEG_FLOW_DATA_T	*prFlow;
	JPEG_OUTPUT_T		*prOut;

	void	*apvComp[9] = {0};

	/* check return	pointer	*/
	if (ppvBuffer == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	/* check input handle */
	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass		!= E_JC_HW_FLOW) ||
			(prBase->zObjectSize	!= sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prFlow	= (JPEG_FLOW_DATA_T*)hDec;
	prBase	= (JPEG_BASE_CLASS_T*)prFlow->hJpegMem;
	prOut	= (JPEG_OUTPUT_T*)prFlow->hJpegMem;
	if (prOut == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	if (//(prBase->eJpegClass		!= E_JC_MEM_MW)	||
			(prBase->zObjectSize	!= sizeof(JPEG_OUTPUT_T)))
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}

	/* a weird way of setting output buffer	*/
	x_memset(apvComp, 0, sizeof(void*) * 9);
	apvComp[3] = ppvBuffer[0];
	apvComp[4] = ppvBuffer[1];
	apvComp[5] = ppvBuffer[2];
#if 1
       //for 5391 imgrz
	apvComp[6] = ppvBuffer[3];
	apvComp[7] = ppvBuffer[4];
	apvComp[8] = ppvBuffer[5];
#endif	
	prOut->pfnGetOutput((JPEGHANDLE)prOut, apvComp);
	prOut->au4McuOffset[0] = 0;
	prOut->au4McuOffset[1] = 0;
	prOut->au4McuOffset[2] = 0;

	return i4Ret;
}

INT32 JDEC_MwFillBuffer(JPEGHANDLE hDec, void **ppvBuf,	UINT32 *pu4Size, UINT32 u4AddSize)
{
	UINT32 u4RingWP;
	UINT32 u4RingSpace;

	JPEG_BASE_CLASS_T	*prBase;
	JPEG_FLOW_DATA_T	*prFlow;
	JPEG_JFIF_HEADER_T	*prJfif;

	/* check return	pointer	*/
	if ((ppvBuf	== NULL) ||	(pu4Size ==	NULL))
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	/* check input handle */
	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass		!= E_JC_HW_FLOW) ||
			(prBase->zObjectSize	!= sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	prJfif = (JPEG_JFIF_HEADER_T*)prFlow->hJpegHdr;
	/* verify jfif header */
	prBase = (JPEG_BASE_CLASS_T*)prJfif;
	if ((prBase->eJpegClass		!= E_JC_JFIF_HEADER) ||
			(prBase->zObjectSize	!= sizeof(JPEG_JFIF_HEADER_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	
	u4RingWP = prJfif->u4RingRP	+ prJfif->u4RingData + u4AddSize;
	u4RingSpace = prJfif->u4RingBufSize - (prJfif->u4RingData + u4AddSize);

	if (u4RingWP >= prJfif->u4RingBufSize)
	{
		u4RingWP -= prJfif->u4RingBufSize;
	}
	
	//Printf("RP %08x WP %08x Data %d Space %d\n", prJfif->u4RingRP, u4RingWP, prJfif->u4RingData, u4RingSpace);
	
	if (u4RingSpace <= 64)
	{
		//read pointer overrun at last row is acceptable
		//but should takecare if happens when stream buffer is too
		//small for one single row
		*pu4Size = 0;
		*ppvBuf = (void*)prJfif->pu1RingBufStart;
		return (INT32)E_JPG_OK;
	}
	
	u4RingSpace -= 64;

	if ((u4RingWP + u4RingSpace) < prJfif->u4RingBufSize)
	{
		*pu4Size = u4RingSpace;
	}
	else
	{
		*pu4Size = prJfif->u4RingBufSize - u4RingWP;
	}
	
	*ppvBuf = (void*)((UINT32)prJfif->pu1RingBufStart + u4RingWP);

	/*
	if (*pu4Size >= 64)
	{
		*pu4Size -= 64;
	}
	*/

	return (INT32)E_JPG_OK;
}
#if 0
INT32 JDEC_MwGetJfif(JPEGHANDLE	hDec, JPEGHANDLE *phJfif)
{
	JPEG_BASE_CLASS_T	*prBase;
	JPEG_FLOW_DATA_T	*prFlow;

	/* check return	pointer	*/
	if (phJfif == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}
	/* check input handle */
	if (hDec ==	(JPEGHANDLE)NULL)
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}

	prBase = (JPEG_BASE_CLASS_T*)hDec;
	if ((prBase->eJpegClass		!= E_JC_HW_FLOW) ||
			(prBase->zObjectSize	!= sizeof(JPEG_FLOW_DATA_T)))
	{
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}
	prFlow = (JPEG_FLOW_DATA_T*)hDec;
	*phJfif	= prFlow->hJpegHdr;

	return (INT32)E_JPG_OK;
}
#endif
VOID JDEC_SetDecodeMode(ENUM_JPEG_DEC_MODE_T eMode, ENUM_IMG_BUFFER_TYPE eMemClass)
{
    _u4JPGMode = (UINT32)eMode;
    _eJPGMemClass = eMemClass;
    JPG_SetDecMode5391((INT32)eMode);
}


INT32 JDEC_MwDecodeSetup(JPEGHANDLE hFlow, ENUM_JPEG_CLASS_T eMemClass)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;
	
	ASSERT(hFlow);

	TRY2( VLDJPG_New(JPGFLW->hJpegHw) );
	TRY2( JMEM_New(JPGFLW->hJpegMem,	eMemClass));//E_JC_MEM_YUV, E_JC_MEM_MW
	if(eMemClass == E_JC_MEM_MW)
	{
            if(JPGHDR->fgParsed)
            {
	          TRY2( _jpg_Glob(hFlow) );
            }
	}
	CATCH_ERROR()
	{
		return i4Ret;
	}
}

void JDEC_MwSetRingBuf(JPEGHANDLE hFlow, UINT8 *pBs_start, UINT32 bufferSize)
{  
      JPEG_FLOW_DATA_T *prFlow;
      JPEG_JFIF_HEADER_T *prJfif;

      ASSERT(hFlow);
      prFlow = (JPEG_FLOW_DATA_T*)hFlow;
      prJfif = prFlow->hJpegHdr;
      
	prJfif->fgRingBuffer			= TRUE;
	prJfif->pu1RingBufStart			= (UINT8*)(((UINT32)pBs_start + 63) & (~63));
	prJfif->u4RingBufSize			= (bufferSize- ((UINT32)prJfif->pu1RingBufStart - (UINT32)pBs_start)) & (~63);
	prJfif->u4RingRP				= 0;
	//prJfif->u4RingData				= 0;
	if (_eJPGMemClass != (UINT32)EXTERNAL_RING_IN_DOUBLE_OUT)
	{
	    prJfif->u4RingData				= 0;
            x_memset((VOID *)prJfif->pu1RingBufStart, 0x0, prJfif->u4RingBufSize);
        }
}

void JDEC_FillBsBuf_End(JPEGHANDLE hFlow, UINT32 entry1_size, UINT32 entry2_size, BOOL beof)
{
      JPEG_FLOW_DATA_T *prFlow;
      JPEG_JFIF_HEADER_T *prJfif;
	UINT32 u4VldWptrOffset;
	CRIT_STATE_T crit;

      ASSERT(hFlow);
      prFlow = (JPEG_FLOW_DATA_T*)hFlow;
      prJfif = prFlow->hJpegHdr;
            //sync memory/cache
 #if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
     HalFlushInvalidateDCacheMultipleLine((UINT32)(prJfif->pu1RingBufStart), (UINT32)(prJfif->u4RingBufSize));
 #else
     HalFlushInvalidateDCache();
 #endif
      // update ringbuffer data length -----------------------------------
      crit = x_crit_start();	//protect u4RingData
      if (entry1_size > 0)
      {
          prJfif->u4RingData += entry1_size;
      }
      if (entry2_size >0)
      {
          prJfif->u4RingData += entry2_size;
      }

	prJfif->fgEOF = beof;	
	if(beof)
	{
		VLDJPG_SetRiscCtrlWP(0xFFFFFFFF);
	}
	else
	{
		u4VldWptrOffset = prJfif->u4RingRP + prJfif->u4RingData;
		u4VldWptrOffset = (u4VldWptrOffset >= prJfif->u4RingBufSize) ? (u4VldWptrOffset - prJfif->u4RingBufSize) : u4VldWptrOffset;
	
      		VLDJPG_SetRiscCtrlWP(PHYSICAL((UINT32)prJfif->pu1RingBufStart) + u4VldWptrOffset);
	}
	x_crit_end(crit);

    if(prJfif->u4RingData > prJfif->u4RingBufSize)
    {
      		LOG(7, "JDEC_FillBsBuf_End error occur\n");
    }

}

INT32 JDEC_MwHeaderParse(JPEGHANDLE hFlow, UINT8 *pBs_start, UINT32 bufferSize, JPEG_HEADER_INFO_T *jpgHdrInfo)
{    
	INT32 i4Ret = (INT32)E_JPG_OK, i4Idx;
      JPEG_FLOW_DATA_T *prFlow;
      JPEG_OUTPUT_T* prMem;
      JMEM_BASE_T* prMemBs;
      JMEM_MW_T* prMemMw;
      JMEM_YUV_LINEAR_T* prMemLinear;
      JPEG_JFIF_HEADER_T *prJfif;

	ASSERT(hFlow);
      ASSERT(jpgHdrInfo);

      prFlow = (JPEG_FLOW_DATA_T*)hFlow;
      prMem = prFlow->hJpegMem;
      prMemMw = prMem->pvLocalData;
      prMemLinear = prMem->pvLocalData;
      prMemBs = prMem->pvLocalData;
      prJfif = prFlow->hJpegHdr;
      
	//check buffer fillness, buffersize will depend on buffer fullness
	if(pBs_start)
	{
	    if ((_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ)||(_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
	    {
                bufferSize = (UINT32)(prJfif->u4RingBufSize - (pBs_start - prJfif->pu1RingBufStart));
	    }
          TRY2( JFIF_New(prFlow->hJpegHdr, pBs_start, bufferSize));  // parsing jpeg header
	}
	else
	{
          TRY2( JFIF_New(prFlow->hJpegHdr, prJfif->pu1RingBufStart, prJfif->u4RingBufSize));  // parsing jpeg header
	}
	TRY2( _jpg_BaselineStart(hFlow));

      //orig image information
	jpgHdrInfo->u4Width = prMemBs->u4Width;
	jpgHdrInfo->u4Height = prMemBs->u4Height;
	jpgHdrInfo->u4TotalRow = prMemBs->u4McuHeight;
	jpgHdrInfo->u4TotalCol = prMemBs->u4McuWidth;
	
	jpgHdrInfo->u4NumScan = JPGHDR->u4NumScan;
	jpgHdrInfo->ucPicStruct = prJfif->ucPicStruct;

      //color fmt information
      if(prMem->rParent.eJpegClass==E_JC_MEM_MW)
      {
          jpgHdrInfo->u4ColorFMT = prMemMw->u4ColorFMT;
      }
      else
      	{
          jpgHdrInfo->u4ColorFMT = prMemLinear->u4ColorFMT;
      	}
      //display row information
	for(i4Idx = 0; i4Idx<3; i4Idx++)
	{
	    if(prMem->rParent.eJpegClass==E_JC_MEM_MW)
	    {
	        jpgHdrInfo->au4RowWidth[i4Idx]= prMemMw->au4RowWidth[i4Idx];
	        jpgHdrInfo->au4RowHeight[i4Idx]= prMemMw->au4RowHeight[i4Idx];	        
	    }
          else
          {
	        jpgHdrInfo->au4RowWidth[i4Idx]= prMemLinear->au4RowWidth[i4Idx];
	        jpgHdrInfo->au4RowHeight[i4Idx]= prMemLinear->au4RowHeight[i4Idx];
          }

          if (jpgHdrInfo->au4RowWidth[i4Idx] & D_JPEG_DRAM_ALIGN_MASK)
          {
              // expand pitch to fit into dram aligned line
              jpgHdrInfo->au4RowWidth[i4Idx] &= (~D_JPEG_DRAM_ALIGN_MASK);
              jpgHdrInfo->au4RowWidth[i4Idx] += D_JPEG_DRAM_ALIGN_SIZE;
          }
          jpgHdrInfo->au4RowSize[i4Idx]= jpgHdrInfo->au4RowWidth[i4Idx]*jpgHdrInfo->au4RowHeight[i4Idx];
	}
	return i4Ret;
	CATCH_ERROR()
	{
		return i4Ret;
	}	
}

INT32 JDEC_MwRowStart(JPEGHANDLE hFlow)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;

	ASSERT(hFlow);
	
	if (JPGFLW->i4CurRow ==	0)
	{
		i4Ret =	_jpg_BaselineStart(hFlow);
	}

	return i4Ret;
}

INT32 JDEC_MwDecodeEnd(JPEGHANDLE hDec)
{
	INT32 i4Ret	= (INT32)E_JPG_OK;

	ASSERT(hDec);
	TRY2( _jpg_BaselineEnd(hDec) );
	JDEC_FreeInstance(hDec);
	return (INT32)E_JPG_OK;

	CATCH_ERROR()
	{
		return i4Ret;
	}
}

INT32 JDEC_MwDecodeRow(JPEGHANDLE hDec)
{
	INT32 i4Ret;
	UINT8 *apu1Addr[9];

	x_memset(apu1Addr, 0, 9	* sizeof(UINT8*));	
	i4Ret =	_jpg_BaselineRow(hDec, apu1Addr);

	if (i4Ret)
	{
		return i4Ret;//(INT32)E_JPG_ERR_GENERAL;
	}

	return (INT32)E_JPG_OK;
}

void JDEC_SetFileSize(JPEGHANDLE hFlow,UINT32 u4FileSize)
{
	JPEG_FLOW_DATA_T *prFlow;
	prFlow = (JPEG_FLOW_DATA_T*)hFlow;
        prFlow->u4FileSize=u4FileSize;
	return ;
}
UINT32 JDEC_GetProgress(JPEGHANDLE hFlow)
{
    UINT32 u4Progress;
    
    if (JPGHDR->rSOF.eJpegFormat == E_JPG_PROGRESSIVE_HUFFMAN)
    {
        u4Progress = (u4GetProgressiveCurScan() * 100) / 10;
    }
    else
    {
		u4Progress = (JPGMEMBS->u4McuHeight) ? ((VLDJPG_ReadCurMcuRow()) * 100)/JPGMEMBS->u4McuHeight : 0;
    }
    return u4Progress;
}
#ifdef THUMBNAIL_SHOW
void JDEC_HandleThumbnail(JPEGHANDLE hFlow)
{
    JPEG_FLOW_DATA_T *prFlow;
    JPEG_JFIF_HEADER_T *prJfif;
    IMGMETA_T rMeta;
    
    ASSERT(hFlow);
    prFlow = (JPEG_FLOW_DATA_T*)hFlow;
    prJfif = prFlow->hJpegHdr;
    
    JFIF_GetImgMetaData(&rMeta,prJfif->pu1RingBufStart,prJfif->u4RingBufSize);
     if (rMeta.pu1Thumbnail != NULL)
    {   
       x_memcpy((void*)prJfif->pu1RingBufStart,(void*)rMeta.pu1Thumbnail,rMeta.u4ThumbnailLength);
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
        HalFlushInvalidateDCacheMultipleLine((UINT32)prJfif->pu1RingBufStart, rMeta.u4ThumbnailLength);
#else
        HalFlushInvalidateDCache();
#endif
        prJfif->fgEOF = TRUE;
    }
     return;
}
#endif

#ifdef __MODEL_slt__
//for verification only
void *IMG_OSD_create(UINT32 dispColorMode)
{      
      DRV_OSD_T *prOsd=NULL;

      prOsd= x_mem_alloc(sizeof(DRV_OSD_T));
	  if(!prOsd)
	  {
	  	x_mem_free(prOsd);
	  	return NULL;
	  }
      x_memset(prOsd, 0, sizeof(DRV_OSD_T));
      prOsd->u4Left = 0;
      prOsd->u4Top = 0;
      // u4Width * bytes-per-pixel MUST align with 16 bytes
      prOsd->u4Width = 1360;//1366;
      prOsd->u4Height = 768;
      prOsd->u4ScaleWidth = prOsd->u4Width;
      prOsd->u4ScaleHeight = prOsd->u4Height;
      // pu1FrameBaseAddr MUST align with 16 bytes
      prOsd->pu1FrameBaseAddr =  VIRTUAL((UINT8 *)BSP_AllocAlignedDmaMemory((prOsd->u4Width*prOsd->u4Height)<<2, 16));
      if ((prOsd->pu1FrameBaseAddr == NULL)||(!prOsd))
      {
		//LOG(0, "Can not allocate memory of size %d bytes for OSD\n", (prOsd->u4Width*prOsd->u4Height)<<2);
		LOG(0, "Can not allocate memory of size ## bytes for OSD\n");
		x_mem_free(prOsd);
		return NULL;
      }

      if(0)//((dispColorMode==(UINT32)OSD_CM_ARGB8888_DIRECT32)||(dispColorMode==(UINT32)OSD_CM_ARGB4444_DIRECT16))
      {
          x_memset(prOsd->pu1FrameBaseAddr, 0, (prOsd->u4Width*prOsd->u4Height)<<u4BytePerPixel[dispColorMode]);
      }
      else //yuv
      {
          x_memset(prOsd->pu1FrameBaseAddr, 0x80, (prOsd->u4Width*prOsd->u4Height)<<u4BytePerPixel[dispColorMode]);      
      }
      #if 0
      TRY2(OSD_Reset());
      rColorMode = (OSD_COLOR_MODE_T)dispColorMode;//OSD_CM_ARGB8888_DIRECT32;
      LINT_SUPPRESS_NEXT_STATEMENT(717)
      OSD_GET_PITCH_SIZE(rColorMode, prOsd->u4Width, u4PitchSize);
      TRY2(OSD_RGN_Create(&u4Region, prOsd->u4Width, prOsd->u4Height,
                               (void*)prOsd->pu1FrameBaseAddr,
                               dispColorMode, u4PitchSize,
                               prOsd->u4Left, prOsd->u4Top,
                               prOsd->u4Width, prOsd->u4Height));

      TRY2(OSD_RGN_LIST_Create(&u4List));
      TRY2(OSD_RGN_Insert(u4Region, u4List));
      TRY2(OSD_PLA_FlipTo((UINT32)OSD_PLANE_2, u4List));

      // luis@2007/5/21, unecessary in 5381
      //TRY2(OSD_SC_SetFormat32Bpp(OSD_PLANE_TO_SCALER((INT32)OSD_PLANE_2)));
      //TRY2(OSD_SC_SetSramConfiguration(2));
      TRY2(OSD_SC_Scale((INT32)OSD_SCALER_2, TRUE,
                            prOsd->u4Width, prOsd->u4Height,
                             prOsd->u4ScaleWidth, prOsd->u4ScaleHeight));

      TRY2(OSD_PLA_Enable((UINT32)OSD_PLANE_2, TRUE));
      UNUSED(PMX_SetEnable(0, 1));   
      #endif
      return (void *)prOsd;
      //CATCH_ERROR() 
      //{
        //  return NULL;
      //}      
}
#endif

static VOID _JDEC_SetRzV(RZ_JPG_SCL_PARAM_SET_T *prJpgParam, UINT32 *pu4RowHeight)
{
	    if (pu4RowHeight[0] == 8)
	    {
	        prJpgParam->u4JpgVfacY = 0;
	    }
	    else if (pu4RowHeight[0] == 16)
	    {
	        prJpgParam->u4JpgVfacY = 1;
	    }
	    else if (pu4RowHeight[0] == 24)
	    {
	        prJpgParam->u4JpgVfacY = 2;
	    }
	    else if (pu4RowHeight[0] == 32)
	    {
	        prJpgParam->u4JpgVfacY = 3;
	    }
	    else
	    {
	        LOG(0, "Can NOT support this Y height = %u\n", pu4RowHeight[0]);
	        ASSERT(0);
	    }

	    if (pu4RowHeight[1] == 8)
	    {
	        prJpgParam->u4JpgVfacCb = 0;
	    }
	    else if (pu4RowHeight[1] == 16)
	    {
	        prJpgParam->u4JpgVfacCb = 1;
	    }
	    else if (pu4RowHeight[1] == 24)
	    {
	        prJpgParam->u4JpgVfacCb = 2;
	    }
	    else if (pu4RowHeight[1] == 32)
	    {
	        prJpgParam->u4JpgVfacCb = 3;
	    }
	    else
	    {
	        prJpgParam->u4JpgVfacCb = prJpgParam->u4JpgVfacY;
	        //LOG(0, "Can NOT support this Cb height = %u\n", pu4RowHeight[1]);
	        //ASSERT(0);
	    }
	    
	    if (pu4RowHeight[2] == 8)
	    {
	        prJpgParam->u4JpgVfacCr = 0;
	    }
	    else if (pu4RowHeight[2] == 16)
	    {
	        prJpgParam->u4JpgVfacCr = 1;
	    }
	    else if (pu4RowHeight[2] == 24)
	    {
	        prJpgParam->u4JpgVfacCr = 2;
	    }
	    else if (pu4RowHeight[2] == 32)
	    {
	        prJpgParam->u4JpgVfacCr = 3;
	    }
	    else
	    {
	        prJpgParam->u4JpgVfacCr = prJpgParam->u4JpgVfacY;
	        //LOG(0, "Can NOT support this Cr height = %u\n", pu4RowHeight[2]);
	        //ASSERT(0);
	    }
}

static VOID _JDEC_SetRzV_Progressive(RZ_JPG_SCL_PARAM_SET_T *prJpgParam, UINT32 *pu4RowHeight, UINT32 u4ColorCmp)
{

	if(u4ColorCmp==0x7)		//enhance mode and not last scan, resizer height should be by picture format
	{
	    if (pu4RowHeight[0] == 8)
	    {
	        prJpgParam->u4JpgVfacY = 0;
	    }
	    else if (pu4RowHeight[0] == 16)
	    {
	        prJpgParam->u4JpgVfacY = 1;
	    }
	    else if (pu4RowHeight[0] == 24)
	    {
	        prJpgParam->u4JpgVfacY = 2;
	    }
	    else if (pu4RowHeight[0] == 32)
	    {
	        prJpgParam->u4JpgVfacY = 3;
	    }
	    else
	    {
	        LOG(0, "Can NOT support this Y height = %u\n", pu4RowHeight[0]);
	        ASSERT(0);
	    }

	    if (pu4RowHeight[1] == 8)
	    {
	        prJpgParam->u4JpgVfacCb = 0;
	    }
	    else if (pu4RowHeight[1] == 16)
	    {
	        prJpgParam->u4JpgVfacCb = 1;
	    }
	    else if (pu4RowHeight[1] == 24)
	    {
	        prJpgParam->u4JpgVfacCb = 2;
	    }
	    else if (pu4RowHeight[1] == 32)
	    {
	        prJpgParam->u4JpgVfacCb = 3;
	    }
	    else
	    {
	        prJpgParam->u4JpgVfacCb = prJpgParam->u4JpgVfacY;
	        //LOG(0, "Can NOT support this Cb height = %u\n", pu4RowHeight[1]);
	        //ASSERT(0);
	    }
	    
	    if (pu4RowHeight[2] == 8)
	    {
	        prJpgParam->u4JpgVfacCr = 0;
	    }
	    else if (pu4RowHeight[2] == 16)
	    {
	        prJpgParam->u4JpgVfacCr = 1;
	    }
	    else if (pu4RowHeight[2] == 24)
	    {
	        prJpgParam->u4JpgVfacCr = 2;
	    }
	    else if (pu4RowHeight[2] == 32)
	    {
	        prJpgParam->u4JpgVfacCr = 3;
	    }
	    else
	    {
	        prJpgParam->u4JpgVfacCr = prJpgParam->u4JpgVfacY;
	        //LOG(0, "Can NOT support this Cr height = %u\n", pu4RowHeight[2]);
	        //ASSERT(0);
	    }
	}
	else		//multi-collect mode
	{
		prJpgParam->u4JpgVfacY = 0;
		prJpgParam->u4JpgVfacCb = 0;
		prJpgParam->u4JpgVfacCr = 0;		
	}

}

VOID JDEC_MwSaveRzInfo(IMG_LIB_INFO_T *prImg, ENUM_IMG_ROTATE_PHASE eRotateDegree)
{
    prJpgImg = prImg;
    eJpgRotateDegree = eRotateDegree;
    _u4JpgRzTempBuf = prImg->rMem.u4RzTempBuf;
}
VOID JDEC_SetStop(BOOL fgJdecStop)
{
    _fgJpgFlw2Stop = fgJdecStop;
}

#if 1

INT32 JDEC_RzRowMode_Vfy(UINT32 u4YSrcBufPitch,UINT32 u4CSrcBufPitch, UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
			UINT32 u4OutBank0AddrY, UINT32 u4OutBank0AddrCb, UINT32 u4OutBank0AddrCr, 
			UINT32 u4OutBank1AddrY, UINT32 u4OutBank1AddrCb, UINT32 u4OutBank1AddrCr, 
			UINT32 au4RowWidth[3], UINT32 au4RowHeight[3],
			UINT32 u4DestBufPitch, UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
			UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
			UINT32 u4DestImgXOff,UINT32 u4DestImgYOff, UINT32 u4DisplayClrMode, 			
			UINT32 u4Comp, BOOL fgPreload, BOOL fgLastScan, UINT32 u4RowIdx, UINT32 u4TotalRows)                
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    //JPEGHANDLE hFlow = prJpgImg->hCurDec;
    //IMG_LIB_DISP_INFO_T *prDispInfo = &prJpgImg->rDispInfo;
    //IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prJpgImg->rMem;
    //JMEM_MW_T *prMemMw = NULL;
    //JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    //JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    //static UINT32 su4RzYSrcL;
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;
    

	eRotateDegree = CLOCKWISE_0;
	
    if (!u4RowIdx)
    {
        su4IsRzInited = FALSE;
    }

	x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
    u4DispW = u4DestBufWidth; //prDispInfo->u4ScaleW;
    u4DispH = u4DestBufHeight;//prDispInfo->u4ScaleH;


    su4RzYSrcW  = au4RowWidth[0];
    su4RzCbSrcW = au4RowWidth[1];
    su4RzCrSrcW = au4RowWidth[2];
    su4RzYSrcH  = au4RowHeight[0];
    su4RzCbSrcH = au4RowHeight[1];
    su4RzCrSrcH = au4RowHeight[2];

	//Preload means that decoding mode is enhance mode.
    rJpgParam.fgPreload = fgPreload;
    _JDEC_SetRzV_Progressive(&rJpgParam, au4RowHeight, u4Comp);   


/*
    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
        _JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);   

    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        if (u4Comp != 0x7)
        {
            //su4RzYSrcH = su4RzCbSrcH =su4RzCrSrcH = 8;
            rJpgParam.u4JpgVfacY = rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = 0;
        }
        else
        {
            _JDEC_SetRzV(&rJpgParam, prMemMw->au4RowHeight);
        }
    }

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
*/

    if ((u4RowIdx%2)== 0)
    {
        su4RzCurrYAddr  = u4OutBank0AddrY;
        su4RzCurrCbAddr = u4OutBank0AddrCb;
        su4RzCurrCrAddr = u4OutBank0AddrCr;
        su4RzPrevYAddr  = u4OutBank1AddrY;
        su4RzPrevCbAddr = u4OutBank1AddrCb;
        su4RzPrevCrAddr = u4OutBank1AddrCr;
    }
    else
    {
        su4RzCurrYAddr  = u4OutBank1AddrY;
        su4RzCurrCbAddr = u4OutBank1AddrCb;
        su4RzCurrCrAddr = u4OutBank1AddrCr;
        su4RzPrevYAddr  = u4OutBank0AddrY;
        su4RzPrevCbAddr = u4OutBank0AddrCb;
        su4RzPrevCrAddr = u4OutBank0AddrCr;
    }

    
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = u4YSrcBufPitch;//su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = u4CSrcBufPitch;//su4RzYSrcL;
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = u4SrcImgWidth;//prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = u4SrcImgHeight;//prImg->rHdrInfo.u4Height;
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    rJpgParam.u4CbSrcW = (su4RzYSrcW == su4RzCbSrcW)?u4SrcImgWidth:(u4SrcImgWidth / 2);//(su4RzYSrcW == su4RzCbSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;
    rJpgParam.u4CbSrcH = (su4RzYSrcH == su4RzCbSrcH)?u4SrcImgHeight:(u4SrcImgHeight / 2);//(su4RzYSrcH == su4RzCbSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)2 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = (su4RzYSrcW == su4RzCrSrcW)?u4SrcImgWidth:(u4SrcImgWidth / 2);//(su4RzYSrcW == su4RzCrSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    rJpgParam.u4CrSrcH = (su4RzYSrcH == su4RzCrSrcH)?u4SrcImgHeight:(u4SrcImgHeight / 2);//(su4RzYSrcH == su4RzCrSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)2: rJpgParam.u4CrSrcH;
    rJpgParam.u4ColorComp = u4Comp;
    rJpgParam.u4IsRsOut = 1;       // block|raster based output
    rJpgParam.fgGray = (u4Comp == (UINT32)0xF) ? 1 : 0;
    
    rJpgParam.u4OutSwap = 0;

    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer
    {
		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgBase = u4DestBufSA1;//(UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4CTgBase = u4DestBufSA2;//(UINT32)prDispInfo->pu1DisplayStart_temp[1];
		}
		else
		{
			rJpgParam.u4YTgBase = u4DestBufSA1;//(UINT32)prDispInfo->pu1DisplayStart[0];
			rJpgParam.u4CTgBase = u4DestBufSA2;//(UINT32)prDispInfo->pu1DisplayStart[1];
		}
        rJpgParam.u4IsJpgSameaddr = 0;
    }
    else	//output to scale working buf
    {
        rJpgParam.u4YTgBase = (UINT32)u4DestBufSA1;
        if ((u4DisplayClrMode == AYCBCR8888_DIRECT32) || (u4DisplayClrMode == ARGB8888_DIRECT32))
        {
            rJpgParam.u4CTgBase = (UINT32)u4DestBufSA1;
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)u4DestBufSA2;
        }
        rJpgParam.u4IsJpgSameaddr = 1;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;

    u4DispW = u4DestBufWidth; //prDispInfo->u4ScaleW;
    u4DispH = u4DestBufHeight;//prDispInfo->u4ScaleH;
    
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0 || eRotateDegree == CLOCKWISE_180 ||
        eRotateDegree == CLOCKWISE_0_WITH_FLIP || eRotateDegree == CLOCKWISE_180_WITH_FLIP) //center
    {

		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgHOffset = 0;
			rJpgParam.u4YTgVOffset = 0;		
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
		}
		else
		{
			rJpgParam.u4YTgHOffset = u4DestImgXOff;//prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = u4DestImgYOff;//prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = u4DestBufPitch;//prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = u4DestBufPitch;//prDispInfo->u4DispPitch;
		}
    }
    else    //center op by gfx
    {
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;    
        rJpgParam.u4YTgBufLen = u4DispW << 2;
        rJpgParam.u4CTgBufLen = u4DispW << 2;
    }    

    if(0)// (prJpgImg->u1MemClass <= RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
        rJpgParam.u4CTgH = u4DispH;

        switch (u4DisplayClrMode)
        {
        case AYCBCR8888_DIRECT32:            
        case ARGB8888_DIRECT32:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
            rJpgParam.u4CTgW = u4DispW;
            break;
		case ARGB4444_DIRECT16:		
			rJpgParam.u4YTgCM = 0;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
			rJpgParam.u4IsVdo2Osd = 0;	   // output in osd format ?		
			break;            
	 default:
 	     rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
            rJpgParam.u4CTgW = u4DispW >> 1;
	     break;
        }
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rJpgParam.u4YTgCM = 0;
        switch (u4DisplayClrMode)
        {
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
          LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", u4DisplayClrMode);
          ASSERT(0);
          break;
        }

        //check height must be even of vdo mode.
        if((u4DisplayClrMode==Y_CBCR420_SEP16)||(u4DisplayClrMode==Y_CBCR422_SEP16))
		{
	        if((rJpgParam.u4YSrcH%2)==1)
	        	rJpgParam.u4YSrcH +=1;

	        if((rJpgParam.u4CbSrcH%2)==1)
	        	rJpgParam.u4CbSrcH +=1;        

	        if((rJpgParam.u4CrSrcH%2)==1)
	        	rJpgParam.u4CrSrcH +=1; 

	        if((rJpgParam.u4YTgH%2)==1)
	        	rJpgParam.u4YTgH +=1;

	        if((rJpgParam.u4CTgH%2)==1)
	        	rJpgParam.u4CTgH +=1;        
		}        
    }
    
    rJpgParam.u4IsFstBl = (u4RowIdx == (UINT32)0)? (UINT32)1 : (UINT32)0;
    rJpgParam.u4IsLstBl = (u4RowIdx == (UINT32)(u4TotalRows - 1))? (UINT32)1 : (UINT32)0;
    
    rJpgParam.u4IsJpgRzOn = 0;    // enable jpg-rz hand-off for scan mode
    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }   
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));

    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

	CATCH_ERROR()
	{
		return i4Ret;
	}    
}

INT32 JDEC_RzRowMode(
			UINT32 u4OutBank0AddrY, UINT32 u4OutBank0AddrCb, UINT32 u4OutBank0AddrCr, 
			UINT32 u4OutBank1AddrY, UINT32 u4OutBank1AddrCb, UINT32 u4OutBank1AddrCr, 
			UINT32 u4Comp, BOOL fgPreload, BOOL fgLastScan, UINT32 u4RowIdx, UINT32 u4TotalRows)
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    JPEGHANDLE hFlow = prJpgImg->hCurDec;
    IMG_LIB_DISP_INFO_T *prDispInfo = &prJpgImg->rDispInfo;
    IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prJpgImg->rMem;
    JMEM_MW_T *prMemMw = NULL;
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    static UINT32 su4RzYSrcL;
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;
    
    if (!u4RowIdx)
    {
        su4IsRzInited = FALSE;
    }
    x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
    u4DispW =prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;

    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
    _JDEC_SetRzV_Progressive(&rJpgParam, prMemYuv->au4RowHeight, u4Comp);   

    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        if (u4Comp != 0x7)
        {
            //su4RzYSrcH = su4RzCbSrcH =su4RzCrSrcH = 8;
            rJpgParam.u4JpgVfacY = rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = 0;
        }
        else
        {
            _JDEC_SetRzV_Progressive(&rJpgParam, prMemMw->au4RowHeight, u4Comp);
        }
    }

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
    
    if ((u4RowIdx%2)== 0)
    {
        su4RzCurrYAddr  = u4OutBank0AddrY;
        su4RzCurrCbAddr = u4OutBank0AddrCb;
        su4RzCurrCrAddr = u4OutBank0AddrCr;
        su4RzPrevYAddr  = u4OutBank1AddrY;
        su4RzPrevCbAddr = u4OutBank1AddrCb;
        su4RzPrevCrAddr = u4OutBank1AddrCr;
    }
    else
    {
        su4RzCurrYAddr  = u4OutBank1AddrY;
        su4RzCurrCbAddr = u4OutBank1AddrCb;
        su4RzCurrCrAddr = u4OutBank1AddrCr;
        su4RzPrevYAddr  = u4OutBank0AddrY;
        su4RzPrevCbAddr = u4OutBank0AddrCb;
        su4RzPrevCrAddr = u4OutBank0AddrCr;
    }

    
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzYSrcL;
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = prImg->rHdrInfo.u4Height;
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    rJpgParam.u4CbSrcW = (su4RzYSrcW == su4RzCbSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;
    rJpgParam.u4CbSrcH = (su4RzYSrcH == su4RzCbSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)2 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW =(su4RzYSrcW == su4RzCrSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    rJpgParam.u4CrSrcH =(su4RzYSrcH == su4RzCrSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)2: rJpgParam.u4CrSrcH;
    rJpgParam.u4ColorComp = u4Comp;
    rJpgParam.u4IsRsOut = 1;       // block|raster based output
    rJpgParam.fgGray = (u4Comp == (UINT32)0xF) ? 1 : 0;    
    rJpgParam.fgPreload = fgPreload;//Preload means that decoding mode is enhance mode.
    rJpgParam.u4OutSwap = 0;

    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer
    {
		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[1];
		}
		else
		{
			rJpgParam.u4YTgBase =(UINT32)prDispInfo->pu1DisplayStart[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart[1];
		}
        rJpgParam.u4IsJpgSameaddr = 0;
    }
    else	//output to scale working buf
    {
        rJpgParam.u4YTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        if((prDispInfo->u4DisplayClrMode == AYCBCR8888_DIRECT32) || (prDispInfo->u4DisplayClrMode == ARGB8888_DIRECT32))
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[1];
        }
        rJpgParam.u4IsJpgSameaddr = 1;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;

    u4DispW = prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;
    
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0 || eRotateDegree == CLOCKWISE_180 ||
        eRotateDegree == CLOCKWISE_0_WITH_FLIP || eRotateDegree == CLOCKWISE_180_WITH_FLIP) //center
    {

		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgHOffset = 0;
			rJpgParam.u4YTgVOffset = 0;		
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
		}
		else
		{
			rJpgParam.u4YTgHOffset = prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = prDispInfo->u4DispPitch;
		}
    }
    else    //center op by gfx
    {
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;    
        rJpgParam.u4YTgBufLen = u4DispW << 2;
        rJpgParam.u4CTgBufLen = u4DispW << 2;
    }    

    if (prJpgImg->u1MemClass <= RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
        rJpgParam.u4CTgH = u4DispH;

        switch (prDispInfo->u4DisplayClrMode)
        {
        case AYCBCR8888_DIRECT32:            
        case ARGB8888_DIRECT32:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
            rJpgParam.u4CTgW = u4DispW;
            break;
        case ARGB4444_DIRECT16:		
            rJpgParam.u4YTgCM = 0;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            rJpgParam.u4IsVdo2Osd = 0;	   // output in osd format ?		
            break;            
	 default:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
            rJpgParam.u4CTgW = u4DispW >> 1;
            break;
        }
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rJpgParam.u4YTgCM = 0;
        switch (prDispInfo->u4DisplayClrMode)
        {
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
            LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", prDispInfo->u4DisplayClrMode);
            ASSERT(0);
            break;
        }

        //check height must be even of vdo mode.
        if((prDispInfo->u4DisplayClrMode==Y_CBCR420_SEP16)||(prDispInfo->u4DisplayClrMode==Y_CBCR422_SEP16))
		{
	        if((rJpgParam.u4YSrcH%2)==1)
	        	rJpgParam.u4YSrcH +=1;

	        if((rJpgParam.u4CbSrcH%2)==1)
	        	rJpgParam.u4CbSrcH +=1;        

	        if((rJpgParam.u4CrSrcH%2)==1)
	        	rJpgParam.u4CrSrcH +=1; 

	        if((rJpgParam.u4YTgH%2)==1)
	        	rJpgParam.u4YTgH +=1;

	        if((rJpgParam.u4CTgH%2)==1)
	        	rJpgParam.u4CTgH +=1;        
		}        
    }
    
    rJpgParam.u4IsFstBl = (u4RowIdx == (UINT32)0)? (UINT32)1 : (UINT32)0;
    rJpgParam.u4IsLstBl = (u4RowIdx == (UINT32)(u4TotalRows - 1))? (UINT32)1 : (UINT32)0;
    
    rJpgParam.u4IsJpgRzOn = 0;    // enable jpg-rz hand-off for scan mode
    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }   
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));

    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

	CATCH_ERROR()
	{
		return i4Ret;
	}    
}

INT32 JDEC_RzScanMode_Vfy(UINT32 u4YSrcBufPitch,UINT32 u4CSrcBufPitch, UINT32 u4SrcImgWidth,UINT32 u4SrcImgHeight,
			UINT32 u4OutBank0AddrY, UINT32 u4OutBank0AddrCb, UINT32 u4OutBank0AddrCr, 
			UINT32 u4OutBank1AddrY, UINT32 u4OutBank1AddrCb, UINT32 u4OutBank1AddrCr, 
			UINT32 au4RowWidth[3], UINT32 au4RowHeight[3],
			UINT32 u4DestBufPitch, UINT32 u4DestBufWidth,UINT32 u4DestBufHeight,
			UINT32 u4DestBufSA1,UINT32 u4DestBufSA2,
			UINT32 u4DestImgXOff,UINT32 u4DestImgYOff, UINT32 u4DisplayClrMode, 			
			UINT32 u4Comp, BOOL fgPreload, BOOL fgLastScan)                
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    //JPEGHANDLE hFlow = prJpgImg->hCurDec;
    //IMG_LIB_DISP_INFO_T *prDispInfo = &prJpgImg->rDispInfo;
    //IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prJpgImg->rMem;
    //JMEM_MW_T *prMemMw = NULL;
    //JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    //JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    //static UINT32 su4RzYSrcL;
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;
    

	eRotateDegree = CLOCKWISE_0;
	su4IsRzInited = FALSE;

	x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
    u4DispW = u4DestBufWidth; //prDispInfo->u4ScaleW;
    u4DispH = u4DestBufHeight;//prDispInfo->u4ScaleH;


    su4RzYSrcW  = au4RowWidth[0];
    su4RzCbSrcW = au4RowWidth[1];
    su4RzCrSrcW = au4RowWidth[2];
    su4RzYSrcH  = au4RowHeight[0];
    su4RzCbSrcH = au4RowHeight[1];
    su4RzCrSrcH = au4RowHeight[2];

	//Preload means that decoding mode is enhance mode.
    rJpgParam.fgPreload = fgPreload;
    _JDEC_SetRzV_Progressive(&rJpgParam, au4RowHeight, u4Comp);   


/*
    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
        _JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);   

    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        if (u4Comp != 0x7)
        {
            //su4RzYSrcH = su4RzCbSrcH =su4RzCrSrcH = 8;
            rJpgParam.u4JpgVfacY = rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = 0;
        }
        else
        {
            _JDEC_SetRzV(&rJpgParam, prMemMw->au4RowHeight);
        }
    }

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
*/

    su4RzCurrYAddr  = u4OutBank0AddrY;
    su4RzCurrCbAddr = u4OutBank0AddrCb;
    su4RzCurrCrAddr = u4OutBank0AddrCr;
    su4RzPrevYAddr  = u4OutBank1AddrY;
    su4RzPrevCbAddr = u4OutBank1AddrCb;
    su4RzPrevCrAddr = u4OutBank1AddrCr;
        
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = u4YSrcBufPitch;//su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = u4CSrcBufPitch;//su4RzYSrcL;
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = u4SrcImgWidth;//prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = u4SrcImgHeight;//prImg->rHdrInfo.u4Height;
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    rJpgParam.u4CbSrcW = (su4RzYSrcW == su4RzCbSrcW)?u4SrcImgWidth:(u4SrcImgWidth / 2);//(su4RzYSrcW == su4RzCbSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;
    rJpgParam.u4CbSrcH = (su4RzYSrcH == su4RzCbSrcH)?u4SrcImgHeight:(u4SrcImgHeight / 2);//(su4RzYSrcH == su4RzCbSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)2 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = (su4RzYSrcW == su4RzCrSrcW)?u4SrcImgWidth:(u4SrcImgWidth / 2);//(su4RzYSrcW == su4RzCrSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    rJpgParam.u4CrSrcH = (su4RzYSrcH == su4RzCrSrcH)?u4SrcImgHeight:(u4SrcImgHeight / 2);//(su4RzYSrcH == su4RzCrSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)2: rJpgParam.u4CrSrcH;
    rJpgParam.u4ColorComp = u4Comp;
    rJpgParam.u4IsRsOut = 1;       // block|raster based output
    rJpgParam.fgGray = (u4Comp == (UINT32)0xF) ? 1 : 0;
    
    rJpgParam.u4OutSwap = 0;

    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer
    {
		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&& (prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgBase = u4DestBufSA1;//(UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4CTgBase = u4DestBufSA2;//(UINT32)prDispInfo->pu1DisplayStart_temp[1];
		}
		else
		{
			rJpgParam.u4YTgBase = u4DestBufSA1;//(UINT32)prDispInfo->pu1DisplayStart[0];
			rJpgParam.u4CTgBase = u4DestBufSA2;//(UINT32)prDispInfo->pu1DisplayStart[1];
		}
        rJpgParam.u4IsJpgSameaddr = 0;
    }
    else	//output to scale working buf
    {
        rJpgParam.u4YTgBase = (UINT32)u4DestBufSA1;
        if ((u4DisplayClrMode == AYCBCR8888_DIRECT32) || (u4DisplayClrMode == ARGB8888_DIRECT32))
        {
            rJpgParam.u4CTgBase = (UINT32)u4DestBufSA1;
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)u4DestBufSA2;
        }
        rJpgParam.u4IsJpgSameaddr = 1;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;

    u4DispW = u4DestBufWidth; //prDispInfo->u4ScaleW;
    u4DispH = u4DestBufHeight; //prDispInfo->u4ScaleH;
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0 || eRotateDegree == CLOCKWISE_180 ||
        eRotateDegree == CLOCKWISE_0_WITH_FLIP || eRotateDegree == CLOCKWISE_180_WITH_FLIP) //center
    {

		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgHOffset = 0;
			rJpgParam.u4YTgVOffset = 0;		
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
		}
		else
		{
			rJpgParam.u4YTgHOffset = u4DestImgXOff;//prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = u4DestImgYOff;//prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = u4DestBufPitch;//prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = u4DestBufPitch;//prDispInfo->u4DispPitch;
		}
    }
    else    //center op by gfx
    {
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;    
        rJpgParam.u4YTgBufLen = u4DispW << 2;
        rJpgParam.u4CTgBufLen = u4DispW << 2;
    }    

    if(0)// (prJpgImg->u1MemClass <= RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
        rJpgParam.u4CTgH = u4DispH;

        switch (u4DisplayClrMode)
        {
        case AYCBCR8888_DIRECT32:            
        case ARGB8888_DIRECT32:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
            rJpgParam.u4CTgW = u4DispW;
            break;
		case ARGB4444_DIRECT16:		
			rJpgParam.u4YTgCM = 0;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
			rJpgParam.u4IsVdo2Osd = 0;	   // output in osd format ?		
			break;            
	 default:
 	     rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
            rJpgParam.u4CTgW = u4DispW >> 1;
	     break;
        }
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rJpgParam.u4YTgCM = 0;
        switch (u4DisplayClrMode)
        {
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
          LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", u4DisplayClrMode);
          ASSERT(0);
          break;
        }

        //check height must be even of vdo mode.
        if((u4DisplayClrMode==Y_CBCR420_SEP16)||(u4DisplayClrMode==Y_CBCR422_SEP16))
		{
	        if((rJpgParam.u4YSrcH%2)==1)
	        	rJpgParam.u4YSrcH +=1;

	        if((rJpgParam.u4CbSrcH%2)==1)
	        	rJpgParam.u4CbSrcH +=1;        

	        if((rJpgParam.u4CrSrcH%2)==1)
	        	rJpgParam.u4CrSrcH +=1; 

	        if((rJpgParam.u4YTgH%2)==1)
	        	rJpgParam.u4YTgH +=1;

	        if((rJpgParam.u4CTgH%2)==1)
	        	rJpgParam.u4CTgH +=1;        
		}        
    }
    
    rJpgParam.u4IsFstBl = 1;
    rJpgParam.u4IsLstBl = 0;
    
    rJpgParam.u4IsJpgRzOn = 1;    // enable jpg-rz hand-off for scan mode
    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    //IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }   
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));
    /*
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();
*/
	CATCH_ERROR()
	{
		return i4Ret;
	}    
}

INT32 JDEC_RzScanMode(
			UINT32 u4OutBank0AddrY, UINT32 u4OutBank0AddrCb, UINT32 u4OutBank0AddrCr, 
			UINT32 u4OutBank1AddrY, UINT32 u4OutBank1AddrCb, UINT32 u4OutBank1AddrCr, 	
			UINT32 u4Comp, BOOL fgPreload, BOOL fgLastScan)                
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    JPEGHANDLE hFlow = prJpgImg->hCurDec;
    IMG_LIB_DISP_INFO_T *prDispInfo = &prJpgImg->rDispInfo;
    IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prJpgImg->rMem;
    JMEM_MW_T *prMemMw = NULL;
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    static UINT32 su4RzYSrcL, su4RzCSrcL;
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;    
    
    x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));

    su4IsRzInited = FALSE;
    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
        _JDEC_SetRzV_Progressive(&rJpgParam, prMemYuv->au4RowHeight, u4Comp);   

    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        if (u4Comp != 0x7)
        {
            //su4RzYSrcH = su4RzCbSrcH =su4RzCrSrcH = 8;
            rJpgParam.u4JpgVfacY = rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = 0;
        }
        else
        {
            _JDEC_SetRzV_Progressive(&rJpgParam, prMemMw->au4RowHeight, u4Comp);
        }
    }

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
    su4RzCSrcL  = prJpgHw->u4CompPitchRow[1];    


    su4RzCurrYAddr  = u4OutBank0AddrY;
    su4RzCurrCbAddr = u4OutBank0AddrCb;
    su4RzCurrCrAddr = u4OutBank0AddrCr;
    su4RzPrevYAddr  = u4OutBank1AddrY;
    su4RzPrevCbAddr = u4OutBank1AddrCb;
    su4RzPrevCrAddr = u4OutBank1AddrCr;
/*
    su4RzPrevYAddr  = u4OutBank0AddrY;
    su4RzPrevCbAddr = u4OutBank0AddrCb;
    su4RzPrevCrAddr = u4OutBank0AddrCr;
    su4RzCurrYAddr  = u4OutBank1AddrY;
    su4RzCurrCbAddr = u4OutBank1AddrCb;
    su4RzCurrCrAddr = u4OutBank1AddrCr;
*/    
    
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzCSrcL;    
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = prImg->rHdrInfo.u4Height;
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    rJpgParam.u4CbSrcW = (su4RzYSrcW == su4RzCbSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;
    rJpgParam.u4CbSrcH = (su4RzYSrcH == su4RzCbSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = (su4RzYSrcW == su4RzCrSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    rJpgParam.u4CrSrcH = (su4RzYSrcH == su4RzCrSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcH;
    rJpgParam.u4ColorComp = u4Comp;
    rJpgParam.u4IsRsOut = 1;       // block|raster based output
    if(su4RzCSrcL == 0)
    {
        rJpgParam.u4CbSrcW = rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW;
        rJpgParam.u4CrSrcH = rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH;
        rJpgParam.u4ColorComp = 0x7;
        rJpgParam.fgGray = 1;
    }
    else
    {
        rJpgParam.fgGray = 0;
    }
    rJpgParam.fgPreload = fgPreload;
    rJpgParam.u4OutSwap = 0;

    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer
    {
		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[1];
		}
		else
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart[1];
		}
        rJpgParam.u4IsJpgSameaddr = 0;
    }
    else	//output to scale working buf
    {
        rJpgParam.u4YTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        if( (prDispInfo->u4DisplayClrMode == AYCBCR8888_DIRECT32) || (prDispInfo->u4DisplayClrMode == ARGB8888_DIRECT32))
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[1];
        }
        rJpgParam.u4IsJpgSameaddr = 1;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;

    u4DispW = prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0 || eRotateDegree == CLOCKWISE_180 ||
        eRotateDegree == CLOCKWISE_0_WITH_FLIP || eRotateDegree == CLOCKWISE_180_WITH_FLIP) //center
    {

		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&& (prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgHOffset = 0;
			rJpgParam.u4YTgVOffset = 0;		
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
		}
		else
		{
			rJpgParam.u4YTgHOffset = prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = prDispInfo->u4DispPitch;
		}
    }
    else    //center op by gfx
    {
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;    
        rJpgParam.u4YTgBufLen = u4DispW << 2;
        rJpgParam.u4CTgBufLen = u4DispW << 2;
    }    

    if (prJpgImg->u1MemClass <= RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
        rJpgParam.u4CTgH = u4DispH;

        switch (prDispInfo->u4DisplayClrMode)
        {
		case AYCBCR8888_DIRECT32:            
                case ARGB8888_DIRECT32:
			rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
			rJpgParam.u4CTgW = u4DispW;
			break;
			
		case ARGB4444_DIRECT16:		
			rJpgParam.u4YTgCM = 0;
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
			rJpgParam.u4CTgW = u4DispW >> 1;
			rJpgParam.u4CTgH = u4DispH;
			rJpgParam.u4IsVdo2Osd = 0;	   // output in osd format ?		
			break;         

		case Y_CBCR420_SEP16:
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
			rJpgParam.u4CTgW = u4DispW >> 1;
			rJpgParam.u4CTgH = u4DispH >> 1;
			rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
			rJpgParam.u4YTgCM = 0;            
			break;
			
		case Y_CBCR422_SEP16:
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
			rJpgParam.u4CTgW = u4DispW >> 1;
			rJpgParam.u4CTgH = u4DispH;
			rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
			rJpgParam.u4YTgCM = 0;            
			break;

		default:
			rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
			rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
			rJpgParam.u4CTgW = u4DispW >> 1;
			break;
		}
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rJpgParam.u4YTgCM = 0;
        switch (prDispInfo->u4DisplayClrMode)
        {
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
          LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", prDispInfo->u4DisplayClrMode);
          ASSERT(0);
          break;
        }
    }
    
    rJpgParam.u4IsFstBl = 1;
    rJpgParam.u4IsLstBl = 0;
    
    rJpgParam.u4IsJpgRzOn = 1;    // enable jpg-rz hand-off for scan mode
    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    #ifdef JPG_DECODING_USING_RZ_IOMMU
	rJpgParam.u4RzIOMMU_MMUTableAddr = prDispInfo->u4JpgUsingRzIOMMU_DispMMUTableAddr;
    #else
    rJpgParam.u4RzIOMMU_MMUTableAddr = 0;
    #endif
    
    IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }   
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }

    #ifdef JPG_DECODING_USING_RZ_IOMMU
    if((rJpgParam.u4RzIOMMU_MMUTableAddr != 0) && (prImg->rDispInfo.u4DisplayClrMode != ARGB4444_DIRECT16))
    {
		IMGRZ_Cfg_MMU_IO((BOOL)0,(BOOL)1);
    }		
    #endif
    
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));
   
    IMGRZ_Flush();
   /*
    IMGRZ_Wait();
    IMGRZ_Unlock();
*/
	CATCH_ERROR()
	{
		return i4Ret;
	}    
}
#endif

#if 1
INT32 JDEC_RzRow(UINT32 u4OutAddrY, UINT32 u4OutAddrCb, UINT32 u4OutAddrCr, UINT32 u4RowIdx,
                UINT32 u4TotalRows, UINT32 u4Comp, BOOL fgPreload)
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    JPEGHANDLE hFlow = prJpgImg->hCurDec;
    IMG_LIB_DISP_INFO_T *prDispInfo = &prJpgImg->rDispInfo;
    IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prJpgImg->rMem;
    JMEM_MW_T *prMemMw = NULL;
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    static UINT32 su4RzYSrcL, su4RzCSrcL;
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;

	x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));

    if (!u4RowIdx)
    {
        su4IsRzInited = FALSE;
    }
    
    u4DispW = prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;

    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
        _JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);   

    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        if (u4Comp != 0x7)
        {
            //su4RzYSrcH = su4RzCbSrcH =su4RzCrSrcH = 8;
            rJpgParam.u4JpgVfacY = rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = 0;
        }
        else
        {
            _JDEC_SetRzV(&rJpgParam, prMemMw->au4RowHeight);
        }
    }

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);

    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];
    su4RzCSrcL  = prJpgHw->u4CompPitchRow[1];

    if (u4RowIdx == 0)
    {
        su4RzPrevYAddr  = u4OutAddrY;
        su4RzPrevCbAddr = u4OutAddrCb;
        su4RzPrevCrAddr = u4OutAddrCr;
    }
    else
    {
        su4RzPrevYAddr  = su4RzCurrYAddr;
        su4RzPrevCbAddr = su4RzCurrCbAddr;
        su4RzPrevCrAddr = su4RzCurrCrAddr;
    }
    su4RzCurrYAddr  = u4OutAddrY;
    su4RzCurrCbAddr = u4OutAddrCb;
    su4RzCurrCrAddr = u4OutAddrCr;
    
    
    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzCSrcL;    
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = prImg->rHdrInfo.u4Height;
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    rJpgParam.u4CbSrcW = (su4RzYSrcW == su4RzCbSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;
    rJpgParam.u4CbSrcH = (su4RzYSrcH == su4RzCbSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = (su4RzYSrcW == su4RzCrSrcW)?prImg->rHdrInfo.u4Width:(prImg->rHdrInfo.u4Width / 2);
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    rJpgParam.u4CrSrcH = (su4RzYSrcH == su4RzCrSrcH)?prImg->rHdrInfo.u4Height:(prImg->rHdrInfo.u4Height / 2);
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcH;
    rJpgParam.u4ColorComp = u4Comp;
    rJpgParam.u4IsRsOut = 1;       // block|raster based output
    rJpgParam.fgGray = (u4Comp == (UINT32)0xF) ? 1 : 0;
    rJpgParam.fgPreload = fgPreload;
    rJpgParam.u4OutSwap = 0;

    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer
    {
		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[1];
		}
		else
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart[0];
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart[1];
		}
        rJpgParam.u4IsJpgSameaddr = 0;
    }
    else	//output to scale working buf
    {
        rJpgParam.u4YTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        if ((prDispInfo->u4DisplayClrMode == AYCBCR8888_DIRECT32) || (prDispInfo->u4DisplayClrMode == ARGB8888_DIRECT32))
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[1];
        }
        rJpgParam.u4IsJpgSameaddr = 1;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;

    u4DispW = prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0 || eRotateDegree == CLOCKWISE_180 ||
        eRotateDegree == CLOCKWISE_0_WITH_FLIP || eRotateDegree == CLOCKWISE_180_WITH_FLIP) //center
    {

		if ((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
		{
			rJpgParam.u4YTgHOffset = 0;
			rJpgParam.u4YTgVOffset = 0;		
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
		}
		else
		{
			rJpgParam.u4YTgHOffset = prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = prDispInfo->u4DispPitch;
		}
    }
    else    //center op by gfx
    {
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;    
        rJpgParam.u4YTgBufLen = u4DispW << 2;
        rJpgParam.u4CTgBufLen = u4DispW << 2;
    }    

    if (prJpgImg->u1MemClass <= RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?
        rJpgParam.u4CTgH = u4DispH;

        switch (prDispInfo->u4DisplayClrMode)
        {
        case AYCBCR8888_DIRECT32:            
        case ARGB8888_DIRECT32:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
            rJpgParam.u4CTgW = u4DispW;
            break;
		case ARGB4444_DIRECT16:		
			rJpgParam.u4YTgCM = 0;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
			rJpgParam.u4IsVdo2Osd = 0;	   // output in osd format ?		
			break;            
	 default:
 	     rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
            rJpgParam.u4CTgW = u4DispW >> 1;
	     break;
        }
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in osd format ?
        rJpgParam.u4YTgCM = 0;
        switch (prDispInfo->u4DisplayClrMode)
        {
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
          LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", prDispInfo->u4DisplayClrMode);
          ASSERT(0);
          break;
        }
    }
    
    rJpgParam.u4IsFstBl = (u4RowIdx == (UINT32)0)? (UINT32)1 : (UINT32)0;
    rJpgParam.u4IsLstBl = (u4RowIdx == (UINT32)(u4TotalRows - 1))? (UINT32)1 : (UINT32)0;
    
    rJpgParam.u4IsJpgRzOn = 0;    // diable jpg-rz hand-off
    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    {
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    }   
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();
    if (u4RowIdx == (u4TotalRows - 1))
    {
        su4IsRzInited = FALSE;
    }
	CATCH_ERROR()
	{
		return i4Ret;
	}    
}
#endif



//#define JPGHW_TIME_PROFILE
#ifdef JPGHW_TIME_PROFILE
static HAL_TIME_T _rDecStart, _rRZStart;
static HAL_TIME_T _rDecEnd, _rRZEnd;
static HAL_TIME_T _rDecAcc, _rRZAcc;
static HAL_TIME_T _rDecdif, _rRZdif;
#endif
static INT32 _jpg_VideoOut5391Drv(JPEGHANDLE hFlow, UINT32 u4RowIdx, 
    UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr)
{
    IMG_LIB_INFO_T *prImg = prJpgImg;
    IMG_LIB_DISP_INFO_T *prDispInfo = &prImg->rDispInfo;
    JMEM_MW_T *prMemMw = NULL;
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    
    UINT32 au4RowSize[3];
    INT32 i4Ret = (INT32)E_JPG_OK;
    UINT32 u4SleepCnt = 0, u4NonSleepCnt = 0;
    
    ASSERT(hFlow);
    ASSERT(apu1OutputAddr != NULL);

    VERIFY(VLDJPG_SetCurrMcuRowNum5391(u4RowIdx) == (INT32)E_HWJPG_OK);


    if (prImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        au4RowSize[0] = prMemYuv->au4RowSize[0];
        au4RowSize[1] = prMemYuv->au4RowSize[1];
        au4RowSize[2] = prMemYuv->au4RowSize[2];
    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        au4RowSize[0] = prMemMw->au4RowSize[0];
        au4RowSize[1] = prMemMw->au4RowSize[1];
        au4RowSize[2] = prMemMw->au4RowSize[2];
    }    

    LOG(3,"Video Out Y address = 0x%08x\n",prDispInfo->pu1DisplayStart[0]);
    LOG(3,"Video Out C address = 0x%08x\n",prDispInfo->pu1DisplayStart[1]);
    
    HalFlushInvalidateDCache();
    if ((u4RowIdx % 2) == 0)
    {
        JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);

        TRY2(VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx));

        TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, (UINT8 *)prDispInfo->pu1DisplayStart[0], au4RowSize[0]) );
        TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, (UINT8 *)prDispInfo->pu1DisplayStart[1], au4RowSize[1]) );
        TRY2( VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, (UINT8 *)prDispInfo->pu1DisplayStart[1], au4RowSize[2]) );

        TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows) );
    }
        // decode
#ifdef JPGHW_TIME_PROFILE	
    HAL_GetTime(&_rDecStart);
#endif

    JPGFLW->u4DecodedSize=0;
    VLDJPG_DecodeOnePic(H_JPGHW);
    while (VLDJPG_CheckDecodePicFinish(H_JPGHW) != (INT32)E_HWJPG_OK)
    {            
        u4NonSleepCnt++;
        if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
        {
            _jpg_UpdateReadPtr(hFlow);
            LOG(7,"%s Decoding Progress=%d%\n",__FUNCTION__,JDEC_GetProgress(hFlow));
            if (JPGHDR->u4RingData < (UINT32)(200 * 1024))
            {
                LOG(3, "JPGHDR->u4RingData:%d\n", JPGHDR->u4RingData);
                IMG_BsBufManager(prImg->ucImgId);  
            }
            u4SleepCnt++;  
            u4NonSleepCnt = 0;
        }     
        if (VLDJPG_CheckDecodingError(H_JPGHW) != (INT32)E_HWJPG_OK)
        {
            i4Ret = -(INT32)E_JPG_ERR_FATAL;
			LOG(3, "%s, line %d decoding error\n", __FUNCTION__, __LINE__);
            break;
        }
        if (u4NonSleepCnt >= (UINT32)0x80000)
        {
            x_thread_delay(1);
            u4SleepCnt++;  
            u4NonSleepCnt = 0;
            if (VLDJPG_CheckStop(H_JPGHW) != (INT32)E_HWJPG_OK)
            {
                i4Ret = -(INT32)E_JPG_ERR_FATAL;
                LOG(3, "already stop, timeout 1\n");
                break;
            }
        }
        if (u4SleepCnt >= (UINT32)30000)
        {
            i4Ret = -(INT32)E_JPG_ERR_FATAL;
            LOG(3, "JPGHDR->u4RingData:%d, time out 2\n", JPGHDR->u4RingData);
            break;
        }
        if (_fgJpgFlw2Stop)
        {
            i4Ret = -(INT32)E_JPG_ERR_FATAL;
			 LOG(3, "stop for _fgJpgFlw2Stop 1\n");
            break;
        }
    }
    
#ifdef JPGHW_TIME_PROFILE	
        HAL_GetTime(&_rDecEnd);
        HAL_GetDeltaTime(&_rDecdif, &_rDecStart, &_rDecEnd);
        _rDecAcc.u4Seconds += _rDecdif.u4Seconds;
        _rDecAcc.u4Micros  += _rDecdif.u4Micros;
        if (_rDecAcc.u4Micros >= 1000000)
        {
            _rDecAcc.u4Seconds += _rDecAcc.u4Micros / 1000000;
            _rDecAcc.u4Micros  %= 1000000;
        }
#endif
    prJpgImg->u4RowIdx = (UINT32)(u4TotalRows - 1);
    u4RowIdx = prJpgImg->u4RowIdx;

    if (i4Ret)
    {
        prJpgImg->u4RowIdx = (UINT32)(u4TotalRows - 1);
        VLDJPG_SetStop(H_JPGHW);
        VLDJPG_SoftwareReset();        
        TRY2(i4Ret);
    }
#ifdef JPEG_VLD_V4
    LOG(5, "Jpgdec CKsum = [%x]\n", VLDJPG_ReadCKsum());
    LOG(5, "Jpgdec CRC = [%x]\n", VLDJPG_ReadCRC());
    LOG(5, "Error detection type = [%x]\n", VLDJPG_ReadErrorType());
#endif
    if (u4RowIdx == (u4TotalRows - 1))
    {
#ifdef JPGHW_TIME_PROFILE	        
        LOG(0, "decoding %u.%06u/RZ %u.%06u\n",
               _rDecAcc.u4Seconds, _rDecAcc.u4Micros, 
               _rRZAcc.u4Seconds, _rRZAcc.u4Micros);        
#endif
        VLDJPG_SetPowerOff();
    }

    // update read pointer and data size of ring buffer
    if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
    {
        _jpg_UpdateReadPtr(hFlow);  
    }

    JPGFLW->u4DecodedSize=JPGFLW->u4FileSize;
    LOG(7,"Decoding Progress=%d%\n",JDEC_GetProgress(hFlow));
    CATCH_ERROR()
    {   
        return i4Ret;
    }
}

static INT32 _jpg_RowRz5391Drv(JPEGHANDLE hFlow, UINT32 u4RowIdx, 
    UINT32 u4MCUsPerRow, UINT32 u4TotalRows, UINT8 **apu1OutputAddr)
{
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;
    IMG_LIB_INFO_T *prImg = prJpgImg;
    ENUM_IMG_ROTATE_PHASE eRotateDegree = eJpgRotateDegree;
    IMG_LIB_DISP_INFO_T *prDispInfo = &prImg->rDispInfo;
    IMG_LIB_MEM_LAYOUT_T *prImgLibMem = &prImg->rMem;
    JMEM_MW_T *prMemMw = NULL;
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    JPEG_VLDJPG_DATA_T *prJpgHw = NULL;
    E_RZ_INOUT_TYPE_T ePreImgrzType;
#ifdef    CC_JPEG_FBM_DYNAMIC_MAPPING
    UINT32 u4DynMapAddr = 0;    
#endif
    
    static UINT32 su4IsRzInited;
    static UINT32 su4RzCurrYAddr, su4RzPrevYAddr;
    static UINT32 su4RzCurrCbAddr, su4RzPrevCbAddr;
    static UINT32 su4RzCurrCrAddr, su4RzPrevCrAddr;
    static UINT32 su4RzYSrcW, su4RzCbSrcW, su4RzCrSrcW;
    static UINT32 su4RzYSrcH, su4RzCbSrcH, su4RzCrSrcH;
    //static UINT32 su4RzYTgW, su4RzYTgH, su4RzCTgW, su4RzCTgH;
    static UINT32 su4RzYSrcL;
    static UINT32 su4PreOutAddr6,su4PreOutAddr7,su4PreOutAddr8;	
    UINT32 au4RowSize[3];
    UINT32 u4DispW, u4DispH;
    INT32 i4Ret = (INT32)E_JPG_OK;
    UINT32 u4SleepCnt = 0, u4NonSleepCnt = 0,u4RzOutputZeroCnt=0;
    
    ASSERT(hFlow);
    ASSERT(apu1OutputAddr != NULL);

    VERIFY(VLDJPG_SetCurrMcuRowNum5391(u4RowIdx) == (INT32)E_HWJPG_OK);

    x_memset((void *)&rJpgParam, 0, sizeof(RZ_JPG_SCL_PARAM_SET_T));
    
    if (!u4RowIdx)
    {
        su4IsRzInited = FALSE;
    }
    
    if (prImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemYuv != NULL);
        su4RzYSrcW  = prMemYuv->au4RowWidth[0];
        su4RzCbSrcW = prMemYuv->au4RowWidth[1];
        su4RzCrSrcW = prMemYuv->au4RowWidth[2];
        su4RzYSrcH  = prMemYuv->au4RowHeight[0];
        su4RzCbSrcH = prMemYuv->au4RowHeight[1];
        su4RzCrSrcH = prMemYuv->au4RowHeight[2];
        _JDEC_SetRzV(&rJpgParam, prMemYuv->au4RowHeight);   
        au4RowSize[0] = prMemYuv->au4RowSize[0];
        au4RowSize[1] = prMemYuv->au4RowSize[1];
        au4RowSize[2] = prMemYuv->au4RowSize[2];
    }
    else
    {    
        prMemMw = (JMEM_MW_T *)(JPGMEM->pvLocalData);
        ASSERT(prMemMw != NULL);
        su4RzYSrcW  = prMemMw->au4RowWidth[0];
        su4RzCbSrcW = prMemMw->au4RowWidth[1];
        su4RzCrSrcW = prMemMw->au4RowWidth[2];
        su4RzYSrcH  = prMemMw->au4RowHeight[0];
        su4RzCbSrcH = prMemMw->au4RowHeight[1];
        su4RzCrSrcH = prMemMw->au4RowHeight[2];
        _JDEC_SetRzV(&rJpgParam, prMemMw->au4RowHeight);
        au4RowSize[0] = prMemMw->au4RowSize[0];
        au4RowSize[1] = prMemMw->au4RowSize[1];
        au4RowSize[2] = prMemMw->au4RowSize[2];
    }    

    prJpgHw = ((JPEG_VLDJPG_DATA_T*)H_JPGHW);
    ASSERT(prJpgHw != NULL);
    su4RzYSrcL  = prJpgHw->u4CompPitchRow[0];

    if (((u4RowIdx % 2) == 0) || (_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ)||(_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
    {
    	JPGMEM->pfnGetOutput(H_JPGMEM, (void**)apu1OutputAddr);

    	TRY2(VLDJPG_SetMcuRowCurr(H_JPGHW, u4RowIdx));

    	// output bank0
    	TRY2(VLDJPG_SetMcuRowOutBuf(H_JPGHW, 0, apu1OutputAddr[0], au4RowSize[0]));
    	TRY2(VLDJPG_SetMcuRowOutBuf(H_JPGHW, 1, apu1OutputAddr[1], au4RowSize[1]));
    	TRY2(VLDJPG_SetMcuRowOutBuf(H_JPGHW, 2, apu1OutputAddr[2], au4RowSize[2]));

        // output bank1
    	TRY2(VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 0, apu1OutputAddr[6], au4RowSize[0]));
    	TRY2(VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 1, apu1OutputAddr[7], au4RowSize[1]));
    	TRY2(VLDJPG_SetMcuRowOutBufBank15391(H_JPGHW, 2, apu1OutputAddr[8], au4RowSize[2]));

	
    	TRY2( VLDJPG_ValidateMcuRowData5391(H_JPGHW, u4MCUsPerRow, u4TotalRows)	);

        // for imgrz
        if (u4RowIdx == 0)
        {
#ifdef JPGHW_TIME_PROFILE        
        _rDecAcc.u4Seconds = 0;
    	_rDecAcc.u4Micros  = 0;
	    _rRZAcc.u4Seconds = 0;
    	_rRZAcc.u4Micros  = 0;
#endif    	
            su4RzPrevYAddr  = (UINT32)apu1OutputAddr[0];
            su4RzPrevCbAddr = (UINT32)apu1OutputAddr[1];
            su4RzPrevCrAddr = (UINT32)apu1OutputAddr[2];
        }
        else
        {
            su4RzPrevYAddr  = su4RzCurrYAddr;
            su4RzPrevCbAddr = su4RzCurrCbAddr;
            su4RzPrevCrAddr = su4RzCurrCrAddr;
        }
        
        su4RzCurrYAddr  = (UINT32)apu1OutputAddr[0];
        su4RzCurrCbAddr = (UINT32)apu1OutputAddr[1];
        su4RzCurrCrAddr = (UINT32)apu1OutputAddr[2];
        
        if((_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ)||(_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))
        {
            su4RzPrevYAddr  = (UINT32)apu1OutputAddr[6];
            su4RzPrevCbAddr = (UINT32)apu1OutputAddr[7];
            su4RzPrevCrAddr = (UINT32)apu1OutputAddr[8];
        }
        
        su4PreOutAddr6 = (UINT32)apu1OutputAddr[6];
        su4PreOutAddr7 = (UINT32)apu1OutputAddr[7];
        su4PreOutAddr8 = (UINT32)apu1OutputAddr[8];
    }
    else
    {
        // for imgrz
        su4RzPrevYAddr  = su4RzCurrYAddr;
        su4RzPrevCbAddr = su4RzCurrCbAddr;
        su4RzPrevCrAddr = su4RzCurrCrAddr;

        su4RzCurrYAddr  = su4PreOutAddr6;//(UINT32)apu1OutputAddr[6];
        su4RzCurrCbAddr = su4PreOutAddr7;//(UINT32)apu1OutputAddr[7];
        su4RzCurrCrAddr = su4PreOutAddr8;//(UINT32)apu1OutputAddr[8];
    }
    
	// decode
#ifdef JPGHW_TIME_PROFILE	
	HAL_GetTime(&_rDecStart);
#endif
    if ((_u4JPGMode != (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ)&&(_u4JPGMode != (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))        //mcu row mode
    {
        i4Ret = VLDJPG_DecodeOneMcuRow5391(H_JPGHW, u4RowIdx);
        rJpgParam.u4IsRsOut = 1;  //static jpeg is raster out
        rJpgParam.u4IsJpgRzOn = 0;    // diable jpg-rz hand-off
        rJpgParam.u4OutSwap = 0;         
    }
    else                                                //pic mode
    {
    
        rJpgParam.u4IsJpgRzOn = 1;    // enable jpg-rz hand-off
        if(prImg->u1MemClass == EXTERNAL_RING_IN_DOUBLE_OUT)
        {
#if defined(CC_MT5395) || defined(JPEG_IS_POST_MT5368)
            rJpgParam.u4IsRsOut = 1;
            rJpgParam.u4OutSwap = 0;
#else
            rJpgParam.u4IsRsOut = 0;  //motion jpeg is block out  
            rJpgParam.u4OutSwap =(UINT32)IMGRZ_SWAP;
#endif
        }
        else
        {
            rJpgParam.u4IsRsOut = 1;  //linux turnkey demo 
            rJpgParam.u4OutSwap =0;
        }
    }

    
    if ((eRotateDegree != CLOCKWISE_0) && (rJpgParam.u4JpgVfacY > rJpgParam.u4JpgVfacCb))
    {
        //422V->444, 420->422H, for generating the identical output of Y & C
        int i;
        rJpgParam.u4JpgVfacCb = rJpgParam.u4JpgVfacCr = rJpgParam.u4JpgVfacY;
        su4RzCbSrcH = su4RzCrSrcH = su4RzYSrcH;
    #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
            u4DynMapAddr = (UINT32)BSP_MapReservedMem((void *)su4RzCurrCbAddr, (su4RzCbSrcH*su4RzYSrcL + su4RzCrSrcH*su4RzYSrcL));
            LOG(7, "%s %d color buffer dynamic map u4DynMapAddr:0x%x\n", __FUNCTION__,__LINE__,u4DynMapAddr);
            if(!u4DynMapAddr)
            {
                LOG(0, "jpeg dynamic map addr fail\n");
                ASSERT(0);
            }
    #endif
        for (i = 7; i>=0; i--)
        {
            x_memcpy((VOID *)VIRTUAL((UINT32)su4RzCurrCbAddr+(i*2*su4RzYSrcL)),
                (VOID *)VIRTUAL(((UINT32)su4RzCurrCbAddr+(i*su4RzYSrcL))),su4RzYSrcL);
            x_memcpy((VOID *)VIRTUAL(((UINT32)su4RzCurrCbAddr+((i*2+1)*su4RzYSrcL))),
                (VOID *)VIRTUAL((UINT32)su4RzCurrCbAddr+(i*su4RzYSrcL)),su4RzYSrcL);
            x_memcpy((VOID *)VIRTUAL(((UINT32)su4RzCurrCrAddr+(i*2*su4RzYSrcL))),
                (VOID *)VIRTUAL((UINT32)su4RzCurrCrAddr+(i*su4RzYSrcL)),su4RzYSrcL);
            x_memcpy((VOID *)VIRTUAL((UINT32)su4RzCurrCrAddr+((i*2+1)*su4RzYSrcL)),
                (VOID *)VIRTUAL((UINT32)su4RzCurrCrAddr+(i*su4RzYSrcL)),su4RzYSrcL);
        }
		
#if defined(CC_MT5365) || defined(CC_MT5395) ||defined(JPEG_IS_POST_MT5368)
    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(su4RzCurrCbAddr), (UINT32)(su4RzCbSrcH*su4RzYSrcL));
    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(su4RzCurrCrAddr), (UINT32)(su4RzCrSrcH*su4RzYSrcL));
#else
    HalFlushInvalidateDCache();
#endif
    
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
    if(u4DynMapAddr)
    {
        LOG(7, "color bur unmap  u4DynMapAddr:0x%x\n", u4DynMapAddr);
        BSP_UnMapReservedMem((void *)u4DynMapAddr);
        u4DynMapAddr = 0;
    }
#endif
    }
#ifdef JPGHW_TIME_PROFILE	
    HAL_GetTime(&_rDecEnd);
    HAL_GetDeltaTime(&_rDecdif, &_rDecStart, &_rDecEnd);
	_rDecAcc.u4Seconds += _rDecdif.u4Seconds;
	_rDecAcc.u4Micros  += _rDecdif.u4Micros;
	if (_rDecAcc.u4Micros >= 1000000)
	{
		_rDecAcc.u4Seconds += _rDecAcc.u4Micros / 1000000;
		_rDecAcc.u4Micros  %= 1000000;
	}
#endif

#if 1 // use imgrz
#ifdef JPGHW_TIME_PROFILE	
	//HAL_GetTime(&_rRZStart);
#endif    

    LOG(7, "RZ prev Y  addr = 0x%08x \n", su4RzPrevYAddr);
    LOG(7, "RZ prev Cb addr = 0x%08x \n", su4RzPrevCbAddr);
    LOG(7, "RZ prev Cr addr = 0x%08x \n", su4RzPrevCrAddr);
    LOG(7, "RZ curr Y  addr = 0x%08x \n", su4RzCurrYAddr);
    LOG(7, "RZ curr Cb addr = 0x%08x \n", su4RzCurrCbAddr);
    LOG(7, "RZ curr Cr addr = 0x%08x \n", su4RzCurrCrAddr);

    rJpgParam.u4IsRsIn = 1;        // block|raster based input
    rJpgParam.u4YSrcBase1 = su4RzCurrYAddr;     // y1
    rJpgParam.u4YSrcBase2 = su4RzPrevYAddr;     // y2
    rJpgParam.u4YSrcBufLen = su4RzYSrcL;
    rJpgParam.u4CSrcBufLen = su4RzYSrcL;    
    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW = prImg->rHdrInfo.u4Width;
    rJpgParam.u4YSrcH = (su4RzYSrcH == su4RzCbSrcH) ? prImg->rHdrInfo.u4Height :(prImg->rHdrInfo.u4Height&0xfffffffe);
    rJpgParam.u4CbSrcBase1 = su4RzCurrCbAddr;    // cb1
    rJpgParam.u4CbSrcBase2 = su4RzPrevCbAddr;    // cb2
    rJpgParam.u4CbSrcHOffset = 0;
    rJpgParam.u4CbSrcVOffset = 0;
    
    if(su4RzYSrcW == su4RzCbSrcW)
    {
        rJpgParam.u4CbSrcW = rJpgParam.u4YSrcW;
    }
    else if (su4RzYSrcW == su4RzCbSrcW*2)
    {
        rJpgParam.u4CbSrcW = rJpgParam.u4YSrcW/2;
    }
    else
    {
        rJpgParam.u4CbSrcW = rJpgParam.u4YSrcW/4;
    }
    rJpgParam.u4CbSrcW = (rJpgParam.u4CbSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcW;

    if(su4RzYSrcH == su4RzCbSrcH)
    {
        rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH;
    }
    else if (su4RzYSrcH == su4RzCbSrcH*2)
    {
        rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH/2;
    }
    else
    {
        rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH/4;
    }
    rJpgParam.u4CbSrcH = (rJpgParam.u4CbSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CbSrcH;
    rJpgParam.u4CrSrcBase1 = su4RzCurrCrAddr;    // cr1
    rJpgParam.u4CrSrcBase2 = su4RzPrevCrAddr;    // cr2
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;

    if(su4RzYSrcW == su4RzCrSrcW)
    {
        rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW;
    }
    else if (su4RzYSrcW == su4RzCrSrcW*2)
    {
        rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW/2;
    }
    else
    {
        rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW/4;
    }
    rJpgParam.u4CrSrcW = (rJpgParam.u4CrSrcW == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcW;
    if(su4RzYSrcH  == su4RzCrSrcH)
    {
        rJpgParam.u4CrSrcH = rJpgParam.u4YSrcH;
    }
    else if (su4RzYSrcH == su4RzCrSrcH*2)
    {
        rJpgParam.u4CrSrcH = rJpgParam.u4YSrcH/2;
    }
    else
    {
        rJpgParam.u4CrSrcH = rJpgParam.u4YSrcH/4;
    }
    rJpgParam.u4CrSrcH = (rJpgParam.u4CrSrcH == (UINT32)0)? (UINT32)1 : rJpgParam.u4CrSrcH;

    rJpgParam.fgPreload = 0;
    if (su4RzCbSrcW == 0 || su4RzCrSrcW == 0 || su4RzCbSrcH == 0 || su4RzCrSrcH == 0)     
    {
        rJpgParam.u4CbSrcW = rJpgParam.u4CrSrcW = rJpgParam.u4YSrcW;
        rJpgParam.u4CrSrcH = rJpgParam.u4CbSrcH = rJpgParam.u4YSrcH;
        rJpgParam.fgGray = 1;
    }
    else
    {
        rJpgParam.fgGray = 0;
    }
    if (rJpgParam.u4YSrcW < 4)rJpgParam.u4YSrcW = 4;
    if (rJpgParam.u4CbSrcW < 4)rJpgParam.u4CbSrcW = 4;
    if (rJpgParam.u4CrSrcW < 4)rJpgParam.u4CrSrcW = 4;
    rJpgParam.u4ColorComp = 0x7;
    u4DispW = prDispInfo->u4ScaleW;
    u4DispH = prDispInfo->u4ScaleH;
    rJpgParam.u4YTgW = (u4DispW > 4)? u4DispW : 4;
    rJpgParam.u4YTgH = u4DispH;
    if (eRotateDegree == CLOCKWISE_0)    //directly output to display buffer    
    {   
        if((prImg->u1MemClass == (UINT8)RING_IN_SINGLE_OUT)&&(prImg->rDispInfo.u4DisplayClrMode==ARGB4444_DIRECT16))
        {
        	//for argb4444 target, covert :  yuv -> aycbcr8888  ->argb4444
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[0];		
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart_temp[0];
			rJpgParam.u4IsJpgSameaddr = 0;
			rJpgParam.u4YTgHOffset = prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = prDispInfo->u4StartY[0];
			//aycbcr8888 DispPitch is 2 times than argb4444 DispPitch
			rJpgParam.u4YTgBufLen = prDispInfo->u4DispPitch << 1;
			rJpgParam.u4CTgBufLen = prDispInfo->u4DispPitch << 1;
        }
		else
		{
			rJpgParam.u4YTgBase = (UINT32)prDispInfo->pu1DisplayStart[0];		
			rJpgParam.u4CTgBase = (UINT32)prDispInfo->pu1DisplayStart[1];
			rJpgParam.u4IsJpgSameaddr = 0;
			rJpgParam.u4YTgHOffset = prDispInfo->u4StartX[0];
			rJpgParam.u4YTgVOffset = prDispInfo->u4StartY[0];
			rJpgParam.u4YTgBufLen = prDispInfo->u4DispPitch;
			rJpgParam.u4CTgBufLen = prDispInfo->u4DispPitch;
		}

    }    
    else	//output to scale working buf    
    {   
        if(0)//((prDispInfo->u4DisplayClrMode==ARGB8888_DIRECT32)||(prDispInfo->u4DisplayClrMode==ARGB4444_DIRECT16))
        {
			rJpgParam.u4YTgBase = (UINT32)prImgLibMem->pau1ScDst2Addr[0];		
			rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDst2Addr[1];
			rJpgParam.u4YTgBufLen = (u4DispW + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = (u4DispW + 0xF) & (~0xF);
        }
		else
		{
			rJpgParam.u4YTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[0];
			rJpgParam.u4YTgBufLen = ((u4DispW << 2) + 0xF) & (~0xF);
			rJpgParam.u4CTgBufLen = ((u4DispW << 2) + 0xF) & (~0xF);
		}
        rJpgParam.u4IsJpgSameaddr = 1;
        rJpgParam.u4YTgHOffset = 0;
        rJpgParam.u4YTgVOffset = 0;        
        if ((prDispInfo->u4DisplayClrMode == AYCBCR8888_DIRECT32) || (prDispInfo->u4DisplayClrMode == ARGB8888_DIRECT32) 
			|| (prDispInfo->u4DisplayClrMode == ARGB4444_DIRECT16))   
        {
           rJpgParam.u4CTgBase = (UINT32)rJpgParam.u4YTgBase ;
        }
        else
        {
            rJpgParam.u4CTgBase = (UINT32)prImgLibMem->pau1ScDstAddr[1];
        }
    }
	
    if (prImg->u1MemClass == RING_IN_SINGLE_OUT) 
    {
        rJpgParam.u4IsVdo2Osd = 1;     // output in osd format ?        
        rJpgParam.u4CTgH = u4DispH;

        switch (prDispInfo->u4DisplayClrMode)
        {
        case AYCBCR8888_DIRECT32:	            
        case ARGB8888_DIRECT32:
		case ARGB4444_DIRECT16:
            rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
            rJpgParam.u4CTgW = u4DispW;
            break;
	 	default:
 	    	rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_CbYCrY;
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;    
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4IsVdo2Osd = 0;  // output in video format
	     break;
        }
    }
    else
    {
        rJpgParam.u4IsVdo2Osd = 0;     // output in VDO format ?
        rJpgParam.u4YTgCM = 0;
        switch (prDispInfo->u4DisplayClrMode)
        {
        //case E_JPEG_YUV420:
        //case E_JPEG_YUV422V:
        case Y_CBCR420_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH >> 1;
            break;
        //case E_JPEG_YUV422:
        //case E_JPEG_YUV444:
        case Y_CBCR422_SEP16:
            rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_422;
            rJpgParam.u4CTgW = u4DispW >> 1;
            rJpgParam.u4CTgH = u4DispH;
            break;
        default:
          LOG(0,"Can NOT support this jpg fmt in video plane = %u\n", prDispInfo->u4DisplayClrMode);
          ASSERT(0);
          break;
        }
    }

    LOG(7, "RZ target Y addr = 0x%08x \n", rJpgParam.u4YTgBase);
    LOG(7, "RZ target C addr = 0x%08x \n", rJpgParam.u4CTgBase);

    rJpgParam.u4IsFstBl = (u4RowIdx == (UINT32)0)? (UINT32)1 : (UINT32)0;
    rJpgParam.u4IsLstBl = (u4RowIdx == (UINT32)(u4TotalRows - 1))? (UINT32)1 : (UINT32)0;

    //rJpgParam.u4IsJpgRzOn = 0;    // diable jpg-rz hand-off

    rJpgParam.u4JpgTempAddr = _u4JpgRzTempBuf;

    #ifdef JPG_DECODING_USING_RZ_IOMMU
	rJpgParam.u4RzIOMMU_MMUTableAddr = prDispInfo->u4JpgUsingRzIOMMU_DispMMUTableAddr;
    #else
	rJpgParam.u4RzIOMMU_MMUTableAddr = 0;    
    #endif

    IMGRZ_Lock();
    if (su4IsRzInited == FALSE)
    { 
        IMGRZ_ReInit();
        su4IsRzInited = TRUE;
    } 
    else
    {
        ePreImgrzType = IMGRZ_GetScaleOpt();
        if (ePreImgrzType != E_RZ_INOUT_TYPE_JPGMD)         //break
        {
            IMGRZ_Unlock();
            su4IsRzInited = FALSE;
            TRY2((INT32)E_HWJPG_BUSY);
        }
    }
 
    #ifdef JPG_DECODING_USING_RZ_IOMMU
    if((rJpgParam.u4RzIOMMU_MMUTableAddr != 0) && (prImg->rDispInfo.u4DisplayClrMode != ARGB4444_DIRECT16))
    {
		IMGRZ_Cfg_MMU_IO((BOOL)0,(BOOL)1);
    }		
    #endif
 
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale((void *)(&rJpgParam));   
#ifdef JPGHW_TIME_PROFILE	
	HAL_GetTime(&_rRZStart);
#endif  
    IMGRZ_Flush();
    if ((_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ)||(_u4JPGMode == (UINT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT))              //Pic mode
    {
#if 1
        JPGFLW->u4DecodedSize=0;
        VLDJPG_DecodeOnePic(H_JPGHW);
        while (VLDJPG_CheckDecodePicFinish(H_JPGHW) != (INT32)E_HWJPG_OK)
        {            
            u4NonSleepCnt++;
#ifdef JPEG_AUTO_TEST
            if(_JpgVerifyCmdGetParam(19,0))
            {
                Printf("VLDJPG_SetStop()\n");
                VLDJPG_SetStop(H_JPGHW);
            }
#endif
            if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
            {
                _jpg_UpdateReadPtr(hFlow);
				LOG(7,"Decoding Progress=%d%\n",JDEC_GetProgress(hFlow));
                if (JPGHDR->u4RingData < (UINT32)(200 * 1024))
                {
                    LOG(3, "JPGHDR->u4RingData:%d\n", JPGHDR->u4RingData);
                    IMG_BsBufManager(prImg->ucImgId);  
                    u4RzOutputZeroCnt=0;
                }
                else
                {
                    x_thread_delay(1);
                    if(IMGRZ_GetScaleLine()==0)
                    {
                       u4RzOutputZeroCnt++;
                    }
                    else
                    {
                       u4RzOutputZeroCnt=0;
                    }
                }
                u4SleepCnt++;  
                u4NonSleepCnt = 0;
            }     
            else
            {
               if(IMGRZ_GetScaleLine()==0)
               {
               	 x_thread_delay(1);
                 u4RzOutputZeroCnt++;
                 u4SleepCnt++;  
               }
               else
               {
                 u4RzOutputZeroCnt=0;
               }         	
            }
            if (VLDJPG_CheckDecodingError(H_JPGHW) != (INT32)E_HWJPG_OK)
            {
                i4Ret = -(INT32)E_JPG_ERR_FATAL;
				LOG(3, "%s, line %d decoding error\n", __FUNCTION__, __LINE__);
                break;
            }
            if (u4NonSleepCnt >= (UINT32)0x80000)
            {
                x_thread_delay(1);
                u4SleepCnt++;  
                u4NonSleepCnt = 0;
                if (VLDJPG_CheckStop(H_JPGHW) != (INT32)E_HWJPG_OK)
                {
                    i4Ret = -(INT32)E_JPG_ERR_FATAL;
                    LOG(3, "time out 0 already stop\n");
                    break;
                }
            }
            if (u4SleepCnt >= (UINT32)30000 || (u4RzOutputZeroCnt>=((rJpgParam.u4YSrcH/u4DispH+10)*2)))
            {
                i4Ret = -(INT32)E_JPG_ERR_FATAL;
                LOG(3, "Time out 1 JPGHDR->u4RingData:%d\n", JPGHDR->u4RingData);
                break;
            }
            if (_fgJpgFlw2Stop)
            {
                i4Ret = -(INT32)E_JPG_ERR_FATAL;
				LOG(3, "time out _fgJpgFlw2Stop\n");
                break;
            }
        }
        
        prJpgImg->u4RowIdx = (UINT32)(u4TotalRows - 1);
        u4RowIdx = prJpgImg->u4RowIdx;
#else
        i4Ret = VLDJPG_DecodeOnePic5391(H_JPGHW, u4RowIdx);
#endif

        if (i4Ret)
        {
            su4IsRzInited = FALSE;
            prJpgImg->u4RowIdx = (UINT32)(u4TotalRows - 1);

            IMGRZ_UnlockWaitIsr();
            IMGRZ_UnlockCmdque();
            IMGRZ_Unlock();
            VLDJPG_SetStop(H_JPGHW);
            VLDJPG_SoftwareReset();
            IMGRZ_Reset();
            TRY2(i4Ret);
        }
    }
    IMGRZ_Wait();
    prDispInfo->u4ScaleRowH =  IMGRZ_GetScaleLine();
    IMGRZ_Unlock();    

#ifdef JPEG_VLD_V4
	LOG(1, "Jpgdec Input CKsum = [%x]\n", VLDJPG_ReadInputCKsum());
        LOG(6, "Jpgdec Bs Input Cksum = [%x]\n", VLDJPG_BsInputCKsum());
        LOG(6, "Jpgdec Huff Chksum = [%x]\n", VLDJPG_HuffCKsum());
        LOG(6, "Jpgdec Vld Out Chksum = [%x]\n", VLDJPG_VldOutCKsum());        
        LOG(6, "Jpgdec Idct out Chksum = [%x]\n", VLDJPG_IdctOutCKsum());
        LOG(6, "Jpgdec Coef Chksum = [%x]\n", VLDJPG_CoefCKsum());
        LOG(6, "Jpgdec Nz In Cksum = [%x]\n", VLDJPG_NzInCKsum());
        LOG(6, "Jpgdec Nz Out Cksum = [%x]\n", VLDJPG_NzOutCKsum());        
	LOG(1, "Jpgdec CKsum = [%x]\n", VLDJPG_ReadCKsum());
	LOG(1, "Jpgdec CRC = [%x]\n", VLDJPG_ReadCRC());
	LOG(1, "Error detection type = [%x]\n", VLDJPG_ReadErrorType());
#endif

#ifdef JPGHW_TIME_PROFILE	
    HAL_GetTime(&_rRZEnd);
    HAL_GetDeltaTime(&_rRZdif, &_rRZStart, &_rRZEnd);
	_rRZAcc.u4Seconds += _rRZdif.u4Seconds;
	_rRZAcc.u4Micros  += _rRZdif.u4Micros;
	if (_rRZAcc.u4Micros >= 1000000)
	{
		_rRZAcc.u4Seconds += _rRZAcc.u4Micros / 1000000;
		_rRZAcc.u4Micros  %= 1000000;
	}
#endif

    
    if (u4RowIdx == (u4TotalRows - 1))
    {
        su4IsRzInited = FALSE;
#ifdef JPGHW_TIME_PROFILE	        
        LOG(0, "decoding %u.%06u/RZ %u.%06u\n",
               _rDecAcc.u4Seconds, _rDecAcc.u4Micros, 
               _rRZAcc.u4Seconds, _rRZAcc.u4Micros);        
#endif
        VLDJPG_SetPowerOff();
    }
#endif

	// update read pointer and data size of ring buffer
	if (JPGHDR->fgRingBuffer && (!JPGHDR->fgEOF))
	{
		_jpg_UpdateReadPtr(hFlow);	
	}
	
	JPGFLW->u4DecodedSize=JPGFLW->u4FileSize;
	LOG(7,"Decoding Progress=%d%\n",JDEC_GetProgress(hFlow));
	CATCH_ERROR()
	{   
		return i4Ret;
	}
}

extern INT32 vJpegProgDec(JPEGHANDLE hParser, void **pvCoefBuffer, void **pvOutBuffer, 
                            void **pvNzBuffer);

INT32 JDEC_SwitchToProgDec(JPEGHANDLE hFlow, IMG_LIB_MEM_LAYOUT_T *prMem)
{
    JMEM_YUV_LINEAR_T *prMemYuv = NULL;
    JMEM_BASE_T* prMemBase;
    VOID *pavComp[9];
    VOID *pvCoefBuffer[3];
    UINT32 au4AllocSize[3], au4CompUnitWidth[3], au4CompUnitHeight[3], au4CompSize[3];
    INT32 i4Ret = 0;

    prMemBase = (JMEM_BASE_T*)JPGMEM->pvLocalData;
    if (!prMemBase->fgProgScan)
    {
        return -1;
    }
    if (prJpgImg->u1MemClass < RING_IN_SINGLE_OUT)
    {
        prMemYuv = (JMEM_YUV_LINEAR_T *)(JPGMEM->pvLocalData);
        UNUSED(prMemYuv);
        //i4Ret = vJpegProgDec(JPGHDR, prMemYuv->pvCoefBuffer, prMemYuv->pvComp, JPGMEMBS->apvNzBuffer);
    }
    else
    {
        au4CompUnitWidth[0] = prMemBase->aau4CompFactor[0][0] * D_DCTSIZE;
	    au4CompUnitWidth[1]	= prMemBase->aau4CompFactor[1][0] * D_DCTSIZE;
	    au4CompUnitWidth[2] = prMemBase->aau4CompFactor[2][0] * D_DCTSIZE;
	    au4CompUnitHeight[0] = prMemBase->aau4CompFactor[0][1] * D_DCTSIZE;
	    au4CompUnitHeight[1] = prMemBase->aau4CompFactor[1][1] * D_DCTSIZE;
	    au4CompUnitHeight[2] = prMemBase->aau4CompFactor[2][1] * D_DCTSIZE;
	    au4CompSize[0] = au4CompUnitWidth[0] * au4CompUnitHeight[0];
	    au4CompSize[1] = au4CompUnitWidth[1] * au4CompUnitHeight[1];
	    au4CompSize[2] = au4CompUnitWidth[2] * au4CompUnitHeight[2];        
        pavComp[0] = prMem->pavComp[0];
        pavComp[1] = prMem->pavComp[1];
        pavComp[2] = prMem->pavComp[2];
        pavComp[6] = prMem->pavComp[3];
        pavComp[7] = prMem->pavComp[4];
        pavComp[8] = prMem->pavComp[5];
        au4AllocSize[0] = prMemBase->u4McuCount * au4CompSize[0];
        au4AllocSize[1] = prMemBase->u4McuCount * au4CompSize[1];
        au4AllocSize[2] = prMemBase->u4McuCount * au4CompSize[2];
        
        pvCoefBuffer[0] = (VOID *)((prMem->u4CurrentUse + 15) & ~15);
        if (((JPGHDR->rSOF.u2ImageWidth * JPGHDR->rSOF.u2ImageHeight) <= JPG_COLLECT_LIMIT) 
			&& (JPGHDR->rSOF.u2ImageWidth <= JPG_COLLECT_WIDTH_LIMIT))//collect mode, the  width is limited by the working buffer size. 
        {
            pvCoefBuffer[1] = (VOID *)((UINT32)pvCoefBuffer[0] + (au4AllocSize[0] << 1));
            pvCoefBuffer[2] = (VOID *)((UINT32)pvCoefBuffer[1] + (au4AllocSize[1] << 1));
            prMemBase->apvNzBuffer[0] = (VOID *)((UINT32)pvCoefBuffer[2] + (au4AllocSize[2] << 1));
        }
        else
        {
            UINT32 u4Size;
            u4Size = (prMemBase->u4McuWidth * au4CompSize[0]) << 1;
            pvCoefBuffer[2] = pvCoefBuffer[1] = pvCoefBuffer[0];
            prMemBase->apvNzBuffer[0] = (VOID *)((UINT32)pvCoefBuffer[2] + u4Size);
        }
        
        prMemBase->apvNzBuffer[1] = (VOID *)((((UINT32)prMemBase->apvNzBuffer[0] + (au4AllocSize[0] >> 3)) + 15) & ~15);
        prMemBase->apvNzBuffer[2] = (VOID *)((((UINT32)prMemBase->apvNzBuffer[1] + (au4AllocSize[1] >> 3)) + 15) & ~15);
        prMem->u4CurrentUse = (UINT32)prMemBase->apvNzBuffer[2] + (au4AllocSize[2] >> 3);
        i4Ret = vJpegProgDec(JPGHDR, pvCoefBuffer, pavComp, prMemBase->apvNzBuffer);
    }

    return i4Ret;
}

UINT16 bDitherMatrix[4][4] = 
{
{0, 8, 2, 10},
{12, 4, 14, 6},
{3, 11, 1, 9},
{15, 7, 13, 5}
};
#if 0
void _jpg_RGB2Gamma(UINT32 u4Addr, UINT32 u4Width, UINT32 u4Height, UINT32 u4OffsetX, 
                    UINT32 u4OffsetY, UINT32 u4Pitch)
{
    UINT32 u4WidthMinus1, u4HeightMinus1,u4Jump, u4ActualWidth = u4Pitch / 4;
    INT32 *pi4Buf = (INT32 *)u4Addr;        
    INT32 i4TempValue;    
    INT32 i4R, i4G, i4B, i4A, i4Y, i4Cb, i4Cr, i4I, i4J;//, i4ILow3;    
    UINT8 *pu1Range = _acRange;   
#ifdef JPGHW_TIME_PROFILE	
    HAL_TIME_T _rRGBStart, _rRGBEnd, _rRGBdif;
    HAL_GetTime(&_rRGBStart); 
#endif
    pu1Range += 256; 
    pi4Buf = (INT32 *)(u4Addr + (u4OffsetY * u4Pitch) + (u4OffsetX * 4));  
    u4WidthMinus1 = u4Width - 1;
    u4HeightMinus1 = u4Height - 1;
    u4Jump = u4ActualWidth - u4Width;

    for (i4I = u4HeightMinus1; i4I >= 0 ; i4I--)    
    {
        //i4ILow3 = i4I & 0x3;
        for (i4J = u4WidthMinus1; i4J >= 0; i4J--)        
        {            
            i4TempValue = *pi4Buf;
            i4A = (i4TempValue & 0xff000000) >> 24;            
            i4Y = (i4TempValue & 0x00ff0000) >> 16;            
            i4Cb = (i4TempValue & 0x0000ff00) >> 8;            
            i4Cr = (i4TempValue & 0x000000ff);             
            
            i4R = (INT32)pu1Range[i4Y + _ai4RR[i4Cr]];  
            //i4R = (INT32)(_au2GammaR[i4R] + bDitherMatrix[i4ILow3][i4J&3]) >> 4;
            i4R = (INT32)pu1Range[i4R];  
                        
            i4G = (INT32)pu1Range[i4Y + ((_ai4BG[i4Cb] + _ai4RG[i4Cr]) >> 16)];   
            //i4G = (INT32)(_au2GammaG[i4G] + bDitherMatrix[i4ILow3][i4J&3]) >> 4;
            i4G = (INT32)pu1Range[i4G];  
                        
            i4B = (INT32)pu1Range[i4Y + _ai4BB[i4Cb]];
           // i4B = (INT32)(_au2GammaB[i4B] + bDitherMatrix[i4ILow3][i4J&3]) >> 4;
            i4B = (INT32)pu1Range[i4B];  
            
            *pi4Buf = (i4A << 24) | (i4R << 16) | (i4G << 8) | i4B;
            pi4Buf++; 
        }
        pi4Buf += u4Jump;
    }
#ifdef JPGHW_TIME_PROFILE	    
    HAL_GetTime(&_rRGBEnd);        
    HAL_GetDeltaTime(&_rRGBdif, &_rRGBStart, &_rRGBEnd);
    LOG(0, "RGB+Gamma %u.%06u\n", _rRGBdif.u4Seconds, _rRGBdif.u4Micros);  
#endif    
}

VOID JDEC_SWGamma(UINT8 *pu1Addr, IMG_LIB_DISP_INFO_T *prDispInfo, UINT32 u4X, UINT32 u4Y)
{    
        if ((eJpgRotateDegree == CLOCKWISE_0) || (eJpgRotateDegree == CLOCKWISE_0_WITH_FLIP) ||
           (eJpgRotateDegree == CLOCKWISE_180) || (eJpgRotateDegree == CLOCKWISE_180_WITH_FLIP))
        {
            _jpg_RGB2Gamma((UINT32)pu1Addr, prDispInfo->u4ScaleW, prDispInfo->u4ScaleH, 
                           u4X, u4Y, prDispInfo->u4DispPitch);
        }
        else
        {
            _jpg_RGB2Gamma((UINT32)pu1Addr, prDispInfo->u4ScaleH, prDispInfo->u4ScaleW,
                           u4X, u4Y, prDispInfo->u4DispPitch);
        }
}
#endif

void VLDJPG_MMU_init(void)
{
#ifdef JPEG_VLD_V4
	#ifdef JDEC_HW_IO_MMU
		VLDJPG_SetMMU();
	#endif
#endif
}


BOOL JDEC_CkgenOnOff(BOOL fgEnable)
{
#if defined(CC_MT5398)||defined(CC_MT5880) || defined(CC_MT5881)||defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
#define BLOCK_RST_CFG0  0x1C0
#define JPG_RST  ((UINT32)1 << 8)
#define BLOCK_CKEN_CFG0 0x1C8
#define JPG_CKEN ((UINT32)1 << 8)
#else
#define BLOCK_RST_CFG0 0x270
#define JPG_RST ((UINT32)1 << 2)
#define BLOCK_CKEN_CFG0 0x278
#define JPG_CKEN ((UINT32)1 << 2)
#endif
    if (fgEnable)
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~JPG_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) | JPG_CKEN);
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) | JPG_RST);
    }
    else
    {
        IO_WRITE32(CKGEN_BASE, BLOCK_RST_CFG0, IO_READ32(CKGEN_BASE, BLOCK_RST_CFG0) & ~JPG_RST);
        IO_WRITE32(CKGEN_BASE, BLOCK_CKEN_CFG0, IO_READ32(CKGEN_BASE, BLOCK_CKEN_CFG0) & ~JPG_CKEN);
    }

    UNUSED(fgEnable);
    return TRUE;
}

#ifdef CC_FUJI3D_SUPPORT
void JDEC_SetMPOParsing(JPEGHANDLE hFlow, BOOL fgChkMPO)
{
    JPEG_FLOW_DATA_T *prFlow;
    JPEG_JFIF_HEADER_T *prJfif;
    
    ASSERT(hFlow);
    prFlow = (JPEG_FLOW_DATA_T*)hFlow;
    prJfif = prFlow->hJpegHdr;

    prJfif->fgChkMPO = fgChkMPO;
    return;
}

BOOL JDEC_GetMPOParsing(JPEGHANDLE hFlow, UINT32* pu4MPFOffset)
{
    JPEG_FLOW_DATA_T *prFlow;
    JPEG_JFIF_HEADER_T *prJfif;
    BOOL fgIsSupportedMPF = FALSE;
    
    ASSERT(hFlow);
    ASSERT(pu4MPFOffset);
    
    prFlow = (JPEG_FLOW_DATA_T*)hFlow;
    prJfif = prFlow->hJpegHdr;

    if(prJfif->fgChkMPO 
        && prJfif->rAPP2.fgAPP2 
        && prJfif->rAPP2.fgIsMPF
        && (prJfif->rAPP2.u4NumOfImages == 1))  //only support stereo
    {
        fgIsSupportedMPF = TRUE;
        *pu4MPFOffset = (UINT32)prJfif->rAPP2.apu1MPEntry[0];
    }
    else
    {
        fgIsSupportedMPF = FALSE;
        *pu4MPFOffset = 0;
    }
    return fgIsSupportedMPF;
}

#endif
static UINT32 _u4MJPG_AsyncInit=0;
static UINT32 _u4MJPGInit=0;
static HANDLE_T _hMJPG_AsyncSeam = NULL_HANDLE;
static HANDLE_T _hMJPG_IsrSeam=NULL_HANDLE;
static MJPEG_DECODER *_prCurDecoder=NULL;
BOOL fgIsMJPEGPlaying = FALSE;
extern void IMG_LockHwResource(UCHAR ucImgId);
extern void IMG_UnLockHwResource(UCHAR ucImgId);
extern void u4hSemaphoreDBGInsert(UINT32 type, UINT32 value);
extern UINT32 u4hardwareSema;

static VOID _MJPG_UpdateReadPtr(JPEG_JFIF_HEADER_T *prJpgHeader)
{
    UINT32 u4NewRP;
    UINT32 u4UsedLen;
    u4NewRP = IO_READ32(VLD0_BASE, 0xfc) - PHYSICAL((UINT32) prJpgHeader->pu1RingBufStart);
    if (u4NewRP < prJpgHeader->u4RingRP)
    {
        u4UsedLen = (prJpgHeader->u4RingBufSize + u4NewRP) - prJpgHeader->u4RingRP;
    }
    else
    {
        u4UsedLen = u4NewRP - prJpgHeader->u4RingRP;
    }
	
    if(prJpgHeader->u4RingData < u4UsedLen)
    {
        LOG(3, "BS Buffer data 0x%x less than used data 0x%x.\n", prJpgHeader->u4RingData, u4UsedLen);
    }
    else
    {
        prJpgHeader->u4RingData -= u4UsedLen;
    }
    prJpgHeader->u4RingRP = u4NewRP;		
}

static INT32 _MJPG_RequestData(MJPEG_DECODER *prDecoder)
{
   JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
   MJPEG_DATAREQ_INFO rDataReq;
   UINT32 u4RingWP;
   CRIT_STATE_T crit;
   
   rDataReq.u4ReadPos=0;
   rDataReq.u4SetReadPos=0;
   u4RingWP = prJpegHdr->u4RingRP+prJpegHdr->u4RingData;
   
   if((!prJpegHdr->fgEOF)&&(u4RingWP<prJpegHdr->u4RingBufSize))
   {
	 rDataReq.u4WriteAddr=(UINT32)(prJpegHdr->pu1RingBufStart+u4RingWP);
	 rDataReq.u4ReqSize=prJpegHdr->u4RingBufSize-u4RingWP;
	 prDecoder->fpDevCb(prDecoder->hDevice,MJPEG_CB_REQ_DATA,(UINT32)(&rDataReq),0);
	 prJpegHdr->u4RingData+=rDataReq.u4ReadSize;
	 if(rDataReq.u4Eof)
	 {
		prJpegHdr->fgEOF=TRUE;
	 }
	 else
	 {
	    if(rDataReq.u4ReadSize!=rDataReq.u4ReqSize)
	    {
			return -1;
	    }
	 }
   }

   if(!prJpegHdr->fgEOF)
   {
	 u4RingWP = prJpegHdr->u4RingRP + prJpegHdr->u4RingData;
	 if(u4RingWP<prJpegHdr->u4RingBufSize)
	 {
        return -2;
	 }
	 
         rDataReq.u4WriteAddr=(UINT32)(prJpegHdr->pu1RingBufStart+u4RingWP-prJpegHdr->u4RingBufSize);
	 rDataReq.u4ReqSize= prJpegHdr->u4RingRP-u4RingWP;

	 prDecoder->fpDevCb(prDecoder->hDevice,MJPEG_CB_REQ_DATA,(UINT32)(&rDataReq),0);
	 prJpegHdr->u4RingData+=rDataReq.u4ReadSize;
	 if(rDataReq.u4Eof)
	 {
		prJpegHdr->fgEOF=TRUE;
	 }
	 else
	 {
	    if(rDataReq.u4ReadSize!=rDataReq.u4ReqSize)
	    {
			return -3;
	    }
	 }
   }

        crit = x_crit_start();	//protect u4RingData
	if(prJpegHdr->fgEOF)
	{
		VLDJPG_SetRiscCtrlWP(0xFFFFFFFF);
	}
	else
	{
  // UINT32 ringBufEnd;
//	ringBufEnd = (UINT32)prJpegHdr->pu1RingBufStart + prJpegHdr->u4RingBufSize;
	
		u4RingWP = prJpegHdr->u4RingRP + prJpegHdr->u4RingData;
		u4RingWP = (u4RingWP >= prJpegHdr->u4RingBufSize) ? (u4RingWP -  prJpegHdr->u4RingBufSize) : u4RingWP;
		
                VLDJPG_SetRiscCtrlWP(PHYSICAL((UINT32)prJpegHdr->pu1RingBufStart+u4RingWP) +128);
	}
	x_crit_end(crit);

    if(prJpegHdr->u4RingData>prJpegHdr->u4RingBufSize)
    {
       return -4;
    }
	VLDJPG_SetRingBuffer((void *)prJpegHdr->pu1RingBufStart,prJpegHdr->u4RingBufSize);
      //sync memory/cache
#if 1 //defined(CC_MT5365) || defined(CC_MT5395)
     //HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(prJpegHdr->pu1RingBufStart), (UINT32)(prJpegHdr->u4RingBufSize));
 #else
     HalFlushInvalidateDCache();
 #endif
    return 0;
}
static INT32 _MJPG_FetchRegValue(MJPEG_DECODER *prDecoder)
{
    JPEG_JFIF_HEADER_T *prJpgHeader=&prDecoder->rJpgHeader;
    JPEG_VLDJPG_DATA_T *prJpgData=&prDecoder->rJpgHw;
	MJPEG_DEC_PARAM *prDecParam=&prDecoder->rDecParam;
	UINT32 u4Num=0,u4Index=0;
	INT32 i4Ret=0;

	prJpgData->pu1HwDcHuffTbl=(UINT8 *)prDecParam->u4HufTable[0];
	prJpgData->pu1HwAcHuffTbl=(UINT8 *)prDecParam->u4HufTable[1];
	
	//Glob   _jpg_Glob
	{
	    JPEG_JFIF_SOF_T *prSOF=&prJpgHeader->rSOF;
		if((prSOF->eJpegFormat ==E_JPG_BASELINE)||
		  (prSOF->eJpegFormat ==E_JPG_EX_SEQ_HUFFMAN))			
		{
		   prJpgData->fgProgressive=FALSE;
		}
		else
		{
		   return -1;
		}
		
		prJpgData->fgGlobSet|= D_GLOB_PROGRESSIVE;
		
		u4Num=prSOF->u1NumComponents;
		if(u4Num>3)
		{
			return -2;
		}
		prJpgData->u1NumComps= u4Num;
		prJpgData->fgGlobSet |= D_GLOB_NCOMP;

		prDecParam->u4MaxHFactor=0;
		prDecParam->u4MaxVFactor=0;
		for (u4Index = 0;u4Index < u4Num; u4Index++)
		{
		   UINT32 u4FactorH=0;
		   UINT32 u4FactorV=0;
		   u4FactorH = prSOF->arSofComp[u4Index].u1HSampFactor;
		   u4FactorV = prSOF->arSofComp[u4Index].u1VSampFactor;
		   prJpgData->au1CompBlkWidth[u4Index] = u4FactorH;
		   prJpgData->fgGlobSet|= D_GLOB_BLK_W;
		   prJpgData->au1CompBlkHeight[u4Index] = u4FactorV;
		   prJpgData->fgGlobSet |= D_GLOB_BLK_H;
		   prDecParam->u4MaxHFactor=MAX(prDecParam->u4MaxHFactor,u4FactorH);
		   prDecParam->u4MaxVFactor=MAX(prDecParam->u4MaxVFactor,u4FactorV);
		   prDecParam->au4CompBlock[u4Index]=u4FactorV*u4FactorH;
		}
		
		prJpgData->u4PicWidth	= prSOF->u2ImageWidth;
		prJpgData->fgGlobSet	|= D_GLOB_PIC_W;
		prJpgData->u4PicHeight	= prSOF->u2ImageHeight;
		prJpgData->fgGlobSet	|= D_GLOB_PIC_H;

		prDecParam->u4McuWidth = (prDecParam->u4MaxHFactor==2) ?
	          ((prJpgData->u4PicWidth+15)>>4):((prJpgData->u4PicWidth+7)>>3);
		prDecParam->u4McuHeight= (prDecParam->u4MaxVFactor==2) ?
		      ((prJpgData->u4PicHeight+15)>>4) :((prJpgData->u4PicHeight+7)>>3);

		for (u4Index = 0;u4Index < u4Num; u4Index++)
		{
		   UINT32 u4FactorH,u4FactorV,u4CompSize;
		   u4FactorH = prSOF->arSofComp[u4Index].u1HSampFactor;
		   u4FactorV = prSOF->arSofComp[u4Index].u1VSampFactor;
		   u4CompSize=u4FactorH*u4FactorV*64;
		   prDecParam->u4RowSize[u4Index]=u4CompSize*prDecParam->u4McuWidth;
		   if(((prDecParam->u4McuWidth&1)!=0)&&(u4FactorH==1))
		   {
                        prDecParam->u4RowSize[u4Index]+=u4CompSize;
		   }
		}

		/* set huff table */
		if(prJpgHeader->rDHT.u4NumDcTbl == 0)
		{
		   x_memcpy((void *)prJpgHeader->rDHT.arDcTbl[0].au1Bits, 
				(void *)_JpgBitsDcLuminance, (UINT32)sizeof(_JpgBitsDcLuminance));
		   x_memcpy((void *)prJpgHeader->rDHT.arDcTbl[0].au1HuffVal, 
				(void *)_JpgValDcLuminance, (UINT32)sizeof(_JpgValDcLuminance));
		   x_memcpy((void *)prJpgHeader->rDHT.arDcTbl[1].au1Bits, 
				(void *)_JpgBitsDcChrominance, (UINT32)sizeof(_JpgBitsDcChrominance));
		   x_memcpy((void *)prJpgHeader->rDHT.arDcTbl[1].au1HuffVal, 
				(void *)_JpgValDcChrominance, (UINT32)sizeof(_JpgValDcChrominance));
		   x_memcpy((void *)prJpgHeader->rDHT.arAcTbl[0].au1Bits, 
				(void *)_JpgBitsAcLuminance, (UINT32)sizeof(_JpgBitsAcLuminance));
		   x_memcpy((void *)prJpgHeader->rDHT.arAcTbl[0].au1HuffVal, 
				(void *)_JpgValAcLuminance, (UINT32)sizeof(_JpgValAcLuminance));
		   x_memcpy((void *)prJpgHeader->rDHT.arAcTbl[1].au1Bits, 
				(void *)_JpgBitsAcChrominance, (UINT32)sizeof(_JpgBitsAcChrominance));
		   x_memcpy((void *)prJpgHeader->rDHT.arAcTbl[1].au1HuffVal, 
				(void *)_JpgValAcChrominance, (UINT32)sizeof(_JpgValAcChrominance));
		   prJpgHeader->rDHT.fgDcTblLoaded = (UINT32)0x3;
		   prJpgHeader->rDHT.fgAcTblLoaded = (UINT32)0x3;
		}
		
		{
			JPEG_JFIF_DHT_T *prDHT=&prJpgHeader->rDHT;
			JPEG_DHT_HUFF_TBL_T *prDHTable;
			for(u4Index = 0; u4Index < D_MAX_JPEG_HUFF_TBL>>1; u4Index++)
			{
				if((prDHT->fgDcTblLoaded & D_BIT(u4Index))!=0)
				{
				   prDHTable=&prDHT->arDcTbl[u4Index];
				   i4Ret = VLDJPG_CalcDerivedHuffTable(prDHTable->au1Bits,prDHTable->au1HuffVal,&prJpgData->arDcHuffTable[u4Index]);
				   if((INT32)E_HWJPG_OK != i4Ret)
				   {
					   return -3;
				   }
				   VLDJPG_TransHuffTblToHwFormat((UINT8 *)VIRTUAL((UINT32)prJpgData->pu1HwDcHuffTbl+(u4Index*256)),prDHTable->au1HuffVal);
				}
				if((prDHT->fgAcTblLoaded & D_BIT(u4Index))!=0)
				{
					prDHTable=&prDHT->arAcTbl[u4Index];
					i4Ret = VLDJPG_CalcDerivedHuffTable(prDHTable->au1Bits,prDHTable->au1HuffVal,&prJpgData->arAcHuffTable[u4Index]);
					if((INT32)E_HWJPG_OK != i4Ret)
					{
						return -4;
					}
					prJpgData->fgAcHuff |= D_BIT(u4Index);
					VLDJPG_TransHuffTblToHwFormat((UINT8 *)VIRTUAL((UINT32)prJpgData->pu1HwAcHuffTbl + (u4Index * 256)),prDHTable->au1HuffVal);
				}
			}
			
			prJpgData->fgGlobSet |= D_GLOB_DCHUFF;
			prJpgData->fgScanSet |= D_SCAN_DCHUFF;
			prJpgData->fgGlobSet |= D_GLOB_ACHUFF;
			prJpgData->fgScanSet |= D_SCAN_ACHUFF;
		}
		
		/* set q table */
		for (u4Index = 0; u4Index < D_MAX_JPEG_QTBL; u4Index++)
		{
			prJpgData->apu1Qtbl[u4Index]= &prJpgHeader->rDQT.aau1Qtbl[u4Index][0];
		}
		prJpgData->fgGlobSet |= D_GLOB_QTBL;
		prJpgData->fgScanSet |= D_SCAN_QTBL;
		
		prJpgData->u4RestartInterval = prJpgHeader->u2RestartInterval;
		prJpgData->u4CurrRestartInterval = prJpgHeader->u2RestartInterval;
		prJpgData->fgGlobSet	 |= D_GLOB_RESTART;
		
		i4Ret=VLDJPG_ValidateGlobData((JPEGHANDLE)prJpgData);
		if(i4Ret!=(INT32)E_HWJPG_OK)
		{
		  return -5;
		}                       
#ifdef MJPEG_VIDEOMODE_ENABLE                                         	     
    	       if((((prSOF->arSofComp[0].u1HSampFactor == 2) && (prSOF->arSofComp[1].u1HSampFactor == 1) ) &&
                    ((prSOF->arSofComp[0].u1VSampFactor == 2 && (prSOF->arSofComp[1].u1VSampFactor == 1)) //420 format
                    ||(prSOF->arSofComp[0].u1VSampFactor == 1&& prSOF->arSofComp[1].u1VSampFactor == 1))))//422 format)
                {                   
                   JPG_SetDecMode5391((INT32)E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT);
                   VLDJPG_SetDecMode5391(E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT);
                }
               else
#endif
#ifdef MJPEG_MCU_ROW_ENABLE
                if(prSOF->arSofComp[0].u1HSampFactor == 1 && prSOF->arSofComp[1].u1HSampFactor == 1 && prSOF->arSofComp[0].u1VSampFactor == 1 && prSOF->arSofComp[1].u1VSampFactor == 1)
                {
                    JPG_SetDecMode5391((INT32)E_JPG_DEC_MODE_BASELINE_MCU_ROW);
                    VLDJPG_SetDecMode5391(E_JPG_DEC_MODE_BASELINE_MCU_ROW);             
                }
                else
#endif
                {
                    JPG_SetDecMode5391((INT32)E_JPG_DEC_MODE_BASELINE_PIC_RZ);
                    VLDJPG_SetDecMode5391(E_JPG_DEC_MODE_BASELINE_PIC_RZ);
                }
                            
	}

	prJpgData->fgRingBuffer	= TRUE;
	prJpgData->u4RingBufferSize	= prJpgHeader->u4RingBufSize;
	prJpgData->pu1RingBufStart = (prJpgHeader->pu1RingBufStart);
	prJpgData->fgLastScan =	TRUE;
        
	// scan
	/* set data start */
	{

		JPEG_JFIF_SOS_T *prSOS=&prJpgHeader->arSOS[0];
		UINT32 u4Block=0;
		prJpgData->u4McuCount = 0;
		prJpgData->pu1DataStart = prSOS->pu1ScanDataStart;
		prJpgData->fgScanSet   |= D_SCAN_DATASTART;
		/* set comp in scan */
		VLDJPG_SetScanCompInScan((JPEGHANDLE)prJpgData,prSOS->u1CompInScan,(UINT8*)prSOS->au1CompId);
		/* associate q tbl to comp, dc/ac id to block */
		u4Num=prJpgHeader->arSOS[0].u1CompInScan;
		if(u4Num>3)
		{
		   return -6;
		}
		
		for(u4Index = 0;u4Index<u4Num;u4Index++)
		{
		   UINT32 u4Count=0;
		   prJpgData->au1CompQ[u4Index] = prJpgHeader->rSOF.arSofComp[u4Index].u1QuantTblNo;
		   for (u4Count = 0; u4Count < prDecParam->au4CompBlock[u4Index];u4Count++,u4Block++)
		   {
			 prJpgData->au1BlkDc[u4Block]	= prSOS->au1DcId[u4Index];
			 prJpgData->au1BlkAc[u4Block]	= prSOS->au1AcId[u4Index];
		   }
		}
		
		prJpgData->fgScanSet |= D_SCAN_COMPQ;
		prJpgData->fgScanSet |= D_SCAN_BLKDC;
		prJpgData->fgScanSet |= D_SCAN_BLKAC;
		
		/* set al ah se ss*/
		prJpgData->u1AlAh=prSOS->u1AhAl;
		prJpgData->fgScanSet|= D_SCAN_ALAH;
		prJpgData->u1Se	  = prSOS->u1Se;
		prJpgData->u1Ss	  = prSOS->u1Ss;
		prJpgData->fgScanSet |= D_SCAN_SESS;
#ifdef JPEG_VLD_V3		
		VLDJPG_UseNewEngine();
                VLDJPG_SetErrorConcealment();        
                VLDJPG_SetErrorDetection();             
#endif
	    VLDJPG_SetProgressiveMode(D_JPEG_BASELINE);
	    prJpgData->u4ProgMode=D_JPEG_BASELINE;


	    /* set last scan flag */
		prJpgData->fgLastScan = (prJpgHeader->au4CompLastScan[0]==0)  \
		                        ||(prJpgHeader->au4CompLastScan[1]==0) \
		                        ||(prJpgHeader->au4CompLastScan[2]==0);
		prJpgData->fgScanSet  |= D_SCAN_LAST;

	    /* mtk patent */
	    prJpgData->u4McuRowStart= 0;
		prJpgData->fgScanSet|= D_SCAN_ROWSTART;
		prJpgData->u4McuRowEnd	= prDecParam->u4McuHeight-1;
		prJpgData->fgScanSet    |= D_SCAN_ROWEND;
		i4Ret=VLDJPG_ValidateScanData((JPEGHANDLE)prJpgData);
		if(i4Ret!=(INT32)E_HWJPG_OK)
		{
                    return -7;
		}
	    prJpgData->i4SavedEOBRUN = 0;
	}

	// Misc
	{
		UINT32 u4AddrX,u4AddrY;
		u4Num=0;
		for(u4Index=0;u4Index<D_MAX_JPEG_HW_COMP;u4Index++)
		{
                       for(u4AddrY=0;u4AddrY<prJpgData->au1CompBlkHeight[u4Index];u4AddrY++)
                       {
                          for(u4AddrX=0;u4AddrX<prJpgData->au1CompBlkWidth[u4Index];u4AddrX++)
                          {
                              if(u4Num>=D_MAX_JPEG_HW_BLOCK)
                              {
                                u4Num=D_MAX_JPEG_HW_BLOCK-1;
                              }
        		      prJpgData->au4BlockAddrX[u4Num] = u4AddrX;
        		      prJpgData->au4BlockAddrY[u4Num]	= u4AddrY;
                              u4Num++;
                          }
                       }
		}
		
		prJpgData->fgMiscSet |= D_MISC_ADDR_X;
		prJpgData->fgMiscSet |= D_MISC_ADDR_Y;

		i4Ret=VLDJPG_ValidateMiscData((JPEGHANDLE)prJpgData);
		if(i4Ret!=(INT32)E_HWJPG_OK)
		{
                     return -8;
		}
	    VLDJPG_Switch2CleanBuffer();
	}

	// buffer infor
	prJpgData->u4CurrMcuRow	= 0;
	prJpgData->fgRowSet	|= D_ROW_CURR;
#ifdef MJPEG_MCU_ROW_ENABLE
        if(VLDJPG_GetDecMode5391() == E_JPG_DEC_MODE_BASELINE_MCU_ROW)
        {
            UINT8 i;        
            LOG(2, "MJPEG real color buffer\n");
            for(i = 0; i < 3; i++)
            {
                LOG(2, "%d 0x%x ", i, prDecParam->u4ColorBuf[i]);
                if(i == 3)
                {
                    LOG(3, "\n");
                }
            }   
    	    prJpgData->apu1OutputBuffer[0]	= (UINT8 *)prDecParam->u4ColorBuf[0];
            prJpgData->apu1OutputBuffer[1]	= (UINT8 *)prDecParam->u4ColorBuf[1];
            prJpgData->apu1OutputBuffer[2]	= (UINT8 *)prDecParam->u4ColorBuf[2];

            prJpgData->au4OutputBufferSize[0] = prDecParam->u4RowSize[0];
            prJpgData->au4OutputBufferSize[1] = prDecParam->u4RowSize[1];
            prJpgData->au4OutputBufferSize[2] = prDecParam->u4RowSize[2];
            LOG(4, "Rowsize: Y 0x%x, Cb:0x%x\n", prJpgData->au4OutputBufferSize[0],prJpgData->au4OutputBufferSize[1]);
           
           
#ifdef JPGHW_TIME_PROFILE
           HAL_GetTime(&_rHwDecStart);
#endif
            for(;prJpgData->u4CurrMcuRow < prDecParam->u4McuHeight; prJpgData->u4CurrMcuRow ++)
            {
                VLDJPG_SetIsrSem(_hMJPG_IsrSeam);
                LOG(3, "current row:%d, colorbuffer:0x%x 0x%x 0x%x\n", prJpgData->u4CurrMcuRow, prJpgData->apu1OutputBuffer[0], prJpgData->apu1OutputBuffer[1], prJpgData->apu1OutputBuffer[2]);
                VLDJPG_SetMcuRowCurr((JPEGHANDLE)prJpgData, prJpgData->u4CurrMcuRow); 
                {                
                    if(prJpgData->u4CurrMcuRow % 2 == 1)
                    {
                        prJpgData->apu1OutputBuffer1[0] = (UINT8 *)(prDecParam->u4ColorBuf[0]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[0]); 
                        prJpgData->apu1OutputBuffer1[1] = (UINT8 *)(prDecParam->u4ColorBuf[1]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[1]); 
                        prJpgData->apu1OutputBuffer1[2] = (UINT8 *)(prDecParam->u4ColorBuf[2]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[2]); 
                        VLDJPG_SetMcuRowOutBufBank15391((JPEGHANDLE)prJpgData, 0, prJpgData->apu1OutputBuffer1[0], prDecParam->u4RowSize[0]) ;
                        VLDJPG_SetMcuRowOutBufBank15391((JPEGHANDLE)prJpgData, 1, prJpgData->apu1OutputBuffer1[1], prDecParam->u4RowSize[1]) ;
                        VLDJPG_SetMcuRowOutBufBank15391((JPEGHANDLE)prJpgData, 2, prJpgData->apu1OutputBuffer1[2], prDecParam->u4RowSize[2]) ;
                        LOG(2, "row:%d\nBank1 y:0x%x  cb:0x%x  cr:0x%x\nBank0 y:0x%x cb:0x%x cr:0x%x\n", prJpgData->u4CurrMcuRow, prJpgData->apu1OutputBuffer1[0], prJpgData->apu1OutputBuffer1[1],prJpgData->apu1OutputBuffer1[2],prJpgData->apu1OutputBuffer[0], prJpgData->apu1OutputBuffer[1], prJpgData->apu1OutputBuffer[2]);                                                                                              
                   }
                    else
                    {                            
                        prJpgData->apu1OutputBuffer[0] = (UINT8 *)(prDecParam->u4ColorBuf[0]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[0]); 
                        prJpgData->apu1OutputBuffer[1] = (UINT8 *)(prDecParam->u4ColorBuf[1]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[1]); 
                        prJpgData->apu1OutputBuffer[2] = (UINT8 *)(prDecParam->u4ColorBuf[2]  + prJpgData->u4CurrMcuRow * prDecParam->u4RowSize[2]); 
                        VLDJPG_SetMcuRowOutBuf((JPEGHANDLE)prJpgData, 0, prJpgData->apu1OutputBuffer[0], prDecParam->u4RowSize[0]);
                        VLDJPG_SetMcuRowOutBuf((JPEGHANDLE)prJpgData, 1, prJpgData->apu1OutputBuffer[1], prDecParam->u4RowSize[1]);
                        VLDJPG_SetMcuRowOutBuf((JPEGHANDLE)prJpgData, 2, prJpgData->apu1OutputBuffer[2], prDecParam->u4RowSize[2]);     
                        LOG(2, "row:%d\nBank0 y:0x%x  cb:0x%x  cr:0x%x\nBank1 y:0x%x cb:0x%x cr:0x%x\n", prJpgData->u4CurrMcuRow, prJpgData->apu1OutputBuffer[0], prJpgData->apu1OutputBuffer[1],prJpgData->apu1OutputBuffer[2],prJpgData->apu1OutputBuffer1[0], prJpgData->apu1OutputBuffer1[1], prJpgData->apu1OutputBuffer1[2]);                                       
                    }
                    
                }                   
                VLDJPG_ValidateMcuRowData5391((JPEGHANDLE)prJpgData,prDecParam->u4McuWidth,prDecParam->u4McuHeight);
                              
                 i4Ret = VLDJPG_DecodeOneMcuRow5391((JPEGHANDLE)prJpgData, prJpgData->u4CurrMcuRow);
              
                 if(prJpgHeader->fgEOF)
                 {
                      LOG(2, "@@@ _hIsrSeam lock\n");
                      VERIFY (x_sema_lock(_hMJPG_IsrSeam, X_SEMA_OPTION_WAIT) == OSR_OK);
                     if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
                     {
                       i4Ret = -7;
                     }
                 }
                 else
                 {
                     while(VLDJPG_CheckDecodePicFinish((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
                     {            
                        if(!prJpgHeader->fgEOF)
                        {
                           _MJPG_UpdateReadPtr(prJpgHeader);
                           if(prJpgHeader->u4RingData < (UINT32)(200 * 1024))
                           {
                              i4Ret=_MJPG_RequestData(prDecoder);
                              if(i4Ret!=0)
                              {
                                 Printf("Request data error %d\n",i4Ret);
                                 i4Ret = -4;
                              }
                           }
                           
                         }
                         
                         if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
                         {
                             i4Ret = -6;
                             break;
                         }
                     }
                 }                                      
            }                           
#ifdef JPGHW_TIME_PROFILE
            HAL_GetTime(&_rHwDecEnd);
            HAL_GetDeltaTime(&_rHwDecDif, &_rHwDecStart, &_rHwDecEnd);
            LOG(1, "jdec hw time: %u.%06u\n", _rHwDecDif.u4Seconds, _rHwDecDif.u4Micros);
#endif
        }        
#endif
#ifdef MJPEG_VIDEOMODE_ENABLE
	if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_RZ)
#endif
        {   
        	prJpgData->apu1OutputBuffer[0]	= (UINT8 *)prDecParam->u4ColorBuf[0];
        	prJpgData->apu1OutputBuffer[1]	= (UINT8 *)prDecParam->u4ColorBuf[1];
        	prJpgData->apu1OutputBuffer[2]	= (UINT8 *)prDecParam->u4ColorBuf[2];
        	prJpgData->apu1OutputBuffer1[0] = (UINT8 *)prDecParam->u4ColorBuf[3];
        	prJpgData->apu1OutputBuffer1[1] = (UINT8 *)prDecParam->u4ColorBuf[4];
        	prJpgData->apu1OutputBuffer1[2] = (UINT8 *)prDecParam->u4ColorBuf[5];
        	prJpgData->au4OutputBufferSize[0] = prDecParam->u4RowSize[0];
        	prJpgData->au4OutputBufferSize[1] = prDecParam->u4RowSize[1];
        	prJpgData->au4OutputBufferSize[2] = prDecParam->u4RowSize[2];
        	prJpgData->fgRowSet	|= D_ROW_OUTBUF | D_ROW_OUTBUFSIZE;
       }
#ifdef MJPEG_VIDEOMODE_ENABLE
        else if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
        {
            prJpgData->apu1OutputBuffer[0] =  (UINT8 *)prDecoder->prInParam->u4DstAddr;
            prJpgData->apu1OutputBuffer[1] =  (UINT8 *)(prDecoder->prInParam->u4CbCrOffset + prDecoder->prInParam->u4DstAddr);
            prJpgData->apu1OutputBuffer[2] =  (UINT8 *)(prDecoder->prInParam->u4CbCrOffset + prDecoder->prInParam->u4DstAddr);     
            prJpgData->au4OutputBufferSize[0] = prDecParam->u4RowSize[0];
      	    prJpgData->au4OutputBufferSize[1] = prDecParam->u4RowSize[1];
            prJpgData->au4OutputBufferSize[2] = prDecParam->u4RowSize[2];
            prJpgData->fgRowSet	|= D_ROW_OUTBUF | D_ROW_OUTBUFSIZE;            
        }
#endif
	i4Ret=VLDJPG_ValidateMcuRowData5391((JPEGHANDLE)prJpgData,prDecParam->u4McuWidth,prDecParam->u4McuHeight);
	if(i4Ret!=(INT32)E_HWJPG_OK)
	{
	   return -9;
	}

	return 0;
}
#ifdef MJPEG_MCU_ROW_ENABLE
static VOID _MJPG_ROWResizeCfg(MJPEG_DECODER *prDecoder,RZ_JPG_SCL_PARAM_SET_T *prRzParam)
{
    MJPG_INPARAM_T *prInParam=prDecoder->prInParam;
    MJPG_OUTPARAM_T *prOutParam=prDecoder->prOutParam;
    JPEG_VLDJPG_DATA_T *prJpgData=&prDecoder->rJpgHw;
    MJPEG_DEC_PARAM *prDecParam=&prDecoder->rDecParam;
    //UINT32 u4Index=0;
    x_memset(prRzParam,0,sizeof(RZ_JPG_SCL_PARAM_SET_T));
    prRzParam->u4JpgVfacY=prJpgData->au1CompBlkHeight[0]-1;
    prRzParam->u4JpgVfacCb=prJpgData->au1CompBlkHeight[1]-1;
    prRzParam->u4JpgVfacCr=prJpgData->au1CompBlkHeight[2]-1;
    
    if(prJpgData->u1NumComps==1)
    {
       prRzParam->fgGray=1;
       prRzParam->u4JpgVfacCb=prRzParam->u4JpgVfacY;
       prRzParam->u4JpgVfacCr=prRzParam->u4JpgVfacY;
    }
    else
    {
            prRzParam->fgGray=0;
    }
    
    prRzParam->u4IsJpgSameaddr = 0;
    prRzParam->u4YTgHOffset = 0;
    prRzParam->u4YTgVOffset = 0;
    prRzParam->u4YTgBase = PHYSICAL(prInParam->u4DstAddr);
    
    prRzParam->u4IsVdo2Osd=0;
    prRzParam->u4YTgCM=0;

    if(prInParam->u4ForVdec)
    {
#if defined(CC_MT5395) || defined(JPEG_IS_POST_MT5368)
       prRzParam->u4IsRsOut=1;
       prRzParam->u4OutSwap=0;
#else
       prRzParam->u4IsRsOut=0;
       prRzParam->u4OutSwap=6;
#endif
       if(prInParam->u4Pitch==0)
       {
           prRzParam->u4YTgBufLen=prJpgData->au1CompBlkWidth[0]*prDecParam->u4McuWidth*8;
           prRzParam->u4YTgBufLen=((prRzParam->u4YTgBufLen+15)/16)*16;
       }
       else
       {
           prRzParam->u4YTgBufLen=prInParam->u4Pitch;
       }        
       prRzParam->u4CTgBufLen=prRzParam->u4YTgBufLen;
       prRzParam->u4CTgBase=prRzParam->u4YTgBase+prInParam->u4CbCrOffset;
    }
    else
    {  
       prRzParam->u4IsRsOut=1;  //static jpeg is raster out
       prRzParam->u4OutSwap=0;
       prRzParam->u4YTgBufLen=prJpgData->au1CompBlkWidth[0]*prDecParam->u4McuWidth*8;
       prRzParam->u4YTgBufLen=((prRzParam->u4YTgBufLen+15)/16)*16;
       prRzParam->u4CTgBase=prRzParam->u4YTgBase+prRzParam->u4YTgBufLen*((prJpgData->u4PicHeight+15)/16)*16;
    }

    prRzParam->u4IsRsIn = 1;        // block|raster based input
    prRzParam->u4YSrcBase1 =  prDecParam->u4ColorBuf[0]; // y1
    prRzParam->u4YSrcBase2 =  prRzParam->u4YSrcBase1; //prDecParam->u4ColorBuf[3]; // y2
    prRzParam->u4YSrcBufLen = prJpgData->u4CompPitchRow[0];
    prRzParam->u4CSrcBufLen = (prJpgData->au1CompBlkWidth[0] == prJpgData->au1CompBlkWidth[1])? prRzParam->u4YSrcBufLen:(prRzParam->u4YSrcBufLen / 2);
    prRzParam->u4YSrcHOffset = 0;
    prRzParam->u4YSrcVOffset = 0;
    prRzParam->u4YSrcW=prJpgData->u4PicWidth;
    prRzParam->u4YSrcH = (prRzParam->u4JpgVfacY == prRzParam->u4JpgVfacCb) ? prJpgData->u4PicHeight :(prJpgData->u4PicHeight&0xfffffffe);

    prRzParam->u4CbSrcBase1 = prDecParam->u4ColorBuf[1];    // cb1
    prRzParam->u4CbSrcBase2 = prRzParam->u4CbSrcBase1 ;//prDecParam->u4ColorBuf[4];    // cb2
    prRzParam->u4CbSrcHOffset = 0;
    prRzParam->u4CbSrcVOffset = 0;
    prRzParam->u4CbSrcW = (prJpgData->au1CompBlkWidth[0] == prJpgData->au1CompBlkWidth[1])? prRzParam->u4YSrcW:(prRzParam->u4YSrcW / 2);
    prRzParam->u4CbSrcH = (prJpgData->au1CompBlkHeight[0]== prJpgData->au1CompBlkHeight[1])? prRzParam->u4YSrcH :(prRzParam->u4YSrcH / 2);

    prRzParam->u4CrSrcBase1 = prDecParam->u4ColorBuf[2];    // cr1
    prRzParam->u4CrSrcBase2 = prRzParam->u4CrSrcBase1;//prDecParam->u4ColorBuf[5];    // cr2
    prRzParam->u4CrSrcHOffset = 0;
    prRzParam->u4CrSrcVOffset = 0;
    prRzParam->u4CrSrcW = (prJpgData->au1CompBlkWidth[0] == prJpgData->au1CompBlkWidth[2])?  prRzParam->u4YSrcW :(prRzParam->u4YSrcW / 2);
    prRzParam->u4CrSrcH = (prJpgData->au1CompBlkHeight[0]== prJpgData->au1CompBlkHeight[2])? prRzParam->u4YSrcH: (prRzParam->u4YSrcH / 2);
    prRzParam->fgPreload = 0;

    if(prRzParam->fgGray)     
    {
        prRzParam->u4CbSrcW = prRzParam->u4CrSrcW = prRzParam->u4YSrcW;
        prRzParam->u4CrSrcH = prRzParam->u4CbSrcH = prRzParam->u4YSrcH;
    }
    
    prRzParam->u4ColorComp = 0x7;

    if(prInParam->u4ForVdec)
    {
        prRzParam->u4YTgW= prInParam->u4Width==0 ?  prRzParam->u4YSrcW : prInParam->u4Width;
        prRzParam->u4YTgH= prInParam->u4Height==0 ? prRzParam->u4YSrcH : prInParam->u4Height;
        prRzParam->u4OutMode = E_RZ_VDO_OUTMD_420;
        prRzParam->u4CTgH = prRzParam->u4YTgH >> 1;
    }
    else
    {
        prRzParam->u4YTgW=prRzParam->u4YSrcW;
        prRzParam->u4YTgH = prRzParam->u4YSrcH;
        prRzParam->u4OutMode = E_RZ_VDO_OUTMD_422;
        prRzParam->u4CTgH = prRzParam->u4YTgH;
    }
    
    prRzParam->u4CTgW = prRzParam->u4YTgW >> 1;
    prRzParam->u4IsFstBl = 1;
    prRzParam->u4IsLstBl =1;//prDecParam->u4McuHeight==1 ? 1 : 0;
    prRzParam->u4IsJpgRzOn = 0; 
    prRzParam->fgBlassign = 1;
    prRzParam->u4JpgTempAddr = prDecParam->u4RzWorkBuf;
    prOutParam->pu1Addr=(UINT8 *)prRzParam->u4YTgBase;
    prOutParam->u4CbCrOffset=prRzParam->u4CTgBase-prRzParam->u4YTgBase;
    prOutParam->u4Height=prRzParam->u4YTgH;
    prOutParam->u4Width=prRzParam->u4YTgW;
    prOutParam->u4Pitch=prRzParam->u4YTgBufLen;
    return ;   
}
#endif

static VOID _MJPG_ResizeCfg(MJPEG_DECODER *prDecoder,RZ_JPG_SCL_PARAM_SET_T *prRzParam)
{
        MJPG_INPARAM_T *prInParam=prDecoder->prInParam;
	MJPG_OUTPARAM_T *prOutParam=prDecoder->prOutParam;
	JPEG_VLDJPG_DATA_T *prJpgData=&prDecoder->rJpgHw;
	MJPEG_DEC_PARAM *prDecParam=&prDecoder->rDecParam;
	//UINT32 u4Index=0;
	x_memset(prRzParam,0,sizeof(RZ_JPG_SCL_PARAM_SET_T));
	prRzParam->u4JpgVfacY=prJpgData->au1CompBlkHeight[0]-1;
	prRzParam->u4JpgVfacCb=prJpgData->au1CompBlkHeight[1]-1;
	prRzParam->u4JpgVfacCr=prJpgData->au1CompBlkHeight[2]-1;
	
	if(prJpgData->u1NumComps==1)
	{
	   prRzParam->fgGray=1;
	   prRzParam->u4JpgVfacCb=prRzParam->u4JpgVfacY;
	   prRzParam->u4JpgVfacCr=prRzParam->u4JpgVfacY;
	}
	else
	{
            prRzParam->fgGray=0;
	}
	
	prRzParam->u4IsJpgSameaddr = 0;
	prRzParam->u4YTgHOffset = 0;
	prRzParam->u4YTgVOffset = 0;
	prRzParam->u4YTgBase = PHYSICAL(prInParam->u4DstAddr);
    
        prRzParam->u4IsVdo2Osd=0;
	prRzParam->u4YTgCM=0;

	if(prInParam->u4ForVdec)
	{
#if defined(CC_MT5395) || defined(JPEG_IS_POST_MT5368)
	   prRzParam->u4IsRsOut=1;
       prRzParam->u4OutSwap=0;
#else
	   prRzParam->u4IsRsOut=0;
       prRzParam->u4OutSwap=6;
#endif
	   if(prInParam->u4Pitch==0)
	   {
		   prRzParam->u4YTgBufLen=prJpgData->au1CompBlkWidth[0]*prDecParam->u4McuWidth*8;
		   prRzParam->u4YTgBufLen=((prRzParam->u4YTgBufLen+15)/16)*16;
	   }
	   else
	   {
		   prRzParam->u4YTgBufLen=prInParam->u4Pitch;
	   }
	   prRzParam->u4CTgBufLen=prRzParam->u4YTgBufLen;
	   prRzParam->u4CTgBase=prRzParam->u4YTgBase+prInParam->u4CbCrOffset;
	}
    else
    {  
	   prRzParam->u4IsRsOut=1;  //static jpeg is raster out
	   prRzParam->u4OutSwap=0;
	   prRzParam->u4YTgBufLen=prJpgData->au1CompBlkWidth[0]*prDecParam->u4McuWidth*8;
	   prRzParam->u4YTgBufLen=((prRzParam->u4YTgBufLen+15)/16)*16;
	   prRzParam->u4CTgBase=prRzParam->u4YTgBase+prRzParam->u4YTgBufLen*((prJpgData->u4PicHeight+15)/16)*16;
    }

    prRzParam->u4IsRsIn = 1;        // block|raster based input
    prRzParam->u4YSrcBase1 =  prDecParam->u4ColorBuf[0]; // y1
    prRzParam->u4YSrcBase2 =  prDecParam->u4ColorBuf[3]; // y2
    prRzParam->u4YSrcBufLen = prJpgData->u4CompPitchRow[0];
	prRzParam->u4CSrcBufLen=prRzParam->u4YSrcBufLen;
    prRzParam->u4YSrcHOffset = 0;
    prRzParam->u4YSrcVOffset = 0;
    prRzParam->u4YSrcW=prJpgData->u4PicWidth;
    prRzParam->u4YSrcH = (prRzParam->u4JpgVfacY == prRzParam->u4JpgVfacCb) ? prJpgData->u4PicHeight :(prJpgData->u4PicHeight&0xfffffffe);

	prRzParam->u4CbSrcBase1 = prDecParam->u4ColorBuf[1];    // cb1
    prRzParam->u4CbSrcBase2 = prDecParam->u4ColorBuf[4];    // cb2
    prRzParam->u4CbSrcHOffset = 0;
    prRzParam->u4CbSrcVOffset = 0;
    prRzParam->u4CbSrcW = (prJpgData->au1CompBlkWidth[0] == prJpgData->au1CompBlkWidth[1])? prRzParam->u4YSrcW:(prRzParam->u4YSrcW / 2);
    prRzParam->u4CbSrcH = (prJpgData->au1CompBlkHeight[0]== prJpgData->au1CompBlkHeight[1])? prRzParam->u4YSrcH :(prRzParam->u4YSrcH / 2);

    prRzParam->u4CrSrcBase1 = prDecParam->u4ColorBuf[2];    // cr1
    prRzParam->u4CrSrcBase2 = prDecParam->u4ColorBuf[5];    // cr2
    prRzParam->u4CrSrcHOffset = 0;
    prRzParam->u4CrSrcVOffset = 0;
    prRzParam->u4CrSrcW = (prJpgData->au1CompBlkWidth[0] == prJpgData->au1CompBlkWidth[2])?  prRzParam->u4YSrcW :(prRzParam->u4YSrcW / 2);
    prRzParam->u4CrSrcH = (prJpgData->au1CompBlkHeight[0]== prJpgData->au1CompBlkHeight[2])? prRzParam->u4YSrcH: (prRzParam->u4YSrcH / 2);
    prRzParam->fgPreload = 0;

	if(prRzParam->fgGray)     
    {
        prRzParam->u4CbSrcW = prRzParam->u4CrSrcW = prRzParam->u4YSrcW;
        prRzParam->u4CrSrcH = prRzParam->u4CbSrcH = prRzParam->u4YSrcH;
    }
	
    prRzParam->u4ColorComp = 0x7;

	if(prInParam->u4ForVdec)
	{
	    prRzParam->u4YTgW= prInParam->u4Width==0 ?  prRzParam->u4YSrcW : prInParam->u4Width;
	    prRzParam->u4YTgH= prInParam->u4Height==0 ? prRzParam->u4YSrcH : prInParam->u4Height;
		prRzParam->u4OutMode = E_RZ_VDO_OUTMD_420;
		prRzParam->u4CTgH = prRzParam->u4YTgH >> 1;
	}
	else
	{
        prRzParam->u4YTgW=prRzParam->u4YSrcW;
        prRzParam->u4YTgH = prRzParam->u4YSrcH;
		prRzParam->u4OutMode = E_RZ_VDO_OUTMD_422;
		prRzParam->u4CTgH = prRzParam->u4YTgH;
	}
	
	prRzParam->u4CTgW = prRzParam->u4YTgW >> 1;
    prRzParam->u4IsFstBl = 1;
    prRzParam->u4IsLstBl =prDecParam->u4McuHeight==1 ? 1 : 0;
	prRzParam->u4IsJpgRzOn = 1; 
    prRzParam->u4JpgTempAddr = prDecParam->u4RzWorkBuf;
	prOutParam->pu1Addr=(UINT8 *)prRzParam->u4YTgBase;
	prOutParam->u4CbCrOffset=prRzParam->u4CTgBase-prRzParam->u4YTgBase;
	prOutParam->u4Height=prRzParam->u4YTgH;
	prOutParam->u4Width=prRzParam->u4YTgW;
	prOutParam->u4Pitch=prRzParam->u4YTgBufLen;
    return ;
}
#ifdef MJPEG_VIDEOMODE_ENABLE
static INT32 _MJPEG_VideoModeDecodeFrame(MJPEG_DECODER *prDecoder)
{
    INT32 i4Ret=0;
    UINT32 u4SleepCnt = 0, u4NonSleepCnt = 0;
    
    JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
    MJPEG_DEC_PARAM *prDecParam=&prDecoder->rDecParam;    
    MJPG_OUTPARAM_T *prOutParam=prDecoder->prOutParam;
    JPEG_VLDJPG_DATA_T *prJpgData=&prDecoder->rJpgHw;  
    MJPG_INPARAM_T *prInParam=prDecoder->prInParam;

    prOutParam->u4Width= (prInParam->u4Height == 0) ? prJpgData->u4PicWidth : prInParam->u4Width;
    prOutParam->u4Height = (prJpgData->au1CompBlkHeight[0]-1 == prJpgData->au1CompBlkHeight[1]-1)?prJpgData->u4PicHeight :(prJpgData->u4PicHeight&0xfffffffe);
    if(prInParam->u4Pitch == 0)
    {
        prOutParam->u4Pitch=(prJpgData->au1CompBlkWidth[0]*prDecParam->u4McuWidth*8 + 0xf) & (~0xf);
        prOutParam->u4Pitch=((prOutParam->u4Pitch+15)/16)*16;
    }
    else
    {
        prOutParam->u4Pitch = prInParam->u4Pitch;
    }
    
    VLDJPG_SetIsrSem(_hMJPG_IsrSeam);
    
    #ifdef JPGHW_TIME_PROFILE
    HAL_GetTime(&_rDecStart);
    #endif
    
    VLDJPG_DecodeOnePic((JPEGHANDLE)&prDecoder->rJpgHw);
     if(prJpegHdr->fgEOF)
    {
    	  LOG(5, "@@@ _hIsrSeam lock\n");
          VERIFY (x_sema_lock(_hMJPG_IsrSeam, X_SEMA_OPTION_WAIT) == OSR_OK);
	  if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
	  {
		i4Ret = -7;
	  }
    }

    else
    {
            while (VLDJPG_CheckDecodePicFinish((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
            {                    
                u4NonSleepCnt++;
                if (!prJpegHdr->fgEOF)
                {
                    _MJPG_UpdateReadPtr(prJpegHdr);
                    if (prJpegHdr->u4RingData < (UINT32)(200 * 1024))
                    {           
                        i4Ret=_MJPG_RequestData(prDecoder);
                        if(i4Ret!=0)
        		 {
        			Printf("Request data error %d\n",i4Ret);
        			i4Ret = -4;
        		 }                 
                        u4SleepCnt++;  
                        u4NonSleepCnt = 0;
                    }
                }     
                if (VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
                {
                    i4Ret = -6;
                    LOG(3, "%s, line %d decoding error\n", __FUNCTION__, __LINE__);
                    break;
                }
                if (u4NonSleepCnt >= (UINT32)0x80000)
                {
                    x_thread_delay(1);
                    u4SleepCnt++;  
                    u4NonSleepCnt = 0;
                    if (VLDJPG_CheckStop((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
                    {
                        i4Ret = -(INT32)E_JPG_ERR_FATAL;
                        LOG(3, "already stop, timeout 1\n");
                        break;
                    }
                }
                if (u4SleepCnt >= (UINT32)30000)
                {
                    i4Ret = -(INT32)E_JPG_ERR_FATAL;
                    LOG(3, "JPGHDR->u4RingData:%d, time out 2\n", prJpegHdr->u4RingData);
                    break;
                }        
            }                    
    }  
    
#ifdef JPGHW_TIME_PROFILE
    HAL_GetTime(&_rDecEnd);            
    HAL_GetDeltaTime(&_rDecdif, &_rDecStart, &_rDecEnd);
    _rDecAcc.u4Seconds += _rDecdif.u4Seconds;
    _rDecAcc.u4Micros  += _rDecdif.u4Micros;
    if (_rDecAcc.u4Micros >= 1000000)
    {
        _rDecAcc.u4Seconds += _rDecAcc.u4Micros / 1000000;
        _rDecAcc.u4Micros  %= 1000000;
    }
        LOG(0, "MJPEG video mode decoding %u.%06u\n",
       _rDecAcc.u4Seconds, _rDecAcc.u4Micros
       );    
#endif
    VLDJPG_SetIsrSem((HANDLE_T)NULL);
    return i4Ret;
}
#endif
#ifdef CC_SUPPORT_MTK_ANDROID_RM 
static INT32 _MJPG_DecodeFrame(MJPEG_DECODER *prDecoder, INT32 i4RzId)
{
	JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
        RZ_JPG_SCL_PARAM_SET_T rJpgParam;
	INT32 i4Ret=0,i4RzPauseTime=0;
	prDecoder->rJpgHw.rParent.eJpegClass=E_JC_VLD_DECODER;
	prDecoder->rJpgHw.rParent.zObjectSize=sizeof(JPEG_VLDJPG_DATA_T);
	VLDJPG_New((JPEGHANDLE)(&prDecoder->rJpgHw));

	i4Ret=_MJPG_RequestData(prDecoder);
        if(i4Ret!=0)
	{
	   Printf("Request First Data error %d\n",i4Ret);
	   return -1;
	}
	
	if(JFIF_New((JPEGHANDLE)(&prDecoder->rJpgHeader),(void *)prDecoder->rDecParam.u4DataAddr,
		prDecoder->prInParam->u4FrameSize)!=0)
	{
           Printf("JFif parse error \n");
	   return 0;
	}
	i4Ret=_MJPG_FetchRegValue(prDecoder);
	
	if(i4Ret!=0)
	{
           Printf("_JPG_FetchRegValue return %d\n",i4Ret);
	   return -3;
	}

#ifdef CC_FUJI3D_SUPPORT
	if(prDecoder->prInParam->fgCheckMPF&& prDecoder->rJpgHeader.rAPP2.fgAPP2 
		&& prDecoder->rJpgHeader.rAPP2.fgIsMPF && (prDecoder->rJpgHeader.rAPP2.u4NumOfImages == 1))
	{
		prDecoder->prOutParam->u4AuxFrameOffSet= (UINT32)prDecoder->rJpgHeader.rAPP2.apu1MPEntry[0];
		prDecoder->prInParam->u4Height=prDecoder->rJpgHw.u4PicHeight>>1;
	}
	else
	{
		prDecoder->prOutParam->u4AuxFrameOffSet=0;
	}
#endif

	prDecoder->prOutParam->ucPicStruct=prDecoder->rJpgHeader.ucPicStruct;
#ifdef MJPEG_MCU_ROW_ENABLE
        if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_MCU_ROW)
        {
            _MJPG_ROWResizeCfg(prDecoder,&rJpgParam);    
            IMGRZ_Lock_Ex(i4RzId);
            IMGRZ_ReInit_Ex(i4RzId);
            IMGRZ_SetScaleOpt_Ex(i4RzId, E_RZ_INOUT_TYPE_JPGMD);
            IMGRZ_Scale_Ex(i4RzId, (void *)(&rJpgParam));   
        #ifdef JPGHW_TIME_PROFILE	
            HAL_GetTime(&_rRZRowStart);
        #endif
            IMGRZ_Flush_Ex(i4RzId);
            IMGRZ_Wait_Ex(i4RzId);
            IMGRZ_Unlock_Ex(i4RzId);
        #ifdef JPGHW_TIME_PROFILE
            HAL_GetTime(&_rRZRowEnd);            
            HAL_GetDeltaTime(&_rRZRowDif, &_rRZRowStart, &_rRZRowEnd);
            LOG(1, "rz one pic need %u.%06u\n", _rRZRowDif.u4Seconds, _rRZRowDif.u4Micros);
        #endif
        }
#endif
#ifdef MJPEG_VIDEOMODE_ENABLE
        if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
        {
            i4Ret = _MJPEG_VideoModeDecodeFrame(prDecoder);
        }
        else if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_RZ)
#endif
        {
            _MJPG_ResizeCfg(prDecoder,&rJpgParam);        	
            IMGRZ_Lock();
            IMGRZ_ReInit();
            IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
            IMGRZ_Scale((void *)(&rJpgParam));   
        #ifdef JPGHW_TIME_PROFILE	
        	HAL_GetTime(&_rRZStart);
        #endif
            IMGRZ_Flush();
            VLDJPG_SetIsrSem(_hMJPG_IsrSeam);
            VLDJPG_DecodeOnePic((JPEGHANDLE)&prDecoder->rJpgHw);
            if(prJpegHdr->fgEOF)
            {
            	 LOG(5, "@@@ _hIsrSeam lock\n");
                 VERIFY (x_sema_lock(_hMJPG_IsrSeam, X_SEMA_OPTION_WAIT) == OSR_OK);
        	  if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        	  {
        		i4Ret = -7;
        	  }
            }
            else
            {
        		while(VLDJPG_CheckDecodePicFinish((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        		{			 
        		   if(!prJpegHdr->fgEOF)
        		   {
        			  _MJPG_UpdateReadPtr(prJpegHdr);
        			  if(prJpegHdr->u4RingData < (UINT32)(200 * 1024))
        			  {
        				 i4Ret=_MJPG_RequestData(prDecoder);
        				 if(i4Ret!=0)
        				 {
        					Printf("Request data error %d\n",i4Ret);
        					i4Ret = -4;
        				 }
        				 i4RzPauseTime=0;
        			  }
        			  else
        			  {
        				 x_thread_delay(1);
        				 if(IMGRZ_GetScaleLine()==0)
        				 {
        					i4RzPauseTime++;
        				 }
        				 if(i4RzPauseTime>3000)
        				 {
        					 i4Ret = -5;
        					 break;
        				 }
        			  }
        			}
        			
        			if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        			{
        				i4Ret = -6;
        				break;
        			}
        		}
            }
            
            VLDJPG_SetIsrSem((HANDLE_T)NULL);
            IMGRZ_Wait();
            IMGRZ_Unlock();
       }
    return i4Ret;
}
#else
static INT32 _MJPG_DecodeFrame(MJPEG_DECODER *prDecoder)
{
	JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
        RZ_JPG_SCL_PARAM_SET_T rJpgParam;
	INT32 i4Ret=0,i4RzPauseTime=0;
	prDecoder->rJpgHw.rParent.eJpegClass=E_JC_VLD_DECODER;
	prDecoder->rJpgHw.rParent.zObjectSize=sizeof(JPEG_VLDJPG_DATA_T);
	VLDJPG_New((JPEGHANDLE)(&prDecoder->rJpgHw));

	i4Ret=_MJPG_RequestData(prDecoder);
        if(i4Ret!=0)
	{
	   Printf("Request First Data error %d\n",i4Ret);
	   return -1;
	}
	
	if(JFIF_New((JPEGHANDLE)(&prDecoder->rJpgHeader),(void *)prDecoder->rDecParam.u4DataAddr,
		prDecoder->prInParam->u4FrameSize)!=0)
	{
           Printf("JFif parse error \n");
	   return 0;
	}
	i4Ret=_MJPG_FetchRegValue(prDecoder);
	
	if(i4Ret!=0)
	{
           Printf("_JPG_FetchRegValue return %d\n",i4Ret);
	   return -3;
	}

#ifdef CC_FUJI3D_SUPPORT
	if(prDecoder->prInParam->fgCheckMPF&& prDecoder->rJpgHeader.rAPP2.fgAPP2 
		&& prDecoder->rJpgHeader.rAPP2.fgIsMPF && (prDecoder->rJpgHeader.rAPP2.u4NumOfImages == 1))
	{
		prDecoder->prOutParam->u4AuxFrameOffSet= (UINT32)prDecoder->rJpgHeader.rAPP2.apu1MPEntry[0];
		prDecoder->prInParam->u4Height=prDecoder->rJpgHw.u4PicHeight>>1;
	}
	else
	{
		prDecoder->prOutParam->u4AuxFrameOffSet=0;
	}
#endif

	prDecoder->prOutParam->ucPicStruct=prDecoder->rJpgHeader.ucPicStruct;

#ifdef MJPEG_VIDEOMODE_ENABLE
        if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_VIDEO_OUTPUT)
        {
            i4Ret = _MJPEG_VideoModeDecodeFrame(prDecoder);
        }
        else 
#endif        
        if(VLDJPG_GetDecMode5391()  == E_JPG_DEC_MODE_BASELINE_PIC_RZ)
        {
            _MJPG_ResizeCfg(prDecoder,&rJpgParam);        	
            IMGRZ_Lock();
            IMGRZ_ReInit();
            IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
            IMGRZ_Scale((void *)(&rJpgParam));   
        #ifdef JPGHW_TIME_PROFILE	
        	HAL_GetTime(&_rRZStart);
        #endif
            IMGRZ_Flush();
            VLDJPG_SetIsrSem(_hMJPG_IsrSeam);
            VLDJPG_DecodeOnePic((JPEGHANDLE)&prDecoder->rJpgHw);
            if(prJpegHdr->fgEOF)
            {
            	 LOG(5, "@@@ _hIsrSeam lock\n");
                 VERIFY (x_sema_lock(_hMJPG_IsrSeam, X_SEMA_OPTION_WAIT) == OSR_OK);
        	  if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        	  {
        		i4Ret = -7;
        	  }
            }
            else
            {
        		while(VLDJPG_CheckDecodePicFinish((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        		{			 
        		   if(!prJpegHdr->fgEOF)
        		   {
        			  _MJPG_UpdateReadPtr(prJpegHdr);
        			  if(prJpegHdr->u4RingData < (UINT32)(200 * 1024))
        			  {
        				 i4Ret=_MJPG_RequestData(prDecoder);
        				 if(i4Ret!=0)
        				 {
        					Printf("Request data error %d\n",i4Ret);
        					i4Ret = -4;
        				 }
        				 i4RzPauseTime=0;
        			  }
        			  else
        			  {
        				 x_thread_delay(1);
        				 if(IMGRZ_GetScaleLine()==0)
        				 {
        					i4RzPauseTime++;
        				 }
        				 if(i4RzPauseTime>3000)
        				 {
        					 i4Ret = -5;
        					 break;
        				 }
        			  }
        			}
        			
        			if(VLDJPG_CheckDecodingError((JPEGHANDLE)&prDecoder->rJpgHw) != (INT32)E_HWJPG_OK)
        			{
        				i4Ret = -6;
        				break;
        			}
        		}
            }
            
            VLDJPG_SetIsrSem((HANDLE_T)NULL);
            IMGRZ_Wait();
            IMGRZ_Unlock();
       }        
    return i4Ret;
}
#endif
static void _MJPG_AsyncLoop(void* pvArg)
{
    INT32 i4Ret=0;
	MJPEG_DECODER *prDecodr;
	UCHAR ucImgId =0;
    #ifdef CC_SUPPORT_MTK_ANDROID_RM
    INT32 i4RzId = 0;

    if(pvArg)
    {
        i4RzId = *(INT32*)pvArg;
    }    
    #endif
    while(1)
    {
		VERIFY (x_sema_lock(_hMJPG_AsyncSeam, X_SEMA_OPTION_WAIT) == OSR_OK);
		if(_prCurDecoder==NULL)
		{
		   Printf("MJPG Async Lock sema,but decoder is null\n");
           continue;
		}
		prDecodr=_prCurDecoder;
		u4hSemaphoreDBGInsert(u4hardwareSema, 12+(ucImgId*0xf0));
		IMG_LockHwResource(ucImgId);
		u4hSemaphoreDBGInsert(u4hardwareSema, 13+(ucImgId*0xf0));
		
#ifdef CC_SUPPORT_MTK_ANDROID_RM                
                i4Ret=_MJPG_DecodeFrame(prDecodr, i4RzId);
#else
		i4Ret=_MJPG_DecodeFrame(prDecodr);
#endif

		u4hSemaphoreDBGInsert(u4hardwareSema, 14+(ucImgId*0xf0));
		IMG_UnLockHwResource(ucImgId);
		u4hSemaphoreDBGInsert(u4hardwareSema, 15+(ucImgId*0xf0));
		if(i4Ret!=0)
		{
		   Printf("MJPG Async:_MJPG_DecodeFrame Error %d\n",i4Ret);
           prDecodr->fpDevCb(prDecodr->hDevice,MJPEG_CB_ERROR,0,0);
		}
		else
		{
			prDecodr->fpDevCb(prDecodr->hDevice,MJPEG_CB_FINISH,0,0);
		}
    }
}
#if 0
UINT32 MJPG_GetWorkBufSize(UINT32 u4Width,UINT32 u4Height)
{
   UNUSED(u4Width);
   UNUSED(u4Height);
   return MJPEG_WORKBUF_SIZE;
}
#endif

#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
/*
* For oryx, dynamic map/unmap fbm to kernal space to get virtual address
* just for some memory that need to be access by cpu
*void BSP_MapReservedMem((void*)phy_addr, UINT32size)
*void BSP_UnMapReservedMem(void *)
*/
static UINT32 u4MJPGMapAddr = 0;
static BOOL fgMjpegFbmMapped = FALSE;
void _MJPGDynamicFbmMap(void)
{   
    FBM_POOL_T* prFbmPool;    
    UINT32 u4Size = 0;

    if(fgMjpegFbmMapped)
        return;
        
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    u4Size = (MJPEG_WORKBUF_SIZE_ALIGN + 0xfff) & (~0xfff);
    u4MJPGMapAddr = (UINT32)BSP_MapReservedMem((void *)(prFbmPool->u4Addr + prFbmPool->u4Size - u4Size), u4Size);
    if(u4MJPGMapAddr == 0)
    {
        LOG(0, "[%s][%d]map fail addr[0x%x] size[0x%x].\n", __FUNCTION__, __LINE__, (prFbmPool->u4Addr + prFbmPool->u4Size - u4Size), u4Size);
        ASSERT(0);
    }
    else
    {
        fgMjpegFbmMapped = TRUE;
        LOG(1, "_MJPGDynamicFbmMap map success.\n");
    }
    
    return;
}

void _MJPG_Unmap(void)
{
    if(fgMjpegFbmMapped)
    {
        BSP_UnMapReservedMem((void *)u4MJPGMapAddr);
        u4MJPGMapAddr = 0;
        fgMjpegFbmMapped = FALSE;
        LOG(1, "_MJPG_Unmap unmap success.\n");        
    }
    
    return;
}
#endif
#ifdef CC_SUPPORT_MTK_ANDROID_RM            
INT32 MJPG_DecoderOpen(HANDLE_T hDevice,HANDLE_T *hDecoder,
	MJPG_OPEN_INFO_T *prOpenInfo, INT32 i4RzId)
{
    MJPEG_DECODER *prDecoder=NULL;
    MJPEG_DEC_PARAM *prDecParam;
    UINT32 u4WorkBuf;
    UINT8 i = 0;
	
	if(prOpenInfo->u4AsyncMode && _u4MJPG_AsyncInit==0)
	{
		HANDLE_T hThread;
		VERIFY (x_sema_create(&_hMJPG_AsyncSeam, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
		VERIFY(x_thread_create(&hThread, MJPG_THREAD_NAME, 3072,
                   80,  _MJPG_AsyncLoop, sizeof(i4RzId), (void*)&i4RzId) == OSR_OK);
		_u4MJPG_AsyncInit=1;
		LOG(0, "@@@@ _u4MJPG_AsyncInit = 1\n");		
	}

	if(!_u4MJPGInit)
	{
           GFX_Init();
           VERIFY (x_sema_create(&_hMJPG_IsrSeam, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
	       _u4MJPGInit=1;
		   LOG(0, "@@@@ _u4MJPGInit = 1\n");
	}
    prDecoder = x_mem_alloc(sizeof(MJPEG_DECODER));
	if(prDecoder==NULL)
	{
       return MJPEG_RET_ERR;
	}
    //mjpeg playing detect, set the flag to true.
    fgIsMJPEGPlaying = TRUE;    
#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING 
    #ifndef CC_SUPPORT_MTK_ANDROID_RM
    _MJPGDynamicFbmMap();
    #endif
#endif

	x_memset(prDecoder,0,sizeof(MJPEG_DECODER));
	prDecoder->hDevice=hDevice;
	prDecoder->fpDevCb=prOpenInfo->fpDevCb;
	prDecParam=&prDecoder->rDecParam;
#ifdef CC_SUPPORT_MTK_ANDROID_RM
    u4WorkBuf =prOpenInfo->u4WorkBufStart;    
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4RzWorkBuf = u4WorkBuf;
    
    u4WorkBuf += MJPEG_RZWORK_BUF;    
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[0]=u4WorkBuf;
    
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[1]=u4WorkBuf;
    LOG(1, "jpeg huffman table2:0x%x\n", prDecParam->u4HufTable[1]);
        u4WorkBuf = prOpenInfo->u4WorkBufStart + MJPEG_FIRST_WORKBUF_ALIGN_SIZE;
#else
	u4WorkBuf=prOpenInfo->u4WorkBufStart;
#endif    
        LOG(2, "mjpeg color buffer start:0x%x\n", u4WorkBuf);
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[0]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[3]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[1]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[4]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[2]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[5]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
#ifndef CC_SUPPORT_MTK_ANDROID_RM    
	prDecParam->u4RzWorkBuf=u4WorkBuf;
		
	u4WorkBuf+=MJPEG_RZWORK_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[0]=u4WorkBuf;
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[1]=u4WorkBuf;
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
#endif
    LOG(2, "mjepg color buffer init\n");
    for(i = 0; i < 6; i++)
    {
        LOG(2, "%d:0x%x ",i, prDecParam->u4ColorBuf[i]);
        if(i == 2)
        {
            LOG(3, "\n");
        }
    }

    if(u4WorkBuf>prOpenInfo->u4WorkBufStart+prOpenInfo->u4WorkBufSize)
	{
	   x_mem_free(prDecoder);
            return -1;
	}
        prDecoder->u4WorkBuffer=u4WorkBuf;
	prDecoder->u4WorkBufSize=prOpenInfo->u4WorkBufSize-(u4WorkBuf-prOpenInfo->u4WorkBufStart);
	prDecoder->u4DataBuffer=prOpenInfo->u4DataBufStart;
	prDecoder->u4DataBufSize=prOpenInfo->u4DataBufSize;

	prDecoder->u4AsyncMode=prOpenInfo->u4AsyncMode;
	if(prDecoder->u4AsyncMode)
	{
       _prCurDecoder=prDecoder;
	}
	
	*hDecoder=(HANDLE_T)prDecoder;
	
	return MJPEG_RET_OK;
}

INT32 MJPG_DecoderDecode(HANDLE_T hDecoder,MJPG_INPARAM_T *prInParam,MJPG_OUTPARAM_T*prOutParam, INT32 i4RzId)
{
        MJPEG_DECODER *prDecoder=(MJPEG_DECODER*)hDecoder;
	JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
	UCHAR ucImgId =0;
	INT32 i4Ret=MJPEG_RET_OK;
	if(hDecoder==0 || prInParam==NULL || prOutParam==NULL)
	{
		return MJPEG_RET_INVALID_PARAM;
	}
	x_memset(&prDecoder->rJpgHeader,0,sizeof(JPEG_JFIF_HEADER_T));
	x_memset(&prDecoder->rJpgHw,0,sizeof(JPEG_VLDJPG_DATA_T));
	
	prDecoder->prInParam=prInParam;
	prDecoder->prOutParam=prOutParam;
	prJpegHdr->rParent.eJpegClass=E_JC_JFIF_HEADER;
	prJpegHdr->rParent.zObjectSize=sizeof(JPEG_JFIF_HEADER_T);
	prJpegHdr->fgRingBuffer=TRUE;
	prJpegHdr->u4RingRP = 0;
	prJpegHdr->ucPicStruct=3;
#ifdef CC_FUJI3D_SUPPORT
    prJpegHdr->fgChkMPO = prInParam->fgCheckMPF;
#endif

	if(prInParam->u4DataReady)
	{
	    prJpegHdr->u4RingData=prInParam->u4FrameSize;
		prJpegHdr->fgEOF=TRUE;
	}
	else
	{
	    prJpegHdr->u4RingData=0;
		prJpegHdr->fgEOF=FALSE;
	}
	
	prJpegHdr->pu1RingBufStart=(UINT8 *)VIRTUAL(prDecoder->u4DataBuffer);
	prJpegHdr->u4RingBufSize=prDecoder->u4DataBufSize;

	if(prInParam->u4ForVdec)
	{
	   prJpegHdr->fgEOF=TRUE;
	   prDecoder->rDecParam.u4DataAddr=(UINT32)VIRTUAL(prInParam->u4DataAddr);
	}
	else
	{
	   prDecoder->rDecParam.u4DataAddr=(UINT32)prJpegHdr->pu1RingBufStart;
	}
	
	if(prDecoder->u4AsyncMode)
	{
		VERIFY (x_sema_unlock(_hMJPG_AsyncSeam) == OSR_OK);
	}
	else
	{
		u4hSemaphoreDBGInsert(u4hardwareSema, 16+(ucImgId*0xf0));
		IMG_LockHwResource(ucImgId);
		u4hSemaphoreDBGInsert(u4hardwareSema, 17+(ucImgId*0xf0));
		
		i4Ret=_MJPG_DecodeFrame(prDecoder, i4RzId);
		u4hSemaphoreDBGInsert(u4hardwareSema, 18+(ucImgId*0xf0));
		IMG_UnLockHwResource(ucImgId);		
		u4hSemaphoreDBGInsert(u4hardwareSema, 19+(ucImgId*0xf0));
		
		if(i4Ret!=0)
		{
		   Printf("MJPG Sync:_MJPG_DecodeFrame Error %d\n",i4Ret);
		   return MJPEG_RET_ERR;
		}
	}

	return MJPEG_RET_OK;
}

#else
INT32 MJPG_DecoderOpen(HANDLE_T hDevice,HANDLE_T *hDecoder,
	MJPG_OPEN_INFO_T *prOpenInfo)
{
    MJPEG_DECODER *prDecoder=NULL;
	MJPEG_DEC_PARAM *prDecParam;
	UINT32 u4WorkBuf;
    UINT8 i = 0;
	
	if(prOpenInfo->u4AsyncMode && _u4MJPG_AsyncInit==0)
	{
		HANDLE_T hThread;
		VERIFY (x_sema_create(&_hMJPG_AsyncSeam, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
		VERIFY(x_thread_create(&hThread, MJPG_THREAD_NAME, 3072,
			80,  _MJPG_AsyncLoop, 4, NULL) == OSR_OK);
		_u4MJPG_AsyncInit=1;
		LOG(0, "@@@@ _u4MJPG_AsyncInit = 1\n");		
	}

	if(!_u4MJPGInit)
	{
           GFX_Init();
           VERIFY (x_sema_create(&_hMJPG_IsrSeam, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
	       _u4MJPGInit=1;
		   LOG(0, "@@@@ _u4MJPGInit = 1\n");
	}
    prDecoder = x_mem_alloc(sizeof(MJPEG_DECODER));
	if(prDecoder==NULL)
	{
       return MJPEG_RET_ERR;
	}
    //mjpeg playing detect, set the flag to true.
    fgIsMJPEGPlaying = TRUE;    
#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING 
    #ifndef CC_SUPPORT_MTK_ANDROID_RM
    _MJPGDynamicFbmMap();
    #endif
#endif

	x_memset(prDecoder,0,sizeof(MJPEG_DECODER));
	prDecoder->hDevice=hDevice;
	prDecoder->fpDevCb=prOpenInfo->fpDevCb;
	prDecParam=&prDecoder->rDecParam;
#ifdef CC_SUPPORT_MTK_ANDROID_RM
    u4WorkBuf =prOpenInfo->u4WorkBufStart;    
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4RzWorkBuf = u4WorkBuf;
    
    u4WorkBuf += MJPEG_RZWORK_BUF;    
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[0]=u4WorkBuf;
    
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[1]=u4WorkBuf;
    LOG(1, "jpeg huffman table2:0x%x\n", prDecParam->u4HufTable[1]);
        u4WorkBuf = prOpenInfo->u4WorkBufStart + MJPEG_FIRST_WORKBUF_ALIGN_SIZE;
#else
	u4WorkBuf=prOpenInfo->u4WorkBufStart;
#endif    
        LOG(2, "mjpeg color buffer start:0x%x\n", u4WorkBuf);
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[0]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[3]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[1]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[4]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[2]=u4WorkBuf;
	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
	prDecParam->u4ColorBuf[5]=u4WorkBuf;

	u4WorkBuf+=MJPEG_COLOR_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
#ifndef CC_SUPPORT_MTK_ANDROID_RM    
	prDecParam->u4RzWorkBuf=u4WorkBuf;
		
	u4WorkBuf+=MJPEG_RZWORK_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[0]=u4WorkBuf;
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
    prDecParam->u4HufTable[1]=u4WorkBuf;
	u4WorkBuf+=MJPEG_HUFTABLE_BUF;
	MJPEG_VALUE_ALIGN(u4WorkBuf,64);
#endif
    LOG(2, "mjepg color buffer init\n");
    for(i = 0; i < 6; i++)
    {
        LOG(2, "%d:0x%x ",i, prDecParam->u4ColorBuf[i]);
        if(i == 2)
        {
            LOG(3, "\n");
        }
    }

    if(u4WorkBuf>prOpenInfo->u4WorkBufStart+prOpenInfo->u4WorkBufSize)
	{
	   x_mem_free(prDecoder);
            return -1;
	}
        prDecoder->u4WorkBuffer=u4WorkBuf;
	prDecoder->u4WorkBufSize=prOpenInfo->u4WorkBufSize-(u4WorkBuf-prOpenInfo->u4WorkBufStart);
	prDecoder->u4DataBuffer=prOpenInfo->u4DataBufStart;
	prDecoder->u4DataBufSize=prOpenInfo->u4DataBufSize;

	prDecoder->u4AsyncMode=prOpenInfo->u4AsyncMode;
	if(prDecoder->u4AsyncMode)
	{
       _prCurDecoder=prDecoder;
	}
	
	*hDecoder=(HANDLE_T)prDecoder;
	
	return MJPEG_RET_OK;
}

INT32 MJPG_DecoderDecode(HANDLE_T hDecoder,MJPG_INPARAM_T *prInParam,MJPG_OUTPARAM_T*prOutParam)
{
    MJPEG_DECODER *prDecoder=(MJPEG_DECODER*)hDecoder;
	JPEG_JFIF_HEADER_T *prJpegHdr=&prDecoder->rJpgHeader;
	UCHAR ucImgId =0;
	INT32 i4Ret=MJPEG_RET_OK;
	if(hDecoder==0 || prInParam==NULL || prOutParam==NULL)
	{
		return MJPEG_RET_INVALID_PARAM;
	}
	x_memset(&prDecoder->rJpgHeader,0,sizeof(JPEG_JFIF_HEADER_T));
	x_memset(&prDecoder->rJpgHw,0,sizeof(JPEG_VLDJPG_DATA_T));
	
	prDecoder->prInParam=prInParam;
	prDecoder->prOutParam=prOutParam;
	prJpegHdr->rParent.eJpegClass=E_JC_JFIF_HEADER;
	prJpegHdr->rParent.zObjectSize=sizeof(JPEG_JFIF_HEADER_T);
	prJpegHdr->fgRingBuffer=TRUE;
	prJpegHdr->u4RingRP = 0;
	prJpegHdr->ucPicStruct=3;
#ifdef CC_FUJI3D_SUPPORT
    prJpegHdr->fgChkMPO = prInParam->fgCheckMPF;
#endif

	if(prInParam->u4DataReady)
	{
	    prJpegHdr->u4RingData=prInParam->u4FrameSize;
		prJpegHdr->fgEOF=TRUE;
	}
	else
	{
	    prJpegHdr->u4RingData=0;
		prJpegHdr->fgEOF=FALSE;
	}
	
	prJpegHdr->pu1RingBufStart=(UINT8 *)VIRTUAL(prDecoder->u4DataBuffer);
	prJpegHdr->u4RingBufSize=prDecoder->u4DataBufSize;

	if(prInParam->u4ForVdec)
	{
	   prJpegHdr->fgEOF=TRUE;
	   prDecoder->rDecParam.u4DataAddr=(UINT32)VIRTUAL(prInParam->u4DataAddr);
	}
	else
	{
	   prDecoder->rDecParam.u4DataAddr=(UINT32)prJpegHdr->pu1RingBufStart;
	}
	
	if(prDecoder->u4AsyncMode)
	{
		VERIFY (x_sema_unlock(_hMJPG_AsyncSeam) == OSR_OK);
	}
	else
	{
		u4hSemaphoreDBGInsert(u4hardwareSema, 16+(ucImgId*0xf0));
		IMG_LockHwResource(ucImgId);
		u4hSemaphoreDBGInsert(u4hardwareSema, 17+(ucImgId*0xf0));
		
		i4Ret=_MJPG_DecodeFrame(prDecoder);

		u4hSemaphoreDBGInsert(u4hardwareSema, 18+(ucImgId*0xf0));
		IMG_UnLockHwResource(ucImgId);		
		u4hSemaphoreDBGInsert(u4hardwareSema, 19+(ucImgId*0xf0));
		
		if(i4Ret!=0)
		{
		   Printf("MJPG Sync:_MJPG_DecodeFrame Error %d\n",i4Ret);
		   return MJPEG_RET_ERR;
		}
	}

	return MJPEG_RET_OK;
}
#endif
INT32 MJPG_DecoderClose(HANDLE_T hDecoder)
{
    MJPEG_DECODER *prDecoder=(MJPEG_DECODER*)hDecoder;
	if(prDecoder==NULL)
	{
       return MJPEG_RET_INVALID_PARAM;
	}
        x_mem_free(prDecoder);
	_prCurDecoder=NULL;
	VLDJPG_SetPowerOff();
	//set MJPEG playing flag to false
	fgIsMJPEGPlaying = FALSE;
#ifdef CC_VDEC_FBM_DYNAMIC_MAPPING 
    #ifndef CC_SUPPORT_MTK_ANDROID_RM
    _MJPG_Unmap();
    #endif
#endif

	return MJPEG_RET_OK;
}

UINT32 MJPG_VLDPeekBytes(HANDLE_T hDecoder, UINT32 offset)
{	
    UINT32 u4RetVal = 0;
    MJPEG_DECODER *prDecoder=(MJPEG_DECODER*)hDecoder;	
	if(prDecoder==NULL)
	{
       return MJPEG_RET_INVALID_PARAM;
	}	
    u4RetVal = VLDJPG_BarrelShifterPeekByte(offset);
	return u4RetVal;
}

UINT32 MJPG_VLDSkipBytes(HANDLE_T hDecoder, UINT32 offset)
{	
    UINT32 u4RetVal = 0;
    MJPEG_DECODER *prDecoder=(MJPEG_DECODER*)hDecoder;	
	if(prDecoder==NULL)
	{
       return MJPEG_RET_INVALID_PARAM;
	}	
    u4RetVal = VLDJPG_BarrelShifterSkipBytes(offset);
	return u4RetVal;
}
JPEG_JFIF_SOF_T* MJPEG_GetSOF(HANDLE_T hDecoder)
{
    MJPEG_DECODER *prDecoder = (MJPEG_DECODER*)hDecoder;    
    JPEG_JFIF_SOF_T* prJpegSOF = &((&prDecoder->rJpgHeader)->rSOF);

    return prJpegSOF;
}
