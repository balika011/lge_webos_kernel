/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : $Date: 2015/02/15 $
  File Revision : $Revision: #1 $
------------------------------------------------------------------------------*/

#include "sonysat_i2c.h"

#include "sonysat_stdlib.h" /* for memcpy */

#define BURST_WRITE_MAX 128 /* Max length of burst write */

sonysat_result_t sonysat_i2c_CommonReadRegister(sonysat_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_I2C_ENTER("sonysat_i2c_CommonReadRegister");

    if(!pI2c){
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }

    result = pI2c->Write(pI2c, deviceAddress, &subAddress, 1, SONYSAT_I2C_START_EN);
    if(result == SONYSAT_RESULT_OK){
        result = pI2c->Read(pI2c, deviceAddress, pData, size, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN);
    }

    SONYSAT_TRACE_I2C_RETURN(result);
}

sonysat_result_t sonysat_i2c_CommonWriteRegister(sonysat_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;
    uint8_t buffer[BURST_WRITE_MAX + 1];

    SONYSAT_TRACE_I2C_ENTER("sonysat_i2c_CommonWriteRegister");

    if(!pI2c){
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    if(size > BURST_WRITE_MAX){
        /* Buffer is too small... */
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    
    buffer[0] = subAddress;
    sonysat_memcpy(&(buffer[1]), pData, size);

    /* send the new buffer */
    result = pI2c->Write(pI2c, deviceAddress, buffer, size+1, SONYSAT_I2C_START_EN | SONYSAT_I2C_STOP_EN);
    SONYSAT_TRACE_I2C_RETURN(result);
}

sonysat_result_t sonysat_i2c_CommonWriteOneRegister(sonysat_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_I2C_ENTER("sonysat_i2c_CommonWriteOneRegister");

    if(!pI2c){
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    result = pI2c->WriteRegister(pI2c, deviceAddress, subAddress, &data, 1);
    SONYSAT_TRACE_I2C_RETURN(result);
}

/* For Read-Modify-Write */
sonysat_result_t sonysat_i2c_SetRegisterBits(sonysat_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask)
{
    sonysat_result_t result = SONYSAT_RESULT_OK;

    SONYSAT_TRACE_I2C_ENTER("sonysat_i2c_SetRegisterBits");

    if(!pI2c){
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_ERROR_ARG);
    }
    if(mask == 0x00){
        /* Nothing to do */
        SONYSAT_TRACE_I2C_RETURN(SONYSAT_RESULT_OK);
    }
    
    if(mask != 0xFF){
        uint8_t rdata = 0x00;
        result = pI2c->ReadRegister(pI2c, deviceAddress, subAddress, &rdata, 1);
        if(result != SONYSAT_RESULT_OK){ SONYSAT_TRACE_I2C_RETURN(result); }
        data = (uint8_t)((data & mask) | (rdata & (mask ^ 0xFF)));
    }

    result = pI2c->WriteOneRegister(pI2c, deviceAddress, subAddress, data);
    SONYSAT_TRACE_I2C_RETURN(result);
}

