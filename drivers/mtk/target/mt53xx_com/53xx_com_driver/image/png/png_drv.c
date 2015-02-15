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
//#include "img_flw.h"

#include "x_assert.h"
#include "x_os.h"
#include "png_drv.h"
#include "png_drv_decode.h"
#include "png_hal_dec.h"
#include "png_debug.h"

#include "x_rm.h"
#include "gfx_if.h"

#include "x_img_dec.h"
#include "x_ckgen.h"

#include "jpg_if.h"
#include "../jpg_6896/img_flw.h"
#include "png_hal_priv.h"
//#include "linux/sched.h"

#if defined (CC_SUPPORT_4K2K) || defined (CC_SUPPORT_4K2K_PHOTO)
#define DISP_PNG_W_LMT 3840
#define DISP_PNG_H_LMT 2160
#else
#define DISP_PNG_W_LMT 3840
#define DISP_PNG_H_LMT 2160
#endif

#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
BOOL _bPngMapAddr = FALSE;
#endif
extern PNG_HAL_INST_T  _tPngHalHwInst[PNG_HAL_HW_INST_NUM];


/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

static HANDLE_T hPngDbgInsertSema = 0;
UINT32 u4hPNGSemaphoreDBGCnt = 0;
UINT32 u4hPNGSemaphoreDBG[256][2]={{0}, {0}};
UINT32 u4hPngSema =1;
UINT32 u4hPng_Hal_Sema =2;

void u4hPNGSemaphoreDBGInsert(UINT32 type, UINT32 value)
{	

	if(hPngDbgInsertSema==0)
	{
		LOG(3, "u4hPNGSemaphoreDBGInsert create hMtinageDbgInsertSema............\n");	
		VERIFY(x_sema_create(&hPngDbgInsertSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);		
	}

	if(hPngDbgInsertSema!=0)
	{
		VERIFY (x_sema_lock(hPngDbgInsertSema, X_SEMA_OPTION_WAIT) == OSR_OK);
	}	

	u4hPNGSemaphoreDBG[u4hPNGSemaphoreDBGCnt][0] = type;
	u4hPNGSemaphoreDBG[u4hPNGSemaphoreDBGCnt][1] = value;  
	u4hPNGSemaphoreDBGCnt++;
	
	if(u4hPNGSemaphoreDBGCnt>=256)
	{
		//x_memset(u4hPNGSemaphoreDBG,0x0, sizeof(u4hPNGSemaphoreDBG));  	
		u4hPNGSemaphoreDBGCnt =0;
	}  	


	if(hPngDbgInsertSema!=0)
	{
		VERIFY (x_sema_unlock(hPngDbgInsertSema) == OSR_OK);    
	}
  
}
 
BOOL _fgPngInit = FALSE;

void vPngAbort(PNG_INST_T *ptPngInst, IMG_NFY_STATE_T eState, BOOL fgNfy);

extern UINT32 u4PngGetSTC(void);

PNGHANDLE PNG_AllocInstance(void)
{
    // allocate	memory,	memset it to zero, set its type	to JDEC
    void *pvMem;

    pvMem =	x_mem_alloc(sizeof(PNG_INST_T));
    if (pvMem == NULL)
    {
        LOG(0, "JDEC mem_alloc failed\n");
        return pvMem;
    }

    x_memset(pvMem,0, sizeof(PNG_INST_T));

    return (PNGHANDLE)pvMem;
}

/*
* For oryx, dynamic map/unmap fbm to kernal space to get virtual address
* just for some memory that need to be access by cpu
*void BSP_MapReservedMem((void*)phy_addr, UINT32size)
*void BSP_UnMapReservedMem(void *)
*/
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING    
void i4PngDynamicFbmMap(void)
{    
    FBM_POOL_T* prFbmPool;    
    UINT32 u4PngMapAddr = 0;

    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    
    u4PngMapAddr = (UINT32)BSP_MapReservedMem((void *)prFbmPool->u4Addr, PNG_RZ_TEMP_BUF_OFFSET);
    vPngLog_L("png 0x%x dynamic map addr:0x%x, size:0x%x\n", prFbmPool->u4Addr, u4PngMapAddr, PNG_RZ_TEMP_BUF_OFFSET);
    if(!u4PngMapAddr)
    {
        LOG(0, "Png fbm dynamic map fail\n");
        ASSERT(0);
    }
    _bPngMapAddr = TRUE;
}

void _PNG_Unmap(ENUM_IMG_BUFFER_TYPE eMemClass)
{    
    FBM_POOL_T* prFbmPool;    
    if( _bPngMapAddr && (eMemClass != EXTERNAL_RING_IN_DOUBLE_OUT))
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0);    

        BSP_UnMapReservedMem((void *)VIRTUAL(prFbmPool->u4Addr));
        vPngLog_L("png unmap\n");
       _bPngMapAddr = FALSE;
    }
}
#endif

