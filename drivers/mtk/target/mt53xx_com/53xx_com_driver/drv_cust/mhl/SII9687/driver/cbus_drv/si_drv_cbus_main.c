/******************************************************************************/
//!file     si_drv_cbus.c
//!brief    SiI9687 CBUS Driver.
//
// NOTE: This driver has a dependency on the Switch driver for the 9687
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2011, Silicon Image, Inc.  All rights reserved.
/******************************************************************************/

#include <linux/string.h>
#include "si_drv_cbus_internal.h"
#include "si_cbus_enums.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_drv_gpio.h"
#include "si_cbus_component.h"

//------------------------------------------------------------------------------
//  CBUS Driver Instance Data
//------------------------------------------------------------------------------

CbusDrvInstanceData_t cbusDrvInstance[SII_NUM_CBUS];
CbusDrvInstanceData_t *pDrvCbus = &cbusDrvInstance[0];

//------------------------------------------------------------------------------
//  CBUS Driver local Data
//------------------------------------------------------------------------------

static uint16_t cbusInitCbusRegsList [20] =
{
    REG_CBUS_DEVICE_CAP_1, 		MHL_VERSION,
    REG_CBUS_DEVICE_CAP_2,		MHL_DEV_CAT_POW,			// MHL PLIM0/PLIM1 needs to be set according to VBUS power it can supply
    REG_CBUS_DEVICE_CAP_3,      MHL_ADOPTER_SIMG_ID_H,
    REG_CBUS_DEVICE_CAP_4,      MHL_ADOPTER_SIMG_ID_L,
    REG_CBUS_DEVICE_CAP_5,		MHL_VID_LINK_MODE,
    REG_CBUS_DEVICE_CAP_6,		MHL_AUD_LINK_MODE,
    REG_CBUS_DEVICE_CAP_8,		MHL_LOGICAL_DEVICE_MAP,
    REG_CBUS_DEVICE_CAP_A,      MHL_FEATURE_SUPPORT,
};


//------------------------------------------------------------------------------
// Description: Set the active cbus port
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------
void SiiDrvCbusConfigure ( uint8_t port )
{
    pDrvCbus->port = port;
}

//------------------------------------------------------------------------------
// Description: Set the driver global data pointer to the requested instance.
// Parameters:  instanceIndex
// Returns:     true if legal index, false if index value is illegal.
//------------------------------------------------------------------------------

bool_t SiiDrvCbusInstanceSet ( uint_t instanceIndex )
{
    if ( instanceIndex < SII_NUM_SWITCH )
    {
        SiiRegInstanceSet( CBUS_PAGE, instanceIndex );
        pDrvCbus = &cbusDrvInstance[ instanceIndex];
        pDrvCbus->instanceIndex = instanceIndex;
        return( true );
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusInstanceGet
// Description: Return the current instance index.
// Parameters:  none
// Returns:     instance index.
//------------------------------------------------------------------------------

int_t SiiDrvCbusInstanceGet ( void )
{
    return( pDrvCbus->instanceIndex );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvCbusStatus
// Description: Returns a status flag word containing CBUS driver-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the CBUS Driver
//------------------------------------------------------------------------------

uint16_t SiiDrvCbusStatus ( void )
{
    uint16_t statusFlags;

    statusFlags = pDrvCbus->statusFlags;
    pDrvCbus->statusFlags &= ~SiiCBUS_INT;  // INT flag only valid first time it is read.

    return( statusFlags );
}

//------------------------------------------------------------------------------
// Description: Initialize the CBUS hardware for the current instance.
// Returns:     TRUE if no problem
//
// Note:        Requires that SiiDrvCbusInstanceSet() is called prior to this call
//------------------------------------------------------------------------------
bool_t SiiDrvCbusInitialize ( void )
{
    uint_t  instanceIndex, index;

    // Clear instance data
    instanceIndex = pDrvCbus->instanceIndex;        // Remember our instance
    memset( pDrvCbus, 0, sizeof( CbusDrvInstanceData_t ));
    pDrvCbus->instanceIndex = instanceIndex;        // Restore our instance

    // Setup local DEVCAP registers for read by the peer per rc7
    for ( index = 0; index < (sizeof( cbusInitCbusRegsList) / 2); index += 2 )
    {
        SiiRegWrite( cbusInitCbusRegsList[ index], cbusInitCbusRegsList[ index + 1] );
    }

    // Enable the VS commands, all interrupts, and clear legacy
    SiiRegWrite( REG_CBUS_INTR_0_MASK, 0xFF );      // Enable desired interrupts
    SiiRegWrite( REG_CBUS_INTR_1_MASK, 0x4C );      // Enable desired interrupts

    return( true );
}
