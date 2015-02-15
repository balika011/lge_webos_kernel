/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012/04/09
  File Revision : 0.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONY_I2C_MTK_H
#define SONY_I2C_MTK_H

#include "sony_i2c.h"

/* Please enable following line if I2C Gateway is necessary */
#define SONY_I2C_SUPPORT_GW

#define SONY_I2C_MTK_BUSID_A  4   /* I2C_A bus ID */
#define SONY_I2C_MTK_BUSID_D  1   /* I2C_D bus ID */

sony_result_t sony_i2c_MTK_CreateI2c(sony_i2c_t* pI2c, uint8_t busID);
#ifdef SONY_I2C_SUPPORT_GW /* I2C Gateway is for Sony demodulator */
sony_result_t sony_i2c_MTK_CreateI2cGw(sony_i2c_t* pI2c, uint8_t gwAddress, uint8_t gwSub, uint8_t busID);
#endif

#endif /* SONY_I2C_MTK_H */
