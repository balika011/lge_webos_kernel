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
 * $RCSfile: drv_resume.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_resume.c
 *  This file implements for full driver resume.
 */
#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/opm_device.h>

#if defined(CC_SUPPORT_STR)

//panel                 
#include "nptv_if.h"    
#include "drv_vdoclk.h" 
#include "drv_display.h"
#include "drv_video.h"  

#include "x_mid.h"
#include "hw_hdmi.h"
#include "hw_lvds.h"
#include "hw_mempll.h"
#include "hw_cpupll.h"
#include "hw_ifpll.h"
#include "hw_pll_group.h"



extern void vDrvODSuspend(void);
extern void vDrvODResume(void);
extern void vDrvMIBSuspend(void);
extern void vDrvMIBResume(void);
extern void vDrvScposSuspend(void);
extern void vDrvScposResume(void);
extern void AUD_pm_str_suspend(void);
extern void AUD_pm_str_resume(void);
#ifdef CC_SUPPORT_VENC
extern void VENC_pm_str_suspend(void);
extern void VENC_pm_str_resume(void);
#endif
extern void OSD_pm_suspend(void);
extern void OSD_pm_resume(void);
extern void GCPU_pm_str_suspend(void);
extern void GCPU_pm_str_resume(void);
extern void GFX_pm_suspend(void);
extern void GFX_pm_resume(void);
extern void DMX_pm_str_suspend(void);
extern void DMX_pm_str_resume(void);
extern void CHIP_CPU_Resume(void);
extern void CHIP_CPU_Suspend(void);
extern void VDEC_pm_str_suspend(void);
extern void VDEC_pm_str_resume(void);
extern void Image_pm_str_suspend(void);
extern void Image_pm_str_resume(void);
extern void Imgrz_pm_str_suspend(void);
extern void Imgrz_pm_str_resume(void);
extern void B2R_pm_str_suspend(void);
extern void B2R_pm_str_resume(void);
extern void SIF_pm_str_suspend(void);
extern void SIF_pm_str_resume(void);
extern void PWM_pm_str_suspend(void);
extern void PWM_pm_str_resume(void);
extern void vDrvPostprocSuspend(void);
extern void vDrvLcDimPQSuspend(void);
extern void vDrvLcdimSpiSuspend(void);
extern void vDrvPostprocResume(void);
extern void vDrvLcDimPQResume(void);
extern void vDrvLcdimSpiResume(void);
extern void TVD_pm_resume(void);
extern void TVD_pm_suspend(void);
extern void ATD_pm_str_resume(void);
extern void ATD_pm_str_suspend(void);
extern void VBI_pm_resume(void);
extern void VBI_pm_suspend(void);
extern void TVE_pm_resume(void);
extern void TVE_pm_suspend(void);
EXTERN void    PCMCIA_pm_str_suspend(void);
EXTERN void    PCMCIA_pm_str_resume(void);
extern void HDMI_pm_suspend(void);
extern void HDMI_pm_resume(void);
#ifdef CC_HDMI_2_0_SUPPORT
extern void HDMI2_pm_suspend(void);
extern void HDMI2_pm_resume(void);
#endif
extern void ADC_pm_resume(void);

//LVDS
extern void vDrvLVDSSuspend(void);
extern void vDrvLVDSResume(void);
extern void vDrvVOPLLSuspend(void);
extern void vDrvVOPLLResume(void);

extern BOOL MID_EnableRegionReadProtect(UINT32 u4Region, BOOL onoff);

extern void vDrvDisplayInit_pm_resume(void);

extern void PDWNC_EnterSuspend(void);
extern void PDWNC_Resume(void);
extern void PDWNC_SetSuspendMode(void);
extern void PDWNC_SetResumeMode(void);

 static int __devinit _mt53xx_STR_probe(struct opm_device *pdev)
{
	printk("_mt53xx_STR_probe\n");
	return 0;
}

static int __devexit _mt53xx_STR_remove(struct opm_device *pdev)
{
	printk("_mt53xx_STR_remove\n");
	return 0;	
}

