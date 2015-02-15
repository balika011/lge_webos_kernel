/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   Permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 	global_configurations.h
 *
 *  Definitions for global configurations
 *  DESCRIPTION : In this file, all global configurations, enumerations, data
			   or structures should be shared between modules positioned in
			   different layers.
			   For example, enumeration for external video supported in the system,
			   should be referenced in User Interface module (APP layer) and,
			   at the same time, referenced Video module(Driver Layer).
			   So, these kind of definition or declaration should be stated
			   here.
 *  @author	Jackee, Lee(�����, jackee@lge.com)
 *  @author  	Lee,illo(b215@lge.com) - modifier
 *  @version 	1.1
 *  @date	2007.12.05
 *  @date	2008.04.07
 *  @see
 */

#ifndef	_GLOBAL_CONFIGS_H_
#define	_GLOBAL_CONFIGS_H_

/*-----------------------------------------------------------------------------
	���� ���
	(Control Constants)
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	#include ���ϵ�
	(File Inclusions)
------------------------------------------------------------------------------*/
#ifdef USE_BOOT
#include "typedef.h"
#endif

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef SINT8 //YKKim 100906
typedef	signed char	__SINT8;
#define SINT8 __SINT8
#endif
/*-----------------------------------------------------------------------------
	��ũ�� �Լ�  ����
	(Macros Definitions)
------------------------------------------------------------------------------*/
#define VRES_WIDTH(_v)			((_v & 0xfff) << 12   )
#define VRES_HEIGHT(_v)			((_v & 0xfff)         )
#define VRES_WIDTH_PART(_v)		((_v & 0xfff000) >> 12)
#define VRES_PROG				( 0x80000000          )	/* Progresive */
#define VRES_INTL				( 0x00000000          )	/* Interlaced */

/*-----------------------------------------------------------------------------
	��� ����
	(Constant Definitions)
------------------------------------------------------------------------------*/
//
// Global Display Resolution Constant Table
//

#define VRES_3840X2160			( VRES_WIDTH(3840) | VRES_HEIGHT(2160) ) // Ultra Definition
#define VRES_1920X1090			( VRES_WIDTH(1920) | VRES_HEIGHT(1090) ) // 081107 jguy for skylife
#define VRES_1920X1088			( VRES_WIDTH(1920) | VRES_HEIGHT(1088) ) // ATSC 1920X1088 format
#define VRES_1920X1080			( VRES_WIDTH(1920) | VRES_HEIGHT(1080) )
#define VRES_1440X1088			( VRES_WIDTH(1440) | VRES_HEIGHT(1088) )
#define VRES_1440X1080			( VRES_WIDTH(1440) | VRES_HEIGHT(1080) )
#define VRES_1440X_480			( VRES_WIDTH(1440) | VRES_HEIGHT( 480) )
#define VRES_1428X_480			( VRES_WIDTH(1428) | VRES_HEIGHT( 480) ) // for composite double-sampling
#define VRES_1418X_576			( VRES_WIDTH(1418) | VRES_HEIGHT( 576) ) //         "           "
#define VRES_1397X_576			( VRES_WIDTH(1397) | VRES_HEIGHT( 576) ) //         "           "
#define VRES_1397X_480			( VRES_WIDTH(1397) | VRES_HEIGHT( 480) ) //         "           "
#define VRES_1387X_576			( VRES_WIDTH(1387) | VRES_HEIGHT( 576) ) //         "           "
#define VRES_1387X_480			( VRES_WIDTH(1387) | VRES_HEIGHT( 480) ) //         "           "
#define VRES_1280X1088			( VRES_WIDTH(1280) | VRES_HEIGHT(1088) )
#define VRES_1280X1080			( VRES_WIDTH(1280) | VRES_HEIGHT(1080) )
#define VRES_1280X1152			( VRES_WIDTH(1280) | VRES_HEIGHT(1152) )
#define VRES_1280X_720			( VRES_WIDTH(1280) | VRES_HEIGHT( 720) )
#define VRES_960X1080			( VRES_WIDTH( 960) | VRES_HEIGHT( 1080) ) 	// AVC
#define VRES_960X720			( VRES_WIDTH( 960) | VRES_HEIGHT( 720) ) 	// AVC
#define VRES__960X_576			( VRES_WIDTH( 960) | VRES_HEIGHT( 576) )
#define VRES__768X_576			( VRES_WIDTH( 768) | VRES_HEIGHT( 576) )
#define VRES__720X1152			( VRES_WIDTH( 720) | VRES_HEIGHT(1152) )
#define VRES__720X_576			( VRES_WIDTH( 720) | VRES_HEIGHT( 576) )
#define VRES__720X_480			( VRES_WIDTH( 720) | VRES_HEIGHT( 480) )
#define VRES__720X_482			( VRES_WIDTH( 720) | VRES_HEIGHT( 482) )
#define VRES__720X_483			( VRES_WIDTH( 720) | VRES_HEIGHT( 483) ) //lachesis_080827 VG-828D No.978 483P
#define VRES__720X_240			( VRES_WIDTH( 720) | VRES_HEIGHT( 240) )
#define VRES__704X_576			( VRES_WIDTH( 704) | VRES_HEIGHT( 576) )
#define VRES__704X_480			( VRES_WIDTH( 704) | VRES_HEIGHT( 480) )
#define VRES__640X_720			( VRES_WIDTH( 640) | VRES_HEIGHT( 720) )	// AVC
#define VRES__640X_576			( VRES_WIDTH( 640) | VRES_HEIGHT( 576) )
#ifdef INCLUDE_BROADBAND
#define VRES__512X_288			( VRES_WIDTH( 512) | VRES_HEIGHT( 288) )
#endif
#define VRES__528X_576			( VRES_WIDTH( 528) | VRES_HEIGHT( 576) )
#define VRES__528X_480			( VRES_WIDTH( 528) | VRES_HEIGHT( 480) )    // totaesun 080725, for ATSC
#define VRES__544X_480			( VRES_WIDTH( 544) | VRES_HEIGHT( 480) )
#define VRES__544X_576			( VRES_WIDTH( 544) | VRES_HEIGHT( 576) )
#define VRES__480X_480			( VRES_WIDTH( 480) | VRES_HEIGHT( 480) )
#define VRES__480X_576			( VRES_WIDTH( 480) | VRES_HEIGHT( 576) )
#define VRES__352X_576			( VRES_WIDTH( 352) | VRES_HEIGHT( 576) )
#define VRES__352X_288			( VRES_WIDTH( 352) | VRES_HEIGHT( 288) )
#define VRES__352X_240			( VRES_WIDTH( 352) | VRES_HEIGHT( 240) )
#define VRES__352X_480			( VRES_WIDTH( 352) | VRES_HEIGHT( 480) )
#define VRES__320X_240			( VRES_WIDTH( 320) | VRES_HEIGHT( 240) )	// jmkim..BBCi Drip
#define VRES__640X_480			( VRES_WIDTH( 640) | VRES_HEIGHT( 480) )
#define VRES__640X_400			( VRES_WIDTH( 640) | VRES_HEIGHT( 400) )
#define VRES__640X_350			( VRES_WIDTH( 640) | VRES_HEIGHT( 350) )
#define VRES__720X_350			( VRES_WIDTH( 720) | VRES_HEIGHT( 350) )
#define VRES__720X_400			( VRES_WIDTH( 720) | VRES_HEIGHT( 400) )
#define VRES__800X_600			( VRES_WIDTH( 800) | VRES_HEIGHT( 600) )
#define VRES__832X_624			( VRES_WIDTH( 832) | VRES_HEIGHT( 624) )
#define VRES__848X_480			( VRES_WIDTH( 848) | VRES_HEIGHT( 480) )
#define VRES__852X_480			( VRES_WIDTH( 852) | VRES_HEIGHT( 480) ) // only for display out
#define VRES_1024X_768			( VRES_WIDTH(1024) | VRES_HEIGHT( 768) )
#define VRES_1024X_720			( VRES_WIDTH(1024) | VRES_HEIGHT( 720) ) // only for display out
#define VRES_1152X_864			( VRES_WIDTH(1152) | VRES_HEIGHT( 864) )
#define VRES_1152X_870			( VRES_WIDTH(1152) | VRES_HEIGHT( 870) )
#define VRES_1152X_900			( VRES_WIDTH(1152) | VRES_HEIGHT( 900) )
#define VRES_1280X_768			( VRES_WIDTH(1280) | VRES_HEIGHT( 768) )
#define VRES_1280X_960			( VRES_WIDTH(1280) | VRES_HEIGHT( 960) )
#define VRES_1280X1024			( VRES_WIDTH(1280) | VRES_HEIGHT(1024) )
#define VRES_1360X_768			( VRES_WIDTH(1360) | VRES_HEIGHT( 768) )
#define VRES_1365X_768			( VRES_WIDTH(1365) | VRES_HEIGHT( 768) ) // only for display out
#define VRES_1366X_768			( VRES_WIDTH(1366) | VRES_HEIGHT( 768) )
#define VRES_1376X_776			( VRES_WIDTH(1376) | VRES_HEIGHT( 776) ) // only for display out
#define VRES_1400X1050			( VRES_WIDTH(1400) | VRES_HEIGHT(1050) )	// 20080531 swwoo FHD Model only
#define VRES_1440X1050			( VRES_WIDTH(1440) | VRES_HEIGHT(1050) )
#define VRES_1440X_576			( VRES_WIDTH(1440) | VRES_HEIGHT( 576) )
#define VRES_1600X1200			( VRES_WIDTH(1600) | VRES_HEIGHT(1200) )
#define VRES_1920X1200			( VRES_WIDTH(1920) | VRES_HEIGHT(1200) )
#define VRES_1440X_900			( VRES_WIDTH(1440) | VRES_HEIGHT( 900) )
#define VRES_1680X1050			( VRES_WIDTH(1680) | VRES_HEIGHT(1050) )
#define VRES__960X_540			( VRES_WIDTH( 960) | VRES_HEIGHT( 540) ) // only for osd resolution

