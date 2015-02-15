//***************************************************************************
//!file     si_drv_diag_internal.h
//!brief    Silicon Image Diagnostics internal driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DIAG_DRV_INTERNAL_H__
#define __SI_DIAG_DRV_INTERNAL_H__
#include "si_common.h"
#include "si_drv_diag.h"

//-------------------------------------------------------------------------------
// DIAG Enums and manifest constants
//-------------------------------------------------------------------------------

#define BIST_TEST_TIMEOUT       150     // Max BIST test time in milliseconds

#define REG_HDMI_TX_CLK_DEBUG   0x0B10
#define REG_CH0_EPST            0x00F9
#define REG_CH0_EPCM            0x00FA
#define REG_CH0_EPCNT           0x00FB
#define REG_CH1_EPST            0x00F7
#define REG_CH1_EPCM            0x00F8
#define REG_CH2_EPST            0x00F5
#define REG_CH2_EPCM            0x00F6
#define REG_CH3_EPST            0x00F3
#define REG_CH3_EPCM            0x00F4


#define BIT_CRC_TEST                    0x04    // EPCM
#define BIT_SELF_AUTH1_TEST             0x08
#define BIT_SELF_AUTH2_TEST             0x10

#define BIT_BIST_COMPLETE               0x01    // EPST
#define BIT_CRC_FAIL                    0x02
#define BIT_SELF_AUTH1_FAIL             0x20
#define BIT_SELF_AUTH2_FAIL             0x40

#define REG_PA_STAT_REG_11      0x05D7



//------------------------------------------------------------------------------
//  Diagnostics Driver Instance Data
//------------------------------------------------------------------------------

typedef struct _DiagInstanceData_t
{
    int             structVersion;
    int             instanceIndex;
    int             lastResultCode;            // Contains the result of the last API function called
    SiiDrvDiagStatus_t statusFlags;

} DiagInstanceData_t;

extern DiagInstanceData_t diagInstance[1];

//------------------------------------------------------------------------------
//  Driver Specific functions
//------------------------------------------------------------------------------


#endif // __SI_DIAG_DRV_INTERNAL_H__
