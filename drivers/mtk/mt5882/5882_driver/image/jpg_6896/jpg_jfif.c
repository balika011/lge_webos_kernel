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

#include "jpg_common.h"
#include "jpg_if.h"
#include "jpg_jfif.h"
#include "jpg_debug.h"
#include "img_lib_if.h"
#ifdef JPEG_VLD_TO_PARSE_HEADER
#include "jpg_hw.h"
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/
#include "x_bim.h"
#define ADVANCE_BYTES(NBYTES) \
	/* this while 0 is safe */ \
	LINT_SUPPRESS_NEXT_STATEMENT(717) \
	do \
	{ \
		i4Ret = _jfifAdvanceBytes(prInstance, NBYTES); \
		if ((INT32)E_JPG_OK != i4Ret) \
		{ \
			return i4Ret; \
		} \
	} while (0)
#ifdef JPEG_VLD_TO_PARSE_HEADER
#define JPEG_READ_BYTE(PINSTANCE, NTH_BYTE)   VLDJPG_BarrelShifterPeekByte(NTH_BYTE)
#define JPEG_READ_2BYTES(PINSTANCE, NTH_BYTE) ((JPEG_READ_BYTE(PINSTANCE, NTH_BYTE) << 8) | JPEG_READ_BYTE(PINSTANCE, (NTH_BYTE) + 1))
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
#define JPEG_READ_BYTE(PINSTANCE, NTH_BYTE) (PINSTANCE->pu1CurrRead[NTH_BYTE])
#define JPEG_READ_2BYTES(PINSTANCE, NTH_BYTE) ((JPEG_READ_BYTE(PINSTANCE, NTH_BYTE) << 8) | JPEG_READ_BYTE(PINSTANCE, (NTH_BYTE) + 1))
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/


/* private interfaces */
//static UINT8  _jfifReadByte			(JPEG_JFIF_HEADER_T *prInstance);
	/* read 1 byte from jpeg stream */
//static UINT16 _jfifRead2Bytes		(JPEG_JFIF_HEADER_T *prInstance);
	/* read 2 bytes from jpeg stream */
//static INT32  _jfifSkipBytes		(JPEG_JFIF_HEADER_T *prInstance, INT32 i4Bytes);
	/* skip i4Bytes bytes in jpeg stream */

/* private interfaces */
static INT32 _jfifAdvanceBytes		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Bytes);
	/* advance read pointer n bytes */
static INT32 _jfifCheckInstance		(const JPEG_JFIF_HEADER_T *prInstance);
	/* check instance */
static INT32 _jfifParse				(JPEG_JFIF_HEADER_T *prInstance);
	/* parse all marker in jpeg file */
static INT32 _jfifParseScan			(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Scan);
	/* parse all marker in jpeg file */
static INT32 _jfifIsMarker			(const JPEG_JFIF_HEADER_T *prInstance);
	/* check if current read pointer points to an valid marker */
static INT32 _jfifPeekMarker		(JPEG_JFIF_HEADER_T *prInstance, UINT16 *pu2Marker);
	/* get next marker (read pointer stops on this marker) */
static INT32 _jfifSkipMarker		(JPEG_JFIF_HEADER_T *prInstance);
	/* skip current marker */
static INT32 _jfifDispatchMarker	(JPEG_JFIF_HEADER_T *prInstance, UINT16 u2Marker);
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
#ifdef CC_FUJI3D_SUPPORT
static INT32 _jfifProcAPP2			(JPEG_JFIF_HEADER_T *prInstance);
	/* process standard APP2 "Exif" marker */
#endif
static INT32 _jfifProcAPP14			(JPEG_JFIF_HEADER_T *prInstance);
	/* process APP14 */
static INT32 _jfifProcAPPother		(JPEG_JFIF_HEADER_T *prInstance);
	/* process all non-supported APPn format */
static INT32 _jfifProcEOI			(JPEG_JFIF_HEADER_T *prInstance);
	/* process end of image marker */



//----------------------------------------------------------

/* private interfaces */

static INT32 _jfifAdvanceBytes      (JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Bytes)
{
    UINT32 ReadP =0, WriteP = 0;
    ASSERT(prInstance != NULL);
    VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

#ifdef JPEG_VLD_TO_PARSE_HEADER
{

    ReadP = VLDJPG_ReadPointer();
    WriteP = VLDJPG_WritePointer();
	ReadP = ReadP - PHYSICAL((UINT32) prInstance->pu1RingBufStart) + (UINT32)prInstance->pu1RingBufStart;
	WriteP =WriteP - PHYSICAL((UINT32) prInstance->pu1RingBufStart)+ (UINT32)prInstance->pu1RingBufStart;
}
    if (((ReadP<WriteP)&&(ReadP+u4Bytes>=WriteP)) || \
        ((ReadP>WriteP)&&(prInstance->u4ImageByteSize-(ReadP-WriteP))<=u4Bytes))
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
    UNUSED(WriteP);
    ReadP = (UINT32)prInstance->pu1CurrRead;
    if (ReadP + u4Bytes >=((UINT32)&prInstance->pu1JpegImage[prInstance->u4ImageByteSize]))
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/
	{
        if (prInstance->fgEOF)
	    {
    		return -(INT32)E_JPG_ERR_PASS_END;
    	}
	    else                                    //not EOF, need more data for parsing
	    {
	        UINT32 u4Remind;
	        UINT32 u4Used;
            UINT32 u4TempRP;
	        u4Remind = (!prInstance->u4RingRP) ? (UINT32)0 : \
	                    (UINT32)(prInstance->pu1RingBufStart + prInstance->u4RingBufSize - ReadP);
	        u4Used = (UINT32)ReadP - (UINT32)prInstance->pu1RingBufStart + u4Remind;
			u4TempRP = (UINT32)ReadP - (UINT32)prInstance->pu1RingBufStart;
			if(u4TempRP <  prInstance->u4RingRP)
			{
			   u4Used = prInstance->u4RingBufSize + u4TempRP - prInstance->u4RingRP;
			}
			prInstance->u4RingRP = u4TempRP;
	        prInstance->u4RingData -= u4Used;
    	    IMG_BsBufManager(prInstance->ucImgId);    	    
        #ifdef JPEG_VLD_TO_PARSE_HEADER
            VERIFY(VLDJPG_SetRingBuffer(prInstance->pu1JpegImage, prInstance->u4ImageByteSize) == (INT32)E_JPG_OK);
            VLDJPG_BarrelShifterSkipBytes(u4Bytes);
        #else   /*JPEG_VLD_TO_PARSE_HEADER*/
            prInstance->pu1CurrRead = (prInstance->pu1CurrRead + u4Bytes) - prInstance->u4RingBufSize;
        #endif  /*JPEG_VLD_TO_PARSE_HEADER*/
	    }
	}
    else
    {
#ifdef JPEG_VLD_TO_PARSE_HEADER
        return VLDJPG_BarrelShifterSkipBytes(u4Bytes);
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
    	prInstance->pu1CurrRead += u4Bytes;
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/
    }
	return (INT32)E_JPG_OK;
}