//lachesis_081201 PC format �߰�.
#define VRES__960X_600			( VRES_WIDTH( 960) | VRES_HEIGHT( 600) )
#define VRES_1088X_612			( VRES_WIDTH(1088) | VRES_HEIGHT( 612) )
#define VRES_1152X_720			( VRES_WIDTH(1152) | VRES_HEIGHT( 720) )
#define VRES_1280X_800			( VRES_WIDTH(1280) | VRES_HEIGHT( 800) )
#define VRES_1600X_900			( VRES_WIDTH(1600) | VRES_HEIGHT( 900) )
#define VRES_1600X1024			( VRES_WIDTH(1600) | VRES_HEIGHT(1024) )
#define VRES_1728X1080			( VRES_WIDTH(1728) | VRES_HEIGHT(1080) )

/* 3D restructuring by seongcheoll.kim 2011.07.01 */
/* HDMI 1.4a secondary format �߰� seongcheoll.kim 2011.06.30 */
/* HDMI 1.4a secondary format�� ���ؼ� 2D���� �����ϴ� Format�� ���ؼ��� �����ϱ�� ���� �Ǿ�
�ű� �߰��� Format ������ by seongcheoll.kim 2011.11.30 */
#if 0
#define VRES_1440x240			( VRES_WIDTH(1440) | VRES_HEIGHT(240) )
#define VRES_1440x288			( VRES_WIDTH(1440) | VRES_HEIGHT(288) )

#define VRES_2880x240			( VRES_WIDTH(2880) | VRES_HEIGHT(240) )
#define VRES_2880x288			( VRES_WIDTH(2880) | VRES_HEIGHT(288) )
#define VRES_2880x480			( VRES_WIDTH(2880) | VRES_HEIGHT(480) )
#define VRES_2880x576			( VRES_WIDTH(2880) | VRES_HEIGHT(576) )
#endif
#define VRES_1280X1470			( VRES_WIDTH(1280) | VRES_HEIGHT(1470) )
#define VRES_1920X2205			( VRES_WIDTH(1920) | VRES_HEIGHT(2205) )
#define VRES_1920X2160			( VRES_WIDTH(1920) | VRES_HEIGHT(2160) )


