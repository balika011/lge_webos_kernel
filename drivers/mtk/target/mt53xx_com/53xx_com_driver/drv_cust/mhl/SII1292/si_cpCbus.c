//***************************************************************************
//!file     si_cpCBUS.c
//!brief    CP 9387 Starter Kit CDC demonstration code.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009, Silicon Image, Inc.  All rights reserved.
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

uint8_t     g_halMsgLevel;
#ifdef DETECT_OCP
extern uint8_t OCP_Flag;
#endif
//------------------------------------------------------------------------------
// Function:    CbusConnectionCheck
// Description: Display any change in CBUS connection state and enable
//              CBUS heartbeat if channel has been connected.
// Parameters:  channel - CBUS channel to check
//------------------------------------------------------------------------------

static void CbusConnectionCheck ( uint8_t channel )
{
    static bool_t busConnected[ 1 ] = {false};
    static bool_t hpd[ 1 ] = {false};
    static bool_t rsen[ 1 ] = {false};
    static uint32_t count   = 0;
    static bool_t timer_set = false;
    uint8_t result;
    cbus_out_queue_t req;
    bool_t connect_wait, hpd_state, path_enable, connect_ready, dcap_rdy, read_info;

   // for warning
    UNUSED(rsen);
	UNUSED(hpd_state);
    UNUSED(count);
    
    /* If CBUS connection state has changed for this channel,   */
    /* update channel state and hardware.                       */

    if (busConnected[ channel ] != SI_CbusChannelConnected( channel ))
    {
        busConnected[ channel ] = SI_CbusChannelConnected( channel );

        if (!busConnected[ channel ])
        {
            SiIRegioModify(REG_SRST, BIT_CBUS_RST, BIT_CBUS_RST);   // Reset CBUS block after CBUS disconnection
            SiIRegioModify(REG_SRST, BIT_CBUS_RST, 0);
            SI_CbusInitParam(channel);

            hpd[channel]  = false;
            rsen[channel] = false;
            count = 0;

            //DEBUG_PRINT(MSG_DBG, ("after reset CBUS,CBUS PAGE: 0x3C = %02X\n", (int)SiIRegioCbusRead(REG_CBUS_LINK_LAYER_CTRL12,channel)));
            //fixed when abort packet received, not wait 2s.
            //write  REG_RX_MISC again when CBUS is reset;
            vPsCtrl(VBUS_ON);
   
        }

 #ifdef SUPPORT_MSCMSG_IGNORE_LIST
        SI_CbusResetMscMsgIgnoreList(channel, CBUS_OUT);
 #endif
	 	SiIRegioWrite(REG_CBUS_RETRY_INTERVAL, 0xBE);
        SI_CbusInitDevCap(channel);

        //      SiIRegioCbusWrite(REG_CBUS_LINK_LAYER_CTRL12,channel,0xA3);  //tiger add this ,request from jason 2011-8-4
        //      //CBUS 0X3C IS SET TO DEFAULTS ,when CBUS connection status changed;

        SiIRegioWrite(REG_CBUS_INTR_STATUS, BIT_MSC_MSG_RCV); //tiger , 12-07-2-11, bi3 hardware defualt value is 1,clear it when init;
    }

    if (busConnected[ channel])
    {

        connect_wait = SI_CbusGetConnectWait(channel);
        hpd_state     = SI_CbusGetHpdState(channel);
        path_enable   = SI_CbusGetPathEnable(channel);
        connect_ready = SI_CbusGetConnectReady(channel);
        dcap_rdy  = SI_CbusGetDcapRdy(channel);
        read_info = SI_CbusGetReadInfo(channel);

        if (!connect_wait)
        {
            if (!timer_set)
            {
                timer_set = true;
                count = timer_count;
            }
            else
            {
				vSil_Delay1ms(DEM_MHL_WAIT_TIME);
                {
                    SI_CbusSetConnectWait(channel, true);
                    count = 0;
                    timer_set = false;
                }
            }
        }

        if (connect_wait)
        {
            if (hpd[channel] != g_HPD_IN)
            {
                hpd[channel] = g_HPD_IN;

                if (hpd[channel])
                {
                    DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: SET HPD\n"));
                    req.command = MSC_SET_HPD;
                    req.dataRetHandler  = SI_MscSetHpd;
                    req.sRetHandlerName = "SI_MscSetHpd";

                    req.retry = 3;  // don't retry twice
					result = (SI_CbusPushReqInOQ( channel, &req, true ) == STATUS_SUCCESS) ? SUCCESS : FAIL;

                    if (result != SUCCESS)
                    {
                        DEBUG_PRINT(MSG_DBG, ("CBUS:: Error found in SI_CbusPushReqInOQ: %02X\n", (int)result));
						hpd[channel] = ((g_HPD_IN == true) ? false : true);
                    }
                }
                else
                {
                    DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: CLR HPD\n"));
                    req.command = MSC_CLR_HPD;
                    req.dataRetHandler  = SI_MscClrHpd; // will send disable path in SI_MscClrHpd
                    req.sRetHandlerName = "SI_MscClrHpd";

                    req.retry = 3;  // don't retry twice
					result = (SI_CbusPushReqInOQ( channel, &req, true ) == STATUS_SUCCESS) ? SUCCESS : FAIL;

                    if (result != SUCCESS)
                    {
                        DEBUG_PRINT(MSG_DBG, ("CBUS:: Error found in SI_CbusPushReqInOQ: %02X\n", (int)result));
						hpd[channel] = ((g_HPD_IN == true) ? false : true);
                    }

 #ifdef SUPPORT_MSCMSG_IGNORE_LIST
                    SI_CbusResetMscMsgIgnoreList(channel, CBUS_IN);
 #endif
                }
            }
        }

        /*if ( hpd_state )
        {
            if (rsen[channel] != g_TX_RSEN_Valid)
            {
                rsen[channel] = g_TX_RSEN_Valid;

                if( rsen[channel] )
                {
                    DEBUG_PRINT(MSG_DBG, ("CBUS:: Path Enable\n"));
                    req.command     = MSC_WRITE_STAT;
                    req.offsetData  = MHL_DEVICE_STATUS_LINK_MODE_REG_OFFSET;
                    req.msgData[0]	= PATH_EN;
                    req.dataRetHandler = SI_MscPathEnable;
                    req.retry = 0;	// don't retry twice
                    result = SI_CbusPushReqInOQ( channel, &req, true );

                    if ( result != SUCCESS )
                    {
                        DEBUG_PRINT(MSG_DBG, ("CBUS:: Error found in SI_CbusPushReqInOQ: %02X\n", (int)result));
                        rsen[channel] = !g_TX_RSEN_Valid;
                    }
                }
            }
        }*/

        if (path_enable)
        {
            if (!connect_ready)
            {
                DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: Connected Ready\n"));
                req.command = MSC_WRITE_STAT;
                req.offsetData = MHL_DEVICE_STATUS_CONNECTED_RDY_REG_OFFSET;
                req.msgData[0] = DCAP_RDY;
                req.dataRetHandler  = SI_MscConnectReady;
                req.sRetHandlerName = "SI_MscConnectReady";

                req.retry = 3;  // don't retry twice
				result = (SI_CbusPushReqInOQ( channel, &req, true ) == STATUS_SUCCESS) ? SUCCESS : FAIL;

                if (result != SUCCESS)
                {
                    DEBUG_PRINT(MSG_DBG, ("CBUS:: Error found in SI_CbusPushReqInOQ: %02X\n", (int)result));
                }
                else
                {
                    SI_CbusSetConnectReady(channel, true);
                }
            }
        }

		if ( connect_ready)
		{
			if ((!( read_info & DCAP_READ_DONE )) && dcap_rdy)
			{
				SI_MscStartGetDevInfo(channel);
			}
		}
	}
	
}

