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


#include "jdec_com_common.h"
#include "jdec_com_jfif.h"
#include "x_os.h"
#include "drv_config.h"
#include "x_debug.h"

#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif

#define ADVANCE_BYTES(NBYTES) \
		i4Ret = _jfifAdvanceBytes(prInstance, NBYTES); \
		if (E_JPG_OK != i4Ret) \
		{ \
			return i4Ret; \
		} \


/* private interfaces */
static UINT8  _jfifReadByte			(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4NthByte);
	/* read 1 byte from jpeg stream */
static UINT16 _jfifRead2Bytes		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4NthByte);
	/* read 2 bytes from jpeg stream */

#define JPEG_READ_BYTE(PINSTANCE, NTH_BYTE) (_jfifReadByte(PINSTANCE, NTH_BYTE))
#define JPEG_READ_2BYTES(PINSTANCE, NTH_BYTE) (_jfifRead2Bytes(PINSTANCE, NTH_BYTE))

/* private interfaces */
static INT32 _jfifAdvanceBytes		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Bytes);
	/* advance read pointer n bytes */
static INT32 _jfifCheckInstance		(JPEG_JFIF_HEADER_T *prInstance);
	/* check instance */
static INT32 _jfifParse				(JPEG_JFIF_HEADER_T *prInstance);
	/* parse all marker in jpeg file */
static INT32 _jfifIsMarker			(JPEG_JFIF_HEADER_T *prInstance);
	/* check if current read pointer points to an valid marker */
static INT32 _jfifPeekMarker		(JPEG_JFIF_HEADER_T *prInstance);
	/* get next marker (read pointer stops on this marker) */
static INT32 _jfifSkipMarker		(JPEG_JFIF_HEADER_T *prInstance);
	/* skip current marker */
static INT32 _jfifDispatchMarker	(JPEG_JFIF_HEADER_T *prInstance);
	/* dispatch this marker to proper handler routine */
static INT32 _jfifProcSOI			(JPEG_JFIF_HEADER_T *prInstance);
	/* process start of image */
static INT32 _jfifProcSOF			(JPEG_JFIF_HEADER_T *prInstance);
	/* process start of frame */
static INT32 _jfifProcSOS			(JPEG_JFIF_HEADER_T *prInstance);
	/* process start of scan */
static INT32 _jfifProcDHT			(JPEG_JFIF_HEADER_T *prInstance);
	/* process de-huffman table */
static INT32 _jfifProcDQT			(JPEG_JFIF_HEADER_T *prInstance);
	/* process de-quantization table */
static INT32 _jfifProcDRI			(JPEG_JFIF_HEADER_T *prInstance);
	/* process restart interval marker */
static INT32 _jfifProcAPP0			(JPEG_JFIF_HEADER_T *prInstance);
	/* process standard APP0 "JFIF" marker (and "JFXX" extension) */
static INT32 _jfifProcAPP1			(JPEG_JFIF_HEADER_T *prInstance);
	/* process standard APP1 "Exif" marker */
static INT32 _jfifProcAPP14			(JPEG_JFIF_HEADER_T *prInstance);
	/* process APP14 */
static INT32 _jfifProcAPPother		(JPEG_JFIF_HEADER_T *prInstance);
	/* process all non-supported APPn format */
static INT32 _jfifProcEOI			(JPEG_JFIF_HEADER_T *prInstance);
	/* process end of image marker */


#ifndef MT8520
#include "assert.h"
#define ASSERT assert

void x_memset(void *pv_to, UINT8 ui1_c, SIZE_T z_l)
{
    UINT32 u4I = 0;

    for(u4I=0; u4I<z_l; u4I++)
        *(((BYTE *)pv_to) + u4I) = (BYTE)ui1_c;
}

void x_memcpy(void *pv_to, const void *pv_from, SIZE_T z_l)
{
    UINT32 u4I = 0;

    for(u4I=0; u4I<z_l; u4I++)
        *(((BYTE *)pv_to) + u4I) = *(((BYTE *)pv_from) + u4I);
}

INT32 x_memcmp(const void *pv_s1, const void *pv_s2, SIZE_T z_l)
{
    UINT32 u4I = 0;

    while(u4I < z_l)
    {
        if(*(((BYTE *)pv_s1) + u4I) != *(((BYTE *)pv_s2) + u4I))
            break;
        u4I++;
    }

    if(u4I == z_l)
      return 0;

    return 1;
}

void *x_mem_alloc(SIZE_T size)
{
  while(1);

  return 0;
}

UINT32 x_mem_free(void *addr)
{
  while(1);

  return 0;
}


INT16 Printf(const CHAR * format, ...)
{
  return 0;
}

#endif


//----------------------------------------------------------

/* private interfaces */
UINT8  _jfifReadByte			(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4NthByte)
{
    UINT8 * pu1Temp = 0;

    
    if ((UINT32)(prInstance->pu1CurrRead + u4NthByte) >= prInstance->u4FifoEnd)
    {
        //UTIL_Printf("[JPG] _jfifReadByte case1 pu4CurrRead = %x | u4NthByte = %d \n",prInstance->pu1CurrRead, u4NthByte);
        pu1Temp = (UINT8 *)(prInstance->pu1CurrRead + u4NthByte - prInstance->u4FifoEnd + prInstance->u4FifoStart);
        return pu1Temp[0];
    }
    else
    {
        return prInstance->pu1CurrRead[u4NthByte];
    }
}

