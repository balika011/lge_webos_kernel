/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file adec_ddi.h
 *
 *  ADEC DD를 제어하는 DDI 함수 header 파일.
 *  AUDIO DDI와 ADEC DD를 연결하는 함수들로 주로 AUDIO DDI와 task에서 호출된다.
 *
 *  @author		Park Jeong-Gun(alwaysok@lge.com)
 *  @version	1.0
 *  @date		2006.04.24
 *  @note
 *  @see		adec_ddi.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _ADEC_DDI_H_
#define _ADEC_DDI_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#include "common.h"
#include "osa_api.h"
#include "global_configurations.h"

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

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
//lip_temp
#define USE_2ND_TV

#if (PLATFORM_TYPE == MTK_PLATFORM)

#ifndef MOVE_SETTING_ADEC_TO_AUDIO
#define MOVE_SETTING_ADEC_TO_AUDIO
#endif
#else // PLATFORM_TYPE
#ifndef MOVE_SETTING_ADEC_TO_AUDIO
#define MOVE_SETTING_ADEC_TO_AUDIO
#endif
#endif // endif PLATFROM_TYPE

#define	USER_DELAY_UNIT	(10)

#ifndef USE_CUR_USER_DELAY
#define	USE_CUR_USER_DELAY ((SINT8)(0x80))
#define	USE_CUR_CONSTANT_DELAY_TIME	(0xFFFFFFFF)
#define	CHANGE_CUR_USER_DELAY_DB	(0xF0F0F0F0)
#define	CHANGE_CUR_MVD_DELAY_DB	(0xF0F0F0F0)
#endif

/**
 * ADEC Source Input Type.
 * TP, PCM, SPDIF
 *
 */
typedef  enum
{
	ADEC_IN_PORT_NONE		=  0,
	ADEC_IN_PORT_TP			=  1,	// From TPA Stream Input
	ADEC_IN_PORT_SPDIF 		=  2,	// From SERIAL INTERFACE 0
	ADEC_IN_PORT_SIF		=  3,	// From Analog Front End (SIF)
	ADEC_IN_PORT_ADC		=  4,	// Fron ADC Input
	ADEC_IN_PORT_HDMI		=  5,	// From HDMI
	ADEC_IN_PORT_I2S		=  6,	// From I2S
	ADEC_IN_PORT_SYSTEM		=  7,	// From System
	ADEC_IN_PORT_MIRROR		=  8,	// for distinguish between dual and mirror on Sub path
} ADEC_IN_PORT_T;

/**
 * ADEC Source Format Type.
 *
 */
typedef  enum
{
	ADEC_SRC_TYPE_UNKNOWN       = 0,
	ADEC_SRC_TYPE_PCM           = 1,
	ADEC_SRC_TYPE_AC3           = 2,
	ADEC_SRC_TYPE_EAC3          = 3,
	ADEC_SRC_TYPE_MPEG          = 4,
	ADEC_SRC_TYPE_AAC           = 5,
	ADEC_SRC_TYPE_HEAAC         = 6,
	ADEC_SRC_TYPE_DRA			= 7,
	ADEC_SRC_TYPE_MP3			= 8,
	ADEC_SRC_TYPE_DTS			= 9,
	ADEC_SRC_TYPE_SIF			= 10,
	ADEC_SRC_TYPE_SIF_BTSC		= 11,
	ADEC_SRC_TYPE_SIF_A2		= 12,
	ADEC_SRC_TYPE_DEFAULT		= 13,
	ADEC_SRC_TYPE_NONE			= 14,
} ADEC_SRC_TYPE_T;


typedef  enum
{
	ADEC_OUT_PORT_TVSPEAKER	= 0x01,
	ADEC_OUT_PORT_SPDIF		= 0x02,
	ADEC_OUT_PORT_HEADPHONE	= 0x08,
}	ADEC_OUTPUT_MASK_T;

/**
 * ADEC SPDIF Type.
 *
 */
typedef  enum
{
	ADEC_SPDIF_NONE			= 0,
	ADEC_SPDIF_PCM			= 1,
	ADEC_SPDIF_AUTO			= 2,
	ADEC_SPDIF_AUTO_AAC		= 3
}  ADEC_SPDIF_MODE_T ;

/**
 * ADEC Clip Play Type.
 *
 */
typedef enum
{
	ADEC_CLIP_NONE			= 0,
	ADEC_CLIP_PLAY			= 1,
	ADEC_CLIP_DONE			= 2,
	ADEC_CLIP_PAUSE			= 3
} ADEC_CLIP_PLAY_T;

/**
 * ADEC Source Format Type.
 *
 */
typedef enum
{
	ADEC_DUALMONO_MODE_LR		= 0,
	ADEC_DUALMONO_MODE_LL		= 1,
	ADEC_DUALMONO_MODE_RR		= 2,
	ADEC_DUALMONO_MODE_MIX		= 3
}ADEC_DUALMONO_OUTPUT_MODE_T;


/**
 * ADEC HDMI Format Type.
 *
 */
typedef enum
{
	ADEC_HDMI_DVI			= 0,
	ADEC_HDMI_NO_AUDIO		= 1,
	ADEC_HDMI_PCM			= 2,
	ADEC_HDMI_AC3			= 3,
	ADEC_HDMI_DTS			= 4,
	ADEC_HDMI_AAC			= 5,
	ADEC_HDMI_DEFAULT
}ADEC_HDMI_TYPE_T;

/**
 * Sample frequency values merged from Audio InfoFrame and Audio Channel Status
 *
 */
