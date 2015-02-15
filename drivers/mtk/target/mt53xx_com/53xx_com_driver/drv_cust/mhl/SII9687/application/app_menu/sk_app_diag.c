//***************************************************************************
//!file     sk_app_diag.c
//!brief    Built-in diagnostics manager
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_sii9687_registers.h"
#include "si_debug.h"
#include "sk_application.h"


static char *termTypeMsgs [] =
{
    "50 ohm (default)",
    "50 ohm Data, 100 ohm Clock",
    "MHL mode termination",
    "---",
};

static char *hpdTypeMsgs [] =
{
    "force low",
    "normal",
    "tri-state",
    "reserved",
};

//-------------------------------------------------------------------------------------------------
//! @brief      Display major state indications of the Switch
//!
//-------------------------------------------------------------------------------------------------

static void DumpSwitchState ( void )
{
    int_t       i;
    bool_t      first = true;
    uint8_t     uData, uData1, mpPortBitMask, portIndex;
    uint16_t    uData16, mhl;
    uint8_t     pwr5vStatus, authStatus, decryptStatus, hDdc, eDdc, ckdtStatus;
    uint16_t    termCtrl, hpdCtrl, bcaps;

    // Overall state

    Printf("\r\n\n---- Switch (Rx) State ----\n" );
    uData = SiiRegRead( REG_STATE );
    Printf("\r\n PWD Power: %s\n", (uData & BIT_PWRON_STANDBY) ? "ON" : "STANDBY" );
    uData = SiiRegRead( REG_TMDST_CTRL1 );
    Printf("\r\n       Tx0: %s (%02X)\n",
        ((uData & (MSK_TMDS_OE | MSK_TMDS_EN)) == (MSK_TMDS_OE | MSK_TMDS_EN)) ? "Enabled" : "---", uData );
    uData = SiiRegRead( REG_PAUTH_CTRL );
    Printf("\r\n  Port FSM: %s (%02X)\n", (uData & BIT_PORT_FSM_EN) ? "Enabled" : "---", uData );
    Printf("\r\n\n" );
    uData = (SiiRegRead( REG_HPE_HEC_PORTS ) >> 2) & 0x1F;
    Printf("\r\nHEC/MHL ports: " );
    for ( i = 0; i < SII_INPUT_PORT_COUNT; i++ )
    {
        if ( uData & ( 1 << i))
        {
            if ( !first )
            {
                Printf("\r\n, " );
            }
            Printf("\r\n%d", i );
            first = false;
        }
    }
    Printf("\r\n (Bits: 0x%02X)\n\n", uData );

    // Main Pipe State

    Printf("\r\n----     Main Pipe     ----\n" );
    uData = SiiRegRead( REG_PAUTH_STAT0 ) & MAIN_PIPE_MASK;
    mpPortBitMask = uData;
    uData = SiiRegRead( REG_RX_PORT_SEL );
    portIndex = uData & MSK_PORT_ALL;
    Printf("\r\n      Port: %d (Bit: %02X)\n", portIndex, mpPortBitMask );
    if ( mpPortBitMask != (1 << portIndex))
    {
        Printf("\r\n** WARNING -- Index and port bits do not match!\n" );
        uData = SiiRegRead( REG_PAUTH_MPOVR );
        if ( uData & BIT_PAUTH_MAIN_OVR )
        {
            Printf("\r\n   User Port Select overridden\n" );
        }
    }

    uData = SiiRegRead( REG_STATE );
    mhl = uData & BIT_MHL;
    if( mhl )
    {
    	Printf("\r\n MHL Cable: %s\n", (uData & BIT_MHL) ? "Connected" : "---" );
    }
    else
    {
    	Printf("\r\nHDMI Cable: %s\n", (uData & BIT_PWR5V) ? "Connected" : "---" );
    }


    uData  = SiiRegRead( REG_HDCP_SHD_BSTAT2 );
    if ( mhl )
    {
    	Printf("\r\n      MODE: %s\n", (uData & BIT_HDMI_MODE) ? "MHL" : "DVI" );
    }
    else
    {
    	Printf("\r\n      MODE: %s\n", (uData & BIT_HDMI_MODE) ? "HDMI" : "DVI" );
    }

    uData = SiiRegRead( REG_STATE );
    Printf("\r\n      CKDT: %s\n", (uData & BIT_CKDT) ? "YES" : "NO" );
    Printf("\r\n      SCDT: %s\n", (uData & BIT_SCDT) ? "YES" : "NO" );
    uData16 = SiiRegRead( REG_ECC_ERRCOUNT_HI_MP );
    uData16 = (uData16 << 8) | SiiRegRead( REG_ECC_ERRCOUNT_LO_MP );
    Printf("\r\nECC Errors: %d\n", uData16 );


    Printf("\r\n\n----   Selected Port (%d)   ----\n", portIndex );
    uData = SiiRegRead( REG_PWR5V_STATUS );
    pwr5vStatus = ((uData & 0x0F) | ((uData >> 2) & 0x10));
    Printf("\r\n    Cable: %s\n", (pwr5vStatus & mpPortBitMask ) ? "Connected" : "---" );
    uData = SiiRegRead( REG_CLKDETECT_STATUS );
    ckdtStatus = ((uData & 0x0F) | ((uData >> 2) & 0x10));
    Printf("\r\n     CKDT: %s\n", (ckdtStatus & mpPortBitMask ) ? "YES" : "NO" );
    uData  = SiiRegRead( REG_PAUTH_STAT1 );
    authStatus = (uData & BIT_AUTH_MASK );
    Printf("\r\n     AUTH: %s\n", (authStatus & mpPortBitMask ) ? "YES" : "NO" );
    decryptStatus = (uData & BIT_DECRYPT_MASK ) >> 4;
    Printf("\r\n  DECRYPT: %s\n", (decryptStatus & mpPortBitMask ) ? "YES" : "NO" );

    uData = SiiRegRead( REG_HDCP_DDC_CTRL );
    hDdc = uData >> 4;
    Printf("\r\n HDCP DDC: %s\n", (hDdc & mpPortBitMask ) ? "Enabled" : "---" );

    uData = SiiRegRead( REG_EN_EDID );
    uData1 = uData & 0x0F;
    eDdc = uData1;
    Printf("\r\n EDID DDC: %s\n", (eDdc & mpPortBitMask) ? "Enabled" : "---" );

    termCtrl = SiiRegRead( REG_TMDS_TERMCTRL0 );
    Printf("\r\n     TERM: %s\n", termTypeMsgs[ (termCtrl >> (portIndex * 2)) & 0x03]);

    hpdCtrl = SiiRegRead( REG_HP_CTRL );
    Printf("\r\n      HPD: %s\n", hpdTypeMsgs[ (hpdCtrl >> (portIndex * 2)) & 0x03]);

    uData16 = ((((uint16_t)SiiRegRead( REG_HDCP_STAT )) & 0xC0) << 2   );
    bcaps   = uData16 | SiiRegRead( REG_HDCP_BCAPS_SET );
    Printf("\r\n Repeater: %s\n", ((bcaps >> (portIndex * 2)) & 0x02) ? "YES" : "NO" );
    if ((bcaps >> (portIndex * 2)) & 0x02)
    {
        Printf("\r\n** WARNING -- RX repeater BCAPS bit set in non-repeater mode\n" );
    }
    Printf("\r\n FIFO Rdy: %s\n", ((bcaps >> (portIndex * 2)) & 0x01) ? "YES" : "NO" );

    Printf("\r\n\n----     All Ports     ----\n" );
    Printf("\r\n    Cable: %02X\n", pwr5vStatus );
    Printf("\r\n     CKDT: %02X\n", ckdtStatus );
    if ( ckdtStatus & ~pwr5vStatus )
    {
        Printf("\r\n** WARNING -- CKDT Activity on unconnected ports\n" );
    }
    Printf("\r\n     AUTH: %02X\n", authStatus );
    Printf("\r\n  DECRYPT: %02X\n", decryptStatus );
    Printf("\r\n HDCP DDC: %02X\n", hDdc );
    Printf("\r\n EDID DDC: %02X\n", eDdc );
    Printf("\r\n     TERM: %03X\n", termCtrl );
    Printf("\r\n      HPD: %03X\n", hpdCtrl );
    Printf("\r\n    BCAPS: %03X\n", bcaps );


}

