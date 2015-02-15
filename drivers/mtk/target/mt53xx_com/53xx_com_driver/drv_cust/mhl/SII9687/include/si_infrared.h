//***************************************************************************
//!file     si_infrared.h
//!brief    Platform Infrared driver, ARM (Stellaris) version.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if !defined __SI_INFRARED_H__
#define __SI_INFRARED_H__
#include "si_common.h"

//-------------------------------------------------------------------------------
//  IR Constants
//-------------------------------------------------------------------------------

//#define IR_BIT_CLOCKS           (29131)     // RC5 bit time in Arm Clocks (16.384MHz)
//#define IR_BIT_CLOCKS           (44450)     // RC5 bit time in Arm Clocks (50MHz with prescaler 1/2)
//#define IR_BIT_CLOCKS           (28448)     // RC5 bit time in Arm Clocks (16MHz)
#define IR_BIT_CLOCKS           (((gSystemClockHz / 1000) * 1778 /*us*/) / 2000)     // RC5 bit time in Arm Clocks (with prescaler 1/2)
//#define IR_MIN_BITPERIOD        (IR_BIT_CLOCKS - (IR_BIT_CLOCKS / 10))  // 10% tolerance
//#define IR_MAX_BITPERIOD        (IR_BIT_CLOCKS + (IR_BIT_CLOCKS / 10))  // 10% tolerance
#define IR_MIN_BITPERIOD        ((2 * (IR_BIT_CLOCKS - (IR_BIT_CLOCKS / 20))) & 0xFFFF)  // 5% tolerance
#define IR_MAX_BITPERIOD        ((2 * (IR_BIT_CLOCKS + (IR_BIT_CLOCKS / 20))) & 0xFFFF)  // 5% tolerance

#define IR_BIT_HALF_TOTAL       (12 * 2)                // Number of half-bits after first two start bits.

#define IR_RING_BUFFER_SIZE         5       // size of RC5 ring buffer.


#define IR_TIMER_BASE       TIMER3_BASE
#define IR_PERIPH_TIMER     SYSCTL_PERIPH_TIMER3
#define IR_GPIO_PIN         GPIO_PIN_2
#define IR_GPIO_CCP			GPIO_PD2_CCP6
#define IR_INT_TIMERA       INT_TIMER3A
#define IR_INT_TIMERB       INT_TIMER3B

//-------------------------------------------------------------------------------
//  IR Interface functions
//-------------------------------------------------------------------------------

// IR decode state machine states

typedef enum
{
    IRSTATE_WAIT_FIRST_EDGE,        // Waiting for a rising edge on the IR sensor
    IRSTATE_MEASURE_START_BIT,      // Measuring the width of the first bit
    IRSTATE_SAMPLE_BITS,            // Sampling the remaining bits in the command
} IrDecodeStates_t;

/** @brief RC5 infrared receive ring buffer status
*/
typedef enum
{
    RC5_BUFFER_EMPTY            = 0x8000,   // Buffer empty flag
    RC5_VALID_ENTRY             = 0x4000,   // Buffer entry is valid.
    RC5_ADDRESS_FIELD           = 0x0780,   // Address field in RC5 command
    RC5_COMMAND_FIELD           = 0x007F,   // Command field in RC5 command
    RC5_TOGGLE_FIELD            = 0x0800,   // Toggle bit in RC5 command

    RC5_KEY_MASK                = ( RC5_ADDRESS_FIELD | RC5_COMMAND_FIELD )
} RC5KeyFlags_t;


