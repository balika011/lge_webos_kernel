//**************************************************************************

//!file     si_api1292.c
//!brief    SiI1292 API main functions.
//
// Note:    This file supports 1292
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009 -2010, Silicon Image, Inc.  All rights reserved.
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

//------------------------------------------------------------------------------
// Device Event Monitor variables
//------------------------------------------------------------------------------

BOOL g_firstPass = true;
uint32_t g_pass = 0;    // For time counting

static uint8_t  HDMI_connect_wait_count     = 0;	// For starter kiter

extern bool_t mhl_con;
extern bool_t g_HDMI_Source_Existence;

#ifdef DETECT_OCP
//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec start
#if 1
uint8_t GPIO_uData_LastStatus = 0xFF;
uint8_t GPIO_uData_CurrentStatus = 0xFF;
#else
uint8_t GPIO_uData_LastStatus=0;
uint8_t GPIO_uData_CurrentStatus = 0;
#endif
//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec end
uint8_t OCP_Flag=0;//true: OCP event enable ; fales: OCP event disable
#endif

void vPsCtrl(uint8_t value)
{
#ifdef PsCtrl_High
	if ((value == VBUS_ON) && (mhl_con)
#ifdef DETECT_OCP
	&& (OCP_Flag != true)
#endif
	)
	{
#ifdef VbusCtrl_By_SoC
		fgSil_NTF_VBUS_High();
#endif
	   	SiIRegioWrite(0x07A, 0x78); //set PS_CNTRL to high, open drain
	}
	else
	{
#ifdef VbusCtrl_By_SoC
		fgSil_NTF_VBUS_Low();
#endif
	   	SiIRegioWrite(0x07A, 0x48); //set PS_CNTRL to low, push pull
	}
#else
	if ((value == VBUS_ON) && (mhl_con)
#ifdef DETECT_OCP
	&& (OCP_Flag != true)
#endif
	)
	{
#ifdef VbusCtrl_By_SoC
		fgSil_NTF_VBUS_High();
#endif
		SiIRegioWrite(0x07A, 0x48);//set PS_CNTRL to low, push pull
	}
	else
	{
#ifdef VbusCtrl_By_SoC
		fgSil_NTF_VBUS_Low();
#endif
	    SiIRegioWrite(0x07A, 0x78); //set PS_CNTRL to high, open drain
	}
#endif
}

//------------------------------------------------------------------------------
// Function:    SI_DeviceStatus
// Description: Return the requested status from the device.
//------------------------------------------------------------------------------

uint8_t SI_DeviceStatus ( uint8_t statusIndex )
{
    uint8_t regValue;

    switch (statusIndex)
    {
    case SI_DEV_IDH:
		SiIRegioRead( REG_DEV_IDH_RX, &regValue);
        break;
    case SI_DEV_IDL:
		SiIRegioRead( REG_DEV_IDL_RX, &regValue);
        break;
    case SI_DEV_REV:
		SiIRegioRead( REG_DEV_REV, &regValue);
		regValue &= 0x0F;
        break;
    }

    return (regValue);
}

//------------------------------------------------------------------------------
// Function:    GatherDeviceEvents
// Description:
//
//------------------------------------------------------------------------------

static uint8_t GatherDeviceEvents ( void )
{
    uint8_t events = 0;
    uint8_t uData;

	SiIRegioRead( REG_INTR1, &uData);

    if (uData & INT_CBUS_CON)
    {
        events |= MHL_EVENT;
		SiIRegioWrite(REG_INTR1, INT_CBUS_CON);
    }
    else
    {
        if (uData & INT_RPWR)
        {
            //events |= HDMI_EVENT;
            if (!mhl_con)
            {
                events |= HDMI_EVENT;
            }

            //HDMI_connect_wait_count = 1;

			SiIRegioWrite(REG_INTR1, INT_RPWR);
        }
    }

    if (uData & INT_HPD)
    {
        events |= TV_EVENT;
		SiIRegioWrite(REG_INTR1, INT_HPD);
    }

    if (uData & INT_RSEN)
    {
        events |= RSEN_EVENT;
		SiIRegioWrite(REG_INTR1, INT_RSEN);
    }

	if (uData & INT_SCDT)
	{
		//	events |= SCDT_EVENT;


		#ifdef BOARD_JULIPORT	   //for auto reset on SCDT
		//SiIRegioWrite(0x005, 0x28); // Hold CEC in Reset

		if(mhl_con)
			vPsCtrl(VBUS_ON);
		#endif

		#if defined(TMDS_MAPPING_For_Rk1292)
		SiIRegioWrite(0x00C, 0x11); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0B); // DATA_MUX_2
		#elif defined(TMDS_MAPPING_For_Cp1292)
		SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2	
		#else
		SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2
		#endif

		SiIRegioWrite(REG_INTR1, INT_SCDT);
	}
	
	if (uData & INT_CKDT)
	{
		//events |= SCDT_EVENT;
		SiIRegioWrite(REG_INTR1, INT_CKDT);
	}
    return events;
}

static void ProcessMhlEvent (void)
{
    uint8_t uData;

	SiIRegioRead(REG_CBUS_BUS_STATUS, &uData);

    //DEBUG_PRINT(MSG_DBG, ("REG_CBUS_BUS_STATUS = %02X\n", (int)uData));

    if (uData & BIT_BUS_CONNECTED)
    {
        //Set g_currentInputMode to MHL, then let CBUS driver handle everything
        g_currentInputMode = MHL;

        //Enable MHL interrupts
        SiIRegioWrite(REG_INTR1_MASK, 0xFF);
        SiIRegioWrite(REG_CBUS_INTR_ENABLE, 0xFF);

        g_MHL_Source_Connected = true;
        DEBUG_PRINT(MSG_ALWAYS, ("MHL Source device Connected\n"));

        if (HDMI_connect_wait_count)
        {
            HDMI_connect_wait_count = 0;
        }                                   // So it's a MHL device, clean the HDMI event

        // Jin: For Bring-up
		#if (FPGA_BUILD == 0)
        SiIRegioModify(0x073, 0x02, 0x02); // change mode to MHL
        SiIRegioModify(0x071, 0x02, 0x02);
        SiIRegioWrite(0x06C, 0x08);

        //SiIRegioWrite(0x009, 0x0);
		#endif // FPGA_BUILD == 0
		#if defined(DebugMode_1)
		SiIRegioWrite(0x013, 0x01);
		#elif defined(DebugMode_2)
		SiIRegioWrite(0x013, 0x80);
		#endif
		fgSil_NTF_CBUS_Connected();
    }
    else
    {
        g_MHL_Source_Connected = false;
        //DEBUG_PRINT(MSG_ALWAYS, ("MHL Source device Unconnected\n"));		
        fgSil_NTF_CBUS_Disconnected();
    }
}

static void ProcessHdmiEvent (void)
{
    uint8_t uData;
    static bool_t print_connected = false;

	SiIRegioRead(REG_STATE, &uData);

    if (uData & BIT_PWR5V)
    {
        g_HDMI_Source_Connected = true;
        print_connected = true;
        DEBUG_PRINT(MSG_ALWAYS, ("HDMI Source device Connected\n"));
		#if defined(DebugMode_1)
		SiIRegioWrite(0x013, 0x80);
		#elif defined(DebugMode_2)
		SiIRegioWrite(0x013, 0x80);
		#endif
    }
    else
    {
        g_HDMI_Source_Connected = false;
        g_HDMI_Source_Valid = false;
        g_currentInputMode = MHL;
		g_HDMI_Source_Existence = false;

        if (print_connected)
        {
            DEBUG_PRINT(MSG_ALWAYS, ("HDMI Source device Unconnected\n"));
            print_connected = false;
        }

        // Disable HDMI Mode
        //SiIRegioModify(REG_SYS_CTRL1, BIT_HDMI_MODE|BIT_RX_CLK, 0);
        //Jin: bringup in stater kit
        SiIRegioWrite(REG_SYS_CTRL1, 0);

        // Disable Rx Termination Core
        SiIRegioModify(REG_RX_CTRL5, BIT_HDMI_RX_EN, 0);

    }
}

