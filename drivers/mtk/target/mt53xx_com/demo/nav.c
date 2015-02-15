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
 * $RCSfile: nav.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav.c
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

#include "osd_if.h"
//#include "gfx_if.h"
#include "ir_if.h"
//#include "drv_fnt.h"
//#include "drv_common.h"
//#include "vdp_if.h"
//#include "sif_if.h"
//#include "nim_if.h"
//#include "drv_aud.h"
//#include "aud_if.h"
//#include "mpv_if.h"
//#include "sv_const.h"
#include "drvcust_if.h"
//#include "rtc_if.h"

LINT_EXT_HEADER_END

#include "x_drv_if.h"

#include "widget_if.h"
#include "psipsr_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_event.h"
#include "nav_vquality.h"
#include "nav_aquality.h"
#include "nav_freqtbl.h"
#include "nav_config.h"
#include "nav_mm.h"

#include "panel.h"

//---------------------------------------------------------------------
// LOG macro related

#define DEFINE_IS_LOG   CLI_IsLog
#include "x_debug.h"


#define ENABLE_AUDIO_VIDEO 1
#define PMX_WORKAROUND 1

#ifdef __MODEL_slt__
extern BOOL _fgSltInit;
#endif

UINT32 u4PmxX = 0;
UINT32 u4PmxY = 0;
UINT32 u4PmxW = 0;
UINT32 u4PmxH = 0;


NAV_OPTION_T rNAVOption = 
{
    TRUE, // Analog tuner 
    TRUE, // cvbs1
    TRUE, // cvbs2
    TRUE, // cvbs3
    TRUE, // s1
    TRUE, // s2
    TRUE, // s3
    TRUE, // ypbpr1
    TRUE, // ypbpr2
    TRUE, // HDMI
    TRUE, // HDMI
    TRUE, // VGA
    
    TRUE, // Digital Tuner
    TRUE, // IEEE 1394

    TRUE, // audio

    TRUE, // 32-bit OSD or not

    TRUE, // Full scan 
};


WIDGET_OSDINFO_T arWidgetOSD[3/*OSD_PLANE_MAX_NUM*/];

WIDGET_LIST_T* prInputSrcList;
WIDGET_LIST_T* prInputSrcListPIP;

BOOL fgInfoOSDOn = FALSE;

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
#ifdef CC_MT5391_SLT
static INT32 _AtscTest(void);
#endif

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
//static UINT32 _u4MenuOSD = 1;
//static UINT32 _u4BorderOSD = 2;
//static UINT32 _u4InfoOSD = 0;

/* Definition of Menu */
static const UINT32 _u4NumMenu = 5;
static const UINT32 _u4NumItemVideo = 12;
static const UINT32 _u4NumItemPicture = 9;
#ifdef CC_NAV_REVERB
static const UINT32 _u4NumItemSound = 6;
#else
static const UINT32 _u4NumItemSound = 5;
#endif
#ifndef CC_NAV_SCART
static const UINT32 _u4NumItemSetup = 5;
#else
static const UINT32 _u4NumItemSetup = 6;
#endif
#ifdef CC_SUPPORT_PVR
static const UINT32 _u4NumItemMultimedia = 2;
#else
static const UINT32 _u4NumItemMultimedia = 1;
#endif

static const UINT32 _au4NumItem[_u4NumMenu] =
{
    _u4NumItemVideo,
    _u4NumItemPicture,
    _u4NumItemSound,
    _u4NumItemSetup,
    _u4NumItemMultimedia
};

