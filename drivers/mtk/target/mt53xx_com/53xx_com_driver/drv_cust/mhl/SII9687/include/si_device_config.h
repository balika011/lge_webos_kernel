//***************************************************************************
//!file     si_device_config.h
//!brief    Sii9687 Device configuration file.
//!brief    Driver layer internal file
//          This file supplies the configuration of resources on the
//          Sii9687 device, including how many of each port, MHL channel,
//          ARC, HEC, etc.
//
//          This file is to be included in each driver-level module.  The
//          information contained in the file is propagated up to the
//          component level via driver API functions.
//
//          This file obtains some specific board implementation information
//          that may affect the configuration and usage of the actual device
//          resources via board API functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#ifndef __SI_DEVICECONFIG_H__
#define __SI_DEVICECONFIG_H__
#include "si_common.h"

//------------------------------------------------------------------------------
//  Compile-time build options
//------------------------------------------------------------------------------

#define USE_INTERNAL_MUTE       1       // Set to 1 to use SiI9687 muting
#define DO_EDID_INIT            1       // Set to 1 to perform EDID initialization during boot process


//------------------------------------------------------------------------------
//  Switch Component SiI9687-specific
//------------------------------------------------------------------------------

#define SII_NUM_SWITCH              1   // Number of SWITCH instances
#define SII_NUM_TX_SIMPLE           1   // Number of TX SIMPLE instances

#define SII_INPUT_PORT_COUNT        4

#define SWITCH_LINK_CALLBACK        ENABLE

//------------------------------------------------------------------------------
//  VSIF Component SiI9687-specific
//------------------------------------------------------------------------------

#define SII_NUM_VSIF                1   // Number of VSIF instances


//------------------------------------------------------------------------------
//  EDID Component SiI9687-specific
//------------------------------------------------------------------------------

#define SII_NUM_EDID                1   // Number of EDID instances

//------------------------------------------------------------------------------
//  CBUS Component SiI9687-specific
//------------------------------------------------------------------------------

#define SII_NUM_CBUS                    1   // Number of CBUS instances

#endif // __SI_DEVICECONFIG_H__
