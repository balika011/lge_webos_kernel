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
jfif header
*/

#ifndef __JDEC_COM_JFIF_H
#define __JDEC_COM_JFIF_H

#include "jdec_com_common.h"

#ifndef MT8520
extern void x_memset(void *pv_to, UINT8 ui1_c, SIZE_T z_l);
extern void x_memcpy(void *pv_to, const void *pv_from, SIZE_T z_l);
#else
#include "x_common.h"
#endif


typedef void* JPEGHANDLE;

#define MAX_JPEG_COMPONENT      3
#define D_MAX_NUM_APP0          2
#define D_MAX_NUM_SCAN          32

/* JPEG marker codes */
typedef enum
{
	M_SOF0  = 0xffc0,
	M_SOF1  = 0xffc1,
	M_SOF2  = 0xffc2,
	M_SOF3  = 0xffc3,

	M_SOF5  = 0xffc5,
	M_SOF6  = 0xffc6,
	M_SOF7  = 0xffc7,

	M_JPG   = 0xffc8,
	M_SOF9  = 0xffc9,
	M_SOF10 = 0xffca,
	M_SOF11 = 0xffcb,

	M_SOF13 = 0xffcd,
	M_SOF14 = 0xffce,
	M_SOF15 = 0xffcf,

	M_DHT   = 0xffc4,

	M_DAC   = 0xffcc,

	M_RST0  = 0xffd0,
	M_RST1  = 0xffd1,
	M_RST2  = 0xffd2,
	M_RST3  = 0xffd3,
	M_RST4  = 0xffd4,
	M_RST5  = 0xffd5,
	M_RST6  = 0xffd6,
	M_RST7  = 0xffd7,

	M_SOI   = 0xffd8,
	M_EOI   = 0xffd9,
	M_SOS   = 0xffda,
	M_DQT   = 0xffdb,
	M_DNL   = 0xffdc,
	M_DRI   = 0xffdd,
	M_DHP   = 0xffde,
	M_EXP   = 0xffdf,

	M_APP0  = 0xffe0,
	M_APP1  = 0xffe1,
	M_APP2  = 0xffe2,
	M_APP3  = 0xffe3,
	M_APP4  = 0xffe4,
	M_APP5  = 0xffe5,
	M_APP6  = 0xffe6,
	M_APP7  = 0xffe7,
	M_APP8  = 0xffe8,
	M_APP9  = 0xffe9,
	M_APP10 = 0xffea,
	M_APP11 = 0xffeb,
	M_APP12 = 0xffec,
	M_APP13 = 0xffed,
	M_APP14 = 0xffee,
	M_APP15 = 0xffef,

	M_JPG0  = 0xfff0,
	M_JPG13 = 0xfffd,
	M_COM   = 0xfffe,

	M_TEM   = 0xff01,

	M_ERROR = 0x100
} JDEC_MARKER_CODE_T;

/* jpeg format */
typedef enum
{
	E_JPG_UNKNOWN_FORMAT,
	E_JPG_BASELINE,
	E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN,
	E_JPG_PROGRESSIVE_HUFFMAN,
	E_JPG_EXTENDED_SEQUENTIAL_ARITHMETIC,
	E_JPG_PROGRESSIVE_ARITHMETIC,
	E_JPG_LOSSLESS_HUFFMAN,
	E_JPG_DIFFERENTIAL_SEQ_HUFFMAN,
	E_JPG_DIFFERENTIAL_PROGRESSIVE_HUFFMAN,
	E_JPG_DIFFERENTIAL_LOSSLESS_HUFFMAN,
	E_JPG_RESERVED_FOR_EXTENSIONS,
	E_JPG_LOSSLESS_ARITHMETIC,
	E_JPG_DIFFERENTIAL_SEQUENTIAL_ARITHMETIC,
	E_JPG_DIFFERENTIAL_PROGRESSIVE_ARITHMETIC,
	E_JPG_UNSUPPORT_FORMAT
} JDEC_FORMAT_T;

/* component info in SOF marker */
typedef struct _JDEC_SOF_COMP_T
{
	UINT8		u1ComponentId;
	UINT8		u1HSampFactor;
	UINT8		u1VSampFactor;
	UINT8		u1QuantTblNo;
    UINT8       u1CompWidth; // component width
    UINT16      u2CompCofSize; // component cofficient buffer width
} JDEC_SOF_COMP_T;

/* SOF data */
typedef struct _JDEC_JFIF_SOF_T
{
	JDEC_FORMAT_T		    eJpegFormat;
	UINT8					u1DataPrecision;
	UINT16					u2ImageHeight;
	UINT16					u2ImageWidth;
	UINT8					u1NumComponents;
	JDEC_SOF_COMP_T			arSofComp[MAX_JPEG_COMPONENT];
	UINT8					au1MapId2Index[256];
	UINT32                  au4ImgMCUHeight[MAX_JPEG_COMPONENT];
	UINT32                  au4ImgMCUWidth[MAX_JPEG_COMPONENT];	
	UINT32                  au4ImgLastMCUHeight[MAX_JPEG_COMPONENT];	
	UINT32                  u4ImgComMCUWidth;
	UINT32                  u4ImgComMCUHeight;
} JDEC_JFIF_SOF_T;

/* raw de-huffman table */
typedef struct _JDEC_DHT_HUFF_TBL_T
{
	UINT8 au1Bits[17];
	UINT8 au1HuffVal[256];
} JDEC_DHT_HUFF_TBL_T;

