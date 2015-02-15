/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file gpio_index.h
 *
 *	This header file defines the DDI functions related to Flexbus Driver
 *
 *	@author 	dhjung (dhjung77@lge.com)
 *	@version	0.1
 *	@date		2008.02.12
 *	@see
 */


/******************************************************************************
	Header File Guarder
******************************************************************************/
#ifndef _CMNIO_GPIO_INDEX_H_
#define _CMNIO_GPIO_INDEX_H_

/******************************************************************************
	전역 제어 상수(Control Constants)
******************************************************************************/

/******************************************************************************
	#include 파일들 (File Inclusions)
******************************************************************************/
#include "gpio_ddi.h"

//#define GPIO_IO_EXP_MAX_NUM		0

/******************************************************************************
	상수 정의(Constant Definitions)
******************************************************************************/

/** GPIO Port Index  */
/** GPIO Group : GPIO0 ~ GPIO159 */
/** GPIO Write Group */
#define GPIO_PORT_WR_CI_ENABLE			( GPIO_CMNIO_WR +  0 )
#define GPIO_PORT_WR_FRC_RESET			( GPIO_CMNIO_WR +  1 )
#define GPIO_PORT_WR_9V_ON  			( GPIO_CMNIO_WR +  2 )
#define GPIO_PORT_WR_8V_ON  			( GPIO_CMNIO_WR +  3 )
#define GPIO_PORT_WR_FLASH_WP			( GPIO_CMNIO_WR +  4 )
#define GPIO_PORT_WR_TUNER_RESET		( GPIO_CMNIO_WR +  5 )
#define GPIO_PORT_WR_REC_8_CTRL_1		( GPIO_CMNIO_WR +  6 )
#define GPIO_PORT_WR_REC_8_CTRL_2		( GPIO_CMNIO_WR +  7 )
#define GPIO_PORT_WR_FRC_EN_1_3PLL 		( GPIO_CMNIO_WR +  8 )
#define GPIO_PORT_WR_AUDIO_RESET		( GPIO_CMNIO_WR +  9 )
#define GPIO_PORT_WR_HDMI_SW_EQ			( GPIO_CMNIO_WR + 10 )
#define GPIO_PORT_WR_SEL_HD2812_CTRL		( GPIO_CMNIO_WR + 11 )
#define GPIO_PORT_WR_SEL_LGS8G13_CTRL		( GPIO_CMNIO_WR + 12 )
#define GPIO_PORT_WR_AGC_SPEED_CTRL		( GPIO_CMNIO_WR + 13 )
#define GPIO_PORT_WR_BOOSTER_CTRL		( GPIO_CMNIO_WR + 14 )
#define GPIO_PORT_WR_LCD_BITSEL			( GPIO_CMNIO_WR + 15 )
#define GPIO_PORT_WR_FRC_EN_1_8VF 		( GPIO_CMNIO_WR + 16 )
#define GPIO_PORT_WR_FRC_EN_3_3VF 		( GPIO_CMNIO_WR + 17 )
#define GPIO_PORT_WR_DTVMNT_SW			( GPIO_CMNIO_WR + 18 )
#define GPIO_PORT_WR_FRC_EN_1_3VF 		( GPIO_CMNIO_WR + 19 )
#define GPIO_PORT_WR_FRC_EN_2_5VF 		( GPIO_CMNIO_WR + 20 )
#define GPIO_PORT_WR_EYEQ_RESET			( GPIO_CMNIO_WR + 21 )
#define GPIO_PORT_WR_HDMI_SW_P0			( GPIO_CMNIO_WR + 22 )
#define GPIO_PORT_WR_HDMI_SW_P1			( GPIO_CMNIO_WR + 23 )
#define GPIO_PORT_WR_HDMI_SW_P2			( GPIO_CMNIO_WR + 24 )
#define GPIO_PORT_WR_HP_DET_SW_1		( GPIO_CMNIO_WR + 25 )
#define GPIO_PORT_WR_HP_DET_SW_2		( GPIO_CMNIO_WR + 26 )
#define GPIO_PORT_WR_HP_DET_SW_3		( GPIO_CMNIO_WR + 27 )
#define GPIO_PORT_WR_HP_DET_SW_4		( GPIO_CMNIO_WR + 28 )
#define GPIO_PORT_WR_HDMI_RESET			( GPIO_CMNIO_WR + 29 )
#define GPIO_PORT_WR_GPIOI2C_SDAWR		( GPIO_CMNIO_WR + 30 )
#define GPIO_PORT_WR_GPIOI2C_SCLWR		( GPIO_CMNIO_WR + 31 )
#define GPIO_PORT_WR_DEMOD_RESET		( GPIO_CMNIO_WR + 32 )
#define GPIO_PORT_WR_CHB_RESET			( GPIO_CMNIO_WR + 33 )
#define GPIO_PORT_WR_TW9910_RESET		( GPIO_CMNIO_WR + 34 )
#define GPIO_PORT_WR_CHB_TUNER_RESET		( GPIO_CMNIO_WR + 35 )
#define GPIO_PORT_WR_L_DIM_RESET		( GPIO_CMNIO_WR + 36 )
#define GPIO_PORT_WR_M_RFMODULE_RESET		( GPIO_CMNIO_WR + 37 )
#define GPIO_PORT_WR_RF_SWITCH_CTL_CHB		( GPIO_CMNIO_WR + 38 )
#define GPIO_PORT_WR_BT_ONOFF			( GPIO_CMNIO_WR + 39 )	// BT_ONOFF
#define GPIO_PORT_WR_BT_RESET			( GPIO_CMNIO_WR + 40 )	// BT_RESET
#define GPIO_PORT_WR_BT_MUTE			( GPIO_CMNIO_WR + 41 )	// BT_MUTE
#define GPIO_PORT_WR_BT_VREGCTRL		( GPIO_CMNIO_WR + 42 )	// BT_VREGCTRL
#define GPIO_PORT_WR_RST_HUB			( GPIO_CMNIO_WR + 43 )	// USB HUB Reset
#define GPIO_PORT_WR_USB_CTRL			( GPIO_CMNIO_WR + 44 )  // USB ctrl
#define GPIO_PORT_WR_USB1_CTRL			( GPIO_CMNIO_WR + 45 )  // USB1 ctrl
#define GPIO_PORT_WR_USB2_CTRL			( GPIO_CMNIO_WR + 46 )  // USB2 ctrl
#define GPIO_PORT_WR_USB3_CTRL			( GPIO_CMNIO_WR + 47 )  // USB3 ctrl
#define GPIO_PORT_WR_EXT_LNA2_CTRL		( GPIO_CMNIO_WR + 48 )	// LNA2
#define GPIO_PORT_WR_EXT_RF_SWITCH		( GPIO_CMNIO_WR + 49 )  // RF SWITCH
#define GPIO_PORT_WR_WIRELESS_TX       	 	( GPIO_CMNIO_WR + 50 )
#define GPIO_PORT_WR_CI_SEL             	( GPIO_CMNIO_WR + 51 )	// /CI_SEL
#define GPIO_PORT_WR_CHB_BOOSTER_CTRL		( GPIO_CMNIO_WR + 52 )

