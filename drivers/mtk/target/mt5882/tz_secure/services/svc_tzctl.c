/*
 * Copyright:
 * ----------------------------------------------------------------------------
 * This confidential and proprietary software may be used only as authorised
 * by a licensing agreement from ARM Limited.
 *      (C) COPYRIGHT 2008-2009 ARM Limited, ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised copies and
 * copies may only be made to the extent permitted by a licensing agreement
 * from ARM Limited.
 * ----------------------------------------------------------------------------
 * $LastChangedRevision: 2907 $
 * ----------------------------------------------------------------------------
 */

/* ============================================================================
    Includes
============================================================================ */
#include <stdlib.h>

#include "tzst.h"
#include "svc_registry.h"
#include "x_hal_5381.h"
#include "tee_common.h"
#include "tee_securestorage.h"
#include "tee_nflx.h"
#include "tee_playready.h"
#include "tee_dtcp.h"
#include "tee_ddi.h"
#include "tz_dmx_if.h"
#ifdef CC_ENABLE_ULI_IN_TEE
#include "tee_uli.h"
#endif
#include "mtskb.h"
#ifdef CC_ENABLE_HDCP2
#include "mthdcp2.h"
#include "drm_hdcp2.h"
#endif

#include "tee_vudu.h"


static UINT8 *gp_u1PlainKey = NULL;
static UINT8 *gp_u1PlainIv = NULL;
static UINT8 *gp_u1DecrptAddr = NULL;
static UINT8 *gp_u1DecrptAlignAddr = NULL;
// MPEG Picture types
#define TZ_PIC_TYPE_I                      0x01        ///< I picture
#define TZ_PIC_TYPE_P                      0x02        ///< P picture
#define TZ_PIC_TYPE_B                      0x03        ///< B picture

/* ============================================================================
    Internal Functions
============================================================================ */

// this function is mainly to test c-lib
static void SvcEx_ReverseArray(UINT8* pu1Array, UINT32 u4Size)
{
    #define SIZE 512*1024

    UINT32 i;
    UINT8* pu1Buf;

    // we allocate a big array to check memory pool size
    pu1Buf = (UINT8*)malloc(SIZE);
    if (!pu1Buf)
    {
        DMSG("malloc error!\n");
        return;
    }

    DMSG("pu1Buf=0x%x, size=%d\n", pu1Buf, SIZE);
    memset((VOID*)pu1Buf, 0, SIZE);

    for (i = 0; i < u4Size; i++)
    {
        pu1Buf[i] = pu1Array[u4Size-1-i];
    }

    memcpy_s2n_chk((VOID*)pu1Array, (VOID*)pu1Buf, u4Size, NULL);
    free(pu1Buf);
}

#ifndef CC_NDEBUG
static void SvcEx_TZ_IO_WRITE32(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    (*((volatile UINT32*)(pIo->u4Base + pIo->u4Offset)) = (pIo->u4Val));
}

static void SvcEx_TZ_IO_WRITE16(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    (*((volatile UINT16*)(pIo->u4Base + pIo->u4Offset)) = (UINT16)pIo->u4Val);
}

static void SvcEx_TZ_IO_WRITE8(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    (*((volatile UINT8*)(pIo->u4Base + pIo->u4Offset)) = (UINT8)pIo->u4Val);
}

static void SvcEx_TZ_IO_READ32(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    pIo->u4Val = *((volatile UINT32*)(pIo->u4Base + pIo->u4Offset));
}

static void SvcEx_TZ_IO_READ16(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    pIo->u4Val = *((volatile UINT16*)(pIo->u4Base + pIo->u4Offset));
}

static void SvcEx_TZ_IO_READ8(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_IO_T *pIo;

    pIo = (TZ_IO_T *)pu1Param;
    pIo->u4Val = *((volatile UINT8*)(pIo->u4Base + pIo->u4Offset));
}
#endif

static BOOL SvcEx_DMX_GetPicHeader(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	#define PIC_HEADER_LEN_MAX  32
	TZ_DMX_PICHEADER_T *pPicHeader;
	UINT32 u4Addr, u4BufStart, u4BufEnd, u4Size, u4CacheLineSize;
	UINT8 *pu1Buf = NULL;
	UINT8 u4Buf[PIC_HEADER_LEN_MAX] = {0};

	pPicHeader = (TZ_DMX_PICHEADER_T *)pu1Param;
	u4Addr = pPicHeader->u4Addr;
	u4BufStart = pPicHeader->u4BufStart;
	u4BufEnd = pPicHeader->u4BufEnd;
	pu1Buf = pPicHeader->pu1Buf;
	u4Size = pPicHeader->u4Size;

	// to make more secured,we limit data size to 32 
	if (u4Size > PIC_HEADER_LEN_MAX)
	{
		DMSG("u4Size > PIC_HEADER_LEN_MAX!\n");
		return FALSE;
	}

	u4CacheLineSize = TZ_CACHE_ALIGN(u4Size);

	TZ_SWD_BufferStart((UINT32)pu1Buf, u4CacheLineSize);

	// Check if the header is across FIFO boundary
	if (((u4Addr + u4Size) - 1) >= u4BufEnd)
	{
		UINT8 u1Len = (UINT8)(u4BufEnd - u4Addr);
		if(u1Len >= u4Size)
		{
			return FALSE;
		}
		TZ_SWD_InvDCache(u4Addr, u1Len);
		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
		memcpy_s2s_chk((void*)u4Buf, (void*)u4Addr, u1Len, NULL);
		memcpy_s2s_chk((void*)(u4Buf + u1Len), (void*)u4BufStart, (u4Size - u1Len), NULL);
		TZ_SWD_InvDCache(u4Addr, u1Len);
		TZ_SWD_InvDCache(u4BufStart, (u4Size - u1Len));
	}
	else
	{
		TZ_SWD_InvDCache(u4Addr, u4Size);
		memcpy_s2s_chk((void*)u4Buf, (void*)u4Addr, u4Size, NULL);
		TZ_SWD_InvDCache(u4Addr, u4Size);
	}

#if 0 //Do not check start code for some multi-media file don't have it
	// check if the first three bytes are 00 00 01/80
	if ((u4Buf[0] != 0x0) || (u4Buf[1] != 0x0) ||
		(u4Buf[2] != 0x1) && (u4Buf[2] != 0x80))
	{
		DMSG("Invalid pic header, or incorrect address!\n");
		return FALSE;
	}
#endif

	memcpy_s2n_chk((void*)pu1Buf, (void*)u4Buf, u4Size, NULL);


	TZ_SWD_BufferEnd((UINT32)pu1Buf, u4CacheLineSize);

	//DMSG("TZ ADDR: 0x%x 0x%x 0x%x 0x%x 0x%x\n", u4Addr, u4BufStart, u4BufEnd, pu1Buf, u4Size);
	//DMSG("TZ PIC: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pu1Buf[0], pu1Buf[1], pu1Buf[2], pu1Buf[3], pu1Buf[4], pu1Buf[5]);
	//DMSG("TZ PIC: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pu1Buf[0], pu1Buf[1], pu1Buf[2], pu1Buf[3], pu1Buf[4], pu1Buf[5]);

	return TRUE;
}

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
static BOOL SvcEx_VDEC_FB_Operation(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	UINT32 u4Value, u4SrcAddr, u4DesAddr, u4Size;
	UINT32 u4RangeStart, u4RangeEnd;
	TZ_FB_OPERATION_T* prFbOp;
	prFbOp = (TZ_FB_OPERATION_T*)pu1Param;
	
	u4RangeStart = GetSecureFBMStart();
	u4RangeEnd = GetSecureFBMEnd();        

	switch (prFbOp->eOperation)
	{
            case TZ_VDEC_FB_MEMSET:                   
            case TZ_VDEC_FB_FLUSHMUTILINE:
            case TZ_VDEC_FB_HALFLUSH:
                    if ((prFbOp->u4SrcAddr < u4RangeStart) || (prFbOp->u4SrcAddr > u4RangeEnd))
                    {
                         DMSG("src address out of trustzone range!\n");
                         return FALSE;
                    }           
                    if (((prFbOp->u4SrcAddr + prFbOp->u4Size)<u4RangeStart) || ((prFbOp->u4SrcAddr+prFbOp->u4Size)>u4RangeEnd))
                    {
                         DMSG("src address end out of trustzone range!\n");
                         return FALSE;
                    }                   
			break;
		case TZ_VDEC_FB_MEMCPY:      
                    if ((prFbOp->u4SrcAddr < u4RangeStart) || (prFbOp->u4SrcAddr > u4RangeEnd) ||
                                (prFbOp->u4DesAddr < u4RangeStart) || (prFbOp->u4DesAddr > u4RangeEnd))
                    {
                            DMSG("address out of trustzone range!\n");
                            return FALSE;
                    }   
                    if (((prFbOp->u4SrcAddr + prFbOp->u4Size) < u4RangeStart) || ((prFbOp->u4SrcAddr + prFbOp->u4Size) > u4RangeEnd) ||
                                ((prFbOp->u4DesAddr + prFbOp->u4Size)< u4RangeStart) || ((prFbOp->u4DesAddr + prFbOp->u4Size)> u4RangeEnd))
                    {
                            DMSG("address out of trustzone range!\n");
                            return FALSE;
                    }                       
			break;

			break;
		case TZ_VDEC_FB_GETVALUE:     
		case TZ_VDEC_FB_SETVALUE:           
                    if ((prFbOp->u4SrcAddr < u4RangeStart) || (prFbOp->u4SrcAddr > u4RangeEnd))
                    {
                         DMSG("src address out of trustzone range!\n");
                         return FALSE;
                    }    
			break;            
	}
     

	switch (prFbOp->eOperation)
	{
		case TZ_VDEC_FB_MEMSET:                   
			memset((VOID*)VIRTUAL(prFbOp->u4SrcAddr), prFbOp->u4Value, prFbOp->u4Size);
			break;
		case TZ_VDEC_FB_FLUSHMUTILINE:
			HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prFbOp->u4SrcAddr), prFbOp->u4Size);
			break;
		case TZ_VDEC_FB_MEMCPY:      
			memcpy((VOID*)VIRTUAL(prFbOp->u4DesAddr), (VOID*)VIRTUAL(prFbOp->u4SrcAddr), prFbOp->u4Size);
			break;
		case TZ_VDEC_FB_HALFLUSH:
			HalFlushDCacheMultipleLine(VIRTUAL(prFbOp->u4SrcAddr), prFbOp->u4Size);
			break;
		case TZ_VDEC_FB_GETVALUE:     
			prFbOp->u4Value = *(UINT32 *)(VIRTUAL(prFbOp->u4SrcAddr));    
			break;
		case TZ_VDEC_FB_SETVALUE:           
			*(UINT32 *)(VIRTUAL(prFbOp->u4SrcAddr)) =  prFbOp->u4Value;
			break;            
	}

	return TRUE;
}

