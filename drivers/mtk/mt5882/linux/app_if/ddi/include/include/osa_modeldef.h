/******************************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 *****************************************************************************/

/** @file OSA_ModelDef.h
 *
 *	Model define option  functions.
 *
 *	@author 	lkh210
 *	@author 	next93
 *	@version	1.0
 *	@date		2006. 9. 21
 *	@note
 *	@see
 */
#ifndef _OSA_MODELDEF_H_
#define _OSA_MODELDEF_H_

/*-------------------------------------------------------------
 * include files
 *------------------------------------------------------------*/
#ifdef USE_BOOT
#include "typedef.h"
#endif

#include "branches.h"

#ifdef __cplusplus
extern "C" {
#endif
/*-------------------------------------------------------------
 * constant definitions
 *------------------------------------------------------------*/

// daesuk.park -100402
// �ѱ��� M ���� ��� Master Release �� �� Event �� Release �ؼ�
// �Ʒ� Define ���� 3, 4, 5 �� �����ϰ� Booting �� debug status �� �Ҿ setting �ϴ� �κп�
// ������ 3, 4, 5 �� �ƴ� ���̸� ������ RELEASE_LEVEL �� �ٲٵ��� ������
#if (PLATFORM_TYPE == BCM_PLATFORM)
#define DEBUG_LEVEL		3 // 0
#define EVENT_LEVEL		4 // 1
#define RELEASE_LEVEL	5 // 2
#else // MStar
//[L8] 2010/12/20 minjun.kim BCM �� ������ ������ ������
#if (PLATFORM_TYPE == LG_PLATFORM)
#define DEBUG_LEVEL		3 // 0
#define EVENT_LEVEL		4 // 1
#define RELEASE_LEVEL	5 // 2
#else
#define DEBUG_LEVEL		0
#define EVENT_LEVEL		1
#define RELEASE_LEVEL	2
#endif
#endif

#if (SYS_DVB)
//#define _EZCALIBRATION_ADD //090330 mrbbu
#endif

#define INCLUDE_EXT_INPUT_ADJUST


#define ADC_MAX_CNT 64
#define VIDEO_INPUT_CNT 12
#define AUDIO_INPUT_CNT 8
#define EXT_INPUT_ADC_RANGE 8 //Ext. Input Adjust�� �� �׸� ���� ���� 0 ~ 8


#define USE_EU_AREA_OPT		1	// Temporarily open For Test - hunkyo.jung@lge.com

typedef enum
{
	INDEX_VIDEO_HDMI1				= 0,
	INDEX_VIDEO_HDMI2				= 1,
	INDEX_VIDEO_HDMI3				= 2,
	INDEX_VIDEO_HDMI4				= 3,
	INDEX_VIDEO_COMP1			= 4,
	INDEX_VIDEO_COMP_BIAS		= INDEX_VIDEO_COMP1,
	INDEX_VIDEO_COMP2			= 5,
	INDEX_VIDEO_COMP3			= 6,
	INDEX_VIDEO_RGB				= 7,
	INDEX_VIDEO_SCARTFULL		= 8,
	INDEX_VIDEO_RCA_AV1			= 9,
	INDEX_VIDEO_RCA_AV_BIAS		= INDEX_VIDEO_RCA_AV1,
	INDEX_VIDEO_RCA_AV2			= 10,
	INDEX_VIDEO_RCA_AV3			= 11,
	INDEX_MAX_VIDEO				= 12,
}EXT_INPUT_VIDEO_INDEX_T;

typedef enum
{
	INDEX_AUDIO_COMP1			= 0,
	INDEX_AUDIO_COMP2			= 1,
	INDEX_AUDIO_COMP3			= 2,
	INDEX_AUDIO_RGB				= 3,
	INDEX_AUDIO_SCARTFULL		= 4,
	INDEX_AUDIO_RCA_AV1			= 5,
	INDEX_AUDIO_RCA_AV_BIAS		= INDEX_AUDIO_RCA_AV1,
	INDEX_AUDIO_RCA_AV2			= 6,
	INDEX_AUDIO_RCA_AV3			= 7,
	INDEX_MAX_AUDIO				= 8,
}EXT_INPUT_AUDIO_INDEX_T;


/*-------------------------------------------------------------
 * macro function definitions
 *------------------------------------------------------------*/

/*-------------------------------------------------------------
 * type definitions
 *------------------------------------------------------------*/

/******************************************************************************
*	Basic Options
*******************************************************************************/

/* DISPLAY_TYPE */
typedef enum
{
	PLASMA_DISPLAY			= 0,
	LCD_DISPLAY 			= 1,
	DLP_PROJECTION			= 2,
	LCD_PROJECTION			= 3,
	CRT_DIRECTVIEW			= 4

}DISPLAY_TYPE_T;

/* LVDS bit */
typedef enum
{
	DISP_8BIT_LVDS			= 0, // 8bit modules
	DISP_10BIT_LVDS			= 1, // 10bits modules
}DISPLAY_LVDS_BIT_T;


/*Audio Power Voltage*/
typedef enum
{
	AUDIO_POWER_7V					= 0, // 7V Power
	AUDIO_POWER_24V					= 1, // 24V Power
	AUDIO_POWER_17V					= 2, // 17V Power add by earnest.son@lge.com
	AUDIO_POWER_20V					= 3, // 20V Power request by jhee.lee@lge.com
	SUPPORT_AUDIO_VOLT_MAX				= 4,
}AUDIO_POWER_V_T;

/*Audio Speaker Voltage*/
typedef enum
{
	AUDIO_SPEAKER_5W					= 0, // 8W Power
	AUDIO_SPEAKER_10W					= 1, // 10W Power
	AUDIO_SPEAKER_15W					= 2, // 15W Power
	AUDIO_SPEAKER_7W					= 3, // 7W Power
	SUPPORT_SPEAKER_WATT_MAX			= 4,
}AUDIO_SPEAKER_W_T;

/* 3D_TYPE_T */
typedef enum
{
	DISP_2D_MD			= 0, // 2D Models
	DISP_3D_PG			= 1, // 3D Passive Models
	DISP_3D_SG			= 2, // 3D Active Models
	DISP_3D_RESERVED	= 3, // 3D reserved

}DISPLAY_3D_TYPE_T;

/* DISPLAY_MODULE_TYPE_T */
typedef enum
{
	DISP_MODULE_XGA			= 0,
	DISP_MODULE_WXGA		= 1,
	DISP_MODULE_FHD			= 2,
	DISP_MODULE_FRC			= 3,

}DISPLAY_MODULE_TYPE_T;

/* LVDS_TYPE_T */
typedef enum
{
    LVDS_VESA				= 0, 	/* LVDS Type is VESA/TI */
    LVDS_JEIDA				= 1 		/* LVDS Type is JEIDA */
}	LVDS_TYPE_T;

#if 1	/* ������� �ʴ� ������ ���� ���� �ʿ�. */
/* AMP_TYPE_T */
typedef enum
{
	/* GP */
	AMP_DEFAULT				= 0,
	AMP_32LH20				= 0,
	AMP_37LH20				= 1,
	AMP_42LH20				= 2,
	AMP_26LH20				= 3,
	AMP_22LH20				= 4,
	AMP_19LH20				= 5,

	AMP_32LH30				= 6,
	AMP_37LH30				= 7,
	AMP_42LH30				= 8,
	AMP_47LH30				= 9,

	AMP_32LH40				= 10,
	AMP_37LH40				= 11,
	AMP_42LH40				= 12,
	AMP_47LH40				= 13,
	AMP_55LH40				= 14,

	/* GP BCM */
	AMP_32LH70				= 15,
	AMP_37LH70				= 16,
	AMP_42LH70				= 17,
	AMP_47LH70				= 18,
	AMP_55LH70				= 19,

	/* PDP Model */
	AMP_42PG60				= 0,	/* 42PG6000  */
	AMP_50PG60				= 1,	/* 50PG6000  */
	AMP_60PG60				= 2,	/* 60PG6000  */
	AMP_60PG30				= 3,	/* 60PG30UD-AA:AUS  */
	AMP_42PG20				= 4,	/* 42PG20D-AA:AUS  */
	AMP_50PG20				= 5,	/* 50PG20D-AA:AUS  */

	/* LCD Model */
	AMP_32LG60				= 50,	/* 32PG6000  */
	AMP_37LG60				= 51,	/* 37PG6000  */
	AMP_42LG60				= 52,	/* 42PG6000  */
	AMP_47LG60				= 53,	/* 47PG6000  */

	AMP_37LB1				= 0,	/* PY3, LY3, 50PB3D, 42LB3D */
	AMP_32LB1				= 1,	/* 32LB1 */
	AMP_37LC2				= 2,	/* 37,42,47 + LB,LC */
	AMP_32LC2				= 3,	/* 26,32 + LB,LC */
	AMP_50PC1				= 4, 	/* PB4,PB5 */
	/* �Ʒ��� Everest3 �� ���� �� */
	AMP_47LY3				= 5, 	/* 47LY3D */
	AMP_42LY3				= 6, 	/* 37,42LY3D */
	AMP_50PB3				= 7, 	/* 50PB3D */
	AMP_32LB3				= 8,	/* 32LB3D */
	AMP_42LB5				= 9, 	/* 47/42LB5D, 42LB4 */
	AMP_60PT1				= 10, 	/* 60PT1 */
	AMP_37LB5				= 11, 	/* 37LB5,37LB4 */
	AMP_60PY3				= 12,	/* 60PY3 */
	AMP_32LB4				= 13, 	/* 32LB4D */
	AMP_42PB4				= 14, 	/* 42PB4D */
	AMP_50PB4				= 15, 	/* 50PB4D */
	AMP_60PB4				= 16, 	/* 60PB4D */
	AMP_32LB9				= 17, 	/* 32LB9D */
	AMP_42LB9				= 18,
}AMP_TYPE_T;
#endif

typedef enum
{
	PDP_42G2	=0,
	PDP_50G2,
	PDP_42G2A,
	PDP_50H3,
	PDP_60H3,
}PDP_MODULE_NAME_T;

typedef enum
{
	ALL_GROUP			=0,
	ARAB_GROUP		=1,
	HEB_GROUP		=2
}LANG_GROUP_TYPE_T;


/* FRONT_DISPLAY_TYPE_T */
typedef enum
{
	FRONT_DISP_NONE 		= 0,
	FRONT_DISP_LED_INDEX	= 1,
	FRONT_DISP_VFD_PANEL	= 2

}FRONT_DISPLAY_TYPE_T;

/* PIP_TYPE_T */
typedef enum
{
	PIP_NON 				= 0,
	PIP_ONE_TUNER			= 1,
	PIP_TWO_TUNER			= 2

}PIP_TYPE_T;

/* FRC_CHIP_TYPE_T */
typedef enum
{
	FRC_CHIP_NONE   			= 0,
	FRC_CHIP_URSA2				= 1,
	FRC_CHIP_URSA3_INTERNAL		= 2,
	FRC_CHIP_URSA3_EXTERNAL		= 3,
	FRC_CHIP_PWIZ				= 4,
	FRC_CHIP_URSA4_INTERNAL		= 5,
	FRC_CHIP_URSA4_EXTERNAL		= 6,
	FRC_CHIP_URSA5_INTERNAL		= 7,
	FRC_CHIP_URSA5_EXTERNAL		= 8,
	FRC_CHIP_PWIZ2				= 9,//GP3 FRCII
	FRC_CHIP_BCM_INTERNAL		= 10,
	FRC_CHIP_LG1150				= 11,/* [L8] soyoung.kim(2010/08/24) - add */
	FRC_CHIP_MTK_INTERNAL		= 12,
	FRC_CHIP_S8_INTERNAL		= 13,
	FRC_CHIP_PWIZ3				= 14,//GP4 FRCIII
#ifdef INCLUDE_UD	
	FRC_CHIP_PWIZ3_CSR505		= 15,//GP4 FRCIII + CSR505
	FRC_CHIP_MAX				= 16,
#else
	FRC_CHIP_MAX				= 15,
#endif
}FRC_CHIP_TYPE_T;


#if (PLATFORM_TYPE == LG_PLATFORM)
#define FRC_CHIP_INTERNAL	FRC_CHIP_NONE
#elif(PLATFORM_TYPE == MSTAR_PLATFORM)
#define FRC_CHIP_INTERNAL	FRC_CHIP_S8_INTERNAL
#elif(PLATFORM_TYPE == MTK_PLATFORM)
#define FRC_CHIP_INTERNAL	FRC_CHIP_MTK_INTERNAL
#elif(PLATFORM_TYPE == BCM_PLATFORM)
#define FRC_CHIP_INTERNAL	FRC_CHIP_BCM_INTERNAL
#else
#define FRC_CHIP_INTERNAL	FRC_CHIP_NONE

#endif
/* TCON_CHIP_TYPE_T */
typedef enum
{
	TCON_CHIP_NONE   		= 0,
	TCON_CHIP_BUF16821		= 1,
	TCON_CHIP_MAX9668		= 2,
	TCON_CHIP_MAX			= 3,
}TCON_CHIP_TYPE_T;


#ifndef MOTION_REMOCON_TYPE_T
/* MOTION REMOCON TYPE */
typedef enum MOTION_REMOCON_TYPE
{
	MOTION_REMOCON_BUILTIN_M3   	= 0,
	MOTION_REMOCON_BUILTIN_M4		= 1,
	MOTION_REMOCON_BUILTIN_M4CI		= 2,
	MOTION_REMOCON_DONGLE			= 3,
	SUPPORT_MOTION_RC_MAX			= 4,
}__MOTION_REMOCON_TYPE_T;
#define MOTION_REMOCON_TYPE_T __MOTION_REMOCON_TYPE_T
#endif // MOTION_REMOCON_TYPE_T



/* MICOM_TYPE_T */
typedef enum
{
	MICOM_INTERNAL				= 0,
	MICOM_MYSON					= 1,
	MICOM_NEC					= 2,
	MICOM_SANYO					= 3
}MICOM_TYPE_T;


#ifndef SUPPORT_3D_TYPE_T
/**
 *	Type of 3D Support Type
 */
typedef enum SUPPORT_3D_TYPE
{
	SUPPORT_2D_ONLY,			/* support 2D only */
	SUPPORT_3D_IN_2D3D,			/* support 3D including 2D to 3D (active galsses 3D)*/
	SUPPORT_3D_MAX,
}__SUPPORT_3D_TYPE_T;

#define SUPPORT_3D_TYPE_T __SUPPORT_3D_TYPE_T
#endif	// SUPPORT_3D_TYPE_T


/**
 *	Type of 3DTV Display.
 */
 typedef enum
{
	TRID_DISPLAY_ACTIVE,		/* 3D Active method  */
	TRID_DISPLAY_PASSIVE,		/* 3D Passive method */
}TRIDTV_DISPLAY_TYPE;

/******************************************************************************
*	Input Number Options
******************************************************************************/
/* INPUT_TYPE_T */
typedef enum
{
	INPUT_TV				= 0,
	INPUT_SCART,
	INPUT_AV,
	INPUT_AUTOAV,
	INPUT_COMP,
	INPUT_RGB,
	INPUT_HDMI,
	INPUT_USB,
	INPUT_MAX,
	INPUT_INVALID			= 0xff

}INPUT_SRC_TYPE_T;

/******************************************************************************
*	Common Options
******************************************************************************/
/* TTX_MODE_TYPE_T */
typedef enum
{
	TTX_MODE_AUTO			= 0,
	TTX_MODE_TOP			= 1,
	TTX_MODE_FLOP			= 2,
	TTX_MODE_OFF			= 3

}TTX_MODE_TYPE_T;


/******************************************************************************
*	PDP Only Options
******************************************************************************/
/* PDP_MODULE_ID_T */
typedef enum
{
	PDP_MODULE_X4 			= 0x72,
	PDP_MODULE_X3			= 0x1c,
	PDP_MODULE_NON			= 0x00

}PDP_MODULE_ID_T;

/* PDP Temperature threshold*/
typedef struct _tag_PDP_TEMP_THRESHOLD{
	UINT32 DnLow;
	UINT32 DnHigh;
	UINT32 UpLow;
	UINT32 UpHigh;

}PDP_TEMP_T;

/******************************************************************************
*	LCD Only Options
******************************************************************************/

/******************************************************************************
*	Picture Quality Options
******************************************************************************/
/**
 * Picture Quality Library
 * @Enumeration and Structure
 */

#if (SYS_ATSC)
typedef enum
{
	/* LCD US model */
	PQL_LCD_US 				= 0x00000001,
	/* LCD KOR model */
	PQL_LCD_KOR				= 0x00000010,

	/* PDP US model */
	PQL_PDP_US				= 0x00010000,
	/* PDP KOR model */
	PQL_PDP_KOR				= 0x00100000,

} PQL_DB_T;
#else
typedef enum
{
	/* LCD EU model */
	PQL_LCD_EU 				= 0x00000001,
	/* LCD AUS model */
	PQL_LCD_AUS				= 0x00000010,
	/* LCD CHN model */
	PQL_LCD_CHN 			= 0x00000100,


	/* PDP EU model */
	PQL_PDP_EU				= 0x00010000,
	/* PDP AUS model */
	PQL_PDP_AUS				= 0x00100000,
	/* PDP CHN model */
	PQL_PDP_CHN 			= 0x01000000,

} PQL_DB_T;
#endif

/* Enumeration */

/* PQL_PIP_TYPE */
typedef enum
{
	PQL_PIP_NONE 			= 0,
	PQL_PIP_ONE_TUNER		= 1,
	PQL_PIP_TWO_TUNER		= 2

} PQL_PIP_T;

/* PQL_REGION */
typedef enum
{
	PQL_KR					= 0,//KOREA	//lachesis_080122 PQL DB���� ���Ǽ��� ���Ͽ� PQL_KR�� PQL_US�� �����Ͽ� ���. �� KR�� ������� ����.
	PQL_US					= 1,//USA
	PQL_EU					= 2,//Europe
	PQL_CN					= 3,//China
	PQL_AU					= 4,//Austrailia
	PQL_SG					= 5,//Singapol
	PQL_ZA					= 6,//���ư�
	PQL_VN					= 7,//Vietnam
	PQL_TW					= 8,//Taiwan
	PQL_A_ONLY				= 9,//Analog Only
} PQL_REGION_T;

/*PQL Support FRC*/
typedef enum
{
	PQL_FRC_NONE			= 0,
	PQL_FRC					= 1,
}PQL_SUPPORT_FRC_T;

/*PQL_Resolution */
typedef enum
{
	PQL_FHD					= 0,
	PQL_HD					= 1,
	PQL_XGA					= 2,
}PQL_RESOLUTION_T;

/* PDP Panel Type*/
typedef enum
{
	PDP_NORMAL_TYPE = 0,
	PDP_H_TYPE		= 1,
	PDP_A_TYPE		= 2,
}PQL_PDPPANEL_TYPE_T;


typedef enum
{
	PQL_TUNER_TDVWH103F		= 0,		//TDVWH103F
	PQL_TUNER_UCA36AL		= 1,		//UCA36AL
	PQL_TUNER_XC5000		= 2,		//XC5000
	PQL_TUNER_XC5000_ATSC   = 3,		//XC5000_ATSC
	PQL_TUNER_TDFWDRXK 		= 4,		//TDFWDRXK
	PQL_TUNER_TDFRZ751D		= 5,		//TDFRZ751D
}PQL_ANA_DEMOD_TYPE_T;

/* Picture Quality Library Configuration Structure */
typedef struct _tag_PICTURE_QUALITY_LIBRARY
{
	UINT32	eDisplayModule;
	UINT32	ePipType;
	UINT32	eModel;
	UINT32	eRegion;

#ifdef INCLUDE_NEW_DB_CODE
//based on the BCM platform
	PQL_DB_T ePqlDB;
#endif

} PQL_CONFIG_T;

/******************************************************************************
*	Currently No Using Options
******************************************************************************/
typedef enum
{
	SS_STATUS_DISABLE 		= 0,
	SS_STATUS_ENABLE		= 1

}SS_STATUS_TYPE_T;

typedef enum
{
	SS_RANGE_00 			= 0,
	SS_RANGE_10				= 10,
	SS_RANGE_15				= 15,
	SS_RANGE_20				= 20,
	SS_RANGE_25				= 25,
	SS_RANGE_30				= 30,
	SS_RANGE_35				= 35,
	SS_RANGE_40				= 40,
	SS_RANGE_45				= 45,
	SS_RANGE_50				= 50

}SS_RANGE_TYPE_T;

/* spread spectrum FreqMod setting 0 to 40KHz, Unit 1KHz*/
/* �ʿ�� �Ʒ� enum�� FreqMod�� �߰��Ͽ� ����ؾ���. */
typedef enum
{
	SS_FREQMOD_DEFAULT		= 20, 	/* 20KHz */
	SS_FREQMOD_5KHZ 		= 5, 	/* 5KHz  */
	SS_FREQMOD_10KHZ 		= 10,	/* 10KHz */
	SS_FREQMOD_12KHZ		= 12,	/* 12KHz */
	SS_FREQMOD_15KHZ 		= 15,	/* 15KHz */
	SS_FREQMOD_20KHZ 		= 20,	/* 20KHz */
	SS_FREQMOD_25KHZ 		= 25,	/* 25KHz */
	SS_FREQMOD_30KHZ 		= 30,	/* 30KHz */
	SS_FREQMOD_35KHZ 		= 35,	/* 35KHz */
	SS_FREQMOD_40KHZ 		= 40,	/* 40KHz */
	SS_FREQMOD_MAX 			= SS_FREQMOD_40KHZ

}SS_FREQMOD_TYPE_T;

/* spread spectrum */
typedef struct _tag_SPREAD_SPECTRUM{
	BOOLEAN bSSEnable ;
	UINT32	nSSPercent;
	UINT32	nSSPeriod;
}SS_CONFIG_T;

/* PDP_FAN_TYPE_T */
typedef enum
{
	FAN_NONE				= 0,
	FAN_1 					= 1,
	FAN_4 					= 2

} PDP_FAN_TYPE_T;


//balup_090811
typedef enum
{
	//gp2 PCB type
	PCB_EU_CN_LOW				= 0,
	PCB_EU_CN_HIGH				= 1,
	PCB_KR_US_BR_TW_AU_LOW		= 2,
	PCB_KR_US_BR_TW_AU_HIGH 	= 3,
	PCB_EU_CN_PREMIUM			= 4,
  	PCB_KR_BCM					= 7,
	PCB_EU_CN_SMALL				= 8,
	PCB_KR_US_BR_TW_AU_SMALL	= 9, // for LE5300(small)

	//saturn6, msart S7�� ���� �ӽ� type
	BOARD_A 		= 10,	//for saturn6 gp2 DVB LCD board
	BOARD_B 		= 11,	//for saturn6 gp2 PDP board
	BOARD_C 		= 12,	//for saturn6 gp2 ATSC board
	BOARD_D 		= 13,	//
	BOARD_S8_DEMO	= 14,	//saturn8 demo board

	//Mstar S8 Board Type
	BOARD_S8_ATSC_KOR 		= 15,
	BOARD_S8_ATSC_US 		= 16,
	BOARD_S8_ATSC_BRZ 		= 17,
	BOARD_S8_DVB_EU 		= 18,
	BOARD_S8_DVB_CN 		= 19,
	BOARD_S8_DVB_ETC 		= 20,

	BOARD_S8_ATSC_KOR_PDP	= 21,
	BOARD_S8_ATSC_US_PDP 	= 22,
	BOARD_S8_ATSC_BRZ_PDP 	= 23,
	BOARD_S8_DVB_EU_PDP 	= 24,
	BOARD_S8_DVB_CN_PDP 	= 25,
	BOARD_S8_DVB_ETC_PDP 	= 26,

	//BBTV BCM BOARD TYPE - ieeum
	BOARD_BCM_ATSC_KOR 		= 30,
	BOARD_BCM_ATSC_US 		= 31,
	BOARD_BCM_ATSC_BRZ 		= 32,
	BOARD_BCM_DVB_EU 		= 33,
	BOARD_BCM_DVB_CN 		= 34,
	BOARD_BCM_DVB_ETC 		= 35,

	//BBTV BCM PDP BOARD
	BOARD_BCM_DVB_EU_PDP	= 40,
	BOARD_BCM_ATSC_KOR_PDP	= 41,
	BOARD_BCM_ATSC_US_PDP	= 42,
	BOARD_BCM_ATSC_BRZ_PDP	= 43,
	BOARD_BCM_DVB_AU_PDP	= 44,

	//BBTV LG BOARD TYPE
	/* - [L8] soyoung.kim(2010/08/24) - add */
	BOARD_LG_ATSC_KOR 		= 50,
	BOARD_LG_ATSC_US 		= 51,
	BOARD_LG_DVB_EU 		= 52,
	BOARD_LG_DVB_ETC 		= 53,

	//BBTV MTK BOARD TYPE
	BOARD_MTK_ATSC_KOR 		= 60,
	BOARD_MTK_ATSC_US 		= 61,
	BOARD_MTK_ATSC_BRZ 		= 62,
	BOARD_MTK_DVB_EU 			= 63,
	BOARD_MTK_DVB_CN 			= 64,
	BOARD_MTK_DVB_ETC 		= 65,

	//BBTV MTK BOARD TYPE for PDP
	BOARD_MTK_ATSC_KOR_PDP 		= 70,
	BOARD_MTK_ATSC_US_PDP 		= 71,
	BOARD_MTK_ATSC_BRZ_PDP 		= 72,
	BOARD_MTK_DVB_EU_PDP 		= 73,
	BOARD_MTK_DVB_AU_PDP 		= 74,
	BOARD_MTK_DVB_TW_PDP 		= 75,


} MAIN_BOARD_T;


//balup_090529
// GP2 Event board Type
typedef enum
{
	BOARD_PT_1ST	=0,
	BOARD_PT_2ND	=1,
	BOARD_PT_3RD	=2,
	BOARD_PT_4TH	=3,
	BOARD_PT_5TH	=4,
	BOARD_DV_1ST	=5,
	BOARD_DV_2ND	=6,
	BOARD_DV_3RD	=7,
	BOARD_PV_1ST	=8,
	BOARD_PV_2ND	=9,
	BOARD_PV_3RD	=10,
	BOARD_PQ_1ST	=11,
	BOARD_PQ_2ND	=12,
	BOARD_PQ_3RD	=13,
	BOARD_MP_1ST	=14,
	BOARD_MP_2ND	=15
} EVENT_BOART_T;

//balup_090530
//GP2 Jackboard Type
typedef enum
{
	JACK_BOARD_A	= 0, //no-jackboard (only use mainboard)
	JACK_BOARD_B	= 1, //same gp1 venus jackboard
	JACK_BOARD_C	= 2,
	JACK_BOARD_D	= 3,
	JACK_BOARD_E	= 4,
	JACK_BOARD_F	= 5
} JACK_BOARD_T;


/******************************************************
 			TOOL_OPTION1 �� �ʿ��� ����ü ����
*****************************************************/
// �Ʒ� ����ü ���� �� �ݵ�� �� �׸��� MAX ���� ���� ������ ��
#define MAX_MODULE_VER		4


/**
 * Named size of panel, comes from nvm_ddi.h
 */
typedef enum
{
	INCH_19=0,
	INCH_22,
	INCH_23,	
	INCH_26,
	INCH_32,
	INCH_37,
	INCH_42,
	INCH_47,
	INCH_50,
	INCH_55,
	INCH_60,
	INCH_65,	
	INCH_72,
	INCH_84,	
	INCH_BASE
} INCH_TYPE_T;

/**
 * not used inch type
 */
typedef enum
{
	INCH_15= INCH_BASE,
    INCH_46,
	INCH_52,
	INCH_70,
} OLD_INCH_TYPE_T;




/**
 *	Type of COMPONENT link (Rear AV or Side AV)
 */
 typedef enum
{
	REAR_COMP	=0,
	SIDE_COMP,
} COMPONENT_LINK_T;


/**
 *	Type of RCA AV link (main AV or Side AV)
 */
 typedef enum
{
	REAR_AV	=0,
	SIDE_AV,
} RCA_AV_LINK_T;

/**
 *	Type of HDMI Link
 */
 typedef enum
{
	REAR_HDMI	=0,
	SIDE_HDMI,
} HDMI_BOARD_LINK_T;

/**
 *	Type of HDMI order
 */
typedef enum
{
	DVI_PORT_A	 = 1,
	DVI_PORT_B   = 2,
	DVI_PORT_C   = 4,
	DVI_PORT_D   = 3
} S7_DVI_PORT_T;

/**
 * index for HDMI port. 
*/
typedef enum
{
	INDEX_HDMI1	= 1,
	INDEX_HDMI2	= 2,
	INDEX_HDMI3	= 3,
	INDEX_HDMI4	= 4,
} HDMI_PORT_INDEX_T;

/**
 *	S7M/S7/S7Tcon ����
 */
 typedef enum
{
	SOC_S7M	=	0,
	SOC_S7T =	1,
	SOC_S7 	=	2,
} MAIN_SOC_T;


/**
 *	Remocon Type
 */
 typedef enum
{
	RC_TYPE_A	=	0,
	RC_TYPE_B	=	1,
	RC_TYPE_C	=	2,
	RC_TYPE_D  =     3,
} Remocon_T;





typedef enum
{

	TOOL_CS41=0, 	// 1
	TOOL_CS46,		// 2
	TOOL_CS56, 		// 3
	TOOL_CS57, 		// 4
	TOOL_EM96, 		// 5
	TOOL_LM46, 		// 6
	TOOL_LM58, 		// 7
	TOOL_LM62, 		// 8
	TOOL_LMG62, 	// 9
	TOOL_LM63,		// 10
	TOOL_LM66,		// 11
	TOOL_LM67,		// 12
	TOOL_LM76,		// 13
	TOOL_LM86,		// 14
	TOOL_LM96,		// 15
	TOOL_LM99,		// 16
	TOOL_LS22,		// 17
	TOOL_LS31,		// 18
	TOOL_LS35,		// 19
	TOOL_LS41,		// 20
	TOOL_LS46,		// 21
	TOOL_LS56,		// 22
	TOOL_LS57,		// 23
	TOOL_LM95,		// 24
	TOOL_LM64,		// 25 - cinema screen support		<LM6400>
	TOOL_LM645,		// 26 - cinema screen not support	<LM6450>
	TOOL_LM61,		// 27 
	TOOL_LCD_END,

	//PDP Tool
	TOOL_PM990= TOOL_LCD_END,	// 1
	TOOL_PM980,					// 2
	TOOL_PM970,					// 3
	TOOL_PM950,					// 4
	TOOL_PM890,					// 5
	TOOL_PM880,					// 6
	TOOL_PM870,					// 7
	TOOL_PM850,					// 8
	TOOL_PM690,					// 9
	TOOL_PM680,					// 10
	TOOL_PM670,					// 11
	TOOL_PM650,					// 12
	TOOL_PM590,					// 13
	TOOL_PM580,					// 14
	TOOL_PM570,					// 15
	TOOL_PM550,					// 16
	TOOL_PM490,					// 17
	TOOL_PM480,					// 18
	TOOL_PM470,					// 19
	TOOL_PM450,					// 20
	TOOL_PA690,					// 21
	TOOL_PA670,					// 22
	TOOL_PA650,					// 23
	TOOL_PA590,					// 24
	TOOL_PA570,					// 25
	TOOL_PA550,					// 26
	TOOL_PA490,					// 27
	TOOL_PA470,					// 28
	TOOL_PA450,					// 29

	TOOL_PDP_END,
	TOOL_BASE	= TOOL_PDP_END,	// LCD Tool�� �߰��Ǿ� PDP Tool�� �����ٴ� ���������� �����.
} TOOL_TYPE_T;

//not used tool type (old type)
typedef enum
{
	TOOL_LK31= TOOL_PDP_END,    // 1 - Analog
	TOOL_LK33,      // 2
	TOOL_LK43,		// 3
	TOOL_LK45,		// 4 - FHD
	TOOL_LK52,		// 5
	TOOL_LK53,		// 6
	TOOL_LK55,		// 7 - 120Hz
	TOOL_LV23,		// 8
	TOOL_LV25,		// 9
	TOOL_LV33,		// 10
	TOOL_LV34,		// 11
	TOOL_LV35, 		// 12
	TOOL_LV45,		// 13
	TOOL_LW45,		// 14
	TOOL_LV55,		// 15
	TOOL_LW55,		// 16
	TOOL_LW65,		// 17
	TOOL_LW75,		// 18
	TOOL_LV75,		// 19
	TOOL_LW77,		// 20
	TOOL_LW85,		// 21
	TOOL_LW95,		// 22
	TOOL_LW96,		// 23
	TOOL_LW97,		// 24 - 72" IOL, 60" IOP
	TOOL_LW98,		// 25 - 21:9
	TOOL_LW56,		// 26
	TOOL_LW57,		// 27
	TOOL_LV37,		// 28
	TOOL_LZ97,		// 29
	TOOL_LV54,		// 30
	TOOL_LV57,		// 31
	TOOL_LZ96,		// 32
	TOOL_LV375,		// 33
	TOOL_LV77,		// 34
	TOOL_LV95,		// 35
	TOOL_LV47,		// 36
	TOOL_LW47,		// 37
	TOOL_LK95,		// 38
	TOOL_LZ99,		// 39
} OLD_TOOL_TYPE_T;
typedef enum
{
	MODULE_LGD = 0,
	MODULE_CMI,
	MODULE_AUO,
	MODULE_SHARP,
	MODULE_IPS,
	MODULE_LCD_END,

	MODULE_LGE = MODULE_LCD_END,
	MODULE_PANASONIC,
	MODULE_PDP_END,
	MODULE_BASE	= MODULE_PDP_END
} MODULE_MAKER_TYPE_T;

//balup_090626
typedef enum
{
	MODULE_VER0		=0,		//not support error out, not support scanning backlight
	MODULE_VER1		=1,     //support error out, not support scanning backlight
	MODULE_VER2		=2,     //support scanning backlight, not support error out
	MODULE_VER3	 	=3		//support error out, support scanning backlight
} MODULE_VERSION_TYPE_T;



/******************************************************
 			TOOL_OPTION2 �� �ʿ��� ����ü ����
******************************************************/
// �Ʒ� ����ü ���� �� �ݵ�� �� �׸��� MAX ���� ���� ������ ��
#define SCART_TYPE_MAX			2
#define RCA_COUNT_MAX			4
#define COMP_COUNT_MAX 		4
#define HDMI_SWITCH_TYPE_MAX	3
#define HDMI_COUNT_MAX			4
#define HDMI_SWAP_ORDER_MAX		7

/* Hdmi swap order */
typedef enum
{
	TYPE_1243 = 0,
	TYPE_1234,
	TYPE_1324,
	TYPE_2143,
	TYPE_4321,
	TYPE_3421,
	TYPE_2134,
}HDMI_SWAP_ORDER_T;

/* Number of USB */
typedef enum
{
	LCD_MODULE_V6	= 0,
	LCD_MODULE_V5	= 1,
}LCD_MODULE_VER_T;


/* Number of USB */
typedef enum
{
	USB_COUNT_1	= 0,
	USB_COUNT_2,
}USB_INPUT_TYPE_T;

/*  Type of Scart input */
typedef enum
{
	SCART_NONE=0,
	SCART_FULL,
	//SCART_FULL_HALF,
}SCART_TYPE_T;

/* EU country Type */
typedef enum
{
	COUNTRY_OTHERS = 0, /* T2 + Othres */
	COUNTRY_NORDIC,
}EUCOUNTRY_TYPE_T;

/*  Number of RCA Input */
typedef enum
{
	RCA_COUNT_1 =0,
	RCA_COUNT_2,
	RCA_COUNT_3,
	RCA_COUNT_4,
}RCA_TYPE_T;

/*   Number of Component Input */
typedef enum
{
	COMP_COUNT_1 =0,
	COMP_COUNT_2,
	COMP_COUNT_3,
	COMP_COUNT_4,
}COMP_TYPE_T;

/*HDMI_SWITCH_TYPE_T */
typedef enum
{
	HDMI_SWITCH_NONE		= 0,
	HDMI_SWITCH_SIL9287		= 1,
	HDMI_SWITCH_SIL9587		= 2,

}HDMI_SWITCH_TYPE_T;

/* Number of HDMI */
typedef enum
{
	HDMI_COUNT_1 =0,
	HDMI_COUNT_2 ,
	HDMI_COUNT_3 ,
	HDMI_COUNT_4 ,
}NUM_HDMI_TYPE_T;

/******************************************************
 			TOOL_OPTION3 �� �ʿ��� ����ü ����
******************************************************/
#define AMP_CHIP_TYPE_MAX			6
#define BACKLIGHT_TYPE_MAX		6

/* AMP type */
typedef enum
{
	AMP_NTP7400 = 0,	/* NTP7400  */
	AMP_NTP7500,		/* NTP7500	*/
	AMP_NTP2AMP,		/* NTP2AMP  */
#if 0
	AMP_NTPSNDBAR,		/* Sound Bar 2AMP */ //LEX8
	AMP_TASSNDBAR,		/* TAS5709  */
	AMP_TAS5727,		/* TAS5727  */
	AMP_TAS2AMP,
#endif
	AMP_STA368,			/* ST368	*/
	AMP_STA380,			/* ST380	*/
	AMP_STA2AMP,		/* STA2AMP	*/

}AMP_CHIP_TYPE_T;

/* BREATHING_LED_T */
typedef enum
{
	LCD_BREATHING_LED_NONE = 0,
	LCD_BREATHING_LED_ONLY = 1,
	LCD_BREATHING_LED_WITH_SOUND = 2,
	PDP_BREATHING_LED_NONE = LCD_BREATHING_LED_NONE,
	PDP_BREATHING_LED_ONLY = LCD_BREATHING_LED_ONLY
} BREATHING_LED_T;


/* Backlight type*/
typedef enum
{
	BL_ROW = 0,
	BL_NOR_LED,
	BL_EDGE_LED,
	//BL_IOP_LED,
	BL_ALEF_LED,
	BL_IOL_LED,
	BL_OLED,
	BL_POLA,
	BL_END,
	
	//Not use
	BL_CCFL,
	//BL_ALEF_LGD,
	//BL_ALEF_FPR,
}BACKLIGHT_TYPE_T;


/* Backlight type*/
typedef enum
{
	BL_IOP_LED = BL_END,
	BL_ALEF_LGD,
	BL_ALEF_FPR,
}OLD_BACKLIGHT_TYPE_T;

/* EDID type*/
typedef enum
{
	EDID_PCM = 0,
	EDID_AC3,
	EDID_RESERVED,
	SUPPORT_EDID_TYPE_MAX,
}EDID_DATA_TYPE_T;


/**
 *	Type of EU Area Option Country
 */
 typedef enum
{
	OPT_AREA_OLD_EU		=0,		//nordic �̿� ���� ��ü�� grouping �� ���, ���� SW�� ����� nordic�̿ܿ� ���� ���п� �ش��� 
	OPT_AREA_EAST_EU	=1,		//EAST EU ���� grouping
	OPT_AREA_WEST_EU	=2,		//WEST EU ���� grouping
	OPT_AREA_ETC_EU		=3,		//Nordic/EAST/WEST �̿� ���� grouping (CIS ���þ� ��)
	OPT_AREA_EU_MAX		=4,
} OPT_AREA_EU_COUNTRY_T;


/* TRIDFMT_CHIP_TYPE_T */	//added by yonguk.lee@lge.com , 2009.12.30
#ifdef INCLUDE_TRIDTV
typedef enum
{
	TRIDFMT_CHIP_NONE   	= 0,
	TRIDFMT_CHIP_LGFPGA		= 1,
	TRIDFMT_CHIP_BCM35425	= 2,
	TRIDFMT_CHIP_FRCII		= 3,
}TRIDFMT_CHIP_TYPE_T;
#endif

/******************************************************
 			TOOL_OPTION4 �� �ʿ��� ����ü ����
*****************************************************/
// �Ʒ� ����ü ���� �� �ݵ�� �� �׸��� MAX ���� ���� ������ ��
#define DIGITAL_DEMOD_MAX		24 // 19 + 2(for DVB-S) kyn + 1(for DTMB-ATBM884x) Alex + 2 (for DTMB-LGDT3921 and DTMB-ATBM8859)
#define ANALOG_DEMOD_MAX		7
#define DIGITAL_DEMOD_S_MAX	4
#define USB_INPUT_MAX			3

typedef enum
{
	DIGITAL_DEMOD_S_DEFAULT = 0,
	DVBS_DS3002,
	DVBS_DS3103,
	NO_DEMODS
}DIG_DEMOD_S_TYPE_T;

typedef enum
{
	DIGITAL_DEMOD_DEFAULT = 0,		// 0
	ATSC_L9,						// 1
	ATSC_S8,						// 2
	ATSC_BCM,						// 3
	ATSC_LGDT3305,					// 4
	ISDB_MN884433,					// 5
	ISDB_TC90517,						// 6
	ISDB_BCM,						// 7
	ISDB_TC90522,					// 8
	CN_LGDT3900,					// 9
	CN_LGDT3911,					// 10
	CN_LGS8G85,						// 11
	CN_ATBM884X,					// 12
	CN_LGDT3921,					//13
	CN_ATBM8859,					//14
	DVB_SONY_T2,					// 15
	DVB_BCM,						// 16
	DVB_L9,							// 17
	DVB_S8,							// 18
	DVB_MTK,						// 19
	DVB_MSB1231,					// 20
	DVB_SI2165,						// 21
	ATSC_MTK,						// 22
	NO_DEMOD						// 23

}DIG_DEMOD_TYPE_T;


typedef enum
{
	ANA_DEMOD_DEFAULT	= 0,
	CHB,			/* CHannel Browser */
	LGT10_R,
	SI2173,
	SI2176,
	HST_J222D,
	SUT_PJ517,

	//not used
	LGT10,
	XC5000,
	SANYO
}ANA_DEMOD_TYPE_T;

//balup_090625
typedef enum
{
	NO_FRC_50HZ			=0,		//no frc
	S7M_IM_100HZ			=1,		//S7M internal FRC 100Hz
	S7_LG_200HZ			=2, 	//S7 + external LG FRC 200Hz
	BCM_URSA3_100HZ		=3		//BCM + URSA3 100Hz
} FRC_TYPE_T;

//balup_090625
typedef enum
{
	FRAME_50HZ			=0,
	FRAME_100HZ		=1,
	FRAME_200HZ		=2,
	FRAME_400HZ		=3
} FRC_FRAME_TYPE_T;

// BB_BlackBox type
typedef enum
{
	NO_BLACK_BOX = 0,
	BLACK_BOX_PT1,
	BLACK_BOX_PT2,
	BLACK_BOX_PT3,
	BLACK_BOX_PT4,
	BLACK_BOX_PV1,
	BLACK_BOX_PV2,
	BLACK_BOX_PV3,
	BLACK_BOX_PV4,
	BLACK_BOX_PQ1,
	BLACK_BOX_PQ2,
	BLACK_BOX_MP1,
	BLACK_BOX_MP2
} BLACK_BOX_TYPE_T;
// BB_BlackBox type

typedef enum
{
	MAX_PWM_DEFAULT	 = 0,
	MAX_PWM_5PER		 = 1,
	MAX_PWM_10PER		 = 2,
	MAX_PWM_15PER		 = 3,
	MAX_PWM_20PER		 = 4,
	MAX_PWM_25PER		 = 5,
	MAX_PWM_30PER		 = 6,
	MAX_PWM_35PER		 = 7,
	MAX_PWM_MAX_CNT	 = 8,
} MAX_PWM_TYPE_T;

typedef enum
{
	LED_BAR_REV_BLK	 = 0,
	LED_BAR_H_4BLK		 = 1,
	LED_BAR_H_12BLK		 = 2,
	LED_BAR_H_6BLK		 = 3,

	LED_BAR_D_10BLK		= 4,
	LED_BAR_D_16BLK		= 5,
	LED_BAR_REV2_BLK		= 6,
	LED_BAR_REV3_BLK		= 7,
	LED_BAR_MAX	 		= 8,
} LED_BAR_TYPE_T;


typedef enum
{
	LD_10_16_BLK		 = 0, //V_16BLK, V10BLK, RES_I, RES_II  (for 2011 modules)
	LD_6_12_BLK		 = 1, //V_12BLK, V_6BLK, H_12BLK, H_6BLK (for 2012 modules)
	LD_BLK_MAX	 	 = 2,
} LOCAL_DIM_BLOCK_TYPE_T;


typedef enum
{
	PENTOUCH_OFF		 = 0,
	PENTOUCH_READY		 = 1,
	PENTOUCH_BUILTIN	 = 2,
	PENTOUCH_NONE		 = 3, //not defined 
	PENTOUCH_MAX	 	 = 4,
} PDP_PENTOUCH_TYPE_T;



typedef enum
{
	LED_CUR_MINIMUN				= 0,
	LED_CUR_DEFAULT				= 1, //no-jackboard (only use mainboard)
	LED_CUR_DEFAULT_PLUS_I		= 2, //same gp1 venus jackboard
	LED_CUR_DEFAULT_PLUS_II		= 3,
	LED_CUR_MAX					= 4,
} LED_CURRENT_T;


typedef enum
{
	PWM_FREQ_50_60_HZ				= 0,
	PWM_FREQ_100_120_HZ				= 1, 
	PWM_FREQ_LVDS_LINK_MAX			= 2,
} PWM_FREQ_T;


//stonedef -  090717
/******************************************************
 			Area Option �� �ʿ��� ����ü ����
******************************************************/
// �Ʒ� ����ü ���� �� �ݵ�� �� �׸��� MAX ���� ���� ������ ��
#define LANG_GROUP_MAX			3
#define DEFAULT_LANG_MAX		3
#define TTX_LANG_GROUP_MAX		6
#define LOCATION_MAX			8
#define WIFI_FREQ_MAX			8

typedef enum{
	LANG_GROUP_AJ_ALL = 0,
	LANG_GROUP_AJ_ARABIC,
	LANG_GROUP_AJ_HEBREW
}LANG_GROUP_T;

typedef enum{
	DEFAULT_LANG_ENGLISH = 0,
	DEFAULT_LANG_PORTUGUESE,
	DEFAULT_LANG_SPANISH
}DEFAULT_LANG_T;

typedef enum{
	TTX_LANG_GROUP_WES_EU = 0,
	TTX_LANG_GROUP_EST_EU,
	TTX_LANG_GROUP_RUS,
	TTX_LANG_GROUP_ARA,
	TTX_LANG_GROUP_FARSI,
	TTX_OFF
}TTX_LANG_GROUP_T;

// 090721 ������� ��û�� Location �� ����
typedef enum{
	LOCATION_OFF = 0
}LOCATION_T;

typedef enum{
	WIFI_FREQ_GROUP_0 = 0,
	WIFI_FREQ_GRUOP_1,
	WIFI_FREQ_GROUP_2,
	WIFI_FREQ_GROUP_3,
	WIFI_FREQ_GROUP_4,
	WIFI_FREQ_GROUP_5,
	WIFI_FREQ_GROUP_6,
	WIFI_FREQ_GROUP_7,
}WIFI_FREQ_T;



/**
* sw.byeon - 100910 : ���ɼ� Union �� ���� ��������.
* �� Tool Option�� �׸� �߰���, �� �Ʒ��� �߰��Ұ�.
* ���� �߰��� ��� Bit�� �и��� ������ ToolOption Valid mark�� �����Ͽ� �ʱ�ȭ���� ������ ���� Ʋ�����Ե�.
* ���� ������� �߰��ؾ� OSD �� ���ɼ� enum�� ������ �����ϰ� ��.
**/

/* Ext input Adjust */
typedef struct
{
	UINT8					nHDMI1;
	UINT8					nHDMI2;
	UINT8					nHDMI3;
	UINT8					nHDMI4;
	UINT8					nCOMP1;
	UINT8					nCOMP2;
	UINT8					nCOMP3;
	UINT8					nRGB1;
	UINT8 					nVideoScartFullportA;
	UINT8					nRCA_AV1;
	UINT8					nRCA_AV2;
	UINT8					nRCA_AV3;
	UINT8					nReserved1;
	UINT8					nReserved2;
	UINT8					nReserved3;
} EXT_VIDEO_INPUT_ADJ_T;

typedef struct
{
	UINT8					nCOMP1;
	UINT8					nCOMP2;
	UINT8					nCOMP3;
	UINT8					nRGB1;
	UINT8 					nAudioScartFullportA;
	UINT8					nRCA_AV1;
	UINT8					nRCA_AV2;
	UINT8					nRCA_AV3;
	UINT8 					nReserved1;
	UINT8					nReserved2;
	UINT8					nReserved3;
} EXT_AUDIO_INPUT_ADJ_T;


/**
 *  Tool Option1 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16 					eModelInchType  		: 4;
		UINT16					eModelToolType			: 7;
		UINT16					eModelModuleType		: 4;
		UINT16					nLVDSBit				: 1;
	} flags;

} TOOL_OPTION1_T;


/**
 *  Tool Option2 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16 					nExtVideoInputIndex		: 6;
		UINT16					nExtAudioInputIndex		: 6;
		UINT16					eBackLight				: 3;
		UINT16					bLocalDimMenu			: 1;
	} flags;
} TOOL_OPTION2_T;

/**
 *  Tool Option3 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16 					bDivx						: 1;
		UINT16 					bToolDB3_reserved_I			: 1;
		UINT16 					bDLNA						: 1;
		UINT16 					bDigitalEye 				: 1;
		UINT16					bHeadphone					: 1;
		UINT16					bLogoLight					: 1;
		UINT16					bDVRReady					: 1;
		UINT16 					bTHX						: 1;
		UINT16					bPictureWizard				: 1;
		UINT16					bISF						: 1;
		UINT16 					bSupportEPA					: 1;
		UINT16					eHDMISwitchIC				: 2;
		UINT16					bSupportDTCPKey				: 1;
		UINT16					bDualView					: 1;
		UINT16					bToolDB3_reserved			: 1;
	} flags;
} TOOL_OPTION3_T;

/**
 *  Tool Option4 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16 					bLocalDimming		: 1;
		UINT16					bPowerBoardType		: 1;
		UINT16					eDigitalDemod_S		: 3;
		UINT16					eDigitalDemod		: 5;
		UINT16					eAnalogDemod		: 3;
		UINT16					eAudioAmp			: 3;
	} flags;

} TOOL_OPTION4_T;

/**
 *  Tool Option5 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16					bSupportWiFi			: 1;
		UINT16					bSupportWiFiBuiltin  	: 1;
		UINT16					bSupportCameraReady	: 1;
		UINT16					eSupportMouseRC		: 2;
		UINT16					bChBrowser				: 1;
		UINT16					bTVLink 				: 1;
		UINT16					bMirrorMode 			: 1;
		UINT16					bOrangeService			: 1;
		UINT16					bNetcastService 		: 1;
		UINT16					bSupportCNTV			: 1;
		UINT16					e3DModelType			: 1;
		UINT16					bLoalDim_BLK_Type		: 1;
		UINT16					bPanelErrorOut			: 1;
		UINT16					bScanningBL			: 1;
		UINT16					bSupport3DTHX			: 1;
	} flags;

} TOOL_OPTION5_T;

/**
 *  Tool Option6 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16					nDefaultStdBacklight		: 4;
		UINT16					nNumOfUSBInput				: 2;
		UINT16					bToolDB6_reserved_I			: 1;
		UINT16					bToolDB6_reserved_II		: 1;
		UINT16 					nAudioPowerVoltage			: 2;
		UINT16 					nSpeakerWattage				: 3;
		UINT16 					eMaxPwmType					: 3;
	} flags;
} TOOL_OPTION6_T;

/**
 *  Tool Option7 of Ez Adjust Menu(Service Menu).
 */
typedef union
{
	UINT16 all;
	struct
	{
		UINT16					bMAC_AP					: 1;
		UINT16					bESN_AP					: 1;
		UINT16					bCI_AP					: 1;
		UINT16					bWIDEVINE_AP			: 1;
		UINT16 					bMARLIN_AP				: 1;
		UINT16 					eEDID_Type				: 2;
		UINT16					eLEDCurrent				: 2;
		UINT16					eLEDBarType				: 2;
		UINT16					bDiiVa					: 1;
		UINT16					bPWMFreq				: 1;
		UINT16					bXvYcc					: 1;
		UINT16					bMRCU					: 1;
		UINT16					bGesture				: 1;
	} flags;
} TOOL_OPTION7_T;


// stonedef - 090717
// Area option ����ü ����
typedef union
{
	UINT16 all;
	struct
	{
		UINT16					eWiFiFreq			: 3;		/* Wi-Fi Frequency */
		UINT16					eLangGroup			: 2;		/* Langage Group */
		UINT16					eTTXLangGroup		: 3;		/* Teletext Langage Group */
		UINT16					bI_II_Save			: 1;		/* I/II Save �������� */
		UINT16					bHDEV				: 1;		/* Audio HDEV On/Off*/
		UINT16					bMONO				: 1;		/* ���� ��� �������� */
		UINT16					eLocation			: 3;
		UINT16					bIsolator			: 1;		/* Isolator ���� ���� */
	}flags;
}AREA_OPTION_T;


//sw.byeon - 100804 (EU Area Option - �赿��C ��û)
typedef union
{
	UINT16 all;
	struct
	{
		UINT16					nNordicUI			: 1;			/* 00:Non-Nordic(UK/Ireland+Others), 01:Nordic */
		UINT16 					bHwOption			: 1;
		UINT16					bT2					: 1;			/* T2 ���� �� Check_Hardware_Option */
		UINT16					bC2					: 1;			/* C2 ���� �� Check_Hardware_Option */
		UINT16					bSatellite			: 1;			/* �������� �� Check_Hardware_Option */
		UINT16					bMHP				: 1;			/* MHP ���� �� */
		UINT16					nHbbtv				: 2;			/* 00:Hbbtv Disable, 01:����, 10:Enable, 11:reserve */
		UINT16					bIsolator			: 1;			/* isolator ���� ���� */
		UINT16					eEastWest_EU		: 2;			/* ��/�� ���� ���� */
		UINT16					bArabicLang			: 1;			/* ArabicLang support */
		UINT16 					nReserved			: 4;
	}flags;
}AREA_OPTION_EU_T;


/**
 *	 Menu ID List of Ez Adjust Tool Option.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT_OPTNUMBER = 0,
	ID_TOOLOPT_INCH,
	ID_TOOLOPT_TOOL,
	ID_TOOLOPT_MODULE_MAKER,
	ID_TOOLOPT_LVDS_BIT,
	NUM_TOOL_OPTION1 					/**< Number of EzAdjust Tool Option */
};



