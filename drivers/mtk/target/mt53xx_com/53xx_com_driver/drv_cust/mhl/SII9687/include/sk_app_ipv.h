//***************************************************************************
//!file     sk_app_ipv.h
//!brief    Application InstaPrevue
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if !defined __SKAPP_IPV_H__
#define __SKAPP_IPV_H__

#include "si_c99support.h"
#include "si_infrared.h"

#define SII_IPV_ALPHA_STEP_MS       80


bool_t  SkAppTaskIpvInitialize( void );
bool_t  SkAppIpvDemoMenu( RC5Commands_t key );
void    SkAppIpvCycleInstaPrevueWindows( bool_t isForward );
void    SkAppIpvDisableInstaPrevue( void );
bool_t  SkAppIpvPipModeEnable ( uint8_t pipSource );

void    SkAppTaskIpv( uint_t inst );

#endif  // __SKAPP_IPV_H__
