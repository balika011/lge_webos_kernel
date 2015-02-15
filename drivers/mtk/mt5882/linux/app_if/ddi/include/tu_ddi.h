/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2008 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file tu_ddi.h
 *
 *  TU Module DDI header file.
 *
 *  @author		Goldman(jhgold@lge.com)
 *  @version	1.0
 *  @date		2008.04.14
 *  @note
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _TU_DDI_H_
#define	_TU_DDI_H_

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
	Control Constants
******************************************************************************/


/******************************************************************************
	File Inclusions
******************************************************************************/

#include "branches.h"
#include "common.h"
#include "country.h"
#include "global_configurations.h"
#include "audio_ddi.h"

/******************************************************************************
 	Constant Definitions
******************************************************************************/

#define	TU_NO_OF_TUNERS			4

#define	TU_TPS_INVALID_CELL_ID	0

#define	TU_INVAL_SPDATA32	0xFFFFFFFF

/**
 * channel bandwidth.
 */
typedef UINT16	TU_CH_BW_KHZ_U16;
#define	TU_CH_BW_ALL   		0
#define	TU_CH_BW_5350KHZ	5350
#define	TU_CH_BW_5500KHZ	5500
#define	TU_CH_BW_6000KHZ	6000
#define	TU_CH_BW_7000KHZ	7000
#define	TU_CH_BW_8000KHZ	8000
#define	TU_CH_BW_UNKNOWN	0xffff

/**
 * TPS : Symbol rate.
 */
typedef UINT16	TU_SYMBOL_RATE_KHZ_U16;

#define	TU_SYMBOL_RATE_5056_KHZ	5056
#define	TU_SYMBOL_RATE_5309_KHZ	5309
#define	TU_SYMBOL_RATE_5360_KHZ	5360
#define	TU_SYMBOL_RATE_5957_KHZ	5957
#define	TU_SYMBOL_RATE_6000_KHZ	6000
#define	TU_SYMBOL_RATE_6111_KHZ	6111
#define	TU_SYMBOL_RATE_6125_KHZ	6125
#define	TU_SYMBOL_RATE_6875_KHZ	6875
#define	TU_SYMBOL_RATE_6900_KHZ	6900
#define	TU_SYMBOL_RATE_6950_KHZ	6950
#define	TU_SYMBOL_RATE_6957_KHZ	6957 /* = 8/115 * 100, DVB-C Annec A Max Symbol Rate */
#define	TU_SYMBOL_RATE_UNKNOWN	0xFFFF


/******************************************************************************
	Macro Definitions
******************************************************************************/
//#ifdef INCLUDE_SUPPORT_DVBSS2 
#define TU_SCIF_MAX_UB 			20
//#endif /* INCLUDE_SUPPORT_DVBSS2 */

#define	TU_OSA_PRINT		OSA_PRINT
#define	TU_CURRENT_TIME()	OSA_ReadMsTicks()

/******************************************************************************
	Type Definitions
******************************************************************************/

/**
 * I2C channel.
*/
typedef UINT8 	TU_I2C_CH_INDEX_T;


/**
 * country specific configuration
*/
typedef struct
{
	BOOLEAN 	bValid;

	/* DT : Digital Terrestrial */
	BOOLEAN 	bDT_Bandwidth_Multi;
	BOOLEAN 	bDT_Bandwidth_8MHz;
	BOOLEAN 	bDT_Bandwidth_7MHz;
	BOOLEAN 	bDT_Bandwidth_6MHz;

	/* DC : Digital Cable */
//	BOOLEAN 	bDC_FullScan;

	/* AA : AT + AC */
	BOOLEAN 	bAA_FullScan;
} TU_COUNTRY_CONFIG_T;