static BOOL SvcEx_VDEC_Check_RealD(UINT8 *pu1Param, UINT32 u4ParmSize)
{
	BOOL bIsFHD = TRUE, bChkFilterType = FALSE;;
	UINT8 u1BlockIdx, u1PixIdx, u1QuincunxCnt = 0;
	INT8   i1Y=0, i1Cb=0, i1Cr=0;
	UINT32 u4Offset, u4BlkSz, u4WIdx, u4PixerIdx = 0, u4UnqualifiedCnt = 0;;
	UINT8* pu1DstY = NULL;
    UINT8* pu1DstC = NULL;
	UINT32* pu4Dst;
	UINT32* pu4DecData;
	static UINT32* pu4TmpBufY = NULL;
    static UINT32* pu4TmpBufC = NULL; 
	
	TZ_DISP_PIC_INFO_T *prDispInfo;
	prDispInfo = (TZ_DISP_PIC_INFO_T*)pu1Param;

	if(pu4TmpBufY == NULL)
    {
        pu4TmpBufY = (UINT32*)malloc(TZ_CACHE_ALIGN(1920));
    }
    if(pu4TmpBufC == NULL)
    {
        pu4TmpBufC = (UINT32*)malloc(TZ_CACHE_ALIGN(1920));
    }
    if ((!pu4TmpBufY) || (!pu4TmpBufC))
    {
        DMSG("malloc fail");
        return FALSE;
    }
	//Y start addr of the last line, block 2 raster
    u4Offset = prDispInfo->u4H % 32;
	u4Offset = (u4Offset > 0) ? u4Offset : 32;
    u4BlkSz = 1 << 9; //(64x32) / 4, uint8 to uint32
    pu4Dst = pu4TmpBufY;
	
    pu4DecData = (UINT32*)((UINT8*)VIRTUAL((UINT32)prDispInfo->pvYAddr)
                + prDispInfo->u4W * (prDispInfo->u4H - u4Offset)
                + 64 * (u4Offset - 2));

	for(u4WIdx = 0; u4WIdx < (prDispInfo->u4W_Cmp - 64); u4WIdx += 64)
	{
		memcpy((void*)pu4Dst, (void*)pu4DecData, 64);   // 16 x4 byte
                pu4DecData += u4BlkSz;
                pu4Dst += 16;
	}
	memcpy((void*)pu4Dst, (void*)pu4DecData, prDispInfo->u4W_Cmp % 64);

    //C start addr of the last line, block 2 raster
	u4Offset = (prDispInfo->u4H>>1) % 16;
	u4Offset = (u4Offset > 0) ? u4Offset : 16;
	u4BlkSz = 1 << 8; //64x16 / 4
	pu4Dst = pu4TmpBufC;

	pu4DecData = (UINT32*)((UINT8*)VIRTUAL((UINT32)prDispInfo->pvCAddr)
                + prDispInfo->u4W * ((prDispInfo->u4H>>1) - u4Offset)
                + 64 * (u4Offset - 1));

	for(u4WIdx = 0; u4WIdx < (prDispInfo->u4W_Cmp - 64); u4WIdx += 64)
	{
		memcpy((void*)pu4Dst, (void*)pu4DecData, 64);
                pu4DecData += u4BlkSz;
                pu4Dst += 16;
	}
	memcpy((void*)pu4Dst, (void*)pu4DecData, prDispInfo->u4W_Cmp % 64);         
    pu1DstY = (UINT8*)(UINT32)pu4TmpBufY;
    pu1DstC = (UINT8*)(UINT32)pu4TmpBufC;
    u4PixerIdx = 0;

            //check 3D tag
	bIsFHD = (prDispInfo->u4W_Cmp == 1920) ? TRUE : FALSE;
	u1BlockIdx = (bIsFHD) ? 0 : 1; //720p start from 1, 1080p start from 0
	u1PixIdx = 0; //start from 0
	for(u4WIdx = 0; u4WIdx < prDispInfo->u4W_Cmp; u4WIdx++)
	{
		if(bIsFHD)
		{
			u1PixIdx = u4WIdx % 32;
			if(u1PixIdx == 0)
			{
				u1BlockIdx++;
			}
		}
		else //1280
        {
			u1PixIdx = (u4WIdx ==0) ? 0 : (u1PixIdx + 1);
            if((((u1BlockIdx & 0x3) == 3) && (u1PixIdx == 22)) ||
                        (((u1BlockIdx & 0x3) < 3) && (u1PixIdx == 21)))
            {
				u1PixIdx = 0;
                u1BlockIdx++;
            }
         }
		if(u4WIdx == (prDispInfo->u4W_Cmp >> 1)) //left blocks are use to distinguish filter type
		{
			u1BlockIdx--;
		}
		if((bIsFHD && ((u4WIdx >> 5) >= 54)) ||
                    ((!bIsFHD) && (u4WIdx >= 1152)))    //start checking filter type at 54 block, u1BlockIdx type=3
		{
			if(!bChkFilterType) //change point
			{
				u1BlockIdx = 2; //default: simple horizontal squeeze type
			}
			bChkFilterType = TRUE;
		}
		u1BlockIdx = (u1BlockIdx == 4) ? 1 : u1BlockIdx;

		if((u1PixIdx < 5) ||
                    (bIsFHD && (u1PixIdx > 26)) ||
                    ((!bIsFHD) && (u1PixIdx > 16)))
		{
			u4PixerIdx +=1;              
			continue;
		}
		pu1DstY = (UINT8*)(UINT32)pu4TmpBufY + u4PixerIdx;
		pu1DstC = (UINT8*)(UINT32)pu4TmpBufC + (u4PixerIdx/2)*2;  //422 YCbCr raster
                
		i1Y = (INT8)*pu1DstY - 16;
		i1Cb = (INT8)*pu1DstC - 128;
		i1Cr = (INT8)*(pu1DstC+1)- 128;
		u4PixerIdx +=1;
		if(u1BlockIdx == 1)
		{
			if(((prDispInfo->bIsCur2D) && ((2327*i1Cr + 213*i1Cb) < 75000) && ((1793*i1Cr - 2115*i1Cb) < 75000)) || 
                       (!(prDispInfo->bIsCur2D) && ((2327*i1Cr + 213*i1Cb) < 55000)&& ((1793*i1Cr - 2115*i1Cb) < 55000)))
			{
				!bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
			}
		}
		else if(u1BlockIdx == 2)
		{
			if(((prDispInfo->bIsCur2D) && ((-2327*i1Cr - 213*i1Cb) < 75000) && ((-534*i1Cr - 2328*i1Cb) < 75000)) || 
                       (!(prDispInfo->bIsCur2D) && ((-2327*i1Cr - 213*i1Cb) < 55000)&& ((-534*i1Cr - 2328*i1Cb) < 55000)))
			{
				!bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
			}
		}
		else
		{
			if(((prDispInfo->bIsCur2D) && ((-1793*i1Cr + 2115*i1Cb) < 75000) && ((534*i1Cr + 2328*i1Cb) < 75000)) || 
                       (!(prDispInfo->bIsCur2D) && ((-1793*i1Cr + 2115*i1Cb) < 55000)&& ((534*i1Cr + 2328*i1Cb) < 55000)))
			{
				!bChkFilterType ? u4UnqualifiedCnt++ : u1QuincunxCnt++;
			}
		}
	}
	prDispInfo->u4UnqualifiedCnt = u4UnqualifiedCnt;
	prDispInfo->u1QuincunxCnt = u1QuincunxCnt;
	return TRUE;
}
#endif

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
static BOOL SvcEx_AU_DELIM_APPEND(UINT8 *pu1Param, UINT32 u4ParmSize)
{
    TZ_AU_DELIM_APPEND_T* prAppendDelim;
    prAppendDelim = (TZ_AU_DELIM_APPEND_T*)pu1Param;
    memcpy_n2s_chk((VOID*)VIRTUAL(prAppendDelim->u4Base + prAppendDelim->u4Offset), (VOID*)VIRTUAL(prAppendDelim->pu1Buf), prAppendDelim->u4Size, NULL);
    return TRUE;
}
#endif

static inline UINT32  TZ_MPG_GET_START_ADDR(UINT32 x, INT32 y, const TZ_DMX_PES_MSG_T *prPes)
{
    if (NULL == prPes)
    {
        return 0;
    }
    
    if (0 <= y)
    {
        return ((x+(UINT32)y>=VIRTUAL(prPes->u4BufEnd)) ? 
                (VIRTUAL(prPes->u4BufStart)+x+(UINT32)y-VIRTUAL(prPes->u4BufEnd)) : 
                (x+(UINT32)y));
    }
    else
    {
        UINT32 u4Tmp = -y;
        return ((x-u4Tmp>=VIRTUAL(prPes->u4BufEnd)) ? 
                (VIRTUAL(prPes->u4BufStart)+x-u4Tmp-VIRTUAL(prPes->u4BufEnd)) : 
                (x-u4Tmp));
    }

}
static inline UINT32 _TZ_SWDMX_MpgGetDatain(UINT32 u4StartAddr, const TZ_DMX_PES_MSG_T * prPes)
{
    UINT32 u4Datain = 0;
    UINT32 u4Cnt = 0;
    UCHAR *puData = (UCHAR *)u4StartAddr;

    // Data Align case
    u4Datain = (UINT32)(*puData);
    for (u4Cnt=1; u4Cnt<4; u4Cnt++)
    {
        puData = (UCHAR *)TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)u4Cnt, prPes);
        u4Datain = u4Datain << 8;
        u4Datain |= (UINT32)(*puData);
    }
  
    return u4Datain;
}

static UINT32 _TZ_SWDMX_MpgSearchStartCode (   UINT16 u2CodeToFind, 
                                                        UINT32 u4StartAddr,      
                                                        const TZ_DMX_PES_MSG_T *prPes)
{
    UINT32 u4CurAddr = u4StartAddr;

    if (prPes == NULL)
    {
        return 0;
    }
    
    if ((u4CurAddr<VIRTUAL(prPes->u4BufStart)) || (u4CurAddr>VIRTUAL(prPes->u4BufEnd)))
    {
        DMSG("%s:%u Error Start addr.\n",__FUNCTION__, __LINE__);
        return 0;
    }
    
    // find start code by value
    while ( (TZ_MPG_GET_START_ADDR(u4CurAddr, (INT32)3, prPes) != VIRTUAL(prPes->u4FrameAddr)) &&
            (_TZ_SWDMX_MpgGetDatain(u4CurAddr++, prPes) != u2CodeToFind) ) 
    {
        // Nothing to do... ... 
    }

    if (_TZ_SWDMX_MpgGetDatain(TZ_MPG_GET_START_ADDR(u4CurAddr, (INT32)-1, prPes), prPes) == u2CodeToFind)
    {
        u4CurAddr = TZ_MPG_GET_START_ADDR(u4CurAddr, (INT32)-1, prPes);
    }
    else
    {
        u4CurAddr = 0;
    }


    return u4CurAddr;
}

static BOOL SvcEx_SWDMX_MpgParsingPts(UINT8 *pu1Param, UINT32 u4ParmSize)
{

    UINT32 u4SkipBits = 0;
    UINT32 u4Datain = 0;
    UINT32 u4Data = 0;
    UINT32 u4StartAddr = 0;
    CHAR  ucType;
    UINT8 u1Length = 0;    
    
    TZ_DMX_PES_MSG_T *prPes;
    prPes = (TZ_DMX_PES_MSG_T*)pu1Param;
    
    DMSG("TZ u4BufStart=0x%x, u4BufEnd=0x%x u4FrameAddr=0x%x u4PrevFrameAddr=0x%x u4FrameType=0x%x u4Pts=0x%x u4Dts=0x%x\n", 
            prPes->u4BufStart, 
            prPes->u4BufEnd,
            prPes->u4FrameAddr,
            prPes->u4PrevFrameAddr,
            prPes->u4PrevFrameType,
            prPes->u4Pts,
            prPes->u4Dts);
            
    // find Pic hdr, skip seq hdr & grp hdr
    u4StartAddr = _TZ_SWDMX_MpgSearchStartCode(0x100, VIRTUAL(prPes->u4PrevFrameAddr), prPes);
    if (u4StartAddr == 0)
    {
        DMSG("%s(%u):%u No Pic start code found, Data may error.\n",
                __FUNCTION__, __LINE__);
        goto EXIT_PORT;
    }
    
    u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
    
    // not picture header
    if (u4Datain != 0x100)
    {
        DMSG("%s(%u):%u Not Pic start code found.\n",
                __FUNCTION__, __LINE__); 
        // ASSERT(0);
        goto EXIT_PORT;
    }
    
    // skip pic header--->begin
    u4SkipBits = 32 + 10 + 3 + 16;
    // pic type different, skip bits num different.
    switch(prPes->u4PrevFrameType)
    {
        case TZ_PIC_TYPE_I:
            // do nothing
            break;
        case TZ_PIC_TYPE_P:
            u4SkipBits += 4;
            break;
        case TZ_PIC_TYPE_B:
            u4SkipBits += 8;
            break;
        default:
            DMSG("Err Pic type.\n");
            goto EXIT_PORT;
    }
    u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u4SkipBits/8), prPes);
    u4SkipBits = u4SkipBits%8;
    
    // skip extra_info_pic
    while(((*((CHAR*)u4StartAddr)) << u4SkipBits) & 0x80)
    {
        u4SkipBits += 9;
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u4SkipBits/8), prPes);
        u4SkipBits = u4SkipBits%8;        
    }
    
    // skip extra_bit_pic
    u4SkipBits+=1;
    if(u4SkipBits%8)
    {
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u4SkipBits/8 + 1), prPes);
    }
    else
    {
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u4SkipBits/8), prPes);
        u4SkipBits = 0;
    }
    
    // skip pic header--->end 
    // skip pic_coding_extension--->begin
    u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
    if(u4Datain != 0x1B5 && u4Datain != 0x1B2)
    {
        DMSG("Expected start code not found.\n");
        goto EXIT_PORT;
    }
    
    if(u4Datain == 0x1B5)
    {
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)8, prPes);
        u4SkipBits = 2;
        // skip compsdite_display_flag content
        if(((*((CHAR*)u4StartAddr)) << 1) & 0x80)
        {
            u4SkipBits += 20;                      
        }
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u4SkipBits/8 + 1), prPes);
        u4SkipBits = 0;            
    }

    u4StartAddr = _TZ_SWDMX_MpgSearchStartCode(0x1B2, u4StartAddr, prPes);
    u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
    if (u4Datain != 0x1B2)
    {
        return TRUE;
    }
    
    // skip start code
    u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)4, prPes);

    // parsing user data
    u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
    
    while ( (!(prPes->u4Dts && prPes->u4Pts)) || 
            (!((u4Datain & 0xFFFFFF00) == 0x0100)) )
    {
        // get length
        u1Length = (UINT8)(*((CHAR*)u4StartAddr));
        
        // skip length
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)1, prPes);
        
        if(u1Length == 0)
        {
            // parse next infomation
            u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
            continue;
        }
        
        // get type
        ucType = (*((CHAR*)u4StartAddr));
        // skip user data type
        u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)1, prPes);
        
        switch(ucType)
        {
            // pts
            case 0x2:
            {
                u4Data = 0;
                u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
                u4Data |= ((u4Datain & 0x3000000) << 6);
                u4Data |= ((u4Datain & 0x7FFF00) << 7);
                u4Data |= ((u4Datain & 0x7F) << 8);

                u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)4, prPes);
                u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
                u4Data |= ((u4Datain >> 24) & 0xFF);
                u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)1, prPes);

                prPes->u4Pts = u4Data;
                
                break;
            }
            
            // dts
            case 0x4:
            {
                u4Data = 0;
                u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
                u4Data |= ((u4Datain & 0x3000000) << 6);
                u4Data |= ((u4Datain & 0x7FFF00) << 7);
                u4Data |= ((u4Datain & 0x7F) << 8);

                u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)4, prPes);
                u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
                u4Data |= ((u4Datain >> 24) & 0xFF);
                u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)1, prPes);
                prPes->u4Dts = u4Data;
                
                break;
            }
        
            default:
            {
                if ( (ucType>10) || (u1Length>6) )
                {
                    DMSG("Err T(%d) L(%d)\n", ucType, u1Length);
                    goto EXIT_PORT;
                }
                
                u4StartAddr = TZ_MPG_GET_START_ADDR(u4StartAddr, (INT32)(u1Length - 1), prPes);
                break;
            }
        }     

        u4Datain = _TZ_SWDMX_MpgGetDatain(u4StartAddr, prPes);
    }
    
