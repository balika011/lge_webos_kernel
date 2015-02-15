/*------------------------------------------------------------------------------
  Copyright 2009-2012 Sony Corporation

  Last Updated  : 2012/03/14
  File Revision : 1.0.1.0
------------------------------------------------------------------------------*/

#include "sonyrf_i2c.h"

#include "sonyrf_stdlib.h" /* for memcpy */

#define BURST_WRITE_MAX 128 /* Max length of burst write */

sonyrf_result_t sonyrf_i2c_CommonReadRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_CommonReadRegister");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    result = pI2c->Write(pI2c, deviceAddress, &subAddress, 1, SONYRF_I2C_START_EN);
    if(result == SONYRF_RESULT_OK){
        result = pI2c->Read(pI2c, deviceAddress, pData, size, SONYRF_I2C_START_EN | SONYRF_I2C_STOP_EN);
    }

    SONYRF_TRACE_I2C_RETURN(result);
}

sonyrf_result_t sonyrf_i2c_CommonWriteRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t buffer[BURST_WRITE_MAX + 1];

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_CommonWriteRegister");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }
    if(size > BURST_WRITE_MAX){
        /* Buffer is too small... */
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }
    
    buffer[0] = subAddress;
    sonyrf_memcpy(&(buffer[1]), pData, size);

    /* send the new buffer */
    result = pI2c->Write(pI2c, deviceAddress, buffer, size+1, SONYRF_I2C_START_EN | SONYRF_I2C_STOP_EN);
    SONYRF_TRACE_I2C_RETURN(result);
}

sonyrf_result_t sonyrf_i2c_CommonWriteOneRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_CommonWriteOneRegister");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }
    result = pI2c->WriteRegister(pI2c, deviceAddress, subAddress, &data, 1);
    SONYRF_TRACE_I2C_RETURN(result);
}

/* For Read-Modify-Write */
sonyrf_result_t sonyrf_i2c_SetRegisterBits(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_SetRegisterBits");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }
    if(mask == 0x00){
        /* Nothing to do */
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }
    
    if(mask != 0xFF){
        uint8_t rdata = 0x00;
        result = pI2c->ReadRegister(pI2c, deviceAddress, subAddress, &rdata, 1);
        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_I2C_RETURN(result); }
        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pI2c->WriteOneRegister(pI2c, deviceAddress, subAddress, data);
    SONYRF_TRACE_I2C_RETURN(result);
}

