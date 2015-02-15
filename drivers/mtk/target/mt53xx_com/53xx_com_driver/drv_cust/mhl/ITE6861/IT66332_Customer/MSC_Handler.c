///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <MSC_Handler.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#define _MSC_HANDLER_TAB_
#include "IT66332.h"
#include "../IT66332_Source/MHL_Rx.h"
#include "../IT66332_Source/HDMI_Switch.h"
#if USING_MHL
#include "MSC_Handler.h"
#include "../IT66332_Source/EDID.h"

#if USING_CALLOC
#include <stdlib.h>
#else
iTE_u8	uc3DInfor[VIC_3D_MAX];
#endif

MSC_WB_INFO	stTxWbInfo;
MSC_MSG_STATE stRxMscState;

void Msc_Init(void)
{
	stRxMscState.eState = eMSC_FREE;
}

void Msc_MsgHandler(void)
{
	#define RCPBUSYNUM      (50)
	#define RAPBUSYNUM      (50)
	iTE_u8	ucMscReg[2];
	MSC_CMD MscTxCmd;

	mhl_rx_br(0x60, 2, ucMscReg);

	iTE_MRx_Msg(("Msc_MsgHandler cmd=0x%x, data=0x%x\n", (int)ucMscReg[0], (int)ucMscReg[1]));
	switch(ucMscReg[0]){
		case	MSC_MSG_MSGE :
#if iTE_MRx_Msg_Print
				Msc_MsgeHandler((MSC_STATUS_CODE)ucMscReg[1]);
#endif
			break;
		case	MSC_MSG_RAP:
				MscTxCmd.ucOffset = MSC_MSG_RAPK;
				iTE_MRx_Msg(("stMscMsg.unMsgData.eRapAction = 0x%x\n",(int)ucMscReg[1]));

				if(SuppRAPCode[ucMscReg[1]] ) {
					if(iTE_TRUE == Msc_RapHandler((RAP_ACTION_CODE)ucMscReg[1])){
						MscTxCmd.ucValue = RAP_NO_ERROR;
					}
				}else{
					MscTxCmd.ucValue = RAP_UNSUPPORT_ACTION_CODE;
				}
				Msc_MscMsgFire(&MscTxCmd);
			break;
		case MSC_MSG_RAPK :
#if iTE_MRx_Msg_Print
				Msc_Rapk_Handler((RAP_ERROR)ucMscReg[1]);
#endif
			break;
		case	MSC_MSG_RCP:
				if(SuppRCPCode[ucMscReg[1] & 0x7F]){
					if(iTE_FALSE == Msc_RcpHandler(ucMscReg[1])){
						MscTxCmd.ucOffset = MSC_MSG_RCPE;
						MscTxCmd.ucValue = RCP_RESPONDER_BUSY;
						Msc_MscMsgFire(&MscTxCmd);
					}
				}else{
					iTE_MRx_Msg(("Rx RCP = 0x%x not supported \n",(int)ucMscReg[1]));
					MscTxCmd.ucOffset = MSC_MSG_RCPE;
					MscTxCmd.ucValue = RCP_INEFFECTIVE_KEY;
					Msc_MscMsgFire(&MscTxCmd);
				}

				MscTxCmd.ucOffset = MSC_MSG_RCPK;
				MscTxCmd.ucValue = ucMscReg[1];
				Msc_MscMsgFire(&MscTxCmd);
			break;
		case	MSC_MSG_RCPK :
#if iTE_MRx_Msg_Print
				Msc_RcpkHandler(ucMscReg[1]);
#endif
			break;

		case	MSC_MSG_RCPE :
#if iTE_MRx_Msg_Print
				Msc_RcpeHandler((RCP_ERROR)ucMscReg[1]);
#endif
			break;

		case	MSC_MSG_UCP  :
				if(iTE_FALSE == Msc_UcpHandler(ucMscReg[1])){
					MscTxCmd.ucOffset = MSC_MSG_UCPE;
					MscTxCmd.ucValue = UCP_INEFFECTIVE_KEY;
					Msc_MscMsgFire(&MscTxCmd);
				}

				MscTxCmd.ucOffset = MSC_MSG_UCPK;
				MscTxCmd.ucValue = ucMscReg[1];
				Msc_MscMsgFire(&MscTxCmd);
			break;

		case	MSC_MSG_UCPK :
#if iTE_MRx_Msg_Print
				Msc_UcpkHandler(ucMscReg[1]);
#endif
			break;

		case	MSC_MSG_UCPE :
#if iTE_MRx_Msg_Print
				Msc_UcpeHandler((UCP_ERROR)ucMscReg[1]);
#endif
			break;

		default :
				iTE_MRx_Msg(("ERROR: Unknown MSC_MSG sub-command code 0x%02X !!!\n", (int)ucMscReg[1]));
				MscTxCmd.ucOffset = MSC_MSG_MSGE;
				MscTxCmd.ucValue = MSC_INVALID_CODE;
				Msc_MscMsgFire(&MscTxCmd);
			break;
	}
}
#if iTE_MRx_Msg_Print
void	Msc_MsgeHandler(MSC_STATUS_CODE eMsgStatus)
{
	iTE_Msg(("Rx MSGE => 0x%x \n",(int)eMsgStatus));
	switch(eMsgStatus){
		case MSC_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case MSC_INVALID_CODE:
				iTE_MRx_Msg(("ERROR: Invalid sub-command code !!!\n"));
			break;
		default  :
				iTE_MRx_Msg(("ERROR: Unknown MSC_MSG status code 0x%02X !!!\n", (int)eMsgStatus));
			break;
	}
}
#endif

