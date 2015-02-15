/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_audio.h
 *
 *  HAL 함수 header 파일.
 *
 *
 *  @author		yong kwang kim(ykwang.kim@lge.com)
 *  @reviser	Jong-Sang Oh(jongsang.oh@lge.com)
 *  @version	1.1
 *  @date		2013.05.31
 *  @note
 *  @see		hal_audio_path.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_AUDIO_H_
#define _HAL_AUDIO_H_

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
/**
 * HAL AUDIO LGSE SMARTSOUND CALLBACK DATA Length(UINT32).
 *
 */
#define HAL_AUDIO_LGSE_SMARTSOUND_CB_LENGTH		7

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
/**
 * HAL AUDIO Decoder Index.
 *
 */
typedef  enum
{
	HAL_AUDIO_ADEC0		= 0,
	HAL_AUDIO_ADEC1		= 1,
	HAL_AUDIO_ADEC_MAX	= HAL_AUDIO_ADEC1,
} HAL_AUDIO_ADEC_INDEX_T;

/**
 * HAL AUDIO Mixer Index.
 *
 */
typedef  enum
{
	HAL_AUDIO_MIXER0	= 0,	/* G-Streamer Mixer Input 0 */
	HAL_AUDIO_MIXER1	= 1,	/* G-Streamer Mixer Input 1 */
	HAL_AUDIO_MIXER2	= 2,	/* G-Streamer Mixer Input 2 */
	HAL_AUDIO_MIXER3	= 3,	/* G-Streamer Mixer Input 3 */
	HAL_AUDIO_MIXER4	= 4,	/* G-Streamer Mixer Input 4 */
	HAL_AUDIO_MIXER5	= 5,	/* HAL  AUDIO Mixer Input 5 */
	HAL_AUDIO_MIXER6	= 6,	/* ALSA AUDIO Mixer Input 6 */
	HAL_AUDIO_MIXER7	= 7,	/* ALSA AUDIO Mixer Input 7 */
	HAL_AUDIO_MIXER_MAX	= HAL_AUDIO_MIXER7,
} HAL_AUDIO_MIXER_INDEX_T;

/**
 * HAL AUDIO TP Index.
 *
 */
typedef  enum
{
	HAL_AUDIO_TP0		= 0,
	HAL_AUDIO_TP1		= 1,
	HAL_AUDIO_TP_MAX	= HAL_AUDIO_TP1,
} HAL_AUDIO_TP_INDEX_T;

/**
 * HAL AUDIO HDMI Port Index.
 * HAL_AUDIO_HDMI_SWITCH is defined for HDMI Switch Model and 1 Port Only in SoC.
 *
 */
typedef  enum
{
	HAL_AUDIO_HDMI0			= 0,	/* H15  : HDMI2.0 Port 0. */
	HAL_AUDIO_HDMI1			= 1,	/* H15  : HDMI2.0 Port 1. */
	HAL_AUDIO_HDMI2 		= 2,	/* H15  : HDMI1.4 Port 0 with ARC. */
	HAL_AUDIO_HDMI3 		= 3,	/* H15  : HDMI1.4 Port 1 with MHL. */
	HAL_AUDIO_HDMI_SWITCH 	= 4,	/* M14+ : HDMI1.4 Port with HDMI Switch Model. */
	HAL_AUDIO_HDMI_MAX		= HAL_AUDIO_HDMI_SWITCH,
} HAL_AUDIO_HDMI_INDEX_T;

/**
 * HAL AUDIO Sound Output Mode.
 *
 */
typedef  enum
{
	HAL_AUDIO_NO_OUTPUT		= 0x00,
	HAL_AUDIO_SPK			= 0x01,
	HAL_AUDIO_SPDIF			= 0x02,
	HAL_AUDIO_SB_SPDIF		= 0x04,
	HAL_AUDIO_SB_PCM		= 0x08,
	HAL_AUDIO_SB_CANVAS		= 0x10,
	HAL_AUDIO_HP			= 0x20,
	HAL_AUDIO_SCART			= 0x40,
	HAL_AUDIO_SPDIF_ES 		= 0x80,
} HAL_AUDIO_SNDOUT_T;

/**
 * HAL AUDIO Resource Type for Inter Connection.
 *
 */
typedef  enum
{
	HAL_AUDIO_RESOURCE_SDEC0			=  0,
	HAL_AUDIO_RESOURCE_SDEC1			=  1,
	HAL_AUDIO_RESOURCE_ATP0				=  2,
	HAL_AUDIO_RESOURCE_ATP1				=  3,
	HAL_AUDIO_RESOURCE_ADC				=  4,
	HAL_AUDIO_RESOURCE_HDMI				=  5,
	HAL_AUDIO_RESOURCE_AAD				=  6,
	HAL_AUDIO_RESOURCE_SYSTEM			=  7,	/* Clip or LMF Play */
	HAL_AUDIO_RESOURCE_ADEC0			=  8,
	HAL_AUDIO_RESOURCE_ADEC1			=  9,
	HAL_AUDIO_RESOURCE_AENC0			= 10,
	HAL_AUDIO_RESOURCE_AENC1			= 11,
	HAL_AUDIO_RESOURCE_SE				= 12,
	HAL_AUDIO_RESOURCE_OUT_SPK			= 13,	/* Speaker */
	HAL_AUDIO_RESOURCE_OUT_SPDIF		= 14,	/* SPDIF Ouput */
	HAL_AUDIO_RESOURCE_OUT_SB_SPDIF		= 15,	/* Sound Bar(SPDIF) : Mixer Output */
	HAL_AUDIO_RESOURCE_OUT_SB_PCM		= 16,	/* Sound Bar(PCM)   : Mixer Output(Wireless) */
	HAL_AUDIO_RESOURCE_OUT_SB_CANVAS	= 17,	/* Sound Bar(CANVAS): Sound Engine Output */
	HAL_AUDIO_RESOURCE_OUT_HP			= 18,	/* Must be controlled by audio decoder.*/
	HAL_AUDIO_RESOURCE_OUT_SCART		= 19,	/* Must be controlled by audio decoder.*/
	HAL_AUDIO_RESOURCE_MIXER0			= 20,	/* Audio Mixer Input 0. */
	HAL_AUDIO_RESOURCE_MIXER1			= 21,	/* Audio Mixer Input 1. */
	HAL_AUDIO_RESOURCE_MIXER2			= 22,	/* Audio Mixer Input 2. */
	HAL_AUDIO_RESOURCE_MIXER3			= 23,	/* Audio Mixer Input 3. */
	HAL_AUDIO_RESOURCE_MIXER4			= 24,	/* Audio Mixer Input 4. */
	HAL_AUDIO_RESOURCE_MIXER5			= 25,	/* Audio Mixer Input 5. */
	HAL_AUDIO_RESOURCE_MIXER6			= 26,	/* Audio Mixer Input 6. */
	HAL_AUDIO_RESOURCE_MIXER7			= 27,	/* Audio Mixer Input 7. */
	HAL_AUDIO_RESOURCE_OUT_SPDIF_ES		= 28,	/* SPDIF ES Ouput Only */
	HAL_AUDIO_RESOURCE_HDMI0			= 29,	/* Audio HDMI Input 0. */
	HAL_AUDIO_RESOURCE_HDMI1			= 30,	/* Audio HDMI Input 1. */
	HAL_AUDIO_RESOURCE_HDMI2			= 31,	/* Audio HDMI Input 2. */
	HAL_AUDIO_RESOURCE_HDMI3			= 32,	/* Audio HDMI Input 3. */
	HAL_AUDIO_RESOURCE_SWITCH			= 33,	/* Audio HDMI Input with switch. */
	HAL_AUDIO_RESOURCE_NO_CONNECTION 	= 0XFF,
} HAL_AUDIO_RESOURCE_T;

/**
 * HAL AUDIO Source Format Type.
 *
 */
typedef  enum
{
	HAL_AUDIO_SRC_TYPE_UNKNOWN      = 0,
	HAL_AUDIO_SRC_TYPE_PCM          = 1,
	HAL_AUDIO_SRC_TYPE_AC3          = 2,
	HAL_AUDIO_SRC_TYPE_EAC3         = 3,
	HAL_AUDIO_SRC_TYPE_MPEG         = 4,
	HAL_AUDIO_SRC_TYPE_AAC          = 5,
	HAL_AUDIO_SRC_TYPE_HEAAC        = 6,
	HAL_AUDIO_SRC_TYPE_DRA			= 7,
	HAL_AUDIO_SRC_TYPE_MP3			= 8,
	HAL_AUDIO_SRC_TYPE_DTS			= 9,
	HAL_AUDIO_SRC_TYPE_SIF			= 10,
	HAL_AUDIO_SRC_TYPE_SIF_BTSC		= 11,
	HAL_AUDIO_SRC_TYPE_SIF_A2		= 12,
	HAL_AUDIO_SRC_TYPE_DEFAULT		= 13,
	HAL_AUDIO_SRC_TYPE_NONE			= 14,
	HAL_AUDIO_SRC_TYPE_DTS_HD_MA 	= 15,
	HAL_AUDIO_SRC_TYPE_DTS_EXPRESS 	= 16,
	HAL_AUDIO_SRC_TYPE_DTS_CD		= 17,
} HAL_AUDIO_SRC_TYPE_T;