UINT16 _jfifRead2Bytes		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4NthByte)
{
    UINT8* pu1Temp = NULL;

    
    if ((UINT32)(prInstance->pu1CurrRead + u4NthByte) >= prInstance->u4FifoEnd)
    {
        //UTIL_Printf("[JPG] _jfifRead2Bytes case1 pu4CurrRead = %x | u4NthByte = %d\n",prInstance->pu1CurrRead,u4NthByte);
        pu1Temp = (UINT8 *)(prInstance->pu1CurrRead + u4NthByte - prInstance->u4FifoEnd + prInstance->u4FifoStart);
        return ((pu1Temp[0] << 8) | (pu1Temp[1]));
        //return ((prInstance->pu1CurrRead[u4NthByte] << 8) | (prInstance->pu1CurrRead[u4NthByte + 1]));
    }
    else if ((UINT32)(prInstance->pu1CurrRead + u4NthByte) >= (prInstance->u4FifoEnd - 1))
    {
        //UTIL_Printf("[JPG] _jfifRead2Bytes case2 pu4CurrRead = %x | u4NthByte = %d\n",prInstance->pu1CurrRead, u4NthByte);
        pu1Temp = (UINT8 *)prInstance->u4FifoStart;
        return ((prInstance->pu1CurrRead[u4NthByte] << 8) | pu1Temp[0]);
    }
    else
    {
        return ((prInstance->pu1CurrRead[u4NthByte] << 8) | (prInstance->pu1CurrRead[u4NthByte + 1]));
    }
}

/* private interfaces */
static INT32 _jfifAdvanceBytes		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Bytes)
{
    VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

    if (prInstance->u4ValidData <= u4Bytes)
    {
        prInstance->u4NeedData = u4Bytes;
        return -E_JPG_ERR_PASS_END;
    }
    prInstance->u4ValidData -= u4Bytes;
    prInstance->u4HasReadData += u4Bytes;
    if (((UINT32)(prInstance->pu1CurrRead) + u4Bytes) >= prInstance->u4FifoEnd)
    {
        prInstance->pu1CurrRead = (UINT8*)(prInstance->u4FifoStart + (((UINT32)(prInstance->pu1CurrRead) + u4Bytes) - prInstance->u4FifoEnd));
    }
    else
    {
        prInstance->pu1CurrRead += u4Bytes;
    }
    return E_JPG_OK;
}

/* instance check */
static INT32 _jfifCheckInstance		(JPEG_JFIF_HEADER_T *prInstance)
{
	if (prInstance == NULL)
	{
		// null instance
		return -E_JPG_ERR_PARAM;
	}

	if (prInstance->fgFatalError == TRUE)
	{
		// an fatal error happened
		return -E_JPG_ERR_FATAL;
	}

	return E_JPG_OK;
}

/* parse header markers in jpeg file */
static INT32 _jfifParse				(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);
/*
	if (prInstance->fgPrsResult == TRUE)
	{
		// already parsed
		return -E_JPG_ERR_ALREADY;
	}
*/
	// parse until EOI
	for(;;)
	{
	    if (prInstance->u4ValidData <= 8)
	    {
            prInstance->u4NeedData = 8;
	        return -E_JPG_ERR_PASS_END;
	    }
		i4Ret = _jfifPeekMarker(prInstance);
		if (E_JPG_OK == i4Ret)
		{
		    prInstance->u2CurrMarkerSz = JPEG_READ_2BYTES(prInstance, 2);
			// marker found
			i4Ret = _jfifDispatchMarker(prInstance);

			if ((M_EOI == prInstance->u2CurrMarker) ||
				(M_SOS == prInstance->u2CurrMarker))
			{
				// stop condition reached - EOI
				if ((prInstance->rDHT.u4DcTblLoaded) &&
					(prInstance->fgSOF == TRUE) &&
					(prInstance->fgSOI == TRUE) &&
					(prInstance->fgDQT == TRUE) &&
					(prInstance->fgFatalError == FALSE) &&
					(i4Ret == E_JPG_OK)) // add by pingzhao for BDP00218299 @2008/11/12
				{
					prInstance->fgPrsResult = TRUE;
				}
				else
				{
					prInstance->fgPrsResult = FALSE;
				}
				break;
			}

			if (E_JPG_OK != i4Ret)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return i4Ret;
}


/* check if current read pointer points to an valid marker */
static INT32 _jfifIsMarker			(JPEG_JFIF_HEADER_T *prInstance)
{
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->u4ValidData <= 0)
	{
		return -E_JPG_ERR_PASS_END;
	}

	if ((prInstance->pu1CurrRead[0] == 0xff) && (prInstance->pu1CurrRead[1] != 0xff))
	{
		return E_JPG_OK;
	}

	return -E_JPG_ERR_NOT_A_MARKER;
}

/* get next marker (read pointer stops on this marker) */
static INT32 _jfifPeekMarker		(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	while ((i4Ret = _jfifIsMarker(prInstance)) == -E_JPG_ERR_NOT_A_MARKER)
	{
		ADVANCE_BYTES(1);
	}
	if (prInstance->u4ValidData <= 8)
	{
        prInstance->u4NeedData = 8;
	    return -E_JPG_ERR_PASS_END;
	}

	prInstance->u2CurrMarker = JPEG_READ_2BYTES(prInstance, 0);

	// exception handling
	switch (prInstance->u2CurrMarker)
	{
    	// we can process some special case here
	    default:
		    break;
	}

	return E_JPG_OK;
}

