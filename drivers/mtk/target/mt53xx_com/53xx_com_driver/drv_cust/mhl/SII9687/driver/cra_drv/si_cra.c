//***************************************************************************
//!file     si_cra.c
//!brief    Silicon Image Device register I/O support.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
//#include "si_memsegsupport.h"
#include "si_platform.h"
#include "si_common.h"
#include "si_cra.h"
#include "si_cra_internal.h"
#include "si_cra_cfg.h"
#include "si_i2c.h"
#include "si_cra_cfg_template.h"

#include "sif_if.h"


static prefuint_t   l_pageInstance[SII_CRA_DEVICE_PAGE_COUNT] = {0};
extern pageConfig_t g_addrDescriptor[SII_CRA_MAX_DEVICE_INSTANCES][SII_CRA_DEVICE_PAGE_COUNT];
extern SiiReg_t     g_siiRegPageBaseReassign [];
extern SiiReg_t     g_siiRegPageBaseRegs[SII_CRA_DEVICE_PAGE_COUNT];

CraInstanceData_t craInstance =
{
    0,                          // structVersion
    0,                          // instanceIndex
    SII_SUCCESS,                // lastResultCode
    0,                          // statusFlags
};

//-------------------------------------------------------------------------------------------------
//! @brief      Read a byte or bytes from the specified bus and i2c device.
//! @param[in]  busIndex
//! @param[in]  deviceId    - I2C device address, eight-bit format (addr + r/w bit)
//! @param[in]  regAddr     - Eight or sixteen bit starting register address
//! @param[in]  pBuffer     - destination buffer.
//! @param[in]  count       - number of registers (bytes) to read.
//! @return     SiiResultCodes_t status of operation (0 == success).
//-------------------------------------------------------------------------------------------------
#if 0
static SiiResultCodes_t CraReadBlockI2c ( prefuint_t busIndex, uint8_t deviceId, uint16_t regAddr, uint8_t *pBuffer, uint16_t count )
{
    SiiResultCodes_t    status;
    SiiI2cMsg_t         msgs[2];
    uint8_t             addrData[2];
    int                 addrLen;

    if ( busIndex != DEV_I2C_ADDR16 )
    {
        // Only one byte register address
        addrData[0] = (uint8_t)regAddr;
        addrLen = 1;
    }
    else
    {
        // Two byte, big endian register address
        addrData[0] = (uint8_t)(regAddr >> 8);
        addrData[1] = (uint8_t)(regAddr);
        addrLen = 2;
    }

    // Split into register address write and register data read messages.
    msgs[0].addr        = deviceId;
    msgs[0].cmdFlags    = SII_MI2C_WR;
    msgs[0].len         = addrLen;
    msgs[0].pBuf        = &addrData[0];
    msgs[1].addr        = deviceId;
    msgs[1].cmdFlags    = SII_MI2C_RD;
    msgs[1].len         = count;
    msgs[1].pBuf        = pBuffer;

    // Send the messages
    status = (SiiResultCodes_t)SiiPlatformI2cTransfer( busIndex, &msgs[0], sizeof( msgs ) / sizeof( SiiI2cMsg_t ));
    return( (SiiResultCodes_t)status );
}
//-------------------------------------------------------------------------------------------------
//! @brief      Write a byte or bytes to the specified bus and i2c device.
//! @param[in]  busIndex
//! @param[in]  deviceId    - I2C device address, eight-bit format (addr + r/w bit)
//! @param[in]  regAddr     - Eight or sixteen bit starting register address
//! @param[in]  pBuffer     - source data buffer.
//! @param[in]  count       - number of registers (bytes) to write (maximum 256).
//! @return     SiiResultCodes_t status of operation (0 == success).
//-------------------------------------------------------------------------------------------------
static SiiResultCodes_t CraWriteBlockI2c ( prefuint_t busIndex, uint8_t deviceId, uint16_t regAddr, const uint8_t *pBuffer, uint16_t count )
{
    SiiResultCodes_t    status;
    SiiI2cMsg_t         msg;
    uint8_t             srcData [ 2 + 256]; // Room for 256 register data bytes and one or two register address bytes
    uint16_t            msgLen;

    if ( busIndex != DEV_I2C_ADDR16 )
    {
        // Only one byte register address
        srcData[0] = (uint8_t)regAddr;
        msgLen = 1;
    }
    else
    {
        // Two byte, big endian register address
        srcData[0] = (uint8_t)(regAddr >> 8);
        srcData[1] = (uint8_t)(regAddr);
        msgLen = 2;
    }

    // Copy payload data to src buffer following the register address.
    memcpy( &srcData[msgLen], pBuffer, count );
    msgLen += count;

    // Create message
    msg.addr            = deviceId;
    msg.cmdFlags        = SII_MI2C_WR;
    msg.len             = msgLen;
    msg.pBuf            = &srcData[0];

    // Send the messages
    status = (SiiResultCodes_t)SiiPlatformI2cTransfer( busIndex, &msg, 1 );
    return( (SiiResultCodes_t)status );
}

