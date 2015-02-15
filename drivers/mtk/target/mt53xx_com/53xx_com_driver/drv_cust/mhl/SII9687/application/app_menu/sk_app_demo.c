//***************************************************************************
//! @file     sk_app_demo.c
//! @brief    Built-in Demo manager
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_drv_internal.h"
#include "si_drv_arc.h"
#include "si_drv_cbus.h"
#include "si_drv_tx_simple.h"
#include "si_drv_diag.h"
#include "si_drv_tpg.h"


static void InternalVideoEnable ( bool_t isEnabled, tpgVideoFormat_t tpgFormat, tpgVideoPattern_t tpgPattern,
                                  tpgClockSource_t clkSrc )
{
    if (isEnabled)
    {
        // Enable Internal video generator
        SiiDrvExtTpgPatternSet(tpgPattern);
        SiiDrvTpgVideoFormatSet(tpgFormat, clkSrc);
        SiiDrvExtTpgEnable(true);
        SiiDrvTxSimpleTmdsEnable(true);
    }
    else
    {
        SiiDrvTpgVideoFormatSet(tpgFormat, SI_TPG_CLK_MAIN_PIPE);
    	SiiDrvExtTpgEnable(false);
    }


#if 0//INC_LCD_DISP
    {
        char *psfmt;
        SI_LCD_CLEAR();
        SI_LCD_PRINTF(0, "Int. Video %s", isEnabled ? "On" : "Off");
        if (isEnabled)
        {
            switch (tpgFormat)
            {
                case  SI_TPG_FMT_VID_720_60: psfmt = "720x60"; break;
                case  SI_TPG_FMT_VID_720_50: psfmt = "720x50"; break;
                case  SI_TPG_FMT_VID_576_50: psfmt = "576x50"; break;
                case  SI_TPG_FMT_VID_480_60: psfmt = "480x60"; break;
                case  SI_TPG_FMT_VID_1080_50_8bpp:  psfmt = "1080x50 8b"; break;
                case  SI_TPG_FMT_VID_1080_50_10bpp: psfmt = "1080x50 10b"; break;
                case  SI_TPG_FMT_VID_1080_50_12bpp: psfmt = "1080x50 12b"; break;
                case  SI_TPG_FMT_VID_1080_60_8bpp:  psfmt = "1080x60 8b"; break;
                case  SI_TPG_FMT_VID_1080_60_10bpp: psfmt = "1080x60 10b"; break;
                case  SI_TPG_FMT_VID_1080_60_12bpp: psfmt = "1080x60 12b"; break;
                default: psfmt = "Unknown"; break;
            }
            SI_LCD_PRINTF(1, "%s c:%s", psfmt, clkSrc ? "IN":"MP");
        }
    }
#endif
}


static void TestPatternSelectDemoMenuPrint(void)
{
    Printf("\r\n\nSelect Video Pattern:\n" );
    Printf("\r\n1 - Red\n" );
    Printf("\r\n2 - Green\n" );
    Printf("\r\n3 - Blue\n" );
    Printf("\r\n4 - Black\n" );
    Printf("\r\n5 - White\n" );
    Printf("\r\n6 - Ramp\n" );
    Printf("\r\n7 - Chess\n" );
    Printf("\r\n8 - Color Bars\n" );

    // Fill up LCD menu
    app.lcdMenu[0] = "Return to main";
    app.lcdMenu[1] = "Red";
    app.lcdMenu[2] = "Green";
    app.lcdMenu[3] = "Blue";
    app.lcdMenu[4] = "Black";
    app.lcdMenu[5] = "White";
    app.lcdMenu[6] = "Ramp";
    app.lcdMenu[7] = "Chess";
    app.lcdMenu[8] = "Color Bars";
    SkAppLcdMenuListSet(app.lcdMenu, 9);
}

