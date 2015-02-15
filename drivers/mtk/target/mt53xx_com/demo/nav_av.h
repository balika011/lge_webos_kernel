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
 * $RCSfile: nav_av.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget.h
 *  This header file declares public function prototypes of widget.
 */

#ifndef NAV_AV_H
#define NAV_AV_H

#include "x_os.h"

#define NAV_MODECHANGE_SEMA_LOCK VERIFY(x_sema_lock(hModeChangeSema, X_SEMA_OPTION_WAIT)==OSR_OK);
#define NAV_MODECHANGE_SEMA_UNLOCK VERIFY(x_sema_unlock(hModeChangeSema)==OSR_OK);
/*
#define NAV_AV_SEMA_LOCK x_sema_lock(hPlayAVSema, X_SEMA_OPTION_WAIT);
#define NAV_AV_SEMA_UNLOCK x_sema_unlock(hPlayAVSema);
*/


#define NAV_DTV_PROG_MAX_NUM 128
#define NAV_ATV_PROG_MAX_NUM 128
#define NAV_TS_MAX_NUM 32
#define NAV_PMT_MAX_NUM 64
#define NAV_PSI_TIMEOUT 2000
#define NAV_NIM_TIMEOUT 5000

#define NAV_DTV_SYSTEM_ATSC 0
#define NAV_DTV_SYSTEM_CABLE 1
#define NAV_DTV_SYSTEM_DVBT_UK 2
#define NAV_DTV_SYSTEM_DVBT_TW 3
#define NAV_DTV_SYSTEM_ISDBT 4
#define NAV_DTV_SYSTEM_DVBC 5

#define NAV_ATV_SYSTEM_CABLE 0
#define NAV_ATV_SYSTEM_AIR 1
#define NAV_ATV_SYSTEM_PAL_BG 2
#define NAV_ATV_SYSTEM_PAL_DK 3
#define NAV_ATV_SYSTEM_PAL_I 4
#define NAV_ATV_SYSTEM_SECAM_L 5
#define NAV_ATV_SYSTEM_SECAM_L1 6
#define NAV_ATV_SYSTEM_NTSC_M 7

#define NAV_VIDEOPATH_MAX_NUM 2

#define NAV_VDP_MASTER 0
#define NAV_VDP_SLAVE 1

#define NAV_OSD_MSGQ "OSDMSGQ"
#define NAV_ATVCB_MSGQ "ATVMSGQ"
#define NAV_ATVAUD_MSGQ "AUDMSGQ"
#define NAV_AUDCTRL_MSGQ "ACTLMSGQ"

#define STREAM_TYPE_VIDEO		2
#define STREAM_TYPE_MPEG1		3
#define STREAM_TYPE_MPEG2		4
#define STREAM_TYPE_PRIVATE		6
#define STREAM_TYPE_AVS         0x42
#define STREAM_TYPE_H264		0x1B
#define STREAM_TYPE_AC3_EU		0xBD
#define STREAM_TYPE_AC3			0x81
#define STREAM_TYPE_AAC			0x11
#define STREAM_TYPE_AAC_BRZ 	0x0F
#define STREAM_TYPE_CYPHER		0x80
#define STREAM_TYPE_UNKNOWN		0xFF

#define NAV_ATD_START_FREQ      (45750000)
#define NAV_ATD_END_FREQ        (863250000)
#define NAV_ATD_FREQ_SMALL_STEP (1500000)
#define NAV_ATD_FREQ_MIDDLE_STEP (2000000)
#define NAV_ATD_FREQ_NEXT_STEP  (8000000)


typedef enum _E_NAV_VS
{
	E_NAV_VS_TUNER1,
	E_NAV_VS_CVBS1,
	E_NAV_VS_CVBS2,
	E_NAV_VS_CVBS3,		
	E_NAV_VS_CVBS4,			
	E_NAV_VS_S1,
	E_NAV_VS_S2,
	E_NAV_VS_S3,
	E_NAV_VS_YPbPr1,
	E_NAV_VS_YPbPr2,
	E_NAV_VS_YPbPr3,
       E_NAV_VS_YPbPr4,  	
	E_NAV_VS_VGA,       
	E_NAV_VS_HDMI,
	E_NAV_VS_HDMI2,
       E_NAV_VS_HDMI3,
       E_NAV_VS_HDMI4,
       E_NAV_VS_HDMI5,       
	E_NAV_VS_DVI,
	// add for 5371
	E_NAV_VS_DT1, // digital tuner
	E_NAV_VS_DT2,
       E_NAV_VS_SCART,     
       E_NAV_VS_SCART2,         
	E_NAV_VS_MAX,

} E_NAV_VS;

typedef enum _E_NAV_VD
{
    E_NAV_VD_TUNER,
    E_NAV_VD_TVD,
    E_NAV_VD_HDTV,    	
    E_NAV_VD_HDMI,    	
    E_NAV_VD_MPEG,        
    E_NAV_VD_I1394,        
    E_NAV_VD_SCART,
    E_NAV_VD_MAX
} E_NAV_VD;

typedef enum _E_NAV_MODE
{
    E_NAV_MODE_SINGLE,	
    E_NAV_MODE_PIP,	
    E_NAV_MODE_POP
} E_NAV_MODE;

typedef struct _NAV_ATV_TBL_PROG_T {    
    UINT16 u2FreqTabIdx;
    UINT32              u4TvSys;
    UINT32              u4SoundSys;  
    UINT32 u4Freq;
} NAV_ATV_TBL_PROG_T;

