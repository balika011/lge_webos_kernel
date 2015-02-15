/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/


/** @file pwm_ddi.h
 *
 *  This header file declares the data types and DDI functions to control the gpio ports
 *
 *  @author		earnest(earnest@lge.com)
 *  @version	1.0
 *  @date		2007.08.01
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _PWM_DDI_H_
#define _PWM_DDI_H_

/******************************************************************************
	#include ���ϵ� (File Inclusions)
******************************************************************************/
#include "common.h"
#include "cmnio_ddi.h"

/******************************************************************************
 	��� ����(Constant Definitions)
******************************************************************************/
#define PWM_VBR_MAX	0xff // 100%
#define PWM_VBR_1P	0x03 // 1% 
#define PWM_VBR_99P	(PWM_VBR_MAX*99/100)	/* 90 */

/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/
/*dreamer : 2008,09,29, (DVB) 37LF7000 DV2 ������, �ӽ÷� pwm1, pwm2 pin���� ���� ���- (ATSC�� �����ϰ�). */
/*taburin : 2008.09.18, mstar atsc mecury 42" pwm1 port ������, �ӽ÷� pwm1, pwm2 pin���� ���� ���.*/


#define PWM_VBR_A_INIT_VALUE			PWM_VBR_MAX
#define PWM_VBR_B_INIT_VALUE			PWM_VBR_MAX
#define PWM_VBR_B_INIT_VALUE_EDGE		PWM_VBR_99P  //99% max


#define PWM_VBR_A_INIT_END_VALUE    	((PWM_VBR_MAX/2)+1)	/* 50% */
#define PWM_VBR_B_INIT_END_VALUE		PWM_VBR_MAX
#define PWM_VBR_B_INIT_END_VALUE_EDGE	PWM_VBR_99P //99% max

//balup_pwm
#if (PLATFORM_TYPE == BCM_PLATFORM || PLATFORM_TYPE == MTK_PLATFORM)

#define PWM_VBR_A_INIT_FREQUENCY_VALUE  2100 //2.1khz   //53000	//150Hz

#if (SYS_ATSC)
#define PWM_VBR_B_INIT_FREQUENCY_VALUE  120
#else
#define PWM_VBR_B_INIT_FREQUENCY_VALUE  100
#endif

#define PWM_VBR_A_INIT_PULSEWIDTH_VALUE		0xA4CA


#define PWM_VBR_B_INIT_PULSEWIDTH_VALUE		0xA4CA


#define PWM_PORT_VBR_A				PWM_DEV_PIN0
#define PWM_PORT_VBR_B				PWM_DEV_PIN1 // ieeum  0���� �����.(������ )
#define PWM_PORT_VBR_C				PWM_DEV_PIN2


#elif (PLATFORM_TYPE == LG_PLATFORM)

#define PWM_VBR_A_INIT_FREQUENCY_VALUE		120		
#if (SYS_ATSC)
#define PWM_VBR_B_INIT_FREQUENCY_VALUE		120
#else
#define PWM_VBR_B_INIT_FREQUENCY_VALUE  	100
#endif

#define PWM_VBR_A_INIT_PULSEWIDTH_VALUE		0xFF 
#define PWM_VBR_B_INIT_PULSEWIDTH_VALUE		0xFF

#define PWM_PORT_VBR_A						PWM_DEV_PIN0 // H/W define
#define PWM_PORT_VBR_B						PWM_DEV_PIN2 // H/W define
#define PWM_PORT_VBR_C						PWM_DEV_PIN1 // LPB���� LED Current �� �����ϱ� ���� PWM duty ���� �� 

#else  //2 MSTAR_PLATFORM

#define PWM_VBR_A_INIT_FREQUENCY_VALUE	0x2C9 // 2.1khz

#define	PWM_VBR_A_DIVIDER				0x07
#define	PWM_VBR_A_CLK_DIV				(12000000 / (PWM_VBR_B_DIVIDER+1))	/* 12MHz / divider */

#define	PWM_VBR_B_DIVIDER				0x07
#define	PWM_VBR_B_CLK_DIV				(12000000 / (PWM_VBR_B_DIVIDER+1))	/* 12MHz / divider */

#define	PWM_VBR_B_DIVIDER_VRST			0x00
#define	PWM_VBR_B_CLK_DIV_VRST			(12000000 / (PWM_VBR_B_DIVIDER_VRST+1))	/* 12MHz / divider */

