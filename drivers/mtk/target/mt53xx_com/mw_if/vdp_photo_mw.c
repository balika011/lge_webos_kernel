/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdp_mw.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file vdp_photo_mw.c
 *  Brief of file vdp_photo_mw.c.
 *  Details of file vdp_photo_mw.c (optional).
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef CC_DFB_SUPPORT_VDP_LAYER

#include "vdo_if.h"
#include "drv_tdtv_drvif.h"
#include "vdp_if.h"
#include "gfx_if.h"

#include "imgrz_if.h"
#include "imgrz_drvif.h"




#include "osd_drvif.h"
#define DEFINE_IS_LOG OSD_IsLog
#include "x_debug.h"

#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "fbm_drvif.h"
//#include "x_chip_id.h"
#include "x_common.h"
#include "x_rm.h"
#include "x_rm_dev_types.h"
#include "x_os.h"
#include "x_gfx.h"
#include "x_plane_mxr.h"
#include "x_sys_name.h"
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "drvcust_if.h"
#include "x_vdp_photo_mw.h"



LINT_EXT_HEADER_END

#define PHOTO_VDP_LAYER_MAX       0x5
#define TMPBUF_WIDTH 1280
#define TMPBUF_HEIGHT 720
#define TMPBUF_BPP    4

#define BUFFER_EMPTY 0
#define BUFFER_FREE 1
#define BUFFER_ALLOC 2

#define VDP_MAX_BUFFER_SIZE 0x400000//>(1920x1088x2), == 0x1000000/4



static UINT32 _au4FbmVdpBufferAddr[4];
static UINT32 _au4FbmVdpBufferSize[4];
static UINT8 _au1FbmVdpBufferStstus[4]; // 0:free, 1:allocated
static UINT32 _u4BufferVdpNumber = 0;



static BOOL _fgVdpInit = FALSE;
static unsigned char _ucFbgId = 0;
//static unsigned char _aucFbId[2] = {FBM_FB_ID_UNKNOWN, FBM_FB_ID_UNKNOWN};
//static unsigned int _au4FbYAddr[2] = {NULL, NULL};
//static unsigned int _au4FbCAddr[2] = {NULL, NULL};
//static unsigned char _u1FrontFbId = 0;
static E_TDTV_DECODER_INPUT _ePrevDetectedMode = E_TDTV_DECODER_INPUT_2D_P; 
//static unsigned int _u4VdpWidth;
//static unsigned int _u4VdpHeight;
static UINT32 _u4CropBottomOffset = 0;
static UINT32 _u4CropTopOffset = 0;
static UINT32 _u4CropLeftOffset = 0;
static UINT32 _u4CropRightOffset = 0;
static BOOL	  _bIs3DMode = FALSE;
//static UCHAR	_fgLastVdpEnableFg = 0;


UINT32 _u4VdpTmpBufAddr = 0;


static mt53fb_vdp_crop_rect mt53fb_vdp_crop_rect_array[PHOTO_VDP_LAYER_MAX];
static mt53fb_imagebuffer _gVdpBufInfo;  


typedef struct _OSD_PALETTE_HANDLE_T
{
	void* pvFreePoint;
	void* pvPalette;
	GFX_PALETTE256_T rPalette;
	GFX_PALETTE_INIT_T rInit;
	GFX_PALETTE_PARAMS_T rParams;
} OSD_PALETTE_HANDLE_T;

typedef struct _OSD_BITMAP_HANDLE_T
{
	void* pvFreePoint;
	GFX_BITMAP_INIT_T rInit;
	GFX_BITMAP_LOCK_INFO_T rLockInfo;
	GFX_BITMAP_PALETTE_T rPalette;
	UINT32 u4BitsPerPixel;
} OSD_BITMAP_HANDLE_T;



#define RET_ON_NULL(PT)                                                        \
    do                                                                         \
    {                                                                          \
        if ((void*)(PT) == NULL)                                               \
        {                                                                      \
            return -(INT32)OSD_RET_INV_ARG;                                    \
        }                                                                      \
    } while (0)

#define SET_COMMAND(FUNC)                                                      \
			do																		   \
			{																		   \
				if ((INT32)(FUNC) == (INT32)OSD_RET_OK) 							   \
				{																	   \
					return RMR_OK;													   \
				}																	   \
				else																   \
				{																	   \
					return RMR_DRV_SET_FAILED;										   \
				}																	   \
			} while (0)





#define INIT_VDPBUFFER(IDX)                                                    \
    do                                                                         \
    {                                                                          \
        if(_gVdpBufInfo.u4Size > 0)                      \
        {                                                                      \
            _au4FbmVdpBufferAddr[(IDX)-1] = _gVdpBufInfo.u4PhyAddr+((IDX)-1) * VDP_MAX_BUFFER_SIZE;            \
            _au4FbmVdpBufferSize[(IDX)-1] = VDP_MAX_BUFFER_SIZE;               \
            _u4BufferVdpNumber++;                                              \
        }                                                                      \
    }while (0)

static void _VdpPhotoInitBuffer(void)
{
    x_memset(_au4FbmVdpBufferAddr, 0, sizeof(_au4FbmVdpBufferAddr));
    x_memset(_au4FbmVdpBufferSize, 0, sizeof(_au4FbmVdpBufferSize));
    x_memset(_au1FbmVdpBufferStstus, BUFFER_EMPTY, sizeof(_au1FbmVdpBufferStstus));

    INIT_VDPBUFFER(1);
    INIT_VDPBUFFER(2);
    INIT_VDPBUFFER(3);
    INIT_VDPBUFFER(4);

    x_memset(_au1FbmVdpBufferStstus, BUFFER_FREE, _u4BufferVdpNumber);
}


static UINT32 _VdpPhotoAllocateFbmBuffer(UINT32* pu4BufferSize)
{
    UINT32 i;

    for (i = 0; i < _u4BufferVdpNumber; i++)
    {
        if (_au1FbmVdpBufferStstus[i] == BUFFER_FREE)
        {
            _au1FbmVdpBufferStstus[i] = BUFFER_ALLOC;
            *pu4BufferSize = _au4FbmVdpBufferSize[i];
            return _au4FbmVdpBufferAddr[i];
        }
    }

    return (UINT32)NULL;
}


