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
/*-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: mtk_mod.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file mtk_mod.c
 *  SDAL module
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "drv_ver.h"
#include "ionodes.h"
#include "cb_low.h"
#include "x_linux.h"
#include "x_serial.h"
#include "x_util.h"
#include "x_timeprofile.h"
#include <linux/device.h>
#include <linux/cdev.h>
#include "panel.h"
#include "x_timer.h"
#include "ostg_if.h"
#include <linux/version.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
#include <linux/sched.h>
#endif
#include "eeprom_if.h"
#include "x_pdwnc.h"
#include "aud_if.h"
#include "x_pinmux.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

#define INIT_SHOW_VER      1


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

MODULE_LICENSE("Proprietary");


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Imported variables
//-----------------------------------------------------------------------------
#ifdef CONFIG_TV_DRV_VFY
unsigned long u4MtalLogMask = 0;  //undefined  symbol,but don't find the source code position. 
#endif
extern struct file_operations cli_fops; /* in cli_mod */
extern void BspMainTask(void); /* in bsp */
#ifndef CONFIG_TV_DRV_VFY
extern struct file_operations sdal_fops; /* in sdal_if */
extern struct file_operations papi_fops; /* in papi_if */
extern struct file_operations mtal_fops; /* in mtal_if */
/* ddi ioctl is not used for current case */
//extern struct file_operations ddi_fops; /*in ddi_if*/
extern struct file_operations cli_fops; /* in cli_mod */
extern struct file_operations cb_fops; /* in cb_mod */
extern struct file_operations cb_native_fops; /* in cb_native */
extern struct file_operations gpio_fops; /* in gpio_mod */
extern struct file_operations jpg_fops; /* in jpg_mod */
extern struct file_operations feeder_fops; /* in feeder_mod */
extern struct file_operations b2r_fops; /* in b2r_mod */

//extern struct file_operations eep_fops;     /* in eep_mod */
extern struct file_operations tsrec_fops; /* in tsrec_mod */
extern struct file_operations fbm_fops;
extern struct file_operations pvr_fops; /* in tsrec_mod */
#ifdef LINUX_TURNKEY_SOLUTION
#if defined(CC_ENABLE_VOMX) || defined(CC_ENABLE_AOMX) || defined(CC_ENABLE_VDECOMX)
extern struct file_operations kmem_fops; /* in kmem_mod */
#endif
#if defined(CC_ENABLE_AOMX)
extern struct file_operations aomx_fops;
#endif
#if defined(CC_ENABLE_VOMX) 
extern struct file_operations vomx_fops; /* in vdec_mod */
#endif
#if defined(CC_ENABLE_VDECOMX)
extern struct file_operations vdecomx_fops; /* in vdecomx*/
extern struct file_operations vdpomx_fops; /* in vdpomx */
#endif
#if defined(CC_ENABLE_VENCOMX)
extern struct file_operations vencomx_fops; /* in vencomx*/
#endif
#if defined(CC_ENABLE_ADECOMX)
extern struct file_operations adecomx_fops; /* in adecomx*/
#endif
#endif

extern void vDrvThermalInit(void);
extern int __init _mt53fb_init(void); /* in mt53_fb */
extern int mtkinp_events_init(void); /* in mt53xx_ir */
extern void fbm_blkdev_init(void);
extern int eeprom_module_init(void);
#ifdef LINUX_TURNKEY_SOLUTION
#if defined(CONFIG_ARCH_MT5396) || defined(CONFIG_ARCH_MT5368) || defined(CONFIG_ARCH_MT5389) || defined(CONFIG_ARCH_MT5398)
extern u32 u4GetEepromMacAddr(u8 *EepromMacAddr);
extern void star_get_mac_addr(u8 *EepromMacAddr);
#endif
#endif

#endif
#ifdef CC_ENABLE_REBOOT_REASON
static int set_reset_reason_init(void);
#endif
extern VOID FBM_Init(VOID);
//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static int panel_early_init = 1;
#ifndef CONFIG_TV_DRV_VFY
extern int osd1_src_width;
extern int osd1_src_height;
extern int osd1_output_width;
extern int osd1_output_height;
extern int osd2_src_width;
extern int osd2_src_height;
extern int osd2_output_width;
extern int osd2_output_height;
#endif
static unsigned int target_country = -1;
static int panel_id = -1;

/* 
   To add new device driver, please allocate unused minor number from Major=241.
   DO NOT ADD ANOTHER MAJOR NUMBER!
   Please see chiling/driver/linux/ioctl/ionodes.h for current usage.
 */
