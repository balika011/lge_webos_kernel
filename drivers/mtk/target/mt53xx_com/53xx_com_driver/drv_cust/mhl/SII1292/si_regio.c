//***************************************************************************

//!file     si_regio.c
//!brief    Silicon Image Device register I/O support.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
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

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

// Register Slave Address page map

static uint8_t XDATA l_regioDecodePage [16] =
{
    REGIO_SLAVE_PAGE_0, REGIO_SLAVE_PAGE_1, REGIO_SLAVE_PAGE_2, REGIO_SLAVE_PAGE_3,
    REGIO_SLAVE_PAGE_4, REGIO_SLAVE_PAGE_5, REGIO_SLAVE_PAGE_6, REGIO_SLAVE_PAGE_7,
    REGIO_SLAVE_PAGE_8, REGIO_SLAVE_PAGE_9, REGIO_SLAVE_PAGE_A, REGIO_SLAVE_PAGE_B,
    REGIO_SLAVE_PAGE_C, REGIO_SLAVE_PAGE_D, REGIO_SLAVE_PAGE_E, REGIO_SLAVE_PAGE_F
};

//------------------------------------------------------------------------------
// Function:    SiIRegioRead
// Description: Read a one byte register.
//              The register address parameter is translated into an I2C slave
//              address and offset. The I2C slave address and offset are used
//              to perform an I2C read operation.
//------------------------------------------------------------------------------

uint8_t SiIRegioRead ( uint16_t regAddr, uint8_t *udata)
{
	return fgSil_Sif_ReadBytes(l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr, udata, 1);
}

//------------------------------------------------------------------------------
// Function:    SiIRegioWrite
// Description: Write a one byte register.
//              The register address parameter is translated into an I2C
//              slave address and offset. The I2C slave address and offset 
//              are used to perform an I2C write operation.
//------------------------------------------------------------------------------

uint8_t SiIRegioWrite ( uint16_t regAddr, uint8_t value )
{
	return fgSil_Sif_WriteBytes(l_regioDecodePage[ regAddr >> 8], (uint8_t)regAddr, &value, 1);
}

//------------------------------------------------------------------------------
// Function:    SiIRegioModify
// Description: Modify a one byte register under mask.
//              The register address parameter is translated into an I2C
//              slave address and offset. The I2C slave address and offset are
//              used to perform I2C read and write operations.
//
//              All bits specified in the mask are set in the register
//              according to the value specified.
//              A mask of 0x00 does not change any bits.
//              A mask of 0xFF is the same a writing a byte - all bits
//              are set to the value given.
//              When only some bits in the mask are set, only those bits are
//              changed to the values given.
//------------------------------------------------------------------------------

void SiIRegioModify ( uint16_t regAddr, uint8_t mask, uint8_t value)
{
	uint8_t abyte;
	
	SiIRegioRead(regAddr, &abyte);

	abyte &= (~mask);                                       //first clear all bits in mask
	abyte |= (mask & value);                                //then set bits from value
	SiIRegioWrite(regAddr, abyte);
}