iTE_u1 Msc_RcpHandler(iTE_u8 ucRcpKey)
{
	Msc_RcpKeyParse(ucRcpKey);
	switch(ucRcpKey){
		//Todo
		default:
			break;
	}
	return iTE_TRUE;
}

#if iTE_MRx_Msg_Print
void Msc_RcpkHandler(iTE_u8 ucRcpKey)
{
	iTE_Msg(("RX RCPK => "));
	Msc_RcpKeyParse( ucRcpKey);
}
void Msc_RcpeHandler(RCP_ERROR eRcpError)
{
	iTE_Msg(("RX RCPE => "));
	switch(eRcpError){
		case	RCP_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case	RCP_INEFFECTIVE_KEY:
				iTE_MRx_Msg(("ERROR: Ineffective RCP Key Code !!!\n"));
			break;
		case	RCP_RESPONDER_BUSY:
				iTE_MRx_Msg(("Responder Busy ...\n"));
			break;
		default:
				iTE_MRx_Msg(("ERROR: Unknown RCP status code !!!\n"));
			break;
	}
}
#endif

iTE_u1 Msc_RapHandler(RAP_ACTION_CODE eRapCmd)
{
	iTE_Msg(("RX MSC_MSG_RAP => "));

	switch(eRapCmd){
		case RAP_POLL:
				iTE_MRx_Msg(("Poll\n"));
			break;
		case RAP_CONTENT_ON:
				iTE_MRx_Msg(("Change to CONTENT_ON state\n"));
				hdmi_sw_set(0x68, 0x22, 0x00);
			break;
		case RAP_CONTENT_OFF:
				iTE_MRx_Msg(("Change to CONTENT_OFF state\n"));
				hdmi_sw_set(0x68, 0x22, 0x22);
			break;
		default:
				iTE_MRx_Msg(("ERROR: Unknown RAP action code 0x%2X !!!\n", eRapCmd));
				return iTE_FALSE;
	}
	return iTE_TRUE;
}

#if iTE_MRx_Msg_Print
void Msc_Rapk_Handler(RAP_ERROR eRapError)
{
	iTE_Msg(("RX RAPK => "));
	switch(eRapError) {
		case RAP_NO_ERROR:
				iTE_MRx_Msg(("No Error\n"));
			break;
		case RAP_UNRECOGN_ACTION_CODE:
				iTE_MRx_Msg(("ERROR: Unrecognized Action Code !!!\n"));
			break;
		case RAP_UNSUPPORT_ACTION_CODE:
				iTE_MRx_Msg(("ERROR: Unsupported Action Code !!!\n"));
			break;
		case RAP_RESPONDER_BUSY:
				iTE_MRx_Msg(("Responder Busy ...\n"));
			break;
		default  :
				iTE_MRx_Msg(("ERROR: Unknown RAP status code 0x%02X !!!\n", eRapError));
			break;
	}
}
#endif