/* DHT data */
typedef struct _JDEC_JFIF_DHT_T
{
	UINT32					u4NumDcTbl;
	UINT32					u4NumAcTbl;
	UINT32					u4DcTblLoaded;	/* bit mask for loaded dc table */
	UINT32					u4AcTblLoaded;	/* bit mask for loaded ac table */
	JDEC_DHT_HUFF_TBL_T		arDcTbl[4];
	JDEC_DHT_HUFF_TBL_T		arAcTbl[4];
} JDEC_JFIF_DHT_T;

/* DQT data */
typedef struct _JDEC_JFIF_DQT_T
{
	/*
	although we leave 2bytes * 64 space here,
	if q table precision is 8bits, we use only
	first half (1x64) of this table
	*/
	UINT8				aau1Qtbl[4][128];
	BOOL				afgPrec[4];
	UINT8				u1NumQ;
} JDEC_JFIF_DQT_T;

typedef struct _JDEC_JFIF_SOS_T
{
	UINT8				u1CompInScan;
	UINT8				au1CompIdx[MAX_JPEG_COMPONENT];
	UINT8				au1DcId[MAX_JPEG_COMPONENT];
	UINT8				au1AcId[MAX_JPEG_COMPONENT];
	UINT8				u1Ss, u1Se, u1Ah, u1Al;
} JDEC_JFIF_SOS_T;


typedef enum _ENUM_APP0_THUMBNAIL_MODE_T
{
	E_THUMB_NONE			= 0x00,
	E_THUMB_JPEG			= 0x10,
	E_THUMB_1BYTE_PIX		= 0x11,
	E_THUMB_3BYTE_PIX		= 0x13
} ENUM_APP0_THUMBNAIL_MODE;

typedef enum _ENUM_APP0_TYPE_T
{
	E_APP0_JFIF,
	E_APP0_JFXX,
	E_APP0_ADOBE,
	E_APP0_OTHER
} ENUM_APP0_TYPE_T;

/* APP0 - jfif, jfxx */
typedef struct _JDEC_JFIF_APP0_T
{
	BOOL				fgAPP0;
	ENUM_APP0_TYPE_T	eApp0Type;
	/* jfif */
	UINT16				u2Version;
	UINT8				u1Units;
	UINT16				u2DensityX;
	UINT16				u2DensityY;

	/* jfxx */
	ENUM_APP0_THUMBNAIL_MODE	eThumbnailMode;

	/* common part in all thumbnail mode */
	BOOL				fgHasThumbnail;
	/* thumbnail dimension */
	UINT8				u1ThumbW;
	UINT8				u1ThumbH;
	/* jpeg thumbnail mode */
	UINT8				*pu1JpegThumbnail;	/* point to JPEG data in APP0 stream */
	/* 1 byte pixel thumbnail mode */
	UINT8				*pu1ColorData;
	UINT32				au4Palette[256];	/* force A to 255, R G B read from APP0 */
	/* 3 byte pixel thumbnail mode */
	UINT8				*pu1RGBdata;		/* point to RGB data in APP0 stream */
} JDEC_JFIF_APP0_T;

/* APP1 - exif */
typedef struct _JDEC_JFIF_APP1_T
{
	BOOL				fgAPP1;		/* APP1 marker found */
	UINT8				*pu1ExifData;
	/* TODO: implement thumbnail picture in 1st IFD here */
	/* TODO: implement partial 0th IFD tag here */
} JDEC_JFIF_APP1_T;

typedef struct _JPEG_JFIF_HEADER_T
{
	UINT32				u4ValidData;            ///< file size of this jpeg image
	UINT8				*pu1CurrRead;           ///< current read pointer
	UINT32              u4FifoStart;            ///< Fifo Start Address
	UINT32              u4FifoEnd;              ///< Fifo Start Address
	UINT32              u4HasReadData;
	UINT32              u4JumpData;
	void                *pvCbInst;              ///< request header bitstream callback function instance

	UINT16              u2CurrMarker;
	UINT16              u2CurrMarkerSz;

	BOOL				fgPrsResult;		    ///< jpeg image parsed successfully
	BOOL				fgFatalError;		    ///< fatal error found and can not decode
	BOOL				fgThumbnail;		    ///< thumbnail image available (app0 or app1 marker)

	BOOL				fgSOI;				    ///< indicate that already have an SOI marker
    BOOL                fgSOF;                  ///< indicate that already have an SOF marker
    BOOL                fgDQT;                  ///< indicate that already have an DQT marker
	BOOL				fgEOI;                  ///< indicate that already have an EOI marker

	JDEC_JFIF_SOF_T		rSOF;       	        ///< SOF - start of frame
	JDEC_JFIF_DHT_T		rDHT;                   ///< DHT - de-huffman table
	JDEC_JFIF_DQT_T		rDQT;                   ///< DQT - de-quantization table
	UINT16				u2RestartInterval;      ///< DRI - restart interval
	UINT32				u4NumApp0;              ///< APP0 - application0 number
	//JDEC_JFIF_APP0_T	arAPP0[D_MAX_NUM_APP0]; ///< APP0 - application0 data
	//JDEC_JFIF_APP1_T	rAPP1;                  ///< APP1 - application1
	JDEC_JFIF_SOS_T		rSOS;                   ///< SOS information
	UINT32              u4BitStmSrtOfst;        ///< the file offset of jpeg bit stream start
	UINT32              u4NeedData;             ///< for BDP00117567, how much data are needed for continue
} JPEG_JFIF_HEADER_T;

/* JFIF group - jpeg header parser */
JPEGHANDLE JFIF_AllocInstance		(void);
	// allocate an jpeg jfif header instance
void JFIF_FreeInstance				(JPEGHANDLE hInstance);
	// release an jpeg jfif header instance
INT32 JFIF_New						(JPEGHANDLE hInstance, void *pvJpegImage, UINT32 u4JpegSize);
	// start and parse a jpeg file image store in memory

#endif //__JDEC_COM_JFIF_H
