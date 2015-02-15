///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _CEC_H_
#define _CEC_H_
//#include "CEC_org.h"
#include "CEC_typedef.h"

void cec_init(void);
iTE_u8 cecfirerdy( void );
iTE_u1 cec_Transmitter(void);
void cec_irq(void);
void cec_handler(void);
void CEC_SwitchPollingMsg(PollingMessage_StateType eState);
void LogicalAddressingHandler(void);
void SystemAudioControl(void);
iTE_u1 CheckUpDownPhyAdr(void);
void cec_decoder(void);
void PollingMessage(void);
void CECChangePort(void);
void SystemInfomationFeature(void);
void Switch_SystemAudioControl(SystemAudioControl_StateType State);
void ARCHandler(void);
void OneTouchPlayHandler(void);
void Switch_OneTouchPlayState(OneTouchPlay_StateType uState);
CEC_DEVICE_TYPE cec_GetDeviceType(iTE_u8 ucMyPhyAdr);
void switch_ARCState(ARC_StateType uState);
void CECUICommandCode(iTE_u8 UIKey);
//****************************************************************************
void SwitchHECState(HEC_StateType	uState);
SYS_STATUS CEC_RxPush(void);
SYS_STATUS CEC_RxPull(P_CEC_FRAME pRxCmd);
SYS_STATUS CEC_TxPush(P_CEC_FRAME pTxCmd);
SYS_STATUS CEC_TxPull(void);
iTE_u1 CEC_QueueFull(pCEC_QBuf pCECBuf);
iTE_u1 CEC_QueueEmpty(pCEC_QBuf pCECBuf);
SYS_STATUS cec_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data);
SYS_STATUS cec_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS cec_w(iTE_u8 Offset, iTE_u8 Data);
SYS_STATUS	cec_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
iTE_u8 cec_r(iTE_u8 Offset);

#endif
