///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT66332_IO.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _IT66332_IO_H
#define _IT66332_IO_H

#include "IT66332.h"

iTE_u16 iTE_BusyWait_ms(iTE_u16 usMsCnt);
void iTE_Sleep_ms(iTE_u16 usMsCnt);
iTE_u1 iTE_i2c_read_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8* p_data,iTE_u8 device );
iTE_u1 iTE_i2c_write_byte(iTE_u8 address,iTE_u8 offset,iTE_u8 byteno,iTE_u8* p_data,iTE_u8 device );
void iTE_ExtIntEn(iTE_u1 bEnable);
#endif