typedef enum
{
    //See CEA-861-D, table 18
	ADEC_SAMPLE_NONE 		= 0,
	ADEC_SAMPLE_32_KHZ		= 1,
	ADEC_SAMPLE_44_1KHZ		= 2,
	ADEC_SAMPLE_48_KHZ		= 3,
	ADEC_SAMPLE_88_2KHZ		= 4,
	ADEC_SAMPLE_96_KHZ		= 5,
	ADEC_SAMPLE_176_4KHZ	= 6,
	ADEC_SAMPLE_192_KHZ		= 7,

	//See IEC60958-3, page 12
	ADEC_SAMPLE_22_05KHZ	= 8,
	ADEC_SAMPLE_24_KHZ		= 9,
	ADEC_SAMPLE_8_KHZ		= 10,
	ADEC_SAMPLE_768_KHZ		= 11
} ADEC_SAMPLE_FREQ_T;

/**
 * Audio Master Clock Frequency Definition
 *
 */
typedef enum
{
	F12_288MHZ,
	F24_576MHZ,
	F18_432MHZ,
	F8_192MHZ,
	F16_384MHZ,
} ADEC_MCLK_REAL_FREQUENCY_T;

/**
 * ADEC Sampling rate for Bluetooth
 *
 */
typedef enum
{
	ADEC_SAMPLERATE_BYPASS	= 0,
	ADEC_SAMPLERATE_48K		= 1,
	ADEC_SAMPLERATE_44_1K	= 2,
	ADEC_SAMPLERATE_32K		= 3,
	ADEC_SAMPLERATE_24K		= 4,
	ADEC_SAMPLERATE_16K		= 5,
	ADEC_SAMPLERATE_12K		= 6,
	ADEC_SAMPLERATE_8K		= 7,
} ADEC_SAMPLERATE_T;

/**
 * ADEC SIF Mode.
 *
 */
typedef enum
{
	ADEC_SIF_MODE_DETECT	= 0,
	ADEC_SIF_BG_NICAM		= 1,
	ADEC_SIF_BG_FM			= 2,
	ADEC_SIF_BG_A2			= 3,
	ADEC_SIF_I_NICAM		= 4,
	ADEC_SIF_I_FM			= 5,
	ADEC_SIF_DK_NICAM		= 6,
	ADEC_SIF_DK_FM			= 7,
	ADEC_SIF_DK1_A2			= 8,
	ADEC_SIF_DK2_A2			= 9,
	ADEC_SIF_DK3_A2			= 10,
	ADEC_SIF_L_NICAM		= 11,
	ADEC_SIF_L_AM			= 12,
	ADEC_SIF_MN_A2			= 13,
	ADEC_SIF_MN_BTSC		= 14,
	ADEC_SIF_MN_EIAJ		= 15,
	ADEC_SIF_NUM_SOUND_STD	= 16,
}ADEC_SIF_STANDARD_T;

/**
 * ADEC SIF Format Type.
 *
 */
typedef enum
{
	ADEC_SIF_NICAM,
	ADEC_SIF_A2,
	ADEC_SIF_FM,
	ADEC_SIF_DETECTING_AVALIBILITY
}ADEC_SIF_AVAILE_STANDARD_T;

/**
 * ADEC Source Format Type.
 *
 */
typedef enum
{
	ADEC_SIF_SYSTEM_BG	= 0x00,	// keep the position. 	refer to IF_PLL_DATA_TYPE_t
	ADEC_SIF_SYSTEM_I,			// keep the position. 	refer to IF_PLL_DATA_TYPE_t
	ADEC_SIF_SYSTEM_DK,			// keep the position. 	refer to IF_PLL_DATA_TYPE_t
	ADEC_SIF_SYSTEM_L,			// keep the position. 	refer to IF_PLL_DATA_TYPE_t
	ADEC_SIF_SYSTEM_MN,			// keep the position. 	refer to IF_PLL_DATA_TYPE_t
	ADEC_SIF_SYSTEM_UNKNOWN = 0xf0
} ADEC_SIF_SOUNDSYSTEM_T;

/**
 * ADEC Source Format Type.
 *
 */
typedef enum
{
	ADEC_SIF_ABSENT	 = 0,
	ADEC_SIF_PRESENT 		 = 1,
	ADEC_SIF_DETECTING_EXSISTANCE
}ADEC_SIF_EXISTENCE_INFO_T;

#ifndef MOVE_SETTING_ADEC_TO_AUDIO
/**
 * ADEC Source User Delay Index Definition
 *
 */
typedef  enum
{
	AUD_GAIN_DEFAULT		= 0,
	AUD_GAIN_DTV_MPEG,
	AUD_GAIN_DTV_BITSTREAM,
	AUD_GAIN_ATV_PAL_BG_NICAM,
	AUD_GAIN_ATV_PAL_BG_MONO,
	AUD_GAIN_ATV_PAL_BG_STEREO,
	AUD_GAIN_ATV_PAL_IDK,
	AUD_GAIN_ATV_PAL_I_NICAM,
	AUD_GAIN_ATV_PAL_DK_NICAM,
	AUD_GAIN_ATV_PAL_L_NICAM,
	AUD_GAIN_ATV_PAL_L_AM,
	AUD_GAIN_ATV_NTSC_A2,
	AUD_GAIN_ATV_NTSC_A2_SAP,
	AUD_GAIN_ATV_BTSC_MONO,
	AUD_GAIN_ATV_BTSC_STEREO,
	AUD_GAIN_ATV_BTSC_SAP,
	AUD_GAIN_ATV_EIAJ,
	AUD_GAIN_AV,
	AUD_GAIN_COMP,
	AUD_GAIN_RGB_DVI,
	AUD_GAIN_HDMI,
/*	AUD_GAIN_HDMI_PCM,
	AUD_GAIN_HDMI_BITSREAM, */
	AUD_GAIN_PLAY_HDD,
#if 1	// Set by input
	AUD_GAIN_EMF_PHOTO_MUSIC,
	AUD_GAIN_EMF_MOVIE,
#else	// Set by audio format
	AUD_GAIN_EMF_MPEG,
	AUD_GAIN_EMF_BITSTREAM,
#endif
	AUD_GAIN_BLUETOOTH,
	AUD_GAIN_MEDIASHARE,
	AUD_GAIN_PICWZARD,
	AUD_GAIN_BB,
	AUD_GAIN_FLASH,

	AUD_GAIN_INDEX_MAX
}  ADEC_GAIN_INDEX_T;
#endif

