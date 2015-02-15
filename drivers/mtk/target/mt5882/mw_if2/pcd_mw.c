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
 * $IRRCSfile$ pcd_mw.h,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 * $CCRevision: $
 * $SWAuthor: $
 * $MD5HEX: $
 *
 * Description:
 *         This header file contains implementation Middleware interface for
 *         Power Control driver.
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
                    include files
 ----------------------------------------------------------------------------*/
#include "x_typedef.h"
#include "x_rm_dev_types.h"
#include "x_rm.h"
#include "x_pwr_ctrl.h"
#include "x_dram.h"
#include "x_pdwnc.h"
#include "x_bim.h"
#include "x_ckgen.h"
//#include "x_pinmux.h"
#include "x_assert.h"
#include "eeprom_if.h"
#include "storage_if.h"
#include "ir_if.h"
#include "drvcust_if.h"
#include "rtc_if.h"
#include "x_gpio.h"
#include "x_debug.h"
#include "sif_if.h"
#include "sif_sw_if.h"
#include "pdwnc_if.h"
#include "nor_if.h"
#include "u_drv_cust.h"
#ifndef CC_UP8032_ATV
#include "x_ldr_env.h"
#endif
#ifndef CC_USB_DISABLE
//#include "musb_if.h"
#endif
#ifdef LINUX_TURNKEY_SOLUTION
//#include "cb_data_rm.h"
//#include "cb_param.h"
#endif

#include "mtperipheral.h"
//#include "x_os.h"
#include "mtdrvcust.h"

#ifdef __KERNEL__
#include <mach/memory.h>
#endif


/*-----------------------------------------------------------------------------
                    configurations
 ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/
#define PCD_DRV_ID          0
#define PCD_COMP_NAME       DRVN_PWR_CTRL

#ifndef PC_MISC_CTRL_COMPANION_CHIP_UPGRADE
#define PC_MISC_CTRL_COMPANION_CHIP_UPGRADE             (1U << 5)
#endif /* PC_MISC_CTRL_COMPANION_CHIP_UPGRADE */

#ifndef PC_MISC_CTRL_RTC_QUIET_BOOT
#define PC_MISC_CTRL_RTC_QUIET_BOOT                     (1U << 6)
#endif /* PC_MISC_CTRL_RTC_QUIET_BOOT */

#define GPIO(x)                 (x)
#define OPCTRL(x)               (OPCTRL0 + (x))
#define OPCTRL0                 (200)       ///< define gpio macro for OPCTRL0

/*-----------------------------------------------------------------------------
        External function.
 ----------------------------------------------------------------------------*/
//extern void IRRC_DisconnectMiddleware(void);
//extern void IRRC_WaitPowerKeyReboot(void);
//extern UINT32 IRRC_IsPowerButtonOff(void);
//extern void vHDMISwitchInit(void);
/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
static BOOL     b_pcd_init = FALSE;
static UINT64   _u8UtcWakeup = 0;
// static UINT64   _u8UtcPowerDown = 0;
static BOOL     _fgVGAWakeup = FALSE;
static BOOL     _fgUartNormalWakeup = FALSE;
static PC_NFY_INFO_T _rPCNfyInfo[PC_WAKE_UP_REASON_UART + 1];
static UINT64  _u8GpioModeCtrlStatus    =0;
static PC_NFY_INFO_T    _rGpioMonitorNfyInfo ={NULL, NULL};
static UINT32   _u4TheInitKey = 0;
static PC_WAKE_UP_REASON_T  _rPCWakeupReason = (PC_WAKE_UP_REASON_T)0;
static UINT32 u4WakeupReason = PC_WAKE_UP_REASON_UNKNOWN;
static UINT32 g_u4AdvSifClkDiv = 0x100;
static UINT32 _u4PCDInitEepromFlags = 0;
static PC_WAKE_UP_INFO_T _rPCDWakeupReason = { PC_WAKE_UP_REASON_UNKNOWN, NULL };
static PC_IRRC_WAKKEY_T _rPCDWakeupIrKey = {BTN_NONE , PC_IRRX_WAKEUPKEY_NONE };
static BOOL _fgRtcAlertTimeSet = FALSE; // To indicate whether Alarm time has been set after wakeup
static PDWNC_WAKE_UP_REASON_T _rPCSetReason = PDWNC_WAKE_UP_REASON_UNKNOWN;
static MTPDWNC_SET_POWER_DOWN_REASON_T _rPCSetDownReason;
#if 0 /* cliu, this is unused */
static UINT8 _szLdrVer[64];
#endif
#ifdef ANDROID_TWO_WORLDS
static PC_NFY_INFO_T _rWakeupTimeSetupNfyInfo ={NULL, NULL};
#endif
/*-----------------------------------------------------------------------------
                    function declarations
 ----------------------------------------------------------------------------*/
void _GpioPolarityChangeNotify_Isr(INT32 i4Gpio, BOOL fgStatus)
{
    UINT32 u4Data;
    ASSERT ( i4Gpio >= 0 );

    u4Data = i4Gpio & 0x0000FFFF;
    u4Data |=  (((UINT32)(fgStatus?1:0))<<16);

/*  ASSERT( _rGpioMonitorNfyInfo.pv_tag !=NULL );   */
    ASSERT( _rGpioMonitorNfyInfo.pf_nfy != NULL );

//_rGpioMonitorNfyInfo.pf_nfy( _rGpioMonitorNfyInfo.pv_tag , PC_COND_WAKE_UP_EXPIRED, u4Data );
_rGpioMonitorNfyInfo.pf_nfy( _rGpioMonitorNfyInfo.pv_tag , PC_COND_GPIO_UPDATED, u4Data );

    Printf( "[_GpioPolarityChangeNotify_Isr] GPIO %d's polarity =%d\n", i4Gpio, fgStatus );
}
static void _GpioNotify_Isr(INT32 i4Gpio, BOOL fgStatus, VOID* pvArg)
{
	UNUSED(pvArg);
	_GpioPolarityChangeNotify_Isr(i4Gpio, fgStatus);

}

static BOOL _PCDInit(void);
// static void _PCDPowerDownHandler(UINT16 u2Dummy);
static UINT32 _PDGetEepromFlags(void);
static void _PCDSetEepromFlag(UINT32 u4Flags);

/*
static void _PCDPowerDownHandler(UINT16 u2Dummy)
{
    UNUSED(u2Dummy);
    Printf("Power down time up. send the power down key.\n");
    VERIFY(IRRX_SendMtkIr(BTN_POWER | KEY_SRC_SW)==0);
}
*/

static UINT32 _PDGetEepromFlags(void)
{
    UINT32 u4Size, u4Val;
    INT32 i4Ret;
    EEPDTVCFG_T rDtvCfg;
    x_memset(&rDtvCfg, 0, sizeof(EEPDTVCFG_T));

    u4Size = MTDRVCUST_InitGet(eEepromDtvDriverSize);
    ASSERT(u4Size >= sizeof(EEPDTVCFG_T));
    if (u4Size < sizeof(EEPDTVCFG_T))
    {
        Printf("DTV EEPROM size is %d\n", u4Size);
    }

    i4Ret = MTEEPDTV_GetCfg(&rDtvCfg);
    ASSERT(i4Ret==0);
    if (i4Ret!=0)
    {
        Printf("DTV EEPROM read failure.\n");
    }

    if ((rDtvCfg.u1Magic1 != DTVCFG_MAGIC_CHAR1) ||
        (rDtvCfg.u1Magic2 != DTVCFG_MAGIC_CHAR2))
    {
        // No data, Enter standby
        return 0;
    }

    u4Val = 0;

    if (rDtvCfg.u1Flags & DTVCFG_FLAG_POWERON_BOOT)
    {
        u4Val |= PC_MISC_CTRL_NO_STANDBY;
    }
    if (rDtvCfg.u1Flags & DTVCFG_FLAG_FACTORY_MODE)
    {
        u4Val |= PC_MISC_CTRL_FACTORY_MODE;
    }
    if (rDtvCfg.u1Flags & DTVCFG_FLAG_POWER_LEDOFF)
    {
        u4Val |= PC_MISC_CTRL_POWER_LED_OFF;
    }
    if (rDtvCfg.u1Flags & DTVCFG_FLAG_POWERON_LAST)
    {
        u4Val |= PC_MISC_CTRL_NO_STANDBY_2;
    }
    if (rDtvCfg.u1Flags & DTVCFG_FLAG_USB_UPGRADE_ON)
    {
        u4Val |= PC_MISC_CTRL_FIRMWARE_UPGRADE;
    }
    if (rDtvCfg.u1Flags & DTVCFG_FLAG_8280_UPGRADE_ON)
    {
        u4Val |= PC_MISC_CTRL_COMPANION_CHIP_UPGRADE;
    }
    if (rDtvCfg.u1Flags2 & DTVCFG_FLAG2_QUIET_BOOT)
    {
        u4Val |= PC_MISC_CTRL_RTC_QUIET_BOOT;
    }

    if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_ROOTFS_BANK)
    {
        u4Val |= PC_MISC_CTRL_DUAL_BANK_ROOTFS;
    }
    if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_KERNEL_BANK)
    {
        u4Val |= PC_MISC_CTRL_DUAL_BANK_KERNEL;
    }

    if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_FAST_BOOT)
    {
        u4Val |= PC_MISC_CTRL_FAST_BOOT;
    }

    if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_NORMAL_BOOT_WITH_SUSPEND)
    {
        u4Val |= PC_MISC_CTRL_NORMAL_BOOT_WITH_SUSPEND;
    }

    if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_NORMAL_BOOT_WITHOUT_SUSPEND)
    {
        u4Val |= PC_MISC_CTRL_NORMAL_BOOT_WITHOUT_SUSPEND;
    }
    #ifdef CC_MAPLE_HOTEL_MODE
    if(rDtvCfg.u1Flags4 & DTVCFG_FLAG4_HOTEL_MODE)
    {
	u4Val |= PC_MISC_CTRL_APP_HOTEL_MODE ;
    }
    //joe_20140328
    #endif
    return u4Val;
}

static UINT32 _PCDWriteDtvCfg(EEPDTVCFG_T *prDtvCfg, UINT32 u4Flags)
{
    UINT32 u4Ret;
    UINT8 u1OrgFlags, u1OrgFlags2, u1OrgFlags3;
    #ifdef CC_MAPLE_HOTEL_MODE
    UINT8 u1OrgFlags4;
    #endif
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
    UINT32 u4BootModeMuxCheck;
#endif
#endif
    u4Ret = 0;
    if ((prDtvCfg->u1Magic1 != DTVCFG_MAGIC_CHAR1) ||
        (prDtvCfg->u1Magic2 != DTVCFG_MAGIC_CHAR2))
    {
        prDtvCfg->u1Magic1 = DTVCFG_MAGIC_CHAR1;
        prDtvCfg->u1Magic2 = DTVCFG_MAGIC_CHAR2;
//        prDtvCfg->u1PanelIdx = 0;
        prDtvCfg->u1Flags = 0; // (0xffU & (~DTVCFG_FLAG_POWERON_LAST));
        prDtvCfg->u1Flags2 = 0;
        prDtvCfg->u1Flags3 = 0;
        u4Ret = 1;
    }

    u1OrgFlags = prDtvCfg->u1Flags;
    u1OrgFlags2 = prDtvCfg->u1Flags2;
    u1OrgFlags3 = prDtvCfg->u1Flags3;
    #ifdef CC_MAPLE_HOTEL_MODE
    u1OrgFlags4 = prDtvCfg->u1Flags4;
    if(u4Flags & PC_MISC_CTRL_APP_HOTEL_MODE)
    {
        Printf("joe._PCDWriteDtvCfg_____PC_MISC_CTRL_APP_HOTEL_MODE____\n");
	prDtvCfg->u1Flags4 |= DTVCFG_FLAG4_HOTEL_MODE;
    }
    else
    {
        prDtvCfg->u1Flags4 &= ~((UINT8)DTVCFG_FLAG4_HOTEL_MODE);
    }
    #endif
    if (u4Flags & PC_MISC_CTRL_NO_STANDBY)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_POWERON_BOOT;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_POWERON_BOOT);
    }

    if (u4Flags & PC_MISC_CTRL_FACTORY_MODE)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_FACTORY_MODE;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_FACTORY_MODE);
    }

    if (u4Flags & PC_MISC_CTRL_POWER_LED_OFF)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_POWER_LEDOFF;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_POWER_LEDOFF);
    }

    if (u4Flags & PC_MISC_CTRL_NO_STANDBY_2)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_POWERON_LAST;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_POWERON_LAST);
    }

    if (u4Flags & PC_MISC_CTRL_FIRMWARE_UPGRADE)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_USB_UPGRADE_ON;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_USB_UPGRADE_ON);
    }

    if (u4Flags & PC_MISC_CTRL_COMPANION_CHIP_UPGRADE)
    {
        prDtvCfg->u1Flags |= DTVCFG_FLAG_8280_UPGRADE_ON;
    }
    else
    {
        prDtvCfg->u1Flags &= ~((UINT8)DTVCFG_FLAG_8280_UPGRADE_ON);
    }
    #ifndef CC_CUSTOM_CONTROL_WAKEUP
    if (u4Flags & PC_MISC_CTRL_RTC_QUIET_BOOT)
    {
        prDtvCfg->u1Flags2 |= DTVCFG_FLAG2_QUIET_BOOT;
    }
    else
    {
        prDtvCfg->u1Flags2 &= ~((UINT8)DTVCFG_FLAG2_QUIET_BOOT);
    }
    #endif
    if (u4Flags & PC_MISC_CTRL_DUAL_BANK_ROOTFS)
    {
        prDtvCfg->u1Flags3 |= DTVCFG_FLAG3_ROOTFS_BANK;
    }
    else
    {
        prDtvCfg->u1Flags3 &= ~(DTVCFG_FLAG3_ROOTFS_BANK);
    }

    if (u4Flags & PC_MISC_CTRL_DUAL_BANK_KERNEL)
    {
        prDtvCfg->u1Flags3 |= DTVCFG_FLAG3_KERNEL_BANK;
    }
    else
    {
        prDtvCfg->u1Flags3 &= ~(DTVCFG_FLAG3_KERNEL_BANK);
    }
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
    u4BootModeMuxCheck = 0;