void PNG_FreeInstance(PNGHANDLE hDec)
{
    PNG_INST_T* prPngInst;    
    
    if (hDec ==	(PNGHANDLE)NULL)
    {
        return;
    }    
    prPngInst = (PNG_INST_T*)hDec;
    //printk("PNG_FreeInstance lock %d \n", current->pid);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 1);
    x_sema_lock(prPngInst->hPngSema, X_SEMA_OPTION_WAIT);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 2);    
    if (prPngInst->ePngState != PNG_STATE_IDLE)
    {
        VERIFY(i4PngHalSetStop(prPngInst->u4HwInstId) == PNG_HAL_RTN_OK);
    }
    if (prPngInst->u4HwInstId != PNG_INVALID32)
    {
        VERIFY(i4PngHalRelHwInst(prPngInst->u4HwInstId) == PNG_HAL_RTN_OK);
        prPngInst->u4HwInstId =  PNG_INVALID32;
    }
    prPngInst->fgActive  = FALSE;	

    u4hPNGSemaphoreDBGInsert(u4hPngSema, 3); 
    x_sema_unlock(prPngInst->hPngSema);
    //printk("PNG_FreeInstance unlock %d \n", current->pid);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 4); 
    
    x_mem_free((void*)hDec);
}
//IMGHANDLE
void* _PNG_Init(UCHAR ucImgId,ENUM_IMG_BUFFER_TYPE eMemClass, void *pvTag)
{
    //static BOOL bInited = FALSE;
    void* pvHandler;
    IMG_LIB_INFO_T* prImg;
    PNG_INST_T* prPngInst;
    FBM_POOL_T* prFbmPool;    
    INT32 i4 = 0;
    
    //--- Image handler alloc
    pvHandler = x_mem_alloc(sizeof(IMG_LIB_INFO_T));
    if (pvHandler == NULL)
    {
        return (void *)NULL;
    }
    x_memset(pvHandler, 0, sizeof(IMG_LIB_INFO_T));
    prImg = (IMG_LIB_INFO_T *)pvHandler;
    prImg->hCurDec = PNG_AllocInstance();

    if ( prImg->hCurDec == NULL)
    {
        return (void*)NULL;
    }
    prImg->pvTag = pvTag;
    prImg->u1MemClass = (UINT8)eMemClass;
    prImg->bStopDec = FALSE;
    prImg->bIsEOF=FALSE;
    prImg->bUseMw=FALSE;
    prPngInst = (PNG_INST_T*)prImg->hCurDec;
	prPngInst->u4ImgId = (UINT32) ucImgId;
    //--- gfx init
    /*
    if ((!bInited) && (eMemClass != LINEAR_IN_LINEAR_OUT))
    {
        GFX_Init();
        bInited = TRUE;
    }
    */

    if (prImg->u1MemClass==EXTERNAL_RING_IN_DOUBLE_OUT)
    {
        //set file size
        //PNG_SetFileSize(prPngInst, 359712);
        //decode cmd from vdec
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_DMX);
	    ASSERT(prFbmPool != NULL);
	    ASSERT(prFbmPool->u4Addr != 0);        
        vPngStateSTART(prPngInst, (UINT8*)prFbmPool->u4Addr, prFbmPool->u4Size);
    }
    else
    {
        prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
        ASSERT(prFbmPool != NULL);
        ASSERT(prFbmPool->u4Addr != 0);         
        
        #ifdef CC_JPEG_FBM_DYNAMIC_MAPPING
        if(!_bPngMapAddr)
        {
            i4PngDynamicFbmMap();
        }
        #endif
        
         for (i4 = 0; i4 < PNG_HAL_HW_INST_NUM; i4++)
        {
#ifdef PNG_VFY_AUTO_TEST        
            _tPngHalHwInst[i4].u4LZ77BufAddr = (UINT32)BSP_AllocAlignedDmaMemory(PNG_LZ77_SIZE, 128);
            _tPngHalHwInst[i4].u4LineBufAddr = (UINT32)BSP_AllocAlignedDmaMemory(4096 * 8, 128);
#else
            _tPngHalHwInst[i4].u4LZ77BufAddr = VIRTUAL((UINT32)(prFbmPool->u4Addr + PNG_LZ77_OFFSET));
            _tPngHalHwInst[i4].u4LineBufAddr  = VIRTUAL((UINT32)(prFbmPool->u4Addr + PNG_LINE_BUF_OFFSET));
#endif
        }
        vPngStateSTART(prPngInst, (UINT8*)prFbmPool->u4Addr, PNG_FIFO_SIZE);
    }

    return (void*)pvHandler;
}