/**
 * Tuner Hardward configuration
*/
typedef struct
{
	BOOLEAN		bValid;
	BOOLEAN		bSerialSupport;		//Support serial TS output.
	BOOLEAN		bSerialDefault;		//Serial TS output is default.

	BOOLEAN		bBoostDisplayDTV;	//Display booster menu at UI
	BOOLEAN		bBoostSupportDTV;	//Support booster for DTV
	BOOLEAN		bBoostDefaultDTV;	//Default booster for DTV
	BOOLEAN		bBoostAutoDTV;		//Auto booster for DTV		Used in FE

	BOOLEAN		bBoostDisplayATV;	//Display booster menu at UI
	BOOLEAN		bBoostSupportATV;	//Support booster for ATV
	BOOLEAN		bBoostDefaultATV;	//Default booster for ATV
	BOOLEAN		bBoostAutoATV;		//Auto booster for ATV		Used in TU

	BOOLEAN		bAntenna5V;			//Support antenna power 5V
	BOOLEAN		bManualFineTune;	//Support manual fine tune
	BOOLEAN		bRFPortSwitch;		//Cable/Air RF input ports
	BOOLEAN		bAudioStandard;		//Whether to check audio standard or not

//	BOOLEAN		bDVBCAutoDetect; 	//Disable from GP4
	BOOLEAN		bDemodOnOffControl;

	BOOLEAN		bSumdCarrierThreshold;	//Sumod Setting for analog CR unlock threshold
	BOOLEAN		bSumdPhaseOption;

	BOOLEAN		bSupportDVBT2;
	BOOLEAN		bSupportSatellite;

	BOOLEAN		bSumdAGCSpeed;
	BOOLEAN		bSumdOverModulation;	
} TU_HW_CONFIG_T;

/**
 * Tuner Port.
 */
typedef enum
{
	TU_PORT_0		= 0,
	TU_PORT_1,

	TU_PORT_UNKNOWN	= 0x80
} TU_PORT_T;


/**
 * Lock state.
 */
typedef enum
{
	TU_LOCK_OK			= 0x00,
	TU_LOCK_FAIL,
	TU_LOCK_UNSTABLE,

	TU_LOCK_WEAK		= 0x10,
	TU_LOCK_POOR,
	TU_LOCK_ATV_PROGRESS,		/* used in auto search */

	TU_LOCK_UNKNOWN 	= 0x80
} TU_LOCK_STATE_T;


/**
 * transmission system.
 */
typedef enum
{
	/* TERRESTRIAL */
	TU_TRANS_SYS_VSB		= 0x00,			/* 5 bit : Don't exceed */
	TU_TRANS_SYS_DVBT,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_DVBT2,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_DTMB,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_ISDBT,						/* 5 bit : Don't exceed */

	/* CABLE */
	TU_TRANS_SYS_DVBC,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_DVBC2,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_QAM,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_ISDBC,						/* 5 bit : Don't exceed */

	/* SATELLITE */
	TU_TRANS_SYS_DVBS,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_DVBS2,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_BS,							/* 4 bit : Don't exceed */
	TU_TRANS_SYS_CS,						/* 5 bit : Don't exceed */

	/* ANALOG */
	TU_TRANS_SYS_NTSC,						/* 5 bit : Don't exceed */
	TU_TRANS_SYS_PAL,						/* 5 bit : Don't exceed */

	TU_TRANS_SYS_END,						/* 5 bit : Don't exceed */

	TU_TRANS_SYS_UNKNOWN	= 0x1F,			/* 5 bit : Don't exceed */
} TU_TRANS_SYSTEM_T;

#define TU_IS_VALID_TRANS_SYSTEM(transSystem)	(transSystem < TU_TRANS_SYS_END ? TRUE : FALSE)

#define	IS_TRANS_SYS_DVBTx(transSystem)		((TU_TRANS_SYS_DVBT	== transSystem) || (TU_TRANS_SYS_DVBT2	== transSystem))
#define	IS_TRANS_SYS_DVBCx(transSystem)		((TU_TRANS_SYS_DVBC	== transSystem) || (TU_TRANS_SYS_DVBC2	== transSystem))
#define	IS_TRANS_SYS_DVBSx(transSystem)		((TU_TRANS_SYS_DVBS	== transSystem) || (TU_TRANS_SYS_DVBS2	== transSystem))


/**
 * tune mode.
 */
