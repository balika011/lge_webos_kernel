///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <MHL_Rx.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
//****************************************************************************
#define _MSC_CONTROL_PACKET_TAB_
#define _MHL_RX_INIT_TAB_
#include "HDMI_Switch.h"
#include "EDID.h"
#include "HDMI_Tx.h"
//#include "config.h"
//#include "../mcu.h"
#if USING_MHL
#include "MHL_Rx.h"
#include "../IT66332_Customer/MSC_Handler.h"
//****************************************************************************
iTE_u16 WakeFailCnt = 0;
iTE_u16 DisvFailCnt = 0;
iTE_u1 bEnCBusDbgMode = EnCBusDbgMode;  // Enable Debug Mode : TRUE
iTE_u8 txscrpad[16], rxscrpad[16];
iTE_u8 fwtxpktbyte, fwtxpktdata[32];
iTE_u8 txmsgdata[2];
iTE_u8 ucRCPState = 0;
iTE_u8 ucRapState = 0;
//MSC_MSG_STATE stRcpState, stRapState, stUcpState;
//iTE_u8 RxUcpMsgSeq = 0, TxUcpMsgSeq = 0;
//iTE_u8 TxRcpMsgSeq = 0, RxRcpMsgSeq = 0;
//iTE_u8 RxRapMsgSeq = 0, TxRapMsgSeq=0;
//iTE_u8 ucMSCBusyCnt=0,ucMSCFailCnt = 0;
//iTE_u8 ucMSGNackCnt = 0;
iTE_u8 ucMscFireCnt = 0;			//added for prevent the deadlock cause by recursive
iTE_u1	bMscSending = iTE_FALSE;
MSC_CMD TxMscCmd;
MHL3D_STATE e3DReqState = MHL3D_REQ_DONE;

iTE_u1 bMaskTxPktDoneInt =MaskTxPktDoneInt;
iTE_u1 bMaskRxPktDoneInt = MaskRxPktDoneInt;
iTE_u1 bEnTxMSG = EnTxMSG;

extern MSC_WB_INFO	stTxWbInfo;
extern iTE_u8 ucFwFlag;
//****************************************************************************
void MHL_Reset(void)
{
	hdmi_sw_set(0x17, 0x11, 0x10);
	hdmi_sw_w(0x08, 0xff);
	iTE_Sleep_ms(100);
	hdmi_sw_w(0x08, 0x00);
	hdmi_sw_set(0x17, 0x11, 0x00);
}
void Msc_FailProcess(MSC_STATUS eMscStatus)
{
#if 0
	if(eMscStatus == MSC_RCVABORT)
		Msc_GetMscErrorCode();
#endif
}
MSC_STATUS MHL_MscReqFailCheck(iTE_u8 *ucMHL18)
{
	MSC_STATUS eMscResult=MSC_FAIL;

	iTE_MRx_Msg(("MSC Req Fail Interrupt (Unexpected) ... \n"));
	if(ucMHL18[0]&0x01 ) {	iTE_MRx_Msg(("ERROR: Incomplete Packet !!!\n"));	             }
	if(ucMHL18[0]&0x02 ) {	iTE_MRx_Msg(("ERROR: MSC 100ms TimeOut !!!\n"));	             }
	if(ucMHL18[0]&0x04 ) {	iTE_MRx_Msg(("ERROR: Protocol Error !!!\n"));		             }
	if(ucMHL18[0]&0x08 ) {	iTE_MRx_Msg(("ERROR: Retry > 32 times !!!\n"));		             }
	if(ucMHL18[0]&0x10 ) {
		iTE_MRx_Msg(("ERROR: Receive ABORT Packet !!!\n"));
		iTE_Sleep_ms(WAITABORTNUM);
		eMscResult = MSC_RCVABORT;
	}
	if(ucMHL18[0]&0x20 ) {
		iTE_MRx_Msg(("MSC_MSG Requester Receive NACK Packet !!! \n"));
		eMscResult = MSC_RCVNACK;
	}
	if(ucMHL18[0]&0x40 ) {
		iTE_MRx_Msg(("***************************************************************\n"));
		iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose at 1st Packet !!! \n"));
		eMscResult = MSC_ARBLOSE;
//		MHL_Reset();
	}
	if(ucMHL18[0]&0x80 ) {
		iTE_MRx_Msg(("***************************************************************\n"));
		iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose before 1st Packet !!!\n"));
		eMscResult = MSC_ARBLOSE;
//		MHL_Reset();
	}
	if(ucMHL18[1]&0x01 ) {
		iTE_MRx_Msg(("ERROR: TX FW Fail in the middle of the command sequence !!!\n"));
		eMscResult = MSC_FWTXFAIL;
	}
	if(ucMHL18[1]&0x02 ) {
		iTE_MRx_Msg(("ERROR: TX Fail because FW mode RxPktFIFO not empty !!!\n"));
		eMscResult = MSC_FWRXPKT;
	}

	return eMscResult;
}