/**
 * HAL AUDIO Source Input Type.
 * TP, PCM, SPDIF
 *
 */
typedef  enum
{
	HAL_AUDIO_IN_PORT_NONE		=  0,
	HAL_AUDIO_IN_PORT_TP		=  1,	// From TPA Stream Input
	HAL_AUDIO_IN_PORT_SPDIF 	=  2,	// From SERIAL INTERFACE 0
	HAL_AUDIO_IN_PORT_SIF		=  3,	// From Analog Front End (SIF)
	HAL_AUDIO_IN_PORT_ADC		=  4,	// Fron ADC Input
	HAL_AUDIO_IN_PORT_HDMI		=  5,	// From HDMI
	HAL_AUDIO_IN_PORT_I2S		=  6,	// From I2S
	HAL_AUDIO_IN_PORT_SYSTEM	=  7,	// From System
} HAL_AUDIO_IN_PORT_T;

/**
 * HAL AUDIO Dual-Mono Output Mode Type.
 *
 */
typedef enum
{
	HAL_AUDIO_DUALMONO_MODE_LR		= 0,
	HAL_AUDIO_DUALMONO_MODE_LL		= 1,
	HAL_AUDIO_DUALMONO_MODE_RR		= 2,
	HAL_AUDIO_DUALMONO_MODE_MIX		= 3,
} HAL_AUDIO_DUALMONO_MODE_T;

/**
 * HAL AUDIO SPDIF Type.
 *
 */
typedef  enum
{
	HAL_AUDIO_SPDIF_NONE		= 0,
	HAL_AUDIO_SPDIF_PCM			= 1,
	HAL_AUDIO_SPDIF_AUTO		= 2,
	HAL_AUDIO_SPDIF_AUTO_AAC	= 3,
	HAL_AUDIO_SPDIF_MIXED_DD	= 4,	/* Google Mixed DD(AC3 Encoding) */
} HAL_AUDIO_SPDIF_MODE_T ;

/**
 * HAL AUDIO HDMI Format Type.
 *
 */
typedef enum
{
	HAL_AUDIO_HDMI_DVI			= 0,
	HAL_AUDIO_HDMI_NO_AUDIO		= 1,
	HAL_AUDIO_HDMI_PCM			= 2,
	HAL_AUDIO_HDMI_AC3			= 3,
	HAL_AUDIO_HDMI_DTS			= 4,
	HAL_AUDIO_HDMI_AAC			= 5,
	HAL_AUDIO_HDMI_DEFAULT		= 6,
	HAL_AUDIO_HDMI_MPEG			= 10,
	HAL_AUDIO_HDMI_DTS_HD_MA	= 11,
	HAL_AUDIO_HDMI_DTS_EXPRESS	= 12,
	HAL_AUDIO_HDMI_DTS_CD		= 13,
} HAL_AUDIO_HDMI_TYPE_T;

/**
 * AUDIO Sampling Frequency Index.
 */
typedef enum
{
	HAL_AUDIO_SAMPLING_FREQ_NONE		=	     0,
	HAL_AUDIO_SAMPLING_FREQ_4_KHZ		=	  4000,
	HAL_AUDIO_SAMPLING_FREQ_8_KHZ		=	  8000,
	HAL_AUDIO_SAMPLING_FREQ_11_025KHZ	=	 11025,
	HAL_AUDIO_SAMPLING_FREQ_12_KHZ		=	 12000,
	HAL_AUDIO_SAMPLING_FREQ_16_KHZ		=	 16000,
	HAL_AUDIO_SAMPLING_FREQ_22_05KHZ	=	 22050,
	HAL_AUDIO_SAMPLING_FREQ_24_KHZ		=	 24000,
	HAL_AUDIO_SAMPLING_FREQ_32_KHZ		=	 32000,
	HAL_AUDIO_SAMPLING_FREQ_44_1KHZ		=	 44100,
	HAL_AUDIO_SAMPLING_FREQ_48_KHZ		=	 48000,
	HAL_AUDIO_SAMPLING_FREQ_64_KHZ		=	 64000,
	HAL_AUDIO_SAMPLING_FREQ_88_2KHZ		=	 88200,
	HAL_AUDIO_SAMPLING_FREQ_96_KHZ		=	 96000,
	HAL_AUDIO_SAMPLING_FREQ_128_KHZ		=	128000,
	HAL_AUDIO_SAMPLING_FREQ_176_4KHZ	=	176400,
	HAL_AUDIO_SAMPLING_FREQ_192_KHZ		=	192000,
	HAL_AUDIO_SAMPLING_FREQ_768_KHZ		=	768000,
	HAL_AUDIO_SAMPLING_FREQ_DEFAULT		=	999000,
} HAL_AUDIO_SAMPLING_FREQ_T;

/**
 * HAL AUDIO Country Type.
 *
 */
typedef  enum
{
	HAL_AUDIO_SIF_TYPE_NONE				= 0x0000,		///< INIT TYPE : NONE
	HAL_AUDIO_SIF_ATSC_SELECT			= 0x0001,		///< INIT TYPE : TV Systems for A2 enabled in default ATSC system
	HAL_AUDIO_SIF_KOREA_A2_SELECT		= 0x0002,		///< INIT TYPE : TV Systems for A2 enabled in Korea A2 system
	HAL_AUDIO_SIF_BTSC_SELECT			= 0x0004,		///< INIT TYPE : TV Systems for BTSC enabled in ATSC(CO, CF) or DVB(Taiwan) system
	HAL_AUDIO_SIF_BTSC_BR_SELECT		= 0x0008,		///< INIT TYPE : TV Systems for BTSC enabled in ATSC(Brazil) system
	HAL_AUDIO_SIF_BTSC_US_SELECT		= 0x0010,		///< INIT TYPE : TV Systems for BTSC enabled in ATSC(US) system
	HAL_AUDIO_SIF_DVB_SELECT 			= 0x0020,		///< INIT TYPE : TV Systems for EU in default DVB system
	HAL_AUDIO_SIF_DVB_ID_SELECT			= 0x0040,		///< INIT TYPE : TV Systems for ID(Indonesia) in DVB(PAL B/G) system
	HAL_AUDIO_SIF_DVB_IN_SELECT			= 0x0080,		///< INIT TYPE : TV Systems for IN(India) in DVB(PAL B) system
	HAL_AUDIO_SIF_DVB_CN_SELECT			= 0x0100,		///< INIT TYPE : TV Systems for CN(China, Hong Kone) in DVB system
	HAL_AUDIO_SIF_DVB_AJJA_SELECT		= 0x0200,		///< INIT TYPE : TV Systems for AJ(Asia JooDong), JA(JooAang Asia) in DVB system
	HAL_AUDIO_SIF_TYPE_MAX				= 0x0FFF,		///< INIT TYPE : MAX
} HAL_AUDIO_SIF_TYPE_T;

