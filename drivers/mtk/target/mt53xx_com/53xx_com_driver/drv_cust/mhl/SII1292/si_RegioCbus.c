//***************************************************************************
//!file     si_regioCbus.c
//!brief    CBUS register I/O function wrappers.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

/***** #include statements ***************************************************/

#include "si_RegioCbus.h"
#include "si_regio.h"
#include "si_hal.h"
#include "si_datatypes.h"
#include "si_cp1292.h"
#include "si_cbusDefs.h"
#include "si_cbus_regs.h"
#include "si_cbus.h"
#include "si_apiRCP.h"
#include "si_apiRAP.h"
#include "si_apiMSC.h"
#include "si_apiConfigure.h"
#include "si_apiCbus.h"
#include "si_api1292.h"
#include "si_1292regs.h"
#include "drv_sil_if.h"

static uint8_t l_cbusPortOffsets [ 1 ] = { 0x00 };

//------------------------------------------------------------------------------
// Function:    SiIRegioCbusRead
// Description: Read a one byte CBUS register with port offset.
//              The register address parameter is translated into an I2C slave
//              address and offset. The I2C slave address and offset are used
//              to perform an I2C read operation.
//------------------------------------------------------------------------------

uint8_t SiIRegioCbusRead ( uint16_t regAddr, uint8_t port )
{
	uint8_t udata;
	SiIRegioRead((regAddr + l_cbusPortOffsets[ port]), &udata);
    return udata;
}

//------------------------------------------------------------------------------
// Function:    SiIRegioCbusWrite
// Description: Write a one byte CBUS register with port offset.
//              The register address parameter is translated into an I2C
//              slave address and offset. The I2C slave address and offset
//              are used to perform an I2C write operation.
//------------------------------------------------------------------------------

void SiIRegioCbusWrite ( uint16_t regAddr, uint8_t channel, uint8_t value )
{
    SiIRegioWrite( regAddr + l_cbusPortOffsets[ channel], value );
}

#if 0 //not used
//------------------------------------------------------------------------------
// Function:    SiIRegioCbusModify
// Description: Read/Write Modify CBUS channel 0 or CBUS channel 1
//------------------------------------------------------------------------------

void SiIRegioCbusModify ( uint16_t regAddr, uint8_t channel, uint8_t mask, uint8_t value )
{
    SiIRegioModify( regAddr + l_cbusPortOffsets[ channel], mask, value );
}

#endif
