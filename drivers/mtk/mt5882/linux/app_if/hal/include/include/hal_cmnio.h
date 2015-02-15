/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_cmnio.h
 *
 *	This header file defines the driver functions related to i2c Driver
 *
 *	@author 	dhjung (dhjung77@lge.com)
 *	@version	0.9
 *	@date		2007.02.12
 *	@note		2007.04.28 modified by earnest for chaplin Chip
 *	@see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _CMNIO_DDI_H_
#define _CMNIO_DDI_H_

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
//#include <osa_api.h>
//#include <osa_kadp.h>
#include "hal_common.h"

/******************************************************************************
 	상수 정의(Constant Definitions)
******************************************************************************/

#define I2C_OK				 0
#define I2C_ERROR			-1


/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/


/******************************************************************************
	형 정의 (Type Definitions)
******************************************************************************/

/* CMNIO I2C */

/**
 * type definition about i2c speed
 *
*/
typedef enum
{
	I2C_MODE_NORMAL		= 	0,		/**< normal speed	: 100K	*/
	I2C_MODE_FAST		=	1,		/**< fast speed 	: 400K	*/
	I2C_MODE_SLOW		=	2,		/**< slow speed 	:  50K	*/
	I2C_MODE_DOWNLOAD1	= 	3,			/**< download speed 	: 700K	*/
	I2C_MODE_DOWNLOAD2	= 	4,			/**< download speed 	: 800K	*/
}	I2C_MODE_T;

/* CMIO GPIO */
/**
 * type definition about gpio group
 *
*/
typedef enum
{
	GPIO_GROUP_GPIO = 1,	/**< gpio group */
	GPIO_GROUP_GPI,			/**< gpi group */
	GPIO_GROUP_GPO			/**< gpo group */

}	GPIO_GROUP_T;

/**
* type definition about gpio high low
*
*/
typedef enum
{
	GPIO_LOW  = 0,			/**< gpio low */
	GPIO_HIGH 				/**< gpio high */
} GPIO_VALUE_T;

/* CMNIO PWM */

/**
* type definition about PWM device's index
*
*/
typedef enum {
	PWM_DEV_PIN0	= 0,	/**< PWM# 0 */
	PWM_DEV_PIN1,			/**< PWM# 1 */
	PWM_DEV_PIN2,			/**< PWM# 2 */
	PWM_DEV_PIN3,			/**< PWM# 3 */
	PWM_DEV_PIN4,			/**< PWM# 4 */
	PWM_DEV_MAX,			/**< PWM# MAX */
	PWM_DEV_NONE	= 0xFF,	/**< PWM# Not used */
} PWM_PIN_SEL_T;

/**
* type definition about PWM device's index mask
*
*/
typedef enum {
	PWM_DEV_PIN_0_MASK		= 1 << PWM_DEV_PIN0,	/**< PWM# 0 */
	PWM_DEV_PIN_1_MASK		= 1 << PWM_DEV_PIN1,	/**< PWM# 1 */
	PWM_DEV_PIN_2_MASK		= 1 << PWM_DEV_PIN2,	/**< PWM# 2 */
	PWM_DEV_PIN_3_MASK		= 1 << PWM_DEV_PIN3,	/**< PWM# 3 */
	PWM_DEV_PIN_4_MASK		= 1 << PWM_DEV_PIN4,	/**< PWM# 4 */

	PWM_DEV_PIN_0_1_MASK	= PWM_DEV_PIN_0_MASK|PWM_DEV_PIN_1_MASK,	/**< PWM# 0/1 */
	PWM_DEV_PIN_0_2_MASK	= PWM_DEV_PIN_0_MASK|PWM_DEV_PIN_2_MASK,	/**< PWM# 0/2 */
	PWM_DEV_PIN_1_2_MASK	= PWM_DEV_PIN_1_MASK|PWM_DEV_PIN_2_MASK,	/**< PWM# 1/2 */

	/* If necessary, add case */

	PWM_DEV_PIN_ALL_MASK	=  PWM_DEV_PIN_0_MASK|PWM_DEV_PIN_1_MASK|PWM_DEV_PIN_2_MASK \
									|PWM_DEV_PIN_3_MASK|PWM_DEV_PIN_4_MASK,	/**< PWM# 0/1/2/3/4 */

	PWM_DEV_PIN_DEFAULT_MASK		/**< depended on pwmIndex setting of HAL_PWM_SetParam function */
} PWM_PIN_SEL_MASK_T;

/**
 * PWM Output Frequency
 */
typedef struct {
	UINT32			pwmfreq_48nHz;		// PWM frequency 48xN Hz from DB table
	UINT32			pwmfreq_50nHz;		// PWM frequency 50xN Hz from DB table
	UINT32			pwmfreq_60nHz;		// PWM frequency 60xN Hz from DB table
} PWM_FREQ_DB_T;

/**
 * PWM Frequency Parameter
 */
typedef struct {
	UINT32			pwm_adapt_freq_enable;
	UINT32			pwmfreq_48nHz;		// PWM frequency 48xN Hz from DB table
	UINT32			pwmfreq_50nHz;		// PWM frequency 50xN Hz from DB table
	UINT32			pwmfreq_60nHz;		// PWM frequency 60xN Hz from DB table
//	PWM_FREQ_DB_T	pwm_adpat_freq_db;
} PWM_ADAPT_FREQ_PARAM_T;