static int_t VerifyBlock(uint8_t pat, uint8_t *pData, int_t len)
{
    int_t i;
    int_t errCount = 0;

    for (i = 0; i < len; i++)
    {
        if (pData[i] != pat)
        {
            errCount++;
        }
    }

    return errCount;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Test NVRAM by repeated writing and reading data patterns
//!
//-------------------------------------------------------------------------------------------------

static void NvramEnduranceTest(void)
{
    const int_t fullCyclesCount = 256;
    int_t i, j;
    int_t totalErrCount = 0;
    int_t errCount = 0;
    bool_t success = false;
    uint8_t tp[EDID_TABLE_LEN];
    uint8_t pat;

    SI_LCD_CLEAR();

    for (j = 0; j < fullCyclesCount; j += 5)
    {
        pat = (uint8_t )j;

        // Generate test pattern table
        for (i = 0; i < EDID_TABLE_LEN; i++)
        {
            tp[i] = pat;
        }

        while (true)
        {
            Printf("\r\n\nWrite test pattern 0x%02X to NVRAM\n", pat );
            Printf("\r\nBoot..." );
           // SI_LCD_PRINTF(0, "PT:0x%02X WR BOOT", pat);

            tp[5] = 0; // Avoid writing non-zero into NVRAM Extra location 5. It may lock up I2C access.
            if (!SiiEdidNvramWrite(EDID_NVRAM_BOOT, tp))
                break;
            tp[5] = pat; // recover
            Printf("\r\nEdid..." );
           // SI_LCD_PRINTF(0, "PT:0x%02X WR EDID", pat);
            if (!SiiEdidNvramWrite(EDID_NVRAM, tp))
                break;
            Printf("\r\nVga...\n" );
           // SI_LCD_PRINTF(0, "PT:0x%02X WR VGA ", pat);
            if (!SiiEdidNvramWrite(EDID_NVRAM_VGA, tp))
                break;

            Printf("\r\nRead test pattern from NVRAM and verify\n" );
            Printf("\r\nBoot..." );
            if (!SiiEdidNvramRead(EDID_NVRAM_BOOT, tp))
                break;
            errCount = VerifyBlock(pat, tp, 64);
            if (errCount < 2)
            {
                errCount = 0; // ignore error caused by protecting location 5
            }
            totalErrCount += errCount;
            Printf("\r\n[%d]", errCount );

            Printf("\r\nEdid..." );
            if (!SiiEdidNvramRead(EDID_NVRAM, tp))
                break;
            errCount = VerifyBlock(pat, tp, 256);
            if (errCount < 4)
            {
                errCount = 0; // ignore errors caused by substitution of CS and PA (3 bytes total)
            }
            totalErrCount += errCount;
            Printf("\r\n[%d]", errCount );

            //Printf;
            if (!SiiEdidNvramRead(EDID_NVRAM_VGA, tp))
                break;
            errCount = VerifyBlock(pat, tp, 128);
            totalErrCount += errCount;
            Printf("\r\n[%d]\n", errCount );
          //  SI_LCD_PRINTF(1, "%d%% ERR:%d        ", (j * 100) / fullCyclesCount, totalErrCount);

            success = true;
            break;
        }

        if (!success)
        {
            Printf("\r\nNVRAM Operation failed!\n" );
            SkAppLcdMessageBox("NVRAM Operation", "FAILED!");
            break;
        }
    }

    // Recover NVRAM content
    if ( !SiiDrvDeviceNvramInitialize( true ))
    {
        Printf("\r\nNVRAM recovery failed!\n" );
        SkAppLcdMessageBox("NVRAM Recovery", "FAILED!");
    }
    else
    {
        Printf("\r\nNVRAM recovered successfully.\n" );
     //   SI_LCD_PRINTF(0,"NVRAM Recovered      ");
    }
}



static void DiagMenuPrint(void)
{
    Printf("\r\n\nDiagnostics\n" );
    Printf("\r\n-----------\n" );
    Printf("\r\nINFO - Display this menu\n" );
    Printf("\r\n0 - Return to main Setup Menu\n" );
    Printf("\r\n1 - Dump current Switch (RX) State\n" );
    Printf("\r\n2 - NVRAM Endurance Test\n" );
    Printf("\r\n\n Select (0-2): " );

    // Fill up LCD menu
    app.lcdMenu[0] = "Return to main  ";
    app.lcdMenu[1] = "NVRAM R/W Test";
    SkAppLcdMenuListSet(app.lcdMenu, 2);
}
//------------------------------------------------------------------------------
// Function:    SkAppDiagMenu
// Description: Handles diagnostics command options
// Returns:
//------------------------------------------------------------------------------

bool_t SkAppDiagMenu ( RC5Commands_t key )
{
    uint16_t        rcKey;
    bool_t          done = false;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        switch(SkAppLcdMenuSelectionGet())
        {
            case 0: key = RC5_0; break;
            case 1: key = RC5_2; break;
        }
    }

    switch ( key )
    {
        case RC5_SELECT:
            DiagMenuPrint();
            break;

        case RC5_0:
            app.setupMenuLevel = 0;
            //Printf("\r\n\n Setup Level -- Press MENU key to select Setup Menu\n" );
            SkAppSetupMenuPrint();
            break;

        case RC5_1:
            DumpSwitchState();
            break;

        case RC5_2:
            Printf("\r\n CAUTION: Test may damage the part! Press OK to Start: " );
            SkAppLcdMessageBox("!MAY LOCK CHIP!", "OK or Cancel ? ");
            done = false;
            while ( !done )
            {
                rcKey = SkAppGetOkCancelKey();
                switch ( rcKey )
                {
                    case RC5_INCREMENT:
                        NvramEnduranceTest();
                        done = true;
                        break;
                    case RC5_BUFFER_EMPTY:
                        break;
                    default:
                        Printf("\r\n---Canceled\n" );
                        SkAppLcdMessageBox("NVRAM R/W Test", "Canceled");
                        done = true;
                        break;
                }
            }
            break;

        case RC5_3:
            break;


        default:
            break;
    }

    return( false );
}

//------------------------------------------------------------------------------
// Function:    SkAppInitDiag
//! @brief      Initializes Diagnostics.
//------------------------------------------------------------------------------

bool_t SkAppInitDiag ( void )
{
    return( true );
}