/**
 * ADEC SPDIF Copy Protection Type
 *
 */
typedef  enum
{
	ADEC_SPDIF_COPY_FREE		= 0,	/* cp-bit : 1, L-bit : 0 */
	ADEC_SPDIF_COPY_NO_MORE		= 1,	/* cp-bit : 0, L-bit : 1 */
	ADEC_SPDIF_COPY_ONCE		= 2,	/* cp-bit : 0, L-bit : 0 */
	ADEC_SPDIF_COPY_NEVER		= 3,	/* cp-bit : 0, L-bit : 1 */
} ADEC_SPDIF_COPYRIGHT_T ;

/**
 * ADEC Audio Control Type
 *
 */
typedef struct ADEC_CTL_TYPE
{
	UINT32	country_mask;
	UINT8	audModuleMask;
	UINT8	audDelyTypeIndex;
	UINT8	audVolCrvIndex;		/* AMP Volume */
	UINT8	audAmpEqIdex;		/* AMP Parametric EQ */
	UINT8	audAmpCtlIndex;		/* AMP register Control value */
	UINT8	audI2SGainIndex;
	UINT8	audSpdifGainIndex;
#if 0
	UINT8	audHpVolIndex;
#endif
} ADEC_CTL_TYPE_T;

/**
 * ADEC Voluume Structure
 * dB Scale		:     main,			       fine
 *   -127dB Min 	: 0x00, (-127dB)	0x00(0dB)
 * - 125.9325 dB 	: 0x01(-126 dB), 	0x01(1/16 dB)
 * - 121.8125 dB 	: 0x05 (-122 dB),   0x03(3/16 dB)
 * -0.9375dB        : 0x7E(-1dB),       0x01(1/16dB)
 *    0 dB          : 0x7F (0 dB),      0x00(0 dB)
 *    30dB          : 0x9D (30dB),      0x00(0 dB)
 */
typedef struct ADEC_VOLUME
{
	UINT8	mainVol		;	// 1 dB step, -127 ~ +30 dB.
	UINT8	fineVol;  	// 1/16 dB step, 0dB~15/16dB
} ADEC_VOLUME_T ;


/**
 * Audio Decoder Debug Information Definition
 * Source, Type, Input Port 관련 변수 모음.
 *
 */
typedef struct	ADEC_DRV_COMMON			/* Source, Type, Input Port 관련 변수 모음 */
{
	ADEC_SRC_TYPE_T   			curAdecFormat;				/**<  Current Audio Format Type */
	ADEC_SRC_TYPE_T   			prevAdecFormat;				/**<  Previous Audio Format Type */
	ADEC_IN_PORT_T				curAdecInputPort;			/**<  Current SPDIF Output Type */
	ADEC_DUALMONO_OUTPUT_MODE_T		curAdecSPKmode;
	ADEC_MCLK_REAL_FREQUENCY_T	curAdecMCLKFreq;
	ADEC_SIF_SOUNDSYSTEM_T		curAdecSifBand;
	ADEC_SIF_EXISTENCE_INFO_T	curAdecSifIsNicam;
	ADEC_SIF_EXISTENCE_INFO_T	curAdecSifIsA2;
	ADEC_SIF_STANDARD_T			curAdecSifStandard;
	ADEC_SAMPLE_FREQ_T			curAdecHdmiSampleFreq;
	UINT8						curAdecVolume;				/**<  Current Adec Volume */
	UINT8						curADCPortNum;				/**<  Current ADC Port Number */
	BOOLEAN						bAdecStart;					/**<  Current DDI ADEC Start 여부 in the DTV Mode */
	BOOLEAN						bAdecPlay;					/**<  Current DDI ADEC Start 여부 in the other Mode*/
	BOOLEAN						bAdecMute;					/**<  Current ADEC Mute ON/OFF 여부 */
	BOOLEAN						bAdecSpdifOutPCM;			/**<  Current SPDIF Output is PCM? */
	BOOLEAN						bCurAdecSpdifOutPCM;		/**<  Current Set SPDIF Output is PCM? */
	BOOLEAN						bSRSOnOff;					/**<  Current SRS ON/OFF 여부 ? */
	BOOLEAN						bCVOnOff;					/**<  Current CV3 CV ON/OFF 여부 ? */
	BOOLEAN						bAVL2OnOff;					/**<  Current CV3 AVL2 ON/OFF 여부 ? */
	BOOLEAN						bSurroundOnOff;				/**<  Current CV3 Surround ON/OFF 여부 ? */
	BOOLEAN						bUEQOnOff;					/**<  Current CV3 UEQ ON/OFF 여부 ? */
	BOOLEAN						bHDEVOnOff;					/**<  Currnet HDEV ON/OFF여부? */ //hongsu.kim
	SINT32						curAdecI2SOutDelay;
	SINT32						curAdecSPDIFOutDelay;
	SINT32						curAdecDACOutDelay;
#ifdef MOVE_SETTING_ADEC_TO_AUDIO
	ADEC_VOLUME_T				curSpkOutVol;
	ADEC_VOLUME_T				curHpOutVol;
#if 0
	ADEC_VOLUME_T				curSpdifOutVol;
	ADEC_VOLUME_T				curScartOutVol;
#endif
	ADEC_VOLUME_T				curSpkOutGain;
	ADEC_VOLUME_T				curHpOutGain;
	ADEC_VOLUME_T				curSpdifOutGain;
	ADEC_VOLUME_T				curScartOutGain;
	ADEC_VOLUME_T				curADOutVol;

#ifdef SPK_GAIN_USE_VOL
	ADEC_VOLUME_T				curSpkOutRealVol;
#endif
#ifdef HP_GAIN_USE_VOL
	ADEC_VOLUME_T				curHpOutRealVol;
#endif
#endif
	BOOLEAN						bAdecESExist;				/**< Current ES 존재 여부 */
	UINT32						curAdecAdditionalDelay;
	ADEC_SPDIF_COPYRIGHT_T		eSpdifCpWrite;				/**< SPDIF Output SCMS(Serail Copy Management System) Copy write : FREE/ONCE/NEVER */
	UINT16						u16HDMICbit;				/**< HDMI Input Cbit(Cbit info 16bit : to set FREE/ONCE/NEVER for SPDIF Ouput */
	UINT8						curSoundBarVol;
} ADEC_DRV_COMMON_T;