/**
 *	 Menu ID List of Ez Adjust Tool Option2.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT2_OPTNUMBER = 0,
	ID_TOOLOPT2_EXT_VIDEO_INDEX ,
	ID_TOOLOPT2_EXT_AUDIO_INDEX ,
	ID_TOOLOPT2_BACKLIGHT_TYPE ,
	ID_TOOLOPT2_LOCALDIM_MENU,
	NUM_TOOL_OPTION2					/**< Number of EzAdjust Tool Option */
};


/**
 *	 Menu ID List of Ez Adjust Tool Option3.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT3_OPTNUMBER = 0,
	ID_TOOLOPT3_DIVX,
	ID_TOOLOPT3_RES_I,
	ID_TOOLOPT3_DLNA,
	ID_TOOLOPT3_DIGITAL_EYE,
	ID_TOOLOPT3_HEADPHONE,
	ID_TOOLOPT3_LOGO_LIGHT,
	ID_TOOLOPT3_DVR_READY,
	ID_TOOLOPT3_THX,
	ID_TOOLOPT3_PICTUREWIZARD, //090330 MRBBU
	ID_TOOLOPT3_ISF,
	ID_TOOLOPT3_ENERGY_STAR,
	ID_TOOLOPT3_HDMI_SWITCH_IC,
	ID_TOOLOPT3_DTCP_KEY,
	ID_TOOLOPT3_DUALVIEW,
	ID_TOOLOPT3_RES_II,
	NUM_TOOL_OPTION3					/**< Number of EzAdjust Tool Option */
};