static bool_t TestPatternSelectDemo ( RC5Commands_t key )
{
    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        key = RC5_0 + SkAppLcdMenuSelectionGet();
    }

    switch ( key )
    {
        case RC5_SELECT:
            TestPatternSelectDemoMenuPrint();
            break;

        case RC5_0:
            app.demoMenuLevel = 0;
            Printf("\r\n\n\nDemo Menu -- Press MENU key to select Demo\n" );
            SkAppDemoMenuPrint();
            break;
        case RC5_1:
        	app.tpgPattern = SI_ETPG_PATTERN_RED;
        	Printf("\r\nRED\n" );
            break;
        case RC5_2:
        	app.tpgPattern = SI_ETPG_PATTERN_GREEN;
        	Printf("\r\nGREEN\n" );
            break;
        case RC5_3:
        	app.tpgPattern = SI_ETPG_PATTERN_BLUE;
        	Printf("\r\nBLUE\n" );
            break;
        case RC5_4:
        	app.tpgPattern = SI_ETPG_PATTERN_BLACK;
        	Printf("\r\nBLACK\n" );
            break;
        case RC5_5:
        	app.tpgPattern = SI_ETPG_PATTERN_WHITE;
        	Printf("\r\nWHITE\n" );
            break;
        case RC5_6:
        	app.tpgPattern = SI_ETPG_PATTERN_RAMP;
        	Printf("\r\nRAMP\n" );
			break;
        case RC5_7:
        	app.tpgPattern = SI_ETPG_PATTERN_CHESS;
        	Printf("\r\nCHESS\n" );
            break;
        case RC5_8:
        	app.tpgPattern = SI_ETPG_PATTERN_COLORBAR;
        	Printf("\r\nCOLORBAR\n" );
            break;
        default:
            break;
    }

    if ((key > RC5_0) && (key <= RC5_8))
    {
        SiiDrvExtTpgPatternSet(app.tpgPattern);
    }
    return( false );
}


