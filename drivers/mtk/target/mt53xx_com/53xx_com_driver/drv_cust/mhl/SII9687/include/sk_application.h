//***************************************************************************
//! @file     sk_application.h
//! @brief    SiI9687 Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#if !defined __SK_APPLICATION_H__
#define __SK_APPLICATION_H__

#include "si_common.h"
#include "sk_configure.h"
#include "si_osal_timer.h"
#include "si_eeprom.h"
#include "si_gpio.h"
#include "si_platform.h"
#include "si_infrared.h"
#include "sk_app_cbus.h"
#include "si_edid_component.h"
#include "si_switch_component.h"
#include "si_drv_tpg.h"
#include "si_drv_device.h"
//#include "si_debugger_hdmigear.h"

#include "si_scheduler.h"

#include "x_printf.h"


#if 0
#include "si_common.h"
#include "sk_configure.h"
#include "../si_osal_timer.h"
#include "../platform/hal/eeprom/si_eeprom.h"
#include "../platform/board/si_gpio.h"
#include "../platform/board/si_platform.h"
#include "../platform/hal/ir_remote/si_infrared.h"
#include "./app_cbus/sk_app_cbus.h"
#include "../component/edid_inp_proc/si_edid_component.h"
#include "../component/switch/si_switch_component.h"
#include "../driver/tpg_drv/si_drv_tpg.h"
#include "../driver/sii9687_drv/si_drv_device.h"
//#include "si_debugger_hdmigear.h"

#include "./mini_sched/si_scheduler.h"
#endif

//------------------------------------------------------------------------------
//  Include app header files appropriate to this build
//------------------------------------------------------------------------------

#if INC_IPV
#include "si_ipv_component.h"
#include "sk_app_ipv.h"
#endif

#if 0//INC_LCD_DISP
#include "si_lcd_st7032.h"
#endif


//------------------------------------------------------------------------------
//
//  Application Constants
//
//------------------------------------------------------------------------------


//#define UI_POLLING_DELAY    100         // UI Loop Polling interval (ms)
#define UI_POLLING_DELAY    100         // UI Loop Polling interval (ms) for 100ms timer
#define COMM_POLLING_DELAY  10          // SiiMon communication task Polling interval (ms)

#define LCD_MENU_MAX_LINES  80          // Maximum number of lines in LCD based menus
#define LCD_LIST_MAX_CHARS  20          // Maximum number of characters in LCD line

//-------------------------------------------------------------------------------------------------
//
//  Type definitions
//
//-------------------------------------------------------------------------------------------------

typedef enum _SiiAppMode_t
{
    APP_MODE_TV,
    APP_MODE_MAIN_MENU,
    APP_MODE_DEMO_MENU,
    APP_MODE_SETUP_MENU,
    APP_MODE_BTST,
    APP_MODE_COUNT

} SiiAppMode_t;

// The SiiAppPowerStatus_t enums are exactly the same as the CEC
// power states, except for APP_POWERSTATUS_INITIAL, which is
// specific to this application. The other enum values must retain
// the same value as the CEC states with the same names.

typedef enum  _SiiAppPowerStatus_t
{
    APP_POWERSTATUS_ON              = 0x00,
    APP_POWERSTATUS_STANDBY         = 0x01,
    APP_POWERSTATUS_STANDBY_TO_ON   = 0x02,
    APP_POWERSTATUS_ON_TO_STANDBY   = 0x03,
    APP_POWERSTATUS_INITIAL         = 0x04,

} SiiAppPowerStatus_t;


typedef enum  _SiiAppTpgMode_t
{
    APP_TPG_OFF,            // TPG is disabled, Rx passes through
    APP_TPG_ON,             // TPG is enabled,  Rx blocked
    APP_TPG_ON_AUTO_FMT,    // TPG is enabled,  Rx blocked, video format is discovered automatically
    APP_TPG_AUTO_ENABLE     // TPG is enabled when Rx has no input, TPG is disabled when Rx has active input
} SiiAppTpgMode_t;


typedef enum _SiiErrorLedPatterns_t
{
    ERRLED_NONE             = 0,
    ERRLED_BAD_INIT         = 0x06,

} SiiErrorLedPatterns_t;

// DIP Switches assignment
typedef enum _SiiUserInterfaceIds_t
{
    UI_AUTO_SWITCH      = SW_1,
    UI_ROVING_ENABLE    = SW_2,
    UI_RC_ENABLE        = SW_3,
    UI_SW_4             = SW_4,
    UI_SW_5             = SW_5,
    UI_FLASH_UPDATE     = SW_6,
    UI_SW_7             = SW_7,

} SiiUserInterfaceIds_t;

//-------------------------------------------------------------------------------
//  Starter Kit EDID Read/write extensions
//-------------------------------------------------------------------------------

enum
{
    EDID_TV             = (EDID_NVRAM_VGA + 1),
    EDID_VGA,
    EDID_FLASH,
    EDID_EEPROM,
    EDID_EEPROM_BOOT,
    EDID_FLASH_VGA,
    EDID0_EEPROM,
    EDID1_EEPROM,
    EDID2_EEPROM,
    EDID3_EEPROM,
    EDID_EEPROM_VGA,
    EDID_FILE
};


//------------------------------------------------------------------------------
//
//  Application Instance Data
//
//------------------------------------------------------------------------------

typedef struct AppInstanceData
{
    int                 structVersion;
    int                 lastResultCode;          // Contains the result of the last API function called
    uint16_t            statusFlags;

    SiiAppPowerStatus_t powerState;
    SiiAppPowerStatus_t oldPowerState;
    SiiAppMode_t        modeSelect;

    bool_t              commMode;

    // EEPROM settings

    bool_t              irEnable;
    bool_t              dontUseNvram;           // Do not perform any NVRAM access, load SRAMs from EEPROM
    uint8_t             dbgMsgLevel;
    uint8_t             edidLoad;

    // F/W update

    bool_t              fwUpdEnabled;

    // ARC App-specific

    bool_t              arcEnabled;       // For ARC start timer

    // TV App-specific

    uint8_t             rotarySwitch;
    uint8_t             lastRotarySwitch;
    uint8_t             portSelect;
    uint8_t             oldPort;
    bool_t              autoSwitch;
    bool_t              rovingEnabled;
    SiiTimer_t          AutoSwTimer;

    // Setup App-specific

    uint8_t             setupMenuLevel;

    // Demo App-specific

    uint8_t             demoMenuLevel;

    // TPG specific

    SiiAppTpgMode_t     tpgMode;
    bool_t              isTpgActiveDemo;       // Video patterns will change periodically
    tpgVideoFormat_t    tpgVideoFormat;
    tpgVideoPattern_t   tpgPattern;
    tpgClockSource_t    tpgClockSrc;
    uint8_t             tpgPatternChangeTime;  // in UI_POLLING_DELAY ms units

    // IPV specific
    bool_t              ipvAllowed;
    bool_t              ipvEnabled;
    bool_t              ipvPipEnabled;
    clock_time_t        ipvTimeLastMs;
#if INC_IPV
    SiiIpvMode_t        ipvMode;
    SiiIpvTheme_t       ipvTheme;
#endif
    int_t               ipvHighlightWindow;
    uint8_t             pipSource;  // Rx port input for PIP

    // LCD Menu specific
    char                *lcdMenu[LCD_MENU_MAX_LINES]; // Specifies maximum number of lines in LCD menus

}   AppInstanceData_t;

extern AppInstanceData_t app;


//-------------------------------------------------------------------------------
//
//  API Function Prototypes
//
//-------------------------------------------------------------------------------

// sk_app_support.c

void    SkAppInitInstanceData ( void );
bool_t  SkAppInitialize( void );
void    SkAppSignon( void );
void    SkAppLcdSignOn(void);
void    SkAppExtMonitorHandler(uint_t inst);
bool_t  SkAppEepromInitialize( bool_t useDefaults, bool_t forceInit );
void    SkAppUiTask(uint_t inst);
void    SkAppUiInputControlsUpdate( void );
void    SkAppBlinkTilReset( uint8_t errorLedPattern );
void    SkAppBlinkAlive(uint_t inst);
void    SkAppBlinkInterrupts(void);
void    SkAppInstaPortLedUpdate ( uint8_t portBits );
void    SkAppDevicePower ( uint_t inst );
void    SkAppDisplayData( uint8_t msgLevel, uint8_t *pData, uint16_t length );
void    SkAppDisplayDataLcd ( uint8_t *pData, uint16_t length );
void 	SkAppProcess( SiiAppPowerStatus_t powerState, int source );

    // sk_app_edid.c

bool_t  SkAppInitEdid ( void );
bool_t  SkAppEdidRead ( uint8_t source, uint8_t *pDest );
bool_t  SkAppEdidWrite ( uint8_t target, const uint8_t *pSource );

    // sk_app_switch.c
uint16_t SkAppGetCurrentVideoWidth(void);
void    SkAppTaskSwitch( uint_t inst );
void    SkAppSwitchPortUpdate(  bool_t isForced );
void    SkAppSwitchDeferredPortUpdate ( clock_time_t delay );
void    SkAppSwitchOnVideoResolutionChange( void );
void    SkAppSwitchOnTxMuteChange ( uint_t inst  );

    // sk_app_arc.c
bool_t  SkAppDeviceInitArc( void );
bool_t  SkAppArcDemoMenu ( RC5Commands_t key );

    // sk_app_main_menu.c

void    SkAppTaskIrRemote( uint_t inst  );
uint8_t SkAppIrGetSingleDigit( uint8_t rangeEnd );
void    SkAppIrRemoteReturnToTV( void );

    // sk_app_tv.c

void 	SkAppTvTask( void );
void    SkAppRcTvMode( RC5Commands_t key, bool_t fullAccess );
void    SkAppPortAutoSwitchInit(void);
uint8_t  SkAppPortAutoSwitchGet(void);//Jane add 0205

    // sk_app_demo.c

void    SkAppDemoMenuPrint(void);
bool_t  SkAppRcDemoMenu( RC5Commands_t key, bool_t fullAccess );
void    SkAppDemoEtpgModeEnable ( bool_t isEnabled );

    // sk_app_setup.c

uint16_t SkAppGetOkCancelKey(void);
void    SkAppSetupMenuPrint(void);
bool_t  SkAppRcSetupMenu( RC5Commands_t key, bool_t fullAccess );

    // sk_app_diag.c

bool_t  SkAppDiagMenu( RC5Commands_t key );

    // si_app_gpio.c

bool_t  SkAppDeviceInitGpio( void );
void    SkAppTaskGpio( uint_t inst );

    // si_app_lcd.c
void    SkAppLcdMenuListSet(char **pOptionsList, int_t len);
void    SkAppLcdMenuLineSelect(int_t line);
void    SkAppLcdMenuSelectionMove(bool_t isUp);
int_t   SkAppLcdMenuSelectionGet(void);
void    SkAppLcdMessageBox(char *pLine1, char *pLine2);



#endif // __SK_APPLICATION_H__
