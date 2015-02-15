/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_demod.h
 *
 *  HAL 함수 header 파일.
 *
 *
 *  @author	Harrison(hyungkyu.noh@lge.com)
 *  @reviser
 *  @version	1.1
 *  @date		2013.05.27
 *  @note
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_DEMOD_COMMON_H_
#define _HAL_DEMOD_COMMON_H_

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
#define HAL_DEMOD_IF_FREQ_05_000_KHZ			5000
#define HAL_DEMOD_IF_FREQ_06_000_KHZ			6000

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
typedef enum
{
	/* TERRESTRIAL */
	HAL_DEMOD_TRANS_SYS_VSB		= 0x00,			/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_DVBT,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_DVBT2,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_DTMB,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_ISDBT,						/* 5 bit : Don't exceed */

	/* CABLE */
	HAL_DEMOD_TRANS_SYS_DVBC,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_DVBC2,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_QAM,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_ISDBC,						/* 5 bit : Don't exceed */

	/* SATELLITE */
	HAL_DEMOD_TRANS_SYS_DVBS,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_DVBS2,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_BS,						/* 4 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_CS,						/* 5 bit : Don't exceed */

	/* ANALOG */
	HAL_DEMOD_TRANS_SYS_NTSC,						/* 5 bit : Don't exceed */
	HAL_DEMOD_TRANS_SYS_PAL,						/* 5 bit : Don't exceed */

	HAL_DEMOD_TRANS_SYS_END,						/* 5 bit : Don't exceed */

	HAL_DEMOD_TRANS_SYS_UNKNOWN	= 0x1F,			/* 5 bit : Don't exceed */
}HAL_DEMOD_TRANS_SYSTEM_T;

/**
 * tune mode.
 */
typedef enum
{
	HAL_DEMOD_TUNE_NORMAL					= 0x10,		/* 0001 XXXX */

	HAL_DEMOD_TUNE_MANUAL					= 0X20,

	/*	---------- SCAN 	---------------------	*/
	HAL_DEMOD_TUNE_SCAN					= 0x30,		/* 0010 XXXX */

		HAL_DEMOD_TUNE_SCAN_START,				/* ATV : Full Scan */

	/*	---------- SPECIFIC 	---------------------	*/
	HAL_DEMOD_TUNE_SPECIFIC					= 0x40,		/* 0100 XXXX */
		HAL_DEMOD_TUNE_SPEC_DVBT_HMLP,			/* DVBT : Hierarchy Mode */
		HAL_DEMOD_TUNE_SPEC_DVBC_FIXED_DATA,	/* DVBC : Use Fixed NIT data */


	/*	---------- UNKNOWN 	---------------------	*/
	HAL_DEMOD_TUNE_UNKNOWN					= 0x80,		/* 1000 XXXX */

	/*	---------- MASK 	---------------------	*/
	HAL_DEMOD_TUNE_MODE_MASK					= 0xF0,		/* 1111 XXXX */


} HAL_DEMOD_TUNE_MODE_T;

/**
 * Lock state.
 */
typedef enum
{
	HAL_DEMOD_LOCK_OK			= 0x00,
	HAL_DEMOD_LOCK_FAIL,
	HAL_DEMOD_LOCK_UNSTABLE,

	HAL_DEMOD_LOCK_WEAK		= 0x10,
	HAL_DEMOD_LOCK_POOR,
	HAL_DEMOD_LOCK_ATV_PROGRESS,		/* used in auto search */

	HAL_DEMOD_LOCK_UNKNOWN 	= 0x80
} HAL_DEMOD_LOCK_STATE_T;

/**
 * channel bandwidth
 */
typedef enum
{
	HAL_DEMOD_CH_BW_8M	= 0,
	HAL_DEMOD_CH_BW_7M,
	HAL_DEMOD_CH_BW_6M,
	HAL_DEMOD_CH_BW_UNKNOWN
} HAL_DEMOD_CHANNEL_BW_T;

/**
 * TPS : constellation.
 */
typedef enum 	/* 4 bit */
{
	HAL_DEMOD_TPS_CONST_QPSK 		= 0x00,
	HAL_DEMOD_TPS_CONST_DQPSK,
	HAL_DEMOD_TPS_CONST_QAM_4NR,
	HAL_DEMOD_TPS_CONST_QAM_4,
	HAL_DEMOD_TPS_CONST_PSK_8,
	HAL_DEMOD_TPS_CONST_VSB_8,
	HAL_DEMOD_TPS_CONST_QAM_16,
	HAL_DEMOD_TPS_CONST_QAM_32,
	HAL_DEMOD_TPS_CONST_QAM_64,
	HAL_DEMOD_TPS_CONST_QAM_128,
	HAL_DEMOD_TPS_CONST_QAM_256,

	HAL_DEMOD_TPS_CONST_END,

	HAL_DEMOD_TPS_CONST_UNKNOWN 	= 0x0F
} HAL_DEMOD_TPS_CONSTELLATION_T;

/**
 * TPS : code rate.
 */