iTE_u1 Msc_UcpHandler(iTE_u8 ucUcpCmd)
{
	iTE_MRx_Msg(("Character Code=%02X ==> ", (int)ucUcpCmd));

	if( ucUcpCmd&0x80 ){
		iTE_MRx_Msg(("ERROR: Unsupported UCP character code !!!\n"));
		return iTE_FALSE;
	}else{
		iTE_MRx_Msg(("%c", (int)ucUcpCmd));
		//To do UCP process
		return iTE_TRUE;
	}
}

#if iTE_MRx_Msg_Print
void Msc_UcpkHandler(iTE_u8 ucUcpkCode)
{
	iTE_Msg(("RX UCPK [0x%x] =%c\n", (int)ucUcpkCode, ucUcpkCode));
}
void Msc_UcpeHandler(UCP_ERROR eUcpeStatus)
{
	iTE_MRx_Msg(("RX UCPE  => "));
	switch(eUcpeStatus){
		case	UCP_NO_ERROR:
				iTE_MRx_Msg((" No Error\n"));
			break;
		case	UCP_INEFFECTIVE_KEY:
				iTE_MRx_Msg((" ERROR: Ineffective UCP Key Code !!!\n"));
			break;
		default:
				iTE_MRx_Msg(("ERROR: Unknown UCP status code 0x%02X !!!\n", (int)eUcpeStatus));
			break;
	}
}
#endif
void Msc_RcpKeyParse( iTE_u8 ucRcpKey )
{
	iTE_pu8 _CODE pucRcpString[0x80]={
		"Select", 			"Up",	 		"Down", 				"Left", 			"Right", 			"Right-Up", 			"Right-Down",		"Left-Up",
		"Left-Down", 		"Root Menu",	 	"Setup Menu", 		"Contents Menu", 	"Favorite Menu", 	"Exit",				NULL,			NULL,
		NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		"Numeric 0",		"Numeric 1",		"Numeric 2",			"Numeric 3",		"Numeric 4",		"Numeric 5",			"Numeric 6",		"Numeric 7",
		"Numeric 8",		"Numeric 9",		"Dot",				"Enter",			"Clear",			NULL,				NULL,			NULL,
		"Channel Up",		"Channel Down",	"Previous Channel",	"Sound Select",	"Input Select", 	"Show Information",	"Help",			"Page Up",
		"Page Down",		NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		NULL,			"Volume Up",		"Volume Down",		"Mute",			"Play",			"Stop",				"Pause",			"Record",
		"Rewind",			"Fast Forward",	"Eject",				"Forward",		"Backward",		NULL,				NULL,			NULL,
		"Angle",			"Subpicture",		NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		"Play Function",	"Pause Play Function",	"Record Function",	"Pause Record Function",	"Stop",	"Mute",				"Restore Volume Funation",	"Tune Function",
		"Select Media Function", NULL,		NULL,				NULL,			NULL,			NULL,				NULL,			NULL,
		NULL,			"F1 (Blue)",		"F2 (Red)",			"F3 (Green)",		"F4 (Yellow)",		"F5",					NULL,			NULL,
		NULL,			NULL,			NULL,				NULL,			NULL,			NULL,				"Vendor Specific",	NULL
	};

	if(pucRcpString[ucRcpKey&0x7f] != NULL){
     		iTE_MRx_Msg(("KeyCode=0x%02X ==> [%s]\n ", (int)ucRcpKey, pucRcpString[ucRcpKey&0x7f]));
	}else{
		iTE_Msg(("ERROR: Reserved RCP sub-command code [0x%x]!!!\n", (int)ucRcpKey));
		return;
	}

	if(ucRcpKey & 0x80){
		iTE_MRx_Msg((" Key Release\n"));
//		CECUICommandCode(ucRcpKey & 0x7F);
	}else{
		iTE_MRx_Msg((" Key Press\n"));
	}
}