/**
 * HAL AUDIO SIF Input Source Type.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_INPUT_EXTERNAL	= 0,
	HAL_AUDIO_SIF_INPUT_INTERNAL	= 1,
} HAL_AUDIO_SIF_INPUT_T;

/**
 * HAL AUDIO SIF Sound System Type.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_SYSTEM_BG			= 0x00,
	HAL_AUDIO_SIF_SYSTEM_I			= 0x01,
	HAL_AUDIO_SIF_SYSTEM_DK			= 0x02,
	HAL_AUDIO_SIF_SYSTEM_L			= 0x03,
	HAL_AUDIO_SIF_SYSTEM_MN			= 0x04,
	HAL_AUDIO_SIF_SYSTEM_UNKNOWN	= 0xF0,
} HAL_AUDIO_SIF_SOUNDSYSTEM_T;

/**
 * HAL AUDIO SIF Sound Standard Mode Type.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_MODE_DETECT	= 0,
	HAL_AUDIO_SIF_BG_NICAM		= 1,
	HAL_AUDIO_SIF_BG_FM			= 2,
	HAL_AUDIO_SIF_BG_A2			= 3,
	HAL_AUDIO_SIF_I_NICAM		= 4,
	HAL_AUDIO_SIF_I_FM			= 5,
	HAL_AUDIO_SIF_DK_NICAM		= 6,
	HAL_AUDIO_SIF_DK_FM			= 7,
	HAL_AUDIO_SIF_DK1_A2		= 8,
	HAL_AUDIO_SIF_DK2_A2		= 9,
	HAL_AUDIO_SIF_DK3_A2		= 10,
	HAL_AUDIO_SIF_L_NICAM		= 11,
	HAL_AUDIO_SIF_L_AM			= 12,
	HAL_AUDIO_SIF_MN_A2			= 13,
	HAL_AUDIO_SIF_MN_BTSC		= 14,
	HAL_AUDIO_SIF_MN_EIAJ		= 15,
	HAL_AUDIO_SIF_NUM_SOUND_STD	= 16,
} HAL_AUDIO_SIF_STANDARD_T;

/**
 * HAL AUDIO SIF Analog Audio Setting Parameter.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_SET_PAL_MONO					=	0x00,	// PAL Mono
	HAL_AUDIO_SIF_SET_PAL_MONO_FORCED			=	0x01,	// PAL Mono Force Mono
	HAL_AUDIO_SIF_SET_PAL_STEREO				=	0x02,	// PAL Stereo
	HAL_AUDIO_SIF_SET_PAL_STEREO_FORCED			=	0x03,	// PAL Stereo Force Mono
	HAL_AUDIO_SIF_SET_PAL_DUALI					=	0x04,	// PAL Dual I
	HAL_AUDIO_SIF_SET_PAL_DUALII				=	0x05,	// PAL Dual II
	HAL_AUDIO_SIF_SET_PAL_DUALI_II				=	0x06,	// PAL Dual I+II
	HAL_AUDIO_SIF_SET_PAL_NICAM_MONO			=	0x07,	// PAL NICAM Mono
	HAL_AUDIO_SIF_SET_PAL_NICAM_MONO_FORCED		=	0x08,	// PAL NICAM Mono Force Mono
	HAL_AUDIO_SIF_SET_PAL_NICAM_STEREO			=	0x09,	// PAL NICAM Stereo
	HAL_AUDIO_SIF_SET_PAL_NICAM_STEREO_FORCED	=	0x0A,	// PAL NICAM Stereo Force Mono
	HAL_AUDIO_SIF_SET_PAL_NICAM_DUALI			=	0x0B,	// PAL NICAM Dual I
	HAL_AUDIO_SIF_SET_PAL_NICAM_DUALII			=	0x0C,	// PAL NICAM Dual II
	HAL_AUDIO_SIF_SET_PAL_NICAM_DUALI_II		=	0x0D,	// PAL NICAM Dual I+II
	HAL_AUDIO_SIF_SET_PAL_NICAM_DUAL_FORCED		=	0x0E,	// PAL NICAM Dual Forced Mono(Not Supported)
	HAL_AUDIO_SIF_SET_PAL_UNKNOWN				=	0x0F,	// PAL Unkown State
	HAL_AUDIO_SIF_SET_NTSC_A2_MONO				=	0x10,	// NTSC(A2) Mono
	HAL_AUDIO_SIF_SET_NTSC_A2_STEREO			=	0x11,	// NTSC(A2) Stereo
	HAL_AUDIO_SIF_SET_NTSC_A2_SAP				=	0x12,	// NTSC(A2) SAP
	HAL_AUDIO_SIF_SET_NTSC_A2_UNKNOWN			=	0x13,	// NTSC(A2) Unkown State
	HAL_AUDIO_SIF_SET_NTSC_BTSC_MONO			=	0x14,	// NTSC(BTSC) Mono
	HAL_AUDIO_SIF_SET_NTSC_BTSC_STEREO			=	0x15,	// NTSC(BTSC) Stereo
	HAL_AUDIO_SIF_SET_NTSC_BTSC_SAP_MONO		=	0x16,	// NTSC(BTSC) SAP Mono
	HAL_AUDIO_SIF_SET_NTSC_BTSC_SAP_STEREO		=	0x17,	// NTSC(BTSC) SAP Stereo
	HAL_AUDIO_SIF_SET_NTSC_BTSC_UNKNOWN			=	0x18,	// NTSC(BTSC) Unkown State
} HAL_AUDIO_SIF_MODE_SET_T;

/**
 * HAL AUDIO SIF Standard Type.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_NICAM					= 0,
	HAL_AUDIO_SIF_A2					= 1,
	HAL_AUDIO_SIF_FM					= 2,
	HAL_AUDIO_SIF_DETECTING_AVALIBILITY = 3,
} HAL_AUDIO_SIF_AVAILE_STANDARD_T;

/**
 * HAL AUDIO SIF Exist Type.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_ABSENT	 			= 0,
	HAL_AUDIO_SIF_PRESENT 		 		= 1,
	HAL_AUDIO_SIF_DETECTING_EXSISTANCE	= 2,
} HAL_AUDIO_SIF_EXISTENCE_INFO_T;

/**
 * HAL AUDIO SIF Analog Audio Getting Parameter.
 *
 */
typedef enum
{
	HAL_AUDIO_SIF_GET_PAL_MONO				=	0x00,	// PAL Mono
	HAL_AUDIO_SIF_GET_PAL_STEREO			=	0x01,	// PAL Stereo
	HAL_AUDIO_SIF_GET_PAL_DUAL				=	0x02,	// PAL Dual
	HAL_AUDIO_SIF_GET_PAL_NICAM_MONO		=	0x03,	// PAL NICAM Mono
	HAL_AUDIO_SIF_GET_PAL_NICAM_STEREO		=	0x04,	// PAL NICAM Stereo
	HAL_AUDIO_SIF_GET_PAL_NICAM_DUAL		=	0x05,	// PAL NICAM Dual
	HAL_AUDIO_SIF_GET_PAL_UNKNOWN			=	0x06,	// PAL Unkown State
	HAL_AUDIO_SIF_GET_NTSC_A2_MONO			=	0x10,	// NTSC(A2) Mono
	HAL_AUDIO_SIF_GET_NTSC_A2_STEREO		=	0x11,	// NTSC(A2) Stereo
	HAL_AUDIO_SIF_GET_NTSC_A2_SAP			=	0x12,	// NTSC(A2) SAP
	HAL_AUDIO_SIF_GET_NTSC_A2_UNKNOWN		=	0x13,	// NTSC(A2) Unkown State
	HAL_AUDIO_SIF_GET_NTSC_BTSC_MONO		=	0x14,	// NTSC(BTSC) Mono
	HAL_AUDIO_SIF_GET_NTSC_BTSC_STEREO		=	0x15,	// NTSC(BTSC) Stereo
	HAL_AUDIO_SIF_GET_NTSC_BTSC_SAP_MONO	=	0x16,	// NTSC(BTSC) SAP Mono
	HAL_AUDIO_SIF_GET_NTSC_BTSC_SAP_STEREO	=	0x17,	// NTSC(BTSC) SAP Stereo
	HAL_AUDIO_SIF_GET_NTSC_BTSC_UNKNOWN		=	0x18,	// NTSC(BTSC) Unkown State
} HAL_AUDIO_SIF_MODE_GET_T;

/**
 * HAL AUDIO Copy Protection Type
 *
 */
typedef  enum
{
	HAL_AUDIO_SPDIF_COPY_FREE		= 0,	/* cp-bit : 1, L-bit : 0 */
	HAL_AUDIO_SPDIF_COPY_NO_MORE	= 1,	/* cp-bit : 0, L-bit : 1 */
	HAL_AUDIO_SPDIF_COPY_ONCE		= 2,	/* cp-bit : 0, L-bit : 0 */
	HAL_AUDIO_SPDIF_COPY_NEVER		= 3,	/* cp-bit : 0, L-bit : 1 */
} HAL_AUDIO_SPDIF_COPYRIGHT_T;

/**
 * HAL AUDIO Volume Structure
 * dB Scale		:     main,	fine
 *   -127dB Min 	: 0x00(-127dB)	 0x00(0 dB)
 * - 125.9325 dB 	: 0x01(-126 dB), 0x01(1/16 dB)
 * - 121.8125 dB 	: 0x05(-122 dB), 0x03(3/16 dB)
 * -0.9375dB        : 0x7E(-1dB),    0x01(1/16dB)
 *    0 dB          : 0x7F(0 dB),    0x00(0 dB)
 *    30dB          : 0x9D(30dB),    0x00(0 dB)
 */
typedef struct HAL_AUDIO_VOLUME
{
	UINT8	mainVol;	// 1 dB step, -127 ~ +30 dB.
	UINT8	fineVol;  	// 1/16 dB step, 0dB ~ 15/16dB
} HAL_AUDIO_VOLUME_T ;

/**
 * HAL AUDIO DOLBY DRC Mode.
 *
 * @see
*/
typedef  enum
{
	HAL_AUDIO_DOLBY_LINE_MODE	= 0,
	HAL_AUDIO_DOLBY_RF_MODE		= 1,
	HAL_AUDIO_DOLBY_DRC_OFF		= 2,
} HAL_AUDIO_DOLBY_DRC_MODE_T;

/**
 * HAL AUDIO DOLBY Downmix Mode.
 *
*/
typedef  enum
{
	HAL_AUDIO_LORO_MODE		= 0,
	HAL_AUDIO_LTRT_MODE		= 1,
} HAL_AUDIO_DOWNMIX_MODE_T;