#ifdef INCLUDE_TRIDTV
#define GPIO_PORT_WR_USB3_PWON			( GPIO_CMNIO_WR + 53 ) // 3D L/R Emitter USB Power Protect
#define GPIO_PORT_WR_USB3_PWERR			( GPIO_CMNIO_WR + 54 ) // 3D L/R Emitter USB Power error detect
#endif

#define GPIO_PORT_WR_FMTUP_NCS			( GPIO_CMNIO_WR + 55 )
#define GPIO_PORT_WR_FMTUP_DCLK			( GPIO_CMNIO_WR + 56 )
#define GPIO_PORT_WR_FMTUP_ASDI			( GPIO_CMNIO_WR + 57 )

#define GPIO_PORT_WR_3D_POWER_EN		( GPIO_CMNIO_WR + 58 )

#define GPIO_PORT_WR_OPT_P				( GPIO_CMNIO_WR	+ 59 )
#define GPIO_PORT_WR_OPT_N				( GPIO_CMNIO_WR + 60 )

#define GPIO_PORT_WR_RST_PHY			( GPIO_CMNIO_WR + 61 )
#define GPIO_PORT_WR_ARC_CTRL			( GPIO_CMNIO_WR + 62 )
#define GPIO_PORT_WR_OPTIC_CTRL			( GPIO_CMNIO_WR + 63 )