/* skip current marker */
static INT32 _jfifSkipMarker		(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	//UINT16 u2Length = prInstance->u2CurrMarkerSz + 2;
	UINT32 u4Length = prInstance->u2CurrMarkerSz + 2;

    if (prInstance->u4ValidData <= u4Length)
    {
        if (u4Length >= (prInstance->u4FifoEnd - prInstance->u4FifoStart)/2)
        {
            prInstance->u4JumpData = u4Length;
            prInstance->u4HasReadData += u4Length;
            return -E_JPG_ERR_SKIP_DATA;
        }
        else
        {
            prInstance->u4NeedData = u4Length;
            return -E_JPG_ERR_PASS_END;
        }
    }
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	ADVANCE_BYTES(u4Length);

	return E_JPG_OK;
}

/* dispatch this marker to proper handler routine */
static INT32 _jfifDispatchMarker	(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	switch (prInstance->u2CurrMarker)
	{
	case (UINT16)M_SOI		:		// start of image
		i4Ret = _jfifProcSOI(prInstance);
		break;
	case (UINT16)M_SOF0		:		// baseline
	case (UINT16)M_SOF1		:		// extended sequential huffman
	case (UINT16)M_SOF2		:		// progressive huffman
	case (UINT16)M_SOF3		:		// lossless huffman
	case (UINT16)M_SOF5		:		// differential sequential huffman
	case (UINT16)M_SOF6		:		// differential progressive huffman
	case (UINT16)M_SOF7		:		// differential lossless huffman
	case (UINT16)M_JPG		:		// reserved jpeg extension
	case (UINT16)M_SOF9		:		// extended sequential arithmetic
	case (UINT16)M_SOF10	:		// progressive arithmetic
	case (UINT16)M_SOF11	:		// lossless arithmetic
	case (UINT16)M_SOF13	:		// differential sequential arithmetic
	case (UINT16)M_SOF14	:		// differential progressive arithmetic
	case (UINT16)M_SOF15	:		// differential lossless arithmetic
		i4Ret = _jfifProcSOF(prInstance);
		if ((E_JPG_OK != i4Ret) && (i4Ret != -E_JPG_ERR_PASS_END))
		{
			//prInstance->rSOF.eJpegFormat	= E_JPG_UNSUPPORT_FORMAT;
			prInstance->fgFatalError		= TRUE;
		}
		break;
	case (UINT16)M_SOS		:		// start of scan
		i4Ret = _jfifProcSOS(prInstance);
		break;
	case (UINT16)M_EOI		:		// end of image
		i4Ret = _jfifProcEOI(prInstance);
		break;
	case (UINT16)M_DHT		:		// de-huffman table
		i4Ret = _jfifProcDHT(prInstance);
		break;
	case (UINT16)M_DQT		:		// de-quantization table
		i4Ret = _jfifProcDQT(prInstance);
		break;
	case (UINT16)M_DRI		:		// restart interval marker
		i4Ret = _jfifProcDRI(prInstance);
		break;
	case (UINT16)M_APP0		:
		i4Ret = _jfifProcAPP0(prInstance);
		break;
	case (UINT16)M_APP1		:
		i4Ret = _jfifProcAPP1(prInstance);
		break;
	case (UINT16)M_APP14	:
		i4Ret = _jfifProcAPP14(prInstance);
		break;
	case (UINT16)M_APP2		:		// NIKON ICC_PROFILE, EXIF FPXR (FlashPix)
	case (UINT16)M_APP3		:
	case (UINT16)M_APP4		:
	case (UINT16)M_APP5		:
	case (UINT16)M_APP6		:
	case (UINT16)M_APP7		:
	case (UINT16)M_APP8		:
	case (UINT16)M_APP9		:
	case (UINT16)M_APP10	:
	case (UINT16)M_APP11	:
	case (UINT16)M_APP12	:
	case (UINT16)M_APP13	:
	case (UINT16)M_APP15	:
		// unsupport marker, just skip it
		i4Ret = _jfifProcAPPother(prInstance);
		break;
    case (UINT16)0xFF00: // Not marker, just skip it.
        ADVANCE_BYTES(2);
        i4Ret = E_JPG_OK;
        break;
	default			:
		// unknown marker, just skip it
		// due to unknown, we can not trust the length
		// hence advance through it
		//ADVANCE_BYTES(1);
		i4Ret = _jfifSkipMarker(prInstance);
		break;
	}

	return i4Ret;
}

/* process start of image */
static INT32 _jfifProcSOI			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret; /* for ADVANCE_BYTES() error return */

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->fgSOI)
	{
		return -E_JPG_ERR_MULTI_SOI;
	}

    prInstance->u4HasReadData = 0;
	prInstance->fgSOI = TRUE;
	ADVANCE_BYTES(2);

	return E_JPG_OK;
}

