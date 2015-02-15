///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <MHL_Rx.h>
//   @author Hanming.Cheng@ite.com.tw
//   @date   2013/09/18
//   @fileversion: ITE_IT66332_1.01
//******************************************/
#ifndef _MHL_RX_H_
#define _MHL_RX_H_
#include "../IT66332_Customer/IT66332_IO.h"
#if USING_MHL

#define MSC_3D_VIC		(0x0010)
#define MSC_3D_DTD		(0x0011)

typedef enum _MHL_INT{
	MHL_INT_3D_REQ = 0x10,
	MHL_INT_GNT_WRT = 0x08,
	MHL_INT_REQ_WRT = 0x04,
	MHL_INT_DSCR_CHG = 0x02,
	MHL_INT_EDID_CHG = 0x02
}MHL_INT;
typedef enum _MHL_STS{
	MHL_STS_DCAP_RDY = 0x01,
	MHL_STS_MUTED = 0x10,
	MHL_STS_PATH_EN = 0x08,
	MHL_STS_CLK_24 = 0x03,
	MHL_STS_CLK_PP = 0x02
}MHL_STS;
typedef enum _MSC_MODE{
	MSC_FW_MODE,
	MSC_HW_MODE,
	MSC_WRITE_BURST_MODE
}MSC_MODE;

typedef struct _MSC_CMD{
	iTE_u8	ucOffset;
	iTE_u8	ucValue;
	iTE_u8	ucCnt;
}MSC_CMD, *P_MSC_CMD;

typedef enum _MSC_STATUS {
	MSC_SUCCESS = 0,
	MSC_FAIL,
	MSC_ABORT,
	MSC_RCVABORT,
	MSC_RCVNACK,
	MSC_ARBLOSE,
	MSC_FWTXFAIL,
	MSC_FWRXPKT
} MSC_STATUS ;


typedef struct _MSC_WB_INFO{
	iTE_u8	*pucTxScrpad;
	iTE_u8	ucCnt;
	iTE_u8	ucOffset;
}MSC_WB_INFO;

typedef enum _MSC_MSG_CMD{
	MSC_MSG_MSGE		= 0x02,
	MSC_MSG_RCP		= 0x10,
	MSC_MSG_RCPK		= 0x11,
	MSC_MSG_RCPE		= 0x12,
	MSC_MSG_RAP		= 0x20,
	MSC_MSG_RAPK		= 0x21,
	MSC_MSG_UCP		= 0x30,
	MSC_MSG_UCPK		= 0x31,
	MSC_MSG_UCPE		= 0x32
}MSC_MSG_CMD;

typedef enum _MSC_STATUS_CODE{
	MSC_NO_ERROR = 0x00,
	MSC_INVALID_CODE = 0x01,
}MSC_STATUS_CODE;
typedef enum _RAP_ACTION_CODE{
	RAP_POLL = 0x00,
	RAP_CONTENT_ON = 0x10,
	RAP_CONTENT_OFF = 0x11
}RAP_ACTION_CODE;

typedef enum _RAP_ERROR{
	RAP_NO_ERROR = 0x00,
	RAP_UNRECOGN_ACTION_CODE = 0x01,
	RAP_UNSUPPORT_ACTION_CODE = 0x02,
	RAP_RESPONDER_BUSY = 0x03
}RAP_ERROR;

typedef enum _RCP_ERROR{
	RCP_NO_ERROR = 0x00,
	RCP_INEFFECTIVE_KEY = 0x01,
	RCP_RESPONDER_BUSY = 0x02
}RCP_ERROR;

typedef enum _UCP_ERROR{
	UCP_NO_ERROR = 0x00,
	UCP_INEFFECTIVE_KEY = 0x01
}UCP_ERROR;

typedef enum _MHL3D_STATE{
	MHL3D_REQ_START,
	MHL3D_REQ_WRT,
	MHL3D_GNT_WRT,
	MHL3D_WRT_BURST,
	MHL3D_REQ_DONE
}MHL3D_STATE;


