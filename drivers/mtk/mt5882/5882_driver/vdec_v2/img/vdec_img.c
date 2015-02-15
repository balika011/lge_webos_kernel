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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_img.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_img.c
 *  This file contains implementation of exported APIs of sending image data to jpeg driver.
 */
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "vdec_drvif.h"
#include "srm_drvif.h"
#include "img_lib_if.h"
#include "imgrz_if.h"
#include "fbm_drvif.h"
#include "vdec_img.h"
#include "x_assert.h"
//#include "x_debug.h"
#include "../vdec_debug.h"
#include "x_img_dec.h"
#include "gfx_if.h"
#include "jpg_if.h"
/********************************************************************
  MACRO CONSTANT DEFINITIONS
********************************************************************/
#ifdef VDEC_VENC_PATH
#define MJPEG_SUPPORT_VENC
#endif

/********************************************************************
  MACRO FUNCTION DEFINITIONS
********************************************************************/

/********************************************************************
  TYPE DEFINITION
********************************************************************/
#define MJPEG_DHT_DEFSIZE 1024
#define MJPEG_MAX_ES_NUM    VDEC_MAX_ES
#if defined(VDEC_IS_POST_MT5368)
#define MJPEG_MAX_WIDTH     1920
#define MJPEG_MAX_HEIGHT    1080
#elif defined(CC_MT5365) || defined(CC_MT5395) 
#define MJPEG_MAX_WIDTH     1280
#define MJPEG_MAX_HEIGHT    720
#else
#define MJPEG_MAX_WIDTH     640
#define MJPEG_MAX_HEIGHT    480  
#endif
#define MJPEG_TRACK_SKIP  4

#if defined(VDEC_IS_POST_MT5368)
#define MJPEG_USE_NEW_FLOW
#define MJPG_PROCE_FIELD_FRAME
#endif

#define _Vdec_GETRINGBUF_VAL(integer,src,pos,startaddr,endaddr) \
{  \
   if(src+pos<(UCHAR *)endaddr) \
   { \
      integer=((UCHAR *)src)[pos]; \
   } \
   else \
   { \
      integer=((UCHAR *)startaddr)[pos-((UCHAR *)endaddr-src)]; \
   } \
}

/********************************************************************
  EXTERN VARIABLES & FUNCTION PROTOTYPES DECLARATIONS
********************************************************************/
#ifdef VDEC_TIME_PROFILE
typedef struct
{    
    UINT8 u1DramMoniter;     //0: none, 1: Total, 2: Mpeg

    UINT32 u4CurDecMs;
    UINT32 u4CurDecCycle;
    UINT32 u4CurDramCycle;
}MJPEGOther;
#endif


typedef struct
{
  VDEC_PES_INFO_T rPesInfo;
#ifdef VDEC_TIME_PROFILE
    MJPEGOther rOther;
#endif
  MJPG_OUTPARAM_T rOutParam;
  MJPG_INPARAM_T rInParam;
  HANDLE_T hJDec;
  UINT32 u4MJPEGNtyValue;
  UINT32 u4ReadPtr;
  UINT32 u4WritePtr;
  #ifdef CC_FUJI3D_SUPPORT
  UINT32 u4MPFOffset;
  BOOL fgMPFParsing;
  #endif
  BOOL fgIsMPF;    
  UCHAR ucMpvId;
  UCHAR ucFbgId;
  UCHAR ucFbId;
  BOOL fgOpened;
  BOOL fgSeqChanged;
}VDEC_IMG_INSTANCE_T;

static VDEC_IMG_INSTANCE_T _rVdecImgInfo[MJPEG_MAX_ES_NUM];
static HANDLE_T _hMJPGDecSema = 0;
static BOOL _fgVdecImgLibInited=FALSE;
static IMG_REGION_T _rDecOut;
static UINT8 _uMJPEG_SKIP_FRAME = MJPEG_TRACK_SKIP;
static UINT32 _uMJPEG_SKIP_FRAME_CNT = 0;

#ifdef VDEC_TIME_PROFILE
static HAL_TIME_T _rTimeHwS, _rTimeHwDt, _rTimeHwE;
static UINT32 u4DelayClock;
static UINT32 u4DramCycle;
static UINT32 u4DecCycle = 0;
#endif

#ifdef CC_SUPPORT_MTK_ANDROID_RM
static UINT8 u1MJPGColorMode = 0xff;    
#endif
extern VOID _VPUSH_ImgFrmDone(UCHAR ucEsId, VOID *pNfyInfo);

// *********************************************************************
// Function    : void _Vdec_ImageDecCb
// Description : call back handler of decode event
// Parameter   : 
// Return      : None
// *********************************************************************
static VOID _Vdec_ImageDecCb(UINT32 u4ImgID, void *pv_tag, void *pv_data, UINT32 u4State)
{    
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
	prVdecImgInfo=&_rVdecImgInfo[u4ImgID];
	prVdecImgInfo->u4MJPEGNtyValue=u4State;
#ifdef CC_FUJI3D_SUPPORT
	prVdecImgInfo->u4MPFOffset = 0;
#endif
    if((u4State == JPG_HW_NFY_FINISHED) || (u4State == JPG_HW_NFY_ERROR) || 
       (u4State == JPG_HW_NFY_STOP_DONE))
	{		
	       #ifdef CC_FUJI3D_SUPPORT
	       if(prVdecImgInfo->fgMPFParsing && (u4State == JPG_HW_NFY_FINISHED))
	       {
	           prVdecImgInfo->u4MPFOffset = (UINT32)pv_data;
	       }
	       #endif
		VERIFY (x_sema_unlock(_hMJPGDecSema) == OSR_OK);
	}    
	else if(u4State!=JPG_HW_NFY_ALREADY_STOPPED)
	{
	  LOG(0, "_VDEC_MJPEGCb MJPEG ES(0) Error=%d\n", u4State); 			   
	}

}

// *********************************************************************
// Function    : UINT32 _Vdec_ImageDecReadPointer
// Description : get read pointer of ring buf
// Parameter   : 
// Return      : read ptr
// *********************************************************************
static UINT32 _Vdec_ImageDecReadPointer(VOID)
{
	UINT32 u4DramRptr,u4HalVlaue=0;
	UINT32 u4SramRptr, u4SramWptr,u4SramRcnt;
	UINT32 u4SramDataSz;
	UINT32 u4ByteAddr=0;
	UINT32 u4Cnt = 0;
	UINT32 u4VStart,u4VEnd;
	
    #if defined(VDEC_IS_POST_MT5368)
    //if (((VLD_READ32(RO_VLD_SRAMCTRL) >> 15) & AA_FIT_TARGET_D) == 1)
	if((VLD_READ32(RO_VLD_FETCH_FINISH) & JPG_VLD_INIT_VALID_FLG))    	
    #endif
    {	
	while((VLD_READ32(RO_VLD_SRAMCTRL) & AA_FIT_TARGET_D) == 0)
	{
        u4Cnt++;
        if(u4Cnt >= (UINT32)0x100000)
        {
            LOG(1, "Rptr-WaitSramStable Fail\n");
            break;
        }
	}
    }
	u4DramRptr = VLD_READ32(RO_VLD_VRPTR);
	u4HalVlaue=VLD_READ32(RO_VLD_VBAR);
	u4SramRptr = u4HalVlaue&0xf;
	u4SramWptr = (u4HalVlaue>>8)&0xf;
	u4SramRcnt = (u4HalVlaue>>24)&0x3;
		
	if (u4SramWptr > u4SramRptr)
	{
		u4SramDataSz = u4SramWptr - u4SramRptr;
	}
	else
	{
		u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
	}
		  
	u4ByteAddr=u4DramRptr+u4SramRcnt*4 -(u4SramDataSz*16)-20;
	u4VStart = VLD_READ32(RW_VLD_VSTART) << 6;
	u4VEnd = VLD_READ32(RW_VLD_VEND) << 6;
	if (u4ByteAddr < u4VStart)
	{
		u4ByteAddr = u4ByteAddr + (u4VEnd - u4VStart);
	}
		
	if(u4ByteAddr > u4VEnd)
	{
		u4ByteAddr = u4VStart + (u4ByteAddr - u4VEnd);
	}
	return (u4ByteAddr);
}


// *********************************************************************
// Function    : _Vdec_StillFrameDecode
// Description : decoding procedure of vdec
// Parameter   : 
// Return      : 
// *********************************************************************
static BOOL _Vdec_ImageDecCreateFBG(UCHAR ucEsId,VDEC_IMG_INSTANCE_T *prVdecImgInfo)
{
	FBM_CREATE_FBG_PAR_T rPar;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;
    UCHAR ucFbgId,ucMJPEGTargetType;
	x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
	rPar.u1VdecId = (UINT8)ucEsId;
	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	if(prVdecEsInfo->eMMSrcType == SWDMX_SRC_TYPE_NETWORK_SKYPE)
	{
		rPar.u1AppMode = FBM_FBG_APP_SKYPE;
	}
	else if((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))
	{
		rPar.u1AppMode = FBM_FBG_APP_OMX;
	}
	else if(prVdecEsInfoKeep->fgVPush && (prVdecEsInfoKeep->eVPushPlayMode != VDEC_PUSH_MODE_TUNNEL))
	{
		rPar.u1AppMode = FBM_FBG_APP_OMX_DISP;
	}
	else if(prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM)
	{
		rPar.u1AppMode = FBM_FBG_APP_Frame_Pack_3D;
	}
	else
	{
		rPar.u1AppMode = FBM_FBG_APP_NORMAL;
	}
	

        if (prVdecEsInfoKeep->fgVPushFBGFromInst)
        {
            LOG(3, "fgCreateFromInst == TRUE\n");
            rPar.fgCreateFromInst = TRUE;
        }

	ucMJPEGTargetType = FBM_SelectGroupType(MJPEG_MAX_WIDTH,MJPEG_MAX_HEIGHT);
    if (prVdecEsInfo->u4RenderPicCnt)
    {
        rPar.fgThumbnailMode = TRUE;
    }
	ucFbgId =FBM_CreateGroupExt(ucMJPEGTargetType,FBM_VDEC_JPEG, prVdecEsInfo->rMMSeqInfo.u2_width, prVdecEsInfo->rMMSeqInfo.u2_height, &rPar);
	if(ucFbgId==FBM_FBG_ID_UNKNOWN)
	{
		LOG(2, "create FBG failed.\n");
		return FALSE;
	}
	
	FBM_SetPlayMode(ucFbgId,FBM_FBG_MM_MODE);
	if(prVdecEsInfoKeep->fgVPush)
	{
		if(prVdecEsInfoKeep->fgVPushDecodeOnly)
		{
			FBM_SetPlayMode(ucFbgId,FBM_FBG_NOTDISPLAY_MODE);
		}
		FBM_RegFbgCbFunc(ucFbgId,
			FBM_CB_FUNC_FB_READY_EX_IND, (UINT32)_VPUSH_DecodeDone, ucEsId);
        if(prVdecEsInfoKeep->fgVPushFBGFromInst)
        {
		    FBM_RegFbgCbFunc(ucFbgId,
			FBM_CB_FUNC_FBG_CHG_EX_IND, (UINT32)_VPUSH_CreateFbgDone, ucEsId);
	    }
	}
	
	FBM_SetSyncStc(ucFbgId, prVdecEsInfo->ucSyncMode, prVdecEsInfo->ucStcId);
	FBM_FbgChgNotify(ucFbgId, ucEsId);
	prVdecImgInfo->ucFbgId=ucFbgId;
	prVdecEsInfo->ucFbgId=ucFbgId;
	prVdecEsInfo->ucFbId = 0xFF;
	return TRUE;
}


