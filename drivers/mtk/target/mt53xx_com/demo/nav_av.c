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
 * $RCSfile: nav_av.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file nav_av.c
 *
 */

#include "x_lint.h"
 LINT_EXT_HEADER_BEGIN
#include "x_printf.h"
#include "x_stl_lib.h"
#include "x_assert.h"
//#include "x_hal_5371.h"
#include "x_hal_926.h"

#include "osd_if.h"
/*
#include "gfx_if.h"
#include "ir_if.h"
#include "vdp_if.h"
#include "mpv_if.h"
#include "dmx_if.h"
#include "fbm_if.h"
#include "aud_if.h"
*/
#include "drv_fnt.h"
#include "drv_common.h"
#include "psipsr_if.h"
#include "x_tuner.h"
//#include "api_tvtuner.h"
//#include "sv_const.h"
//#include "i1394_if.h"
//#include "vdo_misc.h"
//#include "drv_video.h"
//#include "nptv_if.h"
#include "x_drv_cli.h"
#include "u_tuner.h"
#include "drv_dbase.h"
//#include "video_timing.h"
#include "stc_drvif.h"
#include "ir_if.h"
#include "vdo_if.h"
#include "panel.h"
#include "srm_drvif.h"

LINT_EXT_HEADER_END

#include "widget_if.h"
#include "nav.h"
#include "nav_av.h"
#include "nav_freqtbl.h"
#include "nav_i1394.h"
#include "nav_vquality.h"
#include "nav_aquality.h"
#include "nav_event.h"
#include "nav_debug.h"
#include "nav_config.h"
#include "nav_mm.h"

#include "x_drv_if.h"

//---------------------------------------------------------------------------
// Constant definition
//---------------------------------------------------------------------------
#define _SIGNAL_THRESHOLD 10
#define NAV_MPV_ES_DV 5

#define SV_VDO_NOSIGNAL 0
#define SV_VDO_STABLE 3

// SCART Input Type
#define SV_SCART_AUTO			0x00
#define SV_SCART_CVBS_RGB		0x01
#define SV_SCART_CVBS			0x02
#define SV_SCART_RGB			0x03
#define SV_SCART_SV			0x04
//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
HANDLE_T hModeChangeSema;
HANDLE_T hVgaSema;
NAV_VIDEOPATH_T arVideoPath[NAV_VIDEOPATH_MAX_NUM] =
{
    {(UINT32)E_NAV_VS_DT1,  {0, 0, VDP_MAX_REGION_WIDTH, VDP_MAX_REGION_HEIGHT}, 50, 100, 100, 100, 100, 4, 0, 0, FALSE, FALSE},
    {(UINT32)E_NAV_VS_HDMI,  {VDP_MAX_REGION_WIDTH >> 1, 0, VDP_MAX_REGION_WIDTH >> 1, VDP_MAX_REGION_HEIGHT}, 50, 100, 100, 100, 100, 4, 0, 0, FALSE, FALSE}

};
NAV_DTV_PROG_INFO rCurrDTVProg =
{
    0, 0, 0, 0, 1, 0, 0, FALSE, 0
};
NAV_ATV_PROG_INFO rCurrATVProg;
UINT32 ePIPMode = 0;
UINT32 u4ActiveVDP = 0;

CRIT_STATE_T t_crit_scanchannel;

UINT32 u4NumATVProg = 0;
NAV_ATV_TBL_PROG_T arATVTbl[NAV_ATV_PROG_MAX_NUM];
UINT32 u4NumDTVProg = 0;
PSIPSR_TBL_VCT_T arVctTbl[NAV_DTV_PROG_MAX_NUM];
PSIPSR_TBL_TS2FREQ_T arTsTbl[NAV_DTV_PROG_MAX_NUM];
WIDGET_BITMAP_T* prInfoOSDBg;
BOOL fgNavAvInit = FALSE; // for driver to check if nav av is initialized

//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
static UINT32 u4DTVPhyCh = 0;
static UINT32 u4ATVPhyCh = 0;
static UINT32 u4DTVProgAtPhyCh = 0;
static BOOL fgDTVChannelScanning = FALSE;
static BOOL fgATVChannelScanning = FALSE;
#ifndef CC_NAV_DEMOD_DVBT
static UINT32 u4DTVSystem = NAV_DTV_SYSTEM_ATSC;
static UINT32 u4ATVSystem = NAV_ATV_SYSTEM_CABLE;
#else
    #ifndef DRV_IF_BR 
        static UINT32 u4DTVSystem = NAV_DTV_SYSTEM_DVBT_UK;
        static UINT32 u4ATVSystem = NAV_ATV_SYSTEM_PAL_BG;
    #else
        static UINT32 u4DTVSystem = NAV_DTV_SYSTEM_ISDBT;
        static UINT32 u4ATVSystem = NAV_ATV_SYSTEM_NTSC_M;    
    #endif
#endif
static HANDLE_T _hPlayAVSema;
static HANDLE_T _hPlayAVSemaPIP;

static HANDLE_T _hOSDMsgq;
static HANDLE_T _hATVCBMsgq;
static HANDLE_T _hATVAudMsgq;
static HANDLE_T _hTtxThread;
static HANDLE_T _hTtxMsgq;
static HANDLE_T _hAudCtrlMsgq;
static UINT32 _u4EnableNIM = 0;
//static UINT32 _u4SignalLevel = 0;
static INT32 _i4ChStart = 1;
static INT32 _i4ChEnd;// = sizeof(au4AtscChannel)/sizeof(UINT32)-1;
static BOOL fgManualScan = FALSE;
//static UINT32* _prChannel = au4AtscChannel;
static UINT32 _u4TotalTsNum = 0;

static UINT32 u4Timeout = NAV_PSI_TIMEOUT;

static BOOL  _afgQam64[NAV_DTV_PROG_MAX_NUM];

//static UINT32 u4NumDTVProg = 0;
//static PSIPSR_TBL_VCT_T arVctTbl[NAV_DTV_PROG_MAX_NUM];

static PSIPSR_TBL_PMT_T arPMTTbl[NAV_DTV_PROG_MAX_NUM];
//static PSIPSR_TBL_MGT_T arMgtTbl[NAV_DTV_PROG_MAX_NUM];
//static PSIPSR_TBL_TS2FREQ_T arTsTbl[NAV_DTV_PROG_MAX_NUM];

//static UINT32 u4NumATVProg = 0;
//static NAV_ATV_TBL_PROG_T arATVTbl[NTSC_CABLE_CH_NS];

static BOOL _fgBorderOn = FALSE;
static BOOL _fgFreeze[2] =
{
	FALSE,
	FALSE
};

static CRIT_STATE_T _tScanCrit;
static BOOL _fgBreakScan = FALSE;
BOOL _fgCliATVSearch =FALSE;
UINT32  _i4ATVChStart = 1;
UINT32  _i4ATVChEnd ;

#if 0
static HAL_TIME_T _rHalTimeLogStart;
static UINT32 _u4LevelTotal =0;
static UINT32 _u4LevelMax=255;
static UINT32 _u4LevelMin=0;
static UINT32 _u4PERTotal=0;
static UINT32 _u4PERMax=255;
static UINT32 _u4PERMin=0;
static UINT32 _u4SNRTotal=0;
static UINT32 _u4SNRMax=255;
static UINT32 _u4SNRMin=0;
static BOOL _fgCCStatistic = TRUE;
#endif
static BOOL _fgSCStatistic = TRUE;

// log use
static DRV_DBASE_T* _prDbase = NULL;

#ifdef USE_RGB8_CLUT
static WIDGET_FONT_PROPERTY_T rInfoOSDFont =
{
    255, //0xFF90,
    1, //0xF237,
    300,
    24
};
#else
static WIDGET_FONT_PROPERTY_T rInfoOSDFont =
{
    0xFF90,
    0xF237,
    300,
    24
};
#endif

// Main/sub video decoder combination possibility
static const BOOL _fgVDPairTable[E_NAV_VD_MAX][E_NAV_VD_MAX] =
{
//  TUNER	TVD		HDTV	HDMI	MPEG	I1394	SCART
    {FALSE,	FALSE,	TRUE,	TRUE,	FALSE,	FALSE,	FALSE},  // TUNER
    {FALSE,	FALSE,	TRUE,	TRUE,	TRUE,	FALSE,	FALSE},	// TVD
    {TRUE,	TRUE,	FALSE,	TRUE,	TRUE,	FALSE,	FALSE},	// HDTV
    {TRUE,	TRUE,	TRUE,	FALSE,	TRUE,	FALSE,	TRUE},	// HDMI
    {FALSE,	TRUE,	TRUE,	TRUE,	FALSE,	FALSE,	FALSE},	// MPEG
    {FALSE,	FALSE,	FALSE,	FALSE,	FALSE,	FALSE,	FALSE},	// I1394
    {FALSE,	FALSE,	FALSE,	TRUE,	FALSE,	FALSE,	FALSE}	// SCART
};

static const CHAR _aszSourceIdx2Str[E_NAV_VS_MAX][10] =
{
    "ATV", 
    "AV1", "AV2", "AV3", "AV4",
    "S-Video1", "S-Video2", "S-Video3", 
    "YPbPr1", "YPbPr2", "YPbPr3", "YPbPr4", 
    "VGA", 
    "HDMI", "HDMI2", "HDMI3", "HDMI4", "HDMI5",
    "DVI", "DTV", "I1394", "SCART", "SCART2"
};

// Maping from Navigator's input source index to driver's video source index
static const UINT32 _au4SourceIdx2VS[E_NAV_VS_MAX + 1] =
{
    SV_VS_ATD1,
    SV_VS_CVBS1,
    SV_VS_CVBS2,
    SV_VS_CVBS3,
    SV_VS_CVBS4,
    SV_VS_S1,
    SV_VS_S2,
    SV_VS_S3,
    SV_VS_YPbPr1,
    SV_VS_YPbPr2,
    SV_VS_YPbPr3,
    SV_VS_YPbPr4,
    SV_VS_VGA,    
    SV_VS_HDMI1,
    SV_VS_HDMI2,
    SV_VS_HDMI3,
    SV_VS_HDMI4,    
    SV_VS_HDMI5,        
    SV_VS_DVI,    
    SV_VS_DT1,
    SV_VS_DT2,
    SV_VS_SCART1,
    SV_VS_SCART2,
    SV_VS_MAX
};

// Maping from Navigator's input source index to video decoder index
static const UINT32 _au4SourceIdx2VD[E_NAV_VS_MAX + 1] =
{
    E_NAV_VD_TUNER,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_TVD,
    E_NAV_VD_HDTV,
    E_NAV_VD_HDTV,
    E_NAV_VD_HDTV,
    E_NAV_VD_HDTV,    
    E_NAV_VD_HDTV,    
    E_NAV_VD_HDMI,
    E_NAV_VD_HDMI,
    E_NAV_VD_HDMI,
    E_NAV_VD_HDMI,
    E_NAV_VD_HDMI,    
    E_NAV_VD_HDMI,        
    E_NAV_VD_MPEG,
    E_NAV_VD_I1394,
    E_NAV_VD_SCART,
    E_NAV_VD_SCART,
    E_NAV_VD_MAX,
};

static HANDLE_T _hTunerMsg;

static DRV_TV_SYS_T _arTvSysMap[8] =
{
    DRV_TV_SYS_M_N,
    DRV_TV_SYS_M_N,    
    DRV_TV_SYS_B_G,
    DRV_TV_SYS_D_K,
    DRV_TV_SYS_I,    
    DRV_TV_SYS_L,
    DRV_TV_SYS_L1,
    DRV_TV_SYS_M_N
};

static UINT8 u1CurrentMTS = 0;
static UINT8 u1TotalMTS = 0;
static WIDGET_OSDINFO_T _rBorderOSD;
static DRV_OSD_RGN_T _rBorderRegion;
static UINT32 _u4BorderRegionID = NULL;    
static HAL_TIME_T rTime1, rTime2, rTimeDiff;
static UINT32 u4ScartDispalyFormat = DRV_VDO_SCART_FS_ASPECT_16_9;
static UINT32 u4Scart2DispalyFormat = DRV_VDO_SCART_FS_ASPECT_16_9;
//---------------------------------------------------------------------------
// Public function forward declarations
//---------------------------------------------------------------------------
INT32 NAV_ScanDTVChannel(UINT32 u4Src);
INT32 NAV_ScanATVChannel(UINT32 u4Src);
INT32 NAV_SetMode(UINT32 u4Mode);
INT32 NAV_SetProg(UINT32 u4VdpId, UINT32 u4ProgNum);
INT32 NAV_SetSrc(UINT32 u4VdpId, UINT32 u4NewSrc);
INT32 NAV_InitAV(void);
/* Video related */
INT32 NAV_SetVideoHScale(VOID* pvArg);
INT32 NAV_SetVideoVScale(VOID* pvArg);
INT32 NAV_SetVideoHPos(VOID* pvArg);
INT32 NAV_SetVideoVPos(VOID* pvArg);
INT32 NAV_DrawInfoOSD(void);
INT32 NAV_ClearInfoOSD(void);
INT32 NAV_PlayAV(UINT32 u4VdpId, UINT32 u4Src);
INT32 NAV_StopAV(UINT32 u4VdpId, UINT32 u4Src);
INT32 NAV_SetSrcActive(VOID *pvArg);
UINT32 NAV_LookupSourceIndex(const CHAR* szArg);
INT32 NAV_SetFreeze(UINT32 u4VdpId, BOOL fgFreeze);
BOOL NAV_GetFreeze(UINT32 u4VdpId);
void NAV_LockAVMutex(UINT32 u4VdpId);
void NAV_UnlockAVMutex(UINT32 u4VdpId);
INT32 NAV_SwapSrc(void);
BOOL NAV_IsSourcePairValid(UINT32 u4MainSrc, UINT32 u4SubSrc);
void NAV_ATVSigCallback(void* pvNfyTag, UINT32 u4SigStatus, UINT32 u4SigSrc);
BOOL NAV_IsDTVScannning(void);
//void NAV_SetCCStatistic(BOOL fgEnable);
//void NAV_SetSCStatistic(BOOL fgEnable);
void NAV_SetPid(UINT32 u4VPid, UINT32 u4VPidType, UINT32 u4APid, UINT32 u4APidType, UINT32 u4PCRPid, UINT32 u4PhyCh, BOOL fgQam64);
INT32 NAV_SetDTVChannelRange(UINT32 u4ManualScan, UINT32 u4ChStart, UINT32 u4ChEnd);
#if 0
BOOL _NAV_VideoCallback(const DMX_PES_MSG_T* prPes);
#endif
void NAV_OpenNIM(void);

