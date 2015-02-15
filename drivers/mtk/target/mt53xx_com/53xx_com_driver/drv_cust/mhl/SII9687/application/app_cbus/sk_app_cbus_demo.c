//***************************************************************************
//!file     sk_app_cbus_demo.c
//!brief
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1140 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2011, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

#include "sk_application.h"
#include "sk_app_cbus.h"

#include "x_printf.h"


extern char gLcdStr[LCD_MENU_MAX_LINES][LCD_LIST_MAX_CHARS];

//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------

static int_t gCbusDevcapRegisterCount = 0;

//------------------------------------------------------------------------------
// Function:    SiiMhlCbRxMscCmdResponseData
// Description: Response data received from peer in response to an MSC command
//				used only for MHL_READ_DEVCAP, prototype is defined in
//				si_cbus_component.h
// Parameters:  cmd - original MSC cmd sent from this device, response of which
//				      came back from peer the the 'data' below
//				data - response data from peer
// Returns:     void
//------------------------------------------------------------------------------
void SiiMhlCbRxMscCmdResponseData (uint8_t cmd, uint8_t data)
{
    bool_t success = true;

    if( cmd == MHL_READ_DEVCAP )
	{
		Printf("\r\n\n Last read peer's device capability register is: %02X\n", data);
	}

	//Printf("\r\n\nResponse data received for last issued command :: %02X\n", data);
	Printf("\r\n Reg %02X :: %02X\n", gCbusDevcapRegisterCount, data );
    // fill up LCD list
   // SiiSprintf(gLcdStr[gCbusDevcapRegisterCount], "Reg %02X : %02X", gCbusDevcapRegisterCount, data);
    app.lcdMenu[gCbusDevcapRegisterCount + 1] = gLcdStr[gCbusDevcapRegisterCount];

    // Until all register have been read
    if (gCbusDevcapRegisterCount < 15)
    {
        // Continue with next register
        gCbusDevcapRegisterCount++;
        success = SiiMhlRxReadDevCapReg(gCbusDevcapRegisterCount);
        SkAppLcdMenuListSet(app.lcdMenu, gCbusDevcapRegisterCount + 1); // add item (+ head option)
    }
    else
    {
        SkAppLcdMenuListSet(app.lcdMenu, 17); // add all items (+ head option)
        gCbusDevcapRegisterCount = 0; // reset counter for next cycle
    }

    if(!success)
    {
     //   SI_LCD_PRINTF(1, "RD_DEVCAP ERROR!");
    }
}


static void CbusDemoMenuPrint(void)
{
    Printf("\r\n\nCBUS Demo\n" );
    Printf("\r\n--------\n" );
    Printf("\r\n0 - Return to main Demo Menu\n" );
    Printf("\r\n1 - Display this device's DEV CAPS\n" );
    Printf("\r\n2 - Display connected device's DEV CAPS\n" );
    Printf("\r\n3 - Send a MHL_SET_HPD msg to peer\n" );
    Printf("\r\n4 - Send a SET_INT msg to peer\n" );
    Printf("\r\n5 - Send a WRITE_STAT msg to peer\n" );
    Printf("\r\n6 - Send a MHL_MSC_MSG -> ( RCP 'PLAY' cmd ) msg to peer\n" );
    Printf("\r\n7 - Send a MHL_WRITE_BURST msg to peer\n" );

    // Fill up LCD menu
    app.lcdMenu[0] = "Return to main";
    app.lcdMenu[1] = "This DEV CAPS";
    app.lcdMenu[2] = "Remote DEV CAPS";
    app.lcdMenu[3] = "Send SET_HPD";
    app.lcdMenu[4] = "Send SET_INT";
    app.lcdMenu[5] = "Send WRITE_STAT";
    app.lcdMenu[6] = "Send RCP Play";
    app.lcdMenu[7] = "Send WRITE_BRST";
    SkAppLcdMenuListSet(app.lcdMenu, 8);
}

//------------------------------------------------------------------------------
// Function:    DemoCbus
// Description: CBUS demo command options
// Returns:
//------------------------------------------------------------------------------