#define ENABLE_JTAG_TO_DEBUG
#ifdef ENABLE_JTAG_TO_DEBUG
INT32 CLI_Parser(const CHAR *szCmdBuf);
#endif
#define DEBUG_FUNCTION_CALL(x)\
    do{\
	printk(#x"\n");\
	x;\
    } while (0)
extern void mt5396_register_mid_function(unsigned int (*func)(unsigned int));
static int _mt53xx_STR_suspend(struct opm_device *pdev, pm_message_t state)
{
    printk("_mt53xx_STR_suspend\n");
    PDWNC_SetSuspendMode();

#ifdef ENABLE_JTAG_TO_DEBUG
    CLI_Parser("b.jtag 1");
#endif
    DEBUG_FUNCTION_CALL(vDrvLVDSSuspend());
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xff, 0xfe));
    DEBUG_FUNCTION_CALL(bApiVideoMainSubSrc(0xfe, 0xff));
    DEBUG_FUNCTION_CALL(vDrvVOPLLSuspend());
    //DEBUG_FUNCTION_CALL(Tuner_pm_str_suspend());
    DEBUG_FUNCTION_CALL(vDrvODSuspend());
    DEBUG_FUNCTION_CALL(vDrvMIBSuspend());
    DEBUG_FUNCTION_CALL(vDrvPostprocSuspend());
    DEBUG_FUNCTION_CALL(vDrvLcDimPQSuspend());
    DEBUG_FUNCTION_CALL(vDrvLcdimSpiSuspend()); 
    DEBUG_FUNCTION_CALL(AUD_pm_str_suspend());
    DEBUG_FUNCTION_CALL(OSD_pm_suspend());
    DEBUG_FUNCTION_CALL(GFX_pm_suspend());
    DEBUG_FUNCTION_CALL(GCPU_pm_str_suspend());
    DEBUG_FUNCTION_CALL(DMX_pm_str_suspend());
    DEBUG_FUNCTION_CALL(VDEC_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Image_pm_str_suspend());
    DEBUG_FUNCTION_CALL(Imgrz_pm_str_suspend());
    DEBUG_FUNCTION_CALL(TVD_pm_suspend());
    DEBUG_FUNCTION_CALL(ATD_pm_str_suspend());
    DEBUG_FUNCTION_CALL(VBI_pm_suspend());
    DEBUG_FUNCTION_CALL(TVE_pm_suspend());
    DEBUG_FUNCTION_CALL(B2R_pm_str_suspend());
    DEBUG_FUNCTION_CALL(PCMCIA_pm_str_suspend());
    DEBUG_FUNCTION_CALL(HDMI_pm_suspend());
	#ifdef CC_HDMI_2_0_SUPPORT
	DEBUG_FUNCTION_CALL(HDMI2_pm_suspend());
	#endif
    DEBUG_FUNCTION_CALL(SIF_pm_str_suspend());
    DEBUG_FUNCTION_CALL(PWM_pm_str_suspend());
    DEBUG_FUNCTION_CALL(vDrvScposSuspend());
    PDWNC_EnterSuspend();  

    mt5396_register_mid_function(MID_WarmBootRegionProtect);
#if 0
    MID_WarmBootRegionProtect(TRUE);
#endif
        
	return 0;
}