static INT32 _SetPngDisp(PNGHANDLE hCurImg,PNG_INST_T *ptPngInst, IMG_PARAM_T* rInfo)
{
    FBM_POOL_T *prFbmPool, *prFbmPool2;
    UINT8* pu1Src;
    UINT32 u4AvailableSize;
    IMG_REGION_T* prDecOut;
    IMG_LIB_DISP_INFO_T* prDispInfo;
    IMG_LIB_INFO_T* prImg = (IMG_LIB_INFO_T*)hCurImg;
    prDispInfo = (IMG_LIB_DISP_INFO_T*)&prImg->rDispInfo;

    ASSERT(ptPngInst != NULL);
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_PNG);
   
	#ifdef CC_FBM_3D_OSD
	prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPG_VDP);
	#else
    prFbmPool2 = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_JPEG_OSD1);
	#endif    
	
    ASSERT(prFbmPool != NULL);
    ASSERT(prFbmPool->u4Addr != 0);
    ASSERT(prFbmPool2 != NULL);
    ASSERT(prFbmPool2->u4Addr != 0);

    ptPngInst->u4SrcX      = rInfo->rSrcRegion.u4SrcStartX;
    ptPngInst->u4SrcY      = rInfo->rSrcRegion.u4SrcStartY;
    ptPngInst->u4SrcWidth  = rInfo->rSrcRegion.u4SrcWidth;
    ptPngInst->u4SrcHeight = rInfo->rSrcRegion.u4SrcHeight;
#ifdef PNG_DECODING_USING_RZ_IOMMU
    ptPngInst->u4MMUTableAddr = rInfo->u4JpgUsingRzIOMMU_MMUTableAddr;
