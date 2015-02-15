/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012/04/09
  File Revision : 0.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONYRF_I2C_MTK_H
#define SONYRF_I2C_MTK_H

#include "sonyrf_i2c_re231.h"

/* Please enable following line if I2C Gateway is necessary */
#define SONYRF_I2C_SUPPORT_GW

sonyrf_result_t sonyrf_i2c_MTK_CreateI2c_re231(sonyrf_i2c_t* pI2c);
#ifdef SONYRF_I2C_SUPPORT_GW /* I2C Gateway is for Sony demodulator */
sonyrf_result_t sonyrf_i2c_MTK_CreateI2cGw_re231(sonyrf_i2c_t* pI2c, uint8_t gwAddress, uint8_t gwSub);
#endif

#endif /* SONYRF_I2C_MTK_H */
