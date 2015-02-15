/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file hal_gpio_index.h
 *
 *	This header file defines the driver functions related to gpio index
 *
 *	@author 	kwangseok.kim (kwangseok.kim@lge.com)
 *	@version	0.1
 *	@date		2013.05.27
 *	@see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _HAL_GPIO_INDEX_H_
#define _HAL_GPIO_INDEX_H_

/******************************************************************************
	#include ¢®¡×¡Ë¢çAAI¢®Iie (File Inclusions)
******************************************************************************/
#include "hal_common.h"


/******************************************************************************
 	¡Ë¢çio¢®¡×uo A¢®E¡Ë¡ÍAC(Constant Definitions)
******************************************************************************/
/** GPIO Group Index */
#define	_GPIO_GROUP_CMNIO				1
#define	_GPIO_GROUP_MICOM				2
#define	_GPIO_GROUP_IOEXP				3
#define	_GPIO_GROUP_MAX 				4
#define _GPIO_GROUP_SHIFT				12
#define _GPIO_GROUP_SET( x )			( ((x) & 0xF) << _GPIO_GROUP_SHIFT )
#define	GPIO_GROUP_GET( x ) 			( ((x) >> _GPIO_GROUP_SHIFT) & 0xF )

#define GPIO_GROUP_CMNIO				_GPIO_GROUP_SET( _GPIO_GROUP_CMNIO )
#define GPIO_GROUP_MICOM				_GPIO_GROUP_SET( _GPIO_GROUP_MICOM )
#define GPIO_GROUP_IOEXP				_GPIO_GROUP_SET( _GPIO_GROUP_IOEXP )

/** GPIO Type(Direction) */
#define _GPIO_TYPE_WR					1
#define _GPIO_TYPE_RD					2
#define _GPIO_TYPE_RW					(_GPIO_TYPE_WR | _GPIO_TYPE_RD)
#define _GPIO_TYPE_SHIFT				8
#define _GPIO_TYPE_SET( x )				( ((x) & 0xF) << _GPIO_TYPE_SHIFT )

#define GPIO_TYPE_WR					_GPIO_TYPE_SET( _GPIO_TYPE_WR )
#define GPIO_TYPE_RD					_GPIO_TYPE_SET( _GPIO_TYPE_RD )
#define GPIO_TYPE_RW					_GPIO_TYPE_SET( _GPIO_TYPE_RW )

/** GPIO Index Type */
#define GPIO_CMNIO_WR  					(GPIO_GROUP_CMNIO + GPIO_TYPE_WR)
#define GPIO_CMNIO_RD  					(GPIO_GROUP_CMNIO + GPIO_TYPE_RD)
#define GPIO_CMNIO_RW  					(GPIO_GROUP_CMNIO + GPIO_TYPE_RW)
#define GPIO_MICOM_WR					(GPIO_GROUP_MICOM + GPIO_TYPE_WR)
#define GPIO_MICOM_RD					(GPIO_GROUP_MICOM + GPIO_TYPE_RD)
#define GPIO_MICOM_RW					(GPIO_GROUP_MICOM + GPIO_TYPE_RW)
#define GPIO_IOEXP_WR					(GPIO_GROUP_IOEXP + GPIO_TYPE_WR)
#define GPIO_IOEXP_RD					(GPIO_GROUP_IOEXP + GPIO_TYPE_RD)
#define GPIO_IOEXP_RW					(GPIO_GROUP_IOEXP + GPIO_TYPE_RW)