bool_t SkAppCbusDemoMenu ( RC5Commands_t key )
{
    int_t   	i;
    cbus_req_t 	req;
    uint8_t 	devCapValue, regAddr;
    bool_t      mhlTaskStartRequested = false;

    if (key == RC5_INCREMENT || key == RC5_ENTER)
    {
        key = RC5_0 + SkAppLcdMenuSelectionGet();
    }

    switch ( key )
    {
        case RC5_SELECT:
            CbusDemoMenuPrint();
            break;

        case RC5_0:
            app.demoMenuLevel = 0;
            Printf("\r\n\n\nDemo Menu -- Press MENU key to select Demo\n" );
            SkAppDemoMenuPrint();
            break;

        case RC5_1:
            // Get our DEV CAPS
            Printf("\r\n\n\nThis Device Capability Registers are:: \n" );
            app.lcdMenu[0] = "Back to Menu";
            for (i = 0; i < 16; i++)
            {
            	regAddr = REG_CBUS_DEVICE_CAP_0 + i;
            	devCapValue = SiiDrvCbusRegisterGet( regAddr );
                Printf("\r\n Reg %02X :: %02X\n", i, devCapValue );
                // fill up LCD list
//                SiiSprintf(gLcdStr[i], "Reg %02X : %02X", i, devCapValue);
                app.lcdMenu[i + 1] = gLcdStr[i];
            }
            SkAppLcdMenuListSet(app.lcdMenu, 16);
            break;

        case RC5_2:
            Printf("\r\n\n\nPeer Device Capability Registers are:: \n" );
            // Add menu header
            app.lcdMenu[0] = "Back to Menu";
            SkAppLcdMenuListSet(app.lcdMenu, 1);

            // Kick start reading chain.
            // The following non-blocking call will cause SiMhlRxMscCmdRetDataNtfy()
            // callback to be called
            if(SiiMhlRxReadDevCapReg(0))
            {
                gCbusDevcapRegisterCount = 0; // reset register count
                mhlTaskStartRequested = true;
            }
            else
            {
            //    SI_LCD_PRINTF(1, "RD_DEVCAP ERROR!");
            }
            break;

        case RC5_3:
            // Send MHL_CLR_HPD / and then send MHL_SET_HPD
            Printf("\r\n\n\nSending MHL_CLR_HPD to peer" );
       //     SI_LCD_PRINTF(0, "CLR_HPD --> peer");
            if (!(SiiMhlRxHpdSet(false)))
            {
                Printf("\r\n\nCouldn't send MHL_CLR_HPD to peer" );
       //         SI_LCD_PRINTF(1, "CLR_HPD failed!");
            }

            Printf("\r\n\n\nSending MHL_SET_HPD to peer" );
         //   SI_LCD_PRINTF(0, "SET_HPD --> peer");
            if (!(SiiMhlRxHpdSet(true)))
            {
                Printf("\r\n\nCouldn't send MHL_SET_HPD to peer" );
          //      SI_LCD_PRINTF(1, "SET_HPD failed!");
            }
            mhlTaskStartRequested = true;
            break;
        case RC5_4:
        	Printf("\r\n\n\nSending MHL_SET_INT to peer, sending EDID_CHG interrupt");
        	//SI_LCD_PRINTF(0, "SET_INT --> peer ");
			if( !SiiMhlRxSendEdidChange())
			{
				Printf("\r\n\nSiMhlRxSendEdidChange() failed!!\n" );
			//	SI_LCD_PRINTF(0, "SET_INT --> peer ");
			}
			mhlTaskStartRequested = true;
            break;
        case RC5_5:
			Printf("\r\n\n\nSending MHL_WRITE_STAT to peer, sending PATH_EN status to peer");
	//		SI_LCD_PRINTF(0, "WRT_STAT -> peer");
			if( !SiiMhlRxPathEnable( true ) )
			{
				Printf("\r\nSiiMhlRxPathEnable() failed!!\n" );
			//	SI_LCD_PRINTF(1, "WRT_STAT failed!");
			}
			mhlTaskStartRequested = true;
			break;
        case RC5_6:
            Printf("\r\n\n\nSending MHL_MSC_MSG -> ( RCP 'PLAY' cmd ) msg to peer");
         //   SI_LCD_PRINTF(0, "'Play' --> peer");
            if (!(SiiMhlRxSendRCPCmd(MHL_RCP_CMD_PLAY)))
            {
                Printf("\r\n\nCouldn't send RCP msg to peer\n" );
              //  SI_LCD_PRINTF(1, "RCP failed!");
            }
            mhlTaskStartRequested = true;
            break;

        case RC5_7:
            Printf("\r\n\n\nSending MHL_WRITE_BURST to peer, check peer's scratchpad registers!!");
         //   SI_LCD_PRINTF(0, "WRT_BRST -> peer");
            // write different values in different registers
            for (i = 0; i < 16; i++)
            {
                req.msgData[i] = i + 5;
            }

            if (!(SiiMhlRxWritePeerScratchpad(0, 16, &req.msgData[0])))
            {
                Printf("\r\n\nCouldn't send MHL_WRITE_BURST to peer\n" );
           //    SI_LCD_PRINTF(1, "WRT_BRST failed!");
            }
            mhlTaskStartRequested = true;
            break;

        default:
            break;
    }


    if (mhlTaskStartRequested)
    {   // Kick-off command sending process
        SkAppTaskCbus(0);
    }

    return( false );
}