static const DEVLIST_T devlist[] __initdata =
{
#ifdef CONFIG_TV_DRV_VFY
{IO_NODE_MAJOR_CLI,  IO_NODE_MINOR_CLI,  1, DEV_CLI,  S_IRUSR | S_IWUSR | S_IRGRP, &cli_fops},
#else
#ifdef CC_USE_SDAL
    {IO_NODE_MAJOR_SDAL, 0, 1, DEV_SDAL, S_IRUSR | S_IWUSR | S_IRGRP, &sdal_fops},
#endif
#ifdef CC_USE_PAPI
    {IO_NODE_MAJOR_PAPI, 0, 1, DEV_PAPI, S_IRUSR | S_IWUSR | S_IRGRP, &papi_fops},
#endif
#ifdef CC_USE_MTAL
    {IO_NODE_MAJOR_MTAL, IO_NODE_MINOR_MTAL, 1, DEV_MTAL, S_IRUSR | S_IWUSR | S_IRGRP, &mtal_fops},
#endif
#ifdef CC_USE_DDI
    /* ddi ioctl is not used for current case */
    //{IO_NODE_MAJOR_DDI, 0, 1, DEV_DDI, S_IRUSR | S_IWUSR | S_IRGRP, &ddi_fops},
#endif
    {IO_NODE_MAJOR_CLI,  IO_NODE_MINOR_CLI,  1, DEV_CLI,  S_IRUSR | S_IWUSR | S_IRGRP, &cli_fops},
    {IO_NODE_MAJOR_CB,   IO_NODE_MINOR_CB,   1, DEV_CB,   S_IRUSR | S_IWUSR | S_IRGRP, &cb_fops},
#ifdef CONFIG_NATIVE_CB2
    {IO_NODE_MAJOR_CB,   IO_NODE_MINOR_NATIVE_CB,   1, DEV_CB,   S_IRUSR | S_IWUSR | S_IRGRP, &cb_native_fops},
#endif
    {IO_NODE_MAJOR_GPIO, IO_NODE_MINOR_GPIO, 1, DEV_GPIO, S_IRUSR | S_IWUSR | S_IRGRP, &gpio_fops},
    {IO_NODE_MAJOR_JPG,  0, 1, DEV_JPG,  S_IRUSR | S_IWUSR | S_IRGRP, &jpg_fops},
    {IO_NODE_MAJOR_FEEDER, 0, 1, DEV_FEEDER, S_IRUSR | S_IWUSR | S_IRGRP, &feeder_fops},
    {IO_NODE_MAJOR_TSREC, 0, 1, DEV_TSREC, S_IRUSR | S_IWUSR | S_IRGRP, &tsrec_fops},
    {IO_NODE_MAJOR_PVR, 0, 1, DEV_PVR, S_IRUSR | S_IWUSR | S_IRGRP, &pvr_fops},
    {IO_NODE_MAJOR_B2R, 0, 1, DEV_B2R, S_IRUSR | S_IWUSR | S_IRGRP, &b2r_fops},
#if defined(LINUX_TURNKEY_SOLUTION)
#if defined(CC_ENABLE_VOMX) || defined(CC_ENABLE_AOMX) || defined(CC_ENABLE_VDECOMX)
    {IO_NODE_MAJOR_KMEM, 0, 1, DEV_KMEM, S_IRUSR | S_IWUSR | S_IRGRP, &kmem_fops},
#endif
#if defined(CC_ENABLE_AOMX)
    {IO_NODE_MAJOR_AOMX, 0, 1, DEV_AOMX, S_IRUSR | S_IWUSR | S_IRGRP, &aomx_fops},
#endif
#if defined(CC_ENABLE_VOMX)
    {IO_NODE_MAJOR_VOMX, 0, 1, DEV_VOMX, S_IRUSR | S_IWUSR | S_IRGRP, &vomx_fops},
#endif
#if defined(CC_ENABLE_VDECOMX)
    {IO_NODE_MAJOR_VOMX, IO_NODE_MINOR_VDPOMX, 1, DEV_VDPOMX, S_IRUSR | S_IWUSR | S_IRGRP, &vdpomx_fops},
    {IO_NODE_MAJOR_VOMX, IO_NODE_MINOR_VDECOMX, 1, DEV_VDECOMX, S_IRUSR | S_IWUSR | S_IRGRP, &vdecomx_fops},
#endif
#if defined(CC_ENABLE_VENCOMX)
    {IO_NODE_MAJOR_VOMX, IO_NODE_MINOR_VENCOMX, 1, DEV_VENCOMX, S_IRUSR | S_IWUSR | S_IRGRP, &vencomx_fops},
#endif
#if defined(CC_ENABLE_ADECOMX)
    {IO_NODE_MAJOR_AOMX, 1, 1, DEV_ADECOMX, S_IRUSR | S_IWUSR | S_IRGRP, &adecomx_fops},
#endif    
#endif
    {IO_NODE_MAJOR_FBM, IO_NODE_MINOR_FBM, IO_NODE_NUMBER_FBM, DEV_FBM, S_IRUSR | S_IWUSR | S_IRGRP, &fbm_fops},
#endif //#ifdef CONFIG_TV_DRV_VFY
//    {IO_NODE_MAJOR_EEP,  0, DEV_EEP,  S_IRUSR | S_IWUSR | S_IRGRP, &eep_fops},
};

