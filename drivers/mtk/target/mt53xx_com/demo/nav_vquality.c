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
 * $RCSfile: nav_vquality.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_vquality.c.h
 *  This header file declares public function prototypes of widget.
 */
 
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN

#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
//#include "x_hal_5371.h"
#include "x_hal_926.h"

#include "nptv_if.h"
#include "vdp_drvif.h"
#include "osd_if.h"
#include "sv_const.h"
#include "video_def.h"

LINT_EXT_HEADER_END

#include "widget_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_vquality.h"


#include "nav_debug.h"
#include "x_drv_if.h"

static UINT32 _u4NrMode = 0;
static DRV_VDO_QUALITY_T _rQuality;

// Default video qauality table
#ifndef __MODEL_slt__
static NAV_VQ_SETTING_T _arDefaultColorSetting[E_NAV_VS_MAX] =
{
//	Bright,	Contr,	Hue,	Sat,		Sharp,	CTI,		NR,		AL,		SCE, 	BWS	, Backlight, AdaptiveBacklight 
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // analog tuner
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, //cvbs1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // cvbs2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // cvbs3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // cvbs4	
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // s1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // s2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // s3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // ypbpr1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1,		0x1, 	0x1, 	100, 	1}, // ypbpr2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1,		0x1, 	100, 	1}, // hdmi1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1,		0x1, 	100, 	1}, // hdmi2
	{0x32, 	0x32, 	0, 	0x32, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // vga
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // dtv
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x1, 	0x0, 	0x1, 	0x1, 	0x1, 	100, 	1}, // 1394
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // SCART
	{0x32, 	0x32, 	0, 	0x32, 	0x1, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	1}, //DVI	
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x1, 	0x0, 	0x0, 	0x1, 	0x1, 	100, 	1}, //E_NAV_VS_YPbPr3
	{0x32, 	0x32, 	0, 	0x32, 	0x1, 	0x0, 	0x0, 	0x0,		0x0, 	0x0, 	100, 	1}, //E_NAV_VS_YPbPr4
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1,		0x1, 	100, 	1}, // hdmi3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x0, 	0x1, 	0x1,		0x1, 	100, 	1}, // hdmi4
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // SCART2	
};
#else
// Default video qauality table
static NAV_VQ_SETTING_T _arDefaultColorSetting[E_NAV_VS_MAX] =
{
//	Bright,	Contr,	Hue,	Sat,		Sharp,	CTI,		NR,		AL,		SCE, 	BWS	, Backlight, AdaptiveBacklight 
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // analog tuner
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, //cvbs1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // cvbs2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // cvbs3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x1, 	0x1, 	0x1, 	0x1, 	0x1, 	100, 	1}, // cvbs4	
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // s1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // s2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // s3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // ypbpr1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0,		0x0, 	0x0, 	100, 	0}, // ypbpr2
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	0}, // hdmi1
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	0}, // hdmi2
	{0x32, 	0x32, 	0, 	0x32, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // vga
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // dtv
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // 1394
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // SCART
	{0x32, 	0x32, 	0, 	0x32, 	0x1, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	0}, //DVI	
	{0x32, 	0x32, 	0, 	0x32, 	0x2, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, //E_NAV_VS_YPbPr3
	{0x32, 	0x32, 	0, 	0x32, 	0x1, 	0x0, 	0x0, 	0x0,		0x0, 	0x0, 	100, 	0}, //E_NAV_VS_YPbPr4
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	0}, // hdmi3
	{0x32, 	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0,		0x0, 	100, 	0}, // hdmi4
	{0x32,	0x32, 	0, 	0x32, 	0x4, 	0x0, 	0x0, 	0x0, 	0x0, 	0x0, 	100, 	0}, // SCART2	
};
#endif
EXTERN HANDLE_T hVgaSema;

EXTERN UINT16 wDrvVGAGetClock(void);
EXTERN UINT8 u1DrvVGAGetPhase(void);
EXTERN UINT16 wDrvVGAGetHPosition(void);
EXTERN UINT16 wDrvVGAGetVPosition(void);
EXTERN UINT8 fgApiVideoVgaSetClock(UINT16 wValue);
EXTERN UINT8 fgApiVideoVgaSetPhase(UINT8 bValue);
EXTERN UINT8 fgApiVideoAutoColor(void); 
EXTERN UINT8 fgApiVideoVgaAuto(void);