static BOOL _Vdec_ImageDecResetSeqInfo(UCHAR ucEsId,VDEC_IMG_INSTANCE_T *prVdecImgInfo)
{
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_ES_INFO_T *prVdecEsInfo;
    #ifdef CC_MAPLE_CUST_DRV
    ENUM_CODEC_INFO_T eCodecType;
    #endif
    //UCHAR ucMJPEGTargetType;
 	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecImgInfo->ucFbgId);
    if(prFbmSeqHdr == NULL)
    {
       LOG(2, "get seq hdr failed.\n");
       return (INT32)FALSE;
    }

	if((prVdecEsInfo->rMMSeqInfo.u2_width==0)||(prVdecEsInfo->rMMSeqInfo.u2_height==0))
	{
		prVdecEsInfo->rMMSeqInfo.u2_width =prVdecImgInfo->rOutParam.u4Width;
		prVdecEsInfo->rMMSeqInfo.u2_height = 
			prVdecImgInfo->fgIsMPF ? (prVdecImgInfo->rOutParam.u4Height << 1) : prVdecImgInfo->rOutParam.u4Height;
	}

	if ((prVdecEsInfo->rMMSeqInfo.u2_width > (UINT16)MJPEG_MAX_WIDTH) ||
		(prVdecEsInfo->rMMSeqInfo.u2_height> (UINT16)MJPEG_MAX_HEIGHT))
	{
		return FALSE;
	}

	if(prVdecEsInfo->rMMSeqInfo.e_frame_rate==VDEC_SRC_FRAME_RATE_UNKNOWN)
	{
	   prVdecEsInfo->rMMSeqInfo.e_frame_rate = VDEC_SRC_FRAME_RATE_29_97;
	}
		
	prFbmSeqHdr->ucAspRatInf = (UCHAR)prVdecEsInfo->rMMSeqInfo.e_src_asp;
		
	switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
	{
			case VDEC_SRC_FRAME_RATE_15:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
				break;
			case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24_;
				break;
			case VDEC_SRC_FRAME_RATE_24:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_24;
				break;
			case VDEC_SRC_FRAME_RATE_25:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_25;
				break;
			case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30_;
				break;
			case VDEC_SRC_FRAME_RATE_30:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;
				break;
			case VDEC_SRC_FRAME_RATE_50:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_50;
				break;
			case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60_;
				break;
			case VDEC_SRC_FRAME_RATE_60:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_60;
				break;
			default:
				prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_UNKNOWN;
				break;
	}
			
	if(prVdecImgInfo->rOutParam.ucPicStruct==1)
	{
        prFbmSeqHdr->fgProgressiveSeq=FALSE;
 	}
	else
	{
	    prFbmSeqHdr->fgProgressiveSeq = TRUE;
	}
		
	prFbmSeqHdr->fgBrokenLink = FALSE;
	prFbmSeqHdr->fgClosedGop = FALSE;
	prFbmSeqHdr->u2LineSize = prVdecImgInfo->rOutParam.u4Pitch;
	if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG))
	{
		prFbmSeqHdr->u2HSize = MJPEG_MAX_WIDTH;
		prFbmSeqHdr->u2VSize = MJPEG_MAX_HEIGHT;
		prFbmSeqHdr->u2DHS = MJPEG_MAX_WIDTH;
		prFbmSeqHdr->u2DVS = MJPEG_MAX_HEIGHT;
		prFbmSeqHdr->u2OrgHSize = MJPEG_MAX_WIDTH;
		prFbmSeqHdr->u2OrgVSize = MJPEG_MAX_HEIGHT;
	}
	else
	{
		prFbmSeqHdr->u2HSize = prVdecImgInfo->rOutParam.u4Width;
		prFbmSeqHdr->u2VSize = 
			prVdecImgInfo->fgIsMPF ? (prVdecImgInfo->rOutParam.u4Height << 1) : prVdecImgInfo->rOutParam.u4Height;
		prFbmSeqHdr->u2DHS = prVdecImgInfo->rOutParam.u4Width;
		prFbmSeqHdr->u2DVS = 
			prVdecImgInfo->fgIsMPF ? (prVdecImgInfo->rOutParam.u4Height << 1) : prVdecImgInfo->rOutParam.u4Height;
		prFbmSeqHdr->u2OrgHSize = prVdecImgInfo->rOutParam.u4Width;
		prFbmSeqHdr->u2OrgVSize = 
			prVdecImgInfo->fgIsMPF ? (prVdecImgInfo->rOutParam.u4Height << 1) : prVdecImgInfo->rOutParam.u4Height;
	}
		
#ifdef CC_3D_MM_DS_SUPPORT  
	if( prVdecEsInfo->e3DType == VDEC_3D_DUAL_STREAM)
	{
		prFbmSeqHdr->u1B2R3DTrickModeType =  prVdecEsInfo->u13DTrickModeType;
	}
#endif
#if defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
		prFbmSeqHdr->fgRasterOrder = TRUE;
#else
		prFbmSeqHdr->fgRasterOrder = FALSE;
#endif

#ifdef CC_RESIZE_SMALL_IMAGE
    if(((prFbmSeqHdr->u2HSize < FBM_IMG_RESIZE_LIMITATION) ||
        (prFbmSeqHdr->u2VSize < FBM_IMG_RESIZE_LIMITATION)) &&
        (!((prVdecEsInfoKeep->fgVPush) && (prVdecEsInfoKeep->fgVPushDecodeOnly))))
    {
	    prFbmSeqHdr->fgResizeSmallPic = TRUE;
	    if(prFbmSeqHdr->u2HSize < FBM_IMG_RESIZE_LIMITATION)
	    {
	        prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
	        prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
	        prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
	    }
	    else if(prFbmSeqHdr->u2HSize > FBM_IMG_RESIZE_LIMITATION_MAX)
	    {
	        prFbmSeqHdr->u2LineSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
	        prFbmSeqHdr->u2HSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
	        prFbmSeqHdr->u2OrgHSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
	    }
	    else
	    {
	        prFbmSeqHdr->u2LineSize = ((prFbmSeqHdr->u2HSize + 0x3F) & (~0x3F));
	        prFbmSeqHdr->u2HSize = prFbmSeqHdr->u2LineSize;
	        prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2LineSize;
	    }
	    if(prFbmSeqHdr->u2VSize < FBM_IMG_RESIZE_LIMITATION)
	    {
	        prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
	        prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION;
	    }
	    else if(prFbmSeqHdr->u2VSize > FBM_IMG_RESIZE_LIMITATION_MAX)
	    {
	        prFbmSeqHdr->u2VSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
	        prFbmSeqHdr->u2OrgVSize = (UINT16)FBM_IMG_RESIZE_LIMITATION_MAX;
	    }
	    else
	    {
	        prFbmSeqHdr->u2VSize = ((prFbmSeqHdr->u2VSize + 0x3F) & (~0x3F));
	        prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
	    }
	    prFbmSeqHdr->u2DHS = prFbmSeqHdr->u2HSize;
	    prFbmSeqHdr->u2DVS = prFbmSeqHdr->u2VSize;

	    LOG(6, "-> 0 OrgHSize %d, OrgVSize %d, HSize %d, VSize %d, LineSize %d\n",
	                   prFbmSeqHdr->u2OrgHSize, prFbmSeqHdr->u2OrgVSize,
	                   prFbmSeqHdr->u2HSize, prFbmSeqHdr->u2VSize, prFbmSeqHdr->u2LineSize);
    }
    else
    {
        prFbmSeqHdr->fgResizeSmallPic = FALSE;
    }
#endif
#ifdef CC_MAPLE_CUST_DRV
    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)
    {
   	  eCodecType = E_VDEC_CODEC_MOTION_JPEG;
    }
    FBM_SetCodecInfo(prVdecImgInfo->ucFbgId,eCodecType);
#endif    
    return TRUE;
}