/**
 * HAL AUDIO Channel Mode.
 *
 * @see
*/
typedef enum HAL_AUDIO_MODE
{
	HAL_AUDIO_MODE_MONO	 			= 0,
	HAL_AUDIO_MODE_JOINT_STEREO 	= 1,
	HAL_AUDIO_MODE_STEREO		 	= 2,
	HAL_AUDIO_MODE_DUAL_MONO 		= 3,
	HAL_AUDIO_MODE_MULTI			= 4,
	HAL_AUDIO_MODE_UNKNOWN			= 5,
} HAL_AUDIO_MODE_T;

/**
 * HAL Audio Endian of PCM..
 *
 */
typedef enum
{
	HAL_AUDIO_PCM_LITTLE_ENDIAN	= 0,
	HAL_AUDIO_PCM_BIG_ENDIAN	= 1,
} HAL_AUDIO_PCM_ENDIAN_T;

/**
 * HAL Audio Signed of PCM..
 *
 */
typedef enum
{
	HAL_AUDIO_PCM_SIGNED	= 0,
	HAL_AUDIO_PCM_UNSIGNED	= 1,
} HAL_AUDIO_PCM_SIGNED_T;

/**
 * HAL Audio Clip Play Status Type.
 *
 */
typedef enum
{
	HAL_AUDIO_CLIP_NONE		= 0,
	HAL_AUDIO_CLIP_PLAY		= 1,
	HAL_AUDIO_CLIP_STOP		= 2,
	HAL_AUDIO_CLIP_RESUME	= 3,
	HAL_AUDIO_CLIP_PAUSE	= 4,
	HAL_AUDIO_CLIP_DONE		= 5,
} HAL_AUDIO_CLIP_STATUS_T;

/**
 * HAL AUDIO Trick Mode Type.
 *
 */
typedef  enum
{
	HAL_AUDIO_TRICK_NONE				= 0, 	///<  rate : None, TP Live Play
	HAL_AUDIO_TRICK_PAUSE				= 1, 	///<  rate : Pause, DVR Play
	HAL_AUDIO_TRICK_NORMAL_PLAY			= 2, 	///<  rate : Normal Play, DVR Play
	HAL_AUDIO_TRICK_SLOW_MOTION_0P25X 	= 3,	///<  rate : 0.25 Play
	HAL_AUDIO_TRICK_SLOW_MOTION_0P50X 	= 4,	///<  rate : 0.50 Play
	HAL_AUDIO_TRICK_SLOW_MOTION_0P80X 	= 5,	///<  rate : 0.80 Play
	HAL_AUDIO_TRICK_FAST_FORWARD_1P20X 	= 6, 	///<  rate : 1.20 Play
	HAL_AUDIO_TRICK_FAST_FORWARD_1P50X 	= 7, 	///<  rate : 1.50 Play
	HAL_AUDIO_TRICK_FAST_FORWARD_2P00X	= 8,	///<  rate : 2.00 Play
} HAL_AUDIO_TRICK_MODE_T;

/**
 *  HAL AUDIO LGSE PARAMETER Setting TYPE
 *
 */
typedef enum
{
	HAL_AUDIO_LGSE_INIT_ONLY = 0, // "init only" parameter will be written
	HAL_AUDIO_LGSE_VARIABLES = 1, // "variables" will be writen
	HAL_AUDIO_LGSE_ALL		 = 2, // "init only" and "variables" will be written simultaneously
} HAL_AUDIO_LGSE_DATA_MODE_T;

/**
 *  HAL AUDIO LGSE PARAMETER ACCESS TYPE
 *
 */
typedef enum
{
	HAL_AUDIO_LGSE_WRITE	= 0, // "pParams" data will be written to DSP
	HAL_AUDIO_LGSE_READ		= 1, // "pParams" data will be read from DSP to CPU
} HAL_AUDIO_LGSE_DATA_ACCESS_T;

/**
 *  HAL AUDIO LGSE FN004 MODE VARIABLE TYPE
 *
 */
typedef enum
{
	HAL_AUDIO_LGSE_MODE_VARIABLES0	= 0, // "VARIABLES_00"will be written
	HAL_AUDIO_LGSE_MODE_VARIABLES1	= 1, // "VARIABLES_01"will be written
	HAL_AUDIO_LGSE_MODE_VARIABLES2	= 2, // "VARIABLES_02"will be written
	HAL_AUDIO_LGSE_MODE_VARIABLES3	= 3, // "VARIABLES_03"will be written
	HAL_AUDIO_LGSE_MODE_VARIABLES4	= 4, // "VARIABLES_04"will be written
	HAL_AUDIO_LGSE_MODE_VARIABLESALL= 5  // All "VARIABLES" will be written simultaneously. Data will be arranged from 0 to 4.
} HAL_AUDIO_LGSE_VARIABLE_MODE_T;

/**
 *  HAL AUDIO LGSE FUNCTION MODE TYPE
 *
 */
typedef enum
{
	HAL_AUDIO_LGSE_LGSEFN000		= 0,
	HAL_AUDIO_LGSE_LGSEFN001		= 1,
	HAL_AUDIO_LGSE_LGSEFN003		= 3,
	HAL_AUDIO_LGSE_LGSEFN004		= 4,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN005		= 5,	//new webOS 1.0
	HAL_AUDIO_LGSE_LGSEFN006		= 6,
	HAL_AUDIO_LGSE_LGSEFN007		= 7,	//old
	HAL_AUDIO_LGSE_LGSEFN008		= 8,
	HAL_AUDIO_LGSE_LGSEFN009		= 9,
	HAL_AUDIO_LGSE_LGSEFN010		= 10,
	HAL_AUDIO_LGSE_LGSEFN011		= 11,
	HAL_AUDIO_LGSE_LGSEFN012		= 12,
	HAL_AUDIO_LGSE_LGSEFN013		= 13,
	HAL_AUDIO_LGSE_LGSEFN014		= 14,	//new webOS 1.0
	HAL_AUDIO_LGSE_LGSEFN016		= 16,	//new webOS 1.0
	HAL_AUDIO_LGSE_LGSEFN017		= 17,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN019		= 19,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN020		= 20,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN022		= 22,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN023		= 23,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN024		= 24,	//new webOS 1.5
	HAL_AUDIO_LGSE_LGSEFN004MODE1	= 41,
	HAL_AUDIO_LGSE_LGSEFN004MODE2	= 42,
	HAL_AUDIO_LGSE_LGSEFN004MODE3	= 43,
	HAL_AUDIO_LGSE_MODE 			= 100,
	HAL_AUDIO_LGSE_MAIN 			= 101,	//new webOS 1.0
} HAL_AUDIO_LGSE_FUNCLIST_T;

/**
 * HAL AUDIO Encoder Index.
 *
 */
typedef  enum
{
	HAL_AUDIO_AENC0		= 0,
	HAL_AUDIO_AENC1		= 1,
	HAL_AUDIO_AENC_MAX	= HAL_AUDIO_AENC1,
} HAL_AUDIO_AENC_INDEX_T ;

/**
 * HAL AUDIO AENC encode format
 */
typedef enum
{
	HAL_AUDIO_AENC_ENCODE_MP3 	= 0,	/* Encode MP3 format */
	HAL_AUDIO_AENC_ENCODE_AAC	= 1,	/* Encode AAC format */
} HAL_AUDIO_AENC_ENCODING_FORMAT_T;

/**
 * HAL AUDIO AENC encode Status
 */
typedef enum
{
	HAL_AUDIO_AENC_STATUS_STOP	 	= 0,
	HAL_AUDIO_AENC_STATUS_PLAY	 	= 1,
	HAL_AUDIO_AENC_STATUS_ABNORMAL 	= 2,
} HAL_AUDIO_AENC_STATUS_T;

/**
 * HAL AUDIO AENC encode # of channel
 */
typedef enum
{
	HAL_AUDIO_AENC_MONO 	= 0,
	HAL_AUDIO_AENC_STEREO 	= 1,
} HAL_AUDIO_AENC_CHANNEL_T;

/**
 * HAL AUDIO AENC encode bitrate
 */
typedef enum
{
	HAL_AUDIO_AENC_BIT_48K		= 0,
	HAL_AUDIO_AENC_BIT_56K		= 1,
	HAL_AUDIO_AENC_BIT_64K		= 2,
	HAL_AUDIO_AENC_BIT_80K		= 3,
	HAL_AUDIO_AENC_BIT_112K		= 4,
	HAL_AUDIO_AENC_BIT_128K		= 5,
	HAL_AUDIO_AENC_BIT_160K		= 6,
	HAL_AUDIO_AENC_BIT_192K		= 7,
	HAL_AUDIO_AENC_BIT_224K		= 8,
	HAL_AUDIO_AENC_BIT_256K		= 9,
	HAL_AUDIO_AENC_BIT_320K		= 10,
} HAL_AUDIO_AENC_BITRATE_T;

