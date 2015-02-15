/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file audio_ddi.h
 *
 *  AUDIO DD를 제어하는 DPI 함수에 대한 header file.
 *  AUDIO API와 AUDIO DD를 연결하는 함수들로 주로 AUDIO Menu및 source 설정과 관련된
 *  함수들로 이루어져 있음.
 *
 *  @author		Park Jeong-Gun(alwaysok@lge.com)
 *  @version	1.0
 *  @date		2006.04.20
 *  @note
 *  @see		audio_ddi.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _AUDIO_DDI_H_
#define _AUDIO_DDI_H_

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#ifndef WIN32_SIM // Modified by dilly97: fix include
#include "common.h"
#else
#include "common_win32.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "global_configurations.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

#define USE_TOOL_OPTION
//#define MOVE_SETTING_ADEC_TO_AUDIO
#define USE_SEVOLUME_TO_MAIN_VOLUME
#define USE_WOOFER_ONOFF



#define AUD_VOL_MAX	(100)

// stonedef - 090911
// Lip sync Delay - temporary
#define AUD_USER_DELAY_MIN	(-10)
#define AUD_USER_DELAY_MAX	(20)

#ifndef USE_CUR_USER_DELAY
#define	USE_CUR_USER_DELAY ((SINT8)(0x80))
#define	USE_CUR_CONSTANT_DELAY_TIME	(0xFFFFFFFF)
#define	CHANGE_CUR_USER_DELAY_DB	(0xF0F0F0F0)
#define	CHANGE_CUR_MVD_DELAY_DB		(0xF0000000)
#endif

//lip_temp
#define USE_LIPSYNC_BY_RESOL

//lip_temp 
#define USE_LIPSYNC_BY_3D_L9

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
/**
 * Audio SPDIF Information Definition
 *
 */
typedef struct AUDIO_SPDIF_INFO
{
	UINT8   spdifInOnOff	:   1;      /**<   SPDIF Input을 가지는 소스인지 여부 나타냄  */
	UINT8   spdifInExist	:   1;      /**<   SPDIF Input의 존재 여부를 나타냄           */
	UINT8   spdifInType		:   2;      /**<   SPDIF Input이 AC3/PCM/NONE의 여부를 나타냄 */
	UINT8   spdifDemOnOff	:   1;      /**<   SPDIF Input의 Preemphasis여부를 나타냄     */
	UINT8   spdifDviSet		:   1;		/**<   SPDIF Input의 DVI Mode 환경설정 여부를 나타냄 */
	UINT8	spdifHdmiSet	:	1;		/**<   SPDIF Input의 HDMI Mode 환경설정 여부를 나타냄 */
}   AUDIO_SPDIF_INFO_T;

/**
 * AUDIO SPDIF Copy Protection Type
 *
 */
typedef  enum
{
	AUDIO_SPDIF_COPY_FREE		= 0,	/* cp-bit : 1, L-bit : 0 */
	AUDIO_SPDIF_COPY_NO_MORE	= 1,	/* cp-bit : 0, L-bit : 1 */
	AUDIO_SPDIF_COPY_ONCE		= 2,	/* cp-bit : 0, L-bit : 0 */
	AUDIO_SPDIF_COPY_NEVER		= 3,	/* cp-bit : 0, L-bit : 1 */
} AUDIO_SPDIF_COPYRIGHT_T ;

/**
 * Audio Stage Information Definition
 *
 */
typedef struct AUDIO_SET_STAGE_INFO
{
	UINT8   init        			:   1;  /**<  Initialize 여부 */
	UINT8	init_sound_engine		:	1;	/**<  Sound Engine Initialize 여부 */
	UINT8   first_set_source		:   1;	/**<  최초 Audio Set Source 여부 */
	UINT8   set_source  			:   1;	/**<  Audio Source change 상태 */
	UINT8   startDecoding  			:   1;	/**<  Audio Decoding 상태 */
	UINT8   startADDecoding  		:   1;	/**<  Audio AD Decoding 상태 */
}   AUDIO_SET_STAGE_INFO_T;

/**
 * Audio Sampling Frequency Definition
 *
 */
typedef enum    AUDIO_SAMPLING_FREQUENCY
{
	F96K,	/* 96KHz */
	F48K,  /* 48KHz    */
	F44K,  /* 44.1KHz  */
	F32K,  /* 32KHz    */
	F24K,  /* 24KHz	   */
	F22K,  /* 22.05KHz */
	F16K,	/* 16KHz	*/
	F12K,  /* 12KHz    */
	F11K,	/* 11.025KHz */
	F8K,	/* 8KHz		*/
} AUDIO_SAMPLING_FREQUENCY_T;

/**
 * Audio Master Clock Frequency Definition
 *
 */
typedef enum
{
	AUDIO_F12_288MHZ,
	AUDIO_F24_576MHZ,
	AUDIO_F18_432MHZ,
	AUDIO_F8_192MHZ,
	AUDIO_F16_384MHZ,
	AUDIO_F2_048MHZ,
	AUDIO_F3_072MHZ,
	AUDIO_F4_096MHZ,
	AUDIO_F6_144MHZ,
} AUDIO_MCLK_REAL_FREQUENCY_T;

/**
 * Audio Channel Definition
 *
 */
typedef enum    AUDIO_CHANNEL_INFO
{
	ADEC_CH_0,  /* Channel 0 : Main Audio Channel */
	ADEC_CH_1,  /* Channel 1 : Sub Audio Channel */
} AUDIO_CHANNEL_INFO_T;

/**
 * Audio SPDIF Stream Type Definition
 *
 */
typedef enum    SPDIF_STREAM
{
	SPDIF_PCM,
	SPDIF_AC3,
	SPDIF_DTS,
	SPDIF_NONE,
} SPDIF_STREAM_T;

/**
 * Audio Source Type Definition
 *
 */
typedef  enum  AUDIO_SOURCE
{
	AUD_SRC_DTV			= 0,
	AUD_SRC_ATV			= 1,
	AUD_SRC_AV			= 2,
	AUD_SRC_COMP		= 3,
	AUD_SRC_RGB			= 4,
	AUD_SRC_HDMI		= 5,
	AUD_SRC_DVI			= 6,
	AUD_SRC_EMF			= 7,
	AUD_SRC_BOOT		= 8,
	AUD_SRC_BLUETOOTH	= 9,
	AUD_SRC_MEDIASHARE	= 10,
	AUD_SRC_BB			= 11,
	AUD_SRC_FLASH		= 12,
	AUD_SRC_PICWIZARD	= 13,	// Picture Wizard
	AUD_SRC_SUPPORT		= 14,
	AUD_SRC_PLAY_HDD	= 15,
	AUD_SRC_INVALID		= 16
}  AUDIO_SOURCE_T ;

//#ifdef MOVE_SETTING_ADEC_TO_AUDIO
/**
 * Audio Source Delay Index Definition
 *
 */
typedef  enum  AUDIO_DELAY_SRC_INDEX
{
	AUD_DELAY_SRC_DTV		= 0,
	AUD_DELAY_SRC_ATV,
	AUD_DELAY_SRC_AV1,
	AUD_DELAY_SRC_AV2,
	AUD_DELAY_SRC_AV3,
	AUD_DELAY_SRC_COMP1,
	AUD_DELAY_SRC_COMP2,
	AUD_DELAY_SRC_COMP3,
	AUD_DELAY_SRC_RGB,
	AUD_DELAY_SRC_HDMI1,
	AUD_DELAY_SRC_HDMI2,
	AUD_DELAY_SRC_HDMI3,
	AUD_DELAY_SRC_HDMI4,
	AUD_DELAY_SRC_EMF_PHOTO_MUSIC,
	AUD_DELAY_SRC_EMF_MOVIE,
	AUD_DELAY_SRC_BLUETOOTH,
#if 0
	AUD_DELAY_SRC_MEDIASHARE,
	AUD_DELAY_SRC_BB,
	AUD_DELAY_SRC_FLASH,
	AUD_DELAY_SRC_PICWIZARD,	// Picture Wizard
#endif
	AUD_DELAY_SRC_PLAY_HDD,
	AUD_DELAY_SRC_DEFAULT,
	AUD_DELAY_SRC_INDEX_MAX
}  AUDIO_DELAY_SRC_INDEX_T;
//#endif

#ifdef MOVE_SETTING_ADEC_TO_AUDIO
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
}  AUDIO_GAIN_INDEX_T;
#endif