static void _VdpPhotoFreeFbmBuffer(void* pvBufferAddr)
{
    UINT32 i;

    for (i = 0; i < _u4BufferVdpNumber; i++)
    {
        if (_au4FbmVdpBufferAddr[i] == (UINT32)pvBufferAddr)
        {
            _au1FbmVdpBufferStstus[i] = BUFFER_FREE;
            break;
        }
    }
}


//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------
static UINT32 _VdpPhotoGetMwColorMode(UINT32 u4DrvColorMode)
{
    switch (u4DrvColorMode)
    {
    case (UINT32)GFX_COLORMODE_AYCbCr_CLUT2:
        return (UINT32)OSD_CM_YCBCR_CLUT2;

    case (UINT32)GFX_COLORMODE_AYCbCr_CLUT4:
        return (UINT32)OSD_CM_YCBCR_CLUT4;

    case (UINT32)GFX_COLORMODE_AYCbCr_CLUT8:
        return (UINT32)OSD_CM_YCBCR_CLUT8;

    case (UINT32)GFX_COLORMODE_CbYCrY_16:
        return (UINT32)OSD_CM_CBYCRY422_DIRECT16;

    case (UINT32)GFX_COLORMODE_YCbYCr_16:
	case (UINT32)GFX_COLORMODE_YUV_422_RS:
        return (UINT32)OSD_CM_YCBYCR422_DIRECT16;

    case (UINT32)GFX_COLORMODE_AYCbCr_D8888:
        return (UINT32)OSD_CM_AYCBCR8888_DIRECT32;

    case (UINT32)GFX_COLORMODE_ARGB_CLUT2:
        return (UINT32)OSD_CM_RGB_CLUT2;

    case (UINT32)GFX_COLORMODE_ARGB_CLUT4:
        return (UINT32)OSD_CM_RGB_CLUT4;

    case (UINT32)GFX_COLORMODE_ARGB_CLUT8:
        return (UINT32)OSD_CM_RGB_CLUT8;

    case (UINT32)GFX_COLORMODE_RGB_D565 :
        return (UINT32)OSD_CM_RGB565_DIRECT16;

    case (UINT32)GFX_COLORMODE_ARGB_D1555:
        return (UINT32)OSD_CM_ARGB1555_DIRECT16;

    case (UINT32)GFX_COLORMODE_ARGB_D4444:
        return (UINT32)OSD_CM_ARGB4444_DIRECT16;

    case (UINT32)GFX_COLORMODE_ARGB_D8888:
    default:
        return (UINT32)OSD_CM_ARGB8888_DIRECT32;
    }
}


//-----------------------------------------------------------------------------
/** Brief
 *  @param
 *  @return
 */
//-----------------------------------------------------------------------------
static UINT32 _VdpPhothoGetBitsPerPixel(UINT32 u4DrvColorMode)
{
	switch (u4DrvColorMode)
	{
	case OSD_CM_YCBCR_CLUT2:
	case OSD_CM_RGB_CLUT2:
		return 2;

	case OSD_CM_YCBCR_CLUT4:
	case OSD_CM_RGB_CLUT4:
		return 4;

	case OSD_CM_YCBCR_CLUT8:
	case OSD_CM_RGB_CLUT8:
		return 8;

	case OSD_CM_CBYCRY422_DIRECT16:
	case OSD_CM_YCBYCR422_DIRECT16:
	case OSD_CM_RGB565_DIRECT16:
	case OSD_CM_ARGB1555_DIRECT16:
	case OSD_CM_ARGB4444_DIRECT16:
		return 16;

	case OSD_CM_AYCBCR8888_DIRECT32:
	case OSD_CM_ARGB8888_DIRECT32:
	default:
		return 32;
	}
}



static BOOL _VdpPhoto3DCropFuncCB(TDTV_3D_CROP_INTO_T* prCropInfo)
{
    prCropInfo->fgIsFixSrcAspectRatio = 1;
    prCropInfo->fgIsFullResolution = 1;    
    prCropInfo->u4CropBottomOffset = _u4CropBottomOffset;
    prCropInfo->u4CropTopOffset = _u4CropTopOffset;    
    prCropInfo->u4CropLeftOffset = _u4CropLeftOffset;        
    prCropInfo->u4CropRightOffset = _u4CropRightOffset;     
	return 0;
}

static void _VdpPhotoInitFbmBuffer(void)
{
    FBM_POOL_T *prFbmPool;
	mt53fb_imagebuffer _rImgBufInfo; 
	FBM_Init();

	prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_SCPOS_MAIN);
	ASSERT(prFbmPool != NULL);
	ASSERT(prFbmPool->u4Addr != (UINT32)NULL);                
	_rImgBufInfo.u4Size = prFbmPool->u4Size;
	_rImgBufInfo.u4PhyAddr = prFbmPool->u4Addr;
	_rImgBufInfo.u4VirtAddr = VIRTUAL(_rImgBufInfo.u4PhyAddr);

	printf("PHOTO FROM FBM: %x, PHY:%x, SIZE: %x\n",
		       _rImgBufInfo.u4VirtAddr,
		       _rImgBufInfo.u4PhyAddr,
		       _rImgBufInfo.u4Size);           

	prFbmPool = FBM_GetPoolInfo((UCHAR)FBM_POOL_TYPE_JPG_VDP);
	ASSERT(prFbmPool != NULL);
	ASSERT(prFbmPool->u4Addr != (UINT32)NULL);   
		// Handle start offset alignment              
    _gVdpBufInfo.u4PhyAddr = prFbmPool->u4Addr;  

		// Handle size alignment
	_gVdpBufInfo.u4Size = prFbmPool->u4Addr + prFbmPool->u4Size - _gVdpBufInfo.u4PhyAddr;
	if(_gVdpBufInfo.u4Size < VDP_MAX_BUFFER_SIZE*4)
	{
	  printf("please check FBM part about VDP buffer allocate\n");
	}
	_gVdpBufInfo.u4VirtAddr = VIRTUAL(_gVdpBufInfo.u4PhyAddr);
	_VdpPhotoInitBuffer();

	printf("PHOTO JPEG VDP: %x, PHY:%x, SIZE: %x\n",
		       _gVdpBufInfo.u4VirtAddr,
		       _gVdpBufInfo.u4PhyAddr,
		       _gVdpBufInfo.u4Size);     
}

