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
 * Description:
 *
 *---------------------------------------------------------------------------*/
#ifndef MTAL_IOCTL_H
#define MTAL_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include <linux/ioctl.h>

#ifndef SWDMX_DBG_USB
// #define SWDMX_DBG_USB
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

/// MTAL_IOCTL_2ARG_T: Send 2 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[2];
} MTAL_IOCTL_2ARG_T;

/// MTAL_IOCTL_4ARG_T: Send 3 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[3];
} MTAL_IOCTL_3ARG_T;

/// MTAL_IOCTL_4ARG_T: Send 4 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[4];

} MTAL_IOCTL_4ARG_T;

typedef struct
{
    INT32 ai4Arg[5];

} MTAL_IOCTL_5ARG_T;

/// MTAL_IOCTL_6ARG_T: Send 6 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[6];
} MTAL_IOCTL_6ARG_T;

/// MTAL_IOCTL_7ARG_T: Send 7 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[7];
} MTAL_IOCTL_7ARG_T; // 20110801

/// MTAL_IOCTL_8ARG_T: Send 8 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[8];
} MTAL_IOCTL_8ARG_T;

/// MTAL_IOCTL_9ARG_T: Send 9 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[9];
} MTAL_IOCTL_9ARG_T;

/// MTAL_IOCTL_10ARG_T: Send 10 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[10];
} MTAL_IOCTL_10ARG_T;

/// MTAL_IOCTL_11ARG_T: Send 11 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[11];
} MTAL_IOCTL_11ARG_T;

/// MTAL_IOCTL_12ARG_T: Send 12 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[12];
} MTAL_IOCTL_12ARG_T;

/// MTAL_IOCTL_16ARG_T: Send 16 Arguments between user/kernel mode (ioctl interface)
typedef struct
{
    INT32 ai4Arg[16];
} MTAL_IOCTL_16ARG_T;


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

/// MTAL device name
#define DEV_MTAL                        "mtal"

/*
#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2
*/

#define MTAL_IOCTYPE_MTADAC           0
#define MTAL_IOCTYPE_MTAUDDEC            1
#define MTAL_IOCTYPE_MTDMX          2
#define MTAL_IOCTYPE_MTFE_ATSC           3
#define MTAL_IOCTYPE_MTFE_COMMON               4
#define MTAL_IOCTYPE_MTFE_DVBT          5
#define MTAL_IOCTYPE_MTFE_DVBC          6
#define MTAL_IOCTYPE_MTFE_NTSC            7
#define MTAL_IOCTYPE_MTFE_PAL          8
#define MTAL_IOCTYPE_MTGFX            9
#define MTAL_IOCTYPE_MTMPLAYER              10
#define MTAL_IOCTYPE_MTOSD            11
#define MTAL_IOCTYPE_MTPERIPHERAL           12
#define MTAL_IOCTYPE_MTPMX           13
#define MTAL_IOCTYPE_MTSCART         14
#define MTAL_IOCTYPE_MTVDEC            15
#define MTAL_IOCTYPE_MTVDECEX           16
#define MTAL_IOCTYPE_MTVDO            17
#define MTAL_IOCTYPE_MTVFE           18
#define MTAL_IOCTYPE_MTCI           19
#define MTAL_IOCTYPE_MTVDECEX_VBI   20
#define MTAL_IOCTYPE_MTTVE  21
#define MTAL_IOCTYPE_MTFE_DTMB          22
#define MTAL_IOCTYPE_MTFE_ISDBT          23
#define MTAL_IOCTYPE_MTFE_ISDBS          24
#define MTAL_IOCTYPE_MTFE_SBTVD          25
#define MTAL_IOCTYPE_MTIMAGE                26
#define MTAL_IOCTYPE_MTPVR               27
#define MTAL_IOCTYPE_MTIOMMU             28
#define MTAL_IOCTYPE_MTSTC                  29
#define MTAL_IOCTYPE_MTB2R                  30
#define MTAL_IOCTYPE_MTVSS                  31
#define MTAL_IOCTYPE_MTDRM             32
#define MTAL_IOCTYPE_MTDRVCUST           33
#define MTAL_IOCTYPE_MTSYS                34
#define MTAL_IOCTYPE_MTSWDMX                35
#define MTAL_IOCTYPE_MTFEEDER               36
#define MTAL_IOCTYPE_MTRM                37
#define MTAL_IOCTYPE_MTFE_TUNERHAL 38
#define MTAL_IOCTYPE_MTCEC 39
#define MTAL_IOCTYPE_MTFE_UNIATD          40
#define MTAL_IOCTYPE_MTVENC                  41
#define MTAL_IOCTYPE_MTMX                  42
#define MTAL_IOCTYPE_MTTZ                  43
#define MTAL_IOCTYPE_MTPHOTO4K           44
#define MTAL_IOCTYPE_MHL                   45
#define MTAL_IOCTYPE_MTFE_TUNER            46
#define MTAL_IOCTYPE_MTMHL                 47
#define MTAL_IOCTYPE_MTAMB					48

/* MTAL_IOCTYPE_SDAUXOUT	0 */
#define MTAL_IO_ADAC_INIT                      _IOW(MTAL_IOCTYPE_MTADAC, 0, INT32)
#define MTAL_IO_ADAC_SETMUTE                   _IOW(MTAL_IOCTYPE_MTADAC, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ADAC_SETDACFORMATCFG           _IOW(MTAL_IOCTYPE_MTADAC, 2, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_ADAC_SETSPEAKERMUTE            _IOW(MTAL_IOCTYPE_MTADAC, 3, INT32)
#define MTAL_IO_ADAC_SETDACCHANNEL             _IOW(MTAL_IOCTYPE_MTADAC, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ADAC_SETAMPREGISTER            _IOW(MTAL_IOCTYPE_MTADAC, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ADAC_SETARCENABLE              _IOW(MTAL_IOCTYPE_MTADAC, 6, INT32)
#define MTAL_IO_ADAC_HWMUTE                    _IOW(MTAL_IOCTYPE_MTADAC, 7, INT32)
#define MTAL_IO_ADAC_SIFSEL                    _IOW(MTAL_IOCTYPE_MTADAC, 8, INT32)
#define MTAL_IO_ADAC_ADJUSTI2SCLOCK			   _IOW(MTAL_IOCTYPE_MTADAC, 9, INT32)
#define MTAL_IO_ADAC_SETSUBSPEAKERMUTE		   _IOW(MTAL_IOCTYPE_MTADAC, 10, INT32)

/*MTAL_IOCTYPE_MTAUDDEC	1*/
#define MTAL_IO_AUDDEC_INIT                              _IOW(MTAL_IOCTYPE_MTAUDDEC, 0, INT32)
#define MTAL_IO_AUDDEC_GETDECTYPE                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETDECTYPE                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 2, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_PLAY                             _IOW(MTAL_IOCTYPE_MTAUDDEC, 3, INT32)
#define MTAL_IO_AUDDEC_RESUME                         _IOW(MTAL_IOCTYPE_MTAUDDEC, 4, INT32)
#define MTAL_IO_AUDDEC_PAUSE                           _IOW(MTAL_IOCTYPE_MTAUDDEC, 5, INT32)
#define MTAL_IO_AUDDEC_STOP                             _IOW(MTAL_IOCTYPE_MTAUDDEC, 6, INT32)
#define MTAL_IO_AUDDEC_GETDECONPLAYINGSTATUS    _IOW(MTAL_IOCTYPE_MTAUDDEC, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAVSYNMODE            _IOW(MTAL_IOCTYPE_MTAUDDEC, 8, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETAC3COMPMODE        _IOW(MTAL_IOCTYPE_MTAUDDEC, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAC3DOWNMIXMODE          _IOW(MTAL_IOCTYPE_MTAUDDEC, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAC3DRCRANGE         _IOW(MTAL_IOCTYPE_MTAUDDEC, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETATVDETECTMODE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETATVOUTPUTMODE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSIFMODE                           _IOW(MTAL_IOCTYPE_MTAUDDEC, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETDETECTSIFMODE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 15, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETPCMINFO                           _IOW(MTAL_IOCTYPE_MTAUDDEC, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELDELAY                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 17, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELVOLUME              _IOW(MTAL_IOCTYPE_MTAUDDEC, 18, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELMUTE                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 19, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELVOLGAIN            _IOW(MTAL_IOCTYPE_MTAUDDEC, 20, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETBALANCE                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETMASTERVOLUME      _IOW(MTAL_IOCTYPE_MTAUDDEC, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAVC                         _IOW(MTAL_IOCTYPE_MTAUDDEC, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBBE                         _IOW(MTAL_IOCTYPE_MTAUDDEC, 24, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSRS                         _IOW(MTAL_IOCTYPE_MTAUDDEC, 25, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETVIRTUALSURROUND          _IOW(MTAL_IOCTYPE_MTAUDDEC, 26, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBASSBOOST             _IOW(MTAL_IOCTYPE_MTAUDDEC, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBASS                        _IOW(MTAL_IOCTYPE_MTAUDDEC, 28, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETTREBLEBOOST         _IOW(MTAL_IOCTYPE_MTAUDDEC, 29, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETTREBLE                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETEQUALIZER              _IOW(MTAL_IOCTYPE_MTAUDDEC, 31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETEQUALIZERBANDCONFIG   _IOW(MTAL_IOCTYPE_MTAUDDEC, 32, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_AUDDEC_SETAVINPUTMUXSEL      _IOW(MTAL_IOCTYPE_MTAUDDEC, 33, INT32)
#define MTAL_IO_AUDDEC_SETIECCONFIG               _IOW(MTAL_IOCTYPE_MTAUDDEC, 34, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETIECCOPYRIGHT         _IOW(MTAL_IOCTYPE_MTAUDDEC, 35, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETIECCATEGORYCODE  _IOW(MTAL_IOCTYPE_MTAUDDEC, 36, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSPEAKEROUTMODE   _IOW(MTAL_IOCTYPE_MTAUDDEC, 37, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETDUALMODE      _IOW(MTAL_IOCTYPE_MTAUDDEC, 38, INT32)
#ifdef CC_AUD_DDI
#define MTAL_IO_AUDDEC_SETTRIMODE      _IOW(MTAL_IOCTYPE_MTAUDDEC, 39, INT32)
#endif

#define MTAL_IO_AUDDEC_SETHPFFC                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 39, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETHPF                       _IOW(MTAL_IOCTYPE_MTAUDDEC, 40, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETPEQCFG                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 41, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_AUDDEC_SETPEQ                       _IOW(MTAL_IOCTYPE_MTAUDDEC, 42, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBASSENHANCERCFG           _IOW(MTAL_IOCTYPE_MTAUDDEC, 43, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETVIRTUALSURROUDCFG         _IOW(MTAL_IOCTYPE_MTAUDDEC, 44, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETAVCCFG                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 45, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETMASTERVOLUMEFORBYPASSCHANNELINDB  _IOW(MTAL_IOCTYPE_MTAUDDEC, 46, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETMASTERVOLUMEINDB          _IOW(MTAL_IOCTYPE_MTAUDDEC, 47, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBALANCEINDB               _IOW(MTAL_IOCTYPE_MTAUDDEC, 48, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELVOLUMEINDB         _IOW(MTAL_IOCTYPE_MTAUDDEC, 49, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETBASSENHANCER              _IOW(MTAL_IOCTYPE_MTAUDDEC, 50, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETIECCHANNEL                _IOW(MTAL_IOCTYPE_MTAUDDEC, 51, INT32)
#define MTAL_IO_AUDDEC_SETLIMITER                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 52, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETLIMITERCFG                _IOW(MTAL_IOCTYPE_MTAUDDEC, 53, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_SETCLIPPERTHRESHOLDINDB      _IOW(MTAL_IOCTYPE_MTAUDDEC, 54, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETPREGAININDB               _IOW(MTAL_IOCTYPE_MTAUDDEC, 55, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETAC3DOWNMIXMODE            _IOW(MTAL_IOCTYPE_MTAUDDEC, 56, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETAC3MODE                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 57, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETSAMPLINGRATE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 58, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAC3DUALMONO               _IOW(MTAL_IOCTYPE_MTAUDDEC, 59, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETAC3DUALMONO               _IOW(MTAL_IOCTYPE_MTAUDDEC, 60, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETIECCOPYRIGHT              _IOW(MTAL_IOCTYPE_MTAUDDEC, 61, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETIECCATEGORYCODE           _IOW(MTAL_IOCTYPE_MTAUDDEC, 62, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETIECWORDLENGTH             _IOW(MTAL_IOCTYPE_MTAUDDEC, 63, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETIECWORDLENGTH             _IOW(MTAL_IOCTYPE_MTAUDDEC, 64, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SET2BANDAVCCFG               _IOW(MTAL_IOCTYPE_MTAUDDEC, 65, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_GETATVSOUNDSYS               _IOW(MTAL_IOCTYPE_MTAUDDEC, 66, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETHDEVMODE                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 67, INT32)
#define MTAL_IO_AUDDEC_STOPALL                      _IOW(MTAL_IOCTYPE_MTAUDDEC, 68, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCV                        _IOW(MTAL_IOCTYPE_MTAUDDEC, 69, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCVCFG                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 70, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETADMODE                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 71, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_REGCB                        _IOW(MTAL_IOCTYPE_MTAUDDEC, 72, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_SETMIXSOUNDPLAY              _IOW(MTAL_IOCTYPE_MTAUDDEC, 73, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_SETMIXSOUNDSTOP              _IOW(MTAL_IOCTYPE_MTAUDDEC, 74, INT32)
#define MTAL_IO_AUDDEC_SETMIXSOUNDPAUSE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 75, INT32)
#define MTAL_IO_AUDDEC_SETMIXSOUNDRESUME            _IOW(MTAL_IOCTYPE_MTAUDDEC, 76, INT32)
#define MTAL_IO_AUDDEC_SETSOURCEVOLUME              _IOW(MTAL_IOCTYPE_MTAUDDEC, 77, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELDELAYNOUOP         _IOW(MTAL_IOCTYPE_MTAUDDEC, 78, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETADENABLE                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 79, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_LOADAUDIOCLIP                _IOW(MTAL_IOCTYPE_MTAUDDEC, 80, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_AUDDEC_PLAYAUDIOCLIP                _IOW(MTAL_IOCTYPE_MTAUDDEC, 81, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_STOPAUDIOCLIP                _IOW(MTAL_IOCTYPE_MTAUDDEC, 82, INT32)
#define MTAL_IO_AUDDEC_DELETEAUDIOCLIP              _IOW(MTAL_IOCTYPE_MTAUDDEC, 83, INT32)
#define MTAL_IO_AUDDEC_GETAUDIOPTS                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 84, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSPDIFSCMS                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 85, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETAC3ESINFO                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 86, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETMPEGESINFO                _IOW(MTAL_IOCTYPE_MTAUDDEC, 87, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETHEAACESINFO               _IOW(MTAL_IOCTYPE_MTAUDDEC, 88, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAVINPUTMUXSELBYAD         _IOW(MTAL_IOCTYPE_MTAUDDEC, 89, INT32)
#define MTAL_IO_AUDDEC_SETAC3DRCRANGE100            _IOW(MTAL_IOCTYPE_MTAUDDEC, 90, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETATVSOUNDSYSSTRENGTH       _IOW(MTAL_IOCTYPE_MTAUDDEC, 91, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETDSPSPEEDMODE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 92, INT32)
#define MTAL_IO_AUDDEC_ISSIFEXIST                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 93, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBTSCA2THRESH              _IOW(MTAL_IOCTYPE_MTAUDDEC, 94, INT32)
#define MTAL_IO_AUDDEC_LGSEFN000                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 95,MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AUDDEC_SETSPDIFMUTE                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 96, INT32)
#define MTAL_IO_AUDDEC_LGSEFN0010_READ              _IOW(MTAL_IOCTYPE_MTAUDDEC, 97,INT32)
#define MTAL_IO_AUDDEC_GETISESEXIST                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 98,MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETDECONPLAYINGSTATUSREAL    _IOW(MTAL_IOCTYPE_MTAUDDEC, 99, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSOURCEFROM                _IOW(MTAL_IOCTYPE_MTAUDDEC, 100,MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCHANNELVOLUMENEW          _IOW(MTAL_IOCTYPE_MTAUDDEC, 101, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_SETDOLBYDRCMODE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 102,MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETDOWNMIXMODE               _IOW(MTAL_IOCTYPE_MTAUDDEC, 103,MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAACBYPASS                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 104, INT32)
#define MTAL_IO_AUDDEC_GETNICAMTHRESH               _IOW(MTAL_IOCTYPE_MTAUDDEC, 105, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETNICAMTHRESH               _IOW(MTAL_IOCTYPE_MTAUDDEC, 106, INT32)
#define MTAL_IO_AUDDEC_GETHDMISCMS                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 107, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTAUD_OpenPCM						_IOW(MTAL_IOCTYPE_MTAUDDEC, 108, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTAUD_ClosePCM					    _IOW(MTAL_IOCTYPE_MTAUDDEC, 109, INT32)
#define MTAL_IO_AUDDEC_GetPCMDelay		            _IOW(MTAL_IOCTYPE_MTAUDDEC, 110, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_MTAUD_FillPCMBuffer		    _IOW(MTAL_IOCTYPE_MTAUDDEC, 111, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETSOUNDBAR                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 112, INT32)
#define MTAL_IO_AUDDEC_SETSOUNDBARIDDATA		    _IOW(MTAL_IOCTYPE_MTAUDDEC, 113, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_GETSOUNDBARSTATUS		    _IOW(MTAL_IOCTYPE_MTAUDDEC, 114, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBLUETOOTHVOLUME		    _IOW(MTAL_IOCTYPE_MTAUDDEC, 115, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SET_CARRIER_MUTE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 116, INT32)
#define MTAL_IO_AUDDEC_GETAQVERSION                            _IOW(MTAL_IOCTYPE_MTAUDDEC, 117, INT32)
#define MTAL_IO_AUDDEC_SETPRESSKEYSOUNDFLAG             _IOW(MTAL_IOCTYPE_MTAUDDEC, 118, INT32)
#define MTAL_IO_AUDDEC_SETPRESSKEYVOLUME                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 119, INT32)
#define MTAL_IO_AUDDEC_SETPOWERONMUSIC                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 120, INT32)
#define MTAL_IO_AUDDEC_GETPOWERONMUSIC                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 121, INT32)
#define MTAL_IO_AUDDEC_SETMICPHONETYPE                       _IOW(MTAL_IOCTYPE_MTAUDDEC, 122, INT32)
#define MTAL_IO_AUDDEC_GETSPECTRUMINFO              		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 123, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETEQBANDCFG                		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 124, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETEQUALZEREFFECT          		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 125, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_GETCHANNELDELAY         		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 126, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_GETUSERDEFINEEQCFG         		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 127, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETDRVCFGQUALIZER         		 _IOW(MTAL_IOCTYPE_MTAUDDEC, 128, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_AUDDEC_AUD_DspSetPEQCfg2            _IOW(MTAL_IOCTYPE_MTAUDDEC, 129, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AUDDEC_AUD_DspGetPEQCfg2            _IOW(MTAL_IOCTYPE_MTAUDDEC, 130, MTAL_IOCTL_5ARG_T)

#define MTAL_IO_AUDDEC_AUD_DspSetVolCurveTable                      _IOW(MTAL_IOCTYPE_MTAUDDEC, 131, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_AUD_DspSetMasterVolRegData               _IOW(MTAL_IOCTYPE_MTAUDDEC, 132, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_AUD_DspSetExternalVolRegData             _IOW(MTAL_IOCTYPE_MTAUDDEC, 133, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETPRESSKEYSOUNDMUTE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 134, INT32)
#define MTAL_IO_MTAUD_SetAoutGain                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 135, INT32)
#define MTAL_IO_AUDDEC_QueryAudioInfo              _IOW(MTAL_IOCTYPE_MTAUDDEC, 136, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_AUDDEC_SETAUDIOOUTMODE   			_IOW(MTAL_IOCTYPE_MTAUDDEC, 137, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_RM_RESALLOC                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 138, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_SETBIQUAD					_IOW(MTAL_IOCTYPE_MTAUDDEC, 139, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBIQUADCFG					_IOW(MTAL_IOCTYPE_MTAUDDEC, 140, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_AUDDEC_SETBASSBOOST_OUTCTRL         _IOW(MTAL_IOCTYPE_MTAUDDEC, 141, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETBASS_OUTCTRL              _IOW(MTAL_IOCTYPE_MTAUDDEC, 142, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETTREBLEBOOST_OUTCTRL       _IOW(MTAL_IOCTYPE_MTAUDDEC, 143, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETTREBLE_OUTCTRL            _IOW(MTAL_IOCTYPE_MTAUDDEC, 144, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETCLEARSOUND              	_IOW(MTAL_IOCTYPE_MTAUDDEC, 145, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSRSTRUVOLUME				_IOW(MTAL_IOCTYPE_MTAUDDEC, 146, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSRSTRUBASS				_IOW(MTAL_IOCTYPE_MTAUDDEC, 147, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETSUB2LR					_IOW(MTAL_IOCTYPE_MTAUDDEC, 148, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GETAUDIOINFO					_IOW(MTAL_IOCTYPE_MTAUDDEC, 149, MTAL_IOCTL_2ARG_T)

#if 1//def CC_AUD_MULTI_BUFFER_MEMORY_PLAY
#define MTAL_IO_AUDDEC_MULTIBUFFERSETMODE			_IOW(MTAL_IOCTYPE_MTAUDDEC, 150, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_MULTIBUFFERADDDATA			_IOW(MTAL_IOCTYPE_MTAUDDEC, 151, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_MULTIBUFFERGETMAXBUFFNUM		_IOW(MTAL_IOCTYPE_MTAUDDEC, 152, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_MULTIBUFFERGETREMAININGBUFFNUM	_IOW(MTAL_IOCTYPE_MTAUDDEC, 153, MTAL_IOCTL_2ARG_T)
#endif

#define MTAL_IO_AUDDEC_SETMUTEALL					_IOW(MTAL_IOCTYPE_MTAUDDEC, 160, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SETAUTOMUTE					_IOW(MTAL_IOCTYPE_MTAUDDEC, 161, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_AUDDEC_SET							_IOW(MTAL_IOCTYPE_MTAUDDEC, 170, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_GET							_IOW(MTAL_IOCTYPE_MTAUDDEC, 171, MTAL_IOCTL_4ARG_T)

//0x80~ MTK specific tool for 3rd party
//#define MTAL_IO_AUDDEC_RECORDPCM                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 192, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_AUDDEC_DOWNLOAD_DRAM                _IOW(MTAL_IOCTYPE_MTAUDDEC, 193, MTAL_IOCTL_3ARG_T)
//#define MTAL_IO_AUDDEC_UPLOAD_DRAM                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 194, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_RECORDPCM_OPEN               _IOW(MTAL_IOCTYPE_MTAUDDEC, 192, INT32)
#define MTAL_IO_AUDDEC_RECORDPCM_READ               _IOW(MTAL_IOCTYPE_MTAUDDEC, 193, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_RECORDPCM_CLOSE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 194, INT32)

//ALSA Backup
#define MTAL_IO_AUDDEC_PLAYPCM_OPEN                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 195, INT32)    // &u1StreamId
#define MTAL_IO_AUDDEC_PLAYPCM_CLOSE                _IOW(MTAL_IOCTYPE_MTAUDDEC, 196, INT32)    // u1StreamId
#define MTAL_IO_AUDDEC_PLAYPCM_IOCTL                _IOW(MTAL_IOCTYPE_MTAUDDEC, 197, MTAL_IOCTL_5ARG_T)    // u1StreamId,u4SampleRate,u1StereoOnOff,u1BitDepth,u4BufferSize
#define MTAL_IO_AUDDEC_PLAYPCM_WRITE                _IOW(MTAL_IOCTYPE_MTAUDDEC, 198, MTAL_IOCTL_3ARG_T)    // u1StreamId,u4BufPtr,u4BufSize
#define MTAL_IO_AUDDEC_PLAYPCM_QUERY                _IOW(MTAL_IOCTYPE_MTAUDDEC, 199, MTAL_IOCTL_4ARG_T)    // u1StreamId,&pu4RenderSize
//End

//GST/OMX
#define MTAL_IO_AOMX_GETGSTDBGMSK                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 200, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AOMX_SETGSTDBGMSK                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 201, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AOMX_TRIGRENDER                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 202, INT32)
#define MTAL_IO_AOMX_GETCURPTS                      _IOW(MTAL_IOCTYPE_MTAUDDEC, 203, INT32)
//End

#define MTAL_IO_AUDDEC_MW_AUD_Init                  _IOW(MTAL_IOCTYPE_MTAUDDEC, 204, INT32)
#define MTAL_IO_AUDDEC_d_custom_aud_get_min_max     _IOW(MTAL_IOCTYPE_MTAUDDEC, 205, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_d_custom_aud_get             _IOW(MTAL_IOCTYPE_MTAUDDEC, 206, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_d_custom_aud_set             _IOW(MTAL_IOCTYPE_MTAUDDEC, 207, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_AudSet                       _IOW(MTAL_IOCTYPE_MTAUDDEC, 208, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AUDDEC_AudGet                       _IOW(MTAL_IOCTYPE_MTAUDDEC, 209, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AUDDEC_AudDisconnect                _IOW(MTAL_IOCTYPE_MTAUDDEC, 210, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_AudConnect                   _IOW(MTAL_IOCTYPE_MTAUDDEC, 211, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_AUD_IsDecoderRealPlay        _IOW(MTAL_IOCTYPE_MTAUDDEC, 212, INT32)
#define MTAL_IO_AUDDEC_AUD_DSPCmdPlay               _IOW(MTAL_IOCTYPE_MTAUDDEC, 213, INT32)
#define MTAL_IO_AUDDEC_AUD_Read32ByteID             _IOW(MTAL_IOCTYPE_MTAUDDEC, 214, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_AUD_IsHeadphoneExist         _IOW(MTAL_IOCTYPE_MTAUDDEC, 215, INT32)
#define MTAL_IO_AUDDEC_AUD_PlaySetupMelody          _IOW(MTAL_IOCTYPE_MTAUDDEC, 216, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AOMX_GETLASTPTS                     _IOW(MTAL_IOCTYPE_MTAUDDEC, 217, INT32)
#define MTAL_IO_AUDDEC_SETPCMVOLUME		            _IOW(MTAL_IOCTYPE_MTAUDDEC, 218, MTAL_IOCTL_2ARG_T)

// SRS test params
#define MTAL_IO_AUDDEC_SETTESTPARAM		            _IOW(MTAL_IOCTYPE_MTAUDDEC, 219, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_AUDENC_PLAY		                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 220, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDENC_COPY_DATA		            _IOW(MTAL_IOCTYPE_MTAUDDEC, 221, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AUDENC_STOP		                    _IOW(MTAL_IOCTYPE_MTAUDDEC, 222, INT32)
#define MTAL_IO_AUDDEC_UPLOAD_ENABLE		        _IOW(MTAL_IOCTYPE_MTAUDDEC, 223, INT32) //for bluetooth
#define MTAL_IO_AUDENC_INFORM_MUXER                 _IOW(MTAL_IOCTYPE_MTAUDDEC, 224, MTAL_IOCTL_3ARG_T) //for audio pes to mtk muxer