/**
 *	 Menu ID List of Ez Adjust Tool Option4.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT4_OPTNUMBER = 0,
	ID_TOOLOPT4_LOCAL_DIMMING,
	ID_TOOLOPT4_POWER_BOARD_TYPE,
	ID_TOOLOPT4_DIGITAL_DEMOD_S,
	ID_TOOLOPT4_DIGITAL_DEMOD,
	ID_TOOLOPT4_ANALOG_DEMOD,
	ID_TOOLOPT4_AUDIO_AMP,
	NUM_TOOL_OPTION4					/**< Number of EzAdjust Tool Option */
};

/**
 *	 Menu ID List of Ez Adjust Tool Option5.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT5_OPTNUMBER = 0,
	ID_TOOLOPT5_WIFI,				/* BBTV WiFi ���� ���� */
	ID_TOOLOPT5_WIFI_BUILT_IN,				/* BBTV WiFi ���� ���� */
	ID_TOOLOPT5_CAMERA_READY,				/* BBTV Skype */
	ID_TOOLOPT5_MOTION_REMOCON,		/* ���� ���콺 ��� ���� ���� */
	ID_TOOLOPT5_CH_BROWSER,			/* Channel Browser ���� ���� */
	ID_TOOLOPT5_SET_ID,				/* Set ID ��� ���� ���� */
	ID_TOOLOPT5_MIRROR_MODE,			/* ȭ�� Mirror ��� ��� ���� ���� */
	ID_TOOLOPT5_ORANGE_SERVICE,
	ID_TOOLOPT5_NETCAST_SERVICE,
	ID_TOOLOPT5_CNTV,				/*xianghua.yin  20100805  for China CNTV*/
	ID_TOOLOPT5_3D_TYPE,
	ID_TOOLOPT5_LD_BLOCK_TYPE,  //Local dimming Block type
	ID_TOOLOPT5_PANEL_ERROR_OUT,
	ID_TOOLOPT5_SCANNING_BL,
	ID_TOOLOPT5_3DTHX,
	NUM_TOOL_OPTION5					/**< Number of EzAdjust Tool Option */
};


