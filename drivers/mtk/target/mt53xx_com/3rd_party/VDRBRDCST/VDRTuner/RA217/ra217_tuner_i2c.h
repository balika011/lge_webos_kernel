/*------------------------------------------------------------------------------

 Copyright 2009-2010 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2010/06/04
 Revision: 1.0.0.0

------------------------------------------------------------------------------*/

#ifndef RA217_TUNER_I2C_H
#define RA217_TUNER_I2C_H

#include "ra217_tuner_common.h"

#define RA217_TUNER_I2C_START_EN   (0x01)  /* Output START (S) */
#define RA217_TUNER_I2C_STOP_EN    (0x02)  /* Output STOP  (P) */

typedef struct ra217_tuner_i2c_t {
	/*--------------------------------------------------------------------
	  Read/Write bytes from/to the I2C device.

	  ra217_tuner_i2c_t*       pI2c          : Instance of I2C control struct
	  UINT8           deviceAddress : The device address.
	  UINT8           subAddress    : The sub address.
	  UINT8           *pData        : The buffer to store the read from.
	  UINT32          size          : The number of bytes to read from the I2C device.
	  UINT8           mode          : The I2C mode (whether to output START/STOP conditions).
	--------------------------------------------------------------------*/
	ra217_result_t (*Read)(struct ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8* pData, UINT32 size, UINT8 mode);
	ra217_result_t (*Write)(struct ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, const UINT8 *pData, UINT32 size, UINT8 mode);
	ra217_result_t (*ReadRegister)(struct ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, UINT8* pData, UINT32 size);
	ra217_result_t (*WriteRegister)(struct ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, const UINT8 *pData, UINT32 size);
	/* Should be implemented using WriteRegister function */
	ra217_result_t (*WriteOneRegister)(struct ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, UINT8 data);

	UINT8          gwAddress; /* (Only for gateway access) Gateway address (Demod device address) */
	UINT8          gwSub;     /* (Only for gateway access) Gateway sub address */

	UINT32         flags;     /* Flags that can be used by i2c code */
	void*            user;      /* User defined */
} ra217_tuner_i2c_t;

/*------------------------------------------------------------------------------
  I2C help functions
------------------------------------------------------------------------------*/
/* Common ReadRegister/WriteRegister/WriteOneRegister function implementation using Read/Write functions */
ra217_result_t ra217_tuner_i2c_CommonReadRegister(ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, UINT8* pData, UINT32 size);
ra217_result_t ra217_tuner_i2c_CommonWriteRegister(ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, const UINT8* pData, UINT32 size);
ra217_result_t ra217_tuner_i2c_CommonWriteOneRegister(ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, UINT8 data);

/* For Read-Modify-Write */
ra217_result_t ra217_tuner_i2c_SetRegisterBits(ra217_tuner_i2c_t* pI2c, UINT8 deviceAddress, UINT8 subAddress, UINT8 data, UINT8 mask);

#endif /* RA217_I2C_H */
