//***************************************************************************
//!file     si_timer.c
//!brief    Platform timer system, ARM (Stellaris) version.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#include "si_timer.h"
#include "si_common.h"
#include "si_platform.h"
#include "si_arm.h"

volatile unsigned long g_ulTickCounter = 0;

//------------------------------------------------------------------------------
// Array of timer values
//------------------------------------------------------------------------------

static volatile uint16_t gTimerCounters[ TIMER_COUNT ];

static volatile uint16_t gTimerElapsed;
static volatile uint16_t gElapsedTick;
static volatile uint16_t gTimerElapsedGranularity;

static volatile uint16_t gTimerElapsed1;
static volatile uint16_t gElapsedTick1;
static volatile uint16_t gTimerElapsedGranularity1;

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************

void SiiPlatformTimerSysTickIntHandler (void)
{
    int             i;
 g_ulTickCounter++;
// Printf("\r\n g_ulTickCounter=%ld.\n",g_ulTickCounter);
    // Decrement all active timers in array

    for ( i = 0; i < TIMER_COUNT; i++ )
    {
        if ( gTimerCounters[ i ] > 0 )
        {
            gTimerCounters[ i ]--;
        }
    }
    gElapsedTick++;
    if ( gElapsedTick == gTimerElapsedGranularity )
    {
        gTimerElapsed++;
        gElapsedTick = 0;
    }
    gElapsedTick1++;
    if ( gElapsedTick1 == gTimerElapsedGranularity1 )
    {
        gTimerElapsed1++;
        gElapsedTick1 = 0;
    }

//    if ( !ulInt )   // Re-enable interrupts if needed
 //   {
 //       ROM_IntMasterEnable();
  //  }
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerSysTicksGet
// Description: Returns the current number of system ticks since we started
// Parameters:
// Returns:
//------------------------------------------------------------------------------

clock_time_t SiiPlatformTimerSysTicksGet (void)
{
    if(g_ulTickCounter >0xfffff000)
        g_ulTickCounter = 0;
//	Printf("\r\n Get system g_ulTickCounter=%ld",g_ulTickCounter);
    return((clock_time_t)g_ulTickCounter);
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerSet
// Description:
//------------------------------------------------------------------------------

void SiiPlatformTimerSet ( uint8_t index, uint16_t m_sec )
{
   // ROM_SysTickIntDisable();
    switch ( index )
    {
    case TIMER_0:
    case TIMER_1:
    case TIMER_2:
    case TIMER_3:
    case TIMER_4:
        gTimerCounters[ index ] = m_sec;
        break;

    case ELAPSED_TIMER:
        gTimerElapsedGranularity = m_sec;
        gTimerElapsed = 0;
        gElapsedTick = 0;
        break;
    case ELAPSED_TIMER1:
        gTimerElapsedGranularity1 = m_sec;
        gTimerElapsed1 = 0;
        gElapsedTick1 = 0;
        break;
    }
//    ROM_SysTickIntEnable();
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerExpired
// Description: Returns > 0 if specified timer has expired.
//------------------------------------------------------------------------------

uint8_t SiiPlatformTimerExpired ( uint8_t timer )
{
    if ( timer < TIMER_COUNT )
    {
        return( gTimerCounters[ timer ] == 0);
    }

    return( 0 );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerElapsed
// Description: Returns current timer tick.  Rollover depends on the
//              granularity specified in the SetTimer() call.
//------------------------------------------------------------------------------

uint16_t SiiPlatformTimerElapsed ( uint8_t index )
{
    uint16_t elapsedTime;

  //  ROM_SysTickIntDisable();
    if ( index == ELAPSED_TIMER )
        elapsedTime = gTimerElapsed;
    else
        elapsedTime = gTimerElapsed1;

  //  ROM_SysTickIntEnable();
    return( elapsedTime );
}

//------------------------------------------------------------------------------
// Function:    SiiPlatformTimerWait
// Description: Waits for the specified number of milliseconds, using timer 0.
//------------------------------------------------------------------------------

void SiiPlatformTimerWait ( uint16_t m_sec )
{

    SiiPlatformTimerSet( 0, m_sec );

    while ( !SiiPlatformTimerExpired( 0 ) )
    {
        ;  //empty wait loop
    }
//	usleep(1000*m_sec);

}


void SiiPlatformTimerInit (void)
{
    int i;
    Printf("\r\nSiiPlatformTimerInit.\n");

    //initializer timer counters in array
    g_ulTickCounter = 0;
    for ( i = 0; i < TIMER_COUNT; i++ )
    {
        gTimerCounters[ i ] = 0;
    }
    gTimerElapsed  = 0;
    gTimerElapsed1 = 0;
    gElapsedTick   = 0;
    gElapsedTick1  = 0;
    gTimerElapsedGranularity   = 0;
    gTimerElapsedGranularity1  = 0;

  //  rMhlCurentTime = 0;
 //    rMhlCurentTime.u4Seconds = 0;

  //    rMhlOldTime = 0;
 //    rMhlOldTime.u4Seconds = 0;
}