#ifdef MJPEG_USE_NEW_FLOW
static BOOL _Vdec_ImageDecFindBottonFrame(VDEC_IMG_INSTANCE_T *prVdecImgInfo)
{
   UCHAR *puReadPos = (UCHAR *)prVdecImgInfo->u4ReadPtr;
   VDEC_PES_INFO_T *prPesInfo = NULL;
   UINT32 u4ReadSize;
   BOOL fgFindBottom = FALSE;
   UINT32 u4Data = 0, u4ByteValue = 0, u4Cnt = 0;  
   prPesInfo=&prVdecImgInfo->rPesInfo;
   
   if((prPesInfo->u4VldReadPtr < prPesInfo->u4FifoStart) || (prPesInfo->u4VldReadPtr > prPesInfo->u4FifoEnd)
		|| (prVdecImgInfo->u4WritePtr < prPesInfo->u4FifoStart) || (prVdecImgInfo->u4WritePtr > prPesInfo->u4FifoEnd))
   {
		//Invalid VldReadPtr or u4WritePtr, out of the fifo range.
		return FALSE;
   }
   
   if(prVdecImgInfo->u4WritePtr >= prVdecImgInfo->u4ReadPtr)
	{	
		LOG(6, "Wp 0x%x, Rp 0x%x, start 0x%x, end 0x%x\n", prVdecImgInfo->u4WritePtr, prVdecImgInfo->u4ReadPtr, 
			prPesInfo->u4FifoStart, prPesInfo->u4FifoEnd);	
		do
		{
			u4ReadSize=prVdecImgInfo->u4WritePtr-(UINT32)puReadPos;

			if((u4ReadSize < 11) || (u4ReadSize > (prPesInfo->u4FifoEnd - prPesInfo->u4FifoStart)))
			{
				return FALSE;
			}
			
			u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 0) & 0xff;
			u4Data = u4ByteValue;
			u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 1) & 0xff;
			u4Data = (u4Data << 8) + u4ByteValue;

			//print the u4Data
			LOG(6, "[%s][%d], peek u4Data[%x]\n", __FUNCTION__, __LINE__, u4Data);
			
			if(u4Data == 0xffd8)
			{
				//save current read pos
				prVdecImgInfo->u4ReadPtr=(UINT32)puReadPos; 		
			
				//skip SOI, APP0,  FFD8 FFE0 0010 + 41(A)	
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 7); 
				
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 0) & 0xff;
				u4Data = u4ByteValue;				
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 1) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;				
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 2) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;				
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 3) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;
				
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 4); 

				//no use
				puReadPos += 11;
				
				if(u4Data == 0x56493102)
				{
					LOG(4, "@@ Find another bottom field \n");				
					fgFindBottom = TRUE;
					break;
					
				}else if(u4Data == 0x56493101)
				{
					LOG(4, "@@ Find another top field \n");
					break;
				}
				else
				{
					LOG(4, "@@ none top or bottom field information u4Data[0x%x]!\n", u4Data);	
					break;
				}
			}
			else
			{
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 1); 
				puReadPos++;				
			}
			//when the data is bad, make sure that we can exit this loop as soon as possible.
			u4Cnt++;
		}while((u4Cnt < 30)&&(prVdecImgInfo->u4WritePtr > (UINT32)puReadPos));
	}
	else
	{
		u4ReadSize = (prPesInfo->u4FifoEnd - prVdecImgInfo->u4ReadPtr) +
			(prVdecImgInfo->u4WritePtr - prPesInfo->u4FifoStart);
		LOG(6, "Wp 0x%x, Rp 0x%x, start 0x%x, end 0x%x\n", prVdecImgInfo->u4WritePtr, prVdecImgInfo->u4ReadPtr, 
			prPesInfo->u4FifoStart, prPesInfo->u4FifoEnd);
		while((u4Cnt < 30) && (u4ReadSize > 11) && (u4ReadSize <= (prPesInfo->u4FifoEnd - prPesInfo->u4FifoStart)))
		{
			u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 0) & 0xff;
			u4Data = u4ByteValue;			
			u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 1) & 0xff;
			u4Data = (u4Data << 8) + u4ByteValue;
						
			LOG(6, "[%s][%d], peek u4Data[%x] Rp[%x] Wp[%x]\n", __FUNCTION__, __LINE__, u4Data);
			
			if(u4Data == 0xffd8)
			{	
				//save current read pos
				prVdecImgInfo->u4ReadPtr=(UINT32)puReadPos; 		
				
				//skip SOI, APP0,  FFD8 FFE0 0010 + 41(A)	
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 7); 
				
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 0) & 0xff;
				u4Data = u4ByteValue;
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 1) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;			
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 2) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;			
				u4ByteValue = MJPG_VLDPeekBytes(prVdecImgInfo->hJDec, 3) & 0xff;
				u4Data = (u4Data << 8) + u4ByteValue;
				//Four CC, AVI1, 01 TOP, 02 BOTTOM
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 4); 

				//no use
				puReadPos += 11;
				
				if(u4Data == 0x56493102)
				{
					LOG(4, "@@ Find another bottom field \n");				
					fgFindBottom = TRUE;
					break;
					
				}else if(u4Data == 0x56493101)
				{
					LOG(4, "@@ Find another top field \n");
					break;
				}
				else
				{
					LOG(4, "@@ none top or bottom field information u4Data[0x%x]!\n", u4Data);	
					break;
				}
			}
			else
			{
				MJPG_VLDSkipBytes(prVdecImgInfo->hJDec, 1); 
				puReadPos++;		  
				if(puReadPos == (UCHAR *)prPesInfo->u4FifoEnd)
				{
					puReadPos = (UCHAR *)prPesInfo->u4FifoStart;
				}				
			}
			u4ReadSize = (prPesInfo->u4FifoEnd - (UINT32)puReadPos) +
				(prVdecImgInfo->u4WritePtr - prPesInfo->u4FifoStart);	
			u4Cnt++;			
		}		
   }
	
   return fgFindBottom;
}

static INT32 _Vdec_MotionFrameDecode(UCHAR ucMpvId, UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
	VDEC_PES_INFO_T *prPesInfo=NULL;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT32 u4YAddr, u4CAddr;//,u4FrameWidth=0;
    UCHAR ucFbId = 0xff;
    UCHAR ucFbgId;
    BOOL fgNotDisplay,fgUpdateFb=TRUE;
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
    #ifdef CC_SUPPORT_MTK_ANDROID_RM    
    JPEG_JFIF_SOF_T* prJpegCom = NULL;
    INT32 i4RzId = 0;    
    UINT8 i =0;
    #endif
    //FBM_POOL_T* prFbmPool;
	prVdecImgInfo=&_rVdecImgInfo[ucEsId];
	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if ((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        return (INT32)FALSE;
    }
	
    if ((prVdecEsInfo->rMMSeqInfo.u2_width > (UINT16)MJPEG_MAX_WIDTH) ||
        (prVdecEsInfo->rMMSeqInfo.u2_height> (UINT16)MJPEG_MAX_HEIGHT))
    {
        return (INT32)E_VDEC_RES_NOT_SUPPORT;
    }
	
    prPesInfo=&prVdecImgInfo->rPesInfo;
#ifdef CC_SUPPORT_MTK_ANDROID_RM
    if (prVdecEsInfo->ucImgrzId)  
    {
        for (i4RzId = 0; i4RzId < IMGRZ4VDO_NUM; i4RzId++)
        {      
                if (prVdecEsInfo->ucImgrzId & (0x01<<i4RzId)) 
                {                    
                    LOG(3, "Mjpeg get imgrz id:%d\n", i4RzId);
                    break;
                }
        }
    }
    else
    {
		i4RzId = 0;
    }
#endif  

	if(prVdecImgInfo->ucFbgId==FBM_FBG_ID_UNKNOWN)
	{
       if(_Vdec_ImageDecCreateFBG(ucEsId,prVdecImgInfo)==FALSE)
       {
		   return (INT32)E_VDEC_RES_NOT_SUPPORT;
       }
	}

	ucFbgId=prVdecImgInfo->ucFbgId;

    if(FBM_ChkFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET))
    {
        FBM_ResetGroup(ucFbgId);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_NOT_READY);                  
    }
	
	prVdecImgInfo->u4ReadPtr=prPesInfo->u4VldReadPtr;
	prVdecImgInfo->rInParam.fgCheckMPF= FALSE;
	prVdecImgInfo->rInParam.u4Height=0;
	prVdecImgInfo->rInParam.u4Width=0;
	prVdecImgInfo->rInParam.u4Pitch=0;
	prVdecImgInfo->rOutParam.u4AuxFrameOffSet=0;
	LOG(8,"Start First field\n");
	
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecImgInfo->ucFbgId);
    if (prFbmSeqHdr == NULL)
		return (INT32)FALSE;
    #if 0
    if (prVdecEsInfo && prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {

        prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
	    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
    	return (INT32)TRUE;
    }
    #endif
	// For top/botton frame
 DECODE_FRAME: 

	ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
    if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        LOG(2, "get empty frame buffer failed.\n");        
        return (INT32)E_VDEC_RES_NOT_SUPPORT;
    }
	
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4YAddr, &u4CAddr);
    
    prVdecEsInfo->ucFbgId = ucFbgId;
    prVdecEsInfo->ucFbId = ucFbId;
    fgUpdateFb=TRUE;
	
    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
	if(prVdecImgInfo->fgOpened==FALSE)
	{
	   MJPG_OPEN_INFO_T rOpenInfo;
#ifndef CC_SUPPORT_MTK_ANDROID_RM
	   FBM_POOL_T *prFbmPool;
#endif
	   rOpenInfo.fpDevCb=NULL;
	   rOpenInfo.u4AsyncMode=FALSE;
	   rOpenInfo.u4DataBufStart=prPesInfo->u4FifoStart;
	   rOpenInfo.u4DataBufSize=prPesInfo->u4FifoEnd - prPesInfo->u4FifoStart;
#ifdef CC_SUPPORT_MTK_ANDROID_RM
            FBM_GetWorkingBuffer(ucFbgId, &rOpenInfo.u4WorkBufStart, &rOpenInfo.u4WorkBufSize);
            LOG(1, "mjpeg get working buffer:0x%x size:0x%x, use:0x%x\n", rOpenInfo.u4WorkBufStart, rOpenInfo.u4WorkBufSize, MJPEG_WORKBUF_SIZE_ALIGN);
            #ifdef CC_VDEC_FBM_DYNAMIC_MAPPING
            FBM_WorkBufPartMap(ucFbgId, rOpenInfo.u4WorkBufStart, MJPEG_FIRST_WORKBUF_ALIGN_SIZE);
            #endif
#else
       /*When playing MJPEG, can't use JPEG FBM, use last 300K of MPEG FBM instead*/
	   prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
	   if(prFbmPool==NULL)
	   {
          return E_VDEC_RES_NOT_SUPPORT;
	   }
	   rOpenInfo.u4WorkBufStart=prFbmPool->u4Addr + prFbmPool->u4Size - MJPEG_WORKBUF_SIZE_ALIGN;
	   rOpenInfo.u4WorkBufSize = MJPEG_WORKBUF_SIZE_ALIGN;
#endif
#ifdef CC_SUPPORT_MTK_ANDROID_RM            
       if(MJPG_DecoderOpen((HANDLE_T)prVdecImgInfo,(HANDLE_T *)&prVdecImgInfo->hJDec,&rOpenInfo, i4RzId)!=MJPEG_RET_OK)
#else
	   if(MJPG_DecoderOpen((HANDLE_T)prVdecImgInfo,(HANDLE_T *)&prVdecImgInfo->hJDec,&rOpenInfo)!=MJPEG_RET_OK)
#endif
	   {
		  LOG(0, "MJPG_DecoderOpen failed.\n");
		  _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
	      return E_VDEC_RES_NOT_SUPPORT;
	   }
	   else
	   {
          prVdecImgInfo->fgOpened=TRUE;
	   }

       x_memset((VOID *)&prVdecImgInfo->rInParam, 0x0, sizeof(MJPG_INPARAM_T));
       prVdecImgInfo->rInParam.u4DataReady=TRUE;
       prVdecImgInfo->rInParam.u4ForVdec=TRUE;
       if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG)
       {
		   prVdecImgInfo->rInParam.eFmt=IMAGE_PNG;
       }
	   else if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_MJPEG||prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)
	   {
		   prVdecImgInfo->rInParam.eFmt=IMAGE_JPEG;
	   }
	   else 
	   {
           ASSERT(0);
           LOG(2, "[%s-%d]fail.\n", __func__, __LINE__);
		   _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		   return E_VDEC_RES_NOT_SUPPORT;
	   }
	   
	   x_memset((VOID *)&prVdecImgInfo->rOutParam, 0x0, sizeof(MJPG_OUTPARAM_T));
	   prVdecImgInfo->rOutParam.ucPicStruct=3;
	}

    if(prVdecImgInfo->fgOpened==FALSE)
    {
	    LOG(2, "MJPG_DecoderOpen not opened.\n");
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		ASSERT(prVdecImgInfo->fgOpened);
		return E_VDEC_RES_NOT_SUPPORT;
    }

    prVdecImgInfo->rInParam.u4DataAddr=prVdecImgInfo->u4ReadPtr;
	prVdecImgInfo->rInParam.u4DstAddr=u4YAddr;
	prVdecImgInfo->rInParam.u4CbCrOffset=u4CAddr - u4YAddr;
    
	if(prVdecImgInfo->u4WritePtr>prPesInfo->u4VldReadPtr)
	{
		prVdecImgInfo->rInParam.u4FrameSize=prVdecImgInfo->u4WritePtr-prPesInfo->u4VldReadPtr;
	}
	else
	{
		prVdecImgInfo->rInParam.u4FrameSize=(prPesInfo->u4FifoEnd - prPesInfo->u4VldReadPtr)+(prVdecImgInfo->u4WritePtr-prPesInfo->u4FifoStart);
	}

	
