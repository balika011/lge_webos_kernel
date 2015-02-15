///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT66332.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _IT66332_H_
#define	_IT66332_H_

#include "x_printf.h"

///////////////////////////////////////////////////
#define iTE_FALSE	0
#define iTE_TRUE		1

#ifndef _MCU_8051_
	typedef int iTE_u1 ;
	#define _CODE
	#define _IDATA
	#define _XDATA
	typedef	unsigned char	cBYTE;
#else
	typedef bit iTE_u1 ;
	#define _CODE code
	#define _IDATA idata
	#define _XDATA xdata
	typedef	code	unsigned char	cBYTE;
#endif // _MCU_8051_


typedef enum _SYS_STATUS {
    SYS_SUCCESS = 0,
    SYS_FAIL
//    SYS_RESERVED
} SYS_STATUS;

#ifndef NULL
	#define NULL ((void *) 0)
#endif

typedef char iTE_s8, *iTE_ps8;
typedef unsigned char iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;
typedef long iTE_s32, *iTE_ps32;
typedef unsigned long iTE_u32, *iTE_pu32;
///////////////////////////////////////////////////////////



#define USING_MHL				(1)
#define USING_CEC				(0)

#define EnPwrDnMHL				(iTE_TRUE)
#define USING_DEFAULT_3D_VIC	(0)

#ifndef USING_6861
	#define USING_6861				(1)
#else
	#pragma message("predefined USING_6861")
#endif


#if USING_6861
	#define DEFAULT_HDMI		(HDMI_B)
	#define DEFAULT_MHL		(MHL_B)
#else
	#define DEFAULT_HDMI		(HDMI_A)
	#define DEFAULT_MHL		(MHL_A)
#endif

#if USING_MHL
#else
	#define DEFAULT_MHL		(MHL_REV)
#endif

//******************* Defination for Message Print **********************//
#define iTE_Msg_Print			(1)
#define iTE_SW_Msg_Print		(1)
#define iTE_EDID_Msg_Print		(1)
#define iTE_HRx_Msg_Print		(1)
#define iTE_MRx_Msg_Print		(1)
#define iTE_CEC_Msg_Print		(1)

#if iTE_Msg_Print
	#define iTE_Msg(x)			do{	Printf("[iTE]");Printf x;}while(0)
#else
	#define iTE_Msg(x)
#endif

#define WCLK_PRINT				(1)
//******************* Defination for IT66332 **************************//
#define Dongle					(iTE_FALSE)	 //iTE_TRUE for demo, iTE_FALSE for Compliance test
#define INT						(0)
#define EXT						(1)
#define DongleEDID 				(INT)   // INT/EXT
// if on-board system, set this as 1
// #define USING_DEFAULT_EDID		(iTE_FALSE) // on-board system, using default EDID
#define USING_DEFAULT_EDID		(iTE_FALSE) // extended system, use bypass edid.

#define EnAutoHPD 				(iTE_FALSE)//(iTE_TRUE)
#define EnAutoPathEn	 			(iTE_FALSE)//(iTE_TRUE)
#define EnAutoCapRdy	 		(iTE_FALSE)//(iTE_TRUE)

#define FixOCLKD4 				(iTE_FALSE)
#define RCLKFreqSel 				(1)

#define EnFixEQ					(iTE_TRUE)
#if (EnFixEQ == iTE_TRUE)			//v1.04
#define EnAutoEQ 				(iTE_FALSE)//(iTE_TRUE)
#define EnEQFailRty				(iTE_FALSE)
#define EnEQRtyTog				(iTE_FALSE)		//V1.07_7
#else
#define EnAutoEQ					(iTE_TRUE)
#define EnEQFailRty				(iTE_TRUE)
#define EnEQRtyTog				(iTE_TRUE)		//V1.07_7
#endif

#define HPD_RETRIGER_TIME		(200)
#define CBusDrvHOpt				(iTE_FALSE)		//V1.09_

#define HDMISwAddr				(0x94)
#define HDMITxAddr				(0x98)
#define HDMIRxAddr				(0x92)
#define MHLRxAddr				(0xE0)
#define EDIDAddr				(0xE4)
#define CECAddr					(0xC8)	//(0x9C)	

