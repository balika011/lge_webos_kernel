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
 * $RCSfile: nav_if.h,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file widget.h
 *  This header file declares public function prototypes of widget.
 */

#ifndef NAV_IF_H
#define NAV_IF_H

#include "x_drv_if.h"


EXTERN HANDLE_T hModeChangeSema;
#define NAV_MODECHANGE_SEMA_LOCK VERIFY(x_sema_lock(hModeChangeSema, X_SEMA_OPTION_WAIT)==OSR_OK);
#define NAV_MODECHANGE_SEMA_UNLOCK VERIFY(x_sema_unlock(hModeChangeSema)==OSR_OK);

typedef struct _NAV_OPTION_T {
	// Input src enable list
	BOOL fgEnableATV;
	BOOL fgEnableCVBS1;
	BOOL fgEnableCVBS2;
	BOOL fgEnableCVBS3;
	BOOL fgEnableS1;
	BOOL fgEnableS2;
	BOOL fgEnableS3;
	BOOL fgEnableYPbPr1;
	BOOL fgEnableYPbPr2;
	BOOL fgEnableHDMI;
       BOOL fgEnableHDMI2;
	BOOL fgEnableVGA;

	BOOL fgEnableDTV;
	BOOL fgEnableI1394;

	// Audio enable
	BOOL fgEnableAudio; 

	// OSD configuration
	BOOL fgEnableOSD32;

	
	BOOL fgDTVFullScan;

       UINT32 u4PanelId;
	
} NAV_OPTION_T; 

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
	E_NAV_VS_YPbPR3,
       E_NAV_VS_YPbPR4,  	
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

typedef struct _NAV_VIDEOPATH_T {
	UINT32 u4Src;
	DRV_VDO_WINDOW_T rVDPRegion;
	UINT32 u4Volume;
	UINT32 u4Brightness;
	UINT32 u4Contrast;
	UINT32 u4Hue;
	UINT32 u4Saturation;
	UINT32 u4Sharpness;
	UINT32 u4ResX;
	UINT32 u4ResY;
	BOOL fgProgressive;
	BOOL fgValid;
} NAV_VIDEOPATH_T;

typedef enum _E_NAV_MODE
{
	E_NAV_MODE_SINGLE,	
	E_NAV_MODE_PIP,	
	E_NAV_MODE_POP
} E_NAV_MODE;

typedef enum _E_NAV_MSG {
	E_NAV_MSG_OK,
	E_NAV_MSG_NOT_SUPPORT,
	E_NAV_MSG_PARTIAL_OK,
	E_NAV_MSG_CHANNEL_NOT_SCANNED,
	E_NAV_MSG_HAS_INIT,
    	E_NAV_MSG_WRONG_STATE,
	E_NAV_MSG_ERROR = 255
} E_NAV_MSG;

typedef enum _E_NAV_PERF_EVENT
{
    E_NAV_PERF_VDP1STPIC,
    E_NAV_PERF_IRCHPROG,
    E_NAV_PERF_EVENT_MAX
} E_NAV_PERF_EVENT;

typedef struct _NAV_ATV_TBL_PROG_T {    
    UINT16 u2FreqTabIdx;
    UINT32              u4TvSys;
    UINT32              u4SoundSys;    
    UINT32 u4Freq;
} NAV_ATV_TBL_PROG_T;

typedef struct _NAV_ATV_PROG_INFO {
    UINT32 u4ATVTblIdx;
    UINT32 u4ResX;
    UINT32 u4ResY;
    BOOL fgProgressive;
    BOOL fgValid;
} NAV_ATV_PROG_INFO;


#define ATSC_CH_NS            70  // 0~69
#define CABLE_CH_NS           160 // 0~159
#define NTSC_CABLE_CH_NS 135
#define NTSC_RF_CH_NS 69

#define DVBTUK_CH_NS 70//69 // For UK
#define DVBTTW_CH_NS 7 // For Taiwan
#define ISDBT_CH_NS 70 // for Brazil 0~69
//#define DVBT_CH_NS 6 // For Taiwan


EXTERN UINT32 au4AtscChannel[ATSC_CH_NS];
EXTERN UINT32 au4CableChannel[CABLE_CH_NS];
EXTERN UINT32 au4NTSCCABLEChannel[NTSC_CABLE_CH_NS];
EXTERN UINT32 au4NTSCRFChannel[NTSC_RF_CH_NS];
EXTERN UINT32 au4DVBTUKChannel[DVBTUK_CH_NS];
EXTERN UINT32 au4DVBTTWChannel[DVBTTW_CH_NS];
EXTERN UINT32 au4NTSCCABLEFreq[NTSC_CABLE_CH_NS];
EXTERN UINT32 au4NTSCRFFreq[NTSC_RF_CH_NS];

EXTERN INT32 NAV_Init(const NAV_OPTION_T* prOpt);
EXTERN BOOL NAV_EVENT_GetMenuStatus(void);
EXTERN BOOL NAV_IsDTVScannning(void);
EXTERN void NAV_SetCCStatistic(BOOL fgEnable);
EXTERN void NAV_SetSCStatistic(BOOL fgEnable);
EXTERN void NAV_SetPid(UINT32 u4VPid, UINT32 u4VPidType, UINT32 u4APid, UINT32 u4APidType, UINT32 u4PCRPid, UINT32 u4PhyCh, BOOL fgQam64);
EXTERN void NAV_PERF_RecordTime(E_NAV_PERF_EVENT eEvent);
EXTERN void NAV_PERF_DeltaTime(E_NAV_PERF_EVENT eEvent0, E_NAV_PERF_EVENT eEvent1);

/*
 *  nav_av.h
 */
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
EXTERN INT32 NAV_DrawInfoOSD_2(UINT16 u2Major, UINT16 u2Minor);

EXTERN INT32 NAV_ShowBorder(UINT32 u4VdpId);
EXTERN INT32 NAV_ClearBorder(UINT32 u4VdpId);
EXTERN BOOL NAV_GetBorderStatus(void);

EXTERN INT32 NAV_SetSrcActive(VOID *pvArg);
EXTERN INT32 NAV_SetupInputSourceItem(void);
EXTERN INT32 NAV_SetDTVSystem(void* pvArg);
EXTERN INT32 NAV_SetATVSystem(void* pvArg);
EXTERN INT32 NAV_SetDTVChannelRange(UINT32 u4ManualScan, UINT32 u4ChStart, UINT32 u4ChEnd);

EXTERN INT32 NAV_SetFreeze(UINT32 u4VdpId, BOOL fgFreeze);
EXTERN BOOL NAV_GetFreeze(UINT32 u4VdpId);
EXTERN INT32 NAV_StopScan(void);
EXTERN INT32 NAV_SetHDMI(void* pvArg);
EXTERN INT32 NAV_SwapSrc(void);
EXTERN void NAV_LockAVMutex(UINT32 u4VdpId);
EXTERN void NAV_UnlockAVMutex(UINT32 u4VdpId);
EXTERN void NAV_OpenNIM(void);
EXTERN INT32 NAV_SetScart(void* pvArg);

/*
 *  nav_view.c
 */
EXTERN INT32 NAV_VIEWER_Init(VOID);
EXTERN INT32 NAV_VIEWER_Start(const CHAR* szFileName);
EXTERN INT32 NAV_VIEWER_Stop(VOID);

#endif
