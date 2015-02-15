/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file audio_index_table.h
 *
 *  audio index table header file.
 *  Volume Curve Index, Delay Type Index, Amp EQ Index, Amp Control Index
 *  I2S Gain Index, SPDIF Gain Index, Audio Delay Type Index
 *  This File will be updated by project.
 *
 *  @author		Yongchol Kee(yongchol.kee@lge.com)
 *  @version	1.0
 *  @date		2010.08.25
 *  @note
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _AUDIO_INDEX_TABLE_H_
#define _AUDIO_INDEX_TABLE_H_


/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/
//#include "common.h"
//#include "message.h"
//#include "osa_api.h"
//#include "global_configurations.h"
//#include "adec_drv.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/


/****************************************************
* Volume Table in dB, Mapping as linear attenuation *
****************************************************/

/**
 * Audio Delay Type
 *
 */
typedef  enum  AUD_DELAY_TYPE
{
	AUD_DELAY_DEFAULT	= 0,
	AUD_DELAY_USE_HW_OPTION,

	/* LCD MODEL */
	AUD_DELAY_NO_FRC,

	AUD_DELAY_FRC_URSA2,

	AUD_DELAY_FRC_URSA3_INTERNAL,
	AUD_DELAY_FRC_URSA3_EXTERNAL,

	AUD_DELAY_FRC_URSA4_EXTERNAL,
	AUD_DELAY_FRC_URSA4_INTERNAL,

	AUD_DELAY_FRC_URSA5_EXTERNAL,
	AUD_DELAY_FRC_URSA5_INTERNAL,

	AUD_DELAY_PWIZ,
	AUD_DELAY_PWIZ2,
	AUD_DELAY_PWIZ3,
	AUD_DELAY_PWIZ3_CSR505,

	AUD_DELAY_BCM_INTERNAL,
	AUD_DELAY_BCM_INTERNAL_MIRRORMODE,

	AUD_DELAY_MTK_INTERNAL,

	AUD_DELAY_LG1150,

	/* PDP Model */
	AUD_DELAY_PDP,

	AUD_DELAY_END
}  AUD_DELAY_TYPE_T;

/**
 * Parametric EQ Type index Definition
 *
 */
typedef enum AUDIO_AMP_PARAMETRIC_EQ_TYPE
{
	AMP_EQ_DEFAULT 			= 0,

	/* LCD MODEL */
	AMP_EQ_LCD_DEFAULT,
	AMP_EQ_LS57,

	AMP_EQ_LCD_END,

	/* PDP Model */
	AMP_EQ_PDP_DEFAULT = AMP_EQ_LCD_END,
	AMP_EQ_PDP_END,

	AMP_EQ_END = AMP_EQ_PDP_END

} AUDIO_AMP_PARAMETRIC_EQ_TYPE_T;

/**
 * Audio AMP Control Type
 *
 */