/* instance check */
static INT32 _jfifCheckInstance		(const JPEG_JFIF_HEADER_T *prInstance)
{
	if (prInstance == NULL)
	{
		// null instance
		return -(INT32)E_JPG_ERR_PARAM;
	}

	if ((prInstance->rParent.eJpegClass		!= E_JC_JFIF_HEADER) ||
		(prInstance->rParent.zObjectSize	!= sizeof(JPEG_JFIF_HEADER_T)))
	{
		// not an jfif header instance
		return -(INT32)E_JPG_ERR_PARAM;
	}

	if (!prInstance->fgInited)
	{
		// didnt init
		return -(INT32)E_JPG_ERR_NOT_INITED;
	}

	if (prInstance->fgFatalError)
	{
		// an fatal error happened
		return -(INT32)E_JPG_ERR_FATAL;
	}

	return (INT32)E_JPG_OK;
}

/* parse header markers in jpeg file */
static INT32 _jfifParse				(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2JfifMarker;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->fgParsed)
	{
		// already parsed
		return -(INT32)E_JPG_ERR_ALREADY;
	}

	// parse until EOI
	for(;;)
	{
		i4Ret = _jfifPeekMarker(prInstance, &u2JfifMarker);
		if ((INT32)E_JPG_OK == i4Ret)
		{
			// marker found
			i4Ret = _jfifDispatchMarker(prInstance, u2JfifMarker);

			if (((UINT16)M_EOI == u2JfifMarker) ||
				((UINT16)M_SOS == u2JfifMarker))
			{
				// stop condition reached - EOI
				if (//((prInstance->u4NumApp0 != 0) || (prInstance->rAPP1.fgAPP1)) &&
					//(prInstance->rDQT.fgDQT) &&
					/*(prInstance->rDHT.fgDcTblLoaded) &&
					(prInstance->rDHT.fgAcTblLoaded || (prInstance->rSOF.eJpegFormat != E_JPG_BASELINE)) &&*/
					(prInstance->rSOF.fgSOF) &&
					(prInstance->fgSOI) &&
					(prInstance->u4NumScan > 0) &&
					(!prInstance->fgFatalError))
				{
					prInstance->fgParsed = TRUE;
				}
				else
				{
					prInstance->fgParsed = FALSE;
				}
				break;
			}

			if ((INT32)E_JPG_OK != i4Ret)
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


/* parse markers before a SOS */
static INT32 _jfifParseScan		(JPEG_JFIF_HEADER_T *prInstance, UINT32 u4Scan)
{
	INT32 i4Ret = (INT32)E_JPG_OK;
	UINT16 u2JfifMarker;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	// parse until EOI
	for(;;)
	{
		i4Ret = _jfifPeekMarker(prInstance, &u2JfifMarker);
		if ((INT32)E_JPG_OK == i4Ret)
		{
			// parse marker
			// currently M_DQT only, maybe (u2JfifMarker == (UINT16)M_DHT)
			if ((UINT16)M_DQT == u2JfifMarker)
			{
				Printf(" - Found DQT\n");
				i4Ret = _jfifDispatchMarker(prInstance, u2JfifMarker);
			}
			else if ((UINT16)M_DHT == u2JfifMarker)
			{
				Printf(" - Found DHT\n");
				i4Ret = _jfifDispatchMarker(prInstance, u2JfifMarker);
			}
			else if ((UINT16)M_SOS == u2JfifMarker)
			{
				i4Ret = (INT32)E_JPG_OK;
				break;
			}
			else if ((UINT16)M_EOI != u2JfifMarker)
			{
				//Printf("Marker other than DQT DHT in scan : scan(%d) marker(0x%04x)\n",
				//	u4Scan, u2JfifMarker);
                          ADVANCE_BYTES(1); 
				i4Ret = _jfifSkipMarker(prInstance);
			}
			else	// EOI
			{
				Printf("EOI reached ... should not happen here. scan(%d)\n", u4Scan);
				i4Ret = (INT32)E_JPG_OK;
				break;
			}

			if ((INT32)E_JPG_OK != i4Ret)
			{
				// dispatch error
				break;
			}
		}
		else
		{
			break;
		}
	}

	return (INT32)i4Ret;
}



/* check if current read pointer points to an valid marker */
static INT32 _jfifIsMarker			(const JPEG_JFIF_HEADER_T *prInstance)
{
    UINT8 b0 = 0, b1 = 0;
	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

#ifdef JPEG_VLD_TO_PARSE_HEADER
    b0 = VLDJPG_BarrelShifterPeekByte(0);
    b1 = VLDJPG_BarrelShifterPeekByte(1);
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
    b0 = *prInstance->pu1CurrRead;
    b1 = *(prInstance->pu1CurrRead+1);
    if ((UINT32)prInstance->pu1CurrRead >= ((UINT32)prInstance->pu1JpegImage + prInstance->u4ImageByteSize))
    {
        return -(INT32)E_JPG_ERR_PASS_END;
    }
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/
   
    if ((b0 == 0xff) && (b1 != 0xFF))
    {
        if(prInstance->fgSOI==FALSE)
        {
            //skip bytes before M_SOI    
            if ((b0 == 0xff)&&((b1 == 0xd8)))   
            {
                LOG(2, "valid marker found:0x%x\n", b0<<8|b1);
                return (INT32)E_JPG_OK;
            }                
            else
            {
                //LOG(0, "Marker[%x][%x]", *prInstance->pu1CurrRead, *(prInstance->pu1CurrRead+1));                    
                return -(INT32)E_JPG_ERR_NOT_A_MARKER;     
            }                
        }
        LOG(7, "valid marker found:0x%x\n", b0<<8|b1);
        return (INT32)E_JPG_OK;
    }
	LOG(7, "Invalid marker found:0x%x\n", b0<<8|b1);	
	return -(INT32)E_JPG_ERR_NOT_A_MARKER;
}

/* get next marker (read pointer stops on this marker) */
static INT32 _jfifPeekMarker		(JPEG_JFIF_HEADER_T *prInstance, UINT16 *pu2Marker)
{
	INT32 i4Ret;
	UINT16 u2Marker;
    static UINT32 i =0;
	ASSERT(pu2Marker);
	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	while ((i4Ret = _jfifIsMarker(prInstance)) == -(INT32)E_JPG_ERR_NOT_A_MARKER)
	{
		ADVANCE_BYTES(1);
		/*
		i4Ret = _jfifAdvanceBytes(prIntsance, 1);
		if ((INT32)E_JPG_OK != i4Ret)
		{
			break;
		}
		*/
		LOG(1," _jfifPeekMarker.....  \n");
		i++;
		if (!(i%10000))	
		{
		    LOG(0," jpeg parser error....return  \n");
			i=0;
			return -E_JPG_ERR_UNSUPPORT_SOF;
		}
		if(!(i%300))
		{
		   x_thread_delay(1);	
           continue;
		}
	}

	if ((INT32)E_JPG_OK != i4Ret)
	{
		return i4Ret;
	}
	u2Marker = (JPEG_READ_BYTE(prInstance, 0) << 8) + JPEG_READ_BYTE(prInstance, 1);
	// exception handling
	switch (u2Marker)
	{
	// we can process some special case here
	default:
		break;
	}
	*pu2Marker = u2Marker;

	return (INT32)E_JPG_OK;
}

/* skip current marker */
static INT32 _jfifSkipMarker		(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Length;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (_jfifIsMarker(prInstance))
	{
		// although this is not a marker
		// skip marker should return without error
		// due to its function : 'skip marker'
		return (INT32)E_JPG_OK;
	}

	// this is a marker, advance 2 bytes first
	ADVANCE_BYTES(2);

	u2Length = JPEG_READ_2BYTES(prInstance, 0);
	ADVANCE_BYTES(u2Length);

	return (INT32)E_JPG_OK;
}

/* dispatch this marker to proper handler routine */
static INT32 _jfifDispatchMarker	(JPEG_JFIF_HEADER_T *prInstance, UINT16 u2Marker)
{
	INT32 i4Ret;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);
    LOG(7, "_jfifDispatchMarker	0x%x\n", u2Marker);
	switch (u2Marker)
	{
	case M_SOI		:		// start of image
		i4Ret = _jfifProcSOI(prInstance);
		break;
	case M_SOF0		:		// baseline
	case M_SOF1		:		// extended sequential huffman
	case M_SOF2		:		// progressive huffman
	case M_SOF3		:		// lossless huffman
	case M_SOF5		:		// differential sequential huffman
	case M_SOF6		:		// differential progressive huffman
	case M_SOF7		:		// differential lossless huffman
	case M_JPG		:		// reserved jpeg extension
	case M_SOF9		:		// extended sequential arithmetic
	case M_SOF10	:		// progressive arithmetic
	case M_SOF11	:		// lossless arithmetic
	case M_SOF13	:		// differential sequential arithmetic
	case M_SOF14	:		// differential progressive arithmetic
	case M_SOF15	:		// differential lossless arithmetic
		i4Ret = _jfifProcSOF(prInstance);
		if ((INT32)E_JPG_OK != i4Ret)
		{
			//prInstance->rSOF.eJpegFormat	= E_JPG_UNSUPPORT_FORMAT;
			prInstance->fgFatalError		= TRUE;
		}
		break;
	case M_SOS		:		// start of scan
		i4Ret = _jfifProcSOS(prInstance);
		break;
	case M_EOI		:		// end of image
		i4Ret = _jfifProcEOI(prInstance);
		break;
	case M_DHT		:		// de-huffman table
		i4Ret = _jfifProcDHT(prInstance);
		break;
	case M_DQT		:		// de-quantization table
		i4Ret = _jfifProcDQT(prInstance);
		break;
	case M_DRI		:		// restart interval marker
		i4Ret = _jfifProcDRI(prInstance);
		break;
	case M_APP0		:
		i4Ret = _jfifProcAPP0(prInstance);
		break;
	case M_APP1		:
		i4Ret = _jfifProcAPP1(prInstance);
		break;
	case M_APP14	:
		i4Ret = _jfifProcAPP14(prInstance);
		break;
	case M_APP2		:		// NIKON ICC_PROFILE, EXIF FPXR (FlashPix)
#ifdef CC_FUJI3D_SUPPORT	
              if(prInstance->fgChkMPO)
              {
		    i4Ret = _jfifProcAPP2(prInstance);
                  break;
              }
#endif
	case M_APP3		:
	case M_APP4		:
	case M_APP5		:
	case M_APP6		:
	case M_APP7		:
	case M_APP8		:
	case M_APP9		:
	case M_APP10	:
	case M_APP11	:
	case M_APP12	:
	case M_APP13	:
	case M_APP15	:
	case M_COM	    :
		// unsupport marker, just skip it
		i4Ret = _jfifProcAPPother(prInstance);
		// already skiped in _jfifProcAPPother()
		// i4Ret = _jfifSkipMarker(prInstance);
		break;
	default			:
		// unknown marker, just skip it
		// due to unknown, we can not trust the length
		// hence advance through it
		ADVANCE_BYTES(1);
		i4Ret = _jfifSkipMarker(prInstance);
		break;
	}

	return i4Ret;
}

/* process start of image */
static INT32 _jfifProcSOI			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret; /* for ADVANCE_BYTES() error return */

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->fgSOI)
	{
		return -(INT32)E_JPG_ERR_MULTI_SOI;
	}

	prInstance->fgSOI = TRUE;
	ADVANCE_BYTES(2);

	return (INT32)E_JPG_OK;
}