#endif
//------------------------------------------------------------------------------
// Function:    SiiCraInitialize
// Description: Initialize the CRA page instance array and perform any register
//              page base address reassignments required.
// Parameters:  none
// Returns:     None
//------------------------------------------------------------------------------
bool_t SiiCraInitialize ( void )
{
    prefuint_t i, index;
    craInstance.lastResultCode = RESULT_CRA_SUCCESS;

    for (i = 0; i < SII_CRA_DEVICE_PAGE_COUNT; i++)
    {
        l_pageInstance[i] = 0;
    }

    // Perform any register page base address reassignments
    i = 0;
    while ( g_siiRegPageBaseReassign[ i] != 0xFFFF )
    {
        index = g_siiRegPageBaseReassign[ i] >> 8;
        if (( index < SII_CRA_DEVICE_PAGE_COUNT ) && ( g_siiRegPageBaseRegs[ index] != 0xFF))
        {
            // The page base registers allow reassignment of the
            // I2C device ID for almost all device register pages.
            SiiRegWrite( g_siiRegPageBaseRegs[ index], g_siiRegPageBaseReassign[ index] & 0x00FF );
        }
        else
        {
            craInstance.lastResultCode = SII_ERR_INVALID_PARAMETER;
            break;
        }
        i++;
    }

    return( craInstance.lastResultCode == RESULT_CRA_SUCCESS );
}

//------------------------------------------------------------------------------
// Function:    SiiCraGetLastResult
// Description: Returns the result of the last call to a CRA driver function.
// Parameters:  none.
// Returns:     Returns the result of the last call to a CRA driver function
//------------------------------------------------------------------------------
SiiResultCodes_t SiiCraGetLastResult ( void )
{
    return( craInstance.lastResultCode );
}

//------------------------------------------------------------------------------
// Function:    SiiRegInstanceSet
// Description: Sets the instance for subsequent register accesses.  The register
//              access functions use this value as an instance index of the multi-
//              dimensional virtual address lookup table.
// Parameters:  newInstance - new value for instance axis of virtual address table.
// Returns:     None
//------------------------------------------------------------------------------
bool_t SiiRegInstanceSet ( SiiReg_t virtualAddress, prefuint_t newInstance )
{
    prefuint_t va = virtualAddress >> 8;

    craInstance.lastResultCode = RESULT_CRA_SUCCESS;
    if (( va < SII_CRA_DEVICE_PAGE_COUNT) && ( newInstance < SII_CRA_MAX_DEVICE_INSTANCES ))
    {
        l_pageInstance[ va ] = newInstance;
        return( true );
    }

    craInstance.lastResultCode = SII_ERR_INVALID_PARAMETER;
    return( false );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Reads a block of data from sequential registers.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  pBuffer     - destination buffer.
//! @param[in]  count       - number of registers (bytes) to read.
//! @return     none
//! @note       This function relies on the auto-increment model used by
//! @note       Silicon Image devices.  Because of this, if a FIFO register
//! @note       is encountered before the end of the requested count, the
//! @note       data remaining from the count is read from the FIFO, NOT
//! @note       from subsequent registers.
//-------------------------------------------------------------------------------------------------
#if 0
void SiiRegReadBlock ( SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[ l_pageInstance[ virtualAddr]][ virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset, pBuffer, count );
            break;
        case DEV_I2C_OFFSET:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), pBuffer, count );
            break;
        default:
            craInstance.lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}
