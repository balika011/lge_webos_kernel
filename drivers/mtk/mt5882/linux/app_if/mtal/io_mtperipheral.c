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
 * $RCSfile: io_mtperipheral.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: Peripherals, such as NOR/NAND/SIF/RTC/PWM/GPIO (and others), are centralized in
 *                   this file
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtperipheral.h"
#include <linux/mtal_ioctl.h>
#ifdef CONFIG_MT53XX_NATIVE_GPIO
#include <mach/mtk_gpio.h>
#endif
#include "x_bim.h"
#include "x_dram.h"
#include "x_pinmux.h"
#include "x_pdwnc.h"
#include "x_gpio.h"
#include "nor_if.h"
#include "nand_if.h"
#if defined(CC_MSDC_ENABLE)
#include "msdc_if.h"
#endif
#include "sif_if.h"
#include "sif_sw_if.h"
#include "rtc_if.h"
#include "srm_drvif.h"
#include "ir_if.h"
#include "drvcust_if.h"
#ifdef CC_SUPPORT_SMC
#include "smc_if.h"
#endif
#include "cb_data.h"
#include "cb_low.h"

#include "osd_drvif.h"
#include "drv_pwm.h"
#include "eeprom_if.h"
#include "pdwnc_if.h"
#include "ir_if.h"
#include "api_eep.h"
#include "drv_display.h"

#ifdef RC_LAST_KEY_SUPPORT
#include "u_drv_cust.h"
#endif

UINT32  u4AccessRegflag = 1;
UINT32 u4Support_ScanPWM = 0;

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define PWM_REG(eSrc)       (_arPwmRegData[eSrc].u4Reg)
#define PWM_PINSEL(eSrc)    (_arPwmRegData[eSrc].u4PinSel)
#define PWM_FUNC(eSrc)      (_arPwmRegData[eSrc].u4Func)
#define PWM_GPIO(eSrc)      (_arPwmRegData[eSrc].u4Gpio)
#define PWM_EXTPBIT(eSrc)   (_arPwmRegData[eSrc].u4ExtPBit)

#define PWM_PORT_SCAN_SRC   1
#define PWM_PORT_SCAN_COPY  2

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------


static MT_RESULT_T	_MTPERIPHERAL_Read_HDCP2X_Key (unsigned long arg);
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef struct
{
    UINT32 u4Freq;
    UINT32 u4DutyOn;
    UINT32 u4ExtP;
    UINT32 u4RegVal;
    UINT32 fgEnable;
    UINT32 u4Start;
    UINT32 fgDoubleFreq;
} _PWM_ATTR_T;

typedef struct
{
    UINT32 u4Reg;
    UINT32 u4PinSel;
    UINT32 u4Func;
    UINT32 u4Gpio;
    UINT32 u4ExtPBit;
} _PWM_REG_DATA_T;

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#if defined(CC_MT5365) || defined(CC_MT5395) || defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5398)|| defined(CC_MT5399) || defined(CC_MT5890)
static _PWM_REG_DATA_T _arPwmRegData[PWM_SRC2+1] = {
    { REG_RW_PWM0, PIN_OPWM0, PINMUX_FUNCTION2, GPIO(86),   0xff },
    { REG_RW_PWM1, PIN_OPWM1, PINMUX_FUNCTION2, GPIO(87),     0 },
    { REG_RW_PWM2, PIN_OPWM2, PINMUX_FUNCTION2, GPIO(88),     16 },
};
#elif defined(CC_MT5882)
static _PWM_REG_DATA_T _arPwmRegData[PWM_SRC2+1] = {
    { REG_RW_PWM0, PIN_OPWM0, PINMUX_FUNCTION2, GPIO(79),   0xff },
    { REG_RW_PWM1, PIN_OPWM1, PINMUX_FUNCTION2, GPIO(80),     0 },
    { REG_RW_PWM2, PIN_OPWM2, PINMUX_FUNCTION2, GPIO(81),     16 },
};
#elif defined(CC_MT5389)
static _PWM_REG_DATA_T _arPwmRegData[PWM_SRC2+1] = {
    { REG_RW_PWM0, OPWM0_O, PINMUX_FUNCTION0, OPCTRL(12),   0xff },
    { REG_RW_PWM1, OPWM1_O, PINMUX_FUNCTION1, GPIO(54),     0 },
    { REG_RW_PWM2, OPWM2_O, PINMUX_FUNCTION1, GPIO(55),     16 },
};
/*
REG_RW_PWM0/1/2  is PWM0/1/2C0(29100/29110/29120)
*/
#elif defined(CC_MT5880)
static _PWM_REG_DATA_T _arPwmRegData[PWM_SRC2+1] = {
    { REG_RW_PWM0, PIN_OPWM0, PINMUX_FUNCTION1, GPIO(73),   0xff },
    { REG_RW_PWM1, PIN_OPWM1, PINMUX_FUNCTION1, GPIO(74),     0 },
    { REG_RW_PWM2, PIN_OPWM2, PINMUX_FUNCTION1, GPIO(75),     16 },
};

#elif defined(CC_MT5881)
static _PWM_REG_DATA_T _arPwmRegData[PWM_SRC2+1] = {
    { REG_RW_PWM0, PIN_OPWM0, PINMUX_FUNCTION1, GPIO(8),   0xff },
    { REG_RW_PWM1, PIN_OPWM1, PINMUX_FUNCTION1, GPIO(9),     0 },
    { REG_RW_PWM2, PIN_OPWM2, PINMUX_FUNCTION1, GPIO(10),     16 },
};

#endif


static _PWM_ATTR_T _arPwmCache[PWM_SRC2+1] = {{0, 0}, {0, 0}, {0, 0}};
static UINT32 _u4PowerDownEn = WAK_IRRX;
static BOOL _fgExternalRtcWakeUp=FALSE;
#if 0
static UINT32 _au4GpioCallBackPfn[NORMAL_GPIO_NUM];
static UINT32 _au4GpioCallBackArg[NORMAL_GPIO_NUM];
static UINT32 _au4GpioCallBackLevelType[NORMAL_GPIO_NUM];
static INT32 _ai4GpioCallBackLevelValue[NORMAL_GPIO_NUM];
#endif
static UINT32 _gu4PwmPhaseCount = 0;        // range from 0 to 3. maintain by _MtpwmTimer().
static UINT32 _gu4PwmPhaseTolerance = 0;    // timer clock. pass from _MtpwmTriggerPhaseTimer() to _MtpwmTimer().
static UINT32 _gu4PwmTargetPhase = 0;       // range from 0 to 3. pass from _MtpwmTriggerPhaseTimer() to _MtpwmTimer().
static UINT32 _gu4PwmPhaseReg2 = 0;         // PWM Phase Reg 2. pass from _MtpwmTriggerPhaseTimer() to _MtpwmTimer().
#if 0
static UINT32 _fgEverReqWakeupReason = 0;
static UINT32 _fgT8032WakeupReason = (UINT32)PDWNC_WAKE_UP_REASON_HDMI;
#endif
static UINT32 _u4PWMFreqCase;
static MTPWMSRC_TYPE_T _eTSrc;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(INT32)))                        \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_5ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_5ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_7ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_7ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_7ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_7ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_peripheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_FROM_USER_ARG_SIZE(ArgUsr, ArgKernel, size)       \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       (size)))                             \
    {                                                       \
        printk("mtal_ioctl_mtperipheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_ARG_SIZE(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_mtperipheral argument error\n");      \
        return MTR_ERR_INV;                                          \
    } \

#define USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_mtdmx argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

 #ifdef MTMPLAYER_RTC_PRINT
 #undef MTMPLAYER_RTC_PRINT
 #endif
#define MTMPLAYER_RTC_PRINT(func, rtc)    	do {\
                										if(IS_MTMPLAYER_LOG_ENABLE)	\
                										{printf("%s->u1Year=%u:",func,(rtc)->u1Year);	\
                										printf("%s->u1Month=%u:",func,(rtc)->u1Month);	\
                										printf("%s->u1Day=%u:",func,(rtc)->u1Day);	\
                										printf("%s->u1Hour=%u:",func,(rtc)->u1Hour);	\
                										printf("%s->u1Minute=%u:",func,(rtc)->u1Minute);	\
                										printf("%s->u1Second=%u:",func,(rtc)->u1Second);	\
                										printf("\n");}\
                    									} while (0)


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static void _RtcAlertHandler(void* pvTag)
{
    // Just put event with a dummy tag
    UINT32 u4Dummy;
    _CB_PutEvent(CB_MTAL_PERIPHERAL_RTC_TRIGGER, sizeof(UINT32), &u4Dummy);
}

static void _MtpwmTimer(void *pvTag)
{
    _gu4PwmPhaseCount = (_gu4PwmPhaseCount+1) & 0x3;
    if (_gu4PwmTargetPhase != _gu4PwmPhaseCount)
    {
        return;
    }

    if ((BIM_READ32(REG_RW_TIMER1_LOW) + _gu4PwmPhaseTolerance) >= BIM_READ32(REG_RW_TIMER1_LLMT))
    {
        BIM_WRITE32(_gu4PwmPhaseReg2, BIM_READ32(_gu4PwmPhaseReg2) & (~(PWME)));
        BIM_WRITE32(_gu4PwmPhaseReg2, BIM_READ32(_gu4PwmPhaseReg2) | PWME);
        BIM_WRITE32(REG_RW_TIMER_CTRL, BIM_READ32(REG_RW_TIMER_CTRL) & ~(TMR1_CNTDWN_EN | TMR1_AUTOLD_EN));
        _gu4PwmPhaseCount = 0;
    }
}
#if 0
static void _GpioPeriodCallbackHandler(INT32 i4Gpio, BOOL fgStatus)
{
    if( (i4Gpio < 0) || (i4Gpio >= NORMAL_GPIO_NUM) )
    {
        ASSERT(0);
        return;
    }

#ifdef CC_PERIODICAL_LEVEL_TRIGGER   ////period-level callback gpio
    if (_au4GpioCallBackPfn[i4Gpio] != 0)
    {
        MTGPIO_CB_T rGpioCb;

        rGpioCb.i4Gpio = i4Gpio;
        rGpioCb.fgStatus = fgStatus;
        rGpioCb.u4Pfn = _au4GpioCallBackPfn[i4Gpio];
        rGpioCb.u4Arg = _au4GpioCallBackArg[i4Gpio];

        _CB_PutEvent(CB_MTAL_GPIO_TRIGGER, sizeof(MTGPIO_CB_T), &rGpioCb);
    }
    else
    {
        ASSERT(0);
        return;
    }
#else
    if (_au4GpioCallBackPfn[i4Gpio] != 0)
    {
        MTGPIO_CB_T rGpioCb;

        if (_au4GpioCallBackLevelType[i4Gpio] != 0)
        {
            INT32 i4Intr;

            // disable for level type ISR
            i4Intr = 0;
            GPIO_Intrq( i4Gpio, &i4Intr);
        }

        rGpioCb.i4Gpio = i4Gpio;
        rGpioCb.fgStatus = fgStatus;
        rGpioCb.u4Pfn = _au4GpioCallBackPfn[i4Gpio];
        rGpioCb.u4Arg = _au4GpioCallBackArg[i4Gpio];

        _CB_PutEvent(CB_MTAL_GPIO_TRIGGER, sizeof(MTGPIO_CB_T), &rGpioCb);
    }
    else
    {
        ASSERT(0);
        return;
    }
 #endif
}
#endif
#ifdef CC_SUPPORT_SMC
//-----------------------------------------------------------------------------
/** _MtTsRecGBLCallback
 */
//-----------------------------------------------------------------------------
static void _SmcCallbackHandler(UINT8 u1HotPlugStatus)
{
    MTSMC_CB_T rCb;
    rCb.u1Status = u1HotPlugStatus;
    UNUSED(_CB_PutEvent(CB_MTAL_SMC_TRIGGER, sizeof (MTSMC_CB_T), &rCb));
}
static void _SmcCallbackHandlerEx(UINT8 u1HotPlugStatus)
{
    MTSMC_CB_T rCb;    
    rCb.u1Status = u1HotPlugStatus;
    UNUSED(_CB_PutEvent(CB_MTAL_SMC_TRIGGER_EX, sizeof (MTSMC_CB_T), &rCb));
}
#endif

//-----------------------------------------------------------------------------
/** _MTPWM_ScanPWMEnable()  This driver select ScanPWM or PWM
 *  frequence and duty cycle
 *  @param bOnOff
 *  @return MT_RESULT_T
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPWM_ScanPWMEnable(UINT32 bOnOff)
{
    u4Support_ScanPWM = bOnOff;
    if(u4Support_ScanPWM == TRUE)
    {
        UNUSED(BSP_PinSet(PIN_OPWM1, PINMUX_FUNCTION3));
        UNUSED(BSP_PinSet(PIN_OPWM2, PINMUX_FUNCTION3));
    }
    else
    {
        UNUSED(BSP_PinSet(PIN_OPWM1, PINMUX_FUNCTION2));
    }

    return MTR_OK;
}

static MT_RESULT_T _MtpwmSetScanPWM(MTPWMSRC_TYPE_T eSrc, UINT32 u4Start, UINT32 u4DutyOn, BOOL fgDoubleFreq, BOOL fgStepControl)
{
    // Validate duty cycle
    if(u4DutyOn > 1000)
    {
        return MTR_ERR_INV;
    }
    // Validate start position
    if(u4Start > 1000)
    {
        return MTR_ERR_INV;
    }

#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)  || defined(CC_MT5398) || defined(CC_MT5880)
    if(fgDoubleFreq)
    {
        vDrvSetScanPWM(eSrc, (u4Start+1)/2, ((u4DutyOn+1)/2), 500-((u4DutyOn+1)/2));  //rounding to 0 ~ 100%
    }
    else
    {
        vDrvSetScanPWM(eSrc, u4Start, u4DutyOn, (1000-u4DutyOn));  //rounding to 0 ~ 100%
    }
#elif defined(CC_MT5399)|| defined(CC_MT5890)|| defined(CC_MT5861) || defined(CC_MT5882) 
	vDrvSetScanPWMStepControlEn(fgStepControl);
	
	if(fgDoubleFreq)
	{
		vDrvSetScanPWMSW(eSrc, (u4Start+1)/2, ((u4DutyOn+1)/2), 500-((u4DutyOn+1)/2));	//rounding to 0 ~ 100%
	}
	else
	{
		vDrvSetScanPWMSW(eSrc, u4Start, u4DutyOn, (1000-u4DutyOn));  //rounding to 0 ~ 100%
	}
#endif
    _arPwmCache[eSrc].u4Start = u4Start;
    _arPwmCache[eSrc].u4DutyOn = u4DutyOn;
    _arPwmCache[eSrc].fgDoubleFreq = fgDoubleFreq;

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTPWM_SetScanPWM()  This driver control the PWM0 & PWM1 output
 *  Start and duty cycle
 *  @param eSrc      PWM source (SrcPWM0/1/2)
 *  @param u4Sart Start position percentage multiple by 10, ex. u4Sart =300 -->30.0% delay from Vsync
 *  @param u4DutyOn  Duty cycle percentage multiple by 10, ex. u4DutyOn =367 -->36.7% Duty Cycle
 *  @param fgEnable
 *  @return MT_RESULT_T
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPWM_SetScanPWM(unsigned long arg)
{
    MT_RESULT_T rRet;
    MTAL_IOCTL_6ARG_T rArg;
    MTPWMSRC_TYPE_T eSrc;
    UINT32 u4Start;
    UINT32 u4DutyOn;
    BOOL fgEnable;
    BOOL fgDoubleFreq;
	BOOL fgStepControl;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    eSrc=( MTPWMSRC_TYPE_T )rArg.ai4Arg[0];
    u4Start=(UINT32)rArg.ai4Arg[1];
    u4DutyOn=(UINT32)rArg.ai4Arg[2];
    fgEnable=(BOOL)rArg.ai4Arg[3];
    fgDoubleFreq=(BOOL)rArg.ai4Arg[4];
    fgStepControl=(BOOL)rArg.ai4Arg[5];

    MTPERIPHERAL_PRINT(" - eSrc = %d\n"
		"u4Start = 0x%x\n"
		"u4DutyOn = 0x%x\n"
		"fgEnable = %d\n"
		"fgDoubleFreq = %d\n"
		"fgStepControl = %d\n",
		eSrc,
		u4Start,
		u4DutyOn,
		fgEnable,
		fgDoubleFreq,
		fgStepControl);

    if (eSrc > PWM_SRC2)
    {
        return MTR_ERR_INV;
    }


    if (!fgEnable)
    {
        GPIO_SetOut(PWM_GPIO(eSrc), 0);
    }

    rRet = _MtpwmSetScanPWM(eSrc, u4Start, u4DutyOn, fgDoubleFreq, fgStepControl);

    _arPwmCache[eSrc].fgEnable = fgEnable;

    if (fgEnable)
    {
        _arPwmRegData[eSrc].u4Func = PINMUX_FUNCTION3;
        UNUSED(BSP_PinSet(PWM_PINSEL(eSrc), PWM_FUNC(eSrc)));
    }

    return rRet;
}

static MT_RESULT_T _MTPWM_ScanPWMAutoTestEn(unsigned long arg)
{
	//MT_RESULT_T rRet;
    MTAL_IOCTL_7ARG_T rArg;
	
	USR_SPACE_ACCESS_VALIDATE_7ARG(arg);
    COPY_FROM_USER_7ARG(arg, rArg);

#if defined(CC_MT5890)||defined(CC_MT5399)||defined(CC_MT5861)||defined(CC_MT5882)
	_fgScanPWMAuto = rArg.ai4Arg[0];
	_u1ScanPWMTestSrc =( MTPWMSRC_TYPE_T )rArg.ai4Arg[1];    
    _u1ScanPWMTestChg = rArg.ai4Arg[2];
    _u4ScanPWMTesFreq = rArg.ai4Arg[3];
    _i4ScanPWMTestStart = rArg.ai4Arg[4];
    _u4ScanPWMTestBottom = rArg.ai4Arg[5];	
	_u4ScanPWMTestTop = rArg.ai4Arg[6];

	if (_u1ScanPWMTestSrc > PWM_SRC2)
    {
        return MTR_ERR_INV;
    }

	if (_u1ScanPWMTestChg)
	{
		_u4ScanPWMTestHigh = _u4ScanPWMTestTop;
	}
	else
	{
		_u4ScanPWMTestHigh = _u4ScanPWMTestBottom;
	}
	_u4ScanPWMTestHigh = _u4ScanPWMTestHigh*1000/255;
	_u4ScanPWMTestLow = 1000 - _u4ScanPWMTestHigh;
	_u4ScanPWMTestBottom = _u4ScanPWMTestBottom*1000/255;
	_u4ScanPWMTestTop = _u4ScanPWMTestTop*1000/255;
	Printf("_fgScanPWMAuto=%d, Src=%d, _u1ScanPWMTestChg=%d, _u4ScanPWMTesFreq=%d, Bottom=%d, Top=%d\n", 
		_fgScanPWMAuto,_u1ScanPWMTestSrc,_u1ScanPWMTestChg,_u4ScanPWMTesFreq,_u4ScanPWMTestBottom,_u4ScanPWMTestTop);
	Printf("_u4ScanPWMTestStart=%d, _u4ScanPWMTestHigh=%d, _u4ScanPWMTestLow=%d\n", _i4ScanPWMTestStart, _u4ScanPWMTestHigh,_u4ScanPWMTestLow);
#endif

	return MTR_OK;
}

//extern UINT32 u4UIPwmRsn, u4UIPwmP, u4UIPwmH, u4UIPwmExtP;
UINT8 u1PWMModeChgflg = 0;
static MT_RESULT_T _MtpwmSetFrequency(MTPWMSRC_TYPE_T eSrc, UINT32 u4Frequency, UINT32 u4DutyOn)
{
    UINT32 u4PwmRsn, u4PwmP, u4PwmH, u4PwmExtP;
    UINT32 u4BusClk;
	UINT32 u4Val = 0;

	UNUSED(u4Val);

    // Validate duty cycle
    if(u4DutyOn > 1000)
    {
        return MTR_ERR_INV;
    }

    u4BusClk = BSP_GetDomainClock(SRC_BUS_CLK);

    // Validate frequency upper bound is ((Bus clock)/256)
    if ((u4BusClk >> 8) <= u4Frequency)
    {
        return MTR_ERR_INV;
    }

    // Check Rsn value.
    u4PwmRsn = (u4DutyOn == 1000) ? 0xfe : 0xff;
	if(u4Frequency == 0)
	{
		u4PwmP = 0;
		u4PwmH = 0;
	}
	else
	{
	    u4PwmP = ((u4BusClk >> 8)/ u4Frequency);
		u4PwmH = ((u4DutyOn == 1000) || (((u4DutyOn * 257) / 1000) > 0xff)) ? 0xff : ((u4DutyOn * 257) / 1000);
	}
    u4PwmExtP = ((u4PwmP >> 12) & 0xff);
	#if defined(CC_MT5396) || defined(CC_MT5368) || defined(CC_MT5389)  || defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881)|| defined(CC_MT5399) || defined(CC_MT5890)|| defined(CC_MT5882)
	vDrvSetPWM(eSrc,u4PwmP,u4PwmH,u4PwmRsn);
	#else
	if (((PWM_EXTPBIT(eSrc) > 0x20) && (u4PwmP > 0xfff)) || (u4PwmP > 0xfffff))	//only 5365 need
    {
        return MTR_ERR_INV;
    }
    u4Val = BIM_READ32(REG_RW_PWMLVDS);

    BIM_WRITE32(REG_RW_PWMLVDS, (u4Val & (~(0xff << PWM_EXTPBIT(eSrc)))) | ((u4PwmExtP & 0xff) << PWM_EXTPBIT(eSrc)));
    u4Val = (((u4PwmRsn & 0xff) << 24) | ((u4PwmH & 0xff) << 16) | ((u4PwmP & 0xfff) << 4) | PWME);

    if (u4Val != BIM_READ32(PWM_REG(eSrc)))
    {
        BIM_WRITE32(PWM_REG(eSrc), u4Val);
    }
	#endif
    _arPwmCache[eSrc].u4Freq = u4Frequency;
    _arPwmCache[eSrc].u4DutyOn = u4DutyOn;
    _arPwmCache[eSrc].u4ExtP = u4PwmExtP;
#if (!defined(CC_MT5396))&&(!defined(CC_MT5368))&&(!defined(CC_MT5389))&&(!defined(CC_MT5880))&&(!defined(CC_MT5881))&& (!defined(CC_MT5399)) && (!defined(CC_MT5890))&& (!defined(CC_MT5882))
    _arPwmCache[eSrc].u4RegVal = u4Val;
#endif

    return MTR_OK;
}

static MT_RESULT_T _MtpwmTriggerPhaseTimer(MTPWMSRC_TYPE_T eSrc1, MTPWMSRC_TYPE_T eSrc2, UINT32 u4Frequency, UINT8 u1Phase)
{
    static UINT32 _fgInit = 0;
    static MTPWMSRC_TYPE_T _eSrc1;
    static MTPWMSRC_TYPE_T _eSrc2;
    static UINT32 _u4Frequency;
    static UINT8 _u1Phase;

    UINT32 u4Timer, u4Val;
    void (* pfnOldIsr)(UINT16);

    if (_fgInit)
    {
        if ((_eSrc1 == eSrc1) && (_eSrc2 == eSrc2) && (_u4Frequency == u4Frequency) && (_u1Phase == u1Phase))
        {
            return MTR_OK;
        }
    }
    else
    {
        _fgInit = 1;
    }

    _eSrc1 = eSrc1;
    _eSrc2 = eSrc2;
    _u4Frequency = u4Frequency;
    _u1Phase = u1Phase;

    if (u1Phase == PWM_PHASE_0)
    {
        BIM_WRITE32(PWM_REG(eSrc1), BIM_READ32(PWM_REG(eSrc1)) & (~(PWME)));
        BIM_WRITE32(PWM_REG(eSrc2), BIM_READ32(PWM_REG(eSrc2)) & (~(PWME)));
        BIM_WRITE32(PWM_REG(eSrc1), BIM_READ32(PWM_REG(eSrc1)) | PWME);
        BIM_WRITE32(PWM_REG(eSrc2), BIM_READ32(PWM_REG(eSrc2)) | PWME);
        return MTR_OK;
    }

    // Check wait time.
    u4Timer = BSP_GetDomainClock(SRC_TIMER_CLK) / u4Frequency;
    _gu4PwmPhaseTolerance = u4Timer / 80;
    u4Timer = u4Timer / 4;

    switch(u1Phase)
    {
    case PWM_PHASE_90:
        _gu4PwmTargetPhase = 1;
        break;
    case PWM_PHASE_180:
        _gu4PwmTargetPhase = 2;
        break;
    case PWM_PHASE_270:
        _gu4PwmTargetPhase = 3;
        break;
    default:
        return MTR_NOT_SUPPORTED;
    }

    // Hook timer interrupt.
    _gu4PwmPhaseCount = 0;
    VERIFY(x_reg_isr(VECTOR_T1, (x_os_isr_fct)_MtpwmTimer, &pfnOldIsr) == OSR_OK);
    BIM_WRITE32(REG_RW_TIMER1_LLMT, u4Timer);
    BIM_WRITE32(REG_RW_TIMER1_HLMT, 0);
    BIM_WRITE32(REG_RW_TIMER1_LOW, u4Timer);
    BIM_WRITE32(REG_RW_TIMER1_HIGH, 0);

    u4Val = BIM_READ32(REG_RW_TIMER_CTRL) | TMR1_CNTDWN_EN | TMR1_AUTOLD_EN;
    BIM_WRITE32(PWM_REG(eSrc1), BIM_READ32(PWM_REG(eSrc1)) & (~(PWME)));
    BIM_WRITE32(PWM_REG(eSrc1), BIM_READ32(PWM_REG(eSrc1)) | PWME);
    _gu4PwmPhaseReg2 = PWM_REG(eSrc2);
    BIM_WRITE32(REG_RW_TIMER_CTRL, u4Val);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
//GPIO

//----------------------------------------------------------------------------
/** MTGPIO_Init() Initialize GPIO driver and hardware
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_Init (VOID)
{
#if 0
    UINT32 u4Idx;
#endif

    MTPERIPHERAL_PRINT(" - void \n");

    GPIO_Init();

#if 0
    for (u4Idx = 0; u4Idx < NORMAL_GPIO_NUM; u4Idx++)
    {
        _au4GpioCallBackPfn[u4Idx] = 0;
        _au4GpioCallBackArg[u4Idx] = 0;
        _au4GpioCallBackLevelType[u4Idx] = 0;
        _ai4GpioCallBackLevelValue[u4Idx] = 0;
    }
#endif
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTGPIO_SetDirection()  The GPIO Direction (in/out) setting function.
 *  @param i4GpioNum the gpio number to set.
 *  @param eDir Direction in or out
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_SetDirection (unsigned long arg)
{
     MTAL_IOCTL_2ARG_T rArg;
     INT32 i4GpioNum, i4DrvRet;
     MTGPIO_DIR_T eDir;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    eDir=(MTGPIO_DIR_T)rArg.ai4Arg[1];

 //add driver implementation code here
    i4DrvRet= GPIO_Enable(i4GpioNum, (INT32*)&eDir);

    MTPERIPHERAL_PRINT(" - i4GpioNum = %d, eDir = %d\n",
		i4GpioNum,
		eDir);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTGPIO_GetDirection()  The GPIO Direction (in/out) setting function.
 *  @param i4GpioNum the gpio number to set.
 *  @retval MTGPIO_DIR_T, gpio direction
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_GetDirection (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4GpioNum,i4DrvRet;
//    MTGPIO_DIR_T * peDir;
    MTGPIO_DIR_T eDir;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
//    peDir=(MTGPIO_DIR_T*)rArg.ai4Arg[1];

 //add driver implementation code here
    i4DrvRet=GPIO_Enable(i4GpioNum, NULL);
    eDir =i4DrvRet;

    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],MTGPIO_DIR_T);
    COPY_TO_USER_ARG(rArg.ai4Arg[1],eDir,MTGPIO_DIR_T);

    MTPERIPHERAL_PRINT(" - i4GpioNum = %d, direction = %d\n",
		i4GpioNum,
		eDir);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTGPIO_QueryOutVal()  The GPIO Direction out (low/high) query function.
 *  @param i4GpioNum the gpio number to query.
 *  @retval MTGPIO_DIR_T, gpio direction
 */