static struct cdev cdev_data[ARRAY_SIZE(devlist)];

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifndef CONFIG_TV_DRV_VFY
#ifdef LINUX_TURNKEY_SOLUTION
static void _ParallelInitThread(void *pvArgs)
{
    DTVCFG_T rDtvCfg;
    EEPDTV_GetCfg(&rDtvCfg);

    #ifdef CC_SUPPORT_ANDROID_L_RTC_WAKEUP
    if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) || (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
    #else
    if ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT) && (PDWNC_ReadWakeupReason() == PDWNC_WAKE_UP_REASON_RTC))
    #endif
    {
        Printf("_ParallelInitThread: Quiet Boot\n");
    }
    #ifdef CC_QUIET_HOT_BOOT_SUPPORT
    else if (rDtvCfg.u1Flags4 & DTVCFG_FLAG4_QUIET_HOT_BOOT)
    {
        Printf("_ParallelInitThread: Quiet Hot Boot\n");
    }
    #endif		
    else
    {       
        LDR_PmxDisplayHWInit();
    }
}
#endif
#endif

#if 0
static void _AudParallelInitThread(void *pvArgs)
{
	//MTAUDDEC_MW_AUD_Init();
	AUD_Init();

}
#endif

static int __init mtk_init(void)
{
    int i, ret;
    dev_t dev;
   // HANDLE_T hThread1;

#ifdef CC_ENABLE_HDMI_JTAG // for hdmi 0-2 jtag debug 
	BSP_PinSet(PIN_HDMI_CEC, 2);
	BSP_PinSet(PIN_HDMI_0_SCL, 2);
	BSP_PinSet(PIN_HDMI_0_SDA, 2);
	BSP_PinSet(PIN_HDMI_2_SCL, 3);
	BSP_PinSet(PIN_HDMI_2_SDA, 3);
	BSP_PinSet(PIN_HDMI_1_HPD, 1);
	BSP_PinSet(PIN_USB_DM_P0, 0);
	BSP_PinSet(PIN_OPCTRL6, 3);
	BSP_PinSet(PIN_OPCTRL7, 3);
	BSP_PinSet(PIN_OPCTRL8, 3);
	BSP_PinSet(PIN_OPCTRL9, 3);
#endif


#if INIT_SHOW_VER
    {
        #define MAX_VERSION_STRING_LEN 32
        CHAR szVer1[MAX_VERSION_STRING_LEN];
        CHAR szVer2[MAX_VERSION_STRING_LEN];

        if((DRV_GetVersionString1(szVer1, MAX_VERSION_STRING_LEN) == DRVR_OK) &&
           (DRV_GetVersionString2(szVer2, MAX_VERSION_STRING_LEN) == DRVR_OK))
        {
            printk("\nMediaTek MT53XX Driver : %s (%s)\n", szVer1, szVer2);
        }
    }
#endif //INIT_SHOW_VER

    x_os_drv_set_timestamp("adapt_reg_init start");

    if (target_country != -1)
    {
        SetTargetCountry(target_country);
        printk("Set target country to %d\n", target_country);
    }

    _CB_Init();

    // System initialize
    BspMainTask();
#ifndef CONFIG_TV_DRV_VFY

#ifdef MT53XX_I2C_MODULE
        mtk_i2c_init();
#endif //MT53XX_I2C_MODULE
    FBM_Init();

	// Register eeprom devices
	eeprom_module_init();

#ifdef REPLACE_EEPROM_WITH_FLASH
	SIF_Init();//for PDWNC get wake up reason fail when EEP2Flash=true
#endif

	//may use pdwnc module relate
    PDWNC_Init();
#ifndef CC_L_PANEL_FLOW
    if(panel_id != -1)
    {
        SelectPanel(panel_id);
        Printf("Set Panel id to %d\n", panel_id);
    }
    else
    {
        //panel index should be ready before fb_init
        LoadPanelIndex();
    }
    if (panel_early_init != -1)
    {
        HANDLE_T hThread;

        x_thread_create(&hThread, "DrvPanelPowerOnParallelInitInsmod", 
                2048,                          
                100,                           
                _ParallelInitThread, 0, NULL);
    }
#if 0
    x_thread_create(&hThread1, "DrvAudPowerOnParallelInitInsmod", 
    		2048,						   
    		100,						   
    		_AudParallelInitThread, 0, NULL);
#endif
#else //L_PANEL_FLOW
    if (panel_id != -1)
    {
        SelectPanel(panel_id);
        LDR_PmxDisplayHWInit();
    }
#endif    //L_PANEL_FLOW
	// PTP initial and register interrupt
	#if defined(CC_MT5890) || defined( CC_MT5861)
	 vDrvThermalInit();
	#endif

//#ifdef MT53XX_FB_MODULE
    _mt53fb_init();
//#endif //MT53XX_FB_MODULE
#endif //#ifndef CONFIG_TV_DRV_VFY
    // Register nodes and create devices
    for (i = 0; i < ARRAY_SIZE(devlist); i++)
    {
        dev = MKDEV(devlist[i].major, devlist[i].minor);
        ret = register_chrdev_region(dev, devlist[i].numbers, devlist[i].name);
        if (ret)
            goto error;

        cdev_init(&cdev_data[i], devlist[i].fops);
        kobject_set_name(&cdev_data[i].kobj, devlist[i].name, devlist[i].numbers);
        ret = cdev_add(&cdev_data[i], dev, devlist[i].numbers);
        if (ret)
        {
        error:
            printk("unable to get device id %d:%d for %s devs\n", devlist[i].major, devlist[i].minor,
                   devlist[i].name);
        }
    }

    // Register FBM blk devices.
#ifndef CONFIG_TV_DRV_VFY
    fbm_blkdev_init();
    
#ifdef ANDROID
    mtkinp_events_init();
#endif    

#endif //#ifndef CONFIG_TV_DRV_VFY

#ifdef CC_ENABLE_REBOOT_REASON
       set_reset_reason_init();
#endif

    x_os_drv_set_timestamp("adapt_reg_init done");
    
    return 0;
}