#define GPIO_PORT_WR_FE_TSOUT_BUF_CTL		( GPIO_CMNIO_WR + 64 )
#define GPIO_PORT_WR_FE_TSOUT_BUF_CTL2		( GPIO_CMNIO_WR + 65 )
#define GPIO_PORT_WR_L9_TSOUT_BUF_CTL		( GPIO_CMNIO_WR + 66 )
#define GPIO_PORT_WR_L9_TSOUT_BUF_CTL2		( GPIO_CMNIO_WR + 67 )

#define GPIO_PORT_WR_3D_DEPTH_RESET			( GPIO_CMNIO_WR + 68 )
#define GPIO_PORT_WR_3D_DEPTH_WP			( GPIO_CMNIO_WR + 69 )
#define GPIO_PORT_WR_FRC_WP					( GPIO_CMNIO_WR + 70 )

#define GPIO_PORT_WR_OPTIC_SERDES_RST		( GPIO_CMNIO_WR + 71 )
#define GPIO_PORT_WR_OLED_TCON_RESET		( GPIO_CMNIO_WR + 72 ) // OLED TCON RESET으로 변경 함 brian95.hong
#define GPIO_PORT_WR_OPTIC_FPGA_RST			( GPIO_CMNIO_WR + 73 )
#define GPIO_PORT_WR_OPTIC_FPGA_LVDS_INFO	( GPIO_CMNIO_WR + 74 )



#define GPIO_PORT_GPIOI2C_WR_3D_GPIO_0		( GPIO_CMNIO_WR + 75 )
#define GPIO_PORT_GPIOI2C_WR_3D_GPIO_1		( GPIO_CMNIO_WR + 76 )
#define GPIO_PORT_GPIOI2C_WR_3D_GPIO_2		( GPIO_CMNIO_WR + 77 )
#define GPIO_PORT_GPIOI2C_WR_3D_SYNC		( GPIO_CMNIO_WR + 78 )

#define GPIO_PORT_GPIO_LG8300_RESET			( GPIO_CMNIO_WR + 79 )
#define GPIO_PORT_WR_SW_MOTMOD_RS232		( GPIO_CMNIO_WR + 80 )

#define GPIO_PORT_WR_M_RFMODULE_DC			( GPIO_CMNIO_WR + 81 )
#define GPIO_PORT_WR_M_RFMODULE_DD			( GPIO_CMNIO_RW	+ 82 )
#define GPIO_PORT_WR_STUNER_RESET			( GPIO_CMNIO_WR + 83 )
#define GPIO_PORT_WR_EMMC_RESET				( GPIO_CMNIO_WR + 84 )

#define GPIO_PORT_WR_IR_B_RESET				( GPIO_CMNIO_WR + 85 )

#define GPIO_PORT_WR_SMARTCARD_CLK			( GPIO_CMNIO_WR + 86 )
#define GPIO_PORT_WR_SMARTCARD_PWR_SEL		( GPIO_CMNIO_WR + 87 )
#define GPIO_PORT_WR_SMARTCARD_RST			( GPIO_CMNIO_WR + 88 )
#define GPIO_PORT_WR_SMARTCARD_VCC			( GPIO_CMNIO_WR + 89 )
#define GPIO_PORT_WR_SMARTCARD_DATA			( GPIO_CMNIO_WR + 90 )

#define GPIO_PORT_WR_M_RFMODULE_ISP			( GPIO_CMNIO_WR	+ 91 )

#define GPIO_PORT_WR_TUNER_RESET2			( GPIO_CMNIO_WR + 92 )

#define GPIO_PORT_WR_PWM2_DISABLE			( GPIO_CMNIO_WR + 96 )
#define GPIO_PORT_WR_2D_3D_CTRL				( GPIO_CMNIO_WR + 97 )
#define GPIO_PORT_WR_LNB_CTRL				( GPIO_CMNIO_WR + 98 )
#define GPIO_PORT_WR_OPTIC_I2C_SWITCH		( GPIO_CMNIO_WR + 99 )

#define GPIO_PORT_WR_FE_LNA_CTRL_1			( GPIO_CMNIO_WR + 100 )
#define GPIO_PORT_WR_FE_LNA_CTRL_2			( GPIO_CMNIO_WR + 101 )
#define GPIO_PORT_WR_PMIC_RESET				( GPIO_CMNIO_WR + 102 )

