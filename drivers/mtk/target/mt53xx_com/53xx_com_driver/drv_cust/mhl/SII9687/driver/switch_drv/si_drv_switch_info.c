//**************************************************************************
//! @file     si_drv_switch_info.c
//! @brief    SiI9687 Switch Driver input video and Infoframe information API.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "si_drv_device.h"
#include "si_drv_internal.h"
#include "si_drv_switch.h"
#include "si_drv_switch_internal.h"
#include "si_video_tables.h"

//-------------------------------------------------------------------------------------------------
//! @brief      Return InfoFrame captured in an input port.
//!
//! @param[in]  port        - port selection
//! @param[in]  ifType      - info frame type
//! @param[out] pInfoFrame  - pointer to the info frame data
//! @param[out] plen        - pointer to a length of info frame data in bytes
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvSwitchInfoFrameGet(int_t port, SiiPacketType_t ifType, uint8_t *pInfoFrame, int_t *pLen)
{
    pDrvSwitch->lastResultCode = SII_SUCCESS;

    // Select main or sub pipe to be source of the info frame data
    SiiRegModify( REG_PORT_INFOFRAME_SELECT, VAL_RD_PORT_SEL_MASK, port );

    // Read requested IF data
    switch (ifType)
    {
        case SiiPACKET_TYPE_AVI:
            SiiRegReadBlock( REG_SP_AVI_HEADER, pInfoFrame, 18 );
            *pLen = pInfoFrame[2] + 3;
            break;
        case SiiPACKET_TYPE_AIF:
            *pLen = 31;
            SiiRegReadBlock( REG_SP_AIF_HEADER, pInfoFrame, 32 );
            *pLen = pInfoFrame[2] + 3;
            break;
        default:
            pInfoFrame = 0;
            pDrvSwitch->lastResultCode = SII_ERR_INVALID_PARAMETER;
            return (false);
    }

    return (true);
}