/**
* AUDIO Volume Curve for User selection
*
*/
typedef enum
{
	AUD_USER_VOLUME_CVR_LOW		= 0,
	AUD_USER_VOLUME_CVR_MEDIUM	= 1,
	AUD_USER_VOLUME_CVR_HIGH		= 2,
	AUD_USER_VOLUME_CVR_NUM
}AUDIO_USER_VOLUME_CURVE_T;

/**
 * Audio Input source Information
 *
 */
typedef struct AUDIO_SOURCE_INFO
{
	UINT8	srcType;	/** source type */
	UINT8	index;		/** source index */
	UINT8 	Attr;		/** source Attribution */
} AUDIO_SOURCE_INFO_T;

/**
 * Audio Volume Information Definition
 *
 */
typedef  struct  AUDIO_VOLUME
{
	UINT8           left;			/**< left volume  */
	UINT8           right;		/**< right volume  */
	UINT8           center;		/**< center volume  */
	UINT8           leftsur;		/**< left surround volume  */
	UINT8           rightsur;		/**< right surround volume  */
	UINT8           subwoofer;	/**< subwoober volume  */
	UINT8           balance;		/**< balance value  */
	UINT8           fade;		/**< fade value */
	UINT8           mask;		/**< mask  */

#define AUDIO_VOLUME_MASK_LEFT          0x80	/**< left volume mask  */
#define AUDIO_VOLUME_MASK_RIGHT         0x40	/**< right volume mask  */
#define AUDIO_VOLUME_MASK_CENTER        0x20	/**< center volume mask  */
#define AUDIO_VOLUME_MASK_LEFTSUR       0x10	/**< left surround volume mask  */
#define AUDIO_VOLUME_MASK_RIGHTSUR      0x08	/**< right surround volume mask  */
#define AUDIO_VOLUME_MASK_SUBWOOFER     0x04	/**< subwoober volume mask  */
#define AUDIO_VOLUME_MASK_BALANCE       0x02	/**< balance mask  */
#define AUDIO_VOLUME_MASK_FADE          0x01	/**< fade mask  */

}  AUDIO_VOLUME_T ;

#if 0 /* ADEC 단만 사용하도록 변경. */
/**
 * Audio 2CH Volume & Gain
 *
 */
typedef struct AUDIO_2CH_VOLUME
{
	SINT8		integer_volume;	/* 1 dB step */
	UINT8		fine_volume;	/* 1/16 dB step */
} AUDIO_2CH_VOLUME_T;
#endif

/**
 * Audio Surround Type Definition
 *
 */
typedef enum AUDIO_SURROUND_EFFECT
{
	DDI_SURROUND_OFF    = 0,
	DDI_SURROUND_CV,
	DDI_SURROUND_INFINITESOUND
} AUDIO_SURROUND_EFFECT_T ;

/**
 * Audio SPDIF Output Type Definition
 *
 */
typedef enum AUDIO_SPDIF_OUTPUT_MODE
{
	SPDIF_OUTPUT_AUTO,
	SPDIF_OUTPUT_AC3,
	SPDIF_OUTPUT_AAC,
	SPDIF_OUTPUT_PCM
} AUDIO_SPDIF_OUTPUT_MODE_T ;

/**
 * Audio HDMI Input Type Definition
 * heeowen: 20090331
 */
typedef enum AUDIO_HDMI_INPUT_MODE
{
	HDMI_INPUT_UNKNOW = 0,
	HDMI_INPUT_PCM	 = 1,
	HDMI_INPUT_AC3	 = 2,
	HDMI_INPUT_DTS	 = 3,

}AUDIO_HDMI_INPUT_MODE_T;

/**
 * AUDIO SIF Mode.
 *
 */
typedef enum
{
	AUDIO_SIF_MODE_DETECT	= 0,
	AUDIO_SIF_BG_NICAM		= 1,
	AUDIO_SIF_BG_FM			= 2,
	AUDIO_SIF_BG_A2			= 3,
	AUDIO_SIF_I_NICAM		= 4,
	AUDIO_SIF_I_FM			= 5,
	AUDIO_SIF_DK_NICAM		= 6,
	AUDIO_SIF_DK_FM			= 7,
	AUDIO_SIF_DK1_A2		= 8,
	AUDIO_SIF_DK2_A2		= 9,
	AUDIO_SIF_DK3_A2		= 10,
	AUDIO_SIF_L_NICAM		= 11,
	AUDIO_SIF_L_AM			= 12,
	AUDIO_SIF_MN_A2			= 13,
	AUDIO_SIF_MN_BTSC		= 14,
	AUDIO_SIF_MN_EIAJ		= 15,
	AUDIO_SIF_STD_END		= 16,	/* 20080724 : Added by Goldman for FE/TU */
	AUDIO_SIF_STD_UNKNWON 	= 0xF0	/* 20080724 : Added by Goldman for FE/TU */
} AUDIO_SIF_STANDARD_T;

/**
 * Analog Audio NICAM Detection Type Definition
 *
 */
typedef enum ATV_AUDIO_NICAM_SIGNATURE
{
	ATV_AUDIO_NICAM_ABSENT		= 0x00,
	ATV_AUDIO_NICAM_PRESENT		= 0x01,
	ATV_AUDIO_NICAM_DETECTING	= 0x02
} ATV_AUDIO_NICAM_SIGNATURE_T;

/**
 * Speaker Output Type Definition
 *
 */
typedef enum
{
	AUDIO_MODE_MONO	 			= 0,
	AUDIO_MODE_JOINT_STEREO 	= 1,
	AUDIO_MODE_STEREO		 	= 2,
	AUDIO_MODE_DUAL_MONO 		= 3,
	AUDIO_MODE_MULTI			= 4,
	AUDIO_MODE_UNKNOWN			= 5,
} AUDIO_STEREO_MODE_T;

/**
 * Speaker Output Type Definition
 *
 */
typedef enum
{
	AUDIO_SPK_MODE_LR		= 0,
	AUDIO_SPK_MODE_LL		= 1,
	AUDIO_SPK_MODE_RR		= 2,
	AUDIO_SPK_MODE_MIX		= 3
} AUDIO_SPK_OUTPUT_MODE_T;

/**
 * Audio Decoder Information Definition
 *
 */
typedef struct AUDIO_INFO
{
	BOOLEAN					bDecodingStart;	/**< Status of Audio decoder */
	AUDIO_SRC_TYPE_T		decoderType;	/**< Audio Decoder Type */
	AUDIO_STEREO_MODE_T		stereoMode;		/**< Stereo Mode */
}  AUDIO_INFO_T;

/**
 * Analog Audio Check Status Type Definition
 *
 */
typedef enum
{
	AUDIO_SIF_CHECK_STATUS_OFF				= 0, // Initial Status
	AUDIO_SIF_CHECK_STATUS_DETECT_NICAM		= 1, // Detection Process for Nicam
	AUDIO_SIF_CHECK_STATUS_DETECT_A2		= 2, // Detection Process for A2
	AUDIO_SIF_CHECK_STATUS_DETECT_A2DK3		= 3, // Detection Process for DK3
	AUDIO_SIF_CHECK_STATUS_DETECT_FM		= 4, // Detection Process for FM(AM)
	AUDIO_SIF_CHECK_STATUS_DONE				= 5  // Detection Completed
}AUDIO_SIF_CHECK_STATUS_T;

/**
 * Audio Decoder Debug Information Definition
 * Currently Not Used
 *
 */
