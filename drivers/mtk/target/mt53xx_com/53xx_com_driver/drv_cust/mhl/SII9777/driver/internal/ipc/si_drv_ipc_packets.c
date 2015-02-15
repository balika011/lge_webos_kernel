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
 * @file si_drv_ipc_packet.c
 *
 * @brief Host Interface handler
 *
 *****************************************************************************/
#define SII_DEBUG 3

/***** #include statements ***************************************************/

#include "si_system_api.h"
#include "si_drv_cra_api.h"
#include "si_drv_ipc_api.h"
#include "si_drv_ipc_packets.h"

/***** Register Module name **************************************************/

SII_MODULE_NAME_SET(drv_ipc_packet);

/***** local macro definitions ***********************************************/

#define MESSAGE_RX_SIZE                     (SII_DRV_IPC_PACKET_MESSAGE_SIZE)
#define MESSAGE_TX_SIZE                     (SII_DRV_IPC_PACKET_MESSAGE_SIZE)

#define SII_IPC_PACKET_PAYLOAD_ADDR_SIZE    (4)
#define BUFFER_TOTAL_SIZE                   (64)
#define BUFFER_PAYLOAD_SIZE                 (BUFFER_TOTAL_SIZE-SII_IPC_PACKET_PAYLOAD_ADDR_SIZE)

#define PACKET_LAST_SEGMENT_FLAG            0x80

#define INTERRUPT_FLAG_PACKET_RDY           (1<<0)
#define INTERRUPT_FLAG_ACK                  (1<<1)
#define INTERRUPT_FLAG_NACK                 (1<<2)


#ifdef SII_ENV_BUILD_C51
  #define REG_INT_SET(flag)                 (SiiDrvCraWrite8(0, 0x7060, flag))

  #define PACKET_RX_ADDR_BEGIN              0x7280
  #define PACKET_TX_ADDR_BEGIN              0x7220

#else
  #define REG_INT_SET(flag)                 (SiiDrvCraWrite8(0, 0x7070, flag))

  #define PACKET_RX_ADDR_BEGIN              0x7220
  #define PACKET_TX_ADDR_BEGIN              0x7280
#endif

#if 0
#define REG_INT_SET(flag)				  (SiiDrvCraWrite8(0, 0x7060, flag))
	
#define PACKET_RX_ADDR_BEGIN			  0x7280
#define PACKET_TX_ADDR_BEGIN			  0x7220
#endif
/***** local type definitions ************************************************/

typedef struct
{      
    uint8_t                  rxSeqNum;
    uint8_t                  rxFrgNum;
    uint16_t                 rxChrCnt;
    uint16_t                 rxMsgSiz;
    SiiDrvIpcPacketMessage_t rxMessage;

    uint8_t                  txSeqNum;
    uint8_t                  txFrgNum;
    uint16_t                 txChrCnt1;
    uint16_t                 txChrCnt2;
    uint16_t                 txMsgSiz;
    uint8_t                  txRetry;
    SiiDrvIpcPacketMessage_t txMessage;
} Obj_t;

/***** local prototypes ******************************************************/

static void sMessageClear( SiiDrvIpcPacketMessage_t* pMessage );
static uint8_t sCheckSumGet( uint8_t *pBuf, uint16_t len );
static bool_t sCheckSumFailed( uint8_t *pBuf, uint16_t len );
static void sSendMessageHandler( void );
static void sReceiveMessageHandler( void );

/***** local data objects ****************************************************/

static Obj_t* spObj = NULL;

/***** call-back functions ***************************************************/

/***** public functions ******************************************************/

void SiiDrvIpcPacketCreate(void)
{
    /* Allocate memory for object */
    SII_ASSERT(!spObj);
    spObj = (Obj_t*)SiiSysObjSingletonCreate("IcpPacket", SII_INST_NULL, sizeof(Obj_t));
    SII_ASSERT(spObj);

    SII_LOG1A("SiiDrvIpcPacketCreate", 0 , (""));	

    /*--------------------------------*/
    /* Initialize internal states     */
    /*--------------------------------*/
    /* Initialize the object */
    spObj->rxSeqNum  = 0;
    spObj->rxFrgNum  = 0;
    spObj->rxChrCnt  = 0;
    spObj->rxMsgSiz  = 0;
    sMessageClear(&spObj->rxMessage);

    spObj->txSeqNum  = 0;
    spObj->txFrgNum  = 0;
    spObj->txChrCnt1 = 0;
    spObj->txChrCnt2 = 0;
    spObj->txMsgSiz  = 0;
    spObj->txRetry   = 0;
    sMessageClear(&spObj->txMessage);

    /*--------------------------------*/
    /* Static hardware configuration  */
    /*--------------------------------*/ 
    {                
        uint16_t len;
        uint16_t addr;
    
        /* Clear Rx-Message box */
        len  = BUFFER_TOTAL_SIZE;
        addr = PACKET_RX_ADDR_BEGIN;
        while( len-- )
        {
            SiiDrvCraWrite8(0, addr++, 0x00);
        }

        /* Clear Tx-Message box */
        len  = BUFFER_TOTAL_SIZE;
        addr = PACKET_TX_ADDR_BEGIN;
        while( len-- )
        {
            SiiDrvCraWrite8(0, addr++, 0x00);
        }
    }

    /* Enable host interrupts */
    SiiDrvCraWrite8(0, 0x7061, 0x07);
}

