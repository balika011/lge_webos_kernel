/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

#ifndef CONFIG_DEFAULT_H
#define CONFIG_DEFAULT_H


//----------------------------------------------------------------------------
// Initial dram settings
//----------------------------------------------------------------------------
#define DEFAULT_DRAM_TYPE               DDR_II_x2
#define FLAG_FORCE_32BIT    0           // 0-DDR2 x2; 1-DDR2x1
#define DEFAULT_DRAM_COLADDR            (COL_ADDR_BIT_10)
#define DEFAULT_DDR_CLOCK               (783000000)
#define FLAG_DDR_QFP                    0
#define FLAG_FULL_FREQ                  0
#define DEFAULT_DRAM_8_BANKS                0


#define DDR2_PAD_DRIVING_0          0x66116611 //0x66ff66ff
#define DDR2_PAD_DRIVING_1          0x77007700

#define DDR2_OUT_CLK_DELAY_CHAIN_0  0x82808080
#define DDR2_OUT_CLK_DELAY_CHAIN_1  0x82808180
#define DDR2_OUT_CLK_DELAY_CHAIN_2  0x82808080
#define DDR2_OUT_CLK_DELAY_CHAIN_3  0x82808180

#define DDR2_DQS_RDSEL				0x00000000	
#define DDR2_DQ_RDSEL				0x2c2c2c28 //0x28202020

// Binary coded gating window of byte 3 ~ 0 = 49, 53, 47, 55
#define DDR_DQSIEN			0x292F382C


//----------------------------------------------------------------------------
// FBM setup
//----------------------------------------------------------------------------
#ifndef FBM_MEM_CFG_SIZE
#define FBM_MEM_CFG_SIZE                FBM_MEM_CFG_MT5387_VERF
#endif /* FBM_MEM_CFG_SIZE */

//----------------------------------------------------------------------------
// Default Panel setting
//----------------------------------------------------------------------------
#ifndef DEFAULT_PANEL_SELECT
#define DEFAULT_PANEL_SELECT            (PANEL_DEFAULT)
#endif
#define BACKLIGHT_GPIO                  (GPIO(7)) //Andy 2009/6/2 �U�� 02:38:13 (OPCTRL(1))   // GPIO_3
#define BACKLIGHT_ON_VAL                (1) //Andy 2009/6/2 �U�� 02:39:17 (0)
#define PANELPOWER_GPIO                 (GPIO(79))  // LVDS_PWR_ON
#define PANELPOWER_ON_VAL               (1)
#define PANEL_BACKLIGHT_PWM_PORT        (1)
#define PANEL_BACKLIGHT_PWM_PIN         (PINMUX_GPIO_00)
#define PANEL_BACKLIGHT_PWM_GPIO        (1)//(0)
#define SUPPORT_BACKLIGHT_DIMMING_INVERT (1)
#define PANEL_INDEX_FROM_EEPROM     (1)
// disable spread spectrum for vga output
#define SPREAD_SPECTRUM_PERMILLAGE      (0)

#define LOADER_LOGO_NUMBER              (1)

//----------------------------------------------------------------------------
// Audio Customization
//----------------------------------------------------------------------------
#define AUDIO_INCLUDE_FILE              "compal/toshiba_aud.h"
#define AUDIO_Mute_Gpio_Num             (GPIO(5))  //Andy (2009/6/2 �U�� 02:55:24  GPIO(8))
#define AUDIO_Amp_Gpio_Num              (GPIO(15))
#define AUDIO_MUTE_GPIO_POLARITY        AUD_MUTE_GPIO_LOW_ENALBE
 #define AUDIO_AMP_GPIO_POLARITY         AUD_AMP_GPIO_LOW_ENALBE
#define AUDIO_HP_PLUG_IN_GPIO          (GPIO(1)) //Andy TBD 2009/6/2 �U�� 02:58:34(GPIO(7))
#define AUDIO_HP_PLUG_IN_POLARITY       (AUD_GPIO_LOW_ENALBE)
#define AUDIO_INPUT_MUX_SEL_TABLE       (&_rToshibaAudInputMuxTable)
#define AUDIO_INTER_DAC_USAGE               (&_arToshibaInterDacUsage)

//----------------------------------------------------------------------------
// Other GPIO customization
//----------------------------------------------------------------------------
#define FLAG_EXT_STATUS                 (0)
//=>Andy 2009/6/2 �U�� 03:00:40
#define POWER_LED_GPIO               (OPCTRL(3))
#define POWER_LED_ON_VAL             (1)
#define BACKUP_LED_GPIO              (OPCTRL(1))
#define BACKUP_LED_ON_VAL            (1)
//<=Andy
// #define POWER_LED_GPIO               (OPCTRL(0))
// #define POWER_LED_ON_VAL             (0)
// #define BACKUP_LED_GPIO              (OPCTRL(0))
// #define BACKUP_LED_ON_VAL            (0)
// #define POWER_CTRL_GPIO
// #define POWER_CTRL_ON_VAL
// #define DVD_CTRL_GPIO
// #define DVD_CTRL_ON_VAL
#define GPIO_INCLUDE_FILE               "compal/toshiba_gpio.h"
//----------------------------------------------------------------------------
// EEPROM customization
//----------------------------------------------------------------------------
#define SYSTEM_EEPROM_BUS                   (0)
#define SYSTEM_EEPROM_ADDRESS               (0xA0)
#define SYSTEM_EEPROM_SIZE                  (4096)
#define SYSTEM_EEPROM_CLKDIV                (0x100)
#define SYSTEM_EEPROM_PAGESIZE              (16)

