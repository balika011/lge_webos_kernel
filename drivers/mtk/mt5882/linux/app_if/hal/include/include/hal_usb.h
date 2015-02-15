/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file hal_usb.h
 *
 *  This header file declares the data types and DDI functions to control the USB
 *
 *  @author Shinhoe Kim (shnihoe.kim@lge.com)
 *  @version  1.0
 *  @date   2013.12.18
 *  @note   Additional information.
 *  @see
 */

/******************************************************************************
  Header File Guarder
******************************************************************************/
#ifndef _HAL_USB_H_
#define _HAL_USB_H_

/******************************************************************************
  File Inclusions
******************************************************************************/
//#include "usb_kadp.h"
#include "hal_common.h"
/******************************************************************************
  Constant Definitions
******************************************************************************/

/******************************************************************************
  Macro Definitions
******************************************************************************/

/******************************************************************************
  Type Definitions
******************************************************************************/


/******************************************************************************
  Function Declaration
******************************************************************************/
DTV_STATUS_T HAL_USB_Open(void);
DTV_STATUS_T HAL_USB_OvpSetting(void);
/*USB 3.0 Enable,Disable*/
DTV_STATUS_T HAL_USB_SuperSpeedPortUp(UINT32 busNum, UINT32 portNum);
DTV_STATUS_T HAL_USB_SuperSpeedPortOff(UINT32 busNum, UINT32 portNum);
DTV_STATUS_T HAL_USB_SuperSpeedVBusControl(BOOLEAN bOn, UINT32 busNum, UINT32 portNum);
DTV_STATUS_T HAL_USB_SuperSpeedDisable(UINT32 busNum, UINT32 portNum);
UINT32 HAL_USB_SuperSpeedGetPortStatus(BOOLEAN bSSport,UINT32 busNum, UINT32 portNum);
/* Dummy functions to prevent compatability issue for webos 1.0 */
DTV_STATUS_T HAL_USB_RecoveryUSB30(void);
DTV_STATUS_T HAL_USB_SuperSpeedDCControl(BOOLEAN bOn);
DTV_STATUS_T HAL_USB_SuperSpeedVBusControl(BOOLEAN bOn,UINT32 busNum,UINT32 portNum);
#endif  /* _HAL_USB_H_ */