//-----------------------------------------------------------------------------
MT_RESULT_T  _MTGPIO_QueryOutVal (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4GpioNum;
    INT32 eDir;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];

 //add driver implementation code here
    eDir=GPIO_Output(i4GpioNum, NULL);

    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],INT32);
    COPY_TO_USER_ARG(rArg.ai4Arg[1],eDir,INT32);

    MTPERIPHERAL_PRINT(" - i4GpioNum = %d, direction = %d\n",
        i4GpioNum,
        eDir);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTGPIO_Output()  The GPIO out (low/high) set function.
 *  @param i4GpioNum the gpio number to set.
 *  @param pu4Val high/low.
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
MT_RESULT_T  _MTGPIO_Output (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4GpioNum;
    INT32 pu4Val;
    INT32 u4Val;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    pu4Val=(INT32)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_1ARG(pu4Val);
    COPY_FROM_USER_1ARG(pu4Val, u4Val);

    //add driver implementation code here
    GPIO_Output(i4GpioNum, &u4Val);

    MTPERIPHERAL_PRINT(" - i4GpioNum = %d, u4Val = %d\n",
        i4GpioNum,
        u4Val);
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTGPIO_Intrq() The GPIO interrupt enable setting functions. It will check
 *  the i4GpioNum and set to related register bit as 0 or 1.  In this
 *  function, 0 is interrupt disable mode and 1 is interrupt enable mode.
 *  @param i4GpioNum the gpio number to set or read.
 *  @param pi4Intr A integer pointer. It should not be NULL, it must
 *  reference to a integer.  If the integer is 0, this function will set the
 *  mode of the gpio number as interrupt disable mode, otherwise set as
 *  interrupt enable mode.
 *  @It return 0 or 1 (0 is interrupt disable mode,
 *          1 is interrupt enable mode.), return (*pi4Intr).
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_Intrq (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4GpioNum;
    INT32 *pi4Intr;
	INT32 i4Val;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    pi4Intr=(INT32 *)rArg.ai4Arg[1];
    MTPERIPHERAL_PRINT(" - i4GpioNum = %d\n",
		i4GpioNum);
#if 0
    if ((_au4GpioCallBackLevelType[i4GpioNum] != 0) &&
        (pi4Intr != NULL) &&
        (*pi4Intr != 0))
    {
        if ( GPIO_GetIn(i4GpioNum) == _ai4GpioCallBackLevelValue[i4GpioNum] )
        {
            // trigger level match, call back immediately
            MTGPIO_CB_T rGpioCb;

            rGpioCb.i4Gpio = i4GpioNum;
            rGpioCb.fgStatus = _ai4GpioCallBackLevelValue[i4GpioNum];
            rGpioCb.u4Pfn = _au4GpioCallBackPfn[i4GpioNum];
            rGpioCb.u4Arg = _au4GpioCallBackArg[i4GpioNum];

            _CB_PutEvent(CB_MTAL_GPIO_TRIGGER, sizeof(MTGPIO_CB_T), &rGpioCb);

            return MTR_OK;
        }
    }
#endif
     //add driver implementation code here
     i4Val = GPIO_Intrq( i4GpioNum, pi4Intr );
    if(i4Val != -1)
    {
		return MTR_NOT_OK;
    }
    else
    {
        return MTR_OK;
    }
}

//-----------------------------------------------------------------------------
/** _MTGPIO_IntrqQuery() Query the GPIO interrupt enable setting functions.
 *  @pi4Intr, it return 0 or 1 (0 is interrupt disable mode,
 *          1 is interrupt enable mode.)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_IntrqQuery (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    INT32 i4GpioNum;
    INT32 *pi4Intr;
	INT32 i4Val;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    pi4Intr=(INT32 *)rArg.ai4Arg[1];
    MTPERIPHERAL_PRINT(" - i4GpioNum = %d\n",
		i4GpioNum);

     //add driver implementation code here
     i4Val = GPIO_Intrq( i4GpioNum, NULL );
    if(i4Val != -1)
    {
		return MTR_NOT_OK;
    }
    else
    {
    	USR_SPACE_ACCESS_VALIDATE_ARG(pi4Intr,UINT8);
    	COPY_TO_USER_ARG(pi4Intr,i4Val,UINT8);

        return MTR_OK;
    }
}

//-----------------------------------------------------------------------------
/** MTGPIO_Query()  The GPIO interrupt query  functions. It will read abd return interrupt
 *  setting register bit.
 *  @param i4Gpio the gpio number to read.
 *  @param *pi4Intr Interrupt is set (=1) or not(=0)
 *  @param *pi4Fall  Falling Edge Interrupt is set (=1) or not(=0)
 *  @param *pi4Rise  Rising Edge Interrupt is set (=1) or not(=0)
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_Query (unsigned long arg)
{
     MTAL_IOCTL_4ARG_T rArg;
     INT32 i4Gpio;
     INT32 *pi4Intr=NULL;
     INT32 *pi4Fall=NULL;
     INT32 *pi4Rise=NULL;

     USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
     COPY_FROM_USER_4ARG(arg, rArg);

    i4Gpio=( INT32 )rArg.ai4Arg[0];
    pi4Intr=(INT32 *)rArg.ai4Arg[1];
    pi4Fall=(INT32 *)rArg.ai4Arg[2];
    pi4Rise=(INT32 *)rArg.ai4Arg[3];

     //add driver implementation code here
    GPIO_Query(i4Gpio, pi4Intr, pi4Fall, pi4Rise );

    MTPERIPHERAL_PRINT(" - i4Gpio = %d\n"
        "pi4Intr = %d\n"
        "pi4Fall = %d\n"
        "pi4Rise = %d\n",
	 i4Gpio,
	 (int)*pi4Intr,
	 (int)*pi4Fall,
	 (int)*pi4Rise);

    return MTR_OK;
}

#ifdef CONFIG_MT53XX_NATIVE_GPIO
/// Register using native gpio driver.
static LIST_HEAD(user_gpio_list);

struct user_gpio_info
{
    struct list_head list;

    INT32 i4Gpio;
    UINT32 user_callback;
    UINT32 arg;
};

static void _MTGPIO_HandleUserCallback(int gpio, int status, void *data)
{
    struct user_gpio_info *ginfo = (struct user_gpio_info*)data;
    MTGPIO_CB_T rGpioCb;

    rGpioCb.i4Gpio = gpio;
    rGpioCb.fgStatus = status;
    rGpioCb.u4Pfn = ginfo->user_callback;
    rGpioCb.u4Arg = ginfo->arg;
    _CB_PutEvent(CB_MTAL_GPIO_TRIGGER, sizeof(MTGPIO_CB_T), &rGpioCb);
}

static INT32 _MTGPIO_RegUserCallback(INT32 i4Gpio, GPIO_TYPE eType, UINT32 callback, UINT32 arg)
{
    struct user_gpio_info *ginfo = 0;
    struct list_head *plist;
    CRIT_STATE_T state;

    state = x_crit_start();

    // Init listhead if necessary
    if (!user_gpio_list.next)
        INIT_LIST_HEAD(&user_gpio_list);

    // Find/reuse old if already exist.
    list_for_each(plist, &user_gpio_list)
    {
        struct user_gpio_info *tmp = (struct user_gpio_info*)plist;
        if (tmp->i4Gpio == i4Gpio)
        {
            ginfo = tmp;
            break;
        }
    }

    // Insert new one if not exist.
    if (!ginfo)
    {
        ginfo = (struct user_gpio_info*)x_mem_alloc(sizeof(struct user_gpio_info));
        list_add(&ginfo->list, &user_gpio_list);
    }

    if (!ginfo)
    {
        x_crit_end(state);
        return -ENOMEM;
    }

    ginfo->i4Gpio = i4Gpio;
    ginfo->user_callback = callback;
    ginfo->arg = arg;

    x_crit_end(state);

    // Now register it.
    return mtk_gpio_request_irq(i4Gpio, (MTK_GPIO_IRQ_TYPE)eType, _MTGPIO_HandleUserCallback, ginfo);
}
#else
/// Register using old GPIO_Reg
static INT32 _MTGPIO_RegUserCallback(INT32 i4Gpio, GPIO_TYPE eType, UINT32 callback, UINT32 arg)
{
    GPIO_SetIntExtParam(i4Gpio, arg);  //This function *MUST" be executed before GPIO_Reg!!!
    return GPIO_Reg(i4Gpio, eType, (PFN_GPIO_CALLBACK)callback);
}
#endif /* CONFIG_MT53XX_NATIVE_GPIO */

//-----------------------------------------------------------------------------
/** MTGPIO_Reg()  The GPIO Interrupt Callback register funciton. It will copy and keep callback
  * funciton pointer (pfnCallback) in data stucture.
 *  @param i4Gpio the gpio number to read.
 *  @param eType Interrupt type, such as double-edge, rising edge, and falling edge.
 *  @param pfnCallback  Interrupt Callback function
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_Reg (unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    INT32 i4Gpio;
    MTGPIO_TYPE_T eType;
#if 0
    MTPFN_GPIO_CALLBACK pfnCallback;
#endif
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    i4Gpio=( INT32 )rArg.ai4Arg[0];
    eType = (MTGPIO_TYPE_T)(rArg.ai4Arg[1]);
#if 0
    pfnCallback = NULL;
#endif

#if 0
    if( (i4Gpio < 0) || (i4Gpio >= NORMAL_GPIO_NUM) )
    {
        return MTR_NOT_OK;
    }

    _au4GpioCallBackPfn[i4Gpio] = (UINT32) rArg.ai4Arg[2];
    _au4GpioCallBackArg[i4Gpio] = (UINT32) rArg.ai4Arg[3];
    _au4GpioCallBackLevelType[i4Gpio] = 0;

    if (_au4GpioCallBackPfn[i4Gpio] != 0)
    {
        pfnCallback = _GpioPeriodCallbackHandler;
    }
    else
    {
        // deregister, change type to NONE instead of install NULL pointer
        eType = GPIO_TYPE_NONE;
    }
#endif
    MTPERIPHERAL_PRINT("_MTGPIO_Reg - i4Gpio = %d, eType = %d, pfnCallback = 0x%x\n",
        i4Gpio,
        eType,
        (UINT32) rArg.ai4Arg[2]);
    if(_MTGPIO_RegUserCallback(i4Gpio, eType, (UINT32) rArg.ai4Arg[2], (UINT32) rArg.ai4Arg[3]) == 0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-----------------------------------------------------------------------------
/** MTGPIO_SetOut() to set gpio output value.
 *  @param i4GpioNum
 *  @param i4Val 0 or 1.
 *  @retval to return current gpio out register setting.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_SetOut (unsigned long arg)
{
     MTAL_IOCTL_2ARG_T rArg;
     INT32 i4GpioNum;
     INT32 i4Val;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    i4Val=(INT32 )rArg.ai4Arg[1];
    MTPERIPHERAL_PRINT(" - i4GpioNum = %d, i4Val = 0x%x\n",
		i4GpioNum,
		i4Val);

     //add driver implementation code here
     GPIO_SetOut(i4GpioNum, i4Val );

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTGPIO_GetIn()  The GPIO input setting and reading  functions. It will check the
 *  i4GpioNum, set its as an input pin,  and read its polarity register bit to return.
 *  @param i4GpioNum the gpio number to read.
 *  @retval 0 or 1.  (GPIO input value.)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_GetIn (unsigned long arg)
{
     MTAL_IOCTL_2ARG_T rArg;
     INT32 i4GpioNum;
     UINT8 *pi4Val;
     INT8  i4Val = 0;
     MT_RESULT_T ret=MTR_NOT_OK;
	 INT32 i4Mode=0;

     USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
     COPY_FROM_USER_2ARG(arg, rArg);

    i4GpioNum=(INT32)rArg.ai4Arg[0];
    pi4Val=(UINT8*)rArg.ai4Arg[1];

    //add driver implementation code here
    // Set to input mode.
    if ( GPIO_Enable(i4GpioNum, &i4Mode)==i4Mode )
    {
        i4Val = GPIO_GetIn(i4GpioNum);
        if(i4Val >=0)
        {
            ret =MTR_OK;
            *pi4Val = i4Val;

			USR_SPACE_ACCESS_VALIDATE_ARG(pi4Val,UINT8);
			COPY_TO_USER_ARG(pi4Val,i4Val,UINT8);

			MTPERIPHERAL_PRINT(" - i4GpioNum = %d, i4Val = 0x%x\n",
				i4GpioNum,
				i4Val);
        }
        else //error
        {
            ret =MTR_NOT_OK;
            pi4Val = NULL;
        }
    }
    else
    {
        ret =MTR_NOT_SUPPORTED;
        pi4Val = NULL;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/** MTGPIO_PeriodLevelCallback()
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTGPIO_PeriodLevelCallback(unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    INT32 i4Gpio; INT32 i4HighLow; UINT32 u4Period/*mili-second*/; //PFN_GPIO_CALLBACK pfnCallback;
#ifdef CC_PERIODICAL_LEVEL_TRIGGER
	INT32 i4LowRet;
#endif
    UINT32 u4GpioCallBackPfn;
    UINT32 u4GpioCallBackArg;

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);

    i4Gpio=(INT32)rArg.ai4Arg[0];
    i4HighLow=(INT32)rArg.ai4Arg[1];
    u4Period=(UINT32)rArg.ai4Arg[2];
   // pfnCallback = NULL;
/*
    if( (i4Gpio < 0) || (i4Gpio > NORMAL_GPIO_NUM) )
    {
        return MTR_NOT_OK;
    }
*/
    u4GpioCallBackPfn = (UINT32) rArg.ai4Arg[3];//    _au4GpioCallBackPfn[i4Gpio] = (UINT32) rArg.ai4Arg[3];
    u4GpioCallBackArg = (UINT32) rArg.ai4Arg[4];//    _au4GpioCallBackArg[i4Gpio] = (UINT32) rArg.ai4Arg[4];
#if 0
    _ai4GpioCallBackLevelValue[i4Gpio] = i4HighLow;

    if(u4GpioCallBackPfn != 0 )//if (_au4GpioCallBackPfn[i4Gpio] != 0)
    {
        pfnCallback = _GpioPeriodCallbackHandler;
    }
#endif
#ifdef CC_PERIODICAL_LEVEL_TRIGGER   ////period-level callback gpio

    ASSERT(0);
    i4LowRet= GPIO_RegPeriodLevel( i4Gpio, i4HighLow , u4Period , pfnCallback);

    if(i4LowRet==0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }

#else

    if((void *)u4GpioCallBackPfn == NULL)//if(_au4GpioCallBackPfn[i4Gpio] == NULL)
    {
        GPIO_Reg(i4Gpio, GPIO_TYPE_NONE, NULL);
        //_au4GpioCallBackLevelType[i4Gpio] = 0;
    }
    else
    {
    MTPERIPHERAL_PRINT("_MTGPIO_PeriodLevelCallback - i4Gpio = %d, i4HighLow = %d, u4Period = %d, pfnCallback = 0x%x\n",
        i4Gpio,
        i4HighLow,
        u4Period,
        u4GpioCallBackPfn);

        if(!i4HighLow)
        {
            if(_MTGPIO_RegUserCallback(i4Gpio, GPIO_TYPE_INTR_LEVEL_LOW, u4GpioCallBackPfn, u4GpioCallBackArg) == 0)
            {
                return MTR_OK;
            }
            else
            {
                return MTR_NOT_OK;
            }
        }
        else
        {
            if(_MTGPIO_RegUserCallback(i4Gpio, GPIO_TYPE_INTR_LEVEL_HIGH, u4GpioCallBackPfn, u4GpioCallBackArg) == 0)
            {
                return MTR_OK;
            }
            else
            {
                return MTR_NOT_OK;
            }
        }
    }

    return MTR_OK;
#endif
}

void vSetPWMFreqCase(UINT32 u4Freq, MTPWMSRC_TYPE_T eSrc)
{
    _u4PWMFreqCase = u4Freq;
    _eTSrc = eSrc;
}

UINT8 bGetPWMFreqByTiming(void)
{
    UINT8 u1LcdFreq = vDrvGetLCDFreq();
    if ((_u4PWMFreqCase<=0xFF) || (u1LcdFreq == 0))
    {
        return ((UINT8)_u4PWMFreqCase);
    }

    switch(u1LcdFreq)
    {
        case 24:
        case 48:
        case 96:
            return (UINT8)(_u4PWMFreqCase>>8);
            break;
        case 25:
        case 50:
        case 100:
            return (UINT8)(_u4PWMFreqCase>>16);
            break;
        case 30:
        case 60:
        case 120:
            return (UINT8)(_u4PWMFreqCase>>24);
        default:
			if((UINT8)(_u4PWMFreqCase>>16) == 100)
			{
				if (u1LcdFreq < 70)
            {
                u1LcdFreq = u1LcdFreq * 2;
				}
            }
            return u1LcdFreq;
            break;
    }
}

