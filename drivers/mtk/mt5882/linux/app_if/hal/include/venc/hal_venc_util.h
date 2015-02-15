/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file		hal_venc_util.h
 *
 *  Header file for venc_util.
 *
 *  @author		Jaeseop So (jaeseop.so@lge.com)
 *  @version  	1.0
 *  @date     	2011.05.31
 *  @note		Additional information.
 *  @see		hal_venc.h
 */

#ifndef PIONEER_PLATFORM
#include "hal_venc.h"
#endif

#ifndef _VENC_UTIL_H_
#define _VENC_UTIL_H_
/*-----------------------------------------------------------------------------
	Control Constants
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    File Inclusions
-----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/*-----------------------------------------------------------------------------
	Constant Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Macro Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    Type Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Function Prototype Declaration 
-----------------------------------------------------------------------------*/

extern DTV_STATUS_T VENC_CreateTask(LX_VENC_HANDLE_T *pVencHandle);
extern DTV_STATUS_T VENC_DestroyTask(LX_VENC_HANDLE_T *pVencHandle);

extern DTV_STATUS_T VENC_ESDump_Start(LX_VENC_HANDLE_T *pVencHandle);
extern DTV_STATUS_T VENC_ESDump_Stop(LX_VENC_HANDLE_T *pVencHandle);
#ifdef SUPPORT_VENC_DEVICE_FASYNC_FOPS
extern DTV_STATUS_T VENC_RegEncoderCallback(UINT8 port, pfnVENCDataHandlingCB pfnCallBack, LX_VENC_HANDLE_T *pVencHandle);
#else
extern DTV_STATUS_T VENC_RegEncoderCallback(UINT8 port, pfnVENCDataHandlingCB pfnCallBack);
#endif
extern DTV_STATUS_T VENC_InterpretFramerate(VENC_FRAMERATE_T nVENC_FrameRate, UINT32 *nFrameRate, UINT8 *nScanType);
extern DTV_STATUS_T VENC_CopyVideoESFrame(LX_VENC_HANDLE_T *pVencHandle);

/*-----------------------------------------------------------------------------
	Extern Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Includes
-----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif


#endif


//EOF