#ifdef CC_FUJI3D_SUPPORT
	if(!prVdecEsInfo->fgRenderPicCnt)
	{
		prVdecImgInfo->rInParam.fgCheckMPF= TRUE;
	}
#endif

DECODE_AUXFRAME:
#ifdef CC_SUPPORT_MTK_ANDROID_RM
	if(MJPG_DecoderDecode(prVdecImgInfo->hJDec,&prVdecImgInfo->rInParam,&prVdecImgInfo->rOutParam,i4RzId)!=MJPEG_RET_OK)
#else
	if(MJPG_DecoderDecode(prVdecImgInfo->hJDec,&prVdecImgInfo->rInParam,&prVdecImgInfo->rOutParam)!=MJPEG_RET_OK)
#endif
	{
		LOG(2, "MJPG_DecoderDecode Failed.\n");
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		return E_VDEC_FAIL;
	}
#ifdef CC_SUPPORT_MTK_ANDROID_RM
    if(u1MJPGColorMode == 0xff)
    {          
        prJpegCom = MJPEG_GetSOF(_rVdecImgInfo[ucEsId].hJDec);
        do
        {
            if((prJpegCom->arSofComp[i].u1HSampFactor == 1) && (prJpegCom->arSofComp[i].u1VSampFactor == 1))
            {
                i ++;
            }
            else
            {
                break;
            }
        }while(i < prJpegCom->u1NumComponents);
        
        if (prVdecEsInfoKeep->pfDecNfyFct)
        {
            if(i < prJpegCom->u1NumComponents)
            {
                prVdecEsInfoKeep->pfDecNfyFct(prVdecEsInfoKeep->pvDecNfyTag, VDEC_MJPG_IMGRZ_RELEASE, 0, 0);
                LOG(3, "MJPEG not 444, release IMGRZ hw\n");                
                u1MJPGColorMode = 0;     
            }
            else
            {
                u1MJPGColorMode = 1;// 444
            }
        }
    }        
    #endif
	if(prVdecImgInfo->rInParam.fgCheckMPF&&prVdecImgInfo->rOutParam.u4AuxFrameOffSet)
	{
        fgUpdateFb=FALSE;
        prVdecEsInfo->e3DType = VDEC_3D_TB_LF;	
		prVdecImgInfo->fgIsMPF = TRUE;		
	}
	//feedback real width&height, when MW don't pass the sequence info by raw data info (0xfb) in avi container
	if((prVdecEsInfo->rMMSeqInfo.u2_width==0)||(prVdecEsInfo->rMMSeqInfo.u2_height==0))
	{
		prVdecEsInfo->rMMSeqInfo.u2_width =prVdecImgInfo->rOutParam.u4Width;
		prVdecEsInfo->rMMSeqInfo.u2_height =prVdecImgInfo->rOutParam.u4Height;		
	}

	if(prVdecImgInfo->fgSeqChanged)
	{
        if(_Vdec_ImageDecResetSeqInfo(ucEsId,prVdecImgInfo)==FALSE)
        {
            LOG(2, "[%s-%d]fail.\n", __func__, __LINE__);
		   _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		   return E_VDEC_RES_NOT_SUPPORT;
        }
		FBM_SetFrameBufferFlag(prVdecImgInfo->ucFbgId, FBM_FLAG_SEQ_CHG);
        prVdecImgInfo->fgSeqChanged=FALSE;
	}

    if(fgUpdateFb)
    {
		//prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(prVdecImgInfo->ucFbgId);
		if(prFbmSeqHdr == NULL)
		{
		   LOG(2, "get seq hdr failed.\n");
		   return (INT32)FALSE;
		}

		prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
		if (prFbmPicHdr == NULL)
		{
		    LOG(2, "get pic hdr failed.\n");
		    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
		    return (INT32)E_VDEC_RES_NOT_SUPPORT;
		}

#ifndef MJPG_PROCE_FIELD_FRAME
        prVdecImgInfo->rOutParam.ucPicStruct=3;
#endif
		prFbmPicHdr->ucPicCdTp = (UCHAR)I_TYPE;
		prFbmPicHdr->ucPicStruct = (UCHAR)prVdecImgInfo->rOutParam.ucPicStruct;
		prFbmPicHdr->fgProgressiveFrm = TRUE; 
		prFbmPicHdr->fgRepFirstFld = FALSE;
		prFbmPicHdr->fgTopFldFirst = FALSE;
		prFbmPicHdr->u4PTS = prVdecEsInfo->u4PTS;
		prFbmPicHdr->u8PTS = prVdecEsInfo->u8PTS;
		prFbmPicHdr->fgRepFirstFld = FALSE;
		if(prFbmPicHdr->ucPicStruct == 1 || prFbmPicHdr->ucPicStruct==2)
		{
			prFbmSeqHdr->fgProgressiveSeq = FALSE;
			prFbmPicHdr->fgProgressiveFrm = FALSE;
			prFbmPicHdr->fgTopFldFirst = TRUE;
		}
		else
		{
			prFbmSeqHdr->fgProgressiveSeq = TRUE;
			prFbmPicHdr->fgProgressiveFrm = TRUE;
			prFbmPicHdr->fgTopFldFirst = FALSE;
		}

#ifdef CC_RESIZE_SMALL_IMAGE
		prFbmPicHdr->u4PicWidth = prVdecImgInfo->rOutParam.u4Width;
		prFbmPicHdr->u4PicWidthPitch = prVdecImgInfo->rOutParam.u4Pitch;
		prFbmPicHdr->u4PicHeight = prVdecImgInfo->rOutParam.u4Height;
#endif

		//add for A-B repeart precisely
		prFbmPicHdr->u8Offset = prVdecEsInfo->u8Offset;   
		prFbmPicHdr->u8OffsetI = prVdecEsInfo->u8OffsetI;
		prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
		x_memcpy(&prFbmPicHdr->rExtra, &prVdecEsInfo->rExtra, sizeof(FBM_PIC_EXTRA_INFO_T));


		CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
		fgNotDisplay = VDEC_IsNotDisplay((UCHAR)ucEsId, prFbmPicHdr->u4PTS, 0, 0);
		if (fgNotDisplay)
		{
		    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
		    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
            LOG(6, "[%s-%d]EMPTY.\n", __func__, __LINE__);
		}
		else
		{
#ifdef MJPEG_SUPPORT_VENC
		    if (_VDEC_fg2VencFlowOn(ucEsId))
		    {
		        VDEC_VENC_SRC_INFO_T rVdec2VencSrcInfo;
		        rVdec2VencSrcInfo.u4YAddr = u4YAddr;
		        rVdec2VencSrcInfo.u4CAddr = u4CAddr;
		        rVdec2VencSrcInfo.u4Pitch = prFbmSeqHdr->u2LineSize;
		        rVdec2VencSrcInfo.u4Width = prFbmSeqHdr->u2HSize;
		        rVdec2VencSrcInfo.u4Height = prFbmSeqHdr->u2VSize;
		        rVdec2VencSrcInfo.u4Pts = prFbmPicHdr->u4PTS;
		        if (!_VDEC_PrepareVEncSourse(ucEsId, &rVdec2VencSrcInfo))
		        {
		        }
		    }
#endif
		    #ifdef CC_REALD_3D_SUPPORT
		    _VDEC_Check2D3DSwitch(ucEsId, ucFbId);
		    #endif

		    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
		    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
            
            LOG(6, "[%s-%d]FB(%d-%d) DISPLAYQ.\n", __func__, __LINE__, ucFbgId, ucFbId);
			if(prFbmPicHdr->ucPicStruct==2 || prFbmPicHdr->ucPicStruct==3)
			{
				prVdecEsInfo->u4DisplayQPicCnt++;
			}
		    VDEC_CheckStopStatus(ucEsId, prFbmPicHdr->u4PTS, 0);
		}

    }
    else
    {
        LOG(2, "[%s-%d]fail.\n", __func__, __LINE__);
    }

	prVdecImgInfo->u4ReadPtr=_Vdec_ImageDecReadPointer();

    if(prFbmPicHdr&&prFbmPicHdr->ucPicStruct==1)
    {
       if(_Vdec_ImageDecFindBottonFrame(prVdecImgInfo))
       {
		   ASSERT(prPesInfo->u4VldReadPtr>=prPesInfo->u4FifoStart&&prPesInfo->u4VldReadPtr<prPesInfo->u4FifoEnd);
		   prVdecImgInfo->rInParam.fgCheckMPF=FALSE;
		   _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		   LOG(8,"Start Botton field\n");
		   goto DECODE_FRAME;
       }
	   else
	   {
	      LOG(2,"Interleave Frame not find botton field\n");
//          ASSERT(0);
	   }
    }
	else if(prVdecImgInfo->rOutParam.u4AuxFrameOffSet)
	{
       if(prVdecImgInfo->rOutParam.u4AuxFrameOffSet+prPesInfo->u4VldReadPtr>=prPesInfo->u4FifoEnd)
       {
          prVdecImgInfo->u4ReadPtr=prPesInfo->u4FifoStart + \
		  	(prPesInfo->u4VldReadPtr+prVdecImgInfo->rOutParam.u4AuxFrameOffSet-prPesInfo->u4FifoEnd);
       }
	   else
	   {
          prVdecImgInfo->u4ReadPtr=prPesInfo->u4VldReadPtr+prVdecImgInfo->rOutParam.u4AuxFrameOffSet;
	   }
	   
	   ASSERT(prPesInfo->u4VldReadPtr>=prPesInfo->u4FifoStart&&prPesInfo->u4VldReadPtr<prPesInfo->u4FifoEnd);
	   prVdecImgInfo->rInParam.u4DataAddr=prVdecImgInfo->u4ReadPtr;
	   prVdecImgInfo->rInParam.u4DstAddr=u4YAddr+prVdecImgInfo->rOutParam.u4Pitch*prVdecImgInfo->rOutParam.u4Height;
	   prVdecImgInfo->rInParam.u4CbCrOffset-=prVdecImgInfo->rOutParam.u4Pitch*(prVdecImgInfo->rOutParam.u4Height>>1);
	   if(prVdecImgInfo->u4WritePtr>prPesInfo->u4VldReadPtr)
	   {
		   prVdecImgInfo->rInParam.u4FrameSize=prVdecImgInfo->u4WritePtr-prPesInfo->u4VldReadPtr;
	   }
	   else
	   {
		   prVdecImgInfo->rInParam.u4FrameSize=(prPesInfo->u4FifoEnd - prPesInfo->u4VldReadPtr)+(prVdecImgInfo->u4WritePtr-prPesInfo->u4FifoStart);
	   }
       prVdecImgInfo->rInParam.u4Height=prVdecImgInfo->rOutParam.u4Height;
	   prVdecImgInfo->rOutParam.u4Pitch=prVdecImgInfo->rOutParam.u4Pitch;
	   prVdecImgInfo->rInParam.fgCheckMPF=FALSE;
	   prVdecImgInfo->rOutParam.u4AuxFrameOffSet=0;
	   fgUpdateFb=TRUE;
	   _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
	   LOG(8,"Start Aux Frame\n");
	   goto DECODE_AUXFRAME;
	}
	
    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    return (INT32)E_VDEC_OK;
}
#endif

