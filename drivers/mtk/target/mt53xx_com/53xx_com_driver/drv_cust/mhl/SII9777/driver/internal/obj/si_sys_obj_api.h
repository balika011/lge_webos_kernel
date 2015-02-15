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
 * @file si_sys_obj_api.h
 *
 * @brief object base class
 *
 *****************************************************************************/

#ifndef SI_SYS_OBJ_API_H
#define SI_SYS_OBJ_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public macro definitions **********************************************/

//-------------------------------------------------------------------------------------------------
//! @brief      Converts external instantiation reference to pointer of instantiation object as
//!             defined locally in module. Creates an assertion if user would provide a reference
//!             to a different class of instantiation.
//!
//! @param[in]  inst  External reference to instantiation.
//!
//! @return     Pointer to instantiation object.
//-------------------------------------------------------------------------------------------------
#if(SII_ENV_BUILD_ASSERT)
  #define SII_SYS_OBJ_CHECK(listInst, objInst)    ( (SiiSysObjCheck(listInst, SII_INST2OBJ(objInst))) ? (SII_INST2OBJ(objInst)) : ((void xdata*)SII_ASSERT(0)))
#else
  #define SII_SYS_OBJ_CHECK(listInst, objInst)    (SII_INST2OBJ(objInst))
#endif

#define SII_SYS_OBJ_PARENT_INST_GET(childInst)    (SiiSysObjParentInstGet(SII_INST2OBJ(childInst)))

/***** public type definitions ***********************************************/

typedef uint16_t   SiiSysObjSize_t;
typedef SiiInst_t  SiiSysObjListInst_t;

/***** public functions ******************************************************/

void* SiiSysObjSingletonCreate( const char* pClassStr, SiiInst_t parentInst, SiiSysObjSize_t size );
void SiiSysObjSingletonDelete( void* pObj );

SiiSysObjListInst_t SiiSysObjListCreate( const char* pClassStr, SiiSysObjSize_t size );
void SiiSysObjListDelete( SiiSysObjListInst_t listInst );

void* SiiSysObjInstanceCreate( SiiSysObjListInst_t listInst, SiiInst_t parentInst, const char* pInstStr );
void SiiSysObjInstanceDelete( void* pObj );

void* SiiSysObjFirstGet( SiiSysObjListInst_t listInst );
bool_t SiiSysObjCheck( SiiSysObjListInst_t listInst, void* pObj );
SiiInst_t SiiSysObjParentInstGet( void* pObj );

const char* SiiSysObjListNameGet( void* pObj );
const char* SiiSysObjNameGet( void* pObj );
void* SiiSysObjNextGet( void* pObj );
void SiiSysObjMove( void* pObjDes, void* pObjSrc );

#endif /* SI_SYS_OBJ_API_H */

/***** end of file ***********************************************************/