/* process start of frame */
static INT32 _jfifProcSOF			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT8 u1CompIndex;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->fgSOF)
	{
		return -E_JPG_ERR_MULTI_SOF;
	}

    if (prInstance->u4ValidData < prInstance->u2CurrMarkerSz + 4)
    {
        prInstance->u4NeedData = prInstance->u2CurrMarkerSz + 4;
        return -E_JPG_ERR_PASS_END;
    }

	ADVANCE_BYTES(2);	//marker

	/*
	 * do baseline or progressive check here, but due to we implement baseline decoder only,
	 * hence these code was marked out
	 */

	switch (prInstance->u2CurrMarker)
	{
	case M_SOF0		:
		prInstance->rSOF.eJpegFormat = E_JPG_BASELINE;
		break;
	case M_SOF1		:
		prInstance->rSOF.eJpegFormat = E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN;
		break;
	case M_SOF2		:
		prInstance->rSOF.eJpegFormat = E_JPG_PROGRESSIVE_HUFFMAN;
		break;
	case M_SOF3		:
		prInstance->rSOF.eJpegFormat = E_JPG_EXTENDED_SEQUENTIAL_ARITHMETIC;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF5		:
		prInstance->rSOF.eJpegFormat = E_JPG_PROGRESSIVE_ARITHMETIC;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF6		:
		prInstance->rSOF.eJpegFormat = E_JPG_LOSSLESS_HUFFMAN;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF7		:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_SEQ_HUFFMAN;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF9		:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_PROGRESSIVE_HUFFMAN;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF10	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_LOSSLESS_HUFFMAN;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF11	:
		prInstance->rSOF.eJpegFormat = E_JPG_RESERVED_FOR_EXTENSIONS;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF13	:
		prInstance->rSOF.eJpegFormat = E_JPG_LOSSLESS_ARITHMETIC;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF14	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_SEQUENTIAL_ARITHMETIC;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF15	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_PROGRESSIVE_ARITHMETIC;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	default			:
		prInstance->rSOF.eJpegFormat = E_JPG_UNSUPPORT_FORMAT;
		return -E_JPG_ERR_UNSUPPORT_SOF;
	}

	// SOF0 & SOF1 & SOF2 parsing
	if (prInstance->u2CurrMarkerSz < 8)
	{
		return -E_JPG_ERR_INVALID_SOF;
	}

	prInstance->rSOF.u1DataPrecision	= JPEG_READ_BYTE	(prInstance, 2);	// 2
	prInstance->rSOF.u2ImageHeight		= JPEG_READ_2BYTES	(prInstance, 3);	// 3,4
	prInstance->rSOF.u2ImageWidth		= JPEG_READ_2BYTES	(prInstance, 5);	// 5,6
	prInstance->rSOF.u1NumComponents	= JPEG_READ_BYTE	(prInstance, 7);	// 7

	ADVANCE_BYTES(8); //Sz+data1
	prInstance->u2CurrMarkerSz -= 8;

	// start of frame validation test
	if ((prInstance->rSOF.u2ImageHeight		== 0)	||
		(prInstance->rSOF.u2ImageWidth		== 0)	||
		(prInstance->rSOF.u1NumComponents	<  1)	||
		(prInstance->rSOF.u1NumComponents	>  3)	||
		(prInstance->u2CurrMarkerSz != prInstance->rSOF.u1NumComponents * 3))
	{
		return -E_JPG_ERR_INVALID_SOF;
	}

        //init H and V Factor, because when only 1 or 2 component, for image resizer, we need to set these component
        for (u1CompIndex = 0; u1CompIndex < MAX_JPEG_COMPONENT; u1CompIndex++)
        {
            prInstance->rSOF.arSofComp[u1CompIndex].u1HSampFactor	= 1;
            prInstance->rSOF.arSofComp[u1CompIndex].u1VSampFactor	= 1;
        }
        //--

	for (u1CompIndex = 0; u1CompIndex < prInstance->rSOF.u1NumComponents; u1CompIndex++)
	{
		UINT8 u1CompId, u1Factor, u1QuantTblNo;
		u1CompId		= JPEG_READ_BYTE(prInstance, 0);
		u1Factor		= JPEG_READ_BYTE(prInstance, 1);
		u1QuantTblNo	= JPEG_READ_BYTE(prInstance, 2);

		prInstance->rSOF.arSofComp[u1CompIndex].u1ComponentId	= u1CompId;
		prInstance->rSOF.au1MapId2Index[u1CompId] = u1CompIndex;
		prInstance->rSOF.arSofComp[u1CompIndex].u1HSampFactor	= u1Factor >> 4;
		prInstance->rSOF.arSofComp[u1CompIndex].u1VSampFactor	= u1Factor & 0xf;
		prInstance->rSOF.arSofComp[u1CompIndex].u1QuantTblNo	= u1QuantTblNo;
        prInstance->rSOF.arSofComp[u1CompIndex].u1CompWidth     = prInstance->rSOF.arSofComp[u1CompIndex].u1HSampFactor * 8;
        prInstance->rSOF.arSofComp[u1CompIndex].u2CompCofSize   = prInstance->rSOF.arSofComp[u1CompIndex].u1CompWidth * 8 * 2;

        /*Add for CQ68933*/
        if ((prInstance->rSOF.arSofComp[u1CompIndex].u1HSampFactor == 0) ||
            (prInstance->rSOF.arSofComp[u1CompIndex].u1VSampFactor == 0))
        {
            return -E_JPG_ERR_INVALID_SOF;
        }
		ADVANCE_BYTES(3);
	}

    /*Add by pingzhao ,for one component jpg*/
    if (prInstance->rSOF.u1NumComponents == 1)
    {
        prInstance->rSOF.arSofComp[0].u1HSampFactor = 1;
        prInstance->rSOF.arSofComp[0].u1VSampFactor = 1;
    }

	prInstance->fgSOF = TRUE;
	return E_JPG_OK;
}

