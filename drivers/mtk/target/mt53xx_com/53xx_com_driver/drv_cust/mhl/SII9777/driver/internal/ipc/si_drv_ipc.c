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
 * @file si_drv_ipc.c
 *
 * @brief Host Interface handler
 *
 *****************************************************************************/
//#define SII_DEBUG 3

/***** #include statements ***************************************************/
#include "si_system_api.h"
#include "si_drv_cra_api.h"
#include "si_drv_ipc_api.h"
#include "si_drv_ipc_packets.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(si_drv_ipc);

/***** local macro definitions ***********************************************/

#define PAR_SIZE_MAX   SII_DRV_IPC_PACKET_MESSAGE_SIZE

/***** local type definitions ************************************************/

typedef struct
{
    SiiDrvIpcType_t       type;
    SiiDrvIpcOpCode_t     opCode;
    SiiDrvIpcTagCode_t    tagCode;
    uint16_t              parSize;
    uint16_t              parPntr;
    uint8_t               parData[SII_DRV_IPC_PACKET_MESSAGE_SIZE];
} Mes_t;
 
/***** local prototypes ******************************************************/

static void sMessageClear( Mes_t* pMes );
static void sTypeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcType_t type );
static SiiDrvIpcType_t sTypeGet( SiiDrvIpcPacketMessage_t* pMessage );
static void sOpCodeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcOpCode_t opCode );
static SiiDrvIpcOpCode_t sOpCodeGet( SiiDrvIpcPacketMessage_t* pMessage );
static void sTagCodeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcTagCode_t tagCode );
static SiiDrvIpcTagCode_t sTagCodeGet( SiiDrvIpcPacketMessage_t* pMessage );
static uint16_t sParSizeGet( SiiDrvIpcPacketMessage_t* pMessage );
static void sParDataSet( SiiDrvIpcPacketMessage_t* pMessage, uint8_t* pData, uint16_t parSize );
static void sParDataGet( SiiDrvIpcPacketMessage_t* pMessage, uint8_t* pData );
static void sCheckSumUpdate( SiiDrvIpcPacketMessage_t* pMessage );
static bool_t sMessageValidIs( SiiDrvIpcPacketMessage_t* pMessage );

/***** local data objects ****************************************************/

/***** call-back functions ***************************************************/

void SiiIpcPacketCallBack(SiiDrvIpcPacketEvent_t eventFlags)
{
    if( eventFlags & SII_DRV_IPC_PACKET__MESSAGE_RECEIVED )
    {
        SiiDrvIpcPacketMessage_t packMes;

        SiiDrvIpcPacketMessageReceive(&packMes);

        if( sMessageValidIs(&packMes) )
        {
            SiiIpcCallBack(SII_DRV_IPC_EVENT__REQUEST);
        }
    }
}

/***** public functions ******************************************************/

void SiiDrvIpcCreate( void )
{
    SiiDrvIpcPacketCreate();
}

void SiiDrvIpcDelete( void )
{
    SiiDrvIpcPacketDelete();
}

void SiiDrvIpcHandler(SiiDrvIpcIsrFlags_t flags)
{
   SiiDrvIpcPacketHandler(flags);
}

SiiDrvIpcMesInst_t SiiDrvIpcMessageCreate( void )
{
    /* Allocate memory for object */
    Mes_t* pMes = (Mes_t*)SiiSysMallocCreate(sizeof(Mes_t));
    SII_ASSERT(pMes);
    
    sMessageClear(pMes);
    return (SiiDrvIpcMesInst_t)pMes;
}

void SiiDrvIpcMessageDelete( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    SiiSysMallocDelete(pMes);
}

void SiiDrvIpcMessageSend( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*                   pMes = (Mes_t SII_XDATA*)iMessage;
    SiiDrvIpcPacketMessage_t packMes;

    sTypeSet(&packMes, pMes->type);
    sOpCodeSet(&packMes, pMes->opCode);
    sTagCodeSet(&packMes, pMes->tagCode);
    pMes->parSize = pMes->parPntr;
    sParDataSet(&packMes, pMes->parData, pMes->parSize);
    sCheckSumUpdate(&packMes);
    SiiDrvIpcPacketMessageSend(&packMes);
}

void SiiDrvIpcMessageReceive( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*                   pMes = (Mes_t SII_XDATA*)iMessage;
    SiiDrvIpcPacketMessage_t packMes;

    SiiDrvIpcPacketMessageReceive(&packMes);
    pMes->type    = sTypeGet(&packMes);
    pMes->opCode  = sOpCodeGet(&packMes);
    pMes->tagCode = sTagCodeGet(&packMes);
    pMes->parSize = sParSizeGet(&packMes);
    pMes->parPntr = 0;
    sParDataGet(&packMes, pMes->parData);
}

