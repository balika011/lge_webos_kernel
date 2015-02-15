	/******************************************************************************

    LGE. DTV RESEARCH LABORATORY
    COPYRIGHT(c) LGE CO.,LTD. 1998-2007. SEOUL, KOREA.
    All rights are reserved.
    No part of this work covered by the copyright hereon may be
    reproduced, stored in a retrieval system, in any form or
    by any means, electronic, mechanical, photocopying, recording
    or otherwise, without the prior permission of LG Electronics.

    FILE NAME   : xlibc.h
    VERSION     : 1.0
    AUTHOR      : Jackee, Lee(�����, jackee@lge.com)
    DATE        : 2007/02/22
	DESCRIPTION	: This header file is for eXtended LIBC functions.

*******************************************************************************/

#ifndef	 _XLIBC_H_
#define	_XLIBC_H_

/*-----------------------------------------------------------------------------
	���� ���
	(Control Constants)
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	#include ���ϵ�
	(File Inclusions)
------------------------------------------------------------------------------*/
#include 	<stdlib.h>

#include	"xlibc_api.h"
//#include	"common.h"
#include	"osadap.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*-----------------------------------------------------------------------------
	��� ����
	(Constant Definitions)
------------------------------------------------------------------------------*/
#define gDebugEnable xlibc_print_value
/*-----------------------------------------------------------------------------
	��ũ�� �Լ� ����
	(Macro Definitions)
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
	�� ����
	(Type Definitions)
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	�ܺ� ���������� �Լ� prototype ����
	(Extern Variables & Function Prototype Declarations)
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Terminal I/O Control Related O/S Wrapper
+-----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Timer operation Related O/S Wrapper
+-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
| Defines used in osamem.c
+-------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
| Functions in osamem.c
+-------------------------------------------------------------------------------*/
#define UartGetChar			xlibc_getc
#define StackTrace			xlibc_sym_backtrace_ex
#define HEXDUMP				hexdump
#define strupr				xlibc_strupr

extern void XLIBC_Init(void);
extern int 	XLIBC_OSD_Print( char *format, ...);
extern int 	str2indexInOpts(char **argv, const char *opts, int *output);
extern void reloadRegionList(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _XLIBC_H_ */