/**
 *	 Menu ID List of Ez Adjust Tool Option6.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT6_OPTNUMBER = 0,
	ID_TOOLOPT6_STD_BACKLIGHT,
	ID_TOOLOPT6_USB_COUNT,
	ID_TOOLOPT6_RESER_I,
	ID_TOOLOPT6_RESER_II,
	ID_TOOLOPT6_AUDIO_POWER_VOLT,
	ID_TOOLOPT6_SPEAKER_VOLT,
	ID_TOOLOPT6_MAXPWM,
	NUM_TOOL_OPTION6					/**< Number of EzAdjust Tool Option6 */
};
/**
 *	 Menu ID List of Ez Adjust Tool Option7.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum{
	ID_TOOLOPT7_OPTNUMBER = 0,
	ID_TOOLOPT7_MAC_AP,
	ID_TOOLOPT7_ESN_AP,
	ID_TOOLOPT7_CI_AP,
	ID_TOOLOPT7_WIDEVINE_AP,
	ID_TOOLOPT7_MARLIN_AP,
	ID_TOOLOPT7_EDID_TYPE,
	ID_TOOLOPT7_LED_CURRENT,
	ID_TOOLOPT7_LED_BAR_TYPE,
	ID_TOOLOPT7_DIIVA,
	ID_TOOLOPT7_PWMFREQ,
	ID_TOOLOPT7_XVYCC,
	ID_TOOLOPT7_MRCU,
	ID_TOOLOPT7_GESTURE,
	NUM_TOOL_OPTION7					/**< Number of EzAdjust Tool Option7 */
};