#ifdef CC_AUD_DDI
#define MTAL_IO_AUDENC_SETENCSOURCEFROM             _IOW(MTAL_IOCTYPE_MTAUDDEC, 225, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_GetRESOURCE		            _IOW(MTAL_IOCTYPE_MTAUDDEC, 226, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_SETDMXID		                _IOW(MTAL_IOCTYPE_MTAUDDEC, 227, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_SetGstId		                _IOW(MTAL_IOCTYPE_MTAUDDEC, 228, INT32)
#define MTAL_IO_AUDDEC_SetGstEncFlag                _IOW(MTAL_IOCTYPE_MTAUDDEC, 229, INT32)
#define MTAL_IO_AUDBLUETOOTH_COPY_DATA		        _IOW(MTAL_IOCTYPE_MTAUDDEC, 230, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDENC_GETSTC		                _IOW(MTAL_IOCTYPE_MTAUDDEC, 231, INT32)
#define MTAL_IO_AUDDEC_SET_BLUETOOTHCNT             _IOW(MTAL_IOCTYPE_MTAUDDEC, 232, INT32)
#define MTAL_IO_AUDDEC_SET_PLAYMUTEFLAG             _IOW(MTAL_IOCTYPE_MTAUDDEC, 233, INT32)
#define MTAL_IO_AUDIODELAYMODE                      _IOW(MTAL_IOCTYPE_MTAUDDEC, 234, INT32)
#define MTAL_IO_AUDDEC_SET_BYPASS_LGSE              _IOW(MTAL_IOCTYPE_MTAUDDEC, 235, INT32)
#endif
#define MTAL_IO_AUDDEC_SETPREGAININCHANNELDB        _IOW(MTAL_IOCTYPE_MTAUDDEC, 236, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AUDDEC_AUD_GetDecoderState			_IOW(MTAL_IOCTYPE_MTAUDDEC, 237, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_AUD_GetAoutBankNum			_IOW(MTAL_IOCTYPE_MTAUDDEC, 238, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_AUD_FlushAFIFO				_IOW(MTAL_IOCTYPE_MTAUDDEC, 239, INT32)
#define MTAL_IO_AUDDEC_AUD_FlushAudio			    _IOW(MTAL_IOCTYPE_MTAUDDEC, 240, INT32)

//For Upload2()
#define MTAL_IO_AUDDEC_RECORD2PCM_OPEN              _IOW(MTAL_IOCTYPE_MTAUDDEC, 241, INT32)
#define MTAL_IO_AUDDEC_RECORD2PCM_READ              _IOW(MTAL_IOCTYPE_MTAUDDEC, 242, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AUDDEC_RECORD2PCM_CLOSE             _IOW(MTAL_IOCTYPE_MTAUDDEC, 243, INT32)
#define MTAL_IO_AUDDEC_SETSTCID		                _IOW(MTAL_IOCTYPE_MTAUDDEC, 244, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AUDDEC_USER_SET_COMMAND             _IOW(MTAL_IOCTYPE_MTAUDDEC, 245, MTAL_IOCTL_6ARG_T)


/* MTAL_IOCTYPE_DMXOUT 2 */
#define MTAL_IO_DMX_RESET                      _IOW(MTAL_IOCTYPE_MTDMX, 0, INT32)
#define MTAL_IO_DMX_SETFRONTENDEX              _IOW(MTAL_IOCTYPE_MTDMX, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_START                      _IOW(MTAL_IOCTYPE_MTDMX, 2, INT32)
#define MTAL_IO_DMX_STOP                       _IOW(MTAL_IOCTYPE_MTDMX, 3, INT32)
#define MTAL_IO_DMX_SETPID                     _IOW(MTAL_IOCTYPE_MTDMX, 4, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_GETPID                     _IOW(MTAL_IOCTYPE_MTDMX, 5, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_FREEPID                    _IOW(MTAL_IOCTYPE_MTDMX, 6, INT32)
#define MTAL_IO_DMX_GETPIDIDX                  _IOW(MTAL_IOCTYPE_MTDMX, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETVDEPID                  _IOW(MTAL_IOCTYPE_MTDMX, 8, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETAUDPID                  _IOW(MTAL_IOCTYPE_MTDMX, 9, MTAL_IOCTL_3ARG_T)
//#define MTAL_IO_DMX_SETPCRPID                  _IOW(MTAL_IOCTYPE_MTDMX, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETPCR                     _IOW(MTAL_IOCTYPE_MTDMX, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETFTR                     _IOW(MTAL_IOCTYPE_MTDMX, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_GETFTR                     _IOW(MTAL_IOCTYPE_MTDMX, 13, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_PSI_ALCBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_PSI_FREEBFR                _IOW(MTAL_IOCTYPE_MTDMX, 15, INT32)
#define MTAL_IO_DMX_PSI_GETBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 16, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_DMX_PSI_ULKBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 17, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DMX_PSI_FLSBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 20, INT32)
#define MTAL_IO_DMX_PSI_GETBFRSECHEAD          _IOW(MTAL_IOCTYPE_MTDMX, 21, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_DMX_PES_ALKBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_PES_FREEBFR                _IOW(MTAL_IOCTYPE_MTDMX, 23, INT32)
#define MTAL_IO_DMX_PES_ULKBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 24, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DMX_PES_FLSBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 25, INT32)
#define MTAL_IO_DMX_PES_GETBFR                 _IOW(MTAL_IOCTYPE_MTDMX, 26, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_DMX_GETPIDCNT                  _IOW(MTAL_IOCTYPE_MTDMX, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_RSTPIDCNT                  _IOW(MTAL_IOCTYPE_MTDMX, 28, INT32)
#define MTAL_IO_DMX_GETLOKSTE                  _IOW(MTAL_IOCTYPE_MTDMX, 29, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_GETSCRMSTE                 _IOW(MTAL_IOCTYPE_MTDMX, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_INIT                       _IOW(MTAL_IOCTYPE_MTDMX, 31, INT32)
#define MTAL_IO_DMX_SETVIDEOTYPE               _IOW(MTAL_IOCTYPE_MTDMX, 32, INT32)
#define MTAL_IO_DMX_GETCURSTC                  _IOW(MTAL_IOCTYPE_MTDMX, 33, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETGENERICFILTER           _IOW(MTAL_IOCTYPE_MTDMX, 34, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_GETGENERICFILTER           _IOW(MTAL_IOCTYPE_MTDMX, 35, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETAUDDESPID               _IOW(MTAL_IOCTYPE_MTDMX, 36, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETDESKEY                  _IOW(MTAL_IOCTYPE_MTDMX, 37, MTAL_IOCTL_4ARG_T)
//#define MTAL_IO_DMX_SETDESIV                   _IOW(MTAL_IOCTYPE_MTDMX, 38, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETAESKEYLEN               _IOW(MTAL_IOCTYPE_MTDMX, 39, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETAESKEY                  _IOW(MTAL_IOCTYPE_MTDMX, 40, MTAL_IOCTL_3ARG_T)
//#define MTAL_IO_DMX_SETAESIV                   _IOW(MTAL_IOCTYPE_MTDMX, 41, INT32)
#define MTAL_IO_DMX_SETDESCPID                 _IOW(MTAL_IOCTYPE_MTDMX, 42, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETAESEVENODDIV            _IOW(MTAL_IOCTYPE_MTDMX, 43, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETAESON                   _IOW(MTAL_IOCTYPE_MTDMX, 44, INT32)
#define MTAL_IO_DMX_AES_SETDCRINFO             _IOW(MTAL_IOCTYPE_MTDMX, 45, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_AES_MVBLKDATA              _IOW(MTAL_IOCTYPE_MTDMX, 46, INT32)
#define MTAL_IO_DMX_FVR_INIT                   _IOW(MTAL_IOCTYPE_MTDMX, 47, INT32)
#define MTAL_IO_DMX_FVR_SETGBL                 _IOW(MTAL_IOCTYPE_MTDMX, 48, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_FVR_GETGBL                 _IOW(MTAL_IOCTYPE_MTDMX, 49, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_FVR_SETPID                 _IOW(MTAL_IOCTYPE_MTDMX, 50, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_FVR_GETPID                 _IOW(MTAL_IOCTYPE_MTDMX, 51, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_FVR_FREEPID                _IOW(MTAL_IOCTYPE_MTDMX, 52, INT32)
#define MTAL_IO_DMX_FVR_UNLOCK_BUF             _IOW(MTAL_IOCTYPE_MTDMX, 53, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_FVR_FLUSH_BUF              _IOW(MTAL_IOCTYPE_MTDMX, 54, INT32)
#define MTAL_IO_DMX_FVR_START                  _IOW(MTAL_IOCTYPE_MTDMX, 55, INT32)
#define MTAL_IO_DMX_FVR_STOP                   _IOW(MTAL_IOCTYPE_MTDMX, 56, INT32)
#define MTAL_IO_DMX_FVR_ISRECORDING            _IOW(MTAL_IOCTYPE_MTDMX, 57, INT32)
#define MTAL_IO_DMX_FVR_ALLOCMETABUF           _IOW(MTAL_IOCTYPE_MTDMX, 58, INT32)
#define MTAL_IO_DMX_FVR_SETBUFFER              _IOW(MTAL_IOCTYPE_MTDMX, 59, INT32)
#define MTAL_IO_DMX_FVR_GETBUFFERPTR           _IOW(MTAL_IOCTYPE_MTDMX, 60, INT32)
#define MTAL_IO_DMX_FVR_FREEGBL                _IOW(MTAL_IOCTYPE_MTDMX, 61, INT32)
#define MTAL_IO_DMX_SETPID_INSRC               _IOW(MTAL_IOCTYPE_MTDMX, 62, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_FVR_SETKEY                 _IOW(MTAL_IOCTYPE_MTDMX, 63, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_MULTI2_CFG                 _IOW(MTAL_IOCTYPE_MTDMX, 64, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_MULTI2_KEY                 _IOW(MTAL_IOCTYPE_MTDMX, 65, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_MULTI2_CBCIV               _IOW(MTAL_IOCTYPE_MTDMX, 66, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_MULTI2_OFBIV               _IOW(MTAL_IOCTYPE_MTDMX, 67, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_MULTI2_SYSKEY              _IOW(MTAL_IOCTYPE_MTDMX, 68, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_SETRTBMODE                 _IOW(MTAL_IOCTYPE_MTDMX, 69, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_DMX_SETAESRTBMODE              _IOW(MTAL_IOCTYPE_MTDMX, 70, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_SETDESEVENODDIV            _IOW(MTAL_IOCTYPE_MTDMX, 71, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_DMX_DREC_INIT                  _IOW(MTAL_IOCTYPE_MTDMX, 72, INT32)
#define MTAL_IO_DMX_DREC_SETPID                _IOW(MTAL_IOCTYPE_MTDMX, 73, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_UNLOCK_BUF            _IOW(MTAL_IOCTYPE_MTDMX, 74, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_START                 _IOW(MTAL_IOCTYPE_MTDMX, 75, INT32)
#define MTAL_IO_DMX_DREC_STOP                  _IOW(MTAL_IOCTYPE_MTDMX, 76, INT32)
#define MTAL_IO_DMX_DREC_SETCIPHER             _IOW(MTAL_IOCTYPE_MTDMX, 77, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_DREC_GETSTREAMINFO         _IOW(MTAL_IOCTYPE_MTDMX, 78, INT32)
#define MTAL_IO_DMX_DREC_GETSTREAM             _IOW(MTAL_IOCTYPE_MTDMX, 79, INT32)
#define MTAL_IO_DMX_DREC_STOPSTREAM            _IOW(MTAL_IOCTYPE_MTDMX, 80, INT32)
#define MTAL_IO_DMX_DREC_FREEPID               _IOW(MTAL_IOCTYPE_MTDMX, 81, INT32)
#define MTAL_IO_DMX_DREC_GETBUFFERPTR          _IOW(MTAL_IOCTYPE_MTDMX, 82, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_GETREMAINDER          _IOW(MTAL_IOCTYPE_MTDMX, 83, INT32)
#define MTAL_IO_DMX_DREC_SETCONFIG             _IOW(MTAL_IOCTYPE_MTDMX, 84, INT32)

#define MTAL_IO_DMX_MWIF_CALL                  _IOW(MTAL_IOCTYPE_MTDMX, 85, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_SETFILEDESCPID             _IOW(MTAL_IOCTYPE_MTDMX, 86, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_SETPCRPIDEX                _IOW(MTAL_IOCTYPE_MTDMX, 87, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_DMX_DREC_GETTOTALDNBUFFERSIZE  _IOW(MTAL_IOCTYPE_MTDMX, 88, INT32)
#define MTAL_IO_DMX_GCPU_CMD                   _IOW(MTAL_IOCTYPE_MTDMX, 89, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DMX_DREC_SETDESCPID            _IOW(MTAL_IOCTYPE_MTDMX, 90, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_DREC_GETPID                _IOW(MTAL_IOCTYPE_MTDMX, 91, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_DMX_AES_ALLOCENGINE            _IOW(MTAL_IOCTYPE_MTDMX, 92, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_AES_FREEENGINE             _IOW(MTAL_IOCTYPE_MTDMX, 93, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_AES_SETDCRINFOEX           _IOW(MTAL_IOCTYPE_MTDMX, 94, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_AES_MVBLKDATAEX            _IOW(MTAL_IOCTYPE_MTDMX, 95, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_GETESFIFO                  _IOW(MTAL_IOCTYPE_MTDMX, 96, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_GETPACKETSTATUS            _IOW(MTAL_IOCTYPE_MTDMX, 97, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DMX_DREC_SETPID_DECODETYPE     _IOW(MTAL_IOCTYPE_MTDMX, 98, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETTSOUT                   _IOW(MTAL_IOCTYPE_MTDMX, 99, MTAL_IOCTL_2ARG_T)



#define MTAL_IO_DMX_DVB_KEY                    _IOW(MTAL_IOCTYPE_MTDMX, 100, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_DVB_IV                     _IOW(MTAL_IOCTYPE_MTDMX, 101, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DMX_SETPTSDRIFT                _IOW(MTAL_IOCTYPE_MTDMX, 102, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETFILEPID                 _IOW(MTAL_IOCTYPE_MTDMX, 103, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_CONNECT                    _IOW(MTAL_IOCTYPE_MTDMX, 104, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DISCONNECT                 _IOW(MTAL_IOCTYPE_MTDMX, 105, INT32)
#define MTAL_IO_DMX_DREC_GETSCRMSTE            _IOW(MTAL_IOCTYPE_MTDMX, 106, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_RAWTS_GETBFR               _IOW(MTAL_IOCTYPE_MTDMX, 107, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_DMX_RAWTS_ULKBFR               _IOW(MTAL_IOCTYPE_MTDMX, 108, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_DMX_DREC_SITSETPAT              _IOW(MTAL_IOCTYPE_MTDMX, 109, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DMX_DREC_SITFREEPAT             _IOW(MTAL_IOCTYPE_MTDMX, 110, INT32)
#define MTAL_IO_DMX_DREC_SITINSPAT              _IOW(MTAL_IOCTYPE_MTDMX, 111, INT32)
#define MTAL_IO_DMX_DREC_SITINSPATBYTIME        _IOW(MTAL_IOCTYPE_MTDMX, 112, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_SITSTOPTIMER           _IOW(MTAL_IOCTYPE_MTDMX, 113, INT32)

#define MTAL_IO_DMX_DREC_SWAPSETPAT             _IOW(MTAL_IOCTYPE_MTDMX, 114, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DMX_DREC_SWAPFREEPAT            _IOW(MTAL_IOCTYPE_MTDMX, 115, INT32)
#define MTAL_IO_DMX_DREC_SWAPREPPAT             _IOW(MTAL_IOCTYPE_MTDMX, 116, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_DREC_STARTINSDIT            _IOW(MTAL_IOCTYPE_MTDMX, 117, INT32)
#define MTAL_IO_DMX_DREC_STOPINSDIT             _IOW(MTAL_IOCTYPE_MTDMX, 118, INT32)

#define MTAL_IO_DMX_GETRAWSCRMSTE               _IOW(MTAL_IOCTYPE_MTDMX, 119, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_GETRAWSCRMSTE          _IOW(MTAL_IOCTYPE_MTDMX, 120, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_GETHWINFO                   _IOW(MTAL_IOCTYPE_MTDMX, 121, INT32)
#define MTAL_IO_DMX_SETVIDEOTYPEEX              _IOW(MTAL_IOCTYPE_MTDMX, 122, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETPKTSIZE                  _IOW(MTAL_IOCTYPE_MTDMX, 123, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_SETVIDEOTYPEBYPIDX          _IOW(MTAL_IOCTYPE_MTDMX, 124, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DMX_DREC_ALLOCBUFFERPTR         _IOW(MTAL_IOCTYPE_MTDMX, 125, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DREC_FREEBUFFERPTR          _IOW(MTAL_IOCTYPE_MTDMX, 126, INT32)
#define MTAL_IO_DMX_SET_EITSECTION              _IOW(MTAL_IOCTYPE_MTDMX, 127, INT32)
#define MTAL_IO_DMX_SETCRYPTOKEY                _IOW(MTAL_IOCTYPE_MTDMX, 128, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DMX_RELOADSECUREKEY             _IOW(MTAL_IOCTYPE_MTDMX, 129, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_DMX_GETDMXPIDINFO               _IOW(MTAL_IOCTYPE_MTDMX, 130, INT32)
#define MTAL_IO_DMX_CONNECT_AVAILABLEPID        _IOW(MTAL_IOCTYPE_MTDMX, 131, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_CONNECT_AVAILABLEFILTER     _IOW(MTAL_IOCTYPE_MTDMX, 132, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_CONNECT_FILTER              _IOW(MTAL_IOCTYPE_MTDMX, 133, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DMX_DISCONNECT_FILTER           _IOW(MTAL_IOCTYPE_MTDMX, 134, INT32)
#define MTAL_IO_DMX_GETSTCID                    _IOW(MTAL_IOCTYPE_MTDMX, 135, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY_EX  _IOW(MTAL_IOCTYPE_MTDMX, 191, MTAL_IOCTL_8ARG_T)

#define MTAL_IO_DRM_WV_SET_ASSET_KEY           _IOW(MTAL_IOCTYPE_MTDMX, 192, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_DERIVE_CW               _IOW(MTAL_IOCTYPE_MTDMX, 193, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_PASS_KEYBOX             _IOW(MTAL_IOCTYPE_MTDMX, 194, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_BIM_GET_DEVICE_DATA            _IOW(MTAL_IOCTYPE_MTDMX, 195, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_BIM_DM_GET_KEY                 _IOW(MTAL_IOCTYPE_MTDMX, 196, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_BIM_GET_ULPK                   _IOW(MTAL_IOCTYPE_MTDMX, 197, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_BIM_GET_ROMKEY                 _IOW(MTAL_IOCTYPE_MTDMX, 198, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_WV_DECRYPTION              _IOW(MTAL_IOCTYPE_MTDMX, 199, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY0    _IOW(MTAL_IOCTYPE_MTDMX, 200, INT32)
#define MTAL_IO_DRM_DIVX_GET_HW_SECRET_KEY1    _IOW(MTAL_IOCTYPE_MTDMX, 201, INT32)

/* hulu+dash+wv drm only */
#define MTAL_IO_DRM_WV_CREATESESSION		   		_IOW(MTAL_IOCTYPE_MTDMX, 210, INT32)
#define MTAL_IO_DRM_WV_DESTROYSESSION		   		_IOW(MTAL_IOCTYPE_MTDMX, 211, INT32)
#define MTAL_IO_DRM_WV_VALIDATE_KEYBOX 		   	_IOW(MTAL_IOCTYPE_MTDMX, 212, INT32)
#define MTAL_IO_DRM_WV_GENERATE_NONCE		   		_IOW(MTAL_IOCTYPE_MTDMX, 213, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_GENERATE_DERIVEKEY	   	_IOW(MTAL_IOCTYPE_MTDMX, 214, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_DRM_WV_GENERATE_SIGNATURE	   	_IOW(MTAL_IOCTYPE_MTDMX, 215, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DRM_WV_VALIDATEM_MESSAGE	   	_IOW(MTAL_IOCTYPE_MTDMX, 216, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DRM_WV_INSTALL_KEY		       	_IOW(MTAL_IOCTYPE_MTDMX, 217, MTAL_IOCTL_11ARG_T)
#define MTAL_IO_DRM_WV_FLUSH_NONCE		       	_IOW(MTAL_IOCTYPE_MTDMX, 218, INT32)
#define MTAL_IO_DRM_WV_UPDATE_MACKEY		   		_IOW(MTAL_IOCTYPE_MTDMX, 219, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DRM_WV_REFRESH_KEY		       	_IOW(MTAL_IOCTYPE_MTDMX, 220, MTAL_IOCTL_7ARG_T)

#define MTAL_IO_DRM_WV_SELECT_CONTENT_KEY	   	_IOW(MTAL_IOCTYPE_MTDMX, 221, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_WV_DECRYPT_CTR		       	_IOW(MTAL_IOCTYPE_MTDMX, 222, MTAL_IOCTL_9ARG_T)
#define MTAL_IO_DRM_WV_INSTALL_KEYBOX		   		_IOW(MTAL_IOCTYPE_MTDMX, 223, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DRM_WV_GET_DEVICE_ID		   		_IOW(MTAL_IOCTYPE_MTDMX, 224, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_GET_KEY_DATA		       	_IOW(MTAL_IOCTYPE_MTDMX, 225, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_SAVE_KEYBOX			   		_IOW(MTAL_IOCTYPE_MTDMX, 226, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_WV_GET_RANDOM_NUM		   		_IOW(MTAL_IOCTYPE_MTDMX, 227, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_DRM_WV_GENERIC_ENC		       	_IOW(MTAL_IOCTYPE_MTDMX, 228, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_DRM_WV_GENERIC_DEC		       	_IOW(MTAL_IOCTYPE_MTDMX, 229, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_DRM_WV_GENERIC_SIGN		   	   	_IOW(MTAL_IOCTYPE_MTDMX, 230, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_DRM_WV_GENERIC_VERIFY		   		_IOW(MTAL_IOCTYPE_MTDMX, 231, MTAL_IOCTL_7ARG_T)

#define MTAL_IO_DRM_WV_REWRAP_RSAKEY		   		_IOW(MTAL_IOCTYPE_MTDMX, 232, MTAL_IOCTL_12ARG_T)
#define MTAL_IO_DRM_WV_LOAD_RSAKET		       	_IOW(MTAL_IOCTYPE_MTDMX, 233, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRM_WV_GENERATE_RSASIGN		   	_IOW(MTAL_IOCTYPE_MTDMX, 234, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_DRM_WV_RSA_DERIVEKEY		   		_IOW(MTAL_IOCTYPE_MTDMX, 235, MTAL_IOCTL_7ARG_T)


/* MTAL_IOCTYPE_MTGFX   8 */
#define MTAL_IO_GFX_RESET                       _IOW(MTAL_IOCTYPE_MTGFX, 0, INT32)
#define MTAL_IO_GFX_WAIT                        _IOW(MTAL_IOCTYPE_MTGFX, 1, INT32)
#define MTAL_IO_GFX_FLUSH                       _IOW(MTAL_IOCTYPE_MTGFX, 2, INT32)
#define MTAL_IO_GFX_FILLRECT                    _IOW(MTAL_IOCTYPE_MTGFX, 3, INT32)
#define MTAL_IO_GFX_DRAWHLINE                   _IOW(MTAL_IOCTYPE_MTGFX, 4, INT32)
#define MTAL_IO_GFX_DRAWVLINE                   _IOW(MTAL_IOCTYPE_MTGFX, 5, INT32)
#define MTAL_IO_GFX_BITBLT                      _IOW(MTAL_IOCTYPE_MTGFX, 6, INT32)
#define MTAL_IO_GFX_TRANSPARENTBITBLT           _IOW(MTAL_IOCTYPE_MTGFX, 7, INT32)
#define MTAL_IO_GFX_TRANSPARENTFILL             _IOW(MTAL_IOCTYPE_MTGFX, 8, INT32)
#define MTAL_IO_GFX_ALPHABLENDING               _IOW(MTAL_IOCTYPE_MTGFX, 9, INT32)
#define MTAL_IO_GFX_YBR2RGB                     _IOW(MTAL_IOCTYPE_MTGFX, 10, INT32)
#define MTAL_IO_GFX_ALPHAMAPBITBLT              _IOW(MTAL_IOCTYPE_MTGFX, 11, INT32)
#define MTAL_IO_GFX_ROPBITBLT                   _IOW(MTAL_IOCTYPE_MTGFX, 12, INT32)
#define MTAL_IO_GFX_ALPHACOMPOSITION            _IOW(MTAL_IOCTYPE_MTGFX, 13, INT32)
#define MTAL_IO_GFX_IDX2DIRBITBLT               _IOW(MTAL_IOCTYPE_MTGFX, 14, INT32)
#define MTAL_IO_GFX_ROTATE90                    _IOW(MTAL_IOCTYPE_MTGFX, 15, INT32)
#define MTAL_IO_GFX_GETBUFFERPROPERTIES         _IOW(MTAL_IOCTYPE_MTGFX, 16, INT32)
#define MTAL_IO_GFX_SCALER                      _IOW(MTAL_IOCTYPE_MTGFX, 17, INT32)
#define MTAL_IO_GFX_RGB2YBR                     _IOW(MTAL_IOCTYPE_MTGFX, 18, INT32)
#define MTAL_IO_GFX_STRETCHBITBLT       _IOW(MTAL_IOCTYPE_MTGFX, 19, INT32)
#define MTAL_IO_GFX_GETBUFFERPATITION           _IOW(MTAL_IOCTYPE_MTGFX, 20, INT32)
#define MTAL_IO_GFX_STRETCHALPHACOMPOSITION           _IOW(MTAL_IOCTYPE_MTGFX, 21, INT32)

#define MTAL_IO_GFX_ALPHACOMPOSITION_IMPROVE              _IOW(MTAL_IOCTYPE_MTGFX, 22, INT32)

#define MTAL_IO_VGFX_SCALER                    _IOW(MTAL_IOCTYPE_MTGFX, 23, INT32)
#define MTAL_IO_GFX_SET_MMU                   _IOW(MTAL_IOCTYPE_MTGFX, 24, INT32)
#define MTAL_IO_GFX_LOCK                          _IOW(MTAL_IOCTYPE_MTGFX, 25, INT32)
#define MTAL_IO_GFX_UNLOCK                    _IOW(MTAL_IOCTYPE_MTGFX, 26, INT32)
#define MTAL_IO_GFX_SET_FLIPMIRROR        _IOW(MTAL_IOCTYPE_MTGFX, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_GFX_COMPRESSBLIT                      _IOW(MTAL_IOCTYPE_MTGFX, 28, INT32)
#define MTAL_IO_GFX_SCALERCOMPOSITION       _IOW(MTAL_IOCTYPE_MTGFX, 29, INT32)
#define MTAL_IO_GFX_GET_FBM                 _IOW(MTAL_IOCTYPE_MTGFX, 30, INT32)
#define MTAL_IO_GFX_COLORCONVERSION     _IOW(MTAL_IOCTYPE_MTGFX, 31, INT32)
#define MTAL_IO_GFX_SET_SBS                   _IOW(MTAL_IOCTYPE_MTGFX, 33, INT32)
#define MTAL_IO_GFX_FLUSH_DCAHE         _IOW(MTAL_IOCTYPE_MTGFX, 34, INT32)
#define MTAL_IO_GFX_VDORESIZE           _IOW(MTAL_IOCTYPE_MTGFX, 35, INT32)