typedef  struct  AUDIO_DEBUG_INFO
{
	/* AC3 status modes */
	UINT8 ac3_fscod;			/**< FS Code */
	UINT8 ac3_bitratecode;		/**< bitrate code */
	UINT8 ac3_lfe;				/**< low frequency effect */
	UINT8 ac3_acmod;			/**< audio coding mode */
	UINT8 ac3_bsmod;			/**< bs mode */
	UINT8 ac3_bsid;				/**< bs id */
	UINT8 ac3_dsurmod;			/**< surround mode */
	UINT8 ac3_copyright;		/**< copyright */
	UINT8 ac3_origbs;			/**< origbs */
	UINT8 ac3_lancode;			/**< language code */
	UINT8 ac3_langcod;			/**< langcod */
	UINT8 ac3_roomType;			/**< room type */
	UINT8 ac3_mixlevel;			/**< mix level */
	UINT8 ac3_audprodie;		/**< audprodie */

	/* MPEG status mode */
	UINT8 mpeg_bri;     /**<  Bit rate index */
	UINT8 mpeg_p;       /**<  Protection Bit */
	UINT8 mpeg_lay;     /**<  Layer */
	UINT8 mpeg_id;      /**<  Identifier */
	UINT8 mpeg_sfr;     /**<  Sampling Frequency */
	UINT8 mpeg_pad;     /**<  Padding Bit */
	UINT8 mpeg_pri;     /**<  Private Bit */
	UINT8 mpeg_mod;     /**<  Mode */
	UINT8 mpeg_mex;     /**<  Mode Extension */
	UINT8 mpeg_c;       /**<  Coryright */
	UINT8 mpeg_ocb;     /**<  Original/Copy Bit */
	UINT8 mpeg_emp;     /**<  Emphasis rate index */
	UINT8 mpeg_cen;     /**<  Centre */
	UINT8 mpeg_sur;     /**<  Surround */
	UINT8 mpeg_lfe;     /**<  LFE */
	UINT8 mpeg_amx;     /**<  Audio mix */
	UINT8 mpeg_dem;     /**<  Dematrix procedure */
	UINT8 mpeg_ext;     /**<  Extension bitstream present */
	UINT8 mpeg_nml;     /**<  Number of Multi-lingual Channels */
	UINT8 mpeg_mfs;     /**<  Multi-lingual FS */
	UINT8 mpeg_mly;     /**<  Multi-lingual Layer */
	UINT8 mpeg_cib;     /**<  Copyright ID Bit */
	UINT8 mpeg_cis;     /**<  Copyright ID Start */
}  AUDIO_DEBUG_INFO_T ;

/**
 * Audio Transport Stream Input Port Definition
 * Currently Not Used
 *
 */
typedef enum	AUDIO_TP_INSEL
{
	AUDIO_INTTP,
	AUDIO_EXTTP,
	AUDIO_NONTP
} AUDIO_TP_INSEL_T;

/**
 * Analog Audio SIF Type Definition
 *
 */
typedef enum	AUDIO_SIF_TYPE_SEL
{
	MAIN_SIF	=0,
	SUB_SIF
} AUDIO_SIF_TYPE_SEL_T;

/**
 * PVC Setting Mode Type Definition
 *
 */
typedef enum AUDIO_PVC_MODE
{
	AUDIO_PVC_OFF		= 0,	/* PVC Mode Off */
	AUDIO_PVC_NORMALL	= 1,	/* PVC Mode for normal input */
	AUDIO_PVC_HALF		= 2,	/* PVC Mode for mp3 input */
	AUDIO_PVC_BB		= 3,	/* PVC Mode for Netflix/Yahoo/Youtube input */
	AUDIO_PVC_ATV		= 4,	/* PVC Mode for ATV */
	AUDIO_PVC_UNDEFINE
} AUDIO_PVC_MODE_T;

/**
 * Audio Sound Mode Definition
 * STD, MUSIC, MOVIE, SPORTS, GAME
 *
 */
typedef enum
{
	AUDIO_EZ_SOUND_STANDARD = 0,
	AUDIO_EZ_SOUND_MUSIC,
	AUDIO_EZ_SOUND_MOVIE,
	AUDIO_EZ_SOUND_SPORTS,
	AUDIO_EZ_SOUND_GAME,
	AUDIO_EZ_SOUND_VIVID,
	AUDIO_EZ_SOUND_CUSTOM,
	AUDIO_EZ_SOUND_DEFAULT = AUDIO_EZ_SOUND_STANDARD,
} AUDIO_EZ_SOUND_T;

/**
 * Audio Room Mode Definition
 * STD, WALLMOUNT, STAND
 *
 */
typedef enum
{
	AUDIO_CV_ROOMEQ_STANDARD = 0,
	AUDIO_CV_ROOMEQ_WALMOUNT,
	AUDIO_CV_ROOMEQ_STAND,
	AUDIO_CV_ROOMEQ_DEFAULT = AUDIO_CV_ROOMEQ_STANDARD,
} AUDIO_CV_ROOMEQ_T;


/**
 * Headphone Source Type Definition
 *
 */
typedef enum AUDIO_HP_SOURCE
{
	AUDIO_HP_SRC_BEFORE_PP = 0,
	AUDIO_HP_SRC_AFTER_PP = 1,
} AUDIO_HP_SOURCE_T;

/**
 * EMF attribute definitions
 *
 */
typedef enum AUDIO_EMF_SOURCE_ATTR
{
	AUD_EMF_ATTR_PHOTO_MUSIC		= 0,
	AUD_EMF_ATTR_MOVIE,

} AUDIO_EMF_SOURCE_ATTR_T;

#if 0	/* OSA Audio AMP Type을 이용하도록 변경함. */
/**
 * AMP Type Definition
 *
 */
typedef enum AUDIO_AMPCHIP_TYPE
{
	AUDIO_AMP_NTP7000	= 0,
	AUDIO_AMP_NTP7400	= 1,
	AUDIO_AMP_NTP2AMP	= 2,		/* NTP2AMP	*/
#if 0
	AUDIO_AMP_NTP3AMP	= 3,
#endif
	AUDIO_AMP_NTPSNDBAR	= 3,

	AUDIO_AMP_TASSNDBAR	= 4,
#if 0
	AUDIO_AMP_TAS5713	= 5,
#endif
	AUDIO_AMP_TAS5727	= 5,
#if 0
	AUDIO_AMP_TAS3AMP	= 6,
#endif
	AUDIO_AMP_TAS2AMP	= 6,

	AUDIO_AMP_STA368	= 7,
	AUDIO_AMP_TAS5709	= 8,

	AUDIO_AMP_UNKOWN	= 0xff,
} AUDIO_AMPCHIP_TYPE_T;
#endif

/**
 * Audio Amp Chip Status
 *
 */
typedef enum AUDIO_AMP_STATUS
{
	AUD_AMP_NORMAL,
	AUD_AMP_MUTE,	/* Soft Mute */
	AUD_AMP_ABNORMAL,
	AUD_AMP_PWM_OUT_MUTE,
	AUD_AMP_PLL_UNLOCK,
	AUD_AMP_MCLK_INCORRECT,

}AUDIO_AMP_STATUS_T;

/**
 * AUDIO Clip Play Status.
 *
 */
typedef enum
{
	AUDIO_CLIP_NONE			= 0,	// After Power On, Clip PLay is nver started.
	AUDIO_CLIP_PLAY			= 1,	// Clip is playing.
	AUDIO_CLIP_PAUSE		= 2,	// Clip is pausing.
	AUDIO_CLIP_DONE			= 3		// Clip Play is done.
} AUDIO_CLIP_PLAY_T;

/**
 * Analog Audio Status Information Definition
 *
 */
typedef struct AUDIO_DRV_ANALOG_INFO		/* Analog Audio Status 관련 변수 모음 */
{
	ATV_AUDIO_MODE_SET_T 		audSystemAnalogMode;  		/**<  Analog(NTSC) 음성다중 모드 설정값(by Task) */
	ATV_AUDIO_MODE_SET_T 		audUserAnalogMode;    		/**<  Analog(NTSC) 음성다중 모드 설정값(by UI) */
	ATV_AUDIO_MODE_GET_T		audCurAnalogStatus;   		/**<  Analog(NTSC) 음성다중 모드(by Reading Register) */
	AUDIO_SIF_SOUNDSYSTEM_T		audSoundSystem;
	AUDIO_SIF_STANDARD_T		audAnaAudioStandards;
	AUDIO_SIF_CHECK_STATUS_T	audAnaAudioCheckStatus;
	ATV_AUDIO_NICAM_SIGNATURE_T	audCurNicamSignature;		/** < Analog auido Nicam 인지 여부 (by reading register) */
	UINT16						audA2StereoIdentityLevel;	/**<  Current A2 Stereo Identity Level 값 - KOR ONLY */
	UINT16						audA2ThresholdLevel;		/**<  A2 Threshold Level 값 - KOR ONLY */
	BOOLEAN 					audA2ThresholdLevelDown;	/**<  A2 threshold Down 여부 - KOR ONLY */
	BOOLEAN						audATVScanMode;
	BOOLEAN						audPrevATVScanMode;
	UINT8						audDelayForNTSCDetection;
	UINT16						audDelayForDK;
	UINT16						audDelayForNICAM;
	UINT16						audDelayForPopAudioMode;
	UINT16						audDelayForPopBeforeSetAnalogMode;
	UINT16						audDelayForPopAfterSetAnalogMode;
	UINT16						audThresholdForNTSC;
	UINT16						audThresholdForBTSC;
	BOOLEAN						audIsSIFDetected;			/**< stereo mode detection 여부 */
	BOOLEAN						audIsNoised;
} AUDIO_DRV_ANALOG_INFO_T;

