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
// Tool Protection
//----------------------------------------------------------------------------
#define CUSTOMER_NAME 						"sony"
#define CUSTOMER_PASSWORD	                "bravoiris"

//----------------------------------------------------------------------------
// Initial dram settings
//----------------------------------------------------------------------------
#define DEFAULT_DRAM_TYPE                   DDR_II_x2
#define DEFAULT_DRAM_COLADDR                (COL_ADDR_BIT_10)
#ifdef SUPPORT_1GDDR_TYPE
#define DEFAULT_DDR_CLOCK                   (837000000)
#else
#define DEFAULT_DDR_CLOCK                   (783000000)
#endif

//Tag verion eeprom offset
#define EEPROM_NVRAM_VERION_OFFSET	0x393 //(DRVCUST_EEPROM_DRIVER_SIZE - 13) last (6~13) is edid version
#define EEPROM_NVRAM_VERION_LENGTH	(8)  
#define EEPROM_EDID_VERION_OFFSET	0x38B //(DRVCUST_EEPROM_DRIVER_SIZE - 21) last (14~21) is edid version

//----------------------------------------------------------------------------
// FBM setup
//----------------------------------------------------------------------------
#ifndef FBM_MEM_CFG_SIZE
#define FBM_MEM_CFG_SIZE                    FBM_MEM_CFG_MT5387_VERF
#endif /* FBM_MEM_CFG_SIZE */

//----------------------------------------------------------------------------
// Default Panel setting
//----------------------------------------------------------------------------
#ifndef DEFAULT_PANEL_SELECT
#define DEFAULT_PANEL_SELECT                (PANEL_DEFAULT)
#endif
#define BACKLIGHT_GPIO                      (GPIO(3))   // GPIO_3
#define BACKLIGHT_ON_VAL                    (0)
#define PANELPOWER_GPIO                     (GPIO(79))  // LVDS_PWR_ON
#define PANELPOWER_ON_VAL                   (1)
#define PANEL_BACKLIGHT_PWM_PORT            (1)
#define PANEL_BACKLIGHT_PWM_PIN             (PINMUX_GPIO_00)
#define PANEL_BACKLIGHT_PWM_GPIO            (1)

#define SUPPORT_BACKLIGHT_DIMMING_INVERT    (1)

// disable spread spectrum for vga output
#define SPREAD_SPECTRUM_PERMILLAGE          (0)

#define PDWNC_POLARITY                      (0)

#define PANEL_INDEX_FROM_EEPROM             (1)

//----------------------------------------------------------------------------
// Audio Customization
//----------------------------------------------------------------------------
#define AUDIO_INCLUDE_FILE                  "Foxconn/Iris_aud.h"
#define AUDIO_Mute_Gpio_Num                 (GPIO(8))
// #define AUDIO_Amp_Gpio_Num               (OPCTRL(3))
#define AUDIO_MUTE_GPIO_POLARITY            AUD_MUTE_GPIO_HIGH_ENALBE
// #define AUDIO_AMP_GPIO_POLARITY          AUD_AMP_GPIO_HIGH_ENALBE
#define AUDIO_DIGITAL_AMP_RESET_GPIO        (GPIO(7))
#define AUDIO_DIGITAL_AMP_RESET_GPIO_POLARITY    1
#define AUDIO_ADAC_USAGE                    (&_arIrisAdacUsage)
#define AUDIO_INPUT_MUX_SEL_TABLE           (&_rIrisAudInputMuxTable)
#define AUDIO_INTER_DAC_USAGE               (&_arIrisInterDacUsage)

//----------------------------------------------------------------------------
// Other GPIO customization
//----------------------------------------------------------------------------
#define FLAG_EXT_STATUS                     (0)
#define POWER_LED_GPIO                      (GPIO(210))
#define POWER_LED_ON_VAL                    (1)

#define BACKUP_LED_GPIO             (GPIO(205))
#define BACKUP_LED_ON_VAL         1