/*MTAL_IOCTYPE_MTMPLAYER  9 */
#define MTAL_IO_MPLAYER_OPEN                          _IOW(MTAL_IOCTYPE_MTMPLAYER, 0, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_CLOSE                        _IOW(MTAL_IOCTYPE_MTMPLAYER, 1, INT32)
#define MTAL_IO_MPLAYER_RESET                         _IOW(MTAL_IOCTYPE_MTMPLAYER, 2, INT32)
#define MTAL_IO_MPLAYER_SET                            _IOW(MTAL_IOCTYPE_MTMPLAYER, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_START                         _IOW(MTAL_IOCTYPE_MTMPLAYER, 4, INT32)
#define MTAL_IO_MPLAYER_STOP                          _IOW(MTAL_IOCTYPE_MTMPLAYER, 5, INT32)
#define MTAL_IO_MPLAYER_PAUSE                        _IOW(MTAL_IOCTYPE_MTMPLAYER, 6, INT32)
#define MTAL_IO_MPLAYER_RESUME                      _IOW(MTAL_IOCTYPE_MTMPLAYER, 7, INT32)
#define MTAL_IO_MPLAYER_GETINFO                     _IOW(MTAL_IOCTYPE_MTMPLAYER, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_REGCB                        _IOW(MTAL_IOCTYPE_MTMPLAYER, 9, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MPLAYER_SEEK                          _IOW(MTAL_IOCTYPE_MTMPLAYER, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_DISPLAYIMG               _IOW(MTAL_IOCTYPE_MTMPLAYER, 11, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MPLAYER_GETBUFINFO               _IOW(MTAL_IOCTYPE_MTMPLAYER, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_DECIMG                     _IOW(MTAL_IOCTYPE_MTMPLAYER, 13, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_SUBMIT                     _IOW(MTAL_IOCTYPE_MTMPLAYER, 14, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MPLAYER_GETIMG                     _IOW(MTAL_IOCTYPE_MTMPLAYER, 15, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_GETMETA                    _IOW(MTAL_IOCTYPE_MTMPLAYER, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_STEP                       _IOW(MTAL_IOCTYPE_MTMPLAYER, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_SETFIFO                    _IOW(MTAL_IOCTYPE_MTMPLAYER, 18, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_SETSPDISP                    _IOW(MTAL_IOCTYPE_MTMPLAYER, 19, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_MPLAYER_ABORT                        _IOW(MTAL_IOCTYPE_MTMPLAYER, 20, INT32)
#define MTAL_IO_MPLAYER_SLIDESHOW                  _IOW(MTAL_IOCTYPE_MTMPLAYER, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_CHKSUBTITLE                _IOW(MTAL_IOCTYPE_MTMPLAYER, 22, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_CONNECTIMG                  _IOW(MTAL_IOCTYPE_MTMPLAYER, 23, INT32)
#define MTAL_IO_MPLAYER_CLEANFB                  _IOW(MTAL_IOCTYPE_MTMPLAYER, 24, INT32)
#define MTAL_IO_MPLAYER_ASSIGNEXTSUBTITLE          _IOW(MTAL_IOCTYPE_MTMPLAYER, 25, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_GETDRMREGCODE              _IOW(MTAL_IOCTYPE_MTMPLAYER, 26, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_DRMDEACTIVATION            _IOW(MTAL_IOCTYPE_MTMPLAYER, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_GETDRMUIHELPINFO           _IOW(MTAL_IOCTYPE_MTMPLAYER, 28, INT32)
#define MTAL_IO_MPLAYER_SETSRCTYPE                 _IOW(MTAL_IOCTYPE_MTMPLAYER, 29, INT32)
#define MTAL_IO_MPLAYER_SETNUMBSTM                 _IOW(MTAL_IOCTYPE_MTMPLAYER, 30, INT32)
#define MTAL_IO_MPLAYER_NETFLIXEOS                 _IOW(MTAL_IOCTYPE_MTMPLAYER, 31, INT32)
#define MTAL_IO_MPLAYER_GETDECODEDSIZE             _IOW(MTAL_IOCTYPE_MTMPLAYER, 32, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MPLAYER_SETEOS                           _IOW(MTAL_IOCTYPE_MTMPLAYER, 33, INT32)
#define MTAL_IO_MPLAYER_SEEK_POS                    _IOW(MTAL_IOCTYPE_MTMPLAYER, 34, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_GETCAP                      _IOW(MTAL_IOCTYPE_MTMPLAYER, 35, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MPLAYER_CONFIGIMGDECODER            _IOW(MTAL_IOCTYPE_MTMPLAYER, 36, INT32)
#ifdef SWDMX_DBG_USB
#define MTAL_IO_MPLAYER_SWDMX                     _IOW(MTAL_IOCTYPE_MTMPLAYER, 37, MTAL_IOCTL_5ARG_T)
#endif

#define MTAL_IO_IMAGE_CONNECT                      _IOW(MTAL_IOCTYPE_MTIMAGE, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_OPEN                      _IOW(MTAL_IOCTYPE_MTIMAGE, 1, INT32)
#define MTAL_IO_IMAGE_DECODE                      _IOW(MTAL_IOCTYPE_MTIMAGE, 2, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_IMAGE_DISPLAY                      _IOW(MTAL_IOCTYPE_MTIMAGE, 3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_CLOSE                      _IOW(MTAL_IOCTYPE_MTIMAGE, 4, INT32)
#define MTAL_IO_IMAGE_STOP                      _IOW(MTAL_IOCTYPE_MTIMAGE, 5, INT32)
#define MTAL_IO_IMAGE_SET                      _IOW(MTAL_IOCTYPE_MTIMAGE, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_GETINFO                      _IOW(MTAL_IOCTYPE_MTIMAGE, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_GETIMG                      _IOW(MTAL_IOCTYPE_MTIMAGE, 8, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_IMAGE_GETMETA                      _IOW(MTAL_IOCTYPE_MTIMAGE, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_SLIDESHOW                      _IOW(MTAL_IOCTYPE_MTIMAGE, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_CLEANFB                      _IOW(MTAL_IOCTYPE_MTIMAGE, 11, INT32)
#define MTAL_IO_IMAGE_GETCAP                      _IOW(MTAL_IOCTYPE_MTIMAGE, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_IMAGE_FILLBUF                      _IOW(MTAL_IOCTYPE_MTIMAGE, 13, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_CONFIGIMGDECODER  _IOW(MTAL_IOCTYPE_MTIMAGE, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_GETPNGHDR                      _IOW(MTAL_IOCTYPE_MTIMAGE, 15, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_IMAGE_ENLARGEMENT  			_IOW(MTAL_IOCTYPE_MTIMAGE, 16, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_SET_DECODE              _IOW(MTAL_IOCTYPE_MTIMAGE, 17, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_SET_INIT              _IOW(MTAL_IOCTYPE_MTIMAGE, 18, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_IMAGE_MW_CONNECT              _IOW(MTAL_IOCTYPE_MTIMAGE, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_MW_GETINFO                      _IOW(MTAL_IOCTYPE_MTIMAGE, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_SET_DISP_PATH  	    _IOW(MTAL_IOCTYPE_MTIMAGE, 21, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_RESOUCE_CTL  			_IOW(MTAL_IOCTYPE_MTIMAGE, 22, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_QRY_PNG_BUFFER_SIZE   _IOW(MTAL_IOCTYPE_MTIMAGE, 23, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_IMAGE_GET_IMGRZ_NUM         _IOW(MTAL_IOCTYPE_MTIMAGE, 24, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_IMAGE_GET_DEFDECADDR _IOW(MTAL_IOCTYPE_MTIMAGE, 25, MTAL_IOCTL_2ARG_T)//demo dddd

/* MTAL_IOCTYPE_MTOSD	10 */
#define MTAL_IO_OSD_RESET						_IOW(MTAL_IOCTYPE_MTOSD, 0, INT32)
#define MTAL_IO_OSD_BASE_SETOSDPOSITION			_IOW(MTAL_IOCTYPE_MTOSD, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_BASE_GETOSDPOSITION			_IOW(MTAL_IOCTYPE_MTOSD, 2, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_PLA_ENABLE					_IOW(MTAL_IOCTYPE_MTOSD, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_FLIPTO					_IOW(MTAL_IOCTYPE_MTOSD, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_GETBLENDLEVEL			_IOW(MTAL_IOCTYPE_MTOSD, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_SETBLENDLEVEL			_IOW(MTAL_IOCTYPE_MTOSD, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_GETFADING				_IOW(MTAL_IOCTYPE_MTOSD, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_SETFADING				_IOW(MTAL_IOCTYPE_MTOSD, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_DUMP					_IOW(MTAL_IOCTYPE_MTOSD, 9, INT32)
#define MTAL_IO_OSD_PLA_GETFIRSTREGION			_IOW(MTAL_IOCTYPE_MTOSD, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_GETFIRSTREGIONADDRESS	_IOW(MTAL_IOCTYPE_MTOSD, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_RGN_LIST_CREATE				_IOW(MTAL_IOCTYPE_MTOSD, 12, INT32)
#define MTAL_IO_OSD_RGN_LIST_DELETE				_IOW(MTAL_IOCTYPE_MTOSD, 13, INT32)
#define MTAL_IO_OSD_RGN_LIST_GETHEAD			_IOW(MTAL_IOCTYPE_MTOSD, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_RGN_GET						_IOW(MTAL_IOCTYPE_MTOSD, 15, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_RGN_SET						_IOW(MTAL_IOCTYPE_MTOSD, 16, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_RGN_CREATE					_IOW(MTAL_IOCTYPE_MTOSD, 17, MTAL_IOCTL_12ARG_T)
#define MTAL_IO_OSD_RGN_DELETE					_IOW(MTAL_IOCTYPE_MTOSD, 18, INT32)
#define MTAL_IO_OSD_RGN_INSERT					_IOW(MTAL_IOCTYPE_MTOSD, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_RGN_DETACH					_IOW(MTAL_IOCTYPE_MTOSD, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_SC_SCALE					_IOW(MTAL_IOCTYPE_MTOSD, 21, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_OSD_SC_GETSCALERINFO			_IOW(MTAL_IOCTYPE_MTOSD, 22, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_OSD_SC_SETLPF					_IOW(MTAL_IOCTYPE_MTOSD, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GET_FB_RGN_LIST					_IOW(MTAL_IOCTYPE_MTOSD, 24, INT32)
#define MTAL_IO_OSD_GET_FB_RESOLUTION					_IOW(MTAL_IOCTYPE_MTOSD, 25, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_INIT					_IOW(MTAL_IOCTYPE_MTOSD, 26, INT32)
#define MTAL_IO_OSD_SET_3D_ADDRESS					_IOW(MTAL_IOCTYPE_MTOSD, 27, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_PLA_SET_3D_MODE				_IOW(MTAL_IOCTYPE_MTOSD, 28, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_SET_SCROLLINFO				_IOW(MTAL_IOCTYPE_MTOSD, 29, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_OSD_SET_SCROLL_MODE				_IOW(MTAL_IOCTYPE_MTOSD, 30, INT32)
#define MTAL_IO_OSD_PLA_SET_3DREGION				_IOW(MTAL_IOCTYPE_MTOSD, 31, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_ENABLE_AUTOLR				_IOW(MTAL_IOCTYPE_MTOSD, 32, UINT32)
#define MTAL_IO_OSD_GET_LRSTATUS				_IOW(MTAL_IOCTYPE_MTOSD, 33, UINT32)
#define MTAL_IO_OSD_SET_TOPPLANE				_IOW(MTAL_IOCTYPE_MTOSD, 34, UINT32)
#define MTAL_IO_OSD_SET_PLA_LINESHIFT				_IOW(MTAL_IOCTYPE_MTOSD, 35, UINT32)
#define MTAL_IO_OSD_SET_FRAMEBUFFER_PROPERTY				_IOW(MTAL_IOCTYPE_MTOSD, 36, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_OSD_PLA_GETFIRSTREGIONLIST				_IOW(MTAL_IOCTYPE_MTOSD, 37, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_OSD_SET_FLIPMIRROR				_IOW(MTAL_IOCTYPE_MTOSD, 38, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GET_PLA_REGION				_IOW(MTAL_IOCTYPE_MTOSD, 39, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GET_PLA_SRC_CONTENT_REGION				_IOW(MTAL_IOCTYPE_MTOSD, 40, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GET_PLA_SRC_CONTENT_SIZE				_IOW(MTAL_IOCTYPE_MTOSD, 41, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_PLA_GETEnabel				_IOW(MTAL_IOCTYPE_MTOSD, 42, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_BASE_WAIT_VSYNC             _IOW(MTAL_IOCTYPE_MTOSD, 43, INT32)
#define MTAL_IO_OSD_PLA_GLOBAL_ENABLE		    _IOW(MTAL_IOCTYPE_MTOSD, 44, MTAL_IOCTL_2ARG_T)
#ifdef GAL_DDI_SUPPORT
#define MTAL_IO_OSD_GPU_DLMEMINIT		    	_IOW(MTAL_IOCTYPE_MTOSD, 45, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_DLMALLOC		    	_IOW(MTAL_IOCTYPE_MTOSD, 46, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_DLCALLOC		    	_IOW(MTAL_IOCTYPE_MTOSD, 47, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GPU_DLRESIZEALLOC	    	_IOW(MTAL_IOCTYPE_MTOSD, 48, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_OSD_GPU_DLREALLOC		    	_IOW(MTAL_IOCTYPE_MTOSD, 49, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GPU_DLFREE			  		_IOW(MTAL_IOCTYPE_MTOSD, 50, UINT32)
#define MTAL_IO_OSD_GPU_DLMEM_GETSTATUS			_IOW(MTAL_IOCTYPE_MTOSD, 51, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_ADD_SURFACEINDEX		_IOW(MTAL_IOCTYPE_MTOSD, 52, INT32)
#define MTAL_IO_OSD_GPU_GET_SURFACEINDEX		_IOW(MTAL_IOCTYPE_MTOSD, 53, INT32)
#define MTAL_IO_OSD_GPU_SET_SURFACEPOOL			_IOW(MTAL_IOCTYPE_MTOSD, 54, INT32)
#define MTAL_IO_OSD_GPU_CREATESURFACE			_IOW(MTAL_IOCTYPE_MTOSD, 55, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_OSD_GPU_DESTROYSURFACE			_IOW(MTAL_IOCTYPE_MTOSD, 56, UINT32)
#define MTAL_IO_OSD_GPU_INITSURFACESEMA			_IOW(MTAL_IOCTYPE_MTOSD, 57, INT32)
#define MTAL_IO_OSD_GPU_GETOFFSETSURFACE		_IOW(MTAL_IOCTYPE_MTOSD, 58, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_SYNCSURFACE				_IOW(MTAL_IOCTYPE_MTOSD, 59, INT32)
#define MTAL_IO_OSD_GPU_GALINIT					_IOW(MTAL_IOCTYPE_MTOSD, 60, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GPU_GETTEMPSURFACE			_IOW(MTAL_IOCTYPE_MTOSD, 61, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_MULTIPROCLOCK			_IOW(MTAL_IOCTYPE_MTOSD, 62, INT32)
#define MTAL_IO_OSD_GPU_MULTIPROCUNLOCK			_IOW(MTAL_IOCTYPE_MTOSD, 63, INT32)
#define MTAL_IO_OSD_GPU_MALLOC_TO_OFFSET		_IOW(MTAL_IOCTYPE_MTOSD, 64, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_GPU_SET_FRAMESURFACE		_IOW(MTAL_IOCTYPE_MTOSD, 65, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_PLA_ENABLES					_IOW(MTAL_IOCTYPE_MTOSD, 66, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GPU_SET3DMODE				_IOW(MTAL_IOCTYPE_MTOSD, 67, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_OSD_GPU_GET_FRAMESURFACE_ATTRIBUTES _IOW(MTAL_IOCTYPE_MTOSD, 68, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_OSD_GPU_SET_FRAMESURFACE_ATTRIBUTES _IOW(MTAL_IOCTYPE_MTOSD, 69, MTAL_IOCTL_5ARG_T)
#endif
#define MTAL_IO_OSD_GET_PLANE_PROPERTY		    _IOW(MTAL_IOCTYPE_MTOSD, 70, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_CAPTURE_PLANE		        _IOW(MTAL_IOCTYPE_MTOSD, 71, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_OSD_DISABLE_LOADER_LOGO		    _IOW(MTAL_IOCTYPE_MTOSD, 72, INT32)



/*MTAL_IOCTYPE_MTPERIPHERAL 11*/
#define MTAL_IO_MTPERIPHERAL_GPIO_SETDIRECTION                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_GETDIRECTION                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 1, INT32)
#define MTAL_IO_MTPERIPHERAL_GPIO_INTRQ                                      _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_QUERY                                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_REG                                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_SETOUT                                   _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_GETIN                                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 6, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SETFREQUENCY                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 7, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_READSERVOVALUE               _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 8, INT32)
#define MTAL_IO_MTPERIPHERAL_SIF_INIT                                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 9, INT32)
#define MTAL_IO_MTPERIPHERAL_SIF_STOP                                        _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 10,INT32)
#define MTAL_IO_MTPERIPHERAL_SIF_WRITEMULTIPLE                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 11, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_MTPERIPHERAL_SIF_READMULTIPLE                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 12, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_MTPERIPHERAL_RTC_RTCTOUTC                                _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 13, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_RTC_UTCTORTC                                _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 14, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_RTC_INIT                                        _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 15, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_GETTIMEDATA                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 16, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_SETTIMEDATA                           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 17, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_SETALERTTIME                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 18, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_GETALERTTIME                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 19, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_HOOKALERTHANDLER                _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 20, INT32)
#define MTAL_IO_MTPERIPHERAL_RTC_SETWAKEUP                             _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 21, INT32)
#define MTAL_IO_MTPERIPHERAL_NOR_ERASE                                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_NOR_ERASEADDR                             _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_NOR_GETSIZE                                 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 24, INT32)
#define MTAL_IO_MTPERIPHERAL_NOR_READ                                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 25, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_NOR_WRITE                                   _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 26, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_NOR_WRONE                                  _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 27, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_NOR_CALCRC                                 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 28, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_NAND_INIT                                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 29, INT32)
#define MTAL_IO_MTPERIPHERAL_NAND_RESET                                  _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 30, INT32)
#define MTAL_IO_MTPERIPHERAL_NAND_READID                               _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 31, INT32)
#define MTAL_IO_MTPERIPHERAL_NAND_GETSIZE                              _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 32, INT32)
#define MTAL_IO_MTPERIPHERAL_NAND_READBLOCKS                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 33, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_NAND_WRITEBLOCKS                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 34, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_IR_INIT                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 35, INT32)
#define MTAL_IO_MTPERIPHERAL_IR_POLL                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 36, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_PINMUX_SET                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 37, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_POWER_ENTERSTANDBY                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 38, INT32)
#define MTAL_IO_MTPERIPHERAL_POWER_SETWAKEUPSOURCE                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 39, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_PERIODLEVELCALLBACK                   _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 40, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_INIT                                  _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 41, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SETPHASE                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 42, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_MTPERIPHERAL_DRAM_GETBANDWIDTHUSAGE             _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 43, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_T8032_CMD             _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 44, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_POWER_WAKEUP_REASON                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 45, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_IR_WAIT                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 46, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_IR_SET_RC56_SYSTEM_CODE     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 47, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_ENTER_POWER_STATE     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 48, UINT32)
#define MTAL_IO_MTPERIPHERAL_GPIO_QUERY_OUTVAL           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 49, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_MTPERIPHERAL_SMC_INIT                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 50, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_GETCARDPRESENCE                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 51, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_RESET                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 52, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_SENDRECEIVEAPDU                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 53, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_SMC_REQUESTIFS                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 54, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_ACTIVATE                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 55, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_DEACTIVATE                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 56, INT32)
#define MTAL_IO_MTPERIPHERAL_SMC_SET_PROTOCOL                        _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 57, INT32)

#define MTAL_IO_MTPERIPHERAL_PINMUX_SET_EX                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 58, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_MTPERIPHERAL_EEPROM_INIT                                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 60, INT32)
#define MTAL_IO_MTPERIPHERAL_EEPROM_READ                                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 61, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_EEPROM_WRITE                                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 62, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_EEPROM_GETPROPERTY                                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 63, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_SCRBT_CHECK                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 64, INT32)
#define MTAL_IO_MTPERIPHERAL_SET_BW_PARAMETERS                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 65, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_MTPERIPHERAL_SIFSW_INIT                              _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 66, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_SIFSW_WRITEDATA                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 67, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_MTPERIPHERAL_SIFSW_READDATA                          _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 68, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_MTPERIPHERAL_SIFSW_REMOVE                            _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 69, INT32)
#define MTAL_IO_MTPERIPHERAL_SIFSW_WRITEDATA_NO_SUB_ADDR			 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 70, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_MTPERIPHERAL_SIFSW_READDATA_NO_SUB_ADDR              _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 71, MTAL_IOCTL_6ARG_T)

#define MTAL_IO_MTPERIPHERAL_DDI_UART_AccessReg						_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 72, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SetVsyncAlign						_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 73, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_OUTPUT         			 	 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 74, INT32)

#define MTAL_IO_MTPERIPHERAL_BIM_GETDEVID                            _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 75, INT32)
#define MTAL_IO_MTPERIPHERAL_CHECK_CUSTOM_WAKEUP_REASON              _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 76, INT32)
#define MTAL_IO_MTPERIPHERAL_POWER_ENTERPOWERDOWN          			 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 77, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_GPIO_INTRQ_QUERY                        _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 78, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_MTPERIPHERAL_SET_POWER_DOWN_REASON                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 79, INT32)

#define MTAL_IO_MTPERIPHERAL_PDWNC_REBOOT                            _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 80, INT32)
#define MTAL_IO_MTPERIPHERAL_EEPDTV_GETCFG                           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 81, INT32)
#define MTAL_IO_MTPERIPHERAL_EEPDTV_SETCFG                           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 82, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_BY_AC_CUT                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 83, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_BY_POWER_BUTTON                   _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 84, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_CREATE_WTTHREAD               _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 85, INT32)
#define MTAL_IO_MTPERIPHERAL_EEPHDCPINIT                         _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 86, INT32)
#define MTAL_IO_MTPERIPHERAL_EEPNPTV_READ                        _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 87, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_SET_WDTIMEOUT                           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 88, INT32)
#define MTAL_IO_MTPERIPHERAL_GET_WDTIMEOUT                           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 89, INT32)
#define MTAL_IO_MTPERIPHERAL_IR_RAW_BIT_TO_EVT_CODE                  _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 90, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_SYS_SetDramBWEnable					_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 91, INT32)
#define MTAL_IO_MTPERIPHERAL_SYS_GetDramBWInfo						_IOW(MTAL_IOCTYPE_MTPERIPHERAL,	 92, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_EEHDCP_Offset						    _IOW(MTAL_IOCTYPE_MTPERIPHERAL,	 93, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SETSCAN_ENABLE                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 94, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SETSCAN_DUTY                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 95, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_SET_NUMBERS_OF_KEY          			_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 96, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_SET_THE_LAST_OF_FIVE_KEY    			_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 97, MTAL_IOCTL_6ARG_T)

#define MTAL_IO_MTPERIPHERAL_REQUESTSUSPEND          			 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 98, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GETPOWERSTATE                 _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 99, INT32)
#define MTAL_IO_MTPERIPHERAL_SET_RTC_WAKEUP_ENABLE               _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 100, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GETWAKEUPIRKEY                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 101, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GETHOTKEYNUM                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 102, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GETALLHOTKEY                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 103, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_MSDC_READ                     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 104, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_MSDC_WRITE                    _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 105, MTAL_IOCTL_4ARG_T)
#ifdef CC_PARTITION_WP_SUPPORT
#define MTAL_IO_MTPERIPHERAL_MSDC_WPconfig                    _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 106, INT32)
#endif
#define MTAL_IO_MTPERIPHERAL_PDWNC_REBOOT1                            _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 107, INT32)
#define MTAL_IO_MTPERIPHERAL_SET_SCANPWM_AUTO                       _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 108, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_MTPERIPHERAL_HDCP2X_KEY_READ                   _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 109, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GSPI_INT                   	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 110, INT32)
#define MTAL_IO_MTPERIPHERAL_PDWNC_GSPI_RW                   	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 111, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_SENDMTKIRKEY                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 112, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_RELEASEQUEUE                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 113, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_LZHS_DEC                 	     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 114, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_LETGO_STANDBY                 	_IOW(MTAL_IOCTYPE_MTPERIPHERAL, 115, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_MTPERIPHERAL_QUIET_HOT_BOOT_CLEAR            _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 115, INT32) 
#define MTAL_IO_MTPERIPHERAL_IS_QUIET_HOT_BOOT_POWER_ON      _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 116, INT32) 
#define MTAL_IO_MTPERIPHERAL_PDWNC_SET_PWM                	     _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 117, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MTPERIPHERAL_PDWNC_SET_PWM_BREATHLIGHT           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 118, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MTPERIPHERAL_IRRX_PROGRAM_RC56_WAKEUPKETS           _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 119, MTAL_IOCTL_4ARG_T)

#define MTAL_IO_MTPERIPHERAL_SYS_JTAG_PORT_ENABLE					_IOW(MTAL_IOCTYPE_MTPERIPHERAL,	 120, INT32)
#define MTAL_IO_MTPERIPHERAL_SYS_MEM_SSC					_IOW(MTAL_IOCTYPE_MTPERIPHERAL,	 121, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MTPERIPHERAL_PWM_SETINIT                      _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 122, INT32)
#define MTAL_IO_MTPERIPHERAL_PWM_SETPARAMETER                      _IOW(MTAL_IOCTYPE_MTPERIPHERAL, 123, MTAL_IOCTL_2ARG_T)