#endif
#endif
    if (u4Flags & PC_MISC_CTRL_FAST_BOOT)
    {
        prDtvCfg->u1Flags3 |= DTVCFG_FLAG3_FAST_BOOT;
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
        u4BootModeMuxCheck += 1;
#endif
#endif
    }
    else
    {
        prDtvCfg->u1Flags3 &= ~(DTVCFG_FLAG3_FAST_BOOT);
    }

    if (u4Flags & PC_MISC_CTRL_NORMAL_BOOT_WITH_SUSPEND)
    {
        prDtvCfg->u1Flags3 |= DTVCFG_FLAG3_NORMAL_BOOT_WITH_SUSPEND;
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
        u4BootModeMuxCheck += 2;
#endif
#endif
    }
    else
    {
        prDtvCfg->u1Flags3 &= ~(DTVCFG_FLAG3_NORMAL_BOOT_WITH_SUSPEND);
    }

    if (u4Flags & PC_MISC_CTRL_NORMAL_BOOT_WITHOUT_SUSPEND)
    {
        prDtvCfg->u1Flags3 |= DTVCFG_FLAG3_NORMAL_BOOT_WITHOUT_SUSPEND;
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
        u4BootModeMuxCheck += 4;
#endif
#endif
    }
    else
    {
        prDtvCfg->u1Flags3 &= ~(DTVCFG_FLAG3_NORMAL_BOOT_WITHOUT_SUSPEND);
    }
#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
    if((u4BootModeMuxCheck != 1) && (u4BootModeMuxCheck != 2) && (u4BootModeMuxCheck != 4))
    {
        LOG(0,"Error boot mode paramters\n");
#if 0
        if(u4BootModeMuxCheck == 0)
        {
            LOG(0,"Boot Mode is not set!");
        }
        else
        {
            LOG(0,"Only one boot mode can be set!");
            if(u4BootModeMuxCheck & (0x00000001))
            {
                LOG(0,"DTVCFG_FLAG3_FAST_BOOT is set!");
            }
            if(u4BootModeMuxCheck & (0x00000002))
            {
                LOG(0,"DTVCFG_FLAG3_NORMAL_BOOT_WITH_SUSPEND is set!");
            }
            if(u4BootModeMuxCheck & (0x00000004))
            {
                LOG(0,"DTVCFG_FLAG3_NORMAL_BOOT_WITHOUT_SUSPEND is set!");
            }
        }
        ASSERT(0);
#endif
    }
#endif
#endif
    #ifdef CC_MAPLE_HOTEL_MODE
    if ((u1OrgFlags != prDtvCfg->u1Flags) || (u1OrgFlags2 != prDtvCfg->u1Flags2) || (u1OrgFlags3 != prDtvCfg->u1Flags3) ||(u1OrgFlags4 != prDtvCfg->u1Flags4))
    #else
    if ((u1OrgFlags != prDtvCfg->u1Flags) || (u1OrgFlags2 != prDtvCfg->u1Flags2) || (u1OrgFlags3 != prDtvCfg->u1Flags3))
    #endif
    {
        u4Ret = 1;
    }

    return u4Ret;
}

static void _PCDSetEepromFlag(UINT32 u4Flags)
{
    UINT32 u4Size;
    INT32 i4Ret;
    EEPDTVCFG_T rDtvCfg;

    u4Size = MTDRVCUST_InitGet(eEepromDtvDriverSize);
    ASSERT(u4Size >= sizeof(EEPDTVCFG_T));
    UNUSED(u4Size);

    i4Ret = MTEEPDTV_GetCfg(&rDtvCfg);
    ASSERT(i4Ret==0);
    UNUSED(i4Ret);
    if (_PCDWriteDtvCfg(&rDtvCfg, u4Flags))
    {
        i4Ret = MTEEPDTV_SetCfg(&rDtvCfg);
        ASSERT(i4Ret==0);
    }
}

static INT32 _PCD_XferGpio(UINT32 ui4_id)
{
    UINT32 u4Val;

    if (ui4_id & PC_GPIO_TYPE_OP_CTRL)
    {
        u4Val = (ui4_id & ~(PC_GPIO_TYPE_OP_CTRL));
        if (u4Val >= TOTAL_OPCTRL_NUMBER)
        {
            ASSERT(0);
            return -1;
        }
        return (INT32)(OPCTRL(u4Val));
    }
    return (INT32)(GPIO(ui4_id));
}


/*-----------------------------------------------------------------------------
 * Name: _PCDSetAlertHandler
 *
 * Description: This API implements Middleware set functionality for Power
 *              Control driver.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
void _PCDSetPCNfyFct(PC_NFY_INFO_T* prInfo, PC_WAKE_UP_REASON_T eReason)
{
    ASSERT(prInfo->pf_nfy != NULL);
    if (eReason > PC_WAKE_UP_REASON_UART)
    {
        return;
    }
    _rPCNfyInfo[(UINT32)eReason].pf_nfy = prInfo->pf_nfy;
    _rPCNfyInfo[(UINT32)eReason].pv_tag = prInfo->pv_tag;
}


/*-----------------------------------------------------------------------------
 * Name: _PCDAlertHandler
 *
 * Description: This API implements Middleware set functionality for Power
 *              Control driver.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
void _PCDRTCNfyFct(void)
{
    UINT32 u4Data = 0;
    UINT64 u8Utc;
    MTRTC_T rRtc = {0,0,0,0,0,0,0};
    UNUSED(MTRTC_GetTimeDate(&u8Utc));	
	UNUSED(MTRTC_UtcToRtc(&rRtc, u8Utc));
    LOG(7,"[_PCDRTCNfyFct_RTC] %2d/%2d/%2d %2d:%2d:%2d\n",
    	rRtc.u1Year,
    	rRtc.u1Month,
   	    rRtc.u1Day,
    	rRtc.u1Hour,
    	rRtc.u1Minute,
    	rRtc.u1Second
    	);
    if(_u8UtcWakeup != 0 && _rPCNfyInfo[PC_WAKE_UP_REASON_RTC].pf_nfy != NULL)
    {
        _rPCNfyInfo[PC_WAKE_UP_REASON_RTC].pf_nfy(_rPCNfyInfo[PC_WAKE_UP_REASON_RTC].pv_tag, PC_COND_WAKE_UP_EXPIRED, u4Data);		
    }
}
/*-----------------------------------------------------------------------------
 * Name: _CheckWakeupReason
 *
 * Description: This API implements to check the key on/off at initial time.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
#if 0	////#ifdef MW_IF2_TODO
static void _CheckWakeupReason()		
{
    INT32 i, i4Val;
    UINT32 u4TheKey, u4Val;
    SGL_GPIO_T  *prSingleGpioKeyList = NULL;
    SGL_SERVO_T *prSingleServoKeyList = NULL;
    GREED_KEY_T *prGreedGpioKeyList = NULL;

    if (DRVCUST_OptQuery(ePcdWakeupTheKey, &u4TheKey) < 0)
    {
        return;
    }
    if (DRVCUST_OptQuery(eSingleGpioKeyList, (UINT32 *)(void *)&prSingleGpioKeyList) < 0)
    {
        prSingleGpioKeyList = NULL;
    }
    if (DRVCUST_OptQuery(eSingleServoKeyList, (UINT32 *)(void *)&prSingleServoKeyList) < 0)
    {
        prSingleServoKeyList = NULL;
    }
    if (DRVCUST_OptQuery(eGreedGpioKeyList, (UINT32 *)(void *)&prGreedGpioKeyList) < 0)
    {
        prGreedGpioKeyList = NULL;
    }

    i = 0;
    while ((prSingleGpioKeyList) &&
           (prSingleGpioKeyList[i].u4Key != 0) &&
           (prSingleGpioKeyList[i].u4Key != DKBIT_NONE))
    {
        if ((UINT32)GPIO_Input((INT32)prSingleGpioKeyList[i].u4Gpio) == prSingleGpioKeyList[i].u4Val)
        {
            _u4TheInitKey = prSingleGpioKeyList[i].u4Key;
            if (prSingleGpioKeyList[i].u4Key == u4TheKey)
            {
                _rPCWakeupReason = PC_WAKE_UP_REASON_CUSTOM_1;
                return;
            }
        }
        i++;
    }

    i = 0;
    while ((prSingleServoKeyList) &&
           (prSingleServoKeyList[i].u4Key != 0) &&
           (prSingleServoKeyList[i].u4Key != DKBIT_NONE))
    {
        u4Val = IRRX_ReadSvadc(prSingleServoKeyList[i].u4Servo);
        if ((prSingleServoKeyList[i].u4Top > u4Val) &&
            (u4Val >= prSingleServoKeyList[i].u4Btm))
        {
            _u4TheInitKey = prSingleServoKeyList[i].u4Key;
            if (prSingleServoKeyList[i].u4Key == u4TheKey)
            {
                _rPCWakeupReason = PC_WAKE_UP_REASON_CUSTOM_1;
                return;
            }
        }
        i++;
    }

    i = 0;
    while ((prGreedGpioKeyList) &&
           (prGreedGpioKeyList[i].u4Key != 0) &&
           (prGreedGpioKeyList[i].u4Key != DKBIT_NONE))
    {
        i4Val = (INT32)prGreedGpioKeyList[i].u4Val;
        VERIFY(i4Val==GPIO_Output((INT32)prGreedGpioKeyList[i].u4GpioOut, &i4Val));
        //if (i4Val == GPIO_Input((INT32)prGreedGpioKeyList[i].u4GpioIn))
        if ((INT32)prGreedGpioKeyList[i].u4Val == GPIO_Input((INT32)prGreedGpioKeyList[i].u4GpioIn))
        {
            _u4TheInitKey = prGreedGpioKeyList[i].u4Key;
            if (prGreedGpioKeyList[i].u4Key == u4TheKey)
            {
                _rPCWakeupReason = PC_WAKE_UP_REASON_CUSTOM_1;
                return;
            }
        }
        i++;
    }
}
#endif

/*-----------------------------------------------------------------------------
 * Name: _PCDInit
 *
 * Description: This API initializes PCD driver and hardwre.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns: TRUE    Succeed
 *          FALSE   Fail
 ----------------------------------------------------------------------------*/