typedef enum 	/* 4 bit */
{
	HAL_DEMOD_TPS_CODE_1_2 		= 0x00,
	HAL_DEMOD_TPS_CODE_1_3,
	HAL_DEMOD_TPS_CODE_1_4,
	HAL_DEMOD_TPS_CODE_2_3,
	HAL_DEMOD_TPS_CODE_3_4,
	HAL_DEMOD_TPS_CODE_2_5,
	HAL_DEMOD_TPS_CODE_3_5,
	HAL_DEMOD_TPS_CODE_4_5,
	HAL_DEMOD_TPS_CODE_5_6,
	HAL_DEMOD_TPS_CODE_6_7,
	HAL_DEMOD_TPS_CODE_7_8,
	HAL_DEMOD_TPS_CODE_8_9,
	HAL_DEMOD_TPS_CODE_9_10,

	HAL_DEMOD_TPS_CODE_END,

	HAL_DEMOD_TPS_CODE_UNKNOWN 	= 0x0F
} HAL_DEMOD_TPS_CODERATE_T;

/**
 * TPS : guard interval.
 */
typedef enum 	/* 4 bit */
{
	HAL_DEMOD_TPS_GUARD_1_4 		= 0x00,
	HAL_DEMOD_TPS_GUARD_1_8,
	HAL_DEMOD_TPS_GUARD_1_9,
	HAL_DEMOD_TPS_GUARD_1_16 ,
	HAL_DEMOD_TPS_GUARD_1_32,
	HAL_DEMOD_TPS_GUARD_1_128,
	HAL_DEMOD_TPS_GUARD_19_128,
	HAL_DEMOD_TPS_GUARD_19_256,

	HAL_DEMOD_TPS_GUARD_420_C,
	HAL_DEMOD_TPS_GUARD_420_V,
	HAL_DEMOD_TPS_GUARD_595,
	HAL_DEMOD_TPS_GUARD_945_C,
	HAL_DEMOD_TPS_GUARD_945_V,

	HAL_DEMOD_TPS_GUARD_END,

	HAL_DEMOD_TPS_GUARD_UNKNOWN 	= 0x0F
} HAL_DEMOD_TPS_GUARD_INTERVAL_T;

/**
 * TPS : carrier mode.
 */
typedef enum 	/* 4 bit */
{
	HAL_DEMOD_TPS_CARR_1K 			= 0x00,
	HAL_DEMOD_TPS_CARR_2K,
	HAL_DEMOD_TPS_CARR_4K,
	HAL_DEMOD_TPS_CARR_8K,
	HAL_DEMOD_TPS_CARR_16K,
	HAL_DEMOD_TPS_CARR_32K,

	HAL_DEMOD_TPS_CARR_SC,
	HAL_DEMOD_TPS_CARR_MC,

	HAL_DEMOD_TPS_CARR_END,

	HAL_DEMOD_TPS_CARR_UNKNOWN 	= 0x0F
} HAL_DEMOD_TPS_CARRIER_MODE_T;

/**
 * TPS : hierarchy.
 */
typedef enum 	/* 3 bit */
{
	HAL_DEMOD_TPS_HIERA_NONE 		= 0x00,
	HAL_DEMOD_TPS_HIERA_1,
	HAL_DEMOD_TPS_HIERA_2,
	HAL_DEMOD_TPS_HIERA_4,

	HAL_DEMOD_TPS_HIERA_END,

	HAL_DEMOD_TPS_HIERA_UNKNOWN 	= 0x07
} HAL_DEMOD_TPS_HIERARCHY_T;

/**
 * polarization
 */
typedef enum
{
	HAL_DEMOD_TPS_POLARIZATION_HORIZONTAL	= 0x00,
	HAL_DEMOD_TPS_POLARIZATION_VERTICAL,

	HAL_DEMOD_TPS_POLARIZATION_LEFT,
	HAL_DEMOD_TPS_POLARIZATION_RIGHT,

	HAL_DEMOD_TPS_POLARIZATION_MAX,

	HAL_DEMOD_TPS_POLARIZATION_ALL
}HAL_DEMOD_TPS_POLARIZATION_T;

/**
 * Audio
 */

typedef enum
{
	HAL_DEMOD_AUDIO_SIF_SYSTEM_BG		= 0x00,	
	HAL_DEMOD_AUDIO_SIF_SYSTEM_I,				
	HAL_DEMOD_AUDIO_SIF_SYSTEM_DK,			
	HAL_DEMOD_AUDIO_SIF_SYSTEM_L,				
	HAL_DEMOD_AUDIO_SIF_SYSTEM_MN,			
	HAL_DEMOD_AUDIO_SIF_SYSTEM_LP,			
	HAL_DEMOD_AUDIO_SIF_SYSTEM_END,			
 	HAL_DEMOD_AUDIO_SIF_SYSTEM_UNKNOWN = 0xf0
} HAL_DEMOD_AUDIO_SIF_SOUNDSYSTEM_T;


/**
 * signal state.
 */
