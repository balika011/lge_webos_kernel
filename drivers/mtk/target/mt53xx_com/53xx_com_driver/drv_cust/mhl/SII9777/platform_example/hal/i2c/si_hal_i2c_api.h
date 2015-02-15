/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of
 * Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file si_hal_i2c_api.h
 *
 * @brief register access APIs
 *
 *****************************************************************************/

#ifndef SI_HAL_I2C_API_H
#define SI_HAL_I2C_API_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public constant definitions *******************************************/

/***** public functions ******************************************************/

bool_t SiiHalI2cCreate( void );
void SiiHalI2cDelete( void );

int SiiHalI2cBlockRead(uint8_t hwId, uint16_t offset, uint8_t *buffer, uint16_t count);
int SiiHalI2cBlockWrite(uint8_t hwId, uint16_t offset, const uint8_t *buffer, uint16_t count);

#endif //SI_HAL_I2C_API_H

/***** end of file ***********************************************************/