#endif
    vPngLog_L("ptPngInst->u4SrcWidth = %d,u4SrcHeight  = %d\n",ptPngInst->u4SrcWidth,ptPngInst->u4SrcHeight);
    pu1Src = rInfo->rDstRegion.pu1Addr;
    if (pu1Src == NULL)
    {
        u4AvailableSize = prFbmPool->u4Size - JPG_TOTAL_BUFFER_SIZE;
        if (u4AvailableSize < ((rInfo->rDstRegion.u4Pitch * rInfo->rDstRegion.u4Height) <<1))
        {
            return -1;
        }
        pu1Src = (UINT8*)(prFbmPool->u4Addr + JPG_TOTAL_BUFFER_SIZE);
        rInfo->rDstRegion.u4Pitch = 0;	//modified later to fit scaling size
    }
    //check buffer limitation
    if (prImg->u1MemClass==EXTERNAL_RING_IN_DOUBLE_OUT)
    {
        //for video plane purpose, ARGB temp buffer. prFbmPool2->u4Addr
        ptPngInst->pvDst_OnVideoPlane       = pu1Src;
        prDispInfo->pu1DisplayStart[0] = ptPngInst->pvDst       = (VOID*)prFbmPool2->u4Addr;
        prDispInfo->u4DispPitch =  ptPngInst->u4DstPitch  = rInfo->rDstRegion.u4Pitch*4;
        prDispInfo->u4DisplayClrMode = ptPngInst->eDispColorMode = rInfo->eDispColorMode;
        ptPngInst->eDispFinalColorMode = Y_CBCR420_SEP16;
    }
    else
    {
        if (rInfo->eDispColorMode == Y_CBCR422_SEP16)
        {
            ptPngInst->pvDst_OnVideoPlane       = pu1Src;
            prDispInfo->pu1DisplayStart[0] = ptPngInst->pvDst  = (UINT8*)(prFbmPool->u4Addr + PNG_RZ_TEMP_BUF_OFFSET + PNG_RZ_TEMP_BUF_SIZE);

            prDispInfo->u4DispPitch =  ptPngInst->u4DstPitch  = rInfo->rDstRegion.u4Pitch*4;
             ptPngInst->eDispColorMode = AYCBCR8888_DIRECT32;
             ptPngInst->eDispFinalColorMode = Y_CBCR422_SEP16;
             prDispInfo->u4DisplayClrMode = Y_CBCR422_SEP16;
        }
		else if(rInfo->eDispColorMode == Y_CBCR420_SEP16)
        {
            ptPngInst->pvDst_OnVideoPlane       = pu1Src;
            prDispInfo->pu1DisplayStart[0] = ptPngInst->pvDst  = (UINT8*)(prFbmPool->u4Addr + PNG_RZ_TEMP_BUF_OFFSET + PNG_RZ_TEMP_BUF_SIZE);

            prDispInfo->u4DispPitch =  ptPngInst->u4DstPitch  = rInfo->rDstRegion.u4Pitch*4;
             ptPngInst->eDispColorMode = AYCBCR8888_DIRECT32;
             ptPngInst->eDispFinalColorMode = Y_CBCR420_SEP16;
             prDispInfo->u4DisplayClrMode = Y_CBCR420_SEP16;
        }
        else
        {
            prDispInfo->pu1DisplayStart[0] = ptPngInst->pvDst       = pu1Src;
            prDispInfo->u4DispPitch =  ptPngInst->u4DstPitch  = rInfo->rDstRegion.u4Pitch;
            prDispInfo->u4DisplayClrMode = ptPngInst->eDispColorMode = rInfo->eDispColorMode;
            ptPngInst->eDispFinalColorMode = rInfo->eDispColorMode;
        }
    }

    prDispInfo->u4StartX[0] =  ptPngInst->u4DstX      = rInfo->rDstRegion.u4StartX;
    prDispInfo->u4StartY[0] = ptPngInst->u4DstY      = rInfo->rDstRegion.u4StartY;
    prDispInfo->u4DisplayW = ptPngInst->u4DstWidth  =rInfo->rDstRegion.u4Width;
    prDispInfo->u4DisplayH = ptPngInst->u4DstHeight = rInfo->rDstRegion.u4Height;
    ptPngInst->eDstCm      = (GFX_COLORMODE_T)(rInfo->u4DecColorMode);
    prDispInfo->u4Rotate =  (UINT32)(rInfo->eRotateDegree);
    ptPngInst->eRotate =  rInfo->eRotateDegree;

    prDispInfo->u4ScaleFMT = (UINT32)E_YCFMT_RES_2;
    //display image Y:Cr h, w ratio
    prDispInfo->au4FactorH[1] = 0;		//h, Y:Cb = 2:1
    prDispInfo->au4FactorW[1]= 0;			//w, Y:Cb = 2:1
    prDispInfo->au4FactorH[2] = 0;		//h, Y:Cb = 2:1
    prDispInfo->au4FactorW[2]= 0;			//w, Y:Cb = 2:1

    vPngLog_L("ptPngInst->u4DstWidth = %d,u4DstHeight  = %d\n",ptPngInst->u4DstWidth,ptPngInst->u4DstHeight);
    if ((ptPngInst->u4DstWidth==0) || (ptPngInst->u4DstHeight==0))
    {
        if ((ptPngInst->u4SrcWidth > DISP_PNG_W_LMT) || (ptPngInst->u4SrcHeight  > DISP_PNG_H_LMT))
        {
            prDispInfo->u4DisplayW = ptPngInst->u4DstWidth = DISP_PNG_W_LMT;
            prDispInfo->u4DisplayH = ptPngInst->u4DstHeight = DISP_PNG_H_LMT;
            prDispInfo->u4DispPitch = ptPngInst->u4DstPitch  = DISP_PNG_W_LMT * 4;
        }
        else
        {
            prDispInfo->u4DisplayW = ptPngInst->u4DstWidth = ptPngInst->u4SrcWidth;
            prDispInfo->u4DisplayH = ptPngInst->u4DstHeight = ptPngInst->u4SrcHeight ;
            prDispInfo->u4DispPitch = ptPngInst->u4DstPitch = ((ptPngInst->u4SrcWidth + 15) / 16) * 16 * 4;
        }
		
		LOG(0,"-- DispInfo Resolution:w[%d],h[%d],pitch[%d]\n",prDispInfo->u4DisplayW,
			prDispInfo->u4DisplayH, prDispInfo->u4DispPitch);		
    }
    if (ptPngInst->u4DstPitch == 0)
    {
        prDispInfo->u4DispPitch = ptPngInst->u4DstPitch =  ((ptPngInst->u4DstWidth + 15) / 16) * 16  * 4;
    }
    #ifdef __MODEL_slt__
    prDispInfo->u4DispPitch =  ptPngInst->u4DstPitch  =  ((ptPngInst->u4DstWidth  + 15) / 16) * 16  * 4;
    #endif
    /*
    _CalKeepRatioRegion(rInfo->bKeepRatio, ptPngInst->u4SrcWidth, ptPngInst->u4SrcHeight,
                        prDispInfo->u4DisplayW, prDispInfo->u4DisplayH,
                        &prDispInfo->u4ScaleW, &prDispInfo->u4ScaleH);
    */
    prDecOut = rInfo->prDecOut;
    if ((prImg->u1MemClass==EXTERNAL_RING_IN_DOUBLE_OUT) 
		|| (rInfo->eDispColorMode == Y_CBCR422_SEP16) || (rInfo->eDispColorMode == Y_CBCR420_SEP16))
    {
        //for video plane purpose, keep cbcr address
        prDecOut->pu1Addr = ptPngInst->pvDst_OnVideoPlane;		
		if (rInfo->rDstRegion.u4CbCrOffset == 0)
		{
			rInfo->rDstRegion.u4CbCrOffset = prDispInfo->u4DispPitch * prDispInfo->u4DisplayH/4;
			LOG(0, "set ycbcroffset = 0x%x\n", ptPngInst->u4CbCrOffset);
		}
				
        prDecOut->u4CbCrOffset = rInfo->rDstRegion.u4CbCrOffset;
        ptPngInst->u4CbCrOffset = rInfo->rDstRegion.u4CbCrOffset;
        //for video plane purpose, pitch/4
        prDecOut->u4Pitch = ptPngInst->u4DstPitch/4;
    }
    else
    {
        prDecOut->u4Pitch = ptPngInst->u4DstPitch;
		prDecOut->pu1Addr = ptPngInst->pvDst;
        prDecOut->u4CbCrOffset = 0;
    }
    prDecOut->u4StartX = prDispInfo->u4StartX[0];
    prDecOut->u4StartY = prDispInfo->u4StartY[0];
    prDecOut->u4Width = prDispInfo->u4DisplayW;
    prDecOut->u4Height = prDispInfo->u4DisplayH;
    prDecOut->u4TotalLine = prDispInfo->u4DisplayH;
    prDecOut->u4Depth = 4;
    return 0;
}
INT32 _PNG_DecPrepare(PNGHANDLE hCurImg, IMG_PARAM_T *prInfo)
{
    IMG_LIB_INFO_T* prImg = (IMG_LIB_INFO_T*)hCurImg;
    PNG_INST_T* prPngInst = (PNG_INST_T*)prImg->hCurDec;
    if (prPngInst->ePngState != PNG_STATE_INIT)
    {
        return E_IMG_INIT_FAIL;
    }

    if (_SetPngDisp(prImg,prPngInst,prInfo) < 0)
    {
        return E_IMG_INIT_FAIL;
    }

    vPngStateINIT(prPngInst);
    if (prPngInst->i4PngErrorType < 0)
    {
        return E_IMG_INIT_FAIL;
    }
    vPngStatePRSHDR(prPngInst);
    if (prPngInst->i4PngErrorType < 0)
    {
        return E_IMG_INIT_FAIL;
    }

    vPngStateINITHAL(prPngInst);
    if (prPngInst->i4PngErrorType < 0)
    {
        return E_IMG_INIT_FAIL;
    }
    return E_IMG_OK;
}

