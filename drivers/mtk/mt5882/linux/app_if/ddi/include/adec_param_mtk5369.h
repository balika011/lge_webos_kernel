/******************************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 *****************************************************************************/

/** @file adec_param_saturn8.h
 *
 *	ADEC DD 함수 header 파일.
 *
 *	@author		Yongwoon Jung(ywoonjung@lge.com)
 *	@version	0.1
 *	@date		2008.05.30
 *	@note
 *	@see
 */

/******************************************************************************
	Header File Guarder
******************************************************************************/
#ifndef _ADEC_PARAM_H_
#define _ADEC_PARAM_H_

/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
#include "audio_index_table.h"

/******************************************************************************
	상수 정의(Constant Definitions)
******************************************************************************/
#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x)	(sizeof(x)/sizeof(x[0]))
#endif


/*
 * Adec Volume Curve Structure
 *
 *
 */
typedef struct AUDIO_ADEC_VOL_CURVE
{
	//char		adecVolCurveName[20];
	UINT16		adecVolumeCurve[101];
} AUDIO_ADEC_VOL_CURVE_T;


typedef struct ADEC_I2S_VOLUME
{
	UINT8		adecI2SVolType;
	SINT8		adecI2SVolume[AUD_GAIN_INDEX_MAX];
} ADEC_I2S_VOLUME_T;


typedef struct ADEC_SPDIF_VOLUME
{
	UINT8		adecSpdifVolType;
	SINT8		adecSpdifVolume[AUD_GAIN_INDEX_MAX];
} ADEC_SPDIF_VOLUME_T;

typedef struct ADEC_HP_VOLUME
{
	UINT8		adecHpVolType;
	SINT8		adecHpVolume[101];
} ADEC_HP_VOLUME_T;

//typedef struct ADEC_SCART_VOLUME
//{
//	SINT8		adecScartVolVolume[AUD_GAIN_INDEX_MAX];
//} ADEC_SCART_VOLUME_T;


