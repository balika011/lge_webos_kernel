//***************************************************************************
//!file     si_drv_diag.h
//!brief    Silicon Image Diagnostics driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DIAG_DRV_H__
#define __SI_DIAG_DRV_H__
#include "si_common.h"
#include "si_c99support.h"

//-------------------------------------------------------------------------------
// Enums and manifest constants
//-------------------------------------------------------------------------------

typedef enum _SiiDrvDiagStatus_t
{
    SiiDiagStatus_BIST_FAIL     = 0x0001,
} SiiDrvDiagStatus_t;

typedef enum _SiiDiagError_t
{
    SiiDIAG_SUCCESS             = 0,
    SiiDIAG_INVALID_PARAMETER,          // Invalid parameter passed to test
    SiiDIAG_BIST_TEST_FAIL,
    SiiDIAG_BIST_NOT_CLEAR,             // DONE bit did not clear on entry to test
    SiiDIAG_BIST_NOT_COMPLETE,          // DONE bit not get set after test started
    SiiDIAG_NO_CLOCK_ON_PORT,           // Port does not have CKDT, cannot test BIST
    SiiDIAG_NVRAM_READ_ERROR,           // Error reading NVRAM boot data

} SiiDiagError_t;

typedef enum _SiiDiagBistTestIndex_t
{
    SiiDIAG_BIST_CRC_TEST           = 0,
    SiiDIAG_BIST_SELF_AUTH1_TEST,
    SiiDIAG_BIST_SELF_AUTH2_TEST,
} SiiDiagBistTestIndex_t;

//-------------------------------------------------------------------------------
// CPI data structures
//-------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Standard driver functions
//------------------------------------------------------------------------------

uint16_t    SiiDrvDiagStatus( void );
uint_t      SiiDrvDiagGetLastResult( void );

bool_t      SiiDrvDiagBistValidatePort( int_t portIndex );
#if 0
bool_t      SiiDrvDiagBistRun( int_t testIndex, int_t portIndex );
#endif
int_t       SiiDrvDiagEccErrorCount( bool_t reset );
bool_t      SiiDrvDiagEtpgEnabled( void );
uint_t      SiiDrvDiagHdcpGoodStatusGet( uint8_t portMap );
uint_t      SiiDrvDiagPortConnectedGet( uint8_t portMap, bool_t requireClock );
uint_t      SiiDrvDiagHdcpModeStatusGet( uint8_t portMap );

#endif // __SI_DIAG_DRV_H__
