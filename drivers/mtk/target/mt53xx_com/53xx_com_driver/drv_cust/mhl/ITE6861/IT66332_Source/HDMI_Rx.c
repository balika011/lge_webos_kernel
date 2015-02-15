///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <HDMI_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#define _HDMI_RX_INIT_TAB_
#include "HDMI_Rx.h"
#include "HDMI_Switch.h"
#include "../IT66332_Customer/IT66332.h"


void hdmi_rx_init(void)
{
	iTE_u8 ucTemp;
	iTE_HRx_Msg((" hdmi_rx_init_S \n"));
	for(ucTemp=0; ucTemp < HDMI_RX_INIT_TAB_SIZE; ucTemp++){
		hdmi_rx_set(HDMI_RX_INIT_TAB[ucTemp][0],HDMI_RX_INIT_TAB[ucTemp][1], HDMI_RX_INIT_TAB[ucTemp][2]);
	}
	iTE_HRx_Msg((" hdmi_rx_init_E \n"));
}


void hdmi_rx_irq(void)
{
	iTE_u8	ucHRxReg13[2];
	iTE_u8	ucTemp;

	hdmi_rx_br(0x13, 2, ucHRxReg13);

	iTE_HRx_Msg(("hdmi_rx_irq status : 0x%x, 0x%x\n", (int)ucHRxReg13[0], (int)ucHRxReg13[1]));

	ucHRxReg13[1] &= hdmi_rx_r(0x17);		// FW work-around because Reg14[3:2] interrupt issue
	hdmi_rx_bw(0x13, 2, ucHRxReg13);
	iTE_HRx_Msg(("hdmi_rx_irq status : 0x%x, 0x%x\n", (int)ucHRxReg13[0], (int)ucHRxReg13[1]));
     if( ucHRxReg13[0]&0x01 ) {	iTE_HRx_Msg(("HDMIRX Pwr5VOn Interrupt ...\n"));     				}
     if( ucHRxReg13[0]&0x02 ) {	iTE_HRx_Msg(("HDMIRX Pwr5VOff Interrupt ...\n"));					}
     if( ucHRxReg13[0]&0x04 ) {	iTE_HRx_Msg(("HDMIRX Receive 3D VSIF InfoFrame Interrupt ...\n"));	}
	if( ucHRxReg13[0]&0x08 ) {	iTE_HRx_Msg(("HDMIRX HDCP Authentication Start Interrupt ...\n"));			}
	if( ucHRxReg13[0]&0x10 ) {	iTE_HRx_Msg(("HDMIRX HDCP Authentication Done Interrupt ...\n"));			}
	if( ucHRxReg13[0]&0x20 ) {
		iTE_HRx_Msg(("HDMIRX HDMI/DVI Mode Change Interrupt=>"));
		ucTemp = (hdmi_rx_r(0x10) & 0x10);
		if(ucTemp){
			iTE_HRx_Msg(("HDMI\n"));
		}else{
			iTE_HRx_Msg(("DVI\n"));
		}
		if((ucTemp == 0x00) && (hdmi_rx_r(0x12) & 0x01)){
			iTE_HRx_Msg(("HDMI/DVI change when HDCP on, reset HDCP function \n"));
			hdmi_sw_set(0x08, 0x01, 0x01);
			hdmi_sw_set(0x08, 0x01, 0x00);
		}
	}

     if( ucHRxReg13[0]&0x40 ) {
         if(hdmi_rx_r(0x85)&0x3F) {
             hdmi_rx_w(0x85, 0xFF);
             iTE_HRx_Msg(("HDMIRX ECC Error Interrupt !!!\n"));
         }
     }

     if( ucHRxReg13[0]&0x80 ) {	iTE_HRx_Msg(("HDMIRX HDCP Off Interrupt ...\n"));					}
     if( ucHRxReg13[1]&0x01 ) {	iTE_HRx_Msg(("HDMIRX HDMI/MHL Mode Change Interrupt ...\n"));		}
     if( ucHRxReg13[1]&0x02 ) {	iTE_HRx_Msg(("HDMIRX Ctrl Code Error Interrupt !!!\n"));			}
     if( ucHRxReg13[1]&0x04 ) {	iTE_HRx_Msg(("HDMIRX H2VFIFO Overflow Interrupt !!!\n"));     		}
     if( ucHRxReg13[1]&0x08 ) {	iTE_HRx_Msg(("HDMIRX DeSkew Error Interrupt !!!\n"));     			}
}
//****************************************************************************
SYS_STATUS hdmi_rx_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data)
{
	iTE_u8		ucRdData;

	if(InvMask){
		if(~InvMask){
			iTE_i2c_read_byte(HDMIRxAddr, Offset, 1, &ucRdData, HDMI_Rx_I2C_Dev);
			ucRdData &= (~InvMask);
			Data |= ucRdData;
		}
		iTE_i2c_write_byte(HDMIRxAddr, Offset, 1, &Data, HDMI_Rx_I2C_Dev);
	}else{
		  if(~Offset){
		  	iTE_Sleep_ms(Data);
		  }else{
		  	//ToDo for other feature
		  }
	}

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS hdmi_rx_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_write_byte(HDMIRxAddr, Offset, Count, pData, HDMI_Rx_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS hdmi_rx_w(iTE_u8 Offset, iTE_u8 Data)
{
	iTE_i2c_write_byte(HDMIRxAddr, Offset, 1, &Data, HDMI_Rx_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS	hdmi_rx_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_read_byte(HDMIRxAddr, Offset, Count, pData, HDMI_Rx_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
iTE_u8 hdmi_rx_r(iTE_u8 Offset)
{
	iTE_u8 ucRdData;
	iTE_i2c_read_byte(HDMIRxAddr, Offset, 1, &ucRdData, HDMI_Rx_I2C_Dev);
	return ucRdData;
}