#define GPIO_PORT_WR_OTP_WR_CTRL			( GPIO_CMNIO_WR + 103 )
#define GPIO_PORT_WR_SIL_RESET				( GPIO_CMNIO_WR + 104 )
#define GPIO_PORT_WR_OPC_EN 				( GPIO_CMNIO_WR + 105 )
#define GPIO_PORT_WR_LOCAL_DIMMING_EN		( GPIO_CMNIO_WR + 106 )

#define GPIO_PORT_WR_5V_ANT_CTRL		( 0 )	//TBD
#define GPIO_PORT_WR_VIDEO_SW1			( 0 )	// TBD
#define GPIO_PORT_WR_VIDEO_SW2			( 0 )	// TBD
#define	GPIO_PORT_WR_SC1_VIDEO_MUTE		( 0 )   // TBD

/** GPIO Read/Write Group : 향후 PDP를 위해서 필요*/
#define GPIO_PORT_RW_DSUB_SDA			( GPIO_CMNIO_RW + 0x01 )	//pin num 053	--> 0x1335
#define GPIO_PORT_RW_DSUB_SCL			( GPIO_CMNIO_RW + 0x02 )	//pin num 054	--> 0x1336
#define GPIO_PORT_RW_MODULE_SCLK 		( GPIO_CMNIO_RW + 0x03 )	//pin num 134	--> 0x1386
#define GPIO_PORT_RW_MODULE_SDATA		( GPIO_CMNIO_RW + 0x04 )	//pin num 135	--> 0x1387
#define GPIO_PORT_RW_RF_SW			( GPIO_CMNIO_RW + 0x05 )	//pin num 036-- > 0x1324

/** GPIO Read Group */
//	GPIO_CMNIO_RD	:	GPI's GPIO_PORT definition
//	BOARD's OPTION
#define GPIO_PORT_RD_MODEL_OPTION_0		( GPIO_CMNIO_RD +  0 )	//	0:NO-FRC, 1:Internal FRC
#define GPIO_PORT_RD_MODEL_OPTION_1		( GPIO_CMNIO_RD +  1 )	//	2:LG-FRC2, 3:External URSA5
#define GPIO_PORT_RD_MODEL_OPTION_2		( GPIO_CMNIO_RD +  2 )	//	HD		/ 	FHD
#define GPIO_PORT_RD_MODEL_OPTION_3		( GPIO_CMNIO_RD +  3 )	//	Optic     	/	NONE
#define GPIO_PORT_RD_MODEL_OPTION_4		( GPIO_CMNIO_RD +  4 )	//	3D_Depth IC / NONE
#define GPIO_PORT_RD_MODEL_OPTION_5		( GPIO_CMNIO_RD +  5 )	//	DDR 768MB / Default
#define GPIO_PORT_RD_MODEL_OPTION_6		( GPIO_CMNIO_RD +  6 )	//	CP-BOX / NONE
#define GPIO_PORT_RD_MODEL_OPTION_7		( GPIO_CMNIO_RD +  7 )	//	T2 Tuner / NONE
#define GPIO_PORT_RD_MODEL_OPTION_8		( GPIO_CMNIO_RD +  8 )	//	S Tuner / NONE
#define GPIO_PORT_RD_MODEL_OPTION_9		( GPIO_CMNIO_RD +  9 )	//	Reserved
#define GPIO_PORT_RD_MODEL_OPTION_10		( GPIO_CMNIO_RD + 10 )	//	EPI / NONE

#define GPIO_PORT_RD_AV1S_SW			( GPIO_CMNIO_RD + 15 )
#define GPIO_PORT_RD_HDMI1_5V_DET		( GPIO_CMNIO_RD + 16 )
#define GPIO_PORT_RD_HDMI2_5V_DET		( GPIO_CMNIO_RD + 17 )
#define GPIO_PORT_RD_HDMI3_5V_DET		( GPIO_CMNIO_RD + 18 )
#define GPIO_PORT_RD_HDMI4_5V_DET		( GPIO_CMNIO_RD + 19 )
#define GPIO_PORT_RD_COMP1_LINK			( GPIO_CMNIO_RD + 20 )
#define GPIO_PORT_RD_COMP2_LINK			( GPIO_CMNIO_RD + 21 )
#define GPIO_PORT_RD_CVBS1_LINK			( GPIO_CMNIO_RD + 22 )
#define GPIO_PORT_RD_SVHS_LINK			( GPIO_CMNIO_RD + 23 )
#define GPIO_PORT_RD_RGB1_LINK			( GPIO_CMNIO_RD + 24 )