static BOOL _VdpPhotoFBGInit(unsigned int u4Width, unsigned int u4Height, unsigned int u4Pitch)
{
     unsigned char ucFbgType, ucFbgId;
     FBM_SEQ_HDR_T* prFbmSeqHdr;     
     VDP_REGION_T rRegion = {0,}; 
     UNUSED(ucFbgType);
	 UNUSED(ucFbgId);
	 UNUSED(prFbmSeqHdr);
    if(!_fgVdpInit)
    {
        UCHAR ucFbgType, ucFbgId;
        FBM_SEQ_HDR_T* prFbmSeqHdr;

        // Set video src
        bApiVideoSetVideoSrc(0, SV_VS_DT1);
        VDP_SetSrcRegion(0, 1, rRegion);        
        VDP_SetOutRegion(0, 1, rRegion);  
		// VDP_GetEnable(0,&_fgLastVdpEnableFg);
        VDP_SetEnable(0, 1);        
        VDP_SetMode(0, VDP_MODE_BG);
        VDP_SetInput(0, 0, 0);    
   
        ucFbgType = FBM_SelectGroupType(u4Width, u4Height);
        FBM_SetColorMode(FBM_CM_422);
        ucFbgId = FBM_CreateGroup(ucFbgType, FBM_VDEC_JPEG, u4Width, u4Height);
        if(ucFbgId == FBM_FBG_ID_UNKNOWN)
        {
            printf("Create Fbg Fail!! input resolution %dx%d\n", u4Width, u4Height);               
            return ucFbgId;
        }
        FBM_SetPlayMode(ucFbgId, FBM_FBG_MM_MODE);
        FBM_SetSyncStc(ucFbgId, 0, 0); //none
        FBM_FbgChgNotify(ucFbgId, 0);

		if (u4Pitch == 0)
			u4Pitch = ((u4Width + 15) >> 4) << 4;
    
        prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
        prFbmSeqHdr->u2HSize = u4Width;
        prFbmSeqHdr->u2VSize = u4Height;
        prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
        prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
        prFbmSeqHdr->u2LineSize = u4Pitch;        
        prFbmSeqHdr->fgProgressiveSeq = TRUE;
        prFbmSeqHdr->fgRasterOrder = TRUE;
        prFbmSeqHdr->fg422Mode = TRUE;
        prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
    
        FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_SEQ_CHG);

        // Register 3D crop CB
        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, _VdpPhoto3DCropFuncCB);

        _ucFbgId = ucFbgId;
        _fgVdpInit = TRUE;
        
    }
                
     return TRUE;
 }


static BOOL _VdpPhotoFBFlip(unsigned char ucFbgId, 
	unsigned char* pu1SrcYAddr, unsigned char* pu1SrcCAddr, BOOL fgIs3DMode, unsigned int ui4_pitch,
	unsigned int u4CropBottomOffset, unsigned int u4CropTopOffset, unsigned int u4CropLeftOffset, unsigned int u4CropRightOffset)
{
    UINT32 u43DInOut, u2InFmt=0, u2OutFmt;
    FBM_SEQ_HDR_T* prFbmSeqHdr;

    if(_fgVdpInit)
    {
        UCHAR ucFbId;
        UINT32 u4DstYAddr, u4DstCAddr, u4Pitch, u4TotalLine, u4W;
        E_TDTV_DECODER_INPUT eDetectedMode;
        FBM_PIC_HDR_T* prFbmPicHdr;           
        
        //Get back frame buffer
        ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
        if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
        {
            printf("Get empty frame buffer failed.\n");                
            return FALSE;
        }
        
        printf("Get empty frame buffer id = %d.\n", ucFbId);  
       
        
        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DstYAddr, &u4DstCAddr);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if ((prFbmPicHdr == NULL) || (u4DstYAddr == 0) || (u4DstCAddr == 0))
        {
            printf("Alloc frame buffer failed.\n");              
            return FALSE;
        }     
        prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
        prFbmPicHdr->fgProgressiveFrm = TRUE; 
        prFbmPicHdr->fgRepFirstFld = FALSE;
        prFbmPicHdr->fgTopFldFirst = FALSE;
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);    //default
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);
        
        //Bit blit input pu1SrcYAddr, pu1SrcCAddr to destination u4DstYAddr, u4DstCAddr
        FBM_GetFrameBufferSize(ucFbgId, &u4Pitch, &u4TotalLine);
       
        u4W = PANEL_GetPanelWidth();
        u4Pitch = ui4_pitch;
        u4TotalLine = PANEL_GetPanelHeight();

        printf("Alloc frame buffer Yaddr = 0x%8x, Caddr = 0x%8x, pitch = %d, width = %d, height = %d\n", 
                u4DstYAddr, u4DstCAddr, u4Pitch, u4W, u4TotalLine);   
           

        //Y
        GFX_Lock();
        GFX_SetSrc(pu1SrcYAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)u4DstYAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_BitBlt(0, 0, 0, 0, u4W, u4TotalLine);
        //C
        GFX_SetSrc(pu1SrcCAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_SetDst((UINT8 *)u4DstCAddr, (UINT32)CM_RGB_CLUT8, u4Pitch);
        GFX_BitBlt(0, 0, 0, 0, u4W, u4TotalLine);
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
        printf(" u2InFmt =%d PHOTO buffer Yaddr = 0x%8x, Caddr = 0x%8x, 3D mode = %d\n", 
                u2InFmt, (UINT32)pu1SrcYAddr, (UINT32)pu1SrcCAddr, fgIs3DMode);     
        

        //Check 3D Type
        if(fgIs3DMode)
        {
            eDetectedMode = E_TDTV_DECODER_INPUT_SBS_P;
        }
        else
        {
            eDetectedMode = E_TDTV_DECODER_INPUT_2D_P;
        }
        u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);        
        if(eDetectedMode != _ePrevDetectedMode)
        {
//            u43DInOut = u4DrvTDTVMMModeQuery(eDetectedMode);
            u2InFmt = u43DInOut & 0xFFFF;
            u2OutFmt = (u43DInOut >> 16) & 0xFFFF;
            prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
            
            switch(u2OutFmt)
            {
                case E_TDTV_DECODER_OUTPUT_NATIVE:
                    prFbmSeqHdr->fgB2R3DEnable = FALSE;
                    break;
                case E_TDTV_DECODER_OUTPUT_3D_FS:
                    prFbmSeqHdr->fgB2R3DEnable = TRUE;
                    prFbmSeqHdr->fgB2RForce2D = FALSE;
                    prFbmSeqHdr->u4B2R3DWidthOffset = u4Pitch >> 1;
                    prFbmSeqHdr->u4B2R3DHeightOffset = 0;
                    prFbmSeqHdr->u1B2R3DType = B2R_3D_SIDE_BY_SIDE;
                    break;
                default:
                    break;
            }
            _ePrevDetectedMode = eDetectedMode;
            FBM_SetFrameBufferPicFlag(ucFbgId, ucFbId, FBM_MM_3D_CHG_FLAG);
        }
        
        //Display
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);

        // Unmute B2R
        VDP_SetMode(0, VDP_MODE_NORMAL);

    }
  
    
    return TRUE;
    
}


