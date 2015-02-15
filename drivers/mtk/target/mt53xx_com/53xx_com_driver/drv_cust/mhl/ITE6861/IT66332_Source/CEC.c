///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
//****************************************************************************
#define _CEC_LOOKUP_TAB_
//#include <string.h>
#include "../IT66332_Customer/IT66332_IO.h"
#include "../IT66332_Customer/IT66332.h"

#if USING_CEC
#include "CEC.h"
#include "CEC_Reg.h"
//#include "../INC/CEC_typedef.h"
//****************************************************************************

#define old_setting 0
#define Fix_ucRxPhyAdr 0

CEC_QBuf	CECRxQ,CECTxQ;
pCEC_QBuf	pCECRxQ,pCECTxQ;
iTE_u8 		ucInitatorAddr,ucTxPhyAdr[2];
#if Fix_ucRxPhyAdr
iTE_u8	ucRxPhyAdr[4][2];
#endif
ARC_StateType ARCState=sARCNone;
CECDevice CECList[15];
HEC_StateType	HECState=sHEC_None;
iTE_u8	ucUICmdCode;
iTE_u1 bSystemAudioMode=0;
CECAudioStatus uCECAudioStatus;
iTE_u8 ucMyselfAddr;
PollingMessage_StateType PMState=sPMnone;
eCEC_CMD OldTxCECcmd=eCECNone;
CEC_TX_StateType  CECTxState=sCECOff;
OneTouchPlay_StateType OTState=sOTNone;
SystemAudioControl_StateType SACState=sSACnone;
LogicAddressing_StateType LAState=sLANone;
//P_CEC_FRAME	pCECRxCmd,pCECTxCmd;
CEC_FRAME	CECRxCmd,CECTxCmd;
iTE_u8 u8_PowerStatus=0;	// 0: on , 1: standby, 2: in transition standby to on , 3: in transition on to standby
iTE_u1 gbCecTxDone = iTE_TRUE;
#define ARC_SUPPORT	(0)

//****************************************************************************
void	CEC_ClrInt(void)
{
#if old_setting
    	cec_w(REG08,  B_INT_CLR|B_DBGCEC_CLR|B_CEC_SMT); 	//Reset all interrupt
    	cec_w(REG08, 0x00|B_CEC_SMT);
#else
	cec_set(REG08, 0x48, 0x48);
	cec_set(REG08, 0x40, 0x40);
#endif

}
//****************************************************************************
void cec_init(void)
{
	iTE_u8 ucTemp = CEC_CAL_CNT;
	iTE_u32 ulClkCnt = 0;
	iTE_CEC_Msg((" cec_init_S \n"));
//	pCECRxQ = &CECRxQ;
//	pCECTxQ = &CECTxQ;
	CECRxQ.ucRptr = CECRxQ.ucWptr = 0;
	CECTxQ.ucRptr = CECTxQ.ucWptr = 0;

//	pCECRxCmd = &CECRxCmd;
//	pCECTxCmd = &CECTxCmd;
	ucMyselfAddr = IT66332_LogicAddr;
#if 0
	while(ucTemp){
		cec_w(REG09, B_EN100ms_CNT);
		iTE_BusyWait_ms(100);
		cec_w(REG09, 0x00);
		ucTemp--;
		ulClkCnt += cec_r(REG_MSCOUNT_L);
		ulClkCnt += (ULONG)cec_r(REG_MSCOUNT_M) << 8;
		ulClkCnt += (ULONG)cec_r(REG_MSCOUNT_H) << 16;
		iTE_CEC_Msg(("ulCLKCnt=0x%x, 0x%x, 0x%x\n", (int)cec_r(REG_MSCOUNT_H), (int)cec_r(REG_MSCOUNT_M), (int)cec_r(REG_MSCOUNT_L)));
	};


	ulClkCnt /= (1000*CEC_CAL_CNT);
	iTE_CEC_Msg(("ulCLKCnt=0x%x\n", (int)(ulClkCnt&0xff)));
#else
	ulClkCnt = fRefClk/160;
	iTE_CEC_Msg(("ulCLKCnt=0x%x\n", (int)(ulClkCnt&0xff)));
#endif

	if(CEC_RX_SELF_DIS == 1)
		cec_w(REG09,  0x40 | B_RXSELF_SEL);
	else
		cec_w(REG09,  0x40);
#if old_setting
	cec_w(REG_DATA_MIN, 0x00); 		//REG_Data_Min[7:0]
#else
	cec_w(REG_DATA_MIN, 0x14); 		//REG_Data_Min[7:0]
#endif
    	cec_w(REG_TIMER_UNIT, ulClkCnt); 	//REG_Timer_unit[7:0]
    	cec_w(REG_CEC_TARGET_ADDR, ucMyselfAddr); 	//REG_Logical_addr[3:0]

#if old_setting
    	if( CEC_RST==1 )
        	cec_w(REG08,B_CEC_RST|B_CEC_SMT); 	//REG_CEC_Rst


	ucTemp=cec_r(REG09);

    	if( CEC_NACK_EN==1 )
        	cec_w(REG09, ucTemp|B_NACK_EN); 	//REG_NAck_En

	cec_w(REG_INT_STATUS, 0x3f);
	CEC_ClrInt();

	cec_set(REG08, 0x01, 0x01);
#else
	if(CEC_RST==1)
		cec_set(0x08, 0x04, 0x04);
	else
		cec_set(0x08, 0x04, 0x00);

//	if(EnNACK == 1)
//		cec_set(0x09, 0x02, 0x02);
//	else
		cec_set(0x09, 0x02, 0x00);

//	CEC_ClrInt();
		cec_w(REG_INT_STATUS, 0x3f);
#endif
		cec_w(0x06, 3);
		cec_set(0x08, 0x01, 0x01);
	iTE_CEC_Msg((" cec_init_E \n"));
}
//****************************************************************************
void cec_TransmitterFail()
{
	//if cec tx fail, resend cec command

	iTE_u8 ucTemp,ucSize;

	cec_br(REG_TX_HEADER, 18, (iTE_ps8)&CECTxCmd);
	ucTemp = cec_r(REG_CEC_TARGET_ADDR);
	ucSize = cec_r(REG_CEC_OUT_NUM);

	ucTemp =  cec_r(REG_SYS_STATUS);
	if(( ucTemp & (B_READY_FIRE | B_BUS_STATUS)) == (B_READY_FIRE | B_BUS_STATUS)){
		cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT, B_CEC_SMT);         	//FireCmd
		cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT,B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
	}
}
void	cec_ReceiverFail()
{
	//if cec rx fail

	//ToDo
}
void cec_irq(void)
{
	iTE_u8	CEC_int;

	CEC_int = cec_r(REG_INT_STATUS);
	cec_w(REG_INT_STATUS, CEC_int);
	iTE_CEC_Msg(("********************************************cec_irq 0x%x \n", (int)CEC_int));

	if(CEC_int & 0x3f){
		if(CEC_int & B_TX_INT){		iTE_CEC_Msg(("CEC Tx INT \n"));	}
		if(CEC_int & B_TXFAIL_INT){
			iTE_CEC_Msg(("CEC Tx OpCode = 0x%x Fail \n", (int)cec_r(REG_TX_OPCODE)));
			gbCecTxDone = iTE_FALSE;
			cec_TransmitterFail();
		}
		if(CEC_int & B_TXDONE_INT){
			iTE_CEC_Msg(("CEC Tx OpCode = 0x%x Done \n", (int)cec_r(REG_TX_OPCODE)));
			gbCecTxDone = iTE_TRUE;
			cec_Transmitter();
		}


		if(CEC_int & B_RX_INT){		iTE_CEC_Msg(("CEC Rx INT \n"));	}
		if(CEC_int & B_RXDONE_INT){
				iTE_CEC_Msg(("CEC Rx Done \n"));
				CEC_RxPush();
#if 0
	iTE_u8	temp[20]={0x01,0x00, 0xff, 0x04,0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
	cec_bw(0x10, 20, temp);
	if(cecfirerdy()){
		cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT, B_CEC_SMT);         	//FireCmd
		cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT,B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
	}
#endif
		}

		if(CEC_int & B_RXFAIL_INT){
			iTE_CEC_Msg(("CEC Rx Fail \n"));
			cec_ReceiverFail();
		}
		//CEC_ClrInt();
	}
	cec_handler();
}
//****************************************************************************
void cec_handler(void)
{
	cec_decoder();
//	ARCHandler();				//for TV device
//	OneTouchPlayHandler();		//for Audio and player device
//	LogicalAddressingHandler();	//for TV device
//	PollingMessage();			//for player device
//	SystemInfomationFeature();	//for system information Feature
//	SystemAudioControl();		//for Audio control system CTS 11.2.15
	cec_Transmitter();
}
//****************************************************************************
void cec_TxCmdSet(iTE_u8 ucFollower, eCEC_CMD TxCmd, iTE_u8 ucSize)
{
	CECTxCmd.ucHeader = (ucMyselfAddr<<4)+ucFollower;
	CECTxCmd.ucOpcode = TxCmd;
	CECTxCmd.ucSize = ucSize;
	CEC_TxPush(&CECTxCmd);
	CECTxState=sCECcmdGet;
}
//****************************************************************************
void cec_CDCFeature()
{
	switch(CECRxCmd.ucOpcode){
		case	eFeatureAbort:
			iTE_CEC_Msg(("Rx !!! Feature Abort !!!\n"));
			return;
		case	eAbort:
			iTE_CEC_Msg(("Rx !!! Abort !!!\n"));
			CECTxCmd.ucOperand1 = eAbort;
			CECTxCmd.ucOperand2 = 4;
			cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
			break;
	}
}
//****************************************************************************
void cec_OTPFeature()
{
	switch(CECRxCmd.ucOpcode){
		case	eImageViewOn:
			iTE_CEC_Msg(("Rx !!! Image View On !!!\n"));
			break;
		case	eTextViewOn:
			iTE_CEC_Msg(("Rx !!! Text View On !!!\n"));
			break;
		case	eActiveSource:
			iTE_CEC_Msg(("Rx !!! Active Source !!!\n"));
			break;
	}
	OneTouchPlayHandler();
}
//****************************************************************************
void cec_RoutingCtlFeature()
{
	iTE_u8 ucTemp;

	switch(CECRxCmd.ucOpcode){
		case	eRoutingChange:		//<DB><0x80><Original Adr><New Adr>  , size=6		// send by switch device to manually change port
									//reply new address <routine information> to indicate its current active route.
				iTE_CEC_Msg(("Rx !!! Routing Change !!!\n"));
#if Fix_ucRxPhyAdr
				if((ucTxPhyAdr[0]==CECRxCmd.ucOperand3) && (ucTxPhyAdr[1]==CECRxCmd.ucOperand4)){
					CECTxCmd.ucOperand1 = ucRxPhyAdr[ucRxCurPort][0];
					CECTxCmd.ucOperand2 = ucRxPhyAdr[ucRxCurPort][1];
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eRoutingInformation, 4);
				}
#endif
			break;
		case	eRoutingInformation:	//<DB> <0x81> <Physical Address> ,example:[0x5F 0x81 AB CD]  AVR get active route below the switch
				iTE_CEC_Msg(("Rx !!! Routing Information !!!\n"));
#if Fix_ucRxPhyAdr
				if((ucTxPhyAdr[0]==CECRxCmd.ucOperand1) && (ucTxPhyAdr[1]==CECRxCmd.ucOperand2)){
					CECTxCmd.ucOperand1 = ucRxPhyAdr[ucRxCurPort][0];
					CECTxCmd.ucOperand2 = ucRxPhyAdr[ucRxCurPort][1];
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eRoutingInformation, 4);
				}else{
					iTE_CEC_Msg(("Invalid -->eRoutingInformation \n"));
				}