/* process start of scan */
static INT32 _jfifProcSOS			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT8 u1CompInScan;
	UINT8 u1CompIndex;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->fgSOF == FALSE)
	{
		return -E_JPG_ERR_SOS_BEFORE_SOF;
	}

    if (prInstance->u4ValidData < prInstance->u2CurrMarkerSz + 4)
    {
        prInstance->u4NeedData = prInstance->u2CurrMarkerSz + 4;
        return -E_JPG_ERR_PASS_END;
    }

	u1CompInScan = JPEG_READ_BYTE(prInstance, 4);	// 4
	ADVANCE_BYTES(5);   //marker+sz+comp

	// sos validation
	if ((prInstance->u2CurrMarkerSz< (u1CompInScan * 2 + 6)) ||
		(u1CompInScan == 0) ||
		(u1CompInScan > MAX_JPEG_COMPONENT))
	{
		return -E_JPG_ERR_INVALID_SOS;
	}

	// store components in scan
	prInstance->rSOS.u1CompInScan = u1CompInScan;

	prInstance->u2CurrMarkerSz -= 3;	// skip length(2 bytes), comp in scan(1 byte)

	/*
	sos structure :
		2 bytes - SOS marker
		2 bytes - length
		1 byte  - num comp in scan
		n * 3 bytes - comp data
		1 byte  - ss
		1 byte  - se
		1 byte  - ah al
	*/


	// read component id and table no
	for (u1CompIndex = 0; u1CompIndex < u1CompInScan; u1CompIndex++)
	{
		UINT8 u1CompId;
		UINT8 u1TableId;
		UINT8 u1MapedIndex;

		u1CompId = JPEG_READ_BYTE(prInstance, 0);
		u1TableId = JPEG_READ_BYTE(prInstance, 1);
		ADVANCE_BYTES(2);
		prInstance->rSOS.au1CompIdx[u1CompIndex] = prInstance->rSOF.au1MapId2Index[u1CompId];

		u1MapedIndex = prInstance->rSOF.au1MapId2Index[u1CompId];
		/* following data use maped index */
        if (u1MapedIndex > (MAX_JPEG_COMPONENT - 1))
        {
            ASSERT(0);
        }
		prInstance->rSOS.au1DcId[u1MapedIndex] = u1TableId >> 4;
		prInstance->rSOS.au1AcId[u1MapedIndex] = u1TableId & 0xf;
	}

	prInstance->rSOS.u1Ss		= JPEG_READ_BYTE(prInstance, 0);
	prInstance->rSOS.u1Se		= JPEG_READ_BYTE(prInstance, 1);
	{
	    UINT8 u1AhAl = JPEG_READ_BYTE(prInstance, 2);

        //Add by pingzhao ,for work around the speical baseline jpg with Al/Ah != 0
        if ((prInstance->rSOF.eJpegFormat == E_JPG_BASELINE) ||
            (prInstance->rSOF.eJpegFormat == E_JPG_EXTENDED_SEQUENTIAL_HUFFMAN))
        {
            prInstance->rSOS.u1Ah = 0;
            prInstance->rSOS.u1Al = 0;
        }
        else
        {
	    prInstance->rSOS.u1Ah  = u1AhAl >> 4;
	    prInstance->rSOS.u1Al  = u1AhAl & 0xF;
	}
	}
	ADVANCE_BYTES(3);

    prInstance->u4BitStmSrtOfst = prInstance->u4HasReadData;

	return E_JPG_OK;
}