/**
 * Audio Sound Effects Information Definition
 * AVC, BBE, Surround Mode
 *
 */
typedef struct AUDIO_DRV_EFFECT			/* Sound Effect 관련 변수 모음 */
{
	BOOLEAN				bSEEnable;				/**<  Sound Engine ON/OFF  */
	BOOLEAN 			audAVCOnOff;			/**<  AVC ON/OFF  */
	AUDIO_PVC_MODE_T	audPVCMode;				/**<	PVC (Auto Volume) Parameter Mode, 입력 종류에 따라 설정된 PVC mode */
	BOOLEAN				audSZOnOff;				/**< Sound Zooming ON/OFF */
	UINT8				audSZMode;				/**< Sound Zooming Mode */
	BOOLEAN				audDEQOnOff;			/**< Dynamic EQ ON/OFF */
	BOOLEAN				audBBOnOff;				/**< Bass Booster ON/OFF */
	AUDIO_SURROUND_EFFECT_T 	audSurroundMode;    	/**<  Surround Mode(OFF/ClearVoiceII/SRS) */
	UINT8				audCVLevel;			/**<  Clear Voice II Level */
	AUDIO_EZ_SOUND_T	audezSound;				/**<  uEQ Mode */
	AUDIO_CV_ROOMEQ_T	audcvRoomEQ;			/**<  uEQ2 Mode */
	BOOLEAN 			audBassEQOnOff;			/**<  Bass EQ ON/OFF  */
	UINT8			audVolCurveType;
	UINT32			SoundZoomingDepth;
} AUDIO_DRV_EFFECT_T;

/**
* Define Structure for Audio Delay Value
*
*/
typedef struct AUDIO_DELAY_VALUE
{
	UINT8		audDelayType;
	SINT16		audSPKDelayValue[(AUD_SRC_INVALID+1)];
	SINT16		audDACDelayValue[(AUD_SRC_INVALID+1)];
	SINT16		audSPDIFDelayValue[(AUD_SRC_INVALID+1)];
} AUDIO_DELAY_VALUE_T;

/**
 * Audio Sound Effects Information Definition
 * AVC, BBE, Surround Mode
 *
 */
typedef struct AUDIO_DRV_DELAY			/* Audio Delay Value  */
{
	BOOLEAN		curDelayOnOff;
	SINT32		curSPKDelay;
	SINT32		curDACDelay;
	SINT32		curSPDIFDelay;
	SINT32		curAdjustDelay;

	const AUDIO_DELAY_VALUE_T 	*audConstDelay;

#ifdef USE_LIPSYNC_BY_RESOL
	SINT32		audConstSPKDelay;
	SINT32		audConstDACDelay;
	SINT32		audConstSPDIFDelay;
#endif
#if 0
	const AUDIO_DELAY_VALUE_T 	*audSPKConstDelay;					/**<  Surround Mode(OFF/ClearVoiceII/SRS) */
	const AUDIO_DELAY_VALUE_T	*audDACConstDelay;
	const AUDIO_DELAY_VALUE_T	*audSPDIFConstDelay;
#endif
	BOOLEAN		audUserDelayOnOff[AUD_DELAY_SRC_INDEX_MAX];
	SINT8		audSPKUserDelay[AUD_DELAY_SRC_INDEX_MAX];	/* user delay setting value for speaker */
	SINT8		audSPDIFUserDelay[AUD_DELAY_SRC_INDEX_MAX];	/* user delay setting value for spdif */
	SINT8		audAdjustDelay[AUD_DELAY_SRC_INDEX_MAX];	/**<  in-start menu DelayValue */

} AUDIO_DRV_DELAY_T;


/**
 * Audio Sound Effects Information Definition
 * AVC, BBE, Surround Mode
 *
 */
typedef struct AUDIO_DELAY_CONTRL			/* Audio Delay Value  */
{
	BOOLEAN		bUserDelayOnOff;	/**<  user delay on/off */
	SINT8		userSPKDelay;		/**<  user delay for spk out, -5 ~ 15 */
	SINT8		userSPDIFDelay;		/**<  user delay for spdif out, -5 ~ 15 */
	SINT8		adjustSPKDelay;		/**<  adjust menu delay for spk, -10 ~ 20 */
#if	0
	SINT8		adjustSPDIFDelay;	/**<  current treble :: 0 ~ 100 */
	SINT8		userDACDelay;		/**<  current_Description :: -15 ~ 15 */
	SINT8		adjustDACDelay;		/**<  current surround */
#endif
	UINT8		bUserDelayOnOffMask	: 1,	/**< 0. flag for volume masking */
				userSPKDelayMask	: 1,	/**< 1. flag for balance masking */
				userSPDIFDelayMask	: 1,	/**< 2. flag for clearvoice masking */
				adjustSPKDelayMask	: 1;	/**< 3. flag for audio_description masking */
			#if	0
				adjustSPDIFMask		: 1,	/**< 4.  flag for bass masking */
				userDACDelayMask	: 1,	/**< 5.  flag for treble masking */
				adjustDACDelayMask	: 1, 	/**< 6.  flag for clearvoice masking */
				reservedMask		: 1;	/**< 7.  flag for audio_description masking */
			#endif	    	/**<  Surround Mode(OFF/ClearVoiceII/SRS) */
} AUDIO_DELAY_CONTRL_T;


/**
 * Audio Debug Information Definition
 * Source, Type, Mute, Volume 관련 변수 모음.
 *
 */
typedef struct AUDIO_DRV_COMMON			/* Source, Type, Mute, Volume 관련 변수 모음 */
{
	BOOLEAN					audBlockOnOff;					/**<  Current Audio Block 여부 */
	BOOLEAN					audSCARTOutBlockOnOff;			/**<  Current Audio TVOut Block 여부 */
	UINT8					audVolumeMute;					/**<  Current Speaker Block On 여부 */
	UINT8					audUiVolumeMute;				/**<  Current Speaker 조용히 여부 */
	UINT8					audMuteVolume;					/**<  Current 조용히 설정 시 Volume */
	UINT8					audSysDBVolume;					/**<  Current System DB Volume(UI Volume) */
	/* 이상 3 변수는 DDI_AUDIO_GetMuteState(UINT8 *volume)함수를 위한 API 변수 Backup용 변수 */
	UINT8 					audCurVolume;					/**<  Current Speaker Volume */
	UINT8					audCurBalance;					/**<  Current Balance Value */
	BOOLEAN 				audSCARTOutputMute;				/**<  TV Output Mute 여부 */
	BOOLEAN 				audSPDIFOutputMute;				/**<  S/PDIF Output Mute 여부 */
	AUDIO_SOURCE_INFO_T		audPreSetSource;          		/**<  Previous Audio Source */
	AUDIO_SOURCE_INFO_T		audSetSource;          			/**<  Current Audio Source */
	AUDIO_SRC_TYPE_T		audSetType;            			/**<  Current Audio Type */
	AUDIO_SPDIF_OUTPUT_MODE_T		audSPDIFOutType;		/**<  Current SPDIF Output Type */
	BOOLEAN					audAnalogAudTaskMute;			/**<  Current Analog Audio Task Mute 여부 */
	AUDIO_SPK_OUTPUT_MODE_T audSPKOutMode;
	UINT8					audHDEVmode;					/**<	High Deviation Mode 여부 */
	BOOLEAN					audParametricEQOnOff;			/**< 	Parametric EQ(SPK EQ) on/off control   */
	BOOLEAN					audHeadsetConnection;			/**<	유선 헤드셋 연결 여부 */
	BOOLEAN					audTVSpeakerOnOff;				/**<	TV speaker On/Off 여부 */
	BOOLEAN					audIsInputBlocked;				/**<	현재 입력의 input의 block 여부(UI input block, channel block) */
	BOOLEAN					audAMPMute;						/**<	Amp Mute 여부 */
	BOOLEAN					audHDMIInputBlocked;			/**<	HDMI Input Block 여부 */
	BOOLEAN					audIsPowerOnEnd;				/**<	Power ON end 여부 */
	BOOLEAN					audIsTVoutAvailable;			/**<	현재 입력에 대해 TV out 지원 여부 */
	BOOLEAN					audIsInputSupportMenu;			/**<	현재 입력이 support menu 인지의 여부 */
	BOOLEAN					audIsPreSourceAutoAV;			/**<	Check previous source is Auto AV or not */
	BOOLEAN					audARCEnable;					/**<	Enable Audio Return Channel */
	BOOLEAN					audSelectedAMPType;				/**<	Sound bar 대응 모델인 경우 현재 AMP type을 판단해 주는 변수 */
	BOOLEAN					audAVSyncBypass;				/**<	AV Sync 메뉴중 Bypass가 체크여부*/
	UINT16					audSPKOutGain;
	UINT16					audSPKOutVol;
	UINT16					audHPOutGain;
	UINT16					audHPOutVol;
	UINT16					audSPDIFOutGain;
	UINT16					audSCARTOutGain;
} AUDIO_DRV_COMMON_T;