#endif
			break;
		case	eInactiveSource:		//<MS> <0x9D> <Physical Address>		example:<0x40> <0x9D> <AB> <CD>
									// It has no video to be presented to the user, or is goning into standby state.
				iTE_CEC_Msg(("Rx !!!Inactive Source !!!\n"));
				if(ucMyselfAddr == DEVICE_ID_TV){
					if(CECList[ucInitatorAddr].Active == iTE_TRUE){
						CECList[ucInitatorAddr].Active = iTE_FALSE;
						iTE_CEC_Msg(("CECList[%bX].Active -> iTE_FALSE\n",ucInitatorAddr));

						for(ucTemp=1; ucTemp<15; ucTemp++){
							if(CECList[ucTemp].Active == iTE_TRUE){
								CECTxCmd.ucOperand1 = CECList[ucTemp].PhyicalAddr1;
								CECTxCmd.ucOperand2 = CECList[ucTemp].PhyicalAddr2;
								cec_TxCmdSet(ucTemp, eSetStreamPath, 4);
							}
						}
					}
				}
			break;
		case	eRequestActiveSource:	// <DB> <0x85> , need to reply <active source> by CEC active device
				iTE_CEC_Msg(("Rx !!!Request Active Source !!!\n"));
				CECTxCmd.ucOperand1 = ucTxPhyAdr[0];
				CECTxCmd.ucOperand2 = ucTxPhyAdr[1];
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eActiveSource, 4);
			break;
		case	eSetStreamPath:		// <DB> <0x86> <Physical Address> , example:[0x0F 0x86 AB CD] TV request a streaming path from the specified Physical Address
				iTE_CEC_Msg(("Rx !!!Set Stream Path !!!\n"));
				if(ucMyselfAddr==DEVICE_ID_AUDIO){
					CECChangePort();
				}else if((ucMyselfAddr == DEVICE_ID_PLAYBACK1) || (ucMyselfAddr == DEVICE_ID_PLAYBACK2)  || (ucMyselfAddr == DEVICE_ID_PLAYBACK3) ){
					Switch_OneTouchPlayState(sImageViewOn);
				}
			break;
	}
}
//****************************************************************************
void cec_StandbyFeature()
{
	switch(CECRxCmd.ucOpcode){
		case	eStandBy:		// <MS> <0X36> or <DB> <0x36> ,
				iTE_CEC_Msg(("Rx !!!Standby !!!\n"));
			break;
	}
}
//****************************************************************************
void cec_SysInfoFeature()
{
	switch(CECRxCmd.ucOpcode){
		case	eCECVersioin:				//<MS> <0x9E> <CEC version>
				iTE_CEC_Msg(("Rx !!!CEC Version =%bX!!!\n",CECRxCmd.ucOperand1));
			break;
		case	eGetCECVersion :   		//<MS> <0x9F> , example: [0x05 0x9F] TV request AVR to reply CEC version
				iTE_CEC_Msg(("Rx !!!Report CEC Version !!!\n"));
				CECTxCmd.ucOperand1 = 0x04;
				cec_TxCmdSet(ucInitatorAddr, eCECVersioin, 3);
			break;
		case	eGivePhysicalAddress:		//<MS> <0x83> , example:[0x05 0x83] TV request AVR to return Physical address
				iTE_CEC_Msg(("Receive !!!Give Physical Address !!!\n"));
				if(ucMyselfAddr==DEVICE_ID_TV){
					CECTxCmd.ucOperand1 = 0;
					CECTxCmd.ucOperand2 = 0;
					CECTxCmd.ucOperand3 = 0;
				}else{
					CECTxCmd.ucOperand1 = ucTxPhyAdr[0];
					CECTxCmd.ucOperand2 = ucTxPhyAdr[1];
					CECTxCmd.ucOperand3 = cec_GetDeviceType(ucMyselfAddr);
				}
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eReportPhysicalAddress, 5);
			break;
		case	eReportPhysicalAddress:	// <DB> <0x84> <Physicall Adress> <Device type>, example:[0x5F 0x84 AB CD 0]
				iTE_CEC_Msg(("Receive !!!Report Physical Address !!!\n"));
				if((CECRxCmd.ucHeader & 0x0f)==DEVICE_ID_BROADCAST){
					CECList[ucInitatorAddr].PhyicalAddr1=CECRxCmd.ucOperand1;
					CECList[ucInitatorAddr].PhyicalAddr2=CECRxCmd.ucOperand2;
					CECList[ucInitatorAddr].Active=iTE_TRUE;
#if 0
					if(ucMyselfAddr == DEVICE_ID_TV){
						CECTxCmd.ucOperand1 = CECList[ucInitatorAddr].PhyicalAddr1;
						CECTxCmd.ucOperand2 = CECList[ucInitatorAddr].PhyicalAddr2;
						cec_TxCmdSet(ucInitatorAddr, eSetStreamPath, 4);
					}
#endif
				}
			break;
		case eGetMenuLanguage :	//Get Menu Language
				iTE_CEC_Msg(("Receive !!!Get Menu Language !!!\n"));
				CECTxCmd.ucOperand1 = eGetMenuLanguage;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
			break;
		case eSetMenuLanguage :	//Set Menu Language
				iTE_CEC_Msg(("Receive !!!Set Menu Language !!!\n"));
				CECTxCmd.ucOperand1 = eSetMenuLanguage;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
			break;
	}
}
//****************************************************************************
void cec_DeckCtlFeature()
{
	switch(CECRxCmd.ucOpcode){
		case ePlay :	//Play							 //<MS> <featuer about> <op code> <abort reason>
				CECTxCmd.ucOperand1 = ePlay;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
				iTE_CEC_Msg(("Receive !!!Play !!!\n"));
			break;
		case eDeckControl :	//Deck control				//<MS> <featuer about> <op code> <abort reason>
				CECTxCmd.ucOperand1 = eDeckControl;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
				iTE_CEC_Msg(("Receive !!!Deck control !!!\n"));
			break;
		case eGiveDeckStatus :	//Give Deck Status		 //<MS> <featuer about> <op code> <abort reason>
				CECTxCmd.ucOperand1 = eGiveDeckStatus;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
				iTE_CEC_Msg(("Receive !!!Give Deck Status !!!\n"));
			break;
		case eDeckStatus :	//Deck Status				//<MS> <featuer about> <op code> <abort reason>
				CECTxCmd.ucOperand1 = eDeckStatus;
				CECTxCmd.ucOperand2 = 4;
				cec_TxCmdSet(ucInitatorAddr, eFeatureAbort, 4);
				iTE_CEC_Msg(("Receive !!!Deck Status !!!\n"));
			break;
	}
}
//****************************************************************************
void cec_VenSpecCmdFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eVendorCommandWithID :	//Vendor Command With ID			// Vendor Specific
				iTE_CEC_Msg(("Receive !!!Vendor Command With ID !!!\n"));
			break;
		case eDeviceVendorID :   	//<DB> <0x87> <Vendor ID>
				iTE_CEC_Msg(("Receive !!!Device Vendor ID !!!\n"));
			break;
		case eGiveDeviceVendorID :	//<MS> <0x8C>	//need to return <Device Vendor ID>
				iTE_CEC_Msg(("Receive !!!Give Device Vendor ID !!!\n"));
				CECTxCmd.ucOperand1 = 0;
				CECTxCmd.ucOperand2 = 0;
				CECTxCmd.ucOperand3 = 0;
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eDeviceVendorID, 5);
			break;
		case eVendorCommand :    	//Vendor Command				// Vendor Specific
				iTE_CEC_Msg(("Receive !!!Vendor Command !!!\n"));
			break;
		case eRemoteButtonDown :    	//Vendor Remote Button Down		// Vendor Specific
				iTE_CEC_Msg(("Receive !!!Vendor Remote Button Down !!!\n"));
			break;
		case eRemoteButtonUp :    	//Vendor Remote Button Up		// Vendor Specific
				iTE_CEC_Msg(("Receive !!!Vendor Remote Button Up !!!\n"));
			break;
	}
}
//****************************************************************************
void cec_RemoteCtlPassthroughFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eUserPressed :	//<MS> <0x44> <UI Command>
				iTE_CEC_Msg(("Receive !!!User Control Pressed !!!\n"));
				ucUICmdCode=CECRxCmd.ucOperand1;		// SAVE UI command code
			break;
		case eUserReleased :	//<MS> <0X45>
				iTE_CEC_Msg(("Receive !!!User Control Released !!!\n"));
				CECUICommandCode(ucUICmdCode);		// executioin UI command code
			break;
	}
}
//****************************************************************************
void cec_PwrStatusFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eGiveDevicePowerStatus:	// <MS> <0x8F Give Device Power Status> --> <MS> <0x90 Report Power Status> < 0~3 >
				iTE_CEC_Msg(("Receive !!!Give Device Power Status !!!\n"));
				CECTxCmd.ucOperand1 = u8_PowerStatus;			// 0: on , 1: standby, 2: in transition standby to on , 3: in transition on to standby
				cec_TxCmdSet(ucInitatorAddr, eReportPowerStatus, 3);
			break;
		case eReportPowerStatus:
				iTE_CEC_Msg(("Receive !!!Report Power Status !!!\n"));
			break;
	}
}
//****************************************************************************
void cec_SysAudioCtlFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eAudioModeRequest :		//<MS> <0x70 Audio Mode Request > <Phy Adr> ,example:[0x05 0x70 00 00] TV send Audio mode request ON to AVR
									//<MS> <0x70 Audio Mode Request >         ,example:[0x05 0x70] TV send Audio mode request OFF to AVR
				iTE_CEC_Msg(("Receive !!!System Audio Mode Request !!!\n"));
				if(CECRxCmd.ucSize==4){
					bSystemAudioMode=1;	// Initating the system audio control

					if(ucInitatorAddr==DEVICE_ID_TV){
						CECTxCmd.ucOperand1 = bSystemAudioMode;
						cec_TxCmdSet(DEVICE_ID_BROADCAST, eSetSystemAudioMode, 3);		// AVR reply  [0x05 0x72  and Audio On / Off]
					}else{
						Switch_SystemAudioControl(sSetSystemAudioModeToTV);
					}
					//need to un-mute Amplifer speaker
				}else{
					bSystemAudioMode=0;	//Terminating the system audio control
					CECTxCmd.ucOperand1 = bSystemAudioMode;
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eSetSystemAudioMode, 3);		// AVR reply  [0x05 0x72  and Audio On / Off]
					//need to mute Amplifer speaker
				}
			break;
		case eGiveAudioStatus:			//<MS> <0x71>
				iTE_CEC_Msg(("Receive !!!Give Audio Status !!!\n"));
				CECTxCmd.ucOperand1 = uCECAudioStatus.AStatus;
				cec_TxCmdSet(ucInitatorAddr, eReportAudioStatus, 3);		//AVR reply  Report Audio Status [0x05 0x7A and Volume and Mute status]
			break;
		case eSetSystemAudioMode :		//<MS> <0X72> <Audio on/off>
				iTE_CEC_Msg(("Receive Set System Audio Mode !!!\n"));
				if(CECRxCmd.ucOperand1==0)
					bSystemAudioMode=1;	// AVR audio mode ON
				else
					bSystemAudioMode=0;	// AVR audio mode OFF
			break;
		case eReportAudioStatus:			//<MS> <0x7A> <Volume and Mute Status>
				iTE_CEC_Msg(("Receive !!!Report Audio Status !!!\n"));
				uCECAudioStatus.AStatus=CECRxCmd.ucOperand1;
			break;
		case eGiveSystemAudIoModeStatus :	//<MS> <0x7D >  --> <MS> <0x7E > <Audio On / Off>
				iTE_CEC_Msg(("Receive !!!Give System Audio Mode Status !!!\n"));
				CECTxCmd.ucOperand1 = bSystemAudioMode;
				cec_TxCmdSet(ucInitatorAddr, eSystemAudIoModeStatus, 3);		//<Audio On / Off>
			break;
		case eSystemAudIoModeStatus :		//<MS> <0x7E> <System Audio Status>
				iTE_CEC_Msg(("Receive !!!System Audio Mode Status !!!\n"));
				if(CECRxCmd.ucOperand1==0)
					bSystemAudioMode=1;	// AVR audio mode ON
				else
					bSystemAudioMode=0;	// AVR audio mode OFF
			break;
		case eRequestAudioDescriptor :		//<MS> <0xA4> <Audio Format ID and Code>
				iTE_CEC_Msg(("Receive !!!Request Short Audio Descriptor !!!\n"));
				//0x23,0x0F, 0x7F, 0x07,		//PCM 8CH, 32~192KHz, 16/20/24Bit
				//0x23,0x15, 0x1F, 0x38,		//AC3 6CH, 32~96KHz, 16/20/24Bit
				//0x23,0x3F, 0x07, 0xC0,		//DTS 8CH, 32~ 48K, 1536Kbps
				CECTxCmd.ucOperand1 = 0x0F;
				CECTxCmd.ucOperand2 = 0x7F;
				CECTxCmd.ucOperand3 = 0x07;
				CECTxCmd.ucOperand4 = 0x15;
				CECTxCmd.ucOperand5 = 0x1F;
				CECTxCmd.ucOperand6 = 0x38;
				CECTxCmd.ucOperand7 = 0x3F;
				CECTxCmd.ucOperand8 = 0x07;
				CECTxCmd.ucOperand9 = 0xC0;
				cec_TxCmdSet(ucInitatorAddr, eReportAudioDescriptor, 11);		// Report EDID Audio Block !!!
			break;
	}
}
//****************************************************************************
#if ARC_SUPPORT	//no support
void cec_ARCFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eInitiateARC :
				iTE_CEC_Msg(("Receive !!!Initiate ARC !!!\n"));
				switch_ARCState(sReportARCInitiated);
			break;
		case eReportARCInitiated :
				iTE_CEC_Msg(("Receive !!!Report ARC Initiated !!!\n"));
			break;
		case eReportARCTerminated :
				iTE_CEC_Msg(("Receive !!!Report ARC Terminated !!!\n"));
			break;
		case eRequestARCInitiation :
				iTE_CEC_Msg(("Receive !!!Request ARC Initiation !!!\n"));
				switch_ARCState(sInitiateARC);
			break;
		case eRequestARCTermination :    //Request ARC Termination
				iTE_CEC_Msg(("Receive !!!Request ARC Termination !!!\n"));
				if(CheckUpDownPhyAdr()==iTE_TRUE){
					switch_ARCState(sTerminateARC);
				}
			break;
		case eTerminateARC :    //Termiate ARC
				iTE_CEC_Msg(("Receive !!!Termiate ARC !!!\n"));
				switch_ARCState(sReportARCTermination);
			break;
	}
	ARCHandler();
}
#endif
//****************************************************************************
void cec_HECFeature()
{
	switch(CECRxCmd.ucOpcode){
		case eCDC:
			switch( CECRxCmd.ucOperand3){
				case eHEC_InquirState:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_InquireState !!!\n"));
//						SwitchHECState(sHEC_InquirState);
					break;
				case eHEC_ReportState:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_Report_State !!!\n"));
						SwitchHECState(sHEC_ReportState);
					break;
				case eHEC_SetStateAdjacent:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_SetStateAdjacent !!!\n"));
						SwitchHECState(sHEC_SetStateAdjacent);
					break;
				case eHEC_SetState:
						iTE_CEC_Msg(("\nReceive !!!CDC_HEC_SetState !!!\n"));
						SwitchHECState(sHEC_SetState);
					break;
				case eHEC_RequestDeactivation:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_RequestDeactivation !!!\n"));
						SwitchHECState(sHEC_RequestDeactivation);
					break;
				case eHEC_NotifyAlive:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_NotifyAlive !!!\n"));
						SwitchHECState(sHEC_NotifyAlive);
					break;
				case eHEC_Discover:
						iTE_CEC_Msg(("\nReceive !!!CEC_HEC_Discover !!!\n"));
						SwitchHECState(sHEC_Discover);
					break;
				case eHEC_HPDSetState:
						iTE_CEC_Msg(("\nReceive !!!CDC_HPD_SetState !!!\n"));
						SwitchHECState(sHEC_HPDSetState);
					break;
				case eHEC_HPDReportState:
						iTE_CEC_Msg(("\nReceive CEC_HDP_ReportState !!!\n"));
						SwitchHECState(sHEC_HPDReportState);
					break;
				default :
						iTE_CEC_Msg(("\nReceive CDC UnRecongnized OpCode !!!\n"));
					break;
			}
			break;
	}
}
//****************************************************************************
void cec_decoder(void)
{
	iTE_u8 ucTemp;

	iTE_u8 ucFollowerAddr;
	iTE_u1 bValidCmd = iTE_TRUE;

	if(SYS_SUCCESS == CEC_RxPull(&CECRxCmd)){
		//Cmd print
		iTE_CEC_Msg(("RxCmd= "));
		for(ucTemp=0; ucTemp< CECRxCmd.ucSize; ucTemp++){
			iTE_CEC_Msg((" data[%d]=0x%x, ",(int)ucTemp, (int)*((iTE_u8*)&CECRxCmd+ucTemp)));
		}
		iTE_CEC_Msg(("\n"));

		ucFollowerAddr = (CECRxCmd.ucHeader & 0x0f);
		iTE_CEC_Msg(("cec Follower addr = 0x%x\n", (int)ucFollowerAddr));

		if(CECRxCmd.ucSize == 1){
			if(ucMyselfAddr == ucFollowerAddr)
				CEC_SwitchPollingMsg(sReportPhyAdr);
			return;
		}

		//Invalid command check
		for(ucTemp=0; ucTemp<SizeOfLookUpTable_CEC; ucTemp++){
			if(CECRxCmd.ucOpcode == LookUpTable_CEC[ucTemp].cmd)
				break;
		}

		if(ucTemp != (SizeOfLookUpTable_CEC - 1)){
			switch(LookUpTable_CEC[ucTemp].header){
				case	eDirectly:
					if(ucFollowerAddr == DEVICE_ID_BROADCAST){
						bValidCmd = iTE_FALSE;
						iTE_CEC_Msg(("!!! Invalid Direct Cmd !!! \n"));
					}
					break;
				case	eBroadcast:
					if(ucFollowerAddr != DEVICE_ID_BROADCAST){
						bValidCmd = iTE_FALSE;
						iTE_CEC_Msg(("!!! Invalid Direct Cmd !!! \n"));
					}
					break;
				case	eBoth:
					break;
			}
#if 0
			if(LookUpTable_CEC[ucTemp].size != CECRxCmd.ucSize){
				bValidCmd = iTE_FALSE;
				iTE_CEC_Msg(("!!! Invalid Size !!! \n"));
			}
#else
			if(LookUpTable_CEC[ucTemp].size >  CECRxCmd.ucSize){
				bValidCmd = iTE_FALSE;
				iTE_CEC_Msg(("!!! Invalid Size !!! \n"));
			}
#endif
		}

		if(bValidCmd==iTE_TRUE){
			ucInitatorAddr = (CECRxCmd.ucHeader >> 4);
			iTE_CEC_Msg((" Initator Addr = 0x%x\n", (int)ucInitatorAddr));

			switch(LookUpTable_CEC[ucTemp].eType){
				case	eCDCFeature:
						cec_CDCFeature();
					break;
				case	eOTPFeature:
						cec_OTPFeature();
					break;
				case	eRoutingCtlFeature:
						cec_RoutingCtlFeature();
					break;
				case	eStandByFeature:
						cec_StandbyFeature();
					break;
				case	eSysInfoFeature:
						cec_SysInfoFeature();
					break;
				case	eVenSpecCmdFeature:
						cec_VenSpecCmdFeature();
					break;
				case	eDeckCtlFeature:
						cec_DeckCtlFeature();
					break;
				case	eRemoteCtlPassthroughFeature:
						cec_RemoteCtlPassthroughFeature();
					break;
				case	ePowerStatusFeature:
						cec_PwrStatusFeature();
					break;
				case	eSysAudioCtlFeature:
						cec_SysAudioCtlFeature();
					break;
#if ARC_SUPPORT	//no support
				case	eACRFeature:
						cec_ARCFeature();
					break;
#endif
				case	eHECFeature:
						cec_HECFeature();
					break;
				case	eNoneFeature:
					break;
			}
		}
	}
}
//****************************************************************************
void CECChangePort(void){}
void SystemInfomationFeature(void){}
//****************************************************************************
iTE_u8 cecfirerdy( void )
{
	iTE_s8	*OutStatus[]={
							"Received Ack",
							"Received Nack",
							"Retry",
							"Fail",
						};

	iTE_u8 ucTemp;
		iTE_CEC_Msg(("cecfirerdy \n"));
	ucTemp = (B_READY_FIRE | B_BUS_STATUS);
 	if(( cec_r(REG_SYS_STATUS) & ucTemp) == ucTemp){
		iTE_CEC_Msg(("previous cec %s \n",OutStatus[(cec_r(REG_SYS_STATUS) & B_OUT_STATUS)>>2]));
#if 0
		switch((cec_r(REG_SYS_STATUS) & B_OUT_STATUS)>>2){
			case	B_ReceiverACK:
					iTE_CEC_Msg(("Received Ack\n"));
				break;
			case	B_ReceiverNACK:
					iTE_CEC_Msg(("Received Nack\n"));
				break;
			case	iTE
		#define	B_ReceiverRetry			0x08
		#define	B_ReceiverFail			0x0C
		}
#endif
		return iTE_TRUE;
	}else{
		iTE_CEC_Msg(("cec busy\n"));
		return iTE_FALSE;
	}

}
void cec_test()
{
	if(cecfirerdy()){
		iTE_CEC_Msg(("cec fire \n"));
		cec_w(0x10, 0x0f);
		cec_w(0x11, eReportPhysicalAddress);
		cec_w(0x23, 0x02);
		cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT,B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
	}
}
//****************************************************************************
iTE_u1 cec_Transmitter(void)
{
	iTE_u8	ucTemp;
	iTE_CEC_Msg(("cec_Transmitter = %d \n", (int)CECTxState));

	ucTemp =  cec_r(REG_SYS_STATUS);
	if(( ucTemp & (B_READY_FIRE | B_BUS_STATUS)) == (B_READY_FIRE | B_BUS_STATUS)){
#if 1
		if(CEC_TxPull()==SYS_SUCCESS){
			cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT, B_CEC_SMT);         	//FireCmd
			cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT,B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
			OldTxCECcmd=(eCEC_CMD) cec_r(REG_TX_OPCODE);
//			CECTxState=sCECCheckResult;
		}
		return iTE_TRUE;
#else
		switch(CECTxState){
			case	sCECOk:
			case	sCECcmdGet:
			case	sCECTransfer:
					if(CEC_TxPull()==SYS_SUCCESS){
//						cec_w(REG08, B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
						cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT, B_CEC_SMT);         	//FireCmd
						cec_set(REG08, B_FIRE_FRAME|B_CEC_SMT,B_FIRE_FRAME|B_CEC_SMT);         	//FireCmd
						OldTxCECcmd=(eCEC_CMD) cec_r(REG_TX_OPCODE);
						CECTxState=sCECCheckResult;
					}
				break;
			case	sCECCheckResult:
				{
					iTE_u8	ucTemp;

					ucTemp=(cec_r(REG_SYS_STATUS)&B_OUT_STATUS);

					if(ucTemp==B_ReceiverFail)	{
						CECTxState=sCECReceiverFail;
					}else if(ucTemp==B_ReceiverNACK){
						CECTxState=sCECReceiverNack;
					}else if(ucTemp==B_ReceiverRetry){
						//IT6601_PRINT(("Receiver Retry\n");
					}else{
						//IT6601_PRINT(("Receiver ACK\n");
						if( cec_r(REG_INITIATOR_TX_NUM)==cec_r(REG_CEC_OUT_NUM)){
							//IT6601_PRINT((" Result => TX OK\n");
							CECTxState=sCECOk;
						}
					}
				}
				break;
			case sCECReceiverNack:
			case sCECNone:
			case sCECReceiverFail:
			case sCECOff:
				break;
		}
