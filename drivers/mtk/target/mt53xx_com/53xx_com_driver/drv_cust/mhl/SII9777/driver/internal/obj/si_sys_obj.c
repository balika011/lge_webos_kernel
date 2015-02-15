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
 * @file si_sys_obj.c
 *
 * @brief Object creater
 *
 *****************************************************************************/
//#define SII_DEBUG 3

/***** #include statements ***************************************************/

//#include <stdio.h>
//#include <stdarg.h>

#include "si_datatypes.h"
#include "si_sys_obj_api.h"
#include "si_sys_malloc_api.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(lib_obj);

/***** local macro definitions ***********************************************/

#define LIST_OBJ2INST(pListObj)    ( (SiiSysObjListInst_t)SII_OBJ2INST(pListObj) )
#define LIST_INST2OBJ(listInst)    ( (LstObj_t*)SII_INST2OBJ(listInst) )

#define OBJECT2HEADER(pVoid)       ( (HdrObj_t*)((char*)pVoid-sizeof(HdrObj_t)) )
#define HEADER2OBJECT(pHdrObj)     ( (void*)(((char*)pHdrObj)+sizeof(HdrObj_t)) )


/***** local type definitions ************************************************/

typedef struct HdrObj_s
{
    const char*          pInstStr;
    SiiSysObjListInst_t  listInst;
    SiiInst_t            parentInst;
    struct HdrObj_s*     pHdrObjNext;
} HdrObj_t;

typedef struct LstObj_s
{
    const char*          pClassStr;
    HdrObj_t*            pHdrObjLast;
    struct LstObj_s*     pLstObjNext;
    SiiSysObjSize_t      objSize;
} LstObj_t;

/***** local prototypes ******************************************************/

static void sAddList( LstObj_t* pLstObjNew );
static void sRemoveList( LstObj_t* pLstObjDel );
static HdrObj_t* sPrevHeaderGet( HdrObj_t* pHdrObj );
static void sPostInsertHeader( HdrObj_t* pHdrObjDes, HdrObj_t* pHdrObjSrc );
static void sInsertHeader( HdrObj_t* pHdrObj );
static void sRemoveHeader( HdrObj_t* pHdrObj );

/***** local data objects ****************************************************/

static LstObj_t* spLstObjFirst = NULL;

/***** public functions ******************************************************/

void* SiiSysObjSingletonCreate( const char* pClassStr, SiiInst_t parentInst, SiiSysObjSize_t size )
{
    LstObj_t* pLstObj = NULL;
    HdrObj_t* pHdrObj = NULL;
    void*     pVoid   = NULL;

    /* Create new list */
    pLstObj = (LstObj_t*)SiiSysMallocCreate(sizeof(LstObj_t));
    if( pLstObj )
    {
        HdrObj_t* pHdrObj = NULL;

        pLstObj->pClassStr   = pClassStr;
        pLstObj->pHdrObjLast = NULL;
        pLstObj->pLstObjNext = NULL;
        pLstObj->objSize     = size;

        /* Create link between each list of instantiations */
        sAddList(pLstObj);

        /* Request memory for instance header */
        pHdrObj = (HdrObj_t*)SiiSysMallocCreate(sizeof(HdrObj_t)+size);
        if( pHdrObj )
        {
            /* Configure header */
            pHdrObj->pInstStr    = "Singleton";
            pHdrObj->listInst    = LIST_OBJ2INST(pLstObj);
            pHdrObj->parentInst  = parentInst;
            pHdrObj->pHdrObjNext = NULL;

            /* Insert instance to linked list */
            sInsertHeader(pHdrObj);

            /* Make sure that object is linked-in */
            SII_ASSERT(pHdrObj->pHdrObjNext);

            pVoid = HEADER2OBJECT(pHdrObj);
        }
    }
    return pVoid;
}

void SiiSysObjSingletonDelete( void* pObj )
{
    HdrObj_t* pHdrObj;
    LstObj_t* pLstObj;

    SII_ASSERT(pObj);
    pHdrObj = OBJECT2HEADER(pObj);
    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);

    /* Remove instance from linked list */
    SII_ASSERT(pHdrObj->pHdrObjNext);
    sRemoveHeader(pHdrObj);
    SII_ASSERT(!pHdrObj->pHdrObjNext);

    /* Delete instance memory */
    SiiSysMallocDelete(pHdrObj);

    /* Remove list */
    sRemoveList(pLstObj);

    /* Delete list memory */
    SiiSysMallocDelete(pLstObj);
}

SiiSysObjListInst_t SiiSysObjListCreate( const char* pClassStr, SiiSysObjSize_t size )
{
    LstObj_t* pLstObj;

    /* Create new list */
    pLstObj = (LstObj_t*)SiiSysMallocCreate(sizeof(LstObj_t));
    SII_ASSERT(pLstObj);

    pLstObj->pClassStr   = pClassStr;
    pLstObj->pHdrObjLast = NULL;
    pLstObj->pLstObjNext = NULL;
    pLstObj->objSize     = size;

    /* Create link between each list of instantiations */
    sAddList(pLstObj);

    return LIST_OBJ2INST(pLstObj);
}

