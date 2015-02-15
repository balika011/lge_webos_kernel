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
 * @file si_drv_isp.h
 *
 * @brief ISP APIs
 *
 *****************************************************************************/

#ifndef SI_DRV_ISP_H
#define SI_DRV_ISP_H

/***** #include statements ***************************************************/

#include "si_datatypes.h"

/***** public constant definitions *******************************************/

#define SII_DRV_SPI_BUSRT_WRITE_SIZE         256

/***** public functions ******************************************************/

void SiiDrvIspChipErase(void);
void SiiDrvIspBurstWrite(uint32_t addr, const uint8_t* pData, uint16_t len);
bool_t SiiDrvIspOperationDone(void);

#endif SI_DRV_ISP_H

/***** end of file ***********************************************************/
