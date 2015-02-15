/******************************************************************************
 *   TV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2013 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file		hal_svp.h
 *
 *  Hardware Abstration Layer for DRM
 *
 *  @author
 *  @version	1.0
 *  @date		2014.07.
 *  @note
 *  @see
 */

/******************************************************************************
    Header File Guarder
******************************************************************************/
#ifndef	_HAL_SVP_H_
#define	_HAL_SVP_H_

/******************************************************************************
    Control Constants
******************************************************************************/

/******************************************************************************
    File Inclusions
******************************************************************************/
#include "hal_common.h"

/******************************************************************************
    Constant Definitions
******************************************************************************/

/******************************************************************************
    Macro Definitions
******************************************************************************/

/******************************************************************************
    Type Definitions
******************************************************************************/
typedef enum {
	HAL_SVP_REFORMATTER_NONE	= 0,
	HAL_SVP_REFORMATTER_AVC 	= 1
} SVP_REFORMATTER_T;

typedef enum {
	HAL_SVP_ERROR	= -1,
	HAL_SVP_OK		= 0,
	HAL_SVP_NO_AU	= 1,
	HAL_SVP_WAIT	= 2
} SVP_STATUS_T;

/******************************************************************************
    Function Declaration
******************************************************************************/

SVP_STATUS_T HAL_SVP_OpenSecureCpb(unsigned int secCpbAddr, int secCpbSize);
SVP_STATUS_T HAL_SVP_CloseSecureCpb(void);
SVP_STATUS_T HAL_SVP_SetSecureReformatter(SVP_REFORMATTER_T reformatterType);
SVP_STATUS_T HAL_SVP_GetSecureCpbInformation (unsigned long vdecHandle,
	unsigned long *pSecCpbAddr, int *pSecCpbSize, int *pReadOffset, int *pWriteOffset);
SVP_STATUS_T HAL_SVP_GetSecureDecryptBufferHandle(unsigned int *pSecDecBufHandle, unsigned int *pSecDecBufSize);
SVP_STATUS_T HAL_SVP_GetAuInformation(unsigned int *pAuWritePoint, int *pAuWriteLength,
	unsigned int *pWritePoint, unsigned int readPoint, unsigned int secDecBufHandle, unsigned int srcLength);
SVP_STATUS_T HAL_SVP_FlushSecureDecryptBuffer(void);

SVP_STATUS_T HAL_SVP_Widevine_DecryptVideo(unsigned char *pIv, unsigned char *pSrc, unsigned int srcLength,
	unsigned int secDecBufHandle, unsigned int secDecBufwriteOffset, unsigned int *pOutputLength);
SVP_STATUS_T HAL_SVP_Widevine_DecryptAudio(unsigned char *pIv, unsigned char *pSrc, unsigned int srcLength,
	unsigned char *pOutput, unsigned int *pOutputLength);

SVP_STATUS_T HAL_SVP_CopyToSecureDecryptBuffer(unsigned int secDecBufHandle, unsigned int secDecBufwriteOffset,
	unsigned char *pSrc, int length);
SVP_STATUS_T HAL_SVP_CopyFromSecureBuffer(unsigned int srcHandle, unsigned char *pOutput, int length);

#endif /* _HAL_SVP_H_ */

