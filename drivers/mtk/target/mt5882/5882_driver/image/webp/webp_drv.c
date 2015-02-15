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
#include "x_assert.h"
#include "x_os.h"
#include "x_rm.h"
#include "x_img_dec.h"
#include "x_ckgen.h"
#include "x_hal_arm.h"
#include "webp_drv.h"
#include "jpg_if.h"
#include "../jpg_6896/img_flw.h"

#define WEBP_RIFF_DATA_OFFSET       20
#define WEBP_DEC_ID                 0

static UINT32 _u4FillSz = 0;

#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
BOOL _bWebpMapAddr = FALSE;
#endif


static UINT32 _WEBP_CallBack(UINT32 u4Tag, void *pvData, WEBP_NOTIFY_TYPE u4CbType)
{
    if(u4CbType==WEBP_NOTIFY_TYPE_REQDATA)
    {
        WEBP_DATA_REQ_INFO *prDataReqInfo=(WEBP_DATA_REQ_INFO *)pvData;

        if (prDataReqInfo != NULL)
        {
            IMG_BsGetData(0,VIRTUAL(prDataReqInfo->u4WritePosition), prDataReqInfo->u4FileOffset, prDataReqInfo->u4Size);
            ASSERT(_u4FillSz <= prDataReqInfo->u4Size);
			
            HalFlushInvalidateDCacheMultipleLine(VIRTUAL(prDataReqInfo->u4WritePosition), _u4FillSz);
            return _u4FillSz;
        }
    }

    return 0;
}

WEBPHANDLE _WEBP_AllocInstance(void)
{
	// allocate	memory,	memset it to zero, set its type	to JDEC
	void *pvMem;

	pvMem =	x_mem_alloc(sizeof(WEBP_INST_T));
	if (pvMem == NULL)
	{
	      LOG(0, "WEBP_INST_T mem_alloc failed\n");
		return pvMem;
	}

	x_memset(pvMem,	0, sizeof(WEBP_INST_T));
	return (WEBPHANDLE)pvMem;
}


#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
void i4WEBPDynamicFbmMap(void)
{
    UINT32 u4ImgMapAddr = 0;
    FBM_POOL_T* prFbmPool;

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
    ASSERT(prFbmPool != (FBM_POOL_T*)NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    u4ImgMapAddr = (UINT32)BSP_MapReservedMem((void *)prFbmPool->u4Addr, JPG_TOTAL_BUFFER_SIZE);
    LOG(5, "webp map fbm u4ImgMapAddr:0x%x\n", u4ImgMapAddr);
    if(!u4ImgMapAddr)
    {        
        LOG(0, "Img fbm dynamic map fail!\n");
        ASSERT(0);
    }
    _bWebpMapAddr = TRUE;
}

void _WEBP_UnmapFbm(ENUM_IMG_BUFFER_TYPE eMemClass)
{    
    FBM_POOL_T* prFbmPool;    
    
    if(_bWebpMapAddr)
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0);    
        
        LOG(5, "webp unmap fbm virtual addr:0x%x\n", (VIRTUAL(prFbmPool->u4Addr)));
        BSP_UnMapReservedMem((void *)VIRTUAL(prFbmPool->u4Addr));
        _bWebpMapAddr = FALSE;
    }
}
#endif 



void* _WEBP_Init(UCHAR ucImgId,ENUM_IMG_BUFFER_TYPE eMemClass, void *pvTag)
{
    void* pvHandler;
    IMG_LIB_INFO_T* prImg;
    FBM_POOL_T* prFbmPool;

    //--- Image handler alloc
    pvHandler = x_mem_alloc(sizeof(IMG_LIB_INFO_T));
    if (pvHandler == NULL)
    {
        return (void *)NULL;
    }
    x_memset(pvHandler, 0, sizeof(IMG_LIB_INFO_T));
    
    prImg = (IMG_LIB_INFO_T *)pvHandler;
    prImg->hCurDec = _WEBP_AllocInstance();

    if (prImg->hCurDec == NULL)
    {
        x_mem_free(pvHandler);
        return (void*)NULL;
    }
    
    prImg->pvTag = pvTag;
    prImg->u1MemClass = (UINT8)eMemClass;
    prImg->bStopDec = FALSE;
    prImg->bIsEOF=FALSE;
    prImg->bUseMw=FALSE;

    // webp memory related function
    #if defined(CC_MT5890)
    prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_PNG);
	#else
    prFbmPool = FBM_GetPoolInfo(FBM_POOL_TYPE_JPEG);
	#endif
    if(prFbmPool == NULL)
    { 
       LOG(0, "Can't get jpeg fbm type \n");
       _WEBP_FreeInstance(prImg->hCurDec);
       x_mem_free(pvHandler);
       return (void*)NULL;
    }
    else
    { 

       ASSERT(prFbmPool->u4Size >= JPG_TOTAL_BUFFER_SIZE);
        #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
            if(FALSE == _bWebpMapAddr)
            {
                i4WEBPDynamicFbmMap();
            }

		#endif
       _VDEC_Vp8WebPInit(WEBP_DEC_ID, _WEBP_CallBack, prFbmPool->u4Addr, JPG_TOTAL_BUFFER_SIZE);
    }
	UNUSED(ucImgId);
    return (void*)pvHandler;
}

