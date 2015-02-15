///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <HDMI_Tx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#include "HDMI_Tx.h"

SYS_STATUS hdmi_tx_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data)
{
	iTE_u8		ucRdData;

	if(InvMask){
		if(~InvMask){
			iTE_i2c_read_byte(HDMITxAddr, Offset, 1, &ucRdData, HDMI_Tx_I2C_Dev);
			ucRdData &= (~InvMask);
			Data |= ucRdData;
		}
		iTE_i2c_write_byte(HDMITxAddr, Offset, 1, &Data, HDMI_Tx_I2C_Dev);
	}else{
		  if(~Offset){
		  	iTE_Sleep_ms(Data);
		  }else{
		  	//ToDo for other feature
		  }
	}

	return SYS_SUCCESS;
}