EXIT_PORT:
    DMSG("VO(27M Based) PTS(0x%X) DTS(0x%X)\n", prPes->u4Pts, prPes->u4Dts);

    return TRUE;
}
static BOOL SvcEx_SWDMX_DecryptData(UINT8 *pu1Param, UINT32 u4ParmSize)
{
    UINT32 u4FrameSizeA = 0;
    UINT32 u4FrameSizeB = 0;
	UINT32 u4AlignSize = 0;
	#if 1
	
    
    TZ_DESCRYPT_DATA_T *ptDescryptData = (TZ_DESCRYPT_DATA_T*)pu1Param;
	
    if (ptDescryptData == NULL)
    {
        return TZ_ERROR_GENERIC;
    }
    if (gp_u1DecrptAddr == NULL)
    {
	    u4AlignSize	= TZ_CACHE_ALIGN(ptDescryptData->u4FrameSize);
		
		u4AlignSize += TZ_CACHE_LINE_SIZE;
		u4AlignSize += TZ_CACHE_LINE_SIZE;
		
        gp_u1DecrptAddr = (UINT8*)malloc(u4AlignSize);
		gp_u1DecrptAlignAddr = (UINT8*)TZ_CACHE_ALIGN((UINT32)gp_u1DecrptAddr);
        
        DMSG("TZ:Apply Addr 0x%x AlignAddr 0x%x u4FrameSize 0x%x u4AlignSize 0x%x\n",
			gp_u1DecrptAddr,
			gp_u1DecrptAlignAddr,
			ptDescryptData->u4FrameSize,
			u4AlignSize);        
    }
	
    if (gp_u1DecrptAlignAddr != NULL)
    {
        if ((ptDescryptData->u4StartAddr >= ptDescryptData->u4BufStart) &&
            (ptDescryptData->u4StartAddr < ptDescryptData->u4BufEnd))
        {
            if ((ptDescryptData->u4StartAddr + ptDescryptData->u4FrameSize) <= ptDescryptData->u4BufEnd)
            {                
            	memcpy_n2s_chk((void*)gp_u1DecrptAlignAddr, (void*)ptDescryptData->u4StartAddr, ptDescryptData->u4FrameSize,NULL);         
            }
            else
            {
                u4FrameSizeA = ptDescryptData->u4BufEnd - ptDescryptData->u4StartAddr;
                u4FrameSizeB = ptDescryptData->u4FrameSize - (ptDescryptData->u4BufEnd - ptDescryptData->u4StartAddr);

            	memcpy_n2s_chk((void*)gp_u1DecrptAlignAddr, (void*)ptDescryptData->u4StartAddr,u4FrameSizeA,NULL);
            	
            	memcpy_n2s_chk((void*)(gp_u1DecrptAlignAddr + u4FrameSizeA) , (void*)ptDescryptData->u4StartAddr,u4FrameSizeB,NULL);            	            	
            }
            
            //decrypt content         
            if (ptDescryptData->eEncryptMode == TZ_ENCRYPT_MODE_LOCAL_KEY)
            {
                if (ptDescryptData->fgFrameHead  /*||  (Changed ) */)
                {
                    if ((gp_u1PlainKey == NULL) || (gp_u1PlainIv == NULL))
                    {
                        gp_u1PlainKey = (UINT8*)malloc(TZ_CACHE_ALIGN(DECRYPT_SECURE_KEY_LEN*2));
                        if (gp_u1PlainKey == NULL)
                        {
                              DMSG("TZ:allocate key buff fail \n");       
                              return FALSE;   
                        }
                        gp_u1PlainIv = gp_u1PlainKey + DECRYPT_SECURE_KEY_LEN;
                    }                

                    TZ_GCPU_ReloadSecureKeyIv(GCPU_KEY_TYPE_DTCP_KL, 0, AES_TZ_SECURE_KEY_PTR, AES_TZ_SECURE_IV_PTR);
                
                    // get plain iv
                    // call api TZ_SVP_AES_CBC_***() to descrypt
                    //don't need save ,dmx will save it.
                    TZ_GCPU_SECURE_AES_CBC(AES_TZ_SECURE_KEY_PTR,16,AES_TZ_SECURE_IV_PTR,
                    		(UINT8*)gp_u1DecrptAlignAddr,
                    		(UINT8*)gp_u1DecrptAlignAddr,
                    		ptDescryptData->u4FrameSize,
                    		FALSE);
                    
                }    
                else
                {
                     // call api TZ_SVP_AES_CBC_***() to descrypt
                     //don't need save ,dmx will save it.
                    TZ_GCPU_SECURE_AES_CBC(AES_TZ_SECURE_KEY_PTR,16,AES_TZ_SECURE_IV_PTR,
                    		(UINT8*)gp_u1DecrptAlignAddr,
                    		(UINT8*)gp_u1DecrptAlignAddr,
                    		ptDescryptData->u4FrameSize,
                    		FALSE);
                     
                }
            }
            else if (ptDescryptData->eEncryptMode == TZ_ENCRYPT_MODE_DTCP_IP_KEY)
            {
            	#if 0
                if (S_GCPU_OK != TZ_SVP_AES_CBC((UINT8*)gp_u1DecrptAlignAddr,(UINT8*)gp_u1DecrptAlignAddr,
                                                ptDescryptData->u4FrameSize,
                                                FALSE,
                                                gp_u1PlainIv,
                                                gp_u1PlainIv))
                {
                    DMSG("TZ:Decrypt content fail\n");      
    				
                    return FALSE;
                }                
				#endif
				
            }
            else
            {
                // nothing
            }
            
            ptDescryptData->u4BufStart  = (UINT32)gp_u1DecrptAlignAddr;
            ptDescryptData->u4BufEnd    = (UINT32)(gp_u1DecrptAlignAddr + ptDescryptData->u4FrameSize + TZ_CACHE_LINE_SIZE);
            ptDescryptData->u4StartAddr = (UINT32)gp_u1DecrptAlignAddr;
            ptDescryptData->u4FrameSize = ptDescryptData->u4FrameSize; 
            DMSG("TZ:Decrypt content success\n");
        }
		
    }
    #endif
    
    return TRUE;
}
static BOOL SvcEx_SWDMX_FreeDecryptBuf(VOID)
{   
    if (gp_u1PlainKey != NULL)
    {
        free(gp_u1PlainKey);
        gp_u1PlainKey = NULL;
        
    }
    if (gp_u1DecrptAddr != NULL)
    {
        free(gp_u1DecrptAddr);
        gp_u1DecrptAddr = NULL;
    }
    
    DMSG("TZ:Free Decrypt success\n");
    return TRUE;
}

extern void BIM_Trustzone_VFIFO(UINT32 *pbase, UINT32 *psize);
extern void BIM_Trustzone_VFIFO2(UINT32 *pbase, UINT32 *psize);

static BOOL SvcEx_DMX_GetSecureVfifo(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_DMX_SECURE_VFIFO_T *pVfifo;

    pVfifo = (TZ_DMX_SECURE_VFIFO_T *)pu1Param;
    BIM_Trustzone_VFIFO(&(pVfifo->u4Addr), &(pVfifo->u4Size));

    DMSG("TZ VFIFO: 0x%x 0x%x \n", pVfifo->u4Addr, pVfifo->u4Size);
    return TRUE;
}

static BOOL SvcEx_DMX_GetSecureVfifo2(UINT8 *pu1Param, UINT32 u4Size)
{
    TZ_DMX_SECURE_VFIFO_T *pVfifo;

    pVfifo = (TZ_DMX_SECURE_VFIFO_T *)pu1Param;
    BIM_Trustzone_VFIFO2(&(pVfifo->u4Addr), &(pVfifo->u4Size));

    DMSG("TZ VFIFO2: 0x%x 0x%x \n", pVfifo->u4Addr, pVfifo->u4Size);
    return TRUE;
}


#define ChkLengthEQ(len,len_shouldbe) do {         \
            if( !(len == len_shouldbe) )              \
            {                           \
                check_flag = FALSE;             \
                DMSG("cmd %d params check length error %d != %d\n",u4Cmd , len, len_shouldbe);             \
                goto ErrorExit;         \
            }                           \
        } while(FALSE)

#define ChkLengthBG(len,len_shouldbe) do {         \
						if( !(len >= len_shouldbe) )			  \
						{							\
							check_flag = FALSE; 			\
							DMSG("cmd %d params check length error %d < %d\n",u4Cmd , len, len_shouldbe);			   \
							goto ErrorExit; 		\
						}							\
					} while(FALSE)

static BOOL SvcEx_TEE_CheckParamsLength(TEE_COMMON_ARG_T *ptCommonArg,UINT32 u4Cmd)
{
BOOL check_flag = TRUE;
switch (u4Cmd)
	{
    case TZCTL_TEE_PLAYREADY_BASE:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_ECCP256_KEYPAIR_GEN:
		ChkLengthEQ(ptCommonArg->ui4Length[0],64);
		ChkLengthEQ(ptCommonArg->ui4Length[1],32);
		break;
    case TZCTL_TEE_PLAYREADY_ECCP256_KEY_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_ECDSAP256_SIGN:
		ChkLengthBG(ptCommonArg->ui4Length[1],64); //signature length should be bigger than 64
        break;
    case TZCTL_TEE_PLAYREADY_ECCP256_DECRYPT:
		ChkLengthBG(ptCommonArg->ui4Length[0],32);
		ChkLengthBG(ptCommonArg->ui4Length[1],32);
        break;
    case TZCTL_TEE_PLAYREADY_COPYBYTE:
        check_flag = TRUE;
        break;
	case TZCTL_TEE_PLAYREADY_LINKBYTE:
		check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_PROVISIONED_DATA_GET:
        check_flag = TRUE;
        break;
	case TZCTL_TEE_PLAYREADY_GET_KFKEY:
		ChkLengthBG(ptCommonArg->ui4Length[1],(16 + 16));
		break;
    case TZCTL_TEE_PLAYREADY_OMAC1_KEY_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_OMAC1_VERIFY:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_OMAC1_SIGN:
		ChkLengthBG(ptCommonArg->ui4Length[3],16);
        break;
    case TZCTL_TEE_PLAYREADY_CONTENTKEY_AESCTR_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_CONTENTKEY_AESCTR_DMX_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_CONTENT_AESCTR_DECRYPT:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_MULTI_CONTENT_AESCTR_DECRYPT:
        check_flag = TRUE;
        break;
	case TZCTL_TEE_PLAYREADY_DASH_SAMPLE_AESCTR_DECRYPT:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_AESECB_KEY_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_AESECB_ENCRYPT:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_AESECB_DECRYPT:
        check_flag = TRUE;
        break;
/***********************************************************************/
    case TZCTL_TEE_PLAYREADY_AESCBC_KEY_SET:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_AESCBC_ENCRYPT:
        check_flag = TRUE;
        break;
    case TZCTL_TEE_PLAYREADY_AESCBC_DECRYPT:
        check_flag = TRUE;
        break;
	case TZCTL_TEE_PLAYREADY_UNSHUFFLE_CONTENTKEY:
		check_flag = TRUE;
		break;
    case  TZCTL_TEE_PLAYREADY_PREPARECONTENTKEY_PRIME:
		check_flag = TRUE;
		break;	
	}
ErrorExit:
return check_flag;
}