#endif
	}	else{
		return iTE_FALSE;
	}
}
//****************************************************************************
void LogicalAddressingHandler(void)
{
	iTE_u8		ucTemp;
	static iTE_u8	cPollingCount;

	if(ucMyselfAddr!=DEVICE_ID_TV)
		return;


	switch(LAState){
		case sReportPhysicalAddressTransfer:		// only for TV to create CEC device list
				CECTxCmd.ucOperand1 = ucTxPhyAdr[0];
				CECTxCmd.ucOperand2 = ucTxPhyAdr[1];
				CECTxCmd.ucOperand3 = cec_GetDeviceType(ucMyselfAddr);
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eReportPhysicalAddress, 5);
				cPollingCount=0;
				LAState=sPollingMessage;
			break;
		case sPollingMessage:
				cPollingCount++;
				if(cPollingCount<15){
					ucTemp=(cPollingCount |0xF0);
					cec_TxCmdSet(ucTemp, ePollingMessage, 1);
					LAState=sPollingResult;
				}else{
					LAState=sLANone;
					for(ucTemp=1;ucTemp<15;ucTemp++){
						iTE_CEC_Msg(("CECList[%bX]= %bX , PhysicalAddr=[%bX][%bX] \n",	(int)ucTemp, CECList[ucTemp].Active, CECList[ucTemp].PhyicalAddr1, CECList[ucTemp].PhyicalAddr2));
					}
				}
			break;
		case sPollingResult:
				if(CECTxState==sCECReceiverFail){
					if(OldTxCECcmd==ePollingMessage){
						//clear value for CEC list
						CECList[cPollingCount].Active=iTE_FALSE;
						CECList[cPollingCount].PhyicalAddr1=0x00;
						CECList[cPollingCount].PhyicalAddr2=0x00;
						LAState=sPollingMessage;
					}
				}
			break;
		case sReportPhysicalAddressReceived:
				// stat to polling message and check ACK for allocate Logical Address to CEC device
				if(CECRxCmd.ucOpcode==eReportPhysicalAddress){
					if((CECRxCmd.ucHeader & 0x0f)==DEVICE_ID_BROADCAST){
						ucTemp=((CECRxCmd.ucHeader & 0xf0)>>4);
						cPollingCount=ucTemp;
						CECList[cPollingCount].PhyicalAddr1=CECRxCmd.ucOperand1;
						CECList[cPollingCount].PhyicalAddr2=CECRxCmd.ucOperand2;
						CECList[cPollingCount].Active=iTE_TRUE;
						LAState=sPollingMessage;
					}
					iTE_CEC_Msg(("-----Rx Header=%bX, opcode=%bX , operand1=%bX , operand2=%bX ,cPollingCount=%bX-----\n ",
						CECRxCmd.ucHeader, CECRxCmd.ucOpcode, CECRxCmd.ucOperand1, CECRxCmd.ucOperand2, cPollingCount));
				}
			break;
		case sLANone:
			break;

	}
}
//****************************************************************************
void CEC_SwitchPollingMsg(PollingMessage_StateType eState)
{

		iTE_CEC_Msg(("switch_PollingMessage--> %bX \n",eState));
		switch(eState)
		{
			case sPollingMessage1:

					#ifdef	_CEC_DEVICE_PLAYBACK_
						ucMyselfAddr=DEVICE_ID_PLAYBACK1;
					#endif

					#ifdef	_CEC_DEVICE_AVR_
						ucMyselfAddr=DEVICE_ID_AUDIO;
					#endif

//					cec_w(REG_CEC_TARGET_ADDR, ucMyselfAddr);
					cec_TxCmdSet(ucMyselfAddr, ePollingMessage, 1);
				break;
			case sPollingMessage2:

					#ifdef	_CEC_DEVICE_PLAYBACK_
						ucMyselfAddr=DEVICE_ID_PLAYBACK2;
					#endif

					#ifdef	_CEC_DEVICE_AVR_
						ucMyselfAddr=DEVICE_ID_AUDIO;
					#endif

//					cec_w(REG_CEC_TARGET_ADDR, ucMyselfAddr);
					cec_TxCmdSet(ucMyselfAddr, ePollingMessage, 1);
				break;
			case sPollingMessage3:

					#ifdef	_CEC_DEVICE_PLAYBACK_
						ucMyselfAddr=DEVICE_ID_PLAYBACK3;
					#endif

					#ifdef	_CEC_DEVICE_AVR_
						ucMyselfAddr=DEVICE_ID_AUDIO;
					#endif

//					cec_w(REG_CEC_TARGET_ADDR, ucMyselfAddr);
					cec_TxCmdSet(ucMyselfAddr, ePollingMessage, 1);
				break;
			case sReportPhyAdr:
					CECTxCmd.ucOperand1 = ucTxPhyAdr[0];
					CECTxCmd.ucOperand2 = ucTxPhyAdr[1];
					CECTxCmd.ucOperand3 = cec_GetDeviceType(ucMyselfAddr);
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eReportPhysicalAddress, 5);
				break;
			case sReportDeviceVendorID:
					CECTxCmd.ucOperand1 = 0;
					CECTxCmd.ucOperand2 = 0;
					CECTxCmd.ucOperand3 = 0;
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eDeviceVendorID, 5);
				break;

		}
		PMState=eState;
}
//****************************************************************************
void PollingMessage(void)
{
	switch(PMState){
		case sPollingMessage1:
				if(OldTxCECcmd==ePollingMessage){
					switch(CECTxState){
						case sCECReceiverNack:
						case sCECReceiverFail:
								CEC_SwitchPollingMsg(sReportPhyAdr);
								break;

						case sCECOk:
						case sCECNone:
								CEC_SwitchPollingMsg(sPollingMessage2);
								break;
					}
				}
			break;
		case sPollingMessage2:
				if(OldTxCECcmd==ePollingMessage){
					switch(CECTxState){
						case sCECReceiverNack:
						case sCECReceiverFail:
								CEC_SwitchPollingMsg(sReportPhyAdr);
								break;

						case sCECOk:
						case sCECNone:
								CEC_SwitchPollingMsg(sPollingMessage3);
								break;
					}
				}
			break;
		case sPollingMessage3:
				if(OldTxCECcmd==ePollingMessage){
					switch(CECTxState){
						case sCECReceiverFail:
								CEC_SwitchPollingMsg(sReportPhyAdr);
								break;

						case sCECOk:
						case sCECNone:
								CEC_SwitchPollingMsg(sPMnone);
								break;
					}
				}
			break;
		case sReportPhyAdr:
				if(OldTxCECcmd==eReportPhysicalAddress){
					switch(CECTxState){
						case sCECReceiverNack:
						case sCECReceiverFail:
								break;

						case sCECOk:
						case sCECNone:
								CEC_SwitchPollingMsg(sReportDeviceVendorID);
								break;
					}
				}
		case sReportDeviceVendorID:
				if(OldTxCECcmd==eDeviceVendorID){
					switch(CECTxState){
						case sCECReceiverNack:
						case sCECReceiverFail:
								break;

						case sCECOk:
						case sCECNone:
								CEC_SwitchPollingMsg(sPMnone);
								break;
					}
				}
		default:
			break;
	}
}



 //****************************************************************************
