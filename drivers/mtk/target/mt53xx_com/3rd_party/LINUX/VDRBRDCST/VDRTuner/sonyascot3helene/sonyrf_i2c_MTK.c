/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012/04/09
  File Revision : 0.0.0.0
------------------------------------------------------------------------------*/

#include "sonyrf_i2c_MTK.h"
#include "i2c_api.h"
#include "x_os.h"

#define BURST_WRITE_MAX 128

static sonyrf_result_t sonyrf_i2c_MTK_Read(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData,
                  uint32_t size, uint8_t mode)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_Read");

    SONYRF_ARG_UNUSED(mode);

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    if(ucI2cReadOnly_OptBus(pI2c->busID, deviceAddress, (UINT8*)pData, (UINT16)size) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}

static sonyrf_result_t sonyrf_i2c_MTK_Write(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t * pData,
                  uint32_t size, uint8_t mode)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_Write");

    SONYRF_ARG_UNUSED(mode);

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    if(ucI2cWriteOnly_OptBus(pI2c->busID, deviceAddress, (UINT8*)pData, (UINT16)size) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}

#ifdef SONYRF_I2C_SUPPORT_GW /* I2C Gateway is for Sony demodulator */
static sonyrf_result_t sonyrf_i2c_MTK_ReadGw(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData,
                  uint32_t size, uint8_t mode)
{
    UINT32 wordAddr = 0;

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_ReadGw");

    SONYRF_ARG_UNUSED(mode);

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    // I2C address in MTK function should be in 8bit form
    wordAddr = (pI2c->gwSub << 8) | (deviceAddress | 0x01); // I2C Read

    // ucI2cReadOnly_Gateway is for reading from I2CGW (same as Toshiba)
    if(ucI2cReadOnly_Gateway_OptBus(pI2c->busID, pI2c->gwAddress, wordAddr, (UINT8 *)pData, (UINT16)size) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}

static sonyrf_result_t sonyrf_i2c_MTK_WriteGw(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t * pData,
                  uint32_t size, uint8_t mode)
{
    UINT8 buffer[BURST_WRITE_MAX + 1];

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_WriteGw");

    SONYRF_ARG_UNUSED(mode);

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    buffer[0] = deviceAddress;
    x_memcpy(&(buffer[1]), pData, size);

    if(ucI2cWrite_OptBus(pI2c->busID, pI2c->gwAddress, pI2c->gwSub, buffer, (UINT16)(size + 1)) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}
#endif /* SONYRF_I2C_SUPPORT_GW */

static sonyrf_result_t sonyrf_i2c_MTK_ReadRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress,
                  uint8_t* pData, uint32_t size)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_ReadRegister");

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    if(ucI2cRead_OptBus(pI2c->busID, deviceAddress, subAddress, (UINT8*)pData, (UINT16)size) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}

static sonyrf_result_t sonyrf_i2c_MTK_WriteRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress,
                  const uint8_t* pData, uint32_t size)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_WriteRegister");

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    if(ucI2cWrite_OptBus(pI2c->busID, deviceAddress, subAddress, (UINT8*)pData, (UINT16)size) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}

#ifdef SONYRF_I2C_SUPPORT_GW /* I2C Gateway is for Sony demodulator */
static sonyrf_result_t sonyrf_i2c_MTK_WriteRegisterGw(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress,
                  const uint8_t* pData, uint32_t size)
{
    UINT8 buffer[BURST_WRITE_MAX + 2];

    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_WriteRegisterGw");

    if((!pI2c) || (!pData) || (size == 0)) SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);

    buffer[0] = deviceAddress;
    buffer[1] = subAddress;
    x_memcpy(&(buffer[2]), pData, size);

    if(ucI2cWrite_OptBus(pI2c->busID, pI2c->gwAddress, pI2c->gwSub, buffer, (UINT16)(size + 2)) == 0){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
    }else{
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_I2C);
    }
}
#endif /* SONYRF_I2C_SUPPORT_GW */

sonyrf_result_t sonyrf_i2c_MTK_CreateI2c(sonyrf_i2c_t* pI2c, uint8_t busID)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_CreateI2c");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    pI2c->Read = sonyrf_i2c_MTK_Read;
    pI2c->Write = sonyrf_i2c_MTK_Write;
    pI2c->ReadRegister = sonyrf_i2c_MTK_ReadRegister;
    pI2c->WriteRegister = sonyrf_i2c_MTK_WriteRegister;
    pI2c->WriteOneRegister = sonyrf_i2c_CommonWriteOneRegister;
    pI2c->gwAddress = 0;
    pI2c->gwSub = 0;
    pI2c->busID = busID;
    pI2c->user = NULL;

    SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
}

#ifdef SONYRF_I2C_SUPPORT_GW /* I2C Gateway is for Sony demodulator */
sonyrf_result_t sonyrf_i2c_MTK_CreateI2cGw(sonyrf_i2c_t* pI2c, uint8_t gwAddress, uint8_t gwSub, uint8_t busID)
{
    SONYRF_TRACE_I2C_ENTER("sonyrf_i2c_MTK_CreateI2cGw");

    if(!pI2c){
        SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    pI2c->Read = sonyrf_i2c_MTK_ReadGw;
    pI2c->Write = sonyrf_i2c_MTK_WriteGw;
    pI2c->ReadRegister = sonyrf_i2c_CommonReadRegister;
    pI2c->WriteRegister = sonyrf_i2c_MTK_WriteRegisterGw;
    pI2c->WriteOneRegister = sonyrf_i2c_CommonWriteOneRegister;
    pI2c->gwAddress = gwAddress;
    pI2c->gwSub = gwSub;
    pI2c->busID = busID;
    pI2c->user = NULL;

    SONYRF_TRACE_I2C_RETURN(SONYRF_RESULT_OK);
}
#endif /* SONYRF_I2C_SUPPORT_GW */