MSC_STATUS Msc_MscFire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd)
{
	MSC_STATUS eMscResult;
	iTE_u8	ucTxBakBuf[2];
	iTE_u1 bRetryFlag=iTE_FALSE;
	iTE_u8	ucRetryCnt=0;

	if(++ucMscFireCnt > CBUSRTYMAX<<2){
		iTE_MRx_Msg(("Msc_MscFire recursive too many times\n"));
		ucMscFireCnt = 0;
		return MSC_FAIL;
	}

	do{
		eMscResult = Msc_Fire(eMscCtlPkt, MscCmd);

		if( eMscResult == MSC_RCVABORT){
			if(Msc_Fire(MSC_PKT_GET_MSC_ERRORCODE, &TxMscCmd)==MSC_SUCCESS){
				iTE_MRx_Msg(("GET_MSC_ERRORCODE=0x%2X\n",(int)mhl_rx_r(0x56)));
			}
		}

		if (eMscResult != MSC_SUCCESS){
			ucRetryCnt++;

			if(eMscResult != MSC_FWRXPKT){		//should always TRUE
				if(mhl_rx_r(0x04) & 0x10){
					iTE_MRx_Msg(("Check MSC_MSG interrupt to prevent deadlock ...\n"));
					mhl_rx_w(0x04, 0x10);
					iTE_MRx_Msg(("MSC Rx MSC_MSG Interrupt ...\n"));
					mhl_rx_br(0x54, 0x02, ucTxBakBuf );
					Msc_MsgHandler();
					mhl_rx_bw(0x54, 0x02, ucTxBakBuf);
				}else{
					iTE_Sleep_ms(200);
				}
			}
			if(eMscCtlPkt == MSC_PKT_WRITE_BURST){
				mhl_rx_set(0x5C, 0x01, 0x01);	//TxPktFIFO clear for Retry
				Msc_WriteBurstDataFill(stTxWbInfo.ucOffset, stTxWbInfo.ucCnt, stTxWbInfo.pucTxScrpad);
				iTE_MRx_Msg(("Retry WRITE_BURST fire command ...\n"));
			}
			if(EnCBusReDisv && ucRetryCnt>= CBUSRTYMAX && bRetryFlag==iTE_FALSE){
				iTE_MRx_Msg(("ERROR: CBUS Link Layer Error ==> Retry CBUS Discovery Process !!!\n"));
				mhl_rx_set(0x0f, 0x10, 0x10);
				mhl_rx_w(0x08, 0xff);
				mhl_rx_w(0x09, 0xff);
				mhl_rx_set(0x0f, 0x10, 0x00);
				bRetryFlag = iTE_TRUE;
				ucRetryCnt = 0;
			}
			iTE_MRx_Msg(("Retry this command again ... ==> %dth retry\n"));
		}
	}while((eMscResult != MSC_SUCCESS) && (ucRetryCnt < CBUSRTYMAX));

	ucMscFireCnt = 0;
	if(eMscResult != MSC_SUCCESS){
		iTE_MRx_Msg(("*************************************************************\n"));
		iTE_MRx_Msg(("ERROR: MSC Request Maximum Retry Fail\n"));
		return iTE_FALSE;
	}else{
		return iTE_TRUE;
	}

}
MSC_STATUS Msc_MscMsgFire(MSC_CMD *MscCmd){
//	return Msc_Fire(MSC_PKT_MSC_MSG, MscCmd);
	return Msc_MscFire(MSC_PKT_MSC_MSG, MscCmd);
}
MSC_STATUS Msc_Fire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd)
{
	#define CBUSWAITNUM     	(100)
	#define CBUSWAITTIME	(10000)
//	iTE_u8	ucTxData[16];
	//MSC_STATUS eMscResult;
	iTE_u8	ucCBusWaitCnt;


	switch(eMscCtlPkt){
		case	MSC_PKT_WRITE_STAT:
		case	MSC_PKT_SET_INT:
		case	MSC_PKT_READ_DEVCAP:
		case	MSC_PKT_GET_STATE:
		case	MSC_PKT_GET_VENDOR_ID:
		case	MSC_PKT_SET_HPD:
		case	MSC_PKT_CLR_HPD:
		case	MSC_PKT_MSC_MSG:

		case	MSC_PKT_GET_DDC_ERRORCODE:
		case	MSC_PKT_GET_MSC_ERRORCODE:
		case	MSC_PKT_WRITE_BURST:

				mhl_rx_bw(0x54, 0x02, &MscCmd->ucOffset);
				mhl_rx_w(MscFire[eMscCtlPkt][0], MscFire[eMscCtlPkt][1]);
				iTE_MRx_Msg(("MSC_FIRE  Reg0x%x=0x%x\n",(int)MscFire[eMscCtlPkt][0], (int)MscFire[eMscCtlPkt][1]));
			break;
#if 0
//		case		MSC_PKT_GET_SC3_ERRORCODE:
//		case		MSC_PKT_GET_SC1_ERRORCODE:
//		case	MSC_PKT_EOF:
		case	MSC_PKT_ACK:
				ucTxData[0]=0x05;
				ucTxData[1]=Msc_CtlPktTab[MSC_PKT_ACK];
				ucTxData[2]=0x04;
				ucTxData[3]=MscCmd->ucValue;
				mhl_rx_bw(0x58, 4, ucTxData);
				mhl_rx_w(0x51, 0x80);
			break;
//		case	MSC_PKT_NACK:
		case	MSC_PKT_ABORT:
				ucTxData[0]=0x05;
				ucTxData[1]=Msc_CtlPktTab[MSC_PKT_ABORT];
				mhl_rx_bw(0x58, 0x02, ucTxData);
				mhl_rx_w(0x51, 0x80);
			break;

#endif
		default:
			return MSC_FAIL;
	}

	ucCBusWaitCnt = 0;
	while((mhl_rx_r(0x1c)&0x02) && (ucCBusWaitCnt<CBUSWAITNUM)){
		iTE_Sleep_ms(10);
		ucCBusWaitCnt++;
	}
	if(mhl_rx_r(0x05) & 0x02){
		iTE_u8 pucMHL18[2];
		mhl_rx_br(0x18, 0x02, pucMHL18);
		mhl_rx_bw(0x18, 0x02, pucMHL18);
		mhl_rx_w(0x05, 0x02);
		return MHL_MscReqFailCheck(pucMHL18);
	}else if (ucCBusWaitCnt < CBUSWAITNUM){
		return MSC_SUCCESS;
	}else{
		iTE_MRx_Msg(("ERROR: Unknown Issue!!!\n"));
		return MSC_FAIL;
	}
}

