//***************************************************************************

//!file     si_regio.h
//!brief    Silicon Image Device register I/O header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "drv_sil_if.h"

#include "si_datatypes.h"
#include "si_1292regs.h"

#define REGIO_SLAVE_PAGE_0          (SIL_DEV_PAGE_0)		// Main
#define REGIO_SLAVE_PAGE_1          (0x00)      
#define REGIO_SLAVE_PAGE_2          (0x66)      
#define REGIO_SLAVE_PAGE_3          (0x00)
#define REGIO_SLAVE_PAGE_4          (0x68)      
#define REGIO_SLAVE_PAGE_5          (0x00)
#define REGIO_SLAVE_PAGE_6          (0x00)
#define REGIO_SLAVE_PAGE_7          (0x00)
#define REGIO_SLAVE_PAGE_8          (SIL_DEV_PAGE_8)      // CEC
#define REGIO_SLAVE_PAGE_9          (0xA0)      // EDID
#define REGIO_SLAVE_PAGE_A          (0x64)      
#define REGIO_SLAVE_PAGE_B		   (0x90)      
#define REGIO_SLAVE_PAGE_C          (SIL_DEV_PAGE_C)      // CBUS
#define REGIO_SLAVE_PAGE_D          (0xD0)
#define REGIO_SLAVE_PAGE_E          (0xE8)
#define REGIO_SLAVE_PAGE_F          (0x00)
      
#define REGIO_SLAVE_PAGE_E          (0xE8)      
#define REGIO_SLAVE_PAGE_F          (0x00)

//-------------------------------------------------------------------------------
//  Silicon Image Device Register I/O Function Prototypes
//-------------------------------------------------------------------------------

uint8_t SiIRegioRead( uint16_t regAddr, uint8_t *udata);
uint8_t SiIRegioWrite( uint16_t regAddr, uint8_t value);
void    SiIRegioModify( uint16_t regAddr, uint8_t mask, uint8_t value);