/**
 *	 Menu ID List of Ez Adjust Area Option.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum {
	ID_AREAOPT_OPTNUMBER = 0,
	ID_AREAOPT_WIFI_FREQUENCY,
	ID_AREAOPT_LANG_GR,
	ID_AREAOPT_TTX_LANG_GR,
	ID_AREAOPT_I_IISAVE,
	ID_AREAOPT_HDEV,
	ID_AREAOPT_MONO,
	ID_AREAOPT_LOCATION,
	ID_AREAOPT_ISOLATOR,
	NUM_AREA_OPTION
};


//sw.byeon - 100804 (EU Area Option - �赿��C ��û)
/**
 *	 Menu ID List of Ez Adjust EU Area Option.
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 */
enum {
	ID_EU_AREAOPT_OPTNUMBER = 0,
	ID_EU_AREAOPT_NORDIC,
	ID_EU_AREAOPT_HW_OPTION,
	ID_EU_AREAOPT_T2,
	ID_EU_AREAOPT_C2,
	ID_EU_AREAOPT_SATLLITE,
	ID_EU_AREAOPT_MHP,
	ID_EU_AREAOPT_HBBTV,
	ID_EU_AREAOPT_ISOLATOR,
	ID_EU_AREAOPT_EAST_WEST,
	ID_EU_AREAOPT_ARABIC_LANG,
	NUM_EU_AREA_OPTION
};

//sw.byeon - 100919
/**
 *	Menu ID List of Ez Adjust
 *	Ez Adjust Menu Window�� Menu List �� ID��� ��.
 *	���� : EXT_INPUT_VIDEO_INDEX_T , EXT_INPUT_AUDIO_INDEX_T;
 */


enum {
	ID_EXTINPUT_ADJ_VIDEO_INDEX 	= 0,

	ID_EXTINPUT_ADJ_VIDEO_HDMI1,	// 1
	ID_EXTINPUT_ADJ_VIDEO_HDMI2,
	ID_EXTINPUT_ADJ_VIDEO_HDMI3,
	ID_EXTINPUT_ADJ_VIDEO_HDMI4,
	ID_EXTINPUT_ADJ_VIDEO_COMP1,	// 5
	ID_EXTINPUT_ADJ_VIDEO_COMP2,
	ID_EXTINPUT_ADJ_VIDEO_COMP3,
	ID_EXTINPUT_ADJ_VIDEO_RGB,		// 8
	ID_EXTINPUT_ADJ_VIDEO_SCART_FULL,
	ID_EXTINPUT_ADJ_VIDEO_RCA_AV1,	// 10
	ID_EXTINPUT_ADJ_VIDEO_RCA_AV2,
	ID_EXTINPUT_ADJ_VIDEO_RCA_AV3,