//****************************************************************************
void mhl_rx_init(void)
{

	iTE_u16 	usT10usInt;
	iTE_u16	usT10usFlt;
	iTE_u32 	ulTemp;
	iTE_u8 ucTemp;

	iTE_MRx_Msg((" mhl_rx_initS \n"));

	Msc_Init();

	if(CBusTimeAdj){
		ulTemp = fRefClk/10;
		fRefClk += ulTemp;		// add for some HTC phone (RCLK*=1.1)
	}

	usT10usInt = fRefClk/100;
	usT10usFlt = (fRefClk%100) <<7;//* 128 ;
	usT10usFlt /= 100 ;

     	iTE_MRx_Msg(("RCLK=%3d.%03dMHz\n", (int)(fRefClk/1000),(int)(fRefClk%1000)));
     	iTE_MRx_Msg(("T10usInt=0x%03X, T10usFlt=0x%02X\n", (int)usT10usInt, (int)usT10usFlt));
     	mhl_rx_w(0x02, (iTE_u8)(usT10usInt&0xFF));
     	mhl_rx_w(0x03, (iTE_u8)((usT10usInt&0x100)>>1)+usT10usFlt);

	if(CBusTimeAdj){
		fRefClk -= ulTemp;		// add for some HTC phone (RCLK*=1.1)
	}
#if 0
	if((iTE_TRUE == bEnCBusDbgMode) && (MSCCBusDbgCtrl || DDCCBusDbgCtrl)){
		bMaskTxPktDoneInt = iTE_FALSE;
		bMaskRxPktDoneInt = iTE_FALSE;
	}
	ucTemp = bMaskRxPktDoneInt;
	ucTemp = (ucTemp << 2) |  bMaskTxPktDoneInt;
	mhl_rx_w(0x08, ucTemp);

	if(iTE_TRUE == bEnCBusDbgMode)
		mhl_rx_set(0x00, 0x80, 0x80 );
	else
		mhl_rx_set(0x00, 0x80, 0x00 );
#endif
	for(ucTemp=0; ucTemp < MHL_RX_INIT_TAB_SIZE; ucTemp++){
		mhl_rx_set(MHL_RX_INIT_TAB[ucTemp][0],MHL_RX_INIT_TAB[ucTemp][1], MHL_RX_INIT_TAB[ucTemp][2]);
	}

	if( EnStandby ) {
		iTE_MRx_Msg(("IT66332 Enter Standby Mode ...\n"));
	}
	iTE_MRx_Msg((" mhl_rx_initE \n"));
}