//---------------------------------------------------------------------------
// Static function forward declarations
//---------------------------------------------------------------------------
//static INT32 _SetNIM(UINT32 u4Channel);
static INT32 _ScanDTVChannel(UINT32 u4Src);
static INT32 _ScanATVChannel(UINT32 u4Src);
static INT32 _SetMode(UINT32 u4Mode);
static INT32 _SetSrc(UINT32 u4VdpId, UINT32 u4NewSrc);
static INT32 _PlayAV(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _PlayAVDTV(UINT32 u4VdpId);
static INT32 _PlayAVATV(UINT32 u4VdpId);
static INT32 _PlayAVAnalogInput(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _StopAV(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _StopAVDTV(UINT32 u4VdpId);
static INT32 _StopAVATV(UINT32 u4VdpId);
static INT32 _StopAVAnalogInput(UINT32 u4VdpId);
static INT32 _PlayAudio(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _PlayAudioDTV(UINT32 u4VdpId);
static INT32 _PlayAudioATV(UINT32 u4VdpId);
static INT32 _PlayAudioAnalogInput(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _StopAudio(UINT32 u4VdpId, UINT32 u4Src);
static INT32 _StopAudioDTV(UINT32 u4VdpId);
static INT32 _StopAudioATV(UINT32 u4VdpId);
static INT32 _StopAudioAnalogInput(UINT32 u4VdpId);
#if 0
static INT32 _PlayAVI1394(UINT32 u4VdpId);
static INT32 _StopAVI1394(UINT32 u4VdpId);
static void _I1394StatusPollThread(void);
#endif
static INT32 _SetProg(UINT32 u4VdpId, UINT32 u4ProgNum);
static INT32 _SetProgDTV(UINT32 u4VdpId, UINT32 u4ProgNum);
static INT32 _SetProgATV(UINT32 u4VdpId, UINT32 u4ProgNum);
//static INT32 _LookupPhyCh(UINT32 u4TsId);
static INT32 _SetupInputSourceItem(void);
static void _PrintDTVProgInfo(void);
static void _PrintATVProgInfo(void);
static void _SwapVCTEntry(PSIPSR_TBL_VCT_T* prEntry1, PSIPSR_TBL_VCT_T* prEntry2);
static INT32 _CompareVCTEntry(PSIPSR_TBL_VCT_T rEntry1, PSIPSR_TBL_VCT_T rEntry2);
static void _SortVCT(void);
//static void _NIMSigPollThread(void);
//static void _PrintReport(void);
static void _ResetATVTbl(UINT32 u4System);
static INT32 _DrawInfoOSD(const NAV_OSD_MSG_T* prMsg);
//static void _ResetLog(void);
static void _ATVSigProcThread(void);
static void _ATVSigProc(const NAV_ATVCB_MSG_T* prMsg);
//static void _GetOverscanRatio(UINT32 *pu4X,UINT32 *pu4Y); // look up overscan setting table
static void _LookupOverscanRatio(UINT32 u4VdpId, UINT32 *pu4T,UINT32 *pu4B, UINT32 *pu4L, UINT32 *pu4R); // look up overscan setting table

//---------------------------------------------------------------------------
// Importedl function forward declarations
//---------------------------------------------------------------------------
EXTERN void NIM_SetConnType(DRV_CONN_TYPE_T e_conn_type);
EXTERN void NIM_SetMod(TUNER_MODULATION_T Mod);
EXTERN BOOL fgDrvTunerCheckFreqLock(void);
EXTERN BOOL fgDrvTunerCheckTVDLock( UINT8 bTryLoopCnt );
EXTERN UINT8 bDrvTunerSearchNearbyFreq(UINT8 bScanType, UINT16 *wFreqDR);
EXTERN UINT8 bDrvTunerCheckVIFLock(UINT16 wFreqDR);
EXTERN BOOL fgDrvTunerSetFreq(UINT16 wFreqDR);
EXTERN void vDrvTunerInit( void );
EXTERN void TunerDetachI2C(UINT16 TunerID, BOOL bDetach);
#if 0
EXTERN void AUD_SetAvSynMode(UINT8 u1DecId, AV_SYNC_MODE_T eSynMode);
#endif
EXTERN void DBS_Printf(void);
EXTERN void DBS_ErrRate(void);
EXTERN BOOL fgDrvTunerSetSystem(UINT8 bSubSysIdx);
EXTERN UINT16  wDrvTunerAFT( UINT16 wFreqDR );
EXTERN UINT8 bApiVideoMainSubSrc(UINT8 bMainSrc, UINT8 bSubSrc);
EXTERN void vScartChangeMode(UINT8 bICIn);
EXTERN BOOL fgHDMISupportAudio(void);
static void _InitBorder(void)
{
    static BOOL fgInited = FALSE;

    if(!fgInited)
    {
        fgInited = TRUE;
        
        _rBorderRegion.u4Width = (TV_WIDTH >> 2) >> 3 << 3;
        _rBorderRegion.u4Height = TV_HEIGHT >> 2;
        _rBorderRegion.u4ScaleWidth = TV_WIDTH;
        _rBorderRegion.u4ScaleHeight = TV_HEIGHT;
        _rBorderRegion.u1PlaID = NAV_CONFIG_GetBorderOsdPlane();
        _rBorderRegion.rColorMode = DRV_CM_ARGB_4444;
        _rBorderRegion.u4Left = 0;
        _rBorderRegion.u4Top = 0;
        _rBorderRegion.u4Pitch = _rBorderRegion.u4Width << 1;    
    
        _rBorderRegion.pu1FrameBaseAddr = (UINT8*)DRV_SYS_AllocAlignedDmaMemory( 
        	_rBorderRegion.u4Pitch * _rBorderRegion.u4Height, 16);   
    
        _rBorderOSD.u4X = _rBorderRegion.u4Left;
        _rBorderOSD.u4Y = _rBorderRegion.u4Top;    
        _rBorderOSD.u4W = _rBorderRegion.u4Width;    
        _rBorderOSD.u4H = _rBorderRegion.u4Height;   
        _rBorderOSD.apu1OSDBackstage =  _rBorderRegion.pu1FrameBaseAddr;
        _rBorderOSD.apu1OSDBuffer = (UINT8*)DRV_SYS_AllocAlignedDmaMemory( 
        _rBorderRegion.u4Pitch * _rBorderRegion.u4Height, 16);
        _rBorderOSD.u4Plane = NAV_CONFIG_GetBorderOsdPlane();
        _rBorderOSD.u4ColorMode = DRV_CM_ARGB_4444;
    
        x_memset(_rBorderOSD.apu1OSDBackstage, 0, _rBorderRegion.u4Pitch * _rBorderRegion.u4Height);
        x_memset(_rBorderOSD.apu1OSDBuffer, 0, _rBorderRegion.u4Pitch * _rBorderRegion.u4Height);
    }    
    
    // Create OSD later for displaying JPEG image
    DRV_OSD_RGN_Create(&_rBorderRegion, &_u4BorderRegionID);
    DRV_OSD_RGN_SetAlphaMode(_u4BorderRegionID, DRV_OSD_BM_PIXEL);
    
}

static void _ReleaseBorder(void)
{
    DRV_OSD_RGN_Delete(_rBorderRegion.u1PlaID, _u4BorderRegionID);
}

static void _TunerAnaCb(INT32 i4Tag)
{
    DRV_TUNER_MSG_T rMsg;
    
    DRV_TUNER_GetMessage(i4Tag, &rMsg);
   
    LOG(5, "[NAV_AV] Get tuner cb tag value %d\n", (UINT32)i4Tag & 0xffffff);
    
    x_msg_q_send(_hTunerMsg, (void*)&rMsg.rData, sizeof(DRV_TUNER_DATA_T), 253);
}

static void _TunerDigCb(INT32 i4Tag)
{
    DRV_TUNER_MSG_T rMsg;
    
    DRV_TUNER_GetMessage(i4Tag, &rMsg);

    
    LOG(5, "[NAV_AV] Get tuner cb tag value %d\n", (UINT32)i4Tag >> 24);
    
    x_msg_q_send(_hTunerMsg, (void*)&rMsg.rData, sizeof(DRV_TUNER_DATA_T), 253);
}

static void _SigCallbackMain(INT32 i4Tag)
{
    DRV_VDO_INFO_T rInfo;
    NAV_ATVCB_MSG_T rMsg;
    
    DRV_VDO_GetInfo(0, &rInfo);
    rMsg.u4VdpId = 0;
    rMsg.u4Src = NAV_LookupSourceIndexFromMKVSRC(rInfo.rInputSrc);
    LOG(6, "[NAV] Video signal notify %d\n", rMsg.u4SigStatus);
    if(rInfo.rSigStatus == DRV_VDO_SIGNAL_DETECTED)
    {
        rMsg.u4SigStatus = SV_VDO_STABLE;
    }
    else
    {
        rMsg.u4SigStatus = SV_VDO_NOSIGNAL;
    }
    if(arVideoPath[u4ActiveVDP].u4Src != rMsg.u4Src) 
    {
        ;//printf("XXXXXXXXXXXXXXXXXXXXXXXXXX source does not match.\n");
    }
    VERIFY(x_msg_q_send(_hATVCBMsgq, (const void*)&rMsg, sizeof(NAV_ATVCB_MSG_T), 253) == OSR_OK);
}
static void _SigCallbackSub(INT32 i4Tag)
{
    DRV_VDO_INFO_T rInfo;
    NAV_ATVCB_MSG_T rMsg;
    
    DRV_VDO_GetInfo(1, &rInfo);
    rMsg.u4VdpId = 1;  
    if(rInfo.rSigStatus == DRV_VDO_SIGNAL_DETECTED)
    {
        rMsg.u4SigStatus = SV_VDO_STABLE;
    }
    else
    {
        rMsg.u4SigStatus = SV_VDO_NOSIGNAL;
    }
    VERIFY(x_msg_q_send(_hATVCBMsgq, (const void*)&rMsg, sizeof(NAV_ATVCB_MSG_T), 253) == OSR_OK);
}
#if 0
static void _ScartPin8Nfy(UINT32 i4Tag)
{
    DRV_VDO_INFO_T rInfo;
    NAV_ATVCB_MSG_T rMsg;
    
    DRV_VDO_GetInfo(1, &rInfo);
    rMsg.u4VdpId = 0;  
    if(rInfo.rSigStatus == DRV_VDO_SIGNAL_DETECTED)
    {
        rMsg.u4SigStatus = SV_VDO_STABLE;
    }
    else
    {
        rMsg.u4SigStatus = SV_VDO_NOSIGNAL;
    }    
    VERIFY(x_msg_q_send(_hATVCBMsgq, (const void*)&rMsg, sizeof(NAV_ATVCB_MSG_T), 253) == OSR_OK);
}
#endif

static void _ScartProc(INT32 i4Tag)
{
    //DRV_VDO_INFO_T rInfo;
    DRV_VDO_SCART_STATUS_T rStatus;
    DRV_INPUT_SRC_T rInput;
    

    
    DRV_VDO_GetScartStatus(0, &rStatus);
   
    if(rStatus.rFs == DRV_VDO_SCART_FS_ASPECT_4_3 || rStatus.rFs == DRV_VDO_SCART_FS_ASPECT_16_9)
    {
        WIDGET_EVENT_T rEvent;
        rEvent.rIrEvent.rType = DRV_IR_EVENT_EXT;
        rEvent.u4Extra[0] = (UINT32)rStatus.rFs;
        
        rInput = (DRV_INPUT_SRC_T)((UINT32)i4Tag & 0xff);
        
        if(rInput == DRV_IS_SCART_1)
        {
            rEvent.rIrEvent.u4Key = BTN_SCART_1;
            WIDGET_SendExternalEvent(rEvent);
            u4ScartDispalyFormat = (UINT32)rStatus.rFs;
        }
        else
        {
            rEvent.rIrEvent.u4Key = BTN_SCART_2;
            WIDGET_SendExternalEvent(rEvent);
            u4Scart2DispalyFormat = (UINT32)rStatus.rFs;
        }

    }
    printf("[VDO] Scart Pin 8 Notify: %d\n", rStatus);
}


static void _VgaAutoNfy(INT32 i4Tag)
{
#ifndef __MODEL_slt__
    x_sema_unlock(hVgaSema);
#else
    extern HANDLE_T _hSltAuto;
    x_sema_unlock(_hSltAuto);
#endif
}

static void _VgaAutocolorNfy(INT32 i4Tag)
{
#ifndef __MODEL_slt__
    x_sema_unlock(hVgaSema);
#else
    extern HANDLE_T _hSltAutoColor;
    x_sema_unlock(_hSltAutoColor);
#endif
}

#if defined(CC_MT5365) ||defined(CC_MT5395)
static BOOL _IsMainSrc3D() 
{
#ifdef DRV_IF_3DTV
    DRV_VDO_INFO_T rInfo;

	DRV_VDO_GetInfo(NAV_VDP_MASTER, &rInfo);
	if(rInfo.r3DStatus == DRV_VDO_3D_WO_FORCE)
	{
    	return TRUE;
	}
	else
	{
        return FALSE;
	}
#else
    return TRUE;
#endif
}
#endif
//-------------------------------------------------------------------------
/** _SetScart
 *  Set SCART input mode
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 _SetScart(UINT32 u4Mode)
{
    DRV_VDO_SCART_PARAM_T rParam;
    switch(u4Mode)
    {
        case SV_SCART_CVBS:
        case SV_SCART_RGB:
        case SV_SCART_SV:
        case SV_SCART_CVBS_RGB:
        case SV_SCART_AUTO:
            rParam.rMode = (DRV_VDO_SCART_MODE_T)u4Mode;
            DRV_VDO_SetScart(DRV_VDO_SCARTMSK_MODE, &rParam);
        	break;
        default:
        	return E_NAV_MSG_NOT_SUPPORT;
    }
    return E_NAV_MSG_OK;
}

static void _VbiTtxNfy(INT32 i4Tag)
{
    UINT32 u4Tag = (UINT32)i4Tag;
    x_msg_q_send(_hTtxMsgq, &u4Tag, sizeof(u4Tag), 253);

    /*
    DRV_VDO_VBITTX_DATA_T rData;   
    UINT32 u4Msg = u4Tag;    
    BOOL fgTtxLog = FALSE;
    UINT32 i, j;
    
    DRV_VDO_GetVbiTtx(0, &rData, u4Msg);    
    if(fgTtxLog)
    {
        for(i = 0; i < rData.u2NumPacket; i++)
        {
            printf("[TTX] ", rData.prPacket[i].au1Buf[j]);
            for(j = 0; j < 42; j++)
            {
                printf("%c", rData.prPacket[i].au1Buf[j]);
            }
            printf("\n", rData.prPacket[i].au1Buf[j]);
        }
    }    
    DRV_VDO_UnlockVbiTtx(0, &rData, u4Msg);    
    */
}

static BOOL _fgTtxLog = FALSE;
static void _TtxProcThread(void* pvTag)
{
    UINT16 u2MsgIdx;
    UINT32 u4Msg;
    UINT32 u4MsgSize;
    DRV_VDO_VBITTX_DATA_T rData;    

    
    while(1)
    {
        u4MsgSize = sizeof(u4Msg);
        if(x_msg_q_receive(&u2MsgIdx,&u4Msg,(SIZE_T *) &u4MsgSize, &_hTtxMsgq, 1, X_MSGQ_OPTION_WAIT) == OSR_OK)
        {
            if((u4Msg & 0xFF) == DRV_VDO_VBITTX_DATA_ARRIVAL)
            {
                UINT32 i;
                
                DRV_VDO_GetVbiTtx(0, &rData, u4Msg);
                if(_fgTtxLog)
                {
                    for(i = 0; i < rData.u2NumPacket; i++)
                    {
                        printf("[TTX] %d: ", i);
                        printf("%c", rData.prPacket[i].au1Buf[0]);
                        /*
                        for(j = 0; j < 42; j++)
                        {
                            printf("%c", rData.prPacket[i].au1Buf[j]);
                        }
                        */                        
                    }
                }
                DRV_VDO_UnlockVbiTtx(0, &rData, u4Msg);
            }
        }
    }
}

VOID NAV_MeasureTime() 
{
    HAL_GetTime(&rTime2);
    HAL_GetDeltaTime(&rTimeDiff, &rTime1, &rTime2);
    printf("\n[NAV]Change source time: %d.%06d\n\n", rTimeDiff.u4Seconds, rTimeDiff.u4Micros);
}
//-------------------------------------------------------------------------
/** NAV_IsSourcePairValid
 *  Check if the main/sub path input source combination is a valid one
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
BOOL NAV_IsSourcePairValid(UINT32 u4MainSrc, UINT32 u4SubSrc)
{
    if(u4MainSrc < (UINT32)E_NAV_VS_MAX && u4SubSrc < (UINT32)E_NAV_VS_MAX)
    {
        return _fgVDPairTable[_au4SourceIdx2VD[u4MainSrc]][_au4SourceIdx2VD[u4SubSrc]];
    }
    return FALSE;
}

//-------------------------------------------------------------------------
/** NAV_PlayAV
 *  Entry function for playing A/V of each input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_PlayAV(UINT32 u4VdpId, UINT32 u4Src)
{
    return _PlayAV(u4VdpId, u4Src);
}

//-------------------------------------------------------------------------
/** NAV_StopAV
 *  Entry function for stoping A/V of each input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_StopAV(UINT32 u4VdpId, UINT32 u4Src)
{
    return _StopAV(u4VdpId, u4Src);
}

//-------------------------------------------------------------------------
/** NAV_ScanDTVChannel
 *  Entry function for scaning digital channel
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_ScanDTVChannel(UINT32 u4Src)
{
    HANDLE_T hScanThread;
    UNUSED(u4Src);

    // DTV Channel scan can only be invoked when input source is DTV
    if(ePIPMode != E_NAV_MODE_SINGLE || arVideoPath[u4ActiveVDP].u4Src != (UINT32)E_NAV_VS_DT1)
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }

    u4DTVPhyCh = 0;
    u4DTVProgAtPhyCh = 0;

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    t_crit_scanchannel = x_crit_start();
    fgDTVChannelScanning = TRUE;
    //VERIFY(WIDGET_SetRepeat(0) == (INT32)E_NAV_MSG_OK);
    x_crit_end(t_crit_scanchannel);

    VERIFY(_StopAV(u4ActiveVDP, arVideoPath[u4ActiveVDP].u4Src) == (INT32)E_NAV_MSG_OK);
    
    u4NumDTVProg = 0;
    
    VERIFY(WIDGET_SearchListElement("DTV", prInputSrcList) != NULL);

    fgInfoOSDOn = TRUE;

    _fgBreakScan = FALSE;
    if(x_thread_create(&hScanThread, "DTV_SCAN", 2048, 100, (x_os_thread_main_fct)_ScanDTVChannel, 0, NULL) != OSR_OK)
    {
        t_crit_scanchannel = x_crit_start();
        fgDTVChannelScanning = FALSE;
        x_crit_end(t_crit_scanchannel);
    }

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_ScanATVChannel
 *  Entry function for scaning analog channel
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_ScanATVChannel(UINT32 u4Src)
{
    HANDLE_T hScanThread;
    UNUSED(u4Src);

    // ATV Channel scan can only be invoked when input source is ATV
    if(ePIPMode != E_NAV_MODE_SINGLE || arVideoPath[u4ActiveVDP].u4Src != (UINT32)E_NAV_VS_TUNER1)
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    t_crit_scanchannel = x_crit_start();
    fgATVChannelScanning = TRUE;
    //VERIFY(WIDGET_SetRepeat(1) == (INT32)E_NAV_MSG_OK);
    x_crit_end(t_crit_scanchannel);

    VERIFY(_StopAV(u4ActiveVDP, arVideoPath[u4ActiveVDP].u4Src) == (INT32)E_NAV_MSG_OK);

    VERIFY(WIDGET_SearchListElement("ATV", prInputSrcList) != NULL);

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    fgInfoOSDOn = TRUE;

    _fgBreakScan = FALSE;
	VERIFY(x_msg_q_create(&_hATVAudMsgq, NAV_ATVAUD_MSGQ, sizeof(NAV_ATVAUD_MSG_T), 32) == OSR_OK);
    if(x_thread_create(&hScanThread, "ATV_SCAN", 2048, 100, (x_os_thread_main_fct)_ScanATVChannel, 0, NULL)  != OSR_OK)
    {
        t_crit_scanchannel = x_crit_start();
        fgATVChannelScanning = FALSE;
        x_crit_end(t_crit_scanchannel);    
    }   

    return (INT32)E_NAV_MSG_OK;

}

//-------------------------------------------------------------------------
/** NAV_SetMode
 *  Set picture mode. Available picture modes are single, PIP, and POP mode
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetMode(UINT32 u4Mode)
{
    return _SetMode(u4Mode);
}

//-------------------------------------------------------------------------
/** NAV_SetProg
 *  Set currently playing TV program 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetProg(UINT32 u4VdpId, UINT32 u4ProgNum)
{
    return _SetProg(u4VdpId, u4ProgNum);
}

//-------------------------------------------------------------------------
/** NAV_SetSrc
 *  Set curerntly input source of main or sub path
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetSrc(UINT32 u4VdpId, UINT32 u4NewSrc)
{
    INT32 i4Ret;
    HAL_GetTime(&rTime1);
    i4Ret = _SetSrc(u4VdpId, u4NewSrc);
    return i4Ret;
}

//-------------------------------------------------------------------------
/** NAV_SetSrcActive
 *  Set curerntly input source of the video path selected by the menu
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetSrcActive(VOID *pvArg)
{
    CHAR* szArg = (CHAR*)pvArg;
    UINT32 u4Src;

    if( (u4Src = NAV_LookupSourceIndex(szArg)) >= (UINT32)E_NAV_VS_MAX)
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    if(u4ActiveVDP == NAV_VDP_MASTER)
    {
        if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE || NAV_IsSourcePairValid(u4Src, arVideoPath[NAV_VDP_SLAVE].u4Src))
        {
            while ((UINT32)prInputSrcList->prCurr->pvContent != u4Src)
            {
                prInputSrcList->prCurr = prInputSrcList->prCurr->prNext;
            }
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            return _SetSrc(u4ActiveVDP, u4Src);
        }
    }
    else
    {
        if(NAV_IsSourcePairValid(arVideoPath[NAV_VDP_MASTER].u4Src, u4Src))
        {
            while ((UINT32)prInputSrcListPIP->prCurr->pvContent != u4Src)
            {
                prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
            }
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            return _SetSrc(u4ActiveVDP, u4Src);
        }
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

//-------------------------------------------------------------------------
/** NAV_LookupSourceIndex
 *  Lookup input source index according to the input source name
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
UINT32 NAV_LookupSourceIndex(const CHAR* szArg)
{
    UINT32 u4Src;
    if(x_strcmp("DTV", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_DT1;
    }
    else if(x_strcmp("AV1", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS1;
    }
    else if(x_strcmp("AV2", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS2;
    }
    else if(x_strcmp("AV3", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS3;
    }
    else if(x_strcmp("AV4", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS4;
    }
    else if(x_strcmp("S-Video1", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_S1;
    }
    else if(x_strcmp("S-Video2", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_S2;
    }
    else if(x_strcmp("S-Video3", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_S3;
    }
    else if(x_strcmp("YPbPr1", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_YPbPr1;
    }
    else if(x_strcmp("YPbPr2", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_YPbPr2;
    }
    else if(x_strcmp("HDMI", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI;
    }
    else if(x_strcmp("HDMI2", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI2;
    }
    else if(x_strcmp("HDMI3", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI3;        
    }    
    else if(x_strcmp("HDMI4", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI4;
    }    
    else if(x_strcmp("HDMI5", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI5;
    }      
    else if(x_strcmp("VGA", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_VGA;
    }
    else if(x_strcmp("I1394", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_DT2;
    }
    else if(x_strcmp("ATV", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_TUNER1;
    }
    else if(x_strcmp("SCART", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_SCART;
    }
    else if(x_strcmp("SCART2", szArg) == 0)
    {
        u4Src = (UINT32)E_NAV_VS_SCART2;
    }   
    else
    {
        u4Src = (UINT32)E_NAV_VS_MAX;
    }
    return u4Src;
}

//-------------------------------------------------------------------------
/** NAV_LookupSourceIndex
 *  Lookup input source index according to the input source name
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
UINT32 NAV_LookupSourceIndexFromMKVSRC(DRV_INPUT_SRC_T rInput)
{
    UINT32 u4Src;
    if(rInput == DRV_IS_MPEG_1)
    {
        u4Src = (UINT32)E_NAV_VS_DT1;
    }
    else if(rInput == DRV_IS_CVBS_1)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS1;
    }
    else if(rInput == DRV_IS_CVBS_2)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS2;
    }
    else if(rInput == DRV_IS_CVBS_3)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS3;
    }
    else if(rInput == DRV_IS_CVBS_4)
    {
        u4Src = (UINT32)E_NAV_VS_CVBS4;
    }
    else if(rInput == DRV_IS_SVIDEO_1)
    {
        u4Src = (UINT32)E_NAV_VS_S1;
    }
    else if(rInput == DRV_IS_SVIDEO_2)
    {
        u4Src = (UINT32)E_NAV_VS_S2;
    }
    else if(rInput == DRV_IS_SVIDEO_3)
    {
        u4Src = (UINT32)E_NAV_VS_S3;
    }
    else if(rInput == DRV_IS_YPBPR_1)    
    {
        u4Src = (UINT32)E_NAV_VS_YPbPr1;
    }
    else if(rInput == DRV_IS_YPBPR_2)
    {
        u4Src = (UINT32)E_NAV_VS_YPbPr2;
    }
    else if(rInput == DRV_IS_HDMI_1)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI;
    }
    else if(rInput == DRV_IS_HDMI_2)    
    {
        u4Src = (UINT32)E_NAV_VS_HDMI2;
    }
    else if(rInput == DRV_IS_HDMI_3)    
    {
        u4Src = (UINT32)E_NAV_VS_HDMI3;        
    }
    else if(rInput == DRV_IS_HDMI_4)    
    {
        u4Src = (UINT32)E_NAV_VS_HDMI4;
    }    
    else if(rInput == DRV_IS_HDMI_5)
    {
        u4Src = (UINT32)E_NAV_VS_HDMI5;
    }      
    else if(rInput == DRV_IS_VGA_1)
    {
        u4Src = (UINT32)E_NAV_VS_VGA;
    }
    else if(rInput == DRV_IS_MPEG_2)
    {
        u4Src = (UINT32)E_NAV_VS_DT2;
    }
    else if(rInput == DRV_IS_ATUNER_1)
    {
        u4Src = (UINT32)E_NAV_VS_TUNER1;
    }
    else if(rInput == DRV_IS_SCART_1)
    {
        u4Src = (UINT32)E_NAV_VS_SCART;
    }
    else if(rInput == DRV_IS_SCART_2)
    {
        u4Src = (UINT32)E_NAV_VS_SCART2;
    }   
    else
    {
        u4Src = (UINT32)E_NAV_VS_MAX;
    }
    return u4Src;
}

//-------------------------------------------------------------------------
/** _ScanNIM
 *  Set NIM at the given frequency
 *  @param  eSrc        channel number
 *  @retval   E_NVGT_OK, E_NVGT_ERROR
 */
//-------------------------------------------------------------------------
//static INT32 _SetNIM(UINT32 u4Channel)
//{

//    return (INT32)E_NAV_MSG_OK;
//}

//-------------------------------------------------------------------------
/** _SetDTVChannelRange 
 *  Set the physical channel range while perform digital channel scan
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetDTVChannelRange(UINT32 u4ManualScan, UINT32 u4ChStart, UINT32 u4ChEnd)
{
#ifndef CC_NAV_DEMOD_DVBT
    if(!u4ManualScan || u4ChStart < 1 || u4ChStart >= ((u4DTVSystem == NAV_DTV_SYSTEM_ATSC)?ATSC_CH_NS:136))
    {
    	_i4ChStart = 1;
    }
    else
    {
    	_i4ChStart = (INT32)u4ChStart;
    }
    if(!u4ManualScan ||u4ChEnd < u4ChStart || u4ChEnd >= ((u4DTVSystem == NAV_DTV_SYSTEM_ATSC)?ATSC_CH_NS:136))
    {
    	_i4ChEnd = (u4DTVSystem == NAV_DTV_SYSTEM_ATSC)?ATSC_CH_NS-1:135;
    }
    else
    {
    	_i4ChEnd = (INT32)u4ChEnd;
    }
#else
#ifndef DRV_IF_BR
    UINT32 u4Max = (u4DTVSystem==NAV_DTV_SYSTEM_DVBT_UK)?DVBTUK_CH_NS:
                    ((u4DTVSystem==NAV_DTV_SYSTEM_DVBC)?DVBC_CH_NS:DVBTTW_CH_NS);
    if(!u4ManualScan ||u4ChStart < 1 || u4ChStart >= u4Max)
    {
    	_i4ChStart = (u4DTVSystem==NAV_DTV_SYSTEM_DVBT_UK)?21:1;
    }
#else
    UINT32 u4Max = ISDBT_CH_NS;
    if(!u4ManualScan ||u4ChStart < 1 || u4ChStart >= u4Max)
    {
    	_i4ChStart = 7;
    }    
#endif
    else
    {
    	_i4ChStart = (INT32)u4ChStart;
    }
    if(!u4ManualScan ||u4ChEnd < u4ChStart || u4ChEnd >= u4Max)
    {
    	_i4ChEnd = u4Max - 1;
    }
    else
    {
    	_i4ChEnd = (INT32)u4ChEnd;
    }
#endif
	return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_SetDTVChannelRange 
 *  call _SetDTVChannelRange 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetDTVChannelRange(UINT32 u4ManualScan, UINT32 u4ChStart, UINT32 u4ChEnd)
{
	return _SetDTVChannelRange(u4ManualScan, u4ChStart, u4ChEnd);
}

//-------------------------------------------------------------------------
/** _ScanDTVChannel
 *  The real thread to perform digital channel scan
 *  @param  eSrc        Signal source
 *  @retval   E_NVGT_OK, E_NVGT_ERROR
 */
//-------------------------------------------------------------------------
static INT32 _ScanDTVChannel(UINT32 u4Src)
{
    UINT32 u4Ret;
    UINT32 i,j;
    UINT32 u1ChNum = 0;
    PSIPSR_TBL_VCT_T* prVctTbl = arVctTbl;
    PSIPSR_TBL_TS2FREQ_T* prTsTbl = arTsTbl;
    UINT32 u4VdpId = u4ActiveVDP;
    BOOL fgQam64 = FALSE;

    UNUSED(u4Src);

    NAV_LockAVMutex(u4VdpId);

    _u4TotalTsNum = u4NumDTVProg = 0;
    x_memset((VOID*)arVctTbl, (UINT32)NAV_DTV_PROG_MAX_NUM, (UINT32)sizeof(PSIPSR_TBL_VCT_T));

    {
        INT32 i4Ret;

        //_u4EnableNIM = (i4Ret==0)?1:0;
        LOG(6, "_u4EnableNIM=%d, i4Ret=%d\n", _u4EnableNIM, i4Ret);

        if(!_u4EnableNIM)
        {
            _i4ChStart = 0;
            _i4ChEnd = 0;
        }

    }

    for (i = (UINT32)_i4ChStart; i <= (UINT32)_i4ChEnd; i++)
    {

        // Backdoor for fast channel scan
#if NAV_BACKDOOR_DTVSCAN
        if(!rNAVOption.fgDTVFullScan && i != 0 && i != 23 && i != 24 && i != 65 && i != 69)
        {
            continue;
        }
#endif

        _tScanCrit = x_crit_start();
        if(_fgBreakScan)
        {
            x_crit_end(_tScanCrit);
            break;
        }
        x_crit_end(_tScanCrit);

        if(_u4EnableNIM)
        {
            DRV_TV_SYS_T rTvSys;
            UINT32 u4Freq;
            UINT16 u2MsgIdx;
            DRV_TUNER_DATA_T rData;
            UINT32 u4MsgSize = sizeof(DRV_TUNER_DATA_T);            
#ifndef CC_NAV_DEMOD_DVBT
            if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
            {
                u4Freq = au4AtscChannel[i];
            }
            else
            {
                u4Freq = au4CableChannel[i];
            }
#else
            if(u4DTVSystem == NAV_DTV_SYSTEM_DVBT_UK)
            {
                u4Freq = au4DVBTUKChannel[i];
            }
            else if(u4DTVSystem == NAV_DTV_SYSTEM_DVBC) 
            {
                u4Freq = au4DVBCChannel[i];                
            }
#ifdef DRV_IF_BR            
            if(u4DTVSystem == NAV_DTV_SYSTEM_ISDBT)
            {
                u4Freq = au4ISDBTChannel[i];
            }
#endif            
            else
            {
                u4Freq = au4DVBTTWChannel[i];
            }            
#endif        

#ifndef DRV_IF_BR
            u4Freq *= 1000000;
#else
            u4Freq *= 1000;
#endif
            LOG(5, "[NAV_AV] Set tuner cb tag value %d\n", i);

            ASSERT(u4Freq); // u4Freq should not be zero

            // DTV scan channel
#ifndef CC_NAV_DEMOD_DVBT            
            if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_VSB_8;
                rParam.rDigParam.u4QamSize = 8;
                rParam.rDigParam.u4SymbolRate = 0x0;        	
                DRV_TUNER_SetParameter(&rParam);
            }
            else
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_64;
                rParam.rDigParam.u4QamSize = 64;
                rParam.rDigParam.u4SymbolRate = 5057;        	
                DRV_TUNER_SetParameter(&rParam);    
            }            
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)i);
           
            x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T*)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
                                           
            if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
            {
                if(!rData.fgLock)
                {
                    u4DTVPhyCh = i;
                    u4DTVProgAtPhyCh = 0;
                    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
                    continue;
                }
            }
            else
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_256;
                rParam.rDigParam.u4QamSize = 256;
                rParam.rDigParam.u4SymbolRate = 5057;   
                
                if(!rData.fgLock)
                {                    
                    DRV_TUNER_SetParameter(&rParam);    
                    DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)i);
                    {
                        UINT16 u2MsgIdx;
                        x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T*)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
                        
                        if(!rData.fgLock)
                        {
                            u4DTVPhyCh = i;
                            u4DTVProgAtPhyCh = 0;
                            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
                            continue;
                        }
                        fgQam64 = FALSE;                        
                    }
                }
                else
                {
                    fgQam64 = TRUE;
                }
            }                
