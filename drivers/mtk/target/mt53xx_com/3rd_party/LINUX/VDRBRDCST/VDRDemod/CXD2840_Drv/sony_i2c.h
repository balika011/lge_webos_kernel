/*------------------------------------------------------------------------------

 Copyright 2009-2012 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/02/15
 Revision: 1.0.0.1

------------------------------------------------------------------------------*/

#ifndef SONY_I2C_H
#define SONY_I2C_H

#include "sony_common.h"

#define SONY_I2C_START_EN   (0x01)  /* Output START (S) */
#define SONY_I2C_STOP_EN    (0x02)  /* Output STOP  (P) */

typedef struct sony_i2c_t {
    /*--------------------------------------------------------------------
      Read/Write bytes from/to the I2C device.

      sony_i2c_t*       pI2c          : Instance of I2C control struct
      uint8_t           deviceAddress : The device address.
      uint8_t           subAddress    : The sub address.
      uint8_t           *pData        : The buffer to store the read from.
      uint32_t          size          : The number of bytes to read from the I2C device.
      uint8_t           mode          : The I2C mode (whether to output START/STOP conditions).
    --------------------------------------------------------------------*/
    sony_result_t (*Read)(struct sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode);
    sony_result_t (*Write)(struct sony_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t *pData, uint32_t size, uint8_t mode);
    sony_result_t (*ReadRegister)(struct sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size);
    sony_result_t (*WriteRegister)(struct sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t *pData, uint32_t size);
    /* Should be implemented using WriteRegister function */
    sony_result_t (*WriteOneRegister)(struct sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data);

    uint8_t          gwAddress; /* (Only for gateway access) Gateway address (Demod device address) */
    uint8_t          gwSub;     /* (Only for gateway access) Gateway sub address */

    uint32_t         flags;     /* Flags that can be used by i2c code */
    void*            user;      /* User defined */
} sony_i2c_t;

/*------------------------------------------------------------------------------
  I2C help functions
------------------------------------------------------------------------------*/
/* Common ReadRegister/WriteRegister/WriteOneRegister function implementation using Read/Write functions */
sony_result_t sony_i2c_CommonReadRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t* pData, uint32_t size);
sony_result_t sony_i2c_CommonWriteRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, const uint8_t* pData, uint32_t size);
sony_result_t sony_i2c_CommonWriteOneRegister_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data);

/* For Read-Modify-Write */
sony_result_t sony_i2c_SetRegisterBits_DTMB(sony_i2c_t* pI2c, uint8_t deviceAddress, uint8_t subAddress, uint8_t data, uint8_t mask);

#endif /* SONY_I2C_H */