void SiiSysObjListDelete( SiiSysObjListInst_t listInst )
{
    LstObj_t* pLstObj = LIST_INST2OBJ(listInst);

    SII_ASSERT(pLstObj);

    /* Remove all existing linked instances */
    while( pLstObj->pHdrObjLast )
    {
        /* Remove instance from linked list */
        sRemoveHeader(pLstObj->pHdrObjLast);

        /* Delete instance memory */
        SiiSysMallocDelete(pLstObj->pHdrObjLast);
    }

    /* Remove list */
    sRemoveList(pLstObj);

    /* Delete list memory */
    SiiSysMallocDelete(pLstObj);
}

void* SiiSysObjInstanceCreate( SiiSysObjListInst_t listInst, SiiInst_t parentInst, const char* pInstStr )
{
    LstObj_t* pLstObj = NULL;
    HdrObj_t* pHdrObj = NULL;
    void*     pVoid   = NULL;

    SII_ASSERT(listInst);
    pLstObj = LIST_INST2OBJ(listInst);

    /* Request memory for instance header */
    pHdrObj = (HdrObj_t*)SiiSysMallocCreate(sizeof(HdrObj_t)+pLstObj->objSize);
    if( pHdrObj )
    {
        /* Configure header */
        pHdrObj->pInstStr    = pInstStr;
        pHdrObj->listInst    = listInst;
        pHdrObj->parentInst  = parentInst;
        pHdrObj->pHdrObjNext = NULL;

        /* Insert instance to linked list */
        sInsertHeader(pHdrObj);

        /* Make sure that object is linked-in */
        SII_ASSERT(pHdrObj->pHdrObjNext);

        pVoid = HEADER2OBJECT(pHdrObj);
    }
    return pVoid;
}

void SiiSysObjInstanceDelete( void* pObj )
{
    HdrObj_t* pHdrObj;
    LstObj_t* pLstObj;

    SII_ASSERT(pObj);
    pHdrObj = OBJECT2HEADER(pObj);
    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);

    /* Remove instance from linked list */
    SII_ASSERT(pHdrObj->pHdrObjNext);
    sRemoveHeader(pHdrObj);
    SII_ASSERT(!pHdrObj->pHdrObjNext);

    /* Delete instance memory */
    SiiSysMallocDelete(pHdrObj);
}

void* SiiSysObjFirstGet( SiiSysObjListInst_t listInst )
{
    LstObj_t* pLstObj = LIST_INST2OBJ(listInst);

    return (pLstObj->pHdrObjLast) ? (HEADER2OBJECT(pLstObj->pHdrObjLast->pHdrObjNext)) : (NULL);
}

#if(SII_ENV_BUILD_ASSERT)
bool_t SiiSysObjCheck( SiiSysObjListInst_t listInst, void* pObj )
{
    LstObj_t* pLstObj = NULL;

    SII_ASSERT(listInst);
    pLstObj = LIST_INST2OBJ(listInst);
    if( pLstObj )
    {
        HdrObj_t* pHdrObj = OBJECT2HEADER(pObj);

        /* Check if object is registered to the same list */
        if( listInst == pHdrObj->listInst )
            return true;
    }
    return false;
}
#endif

SiiInst_t SiiSysObjParentInstGet( void* pObj )
{
    return OBJECT2HEADER(pObj)->parentInst;
}

#if 0
const char* SiiSysObjListNameGet( void* pObj )
{
    HdrObj_t* pHdrObj = NULL;
    LstObj_t* pLstObj = NULL;

    SII_ASSERT(pObj);
    pHdrObj = OBJECT2HEADER(pObj);
    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);

    return pLstObj->pClassStr;
}
#endif

const char* SiiSysObjNameGet( void* pObj )
{
    HdrObj_t* pHdrObj = NULL;

    SII_ASSERT(pObj);
    pHdrObj = OBJECT2HEADER(pObj);
    return pHdrObj->pInstStr;
}

void* SiiSysObjNextGet( void* pObj )
{
    HdrObj_t* pHdrObj = NULL;
    LstObj_t* pLstObj = NULL;

    SII_ASSERT(pObj);
    pHdrObj = OBJECT2HEADER(pObj);
    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);
    SII_ASSERT(pLstObj);
    if( pHdrObj != pLstObj->pHdrObjLast )
        return HEADER2OBJECT(pHdrObj->pHdrObjNext);

    /* Reached end of list */
    return NULL;
}