void _VdpPhotoResizePhase(UINT32 YSrc, UINT32 Csrc, 
									UINT32 SrcX, UINT32 SrcY, UINT32 SrcPitch, UINT32 SrcWidth, UINT32 SrcHeight,
	    							UINT32 YDst, UINT32 CDst, 
	    							UINT32 DstX, UINT32 DstY, UINT32 DstPitch, UINT32 DstWidth, UINT32 DstHeight)
{
	RZ_VDO_SCL_PARAM_SET_T rSclParam;
	
	IMGRZ_Lock();
	IMGRZ_ReInit();

	x_memset((void *)&rSclParam, 0, sizeof(RZ_VDO_SCL_PARAM_SET_T));

	// set scaling parameters
	rSclParam.u4SrcSwap 	= 0;
	rSclParam.u4IsRsIn		= 1;
	rSclParam.u4YSrcBase	= YSrc;
	rSclParam.u4YSrcBufLen	= SrcPitch;
	rSclParam.u4CSrcBufLen	= SrcPitch;
	rSclParam.u4YSrcHOffset = SrcX;
	rSclParam.u4YSrcVOffset = SrcY;
	rSclParam.u4YSrcW		= SrcWidth;
	rSclParam.u4YSrcH		= SrcHeight;

	rSclParam.u4CSrcBase	= Csrc;
	rSclParam.u4CSrcHOffset = SrcX >> 1;
	rSclParam.u4CSrcVOffset = SrcY;
	rSclParam.u4CSrcW		= SrcWidth >> 1;
	rSclParam.u4CSrcH		= SrcHeight;

	rSclParam.u4OutSwap 	= 0;
	rSclParam.u4IsRsOut 	= 1;
	rSclParam.u4OutMode 	= 1;
	rSclParam.u4IsVdo2Osd	= 0;

	rSclParam.u4YTgBase 	= YDst;
	rSclParam.u4YTgCM		= 0;
	rSclParam.u4YTgBufLen	= DstPitch;
	rSclParam.u4YTgHOffset	= DstX;
	rSclParam.u4YTgVOffset	= DstY;
	rSclParam.u4YTgW		= DstWidth;
	rSclParam.u4YTgH		= DstHeight;

	rSclParam.u4CTgBase 	= CDst;
	rSclParam.u4CTgW		= DstWidth >> 1;
	rSclParam.u4CTgH		= DstHeight;
	rSclParam.u4VdoCbCrSwap = 0;
	rSclParam.u4ABlend 		= (UINT32) 0x80;

	IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_VDOMD);
	IMGRZ_Scale((void *)(&rSclParam));

	IMGRZ_Flush();
	IMGRZ_Wait();
	IMGRZ_Unlock();

}