#define GPIO_PORT_RD_5V_ANT_MNT			( GPIO_CMNIO_RD + 25 )
#define GPIO_PORT_RD_HARDWARE_OPTION_1  	( GPIO_CMNIO_RD + 26 )
#define GPIO_PORT_RD_FAN_ALARM			( GPIO_CMNIO_RD + 27 )
#define GPIO_PORT_RD_SCART1_LINK 		( GPIO_CMNIO_RD + 28 )
#define GPIO_PORT_RD_SCART2_LINK 		( GPIO_CMNIO_RD + 29 )
#define GPIO_PORT_RD_COMP_LINK			( GPIO_CMNIO_RD + 30 )
#define GPIO_PORT_RD_CVBS2_LINK			( GPIO_CMNIO_RD + 31 )
#define GPIO_PORT_RD_GPIOI2C_SDARD		( GPIO_CMNIO_RD + 32 )
#define GPIO_PORT_RD_HP_LINK			( GPIO_CMNIO_RD + 33 )

#define GPIO_PORT_RD_ERROR_OUT			( GPIO_CMNIO_RD + 34 )
#define GPIO_PORT_RD_USB1_OCD			( GPIO_CMNIO_RD + 35 )
#define GPIO_PORT_RD_USB2_OCD			( GPIO_CMNIO_RD + 36 )
#define GPIO_PORT_RD_USB3_OCD			( GPIO_CMNIO_RD + 37 )
#define GPIO_PORT_WR_WIRELESS_RX		( GPIO_CMNIO_RD + 38 )
#define GPIO_PORT_RD_COMP3_LINK			( GPIO_CMNIO_RD + 39 )	/* COMP3 or SIDE COMP1 */

#define GPIO_PORT_RD_FMTUP_DATA			( GPIO_CMNIO_RD + 41 )

#ifdef ADD_GPIOI2C /* To use the two GPIO I2C by Jason Lim */
#define GPIO_PORT_GPIOI2C_SDARD_I2SSD_IN	( GPIO_CMNIO_RD + 42 )
#endif

#define GPIO_PORT_RD_MHL_DET			( GPIO_CMNIO_RD + 43 )
#define GPIO_PORT_RD_HDMI_INT			( GPIO_CMNIO_RD + 44 )

#define GPIO_PORT_RD_EPHY_INT			( GPIO_CMNIO_RD + 45 )
// for PDP board
#define GPIO_PORT_GPIOI2C_SDARD_I2SWS_OUTC	( GPIO_CMNIO_RD + 46 )
#define GPIO_PORT_RD_LNB_SHORTAGE		( GPIO_CMNIO_RD + 47 )	// LNB Shortage Detection in Japan

#define GPIO_PORT_RD_SMARTCARD_DET		( GPIO_CMNIO_RD + 48 )
#define GPIO_PORT_RD_SMARTCARD_DATA		( GPIO_CMNIO_RD + 49 )

#define GPIO_PORT_RD_CAM_SLIDE_DET		( GPIO_CMNIO_RD + 50 )
#define GPIO_PORT_RD_OLED_EXT_COMP_DONE	( GPIO_CMNIO_RD + 51 )

#define GPIO_PORT_RD_NUM_HDMI_PORTS		( 0 )			// Not for ATSC
//#define GPIO_PORT_WR_XC5000_RESET		( 0 )			// Chip Tuner Reset

//TODO read hdmi port check
#define GPIO_RD_CHECK_CONNECT_HDMI		( 0 )

#define GPIO_PORT_WR_DPM			( 0 )
#define GPIO_PORT_WR_HCONV			( 0 )