/* MTAL_IOCTYPE_MTPMX	12 */
#define MTAL_IO_PMX_INIT						_IOW(MTAL_IOCTYPE_MTPMX, 0, INT32)
#define MTAL_IO_PMX_GETPLANEORDER				_IOW(MTAL_IOCTYPE_MTPMX, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_SETPLANEORDER				_IOW(MTAL_IOCTYPE_MTPMX, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_SETBG						_IOW(MTAL_IOCTYPE_MTPMX, 3, INT32)
#define MTAL_IO_PMX_SETALPHA					_IOW(MTAL_IOCTYPE_MTPMX, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_SETALLMUTE					_IOW(MTAL_IOCTYPE_MTPMX, 5, INT32)
#define MTAL_IO_PMX_PANEL_SETFRAMERATE			_IOW(MTAL_IOCTYPE_MTPMX, 6, INT32)
#define MTAL_IO_PMX_GETFRAMERATE				_IOW(MTAL_IOCTYPE_MTPMX, 7, INT32)
#define MTAL_IO_PMX_ENABLELVDSPOWER				_IOW(MTAL_IOCTYPE_MTPMX, 8, INT32)
#define MTAL_IO_PMX_ENABLEBACKLIGHTPOWER		_IOW(MTAL_IOCTYPE_MTPMX, 9, INT32)
#define MTAL_IO_PMX_ENABLELVDSSIGNAL			_IOW(MTAL_IOCTYPE_MTPMX, 10, INT32)
#define MTAL_IO_PMX_SETDRIVINGCURRENT			_IOW(MTAL_IOCTYPE_MTPMX, 11, INT32)
#define MTAL_IO_PMX_TVE_ENABLE					_IOW(MTAL_IOCTYPE_MTPMX, 12, INT32)
#define MTAL_IO_PMX_SETBACKLTDIMMING			_IOW(MTAL_IOCTYPE_MTPMX, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_POWERSEQUENCE				_IOW(MTAL_IOCTYPE_MTPMX, 14, INT32)
#define MTAL_IO_PMX_PANEL_SELECT				_IOW(MTAL_IOCTYPE_MTPMX, 15, INT32)
#define MTAL_IO_PMX_PANEL_SETFILMMODE			_IOW(MTAL_IOCTYPE_MTPMX, 16, INT32)
#define MTAL_IO_PMX_PANEL_GETFILMMODE			_IOW(MTAL_IOCTYPE_MTPMX, 17, INT32)
#define MTAL_IO_PMX_SETSPREADSPECTRUM			_IOW(MTAL_IOCTYPE_MTPMX, 18, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_PANEL_GETRESOLUTION         _IOW(MTAL_IOCTYPE_MTPMX, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_PANEL_SYNCLOCK				_IOW(MTAL_IOCTYPE_MTPMX, 20, INT32)
#define MTAL_IO_PMX_PANEL_GETLOCKSTATUS			_IOW(MTAL_IOCTYPE_MTPMX, 21, INT32)
#define MTAL_IO_PMX_3D_MODE						_IOW(MTAL_IOCTYPE_MTPMX, 22, INT32)
#define MTAL_IO_PMX_3D_GET_LR_INFO				_IOW(MTAL_IOCTYPE_MTPMX, 23, INT32)
#define MTAL_IO_PMX_3D_SET_LR_POLARITY			_IOW(MTAL_IOCTYPE_MTPMX, 24, INT32)
#define MTAL_IO_PMX_SETPLANEORDERARRAY			_IOW(MTAL_IOCTYPE_MTPMX, 25, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_PMX_PANEL_SETFRAMETRACK			_IOW(MTAL_IOCTYPE_MTPMX, 26, INT32)
#define MTAL_IO_PMX_PANEL_SETCOLORDEPTH			_IOW(MTAL_IOCTYPE_MTPMX, 27, INT32)
#define MTAL_IO_PMX_PANEL_SETLVDSSELECT			_IOW(MTAL_IOCTYPE_MTPMX, 28, INT32)
#define MTAL_IO_PMX_PANEL_SETLVDSPIXEL			_IOW(MTAL_IOCTYPE_MTPMX, 29, INT32)
#define MTAL_IO_PMX_PANEL_SETRESOLUTION			_IOW(MTAL_IOCTYPE_MTPMX, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_PANEL_SETHORIPERIOD			_IOW(MTAL_IOCTYPE_MTPMX, 31, INT32)
#define MTAL_IO_PMX_PANEL_SETVERTPERIOD			_IOW(MTAL_IOCTYPE_MTPMX, 32, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PMX_PANEL_SETVERTSYNCWIDTH		_IOW(MTAL_IOCTYPE_MTPMX, 33, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PMX_PANEL_SETVERTBACKPROCH		_IOW(MTAL_IOCTYPE_MTPMX, 34, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PMX_PANEL_GETPANELINDEX			_IOW(MTAL_IOCTYPE_MTPMX, 35, INT32)
#define MTAL_IO_PMX_GETPLANEORDERARRAY			_IOW(MTAL_IOCTYPE_MTPMX, 36, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_PMX_MIX_ALPHA   			    _IOW(MTAL_IOCTYPE_MTPMX, 37, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_PMX_TCONPOWER					_IOW(MTAL_IOCTYPE_MTPMX, 38, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_TCONTABLESEL				_IOW(MTAL_IOCTYPE_MTPMX, 39, INT32)
#define MTAL_IO_PMX_PANEL_SETDISPLAYTYPE        _IOW(MTAL_IOCTYPE_MTPMX, 40, INT32)
#define MTAL_IO_PMX_PANEL_FORCEFRAMERATE		_IOW(MTAL_IOCTYPE_MTPMX, 41, INT32)
#define MTAL_IO_PMX_PANEL_SETHORIZSYNCWIDTH		_IOW(MTAL_IOCTYPE_MTPMX, 42, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PMX_PANEL_SETHORIZBACKPROCH		_IOW(MTAL_IOCTYPE_MTPMX, 43, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PMX_PANEL_SET3DSYNCOUT          _IOW(MTAL_IOCTYPE_MTPMX, 44, INT32)
#define MTAL_IO_PMX_PANEL_SET3DSYNCOUTDELAY     _IOW(MTAL_IOCTYPE_MTPMX, 45, INT32)
#define MTAL_IO_PMX_PANEL_SET2NDCHANNELONOFF    _IOW(MTAL_IOCTYPE_MTPMX, 46, INT32)
#define MTAL_IO_PMX_SHOWPATTERN              	_IOW(MTAL_IOCTYPE_MTPMX, 47, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_PMX_PANEL_GETPANELINFO          _IOW(MTAL_IOCTYPE_MTPMX, 48, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_PMX_GET_PANEL_TYPE				_IOW(MTAL_IOCTYPE_MTPMX, 49, INT32)
#define MTAL_IO_PMX_PANEL_LOADPANELINDEX     	_IOW(MTAL_IOCTYPE_MTPMX, 50, INT32)
#define MTAL_IO_PMX_PANEL_DYNAMIC_CHG			_IOW(MTAL_IOCTYPE_MTPMX, 51, INT32)
#define MTAL_IO_PMX_PANEL_SETLVDSSWAP			_IOW(MTAL_IOCTYPE_MTPMX, 52, INT32)
#define MTAL_IO_PMX_PANEL_SETPCBLVDSSWAP		_IOW(MTAL_IOCTYPE_MTPMX, 53, INT32)
#define MTAL_IO_PMX_PANEL_SET4K2KMODE      		_IOW(MTAL_IOCTYPE_MTPMX, 54, INT32)
#define MTAL_IO_PMX_SETBlk						_IOW(MTAL_IOCTYPE_MTPMX, 55, INT32)
#define MTAL_IO_PMX_PANEL_SETGFXTYPE            _IOW(MTAL_IOCTYPE_MTPMX, 56, INT32)
#define MTAL_IO_PMX_PANEL_SETGFXFRAMEREADY      _IOW(MTAL_IOCTYPE_MTPMX, 57, INT32)
#define MTAL_IO_PMX_ENABLEVBYONESIGNAL			_IOW(MTAL_IOCTYPE_MTPMX, 58, INT32)
#define MTAL_IO_PMX_PANEL_GETDISPLAYTYPE        _IOW(MTAL_IOCTYPE_MTPMX, 59, INT32)
#define MTAL_IO_PMX_TCONREGWRITE                _IOW(MTAL_IOCTYPE_MTPMX, 60, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_TCONREGREAD             	_IOW(MTAL_IOCTYPE_MTPMX, 61, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PMX_PANEL_TCONINIT				_IOW(MTAL_IOCTYPE_MTPMX, 62, INT32)
#define MTAL_TO_PMX_PANEL_TCONENABLE			_IOW(MTAL_IOCTYPE_MTPMX, 63, INT32)
#define MTAL_IO_PMX_PANEL_GETGFXFRAMERATE       _IOW(MTAL_IOCTYPE_MTPMX, 64, INT32)


/* MTAL_IOCTYPE_MTSCART 13 */
#define MTAL_IO_SCART_SETINPMODE                        _IOW(MTAL_IOCTYPE_MTSCART, 0, INT32)
#define MTAL_IO_SCART_REGPIN8FUNC                       _IOW(MTAL_IOCTYPE_MTSCART, 1, INT32)
#define MTAL_IO_SCART_GETPIN8STE                        _IOW(MTAL_IOCTYPE_MTSCART, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SCART_GETINPMODE                        _IOW(MTAL_IOCTYPE_MTSCART, 3, INT32)
#define MTAL_IO_SCART_GETHWINPMODE                      _IOW(MTAL_IOCTYPE_MTSCART, 4, INT32)
#define MTAL_IO_SCART_SETRGBOVERLAY                     _IOW(MTAL_IOCTYPE_MTSCART, 5, INT32)
#define MTAL_IO_SCART_SETSCARTMATRIXENABLE                         _IOW(MTAL_IOCTYPE_MTSCART, 6, INT32)
#define MTAL_IO_SCART_SETSCARTMATRIXYOFFSET                         _IOW(MTAL_IOCTYPE_MTSCART, 7, INT32)
#define MTAL_IO_SCART_SETSCARTMATRIXRGBOFFSET                         _IOW(MTAL_IOCTYPE_MTSCART, 8, INT32)
#define MTAL_IO_SCART_SETSCARTRGBPORCH                         _IOW(MTAL_IOCTYPE_MTSCART, 9, INT32)
#define MTAL_IO_SCART_GETSCARTRGBPORCH                         _IOW(MTAL_IOCTYPE_MTSCART, 10, INT32)
#define MTAL_IO_SCART_GETSCARTFBSTATUS                         _IOW(MTAL_IOCTYPE_MTSCART, 11, INT32)


/* MTAL_IOCTYPE_MTVDEC 14 */
#define MTAL_IO_VDEC_INIT                                _IOW(MTAL_IOCTYPE_MTVDEC, 0, INT32)
#define MTAL_IO_VDEC_TERMINT                             _IOW(MTAL_IOCTYPE_MTVDEC, 1, INT32)
#define MTAL_IO_VDEC_RESET                               _IOW(MTAL_IOCTYPE_MTVDEC, 2, INT32)
#define MTAL_IO_VDEC_PLAY                                _IOW(MTAL_IOCTYPE_MTVDEC, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_PLAY_I_FRAME                        _IOW(MTAL_IOCTYPE_MTVDEC, 4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_PAUSE                               _IOW(MTAL_IOCTYPE_MTVDEC, 5, INT32)
#define MTAL_IO_VDEC_STOP                                _IOW(MTAL_IOCTYPE_MTVDEC, 6, INT32)
#define MTAL_IO_VDEC_STOP_I_FRAME                        _IOW(MTAL_IOCTYPE_MTVDEC, 7, INT32)
#define MTAL_IO_VDEC_GETRESOLUTION                       _IOW(MTAL_IOCTYPE_MTVDEC, 8, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_VDEC_SETAVSYNMODE                        _IOW(MTAL_IOCTYPE_MTVDEC, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GETSTC                              _IOW(MTAL_IOCTYPE_MTVDEC, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GETDECSTATUS                        _IOW(MTAL_IOCTYPE_MTVDEC, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_REGDECERRCB                         _IOW(MTAL_IOCTYPE_MTVDEC, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_REGUSERDATACB                       _IOW(MTAL_IOCTYPE_MTVDEC, 13, INT32)
#define MTAL_IO_VDEC_STARTUSERDATA                       _IOW(MTAL_IOCTYPE_MTVDEC, 14, INT32)
#define MTAL_IO_VDEC_STOPUSERDATA                        _IOW(MTAL_IOCTYPE_MTVDEC, 15, INT32)
#define MTAL_IO_VDEC_GETUSERDATA                         _IOW(MTAL_IOCTYPE_MTVDEC, 16, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_REGFRAMECB                          _IOW(MTAL_IOCTYPE_MTVDEC, 17, INT32)
#define MTAL_IO_VDEC_REGSTATUSCB                        _IOW(MTAL_IOCTYPE_MTVDEC, 18, INT32)
#define MTAL_IO_VDEC_GETDBKINFO                          _IOW(MTAL_IOCTYPE_MTVDEC, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETSPEED                            _IOW(MTAL_IOCTYPE_MTVDEC, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETFRAMEADVANCE                     _IOW(MTAL_IOCTYPE_MTVDEC, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETFLUSHESDATA                      _IOW(MTAL_IOCTYPE_MTVDEC, 22, INT32)
#define MTAL_IO_VDEC_SETDECODEMODE                       _IOW(MTAL_IOCTYPE_MTVDEC, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_DECODE_I_FRAME                      _IOW(MTAL_IOCTYPE_MTVDEC, 24, MTAL_IOCTL_7ARG_T)  // 20110801
#define MTAL_IO_VDEC_QUERY_FBM_POOL                      _IOW(MTAL_IOCTYPE_MTVDEC, 25, MTAL_IOCTL_4ARG_T)  // 20111222
#ifdef CC_ITIMING_ALWAYS
#define MTAL_IO_VDEC_SETFHDDISPLAY                       _IOW(MTAL_IOCTYPE_MTVDEC, 26, MTAL_IOCTL_2ARG_T)
#endif
#define MTAL_IO_VDEC_PLAYCC                              _IOW(MTAL_IOCTYPE_MTVDEC, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_STOPCC                              _IOW(MTAL_IOCTYPE_MTVDEC, 28, INT32)
#define MTAL_IO_VDEC_ALLOCCCBUF                          _IOW(MTAL_IOCTYPE_MTVDEC, 29, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_FREECCBUF                           _IOW(MTAL_IOCTYPE_MTVDEC, 30, INT32)
#define MTAL_IO_VDEC_FLUSHCCBUF                          _IOW(MTAL_IOCTYPE_MTVDEC, 31, INT32)
#define MTAL_IO_VDEC_GETCC                               _IOW(MTAL_IOCTYPE_MTVDEC, 32, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GETDBKINFO2                         _IOW(MTAL_IOCTYPE_MTVDEC, 33, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETDBKINFO2                         _IOW(MTAL_IOCTYPE_MTVDEC, 34, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_ALLOCIFRAMEBUF                      _IOW(MTAL_IOCTYPE_MTVDEC, 35, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_FREEIFRAMEBUF                       _IOW(MTAL_IOCTYPE_MTVDEC, 36, INT32)
#define MTAL_IO_VDEC_SETIFRAMEBUF                        _IOW(MTAL_IOCTYPE_MTVDEC, 37, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETIFRAMEDEC                        _IOW(MTAL_IOCTYPE_MTVDEC, 38, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_REGAUTOTESTCB                       _IOW(MTAL_IOCTYPE_MTVDEC, 39, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETCRCGOLDEN                        _IOW(MTAL_IOCTYPE_MTVDEC, 40, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETAUTOTESTFILEDIR                  _IOW(MTAL_IOCTYPE_MTVDEC, 41, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_SETAUTOTESTFILENAME                 _IOW(MTAL_IOCTYPE_MTVDEC, 42, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_CHKDIVXBONDING                      _IOW(MTAL_IOCTYPE_MTVDEC, 43, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETABINFO                           _IOW(MTAL_IOCTYPE_MTVDEC, 44, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETSRCID                            _IOW(MTAL_IOCTYPE_MTVDEC, 45, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_PLAYMM                              _IOW(MTAL_IOCTYPE_MTVDEC, 46, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_REGGENERALCB                        _IOW(MTAL_IOCTYPE_MTVDEC, 47, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GETTHUMBNAIL                        _IOW(MTAL_IOCTYPE_MTVDEC, 48, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_VDEC_PLAY2                               _IOW(MTAL_IOCTYPE_MTVDEC, 49, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_SETINFO                             _IOW(MTAL_IOCTYPE_MTVDEC, 50, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_CHKCODECCAP                         _IOW(MTAL_IOCTYPE_MTVDEC, 51, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_CLRMMPARAM                          _IOW(MTAL_IOCTYPE_MTVDEC, 52, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SETMMPARAM                          _IOW(MTAL_IOCTYPE_MTVDEC, 53, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_VDEC_PLAYNOTDISPLAY                      _IOW(MTAL_IOCTYPE_MTVDEC, 54, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_REPLAYVDEC                          _IOW(MTAL_IOCTYPE_MTVDEC, 55, INT32)
#define MTAL_IO_VDEC_SETIPTVMODE                         _IOW(MTAL_IOCTYPE_MTVDEC, 56, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GET_LOGO                            _IOW(MTAL_IOCTYPE_MTVDEC, 57, INT32)
#define MTAL_IO_VDEC_GET_LOGO_ADDR                       _IOW(MTAL_IOCTYPE_MTVDEC, 58, INT32)
#define MTAL_IO_VDEC_SET_LOGO                            _IOW(MTAL_IOCTYPE_MTVDEC, 59, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SET_LOGO_ADDR                       _IOW(MTAL_IOCTYPE_MTVDEC, 60, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_SET_SUPERFREEZE                     _IOW(MTAL_IOCTYPE_MTVDEC, 61, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GET_DISP_FRM_ADDR                   _IOW(MTAL_IOCTYPE_MTVDEC, 62, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_THUMBNAILFLUSH                      _IOW(MTAL_IOCTYPE_MTVDEC, 63, INT32)
#define MTAL_IO_VDEC_TRY_CTRL_VALID_SRC                  _IOW(MTAL_IOCTYPE_MTVDEC, 64, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SET_SKEP_MODE                       _IOW(MTAL_IOCTYPE_MTVDEC, 65, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_CLICMD                              _IOW(MTAL_IOCTYPE_MTVDEC, 66, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_ChgYUV2YCbCr                        _IOW(MTAL_IOCTYPE_MTVDEC, 67, INT32)
#define MTAL_IO_VDEC_ALLOC_INPUT                         _IOW(MTAL_IOCTYPE_MTVDEC, 68, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_RELEASE_INPUT                       _IOW(MTAL_IOCTYPE_MTVDEC, 69, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_GET_INPUT_BUF                       _IOW(MTAL_IOCTYPE_MTVDEC, 70, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_PUT_INPUT_BUF                       _IOW(MTAL_IOCTYPE_MTVDEC, 71, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_RM_CB                               _IOW(MTAL_IOCTYPE_MTVDEC, 72, INT32)
#define MTAL_IO_VDEC_RM_SET                              _IOW(MTAL_IOCTYPE_MTVDEC, 73, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_GET_DECNUM                          _IOW(MTAL_IOCTYPE_MTVDEC, 74, INT32)
#define MTAL_IO_VDEC_GETWFDERRINFO                       _IOW(MTAL_IOCTYPE_MTVDEC, 75, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_VDEC_RESET_WFD_INFO                      _IOW(MTAL_IOCTYPE_MTVDEC, 76, INT32)
#define MTAL_IO_VDEC_REGCALLBACK                         _IOW(MTAL_IOCTYPE_MTVDEC, 77, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_GET_DELAYTIME                       _IOW(MTAL_IOCTYPE_MTVDEC, 78, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_GET_DISP_FRM                        _IOW(MTAL_IOCTYPE_MTVDEC, 79, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_SETFRAMETOUSER                      _IOW(MTAL_IOCTYPE_MTVDEC, 80, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_GETTED_DATA                         _IOW(MTAL_IOCTYPE_MTVDEC, 81, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_PTS                      _IOW(MTAL_IOCTYPE_MTVDEC, 82, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_FROM_PTS                 _IOW(MTAL_IOCTYPE_MTVDEC, 83, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_FROM_FIRST_PIC           _IOW(MTAL_IOCTYPE_MTVDEC, 84, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_FROM_POS                 _IOW(MTAL_IOCTYPE_MTVDEC, 85, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_FROM_BACK_STEP           _IOW(MTAL_IOCTYPE_MTVDEC, 86, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VDEC_GET_COLOUR                          _IOW(MTAL_IOCTYPE_MTVDEC, 87, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDEC_SET_RENDER_FROM_PTS_AUTO_PAUSE      _IOW(MTAL_IOCTYPE_MTVDEC, 88, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_PLAY_ONE_FRAME                      _IOW(MTAL_IOCTYPE_MTVDEC, 89, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDEC_GET_HAL_PARAMETER                   _IOW(MTAL_IOCTYPE_MTVDEC, 90, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_VDEC_GET_DIRECT_VDEC					 _IOW(MTAL_IOCTYPE_MTVDEC, 91, MTAL_IOCTL_2ARG_T)

/* MTAL_IOCTYPE_MTVDECEX 31 */
#define MTAL_IO_VDECEX_ISPCTIMING                                _IOW(MTAL_IOCTYPE_MTVDECEX, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETVIDEOSRC                                _IOW(MTAL_IOCTYPE_MTVDECEX, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETMAINSUBSRC                             _IOW(MTAL_IOCTYPE_MTVDECEX, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETCOLORSYSTEM                            _IOW(MTAL_IOCTYPE_MTVDECEX, 3, INT32)
#define MTAL_IO_VDECEX_GETCOLORSYSTEM                            _IOW(MTAL_IOCTYPE_MTVDECEX, 4, INT32)
#define MTAL_IO_VDECEX_SETPORCH                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 5, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_GETPORCH                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 6, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_STARTVGAAUTO                              _IOW(MTAL_IOCTYPE_MTVDECEX, 7, INT32)
#define MTAL_IO_VDECEX_STARTAUTOCOLOR                            _IOW(MTAL_IOCTYPE_MTVDECEX, 8, INT32)
#define MTAL_IO_VDECEX_SETPHASE                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 9, INT32)
#define MTAL_IO_VDECEX_GETPHASE                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETCLOCK                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 11, INT32)
#define MTAL_IO_VDECEX_GETCLOCK                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_VGA_GETAMBIGUOUSTIMINGNUM                 _IOW(MTAL_IOCTYPE_MTVDECEX, 13, INT32)
#define MTAL_IO_VDECEX_VGA_QUERYAMBIGUOUSTIMING                  _IOW(MTAL_IOCTYPE_MTVDECEX, 14, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VGA_SETAMBIGUOUSINDEX                     _IOW(MTAL_IOCTYPE_MTVDECEX, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_VGA_GETCURRENTAMBIGUOUSINDEX              _IOW(MTAL_IOCTYPE_MTVDECEX, 16, INT32)
#define MTAL_IO_VDECEX_REGCB                            			_IOW(MTAL_IOCTYPE_MTVDECEX, 17, MTAL_IOCTL_3ARG_T)
//#define MTAL_IO_VDECEX_REGSIGCHGNFYCB                            _IOW(MTAL_IOCTYPE_MTVDECEX, 17, INT32)
//#define MTAL_IO_VDECEX_REGVGAAUTOCB                              _IOW(MTAL_IOCTYPE_MTVDECEX, 18, INT32)
//#define MTAL_IO_VDECEX_REGAUTOCOLORCB                            _IOW(MTAL_IOCTYPE_MTVDECEX, 19, INT32)
#define MTAL_IO_VDECEX_HDMI_GETASPECTRATIO                   _IOW(MTAL_IOCTYPE_MTVDECEX, 19, INT32)
#define MTAL_IO_VDECEX_HDMI_SETHPDADJUST                         _IOW(MTAL_IOCTYPE_MTVDECEX, 20, INT32)
#define MTAL_IO_VDECEX_HDMI_GETINFOFRAME                         _IOW(MTAL_IOCTYPE_MTVDECEX, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_GETMODE                              _IOW(MTAL_IOCTYPE_MTVDECEX, 22, INT32)
#define MTAL_IO_VDECEX_HDMI_GETDEEPCOLORMODE                              _IOW(MTAL_IOCTYPE_MTVDECEX, 23, INT32)
#define MTAL_IO_VDECEX_STOPVGAAUTO				_IOW(MTAL_IOCTYPE_MTVDECEX, 24, INT32)
#define MTAL_IO_VDECEX_VGA_SETAMBIGUOUSUSRSEL		_IOW(MTAL_IOCTYPE_MTVDECEX, 25, INT32)
#define MTAL_IO_VDECEX_TVD_SETMAGC		_IOW(MTAL_IOCTYPE_MTVDECEX, 26, INT32)
#define MTAL_IO_VDECEX_TVD_SET2CHPGAGAIN		_IOW(MTAL_IOCTYPE_MTVDECEX, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_SETAGCDIGITALGAIN		_IOW(MTAL_IOCTYPE_MTVDECEX, 28, INT32)
#define MTAL_IO_VDECEX_TVD_SETCAGC		_IOW(MTAL_IOCTYPE_MTVDECEX, 29, INT32)
#define MTAL_IO_VDECEX_TVD_SETUVDELAY		_IOW(MTAL_IOCTYPE_MTVDECEX, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_SET2CHCLAMPLEVEL		_IOW(MTAL_IOCTYPE_MTVDECEX, 31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_SET2CHAGCMODE		_IOW(MTAL_IOCTYPE_MTVDECEX, 32, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_GETSTATUS		_IOW(MTAL_IOCTYPE_MTVDECEX, 33, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_SETAAF		_IOW(MTAL_IOCTYPE_MTVDECEX, 34, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_ISPCM                              _IOW(MTAL_IOCTYPE_MTVDECEX, 35, INT32)
#define MTAL_IO_VDECEX_HDMI_GETAUDIOFREQUENCY                              _IOW(MTAL_IOCTYPE_MTVDECEX, 36, INT32)
#define MTAL_IO_VDECEX_HDMI_GETDAUDIOBITWIDTH                              _IOW(MTAL_IOCTYPE_MTVDECEX, 37, INT32)
#define MTAL_IO_VDECEX_ISOVERSAMPLETIMING                                _IOW(MTAL_IOCTYPE_MTVDECEX, 38, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETOVERSAMPLEDEPENDENT                                _IOW(MTAL_IOCTYPE_MTVDECEX, 39, INT32)
#define MTAL_IO_VDECEX_GETOVERSAMPLEDEPENDENT                                _IOW(MTAL_IOCTYPE_MTVDECEX, 40, INT32)
#define MTAL_IO_VDECEX_SETAUTOSEARCH                         _IOW(MTAL_IOCTYPE_MTVDECEX, 41, INT32)
#define MTAL_IO_VDECEX_TVD_SETSECAMSETTING                         _IOW(MTAL_IOCTYPE_MTVDECEX, 42, INT32)
#define MTAL_IO_VDECEX_STARTCVBSAUTOCOLOR                            _IOW(MTAL_IOCTYPE_MTVDECEX, 43, INT32)
#define MTAL_IO_VDECEX_TVD_HOLDMODE                            _IOW(MTAL_IOCTYPE_MTVDECEX, 44, INT32)
#define MTAL_IO_VDECEX_HDMI_GETDAUDIOCATEGORYCODE                    _IOW(MTAL_IOCTYPE_MTVDECEX, 45, INT32)
#define MTAL_IO_VDECEX_HDMI_GETCOLORDOMAIN                      _IOW(MTAL_IOCTYPE_MTVDECEX, 46, INT32)
#define MTAL_IO_VDECEX_SETMONITORSRC                                _IOW(MTAL_IOCTYPE_MTVDECEX, 47, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOSTATUS                                _IOW(MTAL_IOCTYPE_MTVDECEX, 48, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOSRCTYPE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 49, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEODEFSTATUS                                _IOW(MTAL_IOCTYPE_MTVDECEX, 50, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_VDECEX_GET_VIDEODECTYPE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 51, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_VDECEX_VGA_MWIFGETAMBIGUOUSTIMINGNUM                                _IOW(MTAL_IOCTYPE_MTVDECEX, 52, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_VDECEX_VGA_MWIFQUERYAMBIGUOUSTIMING                                _IOW(MTAL_IOCTYPE_MTVDECEX, 53, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_VDECEX_VGA_MWIFSETAMBIGUOUSINDEX                                _IOW(MTAL_IOCTYPE_MTVDECEX, 54, MTAL_IOCTL_2ARG_T)
//#define MTAL_IO_VDECEX_VGA_MWIFSETAMBIGUOUSUSRSEL                                _IOW(MTAL_IOCTYPE_MTVDECEX, 55, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_VGA_MWIFRESETAMBIGUOUSTIMINGEEPROM                                _IOW(MTAL_IOCTYPE_MTVDECEX, 56, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_GET_SIGNALSTRENGTHLEVEL                                _IOW(MTAL_IOCTYPE_MTVDECEX, 57, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SETPORCHTUNE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 58, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_STARTYPBPRAUTO                                _IOW(MTAL_IOCTYPE_MTVDECEX, 59, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOREFRESHRATE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 60, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOINPUTWIDTH                                _IOW(MTAL_IOCTYPE_MTVDECEX, 61, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOINPUTHEIGHT                                _IOW(MTAL_IOCTYPE_MTVDECEX, 62, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_IS_VIDEOSRCINTERLACE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 63, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_IS_MHLSIGNAL											_IOW(MTAL_IOCTYPE_MTVDECEX, 64, MTAL_IOCTL_2ARG_T)

