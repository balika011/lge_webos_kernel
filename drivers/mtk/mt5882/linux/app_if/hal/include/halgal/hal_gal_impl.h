/*****************************************************************************************
 * SIC R&D LAB, LG ELECTRONICS INC., SEOUL, KOREA
 * COPYRIGHT(c) 2011,2012 by LG Electronics Inc.
 *
 * All rights reserved. No part of this work covered by this copyright hereon
 * may be reproduced, stored in a retrieval system, in any form
 * or by any means, electronic, mechanical, photocopying, recording
 * or otherwise, without the prior written  permission of LG Electronics.
 ****************************************************************************************/

/** @file
 *
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author     raxis.lim
 *  @version    1.0
 *  @date       2012-04-21
 *  @note       Additional information.
 */

#ifndef	_HAL_GAL_IMPL_H_
#define	_HAL_GAL_IMPL_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define __USED__	__attribute__ ((used))      /* GNU C extention */

/** if defined, GX will use full multi process safe surface management
 *	otherwise,  GX will use single process safe surface management
 *
 *	@note DO NOT change it
 */
#define  GX_USE_MULTI_PROCESS_SAFE_SURFACE

/** if defined, GX will use palette data stored in the kdriver surface
 *	otherwise,  GX will use private palette data store in GX surface info
 */
#define	 GX_USE_KDRV_PALETTE

/** GX will use H/W image decoder */
#define	GX_USE_HW_PNG_DECODER
#define	GX_USE_HW_JPG_DECODER

/** if defined, GX will satisfy some NC4.0 runtime behaviour
 *	Actually, I don't know why some NC4 workaorunds are working :)
 **/
#define	GX_USE_NC4_SPEC

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "osa_kadp.h"
#include "debug_kadp.h"
#include "gal_kadp.h"
#include "fbdev_kadp.h"
#include "png_kadp.h"
#include "mjpeg_kadp.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define GX_GFX_POSD_MAX			2					/* num of graphic plane = POSD0, POSD1 */
#define	GX_POSD_MAX				(GX_GFX_POSD_MAX+1)	/* num of total osd plane = POSD0, POSD1, POSD3(cursor) */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define GX_ERROR(fmt, args...)    KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_ERROR, fmt, ##args)
#define GX_WARN(fmt, args...)     KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define GX_NOTI(fmt, args...)     KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_NOTI, fmt, ##args)
#define GX_INFO(fmt, args...)     KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_INFO, fmt, ##args)
#define GX_DEBUG(fmt, args...)    KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_DEBUG, fmt, ##args)
#define GX_TRACE(fmt, args...)    KADP_LOGM_PRINT(g_gx_logm_fd, LX_LOGM_LEVEL_TRACE, fmt, ##args)

extern int g_gx_logm_fd;

#define	GX_CHECK_ERROR(__checker,__if_action,fmt,args...)	\
			__CHECK_IF_ERROR(__checker, GX_WARN, __if_action, fmt, ##args )
#define	GX_CHECK_DEBUG(__checker,__if_action,fmt,args...)	\
			__CHECK_IF_ERROR(__checker, GX_DEBUG, __if_action, fmt, ##args )

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/** shared memory for HAL_GAL layer
 *
 *
 *
 */
typedef struct
{
#define GX_SHMEM_MAGIC			0x4758474c		/* 'g','x','g','l' */
	UINT32		magic;

#define	GX_STATUS_NONE			0x00000000
#define	GX_STATUS_FB_INIT_DONE	0x00000001		/* FB device initialized */
#define	GX_STATUS_GFX_INIT_DONE	0x00000002		/* GFX device initialized */
	UINT32		gx_status;

	struct
	{
		UINT32	tid;							/* task is is needed to check offset more accurately */
		UINT32	offset;							/* GAL layer is using offset as unique value */
	}
	gal_surf_list[2000];

	struct
	{
		BOOLEAN	b_active;
		HAL_GAL_SURFACE_INFO_T	surf_info;
	}
	fbSurfList[GX_GFX_POSD_MAX];
}
GX_SHMEM_T;

/** brief information about Image
 *
 */
typedef struct
{
    int     width;
    int     height;
    int     bit_depth;
    int     byte_per_pixel;
//	BOOLEAN b_progressive_format;

	UINT32	output_image_size;				/* estimated decoded image size */
}
GX_IMAGE_HDR_INFO_T;

typedef enum
{
	/** query LX_GAL_SURFACE_T with HAL_GAL_SURFACE_INFO_T
	 *
	 *	args  = pointer to HAL_GAL_SURFACE_INFO_T
	 *	return= (allocated) pointer to LX_GAL_SURFACE_T
	 */
	GX_QUERY_TYPE_GAL_SURFACE,
}
GX_QUERY_TYPE_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int 				GX_CheckImageHdr	( HAL_GAL_IMAGE_FORMAT_T imgFmt, void* pData, int nDataLength, GX_IMAGE_HDR_INFO_T* pInfo );

/** search(query) GFX surface from HAL_GAL_SURFACE_INFO_T
 *
 *	@note LX_GAL_SURFACE_T will be allocated, so you should call GX_FreeQueryData
 */
void*				GX_QueryData		( GX_QUERY_TYPE_T query_type, void* args );
void				GX_FreeQueryData	( GX_QUERY_TYPE_T query_type, void* data );

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int	g_gx_log_level;	/* debug only */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PROTOS_H_ */