static void ProcessTvEvent (void)
{
    uint8_t uData;

	SiIRegioRead(REG_STATE, &uData);

    //DEBUG_PRINT(MSG_DBG, ("REG_STATE = %02X\n", (int)uData));

	SI_CbusInitDevCap(SI_CbusPortToChannel(0));

    if (uData & BIT_HPD)
    {
        if (g_HPD_IN)
        {
            return;
        }

        g_HPD_IN = true; // true means high

		DEBUG_PRINT(MSG_ALWAYS, ("HDMI Tx HPD Detected - 1\n"));		
	}
    else
    {
        if (!g_HPD_IN)
        {
            return;
        }

        g_HPD_IN = false; // false means low

        DEBUG_PRINT(MSG_ALWAYS, ("HPD Undetected\n"));
    }

    SiIRegioWrite(REG_CBUS_INTR_STATUS, BIT_MSC_MSG_RCV); //tiger , 12-07-2-11, bit3 hardware defualt value is 1,clear it when init;

#if (FPGA_BUILD == 0)
    if (g_currentInputMode == HDMI)
    {
        SiIRegioModify(0x073, 0x02, 0); // change mode to HDMI
        SiIRegioWrite(0x071, 0xA0);
        SiIRegioWrite(0x06C, 0x3F);
        SiIRegioWrite(0x070, 0xA8);
        SiIRegioWrite(0x009, 0x3E);

		vPsCtrl(VBUS_OFF);

#if defined(TMDS_MAPPING_For_Rk1292)
		SiIRegioWrite(0x00C, 0x11); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0B); // DATA_MUX_2
#elif defined(TMDS_MAPPING_For_Cp1292)
		SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2	
#else
        SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
        SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2
#endif
#if defined(DebugMode_1)
			SiIRegioWrite(0x013, 0x80);
#elif defined(DebugMode_2)
			SiIRegioWrite(0x013, 0x80);
#endif	
    }
    else
    {
        SiIRegioModify(0x073, 0x02, 0x02); // change mode to MHL
        SiIRegioWrite(0x071, 0xA2);
        SiIRegioWrite(0x06C, 0x08);
        SiIRegioWrite(0x070, 0x48);
        SiIRegioWrite(0x009, 0x0);
		if(mhl_con)
			vPsCtrl(VBUS_ON);

//		SiIRegioWrite(0x005, 0x28); // Hold CEC in Reset
#if defined(TMDS_MAPPING_For_Rk1292)
		SiIRegioWrite(0x00C, 0x11); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0B); // DATA_MUX_2
#elif defined(TMDS_MAPPING_For_Cp1292)
		SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
		SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2	
#else
        SiIRegioWrite(0x00C, 0x45); // DATA_MUX_1
        SiIRegioWrite(0x00D, 0x0E); // DATA_MUX_2
#endif
    }
#endif // FPGA_BUILD == 0

}

static void ProcessRsenEvent (void)
{
    uint8_t uData;

	SiIRegioRead(REG_STATE2, &uData);

    if (uData)
    {
        g_TX_RSEN = true;
    }
    else
    {
        g_TX_RSEN = false;
        g_TX_RSEN_Valid = false;
    }

}

static void ProcessDeviceEvents ( uint8_t events )
{
    if (events & MHL_EVENT)
    {
        ProcessMhlEvent();
    }

    if (events & HDMI_EVENT)
    {
        ProcessHdmiEvent();
    }

    if (events & TV_EVENT)
    {
        ProcessTvEvent();
    }

    if (events & RSEN_EVENT)
    {
        ProcessRsenEvent();
    }
}

static void TxRsenValidCheck (void)
{
    static bool_t timer_set = false;
    static uint32_t count = 0;
	// for warning free
	UNUSED(count);

    if (g_TX_RSEN)
    {
        if (!timer_set)
        {
            timer_set = true;
            count = timer_count;
        }
        else
        {
			vSil_Delay1ms(DEM_RSEN_VALID_TIME);
            {
                g_TX_RSEN_Valid = true;
                count = 0;
                timer_set = false;
                DEBUG_PRINT(MSG_ALWAYS, ("TX RSEN Stable\n"));
				DEBUG_PRINT(MSG_ALWAYS, ("g_currentInputMode = %d\n",(int)g_currentInputMode));
                if (g_currentInputMode == HDMI)
                {
                    // enable Rx Core termination
                    SiIRegioModify(REG_RX_CTRL5, BIT_HDMI_RX_EN, BIT_HDMI_RX_EN);
                }
#if defined(DebugMode_1)
				SiIRegioWrite(0x013, 0x80);
#elif defined(DebugMode_2)
				SiIRegioWrite(0x013, 0x80);
#endif
				//Keno20120606
				//when AC/DC PwrOn (1292 no pwr in STBY), HPD_IN will low to high.
				//per 100ms to 300ms (DEM_RSEN_VALID_TIME), PS_CTRL# will change to default open drain high 5v and 0xD0:0x7A=0xD4.
				//therefore re-set 0xD0:0x7A again.
				if(mhl_con)
					vPsCtrl(VBUS_ON);
				else 
					vPsCtrl(VBUS_OFF);
            }
        }
    }
    else
    {
        timer_set = false;
        count = 0;
    }
}

static void ClearCbusInterrupts(void)
{
    SiIRegioWrite( REG_CBUS_INTR_STATUS, 0xFF );
    SiIRegioWrite( REG_CBUS_LINK_STATUS_2, 0 );
    SiIRegioWrite( REG_CBUS_REQUESTER_ABORT_REASON, 0xFF );
    SiIRegioWrite( REG_CBUS_RESPONDER_ABORT_REASON, 0xFF );
    SiIRegioWrite( REG_CBUS_DDC_ABORT_REASON, 0xFF );
    SiIRegioWrite( REG_CBUS_CEC_ABORT_REASON, 0xFF );
}

static void HdmiSourceValidCheck (void)
{
    static bool_t timer_set = false;
    static uint32_t count = 0;

	// for warning free
	UNUSED(count);
    if (g_HDMI_Source_Connected)
    {
        if (!timer_set)
        {
            timer_set = true;
            count = timer_count;
        }
        else
        {
        	vSil_Delay1ms(DEM_HDMI_VALID_TIME);
            {
                uint8_t uData;

                g_HDMI_Source_Valid = true;
                g_currentInputMode = HDMI;

                // Jin: Fro Bring-up
#if (FPGA_BUILD == 0)
                SiIRegioModify(0x073, 0x02, 0); // change mode to HDMI
                SiIRegioWrite(0x071, 0xA0);
                SiIRegioWrite(0x06C, 0x3F);
                SiIRegioWrite(0x070, 0xA8);
                SiIRegioWrite(0x009, 0x3E);
#endif             // FPGA_BUILD == 0

                // Disable CBUS interrupt
                SiIRegioWrite(REG_INTR1_MASK, 0xFE);
                SiIRegioWrite(REG_CBUS_INTR_ENABLE, 0);

                // Clear CBUS interrupts
				SiIRegioRead(REG_CBUS_INTR_STATUS, &uData);
                if (uData)
                {
                    ClearCbusInterrupts();
                }

                SiIRegioWrite(REG_INTR1, INT_CBUS);

                count = 0;
                DEBUG_PRINT(MSG_ALWAYS, ("HDMI Source device Stable\n"));


                // Confirm HDMI MODE
                //SiIRegioModify(REG_SYS_CTRL1, BIT_HDMI_MODE|BIT_RX_CLK, BIT_HDMI_MODE|BIT_RX_CLK);
                //Jin: bringup in starter kit
                SiIRegioWrite(REG_SYS_CTRL1, 0xF8);

                // Enable HDMI Rx Core Termination
                if (g_TX_RSEN_Valid)
                {
                	g_HDMI_Source_Existence = true;
                    SiIRegioModify(REG_RX_CTRL5, BIT_HDMI_RX_EN, BIT_HDMI_RX_EN);
                }
            }
        }
		#if defined(DebugMode_1)
		SiIRegioWrite(0x013, 0x80);
		#elif defined(DebugMode_2)
		SiIRegioWrite(0x013, 0x80);
		#endif
    }
    else
    {
        timer_set = false;
        count = 0;  // re-count
    }
}


