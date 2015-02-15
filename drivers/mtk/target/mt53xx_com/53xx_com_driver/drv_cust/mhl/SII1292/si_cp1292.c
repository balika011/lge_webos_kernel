//***************************************************************************
//!file     si_cp1292.c
//!brief    CP 1292 FPGA/Starter Kit firmware main module.
//
// No part of this work may be reproduced, modified, distributed,
// transmitted, transcribed, or translated into any language or computer
// format, in any form or by any means without written permission of
// Silicon Image, Inc., 1060 East Arques Avenue, Sunnyvale, California 94085
//
// Copyright 2009-2010, Silicon Image, Inc.  All rights reserved.
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
// Module data
//------------------------------------------------------------------------------
void SI_3DConfig_For_3D_Not_Supported(void);



//------------------------------------------------------------------------------
// Global Data
//------------------------------------------------------------------------------

SI_CP1292Config_t g_data;


uint8_t g_currentInputMode = MHL;

bool_t g_MHL_Source_Connected = false;

bool_t g_HDMI_Source_Connected = false;
bool_t g_HDMI_Source_Valid = false;

bool_t g_HPD_IN  = false;
bool_t g_TX_RSEN = false;
bool_t g_TX_RSEN_Valid = false;
bool_t g_HDMI_Source_Existence = false;

uint8_t PeerDevCap[16] = {0};

extern bool_t      SI_CbusInitialize( void );
extern void 		SI_CbusInitParam( uint8_t channel );

uint8_t SiI_RAP_Status = RAP_NOERROR;
#ifdef DETECT_OCP
extern uint8_t GPIO_uData_LastStatus;
extern uint8_t GPIO_uData_CurrentStatus;
#endif

BOOL g_Support_MHL2 = 0;
BOOL g_Support_Charger_900mA = 0;
bool_t g_Support_ThreeD = (bool_t)0;

//kh, add 2 functions (SiiWriteBurstData, SI_MscWriteBurst_Done) when customer wants to use WB do something.
bool_t g_Non3D_WB_Processing = (bool_t)0;
bool_t g_3D_WB_Processing = (bool_t)0;
static BOOL   g_siI1292_main_loop = TRUE;

BOOL fgSil_Get_Chip_ID(HDMI_SIL_FW_VER_T *prSilFwVer)
{
    uint16_t    temp;

	prSilFwVer->major = 1;
	temp = ((uint16_t)SI_DeviceStatus( SI_DEV_IDH)) << 8;
	temp |= SI_DeviceStatus( SI_DEV_IDL );
	DEBUG_PRINT( MSG_ALWAYS,( "Silicon Image Device: %04X, rev %02X\n\n", temp, (int)(SI_DeviceStatus( SI_DEV_REV ) & 0x0F) ));
	return 1;
}

//------------------------------------------------------------------------------
// Function:    fgSil_Get_FW_Version
// Description: Initialize the 1292 deviice
//------------------------------------------------------------------------------

BOOL fgSil_Get_FW_Version(HDMI_SIL_FW_VER_T *prSilVersion)
{
	prSilVersion->major = 0;
	prSilVersion->second = 0;
	prSilVersion->third = 3;	

	DEBUG_PRINT( MSG_ALWAYS,( "Silicon Image SW Customer Version: %02X.%x%x\n\n", (int)prSilVersion->major, (int)prSilVersion->second, (int)prSilVersion->third));
	return 1;
}

//------------------------------------------------------------------------------
// Function:    fgSil_Set_MHL2_State
// Description: when system initially, SoC calls fgSil_Set_MHL2 for decision  DUT supports MHL 2.0 or MHL 1.2, and what kinds of charger support.
// Param [in]:
//		MHL2_Support = 1: support MHL2 ; MHL2_Support = 0: support MHL 1.2
//		Support_Charger_900mA_Or_1500mA = 1: support charger 900mA ; Support_Charger_900mA_Or_1500mA = 0: support charger 1500mA
// Return Value: 0
// Note: if MHL2_Support = 0, Support_Charger_900mA_Or_1500mA will be ignored for support 500mA only.
//------------------------------------------------------------------------------