#define T8032_CTRL_ERR_LED                  BACKUP_LED_GPIO
#define T8032_CTRL_ERR_LED_ON_VAL           BACKUP_LED_ON_VAL
// #define BACKUP_LED_GPIO                  (OPCTRL(0))
// #define BACKUP_LED_ON_VAL                (0)
// #define POWER_CTRL_GPIO
// #define POWER_CTRL_ON_VAL

//----------------------------------------------------------------------------
// EEPROM customization
//----------------------------------------------------------------------------
#define SYSTEM_EEPROM_BUS                       (0)
#define SYSTEM_EEPROM_ADDRESS                   (0xA0)
#define SYSTEM_EEPROM_SIZE                      (4096)
#define SYSTEM_EEPROM_CLKDIV                    (0x100)
#define SYSTEM_EEPROM_PAGESIZE                  (16)
#define SYSTEM_EEPROM_WP_GPIO                   (OPCTRL(0))
#define SYSTEM_EEPROM_WP_ENABLE_POLARITY        (0)
#define EDID_EEPROM_WP_GPIO			            (OPCTRL(3))
#define EDID_EEPROM_WP_ENABLE_POLARITY          (0)

//----------------------------------------------------------------------------
// CEC customization
//----------------------------------------------------------------------------
#ifndef EEPROM_CEC_MENU_LANGUAGE_OFFSET
#define EEPROM_CEC_MENU_LANGUAGE_OFFSET         (0x498)
#endif

#ifndef EEPROM_CEC_ENABLE_OFFSET
#define EEPROM_CEC_ENABLE_OFFSET      			(0x49C)
#endif

#ifndef EEPROM_CEC_ONE_TOUCH_PLAY_ENABLE_OFFSET
#define EEPROM_CEC_ONE_TOUCH_PLAY_ENABLE_OFFSET (0x49D)
#endif
//----------------------------------------------------------------------------
/// SIF
//----------------------------------------------------------------------------
#define SIF_ENABLE_SCL_STRETCH              (0)
#define SIF_DISABLE_OPEN_DRAIN              (0)
#define SIF_ENABLE_PDWNC_MASTER             (0)

//----------------------------------------------------------------------------
// NOR customization
//----------------------------------------------------------------------------
#ifndef CC_NAND_ENABLE

#define NOR_FLASH_TOTAL_SIZE            (8*1024*1024)

#ifndef LOADER_MAX_SIZE
#define NOR_FLASH_LOADER_SIZE           (4*64*1024)
#else /* LOADER_MAX_SIZE */
#define NOR_FLASH_LOADER_SIZE           (LOADER_MAX_SIZE)   // nor_0 - loader
#endif /* LOADER_MAX_SIZE */

#define NOR_FLASH_APPIMAGE_SIZE         (0x0500000)                    /* nor_1 - system image                 */
#define NOR_PART_SIZE_1                 NOR_FLASH_APPIMAGE_SIZE        /* nor_1 - system image                 */
#define NOR_PART_SIZE_2                 (0x00B0000)                    /* nor_2 - uP bin                       */
#define NOR_PART_SIZE_3                 (0x0010000)                    /* nor_3 - reserved                     */
#define NOR_PART_SIZE_4                 (0x0020000)                    /* nor_4 - channel list A               */
#define NOR_PART_SIZE_5                 (0x0020000)                    /* nor_5 - channel list B               */
#define NOR_PART_SIZE_6                 (0x0080000)                    /* nor_6 - Panel / PQ / AQ              */
#define NOR_PART_SIZE_7                 (0x0100000)                    /* nor_7 - Picture                      */
//#define NOR_PART_SIZE_8                 (0x0050000)                    /* nor_8 - Ramdisk                    */

#define FLASH_PQ_BASE_PARTITION         (6)  
#define FLASH_PQ_BASE_ADDRESS           (NOR_FLASH_LOADER_SIZE + \
                                         NOR_PART_SIZE_1 + \
                                         NOR_PART_SIZE_2 + \
                                         NOR_PART_SIZE_3 + \
                                         NOR_PART_SIZE_4 + \
                                         NOR_PART_SIZE_5)