#else
            if(u4DTVSystem == NAV_DTV_SYSTEM_DVBT_UK)
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_8_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_UNKNOWN;
                rParam.rDigParam.u4QamSize = 8;
                rParam.rDigParam.u4SymbolRate = 0x0;        	
                DRV_TUNER_SetParameter(&rParam);
                DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)i);                
            }
            else if(u4DTVSystem == NAV_DTV_SYSTEM_DVBC) 
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_8_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_UNKNOWN;   //? unknow or QAM64
                rParam.rDigParam.u4QamSize = 64;
                rParam.rDigParam.u4SymbolRate = 0;        	
                DRV_TUNER_SetParameter(&rParam);
                DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGC, rTvSys, TRUE, (void*)i);                
            }
            else
            {
                DRV_TUNER_PARAM_T rParam;
                rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
                rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
                rParam.rDigParam.fgSpectrumInversion = TRUE;
                rParam.rDigParam.pfnTuningDone = _TunerDigCb;
                rParam.rDigParam.pfnLockStatusChange = NULL;
                rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
                rParam.rDigParam.rMod = DRV_TUNER_MOD_UNKNOWN;
                rParam.rDigParam.u4QamSize = 64;
                rParam.rDigParam.u4SymbolRate = 0;        	
                DRV_TUNER_SetParameter(&rParam);    
                DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)i);                
            }            
            
           
            x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T*)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);

            if(!rData.fgLock)
            {
                u4DTVPhyCh = i;
                u4DTVProgAtPhyCh = 0;
                VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
                continue;
            }
            else if(rData.fgLock && (u4DTVSystem == NAV_DTV_SYSTEM_DVBC))
            {
                x_memcpy((void *)&(rDVBCTunerData[i]), (void *)&rData, sizeof(rData));
            }
            
#endif
            
        }
        
        u1ChNum = NAV_DTV_PROG_MAX_NUM;

#if 0 //ndef  CC_NAV_DEMOD_DVBT  // to remove VCT parsing for ATSC
        if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
        {
            u4Ret = (UINT32)PSIPSR_ParseTVCT(prVctTbl, &u1ChNum, prTsTbl, u4Timeout, 0);
        }
        else
        {

            u4Ret = (UINT32)PSIPSR_ParseCVCT(prVctTbl, &u1ChNum, prTsTbl, u4Timeout, 0);
        }
#else
	u4Ret = (UINT32)E_PSIPSR_TIMEOUT;
#endif

        if (u4Ret == (UINT32)E_PSIPSR_OK)
        {
            INT32 i4I;

            arTsTbl[_u4TotalTsNum].u1PhyCh = i;
                    _u4TotalTsNum++;
                    prTsTbl++;


            LOG(6, "VCT parse done---------------------\n");
            LOG(6, "tsId=0x%02x, version=%d, chNum=%d\n", prTsTbl->u2TsId, prTsTbl->u1Version, u1ChNum);

            for (i4I = 0; i4I < (INT32)u1ChNum; i4I++, u4NumDTVProg++, u4DTVProgAtPhyCh++)
            {
                PSIPSR_TBL_VCT_T* prTemp = &arVctTbl[u4NumDTVProg];

                prVctTbl->u2PhyFreq = i;
                prVctTbl++;

                LOG(6, "[%2d.%d %7s] tsId=0x%02x, prog=%d, pcr=0x%02x, v=0x%02x, a1=0x%02x, a2=0x%02x\n",
                    prTemp->u2MajorChNum, prTemp->u2MinorChNum, prTemp->szShortName,
                    prTemp->u2ChTsId, prTemp->u2ProgNum, prTemp->u2PCR,
                    prTemp->u2VPID, prTemp->u2APID1, prTemp->u2APID2);

            }

        }
            // if SLD does not match try PAT and PMT
        else if(u4Ret == (UINT32)E_PSIPSR_SLD_NOT_MATCH)
        {

            //PSIPSR_TBL_PMT_T* parPMTTbl;
            UINT32 u4ProgNum;
            LOG(6, "_ScanChannel: no SLD in VCT, parse PAT/PMT\n");

            u4ProgNum = NAV_DTV_PROG_MAX_NUM;
            u4Ret = (UINT32)PSIPSR_ParsePSI(arPMTTbl, &u4ProgNum, u4Timeout, 0);

            if (u4Ret == (UINT32)E_PSIPSR_OK)
            {
                arTsTbl[_u4TotalTsNum].u1PhyCh = i;
                _u4TotalTsNum++;
                prTsTbl++;

                for (j=0; j<u4ProgNum; j++)
                {
                    // If there's no video pid and audio pid, we exclude the program
                    if(arPMTTbl[j].u4VPID != 0x0 || arPMTTbl[j].u4APID1 != 0x0)
                    {
                        prVctTbl++;
                        arVctTbl[u4NumDTVProg].u2MajorChNum = i;
                        arVctTbl[u4NumDTVProg].u2MinorChNum = j + 1;
                        VERIFY(x_strcpy((CHAR*)(arVctTbl[u4NumDTVProg].szShortName), "No VCT\0") != NULL);

                        arVctTbl[u4NumDTVProg].u2VPID = arPMTTbl[j].u4VPID;
                        arVctTbl[u4NumDTVProg].u4VPIDType = arPMTTbl[j].u4VPIDType;
                        arVctTbl[u4NumDTVProg].u2APID1 = arPMTTbl[j].u4APID1;
                        arVctTbl[u4NumDTVProg].u2APID1Type = arPMTTbl[j].u4APID1Type;
                        arVctTbl[u4NumDTVProg].u2APID2 = arPMTTbl[j].u4APID2;
                        arVctTbl[u4NumDTVProg].u2APID2Type = arPMTTbl[j].u4APID2Type;
                        arVctTbl[u4NumDTVProg].u2APID3 = arPMTTbl[j].u4APID3;
                        arVctTbl[u4NumDTVProg].u2APID3Type = arPMTTbl[j].u4APID3Type;
                        arVctTbl[u4NumDTVProg].u2PCR = arPMTTbl[j].u4PCR;
                        arVctTbl[u4NumDTVProg].u2PhyFreq = i;
                        arVctTbl[u4NumDTVProg].u2ChTsId = 0;
                        _afgQam64[i] = fgQam64;
                        u4DTVProgAtPhyCh++;
                        u4NumDTVProg++;

                    }
                }

                LOG(6, "_ScanChannel: u4ProgNum(PSI)=%d, u1ChNum(VCT)=%d\n",
                        u4ProgNum, u1ChNum);
            }
            else //both parsing vct and parsing pat/pmt fail
            {

                LOG(6, "_ScanChannel: parse PAT/PMT timeout, bad TS\n");
                u4DTVPhyCh = i;
                u4DTVProgAtPhyCh = 0;
                VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
                continue;
            }
        }
        // if timeout, try PAT and PMT
        else if(u4Ret == (UINT32)E_PSIPSR_TIMEOUT)
        {
            //PSIPSR_TBL_PMT_T* parPMTTbl;
            UINT32 u4ProgNum = 0;
            LOG(6, "_ScanChannel: parse VCT timeout, parse PAT/PMT\n");

            u1ChNum = NAV_DTV_PROG_MAX_NUM;

            x_memset(arPMTTbl, 0, sizeof(arPMTTbl));
            u4Ret = (UINT32)PSIPSR_ParsePSI(arPMTTbl, &u1ChNum, u4Timeout, 0);

            if (u4Ret == (UINT32)E_PSIPSR_OK)
            {
                arTsTbl[_u4TotalTsNum].u1PhyCh = i;

                _u4TotalTsNum++;
                prTsTbl++;


                for (j=0; j<u1ChNum; j++)
                {
                    // If there's no video pid and audio pid, we exclude the program
                    if(arPMTTbl[j].u4VPID != 0x0 || arPMTTbl[j].u4APID1 != 0x0)
                    {
                        prVctTbl++;

                        arVctTbl[u4NumDTVProg].u2MajorChNum = i;
                        arVctTbl[u4NumDTVProg].u2MinorChNum = j + 1;
                        VERIFY(x_strcpy((CHAR*)(arVctTbl[u4NumDTVProg].szShortName), "No VCT\0") != NULL);
                        arVctTbl[u4NumDTVProg].u2VPID = arPMTTbl[j].u4VPID;
                        arVctTbl[u4NumDTVProg].u4VPIDType = arPMTTbl[j].u4VPIDType;     
                        arVctTbl[u4NumDTVProg].u2APID1 = arPMTTbl[j].u4APID1;
                        arVctTbl[u4NumDTVProg].u2APID1Type = arPMTTbl[j].u4APID1Type;
                        arVctTbl[u4NumDTVProg].u2APID2 = arPMTTbl[j].u4APID2;
                        arVctTbl[u4NumDTVProg].u2APID2Type = arPMTTbl[j].u4APID2Type;
                        arVctTbl[u4NumDTVProg].u2APID3 = arPMTTbl[j].u4APID3;
                        arVctTbl[u4NumDTVProg].u2APID3Type = arPMTTbl[j].u4APID3Type;
                        arVctTbl[u4NumDTVProg].u2PCR = arPMTTbl[j].u4PCR;
                        arVctTbl[u4NumDTVProg].u2PhyFreq = i;
                        arVctTbl[u4NumDTVProg].u2ChTsId = 0;
                        _afgQam64[i] = fgQam64;
                        
                        u4DTVProgAtPhyCh++;
                        u4NumDTVProg++;
                    }
                }

                LOG(6, "_ScanChannel: u4ProgNum(PSI)=%d, u1ChNum(VCT)=%d\n",
                        u4ProgNum, u1ChNum);
            }
            else //both parsing vct and parsing pat/pmt fail
            {
                LOG(6, "_ScanChannel: parse PAT/PMT timeout, bad TS\n");
                u4DTVPhyCh = i;
                u4DTVProgAtPhyCh = 0;
                VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
                continue;
            }


        }
        u4DTVPhyCh = i;
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);


    }

    rCurrDTVProg.u4VctTblIdx = 0;
    LOG(6, "Total %d programs found\n", u4NumDTVProg);
    for(i = 0; i < u4NumDTVProg; i++)
    {
        LOG(6, "[%d] V-Pid: %d, A-Pid: %d, phy freq: %d\n", i, arVctTbl[i].u2VPID, arVctTbl[i].u2APID1,
            arVctTbl[i].u2PhyFreq);
    }

    _tScanCrit = x_crit_start();
    _fgBreakScan = TRUE;
    x_crit_end(_tScanCrit);

    _SortVCT();
    if(_fgSCStatistic)
    {
        _PrintDTVProgInfo();
    }
    rCurrDTVProg.u4VctTblIdx = 0;

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);


    NAV_UnlockAVMutex(u4VdpId);

    VERIFY(_PlayAVDTV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);

    t_crit_scanchannel = x_crit_start();
    fgDTVChannelScanning = FALSE;
    //VERIFY(WIDGET_SetRepeat(1) == (INT32)E_NAV_MSG_OK);
    x_crit_end(t_crit_scanchannel);
    return (INT32)E_NAV_MSG_OK;

}

#ifdef CC_NAV_DEMOD_DVBT
//-------------------------------------------------------------------------
/** _AudNfyFct  
 *  TV audio system detection notification
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _AudNfyFct (INT32 u4Tag)
{
    DRV_TV_SYS_T rTvSys;
    DRV_SOUND_SYS_T rSoundSys;
    NAV_ATVAUD_MSG_T rMsg;
    
    DRV_AUD_GetTvSys(&rTvSys, &rSoundSys);       
    rMsg.u4TvSys = rTvSys;
    rMsg.u4SoundSys= rSoundSys;
    x_msg_q_send(_hATVAudMsgq, (const void*)&rMsg, sizeof(NAV_ATVAUD_MSG_T), 254);
    
}
//-------------------------------------------------------------------------
/** _AudSysProc
 *  Save detected TV audio system parameters
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _AudSysProc(void)
{

    UINT16 u2MsgIdx;
    UINT32 u4Size = sizeof(NAV_ATVAUD_MSG_T);
    NAV_ATVAUD_MSG_T rMsg;
    //DRV_TV_SYS_T rTvSys;
    //DRV_SOUND_SYS_T rSoundSys;
    if(rNAVOption.fgEnableAudio) 
    {
        DRV_AUD_DetectTvSys();
        VERIFY(x_msg_q_receive(&u2MsgIdx, (void*)&rMsg, (SIZE_T*)&u4Size, &_hATVAudMsgq, 1, X_MSGQ_OPTION_WAIT) == OSR_OK); 
        if(rMsg.u4TvSys == DRV_TV_SYS_UNKNOWN)
        {
            return -1024;
        }
    }
    else 
    {
        rMsg.u4TvSys = DRV_TV_SYS_B_G;
        rMsg.u4SoundSys = DRV_SOUND_SYS_FM_A2;
    }

    arATVTbl[u4NumATVProg].u4TvSys = rMsg.u4TvSys;
    arATVTbl[u4NumATVProg].u4SoundSys = rMsg.u4SoundSys;

    return 0;
}
#endif
//-------------------------------------------------------------------------
/** _AudNfyFct  
 *  TV audio system detection notification
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _AudCtrlDoneNfyFct (INT32 u4Tag)
{
    UINT32 u4Msg = 0xFF;
    x_msg_q_send(_hAudCtrlMsgq, (const void*)&u4Msg, sizeof(UINT32), 254);    
    
}

//-------------------------------------------------------------------------
/** _ScanATVChannel
 *  The real thread to perform analog channel scan
 *  @param  eSrc        Signal source
 *  @retval   E_NVGT_OK, E_NVGT_ERROR
 */
//-------------------------------------------------------------------------