/**
 * Audio Control Type
 *
 */
typedef struct AUDIO_CTL_TYPE
{
	/* If Change this structure.
	  YOU MUST CHANGE 'ADEC_CTL_TYPE_T' in adec_ddi.h file !!! */
	UINT32	country_mask;
	UINT8	audModuleMask;
	UINT8	audDelyTypeIndex;
	UINT8	audVolCrvIndex;		/* AMP Volume */
	UINT8	audAmpEqIndex;		/* AMP Parametric EQ */
	UINT8	audAmpCtlIndex;		/* AMP register Control value */
	UINT8	audSPKGainIndex;
	UINT8	audSPDIFGainIndex;
} AUDIO_CTL_TYPE_T;

/**
 * Audio Control Type
 *
 */
typedef struct AUDIO_MODEL_TYPE
{
	UINT8	inch_type;
	UINT8	tool_type;
	//UINT8	hw_type;	/* AUD_HW_BASE, AUD_HW_2ND, AUD_HW_2ND */
	//위의 hw type은 module 변경시.. power 변경으로 audio setting이 바뀌는 경우가 있음.

	AUDIO_CTL_TYPE_T	audioCtlTbl[];
} AUDIO_MODEL_TYPE_T;


/**
 * Audio Volume Curve Structure
 *
 */
typedef struct AUDIO_VOL_CURVE
{
	UINT8		audVolCrvIndex;
	UINT16		audVolumeCurve[101];
} AUDIO_VOL_CURVE_T;

/**
* Define Structure for DAC vol setting
*
*/
typedef struct AUDIO_DAC_VOLUME
{
	UINT16		DAC_PAL_BG;
	UINT16		DAC_I_DK;
	UINT16		DAC_L_AM;
	UINT16		DAC_BG_NICAM;
	UINT16		DAC_I_DK_NICAM;
	UINT16		DAC_L_NICAM;

	UINT16		DAC_NTSC_A2;
	UINT16		DAC_BTSC_Mono;
	UINT16		DAC_BTSC_Stereo;
	UINT16		DAC_BTSC_SAP;

	UINT16		DAC_AV;
	UINT16		DAC_DTV;
	UINT16		DAC_DEFAULT;
} AUDIO_DAC_VOLUME_T;




/**
 * Audio Country Mask
 *
 */
typedef enum AUD_COUNTRY_MASK
{
	AUD_DEFAULT_COUNTRY_MASK			= 0x00000000,

	AUD_EU_COUNTRY_MASK 				= 0x00000001,	/* EU		*/
	AUD_CN_COUNTRY_MASK 				= 0x00000002,	/* China 	*/
#ifdef ENABLE_AJJA_GROUP
	AUD_AJ_COUNTRY_MASK 				= 0x00000004,
	AUD_JA_COUNTRY_MASK 				= 0x00000008,
	AUD_IL_COUNTRY_MASK 				= 0x00000010,
#else

/*	AUD_ANAL_PAL_COUNTRY_MASK			= 0x00000004,*/	/* Analog Model PAL Area, 중아,아주 아날로그 국가 */
	AUD_ANAL_PAL_ASIA_DEF_COUNTRY_MASK	= 0x00000004,	/* Analog Model PAL Area Default - except India */
	AUD_ANAL_PAL_INDIA_COUNTRY_MASK		= 0x00000008,	/* Analog Model PAL Area - Only India */

	AUD_AU_COUNTRY_MASK 				= 0x00000010,	/* Australia	*/
	AUD_SG_COUNTRY_MASK 				= 0x00000020,	/* Singapore	*/
	AUD_ZA_COUNTRY_MASK 				= 0x00000040,	/* South Africa	*/
	AUD_VN_COUNTRY_MASK 				= 0x00000080,	/* Viet Nam		*/

	AUD_ID_COUNTRY_MASK 				= 0x00000100,	/* Indonesia	*/
	AUD_MY_COUNTRY_MASK 				= 0x00000200,	/* Malaysia		*/
	AUD_IL_COUNTRY_MASK 				= 0x00000400,	/* Israel		*/
	AUD_IR_COUNTRY_MASK 				= 0x00000800,	/* Iran			*/
#endif

	AUD_US_COUNTRY_MASK 				= 0x00100000,	/* United States	*/
	AUD_KR_COUNTRY_MASK 				= 0x00200000,	/* Korea, Republic of 	*/
	AUD_TW_COUNTRY_MASK 				= 0x00400000,	/* Taiwan	*/
	AUD_BR_COUNTRY_MASK 				= 0x00800000,	/* Brazil	*/
	AUD_ANAL_NTSC_COUNTRY_MASK			= 0x01000000,	/* Analog Model NTSC Area, 중남미 아날로그 국가 */
	AUD_JP_COUNTRY_MASK 				= 0x02000000,	/* JAPAN		*/

#ifdef ENABLE_AJJA_GROUP
	/* Emerging Market Group */
	AUD_EMERGING_MARKET_COUNTRY_MASK	= AUD_AJ_COUNTRY_MASK | AUD_JA_COUNTRY_MASK | AUD_IL_COUNTRY_MASK,

	/* PAL Country Group */
	AUD_PAL_COUNTRY_MASK				= AUD_EU_COUNTRY_MASK | AUD_CN_COUNTRY_MASK \
										| AUD_AJ_COUNTRY_MASK | AUD_JA_COUNTRY_MASK | AUD_IL_COUNTRY_MASK,

#else

	/* Emerging Market Group */
	AUD_EMERGING_MARKET_COUNTRY_MASK 	= AUD_AU_COUNTRY_MASK | AUD_SG_COUNTRY_MASK \
										| AUD_ZA_COUNTRY_MASK | AUD_VN_COUNTRY_MASK \
										| AUD_ID_COUNTRY_MASK | AUD_MY_COUNTRY_MASK \
										| AUD_IL_COUNTRY_MASK | AUD_IR_COUNTRY_MASK,

	/* A-ASIA Market Group */
	AUD_ANAL_PAL_COUNTRY_MASK			= AUD_ANAL_PAL_ASIA_DEF_COUNTRY_MASK	\
										| AUD_ANAL_PAL_INDIA_COUNTRY_MASK,
	/* PAL Country Group */
	AUD_PAL_COUNTRY_MASK				= AUD_EU_COUNTRY_MASK | AUD_CN_COUNTRY_MASK \
										| AUD_AU_COUNTRY_MASK | AUD_SG_COUNTRY_MASK \
										| AUD_ZA_COUNTRY_MASK | AUD_VN_COUNTRY_MASK \
										| AUD_ID_COUNTRY_MASK | AUD_MY_COUNTRY_MASK \
										| AUD_IL_COUNTRY_MASK | AUD_IR_COUNTRY_MASK	\
 										| AUD_ANAL_PAL_COUNTRY_MASK,
 #endif

	/* ATSC Country Group */
	AUD_ATSC_COUNTRY_MASK				= AUD_US_COUNTRY_MASK | AUD_KR_COUNTRY_MASK,



	/* NT Country Group */
	AUD_NTSC_COUNTRY_MASK				= AUD_ATSC_COUNTRY_MASK | AUD_TW_COUNTRY_MASK \
										| AUD_BR_COUNTRY_MASK | AUD_ANAL_NTSC_COUNTRY_MASK,

	AUD_UNAVAILABLE_COUNTRY_MASK		= 0xFFFFFFFF,

} AUD_COUNTRY_MASK_MASK_T;


/**
 * Audio Module Mask
 *
 */