//#define FLASH_PQ_BASE_ADDRESS           (0x640000)

#else
//----------------------------------------------------------------------------
// NAND customization
//----------------------------------------------------------------------------
#define NAND_FLASH_TOTAL_SIZE            (32*1024*1024)

#define NAND_FLASH_APPIMAGE_SIZE         (0x0500000)                    /* nand_1 - system image                 */
#define NAND_PART_SIZE_1                 NAND_FLASH_APPIMAGE_SIZE       /* nand_1 - system image                 */
#define NAND_PART_SIZE_2                 (0x00B0000)                    /* nand_2 - uP bin                       */
#define NAND_PART_SIZE_3                 (0x0010000)                    /* nand_3 - reserved                     */
#define NAND_PART_SIZE_4                 (0x0020000)                    /* nand_4 - channel list A               */
#define NAND_PART_SIZE_5                 (0x0020000)                    /* nand_5 - channel list B               */
#define NAND_PART_SIZE_6                 (0x0080000)                    /* nand_6 - Panel / PQ / AQ              */
#define NAND_PART_SIZE_7                 (0x0100000)                    /* nand_7 - Picture                      */
//#define NAND_PART_SIZE_8                 (0x0050000)                    /* nand_8 - Ramdisk                      */

#define NAND_FLASH_STATIC_LAST_PART_ID   (3)
#define FLASH_PQ_BASE_PARTITION          (6)  
#define FLASH_PQ_BASE_ADDRESS            (0)

#define NAND_PART_END                    (7)                           /* total 8 Partitions */


#define NAND_FLASH_STATIC_PART_SIZE \
    (   (NAND_PART_SIZE_0) + \
        (NAND_PART_SIZE_1) + (NAND_PART_SIZE_2) + (NAND_PART_SIZE_3))


#define NAND_FLASH_DYNAMIC_PART_USED_SIZE \
    (   (NAND_PART_SIZE_4) + (NAND_PART_SIZE_5)  + (NAND_PART_SIZE_6) + \
        (NAND_PART_SIZE_7))


#endif /* CC_NAND_ENABLE */


//----------------------------------------------------------------------------
// USB Customization
//----------------------------------------------------------------------------
#define USB0_VBUS_GPIO                      (GPIO_USB_PWR_EN0)
#define USB0_VBUS_GPIO_POLARITY		        (0)


#define USB0_OC_GPIO                        (GPIO_USB_PWR_ERR0)
#define USB0_OC_GPIO_POLARITY		        (0)


//#define USB_DISABLE_SUSPEND_MODE /* Disable USB suspend, share 240 Mhz with H.264.*/
//----------------------------------------------------------------------------
// Button Customization
//----------------------------------------------------------------------------
#define FLAG_BUTTON_SUPPORT                 (1)
#define BUTTON_INCLUDE_FILE                 "Foxconn/Iris_button.h"
//#define GPIO_INCLUDE_FILE                   "Foxconn/Iris_gpio.h"

//----------------------------------------------------------------------------
// MTK IR only setting
//----------------------------------------------------------------------------
//#define IR_INCLUDE_FILE                     "mtk/mtk_ir.h"
#define IR_INCLUDE_FILE                     "Foxconn/mtk_ir_sony.h"

#ifndef NPTV_SOURCE_TABLE
#define NPTV_SOURCE_TABLE                   "Foxconn/source_table.c"
#endif

//----------------------------------------------------------------------------
// IRRX customization
//----------------------------------------------------------------------------
#define IRRX_FLAG_IRRX_BLINKING             (1)
#ifdef SUPPORT_MTK_IR_WAKEUP
#define FLAG_IR_PRGUPDOWN_WAKEUP            (1)
#else
#define FLAG_IR_USER_DEFINE_WAKEUP			(1)
#endif
#define KEYPAD_WAKEUP_BIT                   (DKBIT_POWER | DKBIT_PRG_UP | DKBIT_PRG_DOWN)//david modified