/* process de-huffman table */
static INT32 _jfifProcDHT			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT8 au1Bits[17];
	UINT8 au1HuffVal[256];

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

    if (prInstance->u4ValidData < prInstance->u2CurrMarkerSz + 4)
    {
        prInstance->u4NeedData = prInstance->u2CurrMarkerSz + 4;
        return -E_JPG_ERR_PASS_END;
    }

	ADVANCE_BYTES(4);   //marker+sz

	if (prInstance->u2CurrMarkerSz < (2 + 17))
	{
		return -E_JPG_ERR_INVALID_DHT;
	}

	prInstance->u2CurrMarkerSz -= 2;

	while (prInstance->u2CurrMarkerSz > 16)
	{
		UINT32 u4Bits;
		UINT32 u4Count;
		UINT32 u4HuffIndex;
		BOOL fgIsAc;
		UINT8 u1TblIndex;

		x_memset(au1Bits, 0, 17);
		x_memset(au1HuffVal, 0, 256);

		u1TblIndex = JPEG_READ_BYTE(prInstance, 0);
		fgIsAc = (BOOL) (u1TblIndex >> 4);
		//u1TblIndex &= 0xf;
		u1TblIndex &= 0x3;	// at most 4 table each, for dc and ac
		u4Count = 0;

		for (u4Bits = 1; u4Bits < 17; u4Bits++)
		{
			au1Bits[u4Bits] = JPEG_READ_BYTE(prInstance, u4Bits);
			u4Count += au1Bits[u4Bits];
		}
		ADVANCE_BYTES(17);
		prInstance->u2CurrMarkerSz -= 17;

		if (E_JPG_OK != i4Ret)
		{
			return i4Ret;
		}

		if (u4Count > 256)
		{
			//bogus dht
			u4Count = 256;
		}

		if (prInstance->u2CurrMarkerSz < u4Count)
		{
			//bogus again
			u4Count = prInstance->u2CurrMarkerSz;
		}

		// fill huff value
		for (u4HuffIndex = 0; u4HuffIndex < u4Count; u4HuffIndex++)
		{
			au1HuffVal[u4HuffIndex] = JPEG_READ_BYTE(prInstance, u4HuffIndex);
		}

		prInstance->u2CurrMarkerSz -= u4Count;
		ADVANCE_BYTES(u4Count);

		//UTIL_Printf("DHT : %s table index %d\n", fgIsAc ? "AC" : "DC", u1TblIndex);

		if (fgIsAc)
		{
		    if((prInstance->rDHT.u4AcTblLoaded & (1 << u1TblIndex)) == 0)
		    {
    			prInstance->rDHT.u4NumAcTbl++;
    			prInstance->rDHT.u4AcTblLoaded |= 1 << u1TblIndex;
	    	}
			x_memcpy(prInstance->rDHT.arAcTbl[u1TblIndex].au1HuffVal, au1HuffVal, u4Count);
			x_memcpy(prInstance->rDHT.arAcTbl[u1TblIndex].au1Bits, au1Bits, 17);
		}
		else
		{
		    if((prInstance->rDHT.u4DcTblLoaded & (1 << u1TblIndex)) == 0)
		    {
    			prInstance->rDHT.u4NumDcTbl++;
    			prInstance->rDHT.u4DcTblLoaded |= 1 << u1TblIndex;
	    	}
			x_memcpy(prInstance->rDHT.arDcTbl[u1TblIndex].au1HuffVal, au1HuffVal, u4Count);
			x_memcpy(prInstance->rDHT.arDcTbl[u1TblIndex].au1Bits, au1Bits, 17);
		}
	}

	return E_JPG_OK;
}

/* process de-quantization table */
static INT32 _jfifProcDQT			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	INT32 i4Ntbl = 0;
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

    if (prInstance->u4ValidData < prInstance->u2CurrMarkerSz + 4)
    {
        prInstance->u4NeedData = prInstance->u2CurrMarkerSz + 4;
        return -E_JPG_ERR_PASS_END;
    }

	ADVANCE_BYTES(4);   //marker+Sz

	if (prInstance->u2CurrMarkerSz < (2 + 1 + 64))
	{
		return -E_JPG_ERR_INVALID_DHT;
	}

	prInstance->u2CurrMarkerSz -= 2;

	while (prInstance->u2CurrMarkerSz > 64)
	{
		UINT8 u1Prec;
		UINT8 u1Qtbl;

		u1Prec = JPEG_READ_BYTE(prInstance, 0);
		u1Qtbl = u1Prec & 0x3;	// table index 0 .. 3
		u1Prec >>= 4;

		if (u1Prec && (prInstance->u2CurrMarkerSz < (128 + 1)))
		{
			return -E_JPG_ERR_INVALID_DHT;
		}

		//UTIL_Printf("DQT %d table index %d\n", u1Prec ? 12 : 8, u1Qtbl);

#ifdef DRV_VERIFY_SUPPORT
        x_memcpy(prInstance->rDQT.aau1Qtbl[u1Qtbl],
                 prInstance->pu1CurrRead + 1,
                 u1Prec ? 128: 64);
#else
        {
            UINT32 u4 = 0;
            for (u4=0; u4<(u1Prec?128:64); u4++)
            {
                prInstance->rDQT.aau1Qtbl[u1Qtbl][u4] = JPEG_READ_BYTE(prInstance, u4+1);
            }
        }
#endif
		prInstance->rDQT.afgPrec[u1Qtbl] = (BOOL)u1Prec;

		prInstance->u2CurrMarkerSz -= u1Prec ? (128 + 1) : (64 + 1);
		ADVANCE_BYTES(u1Prec ? 128 + 1 : 64 + 1);
		i4Ntbl++;
	}
	prInstance->rDQT.u1NumQ += i4Ntbl;
	prInstance->fgDQT = TRUE;
	return E_JPG_OK;
}

/* process restart interval marker */
static INT32 _jfifProcDRI			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

    if (prInstance->u4ValidData < prInstance->u2CurrMarkerSz + 4)
    {
        prInstance->u4NeedData = prInstance->u2CurrMarkerSz + 4;
        return -E_JPG_ERR_PASS_END;
    }
	ADVANCE_BYTES(4);   //marker+Sz

	if (prInstance->u2CurrMarkerSz < 4)
	{
		return -E_JPG_ERR_INVALID_DRI;
	}

	prInstance->u2RestartInterval = JPEG_READ_2BYTES(prInstance, 0);

	ADVANCE_BYTES(2);

	return E_JPG_OK;

}

/* process standard APP0 "JFIF" marker (and "JFXX" extension) */
static INT32 _jfifProcAPP0			(JPEG_JFIF_HEADER_T *prInstance)
{
#if 1
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);
	return (_jfifSkipMarker(prInstance));
