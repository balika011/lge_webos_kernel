//******************************************************************************
// !file    si_drv_arc.c
// !brief   Silicon Image Sii9687 ARC driver
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_drv_device.h"
#include "si_cra.h"
#include "si_drv_arc.h"


//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable ARC single mode
//!
//! @param[in]  param - meaning
//-------------------------------------------------------------------------------------------------

void SiiDrvArcSingleModeEnable (bool_t isEnabled)
{
    SiiRegBitsSet(REG_ARCRX_TEST1, REG_ARCRX_TEST1_DEFAULT, isEnabled);
    SiiRegBitsSet(REG_ARCRX_TEST2, REG_ARCRX_TEST2_DEFAULT, isEnabled);
}