void SiiDrvIpcPacketDelete(void)
{
    SII_LOG1A("SiiDrvIpcPacketDelete", 0 , (""));	

    SiiSysObjSingletonDelete(spObj);
    spObj = NULL;
}

void SiiDrvIpcPacketHandler(SiiDrvIpcIsrFlags_t flags)
{
    /* Collect interrupt Flags */
    SII_LOG1A("HostCallBack", 0, ("Process interrupt flags : 0x%02X\n", (uint16_t)flags));	
    if( flags & SII_DRV_IPC_ISR_FLAG__NACK )
    {
        spObj->txRetry++;
        if( 5 <= spObj->txRetry )
        {
            SiiIpcPacketCallBack(SII_DRV_IPC_PACKET__MESSAGE_ERROR);
            spObj->txMessage.size = 0;
        }
        else
        {
            sSendMessageHandler(); /* send remaining packets */
        }
    }

    if( flags & SII_DRV_IPC_ISR_FLAG__ACK )
    {
        spObj->txChrCnt1 = spObj->txChrCnt2;

        if( spObj->txMessage.size > spObj->txChrCnt1 )
        {
            spObj->txFrgNum++;           /* Increase fragment number */
            spObj->txRetry = 0;          /* Reset retry counter */
            sSendMessageHandler();       /* send remaining packets */
        }
        else
        {
            spObj->txMessage.size = 0;   /* Message has been sent */
            spObj->txChrCnt1 = 0;        /* Message has been sent */
            spObj->txSeqNum++;           /* Increase sequence number for next message */
            SiiIpcPacketCallBack(SII_DRV_IPC_PACKET__MESSAGE_SENT);
        }
    }

    if( flags & SII_DRV_IPC_ISR_FLAG__PACKET_RDY )
    {
        sReceiveMessageHandler();
    }
}

uint16_t SiiDrvIpcPacketMessageSend(const SiiDrvIpcPacketMessage_t *ipMessage)
{
    SII_ASSERT(spObj);

    SII_LOG1A("Send", 0 , ("size=%d,", ipMessage->size));	
    {
        uint16_t i = 0;

        for( i=0; i<ipMessage->size; i++ )
        {
            SII_LOG1B((" %02X ", (uint16_t)ipMessage->payLoad[i]));	
        }    
    }
    
    if( spObj->txMessage.size )
        return 0;

    SII_ASSERT(MESSAGE_TX_SIZE >= ipMessage->size);

    spObj->txMessage = *ipMessage;
    spObj->txFrgNum = 0;          /* Reset Fragment numer */
    sSendMessageHandler();        /* Start sending first packet */
    return ipMessage->size;
}

uint16_t SiiDrvIpcPacketMessageReceive(SiiDrvIpcPacketMessage_t *opMessage)
{
    uint16_t size = spObj->rxMessage.size;

    SII_ASSERT(spObj);

    SII_LOG1A("Receive", 0 , ("size=%d,", size));	
    if( size )
    {
        *opMessage = spObj->rxMessage;

        {
            uint16_t i = 0;
    
            for( i=0; i<size; i++ )
            {
                SII_LOG1B((" %02X ", (uint16_t)spObj->rxMessage.payLoad[i]));	
            }    
        }
    }
    return size;
}

/***** local functions *******************************************************/

static void sMessageClear( SiiDrvIpcPacketMessage_t* pMessage )
{
    pMessage->size = 0;
    SII_MEMSET(pMessage->payLoad, 0, SII_DRV_IPC_PACKET_MESSAGE_SIZE);
}

static uint8_t sCheckSumGet( uint8_t *pBuf, uint16_t len )
{
    uint8_t checksum = 0;

    while( len-- )
    {
        checksum += *pBuf;
        pBuf++;
    }
    return (uint8_t)((int16_t)0-checksum);
}

static bool_t sCheckSumFailed( uint8_t *pBuf, uint16_t len )
{
    uint8_t checksum = 0;

    while( len-- )
    {
        checksum += *pBuf;
        pBuf++;
    }
    return (checksum) ? (true) : (false);
}