typedef enum AUD_AMP_CTL_TYPE
{
	AMP_CTL_DEFAULT = 0,

	AMP_CTL_USE_TOOL_OPTION,

	/* 10W 24V */
	AMP_CTL_10W_24V_EU,
	AMP_CTL_10W_24V_AU,
	AMP_CTL_10W_24V_NTSC,
	AMP_CTL_10W_24V_CN,
	AMP_CTL_10W_24V_TW,
	AMP_CTL_10W_24V_BR,
	AMP_CTL_10W_24V_AN_PAL,
	AMP_CTL_10W_24V_AN_NT,

	/* 10W 20V */
	AMP_CTL_10W_20V_EU,
	AMP_CTL_10W_20V_AU,
	AMP_CTL_10W_20V_NTSC,
	AMP_CTL_10W_20V_CN,
	AMP_CTL_10W_20V_TW,
	AMP_CTL_10W_20V_BR,
	AMP_CTL_10W_20V_AN_PAL,
	AMP_CTL_10W_20V_AN_NT,

	/* 10W 17V */
	AMP_CTL_10W_17V_EU,
	AMP_CTL_10W_17V_AU,
	AMP_CTL_10W_17V_NTSC,
	AMP_CTL_10W_17V_CN,
	AMP_CTL_10W_17V_TW,
	AMP_CTL_10W_17V_BR,
	AMP_CTL_10W_17V_AN_PAL,
	AMP_CTL_10W_17V_AN_NT,

	/* 7W 24V */
	AMP_CTL_7W_24V_EU,
	AMP_CTL_7W_24V_AU,
	AMP_CTL_7W_24V_NTSC,
	AMP_CTL_7W_24V_CN,
	AMP_CTL_7W_24V_TW,
	AMP_CTL_7W_24V_BR,
	AMP_CTL_7W_24V_AN_PAL,
	AMP_CTL_7W_24V_AN_NT,

	/* 5W 24V */
	AMP_CTL_5W_24V_EU,
	AMP_CTL_5W_24V_AU,
	AMP_CTL_5W_24V_NTSC,
	AMP_CTL_5W_24V_CN,
	AMP_CTL_5W_24V_TW,
	AMP_CTL_5W_24V_BR,
	AMP_CTL_5W_24V_AN_PAL,
	AMP_CTL_5W_24V_AN_NT,

	/* 5W 15V */
	AMP_CTL_5W_15V_EU,
	AMP_CTL_5W_15V_TW,
	AMP_CTL_5W_15V_AU,

	/* 15W 20V */
	AMP_CTL_15W_20V_AU,
	AMP_CTL_15W_20V_AN_NT,
	AMP_CTL_15W_20V_TW,
	AMP_CTL_15W_20V_BR,

	/* 10W 25V */
	AMP_CTL_10W_25V_TW,

	AMP_CTL_END
} AUD_AMP_CTL_TYPE_T;

/**
 * Audio SPK Volume Curve Type
 *
 */
typedef enum AUDIO_VOL_CRV_TYPE
{
	VOL_CRV_DEFAULT	= 0,
	VOL_CRV_USE_TOOL_OPTION,

	/* 10W 24V */
	VOL_CRV_10W_24V_EU,
	VOL_CRV_10W_24V_AU,
	VOL_CRV_10W_24V_NT,
	VOL_CRV_10W_24V_CN,
	VOL_CRV_10W_24V_BR,
	VOL_CRV_10W_24V_TW,
	VOL_CRV_10W_24V_AN_NT,
	VOL_CRV_10W_24V_AN_PAL,

	/* 10W 20V */
	VOL_CRV_10W_20V_EU,
	VOL_CRV_10W_20V_AU,
	VOL_CRV_10W_20V_NT,
	VOL_CRV_10W_20V_CN,
	VOL_CRV_10W_20V_BR,
	VOL_CRV_10W_20V_TW,
	VOL_CRV_10W_20V_AN_NT,
	VOL_CRV_10W_20V_AN_PAL,

	/* 10W 17V */
	VOL_CRV_10W_17V_EU,
	VOL_CRV_10W_17V_AU,
	VOL_CRV_10W_17V_NT,
	VOL_CRV_10W_17V_CN,
	VOL_CRV_10W_17V_BR,
	VOL_CRV_10W_17V_TW,
	VOL_CRV_10W_17V_AN_NT,
	VOL_CRV_10W_17V_AN_PAL,

	/* 7W 24V */
	VOL_CRV_7W_24V_EU,
	VOL_CRV_7W_24V_AU,
	VOL_CRV_7W_24V_NT,
	VOL_CRV_7W_24V_CN,
	VOL_CRV_7W_24V_BR,
	VOL_CRV_7W_24V_TW,
	VOL_CRV_7W_24V_AN_NT,
	VOL_CRV_7W_24V_AN_PAL,

	/* 5W 24V */
	VOL_CRV_5W_24V_EU,
	VOL_CRV_5W_24V_AU,
	VOL_CRV_5W_24V_NT,
	VOL_CRV_5W_24V_CN,
	VOL_CRV_5W_24V_BR,
	VOL_CRV_5W_24V_TW,
	VOL_CRV_5W_24V_AN_NT,
	VOL_CRV_5W_24V_AN_PAL,

	/* 15W 20V */
	VOL_CRV_15W_20V_AU,
	VOL_CRV_15W_20V_BR,
	VOL_CRV_15W_20V_TW,



	/* 5W 15V */
    VOL_CRV_5W_15V_AN_PAL,
	VOL_CRV_5W_15V_TW,

	VOL_CRV_END
} AUDIO_VOL_CRV_TYPE_T;

