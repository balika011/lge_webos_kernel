//***************************************************************************
//! @file     si_lcd_st7032.h
//! @brief    LCD display driver
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_LCD_ST7032_H__
#define __SI_LCD_ST7032_H__

#include "si_common.h"


//-------------------------------------------------------------------------------
//
//  LCD API Functions
//
//-------------------------------------------------------------------------------

bool_t  SiiPlatformLcdInit(int i2cBusIndex);
void    SiiPlatformLcdClear(void);
void    SiiPlatformLcdPrint(char *text, int_t line);
void    SiiPlatformLcdPrintf(int_t line, const char *formatString, ...);
void    SiiPlatformLcdMoveText(int_t dir);

#endif  // __SI_LCD_ST7032_H__