typedef enum
{
	/*	---------- NORMAL 	---------------------	*/
	TU_TUNE_NORMAL						= 0x10,		/* 0001 XXXX */
		TU_TUNE_NOR_CHANNEL,
		TU_TUNE_NOR_FREQUENCY,
		TU_TUNE_NOR_FINE,				/* ATV : No AFT */

	/*	---------- SCAN 	---------------------	*/
	TU_TUNE_MANUAL						= 0x20,		/* 0010 XXXX */
		TU_TUNE_MAN_CHANNEL,
		TU_TUNE_MAN_FREQUENCY,
		TU_TUNE_MAN_SYSTEM,				/* ATV : PAL */
		TU_TUNE_MAN_FINE_UP,			/* ATV : PAL */
		TU_TUNE_MAN_FINE_DN,			/* ATV : PAL */
		TU_TUNE_MAN_SEARCH_UP,			/* ATV : PAL */
		TU_TUNE_MAN_SEARCH_DN,			/* ATV : PAL */

	TU_TUNE_AUTO						= 0x30,		/* 0011 XXXX */
		TU_TUNE_AUTO_FULL,
		TU_TUNE_AUTO_CHANNEL,
		TU_TUNE_AUTO_FREQUENCY,
		TU_TUNE_AUTO_START,				/* ATV : Full Scan */
		TU_TUNE_AUTO_CONTINUE,			/* ATV : Full Scan */

	/*	---------- SPECIFIC 	---------------------	*/
	TU_TUNE_SPECIFIC					= 0x40,		/* 0100 XXXX */
		TU_TUNE_SPEC_DVBT_HMLP,			/* DVBT : Hierarchy Mode */
		TU_TUNE_SPEC_DVBC_FIXED_DATA,	/* DVBC : Use Fixed NIT data */

	/*	---------- OTHERS 	---------------------	*/
	TU_TUNE_STOP						= 0x50, 	/* 1001 XXXX */
	TU_TUNE_FINISH,								/* Don't use this */

	/*	---------- UNKNOWN 	---------------------	*/
	TU_TUNE_UNKNOWN						= 0x80,		/* 1000 XXXX */

	/*	---------- MASK 	---------------------	*/
	TU_TUNE_MODE_MASK					= 0xF0,		/* 1111 XXXX */
} TU_TUNE_MODE_T;

#define		TU_IS_TUNE_NORMAL(tuneMode)	(TU_TUNE_NORMAL	== (tuneMode & TU_TUNE_MODE_MASK))
#define		TU_IS_TUNE_MANUAL(tuneMode)	(TU_TUNE_MANUAL	== (tuneMode & TU_TUNE_MODE_MASK))
#define		TU_IS_TUNE_AUTO(tuneMode)	(TU_TUNE_AUTO 	== (tuneMode & TU_TUNE_MODE_MASK))
#define		TU_IS_TUNE_STOP(tuneMode)	(TU_TUNE_STOP 	== (tuneMode & TU_TUNE_MODE_MASK))

#define		TU_IS_TUNE_SCAN(tuneMode)	(TU_IS_TUNE_AUTO(tuneMode)||(TU_TUNE_MAN_SEARCH_UP == tuneMode)||\
										(TU_TUNE_MAN_SEARCH_DN == tuneMode ))


/*
**	=======================================================
**	======================== D T V ========================
**	=======================================================
*/


/**
 * signal state.
 */
typedef struct
{
	BOOLEAN bSignalValid;		/*	1 bit */
	UINT8	strength;
	UINT8	quality;

	SINT16	powerLevel;
	UINT32	packetError;
	UINT32	unBER;			/* unit : e-10 */
	UINT32	unAGC;
	UINT32	unSNR;
} TU_SIGNAL_STATE_T;


/*
**	=======================================================
**	======================= COUNTRY =======================
**	=======================================================
*/

/**
 * Special data : DVB-T.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem		: 5;		/* 5 bit */	/* Keep the position and size */

	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	TU_CHANNEL_BW_T			eChannelBW		: 2;		/* 2 bit */	/* Keep the position and size */

	BOOLEAN					bSpectrumInv	: 1;		/* 1 bit */
	BOOLEAN					bDVBT2ScanOn	: 1;		/* 1 bit */

	UINT32									: 1;		/* Reserved */

	BOOLEAN					bProfileHP		: 1;		/* 1 bit */
	TU_TPS_HIERARCHY_T		hierarchy		: 3;		/* 3 bit */

	TU_TPS_CARRIER_MODE_T	carrierMode		: 4;		/* 4 bit */
	TU_TPS_GUARD_INTERVAL_T guardInterval	: 4;		/* 4 bit */

	TU_TPS_CODERATE_T		codeRate		: 4;		/* 4 bit */
	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_DVBT_T;

/**
 * Special data : DVB-T2.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 5;		/* 5 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */
	TU_CHANNEL_BW_T			eChannelBW		: 2;		/* 2 bit */	/* Keep the position and size */

	UINT32									: 3;		/* Reserved */

	UINT8					groupID			: 4;		/* 4 bit */	

	TU_TPS_CARRIER_MODE_T	carrierMode		: 4;		/* 4 bit */
	TU_TPS_GUARD_INTERVAL_T guardInterval	: 4;		/* 4 bit */

	TU_TPS_CODERATE_T		codeRate		: 4;		/* 4 bit */
	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_DVBT2_T;

