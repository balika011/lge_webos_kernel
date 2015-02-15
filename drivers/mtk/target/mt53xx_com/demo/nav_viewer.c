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
 * $RCSfile: nav_viewer.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_viewer.c
 *
 */

#define NAV_VIEWER_DEBUG 0
#define IMG_RESIZER_WORKAROUND 0

#include "x_os.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_blk_dev.h"

#if 0
#if defined(CC_MT5387) && !defined(CC_MT5363)
#include "../5387_driver/fm/x_fm.h"
#endif
#if defined(CC_MT5363) && !defined(CC_MT5365)
#include "../5363_driver/fm/x_fm.h"
#endif
#ifdef CC_MT5365
#include "../5365_driver/fm/x_fm.h"
#endif
#else
#include "x_fm.h"
#endif

#include "x_drv_if.h"

#include "panel.h"
#include "img_lib_if.h"
#include "jpg_if.h"
#include "gfx_if.h"

#include "widget_if.h"
#include "nav.h"
#include "nav_av.h"

#if 1//defined(CC_MT5363) || defined(CC_MT5365)|| defined(CC_MT5395) || defined(CC_MT5368)|| defined(CC_MT5396)|| defined(CC_MT5389)
#define IMG_OSD_PLANE DRV_OSD_PLANE_3
#else
#define IMG_OSD_PLANE DRV_OSD_PLANE_2
#endif

#define BYTES_PER_READ 512
#define MAX_FILE_SIZE 8 * 1024 * 1024

static UINT32 _u4fileSize = 0;
static UINT32 _u4filled_size = 0;
static UINT8* _pu1Bs = 0;

static UINT8* _pu1ImgBuf = NULL;
static UINT32 _u4ImgSize = 0;

static HANDLE_T _hImgFile = NULL;

static HANDLE_T _hJPGSema = NULL;

static DRV_OSD_RGN_T _rDisplayOSD;
static UINT32 _u4DisplayRegionId;
static UINT32 _u4NtyValue;

static BOOL _fgIsEOF = FALSE, _fgFillBufEnd = FALSE;
static UINT32 _u4FillLen[] = {0, 0};

static BOOL bKeepRatio = FALSE;

static CHAR _szImgFile[256] ={0};// NULL;

EXTERN WIDGET_BITMAP_T* prInfoOSDBg;

#define IMG_ID_BY_TYPE(imageType)   (UCHAR)((imageType==IMAGE_PNG)?1:0)
static VOID _IMGDecode(ENUM_IMG_FORMAT eImageType);

static CHAR* _ImageExtension(const CHAR* szFileName)
{
    CHAR* szExt;
    szExt = x_strrchr(szFileName, '.') + 1;
    return szExt;
}

