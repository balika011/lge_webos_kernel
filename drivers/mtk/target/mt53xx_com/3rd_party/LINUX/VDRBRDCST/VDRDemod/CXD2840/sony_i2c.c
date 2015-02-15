/*------------------------------------------------------------------------------

 Copyright 2009-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/02/15
 Revision: 1.0.0.1

------------------------------------------------------------------------------*/

#include "sony_i2c.h"

#include "sony_stdlib.h" /* for memcpy */

#define BURST_WRITE_MAX 128 /* Max length of burst write */

sony_result_t sony_i2c_CommonReadRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_I2C_ENTER("sony_i2c_CommonReadRegister_DTMB");

    if(!pI2c){
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }

    result = pI2c->Write(pI2c, deviceAddress, &subAddress, 1, SONY_I2C_START_EN);
    if(result == SONY_RESULT_OK){
        result = pI2c->Read(pI2c, deviceAddress, pData, size, SONY_I2C_START_EN | SONY_I2C_STOP_EN);
    }

    SONY_TRACE_I2C_RETURN(result);
}

sony_result_t sony_i2c_CommonWriteRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t buffer[BURST_WRITE_MAX + 1];

    SONY_TRACE_I2C_ENTER("sony_i2c_CommonWriteRegister_DTMB");

    if(!pI2c){
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }
    if(size > BURST_WRITE_MAX){
        /* Buffer is too small... */
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }
    
    buffer[0] = subAddress;
    sony_memcpy(&(buffer[1]), pData, size);

    /* send the new buffer */
    result = pI2c->Write(pI2c, deviceAddress, buffer, size+1, SONY_I2C_START_EN | SONY_I2C_STOP_EN);
    SONY_TRACE_I2C_RETURN(result);
}

sony_result_t sony_i2c_CommonWriteOneRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_I2C_ENTER("sony_i2c_CommonWriteOneRegister_DTMB");

    if(!pI2c){
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }
    result = pI2c->WriteRegister(pI2c, deviceAddress, subAddress, &data, 1);
    SONY_TRACE_I2C_RETURN(result);
}

/* For Read-Modify-Write */
sony_result_t sony_i2c_SetRegisterBits_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_I2C_ENTER("sony_i2c_SetRegisterBits_DTMB");

    if(!pI2c){
        SONY_TRACE_I2C_RETURN(SONY_RESULT_ERROR_ARG);
    }
    if(mask == 0x00){
        /* Nothing to do */
        SONY_TRACE_I2C_RETURN(SONY_RESULT_OK);
    }
    
    if(mask != 0xFF){
        uint8_t rdata = 0x00;
        result = pI2c->ReadRegister(pI2c, deviceAddress, subAddress, &rdata, 1);
        if(result != SONY_RESULT_OK){ SONY_TRACE_I2C_RETURN(result); }
        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pI2c->WriteOneRegister(pI2c, deviceAddress, subAddress, data);
    SONY_TRACE_I2C_RETURN(result);
}

