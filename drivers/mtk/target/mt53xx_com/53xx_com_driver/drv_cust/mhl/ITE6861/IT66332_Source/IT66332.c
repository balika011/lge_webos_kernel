///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT66332.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
//****************************************************************************
#include "../IT66332_Customer/IT66332_IO.h"
#include "../IT66332_Customer/IT66332.h"
#include "HDMI_Switch.h"
#include "HDMI_Rx.h"
#include "HDMI_Tx.h"
#include "EDID.h"
#if USING_CEC
#include "CEC.h"
#endif
#if USING_MHL
#include "MHL_Rx.h"
#endif
//****************************************************************************
//****************************************************************************
iTE_u1 bCh0TermFlag = iTE_FALSE;
iTE_u1 bPwrDnFlag = iTE_FALSE;
ITE_HDMI ucHDMI_Port = DEFAULT_HDMI;//HDMI_REV;
ITE_HDMI ucMHL_Port = DEFAULT_HDMI;//MHL_REV;
iTE_u1	bStandBy = iTE_FALSE;
iTE_u32 fRefClk;
iTE_u8	ucFwFlag;
//****************************************************************************
iTE_u1 IT66332_Detect(void)
{
	iTE_u8	ucVenID[2], ucDevID[3], ucRevID;

	hdmi_sw_br(0x00, 2, ucVenID);
	hdmi_sw_br(0x02, 3, ucDevID);

	hdmi_sw_br(0x00, 2, ucVenID);
	hdmi_sw_br(0x02, 3, ucDevID);

	ucRevID = hdmi_sw_r(0x05);

    iTE_Msg(("###############################################\n"));
    iTE_Msg(("#           			IT66332 Initialization           					#\n"));
    iTE_Msg(("###############################################\n"));

	if( ucVenID[0]!=0x54 || ucVenID[1]!=0x49 ) {	//V1.06_1
         iTE_Msg(("\nERROR: Can not find IT66332B0 HDMISW Device !!!\n"));

         iTE_Msg(("Current VenID=%02X-%02X\n", (int)ucVenID[1], (int)ucVenID[0]));
         iTE_Msg(("Current DevID=%02X-%02X-%02X\n", (int)ucDevID[2], (int)ucDevID[1], (int)ucDevID[0]));
         iTE_Msg(("Current RevID=%02X\n", (int)ucRevID));
         return iTE_FALSE;
     }
	ucFwFlag = ucRevID;			//v1.06_1
	if(ucFwFlag == 0xA0)
		ucFwFlag = 0xB0;
	return iTE_TRUE;
}
//****************************************************************************
iTE_u1 IT66332_Init(void)
{
	iTE_Msg(("IT66332_Init S\n"));
	if(iTE_TRUE == IT66332_Detect()){
		hdmi_sw_init();
		if((ucFwFlag==0xB1) && (hdmi_sw_r(0x09)==0xFF)){
			ucFwFlag = 0xC0;								//V1.06_1
			hdmi_sw_set(0x36, 0x08, (WCLKRdyUnit<<3));	//V1.07_1
		}else{
			hdmi_sw_set(0x37, 0xF0, 0x10);				//V1.08_2
		}
		hdmi_rx_init();
		if(ucFwFlag == 0xB0){		//V1.06_3
			if(EnHDCPBypass==iTE_FALSE){
				hdmi_tx_set(0xe5, 0xb0, 0x10);// select HDMITXSRC="01" manually
			}else{
				hdmi_tx_set(0xe5, 0xb0, 0xa0);// select HDMITXSRC automatically
			}
		}
		cal_oclk();
#if USING_CEC
		cec_init();
#endif
		if( EnAutoEQ ) {
			hdmi_sw_set(0x70, 0x0E, 0x0E);
			hdmi_sw_set(0x71, 0x10, 0x10);
			hdmi_sw_set(0x77, 0xF0,(EnEQRtyTog<<7)+ (EnEQFailRty<<6)+0x30);	//V1.07_7
		}else {
			hdmi_sw_set(0x70, 0x0E, 0x00);
			hdmi_sw_set(0x71, 0x10, 0x00);
			hdmi_sw_set(0x77, 0xF0,(EnEQRtyTog<<7)+(EnEQFailRty<<6)+0x00);	//V1.07_7
			if(EnFixEQ){
				hdmi_sw_set(0x70, 0xC0, 0xC0);
				hdmi_sw_w(0x73, 0x00);
				hdmi_sw_w(0x74, 0x3F);
			}
		}
    		 // for 4Kx2K Termination
		hdmi_sw_w(0x67, 0x0F);
#if 0
		{	//V1.06_4
		hdmi_sw_set(0x09, 0xf0, 0x00);
		hdmi_sw_set(0x0f, 0xC0, 0x00);
		hdmi_sw_set(0x31, 0x08, 0x00);
		hdmi_sw_set(0x32, 0xc8, 0x00);
		hdmi_sw_set(0x33, 0x40, 0x00);
		hdmi_sw_set(0x3d, 0xc0, 0x00);
		hdmi_sw_set(0x77, 0x80, 0x00);
		}
#endif

#if USING_MHL
		mhl_rx_init();
#endif


		hdmi_sw_set(0x11, 0x0f, (DEFAULT_MHL << 2) + DEFAULT_HDMI);
//		hdmi_sw_set(0x12, 0x02, (DongleEDID<<1));
//start @20130618 added for clear int status after initial done
		hdmi_rx_w(0x13, 0xff);
		hdmi_rx_w(0x14, 0xff);
#if USING_MHL
		mhl_rx_w(0x05, 0xff);
		mhl_rx_w(0x06, 0xff);
		mhl_rx_w(0x07, 0xff);
#endif
//end @20130618
	     	if( Dongle==iTE_TRUE ) {
	         	hdmi_sw_w(0xFF, 0xC3);
	         	hdmi_sw_w(0xFF, 0xA5);
	         	hdmi_sw_set(0x10, 0x40, 0x40);
	         	hdmi_sw_w(0xFF, 0xFF);
#if USING_MHL
//	         	mhl_rx_set(0x82, 0x0F, 0x03);    // Dongle Mode	//@20130607, ming , move to initial table
#endif
			if( DongleEDID==EXT ) {
	//			mhlrxwr(0x3b, 0x31);       // M2HDDC 200KHz
	//			mhlrxwr(0x3b, 0x19);       // M2HDDC 400KHz
				hdmi_sw_set(0x44, 0x01, 0x01);   // disable shadow EDID
				hdmi_sw_set(0x11, 0x80, 0x80);   // FW Initial Done
			}else{
				hdmi_sw_set(0x17, 0x22, 0x22);   // Force TxPwr5V output
			}
		} else {
			hdmi_sw_set(0x17, 0x22, 0x22);   // Force TxPwr5V output
			hdmi_sw_EdidGet(iTE_FALSE);
			hdmi_sw_set(0x17, 0x22, 0x00);   // Disable TxPwr5V manual output
			hdmi_sw_set(0x11, 0x80, 0x80);   // FW Initial Done
		}
#if USING_MHL
		if(ucFwFlag != 0xC0){
			cbus_fwwa();
		}
#endif
	     	bCh0TermFlag = iTE_FALSE;
		bPwrDnFlag = iTE_FALSE;
     AutoEQFailCnt = 0;
		

		iTE_Msg(("IT66332_Init E\n"));
		return iTE_TRUE;
	}else{
		return iTE_FALSE;
	}
}
//****************************************************************************
void IT66332_Irq(void)
{
	iTE_u1 ucFlag = iTE_FALSE;			//@20130705, added for avoid interrupt can't clear if occure in MHL removed
	iTE_u8 IntState = hdmi_sw_r(0x2C);
	if(IntState){
		iTE_Msg(("IIT66332_Irq S [0x%x] \n", (int)IntState));

		//iTE_ExtIntEn(iTE_FALSE);
		if(bPwrDnFlag == iTE_TRUE){
			IT66332_sw_pwron();
		}

		if((IntState & 0x3E) && !(hdmi_sw_r(0x20)&0x04)){
			hdmi_sw_set(0x09, 0x02, 0x00);
			ucFlag = iTE_TRUE;
		}

		if(IntState & 0x01)
			hdmi_sw_irq();

	//	if( IntState&0x02 )
	//		hdmi_tx_irq();

		if( IntState&0x04 )
			hdmi_rx_irq();
#if USING_MHL
		if( IntState&0x08 )
			mhl_cbus_irq();

		if( IntState&0x10 )
			mhl_set_irq();
#endif
#if USING_CEC
		if( IntState&0x20 )
			cec_irq();
#endif
		if( IntState&0x40 )
			iTE_Msg(("Detect U3 Wakeup Interrupt ...\n"));

		if(ucFlag == iTE_TRUE){
			hdmi_sw_set(0x09, 0x02, 0x02);
		}

		iTE_Msg(("IIT66332_Irq E [0x%x] , HDCP=%d\n", (int)hdmi_sw_r(0x2c),(int)(hdmi_sw_r(0x2b)&0x04)>>2));
		if(bPwrDnFlag == iTE_FALSE){
			if((hdmi_sw_r(0x21) & 0x07) == 0){
				IT66332_sw_pwrdn();
	}
}
		iTE_ExtIntEn(1);
	}
}
//****************************************************************************
void IT66332_sw_pwron(void)
{
	hdmi_sw_set(0x0a, 0x80, 0x00);		//Enable SGRCLK
	iTE_SW_Msg(("Exit PwrDn Mode....\n"));
	bPwrDnFlag = iTE_FALSE;
}
//****************************************************************************
void IT66332_sw_pwrdn(void)
{
	hdmi_sw_set(0x0a, 0x80, 0x80);		//Gating SGRCLK
	iTE_SW_Msg(("Enter PwrDn Mode....\n"));
	bPwrDnFlag = iTE_TRUE;
}
//****************************************************************************
#if	(USING_6861 == 0)
void IT66332_SwitchHdmiMhl(ITE_HDMI ucHdmiIn, ITE_HDMI ucMhlIn)
{
	iTE_u8 ucRdData;
	//extern iTE_u8 PreSta;

	hdmi_sw_set(0x0a, 0x80, 0x00);
	if(ucHdmiIn > HDMI_C)
		ucHdmiIn = HDMI_REV;

#if USING_MHL
	if(ucMhlIn > HDMI_C)
		ucMhlIn = HDMI_REV;
#else
	ucMhlIn = DEFAULT_HDMI;
#endif

	ucRdData = (ucMhlIn << 2) | ucHdmiIn;
	if((hdmi_sw_r(0x11)&0xf) != ucRdData){
		if(bPwrDnFlag == iTE_TRUE) {		//V1.05
			IT66332_sw_pwron();
		}
		if(ucFwFlag != 0xC0){				//V1.07_5
			hdmi_sw_set(0x35, ((ucHdmiIn<<4) + ucHdmiIn), ucHdmiIn<<4);
		}
#if 1
		if((ucHDMI_Port == ucMHL_Port) && (ucFwFlag!=0xC0)){		//V1.07_3
			if(hdmi_sw_r(0x20)&0x04){
				if(EnPwrDnMHL == iTE_TRUE){
					mhl_EnterPwrDn();
				}
				hdmi_sw_set(0x61, 0x70, 0x30);
			}
		}
		if((ucHdmiIn == ucMhlIn) && (ucFwFlag != 0xC0)){			//V1.07_3
			if(hdmi_sw_r(0x20) & 0x04){
				if(EnPwrDnMHL == iTE_FALSE){
					mhl_EnterPwrDn();
//					iTE_Sleep_ms(HPD_RETRIGER_TIME);
				}
				mhl_ExitPwrDn();
				hdmi_sw_set(0x61, 0x70, 0x00);
			}else{
				hdmi_sw_set(0x61, 0x70, 0x30);
			}
		}

#else
		if(ucHDMI_Port == ucMHL_Port){
#if 1
			if(hdmi_sw_r(0x20) & 0x04){
				hdmi_sw_set(0x61, 0x70, 0x00);
				if( EnPwrDnMHL==iTE_TRUE){
	 				mhl_EnterPwrDn();
				}
			}else{
				hdmi_sw_set(0x61, 0x70, 0x03);
			}
#else
//start added by ming @20130607, sync from keming
			if( hdmi_sw_r(0x20) & 0x04)
				hdmi_sw_set(0x61, 0x70, 0x00);
			else
				hdmi_sw_set(0x61, 0x70, 0x03);
// end added
			if( EnPwrDnMHL==iTE_TRUE){
			if(hdmi_sw_r(0x20) & 0x04)
	 			mhl_EnterPwrDn();
		}
#endif
		}
		if(ucHdmiIn == ucMhlIn){
			if(hdmi_sw_r(0x20) & 0x04){
				if(EnPwrDnMHL==iTE_FALSE){
					mhl_EnterPwrDn();
					iTE_Sleep_ms(HPD_RETRIGER_TIME);
				}
				mhl_ExitPwrDn();
			}
		}
#endif
		if(CBusDrvHOpt && (ucHDMI_Port == ucMHL_Port)){		//V1.09_
			hdmi_sw_set(0x35, 0x88, 0x80);
			hdmi_sw_set(0x35, 0x88, 0x00);
		}
		hdmi_sw_set(0x11, 0x0f, ucRdData);
		iTE_Msg(("HPD retrigger\n"));
		if(ucHdmiIn != ucMhlIn)
			IT66332_SetHpd(ucHdmiIn, HPD_LOW);
		if(ucFwFlag != 0xC0){
			iTE_Sleep_ms(HPD_RETRIGER_TIME);
			hdmi_sw_set(0x35, 0x77, 0x00);
		}
		if(ucHdmiIn != ucMhlIn)
			IT66332_SetHpd(ucHdmiIn, HPD_AUTO);
		ucHDMI_Port = ucHdmiIn;
		ucMHL_Port = ucMhlIn;

	}
}
#endif
//****************************************************************************
void IT66332_GetHdmi(ITE_HDMI *ucpCurHdmi)
{
	   *ucpCurHdmi = ucHDMI_Port;
}
//****************************************************************************
void IT66332_GetMhl(ITE_MHL *ucpCurMhl)
{
	   *ucpCurMhl = ucMHL_Port;
}
//****************************************************************************
void IT66332_SetHpd(ITE_HDMI ucHdmiPort, ITE_HPD_CTL ucState)
{
	switch(ucState){
		case	HPD_AUTO:
		case	HPD_LOW:
		case	HPD_HIGH:
#if 1
			switch(ucHdmiPort){
				case	HDMI_A:
				case	HDMI_B:
				case	HDMI_C:
						hdmi_sw_set(0x13+ucHdmiPort, 0x88, ucState);
				default:
					break;
			}
#else
			if(ucHdmiPort < HDMI_REV){
					hdmi_sw_set(0x13+ucHdmiPort, 0x88, ucState);
			}
#endif
			break;
	}
}
//****************************************************************************
void IT66332_PowerDown(iTE_u1 bEnable)
{
	iTE_u8 ucTemp;

	ucTemp = hdmi_sw_r(0x0a) & 0x80;

	if(ucTemp)
		hdmi_sw_set(0x0a, 0x80, 0x00);

	hdmi_sw_w(0xff, 0xc3);
	hdmi_sw_w(0xff, 0xa5);

	if(bEnable){
		hdmi_sw_set(0x12, 0x01, 0x01);
		hdmi_sw_set(0x10, 0xa0, 0xa0);
	}else{
		hdmi_sw_set(0x12, 0x01, 0x00);
		hdmi_sw_set(0x10, 0xa0, 0x00);
	}

	hdmi_sw_w(0xff, 0xff);

	if(ucTemp)
		hdmi_sw_set(0x0a, 0x80, 0x80);
}
//****************************************************************************
//****************************************************************************
#if USING_MHL
void Msc_RcpKeyParse( iTE_u8 ucRcpKey );
void IT66332_MscRcpSend(iTE_u8 ucRcpKey)
{
	MSC_CMD MscTxCmd;
	iTE_Msg(("MSC RCP Send [0x%x] \n", (int)ucRcpKey));
	Msc_RcpKeyParse(ucRcpKey);
	MscTxCmd.ucOffset = MSC_MSG_RCP;
	MscTxCmd.ucValue = ucRcpKey;
	Msc_MscMsgFire(&MscTxCmd);
}
void IT66332_MscUcpSend(iTE_u8 ucUcpKey)
{
	MSC_CMD MscTxCmd;
	iTE_Msg(("MSC RCP Send [0x%x] = ' %c ' \n", (int)ucUcpKey));
	MscTxCmd.ucOffset = MSC_MSG_UCP;
	MscTxCmd.ucValue = ucUcpKey;
	Msc_MscMsgFire(&MscTxCmd);
}
#endif

iTE_u8 IT66332_GetReg(iTE_u8 ucI2cAddr, iTE_u8 ucReg)
{
	iTE_u8	ucRdData;
	iTE_i2c_read_byte(ucI2cAddr, ucReg,  1, &ucRdData, iTE_I2C_Dev);
	return ucRdData;
}