#ifdef JPEG_AUTO_TEST_PERFORMANCE
UINT32 mh_width;
UINT32 mh_height;
#endif

/* process start of frame */
static INT32 _jfifProcSOF			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Marker;
	UINT16 u2Length;
	UINT8 u1CompIndex;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->rSOF.fgSOF)
	{
		return -(INT32)E_JPG_ERR_MULTI_SOF;
	}


	u2Marker = JPEG_READ_2BYTES(prInstance, 0);	// 0,1
	u2Length = JPEG_READ_2BYTES(prInstance, 2); // 2,3
	ADVANCE_BYTES(2);	// skip marker length (does not count)

	/*
	 * do baseline or progressive check here, but due to we implement baseline decoder only,
	 * hence these code was marked out
	 */

	switch (u2Marker)
	{
	case M_SOF0		:
		prInstance->rSOF.eJpegFormat = E_JPG_BASELINE;
		break;
	case M_SOF1		:
		prInstance->rSOF.eJpegFormat = E_JPG_EX_SEQ_HUFFMAN;
		break;
	case M_SOF2		:
		prInstance->rSOF.eJpegFormat = E_JPG_PROGRESSIVE_HUFFMAN;
		break;
		//return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF3		:
		prInstance->rSOF.eJpegFormat = E_JPG_EX_SEQ_ARITHMETIC;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF5		:
		prInstance->rSOF.eJpegFormat = E_JPG_PROGRESSIVE_ARITHMETIC;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF6		:
		prInstance->rSOF.eJpegFormat = E_JPG_LOSSLESS_HUFFMAN;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF7		:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFFERENTIAL_SEQ_HUFFMAN;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF9		:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFF_PROG_HUFFMAN;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF10	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFF_LLESS_HUFFMAN;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF11	:
		prInstance->rSOF.eJpegFormat = E_JPG_RESERVED_FOR_EXTENSIONS;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF13	:
		prInstance->rSOF.eJpegFormat = E_JPG_LOSSLESS_ARITHMETIC;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF14	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFF_SEQ_ARITHMETIC;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	case M_SOF15	:
		prInstance->rSOF.eJpegFormat = E_JPG_DIFF_PROG_ARITHMETIC;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	default			:
		prInstance->rSOF.eJpegFormat = E_JPG_UNSUPPORT_FORMAT;
		return -(INT32)E_JPG_ERR_UNSUPPORT_SOF;
	}

	// SOF0 & SOF1 & SOF2 parsing
	if (u2Length < 8)
	{
		return -(INT32)E_JPG_ERR_INVALID_SOF;
	}

	ADVANCE_BYTES(2);
	prInstance->rSOF.u1DataPrecision	= JPEG_READ_BYTE	(prInstance, 0);	// 2
	prInstance->rSOF.u2ImageHeight		= JPEG_READ_2BYTES	(prInstance, 1);	// 3,4
	ADVANCE_BYTES(3);
	prInstance->rSOF.u2ImageWidth		= JPEG_READ_2BYTES	(prInstance, 0);	// 5,6
	prInstance->rSOF.u1NumComponents	= JPEG_READ_BYTE	(prInstance, 2);	// 7
	ADVANCE_BYTES(3);

    #ifdef JPEG_AUTO_TEST_PERFORMANCE
    mh_height = prInstance->rSOF.u2ImageHeight;
    mh_width = prInstance->rSOF.u2ImageWidth;
    #endif

	u2Length -= 8;

	//code sync
	if ((prInstance->rSOF.u1NumComponents	<  1)||(prInstance->rSOF.u1NumComponents	>  3))
	{
		return (INT32)E_JPG_ERR_INVALID_COMPONENT;
	}
		
	// start of frame validation test
	if ((prInstance->rSOF.u2ImageHeight		== 0)	||
		(prInstance->rSOF.u2ImageWidth		== 0)	||
		(u2Length != ((UINT16)(prInstance->rSOF.u1NumComponents * 3))))
	{
		return -(INT32)E_JPG_ERR_INVALID_SOF;
	}

	for (u1CompIndex = 0; u1CompIndex < prInstance->rSOF.u1NumComponents; u1CompIndex++)
	{
		UINT8 u1CompId, u1Factor, u1QuantTblNo;
		u1CompId		= JPEG_READ_BYTE(prInstance, 0);
		u1Factor		= JPEG_READ_BYTE(prInstance, 1);
		u1QuantTblNo	= JPEG_READ_BYTE(prInstance, 2);

		prInstance->rSOF.arSofComp[u1CompIndex].u1ComponentId	= u1CompId;
		prInstance->rSOF.au1MapId2Index[u1CompId] = u1CompIndex;
	        if(1 == prInstance->rSOF.u1NumComponents)
	        {
	            u1Factor = 0x11;
	        }
		prInstance->rSOF.arSofComp[u1CompIndex].u1HSampFactor	= u1Factor >> 4;
		prInstance->rSOF.arSofComp[u1CompIndex].u1VSampFactor	= u1Factor & 0xf;
		prInstance->rSOF.arSofComp[u1CompIndex].u1QuantTblNo	= u1QuantTblNo;

		ADVANCE_BYTES(3);
	}

	prInstance->rSOF.fgSOF = TRUE;
	return (INT32)E_JPG_OK;
}