#define VRES_FMT_UNKNOWN		( VRES_WIDTH(0xffd)| VRES_HEIGHT(0xffd))
#define VRES_FMT_INVALID		( VRES_WIDTH(0xffe)| VRES_HEIGHT(0xffe))
#define VRES_FMT_NOSIGNAL		( VRES_WIDTH(0)    | VRES_HEIGHT(0)    )
#define VRES__END_MARK			( VRES_WIDTH(0xfff)| VRES_HEIGHT(0xfff))


#define OSD_1920x1080			( VRES_1920X1080 )
#define OSD_1366x768			( VRES_1366X_768 )
#define OSD_1024x768			( VRES_1024X_768 )
#define OSD_960x540				( VRES__960X_540 )

/* Macro moved from micom_ddi.h	*/
#define MICOM_SIZE_OF_POWER_OFF_DATA	(128)


/*-----------------------------------------------------------------------------
	�� ����
	(Type Definitions)
------------------------------------------------------------------------------*/
/**
 * Analog Audio �������߸�� current State Definition.
 *
 */
typedef enum
{
	ATV_GET_PAL_MONO				=	0x00,	// PAL Mono
	ATV_GET_PAL_STEREO				=	0x01,	// PAL Stereo
	ATV_GET_PAL_DUAL				=	0x02,	// PAL Dual
	ATV_GET_PAL_NICAM_MONO			=	0x03,	// PAL NICAM Mono
	ATV_GET_PAL_NICAM_STEREO		=	0x04,	// PAL NICAM Stereo
	ATV_GET_PAL_NICAM_DUAL			=	0x05,	// PAL NICAM Dual
	ATV_GET_PAL_UNKNOWN				=	0x06,	// PAL Unkown State
	ATV_GET_NTSC_A2_MONO			=	0x10,	// NTSC(A2) Mono
	ATV_GET_NTSC_A2_STEREO			=	0x11,	// NTSC(A2) Stereo
	ATV_GET_NTSC_A2_SAP				=	0x12,	// NTSC(A2) SAP
	ATV_GET_NTSC_A2_UNKNOWN			=	0x13,	// NTSC(A2) Unkown State
	ATV_GET_NTSC_BTSC_MONO			=	0x14,	// NTSC(BTSC) Mono
	ATV_GET_NTSC_BTSC_STEREO		=	0x15,	// NTSC(BTSC) Stereo
	ATV_GET_NTSC_BTSC_SAP_MONO		=	0x16,	// NTSC(BTSC) SAP Mono
	ATV_GET_NTSC_BTSC_SAP_STEREO	=	0x17,	// NTSC(BTSC) SAP Stereo
	ATV_GET_NTSC_BTSC_UNKNOWN		=	0x18,	// NTSC(BTSC) Unkown State
} ATV_AUDIO_MODE_GET_T;

/**
 * Analog Audio �������߸�� Setting Parameter Definition.
 *
 */
typedef enum
{
	ATV_SET_PAL_MONO				=	0x00,	// PAL Mono
	ATV_SET_PAL_MONO_FORCED			=	0x01,	// PAL Mono Force Mono
	ATV_SET_PAL_STEREO				=	0x02,	// PAL Stereo
	ATV_SET_PAL_STEREO_FORCED		=	0x03,	// PAL Stereo Force Mono
	ATV_SET_PAL_DUALI				=	0x04,	// PAL Dual I
	ATV_SET_PAL_DUALII				=	0x05,	// PAL Dual II
	ATV_SET_PAL_DUALI_II			=	0x06,	// PAL Dual I+II
	ATV_SET_PAL_NICAM_MONO			=	0x07,	// PAL NICAM Mono
	ATV_SET_PAL_NICAM_MONO_FORCED	=	0x08,	// PAL NICAM Mono Force Mono
	ATV_SET_PAL_NICAM_STEREO		=	0x09,	// PAL NICAM Stereo
	ATV_SET_PAL_NICAM_STEREO_FORCED	=	0x0A,	// PAL NICAM Stereo Force Mono
	ATV_SET_PAL_NICAM_DUALI			=	0x0B,	// PAL NICAM Dual I
	ATV_SET_PAL_NICAM_DUALII		=	0x0C,	// PAL NICAM Dual II
	ATV_SET_PAL_NICAM_DUALI_II		=	0x0D,	// PAL NICAM Dual I+II
	ATV_SET_PAL_NICAM_DUAL_FORCED	=	0x0E,	// PAL NICAM Dual Forced Mono(Not Supported)
	ATV_SET_PAL_UNKNOWN				=	0x0F,	// PAL Unkown State
	ATV_SET_NTSC_A2_MONO			=	0x10,	// NTSC(A2) Mono
	ATV_SET_NTSC_A2_STEREO			=	0x11,	// NTSC(A2) Stereo
	ATV_SET_NTSC_A2_SAP				=	0x12,	// NTSC(A2) SAP
	ATV_SET_NTSC_A2_UNKNOWN			=	0x13,	// NTSC(A2) Unkown State
	ATV_SET_NTSC_BTSC_MONO			=	0x14,	// NTSC(BTSC) Mono
	ATV_SET_NTSC_BTSC_STEREO		=	0x15,	// NTSC(BTSC) Stereo
	ATV_SET_NTSC_BTSC_SAP_MONO		=	0x16,	// NTSC(BTSC) SAP Mono
	ATV_SET_NTSC_BTSC_SAP_STEREO	=	0x17,	// NTSC(BTSC) SAP Stereo
	ATV_SET_NTSC_BTSC_UNKNOWN		=	0x18,	// NTSC(BTSC) Unkown State
} ATV_AUDIO_MODE_SET_T;

/**
 * Audio Type Definition
 *
 */
