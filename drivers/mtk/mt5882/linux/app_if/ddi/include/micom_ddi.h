/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file micom_ddi.h
*
*	Micom Interface header file.
*
*	@author moon kang yun(kymoon@lge.com)
*	@author yongdong lee(cosstar@lge.com) 	- modifier
*	@author kunil lee(dreamer@lge.com) 	- modifier
*	@author hojin koh(hojin.koh@lge.com) 	- modifier
*	@version	1.0
*	@date	  2006.07.08
*	@note
*/

/*---------------------------------------------------------
    (Header File Guarder )
---------------------------------------------------------*/
#ifndef _MICOM_DDI_H_
#define _MICOM_DDI_H_

/*---------------------------------------------------------
    Control ��� ����
    (Control Constants)
---------------------------------------------------------*/

/*---------------------------------------------------------
    #include ���ϵ�
    (File Inclusions)
---------------------------------------------------------*/
#include "global_configurations.h"

/*---------------------------------------------------------
    ��� ����
    (Constant Definitions)
---------------------------------------------------------*/
#if (PLATFORM_TYPE == BCM_PLATFORM)
	/* �ӽ� : BCM Chip�� DDC�� ���� GPIO ������ ����� �ʿ� ����. ���� �ش� �ڵ����� ���� �Ұ�.*/
#undef	INCLUDE_RGB_EDID_EX_MICOM
#endif


// moved to global_configurations.h
// #define MICOM_SIZE_OF_POWER_OFF_DATA			(128)	// 0x80: index(1) + power off status(127)

/*---------------------------------------------------------
    ��ũ�� �Լ� ����
    (Macro Definitions)
---------------------------------------------------------*/

/*	I2C Access Macro */
#define I2C_MICOM_READ(  a, b, c, d, e )	DDI_I2C_Read(  I2C_DEV_MICOM,(a),(b),(c),(d),(e) )
#define I2C_MICOM_WRITE( a, b, c, d, e )	DDI_I2C_Write( I2C_DEV_MICOM,(a),(b),(c),(d),(e) )
#define I2C_MICOM_FRC_READ(  a, b, c, d, e )	DDI_I2C_Read(  I2C_DEV_MICOM_FRC,(a),(b),(c),(d),(e) )
#define I2C_MICOM_FRC_WRITE( a, b, c, d, e )	DDI_I2C_Write( I2C_DEV_MICOM_FRC,(a),(b),(c),(d),(e) )


/*---------------------------------------------------------
    Type ����
    (Type Definitions)
---------------------------------------------------------*/

/**
 * MICOM's version number
 *
 * @see
*/
typedef	struct
{
	UINT8	number[8];	/**< version number	*/

}	MICOM_VERSION_T;


/**
 * LED's version number
 *
 * @see
*/
typedef	struct
{
	UINT8	number[2];	/**< version number	*/

}	LED_VERSION_T;


#ifdef INCLUDE_HDMICEC
/**
 * HDMI CEC
 *
 * @see
*/
typedef	struct
{
	UINT8	command;		/**< HDMI CEC: command	*/
	UINT8	number;			/**< HDMI CEC: number	*/
	UINT8	msgID;			/**< HDMI CEC: message ID	*/
	UINT8	cecdata[20];	/**< HDMI CEC: CEC data	*/

}	MICOM_HDMICEC_T;

#endif	/* #ifdef INCLUDE_HDMICEC */

#ifdef INCLUDE_HOTEL_MODE
/**
 * HOTEL_MODE: Power Control Mode
 *
 * @see
*/
typedef enum
{
	MICOM_PWC_LAST_POWER		= 0,	/**< Power Control Mode: LAST POWER	*/
	MICOM_PWC_ONLY_STBY 		= 1,	/**< Power Control Mode: Only Stand by  */
	MICOM_PWC_ONLY_POWON		= 2,	/**< Power Control Mode: Only Power ON	*/
	MICOM_PWC_UNKNOWN   		= 3		/**< Power Control Mode: UNKNOWN(PARAM ����) */

} 	MICOM_PWC_T;