void SiiDrvIpcMessageClear( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    sMessageClear(pMes);
}

void SiiDrvIpcTypeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcType_t type )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->type = type;
}

SiiDrvIpcType_t SiiDrvIpcTypeGet( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    return pMes->type;
}

void SiiDrvIpcOpCodeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcOpCode_t opCode )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->opCode = opCode;
}

SiiDrvIpcOpCode_t SiiDrvIpcOpCodeGet( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    return pMes->opCode;
}

void SiiDrvIpcTagCodeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcTagCode_t tagCode )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->tagCode = tagCode;
}

SiiDrvIpcTagCode_t SiiDrvIpcTagCodeGet( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    return pMes->tagCode;
}

void SiiDrvIpcBoolPush( SiiDrvIpcMesInst_t iMessage, bool_t par )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->parData[pMes->parPntr++] = (uint8_t)par;
    SII_ASSERT(PAR_SIZE_MAX >= pMes->parPntr);
}

bool_t SiiDrvIpcBoolPop( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*  pMes = (Mes_t SII_XDATA*)iMessage;
    uint8_t par  = 0;

    par = (pMes->parData[pMes->parPntr++] << 0);
    SII_ASSERT(pMes->parSize >= pMes->parPntr);
    return par;
}

void SiiDrvIpcBytePush( SiiDrvIpcMesInst_t iMessage, uint8_t par )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->parData[pMes->parPntr++] = (uint8_t)(par >> 0);
    SII_ASSERT(PAR_SIZE_MAX >= pMes->parPntr);
}

uint8_t SiiDrvIpcBytePop( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*  pMes = (Mes_t SII_XDATA*)iMessage;
    uint8_t par  = 0;

    par = (pMes->parData[pMes->parPntr++] << 0);
    SII_ASSERT(pMes->parSize >= pMes->parPntr);
    return par;
}

void SiiDrvIpcWordPush( SiiDrvIpcMesInst_t iMessage, uint16_t par )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->parData[pMes->parPntr++] = (uint8_t)(par >> 0);
    pMes->parData[pMes->parPntr++] = (uint8_t)(par >> 8);
    SII_ASSERT(PAR_SIZE_MAX >= pMes->parPntr);
}

uint16_t SiiDrvIpcWordPop( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*   pMes = (Mes_t SII_XDATA*)iMessage;
    uint16_t par  = 0;

    par |= (pMes->parData[pMes->parPntr++] << 0);
    par |= (pMes->parData[pMes->parPntr++] << 8);
    SII_ASSERT(pMes->parSize >= pMes->parPntr);
    return par;
}

void SiiDrvIpcLongPush( SiiDrvIpcMesInst_t iMessage, uint32_t par )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    pMes->parData[pMes->parPntr++] = (uint8_t)(par >>  0);
    pMes->parData[pMes->parPntr++] = (uint8_t)(par >>  8);
    pMes->parData[pMes->parPntr++] = (uint8_t)(par >> 16);
    pMes->parData[pMes->parPntr++] = (uint8_t)(par >> 24);
    SII_ASSERT(PAR_SIZE_MAX >= pMes->parPntr);
}

uint32_t SiiDrvIpcLongPop( SiiDrvIpcMesInst_t iMessage )
{
    Mes_t*   pMes = (Mes_t SII_XDATA*)iMessage;
    uint32_t par  = 0;

    par |= (pMes->parData[pMes->parPntr++] <<  0);
    par |= (pMes->parData[pMes->parPntr++] <<  8);
    par |= (pMes->parData[pMes->parPntr++] << 16);
    par |= (pMes->parData[pMes->parPntr++] << 24);
    SII_ASSERT(pMes->parSize >= pMes->parPntr);
    return par;
}

void SiiDrvIpcArrayPush( SiiDrvIpcMesInst_t iMessage, const uint8_t* pPar, uint16_t size )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    SII_MEMCPY(&pMes->parData[pMes->parPntr], pPar, size);
    pMes->parPntr += size;
    SII_ASSERT(PAR_SIZE_MAX >= pMes->parPntr);
}

void SiiDrvIpcArrayPop( SiiDrvIpcMesInst_t iMessage, uint8_t* pPar, uint16_t size )
{
    Mes_t* pMes = (Mes_t SII_XDATA*)iMessage;

    SII_MEMCPY(pPar, &pMes->parData[pMes->parPntr], size);
    pMes->parPntr += size;
    SII_ASSERT(pMes->parSize >= pMes->parPntr);
}

