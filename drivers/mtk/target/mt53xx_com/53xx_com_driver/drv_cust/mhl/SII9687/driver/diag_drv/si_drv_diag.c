//***************************************************************************
//!file     si_drv_diag.c
//!brief    Silicon Image Diagnostics Driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_drv_device.h"
#include "si_drv_diag.h"
#include "si_drv_diag_internal.h"
#include "si_drv_internal.h"
#include "si_drv_edid.h"

//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------
#if 0
static uint8_t  l_chEpstList [SII_INPUT_PORT_COUNT] = { REG_CH0_EPST, REG_CH1_EPST, REG_CH2_EPST, REG_CH3_EPST };
static uint8_t  l_chEpcmList [SII_INPUT_PORT_COUNT] = { REG_CH0_EPCM, REG_CH1_EPCM, REG_CH2_EPCM, REG_CH3_EPCM };

static uint8_t  l_testCtrl [3] = { BIT_CRC_TEST, BIT_SELF_AUTH1_TEST, BIT_SELF_AUTH2_TEST };
static uint8_t  l_testFail [3] = { BIT_CRC_FAIL, BIT_SELF_AUTH1_FAIL, BIT_SELF_AUTH2_FAIL };
#endif
//------------------------------------------------------------------------------
//  Diag Driver Instance Data
//------------------------------------------------------------------------------