#else
void SiiRegReadBlock ( SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    #if 1//i2c9687     
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[ l_pageInstance[ virtualAddr]][ virtualAddr];    
    I2cReadBlock_9589((uint8_t)pPage->address, regOffset,pBuffer, count);
    #endif
}
#endif	

//-------------------------------------------------------------------------------------------------
//! @brief      Read a one byte register.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @return     eight bit register data.
//-------------------------------------------------------------------------------------------------
#if 0
uint8_t SiiRegRead ( SiiReg_t virtualAddr )
{
    uint8_t             value = 0;
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[ l_pageInstance[ virtualAddr]][ virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset, &value, 1 );
            break;
        case DEV_I2C_OFFSET:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraReadBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), &value, 1 );
            break;
        default:
            craInstance.lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }

    return( value );
}
#else
uint8_t SiiRegRead ( SiiReg_t virtualAddr )
{
  	#if 1//i2c9687         
     uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];
   // Printf("MHL_Read   RegisterPage = %02x RegisterPage = %02x",(uint8_t)pPage->address,regOffset );
  
   return( I2CRead_9589((uint8_t)pPage->address, regOffset));
  #else
  virtualAddr=0;
  return 0;
    #endif
}
#endif

//-------------------------------------------------------------------------------------------------
//! @brief      Writes a block of data to sequential registers.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  pBuffer     - source data buffer.
//! @param[in]  count       - number of registers (bytes) to write.
//! @return     none
//! @note       This function relies on the auto-increment model used by
//! @note       Silicon Image devices.  Because of this, if a FIFO register
//! @note       is encountered before the end of the requested count, the
//! @note       data remaining from the count is written to the FIFO, NOT
//! @note       to subsequent registers.
//-------------------------------------------------------------------------------------------------
#if 0
void SiiRegWriteBlock ( SiiReg_t virtualAddr, const uint8_t *pBuffer, uint16_t count )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset, pBuffer, count );
            break;
        case DEV_I2C_OFFSET:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c( DEV_I2C_0, (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), pBuffer, count );
            break;
        default:
            craInstance.lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}
#else
void SiiRegWriteBlock ( SiiReg_t virtualAddr, const uint8_t *pBuffer, uint16_t count )
{
    #if 1//i2c9687   
     uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];          
    I2cWriteBlock_9589( (unsigned char)pPage->address,(unsigned char)regOffset,(unsigned char *)pBuffer, (unsigned short)count );
    #endif
}
#endif


#define SII9587_I2C_TIMING 0x100


