//***************************************************************************
//!file     si_drv_sii9687.c
//!brief    Sii9687 driver.
//!brief    Driver layer internal file
//
//          The Sii9687 driver contains all functions data, and constants
//          common to all modules at the next higher, component driver level.
//          It can be thought of as a wrapper that ties all of the individual
//          Sii9687 functions together to create the Sii9687 device.
//
//          This module is also the interface from the Sii9687 to the
//          physical board environment that it is mounted on.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2007-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_platform.h"        // Interface to board environment
#include "si_osal_timer.h"
#include "si_scheduler.h"
#include "si_drv_internal.h"
#include "si_drv_gpio.h"
#include "si_drv_device.h"
#include "si_drv_switch.h"
#include "si_drv_tx_simple.h"
#if INC_IPV
#include "si_drv_ipv.h"
#endif
#include "si_drv_cbus.h"



// EDID & NVRAM are closely related to device low-level functions
#include "si_edid_component.h"

//------------------------------------------------------------------------------
// Device Component Register initialization list
//------------------------------------------------------------------------------

static const uint16_t initRegsList [] =
{
        REG_CBUS_CH_RST_CTRL,		0x00,
        REG_PAUTH_INV_CTRL,         0x00,   // Disable clock inversion for all ports

        REG_PAUTH_MHL_IEEE_ID1,     0x1D,   // MHL 3D VSIF IEEE ID
        REG_PAUTH_MHL_IEEE_ID2,     0xA6,
        REG_PAUTH_MHL_IEEE_ID3,     0x7C,

        REG_PAUTH_MHL3D_CFG1,       0x19,   // MHL to HDMI conversion configuration
        REG_PAUTH_MHL3D_CFG2,       0x81,   // MHL 3D VSIF Header
        REG_PAUTH_MHL_TESTER_MODE,  0x80,   // Enable MHL VSIF capturing
        REG_PA_CONFIG_11,           0x49,   // VSI header checked by s/w. 3D, IEEE ID and Video Format - by h/w

        REG_CLR_PACKET_BUFFER,      (BIT_CLR_VSI | BIT_VSI_IEEE_CHK_EN), // H/w to clear the VSI contents and enable h/w based VSI ID check
        REG_PAUTH_MHL_CFG2,         0xC0,   // PackedPixel mode (FF)
        REG_TMDS0_BW_I2C,           0x14,
        REG_TMDS1_BW_I2C,           0x14,

        REG_TMDS0_EQ_DATA0,         0x88,   // TMDS MP EQ table
        REG_TMDS0_EQ_DATA1,         0x98,
        REG_TMDS0_EQ_DATA2,         0xA8,
        REG_TMDS0_EQ_DATA3,         0xB8,
        REG_TMDS0_EQ_DATA4,         0xB9,
        REG_TMDS0_EQ_DATA5,         0xC9,
        REG_TMDS0_EQ_DATA6,         0xBA,
        REG_TMDS0_EQ_DATA7,         0xB8,

        REG_TMDS1_EQ_DATA0,         0x98,   // TMDS RP EQ table
        REG_TMDS1_EQ_DATA1,         0xB8,
        REG_TMDS1_EQ_DATA2,         0xB9,
        REG_TMDS1_EQ_DATA3,         0xBA,

        REG_MHL1X_EQ_DATA0,         0x88,   // MHL 1x EQ table
        REG_MHL1X_EQ_DATA1,         0x98,
        REG_MHL1X_EQ_DATA2,         0xA8,
        REG_MHL1X_EQ_DATA3,         0x98,

        REG_PAUTH_ALT_ZONE4_CFG,    0xE0, //TODO: test for #24238
        REG_TMDSRX_CTRL1,			0x0C,

};

//------------------------------------------------------------------------------
//  Device Driver data
//------------------------------------------------------------------------------