extern BOOL TZ_FBM_Initialization(void *prKernParam  , UINT32 u4Size);
/* ============================================================================
    Exported Entry Points
============================================================================ */
UINT32 SvcEx_TZCTL(TZ_SMC_ARG_T const *prSMCArg)
{
    TEE_COMMON_ARG_T *ptCommonArg = NULL;
    BOOL fgRet = TRUE;
	int ss_return = 0; // only for playready securestop error code usage 

    //DMSG("cmd=0x%x, addr=0x%x, size=0x%x\n", prSMCArg->u4Cmd, prSMCArg->u4Addr, prSMCArg->u4Size);

    TZ_SWD_BufferStart(prSMCArg->u4Addr, TZ_CACHE_ALIGN(prSMCArg->u4Size));
	
	SvcEx_TEE_CheckParamsLength((TEE_COMMON_ARG_T *)prSMCArg->u4Addr,prSMCArg->u4Cmd);
    switch (prSMCArg->u4Cmd)
    {
    case TZCTL_SYSTEM_ECHO:
        break;
    case TZCTL_SYSTEM_ENABLE_DEBUG:
        enableDebug((UINT32)prSMCArg->u4Addr);
        break;
    case TZCTL_SYSTEM_REVERSE_ARRAY:
        SvcEx_ReverseArray((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;

	case TZCTL_QUERY_TVP_ENABLED:
		(*(UINT32*)prSMCArg->u4Addr)=(UINT32)(TZ_TVP_QUERY_ENABLED());
		break;
		
#ifndef CC_NDEBUG
    case TZCTL_SYSTEM_IO_WRITE32:
        SvcEx_TZ_IO_WRITE32((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SYSTEM_IO_WRITE16:
        SvcEx_TZ_IO_WRITE16((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SYSTEM_IO_WRITE8:
        SvcEx_TZ_IO_WRITE8((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SYSTEM_IO_READ32:
        SvcEx_TZ_IO_READ32((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SYSTEM_IO_READ16:
        SvcEx_TZ_IO_READ16((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SYSTEM_IO_READ8:
        SvcEx_TZ_IO_READ8((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
#endif // CC_NDEBUG

    case TZCTL_WV_SET_ASSET_KEY:
        fgRet = SvcEx_DRM_WV_SetAssetkey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_WV_SET_DERIVE_CW:
        fgRet = SvcEx_DRM_WV_DeriveCW((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_WV_PASS_KEYBOX:
        fgRet = SvcEx_DRM_WV_PassKeybox((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;

///////////////////////////////////for hulu+dash+wv drm only begin//////////////
#ifdef CC_ENABLE_WIDEVINE_EME
	case TZCTL_WV_VALIDATE_KEYBOX:
		fgRet = SvcEx_DRM_WV_ValidateKeybox((UINT32 *)prSMCArg->u4Addr);
		break;
	case TZCTL_WV_DESTROYSESSION:
        fgRet = SvcEx_DRM_WV_DestroySession((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
	case TZCTL_WV_CREATESESSION:
		fgRet = SvcEx_DRM_WV_CreatSession((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERATE_NONCE:
		fgRet = SvcEx_DRM_WV_GenerateNonceSess((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERATE_DERIVEKEY:
		fgRet = SvcEx_DRM_WV_GenerateDeriveKeys((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERATE_SIGNATURE:
		fgRet = SvcEx_DRM_WV_GenerateSignature((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_VALIDATEM_MESSAGE:
		fgRet = SvcEx_DRM_WV_ValidateMessage((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_INSTALL_KEY:
		fgRet = SvcEx_DRM_WV_InstallKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_FLUSH_NONCE:
		fgRet = SvcEx_DRM_WV_FlushNoncesSess((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_UPDATE_MACKEY:
		fgRet = SvcEx_DRM_WV_UpdateMacKeys((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_REFRESH_KEY:
		fgRet = SvcEx_DRM_WV_RefreshKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_SELECT_CONTENT_KEY:
		fgRet = SvcEx_DRM_WV_SelectContentKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_CHK_CTR_KEY:
		fgRet = SvcEx_DRM_WV_CheckDecCTRContentKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_SAVE_KEYBOX:
		fgRet = SvcEx_DRM_WV_SaveKeybox((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GET_RANDOM_NUM:
		fgRet = SvcEx_DRM_WV_GetRandomNum((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_REWRAP_RSAKEY:
		fgRet = SvcEx_DRM_WV_RewrapDeviceRSAKeySess((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_LOAD_RSAKET:
		fgRet = SvcEx_DRM_WV_LoadDeviceRSAKeySessCtx((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERATE_RSASIGN:
		fgRet = SvcEx_DRM_WV_GenerateRSASignature((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_RSA_DERIVEKEY:
		fgRet = SvcEx_DRM_WV_RSADeriveKeys((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_CHK_CBC_KEY:
		fgRet = SvcEx_DRM_WV_CheckEncDecCBCContentKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
		
	#if 0
	case TZCTL_WV_CHK_DEC_CTR:
		fgRet = SvcEx_DRM_WV_DecryptCTR((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERIC_ENC:
		fgRet = SvcEx_DRM_WV_Generic_Encrypt((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERIC_DEC:
		fgRet = SvcEx_DRM_WV_Generic_Decrypt((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	#endif
	
	case TZCTL_WV_GENERIC_SIGN:
		fgRet = SvcEx_DRM_WV_Generic_Sign((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
	case TZCTL_WV_GENERIC_VERIFY:
		fgRet = SvcEx_DRM_WV_Generic_Verify((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
		break;
#endif
///////////////////////////////////for hulu+dash+wv drm only begin//////////////

    case TZCTL_DRM_DIVX_GETHWSECRETKEY0:
        fgRet = SvcEx_DRM_DIVX_GetHwSecretKey0((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DRM_DIVX_GETHWSECRETKEY1:
        fgRet = SvcEx_DRM_DIVX_GetHwSecretKey1((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;

#ifdef CC_ENABLE_HCI
    case 0x9000:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_DataDecrypt(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9001:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_KeyDataGetIfStarfish(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9002:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_ExportKey(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9003:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_StarfishIdGet(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9004:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_ImportStarfishKey(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9005:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_HashCheck(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9006:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_KeyRefDestroy(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9007:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_CreateSACKeyRef(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9008:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_DataSign(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9009:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_StreamDecrypt(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900a:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_GenerateContentKey(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900b:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_EncryptStreamWithIV(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900c:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_KeyDataEncrypt(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900d:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_LoadKey(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900e:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_GenerateHash(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x900f:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_HmacCheck(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9010:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_PersonalityKeyCheck(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9011:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_ImportSas(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9012:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_GenerateHMAC(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9013:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_ImportBKBContainer(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9014:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_ImportPpprocData(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9015:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_EncryptData(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case 0x9016:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = SvcEx_HCI_DecryptStreamWithOffset(ptCommonArg);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_HCI_INIT:
        fgRet = SvcEx_HCI_Init((MTDMX_MWIF_HCI_INIT_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_FIN:
        fgRet = SvcEx_HCI_Fin((MTDMX_MWIF_HCI_INIT_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_LOCK:
        fgRet = SvcEx_HCI_Lock((MTDMX_MWIF_HCI_LOCK_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_UNLOCK:
        fgRet = SvcEx_HCI_Unlock((MTDMX_MWIF_HCI_UNLOCK_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_KEYREF_CREATE:
        fgRet = SvcEx_HCI_KeyRefCreate((MTDMX_MWIF_HCI_KEY_REF_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_KEYREF_CHECK:
        fgRet = SvcEx_HCI_KeyRefCheck((MTDMX_MWIF_HCI_KEY_REF_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_KEYREF_DESTROY:
        //fgRet = SvcEx_HCI_KeyRefDestroy((MTDMX_MWIF_HCI_KEY_REF_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DEVICE_GET_NUM:
        //fgRet = SvcEx_HCI_DevIdGetNum((MTDMX_MWIF_HCI_ID_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DEVICE_GET_SIZE:
        fgRet = SvcEx_HCI_DevIdGetSize((MTDMX_MWIF_HCI_ID_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DEVICE_GET_NAME:
        fgRet = SvcEx_HCI_DevIdGetName((MTDMX_MWIF_HCI_ID_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_IMPORT_KEY:
        fgRet = SvcEx_HCI_ImportKey((MTDMX_MWIF_HCI_IMPORT_KEY_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_EXPORT_KEY_SIZE:
        //fgRet = SvcEx_HCI_ExportKeySize((MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_EXPORT_KEY:
        //fgRet = SvcEx_HCI_ExportKey((MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_GET_START_FISH_ID_SIZE:
        //fgRet = SvcEx_HCI_StarfishIdGetSize((MTDMX_MWIF_HCI_ID_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_GET_START_FISH_ID:
        //fgRet = SvcEx_HCI_StarfishIdGet((MTDMX_MWIF_HCI_ID_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_GET_START_FISH_KEYDATA_SIZE:
        //fgRet = SvcEx_HCI_KeyDataGetIfStarfishSize((MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_GET_START_FISH_KEYDATA:
        //fgRet = SvcEx_HCI_KeyDataGetIfStarfish((MTDMX_MWIF_HCI_GET_STARFISH_KEYDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_IMPORT_START_FISH_KEY:
        fgRet = SvcEx_HCI_ImportStarfishKey((MTDMX_MWIF_HCI_IMPORT_STARFISH_KEY_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DATA_SIGN_SIZE:
        //fgRet = SvcEx_HCI_DataSignSize((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DATA_SIGN:
        //fgRet = SvcEx_HCI_DataSign((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DATA_DECRYPT_SIZE:
        //fgRet = SvcEx_HCI_DataDecryptSize((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_DATA_DECRYPT:
        //fgRet = SvcEx_HCI_DataDecrypt((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_STREAM_DECRYPT_SIZE:
        //fgRet = SvcEx_HCI_StreamDecryptSize((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_STREAM_DECRYP:
        //fgRet = SvcEx_HCI_StreamDecrypt((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_KEYDATA_ENCRYPT_SIZE:
        //fgRet = SvcEx_HCI_KeyDataEncryptSize((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_KEYDATA_ENCRYPT:
        //fgRet = SvcEx_HCI_KeyDataEncrypt((MTDMX_MWIF_HCI_DATA_OP_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_HMAC_CHECK:
        //fgRet = SvcEx_HCI_HmacCheck((MTDMX_MWIF_HCI_HASH_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_HASH_CHECK:
        //fgRet = SvcEx_HCI_HashCheck((MTDMX_MWIF_HCI_HASH_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_MOVE_KEY_SIZE:
        fgRet = SvcEx_HCI_MoveKeySize((MTDMX_MWIF_HCI_MOVEKEY_T *)prSMCArg->u4Addr);
        break;
    //case TZCTL_HCI_FREEMEM:
    //    fgRet = SvcEx_HCI_FreeMem((MTDMX_MWIF_HCI_FREEMEM_T *)prSMCArg->u4Addr);
    //    break;
    case TZCTL_HCI_MOVE_KEY:
        fgRet = SvcEx_HCI_MoveKey((MTDMX_MWIF_HCI_MOVEKEY_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_PERSONALITYKEY_LOAD:
        fgRet = SvcEx_HCI_PersonalityKeyLoad((MTDMX_MWIF_HCI_PERSONALITY_KEY_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_PERSONALITYKEY_CHECK:
        //fgRet = SvcEx_HCI_PersonalityKeyCheck((MTDMX_MWIF_HCI_PERSONALITY_KEY_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_IMPORT_SAS_SIZE:
        //fgRet = SvcEx_HCI_ImportSasSize((MTDMX_MWIF_HCI_IMPORT_SAS_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_IMPORT_SAS:
        //fgRet = SvcEx_HCI_ImportSas((MTDMX_MWIF_HCI_IMPORT_SAS_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_HCI_IMPORT_SAS_IDS:
        //fgRet = SvcEx_HCI_ImportSasIds((MTDMX_MWIF_HCI_IMPORT_SAS_T *)prSMCArg->u4Addr);
        break;
#endif // CC_ENABLE_HCI

#ifdef CC_ENABLE_SKB
    case TZCTL_SKB_GET_INSTANCE:
        fgRet = SvcEx_SKB_GetInstance((MTDMX_MWIF_SKB_ENGINE_GET_INSTANCE_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_RELEASE:
        fgRet = SvcEx_SKB_Engine_Release((MTDMX_MWIF_SKB_ENGINE_RELEASE_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_GET_INFO:
        fgRet = SvcEx_SKB_GetInfo((MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_GET_PROPERTIES_SIZE:
        fgRet = SvcEx_SKB_GetProertiesSize((MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_GET_PROPERTIES_DATA:
        fgRet = SvcEx_SKB_GetProertiesData((MTDMX_MWIF_SKB_ENGINE_GET_INFO_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CREATE_DATA_FROM_WRAPPED:
        fgRet = SvcEx_SKB_DataFromWrapped((MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_WRAPPED_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CREATE_DATA_FROM_EXPORTED:
        fgRet = SvcEx_SKB_DataFromExported((MTDMX_MWIF_SKB_ENGINE_CREATE_DATA_FROM_EXPORTED_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CREATE_TRANSFORM:
        fgRet = SvcEx_SKB_CreateTransform((MTDMX_MWIF_SKB_ENGINE_CREATE_TRANSFORM_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CREATE_CIPHER:
        fgRet = SvcEx_SKB_CreateCipher((MTDMX_MWIF_SKB_ENGINE_CREATE_CIPHER_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_SECUREDATA_RELEASE:
        fgRet = SvcEx_SKB_SecureData_Release((MTDMX_MWIF_SKB_SECUREDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_SECUREDATA_GETINFO:
        fgRet = SvcEx_SKB_SecureDataGetInfo((MTDMX_MWIF_SKB_SECUREDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_SECUREDATA_EXPORT:
        fgRet = SvcEx_SKB_SecureData_Export((MTDMX_MWIF_SKB_SECUREDATA_EXPORT_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_SECUREDATA_DERIVE:
        fgRet = SvcEx_SKB_SecureData_Derive((MTDMX_MWIF_SKB_SECUREDATA_DERIVE_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_TRANSFORM_RELEASE:
        fgRet = SvcEx_SKB_Transform_Release((MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_TRANSFORM_ADDBYTES:
        fgRet = SvcEx_SKB_Transform_AddBytes((MTDMX_MWIF_SKB_TRANSFORM_ADDBYTES_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_TRANSFORM_ADDSECUREDATA:
        fgRet = SvcEx_SKB_Transform_AddSecureData((MTDMX_MWIF_SKB_TRANSFORM_ADDSECUREDATA_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_TRANSFORM_GETOUTPUT:
        fgRet = SvcEx_SKB_Transform_GetOutput((MTDMX_MWIF_SKB_TRANSFORM_GETOUTPUT_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CIPHER_PROCESSBUFFER:
        fgRet = SvcEx_SKB_Cipher_ProcessBuffer((MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *)prSMCArg->u4Addr);
        break;
    case TZCTL_SKB_CIPHER_RELEASE:
        fgRet = SvcEx_SKB_Cipher_Release((MTDMX_MWIF_SKB_CIPHER_PROCESSBUFFER_T *)prSMCArg->u4Addr);
        break;
#endif // CC_ENABLE_SKB

#ifdef CC_ENABLE_FUJ_MARLIN
    case TZCTL_DRM_TZ_LOADSECRETDATATYPE:
        fgRet = SvcEx_DRM_TZ_LoadSecretDataType((MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DRM_TZ_SETSECRETDATATYPE:
        fgRet = SvcEx_DRM_TZ_SetSecretDataType((MTDMX_MWIF_DRM_TZ_SETSECRETDATATYPE_T *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DRM_TZ_OPEN:
        fgRet = SvcEx_DRM_TZ_Open((MTDMX_MWIF_DRM_TZ_OPEN_T *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DRM_TZ_CLOSE:
        fgRet = SvcEx_DRM_TZ_Close((MTDMX_MWIF_DRM_TZ_OPEN_T *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DRM_TZ_DECODEPACKETSVOD:
        fgRet = SvcEx_DRM_TZ_DecodePacketsVOD((MTDMX_MWIF_DRM_TZ_DECODEPACKETS_VOD_T *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
#endif //  CC_ENABLE_FUJ_MARLIN


#ifdef CC_WIDEVINE_L1_SUPPORT
    case TZCTL_OEMCrypto_Initialize:
        fgRet = TZ_OEMCrypto_Initialize(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_Terminate:
        fgRet = TZ_OEMCrypto_Terminate(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_OpenSession:
        fgRet = TZ_OEMCrypto_OpenSession(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_CloseSession:
        fgRet = TZ_OEMCrypto_CloseSession(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GenerateDerivedKeys:
        fgRet = TZ_OEMCrypto_GenerateDerivedKeys(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GenerateNonce:
        fgRet = TZ_OEMCrypto_GenerateNonce(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GenerateSignature:
        fgRet = TZ_OEMCrypto_GenerateSignature(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_LoadKeys:
        fgRet = TZ_OEMCrypto_LoadKeys(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_RefreshKeys:
        fgRet = TZ_OEMCrypto_RefreshKeys(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_SelectKey:
        fgRet = TZ_OEMCrypto_SelectKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DecryptCTR:
        fgRet = TZ_OEMCrypto_DecryptCTR(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_InstallKeybox:
        fgRet = TZ_OEMCrypto_InstallKeybox(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_IsKeyboxValid:
        fgRet = TZ_OEMCrypto_IsKeyboxValid(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GetDeviceID:
        fgRet = TZ_OEMCrypto_GetDeviceID(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GetKeyData:
        fgRet = TZ_OEMCrypto_GetKeyData(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GetRandom:
        fgRet = TZ_OEMCrypto_GetRandom(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_WrapKeybox:
        fgRet = TZ_OEMCrypto_WrapKeybox(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_RewrapDeviceRSAKey:
        fgRet = TZ_OEMCrypto_RewrapDeviceRSAKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_LoadDeviceRSAKey:
        fgRet = TZ_OEMCrypto_LoadDeviceRSAKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_GenerateRSASignature:
        fgRet = TZ_OEMCrypto_GenerateRSASignature(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DeriveKeysFromSessionKey:
        fgRet = TZ_OEMCrypto_DeriveKeysFromSessionKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_Generic_Encrypt:
        fgRet = TZ_OEMCrypto_Generic_Encrypt(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_Generic_Decrypt:
        fgRet = TZ_OEMCrypto_Generic_Decrypt(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_Generic_Sign:
        fgRet = TZ_OEMCrypto_Generic_Sign(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_Generic_Verify:
        fgRet = TZ_OEMCrypto_Generic_Verify(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_UpdateUsageTable:
        fgRet = TZ_OEMCrypto_UpdateUsageTable(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DeactivateUsageEntry:
        fgRet = TZ_OEMCrypto_DeactivateUsageEntry(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_ReportUsage:
        fgRet = TZ_OEMCrypto_ReportUsage(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DeleteUsageEntry:
        fgRet = TZ_OEMCrypto_DeleteUsageEntry(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DeleteUsageTable:
        fgRet = TZ_OEMCrypto_DeleteUsageTable(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;    
    case TZCTL_OEMCrypto_GetUsageTableWriteRequest:
        fgRet = TZ_OEMCrypto_GetUsageTableWriteRequest(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;    
    case TZCTL_OEMCrypto_VerifyWriteResponse:
        fgRet = TZ_OEMCrypto_VerifyWriteResponse(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;    
    case TZCTL_OEMCrypto_GetUsageTableReadRequest:
        fgRet = TZ_OEMCrypto_GetUsageTableReadRequest(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;    
    case TZCTL_OEMCrypto_VerifyReadResponse:
        fgRet = TZ_OEMCrypto_VerifyReadResponse(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;    
    case TZCTL_OEMCrypto_SetEntitlementKey:
        fgRet = TZ_OEMCrypto_SetEntitlementKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DeriveControlWord:
        fgRet = TZ_OEMCrypto_DeriveControlWord(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DecryptVideo:
        fgRet = TZ_OEMCrypto_DecryptVideo(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_OEMCrypto_DecryptAudio:
        fgRet = TZ_OEMCrypto_DecryptAudio(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_RPMB_LOAD_MAC_KEY:
        fgRet = TZ_RPMB_LOAD_KEY(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_RPMB_GET_WCNT_REQ:
        fgRet = TZ_RPMB_GET_WCNT_REQ(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_RPMB_SET_WCNT_RESP:
        fgRet = TZ_RPMB_SET_WCNT_RESP(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;      
#endif

    case TZCTL_SEC_BUF_OPEN_SESSION:
        fgRet = TZ_SEC_BUF_OPEN_SESSION(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_CLOSE_SESSION:
        fgRet = TZ_SEC_BUF_CLOSE_SESSION(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_ALLOC:
        fgRet = TZ_SEC_BUF_ALLOC(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_FREE:
        fgRet = TZ_SEC_BUF_FREE(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_FLUSH:
        fgRet = TZ_SEC_BUF_FLUSH(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_FRAGMENT_ALLOC:
        fgRet = TZ_SEC_BUF_FRAGMENT_ALLOC(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_FRAGMENT_FREE:
        fgRet = TZ_SEC_BUF_FRAGMENT_FREE(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_CPB_MOVEDATA:
        fgRet = TZ_SEC_BUF_CPB_MOVE_DATA(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SEC_BUF_CPB_GETDATA:
        fgRet = TZ_SEC_BUF_CPB_GET_DATA(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;        
#ifdef CC_ENABLE_HDCP2
    case TZCTL_HDCP2_SET_ENC_LC128:
        fgRet = SvcEx_MTDRM_HDCP2_SetEncLc128(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_SET_ENC_KEY:
        fgRet = SvcEx_MTDRM_HDCP2_SetEncKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_GETCERTINFO:
        fgRet = SvcEx_MTDRM_HDCP2_GetCertInfo(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_DECRYPT_RSAESOAEP:
        fgRet = SvcEx_MTDRM_HDCP2_DecryptRSAESOAEP(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_KDKEYDERIVATION:
        fgRet = SvcEx_MTDRM_HDCP2_KdKeyDerivation(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_COMPUTE_HPRIME:
        fgRet = SvcEx_MTDRM_HDCP2_ComputeHprime(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_COMPUTE_LPRIME:
        fgRet = SvcEx_MTDRM_HDCP2_ComputeLprime(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_COMPUTE_KH:
        fgRet = SvcEx_MTDRM_HDCP2_ComputeKh(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_ENCRYPT_KM:
        fgRet = SvcEx_MTDRM_HDCP2_EncryptKmUsingKh(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_DECRYPT_KM:
        fgRet = SvcEx_MTDRM_HDCP2_DecryptKmUsingKh(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_DECRYPT_EKS:
        fgRet = SvcEx_MTDRM_HDCP2_DecryptEKs(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_KSLC128XOR:
        fgRet = SvcEx_MTDRM_HDCP2_KsXorLC128(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_SETRIV_PID:
        fgRet = SvcEx_MTDRM_HDCP2_SetRiv_Pid(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_PRESETKSLC128:
        fgRet = SvcEx_MTDRM_HDCP2_PresetKsLC128(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_GENERATE_KM:
        fgRet = SvcEx_MTDRM_HDCP2_Generate_Km(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_GENERATE_KS:
        fgRet = SvcEx_MTDRM_HDCP2_Generate_Ks(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_SET_ENC_KM:
        fgRet = SvcEx_MTDRM_HDCP2_SetEncKm(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_GET_ENC_KM:
        fgRet = SvcEx_MTDRM_HDCP2_GetEncKm(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;

    

	/* for HDCP 2.2 */
    case TZCTL_HDCP2_2_KDKEYDERIVATION:
        fgRet = SvcEx_MTDRM_HDCP2_2_KdKeyDerivation(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_2_COMPUTE_HPRIME:
        fgRet = SvcEx_MTDRM_HDCP2_2_ComputeHprime(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_HDCP2_2_COMPUTE_LPRIME:
        fgRet = SvcEx_MTDRM_HDCP2_2_ComputeLprime(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;


	/* for HDCP2.X ALIGN*/
    case TZCTL_HDCP2_SET_ENC_KEYSET:
    fgRet = SvcEx_MTDRM_HDCP2_SetEncKeySet(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

	/* for HDCP2.X TX*/
    case TZCTL_HDCP2_RETRVE_KH_KM:
    	fgRet = SvcEx_MTDRM_HDCP2_RetrvEkh_km(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_SAVE_PAIRINGINFO:
    	fgRet = SvcEx_MTDRM_HDCP2_SavePairingInfo(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_CHECK_RXID:
    	fgRet = SvcEx_MTDRM_HDCP2_CheckRxID(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_COMPUTE_MPRIME:
    	fgRet = SvcEx_MTDRM_HDCP2_ComputeMprime(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_COMPUTE_VPRIME:
    	fgRet = SvcEx_MTDRM_HDCP2_ComputeVprime(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_2_COMPUTE_VHPRIME:
    	fgRet = SvcEx_MTDRM_HDCP2_2_ComputeVprime(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_VERIFY_SIGNATURE:
    	fgRet = SvcEx_MTDRM_HDCP2_VerifySignature(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;	

	
    case TZCTL_HDCP2_GENERATE_KM_FOR_TX:
        fgRet = SvcEx_MTDRM_HDCP2_Generate_Km_for_tx(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;


	
	case TZCTL_HDCP2_SET_KEY_TYPE:
		fgRet = SvcEx_MTDRM_HDCP2_UseTestKey_InKernel(prSMCArg->u4Addr, prSMCArg->u4Size);
	break;


    case TZCTL_HDCP2_ResetPairingInfo:
    	fgRet = SvcEx_MTDRM_HDCP2_ResetPairingInfo(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;
	case TZCTL_HDCP2_HMAC_SHA256	:
		fgRet = SvcEx_MTDRM_HDCP2_HMAC_SHA256(prSMCArg->u4Addr, prSMCArg->u4Size);
		break;


	case TZCTL_HDCP2_SW_DecryptData:
		fgRet = SvcEx_MTDRM_HDCP2_SW_DecryptData(prSMCArg->u4Addr, prSMCArg->u4Size);
		break;

	case TZCTL_HDCP2_ENABLE_TZ_KEY:
		fgRet = SvcEx_MTDRM_HDCP2_enable_tz_key(prSMCArg->u4Addr, prSMCArg->u4Size);
	break;

    case TZCTL_HDCP2_GetPDKey:
    	fgRet = SvcEx_MTDRM_HDCP2_GetPDKey(prSMCArg->u4Addr, prSMCArg->u4Size);
    break;

    case TZCTL_HDCP2_SetPDKey:
        fgRet = SvcEx_MTDRM_HDCP2_SetPDKey(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;


    #endif


#ifdef CC_ENABLE_IPTVES
	/*sony iptves */

	/* sac */
	case TZCTL_IPTVES_SAC_CONSTRUCT:
        fgRet = SvcEx_MTDRM_IPTVES_SAC_CONSTRUCT(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;		

	case TZCTL_IPTVES_SAC_DESTRUCT:
        fgRet = SvcEx_MTDRM_IPTVES_SAC_DESTRUCT(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;		


	case TZCTL_IPTVES_SAC_MAKEMSG:
        fgRet = SvcEx_MTDRM_IPTVES_SAC_MKMSG(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;		

	case TZCTL_IPTVES_SAC_PROCESSMSG:
        fgRet = SvcEx_MTDRM_IPTVES_SAC_PROCESSMSG(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;		


	/* sdi */
	case TZCTL_IPTVES_SDI_INIT:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_INIT(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
									 
	case TZCTL_IPTVES_SDI_FIN:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_FIN(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
															 
	case TZCTL_IPTVES_SDI_CleanupNVS:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_CleanupNVS(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
													   
	case TZCTL_IPTVES_SDI_GetDeviceID:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_GetDeviceID(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
													  
	case TZCTL_IPTVES_SDI_VerifyServiceMetaData:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_VerifyServiceMetaData(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
										
	case TZCTL_IPTVES_SDI_JudgeCRLUpdate:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_JudgeCRLUpdate(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
												 
	case TZCTL_IPTVES_SDI_GetCRLDstPoint:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_GetCRLDstPoint(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
											   
	case TZCTL_IPTVES_SDI_SetCRL:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_SetCRL(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
													   
	case TZCTL_IPTVES_SDI_CheckTrustedTime:
			fgRet = SvcEx_MTDRM_IPTVES_SDI_CheckTrustedTime(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
											 
													 
													 
	/* ecm */										 
	case TZCTL_IPTVES_ECM_ExtractScrambleKey:
			fgRet = SvcEx_MTDRM_IPTVES_ECM_ExtractScrambleKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
										   
	case TZCTL_IPTVES_ECM_GetScrambleKey:
			fgRet = SvcEx_MTDRM_IPTVES_ECM_GetScrambleKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
												   
													 
													 
	/* drm */										 
	case TZCTL_IPTVES_DRM_ExtractContentKey:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_ExtractContentKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
											
	case TZCTL_IPTVES_DRM_DelContentKey:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_DelContentKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
												
	case TZCTL_IPTVES_DRM_DelUncommittedKey:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_DelUncommittedKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
											
	case TZCTL_IPTVES_DRM_GetKeyInfo:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_GetKeyInfo(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
														 
	case TZCTL_IPTVES_DRM_GetKeyNum:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_GetKeyNum(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
														 
	case TZCTL_IPTVES_DRM_GetKeyRefList:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_GetKeyRefList(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
											   
	case TZCTL_IPTVES_DRM_GetContentKey:
			fgRet = SvcEx_MTDRM_IPTVES_DRM_GetContentKey(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		
							

	/* nvs memory */
	case TZCTL_IPTVES_NVS_MEM_Init:
			fgRet = SvcEx_MTDRM_IPTVES_NVS_MEM_Init(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		

	case TZCTL_IPTVES_NVS_MEM_Fin:
			fgRet = SvcEx_MTDRM_IPTVES_NVS_MEM_Fin(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		

	case TZCTL_IPTVES_NVS_MEM_GetRWLen:
			fgRet = SvcEx_MTDRM_IPTVES_NVS_MEM_GetRWLen(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;		

	/* time */
	case TZCTL_IPTVES_TIME_SetTrustedTime:
			fgRet = SvcEx_MTDRM_IPTVES_TIME_SetTrustedTime(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;	

	case TZCTL_IPTVES_TIME_GetTrustedTime:
			fgRet = SvcEx_MTDRM_IPTVES_TIME_GetTrustedTime(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;	
	
	case TZCTL_IPTVES_TIME_Cleanup:
			fgRet = SvcEx_MTDRM_IPTVES_TIME_Cleanup(prSMCArg->u4Addr, prSMCArg->u4Size);
			break;	



#endif



    case TZCTL_TEE_BASE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeCommon(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_SECURESTORAGE_M2M_ENC:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeSecureStorageEnc((TEE_SECURESTORAGE_USERKEY_T *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], (long)ptCommonArg->ui4Length[1], ptCommonArg->apvArgument[2], (long *)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_SECURESTORAGE_M2M_DEC:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeSecureStorageDec((TEE_SECURESTORAGE_USERKEY_T *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], (long)ptCommonArg->ui4Length[1], ptCommonArg->apvArgument[2], (long *)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_BASE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxBaseUT();
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_READ_IDFILE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxReadIdfile(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], (UINT32 *)ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KPE_CIPHER_ENCRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxCipherEncrypt(NULL, 0, ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], ptCommonArg->apvArgument[2], ptCommonArg->ui4Length[2], (UINT32 *)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KPE_CIPHER_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxCipherDecrypt(NULL, 0, ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], ptCommonArg->apvArgument[2], ptCommonArg->ui4Length[2], (UINT32 *)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KPH_HMAC:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxHMAC(NULL, 0, ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KCE_CIPHER_ENCRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxCipherEncrypt(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], ptCommonArg->ui4Length[2], ptCommonArg->apvArgument[3], ptCommonArg->ui4Length[3], (UINT32 *)ptCommonArg->apvArgument[4]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KCE_CIPHER_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxCipherDecrypt(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], ptCommonArg->ui4Length[2], ptCommonArg->apvArgument[3], ptCommonArg->ui4Length[3], (UINT32 *)ptCommonArg->apvArgument[4]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_KCH_HMAC:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxHMAC(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_DH_PUBKEY_GEN:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
         fgRet = !_teeNflxDHPublicKey(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], (UINT32 *)ptCommonArg->apvArgument[1], NULL);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_DH_SECRET2KCEKCH:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
         fgRet = !_teeNflxDHSharedSecret(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], NULL, 0);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_GET_RANDOM_BYTES:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxGetRandomBytes(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_DH_PUBKEY_GEN_V2:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxDHPublicKey(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], (UINT32 *)ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_NFLX_DH_SECRET2KCEKCH_V2:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeNflxDHSharedSecret(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], ptCommonArg->ui4Length[2]);
        _teeCommonLeave(ptCommonArg);
        break;

	case TZCTL_TEE_NFLX_MSL_BASE:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = _teeNflxMSLEntry(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1],ptCommonArg->ui4Length[1]);
	_teeCommonLeave(ptCommonArg);
	break;	
case TZCTL_TEE_NFLX_MSL_STOREIDFILE:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_teeNflxStoreIdfile(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0]);
	_teeCommonLeave(ptCommonArg);
	break;
		
    case TZCTL_TEE_PLAYREADY_BASE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyBaseUT(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_ECCP256_KEYPAIR_GEN:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyECCP256GenKeyPair(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_ECCP256_KEY_SET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyECCP256SetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_ECDSAP256_SIGN:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyECDSAP256Sign(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_ECCP256_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyECCP256Decrypt(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_COPYBYTE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyCopyBytes(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]), *((UINT32 *)ptCommonArg->apvArgument[3]), *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_LINKBYTE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyLinkBytes(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]), ptCommonArg->apvArgument[3], *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_PROVISIONED_DATA_GET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyGetProvisionData(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], *((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_GET_KFKEY:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyGetKFKEY(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], *((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
		break;
    case TZCTL_TEE_PLAYREADY_OMAC1_KEY_SET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyOMAC1SetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_OMAC1_VERIFY:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyOMAC1Verify(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), ptCommonArg->apvArgument[3], *((UINT32 *)ptCommonArg->apvArgument[4]), (BOOL *)ptCommonArg->apvArgument[5]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_OMAC1_SIGN:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyOMAC1Sign(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), ptCommonArg->apvArgument[3], *((BOOL *)ptCommonArg->apvArgument[4]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_CONTENTKEY_AESCTR_SET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyContentSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]),PLAYREADY_CONTENTKEY_VIDEO);
        _teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_AUDIOCONTENTKEY_AESCTR_SET:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		fgRet = !_teePlayReadyContentSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]),PLAYREADY_CONTENTKEY_AUDIO);
		_teeCommonLeave(ptCommonArg);
		break;
	case TZCTL_TEE_NAUTILUS_CONTENTKEY_AESCTR_SET:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		fgRet = !_teePlayReadyContentSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]),NAUTILUS_CONTENTKEY_VIDEO);
		_teeCommonLeave(ptCommonArg);
		break;
    case TZCTL_TEE_NAUTILUS_AUDIOCONTENTKEY_AESCTR_SET:
	    ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	    _teeCommonEnter(ptCommonArg);
	    fgRet = !_teePlayReadyContentSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]),NAUTILUS_CONTENTKEY_AUDIO);
	    _teeCommonLeave(ptCommonArg);
	    break;
    case TZCTL_TEE_PLAYREADY_CONTENTKEY_AESCTR_DMX_SET:
        fgRet = !_teePlayReadyDMXSetKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_TEE_PLAYREADY_CONTENT_AESCTR_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyContentDecrypt((DRM_AES_COUNTER_MODE_CONTEXT *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], *((BOOL *)ptCommonArg->apvArgument[2]), *((UINT64 *)ptCommonArg->apvArgument[4]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_MULTI_CONTENT_AESCTR_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyMultiContentDecrypt((DRM_AES_COUNTER_MODE_CONTEXT *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], (UINT32 *)ptCommonArg->apvArgument[3], ptCommonArg->ui4Length[3] / sizeof(UINT32), *((UINT64 *)ptCommonArg->apvArgument[4]), *((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_DASH_SAMPLE_AESCTR_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyDashSampleDecrypt((DRM_DASHAES_COUNTER_MODE_CONTEXT *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], (UINT32 *)ptCommonArg->apvArgument[3], ptCommonArg->ui4Length[3] / sizeof(UINT32), *((UINT64 *)ptCommonArg->apvArgument[4]), *((BOOL *)ptCommonArg->apvArgument[2]),FALSE);
        _teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_DASH_SAMPLE_AESCTR_DECRYPT_AUDIO:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		fgRet = !_teePlayReadyDashSampleDecrypt((DRM_DASHAES_COUNTER_MODE_CONTEXT *)ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1], (UINT32 *)ptCommonArg->apvArgument[3], ptCommonArg->ui4Length[3] / sizeof(UINT32), *((UINT64 *)ptCommonArg->apvArgument[4]), *((BOOL *)ptCommonArg->apvArgument[2]),TRUE);
		_teeCommonLeave(ptCommonArg);
		break;
    case TZCTL_TEE_PLAYREADY_AESECB_KEY_SET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESECBSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_AESECB_ENCRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESECBEncrypt(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((BOOL *)ptCommonArg->apvArgument[2]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_AESECB_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESECBDecrypt(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((BOOL *)ptCommonArg->apvArgument[2]));
    	_teeCommonLeave(ptCommonArg);
        break;
/***********************************************************************/
    case TZCTL_TEE_PLAYREADY_AESCBC_KEY_SET:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESCBCSetKey(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), *((UINT32 *)ptCommonArg->apvArgument[2]), *((BOOL *)ptCommonArg->apvArgument[3]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_AESCBC_ENCRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESCBCEncrypt(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]),ptCommonArg->apvArgument[2],*((UINT32 *)ptCommonArg->apvArgument[3]),  *((BOOL *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_PLAYREADY_AESCBC_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyAESCBCDecrypt(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), ptCommonArg->apvArgument[2],*((UINT32 *)ptCommonArg->apvArgument[3]), *((BOOL *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
	case TZCTL_TEE_PLAYREADY_UNSHUFFLE_CONTENTKEY:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyUnshuffleContentKey(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]), *((UINT32 *)ptCommonArg->apvArgument[3]), *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
		break;
    case  TZCTL_TEE_PLAYREADY_PREPARECONTENTKEY_PRIME:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teePlayReadyPrepareContentKey_Prime(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]), *((UINT32 *)ptCommonArg->apvArgument[3]), *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
		break;



/*************************** vudu ********************************************/
case  TZCTL_TEE_VUDU_OpenM2mKeySlot:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_OpenM2mKeySlot();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_CloseM2mKeySlot:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_CloseM2mKeySlot();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SelectM2mKeyAlgorithm:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SelectM2mKeyAlgorithm(*((UINT8 *)ptCommonArg->apvArgument[0]), *((UINT8 *)ptCommonArg->apvArgument[1]));
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_LoadClearM2mKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_LoadClearM2mKey(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]), ptCommonArg->apvArgument[3]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_LoadWrappedM2mKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_LoadWrappedM2mKey(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], ptCommonArg->apvArgument[3]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_LoadWrappedM2mKeyClearIv:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_LoadWrappedM2mKeyClearIv(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], *((UINT32 *)ptCommonArg->apvArgument[3]), ptCommonArg->apvArgument[4]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_RunM2mKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_RunM2mKey(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_OpenCAKeySlotHandle:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_OpenCAKeySlotHandle();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_CloseCAKeySlotHandle:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_CloseCAKeySlotHandle();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_GetCAKeySlotHandle:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_GetCAKeySlotHandle();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SetWrappedCAKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SetWrappedCAKey(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT8 *)ptCommonArg->apvArgument[2]));
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SetWrappedAllCAKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SetWrappedAllCAKey(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_LoadCAClearKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_LoadCAClearKey(ptCommonArg->apvArgument[0], *((UINT8 *)ptCommonArg->apvArgument[1]));
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_StartTSDecrypt:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_StartTSDecrypt(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], (UINT32 *)ptCommonArg->apvArgument[2], ptCommonArg->apvArgument[3]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SetOddEvenCAKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SetOddEvenCAKey(ptCommonArg->apvArgument[0], ptCommonArg->apvArgument[1], *((UINT8 *)ptCommonArg->apvArgument[2]));
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SVP_Init:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SVP_Init(*((UINT32 *)ptCommonArg->apvArgument[0]), *((INT32 *)ptCommonArg->apvArgument[1]));
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SVP_TSDecrypt:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SVP_TSDecrypt(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1], ptCommonArg->apvArgument[2], ptCommonArg->apvArgument[3]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SVP_GetAU: /*test*/
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SVP_GetAU(ptCommonArg->apvArgument[0],  *((UINT32 *)ptCommonArg->apvArgument[1]), ptCommonArg->apvArgument[2], ptCommonArg->apvArgument[3], *((UINT32 *)ptCommonArg->apvArgument[4]), ptCommonArg->apvArgument[4]);
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SVP_Flush:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SVP_Flush();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SVP_Release:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SVP_Release();
	_teeCommonLeave(ptCommonArg);
	break;

case  TZCTL_TEE_VUDU_SetM2mKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SetM2mkey(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1]);
	_teeCommonLeave(ptCommonArg);
	break;
    
 case  TZCTL_TEE_VUDU_SetCAKey:
	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
	_teeCommonEnter(ptCommonArg);
	fgRet = !_tee_CRYPTO_VUDU_SetCAkey(*((UINT32 *)ptCommonArg->apvArgument[0]), ptCommonArg->apvArgument[1]);
	_teeCommonLeave(ptCommonArg);
	break;



/****************************************************************************/


		
/***************************CI Plus related********************************************/
 #ifdef CC_CI_PLUS_TZ_SUPPORT
     case TZCTL_TEE_CIPLUS_EXTRACT_CRED:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUSExtractCred(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0],ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_CHECK_CRED_BY_CUSTOMER_CODE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUSCheckCred(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0],ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1],((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_VERIFY_CRED:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCILPLUSVerifyCred(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0],ptCommonArg->apvArgument[1], ptCommonArg->ui4Length[1],((BOOL *)ptCommonArg->apvArgument[2]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_GET_ENCRY_AKH:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUSGetEncryAKH(ptCommonArg->apvArgument[0], ((UINT32 *)ptCommonArg->apvArgument[1]));
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_SET_ENCRY_AKH:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUSSetEncryAKH(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_INIT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Init(ptCommonArg->apvArgument[0],(UINT32*)ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_REMOVE:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Remove(*((UINT32*)ptCommonArg->apvArgument[0]),(UINT32*)ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Data_Confirm:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Data_Confirm(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                                ptCommonArg->apvArgument[2],
                                                (UINT16*)ptCommonArg->apvArgument[3],
                                               (UINT32*)ptCommonArg->apvArgument[4]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_GET_HostDevCert:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_HostDevCert(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_HostBrandCert:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_HostBrandCert(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_Dhph:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_Dhph(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_HostId:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_HostId(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_Ns:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_Ns(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_CamId:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_CamId(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_CamBrandId:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_CamBrandId(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               (UINT16*)ptCommonArg->apvArgument[1],
                                               (UINT32*)ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
    
    case TZCTL_TEE_CIPLUS_DRM_GET_CCKId:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_KeyId(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               (UINT32*)ptCommonArg->apvArgument[1],
                                               (UINT32*)ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
        #if 0
    case TZCTL_TEE_CIPLUS_DRM_GET_CCKId:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_CCKId(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               ptCommonArg->ui4Length[2],
                                               ptCommonArg->apvArgument[3],
                                               ptCommonArg->apvArgument[4],
                                               (UINT32*)ptCommonArg->apvArgument[5]);
        _teeCommonLeave(ptCommonArg);
        break;
        #endif
    case TZCTL_TEE_CIPLUS_DRM_Set_UTC_time:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Set_UTC_Time(*((UINT32*)ptCommonArg->apvArgument[0]),
                                            *((UINT32*)ptCommonArg->apvArgument[1]),
                                            (UINT32*)ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
        
    case TZCTL_TEE_CIPLUS_DRM_Verify_ModuleCert:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Verify_ModuleCert(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               ptCommonArg->ui4Length[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Verify_SignatureB:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Verify_SignatureB(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               ptCommonArg->ui4Length[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Sign_Message:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Sign_Message(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                               ptCommonArg->apvArgument[2],
                                               (UINT16*)ptCommonArg->apvArgument[3],
                                               (UINT32*)ptCommonArg->apvArgument[4]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Generate_Akh:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Generate_Akh(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               (UINT16*)ptCommonArg->apvArgument[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Generate_SacKeys:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Generate_SacKeys(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               ptCommonArg->apvArgument[1],
                                               ptCommonArg->ui4Length[1],
                                               ptCommonArg->apvArgument[2],
                                               ptCommonArg->ui4Length[2],
                                               (UINT32*)ptCommonArg->apvArgument[3]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Generate_DHSK:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Generate_DHSK(*((UINT32*)ptCommonArg->apvArgument[0]),
                                               (UINT32*)ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Decrypt_MessageSac:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Decrypt_MessageSac(*((UINT32*)ptCommonArg->apvArgument[0]),
                                                  ptCommonArg->apvArgument[1],
                                                  ptCommonArg->ui4Length[1],
                                                  ptCommonArg->apvArgument[2],
                                                  (UINT16*)ptCommonArg->apvArgument[3],
                                                  ptCommonArg->apvArgument[4],
                                                  (UINT32*)ptCommonArg->apvArgument[5]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Encrypt_MessageSac:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Encrypt_MessageSac(*((UINT32*)ptCommonArg->apvArgument[0]),
                                                  ptCommonArg->apvArgument[1],
                                                  ptCommonArg->ui4Length[1],
                                                  ptCommonArg->apvArgument[2],
                                                  (UINT16*)ptCommonArg->apvArgument[3],
                                                  (UINT32*)ptCommonArg->apvArgument[4]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_CIPLUS_DRM_Get_NextAuthContext:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = _teeCIPLUS_DRM_Get_NextAuthContext(*((UINT32*)ptCommonArg->apvArgument[0]),
                                                  ptCommonArg->apvArgument[1],
                                                  (UINT16*)ptCommonArg->apvArgument[2],
                                                  ptCommonArg->apvArgument[3],
                                                  (UINT16*)ptCommonArg->apvArgument[4],
                                                  (UINT16*)ptCommonArg->apvArgument[5],
                                                  (UINT32*)ptCommonArg->apvArgument[6]);
        _teeCommonLeave(ptCommonArg);
        break;
  #endif
/***********************************************************************/
    case TZCTL_TEE_DDI_PREENC_A1_DEC:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDDIPreEncA1Dec(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1], (UINT32 *)ptCommonArg->apvArgument[2]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DDI_SESTORE_SIGN:
    	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDDISeStoreSign(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
     	break;
    case TZCTL_TEE_DDI_SESTORE_VERIFY:
    	ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDDISeStoreVerify(ptCommonArg->apvArgument[0], ptCommonArg->ui4Length[0], (UINT32 *)ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
     	break;
#ifdef CC_ENABLE_ULI_IN_TEE
    case TZCTL_TEE_ULI_OBJECT_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);

        fgRet = !_teeUliObjectDecrypt(ptCommonArg->apvArgument[0],
                                      ptCommonArg->apvArgument[1],
                                      ptCommonArg->apvArgument[2],
                                      ptCommonArg->ui4Length[2],
                                      ptCommonArg->apvArgument[3],
                                      ptCommonArg->apvArgument[4],
                                      ptCommonArg->apvArgument[5],
                                      ptCommonArg->apvArgument[6]);
        _teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_ULI_CHK_POTK:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);

        fgRet = !_teeUlChkPOTK(ptCommonArg->apvArgument[0],
                               ptCommonArg->apvArgument[1]);
        _teeCommonLeave(ptCommonArg);
        break;

#endif
    
    case TZCTL_TEE_DTCP_HCI_ENCRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpEncryptHCI(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), ptCommonArg->apvArgument[2]);
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DTCP_HCI_DECRYPT:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpDecryptHCI(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]), ptCommonArg->apvArgument[2]);
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DTCP_HCI_ENCRYPTSWAP:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpSwapEncryptedHCI(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]),
                                        ptCommonArg->apvArgument[2], *((UINT32 *)ptCommonArg->apvArgument[3]),
                                        ptCommonArg->apvArgument[4]);
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DTCP_SETLOCALKEY:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpSetEncLocalKey(*((UINT32 *)ptCommonArg->apvArgument[0]),
                                        ptCommonArg->apvArgument[1], *((UINT32 *)ptCommonArg->apvArgument[2]),
                                        ptCommonArg->apvArgument[3], *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DTCP_DECRYPTHDD:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpDecryptHddData(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]),
                                        ptCommonArg->apvArgument[2], ptCommonArg->apvArgument[3],
                                        *((UINT32 *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;
    case TZCTL_TEE_DTCP_ENCRYPTSWAPHDD:
        ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
        _teeCommonEnter(ptCommonArg);
        fgRet = !_teeDtcpSwapEncryptedHddData(ptCommonArg->apvArgument[0], *((UINT32 *)ptCommonArg->apvArgument[1]),
                                              ptCommonArg->apvArgument[2], *((UINT32 *)ptCommonArg->apvArgument[3]),
                                              *((BOOL *)ptCommonArg->apvArgument[4]));
    	_teeCommonLeave(ptCommonArg);
        break;

#if 1 //antirollback_feature
	case TZCTL_TEE_PLAYREADY_ANTIROLLBACK_LOADIN:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
	    fgRet = !_tee_antirollback_loadinclock((char*)ptCommonArg->apvArgument[0],
                           ptCommonArg->ui4Length[0]);	
		_teeCommonLeave(ptCommonArg);
		break;						 
	case TZCTL_TEE_PLAYREADY_ANTIROLLBACK_UPDATE:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
	    fgRet = !_tee_antirollback_updateclock(*((unsigned long *)ptCommonArg->apvArgument[0]),
                           (char*)ptCommonArg->apvArgument[1],
                           ptCommonArg->ui4Length[1]);	
		_teeCommonLeave(ptCommonArg);
		break;
	case TZCTL_TEE_PLAYREADY_ANTIROLLBACK_GETTIME:
		 ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		 _teeCommonEnter(ptCommonArg);
		 fgRet = !_tee_antirollback_gettime(((unsigned long *)ptCommonArg->apvArgument[0]));  
		 _teeCommonLeave(ptCommonArg);

		break;
#endif

#if 1 //SECURE_STOP_ENABLE
	case TZCTL_TEE_PLAYREADY_SECURESTOP_INITSS:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);

		_teeCommonLeave(ptCommonArg);
		break;						 
	case TZCTL_TEE_PLAYREADY_SECURESTOP_GETSESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);

		_teeCommonLeave(ptCommonArg);
		break;			
	case TZCTL_TEE_PLAYREADY_SECURESTOP_GETSESSIONBYKID:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		_teeCommonLeave(ptCommonArg);
		break;					  
	case TZCTL_TEE_PLAYREADY_SECURESTOP_ADDSESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_add_session((const unsigned char *)ptCommonArg->apvArgument[0]);
		_teeCommonLeave(ptCommonArg);
		break;					   
	case TZCTL_TEE_PLAYREADY_SECURESTOP_CANCELSESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_cancel_session((const unsigned char *)ptCommonArg->apvArgument[0]);
		_teeCommonLeave(ptCommonArg);
		break;						
	case TZCTL_TEE_PLAYREADY_SECURESTOP_BINDLICENSE:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_bind_license((const unsigned char *)ptCommonArg->apvArgument[0], (const unsigned char *)ptCommonArg->apvArgument[1]);
		_teeCommonLeave(ptCommonArg);
		break;						  
	case TZCTL_TEE_PLAYREADY_SECURESTOP_BINDSESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_bind_session((const unsigned char *)ptCommonArg->apvArgument[0], (const unsigned char *)ptCommonArg->apvArgument[1]
									  , *((long *)ptCommonArg->apvArgument[2]),(const unsigned char *)ptCommonArg->apvArgument[3]);
		_teeCommonLeave(ptCommonArg);
		break;					   
	case TZCTL_TEE_PLAYREADY_SECURESTOP_REMOVESESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_remove_session(*((int *)ptCommonArg->apvArgument[0]));
		_teeCommonLeave(ptCommonArg);
		break;						 
	case TZCTL_TEE_PLAYREADY_SECURESTOP_RESETSESSION:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_reset_sessions();
		_teeCommonLeave(ptCommonArg);
		break;						
	case TZCTL_TEE_PLAYREADY_SECURESTOP_WRITE_SECURESTOP:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		__tee_write_secure_stop((SECURE_STOP   *)ptCommonArg->apvArgument[0], (unsigned char *)ptCommonArg->apvArgument[1]
									  , *((int *)ptCommonArg->apvArgument[2]));
		_teeCommonLeave(ptCommonArg);
		break;						  
	case TZCTL_TEE_PLAYREADY_SECURESTOP_INIT_SECURESTOPS:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_init_secure_stops((unsigned char *)ptCommonArg->apvArgument[0]
									  , *((int *)ptCommonArg->apvArgument[1]));
		_teeCommonLeave(ptCommonArg);
		break;					   
	case TZCTL_TEE_PLAYREADY_SECURESTOP_GET_SECURESTOPS:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_get_secure_stops( (const unsigned char *)ptCommonArg->apvArgument[0]
									  , (int *)ptCommonArg->apvArgument[1]);
		(*(int *)ptCommonArg->apvArgument[2]) = ss_return;
		_teeCommonLeave(ptCommonArg);
		break;					   
	case TZCTL_TEE_PLAYREADY_SECURESTOP_GET_SECURESTOP_IDS:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_get_secure_stop_ids( (unsigned char (*)[TEE_SESSION_ID_LEN])ptCommonArg->apvArgument[0]
									  , (int *)ptCommonArg->apvArgument[1]);
		_teeCommonLeave(ptCommonArg);
		break;					  
	case TZCTL_TEE_PLAYREADY_SECURESTOP_GET_SECURESTOP:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_get_secure_stop( (const unsigned char *)ptCommonArg->apvArgument[0]
									  , (unsigned char *)ptCommonArg->apvArgument[1]
									  , (int *)ptCommonArg->apvArgument[2]
									  , *((long *)ptCommonArg->apvArgument[3])
									  , (TEE_SIGNATURE *) ptCommonArg->apvArgument[4]);
		(*(int *)ptCommonArg->apvArgument[5]) = ss_return;
		_teeCommonLeave(ptCommonArg);
		break;					 
	case TZCTL_TEE_PLAYREADY_SECURESTOP_UPDATE_SECURESTOP:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_update_secure_stop( *((int *)ptCommonArg->apvArgument[0])
									  , (int *)ptCommonArg->apvArgument[1]
									  , *((long *)ptCommonArg->apvArgument[2]) );

		_teeCommonLeave(ptCommonArg);
		break;				  
	case TZCTL_TEE_PLAYREADY_SECURESTOP_COMMIT_SECURESTOP:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_commit_secure_stop( (const unsigned char *)ptCommonArg->apvArgument[0]);
		_teeCommonLeave(ptCommonArg);
		break;					
	case TZCTL_TEE_PLAYREADY_SECURESTOP_RESET_SECURESTOPS:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_reset_secure_stops( (int *)ptCommonArg->apvArgument[0]);
		_teeCommonLeave(ptCommonArg);
		break;
	case TZCTL_TEE_PLAYREADY_SECURESTOP_UNLOADSECRETKEY:
		ptCommonArg = (TEE_COMMON_ARG_T *)prSMCArg->u4Addr;
		_teeCommonEnter(ptCommonArg);
		ss_return = _tee_UnloadSecretKey( (TEE_SECRET_KEY *)ptCommonArg->apvArgument[0]);
		_teeCommonLeave(ptCommonArg);
		break;			
	
#endif

    case TZCTL_DMX_INIT:
        fgRet = SvcEx_DMX_Init();
        break;
    case TZCTL_DMX_SET_MMKEY:
        fgRet = SvcEx_DMX_DMX_SetMMKey((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DMX_GETPICHDR:
        fgRet = SvcEx_DMX_GetPicHeader((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DMX_GESECUREVFIFO:
        fgRet = SvcEx_DMX_GetSecureVfifo((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DMX_GESECUREVFIFO2:
        fgRet = SvcEx_DMX_GetSecureVfifo2((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_DMX_FREESECUREKEY:
		fgRet = DMX_FreeSecureKeyIv((UINT8)(prSMCArg->u4Addr));
		break;
    case TZCTL_DMX_RELOADSECUREKEY:
        fgRet = SvcEx_DMX_ReloadSecureKey((UINT8 *)prSMCArg->u4Addr);
        break;
    case TZCTL_DMX_SETCRYPTOKEY:
        fgRet = SvcEx_DMX_SetCryptoKey((UINT8 *)prSMCArg->u4Addr);
        break;
    case TZCTL_FVR_SETCRYPTOKEY:
        fgRet = SvcEx_FVR_SetCryptoKey((UINT8 *)prSMCArg->u4Addr);
        break;
    case TZCTL_DMX_SETMMSECURITY:
        fgRet = SvcEx_DMX_SetMMSecurity((UINT8 *)prSMCArg->u4Addr);
        break;
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_TVP_SUPPORT)
    case TZCTL_VDEC_FB_OPERATION:
        fgRet = SvcEx_VDEC_FB_Operation((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_VDEC_CHECK_REALD:
        fgRet = SvcEx_VDEC_Check_RealD((UINT8 *)prSMCArg->u4Addr,prSMCArg->u4Size);
        break;
#endif
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
    case TZCTL_AU_DELIM_APPEND:
        fgRet = SvcEx_AU_DELIM_APPEND((UINT8 *)prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
#endif
    case TZCTL_GCPU_HW_CMDRUN:
        fgRet = SvcEx_GCPU_Hw_CmdRun(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_HW_CMDRETURN:
        fgRet = SvcEx_GCPU_Hw_CmdReturn(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_ENABLE_IOMMU:
        fgRet = SvcEx_GCPU_EnableIOMMU(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_DISABLE_IOMMU:
        fgRet = SvcEx_GCPU_DisableIOMMU();
        break;
    case TZCTL_GCPU_CLEAR_IRQ:
        fgRet = SvcEx_GCPU_ClearIRQ();
        break;
    case TZCTL_GCPU_CLEAR_IOMMU_IRQ:
        fgRet = SvcEx_GCPU_ClearIommuIRQ();
        break;
    case TZCTL_GCPU_SET_SLOT:
        fgRet = SvcEx_GCPU_Hw_SetSlot(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_HW_RESET:
        fgRet = SvcEx_GCPU_Hw_Reset(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_IRQ_HANDLER:
        fgRet = SvcEx_GCPU_IrqHandle(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_IOMMU_IRQ_HANDLER:
        fgRet = SvcEx_GCPU_IOMMU_IrqHandle(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_GCPU_GET_CHECKSUM:
        fgRet = SvcEx_GCPU_GetRomCodeChecksum(prSMCArg->u4Addr, prSMCArg->u4Size);
        break;
    case TZCTL_SWDMX_MPGPARSPTS:
        fgRet = SvcEx_SWDMX_MpgParsingPts((UINT8 *)prSMCArg->u4Addr,prSMCArg->u4Size);
        break;
    case TZCTL_SWDMX_DECRYPT_DATA:
        fgRet = SvcEx_SWDMX_DecryptData((UINT8 *)prSMCArg->u4Addr,prSMCArg->u4Size);
        break;
    case TZCTL_SWDMX_FREE_DECRYPT_BUF:
        fgRet = SvcEx_SWDMX_FreeDecryptBuf();
        break;		
    case TZCTL_GCPU_RESTORE_SECURE_KEYS:
        fgRet = SvcEx_GCPU_Hw_RestoreSecureKeys();
        break;
#if defined(CC_TVP_SUPPORT)
	case TZCTL_FBM_INITIALIZATION:
		fgRet = TZ_FBM_Initialization((UINT8 *)prSMCArg->u4Addr,prSMCArg->u4Size);
		break;
#endif
    default:
        Printf("SvcEx_TZCTL cmd error: cmd=0x%x, addr=0x%x, size=0x%x\n",
            prSMCArg->u4Cmd, prSMCArg->u4Addr, prSMCArg->u4Size);
        while(1)
    	{
			Printf("SvcEx_TZCTL reach default condition , please check ...\n");
    	};
    }

    TZ_SWD_BufferEnd(prSMCArg->u4Addr, prSMCArg->u4Size);

if (ss_return != 1)
    return fgRet?TZ_SUCCESS:TZ_ERROR_GENERIC;
else
	return TZ_ERROR_GENERIC;
}

