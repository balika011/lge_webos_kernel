/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file	hal_pvr_downdev.h
 *
 *  Header file for PVR down device.
 *
 *  @author   Min Hyoung Kim (a9a9@lge.com)
 *  @version  0.5
 *  @date     2008.02.26
 *
 *  @note      none
 *  @see
 */

#ifndef	_HAL_PVR_DOWNDEV_DD_H_
#define	_HAL_PVR_DOWNDEV_DD_H_

#include "pvr_common.h"

DTV_STATUS_T LXPVR_SetDownloadTimestamp(PVR_TIMESTAMP_T timeStampMode);

DTV_STATUS_T LXPVR_GetDownloadBufferInfo(PVR_DOWN_CH_T ch, void **ppUiPhysAddr, UINT32 *pUiBufSize);

DTV_STATUS_T LXPVR_SetDownloadBuffer(PVR_DOWN_CH_T downChId);

DTV_STATUS_T LXPVR_SetDownloadIndexType(
		PVR_DOWN_CH_T downChId,
		PVR_PID_INFO_T downPID,
		PVR_DOWN_INDEX_TYPE_T idxType
		);

DTV_STATUS_T LXPVR_StartDownload(PVR_DOWN_CH_T ch);

DTV_STATUS_T LXPVR_StopDownload(PVR_DOWN_CH_T ch);

DTV_STATUS_T LXPVR_GetDownloadBuffer(PVR_DOWN_CH_T ch,
		void **streamRdPtr, void **streamWrPtr,
        void **indexPtr, UINT32 *indexSize
		);

DTV_STATUS_T LXPVR_ReturnDownloadBuffer(PVR_DOWN_CH_T ch, void *streamRdPtr, void *streamWrPtr);

#endif		/* _HAL_PVR_DOWNDEV_DD_H_ */

