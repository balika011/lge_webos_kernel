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
 * $RCSfile: io_mtvfe.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file io_mtvfe.c
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "mtsys.h"
#include <linux/mtal_ioctl.h>
#include "nptv_if.h"
#include "eeprom_if.h"

#include "x_bim.h"
#include "x_pdwnc.h"
#include "pdwnc_drvif.h"
#include "mpv_drvif.h"
#include "drv_dbase.h"
#include "fbm_drvif.h"
#include "osd_drvif.h"
#include "srm_drvif.h"
#include "x_linux.h"
#include "x_pinmux.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define DRV_PARALLEL_INIT

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(INT32)))                            \
   {                                                       \
        printk("mtal_ioctl_mtsys argument error\n");     \
        return MTR_ERR_INV;                                          \
    } 
#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_mtsys argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                           

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_mtsys argument error\n");      \
        return MTR_ERR_INV;                                          \
    }  

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_auddec argument error\n");      \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_mtsys argument error\n");     \
        return MTR_ERR_INV;                                          \
    }                                                           

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_mtsys argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static CHAR* _aszDriverInitStr[MAX_SW_TIMESTAMP_SIZE];
static UINT32 _u4DriverIndex = 0;

//-----------------------------------------------------------------------------
/** _LedBlinkingThread
 */
//-----------------------------------------------------------------------------
static void _LedBlinkingThread(void *pvArgs)
{
    static UINT32 _fgLed = 0;
    UINT32 u4BackLightGpio, u4BackLightOnVal, u4BlinkingLedGpio, u4BlinkingLedOnVal, u4LedBlinkingPeriod;    
    if (DRVCUST_InitQuery(eBackLightGpio, &u4BackLightGpio) == 0)
    {
        UNUSED(DRVCUST_InitQuery(eBackLightOnVal, &u4BackLightOnVal));
        UNUSED(DRVCUST_InitQuery(eBootBlinkingLed, &u4BlinkingLedGpio));            
        UNUSED(DRVCUST_InitQuery(eBootBlinkingLedPeriod, &u4LedBlinkingPeriod));
        Printf("[LedBlinking] Gpio_(%d), toggle (%d)ms\n",u4BlinkingLedGpio, u4LedBlinkingPeriod);
        while(!((GPIO_Output(u4BackLightGpio, NULL) == u4BackLightOnVal) && (GPIO_Enable(u4BackLightGpio, NULL) == 1)))
        {
            _fgLed = !_fgLed;
            GPIO_SetOut(u4BlinkingLedGpio, _fgLed);
            x_thread_delay(u4LedBlinkingPeriod);
        }
        Printf("[LedBlinkikng] Panel is turned on, stop blinking led!!!\n");
        UNUSED(DRVCUST_InitQuery(eBootBlinkingLedOnVal, &u4BlinkingLedOnVal));            
        GPIO_SetOut(u4BlinkingLedGpio, u4BlinkingLedOnVal);
    }
    return;    
}