/**
 * Special data : VSB.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	BOOLEAN					bCoChannel		: 1;		/* 1 bit */

	UINT32									: 21;		/* Reserved */

	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_VSB_T;

/**
 * Special data : DTMB.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	BOOLEAN					bRayleigh		: 1;		/* 1 bit */
	BOOLEAN					bM720			: 1;		/* 1 bit */

	UINT32									: 8;		/* Reserved */

	TU_TPS_CARRIER_MODE_T	carrierMode		: 4;		/* 4 bit */
	TU_TPS_GUARD_INTERVAL_T guardInterval	: 4;		/* 4 bit */

	TU_TPS_CODERATE_T		codeRate		: 4;		/* 4 bit */
	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_DTMB_T;

/**
 * TPS : Segment.
 */
typedef enum 	/* 3 bit */
{
	TU_TPS_SEG_FULL 		= 0x00,
	TU_TPS_SEG_1,
	TU_TPS_SEG_3,

	TU_TPS_SEG_END,

	TU_TPS_SEG_UNKNOWN 		= 0x07
} TU_TPS_SEGMENT_T;

/**
 * Special data : ISDBT.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	BOOLEAN					bPartialRcv		: 1;		/* 1 bit */
	BOOLEAN					bEmergency		: 1;		/* 1 bit */

	UINT32									: 5;		/* Reserved */

	TU_TPS_SEGMENT_T		segment			: 3;		/* 3 bit */

	TU_TPS_CARRIER_MODE_T	carrierMode		: 4;		/* 4 bit */
	TU_TPS_GUARD_INTERVAL_T guardInterval	: 4;		/* 4 bit */

	TU_TPS_CODERATE_T		codeRate		: 4;		/* 4 bit */
	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_ISDBT_T;

/**
 * Special data : DVB-C.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 5;		/* 5 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */

	UINT32									: 4;		/* Reserved */

	BOOLEAN					bSpectrumInv	: 1;		/* 1 bit */

	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */

	TU_SYMBOL_RATE_KHZ_U16	symbolRate		: 16; 		/* 16 bit */
} TU_SPECDATA_DVBC_T;

/**
 * Special data : QAM.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	UINT32									: 18;		/* Reserved */

	BOOLEAN					bSpectrumInv	: 1;		/* 1 bit */
	UINT8					cableBand		: 3;		/* 3 bit */
	TU_TPS_CONSTELLATION_T	constellation	: 4;		/* 4 bit */
} TU_SPECDATA_QAM_T;

/**
 * Special data : DVB-S.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		abwtransSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bwValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	UINT32					bwPolarization  : 1;
	UINT32					abwSymbolRate	: 16;
	UINT32					abwCodeRate		: 4;
	BOOLEAN					bwIsBlindScan	: 1;
	UINT32					abwDVBSS2		: 2;
	UINT32					abwReserved		: 2;		/* reserved */

} TU_SPECDATA_DVBS_T;

/**
 * Special data : ATV.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem 	: 4;		/* 4 bit */ /* Keep the position and size */
	BOOLEAN 				bValid			: 1;		/* 1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		: 1;		/* 1 bit */	/* Keep the position and size */

	UINT32									: 10;		/* Reserved */

	AUDIO_SIF_STANDARD_T	audioStandard	: 8;		/* 8 bit */
	AUDIO_SIF_SOUNDSYSTEM_T	audioSystem		: 8;		/* 8 bit */
} TU_SPECDATA_ATV_T;

/**
 * Special data : Common.
 */
typedef struct
{
	TU_TRANS_SYSTEM_T		transSystem		:  5;		/*  5 bit */	/* Keep the position and size */
	BOOLEAN					bValid			:  1;		/*  1 bit */	/* Keep the position and size */
	BOOLEAN					bBoosterOn		:  1;		/*  1 bit */	/* Keep the position and size */
	UINT32									: 25;		/* 25 bit */
} TU_SPECDATA_COMMMON_T;

/**
 * Special data : union.
 */