typedef  enum  AUDIO_TYPE
{
	AUDIO_SRC_TYPE_UNKNOWN = 0,	// Not Setting
	AUDIO_SRC_TYPE_PCM,
	AUDIO_SRC_TYPE_AC3,
	AUDIO_SRC_TYPE_EAC3,
	AUDIO_SRC_TYPE_MPEG,
	AUDIO_SRC_TYPE_AAC,
	AUDIO_SRC_TYPE_HEAAC,
	AUDIO_SRC_TYPE_DRA,
	AUDIO_SRC_TYPE_MP3,
	AUDIO_SRC_TYPE_DTS,
	AUDIO_SRC_TYPE_DEFAULT	// when stop audio decoder
}  AUDIO_SRC_TYPE_T ;



/**
 * FRC MC type
 *
 */
typedef enum {
	FRC_MC_TYPE_OFF = 0,
	FRC_MC_TYPE_LOW,
	FRC_MC_TYPE_HIGH,
	FRC_MC_TYPE_USER,
	FRC_MC_TYPE_55_PULLDOWN,
} FRC_MC_TYPE_T;

/**
 * Data for URSA output type
*/
typedef enum
{
    FRC_LVDS_VESA,
    FRC_LVDS_JEIDA,
} FRC_LVDS_TYPE_T;


/**
*FRC Rect
*
*/
typedef struct {
	UINT16		w;
	UINT16		h;
	UINT16		x;
	UINT16		y;
} FRC_RECT_T;

/**
*FRC bypass Region
*
*/
typedef enum {
	FRC_REGION_0	= 0,
	FRC_REGION_1,
	FRC_REGION_2,
	FRC_REGION_3,
	FRC_REGION_4,
	FRC_REGION_MAX,
} FRC_REGION_T;

/**
 * FRC output Frame rate type
 *
 */
typedef enum {
	FRC_TYPE_50_60_HZ = 0,
	FRC_TYPE_100_120_HZ,
	FRC_TYPE_200_240_HZ,
} FRC_TYPE_FRAME_RATE_T;

/**
 *FRC video mute type
 *
 */
typedef enum {
	FRC_MUTE_TYPE_INPUT	= 0,
	FRC_MUTE_TYPE_OUTPUT,
	FRC_MUTE_TYPE_LVDS,
	FRC_MUTE_TYPE_MAX
} FRC_MUTE_TYPE_T;

/**
 * Power off Mode.
 *
 * @see
 */
typedef enum
{
	/* Micom ��ü���� */
	POWER_OFF_BY_CPUCMD				= 0x00,		/**< CPU Command*/
	POWER_OFF_BY_ABN				= 0x01,		/**< */
	POWER_OFF_BY_KEYTIMEOUT			= 0x02,		/**< �����ð� ���� Power off ���� �������� ���� Power off*/
	POWER_OFF_BY_ACDET				= 0x03,		/**< AC Detec ���� �������� Power off : ���������� ���*/
	POWER_OFF_BY_RESET				= 0x04,		/**< Micom Reset�� ���� Power off */
	POWER_OFF_BY_5VMNT				= 0x05,		/**< 5V MNT�� Detec ���� �������� Power off : ���������� ���*/
	POWER_OFF_BY_NO_POLLING			= 0x06,
	POWER_OFF_BY_POWER_BD_PROTECT	= 0x07,		/**< Power B'd������ ���ǿ��� Power B'd��ȣ�� ���� Power off : ���������� ���*/
	POWER_OFF_BY_1SEC_POWER_OFF		= 0x08,		/**< Main�̻����� I2C Polling�� �߻����� ���� �� ����� Power Key�ΰ��� Power off : ���������� ���*/

	/* CPU ���� */
	POWER_OFF_BY_REMOTE_KEY 		= 0x10,		/**< ������ Power fof*/
	POWER_OFF_BY_OFF_TIMER			= 0x11,		/**< off Timer�� ���� Power off*/
	POWER_OFF_BY_SLEEP_TIMER		= 0x12,		/**< Sleep Timer�� ���� Power off*/
#ifdef INCLUDE_DVR_READY
	POWER_OFF_BY_TS_END				= 0x13,		/* TS���� �� DVR Play �����Ŀ� Power off*/
#else
	POWER_OFF_BY_ABNORMAL1			= 0x13, 		/* Abnormal case */
#endif
	POWER_OFF_BY_INSTOP_KEY 		= 0x15,		/**< Instop Key�� ���� Power off*/
	POWER_OFF_BY_AUTO_OFF			= 0x16,		/**< �ڵ� ���� ��ɿ� ���� Power off */
	POWER_OFF_BY_ON_TIMER			= 0x17,		/**< */
	POWER_OFF_BY_RS232C 			= 0x18,		/**< Rs232C�� ���� Power off*/
	POWER_OFF_BY_RESREC 			= 0x19,		/**< �����ȭ�� ���� Power off*/
	POWER_OFF_BY_RECEND 			= 0x1A,		/**< ��ȭ ���ῡ ���� Power off*/
	POWER_OFF_BY_SWDOWN 			= 0x1B,		/**< Software Downlod�� Power off */
	POWER_OFF_BY_LOCAL_KEY			= 0x1C,		/**< Gemstar Error*/
	POWER_OFF_BY_CPU_ABNORMAL		= 0x1D,		/**< CPU Abnormal ���¿� ���� Power off*/
	POWER_OFF_BY_INV_ERROR			= 0x1E,		/**< LCD Tornado Module Inverter ���� Error*/
//	POWER_OFF_BY_HOMING_COMPLETE		= 0x1E,		/**< �Ϲ��� Cable Card Update�� Power off*/
	POWER_OFF_BY_OTA				= 0x1F,		/**< OTA update ���ῡ ���� Power off*/
	POWER_OFF_BY_MICOM_RESET		= 0x40,
	POWER_OFF_BY_MEMOCAST			= 0x41,		/**< PDP MemoCast�� ���� Power off*/
	POWER_OFF_BY_MAIN_WDT			= 0x42,		/**  WDT reset by main SOC*/
	POWER_OFF_BY_VERIFY_FAIL		= 0x43,		/**  lg application���� verification fail�� system shut-down */
	POWER_OFF_BY_FAN_CONTROL		= 0x44,		/**< Fan Control�� ���� Power of*/	// changwook.joo FAN control 
	POWER_OFF_BY_SIG_DETECT			= 0x45,		/** OLED TV signal detect power */
	POWER_OFF_BY_DATA_FLASH			= 0x46,		/** Data flash init ������ �߻��� error�� ���� power off history�� ���� ����.*/
	POWER_OFF_BY_EIT_TIMER			= 0x47,		/** start : get eit data in warm standby */
	POWER_OFF_BY_EIT_TIMER_END		= 0x48,		/** end  : get eit data in warm standby */
	POWER_OFF_BY_EMM_RECEPTION		= 0x49,		/** start : get emm data in warm standby */
	POWER_OFF_BY_EMM_RECEPTION_END	= 0x4A,		/** end  : get emm data in warm standby */
	POWER_OFF_BY_UNKNOWN			= 0xFF 		/**< ��Ÿ */

}	DDI_POWER_OFF_MODE_T;


