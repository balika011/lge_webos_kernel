//***************************************************************************
//!file     si_switch_component.h
//!brief    Silicon Image Port Processor Switch Component.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_SWITCH_COMPONENT_H__
#define __SI_SWITCH_COMPONENT_H__
#include "si_common.h"
#include "si_device_config.h"
#include "si_c99support.h"

//------------------------------------------------------------------------------
//  Manifest Constants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Switch Component Instance Data
//------------------------------------------------------------------------------

typedef struct _SiiSwitchInstanceData_t
{
	int			structVersion;
	int			lastResultCode;			// Contains the result of the last API function called
	uint16_t	statusFlags;
    uint8_t     selPort;

}	SiiSwitchInstanceData_t;


//------------------------------------------------------------------------------
//  Standard component functions
//------------------------------------------------------------------------------

bool_t   SiiSwitchConfigure ( void );
bool_t   SiiSwitchInitialize ( void );
uint16_t SiiSwitchStatus ( void );
bool_t   SiiSwitchStandby ( void );
bool_t   SiiSwitchResume ( void );
int_t    SiiSwitchGetLastResult ( void );

//-------------------------------------------------------------------------------
//  API Function Prototypes
//-------------------------------------------------------------------------------

void     SiiSwitchSelectSource( uint_t portIndex );
uint8_t  SiiSwitchGetNextPort ( uint8_t currentPort, bool_t isForward );

#endif // __SI_SWITCH_COMPONENT_H__