static INT32 _Vdec_StillFrameDecode(UCHAR ucMpvId, UCHAR ucEsId)
{
    IMG_PARAM_T rInfo;
    IMG_REGION_T rDecOut;
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
	VDEC_PES_INFO_T *prPesInfo=NULL;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    UINT32 u4YAddr, u4CAddr, u4DstAddr;//,u4FrameWidth=0;
    UCHAR ucFbId = 0xff;
    UCHAR ucFbgId;
    BOOL fgNotDisplay;
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
    VDEC_IMG_FRMDONE_DATA rCBData = {0};
    //FBM_POOL_T* prFbmPool;
#ifdef CC_FUJI3D_SUPPORT
    UINT8 u1Marker;
    //HAL_TIME_T rTimeHwS, rTimeHwDt, rTimeHwDt2, rTimeHwE;
#endif

	prVdecImgInfo=&_rVdecImgInfo[ucEsId];
	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    if ((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        return (INT32)FALSE;
    }
    if ((prVdecEsInfo->rMMSeqInfo.u2_width > (UINT16)MJPEG_MAX_WIDTH) ||
        (prVdecEsInfo->rMMSeqInfo.u2_height> (UINT16)MJPEG_MAX_HEIGHT))
    {
        return (INT32)E_VDEC_RES_NOT_SUPPORT;
    }
    prPesInfo=&prVdecImgInfo->rPesInfo;
    
#ifdef CC_FUJI3D_SUPPORT
    //JPEG marker check
    if((prPesInfo->u4VldReadPtr + 1) == prPesInfo->u4FifoEnd)
    {
        u1Marker = *(UINT8*)VIRTUAL(prPesInfo->u4FifoStart);
    }
    else
    {
        u1Marker = *(UINT8*)VIRTUAL(prPesInfo->u4VldReadPtr + 1);
    }
    if((*(UINT8*)VIRTUAL(prPesInfo->u4VldReadPtr) != 0xFF) || (u1Marker != 0xD8))
    {
        LOG(2, "SkipFrm, not JPEG format, pts %d\n", prPesInfo->u4PTS);
        return (INT32)FALSE;
    }
#endif    	    
    if(prVdecImgInfo->ucFbgId==FBM_FBG_ID_UNKNOWN)
    {
		if(_Vdec_ImageDecCreateFBG(ucEsId,prVdecImgInfo)==FALSE)
		{
			return (INT32)E_VDEC_RES_NOT_SUPPORT;
		}
	}


	ucFbgId=prVdecImgInfo->ucFbgId;
    if (FBM_ChkFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET))
    {
        FBM_ResetGroup(ucFbgId);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_RESET);
        FBM_ClrFrameBufferFlag(ucFbgId, (UINT32)FBM_FLAG_NOT_READY);                  
    }
    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if (prFbmSeqHdr == NULL)
    {
        LOG(0, "get seq hdr failed.\n");        
        return (INT32)FALSE;
    }
#if 0
    if (prVdecEsInfo && prVdecEsInfo->fgParsingInfo && !prVdecEsInfo->fgThumbMethod2)
    {

        prFbmSeqHdr->u2HSize = prVdecEsInfo->rMMSeqInfo.u2_width;
	    prFbmSeqHdr->u2VSize = prVdecEsInfo->rMMSeqInfo.u2_height;
    	return (INT32)TRUE;
    }
#endif

#if 1
// for vdec to venc support, we always use FBM_GetEmptyFrameBuffer
// This will simply modification of FBM
    if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG))
    {
        if((prVdecEsInfo->ucFbId == 0xFF) || !prVdecEsInfo->fgNotClearBuffer)
        {
            if((prVdecEsInfo->ucFbId != 0xFF))
            {
                FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            }
            ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
        }
        else
        {
            ucFbId = prVdecEsInfo->ucFbId;
        }
    }
    else
    {
        ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);
    }
#else
    ucFbId = FBM_GetEmptyRefFrameBuffer(ucFbgId, (UINT32)1500);
#endif
    if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        LOG(0, "get empty frame buffer failed.\n");        
        return (INT32)FALSE;
    }
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4YAddr, &u4CAddr);    

    prVdecEsInfo->ucFbgId = ucFbgId;
    prVdecEsInfo->ucFbId = ucFbId;
    
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    if (prFbmPicHdr == NULL)
    {
        LOG(0, "get pic hdr failed.\n");
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
        return (INT32)FALSE;
    }

    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);

    _VDEC_MJPEGSWRst(ucMpvId, ucEsId);

    if (IMG_GetDecState(0) != IMG_INIT)
    {
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
        _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
        return (INT32)FALSE;
    }

    
    x_memset((VOID *)&rDecOut, 0x0, sizeof(IMG_REGION_T));
    x_memset((VOID *)&rInfo, 0x0, sizeof(IMG_PARAM_T));
    rInfo.pu1InBuffer = (UINT8 *)prPesInfo->u4FifoStart;
    rInfo.u8BufSize = (UINT32)(prPesInfo->u4FifoEnd - prPesInfo->u4FifoStart);
    rInfo.rDstRegion.u4StartX = (UINT32)0;
    rInfo.rDstRegion.u4StartY = (UINT32)0;
    //rDecOut.ucPicStruct=3;
 
    rInfo.rDstRegion.u4Width = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_width;
    rInfo.rDstRegion.u4Height = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_height;
    rInfo.rDstRegion.u4Pitch = (UINT32)((prVdecEsInfo->rMMSeqInfo.u2_width + 0x3F)&(~0x3F));

    if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG)
    {
        IMG_Set_ImgSize(0,prVdecEsInfo->u4Imagefilesize);
        rInfo.u4DecColorMode = GFX_COLORMODE_ARGB_D8888;
        rInfo.eDispColorMode = AYCBCR8888_DIRECT32;
    }            
    else
    {
        rInfo.eDispColorMode = Y_CBCR420_SEP16;
    }

    rInfo.eRotateDegree = CLOCKWISE_0;
    rInfo.bKeepRatio = FALSE;
    rInfo.prDecOut = &rDecOut;
    
    rInfo.rDstRegion.pu1Addr = (UINT8 *)u4YAddr;
    rInfo.rDstRegion.u4CbCrOffset = u4CAddr - u4YAddr;
    //TO DO
    rInfo.rMjpg.u4RingOffset = prPesInfo->u4VldReadPtr - prPesInfo->u4FifoStart;
    rInfo.rMjpg.ucFbgId = ucFbgId;

#ifdef VDEC_TIME_PROFILE

    prVdecImgInfo->rOther.u4CurDecMs = 0;
    prVdecImgInfo->rOther.u4CurDecCycle = 0;
    prVdecImgInfo->rOther.u4CurDramCycle = 0;
    prVdecImgInfo->rOther.u1DramMoniter = 2;

    HAL_GetTime(&_rTimeHwS);

    u4DelayClock = (BSP_GetDomainClock(SRC_MEM_CLK) >> 1) ; //dram cycle /sec