typedef union
{
	UINT32					specData32;		/* 32 bit encoded data */
	TU_SPECDATA_COMMMON_T	specCommon;		/* COMMON 	*/

	TU_SPECDATA_DVBT_T		specDVBT;		/* DVB-T 	*/
	TU_SPECDATA_DVBT2_T		specDVBT2;		/* DVB-T2 	*/
	TU_SPECDATA_VSB_T		specVSB;		/* VSB 		*/
	TU_SPECDATA_DTMB_T		specDTMB;		/* DTMB 	*/
	TU_SPECDATA_ISDBT_T		specISDBT;		/* ISDBT 	*/

	TU_SPECDATA_DVBC_T		specDVBC;		/* DVB-C 	*/
	TU_SPECDATA_QAM_T		specQAM;		/* QAM 		*/

	TU_SPECDATA_DVBS_T      specDVBS;		/*DVB-S		*/
	TU_SPECDATA_DVBS_T      specDVBS2;		/*DVB-S2		*/

	TU_SPECDATA_ATV_T		specATV;		/* ANALOG 	*/

} TU_SPECDATA_T;

/**
 * GPIO CONTROL MODE.
 */
typedef enum
{
	TU_CONTROL_BOOSTER	= 0x00,
	TU_CONTROL_RF_SWITCH,
	TU_CONTROL_ANT_5V,
	TU_CONTROL_DEMOD_RESET,		// DTV DEMOD reset
	TU_CONTROL_DEMOD_CLOCK,		// DEMOD clock enable
	TU_CONTROL_S_DEMOD_CLOCK, 			// DVBS DEMOD clock enable
	TU_CONTROL_LNB_SHORTAGE,		// Detection LNB Shortage in Japan
	TU_CONTROL_LNB_POWERCTRL,	// LNB Power Control in Japan
	TU_CONTROL_DEMOD_RESET_MANUAL, // DTV DEMOD reset manual ON/OFF
	TU_CONTROL_LNA_CTRL1,				// LNA ON/OFF in Japan
	TU_CONTROL_LNA_CTRL2,				// LNA ON/OFF in Japan
	TU_CONTROL_END,				// count of control modes

	TU_CONTROL_UNKNOWN	= 0x80
} TU_CONTROL_MODE_T;

/**
 * SUMD DATA MODE
 */
typedef enum
{
	TU_SUMD_DATA_ALL				= 0x00,
	TU_SUMD_DATA_CARRIER_THRESHOLD	= 0x01,		/* CR Threshold  for tuner(Only KR/US in LGT10) */
	TU_SUMD_DATA_PHASE_OPTION		= 0x02,
	TU_SUMD_DATA_AGC_SPEED			= 0x03,
	TU_SUMD_DATA_OVER_MODULATION	= 0x04,
} TU_SUMD_DATA_MODE_T;

/**
 *	Tuner Option struct.
 */
typedef enum
{
   	TU_DEFAULT =0,
	TU_ENHANCED_PHASE,
	TU_ENHANCED_GHOST,
	TU_ENHANCED_PHASE_HIGH
}	TU_PHASE_OPT_T;

/**
 *	DVB transsystem types.
 */
//#ifdef INCLUDE_SUPPORT_DVBSS2
typedef enum tagTU_SAT_TRANS_DVB
{
		TU_SAT_TRANS_DVB_S
	,	TU_SAT_TRANS_DVB_S2
	,	TU_SAT_TRANS_DVB_AUTO

} TU_SAT_TRANS_DVB_T;
//#endif /* INCLUDE_SUPPORT_DVBSS2 */


/**
 *	sumd data for tuner.
 */
typedef struct
{
	UINT32			carrierThreshold;
	TU_PHASE_OPT_T	phaseOption;
	UINT8			agcSpeed;
	UINT8			overModulation;
}	TU_SUMD_DATA_T;

/*
**	=======================================================
**	====================== PARAMETER ======================
**	=======================================================
*/


/**
 * DDI Parameter : Set Demod.
 */
typedef struct
{
	TUNER_NUMBER_T		tunerNo;
	TU_TUNE_MODE_T		tuneMode;

	UINT8				unPLP;
	TU_SPECDATA_T		specData;
	UINT16				tsid; //  for Japan Satelite.
} TU_PI_SET_DEMOD_T;

/**
 * DDI Parameter : Set PLL.
 */
typedef struct
{
	TUNER_NUMBER_T 		tunerNo;
	TRANS_MEDIA_T		transMedia;
	TU_TUNE_MODE_T		tuneMode;

	TU_SPECDATA_T		specData;
	TU_CH_BW_KHZ_U16	channelBW;

	UINT32		frequency;		/* 0 : invalid */
	UINT32		freqCenter; 	/* 0 : invalid */

	BOOLEAN		bIsPIPMode;		// add by seonguk.je
	BOOLEAN		bIsRecMode;		// add by seonguk.je
} TU_PI_SET_PLL_T;


/**
 * DDI Parameter : Tune postjob.
 */
