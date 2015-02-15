/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		hal_ci.h
 *
 *  header of Common Interface DDI Fuction.
 *
 *
 *  @author		Hyunho Kim (hyunho747.kim@lge.com)
 *  @author
 *  @version	1.0
 *  @date		2014.07.21
 *  @date
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _VIP_HAL_H_
#define _VIP_HAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "hal_common.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
DTV_STATUS_T HAL_VIP_Init(SINT32 *pDeviceHandle, char *pcFirmPath);
DTV_STATUS_T HAL_VIP_Close(SINT32 *pDeviceHandle);
DTV_STATUS_T HAL_VIP_Finalize(SINT32 *pDeviceHandle);

DTV_STATUS_T HAL_VIP_Set_Format(SINT32 *pDeviceHandle,
								int func,
								int buf_idx,
								int width, int height, int format);
DTV_STATUS_T HAL_VIP_Del_Format(SINT32 *pDeviceHandle,
								int func,
								int buf_idx);

DTV_STATUS_T HAL_VIP_Set_Input_Buffer(SINT32 *pDeviceHandle,
									int func,
									int buf_idx,
									void *buffer,
									int buffer_size);

DTV_STATUS_T HAL_VIP_Del_Input_Buffer(SINT32 *pDeviceHandle,
									int func,
									int buf_idx);

DTV_STATUS_T HAL_VIP_Set_Output_Buffer(SINT32 *pDeviceHandle,
										int func,
										int buf_idx,
										void *buffer,
										int buffer_size);

DTV_STATUS_T HAL_VIP_Del_Output_Buffer(SINT32 *pDeviceHandle,
										int func,
										int buf_idx);

DTV_STATUS_T HAL_VIP_Run(SINT32 *pDeviceHandle, int func, int buf_idx);

DTV_STATUS_T HAL_VIP_Get_State(SINT32 *pDeviceHandle, void *pState,
							int func, int buf_idx);

DTV_STATUS_T HAL_VIP_Wait_Done(SINT32 *pDeviceHandle, int func, int buf_idx);

DTV_STATUS_T HAL_VIP_Run_Single(SINT32 *pDeviceHandle, int func,
								int width, int height, int format,
								void *ibuf, int isize,
								void *obuf, int osize);

DTV_STATUS_T HAL_VIP_GetLibraryVer(SINT32 *pDeviceHandle,
									unsigned int *VipVer);

DTV_STATUS_T HAL_VIP_SetGestureMode(SINT32 *pDeviceHandle,
									unsigned int _eGestureMode,
									unsigned int _nSupportGestureEvent,
									unsigned int _eCameraType);

DTV_STATUS_T HAL_VIP_SetScreenRes(SINT32 *pDeviceHandle, SINT32 width, SINT32 height);

#ifdef __cplusplus
}
#endif

#endif /*_VIP_HAL_H_ */
