//***************************************************************************

//!file     si_cp1292.h
//!brief    CP 1292 FPGA/Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_CP1292_H__
#define __SI_CP1292_H__

#ifndef SUPPORT_ARM_CC
#include <stdio.h>
#endif
#include "si_hal.h"
#include "si_regio.h"
#include "si_api1292.h"
//#include <si_apiConfigure.h>

//------------------------------------------------------------------------------
//
//  Compile-time build options
//
//------------------------------------------------------------------------------

#define FPGA_BUILD 0           // 1 == FPGA, 0 == Silicon

//------------------------------------------------------------------------------
//
//  CP 1292 Starter Kit Manifest Constants
//
//------------------------------------------------------------------------------

//#define DEM_POLLING_DELAY   100         // Main Loop Polling interval (ms)

//-------------------------------------------------------------------------------
//  Chip Mode for CP 1292
//-------------------------------------------------------------------------------

typedef enum
{
    HDMI = 0,
    MHL
}ENUM_DEVICE_TYPE;


//------------------------------------------------------------------------------
//  typedefs
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t portSelect;
    uint8_t edidLoad;

} SI_CP1292Config_t;

//------------------------------------------------------------------------------
//  Global Data
//------------------------------------------------------------------------------

extern SI_CP1292Config_t g_data;

extern uint8_t g_currentInputMode;

extern bool_t g_MHL_Source_Connected;

extern bool_t g_HDMI_Source_Connected;
extern bool_t g_HDMI_Source_Valid;

extern bool_t g_HPD_IN;
extern bool_t g_TX_RSEN;
extern bool_t g_TX_RSEN_Valid;

#ifndef SUPPORT_ARM_CC
extern BOOL g_deviceInterrupt;
#endif

extern uint8_t PeerDevCap[16];

#define 			timer_count 		4

extern BOOL g_Support_MHL2;
extern BOOL g_Support_Charger_900mA;
extern bool_t g_Support_ThreeD;
extern bool_t g_Non3D_WB_Processing;
extern bool_t g_3D_WB_Processing;

//-------------------------------------------------------------------------------
//  Function Prototypes
//-------------------------------------------------------------------------------

#if (FPGA_BUILD == 0)
void	CpCheckOTPRev( void );
#endif // FPGA_BUILD == 0

bool_t	CpIrHandler( void );

// For MHL cable detect and wakeup pulse detect
BOOL	CpCableDetect( void );


/* si_cpCbus.c      */

void	CpCbusHandler( void );

void MHL_toggle_500ms(void);

#endif  // __SI_CP1292_H__