INT32 _PNG_DecOneSegment(PNGHANDLE hCurImg, UINT32 u4ImgID,IMG_PARAM_T *prInfo)
{
    IMG_LIB_INFO_T* prImg = (IMG_LIB_INFO_T*)hCurImg;
    PNG_INST_T* prPngInst = (PNG_INST_T*)prImg->hCurDec;
    //ENUM_IMG_ROTATE_PHASE eRotateDegree=prInfo->eRotateDegree;
    if (prPngInst->ePngState != PNG_STATE_DECODING)
    {
        return E_IMG_INIT_FAIL;
    }
    if (prImg->bStopDec)
    {
        return E_IMG_CANCEL;
    }
    vPngStateDECODING(prPngInst);
    if (prPngInst->i4PngErrorType < 0)
    {
	    vPngLog_H("[Png] _PNG_DecOneSegment, error type = %d\n", prPngInst->i4PngErrorType);
        return E_IMG_DEC_FAIL;
    }
    if (prPngInst->fgAbort)
    {
        switch (prPngInst->u4NfyStatus)
        {
        case IMG_NFY_FINISHED:
            return E_IMG_DEC_FINISH;
        case IMG_NFY_STOP_DONE:
            return E_IMG_CANCEL;
        default:
            return E_IMG_DEC_FAIL;
        }
    }
    return E_IMG_OK;
}
INT32 PNG_MwFillBuffer(PNGHANDLE hDec, void **ppvBuf,UINT32 *pu4Size, UINT32 u4AddSize)
{
    UINT32 u4RingWP;
    UINT32 u4RingSpace;
    UINT32 u4RingBufSize;
    UINT32 u4RingRP;

    PNG_INST_T	*prPngInst;
    PNG_FIFO_INFO_T *prPngFifo;
    /* check return	pointer	*/
    if ((ppvBuf	== NULL) ||	(pu4Size ==	NULL))
    {
        return PNG_DRV_RTN_FAIL;
    }
    /* check input handle */
    if (hDec ==	(PNGHANDLE)NULL)
    {
        return PNG_DRV_RTN_FAIL;
    }

    prPngInst = (PNG_INST_T*)hDec;
    prPngFifo = &(prPngInst->tFifoInfo);
    u4RingBufSize = prPngFifo->u4FifoEnd - prPngFifo->u4FifoStart;
    u4RingRP = prPngFifo->u4RdPoint - prPngFifo->u4FifoStart;

    u4RingWP = u4RingRP + prPngFifo->u4ValidData + u4AddSize;
    u4RingSpace = u4RingBufSize - (prPngFifo->u4ValidData  + u4AddSize);

    if (u4RingWP >= u4RingBufSize)
    {
        u4RingWP -= u4RingBufSize;
    }

    vPngLog_L("RP %08x WP %08x Data %d Space %d\n", prPngFifo->u4RdPoint, u4RingWP, prPngFifo->u4ValidData, u4RingSpace);

    if (u4RingSpace <= 64)
    {
        //read pointer overrun at last row is acceptable
        //but should takecare if happens when stream buffer is too
        //small for one single row
        *pu4Size = 0;
        *ppvBuf = (void*)prPngFifo->u4FifoStart;
        return (INT32)PNG_DRV_RTN_OK;
    }

    u4RingSpace -= 64;

    if ((u4RingWP + u4RingSpace) < u4RingBufSize)
    {
        *pu4Size = u4RingSpace;
    }
    else
    {
        *pu4Size = u4RingBufSize - u4RingWP;
    }

    *ppvBuf = (void*)((UINT32)prPngFifo->u4FifoStart + u4RingWP);
    vPngLog_L("Fill Buffer  %08x ,size  %d\n", (UINT32)*ppvBuf, (UINT32)*pu4Size);
    return (INT32)PNG_DRV_RTN_OK;

}