unsigned char I2CWrite_9589( unsigned char devId, unsigned char regAddr, unsigned char wrData )
{
  
   // uint8_t i;
   // for (i = 0; i<50; i++)
    {
        //if (SIF_Write(SII9287_I2C_TIMING, devId, regAddr, wrData, 1) != 0)
       if (SIF_Write(SII9587_I2C_TIMING, devId, regAddr, &wrData, 1) != 0)
        {
             return 1;
        }
        Printf("\r\n MHL_SK I2CWrite_9589 devId = %x  RegAdd=%x Value=%x",devId,regAddr,wrData);                      
    }
    Printf("\nMHL_Sk 9587 IIC WRITE FAIL\n");
    return 0;
}
unsigned char I2CRead_9589( unsigned char devId, unsigned char regAddr )
{
   // uint8_t i;
    uint8_t returnData = 0;
    //for (i=0; i<50; i++)
    {
        //if (SIF_Read(SII9287_I2C_TIMING, devId, regAddr, &returnData, 1) != 0)
       if (SIF_Read(SII9587_I2C_TIMING, devId, regAddr, &returnData, 1) > 0)
        {
            return returnData;
        }
       Printf("\r\n MHL_SK 9587 Read DevAdd= %x  RegAdd=%x Value=%x",devId,regAddr,returnData);
           
    }
    Printf("\r\n MHL_SK 9587 IIC READ FAIL\n");
    return( false );
}
unsigned char I2cReadBlock_9589( unsigned char devId, unsigned char regAddr, unsigned char *pRdData, unsigned short nBytes )
{
   // uint8_t i;

    //for (i=0; i<50; i++)
    {
        //if (SIF_Read(SII9287_I2C_TIMING, devId, regAddr, pRdData, nBytes) != 0)
        if (SIF_Read(SII9587_I2C_TIMING, devId, regAddr, pRdData, nBytes) > 0)
        {
            return true;
        }
       Printf("\r\n MHL_SK 9587 I2cReadBlock_9589= %x  RegAdd=%x",devId,regAddr);

    }
    Printf("\nSK 9587 IIC READ BLOCK FAIL\n");
    return false;
}
unsigned char I2cWriteBlock_9589 ( unsigned char devId, unsigned char regAddr,  unsigned char *pWrData, unsigned short nBytes)
{
   // uint8_t i;
    //for (i = 0; i<50; i++)
    {
        //if (SIF_Write(SII9287_I2C_TIMING, devId, regAddr, pWrData, nBytes) != 0)
        if (SIF_Write(SII9587_I2C_TIMING, devId, regAddr, pWrData, nBytes) != 0)
        {  
         
            return true;
        }
        Printf("\r\n MHL_SK I2CWrite_9589 devId = %x  RegAdd=%x",devId,regAddr);                      

    }
    //Printf("\nSk 9589 IIC WRITE FAIL\n");

    return false;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Write a one byte register.
//! @param[in]  virtualAddress  - Sixteen bit virtual register address, including device page.
//! @param[in]  value           - eight bit data to write to register.
//! @return     None
//-------------------------------------------------------------------------------------------------
#if 0
void SiiRegWrite ( SiiReg_t virtualAddr, uint8_t value )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];

    switch ( pPage->busType )
    {
        case DEV_I2C_0:
        case DEV_I2C_1:
        case DEV_I2C_2:
        case DEV_I2C_3:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c( pPage->busType, (uint8_t)pPage->address, regOffset, &value, 1 );
            break;
        case DEV_I2C_OFFSET:
        case DEV_I2C_1_OFFSET:
        case DEV_I2C_2_OFFSET:
        case DEV_I2C_3_OFFSET:
            craInstance.lastResultCode = (SiiDrvCraError_t)CraWriteBlockI2c( pPage->busType - DEV_I2C_OFFSET , (uint8_t)pPage->address, regOffset + (uint8_t)(pPage->address >> 8), &value, 1 );
            break;
        default:
            craInstance.lastResultCode = RESULT_CRA_INVALID_PARAMETER;
            break;
    }
}
#else
void SiiRegWrite ( SiiReg_t virtualAddr, uint8_t value )
{
    	#if 1//i2c9687         
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;

    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];
  
  //Printf("\r\n MHL_Write   RegisterPage = %02x RegisterPage = %02x",(uint8_t)pPage->address, regOffset );
       I2CWrite_9589((uint8_t)pPage->address, regOffset, value );
        #endif
}
#endif
//------------------------------------------------------------------------------
// Function:    SiiRegModify
// Description: Reads the register, performs an AND function on the data using
//              the mask parameter, and an OR function on the data using the
//              value ANDed with the mask. The result is then written to the
//              device register specified in the regAddr parameter.
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              mask    - Eight bit mask
//              value   - Eight bit data to be written, combined with mask.
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegModify ( SiiReg_t virtualAddr, uint8_t mask, uint8_t value)
{
    uint8_t aByte;

    aByte = SiiRegRead( virtualAddr );
    aByte &= (~mask);                       // first clear all bits in mask
    aByte |= (mask & value);                // then set bits from value
    SiiRegWrite( virtualAddr, aByte );
}