void vResetPWMFreq(void)
{
    UINT8 u1Freq = bGetPWMFreqByTiming();
    MTPWMSRC_TYPE_T e1Src = _eTSrc;
    UINT32 u4DutyOn = _arPwmCache[e1Src].u4DutyOn;
    //UINT32 u4Start = _arPwmCache[e1Src].u4Start;

    u1PWMModeChgflg = 0;

    if(u4Support_ScanPWM)
    {
/*
        _MtpwmSetScanPWM(PWM_PORT_SCAN_SRC, _arPwmCache[PWM_PORT_SCAN_SRC].u4Start,
                        _arPwmCache[PWM_PORT_SCAN_SRC].u4DutyOn, _arPwmCache[PWM_PORT_SCAN_SRC].fgDoubleFreq);
        _MtpwmSetScanPWM(PWM_PORT_SCAN_COPY, _arPwmCache[PWM_PORT_SCAN_COPY].u4Start,
                        _arPwmCache[PWM_PORT_SCAN_COPY].u4DutyOn, _arPwmCache[PWM_PORT_SCAN_COPY].fgDoubleFreq);
*/
    }
    else
    {
        _MtpwmSetFrequency(e1Src, u1Freq, u4DutyOn);
    }
}

//PWM
//-----------------------------------------------------------------------------
/** MTPWM_SetFrequency()  This driver control the PWM0 & PWM1 output
 *  frequence and duty cycle
 *  @param eSrc      PWM source (SrcPWM0/1/2)
 *  @param u4Frequency Frequency demanded (in Hz)
 *  @param u4DutyOn  Duty cycle percentage multiple by 10, ex. u4DutyOn =367 -->36.7% Duty Cycle
 *  @param fgEnable
 *  @return MT_RESULT_T
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPWM_SetFrequency(unsigned long arg)
{
    MT_RESULT_T rRet;
    MTAL_IOCTL_4ARG_T rArg;
    MTPWMSRC_TYPE_T eSrc;
    UINT32 u4Frequency;
    UINT32 u4DutyOn;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    eSrc=( MTPWMSRC_TYPE_T )rArg.ai4Arg[0];
    u4Frequency = (UINT32)rArg.ai4Arg[1];
    u4DutyOn=(UINT32)rArg.ai4Arg[2];
    fgEnable=(BOOL)rArg.ai4Arg[3];

    MTPERIPHERAL_PRINT(" - eSrc = %d\n"
		"u4Frequency = 0x%x\n"
		"u4DutyOn = 0x%x\n"
		"fgEnable = %d\n",
		eSrc,
		u4Frequency,
		u4DutyOn,
		fgEnable);

    if (eSrc > PWM_SRC2)
    {
        return MTR_ERR_INV;
    }

    if (!fgEnable)
    {
        GPIO_SetOut(PWM_GPIO(eSrc), 0);
    }
	else
	{
		vSetPWMFreqCase(u4Frequency, eSrc);
		u4Frequency = bGetPWMFreqByTiming();
		rRet = _MtpwmSetFrequency(eSrc, u4Frequency, u4DutyOn);
		_arPwmCache[eSrc].fgEnable = fgEnable;
		UNUSED(BSP_PinSet(PWM_PINSEL(eSrc), PWM_FUNC(eSrc)));
	}
    return rRet;
}

static MT_RESULT_T _MTPWM_Init(unsigned long arg)
{
	vDrvPWM_Init();
	return MTR_OK;
}

static MT_RESULT_T _MTPWM_ApplyParam(unsigned long arg)
{
	DRV_PWM_PARAM_T _PwmSetting;
    MTAL_IOCTL_2ARG_T rArg;
    DRV_PWM_PIN_SEL_T pwmIndex;
	DRV_PWM_PARAM_T * prPwmSetting_usr;

	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	pwmIndex			= (DRV_PWM_PIN_SEL_T)rArg.ai4Arg[0];
    prPwmSetting_usr 	= (DRV_PWM_PARAM_T *)rArg.ai4Arg[1];

	if (copy_from_user((void *) &_PwmSetting, (const void __user *) prPwmSetting_usr, sizeof(DRV_PWM_PARAM_T)))
    {
		return MTR_NOT_OK;
    }
	//vDrvPWM_ApplyParam(PWM_PIN_SEL_T pwmIndex,PWM_PARAM_T *prPwmSetting);
	vDrvPWM_SetParam(pwmIndex , &_PwmSetting);

	return MTR_OK;
}


//-----------------------------------------------------------------------------
/** MTPWM_SetPhase()  This API controls the phase between PWMs.
 *  Must the same frequency and must both enable.
 *  @param prPwm1 PWM source1 and duty
 *  @param prPwm2 PWM source2 and duty
 *  @param u4Frequency Frequency demanded (in Hz)
 *  @param u1Phase phase between src1 and src2
 *  @return MT_RESULT_T
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPWM_SetPhase(unsigned long arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTPWMSET_TYPE_T *prPwm1, *prPwm2;
    UINT32 u4Frequency;
    UINT8 u1Phase;
    MT_RESULT_T rRet;

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);

    prPwm1 = (MTPWMSET_TYPE_T *)&rArg.ai4Arg[0];
    prPwm2 = (MTPWMSET_TYPE_T *)&rArg.ai4Arg[2];
    u4Frequency = (UINT32)rArg.ai4Arg[4];
    u1Phase = (UINT8)rArg.ai4Arg[5];

    MTPERIPHERAL_PRINT("yyy - eSrc1 = %d, Duty1:0x%x\n"
        "eSrc2 = %d, Duty2:0x%x\n"
		"u4Frequency = 0x%x\n"
		"u1Phase = %d\n",
		prPwm1->ePwmSrc, prPwm1->u4DutyOn,
		prPwm2->ePwmSrc, prPwm2->u4DutyOn,
		u4Frequency,
		(UINT32)u1Phase);

    if ((prPwm1->u4DutyOn > 1000) || (prPwm2->u4DutyOn > 1000))
    {
        return MTR_ERR_INV;
    }

    rRet = _MtpwmSetFrequency(prPwm2->ePwmSrc, u4Frequency, prPwm2->u4DutyOn);
    if (rRet != MTR_OK)
    {
        return rRet;
    }

    rRet = _MtpwmSetFrequency(prPwm1->ePwmSrc, u4Frequency, prPwm1->u4DutyOn);
    if (rRet != MTR_OK)
    {
        return rRet;
    }

    return _MtpwmTriggerPhaseTimer(prPwm1->ePwmSrc, prPwm2->ePwmSrc, u4Frequency, u1Phase);
}


//Servo ADC
//-----------------------------------------------------------------------------
/** MTPDWNC_ReadServoADCChannelValue() Read the ServoADC Channel Value
 *  @param u4Channel the channel number
 *  @retval the channel adc value (0-255).
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_ReadServoADCChannelValue(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4Channel;
    UINT8* pu1Value;
    UINT8 u1Value;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4Channel = (UINT32)rArg.ai4Arg[0];
    pu1Value = (UINT8*)rArg.ai4Arg[1];

    // Validate servo ADC number
     if(u4Channel >= TOTAL_SERVOADC_NUMBER)
     {
         return MTR_NOT_SUPPORTED;
     }

     u1Value = PDWNC_ReadServoADCChannelValue(u4Channel);
     *pu1Value = u1Value;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Value,UINT8);
    COPY_TO_USER_ARG(pu1Value, u1Value, UINT8);

    MTPERIPHERAL_PRINT(" - u4Channel = 0x%x, u1Value = 0x%x\n",
		(int)u4Channel,
		(int)u1Value);
    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTPDWNC_EnterStandby
 *  Let ARM enter standby mode
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_EnterStandby (unsigned long arg)
{
    UINT32 u4PowerDownType;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, u4PowerDownType);

    PDWNC_EnterPowerDown(_u4PowerDownEn, u4PowerDownType);

    return MTR_OK;
}

extern INT32 PDWNC_T8032Cmd(PDWNC_T8032_CMD_T* prCmd, PDWNC_T8032_RESPONSE_T* prResponse);

//-------------------------------------------------------------------------
/** _MTPDWNC_EnterPowerDown
 *  Let ARM enter standby mode
 *  @param  _u4PowerDownEn
 *  @param  u4PowerDownType
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_EnterPowerDown (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	UINT32 _u4PowerDownEn;
	UINT32 u4PowerDownType;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    _u4PowerDownEn = (UINT32)rArg.ai4Arg[0];
    u4PowerDownType = (UINT32)rArg.ai4Arg[1];
	#ifdef CC_CUSTOM_CONTROL_WAKEUP
    if(_fgExternalRtcWakeUp)
    {
        _u4PowerDownEn |= (UINT32)WAK_RTC;
    }
	else
	{
	    _u4PowerDownEn &= ~((UINT32)WAK_RTC);
	}
	#endif
    PDWNC_EnterPowerDown(_u4PowerDownEn, u4PowerDownType);

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTPDWNC_RequestSuspendEvent
 *  Let ARM enter standby mode
 *  @param  _u4PowerDownEn
 *  @param  u4PowerDownType
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_RequestSuspendEvent (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	UINT32 u4SuspendEn;
	UINT32 u4SuspendType;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4SuspendEn = (UINT32)rArg.ai4Arg[0];
    u4SuspendType = (UINT32)rArg.ai4Arg[1];
	#ifdef CC_CUSTOM_CONTROL_WAKEUP
	printf("_MTPDWNC_RequestSuspendEvent set external RTC control (%d)\n",_fgExternalRtcWakeUp);
    if(_fgExternalRtcWakeUp)
    {
        u4SuspendEn |= (UINT32)WAK_RTC;
    }
	else
	{
	    u4SuspendEn &= ~((UINT32)WAK_RTC);
	}
	#endif    
    PDWNC_RequestSuspendEvent(u4SuspendEn, u4SuspendType);
    return MTR_OK;
}


//-------------------------------------------------------------------------
/** _MTPDWNC_EnterPowerState
 *  Let ARM enter standby mode
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_EnterPowerState (unsigned long arg)
{
    UINT32 u4PDWNEvent;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, u4PDWNEvent);

    PDWNC_EnterPowerState(u4PDWNEvent);

    return MTR_OK;
}
//-------------------------------------------------------------------------
/** _MTPDWNC_Reboot
 *  reboot IC by watchdog
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
MT_RESULT_T _MTPDWNC_Reboot (VOID)
{
    MTPERIPHERAL_PRINT(" - PDWNC reboot \n");
	PDWNC_Reboot();

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTPDWNC_Reboot1
 *  reboot IC by watchdog
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
MT_RESULT_T _MTPDWNC_Reboot1 (VOID)
{
    MTPERIPHERAL_PRINT(" - PDWNC reboot1 \n");
	PDWNC_Reboot1(); 
	
    return MTR_OK;
}

//----------------------------------------------------------------------------
/** _MTPDWNC_PowerDownByAcCut()
 *  @param  u4Reason
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
MT_RESULT_T  _MTPDWNC_PowerDownByAcCut (unsigned long arg)
{
    UINT32 u4Reason;

    u4Reason = PDWNC_PowerDownByAcCut();

	USR_SPACE_ACCESS_VALIDATE_ARG(arg, UINT32);
    COPY_TO_USER_ARG(arg, u4Reason, UINT32);

    return MTR_OK;
}
//-------------------------------------------------------------------------
/** _MTPDWNC_T8032Cmd
 *  ARM/T8032 command interface
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_T8032Cmd (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTPDWNC_T8032_CMD_T    *usr_prCmd;
    PDWNC_T8032_CMD_T rCmd;

    MTPDWNC_T8032_RESPONSE_T    *usr_prRes;
    PDWNC_T8032_RESPONSE_T rRes;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    usr_prCmd = (MTPDWNC_T8032_CMD_T*)rArg.ai4Arg[0];
    usr_prRes = (MTPDWNC_T8032_RESPONSE_T*)rArg.ai4Arg[1];


    USR_SPACE_ACCESS_VALIDATE_ARG(usr_prCmd, MTPDWNC_T8032_CMD_T);
    COPY_FROM_USER_ARG(usr_prCmd, rCmd, MTPDWNC_T8032_CMD_T);

    if (PDWNC_T8032Cmd(&rCmd, &rRes) < 0)
    {
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_prRes, PDWNC_T8032_RESPONSE_T);
    COPY_TO_USER_ARG(usr_prRes, rRes, PDWNC_T8032_RESPONSE_T);
    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTPDWNC_GetWakeupReason
 *  Query system wakeup reason
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_GetWakeupReason(unsigned long arg)
{
    UINT32 *pu4Val;
     UINT32  u4Reason;
    UINT32 rArg;

    pu4Val=(UINT32*)arg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,rArg);
  	//add driver implementation code here
    u4Reason = PDWNC_ReadWakeupReason();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,UINT32);
    COPY_TO_USER_ARG(pu4Val, u4Reason, UINT32);

    MTPERIPHERAL_PRINT(" - size = 0x%x \n",u4Reason);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/* Name: _CheckCustomWakeupReason
 *
 * Description: This API implements to check the key on/off at initial time.
 */
// ----------------------------------------------------------------------------
static MT_WAKE_UP_REASON_T  _CheckCustomWakeupReason(VOID)
{
    INT32 i, i4Val;
    UINT32 u4TheKey, u4Val;
    SGL_GPIO_T  *prSingleGpioKeyList = NULL;
    SGL_SERVO_T *prSingleServoKeyList = NULL;
    GREED_KEY_T *prGreedGpioKeyList = NULL;
	UINT32	 _u4TheInitKey = 0;
	MT_WAKE_UP_REASON_T	_rPCWakeupReason = (MT_WAKE_UP_REASON_T)0;

    if (DRVCUST_OptQuery(ePcdWakeupTheKey, &u4TheKey) < 0)
    {
        return _rPCWakeupReason;
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
                _rPCWakeupReason = MT_WAKE_UP_REASON_CUSTOM_1;
                return _rPCWakeupReason;
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
                _rPCWakeupReason = MT_WAKE_UP_REASON_CUSTOM_1;
                return _rPCWakeupReason;
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
                _rPCWakeupReason = MT_WAKE_UP_REASON_CUSTOM_1;
                return _rPCWakeupReason;
            }
        }
        i++;
    }

	return _rPCWakeupReason; //MT_WAKE_UP_REASON_UNKNOWN

}
//-----------------------------------------------------------------------------
/* Name: _MTCheckCustomWakeupReason
 *
 * Description: This API implements to check the key on/off at initial time.
 */
// ----------------------------------------------------------------------------
static MT_RESULT_T _MTCheckCustomWakeupReason(unsigned long arg)
{

	MT_WAKE_UP_REASON_T _rPCWakeupReason;

    //add driver implementation code here
    _rPCWakeupReason = _CheckCustomWakeupReason();

	USR_SPACE_ACCESS_VALIDATE_ARG(arg, MT_WAKE_UP_REASON_T);
	COPY_TO_USER_ARG(arg, _rPCWakeupReason, MT_WAKE_UP_REASON_T);

	return MTR_OK;

}