BOOL fgSil_Set_MHL2_State( BOOL MHL2_Support,BOOL Support_Charger_900mA_Or_1500mA)
{
	g_Support_MHL2 = MHL2_Support;
	g_Support_Charger_900mA = Support_Charger_900mA_Or_1500mA;// 1 = 900mA ; 0 = 1500mA
	if(g_Support_MHL2)
	{
		DEBUG_PRINT(MSG_ALWAYS,( "\n[SIMG] Support MHL2.0\n"));
		if(g_Support_Charger_900mA)DEBUG_PRINT(MSG_ALWAYS,( "\n[SIMG] Support MHL2.0 900mA/1500mA = %d (1 = 900mA ; 0 = 1500mA)\n", (BOOL)Support_Charger_900mA_Or_1500mA));
	}
	else
	{
		DEBUG_PRINT(MSG_ALWAYS,( "\n[SIMG] Support MHL1.2\n"));
	}

	return 1;
}

//------------------------------------------------------------------------------
// Function:    fgSil_Set_3D_State
// Description: when system initially, SoC calls fgSil_Set_3D_State for desiction DUT supports 3D or not.
//------------------------------------------------------------------------------

BOOL fgSil_Set_3D_State( BOOL support)
{
	g_Support_ThreeD = (bool_t)support;

	if(g_Support_ThreeD)
	{
		DEBUG_PRINT(MSG_ALWAYS,( "\n[SIMG] Support MHL2.0 3D Function, SI_3D_EDID_Configuration_VIC_DTD() shall be called.\n"));
	}
	if(!g_Support_ThreeD)
	{
		SI_3DConfig_For_3D_Not_Supported();
		DEBUG_PRINT(MSG_ALWAYS,( "\n[SIMG] Support 2D Function\n"));
	}
	return 1;
}

BOOL fgSil_RecoveryMHLCable(void)
{
	//Vbus enabled/disabled depends on cable kind.
    CpCableDetect();

	//Deleted by zhangyue on 2013-03-04 for meeting sony's power seq start
	//#ifdef VbusCtrl_By_SoC
	//fgSil_NTF_VBUS_High();
	//#endif
	//Deleted by zhangyue on 2013-03-04 for meeting sony's power seq end

	//Termination Enable
	SiIRegioModify(0x070, 0x8B, 0x08);

	//Cbus to default 1kohm
	SiIRegioModify(0x087, 0x0C, 0x00);
	SiIRegioModify(0x076, 0x0C, 0x00);
	SiIRegioModify(0x073, 0x0C, 0x00);

	return (true);
}


//------------------------------------------------------------------------------
// Function:    BoardInit
// Description: One time initialization at startup
//------------------------------------------------------------------------------

BOOL fgSil_Init(void)
{


	
    /* Power up the chip, check it's type, and initialize to default values.    */

	//Silicon Image driver will start to implement.
	g_siI1292_main_loop = TRUE;
	
 	//added by zhangyue on 2013-03-05 for meeting sony's power spec start 
	fgSil_RecoveryMHLCable();
 	//added by zhangyue on 2013-03-05 for meeting sony's power spec end
	
	CpCheckOTPRev();						// check OTP Rev and update the registers
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
	g_halMsgLevel       = 0xff;

//	DEBUG_PRINT(MSG_ALWAYS,( "\nsupport detection wake up pulse function for follow CTS1.1\n"));
	//SiIRegioModify(REG_GPIO_CTRL, BIT_GPIO0_OUTPUT_DISABLE, BIT_GPIO0_OUTPUT_DISABLE);
	//SiIRegioModify(REG_TX_SWING1, BIT_SWCTRL_EN, 0);

	SI_CbusInitialize();

	/* Initialize MSC variables of all channels.    */
	SI_CbusInitParam(0);

#ifdef DETECT_OCP
	//initially procession to check VBUS over current or not.
	
	//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec start
	#if 1
	fgSil_GetOCPStatus(0);
	#else
	SiIRegioRead(REG_GPIO_CTRL, &GPIO_uData_CurrentStatus);
		GPIO_uData_CurrentStatus &= BIT_GPIO0_INPUT;
	if(GPIO_uData_CurrentStatus == 0x00)fgSil_NFYOCPStatus(0, GPIO_uData_CurrentStatus);
		GPIO_uData_LastStatus = GPIO_uData_CurrentStatus;
	#endif
	//modified by zhangyue on 2013-03-21 for meeting sony's OCP spec end
	
#endif

	return (true);
}