/* process start of scan */
static INT32 _jfifProcSOS			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Length;
	UINT8 u1CompInScan;
	UINT8 u1CompIndex;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (!prInstance->rSOF.fgSOF)
	{
		return -(INT32)E_JPG_ERR_SOS_BEFORE_SOF;
	}
	ADVANCE_BYTES(2);
	u2Length = JPEG_READ_2BYTES(prInstance, 0); // 2,3
	u1CompInScan = JPEG_READ_BYTE(prInstance, 2);	// 4
	ADVANCE_BYTES(3);

	// sos validation
	if ((u2Length < (UINT16)((u1CompInScan * 2) + 6)) ||
		(u1CompInScan == 0) ||
		(u1CompInScan > D_MAX_JPEG_COMPONENT))
	{
		return -(INT32)E_JPG_ERR_INVALID_SOS;
	}

	// store components in scan
	prInstance->arSOS[prInstance->u4CurrScan].u1CompInScan = u1CompInScan;

	u2Length -= 3;	// skip length(2 bytes), comp in scan(1 byte)

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
	if (prInstance->u4CurrScan != 0)
	{
		x_memcpy(
			&prInstance->arSOS[prInstance->u4CurrScan + 1],
			&prInstance->arSOS[prInstance->u4CurrScan],
			sizeof(JPEG_JFIF_SOS_T));
	}

	prInstance->arSOS[prInstance->u4CurrScan].u4ScanPass = prInstance->u4CurrScan;

	// read component id and table no
	for (u1CompIndex = 0; u1CompIndex < u1CompInScan; u1CompIndex++)
	{
		UINT8 u1CompId;
		UINT8 u1TableId;
		UINT8 u1MapedIndex;
		u1CompId = JPEG_READ_BYTE(prInstance, 0);
		u1TableId = JPEG_READ_BYTE(prInstance, 1);
		ADVANCE_BYTES(2);
		prInstance->arSOS[prInstance->u4CurrScan].au1CompId[u1CompIndex] =
			prInstance->rSOF.au1MapId2Index[u1CompId];

		u1MapedIndex = prInstance->rSOF.au1MapId2Index[u1CompId];
		if (u1MapedIndex >= (UINT8)4)
		{
		    return -(INT32)E_JPG_ERR_INVALID_SOS;
		}
		/* following data use maped index */
		prInstance->arSOS[prInstance->u4CurrScan].au1DcId[u1MapedIndex] = u1TableId >> 4;
		prInstance->arSOS[prInstance->u4CurrScan].au1AcId[u1MapedIndex] = u1TableId & 0xf;
		prInstance->au4CompLastScan[u1MapedIndex] = prInstance->u4CurrScan;
	}

	prInstance->arSOS[prInstance->u4CurrScan].u1Ss		= JPEG_READ_BYTE(prInstance, 0);
	prInstance->arSOS[prInstance->u4CurrScan].u1Se		= JPEG_READ_BYTE(prInstance, 1);
	prInstance->arSOS[prInstance->u4CurrScan].u1AhAl	= JPEG_READ_BYTE(prInstance, 2);
	ADVANCE_BYTES(3);

	prInstance->arSOS[prInstance->u4CurrScan].pu1ScanDataStart = prInstance->pu1CurrRead;
	prInstance->u4CurrScan++;
	prInstance->u4NumScan++;
	return (INT32)E_JPG_OK;
}

/* process de-huffman table */
static INT32 _jfifProcDHT			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret, i;
	UINT16 u2Length;
	UINT8 au1Bits[17];
	UINT8 au1HuffVal[256];

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	for(i=0;i<sizeof(au1HuffVal); i++)
	{
		au1HuffVal[i]=0;
	}
	u2Length = JPEG_READ_2BYTES(prInstance, 2); // 2,3
	ADVANCE_BYTES(4);

	if (u2Length < (2 + 17))
	{
		return -(INT32)E_JPG_ERR_INVALID_DHT;
	}

	u2Length -= 2;

	while (u2Length > 16)
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

        ADVANCE_BYTES(1);
		for (u4Bits = 1; u4Bits < 17; u4Bits++)
		{
			au1Bits[u4Bits] = JPEG_READ_BYTE(prInstance, 0);
            ADVANCE_BYTES(1);
			u4Count += au1Bits[u4Bits];
		}
		u2Length -= 17;

		/*
		if ((INT32)E_JPG_OK != i4Ret)
		{
			return i4Ret;
		}
		*/

		if (u4Count > 256)
		{
			//bogus dht
			u4Count = 256;
		}

		if ((UINT32)u2Length < u4Count)
		{
			//bogus again
			u4Count = u2Length;
		}

		// fill huff value
		for (u4HuffIndex = 0; u4HuffIndex < u4Count; u4HuffIndex++)
		{
			au1HuffVal[u4HuffIndex] = JPEG_READ_BYTE(prInstance, 0);
    		ADVANCE_BYTES(1);
		}

		u2Length -= u4Count;

		//Printf("DHT : %s table index %d\n", fgIsAc ? "AC" : "DC", u1TblIndex);

		if (fgIsAc)
		{
		    if((prInstance->rDHT.fgAcTblLoaded & (1 << u1TblIndex)) == 0)
		    {
			prInstance->rDHT.u4NumAcTbl++;
			prInstance->rDHT.fgAcTblLoaded |= 1 << u1TblIndex;
	    	}
			x_memcpy(prInstance->rDHT.arAcTbl[u1TblIndex].au1HuffVal, au1HuffVal, u4Count);
			x_memcpy(prInstance->rDHT.arAcTbl[u1TblIndex].au1Bits, au1Bits, 17);
		}
		else
		{
		    if((prInstance->rDHT.fgDcTblLoaded & (1 << u1TblIndex)) == 0)
		    {
			prInstance->rDHT.u4NumDcTbl++;
			prInstance->rDHT.fgDcTblLoaded |= 1 << u1TblIndex;
	    	}
			x_memcpy(prInstance->rDHT.arDcTbl[u1TblIndex].au1HuffVal, au1HuffVal, u4Count);
			x_memcpy(prInstance->rDHT.arDcTbl[u1TblIndex].au1Bits, au1Bits, 17);
		}
	}

	return (INT32)E_JPG_OK;
}