/**
 * HOTEL_MODE: key operarion type.
 *
 * @see
*/
typedef enum
{
	MICOM_KEYOP_NORMAL      	= 0,	/**< key Operation NORMAL ����				*/
	MICOM_KEYOP_ONLY_IR     	= 1,	/**< key Operation IR�� ó���ϴ� ����		*/
	MICOM_KEYOP_ONLY_LOCAL   	= 2,	/**< key Operation LOCALŰ�� ó���ϴ� ����	*/
	MICOM_KEYOP_ONLY_IR_POWER	= 3,	/**< key Operation IR POWER�� ó���ϴ� ����	*/
	MICOM_KEYOP_ALL_LOCK		= 4,	/**< key Operation IR & LOCAL KEY ó������ �ʴ� ���� 	*/
	MICOM_KEYOP_SERVICEMODE    	= 5, 	/**< key Operation Service mode Local key �����ϴ� ���� */
	MICOM_KEYOP_UNKNOWN      	= 6 	/**< key Operation UNKNOWN ����(PARAM ����)	*/

} 	MICOM_KEY_OPERATION_T;

#endif	/* #ifdef INCLUDE_HOTEL_MODE */

/**
 * Power indicator command.
 *
 * @see	MICOM_GetImageType( UINT8 * pImageType)
 */
typedef enum
{
	PWR_IND_STANDBY_LED	= 0, 	/**< STANDBY LED�� level ����  	*/
	PWR_IND_POWER_LED	= 1, 	/**< POWER LED ON/OFF			*/
	PWR_IND_SA_SOUND	= 2,	/**< SA SOUND ON/OFF			*/
	PWR_IND_DEMO_MODE	= 3,	/**< DEMO MODE ON/OFF			*/
	PWR_IND_LAST				/**< LAST VALUE to Check Params */

}	DDI_MICOM_PWR_IND_CMD_T;	// POWER INDICATOR


/**
 * Reserve type
 */
typedef enum
{
	MICOM_RESERVED_BY_REMIND		= 0,	/**< ���� ��û */
	MICOM_RESERVED_BY_RESERVE		= 1,	/**< ���� ��ȭ */
	MICOM_RESERVED_BY_MEMOCAST		= 2		/**< MemoCast */
} MICOM_RESERVE_TYPE_T;



#define POWER_INDICATOR_SB_LIGHT 		0x1
#define POWER_INDICATOR_PW_LIGHT 		0x2



/** UI Messaging System�� ���Ǿ����� Callback �Լ��� ���� ����.
 *	API_UTIL_InitUIMessagingSystem �Լ� ȣ�� �� ���Ǿ�����.
 */
#define  MAX_MICOM2UI_MSG_PARAM 	4

typedef void (*DDI_MICOM_UI_MSG_CALLBACK_PTR_T) (UINT32 msg, UINT32 param[MAX_MICOM2UI_MSG_PARAM]);
typedef void (*DDI_MICOM_UI_KEY_CALLBACK_PTR_T) (UINT32 key, UINT32 keyType );
//typedef void (*DDI_MICOM_UI_KEY_CALLBACK_PTR_T) (UINT32 key, BOOLEAN bRepeat);
typedef void (*DDI_MICOM_AUTOTEST_CALLBACK_PTR_T) (UINT16 key, UINT16 keyType ); //hojin.koh_110219 - 2 Byte IR Key Code ����.
																				//shinsung.kang_110725 - UINT8 key -> UINT16 keky
#ifdef INCLUDE_WIRELESS_READY
typedef void (*DDI_MICOM_WIRELESS_PLUG_T) (void);
typedef void (*DDI_MICOM_WIRELESS_UNPLUG_T) (void);
#endif

/*---------------------------------------------------------
    �Լ� ����
    (Function Declaration)
---------------------------------------------------------*/

