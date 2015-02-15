//***************************************************************************
//!file     sk_app_arc.c
//!brief    Wraps board and device functions for the ARC component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "si_drv_arc.h"
#include "si_drv_switch.h"

#if 0
#include "../driver/arc_drv/si_drv_arc.h"
#include "../driver/switch_drv/si_drv_switch.h"
#endif


static void EnableArc(bool_t isEnabled)
{
    app.arcEnabled = isEnabled;
    SiiDrvArcSingleModeEnable(isEnabled);
  //  SiiPlatformGpioLedArc(isEnabled);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Perform any board-level initialization required at the same
//!             time as ARC component initialization
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SkAppDeviceInitArc ( void )
{
    bool_t  success = true;
    Printf("\r\n SkAppDeviceInitArc...\n");
    EnableArc(true);
    return( success );
}


static void ArcDemoMenuPrint(void)
{
    Printf("\r\n\nARC Demo\n" );
    Printf("\r\n--------\n" );
    Printf("\r\n[CC] - Display this menu\n" );
    Printf("\r\n0 - Return to main Demo Menu\n" );
    Printf("\r\n1 - Enable ARC\n" );
    Printf("\r\n2 - Disable ARC\n" );

    // Fill up LCD menu
    app.lcdMenu[0] = "Return to main";
    app.lcdMenu[1] = "Enable ARC";
    app.lcdMenu[2] = "Disable ARC";
    SkAppLcdMenuListSet(app.lcdMenu, 3);
}

//-------------------------------------------------------------------------------------------------
//! @brief      ARC demo command options
//!
//! @param[in]  key - RC command code
//!
//! @return
//-------------------------------------------------------------------------------------------------

bool_t SkAppArcDemoMenu ( RC5Commands_t key )
{

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        key = RC5_0 + SkAppLcdMenuSelectionGet();
    }

    switch ( key )
    {
        case RC5_SELECT:
            ArcDemoMenuPrint();
            break;

        case RC5_0:
            app.demoMenuLevel = 0;
            Printf("\r\n\n Demo Menu -- Press MENU key to select Demo\n" );
            SkAppDemoMenuPrint();
            break;
        case RC5_1:
            EnableArc(true);
            break;
        case RC5_2:
            EnableArc(false);
            break;
        default:
            break;
    }

    return( false );
}