#define 	iTE_I2C_Dev				(0)
#define	HDMI_Sw_I2C_Dev		(iTE_I2C_Dev)
#define	HDMI_Tx_I2C_Dev		(iTE_I2C_Dev)
#define	HDMI_Rx_I2C_Dev		(iTE_I2C_Dev)
#define	MHL_Rx_I2C_Dev		(iTE_I2C_Dev)
#define	EDID_I2C_Dev			(iTE_I2C_Dev)
#define	CEC_I2C_Dev			(iTE_I2C_Dev)
//******************* Defination for HDMI Rx **************************//
#define EnHDCP1p1 				(iTE_FALSE)
#if iTE_HRx_Msg_Print
	#define iTE_HRx_Msg(x)		iTE_Msg(x)
#else
	#define iTE_HRx_Msg(x)
#endif
//******************* Defination for EDID **************************//
#define EDID_EXT_BLOCK_CNT			(1)

#define EDID_PHYSICAL_ADDR_A		(0x1000)
#define EDID_PHYSICAL_ADDR_B		(0x2000)
#define EDID_PHYSICAL_ADDR_C		(0x3000)

#define EDID_BLOCK_SIZE				(0x80)
#define EDID_READ_CNT 				(0x20)
#define EDID_READ_MIN				(0x03)
#if (EDID_READ_CNT < EDID_READ_MIN)
	#pragma message("ERROR: EDID read size must bigger than EDID_READ_MIN")
#endif