typedef struct
{
	UINT32		tunedFrequency;		/* offseted frequency */
	BOOLEAN 	bFinished;
	BOOLEAN 	bFound;				/* ATV : center frequency is found */
	UINT32		delay;

#ifdef INCLUDE_SUPPORT_DVBSS2
	BOOLEAN		bIsMotorRotate;
	BOOLEAN 	bIsManualMode;
	UINT32		motorWaittime;
	BOOLEAN		bIsSCIFEnable;
#endif /* INCLUDE_SUPPORT_DVBSS2 */

} TU_PO_POSTJOB_T;

//#ifdef INCLUDE_SUPPORT_DVBSS2
/**
 * DDI Parameter : Blind scan out params.
 */
typedef struct
{
	UINT32				unFreqLocked;		/* MHz unit */
	UINT32 				SymbolRate;
	TU_TPS_CODERATE_T   Coderate;
	UINT32				unNextValidFreq;    /* MHz unit */
	UINT32				abwDVBSS2;	
} TU_BLIND_SCAN_OUTPUT_PARAMS;

/**
 * DDI Parameter : Blind scan in params.
 */
typedef struct
{
	UINT32  		uiUBFreqFUB;
	UINT8			ucUBSlotNo ;
	UINT8			ucInputBank ;
	
} TU_BLIND_SCAN_INPARAMS_T;

/**
 * DDI Parameter : SCIF Message type.
 */
typedef enum tagTU_SAT_SCIF_MSG_TYPE
{
		TU_SAT_SCIF_AUTODET_START
	,	TU_SAT_SCIF_AUTODET_CONT
	,	TU_SAT_SCIF_AUTODET_UBFOUND
	,	TU_SAT_SCIF_AUTODET_END

} TU_SAT_SCIF_MSG_TYPE_T;

/**
 * DDI Parameter : SCIF Auto detect parameters.
 */
typedef struct 
{
	UINT16 			usUserBandFreq[ TU_SCIF_MAX_UB ];
	UINT8			ucNoofUBSlots ;
	UINT8			ucNoofSatPos ;
	UINT8			ucNoofInputBank ;
	UINT8			ucTypeofBand;
	UINT32			usLocalOscillatorFreq;
}TU_SCIF_SPE_PARAMS_T;

/**
 * DDI Parameter : SCIF Bind detect parameters.
 */

typedef struct 
{
    UINT16      startFreqMHz; 
    UINT16      stopFreqMHz;
} TU_PI_SCIF_BLIND_DETECT_T;

typedef struct 
{
	UINT32	    freq_MHz;
	UINT8	    ub_num;
    
	UINT32	    gain;
} TU_SCIF_BLIND_DETECT_T;

typedef struct 
{
    TU_SCIF_BLIND_DETECT_T      *pUBlist;
    
    UINT8       count;
} TU_PO_SCIF_BLIND_DETECT_T;


/**
 * SCIF ACK
*/
typedef enum {

	TU_SCIF_ACK_YES,
	TU_SCIF_ACK_NO,
	TU_SCIF_ACK_NOT_DETECTED,

	TU_SCIF_ACK_UNKNOWN
	
} TU_SCIF_ACK_T;


//#endif /* INCLUDE_SUPPORT_DVBSS2 */


/**
 * DDI Parameter : frequency offset.
 */
typedef struct
{
	UINT8	PLPCount;		/* count of multiple TS*/
	UINT8	*paPLPID;		/* arrary of PLP ID */
} TU_PO_MULTI_TS_INFO_T;


/**
 * DDI Parameter : get tunning info(CHB usage)
 */
typedef struct
{
	TU_TRANS_SYSTEM_T	transSystem;
	TU_TUNE_MODE_T		tuneMode;

	UINT32		freqCenter;
} TU_PO_GET_TUNE_INFO_T;

/**
 * DDI Parameter : DiSEqC Message(DVB-S usage)
 */
typedef enum TU_PARAM_TONE_MSG
{
	TU_DISEQC_TONEBURST_MODULATED 		=0,
	TU_DISEQC_TONEBURST_UNMODULATED
}TU_PARAM_TONE_MSG_T;


/**
 * DDI Parameter : DiSEqC Message(DVB-S usage)
 */
typedef struct TU_PARAM_DISEQC_MSG
{
	UINT8				*pDiSEqCSendMsgBuff;
	UINT8 				DiSEqCSendMsgSize;
	BOOLEAN 			IsRecEnable;
	UINT8 				*pDiSEqCRecMsgBuff;
	UINT8				DiSEqCRecMsgSize;
	TU_PARAM_TONE_MSG_T	DiSEqCToneMsgType;
	BOOLEAN 			IsDISEqCToneMsg;
}TU_PARAM_DISEQC_MSG_T;