//#define MTAL_IO_VDECEX_TDTV_GETTAG3D                                _IOW(MTAL_IOCTYPE_MTVDECEX, 64, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOINPUTPOLARITY                                _IOW(MTAL_IOCTYPE_MTVDECEX, 65, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOINPUTTIMINGID                                _IOW(MTAL_IOCTYPE_MTVDECEX, 66, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GET_VIDEOINTERNALDEC                                _IOW(MTAL_IOCTYPE_MTVDECEX, 67, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_GET_NRLEVEL                                _IOW(MTAL_IOCTYPE_MTVDECEX, 68, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_GET_HDCPSTATUS                                _IOW(MTAL_IOCTYPE_MTVDECEX, 69, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_GET_SCANINFO                                _IOW(MTAL_IOCTYPE_MTVDECEX, 70, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_GET_AFD                                _IOW(MTAL_IOCTYPE_MTVDECEX, 71, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_GET_INPUTTYPE                                _IOW(MTAL_IOCTYPE_MTVDECEX, 72, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_SUPPORT_AUDIO                                _IOW(MTAL_IOCTYPE_MTVDECEX, 73, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SET_SIGNALBOOSTER                                _IOW(MTAL_IOCTYPE_MTVDECEX, 74, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_SET                                _IOW(MTAL_IOCTYPE_MTVDECEX, 75, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDECEX_GET                                _IOW(MTAL_IOCTYPE_MTVDECEX, 76, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDECEX_TVD_PEDESTAL_EN                            _IOW(MTAL_IOCTYPE_MTVDECEX, 77, INT32)
#define MTAL_IO_VDECEX_GETVIDEODECODERTYPE                            _IOW(MTAL_IOCTYPE_MTVDECEX, 78, INT32)
#define MTAL_IO_VDECEX_TVD_GETDEFAULTPORCH                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 79, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_TVD_SETCHANNELCHANGE                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 80, INT32)
#define MTAL_IO_VDECEX_TVD_SETENABLEDCOLORSYSTEM                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 81, INT32)
#define MTAL_IO_VDECEX_TVD_SETVPRESONOFFGAIN                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 82, INT32)
#define MTAL_IO_VDECEX_TVD_SETHPLL                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 83, INT32)
#define MTAL_IO_VDECEX_HDMI_SET_HDCP_KEY                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 84, INT32)
#define MTAL_IO_VDECEX_HDMI_SET_EDID_DATA                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 85, INT32)
#define MTAL_IO_VDECEX_VGA_SET_EDID_DATA                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 86, INT32)
#define MTAL_IO_VDECEX_HDMI_GET_HDCP_KEY                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 87, INT32)
#define MTAL_IO_VDECEX_HDMI_GET_EDID_DATA                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 88, INT32)
#define MTAL_IO_VDECEX_VGA_GET_EDID_DATA                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 89, INT32)
#define MTAL_IO_VDECEX_HDMI_EDID_ONOFF_CTRL                         _IOW(MTAL_IOCTYPE_MTVDECEX, 90, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_VGA_EDID_ONOFF_CTRL                          _IOW(MTAL_IOCTYPE_MTVDECEX, 91, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_GETPORCHTUNE                                  _IOW(MTAL_IOCTYPE_MTVDECEX, 92, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_HDMI_FASTINIT                            _IOW(MTAL_IOCTYPE_MTVDECEX, 93, INT32)
#define MTAL_IO_VDECEX_TVD_GET_SCANADJ_INFO                           _IOW(MTAL_IOCTYPE_MTVDECEX, 94, INT32)
#define MTAL_IO_VDECEX_TVD_SET_SCANADJ_INFO                           _IOW(MTAL_IOCTYPE_MTVDECEX, 95, INT32)
#define MTAL_IO_VDECEX_GET_SRC_DETECT_STATUS                           _IOW(MTAL_IOCTYPE_MTVDECEX, 96, INT32)
#define MTAL_IO_VDECEX_TVD_GET_ANALOG_COPY_PROTECT                    _IOW(MTAL_IOCTYPE_MTVDECEX, 97, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_GETHDMISPDUPSTA                        _IOW(MTAL_IOCTYPE_MTVDECEX, 98, INT32)
#define MTAL_IO_VDECEX_GETHDMISPDUPSTACHG                        _IOW(MTAL_IOCTYPE_MTVDECEX, 99, INT32)
#define MTAL_IO_VDECEX_GETHDMISPDSVSTA                        _IOW(MTAL_IOCTYPE_MTVDECEX, 100, INT32)
#define MTAL_IO_VDECEX_GETHDMISPDSVSTACHG                        _IOW(MTAL_IOCTYPE_MTVDECEX, 101, INT32)
#define MTAL_IO_VDECEX_GETVIDEOSRC                           _IOW(MTAL_IOCTYPE_MTVDECEX, 102, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_GET_PRE_WIDTH_HEIGHT              _IOW(MTAL_IOCTYPE_MTVDECEX, 103, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDECEX_TVD_SET_SYNCDET_FOR_TUNING            _IOW(MTAL_IOCTYPE_MTVDECEX, 104, INT32)
#define MTAL_IO_VDECEX_HDMI_INIT                            _IOW(MTAL_IOCTYPE_MTVDECEX, 105, INT32)
#define MTAL_IO_VDECEX_HDMI_UnINIT                            _IOW(MTAL_IOCTYPE_MTVDECEX, 106, INT32)
#define MTAL_IO_VDECEX_HDMI_Open                            _IOW(MTAL_IOCTYPE_MTVDECEX, 107, INT32)
#define MTAL_IO_VDECEX_HDMI_Close                            _IOW(MTAL_IOCTYPE_MTVDECEX, 108, INT32)
#define MTAL_IO_VDECEX_HDMI_Connect                            _IOW(MTAL_IOCTYPE_MTVDECEX, 109, INT32)
#define MTAL_IO_VDECEX_HDMI_DisConn                            _IOW(MTAL_IOCTYPE_MTVDECEX, 110, INT32)
#define MTAL_IO_VDECEX_HDMI_Get_SW5v                            _IOW(MTAL_IOCTYPE_MTVDECEX, 111, INT32)
#define MTAL_IO_VDECEX_HDMI_Get_Source_KSV                            _IOW(MTAL_IOCTYPE_MTVDECEX, 112, INT32)
#define MTAL_IO_VDECEX_HDMI_EnableHPD                            _IOW(MTAL_IOCTYPE_MTVDECEX, 113, INT32)
#define MTAL_IO_VDECEX_HDMI_DisableHPD                            _IOW(MTAL_IOCTYPE_MTVDECEX, 114, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_ISMODE							_IOW(MTAL_IOCTYPE_MTVDECEX, 115, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_SENDMSCMSG							_IOW(MTAL_IOCTYPE_MTVDECEX, 116, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_READWRITEBURST						_IOW(MTAL_IOCTYPE_MTVDECEX, 117, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_CHKRCPRECEIVE                      _IOW(MTAL_IOCTYPE_MTVDECEX, 118, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_GETMHLRCPKEY                    _IOW(MTAL_IOCTYPE_MTVDECEX, 119, INT32)
#define MTAL_IO_VDECEX_HDMI_W2D_SETVIDEOMODE                    _IOW(MTAL_IOCTYPE_MTVDECEX, 120, INT32)
#define MTAL_IO_VDECEX_HDMI_SET_EDID_DATA_BY_PORT               _IOW(MTAL_IOCTYPE_MTVDECEX, 121, INT32)
#define MTAL_IO_VDECEX_GETHDMISPD4kPSSTA               _IOW(MTAL_IOCTYPE_MTVDECEX, 122, INT32)
#define MTAL_IO_VDECEX_HDMI_SET_EXTERNAL_EDID_DATA_BY_PORT      _IOW(MTAL_IOCTYPE_MTVDECEX, 123, INT32)
#define MTAL_IO_VDECEX_HDMI_GET_EXTERNAL_EDID_DATA_BY_PORT      _IOW(MTAL_IOCTYPE_MTVDECEX, 124, INT32)
#define MTAL_IO_VDECEX_HDMI_GETPORTINFOFRAME                    _IOW(MTAL_IOCTYPE_MTVDECEX, 125, INT32)
#define MTAL_IO_VDECEX_HDMI_GETPORTMODE                         _IOW(MTAL_IOCTYPE_MTVDECEX, 126, INT32)
#define MTAL_IO_VDECEX_HDMI_GETPORTTIMINGINFO                   _IOW(MTAL_IOCTYPE_MTVDECEX, 127, INT32)
#define MTAL_IO_VDECEX_HDMI_ENABLEMHL                           _IOW(MTAL_IOCTYPE_MTVDECEX, 128, INT32)
#define MTAL_IO_VDECEX_HDMI_MHL_CLEARRCPSTATE                   _IOW(MTAL_IOCTYPE_MTVDECEX, 129, INT32)
#define MTAL_IO_VDECEX_SETVFESRC_PIPELINE                        _IOW(MTAL_IOCTYPE_MTVDECEX, 130, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDECEX_SETVSCSRC_PIPELINE                        _IOW(MTAL_IOCTYPE_MTVDECEX, 131, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDECEX_HDMI_GETHWACTIVEINFO                      _IOW(MTAL_IOCTYPE_MTVDECEX, 132, INT32)
#define MTAL_IO_VDECEX_VFEAVDConnect                             _IOW(MTAL_IOCTYPE_MTVDECEX, 133, MTAL_IOCTL_4ARG_T)




#define MTAL_IO_VDECEX_VBI_STARTNOTIFYCCONOFF                    _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 0, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_STARTNOTIFYTTXONOFF                   _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_STARTNOTIFYWSSONOFF                   _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 2, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_STARTNOTIFYVPSONOFF                   _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 3, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_SETTTXNFY                             _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 4, INT32)
#define MTAL_IO_VDECEX_VBI_SETCCNFY                              _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 5, INT32)
#define MTAL_IO_VDECEX_VBI_SETVPSNFY                             _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 6, INT32)
#define MTAL_IO_VDECEX_VBI_SETWSSNFY                             _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 7, INT32)
#define MTAL_IO_VDECEX_VBI_SETINFONFY                            _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 8, INT32)
#define MTAL_IO_VDECEX_VBI_GETVBIFIFOINFO                        _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 9, INT32)
#define MTAL_IO_VDECEX_VBI_SETVBILINE                        _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 10, INT32)
#define MTAL_IO_VDECEX_VBI_SETCCECCONOFF                        _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 11, INT32)
#define MTAL_IO_VDECEX_VBI_STARTNOTIFYUSWSSONOFF                   _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_STARTNOTIFYTYPEBWSSONOFF                   _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 13, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_SETTYPEBWSSNFY                             _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 14, INT32)
#define MTAL_IO_VDECEX_VBI_SETUSWSSNFY                             _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 15, INT32)
#define MTAL_IO_VDECEX_VBI_GET_ANALOG_COPY_PROTECT                _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 16, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDECEX_VBI_GET_SUPPORT_MHL_PORT_ID              _IOW(MTAL_IOCTYPE_MTVDECEX_VBI, 17, INT32)


#define MTAL_IO_ATSC_OPEN                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 0, INT32)
#define MTAL_IO_ATSC_CLOSE                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 1, INT32)
#define MTAL_IO_ATSC_CONNECT                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 2, INT32)
#define MTAL_IO_ATSC_DISCONNECT                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 3, INT32)
#define MTAL_IO_ATSC_GETSYNC                _IOW(MTAL_IOCTYPE_MTFE_ATSC, 4, INT32)
#define MTAL_IO_ATSC_GETSIGLEVEL                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 5, INT32)
#define MTAL_IO_ATSC_GETPER                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ATSC_GETSNR                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 7, INT32)
#define MTAL_IO_ATSC_GETIFAGC                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 8, INT32)
#define MTAL_IO_ATSC_GETTCMBER                _IOW(MTAL_IOCTYPE_MTFE_ATSC, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ATSC_SETSPCPOLAR                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 10, INT32)
#define MTAL_IO_ATSC_SETTSDIROUT                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 11, INT32)
#define MTAL_IO_ATSC_GETVER                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 12, INT32)
#define MTAL_IO_ATSC_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 13, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_ATSC_SETIFFREQ                   _IOW(MTAL_IOCTYPE_MTFE_ATSC, 14, INT32)
#define MTAL_IO_ATSC_TUNER_CONFIG           _IOW(MTAL_IOCTYPE_MTFE_ATSC, 15, INT32)
#define MTAL_IO_ATSC_DIGTUNERBYPASSI2C           _IOW(MTAL_IOCTYPE_MTFE_ATSC, 16, INT32)
#define MTAL_IO_ATSC_GETFREQOFFSET           _IOW(MTAL_IOCTYPE_MTFE_ATSC, 17, INT32)
#define MTAL_IO_ATSC_DTD_Acquire            _IOW(MTAL_IOCTYPE_MTFE_ATSC, 18, INT32)
#define MTAL_IO_ATSC_SETTSMODE            _IOW(MTAL_IOCTYPE_MTFE_ATSC, 19, INT32)
#define MTAL_IO_ATSC_GETTSFMT            _IOW(MTAL_IOCTYPE_MTFE_ATSC, 20, INT32)
#define MTAL_IO_ATSC_SETTSFMT            _IOW(MTAL_IOCTYPE_MTFE_ATSC, 21, INT32)
#define MTAL_IO_ATSC_SetUSMode				  _IOW(MTAL_IOCTYPE_MTFE_ATSC, 22, INT32)//XJ141218
#define MTAL_IO_ATSC_SETTSPATH				  _IOW(MTAL_IOCTYPE_MTFE_ATSC, 23, INT32)//MTK07043, 150130, Add, for complete TSControlOutput feature.


#define MTAL_IO_ISDBT_OPEN                   _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 0, INT32)
#define MTAL_IO_ISDBT_CLOSE                  _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 1, INT32)
#define MTAL_IO_ISDBT_CONNECT             _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 2, INT32)
#define MTAL_IO_ISDBT_DISCONNECT        _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 3, INT32)
#define MTAL_IO_ISDBT_TSF                      _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 4, INT32)
#define MTAL_IO_ISDBT_GETPER                _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ISDBT_GETBER                _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ISDBT_GETCNR                _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 7, INT32)
#define MTAL_IO_ISDBT_GETSYNC              _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 8, INT32)
#define MTAL_IO_ISDBT_GETIFAGC            _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 9, INT32)
#define MTAL_IO_ISDBT_GETSIGLEVEL       _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 10, INT32)
#define MTAL_IO_ISDBT_GETVER                _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 11, INT32)
#define MTAL_IO_ISDBT_TSID                     _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 12, INT32)
#define MTAL_IO_ISDBT_LNA                       _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 13, INT32)
#define MTAL_IO_ISDBT_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_ISDBT, 14, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_ISDBS_OPEN                   _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 0, INT32)
#define MTAL_IO_ISDBS_CLOSE                  _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 1, INT32)
#define MTAL_IO_ISDBS_CONNECT             _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 2, INT32)
#define MTAL_IO_ISDBS_DISCONNECT        _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 3, INT32)
#define MTAL_IO_ISDBS_TSF                      _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 4, INT32)
#define MTAL_IO_ISDBS_GETPER                _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ISDBS_GETBER                _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_ISDBS_GETCNR                _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 7, INT32)
#define MTAL_IO_ISDBS_GETSYNC              _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 8, INT32)
#define MTAL_IO_ISDBS_GETIFAGC            _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 9, INT32)
#define MTAL_IO_ISDBS_GETSIGLEVEL       _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 10, INT32)
#define MTAL_IO_ISDBS_GETVER                _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 11, INT32)
#define MTAL_IO_ISDBS_TSID                     _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 12, INT32)
#define MTAL_IO_ISDBS_LNB                       _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 13, INT32)
#define MTAL_IO_ISDBS_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_ISDBS, 14, MTAL_IOCTL_3ARG_T)

#define MTAL_IO_DVBT_OPEN                     _IOW(MTAL_IOCTYPE_MTFE_DVBT, 0, INT32)
#define MTAL_IO_DVBT_CLOSE                    _IOW(MTAL_IOCTYPE_MTFE_DVBT, 1, INT32)
#define MTAL_IO_DVBT_CONNECT                  _IOW(MTAL_IOCTYPE_MTFE_DVBT, 2, INT32)/*fix me*/
#define MTAL_IO_DVBT_DISCONNECT               _IOW(MTAL_IOCTYPE_MTFE_DVBT, 3, INT32)
#define MTAL_IO_DVBT_GETSYNC                  _IOW(MTAL_IOCTYPE_MTFE_DVBT, 4, INT32)
#define MTAL_IO_DVBT_GETSIGLEVEL              _IOW(MTAL_IOCTYPE_MTFE_DVBT, 5, INT32)
#define MTAL_IO_DVBT_GETPER                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DVBT_GETSNR                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 7, INT32)/*fix me*/
#define MTAL_IO_DVBT_GETIFAGC                 _IOW(MTAL_IOCTYPE_MTFE_DVBT, 8, INT32)
#define MTAL_IO_DVBT_GETTCMBER                _IOW(MTAL_IOCTYPE_MTFE_DVBT, 9, INT32)
#define MTAL_IO_DVBT_GETVER                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 10, INT32)/*fix me*/
#define MTAL_IO_DVBT_REGCB                    _IOW(MTAL_IOCTYPE_MTFE_DVBT, 11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DVBT_GETTPS                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 12, INT32)
#define MTAL_IO_DVBT_GETFREQOFFSET            _IOW(MTAL_IOCTYPE_MTFE_DVBT, 13, INT32)
#define MTAL_IO_DVBT_TUNER_CONFIG             _IOW(MTAL_IOCTYPE_MTFE_DVBT, 14, INT32)
#define MTAL_IO_DVBT_SETTSPATH         		    _IOW(MTAL_IOCTYPE_MTFE_DVBT, 15, INT32)
#define MTAL_IO_DVBT_SETTSINTERFACE           _IOW(MTAL_IOCTYPE_MTFE_DVBT, 16, INT32)
#define MTAL_IO_DVBT_GETSQI                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 17, INT32)
#define MTAL_IO_DVBT_SETTSPAD                 _IOW(MTAL_IOCTYPE_MTFE_DVBT, 18, INT32)
#define MTAL_IO_DVBT_SETTSCTRL                _IOW(MTAL_IOCTYPE_MTFE_DVBT, 19, INT32)
#define MTAL_IO_DVBT_GetTsFormat              _IOW(MTAL_IOCTYPE_MTFE_DVBT, 20, INT32)
#define MTAL_IO_DVBT_SetTsFormat              _IOW(MTAL_IOCTYPE_MTFE_DVBT, 21, INT32)
#define MTAL_IO_DVBT_SUSPEND                  _IOW(MTAL_IOCTYPE_MTFE_DVBT, 22, INT32)
#define MTAL_IO_DVBT_RESUME                   _IOW(MTAL_IOCTYPE_MTFE_DVBT, 23, INT32)
#define MTAL_IO_DVBT_GETBERParam              _IOW(MTAL_IOCTYPE_MTFE_DVBT, 24, INT32)
#define MTAL_IO_DVBT_GETTIMEOUT               _IOW(MTAL_IOCTYPE_MTFE_DVBT, 25, INT32) 
#define MTAL_IO_DVBT_SetEUMode				  _IOW(MTAL_IOCTYPE_MTFE_DVBT, 26, INT32)//XJ141205


#define MTAL_IO_DVBC_OPEN                     _IOW(MTAL_IOCTYPE_MTFE_DVBC, 0, INT32)
#define MTAL_IO_DVBC_CLOSE                    _IOW(MTAL_IOCTYPE_MTFE_DVBC, 1, INT32)
#define MTAL_IO_DVBC_CONNECT                  _IOW(MTAL_IOCTYPE_MTFE_DVBC, 2, INT32)/*fix me*/
#define MTAL_IO_DVBC_DISCONNECT               _IOW(MTAL_IOCTYPE_MTFE_DVBC, 3, INT32)
#define MTAL_IO_DVBC_GETSYNC                  _IOW(MTAL_IOCTYPE_MTFE_DVBC, 4, INT32)
#define MTAL_IO_DVBC_GETSIGLEVEL              _IOW(MTAL_IOCTYPE_MTFE_DVBC, 5, INT32)
#define MTAL_IO_DVBC_GETPER                   _IOW(MTAL_IOCTYPE_MTFE_DVBC, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DVBC_GETSNR                   _IOW(MTAL_IOCTYPE_MTFE_DVBC, 7, INT32)/*fix me*/
#define MTAL_IO_DVBC_GETIFAGC                 _IOW(MTAL_IOCTYPE_MTFE_DVBC, 8, INT32)
#define MTAL_IO_DVBC_GETTCMBER                _IOW(MTAL_IOCTYPE_MTFE_DVBC, 9, INT32)
#define MTAL_IO_DVBC_GETVER                   _IOW(MTAL_IOCTYPE_MTFE_DVBC, 10, INT32)/*fix me*/
#define MTAL_IO_DVBC_REGCB                    _IOW(MTAL_IOCTYPE_MTFE_DVBC, 11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DVBC_GETFREQOFFSET            _IOW(MTAL_IOCTYPE_MTFE_DVBC, 12, INT32)
#define MTAL_IO_DVBC_TUNER_CONFIG             _IOW(MTAL_IOCTYPE_MTFE_DVBC, 13, INT32)
#define MTAL_IO_DVBC_SETTSPATH         		    _IOW(MTAL_IOCTYPE_MTFE_DVBC, 14, INT32)
#define MTAL_IO_DVBC_SETTSINTERFACE           _IOW(MTAL_IOCTYPE_MTFE_DVBC, 15, INT32)
#define MTAL_IO_DVBC_GetTPSINFO               _IOW(MTAL_IOCTYPE_MTFE_DVBC, 16, INT32)
#define MTAL_IO_DVBC_GetTsFormat              _IOW(MTAL_IOCTYPE_MTFE_DVBC, 17, INT32)
#define MTAL_IO_DVBC_SetTsFormat              _IOW(MTAL_IOCTYPE_MTFE_DVBC, 18, INT32)
#define MTAL_IO_DVBC_GETSQI					          _IOW(MTAL_IOCTYPE_MTFE_DVBC, 19, INT32)
#define MTAL_IO_DVBC_GETSSIDBM				        _IOW(MTAL_IOCTYPE_MTFE_DVBC, 20, INT32) 
#define MTAL_IO_DVBC_SUSPEND			  	        _IOW(MTAL_IOCTYPE_MTFE_DVBC, 21, INT32) 
#define MTAL_IO_DVBC_RESUME			              _IOW(MTAL_IOCTYPE_MTFE_DVBC, 22, INT32)
#define MTAL_IO_DVBC_GETBERParam              _IOW(MTAL_IOCTYPE_MTFE_DVBC, 23, INT32)
#define MTAL_IO_DVBC_GETTIMEOUT               _IOW(MTAL_IOCTYPE_MTFE_DVBT, 24, INT32)


#define MTAL_IO_DTMB_OPEN                     _IOW(MTAL_IOCTYPE_MTFE_DTMB, 0, INT32)
#define MTAL_IO_DTMB_CLOSE                    _IOW(MTAL_IOCTYPE_MTFE_DTMB, 1, INT32)
#define MTAL_IO_DTMB_CONNECT                  _IOW(MTAL_IOCTYPE_MTFE_DTMB, 2, INT32)/*fix me*/
#define MTAL_IO_DTMB_DISCONNECT               _IOW(MTAL_IOCTYPE_MTFE_DTMB, 3, INT32)
#define MTAL_IO_DTMB_GETSYNC                  _IOW(MTAL_IOCTYPE_MTFE_DTMB, 4, INT32)
#define MTAL_IO_DTMB_GETSIGLEVEL              _IOW(MTAL_IOCTYPE_MTFE_DTMB, 5, INT32)
#define MTAL_IO_DTMB_GETPER                   _IOW(MTAL_IOCTYPE_MTFE_DTMB, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DTMB_GETSNR                   _IOW(MTAL_IOCTYPE_MTFE_DTMB, 7, INT32)/*fix me*/
#define MTAL_IO_DTMB_GETIFAGC                 _IOW(MTAL_IOCTYPE_MTFE_DTMB, 8, INT32)
#define MTAL_IO_DTMB_GETTCMBER                _IOW(MTAL_IOCTYPE_MTFE_DTMB, 9, INT32)
#define MTAL_IO_DTMB_GETVER                   _IOW(MTAL_IOCTYPE_MTFE_DTMB, 10, INT32)/*fix me*/
#define MTAL_IO_DTMB_REGCB                    _IOW(MTAL_IOCTYPE_MTFE_DTMB, 11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DTMB_GETTPS                   _IOW(MTAL_IOCTYPE_MTFE_DTMB, 12, INT32)
#define MTAL_IO_DTMB_GETFREQOFFSET            _IOW(MTAL_IOCTYPE_MTFE_DTMB, 13, INT32)
#define MTAL_IO_DTMB_TUNER_CONFIG            _IOW(MTAL_IOCTYPE_MTFE_DTMB, 14, INT32)


#define MTAL_IO_NTSC_OPEN                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 0, INT32)
#define MTAL_IO_NTSC_CLOSE                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 1, INT32)
#define MTAL_IO_NTSC_CONNECT                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 2, INT32)
#define MTAL_IO_NTSC_DISCONNECT                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 3, INT32)
#define MTAL_IO_NTSC_TSF                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 4, INT32)
#define MTAL_IO_NTSC_SSNF                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 5, INT32)
#define MTAL_IO_NTSC_BSNF                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 6, INT32)
#define MTAL_IO_NTSC_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 7, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_NTSC_GETVER                   _IOW(MTAL_IOCTYPE_MTFE_NTSC, 8, INT32)
#define MTAL_IO_NTSC_GETSYNC                _IOW(MTAL_IOCTYPE_MTFE_NTSC, 9, INT32)
#define MTAL_IO_NTSC_GETIFAGC               _IOW(MTAL_IOCTYPE_MTFE_NTSC, 10, INT32)
#define MTAL_IO_NTSC_GETSIGLEVEL            _IOW(MTAL_IOCTYPE_MTFE_NTSC, 11, INT32)


#define MTAL_IO_TUNERHAL_INIT                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 0, INT32)
#define MTAL_IO_TUNERHAL_INITTHREADPROC                  _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 1, INT32)
#define MTAL_IO_TUNERHAL_CONNECT                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 2, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_TUNERHAL_DISCONNECT                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TUNERHAL_SET                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TUNERHAL_SSNF                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 5, INT32)
#define MTAL_IO_TUNERHAL_BSNF                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 6, INT32)
#define MTAL_IO_TUNERHAL_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 7, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_TUNERHAL_TUNERPROCESS                   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 8, INT32)
#define MTAL_IO_TUNERHAL_GET                _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 9, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TUNERHAL_RETRIEVETUNERCFG   _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 10, INT32)
#define MTAL_IO_TUNERHAL_GETCOMPNAME        _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_TUNERHAL_GETTUNERSTATE      _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 12, MTAL_IOCTL_2ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETSIGNAL          _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 13, MTAL_IOCTL_3ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETSIGNALLEVEL     _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 14, MTAL_IOCTL_3ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETSYNC            _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 15, MTAL_IOCTL_3ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETSIGNALLEVELDBM  _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 16, MTAL_IOCTL_3ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETATTRIBUTE       _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 17, MTAL_IOCTL_6ARG_T)    //@weile 2012.1.6
#define MTAL_IO_TUNERHAL_GETATTRIBUTE2      _IOW(MTAL_IOCTYPE_MTFE_TUNERHAL, 18, MTAL_IOCTL_6ARG_T)    //@weile 2012.1.6