void SiiSysObjMove( void* pObjDes, void* pObjSrc )
{
    HdrObj_t* pHdrObjSrc = NULL;
    LstObj_t* pLstObj    = NULL;

    SII_ASSERT(pObjSrc);

    /* No mvove is needed if pSrc and pDes pointing to the same object */
    if( pObjDes == pObjSrc )
        return;

    pHdrObjSrc = OBJECT2HEADER(pObjSrc);
    pLstObj    = LIST_INST2OBJ(pHdrObjSrc->listInst);

    /* Make sure that source object is currently linked in */
    SII_ASSERT(pHdrObjSrc->pHdrObjNext);

    /* Temporarily remove source object from linked list */
    sRemoveHeader(pHdrObjSrc);

    /* With a valid destination object provided the source object will be replaced to directly behind the destination object.      */
    /* However if null pointer is provided for destination object, then source object will be moved to the first position of list. */
    if( pObjDes )
    {
        HdrObj_t* pHdrObjDes = OBJECT2HEADER(pObjDes);

        /* Make sure that both instantiations belong to the same list (type) */
        SII_ASSERT(pHdrObjDes->listInst == pHdrObjSrc->listInst);

        /* Make sure that destination object is a linked in object */
        SII_ASSERT(pHdrObjDes->pHdrObjNext);

        /* Insert in linked list */
        sPostInsertHeader(pHdrObjDes, pHdrObjSrc);

        /* Update last pointer if inserted at end of list */
        if( pHdrObjDes == pLstObj->pHdrObjLast )
        {
            pLstObj->pHdrObjLast = pHdrObjSrc;
        }
    }
    else
    {
        if( pLstObj->pHdrObjLast )
        {
            /* Insert as first object in list */
            sPostInsertHeader(pLstObj->pHdrObjLast, pHdrObjSrc);
        }
        else
        {
            /* First inserted object should point to itself to ensure a circular linked list */
            pHdrObjSrc->pHdrObjNext = pHdrObjSrc;
            pLstObj->pHdrObjLast   = pHdrObjSrc;
        }
    }
}

/***** local functions *******************************************************/

static void sAddList( LstObj_t* pLstObjNew )
{
    if( spLstObjFirst )
    {
        LstObj_t* pLstObj = spLstObjFirst;

        while( pLstObj->pLstObjNext )
        {
            pLstObj = pLstObj->pLstObjNext;
        }
        pLstObj->pLstObjNext = pLstObjNew;
    }
    else
    {
        spLstObjFirst = pLstObjNew;
    }
}

static void sRemoveList( LstObj_t* pLstObjDel )
{
    /* Make sure that at least one list exists */
    SII_ASSERT(spLstObjFirst);
    if( spLstObjFirst == pLstObjDel )
    {
        spLstObjFirst = spLstObjFirst->pLstObjNext;
    }
    else
    {
        LstObj_t* pLstObj = spLstObjFirst;

        /* search linked list */
        while( pLstObj->pLstObjNext )
        {
            if( pLstObj->pLstObjNext == pLstObjDel )
                break;

            pLstObj = pLstObj->pLstObjNext;

        }
        /* Generate assertion if provided obj-list is not found */
        SII_ASSERT(pLstObj->pLstObjNext);

        /* remove and relink linked list */
        pLstObj->pLstObjNext = pLstObjDel->pLstObjNext;
    }
}

static HdrObj_t* sPrevHeaderGet( HdrObj_t* pHdrObj )
{
    HdrObj_t* pHdrObjTmp = pHdrObj;

    while( pHdrObj != pHdrObjTmp->pHdrObjNext )
        pHdrObjTmp = pHdrObjTmp->pHdrObjNext;

    return pHdrObjTmp;
}

static void sPostInsertHeader( HdrObj_t* pHdrObjDes, HdrObj_t* pHdrObjSrc )
{
    pHdrObjSrc->pHdrObjNext = pHdrObjDes->pHdrObjNext;
    pHdrObjDes->pHdrObjNext = pHdrObjSrc;
}

static void sInsertHeader( HdrObj_t* pHdrObj )
{
    LstObj_t* pLstObj = NULL;

    SII_ASSERT(pHdrObj);

    /* Make sure that new header is un-linked */
    SII_ASSERT(!pHdrObj->pHdrObjNext);

    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);
    if( pLstObj->pHdrObjLast )
    {
        /* Insert at the end of list of headers */
        sPostInsertHeader(pLstObj->pHdrObjLast, pHdrObj);
    }
    else
    {
        /* First inserted object should point to itself to ensure a circular linked list */
        pHdrObj->pHdrObjNext = pHdrObj;
    }
    pLstObj->pHdrObjLast = pHdrObj;
}

static void sRemoveHeader( HdrObj_t* pHdrObj )
{
    LstObj_t* pLstObj = NULL;

    SII_ASSERT(pHdrObj);

    /* Make sure that header is currently linked in */
    SII_ASSERT(pHdrObj->pHdrObjNext);

    pLstObj = LIST_INST2OBJ(pHdrObj->listInst);

    /* check if this is the last object to be removed */
    if( pHdrObj == pHdrObj->pHdrObjNext )
    {
        /* Clear reference to linked list */
        pLstObj->pHdrObjLast = NULL;
    }
    else
    {
        HdrObj_t* pHdrObjPrv = sPrevHeaderGet(pHdrObj);

        /* remove instance out of linked list */
        pHdrObjPrv->pHdrObjNext = pHdrObj->pHdrObjNext;

        /* Change last instance reference in case last instance is removed */
        if( pLstObj->pHdrObjLast == pHdrObj )
            pLstObj->pHdrObjLast = pHdrObjPrv;
    }

    /* Clear next reference to indicate that instance has been removed from list */
    pHdrObj->pHdrObjNext = NULL;
}

/***** end of file ***********************************************************/