#if 0    
    if (prVdecImgInfo->rOther.u1DramMoniter == 1) //total bandwidth
    {
#ifdef CC_MT5395
        u4DramCycle = SRM_DramMonitor(0, SRM_DRV_TOTAL, u4DelayClock, 0);
#else
        u4DramCycle = SRM_DramMonitor(SRM_DRV_TOTAL, u4DelayClock, 0);
#endif
    }
    else if (prVdecImgInfo->rOther.u1DramMoniter == 2) //mpeg
#endif        
    {
#ifdef VDEC_TIME_PROFILE
#if defined(CC_MT5395) || defined(VDEC_IS_POST_MT5368)
        u4DramCycle = SRM_DramMonitor(0, SRM_DRV_JPEGDEC, u4DelayClock, 0);
#else
        u4DramCycle = SRM_DramMonitor(SRM_DRV_JPEGDEC, u4DelayClock, 0);
#endif
#endif
    }

#endif

    if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG))
    {
        IMG_SetUseMw(0,TRUE); //block for initial buffer filling.
        if(prVdecEsInfo->fgIsThumbnail)
        {
            rInfo.rDstRegion.u4Width = prVdecEsInfo->u4OutputPicW;
            rInfo.rDstRegion.u4Height = prVdecEsInfo->u4OutputPicH;
            rInfo.bKeepRatio = FALSE;
            if((prVdecEsInfo->u4OutputPicW < 128) || (prVdecEsInfo->u4OutputPicH < 128))
            {
                rInfo.fgThumbnailDec = TRUE;
            }
        }
        else
        {
            rInfo.rDstRegion.u4Width = (UINT32)MJPEG_MAX_WIDTH;
            rInfo.rDstRegion.u4Height = (UINT32)MJPEG_MAX_HEIGHT;
        }
        rInfo.rDstRegion.u4Pitch = (UINT32)MJPEG_MAX_WIDTH;
    }
    else
    {
        IMG_SetFillBsBufEnd(0,(UINT32)rInfo.u8BufSize, 0, TRUE);
    }
	
#ifdef CC_FUJI3D_SUPPORT
	if(!prVdecEsInfo->fgRenderPicCnt)
	{
		rInfo.fgMPFParsing = TRUE;
		prVdecImgInfo->fgMPFParsing = TRUE;
		//HAL_GetTime(&rTimeHwS);
	}
#endif

    IMG_Decode(0,rInfo);        

    VERIFY (x_sema_lock(_hMJPGDecSema, X_SEMA_OPTION_WAIT) == OSR_OK);

#ifdef CC_FUJI3D_SUPPORT
    //HAL_GetTime(&rTimeHwE);
    //HAL_GetDeltaTime(&rTimeHwDt, &rTimeHwS, &rTimeHwE);

    if(prVdecImgInfo->u4MPFOffset > 0)
    {
        //HAL_GetTime(&rTimeHwS);

        IMG_Stop(0);
        IMG_Release(0);

        //decode 2nd MPF image
        _VDEC_MJPEGSWRst(ucMpvId, ucEsId);

        if (IMG_GetDecState(0) != IMG_INIT)
        {
		    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
            return (INT32)FALSE;
        }

        rInfo.rMjpg.u4RingOffset += prVdecImgInfo->u4MPFOffset;
        if(rInfo.rMjpg.u4RingOffset > rInfo.u8BufSize)
        {
            rInfo.rMjpg.u4RingOffset -= rInfo.u8BufSize;
        }
        //LOG(2, "DecMPF Addr 0x%x+0x%x\n", 
        //    (prPesInfo->u4FifoStart + rInfo.rMjpg.u4RingOffset), prVdecImgInfo->u4MPFOffset);

        rInfo.rDstRegion.pu1Addr = (UINT8 *)u4YAddr + 
            (rDecOut.u4Pitch * rDecOut.u4Height);
        rInfo.rDstRegion.u4CbCrOffset -= (rDecOut.u4Pitch * (rDecOut.u4Height >> 1));
        
        rInfo.rDstRegion.u4Width = rDecOut.u4Pitch;
        rInfo.rDstRegion.u4Height = rDecOut.u4Height;
        rInfo.bKeepRatio = FALSE;
        rInfo.fgMPFParsing = FALSE;
        prVdecImgInfo->fgMPFParsing = FALSE;
        
        IMG_SetFillBsBufEnd(0,(UINT32)rInfo.u8BufSize, 0, TRUE);        
        IMG_Decode(0,rInfo);
        VERIFY (x_sema_lock(_hMJPGDecSema, X_SEMA_OPTION_WAIT) == OSR_OK);

        rDecOut.u4Height <<= 1;	//re-setting TnB size to full size
        prVdecEsInfo->e3DType = VDEC_3D_TB_LF;
/*
        //HAL_GetTime(&rTimeHwE);
        //HAL_GetDeltaTime(&rTimeHwDt2, &rTimeHwS, &rTimeHwE);
        if((rTimeHwDt.u4Micros + rTimeHwDt2.u4Micros) > 33000)
        {
            LOG(2, "DT %d.%d/%d.%d, %d\n", rTimeHwDt.u4Seconds, rTimeHwDt.u4Micros/1000,
                rTimeHwDt2.u4Seconds, rTimeHwDt2.u4Micros/1000, prVdecEsInfo->u8PTS);
        }
*/
    }
#endif

#ifdef VDEC_TIME_PROFILE

    _rTimeHwDt.u4Micros = 0;
    HAL_GetTime(&_rTimeHwE);
    HAL_GetDeltaTime(&_rTimeHwDt, &_rTimeHwS, &_rTimeHwE);

    u4DecCycle = MC_READ32(0x778);
    u4DramCycle = IO_REG32(DRAM_BASE, 0x190);

    prVdecImgInfo->rOther.u4CurDecMs += (_rTimeHwDt.u4Micros / 1000);
    prVdecImgInfo->rOther.u4CurDecCycle += u4DecCycle;
    prVdecImgInfo->rOther.u4CurDramCycle += u4DramCycle;

#endif

	if(prVdecImgInfo->fgSeqChanged)
	{
	    prVdecImgInfo->rOutParam.u4Width=rDecOut.u4Width;
	    prVdecImgInfo->rOutParam.u4Height=rDecOut.u4Height;
	    prVdecImgInfo->rOutParam.u4Pitch=rDecOut.u4Pitch;
		prVdecImgInfo->rOutParam.ucPicStruct=rDecOut.ucPicStruct;
		prVdecImgInfo->rOutParam.pu1Addr=rDecOut.pu1Addr;
		prVdecImgInfo->rOutParam.u4CbCrOffset=rDecOut.u4CbCrOffset;
        if(_Vdec_ImageDecResetSeqInfo(ucEsId,prVdecImgInfo)==FALSE)
        {
		   _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
		   return E_VDEC_RES_NOT_SUPPORT;
        }
        prVdecImgInfo->fgSeqChanged=FALSE;
	}

    prFbmPicHdr->ucPicCdTp = (UCHAR)I_TYPE;
    prFbmPicHdr->ucPicStruct = (UCHAR)rDecOut.ucPicStruct;
    prFbmPicHdr->fgProgressiveFrm = TRUE; 
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    prFbmPicHdr->u4PTS = prVdecEsInfo->u4PTS;       
    prFbmPicHdr->u8PTS = prVdecEsInfo->u8PTS;

    //add for A-B repeart precisely
    prFbmPicHdr->u8Offset = prVdecEsInfo->u8Offset;   
    prFbmPicHdr->u8OffsetI = prVdecEsInfo->u8OffsetI;
    prFbmPicHdr->u4Timestap = prVdecEsInfo->u4Timestap;
    x_memcpy(&prFbmPicHdr->rExtra, &prVdecEsInfo->rExtra, sizeof(FBM_PIC_EXTRA_INFO_T));


#ifdef CC_RESIZE_SMALL_IMAGE
    prFbmPicHdr->u4PicWidth = rDecOut.u4Width;
    prFbmPicHdr->u4PicWidthPitch = rDecOut.u4Pitch;
    prFbmPicHdr->u4PicHeight = rDecOut.u4Height;