typedef struct
{
	BOOLEAN bSignalValid;		/*	1 bit */
	UINT8 	unSQI;
	UINT32	packetError;
	UINT32	unBER;			/* unit : e-10 */
	UINT32	unAGC;
	UINT32	unSNR;
} HAL_DEMOD_SIGNAL_STATE_T;

typedef struct
{
	HAL_DEMOD_TUNE_MODE_T			tuneMode;
	HAL_DEMOD_TRANS_SYSTEM_T		transSystem;	
	HAL_DEMOD_CHANNEL_BW_T			eChannelBW;
	BOOLEAN							bSpectrumInv;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;	
} HAL_DEMOD_ATSC_SET_PARAM_T;

typedef struct
{
	HAL_DEMOD_TUNE_MODE_T			tuneMode;
	HAL_DEMOD_TRANS_SYSTEM_T		transSystem;	
	HAL_DEMOD_CHANNEL_BW_T			eChannelBW;
	BOOLEAN							bSpectrumInv;
} HAL_DEMOD_ISDBT_SET_PARAM_T;

typedef struct
{
	HAL_DEMOD_TUNE_MODE_T			tuneMode;
	HAL_DEMOD_TRANS_SYSTEM_T		transSystem;	
	HAL_DEMOD_CHANNEL_BW_T			eChannelBW;
	BOOLEAN							bSpectrumInv;
	BOOLEAN							bProfileHP;
	HAL_DEMOD_TPS_HIERARCHY_T		hierarchy;
	HAL_DEMOD_TPS_CARRIER_MODE_T	carrierMode;
	HAL_DEMOD_TPS_GUARD_INTERVAL_T	guardInterval;
	HAL_DEMOD_TPS_CODERATE_T		codeRate;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;	
} HAL_DEMOD_DVBT_SET_PARAM_T;

typedef struct
{
	HAL_DEMOD_TUNE_MODE_T			tuneMode;
	HAL_DEMOD_TRANS_SYSTEM_T		transSystem;	
	HAL_DEMOD_CHANNEL_BW_T			eChannelBW;
	UINT32							frequency;
	UINT16 							symbolRate;
	BOOLEAN							bSpectrumInv;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;	
} HAL_DEMOD_DVBC_SET_PARAM_T;

/**
*ANALOG
*/
typedef struct
{
	HAL_DEMOD_TRANS_SYSTEM_T 			transSystem;
	BOOLEAN								bSpectrmInv;
	UINT32								ifFrq;

} HAL_DEMOD_ANALOG_SET_PARAM_T;


/**
 * Special data : DVB-T.
 */
typedef struct
{
	BOOLEAN							bSpectrumInv;		/* 1 bit */
	BOOLEAN							bProfileHP;		/* 1 bit */
	HAL_DEMOD_TPS_HIERARCHY_T		hierarchy;		/* 3 bit */

	HAL_DEMOD_TPS_CARRIER_MODE_T	carrierMode;		/* 4 bit */
	HAL_DEMOD_TPS_GUARD_INTERVAL_T	guardInterval;		/* 4 bit */

	HAL_DEMOD_TPS_CODERATE_T		codeRate;		/* 4 bit */
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;		/* 4 bit */
} HAL_DEMOD_SPECDATA_DVBT_T;

/**
 * Special data : DVB-T2.
 */
typedef struct
{
	HAL_DEMOD_TPS_CARRIER_MODE_T	carrierMode;
	HAL_DEMOD_TPS_GUARD_INTERVAL_T 	guardInterval;	

	HAL_DEMOD_TPS_CODERATE_T		codeRate;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;
} HAL_DEMOD_SPECDATA_DVBT2_T;

/**
 * Special data : VSB.
 */
typedef struct
{
	BOOLEAN							bCoChannel;		/* 1 bit */
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;		/* 4 bit */
} HAL_DEMOD_SPECDATA_VSB_T;


/**
 * Special data : DVB-C.
 */
typedef struct
{
	BOOLEAN							bSpectrumInv;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;
	UINT16							symbolRate; 
} HAL_DEMOD_SPECDATA_DVBC_T;

/**
 * Special data : QAM.
 */
typedef struct
{
	BOOLEAN							bSpectrumInv;
	UINT8							cableBand;
	HAL_DEMOD_TPS_CONSTELLATION_T	constellation;
} HAL_DEMOD_SPECDATA_QAM_T;


/**
*ANALOG
*/
typedef struct
{
	UINT32								centerFreq;						
	UINT32								tunedFreq;
	BOOLEAN								bSpectrmInv;
	HAL_DEMOD_TRANS_SYSTEM_T			transSystem;					
	HAL_DEMOD_TUNE_MODE_T				tuneMode;						
	HAL_DEMOD_CHANNEL_BW_T 				channelBW;						
	HAL_DEMOD_AUDIO_SIF_SOUNDSYSTEM_T	audioSystem;						

} HAL_DEMOD_ANALOG_CONFIG_T;


#ifdef __cplusplus
}
#endif

#endif /* _HAL_AUDIO_H_ */