void Switch_OneTouchPlayState(OneTouchPlay_StateType uState)
{
	iTE_CEC_Msg(("Switch_OneTouchPlayState--> %bX \n",uState));
	if(ucMyselfAddr!=DEVICE_ID_TV){
		OTState=uState;

		switch(uState){
			case sImageViewOn:			//			=0x04,	// follower:TV, switch	--> Broadcst
					cec_TxCmdSet(DEVICE_ID_TV, eImageViewOn, 2);
				break;
			case sActiveSource:			//			=0x82,	// follower:TV, switch	--> Broadcst , Directly address
					CECTxCmd.ucOperand1 = ucTxPhyAdr[0];
					CECTxCmd.ucOperand2 = ucTxPhyAdr[1];
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eActiveSource, 4);
				break;
			case sTextViewOn:			//			=0x0D,	// follower:TV
					cec_TxCmdSet(DEVICE_ID_BROADCAST, eTextViewOn, 2);
				break;
			default:
				break;
		}
	}
}
  //****************************************************************************
void Switch_SystemAudioControl(SystemAudioControl_StateType State)
{
	iTE_CEC_Msg(("!!! SwitchSystemInfoFeature -> %bX !!!\n",State));

	SACState=State;

	switch(SACState)
	{
		case sComesOutOfStandby:
				bSystemAudioMode=1;
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eRequestActiveSource, 2);
			break;
		case sSetSystemAudioModeToTV:
				CECTxCmd.ucOperand1 = bSystemAudioMode;
				cec_TxCmdSet(DEVICE_ID_TV, eSetSystemAudioMode, 3);
//				CreatTimerTask(eCECTimeOut,200,SysTimerTask_OnceRun);
			break;
		case sWaitingFeatureAbort:
//				CreatTimerTask(eCECTimeOut,1000,SysTimerTask_OnceRun);
			break;
		case sSetSystemAudioModeToAll:
				CECTxCmd.ucOperand1 = bSystemAudioMode;
				cec_TxCmdSet(DEVICE_ID_BROADCAST, eSetSystemAudioMode, 3);
			break;
	}

}
//****************************************************************************
#if ARC_SUPPORT	//no support
iTE_u1 CheckUpDownPhyAdr(void)
{
#if Fix_ucRxPhyAdr
	iTE_u8 i;
	iTE_u1 Result=iTE_FALSE;
	iTE_u8 pA,pB,pC,pD,MyLevel,TargetLevel;

	for(i=0;i<4;i++)
	{
		if((ucRxPhyAdr[i][0]==CECList[ucInitatorAddr].PhyicalAddr1) && (ucRxPhyAdr[i][1]==CECList[ucInitatorAddr].PhyicalAddr2) )
		{
			return iTE_TRUE;
		}
	}

	pA = ((CECList[ucInitatorAddr].PhyicalAddr1)&0xF0)>>4;
	pB = ((CECList[ucInitatorAddr].PhyicalAddr1)&0x0F);
	pC = ((CECList[ucInitatorAddr].PhyicalAddr2)&0xF0)>>4;
	pD = ((CECList[ucInitatorAddr].PhyicalAddr2)&0x0F);

	if( pA==0 && pB==0 && pC==0 && pD==0 )
		TargetLevel = 0;
	else if( pB==0 && pC==0 && pD==0 )
		TargetLevel = 1;
	else if( pC==0 && pD==0 )
		TargetLevel = 2;
	else if( pD==0 )
		TargetLevel = 3;

	pA = ((ucTxPhyAdr[0])&0xF0)>>4;
	pB = ((ucTxPhyAdr[0])&0x0F);
	pC = ((ucTxPhyAdr[1])&0xF0)>>4;
	pD = ((ucTxPhyAdr[1])&0x0F);

	if( pA==0 && pB==0 && pC==0 && pD==0 )
		MyLevel = 0;
	else if( pB==0 && pC==0 && pD==0 )
		MyLevel = 1;
	else if( pC==0 && pD==0 )
		MyLevel = 2;
	else if( pD==0 )
		MyLevel = 3;


	if(MyLevel>TargetLevel)
		Result=iTE_TRUE;

	return Result;
#else
	return iTE_TRUE;
#endif
}
//****************************************************************************
void switch_ARCState(ARC_StateType uState)
{
	iTE_s8* ARCStr[] = {
	"sRequestARCInitiation",
	"sInitiateARC",
	"sReportARCInitiated",
	"sRequestARCTermination",
	"sTerminateARC",
	"sReportARCTermination",
	"sARCNone",
	} ;
	iTE_u8	ucTemp;

	if(ucMyselfAddr==DEVICE_ID_AUDIO||ucMyselfAddr==DEVICE_ID_TV)
	{
		iTE_CEC_Msg(("switch ARCState -> %s  !!! \n",ARCStr[uState])) ;
		ARCState=uState;
		switch(ARCState)
		{
			case 	sRequestARCInitiation:
						cec_TxCmdSet(ucInitatorAddr, eRequestARCInitiation, 2);
					break;
			case 	sInitiateARC:
						cec_TxCmdSet(ucInitatorAddr, eInitiateARC, 2);
						ucTemp=cec_r(REG_HEAC_CTRL)&(~(B_ARC_TX_EN|B_ARC_RX_EN|B_SPDIFO_EN|B_HEC_EN));
						if(ucMyselfAddr==0){
							ucTemp |= (B_ARC_TX_EN);
						}else{
							ucTemp |= (B_ARC_RX_EN|B_SPDIFO_EN);
						}
						cec_w(REG_HEAC_CTRL,ucTemp);
						ARCState=sARCNone;
						iTE_CEC_Msg((" ARC on \n"));
					break;
			case sReportARCInitiated:
						cec_TxCmdSet(ucInitatorAddr, eReportARCInitiated, 2);
						ucTemp=cec_r(REG_HEAC_CTRL)&(~(B_ARC_TX_EN|B_ARC_RX_EN|B_SPDIFO_EN|B_HEC_EN));
						if(ucMyselfAddr==0){
							ucTemp |= (B_ARC_TX_EN);
						}else{
							ucTemp |= (B_ARC_RX_EN|B_SPDIFO_EN);
						}
						cec_w(REG_HEAC_CTRL,ucTemp);
						ARCState=sARCNone;
					break;
			case sRequestARCTermination:
						cec_TxCmdSet(ucInitatorAddr, eRequestARCTermination, 2);
					break;
			case sTerminateARC:
						cec_TxCmdSet(ucInitatorAddr, eTerminateARC, 2);
						ucTemp=cec_r(REG_HEAC_CTRL)&(~(B_ARC_TX_EN|B_ARC_RX_EN|B_SPDIFO_EN|B_HEC_EN));
						cec_w(REG_HEAC_CTRL,ucTemp);
						ARCState=sARCNone;
//						DumpiT6601Reg();
					break;
			case sReportARCTermination:
						cec_TxCmdSet(ucInitatorAddr, eReportARCTerminated, 2);
						ucTemp=cec_r(REG_HEAC_CTRL)&(~(B_ARC_TX_EN|B_ARC_RX_EN|B_SPDIFO_EN|B_HEC_EN));
						cec_w(REG_HEAC_CTRL,ucTemp);
						ARCState=sARCNone;
//						DumpiT6601Reg();
						iTE_CEC_Msg((" ARC off \n"));
					break;
			case sARCNone:
					break;
		}
	}
}
//****************************************************************************
void ARCHandler(void)
{
	switch(ARCState)	{
		case sRequestARCInitiation:
		case sInitiateARC:
		case sReportARCInitiated:
		case sRequestARCTermination:
		case sTerminateARC:
		case sReportARCTermination:
		case sARCNone:
		default:
			break;
	}
}
//****************************************************************************
void SystemAudioControl(void)		//for Audio control system CTS 11.2.15
{
	switch(SACState)	{
		case sSACnone:
			// TODO: Nothing.
			break;

		case sComesOutOfStandby:
				//please refer CEC Figure 27 A in HDMI Specification 1.4
				//Request Active Source then Waitting Active Source command
				if(CECRxCmd.ucOpcode==eActiveSource){
						Switch_SystemAudioControl(sSetSystemAudioModeToTV);
				}
			break;
		case sSetSystemAudioModeToTV:
				if(OldTxCECcmd==eSetSystemAudioMode){
					switch(CECTxState){
						case sCECReceiverFail:
								Switch_SystemAudioControl(sSetSystemAudioModeToAll);
								break;
						case sCECReceiverNack:
								iTE_CEC_Msg(("!!! Nack !!!\n"));
//								if(TimeOutCheck(eCECTimeOut)==iTE_TRUE)
									Switch_SystemAudioControl(sSetSystemAudioModeToAll);

								break;
						case sCECOk:
						case sCECNone:
								Switch_SystemAudioControl(sWaitingFeatureAbort);
								break;

					}
				}
			break;
		case sWaitingFeatureAbort:
				if(CECRxCmd.ucOpcode==eFeatureAbort){
						Switch_SystemAudioControl(sSACnone);	//finish < Set system Audio Mode >
				}else{
//					if(TimeOutCheck(eCECTimeOut)==iTE_TRUE)
						Switch_SystemAudioControl(sSetSystemAudioModeToAll);
				}
			break;

	}
}
#endif
//****************************************************************************
CEC_DEVICE_TYPE cec_GetDeviceType(iTE_u8 ucMyPhyAdr)
{
	switch(ucMyPhyAdr){
			case DEVICE_ID_TV:
				return	CEC_DEV_TYPE_TV;
			case DEVICE_ID_RECORDING1:
			case DEVICE_ID_RECORDING2:
			case DEVICE_ID_RECORDING3:
				 return	CEC_DEV_TYPE_RECORDING_DEV;
			case DEVICE_ID_RESERVED1:
			case DEVICE_ID_RESERVED2:
			case DEVICE_ID_FREEUSE:
			case DEVICE_ID_BROADCAST:
				 return	CEC_DEV_TYPE_RESERVED;
			case DEVICE_ID_TUNER1:
			case DEVICE_ID_TUNER2:
			case DEVICE_ID_TUNER3:
			case DEVICE_ID_TUNER4:
				 return	CEC_DEV_TYPE_TUNER;
			case DEVICE_ID_PLAYBACK1:
			case DEVICE_ID_PLAYBACK2:
			case DEVICE_ID_PLAYBACK3:
				 return	CEC_DEV_TYPE_PLAYBACK_DEV;
			case DEVICE_ID_AUDIO:
				return	CEC_DEV_TYPE_AUDIO_SYS;
			default:
				return	CEC_DEV_TYPE_PURE_CEC_SWITCH;	//CEC switch
				return	CEC_DEV_TYPE_VIDEO_PROCESSOR;	//Video processor
	}
}
//****************************************************************************
void CECUICommandCode(iTE_u8 UIKey)
{
	switch(UIKey){
		case 0x40:	//Power
				if(u8_PowerStatus==0 || u8_PowerStatus==2)
					u8_PowerStatus=1;	// standby
				else
					u8_PowerStatus=0;	// on
			break;
		case 0x41:	//Volume Up
				if((uCECAudioStatus.Map.AudioVolumeStatus) <0x7F)
					uCECAudioStatus.Map.AudioVolumeStatus++;

				uCECAudioStatus.Map.AudioMuteStatus=iTE_FALSE;
				iTE_CEC_Msg(("CEC volume = %bX \n",uCECAudioStatus.Map.AudioVolumeStatus));
			break;
		case 0x42:	//Volume Down
				if((uCECAudioStatus.Map.AudioVolumeStatus) >0x00){
					uCECAudioStatus.Map.AudioVolumeStatus--;
					uCECAudioStatus.Map.AudioMuteStatus=iTE_FALSE;
				}else{
					if((uCECAudioStatus.Map.AudioVolumeStatus) == 0x00)
						uCECAudioStatus.Map.AudioMuteStatus=iTE_TRUE;
				}
			break;
		case 0x43:	//Mute
				uCECAudioStatus.Map.AudioMuteStatus=~(uCECAudioStatus.Map.AudioMuteStatus);
			break;
		case 0x65:	//Mute
				uCECAudioStatus.Map.AudioMuteStatus=iTE_TRUE;
			break;
		case 0x6C:	//Power OFF
				u8_PowerStatus=1;	// standby
			break;
		case 0x6D:	//Power ON
				u8_PowerStatus=0;	// on
			break;
	}

}