/**
 * Audio SPK(I2S) Out Gain Type
 *
 */
typedef  enum  AUDIO_SPK_GAIN_TYPE
{
	SPK_GAIN_DEFAULT	= 0,

	/* LCD Model */
	SPK_GAIN_EU,
	SPK_GAIN_AU,
	SPK_GAIN_NT,
	SPK_GAIN_CN,
	SPK_GAIN_BR,
	SPK_GAIN_TW,
	SPK_GAIN_AN_NT,
	SPK_GAIN_AN_PAL,

	/* PDP Model */
	SPK_GAIN_EU_PDP,
	SPK_GAIN_AU_PDP,
	SPK_GAIN_NT_PDP,
	SPK_GAIN_BR_PDP,
	SPK_GAIN_TW_PDP,

	/* If you add new index type,
	please add to here. */
	SPK_GAIN_END

}  AUDIO_SPK_GAIN_TYPE_T ;

/**
 * Audio SPDIF Volume Type
 *
 */
typedef  enum  AUDIO_SPDIF_GAIN_TYPE
{
	SPDIF_GAIN_DEFAULT	= 0,
	SPDIF_GAIN_EU,
	SPDIF_GAIN_AU,
	SPDIF_GAIN_NT,
	SPDIF_GAIN_CN,
	SPDIF_GAIN_BR,
	SPDIF_GAIN_TW,
	SPDIF_GAIN_AN_NT,
	SPDIF_GAIN_AN_PAL,

	SPDIF_GAIN_EU_PDP,
	SPDIF_GAIN_AU_PDP,
	SPDIF_GAIN_NT_PDP,
	SPDIF_GAIN_BR_PDP,
	SPDIF_GAIN_TW_PDP,

	/* If you add new index type,
	please add to here. */
	SPDIF_GAIN_END

}  AUDIO_SPDIF_GAIN_TYPE_T;

/**
 * Audio SCART Gain Type
 *
 */
typedef  enum  AUDIO_SCART_GAIN_TYPE
{
	SCART_GAIN_DEFAULT	= 0,
	SCART_GAIN_EU_LCD =	SCART_GAIN_DEFAULT,

	SCART_GAIN_EU_PDP,

	/* If you add new index type,
	please add to here. */
	SCART_GAIN_END

}  AUDIO_SCART_GAIN_TYPE_T ;

/**
 * Audio HP Volume Type
 *
 */
typedef  enum  AUDIO_HP_GAIN_TYPE
{
	HP_GAIN_DEFAULT	= 0,
	HP_GAIN_EU,
	HP_GAIN_AU,
	HP_GAIN_NT,
	HP_GAIN_CN,
	HP_GAIN_BR,
	HP_GAIN_TW,
	HP_GAIN_AN_NT,
	HP_GAIN_AN_PAL,

	/* PDP Model */
	HP_GAIN_EU_PDP,
	HP_GAIN_AU_PDP,
	HP_GAIN_NT_PDP,
	HP_GAIN_BR_PDP,
	HP_GAIN_TW_PDP,

	/* If you add new index type,
	please add to here. */
	HP_GAIN_END

}  AUDIO_HP_GAIN_TYPE_T ;


/**
 * Audio HP Volume Type
 *
 */
typedef  enum  AUDIO_HP_VOL_CRV_TYPE
{
	HP_VOL_CRV_DEFAULT	= 0,
	HP_VOL_CRV_EU,
	HP_VOL_CRV_AU,
	HP_VOL_CRV_NT,
	HP_VOL_CRV_CN,
	HP_VOL_CRV_BR,
	HP_VOL_CRV_TW,
	HP_VOL_CRV_AN_NT,
	HP_VOL_CRV_AN_PAL,

	HP_VOL_CRV_EU_PDP,
	HP_VOL_CRV_AU_PDP,
	HP_VOL_CRV_NT_PDP,

	/* If you add new index type,
	please add to here. */
	HP_VOL_CRV_END

}  AUDIO_HP_VOL_CRV_TYPE_T ;



/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/


/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/

#endif /* _AUDIO_INDEX_TABLE_H_ */

