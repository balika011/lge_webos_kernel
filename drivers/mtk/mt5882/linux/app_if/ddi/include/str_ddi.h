/******************************************************************************
 *	 SW Center OSL Gr,  LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 *****************************************************************************/


/** @file str_ddi.h
 *
 *  This header file declares the data types and DDI functions to control the gpio ports
 *
 *  @author	Lee, Sangseok(sangseok.lee@lge.com)
 *  @version	0.1
 *  @date		2009.11.04
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _STR_DDI_H_
#define _STR_DDI_H_

/******************************************************************************
	#include ���ϵ� (File Inclusions)
******************************************************************************/
#include "common.h"

/******************************************************************************
 	��� ����(Constant Definitions)
******************************************************************************/

/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/

/******************************************************************************
	�� ���� (Type Definitions)
******************************************************************************/

/******************************************************************************
	�Լ� ���� (Function Declaration)
******************************************************************************/
extern DTV_STATUS_T DDI_STR_Init(void);
extern DTV_STATUS_T	DDI_STR_Exit(void);
extern DTV_STATUS_T	DDI_STR_Suspend(void);


#endif  /* _STR_DDI_H_ */