typedef enum AUD_MODULE_MASK
{
	AUD_MODULE_DEFAULT 	= 0x00,
	AUD_MODULE_LGD		= 0x01,
	AUD_MODULE_CMI		= 0x02,
	AUD_MODULE_AUO		= 0x04,
	AUD_MODULE_SHARP 	= 0x08,
	AUD_MODULE_IPS		= 0x10,
	AUD_MODULE_INNOLUX	= 0x20,

	AUD_MODULE_LGE		= 0x40,
	AUD_MODULE_PANASONIC = 0x80,

	AUD_MODULE_LCD		= 0x3F,
	AUD_MODULE_PDP		= 0xC0,
	AUD_MODULE_ALL		= 0xFF,

	AUD_MODULE_NOT_LGD		= (AUD_MODULE_ALL ^ AUD_MODULE_LGD),
	AUD_MODULE_NOT_CMI		= (AUD_MODULE_ALL ^ AUD_MODULE_CMI),
	AUD_MODULE_NOT_AUO		= (AUD_MODULE_ALL ^ AUD_MODULE_AUO),
	AUD_MODULE_NOT_SHARP	= (AUD_MODULE_ALL ^ AUD_MODULE_SHARP),
	AUD_MODULE_NOT_IPS		= (AUD_MODULE_ALL ^ AUD_MODULE_IPS),
	AUD_MODULE_NOT_INNOLUX	= (AUD_MODULE_ALL ^ AUD_MODULE_INNOLUX),

	AUD_MODULE_NOT_LGE		= (AUD_MODULE_ALL ^ AUD_MODULE_LGE),
	AUD_MODULE_NOT_PANASONIC	= (AUD_MODULE_ALL ^ AUD_MODULE_PANASONIC),

	AUD_MODULE_MASK_END = AUD_MODULE_ALL
} AUD_MODULE_MASK_T;


/**
 * Audio AMP Parametric EQ Mode, if SIF System is SECAM_L, EQ is setted Base EQ Off & SECAM_L EQ
 *
 */
typedef enum AUDIO_AMP_CHANGE_EQ_MODE
{
	AUD_AMP_USE_CURRENT_SETTING		= 0,
	AUD_AMP_CHANGE_EQ_NORMAL		= 1,
	AUD_AMP_CHANGE_EQ_CN_ATV 		= 2,
	AUD_AMP_CHANGE_EQ_OFF			= 3,
	AUD_AMP_CHANGE_EQ_TABLE			= 4,
} AUDIO_AMP_CHANGE_EQ_MODE_T;

/**
 * Audio Country Group Index
 *
 */
typedef enum AUD_COUNTRY_GROUP_INDEX
{
	AUD_COUNTRY_GROUP_DEFAULT,
	AUD_COUNTRY_GROUP_EU = AUD_COUNTRY_GROUP_DEFAULT,
	AUD_COUNTRY_GROUP_AU,
	AUD_COUNTRY_GROUP_CN,
	AUD_COUNTRY_GROUP_AN_PAL,
	AUD_COUNTRY_GROUP_KR_US,
	AUD_COUNTRY_GROUP_BR,
	AUD_COUNTRY_GROUP_TW,
	AUD_COUNTRY_GROUP_AN_NT,
	AUD_COUNTRY_GROUP_JP,
	AUD_COUNTRY_GROUP_MAX
} AUD_COUNTRY_GROUP_INDEX_T;



#ifdef MOVE_SETTING_ADEC_TO_AUDIO
/**
 * Audio Out Gain Structure
 *
 */
typedef struct AUDIO_OUT_GAIN
{
	UINT8		audioOutGainType;
	UINT16		audioOutGain[AUD_GAIN_INDEX_MAX];
} AUDIO_OUT_GAIN_T;
#endif

/**
 * Audio Out Volumne Curve Structure
 *
 */
typedef struct AUDIO_OUT_VOL_CRV
{
	UINT8		audioVolCrvType;
	UINT16		audioVolCurve[AUD_VOL_MAX+1];
} AUDIO_OUT_VOL_CRV_T;

/**
 * Speaker Output Type Definition
 *
 */
typedef enum
{
	AUDIO_SPK_GAIN_ADEC		= FALSE,
	AUDIO_SPK_GAIN_AMP		= TRUE
} AUDIO_SPK_GAIN_MODE_T;

/**
 * Speaker Output Type Definition
 *
 */
typedef enum
{
	AUDIO_SPK_VOL_ADEC,
	AUDIO_SPK_VOL_AMP,
	AUDIO_SPK_VOL_MIXED
} AUDIO_SPK_VOLUME_MODE_T;


/**
 * pointer strut for AMP function.
 */
typedef struct AUDIO_DDI_AMP
{
	DTV_STATUS_T		(*pfnInitializeAMP) (void);
	DTV_STATUS_T		(*pfnSetSpeakerVolumeAMP) (UINT8 volSPK);
	DTV_STATUS_T		(*pfnSetBalanceAMP) (UINT8 balance);
	DTV_STATUS_T		(*pfnSetBassAMP) (UINT8 bass);
	DTV_STATUS_T		(*pfnSetTrebleAMP) (UINT8 treble);
	DTV_STATUS_T		(*pfnSetAVCAMP) (BOOLEAN bOnOff);
	DTV_STATUS_T		(*pfnSetVolumeAsInputAMP) (AUDIO_SOURCE_INFO_T eAudioSource, AUDIO_SIF_STANDARD_T sifStandard);
	DTV_STATUS_T		(*pfnSetParametricEqAMP) (BOOLEAN bOnOff);
	DTV_STATUS_T		(*pfnChangeEqModeAMP) (AUDIO_AMP_CHANGE_EQ_MODE_T mode);
	DTV_STATUS_T		(*pfnSetMuteAMP) (BOOLEAN bOnOff);
	DTV_STATUS_T		(*pfnSetPwmOutAMP) (BOOLEAN bOnOff /*, BOOLEAN bSndbarType*/);
	DTV_STATUS_T		(*pfnSetMasterClkAMP) (AUDIO_MCLK_REAL_FREQUENCY_T fMasterClk);
	DTV_STATUS_T		(*pfnSelectAMP) (BOOLEAN bSelectAmp);
	DTV_STATUS_T		(*pfnSetVolumeTypeAMP) (UINT8 volType);
#ifdef USE_WOOFER_ONOFF
	DTV_STATUS_T		(*pfnSetWooferOnOffAMP) (BOOLEAN bOnOff);
#endif
	AUDIO_AMP_STATUS_T	(*pfnGetAmpStatusAMP) (UINT8 checkType);
	void				(*pfnDebugMenuAMP) (void);
} AUDIO_DDI_AMP_T;

#define MAX_SUPPORT_CHIP	20
/**
 * Runtime binding을 위한 structure
 */
typedef struct AUDIO_DDI_RUNBIND
{
	UINT8				ampType;
	AUDIO_DDI_AMP_T		*pFuncSet;
} AUDIO_DDI_RUNBIND_T;