INT32 _JPGInit(VOID)
{
    static BOOL fgInit = FALSE;
    if (fgInit)
    {
        return 0;
    }

    fgInit = TRUE;

    _rDisplayOSD.u4Width = ((TV_WIDTH - 1) >> 4 << 4) + 16;
    _rDisplayOSD.u4Height = TV_HEIGHT;
    _rDisplayOSD.u4ScaleWidth = ((TV_WIDTH - 1) >> 4 << 4) + 16;
    _rDisplayOSD.u4ScaleHeight = TV_HEIGHT;
    _rDisplayOSD.u1PlaID = IMG_OSD_PLANE;
    _rDisplayOSD.rColorMode = DRV_CM_AYBR_8888;
    _rDisplayOSD.u4Left = 0;
    _rDisplayOSD.u4Top = 0;
    _rDisplayOSD.u4Pitch = _rDisplayOSD.u4Width << 2;
#if NAV_VIEWER_DEBUG
    _rDisplayOSD.pu1FrameBaseAddr = 0x3000000;
#else
    _rDisplayOSD.pu1FrameBaseAddr = (UINT8*)DRV_SYS_AllocAlignedDmaMemory( (_rDisplayOSD.u4Width << 2) * TV_HEIGHT, 16);
#endif

    if (_hJPGSema == NULL)
    {
        VERIFY (x_sema_create(&_hJPGSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }

    // Initialize IMG library
    IMG_Lib_Init();

    return 0;
}

INT32 _JPGStart(const CHAR* szFileName)
{

    UINT32 u4Read = 0;
    FM_FILE_INFO_T  rFileInfo;
    ENUM_IMG_FORMAT eImageType;
    UINT8 *pu1Buf;
    CHAR *puExten;

    _u4ImgSize = 0;
    _u4fileSize = 0,
                  _u4filled_size = 0;

    // Open JPEG file
    if (FMR_OK != x_fm_open(FM_ROOT_HANDLE, szFileName, FM_READ_ONLY, 0777, FALSE, &_hImgFile))
    {
        printf("Open file %s fail!\n", szFileName);
        return -1024;
    }

    // Get file info
    if (FMR_OK != x_fm_get_info_by_handle(_hImgFile, &rFileInfo))
    {
        printf("Get file %s info fail!\n", szFileName);
        return -1024;
    }

    // Get file size
    if (rFileInfo.ui8_size > MAX_FILE_SIZE - 512)
    {
        printf("File %s too large!\n", szFileName);
        return -1024;
    }

    if ( (_pu1ImgBuf = x_mem_alloc(MAX_FILE_SIZE)) == NULL)
    {
        printf("Not enough buffer!\n", szFileName);
        return -1024;
    }

    // Read file into buffer, max 4 MB
    pu1Buf = _pu1ImgBuf;
    puExten=_ImageExtension(szFileName);
    if ((x_strcmp(puExten, "jpg") == 0) || (x_strcmp(puExten, "JPG") == 0))
    {
        eImageType=IMAGE_JPEG;
    }
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860) || defined(CC_MT5881))
    else if ((x_strcmp(puExten, "webp") == 0) || (x_strcmp(puExten, "WEBP") == 0))
    {
        eImageType=IMAGE_WEBP;
    }
    else if ((x_strcmp(puExten, "png") == 0) || (x_strcmp(puExten, "PNG") == 0))
    {
        eImageType=IMAGE_PNG;
    }
#endif
    else
    {
        Printf("Unknow image type \n");
        return -1024;
    }

    IMG_Config(IMG_ID_BY_TYPE(eImageType), eImageType);

    while (1)
    {

        if (FMR_OK != x_fm_read(_hImgFile, pu1Buf, BYTES_PER_READ, &u4Read))
        {
            printf("Read file %s fail!\n", szFileName);
            x_fm_close(_hImgFile);
            return -1024;
        }

        //LOG(5, "Read %d\n", _u4ImgSize);

        _u4ImgSize += u4Read;
        pu1Buf += u4Read;

        if (u4Read < BYTES_PER_READ)
        {
            break;
        }

    }
    // Create OSD later for displaying JPEG image
    DRV_OSD_RGN_Create(&_rDisplayOSD, &_u4DisplayRegionId);
    DRV_OSD_RGN_SetAlphaMode(_u4DisplayRegionId, DRV_OSD_BM_PIXEL);

    // Decode the JPEG stream
    _IMGDecode(eImageType);

    DRV_OSD_Enable(IMG_OSD_PLANE, TRUE);

    return 0;

}

INT32 _Final(void)
{
    DRV_OSD_Enable(IMG_OSD_PLANE, FALSE);

    // Create OSD later for displaying JPEG image
    DRV_OSD_RGN_Delete(_rDisplayOSD.u1PlaID, _u4DisplayRegionId);

    if (_pu1ImgBuf)
    {
        x_mem_free(_pu1ImgBuf);
        _pu1ImgBuf = NULL;
    }

    x_fm_close(_hImgFile);
    return 0;
}


void _FillBufCB(
    UINT32 u4ImgID,
    void *pv_tag,
    void *pv_data,
    UINT32 u4State)
{
    IMG_FILL_BUF_MW_T *prBufInfo = (IMG_FILL_BUF_MW_T *)pv_data;
    UINT32 u4curFilledSize;

    if (u4State == JPG_HW_NFY_PROGRESS)
    {
        return;	//ignore the progress notify
    }
    // Finish decode

    _u4NtyValue = u4State;

    if ((u4State == JPG_HW_NFY_FINISHED) ||(u4State == JPG_HW_NFY_ERROR))
    {
        VERIFY (x_sema_unlock(_hJPGSema) == OSR_OK);
        return;
    }

    if (prBufInfo==NULL)
    {
        return;
    }
    if (prBufInfo->b_reset_pos)
    {
        _u4filled_size = prBufInfo->ui4_position;
        _pu1Bs = _pu1ImgBuf + prBufInfo->ui4_position;
    }

    if ((_u4filled_size + prBufInfo->ui4_required_len) >=_u4fileSize)
    {
        u4curFilledSize = _u4fileSize - _u4filled_size;
        _fgIsEOF = TRUE;
    }
    else
    {
        u4curFilledSize = prBufInfo->ui4_required_len;
		_fgIsEOF=FALSE;
    }
	
    x_memcpy(prBufInfo->pv_start_add, (void *)_pu1Bs, u4curFilledSize);
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860) || defined(CC_MT5881))
    HalFlushInvalidateDCacheMultipleLine((UINT32)(prBufInfo->pv_start_add), (u4curFilledSize));
