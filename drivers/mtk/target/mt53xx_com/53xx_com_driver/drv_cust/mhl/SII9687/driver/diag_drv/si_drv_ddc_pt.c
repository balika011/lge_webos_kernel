//***************************************************************************
//!file     si_drv_ddc_pt.c
//!brief    DDC passthrough driver
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"

#include "si_drv_internal.h"
#include "si_drv_device.h"
#include "si_drv_ddc_pt.h"

#define REG_I2C_PT_DDC_STALL_TIMER              0x944
#define REG_I2C_PT_DDC_STALL_CONF               0x945
//#define REG_I2C_PT_CTRL                         0x946
#define BIT_DDC_FLUSH_MODE                          BIT0
#define BIT_DDC_SERVICE_MODE                        0x00
#define BIT_DDC_PT_ENABLE                           BIT1
#define BIT_DDC_PT_PORT_SHIFT                       0x05
#define REG_I2C_DDC_FILTER_SEL                  0x970

//------------------------------------------------------------------------------
// Function:    SiiDrvDdcInitPassThrough
// Description: Set up/Initialize tye DDC pass through 
// Parameters: None
// Returns: None
//------------------------------------------------------------------------------

void  SiiDrvDdcInitPassThrough(void )
{
    SiiRegWrite(REG_I2C_DDC_FILTER_SEL, 0x02);
    SiiRegWrite(REG_I2C_PT_DDC_STALL_TIMER, 0x18);
    SiiRegWrite(REG_I2C_PT_DDC_STALL_CONF, 0x1D);

}

//------------------------------------------------------------------------------
// Function:    SiiDrvDdcPassThrough
// Description: Enable/Disable DDC control for service or flash mode
// Parameters:  ddcType - 0: Service Mode,
//                        1: Flash Mode,
// Parameters:  ddcPort -  0 to 4 for 5 ports
// Returns:
//------------------------------------------------------------------------------

void  SiiDrvDdcPassThrough(uint8_t ddcType, uint8_t ddcPort)
{
    uint8_t ddcVal = 0;

    SiiDrvDdcInitPassThrough();

    ddcVal |= (ddcType == DDC_PT_SERVICE_MODE) ? BIT_DDC_SERVICE_MODE : BIT_DDC_FLUSH_MODE;
    ddcVal |= (BIT_DDC_PT_ENABLE | (ddcPort << BIT_DDC_PT_PORT_SHIFT));

    SiiRegWrite(REG_I2C_PT_CTRL, ddcVal);
}