void mhl_cbus_irq(void)
{
	MSC_CMD TxMscCmd;
	iTE_u8	MHL04[3],MHL15[7];
	iTE_u8	ucTemp;
	//MSC_STATUS eMscResult=MSC_SUCCESS;

	mhl_rx_br(0x04, 3, MHL04);
	mhl_rx_br(0x15, 7, MHL15);
//	mhl_rx_br(0xA0, 4, MHLA0);

	ucTemp = MHL04[0] & 0x10;;
	MHL04[0] &= 0xEF;

//	mhl_rx_bw(0xA0, 4, MHLA0);
	mhl_rx_bw(0x15, 7, MHL15);
	mhl_rx_bw(0x04, 3, MHL04);
	iTE_SW_Msg(("mhl_cbus_irq status : 0x%x, 0x%x, 0x%x ucTemp=0x%x\n", (int)MHL04[0], (int)MHL04[1], (int)MHL04[2],(int)ucTemp));
	iTE_SW_Msg(("mhl_cbus_irq status : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", (int)MHL15[0], (int)MHL15[1], (int)MHL15[2], (int)MHL15[3], (int)MHL15[4], (int)MHL15[5], (int)MHL15[6]));

	if(MHL04[0] & 0x01){
#if iTE_MRx_Msg_Print
		iTE_MRx_Msg(("CBUS Link Layer TX Packet Done Interrupt ...\n"));
		dbgcbustx();
#endif
	}

	if(MHL04[0] & 0x02){
		iTE_MRx_Msg(("ERROR: CBUS Link Layer TX Packet Fail Interrupt ... \n"));
		if(MHL15[0] & 0x10){	iTE_MRx_Msg(("TX Packet Fail when Retry > 32 times !!!\n"));			}
		if(MHL15[0] & 0x20){   iTE_MRx_Msg(("TX Packet TimeOut !!!\n"));							}
		if(MHL15[0] & 0x40){  	iTE_MRx_Msg(("TX Initiator Arbitration Error !!!\n"));					}
		if(MHL15[0] & 0x80){
			iTE_MRx_Msg(("TX CBUS Hang !!!\n"));
//			hdmi_sw_set(0x08,0x20,0x20);
//			hdmi_sw_set(0x08,0x20,0x00);
		}
	}

	if(MHL04[0] & 0x04){
#if iTE_MRx_Msg_Print
		iTE_MRx_Msg(("CBUS Link Layer RX Packet Done Interrupt ...\n"));
		dbgcbusrx();
#endif
	}

	if(MHL04[0] & 0x08){
		iTE_MRx_Msg(("ERROR: CBUS Link Layer RX Packet Fail Interrupt ... \n"));
		if(MHL15[0] & 0x01){	iTE_MRx_Msg(("RX Packet Fail !!!\n"));								}
		if(MHL15[0] & 0x02){   iTE_MRx_Msg(("RX Packet TimeOut !!!\n"));							}
		if(MHL15[0] & 0x04){   iTE_MRx_Msg(("RX Parity Check Error !!!\n"));						}
		if(MHL15[0] & 0x08){
			iTE_MRx_Msg(("Bi-Phase Error !!!\n"));
//			hdmi_sw_set(0x08,0x20,0x20);
//			hdmi_sw_set(0x08,0x20,0x00);
		}
	}
#if 1
	if(ucTemp){
		iTE_MRx_Msg(("MSC RX MSC_MSG Interrupt ...\n"));
		if(!bMscSending){
			iTE_MRx_Msg(("Handler MSC Rx MSC_MSG \n"));
			mhl_rx_w(0x04, 0x10);
			//read_mscmsg();
			Msc_MsgHandler();
		}
	}
#else
	if(MHL04[0] & 0x10){
		iTE_MRx_Msg(("MSC RX MSC_MSG Interrupt ...\n"));
		read_mscmsg();
	}
#endif

	if(MHL04[0] & 0x20){
		iTE_MRx_Msg(("MSC RX WRITE_STAT Interrupt ...\n"));
#if iTE_MRx_Msg_Print
		if(MHL04[1] & 0x10){	iTE_MRx_Msg(("	=> CLK_MODE = 0x%x\n", (int)mhl_rx_r(0xB1)&0x7));	}
		if(MHL04[2] & 0x40){	iTE_MRx_Msg(("	=> DCAP_RDY = 0x%x\n", (int)mhl_rx_r(0xB0)&0x1));	}
		if(MHL04[2] & 0x20){	iTE_MRx_Msg(("	=> MUTED = 0x%x\n", (int)mhl_rx_r(0xB1)&0x10 >> 4));	}
		if(MHL04[2] & 0x10){	iTE_MRx_Msg(("	=> PATH_EN = 0x%x\n", (int)mhl_rx_r(0xB1)&0x8 >> 3));	}
#endif
	}
	if(MHL04[0] & 0x40){
		iTE_MRx_Msg(("MSC RX WRITE_BURST Interrupt ...\n"));
#if iTE_MRx_Msg_Print
		//printf("write_burst ¤º®e \n");
		iTE_MRx_Msg(("	=> count=%d\n	=>data:", (int)mhl_rx_r(0x64)));
		for(ucTemp = 0; ucTemp < 16; ucTemp++){
			iTE_MRx_Msg(("	0x%x", (int)mhl_rx_r(0xc0+ucTemp)));
		}
		iTE_MRx_Msg(("\n"));
#endif
	}
	if(MHL04[0] & 0x80){	iTE_MRx_Msg(("CBus DDC Hang Interrupt ...\n"));			}
	if(MHL04[1] & 0x01){
		iTE_MRx_Msg(("MSC Req Done Interrupt ...\n"));
	}
	if(MHL04[1] & 0x02){
#if 0
		iTE_MRx_Msg(("MSC Req Fail Interrupt (Unexpected) ... \n"));
		if( MHL15[3]&0x01 ) {	iTE_MRx_Msg(("ERROR: Incomplete Packet !!!\n"));	             }
		if( MHL15[3]&0x02 ) {	iTE_MRx_Msg(("ERROR: MSC 100ms TimeOut !!!\n"));	             }
		if( MHL15[3]&0x04 ) {	iTE_MRx_Msg(("ERROR: Protocol Error !!!\n"));		             }
		if( MHL15[3]&0x08 ) {	iTE_MRx_Msg(("ERROR: Retry > 32 times !!!\n"));		             }
		if( MHL15[3]&0x10 ) {
			iTE_MRx_Msg(("ERROR: Receive ABORT Packet !!!\n"));
			iTE_Sleep_ms(WAITABORTNUM);
			eMscResult = MSC_RCVABORT;
		}
		if( MHL15[3]&0x20 ) {
			iTE_MRx_Msg(("MSC_MSG Requester Receive NACK Packet !!! \n"));
			eMscResult = MSC_RCVNACK;
		}
		if( MHL15[3]&0x40 ) {
			iTE_MRx_Msg(("***************************************************************\n"));
			iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose at 1st Packet !!! \n"));
			eMscResult = MSC_ARBLOSE;
			MHL_Reset();
		}
		if( MHL15[3]&0x80 ) {
			iTE_MRx_Msg(("***************************************************************\n"));
			iTE_MRx_Msg(("Disable HW Retry and MSC Requester Arbitration Lose before 1st Packet !!!\n"));
			eMscResult = MSC_ARBLOSE;
			MHL_Reset();
		}
		if( MHL15[4]&0x01 ) {
			iTE_MRx_Msg(("ERROR: TX FW Fail in the middle of the command sequence !!!\n"));
			eMscResult = MSC_FWTXFAIL;
		}
		if( MHL15[4]&0x02 ) {
			iTE_MRx_Msg(("ERROR: TX Fail because FW mode RxPktFIFO not empty !!!\n"));
			eMscResult = MSC_FWRXPKT;
		}

		if(eMscResult == MSC_RCVABORT){
			TxMscCmd.eCtlPkt = MSC_PKT_GET_MSC_ERRORCODE;
			if(Msc_Fire(&TxMscCmd)==iTE_TRUE){
				iTE_MRx_Msg(("GET_MSC_ERRORCODE=0x%2X\n",mhl_rx_r(0x56)));
			}
		}
#else
		if( MSC_RCVABORT == MHL_MscReqFailCheck(&MHL15[3])){
			if(Msc_Fire(MSC_PKT_GET_MSC_ERRORCODE, &TxMscCmd)==MSC_SUCCESS){
				iTE_MRx_Msg(("GET_MSC_ERRORCODE=0x%2X\n",(int)mhl_rx_r(0x56)));
			}
		}
#endif
	}

	if(MHL04[1] & 0x04){	iTE_MRx_Msg(("MSC Rpd Done Interrupt ...\n"));					   			}
	if(MHL04[1] & 0x08){
		iTE_MRx_Msg(("MSC Rpd Fail Interrupt ...\n"));
		if(MHL15[5]  & 0x01){	iTE_MRx_Msg(("ERROR: Initial Bad Offset !!!\n"));							}
		if(MHL15[5] & 0x02){	iTE_MRx_Msg(("ERROR: Incremental Bad Offset !!!\n"));					}
		if(MHL15[5] & 0x04){	iTE_MRx_Msg(("ERROR: Invalid Command !!!\n"));						}
		if(MHL15[5] & 0x08){	iTE_MRx_Msg(("ERROR: Receive dPacket in Responder Idle State !!!\n"));		}
		if(MHL15[5] & 0x10){	iTE_MRx_Msg(("ERROR: Incomplete Packet !!!\n"));						}
		if(MHL15[5] & 0x20){   iTE_MRx_Msg(("ERROR: 100ms TimeOut !!!\n"));							}
		if(MHL15[5] & 0x40){	iTE_MRx_Msg(("MSC_MSG Responder Busy ==> Return NACK Packet !!!\n"));	}
		if(MHL15[5] & 0x80){	iTE_MRx_Msg(("ERROR: Protocol Error !!!\n"));							}
		if(MHL15[6] & 0x01){	iTE_MRx_Msg(("ERROR: Retry > 32 times !!!\n"));							}
		if(MHL15[6] & 0x02){
			iTE_MRx_Msg(("ERROR: Receive ABORT Packet !!!\n"));
			iTE_Sleep_ms(WAITABORTNUM);
		}
	}

	if(MHL04[1] & 0x10 ) {	iTE_MRx_Msg(("CLK_MODE Change Interrupt ...\n"));								}
//	if(MHL04[1] & 0x20 ) {	iTE_MRx_Msg(("DDC Req Fail Interrupt ));											}
	if(MHL04[1] & 0x40 ) {	iTE_MRx_Msg(("DDC Rpd Done Interrupt ...\n"));									}

	if(MHL04[1] & 0x80 ) {
		iTE_MRx_Msg(("DDC Rpd Fail Interrupt ...\n"));
		iTE_MRx_Msg(("DDC Rpd Fail Status MHL16=0x%02X, ", (int)MHL15[1]));
		if(MHL15[1] & 0x01){	iTE_MRx_Msg(("RxState=IDLE and Receive non-SOF Packet !!!\n"));				}
		if(MHL15[1] & 0x02){	iTE_MRx_Msg(("RxState/=IDLE and Receive Unexpected Packet !!!\n"));			}
		if(MHL15[1] & 0x04){	iTE_MRx_Msg(("100ms TimeOut !!!\n"));										}
		if(MHL15[1] & 0x08){	iTE_MRx_Msg(("100ms TimeOut caused by Link Layer Error !!!\n"));				}
		if(MHL15[1] & 0x10){	iTE_MRx_Msg(("Receive Unexpected STOP !!!\n"));							}
		if(MHL15[1] & 0x20){	iTE_MRx_Msg(("Transmit Packet Fail !!!\n"));									}
		if(MHL15[1] & 0x40){	iTE_MRx_Msg(("DDC Bus Hang !!!\n"));										}
		if(MHL15[1] & 0x80){	iTE_MRx_Msg(("TxState/=IDLE and Receive New Packet !!!\n"));					}
		if(MHL15[2] & 0x01){	iTE_MRx_Msg(("TxState/=IDLE and Receive DDCTxArbLose !!!\n"));				}
	}

	if(MHL04[2] & 0x01 ) {
//				P4_2 = ~P4_2;
		iTE_MRx_Msg(("WakeUp Done Interrupt ...\n"));
		WakeFailCnt = 0;
		bEnTxMSG = iTE_FALSE;
		//ReqWrtFlag = iTE_FALSE;
	}

	if(MHL04[2] & 0x02 ) {
		WakeFailCnt++;
			iTE_MRx_Msg(("**************************************************************************\n"));
		iTE_MRx_Msg(("WakeUp Fail Interrupt ... %dth Fail !!!\n", WakeFailCnt));
//		MHL_Reset();
		if(( EnWakeUpAdj==iTE_TRUE) && (WakeFailCnt>=WAKEUPADJNUM) && (ucFwFlag!=0xC0))	//V1.07_8
			mhl_rx_set(0x28, 0x08, 0x08);

		if((WakeFailCnt == WAKEUPFAILMAX) && (ucFwFlag!=0xC0)){						//V1.07_9
			mhl_rx_set(0x28, 0x02, 0x02);
			mhl_rx_set(0x28, 0x02, 0x00);
			WakeFailCnt = 0;
		 }
	}

	if(MHL04[2] & 0x04 ) {
		bMscSending = iTE_FALSE;
		iTE_MRx_Msg(("CBUS Discovery Done Interrupt ...\n"));

		if((EnWakeUpAdj == iTE_TRUE) && (ucFwFlag!=0xC0))		//V1.07_8
			mhl_rx_set(0x28, 0x08, CBusTimeAdj<<3);

		if(ucFwFlag == 0xB0){			//V1.06_3
			if(EnHDCPBypass == iTE_FALSE){
				hdmi_tx_set(0xe5, 0xb0, 0x10);		// select HDMITXSRC="01" manually
			}else{
				hdmi_tx_set(0xe5, 0xb0, 0x00);		// select HDMITXSRC automatically
			}
		}
		DisvFailCnt = 0;

		if( EnAutoCapRdy==iTE_FALSE ) {
			iTE_Sleep_ms(CBUSINITIME);
			TxMscCmd.ucOffset = 0x30;
			TxMscCmd.ucValue = MHL_STS_DCAP_RDY;
			Msc_Fire(MSC_PKT_WRITE_STAT, &TxMscCmd);
			iTE_MRx_Msg(("Set CapRdy by FW ...\n"));
		}

		if( EnAutoPathEn==iTE_FALSE ) {
		iTE_Sleep_ms(CBUSINITIME);
			TxMscCmd.ucOffset = 0x31;
			TxMscCmd.ucValue = MHL_STS_PATH_EN;
			while(Msc_Fire(MSC_PKT_WRITE_STAT, &TxMscCmd)!=MSC_SUCCESS);
//			mhl_rx_set(0x0C, 0x02, 0x02);       // FW trigger HW Send Set PathEn
			iTE_MRx_Msg(("Set PathEn by FW ...\n"));
		}

		if( EnAutoHPD==iTE_FALSE ) {
		iTE_Sleep_ms(CBUSINITIME*30);	  	// min 8 for Galxy note Read DevCap after DCAP_RDY
			while(Msc_Fire(MSC_PKT_SET_HPD, &TxMscCmd)!=MSC_SUCCESS);
			iTE_MRx_Msg(("Set HPD by FW ...\n"));
		}

		bEnTxMSG = iTE_TRUE;
	}

	if(MHL04[2] & 0x08 ) {	iTE_MRx_Msg(("CBUS Discovery Fail Interrupt ... %dth Fail !!!\n", DisvFailCnt++));	 /*mhl_CBusReDiscovery();*/}
	if(MHL04[2] & 0x10 ) {	iTE_MRx_Msg(("CBUS PATH_EN Change Interrupt ...\n PATH_EN = %d", (int)(mhl_rx_r(0xB1) & 0x08)>>3));							}
	if(MHL04[2] & 0x20 ) {	iTE_MRx_Msg(("CBUS MUTE Change Interrupt ...\n Current CBUS MUTE Status = %d \n", (mhl_rx_r(0xB1)&0x10)>>4));	}
	if(MHL04[2] & 0x40 ) {
		iTE_MRx_Msg(("CBUS DCapRdy Change Interrupt ...\n"));
		if(mhl_rx_r(0xB0) & 0x01)
			Msc_Read_DevCap();
	}
	if(MHL04[2] & 0x80 ) {	iTE_MRx_Msg(("VBUS Status Change Interrupt ...\n Current VBUS Status = %d \n", (int)(mhl_rx_r(0x10)&0x08)>>3));		}
//	ucTemp = (MHLA0[0] & 0x1E) | ((MHLA0[1]&0x02)>>1);
//	if(ucTemp){
//		Msc_SetIntHandler(ucTemp);
//	}
}