DeviceDrvInstanceData_t deviceDrvInstance[1] =
{
{
    0,                          // structVersion
    SII_SUCCESS,                // lastResultCode Contains the result of the last API function called
    0,                          // statusFlags

    I2C_SUCCESS,                // lastI2cErrorCode - Contains the last error code incurred by the I2C functions

    0,                          // devType;
    0,                          // devRev;
}
};

DeviceDrvInstanceData_t *pDrvDevice = &deviceDrvInstance[0];

//------------------------------------------------------------------------------
//NOTE: The following sequence will be changed according to customer UI port definition..........
//------------------------------------------------------------------------------
static uint8_t HDMI_IN1 = SiiPORT_1;
//static uint8_t HDMI_IN2 = SiiPORT_0;
//static uint8_t HDMI_IN3 = SiiPORT_2;
//static uint8_t HDMI_IN4 = SiiPORT_3;

//------------------------------------------------------------------------------
//  Function:       SiiEdidValidHdmi
//  Description:    Validate the passed EDID as an HDMI EDID.                                     
//------------------------------------------------------------------------------
static uint8_t SiiEdidValidHdmi ( const uint8_t *pData );

//------------------------------------------------------------------------------
//  Edid table declarations - must be exported by application code.
//------------------------------------------------------------------------------

extern const uint8_t gEdidFlashEdidTable [ EDID_TABLE_LEN ];
extern const uint8_t gEdidFlashEdidVgaTable [ EDID_VGA_TABLE_LEN ];
extern const SiiDeviceBootData_t gEdidFlashDevBootData;

//------------------------------------------------------------------------------
// Function:    DrvDeviceInitRegisters
// Description: Initialize registers that need to be set to non-default values
//              at startup.  In general, these registers are not changed
//              after startup.
// Parameters:  none
// Returns:     none
//
// NOTE: 9687 Output is muted at the exit of this function.
//       If MonitorMute() is not used, this function must
//       be changed to un-mute on exit.
//-----------------------------------------------------------------------------c

