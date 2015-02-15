///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT66332_IO.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#include "IT66332_IO.h"

#ifndef _MCU_8051_
#include "../hdmi_mhl_if_ite6633.h"

iTE_u16 iTE_BusyWait_ms(iTE_u16 usMsCnt)
{
#ifdef SYS_MHL_SUPPORT
	Delay_ms(usMsCnt-1);
#endif
	return usMsCnt;
}
void iTE_Sleep_ms(iTE_u16 usMsCnt)
{
#ifdef SYS_MHL_SUPPORT
	Delay_ms(usMsCnt);
#endif
}
iTE_u1 iTE_i2c_read_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8* p_data,iTE_u8 device )
{
#ifdef SYS_MHL_SUPPORT
	 return i2c_read_byte( address, offset, byteno, p_data, device );
#else
     return 0;
#endif
}
iTE_u1 iTE_i2c_write_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8* p_data,iTE_u8 device )
{
#ifdef SYS_MHL_SUPPORT
	return i2c_write_byte( address, offset, byteno, p_data, device );
#else
	return 0;
#endif
}

#ifdef SYS_MHL_SUPPORT
extern void vIntEn(INT32 En);
#endif
void iTE_ExtIntEn(iTE_u1 bEnable)	//*****
{
#ifdef SYS_MHL_SUPPORT
	vIntEn((INT32)bEnable);
#endif
}
#endif