//-----------------------------------------------------------------------------
/** _MTPDWNC_SetPowerDownReason
 *  Set power down reason
 *  @param _rSetPowerDownReason,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
//
static MT_RESULT_T _MTPDWNC_SetPowerDownReason(unsigned long arg)
{
    PDWNC_SET_POWER_DOWN_REASON_T _rSetPowerDownReason;
	PDWNC_SET_POWER_DOWN_REASON_T *prReason;

    prReason = (PDWNC_SET_POWER_DOWN_REASON_T *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prReason,PDWNC_SET_POWER_DOWN_REASON_T);
    COPY_FROM_USER_ARG(prReason,_rSetPowerDownReason,PDWNC_SET_POWER_DOWN_REASON_T);

  //add driver implementation code here
  	PDWNC_SetPowerDownReason(_rSetPowerDownReason);

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTPDWNC_SetWakeupSource
 *  Set wakeup
 *  @param pu4Key,
 *  @param pu4Raw,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_SetWakeupSource(unsigned long arg)
{
    UINT32 u4Arg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, u4Arg);

    // Some necessary check...

    _u4PowerDownEn = u4Arg;

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTPDWNC_SetWakeupSource
 *  Set wakeup
 *  @param pu4Key,
 *  @param pu4Raw,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_SetRtcWakeupEnable(unsigned long arg)
{
    UINT32 u4RtcWakUpEn=FALSE;

	USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
	COPY_FROM_USER_1ARG(arg, u4RtcWakUpEn);

    if(u4RtcWakUpEn)
    {
        _fgExternalRtcWakeUp = TRUE;
    }
	else
	{
        _fgExternalRtcWakeUp = FALSE;
	}
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTPDWNC_CreateWatchDogThread
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_CreateWatchDogThread(unsigned long arg)
{
    UNUSED(arg);

    PDWNC_CreateWatchDogThread();

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTPDWNC_PwmSetPwm
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_PwmSetPwm(unsigned long arg)
{
	UINT32 u4Path,u4Enable,u4Frequence,u4DutyCycle;
	MTAL_IOCTL_4ARG_T rArg;
	USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
	COPY_FROM_USER_4ARG(arg, rArg);

	u4Path		=(UINT32)rArg.ai4Arg[0];
	u4Enable	=(UINT32)rArg.ai4Arg[1];
	u4Frequence	=(UINT32)rArg.ai4Arg[2];
	u4DutyCycle	=(UINT32)rArg.ai4Arg[3];
	
    if(PDWNC_PwmSetPwm(u4Path,u4Enable,u4Frequence,u4DutyCycle))
    	return MTR_OK;
	else
		return MTR_NOT_OK;
}

//-----------------------------------------------------------------------------
/** _MTPDWNC_PwmSetBreathLight
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_PwmSetBreathLight(unsigned long arg)
{
	UINT32 u4Path,u4Enable,u4Cycle;
	MTAL_IOCTL_3ARG_T rArg;
	USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
	COPY_FROM_USER_3ARG(arg, rArg);

	u4Path	=(UINT32)rArg.ai4Arg[0];
	u4Enable=(UINT32)rArg.ai4Arg[1];
	u4Cycle	=(UINT32)rArg.ai4Arg[2];
	
    if(PDWNC_PwmSetBreathLight(u4Path,u4Enable,u4Cycle))
    	return MTR_OK;
	else
		return MTR_NOT_OK;
}



//SIF
//-------------------------------------------------------------------------
/** MTSIF_Init
 *  Initialize SIF module.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIF_Init (VOID)
{

   MTPERIPHERAL_PRINT(" - void \n");

     //add driver implementation code here
    SIF_Init();

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** MTSIF_Stop
 *  terminate SIF module.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static  MT_RESULT_T  _MTSIF_Stop (VOID)
{
	MTPERIPHERAL_PRINT(" - void \n");

     //add driver implementation code here
     //SIF_Stop();

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** MTSIF_WriteMultipleSubAddr
 *  write data with word address to SIF module.
 *  @param  ePort  	SIF Port ID
 *  @param  u4ClkVal 	Demaned Sif Clock in Khz
 *  @param  u1DevAddr 	Serial interface device address.
 *  @param  pu1Addr 	Pointer to Serial interface word address.
 *                      *pu1Addr is sent first on bus.
 *  @param  u2AddrCnt 	Serial interface word address count (up to 3).
 *  @param  pu1Data 	Pointer to data
 *  @param  u2DataCnt 	Number of byte to read. Max = 256 bytes.
 *  @retval   MT_RESULT_T
 *            MTR_OK for OK
 *            MTR_PARAMETER_ERROR for wrong parameter,
 *            MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIF_WriteMultipleSubAddr (unsigned long arg )
{
//MTSIFPORT_TYPE_T ePort, UINT32 u4ClkVal, UINT8 u1DevAddr,
//       UINT8 *pu1Addr, UINT16 u2AddrCnt, UINT8 *pu1Data, UINT16 u2DataCnt
    MTAL_IOCTL_8ARG_T rArg;
    MTSIFPORT_TYPE_T ePort;
    UINT32 u4ClkVal;
    UINT8 u1DevAddr;
    UINT8 *pu1Addr = NULL;
    UINT16 u2AddrCnt;
    UINT8 *pu1Data_User = NULL;
    UINT16 u2DataCnt;
    UINT8 * pu1KernelBuf = NULL;
    UINT32 u4WordAddr = 0;
    UINT8 *pu1AddrKrnl = NULL;
    SIF_BUS_ID_T mtDrv_SifBusId;
    UINT16 u2Ret;

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);

    ePort = (MTSIFPORT_TYPE_T)rArg.ai4Arg[0];
    u4ClkVal = (UINT32)rArg.ai4Arg[1];
    u1DevAddr = (UINT8)rArg.ai4Arg[2];
    pu1Addr = (UINT8 *)rArg.ai4Arg[3];
    u2AddrCnt = (UINT16)rArg.ai4Arg[4];
    pu1Data_User = (UINT8 *)rArg.ai4Arg[5];
    u2DataCnt = (UINT16)rArg.ai4Arg[6];

    MTPERIPHERAL_PRINT(" - ePort = %d\n"
		"u4ClkVal = 0x%x\n"
		"u1DevAddr = 0x%x\n"
		"u2AddrCnt = 0x%x\n"
		"u2DataCnt = 0x%x\n",
		ePort,
		u4ClkVal,
		u1DevAddr,
		u2AddrCnt,
		u2DataCnt);

    //parameter check
    if ((u2AddrCnt > 3) || (u4ClkVal == 0) || (pu1Data_User == NULL) || (u2DataCnt == 0))
    {
        return MTR_PARAMETER_ERROR;
    }

    if (u2AddrCnt && !pu1Addr)
    {
        return MTR_PARAMETER_ERROR;
    }

    //add driver implementation code here
    pu1KernelBuf = kmalloc(u2DataCnt, GFP_KERNEL);
    if (!pu1KernelBuf)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (ePort == SIF_PORT_0)
    {
        mtDrv_SifBusId = SIF_BUS_SYSTEM_I2C;
    }
    else if (ePort == SIF_PORT_1)
    {
        mtDrv_SifBusId = SIF_BUS_TUNER_I2C;
    }
    else if (ePort == SIF_PORT_TUNER)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C;
    }
    else if (ePort == SIF_PORT_PDWNC)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C_1;
    }
	//#if defined(CC_MT5396)
	else if (ePort == SIF_PORT_PDWNC_1)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C_1;
    }
	//#endif
    else
    {
		kfree(pu1KernelBuf);
		return MTR_PARAMETER_ERROR;
    }

    if (copy_from_user((void *) pu1KernelBuf, (const void __user *) pu1Data_User, u2DataCnt))
    {
        kfree(pu1KernelBuf);
        return MTR_PARAMETER_ERROR;
    }

    if (u2AddrCnt && pu1Addr) //with address
    {
        int i;

        pu1AddrKrnl = kmalloc(u2AddrCnt, GFP_KERNEL);
        if (!pu1AddrKrnl)
        {
            kfree(pu1KernelBuf);
            return MTR_PARAMETER_ERROR;
        }

        if(copy_from_user((void *) pu1AddrKrnl, (const void __user *) pu1Addr, u2AddrCnt))
    	{
			kfree(pu1KernelBuf);
			kfree(pu1AddrKrnl);
			return MTR_NOT_OK;
    	}

        for (i = 0; i < u2AddrCnt; i++)
        {
            u4WordAddr = u4WordAddr << 8;
            u4WordAddr |= *(pu1AddrKrnl + i);
        }
    }

    u2Ret = SIF_X_Write(mtDrv_SifBusId, (27000/u4ClkVal), u1DevAddr, u2AddrCnt,
                        u4WordAddr, pu1KernelBuf, u2DataCnt);

    if (pu1KernelBuf)
    {
        kfree(pu1KernelBuf);
    }
    if (pu1AddrKrnl)
    {
        kfree(pu1AddrKrnl);
    }

    if (u2Ret)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-------------------------------------------------------------------------
/** MTSIF_ReadMultipleSubAddr
 *  read data with word address from SIF module.
 *  @param  ePort   	SIF Port ID
 *  @param  u4ClkVal 	Demaned Sif Clock in Khz
 *  @param  u1DevAddr 	Serial interface device address.
 *  @param  pu1Addr 	Pointer to Serial interface word address.
 *                      *pu1Addr is sent first on bus.
 *  @param  u2AddrCnt 	Serial interface word address count (up to 3).
 *  @param  pu1Data 		Pointer to data
 *  @param  u2DataCnt 	Number of byte to read. Max = 256 bytes.
 *  @retval   MT_RESULT_T
 *            MTR_OK for OK
 *            MTR_PARAMETER_ERROR for wrong parameter,
 *            MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIF_ReadMultipleSubAddr (unsigned long arg)
{
//MTSIFPORT_TYPE_T ePort, UINT32 u4ClkVal, UINT8 u1DevAddr,
//       UINT8 *pu1Addr, UINT16 u2AddrCnt, UINT8 *pu1Data, UINT16 u2DataCnt
    MTAL_IOCTL_8ARG_T rArg;
    MTSIFPORT_TYPE_T ePort;
    UINT32 u4ClkVal;
    UINT8 u1DevAddr;
    UINT8 *pu1Addr = NULL;
    UINT16 u2AddrCnt;
    UINT8 *pu1Data_User = NULL;
    UINT16 u2DataCnt;
    UINT8 * pu1KernelBuf = NULL;
    UINT32 u4WordAddr = 0;
    UINT8 *pu1AddrKrnl = NULL;
    SIF_BUS_ID_T mtDrv_SifBusId;
    UINT16 u2Ret;

    USR_SPACE_ACCESS_VALIDATE_8ARG(arg);
    COPY_FROM_USER_8ARG(arg, rArg);

    ePort = (MTSIFPORT_TYPE_T)rArg.ai4Arg[0];
    u4ClkVal = (UINT32)rArg.ai4Arg[1];
    u1DevAddr = (UINT8)rArg.ai4Arg[2];
    pu1Addr = (UINT8 *)rArg.ai4Arg[3];
    u2AddrCnt = (UINT16)rArg.ai4Arg[4];
    pu1Data_User = (UINT8 *)rArg.ai4Arg[5];
    u2DataCnt = (UINT16)rArg.ai4Arg[6];

    MTPERIPHERAL_PRINT(" - ePort = %d\n"
		"u4ClkVal = 0x%x\n"
		"u1DevAddr = 0x%x\n"
		"u2AddrCnt = 0x%x\n"
		"u2DataCnt = 0x%x\n",
		ePort,
		u4ClkVal,
		u1DevAddr,
		u2AddrCnt,
		u2DataCnt);

    //parameter check
    if( (u2AddrCnt > 3) || (u4ClkVal==0) || (pu1Data_User==NULL) || (u2DataCnt==0))
    {
        return MTR_PARAMETER_ERROR;
    }

    if (u2AddrCnt && !pu1Addr)
    {
        return MTR_PARAMETER_ERROR;
    }

    //add driver implementation code here
    pu1KernelBuf = kmalloc(u2DataCnt, GFP_KERNEL);
    if (!pu1KernelBuf)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (ePort == SIF_PORT_0)
    {
        mtDrv_SifBusId = SIF_BUS_SYSTEM_I2C;
    }
    else if (ePort == SIF_PORT_1)
    {
        mtDrv_SifBusId = SIF_BUS_TUNER_I2C;
    }
    else if (ePort == SIF_PORT_TUNER)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C;
    }
    else if (ePort == SIF_PORT_PDWNC)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C_1;
    }
//#if defined(CC_MT5396)
	else if (ePort == SIF_PORT_PDWNC_1)
    {
        mtDrv_SifBusId = SIF_BUS_PDWNC_I2C_1;
    }
//#endif

    else
    {
		kfree(pu1KernelBuf);
		return MTR_PARAMETER_ERROR;
    }

    if (u2AddrCnt && pu1Addr) //with address
    {
        int i;

        pu1AddrKrnl = kmalloc(u2AddrCnt, GFP_KERNEL);
        if (!pu1AddrKrnl)
        {
            kfree(pu1KernelBuf);
            return MTR_PARAMETER_ERROR;
        }

        if(copy_from_user((void *) pu1AddrKrnl, (const void __user *) pu1Addr, u2AddrCnt))
    	{
            kfree(pu1KernelBuf);
			kfree(pu1AddrKrnl);
			return MTR_NOT_OK;
    	}

        for (i = 0; i < u2AddrCnt; i++)
        {
            u4WordAddr = u4WordAddr << 8;
            u4WordAddr |= *(pu1AddrKrnl + i);
        }
    }

    u2Ret = SIF_X_Read(mtDrv_SifBusId, (27000/u4ClkVal), u1DevAddr, u2AddrCnt,
                        u4WordAddr, pu1KernelBuf, u2DataCnt);

    if(copy_to_user((void __user *)pu1Data_User, (const void *)pu1KernelBuf, u2DataCnt))
	{
        kfree(pu1KernelBuf);
        kfree(pu1AddrKrnl);
		return MTR_PARAMETER_ERROR;
	}
    if (pu1KernelBuf)
    {
        kfree(pu1KernelBuf);
    }
    if (pu1AddrKrnl)
    {
        kfree(pu1AddrKrnl);
    }

    if (u2Ret)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}
//SIFHW
//-------------------------------------------------------------------------
/** _MTSIFSW_Init
 *  Initialize SIFSW module.
 *  @param  u4SDA/u4SCL: data_gpio_id/addr_gpio_id
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIFSW_Init (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4SDA;
    UINT32 u4SCL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4SDA = (UINT32)rArg.ai4Arg[0];
    u4SCL = (UINT32)rArg.ai4Arg[1];

    //add driver implementation code here
    SIFSW_Init(u4SDA, u4SCL);

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** MTSIFSW_WriteDataNoSubAddr
  * I2C write command (no ack type)
  *
  * @param bDevAddr device address
  * @param bDataCount numbber of bytes to be sent out
  * @param pbData pointer of data to be sent out
  * @param bOption options for this command
  * @param bOption bit 2-0: bit delay (us)
  * @param bOption bit 5-3: UINT8 delay (us)
  * @param bOption bit 6: need to check ACK
  * @param bOption bit 7: Reserve
  * @param bOption bit 8: halt and wait for ACK (full implementation of spec ch.7.2)
  * @param bOption bit 9: Not to send stop bit at the end of transaction
  * @param bOption bit10: Not to send start bit at the beginning of transaction
  *  @retval   MT_RESULT_T   MTR_OK for OK, MTR_PARAMETER_ERROR for wrong parameter,
  *                MTR_NOT_OK for execution error
  */
 //-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIFSW_WriteDataNoSubAddr (unsigned long arg )
{
    MTAL_IOCTL_6ARG_T rArg;
    UINT8 bDevAddr;
    UINT8 bDataCount;
    UINT8 *pbData = NULL;
    UINT32 bOption;
    UINT16 u2Delay;
    UINT16 u2SyncTimeout;


    UINT8 * pu1KernelData = NULL;
    UINT8 u1Ret;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    bDevAddr = (UINT8)rArg.ai4Arg[0];
    bDataCount = (UINT8)rArg.ai4Arg[1];
    pbData = (UINT8*)rArg.ai4Arg[2];
    bOption = (UINT32)rArg.ai4Arg[3];
    u2Delay = (UINT16)rArg.ai4Arg[4];
    u2SyncTimeout = (UINT16)rArg.ai4Arg[5];

    //parameter check
    if ((pbData == NULL) || (bDataCount == 0))
    {
        return MTR_PARAMETER_ERROR;
    }
    pu1KernelData = kmalloc(bDataCount, GFP_KERNEL);
    if (!pu1KernelData)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (copy_from_user((void *) pu1KernelData, (const void __user *) pbData, bDataCount))
    {
        kfree(pu1KernelData);
        return MTR_PARAMETER_ERROR;
    }

    MTPERIPHERAL_PRINT(" - _MTSIFSW_WriteDataNoSubAddr"
		"bDevAddr = 0x%x\n"
        "bDataCount = 0x%x\n",
        "pu1KernelData[0] = 0x%x\n"
		"bOption = 0x%x"
		"u2Delay = %d\n",
		bDevAddr,
		bDataCount ,
		pu1KernelData[0] ,
		bOption,
		u2Delay);


    //add driver implementation code here

    u1Ret= SIFSW_WriteDataNoSubAddr(bDevAddr, bDataCount, pu1KernelData, bOption, u2Delay,u2SyncTimeout);

    if (pu1KernelData)
    {
        kfree(pu1KernelData);
    }
    if (u1Ret == bDataCount)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-------------------------------------------------------------------------
/** _MTSIFSW_ReadDataNoSubAddr
   * I2C read command (no ack type)
   *
   * @param bDevAddr device address
   * @param bDataCount numbber of bytes to be rad
   * @param pbData pointer of data to be read
   * @param bOption options for this command
   * @param bOption bit 2-0: bit delay (us)
   * @param bOption bit 5-3: UINT8 delay (us)
   * @param bOption bit 6: need to check ACK
   * @param bOption bit 7: Reserve
   * @param bOption bit 8: halt and wait for ACK (full implementation of spec ch.7.2)
   * @param bOption bit 9: Reserve
   * @param bOption bit10: Not to send start bit at the beginning of transaction
  *  @retval   MT_RESULT_T   MTR_OK for OK, MTR_PARAMETER_ERROR for wrong parameter,
  *                MTR_NOT_OK for execution error
  */
 //-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIFSW_ReadDataNoSubAddr (unsigned long arg)
{
    MTAL_IOCTL_6ARG_T rArg;
    UINT8 bDevAddr;
    UINT8 bDataCount;
    UINT8 *pbData = NULL;
    UINT32 bOption;
    UINT16 u2Delay;
    UINT16 u2SyncTimeout;

    UINT8 * pu1KernelData = NULL;
    UINT8 u1Ret;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

    bDevAddr = (UINT8)rArg.ai4Arg[0];
    bDataCount = (UINT8)rArg.ai4Arg[1];
    pbData = (UINT8*)rArg.ai4Arg[2];
    bOption = (UINT32)rArg.ai4Arg[3];
    u2Delay = (UINT16)rArg.ai4Arg[4];
    u2SyncTimeout = (UINT16)rArg.ai4Arg[5];

    //parameter check
    if ((pbData == NULL) || (bDataCount == 0))
    {
        return MTR_PARAMETER_ERROR;
    }
    pu1KernelData = kmalloc(bDataCount, GFP_KERNEL);
    if (!pu1KernelData)
    {
        return MTR_PARAMETER_ERROR;
    }

    MTPERIPHERAL_PRINT(" - _MTSIFSW_ReadDataNoSubAddr"
		"bDevAddr = 0x%x\n"
        "bDataCount = 0x%x\n",
		"bOption = 0x%x"
		"u2Delay = %d\n",
		bDevAddr,
		bDataCount ,
		bOption,
		u2Delay);

    //add driver implementation code here

    u1Ret= SIFSW_ReadDataNoSubAddr(bDevAddr, bDataCount, pu1KernelData, bOption, u2Delay,u2SyncTimeout);

    if(copy_to_user((void __user *)pbData, (const void *)pu1KernelData, bDataCount))
    {
		if (pu1KernelData)
		{
			kfree(pu1KernelData);
		}
        return MTR_ERR_INV;
    }

    if (pu1KernelData)
    {
        kfree(pu1KernelData);
    }
    if (u1Ret == bDataCount)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-------------------------------------------------------------------------
/** MTSIFSW_X_Read
 * I2C read command (general type)
 *
 * @param bDevAddr device address
 * @param bSubAddr sub-address
 * @param bDataCount numbber of bytes to be read
 * @param pbData pointer of data to be read
 * @param bOption options for this command
 * @param bOption bit 2-0: bit delay (us)
 * @param bOption bit 5-3: UINT8 delay (us)
 * @param bOption bit 6: need to check ACK
 * @param bOption bit 7: Reserve
 * @param bOption bit 8: halt and wait for ACK (full implementation of spec ch.7.2)
 * @return 1 or 0
 * @retval = 1 Read success
 * @retval = 0 Read fail
 */

 //-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIFSW_X_Read (unsigned long arg )
{
    MTAL_IOCTL_6ARG_T rArg;
	UINT16 u2ClkDiv;
    UINT8 u1Ret;
	UINT8 u1DevAddr;
	UINT8 u1WordAddrNum;
	UINT32 u4WordAddr;
	UINT8 * pu1KernelData = NULL;
	UINT8 *pbData = NULL;
	UINT16 u2ByteCnt;

	USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

	u2ClkDiv = (UINT16)rArg.ai4Arg[0];
    u1DevAddr = (UINT8)rArg.ai4Arg[1];
    u1WordAddrNum = (UINT8)rArg.ai4Arg[2];
    u4WordAddr = (UINT32)rArg.ai4Arg[3];
	pbData = (UINT8 *)rArg.ai4Arg[4];
    u2ByteCnt = (UINT16)rArg.ai4Arg[5];

	   //parameter check
    if ((pbData == NULL) || (u2ByteCnt == 0))
    {
        return MTR_PARAMETER_ERROR;
    }

    pu1KernelData = kmalloc(u2ByteCnt, GFP_KERNEL);
    if (!pu1KernelData)
    {
        return MTR_PARAMETER_ERROR;
    }

    MTPERIPHERAL_PRINT(" - _MTSIFSW_X_Read"
		"u1DevAddr = 0x%x\n"
        "u1WordAddrNum = 0x%x\n",
		"u4WordAddr = 0x%x"
		"u2ByteCnt = %d\n",
		u1DevAddr,
		u1WordAddrNum ,
		u4WordAddr,
		u2ByteCnt);

	u1Ret= SIFSW_X_Read((UINT8)0, (27000/u2ClkDiv),u1DevAddr, u1WordAddrNum, u4WordAddr, pu1KernelData, u2ByteCnt);

    if(copy_to_user((void __user *)pbData, (const void *)pu1KernelData, u2ByteCnt))
	{
        kfree(pu1KernelData);
        return MTR_NOT_OK;
	}

    if (pu1KernelData)
    {
        kfree(pu1KernelData);
    }
    if (u1Ret == TRUE)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }

}

//-------------------------------------------------------------------------
/** MTSIFSW_X_Write
 * I2C read command (general type)
 *
 * @param bDevAddr device address
 * @param bSubAddr sub-address
 * @param bDataCount numbber of bytes to be read
 * @param pbData pointer of data to be read
 * @param bOption options for this command
 * @param bOption bit 2-0: bit delay (us)
 * @param bOption bit 5-3: UINT8 delay (us)
 * @param bOption bit 6: need to check ACK
 * @param bOption bit 7: Reserve
 * @param bOption bit 8: halt and wait for ACK (full implementation of spec ch.7.2)
 * @return 1 or 0
 * @retval = 1 read success
 * @retval = 0 read fail
 */
 //-------------------------------------------------------------------------
static MT_RESULT_T  _MTSIFSW_X_Write (unsigned long arg)
{

	MTAL_IOCTL_6ARG_T rArg;
	UINT16 u2ClkDiv;
    UINT8 u1Ret;
	UINT8 u1DevAddr;
	UINT8 u1WordAddrNum;
	UINT32 u4WordAddr;
	UINT8 * pu1KernelData = NULL;
	UINT8 *pu1Buf = NULL;
	UINT16 u2ByteCnt;

	USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

	u2ClkDiv = (UINT16)rArg.ai4Arg[0];
    u1DevAddr = (UINT8)rArg.ai4Arg[1];
    u1WordAddrNum = (UINT8)rArg.ai4Arg[2];
    u4WordAddr = (UINT32)rArg.ai4Arg[3];
    pu1Buf = (UINT8 *)rArg.ai4Arg[4];
	u2ByteCnt = (UINT16)rArg.ai4Arg[5];
	if ((pu1Buf == NULL) || (u2ByteCnt == 0))
    {
        return MTR_PARAMETER_ERROR;
    }
	 pu1KernelData = kmalloc(u2ByteCnt, GFP_KERNEL);
	if (!pu1KernelData)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (copy_from_user((void *) pu1KernelData, (const void __user *) pu1Buf, u2ByteCnt))
    {
        kfree(pu1KernelData);
        return MTR_PARAMETER_ERROR;
    }
	 MTPERIPHERAL_PRINT(" - _MTSIFSW_X_Write"
		"u2ClkDiv = 0x%x\n"
        "u1DevAddr = 0x%x\n",
        "u1WordAddrNum[0] = 0x%x\n"
		"u4WordAddr = 0x%x"
		"u2Delay = %d\n",
		u2ClkDiv,
		u1DevAddr ,
		u1WordAddrNum,
		u4WordAddr,
		u2ByteCnt);
	 //add driver implementation code here
	u1Ret= SIFSW_X_Write((UINT8 )0, (27000/u2ClkDiv), u1DevAddr, u1WordAddrNum, u4WordAddr,pu1Buf,u2ByteCnt);

	 if (pu1KernelData)
    {
        kfree(pu1KernelData);
    }
    if (u1Ret == TRUE)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}
//SIFSW
//-------------------------------------------------------------------------
/** _MTSIFSW_Remove
 *  terminate SIF module.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
MT_RESULT_T  _MTSIFSW_Remove (VOID)
{
	SIFSW_Remove();
    return MTR_OK;
}

//RTC
//----------------------------------------------------------------------------
/** MTRTC_RtcToUtc() Translate RTC to UTC
 *  @param prRtc, Input, The RTC to be translated
 *  @param pu8Utc, Output, The translated UTC
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_RtcToUtc (unsigned long arg)
{
     MTAL_IOCTL_3ARG_T rArg;
     UINT64 *pu8Utc=NULL;
     const MTRTC_T *prRtc=NULL;
     UINT64 u8Utc;
     BOOL fret;
     MT_RESULT_T mtret;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    pu8Utc = (UINT64 *)rArg.ai4Arg[0];
    prRtc=(MTRTC_T *)(&rArg.ai4Arg[1]);

     //add driver implementation code here
     fret =RTC_RtcToUtc(&u8Utc, (RTC_T *)prRtc);
     mtret = fret?MTR_OK:MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu8Utc, UINT64);
    COPY_TO_USER_ARG(pu8Utc, u8Utc, UINT64);

    MTPERIPHERAL_PRINT(" -u8Utc = 0x%08x08%x\n",(INT32)(u8Utc>>32),(INT32)u8Utc);
    MTMPLAYER_RTC_PRINT("prRtc", prRtc);

    return mtret;
}

//----------------------------------------------------------------------------
/** MTRTC_UtcToRtc() Translate UTC to RTC
 *  @param u8Utc, Input, The UTC to be translated
 *  @param prRtc, Output, The translated RTC
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_UtcToRtc (unsigned long arg)
{
     MTAL_IOCTL_3ARG_T rArg;
     MTRTC_T *prRtc=NULL;
     UINT64  u8Utc;
     MTRTC_T rRtc;
     BOOL fret;
     MT_RESULT_T mtret;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    prRtc=(MTRTC_T *)rArg.ai4Arg[0];
    u8Utc = *(UINT64*)(&rArg.ai4Arg[1]);

 //add driver implementation code here
    fret = RTC_UtcToRtc((RTC_T *)&rRtc, u8Utc);
    mtret = fret?MTR_OK:MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(prRtc, MTRTC_T);
    COPY_TO_USER_ARG(prRtc, rRtc, MTRTC_T);

    MTPERIPHERAL_PRINT(" -u8Utc = 0x%08x08%x\n",(INT32)(u8Utc>>32),(INT32)u8Utc);
    MTMPLAYER_RTC_PRINT("prRtc", prRtc);

    return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_Init() Initialize RTC driver and hardware
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_Init (VOID)
{
     MTPERIPHERAL_PRINT(" - void \n");

      //add driver implementation code here
     RTC_Init();

     RTC_HookAlertHandler((PFN_RTC_ALERT_HANDLER_T)_RtcAlertHandler);

     return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_GetTimeDate() Get time from RTC
 *  @param prTime, Output, The time
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_GetTimeDate (unsigned long arg)
{
    const UINT64 *prTime;
    UINT64  rTime;

    prTime =(UINT64*)arg;

  //add driver implementation code here
    if(RTC_GetTimeDate( &rTime ) == false)
	{
    	return MTR_NOT_OK;
	}

    USR_SPACE_ACCESS_VALIDATE_ARG(prTime,UINT64);
    COPY_TO_USER_ARG(prTime,rTime,UINT64);
    MTPERIPHERAL_PRINT(" - rTime = 0x%08x08%x\n",(INT32)(rTime>>32),(INT32)rTime);

    return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_SetTimeDate() Set time to RTC
 *  @param prTime, Input, The time to set.
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_SetTimeDate (unsigned long arg)
{
    const UINT64 *prTime;
    UINT64 rTime;
    prTime = (UINT64 *)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prTime,UINT64);
    COPY_FROM_USER_ARG(prTime,rTime,UINT64);

    MTPERIPHERAL_PRINT(" - rTime = 0x%08x08%x\n",(INT32)(rTime>>32),(INT32)rTime);

    RTC_SetTimeDate( &rTime );

    return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_SetAlertTime() Set time to RTC
 *  @param prTime, Input, The time to set.
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_SetAlertTime (unsigned long arg)
{
    UINT64* pu8Utc;
    UINT64 u8Utc;

    pu8Utc = (UINT64*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(pu8Utc,UINT64);
    COPY_FROM_USER_ARG(pu8Utc,u8Utc,UINT64);
    MTPERIPHERAL_PRINT(" -u8Utc = 0x%08x08%x\n",(INT32)(u8Utc>>32),(INT32)u8Utc);

      //add driver implementation code here
    RTC_SetAlertTime(u8Utc);

    return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_GetAlertTime() Set time to RTC
 *  @param prTime, Input, The time to set.
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_GetAlertTime (unsigned long arg)
{
    UINT64 *prTime=NULL;
    INT64  rTime = 0;

    prTime =(INT64*)arg;

  //add driver implementation code here
    RTC_GetAlertTime( prTime );

    USR_SPACE_ACCESS_VALIDATE_ARG(prTime,INT64);
    COPY_TO_USER_ARG(prTime,rTime,INT64);

    MTPERIPHERAL_PRINT(" - rTime = 0x%08x08%x\n",(INT32)(rTime>>32),(INT32)rTime);
    return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_HookAlertHandler()
 *  @param pfnHandler the callback function.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_HookAlertHandler (unsigned long arg)
{
      MTPFN_RTC_ALERT_HANDLER_T pfnHandler;
      pfnHandler = (MTPFN_RTC_ALERT_HANDLER_T)arg;

      MTPERIPHERAL_PRINT(" - pfnHandler \n");

        //add driver implementation code here
      RTC_HookAlertHandler( pfnHandler);

      return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTRTC_SetWakeup()
 *  @param pfnHandler the callback function.
 */
//----------------------------------------------------------------------------
static MT_RESULT_T  _MTRTC_SetWakeup (unsigned long arg)
{
    MTPFN_RTC_WEAKEUP_HANDLER_T *pfnHandler=NULL;
    pfnHandler =(MTPFN_RTC_WEAKEUP_HANDLER_T *)arg;

    MTPERIPHERAL_PRINT(" - pfnHandler \n");

      //add driver implementation code here
    RTC_SetWakeup((UINT32 *)pfnHandler);

    return MTR_OK;
}

//NOR
//-----------------------------------------------------------------------------
/** to erase data to 0xff on nor flash.
 *  @param u4SectIdx  Sector Index
 *  @param u4SectNum  Sector Numer
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_Erase(unsigned long arg)
{
#if 0
     MTAL_IOCTL_2ARG_T rArg;
     UINT32 u4SectIdx;
     UINT32 u4SectNum;
     INT32 fret;
     MT_RESULT_T mtRet;

     USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
     COPY_FROM_USER_2ARG(arg, rArg);

     u4SectIdx=( UINT32 )rArg.ai4Arg[0];
     u4SectNum=(UINT64 )rArg.ai4Arg[1];

     MTPERIPHERAL_PRINT(" - u4SectIdx = 0x%x, u4SectNum = 0x%x\n",
		u4SectIdx,
		u4SectNum);

  //add driver implementation code here
    fret = NOR_Erase( u4SectIdx,  u4SectNum);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

    return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** erase data to 0xff on nor flash.
 *  @param u4Offset  Offset that start to write
 *  @param u4ByteCount  Size in byte of writting
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_EraseAddr(unsigned long arg)
{
#if 0
     MTAL_IOCTL_2ARG_T rArg;
     UINT32 u4Offset;
     UINT32 u4ByteCount;
     INT32 fret;
     MT_RESULT_T mtRet;

     USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
     COPY_FROM_USER_2ARG(arg, rArg);

     u4Offset=( UINT32 )rArg.ai4Arg[0];
     u4ByteCount=(UINT32 )rArg.ai4Arg[1];
     MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4ByteCount = 0x%x\n",
 		u4Offset,
 		u4ByteCount);

  //add driver implementation code here
    fret = NOR_EraseAddr( u4Offset,  u4ByteCount);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

    return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** get the size of norflash by bytes.
 *  @return Nor flash size in byte
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_GetSize(unsigned long arg)
{
#if 0
    UINT32 *pu4Val;
    UINT32   u4Val;
    INT32 rArg;
    MT_RESULT_T mtRet;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,rArg);

    pu4Val=(UINT32 *)arg;

  //add driver implementation code here
    u4Val = NOR_GetSize();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,UINT32);
    COPY_TO_USER_ARG(pu4Val,u4Val,UINT32);

    MTPERIPHERAL_PRINT(" - size = 0x%x \n",u4Val);
    return MTR_OK;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** read data from nor flash.
 *  @param u4Offset  Position that start to read from
 *  @param pu1Buf    Pointer to storing read-back data
 *  @param u4Len     Length (i.e. size) to read in byte
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_Read(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Offset;
     UINT8* pu1Buf=NULL;
     UINT8   u1Buf;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
     UINT8 *pu1DstBuf = NULL;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset=( UINT32 )rArg.ai4Arg[0];
    pu1Buf=(UINT8*)rArg.ai4Arg[1];
    u4Len=(UINT32)rArg.ai4Arg[2];

  //add driver implementation code here
    pu1DstBuf = kmalloc(u4Len, GFP_KERNEL);
    fret = NOR_Read( u4Offset,  pu1DstBuf,  u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Buf,UINT8);
    COPY_TO_USER_ARG(pu1Buf,u1Buf,UINT8);

    copy_to_user(pu1Buf,pu1DstBuf,u4Len);
    MTPERIPHERAL_PRINT(" - u4Offset = 0x%x,u4Len = 0x%x\n",
		u4Offset,
		u4Len);

    kfree(pu1DstBuf);
     return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** write data to nor flash.
 *  @param u4Offset  Position that start to write to
 *  @param pu1Buf    Pointer to storing write-to data
 *  @param u4Len     Length (i.e. size) to read in byte
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_Write(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Offset;
     UINT8* pu1Buf=NULL;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
     UINT8 *pu1SrcBuf = NULL;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset=( UINT32 )rArg.ai4Arg[0];
    pu1Buf=(UINT8*)rArg.ai4Arg[1];
    u4Len=(UINT32)rArg.ai4Arg[2];
    MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
		u4Len);

  //add driver implementation code here
    pu1SrcBuf = kmalloc(u4Len, GFP_KERNEL);
    copy_from_user(pu1SrcBuf,pu1Buf,u4Len);
    fret = NOR_Write( u4Offset,  pu1SrcBuf,  u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;
    kfree(pu1SrcBuf);

    return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** write data on nor flash and erase automaticallly if need.
 *  @param u4Offset  Position that start to write to
 *  @param pu1MemPtr    Pointer to storing write-to data
 *  @param u4Len     Length (i.e. size) to read in byte
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_WrOnE(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Offset;
     UINT8* pu1MemPtr=NULL;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
     UINT8 *pu1SrcBuf = NULL;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset=( UINT32 )rArg.ai4Arg[0];
    pu1MemPtr=(UINT8*)rArg.ai4Arg[1];
    u4Len=(UINT32)rArg.ai4Arg[2];
    MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
		u4Len);

    pu1SrcBuf = kmalloc(u4Len, GFP_KERNEL);
    copy_from_user(pu1SrcBuf,pu1MemPtr,u4Len);
  //add driver implementation code here
    fret = NOR_WrNoE( u4Offset,  pu1SrcBuf,  u4Len);
    mtRet = (fret==0)?MTR_OK:MTR_NOT_OK;

    kfree(pu1SrcBuf);
    return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** calculate CRC from input source
 *  @param u4Addr, Start Address
 *  @param u4Size, Size beginning from u4Addr
 *  @param pu4Checksum, Memory buffer storing check sum value
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNOR_CalCRC(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Addr;
     UINT32 u4Size;
     UINT32 * pu4Checksum=NULL;
     UINT32 u4Checksum;
     INT32 fret;
     MT_RESULT_T mtRet;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Addr=( UINT32 )rArg.ai4Arg[0];
    u4Size=(UINT32)rArg.ai4Arg[1];
    pu4Checksum=(UINT32 *)rArg.ai4Arg[2];

  //add driver implementation code here
    fret = NOR_CalCRC( u4Addr,  u4Size,  &u4Checksum);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Checksum,UINT32);
    COPY_TO_USER_ARG(pu4Checksum,u4Checksum,UINT32);
    MTPERIPHERAL_PRINT(" - u4Addr = 0x%x\n"
        "u4Size = 0x%x\n"
        "u4Checksum = 0x%x\n",
	 u4Addr,
	 u4Size,
	 u4Checksum);

    return mtRet;
#else
    return MTR_OK;
#endif
}

// NAND
//-----------------------------------------------------------------------------
/** Initialize the nand device and nand driver.
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_Init(VOID)
{
#if 0
    MTPERIPHERAL_PRINT(" - void \n");
  //add driver implementation code here
    NAND_Init();

    return MTR_OK;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** Reset the nand device.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_Reset(VOID)
{
#if 0
    MTPERIPHERAL_PRINT(" - void \n");

      //add driver implementation code here
      NAND_Reset();

    return MTR_OK;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** Read nand chip ID.
 *  @param pu4ID, chip ID
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_ReadID(unsigned long arg)
{
#if 0
    UINT32* pu4ID=NULL;
    UINT32   u4ID;
     INT32 fret;
     MT_RESULT_T mtRet;
    UINT32 rArg;

    pu4ID =(UINT32*)arg;
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,rArg);

  //add driver implementation code here
    fret = NAND_ReadID( &rArg);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ID,UINT32);
    COPY_TO_USER_ARG(pu4ID,rArg,UINT32);

    MTPERIPHERAL_PRINT(" - ID = 0x%x\n",rArg);
    return mtRet;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** Get NAND size.
 *  @return NAND size in page.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_GetSize(unsigned long arg)
{
#if 0
    UINT32 *pu4Val;
    UINT32   u4Val;
     INT64  i8fet;
    UINT32 rArg;

    pu4Val=(UINT32*)arg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,rArg);
  //add driver implementation code here
    i8fet = NAND_GetSize();

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,UINT32);
    COPY_TO_USER_ARG(pu4Val,i8fet,UINT32);

    MTPERIPHERAL_PRINT(" - size = 0x%x \n",i8fet);
    return MTR_OK;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** NAND Read in block
 *  @param u4Offset, block offset to be read
 *  @param pu4Buf, data destination point
 *  @param u4Len, data destination buffer size
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_ReadBlocks(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Offset;
     UINT8* pu1Buf=NULL;
     UINT8   u1Buf;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
     UINT8 *pu1DstBuf = NULL;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

     u4Offset=(UINT32)rArg.ai4Arg[0];
     pu1Buf=(UINT8 *)rArg.ai4Arg[1];
     u4Len=(UINT32)rArg.ai4Arg[2];

  //add driver implementation code here
     pu1DstBuf = kmalloc(u4Len, GFP_KERNEL);
     fret = NAND_Read( u4Offset,  pu1DstBuf,  u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;

     copy_to_user(pu1Buf,pu1DstBuf,u4Len);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Buf,UINT8);

     MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
		u4Len);

    kfree(pu1DstBuf);
    return MTR_OK;
#else
    return MTR_OK;
#endif
}

//-----------------------------------------------------------------------------
/** NAND Write in block
 *  @param u4Offset, block offset to be read
 *  @param pu4Buf, data destination point
 *  @param u4Len, data destination buffer size
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTNAND_WriteBlocks(unsigned long arg)
{
#if 0
     MTAL_IOCTL_3ARG_T rArg;
     UINT32 u4Offset;
     UINT8* pu1Buf=NULL;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
    UINT8 *pu1SrcBuf = NULL;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset=( UINT32 )rArg.ai4Arg[0];
    pu1Buf=(UINT8*)rArg.ai4Arg[1];
    u4Len=(UINT32)rArg.ai4Arg[2];
    MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
             u4Len);

  //add driver implementation code here
    pu1SrcBuf = kmalloc(u4Len, GFP_KERNEL);
    copy_from_user(pu1SrcBuf,pu1Buf,u4Len);
    fret = NAND_EraseAddr(u4Offset,u4Len);
    fret = NAND_Write( u4Offset,  pu1SrcBuf,  u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;
    kfree(pu1SrcBuf);
    return MTR_OK;
#else
    return MTR_OK;
#endif        
}

#if defined(CC_MSDC_ENABLE)
//-----------------------------------------------------------------------------
/** MSDC Read
 *  @param u4Offset, block offset to be read
 *  @param pu4Buf, data destination point
 *  @param u4Len, data destination buffer size
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTMSDC_Read(unsigned long arg)
{
#if 1

     MTAL_IOCTL_4ARG_T rArg;
     UINT32 u4Offset,partId;
     UINT8* pu1Buf=NULL;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
     UINT8 *pu1DstBuf = NULL;
     
     USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
     COPY_FROM_USER_4ARG(arg, rArg);
     
     partId=(UINT32)rArg.ai4Arg[0];
     u4Offset=(UINT32)rArg.ai4Arg[1];
     pu1Buf=(UINT8 *)rArg.ai4Arg[2];
     u4Len=(UINT32)rArg.ai4Arg[3];

  //add driver implementation code here
    pu1DstBuf = kmalloc(u4Len, GFP_KERNEL);
    fret = Loader_ReadMsdc(partId-1, u4Offset, pu1DstBuf, u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;
    
    fret = copy_to_user(pu1Buf,pu1DstBuf,u4Len);
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Buf,UINT8);

     MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
		u4Len);

    kfree(pu1DstBuf);
    return MTR_OK;    
#else
    return MTR_OK;
#endif    
}
//-----------------------------------------------------------------------------
/** MSDC Write
 *  @param u4Offset, block offset to be read
 *  @param pu4Buf, data destination point
 *  @param u4Len, data destination buffer size
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTMSDC_Write(unsigned long arg)
{
#if 1
     MTAL_IOCTL_4ARG_T rArg;
     UINT32 u4Offset,partId;
     UINT8* pu1Buf=NULL;
     UINT32 u4Len;
     INT32 fret;
     MT_RESULT_T mtRet;
    UINT8 *pu1SrcBuf = NULL;
     
     USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
     COPY_FROM_USER_4ARG(arg, rArg);

    partId=(UINT32)rArg.ai4Arg[0];
    u4Offset=( UINT32 )rArg.ai4Arg[1];
    pu1Buf=(UINT8*)rArg.ai4Arg[2];
    u4Len=(UINT32)rArg.ai4Arg[3];
    MTPERIPHERAL_PRINT(" - u4Offset = 0x%x, u4Len = 0x%x\n",
		u4Offset,
             u4Len);

  //add driver implementation code here
    pu1SrcBuf = kmalloc(u4Len, GFP_KERNEL);
    fret = copy_from_user(pu1SrcBuf,pu1Buf,u4Len);
    fret = Loader_WriteMsdc(partId-1, u4Offset,  pu1SrcBuf,  u4Len);
    mtRet=(fret==0)?MTR_OK:MTR_NOT_OK;
    
    kfree(pu1SrcBuf);
    return MTR_OK;
#else
    return MTR_OK;
#endif        
}

#ifdef CC_PARTITION_WP_SUPPORT
static MT_RESULT_T _MTMSDC_WPConfig(unsigned long arg)
{
#if 1
    UINT32 wpConfig;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, UINT32);
    COPY_TO_USER_ARG(arg, wpConfig, UINT32);     
    MTPERIPHERAL_PRINT(" - wpConfig = 0x%x\n",wpConfig);
    MSDCPART_WpConfig(wpConfig);
    
    return MTR_OK;
#else
    return MTR_OK;
#endif        
}
#endif

#endif
//----------------------------------------------------------------------------
/** _MTIRRC_IsPowerButtonOff()
 *	@param	u4Reason
 *	@retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
MT_RESULT_T  _MTIRRC_IsPowerButtonOff (unsigned long arg)
{
	UINT32 u4Reason;

	u4Reason = IRRC_IsPowerButtonOff();

	USR_SPACE_ACCESS_VALIDATE_ARG(arg, UINT32);
	COPY_TO_USER_ARG(arg, u4Reason, UINT32);

	return MTR_OK;
}


EXTERN INT32 IRRC_Init(UINT32 u4PriType);
//IRRX
//-------------------------------------------------------------------------
/** MTIR_Init
 *  Initialize IRRX module.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTIR_Init (MTIR_TYPE_T eType)
{
    UINT32 u4PriType;
#if 1
    if(eType == MTIR_TYPE_DEFAULT)
    {
        u4PriType = DRVCUST_OptGet(eIrrxType);
        printk("MTAL: Init IR Type: 0x%x\n", u4PriType);
    }
    else
#endif
    if(eType == MTIR_TYPE_NEC)
    {
        u4PriType = IRRX_NEC_TYPE;
    }
    else if(eType == MTIR_TYPE_RC56)
    {
        u4PriType = IRRX_RC5_TYPE;
    }
    else if(eType == MTIR_TYPE_SHARP)
    {
        u4PriType = IRRX_SHARP_TYPE;
    }
    else
    {
        return MTR_NOT_SUPPORTED;
    }
    //if(IRRX_InitMtkIr(u4PriType) != IR_SUCC)
    if(IRRC_Init(u4PriType) != IR_SUCC)
    {
        return MTR_NOT_OK;
    }
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** IR Polling function
 *  @param pu4Key,
 *  @param pu4Raw,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTIR_Poll(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32  *usr_pu4Key;
    UINT32  u4Key;
    //IRRX_RAW_DATA_T *usr_prRaw;
    MTIR_IRRX_RAW_DATA_T    *usr_prRaw;
    IRRX_RAW_DATA_T rRaw;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    usr_pu4Key = (UINT32*)rArg.ai4Arg[0];
    usr_prRaw = (MTIR_IRRX_RAW_DATA_T*)rArg.ai4Arg[1];
    if (IRRX_PollMtkIr(&u4Key, &rRaw) == IR_FAIL)
    {
        //MTPERIPHERAL_PRINT("\nIRRX_PollMtkIr ===> IR_FAIL\n");
        return MTR_NOT_OK;
    }
    //MTPERIPHERAL_PRINT("\nIRRX_PollMtkIr ===> IR_Succ\n");

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_pu4Key,UINT32);
    COPY_TO_USER_ARG(usr_pu4Key,u4Key,UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_prRaw,MTIR_IRRX_RAW_DATA_T);
    COPY_TO_USER_ARG(usr_prRaw,rRaw,MTIR_IRRX_RAW_DATA_T);
    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTIRRX_RC56_ProgramWakeupKeysRc()
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTIRRX_RC56_ProgramWakeupKeysRc(unsigned long arg)
{
#if defined(CC_MT5890)
	MTAL_IOCTL_4ARG_T rArg;
    UINT32 u1NumKeys,u4Size;
	
    UINT32 *u1SrcBuf_User = NULL;
	UINT32 *u1SysBuf_User = NULL;
	UINT32 *u1CmdBuf_User = NULL;
    UINT32 pu1KernelBuf_Src[128];
	UINT32 pu1KernelBuf_Sys[128];
	UINT32 pu1KernelBuf_Cmd[128];

	USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u1NumKeys = (UINT32)rArg.ai4Arg[0];
    u1SrcBuf_User = (UINT32*)rArg.ai4Arg[1];
    u1SysBuf_User = (UINT32*)rArg.ai4Arg[2];
	u1CmdBuf_User = (UINT32*)rArg.ai4Arg[3];

	if((u1NumKeys == 0) && (u1NumKeys > 128))
		return MTR_NOT_OK;

	u4Size = u1NumKeys * sizeof(UINT32);
	if (copy_from_user((void *) pu1KernelBuf_Src, (const void __user *) u1SrcBuf_User, u4Size))
    {
		return MTR_NOT_OK;
    }
	if (copy_from_user((void *) pu1KernelBuf_Sys, (const void __user *) u1SysBuf_User, u4Size))
    {
		return MTR_NOT_OK;
    }
	if (copy_from_user((void *) pu1KernelBuf_Cmd, (const void __user *) u1CmdBuf_User, u4Size))
    {
		return MTR_NOT_OK;
    }
	
	IRRX_RC56_ProgramWakeupKeysRc(u1NumKeys,pu1KernelBuf_Src,pu1KernelBuf_Sys,pu1KernelBuf_Cmd);
	return MTR_OK;
#else	
	return MTR_OK;
#endif

}


//-----------------------------------------------------------------------------
/** IR Polling function
 *  @param pu4Key,
 *  @param pu4Raw,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTIR_GetWakeupIrKey(unsigned long arg)
{
	IRRC_WAKKEY_T *pu4Val;
    IRRC_WAKKEY_T pWakeupKey;
    //UINT32 rArg;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    //COPY_FROM_USER_1ARG(arg,rArg);

	pu4Val=(IRRC_WAKKEY_T *)arg;
  	//add driver implementation code here

	IRRX_QueryWakeupKey(&pWakeupKey);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,IRRC_WAKKEY_T);
    COPY_TO_USER_ARG(pu4Val, pWakeupKey, IRRC_WAKKEY_T);

    MTPERIPHERAL_PRINT("Wakeup HotKey = 0x%x , Flag = %d .\n",pWakeupKey.u4WakeupKey,pWakeupKey.fgIrkey);
    return MTR_OK;
}

static MT_RESULT_T _MTIR_GetHotkeyNum(unsigned long arg)
{
	UINT8 *pu1Val;
	UINT8 u1HkNum;


    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    //COPY_FROM_USER_1ARG(arg,rArg);

	pu1Val=(UINT8 *)arg;
  	//add driver implementation code here

	IRRX_GetHotkeyNum(&u1HkNum);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Val,UINT8);
    COPY_TO_USER_ARG(pu1Val, u1HkNum, UINT8);

    MTPERIPHERAL_PRINT("Wakeup HotKey Num = %d .\n",u1HkNum);
    return MTR_OK;
}

static MT_RESULT_T _MTIR_GetAllHotKey(unsigned long arg)
{
	UINT32 *usr_pau4HotKey,*pau4HotKey;
    UINT8  u1HkNum;
	MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

	usr_pau4HotKey = (UINT32*)rArg.ai4Arg[0];
    u1HkNum = (UINT8)rArg.ai4Arg[1];

	pau4HotKey = kmalloc(u1HkNum, GFP_KERNEL);
  	//add driver implementation code here

	IRRX_GetAllHotKey(pau4HotKey,u1HkNum);

	if(copy_to_user((void __user *)usr_pau4HotKey, (const void *)pau4HotKey, u1HkNum*sizeof(UINT32)))
	{
		kfree(pau4HotKey);
		return MTR_NOT_OK;
	}
    if (pau4HotKey)
    {
        kfree(pau4HotKey);
    }
    return MTR_OK;
}

static MT_RESULT_T _MTIR_SendMtkIr(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTPDWNC_RESUME_TYPE type;
    UINT32 u4Key;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    type = (MTPDWNC_RESUME_TYPE)rArg.ai4Arg[0];
    u4Key = (UINT32)rArg.ai4Arg[1];

    Printf("(yjdbg) _MTIR_SendMtkIr: 0x%x, 0x%x\n", type, u4Key);

    if (type == MTPDWNC_RESUME_VIA_IR)
    {
        IRRX_SendMtkIr(u4Key);
    }
    else if (type == MTPDWNC_RESUME_VIA_DEFAULT_IR)
    {
        IRRC_WAKKEY_T pWakeupKey;

        IRRX_QueryWakeupKey(&pWakeupKey);
        IRRX_SendMtkIr(pWakeupKey.u4WakeupKey);
        Printf("(yjdbg) IRRX_QueryWakeupKey: 0x%x\n", pWakeupKey.u4WakeupKey);
    }
    else if (type == MTPDWNC_RESUME_VIA_OTHER)
    {
#if defined(CC_MT5890)
        IRRX_SendMtkIr(DRVCUST_InitGet(ePdwncResumeHandleKey));
#endif
    }

    return MTR_OK;
}

#ifdef CONFIG_NATIVE_CB2
EXTERN void wakeup_pdwnc_queue(void);
static MT_RESULT_T _MTPDWNC_RELEASE_QUEUE(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTPDWNC_RESUME_TYPE type;
    UINT32 u4Key;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    type = (MTPDWNC_RESUME_TYPE)rArg.ai4Arg[0];
    u4Key = (UINT32)rArg.ai4Arg[1];

    Printf("(yjdbg) _MTPDWNC_RELEASE_QUEUE: 0x%x, 0x%x", type, u4Key);

    wakeup_pdwnc_queue();
    return MTR_OK;
}
#endif

#ifdef CC_PDWNC_REBOOT_NOTIFY
EXTERN void unlock_standby(void);
static MT_RESULT_T _MTPDWNC_LETGO_STANDBY(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4Arg1;
    UINT32 u4Arg2;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4Arg1 = (UINT32)rArg.ai4Arg[0];
    u4Arg2 = (UINT32)rArg.ai4Arg[1];

    Printf("(yjdbg) _MTPDWNC_LETGO_STANDBY: 0x%x, 0x%x", u4Arg1, u4Arg2);

    unlock_standby();
    return MTR_OK;
}
#endif

//-----------------------------------------------------------------------------
/** IR convert raw bits to event code function
  *  @param u4RawBits, Input, Ir raw bits
  *  @param pu4EvtCode, Output, event code
  *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTIR_RawBitToEvtCode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4RawBits;
    UINT32 *pu4EvtCode;
    UINT32 u4EvtCode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4RawBits = (UINT32)rArg.ai4Arg[0];
    pu4EvtCode = (UINT32*)rArg.ai4Arg[1];

    u4EvtCode = IRRX_RawBitsToCrystal(u4RawBits);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4EvtCode,UINT32);
    COPY_TO_USER_ARG(pu4EvtCode,u4EvtCode,UINT32);

    return MTR_OK;
}

EXTERN INT32 IRRC_WaitMtkIr(UINT32* prBtnState, UINT32* prKey, IRRX_RAW_DATA_T* rRawData);
//-----------------------------------------------------------------------------
/** Waiting IR Event function
 *  @param pu4Key,
 *  @param pu4Raw,
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTIR_Wait(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4BtnState;
    UINT32 u4Key;
    IRRX_RAW_DATA_T rRaw;

    UINT32* usr_pu4Key;
    UINT32* usr_pu4BtnState;
    MTIR_IRRX_RAW_DATA_T    *usr_prRaw;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    usr_pu4Key = (UINT32*)rArg.ai4Arg[0];
    usr_pu4BtnState = (UINT32*)rArg.ai4Arg[1];
    usr_prRaw = (MTIR_IRRX_RAW_DATA_T*)rArg.ai4Arg[2];
//static INT32 IRRX_WaitMtkIr(UINT32* prBtnState, UINT32* prKey, RAW_DATA_T* rRawData)
    //MTPERIPHERAL_PRINT("IRRC_WaitMtkIr...\n");
    if (IRRC_WaitMtkIr(&u4BtnState, &u4Key, &rRaw) == IR_FAIL)
    {
        //MTPERIPHERAL_PRINT("\nIRRX_PollMtkIr ===> IR_FAIL\n");
        return MTR_NOT_OK;
    }
   // MTPERIPHERAL_PRINT("IRRC_WaitMtkIr BtnState(0x%8x), Key (0x%8x)\n", u4BtnState, u4Key);

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_pu4Key,UINT32);
    COPY_TO_USER_ARG(usr_pu4Key,u4Key,UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_pu4BtnState,UINT32);
    COPY_TO_USER_ARG(usr_pu4BtnState,u4BtnState,UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(usr_prRaw,MTIR_IRRX_RAW_DATA_T);
    COPY_TO_USER_ARG(usr_prRaw,rRaw,MTIR_IRRX_RAW_DATA_T);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTIR_SetFilterSystemCode
 (BOOL fgEnable, BOOL fgPositiveFilter, UINT32 u4Num, UINT32 au4FilterArray[])
 *  @param fgEnable, en/dis RC56 system code fileter
 *  @param fgPositiveFilter,  only support positive filter now
 *  @param u4Num, the numbers of system code in au4FilterArray
 *  @param au4FilterArray,  system code array
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTIR_SetFilterSystemCode(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    UINT32 u4Enable;
    UINT32 u4fgPositiveFilter;
    UINT32 u4CodeCnt;
    UINT32 *pu4Data_User = NULL;
    UINT32 *pu4KernelBuf = NULL;
    INT32 i4Ret;
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    u4Enable = (UINT32)rArg.ai4Arg[0];
    u4fgPositiveFilter = (UINT32)rArg.ai4Arg[1];
    u4CodeCnt = (UINT32)rArg.ai4Arg[2];
    pu4Data_User = (UINT32 *)rArg.ai4Arg[3];

    MTPERIPHERAL_PRINT(
		"u4Enable = 0x%x\n"
		"u4fgPositiveFilter = 0x%x\n"
		"u4CodeCnt = 0x%x\n",
		u4Enable,
		u4fgPositiveFilter,
		u4CodeCnt);

    //parameter check
    if ((pu4Data_User == NULL) || (u4CodeCnt == 0))
    {
        return MTR_PARAMETER_ERROR;
    }
    //add driver implementation code here
    pu4KernelBuf = kmalloc(u4CodeCnt * sizeof(UINT32), GFP_KERNEL);
    if (!pu4KernelBuf)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (copy_from_user((void *) pu4KernelBuf, (const void __user *) pu4Data_User, u4CodeCnt * sizeof(UINT32)))
    {
        kfree(pu4KernelBuf);
		return MTR_NOT_OK;
    }

    //u2Ret = SIF_X_Write(mtDrv_SifBusId, (27000/u4ClkVal), u1DevAddr, u2AddrCnt, u4WordAddr, pu1KernelBuf, u2DataCnt);
    i4Ret = IRRX_FilterSystemCode(u4Enable, u4fgPositiveFilter, u4CodeCnt, pu4KernelBuf);

    if (pu4KernelBuf)
    {
        kfree(pu4KernelBuf);
    }

    if (i4Ret == IR_SUCC)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

static MT_RESULT_T _MTIR_QuietHotBootClr(unsigned long arg)
{
#ifdef CC_QUIET_HOT_BOOT_SUPPORT
    IRRX_IsQuietHotBootClr();
#endif
    return MTR_OK;
}
static MT_RESULT_T _MTIR_IsQuietHotBootPowerOn(unsigned long arg)
{
#ifdef CC_QUIET_HOT_BOOT_SUPPORT
    UINT32 *pu4Val;
    UINT32 u4Result; 
    pu4Val=(UINT32*)arg;

    u4Result = IRRX_IsQuietHotBootPowerOn() ? 1 : 0;
 
    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,UINT32);
    COPY_TO_USER_ARG(pu4Val, u4Result, UINT32);
#endif
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Pinmux Set Function
 *  @param i4PinSel is the bit group of pinmuxers
 *  @param i4Func is the function we want to select
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPINMUX_Set(unsigned long arg)
{
    INT32 i4PinSel;         INT32 i4Func;
    MTAL_IOCTL_2ARG_T rArg;

     USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
     COPY_FROM_USER_2ARG(arg, rArg);

    i4PinSel = rArg.ai4Arg[0];      i4Func= rArg.ai4Arg[1];

    MTPERIPHERAL_PRINT("In kernelL i4PinSel= %d, i4Func =%d\n", i4PinSel, i4Func);
    if( BSP_PinSet(i4PinSel, i4Func ) )
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Pinmux Set Function
 *  @param i4GPIO is GPIO number
 *  @param i4Func is the function we want to select
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 *
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTPINMUX_SetEx(unsigned long arg)
{
    UINT32 u4PinSel;
    INT32 i4Func;
    INT32 i4GPIO;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    i4GPIO = rArg.ai4Arg[0];
    i4Func= rArg.ai4Arg[1];

    BSP_PinGpioGet(i4GPIO, &u4PinSel);
    MTPERIPHERAL_PRINT("In kernel Level GPIO= %d, Function =%u\n", i4GPIO, u4PinSel);
    
    if( BSP_PinSet(u4PinSel, i4Func) )
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** Check if it is secure boot IC or not
 *  @param pu4ScrBt, 1: Secure boot IC; 0: Non-Secure boot IC
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSCRBT_Check(unsigned long arg)
{
    UINT32 *pu4ScrBt;
    UINT32  u4ScrBt;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);

    pu4ScrBt =(UINT32 *)arg;

    #if defined(CC_MT5365) || defined(CC_MT5395)
    if (BIM_IsSecureBoot())
    {
    	u4ScrBt = 1;
    }
    else
    #endif
    {
    	u4ScrBt = 0;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ScrBt, UINT32);
    COPY_TO_USER_ARG(pu4ScrBt, u4ScrBt, UINT32);

    MTPERIPHERAL_PRINT(" - SecureBoot = 0x%x \n", u4ScrBt);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTDRAM_GetBandwidthUsage() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTDRAM_GetBandwidthUsage (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4MonitorSecond;
    UINT32 *pu4Usage;
    UINT32  u4Usage;
    MT_RESULT_T ret = MTR_OK;
    UINT32 u4DelayClock;
    UINT64 u8Usage;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4MonitorSecond = (UINT32)rArg.ai4Arg[0];
    pu4Usage = (UINT32*)rArg.ai4Arg[1];

    // driver implementation start -----

    if (u4MonitorSecond == 0)
    {
        u4MonitorSecond = 1;
    }
#if defined(CC_MT5396)||defined(CC_MT5368)||defined(CC_MT5389)||defined(CC_MT5398)||defined(CC_MT5880)||defined(CC_MT5881)||defined(CC_MT5399)||defined(CC_MT5890)||defined(CC_MT5882)
	u4DelayClock = BSP_GetDomainClock(SRC_MEM_CLK) * u4MonitorSecond;
#else
	u4DelayClock = (BSP_GetDomainClock(CAL_SRC_DMPLL) >> 1) * u4MonitorSecond;
#endif

#if defined(CC_MT5395) || defined(CC_MT5396)||defined(CC_MT5368)||defined(CC_MT5389)||defined(CC_MT5398)||defined(CC_MT5880)||defined(CC_MT5881)||defined(CC_MT5399)||defined(CC_MT5890)||defined(CC_MT5882)
    u4Usage = SRM_DramMonitor(0,SRM_DRV_TOTAL, u4DelayClock, u4MonitorSecond * 1000);
#else
    u4Usage = SRM_DramMonitor(SRM_DRV_TOTAL, u4DelayClock, u4MonitorSecond * 1000);
#endif
    u8Usage = (UINT64)u4Usage;
    u8Usage *= 1000;
    u8Usage = u8Div6432(u8Usage, (UINT64)u4DelayClock, NULL);
    u4Usage = (UINT32)u8Usage;
    // driver implementation end -----

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Usage, UINT32);
    COPY_TO_USER_ARG(pu4Usage, u4Usage, UINT32);

    MTPERIPHERAL_PRINT(" - u4MonitorSecond = %d, pu4Usage = %d\n",
        u4MonitorSecond,
        *pu4Usage);

    return ret;
}

#ifdef CC_SUPPORT_SMC
//-----------------------------------------------------------------------------
/** _MTSMC_Init() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_Init (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;

    if (SMC_Init() != 0)
    {
        rRet = MTR_NOT_OK;
    }
    
    SMC_SetHotPlugCb(_SmcCallbackHandler, SMC_SLOT_NO1);	
    SMC_SetHotPlugCb(_SmcCallbackHandlerEx, SMC_SLOT_NO2);

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_Init() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_GetCardPresence (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;
    
    UINT8 u1SlotNo;
    UINT32 u4Status;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    u1SlotNo = (UINT8)rArg.ai4Arg[0];
    
    u4Status = (UINT32)SMC_GetCardPresense(u1SlotNo);

    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1], UINT32);
    COPY_TO_USER_ARG(rArg.ai4Arg[1], u4Status, UINT32);

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_Reset() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_Reset (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    UINT8 u1SlotNo = (UINT8)arg;
    
    if (SMC_Reset(u1SlotNo) != 0)
    {
        rRet = MTR_NOT_OK;
    }

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_SendReceiveAPDU() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_SendReceiveAPDU (unsigned long arg)
{
    MTAL_IOCTL_5ARG_T rArg;
    MT_RESULT_T rRet = MTR_OK;
    UINT32 u4SendLen = 0, u4RecvLen = 0;
    UINT8 u1SlotNo;
    static UINT8 au1SendBuf[254];
    static UINT8 au1RecvBuf[254];    

    USR_SPACE_ACCESS_VALIDATE_5ARG(arg);
    COPY_FROM_USER_5ARG(arg, rArg);


    u1SlotNo = (UINT8)rArg.ai4Arg[0];
    u4SendLen = (UINT8)rArg.ai4Arg[2];
    // Copy send APDU data from user space
    COPY_FROM_USER_ARG_SIZE(rArg.ai4Arg[1], au1SendBuf, u4SendLen);

    if(SMC_SendReceiveAPDU(u1SlotNo, 
        au1SendBuf, u4SendLen, au1RecvBuf, (UINT8*)&u4RecvLen) != 0)
    {
        rRet = MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[4], UINT32);
    COPY_TO_USER_ARG(rArg.ai4Arg[4], u4RecvLen, UINT32);
 	
    USR_SPACE_ACCESS_VALIDATE_ARG_SIZE(rArg.ai4Arg[3], u4RecvLen);
    COPY_TO_USER_ARG_SIZE(rArg.ai4Arg[3], au1RecvBuf, u4RecvLen);

    MTPERIPHERAL_PRINT(" ");

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_SendReceiveAPDU() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_RequestIFS (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    MTAL_IOCTL_2ARG_T rArg;

    UINT8 u1SlotNo, u1IFS;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u1SlotNo = (UINT8)rArg.ai4Arg[0];
    u1IFS = (UINT8)rArg.ai4Arg[1];

    printf("u1SlotNo = %d, u1IFS = 0x%2X\n", u1SlotNo, u1IFS);
    
    if (SMC_RequestIFS(u1IFS, u1SlotNo) != 0)
    {
        rRet = MTR_NOT_OK;
    }    	

    MTPERIPHERAL_PRINT(" ");

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_SendReceiveAPDU() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_Activate (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    UINT8 u1SlotNo = (UINT8)arg;
    
    if (SMC_Activate(u1SlotNo) != 0)
    {
        rRet = MTR_NOT_OK;
    }    	

    MTPERIPHERAL_PRINT(" ");

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_SendReceiveAPDU() provide the total dram bandwidth usage
 *  @param u4MonitorSecond the monitor perioid in second.
 *  @param pu4Usage the total consumption in permillage
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSMC_Deactivate (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    UINT8 u1SlotNo = (UINT8)arg;
        
    if (SMC_Deactivate(u1SlotNo) != 0)
    {
        rRet = MTR_NOT_OK;
    }    	

    MTPERIPHERAL_PRINT(" ");

    return rRet;
}

//-----------------------------------------------------------------------------
/** _MTSMC_SetSmcProtocol() provide the smc protocol selection
 *  @param eSmcProtocal
 *  @return MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTSMC_SetSmcProtocol (unsigned long arg)
{
    MT_RESULT_T rRet = MTR_OK;
    mtal_smc_protocol_t eSmcProtocal;

    eSmcProtocal = (mtal_smc_protocol_t)arg;

    //rRet = SMC_SetSmcProtocol(eSmcProtocal);

    return rRet;
}

#endif

//EEPROM
//-------------------------------------------------------------------------
/** MTEEPROM_Init
 *  Initialize SIF module.
 *  @param  VOID
 *  @return  VOID
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPROM_Init (VOID)
{
    MTPERIPHERAL_PRINT(" - void \n");

     //add driver implementation code here
    EEPROM_Init();

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** MTEEPROM_Read
 *  read data with word address from SIF module.
 *  @param  ePort   	SIF Port ID
 *  @param  u4ClkVal 	Demaned Sif Clock in Khz
 *  @param  u1DevAddr 	Serial interface device address.
 *  @param  pu1Addr 	Pointer to Serial interface word address.
 *                      *pu1Addr is sent first on bus.
 *  @param  u2AddrCnt 	Serial interface word address count (up to 3).
 *  @param  pu1Data 		Pointer to data
 *  @param  u2DataCnt 	Number of byte to read. Max = 256 bytes.
 *  @retval   MT_RESULT_T
 *            MTR_OK for OK
 *            MTR_PARAMETER_ERROR for wrong parameter,
 *            MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPROM_Read (unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4Offset;
    UINT8 *pu1Data_User = NULL;
    UINT32 u4DataCnt;
    UINT8 * pu1KernelBuf = NULL;
    INT32 i4Ret;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset = (UINT32)rArg.ai4Arg[0];
    pu1Data_User = (UINT8*)rArg.ai4Arg[1];
    u4DataCnt = (UINT32)rArg.ai4Arg[2];


    //parameter check
    if((pu1Data_User==NULL) || (u4DataCnt==0))
    {
        return MTR_PARAMETER_ERROR;
    }

    //add driver implementation code here
    pu1KernelBuf = kmalloc(u4DataCnt, GFP_KERNEL);
    if (!pu1KernelBuf)
    {
        return MTR_PARAMETER_ERROR;
    }

    i4Ret = EEPROM_Read((UINT64)u4Offset, (UINT32)pu1KernelBuf, u4DataCnt);

    if(copy_to_user((void __user *)pu1Data_User, (const void *)pu1KernelBuf, u4DataCnt))
	{
		kfree(pu1KernelBuf);
		return MTR_NOT_OK;
	}

    if (pu1KernelBuf)
    {
        kfree(pu1KernelBuf);
    }

    if (i4Ret == 0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-------------------------------------------------------------------------
/** MTEEPROM_Write
 *  write data with word address to SIF module.
 *  @param  ePort  	SIF Port ID
 *  @param  u4ClkVal 	Demaned Sif Clock in Khz
 *  @param  u1DevAddr 	Serial interface device address.
 *  @param  pu1Addr 	Pointer to Serial interface word address.
 *                      *pu1Addr is sent first on bus.
 *  @param  u2AddrCnt 	Serial interface word address count (up to 3).
 *  @param  pu1Data 	Pointer to data
 *  @param  u2DataCnt 	Number of byte to read. Max = 256 bytes.
 *  @retval   MT_RESULT_T
 *            MTR_OK for OK
 *            MTR_PARAMETER_ERROR for wrong parameter,
 *            MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPROM_Write (unsigned long arg )
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT32 u4Offset;
    UINT8 *pu1Data_User = NULL;
    UINT32 u4DataCnt;
    UINT8 * pu1KernelBuf = NULL;
    INT32 i4Ret;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u4Offset = (UINT32)rArg.ai4Arg[0];
    pu1Data_User = (UINT8*)rArg.ai4Arg[1];
    u4DataCnt = (UINT32)rArg.ai4Arg[2];

    //parameter check
    if ((pu1Data_User == NULL) || (u4DataCnt == 0))
    {
        return MTR_PARAMETER_ERROR;
    }

    //add driver implementation code here
    pu1KernelBuf = kmalloc(u4DataCnt, GFP_KERNEL);
    if (!pu1KernelBuf)
    {
        return MTR_PARAMETER_ERROR;
    }

    if (copy_from_user((void *) pu1KernelBuf, (const void __user *) pu1Data_User, u4DataCnt))
    {
        kfree(pu1KernelBuf);
		return MTR_NOT_OK;
    }

    i4Ret = EEPROM_Write((UINT64)u4Offset, (UINT32)pu1KernelBuf, u4DataCnt);

    if (pu1KernelBuf)
    {
        kfree(pu1KernelBuf);
    }

    if (i4Ret == 0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}

//-----------------------------------------------------------------------------
/** get the HDCP offset address at eeprom
 *  @param pu4ScrOffset HDCP address
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPROM_GetHdcp2xOffset (unsigned long arg )
{
#if defined(CC_MT5880) || defined(CC_MT5398) || defined(CC_MT5860) || defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
	UINT32 *pu4HDCPOffset;
    UINT32  u4HDCPOffset;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);

    pu4HDCPOffset =(UINT32 *)arg;
    u4HDCPOffset = DRVCUST_InitGet(eEepromHDCP2xOffset);
	USR_SPACE_ACCESS_VALIDATE_ARG(pu4HDCPOffset, UINT32);
    COPY_TO_USER_ARG(pu4HDCPOffset, u4HDCPOffset, UINT32);

    MTPERIPHERAL_PRINT(" - u4HDCP Offset = 0x%x \n", u4HDCPOffset);
#endif
    return MTR_OK;
}
//-------------------------------------------------------------------------
/** MTEEPROM_GetProperty
 *  read data with word address from SIF module.
 *  @param  ePort   	SIF Port ID
 *  @param  u4ClkVal 	Demaned Sif Clock in Khz
 *  @param  u1DevAddr 	Serial interface device address.
 *  @param  pu1Addr 	Pointer to Serial interface word address.
 *                      *pu1Addr is sent first on bus.
 *  @param  u2AddrCnt 	Serial interface word address count (up to 3).
 *  @param  pu1Data 		Pointer to data
 *  @param  u2DataCnt 	Number of byte to read. Max = 256 bytes.
 *  @retval   MT_RESULT_T
 *            MTR_OK for OK
 *            MTR_PARAMETER_ERROR for wrong parameter,
 *            MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPROM_GetProperty (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4Size;
    UINT32 u4MwOffset;
    UINT8 *pu1Size_User = NULL;
    UINT8 *pu1MwOffset_User = NULL;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Size_User = (UINT8*)rArg.ai4Arg[0];
    pu1MwOffset_User = (UINT8*)rArg.ai4Arg[1];

    //parameter check
    if((pu1Size_User==NULL) || (pu1MwOffset_User==0))
    {
        return MTR_PARAMETER_ERROR;
    }

    u4Size = (INT32)EEPROM_GetSize();
    u4MwOffset = DRVCUST_InitGet(eEepromMwOffset);

    if(copy_to_user((void __user *)pu1Size_User, (const void *)&u4Size, sizeof(int)))
	{
		return MTR_NOT_OK;
	}
    if(copy_to_user((void __user *)pu1MwOffset_User, (const void *)&u4MwOffset, sizeof(int)))
	{
		return MTR_NOT_OK;
	}
    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTEEPDTV_GetCfg
 *  @param  rDtvCfg 	store EEPDTV cfg data
 *  @retval   MT_RESULT_T   MTR_OK for OK, MTR_PARAMETER_ERROR for wrong parameter,
 *                MTR_NOT_OK for execution error
 */
//-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPDTV_GetCfg (unsigned long arg)
{
	DTVCFG_T rDtvCfg;
    INT32 i4Ret;

    //add driver implementation code here
    i4Ret = EEPDTV_GetCfg(&rDtvCfg);

	USR_SPACE_ACCESS_VALIDATE_ARG(arg, DTVCFG_T);
	COPY_TO_USER_ARG(arg, rDtvCfg, DTVCFG_T);

    if (i4Ret == 0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


//-------------------------------------------------------------------------
/** _MTEEPDTV_SetCfg
  *  @param  rDtvCfg	 store EEPDTV cfg data
  *  @retval   MT_RESULT_T	 MTR_OK for OK, MTR_PARAMETER_ERROR for wrong parameter,
  * 			   MTR_NOT_OK for execution error
  */
 //-------------------------------------------------------------------------
static MT_RESULT_T  _MTEEPDTV_SetCfg (unsigned long arg)
{
    DTVCFG_T *prDtvCfg;
	DTVCFG_T rDtvCfg;
    INT32 i4Ret;

    prDtvCfg = (DTVCFG_T*)arg;
    USR_SPACE_ACCESS_VALIDATE_ARG(prDtvCfg,DTVCFG_T);
    COPY_FROM_USER_ARG(prDtvCfg,rDtvCfg,DTVCFG_T);

    //add driver implementation code here
    i4Ret = EEPDTV_SetCfg(&rDtvCfg);

    if (i4Ret == 0)
    {
        return MTR_OK;
    }
    else
    {
        return MTR_NOT_OK;
    }
}


//-------------------------------------------------------------------------
/** _MTEEPHDCPInit
  */
 //-------------------------------------------------------------------------
static MT_RESULT_T _MTEEPHDCPInit(unsigned long arg)
{
    UNUSED(arg);

    // initail HDCP SRAM.
    if(EEPHDCP_Init() != 0)
    {
        return MTR_NOT_OK;
    }

    return MTR_OK;
}


//-------------------------------------------------------------------------
/** _MTEEPNPTV_Read
  */
 //-------------------------------------------------------------------------
static MT_RESULT_T _MTEEPNPTV_Read(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T    rArg;
    UINT16 u2Addr;
    UINT8 u1Data, *pu1Data;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u2Addr = (UINT16)rArg.ai4Arg[0];
    pu1Data = (UINT8*)rArg.ai4Arg[1];

    u1Data = bApiEepromReadByte(u2Addr);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Data, UINT8);
    COPY_TO_USER_ARG(pu1Data, u1Data, UINT8);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTBIM_GetDeviceID()
  *  @param pu1DeviceID the ID to get.
  *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
  */
 //-----------------------------------------------------------------------------
EXTERN INT32 BIM_GetSecureID(BYTE *pu1SecureID, BYTE u1Size);
MT_RESULT_T _MTBIM_GetDeviceID (unsigned long arg)
{
	  return MTR_NOT_OK;
}

//-----------------------------------------------------------------------------
/** _MTPDWNC_GetPowerState()
  *   Driver customization power state function.
  *  @param *pu4PowerState the power state to get.
  *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail. There is no setting.
  */
 //-----------------------------------------------------------------------------
MT_RESULT_T _MTPDWNC_GetPowerState (unsigned long arg)
{
	UINT32 *pu4PowerState;
	UINT32 u4PState;

	PFN_POWER_STATE_QUERY_FUNC pfnQueryPowerStateFunc = NULL;

	pu4PowerState = (UINT32*)arg;

	if(DRVCUST_InitQuery(ePowerStateQueryFunc, (UINT32 *)(void *)&(pfnQueryPowerStateFunc)) < 0)
	{
		return MTR_NOT_OK;
	}
	else
	{
		if (pfnQueryPowerStateFunc != NULL)
		{
			u4PState = pfnQueryPowerStateFunc();
		}
		else
		{
			return MTR_NOT_OK;
		}
	}

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4PowerState,INT32);
    COPY_TO_USER_ARG(pu4PowerState,u4PState,INT32);

	return MTR_OK;
}

//----------------------------------------------------------------------------
/** MTAGENT_SetBandwidthParam()
 *  @param u4Module, module number to control
 *  @param u4Type, register to control
 *  @param u4Value, value to set
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTAGENT_SetBandwidthParam(unsigned long arg)
{
    UINT32 u4Module;
    UINT32 u4Type;
    UINT32 u4Value;
    MTAL_IOCTL_3ARG_T rArg;

     USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
     COPY_FROM_USER_3ARG(arg, rArg);

    u4Module = rArg.ai4Arg[0];
    u4Type= rArg.ai4Arg[1];
    u4Value= rArg.ai4Arg[2];

    MTPERIPHERAL_PRINT("In kernelL u4Module= %d, u4Type =%d, u4Value =%d\n", u4Module, u4Type,u4Value);

    if(u4Module>=MAX_BW_MODULE_NUM)
    {
    	MTPERIPHERAL_PRINT("Set BW parameter module  type error!\n");
    	return MTR_PARAMETER_ERROR;
    }

    if((u4Module==BM_MODULE_OSD1)||(u4Module==BM_MODULE_OSD2))
    {
    	switch(u4Type)
    	{
    		case SBWP_OSD_CONT_REQ_LMT0:
    			OSD_PLA_SetContReqLmt0(u4Module+1,u4Value);
    			break;
    		case SBWP_OSD_PAUSE_CNT:
    			OSD_PLA_SetPauseCnt(u4Module+1,u4Value);
    			break;
    		case SBWP_OSD_VAC_TH:
    			OSD_PLA_SetVacTh(u4Module+1,u4Value);
    			break;
    		case SBWP_OSD_FIFO_SIZE:
    			OSD_PLA_SetFifoSize(u4Module+1,u4Value);
    			break;
    		case SBWP_OSD_CONT_REQ_LMT:
    			OSD_PLA_SetContReqLmt(u4Module+1,u4Value);
    			break;
    		default:
    			MTPERIPHERAL_PRINT("Set BW parameter reg type error!\n");
    			return MTR_PARAMETER_ERROR;

    	}
    }


    return MTR_OK;
}
//-------------------------------------------------------------------------
/** _MTDDI_UART_AccessReg()
 *  @param u4AccessRegflag, the flag of uart protection ,it is also in ioctl_reg_fun.c
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTDDI_UART_AccessReg(unsigned long arg)
{
	//MTAL_IOCTL_1ARG_T rArg;

	int tmp;

	USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
	COPY_FROM_USER_1ARG(arg, u4AccessRegflag);
	//printf("printf u4AccessRegflag is %d in io_mtperipheral 3441",u4AccessRegflag);
	if(u4AccessRegflag)
	{
		tmp =*((volatile unsigned int*)0xf0008098);
		tmp |= (1<<6);
		*((volatile unsigned int*)0xf0008098) = tmp;
	}
	else
	{
		tmp =*((volatile unsigned int*)0xf0008098);
		tmp &= ~(1<<6);
		*((volatile unsigned int*)0xf0008098) = tmp;
	}

	return MTR_OK;
}

//----------------------------------------------------------------------------
/** _MTIR_SetTheNumberOfKey()
 *  Set the Number of key to driver
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------

static MT_RESULT_T _MTIR_SetTheNumberOfKey(unsigned long arg)
{
#ifdef RC_LAST_KEY_SUPPORT
    MTAL_IOCTL_2ARG_T rArg;
	UINT32 u4NumKey;


    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    u4NumKey = (UINT32)rArg.ai4Arg[0];

	#ifdef SET_THE_NUMBER_OF_KEY_FUNC
	SET_THE_NUMBER_OF_KEY_FUNC(u4NumKey);
	#endif
#endif
	return MTR_OK;
}

static MT_RESULT_T _MTIR_SetTheLastOfFiveKeys(unsigned long arg)
{
#ifdef RC_LAST_KEY_SUPPORT
    MTAL_IOCTL_6ARG_T rArg;
	UINT32 u4IrKey[5];
	UINT8 i = 0;

    USR_SPACE_ACCESS_VALIDATE_6ARG(arg);
    COPY_FROM_USER_6ARG(arg, rArg);

   for(i=0;i<5;i++)
   {
     u4IrKey[i]= (UINT32)(rArg.ai4Arg[i]);
   }

   #ifdef SET_THE_LAST_OF_FIVE_KEY_FUNC
   SET_THE_LAST_OF_FIVE_KEY_FUNC(u4IrKey);
   #endif
#endif
   return MTR_OK;
}
//-------------------------------------------------------------------------
/** _MTDDI_PWM_SetVsyncAlign()
 *  @param esrc mean which PWM,fgEnable means pwm align with vsync enalbe or disable
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------

static MT_RESULT_T _MTDDI_PWM_SetVsyncAlign(unsigned long arg)
{
		MTAL_IOCTL_2ARG_T rArg;
		UINT32 eSrc;
		UINT32 fgEnable;
		USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
		COPY_FROM_USER_2ARG(arg, rArg);
		eSrc = (UINT32)rArg.ai4Arg[0];
		fgEnable = (UINT32)rArg.ai4Arg[1];
		vDrvSetLock(eSrc, fgEnable);
		return MTR_OK;
}


//-------------------------------------------------------------------------
/** _MTPDWNC_SetWatchDogTimeout()
 *  @param u4Timeout, the millisecond of watchdog timeout value, 0 to disable watchdog
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_SetWatchDogTimeOut(unsigned long arg)
{
    UINT32 u4Timeout = (UINT32)arg;

    if((PDWNC_EXT_GetFunc())->pfnCustomWrite != NULL)
    {
         (PDWNC_EXT_GetFunc())->pfnCustomWrite(0, &u4Timeout, sizeof(u4Timeout));
    }

    if(u4Timeout > 0)
    {
        PDWNC_CreateWatchDogThread();
    }

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** MTPDWNC_GetWatchDogTimeout()
 *  @param pu4Timeout, the millisecond of watchdog timeout value, 0 means the watchdog is disabled
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_GetWatchDogTimeOut(unsigned long arg)
{
    UINT32 *pu4Timeout;
    UINT32 u4Timeout;

    if((PDWNC_EXT_GetFunc())->pfnCustomRead != NULL)
    {
         (PDWNC_EXT_GetFunc())->pfnCustomRead(0, &u4Timeout, sizeof(u4Timeout));
    }

    pu4Timeout = (UINT32*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Timeout,UINT32);
    COPY_TO_USER_ARG(pu4Timeout,u4Timeout,UINT32);

    return MTR_OK;
}

//-------------------------------------------------------------------------
/** _MTPDWNC_GspiInit()
 *  @param prGspiMode->u4Mode  GSPI config.
 *  @param prGspiMode->u4Freq  SPI clock, (uint:1hz,Min = 2929.6875Hz, Max = 12000000Hz).
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_GspiInit(unsigned long arg)
{
#if defined(CC_MT5890)
	PDWNC_GSPI_MODE_T *prGspiMode_User;
	PDWNC_GSPI_MODE_T prGspiMode;

	prGspiMode_User = (PDWNC_GSPI_MODE_T *)arg;
	
	if(copy_from_user((void *)&prGspiMode, (const void __user *) prGspiMode_User, sizeof(PDWNC_GSPI_MODE_T)))
    {
		return MTR_NOT_OK;
    }
	PDWNC_GspiInit(&prGspiMode);
#endif	
	return MTR_OK;
}
//-------------------------------------------------------------------------
/** _MTPDWNC_GspiReadWrite()
 *  @param fgReadWrite,  GSPI read/write flag.
 *  @param u4Nob, MDATA_LEN * NDATA_LEN is 32bytes,read/write buffer size (bits,1~256).
 *  @param u1RdBuf, read buffer.
 *  @param u1WrBuf, write buffer.
 *  @retval MTR_OK, Succeed; MTR_NOT_OK, Fail
 */
//----------------------------------------------------------------------------
static MT_RESULT_T _MTPDWNC_GspiReadWrite(unsigned long arg)
{
#if defined(CC_MT5890)
	MTAL_IOCTL_4ARG_T rArg;
    UINT32 fgReadWrite;
    UINT32 u4Nob,u4Size;
    UINT8 *u1RdBuf_User = NULL;
	UINT8 *u1WrBuf_User = NULL;
    UINT8 pu1KernelBuf_Rd[32];
	UINT8 pu1KernelBuf_Wr[32];

	USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    fgReadWrite = (UINT32)rArg.ai4Arg[0];
    u4Nob = (UINT32)rArg.ai4Arg[1];
    u1RdBuf_User = (UINT8*)rArg.ai4Arg[2];
	u1WrBuf_User = (UINT8*)rArg.ai4Arg[3];

	u4Size = (((u4Nob&0xFF) + 1) * (((u4Nob>>16)&0xFF) + 1))/0x08;
	//Printf("_MTPDWNC_GspiReadWrite,u4Size = %d , Nob = 0x%08x, fgReadWrite = 0x%x.\n",u4Size,u4Nob,fgReadWrite);
	if(u4Size > 32) 
	{	
		return MTR_NOT_OK;//The maximum valid value MDATA_LEN*NDATA_LEN is 32 bytes.
	}
	if((fgReadWrite & GSPI_READ) && (u1RdBuf_User == NULL))
	{
		Printf("u1RdBuf_User is NULL .\n");
		return MTR_NOT_OK; 
	}
	if((fgReadWrite & GSPI_WRITE) && (u1WrBuf_User == NULL))
	{
		Printf("u1WrBuf_User is NULL .\n");
		return MTR_NOT_OK; 
	}
	
	if((fgReadWrite & GSPI_WRITE) && (u1WrBuf_User != NULL))
	{
		if (copy_from_user((void *) pu1KernelBuf_Wr, (const void __user *) u1WrBuf_User, u4Size))
	    {
			return MTR_NOT_OK;
	    }
	}
	if(!PDWNC_GspiReadWrite(fgReadWrite, u4Nob, pu1KernelBuf_Rd, pu1KernelBuf_Wr))
	{
		if(fgReadWrite & GSPI_READ)
		{
			if(copy_to_user((void __user *)u1RdBuf_User, (const void *)pu1KernelBuf_Rd, u4Size))
			{
				return MTR_NOT_OK;
			}
		}
		return MTR_OK;
	}
	else
	{
		return MTR_NOT_OK;
	}
#else	
	return MTR_OK;
#endif

}

/** enable Set DramBW Eanble
 *  @param  fgEnable                enable=TRUE/disable=FALSE
 *  @retval MTR_OK                  Success
 *  @retval MTR_NOT_OK              Fail
 */
static MT_RESULT_T _MTSYS_SetDramBWEnable(unsigned long arg)
{
    BOOL fgEnable;

    fgEnable = (BOOL)arg;

    MTPERIPHERAL_PRINT(" - fgEnable = %d\n", fgEnable);
	#if 0
	if(fgEnable)
	{ 
	    // Set monitor cycle.

	}
	else
	{
		
	}
	#endif
    return MTR_OK;
}
/** enable Get DramBW Info
 *  @param
 *  @retval MTR_OK                  Success
 *  @retval MTR_NOT_OK              Fail
 */
static MT_RESULT_T _MTSYS_GetDramBWInfo(unsigned long arg)
{
     MTAL_IOCTL_4ARG_T rArg;

    UINT32 *pu4ChA_total;
    UINT32 *pu4ChA_percent;
    UINT32 *pu4ChB_total;
    UINT32 *pu4ChB_percent;

    UINT32  u4ChA_total = 0;
    UINT32  u4ChA_percent;
    UINT32  u4ChB_total = 0;
    UINT32  u4ChB_percent;

    MT_RESULT_T ret = MTR_OK;
    UINT32 u4DelayClock;
    UINT64 u8Usage;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    pu4ChA_total = (UINT32*)rArg.ai4Arg[0];
    pu4ChA_percent = (UINT32*)rArg.ai4Arg[1];
    pu4ChB_total = (UINT32*)rArg.ai4Arg[2];
    pu4ChB_percent = (UINT32*)rArg.ai4Arg[3];

    // driver implementation start -----

#if defined(CC_MT5396)||defined(CC_MT5368)||defined(CC_MT5389)||defined(CC_MT5398)||defined(CC_MT5880)||defined(CC_MT5881)||defined(CC_MT5399)||defined(CC_MT5890)||defined(CC_MT5882)
	u4DelayClock = BSP_GetDomainClock(SRC_MEM_CLK);
#else
	u4DelayClock = (BSP_GetDomainClock(CAL_SRC_DMPLL) >> 1) ;
#endif

#if defined(CC_MT5395) || defined(CC_MT5396)||defined(CC_MT5368)||defined(CC_MT5389)||defined(CC_MT5398)||defined(CC_MT5399)||defined(CC_MT5890)||defined(CC_MT5882)
    u4ChA_total = SRM_DramMonitor(0,SRM_DRV_TOTAL, u4DelayClock, 1000);
    //u4ChB_total = SRM_DramMonitor(1,SRM_DRV_TOTAL, u4DelayClock, 1000);//mt5882 have no chanel b.so mask it  .
#endif

    u8Usage = (UINT64)u4ChA_total;
    u8Usage *= 1000;
    u8Usage = u8Div6432(u8Usage, (UINT64)u4DelayClock, NULL);
    u4ChA_percent= (UINT32)u8Usage;

    u8Usage = (UINT64)u4ChB_total;
    u8Usage *= 1000;
    u8Usage = u8Div6432(u8Usage, (UINT64)u4DelayClock, NULL);
    //u4ChB_percent = (UINT32)u8Usage;
    u4ChB_percent = 0;//mt5882 have no chanel b.so return 0 directly .

    u4ChA_total = 16 * (u4ChA_total/1024/1024);// for chA 32bit
    //u4ChB_total = 8 * (u4ChB_total/1024/1024);// for chB 16bit
    u4ChB_total =0;//mt5882 have no chanel b.so return 0 directly .


    // driver implementation end -----

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ChA_total, UINT32);
    COPY_TO_USER_ARG(pu4ChA_total, u4ChA_total, UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ChA_percent, UINT32);
    COPY_TO_USER_ARG(pu4ChA_percent, u4ChA_percent, UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ChB_total, UINT32);
    COPY_TO_USER_ARG(pu4ChB_total, u4ChB_total, UINT32);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4ChB_percent, UINT32);
    COPY_TO_USER_ARG(pu4ChB_percent, u4ChB_percent, UINT32);

    return ret;
}

 void DumpHex(const unsigned char *data, int len)
 {
	 int pos = 0;
	 
	 if (!data || !len)
	 {
		 return;
	 }
	 //fprintf(stderr, "Dumping data\n");
 
	 //if (hdcp2x_log_cli)
	 {
		 printk("------------------------------");
		 while(pos < len /*len*/) // only print message ID
		 {
			 if (pos%8 == 0)
				 printk("\n[%04x]|", pos);
			 printk("0x%02x, ", data[pos]); // secure data, cannot show in mtktool
			 pos ++;
		 }
		 printk("\n------------------------------\n");
	 }
	 
 }

