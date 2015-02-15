
/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @hdmisw_ddi.h
 *
 * This source file defines the DDI functions related to FRC9449H Device Driver control
 *
 *  @author	Lee, byung gul[ashton](lachesis@lge.com)
 *  @version	0.1
 *  @date		2007.07.20
 *  @note		Additional information.
 *  @see		hdmisw_ddi.h
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_HDMISW_DDI_H_
#define	_HDMISW_DDI_H_

#ifdef _HDMISW_DDI_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

/******************************************************************************
 #include ���ϵ� (File Inclusions)
******************************************************************************/
#ifndef WIN32
#include "common.h"
#else
#include "common_win32.h"
#endif

/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/

/******************************************************************************
   ���� ��� ���� (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    ���� �� ���� (Local Type Definitions)
******************************************************************************/

//080814_yongs //normally port number start from 1
typedef enum
{
	UI_HDMI_PORT1 =0x00,
	UI_HDMI_PORT2 =0x01,
	UI_HDMI_PORT3 =0x02,
	UI_HDMI_PORT4 =0x03,

	UI_HDMI_PORT_MAX
} UI_HDMI_PORT_TYPE_T;

/******************************************************************************
    ���� �� ���� (Global Type Definitions)
******************************************************************************/

/******************************************************************************
    Extern ���������� �Լ� prototype ����
    (Extern Variables & Function Prototype Declarations)
******************************************************************************/

/******************************************************************************
    Static ������ �Լ� prototype ����
    (Static Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    ���� ������ �Լ� prototype ����
    (Local Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    ���� ������ �Լ� prototype ����
    (Global Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
	�Լ� ���� (Function Definitions)
**********************************d********************************************/

INTERFACE DTV_STATUS_T DDI_HDMISW_Init(void);
INTERFACE DTV_STATUS_T DDI_HDMISW_SelectHDMI(UI_HDMI_PORT_TYPE_T eUiHdmiPort);
INTERFACE DTV_STATUS_T DDI_HDMISW_SelectHPD(UI_HDMI_PORT_TYPE_T eUiHdmiPort, UINT8 hpdctrl);
INTERFACE DTV_STATUS_T DDI_HDMISW_CheckConnectHDMI(UI_HDMI_PORT_TYPE_T eUiHdmiPort, UINT8 *pData);
INTERFACE DTV_STATUS_T DDI_HDMISW_EDIDDownload(void);
INTERFACE DTV_STATUS_T DDI_HDMISW_EDIDCompare(void);
INTERFACE DTV_STATUS_T DDI_HDMISW_EDIDErase(void);

INTERFACE void DDI_HDMISW_DEBUG(void);

#undef INTERFACE

#endif
