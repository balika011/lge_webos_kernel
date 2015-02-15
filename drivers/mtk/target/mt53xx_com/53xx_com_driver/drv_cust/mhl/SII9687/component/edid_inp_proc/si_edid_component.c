//***************************************************************************
//!file     si_edid_component.c
//!brief    Silicon Image EDID Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_edid_component.h"
#include "si_drv_edid.h"

//------------------------------------------------------------------------------
//  Component Data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  EDID Component Instance Data
//------------------------------------------------------------------------------

SiiEdidInstanceData_t edidInstance[SII_NUM_EDID] =
{
{
	0,                          // structVersion
    SII_SUCCESS,                // lastResultCode Contains the result of the last API function called
    0,                          // statusFlags
}
};
SiiEdidInstanceData_t *pEdid = &edidInstance[0];

//------------------------------------------------------------------------------
// Function:    SiiEdidConfigure
// Description:
// Parameters:  none
// Returns:     true if the configuration was successful, or false if some
//              failure occurred
//------------------------------------------------------------------------------

bool_t SiiEdidConfigure ( void )
{

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiEdidInitialize
// Description:
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiEdidInitialize ( void )
{
    // Clear instance data
    memset( pEdid, 0, sizeof( SiiEdidInstanceData_t ));

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiEdidStatus
// Description: Returns a status flag word containing EDID-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the EDID Component
//------------------------------------------------------------------------------

bool_t SiiEdidStatus ( void )
{

    return( pEdid->statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiEdidStandby
// Description: Places the EDID component into standby mode if available
// Parameters:  none
// Returns:     true if the EDID component entered standby mode successfully,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiEdidStandby ( void )
{

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiEdidResume
// Description: Causes operation of the EDID component to return to the state
//				it was prior to the previous call to SiiEdidStandby.
// Parameters:  none
// Returns:     true if the EDID component successfully exited standby mode,
//				or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiEdidResume ( void )
{

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiEdidGetLastResult
// Description: Returns the result of the last SiiEdidxxx function called
// Parameters:  none
// Returns:     Result of last EDID component function.
//------------------------------------------------------------------------------

bool_t SiiEdidGetLastResult ( void )
{

    return( true );
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidValid
//  Description:    Validate the passed EDID.  If more than one block, test for
//                  HDMI information.
//------------------------------------------------------------------------------

bool_t SiiEdidValid ( const uint8_t *pData, bool_t isHdmi )
{
    uint8_t    i, edidOffset, accum;

    /* Check the checksum for the first EDID block. */

    accum       = 0;
    for ( i = 0; i < (EDID_TABLE_LEN / 2); i++ )
    {
        accum += *pData++;             // Gather checksum info
    }
    if ( accum != 0 )
    {
        return( false );
    }

    /* Check HDMI if requested. */

    edidOffset  = 0xFF;
    if ( isHdmi )
    {
        /* Locate 24-bit IEEE Registration number for Vendor-specific data. */
        /* For this search, I am not looking for CEA Extension or a Vendor- */
        /* specific data block.  Rather, I am assuming that the 24-bit      */
        /* IEEE code will be unique enough.                                 */

        /* NOTE: Because we are using an eight-bit index variable, this     */
        /*       loop ends at index 254.  For the phys address field this   */
        /*       is not a problem, and we add byte 255 to the checksum      */
        /*       after the loop.                                            */

    accum       = 0;
    for ( i = (EDID_TABLE_LEN / 2); i < (EDID_TABLE_LEN - 1); i++ )
    {
        if ( *pData == 0x03 )        // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( *(pData + 1) == 0x0C )
            {
                if ( *(pData + 2) == 0x00 )
                {
                    edidOffset = i + 3; // Point to CEC Phys Address field.
                }
            }
        }
        accum += *pData++;              // Gather checksum info
    }
    accum += *pData;    // Gather checksum info for last byte

    return( (( edidOffset != 0xFF ) && ( accum == 0 )) );
    }

    return(  accum == 0 );
}


//------------------------------------------------------------------------------
//  Function:       SiiEdidBootNvramReplace
//  Description:    Replace the checksum for all ports and set the physical address
//                 
//
//
//------------------------------------------------------------------------------
void  SiiEdidBootNvramReplace ( uint8_t *pSrc, uint8_t *pDst, SiiEdidInfo_t *editInfo)
{
    uint8_t i;
    uint8_t cs;

    for (i = 0; i < EDID_DEVBOOT_LEN; i++)
        pDst[i] = pSrc[i];

    pDst[editInfo->paOffset] = (editInfo->paH << 8) | editInfo->paL;

    cs = 0 - (editInfo->cs + (editInfo->paL + editInfo->paH));

    for (i = 0; i < SII_INPUT_PORT_COUNT; i++)
    {
        pDst[editInfo->csOffset + i] = 0 - (cs + ((i + 1) * 0x10));
        DEBUG_PRINT(MSG_DBG,"\nCS (%d): 0x%x", i, pDst[editInfo->csOffset + i]);
    }
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidGetData
//  Description:    search for the location of where the PA resides.
//                 
//
//  Returns: return the address of where the PA resides.
//
//------------------------------------------------------------------------------

bool_t SiiEdidGetData ( uint8_t *pData, SiiEdidInfo_t *editInfo)
{
    uint16_t i;

    for ( i =0; i < EDID_TABLE_LEN - 1; i++ )
    {
        /* Search for where is PA resides  */
        if ( pData[i] == 0x03 )               // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( (pData[i + 1]) == 0x0C )
            {
                if ( (pData[i + 2]) == 0x00 )
                {
                	editInfo->paOffset = (i + 3);
                	editInfo->paH = pData[i + 3];
                	editInfo->paL = pData[i + 4];
                	editInfo->cs = pData[0xFF];
                	return true;
                }
            }
        }
    }

    return false;
}


//------------------------------------------------------------------------------
//  Function:       SiiEdidUpdateHdmiData
//  Description:    Update the passed EDID array physical address (HDMI ext)
//                  and checksum.
//
//  Returns: Eight bit offset within EDID of physical address field, or 0xFF
//           if a failure occurs.
//
//  NOTE:   This function assumes that the HDMI physical address
//          of each port is 0x1000, 0x2000, 0x3000, 0x4000,
//          respectively
//------------------------------------------------------------------------------

uint8_t SiiEdidUpdateHdmiData ( uint8_t *pData, uint8_t port )
{
    uint8_t i, edidOffset, accum;

    /* Generate the checksum for the first EDID block.  */

    accum       = 0;
    for ( i = 0; i < ((EDID_TABLE_LEN / 2) - 1); i++ )
    {
        accum += *pData++;              // Gather checksum info
    }
    *pData++ = 0 - accum;

        /* Locate 24-bit IEEE Registration number for Vendor-specific data. */
        /* For this search, I am not looking for CEA Extension or a Vendor- */
        /* specific data block.  Rather, I am assuming that the 24-bit      */
        /* IEEE code will be unique enough.  This may not hold up.          */

    edidOffset  = 0xFF;
    accum       = 0;
    for ( i = (EDID_TABLE_LEN / 2); i < (EDID_TABLE_LEN - 1); i++ )
    {
        /* Add the physical address if signature is found,  */
        /* otherwise write the data without modification.   */

        if ( *pData == 0x03 )               // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( *(pData + 1) == 0x0C )
            {
                if ( *(pData + 2) == 0x00 )
                {
                    edidOffset              = i + 3;            // Point to CEC Phys Address field.
                    *(pData + 3)    = (port + 1) << 4;
                    *(pData + 4)    = 0;
                }
            }
        }
        accum += *pData++;                // Gather checksum info
    }
    *pData  = 0 - accum;

    return( (uint8_t)edidOffset );
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidRead
//  Description:    Read the selected EDID source into the passed array.
//------------------------------------------------------------------------------

bool_t SiiEdidRead( uint8_t source, uint8_t *pDest )
{
    bool_t success = true;

    switch (source)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_4:
        case EDID_RAM_5:
            if ( !SiiDrvEdidRamBlockRead( source, 0, pDest ))
            {
                success = false;
                break;
            }
            success = SiiDrvEdidRamBlockRead( source, 1, pDest + EDID_BLOCK_LEN );
            break;
        case EDID_RAM_VGA:
        case EDID_RAM_BOOT:
            success = SiiDrvEdidRamBlockRead( source, 0, pDest );
            break;
        case EDID_NVRAM:
        case EDID_NVRAM_BOOT:
        case EDID_NVRAM_VGA:
            success = SiiEdidNvramRead( source, pDest );
            break;
        default:
            success = false;
            break;
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidWrite
//  Description:    Write the selected EDID target from the passed array.
//------------------------------------------------------------------------------

bool_t SiiEdidWrite ( uint8_t target, const uint8_t *pSource )
{
    bool_t success = true;

    switch (target)
    {
        case EDID_RAM_0:
        case EDID_RAM_1:
        case EDID_RAM_2:
        case EDID_RAM_3:
        case EDID_RAM_4:
        case EDID_RAM_5:
            if ( !SiiDrvEdidRamBlockWrite( target, 0, pSource ))
            {
                success = false;
                break;
            }
            success = SiiDrvEdidRamBlockWrite( target, 1, pSource + EDID_BLOCK_LEN );
            break;
        case EDID_RAM_VGA:
        case EDID_RAM_BOOT:
            success = SiiDrvEdidRamBlockWrite( target, 0, pSource );
            break;
        case EDID_NVRAM:
        case EDID_NVRAM_BOOT:
        case EDID_NVRAM_VGA:
            success = SiiEdidNvramWrite( target, pSource );
            break;
        default:
            success = false;
            break;
    }

    return( success );
}

