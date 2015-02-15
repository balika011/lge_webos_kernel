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
 * $RCSfile: io_mtvdo.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file mtvdo.c
 *  This file contains implementation of exported APIs of Video (VDO) module.
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#ifdef CC_MT5395
#define CC_MT5365
#endif
#ifdef CC_MT5389
#define CC_MT5368
#endif

#include "vdp_if.h"
#include "video_def.h"
#include "drv_auto.h"

#ifdef CC_SUPPORT_TVE_OSD_OUT
#include "tve_ctrl.h"
#endif
#include "mtvdo.h"
#include "mtvdecex.h"
#include "cb_low.h"
#include <linux/mtal_ioctl.h>


#include "nptv_drvif.h"
#include "vdp_drvif.h"
#include "api_srch_if.h"
#include "drv_scaler.h"
#include "drv_tdtv_drvif.h"
#include "drv_di.h"
#include "drv_nr.h"
#include "drv_predown.h"
#if defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
#include "drv_mon_out.h"
#include "drv_dvi.h"
#endif
#if defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
	#include "drv_lcdim_v2.h"
#else
	#include "drv_lcdim.h"
#endif	
#include "drv_ldmspi.h"

#if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
#include "drv_mjc_if.h"
#endif
#include "drv_display.h"
#include "nptv_drvif.h"
#include "drv_od.h"
#include "mute_if.h"

#include "vdo_misc.h"
#include "b2r_drvif.h"


#ifdef CC_MT5365
#include "drv_meter.h"
#endif

#include "osd_drvif.h"
#include "srm_drvif.h"
#include "fbm_drvif.h"
#include "eeprom_if.h"
#include "panel.h"
#include "panel_cust.h"

#include "drv_video.h"
#include "x_hal_5381.h"
#include "x_hal_arm.h"

#ifdef DRV_SUPPORT_EXTMJC
#include "extmjc_if.h"
#endif
#ifdef CC_USE_DDI
#include "hw_psc.h"
#include "drv_scaler_psc.h"
#endif
//#include "hw_ycproc.h"
//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define definitions
//-----------------------------------------------------------------------------
extern BOOL bIsAutoMute;

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define USR_SPACE_ACCESS_VALIDATE_1ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(int)))                            \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_1ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(int)))                        \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return -1;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return -1;                                          \
    }
