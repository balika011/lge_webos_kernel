/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_pvr_util.h
 *
 *  Header file for utilities of PVR DIL
 *
 *  @author		Jihoon Lee ( gaius.lee@lge.com)
 *  @version		1.0
 *  @date		2010-05-10
 *  @note		Additional information.
 */

#ifndef _PVR_UTIL_H_
#define _PVR_UTIL_H_

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pvr_common.h"

#include "pvr_kapi.h"
#include "pvr_kadp.h"
//#include "hal_pvr_types.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define USE_PVR_CACHED_BUFFER


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PVR_COLOR_NONE		"\x1b[0m"
#define PVR_COLOR_RED		"\x1b[1;31m"
#define PVR_COLOR_GREEN		"\x1b[1;32m"
#define PVR_COLOR_YELLOW	"\x1b[1;33m"
#define PVR_COLOR_BLUE		"\x1b[1;34m"
#define PVR_COLOR_PURPLE	"\x1b[1;35m"
#define PVR_COLOR_CYAN		"\x1b[1;36m"

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define HAL_PVR_MAX_INDEX	1024
#define HAL_PVR_CHUNK_SIZE	(384*1024)

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
	UINT32		start;
	UINT32		end;
} LX_PVR_UTIL_MEM_T;

typedef struct
{
	LX_PVR_UTIL_MEM_T	paddr;
	LX_PVR_UTIL_MEM_T	vaddr;
	UINT32				size;
} LX_PVR_UTIL_BUF_T;


typedef struct
{
	LX_PVR_CH_T			ch;

	LX_PVR_UTIL_BUF_T	buf;
	void				*rptr;
	void				*wptr;

	LX_PVR_PIE_TYPE_T	pieType;
	UINT16				pid;
	LX_PVR_INDEX_T		indexBuffer[HAL_PVR_MAX_INDEX];
} LX_PVR_DN_CTRL_T;

typedef struct
{
	LX_PVR_CH_T 		ch;

	LX_PVR_UTIL_BUF_T	buf;
	void				*rptr;
	void				*wptr;

	UINT8				chunk[HAL_PVR_CHUNK_SIZE];
} LX_PVR_UP_CTRL_T;


typedef struct
{

	BOOLEAN				bUploadSEDecryptionMode;

	LX_PVR_HW_INFO_T	hwCfg;

	LX_PVR_UP_CTRL_T	upCtrl[LX_PVR_CH_NUM];
	LX_PVR_DN_CTRL_T	dnCtrl[LX_PVR_CH_NUM];
} LX_PVR_CTRL_T, *P_LX_PVR_CTRL_T;


/**
 * PVR module device
 * Each device represent H/W inside PVR module.
 * Device A is linked to SDEC Channel A. Device B is same.
 *
 */
typedef enum
{
	LX_PVR_BUFFER_TYPE_DN,											/**< information of download buffer */
	LX_PVR_BUFFER_TYPE_UP,											/**< information of upload buffer */
	LX_PVR_BUFFER_TYPE_MAX
} LX_PVR_BUFFER_TYPE_T ;


/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/
DTV_STATUS_T LXPVR_Init(void);
void* LXPVR_Va2Pa(void* pAddrVirt);
void* LXPVR_Pa2Va(void* pAddrPhys);

DTV_STATUS_T LXPVR_GetBufferInfo(LX_PVR_CH_T lxch, LX_PVR_BUFFER_TYPE_T type, UINT32 *paddr, UINT32 *size);

BOOLEAN LXPVR_IsDownloadBuffer(void *vaddr);


P_LX_PVR_CTRL_T		LXPVR_GetCtrl(void);
LX_PVR_DN_CTRL_T*	LXPVR_GetDNCtrl(LX_PVR_CH_T lxch);
LX_PVR_UP_CTRL_T*	LXPVR_GetUPCtrl(LX_PVR_CH_T lxch);

void LXPVR_SetUseSEDecryptionMode(BOOLEAN onoff);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PVR_UTIL_H_ */