static void DrvDeviceInitRegisters ( void )
{
    int_t i;
    uint8_t regVal;

    // Set up 9687 state machine control.
    SiiRegWrite(REG_PAUTH_MISC_CTRL0, BIT_USE_FRAME_ECC | BIT_MATCH_IND_SEL |
                BIT_DIS_GEN_VS_CTL3 | BIT_FIX_DELAY | BIT_RECOV_EN);

    /* Turn off port finite state machine and hold device in    */
    /* software reset until finished update.                    */

    SiiRegWrite(REG_PAUTH_CTRL, BIT_IGNORE_PAUTH_HPD | BIT_SKIP_NON_HDCP);
    SiiRegModify(REG_SRST, BIT_SWRST, SET_BITS);

    SiiRegModify(REG_HDCP_RST, BIT_HDCP_RST, SET_BITS); // HDCP arbitration and OTP reset
    SiiRegModify(REG_HDCP_RST, BIT_HDCP_RST, CLEAR_BITS); // Release

    /* Perform the majority of the required register initialization.    */
    for (i = 0; i < (sizeof(initRegsList) / 2); i += 2)
    {
        SiiRegWrite(initRegsList[i], initRegsList[i + 1]);
    }

    SiiRegModify(REG_PAUTH_ECC_CTRL, BIT_EN_ECC, SET_BITS);		// Enable ECC error counter
    SiiRegWrite(REG_HPD_HW_CTRL, MSK_INVALIDATE_P0123); 		// Disable Auto-HPD control on ports 0,1,2,3

    SiiRegModify(REG_PLL0_VCOCAL, 0x0F, 0x0C);

    SiiRegModify(REG_TMDSRX_CTRL2, 0x30, 0x00);
    SiiRegModify(REG_TMDSRX_CTRL3, 0x30, 0x30);
    SiiRegModify(REG_TMDSRX_CTRL4, 0x0F, 0x0A);

    SiiRegModify(REG_DPLL_MULTZONE_CTRL, 0xF0, 0x50);
    SiiRegModify(REG_TMDST_CTRL2, 0xF0, 0x40);
    SiiRegModify(REG_REGUL_PWR_ENABLE, 0xC0, 0x00);

    // enable checking of ECC errors
    // SiiRegModify( REG_PAUTH_ECC_CTRL, BIT_EN_ECC, SET_BITS);

    SiiRegModify(REG_ESD_PROT_SEL_HWSW, (BIT3 | BIT2 | BIT1 | BIT0), (BIT3 | BIT2 | BIT1 | BIT0));

    // Rx termination is for HDMI mode by default
    SiiRegModify(REG_TMDS_TERMCTRL1, MSK_TERM_P0, VAL_TERM_P0_HDMI);
    SiiRegModify(REG_TMDS_TERMCTRL1, MSK_TERM_P1, VAL_TERM_P1_HDMI);
    SiiRegModify(REG_TMDS_TERMCTRL3, MSK_TERM_P2, VAL_TERM_P2_HDMI);
    SiiRegModify(REG_TMDS_TERMCTRL3, MSK_TERM_P3, VAL_TERM_P3_HDMI);

    // Find MHL enabled Rx port and set correct termination for it
    switch (SiiDrvSwitchMhlPortGet())
    {
        case 0:
            SiiRegModify(REG_TMDS_TERMCTRL1, MSK_TERM_P0, VAL_TERM_P0_MHL);
            break;
        case 1:
            SiiRegModify(REG_TMDS_TERMCTRL1, MSK_TERM_P1, VAL_TERM_P1_MHL);
            break;
        case 2:
            SiiRegModify(REG_TMDS_TERMCTRL3, MSK_TERM_P2, VAL_TERM_P2_MHL);
            break;
        case 3:
            SiiRegModify(REG_TMDS_TERMCTRL3, MSK_TERM_P3, VAL_TERM_P3_MHL);
            break;
        default:
            break;
    }

#if 0//(INC_9617 == 1)
    SiiRegModify( REG_IP_CONFIG_43, BGND_DISABLE, SET_BITS );
#endif

    regVal = SiiRegRead(REG_OSC_OTP_CALIB_20MHz);
    regVal &= BIT4|BIT3|BIT2|BIT1|BIT0;

    if ( regVal <= 3 )
    {
    	SiiRegModify( REG_OSC_CTRL_20MHz, BIT7|BIT4|BIT3|BIT2|BIT1|BIT0, 0x80 );
    }
    else
    {
    	SiiRegModify( REG_OSC_CTRL_20MHz, BIT7|BIT4|BIT3|BIT2|BIT1|BIT0, (regVal - 3) | 0x80 );
    }

}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceConfigure
// Description: Before it is initialized for use, the device must be properly
//              configured for the target system requirements.  This function
//              is called to configure the parts of the device that may not
//              directly fall under the auspices of one of the other components.
// Parameters:  none
// Returns:     true if the configuration was successful, or false if some
//				failure occurred
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceConfigure ( void )
{

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceInitialize
// Description: This function disables the device or places the device in
//				software reset if it does not power up in a disabled state.
//				It may be used to initialize registers that require a value
//				different from the power-up state and are common to one or
//				more of the other component modules.
// Parameters:  none
// Returns:     It returns true if the initialization is successful, or false
//				if some failure occurred.
//
// Note:		This function leaves the device in a disabled state until the
//				SiiDrvDeviceRelease function is called.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceInitialize ( void )
{
    bool_t success = true;
    bool_t isSvnRevision = false;
    uint_t bootAttempts  = 5; // number of attempts to boot after h/w reset before failing
    Printf("\r\nSiiDrvDeviceInitialize\n" );

    for (;;)
    {
        while (!SiiDrvDevicePowerUpBoot())
        {
            Printf("\r\nBoot failed! Toggling H/W RESET.\n" );
            if (bootAttempts == 0)
            {
                success = false;
                break;
            }
                #if 0
            // if booting fails, toggle h/w reset
            SiiPlatformGpioMainReset();
            SiiOsTimerWait(300);
                 #endif
            bootAttempts--;
        }

        if (!success)
            break;
#if (FPGA_BUILD == ENABLE)
        // To retrieve SVN revision of FPGA image, not the DEV_ID
        SiiRegModify(REG_DEV_ID_TYPE, BIT_DEV_ID_SVN_REV, SET_BITS);
        isSvnRevision = true;
#endif

        pDrvDevice->devType = SiiRegRead(REG_DEV_IDH_RX);
        pDrvDevice->devType = (pDrvDevice->devType << 8) | SiiRegRead(REG_DEV_IDL_RX);
        pDrvDevice->devRev = SiiRegRead(REG_DEV_REV);
            Printf("\r\nRTL SVN Rev.: %x\n", pDrvDevice->devType);

        if (!isSvnRevision)
        {   // Silicon revision type
            if ((pDrvDevice->devType & 0xFF00) != 0x9600)
            {
                pDrvDevice->lastResultCode = SiiDevice_FAIL_WRONGDEVICE;
                success = false;
            }
        }
        else
        {
            Printf("\r\nRTL SVN Rev.: %x\n", pDrvDevice->devType);
        }
        success = true;
        break;
    }

    return (success);
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceStatus
// Description: Returns a status flag word containing device-specific
//				information about the state of the device.
// Parameters:  none
// Returns:     Sixteen-bit Device status flags word for the Device Component
//------------------------------------------------------------------------------

uint_t SiiDrvDeviceStatus ( void )
{
	return( pDrvDevice->statusFlags );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceStandby
// Description: Places the device into standby mode if available
// Parameters:  none
// Returns:     true if the device entered standby mode successfully, or false
//				if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceStandby ( void )
{
	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceResume
// Description: Causes operation of the device to return to the state it was
//				prior to the previous call to SiiDeviceStandby.
// Parameters:  none
// Returns:     true if the device successfully exited standby mode , or false
//				if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceResume ( void )
{

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDeviceRelease
// Description: Enables the device for normal operation and is called as the
//				last part of device initialization.
//				Since the SiiDeviceInitialize function leaves the device output
//				disabled, this function should enable the device output as a
//				minimum.  All other configuration and initialization functions
//				must be called in between SiiDeviceInitialize and SiiDeviceRelease.
// Parameters:  none
// Returns:     true if the release is successful and the device is ready for
//				operation or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceRelease ( void )
{
    /* Release software reset and finite state machine. Must delay between the two events. */
    Printf("\r\n SiiDrvDeviceRelease..\n");
    SiiRegModify( REG_SRST, BIT_SWRST, CLEAR_BITS );
   // SiiOsTimerWait( 120 );
    SiiRegWrite( REG_PAUTH_CTRL, BIT_IGNORE_PAUTH_HPD | BIT_SKIP_NON_HDCP | BIT_PORT_FSM_EN  );

	return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceStart
// Description: Enables the device for normal operation.  This function performs
//              the procedures necessary for correct operation of the device after
//              all other initialization has been performed.
// Parameters:  none
// Returns:     true if the release is successful and the device is ready for
//              operation or false if some failure occurred.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceStart ( void )
{
    /* Enable EDID and HPD manually so that when we release Auto-HPD,   */
    /* the power-up state is maintained.                                */

    SiiDrvHpdSetState( SiiPORT_ALL, SiiHPD_ACTIVE );
    #if 0 ///i2c9687   
	if(1 == GetCurrentSourceInputVarant())//zxu1
	{
		SiiRegModify( REG_RX_PORT_SEL, MSK_PORT_ALL, 1 );
		Printf("\r\n@@@@Jane: app.portSelect = 1.\n");
	}
	else 
	{
		SiiRegModify( REG_RX_PORT_SEL, MSK_PORT_ALL, 0 );
		Printf("\r\n@@@@Jane: app.portSelect = 0.\n");
	}
    #endif 
    // Perform a long HPE event to kick start all attached devices.
    // It is an attempt to make the connected sources think that they have
    // not been connected until now and to restart everything.

    Printf("\r\n\\_____MP Full HPE_____/\n" );
    SiiDrvSwitchPipeHpeTrigger( SiiPipeType_Main, true );
	SiiDrvSwitchPipeHpeTrigger( SiiPipeType_Background, true );//zxu1

    return( true );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceGetLastResult
// Description: Returns the result of the last SiiDevicexxx function called
// Parameters:  none
// Returns:     Result of last device driver function.
//------------------------------------------------------------------------------

int_t SiiDrvDeviceGetLastResult ( void )
{

	return( pDrvDevice->lastResultCode );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceBootInProgress
// Description: see if boot is in progress
// Parameters:  none
// Returns:     true if device booted correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceBootInProgress ( void )
{
    uint8_t temp;

    temp = SiiRegRead( REG_BSM_STAT );
	if ( temp & BIT_BOOT_INPROGRESS )
	{
		return true;
	}
	else
	{
		return false;
	}

}


//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceBootComplete
// Description: Wait for EDID Boot Done status.
// Parameters:  none
// Returns:     true if device booted correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceBootComplete ( void )
{
    uint8_t     bsmStatus;
    bool_t      success = false;
    bool_t      isFirstRun = true;
    SiiTimer_t  bootTimer;

    /* Wait for boot loading to be done.    */

    SiiOsTimerSet( &bootTimer, 1 );
    for ( ;; )
    {
        bsmStatus = SiiRegRead( REG_BSM_STAT );
        if ( bsmStatus & BIT_BOOT_DONE )
        {
        	success = true;
            break;
        }

 //       if ( ( bsmStatus & BIT_BOOT_ERROR ) || (SiiOsTimerElapsed( bootTimer ) >= 40) ) // 4 second timeout for 100ms polling
        if ( ( bsmStatus & BIT_BOOT_ERROR ) || (SiiOsTimerElapsed( bootTimer ) >= 4000) ) // 4 second timeout
        {
            if (isFirstRun)
            {
                // For empty parts this will force BIT_BOOT_DONE = 1
                SiiRegWrite(REG_NVM_BSM_REPLACE, 0xFF);
                SiiOsTimerSet( &bootTimer, 1 );
                isFirstRun = false;
            }
            else
            {
                // Fail if second attempt fails
                pDrvDevice->lastResultCode = SiiDevice_FAIL_BOOT;
                break;
            }
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
//  Function:       SiiDrvDeviceNvramInitialize
//  Description:    Check to see if the NVRAM has been initialized and do it if needed.
//------------------------------------------------------------------------------

bool_t SiiDrvDeviceNvramInitialize ( bool_t forceInit )
{
    bool_t success = true;
	bool_t NeedReload = false;

    pDrvDevice->lastResultCode = SiiDevice_SUCCESS;  // Start out OK.

#if (FPGA_BUILD == DISABLE)
    if (( SiiRegRead( REG_NVM_STAT ) != VAL_NVM_VALID ) || forceInit )
#endif
	{
		NeedReload = 1;
		Printf("\r\n\nNVRAM Boot Data Empty and Chip is using firstly....");
	}

	// Check whether EDID table was changed?
	// Block 1 checksum check....
	if ( SiiRegRead( REG_CHECKSUM_CH0 + HDMI_IN1 ) != gEdidFlashEdidTable[EDID_TABLE_LEN-1] )
	{
		NeedReload = 1;
		Printf("\r\n\nEDID Block 1 checksum was changed....");
	}
	
	// Block 0 checksum check....
	SiiRegWrite( REG_EDID_FIFO_SEL, BIT_SEL_EDID0 );
	SiiRegWrite( REG_EDID_FIFO_ADDR, (EDID_TABLE_LEN / 2) -1 );
	if ( SiiRegRead( REG_EDID_FIFO_DATA ) != gEdidFlashEdidTable[EDID_TABLE_LEN/2-1] )
	{
		NeedReload = 1;
		Printf("\r\n\nEDID Block 0 checksum was changed....");
	}

	if ( NeedNvramReload(NeedReload) )
    {
		Printf("\r\n\nNVRAM EDID and Boot Data Reload...");
		
        Printf("\r\n\nNVRAM NOT INITIALIZED, initializing..." );
        success = SiiEdidNvramWrite( EDID_NVRAM_BOOT, (uint8_t *)&gEdidFlashDevBootData );
        Printf("[%s][%d]\n",__FUNCTION__,__LINE__);
//        if ( success )
//        {
//            success = SiiEdidNvramWrite( EDID_NVRAM_VGA, gEdidFlashEdidVgaTable );
//        }

		if ( success )
		{
			success = SiiEdidValidHdmi( gEdidFlashEdidTable );
			if ( !success )
			{
				Printf("\r\n\nIncorrect EDID table, Please check it. No action taken.....");
			}
		}

        if ( success )
        {
            Printf("[%s][%d]\n",__FUNCTION__,__LINE__);
            success = SiiEdidNvramWrite( EDID_NVRAM, gEdidFlashEdidTable );
            Printf("[%s][%d]\n",__FUNCTION__,__LINE__);
            /* Force a boot load to get the new EDID data from the NVRAM to the chip.   */

            SiiRegWrite( REG_BSM_INIT, BIT_BSM_INIT );
            success = SiiDrvDeviceBootComplete();
            Printf("[%s][%d]\n",__FUNCTION__,__LINE__);
        }

        if ( success )
        {
            Printf("\r\nsuccessful!222222222222222\n" );
        }
        else
        {
            Printf("\r\nFAILED2222222222222222\n" );
            pDrvDevice->lastResultCode = SiiDevice_FAIL_NVRAM_INIT;
        }
    }

    return( success );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDevicePowerUpBoot
// Description: Perform the device startup sequence.  Reset, wait for boot to
//				complete, optionally initialize EDID NVRAM, and initialize
//				device registers.
// Parameters:  none
// Returns:     true if device booted and initialized correctly
//------------------------------------------------------------------------------

bool_t SiiDrvDevicePowerUpBoot ( void )
{
    pDrvDevice->lastResultCode = SiiDevice_SUCCESS;  // Start out OK.

   // Force a soft Hard Reset to ensure that the Always On Domain logic is reset.
    Printf("\r\nSiiDrvDevicePowerUpBoot\n" );
#if 0
    SiiRegWrite( REG_SPECIAL_PURPOSE, BIT_HARDRESET );
    SiiRegWrite( REG_SPECIAL_PURPOSE, CLEAR_BITS );
#else
// Force a soft Hard Reset to ensure that the Always On Domain logic is reset.

    SiiRegWrite( REG_SPECIAL_PURPOSE, BIT_HARDRESET );
    DEBUG_PRINT( MSG_ALWAYS, "REG_SPECIAL_PURPOSE_1 = 0x%x. \n", SiiRegRead(REG_SPECIAL_PURPOSE) );

    DEBUG_PRINT( MSG_ALWAYS, "REG_SLAVE_ADDR3_3 = 0x%x. \n", SiiRegRead(REG_SLAVE_ADDR3) );
    SiiRegWrite( REG_SLAVE_ADDR3, 0xE2 );//zxu1

    DEBUG_PRINT( MSG_ALWAYS, "REG_SLAVE_ADDR3_4 = 0x%x. \n", SiiRegRead(REG_SLAVE_ADDR3) );

    SiiRegWrite( REG_SPECIAL_PURPOSE, CLEAR_BITS );
    DEBUG_PRINT( MSG_ALWAYS, "REG_SPECIAL_PURPOSE_2 = 0x%x. \n", SiiRegRead(REG_SPECIAL_PURPOSE) );

    DEBUG_PRINT( MSG_ALWAYS, "REG_SLAVE_ADDR3_5 = 0x%x. \n", SiiRegRead(REG_SLAVE_ADDR3) );
    SiiRegWrite( REG_SLAVE_ADDR3, 0xE2 );//zxu1
    DEBUG_PRINT( MSG_ALWAYS, "REG_SLAVE_ADDR3_6 = 0x%x. \n", SiiRegRead(REG_SLAVE_ADDR3) );
#endif
    
	SiiRegWrite( REG_HP_CTRL, VAL_HP_PORT_ALL_HI );

    // Wait for boot loading to be done.

    for ( ;; )
    {
        if ( !SiiDrvDeviceBootComplete() )
        {
            break;
        }
            // Check NVRAM status to determine if it must be initialized (first time boot).
            // We do this here because NVRAM contains some register default information
            // as well as the correct EDID.  If the NVRAM is not initialized, we must
            // initialize it and then reload so that the appropriate registers are
            // loaded.

#if (DO_EDID_INIT == 1)
        if ( !SiiDrvDeviceNvramInitialize( false ))
//      if ( !SiiDrvDeviceNvramInitialize( true ))   //jane add 20130125
        {
            break;
        }
#endif
        // Initialize registers to the Programmers Reference power-up defaults.
        DrvDeviceInitRegisters();
        break;
    }

    return( pDrvDevice->lastResultCode == SiiDevice_SUCCESS );
    //NOTE: Caller must call SiiDrvDeviceRelease() function to bring out of reset
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceInfo
// Description: Return the requested information from the device.
//------------------------------------------------------------------------------

uint_t SiiDrvDeviceInfo ( SiiDrvDeviceStatus_t infoIndex )
{
    uint8_t uData;
    uint_t regValue = 0;

    switch ( infoIndex )
    {
        case SiiDEV_INPUT_CONNECTED:
            // P3:0 of PWR5V status
            uData = SiiRegRead( REG_PWR5V_STATUS );
            regValue = (uData & 0x0F);
            // Add in CBUS ports connected
            regValue |= SiiRegRead( REG_CBUS_DEV_CONNECTED );
            break;
        case SiiDEV_BOOT_STATE_MACHINE:
            regValue = SiiRegRead( REG_BSM_STAT );
            break;
        case SiiDEV_NVRAM:
            regValue = (SiiRegRead( REG_NVM_STAT ) == VAL_NVM_VALID);
            break;
        case SiiDEV_ID:
            regValue = pDrvDevice->devType;
            break;
        case SiiDEV_REV:
            regValue = pDrvDevice->devRev;
            break;
        case SiiDEV_ACTIVE_PORT:
            regValue = SiiRegRead( REG_RX_PORT_SEL ) & MSK_PORT_ALL;
            break;
        case SiiDEV_SELECTED_PORT_BITFIELD:
            regValue = ((SiiRegRead( REG_PAUTH_STAT0 ) & ROVE_PIPE_MASK) >> 4) | (( SiiRegRead( REG_PAUTH_STAT0 ) & MAIN_PIPE_MASK));
            break;
    }

    return( regValue );
}

//------------------------------------------------------------------------------
// Function:    SiiDrvDeviceManageInterrupts
// Description: Monitors Sii9687 interrupts and calls an interrupt processor
//              function in the applicable driver if an interrupt is encountered.
// NOTE:    This function is not designed to be called directly from a physical
//          interrupt handler unless provisions have been made to avoid conflict
//          with normal level I2C accesses.
//          It is intended to be called from normal level by monitoring a flag
//          set by the physical handler.
//------------------------------------------------------------------------------

void SiiDrvDeviceManageInterrupts ( uint_t inst )
{
    uint8_t     intStatus;

		 
    // Check the hardware interrupt
#if 0
    if ( !SiiPlatformInterruptStateGet() )
    {
        return;
    }
#endif
    intStatus = SiiRegRead( REG_INTR_STATE );    // Get the register interrupt info

    // Determine the pending interrupts and service them with driver calls
    // Each function will call its appropriate callback function if a status
    // change is detected that needs upper level attention.

    if (( intStatus & BIT_INTR_GRP0 ) || ( intStatus & BIT_INTR_GRP1 ) || ( intStatus & BIT_INTR_GRP_PA ))   // Tx0 and PA interrupts
    {
   // Printf("\r\nSiiDrvDeviceManageInterrupts intStatus=%02x...\n",intStatus);
        SiiDrvTxSimpleProcessInterrupts();
        SiiDrvSwitchProcessInterrupts();
        SiiDrvGpioProcessInterrupts();
#if INC_IPV
        SiiDrvIpvProcessInterrupts();
#endif

	// SiiSchedSendSignal(SI_EVENT_ID_PLATFORM_DEV_INT, 0);
    }

    if ( intStatus & BIT_INTR_GRP5 )
    {
        SiiDrvCbusProcessInterrupts();
	//SiiSchedSendSignal(SI_EVENT_ID_PLATFORM_DEV_INT, 0);
    }


    // Check if all interrupts are cleared so that h/w interrupt line goes high.
    // If the interrupt line is still low, don't clear the interrupt status
    // and send INT signal to process and clear pending interrupts in the next round
#if 0
    SiiPlatformInterruptDisable();
    if (SiiPlatformInterruptPinStateGet())
    {
        SiiPlatformInterruptClear();
    }
    else
    {
        SiiSchedSendSignal(SI_EVENT_ID_PLATFORM_DEV_INT, 0);
    }
    SiiPlatformInterruptEnable();
#else
	
#endif

    return;
}

//------------------------------------------------------------------------------
//  Function:       NeedNvramReload
//  Description:    Check to see if the NvramReload is needed.
//------------------------------------------------------------------------------

bool_t NeedNvramReload ( bool_t NvramReload )
{
	if ( NvramReload )
	{
		return ( true );
	}
	else
	{
		return ( false );
	}
}

//------------------------------------------------------------------------------
//  Function:       SiiEdidValidHdmi
//  Description:    Validate the passed EDID as an HDMI EDID.                                     
//------------------------------------------------------------------------------
static uint8_t SiiEdidValidHdmi ( const uint8_t *pData )
{
    uint16_t    i;
    uint8_t     accum0 = 0, accum1 = 0;
    uint16_t    edidOffset;

       /* Check the checksum for the first EDID block. */
	   
    for ( i = 0; i < (EDID_TABLE_LEN / 2); i++ )
    {
        accum0 += pData[ i];             // Gather checksum info
    }
    if ( accum0 != 0 )
    {
        return( false );
    }
	
        /* Locate 24-bit IEEE Registration number for Vendor-specific data. */
        /* For this search, I am not looking for CEA Extension or a Vendor- */
        /* specific data block.  Rather, I am assuming that the 24-bit      */
        /* IEEE code will be unique enough.  This may not hold up.          */

    for ( edidOffset = (EDID_TABLE_LEN / 2); edidOffset < EDID_TABLE_LEN; edidOffset++ )
    {
        if ( pData[ edidOffset] == 0x03 )      // 0x000C03 belongs to HDMI Licensing, LLC
        {
            if ( pData[ edidOffset + 1] == 0x0C )
            {
                if ( pData[ edidOffset + 2] == 0x00 )
                {
                    edidOffset += 3;        // Point to CEC Phys Address field.
                    break;
                }
            }
        }
    }

        /* If the physical address signature is found, */
        /* do a checksum on the whole thing.          */

    if ( edidOffset < EDID_TABLE_LEN )
    {
            /* Calculate the new checksum and update the array.  */

        for ( i = (EDID_TABLE_LEN / 2); i < EDID_TABLE_LEN; i++ )
        {
            accum1 += pData[ i];
        }
    }

    if (( edidOffset == EDID_TABLE_LEN ) || ( accum1 != 0 ))
    {
        return( false );
    }
    return( true );
}