static BOOL _PCDInit(void)
{
    INT32 i4Val, fgWrite;
    EEPDTVCFG_T rDtvCfg;
    x_memset(&rDtvCfg, 0, sizeof(EEPDTVCFG_T));

    // Record the initial eeprom value for QuietBoot.
    _u4PCDInitEepromFlags = _PDGetEepromFlags();

    // Set System state on.
    i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
    UNUSED(i4Val);
    ASSERT(i4Val==0);
    fgWrite = 0;

    if ((rDtvCfg.u1Magic1 != DTVCFG_MAGIC_CHAR1) ||
        (rDtvCfg.u1Magic2 != DTVCFG_MAGIC_CHAR2))
    {
        // default flag value is 0.
        fgWrite = _PCDWriteDtvCfg(&rDtvCfg, 0);
    }

#ifndef ADVANCED_STANDBY_CTRL
    if (MTDRVCUST_OptGet(eStdbyUsePrevPowerState) &&
        (!(rDtvCfg.u1Flags & DTVCFG_FLAG_SYSTEM_STATE_ON)))
#else /* ADVANCED_STANDBY_CTRL */
    if (!(rDtvCfg.u1Flags & DTVCFG_FLAG_SYSTEM_STATE_ON))
#endif /* ADVANCED_STANDBY_CTRL */
    {
        rDtvCfg.u1Flags |= (DTVCFG_FLAG_SYSTEM_STATE_ON);
        fgWrite = 1;
    }

    if (MTDRVCUST_OptGet(eStdbyUsePowerBtnCount) &&
        (!(rDtvCfg.u1Flags & DTVCFG_FLAG_POWER_BTN_ON)))
    {
        rDtvCfg.u1Flags |= (DTVCFG_FLAG_POWER_BTN_ON);
        fgWrite = 1;
    }

    if (fgWrite)
    {
        i4Val = MTEEPDTV_SetCfg(&rDtvCfg);
        ASSERT(i4Val==0);
    }

    return TRUE;
}

UINT32 PCDSetAdvSifClkDiv(UINT32 u4Clk)
{
    UINT32 u4OldClk;

    u4OldClk = g_u4AdvSifClkDiv;
    g_u4AdvSifClkDiv = u4Clk;
    return u4OldClk;
}

void _MW_PCDCheckInit(void)
{
    while(!b_pcd_init);
}

/*-----------------------------------------------------------------------------
 * Name: PCDGet
 *
 * Description: This API implements Middleware get functionality for Power
 *              Control driver.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
INT32 PCDGet(DRV_COMP_ID_T* pt_comp_id,
             DRV_GET_TYPE_T e_get_type,
             VOID*          pv_get_info,
             SIZE_T*        pz_get_info_len)
{
    SIZE_T          t_get_info_len;
    UINT64          u8Utc;
    BOOL            fgRet;
    INT32           i4Gpio, i4Val, u4Clk;
    UINT32          u4Val = 0, u4ValNew = 0;
    PC_SIF_CH_ID_T  e_channel_id;
    EEPDTVCFG_T rDtvCfg;
    UINT32 u4Reason = PC_WAKE_UP_REASON_UNKNOWN;
    PC_GPIO_MONITOR_CTRL_EX_T GpioMonitor;
#ifdef SYS_WIFI_BGM	
    UINT8 u1StoreDemo = 0;    
    UINT8 u1Factory = 0;
    UINT8 u1DIAG=0;
    UINT8 u1Watchdog0=0;
    UINT32 u4Addr = 0;
#endif

    /* Check aguments */
    if (!b_pcd_init)
    {
        return RMR_DRV_INV_GET_INFO;
    }

    if ((pt_comp_id == NULL) ||
        (pt_comp_id->e_type != DRVT_PWR_CTRL) ||
        (pt_comp_id->ui2_id != PCD_DRV_ID))
    {
        return RMR_DRV_INV_GET_INFO;
    }

    /* Perform get opertion */
    switch (e_get_type)
    {
        case PC_GET_TYPE_WAKE_UP_CAP:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }

            *(UINT32 *)pv_get_info = (PC_WAKE_UP_CAP_IRRC | PC_WAKE_UP_CAP_FP
                                        | PC_WAKE_UP_CAP_VGA | PC_WAKE_UP_CAP_UART);
            fgRet = MTRTC_GetTimeDate(&u8Utc);
            if (fgRet != 0)
			{
				return RMR_DRV_INV_GET_INFO;
			}
			else			
            {
                *(UINT32 *)pv_get_info |= (PC_WAKE_UP_CAP_RTC);
            }
            break;

        case PC_GET_TYPE_WAKE_UP_CTRL_CAP:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            *(UINT32 *)pv_get_info = (PC_WAKE_UP_CAP_VGA | PC_WAKE_UP_CAP_RTC);
            break;

        case PC_GET_TYPE_WAKE_UP_SETUP:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            *(UINT32 *)pv_get_info = (PC_WAKE_UP_CAP_IRRC | PC_WAKE_UP_CAP_FP);
            *(UINT32 *)pv_get_info |= (_fgUartNormalWakeup) ? PC_WAKE_UP_CAP_UART : 0;
            *(UINT32 *)pv_get_info |= (_u8UtcWakeup) ? (PC_WAKE_UP_CAP_RTC) : 0;
            *(UINT32 *)pv_get_info |= (_fgVGAWakeup) ? (PC_WAKE_UP_CAP_VGA) : 0;
            break;

        case PC_GET_TYPE_WAKE_UP_REASON:
            t_get_info_len = sizeof(PC_WAKE_UP_INFO_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }

            if (_rPCWakeupReason)
            {
                u4WakeupReason = _rPCWakeupReason;
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->e_reason = _rPCWakeupReason;
            }
            else if (u4WakeupReason == PC_WAKE_UP_REASON_UNKNOWN)
            {
                UNUSED(MTPDWNC_GetWakeupReason(&u4WakeupReason));//#ifdef MW_IF2_TODO
                if (u4WakeupReason == PDWNC_WAKE_UP_REASON_WATCHDOG)
                {
                    u4WakeupReason = PDWNC_WAKE_UP_REASON_IRRC;
                }
            }

            ((PC_WAKE_UP_INFO_T *)pv_get_info)->e_reason = (PC_WAKE_UP_REASON_T)u4WakeupReason;