//----------------------------------------------------------------------------
// default HDMI GPIO control
//----------------------------------------------------------------------------
#ifndef HDMI_GPIO_CONF
#define HDMI_GPIO_CONF                      "Foxconn/Iris_hdmi_conf_empty.c"
#endif
//----------------------------------------------------------------------------
// default T8032 FW Properties
//----------------------------------------------------------------------------
#ifndef T8032UP_INCLUDE_FILE
#define T8032UP_INCLUDE_FILE                "Foxconn/Iris_t8032fw.h"
//#define T8032UP_INCLUDE_FILE                "mtk/mtk_t8032fw.h"
#define T8032UP_OFFSET                      au1T8032FW
#define T8032UP_SIZE                        16384
#endif

/*
 * default VGA table
*/
#ifndef VGA_INCLUDE_FILE
#define VGA_INCLUDE_FILE                    "Foxconn/Iris_vga.h"
#endif

/*
 * default TVE File
 */
#if defined(CC_SUPPORT_TVE_CTRL_BY_DRIVER)||defined(CC_SUPPORT_TVE_CTRL_BY_MW)
#define TVE_INCLUDE_FILE                    "mtk/mtk_tve.c"
#endif

/*
 * TVD
*/

#ifndef TVD_SUPPORT_PEDESTAL
#define TVD_SUPPORT_PEDESTAL (1)
#endif

#define TVD_SUPPORT_OUTPUT_216 (1)

/*
 * default quality table
*/


#define SUPPORT_NVM_GAMMA (1)
#define FLASH_PQ_SCE_MAX  (1)

#ifndef QTYTBL_INCLUDE_FILE
    #define QTYTBL_INCLUDE_FILE               "Foxconn/QTY_DEFAULT/Foxconn_QtyTbl.h"
#endif

#ifdef SUPPORT_FLASH_PQ

#ifdef CC_NAND_ENABLE 
    #define FLASH_PQ_USE_NAND_FLASH         (1)
#else
    #define FLASH_PQ_USE_NAND_FLASH         (0)
#endif

#define SUPPORT_FLASH_PQ_ENABLE_GAMMA           (1)
#define SUPPORT_FLASH_PQ_GAMMA_X3               (0)
#define SUPPORT_FLASH_PQ_ENABLE_SCE             (1)
#define SUPPORT_FLASH_PQ_ENABLE_QTY             (1)
#define SUPPORT_FLASH_PQ_ENABLE_QTY_MAX_MIN     (1)
#define SUPPORT_FLASH_PQ_ENABLE_QTY_SMART_PIC   (1)
#define SUPPORT_LINK_M4_CONTRAST_SATURATION     (0)

#endif // #if SUPPORT_FLASH_PQ
/*
 * NVM Gamma
*/
#define SUPPORT_NVM_GAMMA (1)

#define BIND_CONTRAST_SATURATION    (1)

#define UI_QTY_USE_LINEAR_MAP       (1)

/*
 * Gamma Top Point
*/
#define SUPPORT_GAMMA_TOP_POINT (1)
/*
 * default color parameter
*/
#ifndef COLOR_PARAM_INCLUDE_FILE
    #define COLOR_PARAM_INCLUDE_FILE            "Foxconn/PANEL_DEFAULT/color_param.h"
#endif

#ifndef CUSTOM_VIDEO_DRV_BODY_FILE
    #define CUSTOM_VIDEO_DRV_BODY_FILE          "Foxconn/video_drv/Foxconn_video_drv.c"
#endif

/*
 * dynamic backlight parameter
*/

#define MAGIC_MAX_INPUT_FORCE_FACTORY           0xC0 // Modify factory mode

#else /* CONFIG_DEFAULT_H */
#error "Cannot include multi-config files"
#endif /* CONFIG_DEFAULT_H */

