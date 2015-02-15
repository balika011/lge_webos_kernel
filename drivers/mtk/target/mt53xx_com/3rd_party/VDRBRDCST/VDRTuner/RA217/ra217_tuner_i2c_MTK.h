/*------------------------------------------------------------------------------

 Copyright 2011 Sony Corporation

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
 No part of this file may be copied, modified, sold, and distributed in any
 form or by any means without prior explicit permission in writing from
 Sony Corporation.

 Date: 2011/01/11
 Revision: 1.0.0.0

------------------------------------------------------------------------------*/

#ifndef RA217_I2C_MTK_H
#define RA217_I2C_MTK_H

#include "ra217_tuner_i2c.h"

ra217_result_t ra217_tuner_i2c_MTK_CreateI2c(ra217_tuner_i2c_t* pI2c);
ra217_result_t ra217_tuner_i2c_MTK_CreateI2cGw(ra217_tuner_i2c_t* pI2c, UINT8 gwAddress, UINT8 gwSub);

#endif /* RA217_I2C_MTK_H */
