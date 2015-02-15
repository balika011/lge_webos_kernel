/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file	hal_pvr.h
 *
 *  Header file for PVR HAL
 *
 *  @author   Juyoung Jo (juyoung.jo@lge.com)
 *  @version  1.0
 *  @date     2013.03.14
 *
 *  @note      none
 *  @see
 */

#ifndef	_HAL_PVR_H_
#define	_HAL_PVR_H_

/*----------------------------------------------------------------------------------------
  File Inclusions
----------------------------------------------------------------------------------------*/
#include "pvr_common.h"

/*#include "common.h"*/
#if 0
#include "osa_api.h"

#include "dil_sdec.h"
#include "dil_vdec.h"
#include "hal_adec.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------------------
  Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/******************************/
/* Module Initialization APIs */
/******************************/

DTV_STATUS_T HAL_PVR_InitializeModule(void);

DTV_STATUS_T HAL_PVR_InitializeCrypto(void);

DTV_STATUS_T HAL_PVR_InitThumbnail(void);

DTV_STATUS_T HAL_PVR_SetTimeStampMode(PVR_TIMESTAMP_T timeStampMode);

#if 0
DTV_STATUS_T HAL_PVR_InitDownloadDevice(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_DeInitDownloadDevice(PVR_DOWN_CH_T ch);
#endif


/************************************/
/* Download(recording) related APIs */
/************************************/

DTV_STATUS_T HAL_PVR_OpenDownloadDevice(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_CloseDownloadDevice(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_SetDownloadIndexType(
		PVR_DOWN_CH_T downChId,
		PVR_PID_INFO_T downPID,
		PVR_DOWN_INDEX_TYPE_T idxType
		);

DTV_STATUS_T HAL_PVR_StartDownload(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_StopDownload(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_GetDownloadBufferInfo(PVR_DOWN_CH_T ch, UINT8 **ppBufAddr, UINT32 *bufSize);

DTV_STATUS_T HAL_PVR_GetDownloadBuffer(PVR_DOWN_CH_T ch,
		UINT8 **streamRdPtr, UINT8 **streamWrPtr,
		PVR_INDEX_T **indexPtr, UINT32 *indexSize
		);

DTV_STATUS_T HAL_PVR_ReturnDownloadBuffer(PVR_DOWN_CH_T ch, UINT8 *streamRdPtr, UINT8 *streamWrPtr);


/********************************/
/* Upload(playing) related APIs */
/********************************/

#if 0
DTV_STATUS_T HAL_PVR_InitUploadDevice(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_DeInitUploadDevice(PVR_DOWN_CH_T ch);
#endif

DTV_STATUS_T HAL_PVR_OpenUploadDevice(PVR_UP_CH_T ch);

DTV_STATUS_T HAL_PVR_CloseUploadDevice(PVR_DOWN_CH_T ch);

DTV_STATUS_T HAL_PVR_StartUpload(PVR_UP_CH_T ch);

DTV_STATUS_T HAL_PVR_StopUpload(PVR_UP_CH_T ch);

DTV_STATUS_T HAL_PVR_ResetUpload(PVR_UP_CH_T ch);

DTV_STATUS_T HAL_PVR_GetUploadBufferInfo(PVR_UP_CH_T ch, UINT8 **upBufAddr, UINT32 *upBufSize);

DTV_STATUS_T HAL_PVR_GetUploadBuffer(PVR_UP_CH_T ch, UINT8 **ppPushBuffer, UINT32 *pPushSize);

DTV_STATUS_T HAL_PVR_PushUploadBuffer(PVR_UP_CH_T ch, UINT8 *pPushBuffer, UINT32 pushSize);

DTV_STATUS_T HAL_PVR_SetUploadSpeed(PVR_UP_CH_T ch, SINT16 speed);


/***************************/
/* Encryption related APIs */
/***************************/

DTV_STATUS_T HAL_PVR_CRYPTO_SetEncryptionState(BOOLEAN encryptEnable);

DTV_STATUS_T HAL_PVR_CRYPTO_SetCipherAlgorithm(PVR_CIPHER_T cipherAlgorithm);

DTV_STATUS_T HAL_PVR_CRYPTO_GetSecureKey(UINT8 *pPath, UINT8 *pKey);

DTV_STATUS_T HAL_PVR_CRYPTO_SetCipherKeys(PVR_CRYPTO_TYPE_T cryptoInfo);

DTV_STATUS_T HAL_PVR_CRYPTO_GetCipherKeys(PVR_CIPHER_OPERATION_T cipherOperation, PVR_CRYPTO_TYPE_T *pCryptoInfo);

DTV_STATUS_T HAL_PVR_CRYPTO_EncryptData(UINT8 *pDst, UINT8 *pSrc, UINT32 size);

DTV_STATUS_T HAL_PVR_CRYPTO_DecryptData(UINT8 *pDst, UINT8 *pSrc, UINT32 size);

/************************************/
/* Thumbnail Exraction related APIs */
/************************************/
DTV_STATUS_T HAL_PVR_THUMBNAIL_GetThumnail(PVR_DOWN_CH_T ch, PVR_THUMBNAIL_T *pThumbnail);

DTV_STATUS_T HAL_PVR_THUMBNAIL_ExtractThumnail(PVR_PID_INFO_T pidInfo, UINT8 *pBuffer, UINT32 size, PVR_THUMBNAIL_T *pThumData);
DTV_STATUS_T HAL_PVR_THUMBNAIL_ExtractJPEGThumnail(PVR_PID_INFO_T pidInfo, UINT8 *pBuffer, UINT32 size, PVR_THUMBNAIL_T *pThumData);
DTV_STATUS_T HAL_PVR_THUMBNAIL_ExtractJPEGThumbnail(PVR_PID_INFO_T pidInfo, UINT8 *pBuffer, UINT32 size, PVR_THUMBNAIL_T *pThumData, UINT32 timeout);


BOOLEAN HAL_PVR_IsUploading(PVR_UP_CH_T upChId);
BOOLEAN HAL_PVR_IsDownloading(UINT8 channel);

DTV_STATUS_T HAL_PVR_GetUploadBufferOccupiedSize(PVR_UP_CH_T ch, UINT32 *pOccupiedSize);

/*----------------------------------------------------------------------------------------
  Extern Variables
----------------------------------------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif

