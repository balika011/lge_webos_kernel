//***************************************************************************
//! @file     si_drv_tpg.c
//! @brief    Video Generator driver.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2002-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_drv_device.h"
#include "si_regs_tpg9687.h"
#include "si_drv_internal.h"
#include "si_drv_tpg.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Set format (resolution and frame rate) of generated video.
//!
//! @param[in]  format - format ID. @see tpgVideoFormat_t.
//! @param[in]  clockSrc  - video clock source.
//-------------------------------------------------------------------------------------------------

void SiiDrvTpgVideoFormatSet(tpgVideoFormat_t format, tpgClockSource_t clockSrc)
{
    SiiRegModify( REG_VPG_CTRL_3, MSK_VPG_FORMAT | MSK_VPG_CLK_SEL,
                 (clockSrc << 1) | (format << VAL_VPG_FORMAT_SHIFT));

    // SWWA: 24103
    // Power down selected Rx core clock channel to eliminate its influence to VPG clock
    SiiRegBitsSet(REG_CH0_PD_SYS3, MSK_CH0_PD_TMDS_CLK_EN, true);
    if (clockSrc == SI_TPG_CLK_INTERNAL)
    {
        uint8_t portIndex;
        portIndex = SiiRegRead( REG_RX_PORT_SEL ) & MSK_PORT_ALL;
        SiiRegBitsSet(REG_CH0_PD_SYS3, (1 << portIndex) & MSK_CH0_PD_TMDS_CLK_EN, false);
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set video test pattern to be generated.
//!
//! @param[in]  pattern - pattern ID. @see tpgVideoPattern_t.
//-------------------------------------------------------------------------------------------------

void SiiDrvExtTpgPatternSet(tpgVideoPattern_t pattern)
{
    pattern <<= VAL_VPG_PATTERN_SHIFT;
    SiiRegModify( REG_VPG_CTRL_2, MSK_VPG_PATTERN, pattern );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable test video pattern generation.
//!
//!             The pattern is NTSC color bars.
//!
//! @param[in]  isEnabled - enable flag.
//-------------------------------------------------------------------------------------------------

void SiiDrvTpgEnable(bool_t isEnabled)
{
    if (isEnabled)
    {
        Printf("\r\nTPG Enabled\n");
        SiiRegBitsSet(REG_VPG_CTRL_3, BIT_VPG_EN, true );
    }
    else
    {
        Printf("\r\nTPG Disabled\n");
        SiiRegBitsSet(REG_VPG_CTRL_3, BIT_VPG_EN, false );
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Enable or disable extended test video pattern generation.
//!
//!             The pattern is selectable by SiiDrvExtTpgPatternSet() function.
//!
//! @param[in]  isEnabled - enable flag.
//-------------------------------------------------------------------------------------------------

void SiiDrvExtTpgEnable(bool_t isEnabled)
{
    if (isEnabled)
    {
        // Enable internal video generator
        SiiRegBitsSet(REG_VPG_CTRL_3, BIT_VPG_EN, false);
        SiiRegBitsSet(REG_VPG_CTRL_3, BIT_VPG_TMDS_BIST_EN, true); // Select VPG to be TX0 source
        SiiRegBitsSet(REG_VPG_CTRL_1, BIT_VPG_EXT_EN, true);
        Printf("\r\nETPG Enabled\n");
    }
    else
    {
        // Disable internal video generator
        SiiRegBitsSet(REG_CH0_PD_SYS3, MSK_CH0_PD_TMDS_CLK_EN, true); // SWWA: 24103
        SiiRegBitsSet(REG_VPG_CTRL_3, BIT_VPG_TMDS_BIST_EN, false); // Select MP to be TX0 source
        SiiRegBitsSet(REG_VPG_CTRL_1, BIT_VPG_EXT_EN, false);
        Printf("\r\nETPG Disabled\n");
    }
}