INT32 _VdpPhotoYCSepRotate(struct mt53fb_vdp_rotate *prBlit)
{
    INT32 i4Ret = 0;
    UINT32 i;
    INT32 i4Sx, i4Sy, i4Dx, i4Dy;
    
    GFX_Lock();
	GFX_SetSrc((UINT8 *)prBlit->u4Src, CM_RGB_CLUT8, prBlit->u4SrcPitch);
    GFX_SetDst((UINT8 *)prBlit->u4Dst, CM_RGB_CLUT8, prBlit->u4DstPitch);   

    if (prBlit->u4RotateOp == MT53FB_VDP_ROTATE_270)
    {
        // Rotating Y
        i4Sx = (INT32)prBlit->u4SrcX;
        i4Sy = (INT32)prBlit->u4SrcY;
        i4Dx = (INT32)prBlit->u4DstX;
        //i4Dy = (INT32)prBlit->u4DstY + prBlit->u4Width;  
        i4Dy = (INT32)prBlit->u4DstY + prBlit->u4Width - 1; 
        
        // rotated by 90" counter-clock-wise
        GFX_SetHoriToVertLineOpt(1);

        for(i = 0; i < prBlit->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prBlit->u4Width);

            i4Sy++;
            i4Dx++;
        } // ~for
    }
    else if(prBlit->u4RotateOp == MT53FB_VDP_ROTATE_90)
    {
        // Rotating Y
        i4Sx = (INT32)prBlit->u4SrcX;
        i4Sy = (INT32)prBlit->u4SrcY;
        i4Dx = (INT32)prBlit->u4DstX + prBlit->u4Height;
        i4Dy = (INT32)prBlit->u4DstY;               
        
        // rotated by 90" clock-wise
        GFX_SetHoriToVertLineOpt(0);

        for(i = 0; i < prBlit->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prBlit->u4Width);

            i4Sy++;
            i4Dx--;
        } // ~for
    }
    else if(prBlit->u4RotateOp == MT53FB_VDP_ROTATE_180)
    {
        // To use mirror&flip to do rotate 180
      UINT32 u4Switch = 0;
	  u4Switch = u4Switch|D_GFXFLAG_MIRROR|D_GFXFLAG_FLIP;
      #ifdef CC_GFX_FLIP_MIRROR	
      GFX_SetBltOpt(u4Switch, 0, 0);
      GFX_BitBlt(prBlit->u4SrcX, prBlit->u4SrcY, prBlit->u4DstX, prBlit->u4DstY, prBlit->u4Width, prBlit->u4Height);
	  #endif	  
    }

    GFX_Flush();
    GFX_Wait();    
    GFX_Unlock();  
    
    if (prBlit->u4RotateOp == MT53FB_VDP_ROTATE_270)
    {

        struct mt53fb_vdp_rotate rResize;

 //resize to width x height/2, and then rotate to height/2 x width
             //---CbCr rotate
        rResize.u4Src = prBlit->u4Src+prBlit->u4SrcCbCrOffset;
        rResize.u4Dst = _u4VdpTmpBufAddr;
        rResize.u4Width = prBlit->u4Width >> 1;
        rResize.u4Height = prBlit->u4Height;
        rResize.u4DstX = 0;
        rResize.u4DstY = 0;        
        rResize.u4SrcX = prBlit->u4SrcX >> 1;
        rResize.u4SrcY = prBlit->u4SrcY;
        rResize.u4SrcPitch = prBlit->u4SrcPitch;
        rResize.u4DstPitch = prBlit->u4Width << 1;
        //_rotate_resize_phase(&rResize, E_RZ_OSD_DIR_CM_ARGB4444, prBlit->u4Width, prBlit->u4Height >> 1); 
        _VdpPhotoResizePhase(prBlit->u4Src, prBlit->u4Src+prBlit->u4SrcCbCrOffset, prBlit->u4SrcX, prBlit->u4SrcY, 
        					prBlit->u4SrcPitch, prBlit->u4Width, prBlit->u4Height,
	    					_u4VdpTmpBufAddr + prBlit->u4SrcCbCrOffset, _u4VdpTmpBufAddr, 0, 0, 
	    					rResize.u4DstPitch, rResize.u4DstPitch,prBlit->u4Height>>1);

        GFX_Lock();   
        
        //prBlit->u4Height = prBlit->u4Height >> 1;
        
        GFX_SetSrc((UINT8 *)_u4VdpTmpBufAddr, CM_RGB565_DIRECT16, rResize.u4DstPitch);
        GFX_SetDst((UINT8 *)(prBlit->u4Dst+prBlit->u4DstCbCrOffset), CM_RGB565_DIRECT16, prBlit->u4DstPitch);    
    
        // Rotating CbCr
        prBlit->u4Height >>= 1;         
        i4Sx = (INT32)0;
        i4Sy = (INT32)0;
        i4Dx = (INT32)prBlit->u4DstX>>1;
        //i4Dy = (INT32)prBlit->u4DstY + prBlit->u4Width; 
		i4Dy = (INT32)prBlit->u4DstY + prBlit->u4Width - 1; 

        // rotated by 90" counter-clock-wise
        GFX_SetHoriToVertLineOpt(1);

        for(i = 0; i < prBlit->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prBlit->u4Width);

            i4Sy++;
            i4Dx++;
        } // ~for

        GFX_Flush();
        GFX_Wait();    
        GFX_Unlock();   
        
    }
    else if(prBlit->u4RotateOp == MT53FB_VDP_ROTATE_90)
    {
    
        struct mt53fb_vdp_rotate rResize;

             //---CbCr rotate
        rResize.u4Src = prBlit->u4Src+prBlit->u4SrcCbCrOffset;
        rResize.u4Dst = _u4VdpTmpBufAddr;
        rResize.u4Width = prBlit->u4Width >> 1;
        rResize.u4Height = prBlit->u4Height;
        rResize.u4DstX = 0;
        rResize.u4DstY = 0;        
        rResize.u4SrcX = prBlit->u4SrcX >> 1;
        rResize.u4SrcY = prBlit->u4SrcY;
        rResize.u4SrcPitch = prBlit->u4SrcPitch;
        rResize.u4DstPitch = prBlit->u4Width << 1;
        //_rotate_resize_phase(&rResize, E_RZ_OSD_DIR_CM_ARGB4444, prBlit->u4Width, prBlit->u4Height >> 1);
        _VdpPhotoResizePhase(prBlit->u4Src, prBlit->u4Src+prBlit->u4SrcCbCrOffset, prBlit->u4SrcX, prBlit->u4SrcY, 
        					prBlit->u4SrcPitch, prBlit->u4Width, prBlit->u4Height,
	    					_u4VdpTmpBufAddr + prBlit->u4SrcCbCrOffset, _u4VdpTmpBufAddr, 0, 0, 
	    					rResize.u4DstPitch, rResize.u4DstPitch,prBlit->u4Height>>1);
    
        GFX_Lock();   

        //prBlit->u4Height = prBlit->u4Height >> 1;
        
        GFX_SetSrc((UINT8 *)_u4VdpTmpBufAddr, CM_RGB565_DIRECT16, rResize.u4DstPitch);
        GFX_SetDst((UINT8 *)(prBlit->u4Dst+prBlit->u4DstCbCrOffset), CM_RGB565_DIRECT16, prBlit->u4DstPitch);   
    
        // Rotating CbCr
        prBlit->u4Height >>= 1;         
        i4Sx = (INT32)0;
        i4Sy = (INT32)0;
        i4Dx = ((INT32)prBlit->u4DstX>>1) + prBlit->u4Height;
        i4Dy = (INT32)prBlit->u4DstY; 
        
        // rotated by 90" clock-wise        
        GFX_SetHoriToVertLineOpt(0);

        for(i = 0; i < prBlit->u4Height; i++)
        {
            GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, 
                (UINT32)i4Dx, (UINT32)i4Dy, prBlit->u4Width);

            i4Sy++;
            i4Dx--;
        } // ~for
        
        GFX_Flush();
        GFX_Wait();    
        GFX_Unlock();   
        
    }
    else if(prBlit->u4RotateOp == MT53FB_VDP_ROTATE_180)
    {
        // To use mirror&flip to do rotate 180 
     UINT32 u4Switch = 0;
     //, u4TemAddr = 0;
	      		
	 u4Switch = u4Switch |D_GFXFLAG_MIRROR|D_GFXFLAG_FLIP;
     GFX_Lock();
	 GFX_SetSrc((UINT8 *)(prBlit->u4Src+ prBlit->u4SrcCbCrOffset), CM_ARGB4444_DIRECT16, prBlit->u4SrcPitch);
	 GFX_SetDst((UINT8 *)prBlit->u4Dst+ prBlit->u4DstCbCrOffset, CM_ARGB4444_DIRECT16, prBlit->u4DstPitch);
	 #if 0
	 // CbCr dram address need old address
	  u4TemAddr = (UINT32)(prBlit->u4Dst+ prBlit->u4DstCbCrOffset + 
                            ((prBlit->u4DstY + prBlit->u4Height - 1) * prBlit->u4DstPitch) + 
                            ((prBlit->u4DstX>>1) + prBlit->u4Width - 1) );
	 if(u4TemAddr%2 !=0)
	 {
	   GFX_SetDst((UINT8 *)(prBlit->u4Dst+ prBlit->u4DstCbCrOffset - 1), CM_ARGB4444_DIRECT16, prBlit->u4DstPitch); 
	 }
	 else
	 {
	   GFX_SetDst((UINT8 *)(prBlit->u4Dst+ prBlit->u4DstCbCrOffset), CM_ARGB4444_DIRECT16, prBlit->u4DstPitch);
	 }
	 #endif
#ifdef CC_GFX_FLIP_MIRROR 
	 GFX_SetBltOpt(u4Switch, 0, 0);
	 GFX_BitBlt(prBlit->u4SrcX>>1, prBlit->u4SrcY, prBlit->u4DstX>>1, prBlit->u4DstY, prBlit->u4Width>>1, prBlit->u4Height);
#endif	  
     GFX_Flush();
     GFX_Wait();    
     GFX_Unlock();       

	
    }         

    return i4Ret;

}


