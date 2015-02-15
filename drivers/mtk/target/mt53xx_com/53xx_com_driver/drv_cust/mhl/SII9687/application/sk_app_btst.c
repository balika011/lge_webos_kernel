//***************************************************************************
//!file     sk_app_btst.c
//!brief    SK 9687 Starter Kit firmware -  Automated Bench Test
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "../driver/sii9687_drv/si_drv_internal.h"
#include "sk_application.h"
#include "../platform/board/si_platform.h"
#include "../platform/board/si_gpio.h"
#include "../component/ipv/si_ipv_component.h"

// TODO: implement operations testing plan

//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

typedef enum testId
{
    BTEST_NO_MSW,
    BTEST_MSW_M0_S1,
    BTEST_MSW_M1_S0,
    BTEST_MSW_M2_S3,
    BTEST_MSW_M3_S2,
    BTEST_MSW_M4_S5,
    BTEST_MSW_M5_S4,
    BTEST_AUD_EXTR_M_S,
    BTEST_OSD,
    BTEST_IPV,
    BTEST_IV_COLORBAR,
    BTEST_IV_CHESS,
    BTEST_IV_RAMP,
    BTEST_PV,

    NUM_OF_BENCH_TESTS
} testId_t;

#if 0
static clock_time_t bTestDuration[NUM_OF_BENCH_TESTS] =
                    {
                        20000,     //BTEST_NO_MSW
                        10000,     //BTEST_MSW_M0_S1
                        10000,     //BTEST_MSW_M1_S0
                        10000,     //BTEST_MSW_M2_S3
                        10000,     //BTEST_MSW_M3_S2
                        10000,     //BTEST_MSW_M4_S5
                        10000,     //BTEST_MSW_M5_S4
                        20000,     //BTEST_AUD_EXTR_M_S
                        20000,     //BTEST_OSD
                        50000,     //BTEST_IPV
                        10000,     //BTEST_IV_COLORBAR
                         5000,     //BTEST_IV_CHESS
                         5000,     //BTEST_IV_RAMP
                        20000,     //BTEST_PV
                    };

static bool_t isSkipTestFor9573[NUM_OF_BENCH_TESTS] =
                    {
                        false,     //BTEST_NO_MSW
                        true,      //BTEST_MSW_M0_S1
                        true,      //BTEST_MSW_M1_S0
                        true,      //BTEST_MSW_M2_S3
                        true,      //BTEST_MSW_M3_S2
                        true,      //BTEST_MSW_M4_S5
                        true,      //BTEST_MSW_M5_S4
                        true,      //BTEST_AUD_EXTR_M_S
                        false,     //BTEST_OSD
                        false,     //BTEST_IPV
                        false,     //BTEST_IV_COLORBAR
                        false,     //BTEST_IV_CHESS
                        false,     //BTEST_IV_RAMP
                        false,     //BTEST_PV
                    };
#endif


//------------------------------------------------------------------------------
// Function:    SkAppBenchTestIrRemote
// Description: Parse remote control keys for the Bench Test App.
// Parameters:  fullAccess - true if allowed to respond to all normal keys
//                           false if only allowed to respond to power keys
// Returns:     none
//------------------------------------------------------------------------------
void SkAppBenchTestIrRemote ( RC5Commands_t key, bool_t fullAccess )
{
}

//------------------------------------------------------------------------------
// Function:    SkAppBenchTest
// Description: CP 9687 Bench Test Automation
//------------------------------------------------------------------------------