static MT_RESULT_T _MTSYS_DDR_SSC(unsigned long arg)
{
 #if 1
	 UINT32 u4Val;
	 U16 u2frequency1;
	 U32 u4ssc_prd, u4ssc_delta,u4mempll_prediv_hex;
	 UINT32 u4Permillage, u4Frequency;
	 MTAL_IOCTL_2ARG_T rArg;   
     U32 u4mempll_predivider[3] = {1, 2, 4}; //caution: u4mempll_predivider = 2^u4mempll_prediv_hex;
 
	 USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	 COPY_FROM_USER_2ARG(arg, rArg);
	 u4Frequency = (UINT32)rArg.ai4Arg[0];
	 u4Permillage = (UINT32)rArg.ai4Arg[1];
	 
	 if (u4Permillage == 0)
	 {
		 printk("DRAM: Disable SSC.\n");
		 /* Disable Spectrum */
		 //RG_MEMPLL_SSC_EN (12ACH[29]) from 0 to 1 (DDS SSC enable). 20us after RG_MEMPLL_BIAS_RST from FNPLL AN (It is enough here)
		 u4Val = ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2ac));
		 mcCLR_BIT(u4Val, 29);
		 ucDram_Register_Write(mcSET_PHY_REG_ADDR(0x2ac), u4Val);
		 return 0;		  
	 }
 
	 printk("DRAM: SSC u4Permillage=%d, mod_freq=%d.\n", u4Permillage, u4Frequency);
	 
	 if (u4Frequency == 0)
	 {		 
		 printk("Modulation freq Khz, m must >=1\n");
		 return MTR_NOT_OK;
	 }
 
	 /* Disable Spectrum */
	 //RG_MEMPLL_SSC_EN (12ACH[29]) from 0 to 1 (DDS SSC enable). 20us after RG_MEMPLL_BIAS_RST from FNPLL AN (It is enough here)
	 u4Val = ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2ac));
	 mcCLR_BIT(u4Val, 29);
	 ucDram_Register_Write(mcSET_PHY_REG_ADDR(0x2ac), u4Val);
	 x_thread_delay(200);
 
	 u2frequency1 = ((TCMGET_DDR_CLK()/BASE_DDR_CLK)/2)/2;
	 u4Val = ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2a0));
	 u4mempll_prediv_hex = u4Val&0x03;
 
	 //ASSERT(u4mempll_prediv_hex <=2);
	 u4ssc_prd = (((XTAL_MHZ/u4mempll_predivider[u4mempll_prediv_hex])*1000)/u4Frequency)/2;
	 u4ssc_delta = ((u2frequency1*262144)/XTAL_MHZ)*u4Permillage*2/(10000*u4ssc_prd);
	 printk("u4ssc_prd =%08x, u4ssc_delta =%08x\n", u4ssc_prd, u4ssc_delta);
	 
	 if(u4ssc_delta>0xFF)
	 {
		 printk("u4ssc_delta is overflow, please change the ssc setting. \n");
	 }
	 else
	 {
		 //set ssc for down spread
		 u4Val = ((u4ssc_delta)<<16)|(u4ssc_delta<<24);
		 u4Val = (ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2b0))&0x0000FFFF) | u4Val;
		 ucDram_Register_Write(mcSET_PHY_REG_ADDR(0x2b0), u4Val);
 
		 u4Val = (ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2ac))&0xFFFF0000)|(u4ssc_prd&0xFFFF);
		 ucDram_Register_Write(mcSET_PHY_REG_ADDR(0x2ac), u4Val);
	 }
	 //enable ssc
	 //RG_MEMPLL_SSC_EN (12ACH[29]) from 0 to 1 (DDS SSC enable). 20us after RG_MEMPLL_BIAS_RST from FNPLL AN (It is enough here)
	 u4Val = ucDram_Register_Read(mcSET_PHY_REG_ADDR(0x2ac));
	 mcSET_BIT(u4Val, 29);
	 ucDram_Register_Write(mcSET_PHY_REG_ADDR(0x2ac), u4Val);
		 

 #endif
	 return 0;
 }	
 static MT_RESULT_T _MTSYS_ENABLEJTAGPORT(unsigned long arg)
 {
	 BOOL fgEnable;
     UINT32 ui4_country=0;
	 fgEnable = (BOOL)arg;
	 ui4_country =  GetTargetCountry(); 
	 if(fgEnable)
	 {
	      if(ui4_country==COUNTRY_EU)//"hdmi 0-2 to Jtag for eu"
	      {
			  BSP_PinSet(PIN_HDMI_CEC, 2);//0: ice 2: gpio
			  BSP_PinSet(PIN_HDMI_0_SCL, 2);//0:gpio 2:ice
			  BSP_PinSet(PIN_HDMI_0_SDA, 2);//0:gpio 2:ice
			  BSP_PinSet(PIN_HDMI_2_SCL, 3);//0:gpio 3:ice
			  BSP_PinSet(PIN_HDMI_2_SDA, 3);//0:gpio 3:ice
			  BSP_PinSet(PIN_HDMI_1_HPD, 1);//  1 :gpio 
			  BSP_PinSet(PIN_USB_DM_P0, 0); //  0:gpio  
			  BSP_PinSet(PIN_OPCTRL6, 3);  //0:ice  3:gpio
			  BSP_PinSet(PIN_OPCTRL7, 3);//0:ice  3:gpio
			  BSP_PinSet(PIN_OPCTRL8, 3);//0:ice  3:gpio
			  BSP_PinSet(PIN_OPCTRL9, 3);//0:ice  3:gpio
	      }
		  else if (ui4_country==COUNTRY_US) //hdmi 0-1 to Jtag 
		  {
	          BSP_PinSet(PIN_HDMI_CEC, 2);
	          BSP_PinSet(PIN_HDMI_0_SCL, 2);
	          BSP_PinSet(PIN_HDMI_0_SDA, 2);
	          BSP_PinSet(PIN_HDMI_1_SCL, 2);
	          BSP_PinSet(PIN_HDMI_1_SDA, 2);
	          BSP_PinSet(PIN_HDMI_1_HPD, 1);
	          BSP_PinSet(PIN_USB_DM_P0, 0);
	          BSP_PinSet(PIN_OPCTRL6, 3);
	          BSP_PinSet(PIN_OPCTRL7, 3);
	          BSP_PinSet(PIN_OPCTRL8, 3);
	          BSP_PinSet(PIN_OPCTRL9, 3);
		  }
		  else
		  {
    		  BSP_PinSet(PIN_HDMI_CEC, 2);//0: ice 2: gpio
    		  BSP_PinSet(PIN_HDMI_0_SCL, 2);//0:gpio 2:ice
    		  BSP_PinSet(PIN_HDMI_0_SDA, 2);//0:gpio 2:ice
    		  BSP_PinSet(PIN_HDMI_2_SCL, 3);//0:gpio 3:ice
    		  BSP_PinSet(PIN_HDMI_2_SDA, 3);//0:gpio 3:ice
    		  BSP_PinSet(PIN_HDMI_1_HPD, 1);//	1 :gpio 
    		  BSP_PinSet(PIN_USB_DM_P0, 0); //	0:gpio	
    		  BSP_PinSet(PIN_OPCTRL6, 3);  //0:ice	3:gpio
    		  BSP_PinSet(PIN_OPCTRL7, 3);//0:ice  3:gpio
    		  BSP_PinSet(PIN_OPCTRL8, 3);//0:ice  3:gpio
    		  BSP_PinSet(PIN_OPCTRL9, 3);//0:ice  3:gpio
		  }
	 }
	 else 
	 {
		
		 if(ui4_country==COUNTRY_EU)//"hdmi 0-2 to Jtag for eu"
		 {
			 BSP_PinSet(PIN_HDMI_CEC, 1);
			 BSP_PinSet(PIN_HDMI_0_SCL, 1);//0:gpio 2:ice
			 BSP_PinSet(PIN_HDMI_0_SDA, 1);//0:gpio 2:ice
			 BSP_PinSet(PIN_HDMI_2_SCL, 1);//0:gpio 3:ice
			 BSP_PinSet(PIN_HDMI_2_SDA, 1);//0:gpio 3:ice
			 BSP_PinSet(PIN_HDMI_1_HPD, 0);//  1 :gpio 
			 
			 BSP_PinSet(PIN_USB_DM_P0, 0); //  0:gpio  
			 BSP_PinSet(PIN_OPCTRL6, 3);  //0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL7, 3);//0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL8, 3);//0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL9, 3);//0:ice  3:gpio
		 }
         else if(ui4_country==COUNTRY_US)
         {
	         BSP_PinSet(PIN_HDMI_0_SCL, 0);
	         BSP_PinSet(PIN_HDMI_0_SDA, 0);
	         BSP_PinSet(PIN_HDMI_1_SCL, 0);
	         BSP_PinSet(PIN_HDMI_1_SDA, 0);
         }
		 else
		 {
			 BSP_PinSet(PIN_HDMI_CEC, 1);
			 BSP_PinSet(PIN_HDMI_0_SCL, 1);//0:gpio 2:ice
			 BSP_PinSet(PIN_HDMI_0_SDA, 1);//0:gpio 2:ice
			 BSP_PinSet(PIN_HDMI_2_SCL, 1);//0:gpio 3:ice
			 BSP_PinSet(PIN_HDMI_2_SDA, 1);//0:gpio 3:ice
			 BSP_PinSet(PIN_HDMI_1_HPD, 0);//  1 :gpio 
			 
			 BSP_PinSet(PIN_USB_DM_P0, 0); //  0:gpio  
			 BSP_PinSet(PIN_OPCTRL6, 3);  //0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL7, 3);//0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL8, 3);//0:ice  3:gpio
			 BSP_PinSet(PIN_OPCTRL9, 3);//0:ice  3:gpio
		 }

	 }
	 return MTR_OK;
 
 }


 //-------------------------------------------------------------------------
 /*
        read hdcp key from peripheral device, such as nandflash or norflash or emmc, etc.
   */
 //-------------------------------------------------------------------------
 static MT_RESULT_T  _MTPERIPHERAL_Read_HDCP2X_Key (unsigned long arg)
 {
 
	 MTAL_IOCTL_3ARG_T rArg;
	 //UINT64 u4Offset = 0;
	 UINT8 *pu1Data_User = NULL;
	 UINT32 u4DataCnt;
	 UINT8 * pu1KernelBuf = NULL;	  
	 INT32 i4Ret;	 
	 
	 USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
	 COPY_FROM_USER_3ARG(arg, rArg);
 
	 //u4Offset = (UINT32)rArg.ai4Arg[0];
	 pu1Data_User = (UINT8*)rArg.ai4Arg[1];
	 u4DataCnt = (UINT32)rArg.ai4Arg[2];
 
 
	 printk("_MTPERIPHERAL_Read_HDCP2X_Key u4DataCnt(%d)\n",  u4DataCnt);

	 
	 //parameter check
	 if((pu1Data_User==NULL) || (u4DataCnt==0))
	 {
		 return MTR_PARAMETER_ERROR;
	 }
 
	 //add driver implementation code here
	 pu1KernelBuf = kmalloc(u4DataCnt, GFP_KERNEL);
	 if (!pu1KernelBuf)
	 {
		 return MTR_PARAMETER_ERROR;
	 }
	 
	 
    i4Ret = EEPHDCP2x_Read(0, (UINT32)pu1KernelBuf, u4DataCnt);
    if (0 != i4Ret)
    {
		if (pu1KernelBuf) 
		{
			kfree(pu1KernelBuf);
		}
		
        Printf("EEPHDCP2x_Read HDCP2.X KEY fail.\n");
        return 0;
    }


	 Printf("EEPHDCP2x_Read HDCP2.X KEY ok.\n");

	 

	 DumpHex(pu1KernelBuf, u4DataCnt);

	 printk("\n\n");



 
	 if(copy_to_user((void __user *)pu1Data_User, (const void *)pu1KernelBuf, u4DataCnt))
	 {
		 kfree(pu1KernelBuf);
		 return MTR_NOT_OK;
	 }
 
	 if (pu1KernelBuf) 
	 {
		 kfree(pu1KernelBuf);
	 }
 
	 if (i4Ret == 0)
	 {
		 return MTR_OK;
	 }
	 else
	 {
		 return MTR_NOT_OK;
	 }

	 
 }