void mhl_set_irq(void)
{
	iTE_u8	ucMRxRegA0[4];
		iTE_MRx_Msg(("mhl_set_irq ...\n"));
	mhl_rx_br(0xA0, 4, ucMRxRegA0);
	mhl_rx_bw(0xA0, 4, ucMRxRegA0);

	if(ucMRxRegA0[0] & 0x01){        	iTE_MRx_Msg(("MHL Device Capability Change Interrupt ...\n"));     	}
	if(ucMRxRegA0[0] & 0x02){		iTE_MRx_Msg(("MHL DSCR_CHG Interrupt ...\n"));					}
	if(ucMRxRegA0[0] & 0x04){         iTE_MRx_Msg(("MHL REQ_WRT Interrupt ...\n"));					}
	if(ucMRxRegA0[0] & 0x08){
		iTE_MRx_Msg(("MHL GNT_WRT Interrupt ...\n"));
		if(e3DReqState == MHL3D_REQ_WRT){
			e3DReqState = MHL3D_GNT_WRT;
		}
	}
	if(ucMRxRegA0[0] & 0x10){
		iTE_MRx_Msg(("MHL 3D_REQ Interrupt ...\n"));
		if((MHLVer == 0x20) || (MHLVer==0x21)){
			if(e3DReqState == MHL3D_REQ_DONE){
				e3DReqState = MHL3D_REQ_START;
			}
		}
	}
	if(ucMRxRegA0[0] & 0xE0){		iTE_MRx_Msg(("ERROR: MHLA0[7:5] Reserved Interrupt ...\n"));		}
	if(ucMRxRegA0[1] & 0x01){		iTE_MRx_Msg(("ERROR: MHLA1[0] Reserved Interrupt ...\n"));		}
	if(ucMRxRegA0[1] & 0x02){		iTE_MRx_Msg(("MHL EDID Change Interrupt ...\n"));				}
	if(ucMRxRegA0[1] & 0xFC){		iTE_MRx_Msg(("ERROR: MHLA1[7:2] Reserved Interrupt ...\n"));		}
	if(ucMRxRegA0[2] & 0xFF){		iTE_MRx_Msg(("ERROR: MHLA2[7:0] Reserved Interrupt ...\n"));		}
	if(ucMRxRegA0[3] & 0xFF){		iTE_MRx_Msg(("ERROR: MHLA3[7:0] Reserved Interrupt ...\n"));		}

	if(e3DReqState != MHL3D_REQ_DONE){
		Msc_3DProcess(&e3DReqState);
	}
}