void EtpgDemoMenuPrint(void)
{
    Printf("\r\n\nInternal Video Generation Demo\n" );
    Printf("\r\n--------\n" );
    Printf("\r\n0 - Return to main Demo Menu\n" );
    Printf("\r\n1 - Internal Video Generation Demo display OFF\n" );
    Printf("\r\n2 - Internal Video Generation Demo display AUTO\n" );
    Printf("\r\n3 - Internal Video Generation Demo display /w Internal CLK\n" );
    Printf("\r\n4 - Set 480p60 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_480_60) ? "*" : "");
    Printf("\r\n5 - Set 576p50 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_576_50) ? "*" : "");
    Printf("\r\n6 - Set 720p60 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_720_60) ? "*" : "");
    Printf("\r\n7 - Set 720p50 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_720_50) ? "*" : "");
    Printf("\r\n8 - Set 1080p50 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_1080_50_8bpp) ? "*" : "");
    Printf("\r\n9 - Set 1080p60 mode %s\n", (app.tpgVideoFormat == SI_TPG_FMT_VID_1080_60_8bpp) ? "*" : "");
    Printf("\r\n\n Internal Video Pattern selection is in Main Demo menu.\n" );

    app.lcdMenu[0] = "Return to main  ";
    app.lcdMenu[1] = "Disable VPG";
    app.lcdMenu[2] = "Auto Clock VPG";
    app.lcdMenu[3] = "720p 60 ICLK";
    app.lcdMenu[4] = "480p 60";
    app.lcdMenu[5] = "576p 50";
    app.lcdMenu[6] = "720p 60";
    app.lcdMenu[7] = "720p 50";
    app.lcdMenu[8] = "1080p 50";
    app.lcdMenu[9] = "1080p 60";

    SkAppLcdMenuListSet(app.lcdMenu, 10);

}

//------------------------------------------------------------------------------
// Function:    DemoEtpg
// Description: TPG (fake video) demo command options
// Returns:
//------------------------------------------------------------------------------

static bool_t EtpgDemoMenu ( RC5Commands_t key )
{
    bool_t applyFmtChange = false;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        switch(SkAppLcdMenuSelectionGet())
        {
            case 0: key = RC5_0; break;
            case 1: key = RC5_1; break;
            case 2: key = RC5_2; break;
            case 3: key = RC5_3; break;
            case 4: key = RC5_4; break;
            case 5: key = RC5_5; break;
            case 6: key = RC5_6; break;
            case 7: key = RC5_7; break;
            case 8: key = RC5_8; break;
            case 9: key = RC5_9; break;
        }
    }

    switch ( key )
    {
        case RC5_SELECT:
            EtpgDemoMenuPrint();
            break;

        case RC5_0:
        case RC5_DECREMENT:
            app.demoMenuLevel = 0;
            Printf("\r\n\n Demo Menu -- Press MENU key to select Demo\n" );
            SkAppDemoMenuPrint();
            break;
        case RC5_1:
            app.tpgMode = APP_TPG_OFF;
        	SkAppDemoEtpgModeEnable(false);
            break;
        case RC5_2:
            app.tpgMode = APP_TPG_ON_AUTO_FMT;
        	SkAppDemoEtpgModeEnable(true);
            break;
        case RC5_3:
            app.tpgVideoFormat = SI_TPG_FMT_VID_720_60;
            app.tpgClockSrc = SI_TPG_CLK_INTERNAL;
            SkAppDemoEtpgModeEnable(true);
            app.tpgMode = APP_TPG_ON;
            Printf("\r\nInternal Video Generation Resolution: 720p60, VCLK = 74.25 MHz, Internal CLK\n" );
            break;

        case RC5_4:
            app.tpgVideoFormat = SI_TPG_FMT_VID_480_60;
            Printf("\r\nInternal Video Generation Resolution: 480p60, VCLK = 27.00 MHz\n" );
            applyFmtChange = true;
            break;
        case RC5_5:
            app.tpgVideoFormat = SI_TPG_FMT_VID_576_50;
            Printf("\r\nInternal Video Generation Resolution: 576p50, VCLK = 27.00 MHz\n" );
            applyFmtChange = true;
            break;
        case RC5_6:
            app.tpgVideoFormat = SI_TPG_FMT_VID_720_60;
            Printf("\r\nInternal Video Generation Resolution: 720p60, VCLK = 74.25 MHz\n" );
            applyFmtChange = true;
            break;
        case RC5_7:
            app.tpgVideoFormat = SI_TPG_FMT_VID_720_50;
            Printf("\r\nInternal Video Generation Resolution: 720p50, VCLK = 74.25 MHz\n" );
            applyFmtChange = true;
            break;
        case RC5_8:
            app.tpgVideoFormat = SI_TPG_FMT_VID_1080_50_8bpp;
            Printf("\r\nInternal Video Generation Resolution: 1080p50, VCLK = 148.00 MHz\n" );
            applyFmtChange = true;
            break;
        case RC5_9:
            app.tpgVideoFormat = SI_TPG_FMT_VID_1080_60_8bpp;
            Printf("\r\nInternal Video Generation Resolution: 1080p60, VCLK = 148.00 MHz\n" );
            applyFmtChange = true;
            break;
        default:
            break;
    }

    if ( applyFmtChange )
    {
        // Manual setting of resolution assumes using external clock
        app.tpgClockSrc     = SI_TPG_CLK_MAIN_PIPE;
        SkAppDemoEtpgModeEnable(true);
        app.tpgMode = APP_TPG_ON;
    }

    return( false );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Show video from extended internal pattern generator.
//!
//! @param[in]  isEnabled
//-------------------------------------------------------------------------------------------------

void SkAppDemoEtpgModeEnable ( bool_t isEnabled )
{
    if (isEnabled)
    {
        if ((app.tpgMode == APP_TPG_ON_AUTO_FMT) || (app.tpgMode == APP_TPG_AUTO_ENABLE))
        {
            SiiVideoFormat_t vFmt;
            if (app.tpgClockSrc == SI_TPG_CLK_INTERNAL)
            {
                app.tpgVideoFormat  = SI_TPG_FMT_VID_720_60;
            }
            else if(SiiDrvSwitchPortInfoGet( app.portSelect, true, &vFmt))
            {
                // Set VPG video mode compatible with MP TMDS clock
                switch(vFmt.tmdsClock)
                {
                    case 2700:  app.tpgVideoFormat = SI_TPG_FMT_VID_480_60; break;
                    case 7425:  app.tpgVideoFormat = SI_TPG_FMT_VID_720_60; break;
                    case 14850: app.tpgVideoFormat = SI_TPG_FMT_VID_1080_60_8bpp; break;
                    case 18562: app.tpgVideoFormat = SI_TPG_FMT_VID_1080_60_10bpp; break;
                    case 22275: app.tpgVideoFormat = SI_TPG_FMT_VID_1080_60_12bpp; break;
                    default: // don't change if TMDS clock is unknown
                        break;
                }
            }
        }

        InternalVideoEnable(true, app.tpgVideoFormat, app.tpgPattern, app.tpgClockSrc);
    }
    else
    {
        InternalVideoEnable(false, app.tpgVideoFormat, app.tpgPattern, app.tpgClockSrc);
        SkAppSwitchPortUpdate(true);
    }
}


void SkAppDemoMenuPrint(void)
{
    Printf("\r\n\n Demo Menu\n" );
    DEBUG_PRINT( MSG_ALWAYS,   " -----------\n" );
    Printf("\r\n0 - Return to TV Mode\n" );
    Printf("\r\n1 - CBUS Demo\n" );
    Printf("\r\n2 - VPG Demo\n" );
    Printf("\r\n3 - Change Video Pattern\n" );
    Printf("\r\n4 - ARC Demo\n" );
#if INC_IPV
    Printf("\r\n5 - InstaPrevue Demo\n" );
    Printf("\r\n\n Select Demo (0-5): " );
#else
    Printf("\r\n\n Select Demo (0-4): " );
#endif


    // Fill up LCD menu
    app.lcdMenu[0] = "TV Mode";
    app.lcdMenu[1] = "CBUS Demo";
    app.lcdMenu[2] = "VPG Demo";
    app.lcdMenu[3] = "Change Pattern";
    app.lcdMenu[4] = "ARC Demo";
#if INC_IPV
    app.lcdMenu[5] = "InstaPrv Demo";
    SkAppLcdMenuListSet(app.lcdMenu, 6);
#else
    SkAppLcdMenuListSet(app.lcdMenu, 5);
#endif


}

//-------------------------------------------------------------------------------------------------
//! @brief      Parse remote control keys for the Demo application
//!
//! @param[in]  key - RC command code
//! @param[in]  fullAccess  - true if allowed to respond to all normal keys,
//!                           false if only allowed to respond to power keys
//!
//! @return     Key used.
//! @retval     true  - key command consumed
//! @retval     false - key command wasn't processed
//-------------------------------------------------------------------------------------------------

bool_t SkAppRcDemoMenu ( RC5Commands_t key, bool_t fullAccess )
{
    bool_t  usedKey = false;

    switch ( key )
    {
        case RC5_MENU:
           SkAppDemoMenuPrint();
           app.demoMenuLevel = 0; // switch to menu idle mode
           usedKey = true;
           break;

        case RC5_0:
        case RC5_1:
        case RC5_2:
        case RC5_3:
        case RC5_4:
        case RC5_5:
            if (app.demoMenuLevel == 0)
            {
               app.demoMenuLevel = key - RC5_0 + 1;
               // Leave the key unused so that it gets passed to the new mode to display its menu
               key = RC5_SELECT;    // Cause new level menu to be displayed
            }
           break;

        case RC5_INCREMENT: // OK
        case RC5_ENTER:
            if (app.demoMenuLevel == 0)
            {
                app.demoMenuLevel = SkAppLcdMenuSelectionGet() + 1;
                key = RC5_SELECT;    // Cause new level menu to be displayed
            }
            break;

        default:
            break;
    }


    switch ( app.demoMenuLevel )
    {
        case 0:     // Menu IDLE mode
            break;
        case 1:
            SkAppIrRemoteReturnToTV();
            break;
        case 2:
            SkAppCbusDemoMenu( key );
            break;
        case 3:
        	EtpgDemoMenu( key );
            break;
        case 4:
            TestPatternSelectDemo( key );
            break;
        case 5:
            SkAppArcDemoMenu( key );
            break;
#if INC_IPV
        case 6:
            SkAppIpvDemoMenu( key );
            break;
#endif

        default:
            break;
    }

    return( usedKey );
}