#ifndef CC_BUILTIN
fs_initcall(mtk_init);
#else
late_initcall(mtk_init);		//zanyun.wang modify, mtk_init should init later than emmc init when dtv_driver builtin 
#endif

#ifndef CONFIG_TV_DRV_VFY
module_param(osd1_src_width, int, 0444);
module_param(osd1_src_height, int, 0444);
module_param(osd1_output_width, int, 0444);
module_param(osd1_output_height, int, 0444);
module_param(osd2_src_width, int, 0444);
module_param(osd2_src_height, int, 0444);
module_param(osd2_output_width, int, 0444);
module_param(osd2_output_height, int, 0444);
#endif
module_param(panel_early_init, int, 0444);
module_param(target_country, int, 0444);
module_param(panel_id, int, 0444);


#ifdef CC_ENABLE_REBOOT_REASON
static int set_reset_reason_handler(struct notifier_block *self, unsigned long event, void *data)
{
	#if 0
    switch(event){
        case SYS_RESTART:
        {
            int len = strlen((const char *)data);
            UINT32 u4RebootOffset, u4RebootSize;

            if (DRVCUST_InitQuery(eRebootEepromAddress, &u4RebootOffset) || DRVCUST_InitQuery(eRebootEepromSize, &u4RebootSize))
                return -EINVAL;
            
            if (len >= u4RebootSize)
                return -EINVAL;

            UNUSED(EEPROM_Write(u4RebootOffset, (UINT32)(void *)data, len+1));
            return 0;
        }
        default:
            return 0;
    }
	#endif
    return 0;
}

static struct notifier_block reset_reason_notifyer = {
    .notifier_call = set_reset_reason_handler,
    .next = NULL,
    .priority = INT_MIN,
};

static int set_reset_reason_init(void)
{
    register_reboot_notifier(&reset_reason_notifyer);
    return 0;
}
static int set_reset_reason(const char *val, const struct kernel_param *kp)
{
    //write reset reason to misc(@mach-mt53xx/core.c) instead of eeprom
    #if 0
    int len = strlen(val);
    UINT32 u4RebootOffset, u4RebootSize;

    if (DRVCUST_InitQuery(eRebootEepromAddress, &u4RebootOffset) || DRVCUST_InitQuery(eRebootEepromSize, &u4RebootSize))
        return -EINVAL;
    
    if (len >= u4RebootSize)
        return -EINVAL;

    UNUSED(EEPROM_Write(u4RebootOffset, (UINT32)(void *)val, len+1));
	#endif
    return 0;
}

static struct kernel_param_ops reset_reason_ops = {
    .set = set_reset_reason,
};
  
module_param_cb(reset_reason, &reset_reason_ops, 0, 0200);
#endif /* CC_ENABLE_REBOOT_REASON */