INT32 _VdpPhotoStretchBlit(mt53fb_vdp_stretch_blit * pr_vdp_stretch_blit)
{
    INT32 i4Ret = 0;
    RZ_VDO_SCL_PARAM_SET_T rSclParam;
    E_RZ_INOUT_TYPE_T eRzType;
    

    x_memset(&rSclParam,0,sizeof(RZ_VDO_SCL_PARAM_SET_T));
    

    rSclParam.u4SrcSwap     = 0;
    rSclParam.u4IsRsIn      = 1;
    //1st source vdp Y buffer address
    rSclParam.u4YSrcBase    = pr_vdp_stretch_blit->src1_buffer_C_addr; 
    //1st source vdp Y buffer pitch
    rSclParam.u4YSrcBufLen  = pr_vdp_stretch_blit->src1_buffer_Y_pitch;    
    rSclParam.u4YSrcHOffset = 0;
    rSclParam.u4YSrcVOffset = 0;
    //1st source vdp Y buffer width
    rSclParam.u4YSrcW       = pr_vdp_stretch_blit->src1_buffer_Y_width; 
    //1st source vdp Y buffer height
    rSclParam.u4YSrcH       = pr_vdp_stretch_blit->src1_buffer_Y_height;       
    //1st source vdp C buffer address
    rSclParam.u4CSrcBase    = pr_vdp_stretch_blit->src1_buffer_C_addr;       
    rSclParam.u4CSrcHOffset = 0;
    rSclParam.u4CSrcVOffset = 0;
    //1st source vdp C buffer width (422 may be equals to Y)
    rSclParam.u4CSrcW       = pr_vdp_stretch_blit->src1_buffer_Y_width;       
    //1st source vdp C buffer height (422 may be equals to Y)
    rSclParam.u4CSrcH       = pr_vdp_stretch_blit->src1_buffer_Y_height;       

    rSclParam.u4OutSwap     = 0;
    rSclParam.u4IsRsOut     = 1;
    rSclParam.u4OutMode     = 1;    //422 video format output
    rSclParam.u4IsVdo2Osd   = 0;
    //destination Y buffer address
    rSclParam.u4YTgBase     = pr_vdp_stretch_blit->dest_buffer_Y_addr;       
    rSclParam.u4YTgCM       = 0;    //useful when u4IsVdo2Osd==1
    //destination Y buffer pitch
    rSclParam.u4YTgBufLen   = pr_vdp_stretch_blit->dest_buffer_Y_pitch;      
    rSclParam.u4YTgHOffset  = 0;
    rSclParam.u4YTgVOffset  = 0;
    //destination Y buffer width
    rSclParam.u4YTgW        = pr_vdp_stretch_blit->dest_buffer_Y_width;
    //destination Y buffer height
    rSclParam.u4YTgH        = pr_vdp_stretch_blit->dest_buffer_Y_height;     
    //destination C buffer address
    rSclParam.u4CTgBase     = pr_vdp_stretch_blit->dest_buffer_C_addr; 
    //destination C buffer width    (422 may be equals to Y)
    rSclParam.u4CTgW        = pr_vdp_stretch_blit->dest_buffer_Y_width;     
    //destination C buffer height   (422 may be equals to Y)
    rSclParam.u4CTgH        = pr_vdp_stretch_blit->dest_buffer_Y_height;       
    rSclParam.u4VdoCbCrSwap = 0;  
    
    /* blending level, 0~127, pixel percetage of 1nd cource VDP buffer
         (1st buffer pixel)*rSclParam.u4ABlend + 
         (2nd buffer pixel)*(0x80-rSclParam.u4ABlend)  */
    rSclParam.u4ABlend      = pr_vdp_stretch_blit->composition_level;      
    //2nd source vdp Y buffer                                
    rSclParam.u4PreLoadYAddr = pr_vdp_stretch_blit->src2_buffer_Y_addr;  
    //2nd source vdp C buffer
    rSclParam.u4PreLoadCAddr = pr_vdp_stretch_blit->src2_buffer_C_addr;     
    
    eRzType = E_RZ_INOUT_TYPE_VDOMD;

    IMGRZ_Lock();
    IMGRZ_ReInit();
    IMGRZ_SetScaleOpt(eRzType);
    IMGRZ_Scale(&rSclParam);
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();     

    return i4Ret;

}


