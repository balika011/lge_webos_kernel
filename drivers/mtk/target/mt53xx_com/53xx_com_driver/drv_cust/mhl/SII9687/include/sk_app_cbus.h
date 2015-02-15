//***************************************************************************
//!file     sk_app_cbus.h
//!brief    Wraps board and device functions for the CBUS component
//          and the application
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/
#if !defined __SKAPP_CBUS_H__
#define __SKAPP_CBUS_H__

#include <linux/string.h>
#include "sk_application.h"
#include "si_cbus_component.h"
#include "si_drv_cbus.h"
#include "si_drv_switch.h"

#if 0
#include "../sk_application.h"
#include "../../component/cbus/si_cbus_component.h"
#include "../../driver/cbus_drv/si_drv_cbus.h"
#include "../../driver/switch_drv/si_drv_switch.h"
#endif


//------------------------------------------------------------------------------
// Module variables
//------------------------------------------------------------------------------

typedef struct
{
    uint8_t rcpKeyCode;          //!< RCP CBUS Key Code
    uint8_t rc5KeyCode;          //!< RC5 Key Code
} SiiRcpTranslate_t;


#define CBUS_SRC_POLLING_W1				  20
#define CBUS_SRC_WP_WIDTH1				  CBUS_SRC_POLLING_W1
#define CBUS_SRC_WP_WIDTH1_MIN        	((CBUS_SRC_WP_WIDTH1 - (CBUS_SRC_WP_WIDTH1 / 20)) - 2) // 5% tolerance
#define CBUS_SRC_WP_WIDTH1_MAX        	((CBUS_SRC_WP_WIDTH1 + (CBUS_SRC_WP_WIDTH1 / 20)) + 2) // 5% tolerance

#define CBUS_SRC_POLLING_W2				  60
#define CBUS_SRC_WP_WIDTH2				  CBUS_SRC_POLLING_W2
#define CBUS_SRC_WP_WIDTH2_MIN        	((CBUS_SRC_WP_WIDTH2 - (CBUS_SRC_WP_WIDTH2 / 20)) - 2) // 5% tolerance
#define CBUS_SRC_WP_WIDTH2_MAX        	((CBUS_SRC_WP_WIDTH2 + (CBUS_SRC_WP_WIDTH2 / 20)) + 2) // 5% tolerance

#define CBUS_SRC_WP_DISCOVER_MIN			100
#define CBUS_SRC_WP_DISCOVER_MAX			1000

#define CBUS_SRC_WP_POLARITY				1
#define CBUS_SRC_PULSE_MAX					6

#define CBUS_UCP_ASCII_LIMIT				0x7F

enum
{
	CBUS_SRC_WAKE_PULSE,
	CBUS_SRC_DISCOVERY_PULSE,
};

typedef struct _SiiCbusWakeUpData_t
{
    uint8_t cbusPin;
    uint16_t cbusTimeCount;
    uint16_t cbusMax;
    uint16_t cbusMin;
    uint8_t cbusPulse;
    uint8_t cbusState;
} SiiCbusWakeUpData_t;

typedef struct _SiiAppCbusInstanceData_t
{
	int         		instanceIndex;
    bool_t              cbusInterrupt;              // CBUS interrupt flag
    bool_t              cbusEnable;                 // true == CBusEnable
    bool_t              cbusPower;                  // true == cbusPower
    bool_t              oldCbusEnable;              // true == CBusEnable
    bool_t              wakeDetect;
    bool_t              cbusCableSense;             // true == cbus cable detected.
    uint8_t             busConnected;
    uint8_t             port;
    uint8_t             supportMask;                // LD_xx support bits
    SiiCbusWakeUpData_t cbusWakeup;
    SiiTimer_t          wupTimer;
    SiiTimer_t          cbusTimer;
}   SiiAppCbusInstanceData_t;

uint8_t CbusConvertToRcp ( uint8_t keyData );

bool_t  SkAppDeviceInitCbus( void );
void    SkAppTaskCbus( uint_t inst );
bool_t  SkAppCbusSendRcpMessage ( uint8_t keyCode );
bool_t  SkAppCbusSendRapMessage ( uint8_t actCode );
bool_t  SkAppCbusSendUcpMessage ( uint8_t ucpData );
void    SkAppCbusChannelEnable( bool_t isEnable );
//bool_t  SkAppCbusWakeupSequence( uint8_t pwrState);
bool_t  SkAppCbusResume( void );
bool_t  SkAppCbusStandby ( void );
bool_t  SkAppCbusRemoteControlHandler( uint8_t keyCode );

bool_t  SkAppCbusDemoMenu( RC5Commands_t key );

//-------------------------------------------------------------------------------
//  Cbus Wake Sequence start from here
//-------------------------------------------------------------------------------
enum
{
	CBUS_STATE_IDLE,
	CBUS_STATE_LOW,
	CBUS_STATE_HIGH,
};

#define WAKE_BIT_CLOCKS           (100)     // CBus in ms
#define WAKE_MIN_BITPERIOD        (WAKE_BIT_CLOCKS - (WAKE_BIT_CLOCKS / 10))  // 10% tolerance
#define WAKE_MAX_BITPERIOD        (WAKE_BIT_CLOCKS + (WAKE_BIT_CLOCKS / 10))  // 10% tolerance

//-------------------------------------------------------------------------------
//  Cbus Wake Sequence Constants
//-------------------------------------------------------------------------------

#if (CBUS_WAKE_SEQUENCE == 1)
#define WAKE_TIMER_BASE       TIMER3_BASE
#define WAKE_PERIPH_TIMER     SYSCTL_PERIPH_TIMER3
#define WAKE_GPIO_PIN         GPIO_PIN_2
#define WAKE_GPIO_CCP         GPIO_PD2_CCP6
#define WAKE_INT_TIMERA       INT_TIMER3A
#define WAKE_INT_TIMERB       INT_TIMER3B
#else
#define WAKE_TIMER_BASE       TIMER1_BASE
#define WAKE_PERIPH_TIMER     SYSCTL_PERIPH_TIMER1
#define WAKE_GPIO_PIN         GPIO_PIN_1
#define WAKE_GPIO_CCP         GPIO_PD1_CCP2
#define WAKE_INT_TIMERA       INT_TIMER1A
#define WAKE_INT_TIMERB       INT_TIMER1B
#endif

//-------------------------------------------------------------------------------
//  Cbus Wake Sequence Interface functions
//-------------------------------------------------------------------------------

// Cbus Wake Sequence decode state machine states

typedef enum
{
    WAKESTATE_DETECTED,         // Waiting for a rising edge on the IR sensor
    WAKESTATE_WAIT_FIRST_EDGE,  // Waiting for a rising edge on the IR sensor
    WAKESTATE_WAIT_SECOND_EDGE, // Sampling the remaining bits in the command
} wakeDecodeStates_t;

typedef enum
{
    PULSE_NOT_DETECTED,        // Waiting for a rising edge on the IR sensor
    PULSE_DETECTED,            // Sampling the remaining bits in the command
} wakeDecodeDetect_t;

//-------------------------------------------------------------------------------
//  Cbus Wake Sequence Interface functions
//-------------------------------------------------------------------------------

void    SkCbusWakeSequenceInit (void);
uint8_t SkCbusWakeGetStatus ( void );
bool_t  SkAppCbusInstanceSet ( uint_t instanceIndex );
int SkAppCbusInstanceGet (void);


extern SiiAppCbusInstanceData_t *pAppCbus;
#endif  //__SKAPP_CBUS_H__