void PNG_SetFileSize(PNGHANDLE hDec,UINT32 u4FileSize)
{
    PNG_INST_T	*prPngInst;
    prPngInst = (PNG_INST_T*)hDec;
    prPngInst->u4ImgSize = u4FileSize;
    vPngLog_L("Png File size  %d\n",u4FileSize);
}
void PNG_FillBsBuf_End(PNGHANDLE hPngInst, UINT32 entry1_size, UINT32 entry2_size, BOOL beof)
{
    PNG_INST_T *prPngInst;
    PNG_FIFO_INFO_T *rPngFifo;
    UINT32 u4VldWptrOffset;
    UINT32 u4RingBufSize;
    UINT32 u4RingRP;
    //CRIT_STATE_T crit;
    ASSERT(hPngInst);
    prPngInst = (PNG_INST_T*)hPngInst;

    rPngFifo = &(prPngInst->tFifoInfo);

    u4RingBufSize = rPngFifo->u4FifoEnd - rPngFifo->u4FifoStart;
    u4RingRP = rPngFifo->u4RdPoint - rPngFifo->u4FifoStart;
    vPngLog_L("PNG_FillBsBuf_End entry1_size = 0x%08x,entry2_size = 0x%08x, beof = %d \n",
              entry1_size,entry2_size,beof);

    HalFlushInvalidateDCacheMultipleLine(rPngFifo->u4FifoStart,u4RingBufSize);
    // update ringbuffer data length -----------------------------------
    //crit = x_crit_start();	//protect u4RingData
    if (entry1_size > 0)
    {
        rPngFifo->u4ValidData += entry1_size;
    }
    if (entry2_size >0)
    {
        rPngFifo->u4ValidData += entry2_size;
    }

    rPngFifo->fgEoRange = beof;
    if (beof)
    {
        rPngFifo->u4WrPoint = 0xFFFFFFFF;
    }
    else
    {
        u4VldWptrOffset = u4RingRP + rPngFifo->u4ValidData;
        u4VldWptrOffset = (u4VldWptrOffset >= u4RingBufSize) ? (u4VldWptrOffset - u4RingBufSize) : u4VldWptrOffset;

        rPngFifo->u4WrPoint = u4VldWptrOffset + rPngFifo->u4FifoStart;
    }
    //x_crit_end(crit);
}