#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");     \
        return MTR_ERR_INV;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        printk("mtal_ioctl_video valid argument error\n");     \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel,(void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
    
#define COPY_TO_USER_DATA(ArgUsr, ArgKernel, size)              \
    if (copy_to_user((void __user *)ArgUsr, (void *)ArgKernel,   \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }
#define COPY_FROM_USER_DATA(ArgUsr, ArgKernel, size)              \
    if (copy_from_user((void *)ArgKernel,(void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        printk("mtal_ioctl_video argument error\n");      \
        return MTR_ERR_INV;                                          \
    }

#define COPY_TO_USER_DATA_ALLOC(ArgUsr, ArgKernel, size)            \
    if ((ArgKernel == NULL) || (copy_to_user((void __user *)ArgUsr, \
        (void *)ArgKernel, size)))                                  \
    {                                                               \
        printk("mtal_ioctl_video argument error\n");                \
        kfree(ArgKernel);                                           \
        return MTR_ERR_INV;                                         \
    }
#define COPY_FROM_USER_DATA_ALLOC(ArgUsr, ArgKernel, size)          \
    if ((ArgKernel == NULL) || (copy_from_user((void *)ArgKernel,   \
        (void __user *)ArgUsr, size)))                              \
    {                                                               \
        printk("mtal_ioctl_video argument error\n");                \
        kfree(ArgKernel);                                           \
        return MTR_ERR_INV;                                         \
    }

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
extern UINT32 _u4OversampleDependent;  // io_mtvdecex.c
static UINT8 u1LboxEn=0;
static UINT16 u2DynamicScale=0;
#if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
extern UINT8 bSupportDejudder;
#endif
extern UINT8 bSupportLocalDimming;
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
#ifdef CC_USE_DDI
static void _DisableBootLogo(void)
{
    static UINT32 _u4IsBootLogoDisabled = 0;
    if(!_u4IsBootLogoDisabled)
    {
    // Mute OSD before driver init.
        IO_WRITE32(OSD_BASE, 0x200, IO_READ32(OSD_BASE, 0x200) & 0xfffffffeU);
        IO_WRITE32(OSD_BASE, 0x300, IO_READ32(OSD_BASE, 0x300) & 0xfffffffeU);
        IO_WRITE32(OSD_BASE, 0x0, IO_READ32(OSD_BASE, 0x0) | 0x1U);
        x_thread_delay(40);
        #ifndef CC_MT5882
        while(PSC_READ_FLD(PSCSYS_20, PSC_STA_V_CNT) < PSC_READ_FLD(PSCSYS_03, PSC_PNL_VTOTAL)-20);
        #endif
        PMX_SetBg(0x00000000, FALSE);
        _u4IsBootLogoDisabled = 1;
    }
}
#endif
static MT_RESULT_T _MTVDO_Init(void)
{
	VDP_REGION_T rRegion = {0,0,0,0};
		MTVDO_PRINT(" - void\n");
	
		VDP_Init();
#ifdef CC_USE_DDI
		_DisableBootLogo();    
#endif    
		VdoMLInit();
    #ifdef PANEL_POWER_ON_AT_VDOINIT
		vApiPanelPowerSequence(TRUE);
    #endif
		
		VDP_SetSrcRegion((UCHAR)0, (UCHAR)1, *(VDP_REGION_T*)&rRegion);
		VDP_SetOutRegion((UCHAR)1, (UCHAR)1, *(VDP_REGION_T*)&rRegion);  
		return MTR_OK;

}

static MT_RESULT_T _MTVDO_Stop(VOID)
{
    MTVDO_PRINT(" - void\n");
    VDP_Stop();
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_Reset(unsigned long arg)
{
    MTVDO_PATH_T eVdpId;
    eVdpId = (MTVDO_PATH_T)arg;

    MTVDO_PRINT(" - eVdpId = %d\n", (int)eVdpId);
    VDP_Reset((UCHAR)eVdpId);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetDisplayOff(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, fgEnable = %d\n", (int)eVdpId, (int)fgEnable);
    VDP_SetDisplayOff((UINT32)eVdpId, (UINT32)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_Set3DpatternMode(unsigned long arg)
{
	 UINT32 u4Mode = (UINT32)arg;
	 MTVDO_PRINT("_MTVDO_Set3DpatternMode mode:%d\n",u4Mode);
	 Set3DpatternMode(u4Mode);
	 return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetDisplayOff_Ext(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;
    UINT32 u4DispOffModule;
    UINT32 u4CtrlByAp = 1;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];
    u4DispOffModule = (UINT32)rArg.ai4Arg[2];
    
    MTVDO_PRINT(" - eVdpId = %d, u4DispOffModule= %d,fgEnable = %d\n", 
                    (int)eVdpId,(int)u4DispOffModule, (int)fgEnable);
    DRVCUST_OptQuery(eDisplayOffCtrlByAp, (UINT32 *)&u4CtrlByAp);
    if (u4CtrlByAp)
    {
        VDP_SetDisplayOff_Ext((UINT32)eVdpId, u4DispOffModule ,(UINT32)fgEnable);
    }    
    return MTR_OK;
}
static MT_RESULT_T _MTVDO_SetSubCloneMode(unsigned long arg)
{
    BOOL fgEnable;
    fgEnable = (BOOL)arg;

    MTVDO_PRINT(" - fgEnable = %d\n", (int)fgEnable);
    bSubSrcCloneMode( fgEnable );
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetEnable(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, fgEnable = %d\n", (int)eVdpId, (int)fgEnable);
    VDP_SetEnable((UCHAR)eVdpId, (UCHAR)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetEnable(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UCHAR *pucEnable;
    UCHAR ucEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pucEnable = (UCHAR*)rArg.ai4Arg[1];

    VDP_GetEnable((UCHAR)eVdpId, &ucEnable);

    USR_SPACE_ACCESS_VALIDATE_ARG(pucEnable,UCHAR);
    COPY_TO_USER_ARG(pucEnable, ucEnable, UCHAR)
    MTVDO_PRINT(" - eVdpId = %d, ucEnable = %d\n", (int)eVdpId, (int)ucEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UCHAR ucMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    ucMode = (UCHAR)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, ucMode = %d\n", (int)eVdpId, (int)ucMode);
    VDP_SetMode((UCHAR)eVdpId, ucMode);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UCHAR *pucMode;
    UCHAR ucMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pucMode = (UCHAR*)rArg.ai4Arg[1];

    VDP_GetMode((UCHAR)eVdpId, &ucMode);

    USR_SPACE_ACCESS_VALIDATE_ARG(pucMode,UCHAR);
    COPY_TO_USER_ARG(pucMode, ucMode, UCHAR)
    MTVDO_PRINT(" - eVdpId = %d, ucMode = %d\n", (int)eVdpId, (int)ucMode);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetFreeze(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, fgEnable = %d\n", (int)eVdpId, (int)fgEnable);
    VDP_SetFreeze((UCHAR)eVdpId, (UCHAR)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T  _MTVDO_GetFreeze(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UCHAR *pucFreeze;
    UCHAR ucFreeze;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pucFreeze = (UCHAR*)rArg.ai4Arg[1];

    VDP_GetFreeze((UCHAR)eVdpId, &ucFreeze);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pucFreeze,UCHAR);
    COPY_TO_USER_ARG(pucFreeze, ucFreeze, UCHAR)
    MTVDO_PRINT(" - eVdpId = %d, ucFreeze = %d\n", (int)eVdpId, (int)ucFreeze);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetDotByDot(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, fgEnable = %d\n", (int)eVdpId, (int)fgEnable);
    VDP_Set121Map((UCHAR)eVdpId, (UCHAR)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetNonLinearMode(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT8 u1Mode;
    VDP_AUTO_NONLINEAR_PRM_T rNonlinearAutoPara;
    VDP_NONLINEAR_PRM_T rNonlinearManualPara;
    MTVDO_NONLINEAR_PRM_T rNonlinearPara;
    MTVDO_NONLINEAR_PRM_T *prNonlinearPara;

    VDP_NEW_NONLINEAR_PRM_T rNonlPrm;

    x_memset(&rNonlinearPara, 0, sizeof(MTVDO_NONLINEAR_PRM_T));
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u1Mode = (UINT8)rArg.ai4Arg[1];
    prNonlinearPara = (MTVDO_NONLINEAR_PRM_T*)rArg.ai4Arg[2];

    if(prNonlinearPara != NULL)
    {
    	USR_SPACE_ACCESS_VALIDATE_ARG(prNonlinearPara, MTVDO_NONLINEAR_PRM_T);
    	COPY_FROM_USER_ARG(prNonlinearPara, rNonlinearPara, MTVDO_NONLINEAR_PRM_T);
    }
    MTVDO_PRINT(" - eVdpId = %d, u1Mode = %d\n", (int)eVdpId, (int)u1Mode);
    if ( (u1Mode == MTVDO_NONLINEAR_AUTO_1) || (u1Mode == MTVDO_NONLINEAR_AUTO) || (u1Mode == MTVDO_NONLINEAR_MANUAL) || (u1Mode == MTVDO_NONLINEAR_ON) )
    {
       MTVDO_PRINT(" - rNonlinearPara start= %d, end = %d, slope = %d, middle = %d\n", 
    	   (int)rNonlinearPara.u4Start, (int)rNonlinearPara.u4End, (int)rNonlinearPara.u4Slope, (int)rNonlinearPara.u4MiddlePoint);
    }
    else if ( (u1Mode == MTVDO_NEW_NONLINEAR) || (u1Mode == MTVDO_NEW_NONLINEAR_ON) )
    {
      #if defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5860) || defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
         MTVDO_PRINT(" - src_ratio= %d, out_ratio = %d \n", (int)rNonlinearPara.u4SrcMiddieRatio,(int)rNonlinearPara.u4OutMiddleRatio);
      #else
         MTVDO_PRINT(" - Not support non_Linear mode.\n");
         return MTR_NOT_SUPPORTED;
      #endif  
    }

    switch(u1Mode)
    {
        case MTVDO_NONLINEAR_OFF:
            VDP_SetNonlinear((UCHAR)eVdpId, (UCHAR)FALSE);
            VDP_SetNewNonlinearOnOff((UCHAR)eVdpId, (UCHAR)FALSE);
            break;
        case MTVDO_NONLINEAR_ON:
            VDP_SetNonlinear((UCHAR)eVdpId, (UCHAR)TRUE);
            break;
        case MTVDO_NONLINEAR_AUTO_1:
            rNonlinearAutoPara.u1Mode = MTVDO_NONLINEAR_AUTO_1;
            rNonlinearAutoPara.u4Adaptive =0; //no use
            rNonlinearAutoPara.u4EndFactor =rNonlinearPara.u4End;
            rNonlinearAutoPara.u4Slope = rNonlinearPara.u4Slope;
            VDP_SetNonlinear((UCHAR)eVdpId, (UCHAR)TRUE);
            VDP_SetAutoNonLinear((UINT32)eVdpId, rNonlinearAutoPara);
            break;
        case MTVDO_NONLINEAR_AUTO:
            rNonlinearAutoPara.u1Mode = MTVDO_NONLINEAR_AUTO;
            rNonlinearAutoPara.u4Adaptive =0; //no use
            rNonlinearAutoPara.u4EndFactor =rNonlinearPara.u4End;
            rNonlinearAutoPara.u4Slope = rNonlinearPara.u4Slope;
            VDP_SetNonlinear((UCHAR)eVdpId, (UCHAR)TRUE);
            VDP_SetAutoNonLinear((UINT32)eVdpId, rNonlinearAutoPara);
            break;
        case MTVDO_NONLINEAR_MANUAL:
            rNonlinearManualPara.u4End=rNonlinearPara.u4End;
            rNonlinearManualPara.u4Slope = rNonlinearPara.u4Slope;
            rNonlinearManualPara.u4MiddlePoint = rNonlinearPara.u4MiddlePoint;
            rNonlinearManualPara.u4Start= rNonlinearPara.u4Start;
            VDP_SetNonlinear((UCHAR)eVdpId, (UCHAR)TRUE);
            VDP_SetManualNonLinear((UINT32)eVdpId, rNonlinearManualPara);
            break;

        case MTVDO_NEW_NONLINEAR_ON:
            VDP_SetNewNonlinearOnOff((UCHAR)eVdpId, (UCHAR)TRUE);
            break;
        case MTVDO_NEW_NONLINEAR:
            VDP_SetNewNonlinearOnOff((UCHAR)eVdpId, (UCHAR)TRUE);   //turn new non_linear on!
            rNonlPrm.u4SrcMiddleRatio=rNonlinearPara.u4SrcMiddieRatio;
            rNonlPrm.u4OutMiddleRatio=rNonlinearPara.u4OutMiddleRatio;
            VDP_SetNewNonLinear((UCHAR)eVdpId,rNonlPrm );
            break;
        default:
            return MTR_NOT_OK;
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetArg(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_ARG_TYPE_T eArgType;
    INT16 i2Value;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eArgType = (MTVDO_ARG_TYPE_T)rArg.ai4Arg[1];
    i2Value = (INT16)rArg.ai4Arg[2];

    MTVDO_PRINT(" - eVdpId = %d, eArgType = %d, i2Value = 0x%x\n", 
        (int)eVdpId, 
        (int)eArgType,
        i2Value);   

    PE_SetArg((UCHAR)eVdpId, (UCHAR)eArgType, i2Value);

    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    if (bSupportDejudder == FALSE)
    {
        if ((eArgType == MTVDO_ARG_DI_FILM_MODE) && (PANEL_IsSupport120Hz() == FALSE))
        {
            //MJC_UpdateRealCinemaMode(0xFFFF, i2Value, 0xFFFF);
        }    
    }
    #endif

    if (eArgType == MTVDO_ARG_TDTV_PACKING)
    {
        //MJC_UpdateRealCinemaMode(0xFFFF, 0xFFFF, i2Value);
    }

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetArg(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_ARG_TYPE_T eArgType;
    INT16 *pi2Value;
    INT16 i2Value;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eArgType = (MTVDO_ARG_TYPE_T)rArg.ai4Arg[1];
    pi2Value = (INT16*)rArg.ai4Arg[2];

    PE_GetArg((UCHAR)eVdpId, (UCHAR)eArgType, &i2Value);

    USR_SPACE_ACCESS_VALIDATE_ARG(pi2Value,INT16);
    COPY_TO_USER_ARG(pi2Value, i2Value, INT16)
    MTVDO_PRINT(" - eVdpId = %d, eArgType = %d, i2Value = 0x%x\n", 
        (int)eVdpId, 
        (int)eArgType, 
        i2Value);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetOverScan(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_OVERSCAN_REGION_T *prRegion;
    MTVDO_OVERSCAN_REGION_T rRegion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prRegion = (MTVDO_OVERSCAN_REGION_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_OVERSCAN_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_OVERSCAN_REGION_T);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rRegion.u4Top = %d\n"
	 "rRegion.u4Bottom = %d\n"
        "rRegion.u4Left = %d\n"
        "rRegion.u4Right = %d\n", 
        eVdpId,
        (int)rRegion.u4Top,
        (int)rRegion.u4Bottom,
        (int)rRegion.u4Left,
        (int)rRegion.u4Right);

    VDP_SetOverScan((UCHAR)eVdpId, *(VDP_OVERSCAN_REGION_T*)&rRegion);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetOverScan(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_OVERSCAN_REGION_T *prRegion;
    MTVDO_OVERSCAN_REGION_T rRegion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prRegion = (MTVDO_OVERSCAN_REGION_T*)rArg.ai4Arg[1];

    VDP_GetOverScan((UCHAR)eVdpId, (VDP_OVERSCAN_REGION_T*)&rRegion);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_OVERSCAN_REGION_T);
    COPY_TO_USER_ARG(prRegion, rRegion, MTVDO_OVERSCAN_REGION_T);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rRegion.u4Top = %d\n"
	 "rRegion.u4Bottom = %d\n"
        "rRegion.u4Left = %d\n"
        "rRegion.u4Right = %d\n", 
        eVdpId,
        (int)rRegion.u4Top,
        (int)rRegion.u4Bottom,
        (int)rRegion.u4Left,
        (int)rRegion.u4Right);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetSrcRegion(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_REGION_T *prSrcRegion;
    MTVDO_REGION_T rSrcRegion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prSrcRegion = (MTVDO_REGION_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prSrcRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prSrcRegion, rSrcRegion, MTVDO_REGION_T);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rSrcRegion.u4X = %d\n"
	 "rSrcRegion.u4Y = %d\n"
        "rSrcRegion.u4Width = %d\n"
        "rSrcRegion.u4Height = %d\n", 
        eVdpId,
        (int)rSrcRegion.u4X,
        (int)rSrcRegion.u4Y,
        (int)rSrcRegion.u4Width,
        (int)rSrcRegion.u4Height);    

    if (VDP_GetScalerPixelBased())
    {
        if (bDrvVideoGetWidthOverSample(eVdpId) && (_u4OversampleDependent == 0))
        {
            rSrcRegion.u4X = rSrcRegion.u4X << 1;
            rSrcRegion.u4Width = rSrcRegion.u4Width << 1;            
        }
    }
    VDP_SetSrcRegion((UCHAR)eVdpId, (UCHAR)0, *(VDP_REGION_T*)&rSrcRegion);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetSrcRegion(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_REGION_T *prSrcRegion;
    MTVDO_REGION_T rSrcRegion;
    UCHAR ucSrcFullRegion = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prSrcRegion = (MTVDO_REGION_T*)rArg.ai4Arg[1];

    VDP_GetSrcRegion((UCHAR)eVdpId, &ucSrcFullRegion, (VDP_REGION_T*)&rSrcRegion);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rSrcRegion.u4X = %d\n"
	 "rSrcRegion.u4Y = %d\n"
        "rSrcRegion.u4Width = %d\n"
        "rSrcRegion.u4Height = %d\n", 
        eVdpId,
        (int)rSrcRegion.u4X,
        (int)rSrcRegion.u4Y,
        (int)rSrcRegion.u4Width,
        (int)rSrcRegion.u4Height);
    
    if (VDP_GetScalerPixelBased())
    {
        if (bDrvVideoGetWidthOverSample(eVdpId) && (_u4OversampleDependent == 0))
        {
            rSrcRegion.u4X = rSrcRegion.u4X >> 1;
            rSrcRegion.u4Width = rSrcRegion.u4Width >> 1;            
        }
    }    

    USR_SPACE_ACCESS_VALIDATE_ARG(prSrcRegion, MTVDO_REGION_T);
    COPY_TO_USER_ARG(prSrcRegion, rSrcRegion, MTVDO_REGION_T);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetOutRegion(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_REGION_T *prOutRegion;
    MTVDO_REGION_T rOutRegion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prOutRegion = (MTVDO_REGION_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prOutRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prOutRegion, rOutRegion, MTVDO_REGION_T);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rOutRegion.u4X = %d\n"
	 "rOutRegion.u4Y = %d\n"
        "rOutRegion.u4Width = %d\n"
        "rOutRegion.u4Height = %d\n", 
        eVdpId,
        (int)rOutRegion.u4X,
        (int)rOutRegion.u4Y,
        (int)rOutRegion.u4Width,
        (int)rOutRegion.u4Height);  
    
    VDP_SetOutRegion((UCHAR)eVdpId, (UCHAR)0, *(VDP_REGION_T*)&rOutRegion);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetOutRegion(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_REGION_T *prOutRegion;
    MTVDO_REGION_T rOutRegion;
    UCHAR ucOutFullRegion = 0;    

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prOutRegion = (MTVDO_REGION_T*)rArg.ai4Arg[1];

    VDP_GetOutRegion((UCHAR)eVdpId, &ucOutFullRegion, (VDP_REGION_T*)&rOutRegion);

    USR_SPACE_ACCESS_VALIDATE_ARG(prOutRegion, MTVDO_REGION_T);
    COPY_TO_USER_ARG(prOutRegion, rOutRegion, MTVDO_REGION_T);

    MTVDO_PRINT(" - eVdpId = %d\n"
	 "rOutRegion.u4X = %d\n"
	 "rOutRegion.u4Y = %d\n"
        "rOutRegion.u4Width = %d\n"
        "rOutRegion.u4Height = %d\n", 
        eVdpId,
        (int)rOutRegion.u4X,
        (int)rOutRegion.u4Y,
        (int)rOutRegion.u4Width,
        (int)rOutRegion.u4Height);    

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetOutputWindow(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_REGION_T *prOutRegion;
    MTVDO_REGION_T rOutRegion;
    MTVDO_REGION_T rSrcRegion;
	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prOutRegion = (MTVDO_REGION_T*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(prOutRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prOutRegion, rOutRegion, MTVDO_REGION_T);

    rSrcRegion = rOutRegion;

    MTVDO_PRINT(" - eVdpId = %d\n"
    "rOutRegion.u4X = %d\n"
    "rOutRegion.u4Y = %d\n"
    "rOutRegion.u4Width = %d\n"
    "rOutRegion.u4Height = %d\n", 
    	eVdpId,
    	(int)rOutRegion.u4X,
    	(int)rOutRegion.u4Y,
    	(int)rOutRegion.u4Width,
    	(int)rOutRegion.u4Height);	

    if (VDP_GetScalerPixelBased())
    {
        if (bDrvVideoGetWidthOverSample(eVdpId) && (_u4OversampleDependent == 0))
        {
            rSrcRegion.u4X = rSrcRegion.u4X << 1;
            rSrcRegion.u4Width = rSrcRegion.u4Width << 1;            
        }
    }
    VDP_SetSrcRegion((UCHAR)eVdpId, (UCHAR)1, *(VDP_REGION_T*)&rSrcRegion);
    VDP_SetOutRegion((UCHAR)eVdpId, (UCHAR)0, *(VDP_REGION_T*)&rOutRegion);  

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetBg(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4BgColor;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4BgColor = (UINT32)rArg.ai4Arg[1];

    MTVDO_PRINT(" -eVdpId = %d, u4BgColor = 0x%u \n", eVdpId, u4BgColor);
    VDP_SetBg((UCHAR)eVdpId, u4BgColor);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetBg(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 *pu4BgColor;
    UINT32 u4BgColor;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu4BgColor = (UINT32*)rArg.ai4Arg[1];

    VDP_GetBg((UCHAR)eVdpId, &u4BgColor);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4BgColor, UINT32);
    COPY_TO_USER_ARG(pu4BgColor, u4BgColor, UINT32);

    MTVDO_PRINT(" -eVdpId = %d, u4BgColor = 0x%u\n", eVdpId, u4BgColor);
    return MTR_OK;
}

static void _VdoTDNaviChgCb(UINT32 u4VdpId, UINT32 u4NaviTag)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4NaviTag;
    
    _CB_PutEvent(CB_MTAL_VDO_TDNAVI_CHG, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoSrcChgCb(UINT32 u4VdpId, VDP_SOURCE_INFO_T rSrcInfo)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = rSrcInfo.u4SrcWidth;
    rVdoCbT.u4Arg3 = rSrcInfo.u4SrcHeight;
    rVdoCbT.u4Arg4 = rSrcInfo.u4OutWidth;
    rVdoCbT.u4Arg5 = rSrcInfo.u4OutHeight;
    rVdoCbT.u4Arg6 = rSrcInfo.u4FrameRate;
    rVdoCbT.u4Arg7 = rSrcInfo.u4Interlace;    
    
    _CB_PutEvent(CB_MTAL_VDO_SRCCHG_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoPlayDoneCb(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Arg1;
    rVdoCbT.u4Arg3 = u4Arg2;
    
    _CB_PutEvent(CB_MTAL_VDO_PLAYDONE_NFY, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoSrcChgMWCb(UINT32 u4VdpId, VDP_SOURCE_INFO_T rSrcInfo)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = rSrcInfo.u4SrcWidth;
    rVdoCbT.u4Arg3 = rSrcInfo.u4SrcHeight;
    rVdoCbT.u4Arg4 = rSrcInfo.u4OutWidth;
    rVdoCbT.u4Arg5 = rSrcInfo.u4OutHeight;
    rVdoCbT.u4Arg6 = rSrcInfo.u4FrameRate;
    rVdoCbT.u4Arg7 = rSrcInfo.u4Interlace;    
    
    _CB_PutEvent(CB_MTAL_VDO_SRCCHG_MW_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoPtsCb(UINT32 u4VdpId, UINT32 u4Pts, UINT32 u4Arg)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Pts;
    rVdoCbT.u4Arg3 = u4Arg;
    
    _CB_PutEvent(CB_MTAL_VDO_PTS_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoAfdCb(UINT32 u4VdpId, UINT32 u4Afd, FBM_ASPECT_RATIO_T* prAspectRatio)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Afd;
    rVdoCbT.u4Arg3 = prAspectRatio->u1AspectRatioType;

    _CB_PutEvent(CB_MTAL_VDO_AFD_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoUnMuteCb(UINT32 u4VdpId, BOOL fgUnMute)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = (UINT32)fgUnMute;

    _CB_PutEvent(CB_MTAL_VDO_UNMUTE_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoLipSyncCb(void)
{
    MTVDO_CB_T  rVdoCbT;
    _CB_PutEvent(CB_MTAL_VDO_LIPSYNC_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoLipSyncNg2OkCb(void)
{
    MTVDO_CB_T  rVdoCbT;
    _CB_PutEvent(CB_MTAL_VDO_LIPSYNC_NG_TO_OK_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoLipSyncOk2NgCb(void)
{
    MTVDO_CB_T  rVdoCbT;
    _CB_PutEvent(CB_MTAL_VDO_LIPSYNC_OK_TO_NG_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

void _VdoVSSCb(void)
{
    MTVDO_CB_T  rVdoCbT;

    _CB_PutEvent(CB_MTAL_VDO_VSS_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

void _VSS420Cb(void)
{
    MTVDO_CB_T  rVdoCbT;

    _CB_PutEvent(CB_MTAL_VSS420_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}
#if !defined(CC_MT5396) && !defined(CC_MT5368) && !defined(CC_MT5398) && !defined(CC_MT5880) && !defined(CC_MT5881) && !defined(CC_MT5399) && !defined(CC_MT5890) && !defined(CC_MT5882) 
static void _OutVSyncNfy(void)
{
    MTVDO_CB_T  rVdoCbT;
    
    OSD_Vsync_UpdateScroll();    
    _CB_PutEvent(CB_MTAL_VDO_OUT_VSYNC_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}
#endif
static void _VdoDispmodeCb(UINT32 u4VdpId, UINT32 u4Status)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Status;
    
    _CB_PutEvent(CB_MTAL_VDO_DISPMODE_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoBitRateChgCb(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Arg1;
    rVdoCbT.u4Arg3 = u4Arg2;
    
    _CB_PutEvent(CB_MTAL_VDO_BIT_RATE_CHG_NFY, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoResChgCb(UINT32 u4VdpId, UINT32 u4Arg1, UINT32 u4Arg2)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4Arg1;
    rVdoCbT.u4Arg3 = u4Arg2;
    
    _CB_PutEvent(CB_MTAL_VDO_RES_CHG_NFY, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoEosCb(UINT32 u4VdpId, VDP_COND_T eCond, BOOL fgFastForward,UINT32 u4AttachedSrcId)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = (UINT32)eCond;
    rVdoCbT.u4Arg3 = (UINT32)fgFastForward;
    rVdoCbT.u4Arg4 = u4AttachedSrcId;
    
    _CB_PutEvent(CB_MTAL_VDO_MM_COND_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}


static void _VdoStepFinCb(UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret, UINT32 u4AttachedId)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = u4PTS;
    rVdoCbT.u4Arg3 = u4Ret;
    rVdoCbT.u4Arg4 = u4AttachedId;
    
    _CB_PutEvent(CB_MTAL_VDO_MM_STEP_FIN_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoSeekFinCb(UINT32 u4SwdmxSrcId, UINT32 u4VdpId, UINT32 u4PTS, UINT32 u4Ret)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4SwdmxSrcId;
    rVdoCbT.u4Arg2 = u4VdpId;
    rVdoCbT.u4Arg3 = u4PTS;
    rVdoCbT.u4Arg4 = u4Ret;
    
    _CB_PutEvent(CB_MTAL_VDO_MM_SEEK_FIN_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}


static void _VdoSrcRangeChgCb(UINT32 u4VdpId, VDP_REGION_T rSrcInfo)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = rSrcInfo.u4X;
    rVdoCbT.u4Arg3 = rSrcInfo.u4Y;
    rVdoCbT.u4Arg4 = rSrcInfo.u4Width;
    rVdoCbT.u4Arg5 = rSrcInfo.u4Height;  
    
    _CB_PutEvent(CB_MTAL_VDO_SRC_RANGE_CHG_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}

static void _VdoOutRangeChgCb(UINT32 u4VdpId, VDP_REGION_T rOutInfo)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 = u4VdpId;
    rVdoCbT.u4Arg2 = rOutInfo.u4X;
    rVdoCbT.u4Arg3 = rOutInfo.u4Y;
    rVdoCbT.u4Arg4 = rOutInfo.u4Width;
    rVdoCbT.u4Arg5 = rOutInfo.u4Height;  


    
    _CB_PutEvent(CB_MTAL_VDO_OUT_RANGE_CHG_TRIGGER, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}
#if defined(CC_MT5890)
static void _Vdo3DModeChgCb(UINT32 u4VdpId, VDP_3D_MODE_CHG_INFO_T rMode)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 =rMode.u4SrcWidth;
	rVdoCbT.u4Arg2 =rMode.u4SrcHeight;
	rVdoCbT.u4Arg3 =rMode.u4InOutFrameRate;
	rVdoCbT.u4Arg4 =rMode.u1Output3DFormat;
	rVdoCbT.u4Arg5 =rMode.u1Input3DFormat;
	rVdoCbT.u4Arg6 =rMode.u1ColorFormat;
	rVdoCbT.u4Arg7 =rMode.u1ColorSpace;   
    _CB_PutEvent(CB_MTAL_VDO_3D_MODE_CHG, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}
static void _VdoAfter3DModeChgCb(UINT32 u4VdpId, VDP_3D_MODE_CHG_INFO_T rMode)
{
    MTVDO_CB_T  rVdoCbT;

    rVdoCbT.u4Arg1 =rMode.u4SrcWidth;
	rVdoCbT.u4Arg2 =rMode.u4SrcHeight;
	rVdoCbT.u4Arg3 =rMode.u4InOutFrameRate;
	rVdoCbT.u4Arg4 =rMode.u1Output3DFormat;
	rVdoCbT.u4Arg5 =rMode.u1Input3DFormat;
	rVdoCbT.u4Arg6 =rMode.u1ColorFormat;
	rVdoCbT.u4Arg7 =rMode.u1ColorSpace;   
    _CB_PutEvent(VDO_CB_FUNC_AFTER_3D_MODE_CHGED, sizeof(MTVDO_CB_T), (void*)&rVdoCbT);
}
#endif
static VDP_CB_FUNC_ENUM_T _MTVDO_CBEnum_to_VDPCBEnum(MTVDO_CB_FUNC_ENUM_T eMTVDOEnum)
{
    VDP_CB_FUNC_ENUM_T eReturn = VDP_CB_FUNC_NS;

    switch (eMTVDOEnum)
    {
        case VDO_CB_FUNC_PLAY_DONE_IND:
            eReturn = VDP_CB_FUNC_PLAY_DONE_IND;
            break;
            
        case VDO_CB_FUNC_SRC_CHG_IND:
            eReturn = VDP_CB_FUNC_SRC_CHG_IND_2;
            break;

        case VDO_CB_FUNC_SRC_CHG_MW_IND:
        case VDO_CB_FUNC_SRC_CHG_MW_IND_EX:
            eReturn = VDP_CB_FUNC_SRC_CHG_IND;
            break;

        case VDO_CB_FUNC_PTS_IND:
            eReturn = VDP_CB_FUNC_PTS_IND;
            break;

        case VDO_CB_FUNC_AFD_IND:
            eReturn = VDP_CB_FUNC_AFD_IND_2;
            break;

        case VDO_CB_FUNC_UNMUTE_IND:
            eReturn = VDP_CB_FUNC_UNMUTE_IND;
            break;

        case VDO_CB_FUNC_LIPSYNC_IND:
            eReturn = VDP_CB_FUNC_LIPSYNC_IND;
            break;
            
        case VDO_CB_FUNC_LIPSYNC_OK_TO_NG_IND:
            eReturn = VDP_CB_FUNC_LIPSYNC_OK_TO_NG_IND;
            break;
            
        case VDO_CB_FUNC_LIPSYNC_NG_TO_OK_IND:
            eReturn = VDP_CB_FUNC_LIPSYNC_NG_TO_OK_IND;
            break;
            
        case VDO_CB_FUNC_VSS_FIN_IND:
            eReturn = VDP_CB_FUNC_VSS_FIN_IND;
            break;
            
	   case VDO_CB_FUNC_VSS_420_IND:
		    eReturn = VDP_CB_FUNC_VSS_420_IND;
            break;
			
        case VDO_CB_FUNC_TDNAVI_IND:
            eReturn = VDP_CB_FUNC_TDNAVI_IND;
            break;

        case VDO_CB_FUNC_DISP_SUPPORT_IND:
            eReturn = VDP_CB_FUNC_DISP_SUPPORT_IND;
            break;

        case VDO_CB_FUNC_MM_COND_IND:
            eReturn = VDP_CB_FUNC_MM_COND_IND;
            break;

		case VDO_CB_FUNC_RES_CHG_ING:
			eReturn = VDP_CB_FUNC_RES_CHG_IND;
			break;

        case VDO_CB_FUNC_BIT_TATE_CHG_ING:
            eReturn = VDP_CB_FUNC_BIT_RATE_CHG_IND;
            break;

        case VDO_CB_FUNC_MM_STEP_FIN_IND:
            eReturn = VDP_CB_FUNC_MM_STEP_FIN_IND;
            break;
            
        case VDO_CB_FUNC_MM_SEEK_FIN_IND:
            eReturn = VDP_CB_FUNC_MM_SEEK_FIN_IND;
            break;


        case VDO_CB_FUNC_SRC_RANGE_CHG_IND:
            eReturn = VDP_CB_FUNC_DRIVER_SRC_RANGE_CHG_IND;
            break;
            
        case VDO_CB_FUNC_OUT_RANGE_CHG_IND:
            eReturn = VDP_CB_FUNC_DRIVER_OUT_RANGE_CHG_IND;
            break;  

#if defined(CC_MT5890)	
	    case VDO_CB_FUNC_3D_MODE_CHG:
            eReturn = VDP_CB_FUNC_3D_MODE_CHG_IND;
			break;
        case VDO_CB_FUNC_AFTER_3D_MODE_CHGED:
            eReturn = VDP_CB_FUNC_AFTER_3D_MODE_CHGED_IND;
            break;            
#endif            
        default:
            eReturn = VDP_CB_FUNC_NS;
            break;
    }

    return eReturn;
}

static MT_RESULT_T _MTVDO_RegCbFunc(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_CB_FUNC_ENUM_T eFuncType;
    UINT32 u4FuncPtr;
    UINT32 u4Arg1;
    UINT32 u4Arg2;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    eFuncType = (MTVDO_CB_FUNC_ENUM_T)rArg.ai4Arg[0];
    u4FuncPtr = (UINT32)rArg.ai4Arg[1];
    u4Arg1 = (UINT32)rArg.ai4Arg[2];
    u4Arg2 = (UINT32)rArg.ai4Arg[3];

    MTVDO_PRINT(" - eFuncType = %d, u4FuncPtr = %u, u4Arg1 = %u, u4Arg2 = %u\n",
        eFuncType, u4FuncPtr, u4Arg1, u4Arg2);

    switch (eFuncType)
    {
        case VDO_CB_FUNC_PLAY_DONE_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoPlayDoneCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_SRC_CHG_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoSrcChgCb, u4Arg1, u4Arg2);
            break;
        case VDO_CB_FUNC_SRC_CHG_MW_IND:
        case VDO_CB_FUNC_SRC_CHG_MW_IND_EX:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoSrcChgMWCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_PTS_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoPtsCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_AFD_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoAfdCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_UNMUTE_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoUnMuteCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_LIPSYNC_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoLipSyncCb, u4Arg1, u4Arg2);
            break;
            
        case VDO_CB_FUNC_LIPSYNC_OK_TO_NG_IND:
            if((u4Arg1==0) && (u4Arg2==0))
            {
                /* Remove register callback */
                VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)NULL, u4Arg1, u4Arg2);
            }
            else
            {
                VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoLipSyncOk2NgCb, u4Arg1, u4Arg2);
            }
            break;
            
        case VDO_CB_FUNC_LIPSYNC_NG_TO_OK_IND:
            if((u4Arg1==0) && (u4Arg2==0))
            {
                /* Remove register callback */
                VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)NULL, u4Arg1, u4Arg2);
            }
            else
            {
                VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoLipSyncNg2OkCb, u4Arg1, u4Arg2);
            }
            break;

        case VDO_CB_FUNC_OUT_VSYNC_IND:
              #if defined(CC_MT5392B) || defined(CC_MT5365) || defined(CC_MT5395)
        	    NPTV_SetOutVSyncNfyFct((UINT32)_OutVSyncNfy);
              #else
              return MTR_NOT_OK;
              #endif
        	break;

        case VDO_CB_FUNC_VSS_FIN_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoVSSCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_VSS_420_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VSS420Cb, u4Arg1, u4Arg2);
            break;


        case VDO_CB_FUNC_TDNAVI_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoTDNaviChgCb, u4Arg1, u4Arg2);
            break;


        case VDO_CB_FUNC_DISP_SUPPORT_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoDispmodeCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_MM_COND_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoEosCb, u4Arg1, u4Arg2);
            break;

		case VDO_CB_FUNC_RES_CHG_ING:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoResChgCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_BIT_TATE_CHG_ING:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoBitRateChgCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_MM_STEP_FIN_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoStepFinCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_MM_SEEK_FIN_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoSeekFinCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_SRC_RANGE_CHG_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoSrcRangeChgCb, u4Arg1, u4Arg2);
            break;

        case VDO_CB_FUNC_OUT_RANGE_CHG_IND:
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoOutRangeChgCb, u4Arg1, u4Arg2);
            break;                        
#if defined(CC_MT5890)
	    case VDO_CB_FUNC_3D_MODE_CHG:			
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_Vdo3DModeChgCb, u4Arg1, u4Arg2);
			break;
        case VDO_CB_FUNC_AFTER_3D_MODE_CHGED:			
            VDP_RegCbFunc(_MTVDO_CBEnum_to_VDPCBEnum(eFuncType), (UINT32)_VdoAfter3DModeChgCb, u4Arg1, u4Arg2);
#endif            
            
        default:
            return MTR_NOT_OK;
            break;
    }
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetGameMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4VdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4VdpId = (UINT32)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %u, fgEnable = %d\n", u4VdpId, fgEnable);
    VDP_SetGameMode((UINT32)u4VdpId, (UINT8)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_Set3DGameMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4VdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4VdpId = (UINT32)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %u, fgEnable = %d\n", u4VdpId, fgEnable);
    VDP_Set3DGameMode((UINT32)u4VdpId, (UINT8)fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetUnMuteStatus (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL *pfgEnable;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pfgEnable = (BOOL*)rArg.ai4Arg[1];

    fgEnable = VDP_GetUnMuteStatus((UCHAR)eVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(pfgEnable, BOOL);
    COPY_TO_USER_ARG(pfgEnable, fgEnable, BOOL);

    MTVDO_PRINT(" -eVdpId = %d, fgEnable = %d\n", eVdpId, fgEnable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetVideoMuteStatus(UCHAR ucVdpId, UINT32 * prData, UINT32 u4Size)
{
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR; 
    }

    *prData = _u4DrvVideoGetMute(ucVdpId);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_Scalar_EnableGetBuffer(unsigned long arg)
{
    BOOL fgEnable;
    UINT8 u1Ret;
    fgEnable = (BOOL)arg;
    
    MTVDO_PRINT(" - fgEnable = %d\n", fgEnable);

#ifdef CC_MT5392B
    vSupportScalerDramDump(VDP_1, fgEnable);
    return MTR_OK;
#elif defined(CC_MT5395) || defined(CC_MT5365)

    if(fgForceScalerDramDump(VDP_1, fgEnable))
    {
        u1Ret = MTR_OK;
    }
    else
    {
        u1Ret = MTR_NOT_SUPPORTED;
    }

    MTVDO_PRINT(" - u1Ret = %d\n", u1Ret);
#else
    u1Ret = MTR_NOT_SUPPORTED;
#endif
    return u1Ret;    
}

static MT_RESULT_T _MTVDO_Scalar_GetBufferAverage(unsigned long arg)
{
    UINT8 u1Ret;
    #if defined(CC_MT5392B) || defined(CC_MT5395) || defined(CC_MT5365)
    MTVDO_REGION_T rRegion;
    MTVDO_REGION_T *prRegion;
    MTVDO_SCLAR_BUFFER_DATA rData;
    MTVDO_SCLAR_BUFFER_DATA *prData;

    MTAL_IOCTL_2ARG_T    rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    prRegion= (MTVDO_REGION_T*) rArg.ai4Arg[0];
    prData= (MTVDO_SCLAR_BUFFER_DATA*) rArg.ai4Arg[1];
    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);

    //add driver implementation code here
    UNUSED(bInitScalerDump());       
    u1Ret = bGetScalerDRAMAverage(rRegion.u4X, rRegion.u4Y, rRegion.u4Width, rRegion.u4Height,  
    									&(rData.u4VR),  &(rData.u4UB),  &(rData.u4YG));
    UNUSED(bReleaseScalerDump());

    USR_SPACE_ACCESS_VALIDATE_ARG(prData, MTVDO_SCLAR_BUFFER_DATA);
    COPY_TO_USER_ARG(prData, rData, MTVDO_SCLAR_BUFFER_DATA);

    MTVDO_PRINT(" - u2VR = 0x%u ,u2UB = 0x%u, u2YG = 0x%u \n", rData.u4VR, rData.u4UB, rData.u4YG);
    MTVDO_PRINT(" - u1Ret = %d\n", u1Ret);

    #else
    u1Ret = FALSE;
    #endif

    if(u1Ret == FALSE)
    	return MTR_NOT_OK;
    else
        return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetTimingInfoByDec(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_DEC_TYPE_T eDecType;

    MTVDO_TIMING_INFO_T *prTiming;
    MTVDO_TIMING_INFO_T rTiming;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eDecType = (MTVDO_DEC_TYPE_T)rArg.ai4Arg[0];
    prTiming = (MTVDO_TIMING_INFO_T*)rArg.ai4Arg[1];

    if (bDrvVideoSignalStatusByDec(eDecType) == SV_VDO_NOSIGNAL)
    {
        rTiming.u4ActiveHeight = 0;
        rTiming.u4ActiveWidth = 0;
        rTiming.u4FrameRate = 0;
        rTiming.u4HStart = 0;
        rTiming.u4HTotal = 0;
        rTiming.u4Interlace = 0;
        rTiming.u4Phase = 0;
        rTiming.u4VStart = 0;
        rTiming.u4VTotal = 0;
        rTiming.u4HDefPorch = 0;
        rTiming.u4VDefPorch = 0;
        rTiming.u4SignalStatus = 0;
    }
    else
    {
        // get timing info

        rTiming.u4HDefPorch = (UINT32)wDrvVideoGetPorchByDec(eDecType, SV_HPORCH_DEFAULT);
        rTiming.u4VDefPorch = (UINT32)wDrvVideoGetPorchByDec(eDecType, SV_VPORCH_DEFAULT);
        rTiming.u4SignalStatus = bDrvVideoSignalStatusByDec(eDecType);
        
        rTiming.u4HTotal = (UINT32)wDrvVideoGetHTotalByDec(eDecType);
        rTiming.u4VTotal = (UINT32)wDrvVideoGetVTotalByDec(eDecType);
        rTiming.u4FrameRate = bDrvVideoGetRefreshRateByDec(eDecType);

        rTiming.u4HStart = (UINT32)wDrvVideoGetPorchByDec(eDecType, SV_HPORCH_CURRENT);
        rTiming.u4VStart = (UINT32)wDrvVideoGetPorchByDec(eDecType, SV_VPORCH_CURRENT);

        rTiming.u4ActiveWidth = (UINT32)wDrvVideoInputWidthByDec(eDecType);

        if(bDrvVideoGetWidthOverSampleByDec(eDecType) && (_u4OversampleDependent == 0))
        {
            rTiming.u4ActiveWidth = rTiming.u4ActiveWidth >> 1;
            rTiming.u4HStart= rTiming.u4HStart >> 1;//cheerz        
        }
        
        rTiming.u4ActiveHeight = (UINT32)wDrvVideoInputHeightByDec(eDecType); 
        rTiming.u4Interlace = (UINT32)bDrvVideoIsSrcInterlaceByDec(eDecType);
        if(rTiming.u4Interlace)//cheerz
        {
            rTiming.u4VStart = rTiming.u4VStart << 1;
        }
        
        if (eDecType == MTVDO_DEC_VGA)
        {
            rTiming.u4Phase = u1DrvVGAGetPhase();
        }
        else
        {
            rTiming.u4Phase = 0; 
        }
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(prTiming, MTVDO_TIMING_INFO_T);
    COPY_TO_USER_ARG(prTiming, rTiming, MTVDO_TIMING_INFO_T);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetTimingInfo(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;

    MTVDO_TIMING_INFO_T *prTiming;
    MTVDO_TIMING_INFO_T rTiming;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prTiming = (MTVDO_TIMING_INFO_T*)rArg.ai4Arg[1];

    if (bDrvVideoSignalStatus(eVdpId) == SV_VDO_NOSIGNAL)
    {
        rTiming.u4ActiveHeight = 0;
        rTiming.u4ActiveWidth = 0;
        rTiming.u4FrameRate = 0;
        rTiming.u4HStart = 0;
        rTiming.u4HTotal = 0;
        rTiming.u4Interlace = 0;
        rTiming.u4Phase = 0;
        rTiming.u4VStart = 0;
        rTiming.u4VTotal = 0;
    }
    else
    {
        // get timing info
        rTiming.u4HTotal = (UINT32)wDrvVideoGetHTotal(eVdpId);
        rTiming.u4VTotal = (UINT32)wDrvVideoGetVTotal(eVdpId);
        rTiming.u4FrameRate = bDrvVideoGetRefreshRate(eVdpId);
    #ifdef CC_MT5392B
        rTiming.u4HStart = (UINT32)wTVDecGetHPos(eVdpId);
        rTiming.u4VStart = (UINT32)wTVDecGetVPos(eVdpId);
    #else
        rTiming.u4HStart = (UINT32)wDrvVideoGetPorch(eVdpId, SV_HPORCH_CURRENT);
        rTiming.u4VStart = (UINT32)wDrvVideoGetPorch(eVdpId, SV_VPORCH_CURRENT);
    #endif
        rTiming.u4ActiveWidth = (UINT32)wDrvVideoInputWidth(eVdpId);

        #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
        if ((bDrvVideoGetWidthOverSample(eVdpId) && (_u4OversampleDependent == 0)) ||
              ((eVdpId == VDP_1) && u4DrvTDTVIsDecoderContentFull())  
            #if defined(CC_MT5890)  
            || ( IS_IC_5861()&&bDrvVideoIsDecDualMode() )
            #endif
            )

        {
            rTiming.u4ActiveWidth = rTiming.u4ActiveWidth >> 1;
            rTiming.u4HStart= rTiming.u4HStart >> 1;//cheerz        
        }
        #endif
        
        rTiming.u4ActiveHeight = (UINT32)wDrvVideoInputHeight(eVdpId); 
        rTiming.u4Interlace = (UINT32)bDrvVideoIsSrcInterlace(eVdpId);
        if(rTiming.u4Interlace)//cheerz
        {
            rTiming.u4VStart = rTiming.u4VStart << 1;
        }
        
        if (bGetVideoDecType((UINT8)eVdpId) == SV_VD_VGA || bGetVideoDecType((UINT8)eVdpId)==SV_VD_YPBPR)
        {
            rTiming.u4Phase = u1DrvVGAGetPhase();
        }
        else
        {
            rTiming.u4Phase = 0; 
        }
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(prTiming, MTVDO_TIMING_INFO_T);
    COPY_TO_USER_ARG(prTiming, rTiming, MTVDO_TIMING_INFO_T);

    return MTR_OK;
}

static VOID _MTVDO_GetGreatestCommonDivisor(UINT32 * pu4ParWidth, UINT32 * pu4ParHeight)
{
    UINT32 u4ParWidth = *pu4ParWidth;
    UINT32 u4ParHeight = *pu4ParHeight;
    UINT32 u4Mod = 0;
    UINT32 u4Tmp = 0;
    if(u4ParWidth < u4ParHeight)
    {
        u4Tmp = u4ParHeight;
        u4ParHeight = u4ParWidth;
        u4ParWidth = u4Tmp;
    }
    if(u4ParHeight == 0) 
    {
        *pu4ParWidth = 1;
        *pu4ParHeight = 1;
        return;
    }
    u4Mod = u4ParWidth%u4ParHeight;
    while(u4Mod)
    {
        u4ParWidth = u4ParHeight;
        u4ParHeight = u4Mod;
        u4Mod = u4ParWidth%u4ParHeight;
    }
    *pu4ParWidth = (*pu4ParWidth)/u4ParHeight;
    *pu4ParHeight = (* pu4ParHeight)/u4ParHeight;
}

EXTERN UCHAR VDP_FrcGetLockFrameBuffer(UCHAR ucVdpId);
static MT_RESULT_T _MTVDO_GetAspectRatio(unsigned long arg)
{
    UINT8 u1Ret;
    INT32 i4Ret;
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_ASPECTRATIO_T *prAspectRatio;
    MTVDO_ASPECTRATIO_T rAspectRatio;
	UINT32 u4ParWidth;
    UINT32 u4ParHeight;

    UCHAR ucFbgId, ucFbId;
    FBM_PIC_HDR_T* prFbmPicHdr = NULL;
    B2R_ASP_INFO_T t_asp_info;
    
	FBM_SEQ_HDR_T * prFbmSeqHdr ;
	UINT32 u4PicWidth, u4PicHeight;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prAspectRatio = (MTVDO_ASPECTRATIO_T*)rArg.ai4Arg[1];

    ucFbgId = VDP_GetFbg(eVdpId);
    ucFbId = VDP_FrcGetLockFrameBuffer(eVdpId);
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
	prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    x_memset(&t_asp_info, 0x0, sizeof(B2R_ASP_INFO_T));
    i4Ret = VDP_B2R_GetInfo(eVdpId,
                            B2R_GET_TYPE_ASP_INFO,
                            &t_asp_info,
                            sizeof(B2R_ASP_INFO_T));

    if ((B2R_OK == i4Ret) && (prFbmPicHdr != NULL))
    {
        rAspectRatio.u4IdcInfo = t_asp_info.u4AspectRatioIdc;
        rAspectRatio.u4Afd = t_asp_info.u4Afd;
		
        
        if (prFbmSeqHdr && FBM_IsSeamlessFB(ucFbgId, ucFbId))
        {
            u4PicWidth  = prFbmSeqHdr->u4ResizeWidth; 
            u4PicHeight = prFbmSeqHdr->u4ResizeHeight; 
        }
        else
        {
            u4PicWidth  = prFbmPicHdr->u4PicWidth;
            u4PicHeight = prFbmPicHdr->u4PicHeight;
        }
		
        if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_4_3)
	    {
            u4ParWidth = prFbmPicHdr->u4PicHeight * 4;
            u4ParHeight = prFbmPicHdr->u4PicWidth * 3;
            _MTVDO_GetGreatestCommonDivisor(&u4ParWidth, &u4ParHeight);
            rAspectRatio.u4ParWidth = u4ParWidth;
            rAspectRatio.u4ParHeight = u4ParHeight;
            rAspectRatio.u4DarWidth = 4;
            rAspectRatio.u4DarHeight = 3;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_16_9)
        {
            u4ParWidth = prFbmPicHdr->u4PicHeight * 16;
            u4ParHeight = prFbmPicHdr->u4PicWidth * 9;
            _MTVDO_GetGreatestCommonDivisor(&u4ParWidth, &u4ParHeight);
            rAspectRatio.u4ParWidth = u4ParWidth;
            rAspectRatio.u4ParHeight = u4ParHeight;
            rAspectRatio.u4DarWidth = 16;
            rAspectRatio.u4DarHeight = 9;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_221_1)
        {
            u4ParWidth = prFbmPicHdr->u4PicHeight * 221;
            u4ParHeight = prFbmPicHdr->u4PicWidth * 100;
            _MTVDO_GetGreatestCommonDivisor(&u4ParWidth, &u4ParHeight);
            rAspectRatio.u4ParWidth = u4ParWidth;
            rAspectRatio.u4ParHeight = u4ParHeight;
            rAspectRatio.u4DarWidth = 221;
            rAspectRatio.u4DarHeight = 100;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_10_11)
        {
            rAspectRatio.u4ParWidth = 10;
            rAspectRatio.u4ParHeight = 11;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_40_33)
        {
            rAspectRatio.u4ParWidth = 40;
            rAspectRatio.u4ParHeight = 33;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
    	}
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_16_11)
        {
            rAspectRatio.u4ParWidth = 16;
            rAspectRatio.u4ParHeight = 11;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
        }
    	else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_12_11)
        {
            rAspectRatio.u4ParWidth = 12;
            rAspectRatio.u4ParHeight = 11;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_3_2)
        {
            rAspectRatio.u4ParWidth = 3;
            rAspectRatio.u4ParHeight = 2;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_1_1)
        {
            rAspectRatio.u4ParWidth = 1;
            rAspectRatio.u4ParHeight = 1;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight * u4PicHeight;
        }
        else if(t_asp_info.u4AspectRatio == MPEG_ASPECT_RATIO_TRANSMIT)
        {
            rAspectRatio.u4ParWidth = t_asp_info.u4AspectRatioWidth;
            rAspectRatio.u4ParHeight = t_asp_info.u4AspectRatioHeight;
            rAspectRatio.u4DarWidth = t_asp_info.u4AspectRatioWidth * u4PicWidth;
            rAspectRatio.u4DarHeight = t_asp_info.u4AspectRatioHeight * u4PicHeight;
        }
        else
        {
            rAspectRatio.u4ParWidth = 1;
            rAspectRatio.u4ParHeight = 1;
            rAspectRatio.u4DarWidth = rAspectRatio.u4ParWidth*u4PicWidth;
            rAspectRatio.u4DarHeight = rAspectRatio.u4ParHeight*u4PicHeight;
    	}

        u1Ret = MTR_OK;
    }
    else if(B2R_OK == i4Ret)  //No Video & No Audio, Only OSD for DTV(MPEG5)
    {
        rAspectRatio.u4Afd = t_asp_info.u4Afd;
        u1Ret = MTR_OK;
    }
    else
    {
        u1Ret = MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(prAspectRatio, MTVDO_ASPECTRATIO_T);
    COPY_TO_USER_ARG(prAspectRatio, rAspectRatio, MTVDO_ASPECTRATIO_T);

    return u1Ret;
}

static MT_RESULT_T _MTVDO_SetFactoryMode(unsigned long arg)
{
    BOOL fgEnable;

    fgEnable = (BOOL)arg;
    
    MTVDO_PRINT(" - fgEnable = %d\n", fgEnable);
#ifdef CC_SUPPORT_TVE_OSD_OUT
    vApiTVESetOSDOutMode((UINT8)fgEnable);
#endif    

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetMuteModuleInfo(unsigned long  arg)
{

	MTAL_IOCTL_3ARG_T rArg;
	BOOL fgEnable; //arg2
	MTVDO_MUTE_FUNC_T eMuteFuncType; //arg1
	MTVDO_MUTE_MODULE_INFO *pMTVDOMuteInfo; //arg0
	
	VIDEO_MUTE_INFO rMuteInfo;
	VIDEO_MUTE_CMD prDrvMuteCmd;
	memset(&prDrvMuteCmd,0,sizeof(VIDEO_MUTE_CMD));
	
	USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
	COPY_FROM_USER_3ARG(arg, rArg);
	
	pMTVDOMuteInfo = (MTVDO_MUTE_MODULE_INFO* )rArg.ai4Arg[0];
	eMuteFuncType = (MTVDO_MUTE_FUNC_T)rArg.ai4Arg[1];
	fgEnable = (UINT8)rArg.ai4Arg[2];

	switch (eMuteFuncType)
	{
		case MTVDO_MUTE_MODULE_INFO_TYPE:
			if(pMTVDOMuteInfo==NULL)
			{
				return MTR_NOT_OK;
			}
			_vDrvVideoGetMuteStatus(&prDrvMuteCmd);
			rMuteInfo=prDrvMuteCmd.rMuteInfo;
			pMTVDOMuteInfo->u4InvalidMask=rMuteInfo.u4InvalidMask;
			pMTVDOMuteInfo->u4Delay=rMuteInfo.u4Delay;
			break;
		case MTVDO_AUTO_MUTE_TYPE:
			fgEnable=bIsAutoMute;
			break;
		default:
			return MTR_ERR_INV;
	}
	return MTR_OK;
}


static MT_RESULT_T _MTVDO_SetAutoMuteOnOff(unsigned long  arg)
{
    BOOL fgOnOff;
    fgOnOff = ((BOOL)arg)? FALSE: TRUE;

#ifdef CC_MT5392B
#ifdef CC_SCPOS_FORCE_UNMUTE
    vScposForceUnMute(fgOnOff);
#else
    UNUSED(fgOnOff);
#endif
#else
    _vDrvVideoForceUnMute(VDP_1, fgOnOff);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_API_FORCE, VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_MJC,       VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_FRC,       VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_COLORMODE, VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_GAMEMODE,  VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    
    _vDrvVideoForceUnMute(VDP_2, fgOnOff);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_API_FORCE, VDP_2, 0, MUTE_INVALID_CLI_FORCE);    
#endif
    return MTR_OK;
}

#ifdef CC_FOR_POC_DEMO
static MT_RESULT_T _MTVDO_SetAutoBlkOnOff(unsigned long  arg)
{
    BOOL fgOnOff;
    fgOnOff = ((BOOL)arg)? FALSE: TRUE;

#ifdef CC_MT5392B
	UNUSED(fgOnOff);
#else
    _vDrvLVDSForceUnMute(VDP_1, fgOnOff);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_API_FORCE, VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_MJC,       VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_FRC,       VDP_1, 0, MUTE_INVALID_CLI_FORCE);
    _vDrvLVDSForceUnMute(VDP_2, fgOnOff);
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_API_FORCE, VDP_2, 0, MUTE_INVALID_CLI_FORCE);    
#endif
    return MTR_OK;
}
#else
static MT_RESULT_T _MTVDO_SetAutoBlkOnOff(unsigned long  arg)
{
    return MTR_OK;
}
#endif
static MT_RESULT_T _MTVDO_SetScalerPixelBased(unsigned long arg) // drmyung 081226
{
    BOOL fgEnable;

    fgEnable = (BOOL)arg;
    VDP_SetScalerPixelBased(fgEnable);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetxvYCCInnerPatGen(unsigned long arg)
{
#if defined(CC_MT5396) ||defined(CC_MT5389) || defined(CC_MT5368) || defined(CC_MT5398) || defined(CC_MT5880)||defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890)|| defined(CC_MT5882)
		// xvYCC Pattern Gen Removed in 96 or Later
#else
    MTAL_IOCTL_4ARG_T rArg;
    UINT8 bOnOff;
    UINT16 wPatGenR;
    UINT16 wPatGenG;
    UINT16 wPatGenB;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    bOnOff = (UINT8)rArg.ai4Arg[0];
    wPatGenR = (UINT16)rArg.ai4Arg[1];
    wPatGenG = (UINT16)rArg.ai4Arg[2];
    wPatGenB = (UINT16)rArg.ai4Arg[3];

    vDrvSetxvYCCPatGenRGB(wPatGenR, wPatGenG, wPatGenB);
    vDrvSetxvYCCPatGenOnOff(bOnOff);    
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetPeUiRangeParam(unsigned long arg)
{    
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_UI_RANGE_T  rUIParam;	
    UINT8 u1VdpVisType;	
    PE_ARG_TYPE eUiVqItem;    
    MTVDO_UI_RANGE_T* prUIParam;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    u1VdpVisType = (UINT8) rArg.ai4Arg[0];
    eUiVqItem = (PE_ARG_TYPE) rArg.ai4Arg[1];
    prUIParam = (MTVDO_UI_RANGE_T*) rArg.ai4Arg[2];

    COPY_FROM_USER_DATA(prUIParam, &rUIParam, sizeof(MTVDO_UI_RANGE_T));
    PE_SetUiRange(u1VdpVisType, eUiVqItem, (PE_UI_RANGE_T *)&rUIParam);	

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetPeUiRangeParam(unsigned long arg)
{    
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_UI_RANGE_T  rUIParam;        
    MTVDO_UI_RANGE_T* prUIParam;
    PE_ARG_TYPE eUiVqItem;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);    
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    eUiVqItem = (PE_ARG_TYPE) rArg.ai4Arg[1];
    prUIParam = (MTVDO_UI_RANGE_T*) rArg.ai4Arg[2];
    
    PE_GetUiRange(eVdpId, eUiVqItem, (PE_UI_RANGE_T *)&rUIParam);	        
    COPY_TO_USER_DATA(prUIParam, &rUIParam, sizeof(MTVDO_UI_RANGE_T));

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetScanMode(unsigned long arg)
{
    UINT32 u4scan;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT32);
    COPY_FROM_USER_ARG(arg,u4scan,UINT32); 

    vSetAutoSearchType(u4scan);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetDSFactor(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4Factor;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4Factor = (UINT32)rArg.ai4Arg[1];

    MTVDO_PRINT("  - eVdpId = %d, u4Factor = %d\n", (int)eVdpId, (int)u4Factor);

    vScpipSetForcedHpsFactor((UINT32)eVdpId, u4Factor);  
    
    return MTR_OK;
}

#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)|| defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) ||defined(CC_MT5882)
static MT_RESULT_T _MTVDO_SetVerticalDSFactor(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4Factor;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4Factor = (UINT32)rArg.ai4Arg[1];

    MTVDO_PRINT("  - eVdpId = %d, u4Factor = %d\n", (int)eVdpId, (int)u4Factor);

    vScpipSetForcedVpsFactor((UINT32)eVdpId, u4Factor);  
    
    return MTR_OK;
}
#endif

static MT_RESULT_T _MTVDO_SetIMITFlag(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4Factor;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4Factor = (UINT32)rArg.ai4Arg[1];

    MTVDO_PRINT("  - eVdpId = %d, u4Factor = %d\n", (int)eVdpId, (int)u4Factor);

    
    return eRet;
}
static MT_RESULT_T _MTVDO_SetPQ(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_PQ_TYPE_T ePqType;
    void* pkvPqSetInfo = NULL;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;
        
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    ePqType = (MTVDO_PQ_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvPqSetInfo = kmalloc(u4Size, GFP_KERNEL);
    
    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvPqSetInfo, u4Size);

    eRet = (MT_RESULT_T) PE_SetPqInfo((UINT8)eVdpId, (UINT32)ePqType, pkvPqSetInfo, u4Size);

    kfree(pkvPqSetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDO_GetPQ(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_PQ_TYPE_T ePqType;
    void* pkvPqGetInfo;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    ePqType = (MTVDO_PQ_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvPqGetInfo = kmalloc(u4Size, GFP_KERNEL);
    
    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvPqGetInfo, u4Size);

    eRet = PE_GetPqInfo(eVdpId, ePqType, pkvPqGetInfo, u4Size);

    COPY_TO_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvPqGetInfo, u4Size);    

    kfree(pkvPqGetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDO_GET_3D_TAG(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 bPath;
    UINT32 dw3DTag;
    // 1. take pointer from user layper
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    // 2. pass pointer from user layer to kernel layer
    
    // 3. do the driver function
    bPath = rArg.ai4Arg[0];       
    dw3DTag = u4DrvTDTVGetTag3D(bPath);
    
    // 4. pass pointer from kernel layer to user layer
    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],UINT32);    
    COPY_TO_USER_ARG(rArg.ai4Arg[1], dw3DTag, UINT32);    
    
    return MTR_OK;
} 

static MT_RESULT_T _MTVDO_Set3DPanel(unsigned long arg)
{
    UINT8 type;
    UINT32 u4CtlWord;
    
    type = (UINT8)arg;
    u4CtlWord = PANEL_GetControlWord() & 0xFBF3FFFF;
    if (type == 0) // LRLR
    {
        u4CtlWord = u4CtlWord | LVDS_DISP_3D_POLARIZED_LRLR;
    }
    else if (type == 1) // RLRL
    {
        u4CtlWord = u4CtlWord | LVDS_DISP_3D_POLARIZED_RLRL;
    }
    else // shutter for PDP or Active Glass
    {
        u4CtlWord = u4CtlWord | LVDS_DISP_3D_SHUTTER;
        #if defined(CC_MT5398) || defined(CC_MT5880)|| defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
        u4CtlWord = u4CtlWord | PANEL_L12R12;
        #else
        u4CtlWord = u4CtlWord | PANEL_PDP;
        #endif
    }
    
    PANEL_SetControlWord(u4CtlWord);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GET_3D_NAVI_DETECT(unsigned long arg)
{
#if defined(CC_MT5395) && defined(CC_MT5365)
    return MTR_OK;
#else
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 bPath;
    UINT32 dw3DTag;
    // 1. take pointer from user layper
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    // 2. pass pointer from user layer to kernel layer
    
    // 3. do the driver function
    bPath = rArg.ai4Arg[0];       
    dw3DTag = u4DrvTDTVGetNaviTag();
    
    // 4. pass pointer from kernel layer to user layer
    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],UINT32);    
    COPY_TO_USER_ARG(rArg.ai4Arg[1], dw3DTag, UINT32);    
    return MTR_OK;
#endif        
}

static MT_RESULT_T _MTVDO_Set3DNaviMode(unsigned long arg)
{
    E_TDTV_UI_NAVI eNavi;

    if(E_TDTV_UI_NAVI_FULL_AUTO==arg)
    {
        eNavi=E_TDTV_UI_NAVI_FULL_AUTO;
    }
    else if (E_TDTV_UI_NAVI_SEMI_AUTO==arg)
    {
        eNavi=E_TDTV_UI_NAVI_SEMI_AUTO;
    }
    else
    {
        eNavi=E_TDTV_UI_NAVI_MANAUL;
    }

    vDrvTDTVTDNAVI(eNavi);
    return MTR_OK;
}


static MT_RESULT_T _MTVDO_Set3DDCOnOff(unsigned long arg)
{
    UINT8 u1OnOff;

    u1OnOff = (arg > 0) ? SV_ON : SV_OFF;
    vDrvTDTVTDDC(u1OnOff);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetTDTV3DAutoDisplay(unsigned long arg)
{
    UINT32 fgOnOff;

    fgOnOff = (UINT32)arg;
    
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    vDrvTDTVSet3DAutoDislay(fgOnOff);
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_Set3DFramePacking(unsigned long arg)
{
    BOOL fgOnOff;
    fgOnOff = (BOOL)arg;
    
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    MTVDO_PRINT(" - fgOnOff = %d\n", fgOnOff);
    vDrvTDTVPacking(fgOnOff ? 3 : 1); // On: FP mode; Off: Auto mode
    #endif
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetSRAMDramMode(unsigned long arg)
{
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    BOOL fgOnOff;
    MTVDO_PATH_T eVdpId;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgOnOff = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT("  - eVdpId = %d, fgOnOff = %d\n", (int)eVdpId, (int)fgOnOff);

    if (fgOnOff) // force dram mode
    {
        SRM_CliSetDispMode(eVdpId, 0);
        SRM_TriggerCliSrm(eVdpId);
    }
    else // roll back
    {
        SRM_ClearCliSetting();
        SRM_TriggerCliSrm(eVdpId);
    }
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetFlipMirror(unsigned long arg)
{
    BOOL fgMirrorEn, fgFlipEn;
    MTAL_IOCTL_2ARG_T rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    fgMirrorEn = (BOOL)rArg.ai4Arg[0];
    fgFlipEn = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT("  - fgMirrorEn = %d, fgFlipEn = %d\n", (int)fgMirrorEn, (int)fgFlipEn);

    #if defined( CC_MT5396) || defined ( CC_MT5398) || defined (CC_MT5880) || defined (CC_MT5860) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
          return u1SetFlipMirrorConfig(fgMirrorEn, fgFlipEn);
    #else
        return MTR_OK;
    #endif
}

static MT_RESULT_T _MTVDO_SRM_SetTvMode(unsigned long arg)
{
    INT32 u4mode;

    USR_SPACE_ACCESS_VALIDATE_ARG(arg,INT32);
    COPY_FROM_USER_ARG(arg,u4mode,INT32);
    SRM_SetTvMode((SRM_TV_MODE_T)u4mode);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetPlaneWH(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 width;
    UINT32 height;
    UINT32 *pWidth;
    UINT32 *pHeight;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    pWidth = (UINT32 *)rArg.ai4Arg[0];
    pHeight = (UINT32*)rArg.ai4Arg[1];

    width = PANEL_GetPanelWidth();
    height = PANEL_GetPanelHeight();
    USR_SPACE_ACCESS_VALIDATE_ARG(pWidth, UINT32);
    COPY_TO_USER_ARG(pWidth, width, UINT32);
    USR_SPACE_ACCESS_VALIDATE_ARG(pHeight, UINT32);
    COPY_TO_USER_ARG(pHeight, height, UINT32);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCInitialize(unsigned long arg)
{
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    vDrvMJCInit(FALSE);
    if (PANEL_IsSupport120Hz() == FALSE)
    {
        bSupportDejudder = TRUE;
    }
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCResetSW(unsigned long arg)
{
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    vDrvMJCResetSW();
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetMJCFWVer(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 *pMajorVersion;
    UINT8 *pSubVersion;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pMajorVersion = (UINT8 *)rArg.ai4Arg[0];
    pSubVersion = (UINT8 *)rArg.ai4Arg[1];

    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    //MJC_GetFWVer(pMajorVersion, pSubVersion);
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCVideoBlock(unsigned long arg)
{
    BOOL fgOnOff = SV_OFF;
    fgOnOff = (BOOL)arg;
   // USR_SPACE_ACCESS_VALIDATE_ARG(arg, BOOL);
   // COPY_TO_USER_ARG(arg, fgOnOff, BOOL);

    #if defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    if (fgOnOff)
    {
        _vDrvVideoSetMute(MUTE_MODULE_FRC, VDP_1, FOREVER_MUTE, TRUE);
    }
    else
    {
        _vDrvVideoSetMute(MUTE_MODULE_FRC, VDP_1, 1, FALSE);
    }
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCEffectLevel(unsigned long arg)
{
    UINT8 bLevel;
    bLevel = (UINT8) arg;

    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetEffect(bLevel);
    //MJC_UpdateRealCinemaMode(bLevel, 0xFFFF, 0xFFFF);
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCJudderLevel(unsigned long arg)
{
    UINT8 bLevel;
    bLevel = (UINT8) arg;
    
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetDejudderLvl(bLevel);
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCBlurLevel(unsigned long arg)
{
    UINT8 bLevel;
    bLevel = (UINT8) arg;
    
    printf("_MTVDO_SetMJCBlurLevel: %d\n", bLevel);
    
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetDeblurLvl(bLevel);
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCDemoMode(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    BOOL fgOnOff;
    UINT8 bMode;
    UINT16 wCenter;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    fgOnOff = (BOOL) rArg.ai4Arg[0];
    bMode = (UINT8) rArg.ai4Arg[1];
    wCenter = (UINT16) rArg.ai4Arg[2];
    
    MTVDO_PRINT(" - fgOnOff = %d, bMode = %d, wCenter = %d\n", fgOnOff, bMode, wCenter);

    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    if (fgOnOff)
    {
        MJC_SetDemo(MJC_DEMO_SIDEBYSIDE, 0, 2);
    }
    else
    {
        MJC_SetDemo(MJC_DEMO_OFF, 2, 2);
    }
    #endif

    return MTR_OK;
}

#ifdef CC_SUPPORT_4K2K
extern void  DRVCUST_SetExtFrcBypassWindow(UINT8 bWId, UINT16 u2Width, UINT16 u2Height,  UINT16 u2StartX,  UINT16 u2StartY);
#endif
static MT_RESULT_T _MTVDO_SetMJCBypassWindow(unsigned long arg)
{
    MTAL_IOCTL_6ARG_T   rArg;
    MTAL_IOCTL_6ARG_T * prArg;
    UINT8 bWindow;
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4StartX;
    UINT32 u4StartY;
    #ifdef DRV_SUPPORT_EXTMJC
    UINT8 Ret;
    #endif

    prArg = (MTAL_IOCTL_6ARG_T *) arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prArg, MTAL_IOCTL_6ARG_T);
    COPY_FROM_USER_ARG(prArg, rArg, MTAL_IOCTL_6ARG_T);
    bWindow = (UINT8) rArg.ai4Arg[0];
    u4Width = (UINT32) rArg.ai4Arg[1];
    u4Height = (UINT32) rArg.ai4Arg[2];
    u4StartX = (UINT32) rArg.ai4Arg[3];
    u4StartY = (UINT32) rArg.ai4Arg[4];

    #ifdef DRV_SUPPORT_EXTMJC
    drv_extmjc_set_mjc_bypass_window_directly(bWindow, (UINT16)u4Width, (UINT16)u4Height, (UINT16)u4StartX, (UINT16)u4StartY,&Ret);
    #endif
    
    	#ifdef CC_SUPPORT_4K2K
        DRVCUST_SetExtFrcBypassWindow(bWindow, (UINT16)u4Width, (UINT16)u4Height, (UINT16)u4StartX, (UINT16)u4StartY);
    	#endif
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCDemoBar(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T   rArg;
    BOOL   fgOnOff;
    UINT8  u1R;
    UINT8  u1G;
    UINT8  u1B;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);

    fgOnOff = (BOOL) rArg.ai4Arg[0];
    u1R = (UINT8) rArg.ai4Arg[1];
    u1G = (UINT8) rArg.ai4Arg[2];
    u1B = (UINT8) rArg.ai4Arg[3];

    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetDemoBar(fgOnOff, u1R, u1G, u1B);
    #endif
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCMCOnOff(unsigned long arg)
{
    BOOL fgOnOff;

    fgOnOff = (BOOL)arg;
    
    #if defined(CC_MT5396) || defined(CC_MT5398)|| defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetMCOnOff(fgOnOff);
    #endif

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetMJCMC(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;

    UINT8 blurLevel;
    UINT8 judderLevel;
    UINT8 fgRealCinema;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
        
    blurLevel    = (UINT8)rArg.ai4Arg[0];
    judderLevel  = (UINT8)rArg.ai4Arg[1];
    fgRealCinema = (UINT8)rArg.ai4Arg[2];

    printf("_MTVDO_SetMJCMC: %d, %d\n", blurLevel, judderLevel);
    
    #if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    MJC_SetRealCinemaMode(fgRealCinema);
    MJC_SetDejudderLvl(judderLevel);
    MJC_SetDeblurLvl(blurLevel);
    vDrvUpdateRealCinemaMode(0xFFFF, fgRealCinema, 0xFFFF);
    #endif

    return MTR_OK;
}

MT_RESULT_T _MTVDO_GET_NET_SWAP_Buffer(unsigned long arg) 
{
    MT_RESULT_T eRet = MTR_OK ;
    MTVDO_VIDEO_BUFFER rBuffer;
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_VIDEO_BUFFER* prBuffer;
    FBM_POOL_T* prFbmPool;
    	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);
    prBuffer = (MTVDO_VIDEO_BUFFER*)rArg.ai4Arg[0];
    prFbmPool = FBM_GetPoolInfo((UINT8)FBM_POOL_TYPE_NET_SWAP);
    rBuffer.u4Addr= prFbmPool->u4Addr;
    rBuffer.u4Size= prFbmPool->u4Size;

    USR_SPACE_ACCESS_VALIDATE_ARG(prBuffer, MTVDO_VIDEO_BUFFER);
    COPY_TO_USER_ARG(prBuffer, rBuffer, MTVDO_VIDEO_BUFFER);

    return eRet;
}

MT_RESULT_T _MTVDO_SET_FBMMode(unsigned long arg) 
{
    UINT32 u4Mode;
    u4Mode = (UINT32)arg;
    
    SRM_SetCurrentFBMMode(u4Mode);
    return MTR_OK;
}
MT_RESULT_T _MTVDO_GET_FBMMode (unsigned long arg)
{

	UINT32 *pu4Val;
    UINT32  i4Mode;
    UINT32 rArg;
     
    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg,rArg);
  //add driver implementation code here
    i4Mode = SRM_GetCurrFBMMode();

    pu4Val=(UINT32*)rArg;

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Val,UINT32);
    COPY_TO_USER_ARG(pu4Val, i4Mode, UINT32);

    MTPERIPHERAL_PRINT(" - size = 0x%x \n",i4Mode);

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_EnableDIDramDump(unsigned long arg)    //for DDI DI Dram Dump
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;
    UINT8 u1Cnt;
    VDP_OVERSCAN_REGION_T rRegion;
    VDP_REGION_T rSrcRegion;
    #ifdef DI_DUMP_COLOR_MODE_TRANS
    static BOOL fgIsChgColorMode = 0;
    UINT16 u2Offset;
    #endif
    UINT8 fgIsNoDram;
    static UINT8 fgIsPredown;
    static BOOL fgIsChgFBM = 0;
    E_3DNR_MODE eOldNRmode = MCNR_MODE_OFF;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    u1Cnt = 0;
    if (fgEnable)
    {
        #ifdef DI_DUMP_COLOR_MODE_TRANS
        ///  bypass preproce
        u2Offset = (eVdpId == VDP_1) ? 0 : 0x100;
        vRegWriteFldAlign(0x3084 + u2Offset, 1, Fld(1,11,AC_MSKB1));

        // if RGB & 444 to 422  need to force 422
        if (SV_VD_DVI == bGetVideoDecType((UINT8)eVdpId) && bHDMIInputType() == 1 && !bIsScalerInput444((UINT8)eVdpId))
        {
            vSetHDMIColorMode(eVdpId, SV_HDMI_MODE_GRAPHIC);
            fgIsChgColorMode = 1;
        }
        else if (SV_VD_VGA == bGetVideoDecType(VDP_2) && u1GetVGAColorMode() == SV_TRUE)
        {
            vSetVGAColorMode(SV_VGA_MODE_GRAPHIC);
            fgIsChgColorMode = 1;
        }

        if (fgIsChgColorMode)
        {
            do{ 
    	        x_thread_delay(50);
    	        if (++u1Cnt == 20)
    	        {
    	            return MTR_NOT_OK;
    	        }
            }while( fgIsMainFlgSet(MAIN_FLG_MODE_CHG) ||
            bDrvVideoSignalStatus(eVdpId) != SV_VDO_STABLE ||  
            _vDrvVideoGetMuteDelay(eVdpId, MUTE_MODULE_MDDI01));
        }
        #endif
        
        eOldNRmode = eDrvNRGet3DNRMode();
        vDrvNRSet3DNRMode(MCNR_MODE_OFF);
        
        fgIsPredown = (u4PDSGetHPDSFactor(eVdpId) != 0x8000) ? TRUE : FALSE;
        fgIsPredown = fgIsPredown | ((u4PDSGetVPDSFactor(eVdpId) != 0x8000) ? TRUE : FALSE);
        fgIsNoDram = !fgDrvDIIsMIBDramAvaliable(eVdpId);
        
        if (fgIsPredown | fgIsNoDram)
        {           
            FBM_SetPscanTestMode(fgEnable);
            fgIsChgFBM = 1;
            vSetMainFlg(MAIN_FLG_MODE_CHG);
            vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
            
            if (bDrvVideoIsSrcInterlace(eVdpId))
            {
                vDrvDISetDataMode(eVdpId, E_MIB_FRC_MIB, E_MIB_DI_Y1C1, E_MIB_NR_OFF);
            }
            else
            {
                vDrvDISetDataMode(eVdpId, E_MIB_FRC_MIB, E_MIB_DI_OFF, E_MIB_NR_OFF);
            }

            if (fgIsPredown)
            {
                vScpipSetForcedHpsFactor((UINT32)eVdpId, 0x8000);  
                vScpipSetForcedVpsFactor((UINT32)eVdpId, 0x8000);  
            }
        }

        rRegion.u4Top = 0;
        rRegion.u4Bottom = 0;
        rRegion.u4Left = 0;
        rRegion.u4Right = 0;
        VDP_SetOverScan((UCHAR)eVdpId, *(VDP_OVERSCAN_REGION_T*)&rRegion);

        rSrcRegion.u4X = 0; 
        rSrcRegion.u4Y = 0;
        rSrcRegion.u4Width = VDP_MAX_REGION_WIDTH;
        rSrcRegion.u4Height = VDP_MAX_REGION_HEIGHT;
        VDP_SetSrcRegion((UCHAR)eVdpId, (UCHAR)1, *(VDP_REGION_T*)&rSrcRegion);                
        
        do{ 
    	    x_thread_delay(50);
    	    if (++u1Cnt == 20)
    	    {
    	        return MTR_NOT_OK;
    	    }
        }while(fgIsMainFlgSet(MAIN_FLG_MODE_CHG) ||
               VRM_IMPORT_WAIT_NONE != vDrvGetImportWaitFlag(eVdpId));
		
        vDrvDIDumpDisSnakeMapping(eVdpId);
        x_thread_delay(100);
    }
    else
    { 
        vDrvDIDumpEnd(eVdpId);
        vDrvNRSet3DNRMode(eOldNRmode);
        if (fgIsChgFBM)
        {
            vDrvDISetDataMode(eVdpId, E_MIB_FRC_SCL, E_MIB_DI_Y1C1, E_MIB_NR_YC);// set to default
            fgIsChgFBM = 0;
            FBM_SetPscanTestMode(fgEnable);
            vSetMainFlg(MAIN_FLG_MODE_CHG);
            vSetMainFlg(MAIN_FLG_MODE_DET_DONE);
        }

        if (fgIsPredown)
        {
            vScpipSetForcedHpsFactor((UINT32)eVdpId, 0);  
            vScpipSetForcedVpsFactor((UINT32)eVdpId, 0);  
            fgIsPredown =0;
        }

        #ifdef DI_DUMP_COLOR_MODE_TRANS
        u2Offset = (eVdpId == VDP_1) ? 0 : 0x100;
        vRegWriteFldAlign(0x3084 + u2Offset, 0, Fld(1,11,AC_MSKB1));

        if (SV_VD_DVI == bGetVideoDecType((UINT8)eVdpId) && bHDMIInputType()== 1 && fgIsChgColorMode)
        {
             vSetHDMIColorMode(eVdpId, SV_HDMI_MODE_AUTO);
            fgIsChgColorMode = 0;
        }
        else if (SV_VD_VGA == bGetVideoDecType(VDP_2) && fgIsChgColorMode)
        {
            vSetVGAColorMode(SV_VGA_MODE_VIDEO);
            fgIsChgColorMode = 0;
        }
        #endif

        do{ 
            x_thread_delay(50);
            if (++u1Cnt == 20)
            {
                return MTR_NOT_OK;
            }
        }while( fgIsMainFlgSet(MAIN_FLG_MODE_CHG) ||
                VRM_IMPORT_WAIT_NONE != vDrvGetImportWaitFlag(eVdpId));
    }

    return MTR_OK;
}

extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
static MT_RESULT_T _MTVDO_DI_GetDramPixels(unsigned long arg)
{

    MTVDO_REGION_T *prRegion;
    MTVDO_REGION_T rRegion;
    FBM_POOL_T* prPool;
    MTVDO_PATH_T eVdpId;
    MTAL_IOCTL_2ARG_T    rArg;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    prRegion = (MTVDO_REGION_T *) rArg.ai4Arg[1];

    if (prRegion == NULL)
    {
        return MTR_ERR_INV;       
    }	
    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);
    vDrvDISetDumpRegion(rRegion.u4X, rRegion.u4Y, rRegion.u4Width, rRegion.u4Height);
    vDrvDIDumpSetSize(eVdpId); 

    if(eVdpId == MTVDO_SUB)
    {
        prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_MDDI_PIP, NULL);
    }
    else
    {
        prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_MDDI_MAIN, NULL);
    }

    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(prPool->u4Addr),prPool->u4Size);
    
    x_thread_delay(70); 
    vDrvDIDumpStart(eVdpId);
    vDrvDIDumpEnd(eVdpId);

    return MTR_OK;

}

static MT_RESULT_T _MTVDO_DI_WriteDramPixels(unsigned long arg)
{
    DI_BUFFER_DATA rPixel;
    MTVDO_LGDDI_BUFFER_DATA *prAllData;
    MTVDO_LGDDI_BUFFER_DATA *prData;
    MTVDO_REGION_T *prRegion;
    MTVDO_REGION_T rRegion;
    MTVDO_PATH_T eVdpId;
    UINT32 u4I, u4J, u4Size;
    UINT16 x, y, u2W, u2H;
    UINT8 fgIs422;
    MTAL_IOCTL_3ARG_T    rArg;
    FBM_POOL_T* prPool;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    prRegion = (MTVDO_REGION_T *) rArg.ai4Arg[1];
    prData = (MTVDO_LGDDI_BUFFER_DATA*)rArg.ai4Arg[2];

    if (prRegion == NULL)
    {
        return MTR_ERR_INV;       
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(prRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prRegion, rRegion, MTVDO_REGION_T);

    fgIs422 = u1DrvDiIs422Mode(eVdpId);

    u2W = (UINT16) rRegion.u4Width;
    u2H = (UINT16) rRegion.u4Height;
    u4Size = (UINT32)(u2W) * (UINT32)(u2H); 
    u4Size *= sizeof(MTVDO_LGDDI_BUFFER_DATA);
    prAllData = (MTVDO_LGDDI_BUFFER_DATA *) kmalloc(u4Size, GFP_KERNEL);
    
    if (prAllData == NULL)
    {
        kfree(prAllData);
        return MTR_NOT_OK;
    }

    if (copy_from_user(prAllData, prData, u4Size))    
    {                                                                          
        printk("%s copy_from_user error\n", __FUNCTION__);   
        kfree(prAllData);                  
        return MTR_ERR_INV;                                                    
    }
    
    for (u4I = 0; u4I < rRegion.u4Height; u4I++ )
    {
        y = (UINT16)(u4I + rRegion.u4Y);
        for (u4J = 0; u4J < rRegion.u4Width; u4J++ )
        {
            x = (UINT16)(u4J + rRegion.u4X);
            rPixel.u4YG = (UINT32) prAllData[u4I * rRegion.u4Width + u4J].u2YG;
            if (fgIs422)
            {
                if (x % 2 == 0)
                {
                    rPixel.u4UB = (UINT32) prAllData[u4I * rRegion.u4Width + u4J].u2UB;
                    rPixel.u4VR = 0;
                }
                else
                {
                    rPixel.u4UB = (UINT32) prAllData[u4I * rRegion.u4Width + u4J].u2VR;
                    rPixel.u4VR = 0;
                }
            }
            else
            {
                rPixel.u4UB = (UINT32) prAllData[u4I * rRegion.u4Width + u4J].u2UB;
                rPixel.u4VR = (UINT32) prAllData[u4I * rRegion.u4Width + u4J].u2VR;
            }
            vDrvDIWritePixelValue(eVdpId,x,y,(DI_BUFFER_DATA*)&rPixel);
        }	
    }
    if(eVdpId == MTVDO_SUB)
    {
        prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_MDDI_PIP, NULL);
    }
    else
    {
        prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_MDDI_MAIN, NULL);
    }

    HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(prPool->u4Addr),prPool->u4Size);

    kfree(prAllData);

    return MTR_OK;
}

MT_RESULT_T _MTVDO_DI_GetFormatInfo(unsigned long arg) 
{
    MT_RESULT_T eRet = MTR_OK ;
    MTVDO_FRAME_FORMAT_DATA rFormat;
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_FRAME_FORMAT_DATA* prFormat;
    UINT8 bType, fgIsPredown;
    	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prFormat = (MTVDO_FRAME_FORMAT_DATA*)rArg.ai4Arg[1];
    
    rFormat.fgIsInterlace = bDrvVideoIsSrcInterlace(eVdpId);
    rFormat.fgIs10bit      = u1DrvDiIs10Bit(eVdpId);
    rFormat.fgIsTopField = u1DrvDiIsTopField(eVdpId);
    rFormat.fgIs422        = u1DrvDiIs422Mode(eVdpId);
    fgIsPredown = (u4PDSGetHPDSFactor(eVdpId) != 0x8000) ? TRUE : FALSE;
    rFormat.fgIsPredown = fgIsPredown | ((u4PDSGetVPDSFactor(eVdpId) != 0x8000) ? TRUE : FALSE);
    //rFormat.fgIsPredown = (u4ScpipGetHwHPS(eVdpId) != 0x8000) ? TRUE : FALSE;

    bType = bGetVideoDecType((UINT8)eVdpId);
    rFormat.fgIsYUV = !bIsScalerInputRGB((UINT8)eVdpId);

    USR_SPACE_ACCESS_VALIDATE_ARG(prFormat, MTVDO_FRAME_FORMAT_DATA);
    COPY_TO_USER_ARG(prFormat, rFormat, MTVDO_FRAME_FORMAT_DATA);
    
    return eRet;
}

    
static MT_RESULT_T _MTVDO_SetFreezeDIFrameBuffer(unsigned long arg)    //for DDI DI Dram Dump
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    vDrvDIFreezeOnOff(eVdpId, fgEnable);

    return MTR_OK;
}



static MT_RESULT_T _MTVDO_GetSBSContent(unsigned long arg)
{
    UINT32* pu4SBSContent;
    UINT32  u4SBSContent;

    pu4SBSContent = (UINT32*)arg;
    u4SBSContent = (UINT32)u4DrvTDTVIsDecoderContentFull();   

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4SBSContent,INT32);
    COPY_TO_USER_ARG(pu4SBSContent, u4SBSContent,INT32);

    MTPMX_PRINT(" - u4SBSContent = %d\n", (int)u4SBSContent);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetFRCMuteOnOff(unsigned long arg)
{
    BOOL fgOnOff;
    
    fgOnOff = ((BOOL)arg)? FALSE: TRUE;
    _vDrvVideoSetMuteInvalidFlag(MUTE_MODULE_MJC, VDP_1, fgOnOff, MUTE_INVALID_CLI_FORCE);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetFrameRate(unsigned long arg)
{
#if defined(CC_MT5396) || defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    UINT32* pu4FrameRate;
    UINT32  u4FrameRate;

    pu4FrameRate = (UINT32*)arg;

    u4FrameRate = (UINT32)u2DrvGetInternalFrameRate();   

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4FrameRate,INT32);
    COPY_TO_USER_ARG(pu4FrameRate, u4FrameRate,INT32);

    MTPMX_PRINT(" - u4FrameRate = %d\n", (int)u4FrameRate);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GET_3D_LR_ORDER(unsigned long arg)
{
    #if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 bPath;
    UINT32 dw3DLROrder;

    // 1. take pointer from user layper
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    // 2. pass pointer from user layer to kernel layer
    
    // 3. do the driver function
    bPath = rArg.ai4Arg[0];       
    dw3DLROrder = u4DrvTDTVGetInputLROrder();
    
    // 4. pass pointer from kernel layer to user layer
    USR_SPACE_ACCESS_VALIDATE_ARG(rArg.ai4Arg[1],UINT32);    
    COPY_TO_USER_ARG(rArg.ai4Arg[1], dw3DLROrder, UINT32);    
    #endif
    return MTR_OK;   
}

static MT_RESULT_T _MTVDO_EnableOverDriver(unsigned long arg)
{
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    BOOL fgOnOff;

    fgOnOff = (BOOL)arg;
    
    vDrvODOnOff(fgOnOff);
    vDrvODBypass(!fgOnOff);
#endif    
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_EnablePCID(unsigned long arg)
{
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    BOOL fgOnOff;
        
    fgOnOff = (BOOL)arg;
    vDrvPCIDOnOff(fgOnOff);
#endif    

    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetInputFrameRate(unsigned long arg)
{

    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32* pu4FrameRate;
    UINT32  u4FrameRate;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu4FrameRate = (UINT32*)rArg.ai4Arg[1];

    u4FrameRate = bDrvVideoGetRefreshRate(eVdpId);   

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4FrameRate,INT32);
    COPY_TO_USER_ARG(pu4FrameRate, u4FrameRate,INT32);

    MTPMX_PRINT(" - u4FrameRate = %d\n", (int)u4FrameRate);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetFrameDelay(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;

    INT32* pu4FrameDelay;
    INT32  i4FrameDelay;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu4FrameDelay = (INT32*)rArg.ai4Arg[1];

    i4FrameDelay = (INT32)bDrvVideoGetFrameDelay(eVdpId);   

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4FrameDelay,INT32);
    COPY_TO_USER_ARG(pu4FrameDelay, i4FrameDelay,INT32);

    MTPMX_PRINT(" - u2FrameDelay = %d\n", (int)i4FrameDelay);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SET_3D_NAVI_PARAM(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    UINT8 bPath;
    UINT32 dwNaviParam;
    UINT32 dwNaviValue;

    // 1. take pointer from user layper
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    
    // 2. pass pointer from user layer to kernel layer
    
    // 3. do the driver function
    bPath = rArg.ai4Arg[0];
    dwNaviParam = rArg.ai4Arg[1];
    dwNaviValue = rArg.ai4Arg[2];
    //vDrvTDTVSetNaviParam(bPath, dwNaviParam, dwNaviValue);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetVgaMode(unsigned long arg)
{
    UINT8 bMode;

    bMode = *((UINT8 *)arg);

    MTVDO_PRINT(" - bMode = %d\n", (int)bMode);
    vSetVGAColorMode(bMode);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetHDMIMode(unsigned long arg)
{
    UINT8 bMode;

    bMode = *((UINT8 *)arg);
    MTVDO_PRINT(" - bMode = %d\n", (int)bMode);
    //vSetHDMIColorMode(bMode);
    //please use MTVDECEX_HDMI_SetColorMode instead
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_BYPASS_PREPROC(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;
    static UINT8 fgIsGraphic = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    if (fgEnable)
    {
        if (SV_VD_DVI == bGetVideoDecType((UINT8)eVdpId) && bHDMIInputType() == 1 && !bIsScalerInput444((UINT8)eVdpId))
        {
            vSetHDMIColorMode(eVdpId, SV_HDMI_MODE_GRAPHIC);
            fgIsGraphic = 1;
        }
        //vDrvMatrixCtrlSel(eVdpId, 4);
    }
    else
    {   
        if (SV_VD_DVI == bGetVideoDecType((UINT8)eVdpId) && bHDMIInputType() == 1 && fgIsGraphic)
        {
            vSetHDMIColorMode(eVdpId, SV_HDMI_MODE_AUTO);
            fgIsGraphic = 0;
        }
        //vDrvMatrixCtrlSel(eVdpId, 0);
    }
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcdimInitialize(unsigned long arg)
{
    #if defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890)
    //    PANEL_LOCAL_DIMMING_ATTRIBUTE_T * _pPanelLCTable = (PANEL_LOCAL_DIMMING_ATTRIBUTE_T *)arg;
        //  define local dimming initialization here
    #ifndef CC_MTK_LOADER    
        if (fgIsSWRegInit()!=SV_FALSE)
        {
            if(PANEL_IsSupportLocalDimming())
            {
                if(bDrvLcdimSpiInit() == SV_FAIL)
                {
                    bSupportLocalDimming = SV_FALSE;
                }        
                else    // PQ setting is only initialized when SPI is ready
                {
                    if(bDrvLcDimPanelInit() == SV_SUCCESS)
                    {
                        vDrvLcDimQualityInit();
                        bSupportLocalDimming = SV_TRUE;
                    }
                    else
                    {
                        bSupportLocalDimming = SV_FALSE;
                    }
                }
            }    
            else
            {
                bSupportLocalDimming = SV_FALSE;
            }
        } 
    #endif
    #endif
        return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcDimEn(unsigned long arg)
{
    BOOL fgEnable;
    fgEnable = (BOOL)arg;
    
    #if defined(CC_MT5880) || defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    vDrvSetLcDimOnOff(fgEnable);
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcDSEn(unsigned long arg)
{
    BOOL fgEnable;
    fgEnable = (BOOL)arg;
    
    // wait to sync to 98
    #if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    vDrvSetLcDSOnOff(fgEnable);
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcDimLUTIdx(unsigned long arg)
{
    UINT8 u1Index;
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg,UINT8);
    COPY_FROM_USER_ARG(arg,u1Index,UINT8); 
    
    #ifdef CC_MT5398
    //  set dimming LUT index here
    //  set sw curve or hardware curve?
    #endif
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcDimDemoMode(unsigned long arg)
{

    MTAL_IOCTL_2ARG_T rArg;

    UINT8 bType;
    BOOL bCtrl;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    bType = (UINT8)rArg.ai4Arg[0];
    bCtrl = (UINT8)rArg.ai4Arg[1];

// wait to sync to 98
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
    vDrvLcdimSetDSDemo(bType, bCtrl);
 #endif
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetLcDimSpiCommand(unsigned long arg)
{

    MTAL_IOCTL_2ARG_T rArg;

    UINT8 bMask;
    UINT8 bOnOff;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    
    bMask = (UINT8)rArg.ai4Arg[0];
    bOnOff = (UINT8)rArg.ai4Arg[1];

    vDrvLcdimSpiSetHeader(bMask, bOnOff);
	
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetODTable(unsigned long arg)
{
// To do?
#if 0
    UINT32 u4ODTable;
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, UINT32);
    COPY_TO_USER_ARG(arg, u4ODTable, UINT32);

    x_memcpy(OD_Table, u4ODTable, OD_TBL_M_SIZE);
    vApiRegisterVideoEvent(PE_EVENT_OD, SV_VP_MAIN, SV_ON);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SRMResume(unsigned long arg)
{
    MTVDO_PRINT(" - SRM Resume\n");
    SRM_Resume();
    return MTR_OK;
}

//mw_if porting start
static MT_RESULT_T _MTVDO_SetDownBwMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4VdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4VdpId = (UINT32)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %u, fgEnable = %d\n", u4VdpId, fgEnable);

    VDP_SetDownBWMode((UINT8)fgEnable);
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetCondForceUnmute(unsigned long  arg)
{

    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT8 u1Enable;
    UINT8 u1UnMuteType;
    MTVDO_FORCE_UNMUTE_T eUnMuteType;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eUnMuteType = (MTVDO_FORCE_UNMUTE_T)rArg.ai4Arg[1];
    u1Enable = (UINT8)rArg.ai4Arg[2];

    switch (eUnMuteType)
    {
        case MTVDO_NO_SIGNAL_UNMUTE:
            u1UnMuteType = SV_FORCE_UNMUTE_TYPE_ON_NO_SIG;
            break;
        case MTVDO_NO_SIGNAL2_UNMUTE:
            u1UnMuteType = SV_FORCE_UNMUTE_TYPE_ON_NO_SIG2;
            break;
        case MTVDO_ATV_CHG_CH_UNMUTE:
            u1UnMuteType = SV_FORCE_UNMUTE_TYPE_ATV_CHG_CH;
            break;
        case MTVDO_ATV_PVR_UNMUTE:
            u1UnMuteType = SV_FORCE_UNMUTE_TYPE_ALL;
            #if defined(CC_MT5398) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882)
            FBM_FixedScalerDram(u1Enable);
            #endif
            break;
        default:
            return MTR_ERR_INV;
    }
    vDrvSetForceUmute(eVdpId, u1UnMuteType, u1Enable);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetLBoxEnable(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UCHAR *pucEnable;
    UCHAR ucEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pucEnable = (UCHAR*)rArg.ai4Arg[1];

    ucEnable = (UCHAR)u1LboxEn;

    USR_SPACE_ACCESS_VALIDATE_ARG(pucEnable,UCHAR);
    COPY_TO_USER_ARG(pucEnable, ucEnable, UCHAR)
    MTVDO_PRINT(" - eVdpId = %d, ucEnable = %d\n", (int)eVdpId, (int)ucEnable);
    return MTR_OK;

}

static MT_RESULT_T _MTVDO_SetLBoxEnable(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL fgEnable;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    fgEnable = (BOOL)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, fgEnable = %d\n", 
        (int)eVdpId, (int)fgEnable);

    if (VDP_SetLBoxDetection(eVdpId, fgEnable, 0) != VDP_SET_OK)
    {
        return MTR_NOT_OK;
    }
    
    return MTR_OK;
}


static MT_RESULT_T _MTVDO_SetLBoxConfig(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4DetPeriod, u4Ratio;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4DetPeriod = (UINT32)rArg.ai4Arg[1];
    u4Ratio = (UINT32)rArg.ai4Arg[2];

    MTVDO_PRINT(" - eVdpId = %d, u4DetPeriod= %d u4Ratio=%d \n", 
        (int)eVdpId, u4DetPeriod, u4Ratio);


    if (VDP_LBoxDetectMode(eVdpId, u4DetPeriod, u4Ratio) != VDP_SET_OK)
    {
        return MTR_NOT_OK;
    }
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetCapability(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_CAPABILITY_T eCapType;
    UCHAR *pucIsSupport;
    UCHAR ucIsSupport;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eCapType = (MTVDO_CAPABILITY_T)rArg.ai4Arg[1];
    pucIsSupport = (UCHAR*)rArg.ai4Arg[2];

    switch(eCapType)
    {
        case  MTVDO_FREEZABLE_CAP:
            VDP_GetFreezable(eVdpId, &ucIsSupport);
            break;

        case  MTVDO_OVERSCAN_CAP:
            ucIsSupport = (UCHAR) VDP_IsSupportOverscan(eVdpId);
            break;

        case  MTVDO_NONLINEAR_SCALING_CAP:
            ucIsSupport = (UCHAR) VDP_IsSupportNonlinear(eVdpId);
            break;

        case  MTVDO_DOTBYDOT_CAP:
            ucIsSupport = (UCHAR) VDP_IsSupportDotByDot();
            break;

        case  MTVDO_MJC3D_CAP:
            ucIsSupport = (UCHAR) u1DrvTDTVMJCEnableQuery();
            break;
        case MTVDO_ASP_RATIO_CAP:
            ucIsSupport = (UCHAR) VDP_IsSupportAspectRatio(eVdpId);
            break;
       
         default:
            return MTR_NOT_SUPPORTED;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pucIsSupport,UCHAR);
    COPY_TO_USER_ARG(pucIsSupport, ucIsSupport, UCHAR)
    return MTR_OK;
}
static MT_RESULT_T _MTVDO_SetSrmMode(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_SRM_MODE_T eSrmMode;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eSrmMode = (MTVDO_SRM_MODE_T)rArg.ai4Arg[1];

    MTVDO_PRINT(" - eVdpId = %d, eSrmMode = %d\n", 
        (int)eVdpId, (int)eSrmMode);

    switch(eSrmMode)
    {
        case MTVDO_MM_VIDEO_MODE:
            VDP_SetThumbnailMode(eVdpId, 0);
            SRM_SetMmModeByPath(eVdpId, SRM_MM_MODE_TYPE_VIDEO);
           
            break;
        case MTVDO_MM_THUMBNAIL_MODE:
            VDP_SetThumbnailMode(eVdpId, 1);

            SRM_SetMmModeByPath(eVdpId, SRM_MM_MODE_TYPE_THUMBNAIL);
          
            break;
        default:
        case MTVDO_MM_MODE_OFF:
            VDP_SetThumbnailMode(eVdpId, 0);
            SRM_SetMmModeByPath(eVdpId, SRM_MM_MODE_TYPE_OFF);
           
            break;
    }
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_UpdateCmdDelay(unsigned long arg)
{
    INT32 rArg;
    MTVDO_PATH_T eVdpId;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg;

    
    if (VDP_UpdateCmdDelay((UINT32)eVdpId) != VDP_SET_OK)
    {
        return MTR_NOT_OK;
    }
    return MTR_OK;
}
static MT_RESULT_T _MTVDO_GetCmdDelay(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4CmdDelay;
    UINT32 *pu4CmdDelay;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu4CmdDelay = (UINT32 *)rArg.ai4Arg[1];

    u4CmdDelay = VDP_GetCmdDelay(); 

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4CmdDelay,UINT32);
    COPY_TO_USER_ARG(pu4CmdDelay, u4CmdDelay, UINT32)
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetVideoMute(unsigned long  arg)
{

    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    BOOL bReplaceByLongerDelay;
    UINT32 u4UnMuteDelay;
    MTVDO_MUTE_T eUnMuteType;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    eUnMuteType = (MTVDO_MUTE_T)rArg.ai4Arg[1];
    u4UnMuteDelay = (UINT32)rArg.ai4Arg[2];
    bReplaceByLongerDelay = (BOOL)rArg.ai4Arg[3];

    switch(eUnMuteType)
    {
        case MTVDO_DISP_FMT_MUTE:
            _vDrvVideoSetMute(MUTE_MODULE_MW_DISP_FMT, eVdpId, u4UnMuteDelay, bReplaceByLongerDelay);
            break;
        case MTVDO_AP_MUTE: 
            _vDrvVideoSetMute(MUTE_MODULE_API_FORCE, eVdpId, u4UnMuteDelay, bReplaceByLongerDelay);
            break;
        default:
            break;
    }
    return MTR_OK;
}
static MT_RESULT_T _MTVDO_SetDynamicScale(unsigned long  arg)
{

    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4Delay;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4Delay = (UINT32)rArg.ai4Arg[1];

    u2DynamicScale = (UINT16) u4Delay ;

    if(u4Delay != 0)
    {
        if (VDP_SetDynamiceScaler(eVdpId, 1, u4Delay) != VDP_SET_OK)
        {
            return MTR_NOT_OK;
        }    
    }
    else
    {
        if (VDP_SetDynamiceScaler(eVdpId, 0, 250) != VDP_SET_OK)
        {
            return MTR_NOT_OK;
        }    
    }
    
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetDynamicScale(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 *pu4Delay;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu4Delay = (UINT32 *)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(pu4Delay,UINT32);
    COPY_TO_USER_ARG(pu4Delay, u2DynamicScale, UINT32)
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetDramInfo(unsigned long arg)
{
#if 1
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;

    MTVDO_DRAM_INFO_T *prDramInfo;
    MTVDO_DRAM_INFO_T rDramInfo;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    prDramInfo= (MTVDO_DRAM_INFO_T*)rArg.ai4Arg[1];

    VDP_GetDramInf(eVdpId,(DRAM_INFO_T *)&rDramInfo);    
                    
    USR_SPACE_ACCESS_VALIDATE_ARG(prDramInfo, MTVDO_DRAM_INFO_T);
    COPY_TO_USER_ARG(prDramInfo, rDramInfo, MTVDO_DRAM_INFO_T);
#endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetScalerDispMode(unsigned long arg)
{
    INT32 rArg;
    UCHAR *pucEnable;
    UCHAR ucEnable;

    USR_SPACE_ACCESS_VALIDATE_1ARG(arg);
    COPY_FROM_USER_1ARG(arg, rArg);
    pucEnable = (UCHAR*)rArg;

    ucEnable = (UCHAR)VDP_CheckScalerDisplayMode();

    USR_SPACE_ACCESS_VALIDATE_ARG(pucEnable,UCHAR);
    COPY_TO_USER_ARG(pucEnable, ucEnable, UCHAR)
    MTVDO_PRINT(" ucEnable = %d\n", (int)ucEnable);
    return MTR_OK;

}

static MT_RESULT_T _MTVDO_GetMirrorFlip(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4Type;
    UINT32 *pu4FlipMirror;
    UINT32 u4FlipMirror;
    DTVCFG_T rDtvCfg;
    UINT32 u4Ret;
    UINT8 u1Flag;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4Type = (UINT32)rArg.ai4Arg[0];
    pu4FlipMirror= (UINT32 *)rArg.ai4Arg[1];

    if (EEPDTV_GetCfg(&rDtvCfg))
    {
        u4Ret = MTR_ERR_INV;
    }
    else
    {
        u4Ret = MTR_OK;
        
        if (u4Type == ENUM_FLIP_TYPE)  // flip
        {
            u1Flag = DTVCFG_FLAG2_FLIP_ON;            
        }
        else // mirror
        {
            u1Flag = DTVCFG_FLAG2_MIRROR_ON;            
        }

        if (rDtvCfg.u1Flags2 & u1Flag)
        {
            u4FlipMirror = 1 ;
        }
        else
        {
            u4FlipMirror = 0 ;
        }

        USR_SPACE_ACCESS_VALIDATE_ARG(pu4FlipMirror,UINT32);
        COPY_TO_USER_ARG(pu4FlipMirror, u4FlipMirror, UINT32);
    }
    
    
    return u4Ret;
}

static MT_RESULT_T _MTVDO_SetMirrorFlip(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT32 u4Type;
    UINT32 u4FlipMirror;
    DTVCFG_T rDtvCfg;
    UINT32 u4Ret;
    UINT8 u1Flag;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    u4Type = (UINT32)rArg.ai4Arg[0];
    u4FlipMirror= (UINT32)rArg.ai4Arg[1];

    if (EEPDTV_GetCfg(&rDtvCfg))
    {
        u4Ret = MTR_ERR_INV;
    }
    else
    {
        u4Ret = MTR_OK;
        
        if (u4Type == ENUM_FLIP_TYPE )  // flip
        {
            u1Flag = DTVCFG_FLAG2_FLIP_ON;            
        }
        else // mirror
        {
            u1Flag = DTVCFG_FLAG2_MIRROR_ON;            
        }

        rDtvCfg.u1Flags2 &= ~(u1Flag);
        if (u4FlipMirror)
        {
            rDtvCfg.u1Flags2 |= (u1Flag);
        }

        if(EEPDTV_SetCfg(&rDtvCfg))
        {
            u4Ret = MTR_ERR_INV;
        }
    }
        
    return u4Ret;
}

static MT_RESULT_T _MTVDO_GetScanMode(UINT32* pkvVdoSetInfo, UINT32 u4Size)
{
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }
    *pkvVdoSetInfo = u4GetAutoSearchType();
    MTVDO_PRINT(" - get scan mode = %d\n", *pkvVdoSetInfo);
            
    return  MTR_OK;
}

static INT32 _MTVDO_Set3DImgSaftyLvl(const UINT32* prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
    MTVDECEX_PRINT(" - set 3d img safty 0x%x\n",(* prData));  
    vDrvTDTVTImgSafety((* prData)); 
     return  MTR_OK;
}

static INT32 _MTVDO_Get3DFmtCap(UCHAR ucVdpId, UINT32* prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
  
    * prData = u4DrvTDTV3DFMTCapQuery(ucVdpId); 
     MTVDECEX_PRINT(" - _MTVDO_Get3DFmtCap %d 0x%x\n",ucVdpId, (* prData));

     return  MTR_OK;
}

static INT32 _MTVDO_Get3DCtrlCap(UCHAR ucVdpId, UINT32* prData, UINT32 u4Size)
{

    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
    * prData = u4DrvTDTV3DCtrlCapQuery(ucVdpId);    
    MTVDECEX_PRINT(" - _MTVDO_Get3DCtrlCap %d 0x%x\n",ucVdpId, (* prData));
    return  MTR_OK;
}

static INT32 _MTVDO_Get3DImgSaftyLvl(UINT32* prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = eDrvTDTVTDImgSafetyLvlQuery(); 
         MTVDECEX_PRINT(" - _MTVDO_Get3DImgSafty  0x%x\n" , (* prData));
     return  MTR_OK;
}
static INT32 _MTVDO_Get3DNavMode(UINT32* prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = eDrvTDTVTDNavimModeQuery();
         MTVDECEX_PRINT(" - _MTVDO_Get3DImgSafty  0x%x\n" , (* prData));
     return  MTR_OK;
}

static INT32 _MTVDO_Get3DDoneByMJC(UINT32* prData, UINT32 u4Size)
{
  
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = vScpipIs3DDoneByMJC();
         MTVDECEX_PRINT(" - _MTVDO_Get3DDoneByMJC  0x%x\n" , (* prData));
     return  MTR_OK;
}

static MT_RESULT_T _MTVDO_GetMinSrcRegionWidth(UINT32* prData, UINT32 u4Size)
{

    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = VDP_GetMinSrcRegionWidth();
         MTVDECEX_PRINT(" - _MTVDO_GetMinSrcRegionWidth  0x%x\n" , (* prData));

    return MTR_OK;

}

static MT_RESULT_T _MTVDO_GetMinSrcRegionHeight(UINT32* prData, UINT32 u4Size)
{

    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = VDP_GetMinSrcRegionHeight();
         MTVDECEX_PRINT(" - _MTVDO_GetMinSrcRegionHeight  0x%x\n" , (* prData));

    return MTR_OK;

}


static MT_RESULT_T _MTVDO_TDTV_GetTag3D(UCHAR ucVdpId, MTTDTV_TAG3D_TYPE_T* prData, UINT32 u4Size)
{

    if (u4Size != sizeof(MTTDTV_TAG3D_TYPE_T))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        * prData = (MTTDTV_TAG3D_TYPE_T) u4DrvTDTVGetTag3D(ucVdpId);
         MTVDECEX_PRINT(" - _MTVDO_GetMinSrcRegionHeight  0x%x\n" , (* prData));

    return MTR_OK;

}

static MT_RESULT_T _MTVDO_SetMonOutEnable(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    BOOL fgEnable;

    fgEnable = (BOOL)arg;
    MTVDO_PRINT(" Set Mon out fgEnable = %d\n", (int)fgEnable);
    vMonOutSetEnable((UINT8)fgEnable);
#endif
	return MTR_OK;
}

static MT_RESULT_T _MTVDO_GetMonOutEnable(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    UCHAR *pEnable;
    UCHAR ucEnable;

    pEnable = (UCHAR *)arg;
    ucEnable = vMonOutGetEnable();
    USR_SPACE_ACCESS_VALIDATE_ARG(pEnable,  UCHAR);
    COPY_TO_USER_ARG(pEnable,  ucEnable,  UCHAR);
    MTVDO_PRINT(" Get Mon out fgEnable = %d\n", (int)ucEnable);
#endif
    return MTR_OK;    
}

static MT_RESULT_T _MTVDO_SetMonOutSource(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

	MTVDO_MON_OUT_IN_SELECT eMonsrcsel;

	eMonsrcsel = (MTVDO_MON_OUT_IN_SELECT)arg;
	vMonOutSetMuxSelIn(eMonsrcsel);
    MTVDO_PRINT(" Set Mon out input = %d\n", (int)eMonsrcsel);
#endif
    return MTR_OK;	
}

static MT_RESULT_T _MTVDO_GetMonOutSource(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    UCHAR *pSrcsel;
    UCHAR ucSrcsel;

    pSrcsel = (UCHAR *)arg;
    ucSrcsel = vMonOutGetMuxSelIn();
    USR_SPACE_ACCESS_VALIDATE_ARG(pSrcsel,  UCHAR);
    COPY_TO_USER_ARG(pSrcsel,  ucSrcsel,  UCHAR);
    MTVDO_PRINT(" Get Mon out mux sel = %d\n", (int)ucSrcsel);
#endif
    return MTR_OK;   	
}

/*
E_MON_OUT_WITH_ALL_OSD  =0,
E_MON_OUT_WITH_ONE_OSD  =1,
E_MON_OUT_WITH_NO_OSD   =2,
*/
static MT_RESULT_T _MTVDO_SetMonOutOSDSel(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

	E_MON_OUT_IN_SELECT_2 MonOutOSDIn;

	MonOutOSDIn = (E_MON_OUT_IN_SELECT_2)arg;
	vMonOutSetOSDSelIn(MonOutOSDIn);
    MTVDO_PRINT(" Set Mon out OSD SEL = %d\n", (int)MonOutOSDIn);
#endif
    return MTR_OK;	 	
}

static MT_RESULT_T _MTVDO_SetMonOutOutputWindow(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    MTVDO_REGION_T *prOutRegion;
    MTVDO_REGION_T rOutRegion;

    prOutRegion = (MTVDO_REGION_T*)arg;

    USR_SPACE_ACCESS_VALIDATE_ARG(prOutRegion, MTVDO_REGION_T);
    COPY_FROM_USER_ARG(prOutRegion, rOutRegion, MTVDO_REGION_T);

	vMonOutSetOutputRes(rOutRegion.u4Width,rOutRegion.u4Height);	
    MTVDO_PRINT(" Set Mon out output width %d height %d = %d\n", rOutRegion.u4Width,rOutRegion.u4Height);
#endif
	return MTR_OK;   	
}

/*
	1: with OSD
	0: without OSD
*/
static MT_RESULT_T _MTVDO_SetOSDSel(unsigned long arg)
{
#if defined(CC_MT5882)

	UINT8 fgEnable;
	fgEnable = (UINT8)arg;
	
	vCCIROSDOnOff(fgEnable);
    MTVDO_PRINT(" Set Video mixer OSD SEL = %d\n", fgEnable);
#endif
    return MTR_OK;	 	
}

static MT_RESULT_T _MTVDO_SetVideoPixelShift(unsigned long arg)
{
#if defined(CC_MT5890)||defined(CC_MT5861)
    MTAL_IOCTL_3ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT32 u4Xoffset, u4Yoffset;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    u4Xoffset = (UINT32)rArg.ai4Arg[1];
    u4Yoffset = (UINT32)rArg.ai4Arg[2];

	vScpipSetVideoPixelShift(eVdpId, u4Xoffset, u4Yoffset);
    MTVDO_PRINT(" Set Video Pixel Shift x offset: %d y offset: %d = %d\n",u4Xoffset,u4Yoffset);
#endif
	return MTR_OK;   	
}

static MT_RESULT_T _MTVDO_GetMonOutOutputWindow(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    MTVDO_REGION_T *prOutRegion;
    MTVDO_REGION_T rOutRegion;
	UINT16 width,height;
    prOutRegion = (MTVDO_REGION_T *)arg;
    vMonOutGetOutputRes(&width,&height);
	rOutRegion.u4Width = (UINT32)width;
	rOutRegion.u4Height = (UINT32)height;
	rOutRegion.u4X = 0;
	rOutRegion.u4Y = 0;
    USR_SPACE_ACCESS_VALIDATE_ARG(prOutRegion,  MTVDO_REGION_T);
    COPY_TO_USER_ARG(prOutRegion,  rOutRegion,  MTVDO_REGION_T);
#endif
    return MTR_OK;	
}

static MT_RESULT_T _MTVDO_SetMonDotByDotOutput(unsigned long arg)
{
#if defined(CC_MT5399) || defined(CC_MT5890)

    BOOL bDotflag;
    bDotflag = (BOOL)arg;
    MTVDO_PRINT(" Set Mon out Dot by Dot OutOutput = %d\n", (int)bDotflag);
    vMonOutSetDotByDotOutput(bDotflag);
    
#endif
    return MTR_OK;	
}

static MT_RESULT_T _MTVDO_SetBrowserInput(unsigned long arg)
{
    #if defined(CC_MT5890)
    BOOL fgEnable;
    fgEnable = (BOOL)arg;
    MTVDO_PRINT("fgEnable = %d\n", (int)fgEnable);
    VDP_SetBrowserInput((UINT8)fgEnable);
	#endif
	return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetSpeIPTVS(unsigned long arg)
{
    #if defined(CC_MT5890)
    BOOL fgEnable;
    fgEnable = (BOOL)arg;
    MTVDO_PRINT("fgEnable = %d\n", (int)fgEnable);
    VDP_SetSpeIPTVS((UINT8)fgEnable);
	#endif
	return MTR_OK;
}
static MT_RESULT_T _MTVDO_TDTV_GetDecTag3D(MTAL_IOCTL_2ARG_T* prData, UINT32 u4Size)
{

    if (u4Size != sizeof(MTAL_IOCTL_2ARG_T))
    {
        return MTR_PARAMETER_ERROR;                            
    }    
        prData->ai4Arg[1] = (UINT32) u4DrvTDTVGetDecTag3D((E_DEC_MOD)prData->ai4Arg[0]);
         MTVDECEX_PRINT(" - _MTVDO_GetMinSrcRegionHeight  0x%x\n" , (* prData));

    return MTR_OK;

}

static MT_RESULT_T _MTVDO_ExtmjcInit(void)
{
    MTVDO_PRINT(" - void\n");
    #ifdef DRV_SUPPORT_EXTMJC
    drv_extmjc_init();
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetPentouchMode(const UINT32* prData, UINT32 u4Size)
{
    if (u4Size != sizeof(UINT32))
    {
        return MTR_PARAMETER_ERROR;
    }

    MTVDO_PRINT(" - set pentouch mode %d\n", (* prData));
	#ifndef CC_MT5881
    vDrvDisplaySetPentouchMode((* prData)); 
    #endif
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_SetCmd(UCHAR ucVdpId, UINT32 u4VdoType, const void* pkvVdoSetInfo, UINT32 u4Size)
{
    MT_RESULT_T i4Ret = MTR_NOT_SUPPORTED;
    
    switch ((MTVDO_VDO_TYPE_T) u4VdoType)
    {
        case VDO_SET_GET_3D_IMG_SAFTY:
            {
                i4Ret = _MTVDO_Set3DImgSaftyLvl((UINT32 *) pkvVdoSetInfo,  u4Size);
            }
            break;
        case VDO_SET_PENTOUCH_MODE:
            i4Ret = _MTVDO_SetPentouchMode((UINT32 *) pkvVdoSetInfo,  u4Size);
            break;
        default:
            i4Ret = MTR_NOT_SUPPORTED;
            break;
    }
    
    return i4Ret;
}

static MT_RESULT_T _MTVDO_GetCmd(UCHAR ucVdpId, UINT32 u4VdoType, void* pkvVdoGetInfo, UINT32 u4Size)
{
    MT_RESULT_T i4Ret = MTR_NOT_SUPPORTED;
    
    switch ((MTVDO_VDO_TYPE_T) u4VdoType)
    {    
        case VDO_GET_SCAN_MODE:
        {
            i4Ret = _MTVDO_GetScanMode((UINT32*) pkvVdoGetInfo,  u4Size);
        }    
            break; 
        case VDO_GET_3D_FMT_CAP:
        {
            i4Ret = _MTVDO_Get3DFmtCap(ucVdpId, (UINT32 *) pkvVdoGetInfo,  u4Size);
        }                
            break;          
        case VDO_GET_3D_CTRL_CAP:
        {
            i4Ret = _MTVDO_Get3DCtrlCap(ucVdpId, (UINT32 *) pkvVdoGetInfo,  u4Size);
        }        
            break;
        case VDO_SET_GET_3D_IMG_SAFTY:
        {
            i4Ret = _MTVDO_Get3DImgSaftyLvl((UINT32 *) pkvVdoGetInfo,  u4Size);
        }          
            break;          
        case VDO_GET_3D_NAV_MODE: 
        {
            i4Ret = _MTVDO_Get3DNavMode((UINT32 *) pkvVdoGetInfo,  u4Size);
        }          
            break;          
        case VDO_GET_3D_DONE_BY_MJC: 
        {
            i4Ret = _MTVDO_Get3DDoneByMJC((UINT32 *) pkvVdoGetInfo,  u4Size);
        }          
            break;             
        case VDO_GET_MIN_SRC_REGION_WIDTH: 
        {
            i4Ret = _MTVDO_GetMinSrcRegionWidth((UINT32 *) pkvVdoGetInfo,  u4Size);
        }          
            break;          
        case VDO_GET_MIN_SRC_REGION_HEIGHT: 
        {
            i4Ret = _MTVDO_GetMinSrcRegionHeight((UINT32 *) pkvVdoGetInfo,  u4Size);
        }          
            break;         
        case VDO_GET_TDTV_GETTAG3D: 
        {
            i4Ret = _MTVDO_TDTV_GetTag3D(ucVdpId, (MTTDTV_TAG3D_TYPE_T  *) pkvVdoGetInfo,  u4Size);
        }
            break;          
        case VDO_GET_TDTV_GETDECTAG3D: 
        {
            i4Ret = _MTVDO_TDTV_GetDecTag3D((MTAL_IOCTL_2ARG_T *) pkvVdoGetInfo,  u4Size);
        }
            break;                 
        case VDO_GET_MUTE_STATUS:
            i4Ret = _MTVDO_GetVideoMuteStatus(ucVdpId, (UINT32*) pkvVdoGetInfo, u4Size);
            break;
        default:
            i4Ret = MTR_NOT_SUPPORTED;
            break;
    }
    
    return i4Ret;
}

static MT_RESULT_T _MTVDO_Set(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_VDO_TYPE_T eVdoType;
    void* pkvVdoSetInfo = NULL;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;
        
    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    eVdoType = (MTVDO_VDO_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvVdoSetInfo = kmalloc(u4Size, GFP_KERNEL);

    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdoSetInfo, u4Size);

    eRet = (MT_RESULT_T) _MTVDO_SetCmd(eVdpId, eVdoType, pkvVdoSetInfo, u4Size);

    kfree(pkvVdoSetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDO_Get(unsigned long arg)
{
    MTAL_IOCTL_4ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    MTVDO_PQ_TYPE_T eVdoType;
    void* pkvVdoGetInfo;
    UINT32 u4Size = 0;
    MT_RESULT_T eRet = MTR_NOT_SUPPORTED;

    USR_SPACE_ACCESS_VALIDATE_4ARG(arg);
    COPY_FROM_USER_4ARG(arg, rArg);
    
    eVdpId = (MTVDO_PATH_T) rArg.ai4Arg[0];
    eVdoType = (MTVDO_VDO_TYPE_T) rArg.ai4Arg[1];
    u4Size = (UINT32) rArg.ai4Arg[3];

    USR_SPACE_ACCESS_VALIDATE_ARG_K(rArg.ai4Arg[2], u4Size);
    pkvVdoGetInfo = kmalloc(u4Size, GFP_KERNEL);

    COPY_FROM_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdoGetInfo, u4Size);

    eRet = _MTVDO_GetCmd(eVdpId, eVdoType, pkvVdoGetInfo, u4Size);

    COPY_TO_USER_DATA_ALLOC(rArg.ai4Arg[2], pkvVdoGetInfo, u4Size);    

    kfree(pkvVdoGetInfo);

    return eRet;
}

static MT_RESULT_T _MTVDO_GetVideoDone(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    MTVDO_PATH_T eVdpId;
    UINT8* pu1VideoDone;
    UINT8  u1VideoDone;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);
    eVdpId = (MTVDO_PATH_T)rArg.ai4Arg[0];
    pu1VideoDone = (UINT8*)rArg.ai4Arg[1];

    u1VideoDone = (UINT8)fgIsVideoDone(eVdpId);   

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1VideoDone, UINT8);
    COPY_TO_USER_ARG(pu1VideoDone, u1VideoDone, UINT8);

    MTVDO_PRINT(" - u1VideoDone = %d\n", (int)u1VideoDone);
    return MTR_OK;
}

static MT_RESULT_T _MTVDO_3DPanelFormatChg(unsigned long arg)
{
#ifdef CC_SCPOS_3DTV_SUPPORT
	UINT8 type;
	type = (UINT8)arg;
	
	vDrvDisp3DModeChg(type);
#endif
    return MTR_OK;
}

//mw_if porting end
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

int mtal_ioctl_mtvdo(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg)
{
    int i4Ret = 0;

    switch (cmd)
    {
        case MTAL_IO_VDO_INIT:
            return _MTVDO_Init();
        case MTAL_IO_VDO_STOP:
            return _MTVDO_Stop();
        case MTAL_IO_VDO_RESET:
            return _MTVDO_Reset( arg);
        case MTAL_IO_VDO_SET_ENABLE:
            return _MTVDO_SetEnable( arg);
        case MTAL_IO_VDO_GET_ENABLE:
            return _MTVDO_GetEnable( arg);
        case MTAL_IO_VDO_SET_MODE:
            return _MTVDO_SetMode( arg);
        case MTAL_IO_VDO_GET_MODE:
            return _MTVDO_GetMode( arg);
        case MTAL_IO_VDO_SET_FREEZE:
            return _MTVDO_SetFreeze( arg);
        case MTAL_IO_VDO_GET_FREEZE:
            return _MTVDO_GetFreeze( arg);
        case MTAL_IO_VDO_SET_NONLINEAR:
            return _MTVDO_SetNonLinearMode(arg);
        case MTAL_IO_VDO_SET_DOTBYDOT:
            return _MTVDO_SetDotByDot(arg);
        case MTAL_IO_VDO_SET_ARG:
            return _MTVDO_SetArg( arg);
        case MTAL_IO_VDO_GET_ARG:
            return _MTVDO_GetArg( arg);
        case MTAL_IO_VDO_SET_OVERSCAN:
            return _MTVDO_SetOverScan( arg);
        case MTAL_IO_VDO_GET_OVERSCAN:
            return _MTVDO_GetOverScan( arg);
        case MTAL_IO_VDO_SET_SRCREGION:
            return _MTVDO_SetSrcRegion( arg);
        case MTAL_IO_VDO_GET_SRCREGION:
            return _MTVDO_GetSrcRegion( arg);
        case MTAL_IO_VDO_SET_OUTREGION:
            return _MTVDO_SetOutRegion( arg);
        case MTAL_IO_VDO_GET_OUTREGION:
            return _MTVDO_GetOutRegion( arg);
        case MTAL_IO_VDO_SET_BG:
            return _MTVDO_SetBg( arg);
        case MTAL_IO_VDO_GET_BG:
            return _MTVDO_GetBg( arg);
        case MTAL_IO_VDO_REGCB:
            return _MTVDO_RegCbFunc( arg);
        case MTAL_IO_VDO_SET_GAMEMODE:
            return _MTVDO_SetGameMode( arg);
        case MTAL_IO_VDO_SET_3DGAMEMODE:
            return _MTVDO_Set3DGameMode( arg);
        case MTAL_IO_VDO_GET_UNMUTE:
            return _MTVDO_GetUnMuteStatus( arg);
        case MTAL_IO_VDO_GET_FRAMEBUFFER_AVG:
            return _MTVDO_Scalar_GetBufferAverage( arg);
        case MTAL_IO_VDO_GET_FRAMEBUFFER_ONOFF:
            return _MTVDO_Scalar_EnableGetBuffer(arg);
        case MTAL_IO_VDO_GET_TIMINGINFO:
            return _MTVDO_GetTimingInfo(arg);
        case MTAL_IO_VDO_GET_TIMINGINFO_BY_DEC:
            return _MTVDO_GetTimingInfoByDec(arg);
        case MTAL_IO_VDO_GET_ASPECTRATIO:
            return _MTVDO_GetAspectRatio(arg);
        case MTAL_IO_VDO_SET_FACTORY_MODE:
            return _MTVDO_SetFactoryMode( arg);  
        case MTAL_IO_VDO_SET_AUTOMUTE_ONOFF:
            return _MTVDO_SetAutoMuteOnOff( arg);
		case MTAL_IO_VDO_SET_AUTOBLK_ONOFF:
            return _MTVDO_SetAutoBlkOnOff( arg);
        case MTAL_IO_VDO_SET_SCALER_PIXELBASED: 
            return _MTVDO_SetScalerPixelBased(arg);
        case MTAL_IO_VDO_SET_XVYCC_INNER_PTGEN:
            return _MTVDO_SetxvYCCInnerPatGen(arg);
        case MTAL_IO_VDO_SET_PE_UI_MIN_MAX_DFT:
            return _MTVDO_SetPeUiRangeParam(arg);		
        case MTAL_IO_VDO_GET_PE_UI_MIN_MAX_DFT:
            return _MTVDO_GetPeUiRangeParam(arg);                
        case MTAL_IO_VDO_SET_SCANMODE:
            return _MTVDO_SetScanMode(arg);	
        case MTAL_IO_VDO_SET_DS_FACTOR:
            return _MTVDO_SetDSFactor( arg);  		    
        case MTAL_IO_VDO_SET_PQ:
            return _MTVDO_SetPQ(arg);
        case MTAL_IO_VDO_GET_PQ:
            return _MTVDO_GetPQ(arg);
        case MTAL_IO_VDO_GET_3D_TAG:
            return _MTVDO_GET_3D_TAG(arg);
        case MTAL_IO_VDO_SET_3D_PANEL:
            return _MTVDO_Set3DPanel(arg);
        case MTAL_IO_VDO_SET_OUTPUTWINDOW:
            return _MTVDO_SetOutputWindow(arg);
        case MTAL_IO_VDO_SRM_SETTVMODE:
            return _MTVDO_SRM_SetTvMode(arg);
        case MTAL_IO_VDO_GetPlaneWH:
            return _MTVDO_GetPlaneWH(arg);	
        case MTAL_IO_VDO_SET_MJC_INITIALIZE:
            return _MTVDO_SetMJCInitialize(arg);
        case MTAL_IO_VDO_SET_MJC_RESETSW:
            return _MTVDO_SetMJCResetSW(arg);
        case MTAL_IO_VDO_GET_MJC_FW_VER:
            return _MTVDO_GetMJCFWVer(arg);
        case MTAL_IO_VDO_SET_MJC_VIDEO_BLOCK:
            return _MTVDO_SetMJCVideoBlock(arg);
        case MTAL_IO_VDO_SET_MJC_EFCT_LVL:
            return _MTVDO_SetMJCEffectLevel( arg);
        case MTAL_IO_VDO_SET_MJC_JUDDER_LVL:
            return _MTVDO_SetMJCJudderLevel(arg);
        case MTAL_IO_VDO_SET_MJC_BLUR_LVL:
            return _MTVDO_SetMJCBlurLevel(arg);
        case MTAL_IO_VDO_SET_MJC_DEMO_MODE:
            return _MTVDO_SetMJCDemoMode( arg);
        case MTAL_IO_VDO_SET_MJC_BYPASS_WINDOW:
            return _MTVDO_SetMJCBypassWindow(arg);
        case MTAL_IO_VDO_SET_MJC_DEMO_BAR:
            return _MTVDO_SetMJCDemoBar(arg);
        case MTAL_IO_VDO_SET_MJC_MC_ONOFF:
            return _MTVDO_SetMJCMCOnOff(arg);
        case MTAL_IO_VDO_SET_MJC_MC:
            return _MTVDO_SetMJCMC(arg);
        case MTAL_IO_VDO_GET_3D_NAVI_DETECT:
            return _MTVDO_GET_3D_NAVI_DETECT(arg);
        case MTAL_IO_VDO_SET_3D_NAVI_MODE:
            return _MTVDO_Set3DNaviMode(arg);
        case MTAL_IO_VDO_SET_3D_DEPTH_CTRL_ON_OFF:
            return _MTVDO_Set3DDCOnOff(arg);
        case MTAL_IO_VDO_DI_GET_DRAM_PIXELS:
            return _MTVDO_DI_GetDramPixels(arg);    
        case MTAL_IO_VDO_DI_WRITE_DRAM_PIXELS:
            return _MTVDO_DI_WriteDramPixels(arg);   	                       
        case MTAL_IO_VDO_DI_GET_FORMAT_INFO:
            return _MTVDO_DI_GetFormatInfo(arg);    
        case MTAL_IO_VDO_SET_DI_FREEZE_ONOFF:
            return _MTVDO_SetFreezeDIFrameBuffer(arg);
        case MTAL_IO_VDO_SET_FBM_MODE:
            return _MTVDO_SET_FBMMode(arg);
	   case MTAL_IO_VDO_GET_FBM_MODE:
            return _MTVDO_GET_FBMMode(arg);
        case MTAL_IO_VDO_GET_NET_SWAP_BUFFER:
            return _MTVDO_GET_NET_SWAP_Buffer(arg);
        case MTAL_IO_VDO_DI_ENABLE_DUMP_DRAM:
            return _MTVDO_EnableDIDramDump(arg);
        case MTAL_IO_VDO_GET_SBSCONTENT:
            return _MTVDO_GetSBSContent(arg);
        case MTAL_IO_VDO_SET_FRCMUTE_ONOFF:
            return _MTVDO_SetFRCMuteOnOff(arg);
        case MTAL_IO_VDO_GETFRAMERATE:
            return _MTVDO_GetFrameRate(arg);     
        case MTAL_IO_VDO_GET_3D_LR_ORDER:
            return _MTVDO_GET_3D_LR_ORDER(arg);
        case MTAL_IO_VDO_ENABLE_OD:
            return _MTVDO_EnableOverDriver(arg);
        case MTAL_IO_VDO_ENABLE_PCID:
            return _MTVDO_EnablePCID(arg);      
        case MTAL_IO_VDO_GETINPUTFRAMERATE:
            return _MTVDO_GetInputFrameRate(arg);
        case MTAL_IO_VDO_GETFRAMEDELAY:
            return _MTVDO_GetFrameDelay(arg);
        case MTAL_IO_VDO_SET_3D_NAVI_PARAM:
            return _MTVDO_SET_3D_NAVI_PARAM(arg);
        case MTAL_IO_VDO_SET_VGA_MODE:
            return _MTVDO_SetVgaMode(arg);
        case MTAL_IO_VDO_SET_HDMI_MODE:
            return _MTVDO_SetHDMIMode(arg);
        case MTAL_IO_VDO_SET_3DFP_ONFF:
            return _MTVDO_Set3DFramePacking(arg);
        case MTAL_IO_VDO_SET_SRAMDRAMMODE_ONOFF:
            return _MTVDO_SetSRAMDramMode(arg);
        case MTAL_IO_VDO_SET_FLIPMIRROR:
            return _MTVDO_SetFlipMirror(arg);
        case MTAL_IO_VDO_SET_3D_AUTO_DISPLAY:
            return _MTVDO_SetTDTV3DAutoDisplay(arg);
        case MTAL_IO_VDO_BYPASS_PREPROC:
            return _MTVDO_BYPASS_PREPROC(arg);    	
        case MTAL_IO_VDO_SET_LCDIM_INITIALIZE:
            return _MTVDO_SetLcdimInitialize(arg);
        case MTAL_IO_VDO_SET_LCDIM_ONOFF:
            return _MTVDO_SetLcDimEn(arg);
        case MTAL_IO_VDO_SET_LCDS_ONOFF:
            return _MTVDO_SetLcDSEn(arg);
        case MTAL_IO_VDO_SET_LCDIM_LUTIDX:
            return _MTVDO_SetLcDimLUTIdx(arg);          
        case MTAL_IO_VDO_SET_LCDIM_DEMO:
            return _MTVDO_SetLcDimDemoMode(arg);          
        case MTAL_IO_VDO_SET_SPI_COMMAND:
            return _MTVDO_SetLcDimSpiCommand(arg);
        case MTAL_IO_VDO_SET_OD_TABLE:
            return _MTVDO_SetODTable(arg);
        case MTAL_IO_VDO_SRM_RESUME:
            return _MTVDO_SRMResume(arg);
        case MTAL_IO_VDO_SET_DOWN_BW_MODE:
            return _MTVDO_SetDownBwMode(arg);
        case MTAL_IO_VDO_SET_COND_FORCE_UNMUTE:
            return _MTVDO_SetCondForceUnmute(arg);
        case MTAL_IO_VDO_GET_LBOX_ENABLE:
            return _MTVDO_GetLBoxEnable(arg);
        case MTAL_IO_VDO_SET_LBOX_ENABLE:
            return _MTVDO_SetLBoxEnable(arg);
        case MTAL_IO_VDO_SET_LBOX_CONFIG:
            return _MTVDO_SetLBoxConfig(arg);
        case MTAL_IO_VDO_GET_CAPABILITY:
            return _MTVDO_GetCapability(arg);
        case MTAL_IO_VDO_SET_SRM_MODE:
            return _MTVDO_SetSrmMode(arg);
        case MTAL_IO_VDO_UPDATE_CMD_DELAY:
            return _MTVDO_UpdateCmdDelay(arg);
        case MTAL_IO_VDO_GET_CMD_DELAY:
            return _MTVDO_GetCmdDelay(arg);
        case MTAL_IO_VDO_SET_VIDEO_MUTE:
            return _MTVDO_SetVideoMute(arg);
        case MTAL_IO_VDO_SET_DYNAMIC_SCALE:
            return _MTVDO_SetDynamicScale(arg);
        case MTAL_IO_VDO_GET_DYNAMIC_SCALE:
            return _MTVDO_GetDynamicScale(arg);
        case MTAL_IO_VDO_GET_DRAM_INFO:
            return _MTVDO_GetDramInfo(arg);
        case MTAL_IO_VDO_GET_SCALER_DISP_MODE:
            return _MTVDO_GetScalerDispMode(arg);
        case MTAL_IO_VDO_GET_MIRROR_FILP:
            return _MTVDO_GetMirrorFlip(arg);
        case MTAL_IO_VDO_SET_MIRROR_FILP:
            return _MTVDO_SetMirrorFlip(arg);
        case MTAL_IO_VDO_EXTMJCINIT:
            return _MTVDO_ExtmjcInit();
        case MTAL_IO_SET_DISPLAY_OFF:
            return _MTVDO_SetDisplayOff(arg);
		case MTAL_IO_VDO_SET_3D_PATTERN_MODE:
			return _MTVDO_Set3DpatternMode(arg);	
        case MTAL_IO_SET_DISPLAY_OFF_EXT:
            return _MTVDO_SetDisplayOff_Ext(arg);
        case MTAL_IO_VDO_SET_VDS_FACTOR:
            return _MTVDO_SetVerticalDSFactor( arg);
		case MTAL_IO_VDO_SET_MONOUT_ENABLE:
			return _MTVDO_SetMonOutEnable(arg);
		case MTAL_IO_VDO_GET_MONOUT_ENABLE:
			return _MTVDO_GetMonOutEnable(arg);
		case MTAL_IO_VDO_SET_MONOUT_SOURCE:
			return _MTVDO_SetMonOutSource(arg);			
		case MTAL_IO_VDO_GET_MONOUT_SOURCE:
			return _MTVDO_GetMonOutSource(arg);	
		case MTAL_IO_VDO_SET_MONOUT_OUTPUT_WINDOW:
			return _MTVDO_SetMonOutOutputWindow(arg);
		case MTAL_IO_VDO_GET_MONOUT_OUTPUT_WINDOW:
			return _MTVDO_GetMonOutOutputWindow(arg);
        case MTAL_IO_VDO_SET_MONOUT_DOT_BY_DOT_OUTPUT:
			return _MTVDO_SetMonDotByDotOutput(arg);
        case MTAL_IO_VDO_SET_MONOUT_OSDSEL:
            return _MTVDO_SetMonOutOSDSel(arg);
		case MTAL_IO_VDO_SET_OSDSEL: 
            return _MTVDO_SetOSDSel(arg);
        case MTAL_IO_VDO_SET_VIDEO_PIXEL_SHIFT:
            return _MTVDO_SetVideoPixelShift(arg);
#if defined(CC_MT5398) || defined(CC_MT5880) || defined(CC_MT5860)|| defined(CC_MT5881) || defined(CC_MT5399) || defined(CC_MT5890) || defined(CC_MT5882) 
        case MTAL_IO_VDO_SET_SUB_CLONE_MODE:
            return _MTVDO_SetSubCloneMode(arg);
#endif
        case MTAL_IO_VDO_SET_BROWSER_INPUT_ENABLE:
			return _MTVDO_SetBrowserInput(arg);
		case MTAL_IO_VDO_SET_SPE_IPTVS_ENABLE:
			return _MTVDO_SetSpeIPTVS(arg);
         case MTAL_IO_VDO_SET:
            i4Ret = _MTVDO_Set(arg);
            break;
            
        case MTAL_IO_VDO_GET:
            i4Ret = _MTVDO_Get(arg);
            break;   
		case MTAL_IO_VDO_GET_MUTE_MODULE_INFO:
			i4Ret=_MTVDO_GetMuteModuleInfo(arg);
			break;	
	    case MTAL_IO_VDO_SET_IMT_FLAG:
			i4Ret=_MTVDO_SetIMITFlag(arg);
			break;
			case MTAL_IO_VDO_GET_VIDEO_DONE:
				i4Ret= _MTVDO_GetVideoDone(arg);
				break;
			case MTAL_IO_VDO_SET_3D_PANEL_FORMAT_CHG:
				i4Ret=	_MTVDO_3DPanelFormatChg(arg);
				break;

        default:
            break;
    }
    return i4Ret;
}