#if (iTE_TRUE == USING_DEFAULT_EDID)
#ifdef _EDID_DEFAULT_TAB_
#if 0
#define EDID_VSDB_ADDR				(0x9E)
static iTE_u8 _CODE DEFAULT_EDID_TAB[256] = {
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x4C, 0x2D, 0xAC, 0x06, 0x01, 0x00, 0x00, 0x00,
0x34, 0x13, 0x01, 0x03, 0x80, 0x66, 0x39, 0x78, 0x0A, 0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26,
0x0F, 0x50, 0x54, 0xBD, 0xEF, 0x80, 0x71, 0x4F, 0x81, 0x00, 0x81, 0x40, 0x81, 0x80, 0x95, 0x00,
0x95, 0x0F, 0xB3, 0x00, 0xA9, 0x40, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
0x45, 0x00, 0xA0, 0x5A, 0x00, 0x00, 0x00, 0x1E, 0x66, 0x21, 0x50, 0xB0, 0x51, 0x00, 0x1B, 0x30,
0x40, 0x70, 0x36, 0x00, 0xA0, 0x5A, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x18,
0x4B, 0x1A, 0x51, 0x17, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
0x00, 0x53, 0x41, 0x4D, 0x53, 0x55, 0x4E, 0x47, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x59,

0x02, 0x03, 0x33, 0xF1, 0x46, 0x90, 0x04, 0x05, 0x03, 0x02, 0x01, 0x23, 0x09, 0x07, 0x07, 0x83,
0x01, 0x00, 0x00, 0xE2, 0x00, 0x0F, 0xE3, 0x05, 0x03, 0x01, 0x78, 0x03, 0x0C, 0x00, 0x10, 0x00,
0xB8, 0x2D, 0x20, 0xC0, 0x0E, 0x01, 0x00, 0x00, 0x12, 0x18, 0x20, 0x28, 0x20, 0x38, 0x20, 0x58,
0x20, 0x68, 0x20, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xA0,
0x5A, 0x00, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25,
0x00, 0xA0, 0x5A, 0x00, 0x00, 0x00, 0x9E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10,
0x3E, 0x96, 0x00, 0xA0, 0x5A, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
};
#else
#define EDID_VSDB_ADDR				(0x9F)
static iTE_u8 _CODE DEFAULT_EDID_TAB[256] = {
0x00,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0x00,
0x44,	0x89,	0xD4,	0x03,	0x15,	0xCD,	0x5B,	0x07,
0x15,	0x15,	0x01,	0x03,	0x80,	0x50,	0x2D,	0x78,
0x1A,	0x0D,	0xC9,	0xA0,	0x57,	0x47,	0x98,	0x27,
0x12,	0x48,	0x4C,	0x20,	0x00,	0x00,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x02,	0x3A,
0x80,	0x18,	0x71,	0x38,	0x2D,	0x40,	0x58,	0x2C,
0x45,	0x00,	0x20,	0xC2,	0x31,	0x00,	0x00,	0x1E,
0x01,	0x1D,	0x80,	0x18,	0x71,	0x1C,	0x16,	0x20,
0x58,	0x2C,	0x25,	0x00,	0xC4,	0x8E,	0x21,	0x00,
0x00,	0x9E,	0x00,	0x00,	0x00,	0xFC,	0x00,	0x48,
0x44,	0x4D,	0x49,	0x20,	0x41,	0x6E,	0x61,	0x6C,
0x79,	0x7A,	0x65,	0x72,	0x00,	0x00,	0x00,	0xFD,
0x00,	0x17,	0xF1,	0x08,	0x8C,	0x1E,	0x00,	0x0A,
0x20,	0x20,	0x20,	0x20,	0x20,	0x20,	0x01,	0x9B,

0x02,	0x03,	0x2E,	0x73,	0x52,   0x1F,   0x90,   0x22,
0x21,   0x20,   0x1D,	0x1A,   0x16,   0x14,   0x13,   0x12,
0x0F,   0x0B,   0x07,   0x05,   0x04,   0x03,   0x01,	0x23,
0x0F,   0x7F,   0x07,   0x72,   0x03,   0x0C,   0x00,   0x10,
0x00,   0xB8,   0x3C,   0x20,   0xC0,   0x08,   0x00,   0x41,
0x82,   0x10,   0x88,   0x10,   0xE8,   0x10,   0x01,   0x1D,
0x00,   0x72,   0x51,   0xD0,   0x1E,   0x20,   0x6E,   0x28,
0x55,   0x00,   0xA0,   0x5A,   0x00,   0x00,   0x00,   0x1E,
0x01,   0x1D,   0x80,   0x18,   0x71,   0x1C,   0x16,   0x20,
0x58,   0x2C,   0x25,   0x00,   0xA0,   0x5A,   0x00,   0x00,
0x00,   0x9E,   0x8C,   0x0A,   0xD0,   0x8A,   0x20,   0xE0,
0x2D,   0x10,   0x10,   0x3E,   0x96,   0x00,   0xA0,   0x5A,
0x00,   0x00,   0x00,	0x18,	0x00,	0x00,	0x00,	0x00,
0x00,   0x00,   0x00,	0x00,	0x00,	0x00,	0x00,   0x00,
0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,   0xEB,
};
#endif
#endif
#endif

#if iTE_EDID_Msg_Print
	#define iTE_EDID_Msg(x)			iTE_Msg(x)
#else
	#define iTE_EDID_Msg(x)
#endif
//******************* Defination for Switch **************************//

#define DbgSel						(5)
#define EnVCLKInv 					(iTE_FALSE)
#define EnCLKD5Inv					(iTE_FALSE)
#define DisClkDiffInt					(iTE_TRUE)
#define	V18V						(0)
#define	V12V						(0)
#define DisIntEDID					(iTE_FALSE)
#define EnEDIDSeg					(iTE_TRUE)
#define UseIPLOCK 					(iTE_FALSE)
#define EnTxHPD2RxHPD 				(iTE_FALSE)//(iTE_TRUE)

#define AutoPortChgTxAFETog			(iTE_TRUE)
#define AutoPortChgRxAFETog			(iTE_FALSE)
#define AutoPortChgDetRst 			(iTE_FALSE)

#define EnHPDPD						(iTE_TRUE)		// TX HPD Pull-Down
#define EnRxPwrPD					(iTE_FALSE)		// RX PWR Pull-Down
#define EnH2VAutoRst					(iTE_TRUE)
#define EnSkewFailRst				(iTE_TRUE)

#define AutoHPDDly					(0x00)
#define AutoPathEnDly 				(0x00)
#define AutoCapRdyDly				(0x00)

