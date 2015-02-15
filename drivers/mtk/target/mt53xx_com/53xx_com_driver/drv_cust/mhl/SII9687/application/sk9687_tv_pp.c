//***************************************************************************
//! @file     sk9687_tv_pp.c
//! @brief    SiI9687 Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"

//------------------------------------------------------------------------------
//  Application Instance Data
//------------------------------------------------------------------------------
#if 0
AppInstanceData_t app;


//-------------------------------------------------------------------------------------------------
//!
//! @brief      Main entry point for SK9687 Starter Kit firmware.
//!
//-------------------------------------------------------------------------------------------------

int main(void)
{
    // Initialize application instance data
    SkAppInitInstanceData();

    // Initialize OS scheduler
    SiiSchedInitialize();

    //---------------------------------------------------------------------
    // Bind events to their handlers
    //---------------------------------------------------------------------
    // Link Device IRQ event to interrupt manager
    SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_DEV_INT, 0, SiiDrvDeviceManageInterrupts, 0);

    // Link driver interrupt handlers to their app tasks
#if INC_IPV
    SiiSchedEventHandlerBind(SI_EVENT_ID_IPV_INT, 0, SkAppTaskIpv, 0);
#endif
    SiiSchedEventHandlerBind(SI_EVENT_ID_SWITCH_INT, 0, SkAppTaskSwitch, 0);
    SiiSchedEventHandlerBind(SI_EVENT_ID_CBUS_INT, 0, SkAppTaskCbus, 0);
    SiiSchedEventHandlerBind(SI_EVENT_ID_GPIO_INT, 0, SkAppTaskGpio, 0);
    SiiSchedEventHandlerBind(SI_EVENT_ID_TX_INT, 0, SkAppSwitchOnTxMuteChange, 0);

    // Link platform interrupt handlers to tasks
    SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_IR_INT, 0, SkAppTaskIrRemote, 0);
    SiiSchedEventHandlerBind(SI_EVENT_ID_PLATFORM_JSTK_INT, 0, SkAppTaskIrRemote, 0);

    //---------------------------------------------------------------------
    // Platform Initialization
    //---------------------------------------------------------------------

    // Initialize the micro controller but not the the device
  //  if ( !SiiPlatformInitialize() )
  if (0)
    {
        SkAppBlinkTilReset( ERRLED_BAD_INIT );
    }

    SkAppSignon();

    // Check starter kit EEPROM and initialize it if necessary.
    if ( SkAppEepromInitialize( true, false ))
    {
        Printf("\r\nStarter Kit EEPROM initialized to defaults.\n");
    }

    //---------------------------------------------------------------------
    // Schedule background tasks
    //---------------------------------------------------------------------

    // Alive LED blinker
    SiiSchedTimerSet(500, SkAppBlinkAlive, 0, true, SI_SCHED_PRIORITY_NORMAL);
    // Power Control and Initialization Task
    SiiSchedTimerSet(5, SkAppDevicePower, 0, false, SI_SCHED_PRIORITY_NORMAL);
    // SiiMon communication handler
    SiiSchedTimerSet(COMM_POLLING_DELAY, SkAppExtMonitorHandler, 0, true, SI_SCHED_PRIORITY_HIGH);
    // UI tasks
    SiiSchedTimerSet(UI_POLLING_DELAY, SkAppUiTask, 0, true, SI_SCHED_PRIORITY_NORMAL);

    //---------------------------------------------------------------------
    // Yield to OS scheduler
    //---------------------------------------------------------------------

    SiiSchedMainLoop();
    return( 0 );
}


#endif