/** GPIO Write Group */
#define GPIO_PORT_WR_FRC_RESET			( GPIO_CMNIO_WR +  0 )
#define GPIO_PORT_WR_TUNER_RESET		( GPIO_CMNIO_WR +  1 )
#define GPIO_PORT_WR_HDMI_RESET			( GPIO_CMNIO_WR +  2 )
#define GPIO_PORT_WR_STUNER_RESET		( GPIO_CMNIO_WR +  3 )
#define GPIO_PORT_WR_TUNER_RESET2		( GPIO_CMNIO_WR +  4 )
#define GPIO_PORT_WR_DEMOD_RESET		( GPIO_CMNIO_WR +  5 )
#define GPIO_PORT_WR_EXT_RF_SWITCH		( GPIO_CMNIO_WR +  6 )  // RF SWITCH
#define GPIO_PORT_WR_M_RFMODULE_RESET	( GPIO_CMNIO_WR +  7 )
#define GPIO_PORT_WR_USB1_CTRL			( GPIO_CMNIO_WR +  8 )  // USB1 ctrl
#define GPIO_PORT_WR_USB2_CTRL			( GPIO_CMNIO_WR +  9 )  // USB2 ctrl
#define GPIO_PORT_WR_USB3_CTRL			( GPIO_CMNIO_WR + 10 )  // USB3 ctrl
#define GPIO_PORT_WR_RST_HUB			( GPIO_CMNIO_WR + 11 )	// USB HUB Reset
#define GPIO_PORT_WR_AUDIO_RESET		( GPIO_CMNIO_WR + 12 )
#define GPIO_PORT_WR_PMIC_RESET			( GPIO_CMNIO_WR + 13 )
#define GPIO_PORT_WR_FE_LNA_CTRL_1		( GPIO_CMNIO_WR + 14 )
#define GPIO_PORT_WR_FE_LNA_CTRL_2		( GPIO_CMNIO_WR + 15 )
#define GPIO_PORT_WR_2D_3D_CTRL			( GPIO_CMNIO_WR + 16 )
#define GPIO_PORT_WR_OPC_EN 			( GPIO_CMNIO_WR + 17 )
#define GPIO_PORT_WR_VCOM_DYN_CTRL		( GPIO_CMNIO_WR + 18 )
#define GPIO_PORT_WR_D13_RESET			( GPIO_CMNIO_WR + 19 )
#define GPIO_PORT_WR_AMP_RESET_N_1		( GPIO_CMNIO_WR + 20 )
#define GPIO_PORT_WR_AMP_RESET_N		( GPIO_CMNIO_WR + 21 )
#define GPIO_PORT_WR_FRC_FLASH_WP		( GPIO_CMNIO_WR + 22 )
#define GPIO_PORT_WR_SIL_RESET			( GPIO_CMNIO_WR + 23 )
#define GPIO_PORT_WR_PHY_RESET			( GPIO_CMNIO_WR + 24 )
#define GPIO_PORT_WR_HDMI_HPD_1			( GPIO_CMNIO_WR + 25 )
#define GPIO_PORT_WR_HDMI_HPD_2			( GPIO_CMNIO_WR + 26 )
#define GPIO_PORT_WR_HDMI_HPD_3			( GPIO_CMNIO_WR + 27 )
#define GPIO_PORT_WR_HDMI_HPD_4			( GPIO_CMNIO_WR + 28 )
#define GPIO_PORT_WR_HDMI_SPLITTER_TEST	( GPIO_CMNIO_WR + 29 )
#define GPIO_PORT_WR_HDMI_MUX_SEL		( GPIO_CMNIO_WR + 30 )
#define GPIO_PORT_WR_PA168_RESET		( GPIO_CMNIO_WR + 31 )
#define GPIO_PORT_WR_IR_B_RESET			( GPIO_CMNIO_WR + 32 )
#define GPIO_PORT_WR_U14_RESET			( GPIO_CMNIO_WR + 33 )
#define GPIO_PORT_WR_URSA_RESET			( GPIO_CMNIO_WR + 34 )
#define GPIO_PORT_WR_R9531_RESET		( GPIO_CMNIO_WR + 35 )
#define GPIO_PORT_WR_D14_RESET			( GPIO_CMNIO_WR + 36 )
#define GPIO_PORT_WR_STP_MUX_IN1		( GPIO_CMNIO_WR + 37 )
#define GPIO_PORT_WR_STP_MUX_IN2		( GPIO_CMNIO_WR + 38 )
#define GPIO_PORT_WR_DCP_CTL 			( GPIO_CMNIO_WR + 39 )
#define GPIO_PORT_WR_R9531_WP 			( GPIO_CMNIO_WR + 40 )