/**
 * Power off Mode.
 *
 * @see
 */
typedef enum
{
	POWER_ON_BY_REMOTE_KEY			= 0x20,		/**< Remocon�� ���� Power on*/
	POWER_ON_BY_ON_TIMER			= 0x21,		/**< On Time�� ���� Power on */
	POWER_ON_BY_LOCAL_KEY			= 0x22,		/**< Local Key�� ���� Power on*/
	POWER_ON_BY_RESERVE 			= 0x23,		/**< ���� �ð��� ���� Power on  */
	POWER_ON_BY_RS232C				= 0x24,		/**< Rs232C Command�� ���� Power on*/
	POWER_ON_BY_POWER_ONLY			= 0x25,		/**< Power Only�� ���� Power on*/
	POWER_ON_BY_REMIND 				= 0x26,		/**< ���� ��û�� ���� Power on */
	POWER_ON_BY_SWDOWN				= 0x27,		/**< Software Download�� Power on*/
	POWER_ON_BY_LAST_COLD			= 0x28,		/**< ���� COLD(standby) ���¿��� Plug off, Plug on�� ���� Power on */
	POWER_ON_BY_LAST_COOL			= 0x29,		/**< ���� COOL ���¿��� Plug off, Plug on�� ���� Power on - ������ ���� ����*/
	POWER_ON_BY_LAST_WARM			= 0x2A,		/**< ���� WARM(active standby) ���¿��� Plug off, Plug on�� ���� Power on*/
	POWER_ON_BY_LAST_POWERON		= 0x2B,		/**< ���� ON(running) ���¿��� Plug off, Plug on�� ���� Power on*/
	POWER_ON_BY_OTA					= 0x2C,		/**< OTA ���࿡ ���ؼ� Power on ��(���� POWER_ON_BY_COOL��ġ )*/
	POWER_ON_BY_CPU_ABNORMAL		= 0x2D,		/**< CPU Abnormal*/
	POWER_ON_BY_HOMING				= 0x2E,		/**< �Ϲ��� Cable Card�� ���� Power on */
	POWER_ON_BY_HDMI_CEC			= 0x2F,		/**< HDMI CEC Message "image view on"�� ���� Power on*/
	POWER_ON_BY_STORE_POWERON		= 0x30,		// <091103 leehc> ���� ���� ������ ����.
	POWER_ON_BY_MICOM_PWR_OFF_ON	= 0x31,
	POWER_ON_BY_MEMOCAST			= 0x32,		/**< MemoCast�� ���� Power on */
	POWER_ON_BY_EITTIMER			= 0x33,		/** �Ϻ��� EIT timer�� ���� power on warm stand-by */
	POWER_ON_BY_EMMTIMER			= 0x35,		/** �Ϻ��� Emm timer�� ���� power on warm stand-by */
	POWER_ON_BY_UNKNOWN 			= 0xFF		/**< ��Ÿ */

}	DDI_POWER_ON_MODE_T;


/**
 * Power State.
 *
 * @see	DDI_MICOM_GetPowerState (DDI_POWER_STATE_T *pPowerState)
 */
typedef enum
{
	POWER_STATE_COLD_STANDBY		= 0, 	/**< Micom���� ���� ����, CPU, Module�� ���� ���� �ʴ� ����  */
	POWER_STATE_COOL_STANDBY		= 1, 	/**< Cool ���� �Ϲ��� only*/
	POWER_STATE_WARM_STANDBY		= 2,	/**< Micom,CPU�� �������� , Module�� ���޵��� �ʴ� ���� (�����ȭ�� ���)*/
	POWER_STATE_ACTIVE 				= 3,	/**< �Ϲ����� Power on ���� ���� */
	POWER_STATE_BOOT				= 4,	/**< System Booting ���϶�*/
	POWER_STATE_SWDOWN				= 5,	/**< Software Download ���� */
	POWER_STATE_DEEP_SLEEP_STANDBY	= 6, 	/**< Deep Sleep���� ��ȯ */
	POWER_STATE_UNKNOWN				= 7		/**< �˼� ���� ���� */

}	DDI_POWER_STATE_T;


/**
 * Next Power On Mode.
 *
 * @see	DDI_MICOM_GetPowerState (DDI_POWER_STATE_T *pPowerState)
 */
typedef enum
{
	NEXT_POWER_ON_FROM_COLD_STANDBY			= 0, 	/**< Micom���� ���� ����, CPU, Module�� ���� ���� �ʴ� ����  */
	NEXT_POWER_ON_FROM_DEEP_SLEEP_STANDBY		= 1, 	/**< Cool ���� �Ϲ��� only*/
	NEXT_POWER_ON_FROM_STATE_UNKNOWN				= 2		/**< �˼� ���� ���� */

}	DDI_NEXT_POWER_ON_FROM_T;




typedef enum
{
	LED_CMD_CTRL_NONE 		= 0,
	LED_CMD_CTRL_COLD2HOT 	= 1,
	LED_CMD_CTRL_COLD2WARM 	= 2,
	LED_CMD_CTRL_HOT2COLD 	= 3,
	LED_CMD_CTRL_HOT2WARM 	= 4,
	LED_CMD_CTRL_DIRECT_COLD = 5,
	LED_CMD_CTRL_DEMO_MODE = 8

}	DDI_MICOM_LED_CMD_T;



/**
 * Paired structure for brightness and contrast, moved from nvm_ddi.h
 */
