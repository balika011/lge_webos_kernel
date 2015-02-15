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
#ifndef _UART_DDI_H_
#define _UART_DDI_H_

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
#if (PLATFORM_TYPE == MTK_PLATFORM)
#define	PDP_UART_PORT					2	// PDP UART2
#define	CPU_UART_PORT					1
#else
#define	PDP_UART_PORT					1
#define	CPU_UART_PORT					2
#endif

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
/**
* [L8] luma95   L8 uart는 총 3개의 UART가 있으며 모두 디버그 용도이다.
* 각 UART 마다 몇개씩의 모듈들이 MUXING 되어있으므로 아래 enum의 값을 가지고
* switching 해줘야 한다.
*
* @see	
*/
typedef enum
{
	TOP_UART0=1,
	TOP_UART1,
	TOP_UART2,
	TOP_MAX,
} TOP_UART_T;
/**
* [L8] luma95   L8 uart는 총 3개의 UART가 있으며 모두 디버그 용도이다.
* 각 UART 마다 몇개씩의 모듈들이 MUXING 되어있으므로 아래 enum의 값을 가지고
* switching 해줘야 한다.
*
* @see	
*/
typedef enum
{
	UART_CPU0=1,
	UART_SP0,
	UART_LED0,
	UART_SPP0,
	UART_CPU1,
	UART_SP1,
	UART_CPU2,
	UART_SPP1,
	UART_LED1,
} TOP_UART_TYPE_T;
#endif

#if (PLATFORM_TYPE == MTK_PLATFORM)
/**
* [MTK] Port portection 을 우해 준비해둠.
* 각 UART 마다 몇개씩의 모듈들이 MUXING 되어있으므로 아래 enum의 값을 가지고
* switching 해줘야 한다.
*
* @see	
*/
typedef enum{
    UART_PORT0 = 0,
    UART_PORT1,
    UART_PORT2,
	UART_MAX,
 }UART_DDI_PORT_T;	 
#endif

#if (PLATFORM_TYPE == MTK_PLATFORM)
extern DTV_STATUS_T DDI_UART_Open(UART_DDI_PORT_T portIdx);
#else
extern DTV_STATUS_T DDI_UART_Open(void);
#endif
extern void DDI_UART_Change(int uart_port);
extern void DDI_UART_ReadEnable(int readEnable);
#if (PLATFORM_TYPE == MTK_PLATFORM)
extern DTV_STATUS_T DDI_UART_Close(UART_DDI_PORT_T portIdx);
#else
extern DTV_STATUS_T DDI_UART_Close(void);
#endif
extern DTV_STATUS_T DDI_UART_ReadCheck(unsigned char *pCheckData, unsigned int  checkSize, unsigned int uart_port);
extern DTV_STATUS_T DDI_UART_Write(unsigned char *pWriteData, unsigned int  writeSize, unsigned int uart_port);
#if (PLATFORM_TYPE == MTK_PLATFORM)
extern int DDI_UART_ReadDataPrint(UART_DDI_PORT_T portIdx);
#else
extern void DDI_UART_ReadDataPrint(void);
#endif
#ifdef INCLUDE_NETFLIX
extern void DDI_UART_Enable(void);
extern void DDI_UART_Disable(void);
#endif

#if (PLATFORM_TYPE == MTK_PLATFORM)
extern int DDI_UART_SetPort(UART_DDI_PORT_T portIdx, long speed, char databit, char stopbit, char parity);
extern int DDI_UART_Change_Pinmux(UART_DDI_PORT_T portIdx);
#endif
#endif  /* _UART_DDI_H_ */