void SkAppBenchTest ( void )
{
#if 0
    static testId_t test = BTEST_NO_MSW;
    // SiI9573 version of the chip doesn't have sub-pipe, so some of the tests must be skipped
    bool_t is9573 = (SiiDrvDeviceInfo( SiiDEV_ID ) != 0x9687);

    /****************************************************************************************/
    /* Display Port Processor State                                                         */
    /****************************************************************************************/

    SkAppInstaPortLedUpdate( SiiDrvDeviceInfo(SiiDEV_SELECTED_PORT_BITFIELD));

    if ( !SiiOsTimerExpired( app.appTimer ))
    {
        return;
    }

    if (is9573 && isSkipTestFor9573[test])
    {
        test++;
        Printf("\r\nTest Skipped for SiI9573.\n" );
        return;
    }
    else if (test < NUM_OF_BENCH_TESTS)
    {
        SiiOsTimerSet( &app.appTimer, bTestDuration[test]);
    }


    Printf("\r\n###########################################################################\n" );

    switch(test)
    {
        case   BTEST_NO_MSW:
            SiiIpvModeSet( SiiIpvModeALL );
            SiiIpvEnable( false );
            SkAppMswDemoSetSources(0, 0);
            Printf("\r\nNO MSW Test...\n" );
            test++; // move to next test
            break;

        case   BTEST_MSW_M0_S1:
            // Clean up previous test
            SiiIpvEnable( false );
            Printf("\r\nMSW Test. Main:0, Sub:1...\n" );
            SkAppMswDemoSetSources( 0, 1 );
            test++; // move to next test
            break;

        case   BTEST_MSW_M1_S0:
            Printf("\r\nMSW Test. Main:1, Sub:0...\n" );
            SkAppMswDemoSetSources( 1, 0 );
            test++; // move to next test
            //test+=4;
            break;

        case   BTEST_MSW_M2_S3:
            Printf("\r\nMSW Test. Main:2, Sub:3...\n" );
            SkAppMswDemoSetSources( 2, 3 );
            test++; // move to next test
            break;

        case   BTEST_MSW_M3_S2:
            Printf("\r\nMSW Test. Main:3, Sub:2...\n" );
            SkAppMswDemoSetSources( 3, 2 );
            test++; // move to next test
            break;

        case   BTEST_MSW_M4_S5:
            Printf("\r\nMSW Test. Main:4, Sub:5...\n" );
            SkAppMswDemoSetSources( 4, 5 );
            test++; // move to next test
            break;

        case   BTEST_MSW_M5_S4:
            Printf("\r\nMSW Test. Main:5, Sub:4...\n" );
            SkAppMswDemoSetSources( 5, 4 );
            test++; // move to next test
            break;

        case   BTEST_AUD_EXTR_M_S:
            SkAppMswDemoSetSources( 0, 1 ); // MSW should be enabled
            Printf("\r\nAudio Extraction Test...\n" );
            SkPlatformGpioAudioInOut(true);
            SiiDrvAudRxInstanceSet(0);
            SiiRegWrite(REG_AUDIO_CHAN_SEL, 0x0);
            SiiRegWrite(REG_AUDRX_CTRL_P1, 0x30);
            test++; // move to next test
            break;

        case   BTEST_OSD:
            // Cleanup previous test
            SkAppMswDemoSetSources(0, 0);
            Printf("\r\nOSD Test...\n" );
            SiiOsdShowWindows(true);
            test++; // move to next test
            break;

        case   BTEST_IPV:
            SiiOsdShowWindows(false);
            Printf("\r\nIPV Test...\n" );
            SiiIpvThemeSet( SiiIpvTOP_CENTER );
            SiiIpvEnable( true );
            app.ipvTheme = SiiIpvTOP_CENTER;
            test++; // move to next test
            break;

        case   BTEST_IV_COLORBAR:
            Printf("\r\nInternal Video Test: Color Bars.\n" );
            app.newSource[0] = app.newSource[1] = SiiSwitchSourceInternalVideo;
            SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_COLORBAR);
            test++; // move to next test
            break;

        case   BTEST_IV_CHESS:
            Printf("\r\nInternal Video Test: Chess Field.\n" );
            SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_CHESS);
            test++; // move to next test
            break;

        case   BTEST_IV_RAMP:
            Printf("\r\nInternal Video Test: Gray Levels Ramp.\n" );
            SiiDrvExtTpgPatternSet(app.tpgVideoPattern = SI_ETPG_PATTERN_RAMP);
            test++; // move to next test
            break;

        case   BTEST_PV:
            Printf("\r\nParallel Video test.\n" );
            app.newSource[0] = app.newSource[1] = SiiSwitchSourceParallelVideo;
            test++; // move to next test
            break;


        default:
            Printf("\r\nBENCH TEST IS OVER.\n" );
            test = BTEST_NO_MSW;
            break;

    }
    Printf("\r\n###########################################################################\n" );
#endif
}

