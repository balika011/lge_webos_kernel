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
 * @file si_drv_ipc_packet_api.h
 *
 * @brief IPC message protocol
 *
 *****************************************************************************/
#ifndef SI_DRV_IPC_PACKET_H
#define SI_DRV_IPC_PACKET_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"
#include "si_drv_ipc_api.h"

/***** public macro definitions **********************************************/

#define SII_DRV_IPC_PACKET__MESSAGE_RECEIVED     0x00000001 //!< Message has been received
#define SII_DRV_IPC_PACKET__MESSAGE_SENT         0x00000002 //!< Message has been sent
#define SII_DRV_IPC_PACKET__MESSAGE_ERROR        0x00000004 //!< Not able to send Message

#define SII_DRV_IPC_PACKET_MESSAGE_SIZE          268

/***** public type definitions ***********************************************/

typedef uint32_t SiiDrvIpcPacketEvent_t;

typedef struct
{
    uint16_t size;
    uint8_t  payLoad[SII_DRV_IPC_PACKET_MESSAGE_SIZE];
} SiiDrvIpcPacketMessage_t;

/***** call-back functions ***************************************************/

void SiiIpcPacketCallBack(SiiDrvIpcPacketEvent_t eventFlags);

/***** public functions ******************************************************/

void SiiDrvIpcPacketCreate(void);
void SiiDrvIpcPacketDelete(void);
void SiiDrvIpcPacketHandler(SiiDrvIpcIsrFlags_t flags);
uint16_t SiiDrvIpcPacketMessageSend(const SiiDrvIpcPacketMessage_t *ipMessage);
uint16_t SiiDrvIpcPacketMessageReceive(SiiDrvIpcPacketMessage_t *opMessage);
void SiiDrvIpcPacketIsr(uint8_t intFlags);

#endif // SI_DRV_IPC_PACKET_H

/***** end of file ***********************************************************/