/** Add logical GPIO pin for H15 / LM15U **/
#define GPIO_PORT_WR_RETENTION_DISABLE	( GPIO_CMNIO_WR + 41 )
#define GPIO_PORT_WR_LOCAL_DIM_EN		( GPIO_CMNIO_WR + 42 )
#define GPIO_PORT_WR_BT_RESET			( GPIO_CMNIO_WR + 43 )
#define GPIO_PORT_WR_DEBUG				( GPIO_CMNIO_WR + 44 )
#define GPIO_PORT_WR_TCON_I2C_EN		( GPIO_CMNIO_WR + 45 )
#define GPIO_PORT_WR_SIL9679_RESET		( GPIO_CMNIO_WR + 46 )
#define GPIO_PORT_WR_HDMI_SW_SWITCH		( GPIO_CMNIO_WR + 47 )
#define GPIO_PORT_WR_WL_MUTE			( GPIO_CMNIO_WR + 48 )
#define GPIO_PORT_WR_WL_RESET			( GPIO_CMNIO_WR + 49 )
#define GPIO_PORT_WR_LOCAL_DIMMING_EN	( GPIO_CMNIO_WR + 50 )
#define GPIO_PORT_WR_MN864778_RESET		( GPIO_CMNIO_WR + 51 )
#define GPIO_PORT_WR_PCM_5V_CTL			( GPIO_CMNIO_WR + 52 )
#define GPIO_PORT_WR_3D_EN				( GPIO_CMNIO_WR + 53 )
#define GPIO_PORT_WR_OTP_WRITE			( GPIO_CMNIO_WR + 54 )

#define GPIO_PORT_WR_CI_ENABLE			( GPIO_CMNIO_WR + 55 )
#define GPIO_PORT_WR_CVBS_OUT_SEL		( GPIO_CMNIO_WR + 56 )
#define GPIO_PORT_WR_URSA_RESET_SOC		( GPIO_CMNIO_WR + 57 )
#define GPIO_PORT_WR_PCM_RESET			( GPIO_CMNIO_WR + 58 )
#define GPIO_PORT_WR_SIL9679_INT		( GPIO_CMNIO_WR + 59 )

/** GPIO Read Group */
#define GPIO_PORT_RD_MODEL_OPTION_0		( GPIO_CMNIO_RD +  0 )
#define GPIO_PORT_RD_MODEL_OPTION_1		( GPIO_CMNIO_RD +  1 )
#define GPIO_PORT_RD_MODEL_OPTION_2		( GPIO_CMNIO_RD +  2 )
#define GPIO_PORT_RD_MODEL_OPTION_3		( GPIO_CMNIO_RD +  3 )
#define GPIO_PORT_RD_MODEL_OPTION_4		( GPIO_CMNIO_RD +  4 )
#define GPIO_PORT_RD_MODEL_OPTION_5		( GPIO_CMNIO_RD +  5 )
#define GPIO_PORT_RD_MODEL_OPTION_6		( GPIO_CMNIO_RD +  6 )
#define GPIO_PORT_RD_MODEL_OPTION_7		( GPIO_CMNIO_RD +  7 )
#define GPIO_PORT_RD_MODEL_OPTION_8		( GPIO_CMNIO_RD +  8 )
#define GPIO_PORT_RD_MODEL_OPTION_9		( GPIO_CMNIO_RD +  9 )
#define GPIO_PORT_RD_MODEL_OPTION_10	( GPIO_CMNIO_RD + 10 )
#define GPIO_PORT_RD_COMP1_LINK			( GPIO_CMNIO_RD + 11 )
#define GPIO_PORT_RD_CVBS1_LINK			( GPIO_CMNIO_RD + 12 )
#define GPIO_PORT_RD_SCART1_LINK 		( GPIO_CMNIO_RD + 13 )
#define GPIO_PORT_RD_HP_LINK			( GPIO_CMNIO_RD + 14 )
#define GPIO_PORT_RD_ERROR_OUT			( GPIO_CMNIO_RD + 15 )
#define GPIO_PORT_RD_USB1_OCD			( GPIO_CMNIO_RD + 16 )
#define GPIO_PORT_RD_USB2_OCD			( GPIO_CMNIO_RD + 17 )
#define GPIO_PORT_RD_USB3_OCD			( GPIO_CMNIO_RD + 18 )
#define GPIO_PORT_RD_CAM_SLIDE_DET		( GPIO_CMNIO_RD + 19 )
#define GPIO_PORT_RD_OLED_EXT_COMP_DONE	( GPIO_CMNIO_RD + 20 )
#define GPIO_PORT_RD_INTR_VOICE_DET_CB	( GPIO_CMNIO_RD + 21 )  // register callback funciton. (special case)
#define GPIO_PORT_RD_PA168_DONE			( GPIO_CMNIO_RD + 22 )
#define GPIO_PORT_RD_INTR_U14_H13_CON	( GPIO_CMNIO_RD + 23 )

