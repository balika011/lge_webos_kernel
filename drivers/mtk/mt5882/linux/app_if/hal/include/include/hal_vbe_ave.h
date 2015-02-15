
/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_vbe_ave.h
 *
 *  It is the header file for controling VIDEO function.
 *  The header of VIDEO DDI is controlled by this file.
 *
 *  @author	yongsu.yoo(yongsu.yoo@lge.com)
 *  @version	0.1
 *  @date		2013.03.11
 *  @note
 *  @see
 */

/*-----------------------------------------------------------------------------
	Header File Guarder
------------------------------------------------------------------------------*/
#ifndef _HAL_VBE_AVE_H_
#define _HAL_VBE_AVE_H_

/*-----------------------------------------------------------------------------
	Control Constants
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 	File Inclusions
------------------------------------------------------------------------------*/
#include "hal_common.h"


#ifdef	__cplusplus
	extern "C"
	{
#endif /* __cplusplus */

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
	typedef enum
	{
		VBE_AVE_VBI_PAL_TELETEXT = 0, /**< VBI teletxet */
		VBE_AVE_VBI_PAL_WSS,			/**< VBI wss */
		VBE_AVE_VBI_PAL_VPS,			/**< VBI vps */
		VBE_AVE_VBI_NTSC_CC608,		/**< VBI cc608 */
		VBE_AVE_VBI_NTSC_WSS, 		/**< VBI NTSC wss */ // 2013.04.25. yongsu.yoo
	//	VIDEO_DDI_VBI_DVB_TELETEXT,
	//	VIDEO_DDI_VBI_DVB_SUBTITLE,
	//	VIDEO_DDI_VBI_ATSC_CC608,
	//	VIDEO_DDI_VBI_ATSC_CC708,
		VBE_AVE_VBI_ALL,
		VBE_AVE_VBI_MAX_SERVICE,		/**< VBI max */
	} VBE_AVE_VBI_TYPE_T;

typedef enum
{
    VSC_SCALER0,
	VSC_SCALER1,
	TUNER_OUTPUT,
	VDEC0,	//LM15U
	VDEC1,	//LM15U
}VBE_AVE_INPUT_INDEX;


/*-----------------------------------------------------------------------------
	Global Type Definitions
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
	Static Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
DTV_STATUS_T	HAL_VBE_AVE_Initialize(void);
DTV_STATUS_T	HAL_VBE_AVE_Uninitialize(void);
DTV_STATUS_T	HAL_VBE_AVE_Open(void);
DTV_STATUS_T  	HAL_VBE_AVE_IsATVWatching(void);
DTV_STATUS_T	HAL_VBE_AVE_Close(void);
DTV_STATUS_T    HAL_VBE_AVE_SetATVWatching(VBE_AVE_INPUT_INDEX AVEInput, BOOLEAN bIsATVWatching);

DTV_STATUS_T  	HAL_VBE_AVE_Connect(VBE_AVE_INPUT_INDEX AVEInput);
DTV_STATUS_T	HAL_VBE_AVE_Disconnect(void);
	
extern DTV_STATUS_T HAL_VBE_AVE_VBI_SetInsert(VBE_AVE_VBI_TYPE_T type, UINT8 OnOff);
extern DTV_STATUS_T HAL_VBE_AVE_VBI_InsertData (VBE_AVE_VBI_TYPE_T type, UINT32 nrLine, UINT32 *pLines, UINT32 size, UINT8 *pData);
DTV_STATUS_T 		HAL_VBE_AVE_SetVCRWinBlank(BOOLEAN bOnOff);

/*-----------------------------------------------------------------------------
	Global Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Local Variables & Function Prototypes Declarations
------------------------------------------------------------------------------*/
typedef enum
{
	HAL_ATV_MODE_WATCHING,	///< Watching ATV or Recording (CVD Clamping ON)
	HAL_ATV_MODE_NOT_WATCHING,	///< CVD powerdown or ATV is not selected. (Buffer Clamping ON)
	HAL_ATV_MODE_NOT_DEFINED,	///< ATV Mode not defined
	HAL_ATV_MODE_MAX,
}	HAL_VBE_ATV_Mode_t;

#ifdef	__cplusplus
	}
#endif /* __cplusplus */

#endif /* _hal_vfe_ave_ */