static INT32 _ScanATVChannel(UINT32 u4Src)
{
    UINT32 i;
    UINT32 u4End = 0;
    //UINT16 u2TempFreq;
    UINT32* pu4ChTbl;
    //UINT32 u4SearchMethod;
    UINT32 u4VdpId = u4ActiveVDP;

    DRV_TUNER_DATA_T rData;
#ifdef CC_NAV_DEMOD_DVBT    
    DRV_AUD_PARAM_T rParam;
#endif    
    UNUSED(u4Src);
    UNUSED(u4End);
    UNUSED(pu4ChTbl);
    NAV_LockAVMutex(u4VdpId);

    u4NumATVProg = 0;

#ifndef CC_NAV_DEMOD_DVBT
    if(u4ATVSystem == NAV_ATV_SYSTEM_CABLE)
    {
        u4End = sizeof(au4NTSCCABLEChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4NTSCCABLEChannel;
    }
    else
    {
        u4End = sizeof(au4NTSCRFChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4NTSCRFChannel;
    }
#else

    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_BG)
    {
        u4End = sizeof(au4PalBGChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4PalBGChannel;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_DK)
    {
        u4End = sizeof(au4PalDKChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4PalDKChannel;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_I)
    {
        u4End = sizeof(au4PalIChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4PalIChannel;

    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L)
    {
        u4End = sizeof(au4SECAMLChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4SECAMLChannel;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L1)
    {
        u4End = sizeof(au4SECAML1Channel)/sizeof(UINT32)-1;
        pu4ChTbl = au4SECAML1Channel;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_NTSC_M)
    {
        u4End = sizeof(au4NTSCMChannel)/sizeof(UINT32)-1;
        pu4ChTbl = au4NTSCMChannel;
    }

    _fgAutoSearch = TRUE;

    rParam.rCodingType = DRV_AUD_CT_DETECTOR;
    rParam.rInputSrc = DRV_IS_ATUNER_1;
    rParam.pfnDetectDone = _AudNfyFct;
	 rParam.pfnPlayDone = _AudCtrlDoneNfyFct;
      rParam.pfnStopDone = _AudCtrlDoneNfyFct;
    DRV_AUD_SetParameter(&rParam);
#endif

    UNUSED(bApiVideoSetVideoSrc(u4VdpId, _au4SourceIdx2VS[E_NAV_VS_TUNER1])); // make channel scan function correctly
    LOG(7, "Start NPTV channel scan\n");

#if !defined(CC_NAV_DEMOD_DVBT) || defined(DRV_IF_BR)
	if(!_fgCliATVSearch)
	{
	   _i4ATVChStart =1;
	   _i4ATVChEnd = u4End;
	   
	}
    for(i = _i4ATVChStart-1; i <= _i4ATVChEnd; i++)
    {
        _tScanCrit = x_crit_start();
        if(_fgBreakScan)
        {
            x_crit_end(_tScanCrit);
            break;
        }
        x_crit_end(_tScanCrit);

        u4ATVPhyCh = i;
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

        LOG(5, "[NAV_AV] Set tuner cb tag value %d\n", i);        
        DRV_TUNER_Search(&pu4ChTbl[i], DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], pu4ChTbl[i], (VOID*)i);
        {
            UINT16 u2Idx;
            SIZE_T u4Size = sizeof(DRV_TUNER_DATA_T);                              
            x_msg_q_receive(&u2Idx, (void*)&rData, &u4Size, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);       
        }

        x_thread_delay(500);


        if(rData.fgLock)                         
        {
            arATVTbl[u4NumATVProg].u2FreqTabIdx = i;
            u4NumATVProg++;                      
        }
        else
        {
            // Nothing
        }
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    }
#else  
    {
        UINT32 u4Freq = NAV_ATD_START_FREQ;      
        UINT32 u4FreqBound = NAV_ATD_START_FREQ + NAV_ATD_FREQ_NEXT_STEP;
        if(_fgCliATVSearch)
		{
		   u4Freq = _i4ATVChStart;
		   u4FreqBound = _i4ATVChStart + NAV_ATD_FREQ_NEXT_STEP ;
		}
        while(1)
        {    
            _tScanCrit = x_crit_start();
            if(_fgBreakScan)
            {
                x_crit_end(_tScanCrit);
                break;
            }
            x_crit_end(_tScanCrit);
    
            u4ATVPhyCh = u4Freq - 1;
            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    
            LOG(5, "[NAV_AV] Set tuner cb tag value %d\n", i);
            DRV_TUNER_Search(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], u4FreqBound, (VOID*)i);
            {
                UINT16 u2Idx;
                SIZE_T u4Size = sizeof(DRV_TUNER_DATA_T);                              
                x_msg_q_receive(&u2Idx, (void*)&rData, (SIZE_T *)&u4Size, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);       
            }

            Printf("Freq: %d -- %d\n", u4Freq, u4FreqBound);
            
            if(u4Freq > NAV_ATD_END_FREQ)
            {
                break;
            }
			 if((_fgCliATVSearch)&&(u4Freq > _i4ATVChEnd))
            {
                break;
            }
            //x_thread_delay(500);
    
            if(rData.fgLock && (_AudSysProc() != DRV_TV_SYS_UNKNOWN))
            {
                u4Freq = rData.u4Freq * 1000;
                arATVTbl[u4NumATVProg].u4Freq = u4Freq;

                //recovery format detected by Aud if it is SECAM or NTSC M system
                if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L || u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L1)
                {
                    if(u4Freq >= 100 * 1000 * 1000)
                    {
                        arATVTbl[u4NumATVProg].u4TvSys = DRV_TV_SYS_L;
                    }   
                    else
                    {
                        arATVTbl[u4NumATVProg].u4TvSys = DRV_TV_SYS_L1;
                    }
                }
                else if(u4ATVSystem == NAV_ATV_SYSTEM_NTSC_M)
                {
                    arATVTbl[u4NumATVProg].u4TvSys = DRV_TV_SYS_M_N;
                }

                u4NumATVProg++;
                u4Freq += NAV_ATD_FREQ_MIDDLE_STEP;
            }
            else
            {
                // Nothing
				u4Freq = u4Freq + NAV_ATD_FREQ_NEXT_STEP;
            }
            u4FreqBound = u4Freq + NAV_ATD_FREQ_NEXT_STEP;

            VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
        }
    }    
#endif	


    _fgAutoSearch = FALSE;

    _tScanCrit = x_crit_start();
    _fgBreakScan = TRUE;
    x_crit_end(_tScanCrit);

    if(_fgSCStatistic)
    {
        _PrintATVProgInfo();
    }

    rCurrATVProg.u4ATVTblIdx = 0;

    NAV_UnlockAVMutex(u4VdpId);

    VERIFY(_PlayAVATV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);

    t_crit_scanchannel = x_crit_start();
    fgATVChannelScanning = FALSE;
    //VERIFY(WIDGET_SetRepeat(1) == (INT32)E_NAV_MSG_OK);
    x_crit_end(t_crit_scanchannel);
    
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
	x_msg_q_delete(_hATVAudMsgq);
    return (INT32)E_NAV_MSG_OK;

}

//-------------------------------------------------------------------------
/** _SetSrc
 *  Switch input source of main or sub path
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetSrc(UINT32 u4VdpId, UINT32 u4NewSrc)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    VERIFY(_StopAV(u4VdpId, arVideoPath[u4VdpId].u4Src) == (INT32)E_NAV_MSG_OK);
    arVideoPath[u4VdpId].u4Src = u4NewSrc;
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    VERIFY(_PlayAV(u4VdpId, u4NewSrc) != (INT32)E_NAV_MSG_ERROR);

    return (INT32)E_NAV_MSG_OK;
}

static void _TunerSetBreak()
{
    DRV_TUNER_SetBreak();
}

//-------------------------------------------------------------------------
/** _PlayAV
 *  Entry function for playing A/V of each input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAV(UINT32 u4VdpId, UINT32 u4Src)
{
    VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);

  #ifdef __MODEL_slt__
    DRV_VDO_Unmute(u4VdpId);
  #endif

    if(u4Src == (UINT32)E_NAV_VS_MAX)
    {    
        //UNUSED(bApiVideoSetVideoSrc(u4VdpId, _au4SourceIdx2VS[(UINT32)E_NAV_VS_MAX]));    
        return (INT32)E_NAV_MSG_NOT_SUPPORT;        
    }
    else if(u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        return(_PlayAVATV(u4VdpId));
    }
    else if(u4Src != (UINT32)E_NAV_VS_DT1 && u4Src != (UINT32)E_NAV_VS_DT2)
    {
        return(_PlayAVAnalogInput(u4VdpId, u4Src));
    }
    else if(u4Src == (UINT32)E_NAV_VS_DT1)
    {
        return(_PlayAVDTV(u4VdpId));
    }
    // DT2: Multimedia  
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        //return(_PlayAVMM(u4VdpId));
    }  
#ifdef CC_YES_1394    
    // DT2: I1394
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        return(_PlayAVI1394(u4VdpId));
    }
#endif    

	return E_NAV_MSG_NOT_SUPPORT;
}


//-------------------------------------------------------------------------
/** _PlayAV
 *  Entry function for playing A/V of each input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAudio(UINT32 u4VdpId, UINT32 u4Src)
{
    VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);

  #ifdef __MODEL_slt__
    DRV_VDO_Unmute(u4VdpId);
  #endif

    if(u4Src == (UINT32)E_NAV_VS_MAX)
    {    
        //UNUSED(bApiVideoSetVideoSrc(u4VdpId, _au4SourceIdx2VS[(UINT32)E_NAV_VS_MAX]));    
        return (INT32)E_NAV_MSG_NOT_SUPPORT;        
    }
    else if(u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        return(_PlayAudioATV(u4VdpId));
    }
    else if(u4Src != (UINT32)E_NAV_VS_DT1 && u4Src != (UINT32)E_NAV_VS_DT2)
    {
        return(_PlayAudioAnalogInput(u4VdpId, u4Src));
    }
    else if(u4Src == (UINT32)E_NAV_VS_DT1)
    {
        return(_PlayAudioDTV(u4VdpId));
    }
    // DT2: Multimedia  
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        //return(_PlayAVMM(u4VdpId));
    }  
#ifdef CC_YES_1394    
    // DT2: I1394
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        return(_PlayAVI1394(u4VdpId));
    }
#endif    

	return E_NAV_MSG_NOT_SUPPORT;
}


UINT32 _bDVIAudio =FALSE;
void  _SetDVIAudio(UINT32 u4VdpId)
{
    DRV_ERR_CODE_T rRetCode;
	
	if(rNAVOption.fgEnableAudio && (u4VdpId == NAV_VDP_MASTER))
    {
			
       if((fgHDMISupportAudio()==0)&&(!_bDVIAudio))//DVI mode
        {
        DRV_AUD_Stop();  
			{
				UINT16 u2MsgIdx = 0;
				UINT32 u4Msg;
				UINT32 u4Size = sizeof(UINT32);
		
				if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
				{
					VERIFY(0);					  
				}
				if(u4Msg != 0xFF)
				{
					VERIFY(0);					  
				}
			}		   
	
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;
        rParam.pfnStopDone = _AudCtrlDoneNfyFct;
        rParam.pfnDetectDone = NULL;
        rParam.rCodingType = DRV_AUD_CT_PCM;
        rParam.rInputSrc = DRV_IS_VGA_1;                    		
        _bDVIAudio=TRUE;
        DRV_AUD_SetParameter(&rParam);
		rRetCode = DRV_AUD_Start();
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
           
    	  }  		
		}
	   else if((fgHDMISupportAudio()==1)&&(_bDVIAudio)) //Hdmi mode
	   	{
	   	DRV_AUD_Stop();  
			{
				UINT16 u2MsgIdx = 0;
				UINT32 u4Msg;
				UINT32 u4Size = sizeof(UINT32);
		
				if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
				{
					VERIFY(0);					  
				}
				if(u4Msg != 0xFF)
				{
					VERIFY(0);					  
				}
			}		   
	
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;
        rParam.pfnStopDone = _AudCtrlDoneNfyFct;
        rParam.pfnDetectDone = NULL;
		
	   	_bDVIAudio=FALSE;
	    rParam.rCodingType = DRV_AUD_CT_PCM;
        rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_HDMI_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_HDMI);

        DRV_AUD_SetParameter(&rParam);
		rRetCode = DRV_AUD_Start();
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
           
    	  }  
	   	}
	   
    }	
  
}

//-------------------------------------------------------------------------
/** _PlayAVAnalogInput
 *  Entry function for playing analog input except tuner
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAVAnalogInput(UINT32 u4VdpId, UINT32 u4Src)
{
    DRV_ERR_CODE_T rRetCode;
    DRV_VDO_PARAM_T rParam;
    UINT32 u4Unit;
    rParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[u4Src];
    rParam.u4BackgroundColor = 0x0;
    rParam.rVideoWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
    rParam.rVideoWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
    rParam.rVideoWindow.u4Width= arVideoPath[u4VdpId].rVDPRegion.u4Width;
    rParam.rVideoWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;
    rParam.rDispFmt = DRV_VDO_DISP_FMT_AUTO;    
    if(u4VdpId == 0)
    {
        rParam.rCallbackList.pfnResChange = _SigCallbackMain;
    }
    else
    {
        rParam.rCallbackList.pfnResChange = _SigCallbackSub;    
    }
    rParam.rCallbackList.pfnMpegCcData = NULL;
    rParam.rCallbackList.pfnScartStatus = _ScartProc;
    rParam.rCallbackList.pfnVbiCcData = NULL;
    rParam.rCallbackList.pfnVbiTtxData = NULL;
    rParam.rCallbackList.pfnVbiVpsData = NULL;
    rParam.rCallbackList.pfnVbiWssData = NULL;
    rParam.rCallbackList.pfnVgaAuto = _VgaAutoNfy;
    rParam.rCallbackList.pfnVgaAutocolor = _VgaAutocolorNfy;       

    //Special case for SCART's aspect ratio
    switch(u4Src) 
    {
    case E_NAV_VS_SCART:
        if(u4ScartDispalyFormat == DRV_VDO_SCART_FS_ASPECT_4_3) 
        {
            //Aspect_16_9 to ASPECT_4_3
            u4Unit = rParam.rVideoWindow.u4Width/8;
            rParam.rVideoWindow.u4Left = rParam.rVideoWindow.u4Left + u4Unit;
            rParam.rVideoWindow.u4Width= rParam.rVideoWindow.u4Width - 2 * u4Unit;           
        }
        break;
    case E_NAV_VS_SCART2:
        if(u4Scart2DispalyFormat == DRV_VDO_SCART_FS_ASPECT_4_3) 
        {
            //Aspect_16_9 to ASPECT_4_3
            u4Unit = rParam.rVideoWindow.u4Width/8;
            rParam.rVideoWindow.u4Left = rParam.rVideoWindow.u4Left + u4Unit;
            rParam.rVideoWindow.u4Width= rParam.rVideoWindow.u4Width - 2 * u4Unit;               
        }
        break;
    }

    DRV_VDO_SetParameter(u4VdpId, &rParam);
    DRV_VDO_Start(u4VdpId);
    
#ifdef __MODEL_slt__
    NAV_VQ_Reset(u4VdpId);    
#endif  

#ifndef __MODEL_slt__
    if(u4VdpId == u4ActiveVDP)
    {
    
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;
        rParam.pfnStopDone = _AudCtrlDoneNfyFct;
        rParam.pfnDetectDone = NULL;
        switch(arVideoPath[u4VdpId].u4Src)
        {
            case E_NAV_VS_CVBS1:
            case E_NAV_VS_CVBS2:
            case E_NAV_VS_CVBS3:
            case E_NAV_VS_CVBS4:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_CVBS_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_CVBS1);
                
                break;
            case E_NAV_VS_S1:
            case E_NAV_VS_S2:
            case E_NAV_VS_S3:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_SVIDEO_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_S1);                    		
                
                break;
            case E_NAV_VS_YPbPr1:
            case E_NAV_VS_YPbPr2:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_YPBPR_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_YPbPr1);
                
                break;
            case E_NAV_VS_VGA:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = DRV_IS_VGA_1;                    		

                break;
            case E_NAV_VS_HDMI:
            case E_NAV_VS_HDMI2:
            case E_NAV_VS_HDMI3:
            case E_NAV_VS_HDMI4:
            case E_NAV_VS_HDMI5:            	
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_HDMI_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_HDMI);
                
				_bDVIAudio =FALSE;
                
                break;
            case E_NAV_VS_SCART:
            case E_NAV_VS_SCART2:                	
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_SCART_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_SCART);
                
                break;    
            default:
                VERIFY(0);
        }

        if(rNAVOption.fgEnableAudio) 
        {
            DRV_AUD_SetParameter(&rParam);
            rRetCode = DRV_AUD_Start();
        }
                
	    DRV_TVE_Stop(0);
		DRV_TVE_Stop(1);//tenny
		#if defined(CC_MT5365) ||defined(CC_MT5395)
	    if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE && !_IsMainSrc3D()) 
	    #else
		if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
		#endif
	    {
            DRV_TVE_SetSource(0, _au4SourceIdx2VS[u4Src]);
            rRetCode = DRV_TVE_Start(0);
            DRV_TVE_SetSource(1, _au4SourceIdx2VS[u4Src]);
            rRetCode = DRV_TVE_Start(1);
	    }

        if(rNAVOption.fgEnableAudio && rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
           
    	}     
    }
#endif    

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _PlayAVAnalogInput
 *  Entry function for playing analog input except tuner
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAudioAnalogInput(UINT32 u4VdpId, UINT32 u4Src)
{
    DRV_ERR_CODE_T rRetCode;

#ifndef __MODEL_slt__
    {
    
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;
        rParam.pfnStopDone = _AudCtrlDoneNfyFct;
        rParam.pfnDetectDone = NULL;
        switch(arVideoPath[u4VdpId].u4Src)
        {
            case E_NAV_VS_CVBS1:
            case E_NAV_VS_CVBS2:
            case E_NAV_VS_CVBS3:
            case E_NAV_VS_CVBS4:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_CVBS_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_CVBS1);
                
                break;
            case E_NAV_VS_S1:
            case E_NAV_VS_S2:
            case E_NAV_VS_S3:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_SVIDEO_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_S1);                    		
                
                break;
            case E_NAV_VS_YPbPr1:
            case E_NAV_VS_YPbPr2:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_YPBPR_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_YPbPr1);
                
                break;
            case E_NAV_VS_VGA:
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = DRV_IS_VGA_1;                    		

                break;
            case E_NAV_VS_HDMI:
            case E_NAV_VS_HDMI2:
            case E_NAV_VS_HDMI3:
            case E_NAV_VS_HDMI4:
            case E_NAV_VS_HDMI5:            	
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_HDMI_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_HDMI);
                
				_bDVIAudio =FALSE;
                
                break;
            case E_NAV_VS_SCART:
            case E_NAV_VS_SCART2:                	
                rParam.rCodingType = DRV_AUD_CT_PCM;
                rParam.rInputSrc = (DRV_INPUT_SRC_T)(DRV_IS_SCART_1 + arVideoPath[u4VdpId].u4Src - (UINT32)E_NAV_VS_SCART);
                
                break;    
            default:
                VERIFY(0);
        }

        if(rNAVOption.fgEnableAudio) 
        {
            DRV_AUD_SetParameter(&rParam);
            rRetCode = DRV_AUD_Start();
        }

        if(rNAVOption.fgEnableAudio && rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            } 
    	}     
    }
#endif    

    return (INT32)E_NAV_MSG_OK;
}


#ifdef CC_YES_1394
//-------------------------------------------------------------------------
/** _PlayAVI1394
 *  Switching to I1394. For I1394, we do neither detect nor play immediately after input source switching to it
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAVI1394(UINT32 u4VdpId)
{
    //INT32 i4Ret;
    WIDGET_MENULIST_T* prList = arWidgetOSD[0].prList;
    WIDGET_MENU_T* prMenu;

    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    arVideoPath[u4VdpId].u4Src = (UINT32)E_NAV_VS_DT2;
    arVideoPath[u4VdpId].fgValid = TRUE;

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    prMenu = WIDGET_LookupMenu(prList, "i1394");
    prMenu->fgEnable = TRUE;
    prMenu->prCurr = NULL;
    prList->prCurr = prMenu;
    NAV_EVENT_SetMenuStatus(FALSE);

    VERIFY(WIDGET_ShowMenuList(prList) == (INT32)E_NAV_MSG_OK);
    //i4Ret = NAV_I1394_DetectDevice();

    //return i4Ret;
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _StopAVI1394
 *  Stop IEEE 1394. If the IEEE 1394 device is playing, forward, or reward, calling this will stop it
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAVI1394(UINT32 u4VdpId)
{
    INT32 i4Ret;
    WIDGET_MENULIST_T* prList = arWidgetOSD[0].prList;
    WIDGET_MENU_T* prMenu;

    UNUSED(u4VdpId);

    i4Ret = NAV_I1394_StopDevice();

    // When leave i1394 input src, we must disable i1394 menu
    prMenu = WIDGET_LookupMenu(prList, "i1394");
    prMenu->fgEnable = FALSE;
    prMenu->prCurr = NULL;
    prMenu = prList->prCurr = prList->prHead;
    NAV_EVENT_SetMenuStatus(FALSE);


    // If there isn't any enabled menu, it will loop forever
    while(!prMenu->fgEnable)
    {
        prMenu = prMenu->prNext;
    }
    VERIFY(WIDGET_DrawMenuList(prList, prList->prParentOSD) == (INT32)E_NAV_MSG_OK);
    VERIFY(WIDGET_Flush(prList->prParentOSD)  == (INT32)E_NAV_MSG_OK);
    return i4Ret;

}
#endif

//-------------------------------------------------------------------------
/** _StopAVAnalogInput
 *  Stop analog input source except tuner
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAVAnalogInput(UINT32 u4VdpId)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    NAV_LockAVMutex(u4VdpId);

    if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
    {
        DRV_AUD_Stop();  
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }          
    }

    DRV_VDO_Stop(u4VdpId);


    
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _StopAVAnalogInput
 *  Stop analog input source except tuner
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAudioAnalogInput(UINT32 u4VdpId)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    NAV_LockAVMutex(u4VdpId);

    if(rNAVOption.fgEnableAudio)
    {
        DRV_AUD_Stop();  
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }          
    }

    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _PlayAVATV
 *  Play analog TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAVATV(UINT32 u4VdpId)
{
    UINT32 u4Freq;
    DRV_TUNER_DATA_T rData;
    DRV_ERR_CODE_T rRetCode;
     
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);


    DRV_TVE_Stop(0);
	
    DRV_TVE_Stop(1);//tenny
#if defined(CC_MT5365) ||defined(CC_MT5395)
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE && !_IsMainSrc3D()) 
#else
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
#endif
    {
        DRV_TVE_SetSource(0,_au4SourceIdx2VS[E_NAV_VS_TUNER1]);
        DRV_TVE_Start(0);
		 DRV_TVE_SetSource(1,_au4SourceIdx2VS[E_NAV_VS_TUNER1]);
        DRV_TVE_Start(1);
    }

    NAV_LockAVMutex(u4VdpId);

    UNUSED(bApiVideoSetVideoSrc(u4VdpId, _au4SourceIdx2VS[E_NAV_VS_TUNER1])); // make channel scan function correctly
  
    if(u4NumATVProg == 0)
    {
        LOG(6, "No analog program available!\n");
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
        NAV_UnlockAVMutex(u4VdpId);
        return (INT32)E_NAV_MSG_OK; //return (INT32)E_NAV_MSG_CHANNEL_NOT_SCANNED;
    }

#ifndef CC_NAV_DEMOD_DVBT
    if(u4ATVSystem == NAV_ATV_SYSTEM_CABLE)
    {
    	u4Freq = au4NTSCCABLEChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else
    {
    	u4Freq = au4NTSCRFChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
#else
    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_BG)
    {
    	u4Freq = au4PalBGChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_DK)
    {
    	u4Freq = au4PalDKChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_I)
    {
    	u4Freq = au4PalIChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L)
    {
    	u4Freq = au4SECAMLChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L1)
    {
    	u4Freq = au4SECAML1Channel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_NTSC_M)
    {
    	u4Freq = au4NTSCMChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx];
    }
#endif

#ifndef CC_NAV_DEMOD_DVBT
    if(u4ATVSystem == NAV_ATV_SYSTEM_CABLE)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAC, _arTvSysMap[u4ATVSystem], FALSE, (void*)2);
        au4NTSCCABLEChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, (void*)2);    
        au4NTSCRFChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
#else
/*
    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_BG)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4PalBGChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_DK)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4PalDKChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_PAL_I)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4PalIChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4SECAMLChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_SECAM_L1)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4SECAML1Channel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else    if(u4ATVSystem == NAV_ATV_SYSTEM_NTSC_M)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, _arTvSysMap[u4ATVSystem], FALSE, 2);   
        au4NTSCMChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
*/
    if(u4ATVSystem == NAV_ATV_SYSTEM_NTSC_M)
    {
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAC, _arTvSysMap[u4ATVSystem], FALSE, (void*)2);    
        au4NTSCMChannel[arATVTbl[rCurrATVProg.u4ATVTblIdx].u2FreqTabIdx] = u4Freq;
    }
    else
    {
        DRV_TV_SYS_T rTvSys = (DRV_TV_SYS_T)arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSys;
        u4Freq = arATVTbl[rCurrATVProg.u4ATVTblIdx].u4Freq;
        DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_ANAT, rTvSys, FALSE, (void*)2);   
    }
#endif
    {
        UINT16 u2Idx;
        SIZE_T u4Size = sizeof(DRV_TUNER_DATA_T);                              
        x_msg_q_receive(&u2Idx, (void*)&rData, &u4Size, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
        LOG(5, "[TUNER] Received tuner op done message from TUNER: %d\n", rData.fgLock);            
    }

    {
        DRV_VDO_PARAM_T rParam;
        rParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[E_NAV_VS_TUNER1];
        rParam.u4BackgroundColor = 0x0;
        rParam.rVideoWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
        rParam.rVideoWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
        rParam.rVideoWindow.u4Width= arVideoPath[u4VdpId].rVDPRegion.u4Width;
        rParam.rVideoWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;
        rParam.rDispFmt = DRV_VDO_DISP_FMT_AUTO;
        if(u4VdpId == 0)
        {
            rParam.rCallbackList.pfnResChange = _SigCallbackMain;
        }
        else
        {
            rParam.rCallbackList.pfnResChange = _SigCallbackSub;    
        }
        rParam.rCallbackList.pfnMpegCcData = NULL;
        rParam.rCallbackList.pfnScartStatus = _ScartProc;
        rParam.rCallbackList.pfnVbiCcData = NULL;
        rParam.rCallbackList.pfnVbiTtxData = _VbiTtxNfy;
        rParam.rCallbackList.pfnVbiVpsData = NULL;
        rParam.rCallbackList.pfnVbiWssData = NULL;
        rParam.rCallbackList.pfnVgaAuto = _VgaAutoNfy;
        rParam.rCallbackList.pfnVgaAutocolor = _VgaAutocolorNfy;    
        DRV_VDO_SetParameter(u4VdpId, &rParam);
        DRV_VDO_Start(u4VdpId);  
    }

     // Workaround for display OSD
    {
        DRV_VDO_INFO_T rInfo;
        DRV_VDO_GetInfo(0, &rInfo);
        arVideoPath[u4VdpId].u4ResX = rInfo.u2Width;
        arVideoPath[u4VdpId].u4ResY = rInfo.u2Height;
        arVideoPath[u4VdpId].fgProgressive = rInfo.fgProgressive;
        arVideoPath[u4VdpId].fgValid = TRUE;  
        NAV_DrawInfoOSD();
    }

#ifndef __MODEL_slt__ 
    if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
    {
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;            
        rParam.pfnStopDone = _AudCtrlDoneNfyFct; 
#if !defined(CC_NAV_DEMOD_DVBT ) || defined(DRV_IF_BR)
        rParam.rCodingType = DRV_AUD_CT_MTS;
#else
        //if (_IsMaskSetToMtsSys(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask))
        if(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_BTSC &&
        arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSys == DRV_TV_SYS_M_N)
        {
            rParam.rCodingType = DRV_AUD_CT_MTS;        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2_DK1
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2_DK2)
        {
            rParam.rCodingType = DRV_AUD_CT_A2;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_A2);
            //AUD_DspSetTvSrcMode(_GetTvSysFormat(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask));
        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_MONO 
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_AM 
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_MONO_NICAM)
        {
            rParam.rCodingType = DRV_AUD_CT_PAL;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PAL);
            //AUD_DspSetTvSrcMode(_GetTvSysFormat(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask));
        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_EIA_J)
        {        			    
            rParam.rCodingType = DRV_AUD_CT_FMFM;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_FMFM);
            //AUD_DspSetTvSrcMode(SV_FM_FM);
        
        }
        else
        {
            rParam.rCodingType = DRV_AUD_CT_MTS;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_MTS);
            LOG(1, "Improper audio decoder format\n");
        }
#endif        
        rParam.rInputSrc = DRV_IS_ATUNER_1;
        rParam.pfnDetectDone = NULL;
        DRV_AUD_SetParameter(&rParam);

        DRV_AUD_SetTvSys((DRV_TV_SYS_T)(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSys), (DRV_SOUND_SYS_T)(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys));                    
    
        rRetCode = DRV_AUD_Start();
        
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }                      
    }
#endif     

    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _PlayAVATV
 *  Play analog TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAudioATV(UINT32 u4VdpId)
{


    DRV_ERR_CODE_T rRetCode;
     
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
   
    NAV_LockAVMutex(u4VdpId);
  
    if(u4NumATVProg == 0)
    {
        LOG(6, "No analog program available!\n");
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
        NAV_UnlockAVMutex(u4VdpId);
        return (INT32)E_NAV_MSG_OK; //return (INT32)E_NAV_MSG_CHANNEL_NOT_SCANNED;
    }

#ifndef __MODEL_slt__ 
    if(rNAVOption.fgEnableAudio)
    {
        DRV_AUD_PARAM_T rParam;
        rParam.pfnPlayDone = _AudCtrlDoneNfyFct;            
        rParam.pfnStopDone = _AudCtrlDoneNfyFct; 
#if !defined(CC_NAV_DEMOD_DVBT) || defined(DRV_IF_BR)
        rParam.rCodingType = DRV_AUD_CT_MTS;
#else
        //if (_IsMaskSetToMtsSys(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask))
        if(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_BTSC &&
        arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSys == DRV_TV_SYS_M_N)
        {
            rParam.rCodingType = DRV_AUD_CT_MTS;        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2_DK1
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_A2_DK2)
        {
            rParam.rCodingType = DRV_AUD_CT_A2;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_A2);
            //AUD_DspSetTvSrcMode(_GetTvSysFormat(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask));
        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_MONO 
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_AM 
        || arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_MONO_NICAM)
        {
            rParam.rCodingType = DRV_AUD_CT_PAL;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_PAL);
            //AUD_DspSetTvSrcMode(_GetTvSysFormat(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSysMask, arATVTbl[rCurrATVProg.u4ATVTblIdx].u4AudSysMask));
        
        }
        else if (arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys == DRV_SOUND_SYS_FM_EIA_J)
        {        			    
            rParam.rCodingType = DRV_AUD_CT_FMFM;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_FMFM);
            //AUD_DspSetTvSrcMode(SV_FM_FM);
        
        }
        else
        {
            rParam.rCodingType = DRV_AUD_CT_MTS;
            //AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_ANALOG_TUNER, AUD_FMT_MTS);
            LOG(1, "Improper audio decoder format\n");
        }
#endif        
        rParam.rInputSrc = DRV_IS_ATUNER_1;
        rParam.pfnDetectDone = NULL;
        DRV_AUD_SetParameter(&rParam);

        DRV_AUD_SetTvSys((DRV_TV_SYS_T)(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4TvSys), (DRV_SOUND_SYS_T)(arATVTbl[rCurrATVProg.u4ATVTblIdx].u4SoundSys));                    
    
        rRetCode = DRV_AUD_Start();
        
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);

            INT32 i4Ret;
            i4Ret = x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT);                
            if(i4Ret != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }                      
    }
#endif     

    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _StopAVATV
 *  Stop analog TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAVATV(UINT32 u4VdpId)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    NAV_LockAVMutex(u4VdpId);

    if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
    {
        
        DRV_AUD_Stop();   
        {
            UINT16 u2MsgIdx ;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }  
        
    }
   
    DRV_VDO_Stop(u4VdpId);



    arVideoPath[u4VdpId].fgValid = FALSE;


    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(u4VdpId);;
    return (INT32)E_NAV_MSG_OK;
}


static INT32 _StopAudioATV(UINT32 u4VdpId)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    NAV_LockAVMutex(u4VdpId);

    if(rNAVOption.fgEnableAudio)
    {
        
        DRV_AUD_Stop();   
        {
            UINT16 u2MsgIdx ;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }  
        
    }
   
    NAV_UnlockAVMutex(u4VdpId);;
    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _StopAV
 *  Entry function for stopping A/V
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAV(UINT32 u4VdpId, UINT32 u4Src)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    if(u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        return(_StopAVATV(u4VdpId));
    }
    else if(u4Src < (UINT32)E_NAV_VS_DT1)
    {
        return(_StopAVAnalogInput(u4VdpId));
    }
    else if(u4Src == (UINT32)E_NAV_VS_DT1)
    {
        return(_StopAVDTV(u4VdpId));
    }