/**
 * PWM Scanning Parameter
 */
#ifndef MTK_BAD
	typedef struct {
		BOOLEAN scan_Auto; 
		UINT8 scan_Chg; 
		UINT8 scan_Double; 
		SINT32 scan_Start; 
		UINT32 scan_Top; 
		UINT32 scan_Bottom;
	} PWM_SCANNING_PARAM_T;
#else
	typedef struct {
		UINT32	pwm_scanning_enable;
		UINT32	pwm_pos_start;
	} PWM_SCANNING_PARAM_T;
#endif

/**
 * PWM Setting Parameter
 */
typedef struct {
	UINT32					pwm_enable;
	UINT32					pwm_duty;
	UINT32					pwm_frequency; // If pwm_adapt_freq_enable == TRUE, ignored
	PWM_ADAPT_FREQ_PARAM_T	pwm_adapt_freq_param;
	UINT32					pwm_lock;
	UINT32					pwm_pos_start;
	UINT32					pwm_scanning_enable;
} PWM_PARAM_T;

/******************************************************************************
	함수 선언 (Function Declaration)
******************************************************************************/
/* CMNIO I2C */
extern DTV_STATUS_T HAL_I2C_Init (void);
extern DTV_STATUS_T HAL_I2C_Read (UINT8 ChNum, I2C_MODE_T transMode, UINT8 slaveAddr, UINT32 subAddrMode,
									   UINT8 *subAddr, UINT16 nBytes, UINT8 *rxBuf, UINT32 retry);
extern DTV_STATUS_T HAL_I2C_Write(UINT8 ChNum, I2C_MODE_T transMode, UINT8 slaveAddr, UINT32 subAddrMode,
								 	   UINT8 *subAddr, UINT16 nBytes, UINT8 *txBuf, UINT32 retry);

/* CMNIO GPIO */
extern DTV_STATUS_T	HAL_GPIO_Init (UINT8 boardType);
extern DTV_STATUS_T	HAL_GPIO_InitEx( UINT8 boardType, UINT8 resolution );
extern DTV_STATUS_T	HAL_GPIO_SetPortDirection(UINT32 portIndex, BOOLEAN direction);
extern DTV_STATUS_T	HAL_GPIO_SetPortDirection_SP(UINT32 portIndex, BOOLEAN direction); //sungwoo.han 1234
extern DTV_STATUS_T	HAL_GPIO_SetOutputPort(UINT32 portIndex, UINT8 data);


#if 1 /* Jason Lim, for fast I2C */
extern DTV_STATUS_T HAL_GPIO_I2C_ReadWriteByte(UINT32 portIndex_sda, UINT32 portIndex_scl, BOOLEAN bWrite, UINT8 *pData);
#endif


extern DTV_STATUS_T	HAL_GPIO_GetInputPort(UINT32 portIndex, UINT8 *pData);
extern DTV_STATUS_T	HAL_GPIO_SetOutputPortArray(UINT32 numArray, UINT32 portArray[], UINT8 dataArray[]);
extern DTV_STATUS_T	HAL_GPIO_GetInputPortArray(UINT32 numArray, UINT32 portArray[], UINT8 dataArray[]);
extern DTV_STATUS_T	HAL_GPIO_SetIntrCallBack(UINT32 portIndex, void (*pfnGPIO_CB)(UINT32), UINT32 enable);

/* CMNIO PWM */
extern DTV_STATUS_T HAL_PWM_Init (void);
extern DTV_STATUS_T HAL_PWM_SetAdaptFreqCtrl (PWM_PIN_SEL_T pwmIndex, BOOLEAN onOff, PWM_FREQ_DB_T pwmFreqDb);
extern DTV_STATUS_T HAL_PWM_SetFrequency(PWM_PIN_SEL_T pwmIndex, UINT32 data);
extern DTV_STATUS_T HAL_PWM_SetDutyCycle (PWM_PIN_SEL_T pwmIndex, UINT32 data);
extern DTV_STATUS_T HAL_PWM_SetEnable (PWM_PIN_SEL_T pwmIndex, BOOLEAN data);
extern DTV_STATUS_T HAL_PWM_SetPulseWidth (PWM_PIN_SEL_T pwmIndex, UINT32 data);
extern DTV_STATUS_T HAL_PWM_SetVsyncAlign(PWM_PIN_SEL_T pwmIndex, BOOLEAN data);

// NC5 new interface
extern DTV_STATUS_T HAL_PWM_SetParam(PWM_PIN_SEL_T pwmIndex, PWM_PARAM_T* pstPWMParam);
extern DTV_STATUS_T HAL_PWM_GetParam(PWM_PIN_SEL_T pwmIndex, PWM_PARAM_T* pstPWMParam);
extern DTV_STATUS_T HAL_PWM_ApplyParamSet(PWM_PIN_SEL_MASK_T pwmIndex_mask);





#endif /* _CMNIO_DDI_H_ */