#define EnPTGenK706					(iTE_FALSE)//(iTE_TRUE)
#define ForceWrUpd 					(iTE_TRUE)

#define RCLKDeltaSel 					(2)

#define DDCWAITNUM					(10)
#define DDCWAITTIME				(1)

#define EQRTYMAX					(100)
#define WCLKRdyUnit					(1)			//V1.07_1
// CBUS Input Option
#define EnCBusSMT 					(iTE_FALSE)	// FASLE for 4.3.13.1 &  4.3.16.1 ATC
// CBUS Driving
#define CBUSDrv						(1)

#if iTE_SW_Msg_Print
	#define iTE_SW_Msg(x)			iTE_Msg(x)
#else
	#define iTE_SW_Msg(x)
#endif

#define EnMaxTxSwing				(0)
//******************* Defination for MHL **************************//
#ifdef _MSC_HANDLER_TAB_

iTE_u8 _CODE uc3DDtd[]={0x00};

#if USING_DEFAULT_3D_VIC
iTE_u8 _CODE uc3DVic[]={0x00};
#endif

#endif
//MHL DCAP
#define MHLVer			 			(0x21)    // V1.2=0x12, V2.0=0x20

#define DEV_TYPE					(0x01)	//Sink:0x01, Source:0x02, Dongle:0x03
#define POW							(0x01)
#define PLIM							(0x01)	//500mA:0x00, 900mA:0x01, 1500mA:0x10

#define ADOPTER_ID_H				(0x02)
#define ADOPTER_ID_L				(0x45)

#define SUPP_VID_LINK				(0x37)
#define EnPackPix			 		(iTE_TRUE)

#define AUD_CH 						(0x03)    // 2ch: 0x01, 8ch: 0x03

#define VIDEO_TYPE					(0x00)
#define LOG_DEV_MAP				(0x41)
#define BANDWIDTH					(0x0F)
#define FEATURE_FALG				(0x1F)
#define DEVICE_ID_H					(0x63)
#define DEVICE_ID_L					(0x32)
#define SCRATCHPAD_SIZE			(0x10)
#define INT_STA_SIZE				(0x33)


#define EnHDCPBypass 				(iTE_TRUE)   // iTE_FALSE: Internal for demo, iTE_TRUE: External
#define EnPPHDCPOpt 				(iTE_TRUE)
#define EnPPHDCPOpt2 				(iTE_TRUE)//(iTE_FALSE)//(iTE_TRUE)
#define EnPPGBSwap 					(iTE_TRUE)

// CBUS Option
#define AckHigh						(0xB)
#define AckLow						(1)
// CBUS Discovery and Disconnect Option
#define EnCBusReDisv 				(iTE_FALSE)			// FALSE for ATC
#define CBusTimeAdj					(iTE_TRUE)			// iTE_TRUE: for Some HTC Phone, iTE_FALSE: for ATC
#define EnCBusFloat					(iTE_TRUE)
#define EnWakeUpAdj 				(iTE_TRUE)			 // iTE_TRUE: Enable auto CBusTimeAdj for HTC Phone
#define EnStandby					(iTE_FALSE)

// Link Layer Option
#define DisCBusBurst				(iTE_FALSE)   // iTE_TRUE: for CBUS single packet mode

// CBus Debug Mode
#define EnCBusDbgMode 				(iTE_FALSE)  // Enable Debug Mode : iTE_TRUE
#define DDCCBusDbgCtrl				(iTE_FALSE)
#define MSCCBusDbgCtrl				(iTE_FALSE)

#define WAKEUPFAILMAX				(600)
#define WAKEUPADJNUM    			(10)
#define WAITABORTNUM  			  	(3000)//(300)			//change from 3000 to 5000 for ATC 6.3.6.5
#define CBUSINITIME   			  	(5)
#define MSCHwMask 					(0x0040)			// READ_DEVCAP firmware mode
#define CBUSRTYMAX      				(3)				// default=20, 3 for ATC

// CBUS Input Option
#define EnCBusDeGlitch				(iTE_TRUE)
#define MSCT100msTO				(2)        			// 0: 100ms, 1: 99ms, 2: 101ms