INT32 _WEBP_DecPrepare(WEBPHANDLE hCurImg, IMG_PARAM_T *prInfo)
{
    FBM_POOL_T *prFbmPool;
    IMG_LIB_INFO_T* prImg;
    WEBP_INST_T* pInst;
    WEBP_DECIN_PARAM_T* pInParam;
    IMG_REGION_T* pDst;  
    UINT32 u4AvailableSize;

    // prepare obj pointer
    if ((prImg=(IMG_LIB_INFO_T*)hCurImg) == NULL)
    {
        return (INT32)E_IMG_NULL_HANDLER;
    }
    
    if ((pInst = (WEBP_INST_T*)prImg->hCurDec) == NULL)
    {
        return (INT32)E_IMG_NULL_HANDLER;
    }
    
    pInParam = (WEBP_DECIN_PARAM_T*)(&pInst->rInParam);
    pDst = &(prInfo->rDstRegion);

    // setup InParam
   	if(prInfo->eDispColorMode==AYCBCR8888_DIRECT32)
	{
	   pInParam->eColorMode = WEBP_COLOR_AYCBCR8888;
	}
	else if(prInfo->eDispColorMode==ARGB8888_DIRECT32)
	{
		pInParam->eColorMode = WEBP_COLOR_ARGB8888;
	}
	else if(prInfo->eDispColorMode==Y_CBCR420_SEP16)
    {
	   pInParam->eColorMode = WEBP_COLOR_YCSEP420;
    }
	else if(prInfo->eDispColorMode==Y_CBCR422_SEP16)
    {
	   pInParam->eColorMode = WEBP_COLOR_YCSEP422;
    }
	else
	{
	   Printf("Error output color mode %d\n",prInfo->eDispColorMode);
	   ASSERT(0);
	   return E_IMG_DEC_FAIL;
	}

	if(prInfo->rDstRegion.pu1Addr==NULL)
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG);
        u4AvailableSize = prFbmPool->u4Size - JPG_TOTAL_BUFFER_SIZE;
        if (u4AvailableSize < ((pDst->u4Pitch * pDst->u4Height) <<1))
        {
            return (INT32)E_IMG_DISP_INFO_ERROR;
        }
        pInParam->u4DestBuffer = prFbmPool->u4Addr + JPG_TOTAL_BUFFER_SIZE;
    }
	else
	{
        pInParam->u4DestBuffer=(UINT32)(prInfo->rDstRegion.pu1Addr);
	}
	
    if(pDst->u4Pitch==0)
    {
       pInParam->u4Pitch=((pDst->u4Width+15)/16)*16;
       if(pInParam->eColorMode == WEBP_COLOR_AYCBCR8888 || pInParam->eColorMode == WEBP_COLOR_ARGB8888)
       {
           pInParam->u4Pitch = pInParam->u4Pitch<<2;
       }
    }
    else
    {
       pInParam->u4Pitch=pDst->u4Pitch;
    }

	if(pDst->u4CbCrOffset==0 && (prInfo->eDispColorMode==Y_CBCR420_SEP16||prInfo->eDispColorMode==Y_CBCR422_SEP16))
	{
        pDst->u4CbCrOffset=((pInParam->u4Pitch*pDst->u4Height+15)/16)*16;
	}
	
	pInParam->u4CbCrOffset=pDst->u4CbCrOffset;
    ASSERT((pInParam->u4Pitch&0xf)==0);
	if(prInfo->bKeepRatio)
	{
       WEBP_SETFLG(pInParam->u4Flag,WEBP_DEC_FLAG_KEEPRATIO);
	}
	
    pInParam->u4FileOffset=WEBP_RIFF_DATA_OFFSET;
    pInParam->u4Width = pDst->u4Width;
    pInParam->u4Height = pDst->u4Height;
    pInParam->u4StartX = pDst->u4StartX;
    pInParam->u4StartY = pDst->u4StartY; 
/*  
   pInParam->u4Pitch = pDst->u4Pitch;//((pInParam->u4Width + 15)/16)*16;

    if (WEBP_CLOR_TYPE != Y_CBCR888_YCSEP)
    {
        pInParam->u4Pitch = pInParam->u4Pitch<<2;
    }
*/
    return (INT32)E_IMG_OK;
}