/**
 * DDI Parameter : LNB Voltage(DVB-S usage)
 */
typedef enum TU_LNB_VOLTAGE
{
		LNB_12_709		=0
		,LNB_13_042		=1
		,LNB_13_375		=2
		,LNB_13_709		=3
		,LNB_14_042		=4
		,LNB_14_375		=5
		,LNB_14_709		=6
		,LNB_15_042		=7
		,LNB_18_042		=8
		,LNB_18_375		=9
		,LNB_18_709		=10
		,LNB_19_042		=11
		,LNB_19_375		=12
		,LNB_19_709		=13
		,LNB_20_042		=14
		,LNB_20_375		=15
		,LNB_OFF			=255
}TU_LNB_VOLTAGE_T;


/******************************************************************************
	Function Declaration
******************************************************************************/

extern	void			DDI_TU_PowerOnReset(void);
extern	DTV_STATUS_T 	DDI_TU_Probe(TUNER_NUMBER_T tunerNo, TU_HW_CONFIG_T *pHardwareConfig);
extern	DTV_STATUS_T 	DDI_TU_Initialize(TUNER_NUMBER_T tunerNo, TRANS_MEDIA_T transMedia);
extern	void			DDI_TU_ChangeCountryConfig(TU_COUNTRY_CONFIG_T countryConfig);
extern	DTV_STATUS_T 	DDI_TU_ChangeTransMedia(TUNER_NUMBER_T tunerNo, TRANS_MEDIA_T transMedia);
extern	DTV_STATUS_T 	DDI_TU_SetPLL(TU_PI_SET_PLL_T paramStruct);
extern	DTV_STATUS_T 	DDI_TU_SetDemod(TU_PI_SET_DEMOD_T paramStruct);
extern	DTV_STATUS_T 	DDI_TU_ChangePLP(TUNER_NUMBER_T tunerNo, UINT8 unPLP);
extern	DTV_STATUS_T 	DDI_TU_TunePostJob(TUNER_NUMBER_T tunerNo, TU_PO_POSTJOB_T *pParamOutput);
extern	DTV_STATUS_T 	DDI_TU_ControlOutput(TUNER_NUMBER_T tunerNo, BOOLEAN bEnableOutput);
#ifdef INCLUDE_NEW_SDEC_PATH
extern	DTV_STATUS_T 	DDI_TU_SetOutputPath(TUNER_NUMBER_T tunerNo, UINT8 chPort, BOOLEAN bWithCAM);
#else
extern	DTV_STATUS_T 	DDI_TU_SetOutputPath(TUNER_NUMBER_T tunerNo, BOOLEAN bWithCAM);
#endif
extern	DTV_STATUS_T 	DDI_TU_ControlTSMode(TUNER_NUMBER_T tunerNo, BOOLEAN bIsSerial);
extern 	DTV_STATUS_T 	DDI_TU_CheckLockState(TUNER_NUMBER_T tunerNo, TU_LOCK_STATE_T *pLockState);
extern	DTV_STATUS_T 	DDI_TU_CheckSignalState(TUNER_NUMBER_T tunerNo, TU_SIGNAL_STATE_T *pSignalState);
extern	DTV_STATUS_T	DDI_TU_CheckSpecialData(TUNER_NUMBER_T tunerNo, TU_SPECDATA_T *pSpecData);
extern	DTV_STATUS_T	DDI_TU_SetSpecialData(TUNER_NUMBER_T tunerNo, TU_SPECDATA_T specData);
extern	DTV_STATUS_T 	DDI_TU_CheckFrequencyOffset(TUNER_NUMBER_T tunerNo, UINT32 *pFreqOffset);

extern	DTV_STATUS_T 	DDI_TU_DVB_GetMultiPLPInfo(TUNER_NUMBER_T tunerNo, TU_PO_MULTI_TS_INFO_T *pParamOut);
extern	UINT16 			DDI_TU_DVB_GetCellID(TUNER_NUMBER_T tunerNo, TRANS_MEDIA_T transMedia);
extern UINT32			DDI_TU_GetDemodFWVersion(TUNER_NUMBER_T tunerNo, TRANS_MEDIA_T transMedia);