#endif
    _u4filled_size += u4curFilledSize;
    _pu1Bs += u4curFilledSize;
    _u4FillLen[prBufInfo->ui4_trsn_id] = u4curFilledSize;
	
    if (prBufInfo->ui4_trsn_id == 1)
    {
        _fgFillBufEnd = TRUE;
    }

    VERIFY (x_sema_unlock(_hJPGSema) == OSR_OK);

    // clean lint warning
    UNUSED(u4ImgID);
    if (pv_tag == NULL)
    {
        return;
    }
    else
    {
        *((UINT8 *)pv_tag) = 0;
    }
}

static VOID _IMGDecode(ENUM_IMG_FORMAT eImageType)
{
    IMG_PARAM_T rInfo;
    IMG_REGION_T rDecOut;
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860) || defined(CC_MT5881))
    UINT32 u4PicWidth;
    UINT32 u4PicHeight;
#endif
    // Buffer notification mode
    rInfo.u8BufSize = NULL;
    rInfo.pu1InBuffer = NULL;

    // Necessary to set these??
    rInfo.eFormat = eImageType;

    // Set dest OSD
    GFX_Lock();
    GFX_SetDst(_rDisplayOSD.pu1FrameBaseAddr, (UINT32)CM_AYCbCr8888_DIRECT32, _rDisplayOSD.u4Width << 2);
    GFX_SetColor(0xff008080);
    GFX_Fill(0, 0, _rDisplayOSD.u4Width, _rDisplayOSD.u4Height);
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();

    rInfo.rDstRegion.pu1Addr = _rDisplayOSD.pu1FrameBaseAddr;
    rInfo.rDstRegion.u4Width = _rDisplayOSD.u4Width;
    rInfo.rDstRegion.u4Height = _rDisplayOSD.u4Height;
    rInfo.rDstRegion.u4TotalLine = _rDisplayOSD.u4Height;
    rInfo.rDstRegion.u4StartX = 0;
    rInfo.rDstRegion.u4StartY = 0;
    rInfo.rDstRegion.u4Pitch = _rDisplayOSD.u4Pitch;
    rInfo.rDstRegion.u4Depth = 8;
    rInfo.rDstRegion.u4CbCrOffset = 0;

    // Default to use AYBR8888
    rInfo.eDispColorMode = AYCBCR8888_DIRECT32;

    // No rotate
    rInfo.eRotateDegree = CLOCKWISE_0;

    rInfo.prDecOut = &rDecOut;
    rInfo.bKeepRatio = 1;

    if (_pu1ImgBuf == NULL)
    {
        return;
    }

    // sync ram and cache
    HalFlushInvalidateDCache();

    _pu1Bs = _pu1ImgBuf;

    _u4fileSize = _u4ImgSize;

    _u4filled_size = 0;