const ADEC_I2S_VOLUME_T gI2S_VOL[] =
{
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
	{
		/*	0,	SPK_GAIN_DEFAULT,	Default Curve - Default & Test Temp  */
		SPK_GAIN_DEFAULT,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	1,	SPK_GAIN_EU, 	성장원Y, 111010 */
		SPK_GAIN_EU,
		{
			-12,		0,			0,
			-2,			-12,		-12,		-12,		-10,		-2,			-5,		-5,
			0,			0,			0,			0,			0,			0,
			-2,	 		-2,			-8,			-2,			0,
			-15,		-2,			0,			0, 			0,			0, 			0
		}
		,
	},
	{
		/*	2,	SPK_GAIN_AU, 	Kim Minhan J, 091211  */
		SPK_GAIN_AU,
		{
			0,			0,			0,
			0,		   -9, 		    0,		   -8,			0,			0,			0,			0,
			0,			0,			2,			0,			0,			0,
			4, 		    4,		   -1,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},

	},

	{
		/*	3,	SPK_GAIN_NT_LD45, Kim Sehwan Y, 100106 last requested by YW, Yoon for DTV  */
		SPK_GAIN_NT,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			-2,			-2,			-5,			-5,			0,			0,
			-1, 		-1,			-4,			0,			0,
			-18,		-2,			0,			0, 			0,			0, 			0
		},
	},

	{
		/*	4,	SPK_GAIN_CN, 	42LD45, Guojing & NamJaeHo, 091209	*/
		SPK_GAIN_CN,
		{
			-12,		0,			0,
			-2,			-12,		-12,		-12,		-10,		-2,			-5,		-5,
			-11,		0,			0,			0,			0,			0,
			-2,	 		-2,			-8,			6,			0,
			-27,		-14,		0,			0, 			0,			0, 			0
		},
	},

	{
       	 /* 5, SPK_GAIN_BR, 32LD350, Choe Hye Jeong, 091124 */
        	SPK_GAIN_BR,
		 {
			 0, 		 	0, 		 	0,
			 0, 		 	0, 		 	0, 			 0, 		 	0, 		0, 		 0, 		 0,
			 0, 		 	0, 			 -8,		 -8,			 	-8, 		 0,
			 -3,		 	-3,		 -13, 		 	0, 		 	0,
			 0, 		 0, 		 0, 		 0, 		 	0, 		 	0, 		0
		 },



	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	*		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	*		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	*		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	*		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,	 		Flash	*/
	{
		/*	6,	SPK_GAIN_TW, 	37LD450, Lee Jun Gu, 091123  */
		SPK_GAIN_TW,
		{
			1,			-1,			-1,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			-4,			-4,			-4,			0,
			0, 			0,			-10,			-1,			-1,
			-17,		-1,			-1,			-1, 			-1,			-1, 			-1
		},
	},
	{
		/* 7, SPK_GAIN_AN_NT, Lee Jun Gu, 100116 */
		SPK_GAIN_AN_NT,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},

	{
		/*	8,	SPK_GAIN_AN_PAL, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_AN_PAL,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	9,	SPK_GAIN_EU_PDP, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_EU_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	10,	SPK_GAIN_AU_PDP, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_AU_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	*		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	*		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	*		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	*		EMF_Music,	EMF_Movie,	Blutooth,		MediaShare,	PicWizard,	BB,	 		Flash	*/
	{
		/*	11,	SPK_GAIN_NT_PDP, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_NT_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	12,	SPK_GAIN_BR_PDP, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_BR_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	13,	SPK_GAIN_TW_PDP, 	Default Curve - Default & Test Temp  */
		SPK_GAIN_TW_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
};

const ADEC_SPDIF_VOLUME_T gSPDIF_VOL[] =
{
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
	{
		/*	0,	SPDIF_GAIN_DEFAULT,		Default Curve - Default & Test Temp  */
		SPDIF_GAIN_DEFAULT,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	1,	SPDIF_GAIN_EU,	LD45/LE53, Goo HyungSun J, 091010	*/
		SPDIF_GAIN_EU,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	2,	SPDIF_GAIN_AU,	Default Curve - Default & Test Temp  */
		SPDIF_GAIN_AU,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	3,	SPDIF_GAIN_NT,	LD45/LE53, Hyoyung, Kim J, 091118  */
		/*						change USB gain for bypassing, Sehwan Kim Y 091124, do not change plz, DQA confirmed */
		SPDIF_GAIN_NT,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	4,	SPDIF_GAIN_EU,   LD45, Nam Jae Ho , 091208   */
		SPDIF_GAIN_CN,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	5,	SPDIF_GAIN_BR,	Default Curve - Default & Test Temp  */
		SPDIF_GAIN_BR,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV, 		Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare, PicWizard,	BB, 		Flash	*/
	{
		/*	6,	SPDIF_GAIN_TW,	MSTAR Model, Lee Jun Gu, 091114 */
		SPDIF_GAIN_TW,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			-4,			-4,			-4,			0,
			1,			1,			1,			0,			0,
			0,			0,			0,			0,			0,			0,			0
		},

	},
	{
		/*	7,	SPDIF_GAIN_AN_NT,	Lee Jun Gu, 091114 */
		SPDIF_GAIN_AN_NT,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},

	},
	{
		/*	8,	SPDIF_GAIN_AN_PAL,	Default Curve - Default & Test Temp  */
		SPDIF_GAIN_AN_PAL,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},

	},
	{
		/*	9,	SPDIF_GAIN_EU_PDP	*/
		SPDIF_GAIN_EU_PDP,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	10, SPDIF_GAIN_AU_PDP	*/
		SPDIF_GAIN_AU_PDP,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	11, SPDIF_GAIN_NT_PDP	*/
		SPDIF_GAIN_NT_PDP,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	12, SPDIF_GAIN_BR_PDP	*/
		SPDIF_GAIN_BR_PDP,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},
	{
		/*	13, SPDIF_GAIN_TW_PDP	*/
		SPDIF_GAIN_TW_PDP,
		{
			0,			0,			0,
			0,			0,			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0,			0,			0,			8,			0,
			0,			0,			0,			0,			0,			0,			0
		},
	},



	/*		Default,	DTV_M,		DTV_BI,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV, 		Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare, PicWizard,	BB, 		Flash	*/
};