//#define SYSTEM_EEPROM_WP_GPIO               (ADIN(5)) //Andy 2009/6/2 �U�� 03:05:11 (OPCTRL(0))
//#define SYSTEM_EEPROM_WP_ENABLE_POLARITY    (0)
//#define EDID_EEPROM_WP_GPIO			        (GPIO(3)) //Andy 2009/6/2 �U�� 03:06:08 (OPCTRL(3))
//#define EDID_EEPROM_WP_ENABLE_POLARITY      (0)

#define SYSTEM_EEPROM_WP_GPIO               (OPCTRL(0))
#define SYSTEM_EEPROM_WP_ENABLE_POLARITY    (0)
#define EDID_EEPROM_WP_GPIO			        (OPCTRL(3))
#define EDID_EEPROM_WP_ENABLE_POLARITY      (0)

#define EEPROM_CEC_MENU_LANGUAGE_OFFSET      (0x500)
#define EEPROM_CEC_ENABLE_OFFSET      			(0x504)
#define EEPROM_CEC_ONE_TOUCH_PLAY_ENABLE_OFFSET (0x505)

//----------------------------------------------------------------------------
/// SIF
//----------------------------------------------------------------------------
#define SIF_ENABLE_SCL_STRETCH      (0)
#define SIF_DISABLE_OPEN_DRAIN      (0)
#define SIF_ENABLE_PDWNC_MASTER     (0)

//----------------------------------------------------------------------------
// Default usb upgrade setting
//----------------------------------------------------------------------------
#define AUTO_USB_UPGRADE_ENABLE     (1)


//----------------------------------------------------------------------------
// USB Customization
//----------------------------------------------------------------------------
#define USB0_VBUS_GPIO                      (GPIO(46))
#define USB0_VBUS_GPIO_POLARITY		        (0)//(1)
// #define USB1_VBUS_GPIO                      (2)
// #define USB1_VBUS_GPIO_POLARITY		        (1)

#define USB0_OC_GPIO                        (GPIO(47))
#define USB0_OC_GPIO_POLARITY		        (0)
// #define USB1_OC_GPIO                        (3)
// #define USB1_OC_GPIO_POLARITY		        (0)

//#define USB_DISABLE_SUSPEND_MODE /* Disable USB suspend, share 240 Mhz with H.264.*/
//----------------------------------------------------------------------------
// Button Customization
//----------------------------------------------------------------------------
//#define FLAG_BUTTON_SUPPORT                 (0)
//=>Andy 2009/6/2 �U�� 03:15:15
#define FLAG_BUTTON_SUPPORT                 (1)
#define BUTTON_INCLUDE_FILE                 "compal/toshiba_button.h"
//<Andy

//----------------------------------------------------------------------------
// MTK IR only setting
//----------------------------------------------------------------------------
#define IR_INCLUDE_FILE                     "compal/toshiba_ir.h"

#ifndef NPTV_SOURCE_TABLE
#define NPTV_SOURCE_TABLE                   "compal/toshiba_source_table.c"
#endif

//----------------------------------------------------------------------------
// default HDMI GPIO control
//----------------------------------------------------------------------------
#ifndef HDMI_GPIO_CONF
#define HDMI_GPIO_CONF                      "compal/toshiba_hdmi_conf_empty.c"
#endif
//----------------------------------------------------------------------------
// default T8032 FW Properties
//----------------------------------------------------------------------------
#ifndef T8032UP_INCLUDE_FILE
#define T8032UP_INCLUDE_FILE                "compal/toshiba_t8032fw.h"
#define T8032UP_OFFSET                      au1T8032FW
#define T8032UP_SIZE                        16384
#endif

/*
 * default VGA table
*/
#ifndef VGA_INCLUDE_FILE
#define VGA_INCLUDE_FILE                    "compal/toshiba_vga.h"
#endif

#if defined(CC_SUPPORT_TVE_CTRL_BY_DRIVER) || defined(CC_SUPPORT_TVE_CTRL_BY_MW)
#define TVE_INCLUDE_FILE                    "compal/toshiba_tve.c"
#endif

/*
 * default quality table
*/
#ifndef QTYTBL_INCLUDE_FILE
//#define QTYTBL_INCLUDE_FILE                 "mtk/QTY_DEFAULT/mtk_QtyTbl.h"
#endif

#ifdef SUPPORT_FLASH_PQ
#endif // #if SUPPORT_FLASH_PQ

/*
 * default color parameter
*/
#ifndef COLOR_PARAM_INCLUDE_FILE
//#define COLOR_PARAM_INCLUDE_FILE            "mtk/PANEL_DEFAULT/color_param.h"
#endif

//----------------------------------------------------------------------------
// SCART FS Status Servo ADC Customization
//----------------------------------------------------------------------------
#ifndef SCART1_FS_SERVO_ADC
#define SCART1_FS_SERVO_ADC     0
#endif
#ifndef SCART2_FS_SERVO_ADC
#define SCART2_FS_SERVO_ADC     4
#endif

/*
 * dynamic backlight parameter
*/

#else /* CONFIG_DEFAULT_H */
#error "Cannot include multi-config files"
#endif /* CONFIG_DEFAULT_H */