static int _VdpPhotoGetBuffer(void* pvArg)
{
	mt53fb_imagebuffer rBuf; 
    rBuf.u4Size = _gVdpBufInfo.u4Size;
    rBuf.u4PhyAddr = _gVdpBufInfo.u4PhyAddr;
    rBuf.u4VirtAddr = _gVdpBufInfo.u4VirtAddr;
    x_memcpy(pvArg, (void*)&rBuf, sizeof(mt53fb_imagebuffer));
    
    return 0;
}

static int _VdpPhotoGetCropRect(void* pvArg)
{

   mt53fb_vdp_crop_rect  t_s_rect; 
	
    x_memcpy(&t_s_rect, pvArg, sizeof(mt53fb_vdp_crop_rect));
    
    if(t_s_rect.ui4_layer > PHOTO_VDP_LAYER_MAX )
    {
         return -1;
    }
    
    x_memcpy(pvArg,&mt53fb_vdp_crop_rect_array[t_s_rect.ui4_layer], 
        sizeof(mt53fb_vdp_crop_rect));
    
    return 0;
}

static int _VdpPhotoSetCropRect(void* pvArg)
{
   
   mt53fb_vdp_crop_rect  t_s_rect; 
	  
	  x_memcpy(&t_s_rect, pvArg, sizeof(mt53fb_vdp_crop_rect));
	  
	  if(t_s_rect.ui4_layer > PHOTO_VDP_LAYER_MAX )
	  {
		   return -1;
	  }
	  
	  x_memcpy(&mt53fb_vdp_crop_rect_array[t_s_rect.ui4_layer], &t_s_rect, sizeof(mt53fb_vdp_crop_rect));
   
    // Set 3D crop offset
    _u4CropBottomOffset = t_s_rect.crop_bottom_offset;
    _u4CropTopOffset    = t_s_rect.crop_top_offset;        
    _u4CropLeftOffset   = t_s_rect.crop_left_offset;
    _u4CropRightOffset  = t_s_rect.crop_right_offset;  
	_bIs3DMode			= t_s_rect.ui4_3d_mode;
    printf("Crop Rect[%d,%d,%d,%d]\n",
                _u4CropLeftOffset,_u4CropTopOffset,
                _u4CropRightOffset,_u4CropBottomOffset);
   	
    return 0;
}

static INT32 _VdpPhotoStretchBlitCmd(void* pvArg)
{
   mt53fb_vdp_stretch_blit rVdpStretchBlit;

    x_memcpy((void*)&rVdpStretchBlit, pvArg, sizeof(mt53fb_vdp_stretch_blit));
  
    return _VdpPhotoStretchBlit(&rVdpStretchBlit); 
}



static int _VdpPhotoInit(void *pvArg)
{
	mt53fb_vdp_init rInitData;
	 
	 x_memcpy( (void*)&rInitData, pvArg, sizeof(mt53fb_vdp_init));
	
	 if(!_VdpPhotoFBGInit(PANEL_GetPanelWidth(), PANEL_GetPanelHeight(), 0))
	 {
		 printf("_VdpPhotoInit() - fbg init fail\n");
		 return -1;	
	 }

	 _VdpPhotoInitFbmBuffer();
	_u4VdpTmpBufAddr =  (UINT32)BSP_AllocAlignedDmaMemory(TMPBUF_WIDTH*TMPBUF_HEIGHT*TMPBUF_BPP, 16);
    ASSERT(_u4VdpTmpBufAddr != 0);
	
    return 0;
}

static int _VdpPhotoFlip(void* pvArg)
{
   
	struct mt53fb_vdp_flip rFlipData;

	if (pvArg == NULL)
	{
		printf("_VdpPhotoFlip argument invalid is NULL.\n");
		return 0;
	}
    
    x_memcpy( (void*)&rFlipData, pvArg, sizeof(struct mt53fb_vdp_flip));

    if(!_VdpPhotoFBGInit(PANEL_GetPanelWidth(), PANEL_GetPanelHeight(), rFlipData.ui4_pitch))
    {
        printf("_VdpPhotoFlip() - FBG init fail\n");    
        return -1;    
    }
    if(!_VdpPhotoFBFlip(_ucFbgId, rFlipData.pu1SrcYAddr, rFlipData.pu1SrcCAddr, _bIs3DMode, rFlipData.ui4_pitch,
    	_u4CropBottomOffset, _u4CropTopOffset, _u4CropLeftOffset, _u4CropRightOffset))
    {
        printf( "_VdpPhotoFlip() - FB flip fail\n");    
        return -1;    
    }

    return 0;
}

static int _VdpPhotoSet(void* pvArg)
{
   struct mt53fb_vdp_set rSetData;
    
    x_memcpy( (void*)&rSetData, pvArg, sizeof(struct mt53fb_vdp_set));
 
    //layer release
    if(!rSetData.ucEnable)
    {
        VDP_SetMode(0, VDP_MODE_BG);    
        FBM_ReleaseGroup(_ucFbgId);
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);

        fpDrvTDTVReg3DCropCB(E_TDTV_CB_3D_CROP_MM, NULL);
        _fgVdpInit = FALSE;
        // printk(KERN_WARNING "VDP release fbg!\n");        
    }
  
    return 0;
}

INT32 _VdpPhotoRotate(void* pvArg)
{
 
 struct mt53fb_vdp_rotate rVdpBlit;
 
 
 x_memcpy((void*)&rVdpBlit, pvArg, sizeof(struct mt53fb_vdp_rotate));
 
 return _VdpPhotoYCSepRotate(&rVdpBlit); 

}