#else
	INT32 i4Ret;
	INT32 i4Index;
	UINT16 u2Length;
	UINT8 au1Identifier[5];

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	u2Length = JPEG_READ_2BYTES(prInstance, 2);	// 2, 3
	ADVANCE_BYTES(4);	// skip marker and length

	if (u2Length < 8)
	{
		return -E_JPG_ERR_INVALID_APP0;
	}
	u2Length -= 2;

	/* identifier */
	for (i4Index = 0; i4Index < 5; i4Index++)
	{
		au1Identifier[i4Index] = JPEG_READ_BYTE(prInstance, i4Index);
	}

	ADVANCE_BYTES(5);	// skip identifier bytes
	u2Length -= 5;

	if (0 == x_memcmp(au1Identifier, "JFIF\0", 5))
	{
		// jfif
		if (u2Length < 9)
		{
			return -E_JPG_ERR_INVALID_APP0;
		}
		prInstance->arAPP0[E_APP0_JFIF].eApp0Type = E_APP0_JFIF;
		/* version */
		prInstance->arAPP0[E_APP0_JFIF].u2Version = JPEG_READ_2BYTES(prInstance, 0);
		ADVANCE_BYTES(2);
		u2Length -= 2;

		/* units */
		prInstance->arAPP0[E_APP0_JFIF].u1Units = JPEG_READ_BYTE(prInstance, 0);
		ADVANCE_BYTES(1);
		u2Length -= 1;

		/* x & y deisity */
		prInstance->arAPP0[E_APP0_JFIF].u2DensityX = JPEG_READ_2BYTES(prInstance, 0);
		prInstance->arAPP0[E_APP0_JFIF].u2DensityY = JPEG_READ_2BYTES(prInstance, 2);
		ADVANCE_BYTES(4);
		u2Length -= 4;

		/* thumbnail w & h */
		prInstance->arAPP0[E_APP0_JFIF].u1ThumbW = JPEG_READ_BYTE(prInstance, 0);
		prInstance->arAPP0[E_APP0_JFIF].u1ThumbH = JPEG_READ_BYTE(prInstance, 1);
		ADVANCE_BYTES(2);
		u2Length -= 2;

		/* basic thumbnail */
		if ((prInstance->arAPP0[E_APP0_JFIF].u1ThumbW != 0) &&
			(prInstance->arAPP0[E_APP0_JFIF].u1ThumbH != 0) &&
			(u2Length == (UINT16)prInstance->arAPP0[E_APP0_JFIF].u1ThumbW * (UINT16)prInstance->arAPP0[E_APP0_JFIF].u1ThumbH * 3))
		{
			prInstance->arAPP0[E_APP0_JFIF].fgHasThumbnail = TRUE;
			prInstance->arAPP0[E_APP0_JFIF].pu1RGBdata = prInstance->pu1CurrRead;
		}

		prInstance->arAPP0[E_APP0_JFIF].fgAPP0 = TRUE;
	}
	else if (0 == x_memcmp(au1Identifier, "JFXX\0", 5))
	{
		// jfxx
		if (u2Length < 1)
		{
			return -E_JPG_ERR_INVALID_APP0;
		}
		prInstance->arAPP0[E_APP0_JFXX].eApp0Type = E_APP0_JFXX;
		/* extension code */
		prInstance->arAPP0[E_APP0_JFXX].eThumbnailMode = (ENUM_APP0_THUMBNAIL_MODE) JPEG_READ_BYTE(prInstance, 0);
		ADVANCE_BYTES(1);
		u2Length -= 1;

		if (prInstance->arAPP0[E_APP0_JFXX].eThumbnailMode == E_THUMB_JPEG)
		{
			prInstance->arAPP0[E_APP0_JFXX].pu1JpegThumbnail = prInstance->pu1CurrRead;
		}
		else if (prInstance->arAPP0[E_APP0_JFXX].eThumbnailMode == E_THUMB_1BYTE_PIX)
		{
			if (u2Length < 2)
			{
				return -E_JPG_ERR_INVALID_APP0;
			}
			/* thumbnail w & h */
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbW = JPEG_READ_BYTE(prInstance, 0);
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbH = JPEG_READ_BYTE(prInstance, 1);
			ADVANCE_BYTES(2);
			u2Length -= 2;
			/* palettd & pixel validation */
			if ((prInstance->arAPP0[E_APP0_JFXX].u1ThumbW != 0) &&
				(prInstance->arAPP0[E_APP0_JFXX].u1ThumbH != 0) &&
				(u2Length == prInstance->arAPP0[E_APP0_JFXX].u1ThumbW * prInstance->arAPP0[E_APP0_JFXX].u1ThumbH + 768))
			{
				UINT8 u1R, u1G, u1B;
				for (i4Index = 0; i4Index < 256; i4Index++)
				{
					u1R = JPEG_READ_BYTE(prInstance, 0);
					u1G = JPEG_READ_BYTE(prInstance, 1);
					u1B = JPEG_READ_BYTE(prInstance, 2);
					ADVANCE_BYTES(3);
					u2Length -= 3;
					prInstance->arAPP0[E_APP0_JFXX].au4Palette[i4Index] =
						0xff000000 |
						(u1R << 16) |
						(u1G << 8) |
						u1B;
				}
				prInstance->arAPP0[E_APP0_JFXX].pu1ColorData = prInstance->pu1CurrRead;
				prInstance->arAPP0[E_APP0_JFXX].fgHasThumbnail = TRUE;
			}
		}
		else if (prInstance->arAPP0[E_APP0_JFXX].eThumbnailMode == E_THUMB_3BYTE_PIX)
		{
			if (u2Length < 2)
			{
				return -E_JPG_ERR_INVALID_APP0;
			}
			/* thumbnail w & h */
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbW = JPEG_READ_BYTE(prInstance, 0);
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbH = JPEG_READ_BYTE(prInstance, 1);
			ADVANCE_BYTES(2);
			u2Length -= 2;
			/* palettd & pixel validation */
			if ((prInstance->arAPP0[E_APP0_JFXX].u1ThumbW != 0) &&
				(prInstance->arAPP0[E_APP0_JFXX].u1ThumbH != 0) &&
				(u2Length == prInstance->arAPP0[E_APP0_JFXX].u1ThumbW * prInstance->arAPP0[E_APP0_JFXX].u1ThumbH * 3))
			{
				prInstance->arAPP0[E_APP0_JFXX].pu1RGBdata = prInstance->pu1CurrRead;
				prInstance->arAPP0[E_APP0_JFXX].fgHasThumbnail = TRUE;
			}
		}
		else
		{
			// no thumbnail in jfxx ...
			// TODO : try more cases
			return -E_JPG_ERR_INVALID_APP0;
		}

		prInstance->arAPP0[E_APP0_JFXX].fgAPP0 = TRUE;
	}
	else
	{
		// unsuppported tag
		// just skip it
		ADVANCE_BYTES(u2Length);
	}

	prInstance->u4NumApp0++;
	return E_JPG_OK;
