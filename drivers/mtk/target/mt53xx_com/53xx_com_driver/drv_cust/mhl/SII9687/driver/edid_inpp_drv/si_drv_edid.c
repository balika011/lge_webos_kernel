//***************************************************************************
//!file     si_drv_edid.c
//!brief    Silicon Image EDID driver.  Contains functions for reading and
//          writing Silicon Image device NVRAM and port SRAMS, as well as
//          boot data.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
//#include "malloc.h"         // For malloc/free
#include "si_common.h"

#include "si_edid_component.h"

#include "si_drv_internal.h"
#include "si_drv_device.h"
#include "si_drv_edid.h"

//------------------------------------------------------------------------------
//  Module Data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Function:       NvramCommand
//  Description:    Execute the passed NVRAM command and wait for done bit.
//------------------------------------------------------------------------------

static bool_t NvramCommand ( uint8_t command )
{
    uint8_t     test;
    SiiTimer_t  timeoutTimer;

        /* Start the NVRAM program operation and wait for it to finish. */

    SiiRegWrite( REG_NVM_COMMAND, command );
    SiiOsTimerSet( &timeoutTimer, 1 );
    for ( ;; )
    {
        test = SiiRegRead( REG_NVM_COMMAND_DONE );
        if ( test & BIT_NVM_COMMAND_DONE )
            break;
        if ( SiiOsTimerElapsed( timeoutTimer ) >= 4000 )    // 4 second timeout
        {
            return( false );
        }
    }
    return( true );
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidNvramRead
//  Description:    Read data from the NVRAM of the specified type.
//                  The type determines the amount of data written to
//                  the pDest array.
//  Parameters:     nvramType - EDID_NVRAM, specifies 256 byte main NVRAM
//                              EDID_NVRAM_VGA, specifies 128 byte VGA EDID
//                              EDID_NVRAM_BOOT, specifies 64  byte Boot Data
//------------------------------------------------------------------------------

bool_t SiiEdidNvramRead ( uint_t nvramType, uint8_t *pDest )
{
    bool_t      success = false;
    uint8_t     sramType, nvramCommand;

    switch ( nvramType )
    {
    case EDID_NVRAM:
        sramType        = EDID_RAM_0;
        nvramCommand    = VAL_COPY_EDID;
        break;

//    case EDID_NVRAM_VGA:
//        sramType        = EDID_RAM_0;
//        nvramCommand    = VAL_COPY_VGA;
//        break;

    case EDID_NVRAM_BOOT:
        sramType        = EDID_RAM_BOOT;
        nvramCommand    = VAL_COPY_DEVBOOT;
        break;
    default:
        nvramCommand    = 0;
    }

    if ( nvramCommand != 0 )
    {
        // Copy the NVRAM data into EDID ram 0.

        SiiRegWrite( REG_NVM_COPYTO, VAL_NVM_COPYTO_PORT0 );
        if ( NvramCommand( nvramCommand ))
        {
            success = SiiDrvEdidRamBlockRead( sramType, 0, pDest );

            // Read both blocks if EDID

            if ( nvramCommand == VAL_COPY_EDID)
            {
                success = SiiDrvEdidRamBlockRead( sramType, 1, (pDest + EDID_BLOCK_LEN) );
            }
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidNvramWrite
//  Description:    Program the passed data into the NVRAM of the specified
//                  type.  The type determines the amount of data read from
//                  the pSource array.
//  Parameters:     nvramType - EDID_NVRAM, specifies 256 byte main NVRAM
//                              EDID_NVRAM_VGA, specifies 128 byte VGA EDID
//                              EDID_NVRAM_BOOT, specifies 64  byte Boot Data
//------------------------------------------------------------------------------

bool_t SiiEdidNvramWrite ( uint_t nvramType, const uint8_t *pSource )
{
    uint8_t     sramType, nvramCommand;

    switch ( nvramType )
    {
    case EDID_NVRAM:
        sramType        = EDID_RAM_0;
        nvramCommand    = VAL_PRG_EDID;
        break;

//    case EDID_NVRAM_VGA:
//        sramType        = EDID_RAM_0;
//        nvramCommand    = VAL_PRG_VGA;
//        break;

    case EDID_NVRAM_BOOT:
        sramType        = EDID_RAM_BOOT;
        nvramCommand    = VAL_PRG_DEVBOOT;
        break;
    default:
        nvramCommand    = 0;
    }

    if ( nvramCommand != 0 )
    {
        // Load the source data into the appropriate SRAM
        SiiDrvEdidRamBlockWrite( sramType, 0, pSource );
        if ( nvramType == EDID_NVRAM )
        {
            SiiDrvEdidRamBlockWrite( sramType, 1, pSource + EDID_BLOCK_LEN );
        }

        // Start the NVRAM program operation and wait for it to finish.

        return( NvramCommand( nvramCommand ));
    }

    return( false );
}

//------------------------------------------------------------------------------
//  Function:       SiiDrvEdidRamBlockRead
//  Description:    Read data from the specified port EDID RAM and offset.
//                  The portIndex value determines the amount of data
//                  written to the pDest array.
//  Parameters:     portIndex - 0x00-0x03       read 128 bytes at the specified
//                                              offset from port rams 0-3
//                              EDID_RAM_VGA    read 128 bytes from VGA port ram
//                              EDID_RAM_BOOT   read 64 bytes from Boot Data ram
//                  offset    - 0 or 1, specifies the 128-byte block offset
//                              within the port RAM space (256 bytes)
//------------------------------------------------------------------------------

bool_t SiiDrvEdidRamBlockRead ( uint_t portIndex, uint_t offset, uint8_t *pDest )
{
    uint8_t     ramBitSelect, fifoIndex;
    uint16_t	len;

    switch ( portIndex )
    {
    case EDID_RAM_0:
    case EDID_RAM_1:
    case EDID_RAM_2:
    case EDID_RAM_3:
        ramBitSelect    = BIT_SEL_EDID0 << portIndex;
        fifoIndex       = VAL_FIFO_ADDR_00 + (EDID_BLOCK_LEN * offset);
        len             = EDID_BLOCK_LEN;
        break;

    case EDID_RAM_VGA:
        ramBitSelect    = BIT_SEL_EDID_VGA;
        fifoIndex       = VAL_FIFO_ADDR_00;
        len             = EDID_VGA_TABLE_LEN;
        break;

    case EDID_RAM_BOOT:
        ramBitSelect    = BIT_SEL_DEVBOOT;
        fifoIndex       = VAL_FIFO_ADDR_00;
        len             = EDID_DEVBOOT_LEN;
        break;
    default:
        len = 0;
    }

    if ( len != 0 )
    {
        // Point to beginning of selected port EDID ram.
        SiiRegModify( REG_EDID_FIFO_SEL, VAL_SEL_EDID_MASK, ramBitSelect );
        SiiRegWrite( REG_EDID_FIFO_ADDR, fifoIndex );

        // Read it into the destination.
        SiiRegReadBlock( REG_EDID_FIFO_DATA, pDest, len );
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
//  Function:       SiiDrvEdidRamBlockWrite
//  Description:    Write an EDID block (128 bytes) to the specified port RAM
//                  and offset.
//  Description:    Write data to the specified port EDID RAM and offset.
//                  The portIndex value determines the amount of data
//                  read from the pSource array.
//  Parameters:     portIndex - 0x00-0x04       write 128 bytes to the specified
//                                              offset in port rams 0-4
//                              EDID_RAM_VGA    write 128 bytes to VGA port ram
//                              EDID_RAM_BOOT   write 64 bytes to Boot Data ram
//                  offset    - 0 or 1, specifies the 128-byte block offset
//                              within the port RAM space (256 bytes)
//------------------------------------------------------------------------------

bool_t SiiDrvEdidRamBlockWrite ( uint_t portIndex, uint_t offset, const uint8_t *pSource )
{
    uint8_t     ramBitSelect, fifoIndex;
    uint16_t	len;

    switch ( portIndex )
    {
    case EDID_RAM_0:
    case EDID_RAM_1:
    case EDID_RAM_2:
    case EDID_RAM_3:
        ramBitSelect    = BIT_SEL_EDID0 << portIndex;
        fifoIndex       = VAL_FIFO_ADDR_00 + (EDID_BLOCK_LEN * offset);
        len             = EDID_BLOCK_LEN; //EDID_TABLE_LEN;
        break;

    case EDID_RAM_VGA:
        ramBitSelect    = BIT_SEL_EDID_VGA;
        fifoIndex       = VAL_FIFO_ADDR_00;
        len             = EDID_VGA_TABLE_LEN;
        break;

    case EDID_RAM_BOOT:
        ramBitSelect    = BIT_SEL_DEVBOOT;
        fifoIndex       = VAL_FIFO_ADDR_00;
        len             = EDID_DEVBOOT_LEN;
        break;
    default:
        len = 0;
    }

    if ( len != 0 )
    {
        // Point to beginning of selected port EDID ram.

        SiiRegModify( REG_EDID_FIFO_SEL, VAL_SEL_EDID_MASK, ramBitSelect );
        SiiRegWrite( REG_EDID_FIFO_ADDR, fifoIndex );

        // Write it from the passed array.

        SiiRegWriteBlock( REG_EDID_FIFO_DATA, pSource, len );
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
//  Function:       SiiDrvEdidBlockRead
//  Description:    Read a block from the selected EDID source into the passed array.
//------------------------------------------------------------------------------

bool_t SiiDrvEdidBlockRead ( uint8_t source, int_t blockIndex, uint8_t *pDest )
{
    uint8_t tempEdid[EDID_TABLE_LEN];
    bool_t success = true;

    // Boundary checking...
    if ( blockIndex >= (EDID_TABLE_LEN / EDID_BLOCK_LEN))
    {
        return( false );
    }

    switch (source)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_4:
            success = SiiDrvEdidRamBlockRead( source, blockIndex, pDest );
            break;
        case EDID_RAM_VGA:
        case EDID_RAM_BOOT:
            success = SiiDrvEdidRamBlockRead( source, 0, pDest );
            break;
        case EDID_NVRAM:
        case EDID_NVRAM_BOOT:
        case EDID_NVRAM_VGA:

            // The NVRAM read is always 256 bytes, so just give the caller
            // the part they want.

            success = SiiEdidNvramRead( source, tempEdid );
            if ( success )
            {
                memcpy( pDest, &tempEdid[ blockIndex * EDID_BLOCK_LEN], EDID_BLOCK_LEN );
            }
            break;

        default:
            success = false;
            break;
    }

    return( success );
}



//------------------------------------------------------------------------------
//  Function:       SiiDrvEdidCopyFromNvram
//  Description:    Force a boot load to get the new EDID data from the NVRAM to the chip.
//------------------------------------------------------------------------------

void SiiDrvEdidCopyFromNvram(void)
{
    SiiRegWrite( REG_BSM_INIT, BIT_BSM_INIT );
    SiiDrvDeviceBootComplete();
}


////------------------------------------------------------------------------------
////  Function:       SiiEdidLoadPortRam
////  Description:    Load the specified port ram from the specified source.
////
////  NOTE: This function does not lower or raise the Hot plug signal, so that
////        will have to be performed by the caller.
////------------------------------------------------------------------------------
//
//bool_t SiiEdidLoadPortRam ( uint8_t source, uint8_t port )
//{
//    bool_t success = true;
//
//    uint8_t *pData = malloc( EDID_TABLE_LEN );
//    if ( SiiEdidRead( source, pData ))
//    {
//        if ( port < EDID_RAM_VGA )
//        {
//            success = (SiiEdidUpdateHdmiData( pData, port ) != 0xFF );
//        }
//
//        /* Write to selected EDID ram.  */
//
//        if ( success )
//        {
//            EdidWritePortRam( port, pData );
//        }
//    }
//
//    free( pData );
//    return( success );
//}