#endif

    if (prVdecImgInfo->u4MJPEGNtyValue!= (UINT32)JPG_HW_NFY_FINISHED)
    {
    	//return (INT32)FALSE; 
    	LOG(1, "one frame broken\n");
    }
    CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);
    fgNotDisplay = VDEC_IsNotDisplay((UCHAR)ucEsId, prFbmPicHdr->u4PTS, 0, 0);
    if (fgNotDisplay || (prVdecImgInfo->u4MJPEGNtyValue != (UINT32)JPG_HW_NFY_FINISHED))
    {
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
    }
    else
    {
#ifdef MJPEG_SUPPORT_VENC
        if (_VDEC_fg2VencFlowOn(ucEsId))
        {
            VDEC_VENC_SRC_INFO_T rVdec2VencSrcInfo;
            rVdec2VencSrcInfo.u4YAddr = u4YAddr;
            rVdec2VencSrcInfo.u4CAddr = u4CAddr;
            rVdec2VencSrcInfo.u4Pitch = prFbmSeqHdr->u2LineSize;
            rVdec2VencSrcInfo.u4Width = prFbmSeqHdr->u2HSize;
            rVdec2VencSrcInfo.u4Height = prFbmSeqHdr->u2VSize;
            rVdec2VencSrcInfo.u4Pts = prFbmPicHdr->u4PTS;
            if (!_VDEC_PrepareVEncSourse(ucEsId, &rVdec2VencSrcInfo))
            {
            }
        }
#endif
        FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
        #ifdef CC_REALD_3D_SUPPORT
        _VDEC_Check2D3DSwitch(ucEsId, ucFbId);
        #endif
        if((prVdecEsInfoKeep->eCurFMT == VDEC_FMT_JPG)||(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG))
        {
            x_memcpy((void*)&_rDecOut, (void*)&rDecOut, sizeof(_rDecOut));
            FBM_UpdateReferenceList(ucFbgId, ucFbId, TRUE);
            rCBData.u2OriginalWidth = rDecOut.u4Width;
            rCBData.u2OriginalHeight = rDecOut.u4Height;
            if((prVdecEsInfo->u4RotationDegree == 0) && (!prVdecEsInfo->fgIsThumbnail))
            {
                FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
                rCBData.u2OutputWidth = MJPEG_MAX_WIDTH;
                rCBData.u2OutputHeight = MJPEG_MAX_HEIGHT;
                _VPUSH_ImgFrmDone(ucEsId, (void*)&rCBData);
            }
            else
            {
                //prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_MPEG);
                //check MPEG partition
                #if 0
                u4DstAddr = prFbmPool->u4Addr;
                if(u4YAddr < (prFbmPool->u4Addr + (prFbmPool->u4Size >> 1)))
                {
                    u4DstAddr += (prFbmPool->u4Size >> 1);
                }
                #endif
                u4DstAddr = u4CAddr + (u4CAddr - u4YAddr);
                FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_EMPTY);
                if(prVdecEsInfo->fgIsThumbnail)
                {
                    //color convert
                    GFX_Lock();
                    GFX_MMU_Set_Enable(FALSE);
                    GFX_SetColCnvFmt(E_YCFMT_420LINEAR, (UINT32)E_SWAP_BLOCK, (UINT32)E_VSTD_BT709, 
                        (UINT32)E_VSYS_COMP);
                    GFX_SetColCnvSrc((UINT8*)u4YAddr, prFbmSeqHdr->u2LineSize,
                        (UINT8*)u4CAddr, prFbmSeqHdr->u2LineSize, 0);
                    GFX_SetDst((UINT8*)u4DstAddr, CM_ARGB8888_DIRECT32, prVdecEsInfo->u4OutputPicW<<2);
                    GFX_SetAlpha(0xFF);
                    GFX_ColConv(0, 0, prVdecEsInfo->u4OutputPicW, prVdecEsInfo->u4OutputPicH);
                    
                    GFX_Flush();
                    GFX_Wait();
                    GFX_Unlock();
                    rCBData.u2OutputWidth = prVdecEsInfo->u4OutputPicW;
                    rCBData.u2OutputHeight = prVdecEsInfo->u4OutputPicH;
                    rCBData.pBuffer = (UINT8*)(VIRTUAL(u4DstAddr));
                    _VPUSH_ImgFrmDone(ucEsId, (void*)&rCBData);
                }
                else    //rotation
                {
                    _VDEC_MJPEGSetParam(ucEsId, 0, prVdecEsInfo->u4RotationDegree/90, 0, 0);
                }
            }
        }
        else
        {
            FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE);
            FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
        }
        prVdecEsInfo->u4DisplayQPicCnt++;
        VDEC_CheckStopStatus(ucEsId, prFbmPicHdr->u4PTS, 0);
    }    
    prVdecImgInfo->u4ReadPtr=_Vdec_ImageDecReadPointer();
    IMG_Stop(0);
    IMG_Release(0);
    
    _VDEC_UnlockVld(ucEsId, prVdecEsInfo->u4VldId);
    
    return (INT32)E_VDEC_OK;
}


// *********************************************************************
// Function    : VOID _VDEC_MJPEGInit
// Description : Initialization procedure of vdec
// Parameter   : 
// Return      : 
// *********************************************************************
INT32 _VDEC_MJPEGInit(UCHAR ucEsId)
{
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
	VDEC_ES_INFO_T *prVdecEsInfo;
	ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    _VDEC_LockVld(ucEsId, prVdecEsInfo->u4VldId);
    if(!_fgVdecImgLibInited)
    {
        if (!_hMJPGDecSema)
        {
            if (x_sema_create(&_hMJPGDecSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) != OSR_OK)
            {
                _VDEC_UnlockVld(ucEsId,prVdecEsInfo->u4VldId);
                return E_VDEC_FAIL;
            }
        }

        IMGRZ_Init();
		IMG_Lib_Init();
    }
	
	if(prVdecEsInfoKeep->eCurFMT == VDEC_FMT_PNG)
	{
		IMG_Config(0,IMAGE_PNG);
	}
	else
	{
		IMG_Config(0,IMAGE_JPEG);
	}
	
    _fgVdecImgLibInited=TRUE;
    x_memset((VOID *)(&_rVdecImgInfo[ucEsId]),0,sizeof(VDEC_IMG_INSTANCE_T));
    _rVdecImgInfo[ucEsId].ucFbgId=FBM_FBG_ID_UNKNOWN;
	_rVdecImgInfo[ucEsId].fgSeqChanged=TRUE;
    _rVdecImgInfo[ucEsId].fgIsMPF=FALSE;
    _uMJPEG_SKIP_FRAME_CNT = 0;
    _VDEC_UnlockVld(ucEsId,prVdecEsInfo->u4VldId);
    return E_VDEC_OK;
}


// *********************************************************************
// Function    : VOID _VDEC_MJPEGSWRst
// Description : software reset procedure of vdec
// Parameter   : 
// Return      : 
// *********************************************************************
VOID _VDEC_MJPEGSWRst(UCHAR ucMpvId, UCHAR ucEsId)
{ 

#ifdef MJPEG_USE_NEW_FLOW
   VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
   ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
   prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
   if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
   {
	   return;
   }
   else if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_JPG || prVdecEsInfoKeep->eCurFMT==VDEC_FMT_PNG)
#endif
   { 
    if(IMG_GetDecState(0) == (UINT32)IMG_RELEASE)
    {
        IMG_Init_Ex(EXTERNAL_RING_IN_DOUBLE_OUT, NULL, ucEsId, 0, _Vdec_ImageDecCb);
    }  
   }
}

// *********************************************************************
// Function    : INT32 _VDEC_MJPEGDecode
// Description : Decode frame
// Parameter   : 
// Return      : OK: success
// *********************************************************************

INT32 _VDEC_MJPEGDecode(UCHAR ucMpvId, UCHAR ucEsId)
{
    INT32 i4DecRet=E_VDEC_OK;
#ifdef MJPEG_USE_NEW_FLOW
    VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
	{
        i4DecRet= _Vdec_MotionFrameDecode(ucMpvId,ucEsId);
	}
	else if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_JPG || prVdecEsInfoKeep->eCurFMT==VDEC_FMT_PNG)
#endif
	{
       i4DecRet= _Vdec_StillFrameDecode(ucMpvId,ucEsId);
	}
	return i4DecRet;
}

// *********************************************************************
// Function    : VOID _VDEC_MJPEGStop
// Description : stopping procedure of vdec
// Parameter   : 
// Return      : 
// *********************************************************************
VOID _VDEC_MJPEGStop(UCHAR ucEsId)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
	
    ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
	prVdecImgInfo=&_rVdecImgInfo[ucEsId];
    if (prVdecImgInfo->ucFbgId!= (UCHAR)FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(prVdecImgInfo->ucFbgId, (UINT32)20);    // block until display unlock   
	    FBM_ReleaseGroup(prVdecImgInfo->ucFbgId);
	    prVdecImgInfo->ucFbgId = (UCHAR)FBM_FBG_ID_UNKNOWN;
	    prVdecEsInfo->ucFbgId = (UCHAR)FBM_FBG_ID_UNKNOWN;
    }
	
#ifdef MJPEG_USE_NEW_FLOW
    {
		VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
		prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
	    if(prVdecEsInfoKeep->eCurFMT==VDEC_FMT_MJPEG)
	    {
		    MJPG_DecoderClose(prVdecImgInfo->hJDec);
            #ifdef CC_SUPPORT_MTK_ANDROID_RM
                    u1MJPGColorMode = 0xff;    
            #endif
	    }
    }
#endif
	return;
}


// *********************************************************************
// Function    : VOID _VDEC_MJPEGStop
// Description : Get the Pes Struct for using vdec
// Parameter   : 
// Return      : OK: success
// *********************************************************************
VOID _VDEC_MJPEGGetPesStruct(UCHAR ucEsId, void **ppvPesInfo)
{
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
	prVdecImgInfo=&_rVdecImgInfo[ucEsId];
    *ppvPesInfo = (VOID*)(&prVdecImgInfo->rPesInfo);
}

// *********************************************************************
// Function    : VOID _VDEC_MJPEGSetParam
// Description : Get param
// Parameter   :
// Return      :
// *********************************************************************
void _VDEC_MJPEGSetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
    VDEC_ES_INFO_T *prVdecEsInfo;
    UCHAR ucFbId;
    UINT32 u4DstYAddr, u4DstCAddr;
    FBM_PIC_HDR_T *prFbmPicHdrSrc;
    FBM_PIC_HDR_T *prFbmPicHdrDst;
    IMG_BUF_PARAM_T rCmd;
    IMG_REGION_T rOutputRegion = {0};
    VDEC_IMG_FRMDONE_DATA rCBData = {0};
    IMG_REGION_T rOutputDispRegion = {0};

    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);

    if (ucEsId >= VDEC_MAX_ES)
    {
        ASSERT(ucEsId < VDEC_MAX_ES);
        return;
    }

    ucFbId = FBM_GetEmptyFrameBuffer(prVdecEsInfo->ucFbgId, (UINT32)1500);
    if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
    {
        LOG(0, "get empty frame buffer failed.\n");        
        return;
    }
	
    FBM_UpdateReferenceList(prVdecEsInfo->ucFbgId, ucFbId, FALSE);

    //picture header
    prFbmPicHdrSrc = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, prVdecEsInfo->ucFbId);
    prFbmPicHdrDst = FBM_GetFrameBufferPicHdr(prVdecEsInfo->ucFbgId, ucFbId);
    if((prFbmPicHdrSrc == NULL) || (prFbmPicHdrDst == NULL))
    {
        LOG(0, "get pic hdr failed.\n");
        FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_EMPTY);
        return;
    }
    x_memcpy((void*)prFbmPicHdrDst, (void*)prFbmPicHdrSrc, sizeof(FBM_PIC_HDR_T));

    //rotation
    x_memset((void*)&rCmd, 0, sizeof(rCmd));
    rCmd.bKeepRatio = TRUE;
    rCmd.eDispColorMode = Y_CBCR420_SEP16;
    rCmd.eRotateDegree = (ENUM_IMG_ROTATE_PHASE)u4Param1;

    x_memcpy((void*)&rCmd.rSrcRegion, (void*)&_rDecOut, sizeof(_rDecOut));
    x_memcpy((void*)&rCmd.rDstRegion, (void*)&_rDecOut, sizeof(_rDecOut));
    FBM_GetFrameBufferAddr(prVdecEsInfo->ucFbgId, ucFbId, &u4DstYAddr, &u4DstCAddr);
    rCmd.rDstRegion.pu1Addr = (UINT8*)u4DstYAddr;
    rCmd.rDstRegion.u4Width = rCmd.rDstRegion.u4Pitch;
    rCmd.rDstRegion.u4Height = rCmd.rDstRegion.u4TotalLine;
    rCmd.rDstRegion.u4StartX = 0;
    rCmd.rDstRegion.u4StartY = 0;
    rCmd.prDstRegion = &rOutputRegion;

    IMG_Set(rCmd, &rOutputDispRegion);
    
    //display directly
    FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId, ucFbId, FBM_FB_STATUS_DISPLAYQ);

    rCBData.u2OriginalWidth = rCmd.prDstRegion->u4Width;
    rCBData.u2OriginalHeight = rCmd.prDstRegion->u4Height;
    rCBData.u2OutputWidth = MJPEG_MAX_WIDTH;
    rCBData.u2OutputHeight = MJPEG_MAX_HEIGHT;
    _VPUSH_ImgFrmDone(ucEsId, (void*)&rCBData);
}


