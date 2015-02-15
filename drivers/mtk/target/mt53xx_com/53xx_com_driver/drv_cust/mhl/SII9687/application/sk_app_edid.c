//***************************************************************************
//!file     sk_app_edid.c
//!brief    Wraps board and device functions for the EDID component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include "si_edid_component.h"
#include "x_printf.h"


// Included here only because this is a 'special' app layer module



#if 0
#include "../platform/hal/eeprom/si_eeprom.h"
#include "sk_application.h"
#include "../component/edid_inp_proc/si_edid_component.h"

// Included here only because this is a 'special' app layer module

#include "../driver/sii9687_drv/si_drv_device.h"
#include "../driver/sii9687_drv/si_drv_internal.h"
#endif
//------------------------------------------------------------------------------
// Function:    SkAppInitEdid
// Description: Perform any board-level initialization required at the same
//              time as EDID component initialization
// Parameters:  none
// Returns:     none
//------------------------------------------------------------------------------

bool_t SkAppInitEdid ( void )
{
    Printf("\r\n  SkAppInitEdid..\n");

    return SiiEdidInitialize();
}


//------------------------------------------------------------------------------
//  Function:       SkAppEdidRead
//  Description:    Read the selected EDID source into the passed array.
//------------------------------------------------------------------------------

bool_t SkAppEdidRead ( uint8_t source, uint8_t *pDest )
{
    bool_t      success = true;
#if 0
    switch (source)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_VGA:
        case EDID_RAM_BOOT:
        case EDID_NVRAM:
        case EDID_NVRAM_BOOT:
        case EDID_NVRAM_VGA:
            success = SiiEdidRead( source, pDest );
            break;

        case EDID_EEPROM:
            success = SiiPlatformEepromReadBlock( EE_EDID_OFFSET, pDest, EDID_TABLE_LEN );
            break;
        case EDID_EEPROM_BOOT:
            success = SiiPlatformEepromReadBlock( EE_DEVBOOT_OFFSET, pDest, EDID_DEVBOOT_LEN );
            break;
        case EDID_EEPROM_VGA:
            success = SiiPlatformEepromReadBlock( EE_EDIDVGA_OFFSET, pDest, EDID_VGA_TABLE_LEN );
            break;
        case EDID0_EEPROM:
            success = SiiPlatformEepromReadBlock( EE_EDID0_OFFSET, pDest, EDID_TABLE_LEN );
            break;
        case EDID1_EEPROM:
            success = SiiPlatformEepromReadBlock( EE_EDID1_OFFSET, pDest, EDID_TABLE_LEN );
            break;
        case EDID2_EEPROM:
            success = SiiPlatformEepromReadBlock( EE_EDID2_OFFSET, pDest, EDID_TABLE_LEN );
            break;
        case EDID3_EEPROM:
            success = SiiPlatformEepromReadBlock( EE_EDID3_OFFSET, pDest, EDID_TABLE_LEN );
            break;
        default:
            Printf("\r\nnEDID Read source not supported: %d\n", (uint16_t)source );
            break;
    }
#endif
    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SkAppEdidWrite
//  Description:    Write the selected EDID target from the passed array.
//------------------------------------------------------------------------------

bool_t SkAppEdidWrite ( uint8_t target, const uint8_t *pSource )
{
    bool_t      success = true;
#if 0
    switch (target)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_VGA:
        case EDID_RAM_BOOT:
        case EDID_NVRAM:
        case EDID_NVRAM_BOOT:
        case EDID_NVRAM_VGA:
            success = SiiEdidWrite( target, pSource);
            break;

        case EDID_EEPROM:
        case EDID0_EEPROM:
            success = SiiPlatformEepromWriteBlock( EE_EDID_OFFSET, pSource, 64 );    //EDID_TABLE_LEN );
            SiiPlatformEepromWriteByte( EE_EDID_VALID, CONFIG_VALID );
            break;
        case EDID_EEPROM_BOOT:
            success = SiiPlatformEepromWriteBlock( EE_DEVBOOT_OFFSET, pSource, EDID_DEVBOOT_LEN );
            SiiPlatformEepromWriteByte( EE_DEVBOOTDATA_VALID, CONFIG_VALID );
            break;
        case EDID_EEPROM_VGA:
            success = SiiPlatformEepromWriteBlock( EE_EDIDVGA_OFFSET, pSource, EDID_VGA_TABLE_LEN );
            SiiPlatformEepromWriteByte( EE_EDIDVGA_VALID, CONFIG_VALID );
            break;
        case EDID1_EEPROM:
            success = SiiPlatformEepromWriteBlock( EE_EDID1_OFFSET, pSource, EDID_TABLE_LEN );
            SiiPlatformEepromWriteByte( EE_EDID1_VALID, CONFIG_VALID );
            break;
        case EDID2_EEPROM:
            success = SiiPlatformEepromWriteBlock( EE_EDID2_OFFSET, pSource, EDID_TABLE_LEN );
            SiiPlatformEepromWriteByte( EE_EDID2_VALID, CONFIG_VALID );
            break;
        case EDID3_EEPROM:
            success = SiiPlatformEepromWriteBlock( EE_EDID3_OFFSET, pSource, EDID_TABLE_LEN );
            SiiPlatformEepromWriteByte( EE_EDID3_VALID, CONFIG_VALID );
            break;
        default:
            DEBUG_PRINT( MSG_DBG,  "\n-- EDID Write target not supported: %d --\n", (uint16_t)target );
            break;
    }
#endif
    return( success );
}