//------------------------------------------------------------------------------
// Function:    SiiRegBitsSet
// Description: Reads the register, sets the passed bits, and writes the
//              result back to the register.  All other bits are left untouched
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              bits   - bit data to be written
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegBitsSet ( SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits )
{
    uint8_t aByte;

    aByte = SiiRegRead( virtualAddr );
    aByte = (setBits) ? (aByte | bitMask) : (aByte & ~bitMask);
    SiiRegWrite( virtualAddr, aByte );}

//------------------------------------------------------------------------------
// Function:    SiiRegBitsSetNew
// Description: Reads the register, sets or clears the specified bits, and
//              writes the result back to the register ONLY if it would change
//              the current register contents.
// Parameters:  regAddr - Sixteen bit register address, including device page.
//              bits   - bit data to be written
//              setBits- true == set, false == clear
// Returns:     None
//------------------------------------------------------------------------------
void SiiRegBitsSetNew ( SiiReg_t virtualAddr, uint8_t bitMask, bool_t setBits )
{
    uint8_t newByte, oldByte;

    oldByte = SiiRegRead( virtualAddr );
    newByte = (setBits) ? (oldByte | bitMask) : (oldByte & ~bitMask);
    if ( oldByte != newByte )
    {
        SiiRegWrite( virtualAddr, newByte );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Reads a block of data from EDID record over DDC link.
//! @param[in]  segmentAddress  - Sixteen bit virtual EDID segment address, including device page.
//! @param[in]  virtualAddress  - Sixteen bit virtual EDID address, including device page.
//! @param[in]  pBuffer         - source data buffer.
//! @param[in]  count           - number of registers (bytes) to read.
//! @return     none
//-------------------------------------------------------------------------------------------------
void SiiRegEdidReadBlock ( SiiReg_t segmentAddr, SiiReg_t virtualAddr, uint8_t *pBuffer, uint16_t count )
{
    uint8_t             regOffset = (uint8_t)virtualAddr;
    pageConfig_t        *pPage;
    SiiI2cMsg_t         msgs[3];
    int                 msgIndex = 0;

    // Split into register address write and register data read messages.
    if ((segmentAddr & 0xFF) != 0)  // Default segment #0 index should not be sent explicitly
    {
        regOffset = (uint8_t)segmentAddr;
        segmentAddr >>= 8;
        pPage = &g_addrDescriptor[l_pageInstance[segmentAddr]][segmentAddr];

        // Write non-zero EDID segment address
        msgs[0].addr        = pPage->address;
        msgs[0].cmdFlags    = SII_MI2C_WR;
        msgs[0].len         = 1;
        msgs[0].pBuf        = &regOffset;
        msgIndex = 1;
    }

    // Read the actual EDID data
    regOffset = (uint8_t)virtualAddr;
    virtualAddr >>= 8;
    pPage = &g_addrDescriptor[l_pageInstance[virtualAddr]][virtualAddr];
    msgs[msgIndex].addr        = pPage->address;
    msgs[msgIndex].cmdFlags    = SII_MI2C_WR;
    msgs[msgIndex].len         = 1;
    msgs[msgIndex].pBuf        = &regOffset;
    msgIndex++;
    msgs[msgIndex].addr        = pPage->address;
    msgs[msgIndex].cmdFlags    = SII_MI2C_RD;
    msgs[msgIndex].len         = count;
    msgs[msgIndex].pBuf        = pBuffer;

    // Send the messages
    //craInstance.lastResultCode = (SiiDrvCraError_t)SiiPlatformI2cTransfer( pPage->busType, &msgs[0], sizeof( msgs ) / sizeof( SiiI2cMsg_t ));
}