static const UINT32 _u4NumNROption = 4;
static NAV_MENUITEM_T _arNROption[_u4NumNROption] =
{
    {"OFF", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"LOW", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"MID", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"HIGH", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
};

static const UINT32 _u4Num3DTVOption = 3;
static NAV_MENUITEM_T _ar3DTVOption[_u4Num3DTVOption] =
{
    {"3D", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"FORCE 2D L", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"FORCE 2D R", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
};

static const UINT32 _u4MaxNumItemVideo5372 = 12;
static NAV_MENUITEM_T _arVideoItem5372[_u4MaxNumItemVideo5372]  =
{
    {"H-Scale", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_SetVideoHScale, VDP_MAX_REGION_WIDTH, 0, VDP_MAX_REGION_WIDTH},
    {"V-Scale", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_SetVideoVScale, VDP_MAX_REGION_HEIGHT, 0, VDP_MAX_REGION_HEIGHT},
    {"H-Pos", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_SetVideoHPos, 0, 0, VDP_MAX_REGION_WIDTH - 1},
    {"V-Pos", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_SetVideoVPos, 0, 0, VDP_MAX_REGION_HEIGHT - 1},  
    {"NR", E_NAV_MENUITEM_TYPE_OPTION, _arNROption, _u4NumNROption, (INT32 (*)(void*))NAV_VQ_SetNRMode, 0, 0, 0},
    {"VGA H-Pos", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAHPos, 0, -63, 63},
    {"VGA V-Pos", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAVPos, 0, -31, 31},
    {"VGA Phase", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAPhase, 15, 0, 31},
    {"VGA Clock", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAClock, 0, -127, 127},
    {"VGA Auto", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_VQ_SetVGAAuto, 0, 0, 0},
    {"VGA Auto Color", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_VQ_SetVGAAutoColor, 0, 0, 0},
    {"3DTV", E_NAV_MENUITEM_TYPE_OPTION, _ar3DTVOption, _u4Num3DTVOption, (INT32 (*)(void*))NAV_VQ_Set3DTVMode, 0, 0, 0},
};
//static const UINT32 _u4MaxNumItemVideo5370 = 5;
//static NAV_MENUITEM_T _arVideoItem5370[_u4MaxNumItemVideo5370] =
//{
//    {"NR", E_NAV_MENUITEM_TYPE_OPTION, _arNROption, _u4NumNROption, NAV_VQ_SetNRMode, 0, 0, 0},
//    {"VGA Phase", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAPhase, 15, 0, 31},
//    {"VGA Clock", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetVGAClock, 0, -127, 127},
//    {"VGA Auto", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_VQ_SetVGAAuto, 0, 0, 0},
//    {"VGA Auto Color", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_VQ_SetVGAAutoColor, 0, 0, 0}
//};
static NAV_MENUITEM_T _arVideoItem[_u4NumItemVideo];

static NAV_MENUITEM_T _arPictureItem[_u4NumItemPicture] =
{
    {"Brightness", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetBrightness, 50, 0, 100},
    {"Contrast", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetContrast, 50, 0, 100},
    {"Hue", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetHue, 0, -50, 50},
    {"Saturation", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetSaturation, 50, 0, 100},
    {"Sharpness", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0,NAV_VQ_SetSharpness, 2, 0, 20},
    {"CTI", E_NAV_MENUITEM_TYPE_BINARYVALUE, NULL, 0, NAV_VQ_SetCTI, TRUE, FALSE, TRUE},
    {"Adaptive Luma", E_NAV_MENUITEM_TYPE_BINARYVALUE, NULL, 0, NAV_VQ_SetAdaptiveLuma, TRUE, FALSE, TRUE},
    {"SCE", E_NAV_MENUITEM_TYPE_BINARYVALUE, NULL, 0, NAV_VQ_SetSCE, TRUE, FALSE, TRUE},
    //{"BWS", E_NAV_MENUITEM_TYPE_BINARYVALUE, NULL, 0, NAV_VQ_SetBWS, TRUE, FALSE, TRUE},
    {"Backlight", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_VQ_SetBacklight, 50, 0, 100}    
};

#ifdef CC_NAV_REVERB
static const UINT32 _u4NumReverbOption = 8;
static NAV_MENUITEM_T _arReverbOption[_u4NumReverbOption] =
{
    {"OFF", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"CONCERT", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"LIVING ROOM", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"HALL", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"BATHROOM", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"CAVE", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"ARENA", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"CHURCH", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
};
#endif
static const UINT32 _u4NumEqualizerOption = 8;
static NAV_MENUITEM_T _arEqualizerOption[_u4NumEqualizerOption] =
{
    {"NONE", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"ROCK", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"POP", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"LIVE", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"DANCE", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"TECHNO", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"CLASSIC", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"SOFT", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
};
static NAV_MENUITEM_T _arSoundItem[_u4NumItemSound] =
{
    {"Bass", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_AQ_SetBass, 14, 0, 28},
    {"Treble", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_AQ_SetTreble, 14, 0, 28},
    {"Balance", E_NAV_MENUITEM_TYPE_RANGEDVALUE, NULL, 0, NAV_AQ_SetBalance, 50, 0, 100},
    {"Surround", E_NAV_MENUITEM_TYPE_BINARYVALUE, NULL, 0, NAV_AQ_SetSurround, FALSE, FALSE, TRUE},
    #ifdef CC_NAV_REVERB
    {"Reverb", E_NAV_MENUITEM_TYPE_OPTION, _arReverbOption, _u4NumReverbOption, NAV_AQ_SetReverb, 0, 0, 0},
    #endif
    {"Equalizer", E_NAV_MENUITEM_TYPE_OPTION, _arEqualizerOption, _u4NumEqualizerOption, NAV_AQ_SetEqualizer, 0, 0, 0}
};

#ifndef CC_NAV_DEMOD_DVBT
static const UINT32 _u4NumDTVSystemOption = 2;
#else
#ifndef DRV_IF_BR
static const UINT32 _u4NumDTVSystemOption = 3;
#else
static const UINT32 _u4NumDTVSystemOption = 1;
#endif
#endif
#ifndef CC_NAV_DEMOD_DVBT
static NAV_MENUITEM_T _arDTVSystemOption[_u4NumDTVSystemOption] =
{
    {"Terrestrial", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"Cable", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
};
static const UINT32 _u4NumATVSystemOption = 2;
static NAV_MENUITEM_T _arATVSystemOption[_u4NumATVSystemOption] =
{
    {"Cable", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"Air", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
};
#else
#ifndef DRV_IF_BR
static NAV_MENUITEM_T _arDTVSystemOption[_u4NumDTVSystemOption] =
{
    {"DVB-T UK", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"DVB-T TW", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"DVB-C", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
};
#else
static NAV_MENUITEM_T _arDTVSystemOption[_u4NumDTVSystemOption] =
{
    {"ISDBT", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
};
#endif
#ifndef DRV_IF_BR
static const UINT32 _u4NumATVSystemOption = 6;
#else
static const UINT32 _u4NumATVSystemOption = 1;
#endif
static NAV_MENUITEM_T _arATVSystemOption[_u4NumATVSystemOption] =
{
#ifdef DRV_IF_BR
    {"NTSC", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
#else
    {"PAL B/G", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"PAL D/K", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"PAL I", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"SECAM L", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"SECAM L1", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"NTSC M", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0}
#endif
};
#endif


static UINT32 _u4NumInputSourceOption;
#ifdef CC_YES_1394    
static const UINT32 _u4MaxNumInputSourceOption = 20;
#else
static const UINT32 _u4MaxNumInputSourceOption = 20;
#endif
static NAV_MENUITEM_T _arInputSourceOption[_u4MaxNumInputSourceOption];

/*
#define SV_SCART_CVBS			0x00
#define SV_SCART_RGB			0x01
#define SV_SCART_SV				0x02
#define SV_SCART_CVBS_RGB		0x03
#define SV_SCART_AUTO			0x04
*/
#ifdef CC_NAV_SCART
static const UINT32 _u4NumScartOption = 5;
static NAV_MENUITEM_T _arScartOption[_u4NumScartOption] =
{
	{"CVBS", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"RGB", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},    
    {"SV", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},
    {"CVBS+RGB", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},    
    {"AUTO", E_NAV_MENUITEM_TYPE_LEAF, NULL, 0, NULL, 0, 0, 0},    
};
#endif
static NAV_MENUITEM_T _arSetupItem[_u4NumItemSetup] =
{
    {"AV Input Source", E_NAV_MENUITEM_TYPE_OPTION, _arInputSourceOption, _u4MaxNumInputSourceOption, NAV_SetSrcActive, 0, 0, 0},
    {"DTV Scan Channel", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_ScanDTVChannel, 0, 0, 0},
    {"DTV System", E_NAV_MENUITEM_TYPE_OPTION, _arDTVSystemOption, _u4NumDTVSystemOption, NAV_SetDTVSystem, 0, 0, 0},
    {"ATV Scan Channel", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_ScanATVChannel, 0, 0, 0},
    {"ATV System", E_NAV_MENUITEM_TYPE_OPTION, _arATVSystemOption, _u4NumATVSystemOption, NAV_SetATVSystem, 0, 0, 0},
#ifdef CC_NAV_SCART
    {"SCART Mode", E_NAV_MENUITEM_TYPE_OPTION, _arScartOption, _u4NumScartOption, NAV_SetScart, 0, 0, 0},
#endif
};

static NAV_MENUITEM_T _arMultimediaItem[_u4NumItemMultimedia] =
{
    {"Browse", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_MM_Browse, 0, 0, 0},
#ifdef CC_SUPPORT_PVR
    {"Enter PVR Mode", E_NAV_MENUITEM_TYPE_ACTION, NULL, 0, (INT32 (*)(void*))NAV_MM_PVR_Mode, 0, 0, 0},
#endif
};

static NAV_MENUTITLE_T _arMenuTitle[_u4NumMenu] =
{
    {"VIDEO", _arVideoItem},
    {"PICTURE", _arPictureItem},
    {"SOUND", _arSoundItem},
    {"SETUP", _arSetupItem},
    {"MEDIA", _arMultimediaItem}
};

#ifdef USE_RGB8_CLUT
WIDGET_BITMAP_T arBmpTbl[] =
{
    {"menu_bg", NULL, 816, 412, 816, 0x52140},
        
    {"menutitle_video", NULL, 192, 48, 192, 0x2400},
    {"menutitle_video_off", NULL, 192, 48, 192, 0x2400},
    {"menutitle_color", NULL, 192, 48, 192, 0x2400},
    {"menutitle_color_off", NULL, 192, 48, 192, 0x2400},
    {"menutitle_sound", NULL, 192, 48, 192, 0x2400},
    {"menutitle_sound_off", NULL, 192, 48, 192, 0x2400},
    {"menutitle_setup", NULL, 192, 48, 192, 0x2400},
    {"menutitle_setup_off", NULL, 192, 48, 192, 0x2400},
//#ifdef CC_YES_1394
    {"menutitle_i1394", NULL, 192, 48, 192, 0x2400},
    {"menutitle_i1394_off", NULL, 192, 48, 192, 0x2400},
//#endif
    {"info_osd", NULL, 816, 98, 816, 0x13860},
    {NULL, NULL, 0, 0, 0, 0}
};
#else
WIDGET_BITMAP_T arBmpTbl[] =
{
    {"menu_bg", NULL, 808, 412, 1616, 0xA28C0},
        
    {"menutitle_video", NULL, 192, 48, 384, 0x4800},
    {"menutitle_video_off", NULL, 192, 48, 384, 0x4800},
    {"menutitle_color", NULL, 192, 48, 384, 0x4800},
    {"menutitle_color_off", NULL, 192, 48, 384, 0x4800},
    {"menutitle_sound", NULL, 192, 48, 384, 0x4800},
    {"menutitle_sound_off", NULL, 192, 48, 384, 0x4800},
    {"menutitle_setup", NULL, 192, 48, 384, 0x4800},
    {"menutitle_setup_off", NULL, 192, 48, 384, 0x4800},
//#ifdef CC_YES_1394
    {"menutitle_i1394", NULL, 192, 48, 384, 0x4800},
    {"menutitle_i1394_off", NULL, 192, 48, 384, 0x4800},
//#endif
    {"info_osd", NULL, 808, 98, 1616, 0x26AA0},
    {NULL, NULL, 0, 0, 0, 0}
};
#endif

static UINT32 _u4BmpFlashOffset = 0x102C0000;

EXTERN UINT32 u4NumATVProg;
EXTERN NAV_ATV_TBL_PROG_T arATVTbl[NTSC_CABLE_CH_NS];
EXTERN UINT32 u4NumDTVProg;
EXTERN PSIPSR_TBL_VCT_T arVctTbl[NAV_DTV_PROG_MAX_NUM];
EXTERN PSIPSR_TBL_TS2FREQ_T arTsTbl[NAV_DTV_PROG_MAX_NUM];

//---------------------------------------------------------------------------
// Public function forward declarations
//---------------------------------------------------------------------------
INT32 NAV_Init(const NAV_OPTION_T* prOpt);

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------
static INT32 _CopyOption(const NAV_OPTION_T* prOpt);
static WIDGET_MENULIST_T* _SetupMenuList(void);
static void _LoadBmp(void);
static void _AddInputSourceItem(NAV_MENUITEM_T* prItem, const CHAR* szName);
//static void _RTCAlertHandler(void);
static void _InitDrivers(void);
static void _InitInputSourceList(void);
static void _InitWidget(void);

//---------------------------------------------------------------------------
// Imported function forward declarations
//---------------------------------------------------------------------------
EXTERN BOOL DBS_Init(void);
EXTERN BOOL VdoMLInit(void);
EXTERN UINT16* NAV_LookupUI(void);
EXTERN INT32 WIDGET_InitFont(void);

#ifdef CC_MT5391_SLT
#include "TDIS.h"
static INT32 _AtscTest(void)
{
    TUNER_CONFIG_TBL_T rTunerCfgTbl;
    UINT32 u4Lvl = 0;
    UINT32 i;
    
    CLI_Parser("nim.go dvbt");
    CLI_Parser("nim.c");    
    CLI_Parser("nim.go atsc");     
    CLI_Parser("nim.freq 533000");    
    
    TDIS_RetrieveTunerCfgTbl(&rTunerCfgTbl);   
    
    for(i = 0; i < 10; i++)
    {                        
        u4Lvl += TunerGetSignalSNR(rTunerCfgTbl.tTunerCfg[0].TunerID, DRV_TUNER_CT_DIGT);
    }
    u4Lvl /= 10;
    if(u4Lvl < 20)
    {
        PMX_ShowPattern(TRUE);
    }

    CLI_Parser("nim.go cable256");     
    CLI_Parser("nim.freq 57000");    

    for(i = 0; i < 10; i++)
    {                        
        u4Lvl += TunerGetSignalSNR(rTunerCfgTbl.tTunerCfg[0].TunerID, DRV_TUNER_CT_DIGC);
    }
    u4Lvl /= 10;
    if(u4Lvl < 20)
    {
        PMX_ShowPattern(TRUE);
    }
    
    return 0;
}
#endif

static INT32 _CopyOption(const NAV_OPTION_T* prOpt)
{
    if(prOpt == NULL)
    {
        return (INT32)E_NAV_MSG_OK;
    }
    x_memcpy(&rNAVOption, prOpt, sizeof(NAV_OPTION_T));
    
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _SetupMenuList
 *  Setup Navigator's menu structure
 *  @param  
 *  @retval  NULL  FAIL. 
 */
//-------------------------------------------------------------------------
static WIDGET_MENULIST_T* _SetupMenuList(void) 
{
    UINT32 i, j, k;
    UINT32 u4BmpIdx = 0;
    WIDGET_MENULIST_T* prList;
    WIDGET_MENU_T* prMenu;
    WIDGET_MENUITEM_T* prItem;
    WIDGET_STRINGLIST_T* prStrList;
    NAV_MENUITEM_T* prItemDef;
    
    prList = WIDGET_NewMenuList("TV OSD", arWidgetOSD[0].u4ColorMode, &arBmpTbl[u4BmpIdx++]); // create menu list widget
    VERIFY(WIDGET_AddMenuList(arWidgetOSD, prList) == (INT32)E_NAV_MSG_OK);
    
	// create menu widget
    for(i = 0; i <_u4NumMenu; i++)
    {
        prMenu = WIDGET_NewMenu(_arMenuTitle[i].szTitle, arWidgetOSD[0].u4ColorMode, 
        &arBmpTbl[u4BmpIdx], &arBmpTbl[u4BmpIdx + 1], &arBmpTbl[u4BmpIdx + 1]); 

        
        u4BmpIdx += 2;
        
        VERIFY(WIDGET_AddMenu(prList, prMenu) == (INT32)E_NAV_MSG_OK);
        
		// create menu items widget for  menu [i]
        for(j = 0; j < _au4NumItem[i]; j++)
        {
            prItemDef = &(_arMenuTitle[i].prItem[j]);
            switch(prItemDef->eItemType)
            {
                case E_NAV_MENUITEM_TYPE_ACTION:
                    prItem = (WIDGET_MENUITEM_T*)(void*)WIDGET_NewMenuItem_Action(prItemDef->szTitle, arWidgetOSD[0].u4ColorMode, NULL);
                    VERIFY(WIDGET_SetAction(
                        prItem, 
                        NAV_IRHandler_MenuItem_Action, 
                        prItemDef->pfOp, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemAction, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemAction) 
                        == (INT32)E_NAV_MSG_OK);
                    
                    VERIFY(WIDGET_AddMenuItem(prMenu, prItem) == (INT32)E_NAV_MSG_OK);;
                    break;
                case E_NAV_MENUITEM_TYPE_OPTION:
                    prItem = (WIDGET_MENUITEM_T*)(void*)WIDGET_NewMenuItem_Option(prItemDef->szTitle, arWidgetOSD[0].u4ColorMode, NULL);
                    VERIFY(WIDGET_SetAction(
                        prItem, 
                        NAV_IRHandler_MenuItem_Option, 
                        prItemDef->pfOp, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemOption, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemOption) 
                        == (INT32)E_NAV_MSG_OK);
                    
                    VERIFY(WIDGET_AddMenuItem(prMenu, prItem) == (INT32)E_NAV_MSG_OK);;
                    prStrList = WIDGET_NewStringList();
                    VERIFY(WIDGET_AddStringList((WIDGET_MENUITEM_OPTION_T*)(void*)prItem, prStrList) == (INT32)E_NAV_MSG_OK);
                    for(k = 0; k < prItemDef->u4NumSubItem; k++)
                    {                     
                        VERIFY(WIDGET_AddStringListElement(prStrList, (*prItemDef).prSubItem[k].szTitle) == (INT32)E_NAV_MSG_OK);           
                    }                       
                    break;
                case E_NAV_MENUITEM_TYPE_BINARYVALUE:
                    prItem = (WIDGET_MENUITEM_T*)(void*)WIDGET_NewMenuItem_BinaryValue(prItemDef->szTitle, (UINT32)prItemDef->i4Value, arWidgetOSD[0].u4ColorMode, NULL);                  
                    VERIFY(WIDGET_SetAction(
                        prItem, 
                        NAV_IRHandler_MenuItem_BinaryValue, 
                        prItemDef->pfOp, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemBinaryValue, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemBinaryValue) 
                        == (INT32)E_NAV_MSG_OK);
                    
                    VERIFY(WIDGET_AddMenuItem(prMenu, prItem) == (INT32)E_NAV_MSG_OK); 
                    break;
                case E_NAV_MENUITEM_TYPE_RANGEDVALUE:
                    prItem = (WIDGET_MENUITEM_T*)(void*)WIDGET_NewMenuItem_RangedValue(prItemDef->szTitle, prItemDef->i4Value, prItemDef->i4Min, prItemDef->i4Max, arWidgetOSD[0].u4ColorMode, NULL);
                    VERIFY(WIDGET_SetAction(
                        prItem, 
                        NAV_IRHandler_MenuItem_RangedValue, 
                        prItemDef->pfOp, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemRangedValue, 
                        (INT32 (*)(const WIDGET_MENUITEM_T *, const WIDGET_OSDINFO_T *, UINT32))WIDGET_DrawMenuItemRangedValue) 
                        == (INT32)E_NAV_MSG_OK);
                    
                    VERIFY(WIDGET_AddMenuItem(prMenu, prItem) == (INT32)E_NAV_MSG_OK);
                    break;  
                default:
                    break;
            }

        }
        prMenu->prCurr = NULL;
    }

#ifdef CC_YES_1394
    NAV_I1394_SetupMenu(prList);
#endif
    return prList;
}


static void _LoadBmp(void)
{
    UINT32 u4Offset = 0;
    UCHAR ucIndex = 0;

    _u4BmpFlashOffset = (UINT32)NAV_LookupUI();

    while (arBmpTbl[ucIndex].szName!=NULL)
    {
        arBmpTbl[ucIndex].pu1Address = (UINT8*)DRV_SYS_AllocAlignedDmaMemory(arBmpTbl[ucIndex].u4Size, 0x10);
        VERIFY(arBmpTbl[ucIndex].pu1Address != NULL);
        x_memcpy((void*)(arBmpTbl[ucIndex].pu1Address), (void*)(_u4BmpFlashOffset + u4Offset), arBmpTbl[ucIndex].u4Size);
        LOG(5, "Load bitmap size %x from %x to %x\n", arBmpTbl[ucIndex].u4Size, _u4BmpFlashOffset + u4Offset, arBmpTbl[ucIndex].pu1Address);
        u4Offset += arBmpTbl[ucIndex].u4Size;
        ucIndex++;
    }

}

//-------------------------------------------------------------------------
/** _InitDrivers
 *  Initialize MT537x's various driver modules
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _InitDrivers(void)
{
   
    //RTC_HookAlertHandler(_RTCAlertHandler);
#if ENABLE_AUDIO_VIDEO
    DRV_Init();   
#else
    DRV_SYS_Init();
/*
    FBM_Init();
    VDP_Init();
    VERIFY(VdoMLInit());
    
    PMX_Init();
    VERIFY(PMX_SetBg(0, 0) == PMX_SET_OK);
    CLI_Parser("pmx.s.p 16");
    VERIFY(PMX_SetPlaneOrderArray(PMX_1, au4PlaneOrder) == PMX_SET_OK);
    VERIFY(PMX_SetEnable(0, 1) == PMX_SET_OK);
    */
    DRV_OSD_Init();
    DRV_GFX_Init();
#endif
    NAV_OpenNIM();
   
#ifdef CC_YES_1394
    // call it before setup menu
    NAV_I1394_Init();
#endif

    NAV_MM_Init(); 
}

//-------------------------------------------------------------------------
/** _InitInputSourceList
 *  Initialize supported input source list according to board configuration
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _InitInputSourceList(void)
{
    static BOOL *pfgInputSrc;
    UINT32 i = 0;
    
    NAV_CONFIG_GetInputSrc(&pfgInputSrc);
    
    // support src list in vdp 1
    prInputSrcList = WIDGET_NewList();

    // support src list in vdp 2
    prInputSrcListPIP = WIDGET_NewList();
    
   // we currently forbid DTV in vdp2
#ifdef __MODEL_slt__
    if(pfgInputSrc[E_NAV_VS_HDMI])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI");     
        WIDGET_AddListElement(prInputSrcList, "HDMI", (VOID*)E_NAV_VS_HDMI);
        WIDGET_AddListElement(prInputSrcListPIP, "HDMI", (VOID*)E_NAV_VS_HDMI);
    }
#endif
    if(pfgInputSrc[E_NAV_VS_DT1])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "DTV");   
        LOG(5, "ADD INPUT: %s\n", _arInputSourceOption[i - 1].szTitle);
        VERIFY(WIDGET_AddListElement(prInputSrcList, "DTV", (VOID*)E_NAV_VS_DT1) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "DTV", (VOID*)E_NAV_VS_DT1) == (INT32)E_WIDGET_MSG_OK);
    }
    
    if(pfgInputSrc[E_NAV_VS_CVBS1])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "AV1");   
        LOG(5, "ADD INPUT: %s\n", _arInputSourceOption[i - 1].szTitle);
        VERIFY(WIDGET_AddListElement(prInputSrcList, "AV1", (VOID*)E_NAV_VS_CVBS1) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "AV1", (VOID*)E_NAV_VS_CVBS1) == (INT32)E_WIDGET_MSG_OK);
    }

    if(pfgInputSrc[E_NAV_VS_CVBS2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "AV2");      
        LOG(5, "ADD INPUT: %s\n", _arInputSourceOption[i - 1].szTitle);
        VERIFY(WIDGET_AddListElement(prInputSrcList, "AV2", (VOID*)E_NAV_VS_CVBS2) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "AV2", (VOID*)E_NAV_VS_CVBS2) == (INT32)E_WIDGET_MSG_OK);
    }    
    if(pfgInputSrc[E_NAV_VS_CVBS3])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "AV3");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "AV3", (VOID*)E_NAV_VS_CVBS3) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "AV3", (VOID*)E_NAV_VS_CVBS3) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_CVBS4])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "AV4");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "AV4", (VOID*)E_NAV_VS_CVBS4) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "AV4", (VOID*)E_NAV_VS_CVBS4) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_S1])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "S-Video1");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "S-Video1", (VOID*)E_NAV_VS_S1) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "S-Video1", (VOID*)E_NAV_VS_S1) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_S2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "S-Video2");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "S-Video2", (VOID*)E_NAV_VS_S2) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "S-Video2", (VOID*)E_NAV_VS_S2) == (INT32)E_WIDGET_MSG_OK);
    } 
    if(pfgInputSrc[E_NAV_VS_S3])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "S-Video3");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "S-Video3", (VOID*)E_NAV_VS_S3) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "S-Video3", (VOID*)E_NAV_VS_S3) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_YPbPr1])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "YPbPr1");      
        VERIFY(WIDGET_AddListElement(prInputSrcList, "YPbPr1", (VOID*)E_NAV_VS_YPbPr1) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "YPbPr1", (VOID*)E_NAV_VS_YPbPr1) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_YPbPr2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "YPbPr2");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "YPbPr2", (VOID*)E_NAV_VS_YPbPr2) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "YPbPr2", (VOID*)E_NAV_VS_YPbPr2) == (INT32)E_WIDGET_MSG_OK);
    }
#ifndef __MODEL_slt__
    if(pfgInputSrc[E_NAV_VS_HDMI])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "HDMI", (VOID*)E_NAV_VS_HDMI) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "HDMI", (VOID*)E_NAV_VS_HDMI) == (INT32)E_WIDGET_MSG_OK);
    }   
#endif
    if(pfgInputSrc[E_NAV_VS_HDMI2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI2");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "HDMI2", (VOID*)E_NAV_VS_HDMI2) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "HDMI2", (VOID*)E_NAV_VS_HDMI2) == (INT32)E_WIDGET_MSG_OK);
    }   
    if(pfgInputSrc[E_NAV_VS_HDMI3])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI3");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "HDMI3", (VOID*)E_NAV_VS_HDMI3) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "HDMI3", (VOID*)E_NAV_VS_HDMI3) == (INT32)E_WIDGET_MSG_OK);
    }   
    if(pfgInputSrc[E_NAV_VS_HDMI4])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI4");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "HDMI4", (VOID*)E_NAV_VS_HDMI4) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "HDMI4", (VOID*)E_NAV_VS_HDMI4) == (INT32)E_WIDGET_MSG_OK);
    }       
    if(pfgInputSrc[E_NAV_VS_HDMI5])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "HDMI5");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "HDMI5", (VOID*)E_NAV_VS_HDMI5) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "HDMI5", (VOID*)E_NAV_VS_HDMI5) == (INT32)E_WIDGET_MSG_OK);
    }        
    if(pfgInputSrc[E_NAV_VS_VGA])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "VGA");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "VGA", (VOID*)E_NAV_VS_VGA) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "VGA", (VOID*)E_NAV_VS_VGA) == (INT32)E_WIDGET_MSG_OK);
    }    
#ifdef CC_YES_1394
    if(pfgInputSrc[E_NAV_VS_DT2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "I1394");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "I1394", (VOID*)E_NAV_VS_DT2) == (INT32)E_WIDGET_MSG_OK);
        //WIDGET_AddListElement(prInputSrcListPIP, "I1394", (VOID*)E_NAV_VS_DT2);
    }
#endif      

    if(pfgInputSrc[E_NAV_VS_SCART])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "SCART");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "SCART", (VOID*)E_NAV_VS_SCART) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "SCART", (VOID*)E_NAV_VS_SCART) == (INT32)E_WIDGET_MSG_OK);
    }
    if(pfgInputSrc[E_NAV_VS_SCART2])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "SCART2");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "SCART2", (VOID*)E_NAV_VS_SCART2) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "SCART2", (VOID*)E_NAV_VS_SCART2) == (INT32)E_WIDGET_MSG_OK);
    }  
    if(pfgInputSrc[E_NAV_VS_TUNER1])
    {
        _AddInputSourceItem(&_arInputSourceOption[i++], "ATV");
        VERIFY(WIDGET_AddListElement(prInputSrcList, "ATV", (VOID*)E_NAV_VS_TUNER1) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_AddListElement(prInputSrcListPIP, "ATV", (VOID*)E_NAV_VS_TUNER1) == (INT32)E_WIDGET_MSG_OK);
    }
    _u4NumInputSourceOption = i;

	// FIXME: index should not be hard coded
    _arSetupItem[0].u4NumSubItem =  _u4NumInputSourceOption;

    x_memset(_arVideoItem, 0, sizeof(NAV_MENUITEM_T) * _u4NumItemVideo);

    #if 0
    if (DRVCUST_OptGet(eFbmMemSize) != FBM_MEM_CFG_MT5371)
    {   
        x_memcpy(_arVideoItem, _arVideoItem5372, sizeof(NAV_MENUITEM_T) * _u4MaxNumItemVideo5372);
    }
    else
    {
        x_memcpy(_arVideoItem, _arVideoItem5370, sizeof(NAV_MENUITEM_T) * _u4MaxNumItemVideo5370);
    }
    #else
            x_memcpy(_arVideoItem, _arVideoItem5372, sizeof(NAV_MENUITEM_T) * _u4MaxNumItemVideo5372);
    #endif
}

//-------------------------------------------------------------------------
/** _InitWidget
 *  Initialize widget configuration such as width, height, color mode, and scaling factor;
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _InitWidget(void)
{

    //WIDGET_OSDINFO_T rWidgetSetting;
    //UINT32 u4Pitch;

/*
#ifdef USE_RGB8_CLUT 
    u4Pitch = 1;
    rWidgetSetting.u4X = (u4PmxW - 816) / 2;
    rWidgetSetting.u4Y = ((u4PmxH - 412) / 2) - 98;
    rWidgetSetting.u4W = 816;
    rWidgetSetting.u4H = 412 + 98;    
    rWidgetSetting.u4Plane = 0;
    rWidgetSetting.u4ColorMode = (UINT32)DRV_CM_RGB_8;
#else    
    u4Pitch = 2;
    rWidgetSetting.u4X = (u4PmxW - 808) / 2;
    rWidgetSetting.u4Y = ((u4PmxH - 412) / 2) - 98;
    rWidgetSetting.u4W = 808;
    rWidgetSetting.u4H = 412 + 98;    
    rWidgetSetting.u4Plane = 0;
    rWidgetSetting.u4ColorMode = (UINT32)DRV_CM_ARGB_4444;
#endif
*/

    /* OSD 1 for menu */
#ifdef USE_RGB8_CLUT     
    arWidgetOSD[0].u4X = (u4PmxW - 816) / 2;
    arWidgetOSD[0].u4Y = (u4PmxH - 412) / 2;
    arWidgetOSD[0].u4W = 816;
    arWidgetOSD[0].u4H = 412;    
    arWidgetOSD[0].u4Plane = 0;
    
    arWidgetOSD[0].u4ColorMode = (UINT32)DRV_CM_RGB_8;
#else
    arWidgetOSD[0].u4X = (u4PmxW - 808) / 2;
    arWidgetOSD[0].u4Y = (u4PmxH - 412) / 2;
    arWidgetOSD[0].u4W = 808;
    arWidgetOSD[0].u4H = 412;    
    arWidgetOSD[0].u4Plane = NAV_CONFIG_GetMenuOsdPlane();
    arWidgetOSD[0].u4ColorMode = (UINT32)DRV_CM_ARGB_4444;
#endif
    arWidgetOSD[0].fgEnable = TRUE;
    arWidgetOSD[0].fgNoTitleBmp = FALSE;     
    
    /* OSD 3 for msg */

#ifdef USE_RGB8_CLUT    
    arWidgetOSD[1].u4X = (u4PmxW - 816) / 2;
    arWidgetOSD[1].u4Y = ((u4PmxH - 412) / 2) - 98;
    arWidgetOSD[1].u4W = 816;
    arWidgetOSD[1].u4H = 98;
    arWidgetOSD[1].u4Plane = 0;
    arWidgetOSD[1].u4ColorMode = (UINT32)DRV_CM_RGB_8;
#else
    arWidgetOSD[1].u4X = (u4PmxW - 808) / 2;
    arWidgetOSD[1].u4Y = ((u4PmxH - 412) / 2) - 98;
    arWidgetOSD[1].u4W = 808;
    arWidgetOSD[1].u4H = 98;
    arWidgetOSD[1].u4Plane = NAV_CONFIG_GetMenuOsdPlane();
    arWidgetOSD[1].u4ColorMode = (UINT32)DRV_CM_ARGB_4444;
#endif
    arWidgetOSD[1].fgEnable = TRUE;
    arWidgetOSD[1].fgNoTitleBmp = FALSE;
    
    VERIFY(WIDGET_Init(&arWidgetOSD[0], 2) == (INT32)E_WIDGET_MSG_OK);
    DRV_OSD_SetRegionAlpha(arWidgetOSD[0].u4Region, 0xD0);
    DRV_OSD_SetRegionAlpha(arWidgetOSD[1].u4Region, 0xD0);
    


}

static void _AddInputSourceItem(NAV_MENUITEM_T* prItem, const CHAR* szName)
{    
    x_strcpy(prItem->szTitle, szName);    
    prItem->eItemType = E_NAV_MENUITEM_TYPE_LEAF;   
    prItem->pfOp = NULL;    prItem->u4NumSubItem = 0;    
    prItem->prSubItem = NULL;    
    prItem->i4Max = 0;    
    prItem->i4Min = 0;    
    prItem->i4Value = 0;    
}

//static void _RTCAlertHandler(void)
//{
    /*
    UINT64 u8Utc;
    RTC_T rRtc;
    RTC_GetTimeDate(&u8Utc);
    RTC_UtcToRtc(&rRtc, u8Utc);
    Printf("[RTC] %2d/%2d/%2d %2d:%2d:%2d\n", 
    	rRtc.u1Year,
    	rRtc.u1Month,
   	rRtc.u1Day,
    	rRtc.u1Hour,
    	rRtc.u1Minute,
    	rRtc.u1Second
    	);
   */ 	
//}

INT32 NAV_InitHandlers(void)
{
    VERIFY(WIDGET_SetIRHandler1KeyAction(NAV_IRHandler_1KeyAction) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_SetIRHandler2KeyAction(NAV_IRHandler_SetProg2Key) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_SetIRHandler3KeyAction(NAV_IRHandler_SetProg3Key) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_SetHandler(NAV_EventHandler) == (INT32)E_WIDGET_MSG_OK);
    return E_NAV_MSG_OK;
}

WIDGET_BITMAP_T* NAV_LookupBmp(const CHAR* szStr)
{
    UCHAR ucIndex = 0;
    while (arBmpTbl[ucIndex].szName != NULL)
    {
        if(x_strcmp(szStr, arBmpTbl[ucIndex].szName) == 0)
        {
            return &arBmpTbl[ucIndex];
        }
        ucIndex++;
    }
    return NULL;
}

INT32 NAV_Init(const NAV_OPTION_T* prOpt)
{
    static BOOL fgInit = FALSE;
    //UINT32 i = 0;     
    WIDGET_MENULIST_T* prList;
    
    VERIFY(prOpt != NULL);
    
    if(fgInit)
    {
        Printf("Navigator has been initialized\n");
        if(rNAVOption.fgDTVFullScan != prOpt->fgDTVFullScan)
        {
            rNAVOption.fgDTVFullScan = prOpt->fgDTVFullScan;
            if(rNAVOption.fgDTVFullScan)
            {
                Printf("Now using DTV full channel scan\n");
            }
            else 
            {
                Printf("Now using DTV backdoor channel scan\n");
            }
        }
        return (INT32)E_NAV_MSG_HAS_INIT;
    }

    fgInit = TRUE;

    VERIFY(_CopyOption(prOpt) == (INT32)E_NAV_MSG_OK);
    NAV_CONFIG_Init();

    WIDGET_StartPowerBtn();
    
    _InitDrivers();   

    u4PmxW = TV_WIDTH;
    u4PmxH = TV_HEIGHT;

#if 0
   if(0)
   {
   	 UINT32 u4HeaderOff;
        UINT32 u4DTVChSize = NAV_DTV_PROG_MAX_NUM * sizeof(PSIPSR_TBL_VCT_T);
        UINT32 u4ATVChSize = NAV_ATV_PROG_MAX_NUM * sizeof(NAV_ATV_TBL_PROG_T);
        CHAR szMagic[12];
        
        VERIFY(DRV_SYS_ReadNor(0, szMagic, x_strlen(NAV_NOR_MAGIC) + 1) == DRV_E_OK);
  
        if(x_strcmp(szMagic, NAV_NOR_MAGIC) == 0)
        {       
            u4HeaderOff = 12;              
            VERIFY(DRV_SYS_ReadNor(u4HeaderOff, (UINT32)&u4NumATVProg, sizeof(UINT32)) == DRV_E_OK);                
            u4HeaderOff += sizeof(UINT32);
            VERIFY(DRV_SYS_ReadNor(u4HeaderOff, (UINT32)&u4NumDTVProg, sizeof(UINT32)) == DRV_E_OK);   
            if(u4NumATVProg != 0)
            {
                u4HeaderOff += sizeof(UINT32);
                VERIFY(DRV_SYS_ReadNor(u4HeaderOff, (UINT32)&rCurrATVProg.u4ATVTblIdx, sizeof(UINT32)) == DRV_E_OK);   
                VERIFY(DRV_SYS_ReadNor(0x100, (UINT32)arATVTbl, u4ATVChSize) == DRV_E_OK);            
            }
            if(u4NumDTVProg != 0)
            {
                u4HeaderOff += sizeof(UINT32);
                VERIFY(DRV_SYS_ReadNor(u4HeaderOff, (UINT32)&rCurrDTVProg.u4VctTblIdx, sizeof(UINT32)) == DRV_E_OK);           
                VERIFY(DRV_SYS_ReadNor(0x100 + u4ATVChSize, (UINT32)arVctTbl, u4DTVChSize) == DRV_E_OK);
                VERIFY(DRV_SYS_ReadNor(0x100 + u4ATVChSize + u4DTVChSize, (UINT32)arTsTbl, NAV_DTV_PROG_MAX_NUM * sizeof(PSIPSR_TBL_TS2FREQ_T)) == DRV_E_OK);
            }
        }
    } 
#endif   
   
    _InitWidget();
    _InitInputSourceList();

//#ifndef __MODEL_slt__         
    _LoadBmp();
//#endif

    WIDGET_InitFont();
    
    prList = _SetupMenuList();

    VERIFY(WIDGET_ClearScreen(&(arWidgetOSD[0])) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_ClearScreen(&(arWidgetOSD[1])) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&(arWidgetOSD[0])) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&(arWidgetOSD[1])) == (INT32)E_WIDGET_MSG_OK);

    #if 0
    if (DRVCUST_OptGet(eFbmMemSize) != FBM_MEM_CFG_MT5371)
    {   
        VERIFY(WIDGET_ClearScreen(&(arWidgetOSD[1])) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_Flush(&(arWidgetOSD[1])) == (INT32)E_WIDGET_MSG_OK);
    }
    #endif

#if ENABLE_AUDIO_VIDEO
    VERIFY(NAV_InitAV() != (INT32)E_NAV_MSG_ERROR);          
#endif
    VERIFY(WIDGET_ShowMenuList(prList) == (INT32)E_WIDGET_MSG_OK);
    
    //DRV_OSD_SetColorkey(arWidgetOSD[0].u4Region, TRUE, 254);
    DRV_OSD_Enable(arWidgetOSD[0].u4Plane, TRUE);
    
    // Set IR key handlers 
    VERIFY(NAV_InitHandlers() == E_NAV_MSG_OK);
    
    VERIFY(WIDGET_StartIRLoop(arWidgetOSD) == (INT32)E_WIDGET_MSG_OK);

#ifdef __MODEL_slt__
    if(!_fgSltInit)
    {
        _fgSltInit = TRUE;
    }
#endif

    //Add three command to solve the issue of loader initialized panel already
    CLI_Parser("pmx.s.bg 0");
    CLI_Parser("pmx.e 0");
    CLI_Parser("pmx.e 1");

    if(rNAVOption.fgEnableAudio) 
    {
        //Driver will not init SPDIF automatically, so init SPDIF here
        CLI_Parser("aud.io.spdif.iecs 0 1");
    }
    
    return (INT32)E_NAV_MSG_OK;
}



