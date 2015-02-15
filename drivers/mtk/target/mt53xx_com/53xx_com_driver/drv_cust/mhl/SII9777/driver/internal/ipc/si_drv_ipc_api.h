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
 * @file si_drv_ipc_api.h
 *
 * @brief IPC message protocol
 *
 *****************************************************************************/
#ifndef SI_DRV_IPC_API_H
#define SI_DRV_IPC_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public macro definitions **********************************************/

#define SII_DRV_IPC_EVENT__REQUEST               0x01

#define SII_DRV_IPC_ISR_FLAG__PACKET_RDY         0x01
#define SII_DRV_IPC_ISR_FLAG__ACK                0x02
#define SII_DRV_IPC_ISR_FLAG__NACK               0x04

/***** public type definitions ***********************************************/

typedef uint8_t    SiiDrvIpcIsrFlags_t;
typedef uint32_t   SiiDrvIpcEvent_t;
typedef SiiInst_t  SiiDrvIpcMesInst_t;
typedef uint16_t   SiiDrvIpcOpCode_t;
typedef uint8_t    SiiDrvIpcTagCode_t;

typedef enum
{
    SII_DRV_IPC_TYPE__NONE,
    SII_DRV_IPC_TYPE__REQUEST,
    SII_DRV_IPC_TYPE__RESPONSE,
    SII_DRV_IPC_TYPE__NOTIFY
} SiiDrvIpcType_t;

/***** call-back functions ***************************************************/

void SiiIpcCallBack(SiiDrvIpcEvent_t eventFlags);

/***** public functions ******************************************************/

void SiiDrvIpcCreate( void );
void SiiDrvIpcDelete( void );
void SiiDrvIpcHandler(SiiDrvIpcIsrFlags_t flags);

SiiDrvIpcMesInst_t SiiDrvIpcMessageCreate( void );
void SiiDrvIpcMessageDelete( SiiDrvIpcMesInst_t ipcInst );
void SiiDrvIpcMessageSend( SiiDrvIpcMesInst_t iMessage );
void SiiDrvIpcMessageReceive( SiiDrvIpcMesInst_t iMessage );
void SiiDrvIpcMessageClear( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcTypeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcType_t type );
SiiDrvIpcType_t SiiDrvIpcTypeGet( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcOpCodeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcOpCode_t opCode );
SiiDrvIpcOpCode_t SiiDrvIpcOpCodeGet( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcTagCodeSet( SiiDrvIpcMesInst_t iMessage, SiiDrvIpcTagCode_t tagCode );
SiiDrvIpcTagCode_t SiiDrvIpcTagCodeGet( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcBoolPush( SiiDrvIpcMesInst_t iMessage, bool_t par );
bool_t SiiDrvIpcBoolPop( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcBytePush( SiiDrvIpcMesInst_t iMessage, uint8_t par );
uint8_t SiiDrvIpcBytePop( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcWordPush( SiiDrvIpcMesInst_t iMessage, uint16_t par );
uint16_t SiiDrvIpcWordPop( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcLongPush( SiiDrvIpcMesInst_t iMessage, uint32_t par );
uint32_t SiiDrvIpcLongPop( SiiDrvIpcMesInst_t iMessage );

void SiiDrvIpcArrayPush( SiiDrvIpcMesInst_t iMessage, const uint8_t* pPar, uint16_t size );
void SiiDrvIpcArrayPop( SiiDrvIpcMesInst_t iMessage, uint8_t* pPar, uint16_t size );

#endif // SI_DRV_IPC_API_H

/***** end of file ***********************************************************/