typedef struct
{
	UINT8	nSubBrightness;
	UINT8	nSubContrast;
} ADJUST_SUB_BC_T;

/**
 *	Calibration data structure, moved from nvm_ddi.h
 */
typedef struct
{
	UINT16	adcRedGain;
	UINT16	adcGreenGain;
	UINT16	adcBlueGain;
	UINT16	adcRedOffset;
	UINT16	adcGreenOffset;
	UINT16	adcBlueOffset;

} ADC_CALIB_DATA_T;

/*
typedef struct
{
	UINT16	adcRedOffset1;
	UINT16	adcGreenOffset1;
	UINT16	adcBlueOffset1;
	UINT16	adcRedOffset2;
	UINT16	adcGreenOffset2;
	UINT16	adcBlueOffset2;
	UINT16	adcRedGain;
	UINT16	adcGreenGain;
	UINT16	adcBlueGain;
} ADC_CALIB_DATA_T;
*/



/************************************************************************
 *	FRONT END
 ************************************************************************/


/**
 * tuner number difinition
 */
typedef enum
{
	TUNER_0			= 0,
	TUNER_1,
	TUNER_2,
	TUNER_3,
	TUNER_UNKNOWN,
	TUNER_UN_CHANGED
} TUNER_NUMBER_T;

#define	TUNER_DEFAULT	TUNER_0


/**
* transmission media.
*/
typedef enum
{
	TRANS_MEDIA_DIGITAL				= 0x00,		/*** DON'T USE THIS ENUM ***/
	TRANS_MEDIA_D_TERRESTRIAL,
	TRANS_MEDIA_D_CABLE,
	TRANS_MEDIA_D_SATELLITE,
	TRANS_MEDIA_D_SATELLITE_JPBS,
	TRANS_MEDIA_D_SATELLITE_JPCS,

	TRANS_MEDIA_ANALOG				= 0x10,		/*** DON'T USE THIS ENUM ***/
	TRANS_MEDIA_A_TERRESTRIAL,
	TRANS_MEDIA_A_CABLE,
	TRANS_MEDIA_A_SATELLITE,

	TRANS_MEDIA_NONE				= 0x20,

	TRANS_MEDIA_UNKNOWN				= 0x80,

	TRANS_MEDIA_MASK				= 0xF0		/*** DON'T USE THIS ENUM ***/
} TRANS_MEDIA_T;

#define	IS_TRANS_MEDIA_DT(transMedia)		(TRANS_MEDIA_D_TERRESTRIAL	== transMedia)
#define	IS_TRANS_MEDIA_DC(transMedia)		(TRANS_MEDIA_D_CABLE		== transMedia)
#define	IS_TRANS_MEDIA_DS(transMedia)		((TRANS_MEDIA_D_SATELLITE	== transMedia) || (TRANS_MEDIA_D_SATELLITE_JPBS == transMedia) || (TRANS_MEDIA_D_SATELLITE_JPCS == transMedia))

#define	IS_TRANS_MEDIA_AT(transMedia)		(TRANS_MEDIA_A_TERRESTRIAL	== transMedia)
#define	IS_TRANS_MEDIA_AC(transMedia)		(TRANS_MEDIA_A_CABLE		== transMedia)
#define	IS_TRANS_MEDIA_AS(transMedia)		(TRANS_MEDIA_A_SATELLITE	== transMedia)

#define	IS_TRANS_MEDIA_TERRE(transMedia)	((TRANS_MEDIA_D_TERRESTRIAL	== transMedia) || (TRANS_MEDIA_A_TERRESTRIAL	== transMedia))
#define	IS_TRANS_MEDIA_CABLE(transMedia)	((TRANS_MEDIA_D_CABLE		== transMedia) || (TRANS_MEDIA_A_CABLE			== transMedia))
#define	IS_TRANS_MEDIA_SATEL(transMedia)	((TRANS_MEDIA_D_SATELLITE	== transMedia) || (TRANS_MEDIA_D_SATELLITE_JPBS == transMedia) || (TRANS_MEDIA_D_SATELLITE_JPCS == transMedia) || (TRANS_MEDIA_A_SATELLITE		== transMedia))

#define	IS_TRANS_MEDIA_DIGITAL(transMedia)	(TRANS_MEDIA_DIGITAL		== (transMedia & TRANS_MEDIA_MASK))
#define	IS_TRANS_MEDIA_ANALOG(transMedia)	(TRANS_MEDIA_ANALOG			== (transMedia & TRANS_MEDIA_MASK))

/**
 * TPS : constellation.
 */
typedef enum 	/* 4 bit */
{
	TU_TPS_CONST_QPSK 		= 0x00,
	TU_TPS_CONST_DQPSK,
	TU_TPS_CONST_QAM_4NR,
	TU_TPS_CONST_QAM_4,
	TU_TPS_CONST_PSK_8,
	TU_TPS_CONST_VSB_8,
	TU_TPS_CONST_QAM_16,
	TU_TPS_CONST_QAM_32,
	TU_TPS_CONST_QAM_64,
	TU_TPS_CONST_QAM_128,
	TU_TPS_CONST_QAM_256,

	TU_TPS_CONST_END,

	TU_TPS_CONST_UNKNOWN 	= 0x0F
} TU_TPS_CONSTELLATION_T;

/**
 * TPS : code rate.
 */
typedef enum 	/* 4 bit */
{
	TU_TPS_CODE_1_2 		= 0x00,
	TU_TPS_CODE_2_3,
	TU_TPS_CODE_3_4,
	TU_TPS_CODE_2_5,
	TU_TPS_CODE_3_5,
	TU_TPS_CODE_4_5,
	TU_TPS_CODE_5_6,
	TU_TPS_CODE_6_7,
	TU_TPS_CODE_7_8,
	TU_TPS_CODE_8_9,
	TU_TPS_CODE_9_10,

	TU_TPS_CODE_END,

	TU_TPS_CODE_UNKNOWN 	= 0x0F
} TU_TPS_CODERATE_T;

/**
 * TPS : guard interval.
 */
