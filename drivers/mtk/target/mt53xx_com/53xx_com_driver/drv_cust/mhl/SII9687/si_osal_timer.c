//***************************************************************************
//!file     si_osal_timer.c
//!brief    Starter Kit Operating System Adaptation Layer Timer functions.
//
// No part of this work may be reproduced, modified, distributed, 
// transmitted, transcribed, or translated into any language or computer 
// format, in any form or by any means without written permission of 
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2012, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/


#include "si_timer.h"
#include "si_osal_timer.h"
#include "si_common.h"
#include "si_platform.h"


//extern inline clock_time_t SkTimeDiffMs ( clock_time_t t1, clock_time_t t2);   // for actual code.

inline clock_time_t SkTimeDiffMs ( clock_time_t t1, clock_time_t t2)
{
    if (t2 >= t1)
    {
        return (t2 - t1);
    }

    // overflow situation (t2 < t1)
 //   return( t1 + ((CLOCK_TIME_MAX - t2) + 1));
    return( t1 + ((0xffffffff - t2) + 1));
}



//-------------------------------------------------------------------------------------------------
//! @brief      Returns number of ticks since startup.
// Note:        The count rolls over at ~49.7 days.
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
clock_time_t SiiOsTimerTotalElapsed ( void )
{

  return( SiiPlatformTimerSysTicksGet());
 // return 0;
 
}

//-------------------------------------------------------------------------------------------------
//! @brief      Start an OS timer.
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
void SiiOsTimerSet ( SiiTimer_t *timerInfo, clock_time_t delay  )
{
    timerInfo->msDelay  = delay;
    timerInfo->msStart  = SiiOsTimerTotalElapsed();
    timerInfo->enabled  = true;
    timerInfo->oneShot  = false;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Start an OS timer.
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
void SiiOsTimerSetOneShot ( SiiTimer_t *timerInfo, clock_time_t delay  )
{
    timerInfo->msDelay  = delay;
    timerInfo->msStart  = SiiOsTimerTotalElapsed();
    timerInfo->enabled  = true;
    timerInfo->oneShot  = true;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Check passed OS timer for expiration.
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
bool_t SiiOsTimerExpired ( SiiTimer_t timerInfo )
{
    if ( !timerInfo.enabled )
    {
   // Printf("\r\n timerInfo.enabled  ",timerInfo.enabled );
        return( false );
    }
   // Printf("\r\n timerInfo.msStart=%ld ",timerInfo.msStart );
   // Printf("\r\n timerInfo.msDelay=%ld ",timerInfo.msDelay );
    if (SkTimeDiffMs( timerInfo.msStart, SiiOsTimerTotalElapsed()) >= timerInfo.msDelay)
    {
        if ( timerInfo.oneShot )
        {
            timerInfo.enabled = false;
        }
        return( true );
    }
  //  Printf("\r\n timerInfo.enabled2  ",timerInfo.enabled );
    return( false );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Cancel the specified timer. Canceling will result in calls to SiiOsTimerExpired()
//!             returning false for this timer.
//!
//! @retval     - true: successful.
//! @retval     - false: failure
//-------------------------------------------------------------------------------------------------
void SiiOsTimerCancel ( SiiTimer_t timerInfo )
{

    timerInfo.enabled = false;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return difference between current time and the passed timer's start time.
//!
//! @retval     Difference time in ms.
//-------------------------------------------------------------------------------------------------
clock_time_t SiiOsTimerElapsed ( SiiTimer_t timerInfo )
{
    /*
clock_time_t t1;
t1=SkTimeDiffMs( timerInfo.msStart, SiiOsTimerTotalElapsed());
Printf("\r\nSiiOsTimerElapsed timerInfo.msStart=%ld  t1=%ld \n",timerInfo.msStart,t1 );
return(t1);*/
  return( SkTimeDiffMs( timerInfo.msStart, SiiOsTimerTotalElapsed()));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Synchronous wait for specified number of milliseconds.
//-------------------------------------------------------------------------------------------------
void SiiOsTimerWait ( clock_time_t delay )
{
    SiiTimer_t timerInfo;

    SiiOsTimerSet( &timerInfo, delay );
    while ( !SiiOsTimerExpired( timerInfo ))
    {

    }
}

//------------------------------------------------------------------------------
// Function:    SiiTimerExpired
// Description: Wrapper for equivalent board component function
// Parameters:
// Returns:
//------------------------------------------------------------------------------

uint8_t SiiTimerExpired ( uint8_t timer )
{

    return( SiiPlatformTimerExpired( timer ));
 // return( 0);

}

//------------------------------------------------------------------------------
// Function:    SiiTimerElapsed
// Description: Wrapper for equivalent board component function
// Parameters:
// Returns:
//------------------------------------------------------------------------------

long SiiTimerElapsed( uint8_t index )
{
    
    return( SiiPlatformTimerElapsed( index ));
   // return(  0);
}

//------------------------------------------------------------------------------
// Function:    SiiTimerSet
// Description: Wrapper for equivalent board component function
// Parameters:
// Returns:
//------------------------------------------------------------------------------

void SiiTimerSet ( uint8_t index, uint16_t mSec )
{
    SiiPlatformTimerSet( index, mSec );
}