/**
 * HAL AUDIO PCM Input Mode.
 *
 */
typedef  enum
{
	HAL_AUDIO_PCM_SB_PCM		= 0,	/* Sound Bar Output : No SE Ouput */
	HAL_AUDIO_PCM_SB_CANVAS		= 1,	/* Sound Bar Output : SE Ouput */
	HAL_AUDIO_PCM_I2S			= 2,	/* I2S(L/R) Input for AEC(Audio Echo Cancellation) */
	HAL_AUDIO_PCM_I2S0			= HAL_AUDIO_PCM_I2S,	/* I2S0(L/R) Input for AEC(Audio Echo Cancellation) */
	HAL_AUDIO_PCM_INPUT_MAX		= 3,	/* PCM Input Max */
} HAL_AUDIO_PCM_INPUT_T;

/**
 * HAL AUDIO Input/Output I2S Port.
 *
 */
typedef  enum
{
	HAL_AUDIO_INOUT_I2S0		= 0x01,	/* I2S0(L/R) Input/Output for AEC(Audio Echo Cancellation) */
	HAL_AUDIO_INOUT_I2S1		= 0x02, /* I2S1(Lh/Rh) Input/Output for AEC(Audio Echo Cancellation) */
	HAL_AUDIO_INOUT_I2S2		= 0x04, /* I2S2(Lr/Rr) Input/Output for AEC(Audio Echo Cancellation) */
	HAL_AUDIO_INOUT_I2S3		= 0x08, /* I2S3(C/Lf) Input/Output for AEC(Audio Echo Cancellation) */
} HAL_AUDIO_INOUT_I2S_T;

/**
 * HAL AUDIO AC3 ES Info
 *
 * @see
*/
typedef struct HAL_AUDIO_AC3_ES_INFO{
    UINT8 bitRate;
    UINT8 sampleRate;
    UINT8 channelNum;
    UINT8 EAC3;       	/* AC3 0x0, EAC3 0x1*/
} HAL_AUDIO_AC3_ES_INFO_T;

/**
 * HAL AUDIO MPEG ES Info
 *
 * @see
*/
typedef struct HAL_AUDIO_MPEG_ES_INFO{
    UINT8 bitRate;
    UINT8 sampleRate;
    UINT8 layer;
    UINT8 channelNum;
} HAL_AUDIO_MPEG_ES_INFO_T;

/**
 * HAL AUDIO HE-AAC ES Info
 *
 * @see
*/
typedef struct HAL_AUDIO_HEAAC_ES_INFO{
    UINT8 version;    		  	/* AAC = 0x0, HE-AACv1 = 0x1, HE-AACv2 = 0x2 */
    UINT8 transmissionformat;   /* LOAS/LATM = 0x0, ADTS = 0x1*/
    UINT8 channelNum;
} HAL_AUDIO_HEAAC_ES_INFO_T;

/**
 * HAL AUDIO ES Info
 *
 * @see
*/
typedef struct HAL_AUDIO_ES_INFO{
    HAL_AUDIO_SRC_TYPE_T 		adecFormat;
    HAL_AUDIO_MODE_T 			audioMode;
    HAL_AUDIO_HEAAC_ES_INFO_T 	heAAcEsInfo;
    HAL_AUDIO_MPEG_ES_INFO_T  	mpegESInfo;
    HAL_AUDIO_AC3_ES_INFO_T   	ac3ESInfo;
} HAL_AUDIO_ES_INFO_T;

/**
 * HAL AUDIO Clip Decoder Play Info
 *
*/
typedef struct HAL_AUDIO_CLIP_DEC_PARAM{
	HAL_AUDIO_SRC_TYPE_T 		clipType;
	UINT32 						repeatNumber;		/* The play number of audio clip. */
} HAL_AUDIO_CLIP_DEC_PARAM_T;

/**
 * HAL AUDIO Clip Mixer Play Info
 *
*/
typedef struct HAL_AUDIO_CLIP_MIX_PARAM{
	UINT32 						numOfChannel;		/* 2  : stereo, 1 : mono,  8 : 8 channel */
	UINT32 						bitPerSample;		/* 16 : 16 bit, 8 : 8 bit 24 : 24bit */
	HAL_AUDIO_SAMPLING_FREQ_T	samplingFreq;		/* 48000 : 48Khz, 44100 : 44.1Khz */
	HAL_AUDIO_PCM_ENDIAN_T		endianType;			/* 0  : little endian, 1 : big endian */
	HAL_AUDIO_PCM_SIGNED_T		signedType;			/* 0  : signed PCM, 1 : unsigned PCM */
	UINT32 						repeatNumber;		/* The play number of audio clip. */
} HAL_AUDIO_CLIP_MIX_PARAM_T;

/**
 * HAL AUDIO Sound Bar Set Info
 *
*/
typedef struct HAL_AUDIO_SB_SET_INFO{
    UINT32 			barId;
    UINT32 			volume;
	BOOLEAN 		bMuteOnOff;
	BOOLEAN 		bPowerOnOff;
} HAL_AUDIO_SB_SET_INFO_T;

/**
 * HAL AUDIO Sound Bar(Canvas) Command Set Info
 *
*/
typedef struct HAL_AUDIO_SB_SET_CMD{
	BOOLEAN 		bAutoVolume;
	UINT8	 		wooferLevel;
    UINT32 			reservedField;
} HAL_AUDIO_SB_SET_CMD_T;

/**
 * HAL AUDIO Sound Bar Get Info
 *
*/
typedef struct HAL_AUDIO_SB_GET_INFO{
    UINT32 			subFrameID;
    UINT8 			subFrameData;
	UINT8 			subFramePowerData;
	UINT8 			subframeChecksum;
} HAL_AUDIO_SB_GET_INFO_T;

/**
 * HAL AUDIO Sound Bar(Canvas) Command Get Info
 *
*/
typedef struct HAL_AUDIO_SB_GET_CMD{
    UINT32 			subFrameID;
    UINT32 			subFrameData;
	UINT8 			subframeChecksum;
} HAL_AUDIO_SB_GET_CMD_T;

/**
 * HAL AUDIO AENC Info
 *
*/
typedef struct HAL_AUDIO_AENC_INFO
{
	//Get Info for debugging on DDI
	HAL_AUDIO_AENC_STATUS_T				status;	// current ENC Status
	HAL_AUDIO_AENC_ENCODING_FORMAT_T	codec;	// current ENC Codec

	UINT32	errorCount;							// current ENC error counter
	UINT32	inputCount;							// current ENC input counter - we distinguish whether or not enter data from input.
	UINT32	underflowCount;						// current ENC underflowCnt in kernel space - we distinguish which module have problem between muxer and encdoer
	UINT32	overflowCount;						// current ENC overflowCnt - we distinguish

	//Set Info - it is applied realtime, no matter stop&start
	HAL_AUDIO_AENC_CHANNEL_T	channel;		// number of channel
	HAL_AUDIO_AENC_BITRATE_T	bitrate;		// bitrate
} HAL_AUDIO_AENC_INFO_T;

/**
 *  HAL AUDIO PCM DATA TYPE
 *
 */
typedef struct HAL_AUDIO_PCM_DATA
{
    UINT8	index;		// PCM index
	UINT32	pts;		// PTS(unit : ms, 90Khz clock base, max value : 47721858 = 0xFFFFFFFF/90)
	UINT8	*pData;		// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
} HAL_AUDIO_PCM_DATA_T;

/**
 * HAL AUDIO PCM Capture Info
 * The PCM other parameter is pre-defined as follows.(little endian, singed)
 *
*/
typedef struct HAL_AUDIO_PCM_INFO{
	HAL_AUDIO_SAMPLING_FREQ_T	samplingFreq;		/* 48000 : 48Khz, 44100 : 44.1Khz */
	UINT32 						numOfChannel;		/* 2  : stereo, 1 : mono */
	UINT32 						bitPerSample;		/* 16 : 16 bit, 8 : 8 bit */
	HAL_AUDIO_INOUT_I2S_T		inOutI2SPort;		/* I2S Input/Output Port Bit Mask.(I2S0 & I2S1 => 0x03) */
} HAL_AUDIO_PCM_INFO_T;

/**
 *  HAL AUDIO CALLBACK TYPE
 *
 */
typedef enum
{
	HAL_AUDIO_WARNING_DEFAULT 	  		= 0,
	HAL_AUDIO_WARNING_PIP_OVERRUN 		= 1, //AAC5.1 ch X 2 decoding인 경우, performance이슈로 callback notify on H13
	HAL_AUDIO_WARNING_1KHZ_TONE_ON 		= 2, //ERP Power Requirement : 1KHz Tone Detect On(Woofer Amp PWM OFF)
	HAL_AUDIO_WARNING_1KHZ_TONE_OFF		= 3, //ERP Power Requirement : 1KHz Tone Detect Off(Woofer Amp PWM ON)
} HAL_AUDIO_WARNING_IDX;

