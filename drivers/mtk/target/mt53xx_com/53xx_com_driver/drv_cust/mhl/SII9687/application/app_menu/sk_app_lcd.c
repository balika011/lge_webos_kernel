//***************************************************************************
//! @file     sk_app_lcd.c
//! @brief    Text based LCD menu
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"

static char **pMenuLines;
static int_t  menuLen;
static int_t  curLine = 0;
static bool_t selPos = 0; // position of selected item on LCD screen (0, 1)

//-------------------------------------------------------------------------------------------------
//! @brief      Displays lines of menu and current selector.
//-------------------------------------------------------------------------------------------------

static void UpdateMenuView(void)
{
    SI_LCD_CLEAR();
    if (selPos == 0) // top selector
    {
   //     SI_LCD_PRINTF(0, ">%s", pMenuLines[curLine]);
        if ((curLine + 1) < menuLen)
        {
     //       SI_LCD_PRINTF(1, " %s", pMenuLines[curLine + 1]);
        }
    }
    else // bottom selector
    {
        if (curLine > 0)
        {
  //          SI_LCD_PRINTF(0, " %s", pMenuLines[curLine - 1]);
        }
  //      SI_LCD_PRINTF(1, ">%s", pMenuLines[curLine]);
    }

}


//-------------------------------------------------------------------------------------------------
//! @brief      Set a list of text options for a menu and show it.
//!
//! @param[in]  pOptionsList - pointer to array of strings
//! @param[in]  len          - total number of options
//-------------------------------------------------------------------------------------------------

void SkAppLcdMenuListSet(char **pOptionsList, int_t len)
{
    pMenuLines = pOptionsList;
    menuLen = len;

    // reset position pointers
    curLine = 0;
    selPos = 0;

    // show menu
    UpdateMenuView();
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set selected menu item and updates the screen.
//!
//! @param[in]  line - menu item index
//-------------------------------------------------------------------------------------------------

void SkAppLcdMenuLineSelect(int_t line)
{
    if (line == curLine)
    {
        return; // no change needed
    }

    if (selPos == 0) // top line
    {
        if (line > curLine) // going down
        {
            selPos = 1; // just move the selector
        }
        // when going up, keep the top position of the selector
    }
    else // bottom line
    {
        if (line < curLine) // going up
        {
            selPos = 0; // just move the selector
        }
        // when going down, keep the top position of the selector
    }
    curLine = line;
    UpdateMenuView();
}

//-------------------------------------------------------------------------------------------------
//! @brief      Move selection up or down and update the screen.
//!
//! @param[in]  isUp - true for moving Up and false for moving Down
//-------------------------------------------------------------------------------------------------

void SkAppLcdMenuSelectionMove(bool_t isUp)
{
    if (isUp)
    {
        if (curLine > 0)
        {
            SkAppLcdMenuLineSelect(curLine - 1);
        }
    }
    else
    {
        if (curLine < (menuLen - 1))
        {
            SkAppLcdMenuLineSelect(curLine + 1);
        }
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Return currently selected line index.
//-------------------------------------------------------------------------------------------------

int_t SkAppLcdMenuSelectionGet(void)
{
    return curLine;
}


//-------------------------------------------------------------------------------------------------
//! @brief      Prints one or two line of text.
//!
//! @param[in]  pLine1/2 - pointer to text, NULL if line is empty
//-------------------------------------------------------------------------------------------------

void SkAppLcdMessageBox(char *pLine1, char *pLine2)
{
    SI_LCD_CLEAR();
    if (pLine1)
    {
 //       SI_LCD_PRINTF(0, pLine1);
    }
    if(pLine2)
    {
  //      SI_LCD_PRINTF(1, pLine2);
    }
}


