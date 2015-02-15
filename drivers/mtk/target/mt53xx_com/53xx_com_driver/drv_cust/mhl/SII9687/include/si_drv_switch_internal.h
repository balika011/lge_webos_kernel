//***************************************************************************
//!file     si_drv_switch_internal.h
//!brief    SiI9687 Port Switch driver functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#ifndef __SI_DRVSWITCH_INTERNAL_H__
#define __SI_DRVSWITCH_INTERNAL_H__

#include "si_drv_switch.h"

//------------------------------------------------------------------------------
//  Manifest Constants
//------------------------------------------------------------------------------

typedef enum _SiiPortType_t
{
    SiiPortType_HDMI,
    SiiPortType_MHL,
} SiiPortType_t;


//------------------------------------------------------------------------------
//  Switch Driver Instance Data
//------------------------------------------------------------------------------

typedef struct
{
    int             structVersion;
    uint_t          instanceIndex;
    int             lastResultCode;         // Contains the result of the last API function called
    SwitchStatusFlags_t statusFlags;

    uint8_t         userPortSelect;
    bool_t          mpCableIn;
    bool_t          firstPass;              // 1 == 'First time HPE' has not run yet.

    int_t           arcPortIndex;
    int_t           mhlPortIndex;
    SiiPortType_t   portType[SII_INPUT_PORT_COUNT];         // Port type info for each port

    uint8_t         mpInfoFrameMask;                        // Determines which infoframe types will be
                                                            // monitored for change on the main pipe.
    uint8_t         mpInfoFrameInterruptStatus;

    bool_t          (*pHpdSetStateMhl)( uint_t, uint_t );   // MHL callback function pointer.
    bool_t          (*pHpdSetStateCbus)( uint_t, uint_t );  // CBUS callback function pointer.
    void            (*pCallbackStatusChange)( uint16_t );   // Status Change callback function pointer
}   SwitchDrvInstanceData_t;

extern SwitchDrvInstanceData_t drvSwitchInstance[SII_NUM_SWITCH];
extern SwitchDrvInstanceData_t *pDrvSwitch;

//-------------------------------------------------------------------------------
//  Driver Function Prototypes
//-------------------------------------------------------------------------------

enum
{
    SiiTERM_HDMI                = 0x00,     // Enable for HDMI mode
    SiiTERM_MHL                 = 0x55,     // Enable for MHL mode
    SiiTERM_DISABLE             = 0xFF,     // Disable
};

//------------------------------------------------------------------------------
// Function:    SiiDrvSwitchHpdSetState
// Description: Set the correct HPD pin state for HDMI 1.3/a operation mode.
//              (Switch only, no MHL or CDC)
// Parameter:   port:       HDMI Port to control
//              newState:   Requested new state for HPD
//
//                  SiiHPD_ACTIVE       - HPD HI, HDCP, EDID, RX Term enabled
//                  SiiHPD_INACTIVE     - HPD LOW, HDCP, RX Term disabled
//                  SiiHPD_ACTIVE_EX    - EDID, RX Term enabled
//                  SiiHPD_INACTIVE_EX  - HPD HI, HDCP, EDID, RX Term disabled
//
//                  RX term enabled is HDMI termination.
//
// Returns:     TRUE if change to new state was successful.
//              FALSE otherwise.
//------------------------------------------------------------------------------

bool_t SiiDrvSwitchHpdSetState ( uint_t portIndex, uint_t newState );



//------------------------------------------------------------------------------
//  InfoFrame-Specific functions
//------------------------------------------------------------------------------

void    SwitchInfoframeProcessInterrupts( void );
uint8_t SiiVsifPacketHasNewData(  uint8_t packetChn, uint8_t packetType ); //TODO: empty prototype


#endif  // __SI_DRVSWITCH_INTERNAL_H__



