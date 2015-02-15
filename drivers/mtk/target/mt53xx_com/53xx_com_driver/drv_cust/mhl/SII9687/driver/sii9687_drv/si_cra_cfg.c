//***************************************************************************
//!file     si_cra_cfg.c
//!brief    Silicon Image Si9678 Starter Kit Firmware CRA configuration data.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_common.h"
#include "si_cra.h"
#include "si_cra_cfg.h"
#include "si_sii9687_registers.h"


//------------------------------------------------------------------------------
// si_cra_cfg.h
//------------------------------------------------------------------------------

// Index to this array is the virtual page number in the MSB of the REG_xxx address values
// Indexed with siiRegPageIndex_t value shifted right 8 bits
// DEV_PAGE values must correspond to the order specified in the SiiRegPageIndex_t enum list
pageConfig_t    g_addrDescriptor[SII_CRA_MAX_DEVICE_INSTANCES][SII_CRA_DEVICE_PAGE_COUNT] =
{
    // Instance 0 (only one for 9687)
    {
    { DEV_I2C_0,        DEV_PAGE_PP_0   },    // Port Processor
    { DEV_I2C_0,        DEV_PAGE_IPV    },    // InstaPreview
    { DEV_I2C_0,        DEV_PAGE_PP_2   },    // RX TMDS
    { DEV_I2C_0,        DEV_PAGE_PP_5   },    // PAUTH0
    { DEV_I2C_0,        DEV_PAGE_PP_6   },    // PAUTH1
    { DEV_I2C_0,        DEV_PAGE_PP_7   },    // PAUTH2
    { DEV_I2C_0,        DEV_PAGE_PP_9   },    // NVRAM
    { DEV_I2C_0,        DEV_PAGE_PP_A   },    // RX TMDS
    { DEV_I2C_0,        DEV_PAGE_PP_B   },    // TX TMDS
    { DEV_I2C_0,        DEV_PAGE_CBUS   },    // CBUS 0

    { DEV_DDC_0,        DEV_PAGE_DDC_EDID },  // TX EDID DDC 0
    { DEV_DDC_0,        DEV_PAGE_DDC_SEGM },  // TX EDID DDC 0
    }
};

// Register addresses for re-assigning page base addresses
// These registers specify the I2C address that the SI device will
// respond to for the specific control register page
SiiReg_t g_siiRegPageBaseRegs [SII_CRA_DEVICE_PAGE_COUNT] =
{
    PP_PAGE_0 | 0xFF,   // Device Base  - Cannot be reassigned
    REG_SLAVE_ADDR11,   // IPV
    REG_SLAVE_ADDR6,    // RX TMDS, Page 2
    REG_SLAVE_ADDR8,    // PA Pages 5,6,7 have special mapping requirements
    REG_SLAVE_ADDR8,    // PA
    REG_SLAVE_ADDR8,    // PA
    REG_SLAVE_ADDR3,    // NVRAM/GPIO
    REG_SLAVE_ADDR4,    // RX TMDS, Page A
    REG_SLAVE_ADDR5,    // TX TMDS, page B
    REG_SLAVE_ADDR1,    // CBUS
    PP_PAGE_0 | 0xFF,   // TX DDC EDID  - Cannot be reassigned
    PP_PAGE_0 | 0xFF,   // TX DDC EDID Segment address  - Cannot be reassigned
};

// TODO:OEM - Add entries to reassign register page base addresses if needed
SiiReg_t g_siiRegPageBaseReassign [] =
{
//        PP_PAGE_3 | 0xFC,       // Example of changing default page 3 device ID (0xFA) to 0xFC
//                                // Note that the DEV_PAGE_IPV enum value must also be changed

        0xFFFF      // End of reassignment list
};
