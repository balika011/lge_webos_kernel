//***************************************************************************

//!file     si_cp9387_ext.c
//!brief    CP 9387 Start Kit extension functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2008-2009, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

/***** #include statements ***************************************************/

#include "si_RegioCbus.h"
#include "si_regio.h"
#include "si_hal.h"
#include "si_datatypes.h"
#include "si_cp1292.h"
#include "si_cbusDefs.h"
#include "si_cbus_regs.h"
#include "si_cbus.h"
#include "si_apiRCP.h"
#include "si_apiRAP.h"
#include "si_apiMSC.h"
#include "si_apiConfigure.h"
#include "si_apiCbus.h"
#include "si_api1292.h"
#include "si_1292regs.h"
#include "drv_sil_if.h"

//-------------------------------------------------------------------------------
//  Global data
//-------------------------------------------------------------------------------
#ifndef SUPPORT_ARM_CC
BOOL g_deviceInterrupt = false;
#endif
bool_t mhl_con = false;

extern uint16_t g_pulseWidthCounter;
extern bool_t g_startWidthCounter;

#ifdef DETECT_OCP
extern uint8_t OCP_Flag;
#endif
//------------------------------------------------------------------------------
// Function:    DeviceIntHandler
// Description: Silicon Image device interrupt handler.  This function only
//              sets the global interrupt flag, since accessing the I2C port
//              in an ISR is not a good thing.
//------------------------------------------------------------------------------

#ifndef SUPPORT_ARM_CC
static void DeviceIntHandler ( void ) interrupt 0
{
    g_deviceInterrupt = true;
}
#endif

#if (FPGA_BUILD == 0)

 #define LATEST_OTP_REV 5

//------------------------------------------------------------------------------
// Function:    CpCheckOTPRev
// Description: Read OTP Revision and update registes
//			Only in Chip mode, not for FPGA
//------------------------------------------------------------------------------
void CpCheckOTPRev( void )
{
    uint8_t OTP_rev;

	SiIRegioRead(REG_OTP_REV, &OTP_rev);
    OTP_rev &= MASK_OTP_REV;

    DEBUG_PRINT(MSG_DBG, ("Read OTP Revison ... %d\n", (int)OTP_rev));
    DEBUG_PRINT(MSG_DBG, ("Latest OTP Revison ... %d\n", (int)LATEST_OTP_REV));

	//deleted by zhangyue on 2013-03-21 for meeint sony's powe seq spec start
	#if 0
	SiIRegioModify(REG_SRST,BIT_SWRST | BIT_CBUS_RST, BIT_SWRST | BIT_CBUS_RST);
	vSil_Delay1ms(10);
	SiIRegioModify(REG_SRST,BIT_SWRST | BIT_CBUS_RST, 0x00);
	#endif
	//deleted by zhangyue on 2013-03-21 for meeint sony's powe seq spec end
	
    switch (OTP_rev)
    {
    case 0:
        SiIRegioWrite(0x071, 0xA2);     // RX_Ctrl6
        SiIRegioWrite(0x06C, 0x08);     // RX_Ctrl1
        SiIRegioWrite(0x024, 0x84);     // DPLL_CFG6
        SiIRegioWrite(0x00C, 0x45);     // DATA_MUX_1
        SiIRegioWrite(0x00D, 0x0E);     // DATA_MUX_2
        SiIRegioWrite(0x050, 0xEE);     // TX_SWING
        SiIRegioWrite(0x051, 0x80);     // TX_SWING1
        SiIRegioWrite(0x07E, 0x10);     // POWERMUX_CTRL2, To adjust 20MHz oscillator setting
        SiIRegioWrite(0x081, 0x98);     // CBUS_FLOAT
        SiIRegioWrite(0x017, 0x00);     // PEQ_VAL0
        SiIRegioWrite(0x018, 0x20);     // PEQ_VAL1
        SiIRegioWrite(0x019, 0x40);     // PEQ_VAL2
        SiIRegioWrite(0x01A, 0x70);     // PEQ_VAL3
        SiIRegioWrite(0x01B, 0x13);     // PEQ_VAL4
        SiIRegioWrite(0x01C, 0x23);     // PEQ_VAL5
        SiIRegioWrite(0x01D, 0x33);     // PEQ_VAL6
        SiIRegioWrite(0x01E, 0x63);     // PEQ_VAL7
        SiIRegioWrite(0x089, 0x22);     // DPLL_BW_CFG1
        SiIRegioWrite(0x08B, 0x00);     // MEQ_VAL0
        SiIRegioWrite(0x08C, 0x20);     // MEQ_VAL1
        SiIRegioWrite(0x08D, 0x40);     // MEQ_VAL2
        SiIRegioWrite(0x08E, 0x70);     // MEQ_VAL3
        SiIRegioWrite(0x013, 0x01);     // DPLL_CFG3, turn OFF BW scan
        SiIRegioWrite(0x01F, 0x14);     // PBW_VAL0, HDMI BW
        SiIRegioWrite(0x020, 0x15);     // PBW_VAL1, This value may not be optimal, but is better than the current default one.
                                        // Will probably not be used since BW scan is OFF, but probably is safer to have a decent value in this register just in case.
        SiIRegioWrite(0x021, 0x1B);     // PBW_VAL2, MHL BW
        SiIRegioWrite(0x022, 0x1C);     // PBW_VAL3, This value may not be optimal, but is better than the current default one.
                                        // Will probably not be used since BW scan is OFF, but probably is safer to have a decent value in this register just in case.
        SiIRegioWrite(0x014, 0x9C);     // DPLL_CFG4, SCDT auto reset enable
        SiIRegioWrite(0x077, 0x60);     // CBUS_HPD_HOLD. Must change this value from default of 0xC0 to pass MHL spec for TSINK: DCAP_RDY parameter.  Must also change to this new value for the dongle (SiI9292) as well
        SiIRegioWrite(0x073, 0x03);     // CBUS_DISC_OVR. Enable default MHL/HDMI mode strapping override.  New register value strapping is MHL mode.
        SiIRegioWrite(0xC07, 0x77);     // RSVD. Change default value of CBUS drive strength.  Must also change to this new value for the dongle (SiI9292) as well
    case 1:
        SiIRegioWrite(0xC80, 0x00);             // Dev Cap 0: DEV_STATE
        SiIRegioWrite(0xC81, 0x10);             // Dev Cap 1: MHL_VERSION
        SiIRegioWrite(0xC82, 0x01);             // Dev Cap 2: DEV_CAT
        SiIRegioWrite(0xC83, 0x00);             // Dev Cap 3: ADOPTER_ID_H
        SiIRegioWrite(0xC84, 0x00);             // Dev Cap 4: ADOPTER_ID_L
        SiIRegioWrite(0xC85, 0x37);             // Dev Cap 5: VID_LINK_MODE
        SiIRegioWrite(0xC86, 0x00);             // Dev Cap 6: AUD_LINK_MODE
        SiIRegioWrite(0xC87, 0x00);             // Dev Cap 7: VIDEO_TYPE
        SiIRegioWrite(0xC88, 0x00);             // Dev Cap 8: LOG_DEV_MAP
        SiIRegioWrite(0xC89, 0x0F);             // Dev Cap 9: BANDWIDTH
        SiIRegioWrite(0xC8A, 0x07);             // Dev Cap A: FEATURE_FLAG
        SiIRegioWrite(0xC8B, 0x92);             // Dev Cap B: DEVICE_ID_H
        SiIRegioWrite(0xC8D, 0x10);             // Dev Cap D: SCRATCHPAD_SIZE
        SiIRegioWrite(0xC8F, 0x00);             // Dev Cap F: RESERVED
    case 2:
        SiIRegioWrite(0x00A, 0x03);     // SYS_CTRL3: TMDS output swing enable controlled by CKDT and SCDT
        SiIRegioWrite(0x00B, 0x1F);     // PAD_COMB_CTRL: Set bits [1:0] both to 1 to enable CBUS Vinh enhancement
        SiIRegioWrite(0xC07, 0x77);     // RSVD: Change default value of CBUS drive strength to maximum value
    case 3:
        SiIRegioWrite(0x07C, 0x2C);     // DDC_CTRL: Adjust DDC speed to increase DDC I2C setup time for I2C compliance
        SiIRegioWrite(0xC8E, 0x33);     // Dev Cap E: INT_STAT_SIZE
        SiIRegioWrite(0xC8C, 0x41);     // Dev Cap C: DEVICE_ID_L, Upper nibble [7:4] of this register is used to indicate OTP revision. Lower nibble [3:0] of this register is used to indicate silicon revision
        DEBUG_PRINT(MSG_DBG, ("Updating registers ...\n"));
    case 4:
	case 5:
        break;
    default:
        DEBUG_PRINT(MSG_ALWAYS, ("Error in OTP Revison\n"));
        break;
    }
}

#endif // (FPGA_BUILD == 0)

//------------------------------------------------------------------------------
// Function:    CpCableDetect
// Description: To check if MHL cable connected
//
//
//
//------------------------------------------------------------------------------
BOOL CpCableDetect( void )
{
    uint8_t uData;

#ifdef BOARD_CP1292
	SiIRegioRead(REG_GPIO_CTRL, &uData);
    if (uData & BIT_GPIO0_INPUT)
    {
#endif
#ifdef BOARD_JULIPORT
	SiIRegioRead(REG_STATE, &uData);

	if(uData & BIT_V5DET)
	{

#endif
	//	if (!mhl_con)
        {
            mhl_con = true;
            vPsCtrl(VBUS_ON);
            //DEBUG_PRINT(MSG_DBG, ("MHL Cable Connected!\n"));
        }

        return true;
#ifdef BOARD_JULIPORT
    }
#endif
#ifdef BOARD_CP1292
	}
#endif
	else
	{
	//	if (mhl_con)
        {
            mhl_con = false;
            vPsCtrl(VBUS_OFF);
            //DEBUG_PRINT(MSG_DBG, ("MHL Cable Unconnected!\n"));
        }

        return false;
    }

}

