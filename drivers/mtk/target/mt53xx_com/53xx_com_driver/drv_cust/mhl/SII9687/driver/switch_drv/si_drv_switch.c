//**************************************************************************
//!file     si_drv_switch.c
//!brief    SiI9687 Port Switch driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_platform.h"        // Interface to board environment
#include "si_scheduler.h"
#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_drv_switch_internal.h"



//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------

SwitchDrvInstanceData_t drvSwitchInstance[SII_NUM_SWITCH];
SwitchDrvInstanceData_t *pDrvSwitch = &drvSwitchInstance[0];


//-------------------------------------------------------------------------------------------------
//! @brief      Return index of MHL capable Rx port.
//!
//!             This information come out of NVRAM Boot data.
//!
//! @return     0 based port index, 0xFF - if MHL port not found.
//-------------------------------------------------------------------------------------------------

int_t SiiDrvSwitchMhlPortGet(void)
{
    uint8_t portIndex, cBusPorts;

    cBusPorts = SiiRegRead(REG_CBUS_PORT_SEL_AUTO);
    for (portIndex = 0; portIndex < SII_INPUT_PORT_COUNT; portIndex++)
    {
        if (cBusPorts & (1 << portIndex))
        {
            return portIndex;
        }
    }

    return 0xFF;
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchAutoHpeCbusEnable
// Description: Add or remove the designated CBUS port to/from the Auto-HPD
//              Software Interrupt mask. The type of port decides how we handle
//              Hot Plug Events (HPE) on that port.  If it is regular HDMI,
//              we do standard a Hot Plug. When the preauthentication logic
//              determines that the CBUS port needs an HPE, it fires off a
//              SW HPE interrupt and lets the software send the appropriate
//              HPE message over CBUS.
// Parameters:  true == enable
// Returns:     n/a
//------------------------------------------------------------------------------

void SiiDrvSwitchAutoHpeCbusEnable ( bool_t enable )
{
    uint8_t portMask, mask, nvramMask;

    // Clear the CBUS mask bit position in the Auto-HPD mask register
    mask        = (1 << pDrvSwitch->mhlPortIndex) << 2;
    portMask    = (SiiRegRead( REG_HPE_HEC_PORTS ) & ~mask);

    if ( enable )
    {
        // Enable only if also enabled in NVRAM
        nvramMask   = SiiRegRead( REG_CBUS_PORT_SEL_AUTO ) << 2;
        if ( nvramMask & mask )
        {
            // Replace the portType[] entry with HEAC port type
            pDrvSwitch->portType[ pDrvSwitch->mhlPortIndex] = SiiPortType_MHL;
            portMask |= mask;   // Add the HEC bit to the register mask
            Printf("\r\nSiiDrvSwitchAutoHpeCbusEnable:: - MHL port#: %02x\n", (int)pDrvSwitch->mhlPortIndex);
        }
    }
    else
    {
        // Replace the portType[] entry with HDMI port type
        pDrvSwitch->portType[ pDrvSwitch->mhlPortIndex] = SiiPortType_HDMI;
        Printf("\r\nSiiDrvSwitchAutoHpeCbusEnable:: - non-MHL port#: %02x\n", (int)pDrvSwitch->mhlPortIndex);
    }

    // Update the hardware
    SiiRegModify( REG_HPE_HEC_PORTS, VAL_FW_HPE_MASK, portMask );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchInitialize
// Description:
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvSwitchInitialize ( uint_t arcPortIndex )
{
	
    // Clear instance data
    memset( pDrvSwitch, 0, sizeof( SwitchDrvInstanceData_t ));
    Printf("\r\n  SiiDrvSwitchInitialize..\n");
    pDrvSwitch->mpInfoFrameMask = 0x3F; //TODO: Why 3F?

    // set AUTO_TERM bit to 1 for automatic TMDS off in case of no MHL connection
    SiiRegModify(REG_AUTO_CONFIG, BIT_AUTO_TERM_EN, CLEAR_BITS);    //Enable Auto Termination for MHL

    pDrvSwitch->mhlPortIndex = SiiDrvSwitchMhlPortGet();
    pDrvSwitch->arcPortIndex = arcPortIndex;
    Printf("\r\nPort assignments: MHL (%01x), ARC (%01x)\n", (int)pDrvSwitch->mhlPortIndex, (int)pDrvSwitch->arcPortIndex);

    // Clear the hardware Auto-HPD mask to default (All HDMI),
    SiiRegModify( REG_HPE_HEC_PORTS, VAL_FW_HPE_MASK, CLEAR_BITS );

    // Enable the interrupts for this driver
    SiiRegBitsSet( REG_PA_INTR1_MASK, BIT_MP_HPE | BIT_RP_HPE, SET_BITS );
    SiiRegBitsSet( REG_PA_INTR3_MASK, BIT_HPD_END_MP, SET_BITS );
    SiiRegBitsSet( REG_PA_INTR6_MASK, BIT_PA6_MASK_MP_NEW_ALL | BIT_PA6_MASK_MP_NO_ALL, SET_BITS );
    SiiRegBitsSet( REG_INT_MASK6, BIT_5VPWR_CHANGE, SET_BITS );

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchStatus
// Description: Returns a status flag word containing Switch driver-specific
//              information about the state of the device.
// Parameters:  mask - 0x0000:   Return the entire status word without clearing driver status flags
//                     non-zero: Return the status bits indicated by the set mask bits.
//                               If a mask bit refers to one of the switch interrupt flags covered
//                               (except the main switch INT flag), the bit is cleared in the driver
//                               status.  The main switch INT status bit is NOT cleared until
//                               ALL switch interrupt flag bits have been read.
// Returns:     Sixteen-bit status flags word for the Switch Driver
//------------------------------------------------------------------------------

SwitchStatusFlags_t SiiDrvSwitchStatus ( SwitchStatusFlags_t mask )
{
    SwitchStatusFlags_t intClearMask;
    SwitchStatusFlags_t statusFlags = pDrvSwitch->statusFlags;

    if ( mask )
    {
        // Return only the flags they are interested in

        statusFlags &= mask;

        // Clear the interrupt flags that were requested (except main interrupt flag).

        intClearMask = mask & SiiSWITCH_INTFLAGS;
        pDrvSwitch->statusFlags &= ~intClearMask;

        // If all interrupt flags are cleared, clear the main interrupt flag

        if (( pDrvSwitch->statusFlags & (SiiSWITCH_INTFLAGS | SiiSWITCH_SCDT_CHANGE | SiiSWITCH_MP_INFOFRAME_INTR_VALID)) == 0 )
        {
            pDrvSwitch->statusFlags &= ~SiiSWITCH_INT;
        }
    }

    return( statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchIfInterruptStatusGet
// Description: Returns the last Infoframe interrupt Status data retrieved by
//              the Switch ISR.
// Parameters:  pData - pointer to return data buffer (1 byte).
// Returns:     true if new interrupt status data is available, false if not.
//              pData - Destination for interrupt status data.
//------------------------------------------------------------------------------

bool_t  SiiDrvSwitchIfInterruptStatusGet ( uint8_t *pData )
{

    *pData = pDrvSwitch->mpInfoFrameInterruptStatus;
    if ( pDrvSwitch->statusFlags & SiiSWITCH_MP_INFOFRAME_INTR_VALID )
    {
        pDrvSwitch->statusFlags &= ~SiiSWITCH_MP_INFOFRAME_INTR_VALID;

        // If all interrupt flags are cleared, clear the main interrupt flag
        // (we KNOW the above interrupt has been cleared, check the rest.)

        if (( pDrvSwitch->statusFlags & (SiiSWITCH_INTFLAGS | SiiSWITCH_SCDT_CHANGE )) == 0 )
        {
            pDrvSwitch->statusFlags &= ~SiiSWITCH_INT;
        }
        return( true );
    }
    return( false );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchStateGet
// Description: Get the selected state type information.
// Parameters:  stateType   - Type of state returned.
// Returns:     Returns an eight bit state value of the selected type.
//------------------------------------------------------------------------------

uint8_t SiiDrvSwitchStateGet ( SiiDrvSwitchStateTypes_t stateType )
{
    uint8_t stateValue, temp;

    stateValue = 0;
    switch ( stateType )
    {
    case SiiSwitch_FIRST_PASS:
        stateValue = pDrvSwitch->firstPass;
        break;
    case SiiSwitch_SELECTED_INPUT_CONNECT_STATE:
        stateValue = pDrvSwitch->mpCableIn;
        break;
    case SiiSwitch_INPUT_PORTS_CONNECT_STATE:

        // Include both HDMI and MHL.

        stateValue  = SiiRegRead( REG_PWR5V_STATUS ) & VAL_PWR5V_MASK;
        temp        = SiiRegRead( REG_HDMIM_CP_PAD_STAT ) & VAL_PWR5V_MASK;
        stateValue  = stateValue | ( temp & 0x0F );
        break;
    }

    return( stateValue );
}

#if (SWITCH_LINK_CALLBACK == DISABLE)
//------------------------------------------------------------------------------
// Function:    SwitchCallback
// Description: Call the specified indirect callback function if it has been
//              registered.
//------------------------------------------------------------------------------

static void SwitchCallback ( int callbackType, uint16_t statusFlags )
{
    void (*pCallback)(uint16_t) = 0;

    switch ( callbackType )
    {
    case CALLBACK_STATUS_CHANGE:

        pCallback = pDrvSwitch->pCallbackStatusChange;
        break;
    }

    if ( pCallback )
    {
        (pCallback)( statusFlags );
    }
}
#endif

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchSourceSelect
// Description: Connect the passed input port to the main pipe.
//------------------------------------------------------------------------------

void SiiDrvSwitchSourceSelect ( uint_t portIndex )
{
    // Switch user select to the requested port.
    SiiRegModify( REG_RX_PORT_SEL, MSK_PORT_ALL, portIndex );
    pDrvSwitch->userPortSelect = portIndex;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return currently selected source in MP.
//!
//! @return     port index.
//-------------------------------------------------------------------------------------------------

uint_t SiiDrvSwitchSourceGet ( void )
{
    return pDrvSwitch->userPortSelect;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Select port in Sub Pipe.
//!
//!             Automatic roving is expected to be stopped before calling this function.
//!             Primary use is for PIP source selection.
//!
//! @param[in]  subSource - port index
//-------------------------------------------------------------------------------------------------

void SiiDrvSwitchSubPortSelect( uint_t subPortIndex )
{
    SiiRegModify(REG_IP_CONFIG_47, MSK_ALL_PORTS, 0x01 << subPortIndex);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enable/Disable SP Roving.
//!
//! @param[in]  isEnabled  - enable flag.
//-------------------------------------------------------------------------------------------------

void SiiDrvSwitchRovingEnable( bool_t isEnabled )
{
    SiiRegModify( REG_IP_CONFIG_43, BGND_DISABLE, isEnabled ? CLEAR_BITS : SET_BITS );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchProcessInterrupts
// Description:
//
// NOTE:        If called in a polling manner, this function should be called
//              relatively often to avoid delay in muting and unmuting when
//              using external mute.
//------------------------------------------------------------------------------

void SiiDrvSwitchProcessInterrupts ( void )
{
    uint8_t     intStatus;
    uint16_t    statusFlags = 0;                // generate a new set every time

    // Check plug/unplug interrupt for the main pipe (selected port).
 	
    intStatus = SiiRegRead( REG_INT_STATUS6 ) & BIT_5VPWR_CHANGE;
    Printf("\r\nSiiDrvSwitchProcessInterrupts,REG_INT_STATUS6=%02x\n",intStatus);
    if (intStatus )
    {
        SiiRegWrite( REG_INT_STATUS6, intStatus );

        /* Update the selected port cable state.    */

        if ( !(SiiRegRead( REG_STATE ) & ( BIT_PWR5V | BIT_MHL )))
        {
            pDrvSwitch->mpCableIn = false;
            Printf("\r\nMP Cable OUT\n" );
        }
        else
        {
            pDrvSwitch->mpCableIn = true;
            Printf("\r\nMP Cable IN\n" );
        }

        statusFlags |= SiiSWITCH_CABLE_CHANGE;
    }

    // Handle CBUS/CDC Main Pipe port HPE requests from pre-authentication hardware.
    // NOTE:    For RP requests, the HPE must be triggered by the firmware as soon as
    //          possible after receiving this interrupt; before the RP switches to another port.

    intStatus = SiiRegRead( REG_PA_INTR1 ) & (BIT_MP_HPE | BIT_RP_HPE);
        Printf("\r\nSiiDrvSwitchProcessInterrupts,REG_PA_INTR1=%02x\n",intStatus);

    if ( intStatus )
    {
        SiiRegWrite( REG_PA_INTR1, intStatus );
        if ( intStatus & BIT_MP_HPE)
        {
            statusFlags |= SiiSWITCH_MP_HPE_TRIGGER;
        }
        if ( intStatus & BIT_RP_HPE)
        {
            statusFlags |= SiiSWITCH_RP_HPE_TRIGGER;
        }
    }

    // Check HPE Done interrupt.

    intStatus = SiiRegRead( REG_PA_INTR3 ) & BIT_HPD_END_MP;
    if ( intStatus )
    {
        SiiRegWrite( REG_PA_INTR3, intStatus );

        // Restore EDDC disable end time to 0 in case it was used

        DEBUG_PRINT( MSG_DBG | DBGF_TS, ("MP HPE End\n"));
        SiiRegWrite( REG_HPE_EDDC_END_DLY, 0x00 );
        statusFlags |= SiiSWITCH_HPE_COMPLETE;

        if ( pDrvSwitch->firstPass )
        {
            pDrvSwitch->firstPass = false;
        }
    }

    // Notify upper layer that a switch module interrupt occurred

    if ( statusFlags )
    {
        pDrvSwitch->statusFlags |= ( statusFlags | SiiSWITCH_INT );
    }

    SwitchInfoframeProcessInterrupts();     // Check RX InfoFrame changes

    if ( pDrvSwitch->statusFlags & SiiSWITCH_INT )
    {
        SiiSchedSendSignal(SI_EVENT_ID_SWITCH_INT, 0);
    }

}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchDeviceRXTermControl
// Description: Enable or disable RX termination for the selected port(s)
// Parameters:  portIndex   - 0-3:  Switch port to control
//                          - 0xFF: Apply to all ports.
//              enableVal   - The bit pattern to be used to enable or disable
//                            termination
//                          0x00 - Enable for HDMI mode
//                          0x55 - Enable for MHL mode
//                          0xFF - Disable
//
// Note:        The 'enableVal' parameter for this function is NOT boolean as
//              it is for the companion si_DeviceXXXcontrol functions.
// Note:        The 'portIndex' parameter value 0xFF should not be used unless
//              all ports are HDMI1.3/a (not MHL or CDC)
//------------------------------------------------------------------------------
void SiiDrvSwitchDeviceRXTermControl ( uint8_t portIndex, uint8_t enableVal )
{
    uint8_t value;

    /* If caller wants all ports controlled at once...  */

    if ( portIndex == SiiPORT_ALL )
    {
        Printf("\r\nnSiiPORT_ALL\n" );
        SiiRegModify( REG_TMDS_TERMCTRL0, SET_BITS, enableVal );
    }

    value = (MSK_TERM << ((portIndex % 4) * 2));
    SiiRegModify( REG_TMDS_TERMCTRL0, value, enableVal );
}