#define MTAL_IO_CEC_INIT                   _IOW(MTAL_IOCTYPE_MTCEC, 0, INT32)
#define MTAL_IO_CEC_REGCB                  _IOW(MTAL_IOCTYPE_MTCEC, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_CEC_SETMWREADY             _IOW(MTAL_IOCTYPE_MTCEC, 2, INT32)
#define MTAL_IO_CEC_SETLOGICADDRESS        _IOW(MTAL_IOCTYPE_MTCEC, 3, INT32)
#define MTAL_IO_CEC_SETENABLE              _IOW(MTAL_IOCTYPE_MTCEC, 4, INT32)
#define MTAL_IO_CEC_GETLOGICALADDRESS      _IOW(MTAL_IOCTYPE_MTCEC, 5, INT32)
#define MTAL_IO_CEC_CHECKACTIVESOURCE      _IOW(MTAL_IOCTYPE_MTCEC, 6, INT32)
#define MTAL_IO_CEC_ENQUEUETX              _IOW(MTAL_IOCTYPE_MTCEC, 7, INT32)
#ifdef LG_CEC
#define MTAL_IO_CEC_READ_RX_COMMAND        _IOW(MTAL_IOCTYPE_MTCEC, 8, INT32)
#define MTAL_IO_CEC_READ_TX_RESULT        _IOW(MTAL_IOCTYPE_MTCEC, 9, INT32)
#define MTAL_IO_CEC_READ_RESPONSE        _IOW(MTAL_IOCTYPE_MTCEC, 10, INT32)
#endif
#define MTAL_IO_CEC_SETARCENABLE        _IOW(MTAL_IOCTYPE_MTCEC, 11, INT32)
#define MTAL_IO_CEC_ClearLOGICADDRESS   _IOW(MTAL_IOCTYPE_MTCEC, 12, INT32)
#define MTAL_IO_CEC_SENDINGSTANDBY   	_IOW(MTAL_IOCTYPE_MTCEC, 13, INT32)
#define MTAL_IO_CEC_WAKEUP   			_IOW(MTAL_IOCTYPE_MTCEC, 14, INT32)
#define MTAL_IO_CEC_STANDARDCOMMAND   	_IOW(MTAL_IOCTYPE_MTCEC, 15, INT32)
#define MTAL_IO_CEC_SYSCTRL   	_IOW(MTAL_IOCTYPE_MTCEC, 16, INT32)
#define MTAL_IO_CEC_ISCONNECTES   	_IOW(MTAL_IOCTYPE_MTCEC, 17, INT32)


#define MTAL_IO_PAL_OPEN                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 0, INT32)
#define MTAL_IO_PAL_CLOSE                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 1, INT32)
#define MTAL_IO_PAL_CONNECT                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 2, INT32)
#define MTAL_IO_PAL_DISCONNECT                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 3, INT32)
#define MTAL_IO_PAL_TSF                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 4, INT32)
#define MTAL_IO_PAL_SSNF                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 5, INT32)
#define MTAL_IO_PAL_BSNF                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 6, INT32)
#define MTAL_IO_PAL_REGCB                   _IOW(MTAL_IOCTYPE_MTFE_PAL, 7, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PAL_GETFREQBOUND                _IOW(MTAL_IOCTYPE_MTFE_PAL, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PAL_BypassSIF                _IOW(MTAL_IOCTYPE_MTFE_PAL,9, INT32)
#define MTAL_IO_PAL_NTSC443DET                _IOW(MTAL_IOCTYPE_MTFE_PAL,10, INT32)
#define MTAL_IO_PAL_SUSPEND                _IOW(MTAL_IOCTYPE_MTFE_PAL,11, INT32)
#define MTAL_IO_PAL_RESUME                _IOW(MTAL_IOCTYPE_MTFE_PAL,12, INT32)
#define MTAL_IO_PAL_GETLOCKSTATUS             _IOW(MTAL_IOCTYPE_MTFE_PAL,13, INT32)
#define MTAL_IO_PAL_GETIFAGC                  _IOW(MTAL_IOCTYPE_MTFE_PAL,14, INT32)
#define MTAL_IO_PAL_GETSIGLEVEL               _IOW(MTAL_IOCTYPE_MTFE_PAL,15, INT32)

#define MTAL_IO_ATD_OPEN                  _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 0, INT32)
#define MTAL_IO_ATD_DISCONNECT            _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 1, INT32)
#define MTAL_IO_ATD_ACQUIRE               _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 2, INT32)
#define MTAL_IO_ATD_GETAFTWINSTS          _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 3, INT32)
#define MTAL_IO_ATD_GETCFOKHZ             _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 4, INT32)
#define MTAL_IO_ATD_LOADEQ                _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 5, INT32)
#define MTAL_IO_ATD_SETSYSTEM             _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 6, INT32)
#define MTAL_IO_ATD_SETIF                 _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 7, INT32)
#define MTAL_IO_ATD_SETREG                _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 8, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_ATD_PATCH                 _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 9, INT32)
#define MTAL_IO_ATD_CONNECT               _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 10, INT32)
#define MTAL_IO_ATD_CHECK_ALIVE           _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 11, INT32)
#define MTAL_IO_ATD_SETSNOWMODE           _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 12, INT32)
#define MTAL_IO_ATD_GET_VER               _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 13, INT32)
#define MTAL_IO_ATD_SETA2MMODE            _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 14, INT32)
#define MTAL_IO_ATD_BYPASSSIF             _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 15, INT32)
#define MTAL_IO_ATD_Get_SNR               _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 16, INT32)
#define MTAL_IO_ATD_SETUSMODE             _IOW(MTAL_IOCTYPE_MTFE_UNIATD, 17, INT32)

#define MTAL_IO_SBTVD_OPEN                  _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 0, INT32)
#define MTAL_IO_SBTVD_CLOSE                 _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 1, INT32)
#define MTAL_IO_SBTVD_CONNECT               _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 2, INT32)
#define MTAL_IO_SBTVD_DISCONNECT            _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 3, INT32)
#define MTAL_IO_SBTVD_GETSYNC               _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 4, INT32)
#define MTAL_IO_SBTVD_GETSIGLEVEL           _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 5, INT32)
#define MTAL_IO_SBTVD_GETPER                _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 6, UINT32)
#define MTAL_IO_SBTVD_GETBER                _IOW(MTAL_IOCTYPE_MTFE_SBTVD, 7, UINT32)

#define MTAL_IO_VDO_INIT                        _IOW(MTAL_IOCTYPE_MTVDO, 0, INT32)
#define MTAL_IO_VDO_STOP                        _IOW(MTAL_IOCTYPE_MTVDO, 1, INT32)
#define MTAL_IO_VDO_RESET                       _IOW(MTAL_IOCTYPE_MTVDO, 2, INT32)
#define MTAL_IO_VDO_SET_ENABLE                  _IOW(MTAL_IOCTYPE_MTVDO, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_ENABLE                  _IOW(MTAL_IOCTYPE_MTVDO, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MODE                    _IOW(MTAL_IOCTYPE_MTVDO, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_MODE                    _IOW(MTAL_IOCTYPE_MTVDO, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_FREEZE                  _IOW(MTAL_IOCTYPE_MTVDO, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_FREEZE                  _IOW(MTAL_IOCTYPE_MTVDO, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_ARG                     _IOW(MTAL_IOCTYPE_MTVDO, 9, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_ARG                     _IOW(MTAL_IOCTYPE_MTVDO, 10, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_OVERSCAN                _IOW(MTAL_IOCTYPE_MTVDO, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_OVERSCAN                _IOW(MTAL_IOCTYPE_MTVDO, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_SRCREGION               _IOW(MTAL_IOCTYPE_MTVDO, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_SRCREGION               _IOW(MTAL_IOCTYPE_MTVDO, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_OUTREGION               _IOW(MTAL_IOCTYPE_MTVDO, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_OUTREGION               _IOW(MTAL_IOCTYPE_MTVDO, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_BG                      _IOW(MTAL_IOCTYPE_MTVDO, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_BG                      _IOW(MTAL_IOCTYPE_MTVDO, 18, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_REGCB                       _IOW(MTAL_IOCTYPE_MTVDO, 19, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_SET_GAMEMODE                _IOW(MTAL_IOCTYPE_MTVDO, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3DGAMEMODE              _IOW(MTAL_IOCTYPE_MTVDO, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_UNMUTE                  _IOW(MTAL_IOCTYPE_MTVDO, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_FRAMEBUFFER_AVG         _IOW(MTAL_IOCTYPE_MTVDO, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MJC_ONFF                _IOW(MTAL_IOCTYPE_MTVDO, 29, INT32)
#define MTAL_IO_VDO_SET_MJC_DEMO_MODE           _IOW(MTAL_IOCTYPE_MTVDO, 30, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_MJC_FLIP_MIRROR         _IOW(MTAL_IOCTYPE_MTVDO, 31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MJC_FILM_MODE           _IOW(MTAL_IOCTYPE_MTVDO, 32, INT32)
#define MTAL_IO_VDO_SET_MJC_EFCT_LVL            _IOW(MTAL_IOCTYPE_MTVDO, 33, INT32)
#define MTAL_IO_VDO_SET_DOTBYDOT                _IOW(MTAL_IOCTYPE_MTVDO, 34, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_NONLINEAR               _IOW(MTAL_IOCTYPE_MTVDO, 35, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_TIMINGINFO		        _IOW(MTAL_IOCTYPE_MTVDO, 36, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MJC_RESETSW             _IOW(MTAL_IOCTYPE_MTVDO, 37, INT32)
#define MTAL_IO_VDO_SET_MJC_VIDEO_BLOCK         _IOW(MTAL_IOCTYPE_MTVDO, 38, INT32)
#define MTAL_IO_VDO_SET_MJC_JUDDER_LVL          _IOW(MTAL_IOCTYPE_MTVDO, 39, INT32)
#define MTAL_IO_VDO_SET_MJC_BLUR_LVL            _IOW(MTAL_IOCTYPE_MTVDO, 40, INT32)
#define MTAL_IO_VDO_GET_MJC_FW_VER              _IOW(MTAL_IOCTYPE_MTVDO, 41, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_FACTORY_MODE            _IOW(MTAL_IOCTYPE_MTVDO, 45, INT32)
#define MTAL_IO_VDO_SET_MJC_INITIALIZE          _IOW(MTAL_IOCTYPE_MTVDO, 46, INT32)
#define MTAL_IO_VDO_SET_MJC_BYPASS_WINDOW       _IOW(MTAL_IOCTYPE_MTVDO, 47, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_VDO_SET_MJC_DEMO_BAR            _IOW(MTAL_IOCTYPE_MTVDO, 49, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_SET_MJC_MC_ONOFF            _IOW(MTAL_IOCTYPE_MTVDO, 50, INT32)
#define MTAL_IO_VDO_SET_MJC_MC                  _IOW(MTAL_IOCTYPE_MTVDO, 51, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_FRAMEBUFFER_ONOFF	    _IOW(MTAL_IOCTYPE_MTVDO, 54, INT32)
#define MTAL_IO_VDO_GET_PE_UI_MIN_MAX_DFT       _IOW(MTAL_IOCTYPE_MTVDO, 59, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_AUTOMUTE_ONOFF          _IOW(MTAL_IOCTYPE_MTVDO, 60, INT32)
#define MTAL_IO_VDO_SET_SCALER_PIXELBASED	    _IOW(MTAL_IOCTYPE_MTVDO, 61, INT32)
#define MTAL_IO_VDO_SET_XVYCC_INNER_PTGEN       _IOW(MTAL_IOCTYPE_MTVDO, 62, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_SET_PE_UI_MIN_MAX_DFT       _IOW(MTAL_IOCTYPE_MTVDO, 63, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_SCANMODE                _IOW(MTAL_IOCTYPE_MTVDO, 64, INT32)
#define MTAL_IO_VDO_SET_DS_FACTOR               _IOW(MTAL_IOCTYPE_MTVDO, 65, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_PQ                      _IOW(MTAL_IOCTYPE_MTVDO, 66, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_GET_PQ                      _IOW(MTAL_IOCTYPE_MTVDO, 67, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_GET_3D_TAG                  _IOW(MTAL_IOCTYPE_MTVDO, 68, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3D_PANEL                _IOW(MTAL_IOCTYPE_MTVDO, 69, INT32)
#define MTAL_IO_VDO_SET_3DFP_ONFF               _IOW(MTAL_IOCTYPE_MTVDO, 70, INT32)
#define MTAL_IO_VDO_SET_SRAMDRAMMODE_ONOFF      _IOW(MTAL_IOCTYPE_MTVDO, 71, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_FLIPMIRROR              _IOW(MTAL_IOCTYPE_MTVDO, 72, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_OUTPUTWINDOW			_IOW(MTAL_IOCTYPE_MTVDO, 73, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SRM_SETTVMODE				_IOW(MTAL_IOCTYPE_MTVDO, 74, INT32)
#define MTAL_IO_VDO_GetPlaneWH			        _IOW(MTAL_IOCTYPE_MTVDO, 75, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SETALLMUTE			        _IOW(MTAL_IOCTYPE_MTVDO, 76, INT32)
#define MTAL_IO_VDO_SET_LCDIM_INITIALIZE    	_IOW(MTAL_IOCTYPE_MTVDO, 77, INT32)
#define MTAL_IO_VDO_SET_LCDIM_ONOFF				_IOW(MTAL_IOCTYPE_MTVDO, 78, INT32)
#define MTAL_IO_VDO_SET_LCDIM_LUTIDX			_IOW(MTAL_IOCTYPE_MTVDO, 79, INT32)
#define MTAL_IO_VDO_SET_LCDS_ONOFF				_IOW(MTAL_IOCTYPE_MTVDO, 80, INT32)
#define MTAL_IO_VDO_SET_LCDIM_DEMO				_IOW(MTAL_IOCTYPE_MTVDO, 81, INT32)
#define MTAL_IO_VDO_ENABLE_OD				    _IOW(MTAL_IOCTYPE_MTVDO, 82, INT32)
#define MTAL_IO_VDO_GETFRAMERATE				_IOW(MTAL_IOCTYPE_MTVDO, 83, INT32)
#define MTAL_IO_VDO_GETINPUTFRAMERATE           _IOW(MTAL_IOCTYPE_MTVDO, 84, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_DI_GET_DRAM_PIXELS          _IOW(MTAL_IOCTYPE_MTVDO, 87, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_DI_WRITE_DRAM_PIXELS        _IOW(MTAL_IOCTYPE_MTVDO, 88, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_3D_NAVI_DETECT	        _IOW(MTAL_IOCTYPE_MTVDO, 89, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3D_NAVI_MODE	        _IOW(MTAL_IOCTYPE_MTVDO, 90, INT32)
#define MTAL_IO_VDO_SET_3D_DEPTH_CTRL_ON_OFF	_IOW(MTAL_IOCTYPE_MTVDO, 91, INT32)
#define MTAL_IO_VDO_SET_FBM_MODE                _IOW(MTAL_IOCTYPE_MTVDO, 92, INT32)
#define MTAL_IO_VDO_GET_NET_SWAP_BUFFER         _IOW(MTAL_IOCTYPE_MTVDO, 93, INT32)
#define MTAL_IO_VDO_DI_ENABLE_DUMP_DRAM         _IOW(MTAL_IOCTYPE_MTVDO, 94, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_SBSCONTENT				_IOW(MTAL_IOCTYPE_MTVDO, 95, INT32)
#define MTAL_IO_VDO_GET_ASPECTRATIO             _IOW(MTAL_IOCTYPE_MTVDO, 96, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_FRCMUTE_ONOFF           _IOW(MTAL_IOCTYPE_MTVDO, 97, INT32)
#define MTAL_IO_VDO_GET_3D_LR_ORDER             _IOW(MTAL_IOCTYPE_MTVDO, 98, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3D_AUTO_DISPLAY	        _IOW(MTAL_IOCTYPE_MTVDO, 99, INT32)
#define MTAL_IO_VDO_DI_GET_FORMAT_INFO          _IOW(MTAL_IOCTYPE_MTVDO, 102, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_DI_FREEZE_ONOFF         _IOW(MTAL_IOCTYPE_MTVDO, 104, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_FBM_MODE                _IOW(MTAL_IOCTYPE_MTVDO, 105, INT32)

#define MTAL_IO_VDO_ENABLE_PCID				    _IOW(MTAL_IOCTYPE_MTVDO, 106, INT32)
#define MTAL_IO_VDO_GETFRAMEDELAY			    _IOW(MTAL_IOCTYPE_MTVDO, 107, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3D_NAVI_PARAM	        _IOW(MTAL_IOCTYPE_MTVDO, 108, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_VGA_MODE			    _IOW(MTAL_IOCTYPE_MTVDO, 109, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_HDMI_MODE			    _IOW(MTAL_IOCTYPE_MTVDO, 110, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_BYPASS_PREPROC              _IOW(MTAL_IOCTYPE_MTVDO, 111, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_OD_TABLE	            _IOW(MTAL_IOCTYPE_MTVDO, 112, INT32)
#define MTAL_IO_VDO_SRM_RESUME                  _IOW(MTAL_IOCTYPE_MTVDO, 113, INT32)
#define MTAL_IO_VDO_SET_DOWN_BW_MODE		    _IOW(MTAL_IOCTYPE_MTVDO, 114, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_COND_FORCE_UNMUTE	    _IOW(MTAL_IOCTYPE_MTVDO, 115, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_LBOX_ENABLE				_IOW(MTAL_IOCTYPE_MTVDO, 116, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_LBOX_ENABLE				_IOW(MTAL_IOCTYPE_MTVDO, 117, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_LBOX_CONFIG				_IOW(MTAL_IOCTYPE_MTVDO, 118, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_GET_CAPABILITY			    _IOW(MTAL_IOCTYPE_MTVDO, 119, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_SRM_MODE			    _IOW(MTAL_IOCTYPE_MTVDO, 119, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_UPDATE_CMD_DELAY		    _IOW(MTAL_IOCTYPE_MTVDO, 121, INT32)
#define MTAL_IO_VDO_GET_CMD_DELAY			    _IOW(MTAL_IOCTYPE_MTVDO, 122, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_VIDEO_MUTE			    _IOW(MTAL_IOCTYPE_MTVDO, 123, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_SET_DYNAMIC_SCALE			_IOW(MTAL_IOCTYPE_MTVDO, 124, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_DYNAMIC_SCALE			_IOW(MTAL_IOCTYPE_MTVDO, 125, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_DRAM_INFO				_IOW(MTAL_IOCTYPE_MTVDO, 126, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_SCALER_DISP_MODE		_IOW(MTAL_IOCTYPE_MTVDO, 127, INT32)
//#define MTAL_IO_VDO_GET_MIN_SRC_REGION_WIDTH	_IOW(MTAL_IOCTYPE_MTVDO, 128, INT32)
#define MTAL_IO_VDO_GET_MIRROR_FILP	            _IOW(MTAL_IOCTYPE_MTVDO, 129, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MIRROR_FILP	            _IOW(MTAL_IOCTYPE_MTVDO, 130, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET	                        _IOW(MTAL_IOCTYPE_MTVDO, 131, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_GET	                        _IOW(MTAL_IOCTYPE_MTVDO, 132, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VDO_EXTMJCINIT                  _IOW(MTAL_IOCTYPE_MTVDO, 133, INT32)
#define MTAL_IO_SET_DISPLAY_OFF	                _IOW(MTAL_IOCTYPE_MTVDO, 134, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_VDS_FACTOR	            _IOW(MTAL_IOCTYPE_MTVDO, 135, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_SUB_CLONE_MODE          _IOW(MTAL_IOCTYPE_MTVDO, 136, INT32)
#define MTAL_IO_SET_DISPLAY_OFF_EXT	            _IOW(MTAL_IOCTYPE_MTVDO, 137, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_SPI_COMMAND             _IOW(MTAL_IOCTYPE_MTVDO, 138, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_MONOUT_ENABLE			_IOW(MTAL_IOCTYPE_MTVDO, 139, INT32)
#define MTAL_IO_VDO_GET_MONOUT_ENABLE			_IOW(MTAL_IOCTYPE_MTVDO, 140, INT32)
#define	MTAL_IO_VDO_SET_MONOUT_SOURCE			_IOW(MTAL_IOCTYPE_MTVDO, 141, INT32)
#define	MTAL_IO_VDO_GET_MONOUT_SOURCE			_IOW(MTAL_IOCTYPE_MTVDO, 142, INT32)
#define MTAL_IO_VDO_SET_MONOUT_OUTPUT_WINDOW	_IOW(MTAL_IOCTYPE_MTVDO, 143, INT32)
#define MTAL_IO_VDO_GET_MONOUT_OUTPUT_WINDOW	_IOW(MTAL_IOCTYPE_MTVDO, 144, INT32)
#define MTAL_IO_VDO_SET_MONOUT_DOT_BY_DOT_OUTPUT	_IOW(MTAL_IOCTYPE_MTVDO, 145, INT32)
#define MTAL_IO_VDO_SET_AUTOBLK_ONOFF          _IOW(MTAL_IOCTYPE_MTVDO, 146, INT32)
#define MTAL_IO_VDO_GET_MUTE_MODULE_INFO	    _IOW(MTAL_IOCTYPE_MTVDO, 147, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_BROWSER_INPUT_ENABLE	_IOW(MTAL_IOCTYPE_MTVDO, 148, INT32)
#define MTAL_IO_VDO_SET_SPE_IPTVS_ENABLE		_IOW(MTAL_IOCTYPE_MTVDO, 149, INT32)
#define MTAL_IO_VDO_SET_MONOUT_OSDSEL	        _IOW(MTAL_IOCTYPE_MTVDO, 150, INT32)
#define MTAL_IO_VDO_SET_VIDEO_PIXEL_SHIFT	    _IOW(MTAL_IOCTYPE_MTVDO, 151, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VDO_SET_IMT_FLAG				_IOW(MTAL_IOCTYPE_MTVDO, 152, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_GET_VIDEO_DONE              _IOW(MTAL_IOCTYPE_MTVDO, 153, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_3D_PATTERN_MODE         _IOW(MTAL_IOCTYPE_MTVDO, 153, INT32)
#define MTAL_IO_VDO_SET_3D_PANEL_FORMAT_CHG     _IOW(MTAL_IOCTYPE_MTVDO, 154, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VDO_SET_OSDSEL	                _IOW(MTAL_IOCTYPE_MTVDO, 155, INT32)
#define MTAL_IO_VDO_GET_TIMINGINFO_BY_DEC       _IOW(MTAL_IOCTYPE_MTVDO, 156, MTAL_IOCTL_2ARG_T)



/*********MTAL_IOCTYPE_MTVFE   17*******************/
#define MTAL_IO_VFE_ADCSETTINGRESET	_IOW(MTAL_IOCTYPE_MTVFE, 0, INT32)
#define MTAL_IO_VFE_ADCCALIBRATION	_IOW(MTAL_IOCTYPE_MTVFE, 1, INT32)
#define MTAL_IO_VFE_GETHDADCOFFSET	_IOW(MTAL_IOCTYPE_MTVFE, 2, INT32)
#define MTAL_IO_VFE_GETHDADCGAIN	_IOW(MTAL_IOCTYPE_MTVFE, 3, INT32)
#define MTAL_IO_VFE_SETHDADCOFFSET	_IOW(MTAL_IOCTYPE_MTVFE, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_SETHDADCGAIN	_IOW(MTAL_IOCTYPE_MTVFE, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_GETADCMAXVALUE	_IOW(MTAL_IOCTYPE_MTVFE, 6, INT32)
#define MTAL_IO_VFE_GETBLANKVAL		_IOW(MTAL_IOCTYPE_MTVFE, 7, INT32)
#define MTAL_IO_VFE_SETSYNCSLICELEVEL  _IOW(MTAL_IOCTYPE_MTVFE, 8, INT32)
#define MTAL_IO_VFE_SET_PGAGAIN_OFFSET_INIT_VALUE  _IOW(MTAL_IOCTYPE_MTVFE, 9, INT32)
#define MTAL_IO_VFE_GET_GAIN_STORAGE		_IOW(MTAL_IOCTYPE_MTVFE, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_EBGAIN_GAIN_OVERRULE _IOW(MTAL_IOCTYPE_MTVFE, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_Embedded_GAIN _IOW(MTAL_IOCTYPE_MTVFE, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_GETEBGAIN _IOW(MTAL_IOCTYPE_MTVFE, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VFE_SETHDADCPOW _IOW(MTAL_IOCTYPE_MTVFE, 14, MTAL_IOCTL_2ARG_T)



/*MTAL_IOCTYPE_MTCI 18*/
#define MTAL_IO_CI_INIT                         _IOW(MTAL_IOCTYPE_MTCI, 0, INT32)
#define MTAL_IO_CI_DETECTCARD                  _IOW(MTAL_IOCTYPE_MTCI, 1, INT32)
#define MTAL_IO_CI_RESET                        _IOW(MTAL_IOCTYPE_MTCI, 2, INT32)
#define MTAL_IO_CI_CHECKCIS                     _IOW(MTAL_IOCTYPE_MTCI, 3, INT32)
#define MTAL_IO_CI_WRITECOR                     _IOW(MTAL_IOCTYPE_MTCI, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_BYPASSONOFF                  _IOW(MTAL_IOCTYPE_MTCI, 5, INT32)
#define MTAL_IO_CI_READ                         _IOW(MTAL_IOCTYPE_MTCI, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_WRITE                        _IOW(MTAL_IOCTYPE_MTCI, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_NEGOBUF                      _IOW(MTAL_IOCTYPE_MTCI, 8, INT32)
#define MTAL_IO_CI_READDASTATUS                 _IOW(MTAL_IOCTYPE_MTCI, 9, INT32)
#define MTAL_IO_CI_READREG                      _IOW(MTAL_IOCTYPE_MTCI, 10, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_CI_WRITEREG                     _IOW(MTAL_IOCTYPE_MTCI, 11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_CI_READCOR                      _IOW(MTAL_IOCTYPE_MTCI, 12, INT32)
#define MTAL_IO_CI_TESTCARDREADY                _IOW(MTAL_IOCTYPE_MTCI, 13, INT32)
#define MTAL_IO_CI_ENABLESLOT                   _IOW(MTAL_IOCTYPE_MTCI, 14, INT32)
#define MTAL_IO_CI_DETECTPODACTIVE              _IOW(MTAL_IOCTYPE_MTCI, 15, INT32)
#define MTAL_IO_CI_PCMCIAENABLETS               _IOW(MTAL_IOCTYPE_MTCI, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_RESETCAM                     _IOW(MTAL_IOCTYPE_MTCI, 17, INT32)
#define MTAL_IO_CI_SETCARDDETECTONOFF           _IOW(MTAL_IOCTYPE_MTCI, 18, INT32)
#define MTAL_IO_CI_SETBYPASS                    _IOW(MTAL_IOCTYPE_MTCI, 19, INT32)
#define MTAL_IO_CI_EXTERNALDEMOD                _IOW(MTAL_IOCTYPE_MTCI, 20, INT32)
#define MTAL_IO_CI_INIT0                        _IOW(MTAL_IOCTYPE_MTCI, 21, INT32)
#define MTAL_IO_CI_SETPODCIMODE                 _IOW(MTAL_IOCTYPE_MTCI, 22, INT32)
#define MTAL_IO_CI_PCMCIATOPOD                  _IOW(MTAL_IOCTYPE_MTCI, 23, INT32)
#define MTAL_IO_CI_ENABLEISR                    _IOW(MTAL_IOCTYPE_MTCI, 24, INT32)
#define MTAL_IO_CI_AUTOSETTSPATH                _IOW(MTAL_IOCTYPE_MTCI, 25, INT32)
#define MTAL_IO_CI_CAMWORKAROUND                _IOW(MTAL_IOCTYPE_MTCI, 26, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_SetBypass0                   _IOW(MTAL_IOCTYPE_MTCI, 27, INT32)
#define MTAL_IO_CI_SetBypassTsRouting0          _IOW(MTAL_IOCTYPE_MTCI, 28, INT32)
#define MTAL_IO_CI_RegIsr                       _IOW(MTAL_IOCTYPE_MTCI, 29, INT32)
#define MTAL_IO_CI_SETTSPATH                    _IOW(MTAL_IOCTYPE_MTCI, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_CI_CHECKCIPLUSCAP               _IOW(MTAL_IOCTYPE_MTCI, 31, INT32)
#define MTAL_IO_CI_READCIPLUSVER                _IOW(MTAL_IOCTYPE_MTCI, 32, INT32)
#define MTAL_IO_CI_READCIPLUSPROF               _IOW(MTAL_IOCTYPE_MTCI, 33, UINT32)
#define MTAL_IO_CI_FAST_BOOT_CB                 _IOW(MTAL_IOCTYPE_MTCI, 34, INT32)
#define MTAL_IO_CI_DUALTUNERTSPATH              _IOW(MTAL_IOCTYPE_MTCI, 35, UINT32)
#define MTAL_IO_CI_SETCAMVER                    _IOW(MTAL_IOCTYPE_MTCI, 36, UINT32)
#define MTAL_IO_CI_CLOSE                        _IOW(MTAL_IOCTYPE_MTCI, 37, INT32)
#define MTAL_IO_CI_RESTART                      _IOW(MTAL_IOCTYPE_MTCI, 38, INT32)