/* MICOM's GPIO */
#define GPIO_PORT_WR_AMP_MUTE			( GPIO_MICOM_WR + 1  )
#define GPIO_PORT_WR_INV_CTL			( GPIO_MICOM_WR + 2  )
#define GPIO_PORT_WR_PANEL_CTL			( GPIO_MICOM_WR + 3  )
#define GPIO_PORT_WR_HP_MUTE			( GPIO_MICOM_WR + 4  )
#define GPIO_PORT_WR_LED_B			( GPIO_MICOM_WR + 5  )
#define GPIO_PORT_WR_LED_ON			( GPIO_MICOM_WR + 6  )
#define GPIO_PORT_WR_SCART1_MUTE		( GPIO_MICOM_WR + 7  )
#define GPIO_PORT_WR_SCART2_MUTE		( GPIO_MICOM_WR + 8  )
#define GPIO_PORT_WR_WARM_LED_ON		( GPIO_MICOM_WR + 9  )
#define GPIO_PORT_WR_EDID_WP			( GPIO_MICOM_WR + 10 )
#if 0	// SOC GPIO로 이동
#define GPIO_PORT_WR_OPC_EN 			( GPIO_MICOM_WR + 11 )
#endif
#define GPIO_PORT_WR_VGA_EEPROM_WP		( GPIO_MICOM_WR + 12 ) //balup_090728
#define GPIO_PORT_WR_WIRELESS_READY_SWITCH	( GPIO_MICOM_WR + 13 ) //balup
#define GPIO_PORT_WR_WIRELESS_READY_POWER	( GPIO_MICOM_WR + 14 ) //balup
#define GPIO_PORT_RD_HWOPTION			( GPIO_MICOM_RD + 15 ) //balup_090908

/* PDP Only */
#define GPIO_PORT_WR_PDP_DISP 			( GPIO_MICOM_WR + 16 ) //earnest_090911
#define	GPIO_PORT_WR_PDP_PORT			( GPIO_MICOM_WR + 17 )	// TBD

#define GPIO_PORT_WR_NAND_WP			( GPIO_MICOM_WR + 18 )//hjkoh_091027 for nand write protection.
#define GPIO_PORT_WR_PDP_5VON 			( GPIO_MICOM_WR + 19 )	// PDP Module 5V on/off

#define	GPIO_PORT_WR_PDP_VAVS			( GPIO_MICOM_WR + 20 )	// PDP Module VaVs on/off


/* NOT USED */
#define	GPIO_PORT_WR_MNT_OUT_MUTE		( 0 )
#define GPIO_PORT_WR_TV_OUT_MUTE		( 0 )
#define GPIO_PORT_WR_STANDBY_MUTE		( 0 )			//TBD
#define GPIO_PORT_WR_AI_ENABLE			( 0 )
#define GPIO_PORT_WR_PDP_RL_ON   		( 0 ) 			// for DVB PDP.
//efine	GPIO_PORT_WR_PDP_VAVS			( 0 ) 			// for DVB PDP
#define GPIO_PORT_WR_PDP_LEDW1			( 0 )			// for DVB PDP
#define GPIO_PORT_WR_PDP_LEDW2			( 0 )			// for DVB PDP


/*	GPIO INFO	:	GPIO_PIN_INFO_T의 초기값
	** PLATFORM 별로 초기값 및 ACTIVE 상태(inverted)를 상이하게 설정하는 경우에 사용됨 **
*/
#define	GPIO_INFO_WR_INV_CTL			{ GPIO_PORT_WR_INV_CTL ,	FALSE,	FALSE	}		//INV_CTL	(init: FALSE	inverted: FALSE)
#define GPIO_INFO_WR_PANEL_CTL			{ GPIO_PORT_WR_PANEL_CTL,	FALSE,	FALSE	}		//PANEL_CTL	(init: FALSE	inverted: FALSE)
#define GPIO_INFO_WR_AMP_MUTE   		{ GPIO_PORT_WR_AMP_MUTE,	TRUE,	FALSE	}		//AMP_MUTE	(init: TRUE 	inverted: FALSE)
#define GPIO_INFO_WR_HP_MUTE			{ GPIO_PORT_WR_HP_MUTE ,	TRUE,	TRUE	}		//HP_MUTE	(init: TRUE 	inverted: TRUE)
#define GPIO_INFO_WR_SCART_MUTE			{ GPIO_PORT_WR_SCART1_MUTE ,	TRUE,	TRUE	}

#define GPIO_PORT_UNDEFINE			-1

#define GPIO_OUTPUT_PUSHPULL			0
#define GPIO_OUTPUT_OPENDRAIN			1
#endif  /* _CMNIO_GPIO_INDEX_H_ */
