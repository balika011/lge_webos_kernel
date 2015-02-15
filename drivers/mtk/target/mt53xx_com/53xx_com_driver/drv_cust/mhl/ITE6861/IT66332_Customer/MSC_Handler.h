///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <MSC_Handler.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _CEC_HANDLER_H_
#define _CEC_HANDLER_H_

#ifdef _MSC_HANDLER_TAB_
static iTE_u8 _CODE SuppRCPCode[128]= {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,	// 0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 1
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,	// 2
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,	// 3
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,	// 4
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 5
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,	// 6
	0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 	// 7
};

static iTE_u8 _CODE SuppRAPCode[32] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	// 0
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	// 1
};
#endif

void Msc_Init(void);
void Msc_MsgHandler(void);
void Msc_MsgeHandler(MSC_STATUS_CODE eMsgStatus);

iTE_u1 Msc_RcpHandler(iTE_u8 ucRcpKey);
void Msc_RcpkHandler(iTE_u8 eRcpKey);
void Msc_RcpeHandler(RCP_ERROR eRcpError);
iTE_u1 Msc_RapHandler(RAP_ACTION_CODE eRapCmd);
void Msc_Rapk_Handler(RAP_ERROR eRapError);
iTE_u1 Msc_UcpHandler(iTE_u8 ucUcpCmd);

#if iTE_MRx_Msg_Print
void Msc_UcpkHandler(iTE_u8 ucUcpkCode);
void Msc_UcpeHandler(UCP_ERROR eUcpeStatus);
#endif
iTE_u1 Msc_WriteBurstDataFill(iTE_u8 ucOffset, iTE_u8 ucByteNo, iTE_u8 *pucData);
void Msc_RcpKeyParse( iTE_u8 ucRcpKey );
iTE_u1 Msc_UcpKeyParse( iTE_u8 ucUcpKey );

void Msc_3DProcess(MHL3D_STATE *e3DReqState);
#endif
