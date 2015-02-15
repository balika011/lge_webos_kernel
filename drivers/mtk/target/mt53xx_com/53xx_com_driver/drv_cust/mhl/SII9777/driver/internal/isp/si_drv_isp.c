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
 * @file isp.c
 *
 * @brief ISP APIs
 *
 *****************************************************************************/

/***** #include statements ***************************************************/

#include "si_system_api.h"
#include "si_drv_cra_api.h"

/***** local macro definitions ***********************************************/

#define ISP_OPCODE__WREN                    0x06
#define ISP_OPCODE__CHIP_ERASE              0x60
#define ISP_OPCODE__PROG                    0x02
#define ISP_OPCODE__DATA_RD                 0x03
#define ISP_OPCODE__STATUS                  0x05

#define REG_I2C_CONFIG_AUTOINC_SET(val)     SiiDrvCraBitsSet8(0, 0x000C, 0x01, val)
#define REG_ISP_CONFIG_OPCODECTRL_SET(val)  SiiDrvCraBitsSet8(0, 0x7030, 0x0C, val);
#define REG_ISP_CONFIG_SET(val)             SiiDrvCraWrite8(0, 0x7030, val)
#define REG_ISP_OPCODE_SET(val)             SiiDrvCraWrite8(0, 0x7080, val)
#define REG_ISP_ADDR_SET(val)               SiiDrvCraWrite24(0, 0x7084, val)
#define REG_ISP_BLOCK_DATA_SET(p, size)     SiiDrvCraBlockWrite8(0, 0x7088, p, size)
#define REG_ISP_STATUS_GET()                SiiDrvCraRead8(0, 0x708a)

/***** local type definitions ************************************************/

/***** local prototypes ******************************************************/

static void sWaitSpim( void );
static void sIspEnable( void );
static void sWriteEnable( void );
static void sWriteBurstEnable( bool_t bBurst );
static void sAutoIncEnable( bool_t bAutoInc );
static void sWriteAddr( uint32_t addr );
static void sWriteOpcode( uint8_t val );

/***** local data objects ****************************************************/

/***** public functions ******************************************************/

void SiiDrvIspChipErase()
{
    sIspEnable();
    sWriteEnable();
    REG_ISP_OPCODE_SET(ISP_OPCODE__CHIP_ERASE);
    sWaitSpim();
}

void SiiDrvIspBurstWrite(uint32_t addr, const uint8_t* pData, uint16_t len)
{
    sIspEnable();
    sWriteBurstEnable(1);
    sWriteEnable();
    sWriteOpcode(ISP_OPCODE__PROG);
    sWriteAddr(addr);
    sAutoIncEnable(0);

    REG_ISP_BLOCK_DATA_SET(pData, len);

    sAutoIncEnable(1);
    sWriteBurstEnable(0);
}

bool_t SiiDrvIspOperationDone(void)
{
    REG_ISP_OPCODE_SET(ISP_OPCODE__STATUS);
    sWaitSpim();
    return ( (REG_ISP_STATUS_GET() & 0x01) == 0x00 );
}

/***** local functions *******************************************************/

static void sWaitSpim( void )
{
}

static void sIspEnable( void )
{
    REG_ISP_CONFIG_SET(0);
    REG_ISP_CONFIG_SET(1);
}

static void sWriteEnable( void )
{
    REG_ISP_OPCODE_SET(ISP_OPCODE__WREN);
    sWaitSpim();
}

static void sWriteBurstEnable( bool_t bBurst )
{
    REG_ISP_CONFIG_OPCODECTRL_SET((bBurst) ? (1) : (0));
}

static void sAutoIncEnable( bool_t bAutoInc )
{
    REG_I2C_CONFIG_AUTOINC_SET((bAutoInc) ? (1) : (0));
}

static void sWriteAddr(uint32_t addr)
{
    REG_ISP_ADDR_SET(addr);
    sWaitSpim();
}

static void sWriteOpcode(uint8_t val)
{
    REG_ISP_OPCODE_SET(val);
    sWaitSpim();
}

/***** end of file ***********************************************************/