/*=======================================================
	MICOM Interface Function ���� (micom_task.c)
=======================================================*/
extern	DTV_STATUS_T	DDI_MICOM_Initialize(void);
extern	void 			DDI_MICOM_AutoTest(UINT16 type, UINT16 value);
extern	DTV_STATUS_T	DDI_MICOM_SetPowerKeyLock( BOOLEAN isLocked );
extern	DTV_STATUS_T	DDI_MICOM_SetTVLinkIRMode( BOOLEAN isEnabled );
extern	DTV_STATUS_T	DDI_MICOM_SetWhiteBalanceIRMode( BOOLEAN isEnabled );
extern 	DTV_STATUS_T DDI_MICOM_SetSelfDiagnosisIRMode(BOOLEAN isEnabled);
extern DTV_STATUS_T DDI_MICOM_SetTouchSensitivityAdjustMode( BOOLEAN isEnabled);


extern	void 			DDI_MICOM_RegisterUICallback( 	DDI_MICOM_UI_MSG_CALLBACK_PTR_T pfnMsgCbFn,
														DDI_MICOM_UI_KEY_CALLBACK_PTR_T pfnKeyCbFn  );
extern 	void 			DDI_MICOM_RegisterAutoTestCallback( DDI_MICOM_AUTOTEST_CALLBACK_PTR_T pfnAutoTestCbFn );
extern  DTV_STATUS_T 	DDI_MICOM_SetSSUBlinking(BOOLEAN bEnable);

#ifdef INCLUDE_WIRELESS_READY
extern void DDI_MICOM_RegisterWirelessDetCallback(	DDI_MICOM_WIRELESS_PLUG_T pfnWirelessPlug,
									DDI_MICOM_WIRELESS_UNPLUG_T pfnWirelessUnplug);
extern DTV_STATUS_T DDI_MICOM_GetWirelessDongleConnection(UINT8* pMode);
#endif
/*=======================================================
	MICOM Interface Function ���� (micom_if.c)
=======================================================*/
extern	DTV_STATUS_T	DDI_MICOM_GetVersion (MICOM_VERSION_T *pVersion);
extern	DTV_STATUS_T	DDI_FRCMICOM_GetVersion (MICOM_VERSION_T *pVersion);

extern	DTV_STATUS_T	DDI_MICOM_GetRTCTime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_SetRTCTime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_SetOnTime (UINT8 hour, UINT8 minute, UINT8 repeat);
extern  DTV_STATUS_T	DDI_MICOM_GetOnTime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_CancelOnTime (void);
extern	DTV_STATUS_T	DDI_MICOM_SetOffTime (UINT8 hour, UINT8 minute, UINT8 repeat);
extern	DTV_STATUS_T	DDI_MICOM_CancelOffTime (void);
extern	DTV_STATUS_T	DDI_MICOM_SetReserveTime (TIME_T *pTime, UINT8 uReservedType);
extern	DTV_STATUS_T	DDI_MICOM_CancelReserveTime (void);
extern	DTV_STATUS_T	DDI_MICOM_SetOTAOnTime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_GetOTATime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_SetOTATime (TIME_T *pTime);

extern	DTV_STATUS_T	DDI_MICOM_SetChildLock( BOOLEAN isLocked );
extern	DTV_STATUS_T	DDI_MICOM_ReleasePoweronMode ( BOOLEAN isInstop );

extern	DTV_STATUS_T	DDI_MICOM_SetEITTime (UINT8 hour, UINT8 minute);
extern	DTV_STATUS_T	DDI_MICOM_CancelEITTime (void);
extern 	DTV_STATUS_T	DDI_MICOM_GetEITTime (TIME_T *pTime);

extern	DTV_STATUS_T	DDI_MICOM_SetEmmTime (TIME_T *pTime);
extern	DTV_STATUS_T	DDI_MICOM_CancelEmmTime (void);
extern 	DTV_STATUS_T	DDI_MICOM_GetEmmTime (TIME_T *pTime);

