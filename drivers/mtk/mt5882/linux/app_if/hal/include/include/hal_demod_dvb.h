/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2008 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file demod_lg115x_dvb.h
 *
 *  DVB T/C Demod.
 *
 *  @author		Jeongpil Yun(jeongpil.yun@lge.com)
 *  @version	0.1
 *  @date		2010.01.13
 *  @see
 */

/******************************************************************************
	Header File Guarder
******************************************************************************/
#ifndef _HAL_DEMOD_DVB_H_
#define	_HAL_DEMOD_DVB_H_

#include "hal_demod_common.h"

/******************************************************************************
	Control Constants
******************************************************************************/
//#define FPGA_DEMOD_TEST


/******************************************************************************
	File Inclusions
******************************************************************************/



/******************************************************************************
 	Constant Definitions
******************************************************************************/


/******************************************************************************
	Macro Definitions
******************************************************************************/

#define GBB_DEMOD_PRINT FE_PRINT_0
//#define GBB_DEMOD_PRINT OSA_PRINT //FE_PRINT_0
#define DEFINE_SEPARATE_RESET 1

/* Nordig SSI Reference Vale */
/* QPSK */
#define GBB_DVB_DVBT_QPSK_1_2_REF			-93 //-81
#define GBB_DVB_DVBT_QPSK_2_3_REF			-91 //-79
#define GBB_DVB_DVBT_QPSK_3_4_REF			-90 //-78
#define GBB_DVB_DVBT_QPSK_5_6_REF			-89 //-77
#define GBB_DVB_DVBT_QPSK_7_8_REF			-88 //-76


/* 16QAM */
#define GBB_DVB_DVBT_16QAM_1_2_REF			-87 //-75
#define GBB_DVB_DVBT_16QAM_2_3_REF			-85 //-73
#define GBB_DVB_DVBT_16QAM_3_4_REF			-84 //-72
#define GBB_DVB_DVBT_16QAM_5_6_REF			-83 //-71
#define GBB_DVB_DVBT_16QAM_7_8_REF			-82 //-70


/* 64QAM */
#define GBB_DVB_DVBT_64QAM_1_2_REF			-82 //-69
#define GBB_DVB_DVBT_64QAM_2_3_REF			-80 //-68
#define GBB_DVB_DVBT_64QAM_3_4_REF			-78 //-66
#define GBB_DVB_DVBT_64QAM_5_6_REF			-77 //-65
#define GBB_DVB_DVBT_64QAM_7_8_REF			-76 //-64


/* DVBC */
#define GBB_DVB_DVBC_16QAM_REF				-77 //-65
#define GBB_DVB_DVBC_32QAM_REF				-77 //-65
#define GBB_DVB_DVBC_64QAM_REF				-77 //-65
#define GBB_DVB_DVBC_128QAM_REF				-72 //-60
#define GBB_DVB_DVBC_256QAM_REF				-72 //-60

#define	GBB_DVB_SIGNAL_VALID_COUNT			5


// unit : need to be modify for MSB1228
#define	GBB_DVB_DVBT_LOCK_DELAY_DEFAULT		1500	// for AGC/SYM Lock
#define	GBB_DVB_DVBT_LOCK_DELAY_OFDM		5000	// for OFDM
#define	GBB_DVB_DVBT_LOCK_DELAY_FEC			5000	// for FEC
#define	GBB_DVB_DVBT_LOCK_DELAY_AUTO		1500	// for Auto Scan
#define GBB_DVB_DVBT_LOCK_DELAY_TPS			5000
#define GBB_DVB_DVBT_LOCK_DELAY_7MHZ		2000	// MSB1228 not support Never for 7MHz


/* DVB-C Lock Time */
#define	GBB_DVB_DVBC_LOCK_DELAY_DEFAULT		2000		// Frame Lock Waiting Lock
#define	GBB_DVB_DVBC_LOCK_DELAY_CONST		800		//2000	// for AGC/SYM Lock
#define	GBB_DVB_DVBC_LOCK_DELAY_TR			3000	// for DVBC
#define	GBB_DVB_DVBC_LOCK_DELAY_AUTO_DETECT	2000	// for AGC/SYM Lock


#define	GBB_DVB_AUTO_BOOSTER_DELAY			300		// XC5000 + MSB1228 Do not use Auto Booster

