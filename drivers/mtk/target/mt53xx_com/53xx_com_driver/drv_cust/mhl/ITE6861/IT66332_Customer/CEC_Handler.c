///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <CEC_Handler.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#include "IT66332_IO.h"
#include "IT66332.h"

#if USING_CEC
#include "CEC_Handler.h"
#include "../IT66332_Source/CEC.h"

eCEC_CMD eOldTxCECcmd=eCECNone;

//****************************************************************************
void Cec_ARCHandler(void)
{
	switch(ARCState)
	{
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
void Cec_OneTouchPlayHandler(void)
{
	switch(OTState){
		case 	sImageViewOn:			//			=0x82,	// follower:TV, switch	--> Broadcst , Directly address
				if(eOldTxCECcmd==eImageViewOn){
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
				if(eOldTxCECcmd==eActiveSource){
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
				if(eOldTxCECcmd==sTextViewOn)	{
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

void Switch_OneTouchPlayState(OneTouchPlay_StateType uState)
{
	iTE_CEC_Msg(("Switch_OneTouchPlayState--> %bX \n",uState));
	if(ucMyselfAddr!=DEVICE_ID_TV){
		OTState=uState;

		switch(uState){
			case sImageViewOn:			//			=0x04,	// follower:TV, switch	--> Broadcst
					CECCmdSet(DEVICE_ID_TV,eImageViewOn,0,0);
					break;
			case sActiveSource:			//			=0x82,	// follower:TV, switch	--> Broadcst , Directly address
					if(ucMyselfAddr!=DEVICE_ID_TV) //&& Myself_LogicAdr!=DEVICE_ID_AUDIO)
						CECCmdSet(DEVICE_ID_BROADCAST, eActiveSource,ucTxPhyAdr[0],ucTxPhyAdr[1]);
					break;
			case sTextViewOn:			//			=0x0D,	// follower:TV
					CECCmdSet(DEVICE_ID_BROADCAST,eTextViewOn,0,0);
					break;
			default:
					break;
		}
	}
}
#endif
