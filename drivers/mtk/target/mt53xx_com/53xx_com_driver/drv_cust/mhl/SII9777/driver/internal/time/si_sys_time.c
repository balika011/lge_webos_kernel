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
 * @file si_sys_time.c
 *
 * @brief Time library
 *
 *****************************************************************************/
//#define SII_DEBUG 3

/***** #include statements ***************************************************/

#include "si_datatypes.h"
#include "si_sys_time_api.h"
#include "sii9777_platform_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_time);

/***** local macro definitions ***********************************************/

#define MILLI_TO_MAX          (((SiiSysTimeMilli_t)~0)>>1)  /* Maximum milli out must be set to less than half the range of SiiSysTimeMilli_t */

/***** local type definitions ************************************************/

/***** local prototypes ******************************************************/

static void sTimeOutMilliSet( SiiSysTimeMilli_t* pMilliTO, SiiSysTimeMilli_t timeOut );
static bool_t sTimeOutMilliIs( const SiiSysTimeMilli_t* pMilliTO );

/***** local data objects ****************************************************/

/***** public functions ******************************************************/

SiiSysTimeMilli_t SiiSysTimeMilliDiffGet( SiiSysTimeMilli_t t1, SiiSysTimeMilli_t t2 )
{
    if( t2 < t1 )
        return((SiiSysTimeMilli_t)~0) - t1 + t2 + 1;
    else
        return t2 - t1;
}

void SiiSysTimeOutMilliSet( SiiSysTimeMilli_t* pMilliTO, SiiSysTimeMilli_t timeOut )
{
    sTimeOutMilliSet(pMilliTO, timeOut);
}

bool_t SiiSysTimeOutMilliIs( const SiiSysTimeMilli_t* pMilliTO )
{
    return sTimeOutMilliIs(pMilliTO);
}

void SiiSysTimeMilliDelay( SiiSysTimeMilli_t millDelay )
{
    SiiSysTimeMilli_t milliTO;

    sTimeOutMilliSet(&milliTO, millDelay);

    while(!sTimeOutMilliIs(&milliTO));
}

/***** local functions *******************************************************/

static void sTimeOutMilliSet( SiiSysTimeMilli_t* pMilliTO, SiiSysTimeMilli_t timeOut )
{
    SII_ASSERT(MILLI_TO_MAX>timeOut);
    *pMilliTO = (SiiSysTimeMilli_t)SiiPlatformTimeMilliGet() + timeOut;
}

static bool_t sTimeOutMilliIs( const SiiSysTimeMilli_t* pMilliTO )
{
    SiiSysTimeMilli_t milliNew = (SiiSysTimeMilli_t)SiiPlatformTimeMilliGet();
    SiiSysTimeMilli_t milliDif = (*pMilliTO > milliNew) ? (*pMilliTO - milliNew) : (milliNew - *pMilliTO);
    if( MILLI_TO_MAX < milliDif )
        return (*pMilliTO >  milliNew) ? (true) : (false);
    else
        return (*pMilliTO <= milliNew) ? (true) : (false);
}


/***** end of file ***********************************************************/
