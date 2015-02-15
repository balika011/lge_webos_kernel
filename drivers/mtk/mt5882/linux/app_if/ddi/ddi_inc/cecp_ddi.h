/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file cecp_ddi.h
 *
 *  ADEC DD�� �����ϴ� DDI �Լ� header ����.
 *  AUDIO DDI�� ADEC DD�� �����ϴ� �Լ���� �ַ� AUDIO DDI�� task���� ȣ��ȴ�.
 *
 *  @author		Park Jeong-Gun(alwaysok@lge.com)
 *  @version	1.0
 *  @date		2006.04.24
 *  @note
 *  @see		cecp_ddi.c
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _CECP_DDI_H_
#define _CECP_DDI_H_

/******************************************************************************
 #include ���ϵ� (File Inclusions)
******************************************************************************/
#include "branches.h"
#include "common.h"
#include "osa_api.h"
#include "global_configurations.h"

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

DTV_STATUS_T DDI_CECP_SetMicomReady(void);

#endif /* _CECP_DDI_H_ */

