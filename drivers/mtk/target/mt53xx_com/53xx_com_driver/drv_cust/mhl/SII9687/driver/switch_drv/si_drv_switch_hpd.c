//**************************************************************************
//!file     si_drv_hpd.c
//!brief    SiI9687 HPD-Rx Driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_drv_switch_internal.h"
#include "si_drv_arc.h"


//------------------------------------------------------------------------------
// Function:    si_DeviceHdcpDdcControl
// Description: Enable or disable HDCP access for the selected port(s)
// Parameters:  portIndex   - 0-3:  Switch port to control
//                          - 0xFF: Apply to all ports.
//              enableHDCP  - true to enable, false to disable
//
// Note:        The 'portIndex' parameter value 0xFF should not be used unless
//              all ports are HDMI1.3/a (not MHL or CDC)
//------------------------------------------------------------------------------

static void DeviceHdcpDdcControl ( uint8_t portIndex, bool_t enableHDCP )
{
    uint8_t enableVal, enableMask;

    enableVal = enableHDCP ? SET_BITS : CLEAR_BITS;

    if ( portIndex == SiiPORT_ALL )     // All ports at once
    {
        enableMask = MSK_DDC_EN;
    }
    else                                // Port 0 - 3
    {
        enableMask = (BIT_DDC0_EN << portIndex);
    }

    SiiRegModify( REG_HDCP_DDC_CTRL, enableMask, enableVal );
}

//------------------------------------------------------------------------------
// Function:    SwitchHpdSetState
// Description: (Switch only, no MHL or CDC) - Set the correct HPD pin state
//              for HDMI 1.3/a operation mode.
// Parameter:   port:       HDMI Port to control
//              newState:   Requested new state for HPD
//
//                  SiiHPD_ACTIVE       - HPD HI, HDCP, EDID, RX Term enabled
//                  SiiHPD_INACTIVE     - HPD LOW, HDCP, RX Term disabled
//                  SiiHPD_ACTIVE_EX    - EDID, RX Term enabled
//                  SiiHPD_INACTIVE_EX  - HPD HI, HDCP, EDID, RX Term disabled
//
//                  RX term enabled is HDMI termination.
//
// Returns:     TRUE if change to new state was successful.
//              FALSE otherwise.
//------------------------------------------------------------------------------

