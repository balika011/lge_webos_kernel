//***************************************************************************
//!file     si_drv_device.h
//!brief    Sii9687 driver header.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_DRV_DEVICE_H__
#define __SI_DRV_DEVICE_H__
#include "si_datatypes.h"
#include "si_debug.h"
#include "si_sii9687_registers.h"
#include "si_regs_mhl9687.h"

typedef enum _SiiPipeTypes_t
{
    SiiPipeType_Main        = 0,
    SiiPipeType_Background  = 1
} SiiPipeTypes_t;

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceConfigure
// Description: Before it is initialized for use, the device must be properly
//              configured for the target system requirements.  This function
//              is called to configure the parts of the device that may not
//              directly fall under the auspices of one of the other components.
// Parameters:  none
// Returns:     true if the configuration was successful, or false if some
//              failure occurred
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceConfigure( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceInitialize
// Description: This function disables the device or places the device in
//              software reset if it does not power up in a disabled state.
//              It may be used to initialize registers that require a value
//              different from the power-up state and are common to one or
//              more of the other component modules.
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//
// Note:        The function leaves the device in disabled state until the
//              SiiDeviceRelease function is called.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceInitialize( void );

//------------------------------------------------------------------------------
// Function:    SiiDRVDeviceStatus
// Description: Returns a status flag word containing device-specific
//              information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit Device status flags word for the Device Component
//------------------------------------------------------------------------------

uint_t SiiDrvDeviceStatus( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceStandby
// Description: Places the device into standby mode if available
// Parameters:  none
// Returns:     true if the device entered standby mode successfully, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceStandby( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceResume
// Description: Causes operation of the device to return to the state it was
//              prior to the previous call to SiiDeviceStandby.
// Parameters:  none
// Returns:     true if the device successfully exited standby mode , or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceResume( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceRelease
// Description: Enables the device for normal operation and is called as the
//              last part of device initialization.
//              Since the SiiDeviceInitialize function leaves the device output
//              disabled, this function should enable the device output as a
//              minimum.  All other configuration and initialization functions
//              must be called in between SiiDeviceInitialize and SiiDeviceRelease.
// Parameters:  none
// Returns:     true if the release is successful and the device is ready for
//              operation or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceRelease( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceStart
// Description: Enables the device for normal operation.  This function performs
//              the procedures necessary for correct operation of the device after
//              all other initialization has been performed.
// Parameters:  none
// Returns:     true if the release is successful and the device is ready for
//              operation or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceStart ( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceGetLastResult
// Description: Returns the result of the last SiiDevicexxx function called
// Parameters:  none
// Returns:     Result of last device component function.
//------------------------------------------------------------------------------

enum
{
    SiiDevice_SUCCESS            = 0x00,
    SiiDevice_FAIL_DEVID_READ,
    SiiDevice_FAIL_WRONGDEVICE,
    SiiDevice_FAIL_BOOT,
    SiiDevice_FAIL_NVRAM_INIT,
};

int_t SiiDrvDeviceGetLastResult( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceBootComplete
// Description: Wait for EDID Boot Done status.
// Parameters:  none
// Returns:     true if device booted correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceBootComplete( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceBootInProgress
// Description: see if boot is in progress
// Parameters:  none
// Returns:     true if device booted correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceBootInProgress ( void );

//------------------------------------------------------------------------------
// Function:    SiiDrvDevicePowerUpBoot
// Description: Perform the device startup sequence.  Reset, wait for boot to
//              complete, optionally initialize EDID NVRAM, and initialize
//              device registers.
// Parameters:  none
// Returns:     true if device booted and initialized correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDevicePowerUpBoot( void );

//------------------------------------------------------------------------------
//  Function:       SiiDrvDeviceNvramInitialize
//  Description:    Check to see if the NVRAM has been initialized and do it if needed.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceNvramInitialize( bool_t forceInit );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceInfo
// Description: Return the requested information from the device.
//------------------------------------------------------------------------------

typedef enum
{
    SiiDEV_INPUT_CONNECTED  = 1,
    SiiDEV_BOOT_STATE_MACHINE,
    SiiDEV_NVRAM,
    SiiDEV_ID,
    SiiDEV_REV,
    SiiDEV_ACTIVE_PORT,
    SiiDEV_SELECTED_PORT_BITFIELD
} SiiDrvDeviceStatus_t;

uint_t SiiDrvDeviceInfo( SiiDrvDeviceStatus_t infoIndex );

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceManageInterrupts
// Description: Monitors Sii9687 interrupts and calls an interrupt process
//              function in the applicable driver.

// NOTE:    This function is not designed to be called directly from a physical
//          interrupt handler unless provisions have been made to avoid conflict
//          with normal level I2C accesses.
//          It is intended to be called from normal level by monitoring a flag
//          set by the physical handler.
//------------------------------------------------------------------------------

void SiiDrvDeviceManageInterrupts( uint_t inst );

//------------------------------------------------------------------------------
//  Function:       NeedNvramReload
//  Description:    Check to see if the NvramReload is needed.
//------------------------------------------------------------------------------

bool_t NeedNvramReload ( bool_t NvramReload );

#endif // __SI_DRV_DEVICE_H__
