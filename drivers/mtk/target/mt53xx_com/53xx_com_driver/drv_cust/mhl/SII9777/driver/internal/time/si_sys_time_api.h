/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_sys_time_api.h
 *
 * @brief Time Library 
 *
 *****************************************************************************/

#ifndef SI_SYS_TIME_API_H
#define SI_SYS_TIME_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public macro definitions **********************************************/

/***** public type definitions ***********************************************/

typedef uint32_t SiiSysTimeMicro_t;
typedef uint32_t SiiSysTimeMilli_t;

/***** public functions ******************************************************/

//-------------------------------------------------------------------------------------------------
//! @brief      Calculates time difference between t1 and t2 and prevent result from roll-over 
//!             corruption (t1<t2).
//!
//! @param[in]  t1 - Number of milli seconds.
//! @param[in]  t2 - Number of milli seconds.
//!
//! @return     Number of milli seconds.
//-------------------------------------------------------------------------------------------------
SiiSysTimeMilli_t SiiSysTimeMilliDiffGet( SiiSysTimeMilli_t t1, SiiSysTimeMilli_t t2 );

//-------------------------------------------------------------------------------------------------
//! @brief      Configures a milli time-out object. This object can be used with 'SiiSysTimeOutMilliIs'
//!             to find out if 'MilliTO' object has been expired.
//!             example:
//!             {
//!                 SiiSysTimeMilli_t MilliTO;
//!
//!                 SiiSysTimeOutMilliSet(&MilliTO, 100);
//!                 while( !SiiSysTimeOutMilliIs(MilliTO) )
//!                 { .... }
//!             }
//!
//! @param[in]  pMilliTO - pointer to 'MilliTO' object.
//! @param[in]  timeOut  - Number of milli seconds.
//-------------------------------------------------------------------------------------------------
void             SiiSysTimeOutMilliSet( SiiSysTimeMilli_t* pMilliTO, SiiSysTimeMilli_t timeOut );

//-------------------------------------------------------------------------------------------------
//! @brief      Finds out if 'MillTO' object has been expired.
//!
//! @param[in]  pMilliTO - pointer to 'MilliTO' object.
//!
//! @return     true if 'MilliTO' object has been expired.
//-------------------------------------------------------------------------------------------------
bool_t           SiiSysTimeOutMilliIs( const SiiSysTimeMilli_t* pMilliTO );

//-------------------------------------------------------------------------------------------------
//! @brief      Blocks execution for x number of milli seconds.
//!
//! @param[in]  milliDelay - Number of milli seconds.
//-------------------------------------------------------------------------------------------------
void             SiiSysTimeMilliDelay( SiiSysTimeMilli_t milliDelay );

#endif /* SI_SYS_TIME_API_H */

/***** end of file ***********************************************************/