#ifdef USE_LIPSYNC_BY_RESOL
typedef enum {
	TYPE_LIPSYNC_DELAY_NONE = 0,
	TYPE_LIPSYNC_DELAY_RF,
	TYPE_LIPSYNC_DELAY_AV_NTSC,
	TYPE_LIPSYNC_DELAY_AV_PALM,
	TYPE_LIPSYNC_DELAY_AV_PAL,
	TYPE_LIPSYNC_DELAY_COMP_480I,
	TYPE_LIPSYNC_DELAY_COMP_576I,
	TYPE_LIPSYNC_DELAY_COMP_480P,
	TYPE_LIPSYNC_DELAY_COMP_576P,
	TYPE_LIPSYNC_DELAY_COMP_720P_50HZ,
	TYPE_LIPSYNC_DELAY_COMP_720P_60HZ,  //10
	TYPE_LIPSYNC_DELAY_COMP_1080I_50HZ,
	TYPE_LIPSYNC_DELAY_COMP_1080I_60HZ,
	TYPE_LIPSYNC_DELAY_COMP_1080P_50HZ,
	TYPE_LIPSYNC_DELAY_COMP_1080P_60HZ,
	TYPE_LIPSYNC_DELAY_COMP_1080P_24HZ,
	TYPE_LIPSYNC_DELAY_COMP_1080P_30HZ,
	TYPE_LIPSYNC_DELAY_RGB_640X350_70_09HZ,
	TYPE_LIPSYNC_DELAY_RGB_720X400_70_08HZ,
	TYPE_LIPSYNC_DELAY_RGB_640X480_59_94HZ,
	TYPE_LIPSYNC_DELAY_RGB_640X480_72_80HZ,  //20
	TYPE_LIPSYNC_DELAY_RGB_640X480_75_00HZ,
	TYPE_LIPSYNC_DELAY_RGB_800X600_56_25HZ,
	TYPE_LIPSYNC_DELAY_RGB_800X600_60_31HZ,
	TYPE_LIPSYNC_DELAY_RGB_800X600_72_18HZ,
	TYPE_LIPSYNC_DELAY_RGB_800X600_75_00HZ,
	TYPE_LIPSYNC_DELAY_RGB_1024X768_60HZ,
	TYPE_LIPSYNC_DELAY_RGB_1024X768_70_06HZ,
	TYPE_LIPSYNC_DELAY_RGB_1024X768_75_02HZ,
	TYPE_LIPSYNC_DELAY_RGB_1280X768_59_87HZ,
	TYPE_LIPSYNC_DELAY_RGB_1280X768_74_893HZ,  //30
	TYPE_LIPSYNC_DELAY_RGB_1360X768_60_015HZ,
	TYPE_LIPSYNC_DELAY_RGB_1280X1024_60_020HZ,
	TYPE_LIPSYNC_DELAY_RGB_1280X1024_75_025HZ,
	TYPE_LIPSYNC_DELAY_RGB_1600X1200_60HZ,
	TYPE_LIPSYNC_DELAY_RGB_1920X1080_59_934HZ,
	TYPE_LIPSYNC_DELAY_HDMI_480P,
	TYPE_LIPSYNC_DELAY_HDMI_576P,
	TYPE_LIPSYNC_DELAY_HDMI_720P_60HZ,
	TYPE_LIPSYNC_DELAY_HDMI_720P_50HZ,
	TYPE_LIPSYNC_DELAY_HDMI_1080I_60HZ,  //40
	TYPE_LIPSYNC_DELAY_HDMI_1080I_50HZ,
	TYPE_LIPSYNC_DELAY_HDMI_1080P_50HZ,
	TYPE_LIPSYNC_DELAY_HDMI_1080P_60HZ,
	TYPE_LIPSYNC_DELAY_HDMI_1080P_24HZ,
	TYPE_LIPSYNC_DELAY_HDMI_1080P_30HZ,

	//DVI 임시값
	TYPE_LIPSYNC_DELAY_DVI_800X600_60HZ,
	TYPE_LIPSYNC_DELAY_DVI_1024X768_60HZ,
	TYPE_LIPSYNC_DELAY_DVI_1360X768_60HZ,
	TYPE_LIPSYNC_DELAY_DVI_1280X1024_60HZ,
	TYPE_LIPSYNC_DELAY_DVI_1600X1200_60HZ,  //50
	TYPE_LIPSYNC_DELAY_DVI_1920X1080_60HZ,

	TYPE_LIPSYNC_DELAY_BAD_VIDEO,
	TYPE_LIPSYNC_DELAY_MAX

}TYPE_LIPSYNC_DELAY_T;

typedef struct{
	SINT32								nI2sDelay;
	SINT32								nDacDelay;
	SINT32								nSpdifDelay;

}STRUCT_LIPSYNC_FORMAT_RESOLUTION_T;

/**
* Define Structure for Audio Delay Value, When resolusion changed,
*
*/
typedef struct AUDIO_LIPSYNC_RESOLUTION_DELAY
{
	UINT8								audDelayType;
	STRUCT_LIPSYNC_FORMAT_RESOLUTION_T	audDelayValue[TYPE_LIPSYNC_DELAY_MAX];
} AUDIO_LIPSYNC_RESOLUTION_DELAY_T;

#ifdef USE_LIPSYNC_BY_3D_L9
typedef enum {
	TYPE_LIPSYNC_DELAY_EXTRA_NONE = 0,
	TYPE_LIPSYNC_DELAY_EXTRA_3DMODE,  //3D, 2Dto3D Mode
	TYPE_LIPSYNC_DELAY_EXTRA_PICMODE, //Game, Truemotion mode
	TYPE_LIPSYNC_DELAY_EXTRA_MAX,
}TYPE_LIPSYNC_DELAY_EXTRA_T;

typedef struct AUDIO_LIPSYNC_DELAY_EXTRA
{
	BOOLEAN		audDelayExtraOnOff;
	SINT32		audDelayExtra[TYPE_LIPSYNC_DELAY_EXTRA_MAX];
} AUDIO_LIPSYNC_DELAY_EXTRA_T;

#endif	//USE_LIPSYNC_BY_3D_L9

#endif //USE_LIPSYNC_BY_RESOL

/**
 * Audio Level Sound for japanese antena setting.
 * Level1 ~ Level8
 */
typedef enum
{
	AUDIO_LVL_SOUND_DEFAULT = 0,
	AUDIO_LVL_SOUND1,		/* beep_220hz */
	AUDIO_LVL_SOUND2,		/* beep_293hz */
	AUDIO_LVL_SOUND3,		/* beep_392hz */
	AUDIO_LVL_SOUND4,		/* beep_523hz */
	AUDIO_LVL_SOUND5,		/* beep_698hz */
	AUDIO_LVL_SOUND6,		/* beep_987hz */
	AUDIO_LVL_SOUND7,		/* beep_1318hz */
	AUDIO_LVL_SOUND8,		/* beep_1760hz */
} AUDIO_LEVEL_SOUND_T;