const ADEC_HP_VOLUME_T gHP_VOL[] =
{
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
	{
		/*	0,	SPK_GAIN_DEFAULT,	Default Curve - Default & Test Temp  */
		HP_GAIN_DEFAULT,
		{
			-30,			-30,			-30,
			-30,			-30, 			-30,			-30,			-30,			-30,			0,			0,
			-30,			-30,			-30,			-30,			-30,			-30,
			-30, 			-30,			-30,			-30,			-30,
			-30,			-30,			-30,			-30, 			-30,			-30, 			0
		},
	},
	{
		/*	1,	SPK_GAIN_EU, 	성장원Y, 111010 */
		HP_GAIN_EU,
		{
			-30,			-30,			-30,
			-25,			-39, 			-39,			-39,			-25,			-25,			-33,			-33,
			-30,			-30,			-30,			-30,			-30,			-30,
			-29, 			-29,			-34,			-21,			-30,
			-6,				7,				-30,			-30, 			-30,			-30, 			0
		}
		,
	},
	{
		/*	2,	SPK_GAIN_AU, 	Kim Minhan J, 091211  */
		HP_GAIN_AU,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},

	},

	{
		/*	3,	SPK_GAIN_NT_LD45, Kim Sehwan Y, 100106 last requested by YW, Yoon for DTV  */
		HP_GAIN_NT,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},

	{
		/*	4,	SPK_GAIN_CN, 	42LD45, Guojing & NamJaeHo, 091209	*/
		HP_GAIN_CN,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},

	{
       	 /* 5, SPK_GAIN_BR, 32LD350, Choe Hye Jeong, 091124 */
        	HP_GAIN_BR,
		 {
			 0, 		 0, 		 0,
			 0, 		 0, 		 0, 		 0, 		 0, 		 0, 		 0, 		 0,
			 0, 		 0, 		 0, 		 0, 		 0, 		 0,
			 0, 		 0, 		 0, 		 0, 		 0,
			 0, 		 0, 		 0, 		 0, 		 0, 		 0, 		 0
		 },



	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	*		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	*		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	*		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	*		EMF_Music,	EMF_Movie,	Blutooth,		MediaShare,	PicWizard,	BB,	 		Flash	*/
	{
		/*	6,	SPK_GAIN_TW, 	37LD450, Lee Jun Gu, 091123  */
		HP_GAIN_TW,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/* 7, SPK_GAIN_AN_NT, Lee Jun Gu, 100116 */
		HP_GAIN_AN_NT,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},

	{
		/*	8,	SPK_GAIN_AN_PAL, 	Default Curve - Default & Test Temp  */
		HP_GAIN_AN_PAL,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	9,	SPK_GAIN_EU_PDP, 	Default Curve - Default & Test Temp  */
		HP_GAIN_EU_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	10,	SPK_GAIN_AU_PDP, 	Default Curve - Default & Test Temp  */
		HP_GAIN_AU_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	*		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	*		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	*		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	*		EMF_Music,	EMF_Movie,	Blutooth,		MediaShare,	PicWizard,	BB,	 		Flash	*/
	{
		/*	11,	SPK_GAIN_NT_PDP, 	Default Curve - Default & Test Temp  */
		HP_GAIN_NT_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	{
		/*	12,	SPK_GAIN_BR_PDP, 	Default Curve - Default & Test Temp  */
		HP_GAIN_BR_PDP,
		{
			0,			0,			0,
			0,			0, 			0,			0,			0,			0,			0,			0,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
		},
	},
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
};

const SINT8 gSCART_VOL[AUD_GAIN_INDEX_MAX] =
{
	/*		Default,	DTV_MPEG,	DTV_Bitstream,
	 *		BG_NICAM,	BG_Mono,	BG_Stereo,	PAL_I_DK,	I_NICAM,	DK_NICAM,	L_NICAM,	SECAM_L_AM,
	 *		NTSC_A2,	NT_A2_SAP,	BTSC_Mono,	BT_Stereo,	BTSC_Sap,	EIAJ,
	 *		AV,			Component,	RGB_DVI,	HDMI,		PLAY_HDD,
	 *		EMF_Music,	EMF_Movie,	Blutooth,	MediaShare,	PicWizard,	BB,			Flash	*/
			0,			0,			0,
			5,			5, 			5,			5,			5,			5,			10,			10,
			0,			0,			0,			0,			0,			0,
			0, 			0,			0,			0,			0,
			0,			0,			0,			0, 			0,			0, 			0
};


/******************************************************************************
	매크로 함수 정의 (Macro Definitions)
******************************************************************************/



/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
#define HIBYTE(value)				((U8)((value) / 0x100))
#define LOBYTE(value)				((U8)(value))

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/

#endif /* _ADEC_PARAM_H_ */

