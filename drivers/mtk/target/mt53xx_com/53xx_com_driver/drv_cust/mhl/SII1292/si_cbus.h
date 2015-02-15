//***************************************************************************

//!file     si_cbus.h
//!brief    si_apiCbus wrapper for multiple chip.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef _CBUS_H
#define _CBUS_H

//------------------------------------------------------------------------------
//  1292 definitions
//------------------------------------------------------------------------------
//
// Where specific CBUS registers are located
//

#define CBUS_I2C_BUS_ADDRESS (0xC8)  // I2C slave address for CBUS

#define CBUS_DEVCAP_BASE (0x80)
#define CBUS_INTR_BASE (0xA0)
#define CBUS_STAT_BASE (0xB0)
#define CBUS_SCRATCHPAD_BASE (0xC0)

// Chip specific MHL parameters.
#if 0//Keno20120306, disablement for API use. It is defined to drv_sil_if.h
#define MHL_DEVICE_CATEGORY (MHL_DEV_CAT_SINK)

#define MHL_LOGICAL_DEVICE (LD_DISPLAY | LD_SPEAKER)
#endif
#endif  // _CBUS_H