/* process de-quantization table */
static INT32 _jfifProcDQT			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Length;
	INT32 i4Ntbl = 0;
	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	u2Length = JPEG_READ_2BYTES(prInstance, 2); // 2,3
	ADVANCE_BYTES(4);

	if (u2Length < (2 + 1 + 64))
	{
		return -(INT32)E_JPG_ERR_INVALID_DHT;
	}

	u2Length -= 2;

	while (u2Length > 64)
	{
		UINT8 u1Prec;
		UINT8 u1Qtbl;

		u1Prec = JPEG_READ_BYTE(prInstance, 0);
        ADVANCE_BYTES(1);
		u1Qtbl = u1Prec & 0x3;	// table index 0 .. 3
		u1Prec >>= 4;

		if (u1Prec && (u2Length < (128 + 1)))
		{
			return -(INT32)E_JPG_ERR_INVALID_DHT;
		}

		//LOG(3,"DQT %d table index %d\n", u1Prec ? 12 : 8, u1Qtbl);
#ifdef JPEG_VLD_TO_PARSE_HEADER
{
        UINT8 qIdx = 0;
        for(qIdx=0; qIdx < (u1Prec ? (SIZE_T)128: (SIZE_T)64); qIdx++)
        {
            prInstance->rDQT.aau1Qtbl[u1Qtbl][qIdx] = JPEG_READ_BYTE(prInstance, 0);
            ADVANCE_BYTES(1);
        }
}
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
        x_memcpy(prInstance->rDQT.aau1Qtbl[u1Qtbl],
            prInstance->pu1CurrRead,
            u1Prec ? (SIZE_T)128: (SIZE_T)64);
        ADVANCE_BYTES(u1Prec ? (UINT32)(128) : (UINT32)(64));
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/

		prInstance->rDQT.afgPrec[u1Qtbl] = (BOOL)u1Prec;
		u2Length -= u1Prec ? (128 + 1) : (64 + 1);
		i4Ntbl++;
	}
	prInstance->rDQT.u1NumQ += i4Ntbl;
	prInstance->rDQT.fgDQT = TRUE;
	return (INT32)E_JPG_OK;
}

/* process restart interval marker */
static INT32 _jfifProcDRI			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Length;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	u2Length = JPEG_READ_2BYTES(prInstance, 2); // 2,3
	ADVANCE_BYTES(4);

	if (u2Length < 4)
	{
		return -(INT32)E_JPG_ERR_INVALID_DRI;
	}

	prInstance->u2RestartInterval = JPEG_READ_2BYTES(prInstance, 0);

	ADVANCE_BYTES(2);

	return (INT32)E_JPG_OK;

}

/* process standard APP0 "JFIF" marker (and "JFXX" extension) */
static INT32 _jfifProcAPP0			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	INT32 i4Index;
	UINT16 u2Length;
	UINT8 au1Identifier[5];

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	u2Length = JPEG_READ_2BYTES(prInstance, 2);	//2, 3
	ADVANCE_BYTES(4);	// skip marker and length

        if (u2Length ==4)
        {
            //special handling
            ADVANCE_BYTES(2);	// skip marker and length
            goto APP0_ERROR_HANDLING;
        }

	if (u2Length < 8)
	{
		return -(INT32)E_JPG_ERR_INVALID_APP0;
	}
	u2Length -= 2;

	/* identifier */
	for (i4Index = 0; i4Index < 5; i4Index++)
	{
		au1Identifier[i4Index] = JPEG_READ_BYTE(prInstance, 0);
    	ADVANCE_BYTES(1);	// skip identifier bytes        
	}
	u2Length -= 5;
	prInstance->ucPicStruct = 3;
	if (0 == x_memcmp(au1Identifier, "JFIF\0", 5))
	{
		// jfif
		if (u2Length < 9)
		{
			return -(INT32)E_JPG_ERR_INVALID_APP0;
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
			(u2Length == (UINT16)((UINT16)prInstance->arAPP0[E_APP0_JFIF].u1ThumbW * (UINT16)prInstance->arAPP0[E_APP0_JFIF].u1ThumbH * 3)))
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
			return -(INT32)E_JPG_ERR_INVALID_APP0;
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
				return -(INT32)E_JPG_ERR_INVALID_APP0;
			}
			/* thumbnail w & h */
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbW = JPEG_READ_BYTE(prInstance, 0);
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbH = JPEG_READ_BYTE(prInstance, 1);
			ADVANCE_BYTES(2);
			u2Length -= 2;
			/* palettd & pixel validation */
			if ((prInstance->arAPP0[E_APP0_JFXX].u1ThumbW != 0) &&
				(prInstance->arAPP0[E_APP0_JFXX].u1ThumbH != 0) &&
				(u2Length == (UINT16)(((UINT16)prInstance->arAPP0[E_APP0_JFXX].u1ThumbW * (UINT16)prInstance->arAPP0[E_APP0_JFXX].u1ThumbH) + 768)))
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
						(UINT32)(0xff000000 |
						(u1R << 16) |
						(u1G << 8) |
						u1B);
				}
				prInstance->arAPP0[E_APP0_JFXX].pu1ColorData = prInstance->pu1CurrRead;
				prInstance->arAPP0[E_APP0_JFXX].fgHasThumbnail = TRUE;
			}
		}
		else if (prInstance->arAPP0[E_APP0_JFXX].eThumbnailMode == E_THUMB_3BYTE_PIX)
		{
			if (u2Length < 2)
			{
				return -(INT32)E_JPG_ERR_INVALID_APP0;
			}
			/* thumbnail w & h */
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbW = JPEG_READ_BYTE(prInstance, 0);
			prInstance->arAPP0[E_APP0_JFXX].u1ThumbH = JPEG_READ_BYTE(prInstance, 1);
			ADVANCE_BYTES(2);
			u2Length -= 2;
			/* palettd & pixel validation */
			if ((prInstance->arAPP0[E_APP0_JFXX].u1ThumbW != 0) &&
				(prInstance->arAPP0[E_APP0_JFXX].u1ThumbH != 0) &&
				(u2Length == (UINT16)(((UINT16)prInstance->arAPP0[E_APP0_JFXX].u1ThumbW * (UINT16)prInstance->arAPP0[E_APP0_JFXX].u1ThumbH) * 3)))
			{
				prInstance->arAPP0[E_APP0_JFXX].pu1RGBdata = prInstance->pu1CurrRead;
				prInstance->arAPP0[E_APP0_JFXX].fgHasThumbnail = TRUE;
			}
		}
		else
		{
			// no thumbnail in jfxx ...
			// TODO : try more cases
			return -(INT32)E_JPG_ERR_INVALID_APP0;
		}

		prInstance->arAPP0[E_APP0_JFXX].fgAPP0 = TRUE;
	}
	else if (0 == x_memcmp(au1Identifier, "AVI1\1", 5))
	{
		prInstance->ucPicStruct = 1;
	}
	else if(0 == x_memcmp(au1Identifier, "AVI1\2", 5))
	{
		prInstance->ucPicStruct = 2;
		//return -(INT32)E_JPG_ERR_AVI1_UNSUPPORT;//only decode top field 
	}
	else
	{
		// unsuppported tag
		// just skip it
		//ADVANCE_BYTES(u2Length);
	}

	ADVANCE_BYTES(u2Length);