static INT32 _VdpPhotoCreateBitmap(GFX_BITMAP_INIT_T* prBmpInit)
{
    OSD_BITMAP_HANDLE_T *prBmp;
    UINT32 u4bpp; // bits per pixel
    UINT32 u4Pitch;
    UINT32 u4Lines;

    RET_ON_NULL(prBmpInit);

    prBmp = x_mem_alloc(sizeof(OSD_BITMAP_HANDLE_T));
    if (prBmp == NULL)
    {
        return -(INT32)OSD_RET_OUT_OF_MEM;
    }

    // create bitmap bits area
    u4bpp = _VdpPhothoGetBitsPerPixel(_VdpPhotoGetMwColorMode(prBmpInit->e_colormode));

    // bytes, aligned on 128 bits
    u4Pitch = (((u4bpp * prBmpInit->ui4_bmp_w) + (15 * 8)) >> 3) & (~0xf);
    // luis@2008/1/30, remove even line protection
    //u4Lines = (prBmpInit->ui4_bmp_h + 1) & (~1);
    u4Lines = prBmpInit->ui4_bmp_h;

    // build init info
    x_memset(prBmp, 0, sizeof(OSD_BITMAP_HANDLE_T));

#ifdef CC_MT5392B    	
    #if FIX_MM_OSD_GARBAGE
				_fgMMOSD = 1;
		#endif
#endif		    	
        prBmp->pvFreePoint =
        (void*)_VdpPhotoAllocateFbmBuffer((UINT32*)&prBmpInit->z_new_bitmap_size);

    if (prBmp->pvFreePoint == NULL)
    {
        printf("_VdpPhotoAllocateFbmBuffer fail\n");
        return -(INT32)OSD_RET_OUT_OF_MEM;
    }

    
    if( (prBmpInit->z_new_bitmap_size) < (u4Pitch * u4Lines) )//fbm size smaller than required
    {
        printf("_VdpPhotoAllocateFbmBuffer fbm vdp size smaller than required\n");
    	return -(INT32)OSD_RET_OUT_OF_MEM;
    }
    x_memset(prBmp->pvFreePoint, 0, u4Pitch * u4Lines);

    HalFlushDCache();

    x_memcpy(&prBmp->rInit, prBmpInit, sizeof(GFX_BITMAP_INIT_T));
    prBmp->rInit.h_new_bitmap = (GFX_HBITMAP_T)(void*)prBmp;

    // build lock info
    prBmp->rLockInfo.e_colormode = prBmpInit->e_colormode;
    prBmp->rLockInfo.z_subplane = 1;
    prBmp->rLockInfo.at_subplane[0].ui4_width = prBmpInit->ui4_bmp_w;
    prBmp->rLockInfo.at_subplane[0].ui4_height = prBmpInit->ui4_bmp_h;
    prBmp->rLockInfo.at_subplane[0].ui4_pitch = (((u4bpp/2 * prBmpInit->ui4_bmp_w) + (15 * 8)) >> 3) & (~0xf);//YUV422_SEP


   	prBmp->rLockInfo.at_subplane[0].pv_bits = prBmp->pvFreePoint;
   
    // skip first 32 bytes, 128 bits aligned
    prBmp->rLockInfo.h_bitmap = (GFX_HBITMAP_T)(void*)prBmp;

    //indexed color mode
    prBmp->rPalette.h_bitmap = (GFX_HBITMAP_T)(void*)prBmp;
    prBmp->rPalette.h_palette = prBmp->rInit.h_palette;
    prBmp->u4BitsPerPixel = u4bpp;

    prBmpInit->h_new_bitmap = prBmp->rInit.h_new_bitmap;
    return (INT32)OSD_RET_OK;
}

static INT32 _VdpPhotoDeleteBitmap(GFX_HBITMAP_T rBmp)
{
    OSD_BITMAP_HANDLE_T *prBmp;

    RET_ON_NULL(rBmp);

    prBmp = (OSD_BITMAP_HANDLE_T*)(void*)rBmp;
    if (prBmp->pvFreePoint)
    {
        _VdpPhotoFreeFbmBuffer(prBmp->pvFreePoint);
        x_mem_free(prBmp);
    }
    return (INT32)OSD_RET_OK;
}


INT32 _VdpPhotoSetCommand(DRV_COMP_ID_T* prCompId, DRV_SET_TYPE_T rSetType,
                            const VOID* pvSetInfo, SIZE_T zSetInfoLen)
{
  switch(rSetType)
	{
	case OSD_SET_VDPPHOTO_INIT:
		SET_COMMAND(_VdpPhotoInit((void*)pvSetInfo));
	case OSD_SET_VDPPHOTO_FLIP:
		SET_COMMAND(_VdpPhotoFlip((void*)pvSetInfo));	
	case OSD_SET_VDPPHOTO_SETTING:		
		SET_COMMAND(_VdpPhotoSet((void*)pvSetInfo));						
	case OSD_SET_VDPPHOTO_ROTATE: 	
		SET_COMMAND(_VdpPhotoRotate((void*)pvSetInfo));	
	case OSD_SET_VDPPHOTO_GETBUFFER:
		SET_COMMAND(_VdpPhotoGetBuffer((void*)pvSetInfo)); 
case OSD_SET_VDPPHOTO_SET_CROP_RECT:
		SET_COMMAND(_VdpPhotoSetCropRect((void*)pvSetInfo));
	case OSD_SET_VDPPHOTO_GET_CROP_RECT:
		SET_COMMAND(_VdpPhotoGetCropRect((void *)pvSetInfo));
	case OSD_SET_VDPPHOTO_STRETCH_BLIT:
		SET_COMMAND(_VdpPhotoStretchBlitCmd((void*)pvSetInfo));
	case OSD_GET_VDPPHOTO_CREATE_BITMAP:
		SET_COMMAND(_VdpPhotoCreateBitmap((GFX_BITMAP_INIT_T*)pvSetInfo));
	case OSD_SET_VDPPHOTO_DELETE_BITMAP:
		SET_COMMAND(_VdpPhotoDeleteBitmap((GFX_HBITMAP_T)pvSetInfo));
    default:
        return RMR_DRV_INV_SET_INFO;
  	
   }
}

#endif


