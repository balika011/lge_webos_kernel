/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_audio_drv.h
 *
 *  AUDIO HAL 함수 header 파일.
 *
 *  @author		Jong-Sang Oh(jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2013.05.10
 *  @see
 *
 *  @addtogroup lg1154_adec
 *	@{
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_AUDIO_DRV_H_
#define _HAL_AUDIO_DRV_H_


/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#include "osa_kadp.h"
#include "debug_kadp.h"

#include "audio_kadp_master.h"
#include "audio_kadp_decoder.h"
#include "audio_kadp_renderer.h"
#include "audio_kadp_encoder.h"
#include "audio_kadp_capturer.h"
#include "aad_kadp.h"
#include "hdmi_kadp.h"
#include "hdmi20_kadp.h"
#include "uvd_kadp.h"	//For 4K TP UDTV

#include "hal_audio.h"

//define for SCART HW Bypass Mode
#ifndef PIONEER_PLATFORM			//webOS platform
//#define DISABLE_SCART_HW_BYPASS	//webOS platform
#else								//PIONEER platform
#undef DISABLE_SCART_HW_BYPASS		//PIONEER platform
//#define DISABLE_SCART_HW_BYPASS		//test only
#endif


/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
/* HAL AUDIO DRIVER Version Info. */
#define	HAL_AUDIO_DRV_VERSION		2014092900

/* AUDIO용 Semaphore 선언 */
extern LX_OSA_OBJECT_T 				gAUDIOSemaId;		/* AUDIO function semaphore */
#define AUDIO_MACRO_LOCK()			KADP_OSA_LockSema(gAUDIOSemaId, LX_OSA_INF_WAIT)
#define AUDIO_MACRO_UNLOCK()		KADP_OSA_UnlockSema(gAUDIOSemaId)

/* ADEC용 Semaphore 선언 */
extern LX_OSA_OBJECT_T 				gADECSemaId;		/* ADEC function semaphore */
#define ADEC_MACRO_LOCK()			KADP_OSA_LockSema(gADECSemaId, LX_OSA_INF_WAIT)
#define ADEC_MACRO_UNLOCK()			KADP_OSA_UnlockSema(gADECSemaId)

/* ADEC4K용 Semaphore 선언 */
extern LX_OSA_OBJECT_T 				gADEC4KSemaId;		/* ADEC4K function semaphore */
#define ADEC4K_MACRO_LOCK()			KADP_OSA_LockSema(gADEC4KSemaId, LX_OSA_INF_WAIT)
#define ADEC4K_MACRO_UNLOCK()		KADP_OSA_UnlockSema(gADEC4KSemaId)

/* AAD용 Semaphore 선언 */
extern LX_OSA_OBJECT_T 				gAADSemaId;			/* AAD function semaphore */
#define AAD_MACRO_LOCK()			KADP_OSA_LockSema(gAADSemaId, LX_OSA_INF_WAIT)
#define AAD_MACRO_UNLOCK()			KADP_OSA_UnlockSema(gAADSemaId)

/* AENC0용Semaphore 선언 */
extern LX_OSA_OBJECT_T 				gAENC0SemaId;		/* AENC function semaphore */
#define AENC_MACRO_LOCK()			KADP_OSA_LockSema(gAENC0SemaId, LX_OSA_INF_WAIT)
#define AENC_MACRO_UNLOCK()			KADP_OSA_UnlockSema(gAENC0SemaId)

/* define country group */
#define HAL_AUDIO_DVB_ALL_MASK		(HAL_AUDIO_SIF_DVB_SELECT		\
								   | HAL_AUDIO_SIF_DVB_ID_SELECT	\
								   | HAL_AUDIO_SIF_DVB_IN_SELECT	\
								   | HAL_AUDIO_SIF_DVB_CN_SELECT 	\
								   | HAL_AUDIO_SIF_DVB_AJJA_SELECT )

/* define Simple Audio SDEC/HDMI/Decoder/Encoder Number */
#define AUD_SDEC0		HAL_AUDIO_RESOURCE_SDEC0
#define AUD_SDEC1		HAL_AUDIO_RESOURCE_SDEC1

/* decoder */
#define AUD_ADEC0		0
#define AUD_ADEC1		1
#define AUD_ADEC_MAX	2

/* hdmi */
#define AUD_HDMI0		0
#define AUD_HDMI1		1
#define AUD_HDMI2		2
#define AUD_HDMI3		3
#define AUD_HDMI4		4
#define AUD_HDMI_MAX	5

/* mixer */
#define AUD_AMIX0		0
#define AUD_AMIX1		1
#define AUD_AMIX2		2
#define AUD_AMIX3		3
#define AUD_AMIX4		4
#define AUD_AMIX5		5
#define AUD_AMIX6		6
#define AUD_AMIX7		7
#define AUD_AMIX_MAX	8

/* encoder */
#define AUD_AENC0		0
#define AUD_AENC1		1
#define AUD_AENC_MAX	2

/* capturer */
#define AUD_APCM0		0
#define AUD_APCM1		1
#define AUD_APCM2		2
#define AUD_APCM_MAX	3

/* input, output */
#define AUD_MIXER_MAX				(AUD_ADEC_MAX + AUD_AMIX_MAX)
#define AUD_I2S_CHANNEL_MAX			4
#define AUD_ADC_INPUT_MAX			5

/* define invalid audio resource number. */
#define AUD_RESOURCE_INVALID				0xFF	// HAL_AUDIO_RESOURCE_NO_CONNECTION

/* define delay for DSP start operation */
#define DSP_START_DELAY_10MS				(10)	// 10 ms for DSP start operation

/* define delay for DSP stop operation */
#define DSP_STOP_DELAY_10MS					(10)	// 10 ms for DSP stop operation

/* define delay for GPIO mute */
#define DAC_MUTE_DELAY_30MS					(30+10)	// 30 ms for SCART GPIO mute

/* In DVB mode, calls 30ms and in ATSC mode, calls 20ms(maybe) */
#define DDI_DEBUG_MSG_PRINT_TIME_10S		350

/* define adec volume scale and step*/
#define ADEC_MAIN_VOLUME_GAIN_0DB			0x7F
#define ADEC_FINE_VOLUME_GAIN_STEP			0x10

/* define adec DSP volume value */
#define ADEC_DSP_MAIN_VOLUME_GAIN_0DB		0x00800000
#define ADEC_DSP_MAIN_VOLUME_GAIN_MUTE		0x00000000
#define ADEC_DSP_MIX_VOLUME_GAIN_0DB		1024
#define ADEC_DSP_MIX_VOLUME_GAIN_MUTE		0

/* define adec clip status */
#define HAL_AUDIO_CLIP_PLAY_MIX				(HAL_AUDIO_CLIP_DONE + 1)
#define HAL_AUDIO_CLIP_PAUSE_MIX			(HAL_AUDIO_CLIP_DONE + 2)
#define HAL_AUDIO_CLIP_PLAY_SIZE			(0x20000)	//128KB
#define HAL_AUDIO_CLIP_PLAY_MIN_SIZE		(0x02800)	//10KB, Buffer Max Access Size : 20KB

/* Check a audio task status. */
#define	BOOT_DELAY_TIME_OF_THREAD_3SEC		(3000)
#define	CHECK_TIME_OF_ADEC_500MSEC			(500)
#define	CHECK_TIME_OF_ADEC_1SEC				(1000)
#define CHECK_TIME_OF_SMART_SOUND			(gAudioModuleInfo.callbackPeriod/CHECK_TIME_OF_ADEC_500MSEC)
#define THREAD_DEBUG_MSG_PRINT_TIME_10S		(10*1000/CHECK_TIME_OF_ADEC_500MSEC)
#define THREAD_DEBUG_MSG_PRINT_TIME_100S	(100*1000/CHECK_TIME_OF_ADEC_500MSEC)

/* define smart sound callback data size */
#define SMARTSOUND_LENGTH					(HAL_AUDIO_LGSE_SMARTSOUND_CB_LENGTH)

/* define default A2 Threshold Level. */
#define DEFAULT_A2_THRESHOLD_VALUE			(11)
#define DEFAULT_A2_THRESHOLD_LEVEL			(0x201A)

/* define max get delay time. */
#define AUD_MAX_GET_DELAY_TIME				(100)
#define AUD_GET_DELAY_TIME_40MS				(40)

/* define lip sync offset. */
#define ADEC_LIPSYNC_OFFSET_MIN_150MS		(150)
#define ADEC_LIPSYNC_OFFSET_MAX_2000MS		(2000)

/* define Free Min TP Buffer Size : 10KB */
#define HAL_AUDIO_TP_BUFFER_MIN_SIZE		(0x02800)	//10KB, Buffer Max Access Size : 20KB


/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/
#define LGSE_SUPPORT	//define for HAL Implementation for LG Sound Engine(Auto Volume, Clear Voice...)

/* LOGM debug message wrapper : HAL AUDIO */
extern SINT32 gHalAudioLogmFd;
#define HAL_AUDIO_LOGM 						"hal-audio"
#define HAL_AUDIO_ERROR(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_ERROR, 	fmt, ##args)
#define HAL_AUDIO_WARN(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define HAL_AUDIO_NOTI(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_NOTI, 	fmt, ##args)
#define HAL_AUDIO_INFO(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_INFO, 	fmt, ##args)
#define HAL_AUDIO_DEBUG(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_DEBUG, 	fmt, ##args)
#define HAL_AUDIO_TRACE(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE, 	fmt, ##args)

#define HAL_AUDIO_STATE_LOG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+1, fmt, ##args)
#define HAL_AUDIO_PATH_LOG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+2, fmt, ##args)
#define HAL_AUDIO_ADEC_LOG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+3, fmt, ##args)
#define HAL_AUDIO_AENC_LOG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+4, fmt, ##args)
#define HAL_AUDIO_AAD_LOG(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+6, fmt, ##args)
#define HAL_AUDIO_SE_LOG(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+7, fmt, ##args)
#define HAL_AUDIO_OUT_LOG(fmt, args...)		KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+8, fmt, ##args)
#define HAL_AUDIO_SAME_LOG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioLogmFd, LX_LOGM_LEVEL_TRACE+9, fmt, ##args)

#define	AUDIO_CRITICAL_ERROR(args...) 		HAL_AUDIO_ERROR("[AUDIO ERROR] " args)
#define	ADEC_CRITICAL_ERROR(args...) 		HAL_AUDIO_ERROR("[ADEC ERROR] " args)
#define	AENC_CRITICAL_ERROR(args...) 		HAL_AUDIO_ERROR("[AENC ERROR] " args)
#define	AAD_CRITICAL_ERROR(args...) 		HAL_AUDIO_ERROR("[AAD ERROR] " args)

/* LOGM debug message wrapper : HAL AUDIO PCM */
extern SINT32 gHalAudioApcmLogmFd;
#define HAL_AUDIO_APCM_LOGM 				"hal-audio-apcm"
#define HAL_AUDIO_APCM_ERROR(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_ERROR, 	fmt, ##args)
#define HAL_AUDIO_APCM_WARN(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define HAL_AUDIO_APCM_NOTI(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_NOTI, 	fmt, ##args)
#define HAL_AUDIO_APCM_INFO(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_INFO, 	fmt, ##args)
#define HAL_AUDIO_APCM_DEBUG(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_DEBUG, 	fmt, ##args)
#define HAL_AUDIO_APCM_TRACE(fmt, args...)	KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_TRACE, 	fmt, ##args)

#define HAL_AUDIO_APCM_LOG(fmt,args...)		KADP_LOGM_PRINT(gHalAudioApcmLogmFd, LX_LOGM_LEVEL_TRACE+1, fmt, ##args)
#define	APCM_CRITICAL_ERROR(args...) 		HAL_AUDIO_APCM_ERROR("[APCM ERROR] " args)


/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
/**
 * Audio Resource Status
 */
typedef enum
{
	AUDIO_RESOURCE_OPEN 		= 0,
	AUDIO_RESOURCE_CONNECT 		= 1,
	AUDIO_RESOURCE_DISCONNECT 	= 2,
	AUDIO_RESOURCE_CLOSE	 	= 3,
	AUDIO_RESOURCE_START	 	= 4,
	AUDIO_RESOURCE_STOP		 	= 5,
	AUDIO_RESOURCE_UNKNOWN	 	= 6,
}AUDIO_RESOURCE_STATUS_T;

/**
 * APCM PCM Capture Status
 */
typedef enum
{
	APCM_STATUS_STOP 		= 0,
	APCM_STATUS_PLAY		= 1,
	APCM_STATUS_ABNORMAL	= 2,
}APCM_STATUS_T;

/* define audio context for master, decoder, render and encoder. */
typedef struct AdecDevContext
{
	//For Master
	LX_AUD_MASTER_CTX_T 	*pMasterCtx;

	//For Decoder & Render
	LX_AUD_DECODER_CTX_T	*pDecoderCtx[AUD_ADEC_MAX];
	LX_AUD_RENDER_CTX_T 	*pRenderCtx[AUD_ADEC_MAX];

	//For SPDIF ES output
	LX_AUD_RENDER_CTX_T 	*pSpdifRenderCtx;

	//For Audio Clip Dec
	LX_AUD_DECODER_CTX_T	*pClipDecDecoderCtx[AUD_ADEC_MAX];
	LX_AUD_RENDER_CTX_T 	*pClipDecRenderCtx[AUD_ADEC_MAX];

	//For Audio Clip Mix
	LX_AUD_RENDER_CTX_T 	*pClipMixRenderCtx[AUD_AMIX_MAX];

	//For Audio Encoder
	LX_AUD_ENCODER_CTX_T	*pEncoderCtx[AUD_AENC_MAX];
	LX_AUD_CAPTURE_CTX_T	*pCaptureCtx[AUD_AENC_MAX];
} ADEC_DEV_CONTEXT_T;

/* define audio context for master and capturer */
typedef struct AdecDevApcmContext
{
	//For Master
	LX_AUD_MASTER_CTX_T 	*pMasterCtx;

	//For Audio PCM Capture
	LX_AUD_CAPTURE_CTX_T	*pPcmCaptureCtx[AUD_APCM_MAX];
} ADEC_DEV_APCM_CONTEXT_T;

/**
 * Audio Resource Status Info.
 * This enumeration describes the Audio Resource Status Information.
*/
typedef struct AudioResourceInfo
{
	AUDIO_RESOURCE_STATUS_T		tp0Input;			///< Audio Resource : TP0 Input
	AUDIO_RESOURCE_STATUS_T		tp1Input;			///< Audio Resource : TP1 Input
	AUDIO_RESOURCE_STATUS_T		adcInput;			///< Audio Resource : ADC Input
	AUDIO_RESOURCE_STATUS_T		hdmiInput;			///< Audio Resource : HDMI Input
	AUDIO_RESOURCE_STATUS_T		aadInput;			///< Audio Resource : AAD Input
	AUDIO_RESOURCE_STATUS_T		systemInput;		///< Audio Resource : SYSTEM Input
	AUDIO_RESOURCE_STATUS_T		decoder0;			///< Audio Resource : Audio Decoder0
	AUDIO_RESOURCE_STATUS_T		decoder1;			///< Audio Resource : Audio Decoder1
	AUDIO_RESOURCE_STATUS_T		encoder0;			///< Audio Resource : Audio Encoder0
	AUDIO_RESOURCE_STATUS_T		encoder1;			///< Audio Resource : Audio Encoder1(Not Used Now)
	AUDIO_RESOURCE_STATUS_T		mixer[AUD_AMIX_MAX];///< Audio Resource : Audio Mixer(0~7)
	AUDIO_RESOURCE_STATUS_T		hdmiInputPort[AUD_HDMI_MAX];		///< Audio Resource : Audio HDMI(0~3)

	AUDIO_RESOURCE_STATUS_T		soundEngine;						///< Audio Resource : Sound Engine
	AUDIO_RESOURCE_STATUS_T		soundEngineAdec[AUD_ADEC_MAX];		///< Audio Resource : Sound Engine for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		soundEngineAmixer[AUD_AMIX_MAX];	///< Audio Resource : Sound Engine for AMIXER0~7
	AUDIO_RESOURCE_STATUS_T		spkOutput;							///< Audio Resource : Speaker Sound Output

	AUDIO_RESOURCE_STATUS_T		spdifOutput;						///< Audio Resource : SPDIF Sound Output
	AUDIO_RESOURCE_STATUS_T		spdifOutputAdec[AUD_ADEC_MAX];		///< Audio Resource : SPDIF Sound Output for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		spdifOutputAmixer[AUD_AMIX_MAX];	///< Audio Resource : SPDIF Sound Output for AMIXER0~7

	AUDIO_RESOURCE_STATUS_T		spdifESOutput;						///< Audio Resource : SPDIF ES Sound Output
	AUDIO_RESOURCE_STATUS_T		spdifESOutputAdec[AUD_ADEC_MAX];	///< Audio Resource : SPDIF ES Sound Output for ADEC0~1

	AUDIO_RESOURCE_STATUS_T		sbSpdifOutput;						///< Audio Resource : Sound Bar SPDIF Sound Output
	AUDIO_RESOURCE_STATUS_T		sbSpdifOutputAdec[AUD_ADEC_MAX];	///< Audio Resource : Sound Bar SPDIF for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		sbSpdifOutputAmixer[AUD_AMIX_MAX];	///< Audio Resource : Sound Bar SPDIF for AMIXER0~7

	AUDIO_RESOURCE_STATUS_T		sbPcmOutput;						///< Audio Resource : Sound Bar PCM Sound Output
	AUDIO_RESOURCE_STATUS_T		sbPcmOutputAdec[AUD_ADEC_MAX];		///< Audio Resource : Sound Bar PCM for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		sbPcmOutputAmixer[AUD_AMIX_MAX];	///< Audio Resource : Sound Bar PCM for AMIXER0~7

	AUDIO_RESOURCE_STATUS_T		sbCanvasOutput;						///< Audio Resource : Sound Bar CANVAS Sound Output
	AUDIO_RESOURCE_STATUS_T		sbCanvasOutputAdec[AUD_ADEC_MAX];	///< Audio Resource : Sound Bar CANVAS for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		sbCanvasOutputAmixer[AUD_AMIX_MAX];	///< Audio Resource : Sound Bar CANVAS for AMIXER0~7

	AUDIO_RESOURCE_STATUS_T		hpOutput;							///< Audio Resource : HP Sound Output
	AUDIO_RESOURCE_STATUS_T		hpOutputAdec[AUD_ADEC_MAX];			///< Audio Resource : HP Sound Output for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		hpOutputAmixer[AUD_AMIX_MAX];		///< Audio Resource : HP Sound Output for AMIXER0~7

	AUDIO_RESOURCE_STATUS_T		scartOutput;						///< Audio Resource : SCART Sound Output
	AUDIO_RESOURCE_STATUS_T		scartOutputAdec[AUD_ADEC_MAX];		///< Audio Resource : SCART Sound Output for ADEC0~1
	AUDIO_RESOURCE_STATUS_T		scartOutputAmixer[AUD_AMIX_MAX];	///< Audio Resource : SCART Sound Output for AMIXER0~7
} AUDIO_RESOURCE_INFO_T;

/**
 * ADEC Audio Decoder Mode Info.
 * This enumeration describes the ADEC decoder mode information.
*/
typedef struct AdecDecoderMode
{
	LX_AUD_DECODER_DRC_T		drcMode;		///< AC-3, AAC DRC Mode.
	LX_AUD_DECODER_DOWNMIX_T	downmixMode; 	///< AC-3, AAC Downmix Mode..
} ADEC_DECODER_MODE_T;

/**
 * Audio Decoder Internal Module Status Info.
 *
 */
typedef struct AudioAdecModuleInfo
{
	BOOLEAN		bSyncAVMode;					///<  Current Sync AV mode or FreeRun Mode
	BOOLEAN		bSetNoPCRMode;					///<  Current ADEC No PCR Mode Status for TP Input
	BOOLEAN		bSetGstcMode;					///<  Current ADEC GSTC Mode Status for TP Input
	BOOLEAN		bSetMasterMode;					///<  Current ADEC Master GSTC Mode Status for TP Input
	UINT32		bAVSyncOffset;					///<  Current ADEC Lip Sync Offset Value for TP Input

	BOOLEAN		bTrickPause;					///<  Current Audio Trick Pause State Enabled.
	HAL_AUDIO_TRICK_MODE_T		eTrickMode;		///<  Current Audio ADEC Trick Mode Status

	BOOLEAN		bAencBypassMode;				///<  Current Audio AENC AAC Codec Bypass Mode because of DSP performance issue
	BOOLEAN		bAencStart;						///<  Current Audio AENC Encoder Start Status from AENC Module.
	BOOLEAN		bHpOutputConnected; 			///<  Current Audio HP Output Port Connected Status for PIP mode.
	#ifdef DISABLE_SCART_HW_BYPASS
	BOOLEAN		bScartOutputEnabled; 			///<  Current Audio SCART Output Port Enabled Status for SCART Bypass mode.
	BOOLEAN		bScartOutputConnected; 			///<  Current Audio SCART Output Port Connected Status for SCART Bypass mode.
	#endif

	UINT32		tpBufMaxSize;					///<  Current ADEC TP Buffer Max Size
	UINT32		tpBufFreeSize;					///<  Current ADEC TP Buffer Free Size

	UINT32						decoderDelay;	///<  Current ADEC Decoder Delay Time(Set)
	UINT32						internalDelay;	///<  Current ADEC Decoder Internal Delay Time(Get)
	ADEC_DECODER_MODE_T 		decoderMode;	///<  Current ADEC Decoder Mode
	LX_AUD_DECODER_OUT_MODE_T	dualMonoMode;	///<  Current Audio Decoder and Encoder Dual-Mono Output Mode

	BOOLEAN					bAdecStartFor4KTP;		///<  Current ADEC Start Status For4KTP
	HAL_AUDIO_SRC_TYPE_T   	curAdecFormatFor4KTP;	///<  Current ADEC Format Type For4KTP

	LX_AAD_INIT_TYPE_T		aadInitType;		///<  Current AAD Init Type
	UINT16					A2ThresholdValue;	///<  Current SIF A2 threshold Value of A2 from UI
	UINT16					A2ThresholdLevel;	///<  Current SIF A2 threshold level of A2
	BOOLEAN					bHighDevOnOff;		///<  Current Set State of SIF High Deviation On/Off
	LX_AAD_HIGH_DEV_LEVEL_T	highDeviationLevel; ///<  Current SIF High Deviation Level Status

	HAL_AUDIO_CLIP_STATUS_T		clipDecStatus;	///<  Current ADEC Clip DEC Play Status
	UINT8				*pClipDecBuffer;		///<  The Buffer for ADEC Clip DEC Play
	UINT32				clipDecSize;			///<  The Buffer Size for Adec Clip DEC Play
	UINT32				clipDecLocation;		///<  The Current Buffer Location for Adec Clip DEC Play
	UINT32				clipDecRepeatNumber;	///<  The Repeat Number for ADEC Clip DEC Play
	pfnAdecoderClipDone pfnClipDoneCallBack;	///<  The function pointer of ADEC Clip Play Done

	HAL_AUDIO_CLIP_STATUS_T		clipMixStatus;	///<  Current ADEC Clip Play Mix Status
	UINT8				*pClipMixBuffer;		///<  The Buffer for ADEC Clip Mix Play
	UINT32				clipMixSize;			///<  The Buffer Size for Adec Clip Mix Play
	UINT32				clipMixLocation;		///<  The Current Buffer Location for Adec Clip Mix Play
	UINT32				clipMixRepeatNumber;	///<  The Repeat Number for ADEC Clip Mix Play
	BOOLEAN				clipMixSpdifEnabled;	///<  The Enabled Status SPDIF Output for ADEC Clip Mix Play

	BOOLEAN				adecDecMute;			///<  Current ADEC Decoder Output Mute Status
	BOOLEAN				adecDecESMute;			///<  Current ADEC Decoder ES Output Mute Status
	UINT32				adecDecVolume;			///<  Current ADEC Decoder Output Volume
	UINT32				adecMixVolume;			///<  Current ADEC Mixer Input Volume
	UINT32				adecMixADVolume;		///<  Current ADEC Mixer Input Volume for Audio Description

	HAL_AUDIO_VOLUME_T	curDecOutVol;			///<  Current ADEC Decoder Output UI Volume
	HAL_AUDIO_VOLUME_T	curMixInVol;			///<  Current ADEC Mixer Input UI Volume
	HAL_AUDIO_VOLUME_T	curMixInADVol;			///<  Current ADEC Mixer Input UI Volume for Audio Description
} AUDIO_ADEC_MODULE_INFO_T;

/**
 * Audio Mixer Internal Module Status Info.
 *
 */
typedef struct AudioAmixModuleInfo
{
	HAL_AUDIO_CLIP_STATUS_T		clipMixStatus;	///<  Current ADEC Clip Play Mix Status
	UINT8				*pClipMixBuffer;		///<  The Buffer for ADEC Clip Mix Play
	UINT32				clipMixSize;			///<  The Buffer Size for Adec Clip Mix Play
	UINT32				clipMixLocation;		///<  The Current Buffer Location for Adec Clip Mix Play
	UINT32				clipMixRepeatNumber;	///<  The Repeat Number for ADEC Clip Mix Play
	BOOLEAN				clipMixSpdifEnabled;	///<  The Enabled Status SPDIF Output for ADEC Clip Mix Play
	pfnAmixerClipDone	pfnClipDoneCallBack;	///<  The function pointer of ADEC Clip Play Done

	BOOLEAN				amixRenMute;			///<  Current Audio Renderer Output Mute Status
	UINT32				amixRenVolume;			///<  Current Audio Renderer Output Volume
	UINT32				amixMixVolume;			///<  Current Audio Mixer Input Volume

	HAL_AUDIO_VOLUME_T	curRenOutVol;			///<  Current Audio Renderer Output UI Volume
	HAL_AUDIO_VOLUME_T	curAmixInVol;			///<  Current Audio Mixer Input UI Volume
} AUDIO_AMIX_MODULE_INFO_T;

/**
 * Audio Internal Module Status Info.
 *
 */
typedef struct AudioModuleInfo
{
	UINT32		versionInfo;					///<  The version info. of hal audio driver version.

	UINT32		taskId;							///<  The task id for AUDIO Event Task
	BOOLEAN		bInitDone;						///<  Audio Driver Init Done Status
	UINT8		audioI2SNumber;					///<  Current Audio I2S Channel Number for Speaker
	HAL_AUDIO_SAMPLING_FREQ_T audioI2SFreq;		///<  Current Audio I2S Sampling Frequency for Speaker

	UINT8		adcPortNum;						///<  Current Audio ADC Port Number
	UINT8		trickDecNum;					///<  Current Audio Trick Decoder Number
	UINT8		aencDecNum;						///<  Current Audio AENC Decoder Number
	UINT8		mainDecoderNum;					///<  Current Audio Main Decoder Number for Main and SPDIF ES Output.

	BOOLEAN		bAudioDescription;				///<  Current Audio Description Mode Enabled.(SPDIF PCM Main Only Status for AD case.)
	BOOLEAN		bSpdifMainOnly;					///<  Current Audio SPDIF PCM Main Only Status for AD case
	BOOLEAN		bSpdifAacBypass;				///<  Current Audio SPDIF ES AAC BYPASS Status for Japan Model case
	BOOLEAN		bHpOutputConnected; 			///<  Current Audio HP Output Port Connected Status for Normal mode.
	#ifdef DISABLE_SCART_HW_BYPASS
	BOOLEAN		bScartOutputConnected; 			///<  Current Audio SCART Output Port Connected Status for Normal mode.
	#endif

	UINT32		audioMixVolume[AUD_MIXER_MAX];	///<  Current Audio Main Mixer Input Volume
	UINT32		audioSpkVolume;					///<  Current Audio SPK Volume
	UINT32		audioSpdifVolume;				///<  Current Audio SPDIF Output Volume
	UINT32		audioHpVolume;					///<  Current Audio HP Output Volume
	#ifdef DISABLE_SCART_HW_BYPASS
	UINT32		audioScartVolume;				///<  Current Audio SCART Output Volume
	#else
	UINT16		audioScartDacVolume;			///<  Current Audio SCART DAC Output Volume
	#endif

	BOOLEAN		audioSpkMute;					///<  Current Audio SPK Output Mute
	BOOLEAN		audioSpdifMute;					///<  Current Audio SPDIF Output Mute
	BOOLEAN		audioHpMute;					///<  Current Audio HP Output Mute
	#ifdef DISABLE_SCART_HW_BYPASS
	BOOLEAN		audioScartMute;					///<  Current Audio SCART Output Mute
	#else
	BOOLEAN		audioScartDacMute;				///<  Current Audio SCART DAC Output Mute
	#endif
	BOOLEAN 	bSoundBarMode;					///<  Current Audio Sound Bar Mode Enabled Status for sound bar controlling
	BOOLEAN 	bWirelessMode;					///<  Current Audio Wireless Sound Bar Mode Enabled Status for sound bar controlling
	BOOLEAN 	bSoundCanvasMode;				///<  Current Audio Sound Frame Mode Enabled Status for sound bar controlling

	HAL_AUDIO_RESOURCE_T	connectTP0;			///<  Current Audio ATP0 Connect Resource
	HAL_AUDIO_RESOURCE_T	connectTP1;			///<  Current Audio ATP1 Connect Resource
	HAL_AUDIO_SNDOUT_T 		soundOutType;		///<  Current Audio Sound Output Type Mask

	#ifndef DISABLE_SCART_HW_BYPASS
	BOOLEAN		bScartAadBypass;				///<  Current Audio SCART AAD BYPASS Output Enabled Status for DVB Model case
	UINT8		scartDecNum;					///<  Current Audio SCART Decoder Number of SCART AAD BYPASS Case.
	#endif

	UINT8		aacPcmDecNum;					///<  Current Audio AAC PCM Decoder Number of SPDIF PCM Ouput if Fs is not 48KHz.
	UINT8		aac51DualDecNum;				///<  Current Audio AAC5.1 Dual Decoder Number of DSP Performance Issue.

	UINT32		internalDelay;					///<  Current ADEC Decoder Internal Delay Time(Get)
	BOOLEAN		bSetARCOnOff;					///<  Current ADEC ARC On/Off Status for HDMI ARC Output

	pfnLGSESmartSound		pfnCallBack;		///<  The callback function pointer for smart sound callback
	UINT32		callbackPeriod;					///<  The callback period for smart sound callback
	UINT32		smartSound[SMARTSOUND_LENGTH];	///<  The callback data for smart sound callback
} AUDIO_MODULE_INFO_T;

/**
 * ADEC ENC Module Info.
 * ADEC encoder module start and status info.
 *
 */
typedef struct AencModuleInfo
{
	UINT32			taskId;							///<  The task id for AENC
	BOOLEAN			bInitDone;						///<  The initialized status of AENC module
	BOOLEAN			bBufReleased;					///<  The status of AENC local buffer released status from caller

	BOOLEAN			bFirstPts;						///<  The status of First PTS value arrived from audio DSP.
	UINT64			ptsFromEncoder;					///<  The PTS value for AENC module from audio DSP.
	UINT64			ptsToEncoder;					///<  The PTS value for SENC module from AENC module.

	UINT8			*pBuffer;						///<  The Buffer for AENC Capture device

	HAL_AUDIO_AENC_ENCODING_FORMAT_T	codec;		///<  The encoder codec

	HAL_AUDIO_AENC_INDEX_T		index;				///<  The encoder index
	pfnAENCDataHandling			fnDataHandlingCB;	///<  The callback function pointer for aenc callback

	UINT32						numOfChannel;		///< The number of channnel
	LX_AUD_ENCODER_BITRATE_T	bitRate;			///< The Bit Rate for Audio Encoder

	UINT32				adecEncodeVolume;			///<  Current AENC ES Capture Volume
	HAL_AUDIO_VOLUME_T	curEncodeVolume;			///<  Current AENC ES Capture dB Volume
} AENC_MODULE_INFO_T;

/**
 * ADEC PCM Module Info.
 * ADEC PCM capture module start and status info.
 *
 */
typedef struct ApcmModuleInfo
{
	UINT32			taskId;						///<  The task id for APCM
	APCM_STATUS_T	status;						///<  The current APCM Status

	UINT32			errorCount;					///<  The current APCM error counter
	UINT32			inputCount;					///<  The current APCM input counter
	UINT32			underflowCount;				///<  The current APCM underflowCount in kernel space
	UINT32			overflowCount;				///<  The current APCM overflowCount

	BOOLEAN			bInitDone;					///<  The initialized status of APCM module
	BOOLEAN			bBufReleased;				///<  The status of APCM local buffer released status from caller

	UINT8			*pBuffer;					///<  The Buffer for APCM Capture device

	pfnPCMSending	fnDataSendingCB;			///<  The callback function pointer for apcm callback

	pfnPCMDataHandling		fnDataHandlingCB;	///<  The callback function pointer for apcm callback
	pthread_t				pthread;			///<  The Callback task for APCM PCM data

	LX_AUD_CAPTURE_PARAM_T	capParam;			///< The current Capture Parameters

	UINT8			pcmCaptureCount;			///<  Current PCM Capture audio data count(1 = 5.2ms, 1024 bytes)
	UINT8			i2sCaptureCount;			///<  Current I2S Capture audio channel count

	UINT32				adecCaptureVolume;		///<  Current ADEC PCM Capture Volume
	HAL_AUDIO_VOLUME_T	curCaptureVolume;		///<  Current ADEC PCM Capture dB Volume
} APCM_MODULE_INFO_T;


/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
DTV_STATUS_T HAL_AUDIO_PCM_GetData(HAL_AUDIO_PCM_INPUT_T apcmIndex, UINT8 *pBuffer, UINT16 *pLength);

/* ADEC Function. */
//The audio decoder and renderer control function.
DTV_STATUS_T ADEC_StartDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T audioType);
DTV_STATUS_T ADEC_StopDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T ADEC_OpenDecoderAndRender(HAL_AUDIO_ADEC_INDEX_T adecIndex, LX_AUD_DECODER_CTX_T **pDecoderCtx, LX_AUD_RENDER_CTX_T **pRenderCtx);
DTV_STATUS_T ADEC_OpenAndSetSPDIFRender(LX_AUD_DECODER_CTX_T *pDecoderCtx, LX_AUD_RENDER_CTX_T **pRenderCtx, BOOLEAN bForcedPCM);
DTV_STATUS_T ADEC_SetDecoderAndRender(HAL_AUDIO_ADEC_INDEX_T adecIndex, LX_AUD_DECODER_CTX_T *pDecoderCtx, LX_AUD_RENDER_CTX_T *pRenderCtx);
DTV_STATUS_T ADEC_SetLipsyncParam(HAL_AUDIO_ADEC_INDEX_T adecIndex, LX_AUD_RENDER_CTX_T *pRenderCtx);
DTV_STATUS_T ADEC_CloseDecoderAndRender(HAL_AUDIO_ADEC_INDEX_T adecIndex, LX_AUD_DECODER_CTX_T **pDecoderCtx, LX_AUD_RENDER_CTX_T **pRenderCtx);
DTV_STATUS_T ADEC_CloseSPDIFRender(LX_AUD_DECODER_CTX_T *pDecoderCtx, LX_AUD_RENDER_CTX_T **pRenderCtx);

//The audio event message callback function.
void ADEC_ProcessCallbackEventMessage(LX_AUD_CB_TYPE_T eventCB);

//The connection status check function.
BOOLEAN ADEC_SPK_CheckConnectStatus(void);
BOOLEAN ADEC_SPDIF_CheckConnectStatus(void);
BOOLEAN ADEC_SB_SPDIF_CheckConnectStatus(void);
BOOLEAN ADEC_SB_PCM_CheckConnectStatus(void);
BOOLEAN ADEC_SB_CANVAS_CheckConnectStatus(void);
BOOLEAN ADEC_HP_CheckConnectStatus(void);
BOOLEAN ADEC_SCART_CheckConnectStatus(void);

//The audio clip check buffer status function.
void ADEC_CheckPlayClipDecBufferStatus(void);
void ADEC_CheckPlayClipMixBufferStatus(void);

//The audio smart sound callback function.
void ADEC_CheckSmartSoundCallbackStatus(void);

#endif /* _HAL_AUDIO_DRV_H_ */