/***** local functions *******************************************************/

static void sMessageClear( Mes_t* pMes )
{
    pMes->type     = SII_DRV_IPC_TYPE__NONE;
    pMes->opCode   = 0;
    pMes->tagCode  = 0;
    pMes->parSize  = 0;
    pMes->parPntr  = 0;
    SII_MEMSET(pMes->parData, 0, SII_DRV_IPC_PACKET_MESSAGE_SIZE);
}

static void sTypeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcType_t type )
{
    uint8_t val = 0;

    switch( type )
    {
        case SII_DRV_IPC_TYPE__NONE     : val = 0; break;
        case SII_DRV_IPC_TYPE__REQUEST  : val = 1; break;
        case SII_DRV_IPC_TYPE__RESPONSE : val = 2; break;
        case SII_DRV_IPC_TYPE__NOTIFY   : val = 3; break;
        default : break;
    }
    pMessage->payLoad[1] = (pMessage->payLoad[1]&0x0F) | (val<<4);
}

static SiiDrvIpcType_t sTypeGet( SiiDrvIpcPacketMessage_t* pMessage )
{
    SiiDrvIpcType_t type = SII_DRV_IPC_TYPE__NONE;

    switch( pMessage->payLoad[1] >> 4 )
    {
        case 0x1 : type = SII_DRV_IPC_TYPE__REQUEST;  break;
        case 0x2 : type = SII_DRV_IPC_TYPE__RESPONSE; break;
        case 0x3 : type = SII_DRV_IPC_TYPE__NOTIFY;   break;
        default : break;
    }
    return type;
}

static void sOpCodeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcOpCode_t opCode )
{
    pMessage->payLoad[0] = (uint8_t)(opCode & 0x00FF);
    pMessage->payLoad[1] = (pMessage->payLoad[1]&0xF0) | (uint8_t)((opCode>>8) & 0x000F);
}

static SiiDrvIpcOpCode_t sOpCodeGet( SiiDrvIpcPacketMessage_t* pMessage )
{
    return (SiiDrvIpcOpCode_t)((((uint16_t)(pMessage->payLoad[1]&0x0F))<<8) | pMessage->payLoad[0]);
}

static void sTagCodeSet( SiiDrvIpcPacketMessage_t* pMessage, SiiDrvIpcTagCode_t tagCode )
{
    pMessage->payLoad[4] = (uint8_t)(tagCode);
}

static SiiDrvIpcTagCode_t sTagCodeGet( SiiDrvIpcPacketMessage_t* pMessage )
{
    return (SiiDrvIpcTagCode_t)pMessage->payLoad[4];
}

static uint16_t sParSizeGet( SiiDrvIpcPacketMessage_t* pMessage )
{
    return (uint16_t)((((uint16_t)(pMessage->payLoad[3]&0x3))<<8) | pMessage->payLoad[2]);
}

static void sParDataSet( SiiDrvIpcPacketMessage_t* pMessage, uint8_t* pData, uint16_t parSize )
{
    pMessage->payLoad[2] = (uint8_t)(parSize & 0x00FF);
    pMessage->payLoad[3] = (uint8_t)((parSize>>8) & 0x0003);
    SII_MEMCPY(&pMessage->payLoad[6], pData, parSize);
}

static void sParDataGet( SiiDrvIpcPacketMessage_t* pMessage, uint8_t* pData )
{
    SII_MEMCPY(pData, &pMessage->payLoad[6], pMessage->size-6);
}

static void sCheckSumUpdate( SiiDrvIpcPacketMessage_t* pMessage )
{
    uint16_t    size  = 6 + sParSizeGet(pMessage);
    uint8_t*    pData = pMessage->payLoad;
    uint8_t     cs    = 0;

    pMessage->size       = size;
    pMessage->payLoad[5] = 0;
    while( size-- )
    {
        cs += *pData;
        pData++;
    }
    pMessage->payLoad[5] = (uint8_t)(0-((int16_t)cs));
}

static bool_t sMessageValidIs( SiiDrvIpcPacketMessage_t* pMessage )
{
    uint16_t    size  = pMessage->size;
    uint8_t*    pData = pMessage->payLoad;
    uint8_t     cs    = 0;

    while( size-- )
    {
        cs += *pData;
        pData++;
    }
    return (cs) ? (false) : (true);
}

/***** end of file ***********************************************************/