#ifdef CC_YES_1394    
    // DT2: I1394
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        return(_StopAVI1394(u4VdpId));
    }
#endif
    else if(u4Src <= (UINT32)E_NAV_VS_SCART2)
    {
        return(_StopAVAnalogInput(u4VdpId));
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

static INT32 _StopAudio(UINT32 u4VdpId, UINT32 u4Src)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    if(u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        return(_StopAudioATV(u4VdpId));
    }
    else if(u4Src < (UINT32)E_NAV_VS_DT1)
    {
        return(_StopAudioAnalogInput(u4VdpId));
    }
    else if(u4Src == (UINT32)E_NAV_VS_DT1)
    {
        return(_StopAudioDTV(u4VdpId));
    }
#ifdef CC_YES_1394    
    // DT2: I1394
    else if(u4Src == (UINT32)E_NAV_VS_DT2)
    {
        return(_StopAVI1394(u4VdpId));
    }
#endif
    else if(u4Src <= (UINT32)E_NAV_VS_SCART2)
    {
        return(_StopAudioAnalogInput(u4VdpId));
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}


static void _TunerStatusCallback(INT32 i4Tag)
{
    BOOL fgStatus;
    NAV_ATVCB_MSG_T rMsg;
    DRV_TUNER_MSG_T rTunerMsg;

    //DRV_TUNER_GetLockStatus(&rStatus, i4Tag);
    DRV_TUNER_GetMessage(i4Tag, &rTunerMsg);

    fgStatus = rTunerMsg.rData.fgLock;
    if(arVideoPath[VDP_1].u4Src == E_NAV_VS_DT1)
    {
        rMsg.u4VdpId = VDP_1;    
    }
    else if(arVideoPath[VDP_2].u4Src == E_NAV_VS_DT1)
    {
        rMsg.u4VdpId = VDP_2;    
    }
    else //current vdp are both not playing E_NAV_VS_DT1
    {
        return; // return directly
    }
    
    rMsg.u4Src = E_NAV_VS_DT1;    
    LOG(6, "[NAV] Tuner signal notify %d\n", rMsg.u4SigStatus);    
    if(!fgStatus)
    {
        rMsg.u4SigStatus = 4;// SV_VDO_NOSIGNAL; 
        VERIFY(x_msg_q_send(_hATVCBMsgq, (const void*)&rMsg, sizeof(NAV_ATVCB_MSG_T), 254) == OSR_OK);                    
    }
    else if(fgStatus)
    {
        rMsg.u4SigStatus = 5; //SV_VDO_STABLE;
        VERIFY(x_msg_q_send(_hATVCBMsgq, (const void*)&rMsg, sizeof(NAV_ATVCB_MSG_T), 254) == OSR_OK);                    
    }
    else
    {
        return;
    }

}

//-------------------------------------------------------------------------
/** _DrawOSDThread 
 *  OSD redraw event listener 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _DrawOSDThread(void)
{
    UINT16 u2MsgIdx, u2MsgCount;
    SIZE_T u4Size = sizeof(NAV_OSD_MSG_T);
    NAV_OSD_MSG_T rMsg;

    while(1)
    {
        VERIFY(x_msg_q_num_msgs(_hOSDMsgq, &u2MsgCount) == OSR_OK);
        if(x_msg_q_receive(&u2MsgIdx, (void*)&rMsg, &u4Size, &_hOSDMsgq, 1, X_MSGQ_OPTION_WAIT) == OSR_OK)
        {
            LOG(6, "OSD Thread Draw!\n");
            UNUSED(_DrawInfoOSD(&rMsg));
        }
    }
}

//-------------------------------------------------------------------------
/** _ATVSigProcThread
 *  Video signal change event listener 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _ATVSigProcThread(void)
{
    UINT16 u2MsgIdx, u2MsgCount;
    SIZE_T u4Size = sizeof(NAV_ATVCB_MSG_T);
    NAV_ATVCB_MSG_T rMsg;

    while(1)
    {
        VERIFY(x_msg_q_num_msgs(_hATVCBMsgq, &u2MsgCount) == OSR_OK);
        if(x_msg_q_receive(&u2MsgIdx, (void*)&rMsg, &u4Size, &_hATVCBMsgq, 1, X_MSGQ_OPTION_WAIT) == OSR_OK)
        {
#ifndef __MODEL_slt__        
            LOG(6, "[NAV] Proc ATV signal notify callback!\n");
            UNUSED(_ATVSigProc(&rMsg));
#endif    
        }
    }
}

#ifdef CC_YES_1394
static void _I1394StatusPollThread(void)
{
    INT32 i4State;
    INT32 i4I1394PrevState = (INT32)TP_STOP;

    while(1)
    {
        NAV_LockAVMutex(NAV_VDP_MASTER);

        if(NAV_I1394_GetState() == 3)
        {
            if((i4State = NAV_I1394_GetMode()) != i4I1394PrevState)
            {
                VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
            }
            i4I1394PrevState = i4State;
        }

        NAV_UnlockAVMutex(NAV_VDP_MASTER);

        x_thread_delay(100);
    }
}
#endif

//-------------------------------------------------------------------------
/**  _NAV_VideoCallback
 *  Callback from demux to mpeg decoder
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
#if 0
BOOL _NAV_VideoCallback(const DMX_PES_MSG_T* prPes)
{
    MPV_PES_INFO_T rMpvPes;

    VERIFY(prPes != NULL);

    switch (prPes->u4FrameType)
    {
    case PIC_TYPE_I:
        rMpvPes.ucPicType = MPV_I_TYPE;
        break;

    case PIC_TYPE_P:
        rMpvPes.ucPicType = MPV_P_TYPE;
        break;

    case PIC_TYPE_B:
        rMpvPes.ucPicType = MPV_B_TYPE;
        break;

    default:
        VERIFY(0);
        break;
    }

    rMpvPes.ucMpvId = prPes->u1DeviceId;
    rMpvPes.ucEsId = prPes->u1Channel;
    rMpvPes.u4FifoStart = prPes->u4BufStart;
    rMpvPes.u4FifoEnd = prPes->u4BufEnd;
    rMpvPes.u4VldReadPtr = prPes->u4FrameAddr;
    rMpvPes.u4PTS = prPes->u4Pts;
    rMpvPes.u4DTS = prPes->u4Dts;
    rMpvPes.fgDtsValid = prPes->fgPtsDts;
    rMpvPes.fgSeqHdr = prPes->fgSeqHeader;
    rMpvPes.fgGop = prPes->fgGop;

    return (MPV_CallBackEsm(&rMpvPes) == 1);
}
#endif

BOOL _ScrambleChgNfy(UINT8 u1Pidx, DMX_NOTIFY_CODE_T eCode,
    UINT32 u4Data, const void* pvNotifyTag)
{
    rCurrDTVProg.u4Scrambled = u4Data;
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

	return FALSE;
}


//-------------------------------------------------------------------------
/** _PlayAVDTV
 *  Play digital TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAVDTV(UINT32 u4VdpId)
{
    DRV_VDO_PARAM_T rVideoParam;
    DRV_AUD_PARAM_T rAudioParam;
    DRV_ERR_CODE_T rRetCode;
    UINT16 u2Chnl;
    
    NAV_LockAVMutex(u4VdpId);
    DRV_TVE_Stop(0);
	
    DRV_TVE_Stop(1);//tenny
#if defined(CC_MT5365) ||defined(CC_MT5395)
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE && !_IsMainSrc3D()) 
#else
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
#endif
    {
        DRV_TVE_SetSource(0, _au4SourceIdx2VS[E_NAV_VS_DT1]);
        DRV_TVE_Start(0); 
		DRV_TVE_SetSource(1, _au4SourceIdx2VS[E_NAV_VS_DT1]);
        DRV_TVE_Start(1);
    }

    if(u4NumDTVProg == 0)
    {

        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

        NAV_UnlockAVMutex(u4VdpId);

        return (INT32)E_NAV_MSG_OK;
    }
    // reset MTS to 0
    u1CurrentMTS = 0;
    u1TotalMTS = 0;    
    // get total MTS
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1 != 0)
    {
        u1TotalMTS++;
    }
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID2 != 0)
    {
        u1TotalMTS++;
    }
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID3 != 0)
    {
        u1TotalMTS++;
    }
    
#ifndef __MODEL_slt__
    if(_u4EnableNIM)
    {
        DRV_TV_SYS_T rTvSys = DRV_TV_SYS_UNKNOWN; // set default value to prevent build error
        UINT32 u4Freq;
#ifndef CC_NAV_DEMOD_DVBT
        if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= ATSC_CH_NS || u2Chnl < 1)
            {
                u2Chnl = 1;
            }
            u4Freq = au4AtscChannel[arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq];
        }
        else
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= CABLE_CH_NS || u2Chnl < 1)
            {
                u2Chnl = 1;
            }            
            u4Freq = au4CableChannel[arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq];
        }
#else
        if(u4DTVSystem == NAV_DTV_SYSTEM_DVBT_UK)
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= DVBTUK_CH_NS || u2Chnl < 21)
            {
                u2Chnl = 21;
            }
            u4Freq = au4DVBTUKChannel[u2Chnl];
        }
        else if(u4DTVSystem == NAV_DTV_SYSTEM_DVBC)
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= DVBC_CH_NS || u2Chnl < 1)
            {
                u2Chnl = 1;
            }
            u4Freq = au4DVBCChannel[u2Chnl];
        }
#ifdef DRV_IF_BR
        else if(u4DTVSystem == NAV_DTV_SYSTEM_ISDBT)
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= ISDBT_CH_NS || u2Chnl < 7)
            {
                u2Chnl = 7;
            }            
            u4Freq = au4ISDBTChannel[u2Chnl];
        }
#endif
        else
        {
            u2Chnl = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq;
            if(u2Chnl >= DVBTTW_CH_NS || u2Chnl < 1)
            {
                u2Chnl = 1;
            }            
            u4Freq = au4DVBTTWChannel[u2Chnl];
        }
#endif            

#ifndef DRV_IF_BR
        u4Freq *= 1000000;
#else
        u4Freq *= 1000;
#endif

#ifndef CC_NAV_DEMOD_DVBT
        if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
        {
            DRV_TUNER_PARAM_T rParam;
            rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
            rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
            rParam.rDigParam.fgSpectrumInversion = TRUE;
            rParam.rDigParam.pfnTuningDone = _TunerDigCb;
            rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
            rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
            rParam.rDigParam.rMod = DRV_TUNER_MOD_VSB_8;
            rParam.rDigParam.u4QamSize = 8;
            rParam.rDigParam.u4SymbolRate = 0x0;        	
            DRV_TUNER_SetParameter(&rParam);
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)1);
            {
                UINT16 u2MsgIdx;
                DRV_TUNER_DATA_T rData;
                UINT32 u4MsgSize = sizeof(DRV_TUNER_DATA_T);
                x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T*)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
            }
        }
        else
        {
            DRV_TUNER_PARAM_T rParam;
            rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
            rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
            rParam.rDigParam.fgSpectrumInversion = TRUE;
            rParam.rDigParam.pfnTuningDone = _TunerDigCb;
            rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
            rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
            
            if(_afgQam64[arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq])
            {
                rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_64;
                rParam.rDigParam.u4QamSize = 64;
            }
            else
            {
                rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_256;
                rParam.rDigParam.u4QamSize = 256;
            }            
            rParam.rDigParam.u4SymbolRate = 0x5057;        	
            
            DRV_TUNER_SetParameter(&rParam);
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGC, rTvSys, TRUE, (void*)1);
            {
                UINT16 u2MsgIdx;
                DRV_TUNER_DATA_T rData;
                UINT32 u4MsgSize = sizeof(DRV_TUNER_DATA_T);
                x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T*)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
            }            
        }
#else
        if(u4DTVSystem == NAV_DTV_SYSTEM_DVBT_UK)
        {        
            DRV_TUNER_PARAM_T rParam;
            rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
            rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
            rParam.rDigParam.fgSpectrumInversion = TRUE;
            rParam.rDigParam.pfnTuningDone = _TunerDigCb;
            rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
            rParam.rDigParam.rBandwidth = DRV_TUNER_BW_8_MHz;
            rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_64;
            rParam.rDigParam.u4QamSize = 64;
            rParam.rDigParam.u4SymbolRate = 0x0;
            DRV_TUNER_SetParameter(&rParam);
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)1);
        }
        else if(u4DTVSystem == NAV_DTV_SYSTEM_DVBC)
        {        
            DRV_TUNER_PARAM_T rParam;
            rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
            rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
            rParam.rDigParam.fgSpectrumInversion = TRUE;
            rParam.rDigParam.pfnTuningDone = _TunerDigCb;
            rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
            rParam.rDigParam.rBandwidth = DRV_TUNER_BW_8_MHz;
            rParam.rDigParam.rMod = DRV_TUNER_MOD_UNKNOWN;
            rParam.rDigParam.u4QamSize = 0; //rDVBCTunerData[u2Chnl].QAMSize;
            rParam.rDigParam.u4SymbolRate = 0; //rDVBCTunerData[u2Chnl].SymbolRate;
            DRV_TUNER_SetParameter(&rParam);
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGC, rTvSys, TRUE, (void*)1);
        }
        else
        {
            DRV_TUNER_PARAM_T rParam;
            rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
            rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
            rParam.rDigParam.fgSpectrumInversion = TRUE;
            rParam.rDigParam.pfnTuningDone = _TunerDigCb;
            rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
            rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
            rParam.rDigParam.rMod = DRV_TUNER_MOD_QAM_64;
            rParam.rDigParam.u4QamSize = 64;
            rParam.rDigParam.u4SymbolRate = 0x0;

            UNUSED(_afgQam64[arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PhyFreq]);
            
            DRV_TUNER_SetParameter(&rParam);
            DRV_TUNER_SetFreq(&u4Freq, DRV_TUNER_CT_DIGT, rTvSys, TRUE, (void*)1);
        }
        
        {
            UINT16 u2MsgIdx;
            DRV_TUNER_DATA_T rData;
            UINT32 u4MsgSize = sizeof(DRV_TUNER_DATA_T);
            x_msg_q_receive(&u2MsgIdx, &rData, (SIZE_T *)&u4MsgSize, &_hTunerMsg, 1, X_MSGQ_OPTION_WAIT);
            arVideoPath[u4VdpId].fgValid = rData.fgLock; //for info osd to display no signal if tuner is not locked
        }
#endif //CC_NAV_DEMOD_DVBT

    }
#endif

    rVideoParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[E_NAV_VS_DT1];
    rVideoParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2VPID;
    rVideoParam.u4VPIDType = arVctTbl[rCurrDTVProg.u4VctTblIdx].u4VPIDType;
    rVideoParam.u2PcrPid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PCR;    
    rVideoParam.u4BackgroundColor = 0x0;
    rVideoParam.rVideoWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
    rVideoParam.rVideoWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
    rVideoParam.rVideoWindow.u4Width= arVideoPath[u4VdpId].rVDPRegion.u4Width;
    rVideoParam.rVideoWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;
    rVideoParam.rDispFmt = DRV_VDO_DISP_FMT_AUTO;    
    if(u4VdpId == 0)
    {
        rVideoParam.rCallbackList.pfnResChange = _SigCallbackMain;
    }
    else
    {
        rVideoParam.rCallbackList.pfnResChange = _SigCallbackSub;    
    }
    rVideoParam.rCallbackList.pfnMpegCcData = NULL;
    rVideoParam.rCallbackList.pfnScartStatus = _ScartProc;
    rVideoParam.rCallbackList.pfnVbiCcData = NULL;
    rVideoParam.rCallbackList.pfnVbiTtxData = NULL;
    rVideoParam.rCallbackList.pfnVbiVpsData = NULL;
    rVideoParam.rCallbackList.pfnVbiWssData = NULL;    
    DRV_VDO_SetScrambleChgNfy((void*)_ScrambleChgNfy);
    DRV_VDO_SetParameter(u4VdpId, &rVideoParam);
    DRV_VDO_Start(u4VdpId);
    
#if 0//def __MODEL_slt__
    {
        extern UINT32 _u4OverscanRatio;
        VDP_REGION_T rSrcRegion;
        rSrcRegion.u4X = _u4OverscanRatio;
        rSrcRegion.u4Y = _u4OverscanRatio;
        rSrcRegion.u4Width = VDP_MAX_REGION_WIDTH - _u4OverscanRatio;
        rSrcRegion.u4Height = VDP_MAX_REGION_HEIGHT - _u4OverscanRatio;        
        //VDP_SetSrcRegion(0, 0, rSrcRegion);        
        //DRV_VDO_SetOverScanRatio(0, _u4OverscanRatio << 1, _u4OverscanRatio << 1);
    }
#endif

#ifndef __MODEL_slt__      
    if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
    {
        rAudioParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1;
        rAudioParam.u2PcrPid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PCR;
        rAudioParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[E_NAV_VS_DT1];

        if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AC3 || 
           arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AC3_EU )
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AC3; // For ATSC
        }
        else if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AAC
			|| arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AAC_BRZ
        	)
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AAC; // For ATSC
        }        
        else
        {
            rAudioParam.rCodingType = DRV_AUD_CT_MPEG;        
        }

        rAudioParam.pfnPlayDone = _AudCtrlDoneNfyFct;            
        rAudioParam.pfnStopDone = _AudCtrlDoneNfyFct;         
        rAudioParam.pfnDetectDone = NULL;             
        DRV_AUD_SetScrambleChgNfy((void*)_ScrambleChgNfy);        
        DRV_AUD_SetParameter(&rAudioParam);
      
        rRetCode = DRV_AUD_Start();
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }       
        DRV_AUD_Unmute();
    }
#endif
    
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(u4VdpId);
    

    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _PlayAVDTV
 *  Play digital TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _PlayAudioDTV(UINT32 u4VdpId)
{

    DRV_AUD_PARAM_T rAudioParam;
    DRV_ERR_CODE_T rRetCode;

    
    NAV_LockAVMutex(u4VdpId);

    if(u4NumDTVProg == 0)
    {

        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

        NAV_UnlockAVMutex(u4VdpId);

        return (INT32)E_NAV_MSG_OK;
    }

    // reset MTS to 0
    u1CurrentMTS = 0;
    u1TotalMTS = 0;    
    // get total MTS
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1 != 0)
    {
        u1TotalMTS++;
    }
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID2 != 0)
    {
        u1TotalMTS++;
    }
    if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID3 != 0)
    {
        u1TotalMTS++;
    }
    
#ifndef __MODEL_slt__      
    if(rNAVOption.fgEnableAudio)
    {
        rAudioParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1;
        rAudioParam.u2PcrPid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PCR;
        rAudioParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[E_NAV_VS_DT1];

        if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AC3 || 
           arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AC3_EU )
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AC3; // For ATSC
        }
        else if(arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AAC
			|| arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type == STREAM_TYPE_AAC_BRZ
        	)
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AAC; // For ATSC
        }        
        else
        {
            rAudioParam.rCodingType = DRV_AUD_CT_MPEG;        
        }

        rAudioParam.pfnPlayDone = _AudCtrlDoneNfyFct;            
        rAudioParam.pfnStopDone = _AudCtrlDoneNfyFct;         
        rAudioParam.pfnDetectDone = NULL;             
        DRV_AUD_SetScrambleChgNfy((void*)_ScrambleChgNfy);        
        DRV_AUD_SetParameter(&rAudioParam);
      
        rRetCode = DRV_AUD_Start();
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }       
        DRV_AUD_Unmute();
    }
#endif

    NAV_UnlockAVMutex(u4VdpId);
    

    return (INT32)E_NAV_MSG_OK;
}


INT32 SwitchDTVMTS(UINT32 u4VdpId)
{
    UINT8 i = 0;
    DRV_AUD_PARAM_T rAudioParam;
    DRV_ERR_CODE_T rRetCode;

    //Find next avaliable MTS
    while(i < 3) 
    {
        i++;
        u1CurrentMTS = (++u1CurrentMTS)%3;
        if(((u1CurrentMTS == 0) && ((arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1) != 0)) ||
           ((u1CurrentMTS == 1) && ((arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID2) != 0)) ||
           ((u1CurrentMTS == 2) && ((arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID3) != 0))
            )
        {
            break;
        }
    }

    //no avaliable audio pid
    if(i >= 3) 
    {
        return (INT32)E_NAV_MSG_OK; 
    }
    
    NAV_LockAVMutex(u4VdpId);

    //Stop current audio
    if(rNAVOption.fgEnableAudio && u4VdpId == NAV_VDP_MASTER)
    {    
        DRV_AUD_Stop();
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }  
    }

#ifndef __MODEL_slt__
  
    
    if(rNAVOption.fgEnableAudio && u4VdpId == NAV_VDP_MASTER)
    {
        UINT16 u2APIDType = 0;                    
        if(u1CurrentMTS == 0)
        {
            rAudioParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1;
            u2APIDType = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID1Type;            
        }
        else if(u1CurrentMTS == 1)
        {
            rAudioParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID2;
            u2APIDType = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID2Type;             
        }
        else if(u1CurrentMTS == 2)
        {
            rAudioParam.u2Pid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID3;
            u2APIDType = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2APID3Type;            
        }

        if(u2APIDType == STREAM_TYPE_AC3 || u2APIDType == STREAM_TYPE_AC3_EU)
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AC3; // For ATSC
        }
        else if(u2APIDType == STREAM_TYPE_AAC
			|| u2APIDType == STREAM_TYPE_AAC_BRZ
        	)
        {
            rAudioParam.rCodingType = DRV_AUD_CT_AAC; // For ATSC
        }        
        else
        {
            rAudioParam.rCodingType = DRV_AUD_CT_MPEG;        
        }
            
        rAudioParam.u2PcrPid = arVctTbl[rCurrDTVProg.u4VctTblIdx].u2PCR;
        rAudioParam.rInputSrc = (DRV_INPUT_SRC_T)_au4SourceIdx2VS[E_NAV_VS_DT1];

        rAudioParam.pfnPlayDone = _AudCtrlDoneNfyFct;            
        rAudioParam.pfnStopDone = _AudCtrlDoneNfyFct;         
        rAudioParam.pfnDetectDone = NULL;             
        DRV_AUD_SetScrambleChgNfy((void*)_ScrambleChgNfy);        
        DRV_AUD_SetParameter(&rAudioParam);
      
        rRetCode = DRV_AUD_Start();
        if(rRetCode == DRV_E_OK)
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }       
        DRV_AUD_Unmute();
    }
#endif

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
    
}


//-------------------------------------------------------------------------
/** _StopAVDTV
 *  Stop digital TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAVDTV(UINT32 u4VdpId)
{

    NAV_LockAVMutex(u4VdpId);

    if(u4NumDTVProg == 0)
    {
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);      
        NAV_UnlockAVMutex(u4VdpId);
        return (INT32)E_NAV_MSG_OK;
    }


    if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
    {    
        DRV_AUD_Stop();
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }  
    }
    
    DRV_VDO_Stop(u4VdpId);
    
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);    
    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _StopAVDTV
 *  Stop digital TV
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _StopAudioDTV(UINT32 u4VdpId)
{

    NAV_LockAVMutex(u4VdpId);

    if(u4NumDTVProg == 0)
    {
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);      
        NAV_UnlockAVMutex(u4VdpId);
        return (INT32)E_NAV_MSG_OK;
    }


    if(rNAVOption.fgEnableAudio)
    {    
        DRV_AUD_Stop();
        {
            UINT16 u2MsgIdx = 0;
            UINT32 u4Msg;
            UINT32 u4Size = sizeof(UINT32);
    
            if(x_msg_q_receive(&u2MsgIdx, (void*)&u4Msg, (SIZE_T*)&u4Size, &_hAudCtrlMsgq, 1, X_MSGQ_OPTION_WAIT) != OSR_OK)
            {
                VERIFY(0);                    
            }
            if(u4Msg != 0xFF)
            {
                VERIFY(0);                    
            }
        }  
    }
    
    NAV_UnlockAVMutex(u4VdpId);

    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _ShowBorder
 *  Show video boarder for currently active video source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_ShowBorder(UINT32 u4VdpId)
{
    WIDGET_RECT_PROPERTY_T rRect;
    _fgBorderOn = TRUE;
    VERIFY(OSD_PLA_Enable(_rBorderOSD.u4Plane, TRUE) == (INT32)DRV_E_OK);

    rRect.u4X = ((arVideoPath[u4VdpId].rVDPRegion.u4Left * TV_WIDTH) >> 2) / VDP_MAX_REGION_WIDTH;
    rRect.u4Y = ((arVideoPath[u4VdpId].rVDPRegion.u4Top * TV_HEIGHT) >> 2) / VDP_MAX_REGION_HEIGHT;
    rRect.u4W = ((arVideoPath[u4VdpId].rVDPRegion.u4Width * TV_WIDTH) >> 2) / VDP_MAX_REGION_WIDTH;
    rRect.u4H = ((arVideoPath[u4VdpId].rVDPRegion.u4Height * TV_HEIGHT) >> 2) / VDP_MAX_REGION_HEIGHT;

    // Workaround for sub scaler calculation 
#ifdef CC_MT5363
if(TV_WIDTH%4!=0)
{
    if(rRect.u4X != 0)
    {
    	rRect.u4X -= 2;
        rRect.u4W -= 2;   	
    }
     if((arVideoPath[u4VdpId].rVDPRegion.u4Height != VDP_MAX_REGION_HEIGHT)&&(ePIPMode == (UINT32)E_NAV_MODE_PIP))
    {    	
        rRect.u4H += 2;
       // rRect.u4W -= 4;
    }
	  if((arVideoPath[u4VdpId].rVDPRegion.u4Height == VDP_MAX_REGION_HEIGHT)&&(ePIPMode == (UINT32)E_NAV_MODE_PIP))
    {    	
        rRect.u4H += 0;
        rRect.u4W -= 6;
    }
}
#else
    //CC_MT5387 case
    if(rRect.u4X != 0)
    {
    	rRect.u4X -= 2;
        //rRect.u4W += 2;   	
    }
    if(arVideoPath[u4VdpId].rVDPRegion.u4Height != VDP_MAX_REGION_HEIGHT)
    {    	
        rRect.u4H += 2;
        rRect.u4W += 2;
    }
#endif
    rRect.u4Color = rInfoOSDFont.u4FrColor;
    VERIFY(WIDGET_DrawBorder(&rRect, 2, &_rBorderOSD) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&_rBorderOSD) == (INT32)E_WIDGET_MSG_OK);

    _PlayAudio(u4VdpId, arVideoPath[u4VdpId].u4Src);
    
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _ClearBorder
 *  Hide video boarder for currently active video source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_ClearBorder(UINT32 u4VdpId)
{
    WIDGET_RECT_PROPERTY_T rRect;
    _fgBorderOn = FALSE;

    rRect.u4X = ((arVideoPath[u4VdpId].rVDPRegion.u4Left * TV_WIDTH) >> 2) / VDP_MAX_REGION_WIDTH;
    rRect.u4Y = ((arVideoPath[u4VdpId].rVDPRegion.u4Top * TV_HEIGHT) >> 2) / VDP_MAX_REGION_HEIGHT;
    rRect.u4W = ((arVideoPath[u4VdpId].rVDPRegion.u4Width * TV_WIDTH) >> 2) / VDP_MAX_REGION_WIDTH;
    rRect.u4H = ((arVideoPath[u4VdpId].rVDPRegion.u4Height * TV_HEIGHT) >> 2) / VDP_MAX_REGION_HEIGHT;

    // Workaround for sub scaler calculation
#ifdef CC_MT5363
if(TV_WIDTH%4!=0)
{
    if(rRect.u4X != 0)
    {
    	rRect.u4X -= 2;
        rRect.u4W -= 2;   	
    }
     if((arVideoPath[u4VdpId].rVDPRegion.u4Height != VDP_MAX_REGION_HEIGHT)&&(ePIPMode == (UINT32)E_NAV_MODE_PIP))
    {    	
        rRect.u4H += 2;
       // rRect.u4W -= 4;
    }
	  if((arVideoPath[u4VdpId].rVDPRegion.u4Height == VDP_MAX_REGION_HEIGHT)&&(ePIPMode == (UINT32)E_NAV_MODE_PIP))
    {    	
        rRect.u4H += 0;
        rRect.u4W -= 6;
    }
}
#else	  
    if(rRect.u4X != 0)
    {
    	rRect.u4X -= 2;
        //rRect.u4W += 2;   	
    }
    if(arVideoPath[u4VdpId].rVDPRegion.u4Height != VDP_MAX_REGION_HEIGHT)
    {    	
        rRect.u4H += 2;
        rRect.u4W += 2;
    }
#endif

    rRect.u4Color = 0x0;
    VERIFY(WIDGET_DrawBorder(&rRect, 2, &_rBorderOSD) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&_rBorderOSD) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(OSD_PLA_Enable(_rBorderOSD.u4Plane, FALSE) == (INT32)DRV_E_OK);

    _StopAudio(u4VdpId, arVideoPath[u4VdpId].u4Src);
        
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _GetBorderStatus
 *  Get video boarder status for currently active video source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
BOOL NAV_GetBorderStatus(void)
{
	return _fgBorderOn;
}


//-------------------------------------------------------------------------
/** _SetMode
 *  Set picture mode. Available picture modes are single, PIP, and POP mode
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetMode(UINT32 u4Mode)
{
    UINT32 u4PrevMode = ePIPMode;
	
    WIDGET_MENUITEM_RANGEDVALUE_T* prHScaleItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "H-Scale");
    WIDGET_MENUITEM_RANGEDVALUE_T* prVScaleItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "V-Scale");
    WIDGET_MENUITEM_RANGEDVALUE_T* prHPosItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "H-Pos");
    WIDGET_MENUITEM_RANGEDVALUE_T* prVPosItem = (WIDGET_MENUITEM_RANGEDVALUE_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "VIDEO", "V-Pos");

    NAV_MODECHANGE_SEMA_LOCK;

    if(u4Mode == (UINT32)E_NAV_MODE_SINGLE)
    {
        //DRV_VDO_WINDOW_T rWindow;
#ifndef __MODEL_slt__
        VERIFY(NAV_ClearBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
#endif
        //rWindow.u4Left = prHPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Left = 0;
        //rWindow.u4Top = prVPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Top = 0;
        //rWindow.u4Width = prHScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width = VDP_MAX_REGION_WIDTH;
        //rWindow.u4Height = prVScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Height = VDP_MAX_REGION_HEIGHT;
     
     
        DRV_TVE_Stop(1);
        DRV_TVE_Stop(0);
        
        SRM_SetTvMode((SRM_TV_MODE_T)u4Mode);
        // Trigger SRM by disable and re-enable SCPOS
        DRV_VDO_Mute(NAV_VDP_MASTER);        
        DRV_VDO_Mute(NAV_VDP_SLAVE);
        VERIFY(_StopAV(NAV_VDP_SLAVE, arVideoPath[NAV_VDP_SLAVE].u4Src) == (INT32)E_NAV_MSG_OK); 
        VERIFY(_StopAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src) == (INT32)E_NAV_MSG_OK);

        ePIPMode = (UINT32)E_NAV_MODE_SINGLE;
        u4ActiveVDP = NAV_VDP_MASTER;

			
        VERIFY(_PlayAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src) == (INT32)E_NAV_MSG_OK);          
        DRV_VDO_Unmute(NAV_VDP_MASTER);    
        
        VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

        VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);
        VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
    }
    else if(u4Mode == (UINT32)E_NAV_MODE_POP || u4Mode == (UINT32)E_NAV_MODE_PIP)
    {
        //DRV_VDO_WINDOW_T rWindow, rSubWindow;

        if(ePIPMode != (UINT32)E_NAV_MODE_SINGLE)
        {
            VERIFY(NAV_ClearBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
        }

        if(u4Mode == (UINT32)E_NAV_MODE_POP)
        {
#ifndef __MODEL_slt__
            prHPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Left= 0;
            prVPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Top = (VDP_MAX_REGION_HEIGHT) >> 2;
            prHScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) >> 1;
            prVScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Height= (VDP_MAX_REGION_HEIGHT) >> 1;
#else
            prHPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Left= 0;
            prVPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Top = 0;
            prHScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) >> 1;
            prVScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Height= VDP_MAX_REGION_HEIGHT;
#endif
        }
        else
        {
            prHPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Left = 0;
            prVPosItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Top = 0;
            prHScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width= VDP_MAX_REGION_WIDTH;
            prVScaleItem->i4Value = arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Height= VDP_MAX_REGION_HEIGHT;

        }

#ifndef __MODEL_slt__
        if(u4Mode == (UINT32)E_NAV_MODE_POP)
        {
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Top = (VDP_MAX_REGION_HEIGHT) >> 2;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height= (VDP_MAX_REGION_HEIGHT) >> 1;
        }
        else
        {
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = 0;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Top = 0;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) / 3;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height= (VDP_MAX_REGION_HEIGHT) / 3;
        }
#else
        if(u4Mode == (UINT32)E_NAV_MODE_POP)
        {
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Top = 0;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height= VDP_MAX_REGION_HEIGHT;
        }
        else
        {
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Top = (VDP_MAX_REGION_HEIGHT) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Width= (VDP_MAX_REGION_WIDTH) >> 1;
            arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height= (VDP_MAX_REGION_HEIGHT) >> 1;
        }
#endif

        ePIPMode = u4Mode;
        if(u4Mode == (UINT32)E_NAV_MODE_POP)
        {
            u4ActiveVDP = NAV_VDP_MASTER;
        }
        else
        {
            u4ActiveVDP = NAV_VDP_SLAVE;
        }
#ifndef __MODEL_slt__        
        VERIFY(NAV_ShowBorder(u4ActiveVDP) == (INT32)E_NAV_MSG_OK);
#endif
        VERIFY(_SetupInputSourceItem() == (INT32)E_NAV_MSG_OK);

        VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_Flush(&arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);

        SRM_SetTvMode((SRM_TV_MODE_T)u4Mode);
        // Trigger SRM by disable and re-enable SCPOS
        DRV_VDO_Mute(NAV_VDP_MASTER);
        DRV_VDO_Mute(NAV_VDP_SLAVE);
        VERIFY(_StopAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src) == (INT32)E_NAV_MSG_OK);   
        // If PlayAV (ATV) before ATV channel scan, this will lead to assertion
        //VERIFY(_PlayAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src) == (INT32)E_NAV_MSG_OK);   
        if(_PlayAV(NAV_VDP_MASTER, arVideoPath[NAV_VDP_MASTER].u4Src) != (INT32)E_NAV_MSG_OK) 
        {
            LOG(6, "PlayAV is not OK.\n");
        }
        
        while(!NAV_IsSourcePairValid((UINT32)prInputSrcList->prCurr->pvContent, (UINT32)prInputSrcListPIP->prCurr->pvContent) )
        {
            prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
			arVideoPath[NAV_VDP_SLAVE].u4Src = (UINT32)prInputSrcListPIP->prCurr->pvContent;//tenny add
        }

        if(u4PrevMode == E_NAV_MODE_POP)
        {
            // Trigger SRM by disable and re-enable SCPOS        
            VERIFY(_StopAV(NAV_VDP_SLAVE, arVideoPath[NAV_VDP_SLAVE].u4Src) == (INT32)E_NAV_MSG_OK);   
            // If PlayAV (ATV) before ATV channel scan, this will lead to assertion
            //VERIFY(_PlayAV(NAV_VDP_SLAVE, arVideoPath[NAV_VDP_SLAVE].u4Src) == (INT32)E_NAV_MSG_OK);          
            if(_PlayAV(NAV_VDP_SLAVE, arVideoPath[NAV_VDP_SLAVE].u4Src) != (INT32)E_NAV_MSG_OK) 
            {
                LOG(6, "PlayAV is not OK.\n");
            }
#ifndef __MODEL_slt__
        // trigger audio playing, always stop master and play slave in PIP mode
        VERIFY(NAV_ClearBorder(NAV_VDP_MASTER) == (INT32)E_NAV_MSG_OK);
        VERIFY(NAV_ShowBorder(NAV_VDP_SLAVE) == (INT32)E_NAV_MSG_OK);
#endif                     
        }
        else
        {
            VERIFY(_PlayAV(NAV_VDP_SLAVE, (UINT32)prInputSrcListPIP->prCurr->pvContent) != (INT32)E_NAV_MSG_ERROR);
        }
        DRV_TVE_Stop(1);
        DRV_TVE_Stop(0);
        DRV_VDO_Unmute(NAV_VDP_MASTER);         
        DRV_VDO_Unmute(NAV_VDP_SLAVE); 
    }

    NAV_MODECHANGE_SEMA_UNLOCK;

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _SetProg
 *  Set currently playing TV program 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetProg(UINT32 u4VdpId, UINT32 u4ProgNum)
{
    UINT32 u4Src = arVideoPath[u4VdpId].u4Src;
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    if(u4Src == (UINT32)E_NAV_VS_DT1)
    {
        return _SetProgDTV(u4VdpId, u4ProgNum);
    }
    else if(u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        return _SetProgATV(u4VdpId, u4ProgNum);
    }
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _SetProgDTV
 *  Set currently playing digital TV program 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetProgDTV(UINT32 u4VdpId, UINT32 u4ProgNum)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    NAV_MODECHANGE_SEMA_LOCK;
    VERIFY(_StopAVDTV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);
    rCurrDTVProg.u4VctTblIdx = u4ProgNum;
    VERIFY(_PlayAVDTV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);
    NAV_MODECHANGE_SEMA_UNLOCK;
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** _SetProgATV
 *  Set currently playing analog TV program 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static INT32 _SetProgATV(UINT32 u4VdpId, UINT32 u4ProgNum)
{
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    NAV_MODECHANGE_SEMA_LOCK;
    VERIFY(_StopAVATV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);
    rCurrATVProg.u4ATVTblIdx = u4ProgNum;
    VERIFY(_PlayAVATV(u4VdpId) != (INT32)E_NAV_MSG_ERROR);
    NAV_MODECHANGE_SEMA_UNLOCK;
    return (INT32)E_NAV_MSG_OK;
}


//-------------------------------------------------------------------------
/** _SetVideoHScale
 *  Set video horizontal scale of currently active video input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetVideoHScale(VOID* pvArg)
{
    INT32 i4HScale = (INT32)pvArg;
    UINT32 u4WBound;
    UINT32 u4VdpId = u4ActiveVDP;
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    if(u4VdpId == NAV_VDP_MASTER)
    {
        u4WBound = VDP_MAX_REGION_WIDTH;
    }
    else
    {
        u4WBound = (VDP_MAX_REGION_WIDTH) >> 1;
    }
    if(i4HScale > 0 && arVideoPath[u4VdpId].rVDPRegion.u4Left + (UINT32)i4HScale <= VDP_MAX_REGION_WIDTH && arVideoPath[u4VdpId].rVDPRegion.u4Left <= u4WBound)
    {
        DRV_VDO_WINDOW_T rWindow;

        arVideoPath[u4VdpId].rVDPRegion.u4Width = (UINT32)i4HScale;
        rWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
        rWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
        rWindow.u4Width = arVideoPath[u4VdpId].rVDPRegion.u4Width;
        rWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;        
        VERIFY(DRV_VDO_SetOutputWindow(u4VdpId, &rWindow) == DRV_E_OK);

        return (INT32)E_NAV_MSG_OK;

    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

//-------------------------------------------------------------------------
/** _SetVideoVScale
 *  Set video vertical scale of currently active video input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetVideoVScale(VOID* pvArg)
{
    INT32 i4VScale = (INT32)pvArg;
    UINT32 u4VdpId = u4ActiveVDP;
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    if(i4VScale > 0 && arVideoPath[u4VdpId].rVDPRegion.u4Top + (UINT32)i4VScale <= VDP_MAX_REGION_HEIGHT)
    {
        DRV_VDO_WINDOW_T rWindow;

        arVideoPath[u4VdpId].rVDPRegion.u4Height = (UINT32)i4VScale;
        rWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
        rWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
        rWindow.u4Width = arVideoPath[u4VdpId].rVDPRegion.u4Width;
        rWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;        
        VERIFY(DRV_VDO_SetOutputWindow(u4VdpId, &rWindow) == DRV_E_OK);

        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

//-------------------------------------------------------------------------
/** _SetVideoHPos
 *  Set video horizontal position of currently active video input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetVideoHPos(VOID* pvArg)
{
    INT32 i4HPos = (INT32)pvArg;
    UINT32 u4VdpId = u4ActiveVDP;
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    if(i4HPos >= 0 && arVideoPath[u4VdpId].rVDPRegion.u4Width + (UINT32)i4HPos <= VDP_MAX_REGION_WIDTH)
    {
        DRV_VDO_WINDOW_T rWindow;

        arVideoPath[u4VdpId].rVDPRegion.u4Left = (UINT32)i4HPos;
        rWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
        rWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
        rWindow.u4Width = arVideoPath[u4VdpId].rVDPRegion.u4Width;
        rWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;        
        VERIFY(DRV_VDO_SetOutputWindow(u4VdpId, &rWindow) == DRV_E_OK);
        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

//-------------------------------------------------------------------------
/** _SetVideoVPos
 *  Set video vertical position of currently active video input source
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetVideoVPos(VOID* pvArg)
{
    INT32 i4VPos = (INT32)pvArg;
    UINT32 u4VdpId = u4ActiveVDP;
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);
    if(i4VPos >= 0 && arVideoPath[u4VdpId].rVDPRegion.u4Height + (UINT32)i4VPos <= VDP_MAX_REGION_HEIGHT)
    {
        DRV_VDO_WINDOW_T rWindow;    

        arVideoPath[u4VdpId].rVDPRegion.u4Top = (UINT32)i4VPos;
        rWindow.u4Left = arVideoPath[u4VdpId].rVDPRegion.u4Left;
        rWindow.u4Top = arVideoPath[u4VdpId].rVDPRegion.u4Top;
        rWindow.u4Width = arVideoPath[u4VdpId].rVDPRegion.u4Width;
        rWindow.u4Height = arVideoPath[u4VdpId].rVDPRegion.u4Height;        
        VERIFY(DRV_VDO_SetOutputWindow(u4VdpId, &rWindow) == DRV_E_OK);

        return (INT32)E_NAV_MSG_OK;
    }
    return (INT32)E_NAV_MSG_NOT_SUPPORT;
}

//-------------------------------------------------------------------------
/** _ATVSigProc
 *  Handler for Video signal change event
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
static void _ATVSigProc(const NAV_ATVCB_MSG_T* prMsg)//(void* pvNfyTag, UINT32 u4SigStatus, UINT32 u4SigSrc)
{
    UINT32 u4VdpId = prMsg->u4VdpId;
    UINT32 u4SigSrc = prMsg->u4Src;
    UINT32 u4SigStatus = prMsg->u4SigStatus;
    DRV_VDO_INFO_T rInfo;
    UINT32 u4T = 0, u4B = 0, u4L = 0, u4R = 0;
    
    static BOOL fgEnableProc = TRUE;

    UNUSED(u4SigSrc);
    VERIFY(u4VdpId < NAV_VIDEOPATH_MAX_NUM);

    if(fgATVChannelScanning || !fgEnableProc)
    {
        return;
    }

    NAV_LockAVMutex(u4VdpId);

    DRV_VDO_GetInfo(u4VdpId, &rInfo);
    
    if( (u4SigStatus == 5 || u4SigStatus == SV_VDO_STABLE) && 
    	(arVideoPath[u4VdpId].u4Src == E_NAV_VS_DT1 || arVideoPath[u4VdpId].u4Src == E_NAV_VS_DT2) ) // dtv & stable
    {
        //UINT16 u2MsgIdx = 0;
        //NAV_ATVCB_MSG_T rMsg;
        //UINT32 u4Size = sizeof(NAV_ATVCB_MSG_T);
        
        arVideoPath[u4VdpId].u4ResX = rInfo.u2Width;
        arVideoPath[u4VdpId].u4ResY = rInfo.u2Height;
        arVideoPath[u4VdpId].fgProgressive = rInfo.fgProgressive;
        arVideoPath[u4VdpId].fgValid = TRUE;  

        _LookupOverscanRatio(u4VdpId, &u4T, &u4B, &u4L, &u4R);
        DRV_VDO_SetOverScanRatio(u4VdpId, u4T, u4B, u4L, u4R);
        
        DRV_VDO_Unmute(u4VdpId);            
    }
    else if(u4SigStatus == (UINT32)SV_VDO_STABLE && !fgATVChannelScanning) // analog & stable
    {
        //UINT32 u4Offset;
        //DRV_AUD_PARAM_T rParam;
        //BOOL fgIsVgaTiming;
        //DRV_ERR_CODE_T rRetCode;


        arVideoPath[u4VdpId].u4ResX = rInfo.u2Width;
        arVideoPath[u4VdpId].u4ResY = rInfo.u2Height;
        arVideoPath[u4VdpId].fgProgressive = rInfo.fgProgressive;
        arVideoPath[u4VdpId].fgValid = TRUE;                

        if(rInfo.fgVgaTiming)
        {
            DRV_VDO_SetOverScanRatio(u4VdpId, 0, 0, 0, 0);      
        }
        else
        {
            _LookupOverscanRatio(u4VdpId, &u4T, &u4B, &u4L, &u4R);
            DRV_VDO_SetOverScanRatio(u4VdpId, u4T, u4B, u4L, u4R);      
        }            
        LOG(6, "stop and play audio\n");

        DRV_VDO_Unmute(u4VdpId);
        if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
        {
            DRV_AUD_Unmute();
        }

        //vApiCvbsOutCtrl(_bSrcMainNew); 
        
    }
    else if( (u4SigStatus == 4 || u4SigStatus < (UINT32)SV_VDO_STABLE) && 
    	            (arVideoPath[u4VdpId].u4Src == E_NAV_VS_DT1 || arVideoPath[u4VdpId].u4Src == E_NAV_VS_DT2) ) // dtv & unstable
    {
        arVideoPath[u4VdpId].fgValid = FALSE;  
        DRV_VDO_Mute(u4VdpId);
    }
    else if(u4SigStatus < (UINT32)SV_VDO_STABLE && !fgATVChannelScanning) // analog & unstable
    {       
        arVideoPath[u4VdpId].fgValid = FALSE;  
        DRV_VDO_Mute(u4VdpId);
        if((arVideoPath[u4VdpId].u4Src != E_NAV_VS_DT1) &&
        	(arVideoPath[u4VdpId].u4Src != E_NAV_VS_DT2) )
        {        
            if(rNAVOption.fgEnableAudio && u4VdpId == u4ActiveVDP)
            {
                //DRV_AUD_Mute();
            }        
        }
    }
    if (u4SigStatus  ==(UINT32)SV_VDO_STABLE &&(arVideoPath[u4VdpId].u4Src>=E_NAV_VS_HDMI)&&(arVideoPath[u4VdpId].u4Src<=E_NAV_VS_HDMI5))
    {
		_SetDVIAudio(u4VdpId);
	}

    // If it is a 3D source, disable TVE
#if defined(CC_MT5365) ||defined(CC_MT5395)
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE && !_IsMainSrc3D()) 
#else
	if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
#endif
    {
        DRV_TVE_SetSource(0, _au4SourceIdx2VS[arVideoPath[NAV_VDP_MASTER].u4Src]);
        DRV_TVE_Start(0); 

		DRV_TVE_SetSource(1, _au4SourceIdx2VS[arVideoPath[NAV_VDP_MASTER].u4Src]);
        DRV_TVE_Start(1); 
    }
    else 
    {
        DRV_TVE_Stop(1);
        DRV_TVE_Stop(0);
    }
    
    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);

    NAV_UnlockAVMutex(u4VdpId);

}

static void _LookupOverscanRatio(UINT32 u4VdpId, UINT32 *pu4T,UINT32 *pu4B, UINT32 *pu4L, UINT32 *pu4R) // look up overscan setting table
{
    switch(_au4SourceIdx2VD[arVideoPath[u4VdpId].u4Src])
    {
#if 0    
        case (E_NAV_VD_TVD):
            *pu4T = 39;
            *pu4B = 8;   
            *pu4L = 27;
            *pu4R = 26;                     
            break;  
        case (E_NAV_VD_HDTV):
            break;
        case (E_NAV_VD_HDMI):
            *pu4T = 20;
            *pu4B = 10; 
            *pu4L = 15;
            *pu4R = 15;             
            break;
#endif            
        default:
            *pu4T = 0;
            *pu4B = 0; 
            *pu4L = 0;
            *pu4R = 0;             
    }
}

static void _ResetATVTbl(UINT32 u4System)
{
    UINT32 i;
#if 0
#ifndef CC_NAV_DEMOD_DVBT
    if(u4System == NAV_ATV_SYSTEM_CABLE)
    {
        u4NumATVProg = NTSC_CABLE_CH_NS;
    }
    else
    {
        u4NumATVProg = NTSC_RF_CH_NS;
    }
#else
    if(u4System == NAV_ATV_SYSTEM_PAL_BG)
    {
        u4NumATVProg = PALBG_CH_NS;
    }
    else    if(u4System == NAV_ATV_SYSTEM_PAL_DK)
    {
        u4NumATVProg = PALDK_CH_NS;
    }
    else    if(u4System == NAV_ATV_SYSTEM_PAL_I)
    {
        u4NumATVProg = PALI_CH_NS;
    }
    else    if(u4System == NAV_ATV_SYSTEM_SECAM_L)
    {
        u4NumATVProg = SECAML_CH_NS;
    }
    else    if(u4System == NAV_ATV_SYSTEM_SECAM_L1)
    {
        u4NumATVProg = SECAML1_CH_NS;
    }
    else    if(u4System == NAV_ATV_SYSTEM_NTSC_M)
    {
        u4NumATVProg = NTSCM_CH_NS;
    }
#endif
#endif
    for(i = 0; i < NAV_ATV_PROG_MAX_NUM; i++)
    {
        arATVTbl[i].u2FreqTabIdx = i;
    }

}

//-------------------------------------------------------------------------
/** NAV_InitAV
 *  Initalize A/V playback related threads, semaphores, and message queues, etc
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_InitAV(void)
{
    //static BOOL fgInit = FALSE;
    static HANDLE_T hOSDThread;
    //static HANDLE_T hATVSigPollThread;
    //static HANDLE_T hNIMSigPollThread;
    static HANDLE_T hATVProcThread;
    CRIT_STATE_T rCritState;

    if(!fgNavAvInit)
    {
        VERIFY(x_sema_create(&_hPlayAVSema, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY(x_sema_create(&_hPlayAVSemaPIP, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY(x_sema_create(&hModeChangeSema, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);        
        VERIFY(x_sema_create(&hVgaSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);        
        VERIFY(x_msg_q_create(&_hOSDMsgq, NAV_OSD_MSGQ, sizeof(NAV_OSD_MSG_T), 32) == OSR_OK);
        VERIFY(x_msg_q_create(&_hATVCBMsgq, NAV_ATVCB_MSGQ, sizeof(NAV_ATVCB_MSG_T), 256) == OSR_OK);
  //      VERIFY(x_msg_q_create(&_hATVAudMsgq, NAV_ATVAUD_MSGQ, sizeof(NAV_ATVAUD_MSG_T), 32) == OSR_OK);
        VERIFY(x_msg_q_create(&_hTunerMsg, "M_T2U", sizeof(DRV_TUNER_MSG_T), 32) == OSR_OK);
        VERIFY(x_msg_q_create(&_hTtxMsgq, "M_TTX", sizeof(UINT32), 32) == OSR_OK);           
        VERIFY(x_msg_q_create(&_hAudCtrlMsgq, NAV_AUDCTRL_MSGQ, sizeof(UINT32), 32) == OSR_OK);

        fgNavAvInit = TRUE;
    }
    else
    {
        return (INT32)E_NAV_MSG_HAS_INIT;
    }

    prInfoOSDBg = NAV_LookupBmp("info_osd");

    //VERIFY(bApiVideoSetVideoSrc(NAV_VDP_MASTER, _au4SourceIdx2VS[(UINT8)E_NAV_VS_DT1]) == SV_SUCCESS);

    if(rNAVOption.fgEnableAudio) 
    {
        VERIFY(NAV_AQ_Init() == (INT32)E_NAV_MSG_OK);
    }

    if(u4NumATVProg == 0)
    {    	
        _ResetATVTbl(u4ATVSystem);
    }

    VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);

    {
        DRV_TUNER_PARAM_T rParam;
        rParam.rAnaParam.pfnTuningDone = _TunerAnaCb;
        rParam.rAnaParam.rTvSys = DRV_TV_SYS_B_G;
        rParam.rDigParam.fgSpectrumInversion = TRUE;
        rParam.rDigParam.pfnTuningDone = _TunerDigCb;
        rParam.rDigParam.pfnLockStatusChange = _TunerStatusCallback;
        rParam.rDigParam.rBandwidth = DRV_TUNER_BW_6_MHz;
        rParam.rDigParam.rMod = DRV_TUNER_MOD_VSB_8;
        rParam.rDigParam.u4QamSize = 8;
        rParam.rDigParam.u4SymbolRate = 0x0;        	
        DRV_TUNER_SetParameter(&rParam);
    }
    
    if(rNAVOption.fgEnableOSD32)
    {
        rInfoOSDFont.u4FrColor = 0xFFFFFF00;
    }
    else
    {
        rInfoOSDFont.u4FrColor = 0xFFF0;
    }

    VERIFY(x_thread_create(&hOSDThread, "PROGINFO", NAV_IR_STACK_SZ, NAV_IR_PRIORITY, (x_os_thread_main_fct)_DrawOSDThread, 0, NULL) == OSR_OK);
#ifdef CC_YES_1394
    VERIFY(x_thread_create(&hATVSigPollThread, "1394POL", 2048, 100, (x_os_thread_main_fct)_I1394StatusPollThread, 0, NULL) == OSR_OK);
#endif
    VERIFY(x_thread_create(&hATVProcThread, "ATVPROC", 2048, 100, (x_os_thread_main_fct)_ATVSigProcThread, 0, NULL) == OSR_OK);
    VERIFY(x_thread_create(&_hTtxThread, "T_TTX", 2048, 100, _TtxProcThread, 0, NULL) == OSR_OK);
    
    _prDbase = DBS_Lock(&rCritState);
    VERIFY(_prDbase);
    VERIFY(DBS_Unlock(_prDbase, rCritState));

#ifdef CC_MT5391_SLT
    NAV_SetPid(0x7d1, 0x1b, 0x7d2, STREAM_TYPE_AAC, 0x7d1, 68, FALSE);
#endif

    if(rNAVOption.fgEnableAudio) 
    {
        DRV_AUD_PARAM_T rAudioParam;
        rAudioParam.u2Pid = 0;
        rAudioParam.u2PcrPid = 0;
        rAudioParam.rInputSrc = DRV_IS_CVBS_1;
        rAudioParam.rCodingType = DRV_AUD_CT_PCM;
        rAudioParam.pfnPlayDone = (PFN_DRV_CALLBACK)_AudCtrlDoneNfyFct;            
        rAudioParam.pfnStopDone = (PFN_DRV_CALLBACK)_AudCtrlDoneNfyFct;   
        rAudioParam.pfnDetectDone = NULL;
        DRV_AUD_SetParameter(&rAudioParam);
    }

    //VERIFY(_PlayAV(NAV_VDP_MASTER, NAV_CONFIG_GetDefInputSrc()) != (INT32)E_NAV_MSG_ERROR);
    //VERIFY(_SetSrc(0, NAV_CONFIG_GetDefInputSrc()) == E_NAV_MSG_OK);
    
    VERIFY(NAV_SetSrcActive((void*)_aszSourceIdx2Str[NAV_CONFIG_GetDefInputSrc()]) == E_NAV_MSG_OK);    
    
    while(prInputSrcListPIP->prCurr->pvContent != (void*)E_NAV_VS_HDMI)
    {
        prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
    } 

    _InitBorder();
    DRV_TVE_Init();
    
    //fgInit = TRUE;
    _fgCliATVSearch =FALSE;
    return (INT32)E_NAV_MSG_OK;

}

//-------------------------------------------------------------------------
/** NAV_DrawInfoOSD
 *  Wrapper for sending redraw OSD event
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_DrawInfoOSD(void)
{
    NAV_OSD_MSG_T rMsg;
    UINT16 u2MsgCount;
    x_memcpy((void*)&(rMsg.rVideoPath), (const void*)&arVideoPath[u4ActiveVDP], sizeof(NAV_VIDEOPATH_T));
    rMsg.u4Volume = (UINT32)NAV_AQ_GetVol();
    rMsg.fgMute = (BOOL)NAV_AQ_GetMute();
    rMsg.fgFreeze = NAV_GetFreeze(u4ActiveVDP);
    rMsg.u4VctTblIdx = rCurrDTVProg.u4VctTblIdx;
    rMsg.u4ATVTblIdx = rCurrATVProg.u4ATVTblIdx;
    rMsg.fgChannelChange = FALSE;
	/*DTV00137892*/
	VERIFY(x_msg_q_num_msgs(_hOSDMsgq, &u2MsgCount) == OSR_OK);
	if(u2MsgCount < 32)
	{
        VERIFY(x_msg_q_send(_hOSDMsgq, (const void*)&rMsg, sizeof(NAV_OSD_MSG_T), 254) == OSR_OK);
	}
	else
	{
	    printf("[DEMO]#####NAV_DrawInfoOSD:_hOSDMsgq is too long and ignore message######\n");
	}
    return (INT32)E_NAV_MSG_OK;
}