iTE_u1 Msc_WriteBurstDataFill(iTE_u8 ucOffset, iTE_u8 ucByteNo, iTE_u8 *pucData)
{
	iTE_u8	ucTemp;

	stTxWbInfo.pucTxScrpad = pucData;
	stTxWbInfo.ucCnt = ucByteNo;
	stTxWbInfo.ucOffset = ucOffset;

	if(ucByteNo<=2 || (ucOffset+ucByteNo)>16){
		iTE_Msg(("ERROR: Set Burst Write Data Fail\n"));
		return iTE_FALSE;
	}else{
		mhl_rx_bw(0x5E, 2, pucData);
		mhl_rx_w(0x54, 0x40+ucOffset);
		if(MSCBurstWrID)
			ucTemp = 2;
		else
			ucTemp = 0;

		iTE_MRx_Msg(("Msc_WriteBurstDataFill ="));
		for(; ucTemp < ucByteNo;){
			iTE_MRx_Msg((" 0x%x\n ", (int)pucData[ucTemp]));
			mhl_rx_w(0x59, pucData[ucTemp++]);
		}
		iTE_MRx_Msg(("\n"));
		return iTE_TRUE;
	}
}

MHL3D_STATE MSC_3DInforSend(iTE_u1 b3dDtd)
{
	iTE_u8	ucTemp, uc3DInforLen;
	iTE_u8	ucWBData[16];
	iTE_u8	uc3DTempCnt;
	MHL3D_STATE eRet3dState;

	uc3DTempCnt = st3DParse.uc3DTempCnt & 0x7F;

	if(b3dDtd){
		ucWBData[0] = MSC_3D_DTD >> 8;
		ucWBData[1] = MSC_3D_DTD & 0xff;
		ucWBData[3] = st3DParse.ucDtdCnt;
	}else{
		ucWBData[0] = MSC_3D_VIC >> 8;
		ucWBData[1] = MSC_3D_VIC & 0xff;
		ucWBData[3] = st3DParse.ucVicCnt;
	}
	ucWBData[2] = 0;
	ucWBData[4] = (uc3DTempCnt / 5) + 1;

	ucWBData[5] = ucWBData[3] - uc3DTempCnt;
	if(ucWBData[5] > 5){
		ucWBData[5] = 5;
		eRet3dState = MHL3D_REQ_WRT;
		st3DParse.uc3DTempCnt += 5;
		iTE_Msg(("*** MSC_3DInforSend MHL3D_REQ_WRT ***\n"));
	}else{
		if(b3dDtd){
			st3DParse.uc3DTempCnt = 0;
			eRet3dState = MHL3D_REQ_WRT;
			iTE_Msg(("*** MSC_3DInforSend DTD Done ***\n"));
		}else{
			st3DParse.uc3DTempCnt = 0x80;
			eRet3dState = MHL3D_REQ_DONE;
			iTE_Msg(("*** MSC_3DInforSend VIC Done ***\n"));
		}
	}
	uc3DInforLen = 6 + (ucWBData[5] * 2);
	for(ucTemp = 6; ucTemp < uc3DInforLen; ){
		ucWBData[ucTemp++] = 0x00;
		if(b3dDtd){
			ucWBData[ucTemp++] = uc3DDtd[uc3DTempCnt++];
		}else{
			ucWBData[ucTemp++] = st3DParse.uc3DInfor[uc3DTempCnt++];
		}
	}
	do{
		if(--ucTemp != 2){
			ucWBData[2] -= ucWBData[ucTemp];
		}
	}while(ucTemp);


	if(iTE_TRUE == Msc_WriteBurstDataFill(0x00, uc3DInforLen, ucWBData)){
		TxMscCmd.ucOffset = 0x40;
		if(Msc_Fire(MSC_PKT_WRITE_BURST, &TxMscCmd)==MSC_SUCCESS){
			TxMscCmd.ucOffset = 0x20;
			TxMscCmd.ucValue = MHL_INT_DSCR_CHG;
			Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
		}
	}
	iTE_Msg(("*** MSC_3DInforSend eRet3dState = 0x%x ***\n", (int)eRet3dState));
	return eRet3dState;
}