//-------------------------------------------------------------------------------------------------
//! @brief      Returns the resolution info for the requested port, if the port
//!             is connected to an active source.  For DVI modes, the alternate
//!             resolution detection hardware is used, for HDMI, the AVI VIC
//!             data is used.
//! @param[in]  port             - index of Rx port
//! @param[in]  isMainPipe       - true if port is selected to MP
//! @param[in]  pVideoFormat     - pointer to a video format structure
//!
//! @return     true if detection was successful
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvSwitchPortInfoGet ( uint8_t port, bool_t isMainPipe, SiiVideoFormat_t *pVideoFormat)
{
    bool_t  success = false;
    uint8_t portInfo[18];
    uint8_t vic;
    uint16_t hBlank, vBlank;
    const videoMode_t *pVideoTabMode = 0;
    const videoMode_t *pVideoTabModeBasedOnAvi = 0;

    if ( port >= SII_INPUT_PORT_COUNT )
    {
        pDrvSwitch->lastResultCode = SII_ERR_INVALID_PARAMETER;
        return( false );
    }
    pDrvSwitch->lastResultCode = SII_SUCCESS;

    // Get raw data for the port.
    SiiRegModify( REG_PORT_INFOFRAME_SELECT, VAL_RD_PORT_SEL_MASK, port );

    pVideoFormat->isHdmi = ((SiiRegRead( REG_PORT_ALTRES_MISC ) & BIT_IS_HDMI) != 0);
    pVideoFormat->isMhl  = ((SiiRegRead(REG_STATE) & BIT_MHL) == BIT_MHL);

    // Try to get the resolution info from the alternate resolution detection hardware.
    SiiRegReadBlock( isMainPipe ? REG_MP_PORT_ALTRES_DATA_0 : REG_RP_PORT_ALTRES_DATA_0, portInfo, 8 );
    if (((portInfo[7] & BIT_ALT_RES_DATA_VALID) != 0))
    {
        pVideoFormat->isInterlace  = ((portInfo[7] & BIT_IS_INTERLACED) != 0);
        pVideoFormat->hRes         = portInfo[0] | ((int_t)portInfo[1] << 8);
        hBlank                     = (portInfo[2] | ((int_t)portInfo[3] << 8));
        pVideoFormat->vRes         = portInfo[4] | ((int_t)portInfo[5] << 8);
        vBlank                     = portInfo[6];

        // VIC was not valid, try to find a match for detected resolution
        pVideoTabMode = SiiVideoTableMatchFind(pVideoFormat->hRes, hBlank, pVideoFormat->vRes, vBlank, pVideoFormat->isInterlace);
        pVideoFormat->vic          = pVideoTabMode ? pVideoTabMode->Vic4x3 : 0;

        // Defaults
        pVideoFormat->vRate        = 0; // unknown
        pVideoFormat->tmdsClock    = 0;
        pVideoFormat->pRepl        = 1;

        // Resolution information is enough for at least partial success when not all other data is found
        success = true;
    }

    // Extract the VIC information from the
    // AVI InfoFrame stored for the requested port.
    SiiRegReadBlock(REG_SP_AVI_HEADER, portInfo, 18);

    if (((portInfo[REG_SP_AVI_VALID - REG_SP_AVI_HEADER] & BIT_SP_AVI_DATA_VALID) != 0))
    {
        vic = portInfo[ REG_SP_AVI_PB4 - REG_SP_AVI_HEADER] & 0x7F;
        pVideoFormat->vic = vic;
        pVideoFormat->pRepl = (portInfo[REG_SP_AVI_PB5 - REG_SP_AVI_HEADER] & 0x0F) + 1; // pixel repetition factor

        // Set pointer to video mode in the table defined by AVI VIC
        if ((vic > 0) && (vic <= LAST_KNOWN_CEA_VIC))
        {
            pVideoTabModeBasedOnAvi = &VideoModeTable[ ceaVicToVideoTableIndex[vic] ];
        }

        // Use AVI VIC only if h/w resolution detection method failed
        // or when detected resolution matches the one from AVI
        if(pVideoTabModeBasedOnAvi)
        {
            if (!pVideoTabMode ||
                 ((pVideoTabModeBasedOnAvi->Active.H == pVideoFormat->hRes) &&
                  (pVideoTabModeBasedOnAvi->Active.V == pVideoFormat->vRes) &&
                  (pVideoTabModeBasedOnAvi->Interlaced == pVideoFormat->isInterlace)) )
            {
                pVideoTabMode = pVideoTabModeBasedOnAvi;
            }
        }
    }

    // Extract GCP:Color Depth info
    pVideoFormat->colorDepth = SiiRegRead(isMainPipe ? REG_MP_COLOR_DEPTH : REG_RP_COLOR_DEPTH) & MSK_COLOR_DEPTH;

    if(pVideoTabMode)
    {
        pVideoFormat->hRes         = pVideoTabMode->Active.H;
        pVideoFormat->vRes         = pVideoTabMode->Active.V;
        pVideoFormat->vRate        = pVideoTabMode->VFreq;
        pVideoFormat->isInterlace  = pVideoTabMode->Interlaced;
        pVideoFormat->tmdsClock    = pVideoTabMode->PixClk;
        // Multiply pixel clock value to a factor 5/4 for CD 30bit, 3/2 for CD 36bit and 2 for CD 48bit
        switch (pVideoFormat->colorDepth)
        {
            case SiiCOLORDEPTH_10: // 10 bit/col
                pVideoFormat->tmdsClock = (pVideoFormat->tmdsClock * 5) / 4;
                break;
            case SiiCOLORDEPTH_12: // 12 bit/col
                pVideoFormat->tmdsClock = (pVideoFormat->tmdsClock * 3) / 2;
                break;
            case SiiCOLORDEPTH_16: // 16 bit/col
                pVideoFormat->tmdsClock *= 2;
                break;
        }
        // protect against too high TMDS clock detection (> 300 MHz)
        success = (pVideoFormat->tmdsClock <= 30000);
    }

    // PackedPixel mode detection
    if (pVideoFormat->isMhl)
    {
        // PackedPixel formats 16,31
        if ((pVideoFormat->vic == 16) || (pVideoFormat->vic == 31))
        {
            // TMDS clock is 1/2 of corresponding HDMI clock
            pVideoFormat->tmdsClock /= 2;
        }
    }

    if (!success)
    {
        // Defaults are the last resort
        pVideoFormat->isInterlace  = false;
        pVideoFormat->hRes         = 0;
        pVideoFormat->vRes         = 0;
        pVideoFormat->vRate        = 0;
        pVideoFormat->colorDepth   = 0;
        pVideoFormat->tmdsClock    = 0;
        pVideoFormat->vic          = 0;
        pVideoFormat->pRepl        = 1;
    }

    return( success );
}


//-------------------------------------------------------------------------------------------------
//! @brief      Processes InfoFrame change interrupts from the port attached to
//!             the main pipe.
//!             Function checks only the main pipe interrupts so that real-time
//!             notification of changes to the active port is available.
//!             The current status of any of the background ports can be
//!             read from the hardware as necessary.
//-------------------------------------------------------------------------------------------------

void SwitchInfoframeProcessInterrupts ( void )
{
    uint8_t intStatus;

    // Check main pipe InfoFrame status. If the status of an InfoFrame type
    // that we are interested in is set, go ahead and process it.

    intStatus = SiiRegRead( REG_PA_INTR6 ) & VAL_MP_IF_STATUS_MASK;
    SiiRegWrite( REG_PA_INTR6, intStatus );      // Clear all status flags for MP.
    intStatus >>= 2;                             // Main pipe bits are MSBs
    intStatus &= pDrvSwitch->mpInfoFrameMask;
    if ( intStatus )
    {
        pDrvSwitch->statusFlags |= SiiSWITCH_MP_INFOFRAME_INTR_VALID | SiiSWITCH_INT;
        pDrvSwitch->mpInfoFrameInterruptStatus = intStatus;
    }
}