DiagInstanceData_t diagInstance[1] = {{ 0 }};
DiagInstanceData_t *pDiag = &diagInstance[0];

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagInitialize
//! @brief      Initialize the Diagnostics hardware
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//              if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDiagInitialize ( void )
{
    bool_t success = true;

    memset( pDiag, 0, sizeof( DiagInstanceData_t ));

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagStatus
// Description: Returns a status flag word containing CPI-specific
//				information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit status flags word for the CPI Component
//------------------------------------------------------------------------------

uint16_t SiiDrvDiagStatus ( void )
{
    uint16_t statusFlags = pDiag->statusFlags;

	return( statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagGetLastResult
// Description: Returns the result of the last SiiDiagxxx function called
// Parameters:  none
// Returns:     Result of last Diagnostics component function.
//------------------------------------------------------------------------------

uint_t SiiDrvDiagGetLastResult ( void )
{

	return( pDiag->lastResultCode );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagPortConnectedGet
// Description: Return the connected status for the ports corresponding to the
//              port bitfield map.
// Parameters:  portMap      - port bitfield map.  Bit 0 == port 0, bit 4 == port 4
//              requireClock - To be considered connected, CKDT must also be present
// Returns:     Port bit==1: port connected, bit==0: no source device at port
//------------------------------------------------------------------------------

uint_t SiiDrvDiagPortConnectedGet ( uint8_t portMap, bool_t requireClock )
{
    uint8_t uData;

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    if ( requireClock )
    {
        // Join P4 with P3:0 of CKDT status and mask with requested ports
        uData = SiiRegRead( REG_CLKDETECT_STATUS );
        portMap = (((uData >> 2) & 0x10) | (uData & 0x0F)) & portMap;
    }

    // Join P4 with P3:0 of PWR5V status and mask with requested ports
    uData = SiiRegRead( REG_PWR5V_STATUS );
    uData = ((uData >> 2) & 0x10) | (uData & 0x0F);
    // Add in CBUS ports connected
    uData |= SiiRegRead( REG_CBUS_DEV_CONNECTED );
    portMap = uData & portMap;
    return( portMap );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagBistRun
// Description: Run the specified BIST
// Returns:     true    - test successful
//              false   - test failed
//------------------------------------------------------------------------------
#if 0
bool_t SiiDrvDiagBistRun ( int_t testIndex, int_t portIndex )
{
    long    startTime;
    uint8_t uData, tmdsEnableState;

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    // Select the clock source for the tests.
    // Set to use port clock directly, clear to use the clock after the input clock mux
    SiiRegWrite( REG_HDMI_TX_CLK_DEBUG, 0x01 );

    // Enable TX0 TMDS channels, but disable the output enables.
    tmdsEnableState = SiiRegRead( REG_TMDST_CTRL1 );
    SiiRegWrite( REG_TMDST_CTRL1, MSK_TMDS_EN );

    do
    {
        if (( testIndex >= sizeof( l_testCtrl )) || ( portIndex >= SII_INPUT_PORT_COUNT ))
        {
            pDiag->lastResultCode = SiiDIAG_INVALID_PARAMETER;
            break;
        }

        pDiag->statusFlags |= SiiDiagStatus_BIST_FAIL;     // Assume fail

        // Clearing and reading back the RX BIST status
        SiiRegWrite( REG_CH0_EPST, 0x00 );               // First time after power up requires this...
        SiiRegWrite( l_chEpstList[ portIndex], 0x00 );   // But all other times require this.
        SiiRegWrite( REG_CH0_EPCNT, 0x0C );              // LOAD KSV DONE bit & MI2C errors are stored
                                                                    // in port 3 status register, so get rid of those.
        uData = SiiRegRead( l_chEpstList[ portIndex] );
        if ( uData != 0x00 )
        {
            Printf("\r\nRX BIST status not cleared on start (%02X)\n", uData );
            pDiag->lastResultCode = SiiDIAG_BIST_NOT_CLEAR;
            break;
        }

        // Run the test
        SiiRegWrite( l_chEpcmList[ portIndex], l_testCtrl[ testIndex] );
        startTime = SiiTimerTotalElapsed();
        do
        {
            uData = SiiRegRead( l_chEpstList[ portIndex] );
            if (( SiiTimerTotalElapsed() - startTime ) > BIST_TEST_TIMEOUT )
            {
                break;
            }
        } while (( uData & BIT_BIST_COMPLETE ) == 0 );
        SiiRegWrite( l_chEpstList[ portIndex], 0x00 );   // Clear result

        // Set results
        if ( !(uData & BIT_BIST_COMPLETE ))
        {
            pDiag->lastResultCode = SiiDIAG_BIST_NOT_COMPLETE;
            break;
        }
        if ( uData & l_testFail[ testIndex] )
        {
            pDiag->lastResultCode = SiiDIAG_BIST_TEST_FAIL;
            break;
        }
        pDiag->statusFlags &= ~SiiDiagStatus_BIST_FAIL;     // Did NOT fail, remove flag

    } while ( 0 );

    //Restore original state
    SiiRegWrite( REG_HDMI_TX_CLK_DEBUG, 0x00 );
    SiiRegWrite( REG_TMDST_CTRL1, tmdsEnableState );

    return( pDiag->lastResultCode == SiiDIAG_SUCCESS );
}
#endif
//------------------------------------------------------------------------------
// Function:    SiiDrvDiagValidatePort
// Description: Determines if it is possible to run a test on the specified port
// Returns:     true    - test successful
//              false   - test failed
//------------------------------------------------------------------------------

bool_t SiiDrvDiagBistValidatePort ( int_t portIndex )
{
    uint8_t uData;

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    do  {
        if ( portIndex >= SII_INPUT_PORT_COUNT )
        {
            pDiag->lastResultCode = SiiDIAG_INVALID_PARAMETER;
            break;
        }
        uData = SiiDrvDiagPortConnectedGet( (BIT_CKDT_0 << portIndex), true );
        if ( uData == 0 )
        {
            pDiag->lastResultCode = SiiDIAG_NO_CLOCK_ON_PORT;
            break;
        }
    } while ( 0 );

    return( pDiag->lastResultCode == SiiDIAG_SUCCESS );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagEccErrorCount
// Description:
// Returns:     ECC error count
//------------------------------------------------------------------------------

int_t SiiDrvDiagEccErrorCount ( bool_t reset )
{
    uint16_t    eccErrCount;

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    //Check port ECC enable?

    eccErrCount = SiiRegRead( REG_ECC_ERRCOUNT_HI_MP );
    eccErrCount = ( eccErrCount << 8 ) | SiiRegRead( REG_ECC_ERRCOUNT_LO_MP );

    if ( reset )
    {
        SiiRegBitsSet( REG_PAUTH_MISC_CTRL0, BIT_CLEAR_ECC, SET_BITS );
        SiiRegBitsSet( REG_PAUTH_MISC_CTRL0, BIT_CLEAR_ECC, CLEAR_BITS );
    }
    return( eccErrCount );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagHdcpGoodStatusGet
// Description: Return the HDCP GOOD status for the ports corresponding to the
//              port bitfield map.
// Parameters:  portMap - port bitfield map.  Bit 0 == port 0, bit 4 == port 4
// Returns:     Port bit==1: HDCP good, bit==0: HDCP Bad
//------------------------------------------------------------------------------

uint_t SiiDrvDiagHdcpGoodStatusGet ( uint8_t portMap )
{
    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    return( (SiiRegRead( REG_RO_MP_CONTENT ) >> 3) & portMap );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagHdcpModeStatusGet
// Description: Return the HDCP/NON-HDCP status for the ports corresponding
//              to the port bitfield map.
// Parameters:  portMap - port bitfield map.  Bit 0 == port 0
// Returns:     Port bit==1: HDCP, bit==0: NON-HDCP
//------------------------------------------------------------------------------

uint_t SiiDrvDiagHdcpModeStatusGet ( uint8_t portMap )
{
    uint8_t uData, authStatus;

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    // Gather auth status bits for all ports.
    uData  = SiiRegRead( REG_PAUTH_STAT1 );
    authStatus = (uData & BIT_AUTH_MASK );

    // If authentication has been attempted, assume HDCP mode
    return( authStatus & portMap );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDiagEtpgEnabled
// Description: Determine if NVRAM ETPG bit is set to allow extended test patterns
// Returns:     true    - ETPG enabled
//              false   - ETPG disabled
//
// NOTE:        Since this function reads from NVRAM, it may slow down testing
//              if performed before every test.  Therefore, it should only be
//              used before running a group of tests.
//------------------------------------------------------------------------------

bool_t SiiDrvDiagEtpgEnabled ( void )
{
    bool_t  success;
    uint8_t bootData [64];

    pDiag->lastResultCode = SiiDIAG_SUCCESS;

    success = false;
    do  {

        if ( !SiiEdidNvramRead( EDID_NVRAM_BOOT, bootData ))
        {
            pDiag->lastResultCode = SiiDIAG_NVRAM_READ_ERROR;
            break;
        }

        if (( bootData[ 0x28] & 0x01 ) == 0 )
        {
            break;
        }
        success = true;     // enable bit is set
    } while ( 0 );

    return( success );
}