INT32 _SetVGAPhase(UINT32 u4VdpId, INT32 i4Phase);
INT32 _SetVGAAuto(UINT32 u4VdpId);
INT32 _SetVGAClock(UINT32 u4VdpId, INT32 i4Clock);
INT32 _SetVGAAutoColor(UINT32 u4VdpId);

static INT32 _SetBrightness(UINT32 u4VdpId, INT32 i4Value)
{
    if((i4Value >= 0) && (i4Value <= 100))
    {	
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_BRIGHTNESS, i4Value));   
        _rQuality.u1Brightness = i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_BRIGHTNESS, &_rQuality);          
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
static INT32 _SetContrast(UINT32 u4VdpId, INT32 i4Value)
{
    if((i4Value >= 0) && (i4Value <= 100))
    {
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_CONTRAST, i4Value));
        _rQuality.u1Contrast = i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_CONTRAST, &_rQuality);             
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
static INT32 _SetHue(UINT32 u4VdpId, INT32 i4Value)
{
    if((i4Value >= -50) && (i4Value <= 50))
    {
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_HUE, i4Value + 32));
        _rQuality.i1Hue = i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_HUE, &_rQuality);             
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;

}

static INT32 _SetSaturation(UINT32 u4VdpId, INT32 i4Value)
{
    if((i4Value >= 0) && (i4Value <= 100))
    {
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_SATURATION, i4Value));
        _rQuality.u1Saturation= i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_SATURATION, &_rQuality);             
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
static INT32 _SetSharpness(UINT32 u4VdpId, INT32 i4Value)
{
    if((i4Value >= 0) && (i4Value <= 20))
    {
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_SHARPNESS, i4Value));
        _rQuality.u1Sharpness = i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_SHARPNESS, &_rQuality);             
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

static INT32 _SetCTI(UINT32 u4VdpId, INT32 i4Value)
{
	if((i4Value >= 0) && (i4Value <= 3))
	{
		//UNUSED(PE_SetArg(u4VdpId, PE_ARG_CTI, i4Value));
        _rQuality.u1Cti = i4Value;
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_CTI, &_rQuality);   		
		return (INT32)E_NAV_MSG_OK;
	}
	return (INT32)E_NAV_MSG_NOT_SUPPORT;

}

static INT32 _SetAdaptiveLuma(UINT32 u4VdpId, UINT32 u4Enable)
{
    if(u4VdpId != NAV_VDP_MASTER)
    {
        return (INT32)E_NAV_MSG_OK;
    }
    
    if(u4Enable)
    {	    
        _rQuality.u1AdaptiveLuma = 1;
    }
    else
    {
        _rQuality.u1AdaptiveLuma = 0;
    }

    DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_ADAPTIVE_LUMA, &_rQuality);       
    return (INT32)E_NAV_MSG_OK;
}

static INT32 _SetSCE(UINT32 u4VdpId, UINT32 u4Enable)
{
    if(u4Enable == 1)
    {
        UNUSED(PE_SetArg(u4VdpId, PE_ARG_SCE, u4Enable));
    }
    else if(u4Enable == 0)
    {
        UNUSED(PE_SetArg(u4VdpId, PE_ARG_SCE, u4Enable));
    }    

    return (INT32)E_NAV_MSG_OK;
}

