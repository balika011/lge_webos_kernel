/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *---------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: digiamp_others.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file digiamp_others.h
 *  Brief of file digiamp_others.h. \n
 */

#ifndef DIGIAMP_TAS5711_H
#define DIGIAMP_TAS5711_H

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------

#include "x_typedef.h"
#include "aud_if.h"

//---------------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------------

//TAS5711
#define SIF_CLK_DIV							0x100

#define TAS5711_ADDR						0x34

#define TAS5711_SYS_CONTROL1				0x03
#define TAS5711_SERIAL_DATA				0x04
#define TAS5711_SYS_CONTROL2				0x05
#define TAS5711_SOFT_MUTE					0x06
#define TAS5711_MASTER_VOLUME				0x07
#define TAS5711_CH1_VOLUME				0x08
#define TAS5711_CH2_VOLUME				0x09
#define TAS5711_CH3_VOLUME				0x0a
#define TAS5711_VOLUME_CONFIG				0x0e
#define TAS5711_MODULATION_LIMIT			0x10
#define TAS5711_IC_DELAY_CH1				0x11
#define TAS5711_IC_DELAY_CH2				0x12
#define TAS5711_IC_DELAY_CH3				0x13
#define TAS5711_IC_DELAY_CH4				0x14
#define TAS5711_START_STOP_PERIOD			0x1a
#define TAS5711_TRIM						0x1b
#define TAS5711_BKND_ERR					0x1c

#define TAS5711_INPUT_MUX					0x20
#define TAS5711_CH4_SOURCE_SELECT           0x21
#define TAS5711_PWM_OUT_MUX				0x25

#define TAS5711_DRC_ENERGY				0x3a
#define TAS5711_DRC_ATTACK				0x3b
#define TAS5711_DRC_DELAY					0x3c
#define TAS5711_DRC_THRESHOLD				0x40
#define TAS5711_DRC_COMPRESSION_RATIO	0x41
#define TAS5711_DRC_OFFSET					0x42

#define TAS5711_DRC2_ENERGY             0x3d
#define TAS5711_DRC2_ATTACK             0x3e
#define TAS5711_DRC2_DELAY              0x3f
#define TAS5711_DRC2_THRESHOLD          0x43
#define TAS5711_DRC2_COMPRESSION_RATIO  0x44
#define TAS5711_DRC2_OFFSET				0x45


#define TAS5711_DRC_CONTROL				0x46

#define TAS5711_BANK_SWITCH				0x50

#define TAS5711_CH1_OUPUT_MIXER         0x51
#define TAS5711_CH2_OUPUT_MIXER         0x52
#define TAS5711_CH4_OUPUT_MIXER         0x60
#define TAS5711_SUBCH_BIQUAD1_COEFF     0x5a
#define TAS5711_SUBCH_BIQUAD2_COEFF     0x5b


//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

extern AMP_T* _AMP_GetAmpTAS5711FunTbl(void);

#endif /* DIGIAMP_OTHERS_H */

