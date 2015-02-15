/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2008 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file tu_lgt10.h
 *
 *  ATV IC.
 *
 *  @author		Goldman(jhgold@lge.com)
 *  @version	0.5
 *  @date		2009.06.25
 *  @note		Based on XC5000
 *  @see
 */

/******************************************************************************
	Header File Guarder
******************************************************************************/
#ifndef _TU_DEMOD_ANALOG_H_
#define	_TU_DEMOD_ANALOG_H_

/******************************************************************************
	Control Constants
******************************************************************************/


/******************************************************************************
	File Inclusions
******************************************************************************/
	
#include "hal_demod_common.h"


/******************************************************************************
 	Constant Definitions
******************************************************************************/

/******************************************************************************
	Macro Definitions
******************************************************************************/


/******************************************************************************
	Type Definitions
******************************************************************************/
//#ifndef LX_ADEMOD_Result
//typedef UINT32			LX_ADEMOD_Result;		/*	return codes					*/
//#endif

/******************************************************************************
	Function Declaration
******************************************************************************/
extern DTV_STATUS_T HAL_ADEMOD_Probe(UINT8 portI2C, UINT32 *pHandleI2C);
extern DTV_STATUS_T HAL_ADEMOD_NTSC_Initialize(void );
extern DTV_STATUS_T HAL_ADEMOD_PAL_Initialize(void );
extern DTV_STATUS_T HAL_ADEMOD_ChangeTransMedia(HAL_DEMOD_TRANS_SYSTEM_T transMidia) ;
extern DTV_STATUS_T HAL_ADEMOD_Change_TransMedia(HAL_DEMOD_ANALOG_SET_PARAM_T *pChangeParam);
extern DTV_STATUS_T HAL_ADEMOD_SetDemod(HAL_DEMOD_ANALOG_CONFIG_T *pAnalogParam );
extern DTV_STATUS_T HAL_ADEMOD_TunePostJob(BOOLEAN *pFinished, SINT32 *pOffsetFreq, HAL_DEMOD_LOCK_STATE_T *pLockState);
extern DTV_STATUS_T HAL_ADEMOD_ControlOutput( BOOLEAN bEnableOutput);
extern DTV_STATUS_T HAL_ADEMOD_ATVIsLocked( BOOLEAN isLocked);
extern DTV_STATUS_T HAL_ADEMOD_GetSignalQulity( UINT32 *pSignalQaulity);
extern DTV_STATUS_T HAL_ADEMOD_GetSNR( UINT32 *pSignalQaulity);
extern DTV_STATUS_T HAL_ADEMOD_CheckLockState(HAL_DEMOD_LOCK_STATE_T *pLockState);
extern DTV_STATUS_T HAL_ADEMOD_SetSpecialData(HAL_DEMOD_AUDIO_SIF_SOUNDSYSTEM_T audioSystem );
extern void HAL_ADEMOD_DebugMenu (void);

#endif /* End of _HAL_DEMOD_ANALOG_H_ */