#define MSCBurstWrTO				(2)					// 0: 320ms, 1: 340ms, 2: 360ms for ATC

#if (EnCBusDbgMode && (MSCCBusDbgCtrl || DDCCBusDbgCtrl))
#define MaskTxPktDoneInt 			(iTE_FALSE)
#define MaskRxPktDoneInt			(iTE_FALSE)
#else
#define  MaskTxPktDoneInt			(iTE_TRUE)
#define  MaskRxPktDoneInt			(iTE_TRUE)
#endif
#define EnTxMSG						(iTE_FALSE)

// DDC Option
#define MaskDDCDoneInt				(iTE_FALSE)
// MSC Option
#define MaskMSCDoneInt				(iTE_FALSE) // MSC Rpd Done Mask and MSC Req Done Mask

#define MSCBurstWrID				(iTE_FALSE)   // iTE_TRUE: Adopter ID from MHL5E/MHL5F
#define EnMSCBurstWr 				(iTE_TRUE)    // iTE_TRUE: default
#define MSCBurstWrOpt				(iTE_FALSE)  // iTE_TRUE: Not write Adopter ID into ScratchPad
#define EnPktFIFOBurst				(iTE_TRUE)  // iTE_TRUE for MSC test
#define EnMSCHwRty 					(iTE_FALSE)
#define MSCRxUCP2Nack				(iTE_FALSE)

#define VIC_3D_MAX					(32)

#if USING_DEFAULT_3D_VIC
#define USING_CALLOC				(0)		//calloc will not use when USING_DEFAULT_3D_VIC set
#else
#define USING_CALLOC				(0)		//set if system support calloc function
#endif

#if iTE_MRx_Msg_Print
	#define iTE_MRx_Msg(x)			iTE_Msg(x)
#else
	#define iTE_MRx_Msg(x)
#endif
//******************* Defination for CEC **************************//
#define IT66332_LogicAddr			(DEVICE_ID_TV)

#define CEC_RX_SELF_DIS				(1)
#define CEC_RST						(0)
#define CEC_NACK_EN					(0)
#define CEC_CAL_CNT					(1)

#if iTE_CEC_Msg_Print
	#define iTE_CEC_Msg(x)  			iTE_Msg(x)
#else
	#define iTE_CEC_Msg(x)
#endif
//*****************************************************//
typedef enum _ITE_HDMI {
	HDMI_A = 0,
	HDMI_B = 1,
	HDMI_C = 2,
	HDMI_REV = 3
} ITE_HDMI;

typedef enum _ITE_MHL {
	MHL_A = 0,
	MHL_B = 1,
	MHL_C = 2,
	MHL_REV = 3
} ITE_MHL;

typedef enum _ITE_HPD_CTL {
	HPD_AUTO = 0x00,
	HPD_LOW = 0x80,
	HPD_HIGH = 0x88
} ITE_HPD_CTL;

//*****************************************************//
extern iTE_u1 IT66332_Init(void);
extern void IT66332_Irq(void);
void IT66332_sw_pwron(void);
void IT66332_sw_pwrdn(void);
void IT66332_SwitchHdmiMhl(ITE_HDMI ucHdmiIn, ITE_HDMI ucMhlIn);
void IT66332_GetHdmi(ITE_HDMI *ucpCurHdmi);
void IT66332_GetMhl(ITE_MHL *ucpCurMhl);
void IT66332_SetHpd(ITE_HDMI ucHdmiPort, ITE_HPD_CTL ucState);
void IT66332_PowerDown(iTE_u1 bEnable);
iTE_u8 IT66332_GetReg(iTE_u8 ucI2cAddr, iTE_u8 ucReg);
#if USING_MHL
void IT66332_MscRcpSend(iTE_u8 ucRcpKey);
void IT66332_MscUcpSend(iTE_u8 ucUcpKey);
#endif
extern ITE_HDMI ucHDMI_Port;
extern ITE_HDMI ucMHL_Port;
extern iTE_u32 fRefClk;
extern iTE_u1	bStandBy;
#endif
