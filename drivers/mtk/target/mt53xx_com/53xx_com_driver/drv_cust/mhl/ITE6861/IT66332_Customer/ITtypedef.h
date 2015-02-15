///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <typedef.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _ITTYPEDEF_H_
#define _ITTYPEDEF_H_


//////////////////////////////////////////////////
// data type
//////////////////////////////////////////////////
#define iTE_FALSE	0
#define iTE_TRUE		1

#ifndef _MCU_8051_
	typedef int iTE_u1 ;
	#define _CODE
	#define _IDATA
	#define _XDATA
	typedef	unsigned char	cBYTE;
#else
	typedef bit iTE_u1 ;
	#define _CODE code
	#define _IDATA idata
	#define _XDATA xdata
	typedef	code	unsigned char	cBYTE;
#endif // _MCU_8051_


typedef enum _SYS_STATUS {
    SYS_SUCCESS = 0,
    SYS_FAIL
//    SYS_RESERVED
} SYS_STATUS;

#ifndef NULL
	#define NULL ((void *) 0)
#endif

typedef char iTE_s8, *iTE_ps8;
typedef unsigned char iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;
typedef long iTE_s32, *iTE_ps32;
typedef unsigned long iTE_u32, *iTE_pu32;
#endif
