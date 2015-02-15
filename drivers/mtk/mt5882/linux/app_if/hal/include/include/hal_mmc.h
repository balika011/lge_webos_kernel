/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file hal_mmc.h
 *
 *  EMMC related header file
 *
 *  @author	Kwangseok Kim (kwangseok.kim@lge.com)
 *  @version	1.0
 *  @date		2012.10.15
 *  @note
 *  @see		hal_mmc.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _hal_mmc_h_
#define _hal_mmc_h_

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "hal_common.h"
#ifndef MTK_BAD
#include <asm-generic/ioctl.h>
#endif

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/
#define MMC_BLOCK_MAJOR		179
#define MMC_IOC_PON_ON _IO(MMC_BLOCK_MAJOR, 1)
#define MMC_IOC_PON_SHORT _IO(MMC_BLOCK_MAJOR, 2)
#define MMC_IOC_PON_LONG _IO(MMC_BLOCK_MAJOR, 3)
#define MMC_IOC_GET_EXTCSD _IO(MMC_BLOCK_MAJOR, 4)
#define MMC_IOC_VENDOR_CMD _IO(MMC_BLOCK_MAJOR, 5)
#define MMC_IOC_READ_SINGLE _IO(MMC_BLOCK_MAJOR, 6)
#define MMC_IOC_GET_CID _IO(MMC_BLOCK_MAJOR, 7)
#define MMC_IOC_GEN_CMD_WR _IO(MMC_BLOCK_MAJOR, 8)
#define MMC_IOC_GEN_CMD_RD _IO(MMC_BLOCK_MAJOR, 9)
#ifndef MTK_BAD
#define MMC_IOC_READ_MULTIPLE _IO(MMC_BLOCK_MAJOR, 10)
#define MMC_IOC_GEN_CMD _IO(MMC_BLOCK_MAJOR, 11)
#define MMC_IOC_CLAIM_HOST _IO(MMC_BLOCK_MAJOR, 12)
#define MMC_IOC_RELEASE_HOST _IO(MMC_BLOCK_MAJOR, 13)
#define MMC_IOC_IOS_CLK_SET  _IO(MMC_BLOCK_MAJOR, 14)
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

DTV_STATUS_T HAL_MMC_Open(void);
DTV_STATUS_T HAL_MMC_Close(void);
DTV_STATUS_T HAL_MMC_PON_Short(void);
DTV_STATUS_T HAL_MMC_PON_Long(void);
DTV_STATUS_T HAL_MMC_Get_CID(unsigned char *buff);
DTV_STATUS_T HAL_MMC_Get_SMART_Report(unsigned char *buff, unsigned int *len);

#endif  /* _hal_mmc.h_ */