INT32 _WEBP_DecOneSegment(WEBPHANDLE hCurImg,UINT32 u4ImgID, IMG_PARAM_T *prInfo)
{
    IMG_LIB_INFO_T* prImg;
    WEBP_INST_T* pInst;
	IMG_REGION_T* prDecOut=prInfo->prDecOut;
    if((prImg=(IMG_LIB_INFO_T*)hCurImg) == NULL)
    {
        return (INT32)E_IMG_NULL_HANDLER;
    }
    
    if((pInst = (WEBP_INST_T*)prImg->hCurDec) == NULL)
    {
        return (INT32)E_IMG_NULL_HANDLER;
    }    
#if defined(CC_MT5890) ||  defined(CC_MT5882) 

    if(_VDEC_Vp8WebPDecStart(WEBP_DEC_ID, WEBP_DEC_MODE_MCUROW, &pInst->rInParam, &pInst->rOutParam))
#else
    if(_VDEC_Vp8WebPDecStart(WEBP_DEC_ID, WEBP_DEC_MODE_AUTO, &pInst->rInParam, &pInst->rOutParam))
#endif
    {
		prDecOut->pu1Addr=(UINT8 *)pInst->rInParam.u4DestBuffer;
		prDecOut->u4Pitch= pInst->rInParam.u4Pitch;
		prDecOut->u4Depth=4;
		prDecOut->u4CbCrOffset=pInst->rOutParam.u4CbCrOffset;
		prDecOut->u4Width=pInst->rOutParam.u4Width;
		prDecOut->u4Height=pInst->rOutParam.u4Height;
		prDecOut->u4TotalLine=prInfo->rDstRegion.u4Height;
		prDecOut->u4StartX=pInst->rOutParam.u4StartX;
		prDecOut->u4StartY=pInst->rOutParam.u4StartY;
        return (INT32)E_IMG_DEC_FINISH;
    }

    return (INT32)E_IMG_DEC_FAIL;
}


INT32 _WEBP_BsBufManager(WEBPHANDLE hCurImg, UINT32 u4ImgID)
{
    return (INT32)E_IMG_OK;
}

void _WEBP_SetFillBsBufEnd(WEBPHANDLE hCurImg, UINT32 u4Entry1Size,
                          UINT32 u4Entry2Size, BOOL bIsEOF)
{
    LOG(6, "_WEBP_SetFillBsBufEnd %d %d\n", u4Entry1Size, u4Entry2Size);

    if (bIsEOF)
    {
        LOG(5, "_WEBP_SetFillBsBufEnd EOF\n");
    }

    _u4FillSz = u4Entry1Size;

    ASSERT(u4Entry2Size == 0);
}

INT32 _WEBP_CheckDecParam(WEBPHANDLE hCurImg, IMG_PARAM_T *prInfo)
{
    IMG_LIB_INFO_T* prImg;

    prImg=(IMG_LIB_INFO_T*)hCurImg;

    if (prImg == NULL)
    {
        return (INT32)E_IMG_NULL_HANDLER;
    }

    if(prInfo->rDstRegion.u4Width < 4 || prInfo->rDstRegion.u4Height < 4)
    {
        return (INT32)E_IMG_DISP_INFO_ERROR;
    }

    return (INT32)E_IMG_OK;
}

ENUM_IMG_RET _WEBP_Stop(WEBPHANDLE hCurImg)
{
    IMG_LIB_INFO_T* prImg;
//    WEBP_INST_T* prInst;
    
    if ((prImg = (IMG_LIB_INFO_T*)hCurImg) == NULL)
    {
        return E_IMG_NULL_HANDLER;
    }
    
//    if ((prInst = (WEBP_INST_T*)prImg->hCurDec) == NULL)
//    {
//        return E_IMG_NULL_HANDLER;
//    }
    
    prImg->bStopDec = TRUE;
    _VDEC_Vp8WebPDecStop(WEBP_DEC_ID);

    return E_IMG_OK;
}

INT32 _WEBP_MwFillBuffer(WEBPHANDLE hDec, void **ppvBuf,UINT32 *pu4Size, UINT32 u4AddSize)
{
    return (INT32)E_IMG_CANCEL;
}

void _WEBP_SetFileSize(WEBPHANDLE hDec,UINT32 u4FileSize)
{
    WEBP_INST_T* pInst;
    WEBP_DECIN_PARAM_T* pInParam;

    if ((pInst = (WEBP_INST_T*)hDec) == NULL)
    {
        return;
    }
    
    if ((pInParam = (WEBP_DECIN_PARAM_T*)(&pInst->rInParam)) == NULL)
    {
        return;
    }

    pInParam->u4FileSize =  u4FileSize;
}

void _WEBP_FreeInstance(WEBPHANDLE hDec)
{
    if (hDec ==	(WEBPHANDLE)NULL)
    {
        return;
    }
    x_mem_free((void*)hDec);
}

VOID _WEBP_GetDecInfo(WEBPHANDLE hDec,UINT32 u4GetType,UINT32 u4Param1,UINT32 u4Param2)
{
    IMG_LIB_INFO_T* prImg=(IMG_LIB_INFO_T*)hDec;
	ENUM_IMG_GETINFO_TYPE eGetType=(ENUM_IMG_GETINFO_TYPE)u4GetType;
    if(prImg==NULL)
    {
        return;
    }
	
	if(eGetType==IMG_GETINFO_PROGRESS)
	{
	   if(u4Param1==0)
	   {
          ASSERT(0);
		  return;
	   }
	   *((UINT32 *)u4Param1)=_VDEC_Vp8WebPGetProgress(WEBP_DEC_ID);
	}
	return;
}