#ifdef VDEC_TIME_PROFILE

// *********************************************************************
// Function    : VOID _VDEC_MJPEGGetParam
// Description : Get param
// Parameter   :
// Return      :
// *********************************************************************
VOID _VDEC_MJPEGGetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
#ifdef VDEC_TIME_PROFILE
    VDEC_IMG_INSTANCE_T *prVdecImgInfo = NULL;
    MJPEGOther *prOther;

    prVdecImgInfo = &_rVdecImgInfo[ucEsId];
    prOther = &(prVdecImgInfo->rOther);

    ASSERT(ucEsId < VDEC_MAX_ES);
    ASSERT(pu4Param1 != NULL);
    ASSERT(pu4Param2 != NULL);
    ASSERT(pu4Param3 != NULL);

    if (u4Type == (UINT32)MJPEG_TIME_PROFILE)
    {
        *pu4Param1 = prVdecImgInfo->ucFbgId;
        *pu4Param2 = prOther->u4CurDecMs;       //hw dec
    }
    else if (u4Type == (UINT32)MJPEG_TIME_PROFILE2)
    {
        *pu4Param2 = 2;            
        *pu4Param3 = 1;
    }
    else if (u4Type == (UINT32)MJPEG_TIME_PROFILE3)
    {
        *pu4Param2 = prOther->u4CurDecCycle;
        *pu4Param3 = prOther->u4CurDramCycle;
    }
    else
    {
        ////////
    }
#endif

    UNUSED(ucEsId);
    UNUSED(u4Type);
    UNUSED(pu4Param1);
    UNUSED(pu4Param2);
    UNUSED(pu4Param3);
}
#endif
#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL _VDEC_MJPEGIsPic(UCHAR ucEsId, VDEC_PES_INFO_T *prPes)
{
    UNUSED(ucEsId);
    UNUSED(prPes);
    return TRUE;
}
#endif

// *********************************************************************
// Function    : VOID _VDEC_MJPEGUpdDmxRptr
// Description : update dmx read pointer for using vdec
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_MJPEGUpdDmxRptr(UCHAR ucEsId)
{
    UINT32 u4CurRptr;
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
    ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
	prVdecImgInfo=&_rVdecImgInfo[ucEsId];    
    //u4CurRptr = _VDEC_VldMJPEGRPtr(&u4CurRptrBits);
    u4CurRptr=prVdecImgInfo->rPesInfo.u4VldReadPtr; //prVdecImgInfo->u4ReadPtr;
    //update dmx pointer
	//_DMX_GetVideoChannel(prVdecImgInfo->rPesInfo.ucEsId*3,&uChannelId);
    if (!VDEC_SetRptr(prVdecImgInfo->rPesInfo.ucEsId, u4CurRptr, u4CurRptr))                
    {                    
        //LOG(3, "MJPEG ES(%d) VDEC_SetRptr Fail 0\n", prVdecImgInfo->rPesInfo.ucEsId);                
		LOG(0, "MJPEG ES(%d) 0 DMX_UpdVRPtr Fail VS 0x%x, VE 0x%x, PicS 0x%x, Jdec Rptr 0x%x\n",
            prVdecImgInfo->rPesInfo.ucEsId,
            prVdecImgInfo->rPesInfo.u4FifoStart, prVdecImgInfo->rPesInfo.u4FifoEnd,
            prVdecImgInfo->rPesInfo.u4VldReadPtr, prVdecImgInfo->u4ReadPtr);	        
    }
}


// *********************************************************************
// Function    : VOID _VDEC_MJPEGSetDmxWptr
// Description : update dmx write pointer by vdec
// Parameter   : 
// Return      : 
// *********************************************************************
void _VDEC_MJPEGSetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
    ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
	prVdecImgInfo=&_rVdecImgInfo[ucEsId];
	prVdecImgInfo->u4WritePtr=u4Wptr;
    return;
}


// *********************************************************************
// Function    : VOID _VDEC_MJPEGEsFlwCtrl
// Description : skip one frame
// Parameter   : 
// Return      : 
// *********************************************************************
BOOL _VDEC_MJPEGEsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
    static UINT8 u1SkipCnt = 0;
    BOOL fgSkip = FALSE;
    VDEC_IMG_INSTANCE_T *prVdecImgInfo=NULL;
	VDEC_PES_INFO_T *prPesInfo;
    VDEC_ES_INFO_T* prVdecEsInfo;
    VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
    ASSERT(ucEsId < (UCHAR)MJPEG_MAX_ES_NUM);
	prVdecImgInfo=&_rVdecImgInfo[ucEsId]; 
	prPesInfo=&prVdecImgInfo->rPesInfo;
    prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);
    
    ASSERT(prVdecEsInfo!=NULL);
    ASSERT(prVdecEsInfoKeep!=NULL);
    
    if((prVdecEsInfo == NULL) || (prVdecEsInfoKeep == NULL))
    {
        return FALSE;
    }
        
    if (prVdecImgInfo->ucFbgId== (UCHAR)FBM_FBG_ID_UNKNOWN)
    {
        return FALSE;
    }

    if (prVdecEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    if( prVdecEsInfo->e3DType != VDEC_3D_DUAL_STREAM)
    {

		if(prVdecEsInfoKeep->fgVPush)
		{
			if(1) //(FBM_ChkFrameBufferFlag(prVdecImgInfo->ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC))
			{				
				switch(prVdecEsInfo->u4MMSpeed)
				{
					//case STC_SPEED_TYPE_FORWARD_2X:
					//	_uMJPEG_SKIP_FRAME = MJPEG_TRACK_SKIP;				
					case STC_SPEED_TYPE_FORWARD_4X:
						_uMJPEG_SKIP_FRAME = MJPEG_TRACK_SKIP -1;	
						break;
					case STC_SPEED_TYPE_FORWARD_8X:
						_uMJPEG_SKIP_FRAME = MJPEG_TRACK_SKIP*2 -1;
						break;
					case STC_SPEED_TYPE_FORWARD_16X:
						_uMJPEG_SKIP_FRAME = MJPEG_TRACK_SKIP*4 -1;				
						break;
					case STC_SPEED_TYPE_FORWARD_2X:						
					default :
						_uMJPEG_SKIP_FRAME = 0;				
						break;					

				}

				if(_uMJPEG_SKIP_FRAME>0)
				{
					LOG(5, "&&[%d][%d][%d][%d]\n", (prVdecEsInfo->u4MMSpeed), _uMJPEG_SKIP_FRAME, ucSkipMode, _uMJPEG_SKIP_FRAME_CNT);
					fgSkip = TRUE;
					if (u1SkipCnt > _uMJPEG_SKIP_FRAME)
					{
						fgSkip = FALSE;
						u1SkipCnt = 0;
					}
					if (fgSkip)
					{
						//update dmx
						if (!VDEC_SetRptr(prPesInfo->ucEsId,  prPesInfo->u4VldReadPtr, prPesInfo->u4VldReadPtr))
						{                    
							//LOG(3, "EsFlwCtrl MJPEG ES(%d) VDEC_SetRptr Fail 1\n", prPesInfo->ucEsId);                
					        LOG(3, "MJPEG ES(%d) 1 DMX_UpdVRPtr Fail VS 0x%x, VE 0x%x, PicS 0x%x, Jdec Rptr 0x%x\n",
					            prPesInfo->ucEsId,
					            prPesInfo->u4FifoStart, prPesInfo->u4FifoEnd,
					            prPesInfo->u4VldReadPtr, prVdecImgInfo->u4ReadPtr);							
						}

						FBM_ClrFrameBufferFlag(prVdecImgInfo->ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC);
						u1SkipCnt++;
						_uMJPEG_SKIP_FRAME_CNT++;
						return TRUE;
					}
				}
			}

		}
		else
		{
			if (FBM_ChkFrameBufferFlag(prVdecImgInfo->ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC) && (ucSkipMode != (UCHAR)IMode))
        {
            fgSkip = TRUE;
            if (u1SkipCnt > MJPEG_TRACK_SKIP)
            {
                fgSkip = FALSE;
                u1SkipCnt = 0;
            }
            if (fgSkip)
            {
                //update dmx
		  if (!VDEC_SetRptr(prPesInfo->ucEsId,  prPesInfo->u4VldReadPtr, prPesInfo->u4VldReadPtr))
                {                    
						//LOG(3, "EsFlwCtrl MJPEG ES(%d) VDEC_SetRptr Fail 2\n", prPesInfo->ucEsId);                
							LOG(0, "MJPEG ES(%d) 2 DMX_UpdVRPtr Fail VS 0x%x, VE 0x%x, PicS 0x%x, Jdec Rptr 0x%x\n",
					            prPesInfo->ucEsId,
					            prPesInfo->u4FifoStart, prPesInfo->u4FifoEnd,
					            prPesInfo->u4VldReadPtr, prVdecImgInfo->u4ReadPtr);							
                }

                FBM_ClrFrameBufferFlag(prVdecImgInfo->ucFbgId, (UINT32)FBM_FLAG_SKIP_PIC);
                u1SkipCnt++;
                    
                return TRUE;
            }
        }
    }
    }
    u1SkipCnt = 0;
    
    return FALSE;
}


// *********************************************************************
// Function    : BOOL _VDEC_IsSupportMJPEG
// Description : 
// Parameter   : 
// Return      : 
// *********************************************************************
BOOL _VDEC_IsSupportMJPEG(VOID)
{
    return TRUE;
}