void _PNG_SetFillBsBufEnd(PNGHANDLE hCurImg, UINT32 u4Entry1Size, UINT32 u4Entry2Size, BOOL bIsEOF)
{
    IMG_LIB_INFO_T* prImg = (IMG_LIB_INFO_T*)hCurImg;

    //--- error checking-----------------------------------------------
    if (prImg == NULL)
    {
        return;
    }
    PNG_FillBsBuf_End(prImg->hCurDec, u4Entry1Size, u4Entry2Size, bIsEOF);
    prImg->bIsEOF = bIsEOF;
}

ENUM_IMG_RET _PNG_Stop(PNGHANDLE hCurImg)
{
    IMG_LIB_INFO_T* prImg = (IMG_LIB_INFO_T*)hCurImg;
    PNG_INST_T* prPngInst = NULL;

    if (prImg == NULL)
    {
        return E_IMG_NULL_HANDLER;
    }

    prPngInst = (PNG_INST_T*)prImg->hCurDec;
    //printk("_PNG_Stop lock %d \n", current->pid);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 5); 
    x_sema_lock(prPngInst->hPngSema, X_SEMA_OPTION_WAIT);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 6); 

    prImg->bStopDec = TRUE;
    if (prPngInst->ePngState != PNG_STATE_IDLE)
    {
        VERIFY(i4PngHalSetStop(prPngInst->u4HwInstId) == PNG_HAL_RTN_OK);
        vPngAbort(prPngInst, IMG_NFY_STOP_DONE, TRUE);
    }
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 7); 
    x_sema_unlock(prPngInst->hPngSema);
    //printk("_PNG_Stop unlock %d \n", current->pid);
    u4hPNGSemaphoreDBGInsert(u4hPngSema, 8); 
    return E_IMG_OK;
}
void vPngAbort(PNG_INST_T *ptPngInst, IMG_NFY_STATE_T eState, BOOL fgNfy)
{
    INT32 i4Ret;


    if (fgNfy)
    {
        vPngLog_L( "[Png] State:%d, Current STC:%d\n", eState, u4PngGetSTC());
        if (eState == IMG_NFY_ERROR)
        {
            vPngLog_H( "[Png] IMG_NFY_ERROR ImgId=%x, ErrCode=%d, State=%d, DecState=%d\n", ptPngInst->u4ImgId, ptPngInst->i4PngErrorType, ptPngInst->ePngState, ptPngInst->ePngDecState);
        }
    }

    //clean event
    // i4Ret = x_ev_group_set_event(ptPngInst->hPngEvent, 0x00, X_EV_OP_CLEAR);
    //VERIFY(i4Ret == OSR_OK);
    //stop current decode
    vPngSWReset(2);
   // i4Ret = i4PngHalReset(ptPngInst->u4HwInstId);
	
 //  VERIFY(i4Ret == PNG_HAL_RTN_OK); 
    i4Ret = i4PngHalDisable(ptPngInst->u4HwInstId);
    VERIFY(i4Ret == PNG_HAL_RTN_OK);
    i4Ret = i4PngHalSetAutoSkipDisable(ptPngInst->u4HwInstId);
    VERIFY(i4Ret == PNG_HAL_RTN_OK);
    //set state
    ptPngInst->ePngState = PNG_STATE_IDLE;
    ptPngInst->ePngDecState = PNG_DEC_STATE_IDLE;

}

#if 0
INT32 i4PngGetProgress(UINT16 u2CompId, UINT32* pu4Progress)
{
    PNG_INST_T *ptPngInst = &(_artPngInst[u2CompId]);

    *pu4Progress = ptPngInst->u4Progress;
    return RMR_OK;
}
#endif

