/*------------------------------------------------------------------------------
  Copyright 2009-2012 Sony Corporation

  Last Updated  : 2012/03/14
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONYRF_I2C_H
#define SONYRF_I2C_H

#include "sonyrf_common.h"

#define SONYRF_I2C_START_EN   (0x01)  /* Output START (S) */
#define SONYRF_I2C_STOP_EN    (0x02)  /* Output STOP  (P) */

typedef struct sonyrf_i2c_t {
    /*--------------------------------------------------------------------
      Read/Write bytes from/to the I2C device.

      sonyrf_i2c_t*       pI2c          : Instance of I2C control struct
      uint8_t           deviceAddress : The device address.
      uint8_t           subAddress    : The sub address.
      uint8_t           *pData        : The buffer to store the read from.
      uint32_t          size          : The number of bytes to read from the I2C device.
      uint8_t           mode          : The I2C mode (whether to output START/STOP conditions).
    --------------------------------------------------------------------*/
    sonyrf_result_t (*Read)(struct sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode);
    sonyrf_result_t (*Write)(struct sonyrf_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode);
    sonyrf_result_t (*ReadRegister)(struct sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size);
    sonyrf_result_t (*WriteRegister)(struct sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t *pData, uint32_t size);
    /* Should be implemented using WriteRegister function */
    sonyrf_result_t (*WriteOneRegister)(struct sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data);

    uint8_t          gwAddress; /* (Only for gateway access) Gateway address (Demod device address) */
    uint8_t          gwSub;     /* (Only for gateway access) Gateway sub address */

    uint32_t         flags;     /* Flags that can be used by i2c code */
    uint8_t          busID;     /**< Only for MTK I2C_A : 4, I2C_D : 1 */
    void*            user;      /* User defined */
} sonyrf_i2c_t;

/*------------------------------------------------------------------------------
  I2C help functions
------------------------------------------------------------------------------*/
/* Common ReadRegister/WriteRegister/WriteOneRegister function implementation using Read/Write functions */
sonyrf_result_t sonyrf_i2c_CommonReadRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size);
sonyrf_result_t sonyrf_i2c_CommonWriteRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size);
sonyrf_result_t sonyrf_i2c_CommonWriteOneRegister(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data);

/* For Read-Modify-Write */
sonyrf_result_t sonyrf_i2c_SetRegisterBits(sonyrf_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask);

#endif /* SONYRF_I2C_H */