static INT32 _SetBacklight(UINT32 u4VdpId, INT32 i4Value)
{
    
	if((i4Value >= 0) && (i4Value <= 100))
	{
		if(PE_SetArg(u4VdpId, PE_ARG_BACK_LIGHT_LVL, i4Value) == VDP_SET_OK)
		{			
			return (INT32)E_NAV_MSG_OK;
		}
	}
	
	return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
#if 0
static INT32 _SetAdaptiveBacklight(UINT32 u4VdpId, BOOL fgEnable)
{
    
	if(PE_SetArg(u4VdpId, PE_ARG_ADAPTIVE_BACK_LIGHT, (UCHAR)fgEnable) == VDP_SET_OK)
	{			
		return (INT32)E_NAV_MSG_OK;
	}
	
	return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
#endif

INT32 NAV_VQ_Reset(UINT32 u4VdpId)
{
    INT32 i4Ret;
    UINT32 u4DefaultNR;

    WIDGET_MENUITEM_RANGEDVALUE_T* prBrightnessItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prContrastItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prHueItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prSaturationItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prSharpnessItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prCTIItem;
    WIDGET_MENUITEM_BINARYVALUE_T* prAdaptiveLumaItem;
    WIDGET_MENUITEM_BINARYVALUE_T* prSCEItem;
    //WIDGET_MENUITEM_BINARYVALUE_T* prBWSItem;
    WIDGET_MENUITEM_OPTION_T* prNRItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prPhaseItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prClockItem;
    //WIDGET_MENUITEM_RANGEDVALUE_T* prBacklightItem;

    
    prBrightnessItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Brightness");
	ASSERT(prBrightnessItem != NULL);
    prContrastItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Contrast");
	ASSERT(prContrastItem != NULL);
    prHueItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Hue");
	ASSERT(prHueItem != NULL);
    prSaturationItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Saturation");
	ASSERT(prSaturationItem != NULL);
    prSharpnessItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Sharpness");
	ASSERT(prSharpnessItem != NULL);
    prCTIItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "CTI");
	ASSERT(prCTIItem != NULL);
    prAdaptiveLumaItem = (WIDGET_MENUITEM_BINARYVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Adaptive Luma");
	ASSERT(prAdaptiveLumaItem != NULL);
    prSCEItem = (WIDGET_MENUITEM_BINARYVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "SCE");
	ASSERT(prSCEItem != NULL);
    //VERIFY(prBWSItem = (WIDGET_MENUITEM_BINARYVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "BWS"));
    prNRItem = (WIDGET_MENUITEM_OPTION_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "NR");
	ASSERT(prNRItem != NULL);
    prPhaseItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA Phase");
	ASSERT(prPhaseItem != NULL);
    prClockItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA Clock");
	ASSERT(prClockItem != NULL);
    //VERIFY(prBacklightItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "PICTURE", "Backlight"));
    u4DefaultNR = (UINT32)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4NR;
	
    i4Ret = _SetBrightness(u4VdpId, prBrightnessItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Brightness);
    i4Ret = _SetContrast(u4VdpId, prContrastItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Contrast);
    i4Ret = _SetHue(u4VdpId,  prHueItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Hue);
    i4Ret = _SetSaturation(u4VdpId, prSaturationItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Saturation);
    i4Ret = _SetSharpness(u4VdpId, prSharpnessItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Sharpness);
    
    i4Ret = _SetCTI(u4VdpId, prCTIItem->i4Value = _arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4CTI);
    prAdaptiveLumaItem->fgOn = (BOOL)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4AdaptiveLuma;
    i4Ret = _SetAdaptiveLuma(u4VdpId, (UINT32)prAdaptiveLumaItem->fgOn);
    prSCEItem->fgOn = (BOOL)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4SCE;
    i4Ret = _SetSCE((UINT8)u4VdpId, (UINT8)prSCEItem->fgOn);
    // Obsolete
    /*
    prBWSItem->fgOn = (BOOL)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4BWS;
    i4Ret = _SetBWS(u4VdpId, (UINT32)prBWSItem->fgOn);
    */
#ifndef __MODEL_slt__
    i4Ret = _SetVGAPhase(u4VdpId, (INT32)prPhaseItem->i4Value);
    i4Ret = _SetVGAClock(u4VdpId, (INT32)prClockItem->i4Value);
#endif
// Intentionally disabled
//    i4Ret = _SetAdaptiveBacklight((UINT8)u4VdpId, (BOOL)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4AdaptiveBacklight);

    i4Ret = _SetBacklight((UINT8)u4VdpId, (UINT8)_arDefaultColorSetting[arVideoPath[u4VdpId].u4Src].u4Backlight); 
    
    
    VERIFY(NAV_VQ_SetNR((INT32)u4DefaultNR) == (INT32)E_NAV_MSG_OK);
    /*
    if(arVideoPath[u4VdpId].u4Src == E_NAV_VS_VGA)
    {
    	*((volatile unsigned int *)0x20022584) = *((unsigned int*)0x30000000);
    }
    */
    if(u4DefaultNR == 0)
    {
        prNRItem->prStrList->prCurr = (WIDGET_STRINGLIST_ELEMENT_T*)(void*)WIDGET_SearchListElement("OFF", (WIDGET_LIST_T*)(void*)prNRItem->prStrList);
    }
    else if(u4DefaultNR == 1)
    {
        prNRItem->prStrList->prCurr = (WIDGET_STRINGLIST_ELEMENT_T*)(void*)WIDGET_SearchListElement("LOW", (WIDGET_LIST_T*)(void*)prNRItem->prStrList);
    }
    else if(u4DefaultNR == 2)
    {
        prNRItem->prStrList->prCurr = (WIDGET_STRINGLIST_ELEMENT_T*)(void*)WIDGET_SearchListElement("MID", (WIDGET_LIST_T*)(void*)prNRItem->prStrList);
    }	
    else if(u4DefaultNR == 3)
    {
        prNRItem->prStrList->prCurr = (WIDGET_STRINGLIST_ELEMENT_T*)(void*)WIDGET_SearchListElement("HIGH", (WIDGET_LIST_T*)(void*)prNRItem->prStrList);
    }
    

    UNUSED(i4Ret);   

    
    VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_VQ_Init(void)
{

/*
    vApiVideoWhitePeakLimit(1);

    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_BLE, 1));    
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_BLE, 1));  
    
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_DEINT_FILM, VDP_SV_CN_AUTO));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_DEINT_DEINT, VDP_DI_AUTO_MODE));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_DEINT_EDGE, VDP_SV_V_EDGE_HIGH));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_DEINT_FILM, VDP_SV_CN_AUTO));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_DEINT_DEINT, VDP_DI_AUTO_MODE));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_DEINT_EDGE, VDP_SV_V_EDGE_HIGH));

    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_ADAPTIVE_BACK_LIGHT, 0));
    
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_R_GAIN, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_G_GAIN, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_B_GAIN, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_R_GAIN, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_G_GAIN, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_B_GAIN, 0x80));

    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_R_OFFSET, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_G_OFFSET, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_MASTER, PE_ARG_B_OFFSET, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_R_OFFSET, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_G_OFFSET, 0x80));
    UNUSED(PE_SetArg(NAV_VDP_SLAVE, PE_ARG_B_OFFSET, 0x80));
    

    
    VERIFY(NAV_VQ_Reset(NAV_VDP_MASTER) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(NAV_VQ_Reset(NAV_VDP_SLAVE) == (INT32)E_WIDGET_MSG_OK);  
*/    
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_VQ_SetNR(INT32 i4Nr)
{
    //UINT32 u4VdpId = 0;
    if( (i4Nr >= 0) && (i4Nr <= 3) )
    {
        _rQuality.u1Nr = i4Nr;
        _rQuality.u13dNr = i4Nr;
        //UNUSED(PE_SetArg(u4VdpId, PE_ARG_NR, (UCHAR)i4Nr));
        DRV_VDO_SetQuality(0, DRV_VDO_QTYMSK_NR | DRV_VDO_QTYMSK_3D_NR, &_rQuality);        
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

UINT32 NAV_VQ_GetNR(void)
{
    return _u4NrMode;
}

INT32 NAV_VQ_SetNRMode(const void* pvArg)
{
    const CHAR* strNr = (const CHAR*)pvArg;
    if(x_strcmp(strNr, "OFF") == 0)
    {
        UNUSED(NAV_VQ_SetNR(0));
    }
    else if(x_strcmp(strNr, "LOW") == 0)
    {
        UNUSED(NAV_VQ_SetNR(1));
    }
    else if(x_strcmp(strNr, "MID") == 0)
    {
        UNUSED(NAV_VQ_SetNR(2));
    }
    else if(x_strcmp(strNr, "HIGH") == 0)
    {
        UNUSED(NAV_VQ_SetNR(3));
    }
    return (INT32)E_NAV_MSG_OK;
}
    
INT32 NAV_VQ_Set3DTVMode(const void* pvArg)
{
    const CHAR* str = (const CHAR*)pvArg;
    if(x_strcmp(str, "3D") == 0)
    {
        UNUSED(DRV_VDO_Set3DTV(u4ActiveVDP, DRV_VDO_3D_WO_FORCE));
    }
    else if(x_strcmp(str, "FORCE 2D L") == 0)
    {
        UNUSED(DRV_VDO_Set3DTV(u4ActiveVDP, DRV_VDO_3D_FORCE_2D_L));
    }
    else if(x_strcmp(str, "FORCE 2D R") == 0)
    {
        UNUSED(DRV_VDO_Set3DTV(u4ActiveVDP, DRV_VDO_3D_FORCE_2D_R));
    }
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_VQ_SetBrightness(void* pvArg)
{
    return _SetBrightness(u4ActiveVDP, (INT32)pvArg);
}
INT32 NAV_VQ_SetContrast(void* pvArg)
{
    return _SetContrast(u4ActiveVDP, (INT32)pvArg);
}
INT32 NAV_VQ_SetHue(void* pvArg)
{
    return _SetHue(u4ActiveVDP, (INT32)pvArg);
}
INT32 NAV_VQ_SetSaturation(void* pvArg)
{
    return _SetSaturation(u4ActiveVDP, (INT32)pvArg);
}
INT32 NAV_VQ_SetSharpness(void* pvArg)
{
    return _SetSharpness(u4ActiveVDP, (INT32)pvArg);
}

INT32 NAV_VQ_SetAdaptiveLuma(void* pvArg)
{
    return _SetAdaptiveLuma(u4ActiveVDP, (UINT32)pvArg);
}

INT32 NAV_VQ_SetSCE(void* pvArg)
{
    return _SetSCE(u4ActiveVDP, (UINT32)pvArg);
}
#if 0
INT32 NAV_VQ_SetBWS(void* pvArg)
{ 
}
#endif
INT32 NAV_VQ_SetCTI(void* pvArg)
{
    return _SetCTI(u4ActiveVDP, (INT32)pvArg);
}

INT32 NAV_VQ_SetBacklight(void* pvArg)
{
    return _SetBacklight(u4ActiveVDP, (INT32)pvArg);
}

INT32 _SetVGAHPos(UINT32 u4VdpId, INT32 i4HPos)
{
    DRV_VDO_VGA_PARAM_T rParam;
    if( ( (arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_VGA) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr1) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr2) )
    	||  (i4HPos < -63) || (i4HPos > 63) )
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    if(Get_VGAMODE_IPH_BP(_bVgaTiming)>i4HPos)
    	{
    rParam.u1HPos= Get_VGAMODE_IPH_BP(_bVgaTiming)-i4HPos;
    	}
	else
		{
	rParam.u1HPos= 0;	
		}
    DRV_VDO_SetVga(DRV_VDO_VGAMSK_HPOS, &rParam);
    
    return (INT32)E_NAV_MSG_OK;
}

INT32 _SetVGAVPos(UINT32 u4VdpId, INT32 i4VPos)
{
    DRV_VDO_VGA_PARAM_T rParam;
    if( ( (arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_VGA) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr1) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr2) )
    	||  (i4VPos < -31) || (i4VPos > 31) )
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    if(Get_VGAMODE_IPV_STA(_bVgaTiming)>i4VPos)
    	{
    rParam.u1VPos= Get_VGAMODE_IPV_STA(_bVgaTiming)-i4VPos;
    	}
	else
		{
	rParam.u1VPos=0;
		}
    DRV_VDO_SetVga(DRV_VDO_VGAMSK_VPOS, &rParam);
    
    return (INT32)E_NAV_MSG_OK;
}


INT32 _SetVGAPhase(UINT32 u4VdpId, INT32 i4Phase)
{
    DRV_VDO_VGA_PARAM_T rParam;
    if( ( (arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_VGA) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr1) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr2) )
    	||  (i4Phase < 0) || (i4Phase > 31) )
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    /*
    if(fgApiVideoVgaSetPhase((UINT8)u4VdpId, (UINT8)i4Phase) != (UINT8)TRUE)
    {
        return (INT32)E_NAV_MSG_ERROR;
    }
    */
    rParam.u1Phase = i4Phase;
    DRV_VDO_SetVga(DRV_VDO_VGAMSK_PHASE, &rParam);
    
    return (INT32)E_NAV_MSG_OK;
}

INT32 _SetVGAAuto(UINT32 u4VdpId)
{
    WIDGET_MENUITEM_RANGEDVALUE_T* prClockItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prPhaseItem;
	WIDGET_MENUITEM_RANGEDVALUE_T* prHPosItem;
    WIDGET_MENUITEM_RANGEDVALUE_T* prVPosItem;
    prClockItem =  (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA Clock");
    prPhaseItem =  (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA Phase");
	prHPosItem =  (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA H-Pos");
    prVPosItem =  (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "VGA V-Pos");
    if( (arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_VGA) || 
    	(arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_YPbPr1) || 
    	(arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_YPbPr2) )
    {
        DRV_VDO_Auto(u4VdpId);
#ifndef __MODEL_slt__                     
        if(x_sema_lock_timeout(hVgaSema, 5000) == OSR_OK)
#endif            
        {
     //      return (INT32)E_NAV_MSG_OK;
        }        
        if(fgApiVideoVgaAuto() == (UINT8)TRUE)
        {
            while(_bAutoFlag != 0)
            {
                x_thread_delay(100);
            }
            prClockItem->i4Value = (INT32)(wDrvVGAGetClock() - 127);
            prPhaseItem->i4Value = (INT32)u1DrvVGAGetPhase();
			prHPosItem->i4Value = (INT32)(Get_VGAMODE_IPH_BP(_bVgaTiming)-wDrvVGAGetHPosition());
			prVPosItem->i4Value = (INT32)(Get_VGAMODE_IPV_STA(_bVgaTiming)-wDrvVGAGetVPosition());
            VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
            VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
            return (INT32)E_NAV_MSG_OK;
        }
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

INT32 _SetVGAClock(UINT32 u4VdpId, INT32 i4Clock)
{
    DRV_VDO_VGA_PARAM_T rParam;
    if( ( (arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_VGA) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr1) &&
    	(arVideoPath[u4VdpId].u4Src != (UINT32)E_NAV_VS_YPbPr2) ) ||  
    	(i4Clock < -127) || (i4Clock > 127) )
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    /*
    if(fgApiVideoVgaSetClock((UINT8)u4VdpId, (UINT8)(i4Clock + 127)) != (UINT8)TRUE)
    {
        return (INT32)E_NAV_MSG_ERROR;
    }
    */
    rParam.u1Clock = i4Clock + 127;    
    DRV_VDO_SetVga(DRV_VDO_VGAMSK_CLOCK, &rParam);
    return (INT32)E_NAV_MSG_OK;
}

INT32 _SetVGAAutoColor(UINT32 u4VdpId)
{
    if( (arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_VGA) || 
    	(arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_YPbPr1) || 
    	(arVideoPath[u4VdpId].u4Src == (UINT32)E_NAV_VS_YPbPr2) )
    {         
        DRV_VDO_Autocolor(u4VdpId);
#ifndef __MODEL_slt__             
        if(x_sema_lock_timeout(hVgaSema, 5000) == OSR_OK)
#endif       	
        {
            return (INT32)E_NAV_MSG_OK;
        }        
        /*
        if(fgApiVideoAutoColor() == (UINT8)TRUE)
        {
            while(_bAutoColorState0 != (UINT8)VDO_AUTO_COLOR_NOT_BEGIN)
            {
                x_thread_delay(100);
            }
            return (INT32)E_NAV_MSG_OK;
        }
        */
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
	
}

INT32 NAV_VQ_SetVGAVPos(void* pvArg)
{
    return _SetVGAVPos(u4ActiveVDP, (INT32)pvArg);
}

INT32 NAV_VQ_SetVGAHPos(void* pvArg)
{
    return _SetVGAHPos(u4ActiveVDP, (INT32)pvArg);
}

INT32 NAV_VQ_SetVGAPhase(void* pvArg)
{
    return _SetVGAPhase(u4ActiveVDP, (INT32)pvArg);
}


INT32 NAV_VQ_SetVGAAuto(void* pvArg)
{
    UNUSED(pvArg);
    return _SetVGAAuto(u4ActiveVDP);
}

INT32 NAV_VQ_SetVGAClock(void* pvArg)
{
    return _SetVGAClock(u4ActiveVDP, (INT32)pvArg);
}

INT32 NAV_VQ_SetVGAAutoColor(void* pvArg)
{
    UNUSED(pvArg);
    return _SetVGAAutoColor(u4ActiveVDP);
}