#define PWM_PORT_VBR_A					PWM_DEV_PIN0
#define PWM_PORT_VBR_B					PWM_DEV_PIN2

#endif  // (PLATFORM_TYPE == BCM_PLATFORM)

#define LED_CURRENT_NORMAL		0
#define LED_CURRENT_CPLUS		1
#define LED_CURRENT_STOREUP		2

#define CURR_LIMIT_CPLUS	0x7f

/******************************************************************************
	�� ���� (Type Definitions)
******************************************************************************/
/**
 * PWM Configuration type
 */
typedef struct {
	UINT8			prescale;			///<00-1,01-2,10-4,11-8
	UINT8			tclkDivEn;			///<0=tclk 1-tclk/511
	UINT16			pwmPeriod;			///<10 bit value
	UINT8	        pwmDevClkSel;		///<only for PWM2/3 0-tclk/tclkby511 1-display hsync
	UINT8			pwmVreset;			///<only for PWM2/3 0-pwm count roll over continuosly 1:on every disp vsync edge
	UINT8			pwmHsyncPolarity;	///<only for PWM2/3 0-normal 1-invert
	UINT8			pwmVsyncPolarity;	///<only for PWM2/3 0-normal 1-invert
} PWM_CONFIG_T;


/**
 *	PWM DUTY changing rule
 */
typedef enum{
	FOLLOW_PWN_ONOFF = 0,
	DONTCARE_PWM_ONOFF = 1,
	STOREMODE_PWM_ONFF =2 //store or home mode detect�� ���� pwm comtrol
}PWM_DUTY_CHANGE_T;

/**
 *	PANEL Pawer Save type
 */
typedef enum {
	PWM_STO_CTRL_NONE = 0,
	PWM_STO_CTRL_VBRA = 1,
	PWM_STO_CTRL_VBRB = 2
} PWM_STO_CTRL_T;

/******************************************************************************
	�Լ� ���� (Function Declaration)
******************************************************************************/
extern DTV_STATUS_T	DDI_PWM_Init ( DTV_STATUS_T (*pfnSetFrcScanningBL)(UINT8 portNum, UINT8 duty));
extern DTV_STATUS_T	DDI_PWM_UnInit (void);
extern DTV_STATUS_T DDI_PWM_SetLEDCurrent(BOOLEAN bEnable);
extern DTV_STATUS_T DDI_PWM_SetLEDCurrentForStoreMode(UINT8 u8Offset);
extern DTV_STATUS_T DDI_PWM_PostInit(void);

//extern DTV_STATUS_T	DDI_PWM_SetPulseWidth (PWM_PIN_SEL_T portIndex, UINT16 data);
//extern DTV_STATUS_T	DDI_PWM_SetFrequency (PWM_PIN_SEL_T portIndex, UINT32 data);
//extern DTV_STATUS_T	DDI_PWM_SetDutyCycle (PWM_PIN_SEL_T portIndex, UINT16 data);
//extern DTV_STATUS_T	DDI_PWM_SetEnable (PWM_PIN_SEL_T portIndex, BOOLEAN data);
extern UINT16	DDI_PWM_GetVBRPortA (void);
extern UINT16	DDI_PWM_GetVBRPortB (void);
extern DTV_STATUS_T	DDI_PWM_SetVBRPortA (UINT16 data,PWM_DUTY_CHANGE_T dutyChange);
extern DTV_STATUS_T	DDI_PWM_SetVBRPortB (UINT16 data,PWM_DUTY_CHANGE_T dutyChange);
extern DTV_STATUS_T	DDI_PWM_SetPwmOnoff (BOOLEAN onoff);
extern DTV_STATUS_T	DDI_PWM_SetVBRFreqPortB( UINT16 freq );
extern DTV_STATUS_T DDI_PWM_ChangeVBRFreqPortB(UINT16 frameRate);
extern void		DDI_PWM_SetPwmFreqMode(UINT32 pwmUid);
extern BOOLEAN	DDI_PWM_IsVariableFreqMode(void);
extern void		DDI_PWM_SetPwmVsyncMode(UINT32 pwmUid);
extern BOOLEAN	DDI_PWM_IsPwmVsyncMode(void);

//extern PWM_STO_CTRL_T DDI_PWM_GetStoreControlMethord(void);

extern void DDI_PWM_DEBUG (void);
extern UINT8 DDI_PWM_GetPWM_LEDCurrent(void);

#endif  /* _PWM_DDI_H_ */