//****************************************************************************

//****************************************************************************
void read_mscmsg( void )
{
//	Msc_MsgHandler();
}
//****************************************************************************
#if iTE_MRx_Msg_Print
void Msc_ParseDevCap(iTE_u8 ucOffset, iTE_u8 ucData)
{
	switch(ucOffset){
		case	0:
				iTE_MRx_Msg(("DEV_STATE=0x%x\n", (int)ucData));
			break;
		case	1:
				iTE_MRx_Msg(("MHL_Version=0x%x\n",(int)ucData));
			break;
		case	2:
				switch(ucData & 0xF){
					case	0:	iTE_MRx_Msg(("ERROR: DEV_TYPE at least one bit must be set !!!\n"));	break;
					case	1:	iTE_MRx_Msg(("DEV_TYPE = Sink, "));	break;
					case	2:	iTE_MRx_Msg(("DEV_TYPE = Source, "));	break;
					case	3:	iTE_MRx_Msg(("DEV_TYPE = Dongle, "));	break;
					default:		iTE_MRx_Msg(("ERROR: Reserved for future use !!! "));	break;
				}
				iTE_MRx_Msg(("POW=%d, PLIM=%d \n", (int)((ucData&0x10)>>4), (int)((ucData&0x60)>>5)));
			break;
		case	3:
				iTE_MRx_Msg(("ADOPTER_ID_H=0x%X, ", (int)ucData));
			break;
		case	4:
				iTE_MRx_Msg(("ADOPTER_ID_L=0x%X \n",(int)ucData));
			break;
		case	5:
				iTE_MRx_Msg(("\nVID_LINK_MODE:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	SUPP_RGB444\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	SUPP_YCBCR444\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	SUPP_YCBCR422\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	SUPP_PPIXEL\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	SUPP_ISLANDS\n"));
				if(ucData&0x20)	iTE_MRx_Msg(("	SUPP_VGA\n"));
			break;
		case	6:
				iTE_MRx_Msg(("\nAUD_LINK_MODE:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	SUPP_2CH\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	SUPP_8CH\n"));
			break;
		case	7:
				if(ucData & 0x80){
					iTE_MRx_Msg(("\nVIDEO_TYPE:\n"));
					if(ucData&0x01)	iTE_MRx_Msg(("	VT_GRAPHICS\n"));
					if(ucData&0x02)	iTE_MRx_Msg(("	VT_PHOTO\n"));
					if(ucData&0x04)	iTE_MRx_Msg(("	VT_CINEMA\n"));
					if(ucData&0x08)	iTE_MRx_Msg(("	VT_GAME\n"));
				}else{
					iTE_MRx_Msg(("Not Support VIDEO_TYPE\n"));
				}
			break;
		case	8:
				iTE_MRx_Msg(("\nLOG_DEV_MAP:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	LD_DISPLAY\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	LD_VIDEO\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	LD_AUDIO\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	LD_MEDIA\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	LD_TUNER\n"));
				if(ucData&0x20)	iTE_MRx_Msg(("	LD_RECORD\n"));
				if(ucData&0x40)	iTE_MRx_Msg(("	LD_SPEAKER\n"));
				if(ucData&0x80)	iTE_MRx_Msg(("	LD_GUI\n"));
			break;
		case	9:
				iTE_MRx_Msg(("\nBANDWIDTH= %dMHz\n", (int)ucData*5));
			break;
		case	10:
				iTE_MRx_Msg(("FEATURE_FLAG:\n"));
				if(ucData&0x01)	iTE_MRx_Msg(("	RCP_SUPPORT\n"));
				if(ucData&0x02)	iTE_MRx_Msg(("	RAP_SUPPORT\n"));
				if(ucData&0x04)	iTE_MRx_Msg(("	SP_SUPPORT\n"));
				if(ucData&0x08)	iTE_MRx_Msg(("	UCP_SEND_SUPPORT\n"));
				if(ucData&0x10)	iTE_MRx_Msg(("	UCP_RECV_SUPPORT\n"));
			break;
		case	11:
				iTE_MRx_Msg(("DEVICE_ID_H=0x%x, ", (int)ucData));
			break;
		case	12:
				iTE_MRx_Msg(("DEVICE_ID_L=0x%x\n", (int)ucData));
			break;
		case	13:
				iTE_MRx_Msg(("SCRATCHPAD_SIZE=%d Bytes\n", (int)ucData));
			break;
		case	14:
				iTE_MRx_Msg(("INT_SIZE=%d\n", (int)(ucData&0xf)+1));
				iTE_MRx_Msg(("STAT_SIZE=%d\n", (int)(ucData>>4)+1));
			break;
	}
}
#endif
//****************************************************************************
void Msc_Read_DevCap(void)
{
	iTE_u8	ucOffset, ucTemp;

	for(ucOffset=0; ucOffset<0x10; ucOffset++){
		TxMscCmd.ucOffset = ucOffset;

		if(MSC_SUCCESS == Msc_Fire(MSC_PKT_READ_DEVCAP, &TxMscCmd)){
			ucTemp = mhl_rx_r(0x56);
			if((3 == ucOffset) || (4 == ucOffset)){
				mhl_rx_w(0x5B+ucOffset, ucTemp);
			}
#if iTE_MRx_Msg_Print
			Msc_ParseDevCap(ucOffset, ucTemp);
#endif
		}
	}
}
//****************************************************************************