typedef struct _NAV_DTV_PROG_INFO {
    UINT32 u4VctTblIdx;
    UINT32 u4TsTblIdx;
    UINT32 u4EsId;
    UINT32 u4Pidx;
    UINT32 u4APidx;
    UINT32 u4ResX;
    UINT32 u4ResY;
    BOOL fgProgressive;
    BOOL fgValid;
    UINT32 u4Scrambled;
} NAV_DTV_PROG_INFO;

typedef struct _NAV_ATV_PROG_INFO {
    UINT32 u4ATVTblIdx;
    UINT32 u4ResX;
    UINT32 u4ResY;
    BOOL fgProgressive;
    BOOL fgValid;
} NAV_ATV_PROG_INFO;

typedef struct _NAV_OSD_MSG_T {
    // Video path information
    NAV_VIDEOPATH_T rVideoPath;
    // Volume
    UINT32 u4Volume;
    // Freeze and mute option
    BOOL fgMute;
    BOOL fgFreeze;
    // VCT/ATV Table idx
    UINT32 u4VctTblIdx;
    UINT32 u4ATVTblIdx;
    BOOL fgChannelChange;
    UINT16 u2Major;
    UINT16 u2Minor;    
} NAV_OSD_MSG_T;

typedef struct _NAV_ATVCB_MSG_T {
    UINT32 u4VdpId;
    UINT32 u4SigStatus;
    UINT32 u4Src;
} NAV_ATVCB_MSG_T;

typedef struct _NAV_ATVAUD_MSG_T {
    UINT32              u4TvSys;
    UINT32              u4SoundSys;
} NAV_ATVAUD_MSG_T;

EXTERN NAV_VIDEOPATH_T arVideoPath[NAV_VIDEOPATH_MAX_NUM];
EXTERN NAV_DTV_PROG_INFO rCurrDTVProg;
EXTERN NAV_ATV_PROG_INFO rCurrATVProg;
EXTERN UINT32 ePIPMode;
EXTERN UINT32 u4ActiveVDP;
EXTERN UINT32 u4NumDTVProg;
EXTERN UINT32 u4NumATVProg;
// For protecting Audio/Video play/Stop
EXTERN HANDLE_T hModeChangeSema;
EXTERN HANDLE_T hPlayAVSema;
EXTERN NAV_ATV_TBL_PROG_T arATVTbl[];

EXTERN BOOL _fgAutoSearch;

EXTERN INT32 NAV_InitAV(void);
/* video related */
EXTERN INT32 NAV_SetVideoHScale(VOID* pvArg);
EXTERN INT32 NAV_SetVideoVScale(VOID* pvArg);
EXTERN INT32 NAV_SetVideoHPos(VOID* pvArg);
EXTERN INT32 NAV_SetVideoVPos(VOID* pvArg);
	
EXTERN INT32 NAV_ScanDTVChannel(UINT32 u4Src);
EXTERN INT32 NAV_ScanATVChannel(UINT32 u4Src);

EXTERN INT32 NAV_SetMode(UINT32 u4Mode);
EXTERN INT32 NAV_SetProg(UINT32 u4VdpId, UINT32 u4ProgNum);
EXTERN INT32 NAV_SetSrc(UINT32 u4VdpId, UINT32 u4NewSrc);

EXTERN INT32 NAV_DrawInfoOSD(void);
EXTERN INT32 NAV_ClearInfoOSD(void);


EXTERN INT32 NAV_ShowBorder(UINT32 u4VdpId);
EXTERN INT32 NAV_ClearBorder(UINT32 u4VdpId);
EXTERN BOOL NAV_GetBorderStatus(void);
EXTERN void NAV_InitBorder(void);
EXTERN void NAV_ReleaseBorder(void);

EXTERN INT32 NAV_SetSrcActive(VOID *pvArg);
EXTERN INT32 NAV_SetupInputSourceItem(void);
EXTERN INT32 NAV_SetDTVSystem(void* pvArg);
EXTERN INT32 NAV_SetATVSystem(void* pvArg);
EXTERN INT32 NAV_SetDTVChannelRange(UINT32 u4ManualScan, UINT32 u4ChStart, UINT32 u4ChEnd);
EXTERN INT32 SwitchDTVMTS(UINT32 u4VdpId);

EXTERN INT32 NAV_SetFreeze(UINT32 u4VdpId, BOOL fgFreeze);
EXTERN BOOL NAV_GetFreeze(UINT32 u4VdpId);
EXTERN INT32 NAV_StopScan(void);
EXTERN INT32 NAV_SetHDMI(void* pvArg);
EXTERN INT32 NAV_SwapSrc(void);
EXTERN void NAV_LockAVMutex(UINT32 u4VdpId);
EXTERN void NAV_UnlockAVMutex(UINT32 u4VdpId);
EXTERN void NAV_OpenNIM(void);
EXTERN INT32 NAV_SetScart(void* pvArg);
EXTERN UINT32 NAV_GetNumATVProg(void);
EXTERN UINT32 NAV_GetNumDTVProg(void);
EXTERN BOOL NAV_IsATVScanning(void);
EXTERN BOOL NAV_IsDTVScanning(void);
EXTERN UINT32 NAV_GetATVProgNum(UINT32 u4ProgIdx);

EXTERN WIDGET_FONT_PROPERTY_T* NAV_GetDefaultFont(void);
EXTERN UINT32 NAV_LookupSourceIndexFromMKVSRC(DRV_INPUT_SRC_T rInput);
EXTERN void NAV_MeasureTime(void);
#endif