INT32 NAV_DrawInfoOSD_2(UINT16 u2Major, UINT16 u2Minor)
{
    NAV_OSD_MSG_T rMsg;

    x_memcpy((void*)&(rMsg.rVideoPath), (const void*)&arVideoPath[u4ActiveVDP], sizeof(NAV_VIDEOPATH_T));
    rMsg.u4Volume = (UINT32)NAV_AQ_GetVol();
    rMsg.fgMute = (BOOL)NAV_AQ_GetMute();
    rMsg.fgFreeze = NAV_GetFreeze(u4ActiveVDP);
    rMsg.u4VctTblIdx = rCurrDTVProg.u4VctTblIdx;
    rMsg.u4ATVTblIdx = rCurrATVProg.u4ATVTblIdx;
    rMsg.fgChannelChange = TRUE;
    rMsg.u2Major = u2Major;
    rMsg.u2Minor = u2Minor;    
    VERIFY(x_msg_q_send(_hOSDMsgq, (const void*)&rMsg, sizeof(NAV_OSD_MSG_T), 254) == OSR_OK);
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_DrawInfoOSD
 *  Handler for redraw OSD event
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 _DrawInfoOSD(const NAV_OSD_MSG_T* prMsg)
{
    CHAR szBuf[100], szBuf2[50], szBuf3[20], szABuf[20];
    WIDGET_RECT_PROPERTY_T rRect;
    WIDGET_FONT_PROPERTY_T rFont;
    DRV_VDO_INFO_T rInfo;
    UINT8 u1RefreshRate;
    UINT16 u2HTotal;
    UINT16 u2VTotal;
    
    VERIFY(prMsg != NULL);
    
	DRV_VDO_GetInfo(u4ActiveVDP, &rInfo);    
    u1RefreshRate = rInfo.u1RefreshRate;
    u2HTotal = rInfo.u2HTotal;
    u2VTotal = rInfo.u2VTotal;

    rFont.u4FontSize = rInfoOSDFont.u4FontSize;
    rFont.u4BgWidth = 100;
    rFont.u4FrColor = rInfoOSDFont.u4FrColor;
    rFont.u4BgColor = rInfoOSDFont.u4BgColor;

    VERIFY(WIDGET_ClearScreen(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
//#ifndef __MODEL_slt__        
    VERIFY(WIDGET_DrawBitmap(prInfoOSDBg, 0, 0, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
//#endif
    VERIFY(WIDGET_DrawString("Volume", 480, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

    if(prMsg->fgMute)
    {
        VERIFY(WIDGET_DrawString("MUTE", 580, 50, &rFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    }
    else
    {

        rRect.u4X = 580;
        rRect.u4Y = 60;
        rRect.u4W = 100;
        rRect.u4H = rInfoOSDFont.u4FontSize / 2;
        rRect.u4Color = rInfoOSDFont.u4FrColor;
        VERIFY(WIDGET_DrawBorder(&rRect, 1, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        rRect.u4W = prMsg->u4Volume;
        VERIFY(WIDGET_Fill(&rRect, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        UNUSED(x_sprintf(szBuf, "%d", prMsg->u4Volume));

        VERIFY(WIDGET_DrawString(szBuf, 700, 50, &rFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    }


    t_crit_scanchannel = x_crit_start();
    if(fgDTVChannelScanning)
    {
        if( u4DTVProgAtPhyCh > 0)
        {
#ifndef CC_NAV_DEMOD_DVBT
            UNUSED(sprintf(szBuf2, "Scanning frequency %d    %d programs found", (u4DTVSystem==NAV_DTV_SYSTEM_ATSC)?au4AtscChannel[u4DTVPhyCh]:au4CableChannel[u4DTVPhyCh], u4DTVProgAtPhyCh));
#else
#ifndef DRV_IF_BR
 	     UNUSED(sprintf(szBuf2, "Scanning frequency %d    %d programs found", (u4DTVSystem==NAV_DTV_SYSTEM_DVBT_UK)?au4DVBTUKChannel[u4DTVPhyCh]:
 	     ((u4DTVSystem==NAV_DTV_SYSTEM_DVBT_TW)?au4DVBTTWChannel[u4DTVPhyCh]:au4DVBCChannel[u4DTVPhyCh]), u4DTVProgAtPhyCh));
#else
            UNUSED(sprintf(szBuf2, "Scanning frequency %d    %d programs found", au4ISDBTChannel[u4DTVPhyCh], u4DTVProgAtPhyCh));
#endif
#endif
        }
        else
        {
#ifndef CC_NAV_DEMOD_DVBT
            UNUSED(sprintf(szBuf2, "Scanning frequency %d    No program", (u4DTVSystem==NAV_DTV_SYSTEM_ATSC)?au4AtscChannel[u4DTVPhyCh]:au4CableChannel[u4DTVPhyCh]));
#else
#ifndef DRV_IF_BR
            UNUSED(sprintf(szBuf2, "Scanning frequency %d    No program", (u4DTVSystem==NAV_DTV_SYSTEM_DVBT_UK)?au4DVBTUKChannel[u4DTVPhyCh]:
            ((u4DTVSystem==NAV_DTV_SYSTEM_DVBT_TW)?au4DVBTTWChannel[u4DTVPhyCh]:au4DVBCChannel[u4DTVPhyCh])));
#else
            UNUSED(sprintf(szBuf2, "Scanning frequency %d    No program", au4ISDBTChannel[u4DTVPhyCh]));
#endif
#endif
        }
        VERIFY(WIDGET_DrawString("DTV", 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

        UNUSED(sprintf(szBuf, "Totally %d programs", u4NumDTVProg));
        VERIFY(WIDGET_DrawString(szBuf2, 160, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_DrawString(szBuf, 20, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

        VERIFY(WIDGET_Flush(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        x_crit_end(t_crit_scanchannel);
        return (INT32)E_NAV_MSG_OK;
    }
    x_crit_end(t_crit_scanchannel);

    t_crit_scanchannel = x_crit_start();
    if(fgATVChannelScanning)
    {

        UNUSED(sprintf(szBuf2, "Scanning channel %d   ", u4ATVPhyCh + 1 ));
        VERIFY(WIDGET_DrawString("ATV", 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        UNUSED(sprintf(szBuf, "Totally %d programs", u4NumATVProg));
        VERIFY(WIDGET_DrawString(szBuf2, 160, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_DrawString(szBuf, 20, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

        VERIFY(WIDGET_Flush(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        x_crit_end(t_crit_scanchannel);
        return (INT32)E_NAV_MSG_OK;
    }
    x_crit_end(t_crit_scanchannel);
    
#ifndef CC_YES_1394
    if(prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_DT1)    	    
#else
    if(prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_DT1  && NAV_I1394_GetState() != 3)    	    
#endif
    {
        if(u4NumDTVProg == 0)
        {
            UNUSED(x_sprintf(szBuf, "No program"));
            UNUSED(x_sprintf(szBuf2, " "));
        }
        else
        {
            // print channel number
            UNUSED(x_sprintf(szBuf, "%d.%d %7s",
                arVctTbl[prMsg->u4VctTblIdx].u2MajorChNum,
                arVctTbl[prMsg->u4VctTblIdx].u2MinorChNum,
                arVctTbl[prMsg->u4VctTblIdx].szShortName));
        
            if(!(prMsg->rVideoPath.fgValid))
            {
                UNUSED(x_sprintf(szBuf2, "No signal "));
            }
             else
            {
                CHAR *pszVType, *pszAType;
                UINT16 u2APIDType = 0;
                if(arVctTbl[prMsg->u4VctTblIdx].u2VPID != 0x0)
                {

                    switch(arVctTbl[prMsg->u4VctTblIdx].u4VPIDType)
                    {
                        case (STREAM_TYPE_H264):
                            pszVType = "H.264";
                            break;
                        case (STREAM_TYPE_AVS):
                            pszVType = "AVS";
                            break;                            
                        default:
                            pszVType = "MPEG";
                    }

                    if(u1CurrentMTS == 0)
                    {
                        u2APIDType = arVctTbl[prMsg->u4VctTblIdx].u2APID1Type;
                    }
                    else if(u1CurrentMTS == 1)
                    {
                        u2APIDType = arVctTbl[prMsg->u4VctTblIdx].u2APID2Type;
                    }
                    else if(u1CurrentMTS == 2)
                    {
                        u2APIDType = arVctTbl[prMsg->u4VctTblIdx].u2APID3Type;
                    }                       

                    //if(arVctTbl[prMsg->u4VctTblIdx].u2APID != 0x0)
                    //{
                        switch(u2APIDType)
                        {
                            case (STREAM_TYPE_MPEG1):
                            case (STREAM_TYPE_MPEG2):                                 
                                pszAType = "MPEG";
                                break;                                
                            case (STREAM_TYPE_AAC):                      	
                            case (STREAM_TYPE_AAC_BRZ):
                                pszAType = "AAC";
                                break;
                            case (STREAM_TYPE_AC3):  
                            case (STREAM_TYPE_AC3_EU):                                 
                                pszAType = "AC3";
                                break;                                 
                            default:
                                pszAType = "None";
                        }
                    //}  
                    
                x_sprintf(szABuf, "%s (%d/%d)", pszAType, (u1CurrentMTS+1), u1TotalMTS);
#if 1//def SUPPORT_SCRAMBLE_DETECT            
                    if(rCurrDTVProg.u4Scrambled != 1)
                    {
                        UNUSED(x_sprintf(szBuf2, "%2d(%d) * %2d%s(%d) @ %d ",
                    	    prMsg->rVideoPath.u4ResX, u2HTotal,
                    	    prMsg->rVideoPath.u4ResY, 
                    	    (prMsg->rVideoPath.fgProgressive)? "p":"i",
                    	    u2VTotal,
                    	    u1RefreshRate));

                        // print channel number
                        UNUSED(x_sprintf(szBuf, "%d.%d %7s, V: %s, A: %s",
                            arVctTbl[prMsg->u4VctTblIdx].u2MajorChNum,
                            arVctTbl[prMsg->u4VctTblIdx].u2MinorChNum,
                            arVctTbl[prMsg->u4VctTblIdx].szShortName, 
                            pszVType, szABuf));
                    }     
                    else           	
                    {
                        UNUSED(x_sprintf(szBuf2, "Scrambled Program"));
                    }
#else
                    UNUSED(x_sprintf(szBuf2, "%2d*%2d %s",
                	    prMsg->rVideoPath.u4ResX,
                	    prMsg->rVideoPath.u4ResY,
                	    (prMsg->rVideoPath.fgProgressive)? "p":"i"));
#endif
                }
                else
                {
                    switch(arVctTbl[prMsg->u4VctTblIdx].u2APID1Type)
                    {
                        case (STREAM_TYPE_MPEG1):
                        case (STREAM_TYPE_MPEG2):                            
                            pszAType = "MPEG";
                            break;
                        case (STREAM_TYPE_AAC): 
                        case (STREAM_TYPE_AAC_BRZ):                            
                            pszAType = "AAC";
                            break;
                        case (STREAM_TYPE_AC3): 
                        case (STREAM_TYPE_AC3_EU):                            
                            pszAType = "AC3";
                            break;                             
                        default:
                            pszAType = "None";
                    }
                    UNUSED(x_sprintf(szBuf2, "Audio Program, A: %s",
                        pszAType
                    ));

                }
            }
        }
    }
    else if(prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_TUNER1)
    {
        if(u4NumATVProg > 0 && prMsg->rVideoPath.fgValid)
        {
            if(!prMsg->fgChannelChange)
            {
                UNUSED(x_sprintf(szBuf, "%3d", arATVTbl[prMsg->u4ATVTblIdx].u2FreqTabIdx + 1));
            }
            else
            {
                UNUSED(x_sprintf(szBuf, "%3d", prMsg->u2Major));
            }

            UNUSED(x_sprintf(szBuf2, "%2d(%d) * %2d%s(%d) @ %d ",
        	    prMsg->rVideoPath.u4ResX, u2HTotal,
        	    prMsg->rVideoPath.u4ResY, 
        	    (prMsg->rVideoPath.fgProgressive)? "p":"i",
        	    u2VTotal,
        	    u1RefreshRate));
        }
        else
        {
            UNUSED(x_sprintf(szBuf, "No signal"));
            UNUSED(x_sprintf(szBuf2, " "));
        }
    }

    else if(prMsg->rVideoPath.u4Src <= (UINT32)E_NAV_VS_SCART2 && prMsg->rVideoPath.u4Src != (UINT32)E_NAV_VS_DT2)
    {
        if(prMsg->rVideoPath.fgValid)
        {
            UNUSED(x_sprintf(szBuf, " "));
            UNUSED(x_sprintf(szBuf2, "%2d(%d) * %2d%s(%d) @ %d ",
        	    prMsg->rVideoPath.u4ResX, u2HTotal,
        	    prMsg->rVideoPath.u4ResY, 
        	    (prMsg->rVideoPath.fgProgressive)? "p":"i",
        	    u2VTotal,
        	    u1RefreshRate));
        }
        else
        {
            UNUSED(x_sprintf(szBuf, "No signal"));
            UNUSED(x_sprintf(szBuf2, " "));
        }
    }
    // Multimedia playback
    else if(prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_DT2)
    {
        NAV_MM_UpdateUI(&arWidgetOSD[1]);
        return (INT32)E_NAV_MSG_OK;
    }

#ifdef CC_YES_1394
    // I1394 info
    else if(prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_DT2)
    {
        INT32 i4Mode;
        if(NAV_I1394_GetState() == 3)
        {
            i4Mode = NAV_I1394_GetMode();
            if(i4Mode == (INT32)TP_PLAY || i4Mode == (INT32)TP_REC_PLAY)
            {
                UNUSED(x_sprintf(szBuf, "Playing"));
            }
            else if(i4Mode == (INT32)TP_PLAY_FAST_FORWARD)
            {
                UNUSED(x_sprintf(szBuf, "Forward"));
            }
            else if(i4Mode == (INT32)TP_PLAY_FAST_REVERSE)
            {
                UNUSED(x_sprintf(szBuf, "Reverse"));
            }
            else if(i4Mode == (INT32)TP_STOP)
            {
                UNUSED(x_sprintf(szBuf, "Stop"));
            }
            else
            {
                UNUSED(x_sprintf(szBuf, "Unknown state"));
            }

        }
        else
        {
            UNUSED(x_sprintf(szBuf, "Stop"));
        }
        VERIFY(WIDGET_DrawString("I1394", 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_DrawString(szBuf, 20, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        VERIFY(WIDGET_Flush(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
        return (INT32)E_NAV_MSG_OK;
    }
#endif

    else
    {

        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }

    if((prMsg->rVideoPath.u4Src == (UINT32)E_NAV_VS_DT1) && DRV_TUNER_IsCamConnected())
    {
        x_sprintf(szBuf3, "%s (CAM Connected)", _aszSourceIdx2Str[prMsg->rVideoPath.u4Src]);
    }
    else
    {
        x_sprintf(szBuf3, "%s", _aszSourceIdx2Str[prMsg->rVideoPath.u4Src]);
    }
    
#ifdef DRV_IF_3DTV 
    //check 3D or not
    UNUSED(x_sprintf(szBuf2, "%s %s ", szBuf2, (rInfo.r3DStatus == DRV_VDO_3D_2D_ONLY)? "":"3D"));
#endif

    VERIFY(WIDGET_DrawString(szBuf3, 20, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_DrawString(szBuf,  20, 50, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_DrawString(szBuf2, 400, 10, &rInfoOSDFont, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_ClearInfoOSD
 *  Hide program info OSD
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_ClearInfoOSD(VOID)
{
    WIDGET_RECT_PROPERTY_T rRect;
    rRect.u4X = arWidgetOSD[1].u4X;
    rRect.u4Y = arWidgetOSD[1].u4Y;
    rRect.u4W = arWidgetOSD[1].u4W;
    rRect.u4H = arWidgetOSD[1].u4H;
    rRect.u4Color = 254;
    VERIFY(WIDGET_Fill(&rRect, &arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    VERIFY(WIDGET_Flush(&arWidgetOSD[1]) == (INT32)E_WIDGET_MSG_OK);
    return (INT32)E_NAV_MSG_OK;
}

static INT32 _SetupInputSourceItem(void)
{
    WIDGET_MENUITEM_OPTION_T* prItem;
    WIDGET_STRINGLIST_T* prList;
    //WIDGET_STRINGLIST_ELEMENT_T* prEle;
    //UINT32 i;
    prItem = (WIDGET_MENUITEM_OPTION_T*)(void*)WIDGET_LookupMenuItem(arWidgetOSD[0].prList, "SETUP", "AV Input Source");
    prList = prItem->prStrList;

    // Remove all previous items
    for(; prList->prHead != NULL; UNUSED(WIDGET_RemoveLastStringListElement(prList)))
    {
    }

    // Reconstruct items
    if(ePIPMode == (UINT32)E_NAV_MODE_SINGLE)
    {
        WIDGET_LIST_ELEMENT_T* prEle = prInputSrcList->prCurr;
        VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
        for(prEle = prEle->prNext; prEle != prInputSrcList->prCurr; prEle = prEle->prNext)
        {
            VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
        }

    }
#if NAV_PIP_DTV_CVBS1
    else if(u4ActiveVDP == NAV_VDP_MASTER)
    {
        WIDGET_LIST_ELEMENT_T* prEle = prInputSrcList->prCurr;
        VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
    }
    else
    {
        WIDGET_LIST_ELEMENT_T* prEle = prInputSrcListPIP->prCurr;
        VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
    }

#else

    else if(u4ActiveVDP == NAV_VDP_MASTER)
    {
        WIDGET_LIST_ELEMENT_T* prEle = prInputSrcList->prCurr;
        VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
        for(prEle = prEle->prNext; prEle != prInputSrcList->prCurr; prEle = prEle->prNext)
        {

            if(NAV_IsSourcePairValid((UINT32)prEle->pvContent, arVideoPath[NAV_VDP_SLAVE].u4Src))
            {
                VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
            }
        }
#ifndef __MODEL_slt__       
        arVideoPath[u4ActiveVDP].u4Src = (UINT32)(prInputSrcList->prCurr->pvContent);        
#endif
    }
    else
    {
        WIDGET_LIST_ELEMENT_T* prEle = prInputSrcListPIP->prCurr;
        VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
        for(prEle = prEle->prNext; prEle != prInputSrcListPIP->prCurr; prEle = prEle->prNext)
        {

            if(NAV_IsSourcePairValid(arVideoPath[NAV_VDP_MASTER].u4Src, (UINT32)prEle->pvContent))
            {
                VERIFY(WIDGET_AddStringListElement(prList, _aszSourceIdx2Str[(UINT32)prEle->pvContent]) == (INT32)E_WIDGET_MSG_OK);
            }
        }
#ifndef __MODEL_slt__       
        arVideoPath[u4ActiveVDP].u4Src = (UINT32)(prInputSrcListPIP->prCurr->pvContent);                             
#endif
    }

#endif

    return (INT32)E_NAV_MSG_OK;

}

INT32 NAV_SetupInputSourceItem(void)
{
    return _SetupInputSourceItem();
}

//-------------------------------------------------------------------------
/** NAV_SetDTVSystem
 *  Set digital TV system
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetDTVSystem(void* pvArg)
{
#ifndef CC_NAV_DEMOD_DVBT
    CHAR* szSystem = (CHAR*)pvArg;
    if(x_strcmp("Terrestrial", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_ATSC;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }
    else if(x_strcmp("Cable", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_CABLE;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }
    return (INT32)E_NAV_MSG_OK;
#else
    CHAR* szSystem = (CHAR*)pvArg;
    if(x_strcmp("DVB-T UK", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_DVBT_UK;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }
    else if(x_strcmp("DVB-C", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_DVBC;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }
#ifdef DRV_IF_BR    
    else if(x_strcmp("ISDBT BR", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_ISDBT;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }   
#endif    
    else if(x_strcmp("DVB-T TW", szSystem) == 0)
    {
        u4DTVSystem = NAV_DTV_SYSTEM_DVBT_TW;
        VERIFY(_SetDTVChannelRange(fgManualScan, 0, 0) == (INT32)E_NAV_MSG_OK);
    }
    return (INT32)E_NAV_MSG_OK;

#endif
}

//-------------------------------------------------------------------------
/** NAV_SetATVSystem
 *  Set analog TV system
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetATVSystem(void* pvArg)
{
#ifndef CC_NAV_DEMOD_DVBT
    CHAR* szSystem = (CHAR*)pvArg;
    if(x_strcmp("Air", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_AIR);

    }
    else if(x_strcmp("Cable", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_CABLE);
    }
    return (INT32)E_NAV_MSG_OK;
#else
    CHAR* szSystem = (CHAR*)pvArg;
    if(x_strcmp("PAL B/G", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_PAL_BG);
    }
    else if(x_strcmp("PAL D/K", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_PAL_DK);
    }
    else if(x_strcmp("PAL I", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_PAL_I);
    }
    else if(x_strcmp("SECAM L", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_SECAM_L);
    }
    else if(x_strcmp("SECAM L1", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_SECAM_L1);
    }
    else if(x_strcmp("NTSC M", szSystem) == 0)
    {
          _ResetATVTbl(u4ATVSystem = NAV_ATV_SYSTEM_NTSC_M);
    }
    return (INT32)E_NAV_MSG_OK;
#endif
}


static void _SwapVCTEntry(PSIPSR_TBL_VCT_T* prEntry1, PSIPSR_TBL_VCT_T* prEntry2)
{
	PSIPSR_TBL_VCT_T rTemp;
	x_memcpy(&rTemp, prEntry1, sizeof(PSIPSR_TBL_VCT_T));
	x_memcpy(prEntry1, prEntry2, sizeof(PSIPSR_TBL_VCT_T));
	x_memcpy(prEntry2, &rTemp, sizeof(PSIPSR_TBL_VCT_T));
}

static INT32 _CompareVCTEntry(PSIPSR_TBL_VCT_T rEntry1, PSIPSR_TBL_VCT_T rEntry2 )
{
	return (INT32)((UINT32)rEntry1.u2MajorChNum * 65536 + rEntry1.u2MinorChNum -
		((UINT32)rEntry2.u2MajorChNum * 65536 + rEntry2.u2MinorChNum));
}

static void _SortVCT(void)
{
	UINT32 i, j;
	INT32 i4CompRes;
	UINT32 u4PrevMajor;

	for(i = 0; i < u4NumDTVProg; i++)
	{
		for(j = i; j < u4NumDTVProg; j++)
		{
			i4CompRes = _CompareVCTEntry(arVctTbl[i], arVctTbl[j]);
			if(i4CompRes > 0)
			{
				_SwapVCTEntry(&arVctTbl[i], &arVctTbl[j]);
			}
			else if(i4CompRes == 0 && arVctTbl[i].u2ChTsId < arVctTbl[j].u2ChTsId)
			{
				_SwapVCTEntry(&arVctTbl[i], &arVctTbl[j]);
			}
		}
	}
	u4PrevMajor = 0;
	for(i = 1; i < u4NumDTVProg; i++)
	{
		if(arVctTbl[i].u2ChTsId == 0 && arVctTbl[i].u2MajorChNum == u4PrevMajor)
		{
			arVctTbl[i].u2MinorChNum = arVctTbl[i - 1].u2MinorChNum + 1;
		}
		u4PrevMajor = arVctTbl[i].u2MajorChNum;
	}


}

static void _PrintDTVProgInfo(void)
{
    static UINT32 u4Count = 1;
    UINT32 i;
    UINT32* pu4ChTbl;
#ifndef CC_NAV_DEMOD_DVBT
    if(u4DTVSystem == NAV_DTV_SYSTEM_ATSC)
    {
        pu4ChTbl = au4AtscChannel;
    }
    else
    {
        pu4ChTbl = au4CableChannel;
    }
#else
    if(u4DTVSystem == NAV_DTV_SYSTEM_DVBT_UK)
    {
        pu4ChTbl = au4DVBTUKChannel;
    }
    else if(u4DTVSystem == NAV_DTV_SYSTEM_DVBC) 
    {
        pu4ChTbl = au4DVBCChannel;
    }
#ifdef DRV_IF_BR
    else if (u4DTVSystem == NAV_DTV_SYSTEM_ISDBT)
    {
        pu4ChTbl = au4ISDBTChannel;
    }
#endif
    else
    {
        pu4ChTbl = au4DVBTTWChannel;
    }
#endif
    LOG(5, "\n---------- Scan Channel Report %04d Start ----------\n", u4Count);
#ifndef CC_NAV_DEMOD_DVBT
    LOG(5, "Total Programs=%-3d, Type=%s\n", u4NumDTVProg, (u4DTVSystem==NAV_DTV_SYSTEM_ATSC)?"ATSC Terrestrial":"ATSC Cable");
#else
#ifndef DRV_IF_BR
    LOG(5, "Total Programs=%-3d, Type=%s\n", u4NumDTVProg, (u4DTVSystem==NAV_DTV_SYSTEM_DVBT_UK)?"DVB-T UK":"DVB-T TW");
#else
    LOG(5, "Total Programs=%-3d, Type=%s\n", u4NumDTVProg, "ISDBT BR");
#endif
#endif
    for(i = 0; i < u4NumDTVProg; i++)
    {
        LOG(5, "Channel %2d.%-2d %-10s %4dMhz\n",
			arVctTbl[i].u2MajorChNum,
			arVctTbl[i].u2MinorChNum,
			arVctTbl[i].szShortName,
			pu4ChTbl[arVctTbl[i].u2PhyFreq]);
    }

    LOG(5, "---------- Scan Channel Report %04d End ----------\n\n", u4Count++);

}


static void _PrintATVProgInfo(void)
{
    static UINT32 u4Count = 1;
    UINT32 i;
    UINT32* pu4FreqTbl;
#ifdef CC_NAV_DEMOD_DVBT
    UCHAR* pucTvSys;
#endif
    if(u4ATVSystem == NAV_ATV_SYSTEM_AIR)
    {
        pu4FreqTbl = au4NTSCRFFreq;
    }
    else
    {
        pu4FreqTbl = au4NTSCCABLEFreq;
    }

    LOG(5, "\n---------- Scan Channel Report %04d Start ----------\n", u4Count);
#ifndef CC_NAV_DEMOD_DVBT    
    LOG(5, "Total Programs=%-3d, Type=%s\n", u4NumATVProg, (u4ATVSystem==NAV_ATV_SYSTEM_AIR)?"NTSC Air":"NTSC Cable");
#else
    switch(u4ATVSystem)
    {
        case(NAV_ATV_SYSTEM_PAL_BG):
            pucTvSys = "PAL_BG";    
            break;
        case(NAV_ATV_SYSTEM_PAL_DK):
            pucTvSys = "PAL_DK";    
            break;
        case(NAV_ATV_SYSTEM_PAL_I):
            pucTvSys = "PAL_I";    
            break;            
        case(NAV_ATV_SYSTEM_SECAM_L):
            pucTvSys = "SECAM_L";    
            break;
        case(NAV_ATV_SYSTEM_SECAM_L1):
            pucTvSys = "SECAM_L1";    
            break;       
        case(NAV_ATV_SYSTEM_NTSC_M):
            pucTvSys = "NTSC_M";    
            break;                   
    }
    LOG(5, "Total Programs=%-3d, Type=%s\n", u4NumATVProg, pucTvSys);
#endif
    for(i = 0; i < u4NumATVProg; i++)
    {
        LOG(5, "Channel %3d %3d.%2dMhz\n", arATVTbl[i].u2FreqTabIdx + 1, pu4FreqTbl[arATVTbl[i].u2FreqTabIdx]/1000, pu4FreqTbl[arATVTbl[i].u2FreqTabIdx]%1000/10);
    }

    LOG(5, "---------- Scan Channel Report %04d End ----------\n\n", u4Count++);

}

BOOL NAV_IsDTVScanning(void)
{
    return fgDTVChannelScanning;
}
BOOL NAV_IsATVScanning(void)
{
    return fgATVChannelScanning;
}

BOOL NAV_GetFreeze(UINT32 u4VdpId)
{
    if(u4VdpId > NAV_VDP_SLAVE)
    {
        return FALSE;
    }
    return _fgFreeze[u4VdpId];
}
INT32 NAV_SetFreeze(UINT32 u4VdpId, BOOL fgFreeze)
{
    if(u4VdpId > NAV_VDP_SLAVE)
    {
        return (INT32)E_NAV_MSG_NOT_SUPPORT;
    }
    if((_fgFreeze[u4VdpId] == fgFreeze))
    {
        VERIFY(DRV_VDO_Freeze(u4VdpId) == DRV_E_OK);
    }
    else
    {
        VERIFY(DRV_VDO_Resume(u4VdpId) == DRV_E_OK);
    }
    return (INT32)E_NAV_MSG_OK;
}

UINT32 NAV_GetNumATVProg(void)
{
	return u4NumATVProg;
}
UINT32 NAV_GetNumDTVProg(void)
{
	return u4NumDTVProg;
}

UINT32 NAV_GetATVProgNum(UINT32 u4ProgIdx)
{
    return (arATVTbl[u4ProgIdx].u2FreqTabIdx + 1);
}
#if 0
void NAV_SetCCStatistic(BOOL fgEnable)
{
	_fgCCStatistic = fgEnable;
}
void NAV_SetSCStatistic(BOOL fgEnable)
{
	_fgSCStatistic = fgEnable;
}
#endif
//-------------------------------------------------------------------------
/** NAV_SetPid
 *  Set video/audio/pcr PID for digital TV playback 
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
void NAV_SetPid(UINT32 u4VPid, UINT32 u4VPidType, UINT32 u4APid, UINT32 u4APidType, UINT32 u4PCRPid, UINT32 u4PhyCh, BOOL fgQam64)
{

    //NAV_LockAVMutex(u4VdpId);

    u4NumDTVProg = 0;
    arVctTbl[u4NumDTVProg].u2MajorChNum = u4PhyCh;
    arVctTbl[u4NumDTVProg].u2MinorChNum = 1;
    VERIFY(x_strcpy((CHAR*)(arVctTbl[u4NumDTVProg].szShortName), "No VCT\0") != NULL);

    arVctTbl[u4NumDTVProg].u2VPID = u4VPid;
    arVctTbl[u4NumDTVProg].u4VPIDType = u4VPidType;
    arVctTbl[u4NumDTVProg].u2APID1 = u4APid;
    arVctTbl[u4NumDTVProg].u2APID1Type = u4APidType;
    arVctTbl[u4NumDTVProg].u2APID2 = 0;
    arVctTbl[u4NumDTVProg].u2APID2Type = 0;
    arVctTbl[u4NumDTVProg].u2APID3 = 0;
    arVctTbl[u4NumDTVProg].u2APID3Type = 0;
    arVctTbl[u4NumDTVProg].u2PCR = u4PCRPid;
    arVctTbl[u4NumDTVProg].u2PhyFreq = u4PhyCh;
    arVctTbl[u4NumDTVProg].u2ChTsId = 0;

    _afgQam64[arVctTbl[u4NumDTVProg].u2PhyFreq] = fgQam64;
    
    u4NumDTVProg++;
    
    //NAV_UnlockAVMutex(u4VdpId);

}

//-------------------------------------------------------------------------
/** NAV_StopScan
 *  Break digital/analog channel scan thread
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_StopScan(void)
{
    _tScanCrit = x_crit_start();
    _fgBreakScan = TRUE;
    _TunerSetBreak();
    x_crit_end(_tScanCrit);
    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_LockAVMutex
 *  Lock A/V playback mutex
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
void NAV_LockAVMutex(UINT32 u4VdpId)
{
	if(u4VdpId == 0)
	{
		VERIFY(x_sema_lock(_hPlayAVSema, X_SEMA_OPTION_WAIT) == OSR_OK);
	}
	else
	{
		VERIFY(x_sema_lock(_hPlayAVSemaPIP, X_SEMA_OPTION_WAIT) == OSR_OK);
	}
}

//-------------------------------------------------------------------------
/** NAV_UnlockAVMutex
 *  Unlock A/V playback mutex
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
void NAV_UnlockAVMutex(UINT32 u4VdpId)
{
	if(u4VdpId == 0)
	{
		VERIFY(x_sema_unlock(_hPlayAVSema) == OSR_OK);
	}
	else
	{
		VERIFY(x_sema_unlock(_hPlayAVSemaPIP) == OSR_OK);
	}
}

//-------------------------------------------------------------------------
/** NAV_SwapSrc
 *  Swap main's and sub path's input sources
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SwapSrc(void)
{

    UINT32 u4TargetSubSrc = arVideoPath[NAV_VDP_MASTER].u4Src;
    UINT32 u4TargetMainSrc = arVideoPath[NAV_VDP_SLAVE].u4Src;
    VERIFY(_StopAV(NAV_VDP_MASTER, u4TargetSubSrc) == (INT32)E_NAV_MSG_OK);
    VERIFY(_StopAV(NAV_VDP_SLAVE, u4TargetMainSrc) == (INT32)E_NAV_MSG_OK);
    UNUSED(bApiVideoMainSubSrc(_au4SourceIdx2VS[u4TargetMainSrc], _au4SourceIdx2VS[u4TargetSubSrc]));
    while ((UINT32)prInputSrcList->prCurr->pvContent != u4TargetMainSrc)
    {
        prInputSrcList->prCurr = prInputSrcList->prCurr->prNext;
    }
    while ((UINT32)prInputSrcListPIP->prCurr->pvContent != u4TargetSubSrc)
    {
        prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
    }

    VERIFY(_PlayAV(NAV_VDP_MASTER, u4TargetMainSrc) != (INT32)E_NAV_MSG_ERROR);
    VERIFY(_PlayAV(NAV_VDP_SLAVE, u4TargetSubSrc) != (INT32)E_NAV_MSG_ERROR);

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_SwapSrcMode
 *  Swap main's and sub path's input sources, plus chnaging pictire mode between POP and PIP
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SwapSrcMode(void)
{

    UINT32 u4TargetSubSrc = arVideoPath[NAV_VDP_MASTER].u4Src;
    UINT32 u4TargetMainSrc = arVideoPath[NAV_VDP_SLAVE].u4Src;
    VERIFY(_StopAV(NAV_VDP_MASTER, u4TargetSubSrc) == (INT32)E_NAV_MSG_OK);
    VERIFY(_StopAV(NAV_VDP_SLAVE, u4TargetMainSrc) == (INT32)E_NAV_MSG_OK);
    UNUSED(bApiVideoMainSubSrc(_au4SourceIdx2VS[u4TargetMainSrc], _au4SourceIdx2VS[u4TargetSubSrc]));
    while ((UINT32)prInputSrcList->prCurr->pvContent != u4TargetMainSrc)
    {
        prInputSrcList->prCurr = prInputSrcList->prCurr->prNext;
    }
    while ((UINT32)prInputSrcListPIP->prCurr->pvContent != u4TargetSubSrc)
    {
        prInputSrcListPIP->prCurr = prInputSrcListPIP->prCurr->prNext;
    }

    if(ePIPMode == E_NAV_MODE_PIP)
    {
        arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width = VDP_MAX_REGION_WIDTH >> 1;
        arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height = VDP_MAX_REGION_HEIGHT;
        arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = VDP_MAX_REGION_WIDTH >> 1;
        ePIPMode = E_NAV_MODE_POP;
    }
    else if(ePIPMode == E_NAV_MODE_POP)
    {
        arVideoPath[NAV_VDP_MASTER].rVDPRegion.u4Width = VDP_MAX_REGION_WIDTH;
        arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Height = VDP_MAX_REGION_HEIGHT >> 1;
        arVideoPath[NAV_VDP_SLAVE].rVDPRegion.u4Left = 0;
        ePIPMode = E_NAV_MODE_PIP;
    }

    VERIFY(_PlayAV(NAV_VDP_MASTER, u4TargetMainSrc) != (INT32)E_NAV_MSG_ERROR);
    VERIFY(_PlayAV(NAV_VDP_SLAVE, u4TargetSubSrc) != (INT32)E_NAV_MSG_ERROR);

    VERIFY(NAV_DrawInfoOSD() == (INT32)E_NAV_MSG_OK);
    VERIFY(WIDGET_DrawMenuList(arWidgetOSD[0].prList, &arWidgetOSD[0]) == (INT32)E_WIDGET_MSG_OK);

    return (INT32)E_NAV_MSG_OK;
}

//-------------------------------------------------------------------------
/** NAV_OpenNim
 *  Open the NIM module
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
void NAV_OpenNIM(void)
{    
   _u4EnableNIM = 1;
   /*
   if(DRV_TUNER_Init() == 0)
   {
       _u4EnableNIM = 1;
   }
   else
   {
       _u4EnableNIM = 0;
   }
   */
}

//-------------------------------------------------------------------------
/** NAV_SetScart
 *  Wrapper for setting SCART input mode
 *  @param  
 *  @retval  NONE
 */
//-------------------------------------------------------------------------
INT32 NAV_SetScart(void* pvArg)
{
    CHAR* szMode = (CHAR*)pvArg;
    if(x_strcmp("CVBS", szMode) == 0)
    {
          return _SetScart(SV_SCART_CVBS);
    }
    else if(x_strcmp("RGB", szMode) == 0)
    {
          return _SetScart(SV_SCART_RGB);
    }
    else if(x_strcmp("SV", szMode) == 0)
    {
          return _SetScart(SV_SCART_SV);
    }
    else if(x_strcmp("CVBS+RGB", szMode) == 0)
    {
          return _SetScart(SV_SCART_CVBS_RGB);
    }
    else if(x_strcmp("AUTO", szMode) == 0)
    {
          return _SetScart(SV_SCART_AUTO);
    }
    else
    {
        return E_NAV_MSG_NOT_SUPPORT;
    }
}


WIDGET_FONT_PROPERTY_T* NAV_GetDefaultFont(void)
{
    return &rInfoOSDFont;
}

void NAV_InitBorder(void)
{
    _InitBorder();
}

void NAV_ReleaseBorder(void)
{
    _ReleaseBorder();
}