void Msc_3DProcess(MHL3D_STATE *e3DReqState)
{
	iTE_u8	ucTemp;

	iTE_Msg(("***Msc_3DProcess***\n"));
	switch(*e3DReqState){
		case	MHL3D_REQ_DONE:
				iTE_Msg(("***Msc_3DProcess*** MHL3D_REQ_DONE\n"));
			break;
		case	MHL3D_REQ_START:
				iTE_Msg(("***Msc_3DProcess*** MHL3D_REQ_START\n"));
				if(st3DParse.uc3DInfor == 0){
#if USING_DEFAULT_3D_VIC
					ucTemp = sizeof(uc3DVic);
					if((ucTemp==1) && (uc3DVic[0] == 0))
						ucTemp = 0;
					st3DParse.ucVicCnt = ucTemp;
					st3DParse.uc3DInfor = uc3DVic;
#else
	#if USING_CALLOC
					st3DParse.uc3DInfor = (iTE_u8*)calloc(st3DParse.ucVicCnt , sizeof(iTE_u8));
	#else
					st3DParse.uc3DInfor = uc3DInfor;
	#endif
					ucTemp = st3DParse.ucVicCnt;
					do{
						st3DParse.uc3DInfor[--ucTemp] = 0;
					}while(ucTemp);

					edid_Parse3DVic(&st3DParse);
#endif
					ucTemp = sizeof(uc3DDtd);
					if((ucTemp == 1) && (uc3DDtd[0]==0))
						ucTemp = 0;
					st3DParse.ucDtdCnt = ucTemp;
					st3DParse.uc3DTempCnt = 0x80;
				}

				if(st3DParse.uc3DInfor){
					TxMscCmd.ucOffset = 0x20;
					TxMscCmd.ucValue = MHL_INT_REQ_WRT;
					Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_WRT;

#if USING_CALLOC
				}else{
					iTE_Msg(("*************ERROR: st3DParse.uc3DInfor mem alloc fail  ***************\n" ));
					Msc_Fire(MSC_PKT_ABORT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_DONE;
					st3DParse.uc3DTempCnt = 0x80;
#endif
				}
			break;
		case	MHL3D_REQ_WRT:
					iTE_Msg(("***Msc_3DProcess*** MHL3D_REQ_WRT\n"));
					*e3DReqState = MHL3D_GNT_WRT;
			break;
		case	MHL3D_GNT_WRT:
				iTE_Msg(("***Msc_3DProcess*** MHL3D_GNT_WRT\n"));
				if(st3DParse.uc3DTempCnt & 0x80){
					*e3DReqState = MSC_3DInforSend(iTE_TRUE);
					iTE_Msg(("***Msc_3DProcess*** MSC_3DInforSend(iTE_TRUE)\n"));
				}else{
					*e3DReqState = MSC_3DInforSend(iTE_FALSE);
					iTE_Msg(("***Msc_3DProcess*** MSC_3DInforSend(iTE_FALSE)\n"));
				}
					if(*e3DReqState == MHL3D_REQ_DONE){
#if USING_CALLOC
					free(st3DParse.uc3DInfor);
#endif
						st3DParse.uc3DInfor = 0x00;
						st3DParse.uc3DTempCnt = 0x80;
					iTE_Msg(("***Msc_3DProcess*** MHL3D_REQ_DONE\n"));
				}else{
					TxMscCmd.ucOffset = 0x20;
					TxMscCmd.ucValue = MHL_INT_REQ_WRT;
					Msc_Fire(MSC_PKT_SET_INT, &TxMscCmd);
					*e3DReqState = MHL3D_REQ_WRT;
					iTE_Msg(("***Msc_3DProcess*** MHL3D_REQ_WRT\n"));
				}
			break;
	default:
		break;

	}

}

#endif