	ID_EXTINPUT_ADJ_AUDIO_INDEX,	//13

	ID_EXTINPUT_ADJ_AUDIO_COMP1,	//14
	ID_EXTINPUT_ADJ_AUDIO_COMP2,
	ID_EXTINPUT_ADJ_AUDIO_COMP3,
	ID_EXTINPUT_ADJ_AUDIO_RGB,		// 17
	ID_EXTINPUT_ADJ_AUDIO_SCART_FULL,
	ID_EXTINPUT_ADJ_AUDIO_RCA_AV1,	// 19
	ID_EXTINPUT_ADJ_AUDIO_RCA_AV2,
	ID_EXTINPUT_ADJ_AUDIO_RCA_AV3,
	NUM_EXTINPUT_ADJ				// 22
};

/******************************************************************************
*	From now variables and functions definitions
******************************************************************************/
extern UINT32				OSA_MD_IsPortProtection(void);
extern UINT32				OSA_MD_GetDbgMode(void);
extern void					OSA_MD_SetDbgMode(UINT32 debugMode);
extern void					OSA_MD_SaveModelName(UINT8 *modelName);
extern UINT8			   *OSA_MD_GetModelName(void);
extern UINT8			   *OSA_MD_GetSerialNumber(void);
extern UINT8			   *OSA_MD_GetProjectName(void);
extern UINT8				OSA_MD_GetHardwareVersion(void);
extern UINT8				*OSA_MD_GetSWVersion(void);
extern UINT8			   *OSA_MD_GetProjectName(void);
extern BOOLEAN 				OSA_MD_IsSupportMonitorPanel(void);
extern MAIN_SOC_T 			OSA_MD_GetSOCType(void);
extern HDMI_BOARD_LINK_T	OSA_MD_GetHDMI_LinkPosition(void);
extern BOOLEAN 				OSA_MD_IsSupportGIPPanel(void);
extern Remocon_T 			OSA_MD_GetRemoconType(void);
extern BOOLEAN				OSA_MD_IsSupportNetworkSpec(void);
extern BOOLEAN 				OSA_MD_IsSupportNetCastIPTV_Service(void);
extern BOOLEAN 				OSA_MD_IsSupportOrangeIPTV_Service(void);
extern BOOLEAN 				OSA_MD_IsSupportPSU_PowerBoard(void);
extern BOOLEAN 				OSA_MD_IsSupportSkype(void);
extern BOOLEAN 				OSA_MD_IsSupportWiFi(void);
extern BOOLEAN				OSA_MD_IsSupportWiFiBuiltin(void);
extern BOOLEAN 				OSA_MD_IsSupportCameraReady(void);


#ifdef USE_BOOT
typedef struct _I2C_DEF_T{
	char		devName[30];
	UINT32	devCh;
	UINT32	devAddr;
	UINT32	devSpd;
} I2C_DEF_T;
#endif //USE_BOOT



typedef struct _tag_MODEL_CONFIG
{
#ifdef USE_BOOT
		I2C_DEF_T 				i2c[30];
#endif //USE_BOOT
//yj74.kim		BOOLEAN					bSupportB0LVDSMAP; 				///0< bSupportB0LVDSMAP
	/* Tool link Options */
	/* Tool 1  */
		INCH_TYPE_T				eModuleInchType;				///1<  Inch Type
		TOOL_TYPE_T				eToolType;						///2<  Tool Type
		MODULE_MAKER_TYPE_T		eModuleMakerType;				///3<  Module Maker
		MODULE_VERSION_TYPE_T	eModuleVersionType;				///4<  Module version
		UINT8						nLVDSBit;						///5<  nLVDSBit

	/* Tool2 */
		UINT32						nExtVideoInputIndex;			///6< �ܺ� �Է� video ADC port table index
		UINT32						nExtAudioInputIndex;			///7< �ܺ� �Է� Audio ADC port table index
		BACKLIGHT_TYPE_T			eBackLightType;					///8< Module BackLight Type
		BOOLEAN					bLocalDimMenu;					///9< Reserved_1

	/* Tool3 */
		BOOLEAN 					bSupportDIVX;					///10< DIVX Support
		BOOLEAN 					bToolDB3_reserved_I;			///11< DivX Plus Support
		BOOLEAN 					bSupportDLNA;					///12< DLNA Support
		BOOLEAN 					bSupportDigitalEye; 			///13< Digital Eye Support
		BOOLEAN 					bSupportHeadPhone;				///14< Headphone Support
		BOOLEAN 					bSupportWirelessReady;			///15< Wireless Ready --> model config ������ ������ setup 
		BOOLEAN					bLogoLight;						///15-1 < Logo light 
		BOOLEAN 					bSupportDVR_Ready;				///16< DVR_Ready
		BOOLEAN 					bSupportTHX;					///17< THX Support
		BOOLEAN 					bSupportPictureWizard;			///18< picture wizard 3 picture ���� ����
		BOOLEAN 					bSupportISF;					///19< ISF mode ���� ����
		BOOLEAN 					bSupportEPA;					///20< Energy Star ���� ǥ�� ����
		HDMI_SWITCH_TYPE_T		eHDMISwitchType;				///21<  HDMISW type �߰�
		BOOLEAN					bSupportDTCPKey;				///22< DTCP key download ���� 
		BOOLEAN					bDualView;						///23< bDualView
		BOOLEAN					bToolDB3_reserved;						///24< bIsolator

	/* Tool4  */
		BOOLEAN					bSupportLocalDimming;			///25< Local Dimming ���� ����(bSupportDimmingCtrl�ʹ� �ٸ�)
		BOOLEAN					bSupportPSUPowerBD;				///26< LIPS/PSU power board ����
		DIG_DEMOD_S_TYPE_T		eDigitalDemodType_S;			///27< DVBS digital demodulator type
		DIG_DEMOD_TYPE_T			eDigitalDemodType;				///28< DVB T2C digital demodulator type
		ANA_DEMOD_TYPE_T			eAnalogDemodType;				///29< analog demodulator type
		AMP_CHIP_TYPE_T 			eAMPChipType;					///30< AMP Chip Type

	/* Tool5 */
		BOOLEAN 					bSupportChBrowser;				///31< Channel Browser ���� ����
		MOTION_REMOCON_TYPE_T	eSupportMouseRC;				///32< ���� ������ ���� ����
		BOOLEAN 					bSupportTVLink; 				///33< Tv Link ��� ���� ����
		BOOLEAN 					bSupportMirrorMode; 			///34< ȭ�� mirror ��� ���� ����
		BOOLEAN 					bSupportOrange; 				///35< ������ IPTV ���� ���� ����
		BOOLEAN 					bSupportNetCast;				///36< netcast ���� ���� ����
		Remocon_T					eRemoconType;					///37< Remocon Type ����
		BOOLEAN 					bSupportCameraReady;			///38< BBTV Skype on/off
		BOOLEAN 					bSupportWiFi;					///39< WiFi Dongle���� ����
		BOOLEAN 					bSupportWiFiBuiltin;				///40< WiFi Builtin ���� ����
		BOOLEAN 					bSupportCNTV;					///41< China CNTV  xianghua.yin
		SUPPORT_3D_TYPE_T  		e3DModelType;					///42< 3D type (2D/3D SG/3D PG)
		LOCAL_DIM_BLOCK_TYPE_T 	bLoalDim_BLK_Type;			///43< Local Dimming Block Type (6/12 BLK or 10/16 BLK)
		BOOLEAN					bPanelErrorOut;					///44<  bPanelErrorOut    ---> PDP�� ��� Memocast�� ����. 
		BOOLEAN					bScanningBL;					///45<  bScanningBL
		BOOLEAN 					bSupport3DTHX;					///46< bSupport 3DTHX

	/* Tool6 */
		UINT32						nDefaultStdBacklight;			///47 < Standard Backlight Default value
		UINT32						nNumOfUSBInput; 				///48< USB input
		BOOLEAN					bToolDB6_reserved_I;			///49< bToolDB6_reserved_I
		BOOLEAN					bToolDB6_reserved_II;			///50< bToolDB6_reserved_II
		UINT8						nAudioPowerVoltage;				///51<  nAudioPowerVoltage
		UINT8						nSpeakerWattage;				///52<  nSpeakerWattage
		MAX_PWM_TYPE_T			eMaxPwmType;					///53 < Max PWM type

	/* Tool7 */
		BOOLEAN 					bMAC_AP;						///54
		BOOLEAN 					bESN_AP;						///55
		BOOLEAN 					bCI_AP;							///56
		BOOLEAN 					bWIDEVINE_AP;					///57
		BOOLEAN 					bMARLIN_AP;						///58
		EDID_DATA_TYPE_T			eEDID_Type;						///59
		LED_CURRENT_T				eLEDCurrent;					///59-1
		LED_BAR_TYPE_T			eLEDBarType;					///59-2 ----> PDP�� ��� Pentouch TV�� ����. 
		BOOLEAN					bDiiVa;							//59-3
		BOOLEAN					bPWMFreq;						//59-4
		UINT16						bXvYcc;							//59-5
		UINT16						bMRCU;							//59-6
		UINT16						bGesture;						//59-7

	/* Ext Input Adj */
		// video port
		UINT32					nVideoHDMIportA;				///60
		UINT32					nVideoHDMIportB;				///61
		UINT32					nVideoHDMIportC;				///62
		UINT32					nVideoHDMIportD;				///63
		UINT32					nVideoCOMPportA;				///64
		UINT32					nVideoCOMPportB;				///65
		UINT32					nVideoCOMPportC;				///66
		UINT32					nVideoRGBportA;					///67
		UINT32					nVideoScartFullportA;			///68
		UINT32 					nVideoRCAAVportA;				///69
		UINT32					nVideoRCAAVportB;				///70
		UINT32					nVideoRCAAVportC;				///71

		// audio port
		UINT32					nAudioCOMPportA;				///72
		UINT32					nAudioCOMPportB;				///73
		UINT32					nAudioCOMPportC;				///74
		UINT32					nAudioRGBportA;					///75
		UINT32					nAudioScartFullportA;			///76
		UINT32					nAudioRCAAVportA;				///77
		UINT32					nAudioRCAAVportB;				///78
		UINT32					nAudioRCAAVportC;				///79
		/* Basic options  */
		UINT32					eDebugMode;						///80< Debug mode ����.
#ifdef USE_BOOT
		char					szCountryGroupCode[30];			///81< Country Group Code ����. (default: ZZ)
#else
		char					*szCountryGroupCode;			///81< Country Group Code ����. (default: ZZ)
#endif
		char					*szOtaid;						///82< OTA ID
		DISPLAY_TYPE_T			eDispType;						///83< Display Type
		UINT32 					eDispResolution;				///84< Display Resolution Type
		LVDS_TYPE_T				eDispLvdsType;					///85< Display LVDS Type
		AMP_TYPE_T 				eAMPType;						///86< AMP Type
		UINT32					eOSDResolution;					///87< OSD Resolution Type
		FRONT_DISPLAY_TYPE_T	eFrontDispType;					///88< Front Display Type
		PIP_TYPE_T				ePIPType;						///89< PIP Type
		FRC_CHIP_TYPE_T			eFrcChipType;					///90< FRC Chip Type
		MICOM_TYPE_T			eMICOMType;						///91< Micom Type
		UINT32					eBoardVer;						///92< Board Version

		/* Input Number Options */
		UINT32					nMaxNumOfMediaPath;				///93< Number of Media Path : Normal(main only):1, PIP(main/sub):2
		UINT32					nMaxNumOfTvPath;				///94< Number of Tv Path : Is TV/TV PIP possible?
		UINT32					nMaxNumOfTVInput;				///95< Number of TV Input
		UINT32					nMaxNumOfAUTOAVInput;			///96< Number of AUTOAV Input
		UINT32					nMaxNumOfTVTuner;				///97< Number of TV Tuners

		/* Common Options */
		HDMI_SWAP_ORDER_T		nHDMIPortSwapOrder;				///98< HDMI input Port swap ����
		BOOLEAN					bSupportBlueback;				///99< Blueback Support
		BOOLEAN					bSupportNetflix;				///100< NetFlix on/off

		/* Analog only Options */
		LANG_GROUP_TYPE_T		eLangGroupType;					///101< Lang Group Type
		BOOLEAN					bSupportLangGroup;				///102< Support Lang Group

		BOOLEAN					bSupportAnaBooster; 			///103< Analog Booster On/Off ��������
		BOOLEAN					bSupportGameMode; 				///104< AV Mode(���� Game mode�� PDP only, AV Mode�� ����) ��������
		BOOLEAN					bUSBPowerOn; 					///105< USB Power On ����(USB Switch���� �ٸ��� ����)

		/* PDP Only Options	*/
		PDP_MODULE_ID_T			ePDPModuleID;					///106< PDP Module ID
		PDP_TEMP_T 				sTempThreshold;					///107< PDP model ���� Temperature Threshold�� �ٸ��� ������.
		BOOLEAN					bSupportXDPlasma;				///108< PDP XD Plasma ���� ����
		BOOLEAN					bSupportNewPowerSave; 			///109< New ������� ��������
		BOOLEAN					bSupportAutoStoreMode; 			///110< Auto Store Mode ��������

		/* LCD Only Options	*/
		BOOLEAN					bSupportDimmingCtrl;			///111< Dimming Control ���� ����
		BOOLEAN					bSupportOLED;					///112< OLED Display

		/* Picture Quality Options */
		PQL_CONFIG_T			sPqlConfig;						///113< Picture Quality Library

		/* Currently No Using Options */
		BOOLEAN 				bSupport2Hd;					///114< HD 2 Port ��������
		BOOLEAN 				bSupportSubWoofer;				///115< PDP Sub Woofer ��������


		PDP_FAN_TYPE_T			eFanType;								///116< 08�� ���ĺ��� PDP FAN ������.
		SS_CONFIG_T 			sSpreadSpecturm;						///117< EMI Test ��� ���� �� �� ���� �ݿ��ؾ� ��
		BOOLEAN					bSupportTruM;							///118< TrueM Support
		BOOLEAN					bSupportVcomPgamma;						///119< Vcom Pgamma Control ���� ����
		BOOLEAN					bSupportStartupSound;					///120> Startup Sound Support
		UINT32					nWhiteBalanceNpoint; 					///121< n point white balance
		BOOLEAN					bSupportLVDSSwap;						///122< LVDS Swap Support
		BOOLEAN					bSupportGIPPanel;						///123< GIP or Non-GIP panel use
		BOOLEAN 				bSupportMonitorPanel;					///124< Monitor Panel Support
		BOOLEAN					bSupportEDBMS;							///125< EDMBS support
		MAIN_BOARD_T			eMainBoardType; 						///126< Main PCB Type
		EVENT_BOART_T			eEventBoardType; 						///127< Event Board Type
		JACK_BOARD_T			eJackBoardType;							///128< Jack Board Type

		/* Chip depedend Options to delete PlatForm Type Compile Options */
		BOOLEAN					bSupportInternalADC; 					///129< Mstar(support), BCM(not support)
		BOOLEAN					bSupportSpreadSpectrumSpanContol; 		///130< Mstar(support), BCM(not support)
		BOOLEAN					bSupportSPIBoot;				  		///131< Mstar(support), BCM(not support)
		BOOLEAN					bSupportGpioI2C;				  		///132< Mstar(not support), BCM(support)
		BOOLEAN					bSupportLEDVersion;			  			///133< on/off ���� �Ҹ�Ȯ��.
		BOOLEAN					bSupportInternalMicomIrLock;	  		///134< ATSC Saturn(Suport), ETC (not support)
		BOOLEAN					bSupportWarmStandby;			  		///135< DVB Saturn(Support),ETC (not support)
		BOOLEAN					bSupportMiniLVDS;				  		///136< support: mini LVDS, not support: LVDS
		BOOLEAN 				bSupportHighDDRSize;			  		///137< support: 512MB, not support: 256MB
		BOOLEAN 				bSupportFHDModule;			  			///138< support: FHD, not support: HD
		UINT32					eHardwareVersion;				 		///139< SSU, NSU, USB upgrade�뵵

		/* BroadCast System Option - Check_Hardware_Option */

		BOOLEAN					bSupportMHP;							///140< bSupportMHP ��� ���� ����
		BOOLEAN 				bSupportT2;			 		 			///141< bSupportT2 ��� ���� ����
		BOOLEAN 				bSupportSatellite;			  			///142< bSupportSatellite ��� ���� ����
		BOOLEAN 				bSupportC2;			 		 			///141< bSupportC2 ��� ���� ����
		BOOLEAN 				bSupportHwOption;			  			///143< bSupportHwOption ��� ���� ����
		BOOLEAN					bSupportHomeDashBoard;					///144< bSupportHomeDashBoard ��� ���� ����

		char					*szModelSecret;							///146< model secret
		BOOLEAN					bSupportCompRCACommon;					///147< Component RCA common ��� ���� ����
		BOOLEAN					bSdpProductionMode;						///148< sdp default server
		/* Swap about port of Supporting HDMI ARC channel */
		BOOLEAN					bSupportHDMIARCSwap;					///149< bSupport HDMI ARC port swap ���� ����
		DIG_DEMOD_TYPE_T		eDigitalDemodType_T2;                   ///150< digital demod T2

		BOOLEAN					bSupport3DdepthIC;						///151< 3D Depth IC ���� ����
		BOOLEAN					bSupportDDR768MB;						///152< DDR 768MB ���� ����
		BOOLEAN					bSupportEPI;							///153< EPI ���� ����
		BOOLEAN					bSupportZoran;							///153< Zoran���� ����
		BOOLEAN					bSupportCPBOX;							///154< CP-Box ���� ����
		BOOLEAN					bSupportMouseRC;						///155< Motion Remocon ���� ����

		UINT8					nSupportCountryUI;						///156< Country UI ��� (0:UK/Ireland, 1:NordicUI, 2:Others UI)		
		BOOLEAN					bSupportSampleSet;						///157< Sample Set (TV or Box) ���� ���� 
		BOOLEAN				bSupportAppStore;							///158< Appstore 
} MODEL_DEFINE_T;
#ifdef USE_BOOT
MODEL_DEFINE_T	_gModelConfig;
#endif