BOOL fgSil_SetVBus(UINT8 value)//Set PS_CTRL to 0 or 1
{
	//added by zhangyue on 2013-03-21 for meeting the OCP spec start
	OCP_Flag = !value; //if 
	//added by zhangyue on 2013-03-21 for meeting the OCP spec end
	
    vPsCtrl(value);
	return (true);
}

//------------------------------------------------------------------------------
// Function:    SI_DeviceEventMonitor
// Description: Monitors important events in the life of the Main pipe and
//              intervenes where necessary to keep it running clean.
//
// NOTE:        This function is designed to be called at 100 millisecond
//              intervals. If the calling interval is changed, the
//              DEM_POLLING_DELAY value must be changed also to maintain the
//              proper intervals.
//
//------------------------------------------------------------------------------

void SI_DeviceEventMonitor ( void )
{
    uint8_t events = 0;

    //DEBUG_PRINT(MSG_DBG, ("[%d] ", (int)g_pass));
    g_pass++;
    if ((int)g_pass < 0) //if overflow
    {
        g_pass = 0;
    }

    if (g_firstPass)
    {
        BOOL mhl_cable_con = false;

        g_firstPass = false;

        //SiIRegioModify(REG_RX_MISC, BIT_PSCTRL_OEN|BIT_PSCTRL_OUT|BIT_PSCTRL_AUTO, BIT_PSCTRL_OUT);

        //enable all INT mask
		SiIRegioWrite(REG_INTR1_MASK, 0xFF);

#ifdef BOARD_JULIPORT
		//enable mhl cable connect based on  V5DET 0x4E[0] mask at 0x4E[1], if high them MHL cable det.
		SiIRegioWrite(REG_INTR2_STATUS_MASK,0x02);
#endif
		//deleted by zhangyue on 2013-02-26 for meeting sony's power spec start
		//MHL_toggle_500ms();
		//deleted by zhangyue on 2013-02-26 for meeting sony's power spec end

        mhl_cable_con = CpCableDetect();

        // Jin: for INT_RPWR/INT_CBUS doesn't show at first process
        {
			uint8_t	 uData;
			SiIRegioRead(REG_STATE, &uData);

            if (uData & BIT_PWR5V)
            {
                //HDMI_connect_wait_count = 1;
                if (!mhl_con)
                {
                    events |= HDMI_EVENT;
                }
            }

            if (mhl_cable_con)
            {
                if (uData & BIT_HPD)
                {
                    g_HPD_IN = true; // true means high

					DEBUG_PRINT(MSG_ALWAYS, ("HDMI Tx HPD Detected - 2\n"));		
                }
            }

			SiIRegioRead(REG_CBUS_BUS_STATUS, &uData);

            if (uData & BIT_BUS_CONNECTED)
            {
                events |= MHL_EVENT;
            }
        }
    }
#ifdef DETECT_OCP
	//deleted by zhangyue on 2013 for meeting sony's OCP spec start
	#if 0
		SiIRegioRead(REG_GPIO_CTRL, &GPIO_uData_CurrentStatus);
		GPIO_uData_CurrentStatus &= BIT_GPIO0_INPUT;
		if(GPIO_uData_CurrentStatus != GPIO_uData_LastStatus)
		{
			fgSil_NFYOCPStatus(0, GPIO_uData_CurrentStatus);
		}
		GPIO_uData_LastStatus = GPIO_uData_CurrentStatus;
	#endif
	//deleted by zhangyue on 2013 for meeting sony's OCP spec end
#endif

	CpCableDetect();
	
    events |= GatherDeviceEvents();       // Gather info for this pass
    //DEBUG_PRINT(MSG_DBG, ("events = %02X\n", (int)events));

    /*if (HDMI_connect_wait_count)
    {
        if (HDMI_connect_wait_count == 5)	// wait at least 200ms for CBUS discovery
        {
            events |= HDMI_EVENT;
            HDMI_connect_wait_count = 0;
        }
        else
        {
            HDMI_connect_wait_count++;
        }
    }*/

    ProcessDeviceEvents(events);

    // make sure TX_RSEN show 1s
    if (g_TX_RSEN_Valid == false)
    {
        TxRsenValidCheck();
    }

    if (g_HDMI_Source_Valid == false)
    {
        HdmiSourceValidCheck();
    }
}

