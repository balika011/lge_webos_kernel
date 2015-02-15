///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <debug.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _DEBUG_H_
#define _DEBUG_H_

#define iTE_Msg_Print		(1)
#define iTE_SW_Msg_Print	(1)
#define iTE_EDID_Msg_Print	(1)
#define iTE_HRx_Msg_Print	(1)
#define iTE_MRx_Msg_Print	(1)
#define iTE_CEC_Msg_Print	(1)

#if iTE_CEC_Msg_Print
	#define iTE_CEC_Msg(x)  			printf x
#else
	#define iTE_CEC_Msg(x)
#endif

#if iTE_Msg_Print
	#define iTE_Msg(x)					printf x
#else
	#define iTE_Msg(x)
#endif

#if iTE_SW_Msg_Print
	#define iTE_SW_Msg(x)				printf x
#else
	#define iTE_SW_Msg(x)
#endif

#if iTE_EDID_Msg_Print
	#define iTE_EDID_Msg(x)				printf x
#else
	#define iTE_EDID_Msg(x)
#endif

#if iTE_HRx_Msg_Print
	#define iTE_HRx_Msg(x)				printf x
#else
	#define iTE_HRx_Msg(x)
#endif

#if iTE_MRx_Msg_Print
	#define iTE_MRx_Msg(x)				printf x
#else
	#define iTE_MRx_Msg(x)
#endif

#endif // _DEBUG_H_
