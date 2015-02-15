//***************************************************************************

//!file     si_apiMSC.c
//!brief    CP 1292 Starter Kit MSC functions.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
//***************************************************************************/

/**
 * @file si_apiMSC.c
 *
 * MSC command function of SiI1292
 *
 * $Author: Silicon Image, firmware team
 * $Rev: 0.3
 * $Date: Apr. 2010
 *
 *****************************************************************************/

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

extern uint8_t g_u3DstateMachine;
extern _3D_SUPPORT_DTD_VIC_LIST_T g_3D_SUPPORT_DTD_VIC_LIST;

/***** public functions *******************************************************/

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: CLR_HPD
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscClrHpd(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusSetHpdState(channel, false);
    SI_CbusOQCleanActiveReq(channel);

    DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: Path Disable\n"));
    req.command = MSC_WRITE_STAT;
    req.offsetData = MHL_DEVICE_STATUS_LINK_MODE_REG_OFFSET;
    req.msgData[0] = 0;
    req.dataRetHandler  = SI_MscPathDisable;
    req.sRetHandlerName = "SI_MscPathDisable";

    req.retry = 3;  // retry 3 times if timeout or abort for important MSC commands
    SI_CbusPushReqInOQ( channel, &req, true );

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: SET_HPD
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscSetHpd(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    //Jin: Fix for Bug#22149
    //SiIRegioCbusWrite( REG_CBUS_DEVICE_CAP_0, channel, POWER_STATE_ACTIVE);

    SI_CbusSetHpdState(channel, true);
    SI_CbusOQCleanActiveReq(channel);

#if 0//if add this, system will hang up, so remove
	DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: EDID Change\n"));			
	req.command     = MSC_WRITE_STAT;
	req.offsetData  = DCHANGE_INT;
	req.msgData[0]	= EDID_CHG;
    req.dataRetHandler  = SI_WriteBurstDataDoneDummy;
	req.sRetHandlerName = "SI_MscEdidCHG";

	req.retry = 3;
	SI_CbusPushReqInOQ( channel, &req, true );				
#endif
    DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: Set Path Enable\n"));
    req.command = MSC_WRITE_STAT;
    req.offsetData = MHL_DEVICE_STATUS_LINK_MODE_REG_OFFSET;
    req.msgData[0] = PATH_EN;
    req.dataRetHandler  = SI_MscPathEnable;
    req.sRetHandlerName = "SI_MscPathEnable";

    req.retry = 3;  // retry 3 times if timeout or abort for important MSC commands
    SI_CbusPushReqInOQ( channel, &req, true );

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: SET_INT, GRT_WRT
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscGrtWrt(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusSetGrtWrt(channel, true);
    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When REQ_WRT is transfered, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_DTD_REQ_WRT_Done(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear 3D_DTD_REQ_WRT from output queue.\n"));

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When REQ_WRT is transfered, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_VIC_REQ_WRT_Done(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear 3D_VIC_REQ_WRT from output queue.\n"));

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When transfer of 3D_DTD is done, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_DTD_DSCR_CHG_Done(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear 3D_DTD_DSCR_CHG from output queue.\n"));

    if (g_u3DstateMachine == STEP_9_3D_DTD_WRT_DTD_DONE)
    {
        //Step 10 of Response to 3D_REQ
        g_u3DstateMachine = STEP_10_3D_DTD_DSCR_CHG_DONE;

        //Tell if all DTD have been sent out.
        if (g_3D_SUPPORT_DTD_VIC_LIST._3D_DTD_Index < g_3D_SUPPORT_DTD_VIC_LIST._3D_DTD_TOT_ENT) //need to send more DTD
        {
            g_u3DstateMachine = STEP_6_3D_DTD_REQ_WRT;
            req.command = MSC_SET_INT;
            req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
            req.msgData[0] = CBUS_INT_BIT_REQ_WRT;
            req.dataRetHandler  = SI_MscWriteBurst3D_DTD_REQ_WRT_Done;
            req.sRetHandlerName = "SI_MscWriteBurst3D_DTD_REQ_WRT_Done";
            req.retry = 3;

            // push REQ_WRT command to out queue.
            SI_CbusPushReqInOQ( channel, &req, true );
            DEBUG_PRINT(MSG_DBG, ("CBUS:: Push 3D_DTD_REQ_WRT into output queue.\n"));
        }
        else
        {
            g_u3DstateMachine = STEP_0_3D_IDLE;
            DEBUG_PRINT(MSG_DBG, ("CBUS:: Response to 3D_REQ is done.\n"));
			g_3D_WB_Processing = false;
			fgSil_NTF_3D_Communicated();
        }
    }
    else
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: g_u3DstateMachine is wrong(0x%0x).\n", (int)g_u3DstateMachine));
    }

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When transfer of 3D_DTD is done, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_VIC_DSCR_CHG_Done(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear DSCR_CHG from output queue.\n"));

    if (g_u3DstateMachine == STEP_4_3D_VIC_WRT_VIC_DONE)
    {
        //Step 5 of Response to 3D_REQ
        g_u3DstateMachine = STEP_5_3D_VIC_DSCR_CHG_DONE;

        //Tell if all VICs have been sent out.
        if (g_3D_SUPPORT_DTD_VIC_LIST._3D_VIC_Index < g_3D_SUPPORT_DTD_VIC_LIST._3D_VIC_TOT_ENT) //need to send more VICs
        {
            g_u3DstateMachine = STEP_1_3D_VIC_REQ_WRT;
            req.command = MSC_SET_INT;
            req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
            req.msgData[0] = CBUS_INT_BIT_REQ_WRT;
            req.dataRetHandler  = SI_MscWriteBurst3D_VIC_REQ_WRT_Done;
            req.sRetHandlerName = "SI_MscWriteBurst3D_VIC_REQ_WRT_Done";
            req.retry = 3;

            // push REQ_WRT command to out queue.
            SI_CbusPushReqInOQ( channel, &req, true );
            DEBUG_PRINT(MSG_DBG, ("CBUS:: Push 3D_VIC_REQ_WRT into output queue.\n"));
        }
        else //all VICs are sent out.
        {
            g_u3DstateMachine = STEP_6_3D_DTD_REQ_WRT;

            req.command = MSC_SET_INT;
            req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
            req.msgData[0] = CBUS_INT_BIT_REQ_WRT;
            req.dataRetHandler  = SI_MscWriteBurst3D_DTD_REQ_WRT_Done;
            req.sRetHandlerName = "SI_MscWriteBurst3D_DTD_REQ_WRT_Done";
            req.retry = 3;

            // push REQ_WRT command to out queue.
            SI_CbusPushReqInOQ( channel, &req, true );
            DEBUG_PRINT(MSG_DBG, ("CBUS:: Push 3D_DTD_REQ_WRT into output queue.\n"));
        }
    }
    else
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: g_u3DstateMachine is wrong(0x%0x).\n", (int)g_u3DstateMachine));
    }

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When transfer of 3D_DTD is done, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_3D_DTD_Done(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear 3D_DTD from output queue.\n"));

    //Step 9 of Response to 3D_REQ
    //Send DSCR_CHG
    g_u3DstateMachine = STEP_9_3D_DTD_WRT_DTD_DONE;
    req.command = MSC_SET_INT;
    req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
    req.msgData[0] = CBUS_INT_BIT_DSCR_CHG;
    req.dataRetHandler  = SI_MscWriteBurst3D_DTD_DSCR_CHG_Done;
    req.sRetHandlerName = "SI_MscWriteBurst3D_DTD_DSCR_CHG_Done";
    req.retry = 3;

    // push REQ_WRT command to out queue.
    SI_CbusPushReqInOQ( channel, &req, true );
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Push DSCR_CHG into output queue.\n"));

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send 3D info. When transfer of 3D_VIC is done, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst3D_3D_VIC_Done(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusOQCleanActiveReq(channel);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear 3D_VIC from output queue.\n"));

    //Step 4 of Response to 3D_REQ
    //Send DSCR_CHG
    g_u3DstateMachine = STEP_4_3D_VIC_WRT_VIC_DONE;
    req.command = MSC_SET_INT;
    req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
    req.msgData[0] = CBUS_INT_BIT_DSCR_CHG;
    req.dataRetHandler  = SI_MscWriteBurst3D_VIC_DSCR_CHG_Done;
    req.sRetHandlerName = "SI_MscWriteBurst3D_VIC_DSCR_CHG_Done";
    req.retry = 3;

    // push REQ_WRT command to out queue.
    SI_CbusPushReqInOQ( channel, &req, true );
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Push DSCR_CHG into output queue.\n"));

    return (true);
}

/*****************************************************************************/
/**
 *  @The function of send Scratchpad info. When transfer of scratchpad data is done, it will be invoked.
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscWriteBurst_Done(uint8_t channel)
{
    cbus_out_queue_t req;

//    SI_CbusOQCleanActiveReq(channel);
//    DEBUG_PRINT(MSG_DBG, ("CBUS:: Clear scratchpad data from output queue.\n"));

    //g_u3DstateMachine = STEP_4_3D_VIC_WRT_VIC_DONE;
    req.command = MSC_SET_INT;
    req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
    req.msgData[0] = CBUS_INT_BIT_DSCR_CHG;
    req.dataRetHandler  = SI_WriteBurstDataDoneDummy;
    req.sRetHandlerName = "SI_MscWriteBurst_Done";
    req.retry = 3;

    // push DSCR_CHG command to out queue.
    SI_CbusPushReqInOQ( channel, &req, true );
    DEBUG_PRINT(MSG_DBG, ("CBUS:: Push DSCR_CHG into output queue.\n"));

    return (true);
}
/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: WRITE_STAT, Path enable
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscPathEnable(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusSetPathEnable(channel, true);
    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: WRITE_STAT, Path enable
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscPathDisable(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusSetPathEnable(channel, false);
    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: SET_INT, DCAP_CHG
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI9292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscDevCapChange(uint8_t channel, uint8_t uData)
{
    uData = uData;

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: WRITE_STAT, Connect Ready
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscConnectReady(uint8_t channel, uint8_t uData)
{
    cbus_out_queue_t req;

    uData = uData;

    SI_CbusOQCleanActiveReq(channel);

    //after send DCAP_RDY, send DCAP_CHG to inform source to read devcap , tiger qin 08-10-2011

    req.command = MSC_SET_INT;
    req.offsetData = MHL_REG_CHANGE_INT_OFFSET;
    req.msgData[0] = CBUS_INT_BIT_DCAP_CHG;
    req.dataRetHandler  = SI_MscDevCapChange;
    req.sRetHandlerName = "SI_MscDevCapChange";

    req.retry = 3;  // retry 3 times if timeout or abort for important MSC commands

    SI_CbusPushReqInOQ( channel, &req, true );
    DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: Self Device Capability Registers Change\n"));
    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, bandwidth info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadBandwidth(uint8_t channel, uint8_t uData)
{
    uint32_t frequency;

    //	uint8_t  tmp = 0 ;
    PeerDevCap[9] = uData;

       //for warning free
       UNUSED(frequency);
       
    if (uData == 0)
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: No Bandwidth is indicated\n"));
    }
    else
    {
        frequency = uData * 5;
		DEBUG_PRINT(MSG_ALWAYS, ("CBUS:: MHL Source Device Info: Bandwidth: %dMHz\n", (int)frequency));		
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);


    return (true);
}

bool_t SI_MscReadDeviceID_H(uint8_t channel, uint8_t uData)
{
    PeerDevCap[11] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Device ID_H 0x%02X.\n", (int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);
    return (true);
}

bool_t SI_MscReadDeviceID_L(uint8_t channel, uint8_t uData)
{
    PeerDevCap[12] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Device ID_L 0x%02X.\n", (int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);
    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, video type info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadVideoType(uint8_t channel, uint8_t uData)
{
    PeerDevCap[7] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Support Video Type: \n"));

    if (uData & SUPP_VT)
    {
        if (uData & VT_GRAPHICS)
        {
            DEBUG_PRINT(MSG_DBG, ("Graphics type of video content; \n"));
        }

        if (uData & VT_PHOTO)
        {
            DEBUG_PRINT(MSG_DBG, ("Photo type of video content; \n"));
        }

        if (uData & VT_CINEMA)
        {
            DEBUG_PRINT(MSG_DBG, ("Cinema type of video content; \n"));
        }

        if (uData & VT_GAME)
        {
            DEBUG_PRINT(MSG_DBG, ("Game type of video content; \n"));
        }
    }
    else
    {
        DEBUG_PRINT(MSG_DBG, ("None\n"));
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, audio link mode info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadAudioLinkMode(uint8_t channel, uint8_t uData)
{
    PeerDevCap[6] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Audio Link Mode: \n"));

    if (uData == 0)
    {
        DEBUG_PRINT(MSG_DBG, ("None\n"));
    }
    else
    {
        if (uData & AUD_2CH)
        {
            DEBUG_PRINT(MSG_DBG, ("2-Channel PCM audio streams (with Layout 0 audio data packets) \n"));
        }

        if (uData & AUD_8CH)
        {
            DEBUG_PRINT(MSG_DBG, ("8-channel PCM audio streams (with Layout 1 audio data packets) \n"));
        }
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, video link mode info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadVideoLinkMode(uint8_t channel, uint8_t uData)
{
    PeerDevCap[5] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Video Link Mode: \n"));

    if (uData == 0)
    {
        DEBUG_PRINT(MSG_DBG, ("None\n"));
    }
    else
    {
        if (uData & SUPP_RGB444)
        {
            DEBUG_PRINT(MSG_DBG, ("RGB 4:4:4 \n"));
        }

        if (uData & SUPP_YCBCR444)
        {
            DEBUG_PRINT(MSG_DBG, ("YCBCR 4:4:4 \n"));
        }

        if (uData & SUPP_YCBCR422)
        {
            DEBUG_PRINT(MSG_DBG, ("YCBCR 4:2:2 \n"));
        }

        if (uData & SUPP_PPIXEL)
        {
            DEBUG_PRINT(MSG_DBG, ("Packed Pixel \n"));
        }

        if (uData & SUPP_ISLANDS)
        {
            DEBUG_PRINT(MSG_DBG, ("Data Islands \n"));
        }
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 
    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, Logical device map info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadLogDevMap(uint8_t channel, uint8_t uData)
{
    PeerDevCap[8] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Log Dev Map: "));

    if (uData == 0)
    {
        DEBUG_PRINT(MSG_DBG, ("None\n"));
    }
    else
    {
        if (uData & LD_DISPLAY)
        {
            DEBUG_PRINT(MSG_DBG, ("Display \n"));
        }

        if (uData & LD_VIDEO)
        {
            DEBUG_PRINT(MSG_DBG, ("Video \n"));
        }

        if (uData & LD_AUDIO)
        {
            DEBUG_PRINT(MSG_DBG, ("Audio \n"));
        }

        if (uData & LD_MEDIA)
        {
            DEBUG_PRINT(MSG_DBG, ("Media \n"));
        }

        if (uData & LD_TUNER)
        {
            DEBUG_PRINT(MSG_DBG, ("Tuner \n"));
        }

        if (uData & LD_RECORD)
        {
            DEBUG_PRINT(MSG_DBG, ("Record \n"));
        }

        if (uData & LD_SPEAKER)
        {
            DEBUG_PRINT(MSG_DBG, ("Speaker \n"));
        }

        if (uData & LD_GUI)
        {
            DEBUG_PRINT(MSG_DBG, ("Gui \n"));
        }
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, device category info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadDevCat(uint8_t channel, uint8_t uData)
{
	uint8_t DEV_TYPE = 0;
	bool_t POW = (bool_t)0;
	uint8_t PLIM = 0;

    PeerDevCap[2] = uData;
	DEV_TYPE = uData & 0x0F;

	if ((uData & 0x10) == 0x10)
	{
		POW = (bool_t)1;
	}
	else
	{
		POW = (bool_t)0;
	}

	PLIM = uData & 0x60;

    switch (DEV_TYPE)
    {
    case NOT_ALLOWED:
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: DEV_TYPE:: NOT_ALLOWED, "));
        break;

    case SINK_DEV:
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: DEV_TYPE:: SINK_DEV, "));
        break;

    case SOURCE_DEV:
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: DEV_TYPE:: SOURCE_DEV, "));
        break;

    case DONGLE_DEV:
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: DEV_TYPE:: DONGLE_DEV, "));
        break;

    default:
        DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: DEV_TYPE:: INCORRECT DEVICE, "));
        break;
    }

    switch (POW)
    {
    case POW_YES:
        DEBUG_PRINT(MSG_DBG, ("POW:: YES, "));
        break;

    default:
        DEBUG_PRINT(MSG_DBG, ("POW:: NO, "));
        break;
    }

    switch (PLIM)
    {
    case PLIM_500mA:
        DEBUG_PRINT(MSG_DBG, ("PLIM:: 500mA\n"));
        break;

    case PLIM_900mA:
        DEBUG_PRINT(MSG_DBG, ("PLIM:: 900mA\n"));
        break;

    case PLIM_1500mA:
        DEBUG_PRINT(MSG_DBG, ("PLIM:: 1500mA\n"));
        break;

    default:
        DEBUG_PRINT(MSG_DBG, ("PLIM:: Reserved\n"));
        break;
    }

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);
    return (true);
}


bool_t SI_MscReadAdopterID_H(uint8_t channel, uint8_t uData)
{
    PeerDevCap[3] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Adopter ID_H 0x%02X.\n", (int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);
    return (true);
}

bool_t SI_MscReadAdopterID_L(uint8_t channel, uint8_t uData)
{
    PeerDevCap[4] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Adopter ID_L 0x%02X.\n", (int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);
    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, MHL version info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadMhlVersion(uint8_t channel, uint8_t uData)
{
    uint8_t mhl_major, mhl_minor;
       //for warning free
       UNUSED(mhl_major);
       UNUSED(mhl_minor);

    PeerDevCap[1] = uData;

    mhl_major = (uData & 0xF0) >> 4;
    mhl_minor = uData & 0x0F;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: MHL Version %02X.%02X\n", (int)mhl_major, (int)mhl_minor));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, feature flag info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadFeatureFlag(uint8_t channel, uint8_t uData)
{
    //	uint8_t result;
    //	cbus_out_queue_t req;
	bool_t rcp_support, rap_support;

    PeerDevCap[10] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Feature: RCP Support: %s\n", (uData
                                                                                           & MHL_RCP_SUPPORT) ? "Yes" : "No"));
    rcp_support = (uData & RCP_SUPPORT) ? true : false;
    SI_CbusSetRcpSupport(channel, rcp_support);
    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Feature: RAP Support: %s\n", (uData
                                                                                           & MHL_RAP_SUPPORT) ? "Yes" : "No"));
    rap_support = (uData & RAP_SUPPORT) ? true : false;
    SI_CbusSetRapSupport(channel, rap_support);


    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING)); 

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, device state info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadDeviceState(uint8_t channel, uint8_t uData)
{
    PeerDevCap[0] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Device State:: 0x%02X\n",(int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING));

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, scratchpad size info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadScratchpadSize(uint8_t channel, uint8_t uData)
{
    PeerDevCap[13] = uData;

    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Scratchpad Size:: 0x%02X\n",(int)uData));

    SI_CbusSetReadInfo(channel, ((SI_CbusGetReadInfo(channel) & DCAP_ITEM_INDEX_MASK) + 1) & (~DCAP_ITEM_READ_DOING));

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus MSC command transfer done handler: MSC command: READ_DEVCAP, interrput state size info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *  @param[in]          uData			The return data of the command when transfer done
 *
 *  @return             Result
 *  @retval             true, Success
 *  @retval             false, Failure
 *
 *****************************************************************************/
bool_t SI_MscReadIntStatSize(uint8_t channel, uint8_t uData)
{
	uint8_t INT_SIZE, STAT_SIZE;
	
    PeerDevCap[14] = uData;
	INT_SIZE = (uData & 0x0F);
	STAT_SIZE = (uData & 0xF0) >> 4;
	
    DEBUG_PRINT(MSG_DBG, ("CBUS:: MHL Source Device Info: Interrput Size:: 0x%02X ; State Size:: 0x%02X\n",(int)INT_SIZE, (int)STAT_SIZE));

    SI_CbusSetReadInfo(channel, DCAP_READ_DONE); //finish read

	SI_MscDevCapAPIStateUpdate();

    SI_CbusOQCleanActiveReq(channel);

    return (true);
}

/*****************************************************************************/

/**
 *  Cbus operation: start to get the peer's device cap registers' info
 *
 *
 *  @param[in]          channel		Cbus channel, always 0 in SiI1292
 *
 *  @return             void
 *
 *****************************************************************************/
typedef struct tagDevcapRead
{
    uint8_t          addressoffset;
    dataRetHandler_t dataRetHandler;
    uint8_t *        sRetHandlerName;
} DevcapReadItem_t;

DevcapReadItem_t DevcapReadArray[] =
{
    {MHL_DEV_CAP_DEV_STATE_REG_OFFSET,   SI_MscReadDeviceState,	"SI_MscReadDeviceState"   },//offset = 0x00
    {MHL_DEV_CAP_MHL_VERSION_REG_OFFSET,   SI_MscReadMhlVersion,	"SI_MscReadMhlVersion"   },//offset = 0x01
    {MHL_DEV_CAP_DEV_CAT_REG_OFFSET,       SI_MscReadDevCat,		"SI_MscReadDevCat"       },//offset = 0x02
	{MHL_DEV_CAP_ADOPTER_IDH_REG_OFFSET,	   SI_MscReadAdopterID_H,		"SI_MscReadAdapterID_H"		 },//offset = 0x03
	{MHL_DEV_CAP_ADOPTER_IDL_REG_OFFSET,	   SI_MscReadAdopterID_L,		"SI_MscReadAdapterID_L"		 },//offset = 0x04
    {MHL_DEV_CAP_VID_LINK_MODE_REG_OFFSET, SI_MscReadVideoLinkMode, "SI_MscReadVideoLinkMode"},//offset = 0x05
    {MHL_DEV_CAP_AUD_LINK_MODE_REG_OFFSET, SI_MscReadAudioLinkMode, "SI_MscReadAudioLinkMode"},//offset = 0x06
    {MHL_DEV_CAP_VIDEO_TYPE_REG_OFFSET,    SI_MscReadVideoType,		"SI_MscReadVideoType"    },//offset = 0x07
	{MHL_DEV_CAP_LOG_DEV_MAP_REG_OFFSET,   SI_MscReadLogDevMap, 	"SI_MscReadLogDevMap"	 },//offset = 0x08
    {MHL_DEV_CAP_BANDWIDTH_REG_OFFSET,     SI_MscReadBandwidth,		"SI_MscReadBandwidth"    },//offset = 0x09
	{MHL_DEV_CAP_FEATURE_FLAG_REG_OFFSET,  SI_MscReadFeatureFlag,	"SI_MscReadFeatureFlag"  },//offset = 0x0A
	{MHL_DEV_CAP_DEVICE_IDH_REG_OFFSET,    SI_MscReadDeviceID_H,		"SI_MscReadAdapterID_H" 	 },//offset = 0x0B
	{MHL_DEV_CAP_DEVICE_IDL_REG_OFFSET,    SI_MscReadDeviceID_L,		"SI_MscReadAdapterID_L" 	 },//offset = 0x0C
	{MHL_DEV_CAP_Scratchpad_Size_REG_OFFSET,    SI_MscReadScratchpadSize,		"SI_MscReadScratchpadSize" 	 },//offset = 0x0D
	{MHL_DEV_CAP_IntStat_Size_REG_OFFSET,    SI_MscReadIntStatSize,		"SI_MscReadIntStatSize" 	 },//offset = 0x0E
};

#define DevcapReadItemCount (sizeof(DevcapReadArray) / sizeof(DevcapReadArray[0]))

void SI_MscDevCapAPIStateUpdate(void)
{
	HDMI_SIL_DevCap_STATUS_T pReg;

	pReg.DevCapState_DEVICE_STATE = 1; 
	pReg.DevCapState_MHL_VERSION = 1; 
	pReg.DevCapState_MHL_DEVICE_CATEGORY = 1; 
	pReg.DevCapState_ADOPTER_ID_H = 1; 
	pReg.DevCapState_ADOPTER_ID_L = 1; 
	pReg.DevCapState_VID_LINK_MODE = 1; 
	pReg.DevCapState_AUD_LINK_MODE = 1; 
	pReg.DevCapState_VIDEO_TYPE = 1; 

	pReg.DevCapState_LOG_DEV_MAP = 1; 
	pReg.DevCapState_BANDWIDTH = 1; 
	pReg.DevCapState_FEATURE_FLAG = 1; 
	pReg.DevCapState_DEVICE_ID_H = 1; 
	pReg.DevCapState_DEVICE_ID_L = 1; 
	pReg.DevCapState_SCRATCHPAD_SIZE = 1; 
	pReg.DevCapState_INT_STAT_SIZE = 1; 
	pReg.DevCapState_Reserved_for_Future_Use = 0; 

	pReg.DevCapValue_DEVICE_STATE = PeerDevCap[0];
	pReg.DevCapValue_MHL_VERSION = PeerDevCap[1]; 
	pReg.DevCapValue_MHL_DEVICE_CATEGORY = PeerDevCap[2]; 
	pReg.DevCapValue_ADOPTER_ID_H = PeerDevCap[3]; 
	pReg.DevCapValue_ADOPTER_ID_L = PeerDevCap[4]; 
	pReg.DevCapValue_VID_LINK_MODE = PeerDevCap[5]; 
	pReg.DevCapValue_AUD_LINK_MODE = PeerDevCap[6]; 
	pReg.DevCapValue_VIDEO_TYPE = PeerDevCap[7]; 
	pReg.DevCapValue_LOG_DEV_MAP = PeerDevCap[8]; 
	pReg.DevCapValue_BANDWIDTH = PeerDevCap[9]; 
	pReg.DevCapValue_FEATURE_FLAG = PeerDevCap[10]; 
	pReg.DevCapValue_DEVICE_ID_H = PeerDevCap[11]; 
	pReg.DevCapValue_DEVICE_ID_L = PeerDevCap[12]; 
	pReg.DevCapValue_SCRATCHPAD_SIZE = PeerDevCap[13];
	pReg.DevCapValue_INT_STAT_SIZE = PeerDevCap[14]; 
	pReg.DevCapValue_Reserved_for_Future_Use = PeerDevCap[15]; 

	DEBUG_PRINT(MSG_DBG, ("Cbus:: SI_MscDevCapInfoUpdate:[ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] [ %02X] \n",
		(int)pReg.DevCapValue_DEVICE_STATE, (int)pReg.DevCapValue_MHL_VERSION, (int)pReg.DevCapValue_MHL_DEVICE_CATEGORY, (int)pReg.DevCapValue_ADOPTER_ID_H,
		(int)pReg.DevCapValue_ADOPTER_ID_L, (int)pReg.DevCapValue_VID_LINK_MODE, (int)pReg.DevCapValue_AUD_LINK_MODE, (int)pReg.DevCapValue_VIDEO_TYPE,
		(int)pReg.DevCapValue_LOG_DEV_MAP, (int)pReg.DevCapValue_BANDWIDTH, (int)pReg.DevCapValue_FEATURE_FLAG, (int)pReg.DevCapValue_DEVICE_ID_H,
		(int)pReg.DevCapValue_DEVICE_ID_L, (int)pReg.DevCapValue_SCRATCHPAD_SIZE, (int)pReg.DevCapValue_INT_STAT_SIZE, (int)pReg.DevCapValue_Reserved_for_Future_Use)); 

		
	fgSil_NTFDevCap(&pReg);
}

void SI_MscStartGetDevInfo(uint8_t channel)
{
    uint8_t result;
    cbus_out_queue_t req;
    uint8_t index, read_info;

    read_info = SI_CbusGetReadInfo(channel);
    index = read_info & DCAP_ITEM_INDEX_MASK;
    read_info &= (~DCAP_ITEM_INDEX_MASK);

    if (read_info & DCAP_READ_DONE)
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: DCAP_READ_DONE \n"));
        return;
    }

    if (read_info & DCAP_ITEM_READ_DOING)
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: current DCAP READ ITEM not done yet\n"));
        return;
    }

    if (index >= DevcapReadItemCount)
    {
        DEBUG_PRINT(MSG_DBG, ("CBUS:: index error \n"));
        return;
    }

    DEBUG_PRINT(MSG_DBG, ("CBUS:: Read MHL Source Device infomation\n"));
    DEBUG_PRINT(MSG_DBG, ("CBUS:: current_item_index = %d\n", (int)index));

    //    DEBUG_PRINT(MSG_DBG, ("CBUS:: DevcapReadItemCount = %d\n",(int)DevcapReadItemCount));

    req.command = MSC_READ_DEVCAP;
    req.offsetData = DevcapReadArray[index].addressoffset;
    req.dataRetHandler  = DevcapReadArray[index].dataRetHandler;
    req.sRetHandlerName = DevcapReadArray[index].sRetHandlerName;

    req.retry = 3;  // retry 3 times if timeout or abort for important MSC commands
    result = SI_CbusPushReqInOQ( channel, &req, true );     // make sure the first Read_Dev_Cap can be put in the queue

    if (result != SUCCESS)
    {
        DEBUG_PRINT(MSG_DBG, ("Cbus:: SI_MscStartGetDevInfo, Error found in SI_CbusPushReqInOQ: %02X\n", (int)result));
    }
    else
    {
        SI_CbusSetReadInfo(channel, index | DCAP_ITEM_READ_DOING );
    }
}

bool_t	SI_WriteBurstDataDone(uint8_t channel, uint8_t dummy)
{
    DEBUG_PRINT(MSG_DBG, ("CBUS:: WriteBurstDataDone\n"));
    SI_CbusOQCleanActiveReq(channel);
    return true;
}