/**
 *  HAL AUDIO AENC DATA MESSAGE TYPE
 *
 */
typedef struct HAL_AUDIO_AENC_DATA
{
	UINT32	index;		// Encoder index
	UINT64	pts;		// PTS
	UINT8	*pData; 	// pointer to Audio Data
	UINT32	dataLen;	// Audio Data Length
	UINT8	*pRStart;	// start pointer of buffer
	UINT8	*pREnd; 	// end pointer of buffer
}HAL_AUDIO_AENC_DATA_T;

/**
 *  HAL AUDIO CALLBACK FUNCTION
 *
 */
typedef DTV_STATUS_T (*pfnAdecoderClipDone)(HAL_AUDIO_ADEC_INDEX_T adecIndex);
typedef DTV_STATUS_T (*pfnAmixerClipDone)(HAL_AUDIO_MIXER_INDEX_T amixIndex);
typedef DTV_STATUS_T (*pfnAENCDataHandling)(HAL_AUDIO_AENC_DATA_T *pMsg);
typedef DTV_STATUS_T (*pfnPCMDataHandling)(HAL_AUDIO_PCM_DATA_T *pMsg);
typedef DTV_STATUS_T (*pfnPCMSending)(UINT8 *pBuf, UINT16 length);
typedef DTV_STATUS_T (*pfnADECWarning)(HAL_AUDIO_WARNING_IDX index);
typedef DTV_STATUS_T (*pfnLGSESmartSound)(UINT32 *pData, UINT16 length);

/**
 * HAL AUDIO Decoder Debug Information Definition
 *
 */
typedef struct HAL_AUDIO_ADEC_INFO
{
	HAL_AUDIO_SRC_TYPE_T   				curAdecFormat;				/* Current ADEC Format Type */
	HAL_AUDIO_SRC_TYPE_T   				prevAdecFormat;				/* Previous ADEC Format Type */
	HAL_AUDIO_IN_PORT_T					curAdecInputPort;			/* Current ADEC Input Type */
	HAL_AUDIO_IN_PORT_T					prevAdecInputPort;			/* Previous ADEC Input Type */
	UINT8								curADCPortNum;				/* Current ADC Port Number */
	UINT8								prevADCPortNum;				/* Previous ADC Port Number */
	HAL_AUDIO_HDMI_INDEX_T				curHDMIPortNum;				/* Current HDMI Port Number */
	HAL_AUDIO_HDMI_INDEX_T				prevHDMIPortNum;			/* Previous HDMI Port Number */

	HAL_AUDIO_RESOURCE_T				curTPInputPort;				/* Current ADEC TP Input Port(HAL_AUDIO_RESOURCE_SDEC0/1) */
	HAL_AUDIO_RESOURCE_T				prevTPInputPort;			/* Previous ADEC TP Input Port(HAL_AUDIO_RESOURCE_SDEC0/1) */
	HAL_AUDIO_RESOURCE_T				curAENCInput;				/* Current ADEC to AENC Input Port(HAL_AUDIO_RESOURCE_AENC0/1) */
	HAL_AUDIO_RESOURCE_T				prevAENCInput;				/* Previous ADEC to AENC Input Port(HAL_AUDIO_RESOURCE_AENC0/1) */

	BOOLEAN								bAdecStart;					/* Current ADEC Start Status */
	BOOLEAN								bAdecESExist;				/* Current ADEC ES Exist Status */
	HAL_AUDIO_DUALMONO_MODE_T			curAdecDualmonoMode;		/* Current ADEC Dual Mono Mode Status */

	HAL_AUDIO_SIF_INPUT_T 				sifSource;					/* Currnet SIF Source Input Status */
	HAL_AUDIO_SIF_TYPE_T 				curSifType;					/* Currnet SIF Type Status */
	BOOLEAN								bHighDevOnOff;				/* Currnet High DEV ON/OFF Status */
	HAL_AUDIO_SIF_SOUNDSYSTEM_T			curSifBand;					/* Currnet SIF Sound Band(Sound System) */
	HAL_AUDIO_SIF_STANDARD_T			curSifStandard;				/* Currnet SIF Sound Standard */
	HAL_AUDIO_SIF_EXISTENCE_INFO_T		curSifIsA2;					/* Currnet SIF A2 Exist Status */
	HAL_AUDIO_SIF_EXISTENCE_INFO_T		curSifIsNicam;				/* Currnet SIF NICAM Exist Status */
	HAL_AUDIO_SIF_MODE_SET_T 			curSifModeSet;				/* Currnet SIF Sound Mode Set Status */
	HAL_AUDIO_SIF_MODE_GET_T 			curSifModeGet;				/* Currnet SIF Sound Mode Get Status */

	HAL_AUDIO_HDMI_TYPE_T  				curHdmiAudioType;			/* Current HDMI Audio Format Type */
	HAL_AUDIO_SAMPLING_FREQ_T			curHdmiSamplingFreq;		/* Current HDMI PCM Sampling Frequency */
	HAL_AUDIO_SPDIF_COPYRIGHT_T			curHdmiCopyInfo;			/* HDMI Input Copy Info.(Cbit info 16bit : to set FREE/ONCE/NEVER for SPDIF Ouput) */
} HAL_AUDIO_ADEC_INFO_T;

/**
 * HAL AUDIO HAL Driver Information Definition
 *
 */
typedef struct HAL_AUDIO_COMMON_INFO
{
	HAL_AUDIO_SPDIF_MODE_T 		curAudioSpdifMode;			/* Current Audio SPDIF Output Mode. */
	BOOLEAN						bAudioSpdifOutPCM;			/* Current Audio SPDIF Output is PCM. */
	BOOLEAN						bCurAudioSpdifOutPCM;		/* Current UI Setting SPDIF Output is PCM. */

	HAL_AUDIO_SPDIF_COPYRIGHT_T	curSpdifCopyInfo;			/* SPDIF Output SCMS(Serail Copy Management System) => Copy Info. : FREE/ONCE/NEVER */
	UINT8						curSpdifCategoryCode;		/* Current Audio SPDIF Category Code */
	BOOLEAN						curSpdifLightOnOff;			/* Current Audio SPDIF Light Output ON/OFF */

	UINT32						curSPKOutDelay;
	UINT32						curSPDIFOutDelay;
	UINT32						curHPOutDelay;
	UINT32						curSCARTOutDelay;

	HAL_AUDIO_VOLUME_T			curSPKOutVolume;
	HAL_AUDIO_VOLUME_T			curSPDIFOutVolume;
	HAL_AUDIO_VOLUME_T			curHPOutVolume;
	HAL_AUDIO_VOLUME_T			curSCARTOutVolume;

	BOOLEAN						curSPKMuteStatus;
	BOOLEAN						curSPDIFMuteStatus;
	BOOLEAN						curHPMuteStatus;
	BOOLEAN						curSCARTMuteStatus;

	HAL_AUDIO_SB_SET_INFO_T 	curSoundBarInfo;
	HAL_AUDIO_SB_SET_CMD_T		curSoundBarCommand;
} HAL_AUDIO_COMMON_INFO_T;


/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
DTV_STATUS_T HAL_AUDIO_InitializeModule(HAL_AUDIO_SIF_TYPE_T eSifType);
DTV_STATUS_T HAL_AUDIO_FinalizeModule(void);
DTV_STATUS_T HAL_AUDIO_SetI2SOutNumber(UINT8 number);
DTV_STATUS_T HAL_AUDIO_SetSPKOutput(UINT8 i2sNumber, HAL_AUDIO_SAMPLING_FREQ_T samplingFreq);
DTV_STATUS_T HAL_AUDIO_RegWarningCallback(pfnADECWarning pfnCallBack);