typedef enum _MSC_PKT_ID{
	MSC_PKT_WRITE_STAT = 0,
	MSC_PKT_SET_INT,
	MSC_PKT_READ_DEVCAP,
	MSC_PKT_GET_STATE,
	MSC_PKT_GET_VENDOR_ID,
	MSC_PKT_SET_HPD,
	MSC_PKT_CLR_HPD,
	MSC_PKT_MSC_MSG,
	MSC_PKT_GET_DDC_ERRORCODE,
	MSC_PKT_GET_MSC_ERRORCODE,
	MSC_PKT_WRITE_BURST,
	MSC_PKT_GET_SC1_ERRORCODE,
	MSC_PKT_GET_SC3_ERRORCODE,
	MSC_PKT_EOF,
	MSC_PKT_ACK,
	MSC_PKT_NACK,
	MSC_PKT_ABORT,
	MSC_PKT_ID_END
}MSC_PKT_ID;

#ifdef _MHL_RX_INIT_TAB_
//static iTE_u8 _CODE Msc_CtlPktTab[MSC_PKT_ID_END]={0x33, 0x34, 0x35, 0x60, 0x60, 0x61, 0x62, 063, 0x64, 0x65, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x32};
//	iTE_u8 _CODE uc3DDtd[]={0x00, 0x11, 0xEE, 0x00, 0x01, 0x00};

iTE_u8	_CODE MscFire[17][2] = {		{0x50, 0x80},		{0x50, 0x80},		{0x50, 0x40},		{0x50, 0x01},
	{0x50, 0x02},		{0x50, 0x04},		{0x50, 0x08},		{0x51, 0x02},		{0x50, 0x10},		{0x50, 0x20},
	{0x51, 0x01},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},		{0x51, 0x80},
	{0x51, 0x80},
};

static iTE_u8 _CODE MHL_RX_INIT_TAB[][3] = {
	{0x00, 0x8f, (EnCBusDbgMode<<7) + (MSCCBusDbgCtrl<<2) + DDCCBusDbgCtrl},
	{0x01, 0x8c, (EnCBusDeGlitch<<7) + (MSCT100msTO<<2)},
	{0x08, 0xff, (MaskRxPktDoneInt<<2) + (MaskTxPktDoneInt)},
	{0x09, 0xff, (MaskDDCDoneInt<<6) + (MaskDDCDoneInt<<4) + (MaskMSCDoneInt<<2) + MaskMSCDoneInt},
	{0x0a, 0xff, 0x00},
	{0x0c, 0x88, (EnPPHDCPOpt<<7) + (EnPPHDCPOpt2<<3)},
	{0x0f, 0x20, EnPPGBSwap<<5},
	{0x05, 0xff, 0xff},
	{0x06, 0xff, 0xff},
	{0x07, 0xff, 0xff},
	{0x28, 0x0c, (CBusTimeAdj<<3) + (EnCBusFloat<<2)},
	{0x29, 0x0c, 0x04},					// CBus Deglitch select
	{0x36, 0xfc, (AckHigh<<4) + (AckLow<<2)},
	{0x3c, 0x10, EnHDCPBypass << 4},
	{0x5c, 0xbc, (EnPktFIFOBurst<<7) + (MSCBurstWrOpt<<5) + (EnMSCBurstWr<<4) + (EnMSCHwRty<<3) + (MSCRxUCP2Nack<<2)},
	{0x66, 0x03, MSCBurstWrTO},

#if 1
	{0x81, 0xff, MHLVer},					//MHL Version
	{0x82, 0xff, (PLIM<<5)+(POW<<4)+DEV_TYPE},		//DEV_CAT	// for 4.3.24.1 ATC
	{0x83, 0xff, ADOPTER_ID_H},			//ADOPTER_ID_H
	{0x84, 0xff, ADOPTER_ID_L},			//ADOPTER_ID_L
	{0x85, 0xff, SUPP_VID_LINK | (EnPackPix << 3)},		//VID_LINK_MODE
	{0x86, 0xff, AUD_CH},					//AUD_LINK_MODE
	{0x87, 0xff, VIDEO_TYPE},				//VIDEO_TYPE
	{0x88, 0xff, LOG_DEV_MAP},			//LOG_DEV_MAP
	{0x89, 0xff, BANDWIDTH},				//BANDWIDTH
	{0x8a, 0xff, FEATURE_FALG},			//FEATURE_FALG
	{0x8b, 0xff, DEVICE_ID_H},			// DEVICE_ID_H
	{0x8c, 0xff, DEVICE_ID_L},			// DEVICE_ID_L
	{0x8d, 0xff, SCRATCHPAD_SIZE},		//SCRATCHPAD_SIZE
	{0x8e, 0xff, INT_STA_SIZE},			// INT_STA_SIZE

#else
	{0x81, 0xff, MHLVer},
	{0x82, 0x70, 0x30},			// for 4.3.24.1 ATC
	{0x83, 0xff, 0x02},			//ADOPTER_ID_H
	{0x84, 0xff, 0x45},			//ADOPTER_ID_L
	{0x85, 0x08, EnPackPix << 3},
	{0x86, 0xff, AUD_CH},
	{0x8b, 0xff, 0x63},			// DEVICE_ID_H
	{0x8c, 0xff, 0x32},			// DEVICE_ID_L
#if (Dongle == iTE_TRUE)
	{0x82, 0x0F, 0x03},
#endif
#endif
#if EnStandby
	{0x28, 0x02, 0x02},
#endif
};
#define MHL_RX_INIT_TAB_SIZE sizeof(MHL_RX_INIT_TAB)/3
#endif
typedef enum _MSC_STATE{
	eMSC_FREE,
	eMSC_GET,
	eMSC_PROCESS,
}MSC_STATE;
typedef struct _MSC_MSG{
	MSC_MSG_CMD	eMsgCmd;
	union{
		MSC_STATUS_CODE	eMsgeStatus;
		RAP_ACTION_CODE	eRapAction;
		RAP_ERROR			eRapkError;
		iTE_u8				ucRcpKey;
		iTE_u8				ucRcpkKey;
		RCP_ERROR			eRcpeStatus;
		iTE_u8				ucUcpChar;
		iTE_u8				ucUcpkChar;
		UCP_ERROR			eUcpeStatus;
	}unMsgData;
}MSC_MSG;
typedef struct _MSC_MSG_STATE{
	union{
		RAP_ACTION_CODE	eActionCode;
		iTE_u8	ucKeyCode;
		iTE_u8	ucCharCode;
	}unData;
	MSC_STATE	eState;
}MSC_MSG_STATE;