BOOL fgSil_GetStatus(uint8_t port, HDMI_SIL_STATUS_T *prSilStatus)
{
 //    uint8_t uData = 0;
     
	//byte1
	prSilStatus->SupportEdid = 0;//not support
	prSilStatus->SupportHdcp = 0;//not support
	prSilStatus->SupportMhl = 1;//support
	prSilStatus->HdcpKeyGood = 0;//not support
	prSilStatus->EdidKeyGood= 0;//not support

	//byte2
        //0x01 is MHL/HDMI cable connection, 0x00 is MHL/HDMI cable no connection
	if(mhl_con==1)
		prSilStatus->RxCableDetected=0x01;
	else if (g_HDMI_Source_Connected ==1)
		prSilStatus->RxCableDetected=0x01;
	else
		prSilStatus->RxCableDetected=0x00;

	prSilStatus->RxSync = 0;//not support
	prSilStatus->RxStable = 0;//not support
	prSilStatus->HdcpActive = 0;//not support
	if(g_MHL_Source_Connected==1)
		prSilStatus->MhlHdmiDevice = 0x11;//bit4: existence device or not, bit0: MHL/Hdmi device (MHL:1, HDMI:0)
	else if(g_HDMI_Source_Existence==1)
		prSilStatus->MhlHdmiDevice = 0x10;//bit4: existence device or not, bit0: MHL/Hdmi device (MHL:1, HDMI:0)
	else
		prSilStatus->MhlHdmiDevice = 0x00;
	prSilStatus->HdmiMode = 0;//not support

	//byte3
	//0x01 is Hdmi Tx side cable existence, 0x00 is Hdmi Tx side cable not existence
	if (g_HPD_IN) prSilStatus->TxCableDetected = 1;
	else prSilStatus->TxCableDetected = 0;
	prSilStatus->TxOutputing = 0;//not support
	prSilStatus->TxBlackMute = 0;//not support
	prSilStatus->PowerSaving = 0;//not support

	return(SUCCESS);
}

BOOL fgSil_GetOCPStatus(UINT8 port)
{

	//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec start	
	#if 1
	SiIRegioRead(REG_GPIO_CTRL, &GPIO_uData_CurrentStatus);
	GPIO_uData_CurrentStatus &= BIT_GPIO0_INPUT;
	if(GPIO_uData_CurrentStatus != GPIO_uData_LastStatus)
	{
		fgSil_NFYOCPStatus(0, GPIO_uData_CurrentStatus);
	}
	GPIO_uData_LastStatus = GPIO_uData_CurrentStatus;
	
	return GPIO_uData_CurrentStatus ? false : true;
	#else
 	uint8_t u1buf = 0;
 
    if (GPIO_uData_CurrentStatus & BIT_GPIO0_INPUT) {
        return (true);
    } else {
        SiIRegioRead(REG_GPIO_CTRL, &u1buf);
        return ((u1buf & BIT_GPIO0_INPUT) ? true:false);
    }
	#endif
	//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec end
}