/******************************************************************************
	변수 선언 (Variables Declaration)
******************************************************************************/
/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
extern DTV_STATUS_T DDI_AUDIO_GetCurAnalogMode(ATV_AUDIO_MODE_GET_T *pAtvMode);
extern DTV_STATUS_T DDI_AUDIO_Init ( void );
extern DTV_STATUS_T DDI_AUDIO_EnableSCARTOutput(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetAmpMute(BOOLEAN   bOn);
extern UINT16		DDI_AUDIO_GetCurA2ThresholdLevel(void);
extern DTV_STATUS_T DDI_AUDIO_SetA2ThresholdLevel(UINT16 a2ThresholdLvl);
extern UINT16 		DDI_AUDIO_GetCurNicamThresholdLevel(void);
extern DTV_STATUS_T	DDI_AUDIO_SetNicamThresholdLevel(UINT16 nicamThresholdLvl);
extern DTV_STATUS_T DDI_AUDIO_SetBalance(UINT8 balance);
extern DTV_STATUS_T DDI_AUDIO_SetEqualizerLevel(UINT8 band,UINT8 levelIndex);
extern DTV_STATUS_T DDI_AUDIO_SetBass(UINT8 bass) ;
extern DTV_STATUS_T DDI_AUDIO_SetTreble(UINT8 treble) ;
extern DTV_STATUS_T DDI_AUDIO_SetBBE(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetSCARTOutputMute(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetATVOutVolume(void);
extern DTV_STATUS_T DDI_AUDIO_SetUserAnalogMode(ATV_AUDIO_MODE_SET_T eMode);
extern DTV_STATUS_T DDI_AUDIO_SetSource (AUDIO_SOURCE_INFO_T  eAudioSource, AUDIO_SRC_TYPE_T eAudioType) ;
extern DTV_STATUS_T DDI_AUDIO_SetSpdifOutputMute(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetSpdifOutputType( AUDIO_SPDIF_OUTPUT_MODE_T eMode );
DTV_STATUS_T  DDI_AUDIO_SetSpdifOutputSCMS(AUDIO_SPDIF_COPYRIGHT_T copyRight);
DTV_STATUS_T  DDI_AUDIO_EnableAudioReturnChannel(BOOLEAN bOnOff);
DTV_STATUS_T DDI_AUDIO_GetAudioClipStatus(AUDIO_CLIP_PLAY_T *pStatus);
DTV_STATUS_T DDI_AUDIO_PlayAudioClip(UINT32 bufSize,void *pClipBufferPointer,UINT32 repeatNumber, AUDIO_SRC_TYPE_T clipType, BOOLEAN bMix, void (* pfnAclipDone)(void));
DTV_STATUS_T DDI_AUDIO_PauseAudioClip(void);
DTV_STATUS_T DDI_AUDIO_ResumeAudioClip(void);
DTV_STATUS_T DDI_AUDIO_StopAudioClip(void);
DTV_STATUS_T DDI_AUDIO_StartDecoding(AUDIO_SRC_TYPE_T adecType, AUDIO_SRC_TYPE_T adType);
DTV_STATUS_T DDI_AUDIO_StopDecoding(void);
DTV_STATUS_T DDI_AUDIO_GetDecoderInfo(AUDIO_INFO_T *pDecoderInfo);
extern DTV_STATUS_T DDI_AUDIO_SetSurroundEffect(AUDIO_SURROUND_EFFECT_T eSurround);
extern DTV_STATUS_T DDI_AUDIO_SetVolume(UINT8 volume) ;
/*--------      AUDIO DDI Functions related with Audio Task  ------------------------------------------*/
#ifdef USE_LIPSYNC_BY_RESOL
extern void         DDI_AUDIO_RunSpdifPeriodic(UINT32 unLipSyncDelayIndex, BOOLEAN bUsed);
#else
extern void         DDI_AUDIO_RunSpdifPeriodic(BOOLEAN bUsed);
#endif
extern AUDIO_SIF_STANDARD_T DDI_AUDIO_CheckSoundStandard(AUDIO_SIF_SOUNDSYSTEM_T audSoundSystem);
extern AUDIO_SIF_STANDARD_T DDI_AUDIO_GetSoundStandard(void);
extern DTV_STATUS_T DDI_AUDIO_RunAnalogAudioPeriodic(ATV_AUDIO_MODE_SET_T analogAudioMode);
extern void         DDI_AUDIO_EnableAnalogAudioScanPeriodic(BOOLEAN	bOnOff );
extern DTV_STATUS_T DDI_AUDIO_DetectSoundSystem(AUDIO_SIF_SOUNDSYSTEM_T soundSystem, UINT32 *pSifStrength, BOOLEAN bRetryRequested);
extern DTV_STATUS_T DDI_AUDIO_SetSoundSystem(AUDIO_SIF_SOUNDSYSTEM_T sifBand);
extern DTV_STATUS_T DDI_AUDIO_SetSPKOutMode(AUDIO_SPK_OUTPUT_MODE_T outputMode);
extern DTV_STATUS_T DDI_AUDIO_GetAudioScanStatus(BOOLEAN *pbAudScanStatus);
extern DTV_STATUS_T DDI_AUDIO_SetHDEVMode(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetParametricEQ(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetAmpFrequency(AUDIO_MCLK_REAL_FREQUENCY_T fMasterClk);
extern DTV_STATUS_T DDI_AUDIO_SetHPMute(BOOLEAN bOnOff);
extern DTV_STATUS_T DDI_AUDIO_SetHPOutputType(AUDIO_HP_SOURCE_T hpSrc);
extern DTV_STATUS_T DDI_AUDIO_SetADVolume(UINT8 volume);

/*--------      AUDIO DDI Functions related with Audio Sound Endigne    -----------------------------*/
DTV_STATUS_T DDI_AUDIO_SetClearVoice(BOOLEAN bOnOff, UINT8 uCVIIValue);
DTV_STATUS_T DDI_AUDIO_SetAutoVolumeControl(BOOLEAN bOnOff);
DTV_STATUS_T DDI_AUDIO_SetBassBooster(BOOLEAN bOnOff);
DTV_STATUS_T DDI_AUDIO_SetDynamicEQ(BOOLEAN bOnOff);
DTV_STATUS_T DDI_AUDIO_SetInfiniteSurround(BOOLEAN bOnOff);
DTV_STATUS_T DDI_AUDIO_SetSoundZooming(BOOLEAN bOnOff, UINT32 mode);
DTV_STATUS_T DDI_AUDIO_SetSoundZoomingParam(UINT32 depthInfo);
DTV_STATUS_T DDI_AUDIO_SetSEVolume(UINT8 volume);
DTV_STATUS_T DDI_AUDIO_SetSEVolumeType(UINT8 type);
DTV_STATUS_T DDI_AUDIO_SetSoundOptimizer(AUDIO_CV_ROOMEQ_T ezSound);
DTV_STATUS_T DDI_AUDIO_SetSoundMode(AUDIO_EZ_SOUND_T ezSound, UINT8 band, UINT8 bandValue);
DTV_STATUS_T DDI_AUDIO_SetAVSyncBypass(BOOLEAN bSet);
DTV_STATUS_T DDI_AUDIO_GetInputLevel(BOOLEAN bBSI, UINT32 *pLeftCh, UINT32 *pRightCh);
DTV_STATUS_T DDI_AUDIO_DisableSoundEngine(UINT8 disableLevel);
DTV_STATUS_T DDI_AUDIO_InitSoundEngine(void);

extern DTV_STATUS_T DDI_AUDIO_SetInputBlock(BOOLEAN bOnOff);
extern BOOLEAN DDI_AUDIO_IsSIFDetectCompleted(void);
extern BOOLEAN DDI_AUDIO_IsESExist(void);

extern BOOLEAN DDI_AUDIO_IsSIFDetected(void);

extern BOOLEAN DDI_AUDIO_IsSIFExist(void);
extern BOOLEAN DDI_AUDIO_IsAudioInputExist(SINT32 centerLevel);
extern BOOLEAN DDI_AUDIO_IsAudioInputExistForBSI(SINT32 centerLevel);

extern DTV_STATUS_T DDI_AUDIO_SetDACOutMute(UINT8 PortNum, BOOLEAN bOnOff);
extern AUDIO_HDMI_INPUT_MODE_T DDI_AUDIO_GetHDMIAudioType(void);
DTV_STATUS_T DDI_AUDIO_ForcePCAudioInHDMI(BOOLEAN bForcedPcAudioInHDMI);
DTV_STATUS_T DDI_AUDIO_SetHPConnectionStatus(BOOLEAN bOnOff);

DTV_STATUS_T DDI_AUDIO_PlayBeep(UINT32 repeatNumber, void (*pfnClip_callback_function)(void) );

DTV_STATUS_T DDI_AUDIO_StartSelfTest(UINT32 repeatNumber, void (*pfnClip_callback_function)(void) );
DTV_STATUS_T DDI_AUDIO_StopSelfTest(void);
DTV_STATUS_T DDI_AUDIO_ClearAnalogAudioStatus( void );
DTV_STATUS_T DDI_AUDIO_SetSyncAV(BOOLEAN bOnOff);

extern DTV_STATUS_T DDI_AUDIO_ChangeAMPType(void);



UINT8		DDI_AUDIO_GetAudioDelayIndex( AUDIO_SOURCE_INFO_T eSource );
DTV_STATUS_T DDI_AUDIO_SetDelayTime( AUDIO_DELAY_CONTRL_T sDelayControl, BOOLEAN bForced );
#ifdef USE_LIPSYNC_BY_RESOL
DTV_STATUS_T DDI_AUDIO_SetDelayTimeFromUserDelay( AUDIO_DELAY_CONTRL_T sDelayControl, BOOLEAN bForced );
DTV_STATUS_T	DDI_AUDIO_SetLipSyncDelayByIndex(UINT32 unDelayTypeIdx, BOOLEAN bForced);
DTV_STATUS_T	DDI_AUDIO_SetLipSyncDelay(SINT32 nI2SDelay, SINT32 nDACDelay, SINT32 nSPDIFDelay, BOOLEAN bForced);
#ifdef USE_LIPSYNC_BY_3D_L9
DTV_STATUS_T DDI_AUDIO_SetLipsyncExtraDelayByAnyMode(UINT32 unMode, SINT32 unValue);
SINT32 DDI_AUDIO_GetSumOfLipsyncExtraDelay(void);
#endif //USE_LIPSYNC_BY_3D_L9
#endif	//USE_LIPSYNC_BY_RESOL

#if 0
BOOLEAN DDI_AUDIO_GetPlayBeepStatus(void);
void DTV_AUDIO_SetPlayBeepStatusDone(void);
#endif
DTV_STATUS_T DDI_AUDIO_SelectAMP(BOOLEAN bSelectAmp);
DTV_STATUS_T DDI_AUDIO_GetSNDBARAMPType(BOOLEAN bAmpType);
DTV_STATUS_T DDI_AUDIO_SetVolumeType(UINT8 type);

DTV_STATUS_T DDI_AUDIO_PlayLVLSound(AUDIO_LEVEL_SOUND_T soundLevel, UINT32 repeatNumber);
DTV_STATUS_T DDI_AUDIO_StopLVLSound(void);
#ifdef USE_WOOFER_ONOFF
DTV_STATUS_T  DDI_AUDIO_SetWooferOnOff(BOOLEAN bOnOff);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _AUDIO_DDI_H_ */