#define GBB_DVB_DVBC_SCAN_OFFSET_COUNT		3

#define GBB_DVB_DEMOD_LOCK_COUNT			10
#define GBB_DVB_DEMOD_LOCK_DELAY			50 /* msec delay */

/* GBB DVB-C Const. */
#define GBB_DVB_DVBC_CONST_16QAM			0
#define GBB_DVB_DVBC_CONST_32QAM			1
#define GBB_DVB_DVBC_CONST_64QAM			2
#define GBB_DVB_DVBC_CONST_128QAM			3
#define GBB_DVB_DVBC_CONST_256QAM			4


//#define DBG_PRINT printf
/******************************************************************************
	Type Definitions
******************************************************************************/

/**
 * GBB Signal State
*/
typedef enum /*LG1150 Signal State */
{
	// =============================================== //
	GBB_DVB_POWER_VALID		= 0x00,
	GBB_DVB_NO_CHANNEL_POWER,
	GBB_DVB_CHECKING_POWER,
	GBB_DVB_SIGNAL_UNKNOWN		= 0x80
	// =============================================== //
} GBB_DVB_SIGNAL_STATE_T;

/**
 * GBB RF MODE
*/
typedef enum
{
	GBB_DVB_AIR_MODE = 0,
	GBB_DVB_CABLE_MODE = 1
} GBB_DVB_MODE_T;

/**
 * GBB Cable channel MODE
*/
typedef enum
{
	GBB_DVB_STD_BAND = 0,
	GBB_DVB_IRC_BAND,
	GBB_DVB_HRC_BAND
} GBB_DVB_BAND_T;

typedef struct
{
	UINT8	chanIndex;
	UINT32	freqCenter;
	UINT8	count;
	UINT16  offset;
} GBB_DVB_TABLE_CHANNEL_T;





/******************************************************************************
	Function Declaration
******************************************************************************/
extern	UINT32	HAL_DEMOD_DVB_Probe( UINT8 portI2C);
extern	int		HAL_DEMOD_DVB_Initialize(void);
extern	int		HAL_DEMOD_DVB_ChangeTransMedia(HAL_DEMOD_TRANS_SYSTEM_T transSystem);
extern	int		HAL_DEMOD_DVB_DVBT_SetDemod(HAL_DEMOD_DVBT_SET_PARAM_T paramStruct);
extern  int		HAL_DEMOD_DVB_DVBC_SetDemod(HAL_DEMOD_DVBC_SET_PARAM_T paramStruct);
extern	int 	HAL_DEMOD_DVB_DVBC_TunePostJob( BOOLEAN *pFinished);
extern	int 	HAL_DEMOD_DVB_DVBT_TunePostJob( BOOLEAN *pFinished);
extern	int 	HAL_DEMOD_DVB_GetPacketError( UINT32 *pFecPkerr);
extern	int		HAL_DEMOD_DVB_CheckFrequencyOffset(SINT32 *pFreqOffset);
extern	int 	HAL_DEMOD_DVB_ControlOutput(BOOLEAN bEnableOutput);
extern 	int		HAL_DEMOD_DVB_GetCellID(UINT16 *pCellID);
extern	int		HAL_DEMOD_DVB_ControlTSMode( BOOLEAN bIsSerial);
extern	int		HAL_DEMOD_DVB_DVBT_CheckLock(HAL_DEMOD_LOCK_STATE_T *pLockState);
extern	int		HAL_DEMOD_DVB_DVBC_CheckLock(HAL_DEMOD_LOCK_STATE_T *pLockState);
extern	int		HAL_DEMOD_DVB_DVBT_CheckSpecialData(HAL_DEMOD_SPECDATA_DVBT_T *pSpecDVBT);
extern	int		HAL_DEMOD_DVB_DVBC_CheckSpecialData(HAL_DEMOD_SPECDATA_DVBC_T *pSpecDVBC);
extern	int		HAL_DEMOD_DVB_DVBT_GetSignalState(HAL_DEMOD_SIGNAL_STATE_T *pSignalState);
extern	int 	HAL_DEMOD_DVB_DVBC_GetSignalState(HAL_DEMOD_SIGNAL_STATE_T *pSignalState);
extern	int 	HAL_DEMOD_DVB_DebugMenu(void);


#endif /* End of _DEMOD_LG115X_DVB_H_ */