//------------------------------------------------------------------------------
// Function:    CpCbusProcessPrivateMessage
// Description: Get the result of the last message sent and use it appropriately
//              or process a request from the connected device.
// Parameters:  channel - CBUS channel that has message data for us.
//------------------------------------------------------------------------------

static uint8_t CpCbusProcessPrivateMessage ( uint8_t channel )
{
    uint8_t result, result1, result2;

    result1 = SI_CbusIQReqStatusCheck(channel);
    result2 = SI_CbusOQReqStatusCheck(channel);
    result = result1 | result2;

    return (result);
}

//------------------------------------------------------------------------------
// Function:    CpCbusHandler
// Description: Polls the send/receive state of the CBUS hardware.
//------------------------------------------------------------------------------

void CpCbusHandler ( void )
{
    uint8_t channel, status;

	channel = 0;
    /* Check the current input to see if it switch between HDMI and MHL */

    if ((g_currentInputMode == HDMI))
    {
        return;
    }

    /* Monitor all CBUS channels.   */


        /* Update CBUS status.  */
        SI_CbusUpdateBusStatus( channel );
        CbusConnectionCheck( channel );

        /* Monitor CBUS interrupts. */
        status = SI_CbusHandler( channel );
        if (status != SUCCESS)
        {
            //DEBUG_PRINT(MSG_ALWAYS, ("Error returned from SI_CbusHandler: %02X\n", (int)status));
        }

        status = CpCbusProcessPrivateMessage( channel );
        if (status != SUCCESS)
        {
            DEBUG_PRINT(MSG_DBG, ("Error returned from CpCbusProcessPrivateMessage: %02X\n", (int)status));
        }

		
		if(g_Non3D_WB_Processing == true)
		{
			if(g_3D_WB_Processing == false)
			{
				cbus_out_queue_t req;
		
				g_Non3D_WB_Processing = false;
				req.command 		= MSC_SET_INT;
				req.offsetData		= MHL_REG_CHANGE_INT_OFFSET;
				req.msgData[0]	= CBUS_INT_BIT_REQ_WRT;
				req.dataRetHandler = SI_WriteBurstDataDoneDummy;
				req.sRetHandlerName = "SI_Msc_REQ_WRT_Done";
				req.retry = 0;
		
				SI_CbusPushReqInOQ( 0, &req, true );
			}
		}
}