bool_t SwitchHpdSetState ( uint_t portIndex, SiiDrvHpdActions_t newState )
{
    int_t   pipe;
    bool_t  doEdid = false;

    switch ( newState )
    {
        // HPD signal active, HDCP access enabled, RX Term enabled, EDID access enabled
        case SiiHPD_ACTIVE:
            SiiRegWrite( REG_EN_EDID, VAL_EN_DDC_ALL );
            SiiDrvSwitchDeviceRXTermControl( portIndex, SiiTERM_HDMI );
            SiiDrvSwitchDeviceHpdControl( portIndex, true, false );
            DeviceHdcpDdcControl( portIndex, true );
            break;

        // RX termination enabled, all else let as is.
        case SiiHPD_ACTIVE_EX:
            SiiRegWrite( REG_EN_EDID, VAL_EN_DDC_ALL );
            SiiDrvSwitchDeviceRXTermControl( portIndex, SiiTERM_HDMI );
            break;

        // HPD signal inactive, HDCP access disabled, RX Term disabled, EDID access disabled
        case SiiHPD_INACTIVE_EX:
        //  -------------------------- Same as SiiHPD_INACTIVE --------------------------

        // HPD signal inactive, HDCP access disabled, RX Term disabled
        case SiiHPD_INACTIVE:
            SiiRegWrite( REG_EN_EDID, VAL_EN_DDC_NONE );          // Disable all EDID
            SiiDrvSwitchDeviceHpdControl( portIndex, false, false );
            SiiDrvSwitchDeviceRXTermControl( portIndex, SiiTERM_DISABLE );
            DeviceHdcpDdcControl( portIndex, false );
            break;

        case SiiHPD_TOGGLE_EDID:

            doEdid = true;
            // Fall through to SiiHPD_TOGGLE
        case SiiHPD_TOGGLE:

            // Requested port on main or roving pipe?
            if ( portIndex == (SiiRegRead( REG_RX_PORT_SEL ) & MSK_PORT_ALL))
            {
                pipe = 0;
            }
            else
            {
                pipe = 1;
            }
            //Use auto-HPE hardware
            SiiDrvSwitchPipeHpeTrigger( pipe, doEdid );
            break;

        default:
            Printf("\r\nUnused HDMI SetHpd newState: %02X\n", (int)newState );
            break;
    }

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SwitchHpdCallbackGet
// Description: Get a pointer to the HPD state set function for the requested port
// Parameters:  portIndex   - 0-4:  Switch port to control
//------------------------------------------------------------------------------

static BoolTwoUInts_t SwitchHpdCallbackGet ( int_t portIndex )
{
    BoolTwoUInts_t pCallHpdSetState = 0;

#if (SWITCH_LINK_CALLBACK == ENABLE)

        // If developer has chosen a compile-time link,
        // they must also ensure that the CBUS driver and CDC component exist
        // and contain the functions used below.

        switch ( pDrvSwitch->portType[ portIndex])
        {
        case SiiPortType_HDMI:
        	pCallHpdSetState = (BoolTwoUInts_t)SwitchHpdSetState;
        	break;
        case SiiPortType_MHL:
        	pCallHpdSetState = (BoolTwoUInts_t)SiiMhlCbHpdSetState;
        	break;

        default:
            break;
        }
#else
        // Developer has chosen run-time registered links, which means
        // that if a CBUS device has NOT been registered for a port
        // specified as CDC, this function will fail.

        switch ( pDrvSwitch->prtType[ portIndex])
        {
        case SiiPortType_HDMI:  pCallHpdSetState = (BoolTwoUInts_t)SwitchHpdSetState; break;
        case SiiPortType_MHL:   pCallHpdSetState = 0;       break;
        default:
            break;
        }
#endif
        return( pCallHpdSetState );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchDeviceHpdControl
// Description: Enable or disable HPD for the selected port(s)
// Parameters:  portIndex   - 0-4:  Switch port to control
//                          - 0xFF: Apply to all ports.
//              enableHPD   - true to enable, false to disable
//              mode        - true to tristate the HPD.
//                          - false to clear hpd.
//
// Note:        The 'portIndex' parameter value 0xFF should not be used unless
//              all ports are HDMI1.3/a (not MHL or CDC)
//------------------------------------------------------------------------------

void SiiDrvSwitchDeviceHpdControl ( uint8_t portIndex, bool_t enableHPD, uint8_t mode)
{
    uint8_t enableVal;

    if (mode)
        enableVal = enableHPD ? VAL_HP_PORT_MHL : CLEAR_BITS;
    else
        enableVal = enableHPD ?  VAL_HP_PORT_ALL_HI : CLEAR_BITS;

    SiiRegModify( REG_HP_CTRL, (VAL_HP_PORT0_MASK << ((portIndex % 4) * 2)), enableVal );
}

//------------------------------------------------------------------------------
// Function:    HpdSetState
// Description: Set the correct HPD state for a single port.
// Parameter:   portIndex   - Port (0-4), does not accept SiiPORT_ALL
//              newState    - Same as SiiDrvHpdSetState
// Returns:     true        - Change was successful
//              false       - Failure occurred.
//------------------------------------------------------------------------------

static bool_t HpdSetState ( uint8_t portIndex, uint8_t newState )
{
    BoolTwoUInts_t pCallHpdSetState = 0;

    // If a valid function pointer for this port, get it.
    pCallHpdSetState = SwitchHpdCallbackGet( portIndex );
    if ( pCallHpdSetState == 0 )
    {
        DEBUG_PRINT( SW_ERR, "No SetHPDState function for port %d!\n", (int)portIndex );
        return( false );
    }

    switch ( pDrvSwitch->portType[ portIndex] )
    {
    case SiiPortType_HDMI:
        break;
    case SiiPortType_MHL:

        // We also have to disable/enable the HDCP Access for CBUS.
        // CBUS recognizes the same enable/disable controls as used for HDMI
        if ( newState == SiiHPD_ACTIVE )
        {
            SiiRegWrite( REG_EN_EDID, VAL_EN_DDC_ALL );
            DeviceHdcpDdcControl( portIndex, true );
        }
        else if ( newState == SiiHPD_INACTIVE )
        {
            SiiRegWrite( REG_EN_EDID, VAL_EN_DDC_NONE );
            DeviceHdcpDdcControl( portIndex, false );
        }
        break;
    }

    // Call the appropriate SetHpd function
    return( (*pCallHpdSetState)( portIndex, newState ) );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvHpdSetState
// Description: Set the correct HPD state as determined by the request and
//              the current operating mode of the port.
// Parameter:   port:       HDMI Port to control
//              newState:   Requested new state for HPD
//
//              If Port is in CDC, MHL, or HDMI1.4 modes:
//
//                  SiiHPD_ACTIVE       - Send HPD HI command
//                  SiiHPD_INACTIVE     - Send HPD LOW command
//                  SiiHPD_ACTIVE_EX    - Send HPD HI command
//                  SiiHPD_INACTIVE_EX  - Send HPD LOW command
//                  SiiHPD_TOGGLE       - MHL - Toggle HPD pin HI then LOW
//                                        All other modes, nothing
//
//                  HDCP, EDID, RX Term enabled for all values of newState.
//                  MHL mode RX term enabled is MHL termination.
//
//              If Port is in HDMI1.3a mode:
//
//                  SiiHPD_ACTIVE       - HPD HI, HDCP, EDID, RX Term enabled
//                  SiiHPD_INACTIVE     - HPD LOW, HDCP, RX Term disabled
//                  SiiHPD_ACTIVE_EX    - EDID, RX Term enabled
//                  SiiHPD_INACTIVE_EX  - HPD LOW, HDCP, EDID, RX Term disabled
//                  SiiHPD_TOGGLE       - Nothing
//
//                  RX term enabled is HDMI termination.
//
// Returns:     TRUE if change to new state was successful.
//              FALSE otherwise.
//------------------------------------------------------------------------------

bool_t SiiDrvHpdSetState ( uint8_t portIndex, SiiDrvHpdActions_t newState )
{
    bool_t  success;
    uint8_t i;

    /* If all ports specified, need to iterate through ports.   */

    if ( portIndex == SiiPORT_ALL )
    {
        success = true;
        for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
        {
            /* Set the state for this port and accumulate success or failure.   */

            success = (HpdSetState( i, newState ) && success);
        }
    }
    else
    {
        success = HpdSetState( portIndex, newState );
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchPipeHpeTrigger
// Description: Trigger an automatic hot plug event sequence (ON-OFF) on the port
//              attached to the selected pipe.
// Parameter:   pipe:           0 - HPE on port attached to main pipe
//                              1 - HPE on port attached to roving pipe
//              includeEdid:    false - DO NOT disable EDID DDC during HPE
//                              true  - Disable EDID DDC during HPE
//------------------------------------------------------------------------------

void SiiDrvSwitchPipeHpeTrigger ( uint8_t pipe, bool_t includeEdid )
{
    int_t           i;
    uint8_t         portIndex, triggerMask;
    BoolTwoUInts_t  pCallHpdSetState = 0;

    // Determine which port we're dealing with...

    if ( pipe == SiiPipeType_Main )
    {
        portIndex = SiiRegRead( REG_RX_PORT_SEL ) & MSK_PORT_ALL;
    }
    else
    {
        portIndex = (SiiRegRead( REG_PAUTH_STAT0 ) & ROVE_PIPE_MASK) >> 4;
        if ( portIndex == 0 )
        {
            portIndex = 0x01;
        }
        for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
        {
            if (( 0x01 << i ) == portIndex )
            {
                portIndex = i;
                break;
            }
        }
    }

    do  // (for error exit...)
    {
        // If port is marked for message-based HPE, get the appropriate CEC message function.
        if ( pDrvSwitch->portType[ portIndex] != SiiPortType_HDMI )
        {
            pCallHpdSetState = SwitchHpdCallbackGet( portIndex );
            if ( pCallHpdSetState == 0 )
            {
                DEBUG_PRINT( SW_ERR, "No SetHPDState function for port %d!\n", (int)portIndex );
                break;
            }
        }

        switch ( pDrvSwitch->portType[ portIndex] )
        {
            case SiiPortType_HDMI:

                // Use the auto-HPE hardware to perform a physical hot plug event.
                triggerMask = pipe ? (BIT_OVR_RP | BIT_TRIGGER_RP) : (BIT_OVR_MP | BIT_TRIGGER_MP);
                if ( includeEdid )
                {
                    // Set end time of EDID HPE timer to standard 1200ms so that EDID access
                    // will be disabled during HPE sequence.  This will be cleared in the
                    // ISR triggered at the end of the HPE event.
                    SiiRegWrite( REG_HPE_EDDC_END_DLY, 0x0C );
                }

                // Toggle the HW HPE for the selected pipe, leave HPE in automatic hardware control
                SiiRegModify( REG_HPE_CONTROL, triggerMask, SET_BITS );
                SiiRegModify( REG_HPE_CONTROL, triggerMask, CLEAR_BITS );
                break;

            case SiiPortType_MHL:
                // Call the SetHPDState function for this port type.
                (*pCallHpdSetState)( portIndex, SiiHPD_TOGGLE );
                break;

        }
    } while (0);    // Do the above only once.
}



void DisplayTxRxState (void )
{
    int_t rxHpd, rxTerm;
    static int_t orxHpd, orxTerm = 0;

    rxHpd   = SiiRegRead( REG_HP_CTRL ) & VAL_HP_PORT0_MASK;
    rxTerm  = (SiiRegRead( REG_TMDS_TERMCTRL0 ) & MSK_TERM) ? 0 : 1;
    if (( orxHpd != rxHpd) || (orxTerm != rxTerm))
    {
        DEBUG_PRINT( DBGF_TS, "RX HPD: %d  RX TERM: %d\n", rxHpd, rxTerm );

        orxHpd = rxHpd;
        orxTerm = rxTerm;
    }
}