typedef enum 	/* 4 bit */
{
	TU_TPS_GUARD_1_4 		= 0x00,
	TU_TPS_GUARD_1_8,
	TU_TPS_GUARD_1_9,
	TU_TPS_GUARD_1_16 ,
	TU_TPS_GUARD_1_32,
	TU_TPS_GUARD_1_128,
	TU_TPS_GUARD_19_128,
	TU_TPS_GUARD_19_256,

	TU_TPS_GUARD_420_C,
	TU_TPS_GUARD_420_V,
	TU_TPS_GUARD_595,
	TU_TPS_GUARD_945_C,
	TU_TPS_GUARD_945_V,

	TU_TPS_GUARD_END,

	TU_TPS_GUARD_UNKNOWN 	= 0x0F
} TU_TPS_GUARD_INTERVAL_T;

/**
 * TPS : carrier mode.
 */
typedef enum 	/* 4 bit */
{
	TU_TPS_CARR_1K 			= 0x00,
	TU_TPS_CARR_2K,
	TU_TPS_CARR_4K,
	TU_TPS_CARR_8K,
	TU_TPS_CARR_16K,
	TU_TPS_CARR_32K,

	TU_TPS_CARR_SC,
	TU_TPS_CARR_MC,

	TU_TPS_CARR_END,

	TU_TPS_CARR_UNKNOWN 	= 0x0F
} TU_TPS_CARRIER_MODE_T;

/**
 * TPS : hierarchy.
 */
typedef enum 	/* 3 bit */
{
	TU_TPS_HIERA_NONE 		= 0x00,
	TU_TPS_HIERA_1,
	TU_TPS_HIERA_2,
	TU_TPS_HIERA_4,

	TU_TPS_HIERA_END,

	TU_TPS_HIERA_UNKNOWN 	= 0x07
} TU_TPS_HIERARCHY_T;

/**
 * polarization
 */
typedef enum
{
	TU_TPS_POLARIZATION_HORIZONTAL	= 0X00,
	TU_TPS_POLARIZATION_VERTICAL,

	TU_TPS_POLARIZATION_LEFT,
	TU_TPS_POLARIZATION_RIGHT,

	TU_TPS_POLARIZATION_MAX,

	TU_TPS_POLARIZATION_ALL
}TU_TPS_POLARIZATION_T;

/************************************************************************
 *	End of FRONT END
 ************************************************************************/

/**
 * Analog Audio Sound System Type Definition
 */
typedef enum
{
	AUDIO_SIF_SYSTEM_BG		= 0x00,	// keep the value. 	refer to IF_PLL_DATA_TYPE_t
	AUDIO_SIF_SYSTEM_I,				// keep the value. 	refer to IF_PLL_DATA_TYPE_t
	AUDIO_SIF_SYSTEM_DK,			// keep the value. 	refer to IF_PLL_DATA_TYPE_t
	AUDIO_SIF_SYSTEM_L,				// keep the value. 	refer to IF_PLL_DATA_TYPE_t
	AUDIO_SIF_SYSTEM_MN,			// keep the value.	refer to IF_PLL_DATA_TYPE_t
	AUDIO_SIF_SYSTEM_LP,			/* 20080724 : Added by Goldman for FE/TU */
	AUDIO_SIF_SYSTEM_END,			/* 20080724 : Added by Goldman for FE/TU */
 	AUDIO_SIF_SYSTEM_UNKNOWN = 0xf0
} AUDIO_SIF_SOUNDSYSTEM_T;


/************************************************************************
 *	3DTV
 ************************************************************************/

/**
 * Define 3D Mode Flag mode.
 */
typedef enum
{
	TRIDTV_3DMODE_ON = 0,
	TRIDTV_3DMODE_OFF,
	TRIDTV_3DMODE_2DTO3D,
	TRIDTV_3DMODE_3DTO2D,
	TRIDTV_3DMODE_MAX,
} TRIDTV_3DMODE_FLAG_T;
// TODO: ***����*** (UI) ui_svc_tridtv.h  (MW)  video_api.h (DRIVE) global_configurations.h enum�� �����ؾ� ��.
/**
 * Define 3D Input Type.
 */
typedef enum
{
	/*	Drive���� �Ʒ� value ��� ����.	*/
	INPUT_TYPE_3DOFF = 0,			//VIDEO_3D_FORMAT_2D,

	//interim format - half
	INPUT_TYPE_TOPNBOTTOM,		//VIDEO_3D_FORMAT_TOP_BOTTOM,
	INPUT_TYPE_SIDEBYSIDE,		//VIDEO_3D_FORMAT_SIDEBYSIDE_HALF,
	INPUT_TYPE_CHECKERBOARD,	//VIDEO_3D_FORMAT_CHECKER_BOARD,
	INPUT_TYPE_FRAMESEQ,		//VIDEO_3D_FORMAT_FRAME_SEQ
	INPUT_TYPE_COLUMN_INTERLEAVE,	/**< for H.264*/
	INPUT_TYPE_LINE_INTERLEAVE_HALF,	/**< for H.264*/

	//Full format
	INPUT_TYPE_FRAMEPAC,			//VIDEO_3D_FORMAT_FRAME_PACKING
	INPUT_TYPE_FIELD_ALTERNATIVE,	//VIDEO_3D_FORMAT_FIELD_ALTERNATIVE
	INPUT_TYPE_LINE_ALTERNATIVE,	//VIDEO_3D_FORMAT_LINE_ALTERNATIVE
	INPUT_TYPE_SIDEBYSIDE_FULL,		//VIDEO_3D_FORMAT_SIDEBYSIDE_FULL,
	INPUT_TYPE_DUALSTREAM, 	/**< Full format*/

	/*	Drive���� �Ʒ� value ��� ����. */
	INPUT_TYPE_2DTO3D,			//VIDEO_3D_FORMAT_2DTO3D,

	INPUT_TYPE_MAX,
} TRIDTV_INPUT_TYPE_T;

/**
 * Define 3D Input LR Type.
 */
