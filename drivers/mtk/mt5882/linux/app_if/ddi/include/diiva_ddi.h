/*
 *

DCL API Library and Host Interface device driver.
Copyright (C) 2010  Synerchip Co., Ltd, All Rights Reserved.

This program is public source software; you can redistribute it and/or
modify it under the terms of the DiiVA Public Source License as published
by Synerchip Co., Ltd.

This program is distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
KIND, EITHER EXPRESS OR IMPLIED, AND SYNERCHIP HEREBY DISCLAIMS ALL SUCH
WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
See the DiiVA Public Source License for more details.

You should have received a copy of the DiiVA DiiVA Public Source License
along with this program; if not, write to the

DiiVA Licensing LLC
880 Arques Avenue
Sunnyvale, CA 94085 USA
or go to www.diiva.org/public-source-licenses.html


 /** @file 		diiva_ddi.h
 *
 *  DDI Header File for DiiVA.
 *
 *
 *  @author	    jaeug.lew (jaeug.lew@lge.com)
 *  @author
 *  @version	1.0
 *  @date		2011.01.20
 *  @date
 *  @note		Just Load Headers
 *  @see
 */

#ifndef _DIIVA_DDI_H_
#define _DIIVA_DDI_H_

#ifndef RELEASE
#define HIF_DEBUG
#endif


/******************************************************************************
    전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include <osa_api.h> 
#include <common.h> 
#include "dcl_prot.h"	
#include "dcl_hif.h"
#include "dcl_api_defs.h" 
#include "dcl_system.h"
//#include "../bcm35230/cmnio/cmnio_gpio_map.h"
#include "gpio_index.h"
#include "micom_ddi.h"
#include "i2c_diiva.h"
  

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

#if !defined(TATOOINE)
#error TATOOINE not defined, i.e. dcl_system.h not incl ...
#endif


#ifndef CONFIG_DIIVA_I2C
#define CONFIG_DIIVA_I2C		1	//set Diiva I2C
#endif



#endif /*_DIIVA_DDI_H_ */