/*
*******************************************************************************
*	Basic Options
*******************************************************************************
*/
extern UINT32 			OSA_MD_GetDisplayType(void);
extern UINT32 			OSA_MD_GetToolType(void);
extern BOOLEAN 			OSA_MD_IsSupportOLED(void);
extern BOOLEAN			OSA_MD_IsSupportCPBOX(void);
extern BOOLEAN			OSA_MD_IsSupportSampleSet(void);
extern BOOLEAN			OSA_MD_IsSuppor3D_Depth_IC(void);
extern BOOLEAN			OSA_MD_IsDDRSize768MB(void);
extern BOOLEAN			OSA_MD_IsSupportEPI(void);
extern BOOLEAN			OSA_MD_IsSupportZoran(void);
//extern UINT8 			OSA_MD_GetDispPanelType(void);
extern UINT8 				OSA_MD_GetModuleInchType(void);
extern UINT8 				OSA_MD_GetModuleMakerType(void);
extern UINT8 				OSA_MD_GetModuleVersionType(void);
extern UINT8 				OSA_MD_GetModuleBackLightType(void);
extern UINT8 				OSA_MD_GetUSBInputNumber(void);
extern BOOLEAN 			OSA_MD_IsSupportCompRCACommon(void);
extern BOOLEAN			OSA_MD_GetLevelOfUSBHub(void);
extern BOOLEAN 			OSA_MD_IsSupportSetID(void);
extern BOOLEAN 			OSA_MD_IsSupportMirrorMode(void);
extern BOOLEAN 			OSA_MD_IsSupportChBrowser(void);
extern BOOLEAN		 	OSA_MD_IsSupportMotionRemocon(void);
extern MOTION_REMOCON_TYPE_T OSA_MD_GetMotionRemoconType(void);
extern UINT32 			OSA_MD_GetDisplayResolution(void);
extern UINT32				OSA_MD_GetDisplayLvdsType(void);
extern UINT8 				OSA_MD_GetAmpType(void);
extern UINT8 				OSA_MD_GetAmpChipType(void);
extern UINT32 			OSA_MD_GetBoardVer(void);
extern BOOLEAN 			OSA_MD_IsSupportMiniLVDS(void);

extern UINT8 OSA_MD_GetTunerType(void);

extern UINT32 				OSA_MD_GetOsdResolution(void);
extern UINT32 				OSA_MD_GetFrontDisplayType(void);
extern UINT8 				OSA_MD_GetPipType(void);
extern UINT16				OSA_MD_GetCountryGroupCode(void);
extern char 				*OSA_MD_GetSWU_OTAID(void);
//ashton_080722 add HDMISW type
extern HDMI_SWITCH_TYPE_T OSA_MD_GetHDMISwitchType(void);
extern UINT8 				OSA_MD_GetMainBoardType(void); //balup_090528
extern UINT8 				OSA_MD_GetEventBoardType(void); //balup_090529
extern UINT8 				OSA_MD_GetJackBoardType(void); //balup_090530
extern UINT8				OSA_MD_GetMICOMType(void);
//extern BOOLEAN				OSA_MD_IsSupportJackBoard(void);
extern BOOLEAN				OSA_MD_IsSupportLocalDimming(void);
extern BOOLEAN				OSA_MD_IsSupportVcomPgamma(void);
extern BOOLEAN 				OSA_MD_IsSupportStartupSound(void);
extern BOOLEAN 				OSA_MD_IsSupportPDPROMDownload(void);
//extern BOOLEAN				OSA_MD_IsSupportChinaClearQAM(void);
extern BOOLEAN 				OSA_MD_IsCCFLPanelUsed(void);

/******************************************************************************
*	Input Number Options
******************************************************************************/
extern UINT8					OSA_MD_GetMaxMediaPathNumber(void);
extern UINT8					OSA_MD_GetMaxTvPathNumber(void);
extern UINT8 					OSA_MD_GetMaxInputNumber(INPUT_SRC_TYPE_T input);
extern UINT8					OSA_MD_GetMaxTVTunerNumber(void);
extern UINT8					OSA_MD_GetMaxUSBPort(void);
extern BOOLEAN 					OSA_MD_IsSupportHDMIARCSwap(void);
extern BOOLEAN 					OSA_MD_IsSupportUSBSwap(void);
extern BOOLEAN 					OSA_MD_IsSupportComponentSwap(void);
extern BOOLEAN 					OSA_MD_IsSupportAVSwap(void);
extern LED_CURRENT_T 			OSA_MD_GetPanel_LEDCurrent(void);
extern LED_BAR_TYPE_T			OSA_MD_GetPanel_BarType(void);
extern BOOLEAN					OSA_MD_IsSupportDiiVA(void);
extern PWM_FREQ_T	OSA_MD_GetPWMFreqType(void);
extern LOCAL_DIM_BLOCK_TYPE_T OSA_MD_GetLD_BLK_Type(void);
/******************************************************************************
*	Common Options
******************************************************************************/