/**
 * AC3 ES Info
 *
 * @see
*/
typedef struct ADEC_AC3_ES_INFO{
    UINT8 bitRate;
    UINT8 sampleRate;
    UINT8 channelNum;/* it is not fixed yet depend on mstar definition mono=0x0,Lo/Ro= 0x1,L/R/=0x2,L/R/LS/RS =0x3*/
    UINT8  EAC3;       /* AC3 0x0, EAC3 0x1*/
}ADEC_AC3_ES_INFO_T;

/**
 * MPEG ES Info
 *
 * @see
*/
typedef struct ADEC_MPEG_ES_INFO{
    UINT8 bitRate;
    UINT8 sampleRate;
    UINT8 layer;
    UINT8  channelNum; /* mono =0x0,stereo=0x1,multi-channel =0x2*/
}ADEC_MPEG_ES_INFO_T;

/**
 * HE-AAC ES Info
 *
 * @see
*/
typedef struct ADEC_HEAAC_ES_INFO{
    UINT8 version;    		  /*AAC 0x0  HE-AACv1= 0x1,HE-AACv2=0x2 */
    UINT8 transmissionformat;     /* LOAS/LATM =0x0 , ADTS=0x1*/
    UINT8 channelNum;
}ADEC_HEAAC_ES_INFO_T;

/**
 * ADEC_AUDIO_MODE
 *
 * @see
*/
typedef enum ADEC_AUDIO_MODE
{
	ADEC_AUDIO_MODE_MONO	 			= 0,
	ADEC_AUDIO_MODE_JOINT_STEREO 		= 1,
	ADEC_AUDIO_MODE_STEREO		 		= 2,
	ADEC_AUDIO_MODE_DUAL_MONO 			= 3,
	ADEC_AUDIO_MODE_MULTI				= 4,
	ADEC_AUDIO_MODE_UNKNOWN				= 5,
}ADEC_AUDIO_MODE_T;

/**
 * ADEC ES Info
 *
 * @see
*/
typedef struct ADEC_ES_INFO{
    ADEC_SRC_TYPE_T adecFormat;
    ADEC_AUDIO_MODE_T audioMode;
    ADEC_HEAAC_ES_INFO_T heAAcEsInfo;
    ADEC_MPEG_ES_INFO_T  mpegESInfo;
    ADEC_AC3_ES_INFO_T   ac3ESInfo;
}ADEC_ES_INFO_T;

/**
 * ADEC AC3/AC3+ ACMOD Type.
 *
 */
typedef  enum
{
	ADEC_AC3ACMOD_TWO_MONO_1_CH1_CH2			= 0,	/* 1+1 */
	ADEC_AC3ACMOD_ONE_CENTER_1_0_C				= 1,	/* 100 */
	ADEC_AC3ACMOD_TWO_CHANNEL_2_0_L_R			= 2,	/* 200 */
	ADEC_AC3ACMOD_THREE_CHANNEL_3_0_L_C_R		= 3,	/* 300 */
	ADEC_AC3ACMOD_THREE_CHANNEL_2_1_L_R_S		= 4,	/* 210 */
	ADEC_AC3ACMOD_FOUR_CHANNEL_3_1_L_C_R_S		= 5,	/* 310 */
	ADEC_AC3ACMOD_FOUR_CHANNEL_2_2_L_R_SL_SR	= 6,	/* 220 */
	ADEC_AC3ACMOD_FIVE_CHANNEL_3_2_L_C_R_SL_SR	= 7,	/* 320 & 321 */
} ADEC_AC3_ACMOD_T ;


/**
 * Ginga
 */
#ifdef INCLUDE_GINGA