extern  DTV_STATUS_T	DDI_MICOM_SetOperatorSearchTime (TIME_T * pTime);
extern  DTV_STATUS_T	DDI_MICOM_GetOperatorSearchTime (TIME_T * pTime);
extern 	DTV_STATUS_T	DDI_MICOM_CancelOperatorSearchTime (void);

extern  DTV_STATUS_T	DDI_MICOM_ResetPowerOffStatus(void);
// <090216 Leehc> RS232 KM command ( Local key , IR Lock ) ����.
// DC off �ÿ��� �����ǰ� AC off �ÿ��� �����ǵ��� �ϱ� ���� ���ο� micom protocol �߰���.
extern  DTV_STATUS_T 	DDI_MICOM_SetEventLock( BOOLEAN isLocked );
extern 	DTV_STATUS_T 	DDI_MICOM_SetRS232KeyLock(BOOLEAN isLocked,  UINT8 region);
extern  DTV_STATUS_T 	DDI_MICOM_GetRS232KeyLock(BOOLEAN *pisLocked);

extern	DTV_STATUS_T	DDI_MICOM_GetReserveTime (TIME_T *pTime, BOOLEAN *isReserved);
extern	DTV_STATUS_T	DDI_MICOM_SetPollingMode( BOOLEAN isEnabled );
extern	DTV_STATUS_T	DDI_MICOM_SetCommSetID (UINT8 setID);

#ifdef INCLUDE_HOTEL_MODE
extern	DTV_STATUS_T	DDI_MICOM_PowerModeCtrl (MICOM_PWC_T powermode );
extern	DTV_STATUS_T	DDI_MICOM_KeyOperationModeCtrl (MICOM_KEY_OPERATION_T keyoperation );
#endif

#ifdef INCLUDE_HDMICEC
extern	void        	DDI_MICOM_InitCecModule(void);//080811_yongs
extern	DTV_STATUS_T	DDI_MICOM_RequestCecForward(void);
extern	DTV_STATUS_T	DDI_MICOM_SendCecData (MICOM_HDMICEC_T *pCecdata);
extern  DTV_STATUS_T	DDI_MICOM_SetCecSyncPowerOnOff(UINT8 onoff);

#endif


/*	MICOM's POWER MANAGEMENT	micom_power.c  */
extern	DTV_STATUS_T	DDI_MICOM_GetPowerState (DDI_POWER_STATE_T *pPowerState);
extern	DTV_STATUS_T	DDI_MICOM_GetPowerOnMode (DDI_POWER_ON_MODE_T *pPowerOnMode);
extern	DTV_STATUS_T	DDI_MICOM_SetPowerOnMode (DDI_POWER_ON_MODE_T PowerOnMode);
extern	DTV_STATUS_T	DDI_MICOM_SetPowerOffMode( DDI_POWER_OFF_MODE_T tPowerOffMode );
extern	DTV_STATUS_T	DDI_MICOM_GetPowerOffMode( DDI_POWER_OFF_MODE_T * pPowerOffMode );
extern	DTV_STATUS_T	DDI_MICOM_GetPowerOffModeData( UINT8 index, UINT8 * pData );
extern	DTV_STATUS_T	DDI_FRCMICOM_GetPowerOffModeData( UINT8 index, UINT8 * pData );

extern	DTV_STATUS_T	DDI_MICOM_SendSystemPowerControlCommand (DDI_POWER_STATE_T powerState, int powerOnOffMode);
extern	DTV_STATUS_T	DDI_MICOM_SetAutoPowerOffOn(UINT8 rSecond,UINT8 powerOffMode);
extern	DTV_STATUS_T	DDI_MICOM_CancelPowerOff (void);
extern	DTV_STATUS_T	DDI_MICOM_SendHostReadyCommand (void);
extern	DTV_STATUS_T	DDI_MICOM_SendStartedApplication (void);
extern	DTV_STATUS_T	DDI_MICOM_SendHostSystemAliveSignal (void);
extern	DTV_STATUS_T	DDI_MICOM_GetPowerErrorHistory( UINT8 index, UINT8 * pData );
extern	DTV_STATUS_T	DDI_MICOM_ResetPowerErrorHistory(void);

