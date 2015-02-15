/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file vbe_sre_soc_ddi.h
 *
 *  It is the header file for controling Video Backend SRE function.
 *  The header of VBE DDI is controlled by this file.
 *
 *  @author	Seol, SeongWoon(sw.seol@lge.com)
 *  @version	0.1
 *  @date		2012.06.13
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VBE_DISP_SR_H_
#define _HAL_VBE_DISP_SR_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
#include "hal_common.h"

/*-----------------------------------------------------------------------------
	Constant Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Macro Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Extern Variables & Function Prototype Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Constant Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Type Definitions
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global Type Definitions
------------------------------------------------------------------------------*/
typedef struct
{
UINT8 enable;					///< enable
UINT8 DB_grade; 				///< DB_grade 
UINT8 Mode_ls;					///<Mode_ls
UINT8 SR_a_local_scale; 		///<SR_a local scale 
UINT8 SR_b_local_scale ;		///<SR_b local scale 
UINT8 PP_X_filter_coefficient ; ///<PP_X filter coefficient 
UINT8 PP_Y_filter_coefficient ; ///<PP_Y filter coefficient 
UINT8 PP_global_scale;			///<PP_global scale 
UINT8 PP_local_scale;			///<PP_local scale
}
VBE_SR_CMN_T;

typedef struct
{
UINT8 command;					///< cotrol 0 : OFF, 1 :low , 2: midium, 3:high 
UINT8 SR_a_global_scale;			///<PP_global scale 
UINT8 SR_b_global_scale;			///<PP_local scale
}
VBE_SR_CON_T;
/*-----------------------------------------------------------------------------
	Static Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Global Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

DTV_STATUS_T HAL_VBE_DISP_SR_Initialize(void);
DTV_STATUS_T HAL_VBE_DISP_SR_Uninitialize(void);
DTV_STATUS_T HAL_VBE_DISP_SR_Open(void);
DTV_STATUS_T HAL_VBE_DISP_SR_Close(void);

DTV_STATUS_T HAL_VBE_DISP_SR_SetSRControl(VBE_SR_CON_T sControl);
DTV_STATUS_T HAL_VBE_DISP_SR_SetSRParam(VBE_SR_CMN_T sSRParam);
DTV_STATUS_T HAL_VBE_DISP_SR_GetSRFirmwareVersion(UINT8 *pVersion);
DTV_STATUS_T HAL_VBE_DISP_SR_Debug(void);

DTV_STATUS_T HAL_VBE_DISP_SR_SetSROnOff(BOOLEAN bOnOff);
DTV_STATUS_T HAL_VBE_DISP_SR_SetSRDBGrade(UINT8 u8DBGrade);
DTV_STATUS_T HAL_VBE_DISP_SR_SetSRStrength(UINT8 u8Strength);
#endif /* _HAL_VBE_DISP_SR_H_ */