/*
Summary:
    Digital audio formats
Description:
    Values 0, 3 and 4 are reservered for baudio_format_mpeg, even though baudio_format_mpeg == 0x3.
        3 is the value for MPEG1 audio (ISO/IEC 11172-3).
        4 is the value for MPEG2 audio (ISO/IEC 13818-3).
*/
typedef enum ADEC_BAUDIO_FORMAT{
   ADEC_baudio_format_unknown = 0,          /* unknown/not supported audio format */
   ADEC_baudio_format_mpeg = 0x3,           /* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
   ADEC_baudio_format_mp3 = 0x1,            /* MPEG1/2, layer 3. */
   ADEC_baudio_format_aac = 0xF,            /* Advanced audio coding. Part of MPEG-4 */
   ADEC_baudio_format_aac_plus = 0x11,      /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) */
   ADEC_baudio_format_aac_plus_adts = 0x12, /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) sync and LATM mux */   
   ADEC_baudio_format_aac_plus_loas = 0x11, /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
   ADEC_baudio_format_ac3 = 0x81,           /* Dolby Digital AC3 audio */
   ADEC_baudio_format_ac3_plus = 0x6,       /* Dolby Digital Plus (AC3+ or DDP) audio */
   ADEC_baudio_format_dts = 0x82,           /* Digital Digital Surround sound. */
   ADEC_baudio_format_lpcm_hddvd = 0x83,    /* LPCM, HD-DVD mode */
   ADEC_baudio_format_lpcm_bluray = 0x84,   /* LPCM, Blu-Ray mode */
   ADEC_baudio_format_dts_hd = 0x85,        /* Digital Digital Surround sound, HD */
   ADEC_baudio_format_wma_std = 0x86,       /* WMA Standard */  
   ADEC_baudio_format_wma_pro = 0x87,       /* WMA Professional */  
   ADEC_baudio_format_lpcm_dvd = 0x88,      /* LPCM, DVD mode */
   ADEC_baudio_format_avs = 0x43,           /* AVS Audio */
   ADEC_baudio_format_dra = 0xda
} ADEC_BAUDIO_FORMAT_T;

typedef  enum
{
	ADEC_FORMAT_UNKNOWN		= 0,	/**< Default, Unknown value */
	ADEC_FORMAT_PCM,				/**< PCM */
	ADEC_FORMAT_AC3,				/**< DOLBY DIGITAL : ATSC Default */
	ADEC_FORMAT_AC3_PLUS,			/**< DOLBY DIGITAL PLUS */
	ADEC_FORMAT_MPEG,				/**< MPEG 1,2 AUDIO Layer 1/2 : Europe Default */
	ADEC_FORMAT_MP3,				/**< MPEG 1,2 AUDIO Layer 3 */
	ADEC_FORMAT_AAC,				/**< AAC (MPEG2 AAC LC) : Japan Default */
	ADEC_FORMAT_HE_AAC,				/**< HE-AAC (MPEG4 HE-AAC) : Brazil Default */
	ADEC_FORMAT_DTS,				/**< DTS */
	ADEC_FORMAT_DIVX,				/**< DIVX용 Audio Format : Temporal Usage : 2008.07.05 */
	ADEC_FORMAT_MAX
} ADEC_FORMAT_T;


typedef struct ADEC_PARSE_AUDIO
{
	ADEC_FORMAT_T code;
	UINT32 bitrate;
	UINT32 samplerate;
	
} ADEC_AUDIO_INFO_T;

/**
 * ADEC Callback Status for Ginga
 *
 */
typedef enum
{
	ADEC_GINGA_NONE		= 0,
	ADEC_GINGA_EOS		= 1,	//End of Stream
	ADEC_GINGA_BOS		= 2,	//Begin of Stream
	ADEC_GINGA_ERROR	= 3,	//Stream Error
} ADEC_GINGA_MESSAGE_T;

/*
* Ginga Porting Layer Callback
*/
typedef void (*PORTINGLAYER_Callback)(void *context, int param);
#endif


/**
 * ADEC AAC ACMOD Type.
 *
 */
typedef  enum
{
	ADEC_AACACMOD_TWO_MONO_1_CH1_CH2			= 0,	/* 1+1 */
	ADEC_AACACMOD_ONE_CENTER_1_0_C				= 1,	/* 100 */
	ADEC_AACACMOD_TWO_CHANNEL_2_0_L_R			= 2,	/* 200 */
	ADEC_AACACMOD_THREE_CHANNEL_3_0_L_C_R		= 3,	/* 300 */
	ADEC_AACACMOD_THREE_CHANNEL_2_1_L_R_S		= 4,	/* 210 */
	ADEC_AACACMOD_FOUR_CHANNEL_3_1_L_C_R_S		= 5,	/* 310 */
	ADEC_AACACMOD_FOUR_CHANNEL_2_2_L_R_SL_SR	= 6,	/* 220 */
	ADEC_AACACMOD_FIVE_CHANNEL_3_2_L_C_R_SL_SR	= 7,	/* 320 & 321 */
} ADEC_AAC_ACMOD_T ;

typedef  enum
{
	ADEC_DOLBY_LINE_MODE	= 0,
	ADEC_DOLBY_RF_MODE		= 1,
	ADEC_DOLBY_DRC_OFF		= 2,
} ADEC_DOLBY_DRC_MODE_T ;

typedef  enum
{
	ADEC_LORO_MODE		= 0,
	ADEC_LTRT_MODE		= 1,
} ADEC_DOWNMIX_MODE_T ;

/**
 * ADEC LGSE PARAMETER Setting TYPE
 *
 */
typedef enum
{
	ADEC_LGSE_INIT_ONLY = 0, // "init only" parameter will be written
	ADEC_LGSE_VARIABLES = 1, // "variables" will be writen
	ADEC_LGSE_ALL		= 2  // "init only" and "variables" will be written simultaneously
} ADEC_LGSE_DATA_MODE_T ;

/**
 * ADEC LGSE PARAMETER ACCESS TYPE
 *
 */
typedef enum
{
	ADEC_LGSE_WRITE	= 0, // "pParams" data will be written to DSP
	ADEC_LGSE_READ	= 1, // "pParams" data will be read from DSP to CPU
} ADEC_LGSE_DATA_ACCESS_T ;

/**
 * ADEC LGSE FN004 MODE VARIABLE TYPE
 *
 */
typedef enum
{
	ADEC_LGSE_MODE_VARIABLES0	= 0, // "VARIABLES_00"will be written
	ADEC_LGSE_MODE_VARIABLES1	= 1, // "VARIABLES_01"will be written
	ADEC_LGSE_MODE_VARIABLES2	= 2, // "VARIABLES_02"will be written
	ADEC_LGSE_MODE_VARIABLES3	= 3, // "VARIABLES_03"will be written
	ADEC_LGSE_MODE_VARIABLES4	= 4, // "VARIABLES_04"will be written
	ADEC_LGSE_MODE_VARIABLESALL = 5  // All "VARIABLES" will be written simultaneously. Data will be arranged from 0 to 4.
} ADEC_LGSE_VARIABLE_MODE_T ;