/*	MICOM's GPIO 		micom_gpio.c	*/
extern	DTV_STATUS_T	DDI_MICOM_GPIO_Init(void);
extern	DTV_STATUS_T	DDI_MICOM_GPIO_GetInputPort( UINT32 portIndex, UINT8 *pData );
extern	DTV_STATUS_T	DDI_MICOM_GPIO_SetOutputPort( UINT32 portIndex, UINT8 data );

/*	MICOM's etc 		micom_etc.c 	*/
#ifdef	INCLUDE_INTERNAL_MICOM_CASE
	extern 	DTV_STATUS_T	DDI_MICOM_LED_SetPowerStatus( DDI_POWER_STATE_T state ); //zuel_1021
	extern DTV_STATUS_T	DDI_MICOM_LedSetActiveStatus( DDI_POWER_STATE_T state );
#else
	extern DTV_STATUS_T	DDI_MICOM_LED_SetControlCommand( DDI_MICOM_LED_CMD_T tLEDCmd );
#endif

extern	DTV_STATUS_T 	DDI_MICOM_SetPowerIndicator( DDI_MICOM_PWR_IND_CMD_T tCmd, UINT8 tVal);
extern  DTV_STATUS_T 	DDI_MICOM_SetPowerIndicatorWithoutSave(UINT8 tVal);

extern	DTV_STATUS_T	DDI_MICOM_EnableTouchSensitivityMode( BOOLEAN bOnOff );
extern	DTV_STATUS_T	DDI_MICOM_GetGlobalTouchSensitivity( UINT8 *pVal );
extern	DTV_STATUS_T	DDI_MICOM_SetGlobalTouchSensitivity( UINT8 tVal );
extern	DTV_STATUS_T	DDI_MICOM_GetLocalTouchSensitivity( UINT8 index, UINT8 *pVal );
extern	DTV_STATUS_T	DDI_MICOM_SetLocalTouchSensitivity( UINT8 index, UINT8 tVal );
extern	DTV_STATUS_T	DDI_MICOM_GetTouchCountTime(UINT8 *pVal);
extern	DTV_STATUS_T	DDI_MICOM_SetTouchCountTime(UINT8 tVal);
extern	DTV_STATUS_T	DDI_MICOM_GetTouchScanData(UINT8 index, UINT16 *pVal);

extern	DTV_STATUS_T	DDI_MICOM_GetTouchDefaultScanData(UINT8 index, UINT16 *pVal);
extern	DTV_STATUS_T	DDI_MICOM_SetDefaultTouchSensitivity(UINT8 index, UINT8 tVal);
extern	DTV_STATUS_T	DDI_MICOM_SetTouchRecalculation(UINT8 tVal);
extern	DTV_STATUS_T	DDI_MICOM_ResetTouchSensitivity(void);

extern	DTV_STATUS_T	DDI_MICOM_EnableRGBSensor( BOOLEAN bOnOff );
extern	DTV_STATUS_T	DDI_MICOM_LED_SetDisplayMode(BOOLEAN bOnOff);
extern	DTV_STATUS_T	DDI_MICOM_SetToolType(UINT8 type);
extern	DTV_STATUS_T	DDI_MICOM_SetPDPRomDownloadStatus( UINT8 status );

#if 0
extern  void 			DDI_MICOM_LED_EnableBlinkLED(BOOLEAN bOnOff);
#endif

#ifdef INCLUDE_RGB_EDID_EX_MICOM
extern	DTV_STATUS_T	DDI_MICOM_UpdateRGBEDIDDate(UINT8 year, UINT8 month );
extern	DTV_STATUS_T	DDI_MICOM_ResetRGBEDID(void);
#endif

