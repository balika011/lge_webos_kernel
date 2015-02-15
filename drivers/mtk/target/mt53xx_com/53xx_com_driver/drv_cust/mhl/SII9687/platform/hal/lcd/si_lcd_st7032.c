//***************************************************************************
//! @file     si_lcd_st7032.c
//! @brief    LCD display driver
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "stdarg.h"
#include <linux/string.h>
#include "si_common.h"
#include "si_osal_timer.h"
#include "si_i2c.h"
#include "si_lcd_st7032.h"


//-------------------------------------------------------------------------------
//
//  Constants definitions
//
//-------------------------------------------------------------------------------

#define LCD_I2C_ADDRESS         0x7C

#define LCD_REG_COMMAND         0x00
#define LCD_REG_DATA            0x40

#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_RET_HOME        0x02
#define LCD_CMD_ENTRY_MODE      0x06  // Cursor moves right, no display shift
#define LCD_CMD_DISP_ON         0x0C  // no cursor, no cursor blinking
#define LCD_CMD_DDRAM_ADDR_SET  0x80

#define LCD_CMD_FUNC            0x30
#define BIT_TWO_LINES           (BIT3) // rather than 1 line
#define BIT_DOUBLE_HEIGHT_FONT  (BIT2)
#define BIT_INSTR_TAB1_SELECT   (BIT0) // rather than selecting INSTR table 0

#define LCD_FUNC0_DISP_SHFT     0x10
#define BIT_SET_SCREEN          (BIT3) // rather than cursor
#define BIT_MOVE_RIGHT          (BIT2) // rather than left

#define LCD_FUNC1_INT_OSC       0x14
#define LCD_FUNC1_PWR_CTRL      0x5D
#define LCD_FUNC1_FLWR_CTRL     0x6D
#define LCD_FUNC1_CONTRAST      0x7C

#define LCD_VISIBLE_COLS        16
#define LCD_TOTAL_COLS          40


//-------------------------------------------------------------------------------
//
//  Local types
//
//-------------------------------------------------------------------------------

typedef struct _SiiLcdCmdTrain_t
{
    uint8_t  value;
    uint8_t  delayMs;

} SiiLcdCmdTrain_t;

//-------------------------------------------------------------------------------
//
//  Global data
//
//-------------------------------------------------------------------------------

static int gI2cBusIndex = 0;

//-------------------------------------------------------------------------------
//
//  Local functions
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//! @brief      Write command or data sequence to LCD module over I2C interface
//!             with specified delays after each transaction.
//!
//! @param[in]  pTrain   - pointer to a "command train" (pairs of data and delays)
//! @param[in]  len      - number of elements in the command train
//-------------------------------------------------------------------------------------------------

