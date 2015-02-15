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
 * $RCSfile: nav_aquality.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_aquality.c
 *  
 */


#include "x_lint.h"
LINT_EXT_HEADER_BEGIN
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
//#include "x_hal_5371.h"
#include "x_hal_926.h"
#include "x_drv_cli.h"

#include "nptv_if.h"
#include "vdp_if.h"
#include "osd_if.h"
//#include "drv_dsp_cfg.h"

LINT_EXT_HEADER_END

#include "x_drv_if.h"

#include "widget_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_aquality.h"

#include "nav_debug.h"



static UINT32 _u4AudioVol = 50;
static UINT32 _u4AudioMute = 0;


INT32 NAV_AQ_SetVol(void* pvArg)
{
	INT32 i4Vol = (INT32)pvArg;
	DRV_AUD_QUALITY_T rQuality;
	if( (i4Vol < 0) || (i4Vol > 100) )
	{
		return (INT32)E_NAV_MSG_NOT_SUPPORT;
	}	
	_u4AudioVol = (UINT32)i4Vol;
	if(!NAV_AQ_GetMute())
	{
	       rQuality.u1Volume = _u4AudioVol;
		DRV_AUD_SetQuality(DRV_AUD_QTYMSK_VOLUME, &rQuality);
	}
	
	return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_AQ_GetVol(void)
{
	return (INT32)_u4AudioVol;
}

INT32 NAV_AQ_SetBass(void* pvArg)
{
	INT32 i4Gain = (INT32)pvArg;
	DRV_AUD_QUALITY_T rQuality;
	if( (i4Gain >= 0) && (i4Gain < 29) )
	{
              rQuality.u1Bass= i4Gain;
		DRV_AUD_SetQuality(DRV_AUD_QTYMSK_BASS, &rQuality);
		return (INT32)E_NAV_MSG_OK;
	}	
	return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
INT32 NAV_AQ_SetTreble(void* pvArg)
{
	INT32 i4Gain = (INT32)pvArg;
	DRV_AUD_QUALITY_T rQuality;	
	if( (i4Gain >= 0) && (i4Gain < 29) )
	{
              rQuality.u1Treble = i4Gain;
		DRV_AUD_SetQuality(DRV_AUD_QTYMSK_TREBLE, &rQuality);
		return (INT32)E_NAV_MSG_OK;
	}
	return (INT32)E_NAV_MSG_NOT_SUPPORT;
}
INT32 NAV_AQ_SetBalance(void* pvArg)
{
   INT32 i4Gain = (INT32)pvArg; 
   DRV_AUD_QUALITY_T rQuality;	
   
   if ((i4Gain < 0) || (i4Gain > 100))
   {
		return (INT32)E_NAV_MSG_NOT_SUPPORT;
   }
              rQuality.u1Balance = i4Gain;
		DRV_AUD_SetQuality(DRV_AUD_QTYMSK_BALANCE, &rQuality);
   return (INT32)E_NAV_MSG_OK;
}

#ifdef CC_NAV_REVERB
INT32 NAV_AQ_SetReverb(void* pvArg)
{
    const CHAR* szMode = (const CHAR*)pvArg;    
    DRV_AUD_QUALITY_T rQuality;
    if(x_strcmp(szMode, "OFF") == 0)
    {
        rQuality.u1Reverb = 0;
    }
    else if(x_strcmp(szMode, "CONCERT") == 0)
    {
        rQuality.u1Reverb = 1;
    }
    else if(x_strcmp(szMode, "LIVING ROOM") == 0)
    {
        rQuality.u1Reverb = 2;
    }
    else if(x_strcmp(szMode, "HALL") == 0)
    {
        rQuality.u1Reverb = 3;     
    }
    else if(x_strcmp(szMode, "BATHROOM") == 0)
    {
        rQuality.u1Reverb = 4;
    }
    else if(x_strcmp(szMode, "CAVE") == 0)
    {
        rQuality.u1Reverb = 5;      
    }
    else if(x_strcmp(szMode, "ARENA") == 0)
    {
        rQuality.u1Reverb = 6;       
    }
    else if(x_strcmp(szMode, "CHURCH") == 0)
    {
        rQuality.u1Reverb = 7;
    }   
    DRV_AUD_SetQuality(DRV_AUD_QTYMSK_REVERB, &rQuality);
    
    UNUSED(pvArg);
    return (INT32)E_NAV_MSG_OK;
}
#endif

INT32 NAV_AQ_SetEqualizer(void* pvArg)
{
    const CHAR* szMode = (const CHAR*)pvArg;
    DRV_AUD_QUALITY_T rQuality;
    if(x_strcmp(szMode, "NONE") == 0)
    {
        rQuality.u1Equalizer = 0;
    }
    else if(x_strcmp(szMode, "ROCK") == 0)
    {
        rQuality.u1Equalizer = 1;
    }
    else if(x_strcmp(szMode, "POP") == 0)
    {
        rQuality.u1Equalizer = 2;
    }
    else if(x_strcmp(szMode, "LIVE") == 0)
    {
        rQuality.u1Equalizer = 3;
    }
    else if(x_strcmp(szMode, "DANCE") == 0)
    {
        rQuality.u1Equalizer = 4;
    }
    else if(x_strcmp(szMode, "TECHNO") == 0)
    {
        rQuality.u1Equalizer = 5;
    }
    else if(x_strcmp(szMode, "CLASSIC") == 0)
    {
        rQuality.u1Equalizer = 6;
    }
    else if(x_strcmp(szMode, "SOFT") == 0)
    {
        //AUD_DspChEqLoadPreset(AUD_DEC_MAIN, 7);
        rQuality.u1Equalizer = 7;
    }
    DRV_AUD_SetQuality(DRV_AUD_QTYMSK_EQUALIZER, &rQuality);    
    UNUSED(pvArg);
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_AQ_SetSurround(void* pvArg)
{
    DRV_AUD_QUALITY_T rQuality;
    rQuality.u1Surround = (UINT8)(INT32)pvArg;
    DRV_AUD_SetQuality(DRV_AUD_QTYMSK_SURROUND, &rQuality);    	
    return (INT32)E_NAV_MSG_OK;
}
INT32 NAV_AQ_SetMute(UINT32 u4Mute)
{
       DRV_AUD_QUALITY_T rQuality;      
	_u4AudioMute = u4Mute;
	if(u4Mute)
	{	
		rQuality.u1Volume = 0;
       }
	else
	{		
		rQuality.u1Volume = _u4AudioVol;
	}	
       DRV_AUD_SetQuality(DRV_AUD_QTYMSK_VOLUME, &rQuality);    	
	return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_AQ_GetMute(void)
{
	return (INT32)_u4AudioMute;
}

INT32 NAV_AQ_Init(void)
{
	
	VERIFY(NAV_AQ_SetVol((void*)50) == (INT32)E_NAV_MSG_OK);
	
	return (INT32)E_NAV_MSG_OK;
}