APP0_ERROR_HANDLING:
	prInstance->u4NumApp0++;
	return (INT32)E_JPG_OK;
}

/* process standard APP1 "Exif" marker */
static INT32 _jfifProcAPP1			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;
	UINT16 u2Length;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->rAPP1.fgAPP1)
	{
		//return -(INT32)E_JPG_ERR_MULTI_APP1;
	}
	ADVANCE_BYTES(2);
	u2Length = JPEG_READ_2BYTES(prInstance, 0);
	if (u2Length < 2)
	{
		return -(INT32)E_JPG_ERR_INVALID_APP1;
	}
    
	prInstance->rAPP1.pu1ExifData = prInstance->pu1CurrRead;
	prInstance->rAPP1.fgAPP1 = TRUE;
	ADVANCE_BYTES(u2Length);
	return (INT32)E_JPG_OK;
}

#ifdef CC_FUJI3D_SUPPORT
static INT32 _jfifProcAPP2			(JPEG_JFIF_HEADER_T *prInstance)
{
	INT32 i4Ret;

        UINT32 u4First, u4Second, u4Third, u4Forth, u4Offset;
        UINT16 u2Endian,  u2Tag, u2I;
        UINT8 au1Tmp[6];
        BOOL fgBigEndian;
        //UINT8* pu1Buf;

	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->rAPP2.fgAPP2)
	{
		//return -(INT32)E_JPG_ERR_MULTI_APP1;
	}

    {
        ADVANCE_BYTES(2);
        //pu1Buf = prInstance->pu1CurrRead;
        prInstance->rAPP2.u4APP2Length = JPEG_READ_2BYTES(prInstance, 0);
        ADVANCE_BYTES(2);

        for (u2I = 0; u2I < 4; u2I++)
        {
            au1Tmp[u2I] = JPEG_READ_BYTE(prInstance, 0);
            ADVANCE_BYTES(1);
        }
        if (au1Tmp[0] == 0x4D && au1Tmp[1] == 0x50 && au1Tmp[2] == 0x46)
        {
            prInstance->rAPP2.fgIsMPF = TRUE;
            //LOG(2, "APP2 IsMPF = 1, 0x%lx\n", (UINT32)prInstance->pu1CurrRead);
        }
        else
        {
            prInstance->rAPP2.fgIsMPF = FALSE;
            //LOG(2, "APP2 IsMPF = 0\n");
            return (INT32)E_JPG_OK;
        }
        //MP Endian
       // pu1App2 = prInstance->pu1CurrRead;                       //app1 body start addr
        u2Endian = JPEG_READ_2BYTES(prInstance, 0);
        ADVANCE_BYTES(2);
        fgBigEndian = (u2Endian == 0x4949) ? FALSE : TRUE;
        //LOG(2, "APP2 fgBigEndian = %d\n", fgBigEndian);

        ADVANCE_BYTES(2);               //skip 2a00
        //first IFD offset
        u4First = JPEG_READ_BYTE(prInstance, 0);
        u4Second = JPEG_READ_BYTE(prInstance, 1);
        u4Third = JPEG_READ_BYTE(prInstance, 2);
        u4Forth = JPEG_READ_BYTE(prInstance, 3);
        u4Offset = (fgBigEndian) ? (u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth :
                   (u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First;
        //LOG(2, "APP2 1st IFD offset = %ld, 0x%lx\n", u4Offset, (UINT32)prInstance->pu1CurrRead);

        //Skip Count (2 byte), version number (12 byte = 2 tag+2 type+4 count+4offset)
        //pu1Buf = pu1App2 + u4Offset + 14;
        ADVANCE_BYTES(u4Offset + 10); 

        u4First = JPEG_READ_BYTE(prInstance, 0);
        u4Second = JPEG_READ_BYTE(prInstance, 1);
        u2Tag = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
        ADVANCE_BYTES(4);
        //pu1Buf += 2;                    //Type
        if (u2Tag == 0xB001)
        {
            u4First = JPEG_READ_BYTE(prInstance, 0);
            u4Second = JPEG_READ_BYTE(prInstance, 1);
            u4Third = JPEG_READ_BYTE(prInstance, 2);
            u4Forth = JPEG_READ_BYTE(prInstance, 3);
            prInstance->rAPP2.u4NumOfImages = (fgBigEndian) ? ((u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth) :
                                   ((u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First);
            //LOG(2, "APP2 u4NumOfImages = %ld, 0x%lx\n", prInstance->rAPP2.u4NumOfImages, (UINT32)prInstance->pu1CurrRead);
            ADVANCE_BYTES(8); 
            //pu1Buf += 4; //offset
        }
        for (u2I = 0; u2I < prInstance->rAPP2.u4NumOfImages; u2I++)
        {
            if (u2I == 1)//MPF_MAX_IMAGE)
            {
                break;
            }
            u4First = JPEG_READ_BYTE(prInstance, 0);
            u4Second = JPEG_READ_BYTE(prInstance, 1);
            u2Tag = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
            ADVANCE_BYTES(2); 
            //pu1Buf += 2;       //Type
            if (u2Tag == 0xB002)
            {
                //pu1Buf += 16; //count
                ADVANCE_BYTES(10); 
                u4First = JPEG_READ_BYTE(prInstance, 0);
                u4Second = JPEG_READ_BYTE(prInstance, 1);
                ADVANCE_BYTES(2); 
                u2Tag = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
                if (u2Tag == 0xB003)
                {
                    ADVANCE_BYTES(10); 
                    u4First = JPEG_READ_BYTE(prInstance, 0);
                    u4Second = JPEG_READ_BYTE(prInstance, 1);
                    ADVANCE_BYTES(2); 
                    u2Tag = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
                    if (u2Tag == 0xB004)
                    {
                        ADVANCE_BYTES(14); 
                    }
                    else
                    {
                        ADVANCE_BYTES(2); 
                    }
                }
                else if (u2Tag == 0xB004)
                {
                    ADVANCE_BYTES(14); 
                }
                else
                {
                    ADVANCE_BYTES(2); 
                }
                ADVANCE_BYTES(4); 
                u4First = JPEG_READ_BYTE(prInstance, 0);
                u4Second = JPEG_READ_BYTE(prInstance, 1);
                u4Third = JPEG_READ_BYTE(prInstance, 2);
                u4Forth = JPEG_READ_BYTE(prInstance, 3);
                ADVANCE_BYTES(4); 
                prInstance->rAPP2.apu1MPEntry[u2I] = (UINT8*)((fgBigEndian) ? ((u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth) :
                                                   ((u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First));
                //LOG(2, "APP2 apu1MPEntry[%d] = 0x%lx, 0x%lx\n", u2I, (UINT32)prInstance->rAPP2.apu1MPEntry[u2I], (UINT32)prInstance->pu1CurrRead);
            }
        }
    }
	
	prInstance->rAPP2.fgAPP2 = TRUE;
	//ADVANCE_BYTES(u2Length);
	return (INT32)E_JPG_OK;
}
#endif

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
	ASSERT(prInstance != NULL);
	VERIFY(_jfifCheckInstance(prInstance) == (INT32)E_JPG_OK);

	if (prInstance->fgEOI)
	{
		return -(INT32)E_JPG_ERR_MULTI_EOI;
	}

	ADVANCE_BYTES(2);

	prInstance->fgEOI = TRUE;
	prInstance->pu1EOI = prInstance->pu1CurrRead;

	return (INT32)E_JPG_OK;
}



/* export interfaces */
// get sizeof instance
JPEGHANDLE JFIF_AllocInstance(void)
{
	JPEGHANDLE hInstance;
	JPEG_BASE_CLASS_T *prRootObject;

	// allocate jfif header instance
	hInstance = (JPEGHANDLE)x_mem_alloc(sizeof(JPEG_JFIF_HEADER_T));
	if (hInstance == NULL)
	{
		return hInstance;
	}
	x_memset(hInstance, 0, sizeof(JPEG_JFIF_HEADER_T));
	// init its root object
	prRootObject = (JPEG_BASE_CLASS_T *)hInstance;
	prRootObject->eJpegClass = E_JC_JFIF_HEADER;
	prRootObject->zObjectSize = sizeof(JPEG_JFIF_HEADER_T);

	return hInstance;
}

void JFIF_FreeInstance(JPEGHANDLE hInstance)
{
	x_mem_free(hInstance);
}

// start and parse a jpeg file image store in memory
INT32 JFIF_New(JPEGHANDLE hInstance, void *pvJpegImage, UINT32 u4JpegSize)
{
	INT32 i4Ret;
	JPEG_JFIF_HEADER_T *prInstance;

	prInstance = (JPEG_JFIF_HEADER_T*)hInstance;

	if (prInstance == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}

	prInstance->pu1JpegImage = (UINT8*)pvJpegImage;
#ifdef JPEG_VLD_TO_PARSE_HEADER
    if(pvJpegImage)
    {
        VERIFY(VLDJPG_SetBarrelShifter(pvJpegImage) == (INT32)E_HWJPG_OK);
    }
#else   /*JPEG_VLD_TO_PARSE_HEADER*/
    prInstance->pu1CurrRead = prInstance->pu1JpegImage;
#endif  /*JPEG_VLD_TO_PARSE_HEADER*/
	prInstance->u4ImageByteSize = u4JpegSize;
	prInstance->fgInited = TRUE;

	i4Ret = _jfifParse(prInstance);
	if ((INT32)E_JPG_OK != i4Ret)
	{
		return i4Ret;
	}

	if (prInstance->fgParsed)
	{
		return (INT32)E_JPG_OK;
	}
	else
	{
		return -(INT32)E_JPG_ERR_PARSE_FAILED;
	}
}

INT32 JFIF_ParseProgMarker(JPEGHANDLE hInstance, UINT32 u4Scan)
{
	INT32 i4Ret;
	JPEG_JFIF_HEADER_T *prInstance;

	prInstance = (JPEG_JFIF_HEADER_T*)hInstance;

	if (prInstance == NULL)
	{
		return -(INT32)E_JPG_ERR_PARAM;
	}

	i4Ret = _jfifParseScan(prInstance, u4Scan);
	return i4Ret;
}

#ifdef THUMBNAIL_SHOW
static INT32 _JFIF_GetMarker(UINT8 **ppu1Ptr, UINT8 *pu1End);
//static VOID _JFIF_GetSOF(UINT8 *pu1Buf, IMGMETA_T *pMeta);
static VOID _JFIF_GetExifTag(UINT8 *pu1App1, UINT16 u2Tag,
                                UINT32 u4Len, UINT32 u4Offset, BOOL fgBigEndian, IMGMETA_T *pMeta);

static VOID _JFIF_GetAPP1(UINT8 *pu1Buf, IMGMETA_T *pMeta);
#define IMAGE_EXIF_MAKE_TAG         0x010f
#define IMAGE_EXIF_MODEL_TAG        0x0110
#define IMAGE_EXIF_ORIENTATION_TAG  0x0112
#define IMAGE_EXIF_DATE_TAG         0x0132
#define IMAGE_EXIF_INTERCHANGE      0x0201
#define IMAGE_EXIF_INTERCHANGELEN   0x0202
#define RINGBUFFERSIZE 0x80000
#define _ABS_JFIF(x)  (((UINT32)x) & 0x7fffffff)

INT32 JFIF_GetImgMetaData(IMGMETA_T *pMeta,
                                   UINT8 *pu1InBuf, UINT32 u4InSize)
{
    INT32 eRet = 0;
    INT32 i4Marker = 0;
    UINT32 u4First, u4Second;
    UINT8 *pu1Buf, *pu1Ptr,*pu1End;
    UINT32 u4Len;
    if (pMeta == NULL)
    {
        LOG(0," - Image Meta struct is null\n");
        return -1;
    }

    x_memset((void*)pMeta, 0x0, sizeof(IMGMETA_T));

    if (pu1InBuf != NULL)
    {
        pu1Buf = pu1InBuf;
        pu1End = pu1Buf + u4InSize;
    }
    else
    {
       return -1;
    }

    pu1Ptr = pu1Buf;

        LOG(3,"Exif Buf Start %x, End 0x%x\n", (UINT32)pu1Buf, (UINT32)pu1End);
        while ((_ABS_JFIF(pu1Ptr) < _ABS_JFIF(pu1End)) && (i4Marker != M_SOS))
        {
            i4Marker = _JFIF_GetMarker(&pu1Ptr, pu1End);
            LOG(3,"0x%x\n",i4Marker);
            switch (i4Marker)
            {
            case M_SOI:
                break;
            case M_EOI:
                pu1End = pu1Ptr;
                break;
            case M_SOF0:
            case M_SOF2:
                //_JFIF_GetSOF(pu1Ptr, pMeta);
                break;
            case M_APP1:
                _JFIF_GetAPP1(pu1Ptr, pMeta);
                return 0;
            case M_APP2:
                break;
            default:
                break;
            }
            if (i4Marker != M_SOI)
            {
                u4First = *pu1Ptr;
                u4Second = *(pu1Ptr + 1);
                u4Len = (u4First << 8) | u4Second;
                pu1Ptr += u4Len;
                LOG(3,"Consume 0x%x, Ptr 0x%x\n",u4Len, (UINT32)pu1Ptr);
            }
        }
#if 0
        LOG(0," width:%d, height:%d, orientation:%d\n",
                      pMeta->u4Width, pMeta->u4Height, pMeta->u4Orientation);
        if ((!pMeta->u4Width) || (!pMeta->u4Height))
        {
            LOG(0,"jpg resolution error\n");
            return -1;
        }

        pMeta->u4BPP = (u4InSize / (pMeta->u4Width * pMeta->u4Height));
 #endif
    return eRet;
}
static INT32 _JFIF_GetMarker(UINT8 **ppu1Ptr, UINT8 *pu1End)
{
    UINT8 *pu1Buf;
    UINT8 u1First, u1Second;
    INT32 i4Marker = -1;

    pu1Buf = *ppu1Ptr;
    while (1)
    {
        if ((_ABS_JFIF(pu1Buf))>= (_ABS_JFIF(pu1End)))
        {
            LOG(0,"marker not found %08x\n",(_ABS_JFIF(pu1Buf)));
            break;
        }
        u1First = *pu1Buf++;
        u1Second = *pu1Buf;
      
        if ((u1First == 0xff) && (u1Second >= 0xc0) && (u1Second <= 0xfe) && ((_ABS_JFIF(pu1Buf)) <  (_ABS_JFIF(pu1End))))
        {
        
            i4Marker = (0xff << 8)| (*pu1Buf++);
                        break;
        }
    }
    *ppu1Ptr = pu1Buf;

    return i4Marker;
}
#if 0
static VOID _JFIF_GetSOF(UINT8 *pu1Buf, IMGMETA_T *pMeta)
{
    UINT32 u4First, u4Second;

    pu1Buf += 3;
    u4First = *pu1Buf++;
    u4Second = *pu1Buf++;
    pMeta->u4Height = (u4First << 8) | u4Second;
    u4First = *pu1Buf++;
    u4Second = *pu1Buf++;
    pMeta->u4Width = (u4First << 8) | u4Second;
}
#endif
static VOID _JFIF_GetExifTag(UINT8 *pu1App1, UINT16 u2Tag,
                                UINT32 u4Len, UINT32 u4Offset, BOOL fgBigEndian, IMGMETA_T *pMeta)
{
    UINT8 *pu1Buf;

    pu1Buf = (u4Len <= 4) ? (UINT8 *)&u4Offset : pu1App1 + u4Offset;

    switch (u2Tag)
    {
    case IMAGE_EXIF_MAKE_TAG:
        x_memcpy(pMeta->au1Maker, pu1Buf, u4Len);
        LOG(5,"Maker %s\n", pMeta->au1Maker);
        break;
    case IMAGE_EXIF_MODEL_TAG:
        x_memcpy(pMeta->au1Model, pu1Buf, u4Len);
        LOG(5,"Model %s\n", pMeta->au1Model);
        break;
    case IMAGE_EXIF_DATE_TAG:
        x_memcpy(pMeta->au1Date, pu1Buf, u4Len);
        LOG(5,"Date %s\n", pMeta->au1Date);
        break;
    case IMAGE_EXIF_ORIENTATION_TAG:
        pMeta->u4Orientation = (fgBigEndian) ? u4Offset >> 16: u4Offset;
        break;
    case IMAGE_EXIF_INTERCHANGE:
        pu1Buf = pu1App1 + u4Offset;
        if ((u4Offset == 0x0) || (u4Offset > RINGBUFFERSIZE))
        {
            pMeta->pu1Thumbnail = NULL;
        }
        else
        {
            pMeta->pu1Thumbnail = pu1Buf;
            if ((pMeta->pu1Thumbnail[0] != 0xff) || (pMeta->pu1Thumbnail[1] != 0xd8))
            {
                pMeta->pu1Thumbnail = NULL;
            }
        }
        break;
    case IMAGE_EXIF_INTERCHANGELEN:
        pMeta->u4ThumbnailLength = (u4Offset >=  RINGBUFFERSIZE) ? 0 : u4Offset;
        break;
    default:
        break;
    }
}

static VOID _JFIF_GetAPP1(UINT8 *pu1Buf, IMGMETA_T *pMeta)
{
    UINT8 au1Tmp[6], *pu1App1;
    UINT32 u4First, u4Second, u4Third, u4Forth, u4Offset, u4Len;
    UINT16 u2Endian, u2TagCnt, u2Tag, u2I, u2J;
    //BOOL fgExifExist;
    BOOL fgBigEndian;

    pMeta->pu1APP1 = pu1Buf;
    u4First = *pu1Buf++;
    u4Second = *pu1Buf++;
    pMeta->u4APP1Length = (u4First << 8) | u4Second;

    for (u2I = 0; u2I < 6; u2I++)
    {
        au1Tmp[u2I] = *pu1Buf++;
    }
    if (au1Tmp[0] == 0x45 && au1Tmp[1] == 0x78 && au1Tmp[2] == 0x69 && au1Tmp[3] == 0x66
            && au1Tmp[4] == 0x00 && au1Tmp[5] == 0x00)
    {
       // fgExifExist = TRUE;
    }
    else
    {
        //fgExifExist = FALSE;
        return;
    }
    pu1App1 = pu1Buf;                       //app1 body start addr
    u4First = *pu1Buf++;
    u4Second = *pu1Buf++;
    u2Endian = (u4First << 8) | u4Second;
    fgBigEndian = (u2Endian == 0x4949) ? FALSE : TRUE;
    pu1Buf += 2;                //skip 2a00
    for (u2J = 0; u2J < 2; u2J++)                   //get ifd0 and ifd1
    {
        u4First = *pu1Buf++;
        u4Second = *pu1Buf++;
        u4Third = *pu1Buf++;
        u4Forth = *pu1Buf;
        u4Offset = (fgBigEndian) ? (u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth :
                   (u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First;

        pu1Buf = pu1App1 + u4Offset;
        u4First = *pu1Buf++;
        u4Second = *pu1Buf++;
        u2TagCnt = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
        if ((u4Offset == 0) || (u4Offset >= pMeta->u4APP1Length))
        {
            break;
        }
        for (u2I = 0; u2I < u2TagCnt; u2I++)
        {
            u4First = *pu1Buf++;
            u4Second = *pu1Buf++;
            u2Tag = (fgBigEndian) ? (u4First << 8) | u4Second : (u4Second << 8) | u4First;
            pu1Buf += 2;                    //Type
            u4First = *pu1Buf++;
            u4Second = *pu1Buf++;
            u4Third = *pu1Buf++;
            u4Forth = *pu1Buf++;
            u4Len = (fgBigEndian) ? (u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth :
                    (u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First;
            u4First = *pu1Buf++;
            u4Second = *pu1Buf++;
            u4Third = *pu1Buf++;
            u4Forth = *pu1Buf++;
            u4Offset = (fgBigEndian) ? (u4First << 24) | (u4Second << 16) | (u4Third << 8) | u4Forth :
                       (u4Forth << 24) | (u4Third << 16) | (u4Second << 8) | u4First;

            _JFIF_GetExifTag(pu1App1, u2Tag, u4Len, u4Offset, fgBigEndian, pMeta);
        }
    }
    LOG(3,"thumbnail %x, len %d, 0x%x\n", (UINT32)pMeta->pu1Thumbnail, pMeta->u4ThumbnailLength, (UINT32)pu1Buf);
}
#endif


