/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file	hal_pvr_updev.h
 *
 *  Header file for PVR HAL upload device.
 *
 *  @author   Juyoung Jo (juyoung.jo@lge.com)
 *  @version  0.5
 *  @date     2013.04.23
 *
 *  @note      none
 *  @see
 */

#ifndef	_HAL_PVR_UPDEV_DD_H_
#define	_HAL_PVR_UPDEV_DD_H_

#include "pvr_common.h"


DTV_STATUS_T LXPVR_SetUploadTimestamp(PVR_TIMESTAMP_T timeStampMode);

DTV_STATUS_T LXPVR_GetUploadBufferInfo(PVR_UP_CH_T ch, UINT8 **ppBufAddr, UINT32 *pBufSize);

DTV_STATUS_T LXPVR_OpenUploadDevice(PVR_UP_CH_T ch);

DTV_STATUS_T LXPVR_StartUpload(PVR_UP_CH_T ch);

DTV_STATUS_T LXPVR_StopUpload(PVR_UP_CH_T ch);

DTV_STATUS_T LXPVR_ResetUpload(PVR_UP_CH_T ch);

DTV_STATUS_T LXPVR_GetUploadBuffer(PVR_UP_CH_T ch, UINT8 **ppPushBuffer, UINT32 *pPushSize);

DTV_STATUS_T LXPVR_PushUploadBuffer(PVR_UP_CH_T ch, UINT8 *pPushBuffer, UINT32 pushSize);

DTV_STATUS_T LXPVR_SetUploadSpeed(PVR_UP_CH_T upChId, SINT16 speed);


#endif		/* _HAL_PVR_UPDEV_DD_H_ */

