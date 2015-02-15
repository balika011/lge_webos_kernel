/*
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2006 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 */

#ifndef _hal_dvdec_h_
#define _hal_dvdec_h_


#include "hal_common.h"
#include "lxvdec.h"
#include "lxvo.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _HAL_VDEC_DIRECT_VideoInfo
{
	/* for backward compatibility */
	int structure_size;

	int width, height;
	enum
	{
		HAL_VDEC_DIRECT_PROGRESSIVE,
		HAL_VDEC_DIRECT_INTERLACED,
	} scan_type;
	/* actural framerate = framerate_num / framerate_den */
	int framerate_num, framerate_den;
	int par_w, par_h;
}HAL_VDEC_DIRECT_VideoInfo;

typedef struct _HAL_DVDEC_INTERNAL
{
	LXVDEC_OUTPUT_DEVICE de;
	lxvdec *dec;
	UINT32 fourcc;
	HAL_VDEC_DIRECT_VideoInfo info;
	lxvo_3d trid;
}HAL_DVDEC_INTERNAL;

typedef enum
{
	HAL_VDEC_DIRECT_3D_TYPE_NONE,	// 2D
	HAL_VDEC_DIRECT_3D_TYPE_SIDEBYSIDE,
	HAL_VDEC_DIRECT_3D_TYPE_TOPANDBOTTOM,

	HAL_VDEC_DIRECT_3D_TYPE_MAX,
} HAL_VDEC_DIRECT_3D_TYPE_T;

typedef struct HAL_DVDEC HAL_DVDEC_T;

HAL_DVDEC_T *HAL_VDEC_DIRECT_Open(unsigned int fourcc,
		int width, int height,
		HAL_VDEC_DIRECT_3D_TYPE_T trid_type);
DTV_STATUS_T HAL_VDEC_DIRECT_SetDEId(HAL_DVDEC_T *vdec, int deid);
DTV_STATUS_T HAL_VDEC_DIRECT_Play(HAL_DVDEC_T *vdec, void *data, int size);
DTV_STATUS_T HAL_VDEC_DIRECT_Stop(HAL_DVDEC_T *vdec);
DTV_STATUS_T HAL_VDEC_DIRECT_Close(HAL_DVDEC_T *vdec);
DTV_STATUS_T HAL_VDEC_DIRECT_GetVideoInfo(HAL_DVDEC_T *vdec,HAL_VDEC_DIRECT_VideoInfo *info);


#ifdef __cplusplus
}
#endif

#endif

