/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file uart_ddi.h
 *
 *  This header file declares the data types and DDI functions to control the SPI(serial peripheral interface)
 *
 *  @author	Kyoungwon Park (ryan2808@lge.com) 
 *  @version	2.0
 *  @date		2009.01.15
 *  @date		2009.11.19
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _MMC_DDI_H_
#define _MMC_DDI_H_

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
#define MMC_BLOCK_MAJOR		179
#define MMC_IOC_PON_ON _IO(MMC_BLOCK_MAJOR, 1)
#define MMC_IOC_PON_SHORT _IO(MMC_BLOCK_MAJOR, 2)
#define MMC_IOC_PON_LONG _IO(MMC_BLOCK_MAJOR, 3)
#define MMC_IOC_GET_EXTCSD _IO(MMC_BLOCK_MAJOR, 4) 

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
#if (PLATFORM_TYPE == LG_PLATFORM)

extern DTV_STATUS_T DDI_MMC_OPEN();
extern DTV_STATUS_T DDI_MMC_PON_Get_EXTCSD();
extern DTV_STATUS_T DDI_MMC_PON_SHORT();
extern DTV_STATUS_T DDI_MMC_PON_LONG();
extern DTV_STATUS_T DDI_MMC_CLOSE();

#endif

#endif  /* _UART_DDI_H_ */


