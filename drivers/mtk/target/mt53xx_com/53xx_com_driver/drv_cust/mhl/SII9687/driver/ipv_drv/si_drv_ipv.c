//**************************************************************************
//!file     si_drv_ipv.c
//!brief    SiI9687 InstaPrevue driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include <linux/string.h>
#include "si_osal_timer.h"
#include "si_scheduler.h"
#include "si_ipv_component.h"
#include "si_drv_ipv_internal.h"
#include "si_drv_device.h"
#include "si_regs_ipv9687.h"
#include "si_cra.h"

//------------------------------------------------------------------------------
//  Driver Data
//------------------------------------------------------------------------------

IpvDrvInstanceData_t drvIpvInstance[SII_NUM_IPV];
IpvDrvInstanceData_t *pDrvIpv = &drvIpvInstance[0];

//-------------------------------------------------------------------------------------------------
//! @brief      Initialize the IPV driver.
//!
//! @return     true - success  false - some failure occurred
//------------------------------------------------------------------------------
bool_t SiiDrvIpvInitialize ( void )
{
    memset( pDrvIpv, 0, sizeof( IpvDrvInstanceData_t ));    // Clear instance data

    SiiRegInstanceSet( PP_PAGE_0, 0 );        // Always use instance 0 for CRA
    SiiDrvIpvEnable( false );                 // Start disabled

    // Disable SF animation feature by default
    SiiDrvIpvAnimationEnable(false);
    // Set speed and acceleration for moves
    SiiDrvIpvAnimationConfig(5, 16, 5, 16);

    // Enable the interrupts for this driver
    SiiRegBitsSet( REG_PA_INTR9_MASK, BIT_MP_RES_STABLE_CHG, true );
    return( true );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Returns the result of the last call to a IPV driver function.
//!
//! @return     SiiDrvIpvResultCodes_t
//-------------------------------------------------------------------------------------------------
SiiDrvIpvResultCodes_t SiiDrvIpvGetLastResult( void )
{
    return( pDrvIpv->lastResultCode );
}

//-------------------------------------------------------------------------------------------------
//! @brief      Select the basic operating mode for InstaPrevue.
//!
//! @param[in]  mode -  SiiIpvModeALL:      Always display all 5 ports
//!                     SiiIpvModeACTIVE:   Display only active ports
//!                     SiiIpvModeSELECT:   Display only RP selected port (PIP mode)
//
//! @comment    IPV must be disabled when this function is called.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvModeSet ( SiiIpvMode_t mode )
{
    pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
    switch ( mode )
    {
        case SiiIpvModeALL:
        case SiiIpvModeACTIVE:
            pDrvIpv->ipvMode = mode;
            SiiRegWrite( REG_IPV_MODE, mode );
            SiiRegModify( REG_IP_CONFIG_43, BGND_DISABLE, CLEAR_BITS );
            break;
        case SiiIpvModeSELECT:
            pDrvIpv->ipvMode = SiiIpvModeSELECT;
            SiiRegWrite( REG_IPV_MODE, SiiIpvModeSELECT );
            // Disable background roving
            SiiRegModify( REG_IP_CONFIG_43, BGND_DISABLE, SET_BITS );
            break;
        default:
            pDrvIpv->lastResultCode = SII_DRV_IPV_ERR_INVALID_PARAMETER;
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Start or stop the IPV engine and display.
//!             When started, IPV display mixing is enabled. When stopped, the IPV
//!             display is removed from the main pipe display.  If an alpha blend
//!             step is specified in the _SiiIpvThemeData_t structure, each display
//!             window will fade in and out at the specified step rate.
//!
//! @param[in]  isEnable -  true:   display
//!                         false:  hide
//! @remark     Only windows in the enableMask are affected by this function.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvEnable ( bool_t isEnable )
{
    pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;

    pDrvIpv->isEnabled = isEnable;
    if ( isEnable )
    {
        SiiRegWrite( REG_IPV_BORDER_CTRL, BIT_DB_ALL );                                 // enable window borders
        SiiRegModify( REG_IPV_CTRL, IPV_RESTART_CTRL1 | IPV_RESTART_CTRL0, SET_BITS );  // (Re)Start down scaler
        SiiRegModify( REG_IPV_WIN_STATUS, BIT_FORCE_BLANK, CLEAR_BITS );                // Enable video to preview windows
        SiiRegWrite( REG_IPV_WIN_ENABLE, pDrvIpv->enableMask );                         // enable window display

        SiiRegWrite( REG_PAUTH_ENGHLIM2, 10 );
    }
    else
    {
        SiiRegWrite( REG_IPV_WIN_ENABLE, 0 );
        SiiRegModify( REG_IPV_WIN_STATUS, BIT_FORCE_BLANK, BIT_FORCE_BLANK );           // Disable video to preview windows - force to blank

        SiiRegWrite( REG_PAUTH_ENGHLIM2, 3 );
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Resets IPV downsampler logic.
//!
//!             This function helps IPV logic to recognize and apply changes in AVI InfoFrame
//!             when video resolution or video clock don't change.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvDownsamplerReset(void )
{
    SiiRegModify(REG_IPV_CTRL, IPV_RESTART_CTRL1, SET_BITS);
}


//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvEnableMaskSet ( uint_t enableMask )
{
    if ( enableMask & ~MSK_ENABLE_MASK )
    {
        pDrvIpv->lastResultCode = SII_DRV_IPV_ERR_INVALID_PARAMETER;
    }
    else
    {
        pDrvIpv->enableMask = enableMask;
        pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvBorderEnablesSet ( uint8_t borderEnableMask )
{
    if ( borderEnableMask & ~MSK_DB )
    {
        pDrvIpv->lastResultCode = SII_DRV_IPV_ERR_INVALID_PARAMETER;
    }
    else
    {
        pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
        SiiRegWrite( REG_IPV_BORDER_CTRL, borderEnableMask & MSK_DB );
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief      Enable/Disable InstaPrevue Animation feature.
//!
//! @param[in]  isEnabled
//-------------------------------------------------------------------------------------------------

void SiiDrvIpvAnimationEnable(bool_t isEnabled)
{
    pDrvIpv->isAnimationEnabled = isEnabled;
    SiiRegModify( REG_PV_LOC_CTRL, BIT_PV_SF_ANIM_ENABLE, isEnabled ? SET_BITS : CLEAR_BITS);
}

//-------------------------------------------------------------------------------------------------
//! @brief      Check if InstaPrevue Animation has fully completed, so that all sub-frames
//!             are in their target places.
//-------------------------------------------------------------------------------------------------

bool_t SiiDrvIpvAnimationIsComplete(void)
{
    if (pDrvIpv->isAnimationEnabled)
    {
        return SiiRegRead( REG_PV_LOC_CTRL) & BIT_PV_SF_MOVE_START;
    }
    else
    {
        return true;
    }
}


//-------------------------------------------------------------------------------------------------
//! @brief      Set speed and acceleration factors for IPV animation feature.
//!
//! @param[in]  xSpeed - initial speed in horizontal direction
//! @param[in]  xAccel - acceleration in horizontal direction
//! @param[in]  ySpeed - initial speed in vertical direction
//! @param[in]  yAccel - acceleration in vertical direction
//-------------------------------------------------------------------------------------------------

void SiiDrvIpvAnimationConfig(int_t xSpeed, int_t xAccel, int_t ySpeed, int_t yAccel)
{
    // Horizontal
    SiiRegWrite(REG_PV_LOC_CTRL_X_SP, xSpeed);
    SiiRegWrite(REG_PV_LOC_CTRL_X_AC, xAccel);
    // Vertical
    SiiRegWrite(REG_PV_LOC_CTRL_Y_SP, ySpeed);
    SiiRegWrite(REG_PV_LOC_CTRL_Y_AC, yAccel);
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//! @warning    Intended to be called from IPV component.  No parameter checking performed.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvWindowCoordSet ( int_t winIndex, uint16_t x, uint16_t y )
{
    winIndex *= 2;
    SiiRegWrite( REG_IPV_WIN_X0_L + winIndex, (uint8_t)(x >> 0) );
    SiiRegWrite( REG_IPV_WIN_X0_H + winIndex, (uint8_t)(x >> 8) );
    SiiRegWrite( REG_IPV_WIN_Y0_L + winIndex, (uint8_t)(y >> 0) );
    SiiRegWrite( REG_IPV_WIN_Y0_H + winIndex, (uint8_t)(y >> 8) );

    if (pDrvIpv->isAnimationEnabled)
    {
        // Start moving the subframe
        SiiRegModify( REG_PV_LOC_CTRL, BIT_PV_SF_MOVE_START, SET_BITS);
    }
    
    pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;    
}

//-------------------------------------------------------------------------------------------------
//! @brief      Set alpha blending level
//!
//! @param[in]  winIndex  -  window id
//! @param[in]  newAlpha  -  alpha level
//!
//! @warning    Intended to be called from IPV component.  No parameter checking performed.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvWindowAlphaSet ( int_t winIndex, uint8_t newAlpha )
{
    int i;

    if ( winIndex == -1 )
    {
        for ( i = 0; i < IPV_WINDOW_COUNT; i++ )
        {
            SiiRegWrite( REG_IPV_ALPHA_0 + i, newAlpha );
        }
    }
    else
    {
        SiiRegWrite( REG_IPV_ALPHA_0 + winIndex, newAlpha );
    }

    pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//! @warning    Intended to be called from IPV component.  No parameter checking is performed.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvBoundaryColorTableSet( SiiIpvBorderColors_t tableIndex, uint8_t *pCrCbY )
{
    pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
    switch ( tableIndex )
    {
        case SiiIpvBorderColorALL:
        case SiiIpvBorderColorACTIVE:
        case SiiIpvBorderColorNONAuth:
        case SiiIpvBorderColorUNSUPPORTED:
            SiiRegWriteBlock( REG_IPV_ALL_BORDER_CR + (tableIndex * YCBCR_LEN), pCrCbY, YCBCR_LEN );
            break;

        case SiiIpvBorderColorUNSUPPORTEDBackground:
            SiiRegWriteBlock( REG_IPV_USF_BGD_CR, pCrCbY, YCBCR_LEN );
            break;

        default:
            pDrvIpv->lastResultCode = SII_DRV_IPV_ERR_INVALID_PARAMETER;
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//! @warning    Intended to be called from IPV component.  No parameter checking is performed.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvWindowAlphaTableSet( int_t firstWinIndex, uint8_t *pAlphaMax, uint8_t *pAlphaStep, int_t winCount )
{
    if (winCount <= (IPV_WINDOW_COUNT - firstWinIndex))
    {
        SiiRegWriteBlock( REG_IPV_ALPHA_0 + firstWinIndex, pAlphaMax, winCount );
        memcpy( &pDrvIpv->alphaStep[ firstWinIndex], pAlphaStep, winCount );
        pDrvIpv->lastResultCode = SII_DRV_IPV_SUCCESS;
    }
    else
    {
        pDrvIpv->lastResultCode = SII_DRV_IPV_ERR_INVALID_PARAMETER;
    }
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvWindowAlphaFade( bool_t isEnable )
{

}

//-------------------------------------------------------------------------------------------------
//! @brief      Select a preview window size set from one of the built-in sizes.
//!
//! @param[in]  newWinSize
//! @warning    Intended to be called from IPV component.  No parameter checking is performed.
//-------------------------------------------------------------------------------------------------
void SiiDrvIpvSizeSet( uint16_t *p16x9, uint16_t *p4x3, uint8_t *pHeight )
{
    int_t   i;

    for ( i = 0; i < IPV_WINDOW_COUNT; i++ )
    {
        SiiRegWrite( REG_IPV_1WIN_16x9_WIDTH + i, (uint8_t)(p16x9[i] >> 3 ));
        SiiRegWrite( REG_IPV_1WIN_4x3_WIDTH + i, (uint8_t)(p4x3[i] >> 3 ));

        SiiRegWrite( REG_IPV_1WIN_HEIGHT + i, (uint8_t )(pHeight[i] >> 2 ));
    }
    pDrvIpv->lastResultCode = SII_IPV_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
//! @brief      Test for pending interrupts and handle as necessary.
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvIpvHandleInterrupts( void )
{
    bool_t  resStableAndWasEnabled = false;
    uint8_t u8Data;

    if ( pDrvIpv->interruptPending )
    {
        // Determine new main port resolution stability state.
        u8Data = SiiRegRead( REG_MP_GCP_STATUS ) & BIT_MP_RES_STABLE;

        Printf("\r\nResolution Stable Change interrupt: %s\n", u8Data ? "STABLE" : "UNSTABLE" );
        if (( u8Data == 0 ) && pDrvIpv->isEnabled )
        {
            // Main port resolution is unstable, disable InstaPrevue windows and
            // clear the sub-frame valid flag.
            pDrvIpv->wasEnabled = true;
            SiiDrvIpvEnable( false );
        }
        else if ( pDrvIpv->wasEnabled )
        {
            pDrvIpv->wasEnabled = false;
            resStableAndWasEnabled = true;
        }

        pDrvIpv->interruptPending = false;
    }
    return( resStableAndWasEnabled );
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
bool_t SiiDrvIpvProcessInterrupts ( void )
{

    uint8_t intStatus;
   	Printf("\r\nSiiDrvIpvProcessInterrupts\n");


    intStatus = SiiRegRead( REG_PA_INTR9 ) & BIT_MP_RES_STABLE_CHG;
    if ( intStatus )
    {
        SiiRegWrite( REG_PA_INTR9, intStatus );
        pDrvIpv->interruptPending = true;
        SiiSchedSendSignal(SI_EVENT_ID_IPV_INT, 0);
    }

    return( pDrvIpv->interruptPending );
}

//-------------------------------------------------------------------------------------------------
//! @brief
//!
//! @param[in]
//-------------------------------------------------------------------------------------------------
int_t SiiDrvIpvActiveWindowsGet ( void )
{
    return( SiiRegRead( REG_IPV_WIN_STATUS ) & MSK_IPV_WIN_VALID );
}