extern	DTV_STATUS_T 	DDI_TU_SetControl(TUNER_NUMBER_T tunerNo, TU_CONTROL_MODE_T controlMode, BOOLEAN bControl);
extern	DTV_STATUS_T 	DDI_TU_GetControl(TUNER_NUMBER_T tunerNo, TU_CONTROL_MODE_T controlMode, BOOLEAN *pbControl);

extern	void			DDI_TU_SetSumdData(TUNER_NUMBER_T tunerNo, TU_SUMD_DATA_MODE_T sumdMode, void* pSumdData);
extern UINT16 			DDI_TU_Monitor (TUNER_NUMBER_T tunerNo, TU_LOCK_STATE_T *pLockState);
extern	void 			DDI_TU_DebugMenu (TUNER_NUMBER_T tunerNo);

extern	DTV_STATUS_T 	DDI_TU_SetTuneMode(TUNER_NUMBER_T tunerNo, TU_TUNE_MODE_T tuneMode);
extern  DTV_STATUS_T 	DDI_TU_SATSetLNBPower(TUNER_NUMBER_T tunerNo,BOOLEAN bOnOff);
extern DTV_STATUS_T 	DDI_TU_GetLNBPowerStatus(TUNER_NUMBER_T tunerNo, BOOLEAN *pbStatus);
extern  DTV_STATUS_T 	DDI_TU_SetAttenuator(TUNER_NUMBER_T tunerNo,BOOLEAN bOnOff);
extern  DTV_STATUS_T 	DDI_TU_GetAttenuatorStatus(TUNER_NUMBER_T tunerNo, BOOLEAN *pbStatus);

#ifdef INCLUDE_SUPPORT_DVBSS2
extern  DTV_STATUS_T 	DDI_TU_SATSendReceiveDiSEqCMsg(TUNER_NUMBER_T tunerNo,TU_PARAM_DISEQC_MSG_T DiSEqCParm);
extern  DTV_STATUS_T 	DDI_TU_SATSendToneBurst(TUNER_NUMBER_T tunerNo , TU_PARAM_TONE_MSG_T ToneBurstParm);
extern  DTV_STATUS_T 	DDI_TU_SATSetOutputVoltage(TUNER_NUMBER_T tunerNo,TU_LNB_VOLTAGE_T LNBVoltage);
extern  DTV_STATUS_T 	DDI_TU_SATSet22khzOnOff(TUNER_NUMBER_T tunerNo,BOOLEAN Is22KhzOn);
extern  DTV_STATUS_T 	DDI_TU_SATCheckLNBFault( TUNER_NUMBER_T tunerNo , BOOLEAN *bIsLNBFault);
extern  DTV_STATUS_T 	DDI_TU_SATBSGetLockFreq(TUNER_NUMBER_T tunerNo, TU_BLIND_SCAN_OUTPUT_PARAMS	*plockFreqParam );
extern  DTV_STATUS_T 	DDI_TU_SATBSGetNextFreq(TUNER_NUMBER_T tunerNo, TU_BLIND_SCAN_OUTPUT_PARAMS	*plockFreqParam );
extern  DTV_STATUS_T 	DDI_TU_SATBSInit(TUNER_NUMBER_T tunerNo, UINT32 uiStartFreq, UINT32 uiEndFreq , BOOLEAN bIsUnicableEnable);
extern  DTV_STATUS_T 	DDI_TU_SATBSSetSCIFParams( TUNER_NUMBER_T tunerNo, TU_BLIND_SCAN_INPARAMS_T *pstBSInputparams );
extern	DTV_STATUS_T	DDI_TU_SATSCIFSetFlag ( TUNER_NUMBER_T tunerNo, BOOLEAN isEnable);
extern  DTV_STATUS_T    DDI_TU_SATSCIFAutodetectUB (TUNER_NUMBER_T tunerNo, TU_PI_SCIF_BLIND_DETECT_T paramIn, TU_PO_SCIF_BLIND_DETECT_T *paramOut);
extern  DTV_STATUS_T    DDI_TU_SATSCIFCheckACK   (TUNER_NUMBER_T tunerNo, UINT32 freqMhz, TU_SCIF_ACK_T *pScifAck, UINT32 *pGain);

#endif /* INCLUDE_SUPPORT_DVBSS2 */

extern  DTV_STATUS_T TU_OSDDGB_Setup (DTV_STATUS_T (*init)(void), DTV_STATUS_T (*clear)(void), void (*print)(char *format, ...), DTV_STATUS_T (*Show)(void));


#ifdef __cplusplus
}
#endif

#endif	/* _TU_DDI_H_ */