typedef enum
{
	INPUT_RL_SEQ = 0,
	INPUT_LR_SEQ,
	INPUT_LR_MAX,
} TRIDTV_INPUT_LR_SEQ_T;
// TODO: (YKKIM) Main SOC ��� FREQ TYPE���� (= FRC/FORMATTER INPUT FREQ����)
/**
 * Define 3D Input Freq. (MAIN SOC OUTPUT FREQ TYPE= FORMATTER INPUT FREQ TYPE)
 */

typedef enum
{
	//scaler output frame rate.
	INPUT_FREQ_24HZ = 0,
	INPUT_FREQ_30HZ,
	INPUT_FREQ_48HZ,
	INPUT_FREQ_50HZ,
	INPUT_FREQ_60HZ,

	INPUT_FREQ_MAX,

} TRIDTV_INPUT_FREQ_T;
/**
 * Define 3D Output Type.
 */
typedef enum
{
	OUTPUT_TYPE_DUAL = 0,
	OUTPUT_TYPE_INTERIM,
	OUTPUT_TYPE_FRAMESEQ,
	OUTPUT_TYPE_FRAMESEQ_LRLR, //60Hz/120Hz/240Hz output
	OUTPUT_TYPE_FRAMESEQ_LLRR, //120Hz/240Hz output
	OUTPUT_TYPE_FRAMESEQ_LBRB, //240Hz
	OUTPUT_TYPE_FRAMESEQ_LURU, //240Hz
	OUTPUT_TYPE_LINEINTERLEAVED,
	OUTPUT_TYPE_MAX,
} TRIDTV_OUTPUT_TYPE_T;

/**
 * Define 3D Output LR Type.
 */
typedef enum
{
	OUTPUT_RL_SEQ = 0,
	OUTPUT_LR_SEQ,
	OUTPUT_LR_MAX,
} TRIDTV_OUTPUT_LR_SEQ_T;

/**
 * Define 3D Output Freq.
 */
typedef enum
{
	OUTPUT_FREQ_60HZ = 0,
	OUTPUT_FREQ_100HZ,
	OUTPUT_FREQ_120HZ,
	OUTPUT_FREQ_240HZ,
	OUTPUT_FREQ_50HZ,
	OUTPUT_FREQ_BYPASS,
	OUTPUT_FREQ_MAX,
} TRIDTV_OUTPUT_FREQ_T;

// TODO: ���� (UI) ui_svc_tridtv.h  (MW)  video_api.h (DRIVE) global_configurations.h enum�� �����ؾ� ��.
/**
 * Define 3D Input Config Info. *
 * Description: UI�� ���� ���޵Ǵ� �� �߰�.
 */
typedef struct TRIDTV_INPUT_CONFIG_INFO
{
	TRIDTV_3DMODE_FLAG_T	n3DInputMode;	// 0: 3D ON, 1:3D OFF, 2:2DTO3D, 3:3DTO2D
	TRIDTV_INPUT_TYPE_T 	n3DInputType;	// 1:TNB 2:SBS, 3:
	TRIDTV_INPUT_LR_SEQ_T	n3DInputLRSeq;	// 0:LR, 1:RL
	TRIDTV_INPUT_FREQ_T 	n3DInputFreq;	// NOT USED.

	UINT8					n3DInputDepthCtrl;		// 0~10
	SINT8					n3DInputConvertCtrl;	//-10~10
	UINT16					nVerticalSize;			//
	BOOLEAN					bPositiveDepth;	/* TRUE: Positive depth, FALSE: Negative depth */	
	BOOLEAN					bAutoConvMode;	/* TRUE: 3D auto convergence, FALSE: 3D manual convergence */
} TRIDTV_INPUT_CONFIG_INFO_T;

/**
 * Define 3D Output Config Info.
 */
typedef struct TRIDTV_OUTPUT_CONFIG_INFO
{
	TRIDTV_OUTPUT_TYPE_T	n3DOutputType;
	TRIDTV_OUTPUT_LR_SEQ_T	n3DOutputLRSeq;
	TRIDTV_OUTPUT_FREQ_T	n3DOutputFreq;
} TRIDTV_OUTPUT_CONFIG_INFO_T;

/**
 * Define 3D All of Input/Output  Config Info.
 */
typedef struct TRIDTV_CONFIG_INFO
{
	//TRIDTV_3DMODE_FLAG_T	n3DModeStatus;
	TRIDTV_INPUT_CONFIG_INFO_T	st3DInputCfgInfo;
	TRIDTV_OUTPUT_CONFIG_INFO_T st3DOutputCfgInfo;
} TRIDTV_CONFIG_INFO_T;

/**
 * Define 3D Output LR Type.
 */
typedef enum
{
	SYNC_NONE = 0,
	SYNC_GOOD,
	SYNC_BAD,
	SYNC_MAX,
} TRIDTV_INPUT_SYNCSTATUS_T;

/**
 * Define 3D Cfg items.
 */
typedef enum
{
	TRIDTV_3DMODE_STATUS = 0,
	TRIDTV_3D_INPUT_TYPE,
	TRIDTV_3D_INPUT_LR_SEQ,
	TRIDTV_3D_INPUT_FREQ,
	TRIDTV_3D_OUTPUT_TYPE,
	TRIDTV_3D_OUTPUT_LR_SEQ,
	TRIDTV_3D_OUTPUT_FREQ,
	TRIDTV_CFG_ITEMS_MAX,
} TRIDTV_CFG_ITEMS_T;


/** -->��� ����. VIDEO API������ <--
 * Define 3D HDMI VSI Info.
 *
 */
typedef struct TRIDTV_HDMI_VSI_INFO
{
	UINT8	uHDMI_VSI_Video_Format;
	UINT8	uHDMI_VSI_3D_Structure;
	UINT8	uHDMI_VSI_3D_Ext_Data;
	UINT8	uHDMI_VSI_VIC;
} TRIDTV_HDMI_VSI_INFO_T;

#define MIN_TOUCHSENSE	40
#define MAX_TOUCHSENSE	225
#define DEFAULT_TOUCHSENSE	60

#define MAX_WB_IRE_STEP         21

/*-----------------------------------------------------------------------------
	�ܺ� ���������� �Լ� prototype ����
	(Extern Variables & Function Prototype Declarations)
------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _GLOBAL_CONFIGS_H_ */