/* Open, Close */
DTV_STATUS_T HAL_AUDIO_TP_Open(HAL_AUDIO_TP_INDEX_T tpIndex);
DTV_STATUS_T HAL_AUDIO_TP_Close(HAL_AUDIO_TP_INDEX_T tpIndex);
DTV_STATUS_T HAL_AUDIO_ADC_Open(UINT8 portNum);
DTV_STATUS_T HAL_AUDIO_ADC_Close(UINT8 portNum);
DTV_STATUS_T HAL_AUDIO_HDMI_Open(void);
DTV_STATUS_T HAL_AUDIO_HDMI_Close(void);
DTV_STATUS_T HAL_AUDIO_HDMI_OpenPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex);
DTV_STATUS_T HAL_AUDIO_HDMI_ClosePort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex);
DTV_STATUS_T HAL_AUDIO_AAD_Open(void);
DTV_STATUS_T HAL_AUDIO_AAD_Close(void);
DTV_STATUS_T HAL_AUDIO_SYSTEM_Open(void);
DTV_STATUS_T HAL_AUDIO_SYSTEM_Close(void);
DTV_STATUS_T HAL_AUDIO_ADEC_Open(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_ADEC_Close(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_AENC_Open(HAL_AUDIO_AENC_INDEX_T aencIndex);
DTV_STATUS_T HAL_AUDIO_AENC_Close(HAL_AUDIO_AENC_INDEX_T aencIndex);
DTV_STATUS_T HAL_AUDIO_SE_Open(void);
DTV_STATUS_T HAL_AUDIO_SE_Close(void);
DTV_STATUS_T HAL_AUDIO_SNDOUT_Open(HAL_AUDIO_SNDOUT_T soundOutType);
DTV_STATUS_T HAL_AUDIO_SNDOUT_Close(HAL_AUDIO_SNDOUT_T soundOutType);

/* Connect & Disconnect */
DTV_STATUS_T HAL_AUDIO_TP_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_TP_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_ADC_Connect(UINT8 portNum);
DTV_STATUS_T HAL_AUDIO_ADC_Disconnect(UINT8 portNum);
DTV_STATUS_T HAL_AUDIO_HDMI_Connect(void);
DTV_STATUS_T HAL_AUDIO_HDMI_Disconnect(void);
DTV_STATUS_T HAL_AUDIO_HDMI_ConnectPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex);
DTV_STATUS_T HAL_AUDIO_HDMI_DisconnectPort(HAL_AUDIO_HDMI_INDEX_T hdmiIndex);
DTV_STATUS_T HAL_AUDIO_AAD_Connect(void);
DTV_STATUS_T HAL_AUDIO_AAD_Disconnect(void);
DTV_STATUS_T HAL_AUDIO_SYSTEM_Connect(void);
DTV_STATUS_T HAL_AUDIO_SYSTEM_Disconnect(void);
DTV_STATUS_T HAL_AUDIO_ADEC_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_ADEC_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_AMIX_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_AMIX_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_AENC_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_AENC_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_SE_Connect(HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_SE_Disconnect(HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_SNDOUT_Connect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);
DTV_STATUS_T HAL_AUDIO_SNDOUT_Disconnect(HAL_AUDIO_RESOURCE_T currentConnect, HAL_AUDIO_RESOURCE_T inputConnect);

/* Start & Stop */
DTV_STATUS_T HAL_AUDIO_StartDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T audioType);
DTV_STATUS_T HAL_AUDIO_StopDecoding(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_SetMainDecoderOutput(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_SetSubDecoderOutput(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SNDOUT_T soundOutType, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_StartDecodingFor4KTP(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T audioType);
DTV_STATUS_T HAL_AUDIO_StopDecodingFor4KTP(HAL_AUDIO_ADEC_INDEX_T adecIndex);

/* HDMI */
DTV_STATUS_T HAL_AUDIO_HDMI_GetAudioMode(HAL_AUDIO_HDMI_TYPE_T *pHDMIMode);
DTV_STATUS_T HAL_AUDIO_HDMI_SetAudioReturnChannel(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_HDMI_GetCopyInfo(HAL_AUDIO_SPDIF_COPYRIGHT_T *pCopyInfo);

DTV_STATUS_T HAL_AUDIO_HDMI_GetPortAudioMode(HAL_AUDIO_HDMI_INDEX_T hdmiIndex, HAL_AUDIO_HDMI_TYPE_T *pHDMIMode);
DTV_STATUS_T HAL_AUDIO_HDMI_SetPortAudioReturnChannel(HAL_AUDIO_HDMI_INDEX_T hdmiIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_HDMI_GetPortCopyInfo(HAL_AUDIO_HDMI_INDEX_T hdmiIndex, HAL_AUDIO_SPDIF_COPYRIGHT_T *pCopyInfo);

/* Decoder */
DTV_STATUS_T HAL_AUDIO_SetSyncMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetDolbyDRCMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DOLBY_DRC_MODE_T drcMode);
DTV_STATUS_T HAL_AUDIO_SetDownMixMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DOWNMIX_MODE_T downmixMode);
DTV_STATUS_T HAL_AUDIO_GetDecodingType(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_SRC_TYPE_T *pAudioType);
DTV_STATUS_T HAL_AUDIO_GetAdecStatus(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_ADEC_INFO_T *pAudioAdecInfo);
DTV_STATUS_T HAL_AUDIO_SetDualMonoOutMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_DUALMONO_MODE_T outputMode);
DTV_STATUS_T HAL_AUDIO_TP_GetESInfo(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_ES_INFO_T *pAudioESInfo);
BOOLEAN HAL_AUDIO_TP_IsESExist(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_TP_GetAudioPTS(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 *pPts);
DTV_STATUS_T HAL_AUDIO_TP_SetAudioDescriptionMain(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetTrickMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_TRICK_MODE_T eTrickMode);
DTV_STATUS_T HAL_AUDIO_TP_SetAVSyncNoPCRMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bSetNoPCR, UINT32 offset);
DTV_STATUS_T HAL_AUDIO_TP_SetAVSyncVdecBasedMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff, UINT32 offset);
DTV_STATUS_T HAL_AUDIO_TP_SetAVSyncAdecBasedMode(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff, UINT32 offset);
DTV_STATUS_T HAL_AUDIO_TP_GetBufferStatus(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 *pMaxSize, UINT32 *pFreeSize);

/* Clip Play */
/* Clip Play for Decoder : 2 Decoder support. */
DTV_STATUS_T HAL_AUDIO_PlayClipDecoder(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_CLIP_DEC_PARAM_T clipInfo, UINT32 bufSize, void *pBufClip, pfnAdecoderClipDone pfnCallBack);
DTV_STATUS_T HAL_AUDIO_StopClipDecoder(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_PauseClipDecoder(HAL_AUDIO_ADEC_INDEX_T adecIndex);
DTV_STATUS_T HAL_AUDIO_ResumeClipDecoder(HAL_AUDIO_ADEC_INDEX_T adecIndex);

/* Clip Play for Mixer : 8 Mixer support. */
DTV_STATUS_T HAL_AUDIO_PlayClipMixer(HAL_AUDIO_MIXER_INDEX_T mixerIndex, HAL_AUDIO_CLIP_MIX_PARAM_T clipInfo, UINT32 bufSize, void *pBufClip, pfnAmixerClipDone pfnCallBack);
DTV_STATUS_T HAL_AUDIO_StopClipMixer(HAL_AUDIO_MIXER_INDEX_T mixerIndex);
DTV_STATUS_T HAL_AUDIO_PauseClipMixer(HAL_AUDIO_MIXER_INDEX_T mixerIndex);
DTV_STATUS_T HAL_AUDIO_ResumeClipMixer(HAL_AUDIO_MIXER_INDEX_T mixerIndex);

/* Volume, Mute & Delay */
DTV_STATUS_T HAL_AUDIO_SetDecoderInputGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_SetDecoderChannelGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T leftVolume, HAL_AUDIO_VOLUME_T rightVolume);
DTV_STATUS_T HAL_AUDIO_SetDecoderOutputGain(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_SetDecoderDelayTime(HAL_AUDIO_ADEC_INDEX_T adecIndex, UINT32 delayTime);
DTV_STATUS_T HAL_AUDIO_SetMixerInputGain(HAL_AUDIO_MIXER_INDEX_T mixerIndex, HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_SetMixerOutputGain(HAL_AUDIO_MIXER_INDEX_T mixerIndex, HAL_AUDIO_VOLUME_T volume);

DTV_STATUS_T HAL_AUDIO_SetSPKOutVolume(HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_SetSPDIFOutVolume(HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_SetHPOutVolume(HAL_AUDIO_VOLUME_T volume, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SetSCARTOutVolume(HAL_AUDIO_VOLUME_T volume, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SetAudioDescriptionVolume(HAL_AUDIO_ADEC_INDEX_T adecIndex, HAL_AUDIO_VOLUME_T volume);

DTV_STATUS_T HAL_AUDIO_SetDecoderInputMute(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetDecoderInputESMute(HAL_AUDIO_ADEC_INDEX_T adecIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetMixerInputMute(HAL_AUDIO_MIXER_INDEX_T mixerIndex, BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetSPKOutMute(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetSPDIFOutMute(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetHPOutMute(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SetSCARTOutMute(BOOLEAN bOnOff);

DTV_STATUS_T HAL_AUDIO_GetSPKOutMuteStatus(BOOLEAN *pOnOff);
DTV_STATUS_T HAL_AUDIO_GetSPDIFOutMuteStatus(BOOLEAN *pOnOff);
DTV_STATUS_T HAL_AUDIO_GetHPOutMuteStatus(BOOLEAN *pOnOff);
DTV_STATUS_T HAL_AUDIO_GetSCARTOutMuteStatus(BOOLEAN *pOnOff);

DTV_STATUS_T HAL_AUDIO_SetSPKOutDelayTime(UINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SetSPDIFOutDelayTime(UINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SetHPOutDelayTime(UINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SetSCARTOutDelayTime(UINT32 delayTime, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_GetDelayTime(HAL_AUDIO_RESOURCE_T resource, UINT32 *pDelayTime);
DTV_STATUS_T HAL_AUDIO_GetStatusInfo(HAL_AUDIO_COMMON_INFO_T *pAudioStatusInfo);

/* SPDIF(Sound Bar) */
DTV_STATUS_T HAL_AUDIO_SPDIF_SetOutputType(HAL_AUDIO_SPDIF_MODE_T eSPDIFMode, BOOLEAN bForced);
DTV_STATUS_T HAL_AUDIO_SPDIF_SetCopyInfo(HAL_AUDIO_SPDIF_COPYRIGHT_T copyInfo);
DTV_STATUS_T HAL_AUDIO_SPDIF_SetCategoryCode(UINT8 categoryCode);
DTV_STATUS_T HAL_AUDIO_SPDIF_SetLightOnOff(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SB_SetOpticalIDData(HAL_AUDIO_SB_SET_INFO_T info);
DTV_STATUS_T HAL_AUDIO_SB_GetOpticalStatus(HAL_AUDIO_SB_GET_INFO_T *pInfo);
DTV_STATUS_T HAL_AUDIO_SB_SetCommand(HAL_AUDIO_SB_SET_CMD_T info);
DTV_STATUS_T HAL_AUDIO_SB_GetCommandStatus(HAL_AUDIO_SB_GET_CMD_T *pInfo);

/* SE */
DTV_STATUS_T HAL_AUDIO_LGSE_SetMode(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetMain(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN000(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN001(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN003(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
//DTV_STATUS_T HAL_AUDIO_LGSE_SetFN004(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption,		\
//									 HAL_AUDIO_LGSE_VARIABLE_MODE_T varOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN004Mode1(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption, 	\
										  HAL_AUDIO_LGSE_VARIABLE_MODE_T varOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN004Mode2(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN004Mode3(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN004(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption, HAL_AUDIO_LGSE_VARIABLE_MODE_T varOption);

DTV_STATUS_T HAL_AUDIO_LGSE_SetFN005(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN006(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN007(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN008(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN009(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN010(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_ACCESS_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN011(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN012(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN013(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN014(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN016(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN017(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN019(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN020(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN022(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN023(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_SetFN024(UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_GetData(HAL_AUDIO_LGSE_FUNCLIST_T funcList, HAL_AUDIO_LGSE_DATA_ACCESS_T rw, 			\
									UINT32 *pParams, UINT16 noParam, HAL_AUDIO_LGSE_DATA_MODE_T dataOption);
DTV_STATUS_T HAL_AUDIO_LGSE_RegSmartSoundCallback(pfnLGSESmartSound pfnCallBack, UINT32 callbackPeriod);
DTV_STATUS_T HAL_AUDIO_LGSE_GetParamData(HAL_AUDIO_LGSE_FUNCLIST_T funcList, HAL_AUDIO_LGSE_DATA_MODE_T dataOption,	\
										 UINT16 noParam, UINT32 *pParams);

/* AAD */
DTV_STATUS_T HAL_AUDIO_SIF_SetInputSource(HAL_AUDIO_SIF_INPUT_T sifSource);
DTV_STATUS_T HAL_AUDIO_SIF_SetHighDevMode(BOOLEAN bOnOff);
DTV_STATUS_T HAL_AUDIO_SIF_SetBandSetup(HAL_AUDIO_SIF_TYPE_T eSifType, HAL_AUDIO_SIF_SOUNDSYSTEM_T sifBand);
DTV_STATUS_T HAL_AUDIO_SIF_SetModeSetup(HAL_AUDIO_SIF_STANDARD_T sifStandard);
DTV_STATUS_T HAL_AUDIO_SIF_SetUserAnalogMode(HAL_AUDIO_SIF_MODE_SET_T sifAudioMode);
DTV_STATUS_T HAL_AUDIO_SIF_SetA2ThresholdLevel(UINT16 thrLevel);
DTV_STATUS_T HAL_AUDIO_SIF_SetNicamThresholdLevel(UINT16 thrLevel);
DTV_STATUS_T HAL_AUDIO_SIF_GetBandDetect(HAL_AUDIO_SIF_SOUNDSYSTEM_T soundSystem, UINT32 *pBandStrength);
DTV_STATUS_T HAL_AUDIO_SIF_DetectSoundSystem(HAL_AUDIO_SIF_SOUNDSYSTEM_T setSoundSystem, BOOLEAN bManualMode, HAL_AUDIO_SIF_SOUNDSYSTEM_T *pDetectSoundSystem, UINT32 *pSignalQuality);
DTV_STATUS_T HAL_AUDIO_SIF_CheckNicamDigital(HAL_AUDIO_SIF_EXISTENCE_INFO_T *pIsNicamDetect);
DTV_STATUS_T HAL_AUDIO_SIF_CheckAvailableSystem(HAL_AUDIO_SIF_AVAILE_STANDARD_T standard, HAL_AUDIO_SIF_EXISTENCE_INFO_T *pAvailability);
DTV_STATUS_T HAL_AUDIO_SIF_CheckA2DK(HAL_AUDIO_SIF_STANDARD_T standard, HAL_AUDIO_SIF_EXISTENCE_INFO_T *pAvailability);
DTV_STATUS_T HAL_AUDIO_SIF_GetA2StereoLevel(UINT16 *pLevel);
DTV_STATUS_T HAL_AUDIO_SIF_GetNicamThresholdLevel(UINT16 *pLevel);
DTV_STATUS_T HAL_AUDIO_SIF_GetCurAnalogMode(HAL_AUDIO_SIF_MODE_GET_T *pSifAudioMode);
BOOLEAN HAL_AUDIO_SIF_IsSIFExist(void);
DTV_STATUS_T HAL_AUDIO_SIF_SetAudioEQMode(BOOLEAN bOnOff);

/* AENC */
DTV_STATUS_T HAL_AUDIO_AENC_Initialize(void);
DTV_STATUS_T HAL_AUDIO_AENC_Destroy(void);
DTV_STATUS_T HAL_AUDIO_AENC_Start(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_ENCODING_FORMAT_T audioType);
DTV_STATUS_T HAL_AUDIO_AENC_Stop(HAL_AUDIO_AENC_INDEX_T aencIndex);
DTV_STATUS_T HAL_AUDIO_AENC_RegCallback(HAL_AUDIO_AENC_INDEX_T aencIndex, pfnAENCDataHandling pfnCallBack);
DTV_STATUS_T HAL_AUDIO_AENC_SetInfo(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_INFO_T info);
DTV_STATUS_T HAL_AUDIO_AENC_GetInfo(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_AENC_INFO_T *pInfo);
DTV_STATUS_T HAL_AUDIO_AENC_SetVolume(HAL_AUDIO_AENC_INDEX_T aencIndex, HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_AENC_CopyData(HAL_AUDIO_AENC_INDEX_T aencIndex, UINT8 *pDest, UINT8 *pBufAddr, UINT32 datasize, UINT8 *pRStart, UINT8 *pREnd);
DTV_STATUS_T HAL_AUDIO_AENC_ReleaseData(HAL_AUDIO_AENC_INDEX_T aencIndex, UINT8 *pBufAddr, UINT32 datasize);

/* PCM(Sound Bar Buletooth, PCM Capture) */
DTV_STATUS_T HAL_AUDIO_PCM_StartUpload(HAL_AUDIO_PCM_INPUT_T apcmIndex);
DTV_STATUS_T HAL_AUDIO_PCM_StopUpload(HAL_AUDIO_PCM_INPUT_T apcmIndex);
DTV_STATUS_T HAL_AUDIO_PCM_RegSendPCMCallback(HAL_AUDIO_PCM_INPUT_T apcmIndex, pfnPCMSending pfnCallBack);
DTV_STATUS_T HAL_AUDIO_PCM_RegUploadCallBack(HAL_AUDIO_PCM_INPUT_T apcmIndex, pfnPCMDataHandling pfnCallBack);
DTV_STATUS_T HAL_AUDIO_PCM_SetVolume(HAL_AUDIO_PCM_INPUT_T apcmIndex, HAL_AUDIO_VOLUME_T volume);
DTV_STATUS_T HAL_AUDIO_PCM_SetDataCount(HAL_AUDIO_PCM_INPUT_T apcmIndex, UINT8 count);	//for bluetooth, 1 count takes 5.3ms.
DTV_STATUS_T HAL_AUDIO_PCM_SetInfo(HAL_AUDIO_PCM_INPUT_T apcmIndex, HAL_AUDIO_PCM_INFO_T info);
DTV_STATUS_T HAL_AUDIO_PCM_GetInfo(HAL_AUDIO_PCM_INPUT_T apcmIndex, HAL_AUDIO_PCM_INFO_T *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_AUDIO_H_ */