BOOL fgSil_MHLSignalControl(uint8_t HiZ, uint8_t TermOff, uint8_t VbusDisable)
{
	if (HiZ) {
	    //Cbus Hi-z
	    SiIRegioModify(0x073, 0x0C, 0x0C);
	    SiIRegioModify(0x076, 0x0C, 0x0C);
	    SiIRegioModify(0x087, 0x0C, 0x0C);
	}else{
	    //Cbus to default 1kohm
	    SiIRegioModify(0x087, 0x0C, 0x00);
	    SiIRegioModify(0x076, 0x0C, 0x00);
	    SiIRegioModify(0x073, 0x0C, 0x00);
	}

   if (TermOff) {
		//Termination Disable
		SiIRegioModify(0x070, 0x8B, 0x03);
   }else{
		//Termination Enable
		SiIRegioModify(0x070, 0x8B, 0x08);
   }

	if(VbusDisable)
	{
		vPsCtrl(VBUS_OFF);
	} else {
		vPsCtrl(VBUS_ON);
	}

	return (true);
}

void MHL_toggle_500ms(void)
{
	int8_t i = 0;

	//Cbus Hi-z
	SiIRegioModify(0x073, 0x0C, 0x0C);
	SiIRegioModify(0x076, 0x0C, 0x0C);
	SiIRegioModify(0x087, 0x0C, 0x0C);

	//Termination Disable
	SiIRegioModify(0x070, 0x8B, 0x83);

	vSil_Delay1ms(20);	

	//Vbus disable, waiting
	for(i = 0 ; i <= 10; i ++)//PAS_CTRL# delay 550ms low
	{
		vPsCtrl(VBUS_OFF);
		vSil_Delay1ms(50);   //11x50=550ms
	}

	fgSil_RecoveryMHLCable();
}