#if (defined(CC_MT5368) || defined(CC_MT5396) || defined(CC_MT5389) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860) || defined(CC_MT5881))
    switch (eImageType)
    {
    case IMAGE_PNG:
        u4PicWidth = 0;
        u4PicHeight = 0;
        u4PicWidth += ((UINT32)(_pu1ImgBuf[16])) << 24;
        u4PicWidth += ((UINT32)(_pu1ImgBuf[17])) << 16;
        u4PicWidth += ((UINT32)(_pu1ImgBuf[18])) << 8;
        u4PicWidth += ((UINT32)(_pu1ImgBuf[19]));
        u4PicHeight += ((UINT32)(_pu1ImgBuf[20])) << 24;
        u4PicHeight += ((UINT32)(_pu1ImgBuf[21])) << 16;
        u4PicHeight += ((UINT32)(_pu1ImgBuf[22])) << 8;
        u4PicHeight += ((UINT32)(_pu1ImgBuf[23]));
        rInfo.rSrcRegion.u4SrcStartX = 0;
        rInfo.rSrcRegion.u4SrcStartY = 0;
        rInfo.rSrcRegion.u4SrcWidth = u4PicWidth;
        rInfo.rSrcRegion.u4SrcHeight = u4PicHeight;
        rInfo.u4DecColorMode = 12;//GFX_COLORMODE_ARGB_D8888;
       // rInfo.eDispColorMode = ARGB8888_DIRECT32;
        Printf("PNG width = %d, height = d\n", u4PicWidth,u4PicHeight);
        break;
    default:
        break;
    }
#endif
#if IMG_RESIZER_WORKAROUND
    {
        UINT32 u4Val;
        u4Val = IO_READ32(IO_VIRT + 0x2d000, 0x5c4);
        u4Val &= 0xff0000ff;
        IO_WRITE32(IO_VIRT + 0x2d000, 0x5c4, u4Val);
    }
#endif

    IMG_Init(RING_IN_SINGLE_OUT, NULL, IMG_ID_BY_TYPE(eImageType), _FillBufCB);
	IMG_SetUseMw(IMG_ID_BY_TYPE(eImageType), TRUE);
    IMG_Set_ImgSize(IMG_ID_BY_TYPE(eImageType), _u4ImgSize);
    IMG_Decode(IMG_ID_BY_TYPE(eImageType), rInfo);

    do
    {
        VERIFY (x_sema_lock(_hJPGSema, X_SEMA_OPTION_WAIT) == OSR_OK);
        if (_fgFillBufEnd)
        {
		   UINT32 u4DataLen0=_u4FillLen[0];
		   UINT32 u4DataLen1=_u4FillLen[1];
		    _u4FillLen[0] = 0;
            _u4FillLen[1] = 0;
			_fgFillBufEnd = FALSE;
            if (u4DataLen0==0)
            {
                IMG_SetFillBsBufEnd(IMG_ID_BY_TYPE(eImageType), u4DataLen1, 0, _fgIsEOF);
            }
            else
            {
                IMG_SetFillBsBufEnd(IMG_ID_BY_TYPE(eImageType), u4DataLen0, u4DataLen1, _fgIsEOF);
            }

        }
    }
    while (_u4NtyValue == JPG_HW_NFY_FILL_BUF);

    bKeepRatio = ~bKeepRatio;

    IMG_Release(IMG_ID_BY_TYPE(eImageType));

#if IMG_RESIZER_WORKAROUND
    {
        UINT32 u4Val;
        u4Val = IO_READ32(IO_VIRT + 0x2d000, 0x5c4);
        u4Val |= 0x00ffff00;
        IO_WRITE32(IO_VIRT + 0x2d000, 0x5c4, u4Val);
    }
#endif

}


INT32 NAV_MM_ViewerUI(WIDGET_OSDINFO_T* prOSD)
{
    VERIFY(WIDGET_DrawBitmap(prInfoOSDBg, 0, 0, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

    VERIFY(WIDGET_DrawString("Image Viewer", 20, 10, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);

    //VERIFY(WIDGET_DrawString("Multimedia", 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

    VERIFY(WIDGET_DrawString(_szImgFile,  20, 50, NAV_GetDefaultFont(), prOSD) == (INT32)E_WIDGET_MSG_OK);

    WIDGET_ClearScreen(&arWidgetOSD[0]);
    VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(prOSD) == (INT32)E_WIDGET_MSG_OK);
    return (INT32)E_NAV_MSG_OK;
}


INT32 NAV_VIEWER_Init(VOID)
{
    return _JPGInit();
}

INT32 NAV_VIEWER_Start(const CHAR* szFileName)
{  
	UINT32 cnt=0;
	for(cnt =0; cnt<256; cnt++)
    {
      _szImgFile[cnt]=0;
    }
    x_strcpy(_szImgFile, szFileName);
    return _JPGStart(szFileName);
}

INT32 NAV_VIEWER_Stop(VOID)
{
    //_szImgFile = NULL;
    return _Final();
}
