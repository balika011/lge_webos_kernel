/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/
/** @file hal_gpio.h
 *
 *  This c file defines the DDI functions related to I2C Driver.
 *
 *  @author		dhjung(dhjung77@lge.com)
 *  @version	0.1
 *  @date		2008.02.12
 *  @note
 *  @see
 */

/*****************************************************************************
	Header File Guarder
*****************************************************************************/
#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*****************************************************************************
	#include File
*****************************************************************************/
#include "hal_common.h"

/*****************************************************************************
	Constant Definitions
*****************************************************************************/
extern SINT32 gHalGpioLogmFd;
#define HAL_GPIO_LOGM 						"hal-gpio"
#define HAL_GPIO_ERROR(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_ERROR, 	fmt, ##args)
#define HAL_GPIO_WARN(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_WARNING, fmt, ##args)
#define HAL_GPIO_NOTI(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_NOTI, 	fmt, ##args)
#define HAL_GPIO_INFO(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_INFO, 	fmt, ##args)
#define HAL_GPIO_DEBUG(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_DEBUG, 	fmt, ##args)
#define HAL_GPIO_TRACE(fmt,args...)		KADP_LOGM_PRINT(gHalGpioLogmFd, LX_LOGM_LEVEL_TRACE, 	fmt, ##args)





#endif	/* _HAL_GPIO_H_ */