static bool_t i2cLcdWriteCmdTrain(SiiLcdCmdTrain_t *pTrain, uint8_t len)
{
    bool_t success = true;

    int_t i = 0;

    if ( SiiPlatformI2cSendStart( gI2cBusIndex, LCD_I2C_ADDRESS, &(pTrain->value), 1, len <= 1 ) != PLATFORM_SUCCESS )
    {
        success = false;
    }

    if (pTrain->delayMs)
    {
        SiiOsTimerWait(pTrain->delayMs);
    }

    i = 1;
    while ((i < len) && success)
    {
        if (SiiPlatformI2cSendContinue(gI2cBusIndex, &(pTrain[i].value), 1, i == (len - 1)) != PLATFORM_SUCCESS )
        {
            success = false;
        }
        else
        {
            if (pTrain[i].delayMs)
            {
                SiiOsTimerWait(pTrain[i].delayMs);
            }
            i++;
        }
    }

    return success;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Write data block (text) to LCD module over I2C interface.
//!
//! @param[in]  pBuf   - pointer to data buffer
//! @param[in]  bufLen - number of bytes in the buffer
//-------------------------------------------------------------------------------------------------

static bool_t i2cLcdWriteBlock(uint8_t *pBuf, uint_t bufLen)
{
    bool_t success = true;
    int_t i = 0;

    if (SiiPlatformI2cSendStart( gI2cBusIndex, LCD_I2C_ADDRESS, pBuf, 1, bufLen <= 1 ) != PLATFORM_SUCCESS)
    {
        success = false;
    }
    i = 1;
    while ((i < bufLen) && success)
    {
        if (SiiPlatformI2cSendContinue(gI2cBusIndex, &(pBuf[i]), 1, i == (bufLen - 1)) != PLATFORM_SUCCESS )
        {
            success = false;
        }
        else
        {
            i++;
        }
    }

    return success;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set cursor position in the beginning of specified line.
//!
//! @param[in]  line - line index on LCD screen (0 or 1)
//-------------------------------------------------------------------------------------------------

static void SetLine(int_t line)
{
    uint8_t cmds[2] = {LCD_REG_COMMAND, 0};

    // DDRAM addresses for line 1: 00 - 39
    // DDRAM addresses for line 2: 40 - 79
    cmds[1] = LCD_CMD_DDRAM_ADDR_SET | (line ? 40 : 0);
    i2cLcdWriteBlock(cmds, 2);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Print plain text on LCD display into current position.
//!
//! @param[in]  text - string to print (length is limited by 16 visible and 24 hidden characters)
//-------------------------------------------------------------------------------------------------

static void PrintString(char *str)
{
    int_t i;
    uint8_t ustr[LCD_TOTAL_COLS + 2]; // +2 reserves space for a command and 0 termination
    int_t len;

    len = strlen(str);
    if (len >= LCD_TOTAL_COLS)
    {
        len = LCD_TOTAL_COLS ;
    }

    ustr[0] = LCD_REG_DATA;

    i = 1;
    while ((ustr[i++] = *str++)); // copy string

    i2cLcdWriteBlock(ustr, len + 1);
}

//-------------------------------------------------------------------------------
//
//  API functions
//
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//! @brief      Initialization of LCD display.
//!
//!             Must be called before using any other LCD API functions.
//!
//! @param[in]  i2cBusIndex - selection of I2C bus by index
//!
//! @return     success flag.
//-------------------------------------------------------------------------------------------------

bool_t SiiPlatformLcdInit(int i2cBusIndex)
{
    bool_t success = true;

    SiiLcdCmdTrain_t initSet[] = {
            {LCD_REG_COMMAND,0},
            {LCD_CMD_FUNC | BIT_TWO_LINES,1},
            {LCD_CMD_FUNC | BIT_TWO_LINES | BIT_INSTR_TAB1_SELECT,1},
            {LCD_FUNC1_INT_OSC,1},
            {LCD_FUNC1_CONTRAST,1},
            {LCD_FUNC1_PWR_CTRL,1},
            {LCD_FUNC1_FLWR_CTRL,250},
            {LCD_CMD_DISP_ON,1},
            {LCD_CMD_CLEAR,1},
            {LCD_CMD_ENTRY_MODE,10}
    };

    gI2cBusIndex = i2cBusIndex;

    SiiOsTimerWait(50);
    success = i2cLcdWriteCmdTrain(initSet, sizeof(initSet)/sizeof(SiiLcdCmdTrain_t));

    return success;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Clear all text on LCD display.
//-------------------------------------------------------------------------------------------------

void SiiPlatformLcdClear(void)
{
    SiiLcdCmdTrain_t clearSet[] =  {{LCD_REG_COMMAND, 0},
                                    {LCD_CMD_CLEAR,1},
                                    {LCD_CMD_ENTRY_MODE,1}};
    i2cLcdWriteCmdTrain(clearSet, sizeof(clearSet)/sizeof(SiiLcdCmdTrain_t));
}

//-------------------------------------------------------------------------------------------------
//! @brief      Print plain text on LCD display.
//!
//! @param[in]  text - string to print (length is limited by 16 characters)
//! @param[in]  line - line index on LCD screen (0 or 1)
//-------------------------------------------------------------------------------------------------

void SiiPlatformLcdPrint(char *text, int_t line)
{
    SetLine(line);
    PrintString(text);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Print text on LCD display using C Printf() formatting.
//!
//!             NOTE: Total length of a string must be limited by 40 characters
//!
//! @param[in]  line         - line index on LCD screen (0 or 1)
//! @param[in]  formatString - Printf() style formatting string
//! @param[in]  ...          - the rest of arguments referred to in the format string
//-------------------------------------------------------------------------------------------------

void SiiPlatformLcdPrintf(int_t line, const char *formatString,...)
{
    char            outString[2 * LCD_TOTAL_COLS]; // double buffer for protection
    unsigned long   ulCount;
    va_list         args;

    va_start(args, formatString);
    ulCount = ___sPrintf( outString, formatString, args );
    va_end(args);

    SiiPlatformLcdPrint(outString, line);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Shifts text on display left or right.
//!
//!             Text in both lines move together.
//!
//! @param[in]  dir - shift direction (negative: left, positive: right)
//-------------------------------------------------------------------------------------------------

void SiiPlatformLcdMoveText(int_t dir)
{
    uint8_t cmds[] = {
            LCD_REG_COMMAND,
            LCD_CMD_FUNC | BIT_TWO_LINES,
            LCD_FUNC0_DISP_SHFT | BIT_SET_SCREEN};

    if (dir > 0)
    {
        cmds[2] |= BIT_MOVE_RIGHT;
    }
    i2cLcdWriteBlock(cmds, 3);
}


//TODO: implement
// 1. Cursor on/off, position, move
// 2. 1/2 lines, double height font
// 3. Set character generator (8 characters in CGRAM), icons from ICON RAM