typedef enum
{
	ADEC_LGSE_LGSEFN000	= 0,
	ADEC_LGSE_LGSEFN001	= 1,
	ADEC_LGSE_LGSEFN002	= 2,
	ADEC_LGSE_LGSEFN003	= 3,
	ADEC_LGSE_LGSEFN006	= 6,
	ADEC_LGSE_LGSEFN007	= 7,
	ADEC_LGSE_LGSEFN008	= 8,
	ADEC_LGSE_LGSEFN009	= 9,
	ADEC_LGSE_LGSEFN010	= 10,
	ADEC_LGSE_LGSEFN011	= 11,
	ADEC_LGSE_LGSEFN012	= 12,
	ADEC_LGSE_LGSEFN013	= 13,
	ADEC_LGSE_LGSEFN004MODE1	= 41,
	ADEC_LGSE_LGSEFN004MODE2	= 42,
	ADEC_LGSE_LGSEFN004MODE3	= 43,
	ADEC_LGSE_MODE = 100
} ADEC_LGSE_FUNCLIST_T ;

typedef  enum
{
	ADEC_MAIN_PATH_IDX		 =	0,	// Main TV path
	ADEC_SUB_PATH_IDX		 =	1,	// Sub TV path
} ADEC_PATH_IDX;		//Path index for 2nd TV

 typedef struct ADEC_BLUETOOTH_COMMON
 {
     UINT32 *pBufAddr;                 //user space buffer pointor
     UINT32 BufSize;                   //userspace buffer size
     ADEC_SAMPLERATE_T sampleRate;     //BT uploade data set info:sampleRate
     ADEC_AUDIO_MODE_T channelMode;    //BT uploade data set info:channelMode
 } ADEC_BT_T;

 typedef struct ADEC_BT_DATA
 {
    UINT32	channel;	// Encoder index
	UINT64	pts;		// PTS
	UINT32	*pData;		// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
	UINT32	*pRStart;	// start pointer of buffer
	UINT32	*pREnd;		// end pointer of buffer
 }ADEC_BT_DATA_T;
 typedef  enum
 {
	 ADEC_SIF_MAIN_PATH_IDX 	  =  0,  // Main tuner SIF use case
	 ADEC_SIF_SUB_PATH_IDX		  =  1,  // Sub tuner  SIF use case , recording source use
 } ADEC_SIF_PATH_IDX;		 //Path index for 2nd TV

typedef enum
{
    ADEC_SOUNDBAROT_VOLUME_FIX_DELAY = 0,     //soundbar thread 20ms
    ADEC_SOUNDBAROT_VOLUME_VARIABLE_DELAY = 1,//soundbar thread 5ms		
}ADEC_SOUNDBAROT_THREAD_DELAY;

 typedef DTV_STATUS_T (*pfnBTDataHandlingCB)(ADEC_BT_DATA_T *pMsg);
 typedef int (*pfnSendPCMCB)(UINT8 *buf, UINT16 len);


 /* HDMI Audio CBIT mask */
#define ADEC_CBIT_16BIT_MASK				(0xffff)
#define ADEC_CBIT_COPYRIGHT_MASK			(0x1)
#define ADEC_CBIT_CATEGORY_CODE_MASK_0_2	(0x7)
#define ADEC_CBIT_CATEGORY_CODE_MASK_0_6	(0x7f)
#define ADEC_CBIT_CATEGORY_CODE_MASK_3		(0x8)
#define ADEC_LBIT_MASK						(0x80)
/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/

/*------ ADEC DDI Functions -------------------------------------------------------------*/
DTV_STATUS_T DDI_ADEC_InitializeModule(void);
DTV_STATUS_T DDI_ADEC_SetSource(ADEC_PATH_IDX nPathIdx, ADEC_IN_PORT_T adecSource, UINT8 portNum, ADEC_SRC_TYPE_T adecType);

DTV_STATUS_T DDI_ADEC_GetLGSEData(ADEC_LGSE_FUNCLIST_T funcList, ADEC_LGSE_DATA_ACCESS_T rw, UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);