//****************************************************************************
#if iTE_MRx_Msg_Print
void mhl_DbgCBus(iTE_u8 ucTxRxReg)
{
	iTE_u8	ucTemp,ucCBusByteNo;
	iTE_u8	ucCBusData[0x20];

	ucCBusByteNo = (mhl_rx_r(ucTxRxReg) >> 3);
	if(ucCBusByteNo){
		do{
			if(ucCBusByteNo > 16){
				iTE_MRx_Msg(("WARNING: ByteNo=%d > 16!!!\n", (int)ucCBusByteNo));
				ucCBusByteNo = 16;
			}
			iTE_MRx_Msg(("CBus Byte Number = %d\n",(int)ucCBusByteNo));
			mhl_rx_br(ucTxRxReg, ucCBusByteNo*2, ucCBusData);
			for(ucTemp=0; ucTemp<ucCBusByteNo; ucTemp++){
				iTE_MRx_Msg(("CBus Byte[%d] = 0x%X%2X \n", (int)ucTemp,(int)(ucCBusData[ucTemp*2]&0x7),(int)ucCBusData[ucTemp*2+1]));
				switch(ucCBusData[ucTemp*2]&0x06){
					case	0x00:	iTE_MRx_Msg(("DDC "));				break;
					case	0x02:	iTE_MRx_Msg(("Vendor-Specific "));		break;
					case	0x04:	iTE_MRx_Msg(("MSC "));				break;
					case	0x06:	iTE_MRx_Msg(("Reserved "));			break;
				}
				if(ucCBusData[ucTemp*2] & 0x01){
					iTE_MRx_Msg(("Control Packet "));
					parse_ctrlpkt(&ucCBusData[ucTemp*2]);
				}else{
					iTE_MRx_Msg(("Data Packet =0x%2X \n",(int)ucCBusData[ucTemp*2+1]));
				}
			}
			ucCBusByteNo = (mhl_rx_r(ucTxRxReg) >> 3);
		}while(ucCBusByteNo);
	}
}

