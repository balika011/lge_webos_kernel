/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file	hal_pvr_thumbnail.h
 *
 *  Header file for PVR thumbnail.
 *
 *  @author   Jihoon Lee ( gaius.lee@lge.com)
 *  @version  1.0
 *  @date     2010-05-11
 *
 *  @note      none
 *  @see
 */

#ifndef	_PVR_THUMBNAIL_DD_H_
#define	_PVR_THUMBNAIL_DD_H_

#include "pvr_common.h"

#include "pvr_kadp.h"
#include "pvr_kapi.h"
#include "sys/mman.h"

#include "venc_kadp.h"
#include "venc_kapi.h"
#include "vdec_kadp.h"
#include "sdec_util.h"


#define	PVR_THUMBNAIL_WIDTH								(240)
#define	PVR_THUMBNAIL_HEIGHT							(136)
#define PVR_THUMBNAIL_STRIDE							(PVR_THUMBNAIL_WIDTH * sizeof(UINT32))

#define _PVR_DEFAULT_JPEG_THUMBNAIL_XSIZE				(1920)
#define _PVR_DEFAULT_JPEG_THUMBNAIL_YSIZE				(1080)

#define PVR_THUMBNAIL_VDEC0								(0)
#define PVR_THUMBNAIL_VDEC1								(2)

#define LX_PVR_THUMB_TOTAL_WAIT_TIME					5000	/* 5 second */
#define LX_PVR_THUMB_VDEC_WAIT_TIME						100		/* 100 ms */
#define LX_PVR_THUMB_VDEC_WAIT_COUNT					(LX_PVR_THUMB_TOTAL_WAIT_TIME /	LX_PVR_THUMB_VDEC_WAIT_TIME)

#define PVR_MJPEG_ENCODER_ID							(0)

/**
 * Thumbnail data status
 *
 */
typedef enum
{
	LX_PVR_THUMBNAIL_STATE_IDLE,										/**< There is no thumbnail processing */
	LX_PVR_THUMBNAIL_STATE_PROCESSING,									/**< There is thumbnail processing, please wait */
	LX_PVR_THUMBNAIL_STATE_DATAREADY,									/**< There is waiting thumbnail data, please get it! */
	LX_PVR_THUMBNAIL_STATE_ERROR,										/**< There is error in thumbnail processing */
} LX_PVR_THUMBNAIL_STATE_T ;

/**
 * Thumbnail Output Format
 */
typedef enum
{
	LX_PVR_THUMBNAIL_FORMAT_ARGB,									/**< ARGB8888 */
	LX_PVR_THUMBNAIL_FORMAT_JPEG,									/**< JPEG Image */
} LX_PVR_THUMBNAIL_OUTFORMAT_T ;

/**
 * PVR Thumbnail Control
 */
typedef struct
{
	LX_PVR_THUMBNAIL_STATE_T		thumbnailState;						/**< State of Thumbnail */

	UINT8							*pScaledBuffer;						/**< Buffer pointer for scaled thumbnail data from venc */
	UINT8							*pConvertedBuffer;					/**< Buffer pointer for thumbnail data which converted from YUV to ARGB */
	UINT32							uiScaledBufferSize;					/**< Buffer size of scaled thumbnail data from venc */
	UINT32							uiConvertedBufferSize;				/**< Buffer size of thumbnail data which converted from YUV to ARGB */

	UINT32							uiPictureWidth;						/**< Width of resized thumbnail picture */
	UINT32							uiPictureHeight;					/**< Height of resized thumbnail picture */
	UINT32							uiPictureStride;					/**< Stride of resized thumbnail picture */

	//LX_VDEC_CTX_T					*pVdecCtx;							/**< Context of VDEC. using for decoding I-Frame */
	VDEC_CODEC_T					videoCodecType;						/**< Video Codec type */

	SINT32							memFd;								/**< memory fd for memory mapping */
	UINT32							uiVENCMmapAddr;						/**< Address of Memory mapped VENC Memory*/
	UINT32							uiVENCMmapSize;						/**< Size of Memory mapped VENC Memory*/
} LX_PVR_THUMBNAIL_CTRL_T, *P_LX_PVR_THUMBNAIL_CTRL_T;


DTV_STATUS_T LXPVR_Thumbnail_Init(void);
DTV_STATUS_T LXPVR_GetThumbnailSize(UINT32 *puiWidth, UINT32 *puiHeight);
DTV_STATUS_T LXPVR_CloseThumbMemDevice(void);
DTV_STATUS_T LXPVR_RetreiveThumbnail(PVR_DOWN_CH_T downChId);
DTV_STATUS_T LXPVR_GetThumbnail(UINT32 *pImagePtr, UINT32 *pImageSize);
LX_PVR_THUMBNAIL_STATE_T LXPVR_GetThumbnailStatus(void);
void LXPVR_SetVdecCodec(VDEC_CODEC_T videoCodecType);
DTV_STATUS_T LXPVR_ExtractThumbnail(PVR_PID_INFO_T *pidInfo, UINT8 *pBuffer, UINT32 size,
								PVR_THUMBNAIL_T *pThumData, LX_PVR_THUMBNAIL_OUTFORMAT_T out_format);
DTV_STATUS_T LXPVR_ExtractThumbnailWithTimeout(PVR_PID_INFO_T *pidInfo, UINT8 *pBuffer, UINT32 size,
								PVR_THUMBNAIL_T *pThumData, LX_PVR_THUMBNAIL_OUTFORMAT_T out_format, UINT32 timeout);

#endif