DTV_STATUS_T DDI_ADEC_LGSEFN000(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init : 17, Var:3
DTV_STATUS_T DDI_ADEC_LGSEFN001(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init :  0, Var:18
DTV_STATUS_T DDI_ADEC_LGSEFN002(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init : 13, Var:11
DTV_STATUS_T DDI_ADEC_LGSEFN003(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init :330, Var:0
DTV_STATUS_T DDI_ADEC_LGSEFN006(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init : 10, Var:0
DTV_STATUS_T DDI_ADEC_LGSEFN007(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init :  5, Var:2
DTV_STATUS_T DDI_ADEC_LGSEFN008(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);		// init :119, Var:7
DTV_STATUS_T DDI_ADEC_LGSEFN009(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init :404, Var:2
DTV_STATUS_T DDI_ADEC_LGSEFN004Mode1(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption,
	                                                          ADEC_LGSE_VARIABLE_MODE_T varOption);	// init :  0, Var:5 X 5
DTV_STATUS_T DDI_ADEC_LGSEFN004Mode2(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);	// init :  0, Var:15
DTV_STATUS_T DDI_ADEC_LGSEFN004Mode3(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);	// init : 35, Var:0
DTV_STATUS_T DDI_ADEC_LGSEFN010(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_ACCESS_T dataOption);			// init : 10, Var:2
DTV_STATUS_T DDI_ADEC_LGSEFN011(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);		// init :  3, Var:63
DTV_STATUS_T DDI_ADEC_LGSEFN012(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);		// init :  4, Var:2
DTV_STATUS_T DDI_ADEC_LGSEFN013(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);		// init :  5, Var:0

DTV_STATUS_T DDI_ADEC_LGSEMode(UINT32 *pParams, UINT16 noParam, ADEC_LGSE_DATA_MODE_T dataOption);			// init :  0, Var:3

#ifndef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetVolume(UINT8  volume) ;
#else
DTV_STATUS_T DDI_ADEC_SetVolume(ADEC_VOLUME_T  volume) ;
#endif
DTV_STATUS_T DDI_ADEC_SetBalance(UINT8 balance);
DTV_STATUS_T DDI_ADEC_SetBass(UINT8 bass) ;
DTV_STATUS_T DDI_ADEC_SetTreble(UINT8   treble) ;

#ifdef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetDACOutVolume(UINT8 portNum, ADEC_VOLUME_T volume, BOOLEAN bForced);
#else
DTV_STATUS_T DDI_ADEC_SetDACOutVolume(UINT8 portNum, UINT16 volume, BOOLEAN bForced);
#endif


#ifdef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetADVolume(ADEC_VOLUME_T volume);
#else
DTV_STATUS_T DDI_ADEC_SetADVolume(UINT8 volume);
#endif

DTV_STATUS_T DDI_ADEC_SetSPDIFOutputType(ADEC_SPDIF_MODE_T eSpdifMode,BOOLEAN bForced);
DTV_STATUS_T DDI_ADEC_SetSPDIFSCMS(ADEC_SPDIF_COPYRIGHT_T copyRight);
DTV_STATUS_T DDI_ADEC_SetSPDIFMute(BOOLEAN bOnOff);
DTV_STATUS_T DDI_ADEC_SetAudioReturnChannel(BOOLEAN bOnOff);
DTV_STATUS_T DDI_ADEC_SetDualMonoOutMode(ADEC_PATH_IDX nPathIdx, ADEC_DUALMONO_OUTPUT_MODE_T outputMode);
DTV_STATUS_T DDI_ADEC_SetDACOutMute(UINT8 portNum, BOOLEAN bOnOff);
DTV_STATUS_T DDI_ADEC_GetHDMIAudioMode(ADEC_HDMI_TYPE_T *pHdmiMode);

DTV_STATUS_T DDI_ADEC_SetI2SOutDelayTime( SINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T DDI_ADEC_SetSPDIFOutDelayTime( SINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T DDI_ADEC_SetDACOutDelayTime( SINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T DDI_ADEC_SetUserAnalogMode(ATV_AUDIO_MODE_SET_T eMode);
DTV_STATUS_T DDI_ADEC_GetCurAnalogMode(ATV_AUDIO_MODE_GET_T *pATVAudioMode);
DTV_STATUS_T DDI_ADEC_GetBtscA2StereoLevel(UINT16 *pLevel);
DTV_STATUS_T DDI_ADEC_SetBtscA2ThresholdLevel(UINT16 thrLevel);

DTV_STATUS_T DDI_ADEC_GetESInfo(ADEC_PATH_IDX nPathIdx, ADEC_ES_INFO_T *pAudioESInfo);
DTV_STATUS_T DDI_ADEC_SetHDEVMode(BOOLEAN bOnOff);
DTV_STATUS_T DDI_ADEC_SIF_SetBandSetup(ADEC_SIF_SOUNDSYSTEM_T sifBand);

DTV_STATUS_T DDI_ADEC_SIF_GetBandDetect(ADEC_SIF_SOUNDSYSTEM_T soundSystem, UINT32 *pBandStrength, BOOLEAN doubleChecked);

DTV_STATUS_T DDI_ADEC_SIF_SetModeSetup(ADEC_SIF_STANDARD_T sifStandard);
DTV_STATUS_T DDI_ADEC_SIF_CheckNicamDigital(ADEC_SIF_EXISTENCE_INFO_T *pIsNicamDetect);
DTV_STATUS_T DDI_ADEC_SIF_CheckAvailableSystem(ADEC_SIF_AVAILE_STANDARD_T standard,
														ADEC_SIF_EXISTENCE_INFO_T *pAvailability);
DTV_STATUS_T DDI_ADEC_SIF_CheckA2DK( ADEC_SIF_STANDARD_T standard, ADEC_SIF_EXISTENCE_INFO_T *pAvailability );

#ifdef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetATVOutVolume(void);
#else
DTV_STATUS_T DDI_ADEC_SetATVOutVolume(void);
#endif
#ifndef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetSPDIFVolume(UINT8 volume);
#else
DTV_STATUS_T DDI_ADEC_SetSPDIFVolume(ADEC_VOLUME_T volume);
#endif
DTV_STATUS_T DDI_ADEC_SetBluetoothVolume(ADEC_VOLUME_T volume);
#ifndef MOVE_SETTING_ADEC_TO_AUDIO
DTV_STATUS_T DDI_ADEC_SetVolumeAsInput(ADEC_GAIN_INDEX_T index);
DTV_STATUS_T DDI_ADEC_InitToolOptionParameter(const ADEC_CTL_TYPE_T * audCtlIndex);
#endif

DTV_STATUS_T DDI_ADEC_GetAudioPTS(UINT32 *pPts);

DTV_STATUS_T DDI_ADEC_GetAdecStatus(ADEC_DRV_COMMON_T *pAdecDriverInfo);
DTV_STATUS_T DDI_ADEC_GetDecodingType(ADEC_PATH_IDX nPathIdx, ADEC_SRC_TYPE_T *pAdecType);
DTV_STATUS_T DDI_ADEC_StartDecoding(ADEC_PATH_IDX nPathIdx, ADEC_SRC_TYPE_T adecType);
DTV_STATUS_T DDI_ADEC_StopDecoding (ADEC_PATH_IDX nPathIdx);
DTV_STATUS_T DDI_ADEC_SetAudSyncMode(ADEC_PATH_IDX nPathIdx, UINT8 onOff);

DTV_STATUS_T DDI_ADEC_StartAudioDescription(ADEC_SRC_TYPE_T adType);
DTV_STATUS_T DDI_ADEC_StopAudioDescription(void);

DTV_STATUS_T DDI_ADEC_PlayAudioClip(UINT32 bufSize,void *pClipBufferPointer,UINT32 repeatNumber, ADEC_SRC_TYPE_T clipType, BOOLEAN bMix, void (* pfnAclipDone)(void));
DTV_STATUS_T DDI_ADEC_StopAudioClip(void);
DTV_STATUS_T DDI_ADEC_PauseAudioClip(void);
DTV_STATUS_T DDI_ADEC_ResumeAudioClip(void);
DTV_STATUS_T DDI_ADEC_SetI2SOutVolume(ADEC_VOLUME_T  volume);
DTV_STATUS_T DDI_ADEC_SetHPOutVolume(ADEC_VOLUME_T  volume);
DTV_STATUS_T DDI_ADEC_SetSCARTOutVolume(ADEC_VOLUME_T  volume);
DTV_STATUS_T DDI_ADEC_SetSPDIFOutVolume(ADEC_VOLUME_T  volume);
DTV_STATUS_T DDI_ADEC_SetInputGain(ADEC_VOLUME_T  volume);

BOOLEAN DDI_ADEC_IsSIFExist(void);
BOOLEAN DDI_ADEC_IsESExist(ADEC_PATH_IDX nPathIdx);
DTV_STATUS_T DDI_ADEC_SetDolbyDRCMode(ADEC_PATH_IDX nPathIdx, ADEC_DOLBY_DRC_MODE_T drcMode);
DTV_STATUS_T DDI_ADEC_SetDownMixMode(ADEC_PATH_IDX nPathIdx, ADEC_DOWNMIX_MODE_T downmixMode);

#if 0
DTV_STATUS_T DDI_ADEC_Sub_SetSource(ADEC_IN_SUB_PORT_T adecSource, UINT8 portNum, ADEC_SRC_TYPE_T adecType);
DTV_STATUS_T DDI_ADEC_Sub_StartDecoding(ADEC_SRC_TYPE_T adecType);
DTV_STATUS_T DDI_ADEC_Sub_StopDecoding ( void );
DTV_STATUS_T DDI_ADEC_Sub_SetAudSyncMode(UINT8 onOff);
DTV_STATUS_T DDI_ADEC_Sub_SetDualMode(ADEC_DUALMONO_OUTPUT_MODE_T outputMode);
DTV_STATUS_T DDI_ADEC_Sub_GetESInfo(ADEC_ES_INFO_T *pAudioESInfo);
BOOLEAN      DDI_ADEC_Sub_IsESDecoderable(void);
#endif

DTV_STATUS_T DDI_ADEC_GetNicamThresholdLevel(UINT16 *pLevel);
DTV_STATUS_T DDI_ADEC_SetNicamThresholdLevel(UINT16 thrLevel);

#if 0
DTV_STATUS_T DDI_ADEC_PlayPCM_open(void);
DTV_STATUS_T DDI_ADEC_PlayPCM_close(void);
DTV_STATUS_T DDI_ADEC_PlayPCM_ioctl(UINT32 u4SampleRate, UINT8 u1StereoOnOff, UINT8 u1BitDepth, UINT32 u4BufferSize);
DTV_STATUS_T DDI_ADEC_PlayPCM_write(UINT32 u4BufPtr, UINT32 u4BufSize);
#endif
DTV_STATUS_T DDI_ADEC_GetHDMICbit(ADEC_SPDIF_COPYRIGHT_T *copy_info);

DTV_STATUS_T DDI_ADEC_SetSoundBarOnOff(BOOLEAN set);
#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_ADEC_SetSoundBarIDData(UINT8 ID,UINT8 Data,ADEC_SOUNDBAROT_THREAD_DELAY volumeInfo);
DTV_STATUS_T DDI_ADEC_GetSoundBarStatus(UINT8 subframeNum, UINT8 *psubframeID,UINT8 *psubframeData); 
#else
DTV_STATUS_T DDI_ADEC_SetSoundBarIDData(UINT32 ID,UINT8 data);
DTV_STATUS_T DDI_ADEC_GetSoundBarStatus(UINT32 *psubframeID,UINT8 *psubframeData,UINT8 *psubframeChecksum); 
#endif

#if (PLATFORM_TYPE == LG_PLATFORM)
DTV_STATUS_T DDI_ADEC_ConnectOutputPort(ADEC_PATH_IDX nPathIdx, ADEC_OUTPUT_MASK_T outPort);
#endif
DTV_STATUS_T DDI_ADEC_SetMixPCMVolume(ADEC_VOLUME_T volume);

//#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_ADEC_RegSendPCMCallback(pfnSendPCMCB pfnCallBack);
DTV_STATUS_T DDI_ADEC_StartUploadPCM(void);
DTV_STATUS_T DDI_ADEC_StopUploadPCM(void);
//#endif

#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_ADEC_SIFAdcSel(ADEC_SIF_PATH_IDX SIFSel);
#endif
DTV_STATUS_T DDI_ADEC_SetBluetoothVolume(ADEC_VOLUME_T volume);
#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_ADEC_SetBluetoothCnt(UINT8 cnt);//for bluetooth, 1cnt takes 5.3ms
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ADEC_DDI_H_ */

