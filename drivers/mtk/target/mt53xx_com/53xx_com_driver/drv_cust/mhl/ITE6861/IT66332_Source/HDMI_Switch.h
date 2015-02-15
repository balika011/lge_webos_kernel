///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <HDMI_Switch.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
//****************************************************************************
// HDMI_Switch.h
//
//****************************************************************************
#ifndef _HDMI_SWITCH_H
#define _HDMI_SWITCH_H

#include "../IT66332_Customer/IT66332_IO.h"


//#include "hdmisw_header.h"

#ifdef _HDMI_SW_INIT_TAB_
static iTE_u8 _CODE	HDMI_SW_INIT_TAB[][3] = {
//	{0x6e, 0x20, 0x20},
	{0x0A, 0x80, 0x00},
	{0x06, 0x80, 0x80},
	{0x06, 0xff, 0x01},
	{0x07, 0xff, 0x80},
	{0x06, 0x80, 0x80},
	//{0x3c, 0x0f, 0x00},
	{0x10, 0xff, DbgSel},
	{0x80, 0xff, HDMIRxAddr+1},
	{0x81, 0xff, HDMITxAddr+1},
	{0x82, 0xff, EDIDAddr+1},
	{0x83, 0xff, CECAddr+1},
	{0x0b, 0x03, (EnVCLKInv << 1) + EnCLKD5Inv},
	{0x89, 0xc0, (CBUSDrv << 6)},
	{0x8A, 0x40, (EnCBusSMT << 6)},
     	{0x8B, 0x03, (EnRxPwrPD<<1)+EnHPDPD},
	{0x0d, 0xff, 0xc7},
	{0x11, 0x70, (ForceWrUpd<<6) + (RCLKFreqSel << 4)},
	{0x12, 0x02, (DongleEDID<<1)},
	{0x1c, 0xff, 0x00},
	{0x1d, 0xff, 0x00},
	{0x1e, 0xff, (DisClkDiffInt << 6) + 0x04},
	{0x1f, 0xff, 0x00},
	{0x60, 0x30, 0x20},
	{0x63, 0x10, 0x00},
	{0x62, 0x0c, 0x0c},//(EnPTGenK706<<3) + 0x04},
#if (EnPTGenK706==iTE_FALSE)
	{0x65, 0x88, 0x80},
#endif

#if (EnMaxTxSwing)
	{0x66, 0x77, 0x77},
#endif
	{0x6e, 0x0f, (V18V << 2) + V12V},
	{0x39, 0xc0, RCLKDeltaSel << 6},
	{0x44, 0x09, (EnEDIDSeg<<3) + DisIntEDID},
	{0x36, 0x02, UseIPLOCK << 1},
	{0x0f, 0x01, EnTxHPD2RxHPD},
	{0x31, 0x07, (AutoPortChgTxAFETog<<2)+(AutoPortChgRxAFETog<<1)+AutoPortChgDetRst},
	{0x32, 0x07, (EnAutoCapRdy<<2)+(EnAutoPathEn<<1)+EnAutoHPD},
	{0x33, 0x3f, (AutoCapRdyDly<<4)+(AutoPathEnDly<<2)+AutoHPDDly},
//	{0x37, 0xf0, 0x10},					//V1.08_2
	{0x3c, 0x0f, 0x00},
};
#define HDMI_SW_INIT_TAB_SIZE sizeof(HDMI_SW_INIT_TAB)/3
#endif

void hdmi_sw_init(void);
void hdmi_sw_irq(void);
iTE_u1 ddcwait(void);
void hdmi_sw_EdidGet(iTE_u1 bChkTxHPD);
void hdmi_HpdLowAll(void);
void hdmi_HpdAutoAll(void);
SYS_STATUS hdmi_sw_set(iTE_u8 Offset, iTE_u8 Mask, iTE_u8 Data);
SYS_STATUS hdmi_sw_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS hdmi_sw_w(iTE_u8 Offset, iTE_u8 Data);
SYS_STATUS hdmi_sw_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
iTE_u8 hdmi_sw_r(iTE_u8 Offset);

#if WCLK_PRINT
void cal_wclk(iTE_u8 );
#endif
void cbus_fwwa(void);
void mhl_EnterPwrDn(void);
void mhl_ExitPwrDn(void);
void cal_oclk(void);

extern iTE_u8     AutoEQFailCnt;
#endif