extern void vDrvVideoMuteColor(UINT32 u4VdpId, UINT32 u4MuteColor);
static int _mt53xx_STR_resume(struct opm_device *pdev)
{
    printk("_mt53xx_STR_resume\n");
#if 0
    MID_WarmBootRegionProtect(FALSE);
#endif
    PDWNC_SetResumeMode();

#if 1//tmp close
    DEBUG_FUNCTION_CALL(vDrvScposResume());
    DEBUG_FUNCTION_CALL(vDrvVOPLLResume());
    DEBUG_FUNCTION_CALL(vDrvODResume());
    DEBUG_FUNCTION_CALL(vDrvMIBResume());
    DEBUG_FUNCTION_CALL(vDrvLcDimPQResume());
    DEBUG_FUNCTION_CALL(vDrvLcdimSpiResume());        
    DEBUG_FUNCTION_CALL(vDrvPostprocResume());
    DEBUG_FUNCTION_CALL(AUD_pm_str_resume());
    DEBUG_FUNCTION_CALL(OSD_pm_resume());
    DEBUG_FUNCTION_CALL(GFX_pm_resume());
    DEBUG_FUNCTION_CALL(GCPU_pm_str_resume());
    DEBUG_FUNCTION_CALL(DMX_pm_str_resume());
    DEBUG_FUNCTION_CALL(VDEC_pm_str_resume());
    DEBUG_FUNCTION_CALL(Image_pm_str_resume());
    DEBUG_FUNCTION_CALL(Imgrz_pm_str_resume());
    DEBUG_FUNCTION_CALL(TVD_pm_resume());
    DEBUG_FUNCTION_CALL(SIF_pm_str_resume());
    DEBUG_FUNCTION_CALL(ATD_pm_str_resume());
    //DEBUG_FUNCTION_CALL(Tuner_pm_str_resume());
    DEBUG_FUNCTION_CALL(VBI_pm_resume());
    DEBUG_FUNCTION_CALL(TVE_pm_resume());
    DEBUG_FUNCTION_CALL(B2R_pm_str_resume());
    DEBUG_FUNCTION_CALL(PCMCIA_pm_str_resume());
    DEBUG_FUNCTION_CALL(HDMI_pm_resume());
	#ifdef CC_HDMI_2_0_SUPPORT
	DEBUG_FUNCTION_CALL(HDMI2_pm_resume());
	#endif
    DEBUG_FUNCTION_CALL(PWM_pm_str_resume());

    DEBUG_FUNCTION_CALL(vDrvDisplayInit_pm_resume());
    DEBUG_FUNCTION_CALL(ADC_pm_resume());// please put it after the one who call VOPLL_pm_resume();
    DEBUG_FUNCTION_CALL(vDrvLVDSResume());
    //vDrvVideoMuteColor(SV_VP_MAIN, 0);
#else
    vApiPanelPowerSequence(TRUE); //tmp
#endif
    // send a fake power key
    PDWNC_Resume();
        
    return 0;
}

#if 0
static struct platform_driver _mt53xx_STR_driver =
{
	.probe          = _mt53xx_STR_probe,
	.remove         = _mt53xx_STR_remove,
	.suspend        = _mt53xx_STR_suspend,
	.resume         = _mt53xx_STR_resume,
	.driver		= {
	        .name	= "mt53xx_STR",
		.owner	= THIS_MODULE,
	},
};

static struct platform_device _mt53xx_STR_device = {
	.name			= "mt53xx_STR",
	.id			= 0
};

int  mt53xx_STR_init(void)
{
	printk("mt53xx_STR_init\n");
	platform_device_register(&_mt53xx_STR_device);    
	return platform_driver_register(&_mt53xx_STR_driver);
}
#endif

#if 1 // def CONFIG_OPM
#include <linux/opm_device.h>
#include <linux/err.h>

extern void PDWNC_Reboot(void);
static int opmcls_reboot;
void opmcls_shutdown(struct opm_device *devptr)
{
	printk("opmcls_shutdown\n");
	if(opmcls_reboot)
		PDWNC_Reboot();
}

static struct opm_driver opmcls_driver_reboot = {
	.shutdown	= opmcls_shutdown,
	.driver		= {
		.name	= "opm_sysclass"
	},
};

static struct opm_device *device;

static struct opm_driver _mt53xx_STR_driver = {
	.probe		= _mt53xx_STR_probe,
	.remove		= _mt53xx_STR_remove,
	.suspend        = _mt53xx_STR_suspend,
	.resume         = _mt53xx_STR_resume,
	.driver		= {
		.name	= "mt53xx_STR",
		.owner	= THIS_MODULE,
	},
};
static struct opm_device *device;

int  mt53xx_STR_init(void)
{
	int err;

	if ((err = opm_driver_register(&_mt53xx_STR_driver)) < 0)
		return err;
	if ((err = opm_driver_register(&opmcls_driver_reboot)) < 0)
		return err;
	device = opm_device_register_simple("mt53xx_STR",0,ORDER_IGNORE);
	if (IS_ERR(device))
		return -ENODEV;						 
	return 0;
}
#endif

#endif