/*MTAL_IOCTYPE_MTTVE 20*/
#define MTAL_IO_TVE_INIT                              _IOW(MTAL_IOCTYPE_MTTVE, 0, INT32)
#define MTAL_IO_TVE_CONNECT                      _IOW(MTAL_IOCTYPE_MTTVE, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_DISCONNECT                _IOW(MTAL_IOCTYPE_MTTVE, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_VIDEOSTART                 _IOW(MTAL_IOCTYPE_MTTVE, 3, INT32)
#define MTAL_IO_TVE_VIDEOSTOP                   _IOW(MTAL_IOCTYPE_MTTVE, 4, INT32)
#define MTAL_IO_TVE_VIDEOMUTE                   _IOW(MTAL_IOCTYPE_MTTVE, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_VIDEOFMT                    _IOW(MTAL_IOCTYPE_MTTVE, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_SENDVBIDATA              _IOW(MTAL_IOCTYPE_MTTVE, 7, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TVE_VBISTART                     _IOW(MTAL_IOCTYPE_MTTVE, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_VBISTOP                       _IOW(MTAL_IOCTYPE_MTTVE, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_SETHSCALING                   _IOW(MTAL_IOCTYPE_MTTVE, 10, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_TVE_SETHCENTERCUT                 _IOW(MTAL_IOCTYPE_MTTVE, 11, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TVE_SETLETTERBOX                  _IOW(MTAL_IOCTYPE_MTTVE, 12, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_TVE_GETSCALINGSTATUS              _IOW(MTAL_IOCTYPE_MTTVE, 13, INT32)
#define MTAL_IO_TVE_SETDACMUTEMUTEMASK    _IOW(MTAL_IOCTYPE_MTTVE, 14, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_TVE_SETENABLE    _IOW(MTAL_IOCTYPE_MTTVE, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_SETSCARTOUTCTRL    _IOW(MTAL_IOCTYPE_MTTVE, 16, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_TVE_GETFMT    _IOW(MTAL_IOCTYPE_MTTVE, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_SETOUTPUTFORMAT    _IOW(MTAL_IOCTYPE_MTTVE, 18, INT32)
#define MTAL_IO_TVE_RESET    _IOW(MTAL_IOCTYPE_MTTVE, 19, INT32)
#define MTAL_IO_TVE_SETCGMSAVALUE    _IOW(MTAL_IOCTYPE_MTTVE, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_GETBYPPATH    _IOW(MTAL_IOCTYPE_MTTVE, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_SETSIGCHGNFY    _IOW(MTAL_IOCTYPE_MTTVE, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_TVE_ENABLEBYPASSMONOUT    _IOW(MTAL_IOCTYPE_MTTVE, 23, MTAL_IOCTL_2ARG_T)


#define MTAL_IO_PVR_INIT        _IOW(MTAL_IOCTYPE_MTPVR, 0, INT32)
#define MTAL_IO_PVR_START       _IOW(MTAL_IOCTYPE_MTPVR, 1, INT32)
#define MTAL_IO_PVR_STOP        _IOW(MTAL_IOCTYPE_MTPVR, 2, INT32)
#define MTAL_IO_PVR_GETBUFFER   _IOW(MTAL_IOCTYPE_MTPVR, 3, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_PUTBUFFER   _IOW(MTAL_IOCTYPE_MTPVR, 4, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_GETTHUMBNAIL    _IOW(MTAL_IOCTYPE_MTPVR, 5, MTAL_IOCTL_6ARG_T)
#define MTAL_IO_PVR_STOPTHUMBNAIL   _IOW(MTAL_IOCTYPE_MTPVR, 6, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_SETSPEED   _IOW(MTAL_IOCTYPE_MTPVR, 7, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_RESETBUFFER                   _IOW(MTAL_IOCTYPE_MTPVR, 8, INT32)
#define MTAL_IO_PVR_DELIVER_EOS  _IOW(MTAL_IOCTYPE_MTPVR, 9, INT32)
#define MTAL_IO_PVR_GETBUFFERSTATUS _IO(MTAL_IOCTYPE_MTPVR, 10)
#define MTAL_IO_PVR_PUTDATACHDATA _IOW(MTAL_IOCTYPE_MTPVR, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PVR_ES_START _IOW(MTAL_IOCTYPE_MTPVR, 12,MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_ES_STOP _IOW(MTAL_IOCTYPE_MTPVR, 13, INT32)
#define MTAL_IO_SET_FRAME_SIZE  _IOW(MTAL_IOCTYPE_MTPVR, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PVR_EXTRACT_THUMBNAIL   _IOW(MTAL_IOCTYPE_MTPVR, 15, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_STOP_EXTRACT_THUMBNAIL   _IOW(MTAL_IOCTYPE_MTPVR, 16, INT32)
#define MTAL_IO_PVR_SETMODE         _IOW(MTAL_IOCTYPE_MTPVR, 17, INT32)
#define MTAL_IO_PVR_SET_INST_ID         _IOW(MTAL_IOCTYPE_MTPVR, 18, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_GET_SECURE_BUFFER   _IOW(MTAL_IOCTYPE_MTPVR, 19, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PVR_PUT_SECURE_BUFFER   _IOW(MTAL_IOCTYPE_MTPVR, 20, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_SET_SECURE              _IOW(MTAL_IOCTYPE_MTPVR, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PVR_RESET_MPG_AUDIO_INFO        _IOW(MTAL_IOCTYPE_MTPVR, 22, INT32)


/* MTAL_IOCTYPE_MTDRVCUST */
#define MTAL_IO_DRVCUST_INITQUERY              _IOW(MTAL_IOCTYPE_MTDRVCUST, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_INITGET                _IOW(MTAL_IOCTYPE_MTDRVCUST, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_OPTQUERY               _IOW(MTAL_IOCTYPE_MTDRVCUST, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_OPTGET                 _IOW(MTAL_IOCTYPE_MTDRVCUST, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_CHIPSUPPORT            _IOW(MTAL_IOCTYPE_MTDRVCUST, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_SET_COUNTRY            _IOW(MTAL_IOCTYPE_MTDRVCUST, 5, INT32)
#define MTAL_IO_DRVCUST_GET_COUNTRY            _IOW(MTAL_IOCTYPE_MTDRVCUST, 6, INT32)
#define MTAL_IO_DRVCUST_TUNER_SET              _IOW(MTAL_IOCTYPE_MTDRVCUST, 7, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_TUNER_GET              _IOW(MTAL_IOCTYPE_MTDRVCUST, 8, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_VID_SET                _IOW(MTAL_IOCTYPE_MTDRVCUST, 9, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_VID_GET                _IOW(MTAL_IOCTYPE_MTDRVCUST, 10, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_EXTMJC_SET             _IOW(MTAL_IOCTYPE_MTDRVCUST, 11, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_EXTMJC_GET             _IOW(MTAL_IOCTYPE_MTDRVCUST, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRVCUST_VID_GET_GAMMA          _IOW(MTAL_IOCTYPE_MTDRVCUST, 13, INT32)
#define MTAL_IO_DRVCUST_VID_GET_MIN_MAX        _IOW(MTAL_IOCTYPE_MTDRVCUST, 14, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_SETDRVINIT             _IOW(MTAL_IOCTYPE_MTDRVCUST, 15, INT32)
#define MTAL_IO_DRVCUST_MISC_SET               _IOW(MTAL_IOCTYPE_MTDRVCUST, 16, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_MISC_GET               _IOW(MTAL_IOCTYPE_MTDRVCUST, 17, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRVCUST_SPEC_SET               _IOW(MTAL_IOCTYPE_MTDRVCUST, 18, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRVCUST_SPEC_GET               _IOW(MTAL_IOCTYPE_MTDRVCUST, 19, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRVCUST_MISC_GET_MIN_MAX       _IOW(MTAL_IOCTYPE_MTDRVCUST, 20, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRVCUST_GET_COUNTRY_MASK       _IOW(MTAL_IOCTYPE_MTDRVCUST, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRVCUST_BOOTUPCHECK            _IOW(MTAL_IOCTYPE_MTDRVCUST, 22, INT32)

/* MTAL_IOCTYPE_MTSYS */
#define MTAL_IO_SYS_LEDBLINK                   _IOW(MTAL_IOCTYPE_MTSYS, 0, INT32)
#define MTAL_IO_SYS_SHOWLOGO                   _IOW(MTAL_IOCTYPE_MTSYS, 1, INT32)
#define MTAL_IO_SYS_SRMPAUSE                   _IOW(MTAL_IOCTYPE_MTSYS, 2, INT32)
#define MTAL_IO_SYS_SRMRESUME                  _IOW(MTAL_IOCTYPE_MTSYS, 3, INT32)
#define MTAL_IO_SYS_SRM_SETMMMODE              _IOW(MTAL_IOCTYPE_MTSYS, 4, INT32)
#define MTAL_IO_SYS_SRM_SETTVMODE              _IOW(MTAL_IOCTYPE_MTSYS, 5, INT32)
#define MTAL_IO_SYS_SRM_GETTVMODE              _IOW(MTAL_IOCTYPE_MTSYS, 6, INT32)
#define MTAL_IO_SYS_SRMINIT                    _IOW(MTAL_IOCTYPE_MTSYS, 7, INT32)
#define MTAL_IO_SYS_FBMINIT                    _IOW(MTAL_IOCTYPE_MTSYS, 8, INT32)
#define MTAL_IO_SYS_DBSINIT                    _IOW(MTAL_IOCTYPE_MTSYS, 9, INT32)
#define MTAL_IO_SYS_DCONOFFTEST                _IOW(MTAL_IOCTYPE_MTSYS,10, INT32)
#define MTAL_IO_SYS_DRV_SETTIMESTAMP           _IOW(MTAL_IOCTYPE_MTSYS,11, INT32)
#define MTAL_IO_SYS_JTAG_ENABLE	               _IOW(MTAL_IOCTYPE_MTSYS,12, INT32)

/* MTAL_IOCTYPE_MTSWDMX */
#define MTAL_IO_SWDMX_INIT                      _IOW(MTAL_IOCTYPE_MTSWDMX, 0, INT32)
#define MTAL_IO_SWDMX_SETINFO                   _IOW(MTAL_IOCTYPE_MTSWDMX, 1, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_GETINFO                   _IOW(MTAL_IOCTYPE_MTSWDMX, 2, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_PAUSE                     _IOW(MTAL_IOCTYPE_MTSWDMX, 3, INT32)
#define MTAL_IO_SWDMX_SETCONTAINERTYPE          _IOW(MTAL_IOCTYPE_MTSWDMX, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_SETSPEED                  _IOW(MTAL_IOCTYPE_MTSWDMX, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_PLAY                      _IOW(MTAL_IOCTYPE_MTSWDMX, 6, INT32)
#define MTAL_IO_SWDMX_FLUSH                     _IOW(MTAL_IOCTYPE_MTSWDMX, 7, INT32)
#define MTAL_IO_SWDMX_STOP                      _IOW(MTAL_IOCTYPE_MTSWDMX, 8, INT32)
#define MTAL_IO_SWDMX_LOCKVALIDSRC              _IOW(MTAL_IOCTYPE_MTSWDMX, 9, INT32)
#define MTAL_IO_SWDMX_UNLOCKSRC                 _IOW(MTAL_IOCTYPE_MTSWDMX,10, INT32)
#define MTAL_IO_SWDMX_SEEKTIME                  _IOW(MTAL_IOCTYPE_MTSWDMX,11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_SWDMX_SEEKPOS                   _IOW(MTAL_IOCTYPE_MTSWDMX,12, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_RENDERFROMPOS             _IOW(MTAL_IOCTYPE_MTSWDMX,13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_SEEKOFFSET                _IOW(MTAL_IOCTYPE_MTSWDMX,14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_SETSTRMID                 _IOW(MTAL_IOCTYPE_MTSWDMX,15, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_SWDMX_QUERYRANGEINFO            _IOW(MTAL_IOCTYPE_MTSWDMX,16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_ADDRANGE                  _IOW(MTAL_IOCTYPE_MTSWDMX,17, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_SWDMX_DELRANGE                  _IOW(MTAL_IOCTYPE_MTSWDMX,18, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_SWDMX_SETDMXNFY                 _IOW(MTAL_IOCTYPE_MTSWDMX,19, INT32)
#define MTAL_IO_SWDMX_SETPLAYMODE               _IOW(MTAL_IOCTYPE_MTSWDMX,20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_DELRANGELIST              _IOW(MTAL_IOCTYPE_MTSWDMX,21, INT32)
#define MTAL_IO_SWDMX_WAITFORPAUSE              _IOW(MTAL_IOCTYPE_MTSWDMX,22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_REG_PID_CHG_CB            _IOW(MTAL_IOCTYPE_MTSWDMX,23, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_PID_CHG_DONE              _IOW(MTAL_IOCTYPE_MTSWDMX,24, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_CODEC_CHG_DONE            _IOW(MTAL_IOCTYPE_MTSWDMX,25, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_SET_FLUSHFEEDER            _IOW(MTAL_IOCTYPE_MTSWDMX,26, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_SWDMX_GNRC_STREAM_INFO         _IOW(MTAL_IOCTYPE_MTSWDMX,27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_SET_HDCPKEY               _IOW(MTAL_IOCTYPE_MTSWDMX,30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_ENABLE_HDCPKEY            _IOW(MTAL_IOCTYPE_MTSWDMX,31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_SWDMX_PLAY_ONE_FRM              _IOW(MTAL_IOCTYPE_MTSWDMX, 32, INT32)

/* MTAL_IOCTYPE_MTFEEDER */
#define MTAL_IO_FEEDER_INIT                      _IOW(MTAL_IOCTYPE_MTFEEDER, 0, INT32)
#define MTAL_IO_FEEDER_SETINFO                   _IOW(MTAL_IOCTYPE_MTFEEDER, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_FEEDER_SETDMXNFY                 _IOW(MTAL_IOCTYPE_MTFEEDER, 2, INT32)
#define MTAL_IO_FEEDER_OPEN                      _IOW(MTAL_IOCTYPE_MTFEEDER, 3, INT32)
#define MTAL_IO_FEEDER_REGPULLDATAFNC            _IOW(MTAL_IOCTYPE_MTFEEDER, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_FEEDER_SETFLUSH                  _IOW(MTAL_IOCTYPE_MTFEEDER, 5, INT32)
#define MTAL_IO_FEEDER_START                     _IOW(MTAL_IOCTYPE_MTFEEDER, 6, INT32)
#define MTAL_IO_FEEDER_SETRING                   _IOW(MTAL_IOCTYPE_MTFEEDER, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_FEEDER_SETAPPQRYMODE             _IOW(MTAL_IOCTYPE_MTFEEDER, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_FEEDER_STOP                      _IOW(MTAL_IOCTYPE_MTFEEDER, 9, INT32)
#define MTAL_IO_FEEDER_CLOSE                     _IOW(MTAL_IOCTYPE_MTFEEDER,10, INT32)
#define MTAL_IO_FEEDER_SETBAGTINFO               _IOW(MTAL_IOCTYPE_MTFEEDER,11, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_FEEDER_SETURIMODE                _IOW(MTAL_IOCTYPE_MTFEEDER,12, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_FEEDER_SETSKYPESRCFLG            _IOW(MTAL_IOCTYPE_MTFEEDER,13, INT32)
#define MTAL_IO_FEEDER_GETBUFINFO                _IOW(MTAL_IOCTYPE_MTFEEDER,14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_FEEDER_GETBUFFULLNESS            _IOW(MTAL_IOCTYPE_MTFEEDER,15, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_FEEDER_SETPRELOADDONE            _IOW(MTAL_IOCTYPE_MTFEEDER,16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_FEEDER_NOTIFYHANDLE              _IOW(MTAL_IOCTYPE_MTFEEDER,17, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_FEEDER_SETMMP                    _IOW(MTAL_IOCTYPE_MTFEEDER,18, INT32)
#define MTAL_IO_FEEDER_INITPRELOAD               _IOW(MTAL_IOCTYPE_MTFEEDER,19, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_FEEDER_GETMEMINFO                _IOW(MTAL_IOCTYPE_MTFEEDER,20, INT32)

/* MTAL_IOCTYPE_MTIOMMU */
#define MTAL_IO_MMU_GETPGD                      _IOW(MTAL_IOCTYPE_MTIOMMU, 0, INT32)
#define MTAL_IO_MMU_PINMEM                      _IOW(MTAL_IOCTYPE_MTIOMMU, 1, INT32)
#define MTAL_IO_MMU_UNPINMEM                    _IOW(MTAL_IOCTYPE_MTIOMMU, 2, INT32)
#define MTAL_IO_MMU_MAPSINGLE                   _IOW(MTAL_IOCTYPE_MTIOMMU, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_MMU_UNMAPSINGLE                 _IOW(MTAL_IOCTYPE_MTIOMMU, 4, MTAL_IOCTL_2ARG_T)

/* MTAL_IOCTYPE_MTSTC */
#define MTAL_IO_STC_STOP_STC                    _IOW(MTAL_IOCTYPE_MTSTC, 0, INT32)
#define MTAL_IO_STC_SET_SYNC_MODE               _IOW(MTAL_IOCTYPE_MTSTC, 1, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_STC_SET_STC_VALUE               _IOW(MTAL_IOCTYPE_MTSTC, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_STC_START_STC                   _IOW(MTAL_IOCTYPE_MTSTC, 3, INT32)
#define MTAL_IO_STC_GET_STC_VALUE               _IOW(MTAL_IOCTYPE_MTSTC, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_STC_GET_SRC                     _IOW(MTAL_IOCTYPE_MTSTC, 5, MTAL_IOCTL_2ARG_T)

/*MTAL_IOCTYPE_MTB2R*/
#define MTAL_IO_B2R_GET_PTS                    _IOW(MTAL_IOCTYPE_MTB2R, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_STEP_FORWARD           _IOW(MTAL_IOCTYPE_MTB2R, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_POS_INFO               _IOW(MTAL_IOCTYPE_MTB2R, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_PIC_INFO               _IOW(MTAL_IOCTYPE_MTB2R, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_PLAY_MODE              _IOW(MTAL_IOCTYPE_MTB2R, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_SLOW_FORWARD           _IOW(MTAL_IOCTYPE_MTB2R, 5, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_AB_INFO                _IOW(MTAL_IOCTYPE_MTB2R, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_THUMBNAIL_MODE         _IOW(MTAL_IOCTYPE_MTB2R, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_SEEK                   _IOW(MTAL_IOCTYPE_MTB2R, 8, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_B2R_SET_PAUSE_MM               _IOW(MTAL_IOCTYPE_MTB2R, 9, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_STEP_FORWARD           _IOW(MTAL_IOCTYPE_MTB2R, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_FB_NO_TIMEOUT          _IOW(MTAL_IOCTYPE_MTB2R, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_SPEED                  _IOW(MTAL_IOCTYPE_MTB2R, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_TRICK_MODE             _IOW(MTAL_IOCTYPE_MTB2R, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_INPUT                  _IOW(MTAL_IOCTYPE_MTB2R, 14, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_B2R_SET_SLOW_FORWARD           _IOW(MTAL_IOCTYPE_MTB2R, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_CAPTURE_INFO           _IOW(MTAL_IOCTYPE_MTB2R, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_CB_ATTACH_ID           _IOW(MTAL_IOCTYPE_MTB2R, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SELECT_GROUP_TYPE          _IOW(MTAL_IOCTYPE_MTB2R, 18, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_B2R_GET_EMPTY_FRAME_BUFFER     _IOW(MTAL_IOCTYPE_MTB2R, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_SEQ_HDR                _IOW(MTAL_IOCTYPE_MTB2R, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_DISP_FRAME             _IOW(MTAL_IOCTYPE_MTB2R, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_RELEASE_FBG                _IOW(MTAL_IOCTYPE_MTB2R, 22, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_FREE_FRAME_BUFFER          _IOW(MTAL_IOCTYPE_MTB2R, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_DISP_STABLE            _IOW(MTAL_IOCTYPE_MTB2R, 24, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_GET_CAPTURE_INFO2          _IOW(MTAL_IOCTYPE_MTB2R, 25, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_3D_INFO                _IOW(MTAL_IOCTYPE_MTB2R, 26, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_B2R_SET_CHPTER_ENABLE          _IOW(MTAL_IOCTYPE_MTB2R, 27, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_FLUSH_FRAME                _IOW(MTAL_IOCTYPE_MTB2R, 28, INT32)
#define MTAL_IO_B2R_SET_MIRROR				   _IOW(MTAL_IOCTYPE_MTB2R, 29, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_SYC_MODE               _IOW(MTAL_IOCTYPE_MTB2R, 30, MTAL_IOCTL_5ARG_T)
//#ifdef CC_B2R_RM_SUPPORT
#define MTAL_IO_B2R_RM_CALLBACK                _IOW(MTAL_IOCTYPE_MTB2R, 31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_RM_CONFLICT_DONE           _IOW(MTAL_IOCTYPE_MTB2R, 32, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_SET_GameMode				   _IOW(MTAL_IOCTYPE_MTB2R, 33, MTAL_IOCTL_2ARG_T)

//#endif
#define MTAL_IO_B2R_SET_AUD_TRIGGER	       _IOW(MTAL_IOCTYPE_MTB2R, 34, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_B2R_REG_CB                     _IOW(MTAL_IOCTYPE_MTB2R, 35, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_B2R_SET_SEEK_MODE		   _IOW(MTAL_IOCTYPE_MTB2R, 36, MTAL_IOCTL_2ARG_T)

/*MTAL_IOCTYPE_MTVSS 31*/
#define MTAL_IO_VSS_INIT                       _IOW(MTAL_IOCTYPE_MTVSS, 0, INT32)
#define MTAL_IO_VSS_CONNECT                    _IOW(MTAL_IOCTYPE_MTVSS, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_DISCONNECT                 _IOW(MTAL_IOCTYPE_MTVSS, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_GET_VIDEO_INFO             _IOW(MTAL_IOCTYPE_MTVSS, 3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VSS_GET_PLATFORM_INFO          _IOW(MTAL_IOCTYPE_MTVSS, 4, INT32)
#define MTAL_IO_VSS_GET_FRAME_BUFFER           _IOW(MTAL_IOCTYPE_MTVSS, 5, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VSS_CALLBACK                   _IOW(MTAL_IOCTYPE_MTVSS, 6, INT32)
#define MTAL_IO_VSS_COLOR_FORMAT               _IOW(MTAL_IOCTYPE_MTVSS, 7, INT32)
#define MTAL_IO_VSS_TRIGGER                    _IOW(MTAL_IOCTYPE_MTVSS, 8, INT32)
#define MTAL_IO_VSS_GET_FRAME_IDX              _IOW(MTAL_IOCTYPE_MTVSS, 9, INT32)
#define MTAL_IO_VSS_GET_ADDR_BASE             _IOW(MTAL_IOCTYPE_MTVSS, 10, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_CONNECT_EX                 _IOW(MTAL_IOCTYPE_MTVSS, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_CONNECT_ALL                _IOW(MTAL_IOCTYPE_MTVSS, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_DISCONNECT_EX              _IOW(MTAL_IOCTYPE_MTVSS, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_DISCONNECT_ALL             _IOW(MTAL_IOCTYPE_MTVSS, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_GET_CAPTURE_MODE           _IOW(MTAL_IOCTYPE_MTVSS, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VSS_GET_FRAME_BUFFER_CAP_INFO  _IOW(MTAL_IOCTYPE_MTVSS, 16, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VSS_DUMP_FORMAT                _IOW(MTAL_IOCTYPE_MTVSS, 17, INT32)
#define MTAL_IO_VSS_GET_FRAME_BUFFER_EX        _IOW(MTAL_IOCTYPE_MTVSS, 18, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VSS_GET_CAPTURE_BUFFER         _IOW(MTAL_IOCTYPE_MTVSS, 19, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_VSS_GET_WRITE_END_INT          _IOW(MTAL_IOCTYPE_MTVSS, 20, INT32)
#define MTAL_IO_VSS_CLR_WRITE_END_INT          _IOW(MTAL_IOCTYPE_MTVSS, 21, INT32)
#define MTAL_IO_VSS_SET_FRAME_DROP             _IOW(MTAL_IOCTYPE_MTVSS, 22, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VSS_SET_WRITE_ENABLE           _IOW(MTAL_IOCTYPE_MTVSS, 23, INT32)
#define MTAL_IO_VSS_SET_WRITE_FREEZE           _IOW(MTAL_IOCTYPE_MTVSS, 24, INT32)



/*MTAL_IOCTYPE_MTDRM*/
#define MTAL_IO_DRM_HDCP2_GETCERTINFO         _IOW(MTAL_IOCTYPE_MTDRM, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_DECRYPT_RSAESOAEP    _IOW(MTAL_IOCTYPE_MTDRM, 1, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_KDKEYDERIVATION   _IOW(MTAL_IOCTYPE_MTDRM, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_COMPUTE_HPRIME   _IOW(MTAL_IOCTYPE_MTDRM, 3, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_COMPUTE_LPRIME   _IOW(MTAL_IOCTYPE_MTDRM, 4, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_COMPUTE_KH          _IOW(MTAL_IOCTYPE_MTDRM, 5, INT32)
#define MTAL_IO_DRM_HDCP2_ENCRYPT_KM   _IOW(MTAL_IOCTYPE_MTDRM, 6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_DECRYPT_KM   _IOW(MTAL_IOCTYPE_MTDRM, 7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_DECRYPT_EKS   _IOW(MTAL_IOCTYPE_MTDRM, 8, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_KSLC128XOR   _IOW(MTAL_IOCTYPE_MTDRM, 9, INT32)
#define MTAL_IO_DRM_HDCP2_DECRYPT_PES   _IOW(MTAL_IOCTYPE_MTDRM, 10, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRM_HDCP2_SET_ENC_KEY   _IOW(MTAL_IOCTYPE_MTDRM, 11, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_SIGN_SNAPSHOT_IMG   _IOW(MTAL_IOCTYPE_MTDRM, 12, INT32)
#define MTAL_IO_DRM_HDCP2_SET_ENC_LC128 _IOW(MTAL_IOCTYPE_MTDRM, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_GET_KS    _IOW(MTAL_IOCTYPE_MTDRM, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_SET_RIV_PID   _IOW(MTAL_IOCTYPE_MTDRM, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_PRESET_KS_LC128   _IOW(MTAL_IOCTYPE_MTDRM, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_GENERATE_KM   _IOW(MTAL_IOCTYPE_MTDRM, 17, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_HDCP2_GENERATE_KS   _IOW(MTAL_IOCTYPE_MTDRM, 18, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRM_HDCP2_SET_ENCRYPTEDKM   _IOW(MTAL_IOCTYPE_MTDRM, 19, INT32)
#define MTAL_IO_DRM_HDCP2_GET_ENCRYPTEDKM   _IOW(MTAL_IOCTYPE_MTDRM, 20, INT32)
#define MTAL_IO_DRM_HDCP2_ENCRYPT_PES   _IOW(MTAL_IOCTYPE_MTDRM, 21, MTAL_IOCTL_4ARG_T)


