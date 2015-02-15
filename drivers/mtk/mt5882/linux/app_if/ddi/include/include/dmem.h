/******************************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 1999 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 *****************************************************************************/

/** @file osa_api.h
 *
 *	POSIX thread based System Call API.
 *
 *	@author 	cskim
 *	@version	1.0
 *	@date		2005. 6. 1
 *	@note
 *	@see
 */

/******************************************************************************
	Header File Guarder
******************************************************************************/

#ifndef _DMEM_H_
#define _DMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "dmem_plat.h"

/******************************************************************************
	상수 정의(Constant Definitions)
******************************************************************************/
#define DMEM_MGR_PRIO_DEFAULT	DMEM_MGR_MIDDLE

#define DMEM_RECLAIM_ONCE					0
#define DMEM_RECLAIM_PERIODIC				(1<<1)

/******************************************************************************
	매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
typedef enum{
	DMEM_MGR_HIGH,
	DMEM_MGR_MIDDLE,
	DMEM_MGR_LOW,
}DMEM_MGR_PRIO_T;

typedef UINT32	DMEM_RECLAIM_T;
typedef int 	(*RECLAIM_CALLBACK_T)(DMEM_RECLAIM_T reclaimID,UINT32 memSize);
typedef struct DMEMOBJ_T *			DMEMOBJ_ID;

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
/*----------------------------------------------------------------------------
	Device Memory Reclaim 관리자 
-----------------------------------------------------------------------------*/
extern DMEM_RECLAIM_T	DMEM_MGR_RegisterReclaim	(char * pName, RECLAIM_CALLBACK_T pfCallback, UINT32 flags, DMEM_MGR_PRIO_T prio);
extern int 				DMEM_MGR_UnRegisterReclaim(DMEM_RECLAIM_T reclaimID);
extern int 				DMEM_MGR_RequestReclaim	(unsigned int size);

/*----------------------------------------------------------------------------
	Soc Device memory 의 malloc/free 및 현재 상태 정보를 dump
-----------------------------------------------------------------------------*/

extern void * 	DMEM_Alloc	(unsigned int nSize);
extern void   	DMEM_Free 	(void *pAddr);
extern DMEM_INFO_T * DMEM_AllocMemInfo (void);
extern void 	DMEM_FreeMemInfo (DMEM_INFO_T * pMemInfo);
extern void 	DMEM_DumpMemInfo	(int level);
/*----------------------------------------------------------------------------
	Device Memory를 System Memory로 변환해주는 utility
-----------------------------------------------------------------------------*/
DMEMOBJ_ID 	DMEM_CreateObject		(char * name, int size);
void 		DMEM_DestoryObject		(DMEMOBJ_ID objID);
int 		DMEM_CopyFromObject	(DMEMOBJ_ID objID, unsigned long offset, char * pDst, int length);
int 		DMEM_CopyToObject	(DMEMOBJ_ID objID, unsigned long offset, char * pSrc, int length);
int			DMEM_GetObjectSize		(DMEMOBJ_ID objID);
void 		DMEM_DumpObject		(DMEMOBJ_ID objID);


#ifdef __cplusplus
}
#endif

#endif  /* _DMEM_H_ */