//****************************************************************************
void dbgcbustx( void )
{
	iTE_MRx_Msg(("\n ----------------- CBus Tx Debug Msg -----------------\n"));
	mhl_DbgCBus(0x58);
	iTE_MRx_Msg(("\n ---------------------------------------------------\n"));
}
//****************************************************************************
void dbgcbusrx( void )
{
	iTE_MRx_Msg(("\n ----------------- CBus Rx Debug Msg -----------------\n"));
	mhl_DbgCBus(0x5A);
	iTE_MRx_Msg(("\n ---------------------------------------------------\n"));
}
//****************************************************************************
void parse_ctrlpkt( iTE_u8* ctrlpkt )
{
    if( (ctrlpkt[0]&0x07)==0x05 ) {
        switch( ctrlpkt[1] ) {
        case 0x32: iTE_MRx_Msg((" ==> EOF \n")); break;
        case 0x33: iTE_MRx_Msg((" ==> ACK \n")); break;
        case 0x34: iTE_MRx_Msg((" ==> NACK \n")); break;
        case 0x35: iTE_MRx_Msg((" ==> ABORT \n")); break;
        case 0x60: iTE_MRx_Msg((" ==> WRITE_STATE or SET_INT \n")); break;
        case 0x61: iTE_MRx_Msg((" ==> READ_DEVCAP \n")); break;
        case 0x62: iTE_MRx_Msg((" ==> GET_STATE \n")); break;
        case 0x63: iTE_MRx_Msg((" ==> GET_VENDOR_ID \n")); break;
        case 0x64: iTE_MRx_Msg((" ==> SET_HPD \n")); break;
        case 0x65: iTE_MRx_Msg((" ==> CLR_HPD \n")); break;
        case 0x68: iTE_MRx_Msg((" ==> MSC_MSG \n")); break;
        case 0x69: iTE_MRx_Msg((" ==> GET_SC1_ERRORCODE \n")); break;
        case 0x6A: iTE_MRx_Msg((" ==> GET_DDC_ERRORCODE \n")); break;
        case 0x6B: iTE_MRx_Msg((" ==> GET_MSC_ERRORCODE \n")); break;
        case 0x6C: iTE_MRx_Msg((" ==> WRITE_BURST \n")); break;
        case 0x6D: iTE_MRx_Msg((" ==> GET_SC3_ERRORCODE \n")); break;
        default  : iTE_MRx_Msg((" ==> ERROR !!! \n"));
        }
    }
    else if( (ctrlpkt[0]&0x07)==0x01 ) {
        switch( ctrlpkt[1] ) {
        case 0x30: iTE_MRx_Msg((" ==> SOF \n")); break;
        case 0x32: iTE_MRx_Msg((" ==> EOF \n")); break;
        case 0x33: iTE_MRx_Msg((" ==> ACK \n")); break;
        case 0x34: iTE_MRx_Msg((" ==> NACK \n")); break;
        case 0x35: iTE_MRx_Msg((" ==> ABORT \n")); break;
        case 0x50: iTE_MRx_Msg((" ==> CONT \n")); break;
        case 0x51: iTE_MRx_Msg((" ==> STOP \n")); break;
        default  : iTE_MRx_Msg((" ==> ERROR !!! \n"));
        }
    }
    else
        iTE_MRx_Msg((" ==> ERROR !!!\n"));
}
#endif
//****************************************************************************
void mhl_CBusReDiscovery(void)
{
#if 0
	iTE_MRx_Msg(("*******************mhl_CBusReDiscovery \n"));
	mhl_rx_set(0x28, 0x02, 0x02);
	iTE_Sleep_ms(1000);
	mhl_rx_set(0x28, 0x02, 0x00);
#endif
}
//****************************************************************************
//****************************************************************************

//****************************************************************************


//****************************************************************************

//****************************************************************************

SYS_STATUS mhl_rx_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data)
{
	iTE_u8		i2c_rdata;

	if(InvMask){
		if(~InvMask){
			iTE_i2c_read_byte(MHLRxAddr, Offset, 1, &i2c_rdata, MHL_Rx_I2C_Dev);
			i2c_rdata &= (~InvMask);
			Data |= i2c_rdata;
		}
		iTE_i2c_write_byte(MHLRxAddr, Offset, 1, &Data, MHL_Rx_I2C_Dev);
	}else{
		  if(~Offset){
		  	iTE_Sleep_ms(Data);
		  }else{
		  	//ToDo for other feature
		  }
	}

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS mhl_rx_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_write_byte(MHLRxAddr, Offset, Count, pData, MHL_Rx_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS mhl_rx_w(iTE_u8 Offset, iTE_u8 Data)
{
	iTE_i2c_write_byte(MHLRxAddr, Offset, 1, &Data, MHL_Rx_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS	mhl_rx_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_read_byte(MHLRxAddr, Offset, Count, pData, MHL_Rx_I2C_Dev);
	return SYS_SUCCESS;
}
//****************************************************************************
iTE_u8 mhl_rx_r(iTE_u8 Offset)
{
	iTE_u8 i2c_rdata;
	iTE_i2c_read_byte(MHLRxAddr, Offset, 1, &i2c_rdata, MHL_Rx_I2C_Dev);
	return i2c_rdata;
}
//****************************************************************************
//****************************************************************************
#endif