int mtal_ioctl_mtperipheral(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
		case MTAL_IO_MTPERIPHERAL_PWM_SetVsyncAlign:
					return  _MTDDI_PWM_SetVsyncAlign(arg);
		case MTAL_IO_MTPERIPHERAL_DDI_UART_AccessReg:
					return  _MTDDI_UART_AccessReg(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_SETDIRECTION:
                    return  _MTGPIO_SetDirection(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_GETDIRECTION:
                    return  _MTGPIO_GetDirection(arg);
            case MTAL_IO_MTPERIPHERAL_GPIO_QUERY_OUTVAL:
                    return  _MTGPIO_QueryOutVal(arg);
        case MTAL_IO_MTPERIPHERAL_GPIO_OUTPUT:
                    return  _MTGPIO_Output(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_INTRQ:
                    return  _MTGPIO_Intrq(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_INTRQ_QUERY:
					return	_MTGPIO_IntrqQuery(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_QUERY:
                    return  _MTGPIO_Query(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_REG:
                    return  _MTGPIO_Reg(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_SETOUT:
                    return  _MTGPIO_SetOut(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_GETIN:
                    return  _MTGPIO_GetIn(arg);
		case MTAL_IO_MTPERIPHERAL_PWM_SETFREQUENCY:
                    return  _MTPWM_SetFrequency(arg);
		case MTAL_IO_MTPERIPHERAL_PWM_SETINIT:
					return  _MTPWM_Init(arg);
	    case MTAL_IO_MTPERIPHERAL_PWM_SETPARAMETER:
					return  _MTPWM_ApplyParam(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_READSERVOVALUE:
                    return  _MTPDWNC_ReadServoADCChannelValue(arg);
		case MTAL_IO_MTPERIPHERAL_SIF_INIT:
                    return  _MTSIF_Init();
		case MTAL_IO_MTPERIPHERAL_SIF_STOP:
                    return  _MTSIF_Stop();
		case MTAL_IO_MTPERIPHERAL_SIF_WRITEMULTIPLE:
                    return  _MTSIF_WriteMultipleSubAddr(arg);
		case MTAL_IO_MTPERIPHERAL_SIF_READMULTIPLE:
                    return  _MTSIF_ReadMultipleSubAddr(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_INIT:
					return	_MTSIFSW_Init(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_WRITEDATA:
					return	_MTSIFSW_X_Write(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_READDATA:
					return	_MTSIFSW_X_Read(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_WRITEDATA_NO_SUB_ADDR:
					return	_MTSIFSW_WriteDataNoSubAddr(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_READDATA_NO_SUB_ADDR:
					return	_MTSIFSW_ReadDataNoSubAddr(arg);
		case MTAL_IO_MTPERIPHERAL_SIFSW_REMOVE:
					return	_MTSIFSW_Remove();
		case MTAL_IO_MTPERIPHERAL_RTC_RTCTOUTC:
                    return  _MTRTC_RtcToUtc(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_UTCTORTC:
                    return  _MTRTC_UtcToRtc(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_INIT:
                    return  _MTRTC_Init();
		case MTAL_IO_MTPERIPHERAL_RTC_GETTIMEDATA:
                    return  _MTRTC_GetTimeDate(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_SETTIMEDATA:
                    return  _MTRTC_SetTimeDate(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_SETALERTTIME:
                    return  _MTRTC_SetAlertTime(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_GETALERTTIME:
                    return  _MTRTC_GetAlertTime(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_HOOKALERTHANDLER:
                    return  _MTRTC_HookAlertHandler(arg);
		case MTAL_IO_MTPERIPHERAL_RTC_SETWAKEUP:
                    return  _MTRTC_SetWakeup(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_ERASE:
                    return  _MTNOR_Erase(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_ERASEADDR:
                    return  _MTNOR_EraseAddr(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_GETSIZE:
                    return  _MTNOR_GetSize(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_READ:
                    return  _MTNOR_Read(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_WRITE:
                    return  _MTNOR_Write(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_WRONE:
                    return  _MTNOR_WrOnE(arg);
		case MTAL_IO_MTPERIPHERAL_NOR_CALCRC:
                    return  _MTNOR_CalCRC(arg);
		case MTAL_IO_MTPERIPHERAL_NAND_INIT:
                    return  _MTNAND_Init();
		case MTAL_IO_MTPERIPHERAL_NAND_RESET:
                    return  _MTNAND_Reset();
		case MTAL_IO_MTPERIPHERAL_NAND_READID:
                    return  _MTNAND_ReadID(arg);
		case MTAL_IO_MTPERIPHERAL_NAND_GETSIZE:
                    return  _MTNAND_GetSize(arg);
		case MTAL_IO_MTPERIPHERAL_NAND_READBLOCKS:
                    return  _MTNAND_ReadBlocks(arg);
		case MTAL_IO_MTPERIPHERAL_NAND_WRITEBLOCKS:
                    return  _MTNAND_WriteBlocks(arg);
        #if defined(CC_MSDC_ENABLE)
        case MTAL_IO_MTPERIPHERAL_MSDC_READ:
                    return  _MTMSDC_Read(arg);
		case MTAL_IO_MTPERIPHERAL_MSDC_WRITE:
                    return  _MTMSDC_Write(arg);                   
#ifdef CC_PARTITION_WP_SUPPORT
        case MTAL_IO_MTPERIPHERAL_MSDC_WPconfig:
                    return  _MTMSDC_WPConfig(arg);
#endif
        #endif
		case MTAL_IO_MTPERIPHERAL_IR_INIT:
                    return  _MTIR_Init(arg);
		case MTAL_IO_MTPERIPHERAL_IR_POLL:
                    return  _MTIR_Poll(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GETWAKEUPIRKEY:
					return  _MTIR_GetWakeupIrKey(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GETHOTKEYNUM:
					return 	_MTIR_GetHotkeyNum(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GETALLHOTKEY:
					return 	_MTIR_GetAllHotKey(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_SENDMTKIRKEY:
					return 	_MTIR_SendMtkIr(arg);
		case MTAL_IO_MTPERIPHERAL_IRRX_PROGRAM_RC56_WAKEUPKETS:
					return 	_MTIRRX_RC56_ProgramWakeupKeysRc(arg);
#ifdef CONFIG_NATIVE_CB2
		case MTAL_IO_MTPERIPHERAL_PDWNC_RELEASEQUEUE:
					return 	_MTPDWNC_RELEASE_QUEUE(arg);
#endif
#ifdef CC_PDWNC_REBOOT_NOTIFY
		case MTAL_IO_MTPERIPHERAL_PDWNC_LETGO_STANDBY:
					return 	_MTPDWNC_LETGO_STANDBY(arg);
#endif
		case MTAL_IO_MTPERIPHERAL_IR_RAW_BIT_TO_EVT_CODE:
                    return	_MTIR_RawBitToEvtCode(arg);
		case MTAL_IO_MTPERIPHERAL_IR_WAIT:
                    return  _MTIR_Wait(arg);
        case MTAL_IO_MTPERIPHERAL_IR_SET_RC56_SYSTEM_CODE:
                    return  _MTIR_SetFilterSystemCode(arg);
        case MTAL_IO_MTPERIPHERAL_QUIET_HOT_BOOT_CLEAR:
                    return  _MTIR_QuietHotBootClr(arg);
        case MTAL_IO_MTPERIPHERAL_IS_QUIET_HOT_BOOT_POWER_ON:
                    return  _MTIR_IsQuietHotBootPowerOn(arg);
	    case MTAL_IO_MTPERIPHERAL_SET_NUMBERS_OF_KEY:
                    return  _MTIR_SetTheNumberOfKey(arg);
		case MTAL_IO_MTPERIPHERAL_SET_THE_LAST_OF_FIVE_KEY:
			        return  _MTIR_SetTheLastOfFiveKeys(arg);
		case MTAL_IO_MTPERIPHERAL_PINMUX_SET:
                    return  _MTPINMUX_Set(arg);
		case MTAL_IO_MTPERIPHERAL_PINMUX_SET_EX:
                    return  _MTPINMUX_SetEx(arg);
		case MTAL_IO_MTPERIPHERAL_POWER_ENTERSTANDBY:
                    return  _MTPDWNC_EnterStandby(arg);
		case MTAL_IO_MTPERIPHERAL_REQUESTSUSPEND:
					return	_MTPDWNC_RequestSuspendEvent(arg);
		case MTAL_IO_MTPERIPHERAL_POWER_ENTERPOWERDOWN:
					return	_MTPDWNC_EnterPowerDown(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_ENTER_POWER_STATE:
                    return  _MTPDWNC_EnterPowerState(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_REBOOT:
					return	_MTPDWNC_Reboot();
		case MTAL_IO_MTPERIPHERAL_PDWNC_REBOOT1:
					return	_MTPDWNC_Reboot1();   
		case MTAL_IO_MTPERIPHERAL_PDWNC_BY_AC_CUT:
					return	_MTPDWNC_PowerDownByAcCut(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_BY_POWER_BUTTON:
					return	_MTIRRC_IsPowerButtonOff(arg);
		case MTAL_IO_MTPERIPHERAL_T8032_CMD:
                    return  _MTPDWNC_T8032Cmd(arg);
		case MTAL_IO_MTPERIPHERAL_POWER_WAKEUP_REASON:
                    return  _MTPDWNC_GetWakeupReason(arg);
		case MTAL_IO_MTPERIPHERAL_CHECK_CUSTOM_WAKEUP_REASON:
					return	_MTCheckCustomWakeupReason(arg);
		case MTAL_IO_MTPERIPHERAL_SET_POWER_DOWN_REASON:
					return	_MTPDWNC_SetPowerDownReason(arg);
		case MTAL_IO_MTPERIPHERAL_POWER_SETWAKEUPSOURCE:
                    return  _MTPDWNC_SetWakeupSource(arg);
		case MTAL_IO_MTPERIPHERAL_SET_RTC_WAKEUP_ENABLE:
                    return  _MTPDWNC_SetRtcWakeupEnable(arg);
        case MTAL_IO_MTPERIPHERAL_PDWNC_CREATE_WTTHREAD:
                    return _MTPDWNC_CreateWatchDogThread(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_SET_PWM:
                    return _MTPDWNC_PwmSetPwm(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_SET_PWM_BREATHLIGHT:
                    return _MTPDWNC_PwmSetBreathLight(arg);			
		case MTAL_IO_MTPERIPHERAL_GPIO_PERIODLEVELCALLBACK:
                    return  _MTGPIO_PeriodLevelCallback(arg);
		case MTAL_IO_MTPERIPHERAL_GPIO_INIT:
                    return  _MTGPIO_Init();
		case MTAL_IO_MTPERIPHERAL_PWM_SETPHASE:
                    return  _MTPWM_SetPhase(arg);
		case MTAL_IO_MTPERIPHERAL_SCRBT_CHECK:
                    return  _MTSCRBT_Check(arg);
		case MTAL_IO_MTPERIPHERAL_DRAM_GETBANDWIDTHUSAGE:
                    return  _MTDRAM_GetBandwidthUsage(arg);
        case MTAL_IO_MTPERIPHERAL_PWM_SETSCAN_ENABLE:
                    return  _MTPWM_ScanPWMEnable(arg);
        case MTAL_IO_MTPERIPHERAL_PWM_SETSCAN_DUTY:
                    return  _MTPWM_SetScanPWM(arg);
		case MTAL_IO_MTPERIPHERAL_SET_SCANPWM_AUTO:
					return  _MTPWM_ScanPWMAutoTestEn(arg);
#ifdef CC_SUPPORT_SMC
		case MTAL_IO_MTPERIPHERAL_SMC_INIT:
                    return  _MTSMC_Init(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_GETCARDPRESENCE:
                    return  _MTSMC_GetCardPresence(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_RESET:
                    return  _MTSMC_Reset(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_SENDRECEIVEAPDU:
                    return  _MTSMC_SendReceiveAPDU(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_REQUESTIFS:
                    return  _MTSMC_RequestIFS(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_ACTIVATE:
                    return  _MTSMC_Activate(arg);
		case MTAL_IO_MTPERIPHERAL_SMC_DEACTIVATE:
                    return  _MTSMC_Deactivate(arg);
    case MTAL_IO_MTPERIPHERAL_SMC_SET_PROTOCOL:
                    return _MTSMC_SetSmcProtocol(arg);
#endif
		case MTAL_IO_MTPERIPHERAL_EEPROM_INIT:
                    return  _MTEEPROM_Init();
		case MTAL_IO_MTPERIPHERAL_EEPROM_READ:
                    return  _MTEEPROM_Read(arg);
		 case MTAL_IO_MTPERIPHERAL_HDCP2X_KEY_READ:
					 return  _MTPERIPHERAL_Read_HDCP2X_Key(arg);
		case MTAL_IO_MTPERIPHERAL_EEPROM_WRITE:
                    return  _MTEEPROM_Write(arg);
		case MTAL_IO_MTPERIPHERAL_EEPROM_GETPROPERTY:
                    return  _MTEEPROM_GetProperty(arg);
		case MTAL_IO_MTPERIPHERAL_EEHDCP_Offset:
					return  _MTEEPROM_GetHdcp2xOffset(arg);
		case MTAL_IO_MTPERIPHERAL_EEPDTV_GETCFG:
                    return  _MTEEPDTV_GetCfg(arg);
		case MTAL_IO_MTPERIPHERAL_EEPDTV_SETCFG:
                    return  _MTEEPDTV_SetCfg(arg);
        case MTAL_IO_MTPERIPHERAL_EEPHDCPINIT:
                    return _MTEEPHDCPInit(arg);
        case MTAL_IO_MTPERIPHERAL_EEPNPTV_READ:
                    return _MTEEPNPTV_Read(arg);
		case MTAL_IO_MTPERIPHERAL_BIM_GETDEVID:
					return	_MTBIM_GetDeviceID(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GETPOWERSTATE:
					return	_MTPDWNC_GetPowerState(arg);
        case MTAL_IO_MTPERIPHERAL_SET_BW_PARAMETERS:
            		return  _MTAGENT_SetBandwidthParam(arg);
		case MTAL_IO_MTPERIPHERAL_SET_WDTIMEOUT:
					return _MTPDWNC_SetWatchDogTimeOut(arg);
		case MTAL_IO_MTPERIPHERAL_GET_WDTIMEOUT:
					return _MTPDWNC_GetWatchDogTimeOut(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GSPI_INT:
					return _MTPDWNC_GspiInit(arg);
		case MTAL_IO_MTPERIPHERAL_PDWNC_GSPI_RW:
					return _MTPDWNC_GspiReadWrite(arg);
		case MTAL_IO_MTPERIPHERAL_SYS_SetDramBWEnable:
			return _MTSYS_SetDramBWEnable(arg);
		case MTAL_IO_MTPERIPHERAL_SYS_GetDramBWInfo:
			return _MTSYS_GetDramBWInfo(arg);
		case MTAL_IO_MTPERIPHERAL_SYS_MEM_SSC:
			return _MTSYS_DDR_SSC(arg);
		case MTAL_IO_MTPERIPHERAL_SYS_JTAG_PORT_ENABLE:
			return _MTSYS_ENABLEJTAGPORT(arg);			
		default:
                    return  MTR_NOT_OK;
      }
	return MTR_OK;
 }

