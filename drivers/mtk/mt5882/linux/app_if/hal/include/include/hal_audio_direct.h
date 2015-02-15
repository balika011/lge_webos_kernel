/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_audio_direct.h
 *
 *  HAL Audio Direct 함수 header 파일.
 *
 *
 *  @author		Minho Hwang(minho.hwang@lge.com)
 *  @reviser	Jong-Sang Oh(jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2013.10.21
 *  @note
 *  @see		hal_audio_direct.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_AUDIO_DIRECT_H_
#define _HAL_AUDIO_DIRECT_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#include "hal_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/


/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/
/* LOGM Debug Message File Descriptor */
extern SINT32 gHalAudDirLogmFd;


/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
/**
 * HAL AUDIO DIRECT Codec Type.
 *
 */
typedef  enum
{
	HAL_AUDIO_DIRECT_CODEC_UNKNOWN,			///< 0
	HAL_AUDIO_DIRECT_CODEC_PCM,				///< 1
	HAL_AUDIO_DIRECT_CODEC_AC3,				///< 2(AC3, E-AC3(DD+))
	HAL_AUDIO_DIRECT_CODEC_AAC,				///< 3(AAC, HE-AAC)
	HAL_AUDIO_DIRECT_CODEC_DRA,				///< 4
	HAL_AUDIO_DIRECT_CODEC_MP3,				///< 5
	HAL_AUDIO_DIRECT_CODEC_WMA_PRO,			///< 6
	HAL_AUDIO_DIRECT_CODEC_DTS,				///< 7
	HAL_AUDIO_DIRECT_CODEC_VORBIS,			///< 8
	HAL_AUDIO_DIRECT_CODEC_AMR_WB,			///< 9
	HAL_AUDIO_DIRECT_CODEC_AMR_NB,			///< 10
	HAL_AUDIO_DIRECT_CODEC_ADPCM,			///< 11
	HAL_AUDIO_DIRECT_CODEC_RA8,				///< 12
	HAL_AUDIO_DIRECT_CODEC_FLAC,			///< 13
	HAL_AUDIO_DIRECT_CODEC_MAX,				///< Max. Value
} HAL_AUDIO_DIRECT_CODEC_TYPE_T;


/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
/**
 *  AUDIO Handler of Direct Player (Cloud Game)
 *
 */
typedef void HAL_AUDIO_DIRECT_CONTEXT;

/**
 * HAL AUDIO DIRECT Index.
 *
 */
typedef  enum
{
	HAL_AUDIO_DIRECT_ADEC0		= 0,	/* Decoder 0			*/
	HAL_AUDIO_DIRECT_ADEC1		= 1,	/* Decoder 1			*/
	HAL_AUDIO_DIRECT_MIXER0		= 2,	/* G-Streamer Mixer 0	*/
	HAL_AUDIO_DIRECT_MIXER1		= 3,	/* G-Streamer Mixer 1	*/
	HAL_AUDIO_DIRECT_MIXER2		= 4,	/* G-Streamer Mixer 2	*/
	HAL_AUDIO_DIRECT_MIXER3		= 5,	/* G-Streamer Mixer 3	*/
	HAL_AUDIO_DIRECT_MIXER4		= 6,	/* G-Streamer Mixer 4	*/
	HAL_AUDIO_DIRECT_MIXER5		= 7,	/* HAL AUDIO Mixer 0	*/
	HAL_AUDIO_DIRECT_MIXER6		= 8,	/* ALSA AUDIO Mixer 0	*/
	HAL_AUDIO_DIRECT_MIXER7		= 9,	/* ALSA AUDIO Mixer 1	*/
	HAL_AUDIO_DIRECT_INDEX_MAX	= HAL_AUDIO_DIRECT_MIXER7,
} HAL_AUDIO_DIRECT_INDEX_T;

/* Direct Clip Play for Cloud Game */
HAL_AUDIO_DIRECT_CONTEXT *HAL_AUDIO_DIRECT_Open(HAL_AUDIO_DIRECT_INDEX_T audioIndex);
DTV_STATUS_T HAL_AUDIO_DIRECT_Close(HAL_AUDIO_DIRECT_CONTEXT *pCtx);
DTV_STATUS_T HAL_AUDIO_DIRECT_Write(HAL_AUDIO_DIRECT_CONTEXT *pCtx, void *pBuff, UINT32 bufSize);
DTV_STATUS_T HAL_AUDIO_DIRECT_Stop(HAL_AUDIO_DIRECT_CONTEXT *pCtx);
DTV_STATUS_T HAL_AUDIO_DIRECT_Pause(HAL_AUDIO_DIRECT_CONTEXT *pCtx);
DTV_STATUS_T HAL_AUDIO_DIRECT_Resume(HAL_AUDIO_DIRECT_CONTEXT *pCtx);
DTV_STATUS_T HAL_AUDIO_DIRECT_Flush(HAL_AUDIO_DIRECT_CONTEXT *pCtx);
DTV_STATUS_T HAL_AUDIO_DIRECT_GetBufferInfo(HAL_AUDIO_DIRECT_CONTEXT *pCtx, UINT32 *pMaxBufSize, UINT32 *pFreeBufSize);
DTV_STATUS_T HAL_AUDIO_DIRECT_GetRawBufferInfo(HAL_AUDIO_DIRECT_CONTEXT *pCtx, UINT32 *pMaxBufSize, UINT32 *pFreeBufSize);
DTV_STATUS_T HAL_AUDIO_DIRECT_SetNoDelayParam(HAL_AUDIO_DIRECT_CONTEXT *pCtx, UINT32 bOnOff, UINT32 upperThreshold, UINT32 lowerThreshold);
DTV_STATUS_T HAL_AUDIO_DIRECT_Start(HAL_AUDIO_DIRECT_CONTEXT *pCtx, HAL_AUDIO_DIRECT_CODEC_TYPE_T audioType, UINT32 samplingFreq, UINT32 numOfChannel, UINT32 bitsPerSample);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_AUDIO_DIRECT_H_ */