void mhl_rx_init(void);
void mhl_cbus_irq(void);
void mhl_set_irq(void);
#if 0
void set_mhlint(iTE_u8 offset, iTE_u8 fiel );
void set_mhlsts(iTE_u8 offset, iTE_u8 status);
void wrboolurst(iTE_u8 offset, iTE_u8 byteno);
SYS_STATUS mscfire(iTE_u8 offset, iTE_u8 Data);
MSC_STATUS mscwait(void);
#endif
void read_mscmsg( void );
void Msc_Read_DevCap(void);
void read_devcap_fwrpd( void );
int read_devcap_fwreqmsg( void );
#if 0
void get_msc_errcode( void );
void send_mscmsg( void );
void set_wrburst_data( int offset, int byteno );
#endif

#if iTE_MRx_Msg_Print
void dbgcbustx( void );
void dbgcbusrx( void );
#endif
void encbusdbg( void );
void discbusdbg( void );
void parse_ctrlpkt( iTE_u8* ctrlpkt );

MSC_STATUS Msc_MscMsgFire(MSC_CMD *MscCmd);
MSC_STATUS Msc_Fire(MSC_PKT_ID eMscCtlPkt, MSC_CMD *MscCmd);
void send_rcpmsg( void );
void send_ucpmsg(void);
void mhl_CBusReDiscovery(void);

SYS_STATUS mhl_rx_set(iTE_u8 Offset, iTE_u8 Mask, iTE_u8 Data);
SYS_STATUS mhl_rx_w(iTE_u8 Offset, iTE_u8 Data);
SYS_STATUS mhl_rx_bw(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
SYS_STATUS	mhl_rx_br(iTE_u8 Offset, iTE_u8 Count, iTE_u8 *pData);
iTE_u8 mhl_rx_r(iTE_u8 Offset);


extern MSC_CMD TxMscCmd;
#endif
#endif