static void sSendMessageHandler( void )
{
    uint8_t  buffer[BUFFER_TOTAL_SIZE];
    uint8_t  *pbuf    = buffer;
    uint16_t check    = spObj->txMessage.size - spObj->txChrCnt1;
    uint8_t  chrCnt   = 0;

    spObj->txChrCnt2 = spObj->txChrCnt1;

    if( BUFFER_PAYLOAD_SIZE < check )
    {
        chrCnt = BUFFER_PAYLOAD_SIZE;
    }
    else
    {
        chrCnt = (uint8_t)check;
        /* Flag fragment number as last packet of message */
        spObj->txFrgNum |= 0x80;
    }

    *(pbuf++) = spObj->txSeqNum;
    *(pbuf++) = spObj->txFrgNum;
    *(pbuf++) = chrCnt;
    *(pbuf++) = 0;
    SII_MEMCPY(pbuf, &spObj->txMessage.payLoad[spObj->txChrCnt2], chrCnt);
    spObj->txChrCnt2 += chrCnt;
    chrCnt += 4;

    /* Write Checksum */
    buffer[3] = sCheckSumGet(buffer, chrCnt);

    /* Write packet to shared memory */
    SiiDrvCraBlockWrite8(0, PACKET_TX_ADDR_BEGIN, buffer, chrCnt);

    REG_INT_SET(INTERRUPT_FLAG_PACKET_RDY); // Set ready flag for receiver

    SII_LOG2A("sSendMessageHandler", 0 , ("seq=%02X, frg=%02X, len=%02X, cs=%02X\n", (uint16_t)spObj->txSeqNum, (uint16_t)spObj->txFrgNum, (uint16_t)chrCnt, (uint16_t)buffer[3]));	
}

static void sReceiveMessageHandler( void )
{
    uint8_t  buffer[BUFFER_TOTAL_SIZE];
    bool_t   bNoError  = true;
    uint8_t  seqNum    = 0;
    uint8_t  frgNum    = 0;
    uint8_t  datlen    = 0;
    bool_t   bLastFrag = false;

    SiiDrvCraBlockRead8(0, PACKET_RX_ADDR_BEGIN, buffer, 4);
    seqNum    = buffer[0];
    frgNum    = buffer[1] & 0x7F;
    datlen    = buffer[2];
    bLastFrag = (buffer[1] & 0x80) ? (true) : (false);

    SII_LOG2A("sReceiveMessageHandler", 0 , ("seq=%02X, frg=%02X, len=%02X, cs=%02X\n", (uint16_t)buffer[0], (uint16_t)buffer[1], (uint16_t)buffer[2], (uint16_t)buffer[3]));	

    /* Check data length byte */
    if( bNoError )
    {
        if( BUFFER_PAYLOAD_SIZE < datlen )
        {
            SII_LOG2B(("Too many bytes in package!\n"));	
            bNoError = false;
        }
    }

    /* Read packet and calculate checksum */
    if( bNoError )
    {
        SiiDrvCraBlockRead8(0, PACKET_RX_ADDR_BEGIN+4, &buffer[4], datlen);
    }

    /* Check checksum value */
    if( bNoError )
    {
        if( sCheckSumFailed(buffer, datlen+4) )
        {
            REG_INT_SET(INTERRUPT_FLAG_NACK); /* checksum error */
            SII_LOG2B(("Wrong checksum!\n"));
            bNoError = false;
        }
    }

    /* Check fragment number */
    if( bNoError )
    {
        if( 0 == spObj->rxFrgNum )
        {
            /* Reset message sequence number */
            spObj->rxSeqNum = seqNum;
        }
        else if( frgNum != spObj->rxFrgNum )
        {
            REG_INT_SET(INTERRUPT_FLAG_NACK); /* Message is too big */
            SII_LOG2B(("wrong fragment number!\n"));	
            bNoError = false;
        }
    }

    /* Check message sequence number */
    if( bNoError )
    {
        if( spObj->rxSeqNum != seqNum )
        {
            REG_INT_SET(INTERRUPT_FLAG_NACK); /* Wrong message sequence number received */
            SII_LOG2B(("Packet with wrong sequence number!\n"));
            bNoError = false;
        }
    }

    /* Process Packet */
    if( bNoError )
    {
        SII_MEMCPY(&spObj->rxMessage.payLoad[spObj->rxChrCnt], &buffer[4], datlen);
        spObj->rxChrCnt += datlen;
        spObj->rxFrgNum++;
        spObj->rxMessage.size = 0;

        if( bLastFrag )
        {
            spObj->rxMessage.size = spObj->rxChrCnt;
            spObj->rxChrCnt = 0;
            spObj->rxFrgNum = 0;

            /* Call back parent before acknowledging last packet */
            SiiIpcPacketCallBack(SII_DRV_IPC_PACKET__MESSAGE_RECEIVED); 
        }

        /* Indicate to sender that packet has been accepted */
        REG_INT_SET(INTERRUPT_FLAG_ACK);
    }
    else
    {
        /* Reset packet state state machine */
        spObj->rxChrCnt = 0;
        spObj->rxFrgNum = 0;
        REG_INT_SET(INTERRUPT_FLAG_NACK);
    }
}

/***** end of file ***********************************************************/