/** Add logical GPIO pin for H15 / LM15U **/
#define GPIO_PORT_RD_MODEL_OPTION_11	( GPIO_CMNIO_RD + 24 )
#define GPIO_PORT_RD_INSTANT_BOOT		( GPIO_CMNIO_RD + 25 )
#define GPIO_PORT_RD_VX1_LOCKN			( GPIO_CMNIO_RD + 26 )
#define GPIO_PORT_RD_SIL9679_INT		( GPIO_CMNIO_RD + 27 )
#define GPIO_PORT_RD_DDR_VENDOR_OPTION1	( GPIO_CMNIO_RD + 28 )
#define GPIO_PORT_RD_DDR_VENDOR_OPTION2	( GPIO_CMNIO_RD + 29 )
#define GPIO_PORT_RD_FAN_TACH			( GPIO_CMNIO_RD + 30 )
#define GPIO_PORT_RD_HDMI1_5V_DET		( GPIO_CMNIO_RD + 31 )
#define GPIO_PORT_RD_HDMI2_5V_DET		( GPIO_CMNIO_RD + 32 )
#define GPIO_PORT_RD_HDMI3_5V_DET		( GPIO_CMNIO_RD + 33 )
#define GPIO_PORT_RD_HDMI4_5V_DET		( GPIO_CMNIO_RD + 34 )
#define GPIO_PORT_RD_MHL_DET			( GPIO_CMNIO_RD + 35 )
#define GPIO_PORT_RD_MHL_OCP			( GPIO_CMNIO_RD + 36 )
#define GPIO_PORT_RD_CVBS2_LINK			( GPIO_CMNIO_RD + 37 )
#define GPIO_PORT_RD_WOL_WAKE_UP		( GPIO_CMNIO_RD + 38 )
#define GPIO_PORT_RD_URSA9_CONNECT		( GPIO_CMNIO_RD + 39 )
#define GPIO_PORT_RD_CAM_CD1_N			( GPIO_CMNIO_RD + 40 )
#define GPIO_PORT_RD_MODEL_OPTION_12		( GPIO_CMNIO_RD + 41 )
#define GPIO_PORT_RD_MODEL_OPTION_13		( GPIO_CMNIO_RD + 42 )
#define GPIO_PORT_RD_MODEL_OPTION_14		( GPIO_CMNIO_RD + 43 )
#define GPIO_PORT_RD_MODEL_OPTION_15		( GPIO_CMNIO_RD + 44 )
#define GPIO_PORT_RD_MODEL_OPTION_16		( GPIO_CMNIO_RD + 45 )

#define GPIO_OUTPUT_PUSHPULL			0
#define GPIO_OUTPUT_OPENDRAIN			1

#define REMOVE_EXCEPT_FOR_HAL_GPIO_INDEX

#define RESOLUTION_HD					0
#define RESOLUTION_FHD					1
#define RESOLUTION_UHD					2
#define RESOLUTION_MAX					3

/******************************************************************************
    ¢®E¢®©­AA¢®¡×I¡Ë¢ç¡Ë¢¥I CO¢®¡×uo A¢®E¡Ë¡ÍAC (Macro Definitions)
******************************************************************************/


/******************************************************************************
	Cu A¢®E¡Ë¡ÍAC (Type Definitions)
******************************************************************************/


/******************************************************************************
	CO¢®¡×uo ¢®¡×u¡Ë¢ç¡§u¢®¡×u¡§I¢®I (Function Declaration)
******************************************************************************/



#endif /* _HAL_GPIO_INDEX_H_ */
