/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file   hal_pvr_debug.h
 *
 *  Type definitions for PVR device driver debug routine
 *
 *  @author   Juyoung Jo (juyoung.jo@lge.com)
 *  @version  0.5
 *  @date     2013.04.23
 *
 *  @note      none
 *  @see
 */

#ifndef _HAL_PVR_DEBUG_H_
#define _HAL_PVR_DEBUG_H_


/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pvr_common.h"
#include "logm_kadp.h"
#include "hal_pvr_util.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PVR_FORCED_PRINTF(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_ERROR, fmt, ##args)
#define PVR_WARN_PRINTF(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define PVR_DBG_PRINTF(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_INFO, fmt, ##args)

#define PVR_HAL_ERROR(fmt, args...)				KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_ERROR, fmt, ##args)
#define PVR_HAL_WARNING(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define PVR_HAL_NOTI(fmt, args...)				KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_NOTI, fmt, ##args)
#define PVR_HAL_INFO(fmt, args...)				KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_INFO, fmt, ##args)
#define PVR_HAL_DEBUG(fmt, args...)				KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_DEBUG, fmt, ##args)
#define PVR_HAL_TRACE(fmt, args...)				KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE, fmt, ##args)
#define PVR_HAL_DOWN_LOG(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE + 1, fmt, ##args)
#define PVR_HAL_INDEX_LOG(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE + 2, fmt, ##args)
#define PVR_HAL_UP_LOG(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE + 3, fmt, ##args)
#define PVR_HAL_CRYPTO_LOG(fmt, args...)		KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE + 4, fmt, ##args)
#define PVR_HAL_THUMBNAIL(fmt, args...)			KADP_LOGM_PRINT(g_pvr_hal_logm_fd, LX_LOGM_LEVEL_TRACE + 5, fmt, ##args)

#define PVR_HAL_CHECK_CODE(__checker,__if_action,fmt,args...)   \
												__CHECK_IF_ERROR(__checker, PVR_HAL_ERROR, __if_action , fmt, ##args )


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum
{
	THUMBNAIL_DATA_TS = 0,
	THUMBNAIL_DATA_ES,
	THUMBNAIL_DATA_YUV,
	THUMBNAIL_DATA_RGB,
	THUMBNAIL_DATA_JPG,
	THUMBNAIL_DATA_MAX
} PVR_THUMBNAIL_DATA_TYPE_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int HAL_PVR_DBG_SaveThumbnailData(PVR_THUMBNAIL_DATA_TYPE_T type, const void* data, UINT32 size);
int HAL_PVR_DBG_SaveYUVThumbnailData(const void* y, const void* cb, int width, int height);

int HAL_PVR_DBG_SaveUploadData(const void* data, UINT32 size);

int HAL_PVR_DBG_CheckDownloadBuffer(LX_PVR_DN_CTRL_T *ctrl, UINT8* ts, UINT32 tsSize,
									LX_PVR_INDEX_T *index, UINT32 indexCount);


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern int g_pvr_hal_logm_fd;


#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif	/* _HAL_PVR_DEBUG_H_ */