void fgSil_PrintAllRegs(void)
{
	uint8_t temp;
	uint16_t i;
	DEBUG_PRINT(MSG_ALWAYS, ("\n\nPage0, Address0xD0 (CI2CA = LOW), Addess0xD4 (CI2CA = HIGH)\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("-----------------------------------------------------------------------\n", 0));
	for(i=0x0000;i<0x0100;i++)
	{
		if((i == 0x00)||(i == 0x10)||(i == 0x20)||(i == 0x30)||(i == 0x40)||(i == 0x50)||(i == 0x60)||(i == 0x70)||
		(i == 0x80)||(i == 0x90)||(i == 0xA0)||(i == 0xB0)||(i == 0xC0)||(i == 0xD0)||(i == 0xE0)||(i == 0xF0))DEBUG_PRINT(MSG_ALWAYS, ("%02X:  ", (int)i));
		SiIRegioRead(i,&temp);
		DEBUG_PRINT(MSG_ALWAYS, ("%02X  ", (int)temp));
		if((i == 0x0F)||(i == 0x1F)||(i == 0x2F)||(i == 0x3F)||(i == 0x4F)||(i == 0x5F)||(i == 0x6F)||(i == 0x7F)||
		(i == 0x8F)||(i == 0x9F)||(i == 0xAF)||(i == 0xBF)||(i == 0xCF)||(i == 0xDF)||(i == 0xEF)||(i == 0xFF))DEBUG_PRINT(MSG_ALWAYS, ("\n", 0));
	}
	DEBUG_PRINT(MSG_ALWAYS, ("\n\nPage8, Address0xC0 (CI2CA = LOW), Addess0xC4 (CI2CA = HIGH)\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("-----------------------------------------------------------------------\n", 0));
	for(i=0x0800;i<0x0900;i++)
	{
		if((i == 0x0800)||(i == 0x0810)||(i == 0x0820)||(i == 0x0830)||(i == 0x0840)||(i == 0x0850)||(i == 0x0860)||(i == 0x0870)||
		(i == 0x0880)||(i == 0x0890)||(i == 0x08A0)||(i == 0x08B0)||(i == 0x08C0)||(i == 0x08D0)||(i == 0x08E0)||(i == 0x08F0))DEBUG_PRINT(MSG_ALWAYS, ("%02X:  ", (int)i));
		SiIRegioRead(i,&temp);
		DEBUG_PRINT(MSG_ALWAYS, ("%02X  ", (int)temp));
		if((i == 0x080F)||(i == 0x081F)||(i == 0x082F)||(i == 0x083F)||(i == 0x084F)||(i == 0x085F)||(i == 0x086F)||(i == 0x087F)||
		(i == 0x088F)||(i == 0x089F)||(i == 0x08AF)||(i == 0x08BF)||(i == 0x08CF)||(i == 0x08DF)||(i == 0x08EF)||(i == 0x08FF))DEBUG_PRINT(MSG_ALWAYS, ("\n", 0));
	}
	DEBUG_PRINT(MSG_ALWAYS, ("\n\nPageC, Address0xC8 (CI2CA = LOW), Addess0xCC (CI2CA = HIGH)\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F\n", 0));
	DEBUG_PRINT(MSG_ALWAYS, ("-----------------------------------------------------------------------\n", 0));
	for(i=0x0C00;i<0x0D00;i++)
	{
		if((i == 0x0C00)||(i == 0x0C10)||(i == 0x0C20)||(i == 0x0C30)||(i == 0x0C40)||(i == 0x0C50)||(i == 0x0C60)||(i == 0x0C70)||
		(i == 0x0C80)||(i == 0x0C90)||(i == 0x0CA0)||(i == 0x0CB0)||(i == 0x0CC0)||(i == 0x0CD0)||(i == 0x0CE0)||(i == 0x0CF0))DEBUG_PRINT(MSG_ALWAYS, ("%02X:  ", (int)i));
		SiIRegioRead(i,&temp);
		DEBUG_PRINT(MSG_ALWAYS, ("%02X  ", (int)temp));
		if((i == 0x0C0F)||(i == 0x0C1F)||(i == 0x0C2F)||(i == 0x0C3F)||(i == 0x0C4F)||(i == 0x0C5F)||(i == 0x0C6F)||(i == 0x0C7F)||
		(i == 0x0C8F)||(i == 0x0C9F)||(i == 0x0CAF)||(i == 0x0CBF)||(i == 0x0CCF)||(i == 0x0CDF)||(i == 0x0CEF)||(i == 0x0CFF))DEBUG_PRINT(MSG_ALWAYS, ("\n", 0));
	}
}

//------------------------------------------------------------------------------
// Function:    main
// Description: CP 1292 startup and supervisor control loop
//------------------------------------------------------------------------------

void vSil_MainLoop(void)
{

    if(g_siI1292_main_loop == FALSE)
        return;

	SI_DeviceEventMonitor();

	CpCbusHandler();
	

	SiI_RAP_Status = fgSil_GetSystemPortStatus(1);

	//Keno20120302, disable for Customer's RC call fgSil_RcpKeys() for replied >>	CpIrHandler();
}

bool_t fgSiI_GetStatus(HDMI_SIL_STATUS_T *prSilStatus)
{
#if 0
		HDMI_SIL_STATUS_T.SupportEdid = 0;
		HDMI_SIL_STATUS_T.SupportHdcp = 0;
		HDMI_SIL_STATUS_T.SupportMhl = 1;
		HDMI_SIL_STATUS_T.HdcpKeyGood = 0;//not support HDCP function
		HDMI_SIL_STATUS_T.EdidKeyGood = 0;//not support EDID
		if(mhl_con || g_HDMI_Source_Connected)HDMI_SIL_STATUS_T.RxCableDetected = 1;else HDMI_SIL_STATUS_T.RxCableDetected = 0;
		if(g_MHL_Source_Connected || g_HDMI_Source_Valid)HDMI_SIL_STATUS_T.RxSync = 1;else HDMI_SIL_STATUS_T.RxSync = 0;
		HDMI_SIL_STATUS_T.RxStable = 0;//not support RxStable, because 1292 doesn't know MHL signal stable or unstable. it is pass through to HDMI.
#endif
    return(SUCCESS);
}

//------------------------------------------------------------------------------
// Function:    fgSil_MHL_Enable_MLoop
// Description: 
//------------------------------------------------------------------------------
VOID fgSil_MHL_Enable_MLoop(BOOL bEnterMloop)
{
    if(bEnterMloop)
    {
        g_siI1292_main_loop = TRUE;
    }
    else
    {
        g_siI1292_main_loop = FALSE;
    }
}

//------------------------------------------------------------------------------
// Function:    fgSil_Enable_Termination
// Description: 
//------------------------------------------------------------------------------

VOID fgSil_Enable_Termination(BOOL bOn)
{
    if (bOn == 0) {
    //Termination Disable
		SiIRegioModify(0x070, 0x8B, 0x03);
    }else{
    //Termination Enable
		SiIRegioModify(0x070, 0x8B, 0x08);
    }
}

//------------------------------------------------------------------------------
// Function:    fgSil_Enable_Cbus
// Description: 
//------------------------------------------------------------------------------

VOID fgSil_Enable_Cbus(BOOL bOn)
{
	if(bOn){
		//Cbus to default 1kohm
		SiIRegioModify(0x087, 0x0C, 0x00);
		SiIRegioModify(0x076, 0x0C, 0x00);
		SiIRegioModify(0x073, 0x0C, 0x00);
	}else{
		//Cbus to Hi-Z
		SiIRegioModify(0x073, 0x0C, 0x0C);
		SiIRegioModify(0x076, 0x0C, 0x0C);
		SiIRegioModify(0x087, 0x0C, 0x0C);

	}
}
