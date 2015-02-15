///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <HDMI_Switch.c>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#define _HDMI_SW_INIT_TAB_
#include "HDMI_Switch.h"
#include "EDID.h"
#include "HDMI_Tx.h"
#include "../IT66332_Customer/IT66332.h"

#if USING_MHL
#include "MHL_Rx.h"
#endif
//****************************************************************************
iTE_u1 bPackPixFlag = iTE_FALSE;
iTE_u1 bCDSenFlag = iTE_FALSE;
iTE_u1 bTxHPDFlag;
iTE_u1 bEDIDNotWrite = iTE_TRUE;
iTE_u32 fWClk, fVClk, fHClk;
iTE_u8     AutoEQFailCnt = 0;
#if USING_MHL
extern iTE_u16 WakeFailCnt;
#endif
extern iTE_u8 ucFwFlag;
//****************************************************************************
void hdmi_sw_init(void)
{
	iTE_u8 ucTemp;

	iTE_SW_Msg((" hdmi_sw_initS \n"));

	for(ucTemp=0; ucTemp < HDMI_SW_INIT_TAB_SIZE; ucTemp++){
		hdmi_sw_set(HDMI_SW_INIT_TAB[ucTemp][0],HDMI_SW_INIT_TAB[ucTemp][1], HDMI_SW_INIT_TAB[ucTemp][2]);
	}
	iTE_SW_Msg((" hdmi_sw_initE \n"));

	bCDSenFlag = iTE_FALSE;
	bPackPixFlag = iTE_FALSE;
}
//****************************************************************************
void hdmi_sw_irq(void)
{
	iTE_u8 ucSwReg18[4];
	iTE_u8 ucRdData;

	iTE_SW_Msg((" hdmi_sw_irq_S\n"));

	hdmi_sw_br(0x18, 4, ucSwReg18);
	hdmi_sw_bw(0x18, 4, ucSwReg18);

	iTE_SW_Msg(("hdmi_sw_irq status : 0x%x, 0x%x, 0x%x, 0x%x\n", (int)ucSwReg18[0], (int)ucSwReg18[1], (int)ucSwReg18[2], (int)ucSwReg18[3]));


	if(ucSwReg18[0] &0x01 ) {		iTE_SW_Msg(("MxPwr5VChg Interrupt => Current MxPwr5V = %d\n", (int)(hdmi_sw_r(0x22)&0x01)));		}
	if(ucSwReg18[0] &0x02 ) {		iTE_SW_Msg(("MxClkRdyChg Interrupt => Current MxClkRdy = %d\n", (int)(hdmi_sw_r(0x22)&0x02)>>1));	}

     	if(ucSwReg18[0] &0x04 ) {
        	iTE_SW_Msg(("MxClkStbChg Interrupt => Current MxClkStb = %d, Current MainPort = %d\n", (int)(hdmi_sw_r(0x22)&0x04)>>2, (int)ucHDMI_Port));

        	if( hdmi_sw_r(0x22)&0x04 ) {
#if WCLK_PRINT
             		ucRdData = hdmi_sw_r(0x23);

	             	if( ucRdData&0x04 )
                 		fHClk = (fRefClk*256)*2;
             		else if( ucRdData&0x08 )
                 		fHClk = fRefClk*256*4;
             		else
                 		fHClk = fRefClk*256;

            		iTE_SW_Msg(("HCLK Average Ready=%d, PreDiv2=%d, PreDiv4=%d,", (int)(ucRdData&0x01), (int)(ucRdData&0x04)>>2, (int)(ucRdData&0x08)>>3));

			ucRdData = hdmi_sw_r(0x20);
             		bCDSenFlag = (ucRdData&0x04)>>2;
             		bPackPixFlag = (ucRdData&0x08)>>3;
             		iTE_SW_Msg(("bCDSenFlag=%d, bPackPixFlag=%d\n", (int)bCDSenFlag, (int)bPackPixFlag));

              	ucRdData = hdmi_sw_r(0x24);
             		fHClk /= ucRdData;
             		if( (ucHDMI_Port != ucMHL_Port) || bCDSenFlag==iTE_FALSE )
                 		fVClk = fHClk;
             		else if( bPackPixFlag==iTE_FALSE )
                 		fVClk = fHClk/3;
             		else
                 		fVClk = fHClk/2;

             		iTE_SW_Msg(("HCLK Average Value=0x%02X, MxHCLK rough frequency=%d.%3dMHz, MxVCLK=%d.%3dMHz\n", (int)ucRdData, (int)(fHClk/1000), (int)(fHClk%1000), (int)(fVClk/1000), (int)(fVClk%1000)));

             		cal_wclk(ucHDMI_Port);

             		iTE_SW_Msg(("Detail Count MxWCLK=%d.%3dMHz, MxHCLK=%d.%3dMHz, MxVCLK=%d,%3dMHz\n\n", (int)(fWClk/1000), (int)(fWClk%1000),  (int)(fHClk/1000), (int)(fHClk%1000),(int)(fVClk/1000), (int)(fVClk%1000)));
#endif
#if 1
//start @20130611 added by ming for manual EQ
			if((iTE_FALSE == EnAutoEQ) && (iTE_FALSE == EnFixEQ)){		//V1.04
				if(ucFwFlag!=0xC0){	//V1.07_6
					if(fHClk <= 150000){
						hdmi_sw_set(0x73, 0x3f, 0x3f);
					}else{
						hdmi_sw_set(0x73, 0x3f, 0x00);
					}
				}
				hdmi_sw_set(0x07, 0x40, 0x40);
				hdmi_sw_set(0x07, 0x40, 0x00);
				hdmi_sw_set(0x70, 0x0F, 0x0E);
				hdmi_sw_set(0x70, 0x0F, 0x0F);
			}
//end
#endif
             		iTE_Sleep_ms(1000);
         	}
    	}

	if(ucSwReg18[0] &0x08 ) {		iTE_SW_Msg(("MxClkHighChg Interrupt => Current MxClkHigh = %d\n", (int)(hdmi_sw_r(0x22)&0x08)>>3));		}
	if(ucSwReg18[0] & 0x10 ) {		iTE_SW_Msg(("MxClkVeryHChg Interrupt => Current MxClkVeryH = %d\n", (int)(hdmi_sw_r(0x22)&0x10)>>4));	}
	if(ucSwReg18[0] & 0x20 ) {		iTE_SW_Msg(("MxClkValidChg Interrupt => Current MxClkValid = %d\n", (int)(hdmi_sw_r(0x22)&0x20)>>5));	}
	if(ucSwReg18[0] & 0x40 ) {
		iTE_SW_Msg(("TxSenChg Interrupt => Current TxSen = %d\n", (int)hdmi_sw_r(0x20)&0x01));
#if 0//USING_MHL
		ucRdData = hdmi_sw_r(0x20);
		if((ucRdData & 0x5) == 4){
			if(ucMHL_Port == ucHDMI_Port)
				mhl_CBusReDiscovery();
		}
#endif
	}

	if(ucSwReg18[0] &0x80 ) {
         bTxHPDFlag = (hdmi_sw_r(0x20)&0x02)>>1;
         iTE_SW_Msg(("TxHPDChg Interrupt => Current TxHPD = %d\n", (int)bTxHPDFlag));
/*
		if( Dongle==iTE_TRUE && DongleEDID==EXT){
			hdmi_HpdLowAll();
			iTE_Sleep_ms(HPD_RETRIGER_TIME);
			hdmi_HpdAutoAll();
		}
*/
		if( Dongle==iTE_TRUE && DongleEDID==INT ) {
			if( iTE_TRUE ==  bTxHPDFlag ) {
//				hdmi_sw_set(0x17, 0x22, 0x22);   // Force TxPwr5V output
				hdmi_sw_EdidGet(iTE_TRUE);
//				hdmi_sw_set(0x17, 0x22, 0x00);   // Disable TxPwr5V manual output
				hdmi_sw_set(0x10, 0x40, 0x00);
				hdmi_sw_set(0x11, 0x80, 0x80);   // FW Initial Done
			}else{
				hdmi_sw_set(0x11, 0x80, 0x00);   // FW Initial Wait
				hdmi_sw_set(0x10, 0x40, 0x40);
				}
		}
     }

	if(ucSwReg18[1]){
			ucRdData = hdmi_sw_r(0x21);
	if(ucSwReg18[1] & 0x01 ) {
			iTE_SW_Msg(("P0Pwr5VChg Interrupt => Current P0Pwr5V = %d\n", (int)(ucRdData&0x01)));
#if 0
			if((ucHDMI_Port==HDMI_A)&&(ucRdData & 0x01)){
			hdmi_sw_set(0x11, 0x03, 0x03);
			hdmi_sw_set(0x11, 0x03, 0x00);
		}
#endif
	}

	if(ucSwReg18[1] & 0x02 ) {
			iTE_SW_Msg(("P1Pwr5VChg Interrupt => Current P1Pwr5V = %d\n", (int)(ucRdData&0x02)));
#if 0
			if((ucHDMI_Port==HDMI_B)&&(ucRdData & 0x02)){
			hdmi_sw_set(0x11, 0x03, 0x03);
			hdmi_sw_set(0x11, 0x03, 0x01);
		}
#endif
	}
	if(ucSwReg18[1] & 0x04 ) {
			iTE_SW_Msg(("P2Pwr5VChg Interrupt => Current P2Pwr5V = %d\n", (int)(ucRdData&0x04)));
#if 0
			if((ucHDMI_Port==HDMI_C)&&(ucRdData & 0x04)){
			hdmi_sw_set(0x11, 0x03, 0x03);
			hdmi_sw_set(0x11, 0x03, 0x02);
		}
#endif
	}

     	if(ucSwReg18[1] & 0x10 ) {
	         	iTE_SW_Msg(("P0ClkRdyChg Interrupt => Current P0ClkRdy = %d\n", (int)(ucRdData&0x10)));
#if WCLK_PRINT
	         	if( ucRdData&0x10 ) {
             		cal_wclk(HDMI_A);
         	}
#endif
     	}

     	if(ucSwReg18[1] & 0x20 ) {
	         	iTE_SW_Msg(("P1ClkRdyChg Interrupt => Current P1ClkRdy = %d\n", (int)(ucRdData&0x20)));
#if WCLK_PRINT
	         	if( ucRdData&0x20 ) {
             		cal_wclk(HDMI_B);
         	}
#endif
     	}

    	 if(ucSwReg18[1] & 0x40 ) {
	        	iTE_SW_Msg(("P2ClkRdyChg Interrupt => Current P2ClkRdy = %d\n", (int)(ucRdData&0x40)));
#if WCLK_PRINT
	         	if( ucRdData&0x40 ) {
             		cal_wclk(HDMI_C);
         	}
#endif
     	}
	}
     	if(ucSwReg18[2] & 0x01 ) {	iTE_SW_Msg(("AutoEQUpd Interrupt !!!\n"));		 	}
     	if(ucSwReg18[2] & 0x02 ) {	iTE_SW_Msg(("DDC ReGen TimeOut Interrupt !!!\n"));    	}
 	if(ucSwReg18[2] & 0x04 ) {	iTE_SW_Msg(("Firmware Timer Trigger Interrupt !!!\n")); 	}
     	if(ucSwReg18[2] & 0x08 ) {	iTE_SW_Msg(("DDC Master Bus Hang Interrupt !!!\n"));   	}

     if(ucSwReg18[2] & 0x10 ) {
         bCDSenFlag = (hdmi_sw_r(0x20)&0x04)>>2;
         iTE_SW_Msg(("MHL CDSen Change Interrupt => Current MHL CDSen = %d\n", (int)bCDSenFlag));
		if(CBusDrvHOpt && bCDSenFlag){	//V1.09_
				//	iTE_Sleep_ms(200);

			hdmi_sw_set(0x35, 0x88, 0x80);
			switch(ucMHL_Port){
				case	MHL_A:
				case	MHL_B:
				case	MHL_C:
					hdmi_sw_set(0x13+ucMHL_Port, 0x88, 0x88);
					iTE_Sleep_ms(200);
					hdmi_sw_set(0x13+ucMHL_Port, 0x88, 0x00);
				default:
					break;
			}
			hdmi_sw_set(0x35, 0x88, 0x00);
			hdmi_sw_w(0x1a, 0x10);
		}
		if(ucFwFlag != 0xC0){
//start added by ming @20130607, sync from keming
			if((ucHDMI_Port == ucMHL_Port)){
				if(bCDSenFlag){
//start @20130618 added for reset MHL if MHL removed
					hdmi_sw_set(0x07, 0x02, 0x00);
					hdmi_sw_w(0x08, 0x00);
//end @20130618
					if(EnPwrDnMHL == iTE_FALSE){
						mhl_EnterPwrDn();
//						iTE_Sleep_ms(HPD_RETRIGER_TIME);
					}
					mhl_ExitPwrDn();
					hdmi_sw_set(0x61, 0x70, 0x00);
//start @20130618 added for release reset MHL if MHL insert
				}else{
					hdmi_sw_set(0x61, 0x70, 0x30);
					hdmi_sw_set(0x07, 0x02, 0x02);
					hdmi_sw_w(0x08, 0xff);
//end @20130618
				}
			}else{
				hdmi_sw_set(0x61, 0x70, 0x30);
				hdmi_sw_set(0x07, 0x02, 0x02);
				hdmi_sw_w(0x08, 0xff);
			}
// end added
		}
#if USING_MHL
		if(bCDSenFlag == iTE_FALSE){
			WakeFailCnt = 0;
		}
#endif
     }

     if(ucSwReg18[2] & 0x20 ) {
         bPackPixFlag = (hdmi_sw_r(0x20)&0x08)>>3;
		iTE_SW_Msg(("PackedPixel Mode Change Interrupt => Current PackPix = %d\n", (int)bPackPixFlag));
	}

	if(ucSwReg18[2] & 0x40 ) {	iTE_SW_Msg(("MxClkDiff Interrupt !!!\n WClk Average Value=0x%02X\n\n", (int)hdmi_sw_r(0x24)));			}
	if(ucSwReg18[3] & 0x01 ) {
		ucRdData = hdmi_sw_r(0x2b)&0x01;
		iTE_SW_Msg(("M2H Det3D Change Interrupt => Current Det3D = %d\n", (int)ucRdData));
		if(EnHDCPBypass == iTE_FALSE  &&  ucFwFlag==0xB0){			//V1.06_3
			if(ucRdData){
				hdmi_tx_set(0xe5, 0xb0, 0x20);	// select HDMITXSRC="20" manually
			}else{
				hdmi_tx_set(0xe5, 0xb0, 0x10);	// select HDMITXSRC="10" manually
			}
		}
     }

     if(ucSwReg18[3] & 0x02 ) {
		ucRdData = hdmi_sw_r(0x2b) & 0x02;
		iTE_SW_Msg(("M2H VidStb Change Interrupt => Current VidStb = %d\n", (int)(ucRdData)));
		if((iTE_FALSE == EnFixEQ) && ( ucRdData == 0x00 )) {
			if( EnAutoEQ )
				hdmi_sw_set(0x77, 0xF0,(EnEQRtyTog<<7)+ (EnEQFailRty<<6)+0x30);		//V1.07_7
			else
				hdmi_sw_set(0x77, 0xF0,(EnEQRtyTog<<7)+ (EnEQFailRty<<6)+0x00);		//V1.07_7
		}
	}

	if(ucSwReg18[3] & 0x04 ) {	iTE_SW_Msg(("M2H HDCP Change Interrupt => Current HDCP = %d\n", (int)(hdmi_sw_r(0x2b)&0x04)>>2));		}

     if(ucSwReg18[3] & 0x08 ) {
		ucRdData = (hdmi_sw_r(0x2b) & 0x08) ;//>> 3;
		iTE_SW_Msg(("M2H Enable CBUS Change Interrupt => Current EnCBus = %d\n", (int)(ucRdData>>3)));
		if((ucRdData == 0) &&  (ucFwFlag!=0xC0)){		//V1.07_2
			//FW work-around for MHL Link Layer Reset
			hdmi_sw_set(0x07, 0x02, 0x02);
			hdmi_sw_set(0x07, 0x02, 0x00);
		}
     }

     if(ucSwReg18[3] & 0x10 ) {
         AutoEQFailCnt = 0;
//         Reg1b &= 0xDF;
#if iTE_SW_Msg_Print
		iTE_SW_Msg(("************************************M2H AutoEQ Done Interrupt ...\n"));
		ucRdData = hdmi_sw_r(0x79);
		iTE_SW_Msg(("Current AMP1Valid=%d, AMP2Valid=%d, DFE=0x%01X\n", (int)((ucRdData&0x10)>>4), (int)((ucRdData&0x20)>>5), (int)(ucRdData&0x0F)));
		iTE_SW_Msg(("For AMP1: AmP16AVR=0x%02X, AMPCnt=%d, MatchCnt=%d\n", (int)hdmi_sw_r(0x7D), (int)hdmi_sw_r(0x7B)&0x7F, (int)hdmi_sw_r(0x7C)));
		hdmi_sw_set(0x72, 0x80, 0x80);
		iTE_SW_Msg(("For AMP2: AMP16AVR=0x%02X, AMPCnt=%d, MatchCnt=%d\n", (int)hdmi_sw_r(0x7D), (int)hdmi_sw_r(0x7B)&0x7F, (int)hdmi_sw_r(0x7C) ));
		hdmi_sw_set(0x72, 0x80, 0x00);
		ucRdData = hdmi_sw_r(0x78);
		iTE_SW_Msg(("Current ForceCS=%d, Rec_CS=0x%01X, Rec_RS=0x%02X\n", (int)(ucRdData&0x80)>>7,(int)(hdmi_sw_r(0x79)&0xC0)>>6, ucRdData&0x7F));
#endif
	}else if(ucSwReg18[3] & 0x20 ) {
#if iTE_SW_Msg_Print
		iTE_SW_Msg(("********************************AutoEQ Fail Interrupt %d !!!\n", (int)AutoEQFailCnt));
		ucRdData = hdmi_sw_r(0x79);
		iTE_SW_Msg(("Current AMP1Valid=%d, AMP2Valid=%d, DFE=0x%01X\n", (int)((ucRdData&0x10)>>4), (int)((ucRdData&0x20)>>5), (int)(ucRdData&0x0F)));
		iTE_SW_Msg(("For AMP1: AmP16AVR=0x%02X, AMPCnt=%d, MatchCnt=%d\n", (int)hdmi_sw_r(0x7D), (int)hdmi_sw_r(0x7B)&0x7F, (int)hdmi_sw_r(0x7C)));
		hdmi_sw_set(0x72, 0x80, 0x80);
		iTE_SW_Msg(("For AMP2: AMP16AVR=0x%02X, AMPCnt=%d, MatchCnt=%d\n", (int)hdmi_sw_r(0x7D), (int)hdmi_sw_r(0x7B)&0x7F, (int)hdmi_sw_r(0x7C) ));
		hdmi_sw_set(0x72, 0x80, 0x00);
		ucRdData = hdmi_sw_r(0x78);
		iTE_SW_Msg(("Current ForceCS=%d, Rec_CS=0x%01X, Rec_RS=0x%02X\n", (int)(ucRdData&0x80)>>7,(int)(hdmi_sw_r(0x79)&0xC0)>>6, ucRdData&0x7F));
#endif

#if 1
//start @20130611 added by ming
			if((iTE_FALSE == EnAutoEQ) && (iTE_FALSE == EnFixEQ) && (EnEQFailRty==iTE_FALSE)){		//V1.07_7
				hdmi_sw_set(0x07, 0x40, 0x40);
				hdmi_sw_set(0x07, 0x40, 0x00);
				hdmi_sw_set(0x70, 0x01, 0x01);
				hdmi_sw_set(0x70, 0x01, 0x00);
			}
//end
#endif
#if 0
		if( (++AutoEQFailCnt%10)==0 ) {
			hdmi_sw_set(0x62, 0x02, 0x00);
			if(hdmi_sw_r(0x64)&0x80)
				hdmi_sw_set(0x64, 0x80, 0x00);
			else
				hdmi_sw_set(0x64, 0x80, 0x80);
			hdmi_sw_set(0x62, 0x02, 0x02);
			iTE_SW_Msg(("Toggle IPLL_POL to try AutoEQ again ...\n"));
		}

		if( AutoEQFailCnt==EQRTYMAX ) {
             hdmi_sw_set(0x77, 0x70, 0x30);
             AutoEQFailCnt = 0;
             iTE_SW_Msg(("Disable AutoEQ Retry !!! => Use Default EQ Setting \n"));
     }
#else
		if( ++AutoEQFailCnt > EQRTYMAX ) {
	             hdmi_sw_set(0x77, 0x70, 0x30);
	             AutoEQFailCnt = 0;
	             iTE_SW_Msg(("Disable AutoEQ Retry !!! => Use Default EQ Setting \n"));
		}else if(((AutoEQFailCnt%10)==0 ) && (EnEQRtyTog==iTE_FALSE)){		//V1.07_7
			hdmi_sw_set(0x62, 0x02, 0x00);

			if(hdmi_sw_r(0x64)&0x80)
				hdmi_sw_set(0x64, 0x80, 0x00);
			else
				hdmi_sw_set(0x64, 0x80, 0x80);

			hdmi_sw_set(0x62, 0x02, 0x02);
			iTE_SW_Msg(("Toggle IPLL_POL to try AutoEQ again ...\n"));

		}
#endif

	}

	iTE_SW_Msg((" hdmi_sw_irq_E\n"));
}
//****************************************************************************
#if WCLK_PRINT
void cal_wclk(iTE_u8 ucPort )
{
#if WCLK_PRINT
	 iTE_u16	usRdData,usTemp;
	 iTE_u8	ucPreDivSel, ucTemp;
iTE_SW_Msg(("cal_wclk\n"));
     hdmi_sw_set(0x90, 0x03, ucPort&0x03);
     // WCLK Count Pre-Test
	hdmi_sw_w(0x90, ucPort | 0x80);
//	P2_4 = ~P2_4;
	usTemp = iTE_BusyWait_ms(5);
	hdmi_sw_w(0x90, ucPort);
//	P2_4 = ~P2_4;

     usRdData = hdmi_sw_r(0x92);
     usRdData += ((hdmi_sw_r(0x91)&0x0F)<<8);
		iTE_SW_Msg(("WCLK_0=0x%x, %x\n",(int)(hdmi_sw_r(0x91)&0x0F), (int)hdmi_sw_r(0x92)));
/*
     if(RCLKFreqSel)
     {
         rddata *=2;
     }
*/
#if 0
     if( usRdData<16 )
         ucPreDivSel = 7;
     else if( usRdData<32 )
         ucPreDivSel = 6;
     else if( usRdData<64 )
         ucPreDivSel = 5;
     else if( usRdData<128 )
         ucPreDivSel = 4;
     else if( usRdData<256 )
         ucPreDivSel = 3;
     else if( usRdData<512 )
         ucPreDivSel = 2;
     else if( usRdData<1024 )
         ucPreDivSel = 1;
     else
         ucPreDivSel = 0;
#else
	ucPreDivSel = 0;
	while(usRdData>>(ucPreDivSel+4)){
		ucPreDivSel++;
	};
	if(ucPreDivSel > 7)
		ucPreDivSel = 0;
	else
		ucPreDivSel = 7 - ucPreDivSel;
#endif

//   printf("predivsel=%d\n", predivsel);

     usRdData = 0;
	ucTemp = ucPort + (ucPreDivSel << 4);
	hdmi_sw_w(0x90, 0x80+ucTemp);
	usTemp = iTE_BusyWait_ms(5);	//2
	hdmi_sw_w(0x90, ucTemp);

         usRdData += (iTE_u16)hdmi_sw_r(0x92);
         usRdData += (iTE_u16)(hdmi_sw_r(0x91)&0x0F)<<8;
		iTE_SW_Msg(("WCLK_0=0x%x, %x\n",(int)(hdmi_sw_r(0x91)&0x0F), (int)hdmi_sw_r(0x92)));

//     sum /= (1*pow(2,predivsel));

     usRdData /= (1 << ucPreDivSel);
	iTE_SW_Msg((" sum = %d \n", (int)usRdData));
     fWClk = fRefClk*2048/usRdData;
#endif
#if USING_MHL
     bCDSenFlag = (hdmi_sw_r(0x20)&0x04)>>2;
     bPackPixFlag = (hdmi_sw_r(0x20)&0x08)>>3;
#if WCLK_PRINT
     	if( ucPort!=ucMHL_Port || bCDSenFlag==iTE_FALSE ){
         fVClk = fWClk;
		fHClk = fWClk;
     	}else if( bPackPixFlag==iTE_TRUE ){
         fVClk = fWClk*2;
		fHClk = fWClk*4;
     	}else{
     		fVClk = fWClk;
		fHClk = fWClk*3;
     	}
	iTE_SW_Msg(("Detail Count P%dWCLK=%d.%3dMHz\n\n", (int)ucPort,(int)(fWClk/1000), (int)(fWClk%1000)));
#endif
#endif
}
#endif
//****************************************************************************
iTE_u1 ddcwait(void)
{
	iTE_u16	usTemp = DDCWAITNUM;

	do{
//		usTemp--;
		iTE_Sleep_ms(DDCWAITTIME);
	}while(((hdmi_sw_r(0x5c) & 0x80) == 0x00) && (--usTemp) );

	if(usTemp){
		return iTE_TRUE;
	}else{
		iTE_SW_Msg(("DDC Bus Status = %02X\n", hdmi_sw_r(0x5c)));
		iTE_SW_Msg(("ERROR: DDC Bus Wait TimeOut => "));
#if 0
		if( hdmi_sw_r(0x1a)&0x08 ) {
			 iTE_SW_Msg(("DDC Bus Hang !!!\n"));
			 iTE_SW_Msg(("Current DDC Status Reg[5C]=0x%02X\n", hdmi_sw_r(0x5c)));
			 hdmi_sw_w(0x5a, 0x0F);  // Abort DDC Command
			 hdmi_sw_w(0x1a, 0x08);  // Clear Interrupt
		}
		else
			 iTE_SW_Msg(("Unknown Issue !!!\n"));

#else
			 iTE_SW_Msg(("Current DDC Status Reg[5C]=0x%02X\n", hdmi_sw_r(0x5c)));
			 hdmi_sw_w(0x5a, 0x0F);  // Abort DDC Command
			 hdmi_sw_w(0x5a, 0x0A);	//Gen DDC Clk
			 hdmi_sw_w(0x1a, 0x08);  // Clear Interrupt
#endif
			hdmi_sw_set(0x07, 0x20, 0x20);
			hdmi_sw_set(0x07, 0x20, 0x00);

		iTE_SW_Msg(("\n\n"));

		return iTE_FALSE;  // will try again
	}

}
//****************************************************************************
void hdmi_sw_EdidGet(iTE_u1 bChkTxHPD)
{
//extern MHL3D_STATE 	 e3DReqState;
//	hdmi_HpdLowAll();
	edid_Get(bChkTxHPD);
//	hdmi_HpdAutoAll();
}
//****************************************************************************
void cbus_fwwa(void)
{
#if USING_MHL
	hdmi_sw_w(0x07, 0x02);
	hdmi_sw_w(0x08, 0xff);
    iTE_SW_Msg(("#######################################################\n"));
    iTE_SW_Msg(("##### Firmware Work Around for IUMAT IO issue !!! #####\n"));
    iTE_SW_Msg(("#######################################################\n"));
	hdmi_sw_w(0x35, 0x80);
	hdmi_sw_w(0x35, 0x00);
	hdmi_sw_w(0x07, 0x00);
	hdmi_sw_w(0x08, 0x00);
#endif
}
void mhl_EnterPwrDn(void)
{
//	mhl_rx_set(0x28, 0x02, 0x02);
	hdmi_sw_set(0x07, 0x02, 0x02);
	hdmi_sw_w(0x08, 0xff);
	iTE_Msg(("MHL enter power down\n"));
}
void mhl_ExitPwrDn(void)
{
	if(EnPwrDnMHL == iTE_FALSE){
		iTE_Sleep_ms(HPD_RETRIGER_TIME);
	}
	hdmi_sw_set(0x07, 0x02, 0x00);
	hdmi_sw_w(0x08, 0x00);
//	mhl_rx_set(0x28, 0x02, 0x00);
	iTE_Msg(("MHL exit power down\n"));
}
//****************************************************************************
void cal_oclk(void)
{
	#define CAL_CNT (1)

	iTE_u32  OSC_CLK;
	iTE_u8	osc_div, ucTemp;
	iTE_u16	usTemp;
	iTE_u8	ucRdData[3];

	iTE_SW_Msg(("cal_oclk \n"));
	usTemp = 0;
	OSC_CLK = 0;
	osc_div = hdmi_sw_r(0x12);
#if 0
	for(ucTemp=0; ucTemp<CAL_CNT; ucTemp++){
		hdmi_sw_w(0x12, 0xc8);
//		P2_4 = ~P2_4;
		usTemp += iTE_BusyWait_ms(100);		//100ms
		hdmi_sw_w(0x12, 0x48);
//		P2_4 = ~P2_4;

		OSC_CLK += (ULONG)hdmi_sw_r(0x2d);
		OSC_CLK += (ULONG)hdmi_sw_r(0x2e)<<8;
		OSC_CLK += (ULONG)hdmi_sw_r(0x2f)<<16;
		iTE_SW_Msg(("OSCCLK_0=0x%x, %x, %x\n",(int)hdmi_sw_r(0x2f), (int)hdmi_sw_r(0x2e), (int)hdmi_sw_r(0x2d)));

		iTE_SW_Msg(("OSCCLK_0=0x%x, %x, %x, %x, usTemp=0x%x\n",(int)(OSC_CLK >> 24)&0xff, (int)(OSC_CLK>>16)&0xff, (int)(OSC_CLK>>8)&0xff, (int)(OSC_CLK&0xff), (int)usTemp));
	}
#else
	for(ucTemp=0; ucTemp<CAL_CNT; ucTemp++){
//		hdmi_sw_w(0x12, 0xc8);
		hdmi_sw_w(0x12, osc_div | 0x80);
//		P2_4 = ~P2_4;
		usTemp += iTE_BusyWait_ms(100);		//100ms
//		hdmi_sw_w(0x12, 0x48);
		hdmi_sw_w(0x12, osc_div);
//		P2_4 = ~P2_4;

		hdmi_sw_br(0x2d, 0x3, ucRdData);

		OSC_CLK += (iTE_u32)ucRdData[0];
		OSC_CLK += (iTE_u32)ucRdData[1]<<8;
		OSC_CLK += (iTE_u32)ucRdData[2]<<16;
		iTE_SW_Msg(("OSCCLK_0=0x%x, %x, %x\n",(int)ucRdData[0], (int)ucRdData[1], (int)ucRdData[2]));

		iTE_SW_Msg(("OSCCLK_0=0x%x, %x, %x, %x, usTemp=0x%x\n",(int)(OSC_CLK >> 24)&0xff, (int)(OSC_CLK>>16)&0xff, (int)(OSC_CLK>>8)&0xff, (int)(OSC_CLK&0xff), (int)usTemp));
	}
#endif
	OSC_CLK /= usTemp;		//KHz

 	iTE_SW_Msg(("OSCCLK_0=0x%x, %x, %x, %x\n",(int)(OSC_CLK >> 24)&0xff, (int)(OSC_CLK>>16)&0xff, (int)(OSC_CLK>>8)&0xff, (int)(OSC_CLK&0xff)));
 	iTE_SW_Msg(("OSCCLK=%d.%dMHz\n", (int)(OSC_CLK/1000), (int)(OSC_CLK%1000)));

	osc_div = OSC_CLK / 10000 ;


	if(((OSC_CLK/osc_div)) > (2000*10 - OSC_CLK/(osc_div+1)))
		osc_div++;

	if(FixOCLKD4)
		osc_div = 4;

	iTE_SW_Msg(("osc_div = %d \n", (int)osc_div));

	iTE_SW_Msg(("OCLK=%d.%03dMHz\n", (int)(OSC_CLK/osc_div/1000), (int)((OSC_CLK/osc_div)%1000)));

	hdmi_sw_set(0x12, 0x70, osc_div << 4);
#if 1
	if(RCLKFreqSel>2){
		fRefClk = OSC_CLK  / 16;
	}else{
		fRefClk = OSC_CLK  / (2 * (1<<RCLKFreqSel));
	}
#else
	switch(RCLKFreqSel){
		case	0:
		case	1:
		case	2:
					fRefClk = OSC_CLK / (2 * (1<<RCLKFreqSel));
					break;
		default:
					fRefClk = OSC_CLK / 16;
					break;
	}
#endif
}
//****************************************************************************
void hdmi_HpdLowAll(void)
{
	hdmi_sw_set(0x13, 0x88, 0x80);
	hdmi_sw_set(0x14, 0x88, 0x80);
	hdmi_sw_set(0x15, 0x88, 0x80);
}
//****************************************************************************
void hdmi_HpdAutoAll(void)
{
	hdmi_sw_set(0x13, 0x88, 0x00);
	hdmi_sw_set(0x14, 0x88, 0x00);
	hdmi_sw_set(0x15, 0x88, 0x00);
}
//****************************************************************************
SYS_STATUS hdmi_sw_set(iTE_u8 Offset, iTE_u8 InvMask, iTE_u8 Data)
{
	iTE_u8 ucRdData;

	if(InvMask){
		if(~InvMask){
			  iTE_i2c_read_byte(HDMISwAddr, Offset, 1, &ucRdData, HDMI_Sw_I2C_Dev);
			  ucRdData &= (~InvMask);
			  Data |= ucRdData;
		}
		iTE_i2c_write_byte(HDMISwAddr, Offset, 1, &Data, HDMI_Sw_I2C_Dev);
	}else{
	}

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS hdmi_sw_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_write_byte(HDMISwAddr, Offset, Count, pData, HDMI_Sw_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS hdmi_sw_w(iTE_u8 Offset, iTE_u8 Data)
{
	iTE_i2c_write_byte(HDMISwAddr, Offset, 1, &Data, HDMI_Sw_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
SYS_STATUS	hdmi_sw_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData)
{
	iTE_i2c_read_byte(HDMISwAddr, Offset, Count, pData, HDMI_Sw_I2C_Dev);

	return SYS_SUCCESS;
}
//****************************************************************************
iTE_u8 hdmi_sw_r(iTE_u8 Offset)
{
	iTE_u8 ucRdData;

	iTE_i2c_read_byte(HDMISwAddr, Offset, 1, &ucRdData, HDMI_Sw_I2C_Dev);

	return ucRdData;
}