//****************************************************************************
//****************************************************************************
void OneTouchPlayHandler(void)
{
	switch(OTState){
		case 	sImageViewOn:			//			=0x82,	// follower:TV, switch	--> Broadcst , Directly address
				if(OldTxCECcmd==eImageViewOn){
					switch(CECTxState){
						case 	sCECReceiverFail:
							break;
						case sCECOk:
						case sCECNone:
								Switch_OneTouchPlayState(sActiveSource);
							break;
					}
				}
				break;
		case sActiveSource:			//			=0x04,	// follower:TV, switch	--> Broadcst
				if(OldTxCECcmd==eActiveSource){
					switch(CECTxState){
						case 	sCECReceiverFail:
							break;
						case sCECOk:
						case sCECNone:
								Switch_OneTouchPlayState(sTextViewOn);
							break;
					}
				}
				break;
		case sTextViewOn:			//			=0x0D,	// follower:TV
				if(OldTxCECcmd==sTextViewOn)	{
					switch(CECTxState){
						case sCECReceiverFail:
							break;
						case sCECOk:
						case sCECNone:
								Switch_OneTouchPlayState(sOTNone);
							break;
					}
				}
				break;
		default:
				break;

	}

}
//****************************************************************************
void SwitchHECState(HEC_StateType	uState)
{
	HECState=uState;
}
//****************************************************************************
SYS_STATUS CEC_RxPush(void)
{
	iTE_CEC_Msg(("CEC_RxPush "));
	if(iTE_TRUE == CEC_QueueFull(&CECRxQ))
		return SYS_FAIL;

	cec_br(REG_RX_HEADER, 19, (iTE_pu8)&(CECRxQ.QBuf[CECRxQ.ucWptr]));
	CECRxQ.ucWptr++;

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS CEC_RxPull(P_CEC_FRAME pRxCmd)
{
	if(CEC_QueueEmpty(&CECRxQ))
		return SYS_FAIL;
//	iTE_CEC_Msg(("ucHeader=0x%x\n",(int)CECRxQ.QBuf[CECRxQ.ucRptr].ucHeader));
#if 0
	pRxCmd = &(CECRxQ.QBuf[CECRxQ.ucRptr]);
	iTE_CEC_Msg(("ucHeader=0x%x\n",(int)pRxCmd->ucHeader));
//	pCECRxCmd = pRxCmd;
#else
//*pRxCmd = (CECRxQ.QBuf[CECRxQ.ucRptr]);
	memcpy(pRxCmd, &(CECRxQ.QBuf[CECRxQ.ucRptr]), sizeof(CEC_FRAME));
#endif
	CECRxQ.ucRptr++;
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS CEC_TxPush(P_CEC_FRAME pTxCmd)
{
	 iTE_CEC_Msg(("CEC_TxPush "));
	if(iTE_TRUE == CEC_QueueFull(&CECTxQ))
		return SYS_FAIL;

	memcpy(&CECTxQ.QBuf[CECTxQ.ucWptr], pTxCmd, sizeof(CEC_FRAME));
	CECTxQ.ucWptr++;
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS CEC_TxPull(void)
{
	iTE_u8 ucTemp,ucSize;
	iTE_CEC_Msg(("CEC_TxPull \n"));
	if(CEC_QueueEmpty(&CECTxQ))
		return SYS_FAIL;

	ucSize = CECTxQ.QBuf[CECTxQ.ucRptr].ucSize;

	cec_w(REG_CEC_TARGET_ADDR ,(CECTxQ.QBuf[CECTxQ.ucRptr].ucHeader & 0xf0)>>4);
	cec_bw(REG_TX_HEADER, ucSize, (iTE_ps8)&(CECTxQ.QBuf[CECTxQ.ucRptr]));
	cec_w(REG_CEC_OUT_NUM, ucSize);

	iTE_CEC_Msg(("TxCmd: "));
	for(ucTemp=0; ucTemp < ucSize; ucTemp++){
		iTE_CEC_Msg(("data[%d]=0x%x, ",(int)ucTemp,(int)*((iTE_u8*)&(CECTxQ.QBuf[CECTxQ.ucRptr])+ucTemp)) );
	}
	CECTxQ.ucRptr++;
	return SYS_SUCCESS;
}
//****************************************************************************
iTE_u1 CEC_QueueFull(pCEC_QBuf pCECBuf)
{
	iTE_CEC_Msg(("ucWptr=%d, ucRptr=%d\n", (int)pCECBuf->ucWptr, (int)pCECBuf->ucRptr));
	if(pCECBuf->ucWptr >= (pCECBuf->ucRptr+Q_SIZE))
		return iTE_TRUE;
	if(pCECBuf->ucRptr >= Q_SIZE){
		pCECBuf->ucRptr -= Q_SIZE;
		pCECBuf->ucWptr -= Q_SIZE;
	}
	return iTE_FALSE;
}
//****************************************************************************
iTE_u1 CEC_QueueEmpty(pCEC_QBuf pCECBuf)
{
	iTE_CEC_Msg(("ucRptr=%d, ucWptr=%d\n",(int)pCECBuf->ucRptr,(int)pCECBuf->ucWptr) );
	if(pCECBuf->ucRptr >= pCECBuf->ucWptr)
		return iTE_TRUE;
	else
		return iTE_FALSE;
}
//****************************************************************************
SYS_STATUS cec_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data)
{
	iTE_u8 ucRdData;

	if(InvMask){
		if(~InvMask){
			  iTE_i2c_read_byte(CECAddr, Offset, 1, &ucRdData, CEC_I2C_Dev);
			  ucRdData &= (~InvMask);
			  Data |= ucRdData;
		}
		iTE_i2c_write_byte(CECAddr, Offset, 1, &Data, CEC_I2C_Dev);
	}else{
	}

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS cec_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_write_byte(CECAddr, Offset, Count, pData, CEC_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS cec_w(iTE_u8 Offset, iTE_u8 Data)
{
	iTE_i2c_write_byte(CECAddr, Offset, 1, &Data, CEC_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS	cec_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_read_byte(CECAddr, Offset, Count, pData, CEC_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
iTE_u8 cec_r(iTE_u8 Offset)
{
	iTE_u8 ucRdData;

	iTE_i2c_read_byte(CECAddr, Offset, 1, &ucRdData, CEC_I2C_Dev);

	return ucRdData;
}
#endif
