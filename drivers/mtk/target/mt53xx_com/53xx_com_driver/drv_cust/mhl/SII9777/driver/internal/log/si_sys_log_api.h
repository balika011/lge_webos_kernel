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
 * @file si_sys_log_api.h
 *
 * @brief logging library
 *
 *****************************************************************************/

#ifndef SI_SYS_LOG_API_H
#define SI_SYS_LOG_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"
#include "si_sys_obj_api.h"

/***** public macro definitions **********************************************/

#define SII_LOG_ENABLE(lev)                    ( lev <= SII_DEBUG )

#ifndef SII_DEBUG
  #define SII_LOG1A(func, obj, str)
  #define SII_LOG2A(func, obj, str)
  #define SII_LOG3A(func, obj, str)
  #define SII_LOG1B(str)
  #define SII_LOG2B(str)
  #define SII_LOG3B(str)
#else
  #if( 0 == SII_DEBUG )
  #define SII_LOG1A(func, obj, str)
  #define SII_LOG2A(func, obj, str)
  #define SII_LOG3A(func, obj, str)
  #define SII_LOG1B(str)
  #define SII_LOG2B(str)
  #define SII_LOG3B(str)
  #elif( 1 == SII_DEBUG )
  #define SII_LOG1A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG2A(func, obj, str)
  #define SII_LOG3A(func, obj, str)
  #define SII_LOG1B(str)                       { SiiSysLogPrintf str; }
  #define SII_LOG2B(str)
  #define SII_LOG3B(str)
  #elif( 2 == SII_DEBUG )
  #define SII_LOG1A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG2A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG3A(func, obj, str)
  #define SII_LOG1B(str)                       { SiiSysLogPrintf str; }
  #define SII_LOG2B(str)                       { SiiSysLogPrintf str; }
  #define SII_LOG3B(str)
  #else
  #define SII_LOG1A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG2A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG3A(func, obj, str)            { SiiSysLogTimeStamp(sSiiModuleNameStr, (char*)func, (void*)(obj)); SiiSysLogPrintf str; }
  #define SII_LOG1B(str)                       { SiiSysLogPrintf str; }
  #define SII_LOG2B(str)                       { SiiSysLogPrintf str; }
  #define SII_LOG3B(str)                       { SiiSysLogPrintf str; }
  #endif
#endif
 	
/***** public type definitions ***********************************************/

/***** public functions ******************************************************/

void SiiSysLogTimeStamp(const char* pClassStr, const char* pFuncStr, void* pObj);
void SiiSysLogPrintf( char* pFrm, ...);

#endif /* SI_SYS_LOG_API_H */

/***** end of file ***********************************************************/