//extern BOOLEAN 				OSA_MD_IsSupportSideSVHS(void);
extern BOOLEAN OSA_MD_IsSupportEMF(void);
extern BOOLEAN OSA_MD_IsBlueback(void);
extern BOOLEAN OSA_MD_IsSupportNetfilx(void);
extern BOOLEAN OSA_MD_IsSupportLangGroup(void);
extern BOOLEAN OSA_MD_IsEManual(void);
//extern BOOLEAN OSA_MD_IsEStremer(void);
//extern BOOLEAN OSA_MD_IsAutoDemo(void);
//extern BOOLEAN OSA_MD_IsSRSHD(void);
extern BOOLEAN OSA_MD_IsDigitalEye(void);
extern BOOLEAN OSA_MD_IsSupportBT(void);
extern BOOLEAN OSA_MD_IsSupportCIFS(void);
extern BOOLEAN OSA_MD_IsSupportDLNA(void);
extern UINT8 OSA_MD_IsSupportDIVX(void);
extern UINT8 OSA_MD_IsSupportDivXplus(void);
extern UINT8 OSA_MD_GetDigitalDemod(void);
extern UINT8 OSA_MD_Get_SATDigitalDemod(void);
extern UINT8 OSA_MD_GetAnalogDemodType(void);
extern BOOLEAN OSA_MD_IsSupportEPA(void);
extern BOOLEAN OSA_MD_IsSupportInternalADC_SOC(void);
extern BOOLEAN OSA_MD_IsSupportSS_SpanControl_SOC(void);
extern BOOLEAN OSA_MD_IsSupportSpiBoot_SOC(void);
extern BOOLEAN OSA_MD_IsSupportGpioI2C_SOC(void);
extern BOOLEAN OSA_MD_IsSupportLEDVersion_SOC(void);
extern BOOLEAN OSA_MD_IsSupportInternalMicomIrLock_SOC(void);
extern BOOLEAN OSA_MD_IsSupportWarmStandby_SOC(void);
extern BOOLEAN OSA_MD_IsSupportPCMEDID(void);
extern BOOLEAN OSA_MD_IsNotSupportPCMEDID(void);
extern BOOLEAN OSA_MD_IsPDPDisplay(void);
extern BOOLEAN OSA_MD_IsNordicCountry(void);
extern BOOLEAN OSA_MD_IsItalyOnlyModel(void);
extern BOOLEAN OSA_MD_IsCentralAsiaCountry(void);
extern BOOLEAN OSA_MD_IsCentralAmericaCountry(void);
extern BOOLEAN OSA_MD_IsSupportFreeviewHD(void);
extern BOOLEAN OSA_MD_IsSupportHwOption(void);
extern BOOLEAN OSA_MD_IsSupportEU_HwOption(void);
extern BOOLEAN OSA_MD_IsSupportMHP(void);
extern BOOLEAN OSA_MD_IsSupportHbbTV(void);
extern BOOLEAN OSA_MD_IsSupportT2(void);
extern BOOLEAN OSA_MD_IsSupportC2(void);
extern BOOLEAN OSA_MD_IsSupportSatellite(void);
extern BOOLEAN OSA_MD_IsSupportMultiTS(void);
extern BOOLEAN OSA_MD_IsSupport2Dto3D(void);
extern BOOLEAN OSA_MD_IsSupportCNETV(void);//xianghua.yin  20100729
#ifdef INCLUDE_2Q
#ifdef INCLUDE_HBBTV
extern BOOLEAN OSA_MD_IsSupportHbbTV(void);
#endif
#endif
extern UINT8	OSA_MD_GetEUCountryUIType(void);
extern BOOLEAN	OSA_MD_IsSupportEU_ArabicLang(void);
extern char*   OSA_MD_GetModelSecret(void);
extern BOOLEAN OSA_MD_IsSdpProductionMode(void);
extern LCD_MODULE_VER_T OSA_MD_GetLCDModuleVersion(void);
extern BOOLEAN OSA_MD_IsSupportDualView(void);
extern BOOLEAN OSA_MD_IsSupportMHP_HWPortOnly(void);
extern BOOLEAN OSA_MD_IsSupport3DSound(void);
extern BOOLEAN OSA_MD_IsSupportIsolator(void);

/******************************************************************************
*	Analog Only Options
******************************************************************************/
extern UINT8 OSA_MD_GetLangGroupType(void);
extern BOOLEAN OSA_MD_IsSupportLangGroup(void);
extern BOOLEAN OSA_MD_IsSupportOPC(void);
// stonedef - 090723
// bSupportNewECON  �׸� ������ ���Ͽ� ���ʿ���
//extern BOOLEAN OSA_MD_IsSupportNewECON(void);
extern BOOLEAN OSA_MD_IsSupportErrorOut(void);
extern BOOLEAN OSA_MD_IsSupportScanningBacklight(void);
extern BOOLEAN OSA_MD_IsSupportMemocast(void);
extern BOOLEAN OSA_MD_IsSupportHeadPhone(void);


extern BOOLEAN				OSA_MD_IsSupportAnaBooster(void);
extern BOOLEAN				OSA_MD_IsUSBPowerOn(void);
extern BREATHING_LED_T		OSA_MD_GetBreathingLEDType(void);
extern UINT8 OSA_MD_IsSupportMAC_AP(void);
extern UINT8 OSA_MD_IsSupportESN_AP(void);
extern UINT8 OSA_MD_IsSupportCI_AP(void);
extern UINT8 OSA_MD_IsSupportWIDEVINE_AP(void);
extern UINT8 OSA_MD_IsSupportMARLIN_AP(void);
extern EDID_DATA_TYPE_T OSA_MD_GetEDIDType(void);
extern PDP_PENTOUCH_TYPE_T OSA_MD_IsSupportPenTouchTV(void);
extern UINT8 OSA_MD_IsSupportDTCP_AP(void);

/******************************************************************************
*	PDP Only Options
******************************************************************************/
extern UINT8				OSA_MD_GetPDPModuleID(void);
extern void					OSA_MD_SetPDPModuleID(UINT8 moduleID);
extern void 				OSA_MD_SetXDPlasma(BOOLEAN bIsXDPlasma);		//[vivakjh] 2009/02/23  PDP XD Plasma ����.
extern BOOLEAN 				OSA_MD_IsSupportXDPlasma(void);			//[vivakjh] 2009/02/23  PDP XD Plasma ����.
extern BOOLEAN 				OSA_MD_IsSupportPDPFHD(void);				//[vivakjh] 2009/10/30
extern UINT8 				OSA_MD_GetPDPPanelType(void);
extern BOOLEAN 				OSA_MD_IsSupportEPA4dot1nEcoFlower(void);
extern void 				OSA_MD_GetPDPModuleName(UINT8 *pStrName);
extern BOOLEAN				OSA_MD_IsSupportAutoStoreMode(void);
extern PDP_TEMP_T 			OSA_MD_GetPDPTempThreshold(void);
extern BOOLEAN 				OSA_MD_IsSupportNewPowerSave(void);
extern BOOLEAN 				OSA_MD_IsSupportGameMode(void);
extern BOOLEAN 				OSA_MD_IsSupportTHX(void);
extern BOOLEAN 				OSA_MD_IsSupportNewPictureWizard(void);
extern BOOLEAN 				OSA_MD_IsSupportISF(void);
/* PDP HardWare option ���� */
extern BOOLEAN 				OSA_MD_PDPHwOption(void);

/******************************************************************************
*	LCD Only Options
******************************************************************************/
extern BOOLEAN				OSA_MD_IsSupportDimming(void);
extern BOOLEAN				OSA_MD_IsSupportDVRReady(void);
extern BOOLEAN 				OSA_MD_IsSupportBootLogo(void);
extern BOOLEAN 				OSA_MD_IsSupportInstantBoot(void);
extern BOOLEAN				OSA_MD_IsSupportWirelessReady(void);
extern BOOLEAN 				OSA_MD_IsSupportEDBMS(void);
extern UINT8  				OSA_MD_GetFRCChipType(void);
extern BOOLEAN				OSA_MD_IsSupportLEDNormal(void);
extern BOOLEAN				OSA_MD_IsLVDSSwapped(void);
extern BOOLEAN				OSA_MD_IsSupportMediaDirector(void);
extern FRC_FRAME_TYPE_T 	OSA_MD_GetFRCFrameRateType(void);//balup_090625
//yj74.kim extern BOOLEAN				OSA_MD_IsSupportB0LVDSMap(void);
extern BOOLEAN				OSA_MD_IsSupportDisplayPort(void);
extern BOOLEAN				OSA_MD_IsSupportHomeDashBoard(void);
extern BOOLEAN 				OSA_MD_IsSupportLogoLight(void);
extern BOOLEAN				OSA_MD_IsSupportDeBlur(void);
//extern BLACK_BOX_TYPE_T 	OSA_MD_GetBlackBoxType(void);

/******************************************************************************
*	Picture Quality Options
******************************************************************************/
extern PQL_CONFIG_T 		OSA_MD_GetPQLConfig(void);
extern UINT8 OSA_MD_GetWhiteBalanceNpoint(void);
extern UINT32 OSA_MD_GetPQLDBIndex(void);
extern UINT8 OSA_MD_GetDefaultStdBacklight(void);
extern void OSA_GetModelNameFromNvm(void);

/******************************************************************************
*	Currently No Using Options
******************************************************************************/
extern BOOLEAN 				OSA_MD_IsSupport2Hd(void);
extern BOOLEAN 				OSA_MD_IsSupportSubWoofer(void);
extern UINT32 				OSA_MD_GetFanType(void);
extern SS_CONFIG_T 			OSA_MD_GetSpreadSpectrum(void);
extern BOOLEAN 				OSA_MD_IsSupportTruM(void);
extern DIG_DEMOD_TYPE_T 	OSA_MD_GetDefaultDigitalDemodType(void);
extern DIG_DEMOD_TYPE_T 		OSA_MD_GetDefaultDigitalDemodSType(void);
extern ANA_DEMOD_TYPE_T 	OSA_MD_GetDefaultAnalogDemodType(void);
extern void					OSA_MD_InitDebugStatus(void);
extern void 				OSA_MD_GetDebugStatus(void);
extern UINT32 				OSA_MD_SaveToolOptionToFile (char * pPath);
extern UINT32 				OSA_MD_LoadToolOptionFromFile (char * pPath);

extern void 				OSA_MD_GetDebugStatus(void);
extern void 				OSA_MD_InitToolOption(void);
extern void 				OSA_MD_GetToolOption(void);
extern void 				OSA_MD_SetValueOfToolOption1(UINT8 index,TOOL_OPTION1_T toolOption1);
extern void 				OSA_MD_SetValueOfToolOption2(UINT8 index,TOOL_OPTION2_T toolOption2);
extern void 				OSA_MD_SetValueOfToolOption3(UINT8 index,TOOL_OPTION3_T toolOption3);
extern void 				OSA_MD_SetValueOfToolOption4(UINT8 index,TOOL_OPTION4_T toolOption4);
extern void 				OSA_MD_SetValueOfToolOption5(UINT8 index,TOOL_OPTION5_T toolOption5);
extern void 				OSA_MD_SetValueOfToolOption6(UINT8 index,TOOL_OPTION6_T toolOption6);
extern void 				OSA_MD_SetValueOfToolOption7(UINT8 index,TOOL_OPTION7_T toolOption7);
extern void 				OSA_MD_SetValueOfEuAreaOption(UINT8 index,AREA_OPTION_EU_T euAreaOption);
extern void  				OSA_MD_SetModelName(UINT8 *modelName);
extern void  				OSA_MD_SetSerialNumber(UINT8 *serialNum);
extern BOOLEAN				OSA_MD_IsInternalMicom(void);
extern SUPPORT_3D_TYPE_T    OSA_MD_Get3DSupportType(void);
extern BOOLEAN				OSA_MD_IsSupportXvYcc(void);
extern BOOLEAN				OSA_MD_IsSupportMRCU(void);
extern BOOLEAN				OSA_MD_IsSupportGesture(void);


extern UINT8 				OSA_MD_GetVideoIndex(void);
extern UINT8 				OSA_MD_GetAudioIndex(void);
extern UINT8 				OSA_MD_GetExtInput_VideoADCPort(EXT_INPUT_VIDEO_INDEX_T video_index);
extern UINT8 				OSA_MD_GetExtInput_AudioADCPort(EXT_INPUT_AUDIO_INDEX_T audio_index);
extern UINT8				OSA_MD_GetExtInput_VideoADCValue(UINT8 video_id, EXT_INPUT_VIDEO_INDEX_T video_index);
extern UINT8				OSA_MD_GetExtInput_AudioADCValue(UINT8 audio_id, EXT_INPUT_AUDIO_INDEX_T audio_index);
extern void 				OSA_MD_SetValueOfVideoExtInputADC(UINT8 index,EXT_VIDEO_INPUT_ADJ_T extInputADJ);
extern void 				OSA_MD_SetValueOfAudioExtInputADC(UINT8 index,EXT_AUDIO_INPUT_ADJ_T extInputADJ);

extern BOOLEAN OSA_MD_IsSdpProductionMode(void);

extern DISPLAY_LVDS_BIT_T 	OSA_MD_GetLVDSBits(void);
extern AUDIO_POWER_V_T 		OSA_MD_GetAudio_PwVolt(void);
extern AUDIO_SPEAKER_W_T 	OSA_MD_GetAudio_SpkWatt(void);
extern MAX_PWM_TYPE_T 	OSA_MD_GetPWMMaxType(void);
extern BOOLEAN				OSA_MD_IsSupport3DTHX(void);
extern BOOLEAN	OSA_MD_IsSupportDDRSize768MB(void);
extern BOOLEAN	OSA_MD_IsSupportLocalDimmingOSDMenu(void);
extern UINT8 OSA_MD_GetPortIndexOfHDMIARC(void);


//extern TRIDTV_DISPLAY_TYPE OSA_MD_Get3DDisplayType(void);	//youngkil.kim, remove warning.

#ifdef __cplusplus
}
#endif

#endif	/* end of _OSA_MODELDEF_H_ */
