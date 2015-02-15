//***************************************************************************
//!file     sk_app_ipv_demo.c
//!brief    InstaPrevue Demo
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"

#if INC_IPV

static void IpvDemoMenuPrint(void)
{
    Printf("\r\n\nInstaPrevue Demo\n" );
    Printf("\r\n--------\n" );
    Printf("\r\n[CC] - Display this menu\n" );
    Printf("\r\n0 - Return to main Demo Menu\n" );
    Printf("\r\n1 - Top Center\n" );
    Printf("\r\n2 - Bottom Center\n" );
    Printf("\r\n3 - Left Middle\n" );
    Printf("\r\n4 - Right Middle\n" );
    Printf("\r\n5 - Triangle\n" );
    Printf("\r\n  - \n" );
    Printf("\r\n6 - Mode: All Ports\n" );
    Printf("\r\n7 - Mode: Only Active Ports\n" );
    Printf("\r\n8 - Mode: Picture-In-Picture\n" );
    Printf("\r\n9 - InstaPrevue Off\n" );

    // Fill up LCD menu
    app.lcdMenu[0] = "Return to main";
    app.lcdMenu[1] = "Top Center";
    app.lcdMenu[2] = "Bottom Center";
    app.lcdMenu[3] = "Left Middle";
    app.lcdMenu[4] = "Right Middle";
    app.lcdMenu[5] = "Triangle";
    app.lcdMenu[6] = "M: All Ports";
    app.lcdMenu[7] = "M: Active Ports";
    app.lcdMenu[8] = "M: PIP";
    app.lcdMenu[9] = "IPV Off";
    SkAppLcdMenuListSet(app.lcdMenu, 10);
}

//-------------------------------------------------------------------------------------------------
//! @brief      InstaPrevue demo command options
//!
//! @param[in]  key - RC command code
//!
//! @return
//-------------------------------------------------------------------------------------------------

bool_t SkAppIpvDemoMenu ( RC5Commands_t key )
{
    int_t   port;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        key = RC5_0 + SkAppLcdMenuSelectionGet();
    }

    switch ( key )
    {
        case RC5_SELECT:
            IpvDemoMenuPrint();
            break;

        case RC5_0:
            app.demoMenuLevel = 0;
            Printf("\r\n\n Demo Menu -- Press MENU key to select Demo\n" );
            SkAppDemoMenuPrint();
            break;
        case RC5_1:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SiiIpvThemeSet( SiiIpvTOP_CENTER );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvTOP_CENTER;
            app.ipvEnabled = true;
            SkAppTaskIpv(0);
            break;
        case RC5_2:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SiiIpvThemeSet( SiiIpvBOTTOM_CENTER );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvBOTTOM_CENTER;
            app.ipvEnabled = true;
            SkAppTaskIpv(0);
            break;
        case RC5_3:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SiiIpvThemeSet( SiiIpvLEFT_MIDDLE );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvLEFT_MIDDLE;
            app.ipvEnabled = true;
            SkAppTaskIpv(0);
            break;
        case RC5_4:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SiiIpvThemeSet( SiiIpvRIGHT_MIDDLE );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvRIGHT_MIDDLE;
            app.ipvEnabled = true;
            SkAppTaskIpv(0);
            break;
        case RC5_5:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SiiIpvThemeSet( SiiIpvPentagram );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvPentagram;
            app.ipvEnabled = true;
            SkAppTaskIpv(0);
            break;
        case RC5_6:
            SiiIpvModeSet( SiiIpvModeALL );
            SkAppTaskIpv(0);
            break;
        case RC5_7:
            SiiIpvModeSet( SiiIpvModeACTIVE );
            SkAppTaskIpv(0);
            break;
        case RC5_8:
            Printf("\r\n Select PIP Port number (0-%d): \n", SII_INPUT_PORT_COUNT - 1 );
            port = SkAppIrGetSingleDigit( SII_INPUT_PORT_COUNT - 1 );
            SkAppIpvPipModeEnable( port );
            app.ipvPipEnabled = true;
            break;
        case RC5_9:
            SiiIpvEnable( false );
            app.ipvPipEnabled = false;
            app.ipvEnabled = false;
            SkAppTaskIpv(0);
            break;
        default:
            break;
    }

    return( false );
}
#endif