// The following codes are for Philips Universal IR Remote Control, code set: 0807
typedef enum
{
    RC5_0,
    RC5_1,
    RC5_2,
    RC5_3,
    RC5_4,
    RC5_5,
    RC5_6,
    RC5_7,
    RC5_8,
    RC5_9,
    RC5_RSVD1,                      // Documents are vague on this one
    RC5_RSVD2,                      // Documents are vague on this one
    RC5_STANDBY         = 0x0C,     //
    RC5_MUTE_OK         = 0x0D,     //
    RC5_PRESETS,
    RC5_SELECT          = 0x0F,     // Doc says RC5_DISPLAY
    RC5_VOL_UP          = 0x10,     //
    RC5_VOL_DOWN        = 0x11,     //
    RC5_BRIGHT_UP,
    RC5_BRIGHT_DOWN,
    RC5_COLOR_SAT_UP,
    RC5_COLOR_SAT_DOWN,
    RC5_BASS_UP,
    RC5_BASS_DOWN,
    RC5_TREBLE_UP,
    RC5_TREBLE_DOWN,
    RC5_ENTER,                      // RC5_BALANCE_RIGHT
    RC5_BALANCE_LEFT,
    RC5_CONTRAST_UP,
    RC5_CONTRAST_DOWN,
    RC5_SEARCH_UP,
    RC5_TINT_UP,
    RC5_PRG_UP          = 0x20,     // CHANNEL_UP
    RC5_PRG_DOWN        = 0x21,     // CHANNEL_DOWN
    RC5_ALT_CHANNEL     = 0x22,     // Used as previous channel

    RC5_PAUSE           = 0x29,     // Doc says RC5_VOTE (whatever that is!)
    RC5_MENU            = 0x2E,     //

//    RC5_PAUSE           = 0x30,     //

    RC5_INCREMENT       = 0x2B,
    RC5_DECREMENT       = 0x2C,
    RC5_OK              = 0x2D,

    RC5_ERASE,
    RC5_FAST_REW        = 0x32,
    RC5_GOTO,
    RC5_FAST_FWD        = 0x34,
    RC5_PLAY            = 0x35,
    RC5_STOP            = 0x36,
    RC5_TOGGLE          = 0x37,     // RC5_REC

    RC5_INPUT           = 0x38,     // Doc says RC5_EXT1
    RC5_EXT2,
    RC5_PIP_MODE,
    RC5_CC              = 0x3B,     // Doc says VIEW_DATA_MODE

    RC5_SYSTEM_STANDBY  = 0x3D,

    RC5_GUIDE           = 0x3E,

    RC5_CURSOR_STEP_UP      = 0x50,
    RC5_CURSOR_STEP_DOWN    = 0x51,
    RC5_CURSOR_STEP_LEFT    = 0x55,
    RC5_CURSOR_STEP_RIGHT   = 0x56,

    RC_POWER                = 0x00C,
    RC_MUTE                 = 0x00D,
    RC_INFO_SELECT          = 0x00F,    // Also Quit button
    RC_VOL_UP               = 0x010,
    RC_VOL_DOWN             = 0x011,

    RC5_UNKNOWN             = 0xFF,

    RC_ENTER                = 0x01A,

    RC_CURSOR_UP            = 0x01C,
    RC_CURSOR_DOWN          = 0x01D,
    RC_CH_PLUS              = 0x020,
    RC_CH_MINUS             = 0x021,
    RC_REPEAT_PREV_CHAN     = 0x022,

    RC_SLEEP                = 0x026,

    RC_CURSOR_RIGHT         = 0x02B,    // Also OK button
    RC_CURSOR_LEFT          = 0x02C,

    RC_MENU                 = 0x02E,

    RC_BLANK_TOGGLE         = 0x037,

    RC_TV_VCR_INPUT         = 0x038,
    RC_PIP                  = 0x03A,
    RC_CC_SUBTITLE          = 0x03B,

    RC_GUIDE                = 0x03E,

    RC_PAUSE                = 0x69,
    RC_FAST_REW_SCAN_MINUS  = 0x72,
    RC_FAST_FWD_SCAN_PLUS   = 0x74,
    RC_PLAY                 = 0x75,
    RC_STOP                 = 0x76,
    RC_REC                  = 0x77,

    RC_UNKNOWN              = 0x7FF,
} RC5Commands_t;

typedef enum
{
	RC5_ADDR_TV=0x00,
	RC5_ADDR_DVD=0x01,
	RC5_ADDR_VCR=0x02,
} RC5Address_t;

// ROOT_MENU
// EXIT
// ENTER
// CLEAR
// UP
// DOWN
// LEFT
// RIGHT
// SOUND_SELECT
// EJECT
// FORWARD
// BACKWARD
// PLAY FUNCTION
// PAUSE_PLAY FUNCTION
// RECORD FUNCTION
// PAUSE_RECORD FUNCTION
// STOP FUNCTION



//------------------------------------------------------------------------------
//  Infrared RC5 protocol sub-component instance data
//------------------------------------------------------------------------------

typedef struct
{
    bool_t      allowAutoRepeat;

    // Key ring buffer

    uint16_t    buffer[IR_RING_BUFFER_SIZE];
    uint8_t     first;
    uint8_t     last;
    uint8_t     overflow;
    uint16_t    lastKey;

    // Interrupt handler data

    uint_t      decodeState;
    uint_t      bitHalfCount;
    bool_t      firstHalfIsHigh;
    uint16_t    rcCode;
    uint_t      bitWidthMeasured;
}   InfraredInstanceData_t;

extern InfraredInstanceData_t irData;

//-------------------------------------------------------------------------------
//  IR Interface functions
//-------------------------------------------------------------------------------

void    SiiPlatformInfraRedInit (void);
uint_t  SiiPlatformInfraRedGetKey( void );

//-------------------------------------------------------------------------------
//  IR h/w interrupt handlers
//-------------------------------------------------------------------------------

void    SiiPlatformInfraRedEdgeTimerIntHandler ( void );
void    SiiPlatformInfraRedSamplingTimerIntHandler ( void );


#endif //__SI_INFRARED_H__