#ifdef SYS_WIFI_BGM	
            //AC -on modify wake up reason for wifi BGM mode
            if (u4WakeupReason == PC_WAKE_UP_REASON_AC_POWER)
            {    
                if (MTR_OK == MTDRVCUST_OptQuery(eEepromStoreDemoAddr, &u4Addr))
				{
					MTEEPROM_Read(u4Addr, &u1StoreDemo, 1);    					
					u1StoreDemo = u1StoreDemo&0x03;
					if ((u1StoreDemo==0x01)||(u1StoreDemo==0x02))
					{
						u1StoreDemo = 1;
					}
                }
                    
                if (MTR_OK == MTDRVCUST_OptQuery(eEepromFactoryModeAddr, &u4Addr))
				{
					MTEEPROM_Read(u4Addr, &u1Factory, 1);
					u1Factory = u1Factory&0x01;
                }
                    
                if (MTR_OK == MTDRVCUST_OptQuery(eEepromDIAGFlagAddr, &u4Addr))
				{
					MTEEPROM_Read(u4Addr, &u1DIAG, 1);
	                u1DIAG = u1DIAG&0x01;
                }
                
                if ((u1StoreDemo==1) || (u1Factory==1) || (u1DIAG==1))
                {
                    ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = 0;
                    Printf(" StoreDemo and DIAG...  boot \n");
                }
                else
                {
                    ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = (void *)PC_WAKE_UP_SUB_REASON_CUSTOM_3 ;
                    _rPCDWakeupReason.pv_data=(void *)PC_WAKE_UP_SUB_REASON_CUSTOM_3 ;
                    Printf("wifi BGM mode set PC_WAKE_UP_SUB_REASON_CUSTOM_3 \n");
                    break;
                }
            }
            
            if (MTR_OK == MTDRVCUST_OptQuery(eEepromWatchdogFlagAddr, &u4Addr))
			{
				//For  watchdog reset need goto wifi BGM
				MTEEPROM_Read(u4Addr, &u1Watchdog0, 1);
				u1Watchdog0 = u1Watchdog0&0x01;
				if (u1Watchdog0 == 1)
				{
					((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = (void *)PC_WAKE_UP_SUB_REASON_CUSTOM_3 ;
					_rPCDWakeupReason.pv_data=(void *)PC_WAKE_UP_SUB_REASON_CUSTOM_3 ;
					Printf("watchdog0 reest goto wifi BGM mode set PC_WAKE_UP_SUB_REASON_CUSTOM_3\n");
					break;
				}
			}
            
            // AC-on or RTC on to check sub wakeup reason.
            if ((u4WakeupReason == PC_WAKE_UP_REASON_RTC)) //|| (u4WakeupReason == PC_WAKE_UP_REASON_AC_POWER))
            {
                // ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = (void *)((_PDGetEepromFlags() & PC_MISC_CTRL_RTC_QUIET_BOOT) ?
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = (void *)((_u4PCDInitEepromFlags & PC_MISC_CTRL_RTC_QUIET_BOOT) ?
                            PC_WAKE_UP_SUB_REASON_QUIET_BOOT : 0);
            }
            else
            {
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = 0;
            }
#else
            // AC-on or RTC on to check sub wakeup reason.
            if(((u4WakeupReason == PC_WAKE_UP_REASON_RTC)) || (u4WakeupReason == PC_WAKE_UP_REASON_AC_POWER))
            {
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = (void *)((_u4PCDInitEepromFlags & PC_MISC_CTRL_RTC_QUIET_BOOT) ?
                            PC_WAKE_UP_SUB_REASON_QUIET_BOOT : 0);
            }
            else
            {
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = 0;
            }

#endif		
            if (_rPCDWakeupReason.e_reason != PC_WAKE_UP_REASON_UNKNOWN)
            {
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->e_reason = _rPCDWakeupReason.e_reason;
                ((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data = _rPCDWakeupReason.pv_data;
            }

            Printf("WAKE_UP_REASON: 0x%08x - 0x%08x\n", (UINT32)(((PC_WAKE_UP_INFO_T *)pv_get_info)->e_reason),
                (UINT32)(((PC_WAKE_UP_INFO_T *)pv_get_info)->pv_data));
            break;
            
		case PC_GET_TYPE_WAKEUP_IRKEY:
			{
				MTIRRC_WAKKEY_T pWakeupKey;
				t_get_info_len = sizeof(PC_IRRC_WAKKEY_T);
	            if (*pz_get_info_len < t_get_info_len)
	            {
	                return RMR_DRV_NOT_ENOUGH_SPACE;
	            }
	            else
	            {
	                *pz_get_info_len = t_get_info_len;
	            }
				if(_rPCDWakeupIrKey.e_fgIrkey == PC_IRRX_WAKEUPKEY_NONE)
				{
					UNUSED(MTIR_GetWakeupIrKey(&pWakeupKey));
					((PC_IRRC_WAKKEY_T *)pv_get_info)->ui4_WakeupKey = pWakeupKey.u4WakeupKey;
					((PC_IRRC_WAKKEY_T *)pv_get_info)->e_fgIrkey     = pWakeupKey.fgIrkey;
					((PC_IRRC_WAKKEY_T *)pv_get_info)->ui4_Raw       = pWakeupKey.u4Raw;
				}
				else
				{
					((PC_IRRC_WAKKEY_T *)pv_get_info)->ui4_WakeupKey = _rPCDWakeupIrKey.ui4_WakeupKey;
					((PC_IRRC_WAKKEY_T *)pv_get_info)->e_fgIrkey     = _rPCDWakeupIrKey.e_fgIrkey;
				}
			}
			break;
		case PC_GET_TYPE_HOTKEY_NUM:
			{
				UINT8   u1HotkeyNum;
				t_get_info_len = sizeof(UINT8);
	            if (*pz_get_info_len < t_get_info_len)
	            {
	                return RMR_DRV_NOT_ENOUGH_SPACE;
	            }
	            else
	            {
	                *pz_get_info_len = t_get_info_len;
	            }
				UNUSED(MTIR_GetHotkeyNum(&u1HotkeyNum));
				*(UINT8 *)pv_get_info = u1HotkeyNum;
			}
			break;
		case PC_GET_TYPE_ALL_HOTKEY:
			{
				UINT32 *arHotkey;
				UINT8   u1HotkeyNum;
				t_get_info_len = sizeof(PC_IRRC_ALL_HOTKEY_T);
	            if (*pz_get_info_len < t_get_info_len)
	            {
	                return RMR_DRV_NOT_ENOUGH_SPACE;
	            }
	            else
	            {
	                *pz_get_info_len = t_get_info_len;
	            }
				arHotkey 	= ((PC_IRRC_ALL_HOTKEY_T *)pv_get_info)->pui4_hotkey;
				u1HotkeyNum = ((PC_IRRC_ALL_HOTKEY_T *)pv_get_info)->u1_keynum;
				UNUSED(MTIR_GetAllHotKey(arHotkey,u1HotkeyNum));
			}
			break;
        case PC_GET_TYPE_WAKE_UP_TIME:
            t_get_info_len = sizeof(TIME_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            // Read in RTC alert time causing wakeup
			UNUSED(MTPDWNC_GetWakeupReason(&u4Reason));//#ifdef MW_IF2_TODO
            if((u4Reason == PDWNC_WAKE_UP_REASON_RTC) && !_fgRtcAlertTimeSet)
            {
                UNUSED(MTRTC_GetAlertTime(&_u8UtcWakeup));
                _fgRtcAlertTimeSet = TRUE;
            }
            *(TIME_T *)pv_get_info = (TIME_T)_u8UtcWakeup;
            break;

        case PC_GET_TYPE_MISC_CTRL:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            *(UINT32 *)pv_get_info = _PDGetEepromFlags();
            break;

        case PC_GET_TYPE_MISC_STATUS:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
			UNUSED(MTPDWNC_PowerDownByAcCut(&u4Val));
            if (u4Val)
            {
                *(UINT32 *)pv_get_info = PC_MISC_STS_POWER_DOWN_BY_AC_CUT;
            }
            else
            {
                *(UINT32 *)pv_get_info = 0;
            }
            break;

        case PC_GET_TYPE_GPIO_MODE:
            t_get_info_len = sizeof(PC_GPIO_MODE_INFO_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }

            i4Gpio = _PCD_XferGpio(((PC_GPIO_MODE_INFO_T *)pv_get_info)->ui4_id);
            if (i4Gpio < 0)
            {
                return RMR_DRV_INV_GET_INFO;
            }

            UNUSED(MTGPIO_GetDirection(i4Gpio, (MTGPIO_DIR_T *)&u4Val));
            if (u4Val)
            {   // output mode.
                ((PC_GPIO_MODE_INFO_T *)pv_get_info)->e_mode = PC_GPIO_MODE_OUTPUT;
            }
            else
            {   // input mode.
                ((PC_GPIO_MODE_INFO_T *)pv_get_info)->e_mode = PC_GPIO_MODE_INPUT;
            }
            break;

        case PC_GET_TYPE_GPIO_STATUS:
            t_get_info_len = sizeof(PC_GPIO_STATUS_INFO_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }

            i4Gpio = _PCD_XferGpio(((PC_GPIO_STATUS_INFO_T *)pv_get_info)->ui4_id);
            if (i4Gpio < 0)
            {
                return RMR_DRV_INV_GET_INFO;
            }

            UNUSED(MTGPIO_GetDirection(i4Gpio, (MTGPIO_DIR_T *)&u4Val));
            if (u4Val)
            {   // output mode.
                UNUSED(MTGPIO_QueryOutVal(i4Gpio, &i4Val));
                ((PC_GPIO_STATUS_INFO_T *)pv_get_info)->e_status =
                    (i4Val ? PC_GPIO_STATUS_HIGH : PC_GPIO_STATUS_LOW);
            }
            else
            {   // input mode.
                UNUSED(MTGPIO_GetIn(i4Gpio, (UINT8*)&u4Val));
                ((PC_GPIO_STATUS_INFO_T *)pv_get_info)->e_status =
                    (u4Val ? PC_GPIO_STATUS_HIGH : PC_GPIO_STATUS_LOW);
            }
            break;

        case PC_GET_TYPE_SIF:
            t_get_info_len = sizeof(PC_SIF_INFO_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
			i4Val = MTSIF_ReadMultipleSubAddr(SIF_PORT_0, 100/*100khz*/, (UINT8)(((PC_SIF_INFO_T *)pv_get_info)->ui4_addr), 
												(UINT8 *)&(((PC_SIF_INFO_T *)pv_get_info)->ui4_offset),
												1, (UINT8 *)&(((PC_SIF_INFO_T *)pv_get_info)->ui1_data), 1 );
			if (i4Val < 0)
            { // failed
                //return RMR_DRV_ERROR;     //this could cause RM abort...
                return RMR_DRV_INV_GET_INFO;
            }

            break;

        case PC_GET_TYPE_SIF_ADVANCED:
            t_get_info_len = sizeof(PC_SIF_INFO_ADVANCED_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }

            i4Val = 0;  u4Clk = g_u4AdvSifClkDiv;
            e_channel_id = ((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->e_channel_id ;

            switch (e_channel_id)
            {
                case PC_SIF_CH_ID_EDID_BUS:
                    /* Do nothing */
                    break;
                case PC_SIF_CH_ID_SYS_BUS:
                    if( ((((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui1_offset_len)<=3) )  /* check sub-address length*/
                    {
                        LOG(7,"SIF_Read: SIF_BUS_SYSTEM_I2C ClkDiv:0x%x  ui4_addr:0x%02X  \n", u4Clk, ((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui4_addr);
						UNUSED(MTSIF_ReadMultipleSubAddr(SIF_PORT_0, (27000/u4Clk), 
															(UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui4_addr), 
															(UINT8 *)&(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui4_offset),
															((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui1_offset_len,
															(UINT8 *)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->pui1_data),
															(UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui1_data_len)));

            }
                    break;
                case PC_SIF_CH_ID_TUNER_BUS:
                    if((((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui1_offset_len) == 0x00 )  /* No sub-address */
                    {
                                                       
                        UNUSED(MTSIF_ReadMultipleSubAddr(SIF_PORT_TUNER, (27000/u4Clk),
                                                        (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui4_addr),
                                                         NULL, 0,
                                                        (UINT8 *)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->pui1_data),
                                                        (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui1_data_len)));
                        Printf("SIF_TunerReadNoSubAddr: ClkDiv:0x%x  ui4_addr:0x%02X  \n", u4Clk, ((PC_SIF_INFO_ADVANCED_T *)pv_get_info)->ui4_addr);
                    }
                    break;
            }

            break;

    case PC_GET_TYPE_SIF_BY_GPIO:
        {
            PC_SIF_INFO_BY_GPIO_T  SwSif_T = *(PC_SIF_INFO_BY_GPIO_T *)pv_get_info;
            UINT8 aru1SubAddr[3];   int i;
            UNUSED(MTSIFSW_Init( SwSif_T.ui4_data_gpio_id, SwSif_T.ui4_addr_gpio_id ));
            if(SwSif_T.ui1_offset_len>0)
            {
                for(i=SwSif_T.ui1_offset_len-1 ; i>=0  ;i--)    //sub-address
                {
                    if (i >= 3)
                    {
                        continue;
                    }
                    aru1SubAddr[i] = (UINT8) (SwSif_T.ui4_offset>>(i*8))&0xFF;
                }
                i4Val= MTSIFSW_WriteDataNoSubAddr( SwSif_T.ui4_addr , SwSif_T.ui1_offset_len , aru1SubAddr,
                                                                            0xff, 10 , 100);     //send sub-addr
                if( i4Val != 0)
                {
                    UNUSED(MTSIFSW_Remove());
                    return RMR_DRV_INV_GET_INFO;
                }
            }
            i4Val = MTSIFSW_ReadDataNoSubAddr(SwSif_T.ui4_addr , SwSif_T.ui1_data_len  , SwSif_T.pui1_data,
                                                                0xff, 10 , 100);     //read data back
            if( i4Val != 0)
            {
                UNUSED(MTSIFSW_Remove());
                return RMR_DRV_INV_GET_INFO;
            }
			UNUSED(MTSIFSW_Remove());
        }
            break;
	case PC_GET_TYPE_GPIO_QUERY:

        /* check validatity of parameter length */
        t_get_info_len = sizeof(PC_GPIO_MONITOR_CTRL_EX_T);
        if (*pz_get_info_len < sizeof(PC_GPIO_MONITOR_CTRL_EX_T) )
        {
            return RMR_DRV_NOT_ENOUGH_SPACE;
        }
        else
        {
            *pz_get_info_len = t_get_info_len;
        }
		GpioMonitor = *((PC_GPIO_MONITOR_CTRL_EX_T*)pv_get_info);
		i4Gpio = GpioMonitor.ui4_gpio;
		i4Val = 0;
		if(MTGPIO_IntrqQuery( i4Gpio, &i4Val) == 0)
		{
			if(i4Val)
			{
				GpioMonitor.b_monitor = TRUE;
			}
			else
			{
			 	GpioMonitor.b_monitor = FALSE;
			}
		}
		else
		{			
			return RMR_DRV_GET_FAILED;
		}
		  *((PC_GPIO_MONITOR_CTRL_EX_T*)pv_get_info) = GpioMonitor;
        break;
    case PC_GET_TYPE_GPIO_MONITOR_CTRL:
        VERIFY( *pz_get_info_len == 8 );
        /* check validatity of parameter length */
        t_get_info_len = sizeof(UINT64);
        if (*pz_get_info_len < sizeof(UINT64) )
        {
            return RMR_DRV_NOT_ENOUGH_SPACE;
        }
        else
        {
            *pz_get_info_len = t_get_info_len;
        }
        *((UINT64*)pv_get_info) = _u8GpioModeCtrlStatus;
        break;

        case PC_GET_TYPE_UPGRADE_CTRL:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
            if (i4Val)
            {
                return RMR_DRV_ERROR;
            }
            *(UINT32 *)pv_get_info = (UINT32) (rDtvCfg.u1Upgrade | ((UINT32)rDtvCfg.u1Upgrade_Ext<<8) );
            break;        
		case PC_GET_TYPE_UPGRADE_FROM_PARTITION_CTRL:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
            if (i4Val)
            {
                return RMR_DRV_ERROR;
            }
            *(UINT32 *)pv_get_info = (UINT32)(rDtvCfg.u1Upgrade_Ext<<8);
            break;
		#ifdef CC_QUIET_HOT_BOOT_SUPPORT	
		case PC_GET_TYPE_QUIET_HOT_BOOT_STATUS:
            t_get_info_len = sizeof(UINT32);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
            i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
            if (i4Val)
            {
                return RMR_DRV_ERROR;
            }
            if((rDtvCfg.u1Flags4 & (UINT8)DTVCFG_FLAG4_QUIET_HOT_BOOT)==0)
            {
                *(UINT32 *)pv_get_info = PC_QUIET_HOT_BOOT_DISABLE;
            }
            else
            {
                *(UINT32 *)pv_get_info = PC_QUIET_HOT_BOOT_ENABLE;
            } 			
            break;			
		#endif	

        case  PC_GET_TYPE_SERVO_AD:
            {
                  UINT32 u4Id, i = 100;
                t_get_info_len = sizeof(PC_SERVO_AD_INFO_T);
                if (*pz_get_info_len < t_get_info_len)
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
                else
                {
                    *pz_get_info_len = t_get_info_len;
                }
                u4Id = ((PC_SERVO_AD_INFO_T *)pv_get_info)->ui4_id;
                if (u4Id >= TOTAL_SERVOADC_NUMBER)
                {
                    ASSERT(0);
                    return RMR_DRV_ERROR;
                }
                do {
                    if(i-- == 0)
                    {
                        break;
                    }
                    UNUSED(MTPDWNC_ReadServoADCChannelValue(u4Id, (UINT32*)&u4Val));
					UNUSED(MTPDWNC_ReadServoADCChannelValue(u4Id, (UINT32*)&u4ValNew));
                } while (u4Val != u4ValNew);
                ((PC_SERVO_AD_INFO_T *)pv_get_info)->ui4_data = u4Val;
            }
            break;
        case PC_GET_TYPE_POWER_STATE:
		#if 0
            {
				PFN_POWER_STATE_QUERY_FUNC pfnQueryPowerStateFunc = NULL;
                t_get_info_len = sizeof(UINT32);
                if (*pz_get_info_len < t_get_info_len)
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
                else
                {
                    *pz_get_info_len = t_get_info_len;
                }
    
                if(MTDRVCUST_InitQuery(ePowerStateQueryFunc, (UINT32 *)(void *)&(pfnQueryPowerStateFunc)) < 0)
                {
                    *(UINT32 *)pv_get_info = PC_PWR_STATE_CTRL_NORMAL;//PDWNC_QueryPowerState();
                }
                else
                {
                    if (pfnQueryPowerStateFunc != NULL)
                    {
                        *(UINT32 *)pv_get_info = pfnQueryPowerStateFunc();
                    }
                }
            }
		#else
			{
				UINT32 u4PwrState = 0;
				t_get_info_len = sizeof(UINT32);
				if (*pz_get_info_len < t_get_info_len)
				{
					return RMR_DRV_NOT_ENOUGH_SPACE;
				}
				else
				{
					*pz_get_info_len = t_get_info_len;
				}
			
				if(MTPDWNC_GetPowerState(&u4PwrState) < 0)
				{
					*(UINT32 *)pv_get_info = PC_PWR_STATE_CTRL_NORMAL;
				}
				else
				{
					*(UINT32 *)pv_get_info = u4PwrState;
				}
			}
	
		#endif
            break;
            
        case PC_GET_TYPE_POWER_DOWN_REASON:
            t_get_info_len = sizeof(PDWNC_SET_POWER_DOWN_REASON_T);
            if (*pz_get_info_len < t_get_info_len)
            {
                return RMR_DRV_NOT_ENOUGH_SPACE;
            }
            else
            {
                *pz_get_info_len = t_get_info_len;
            }
			{
            PDWNC_SET_POWER_DOWN_REASON_T *pReason;
            pReason = (PDWNC_SET_POWER_DOWN_REASON_T *)pv_get_info;
            pReason->u4Reason = _rPCSetDownReason.u4Reason;
            pReason->u4Btn = _rPCSetDownReason.u4Btn;
            }
            break;
        // each IC will return different ID
        case PC_GET_TYPE_DEV_ID:
            {
                //extern INT32 BIM_GetSecureID(BYTE *pu1DeviceID, BYTE u1Size);
                PC_DEV_ID_T*  pt_dev_id = (PC_DEV_ID_T *) pv_get_info;
                
                if (*pz_get_info_len != sizeof (PC_DEV_ID_T))
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
                
                //if (BIM_GetSecureID (pt_dev_id->aui1_dev_id, 8) != 0)                
                if (MTBIM_GetDeviceID (pt_dev_id->aui1_dev_id) != 0)
                {
                    return RMR_DRV_GET_FAILED;
                }
            }
            break;
        case PC_GET_TYPE_WOL_CTRL:
            {
                t_get_info_len = sizeof(UINT32);
                if (*pz_get_info_len < t_get_info_len)
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
                else
                {
                    *pz_get_info_len = t_get_info_len;
                }

                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
                if (i4Val)
                {
                    return RMR_DRV_ERROR;
                }
                if((rDtvCfg.u1Flags3 & (UINT8)DTVCFG_FLAG3_WAKEUP_ON_LAN)==0)
                {
                    *(UINT32 *)pv_get_info = PC_ETHERNET_WOL_DISABLE;
                }
                else
                {
                    *(UINT32 *)pv_get_info = PC_ETHERNET_WOL_ENABLE;
                }  
            }
            break;
		 case PC_GET_TYPE_WIFI_WOL_CTRL:
            {
                t_get_info_len = sizeof(UINT32);
                if (*pz_get_info_len < t_get_info_len)
                {
                    return RMR_DRV_NOT_ENOUGH_SPACE;
                }
                else
                {
                    *pz_get_info_len = t_get_info_len;
                }

                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
                if (i4Val)
                {
                    return RMR_DRV_ERROR;
                }
                if((rDtvCfg.u1Flags4 & (UINT8)DTVCFG_FLAG4_WAKEUP_ON_WLAN)==0)
                {
                    *(UINT32 *)pv_get_info = PC_WIFI_WOL_DISABLE;
                }
                else
                {
                    *(UINT32 *)pv_get_info = PC_WIFI_WOL_ENABLE;
                }  
            }
            break;
        default:
            return RMR_DRV_INV_GET_INFO;
    }

    return RMR_OK;
}

/*-----------------------------------------------------------------------------
 * Name: PCDSet
 *
 * Description: This API implements Middleware set functionality for Power
 *              Control driver.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
INT32 PCDSet(DRV_COMP_ID_T* pt_comp_id,
             DRV_SET_TYPE_T e_set_type,
             const VOID*    pv_set_info,
             SIZE_T         z_set_info_len)
{
    UINT32 u4Val = 0;
    INT32 i4Val, i4EdidWPGpio, i4EdidWPEnable, i4Gpio;
    TIME_T timeVal;
    EEPDTVCFG_T rDtvCfg;
	UINT32 u4Clk;
	UINT32 u4PDWNEvent;
	UINT8 e_channel_id;
    UINT64 _u8OldGpioModeCtrlStatus;
    UINT32 u4GpioNum, u4CtrlWordBitNum;
    UINT64 _u8BitMask;

    UINT32 u4Addr;
    UINT32 u4Val2,u4Val3;
    UINT8 au1Data[6];
    MTPDWNC_T8032_CMD_T rCmd;	
    MTPDWNC_T8032_RESPONSE_T rRes;    
	
    MTRTC_T rRtc = {0,0,0,0,0,0,0};
    PC_GPIO_MONITOR_CTRL_EX_T GpioMonitor;
    i4Val = 0;
    x_memset(&rDtvCfg, 0, sizeof(EEPDTVCFG_T));
	
    x_memset(&rRes, 0, sizeof(MTPDWNC_T8032_RESPONSE_T));
	
    /* Check aguments */
    if (!b_pcd_init)
    {
        return RMR_DRV_INV_SET_INFO;
    }

    i4EdidWPEnable = (INT32)MTDRVCUST_OptGet(eEdidEepromWPEnablePolarity);

    if (0 > MTDRVCUST_OptQuery(eEdidEepromWPGpio, &u4Val))
    {
        i4EdidWPGpio = -1;
    }
    else
    {
        i4EdidWPGpio = (INT32)u4Val;
    }

    if ((pt_comp_id == NULL) ||
        (pt_comp_id->e_type != DRVT_PWR_CTRL) ||
        (pt_comp_id->ui2_id != PCD_DRV_ID))
    {
        return RMR_DRV_INV_SET_INFO;
    }
    /* Perform set opertion */
    switch (e_set_type)
    {
        case PC_SET_TYPE_WAKE_UP_SETUP:
            u4Val = (UINT32)pv_set_info;
            _fgVGAWakeup = (u4Val & PC_WAKE_UP_CAP_VGA) ? TRUE : FALSE;
            _fgUartNormalWakeup = (u4Val & PC_WAKE_UP_CAP_UART) ? TRUE : FALSE;
            if (!(u4Val & PC_WAKE_UP_CAP_RTC))
            {
                _u8UtcWakeup = 0;
            }
            break;

        case PC_SET_TYPE_WAKE_UP_TIME:
            if(pv_set_info == NULL)
            {
                return RMR_DRV_INV_SET_INFO;
            }
            timeVal = *((TIME_T *)pv_set_info);
            _u8UtcWakeup = (UINT64)timeVal;
            UNUSED(MTRTC_UtcToRtc(&rRtc, _u8UtcWakeup));
            LOG(7,"[PC_SET_TYPE_WAKE_UP_TIME] %2d/%2d/%2d %2d:%2d:%2d\n",
            rRtc.u1Year,
    	    rRtc.u1Month,
   	        rRtc.u1Day,
    	    rRtc.u1Hour,
    	    rRtc.u1Minute,
    	    rRtc.u1Second
    	    );
#ifndef ANDROID_TWO_WORLDS /* will redirect to AlarmManager in PC_SET_TYPE_POWER_SUSPEND */
            if(MTRTC_SetAlertTime(&_u8UtcWakeup) != MTR_OK)
            {
                return RMR_DRV_INV_SET_INFO;
            }
#endif            
            _fgRtcAlertTimeSet = TRUE;
            break;

        case PC_SET_TYPE_WAKE_UP_REASON:
            if(pv_set_info == NULL)
            {
                return RMR_DRV_INV_SET_INFO;
            }
            _rPCDWakeupReason.e_reason = ((PC_WAKE_UP_INFO_T *)pv_set_info)->e_reason;
            _rPCDWakeupReason.pv_data = ((PC_WAKE_UP_INFO_T *)pv_set_info)->pv_data;
            break;
		case PC_SET_TYPE_WAKEUP_IRKEY:
            if(pv_set_info == NULL)
            {
                return RMR_DRV_INV_SET_INFO;
            }
			_rPCDWakeupIrKey.ui4_WakeupKey = ((PC_IRRC_WAKKEY_T *)pv_set_info)->ui4_WakeupKey ;
			_rPCDWakeupIrKey.e_fgIrkey = ((PC_IRRC_WAKKEY_T *)pv_set_info)->e_fgIrkey     ;
            break;

        case PC_SET_TYPE_NFY_FCT:
            if(pv_set_info == NULL)
            {
                return RMR_DRV_INV_SET_INFO;
            }
            _PCDSetPCNfyFct( (PC_NFY_INFO_T*)pv_set_info, PC_WAKE_UP_REASON_RTC);
            _rGpioMonitorNfyInfo = *(PC_NFY_INFO_T*)pv_set_info;        //for gpio monitor use (PC_SET_TYPE_GPIO_MONITOR_CTRL)
#ifdef ANDROID_TWO_WORLDS
            _rWakeupTimeSetupNfyInfo = *(PC_NFY_INFO_T*)pv_set_info;    //for inform user space the Wakeup Timer is setupped 
#endif            
            break;

        case PC_SET_TYPE_MISC_CTRL:
            u4Val = (UINT32)pv_set_info;
            _PCDSetEepromFlag(u4Val);
            if (u4Val & PC_MISC_CTRL_FACTORY_MODE)
            {
                if (i4EdidWPGpio >= 0)
                {
                    // disable edid write protect.
                    i4Val = !i4EdidWPEnable;
                     UNUSED (MTGPIO_SetOut(i4EdidWPGpio, i4Val));
                }
            }
            else
            {
                if (i4EdidWPGpio >= 0)
                {
                    // enable edid write protect.
                    i4Val = i4EdidWPEnable;
                    UNUSED (MTGPIO_SetOut(i4EdidWPGpio, i4Val));
                }
            }
            break;

        case PC_SET_TYPE_POWER_STATE:
            if(pv_set_info == NULL)
            {
                return RMR_DRV_INV_SET_INFO;
            }
#ifdef CUSTOM_POWER_STATE_CTRL_ENABLE
            u4PDWNEvent = _rPCSetReason;
            if ((*((PC_PWR_STATE_CTRL_T *)pv_set_info)) == PC_PWR_STATE_CTRL_RUNNING)
            {
                Printf("Enter next state by event %x\n", _rPCSetReason);
            }
            else if ((*((PC_PWR_STATE_CTRL_T *)pv_set_info)) == PC_PWR_STATE_CTRL_SEMI_STANBY)
            {
                Printf("Enter semi-standby \n");
                u4PDWNEvent |= PDWNC_POWER_STATE_SEMI_STANDBY;
            }
            else if ((*((PC_PWR_STATE_CTRL_T *)pv_set_info)) == PC_PWR_STATE_CTRL_NORMAL)
            {
                Printf("Enter semi-standby \n");
                u4PDWNEvent |= PDWNC_POWER_STATE_NORMAL;
            }
            MTPDWNC_EnterPowerState(u4PDWNEvent);
#else
            UNUSED(_rPCSetReason);
            i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
            ASSERT(i4Val==0);
            if ((*((PC_PWR_STATE_CTRL_T *)pv_set_info)) == PC_PWR_STATE_CTRL_STANBY)
            {
                rDtvCfg.u1Flags &= ~((UINT8)DTVCFG_FLAG_SYSTEM_STATE_ON);
            }
            else
            {
                rDtvCfg.u1Flags |= (DTVCFG_FLAG_SYSTEM_STATE_ON);
            }
            i4Val = MTEEPDTV_SetCfg(&rDtvCfg);
            ASSERT(i4Val==0);

            u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_IRRC;
            if (_fgVGAWakeup)
            {
                u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_VGA;
            }

            if (MTDRVCUST_OptGet(eStdbyUsePowerBtnCount))
            {
                UINT8 u1OrgFlags;

                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
                ASSERT(i4Val==0);
                u1OrgFlags = rDtvCfg.u1Flags;
				
				UNUSED(MTIRRC_IsPowerButtonOff(&u4Val));
                if (u4Val)
                {
                    // Set POWER_BTN_ON from X to 0.
                    rDtvCfg.u1Flags &= ~((UINT8)DTVCFG_FLAG_POWER_BTN_ON);
                    // Power off mode.
                    u4Val = 0;

                    u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_KEYPAD;
                }
                else
                {
                    // Set POWER_BTN_ON from X to 1.
                    rDtvCfg.u1Flags |= (DTVCFG_FLAG_POWER_BTN_ON);
                }
                if (u1OrgFlags != rDtvCfg.u1Flags)
                {
                    i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
                    ASSERT(i4Val==0);
                }
            }

            if ((*((PC_PWR_STATE_CTRL_T *)pv_set_info)) == PC_PWR_STATE_CTRL_RUNNING)
            {
                Printf("Enter next state by event %x\n", u4PDWNEvent);
                UNUSED(MTPDWNC_EnterPowerState(u4PDWNEvent | 0x100));
            }
#endif
            break;
        case PC_SET_TYPE_POWER_SUSPEND:
			 u4WakeupReason = PC_WAKE_UP_REASON_UNKNOWN; //clear last status.
			 x_memset(&_rPCDWakeupReason, 0, sizeof(PC_WAKE_UP_INFO_T));

#ifdef ANDROID_TWO_WORLDS /* configure the RTC timers to AlarmManager */
            {
                BOOL   bQuietBoot = FALSE;            
                UINT32 u4Eepflags = 0;
                UINT64 u8CurrentUtc = 0;

                /* Current UTC */
                MTRTC_GetTimeDate(&u8CurrentUtc);
                
                /* Remove Quiet Boot misc control in eeprom */
                u4Eepflags = _PDGetEepromFlags();

                /* Save to a flag */
                if (u4Eepflags & PC_MISC_CTRL_RTC_QUIET_BOOT)
                {
                    bQuietBoot = TRUE;
                }                
                u4Eepflags &= ~(PC_MISC_CTRL_RTC_QUIET_BOOT);
                _PCDSetEepromFlag(u4Eepflags);

                /* Check the alert time, determine 3 cases */
                if (_u8UtcWakeup != 0)
                {
                    if (bQuietBoot)
                    {
                        Printf("[PCD_MW][Twoworlds] Setup AlarmManager: RTC wakeup + Quiet Boot. (Current RTC=%d) (RTC wakeup=%d)\n", (unsigned int)u8CurrentUtc, (unsigned int) _u8UtcWakeup);
                        if (_rWakeupTimeSetupNfyInfo.pf_nfy != NULL)
                        {
                            _rWakeupTimeSetupNfyInfo.pf_nfy( _rWakeupTimeSetupNfyInfo.pv_tag , PC_COND_WAKE_UP_TIMER_SETUP_BGM, (unsigned int) _u8UtcWakeup);
                        }
                    }
                    else
                    {
                        Printf("[PCD_MW][Twoworlds] Setup AlarmManager: RTC wakeup + Non Quiet Boot.(Current RTC=%d) (RTC wakeup=%d)\n", (unsigned int)u8CurrentUtc,(unsigned int) _u8UtcWakeup);
                        if (_rWakeupTimeSetupNfyInfo.pf_nfy != NULL)
                        {                        
                            _rWakeupTimeSetupNfyInfo.pf_nfy( _rWakeupTimeSetupNfyInfo.pv_tag , PC_COND_WAKE_UP_TIMER_SETUP_NORMAL, (unsigned int) _u8UtcWakeup);                                                
                        }
                    }
                }
                else
                {
                    Printf("[PCD_MW][Twoworlds] Setup AlarmManager: No RTC Wakeup from Linux world. Setting clear.\n");
                    if (_rWakeupTimeSetupNfyInfo.pf_nfy != NULL)
                    {                    
                        _rWakeupTimeSetupNfyInfo.pf_nfy( _rWakeupTimeSetupNfyInfo.pv_tag , PC_COND_WAKE_UP_TIMER_SETUP_CLEAR, (unsigned int) _u8UtcWakeup);
                    }
                }
            }
#endif            



#ifdef LINUX_TURNKEY_SOLUTION//#if 0
#if defined(CC_FAST_INIT)
            i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
            ASSERT(i4Val==0);
            if (rDtvCfg.u1Flags3 & DTVCFG_FLAG3_NORMAL_BOOT_WITHOUT_SUSPEND)
            {
                return RMR_OK;
            }
            /**********YJ to do****************/

            /**********YJ to do****************/
            //break;
#endif
#endif
        case PC_SET_TYPE_POWER_DOWN:
            u4Val = WAK_IRRX;
            u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_IRRC;
			#ifndef CC_CUSTOM_CONTROL_WAKEUP
            if (_u8UtcWakeup)
            {
                u4Val |= WAK_RTC;

#ifndef ANDROID_TWO_WORLDS /* will redirect to AlarmManager in PC_SET_TYPE_POWER_SUSPEND */                
                // Set wakeup timer at RTC.
                UNUSED(MTRTC_SetAlertTime(&_u8UtcWakeup));
#endif

            }
			#endif
            if (_fgVGAWakeup)
            {
                u4Val |= WAK_VGA;
                u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_VGA;
            }
            if (_fgUartNormalWakeup)
            {
                u4Val |= WAK_UNOR;
            }
#ifndef ADVANCED_STANDBY_CTRL
            if (MTDRVCUST_OptGet(eStdbyUsePrevPowerState))
#endif /* ADVANCED_STANDBY_CTRL */
            {
                // Set System state off.
                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
                ASSERT(i4Val==0);
                rDtvCfg.u1Flags &= ~((UINT8)DTVCFG_FLAG_SYSTEM_STATE_ON);
                i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
                ASSERT(i4Val==0);
            }

#ifndef CC_USB_DISABLE
			//un used in linux system
            //MUSB_TurnOffVBus();
#endif

//#ifdef CC_NAND_ENABLE
#if 0 // Modified by mtk40109 2010-12-03, don't un-init nand driver cause may call virtual eeprom later.
            // Need to un-init NAND driver. (Only needed for FDM. not needed for BDM/SDM)
            {
                STORG_DEV_T *prDev;
                prDev = STORGOpenDev(STORG_FLAG_NAND);
                STORGSendReq(prDev, STORG_OP_UNINIT, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL);
                STORGCloseDev(prDev);
            }
#endif

            if (MTDRVCUST_OptGet(eStdbyUsePowerBtnCount))
            {
                UINT8 u1OrgFlags;

                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);	
                ASSERT(i4Val==0);
                u1OrgFlags = rDtvCfg.u1Flags;
				
				UNUSED(MTIRRC_IsPowerButtonOff(&u4Val));
                if (u4Val)					
                {
                    // Set POWER_BTN_ON from X to 0.
                    rDtvCfg.u1Flags &= ~((UINT8)DTVCFG_FLAG_POWER_BTN_ON);
                    // Power off mode.
                    u4Val = 0;

                    u4PDWNEvent = (UINT32)PDWNC_POWER_DOWN_KEYPAD;
                }
                else
                {
                    // Set POWER_BTN_ON from X to 1.
                    rDtvCfg.u1Flags |= (DTVCFG_FLAG_POWER_BTN_ON);
                }
                if (u1OrgFlags != rDtvCfg.u1Flags)
                {
                    i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
                    ASSERT(i4Val==0);
                }
            }

            if ((u4Val & WAK_VGA) && MTDRVCUST_OptGet(eStdbyUseVGAStatus))
            {
                i4Val = MTEEPDTV_GetCfg(&rDtvCfg);	
                ASSERT(i4Val==0);
                rDtvCfg.u1Flags &= ~(DTVCFG_FLAG_POWER_BTN_ON);
                i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
                ASSERT(i4Val==0);
            }
            // Write CEC configuration to uP
            if((MTDRVCUST_InitGet(eT8032uPOffset) != 0) && ((u4Addr = MTDRVCUST_InitGet(eEepromCECMenuLanguageOffset)) != 0x0) &&
    		      ((MTDRVCUST_InitGet(eEepromCECEnableOffset)) == 0x00) && ((MTDRVCUST_InitGet(eEepromCECOneTouchPlayEnableOffset)) == 0x00))
            {
	            au1Data[0] = au1Data[1] = au1Data[2] = au1Data[3] = au1Data[4] = au1Data[5] = 0;
                rCmd.au1Data[0] = rCmd.au1Data[1] = rCmd.au1Data[2] = rCmd.au1Data[3] = 0;
                i4Val = MTEEPROM_Read(u4Addr, (UINT8*)au1Data, 6);
                rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_MENU_LANGUAGE;
                rCmd.u1SubCmd = 0;
                x_memcpy(rCmd.au1Data, au1Data, 4);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));

                rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_CEC;
                x_memcpy(rCmd.au1Data, &au1Data[4], 1);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));

                rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_ONE_TOUCH_PLAY;
                x_memcpy(rCmd.au1Data, &au1Data[5], 1);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));
            }
	        else if((MTDRVCUST_InitGet(eT8032uPOffset) != 0) && ((u4Addr= MTDRVCUST_InitGet(eEepromCECMenuLanguageOffset)) != 0x0) &&
	    	     ((u4Val2 = MTDRVCUST_InitGet(eEepromCECEnableOffset)) != 0x00) && ((u4Val3 = MTDRVCUST_InitGet(eEepromCECOneTouchPlayEnableOffset)) != 0x00))
	        {
	            au1Data[0] = au1Data[1] = au1Data[2] = au1Data[3] = au1Data[4] = au1Data[5] = 0;
                rCmd.au1Data[0] = rCmd.au1Data[1] = rCmd.au1Data[2] = rCmd.au1Data[3] = 0;
		        i4Val = MTEEPROM_Read(u4Addr, (UINT8*)au1Data, 4);
		        rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_MENU_LANGUAGE;
		        rCmd.u1SubCmd = 0;
		        x_memcpy(rCmd.au1Data, au1Data, 4);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));

			      i4Val = MTEEPROM_Read(u4Val2, (UINT8*)au1Data, 1);
		        rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_CEC;
                au1Data[1]=0;
                au1Data[2]=0;
                au1Data[3]=0;
		        x_memcpy(rCmd.au1Data, au1Data, 4);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));

				    i4Val = MTEEPROM_Read(u4Val3, (UINT8*)au1Data, 1);
		        rCmd.u1Cmd = PDWNC_CMD_ARMT8032_ENABLE_ONE_TOUCH_PLAY;
		        au1Data[1]=0;
		        au1Data[2]=0;
		        au1Data[3]=0;
		        x_memcpy(rCmd.au1Data, au1Data, 4);
                UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));
	        }
	        rCmd.u1Cmd = PDWNC_CMD_ARMT8032_SET_T8032_CTL_CEC; //Notify T8032 start/stop control CEC
			rCmd.u1SubCmd = 0;
			rCmd.au1Data[0] = 1;	//T8032 control CEC
			UNUSED(MTPDWNC_T8032Cmd(&rCmd, &rRes));

			if(e_set_type == PC_SET_TYPE_POWER_SUSPEND)
				UNUSED(MTPDWNC_RequestSuspendEvent(u4Val, u4PDWNEvent));	
			else
            	UNUSED(MTPDWNC_EnterPowerDown(u4Val, u4PDWNEvent));			
            //UNUSED(MTHDMISwitchInit());			
            break;
		//case PC_SET_TYPE_SUSPEND_EVENT_ENTER:
		//	break;
		case PC_SET_TYPE_POWER_RESUME:
#ifdef ANDROID_TWO_WORLDS
             /* c_pcl_set_request_suspend may called only once. */
             /* however PDWNC_Resume may have several times. We should always clear the variables in resume stage. */

			 u4WakeupReason = PC_WAKE_UP_REASON_UNKNOWN; //clear last status.
			 x_memset(&_rPCDWakeupReason, 0, sizeof(PC_WAKE_UP_INFO_T));
#endif			 
		
			UNUSED(MTCheckCustomWakeupReason((MT_WAKE_UP_REASON_T *)&_rPCWakeupReason)); //get init status after wakeup from suspend mode.
			_u4PCDInitEepromFlags = _PDGetEepromFlags();
		//	if (!_PCDInit())
		//	{
		//		Printf("PC_SET_TYPE_POWER_RESUME call _PCDInit fail!!!!\n");
		//	}
			break;
        case PC_SET_TYPE_POWER_DOWN_REASON:
            {
                UINT32 u4Btn = 0;
                
                PC_POWER_DOWN_INFO_T* prInfo = (PC_POWER_DOWN_INFO_T*)pv_set_info;
                if (pv_set_info == NULL)
                {
                    return RMR_DRV_INV_SET_INFO;
                }
                
                x_memset(&_rPCSetDownReason, 0, sizeof(PDWNC_SET_POWER_DOWN_REASON_T));                
                if (prInfo->pv_data == NULL)//pv_data is the new member for PC_SET_TYPE_POWER_DOWN_REASON, we use it to backward compatible  
                {
                    switch(prInfo->e_reason)
                    {
                    case PC_POWER_DOWN_REASON_IRRC:
                    	u4PDWNEvent = PDWNC_POWER_DOWN_IRRC;
                    	break;
                    case PC_POWER_DOWN_REASON_FP:
                    	u4PDWNEvent = PDWNC_POWER_DOWN_KEYPAD;
                    	break;
                    default:
                    	u4PDWNEvent = PDWNC_POWER_DOWN_UNKNOWN;
                    }
                    if (_fgVGAWakeup)
                    {
                        u4PDWNEvent = PDWNC_POWER_DOWN_VGA;
                    }
                UNUSED(MTPDWNC_EnterPowerState(u4PDWNEvent | 0x100));
                }
                else
                {
                    if (((prInfo->e_reason == PC_POWER_DOWN_REASON_IRRC) || 
                         (prInfo->e_reason == PC_POWER_DOWN_REASON_FP)) && 
                        (prInfo->pv_data == NULL))
                    {
                        return RMR_DRV_INV_SET_INFO;
                    }
                    
                 	_rPCSetReason = PDWNC_WAKE_UP_REASON_UNKNOWN;
                    if (prInfo->e_reason == PC_POWER_DOWN_REASON_IRRC)
                    {
                        u4Btn = *(UINT32 *)(prInfo->pv_data);
                        switch (u4Btn)
                    {
                        case BTN_POWER:
                        case BTN_POWER_OFF:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_IRRC;
                    	break;
                        case BTN_PRG_UP:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_PRG_UP;
                            break;
                        case BTN_PRG_DOWN:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_PRG_DOWN;
                            break;
                        case BTN_DIGIT_0:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_0;
                            break;
                        case BTN_DIGIT_1:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_1;
                            break;
                        case BTN_DIGIT_2:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_2;
                            break;
                        case BTN_DIGIT_3:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_3;
                            break;
                        case BTN_DIGIT_4:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_4;
                            break;
                        case BTN_DIGIT_5:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_5;
                            break;
                        case BTN_DIGIT_6:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_6;
                            break;
                        case BTN_DIGIT_7:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_7;
                            break;
                        case BTN_DIGIT_8:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_8;
                            break;
                        case BTN_DIGIT_9:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGIT_9;
                    	break;
                        case BTN_TV_ANA:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_ANALOG;
                    	break;
                        case BTN_TV_DIG:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGITAL;
                    	break;
                        case BTN_CUSTOM_68:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_RC_DIGITAL_ANALOG;
                    	break;
                        }
                    }
                    else if (prInfo->e_reason == PC_POWER_DOWN_REASON_FP)
                    {
                    	u4Btn = *(UINT32 *)(prInfo->pv_data);
                        switch (u4Btn)
                        {
                        case BTN_POWER:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_KEYPAD;
                            break;
                        case BTN_PRG_UP:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_FP_PRG_UP;
                            break;
                        case BTN_PRG_DOWN:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_FP_PRG_DOWN;
                            break;
                        case BTN_INPUT:
                            _rPCSetReason = PDWNC_WAKE_UP_REASON_FP_INP_SRC;
                            break;
                        }
                    }
                    else
                    {
                        _rPCSetReason = PDWNC_WAKE_UP_REASON_UNKNOWN;
                    }
    
                    _rPCSetDownReason.u4Reason = prInfo->e_reason;
                    _rPCSetDownReason.u4Btn = u4Btn;
                    MTPDWNC_SetPowerDownReason(_rPCSetDownReason);                
                }
            }
            break;

        case PC_SET_TYPE_REBOOT:
            UNUSED(MTPDWNC_Reboot());
            break;

        case PC_SET_TYPE_GPIO_MODE:
			//please call "PC_SET_TYPE_GPIO_STATUS" to set gpio output val directly
            break;

        case PC_SET_TYPE_GPIO_STATUS:
            i4Gpio = _PCD_XferGpio(((PC_GPIO_STATUS_INFO_T *)pv_set_info)->ui4_id);
            if (i4Gpio < 0)
            {
                return RMR_DRV_INV_SET_INFO;
            }

            i4Val = (((PC_GPIO_STATUS_INFO_T *)pv_set_info)->e_status == PC_GPIO_STATUS_HIGH) ? 1 : 0;
            UNUSED(MTGPIO_SetOut(i4Gpio, i4Val));
            break;

        case PC_SET_TYPE_SIF:
        
			i4Val = MTSIF_WriteMultipleSubAddr(SIF_PORT_0, 100/*100KHz*/,
					(UINT8)(((PC_SIF_INFO_T *)pv_set_info)->ui4_addr),
					(UINT8 *)&(((PC_SIF_INFO_T *)pv_set_info)->ui4_offset),
					1,
					(UINT8 *)&(((PC_SIF_INFO_T *)pv_set_info)->ui1_data),
					1);
            if (i4Val < 0)
            { // failed
                //return RMR_DRV_ERROR;     //this could cause RM abort...
                return RMR_DRV_INV_SET_INFO;
            }
            break;

        case PC_SET_TYPE_SIF_ADVANCED:
            i4Val = 0;  u4Clk = g_u4AdvSifClkDiv;
            e_channel_id = ((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->e_channel_id ;

            switch (e_channel_id)
            {
                case PC_SIF_CH_ID_EDID_BUS:
                    /* Do nothing */
                    break;
                case PC_SIF_CH_ID_SYS_BUS:
                    if( ((((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui1_offset_len)<=3) )  /* check sub-address length*/
                    {
                        LOG(7,"SIF_X_Write: SIF_BUS_SYSTEM_I2C ClkDiv:0x%x  ui4_addr:0x%02X  \n", u4Clk, ((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui4_addr);
                        UNUSED(MTSIF_WriteMultipleSubAddr(SIF_PORT_0,
                                                    (27000/u4Clk),
                                                    (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui4_addr),
                                                    (UINT8 *)&((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui4_offset,
                                                    (UINT8)((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui1_offset_len,
                                                    (UINT8 *)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->pui1_data),
                                                    (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui1_data_len)
                                            		));

            }
                    break;
                case PC_SIF_CH_ID_TUNER_BUS:
                    if( ((((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui1_offset_len)<=3) )  /* check sub-address length*/
                    {
                    
                        Printf("MTSIF_WriteMultipleSubAddr: SIF_BUS_TUNER_I2C ClkDiv:0x%x  ui4_addr:0x%02X  \n", u4Clk, ((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui4_addr);
                        UNUSED(MTSIF_WriteMultipleSubAddr(SIF_PORT_TUNER,
                                                    (27000/u4Clk),
                                                    (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui4_addr),
                                                     NULL,
                                                     0,
                                                    (UINT8 *)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->pui1_data),
                                                    (UINT8)(((PC_SIF_INFO_ADVANCED_T *)pv_set_info)->ui1_data_len)
                                            		));
                    }
            break;
            }

            if (i4Val == 0)
            {
                /* Failed */
                //return RMR_DRV_ERROR;     //this could cause RM abort...
                return RMR_DRV_INV_SET_INFO;
            }
            break;

    case PC_SET_TYPE_SIF_BY_GPIO:
        {
            PC_SIF_INFO_BY_GPIO_T  SwSif_T = *(PC_SIF_INFO_BY_GPIO_T *)pv_set_info;
            UINT8 aru1SubAddrData[83];   int i;    UINT8 aru1SubAddr[3];

            if (SwSif_T.ui1_data_len > 80 || SwSif_T.ui1_offset_len >3)
            {
                return RMR_DRV_INV_GET_INFO;
            }
            if(SwSif_T.ui1_offset_len>0)
            {
                for(i=SwSif_T.ui1_offset_len-1 ; i>=0  ;i--)      //sub-address
                {
                    if (i >= 3)
                    {
                        continue;
                    }
                    aru1SubAddr[i] = (UINT8) (SwSif_T.ui4_offset>>(i*8))&0xFF;
                }
                x_memcpy(aru1SubAddrData , aru1SubAddr, SwSif_T.ui1_offset_len );
            }
            x_memcpy(aru1SubAddrData+SwSif_T.ui1_offset_len , SwSif_T.pui1_data, SwSif_T.ui1_data_len );
            i4Val = MTSIFSW_WriteDataNoSubAddr(SwSif_T.ui4_addr , SwSif_T.ui1_offset_len+SwSif_T.ui1_data_len
                                                , aru1SubAddrData,  0xff, 10 , 100);     //send data
            if( i4Val != 0)
            {
                UNUSED(MTSIFSW_Remove());
                return RMR_DRV_INV_GET_INFO;
            }
            UNUSED(MTSIFSW_Remove());
        }
            break;
	case PC_SET_TYPE_GPIO_REG:
        /* check validatity of parameter length */

        if ( z_set_info_len < sizeof(PC_GPIO_MONITOR_CTRL_EX_T) )
        {
            return RMR_DRV_NOT_ENOUGH_SPACE;
        }
        else
        {
            z_set_info_len = sizeof(PC_GPIO_MONITOR_CTRL_EX_T);
        }

        /* register interrupt */
        //check GPIO validatity first
        VERIFY( 0 == MTGPIO_Init() );
        GpioMonitor =   *((PC_GPIO_MONITOR_CTRL_EX_T*)pv_set_info) ;
		i4Gpio = GpioMonitor.ui4_gpio;
		if(GpioMonitor.b_monitor)
			{
				VERIFY( MTGPIO_Reg( i4Gpio, GPIO_TYPE_INTR_BOTH,  (UINT32)0, _GpioNotify_Isr, NULL ) >=0 );
				Printf("\r[PCDSET]: GPIO %d hook to notify isr\n", i4Gpio);

			}
		else
			{
				VERIFY( MTGPIO_Reg( i4Gpio , GPIO_TYPE_NONE, (UINT32)0,  NULL, NULL ) >=0 );
				Printf("\r[PCDSET]: GPIO %d was hooked before-> hook to NULL \n", i4Gpio);
			}
		
		break;

    case PC_SET_TYPE_GPIO_MONITOR_CTRL:
        /* check validatity of parameter length */
        VERIFY( z_set_info_len == 8 );
        if ( z_set_info_len < sizeof(UINT64) )
        {
            return RMR_DRV_NOT_ENOUGH_SPACE;
        }
        else
        {
            z_set_info_len = sizeof(UINT64);
        }

        /* register interrupt */
        //check GPIO validatity first
        VERIFY( 0 == MTGPIO_Init() );

        _u8OldGpioModeCtrlStatus = _u8GpioModeCtrlStatus;
        _u8GpioModeCtrlStatus =   *((UINT64*)pv_set_info) ;
        u4GpioNum= 0;

        for( u4CtrlWordBitNum = 0 ; u4CtrlWordBitNum <=63 ; u4CtrlWordBitNum++)
        {
            if( u4CtrlWordBitNum<=55 )      //GPIO
            {
                u4GpioNum = u4CtrlWordBitNum;           }
            else
            {
                u4GpioNum = u4CtrlWordBitNum -56 + OPCTRL0 ;
            }

            LOG( 9 , "[PCDSET]: GPIO %d\n", u4GpioNum );
            //Printf("[PCDSET]: GPIO %d\n", u4GpioNum);

            _u8BitMask = ((UINT64)1<< u4CtrlWordBitNum);
			//MT_RESULT_T  MTGPIO_Reg (INT32 i4Gpio, MTGPIO_TYPE_T eType, UINT32 u4Period, MTPFN_GPIO_CALLBACK pfnCallback, VOID* pvArg)
            if( _u8GpioModeCtrlStatus & _u8BitMask )    //do set
            {
                //VERIFY( MTGPIO_Reg((INT32)u4GpioNum , GPIO_TYPE_INTR_BOTH, _GpioPolarityChangeNotify_Isr ) >=0 );
                VERIFY( MTGPIO_Reg( (INT32)u4GpioNum , GPIO_TYPE_INTR_BOTH, (UINT32)0, _GpioNotify_Isr, NULL ) >=0 );
                Printf("\r[PCDSET]: GPIO %d hook to notify isr\n", u4GpioNum);
            }
            else        //do un-set
            {
                //set already before?
                if( (_u8OldGpioModeCtrlStatus&(1<< u4CtrlWordBitNum)) == TRUE  )        //yes: unset it
                {
                  //  VERIFY( MTGPIO_Reg((INT32)u4GpioNum , GPIO_TYPE_INTR_BOTH, NULL ) >=0 );
                    VERIFY( MTGPIO_Reg( (INT32)u4GpioNum , GPIO_TYPE_NONE, (UINT32)0,  NULL, NULL ) >=0 );
                    Printf("\r[PCDSET]: GPIO %d was hooked before-> hook to NULL \n", u4GpioNum);
                }
                else        //no: dont care
                {
                    //do nothing
                }
            }
        }

        break;

    case PC_SET_TYPE_UPGRADE_CTRL:
        i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        rDtvCfg.u1Upgrade = (UINT8)(((UINT32)pv_set_info) & 0xffU);
        //rDtvCfg.u1Upgrade_Ext = (UINT8) ((((UINT32)pv_set_info) & 0xff00U)>>8);
        i4Val = MTEEPDTV_SetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;    
	case PC_SET_TYPE_UPGRADE_FROM_PARTITION_CTRL:
        i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        rDtvCfg.u1Upgrade_Ext = (UINT8) ((((UINT32)pv_set_info) & 0xff00U)>>8);
        i4Val = MTEEPDTV_SetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_INV_SET_INFO;
        }
        break;
    case PC_SET_TYPE_WOL_CTRL:
        i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        if(*((PC_Ethernet_WOL_CTRL_T *)pv_set_info)==PC_ETHERNET_WOL_DISABLE)
            rDtvCfg.u1Flags3 &= ~((UINT8)DTVCFG_FLAG3_WAKEUP_ON_LAN);
        else
            rDtvCfg.u1Flags3 |= (UINT8)DTVCFG_FLAG3_WAKEUP_ON_LAN;
        i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        break;
	case PC_SET_TYPE_WIFI_WOL_CTRL:
        i4Val = MTEEPDTV_GetCfg(&rDtvCfg);
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        if(*((PC_WIFI_WOL_CTRL_T *)pv_set_info)==PC_WIFI_WOL_DISABLE)
        	{
            rDtvCfg.u1Flags4 &= ~((UINT8)DTVCFG_FLAG4_WAKEUP_ON_WLAN);
			Printf("\r[WOW_DISABLE]:PC_SET_TYPE_WIFI_WOL_CTRL 0x%x  \n", rDtvCfg.u1Flags4);
        	}
        else
        	{
            rDtvCfg.u1Flags4 |= (UINT8)DTVCFG_FLAG4_WAKEUP_ON_WLAN;
			Printf("\r[WOW_ENABLE]:PC_SET_TYPE_WIFI_WOL_CTRL 0x%x  \n", rDtvCfg.u1Flags4);
        	}
        i4Val = MTEEPDTV_SetCfg(&rDtvCfg);	
        if (i4Val)
        {
            return RMR_DRV_ERROR;
        }
        break;
    default:
        return RMR_DRV_INV_SET_INFO;
    }

    return RMR_OK;
}


/*-----------------------------------------------------------------------------
 * Name: MW_PCDInit
 *
 * Description: This API initializes Power Control driver Middleware
 *              adaptation layer.
 *
 * Inputs: -
 *
 * Outputs: -
 *
 * Returns:
 ----------------------------------------------------------------------------*/
INT32 MW_PCDInit(void)
{
    INT32               i4_return;
    DRV_COMP_REG_T      t_pcd_comp_reg;
    DRV_COMP_FCT_TBL_T  t_pcd_comp_fct_tbl;
	PC_WAKE_UP_REASON_T	_rWakeupReason = (PC_WAKE_UP_REASON_T)0;

    if (b_pcd_init)
    {
        return RMR_OK;
    }
#if 0 /* cliu, this is unused */
    // assume dmx sram was not used by DMX in this stage
#ifdef __KERNEL__
    LDR_ENV_T* prLdrEnv = (LDR_ENV_T*)(DMX_SRAM_VIRT);
#else
    LDR_ENV_T* prLdrEnv = (LDR_ENV_T*)(SRAM_DMX_BASE);
#endif
    x_memcpy((void*)_szLdrVer, (void*)prLdrEnv->u.rHdr.szVersion, 64);
#endif
/*
    Printf("prLdrEnv->u.rHdr.szVersion 0x%x 0x%x 0x%x 0x%x\n",
        prLdrEnv->u.rHdr.szVersion[0],
        prLdrEnv->u.rHdr.szVersion[1],
        prLdrEnv->u.rHdr.szVersion[2],
        prLdrEnv->u.rHdr.szVersion[3]);
*/
    UNUSED(MTCheckCustomWakeupReason((MT_WAKE_UP_REASON_T *)&_rWakeupReason));	
	_rPCWakeupReason = _rWakeupReason;

    UNUSED(_u4TheInitKey);

    if (MTRTC_Init() != MTR_OK)   // Init failed.
    {
        return RMR_DRV_ERROR;
    }

    /* Init underneath driver */
    if (!_PCDInit())
    {
        return RMR_DRV_ERROR;
    }

    MTRTC_HookAlertHandler(_PCDRTCNfyFct);

    /* Setup PCD component ID */
    t_pcd_comp_reg.e_type = DRVT_PWR_CTRL;
    t_pcd_comp_reg.e_id_type = ID_TYPE_IND;
    t_pcd_comp_reg.u.t_ind.ui2_id = PCD_DRV_ID;
    t_pcd_comp_reg.u.t_ind.pv_tag = NULL;
    t_pcd_comp_reg.u.t_ind.ui1_port = 0;

    /* Setup PCD component function table */
    t_pcd_comp_fct_tbl.pf_rm_connect = NULL;
    t_pcd_comp_fct_tbl.pf_rm_disconnect = NULL;
    t_pcd_comp_fct_tbl.pf_rm_get = PCDGet;
    t_pcd_comp_fct_tbl.pf_rm_set = PCDSet;

    /* Register PCD component with Resource Manager database */
    i4_return = x_rm_reg_comp(&t_pcd_comp_reg,
                              1,
                              1,
                              NULL,
                              0,
                              &t_pcd_comp_fct_tbl,
                              NULL,
                              0);

    b_pcd_init = TRUE;

    return i4_return;
}

INT32 d_custom_misc_get_loader_info(UINT8 *szVersion, UINT32 *pu4VersionSize, UINT32 *pu4CheckSum)
{
#if 0 //#ifdef MW_IF2_TODO
    UINT8 *szFlashVer;

    szFlashVer = _szLdrVer;
    szFlashVer[31] = 0;

    if (szVersion && pu4VersionSize)
    {
        if ((*pu4VersionSize) > x_strlen((CHAR *)szFlashVer))
        {
            //x_strcpy((CHAR *)szVersion, (CHAR *)szFlashVer);
            x_strncpy((CHAR *)szVersion, (CHAR *)szFlashVer, x_strlen((CHAR *)szFlashVer));
        }
    }

    if (pu4VersionSize)
    {
        *pu4VersionSize = x_strlen((CHAR *)_szLdrVer) + 1;
    }

    if (pu4CheckSum)
    {
        *pu4CheckSum = *((UINT32 *)(&_szLdrVer[60]));
    }
#endif
	Printf("It is no use, will do nothing!\n");
    return 0;
}

#if 0
INT32 d_custom_get_custom_mode()
{
    return (GET_INIT_CUSTOM_MODE());
}

// pass NULL to query, pass pointer to set.
INT32 d_custom_setquery_image_bank(UINT32 *pu4Bank)
{
    EEPDTVCFG_T rDtvCfg;

    if (EEPDTV_GetCfg(&rDtvCfg))
    {
        return -1;
    }

    if (pu4Bank != NULL)
    {
        if (*pu4Bank)
        {
            rDtvCfg.u1Flags2 |= (DTVCFG_FLAG2_IMAGE_BANK);
        }
        else
        {
            rDtvCfg.u1Flags2 &= ~((UINT8)DTVCFG_FLAG2_IMAGE_BANK);
        }

        if (EEPDTV_SetCfg(&rDtvCfg))
        {
            return -1;
        }
    }
    return ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_IMAGE_BANK) ? 1 : 0);
}

// pass NULL to query, pass pointer to set.
INT32 d_custom_setquery_8032uP_bank(UINT32 *pu4Bank)
{
    EEPDTVCFG_T rDtvCfg;

    if (EEPDTV_GetCfg(&rDtvCfg))
    {
        return -1;
    }

    if (pu4Bank != NULL)
    {
        if (*pu4Bank)
        {
            rDtvCfg.u1Flags2 |= (DTVCFG_FLAG2_8032UP_BANK);
        }
        else
        {
            rDtvCfg.u1Flags2 &= ~((UINT8)DTVCFG_FLAG2_8032UP_BANK);
        }

        if (EEPDTV_SetCfg(&rDtvCfg))
        {
            return -1;
        }
    }
    return ((rDtvCfg.u1Flags2 & DTVCFG_FLAG2_8032UP_BANK) ? 1 : 0);
}

#ifdef CC_SUPPORT_7SEG_DISPLAY
EXTERN INT32 WTRTC_SetLedDim(UINT8 u1Level);
INT32 d_custom_set_7seg_led_dim(UINT8 u1Level)
{
    return (0);
//    return WTRTC_SetLedDim(u1Level);
}
#endif

INT32 d_custom_set_8032uP_led_blink(UINT8 ui1_blink_time)
{
    PDWNC_FuncTbl *prFuncTbl = PDWNC_EXT_GetFunc();

    if ((prFuncTbl != NULL) || (prFuncTbl->pfnSet8032uPLedBlink != NULL))
    {
        return prFuncTbl->pfnSet8032uPLedBlink(ui1_blink_time);
    }

    return (0);
}

INT32 d_custom_misc_ext_control(DRV_CUSTOM_MISC_TYPE_T eCmdType,
                                       VOID* pInfo, UINT32 u4InfoLen)
{
    PDWNC_FuncTbl *prFuncTbl = PDWNC_EXT_GetFunc();

    if ((prFuncTbl == NULL) || (pInfo == NULL))
    {
        return -1;
    }

    // call driver customization loader env init function.
    switch (eCmdType)
    {
    case DRV_CUSTOM_MISC_TYPE_EXT_CTRL_GET:
        if (prFuncTbl->pfnCustomRead != NULL)
        {
            return (INT32)(prFuncTbl->pfnCustomRead(NULL, pInfo, u4InfoLen));
        }
        break;

    case DRV_CUSTOM_MISC_TYPE_EXT_CTRL_SET:
        if (prFuncTbl->pfnCustomWrite != NULL)
        {
            return (INT32)(prFuncTbl->pfnCustomWrite(NULL, pInfo, u4InfoLen));
        }
        break;

    default:
        break;
    }

    return (0);
}
#endif