#endif
}

/* process standard APP1 "Exif" marker */
static INT32 _jfifProcAPP1			(JPEG_JFIF_HEADER_T *prInstance)
{
#if 1
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);
	return (_jfifSkipMarker(prInstance));
#else
	INT32 i4Ret;
	UINT16 u2Length;

	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->rAPP1.fgAPP1 == TRUE)
	{
		return -E_JPG_ERR_MULTI_APP1;
	}

	ADVANCE_BYTES(2);
	u2Length = JPEG_READ_2BYTES(prInstance, 0);
	if (u2Length < 2)
	{
		return -E_JPG_ERR_INVALID_APP1;
	}

	prInstance->rAPP1.pu1ExifData = prInstance->pu1CurrRead;
	prInstance->rAPP1.fgAPP1 = TRUE;
	ADVANCE_BYTES(u2Length);
	return E_JPG_OK;
#endif
}

/* process APP14 */
static INT32 _jfifProcAPP14			(JPEG_JFIF_HEADER_T *prInstance)
{
	return _jfifProcAPP0(prInstance);
}

/* process all non-supported APPn format */
static INT32 _jfifProcAPPother		(JPEG_JFIF_HEADER_T *prInstance)
{
	return _jfifSkipMarker(prInstance);
}

/* process end of image marker */
static INT32 _jfifProcEOI			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	VERIFY(_jfifCheckInstance(prInstance) == E_JPG_OK);

	if (prInstance->fgEOI)
	{
		return -E_JPG_ERR_MULTI_EOI;
	}

	ADVANCE_BYTES(2);

	prInstance->fgEOI = TRUE;

	return E_JPG_OK;
}



/* export interfaces */
// get sizeof instance
JPEGHANDLE JFIF_AllocInstance(void)
{
	JPEGHANDLE hInstance;

	// allocate jfif header instance
	#if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    hInstance = (JPEGHANDLE)x_alloc_aligned_dma_mem(sizeof(JPEG_JFIF_HEADER_T), 128);
#elif CONFIG_SYS_MEM_PHASE3
    hInstance = (JPEGHANDLE)x_mem_aligned_alloc(sizeof(JPEG_JFIF_HEADER_T), 128);
#endif
    #else
	hInstance = (JPEGHANDLE)x_alloc_aligned_nc_mem(sizeof(JPEG_JFIF_HEADER_T), 128);
    #endif
	if (hInstance == NULL)
	{
		return hInstance;
	}
	x_memset(hInstance, 0, sizeof(JPEG_JFIF_HEADER_T));

	return hInstance;
}

void JFIF_FreeInstance(JPEGHANDLE hInstance)
{
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
#if CONFIG_SYS_MEM_PHASE2
    x_free_aligned_dma_mem(hInstance);
#elif CONFIG_SYS_MEM_PHASE3
    x_mem_aligned_free(hInstance);
#endif
    //x_free_vmem(hInstance);
    #else
	x_free_aligned_nc_mem(hInstance);
    #endif
}


// start and parse a jpeg file image store in memory
INT32 JFIF_New(JPEGHANDLE hInstance, void *pvJpegImage, UINT32 u4JpegSize)
{
	INT32 i4Ret;
	JPEG_JFIF_HEADER_T *prInstance;

	prInstance = (JPEG_JFIF_HEADER_T*)hInstance;

	if (prInstance == NULL)
	{
		return -E_JPG_ERR_PARAM;
	}

	prInstance->pu1CurrRead = (UINT8*)pvJpegImage;
	prInstance->u4ValidData = u4JpegSize;

	i4Ret = _jfifParse(prInstance);
	if (E_JPG_OK != i4Ret)
	{
		return i4Ret;
	}

	if (prInstance->fgPrsResult)
	{
		return E_JPG_OK;
	}
	else
	{
		return -E_JPG_ERR_PARSE_FAILED;
	}
}

