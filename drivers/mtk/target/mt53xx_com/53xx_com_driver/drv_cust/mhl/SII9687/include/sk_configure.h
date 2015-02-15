//***************************************************************************
//!file     sk_configure.h
//!brief    Configure the Application and drivers for a specific system design.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_SK_CONFIGURE_H__
#define __SI_SK_CONFIGURE_H__

//------------------------------------------------------------------------------
//  Compile-time build options
//------------------------------------------------------------------------------

#define REPEATER_I2C        0   // 1 == Enable, 0 == Disable


#define   INC_SETUP  1
#define   INC_ARC  1
#define   INC_DIAG  1
#define   INC_EDID  1
#define   INC_CBUS  1
#define   INC_DEBUGGER  1
#define   INC_IPV  1
#define   INC_IV  1
#define   INC_LCD_DISP  1



//------------------------------------------------------------------------------
//  Board-specific constants
//
//  These values are used by the application layer or driver layer, NOT by the
//  component layer, to specify physical board assignments
//------------------------------------------------------------------------------

#define SK_ARC_PORT         1   // HDMI port on board that is connected to ARC

#if 0
#if (INC_9617 == 1)
#define SK_CBUS_PORT        0   // HDMI port on board that is also MHL capable
#else
#define SK_CBUS_PORT        3   // HDMI port on board that is also MHL capable
#endif
#endif

#define SK_CBUS_PORT        3    //jane 20130124 for no mhl port 20130513 leipz


#define SK_LCD_I2C_PORT     1   // I2C port servicing LCD display (0 or 1)


#endif  // __SI_SK_CONFIGURE_H__