/* for hdcp2.2 */
#define MTAL_IO_DRM_HDCP2_2_KDKEYDERIVATION   _IOW(MTAL_IOCTYPE_MTDRM, 22, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_HDCP2_2_COMPUTE_HPRIME   _IOW(MTAL_IOCTYPE_MTDRM, 23, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_2_COMPUTE_LPRIME   _IOW(MTAL_IOCTYPE_MTDRM, 24, MTAL_IOCTL_2ARG_T)

/* for hdcp2.x align*/
#define MTAL_IO_DRM_HDCP2_SET_ENC_KEY_SET_INTO_TZ _IOW(MTAL_IOCTYPE_MTDRM, 25, MTAL_IOCTL_2ARG_T)

/* for hdcp tx */
#define MTAL_IO_DRM_HDCP2_RetrvEkh_km 				_IOW(MTAL_IOCTYPE_MTDRM, 25, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_HDCP2_EncryptRSAESOAEP 			_IOW(MTAL_IOCTYPE_MTDRM, 26, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_HDCP2_PairingInfo 				_IOW(MTAL_IOCTYPE_MTDRM, 27, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_DRM_HDCP2_EncryptKs 				_IOW(MTAL_IOCTYPE_MTDRM, 28, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_DRM_HDCP2_CheckRxID 				_IOW(MTAL_IOCTYPE_MTDRM, 29, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_ComputeMprime 			_IOW(MTAL_IOCTYPE_MTDRM, 30, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_ComputeVprime 			_IOW(MTAL_IOCTYPE_MTDRM, 31, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_2_ComputeVprime 			_IOW(MTAL_IOCTYPE_MTDRM, 32, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_VerifySignature 			_IOW(MTAL_IOCTYPE_MTDRM, 33, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_GET_GENERATE_KM_FOR_TX  	_IOW(MTAL_IOCTYPE_MTDRM, 34, INT32)
#define MTAL_IO_DRM_HDCP2_Set_Test_Key    	        _IOW(MTAL_IOCTYPE_MTDRM, 35, INT32)
#define MTAL_IO_DRM_HDCP2_ResetPairingInfo  	    _IOW(MTAL_IOCTYPE_MTDRM, 36, MTAL_IOCTL_2ARG_T)


/* for hdcp2.2 extra */
#define MTAL_IO_DRM_HDCP2_Set_TX_CAP    	        _IOW(MTAL_IOCTYPE_MTDRM, 37, INT32)

/* sync from lgwebos*/
#define MTAL_IO_DRM_HDCP2_HMAC_SHA256   	_IOW(MTAL_IOCTYPE_MTDRM, 38, MTAL_IOCTL_2ARG_T)

/* fro android */
#define MTAL_IO_DRM_HDCP2_DataDecrypt		_IOW(MTAL_IOCTYPE_MTDRM, 39, MTAL_IOCTL_2ARG_T)


#define MTAL_IO_DRM_HDCP2_If_TZ_KEY    	        _IOW(MTAL_IOCTYPE_MTDRM, 40, INT32)

#define MTAL_IO_DRM_HDCP2_GETPDKey         		_IOW(MTAL_IOCTYPE_MTDRM, 41, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_HDCP2_SETPDKey         		_IOW(MTAL_IOCTYPE_MTDRM, 42, MTAL_IOCTL_2ARG_T)


//RPMB  
#define MTAL_IO_DRM_RPMB_LOAD_MAC_KEY                     _IOW (MTAL_IOCTYPE_MTDRM, 60, unsigned int)
#define MTAL_IO_DRM_RPMB_GET_WCNT_REQ                     _IOW (MTAL_IOCTYPE_MTDRM, 61, unsigned int)
#define MTAL_IO_DRM_RPMB_SET_WCNT_RESP                    _IOW (MTAL_IOCTYPE_MTDRM, 62, unsigned int)


//DRAM Secure Buffer
#define MTAL_IO_DRM_SECURE_ALLOC                          _IOW(MTAL_IOCTYPE_MTDRM, 70, unsigned int)
#define MTAL_IO_DRM_SECURE_FREE                           _IOW(MTAL_IOCTYPE_MTDRM, 71, unsigned int)
#define MTAL_IO_DRM_SECURE_DATAMOVE                       _IOW(MTAL_IOCTYPE_MTDRM, 72, unsigned int)
#define MTAL_IO_DRM_SECURE_GETDATA                        _IOW(MTAL_IOCTYPE_MTDRM, 73, unsigned int)
#define MTAL_IO_DRM_SECURE_DATAMOVE_DONE                  _IOW(MTAL_IOCTYPE_MTDRM, 74, unsigned int)
#define MTAL_IO_DRM_SECURE_GETDATA_INFO                   _IOW(MTAL_IOCTYPE_MTDRM, 75, unsigned int)

//DRM Widevine
#define MTAL_IO_DRM_WV_OEMCRYPTO_INITIALIZE                _IOW (MTAL_IOCTYPE_MTDRM, 80, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_TERMINATE                 _IOW (MTAL_IOCTYPE_MTDRM, 81, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_OPENSESSION               _IOW (MTAL_IOCTYPE_MTDRM, 82, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_CLOSESESSION              _IOW (MTAL_IOCTYPE_MTDRM, 83, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERATEDERIVEDKEYS       _IOW (MTAL_IOCTYPE_MTDRM, 84, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERATENONCE             _IOW (MTAL_IOCTYPE_MTDRM, 85, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERATESIGNATURE         _IOW (MTAL_IOCTYPE_MTDRM, 86, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_LOADKEYS                  _IOW (MTAL_IOCTYPE_MTDRM, 87, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_REFRESHKEYS               _IOW (MTAL_IOCTYPE_MTDRM, 88, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_SELECTKEY                 _IOW (MTAL_IOCTYPE_MTDRM, 89, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTCTR                _IOW (MTAL_IOCTYPE_MTDRM, 90, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_INSTALLKEYBOX             _IOW (MTAL_IOCTYPE_MTDRM, 91, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_ISKEYBOXVALID             _IOW (MTAL_IOCTYPE_MTDRM, 92, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GETDEVICEID               _IOW (MTAL_IOCTYPE_MTDRM, 93, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GETKEYDATA                _IOW (MTAL_IOCTYPE_MTDRM, 94, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GETRANDOM                 _IOW (MTAL_IOCTYPE_MTDRM, 95, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_WRAPKEYBOX                _IOW (MTAL_IOCTYPE_MTDRM, 96, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_REWRAPDEVICERSAKEY        _IOW (MTAL_IOCTYPE_MTDRM, 97, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_LOADDEVICERSAKEY          _IOW (MTAL_IOCTYPE_MTDRM, 98, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERATERSASIGNATURE      _IOW (MTAL_IOCTYPE_MTDRM, 99, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DERIVEKEYSFROMSESSIONKEY  _IOW (MTAL_IOCTYPE_MTDRM,100, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_ENCRYPT           _IOW (MTAL_IOCTYPE_MTDRM,101, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_DECRYPT           _IOW (MTAL_IOCTYPE_MTDRM,102, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_SIGN              _IOW (MTAL_IOCTYPE_MTDRM,113, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GENERIC_VERIFY            _IOW (MTAL_IOCTYPE_MTDRM,114, unsigned int)

#define MTAL_IO_DRM_WV_OEMCRYPTO_SETENTITLEMENTKEY         _IOW (MTAL_IOCTYPE_MTDRM,115, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DERIVECONTROLWORD         _IOW (MTAL_IOCTYPE_MTDRM,116, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTVIDEO              _IOW (MTAL_IOCTYPE_MTDRM,117, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DECRYPTAUDIO              _IOW (MTAL_IOCTYPE_MTDRM,118, unsigned int)

#define MTAL_IO_DRM_WV_OEMCRYPTO_UPDATEUSAGETABLE          _IOW (MTAL_IOCTYPE_MTDRM,119, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DEACTIVATEUSAGEENTRY      _IOW (MTAL_IOCTYPE_MTDRM,120, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_REPORTUSAGE               _IOW (MTAL_IOCTYPE_MTDRM,121, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DELETEUSAGEENTRY          _IOW (MTAL_IOCTYPE_MTDRM,122, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_DELETEUSAGETABLE          _IOW (MTAL_IOCTYPE_MTDRM,123, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GETUSAGETABLEWRITEREQUEST _IOW (MTAL_IOCTYPE_MTDRM,124, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_VERIFYWRITERESPONSE       _IOW (MTAL_IOCTYPE_MTDRM,125, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_GETUSAGETABLEREADREQUEST  _IOW (MTAL_IOCTYPE_MTDRM,126, unsigned int)
#define MTAL_IO_DRM_WV_OEMCRYPTO_VERIFYREADRESPONSE        _IOW (MTAL_IOCTYPE_MTDRM,127, unsigned int)

/* for sony iptves*/

/* sac */
#define MTAL_IO_DRM_IPTVES_SAC_CONSTRUCT     _IOW(MTAL_IOCTYPE_MTDRM, 130, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SAC_DESTRUCT      _IOW(MTAL_IOCTYPE_MTDRM, 131, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SAC_MAKEMSG       _IOW(MTAL_IOCTYPE_MTDRM, 132, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SAC_PROCESSMSG    _IOW(MTAL_IOCTYPE_MTDRM, 133, MTAL_IOCTL_2ARG_T)

/* sdi */
#define MTAL_IO_DRM_IPTVES_SDI_INIT          			 _IOW(MTAL_IOCTYPE_MTDRM, 134, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_FIN          			 _IOW(MTAL_IOCTYPE_MTDRM, 135, INT32)
#define MTAL_IO_DRM_IPTVES_SDI_CleanupNVS       		 _IOW(MTAL_IOCTYPE_MTDRM, 136, INT32)
#define MTAL_IO_DRM_IPTVES_SDI_GetDeviceID    			 _IOW(MTAL_IOCTYPE_MTDRM, 137, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_VerifyServiceMetaData     _IOW(MTAL_IOCTYPE_MTDRM, 138, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_JudgeCRLUpdate      		 _IOW(MTAL_IOCTYPE_MTDRM, 139, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_GetCRLDstPoint            _IOW(MTAL_IOCTYPE_MTDRM, 140, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_SetCRL                    _IOW(MTAL_IOCTYPE_MTDRM, 141, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_SDI_CheckTrustedTime          _IOW(MTAL_IOCTYPE_MTDRM, 142, MTAL_IOCTL_2ARG_T)


/* ecm */
#define MTAL_IO_DRM_IPTVES_ECM_ExtractScrambleKey       _IOW(MTAL_IOCTYPE_MTDRM, 143, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_ECM_GetScrambleKey    		_IOW(MTAL_IOCTYPE_MTDRM, 144, MTAL_IOCTL_2ARG_T)


/* drm */
#define MTAL_IO_DRM_IPTVES_DRM_ExtractContentKey         _IOW(MTAL_IOCTYPE_MTDRM, 145, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_DelContentKey          	 _IOW(MTAL_IOCTYPE_MTDRM, 146, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_DelUncommittedKey       	 _IOW(MTAL_IOCTYPE_MTDRM, 147, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_GetKeyInfo   			 _IOW(MTAL_IOCTYPE_MTDRM, 148, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_GetKeyNum     			 _IOW(MTAL_IOCTYPE_MTDRM, 149, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_GetKeyRefList      		 _IOW(MTAL_IOCTYPE_MTDRM, 150, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_DRM_GetContentKey             _IOW(MTAL_IOCTYPE_MTDRM, 151, MTAL_IOCTL_2ARG_T)


/* nvs*/

#define MTAL_IO_DRM_IPTVES_NVS_Init             _IOW(MTAL_IOCTYPE_MTDRM, 152, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_NVS_Fin       		_IOW(MTAL_IOCTYPE_MTDRM, 153, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_NVS_GetRWLen       	_IOW(MTAL_IOCTYPE_MTDRM, 154, INT32)


#define MTAL_IO_DRM_IPTVES_time_SetTrustedTime     		 _IOW(MTAL_IOCTYPE_MTDRM, 155, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_time_GetTrustedTime     		 _IOW(MTAL_IOCTYPE_MTDRM, 156, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_DRM_IPTVES_time_Cleanup     		     _IOW(MTAL_IOCTYPE_MTDRM, 157, MTAL_IOCTL_2ARG_T)






#define MTAL_IO_MTRM_DIRECT_CALL                 _IOW(MTAL_IOCTYPE_MTRM, 0, INT32)

/* MTAL_IOCTYPE_MTVENC                      41 */
#define MTAL_IO_VENC_INIT                   _IOW(MTAL_IOCTYPE_MTVENC,  0, INT32)
#define MTAL_IO_VENC_TERMINT                _IOW(MTAL_IOCTYPE_MTVENC,  1, INT32)
#define MTAL_IO_VENC_RESET                  _IOW(MTAL_IOCTYPE_MTVENC,  2, INT32)
#define MTAL_IO_VENC_SET                    _IOW(MTAL_IOCTYPE_MTVENC,  3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VENC_GET                    _IOW(MTAL_IOCTYPE_MTVENC,  4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VENC_OPEN                   _IOW(MTAL_IOCTYPE_MTVENC,  5, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VENC_SET_PARAM              _IOW(MTAL_IOCTYPE_MTVENC,  6, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_SET_CTRL               _IOW(MTAL_IOCTYPE_MTVENC,  7, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_DRV_INIT               _IOW(MTAL_IOCTYPE_MTVENC,  8, INT32)
#define MTAL_IO_VENC_RUN                    _IOW(MTAL_IOCTYPE_MTVENC,  9, INT32)
#define MTAL_IO_VENC_STOP                   _IOW(MTAL_IOCTYPE_MTVENC, 10, INT32)
#define MTAL_IO_VENC_CLOSE                  _IOW(MTAL_IOCTYPE_MTVENC, 11, INT32)
#define MTAL_IO_VENC_ENC_FRAME_SYNC         _IOW(MTAL_IOCTYPE_MTVENC, 12, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_ENC_FRAME_ASYNC        _IOW(MTAL_IOCTYPE_MTVENC, 13, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VENC_CREATE_BUFFMGR         _IOW(MTAL_IOCTYPE_MTVENC, 14, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VENC_DELETE_BUFFMGR         _IOW(MTAL_IOCTYPE_MTVENC, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VENC_BUFFMGR_OBJ_GET        _IOW(MTAL_IOCTYPE_MTVENC, 16, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_BUFFMGR_OBJ_FREE       _IOW(MTAL_IOCTYPE_MTVENC, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_VENC_FLUSH_FB               _IOW(MTAL_IOCTYPE_MTVENC, 18, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_INSERT_KEYFRM          _IOW(MTAL_IOCTYPE_MTVENC, 19, INT32)
#define MTAL_IO_VENC_REG_CB                 _IOW(MTAL_IOCTYPE_MTVENC, 20, INT32)
#define MTAL_IO_VENC_UNLOCK_BUF             _IOW(MTAL_IOCTYPE_MTVENC, 21, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_VENC_COPYDATA               _IOW(MTAL_IOCTYPE_MTVENC, 25, MTAL_IOCTL_8ARG_T)

#define MTAL_IO_VENC_INIT_V					_IOW(MTAL_IOCTYPE_MTVENC, 26, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_VENC_GET_CAP_INFO	  	    _IOW(MTAL_IOCTYPE_MTVENC, 27, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_VENC_START_CAPTURE	  	    _IOW(MTAL_IOCTYPE_MTVENC, 28, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_VENC_STOP_CAPTURE	  	    _IOW(MTAL_IOCTYPE_MTVENC, 29, INT32)
#define MTAL_IO_VSS_GET_FRAME_BUFFER_420    _IOW(MTAL_IOCTYPE_MTVENC, 30, INT32)
#define MTAL_IO_VSS_TRIGGER_NEXT_FRAME_420  _IOW(MTAL_IOCTYPE_MTVENC, 31, INT32)



/* MTAL_IOCTYPE_MTMX                        42 */
#define MTAL_IO_MX_INIT                     _IOW(MTAL_IOCTYPE_MTMX, 0, INT32)
#define MTAL_IO_MX_TERMINT                  _IOW(MTAL_IOCTYPE_MTMX, 1, INT32)
#define MTAL_IO_MX_RESET                    _IOW(MTAL_IOCTYPE_MTMX, 2, INT32)
#define MTAL_IO_MX_SET                      _IOW(MTAL_IOCTYPE_MTMX, 3, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MX_GET                      _IOW(MTAL_IOCTYPE_MTMX, 4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MX_SET_CODEC                _IOW(MTAL_IOCTYPE_MTMX, 5, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_MX_COPY_DATA                _IOW(MTAL_IOCTYPE_MTMX, 6, MTAL_IOCTL_4ARG_T)


/*MTAL_IOCTYPE_MTTZ                         43 */
#define MTAL_IO_TZ_COMMON_OPERATE               _IOWR(MTAL_IOCTYPE_MTTZ, 0, MTTZ_COMMON_ARG_T)
#define MTAL_IO_TZ_DECRYPT_INIT                 _IOWR(MTAL_IOCTYPE_MTTZ, 1, MTTZ_COMMON_ARG_T)
#define MTAL_IO_TZ_DECRYPT_OPERATE              _IOWR(MTAL_IOCTYPE_MTTZ, 2, MTTZ_COMMON_ARG_T)
#define MTAL_IO_TZ_DECRYPT_DEINIT               _IOWR(MTAL_IOCTYPE_MTTZ, 3, MTTZ_COMMON_ARG_T)
#define MTAL_IO_TZ_MSL_OPERATE                  _IOWR(MTAL_IOCTYPE_MTTZ, 4, MTTZ_COMMON_ARG_T)
#define MTAL_IO_TZ_DTCP_OPERATE                 _IOWR(MTAL_IOCTYPE_MTTZ, 5, MTTZ_COMMON_ARG_T)


/*MTAL_IOCTYPE_MTPHOTO4K                       44*/
#define MTAL_IO_PHOTO4K_INIT                    _IOW(MTAL_IOCTYPE_MTPHOTO4K, 0, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PHOTO4K_SET_DISPLAY_INFO        _IOW(MTAL_IOCTYPE_MTPHOTO4K, 1, MTAL_IOCTL_8ARG_T)
#define MTAL_IO_PHOTO4K_SHOW                    _IOW(MTAL_IOCTYPE_MTPHOTO4K, 2, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PHOTO4K_DEINIT                  _IOW(MTAL_IOCTYPE_MTPHOTO4K, 3, INT32)
#define MTAL_IO_PHOTO4K_START_FADE_EFFECT       _IOW(MTAL_IOCTYPE_MTPHOTO4K, 4, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_PHOTO4K_PREPARE                 _IOW(MTAL_IOCTYPE_MTPHOTO4K, 5, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_PHOTO4K_PAUSE_FADE_EFFECT       _IOW(MTAL_IOCTYPE_MTPHOTO4K, 6, INT32)
#define MTAL_IO_PHOTO4K_START_PANORAMA_SCROLL   _IOW(MTAL_IOCTYPE_MTPHOTO4K, 7, MTAL_IOCTL_9ARG_T)
#define MTAL_IO_PHOTO4K_PAUSE_PANORAMA_SCROLL   _IOW(MTAL_IOCTYPE_MTPHOTO4K, 8, INT32)
#define MTAL_IO_PHOTO4K_RESTART_FADE_EFFECT     _IOW(MTAL_IOCTYPE_MTPHOTO4K, 9, INT32)
#define MTAL_IO_PHOTO4K_RESTART_PANORAMA_SCROLL _IOW(MTAL_IOCTYPE_MTPHOTO4K, 10, INT32)
#define MTAL_IO_PHOTO4K_VDP_ALLOCATE            _IOW(MTAL_IOCTYPE_MTPHOTO4K, 11, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_PHOTO4K_VDP_DEALLOCATE          _IOW(MTAL_IOCTYPE_MTPHOTO4K, 12, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_PHOTO4K_VDP_GETINFO             _IOW(MTAL_IOCTYPE_MTPHOTO4K, 13, MTAL_IOCTL_7ARG_T)
#define MTAL_IO_PHOTO4K_VDP_SETINFO             _IOW(MTAL_IOCTYPE_MTPHOTO4K, 14, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_PHOTO4K_IS_4K_PANEL             _IOW(MTAL_IOCTYPE_MTPHOTO4K, 15, MTAL_IOCTL_2ARG_T)

#define MTAL_IO_MHL_INIT                   _IOW(MTAL_IOCTYPE_MTMHL, 0, INT32)
#define MTAL_IO_MHL_REGCB                  _IOW(MTAL_IOCTYPE_MTMHL, 1, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_MHL_SENDMSCCOMMAND              _IOW(MTAL_IOCTYPE_MTMHL, 2, INT32)
#define MTAL_IO_MHL_SENDMSCRATCHPAD              _IOW(MTAL_IOCTYPE_MTMHL, 3, INT32)
#define MTAL_IO_MHL_GET_PEER_MHL_VERSION              _IOW(MTAL_IOCTYPE_MTMHL, 4, INT32)
#define MTAL_IO_MHL_SET_OPTION              _IOW(MTAL_IOCTYPE_MTMHL, 5, INT32)
#define MHL_IOCTL_GET_OVER_CURRENT_STA            _IOW(MTAL_IOCTYPE_MTMHL, 6, INT32)

/*MTAL_IOCTYPE_MTFE_TUNER*/
#define MTAL_IO_TUNER_INIT        _IOW(MTAL_IOCTYPE_MTFE_TUNER, 0, INT32)
#define MTAL_IO_TUNER_SETFREQ    _IOW(MTAL_IOCTYPE_MTFE_TUNER, 1, INT32)
#define MTAL_IO_TUNER_OP         _IOW(MTAL_IOCTYPE_MTFE_TUNER, 2, INT32)
#define MTAL_IO_AMB_SETMODE								  _IOW(MTAL_IOCTYPE_MTAMB,  0, INT32)
#define MTAL_IO_AMB_GETMODE								  _IOW(MTAL_IOCTYPE_MTAMB,  1, INT32)
#define MTAL_IO_AMB_SETMUTE								  _IOW(MTAL_IOCTYPE_MTAMB,  2, INT32)
#define MTAL_IO_AMB_GETMUTE                               _IOW(MTAL_IOCTYPE_MTAMB,  3, INT32)
#define MTAL_IO_AMB_SETLOUNGEMOOD                         _IOW(MTAL_IOCTYPE_MTAMB,  4, INT32)
#define MTAL_IO_AMB_GETLOUNGEMOOD                         _IOW(MTAL_IOCTYPE_MTAMB,  5, INT32)
#define MTAL_IO_AMB_SETFEATURELEVEL                       _IOW(MTAL_IOCTYPE_MTAMB,  6, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETFEATURELEVEL                       _IOW(MTAL_IOCTYPE_MTAMB,  7, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_SETWALLCOLOR                          _IOW(MTAL_IOCTYPE_MTAMB,  8, INT32)
#define MTAL_IO_AMB_GETWALLCOLOR                          _IOW(MTAL_IOCTYPE_MTAMB,  9, INT32)
#define MTAL_IO_AMB_SETSTATICCOLORRGB                     _IOW(MTAL_IOCTYPE_MTAMB, 10, INT32)
#define MTAL_IO_AMB_GETSTATICCOLORRGB                     _IOW(MTAL_IOCTYPE_MTAMB, 11, INT32)
#define MTAL_IO_AMB_SETSUNSETDURATION                     _IOW(MTAL_IOCTYPE_MTAMB, 12, INT32)
#define MTAL_IO_AMB_GETSUNSETDURATION                     _IOW(MTAL_IOCTYPE_MTAMB, 13, INT32)
#define MTAL_IO_AMB_GETCONFIGURATION                      _IOW(MTAL_IOCTYPE_MTAMB, 14, MTAL_IOCTL_5ARG_T)
#define MTAL_IO_AMB_SETPIXEL                              _IOW(MTAL_IOCTYPE_MTAMB, 15, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETPIXEL                              _IOW(MTAL_IOCTYPE_MTAMB, 16, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_SETPIXELARRAY                         _IOW(MTAL_IOCTYPE_MTAMB, 17, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETPIXELARRAY                         _IOW(MTAL_IOCTYPE_MTAMB, 18, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETMEASUREDPIXELARRAY                 _IOW(MTAL_IOCTYPE_MTAMB, 19, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETPROCESSEDPIXELARRAY                _IOW(MTAL_IOCTYPE_MTAMB, 20, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETNBROFPIXELSPERBOARD                _IOW(MTAL_IOCTYPE_MTAMB, 21, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_SETFACTORYMATRIXBOARD                 _IOW(MTAL_IOCTYPE_MTAMB, 22, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AMB_GETFACTORYMATRIXBOARD                 _IOW(MTAL_IOCTYPE_MTAMB, 23, MTAL_IOCTL_4ARG_T)
#define MTAL_IO_AMB_SETFACTORYBRIGHTNESSBOARD             _IOW(MTAL_IOCTYPE_MTAMB, 24, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AMB_GETFACTORYBRIGHTNESSBOARD             _IOW(MTAL_IOCTYPE_MTAMB, 25, MTAL_IOCTL_3ARG_T)
#define MTAL_IO_AMB_SAVESETTINGS                          _IOW(MTAL_IOCTYPE_MTAMB, 26, INT32)
#define MTAL_IO_AMB_CHECKSETTINGSSAVED                    _IOW(MTAL_IOCTYPE_MTAMB, 27, INT32)
#define MTAL_IO_AMB_GETFACTORYNBROFLEDSPERBOARD           _IOW(MTAL_IOCTYPE_MTAMB, 28, MTAL_IOCTL_2ARG_T)
#define MTAL_IO_AMB_GETFACTORYLEDHWOPTION                 _IOW(MTAL_IOCTYPE_MTAMB, 29, INT32)
#define MTAL_IO_AMB_INIT								  _IOW(MTAL_IOCTYPE_MTAMB, 30, INT32)

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Prototype  of public functions
//-----------------------------------------------------------------------------
#ifdef __KERNEL__
extern int mtal_ioctl_mtadac(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtauddec(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtdmx(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_atsc(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_dvbt(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_dvbc(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_dtmb(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_ntsc(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_tunerhal(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_pal(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_sbtvd(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_atd(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtgfx(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtmplayer(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtimage(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtosd(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtperipheral(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtpmx(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtscart(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvdo(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvfe(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtci(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvdec(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvdecex(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvbiex(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);
extern int mtal_ioctl_mttve(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);

extern int mtal_ioctl_mtdrvcust(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);

extern int mtal_ioctl_mtsys(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);

extern int mtal_ioctl_mtswdmx(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfeeder(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtb2r(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtstc(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtpvr(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtmmu(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtvss(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtdrm(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtrm(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);

extern int mtal_ioctl_mtcec(struct inode *inode, struct file *file, unsigned int cmd,
              unsigned long arg);

extern int mtal_ioctl_mtvenc(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtmx(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mttz(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtphoto4k(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtfe_tuner(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);
extern int mtal_ioctl_mtmhl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);

extern int mtal_ioctl_mtamb(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg);
#endif  // #ifdef  __kernel__

#ifdef __cplusplus
}
#endif

#endif  // MTAL_IOCTL_H

