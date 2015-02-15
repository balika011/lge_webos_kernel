///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <HDMI_Rx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _HDMI_RX_H
#define	_HDMI_RX_H

#include "../IT66332_Customer/IT66332_IO.h"


#ifdef _HDMI_RX_INIT_TAB_
static iTE_u8 _CODE HDMI_RX_INIT_TAB[][3] = {
	{0x41, 0xff, MHLRxAddr+1},
     	{0x42, 0x30, (EnH2VAutoRst<<5)+(EnSkewFailRst<<4)},
	{0x16, 0xff, 0xff},
	{0x17, 0xff, 0x0f},
	{0x13, 0xff, 0xff},
	{0x14, 0xff, 0xff},
	{0x73, 0x20, EnHDCP1p1<<5},
};
#define HDMI_RX_INIT_TAB_SIZE	sizeof(HDMI_RX_INIT_TAB)/3
#endif

void hdmi_rx_init(void);
void hdmi_rx_irq(void);
SYS_STATUS hdmi_rx_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data);
SYS_STATUS hdmi_rx_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS hdmi_rx_w(iTE_u8 Offset, iTE_u8 Data);
SYS_STATUS	hdmi_rx_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
iTE_u8 hdmi_rx_r(iTE_u8 Offset);
#endif