//-----------------------------------------------------------------------------
/** _MTSYS_LEDBlink
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_LEDBlink(unsigned long arg)
{
    HANDLE_T hLedBlinkingThread;
    UINT32 i4LedBlinking;
    
    //PDWNC_SetT8032ToggleLed(0);
    if((DRVCUST_InitQuery(eFlagBootBlinkingLedSupport, &i4LedBlinking) == 0))
    {
        if ((!DRVCUST_InitGet(eLoaderLogoNumber)) && (i4LedBlinking))
        {
            x_thread_create(&hLedBlinkingThread, "DrvLedBlinking",
                                   2048,
                                   100,
                                   _LedBlinkingThread, 0, NULL);
      
        }
    }

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_ShowLogo
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_ShowLogo(unsigned long arg)
{
    UNUSED(arg);

    if (DRVCUST_OptGet(eDrvLogoVdecEnable))
    {
        MPV_ShowLogo();
    }
    else
    {
        OSD_ShowLogo();
    }
    
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_SRM_Init
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_SRM_Init(unsigned long arg)
{
    UNUSED(arg);

    SRM_Init();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_SRM_Pause
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_SRM_Pause(unsigned long arg)
{
    UNUSED(arg);

    SRM_Pause();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_SRM_Resume
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_SRM_Resume(unsigned long arg)
{
    UNUSED(arg);
    
    SRM_Resume();
    
    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_SRM_SetMmMode
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_SRM_SetMmMode(unsigned long arg)
{
    SRM_MM_MODE_T eMode;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    eMode = (SRM_MM_MODE_T)Arg;

    SRM_SetMmMode(eMode);

    return MTR_OK;
}

static MT_RESULT_T _MTSYS_SRM_SetTvMode(unsigned long arg)
{
    SRM_TV_MODE_T eMode;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    eMode = (SRM_TV_MODE_T)Arg;

    SRM_SetTvMode(eMode);

    return MTR_OK;
}

static MT_RESULT_T _MTSYS_SRM_GetTvMode(unsigned long arg)
{
    SRM_TV_MODE_T eMode;
    SRM_TV_MODE_T *pMode;
    INT32 Arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg, INT32);
    COPY_FROM_USER_1ARG(arg, Arg);

    pMode = (SRM_TV_MODE_T *)Arg;

    eMode = SRM_GetTvMode();

    USR_SPACE_ACCESS_VALIDATE_ARG(pMode,SRM_TV_MODE_T);
    COPY_TO_USER_ARG(pMode, eMode, SRM_TV_MODE_T);

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_FBM_Init
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_FBMInit(unsigned long arg)
{
    UNUSED(arg);

    FBM_Init();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_DBS_Init
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_DBSInit(unsigned long arg)
{
    UNUSED(arg);

    DBS_Init();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_DC_ONOFF_Test
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_DC_ONOFF_Test(unsigned long arg)
{
    UNUSED(arg);

    PDWNC_CreateDCOnOffTestThread();

    return MTR_OK;
}


//-----------------------------------------------------------------------------
/** _MTSYS_Drv_SetTimestamp
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T _MTSYS_Drv_SetTimestamp(unsigned long arg)
{
    static BOOL _fgFirst = TRUE;
    MTAL_IOCTL_2ARG_T    rArg;
    UINT32 u4Strlen;
    CHAR *szString, *szTmp;

    if(_fgFirst)
    {
        UINT8 i;

        for(i=0; i<MAX_SW_TIMESTAMP_SIZE; i++)
        {
            _aszDriverInitStr[i] = NULL;
        }
        _u4DriverIndex = 0;
        _fgFirst = FALSE;
    }

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    szString = (CHAR*)rArg.ai4Arg[0];
    u4Strlen = (UINT32)rArg.ai4Arg[1];

    if(_aszDriverInitStr[_u4DriverIndex] != NULL)
    {
        x_mem_free((void*)_aszDriverInitStr[_u4DriverIndex]);
    }

    _aszDriverInitStr[_u4DriverIndex] = (CHAR*)x_mem_alloc(u4Strlen + 1);
    if(_aszDriverInitStr[_u4DriverIndex] == NULL)
    {
        return MTR_NOT_OK;
    }    
    if (!access_ok(VERIFY_READ, (void __user *)szString,u4Strlen))
    {
        return MTR_ERR_INV;
    }                                                           
    if (copy_from_user(_aszDriverInitStr[_u4DriverIndex], (void __user *)szString, u4Strlen))
    {
        return MTR_ERR_INV;
    }
    szTmp = _aszDriverInitStr[_u4DriverIndex];
    szTmp[u4Strlen] = '\0';
    
    x_os_drv_set_timestamp(_aszDriverInitStr[_u4DriverIndex]);

    _u4DriverIndex++;
    if(_u4DriverIndex >= MAX_SW_TIMESTAMP_SIZE)
    {
        _u4DriverIndex = 0;
    }

    return MTR_OK;
}

#ifdef CC_MT5398
extern BOOL fgDedicateJTag;
extern BOOL fgHDMIJTag0_1;
extern BOOL fgHDMIJTag1_2;
extern BOOL fgHDMIJTag2_3;
extern BOOL fgUSBJTag;
#endif

static MT_RESULT_T _MTSYS_SetJtagPortEnable(unsigned long arg)
{
#ifdef CC_MT5398
    UINT32 u4Enable;
    INT32 i4Ret = 0;
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, u4Enable);    
    if(!u4Enable)
    {
        Printf("Restore Jtag related pins' function to initial state... ");
    	i4Ret += BSP_PinSet(PIN_JTCK, 2);
    	i4Ret += BSP_PinSet(PIN_JTDO, 2);
    	i4Ret += BSP_PinSet(PIN_JTMS, 2);
    	i4Ret += BSP_PinSet(PIN_JTDI, 2);
    	i4Ret += BSP_PinSet(PIN_JTRST_, 2);
        i4Ret += BSP_PinSet(PIN_HDMI_0_SCL, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_0_SDA, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_1_SCL, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_1_SDA, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_2_SCL, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_2_SDA, 1);
    	i4Ret += BSP_PinSet(PIN_HDMI_3_SDA, 1);
    	i4Ret += BSP_PinSet(PIN_HDMI_3_SCL, 1);
    	i4Ret += BSP_PinSet(PIN_HDMI_1_HPD, 0);
    	i4Ret += BSP_PinSet(PIN_HDMI_2_HPD, 0);
        i4Ret += BSP_PinSet(PIN_USB_DM_P0, 0);
        if(i4Ret == 0)
        {
            Printf("ok!\n");
    	    fgDedicateJTag = FALSE;
    	    fgHDMIJTag0_1 = FALSE;
    	    fgHDMIJTag1_2 = FALSE;
    	    fgHDMIJTag2_3 = FALSE;
    	    fgUSBJTag = FALSE;
        }
    	else
    	{
    	    Printf("fail!\n");
    	}
    }
    else
    {
        Printf("hdmi 1-2 to Jtag... ");
        i4Ret += BSP_PinSet(PIN_JTCK, 1);
        i4Ret += BSP_PinSet(PIN_JTDO, 1);
        i4Ret += BSP_PinSet(PIN_JTMS, 1);
        i4Ret += BSP_PinSet(PIN_JTDI, 1);
        i4Ret += BSP_PinSet(PIN_JTRST_, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_0_SCL, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_0_SDA, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_1_SCL, 2);
        i4Ret += BSP_PinSet(PIN_HDMI_1_SDA, 2);
        i4Ret += BSP_PinSet(PIN_HDMI_2_SCL, 2);
        i4Ret += BSP_PinSet(PIN_HDMI_2_SDA, 2);
        i4Ret += BSP_PinSet(PIN_HDMI_3_SDA, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_3_SCL, 1);
        i4Ret += BSP_PinSet(PIN_HDMI_1_HPD, 0);
        i4Ret += BSP_PinSet(PIN_HDMI_2_HPD, 1);
        i4Ret += BSP_PinSet(PIN_USB_DM_P0, 0);
        if(i4Ret == 0)
        {
            Printf("ok!\n");
            fgDedicateJTag = FALSE;
            fgHDMIJTag0_1 = FALSE;
            fgHDMIJTag1_2 = TRUE;
            fgHDMIJTag2_3 = FALSE;
            fgUSBJTag = FALSE;
        }
        else
        {
            Printf("fail!\n");
        }        			
    }
    return MTR_OK;
#else
    UNUSED(arg);
    return MTR_NOT_OK;
#endif    
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

int mtal_ioctl_mtsys(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = MTR_OK;
	   
	switch (cmd) 
	{				
		case MTAL_IO_SYS_LEDBLINK:
			i4Ret = _MTSYS_LEDBlink(arg);
			break;

	    case MTAL_IO_SYS_SHOWLOGO:
	        i4Ret = _MTSYS_ShowLogo(arg);
	        break;

        case MTAL_IO_SYS_SRMINIT:
            i4Ret = _MTSYS_SRM_Init(arg);
            break;

	    case MTAL_IO_SYS_SRMPAUSE:
	        i4Ret = _MTSYS_SRM_Pause(arg);
	        break;

	    case MTAL_IO_SYS_SRMRESUME:
	        i4Ret = _MTSYS_SRM_Resume(arg);
	        break;

	    case MTAL_IO_SYS_SRM_SETMMMODE:
	        i4Ret = _MTSYS_SRM_SetMmMode(arg);
	        break;

	    case MTAL_IO_SYS_SRM_SETTVMODE:
	        i4Ret = _MTSYS_SRM_SetTvMode(arg);
	        break;

	    case MTAL_IO_SYS_SRM_GETTVMODE:
	        i4Ret = _MTSYS_SRM_GetTvMode(arg);
	        break;

	    case MTAL_IO_SYS_FBMINIT:
	        i4Ret = _MTSYS_FBMInit(arg);
	        break;

	    case MTAL_IO_SYS_DBSINIT:
	        i4Ret = _MTSYS_DBSInit(arg);
	        break;

	    case MTAL_IO_SYS_DCONOFFTEST:
	        i4Ret = _MTSYS_DC_ONOFF_Test(arg);
	        break;

	    case MTAL_IO_SYS_DRV_SETTIMESTAMP:
	        i4Ret = _MTSYS_Drv_SetTimestamp(arg);
	        break;
	        
		case MTAL_IO_SYS_JTAG_ENABLE:
            i4Ret = _MTSYS_SetJtagPortEnable(arg);
            break;
		default:
			i4Ret = MTR_NOT_SUPPORTED;
	}
	return i4Ret;
}