extern DTV_STATUS_T DDI_MICOM_SetBaudRate(UINT8 baudrate);
extern DTV_STATUS_T DDI_MICOM_WriteOLP(UINT8 onoff);
extern DTV_STATUS_T DDI_MICOM_WriteCricketFamily(void);


extern DTV_STATUS_T DDI_MICOM_GetSubAssyType(UINT8 *pAssyType);
extern DTV_STATUS_T DDI_MICOM_GetSlaveStatus(UINT8 *pSStatus);

extern DTV_STATUS_T DDI_MICOM_SetInverterErrorReport(void);//hjkoh_091027
extern	DTV_STATUS_T	DDI_MICOM_GetBreathingLedFWVersion(LED_VERSION_T *pVersion);
extern	DTV_STATUS_T	DDI_MICOM_SetBreathingLedFWVersion(UINT8 tVer);
extern DTV_STATUS_T 	DDI_MICOM_SetLEDBlinkingStatus(UINT8 onoff, UINT8 speed);
extern DTV_STATUS_T DDI_MICOM_SetPowerBoardType(UINT8 powerbd);

extern DTV_STATUS_T DDI_MICOM_SetDebugStatus(UINT8 debugstatus);
extern UINT8 DDI_MICOM_GetDebugStatus(void);

//<091117 leehc> DC power off/on auto test ���.
extern DTV_STATUS_T	DDI_MICOM_SetAutoTest(UINT8 onperiodSec , UINT8 offperiodSec);

extern DTV_STATUS_T DDI_MICOM_SetI2CCommandDisable(void);//Micom File USB D/L�� Micom DDI Call�� ���� I2C ����� �������� �Լ�.
extern DTV_STATUS_T DDI_MICOM_SetI2CCommandEnable(void);
extern DTV_STATUS_T DDI_MICOM_GetImageType (UINT8 * pImageType);

extern BOOLEAN  DDI_MICOM_GetDoingSelfDiagnosis(void);
extern void DDI_MICOM_SetDoingSelfDiagnosis(BOOLEAN mode);
extern DTV_STATUS_T	DDI_MICOM_GetMicomModelOption(UINT8 *pModelOption);

extern DTV_STATUS_T DDI_MICOM_SubAssyWriteTest(UINT8 cLen , UINT8 *data);
DTV_STATUS_T	DDI_MICOM_SubAssyReadTest(UINT8 cmd , UINT8 *readValue);
DTV_STATUS_T	DDI_MICOM_GetTouchKeyFWVersion(UINT8 pVer[2]);
BOOLEAN			DDI_MICOM_GetIRRepeatStatus(void);
extern DTV_STATUS_T DDI_MICOM_GetLogoLightBright(UINT8 * pLogoBright);
DTV_STATUS_T 	DDI_MICOM_GetLastPwroffHistory(UINT8 *pHistData);
extern DTV_STATUS_T	DDI_MICOM_Main_WDT(void);
extern DTV_STATUS_T	DDI_MICOM_SetTouchEeprom (UINT8 cmdTouchSense, UINT8 dataTouchSense);
extern 	DTV_STATUS_T	DDI_MICOM_SetEdidWPControlValue (UINT8 wpStatus);
extern 	DTV_STATUS_T	DDI_MICOM_SetDCOffOnKeyLockTime (void);
extern  DTV_STATUS_T	DDI_MICOM_SetLogoLight (UINT8 stdBright, UINT8 actBright); //logo light ������.
extern  DTV_STATUS_T	DDI_MICOM_SetLogoLightComplete (UINT8 setComplete);
extern DTV_STATUS_T	DDI_MICOM_SetWOLWOLOnOff (UINT8 bChkWOL, UINT8 tmpRsv);
extern DTV_STATUS_T DDI_MICOM_GetWOLOnOff(UINT8 * pWOLOnOff);


#endif /* #ifndef _MICOM_DDI_H_ */
