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
 * $RCSfile: nav_config.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_config_foxconnC26.h
 *  
 */
#include "x_typedef.h"
#include "x_stl_lib.h"
#include "x_os.h"

#include "widget_if.h"
#include "vdp_if.h"
#include "osd_if.h"

#include "nav.h"
#include "nav_av.h"
#include "widget_if.h"
#include "vdp_if.h"

#ifdef NAV_CONFIG_FILE
#include NAV_CONFIG_FILE
#endif
#include "config/nav_config_default.h"


static BOOL _afgInputSrc[E_NAV_VS_MAX];

INT32 NAV_CONFIG_Init(void)
{
    _afgInputSrc[E_NAV_VS_TUNER1] = NAV_CONFIG_ATV;
    _afgInputSrc[E_NAV_VS_DT1] = NAV_CONFIG_DTV;
    _afgInputSrc[E_NAV_VS_CVBS1] = NAV_CONFIG_AV1;
    _afgInputSrc[E_NAV_VS_CVBS2] = NAV_CONFIG_AV2;
    _afgInputSrc[E_NAV_VS_CVBS3] = NAV_CONFIG_AV3;
    _afgInputSrc[E_NAV_VS_CVBS4] = NAV_CONFIG_AV4;
    _afgInputSrc[E_NAV_VS_S1] = NAV_CONFIG_SV1;
    _afgInputSrc[E_NAV_VS_S2] = NAV_CONFIG_SV2;
    _afgInputSrc[E_NAV_VS_S3] = NAV_CONFIG_SV3;
    _afgInputSrc[E_NAV_VS_YPbPr1] = NAV_CONFIG_YPbPr1;
    _afgInputSrc[E_NAV_VS_YPbPr2] = NAV_CONFIG_YPbPr2;
    _afgInputSrc[E_NAV_VS_HDMI] = NAV_CONFIG_HDMI1;
    _afgInputSrc[E_NAV_VS_HDMI2] = NAV_CONFIG_HDMI2;
    _afgInputSrc[E_NAV_VS_HDMI3] = NAV_CONFIG_HDMI3;
    _afgInputSrc[E_NAV_VS_HDMI4] = NAV_CONFIG_HDMI4;    
    _afgInputSrc[E_NAV_VS_HDMI5] = NAV_CONFIG_HDMI5;        
    _afgInputSrc[E_NAV_VS_VGA] = NAV_CONFIG_VGA;
    _afgInputSrc[E_NAV_VS_DT2] = NAV_CONFIG_I1394;
    _afgInputSrc[E_NAV_VS_SCART] = NAV_CONFIG_SCART;
    _afgInputSrc[E_NAV_VS_SCART2] = NAV_CONFIG_SCART2;

    return (INT32)E_NAV_MSG_OK;    
}

INT32 NAV_CONFIG_GetInputSrc(BOOL** ppfgSrc)
{
	*ppfgSrc = _afgInputSrc;

    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_CONFIG_GetDefInputSrc(void)
{
	return NAV_DEFAULT_INPUT_SOURCE;
}

INT32 NAV_CONFIG_GetMenuOsdPlane(void)
{
	return NAV_CONFIG_MENU_OSD;
}

INT32 NAV_CONFIG_GetBorderOsdPlane(void)
{
	return NAV_CONFIG_BORDER_OSD;
}

