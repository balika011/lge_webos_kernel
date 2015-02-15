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

#ifndef _DMEM_PLAT_H_
#define _DMEM_PLAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/

/******************************************************************************
	상수 정의(Constant Definitions)
******************************************************************************/
#define DMEM_PLAT_MEMINFO_BLOCK_MAX_NUM		20
/******************************************************************************
	매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/
typedef struct tagDMEM_BLOCKINFO {
	size_t 	internalFragmentSize;
	int		unitCount;
	int		size;
	unsigned long	* callStack;
	unsigned long	  caller;
	char			  callerName[16];
}DMEM_BLOCKINFO_T;

typedef struct tagDMEMINFO {
	size_t totalSize;
	size_t allocSize;
	size_t freeSize;
	int	   allocChunkNum;

	DMEM_BLOCKINFO_T	* allocChunk;
	DMEM_BLOCKINFO_T	freeChunk[DMEM_PLAT_MEMINFO_BLOCK_MAX_NUM];
}DMEM_INFO_T;

typedef struct tagDMEM_PLAT_CONF
{
	void * (*pfDMemAlloc)(unsigned int nSize);
	void (*pfDMemFree)(void *pAddr);
	void (*pfDMemInfo)(DMEM_INFO_T * meminfo);
	int (*pfReclaimCallBack)(unsigned int size);
	int nReclaimRetry;
	int memInfoBlockSizeRatio;
	int memInfoBlockNum;
	int bInit;
	int logginglLevel; /* 0: no 1: caller 2: callstack 3:caller+callstack */
	int callStackDepth;
	int threshold;

}DMEM_PLAT_CONF_T;

/******************************************************************************
	함수 선언 (Function Declaration)
*******************************************************************************/
extern int DMEM_PLAT_Initialize(void);
extern DMEM_PLAT_CONF_T * DMEM_PLAT_GetConfig(void);


#ifdef __cplusplus
}
#endif

#endif  /* _DMEM_PLAT_H_ */
